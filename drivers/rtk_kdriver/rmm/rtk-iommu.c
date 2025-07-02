// SPDX-License-Identifier: GPL-2.0-only
/*
 * IOMMU API for RTK architected implementations
 *
 * Copyright (c) 2023 RealTek Inc.
 * Author: davidwang <davidwang@realtek.com>
 *
 */

#define pr_fmt(fmt)	"rtk_iommu: " fmt

#include <linux/version.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/dma-iommu.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
#include <linux/dma-map-ops.h>
#else
#include <linux/dma-mapping.h>
#endif
#include <linux/errno.h>
#include <linux/iommu.h>
#include <linux/iova.h>
#include <linux/io-pgtable.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/scatterlist.h>
#include <linux/vmalloc.h>

#include <clocksource/arm_arch_timer.h>

#include <rtd_log/rtd_module_log.h>
#include <rmm/rtk-iommu.h>
#include <rmm/pageremap.h>
#include <rmm/optee_rt_mmu_pgtable.h>

#include <mach/rtk_platform.h>
#include <mach/system.h>

/* Struct accessors */
#define rtk_io_pgtable_to_data(x)						\
	container_of((x), struct rtk_io_pgtable, iop)

#define rtk_io_pgtable_ops_to_data(x)					\
	rtk_io_pgtable_to_data(io_pgtable_ops_to_pgtable(x))

struct rtk_iommu_dma_cookie {
	struct iova_domain iovad;
	unsigned int has_vbm_iova;
	unsigned long vbm_iova;
	unsigned long vbm_contig_phy;
	unsigned long vbm_size;
};

struct rtk_iommu_domain {
	struct io_pgtable_cfg		cfg;
	struct io_pgtable_ops		*iop;

	struct list_head iommus;
	spinlock_t iommus_lock; /* lock for iommus list */
	struct iommu_domain domain;

	struct rtk_iommu_dma_cookie iova_cookie;
};

#define RTK_PTE_PAGE_ADDRESS_MASK  0xfffff000
#define RTK_PTE_PAGE_FLAGS_MASK    0x000001fe
#define RTK_PTE_PAGE_VALID         BIT(0)

static inline phys_addr_t rtk_pte_page_address(unsigned int pte)
{
	return (phys_addr_t)pte & RTK_PTE_PAGE_ADDRESS_MASK;
}

static inline bool rtk_pte_is_page_valid(unsigned int pte)
{
	return pte & RTK_PTE_PAGE_VALID;
}

static unsigned int rtk_mk_pte(phys_addr_t page, int prot)
{
//	unsigned int flags = 0;
	page &= RTK_PTE_PAGE_ADDRESS_MASK;
//	return page | flags | RTK_PTE_PAGE_VALID;
	return page;
}

static unsigned int rtk_mk_pte_invalid(unsigned int pte)
{
//	return pte & ~RTK_PTE_PAGE_VALID;
//	return pte | 0xfffff000;
	return 0xdead1000;
}

#if !defined(CONFIG_ARCH_RTK2819A)
#define RTK_IOVA_PTE_MASK    0x07fff000
#else
#define RTK_IOVA_PTE_MASK    0x0ffff000
#endif
#define RTK_IOVA_PTE_SHIFT   12
#define RTK_IOVA_PAGE_MASK   0x00000fff
#define RTK_IOVA_PAGE_SHIFT  0

static unsigned int rtk_iova_pte_index(dma_addr_t iova)
{
	return (unsigned int)(iova & RTK_IOVA_PTE_MASK) >> RTK_IOVA_PTE_SHIFT;
}

static unsigned int rtk_iova_page_offset(dma_addr_t iova)
{
	return (unsigned int)(iova & RTK_IOVA_PAGE_MASK) >> RTK_IOVA_PAGE_SHIFT;
}

struct rtk_io_pgtable {
	struct io_pgtable	iop;
	unsigned int	*pt;
	dma_addr_t pt_dma;
	spinlock_t pt_lock; /* lock for modifying page table */
	spinlock_t tlb_lock; /* lock for tlb range flush */
};

enum {
	ALLOC_PAGE = 0,
	CPU_REMAP_PAGE,
	ALLOC_IOVA,
	MAP_IOVA,
	FLUSH_IOVA,

	NUM_RTK_IOMMU_PERF_TIME_STAGE
} rtk_iommu_perf_time_stage;

const char * rtk_iommu_perf_time_stage_text[] = {
	"alloc_page",
	"cpu_remap_page",
	"alloc_iova",
	"map_iova",
	"flush_iova",
};
	
struct rtk_iommu_perf_time {
	unsigned long alloc_page_start;
	unsigned long alloc_page_end;
	unsigned long cpu_remap_page_start;
	unsigned long cpu_remap_page_end;
	unsigned long alloc_iova_start;
	unsigned long alloc_iova_end;
	unsigned long map_iova_start;
	unsigned long map_iova_end;
	unsigned long flush_iova_start;
	unsigned long flush_iova_end;
};
struct rtk_iommu_perf_time rtk_iommu_perf_time[1] = {0};

unsigned int page_table[NUM_PT_ENTRIES] __attribute__((aligned(4096)));

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
static int __rtk_iommu_map(struct io_pgtable_ops *ops, unsigned long _iova,
						   phys_addr_t paddr, size_t size, int prot, gfp_t gfp);
#else
static int __rtk_iommu_map(struct io_pgtable_ops *ops, unsigned long _iova,
						   phys_addr_t paddr, size_t size, int prot);
#endif
static size_t __rtk_iommu_unmap(struct io_pgtable_ops *ops, unsigned long _iova,
								size_t size, struct iommu_iotlb_gather *gather);
static phys_addr_t __rtk_iommu_iova_to_phys(struct io_pgtable_ops *ops,
											unsigned long iova);

static unsigned int __maybe_unused swap_endian(unsigned int input)
{
	unsigned int output;

	output = (input & 0xff000000) >> 24 |
			(input & 0x00ff0000) >> 8 |
			(input & 0x0000ff00) << 8 |
			(input & 0x000000ff) << 24;

	return output;
}

