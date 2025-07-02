#ifndef __RTK_USB_HUB_WRAPPER_H__
#define __RTK_USB_HUB_WRAPPER_H__
#include "rtk_usb_hub_defines.h"
#define MAX_USB_HUB_DEVICE_NUM 1

#define CONFIG_HUB_PORT_POLL_DATA  {  \
					{3, 3, 1, 0x81, 0x06, false, false}, \
					{3, 3, 2, 0x82, 0x06, false, false}, \
				}


void rtk_usb_hub_wrapper_clock_on(struct platform_device *pdev);
void rtk_usb_hub_clk_on(struct platform_device *pdev);
bool rtk_usb_hub_load_phy_settings(struct platform_device *pdev);
void rtk_usb_hub_wrapper_clock_off(struct platform_device *pdev);
bool rtk_usb_hub_usp_get_state(struct device *dev);
void rtk_usb_hub_switch_hub(struct device *dev, unsigned int hub_idx);
void rtk_usb_hub_post_init(struct platform_device *pdev);
unsigned char rtk_usb_check_mode(struct RTK_USB_HUB_PRIVATE *private_data);
void rtk_usb_host_port_onoff(unsigned char port, bool onoff);
void rtk_usb_vbus_toggle(unsigned char port);
#endif
