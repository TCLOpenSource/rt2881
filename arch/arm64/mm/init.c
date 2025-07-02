// SPDX-License-Identifier: GPL-2.0-only
/*
 * Based on arch/arm/mm/init.c
 *
 * Copyright (C) 1995-2005 Russell King
 * Copyright (C) 2012 ARM Ltd.
 */

#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/errno.h>
#include <linux/swap.h>
#include <linux/init.h>
#include <linux/cache.h>
#include <linux/mman.h>
#include <linux/nodemask.h>
#include <linux/initrd.h>
#include <linux/gfp.h>
#include <linux/memblock.h>
#include <linux/sort.h>
#include <linux/of.h>
#include <linux/of_fdt.h>
#include <linux/dma-direct.h>
#include <linux/dma-map-ops.h>
#include <linux/efi.h>
#include <linux/swiotlb.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/kexec.h>
#include <linux/crash_dump.h>
#include <linux/hugetlb.h>
#include <linux/acpi_iort.h>
#include <linux/kmemleak.h>

#include <asm/boot.h>
#include <asm/fixmap.h>
#include <asm/kasan.h>
#include <asm/kernel-pgtable.h>
#include <asm/kvm_host.h>
#include <asm/memory.h>
#include <asm/numa.h>
#include <asm/sections.h>
#include <asm/setup.h>
#include <linux/sizes.h>
#include <asm/tlb.h>
#include <asm/alternative.h>
#include <asm/xen/swiotlb-xen.h>

#ifdef CONFIG_REALTEK_MEMORY_MANAGEMENT
#include <linux/pageremap.h>
#include <linux/auth.h>
#include <mach/common.h>
#include <mach/rtk_platform.h>

#define CARVEDOUT_LIMIT 512*_MB_
#endif

/*
 * We need to be able to catch inadvertent references to memstart_addr
 * that occur (potentially in generic code) before arm64_memblock_init()
 * executes, which assigns it its actual value. So use a default value
 * that cannot be mistaken for a real physical address.
 */
s64 memstart_addr __ro_after_init = -1;
EXPORT_SYMBOL(memstart_addr);

/*
 * If the corresponding config options are enabled, we create both ZONE_DMA
 * and ZONE_DMA32. By default ZONE_DMA covers the 32-bit addressable memory
 * unless restricted on specific platforms (e.g. 30-bit on Raspberry Pi 4).
 * In such case, ZONE_DMA32 covers the rest of the 32-bit addressable memory,
 * otherwise it is empty.
 *
 * Memory reservation for crash kernel either done early or deferred
 * depending on DMA memory zones configs (ZONE_DMA) --
 *
 * In absence of ZONE_DMA configs arm64_dma_phys_limit initialized
 * here instead of max_zone_phys().  This lets early reservation of
 * crash kernel memory which has a dependency on arm64_dma_phys_limit.
 * Reserving memory early for crash kernel allows linear creation of block
 * mappings (greater than page-granularity) for all the memory bank rangs.
 * In this scheme a comparatively quicker boot is observed.
 *
 * If ZONE_DMA configs are defined, crash kernel memory reservation
 * is delayed until DMA zone memory range size initilazation performed in
 * zone_sizes_init().  The defer is necessary to steer clear of DMA zone
 * memory range to avoid overlap allocation.  So crash kernel memory boundaries
 * are not known when mapping all bank memory ranges, which otherwise means
 * not possible to exclude crash kernel range from creating block mappings
 * so page-granularity mappings are created for the entire memory range.
 * Hence a slightly slower boot is observed.
 *
 * Note: Page-granularity mapppings are necessary for crash kernel memory
 * range for shrinking its size via /sys/kernel/kexec_crash_size interface.
 */
#if IS_ENABLED(CONFIG_ZONE_DMA) || IS_ENABLED(CONFIG_ZONE_DMA32)
phys_addr_t __ro_after_init arm64_dma_phys_limit;
#else
phys_addr_t __ro_after_init arm64_dma_phys_limit = PHYS_MASK + 1;
#endif

/*
 * Provide a run-time mean of disabling ZONE_DMA32 if it is enabled via
 * CONFIG_ZONE_DMA32.
 */
static bool disable_dma32 __ro_after_init;

unsigned int arm64_zone_dma32_bits __ro_after_init = 32;
EXPORT_SYMBOL(arm64_zone_dma32_bits);