static void rtk_iommu_perf_time_start(unsigned int stage)
{
	switch (stage) {
	case ALLOC_PAGE:
		rtk_iommu_perf_time->alloc_page_start = arch_timer_read_counter();
		break;
	case CPU_REMAP_PAGE:
		rtk_iommu_perf_time->cpu_remap_page_start = arch_timer_read_counter();
		break;
	case ALLOC_IOVA:
		rtk_iommu_perf_time->alloc_iova_start = arch_timer_read_counter();
		break;
	case MAP_IOVA:
		rtk_iommu_perf_time->map_iova_start = arch_timer_read_counter();
		break;
	case FLUSH_IOVA:
		rtk_iommu_perf_time->flush_iova_start = arch_timer_read_counter();
		break;
	default:
		break;
	}
}

static void rtk_iommu_perf_time_end(unsigned int stage)
{
	switch (stage) {
	case ALLOC_PAGE:
		rtk_iommu_perf_time->alloc_page_end = arch_timer_read_counter();
		break;
	case CPU_REMAP_PAGE:
		rtk_iommu_perf_time->cpu_remap_page_end = arch_timer_read_counter();
		break;
	case ALLOC_IOVA:
		rtk_iommu_perf_time->alloc_iova_end = arch_timer_read_counter();
		break;
	case MAP_IOVA:
		rtk_iommu_perf_time->map_iova_end = arch_timer_read_counter();
		break;
	case FLUSH_IOVA:
		rtk_iommu_perf_time->flush_iova_end = arch_timer_read_counter();
		break;
	default:
		break;
	}
}

static void rtk_iommu_perf_time_clear(void)
{
	memset(rtk_iommu_perf_time, 0, sizeof(struct rtk_iommu_perf_time));
}

static unsigned long rtk_iommu_perf_time_duration_get(unsigned int stage)
{
	unsigned long duration = 0;
	
	switch (stage) {
	case ALLOC_PAGE:
		duration = rtk_iommu_perf_time->alloc_page_end - rtk_iommu_perf_time->alloc_page_start;
		break;
	case CPU_REMAP_PAGE:
		duration = rtk_iommu_perf_time->cpu_remap_page_end - rtk_iommu_perf_time->cpu_remap_page_start;
		break;
	case ALLOC_IOVA:
		duration = rtk_iommu_perf_time->alloc_iova_end - rtk_iommu_perf_time->alloc_iova_start;
		break;
	case MAP_IOVA:
		duration = rtk_iommu_perf_time->map_iova_end - rtk_iommu_perf_time->map_iova_start;
		break;
	case FLUSH_IOVA:
		duration = rtk_iommu_perf_time->flush_iova_end - rtk_iommu_perf_time->flush_iova_start;
		break;
	default:
		break;
	}

	return duration;
}

static void rtk_iommu_perf_time_print(void)
{
	int i = 0;

	rtd_pr_rmm_info("time duration: ");
	for (i = 0; i < NUM_RTK_IOMMU_PERF_TIME_STAGE; i++)
		rtd_pr_rmm_info("%s: %08ld us ", rtk_iommu_perf_time_stage_text[i], (rtk_iommu_perf_time_duration_get(i) * TIMER_NS_PER_TICK)/1000);

	rtd_pr_rmm_info("\n");
}

static struct rtk_iommu_domain *to_rtk_domain(struct iommu_domain *dom)
{
	return container_of(dom, struct rtk_iommu_domain, domain);
}

extern struct page *alloc_specific_contiguous_memory(size_t size, unsigned long addr, int flags, const void *caller);
extern struct page *alloc_contiguous_memory(size_t size, int flags, const void *caller);
extern int free_contiguous_memory(struct page *page, int count);

extern unsigned int set_entry_rbus; // default dma mode

static void rtk_iommu_tlb_flush_all(struct rtk_io_pgtable *data)
{
//	unsigned long flags;
	unsigned int *pgtable;
	int ret = 0;
	
//	spin_lock_irqsave(&data->tlb_lock, flags);
	rtk_iommu_perf_time_start(FLUSH_IOVA);
	
	pgtable = rtk_iommu_iova_table_offset(RTK_IOVA_PHY_BASE);
	ret = rt_mmu_pgtable_set_entries(RTK_IOVA_PHY_BASE, RTK_IOVA_PHY_LIMIT - RTK_IOVA_PHY_BASE, (set_entry_rbus ? 0 : 1), pgtable);

	rtk_iommu_perf_time_end(FLUSH_IOVA);
//	spin_unlock_irqrestore(&data->tlb_lock, flags);
}

static void rtk_iommu_tlb_flush_range_sync(unsigned long iova, size_t size,
					   size_t granule,
					   struct rtk_io_pgtable *data)
{
//	unsigned long flags;
	unsigned int *pgtable;
	int ret = 0;

//	spin_lock_irqsave(&data->tlb_lock, flags);
	rtk_iommu_perf_time_start(FLUSH_IOVA);
	
	pgtable = rtk_iommu_iova_table_offset(iova);
	ret = rt_mmu_pgtable_set_entries(iova, size, (set_entry_rbus ? 0 : 1), pgtable);

	rtk_iommu_perf_time_end(FLUSH_IOVA);
//	spin_unlock_irqrestore(&data->tlb_lock, flags);
}

static size_t rtk_iommu_unmap_iova(struct rtk_io_pgtable *data,
				  unsigned int *pte_addr, dma_addr_t pte_dma,
				  size_t size)
{
	unsigned int pte_count;
	unsigned int pte_total = size / SPAGE_SIZE;

	assert_spin_locked(&data->pt_lock);

	for (pte_count = 0; pte_count < pte_total; pte_count++) {
		unsigned int pte = pte_addr[pte_count];
//		if (!rtk_pte_is_page_valid(pte))
//			break;

#ifdef DMA_TABLE_ENDIAN_BIG
		pte_addr[pte_count] = swap_endian(rtk_mk_pte_invalid(pte));
#else
		pte_addr[pte_count] = rtk_mk_pte_invalid(pte);
#endif

		rtd_pr_rmm_debug("%s: pte_addr=%lx, pte_dma=%08x, pte=%08x\n", __func__, 
				(unsigned long)pte_addr, (unsigned int)pte_dma, (unsigned int)pte_addr[pte_count]);
	}

	return pte_count * SPAGE_SIZE;
}

