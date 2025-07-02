KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/include/$(CHIP_NAME)/rtk_kdriver
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/include/$(CHIP_NAME)
KBUILD_CPPFLAGS += -I$(srctree)/drivers/staging/android/ion
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/gal/$(CHIP_NAME)

KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/common_header
KBUILD_CFLAGS += -Werror



# if can get register value
#KBUILD_CFLAGS += -DRTK_ENABLE_GDMA_EXT_REGISTER=0
#KBUILD_CFLAGS += -DRTK_GDMA_TEST_6748_OSD1
#KBUILD_CFLAGS += -DRTK_GDMA_TEST_OSD1
#KBUILD_CFLAGS += -DGDMA_CONFIG_ENABLE_FPGA
#KBUILD_CFLAGS += -DGDMA4_ENABLE_VSYNC_NOTIFY

KBUILD_CFLAGS += -DGDMA_CONFIG_ENABLE_INTERRUPT


RTK_KDRV_GDMA_EXT_ENABLE=y

# SE
ifeq ($(CONFIG_RTK_KDRV_SE),m)
obj-m += rtk-se.o
rtk-se-objs := rtk_se_driver.o rtk_se_lib.o rtk_kadp_se.o rtk_se_stch.o
else
obj-$(CONFIG_RTK_KDRV_SE) += rtk_se_driver.o rtk_se_lib.o rtk_kadp_se.o rtk_se_stch.o
endif

ifeq ($(RTK_KDRV_GDMA_EXT_ENABLE),y)
   KBUILD_CFLAGS += -DRTK_ENABLE_GDMA_EXT=1
   KBUILD_CFLAGS += -DGDMA4_ENABLE_VSYNC_NOTIFY=1
endif
   
# GDMA Driver
ifeq ($(CONFIG_RTK_KDRV_GDMA),m)
obj-m += rtk-gdma.o
rtk-gdma-objs := rtk_gdma.o rtk_osd_sr_driver.o osd_dump_agent.o rtk_gdma_split.o rtk_gdma_fence.o
 
ifeq ($(RTK_KDRV_GDMA_EXT_ENABLE),y)
   rtk-gdma-objs += osdExt/rtk_osdExt_main.o osdExt/rtk_osdExt_debug.o osdExt/rtk_osdExt_linux.o osdExt/rtk_osdExt_sr.o osdExt/rtk_osdExt_intr.o osdExt/rtk_osdExt_reg.o osdExt/rtk_osdExt_reg_5.o
 endif
 
else
obj-$(CONFIG_RTK_KDRV_GDMA)     += rtk_gdma.o rtk_osd_sr_driver.o osd_dump_agent.o rtk_gdma_split.o rtk_gdma_fence.o
obj-$(RTK_KDRV_GDMA_EXT_ENABLE) += osdExt/rtk_osdExt_main.o osdExt/rtk_osdExt_debug.o osdExt/rtk_osdExt_linux.o osdExt/rtk_osdExt_sr.o osdExt/rtk_osdExt_intr.o osdExt/rtk_osdExt_reg.o osdExt/rtk_osdExt_reg_5.o

  #test on;y for module_init
#obj-$(CONFIG_RTK_KDRV_GDMA) = simple.o

endif

ifeq ($(CONFIG_RTK_KDRV_REFDECMP),m)
obj-m += rtk-refdecmp.o
rtk-refdecmp-objs := rtk_refdecmp.o rtk_refdecmp_ver.o
else
obj-$(CONFIG_RTK_KDRV_REFDECMP)     += rtk_refdecmp.o rtk_refdecmp_ver.o
endif
# GAL Driver
obj-$(CONFIG_RTK_KDRV_GAL)    += rtk_kadp_gal_gfx.o rtk_kadp_gdma_hal_gal.o rtk_kadp_hal_gal_surface.o
