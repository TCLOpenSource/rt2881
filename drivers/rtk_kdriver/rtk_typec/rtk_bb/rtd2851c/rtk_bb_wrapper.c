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
	if(on) {
		rtd_outl(SYS_REG_SYS_CLKEN5_reg, SYS_REG_SYS_CLKEN5_clken_usb_bb_mask);
		udelay(10);
		rtd_outl(SYS_REG_SYS_SRST5_reg, SYS_REG_SYS_SRST5_rstn_usb_bb_mask);
		udelay(10);
		rtd_outl(SYS_REG_SYS_CLKEN5_reg, SYS_REG_SYS_CLKEN5_clken_usb_bb_mask | SYS_REG_SYS_CLKEN5_write_data_mask);
		udelay(10);
		rtd_outl(SYS_REG_SYS_SRST5_reg, SYS_REG_SYS_SRST5_rstn_usb_bb_mask | SYS_REG_SYS_SRST5_write_data_mask);
		udelay(100);
	} else {
		rtd_outl(SYS_REG_SYS_CLKEN5_reg, SYS_REG_SYS_CLKEN5_clken_usb_bb_mask);
		udelay(10);
		rtd_outl(SYS_REG_SYS_SRST5_reg, SYS_REG_SYS_SRST5_rstn_usb_bb_mask);
	}
	
}

bool is_UsbBillboard_CRT_ON(struct USB_BB_PRIVATE_DATA *private_data)
{
	if((rtd_inl(SYS_REG_SYS_CLKEN5_reg) & SYS_REG_SYS_CLKEN5_clken_usb_bb_mask)
		&& (rtd_inl(SYS_REG_SYS_SRST5_reg) & SYS_REG_SYS_SRST5_rstn_usb_bb_mask))
		return true;
	return false;
}


void UsbBillboard_init_extra_settings(struct USB_BB_PRIVATE_DATA *private_data)
{
	private_data->share_memory_reg = 0xb80644EC;
	private_data->share_memory_reg_bit_mask = (1 << 7);
}

bool rtk_bb_is_typec_attached(struct USB_BB_PRIVATE_DATA *private_data)
{
	bool ret = false;
	unsigned int val = rtd_inl(0xb8064430) & 0xC;
	if((val == 0x4) || (val == 0x8)) {
		ret = true;
	}
	return ret;
}

bool rtk_bb_is_failed_in_typec_altmode(struct USB_BB_PRIVATE_DATA *private_data)
{
	if((rtd_inl(0xb80644c8) & _BIT7) != _BIT7)
		return true;
	else
		return false;
}