static int rtk_iommu_map_iova(struct rtk_io_pgtable *data, unsigned int *pte_addr,
			     dma_addr_t pte_dma, dma_addr_t iova,
			     phys_addr_t paddr, size_t size, int prot)
{
	unsigned int pte_count;
	unsigned int pte_total = size / SPAGE_SIZE;
	phys_addr_t page_phys;

	assert_spin_locked(&data->pt_lock);

	for (pte_count = 0; pte_count < pte_total; pte_count++) {
		unsigned int pte = pte_addr[pte_count];

		if (rtk_pte_is_page_valid(pte))
			goto unwind;

#ifdef DMA_TABLE_ENDIAN_BIG
		pte_addr[pte_count] = swap_endian(rtk_mk_pte(paddr, prot));
#else
		pte_addr[pte_count] = rtk_mk_pte(paddr, prot);
//		rtd_pr_rmm_info("*(unsigned int *)0x%08x = 0x%08x\n", (unsigned int)pte_dma, rtk_mk_pte(paddr, prot));
#endif

		rtd_pr_rmm_debug("%s: pte_index %06d, iova=%08x, pte_addr=%lx, pte_dma=%08x, pte=%08x\n", __func__, 
				(unsigned int)rtk_iova_pte_index(iova),
				(unsigned int)iova, (unsigned long)pte_addr, (unsigned int)pte_dma, (unsigned int)pte_addr[pte_count]);

		paddr += SPAGE_SIZE;
	}

	return 0;
unwind:
	/* Unmap the range of iovas that we just mapped */
	rtk_iommu_unmap_iova(data, pte_addr, pte_dma,
			    pte_count * SPAGE_SIZE);

	iova += pte_count * SPAGE_SIZE;
	page_phys = rtk_pte_page_address(pte_addr[pte_count]);
	rtd_pr_rmm_err("iova: %pad already mapped to %pa cannot remap to phys: %pa prot: %#x\n",
	       &iova, &page_phys, &paddr, prot);

	return -EADDRINUSE;
}

static int rtk_iommu_map(struct iommu_domain *domain, unsigned long _iova,
			phys_addr_t paddr, size_t size, int prot, gfp_t gfp)
{
	struct rtk_iommu_domain *rtk_domain = to_rtk_domain(domain);
	struct rtk_io_pgtable *data = rtk_io_pgtable_ops_to_data(rtk_domain->iop);
	unsigned long flags;
	dma_addr_t pte_dma, iova = (dma_addr_t)_iova;
	unsigned int *pte_addr;
	unsigned int pte_index;
	int ret;

	spin_lock_irqsave(&data->pt_lock, flags);

	pte_index = rtk_iova_pte_index(iova);
	pte_addr = &data->pt[pte_index]; // virtual
	pte_dma = data->pt_dma + pte_index * sizeof(unsigned int); // physical
	rtd_pr_rmm_debug("%s: pte_addr[%06d]=%lx, pgtable=%lx/%lx, pte_dma(%lx/%lx)\n", __func__, pte_index, (unsigned long)pte_addr, 
			(unsigned long)page_table, (unsigned long)&data->pt[pte_index], (unsigned long)pte_dma, (unsigned long)data->pt_dma);
	ret = rtk_iommu_map_iova(data, pte_addr, pte_dma, iova,
				paddr, size, prot);

	spin_unlock_irqrestore(&data->pt_lock, flags);

	return ret;
}

static size_t rtk_iommu_unmap(struct iommu_domain *domain, unsigned long _iova,
			     size_t size, struct iommu_iotlb_gather *gather)
{
	struct rtk_iommu_domain *rtk_domain = to_rtk_domain(domain);
	struct rtk_io_pgtable *data = rtk_io_pgtable_ops_to_data(rtk_domain->iop);
	unsigned long flags;
	dma_addr_t pte_dma, iova = (dma_addr_t)_iova;
	phys_addr_t pt_phys;
	unsigned int *pte_addr;
	unsigned int pte_index;
	size_t unmap_size;

	spin_lock_irqsave(&data->pt_lock, flags);

	pt_phys = (phys_addr_t)data->pt_dma;
	pte_index = rtk_iova_pte_index(iova);
	pte_addr = (unsigned int *)phys_to_virt(pt_phys) + pte_index;
	pte_dma = pt_phys + pte_index * sizeof(unsigned int);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
	iommu_iotlb_gather_add_range(gather, (unsigned long)iova, size);
#else
	iommu_iotlb_gather_add_page(domain, gather, (unsigned long)iova, size);
#endif
	rtd_pr_rmm_debug("%s: pte_addr[%06d]=%lx, pgtable=%lx/%lx, pte_dma(%lx/%lx)\n", __func__, pte_index, (unsigned long)pte_addr, 
			(unsigned long)page_table, (unsigned long)&data->pt[pte_index], (unsigned long)pte_dma, (unsigned long)data->pt_dma);
	unmap_size = rtk_iommu_unmap_iova(data, pte_addr, pte_dma, size);

	spin_unlock_irqrestore(&data->pt_lock, flags);

	return unmap_size;
}

static void rtk_iommu_flush_iotlb_all(struct iommu_domain *domain)
{
	struct rtk_iommu_domain *rtk_domain = to_rtk_domain(domain);
	struct rtk_io_pgtable *data = rtk_io_pgtable_ops_to_data(rtk_domain->iop);

	rtk_iommu_tlb_flush_all(data);
}

static void rtk_iommu_iotlb_sync(struct iommu_domain *domain,
				 struct iommu_iotlb_gather *gather)
{
	struct rtk_iommu_domain *rtk_domain = to_rtk_domain(domain);
	struct rtk_io_pgtable *data = rtk_io_pgtable_ops_to_data(rtk_domain->iop);
	size_t length = gather->end - gather->start + 1;

	rtk_iommu_tlb_flush_range_sync(gather->start, length, gather->pgsize,
				       data);
}

