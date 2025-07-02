/******************************************************************************
 *
 *   Copyright(c) 2024 Realtek Semiconductor Corp. All rights reserved.
 *
 *   @author 
 *
 *****************************************************************************/
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <mach/rtk_platform.h>
#include <linux/kthread.h>
#include <linux/pageremap.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <asm/cacheflush.h>

#include <rtk_kdriver/rtk_crt.h>
#include <rtk_kdriver/io.h>
#include <rtk_kdriver/rtk_otp_region_api.h>

#include <rbus/sys_reg_reg.h>
#include <rbus/pll_reg_reg.h>
#include <rbus/uvc_vgip_reg.h>
#include <rbus/pll27x_reg_reg.h>
#include <rbus/pinmux_lvdsphy_reg.h>

#include "mach.h"
#include "usb_mac.h"
#include "usb_reg.h"

#include "rtk_usb_device_dbg.h"
#include "rtk_usb_device.h"
#include "rtk_usb_device_ioctl.h"

extern struct dwc3 g_rtk_dwc3;

#define RTK_USB_DEVICE_DRIVER_NAME "rtk_usb_device"

//#define USE_DANTE_LOOP
#define USE_DWC3_INTERRUPT

#if defined CONFIG_ARCH_RTK6702
    UINT32 dwc3_offset     = 0xb8058000;
    UINT32 usb_wrap_offset = 0xb805a000;
#elif defined CONFIG_ARCH_RTK2885P
    UINT32 dwc3_offset     = 0xb80ac000;
    UINT32 usb_wrap_offset = 0xb80d9000;
#elif defined CONFIG_ARCH_RTK2819A
    UINT32 dwc3_offset     = 0xb80ac000;
    UINT32 usb_wrap_offset = 0xb80d9000;
#else
    UINT32 dwc3_offset     = 0xb8058000;
    UINT32 usb_wrap_offset = 0xb805a000;
#endif

/*-----------------------------------------------------------------------------------
 * File Operations
 *------------------------------------------------------------------------------------*/

RTK_USB_DEVICE_DRV_T* pUsb_device_drv;

UINT32 rtk_usb_device_log_onoff;
unsigned long rtk_usb_device_dbg_log_level_mask = RTK_USB_DEVICE_LOG_LEVEL_ERR | RTK_USB_DEVICE_LOG_LEVEL_WARNING | RTK_USB_DEVICE_LOG_LEVEL_DEBUG;
unsigned long rtk_usb_device_dbg_log_block_mask = RTK_USB_DEVICE_LOG_BLOCK_ALWAYS;

void dwc3_gadget_uboot_handle_interrupt_ex(void);
UINT8 usb_driver_init(void);
UINT8 usb_uvc_init(void);
void usb_dwc3_init_uac(void);
void pattern_gen_one_frame(void);
void usb_output_one_frame(void);
int dante_loop_real(void *data);
int dante_loop(void *data);

int rtk_usb_device_open(struct inode *inode, struct file *file)
{
        file->private_data = (void*) pUsb_device_drv;
        RTK_USB_DEVICE_DBG_SIMPLE("func %s, line %d fp= %p\n", __func__, __LINE__, file);
        return 0;
}

int rtk_usb_device_release(struct inode *inode, struct file *file)
{
        return 0;
}

#ifdef CONFIG_COMPAT
long compat_rtk_usb_device_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        void __user *compat_arg = compat_ptr(arg);
        return rtk_usb_device_ioctl(filp, cmd, (unsigned long)compat_arg);
}
#endif


static struct file_operations rtk_usb_device_fops = {
        .unlocked_ioctl = rtk_usb_device_ioctl,
#ifdef CONFIG_COMPAT
        .compat_ioctl   = compat_rtk_usb_device_ioctl,
#endif

        .open           = rtk_usb_device_open,
        .release        = rtk_usb_device_release,
};

/***************************************************************************
         ------------------- Power Management ----------------
****************************************************************************/
#ifdef CONFIG_PM

static void rtk_usb_device_shutdown(struct platform_device *pdev)
{
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_shutdown\n");
}

static int rtk_usb_device_suspend(struct device *dev)
{
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_suspend\n");
#if 0
        rtk_uac_disconnect_flow();
        rtk_uvc_disconnect_flow();
#endif
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_suspend finish\n");
        return 0;
}

static int rtk_usb_device_resume(struct device *dev)
{
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_resume\n");
#if 0
        usb_driver_init();
        usb_uvc_init();
        usb_dwc3_init_uac();
#endif
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_resume finish\n");
        return 0;
}

#ifdef CONFIG_HIBERNATION
static int rtk_usb_device_poweroff(struct device *dev)
{
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_poweroff\n");
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_poweroff finish\n");
        return 0;
}

static int rtk_usb_device_restore(struct device *dev)
{
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_restore\n");
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_restore finish\n");
        return 0;
}

static int rtk_usb_device_freeze(struct device *dev)
{
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_freeze \n");
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_freeze finish \n");
        return 0;
}
static int rtk_usb_device_thaw(struct device *dev)
{
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_thaw\n");
        RTK_USB_DEVICE_DBG_SIMPLE( "do rtk_usb_device_thaw finish\n");
        return 0;
}
#endif

#define RTK_USB_DEVICE_NAME    "rtk_usb_device"
#define RTK_USB_UVC_NAME       "rtk_usb_uvc"
#define RTK_USB_DEVICE_USB_IRQ "USB_DEVICE_IRQ"
#define RTK_USB_UVC_IRQ        "USB_UVC_IRQ"

irqreturn_t rtk_usb_device_interrupt(int irq, void *_dwc);
irqreturn_t rtk_usb_device_thread_interrupt(int irq, void *_dwc);
irqreturn_t rtk_usb_uvc_interrupt(int irq, void *_dwc);
irqreturn_t rtk_usb_uvc_thread_interrupt(int irq, void *_dwc);
irqreturn_t rtk_usb_uvc_vgip_interrupt(int irq, void *_dwc);
irqreturn_t rtk_usb_uvc_vgip_thread_interrupt(int irq, void *_dwc);

void create_uac_thread(void);

static int rtk_usb_device_probe(struct platform_device *pdev)
{
        struct task_struct *dante_loop_handle = NULL;
        struct device_node *usb_device_node;
        int                ret = 0;

        return 0;

#ifdef USE_DANTE_LOOP
        dante_loop_handle = kthread_run(dante_loop, NULL, "dante_loop");
#else

#if defined CONFIG_ARCH_RTK6702
        (void)ret;
#elif defined CONFIG_ARCH_RTK2885P
        (void)ret;
#else
        ret = rtk_otp_field_read_int_by_name("sw_func_uvc");

        RTK_USB_DEVICE_DBG_SIMPLE("rtk_otp_field_read_int_by_name sw_func_uvc=%d\n", ret);

        if(ret != 0) {
            RTK_USB_DEVICE_DBG("rtk usb device not in OTP support list\n");
            return 0;
        }
#endif

        spin_lock_init(&g_rtk_dwc3.usb_device_mac_lock);

        (void)usb_device_node;
#ifdef USE_DWC3_INTERRUPT
///////////////////////////////////////////////
//      request rtk device usb isr
///////////////////////////////////////////////
        rtd_outl(SYS_REG_INT_CTRL_SCPU_reg, SYS_REG_INT_CTRL_SCPU_usb3_int_scpu_routing_en_mask);
        rtd_outl(SYS_REG_INT_CTRL_SCPU_3_reg, SYS_REG_INT_CTRL_SCPU_3_usb3_drd_int_scpu_routing_en_mask);

        usb_device_node = of_find_node_by_name(NULL, RTK_USB_DEVICE_NAME);
        if (usb_device_node == NULL) {
            RTK_USB_DEVICE_DBG_SIMPLE("could not find device %s from device tree\n", RTK_USB_DEVICE_NAME);
            return -1;
        }
        
        g_rtk_dwc3.irq_usb = of_irq_get_byname(usb_device_node, RTK_USB_DEVICE_USB_IRQ);
        if (g_rtk_dwc3.irq_usb >= 0) {
            RTK_USB_DEVICE_DBG_SIMPLE("get RTK_USB_DEVICE_USB_IRQ irq %d\n", g_rtk_dwc3.irq_usb);
            ret = request_threaded_irq(g_rtk_dwc3.irq_usb, rtk_usb_device_interrupt, rtk_usb_device_thread_interrupt,
                    IRQF_SHARED, "rtk_usb_device", &g_rtk_dwc3);
            if (ret) {
                RTK_USB_DEVICE_DBG("failed to request irq #%d --> %d\n", g_rtk_dwc3.irq_usb, ret);
            }
        } else {
            RTK_USB_DEVICE_DBG("get irq %s fail\n", RTK_USB_DEVICE_USB_IRQ);
        }
///////////////////////////////////////////////
//      request rtk device usb uvc isr
///////////////////////////////////////////////
        usb_device_node = of_find_node_by_name(NULL, RTK_USB_UVC_NAME);
        if (usb_device_node == NULL) {
            RTK_USB_DEVICE_DBG_SIMPLE("could not find device %s from device tree\n", RTK_USB_UVC_NAME);
            return -1;
        }
        
        g_rtk_dwc3.irq_uvc = of_irq_get_byname(usb_device_node, RTK_USB_UVC_IRQ);
        if (g_rtk_dwc3.irq_uvc >= 0) {
            RTK_USB_DEVICE_DBG_SIMPLE("get RTK_USB_UVC_IRQ irq %d\n", g_rtk_dwc3.irq_uvc);
            ret = request_threaded_irq(g_rtk_dwc3.irq_uvc, rtk_usb_uvc_interrupt, rtk_usb_uvc_thread_interrupt,
                    IRQF_SHARED, "rtk_usb_uvc", &g_rtk_dwc3);
            if (ret) {
                RTK_USB_DEVICE_DBG("failed to request irq #%d --> %d\n", g_rtk_dwc3.irq_uvc, ret);
            }
        } else {
            RTK_USB_DEVICE_DBG("get irq %s fail\n", RTK_USB_UVC_IRQ);
        }

#endif
        usb_driver_init();
        usb_uvc_init();
        usb_dwc3_init_uac();
#ifdef USE_DWC3_INTERRUPT
        rtd_outl(SYS_REG_INT_CTRL_SCPU_reg, SYS_REG_INT_CTRL_SCPU_usb3_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_write_data_mask);
        rtd_outl(SYS_REG_INT_CTRL_SCPU_3_reg, SYS_REG_INT_CTRL_SCPU_3_usb3_drd_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_3_write_data_mask);
#endif
        (void)dante_loop_handle;
        dante_loop_handle = kthread_run(dante_loop_real, NULL, "dante_loop_real");
        create_uac_thread();
#endif
        return 0;
}

static int rtk_usb_device_remove(struct platform_device *pdev)
{
        return 0;
}

static const struct of_device_id rtk_usb_device_of_match[] = {
        {
                .compatible = "realtek,rtk_usb_device",
        },
        {},
};

MODULE_DEVICE_TABLE(of, rtk_usb_device_of_match);

static const struct dev_pm_ops rtk_usb_device_pm_ops = {
        .suspend    = rtk_usb_device_suspend,
        .resume     = rtk_usb_device_resume,
#ifdef CONFIG_HIBERNATION
        .freeze     = rtk_usb_device_freeze,
        .thaw       = rtk_usb_device_thaw,
        .poweroff   = rtk_usb_device_poweroff,
        .restore    = rtk_usb_device_restore,
#endif
};


static struct platform_driver rtk_usb_device_platform_driver = {
        .probe  = rtk_usb_device_probe,
        .remove = rtk_usb_device_remove,
        .shutdown = rtk_usb_device_shutdown,
        .driver = {
                .name         = (char *)RTK_USB_DEVICE_DRIVER_NAME,
                .bus          = &platform_bus_type,
#ifdef CONFIG_PM
                .pm           = &rtk_usb_device_pm_ops,
                .of_match_table = of_match_ptr(rtk_usb_device_of_match),
#endif
        },
};
#endif /* CONFIG_PM */

/***************************************************************************
         ------------------- module Init ----------------
****************************************************************************/
static struct miscdevice rtk_usb_device_miscdev = {
        MISC_DYNAMIC_MINOR,
        RTK_USB_DEVICE_DRIVER_NAME,
        &rtk_usb_device_fops
};

int __init rtk_usb_device_module_init(void)
{
        int result;
        RTK_USB_DEVICE_DBG( "Initial RTK USB Device module\n");

        if (misc_register(&rtk_usb_device_miscdev)) {
                pUsb_device_drv = NULL;
                return -1;
        }

        result = platform_driver_register(&rtk_usb_device_platform_driver);
        if (result) {
                RTK_USB_DEVICE_ERROR("%s: can not register platform driver, ret=%d\n", __func__, result);
                return -1;
        }

        RTK_USB_DEVICE_DBG( "Initial RTK USB Device module successed\n");
        return 0;
}

static void __exit rtk_usb_device_module_exit(void)
{
        RTK_USB_DEVICE_DBG( "[RTK USB Device] Release RTK USB Device module\n");
        platform_driver_unregister(&rtk_usb_device_platform_driver);
        misc_deregister(&rtk_usb_device_miscdev);
        RTK_USB_DEVICE_DBG( "[RTK USB Device] Release RTK USB Device module successed\n");
}



