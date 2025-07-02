#ifndef _RTK_IOMMU_H
#define _RTK_IOMMU_H

#include <linux/iommu.h>
#include <linux/io-pgtable.h>

#if !defined(CONFIG_ARCH_RTK2819A)
/* 128MB iova space */
#define RTK_IOVA_PHY_BASE  0xE0000000
#define RTK_IOVA_PHY_END 0xE8000000
#define NUM_PT_ENTRIES 32768
#else
/* 256MB iova space */
#define RTK_IOVA_PHY_BASE  0xE0000000
#define RTK_IOVA_PHY_END 0xF0000000
#define NUM_PT_ENTRIES 65536
#endif
/* 104MB iova space */
//#define RTK_IOVA_PHY_BASE  0xE0000000
//#define RTK_IOVA_PHY_END 0xE6800000

#define RTK_IOVA_PHY_LIMIT (RTK_IOVA_PHY_END-1)

#define SPAGE_ORDER 12
#define SPAGE_SIZE (1 << SPAGE_ORDER)

enum rtk_iommu_io_pgtable_fmt {
	RTK_IOMMU_IO_PGTABLE_V1,
	RTK_IOMMU_IO_PGTABLE_NUM_FMTS,
};

static const struct iommu_ops rtk_iommu_ops;

struct io_pgtable_ops *rtk_iommu_alloc_io_pgtable_ops(enum rtk_iommu_io_pgtable_fmt fmt,
					    struct io_pgtable_cfg *cfg,
						void *cookie);
void rtk_iommu_free_io_pgtable_ops(struct io_pgtable_ops *ops);

void *rtk_iommu_dma_alloc(struct device *dev, size_t size, dma_addr_t *handle, gfp_t gfp, unsigned long attrs);
void rtk_iommu_dma_free(struct device *dev, size_t size, void *cpu_addr, dma_addr_t handle, unsigned long attrs);

unsigned int *rtk_iommu_iova_table_offset(unsigned long iova);

void rtk_iommu_vbm_iova_init(struct device *dev, unsigned long contig_phy, unsigned long iova, unsigned long size);
void rtk_iommu_vbm_iova_exit(struct device *dev);

#endif