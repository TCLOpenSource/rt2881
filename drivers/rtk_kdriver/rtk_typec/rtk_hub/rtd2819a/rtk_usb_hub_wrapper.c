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
#include <rbus/usb2_hub_c2_reg.h>
#include <linux/suspend.h>
#include <linux/notifier.h>
#include <rbus/usb2_top_reg.h>
#include <rtk_kdriver/rtk_gpio.h>
#include "rtk_usb_hub_defines.h"
#include "rtk_usb_hub_wrapper.h"

//static DEFINE_SPINLOCK(g_hub_switch_lock);

void rtk_usb_hub_wrapper_clock_on(struct platform_device *pdev)
{
    struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
	if(private_data->id == 0) {
		rtd_outl(SYS_REG_SYS_SRST4_reg, SYS_REG_SYS_SRST4_rstn_usb_rbus_mask | SYS_REG_SYS_SRST4_write_data_mask);
		rtd_outl(SYS_REG_SYS_CLKEN4_reg, SYS_REG_SYS_CLKEN4_clken_usb_rbus_mask| SYS_REG_SYS_CLKEN4_write_data_mask);
		
		rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb_hub_2_mask);
	    rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb_hub_2_mask);
	    rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb_hub_wrap_2_mask);
	    rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb_hub_wrap_2_mask);
	    udelay(100);
	    rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb_hub_wrap_2_mask |
									SYS_REG_SYS_SRST6_write_data_mask);
	    udelay(100);
	    rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb_hub_wrap_2_mask |
									SYS_REG_SYS_CLKEN6_write_data_mask);
	    udelay(100);
	} else {
	    rtd_outl(SYS_REG_SYS_SRST4_reg, SYS_REG_SYS_SRST4_rstn_usb_hub_mask);
	    rtd_outl(SYS_REG_SYS_CLKEN4_reg, SYS_REG_SYS_CLKEN4_clken_usb_hub_mask);
	    rtd_outl(SYS_REG_SYS_SRST4_reg, SYS_REG_SYS_SRST4_rstn_usb_hub_wrap_mask);
	    rtd_outl(SYS_REG_SYS_CLKEN4_reg, SYS_REG_SYS_CLKEN4_clken_usb_hub_wrap_mask);
	    udelay(100);
	    rtd_outl(SYS_REG_SYS_SRST4_reg, SYS_REG_SYS_SRST4_rstn_usb_hub_wrap_mask |
									SYS_REG_SYS_SRST4_write_data_mask);
	    udelay(100);
	    rtd_outl(SYS_REG_SYS_CLKEN4_reg, SYS_REG_SYS_CLKEN4_clken_usb_hub_wrap_mask |
									SYS_REG_SYS_CLKEN4_write_data_mask);
	    udelay(100);
	}
	

}


void rtk_usb_hub_clk_on(struct platform_device *pdev)
{
	struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
	if(private_data->id == 0) {
	    // 1. Set usb_hub_reset
	    rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb_hub_2_mask |
	    					SYS_REG_SYS_SRST6_write_data_mask);
	    mdelay(50);
	    // 2. Set usb_hub_clk
	    rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb_hub_2_mask |
	    				SYS_REG_SYS_CLKEN6_write_data_mask);
	} else {
	    rtd_outl(SYS_REG_SYS_SRST4_reg, SYS_REG_SYS_SRST4_rstn_usb_hub_mask |
	    					SYS_REG_SYS_SRST4_write_data_mask);
	    mdelay(50);
	    rtd_outl(SYS_REG_SYS_CLKEN4_reg, SYS_REG_SYS_CLKEN4_clken_usb_hub_mask |
	    				SYS_REG_SYS_CLKEN4_write_data_mask);
	}
}

bool rtk_usb_hub_load_phy_settings(struct platform_device *pdev)
{
    //do nothing
    return true;
}