wait_queue_head_t waitq_test;

#if 1
static void ScalerTimer_DelayXms(int ms)
{
    msleep(ms);
}
#endif

#ifdef CONFIG_ARM64
typedef unsigned long UADDRESS;
#define PT_FMT_UADDR "0x%08lX"
#else
typedef unsigned int UADDRESS;
#define PT_FMT_UADDR "0x%08X"
#endif

UINT8 *EVENT_BUFFER;
UINT8 *TRB_BUFFER;
UINT8 *TRB_ARRAY_BUFFER;
UINT8 *UAC_TRB_ARRAY_BUFFER;
UINT8 *UAC_TRB_BUFFER;

UINT8 *EVENT_BUFFER_cache;
UINT8 *TRB_BUFFER_cache;
UINT8 *TRB_ARRAY_BUFFER_cache;
UINT8 *UAC_TRB_ARRAY_BUFFER_cache;
UINT8 *UAC_TRB_BUFFER_cache;

UADDRESS    EVENT_BUFFER_PHY;
UADDRESS    TRB_BUFFER_PHY;
UADDRESS    TRB_ARRAY_BUFFER_PHY;
UADDRESS    UAC_TRB_ARRAY_BUFFER_PHY;
UADDRESS    UAC_TRB_BUFFER_PHY;

#define STAGE_SETUP  0
#define STAGE_DATA   1
#define STAGE_STATUS 2

#define UAC_HOST_TO_DEVICE_BUFFER_ADDR UAC_TRB_BUFFER
#define UAC_HOST_TO_DEVICE_TRB_ADDR    UAC_TRB_ARRAY_BUFFER
#define UAC_HOST_TO_DEVICE_TRB_CNT     40

#define USB_REQ_CLASS_GET_MIN      0x82
#define USB_REQ_CLASS_GET_MAX      0x83
#define USB_REQ_CLASS_GET_RES      0x84
#define USB_REQ_CLASS_GET_LEN      0x85
#define USB_REQ_CLASS_GET_INFO     0x86
#define USB_REQ_CLASS_GET_DEF      0x87

#define USB_DT_HID_REPORT		0x22


UINT32 rtd_mem_outl(UINT8 *addr, UINT32 value) {
	UINT8 *temp;
	temp = (UINT8*)addr;
	*(temp + 0 ) = value & 0xff;
	*(temp + 1 ) = (value >> 8) & 0xff;
	*(temp + 2 ) = (value >> 16) & 0xff;
	*(temp + 3 ) = (value >> 24) & 0xff;
	return 0;
}

UINT32 rtd_mem_inl(UINT8 *addr) {
	unsigned char *temp;
	UINT32 temp2;
	temp = addr;
	temp2 = *(temp + 0);
	temp2 = temp2 + ((*(temp + 1)<<8)&0xff00);
	temp2 = temp2 + ((*(temp + 2)<<16)&0xff0000);
	temp2 = temp2 + ((*(temp + 3)<<24)&0xff000000);
	return temp2;
}

void dwc3_copy_to_memory(void *dst, void *src, UINT16 len)
{
    memcpy(dst, src, len);
}

#define DWC3_TRBCTL_ISOCHRONOUS_FIRST	DWC3_TRB_CTRL_TRBCTL(6)
#define DWC3_TRBCTL_LINK_TRB		DWC3_TRB_CTRL_TRBCTL(8)

void init_uac_host_to_device_sram_core(void){
	unsigned long long i;
	for(i= (unsigned long long)UAC_TRB_BUFFER_cache; i<(unsigned long long)UAC_TRB_ARRAY_BUFFER_cache; i=i+4) {
		rtd_mem_outl((UINT8*)i, 0x00000000);
		dmac_flush_range( (UINT8*)i, (char*)(i + 4));
		outer_flush_range( (i- (unsigned long long)UAC_TRB_BUFFER_cache)+UAC_TRB_BUFFER_PHY, (i- (unsigned long long)UAC_TRB_BUFFER_cache)+UAC_TRB_BUFFER_PHY +4);
	}


	for(i= 0;i<UAC_HOST_TO_DEVICE_TRB_CNT; i++) {
		rtd_mem_outl((UINT8*)((unsigned long long)UAC_TRB_ARRAY_BUFFER_cache + 0x0 + (UINT32)0x10*i), (UINT32)(unsigned long)UAC_TRB_BUFFER_PHY + i*0xd0);
		rtd_mem_outl((UINT8*)((unsigned long long)UAC_TRB_ARRAY_BUFFER_cache + 0x4 + (UINT32)0x10*i), 0x00000000);
		rtd_mem_outl((UINT8*)((unsigned long long)UAC_TRB_ARRAY_BUFFER_cache + 0x8 + (UINT32)0x10*i), 0xd0);
		//rtd_mem_outl(UAC_TRB_ARRAY_BUFFER + 0xc + (UINT32)0x10*i, (DWC3_TRB_CTRL_HWO | DWC3_TRBCTL_ISOCHRONOUS_FIRST | DWC3_TRB_CTRL_CSP |  DWC3_TRB_CTRL_ISP_IMI | DWC3_TRB_CTRL_IOC));
		rtd_mem_outl((UINT8*)((unsigned long long)UAC_TRB_ARRAY_BUFFER_cache + 0xc + (UINT32)0x10*i), (DWC3_TRB_CTRL_HWO | DWC3_TRBCTL_ISOCHRONOUS_FIRST | DWC3_TRB_CTRL_CSP |  DWC3_TRB_CTRL_ISP_IMI));

		dmac_flush_range( (UINT8*)((unsigned long long)UAC_TRB_ARRAY_BUFFER_cache + 0x0 + (UINT32)0x10*i), (UINT8*)((unsigned long long)UAC_TRB_ARRAY_BUFFER_cache + 0x10 + (UINT32)0x10*i)+16);
		outer_flush_range( UAC_TRB_ARRAY_BUFFER_PHY + (UINT32)0x10*i, UAC_TRB_ARRAY_BUFFER_PHY + (UINT32)0x10*i +16);

	}

	// fill link trb
	rtd_mem_outl((UINT8*)((unsigned long long)UAC_TRB_ARRAY_BUFFER_cache + UAC_HOST_TO_DEVICE_TRB_CNT* (UINT32)16 + 0), (UINT32)(unsigned long)UAC_TRB_ARRAY_BUFFER_PHY);
	rtd_mem_outl((UINT8*)((unsigned long long)UAC_TRB_ARRAY_BUFFER_cache + UAC_HOST_TO_DEVICE_TRB_CNT* (UINT32)16 + 4), 0x00000000);
	rtd_mem_outl((UINT8*)((unsigned long long)UAC_TRB_ARRAY_BUFFER_cache + UAC_HOST_TO_DEVICE_TRB_CNT* (UINT32)16 + 8), 0x00000000);
	rtd_mem_outl((UINT8*)((unsigned long long)UAC_TRB_ARRAY_BUFFER_cache + UAC_HOST_TO_DEVICE_TRB_CNT* (UINT32)16 + 12), DWC3_TRBCTL_LINK_TRB | DWC3_TRB_CTRL_HWO);

    dmac_flush_range( (UINT8*)((unsigned long long)UAC_TRB_ARRAY_BUFFER_cache + UAC_HOST_TO_DEVICE_TRB_CNT* (UINT32)16 + 0), (UINT8*)((unsigned long long)UAC_TRB_ARRAY_BUFFER_cache + UAC_HOST_TO_DEVICE_TRB_CNT* (UINT32)16 + 0)+16);
    outer_flush_range( UAC_TRB_ARRAY_BUFFER_PHY + UAC_HOST_TO_DEVICE_TRB_CNT* (UINT32)16 + 0, UAC_TRB_ARRAY_BUFFER_PHY + UAC_HOST_TO_DEVICE_TRB_CNT* (UINT32)16 + 16);

}

static const UINT8 g_dev_descriptor[18]     = {0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0xDA, 0x0B, 0x92, 0x28, 0x23, 0x02, 0x01, 0x02, 0x03, 0x01};
static const UINT8 g_config_descriptor_u2[781]     = {
    0x09, 0x02, 0x0D, 0x03, 0x02, 0x01, 0x04, 0xC0, 0x01, 0x08, 0x0B, 0x00, 0x02, 0x0E, 0x03, 0x00,
    0x05, 0x09, 0x04, 0x00, 0x00, 0x01, 0x0E, 0x01, 0x00, 0x05, 0x0D, 0x24, 0x01, 0x00, 0x01, 0x34,
    0x00, 0x00, 0x6C, 0xDC, 0x02, 0x01, 0x01, 0x12, 0x24, 0x02, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0C, 0x24, 0x05, 0x02, 0x01, 0x00, 0x40,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x09, 0x24, 0x03, 0x03, 0x01, 0x01, 0x00, 0x02, 0x00, 0x07, 0x05,
    0x81, 0x03, 0x10, 0x00, 0x08, 0x05, 0x25, 0x03, 0x10, 0x00, 0x09, 0x04, 0x01, 0x00, 0x01, 0x0E,
    0x02, 0x00, 0x06, 0x14, 0x24, 0x01, 0x07, 0xA3, 0x02, 0x82, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x24, 0x04, 0x01, 0x02, 0x59, 0x55, 0x59, 0x32,
    0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71, 0x10, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x26, 0x24, 0x05, 0x01, 0x00, 0x80, 0x02, 0x68, 0x01, 0x00, 0x40, 0x19, 0x01, 0x00,
    0xC0, 0x4B, 0x03, 0x00, 0x08, 0x07, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03, 0x2A, 0x2C, 0x0A, 0x00,
    0x40, 0x42, 0x0F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x1E, 0x24, 0x05, 0x02, 0x00, 0x80, 0x07, 0x38,
    0x04, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x48, 0x3F, 0x00, 0x40, 0x4B, 0x4C,
    0x00, 0x01, 0x40, 0x4B, 0x4C, 0x00, 0x0B, 0x24, 0x06, 0x01, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x26, 0x24, 0x07, 0x01, 0x00, 0x80, 0x02, 0x68, 0x01, 0x00, 0x40, 0x19, 0x01, 0x00, 0xC0,
    0x4B, 0x03, 0x00, 0x08, 0x07, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03, 0x2A, 0x2C, 0x0A, 0x00, 0x40,
    0x42, 0x0F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x1E, 0x24, 0x07, 0x02, 0x00, 0x80, 0x07, 0x38, 0x04,
    0x00, 0x00, 0xC2, 0x01, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x48, 0x3F, 0x00, 0x40, 0x4B, 0x4C, 0x00,
    0x01, 0x40, 0x4B, 0x4C, 0x00, 0x1B, 0x24, 0x04, 0x03, 0x02, 0x4E, 0x56, 0x31, 0x32, 0x00, 0x00,
    0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x26, 0x24, 0x05, 0x01, 0x00, 0x80, 0x02, 0x68, 0x01, 0x00, 0x40, 0x19, 0x01, 0x00, 0xC0, 0x4B,
    0x03, 0x00, 0x08, 0x07, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03, 0x2A, 0x2C, 0x0A, 0x00, 0x40, 0x42,
    0x0F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x1E, 0x24, 0x05, 0x02, 0x00, 0x80, 0x07, 0x38, 0x04, 0x00,
    0x00, 0xC2, 0x01, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x76, 0x2F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x01,
    0x40, 0x4B, 0x4C, 0x00, 0x1B, 0x24, 0x04, 0x04, 0x02, 0x49, 0x34, 0x32, 0x30, 0x00, 0x00, 0x10,
    0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x26,
    0x24, 0x05, 0x01, 0x00, 0x80, 0x02, 0x68, 0x01, 0x00, 0x40, 0x19, 0x01, 0x00, 0xC0, 0x4B, 0x03,
    0x00, 0x08, 0x07, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03, 0x2A, 0x2C, 0x0A, 0x00, 0x40, 0x42, 0x0F,
    0x00, 0x40, 0x4B, 0x4C, 0x00, 0x1E, 0x24, 0x05, 0x02, 0x00, 0x80, 0x07, 0x38, 0x04, 0x00, 0x00,
    0xC2, 0x01, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x76, 0x2F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x01, 0x40,
    0x4B, 0x4C, 0x00, 0x1B, 0x24, 0x04, 0x05, 0x02, 0x7D, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11,
    0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70, 0x18, 0x02, 0x00, 0x00, 0x00, 0x00, 0x26, 0x24,
    0x05, 0x01, 0x00, 0x80, 0x02, 0x68, 0x01, 0x00, 0x40, 0x19, 0x01, 0x00, 0xC0, 0x4B, 0x03, 0x00,
    0x08, 0x07, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03, 0x2A, 0x2C, 0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00,
    0x40, 0x4B, 0x4C, 0x00, 0x1E, 0x24, 0x05, 0x02, 0x00, 0x80, 0x07, 0x38, 0x04, 0x00, 0x00, 0xC2,
    0x01, 0x00, 0x00, 0xC2, 0x01, 0x00, 0xEC, 0x5E, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x01, 0x40, 0x4B,
    0x4C, 0x00, 0x1B, 0x24, 0x04, 0x06, 0x02, 0x7E, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11, 0x9F,
    0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x26, 0x24, 0x05,
    0x01, 0x00, 0x80, 0x02, 0x68, 0x01, 0x00, 0x40, 0x19, 0x01, 0x00, 0xC0, 0x4B, 0x03, 0x00, 0x08,
    0x07, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03, 0x2A, 0x2C, 0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x40,
    0x4B, 0x4C, 0x00, 0x1E, 0x24, 0x05, 0x02, 0x00, 0x80, 0x07, 0x38, 0x04, 0x00, 0x00, 0xC2, 0x01,
    0x00, 0x00, 0xC2, 0x01, 0x00, 0x90, 0x7E, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x01, 0x40, 0x4B, 0x4C,
    0x00, 0x1B, 0x24, 0x04, 0x07, 0x02, 0x50, 0x30, 0x31, 0x30, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00,
    0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71, 0x18, 0x02, 0x00, 0x00, 0x00, 0x00, 0x26, 0x24, 0x05, 0x01,
    0x00, 0x80, 0x02, 0x68, 0x01, 0x00, 0x40, 0x19, 0x01, 0x00, 0xC0, 0x4B, 0x03, 0x00, 0x08, 0x07,
    0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03, 0x2A, 0x2C, 0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x40, 0x4B,
    0x4C, 0x00, 0x1E, 0x24, 0x05, 0x02, 0x00, 0x80, 0x07, 0x38, 0x04, 0x00, 0x00, 0xC2, 0x01, 0x00,
    0x00, 0xC2, 0x01, 0x00, 0xEC, 0x5E, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x01, 0x40, 0x4B, 0x4C, 0x00,
    0x06, 0x24, 0x0D, 0x01, 0x01, 0x04, 0x07, 0x05, 0x82, 0x02, 0x00, 0x02, 0x01,
};

