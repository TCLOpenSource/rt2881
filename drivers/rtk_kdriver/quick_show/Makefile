obj-$(CONFIG_RTK_KDRV_QUICKSHOW) += quick_show.o
obj-$(CONFIG_RTK_KDRV_QUICKSHOW_STATUS) += quick_show_status.o

ifneq ($(CHIP_NAME),rtd2851a)
obj-$(CONFIG_RTK_KDRV_QUICKSHOW) += source_monitor.o
endif
