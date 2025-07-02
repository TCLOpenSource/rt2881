#ifndef __USB_PLATFORM_DEF_H__
#define __USB_PLATFORM_DEF_H__
#include <rbus/iso_misc_reg.h>
#define USB_PLATFORM_WAKE_UP_PARAM_DATA  \
{ \
	{EHCI_PORT, 0, ISO_MISC_DUMMY1_wakeup_usb2y_p2_en_shift, 0, ISO_MISC_DUMMY1_reg}, \
	{EHCI_PORT, 1, ISO_MISC_DUMMY1_wakeup_usb2y_p3_en_shift, 0, ISO_MISC_DUMMY1_reg}, \
	{EHCI_PORT, 2, ISO_MISC_DUMMY1_wakeup_usb2y_p4_en_shift, 1, ISO_MISC_DUMMY1_reg}, \
	{XHCI_PORT, 0, ISO_MISC_DUMMY1_wakeup_usb2y_p1_en_shift, 1, ISO_MISC_DUMMY1_reg}, \
} \

#define CONFIG_XHCI_USING_SPECIAL_PHY_SETTING_FLOW

#define CONFIG_USB_PLATFORM_ENABLE_SUSPEND_POWER_DOWN
#define CONFIG_USB_PLATFORM_ENABLE_SHUTDOWN_POWER_DOWN

#endif