#if 1
static const UINT8 g_config_descriptor_u3[793]     = {
    0x09, 0x02, 0x19, 0x03, 0x02, 0x01, 0x04, 0xC0, 0x00, 0x08, 0x0B, 0x00, 0x02, 0x0E, 0x03, 0x00,
    0x05, 0x09, 0x04, 0x00, 0x00, 0x01, 0x0E, 0x01, 0x00, 0x05, 0x0D, 0x24, 0x01, 0x00, 0x01, 0x34,
    0x00, 0x00, 0x6C, 0xDC, 0x02, 0x01, 0x01, 0x12, 0x24, 0x02, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0C, 0x24, 0x05, 0x02, 0x01, 0x00, 0x40,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x09, 0x24, 0x03, 0x03, 0x01, 0x01, 0x00, 0x02, 0x00, 0x07, 0x05,
    0x81, 0x03, 0x10, 0x00, 0x08, 0x06, 0x30, 0x0F, 0x00, 0x00, 0x00, 0x05, 0x25, 0x03, 0x10, 0x00,
    0x09, 0x04, 0x01, 0x00, 0x01, 0x0E, 0x02, 0x00, 0x06, 0x14, 0x24, 0x01, 0x07, 0xA3, 0x02, 0x82,
    0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x24, 0x04,
    0x01, 0x02, 0x59, 0x55, 0x59, 0x32, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38,
    0x9B, 0x71, 0x10, 0x02, 0x00, 0x00, 0x00, 0x00, 0x26, 0x24, 0x05, 0x01, 0x00, 0x80, 0x02, 0x68,
    0x01, 0x00, 0x40, 0x19, 0x01, 0x00, 0xC0, 0x4B, 0x03, 0x00, 0x08, 0x07, 0x00, 0x2A, 0x2C, 0x0A,
    0x00, 0x03, 0x2A, 0x2C, 0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x1E, 0x24,
    0x05, 0x02, 0x00, 0x80, 0x07, 0x38, 0x04, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x00, 0xC2, 0x01, 0x00,
    0x48, 0x3F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x01, 0x40, 0x4B, 0x4C, 0x00, 0x0B, 0x24, 0x06, 0x01,
    0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x26, 0x24, 0x07, 0x01, 0x00, 0x80, 0x02, 0x68, 0x01,
    0x00, 0x40, 0x19, 0x01, 0x00, 0xC0, 0x4B, 0x03, 0x00, 0x08, 0x07, 0x00, 0x2A, 0x2C, 0x0A, 0x00,
    0x03, 0x2A, 0x2C, 0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x1E, 0x24, 0x07,
    0x02, 0x00, 0x80, 0x07, 0x38, 0x04, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x48,
    0x3F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x01, 0x40, 0x4B, 0x4C, 0x00, 0x1B, 0x24, 0x04, 0x03, 0x02,
    0x4E, 0x56, 0x31, 0x32, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71,
    0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x26, 0x24, 0x05, 0x01, 0x00, 0x80, 0x02, 0x68, 0x01, 0x00,
    0x40, 0x19, 0x01, 0x00, 0xC0, 0x4B, 0x03, 0x00, 0x08, 0x07, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03,
    0x2A, 0x2C, 0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x1E, 0x24, 0x05, 0x02,
    0x00, 0x80, 0x07, 0x38, 0x04, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x76, 0x2F,
    0x00, 0x40, 0x4B, 0x4C, 0x00, 0x01, 0x40, 0x4B, 0x4C, 0x00, 0x1B, 0x24, 0x04, 0x04, 0x02, 0x49,
    0x34, 0x32, 0x30, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71, 0x0C,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x26, 0x24, 0x05, 0x01, 0x00, 0x80, 0x02, 0x68, 0x01, 0x00, 0x40,
    0x19, 0x01, 0x00, 0xC0, 0x4B, 0x03, 0x00, 0x08, 0x07, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03, 0x2A,
    0x2C, 0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x1E, 0x24, 0x05, 0x02, 0x00,
    0x80, 0x07, 0x38, 0x04, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x76, 0x2F, 0x00,
    0x40, 0x4B, 0x4C, 0x00, 0x01, 0x40, 0x4B, 0x4C, 0x00, 0x1B, 0x24, 0x04, 0x05, 0x02, 0x7D, 0xEB,
    0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11, 0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70, 0x18, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x26, 0x24, 0x05, 0x01, 0x00, 0x80, 0x02, 0x68, 0x01, 0x00, 0x40, 0x19,
    0x01, 0x00, 0xC0, 0x4B, 0x03, 0x00, 0x08, 0x07, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03, 0x2A, 0x2C,
    0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x1E, 0x24, 0x05, 0x02, 0x00, 0x80,
    0x07, 0x38, 0x04, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x00, 0xC2, 0x01, 0x00, 0xEC, 0x5E, 0x00, 0x40,
    0x4B, 0x4C, 0x00, 0x01, 0x40, 0x4B, 0x4C, 0x00, 0x1B, 0x24, 0x04, 0x06, 0x02, 0x7E, 0xEB, 0x36,
    0xE4, 0x4F, 0x52, 0xCE, 0x11, 0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70, 0x20, 0x02, 0x00,
    0x00, 0x00, 0x00, 0x26, 0x24, 0x05, 0x01, 0x00, 0x80, 0x02, 0x68, 0x01, 0x00, 0x40, 0x19, 0x01,
    0x00, 0xC0, 0x4B, 0x03, 0x00, 0x08, 0x07, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03, 0x2A, 0x2C, 0x0A,
    0x00, 0x40, 0x42, 0x0F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x1E, 0x24, 0x05, 0x02, 0x00, 0x80, 0x07,
    0x38, 0x04, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x90, 0x7E, 0x00, 0x40, 0x4B,
    0x4C, 0x00, 0x01, 0x40, 0x4B, 0x4C, 0x00, 0x1B, 0x24, 0x04, 0x07, 0x02, 0x50, 0x30, 0x31, 0x30,
    0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71, 0x18, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x26, 0x24, 0x05, 0x01, 0x00, 0x80, 0x02, 0x68, 0x01, 0x00, 0x40, 0x19, 0x01, 0x00,
    0xC0, 0x4B, 0x03, 0x00, 0x08, 0x07, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03, 0x2A, 0x2C, 0x0A, 0x00,
    0x40, 0x42, 0x0F, 0x00, 0x40, 0x4B, 0x4C, 0x00, 0x1E, 0x24, 0x05, 0x02, 0x00, 0x80, 0x07, 0x38,
    0x04, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x00, 0xC2, 0x01, 0x00, 0xEC, 0x5E, 0x00, 0x40, 0x4B, 0x4C,
    0x00, 0x01, 0x40, 0x4B, 0x4C, 0x00, 0x06, 0x24, 0x0D, 0x01, 0x01, 0x04, 0x07, 0x05, 0x82, 0x02,
    0x00, 0x04, 0x01, 0x06, 0x30, 0x0F, 0x00, 0x00, 0x00,
};
#else
static const UINT8 g_config_descriptor_u3[44]     = {
    0x09, 0x02, 0x2c, 0x00, 0x01, 0x01, 0x02, 0xc0, 0x32, 0x09, 0x04, 0x00, 0x00, 0x02, 0xff, 0x42, 0x03, 0x04, 0x07, 0x05, 0x81, 0x02, 0x00, 0x04, 0x00, 0x06, 0x30, 0x0f, 0x00, 0x00, 0x00, 0x07, 0x05, 0x02, 0x02, 0x00, 0x04, 0x00, 0x06, 0x30, 0x0f, 0x00, 0x00, 0x00
};
#endif

static unsigned char g_bos_descriptor[24] = 
{
        0x05, 0x0f, 0x16, 0x00, 0x02, 0x07, 0x10, 0x02, 0x06, 0x00, 0x00, 0x00, 0x0a, 0x10, 0x03, 0x00, 0x0f, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char g_hid_report_descriptor[54] = 
{
        0x06, 0xA0, 0xFF, 0x09, 0x01, 0xA1, 0x01, 0x09, 0x02, 0xA1, 0x00, 0x06, 0xA1, 0xFF, 0x09, 0x03,
        0x09, 0x04, 0x15, 0x80, 0x25, 0x7F, 0x35, 0x00, 0x45, 0xFF, 0x75, 0x08, 0x96, 0x06, 0x01, 0x81,
        0x02, 0x09, 0x05, 0x09, 0x06, 0x15, 0x80, 0x25, 0x7F, 0x35, 0x00, 0x45, 0xFF, 0x75, 0x08, 0x96,
        0x06, 0x01, 0x91, 0x02, 0xC0, 0xC0,
};

static unsigned char g_qualifier_descriptor[10] =
{
    0x0A, 0x06, 0x10, 0x02, 0x00, 0x02, 0x01, 0x40, 0x01, 0x00,
};

static const UINT8 descriptor_type_string_0_3[4]  = {0x04, 0x03, 0x09, 0x04};
static const UINT8 descriptor_type_string_1_3[34] = {0x22, 0x03, 0x4c, 0x00, 0x69, 0x00, 0x6e, 0x00, 0x75, 0x00, 0x78, 0x00, 0x20, 0x00, 0x46, 0x00, 0x6f, 0x00, 0x75, 0x00, 0x6e, 0x00, 0x64, 0x00, 0x61, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6f, 0x00, 0x6e, 0x00};
static const UINT8 descriptor_type_string_2_3[28] = {0x1c, 0x03, 0x57, 0x00, 0x65, 0x00, 0x62, 0x00, 0x63, 0x00, 0x61, 0x00, 0x6d, 0x00, 0x20, 0x00, 0x67, 0x00, 0x61, 0x00, 0x64, 0x00, 0x67, 0x00, 0x65, 0x00, 0x74, 0x00};
static const UINT8 descriptor_type_string_3_3[34] = {0x22, 0x03, 0x4c, 0x00, 0x69, 0x00, 0x6e, 0x00, 0x75, 0x00, 0x78, 0x00, 0x20, 0x00, 0x46, 0x00, 0x6f, 0x00, 0x75, 0x00, 0x6e, 0x00, 0x64, 0x00, 0x61, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6f, 0x00, 0x6e, 0x00};
static const UINT8 descriptor_type_string_4_3[12] = {0x0c, 0x03, 0x56, 0x00, 0x69, 0x00, 0x64, 0x00, 0x65, 0x00, 0x6f, 0x00};
static const UINT8 descriptor_type_string_5_3[22] = {0x16, 0x03, 0x55, 0x00, 0x56, 0x00, 0x43, 0x00, 0x20, 0x00, 0x43, 0x00, 0x61, 0x00, 0x6d, 0x00, 0x65, 0x00, 0x72, 0x00, 0x61, 0x00};
static const UINT8 descriptor_type_string_6_3[32] = {0x20, 0x03, 0x56, 0x00, 0x69, 0x00, 0x64, 0x00, 0x65, 0x00, 0x6f, 0x00, 0x20, 0x00, 0x53, 0x00, 0x74, 0x00, 0x72, 0x00, 0x65, 0x00, 0x61, 0x00, 0x6d, 0x00, 0x69, 0x00, 0x6e, 0x00, 0x67, 0x00};
static const UINT8 descriptor_type_string_7_3[34] = {0x22, 0x03, 0x4c, 0x00, 0x69, 0x00, 0x6e, 0x00, 0x75, 0x00, 0x78, 0x00, 0x20, 0x00, 0x46, 0x00, 0x6f, 0x00, 0x75, 0x00, 0x6e, 0x00, 0x64, 0x00, 0x61, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6f, 0x00, 0x6e, 0x00};

UINT8 testcnt =0;

void USB_init_memory(void)
{
    UINT8 *temp_u8p;

    for(temp_u8p= EVENT_BUFFER_cache; temp_u8p<(EVENT_BUFFER_cache+0x1000); temp_u8p=temp_u8p+4) {
        rtd_mem_outl(temp_u8p, 0x00000000);
        dmac_flush_range( temp_u8p, temp_u8p+4);
        outer_flush_range( ((unsigned long long)temp_u8p - (unsigned long long)EVENT_BUFFER_cache) + EVENT_BUFFER_PHY, ((unsigned long long)temp_u8p - (unsigned long long)EVENT_BUFFER_cache) + EVENT_BUFFER_PHY + 4);
    }

    rtd_mem_outl(TRB_BUFFER_cache + 0,  0x00003000);
    rtd_mem_outl(TRB_BUFFER_cache + 4,  0x00060000);
    rtd_mem_outl(TRB_BUFFER_cache + 8,  0x00000000);
    rtd_mem_outl(TRB_BUFFER_cache + 12, 0x00000000);
    dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+16);
    outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY + 16);

    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,  TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,  0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  0x00000008);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000C23);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
}