#ifdef CONFIG_KEXEC_CORE
/*
 * reserve_crashkernel() - reserves memory for crash kernel
 *
 * This function reserves memory area given in "crashkernel=" kernel command
 * line parameter. The memory reserved is used by dump capture kernel when
 * primary kernel is crashing.
 */
static void __init reserve_crashkernel(void)
{
	unsigned long long crash_base, crash_size;
	unsigned long long crash_max = arm64_dma_phys_limit;
	int ret;

	ret = parse_crashkernel(boot_command_line, memblock_phys_mem_size(),
				&crash_size, &crash_base);
	/* no crashkernel= or invalid value specified */
	if (ret || !crash_size)
		return;

	crash_size = PAGE_ALIGN(crash_size);

	/* User specifies base address explicitly. */
	if (crash_base)
		crash_max = crash_base + crash_size;

	/* Current arm64 boot protocol requires 2MB alignment */
	crash_base = memblock_phys_alloc_range(crash_size, SZ_2M,
					       crash_base, crash_max);
	if (!crash_base) {
		pr_warn("cannot allocate crashkernel (size:0x%llx)\n",
			crash_size);
		return;
	}

	pr_info("crashkernel reserved: 0x%016llx - 0x%016llx (%lld MB)\n",
		crash_base, crash_base + crash_size, crash_size >> 20);

	/*
	 * The crashkernel memory will be removed from the kernel linear
	 * map. Inform kmemleak so that it won't try to access it.
	 */
	kmemleak_ignore_phys(crash_base);
	crashk_res.start = crash_base;
	crashk_res.end = crash_base + crash_size - 1;
}
#else
static void __init reserve_crashkernel(void)
{
}
#endif /* CONFIG_KEXEC_CORE */

/*
 * Return the maximum physical address for a zone accessible by the given bits
 * limit. If DRAM starts above 32-bit, expand the zone to the maximum
 * available memory, otherwise cap it at 32-bit.
 */
static phys_addr_t __init max_zone_phys(unsigned int zone_bits)
{
	phys_addr_t zone_mask = DMA_BIT_MASK(zone_bits);
	phys_addr_t phys_start = memblock_start_of_DRAM();

	if (phys_start > U32_MAX)
		zone_mask = PHYS_ADDR_MAX;
	else if (phys_start > zone_mask)
		zone_mask = U32_MAX;

	return min(zone_mask, memblock_end_of_DRAM() - 1) + 1;
}