void rtk_usb_hub_wrapper_clock_off(struct platform_device *pdev)
{
	struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
	if(private_data->id == 0) {
	    // To do wrapper CLK
	    rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb_hub_2_mask);
	    rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb_hub_2_mask);
	    rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb_hub_wrap_2_mask);
	    rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb_hub_wrap_2_mask);
	} else {
	    // To do wrapper CLK
	    rtd_outl(SYS_REG_SYS_SRST4_reg, SYS_REG_SYS_SRST4_rstn_usb_hub_mask);
	    rtd_outl(SYS_REG_SYS_CLKEN4_reg, SYS_REG_SYS_CLKEN4_clken_usb_hub_mask);
	    rtd_outl(SYS_REG_SYS_SRST4_reg, SYS_REG_SYS_SRST4_rstn_usb_hub_wrap_mask);
	    rtd_outl(SYS_REG_SYS_CLKEN4_reg, SYS_REG_SYS_CLKEN4_clken_usb_hub_wrap_mask);
	}
}


void rtk_usb_hub_usp_enable(struct device *dev, bool enable)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
    struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);

	if(private_data->id == 0) {
		if(enable)
			rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg)
									| SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p4_mask);
		else
			rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg)
									& (~SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p4_mask));
	} else {
		if(enable)
			rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg)
									| SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p1_mask);
		else
			rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg)
									& (~SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p1_mask));
	}
}

bool rtk_usb_hub_usp_get_state(struct device *dev)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
    struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
	if(private_data->id == 0) {
		if(rtd_inl(SYS_REG_SYS_CLKDIV_reg) & SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p4_mask)
			return true;
		else
			return false;
	} else {
		if(rtd_inl(SYS_REG_SYS_CLKDIV_reg) & SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p1_mask)
			return true;
		else
			return false;
	}
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
	msleep(200);
	vbus_on(true);
#endif	
}

static bool is_usb2_host_crt_on(void)
{
    bool is_mac_clk_on = false, is_mac_release = false;
    unsigned int clk = 0, mac_rst = 0;
   
    clk = rtd_inl(SYS_REG_SYS_CLKEN2_reg);
    if ((clk & SYS_REG_SYS_CLKEN2_clken_usb2_dwc_mask)
	 && (clk & SYS_REG_SYS_CLKEN2_clken_usb2_wrapper_mask)){
        is_mac_clk_on = true;
    }

    mac_rst = rtd_inl(SYS_REG_SYS_SRST2_reg);
    if ((mac_rst & SYS_REG_SYS_SRST2_rstn_usb2_dwc_mask)
		&& (mac_rst & SYS_REG_SYS_SRST2_rstn_usb2_wrapper_mask)) {
        is_mac_release = true;
    }
	
	if (is_mac_clk_on && is_mac_release)
		return true;
	else
		return false;
}