void USB_init_FPGA_PHY_board(void)
{
    // PHY board reset and enable
#ifdef CONFIG_ARCH_RTK6702
    rtd_outl(0xb8000320, 0x01);   // link status no change
    rtd_outl(0xb8000320, 0x03);   // link status no change
#endif
#ifdef CONFIG_ARCH_RTK2885P
    rtd_outl(0xb8000258, 0x10000000);   // link status no change
    rtd_outl(0xb8000258, 0x30000000);   // link status no change
#endif
}

void USB_init_CRT(void)
{
    rtd_outl(0xb8000114, 0xf8000001);
    rtd_outl(0xb8000104, 0xf8000001);
    
    // rtk_xhci_drd_crt_init
    rtd_outl(0xb8000118, 0x00040000);
    rtd_outl(0xb8000108, 0x00040000);   // link status 5
#ifdef CONFIG_ARCH_RTK6702
    rtd_outl(0xb800010C, 0x000000c6);   // link status no change keep in 5
    rtd_outl(0xb800010C, 0x000000c7);   // link status no change keep in 5
#endif
#ifdef CONFIG_ARCH_RTK2885P
    rtd_outl(0xb800010C, 0x00000034);   // link status no change keep in 5
    rtd_outl(0xb800010C, 0x00000035);   // link status no change keep in 5
#endif
    rtd_outl(0xb8000118, 0x00040001);   // link status no change keep in 5
    rtd_outl(0xb8000118, 0x00040000);   // link status no change keep in 5
    rtd_outl(0xb8000108, 0x00040001);   // link status 4
    rtd_outl(0xb8000118, 0x00040001);   // link status no change keep in 4
}

void USB_init_clear_event_buffer(void)
{
    UINT32 temp;
    rtd_outl(dwc3_offset + 0x704 ,0x00000000);
    temp = rtd_inl(dwc3_offset + 0x40c);
    rtd_outl(dwc3_offset + 0x40c, temp);
}

UINT8 dwc3_usb3_load_phy(void);
void USB_init_phy(void)
{
    dwc3_usb3_load_phy();
}

void USB_init_mac_globe_init(void)
{
    UINT32 temp;

	//Mac setting
	rtd_outl(0xb80ac110,0x30c12014);
	rtd_outl(0xb80ac704,0x40f00000);
    while(1){
        RTK_USB_DEVICE_DBG_SIMPLE( "USB_init_mac_globe_init %d\n", __LINE__);
        temp = rtd_inl(dwc3_offset + 0x704);
        if(temp == 0x00f00000)
            break;
    }

	rtd_outl(0xb80ac200,0x40002407);
	rtd_outl(0xb80ac400,0x40a00000);//mac event buf addr
	rtd_outl(0xb80ac404,0x00000000);
	rtd_outl(0xb80ac408,0x00000030);
	rtd_outl(0xb80ac40c,0x00000000);
	//rtd_outl(0xb80ac110,0x30c12234);//[5:4]scaledown
	rtd_outl(0xb80ac110,0x30c12204);//[5:4]scaledown
	//rtd_outl(0xb80ac700,0x00480805);
	rtd_outl(0xb80ac700,0x00480804);//5G:0  10G:1
	rtd_outl(0xb80ac708,0x0000001f);
	rtd_outl(0xb80ac12c,0x0a400000);
	rtd_outl(0xb80ac11c,0x01001988);
	rtd_outl(0xb80ac720,0x00000003);
	rtd_outl(0xb80ac704,0x00f00200);
	rtd_outl(0xb80ac704,0x00f00a00);
	rtd_outl(0xb80ac060,0x000034c2);

}

void USB_init_mac_enable_EP0(void)
{
    UINT32 temp;
    ////////////////////////////////////////////////////////////////
    //__dwc3_gadget_ep_enable EP0
    ////////////////////////////////////////////////////////////////
    rtd_outl(dwc3_offset + 0x808 ,0x00000000);
    rtd_outl(dwc3_offset + 0x804 ,0x00000000);
    rtd_outl(dwc3_offset + 0x800 ,0x00000000);
    rtd_outl(dwc3_offset + 0x80c ,0x00000409);
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x80c);
        if(temp == 0x9)
            break;
    }
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    
    
    
    rtd_outl(dwc3_offset + 0x808 ,0x00001000);
    rtd_outl(dwc3_offset + 0x804 ,0x00000500);
    rtd_outl(dwc3_offset + 0x800 ,0x00000000);
    rtd_outl(dwc3_offset + 0x80c ,0x00000401);
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x80c);
        if(temp == 0x1)
            break;
    }
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    
    
    rtd_outl(dwc3_offset + 0x808 ,0x00000001);
    rtd_outl(dwc3_offset + 0x804 ,0x00000000);
    rtd_outl(dwc3_offset + 0x800 ,0x00000000);
    rtd_outl(dwc3_offset + 0x80c ,0x00000402);
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x80c);
        if(temp == 0x10002)
            break;
    }
    rtd_outl(dwc3_offset + 0x720 ,0x00000001);
}

void USB_init_mac_enable_EP1(void)
{
    UINT32 temp;
    ////////////////////////////////////////////////////////////////
    //__dwc3_gadget_ep_enable EP1
    ////////////////////////////////////////////////////////////////
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    rtd_outl(dwc3_offset + 0x818 ,0x00001000);
    rtd_outl(dwc3_offset + 0x814 ,0x02000500);
    rtd_outl(dwc3_offset + 0x810 ,0x00000000);
    rtd_outl(dwc3_offset + 0x81c ,0x00000401);
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x1)
            break;
    }
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    
    rtd_outl(dwc3_offset + 0x818 ,0x00000001);
    rtd_outl(dwc3_offset + 0x814 ,0x00000000);
    rtd_outl(dwc3_offset + 0x810 ,0x00000000);
    rtd_outl(dwc3_offset + 0x81c ,0x00000402);
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x20002)
            break;
    }
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    rtd_outl(dwc3_offset + 0x720 ,0x00000003);
}

void USB_init(void)
{
    UINT32 temp;
    UINT8 *temp_u8p;

    (void)temp;
    (void)temp_u8p;

    USB_init_memory();
    //USB_init_FPGA_PHY_board();
    //USB_init_CRT();

    //USB_init_clear_event_buffer();
    USB_init_memory();

#ifdef USE_DANTE_LOOP

    USB_init_phy();
    USB_init_mac_globe_init();
    //dwc->ctrl_req      = 0x23103d80
    //dwc->ctrl_req_addr = 0x23103d80

    ////////////////////////////////////////////////////////////////
    // dwc3_gadget_run_stop
    ////////////////////////////////////////////////////////////////
    rtd_outl(dwc3_offset + 0x704 ,0x80000000);
    // wait

    ////////////////////////////////////////////////////////////////
    //dwc3_gadget_start
    ////////////////////////////////////////////////////////////////
    rtd_outl(dwc3_offset + 0x700 ,0x00080804);

    USB_init_mac_enable_EP0();
    USB_init_mac_enable_EP1();

    ////////////////////////////////////////////////////////////////
    //dwc3_ep0_out_start(dwc);
    ////////////////////////////////////////////////////////////////
    // bpl=0x23103d80 bph=0x00000000 size=0x00000008 ctrl=0x00000020
    rtd_outl(dwc3_offset + 0x808 ,0x00000000);
    rtd_outl(dwc3_offset + 0x804 ,TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x800 ,0x00000000);
    rtd_outl(dwc3_offset + 0x80c ,0x00000406);
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x80c);
        if(temp == 0x6)
            break;
    }
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    
    
    rtd_outl(dwc3_offset + 0x708 ,0x00001e1f);

    ////////////////////////////////////////////////////////////////
    // dwc3_gadget_run_stop
    ////////////////////////////////////////////////////////////////
    rtd_outl(dwc3_offset + 0x704 ,0x80000000);

    rtd_outl(dwc3_offset + 0x108, 0x0000000);  // GTXTHRCFG for FPGA
    rtd_outl(dwc3_offset + 0x10C, 0x0000000);  // GRXTHRCFG for FPGA
    rtd_outl(dwc3_offset + 0x100, 0x000000e);  // GSBUSCFG0 for FPGA

    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x40c);
        if(temp >= 0x10)
            break;
    }
    RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    
    rtd_inl(dwc3_offset + 0x408);
    rtd_outl(dwc3_offset + 0x408 ,0x80000100);
#endif
}

#define UVC_FRAME_BUFFER_NUMBER 3
#define UVC_FRAME_BUFFER_SIZE   0xFD2000
extern UINT8 *uvc_frame_vir[UVC_FRAME_BUFFER_NUMBER];

void mjpeg_enc_qmem_table(void);
void mjpeg_enc_quant_table(void);

int dante_loop_real(void *data)
{
    struct file *pFile;
    loff_t file_offset = 0;
    char tmpData[16];

    (void)tmpData;

    init_waitqueue_head(&waitq_test);

    while(1) {
        //pattern_gen_one_frame();
        //usb_output_one_frame();
#ifndef USE_DWC3_INTERRUPT
        dwc3_gadget_uboot_handle_interrupt_ex();
#endif
        usleep_range(1000, 1000);

        (void)pFile;
        (void)file_offset;
        ScalerTimer_DelayXms(10);
#if 0
        if(rtd_inl(0xb8004ae8) == 0x00000001 ) {
            RTK_USB_DEVICE_DBG_SIMPLE("dante_loop_real write file\n");
            pFile = filp_open("/tmp/usbmounts/uvc.bin", O_CREAT |O_RDWR, 0600);
            file_offset = 0;
            if(kernel_write(pFile, (const char *)uvc_frame_vir[0], UVC_FRAME_BUFFER_SIZE, &file_offset) < 0) {
                RTK_USB_DEVICE_DBG_SIMPLE("dante_loop_real kernel write failed\n");
            }
            filp_close(pFile, NULL);
            rtd_outl(0xb8004ae8, 0);
        }
#endif
        if(rtd_inl(0xb8004ae8) == 0x80000000 ) {
            g_rtk_dwc3.uvc_show_log = 1;
       }
#if 0
        if(rtd_inl(0xb8004ae8) == 0x00000002 ) {
            rtd_outl(SYS_REG_SYS_SRST2_reg,SYS_REG_SYS_SRST2_rstn_bistreg_mask|SYS_REG_SYS_SRST2_write_data_mask);
            rtd_outl(SYS_REG_SYS_CLKEN2_reg, SYS_REG_SYS_CLKEN2_clken_bistreg_mask|SYS_REG_SYS_CLKEN2_write_data_mask);

            rtd_outl( SYS_REG_SYS_CLKEN5_reg ,0x00000009);
            rtd_outl( SYS_REG_SYS_CLKEN0_reg ,0x00001001);
            rtd_outl( PLL_REG_SYS_PLL_CPU_reg ,0x00004165);
            rtd_outl( PLL_REG_SYS_PLL_UVC_2_reg ,0x01c71605);
            rtd_outl( PLL_REG_SYS_PLL_UVC_1_reg ,0x01110009);
            rtd_outl( PLL_REG_SYS_PLL_UVC_2_reg ,0x01c71705);
            rtd_outl( PLL_REG_SYS_DVFS_UVC_1_reg ,0x54200805);
            rtd_outl( PLL_REG_SYS_PLL_UVC_2_reg ,0x01c71707);
            rtd_outl( PLL_REG_SYS_PLL_UVC_2_reg ,0x01c71703);
            rtd_outl( PINMUX_LVDSPHY_LVDSEPIVBY1_PHY_CTRL_0_27_reg ,0x80000000);
            ScalerTimer_DelayXms(10);
            //rtd_outl( PLL27X_REG_SYS_PLL_DISP3_reg ,0xc0000024);
            //rtd_outl( PLL27X_REG_SYS_PLL_DISP2_reg ,0x00000001);
            //rtd_outl( PLL27X_REG_PLL_SSC0_reg ,0x00003ff0);
            //rtd_outl( PLL27X_REG_SYS_PLL_DISP1_reg ,0x01800330);
            //rtd_outl( PLL27X_REG_PLL_SSC0_reg ,0x01c73ff0);
            //rtd_outl( PLL27X_REG_SYS_PLL_DISP3_reg ,0xc0000025);
            //rtd_outl( PLL27X_REG_PLL_SSC0_reg ,0x01c73ff1);
            //rtd_outl( PLL27X_REG_SYS_PLL_DISP3_reg ,0xc0000065);
            //rtd_outl( PLL27X_REG_SYS_PLL_DISP3_reg ,0xc0000061);
            rtd_outl( UVC_VGIP_UVC_VGIP_CTRL_reg ,0x00000001);// [0]: reg_uvc_in_clk_en
            ScalerTimer_DelayXms(10);
#if 0
            // 1920 * 1080
            rtd_outl(0xb8004e00, 0xb9438780); // load qmem,height, width
            mjpeg_enc_qmem_table();
            rtd_outl(0xb8004e00, 0xd9438780); // load quant,height, width
            mjpeg_enc_quant_table();

            rtd_outl(0xb8004e14, 0x000000ff); // q table
            rtd_outl(0xb8004e00, 0x99438780); // height, width
            rtd_outl(0xb8004e68, 0x00094000);
            rtd_outl(0xb8004e70, 0x000016b6);
            rtd_outl(0xb8004e44, 0x00110804);
            rtd_outl(0xb8004e48, 0x38078003);
            rtd_outl(0xb8004e4c, 0x01220002);
            rtd_outl(0xb8004e50, 0x11010311);
            rtd_outl(0xb8004e54, 0x00000001);
            rtd_outl(0xb8004e04, 0x00000003);
#endif
#if 1
            // 3840 * 2160
            rtd_outl(0xb8004e00, 0xb9870F00); // load qmem,height, width
            mjpeg_enc_qmem_table();
            rtd_outl(0xb8004e00, 0xd9870F00); // load quant,height, width
            mjpeg_enc_quant_table();
            rtd_outl(0xb8004e14, 0x000000ff); // q table
            rtd_outl(0xb8004e00, 0x99870F00); // height, width
            rtd_outl(0xb8004e68, 0x001fffff);
            rtd_outl(0xb8004e70, 0x00000400);
            rtd_outl(0xb8004e44, 0x00110808);
            rtd_outl(0xb8004e48, 0x700F0003);
            rtd_outl(0xb8004e4c, 0x01220002);
            rtd_outl(0xb8004e50, 0x11010311);
            rtd_outl(0xb8004e54, 0x00000001);
            rtd_outl(0xb8004e04, 0x00000003);
#endif
            rtd_outl(0xb8004ae8, 0);
        }
#endif
    }
    return 0;
}

