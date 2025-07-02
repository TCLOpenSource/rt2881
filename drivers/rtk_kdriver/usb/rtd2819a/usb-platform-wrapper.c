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

static bool g_need_do_wakeup_p1_flow = false;
static bool g_need_do_wakeup_p4_flow = false;

static int rtk_usb_host_wakeup_resume_port4_setting(void)
{
	printk(KERN_ERR"%s,%d\n", __FUNCTION__, __LINE__);
	/////// change page0 //////
	rtd_outl(0xb8012f04,0x10001);    // change page0
	rtd_outl(0xb8012f04,0x049b1);
	rtd_outl(0xb8012f04,0x0f9b1);
	/////// force dp/dm //////
	rtd_outl(0xb8012f04,0x10001);
	rtd_outl(0xb8012f04,0x02281);
	rtd_outl(0xb8012f04,0x0f281);
	rtd_outl(0xb8012f04,0x10001);
	rtd_outl(0xb8012f04,0x02bc1);
	rtd_outl(0xb8012f04,0x0fbc1);

	//================release latch==================
	/////// change page2 //////
	rtd_outl(0xb8012f04,0x10001);    // change page2
	rtd_outl(0xb8012f04,0x04db1);
	rtd_outl(0xb8012f04,0x0fdb1);
	/////// latch //////
	rtd_outl(0xb8012f04,0x10001);
	rtd_outl(0xb8012f04,0x050f1);
	rtd_outl(0xb8012f04,0x0e0f1);


	//==================remote wakeup(K signal)==================
	/////// 0xF4 //////
	rtd_outl(0xb8012f04,0x10001);  //Select Page0
	rtd_outl(0xb8012f04,0x049b1);
	rtd_outl(0xb8012f04,0x0f9b1);
	/////// 0xF6 //////
	rtd_outl(0xb8012f04,0x10001);       //bit7=1.bit[5:4]=0x1
	rtd_outl(0xb8012f04,0x06901);
	rtd_outl(0xb8012f04,0x0f901);
	/////// 0xF6 //////
	rtd_outl(0xb8012f04,0x10001);       //bit7=0.bit[5:4]=0x0
	rtd_outl(0xb8012f04,0x06001);
	rtd_outl(0xb8012f04,0x0f001);

	//==================release dp/dm==================
	/////// change page0 //////
	rtd_outl(0xb8012f04,0x10001);    // change page0
	rtd_outl(0xb8012f04,0x049b1);
	rtd_outl(0xb8012f04,0x0f9b1);
	/////// 0xF2 //////
	rtd_outl(0xb8012f04,0x10001);    
	rtd_outl(0xb8012f04,0x02001);
	rtd_outl(0xb8012f04,0x0f001);
		return 0;
}

static int rtk_usb_host_wakeup_resume_port1_setting(void)
{

		return 0;
}


int rtk_usb_host_wakeup_resume_setting(void)
{

	rtd_outl(SYS_REG_SYS_SRST4_reg, 
					SYS_REG_SYS_SRST4_rstn_usb_rbus_mask
					| SYS_REG_SYS_SRST4_write_data_mask);

	rtd_outl(SYS_REG_SYS_CLKEN4_reg, 
				SYS_REG_SYS_CLKEN4_clken_usb_rbus_mask
				| SYS_REG_SYS_CLKEN4_write_data_mask);

	rtd_outl(SYS_REG_SYS_SRST3_reg,
						SYS_REG_SYS_SRST3_rstn_usb2_phy_p3_mask |
						SYS_REG_SYS_SRST3_write_data_mask);

	rtd_outl(SYS_REG_SYS_SRST5_reg,
					SYS_REG_SYS_SRST5_rstn_usb2_phy_p4_mask |
					SYS_REG_SYS_SRST5_write_data_mask);

	if(g_need_do_wakeup_p4_flow) {
		if(rtd_inl(SYS_REG_SYS_SRST5_reg) & SYS_REG_SYS_SRST5_rstn_usb2_phy_p4_mask)
			rtk_usb_host_wakeup_resume_port4_setting();
		g_need_do_wakeup_p4_flow = false;
	}
	if(g_need_do_wakeup_p1_flow) {
		if(rtd_inl(SYS_REG_SYS_SRST3_reg) & SYS_REG_SYS_SRST3_rstn_usb2_phy_p1_mask)
			rtk_usb_host_wakeup_resume_port1_setting();
		g_need_do_wakeup_p1_flow = false;
	}
	
	return 0;
}

int rtk_usb_host_wakeup_suspend_port4_setting(void)
{
	printk(KERN_ERR"%s,%d\n", __FUNCTION__, __LINE__);
	rtd_outl(0xb8012f04,0x10001);
	rtd_outl(0xb8012f04,0x04db1);
	rtd_outl(0xb8012f04,0x0fdb1);
	/////// latch ///////
	rtd_outl(0xb8012f04,0x10001);
	rtd_outl(0xb8012f04,0x05001);
	rtd_outl(0xb8012f04,0x0e001);
	return 0;
}

int rtk_usb_host_wakeup_suspend_port1_setting(void)
{
		return 0;
}

int rtk_usb_host_wakeup_suspend_setting(void)
{
	rtd_outl(SYS_REG_INT_CTRL_SCPU_reg,
						SYS_REG_INT_CTRL_SCPU_usb2_int_scpu_routing_en_mask);
	rtd_outl(SYS_REG_INT_CTRL_SCPU_reg,
							SYS_REG_INT_CTRL_SCPU_usb3_int_scpu_routing_en_mask |
							SYS_REG_INT_CTRL_SCPU_write_data(0));
	
	if(rtd_inl(SYS_REG_SYS_SRST5_reg) & SYS_REG_SYS_SRST5_rstn_usb2_phy_p4_mask) {
		rtk_usb_host_wakeup_suspend_port4_setting();
		g_need_do_wakeup_p4_flow = true;
	}

	if(rtd_inl(SYS_REG_SYS_SRST3_reg) & SYS_REG_SYS_SRST3_rstn_usb2_phy_p1_mask) {
		rtk_usb_host_wakeup_suspend_port1_setting();
		g_need_do_wakeup_p1_flow = true;
	}
	return 0;
}

#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)	
int ehci_hcd_set_phy_reg(struct usb_hcd *hcd, unsigned char port1, unsigned char page, unsigned char addr, unsigned char val);
#endif
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)	
int xhci_hcd_set_phy_reg(struct usb_hcd *hcd, unsigned char phy, unsigned char page, unsigned char addr, unsigned char val);
#endif

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