static void rtk_iommu_sync_map(struct iommu_domain *domain, unsigned long iova,
			       size_t size)
{
	struct rtk_iommu_domain *rtk_domain = to_rtk_domain(domain);
	struct rtk_io_pgtable *data = rtk_io_pgtable_ops_to_data(rtk_domain->iop);

	rtk_iommu_tlb_flush_range_sync(iova, size, size, data);
}

static phys_addr_t rtk_iommu_iova_to_phys(struct iommu_domain *domain,
					dma_addr_t iova)
{
	struct rtk_iommu_domain *rtk_domain = to_rtk_domain(domain);
	struct rtk_io_pgtable *data = rtk_io_pgtable_ops_to_data(rtk_domain->iop);
	unsigned long flags;
	phys_addr_t pt_phys, phys = 0;
	unsigned int pte;
	unsigned int *page_table;

	spin_lock_irqsave(&data->pt_lock, flags);

	pt_phys = (phys_addr_t)data->pt_dma;
	page_table = (unsigned int *)phys_to_virt(pt_phys);
	pte = page_table[rtk_iova_pte_index(iova)];
	if (!rtk_pte_is_page_valid(pte))
		goto out;

	phys = rtk_pte_page_address(pte) + rtk_iova_page_offset(iova);
out:
	spin_unlock_irqrestore(&data->pt_lock, flags);

	return phys;
}

static int rtk_iommu_domain_finalise(struct rtk_iommu_domain *rtk_domain)
{
	rtk_domain->cfg = (struct io_pgtable_cfg) {
		.oas = 32,
		.ias = 32,
		.pgsize_bitmap = rtk_iommu_ops.pgsize_bitmap,
	};

	rtk_domain->iop = rtk_iommu_alloc_io_pgtable_ops(RTK_IOMMU_IO_PGTABLE_V1, &rtk_domain->cfg, &rtk_domain->cfg);
	if (!rtk_domain->iop) {
		rtd_pr_rmm_err("Failed to alloc io pgtable\n");
		return -EINVAL;
	}

	/* Update our support page sizes bitmap */
	rtk_domain->domain.pgsize_bitmap = rtk_domain->cfg.pgsize_bitmap;
	return 0;
}

static struct iommu_domain *rtk_iommu_domain_alloc(unsigned type)
{
	struct iommu_domain *domain;
	struct rtk_iommu_domain *rtk_domain;
	struct rtk_iommu_dma_cookie *cookie;
	unsigned long base = RTK_IOVA_PHY_BASE;
	unsigned long order, base_pfn;


	if (type != IOMMU_DOMAIN_DMA)
		return NULL;

	rtk_domain = kzalloc(sizeof(*rtk_domain), GFP_KERNEL);
	if (!rtk_domain)
		return NULL;
	memset(rtk_domain, 0, sizeof(*rtk_domain));

	if (iommu_get_dma_cookie(&rtk_domain->domain))
		goto free_dom;

	if (rtk_iommu_domain_finalise(rtk_domain))
		goto put_dma_cookie;

	domain = &rtk_domain->domain;

	/* Use the smallest supported page size for IOVA granularity */
	order = __ffs(domain->pgsize_bitmap);
	base_pfn = max_t(unsigned long, 1, base >> order);

	cookie = &rtk_domain->iova_cookie;
	init_iova_domain(&cookie->iovad, 1UL << order, base_pfn);

	domain->ops = &rtk_iommu_ops;
	domain->type = type;
	/* Assume all sizes by default; the driver may override this later */
	domain->pgsize_bitmap  = domain->ops->pgsize_bitmap;

	rtk_domain->domain.geometry.aperture_start = 0;
	rtk_domain->domain.geometry.aperture_end = DMA_BIT_MASK(32);
	rtk_domain->domain.geometry.force_aperture = true;

	return &rtk_domain->domain;

put_dma_cookie:
	iommu_put_dma_cookie(&rtk_domain->domain);
free_dom:
	kfree(rtk_domain);
	return NULL;
}

static void __maybe_unused rtk_iommu_domain_free(struct iommu_domain *domain)
{
	struct rtk_iommu_domain *rtk_domain = to_rtk_domain(domain);
	struct rtk_iommu_dma_cookie *cookie = &rtk_domain->iova_cookie;
	struct iova_domain *iovad = &cookie->iovad;

	put_iova_domain(iovad);
	rtk_iommu_free_io_pgtable_ops(rtk_domain->iop);
	iommu_put_dma_cookie(domain);
	kfree(to_rtk_domain(domain));
}

static struct io_pgtable *rtk_iommu_alloc_pgtable(struct io_pgtable_cfg *cfg,
						void *cookie)
{
	struct rtk_io_pgtable *data;

	data = kmalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return NULL;

	data->pt = page_table;
	data->pt_dma = virt_to_phys(page_table);

	rtd_pr_rmm_info("%s: pt=%lx, pt_dma=%08x\n", __func__, (unsigned long)data->pt, (unsigned int)data->pt_dma);

	data->iop.ops = (struct io_pgtable_ops) {
		.map		= __rtk_iommu_map,
		.unmap		= __rtk_iommu_unmap,
		.iova_to_phys	= __rtk_iommu_iova_to_phys,
	};

	data->iop.cfg = *cfg;
	spin_lock_init(&data->pt_lock);
	spin_lock_init(&data->tlb_lock);

	return &data->iop;
}

static void __maybe_unused rtk_iommu_free_pgtable(struct io_pgtable *iop)
{
	struct rtk_io_pgtable *data = rtk_io_pgtable_to_data(iop);

	data->pt = NULL;
	data->pt_dma = 0;

	kfree(data);
}

struct io_pgtable_init_fns rtk_iommu_io_pgtable_init_fns = {
	.alloc	= rtk_iommu_alloc_pgtable,
	.free	= rtk_iommu_free_pgtable,
};

static const struct io_pgtable_init_fns *
rtk_iommu_io_pgtable_init_table[RTK_IOMMU_IO_PGTABLE_NUM_FMTS] = {
	[RTK_IOMMU_IO_PGTABLE_V1] = &rtk_iommu_io_pgtable_init_fns,
};

