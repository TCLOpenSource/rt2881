#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of_platform.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/freezer.h>
#include <mach/pcbMgr.h>
#include <linux/uaccess.h>
#include <rtk_kdriver/io.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/usb2_hub_reg.h>
#include <linux/suspend.h>
#include <linux/notifier.h>
#include <rbus/usb2_top_reg.h>
#include <rtk_kdriver/rtk_gpio.h>
#include "rtk_usb_hub_wrapper.h"

void rtk_usb_hub_wrapper_clock_on(struct platform_device *pdev)
{
    // To do wrapper CLK
    rtd_outl(SYS_REG_SYS_SRST5_reg, SYS_REG_SYS_SRST5_rstn_usb_hub_mask);
    rtd_outl(SYS_REG_SYS_CLKEN5_reg, SYS_REG_SYS_CLKEN5_clken_usb_hub_mask);
    rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb_hub_wrap_mask);
    rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb_hub_wrap_mask);
    udelay(100);
    rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb_hub_wrap_mask |
								SYS_REG_SYS_SRST6_write_data_mask);
    udelay(100);
    rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb_hub_wrap_mask |
								SYS_REG_SYS_CLKEN6_write_data_mask);
    udelay(100);
}


void rtk_usb_hub_clk_on(struct platform_device *pdev)
{
    // 1. Set usb_hub_reset
    rtd_outl(SYS_REG_SYS_SRST5_reg, SYS_REG_SYS_SRST5_rstn_usb_hub_mask |
    					SYS_REG_SYS_SRST5_write_data_mask);
    mdelay(200);
    // 2. Set usb_hub_clk
    rtd_outl(SYS_REG_SYS_CLKEN5_reg, SYS_REG_SYS_CLKEN5_clken_usb_hub_mask |
    				SYS_REG_SYS_CLKEN5_write_data_mask);
}

bool rtk_usb_hub_load_phy_settings(struct platform_device *pdev)
{
    //do nothing
    return true;
}


void rtk_usb_hub_wrapper_clock_off(struct platform_device *pdev)
{
    // To do wrapper CLK
    rtd_outl(SYS_REG_SYS_SRST5_reg, SYS_REG_SYS_SRST5_rstn_usb_hub_mask);
    rtd_outl(SYS_REG_SYS_CLKEN5_reg, SYS_REG_SYS_CLKEN5_clken_usb_hub_mask);
    rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb_hub_wrap_mask);
    rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb_hub_wrap_mask);
}


void rtk_usb_hub_usp_enable(struct device *dev, bool enable)
{
	if(enable)
		rtd_outl(SYS_REG_USB_PC_DPHY_MUX_reg, rtd_inl(SYS_REG_USB_PC_DPHY_MUX_reg)
								| SYS_REG_USB_PC_DPHY_MUX_usb_pc_dphy_mux_p2_mask);
	else
		rtd_outl(SYS_REG_USB_PC_DPHY_MUX_reg, rtd_inl(SYS_REG_USB_PC_DPHY_MUX_reg)
								& (~SYS_REG_USB_PC_DPHY_MUX_usb_pc_dphy_mux_p2_mask));
}

bool rtk_usb_hub_usp_get_state(struct device *dev)
{
	if(rtd_inl(SYS_REG_USB_PC_DPHY_MUX_reg) & SYS_REG_USB_PC_DPHY_MUX_usb_pc_dphy_mux_p2_mask)
		return true;
	else
		return false;
}

#ifdef SUPPORT_VBUS_TOGGLE
static int vbus_on(bool on)
{
#if IS_ENABLED(CONFIG_RTK_KDRV_GPIO)
        return IO_Direct_Set("PIN_USB_5V_EN", on ? 1 : 0);
#else
		return -1;
#endif
}
#endif
void rtk_usb_vbus_toggle(unsigned char port)
{
#ifdef SUPPORT_VBUS_TOGGLE
	vbus_on(false);
	msleep(50);
	vbus_on(true);
#endif	
}

static bool is_usb2_host_crt_on(void)
{
	if((rtd_inl(SYS_REG_SYS_CLKEN2_reg) & SYS_REG_SYS_CLKEN2_clken_usb2_dwc_mask)
		&& (rtd_inl(SYS_REG_SYS_SRST2_reg) & SYS_REG_SYS_SRST2_rstn_usb2_dwc_mask))
		return true;
	else
		return false;
}

