
#ifndef _RTK_OSDEXT_DRIVER_H_
#define _RTK_OSDEXT_DRIVER_H_

#include "rtk_osdExt_types.h"
#include <gal/rtk_gdma_driver.h>

//kernel module 
#include <generated/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>   /* DBG_PRINT()    */
#include <linux/slab.h>     /* kmalloc()      */
#include <linux/fs.h>       /* everything...  */
#include <linux/errno.h>    /* error codes    */
#include <linux/types.h>    /* size_t         */
#include <linux/cdev.h>
#include <linux/dma-mapping.h>
#include <linux/uaccess.h>    /* copy_*_user    */
#include <linux/jiffies.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/pageremap.h>
#include <linux/random.h>
#include <linux/pageremap.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>  /* for threads */
#include <linux/time.h>   /* for using jiffies */
#include <asm/barrier.h> /*dsb()*/
#include <asm/cacheflush.h>


//#include <mach/irqs.h>
#include <mach/platform.h>
#include <mach/system.h>


 //RTK local
//#include <mach/rtk_log.h>

#if 0
#include <panelConfigParameter.h>
#include <tvscalercontrol/panel/panelapi.h>
#include "tvscalercontrol/panel/panelapi.h"
#endif//

#include <rtk_kdriver/io.h> //rtd_inl
#include <rtd_log/rtd_module_log.h>


#if 1
  #define OSD_RTD_INL(a) rtd_inl(a)

#else 
    //not tested yet !
    #define OSD_RTD_OUTL(addr, val) { \
        rtd_pr_gdma_crit("rtd_inl(0x%08x, 0x%08x);\n", addr-0xA0000000, val); \
        rtd_inl(addr); \
    } while(0)

#endif//


#if 1 

  #define OSD_RTD_OUTL rtd_outl

#else  //
// addr-0xA0000000
#define OSD_RTD_OUTL(addr, val) \
    do { \
        rtd_pr_gdma_crit("rtd_outl(0x%08x, 0x%08x);\n", addr, val); \
        rtd_outl(addr, val); \
        rtd_pr_gdma_crit("\t addr: 0x%08x, got 0x%08x   %s %d\n", addr, rtd_inl(addr), __FUNCTION__, __LINE__ ); \
    } while(0)

#endif//


#if 1

#define GDMA_OUTL rtd_outl

#else 	//kkk test only 
#define GDMA_OUTL(addr, val) \
    do { \
        rtd_pr_gdma_crit("rtd_outl(0x%08x, 0x%08x);\n", addr, val); \
        rtd_outl(addr, val); \
        rtd_pr_gdma_crit("\t addr: 0x%08x, got 0x%08x   %s %d\n", addr, rtd_inl(addr), __FUNCTION__, __LINE__ ); \
    } while(0)

#endif




#define GDMAExt_MAX_PIC_Q_SIZE        32 /* 8 */

//#define #define RTK_GDMA_ENABLE_AFBC
#define RTK_GDMA_ENABLE_TFBC


#define GDMAOSD_MOD_HAS_MODE(mode, bitfields) (((mode) & (bitfields)) == (bitfields))

typedef enum  {
    GDMAEXT_OSD_MODE_DEFAULT = 0, //osd 1,4,5
    GDMAEXT_OSD_MODE_OSD1 = 0x00000001,

    GDMAEXT_OSD_MODE_OSD4 = 0x00000002,
    GDMAEXT_OSD_MODE_OSD5 = 0x00000004,
    GDMAEXT_OSD_MODE_OSD_45 = GDMAEXT_OSD_MODE_OSD4 | GDMAEXT_OSD_MODE_OSD5,

    GDMAEXT_OSD_MODE_OSD_DUAL = 0x00000008, // 4+5 only, to distinguish osd 1+ 4+5 

    //test mode
    #if 0 
    GDMAEXT_OSD_MODE_TEST_OSD4_ONLY = 0X10000000,  //bit [28]
    GDMAEXT_OSD_MODE_TEST_OSD5_ONLY = 0X20000000,  //[29]
    GDMAEXT_OSD_MODE_TEST_OSD_45_ONLY = GDMAEXT_OSD_MODE_TEST_OSD5_ONLY | GDMAEXT_OSD_MODE_TEST_OSD4_ONLY,
    #endif//


} GDMAEXT_OSD_MODE;

