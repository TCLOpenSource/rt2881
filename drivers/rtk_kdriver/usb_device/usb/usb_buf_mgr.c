#include "arch.h"
#include <rbus/timer_reg.h>
#include "mach.h"
#include "usb_buf_mgr.h"

#include <linux/clk.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>

#include <linux/clk.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/of.h>
#include <linux/acpi.h>
#include <linux/pinctrl/consumer.h>
#include <linux/reset.h>

#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb/of.h>
#include <linux/usb/otg.h>


#include <linux/kthread.h>
#include <rtk_kdriver/io.h>


#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/pageremap.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/string.h>

#include "rtk_usb_device_dbg.h"
//#pragma nogcse
//PRAGMA_CODESEG(BANK43);
//PRAGMA_CONSTSEG(BANK43);

UINT32 uac_sram_addr_phy;
//UINT32 uac_sram_addr_vir;
UINT8 *uac_sram_addr_vir_cache;
UINT8 *uac_sram_addr_vir_non_cache;

UINT32 uac_ddr_addr_phy;
//UINT32 uac_ddr_addr_vir;
UINT8 *uac_ddr_addr_vir_cache;
UINT8 *uac_ddr_addr_vir_non_cache;

UINT32 uac_avsync_ddr_addr_phy;
//UINT32 uac_avsync_ddr_addr_vir;
UINT8 *uac_avsync_ddr_addr_vir_cache;
UINT8 *uac_avsync_ddr_addr_vir_non_cache;

UINT32 uvc_sram_addr_phy;
//UINT32 uvc_sram_addr_vir;
UINT8 *uvc_sram_addr_vir_cache;
UINT8 *uvc_sram_addr_vir_non_cache;

UINT8 usb_buffer_status = 0;

extern unsigned long long usb_phy_vir_offset;

#define DMA_UVC 1
#define DMA_UAC 2
#define DMA_UAC_DDR 3
#define DMA_AVSYNC_UAC 4

UINT8 usb_request_usbbuf(enum usb_buf_type buf_type, UINT32 *phyaddr, UINT8 **viraddr, UINT32 *size)
{
    unsigned long long temp;
    switch(buf_type) {
        case USB_SRAM_BUF:
            *phyaddr = uvc_sram_addr_phy;
            *viraddr = uvc_sram_addr_vir_non_cache;
            *size = 0x10000;
            break;
        case UAC_SRAM_BUF:
            *phyaddr = uac_sram_addr_phy;
            *viraddr = uac_sram_addr_vir_non_cache;
            *size = 0x10000;
            break;
        case UAC_DDR_BUF:
            *phyaddr = uac_ddr_addr_phy;
            *viraddr = uac_ddr_addr_vir_non_cache;
            *size = 0x10000;
            break;
        case UAC_AVSYNC_DDR_BUF:
            *phyaddr = uac_avsync_ddr_addr_phy;
            *viraddr = uac_avsync_ddr_addr_vir_non_cache;
            *size = USB_UAC_AVSYNC_DDR_BUFFER_SIZE;
            break;
        case UVC_SRAM_BUF:
            *phyaddr = uvc_sram_addr_phy + 0x1850;
            if( (*phyaddr % 256) != 0) {
                *phyaddr = *phyaddr + 0xff;
                *phyaddr = *phyaddr & 0xffffff00;
            }
            *viraddr = uvc_sram_addr_vir_non_cache + 0x1850;
            temp = (unsigned long long)(*viraddr);
            if( (temp % 256) != 0) {
                temp = temp + 0xff;
                temp = temp & (~((unsigned long long)0xff));
                *viraddr = (UINT8*)temp;
            }
            *size = 0x10000 - 0x1850 - (*phyaddr - uvc_sram_addr_phy);
            break;
        default:
            *phyaddr = 0;
            *viraddr = 0;
            *size = 0;
            break;
    }
    return 0;
}

UINT8 usbbuf_init(void) __banked
{
    uvc_sram_addr_vir_cache = (UINT8*)dvr_malloc_uncached_specific( 0x50000, GFP_DCU2, (void*)&uvc_sram_addr_vir_non_cache );
    uvc_sram_addr_phy = dvr_to_phys( (void*)uvc_sram_addr_vir_cache );
    RTK_USB_DEVICE_DBG_SIMPLE( "uvc_sram_addr_phy=%x uvc_sram_addr_cache=%px uvc_sram_addr_non_cache=%px\n", uvc_sram_addr_phy, uvc_sram_addr_vir_cache, uvc_sram_addr_vir_non_cache);
    usb_phy_vir_offset = (unsigned long long)uvc_sram_addr_vir_non_cache - uvc_sram_addr_phy;
    RTK_USB_DEVICE_DBG_SIMPLE( "usb_phy_vir_offset=%lx\n", __LINE__, usb_phy_vir_offset);


    uac_sram_addr_vir_cache     = uvc_sram_addr_vir_cache     + 0x20000;
    uac_sram_addr_vir_non_cache = uvc_sram_addr_vir_non_cache + 0x20000;
    uac_sram_addr_phy           = uvc_sram_addr_phy           + 0x20000;
    RTK_USB_DEVICE_DBG_SIMPLE( "uac_sram_addr_phy=%x uac_sram_addr_cache=%px uac_sram_addr_non_cache=%px\n", uac_sram_addr_phy, uac_sram_addr_vir_cache, uac_sram_addr_vir_non_cache);

    uac_ddr_addr_vir_cache     = uvc_sram_addr_vir_cache     + 0x30000;
    uac_ddr_addr_vir_non_cache = uvc_sram_addr_vir_non_cache + 0x30000;
    uac_ddr_addr_phy           = uvc_sram_addr_phy           + 0x30000;
    RTK_USB_DEVICE_DBG_SIMPLE( "uac_ddr_addr_phy=%x uac_ddr_addr_cache=%px uac_ddr_addr_non_cache=%px\n", uac_ddr_addr_phy, uac_ddr_addr_vir_cache, uac_ddr_addr_vir_non_cache);

    uac_avsync_ddr_addr_vir_cache     = uvc_sram_addr_vir_cache     + 0x40000;
    uac_avsync_ddr_addr_vir_non_cache = uvc_sram_addr_vir_non_cache + 0x40000;
    uac_avsync_ddr_addr_phy           = uvc_sram_addr_phy           + 0x40000;
    RTK_USB_DEVICE_DBG_SIMPLE( "uac_avsync_ddr_addr_phy=%x uac_avsync_ddr_addr_cache=%px uac_avsync_ddr_addr_non_cache=%px\n", uac_avsync_ddr_addr_phy, uac_avsync_ddr_addr_vir_cache, uac_avsync_ddr_addr_vir_non_cache);
    usb_buffer_status = 1;
    return 0;
}


void dmxbuf_uninit(void)
{
    dvr_free(uvc_sram_addr_vir_cache);
    usb_buffer_status = 0;
}