static UINT32 connect_speed = 0;
static UINT32 event_value;
static UINT8 loca_req_data[8];
static UINT8 current_state;
static  UINT32 temp;
static  UINT8 *trb_address_current;
static  UINT8  request = 0;
static  UINT8  request_type = 0;
static  UINT32 cmd;
static  UINT32 frame_number;
static  UINT32 ep0_setup_buf_len;

void dante_loop_init_memory(void)
{
    //EVENT_BUFFER         = dma_alloc_coherent(dwc->sysdev, 0x1000, &EVENT_BUFFER_PHY, GFP_KERNEL);
    //TRB_BUFFER           = dma_alloc_coherent(dwc->sysdev, 0x1000, &TRB_BUFFER_PHY, GFP_KERNEL);
    //TRB_ARRAY_BUFFER     = dma_alloc_coherent(dwc->sysdev, 0x1000, &TRB_ARRAY_BUFFER_PHY, GFP_KERNEL);
    //UAC_TRB_BUFFER       = dma_alloc_coherent(dwc->sysdev, 0x1000, &UAC_TRB_BUFFER_PHY, GFP_KERNEL);
    //UAC_TRB_ARRAY_BUFFER = dma_alloc_coherent(dwc->sysdev, 0x1000, &UAC_TRB_ARRAY_BUFFER_PHY, GFP_KERNEL);

    EVENT_BUFFER_cache = (UINT8*)dvr_malloc_uncached( 0x1000, (void*)&EVENT_BUFFER );
    EVENT_BUFFER_PHY = dvr_to_phys( (void*)EVENT_BUFFER_cache ) ;    /* translate pack buffer from virtual & cached to phys */
    RTK_USB_DEVICE_DBG_SIMPLE( "EVENT_BUFFER_PHY=%x EVENT_BUFFER_cache=%px EVENT_BUFFER=%px\n", EVENT_BUFFER_PHY, EVENT_BUFFER_cache, EVENT_BUFFER);

    TRB_BUFFER_cache = (UINT8*)dvr_malloc_uncached( 0x1000, (void*)&TRB_BUFFER );
    TRB_BUFFER_PHY = dvr_to_phys( (void*)TRB_BUFFER_cache ) ;    /* translate pack buffer from virtual & cached to phys */
    RTK_USB_DEVICE_DBG_SIMPLE( "TRB_BUFFER_PHY=%x TRB_BUFFER_cache=%px TRB_BUFFER=%px\n", TRB_BUFFER_PHY, TRB_BUFFER_cache, TRB_BUFFER);

    TRB_ARRAY_BUFFER_cache = (UINT8*)dvr_malloc_uncached( 0x1000, (void*)&TRB_ARRAY_BUFFER );
    TRB_ARRAY_BUFFER_PHY = dvr_to_phys( (void*)TRB_ARRAY_BUFFER_cache ) ;    /* translate pack buffer from virtual & cached to phys */
    RTK_USB_DEVICE_DBG_SIMPLE( "TRB_ARRAY_BUFFER_PHY=%x TRB_ARRAY_BUFFER_cache=%px TRB_ARRAY_BUFFER=%px\n", TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER);

    UAC_TRB_BUFFER_cache = (UINT8*)dvr_malloc_uncached( 0x1000, (void*)&UAC_TRB_BUFFER );
    UAC_TRB_BUFFER_PHY = dvr_to_phys( (void*)UAC_TRB_BUFFER_cache ) ;    /* translate pack buffer from virtual & cached to phys */
    RTK_USB_DEVICE_DBG_SIMPLE( "UAC_TRB_BUFFER_PHY=%x UAC_TRB_BUFFER_cache=%px UAC_TRB_BUFFER=%px\n", UAC_TRB_BUFFER_PHY, UAC_TRB_BUFFER_cache, UAC_TRB_BUFFER);

    UAC_TRB_ARRAY_BUFFER_cache = (UINT8*)dvr_malloc_uncached( 0xfd2000, (void*)&UAC_TRB_ARRAY_BUFFER );
    UAC_TRB_ARRAY_BUFFER_PHY = dvr_to_phys( (void*)UAC_TRB_ARRAY_BUFFER_cache ) ;    /* translate pack buffer from virtual & cached to phys */
    RTK_USB_DEVICE_DBG_SIMPLE( "UAC_TRB_ARRAY_BUFFER_PHY=%x UAC_TRB_ARRAY_BUFFER_cache=%px UAC_TRB_ARRAY_BUFFER=%px\n", UAC_TRB_ARRAY_BUFFER_PHY, UAC_TRB_ARRAY_BUFFER_cache, UAC_TRB_ARRAY_BUFFER);
}

void dante_loop_process_connect_done(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "[EVENT] = 0x%x DWC3_DEVICE_EVENT_CONNECT_DONE\n",(UINT32) event_value);

    connect_speed = rtd_inl(dwc3_offset + 0x70c);
    connect_speed = connect_speed & 0x00000007;
    switch(connect_speed) {
        case 0:
            RTK_USB_DEVICE_DBG_SIMPLE( "CONNECT SPEED is HIGH SPEED\n");
            break;
        case 1:
            RTK_USB_DEVICE_DBG_SIMPLE( "CONNECT SPEED is FULL SPEED\n");
            break;
        case 4:
            RTK_USB_DEVICE_DBG_SIMPLE( "CONNECT SPEED is SUPER SPEED 5G\n");
            break;
        case 5:
            RTK_USB_DEVICE_DBG_SIMPLE( "CONNECT SPEED is SUPER SPEED PLUS 10G\n");
            break;
        default:
            RTK_USB_DEVICE_DBG_SIMPLE( "CONNECT SPEED is UNKNOW SPEED\n");
            break;
    }

    //RTK_USB_DEVICE_DBG_SIMPLE( "dwc3_offset + 0x70c = 0x%x (Except is 0x230dc)\n", rtd_inl(dwc3_offset + 0x70c));
    //RTK_USB_DEVICE_DBG_SIMPLE( "dwc3_offset + 0x10c = 0x%x (Except is 0x24400000)\n", rtd_inl(dwc3_offset + 0x10c));
//                  rtd_outl(dwc3_offset + 0x10c, 0x4400000);
    //RTK_USB_DEVICE_DBG_SIMPLE( "dwc3_offset + 0x704 = 0x%x (Except is 0x80000000)\n", rtd_inl(dwc3_offset + 0x704));

    rtd_outl(dwc3_offset + 0x704, 0x80000000);
    if(connect_speed==4 || connect_speed==5 )
        rtd_outl(dwc3_offset + 0x808, 0x80001000);   // for U3
    else
        rtd_outl(dwc3_offset + 0x808, 0x80000200);   // for U2

    rtd_outl(dwc3_offset + 0x804, 0x00000500);
    rtd_outl(dwc3_offset + 0x800, 0x00000000);
    rtd_outl(dwc3_offset + 0x80c, 0x00000401);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x80c);
        if(temp == 0x1)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);

    rtd_outl(dwc3_offset + 0x704, 0x80000000);
    if(connect_speed==4 || connect_speed==5 )
        rtd_outl(dwc3_offset + 0x818, 0x80001000);   // for U3
    else
        rtd_outl(dwc3_offset + 0x818, 0x80000200);   // for U2

    rtd_outl(dwc3_offset + 0x814, 0x2000500);
    rtd_outl(dwc3_offset + 0x810, 0x0);
    rtd_outl(dwc3_offset + 0x81c, 0x401);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x1)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
}

void dante_loop_process_XFERNOTREADY_EP0(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "[EVENT] = 0x%x DWC3_DEPEVT_XFERNOTREADY ep=0\n", (UINT32)event_value);
    if( request == USB_REQ_GET_DESCRIPTOR || request == USB_REQ_GET_STATUS || request == USB_REQ_CLASS_GET_LEN || request == USB_REQ_CLASS_GET_INFO || request==USB_REQ_CLASS_GET_MIN || request==USB_REQ_CLASS_GET_MAX || request==USB_REQ_CLASS_GET_RES || request==USB_REQ_CLASS_GET_DEF) {
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,    TRB_BUFFER_PHY);
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,    0x00000000);
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,    0x00000000);
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c43);
        dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
        outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
        
        rtd_outl(dwc3_offset + 0x808, 0x00000000);
        rtd_outl(dwc3_offset + 0x804, TRB_ARRAY_BUFFER_PHY);
        rtd_outl(dwc3_offset + 0x800, 0x00000000);
        rtd_outl(dwc3_offset + 0x80c, 0x00000406);
        RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
        while(1){
            temp = rtd_inl(dwc3_offset + 0x80c);
            if(temp == 0x6)
                break;
        }
        RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    }
}

void dante_loop_process_XFERCOMPLETE_EP0_set_address(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "SET ADDRESS addr=0x%x\n", (UINT32)loca_req_data[2]);
    request = USB_REQ_SET_ADDRESS;
    temp = rtd_inl(dwc3_offset + 0x700);
    RTK_USB_DEVICE_DBG_SIMPLE( "read dwc3_offset + 0x700 = 0x%x\n", (UINT32)temp);
    temp = (UINT32)temp & 0xFFFFFC07;
    temp = temp | ((UINT32)loca_req_data[2] <<3);
    rtd_outl(dwc3_offset + 0x700, temp);
    RTK_USB_DEVICE_DBG_SIMPLE( "write dwc3_offset + 0x700 = 0x%x\n", (UINT32)temp);
    current_state = STAGE_STATUS;
}

void dante_loop_process_XFERCOMPLETE_EP0_set_feature(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "SET FEATURE\n");
    request = USB_REQ_SET_FEATURE;
    current_state = STAGE_STATUS;
}

void dante_loop_process_XFERCOMPLETE_EP0_set_idel(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "SET IDEL\n");
    request = USB_REQ_SET_INTERFACE;
    current_state = STAGE_STATUS;
}

void dante_loop_process_XFERCOMPLETE_EP0_set_interface(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "SET INTERFACE\n");
    request      = USB_REQ_SET_INTERFACE;
    current_state = STAGE_STATUS;
}

void dante_loop_process_XFERCOMPLETE_EP0_set_configuration(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "SET_CONFIGURATION\n");
    request      = USB_REQ_SET_CONFIGURATION;
    current_state = STAGE_STATUS;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_configuration(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "USB_REQ_GET_CONFIGURATION\n");
    request      = USB_REQ_GET_CONFIGURATION;
    current_state = STAGE_STATUS;
}