typedef struct {
    GDMA_WIN_NEXT_ADDR nxtAddr;
    /* Window Region */
    GDMA_WIN_XY       winXY;
    GDMA_WIN_WH       winWH;
    /* Window Attribute */
    GDMA_WIN_ATTR     attr;
    unsigned int      CLUT_addr;
    GDMA_WIN_KEY      colorKey;
    union {
		unsigned int      top_addr;    /* top or progressive OSD start address (sequential) or index (block mode) */
		unsigned int      fbdc1_addr;
    };
    union {
		unsigned int      bot_addr;    /* bottom OSD start address (sequential) or index (block mode) */
		unsigned int      fbdc2_addr;
    };
    union {
		unsigned int      pitch;
		unsigned int      fbdc3_addr;
    };
    GDMA_WIN_INIT     objOffset;
#ifndef BUILD_QUICK_SHOW

#ifdef RTK_GDMA_ENABLE_AFBC
    unsigned int reserved[2];
    AFBC_FORMAT             afbc_format;
    AFBC_TOTAL_TILE         afbc_totaltile;
    AFBC_PIC_PIXEL_XY       afbc_xy_pixel;
    AFBC_START_XY           afbc_xy_sta;
#elif defined(RTK_GDMA_ENABLE_TFBC)
    FBDC_PREBLEND_ATTR      fbdc_attr;
    FBDC_FORMAT             fbdc_format;
    FBDC_ALPHA_ARGB         fbdc_alpha_argb;
    FBDC_BLEND_FACTOR       fbdc_blend1st;
    FBDC_BLEND_FACTOR       fbdc_blend2nd;
    FBDC_TOTAL_TILE         fbdc1_totaltile;
    FBDC_PIC_PIXEL_XY       fbdc1_xy_pixel;
    FBDC_START_XY           fbdc1_xy_sta;
    FBDC_START_XY           fbdc1_preblend_xy_sta;
    FBDC_PLANEALPHA         fbdc_plane_alpha_c0;
    FBDC_TOTAL_TILE         fbdc2_totaltile;
    FBDC_PIC_PIXEL_XY       fbdc2_xy_pixel;
    FBDC_START_XY           fbdc2_xy_sta;
    FBDC_START_XY           fbdc2_preblend_xy_sta;
    FBDC_PLANEALPHA         fbdc_plane_alpha_c1;
    FBDC_TOTAL_TILE         fbdc3_totaltile;
    FBDC_PIC_PIXEL_XY       fbdc3_xy_pixel;
    FBDC_START_XY           fbdc3_xy_sta;
    FBDC_START_XY           fbdc3_preblend_xy_sta;
    FBDC_PLANEALPHA         fbdc_plane_alpha_c2;	/* 30 words for osd win info. */
    unsigned int reserved[2];
#else
    #error "invalid disp compress config!"

#endif

#endif

    /*  below size should be multiple of 16 s.t every element of GDMA_WIN array can be located at 16 byte-aligned address */
    GDMA_WIN_CTRL     ctrl __attribute__((aligned(16)));
    unsigned int      srcImg[2];
    unsigned short    srcPitch[2];
    unsigned short    dst_x;
    unsigned short    dst_y;
    unsigned int      dst_width;
    unsigned int      dst_height;
    bool used;

} GDMA_4_WIN;

#define OSD_PLANE_MAX_COUNT (5)

#define GDMA_STATUS_OFF  (0)
#define GDMA_STATUS_ON  (1)

typedef enum {
	GDMA_STATUS_INTR_ROUTING,
	
} GDMAExt_Status;


typedef struct {
    //OSD_DISPLAY_PLANE plane;
    GDMA_DISPLAY_PLANE plane;

    GDMA_PIC_STATUS      status;
    unsigned short       repeatCnt;
    unsigned int         context;
    SEG_NUMBER seg_num;

    GDMA_WIN             OSDwin    __attribute__((aligned(16)));

    osd_planealpha_ar plane_ar;     /*  fbdc only 8 bits, mixer has 9 bits */
    osd_planealpha_gb plane_gb;     /*  fbdc only 8 bits, mixer has 9 bits */
    osd_clear_x clear_x;
    osd_clear_y clear_y;
	int cookie;
    //GDMA_PIC_MATCH_SYNCSTAMP syncInfo;
    int show;					/* 1: show this plane. 0:hide */

    GDMA_REG_CONTENT reg_content;   /*  pre-calculate by workqueue and store */

    E_BLEND_ORDER onlineOrder;          /*  for final display use */

	int scale_factor;	/* layer scale factor based on panel size & fb size */
    
} GDMAExt_PIC_DATA;




#endif// _RTK_OSDEXT_DRIVER_H_