void rtk_usb_host_port_onoff(unsigned char port, bool onoff)
{
	if(is_usb2_host_crt_on()) {
		if(onoff) {
			if(port == 0)
				rtd_outl(0xb8013054, rtd_inl(0xb8013054) | (1 << 12));
			else if(port == 1)
				rtd_outl(0xb8013058, rtd_inl(0xb8013058) | (1 << 12));
		} else {
			if(port == 0)
				rtd_outl(0xb8013054, rtd_inl(0xb8013054) & (~(1 << 12)));
			else if(port == 1)
				rtd_outl(0xb8013058, rtd_inl(0xb8013058) & (~(1 << 12)));
		}
	}
}

static void do_usb2_phy_reset(unsigned char port,bool release)
{
	if(is_usb2_host_crt_on()) {
		switch(port) {
		case 0:
			if(release)
				rtd_outl(USB2_TOP_RESET_UTMI_reg, rtd_inl(USB2_TOP_RESET_UTMI_reg) & (~0x1));
			else
				rtd_outl(USB2_TOP_RESET_UTMI_reg, rtd_inl(USB2_TOP_RESET_UTMI_reg) | 0x1);
			break;
		case 1:
			if(release)
				rtd_outl(USB2_TOP_RESET_UTMI_2port_reg, rtd_inl(USB2_TOP_RESET_UTMI_2port_reg) & (~0x1));
			else
				rtd_outl(USB2_TOP_RESET_UTMI_2port_reg, rtd_inl(USB2_TOP_RESET_UTMI_2port_reg) | 0x1);
			break;
		default:
			break;
			
		}
	}
}

void rtk_usb_hub_specail_dsp_enable(struct device *dev, unsigned char port, bool enable)
{
	unsigned long flags;
	unsigned int set_val = enable ? 1 : 0;
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
	if(port >= MAX_HUB_TYPEA_PORT_NUM)
		return ;

	spin_lock_irqsave(&private_data->downstreamport_switch_lock, flags);
	if(atomic_read(&private_data->dwonstreamport_should_state[port]) == set_val) {
		spin_unlock_irqrestore(&private_data->downstreamport_switch_lock, flags);
		return ;
	}
#ifdef SUPPORT_VBUS_TOGGLE
	vbus_on(false);	
#endif	
	atomic_set(&private_data->dwonstreamport_should_state[port], set_val);
	spin_unlock_irqrestore(&private_data->downstreamport_switch_lock, flags);
	if(enable) {
		rtk_usb_host_port_onoff(port, false);
		udelay(500);
	}

	do_usb2_phy_reset(port, false);
	udelay(100);
	switch(port) {
	case 0:
		if(enable)
			rtd_outl(SYS_REG_USB_PC_DPHY_MUX_reg, rtd_inl(SYS_REG_USB_PC_DPHY_MUX_reg) |
								SYS_REG_USB_PC_DPHY_MUX_usb_pc_dphy_mux_p0_mask);
		else
			rtd_outl(SYS_REG_USB_PC_DPHY_MUX_reg, rtd_inl(SYS_REG_USB_PC_DPHY_MUX_reg) &
								(~SYS_REG_USB_PC_DPHY_MUX_usb_pc_dphy_mux_p0_mask));		
		break;
	case 1:
		if(enable)
			rtd_outl(SYS_REG_USB_PC_DPHY_MUX_reg, rtd_inl(SYS_REG_USB_PC_DPHY_MUX_reg) |
								SYS_REG_USB_PC_DPHY_MUX_usb_pc_dphy_mux_p1_mask);
		else
			rtd_outl(SYS_REG_USB_PC_DPHY_MUX_reg, rtd_inl(SYS_REG_USB_PC_DPHY_MUX_reg) &
								(~SYS_REG_USB_PC_DPHY_MUX_usb_pc_dphy_mux_p1_mask));		
		break;
	default:
		break;
	}

	do_usb2_phy_reset(port, true);
	if(!enable) {
		udelay(500);
		rtk_usb_host_port_onoff(port, true);
	}
}

void rtk_usb_hub_dsp_enable(struct device *dev, bool enable)
{
	rtk_usb_hub_specail_dsp_enable(dev, 0, enable);
	rtk_usb_hub_specail_dsp_enable(dev, 1, enable);
}

void rtk_usb_hub_switch_hub(struct device *dev, unsigned int hub_idx)
{
	
}

void rtk_usb_hub_post_init(struct platform_device *pdev)
{
	
}

unsigned char rtk_usb_check_mode(struct RTK_USB_HUB_PRIVATE *private_data)
{
	unsigned char mode = 0;	
	if(((rtd_inl(0xb8064430) & 0xC) == 0x4) || ((rtd_inl(0xb8064430) & 0xC) == 0x8)) {//attach
		if((rtd_inl(0xb8064394) & 0x1) == 0) // hub role
			mode = 1;
		else if(private_data->is_need_pop_bb)
			mode = 1;         
	}
	return mode;
}
