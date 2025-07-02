KBUILD_CFLAGS += -Werror -I$(srctree)/mm/
obj-$(CONFIG_REALTEK_MEMORY_MANAGEMENT) += rmm_mod.o
rmm_mod-objs := $(CHIP_NAME)/rmm.o rmm_common.o dvr_cma.o auth.o pageremap.o tlbmap.o rtkblueprint.o rtkrecord.o rmm_svp.o

#ifeq ($(CONFIG_REALTEK_MEMORY_MANAGEMENT),m)
rmm_mod-objs += rmm_vendor_hook.o
#endif

obj-$(CONFIG_REALTEK_IOMMU) += rtk_iommu_m.o
rtk_iommu_m-objs := rtk-iommu.o optee_rt_mmu_pgtable.o

obj-$(CONFIG_REALTEK_DEV_MEM) += rtkmem.o

# for dma-buf and/or ion heaps
obj-$(CONFIG_DMABUF_HEAPS) += dma-buf/
obj-$(CONFIG_ION) += ion/

