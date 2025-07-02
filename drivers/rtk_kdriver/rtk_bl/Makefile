#
# Makefile for the linux kernel.
#
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/ir/$(CHIP_NAME)/inc
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/include/rtd6748/rtk_kdriver/tvscalercontrol/panel
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/include/rtk_kdriver

KBUILD_CFLAGS += -Werror
obj-$(CONFIG_RTK_KDRV_GPIO) += rtk_bl.o
