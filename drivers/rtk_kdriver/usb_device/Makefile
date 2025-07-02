#Copyright (C) 2024 Realtek Semiconductor Corporation.

KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/usb/include/
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/usb_device/include/
KBUILD_CPPFLAGS += -I$(srctree)/drivers/usb/
KBUILD_CPPFLAGS += -I$(srctree)/drivers/usb/dwc2/

obj-$(CONFIG_RTK_KDRV_USB_DEVICE) += rtk-usb-device.o

rtk-usb-device-objs := rtk_usb_device.o \
		rtk_usb_device_ioctl.o \
		usb/usb_buf_mgr.o \
		usb/usb_init.o \
		usb/usb_isr.o \
		usb/usb_mac.o \
		usb/usb_qevent_handler.o \
		usb/usb_utility.o \
		usb/usb_utility_2.o \
		usb/usb_cdc/usb_cdc_api.o \
		usb/usb_cdc/usb_cdc_api_tx.o \
		usb/usb_uac/usb_uac_api.o \
		usb/usb_uac/usb_uac_ctrl.o \
		usb/usb_uac/usb_uac_qevent_handler.o \
		usb/usb_uvc/usb_uvc_api.o \
		usb/usb_uvc/usb_uvc_ctrl.o \
		usb/usb_uvc/usb_uvc_qevent_handler.o \
		usb/usb_uvc/usb_uvc_utility.o \
		usb/usb_hid/usb_hid_api.o \
		usb_non_release/usb_mac_probe.o \
		usb_non_release/usb_phy.o \
		usb_non_release/usb_utility_config_bank37.o \
		usb_non_release/usb_utility_config_bank41.o \
		usb_non_release/usb_uac/usb_uac_hwo_timer.o \
		usb_non_release/usb_uvc/usb_uvc_utility_process_unit.o
		
