#ifndef __USB_PLATFORM_DEF_H__
#define __USB_PLATFORM_DEF_H__
#include <rbus/iso_misc_reg.h>

#define CONFIG_USB_PLATFORM_ENABLE_SUSPEND_POWER_DOWN
#define CONFIG_USB_PLATFORM_ENABLE_SHUTDOWN_POWER_DOWN


#define USB_PLATFORM_WAKE_UP_PARAM_DATA  \
{ \
	{EHCI_PORT, 0, 5, 0, ISO_MISC_DUMMY1_reg}, \
	{EHCI_PORT, 1, 4, 0, ISO_MISC_DUMMY1_reg}, \
	{EHCI_PORT, 2, 3, 1, ISO_MISC_DUMMY1_reg}, \
} \



#endif