void dante_loop_process_XFERCOMPLETE_EP0_clear_feature(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "CLEAE FEATURE\n");
    request      = USB_REQ_CLEAR_FEATURE;
    current_state = STAGE_STATUS;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_status(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET STATUS\n");
    request      = USB_REQ_GET_STATUS;
    temp =  0x00000000;
    rtd_mem_outl(TRB_BUFFER_cache,  temp);
    dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+16);
    outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY + 16);
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,    TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,    0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,    0x00000002);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_len(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET LEN\n");
    request      = USB_REQ_CLASS_GET_LEN;
    ep0_setup_buf_len =  2;
    
    if(loca_req_data[3] == 0x0a)
        temp =  0x00000008;
    if(loca_req_data[3] == 0x0b)
        temp =  0x00000014;
    if(loca_req_data[3] == 0x0c)
        temp =  0x0000001c;
    if(loca_req_data[3] == 0x0d)
        temp =  0x00000007;
    if(loca_req_data[3] == 0x0e)
        temp =  0x00000008;
    if(loca_req_data[3] == 0x0f)
        temp =  0x00000001;
    if(loca_req_data[3] == 0x10)
        temp =  0x00000001;
    if(loca_req_data[3] == 0x13)
        temp =  0x0000001f;
    rtd_mem_outl(TRB_BUFFER_cache,    temp);
    dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+4);
    outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY + 4);
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,  TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,  0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  ep0_setup_buf_len);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_info(void)
{
     RTK_USB_DEVICE_DBG_SIMPLE( "GET INFO\n");
     request      = USB_REQ_CLASS_GET_INFO;
     ep0_setup_buf_len =  1;
    
     if(loca_req_data[3] == 0x0a)
         temp =  0x00000003;
     if(loca_req_data[3] == 0x0b)
         temp =  0x00000003;
     if(loca_req_data[3] == 0x0c)
         temp =  0x00000003;
     if(loca_req_data[3] == 0x0d)
         temp =  0x00000003;
     if(loca_req_data[3] == 0x0e)
         temp =  0x00000003;
     if(loca_req_data[3] == 0x0f)
         temp =  0x00000003;
    if(loca_req_data[3] == 0x10)
        temp =  0x00000003;
    if(loca_req_data[3] == 0x13)
        temp =  0x00000003;
    rtd_mem_outl(TRB_BUFFER_cache,    temp);
     dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+4);
     outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY + 4);
     
     rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,  TRB_BUFFER_PHY);
     rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,  0x00000000);
     rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  ep0_setup_buf_len);
     rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
     dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
     outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
     
     rtd_outl(dwc3_offset + 0x818, 0x00000000);
     rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
     rtd_outl(dwc3_offset + 0x810, 0x00000000);
     rtd_outl(dwc3_offset + 0x81c, 0x00000406);
     //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
     while(1){
         temp = rtd_inl(dwc3_offset + 0x81c);
         if(temp == 0x10006)
             break;
     }
     //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
     current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_min(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET MIN\n");
    request      = USB_REQ_CLASS_GET_MIN;
    ep0_setup_buf_len =  2;
    
    if(loca_req_data[3] == 0x0a)
        temp =  0x00000000;
    if(loca_req_data[3] == 0x0b)
        temp =  0x00000000;
    if(loca_req_data[3] == 0x0c)
        temp =  0x00000000;
    if(loca_req_data[3] == 0x0d)
        temp =  0x00000000;
    if(loca_req_data[3] == 0x0e)
        temp =  0x00000000;
    if(loca_req_data[3] == 0x0f) {
        ep0_setup_buf_len = 1;
        temp =  0x00000000;
    }
    if(loca_req_data[3] == 0x10) {
        ep0_setup_buf_len = 1;
        temp =  0x00000000;
    }
    if(loca_req_data[3] == 0x13)
        temp =  0x00000000;
    rtd_mem_outl(TRB_BUFFER_cache,    temp);
    dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+4);
    outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY + 4);
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,  TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,  0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  ep0_setup_buf_len);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_max(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET MAX\n");
    request      = USB_REQ_CLASS_GET_MAX;
    ep0_setup_buf_len =  2;
    
    if(loca_req_data[3] == 0x0a)
        temp =  0x00000001;
    if(loca_req_data[3] == 0x0b)
        temp =  0x00000001;
    if(loca_req_data[3] == 0x0c)
        temp =  0x00000001;
    if(loca_req_data[3] == 0x0d)
        temp =  0x00000001;
    if(loca_req_data[3] == 0x0e)
        temp =  0x00000001;
    if(loca_req_data[3] == 0x0f) {
        ep0_setup_buf_len = 1;
        temp =  0x00000001;
    }
    if(loca_req_data[3] == 0x10) {
        ep0_setup_buf_len = 1;
        temp =  0x00000001;
    }
    if(loca_req_data[3] == 0x13)
        temp =  0x00000001;
    rtd_mem_outl(TRB_BUFFER_cache,    temp);
    dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+4);
    outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY + 4);
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,  TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,  0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  ep0_setup_buf_len);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_res(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET RES\n");
    request      = USB_REQ_CLASS_GET_RES;
    ep0_setup_buf_len =  2;
    
    if(loca_req_data[3] == 0x0a)
        temp =  0x00000001;
    if(loca_req_data[3] == 0x0b)
        temp =  0x00000001;
    if(loca_req_data[3] == 0x0c)
        temp =  0x00000001;
    if(loca_req_data[3] == 0x0d)
        temp =  0x00000001;
    if(loca_req_data[3] == 0x0e)
        temp =  0x00000001;
    if(loca_req_data[3] == 0x0f) {
        ep0_setup_buf_len = 1;
        temp =  0x00000001;
    }
    if(loca_req_data[3] == 0x10) {
        ep0_setup_buf_len = 1;
        temp =  0x00000001;
    }
    if(loca_req_data[3] == 0x13)
        temp =  0x00000001;
    rtd_mem_outl(TRB_BUFFER_cache,    temp);
    dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+4);
    outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY + 4);
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,  TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,  0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  ep0_setup_buf_len);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_def(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET DEF\n");
    request      = USB_REQ_CLASS_GET_DEF;
    ep0_setup_buf_len =  2;
    
    if(loca_req_data[3] == 0x0a)
        temp =  0x00000000;
    if(loca_req_data[3] == 0x0b)
        temp =  0x00000000;
    if(loca_req_data[3] == 0x0c)
        temp =  0x00000000;
    if(loca_req_data[3] == 0x0d)
        temp =  0x00000000;
    if(loca_req_data[3] == 0x0e)
        temp =  0x00000000;
    if(loca_req_data[3] == 0x0f) {
        ep0_setup_buf_len = 1;
        temp =  0x00000000;
    }
    if(loca_req_data[3] == 0x10) {
        ep0_setup_buf_len = 1;
        temp =  0x00000000;
    }
    if(loca_req_data[3] == 0x13)
        temp =  0x00000000;
    rtd_mem_outl(TRB_BUFFER_cache,    temp);
    dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+4);
    outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY + 4);
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,  TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,  0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  ep0_setup_buf_len);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_device(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET DESCRIPTOR DEVICE\n");
    request      = USB_REQ_GET_DESCRIPTOR;
    request_type = USB_DT_DEVICE;
    
    dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)&g_dev_descriptor, sizeof(g_dev_descriptor));
    dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+sizeof(g_dev_descriptor));
    outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY + sizeof(g_dev_descriptor));
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,    TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,    0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,    sizeof(g_dev_descriptor));
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_config(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET DESCRIPTOR CONFIG\n");
    RTK_USB_DEVICE_DBG_SIMPLE( "%02x %02x %02x %02x %02x %02x %02x %02x\n", loca_req_data[0], loca_req_data[1], loca_req_data[2], loca_req_data[3], loca_req_data[4], loca_req_data[5], loca_req_data[6], loca_req_data[7]);
    request      = USB_REQ_GET_DESCRIPTOR;
    request_type = USB_DT_CONFIG;
    
    ep0_setup_buf_len = (loca_req_data[6]&0xff) + ((loca_req_data[7] << 8)&0xff00);
    RTK_USB_DEVICE_DBG_SIMPLE( "config_size_1 len=%d\n", ep0_setup_buf_len);
    
    if(connect_speed == 4 || connect_speed==5) {
        if( ep0_setup_buf_len > sizeof(g_config_descriptor_u3)) {
            ep0_setup_buf_len = sizeof(g_config_descriptor_u3);
        }
        dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)&g_config_descriptor_u3, ep0_setup_buf_len);
    } else {
        if( ep0_setup_buf_len > sizeof(g_config_descriptor_u2)) {
            ep0_setup_buf_len = sizeof((g_config_descriptor_u2));
        }
        dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)&(g_config_descriptor_u2), ep0_setup_buf_len);
    }
    RTK_USB_DEVICE_DBG_SIMPLE( "config_size_2 len=%d\n", ep0_setup_buf_len);
    
    dmac_flush_range( TRB_BUFFER_cache, (char*)(TRB_BUFFER_cache + ep0_setup_buf_len));
    outer_flush_range(TRB_BUFFER_PHY, TRB_BUFFER_PHY+ep0_setup_buf_len);
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,    TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,    0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  ep0_setup_buf_len);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, (char*)(TRB_ARRAY_BUFFER_cache + 12));
    outer_flush_range(TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY+12);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_USB_DT_HID_REPORT(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET DESCRIPTOR USB_DT_HID_REPORT\n");
    RTK_USB_DEVICE_DBG_SIMPLE( "%02x %02x %02x %02x %02x %02x %02x %02x\n", loca_req_data[0], loca_req_data[1], loca_req_data[2], loca_req_data[3], loca_req_data[4], loca_req_data[5], loca_req_data[6], loca_req_data[7]);
    request      = USB_REQ_GET_DESCRIPTOR;
    request_type = USB_DT_HID_REPORT;
    
    ep0_setup_buf_len = (loca_req_data[6]&0xff) + ((loca_req_data[7] << 8)&0xff00);
    RTK_USB_DEVICE_DBG_SIMPLE( "config_size_1 len=%d\n", ep0_setup_buf_len);
    
    if( ep0_setup_buf_len > sizeof(g_hid_report_descriptor)) {
        ep0_setup_buf_len = sizeof(g_hid_report_descriptor);
    }
    RTK_USB_DEVICE_DBG_SIMPLE( "config_size_2 len=%d\n", ep0_setup_buf_len);
    
    dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)&g_hid_report_descriptor, ep0_setup_buf_len);
    dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+ep0_setup_buf_len);
    outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY + ep0_setup_buf_len);
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,    TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,    0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  ep0_setup_buf_len);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_QUALIFIER(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET DESCRIPTOR QUALIFIER\n");
    request      = USB_REQ_GET_DESCRIPTOR;
    request_type = USB_DT_DEVICE_QUALIFIER;
    
    dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)&g_qualifier_descriptor, sizeof(g_qualifier_descriptor));
    dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+ sizeof(g_qualifier_descriptor));
    outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY +  sizeof(g_qualifier_descriptor));
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,    TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,    0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,    sizeof(g_qualifier_descriptor));
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_other_speed_config(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET DESCRIPTOR OTHER SPEED CONFIG\n");
    request      = USB_REQ_GET_DESCRIPTOR;
    request_type = USB_DT_OTHER_SPEED_CONFIG;
    
    //dwc3_copy_to_memory(TRB_BUFFER, (void *)&g_qualifier_descriptor, sizeof(g_qualifier_descriptor));
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,    TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,    0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,    0);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_string(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET DESCRIPTOR STRING\n");
    request      = USB_REQ_GET_DESCRIPTOR;
    request_type = USB_DT_STRING;
    
    ep0_setup_buf_len = (loca_req_data[6]&0xff) + ((loca_req_data[7] << 8)&0xff00);
    RTK_USB_DEVICE_DBG_SIMPLE( "config_size_1 len=%d\n", ep0_setup_buf_len);
    
    if (loca_req_data[2] == 0x0) {
        if( ep0_setup_buf_len > sizeof(descriptor_type_string_0_3)) {
            ep0_setup_buf_len = sizeof(descriptor_type_string_0_3);
        }
        RTK_USB_DEVICE_DBG_SIMPLE( "config_size_2 len=%d\n", ep0_setup_buf_len);
        dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)descriptor_type_string_0_3, ep0_setup_buf_len);
    } else if (loca_req_data[2] == 0x1) {
        if( ep0_setup_buf_len > sizeof(descriptor_type_string_1_3)) {
            ep0_setup_buf_len = sizeof(descriptor_type_string_1_3);
        }
        RTK_USB_DEVICE_DBG_SIMPLE( "config_size_2 len=%d\n", ep0_setup_buf_len);
        dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)descriptor_type_string_1_3, ep0_setup_buf_len);
    } else if (loca_req_data[2] == 0x2) {
        if( ep0_setup_buf_len > sizeof(descriptor_type_string_2_3)) {
            ep0_setup_buf_len = sizeof(descriptor_type_string_2_3);
        }
        RTK_USB_DEVICE_DBG_SIMPLE( "config_size_2 len=%d\n", ep0_setup_buf_len);
        dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)descriptor_type_string_2_3, ep0_setup_buf_len);
    } else if (loca_req_data[2] == 0x3) {
        if( ep0_setup_buf_len > sizeof(descriptor_type_string_3_3)) {
            ep0_setup_buf_len = sizeof(descriptor_type_string_3_3);
        }
        RTK_USB_DEVICE_DBG_SIMPLE( "config_size_2 len=%d\n", ep0_setup_buf_len);
        dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)descriptor_type_string_3_3, ep0_setup_buf_len);
    } else if (loca_req_data[2] == 0x4) {
        if( ep0_setup_buf_len > sizeof(descriptor_type_string_4_3)) {
            ep0_setup_buf_len = sizeof(descriptor_type_string_4_3);
        }
        RTK_USB_DEVICE_DBG_SIMPLE( "config_size_2 len=%d\n", ep0_setup_buf_len);
        dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)descriptor_type_string_4_3, ep0_setup_buf_len);
    } else if (loca_req_data[2] == 0x5) {
        if( ep0_setup_buf_len > sizeof(descriptor_type_string_5_3)) {
            ep0_setup_buf_len = sizeof(descriptor_type_string_5_3);
        }
        RTK_USB_DEVICE_DBG_SIMPLE( "config_size_2 len=%d\n", ep0_setup_buf_len);
        dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)descriptor_type_string_5_3, ep0_setup_buf_len);
    } else if (loca_req_data[2] == 0x6) {
        if( ep0_setup_buf_len > sizeof(descriptor_type_string_6_3)) {
            ep0_setup_buf_len = sizeof(descriptor_type_string_6_3);
        }
        RTK_USB_DEVICE_DBG_SIMPLE( "config_size_2 len=%d\n", ep0_setup_buf_len);
        dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)descriptor_type_string_6_3, ep0_setup_buf_len);
    } else {
        if( ep0_setup_buf_len > sizeof(descriptor_type_string_3_3)) {
            ep0_setup_buf_len = sizeof(descriptor_type_string_3_3);
        }
        RTK_USB_DEVICE_DBG_SIMPLE( "config_size_2 len=%d\n", ep0_setup_buf_len);
        dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)descriptor_type_string_3_3, ep0_setup_buf_len);
    }
    dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+ ep0_setup_buf_len);
    outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY + ep0_setup_buf_len);
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,    TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,    0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,    ep0_setup_buf_len);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
    
}

void dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_bos(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "GET DESCRIPTOR BOS\n");
    request      = USB_REQ_GET_DESCRIPTOR;
    request_type = USB_DT_BOS;
    
    ep0_setup_buf_len =  sizeof(g_bos_descriptor);
    dwc3_copy_to_memory(TRB_BUFFER_cache, (void *)g_bos_descriptor, ep0_setup_buf_len);
    dmac_flush_range( TRB_BUFFER_cache, TRB_BUFFER_cache+ ep0_setup_buf_len);
    outer_flush_range( TRB_BUFFER_PHY, TRB_BUFFER_PHY + ep0_setup_buf_len);
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,    TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,    0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,    ep0_setup_buf_len);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x818, 0x00000000);
    rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x810, 0x00000000);
    rtd_outl(dwc3_offset + 0x81c, 0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x81c);
        if(temp == 0x10006)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_DATA;
}

void dante_loop_process_XFERCOMPLETE_EP0_set_uac_cur(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "UAC Set CUR\n");
    request  = USB_REQ_SET_SEL;
    
    // 0x682 0x4030700 0x0
    //cmd= 0x401 EP=0x2
    rtd_outl(dwc3_offset + 0x860, 0x00000000); //P2
    rtd_outl(dwc3_offset + 0x864, 0x04030700); //P1
    rtd_outl(dwc3_offset + 0x868, 0x00000682); //P0
    rtd_outl(dwc3_offset + 0x86c, 0x00000401); //cmd
    
    while(1){
        temp = rtd_inl(dwc3_offset + 0x82c);
        if(temp == 0x1)
            break;
    }
    RTK_USB_DEVICE_DBG_SIMPLE( "Ln%d cmd return=%x\n", __LINE__, (UINT32)temp);
    
    //[USB] 0x1 0x0 0x0
    //[USB] cmd= 0x402 EP=0x2
    rtd_outl(dwc3_offset + 0x860, 0x00000000); //P2
    rtd_outl(dwc3_offset + 0x864, 0x00000000); //P1
    rtd_outl(dwc3_offset + 0x868, 0x00000001); //P0
    rtd_outl(dwc3_offset + 0x86c, 0x00000402); //cmd
    while(1){
        temp = rtd_inl(dwc3_offset + 0x86c);
        if( (temp&0xffff) == 0x2)
            break;
    }
    RTK_USB_DEVICE_DBG_SIMPLE( "Ln%d cmd return=%x\n", __LINE__, (UINT32)temp);
    
    
    rtd_outl(dwc3_offset + 0x720, 0x00000007);
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,  TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,  0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  0x00000200);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x808 ,0x00000000);
    rtd_outl(dwc3_offset + 0x804 ,TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x800 ,0x00000000);
    rtd_outl(dwc3_offset + 0x80c ,0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x80c);
        if(temp == 0x6)
            break;
    }
    current_state = STAGE_DATA;
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    init_uac_host_to_device_sram_core();
}

void dante_loop_process_XFERCOMPLETE_EP0_set_sel(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "SET SEL\n");
    request  = USB_REQ_SET_SEL;
    
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,  TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,  0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  0x00000200);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c53);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x808 ,0x00000000);
    rtd_outl(dwc3_offset + 0x804 ,TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x800 ,0x00000000);
    rtd_outl(dwc3_offset + 0x80c ,0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x80c);
        if(temp == 0x6)
            break;
    }
    current_state = STAGE_DATA;
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
}

void dante_loop_process_XFERCOMPLETE_EP0_STAGE_SETUP(void)
{
    temp = (UINT32)rtd_mem_inl(TRB_BUFFER + 0);
    loca_req_data[0] = (UINT32)temp & 0xff;
    loca_req_data[1] = ((UINT32)temp >> 8) & 0xff;
    loca_req_data[2] = ((UINT32)temp >> 16) & 0xff;
    loca_req_data[3] = ((UINT32)temp >> 24) & 0xff;
    
    temp = (UINT32)rtd_mem_inl(TRB_BUFFER + 4);
    loca_req_data[4] = (UINT32)temp & 0xff;
    loca_req_data[5] = ((UINT32)temp >> 8) & 0xff;
    loca_req_data[6] = ((UINT32)temp >> 16) & 0xff;
    loca_req_data[7] = ((UINT32)temp >> 24) & 0xff;
    RTK_USB_DEVICE_DBG_SIMPLE( "ctrl_req = 0x%x 0x%x 0x%x 0x%x\n", (UINT32)loca_req_data[0], (UINT32)loca_req_data[1], (UINT32)loca_req_data[2], (UINT32)loca_req_data[3] );
    RTK_USB_DEVICE_DBG_SIMPLE( "           0x%x 0x%x 0x%x 0x%x\n", (UINT32)loca_req_data[4], (UINT32)loca_req_data[5], (UINT32)loca_req_data[6], (UINT32)loca_req_data[7] );
    RTK_USB_DEVICE_DBG_SIMPLE( "dwc3_offset + 0x700 = 0x%x (Except is 0x80804)\n", (UINT32)rtd_inl(dwc3_offset + 0x700));
    
    if(loca_req_data[1] == 0x05) {
        dante_loop_process_XFERCOMPLETE_EP0_set_address();
    }
    
    if(loca_req_data[1] == 0x03) {
        dante_loop_process_XFERCOMPLETE_EP0_set_feature();
    }
    
    //21 0a 00 00  04 00 00 00
    if(loca_req_data[0] == 0x21 && loca_req_data[1] == 0x0a) {
        dante_loop_process_XFERCOMPLETE_EP0_set_idel();
    }
    
    if(loca_req_data[1] == 0x0b) {
        dante_loop_process_XFERCOMPLETE_EP0_set_interface();
    }
    
    if(loca_req_data[1] == 0x09) {
        dante_loop_process_XFERCOMPLETE_EP0_set_configuration();
    }
    
    if(loca_req_data[1] == 0x08) {
        dante_loop_process_XFERCOMPLETE_EP0_get_configuration();
    }
    
    if(loca_req_data[1] == 0x01) {
        dante_loop_process_XFERCOMPLETE_EP0_clear_feature();
    }
    
    if(loca_req_data[1] == 0x00) {
        dante_loop_process_XFERCOMPLETE_EP0_get_status();
    }
    
    // a1 85 00 0a  00 03 02 00
    if(loca_req_data[0]== 0xa1 && loca_req_data[1]== 0x85 && loca_req_data[5]== 0x03) {
        dante_loop_process_XFERCOMPLETE_EP0_get_len();
    }
    
    //a1 86 00 0a  00 03 01 00
    if(loca_req_data[0]== 0xa1 && loca_req_data[1]== 0x86 && loca_req_data[5]== 0x03) {
        dante_loop_process_XFERCOMPLETE_EP0_get_info();
    }
    
    // a1 82 00 0a  00 03 08 00
    if(loca_req_data[0]== 0xa1 && loca_req_data[1]== 0x82 && loca_req_data[5]== 0x03) {
        dante_loop_process_XFERCOMPLETE_EP0_get_min();
    }
    
    // a1 83 00 0a  00 03 08 00
    if(loca_req_data[0]== 0xa1 && loca_req_data[1]== 0x83 && loca_req_data[5]== 0x03) {
        dante_loop_process_XFERCOMPLETE_EP0_get_max();
    }
    
    // a1 84 00 0a  00 03 08 00
    if(loca_req_data[0]== 0xa1 && loca_req_data[1]== 0x84 && loca_req_data[5]== 0x03) {
        dante_loop_process_XFERCOMPLETE_EP0_get_res();
    }
    
    // a1 87 00 0a  00 03 08 00
    if(loca_req_data[0]== 0xa1 && loca_req_data[1]== 0x87 && loca_req_data[5]== 0x03) {
        dante_loop_process_XFERCOMPLETE_EP0_get_def();
    }
    
    if(loca_req_data[1] == 0x06 && loca_req_data[3]==0x01) {
        dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_device();
    }
    if(loca_req_data[1] == 0x06 && loca_req_data[3]==0x02) {
        dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_config();
    }
    if(loca_req_data[1] == 0x06 && loca_req_data[3]==0x22) {
        dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_USB_DT_HID_REPORT();
    }
    if(loca_req_data[1] == 0x06 && loca_req_data[3]==0x06) {
        dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_QUALIFIER();
    }
    if(loca_req_data[1] == 0x06 && loca_req_data[3]==0x07) {
        dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_other_speed_config();
    }
    
    if(loca_req_data[1] == 0x06 && loca_req_data[3]==0x03) {
        dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_string();
    }
    if(loca_req_data[1] == 0x06 && loca_req_data[3]==0x0f) {
        dante_loop_process_XFERCOMPLETE_EP0_get_descriptor_bos();
    }
    
    if(loca_req_data[1] == 0x31) {
        RTK_USB_DEVICE_DBG_SIMPLE( "SET ISOCH Delay\n");
        request  = USB_REQ_SET_ISOCH_DELAY;
    }
    
    if(loca_req_data[0] == 0x22 && loca_req_data[1] == 0x01) {
        dante_loop_process_XFERCOMPLETE_EP0_set_uac_cur();
    }
    
    if(loca_req_data[1] == 0x30) {
        dante_loop_process_XFERCOMPLETE_EP0_set_sel();
    }
}

void dante_loop_process_XFERCOMPLETE_EP0_STAGE_STATUS(void)
{
    UINT32 temp;

    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,    TRB_BUFFER_PHY);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,    0x00000000);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,    0x00000008);
    rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c23);
    dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
    outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
    rtd_outl(dwc3_offset + 0x808 ,0x00000000);
    rtd_outl(dwc3_offset + 0x804 ,TRB_ARRAY_BUFFER_PHY);
    rtd_outl(dwc3_offset + 0x800 ,0x00000000);
    rtd_outl(dwc3_offset + 0x80c ,0x00000406);
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    while(1){
        temp = rtd_inl(dwc3_offset + 0x80c);
        if(temp == 0x6)
            break;
    }
    //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    current_state = STAGE_SETUP;
}

void dante_loop_process_XFERCOMPLETE_EP0(void)
{
    RTK_USB_DEVICE_INFO( "[EVENT] = 0x%x DWC3_DEPEVT_XFERCOMPLETE ep=0\n", (UINT32)event_value);
    RTK_USB_DEVICE_INFO( "ctrl_req addr = 0x%x\n", (UINT32)TRB_BUFFER_PHY);
    
    if(current_state == STAGE_SETUP) {
        dante_loop_process_XFERCOMPLETE_EP0_STAGE_SETUP();
    } else if(current_state == STAGE_STATUS) {
        dante_loop_process_XFERCOMPLETE_EP0_STAGE_STATUS();
    } else if(current_state == STAGE_DATA) {
        current_state = STAGE_STATUS;
    }
}

