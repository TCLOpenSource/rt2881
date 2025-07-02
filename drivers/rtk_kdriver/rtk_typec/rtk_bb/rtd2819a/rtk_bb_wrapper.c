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
#include <linux/suspend.h>
#include <linux/notifier.h>
#include "rtk_bb.h"
#include "rtk_bb_inc.h"

void UsbBillboard_CRT_ON_OFF(struct USB_BB_PRIVATE_DATA *private_data, bool on)
{
	switch(private_data->id) {
	case 0:
		if(on) {
			rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb_bb_2_mask);
			udelay(10);
			rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb_bb_2_mask);
			udelay(10);
			rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb_bb_2_mask | SYS_REG_SYS_CLKEN6_write_data_mask);
			udelay(10);
			rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb_bb_2_mask | SYS_REG_SYS_SRST6_write_data_mask);
			udelay(100);
		} else {
			rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb_bb_2_mask);
			udelay(10);
			rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb_bb_2_mask);
		}
		break;
	case 1:
		if(on) {
			rtd_outl(SYS_REG_SYS_CLKEN4_reg, SYS_REG_SYS_CLKEN4_clken_usb_bb_mask);
			udelay(10);
			rtd_outl(SYS_REG_SYS_SRST4_reg, SYS_REG_SYS_SRST4_rstn_usb_bb_mask);
			udelay(10);
			rtd_outl(SYS_REG_SYS_CLKEN4_reg, SYS_REG_SYS_CLKEN4_clken_usb_bb_mask | SYS_REG_SYS_CLKEN4_write_data_mask);
			udelay(10);
			rtd_outl(SYS_REG_SYS_SRST4_reg, SYS_REG_SYS_SRST4_rstn_usb_bb_mask | SYS_REG_SYS_SRST4_write_data_mask);
			udelay(100);
		} else {
			rtd_outl(SYS_REG_SYS_CLKEN4_reg, SYS_REG_SYS_CLKEN4_clken_usb_bb_mask);
			udelay(10);
			rtd_outl(SYS_REG_SYS_SRST4_reg, SYS_REG_SYS_SRST4_rstn_usb_bb_mask);
		}
		break;
	default:
		break;
	}
	if(private_data->id == 0) {
        if(on)
			rtd_outl(SYS_REG_INT_CTRL_SCPU_3_reg, SYS_REG_INT_CTRL_SCPU_3_usb_bb2_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_3_write_data_mask);
        else
			rtd_outl(SYS_REG_INT_CTRL_SCPU_3_reg, SYS_REG_INT_CTRL_SCPU_3_usb_bb2_int_scpu_routing_en_mask);

	} else {
		if(on)
			rtd_outl(SYS_REG_INT_CTRL_SCPU_3_reg, SYS_REG_INT_CTRL_SCPU_3_usb_bb1_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_3_write_data_mask);
        else
			rtd_outl(SYS_REG_INT_CTRL_SCPU_3_reg, SYS_REG_INT_CTRL_SCPU_3_usb_bb1_int_scpu_routing_en_mask);
	}	
	
}

bool is_UsbBillboard_CRT_ON(struct USB_BB_PRIVATE_DATA *private_data)
{
	switch(private_data->id) {
	case 0:
		if((rtd_inl(SYS_REG_SYS_CLKEN6_reg) & SYS_REG_SYS_CLKEN6_clken_usb_bb_2_mask)
			&& (rtd_inl(SYS_REG_SYS_SRST6_reg) & SYS_REG_SYS_SRST6_rstn_usb_bb_2_mask))
			return true;
		break;
	case 1:
		if((rtd_inl(SYS_REG_SYS_CLKEN4_reg) & SYS_REG_SYS_CLKEN4_clken_usb_bb_mask)
			&& (rtd_inl(SYS_REG_SYS_SRST4_reg) & SYS_REG_SYS_SRST4_rstn_usb_bb_mask))
			return true;
	default:
		break;
	}
	return false;
}

void UsbBillboard_init_extra_settings(struct USB_BB_PRIVATE_DATA *private_data)
{
	if(private_data->id == 0) {
		private_data->share_memory_reg = 0xb80644EC;
		private_data->share_memory_reg_bit_mask = (1 << 7);
	} else {
		private_data->share_memory_reg = 0xB8064CEC;
		private_data->share_memory_reg_bit_mask = (1 << 7);
	}
}


bool rtk_bb_is_typec_attached(struct USB_BB_PRIVATE_DATA *private_data)
{
	bool ret = false;
	if(private_data->id == 0) {
		unsigned int val = rtd_inl(0xb8064430) & 0xC;
		if((val == 0x4) || (val == 0x8)) {
			ret = true;
		}
	} else {
		unsigned int val = rtd_inl(0xb8064C30) & 0xC;
		if((val == 0x4) || (val == 0x8)) {
			ret = true;
		}
	}
	return ret;
}

bool rtk_bb_is_failed_in_typec_altmode(struct USB_BB_PRIVATE_DATA *private_data)
{
	if(private_data->id == 0) {
		if((rtd_inl(0xb80644c8) & _BIT7) != _BIT7)
			return true;
		else
			return false;
	} else {
		if((rtd_inl(0xb8064cc8) & _BIT7) != _BIT7)
			return true;
		else
			return false;		
	}
}