struct io_pgtable_ops *rtk_iommu_alloc_io_pgtable_ops(enum rtk_iommu_io_pgtable_fmt fmt,
					    struct io_pgtable_cfg *cfg,
					    void *cookie)
{
	struct io_pgtable *iop;
	const struct io_pgtable_init_fns *fns;

	if (fmt >= RTK_IOMMU_IO_PGTABLE_NUM_FMTS)
		return NULL;

	fns = rtk_iommu_io_pgtable_init_table[fmt];
	if (!fns)
		return NULL;

	iop = fns->alloc(cfg, cookie);
	if (!iop)
		return NULL;

	iop->fmt	= (enum io_pgtable_fmt)fmt;
	iop->cookie	= cookie;
	iop->cfg	= *cfg;

	return &iop->ops;
}

void rtk_iommu_free_io_pgtable_ops(struct io_pgtable_ops *ops)
{
	struct io_pgtable *iop;
	const struct io_pgtable_init_fns *fns;
	enum rtk_iommu_io_pgtable_fmt fmt;

	if (!ops)
		return;

	iop = io_pgtable_ops_to_pgtable(ops);
	io_pgtable_tlb_flush_all(iop);

	fmt = (enum rtk_iommu_io_pgtable_fmt)iop->fmt;
	fns = rtk_iommu_io_pgtable_init_table[fmt];
	fns->free(iop);
}

static dma_addr_t rtk_iommu_dma_alloc_iova(struct iommu_domain *domain,
		size_t size, dma_addr_t dma_limit, struct device *dev)
{
	struct rtk_iommu_domain *rtk_domain = to_rtk_domain(domain);
	struct rtk_iommu_dma_cookie *cookie = &rtk_domain->iova_cookie;
	struct iova_domain *iovad = &cookie->iovad;
	unsigned long shift, iova_len, iova = 0;

	if (cookie->has_vbm_iova)
		return (dma_addr_t)cookie->vbm_iova;

	shift = iova_shift(iovad);
	iova_len = size >> shift;
	rtd_pr_rmm_info("%s: size=%lx, shift=%lx, iova_len=%lx\n", __func__, (unsigned long)size, shift, iova_len);
	/*
	 * Freeing non-power-of-two-sized allocations back into the IOVA caches
	 * will come back to bite us badly, so we have to waste a bit of space
	 * rounding up anything cacheable to make sure that can't happen. The
	 * order of the unadjusted size will still match upon freeing.
	 */
	if (iova_len < (1 << (IOVA_RANGE_CACHE_MAX_SIZE - 1)))
		iova_len = roundup_pow_of_two(iova_len);

	rtd_pr_rmm_info("%s: iova_len=%lx\n", __func__, iova_len);

	if (domain->geometry.force_aperture)
		dma_limit = min(dma_limit, domain->geometry.aperture_end);

	rtd_pr_rmm_info("%s: dma_limit=%08x\n", __func__, (unsigned int)dma_limit);

	if (!iova) {
		rtd_pr_rmm_info("%s: iovad, start_pfn=%lx, dma_32bit_pfn=%lx, granule=%lx\n", __func__,
				iovad->start_pfn, iovad->dma_32bit_pfn, iovad->granule);
		iova = alloc_iova_fast(iovad, iova_len, dma_limit >> shift,
				       true);
	}

	return (dma_addr_t)iova << shift;
}

static void rtk_iommu_dma_free_iova(struct rtk_iommu_dma_cookie *cookie,
		dma_addr_t iova, size_t size)
{
	struct iova_domain *iovad = &cookie->iovad;

	if (cookie->has_vbm_iova)
		return;
	
	free_iova_fast(iovad, iova_pfn(iovad, iova),
				size >> iova_shift(iovad));
}

static void __rtk_iommu_dma_unmap(struct iommu_domain *domain, dma_addr_t dma_addr,
		size_t size)
{
	struct rtk_iommu_domain *rtk_domain = to_rtk_domain(domain);
	struct rtk_iommu_dma_cookie *cookie = &rtk_domain->iova_cookie;
	struct iova_domain *iovad = &cookie->iovad;
	size_t iova_off = iova_offset(iovad, dma_addr);
	struct iommu_iotlb_gather iotlb_gather;
	size_t unmapped;

	dma_addr -= iova_off;
	size = iova_align(iovad, size + iova_off);
	iommu_iotlb_gather_init(&iotlb_gather);

	unmapped = iommu_unmap_fast(domain, dma_addr, size, &iotlb_gather);
	WARN_ON(unmapped != size);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
	iommu_iotlb_sync(domain, &iotlb_gather);
#else
	iommu_tlb_sync(domain, &iotlb_gather);
#endif
	rtk_iommu_dma_free_iova(cookie, dma_addr, size);
}

static void __rtk_iommu_dma_free_pages(struct page **pages, int count)
{
	while (count--)
		__free_page(pages[count]);
	kvfree(pages);
}

static struct page **__rtk_iommu_dma_alloc_pages(struct device *dev,
		unsigned int count, unsigned long order_mask, gfp_t gfp)
{
	struct page **pages;
	unsigned int i = 0, nid = dev_to_node(dev);

	order_mask &= (2U << MAX_ORDER) - 1;
	if (!order_mask)
		return NULL;

	pages = kvzalloc(count * sizeof(*pages), GFP_KERNEL);
	if (!pages)
		return NULL;

	/* IOMMU can map any pages, so himem can also be used here */
	gfp |= __GFP_NOWARN | __GFP_HIGHMEM;

	while (count) {
		struct page *page = NULL;
		unsigned int order_size;

		/*
		 * Higher-order allocations are a convenience rather
		 * than a necessity, hence using __GFP_NORETRY until
		 * falling back to minimum-order allocations.
		 */
		for (order_mask &= (2U << __fls(count)) - 1;
		     order_mask; order_mask &= ~order_size) {
			unsigned int order = __fls(order_mask);
			gfp_t alloc_flags = gfp;

			order_size = 1U << order;
			if (order_mask > order_size)
				alloc_flags |= __GFP_NORETRY;
			page = alloc_pages_node(nid, alloc_flags, order);
			if (!page)
				continue;
			if (!order)
				break;
			if (!PageCompound(page)) {
				split_page(page, order);
				break;
			} else if (!split_huge_page(page)) {
				break;
			}
			__free_pages(page, order);
		}
		if (!page) {
			__rtk_iommu_dma_free_pages(pages, i);
			return NULL;
		}
		count -= order_size;
		while (order_size--)
			pages[i++] = page++;
	}
	return pages;
}