extern void __init set_movablecore(unsigned long pages);
static void __init zone_sizes_init(unsigned long min, unsigned long max)
{
	unsigned long max_zone_pfns[MAX_NR_ZONES]  = {0};
	unsigned int __maybe_unused acpi_zone_dma_bits;
	unsigned int __maybe_unused dt_zone_dma_bits;
	phys_addr_t __maybe_unused dma32_phys_limit = max_zone_phys(arm64_zone_dma32_bits);

#if defined(RTK_MEM_LAYOUT_DEVICETREE)
    // leave it native upstream for GKI, don't dedicate cma2 to movable zone.

	unsigned long max_dma = min;
	unsigned long max_normal = max;
    //FIXME considering GKI, there may need another solution.
    // specify cma2 to ZONE_MOVABLE may lower the chance of fragmentation or page migration
    struct device_node *cma2_np;
    unsigned long zone2_start = 0;
    u64 tmp64[2];

#elif defined(CONFIG_REALTEK_MEMORY_MANAGEMENT)

	unsigned long zone_size[MAX_NR_ZONES];
	unsigned long max_dma = min;
	unsigned long high_size;
	int carvedout_in_highmem = 0;
	unsigned long carvedout_start = 0;
	unsigned long carvedout_end = 0;
	unsigned long carvedout_cma_high_size = 0;  // HighMem reserved size (GPU)
	unsigned long carvedout_high_size = 0;      // carvedout memory size in highmem
	carvedout_cma_high_size = carvedout_buf_query(CARVEDOUT_GPU_RESERVED, NULL);
	carvedout_high_size = carvedout_buf_query_range(CARVEDOUT_HIGH_START, CARVEDOUT_HIGH_END, (void **)&carvedout_start, (void **)&carvedout_end);

	printk("[MEM] zone_sizes_init, min = 0x%lx, max = 0x%lx\n", min, max);
#endif

#ifdef CONFIG_ZONE_DMA
	acpi_zone_dma_bits = fls64(acpi_iort_dma_get_max_cpu_address());
	dt_zone_dma_bits = fls64(of_dma_get_max_cpu_address(NULL));
	zone_dma_bits = min3(32U, dt_zone_dma_bits, acpi_zone_dma_bits);
	arm64_dma_phys_limit = max_zone_phys(zone_dma_bits);
	max_zone_pfns[ZONE_DMA] = PFN_DOWN(arm64_dma_phys_limit);
#endif
#ifdef CONFIG_ZONE_DMA32
	max_zone_pfns[ZONE_DMA32] = disable_dma32 ? 0 : PFN_DOWN(dma32_phys_limit);
	if (!arm64_dma_phys_limit)
		arm64_dma_phys_limit = dma32_phys_limit;
#endif
	max_zone_pfns[ZONE_NORMAL] = max;

#if defined(RTK_MEM_LAYOUT_DEVICETREE)
    // leave it native upstream for GKI, don't dedicate cma2 to movable zone.
	max_dma = PFN_DOWN(arm64_dma_phys_limit);

    //FIXME considering GKI, there may need another solution.
    // before GKI, for legacy consistency, let cma2 to MOVABLE zone.
    // for cma other than cma1 and in 32-bit(<max_dma) from NORMAL to MOVABLE

    // for some platform preserve cma-ui4k region, it's always prior than cma2
    cma2_np = of_find_node_by_path("/reserved-memory/cma-ui4k_memory");
    if ((of_property_read_u64_array(cma2_np, "reg", tmp64, 2)) == 0 && !of_property_present(cma2_np, "movablezone_unset")) {
        printk("[MEM] found cma(2)-ui4k 0x%lx 0x%lx in ufdt\n", tmp64[0], tmp64[1]);
        if (tmp64[1] != 0) { // size 0 for disabled
            zone2_start = PFN_DOWN(tmp64[0]);
			if (zone2_start < max_dma) {
				printk("[MEM] max_dma %lx -> %lx\n", max_dma, zone2_start);
				max_dma = zone2_start;
			}
			if (zone2_start < max_normal) {
				printk("[MEM] max_normal %lx -> %lx\n", max_normal, zone2_start);
				max_normal = zone2_start;
            }
        }
        else {
            printk("[MEM] cma(2)-ui4k disabled in ufdt\n");
        }
    }
    else {
        printk("[MEM] NO cma(2)-ui4k in ufdt\n");
    }

    if (zone2_start == 0) { // no early zone boundary found, precondition is ui4k prior than cma2
        cma2_np = of_find_node_by_path("/reserved-memory/cma2_memory");
        if ((of_property_read_u64_array(cma2_np, "reg", tmp64, 2)) == 0 && !of_property_present(cma2_np, "movablezone_unset")) {
            printk("[MEM] found cma2 0x%lx 0x%lx in ufdt\n", tmp64[0], tmp64[1]);
            if (tmp64[1] != 0) {
                zone2_start = PFN_DOWN(tmp64[0]);
				if (zone2_start < max_dma) {
					printk("[MEM] max_dma %lx -> %lx\n", max_dma, zone2_start);
					max_dma = zone2_start;
				}
				if (zone2_start < max_normal) {
					printk("[MEM] max_normal %lx -> %lx\n", max_normal, zone2_start);
					max_normal = zone2_start;
                }
            }
            else {
                printk("[MEM] ERROR cma2 disabled in ufdt\n");
            }
        }
        else {
            printk("[MEM] ERROR cannot find cma2 in ufdt\n");
        }
    }

#elif defined(CONFIG_REALTEK_MEMORY_MANAGEMENT)
	max_dma = PFN_DOWN(arm64_dma_phys_limit);
	memset(zone_size, 0, sizeof(zone_size));
	zone_size[ZONE_DMA32] = max_zone_pfns[ZONE_DMA32];
	if (max_zone_pfns[ZONE_NORMAL] > max_zone_pfns[ZONE_DMA32])
		zone_size[ZONE_NORMAL] = max_zone_pfns[ZONE_NORMAL] - max_zone_pfns[ZONE_DMA32];

	printk("[MEM] zone_size[ZONE_DMA32] = 0x%lx\n", zone_size[ZONE_DMA32]);
	printk("[MEM] zone_size[ZONE_NORMAL] = 0x%lx\n", zone_size[ZONE_NORMAL]);
	printk("[MEM] carvedout_start = 0x%lx, max_dma = 0x%lx, max = 0x%lx\n",
		   carvedout_start, max_dma << PAGE_SHIFT, max << PAGE_SHIFT);

	if (carvedout_start >= (unsigned long)CARVEDOUT_LIMIT) {
		printk("[MEM] %s(): carvedout is in highmem\n", __func__);
		carvedout_in_highmem = 1;
	}
#endif

#if defined(RTK_MEM_LAYOUT_DEVICETREE)
    //FIXME considering GKI, there may need another solution.
    // before GKI, for legacy consistency, let cma2 to MOVABLE zone.
    // for cma higher than max_dma from NORMAL to MOVABLE
	printk("[MEM] min %lx, max %lx, max_dma %lx, max_normal %lx\n", min, max, max_dma, max_normal);
	set_movablecore(max - max_normal);

#elif defined(CONFIG_REALTEK_MEMORY_MANAGEMENT)
	high_size = zone_size[ZONE_DMA32];
	printk("[MEM] highmem size: %lx\n", high_size);

// Remove meanless checking
#if 0
	if ((high_size << PAGE_SHIFT) < kernel_highmem_size) {
		pr_err("[MEM] err: zone_highmem size(%lx) not equal to calculated size(%lx)\n",
			   (high_size << PAGE_SHIFT), kernel_highmem_size);
		BUG();
	}
#endif

	if (high_size > 0) {
		if (!carvedout_in_highmem) {
			carvedout_high_size = 0;
			printk("[MEM] Carvedout in Low Mem, so needless to calculate carvedout for HighMem/Movable zone size\n");
		}

		zone_size[ZONE_MOVABLE] = cma_highmem_size >> PAGE_SHIFT;
		zone_size[ZONE_DMA] = zone_size[ZONE_DMA32] = zone_size[ZONE_DMA32] + zone_size[ZONE_NORMAL] - zone_size[ZONE_MOVABLE];

		if (carvedout_end == memblock_end_of_DRAM()) { // carvedout is in last
			if (zone_size[ZONE_DMA32] < (carvedout_high_size >> PAGE_SHIFT))
				BUG();
			zone_size[ZONE_MOVABLE] += (carvedout_high_size >> PAGE_SHIFT);
			zone_size[ZONE_DMA32] -= (carvedout_high_size >> PAGE_SHIFT);
		}

		if (zone_size[ZONE_DMA32] > max_dma) {
			zone_size[ZONE_DMA] =  zone_size[ZONE_DMA32] = max_dma;
			zone_size[ZONE_NORMAL] = zone_size[ZONE_DMA32] - max_dma;
		} else {
			zone_size[ZONE_NORMAL] = 0;
		}

		printk("### dma32 %lx movable %lx ###\n", 
			   zone_size[ZONE_DMA32],
			   zone_size[ZONE_MOVABLE]);
		if (zone_size[ZONE_NORMAL] > 0)
			printk("### normal %lx ###\n",
				   zone_size[ZONE_NORMAL]);

		set_movablecore(zone_size[ZONE_MOVABLE]);
	}
#endif

	free_area_init(max_zone_pfns);
}