void dante_loop_process_XFERNOTREADY_EP1(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "[EVENT] = 0x%x DWC3_DEPEVT_XFERNOTREADY ep=1\n", (UINT32)event_value);
    
    if(request == USB_REQ_SET_SEL) {
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,  TRB_BUFFER_PHY);
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,  0x00000000);
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  0x00000000);
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c43);
        dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
        outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
        rtd_outl(dwc3_offset + 0x818, 0x00000000);
        rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
        rtd_outl(dwc3_offset + 0x810, 0x00000000);
        rtd_outl(dwc3_offset + 0x81c, 0x00000406);
        //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
        while(1){
            temp = rtd_inl(dwc3_offset + 0x81c);
            if(temp == 0x10006)
                break;
        }
        //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    }
    
    if(request == USB_REQ_SET_ADDRESS || request == USB_REQ_SET_ISOCH_DELAY || request == USB_REQ_SET_INTERFACE || request == USB_REQ_CLEAR_FEATURE || request == USB_REQ_SET_FEATURE || request == USB_REQ_SET_CONFIGURATION || request == USB_REQ_GET_CONFIGURATION) {
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,  TRB_BUFFER_PHY);
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,  0x00000000);
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  0x00000000);
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c33);
        dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
        outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
        rtd_outl(dwc3_offset + 0x818, 0x00000000);
        rtd_outl(dwc3_offset + 0x814, TRB_ARRAY_BUFFER_PHY);
        rtd_outl(dwc3_offset + 0x810, 0x00000000);
        rtd_outl(dwc3_offset + 0x81c, 0x00000406);
        //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
        while(1){
            temp = rtd_inl(dwc3_offset + 0x81c);
            if(temp == 0x10006)
                break;
        }
        //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
    }
}

void dante_loop_process_XFERCOMPLETE_EP1(void)
{
    RTK_USB_DEVICE_INFO( "[EVENT] = 0x%x DWC3_DEPEVT_XFERCOMPLETE ep=1\n", (UINT32)event_value);
    
    if(request == USB_REQ_SET_ADDRESS || request == USB_REQ_SET_ISOCH_DELAY || request == USB_REQ_SET_INTERFACE || request == USB_REQ_CLEAR_FEATURE|| request == USB_REQ_SET_SEL || request == USB_REQ_SET_FEATURE || request == USB_REQ_SET_CONFIGURATION || request == USB_REQ_GET_CONFIGURATION) {
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 0,  TRB_BUFFER_PHY);
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 4,  0x00000000);
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 8,  0x00000008);
        rtd_mem_outl(TRB_ARRAY_BUFFER_cache + 12, 0x00000c23);
        dmac_flush_range( TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER_cache+16);
        outer_flush_range( TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_PHY + 16);
    
        rtd_outl(dwc3_offset + 0x808 ,0x00000000);
        rtd_outl(dwc3_offset + 0x804 ,TRB_ARRAY_BUFFER_PHY);
        rtd_outl(dwc3_offset + 0x800 ,0x00000000);
        rtd_outl(dwc3_offset + 0x80c ,0x00000406);
        //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
        while(1){
            temp = rtd_inl(dwc3_offset + 0x80c);
            if(temp == 0x6)
                break;
        }
        //RTK_USB_DEVICE_DBG_SIMPLE( "test %d\n", __LINE__);
        current_state = STAGE_SETUP;
    }
    
    if(request == USB_REQ_GET_DESCRIPTOR || request == USB_REQ_GET_STATUS || request == USB_REQ_CLASS_GET_LEN || request == USB_REQ_CLASS_GET_INFO || request==USB_REQ_CLASS_GET_MIN || request==USB_REQ_CLASS_GET_MAX || request==USB_REQ_CLASS_GET_RES || request==USB_REQ_CLASS_GET_DEF) {
        //  do nothing
        current_state = STAGE_STATUS;
    }
}

void dante_loop_process_default(void)
{
    if( (event_value&0xffff) == 0x00c4) {
        // uac
        //RTK_USB_DEVICE_DBG_SIMPLE( "[EVENT] = 0x%x DWC3_DEPEVT_XFERNOTREADY ep=2\n", (UINT32)event_value);
        //P0:0x0 P1:0x1a240000 P2:0x0
        //cmd= 0x38680506 EP=0x2
    
        frame_number = rtd_inl(dwc3_offset + 0x70c);
        frame_number = ((UINT32)frame_number>>3) & 0x3fff;
        frame_number = frame_number + 23;
    
        rtd_outl(dwc3_offset + 0x820, 0x00000000); //P2
        rtd_outl(dwc3_offset + 0x824, UAC_TRB_ARRAY_BUFFER_PHY); //P1
        rtd_outl(dwc3_offset + 0x828, 0x00000000); //P0
    
        while(1){
            cmd = (((UINT32)(frame_number)<<16) & 0xfff80000) | 0x406;
            rtd_outl(dwc3_offset + 0x82c, cmd); //cmd
            while(1){
                temp = rtd_inl(dwc3_offset + 0x82c);
                if( (temp&0x00000FFF) == 0x6)
                    break;
                RTK_USB_DEVICE_DBG_SIMPLE( "%x\n", (UINT32)temp);
            }
            if( (temp & 0x00002000) == 0)
                break;
            RTK_USB_DEVICE_DBG_SIMPLE( "retry\n");
            frame_number = frame_number + 8;
        }
        RTK_USB_DEVICE_DBG_SIMPLE( "Ln%d cmd return=%x\n", __LINE__, (UINT32)temp);
        RTK_USB_DEVICE_DBG_SIMPLE( "event frame=%x cmd=%x(%x) DSTS=%x\n", ((UINT32)event_value>>16)&0x3fff, cmd&0x3fff, cmd, ((UINT32)rtd_inl(0xb805870c)>>3) & 0x3fff);
    
    } else {
        RTK_USB_DEVICE_DBG_SIMPLE( "UnKnow Event 0x%x\n", event_value);
    }
}

int dante_loop(void *data)
{
    init_waitqueue_head(&waitq_test);

    dante_loop_init_memory();
    USB_init();

	trb_address_current = EVENT_BUFFER;

	current_state = STAGE_SETUP;
	while(1) {
		temp = rtd_inl(dwc3_offset + 0x40c);
		if(temp != 0x00000000) {
			//RTK_USB_DEVICE_DBG_SIMPLE("dwc3_offset + 0x408 = %x\n", (UINT32)rtd_inl(dwc3_offset + 0x408));
			rtd_outl(dwc3_offset + 0x408 ,0x80000100);
			event_value = *((unsigned long*)trb_address_current);
			trb_address_current = trb_address_current + 4;
			if(trb_address_current >= (EVENT_BUFFER + 0x100))
				trb_address_current = EVENT_BUFFER;

			switch(event_value) {
				case 0x100301:
				case 0x130301:
				case 0x150301:
				case 0x160301:
				case 0x170301:
				case 0x40301:
					RTK_USB_DEVICE_DBG_SIMPLE( "[EVENT] = 0x%x DWC3_DEVICE_EVENT_LINK_STATUS_CHANGE =%d\n", event_value, ((UINT32)event_value>>16) & 0x0f);
					break;
				case 0x101:
					RTK_USB_DEVICE_DBG_SIMPLE( "[EVENT] = 0x%x DWC3_DEVICE_EVENT_RESET\n", (UINT32)event_value);
					rtd_outl(dwc3_offset + 0x700, 0x00080804);
					break;
				case 0x201:
                    dante_loop_process_connect_done();
					break;
				case 0x10c0:
				case 0x20c0:
                    dante_loop_process_XFERNOTREADY_EP0();
					break;
				case 0xc040:
				case 0xe040:
                    dante_loop_process_XFERCOMPLETE_EP0();
					break;
				case 0x10c2:
				case 0x20c2:
                    dante_loop_process_XFERNOTREADY_EP1();
					break;
				case 0xc042:
                    dante_loop_process_XFERCOMPLETE_EP1();
					break;
				default:
                    dante_loop_process_default();
					break;;
			}
			
			rtd_outl(dwc3_offset + 0x40c ,0x00000004);
			//RTK_USB_DEVICE_DBG_SIMPLE("dwc3_offset + 0x408 = %x\n", (UINT32)rtd_inl(dwc3_offset + 0x408));
			rtd_outl(dwc3_offset + 0x408 ,0x00000100);
		}
	}
	return 0;
}

void rtk_usb_device_alloc_memory(void)
{
    //EVENT_BUFFER         = dma_alloc_coherent(dwc->sysdev, 0x1000, &EVENT_BUFFER_PHY, GFP_KERNEL);
    //TRB_BUFFER           = dma_alloc_coherent(dwc->sysdev, 0x1000, &TRB_BUFFER_PHY, GFP_KERNEL);
    //TRB_ARRAY_BUFFER     = dma_alloc_coherent(dwc->sysdev, 0x1000, &TRB_ARRAY_BUFFER_PHY, GFP_KERNEL);
    //UAC_TRB_BUFFER       = dma_alloc_coherent(dwc->sysdev, 0x1000, &UAC_TRB_BUFFER_PHY, GFP_KERNEL);
    //UAC_TRB_ARRAY_BUFFER = dma_alloc_coherent(dwc->sysdev, 0x1000, &UAC_TRB_ARRAY_BUFFER_PHY, GFP_KERNEL);

    EVENT_BUFFER_cache = (UINT8*)dvr_malloc_uncached_specific( 0x1000, GFP_DCU2, (void*)&EVENT_BUFFER );
    EVENT_BUFFER_PHY = dvr_to_phys( (void*)EVENT_BUFFER_cache ) ;    /* translate pack buffer from virtual & cached to phys */
    RTK_USB_DEVICE_DBG_SIMPLE(  "EVENT_BUFFER_PHY=%x EVENT_BUFFER_cache=%px EVENT_BUFFER=%px\n", EVENT_BUFFER_PHY, EVENT_BUFFER_cache, EVENT_BUFFER);

    TRB_BUFFER_cache = (UINT8*)dvr_malloc_uncached_specific( 0x1000, GFP_DCU2, (void*)&TRB_BUFFER );
    TRB_BUFFER_PHY = dvr_to_phys( (void*)TRB_BUFFER_cache ) ;    /* translate pack buffer from virtual & cached to phys */
    RTK_USB_DEVICE_DBG_SIMPLE(  "TRB_BUFFER_PHY=%x TRB_BUFFER_cache=%px TRB_BUFFER=%px\n", TRB_BUFFER_PHY, TRB_BUFFER_cache, TRB_BUFFER);

    TRB_ARRAY_BUFFER_cache = (UINT8*)dvr_malloc_uncached_specific( 0x1000, GFP_DCU2, (void*)&TRB_ARRAY_BUFFER );
    TRB_ARRAY_BUFFER_PHY = dvr_to_phys( (void*)TRB_ARRAY_BUFFER_cache ) ;    /* translate pack buffer from virtual & cached to phys */
    RTK_USB_DEVICE_DBG_SIMPLE(  "TRB_ARRAY_BUFFER_PHY=%x TRB_ARRAY_BUFFER_cache=%px TRB_ARRAY_BUFFER=%px\n", TRB_ARRAY_BUFFER_PHY, TRB_ARRAY_BUFFER_cache, TRB_ARRAY_BUFFER);

    UAC_TRB_BUFFER_cache = (UINT8*)dvr_malloc_uncached_specific( 0x1000, GFP_DCU2, (void*)&UAC_TRB_BUFFER );
    UAC_TRB_BUFFER_PHY = dvr_to_phys( (void*)UAC_TRB_BUFFER_cache ) ;    /* translate pack buffer from virtual & cached to phys */
    RTK_USB_DEVICE_DBG_SIMPLE(  "UAC_TRB_BUFFER_PHY=%x UAC_TRB_BUFFER_cache=%px UAC_TRB_BUFFER=%px\n", UAC_TRB_BUFFER_PHY, UAC_TRB_BUFFER_cache, UAC_TRB_BUFFER);

    UAC_TRB_ARRAY_BUFFER_cache = (UINT8*)dvr_malloc_uncached_specific( 0xfd2000, GFP_DCU2, (void*)&UAC_TRB_ARRAY_BUFFER );
    UAC_TRB_ARRAY_BUFFER_PHY = dvr_to_phys( (void*)UAC_TRB_ARRAY_BUFFER_cache ) ;    /* translate pack buffer from virtual & cached to phys */
    RTK_USB_DEVICE_DBG_SIMPLE(  "UAC_TRB_ARRAY_BUFFER_PHY=%x UAC_TRB_ARRAY_BUFFER_cache=%px UAC_TRB_ARRAY_BUFFER=%px\n", UAC_TRB_ARRAY_BUFFER_PHY, UAC_TRB_ARRAY_BUFFER_cache, UAC_TRB_ARRAY_BUFFER);
}

module_init(rtk_usb_device_module_init);
module_exit(rtk_usb_device_module_exit);
MODULE_IMPORT_NS(VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver);
MODULE_AUTHOR( "Realtek, Realtek Semiconductor");
MODULE_LICENSE( "GPL");
