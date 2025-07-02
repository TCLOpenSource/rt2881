#
# Makefile for the linux kernel.
#
	
GEN_PREBUIL_MK := $(shell $(srctree)/drivers/rtk_kdriver/mach-rtk/include/mach/compile.sh $(srctree))

ifdef CONFIG_RTK_KDRIVER_SUPPORT

KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/include
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/include/rtk_kdriver

KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/include/$(CHIP_NAME)
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/include/$(CHIP_NAME)/rtk_kdriver
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/common/include/
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/common/IPC/generate/include/system
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/common/IPC/include

endif
KBUILD_CPPFLAGS += -DLITTLE_ENDIAN
KBUILD_CPPFLAGS += -fno-unsigned-char

ifeq ($(CONFIG_RTK_8KCODEC),y)
KBUILD_CPPFLAGS += -Idrivers/rtk_codec
endif

# Common support
obj-$(CONFIG_RTK_KDRV_COMMON) += rtk-kdrv-common.o
rtk-kdrv-common-objs += common.o timer.o init.o platform.o
rtk-kdrv-common-objs += ../ip_power_down/$(CHIP_NAME)/power_down_analog.o
rtk-kdrv-common-objs += ../rmm/cache.o ../rmm/rtkcacheflush.o
rtk-kdrv-common-objs += rtk-interrupt.o

obj-$(CONFIG_RTK_KDRV_SYSFS) += rtk-sysfs.o
rtk-sysfs-objs += sysfs.o

#EXTRA_CFLAGS += -I ./

#hiberation
ifdef CONFIG_LG_SNAPSHOT_BOOT
KBUILD_CPPFLAGS += -Ikernel/power/
endif

KBUILD_CPPFLAGS += -I$(srctree)/drivers/staging/android/ion/