static int __init early_arm64_zone_dma32_bits(char *p)
{
	unsigned int value = 32;

    if (!p) {
        return 1;
	}

	value = (unsigned int)memparse(p, &p);
	if (value >= 24 && value < 32) {
		arm64_zone_dma32_bits = value;
		pr_notice("zone_dma32 size adjust to %u MB\n", (1U << arm64_zone_dma32_bits) >> 20);
	}

    return 0;
}
early_param("arm64_zone_dma32_bits", early_arm64_zone_dma32_bits);

static int __init early_disable_dma32(char *buf)
{
	if (!buf)
		return -EINVAL;

	if (!strcmp(buf, "on"))
		disable_dma32 = true;

	return 0;
}
early_param("disable_dma32", early_disable_dma32);

int pfn_is_map_memory(unsigned long pfn)
{
	phys_addr_t addr = PFN_PHYS(pfn);

	/* avoid false positives for bogus PFNs, see comment in pfn_valid() */
	if (PHYS_PFN(addr) != pfn)
		return 0;

	return memblock_is_map_memory(addr);
}
EXPORT_SYMBOL(pfn_is_map_memory);

static phys_addr_t memory_limit = PHYS_ADDR_MAX;

/*
 * Limit the memory size that was specified via FDT.
 */
static int __init early_mem(char *p)
{
	if (!p)
		return 1;

	memory_limit = memparse(p, &p) & PAGE_MASK;
	pr_notice("Memory limited to %lldMB\n", memory_limit >> 20);

	return 0;
}
early_param("mem", early_mem);