void rtk_usb_host_port_onoff(unsigned char port, bool onoff)
{
	if(is_usb2_host_crt_on()) {
		if(onoff) {
			if(port == 0)
				rtd_outl(0xb8012054, rtd_inl(0xb8012054) | (1 << 12));
			else if(port == 1)
				rtd_outl(0xb8012058, rtd_inl(0xb8012058) | (1 << 12));
		} else {
			if(port == 0)
				rtd_outl(0xb8012054, rtd_inl(0xb8012054) & (~(1 << 12)));
			else if(port == 1)
				rtd_outl(0xb8012058, rtd_inl(0xb8012058) & (~(1 << 12)));
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
	if(private_data->id == 0) {
		switch(port) {
		case 0:
			if(enable)
				rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg) |
									SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p2_mask);
			else
				rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg) &
									(~SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p2_mask));		
			break;
		case 1:
			if(enable)
				rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg) |
									SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p3_mask);
			else
				rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg) &
									(~SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p3_mask));		
			break;
		default:
			break;
		}		
	} else {
		switch(port) {
		case 0:
			if(enable)
				rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg) |
									SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p2_mask);
			else
				rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg) &
									(~SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p2_mask));		
			break;
		case 1:
			if(enable)
				rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg) |
									SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p3_mask);
			else
				rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg) &
									(~SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p3_mask));		
			break;
		default:
			break;
		}
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
	unsigned int  hub_upstream_mask =  0;
	if(hub_idx >= MAX_USB_HUB_DEVICE_NUM)
		return;

	hub_upstream_mask = (rtd_inl(SYS_REG_SYS_CLKDIV_reg) & (SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p4_mask | SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p1_mask));
	rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg) & (~(SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p4_mask | SYS_REG_SYS_CLKDIV_usb_pc_dphy_mux_p1_mask)));
	mdelay(10);
	if(hub_idx == 0) {
		rtd_outl(USB2_HUB_C2_USB_HUB_INT_OPTION0_reg,0x08400000);
	      	rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) | (SYS_REG_USB_CTRL_hub1_port_sel_cur_in_mask));
	      	rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) | (SYS_REG_USB_CTRL_hub1_port_sel_nxt_in_mask));
	  	  	udelay(200); // wait host sof for internal disconnection
			//rtd_outl(0xb800010c,0x000000c0);
	        //P2->C2
	        rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) | SYS_REG_USB_CTRL_hub1_port_sel_1_mask);
	        rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) &(~SYS_REG_USB_CTRL_hub2_port_sel_nxt_in_mask));
	        rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) | SYS_REG_USB_CTRL_hub1_port_sel_0_mask);
	        //P3->C2
	        rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) &(~SYS_REG_USB_CTRL_hub2_port_sel_1_mask));
	        rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) &(~(SYS_REG_USB_CTRL_hub2_port_sel_cur_in_mask)));
	        rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) &(~(SYS_REG_USB_CTRL_hub2_port_sel_0_mask)));
			//rtd_outl(0xb800010c,0x000000c1);

	} else {
		rtd_outl(USB2_HUB_USB_HUB_INT_OPTION0_reg,0x08400000);
	       	rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) | (SYS_REG_USB_CTRL_hub2_port_sel_cur_in_mask));
	        rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) | (SYS_REG_USB_CTRL_hub2_port_sel_nxt_in_mask));
	    	udelay(200); // wait host sof for internal disconnection
		//rtd_outl(0xb800010c,0x000000c0);
	        //P2->C1
	        rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) & (~SYS_REG_USB_CTRL_hub1_port_sel_1_mask));
	        rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) &(~SYS_REG_USB_CTRL_hub1_port_sel_cur_in_mask));
	        rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) &(~SYS_REG_USB_CTRL_hub1_port_sel_0_mask));
			//P3->C1
			rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) | SYS_REG_USB_CTRL_hub2_port_sel_1_mask);   // try
			rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) | SYS_REG_USB_CTRL_hub2_port_sel_0_mask);
	      	rtd_outl(SYS_REG_USB_CTRL_reg, rtd_inl(SYS_REG_USB_CTRL_reg) &(~SYS_REG_USB_CTRL_hub1_port_sel_nxt_in_mask));
			//rtd_outl(0xb800010c,0x000000c1);
	}
	rtd_outl(SYS_REG_SYS_CLKDIV_reg, rtd_inl(SYS_REG_SYS_CLKDIV_reg)| hub_upstream_mask);
}


void rtk_usb_hub_post_init(struct platform_device *pdev)
{
    struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
	if(private_data->id == 0)
		rtd_outl(USB2_HUB_C2_HUB_CTRL0_reg, rtd_inl(USB2_HUB_C2_HUB_CTRL0_reg) | USB2_HUB_C2_HUB_CTRL0_reg_force_pll_on_mask);
	else
		rtd_outl(USB2_HUB_HUB_CTRL0_reg, rtd_inl(USB2_HUB_HUB_CTRL0_reg) | USB2_HUB_HUB_CTRL0_reg_force_pll_on_mask);
}

unsigned char rtk_usb_check_mode(struct RTK_USB_HUB_PRIVATE *private_data)
{
	unsigned char mode = 0;	
	if(private_data->id == 0) {
		if(((rtd_inl(0xb8064430) & 0xC) == 0x4) || ((rtd_inl(0xb8064430) & 0xC) == 0x8)) {//attach
			if((rtd_inl(0xb8064394) & 0x1) == 0) // hub role
				mode = 1;
			else if(private_data->is_need_pop_bb)
				mode = 1;         
		}
	} else {
		if(((rtd_inl(0xb8064C30) & 0xC) == 0x4) || ((rtd_inl(0xb8064C30) & 0xC) == 0x8)) {//attach
			if((rtd_inl(0xb8064B94) & 0x1) == 0) // hub role
				mode = 1;
			else if(private_data->is_need_pop_bb)
				mode = 1;         
		}
	}
	return mode;
}