static void __rtk_iommu_dma_free(struct device *dev, size_t size, void *cpu_addr)
{
	size_t alloc_size = PAGE_ALIGN(size);
	int count = alloc_size >> PAGE_SHIFT;
	struct page *page = NULL, **pages = NULL;

	if (IS_ENABLED(CONFIG_DMA_REMAP) && is_vmalloc_addr(cpu_addr)) {
		/*
		 * If it the address is remapped, then it's either non-coherent
		 * or highmem CMA, or an iommu_dma_alloc_remap() construction.
		 */
		pages = dma_common_find_pages(cpu_addr);
		if (!pages)
			page = vmalloc_to_page(cpu_addr);
		dma_common_free_remap(cpu_addr, alloc_size);
	} else {
		/* Lowmem means a coherent atomic or CMA allocation */
		page = virt_to_page(cpu_addr);
	}

	if (pages)
		__rtk_iommu_dma_free_pages(pages, count);

	if (page)
		free_contiguous_memory(page, alloc_size >> PAGE_SHIFT);
}

static struct page **__rtk_iommu_dma_alloc_noncontiguous(struct device *dev, struct iommu_domain *domain,
		size_t size, struct sg_table *sgt, gfp_t gfp, pgprot_t prot,
		unsigned long attrs)
{
	struct rtk_iommu_domain *rtk_domain = to_rtk_domain(domain);
	struct rtk_iommu_dma_cookie *cookie = &rtk_domain->iova_cookie;
	struct iova_domain *iovad = &cookie->iovad;
	int ioprot = 0; //IOMMU_READ | IOMMU_WRITE
	unsigned int count, min_size, alloc_sizes = domain->pgsize_bitmap;
	struct page **pages;
	dma_addr_t iova;
	ssize_t ret;

	min_size = alloc_sizes & -alloc_sizes;
	if (min_size < PAGE_SIZE) {
		min_size = PAGE_SIZE;
		alloc_sizes |= PAGE_SIZE;
	} else {
		size = ALIGN(size, min_size);
	}
	if (attrs & DMA_ATTR_ALLOC_SINGLE_PAGES)
		alloc_sizes = min_size;

	count = PAGE_ALIGN(size) >> PAGE_SHIFT;

	rtk_iommu_perf_time_start(ALLOC_PAGE);
	pages = __rtk_iommu_dma_alloc_pages(dev, count, alloc_sizes >> PAGE_SHIFT,
					gfp);
	if (!pages) {
		rtk_iommu_perf_time_end(ALLOC_PAGE);
		return NULL;
	}
	rtk_iommu_perf_time_end(ALLOC_PAGE);
	
	size = iova_align(iovad, size);
	rtd_pr_rmm_info("%s, size=%lx\n", __func__, size);
	rtk_iommu_perf_time_start(ALLOC_IOVA);
	iova = rtk_iommu_dma_alloc_iova(domain, size, RTK_IOVA_PHY_LIMIT, NULL);
	if (!iova) {
		rtk_iommu_perf_time_end(ALLOC_IOVA);
		goto out_free_pages;
	}
	rtk_iommu_perf_time_end(ALLOC_IOVA);
	rtd_pr_rmm_info("%s, iova=%lx\n", __func__, (unsigned long)iova);

	if (sg_alloc_table_from_pages(sgt, pages, count, 0, size, GFP_KERNEL))
		goto out_free_iova;
#if 0
	if (!(ioprot & IOMMU_CACHE)) {
		struct scatterlist *sg;
		int i;

		for_each_sg(sgt->sgl, sg, sgt->orig_nents, i)
			arch_dma_prep_coherent(sg_page(sg), sg->length);
	}
#endif
	rtd_pr_rmm_info("%s, sg nents=%d\n", __func__, sgt->orig_nents);
	rtk_iommu_perf_time_start(MAP_IOVA);
	ret = iommu_map_sg_atomic(domain, iova, sgt->sgl, sgt->orig_nents, ioprot);
	if (ret < 0 || ret < size) {
		rtk_iommu_perf_time_end(MAP_IOVA);
		goto out_free_sg;
	}
	rtk_iommu_perf_time_end(MAP_IOVA);
	
	sgt->sgl->dma_address = iova;
	sgt->sgl->dma_length = size;
	return pages;
	
out_free_sg:
	sg_free_table(sgt);
out_free_iova:
	rtk_iommu_dma_free_iova(cookie, iova, size);
out_free_pages:
	__rtk_iommu_dma_free_pages(pages, count);
	return NULL;
}

static const struct iommu_ops rtk_iommu_ops = {
	.map = rtk_iommu_map,
	.unmap = rtk_iommu_unmap,
	.flush_iotlb_all = rtk_iommu_flush_iotlb_all,
	.iotlb_sync	= rtk_iommu_iotlb_sync,
	.iotlb_sync_map	= rtk_iommu_sync_map,
	.iova_to_phys = rtk_iommu_iova_to_phys,
	.pgsize_bitmap = SZ_4K,
};

#if 1//def CONFIG_IOMMU_IO_PGTABLE_RTK_SELFTEST

#define __FAIL(ops)	({				\
		WARN(1, "selftest: test failed\n");	\
		selftest_running = false;		\
		-EFAULT;				\
})

static bool selftest_running;
static struct rtk_iommu_domain *__rtk_domain;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
static int __rtk_iommu_map(struct io_pgtable_ops *ops, unsigned long _iova,
			phys_addr_t paddr, size_t size, int prot, gfp_t gfp)
#else
static int __rtk_iommu_map(struct io_pgtable_ops *ops, unsigned long _iova,
			phys_addr_t paddr, size_t size, int prot)
#endif
{
	return rtk_iommu_map(&__rtk_domain->domain, _iova, paddr, size, prot, GFP_KERNEL);
}

