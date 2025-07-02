#include <linux/acpi.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/timer.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include <linux/usb/ehci_pdriver.h>
#include <linux/completion.h>
#include <asm/delay.h>
#include <linux/mutex.h>
#include <rtk_kdriver/io.h>

#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)	
int ehci_hcd_set_phy_reg(struct usb_hcd *hcd, unsigned char port1, unsigned char page, unsigned char addr, unsigned char val);
#endif
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)	
int xhci_hcd_set_phy_reg(struct usb_hcd *hcd, unsigned char phy, unsigned char page, unsigned char addr, unsigned char val);
#endif

int rtk_usb_host_wakeup_resume_setting(void)
{
	return 0;
}

int rtk_usb_host_wakeup_suspend_setting(void)
{
	return 0;
}

void rtk_usb_low_speed_enter_settings(bool is_ehci_or_xhci_port, struct usb_hcd *hcd, int portnum)
{
	if(is_ehci_or_xhci_port) {
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)
		ehci_hcd_set_phy_reg(hcd, portnum, 0, 0xf2, 0xa0);
#endif
	} else {
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)
		//xhci_hcd_set_phy_reg(hcd, portnum, 0, 0xf2, 0xa0);
#endif
	}
}

void rtk_usb_low_speed_exit_settings(bool is_ehci_or_xhci_port, struct usb_hcd *hcd, int portnum)
{
	if(is_ehci_or_xhci_port) {
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)
		ehci_hcd_set_phy_reg(hcd, portnum, 0, 0xf2, 0x00);
#endif
	} else {
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)
		//xhci_hcd_set_phy_reg(hcd, portnum, 0, 0xf2, 0x00);
#endif
	}
}