void __init arm64_memblock_init(void)
{
	s64 linear_region_size = PAGE_END - _PAGE_OFFSET(vabits_actual);

	/*
	 * Corner case: 52-bit VA capable systems running KVM in nVHE mode may
	 * be limited in their ability to support a linear map that exceeds 51
	 * bits of VA space, depending on the placement of the ID map. Given
	 * that the placement of the ID map may be randomized, let's simply
	 * limit the kernel's linear map to 51 bits as well if we detect this
	 * configuration.
	 */
	if (IS_ENABLED(CONFIG_KVM) && vabits_actual == 52 &&
	    is_hyp_mode_available() && !is_kernel_in_hyp_mode()) {
		pr_info("Capping linear region to 51 bits for KVM in nVHE mode on LVA capable hardware.\n");
		linear_region_size = min_t(u64, linear_region_size, BIT(51));
	}

	/* Remove memory above our supported physical address size */
	memblock_remove(1ULL << PHYS_MASK_SHIFT, ULLONG_MAX);

	/*
	 * Select a suitable value for the base of physical memory.
	 */
	memstart_addr = round_down(memblock_start_of_DRAM(),
				   ARM64_MEMSTART_ALIGN);

	if ((memblock_end_of_DRAM() - memstart_addr) > linear_region_size)
		pr_warn("Memory doesn't fit in the linear mapping, VA_BITS too small\n");

	/*
	 * Remove the memory that we will not be able to cover with the
	 * linear mapping. Take care not to clip the kernel which may be
	 * high in memory.
	 */
	memblock_remove(max_t(u64, memstart_addr + linear_region_size,
			__pa_symbol(_end)), ULLONG_MAX);
	if (memstart_addr + linear_region_size < memblock_end_of_DRAM()) {
		/* ensure that memstart_addr remains sufficiently aligned */
		memstart_addr = round_up(memblock_end_of_DRAM() - linear_region_size,
					 ARM64_MEMSTART_ALIGN);
		memblock_remove(0, memstart_addr);
	}

	/*
	 * If we are running with a 52-bit kernel VA config on a system that
	 * does not support it, we have to place the available physical
	 * memory in the 48-bit addressable part of the linear region, i.e.,
	 * we have to move it upward. Since memstart_addr represents the
	 * physical address of PAGE_OFFSET, we have to *subtract* from it.
	 */
	if (IS_ENABLED(CONFIG_ARM64_VA_BITS_52) && (vabits_actual != 52))
		memstart_addr -= _PAGE_OFFSET(48) - _PAGE_OFFSET(52);

	/*
	 * Apply the memory limit if it was set. Since the kernel may be loaded
	 * high up in memory, add back the kernel region that must be accessible
	 * via the linear mapping.
	 */
	if (memory_limit != PHYS_ADDR_MAX) {
		memblock_mem_limit_remove_map(memory_limit);
		memblock_add(__pa_symbol(_text), (u64)(_end - _text));
	}

	if (IS_ENABLED(CONFIG_BLK_DEV_INITRD) && phys_initrd_size) {
		/*
		 * Add back the memory we just removed if it results in the
		 * initrd to become inaccessible via the linear mapping.
		 * Otherwise, this is a no-op
		 */
		u64 base = phys_initrd_start & PAGE_MASK;
		u64 size = PAGE_ALIGN(phys_initrd_start + phys_initrd_size) - base;

		/*
		 * We can only add back the initrd memory if we don't end up
		 * with more memory than we can address via the linear mapping.
		 * It is up to the bootloader to position the kernel and the
		 * initrd reasonably close to each other (i.e., within 32 GB of
		 * each other) so that all granule/#levels combinations can
		 * always access both.
		 */
		if (WARN(base < memblock_start_of_DRAM() ||
			 base + size > memblock_start_of_DRAM() +
				       linear_region_size,
			"initrd not fully accessible via the linear mapping -- please check your bootloader ...\n")) {
			phys_initrd_size = 0;
		} else {
			memblock_remove(base, size); /* clear MEMBLOCK_ flags */
			memblock_add(base, size);
			memblock_reserve(base, size);
		}
	}

	if (IS_ENABLED(CONFIG_RANDOMIZE_BASE)) {
		extern u16 memstart_offset_seed;
		u64 mmfr0 = read_cpuid(ID_AA64MMFR0_EL1);
		int parange = cpuid_feature_extract_unsigned_field(
					mmfr0, ID_AA64MMFR0_EL1_PARANGE_SHIFT);
		s64 range = linear_region_size -
			    BIT(id_aa64mmfr0_parange_to_phys_shift(parange));

		/*
		 * If the size of the linear region exceeds, by a sufficient
		 * margin, the size of the region that the physical memory can
		 * span, randomize the linear region as well.
		 */
		if (memstart_offset_seed > 0 && range >= (s64)ARM64_MEMSTART_ALIGN) {
			range /= ARM64_MEMSTART_ALIGN;
			memstart_addr -= ARM64_MEMSTART_ALIGN *
					 ((range * memstart_offset_seed) >> 16);
		}
	}

	/*
	 * Register the kernel text, kernel data, initrd, and initial
	 * pagetables with memblock.
	 */
	memblock_reserve(__pa_symbol(_stext), _end - _stext);
	if (IS_ENABLED(CONFIG_BLK_DEV_INITRD) && phys_initrd_size) {
		/* the generic initrd code expects virtual addresses */
		initrd_start = __phys_to_virt(phys_initrd_start);
		initrd_end = initrd_start + phys_initrd_size;
	}

#if defined(RTK_MEM_LAYOUT_DEVICETREE)
    // no rtk_reserve, will do early_init_fdt_scan_reserved_mem / rmem_cma_setup

    //? it's moved from rtk_reserve(), not confirmed if it is necessary now //FIXME
    /* M6PRTANOM-100, Reserved to avoid STR resume secondary_core text section be allocated.*/
    memblock_reserve(__pa(&_text), PAGE_SIZE);

#elif defined(CONFIG_REALTEK_MEMORY_MANAGEMENT)
	rtk_reserve(); //FIXME
#endif

	early_init_fdt_scan_reserved_mem();

	if (!IS_ENABLED(CONFIG_ZONE_DMA) && !IS_ENABLED(CONFIG_ZONE_DMA32))
		reserve_crashkernel();

	high_memory = __va(memblock_end_of_DRAM() - 1) + 1;

#if defined(RTK_MEM_LAYOUT_DEVICETREE)
    // leave it native upstream for GKI

#elif defined(CONFIG_REALTEK_MEMORY_MANAGEMENT)
	{
#ifdef CONFIG_VBM_CMA
		unsigned long carvedout_cma_vbm_size, carvedout_cma_vbm_start;
#endif

		int carvedout_in_highmem = 0;
		unsigned long carvedout_start = 0;
		unsigned long carvedout_end = 0;
		unsigned long carvedout_cma_high_size = 0;  // HighMem reserved size (GPU)
		unsigned long carvedout_high_size = 0;      // carvedout memory size in highmem
		int ret = 0;
		unsigned int layout_idx = carvedout_buf_get_layout_idx();

		carvedout_cma_high_size = carvedout_buf_query(CARVEDOUT_GPU_RESERVED, NULL);
		carvedout_high_size = carvedout_buf_query_range(CARVEDOUT_HIGH_START, CARVEDOUT_HIGH_END, (void **)&carvedout_start, (void **)&carvedout_end);
//		carvedout_high_size = fdt_get_carvedout_mem_size_in_highmem(high_start, max);

//		if (arm64_dma_phys_limit == 0) {
//			pr_err("[MEM] err: no lowmem limit\n");
//			BUG();
//		}

		if (memblock_end_of_DRAM() < CARVEDOUT_LIMIT) {
			pr_err("[MEM] err: memblock_end_of_DRAM() < 512MB\n");
			BUG();
		}
		kernel_highmem_size = memblock_end_of_DRAM() - CARVEDOUT_LIMIT; // hardcode for cma size calculation 

		if (carvedout_start >= CARVEDOUT_LIMIT) {
			carvedout_in_highmem = 1;
			printk("[MEM] %s(): carvedout is in highmem\n", __func__);
		} else if (carvedout_end > CARVEDOUT_LIMIT) {
			pr_err("[MEM] err: carvedout region cross memory zone !\n");
			BUG();
		}

		if (!carvedout_in_highmem) {
			carvedout_high_size = 0;
			printk("[MEM] carvedout in Low Mem, so needless to calculate carved-out for HighMem CMA\n");
		}

		if ((carvedout_high_size + ZRAM_RESERVED_SIZE + carvedout_cma_high_size) > kernel_highmem_size) {
			if (carvedout_high_size > kernel_highmem_size) {
				pr_err("[MEM] err: carvedout size(%lx) bigger than kernel highmem size(%lx)\n", carvedout_high_size, kernel_highmem_size);
				BUG();
			} else
				cma_highmem_size = kernel_highmem_size - carvedout_high_size;
		} else {
			cma_highmem_size = kernel_highmem_size - (carvedout_high_size + ZRAM_RESERVED_SIZE + carvedout_cma_high_size);
		}
		cma_highmem_start = memblock_end_of_DRAM() - cma_highmem_size;

		if (carvedout_in_highmem && (carvedout_end == memblock_end_of_DRAM())) { // carvedout is in last
			cma_highmem_start = memblock_end_of_DRAM() - carvedout_high_size - cma_highmem_size;
		}
		printk("[MEM] HighMem carved-out = 0x%x, high_cma_reserved = 0x%x\n", (unsigned int)carvedout_high_size, (unsigned int)carvedout_cma_high_size);

		if ((get_ui_resolution() == UI_RESOLUTION_4K) && (cma_highmem_size > CMA_GPU_4K_SIZE)) {
#ifdef CONFIG_RTK_BUSINESS_DISPLAY_MODEL
			carvedout_buf[layout_idx][CARVEDOUT_CMA_GPU_4K][0] = CARVEDOUT_LIMIT;
#else
			carvedout_buf[layout_idx][CARVEDOUT_CMA_GPU_4K][0] = cma_highmem_start;
#endif
			carvedout_buf[layout_idx][CARVEDOUT_CMA_GPU_4K][1] = CMA_GPU_4K_SIZE;
			carvedout_buf[layout_idx][CARVEDOUT_CMA_HIGH][0] = cma_highmem_start + CMA_GPU_4K_SIZE;
			carvedout_buf[layout_idx][CARVEDOUT_CMA_HIGH][1] = cma_highmem_size - CMA_GPU_4K_SIZE;
		} else {
			carvedout_buf[layout_idx][CARVEDOUT_CMA_HIGH][0] = cma_highmem_start;
			carvedout_buf[layout_idx][CARVEDOUT_CMA_HIGH][1] = cma_highmem_size;
		}
		printk("reserve %08lx - %08lx for high memory cma...\n",
			   carvedout_buf[layout_idx][CARVEDOUT_CMA_HIGH][0], carvedout_buf[layout_idx][CARVEDOUT_CMA_HIGH][0] + carvedout_buf[layout_idx][CARVEDOUT_CMA_HIGH][1]);
		ret = memblock_reserve(carvedout_buf[layout_idx][CARVEDOUT_CMA_HIGH][0], carvedout_buf[layout_idx][CARVEDOUT_CMA_HIGH][1]);
		if (ret)
			pr_err("[MEM] memblock_reserve for highmem CMA failed, errno = %d\n", ret);

		if (carvedout_buf[layout_idx][CARVEDOUT_CMA_GPU_4K][1]) {
			pr_info("reserve %08lx - %08lx for cma gpu_4k...\n",
					carvedout_buf[layout_idx][CARVEDOUT_CMA_GPU_4K][0],
					carvedout_buf[layout_idx][CARVEDOUT_CMA_GPU_4K][0] + carvedout_buf[layout_idx][CARVEDOUT_CMA_GPU_4K][1]);
			ret = memblock_reserve(carvedout_buf[layout_idx][CARVEDOUT_CMA_GPU_4K][0], carvedout_buf[layout_idx][CARVEDOUT_CMA_GPU_4K][1]);

			if (ret)
				pr_err("[MEM] memblock_reserve for CMA_GPU_4K failed, errno = %d\n", ret);
		}

#ifdef CONFIG_VBM_CMA
		carvedout_cma_vbm_size = carvedout_buf_query(CARVEDOUT_CMA_VBM, (void **)&carvedout_cma_vbm_start);
		if (carvedout_cma_vbm_size) {
			pr_info("reserve %08lx - %08lx for high memory vbm cma...\n",
					carvedout_cma_vbm_start, carvedout_cma_vbm_start + carvedout_cma_vbm_size);
			ret = memblock_reserve(carvedout_cma_vbm_start, carvedout_cma_vbm_size);

			if (ret)
				pr_err("[MEM] memblock_reserve for CMA-VBM failed, errno = %d\n", ret);
		}
#endif
	}
#endif
}