static size_t __rtk_iommu_unmap(struct io_pgtable_ops *ops, unsigned long _iova,
			     size_t size, struct iommu_iotlb_gather *gather)
{
	return rtk_iommu_unmap(&__rtk_domain->domain, _iova, size, gather);
}

static phys_addr_t __rtk_iommu_iova_to_phys(struct io_pgtable_ops *ops,
					unsigned long iova)
{
	return rtk_iommu_iova_to_phys(&__rtk_domain->domain, (dma_addr_t)iova);
}

static int __init rtk_iommu_do_selftests(void)
{
	struct iommu_domain *domain;
	struct rtk_iommu_domain *rtk_domain;
	struct io_pgtable_cfg *cfg;
	struct io_pgtable_ops *ops;

//	unsigned int iova, size, iova_start;
//	unsigned int i, loopnr = 0;

//	dma_addr_t dma_handle;
//	void *cpu_addr;

	domain = rtk_iommu_domain_alloc(IOMMU_DOMAIN_DMA);
	if (!domain) {
		rtd_pr_rmm_err("selftest: failed to allocate iommu domain\n");
		return -EINVAL;
	}

	if (rt_mmu_pgtable_init() < 0) {
		rtd_pr_rmm_err("selftest: fail to open ta\n");
		return -EINVAL;
	}

	mm_rt_mmu_sysfs();

	rtk_domain = to_rtk_domain(domain);
	__rtk_domain = rtk_domain; // work-around for selftest
	cfg = &rtk_domain->cfg;
	ops = rtk_domain->iop;

	selftest_running = true;
#if 0
	/*
	 * Initial sanity checks.
	 * Empty page tables shouldn't provide any translations.
	 */
	if (ops->iova_to_phys(ops, 42))
		return __FAIL(ops);

	if (ops->iova_to_phys(ops, SZ_1G + 42))
		return __FAIL(ops);

	if (ops->iova_to_phys(ops, SZ_2G + 42))
		return __FAIL(ops);

	/*
	 * Distinct mappings of different granule sizes.
	 */
	iova = 0;
	for_each_set_bit(i, &cfg->pgsize_bitmap, BITS_PER_LONG) {
		size = 1UL << i;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
		if (ops->map(ops, iova, iova, size, IOMMU_READ |
						    IOMMU_WRITE |
						    IOMMU_NOEXEC |
						    IOMMU_CACHE, GFP_KERNEL))
#else
		if (ops->map(ops, iova, iova, size, IOMMU_READ |
						    IOMMU_WRITE |
						    IOMMU_NOEXEC |
						    IOMMU_CACHE))
#endif
			return __FAIL(ops);

		/* Overlapping mappings */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
		if (!ops->map(ops, iova, iova + size, size,
			      IOMMU_READ | IOMMU_NOEXEC, GFP_KERNEL))
#else
		if (!ops->map(ops, iova, iova + size, size,
			      IOMMU_READ | IOMMU_NOEXEC))
#endif
			return __FAIL(ops);

		if (ops->iova_to_phys(ops, iova + 42) != (iova + 42))
			return __FAIL(ops);

		iova += SZ_16M;
		loopnr++;
	}

	/* Partial unmap */
	i = 1;
	size = 1UL << __ffs(cfg->pgsize_bitmap);
	while (i < loopnr) {
		iova_start = i * SZ_16M;
		if (ops->unmap(ops, iova_start + size, size, NULL) != size)
			return __FAIL(ops);

		/* Remap of partial unmap */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
		if (ops->map(ops, iova_start + size, size, size, IOMMU_READ, GFP_KERNEL))
#else
		if (ops->map(ops, iova_start + size, size, size, IOMMU_READ))
#endif
			return __FAIL(ops);

		if (ops->iova_to_phys(ops, iova_start + size + 42)
		    != (size + 42))
			return __FAIL(ops);
		i++;
	}

	/* Full unmap */
	iova = 0;
	for_each_set_bit(i, &cfg->pgsize_bitmap, BITS_PER_LONG) {
		size = 1UL << i;

		if (ops->unmap(ops, iova, size, NULL) != size)
			return __FAIL(ops);

		if (ops->iova_to_phys(ops, iova + 42))
			return __FAIL(ops);

		/* Remap full block */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
		if (ops->map(ops, iova, iova, size, IOMMU_WRITE, GFP_KERNEL))
#else
		if (ops->map(ops, iova, iova, size, IOMMU_WRITE))
#endif
			return __FAIL(ops);

		if (ops->iova_to_phys(ops, iova + 42) != (iova + 42))
			return __FAIL(ops);

		iova += SZ_16M;
	}
#endif
//	cpu_addr = rtk_iommu_dma_alloc(NULL, 400*1024, &dma_handle, GFP_KERNEL, 0);
//	rtd_pr_rmm_info("%s: cpu_addr=%lx\n", __func__, (unsigned long)cpu_addr);

//	rtk_iommu_dma_free(NULL, 400*1024, cpu_addr, dma_handle, 0);
//	rtd_pr_rmm_info("%s: done rtk_iommu_dma_free\n", __func__);

//	rtk_iommu_domain_free(domain);
//	rtd_pr_rmm_info("%s: done rtk_iommu_domain_free\n", __func__);

	selftest_running = false;

	rtd_pr_rmm_info("self test ok\n");
	return 0;
}
//late_initcall(rtk_iommu_do_selftests);
module_init(rtk_iommu_do_selftests);
#endif

static dma_addr_t __rtk_iommu_dma_map(struct device *dev, phys_addr_t phys,
		size_t size, int prot, u64 dma_mask)
{
	struct rtk_iommu_domain *rtk_domain = __rtk_domain;
	struct rtk_iommu_dma_cookie *cookie = &rtk_domain->iova_cookie;
	struct iova_domain *iovad = &cookie->iovad;
	size_t iova_off = iova_offset(iovad, phys);
	dma_addr_t iova;

	size = iova_align(iovad, size + iova_off);

	rtk_iommu_perf_time_start(ALLOC_IOVA);
	iova = rtk_iommu_dma_alloc_iova(&rtk_domain->domain, size, dma_mask, dev);
	if (!iova) {
		rtk_iommu_perf_time_end(ALLOC_IOVA);
		return DMA_MAPPING_ERROR;
	}
	rtk_iommu_perf_time_end(ALLOC_IOVA);

	rtk_iommu_perf_time_start(MAP_IOVA);
	if (iommu_map_atomic(&rtk_domain->domain, iova, phys - iova_off, size, prot)) {
		rtk_iommu_perf_time_end(MAP_IOVA);
		rtk_iommu_dma_free_iova(cookie, iova, size);
		return DMA_MAPPING_ERROR;
	}
	rtk_iommu_perf_time_end(MAP_IOVA);
	return iova + iova_off;
}

