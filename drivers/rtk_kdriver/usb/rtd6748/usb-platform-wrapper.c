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
		
}

void rtk_usb_low_speed_exit_settings(bool is_ehci_or_xhci_port, struct usb_hcd *hcd, int portnum)
{
		
}