void __init bootmem_init(void)
{
	unsigned long min, max;

	min = PFN_UP(memblock_start_of_DRAM());
	max = PFN_DOWN(memblock_end_of_DRAM());

	early_memtest(min << PAGE_SHIFT, max << PAGE_SHIFT);

	max_pfn = max_low_pfn = max;
	min_low_pfn = min;

	arch_numa_init();

	/*
	 * must be done after arch_numa_init() which calls numa_init() to
	 * initialize node_online_map that gets used in hugetlb_cma_reserve()
	 * while allocating required CMA size across online nodes.
	 */
#if defined(CONFIG_HUGETLB_PAGE) && defined(CONFIG_CMA)
	arm64_hugetlb_cma_reserve();
#endif

	dma_pernuma_cma_reserve();

	kvm_hyp_reserve();

	/*
	 * sparse_init() tries to allocate memory from memblock, so must be
	 * done after the fixed reservations
	 */
	sparse_init();
	zone_sizes_init(min, max);

	/*
	 * Reserve the CMA area after arm64_dma_phys_limit was initialised.
	 */
#ifndef CONFIG_REALTEK_MEMORY_MANAGEMENT
	dma_contiguous_reserve(arm64_dma_phys_limit);
#else

#if defined(RTK_MEM_LAYOUT_DEVICETREE)
	dma_contiguous_reserve(arm64_dma_phys_limit);

#else // !defined(RTK_MEM_LAYOUT_DEVICETREE)
	rtk_reserve_cma(arm64_dma_phys_limit, arm64_dma_phys_limit);

#endif // defined(RTK_MEM_LAYOUT_DEVICETREE)
#endif

	/*
	 * request_standard_resources() depends on crashkernel's memory being
	 * reserved, so do it here.
	 */
	if (IS_ENABLED(CONFIG_ZONE_DMA) || IS_ENABLED(CONFIG_ZONE_DMA32))
		reserve_crashkernel();

	memblock_dump_all();
}

