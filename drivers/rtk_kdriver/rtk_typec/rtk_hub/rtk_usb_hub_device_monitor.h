#ifndef __RTK_USB_HUB_DEVICE_MONITOR_H__
#define __RTK_USB_HUB_DEVICE_MONITOR_H__
#include <linux/usb.h>
bool rtk_usb_hub_add_monitor_usb_dev_path(const char *usb_dev_path);
bool rtk_usb_hub_remove_monitor_usb_dev_path(const char *usb_dev_path);
void rtk_usb_hub_dump_monitor_usb_dev_path(char *buf, unsigned int buf_len);
bool rtk_usb_hub_find_monitor_usb_dev_path(char *dev_path);
void rtk_usb_hub_usb_dev_judge_flow(struct usb_device *udev);
void rtk_usb_hub_get_monitor_usb_dev_info(void);
void send_hub_information_by_uevent(char *message);
#endif