void *rtk_iommu_dma_alloc_pages(struct device *dev, size_t size,
		struct page **pagep, gfp_t gfp, unsigned long attrs)
{
	struct rtk_iommu_domain *rtk_domain = __rtk_domain;
	struct rtk_iommu_dma_cookie __maybe_unused *cookie = &rtk_domain->iova_cookie;

	size_t alloc_size = PAGE_ALIGN(size);
	struct page *page = NULL;
	void *cpu_addr;

#ifdef CONFIG_VBM_CMA
	if (cookie->vbm_contig_phy)
		page = alloc_specific_contiguous_memory(alloc_size, cookie->vbm_contig_phy, GFP_VBM_CMA, __func__);
	else
		page = alloc_contiguous_memory(alloc_size, GFP_DCU2, __func__);
#else
	page = alloc_contiguous_memory(alloc_size, GFP_DCU2, __func__);
#endif
	if (!page)
		return NULL;

	cpu_addr = page_address(page);

	*pagep = page;
//	memset(cpu_addr, 0, alloc_size);
	return cpu_addr;
}

void *rtk_iommu_dma_alloc_remap(struct device *dev, size_t size,
		dma_addr_t *handle, gfp_t gfp,  pgprot_t prot, unsigned long attrs)
{
	struct page **pages;
	struct sg_table sgt;
	void *cache_addr;
	
	pages = __rtk_iommu_dma_alloc_noncontiguous(dev, &__rtk_domain->domain, size, &sgt, gfp, prot, 0);
	if (!pages)
		return NULL;
	*handle = sgt.sgl->dma_address;
	sg_free_table(&sgt);
	rtk_iommu_perf_time_start(CPU_REMAP_PAGE);
	cache_addr = dma_common_pages_remap(pages, size, prot,
			__builtin_return_address(0));
	if (!cache_addr) {
		rtk_iommu_perf_time_end(CPU_REMAP_PAGE);
		rtd_pr_rmm_err("%s: no vaddr\n", __func__);
		goto out_unmap;
	}
	rtk_iommu_perf_time_end(CPU_REMAP_PAGE);
	rtd_pr_rmm_info("%s: cache_addr=%lx, iova=%lx\n", __func__, (unsigned long)cache_addr, (unsigned long)*handle);
	return cache_addr;

out_unmap:
	__rtk_iommu_dma_unmap(&__rtk_domain->domain, *handle, size);
	__rtk_iommu_dma_free_pages(pages, PAGE_ALIGN(size) >> PAGE_SHIFT);
	return NULL;
}

void *rtk_iommu_dma_alloc(struct device *dev, size_t size,
		dma_addr_t *handle, gfp_t gfp, unsigned long attrs)
{
	struct page *page = NULL;
	int ioprot = 0;
	void *cpu_addr;

	if (!(attrs & DMA_ATTR_FORCE_CONTIGUOUS)) {
		/* non contiguous version */
		cpu_addr = rtk_iommu_dma_alloc_remap(dev, size, handle, gfp, PAGE_KERNEL, attrs);

		rtk_iommu_perf_time_print();
		return cpu_addr;
	}

	/* contiguous version */
	rtk_iommu_perf_time_start(ALLOC_PAGE);
	cpu_addr = rtk_iommu_dma_alloc_pages(dev, size, &page, gfp, attrs);
	if (!cpu_addr) {
		rtk_iommu_perf_time_end(ALLOC_PAGE);
		return NULL;
	}
	rtk_iommu_perf_time_end(ALLOC_PAGE);

	*handle = __rtk_iommu_dma_map(dev, page_to_phys(page), size, ioprot,
			RTK_IOVA_PHY_LIMIT);
	if (*handle == DMA_MAPPING_ERROR) {
		__rtk_iommu_dma_free(dev, size, cpu_addr);
		return NULL;
	}

	rtk_iommu_perf_time_print();
	
	return cpu_addr;
}

void rtk_iommu_dma_free(struct device *dev, size_t size, void *cpu_addr,
		dma_addr_t handle, unsigned long attrs)
{
	__rtk_iommu_dma_unmap(&__rtk_domain->domain, handle, size);
	__rtk_iommu_dma_free(dev, size, cpu_addr);

	rtk_iommu_perf_time_clear();
}

unsigned int *rtk_iommu_iova_table_offset(unsigned long iova)
{
	return (unsigned int *)&page_table[rtk_iova_pte_index(iova)];
}

/* 
 * workaround version for using vbm_heap allocator,
 * FIXME! this will conflict with iova allocator,
 * if both thread using iova allocator and vbm_heap allocator
 */
void rtk_iommu_vbm_iova_init(struct device *dev, unsigned long contig_phy, unsigned long iova, unsigned long size)
{
	struct rtk_iommu_domain *rtk_domain = __rtk_domain;
	struct rtk_iommu_dma_cookie *cookie = &rtk_domain->iova_cookie;

	cookie->has_vbm_iova = 1;
	cookie->vbm_iova = iova;
	cookie->vbm_contig_phy = contig_phy;
	cookie->vbm_size = size;
}

void rtk_iommu_vbm_iova_exit(struct device *dev)
{
	struct rtk_iommu_domain *rtk_domain = __rtk_domain;
	struct rtk_iommu_dma_cookie *cookie = &rtk_domain->iova_cookie;

	cookie->has_vbm_iova = 0;
	cookie->vbm_iova = 0;
	cookie->vbm_contig_phy = 0;
	cookie->vbm_size = 0;
}

MODULE_LICENSE("GPL v2");