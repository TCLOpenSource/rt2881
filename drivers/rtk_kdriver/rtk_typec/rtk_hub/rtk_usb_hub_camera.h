#ifndef __RTK_USB_HUB_CAMERA_H__
#define __RTK_USB_HUB_CAMERA_H__
int rtk_usb_hub_register_notifier(void);
void rtk_usb_hub_unregister_notifier(void);
void rtk_usb_hub_check_port(struct device *dev);
void do_check_usb_camera_on_enter_typec(struct device *dev);
void do_check_usb_camera_on_exit_typec(struct device *dev);
int rtk_usb_hub_select_cur_hub(struct device *dev, unsigned int hub_idx);
unsigned int rtk_usb_hub_get_selected_hub(void);
#endif