/*
 * mem_init() marks the free areas in the mem_map and tells us how much memory
 * is free.  This is done after various parts of the system have claimed their
 * memory after the kernel image.
 */
void __init mem_init(void)
{
	if (swiotlb_force == SWIOTLB_FORCE ||
	    max_pfn > PFN_DOWN(arm64_dma_phys_limit))
		swiotlb_init(1);
	else if (!xen_swiotlb_detect())
		swiotlb_force = SWIOTLB_NO_FORCE;

	set_max_mapnr(max_pfn - PHYS_PFN_OFFSET);

	/* this will put all unused low memory onto the freelists */
	memblock_free_all();

	/*
	 * Check boundaries twice: Some fundamental inconsistencies can be
	 * detected at build time already.
	 */
#ifdef CONFIG_COMPAT
	BUILD_BUG_ON(TASK_SIZE_32 > DEFAULT_MAP_WINDOW_64);
#endif

	/*
	 * Selected page table levels should match when derived from
	 * scratch using the virtual address range and page size.
	 */
	BUILD_BUG_ON(ARM64_HW_PGTABLE_LEVELS(CONFIG_ARM64_VA_BITS) !=
		     CONFIG_PGTABLE_LEVELS);

	if (PAGE_SIZE >= 16384 && get_num_physpages() <= 128) {
		extern int sysctl_overcommit_memory;
		/*
		 * On a machine this small we won't get anywhere without
		 * overcommit, so turn it on by default.
		 */
		sysctl_overcommit_memory = OVERCOMMIT_ALWAYS;
	}
}

void free_initmem(void)
{
	free_reserved_area(lm_alias(__init_begin),
			   lm_alias(__init_end),
			   POISON_FREE_INITMEM, "unused kernel");
	/*
	 * Unmap the __init region but leave the VM area in place. This
	 * prevents the region from being reused for kernel modules, which
	 * is not supported by kallsyms.
	 */
	vunmap_range((u64)__init_begin, (u64)__init_end);
}

void dump_mem_limit(void)
{
	if (memory_limit != PHYS_ADDR_MAX) {
		pr_emerg("Memory Limit: %llu MB\n", memory_limit >> 20);
	} else {
		pr_emerg("Memory Limit: none\n");
	}
}
