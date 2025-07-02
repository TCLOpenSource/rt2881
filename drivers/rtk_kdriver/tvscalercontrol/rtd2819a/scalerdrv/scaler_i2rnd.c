/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2006
  * All rights reserved.
  * ========================================================================*/

/*================= File Description =======================================*/
/**
 * @file
 * 	This file is for scaler related functions.
 *
 * @author 	$Author$
 * @date		$Date$
 * @version	$Revision$
 * @ingroup	scaler
 */

/**
 * @addtogroup scaler
 * @{
 */

/*============================ Module dependency  ===========================*/
//#include <unistd.h>			// 'close()'
//#include <fcntl.h>				// 'open()'
//#include <stdio.h>
//#include <string.h>
//#include <sys/ioctl.h>
//#include <time.h>
//#include <sys/mman.h>		// mmap
#include <linux/delay.h>
#include <generated/autoconf.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/uaccess.h>
#include <asm/cacheflush.h>
#include <rtk_kdriver/RPCDriver.h>
#include <mach/system.h>
#include <mach/timex.h>

#include <linux/pageremap.h>
#include <uapi/linux/const.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>/*DMA*/

#include <rbus/pst_i2rnd_reg.h>
#include <rbus/vodma_reg.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/scaler/scalerstruct.h>

#include <tvscalercontrol/scalerdrv/scaler_i2rnd.h>
#include <tvscalercontrol/scalerdrv/scalerdrv.h>


#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
	#include <scaler/scalerDrvCommon.h>
#else
	#include <scalercommon/scalerCommon.h>
#include <scalercommon/scalerDrvCommon.h>
#endif

#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <scaler_vscdev.h>

//#include <rbus/sensio_reg.h>
#include <rbus/vgip_reg.h>
#include <rbus/main_dither_reg.h>
#include <rbus/rgb2yuv_reg.h>
//#include <rbus/peaking_reg.h> //remove from merlin4
// #include <rbus/idcti_reg.h> //FIXME: merlin8 compile error
#include <rbus/histogram_reg.h>
#include <rbus/profile_reg.h>
// #include <rbus/ringfilter_reg.h> //FIXME: merlin8 compile error
#include <rbus/hsd_dither_reg.h>
#include <rbus/mpegnr_reg.h>
#include <rbus/di_reg.h>
#include <rbus/nr_reg.h>
#include <rbus/iedge_smooth_reg.h>
#include <rbus/scaledown_reg.h>
#include <rbus/i_bist_bisr_reg.h>
#include <rbus/mdomain_cap_reg.h>

// #include <rbus/de_xcxl_reg.h> //k25lp removed
#include <rbus/tc_reg.h>
#include <rbus/color_mb_peaking_reg.h>
#include <rbus/ipq_decontour_reg.h>
// #include <rbus/dfilter_reg.h> //FIXME: merlin8 compile error
#include <rbus/mdomain_disp_reg.h>


#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_vgip.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_color_mb_peaking.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_De_xcxl.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Dfilter.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_di.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Histogram.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Hsd_dither.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_IDCTI.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_iedge_smooth.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_ipq_decontour.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Main_Dither.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Mpegnr.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_nr.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Peaking.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Profile.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_rgb2yuv.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_ringfilter.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Scaledown.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_VODMA.h>
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Mdomain_cap.h>
#include <tvscalercontrol/scalerdrv/scaler_pst.h>

//num==,0->not I2run reg,1->I2run reg,2->I2Run reg,data port reg,->Group3 register
unsigned char vgip_0[33]={0,0,1,1,1,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0};
unsigned char color_mb_peaking_0[17]={1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1};
unsigned char De_xcxl_0[47]={1,1,1,1,1,0,0,1,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,0,1,1,1,0,1,\
     0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,1,1};
unsigned char Dfilter_0[2]={1,1};
unsigned char di_0[49]={1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,\
     0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,1};
unsigned char di_1[355]={1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,0,1,0,0,0,1,1,0,\
     0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,1,0,1,\
     0,0,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,\
     1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,\
     1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,1,\
     1,1,1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,1,0,0,0,0,0,0,\
     0,0,0,0,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     1,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,\
     1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char di_2[108]={1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,\
     1,1,0,0,1,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,1,\
     1,1,1,1,0,1,1,1,1,0,0,0,0,0,1,1,1,1};
unsigned char di_3[46]={1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,3,\
     0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};
unsigned char di_4[54]={1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

unsigned char di_5[12]={1,1,1,1,1,1,1,1,1,1,1,1};
unsigned char di_6[3]={1,1,1};


unsigned char Histogram_0[43]={1,1,1,0,0,0,0,0,1,1,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,\
     0,0,0,0,0,0,0,0,0,0,1,1,1};
unsigned char Hsd_dither_0[47]={1,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,\
     1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0};
unsigned char IDCTI_0[7]={1,1,1,1,1,1,1};
unsigned char iedge_smooth_0[60]={1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,0,1,\
     3,1,3,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1};
unsigned char ipq_decontour_0[5]={3,1,1,1,1};
unsigned char Main_Dither_0[21]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
unsigned char Mpegnr_0[45]={1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,1,1,0,0,1,1,0,0,0,0,0,0,1};
unsigned char nr_0[64]={1,1,1,1,1,1,1,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,\
     1,1,1,1,1,0,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,\
     0,0,0,0};
unsigned char nr_1[64]={0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,\
     1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,3,1,1,1,1,1,1,1,1,0,0,0,0,0,0,\
     0,0,0,0};
unsigned char Peaking_0[5]={1,1,1,1,1}; //remove from merlin4
unsigned char Profile_0[50]={1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,1,0,0,0,1,1,1,1,0,1,0,0,0};
unsigned char Profile_1[54]={1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
unsigned char rgb2yuv_0[1]={1};
unsigned char rgb2yuv_1[13]={1,1,1,1,1,1,1,1,1,1,1,1,0};
unsigned char ringfilter_0[5]={1,1,1,1,1};
unsigned char Scaledown_0[61]={1,1,1,1,1,1,1,1,0,0,1,1,0,0,0,1,1,0,0,1,0,0,1,1,0,0,0,0,0,1,\
     0,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,\
     1};
unsigned char Scaledown_1[32]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,\
     1,1};
unsigned char VODMA_0[512]={1,3,1,1,1,0,0,0,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,\
     3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,\
     1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0};

unsigned char VODMA2_0[512]={1,3,1,1,1,0,0,0,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,\
     3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,\
     1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     0,0};

unsigned char Mdomain_cap_0[62]={1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
     1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,\
     0,0};
unsigned char Mdomain_cap_1[28]={1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1};
unsigned char Mdomain_cap_2[5]={1,1,1,1,1};

unsigned char HDR_VTOP_0[23]={1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1};
unsigned char HDR_VTOP_1[5]={1,1,1,1,1};


#define  DI_REGISTER_NUM_ALL  (DI_REGISTER_NUM_0+ DI_REGISTER_NUM_1+  DI_REGISTER_NUM_2 + DI_REGISTER_NUM_3 + DI_REGISTER_NUM_4 + DI_REGISTER_NUM_5 + DI_REGISTER_NUM_6)
#define PROFILE_REGISTER_NUM_ALL (PROFILE_REGISTER_NUM_0 + PROFILE_REGISTER_NUM_1)
#define RGB2YUV_REGISTER_NUM_ALL (RGB2YUV_REGISTER_NUM_0 + RGB2YUV_REGISTER_NUM_1)
#define NR_REGISTER_NUM_ALL (NR_REGISTER_NUM_0 + NR_REGISTER_NUM_1)
#define SCALEDOWN_REGISTER_NUM_ALL (SCALEDOWN_REGISTER_NUM_0 + SCALEDOWN_REGISTER_NUM_1)
#define MDOMAIN_CAP_REGISTER_NUM_ALL (MDOMAIN_CAP_REGISTER_NUM_0 + MDOMAIN_CAP_REGISTER_NUM_1 + MDOMAIN_CAP_REGISTER_NUM_2)

#define I2RND_REGISTER_NUMBER (VODMA_REGISTER_NUM_0 + VGIP_REGISTER_NUM_0 + COLOR_MB_PEAKING_REGISTER_NUM_0 + \
							   DE_XCXL_REGISTER_NUM_0 + DFILTER_REGISTER_NUM_0 + DI_REGISTER_NUM_ALL + HISTOGRAM_REGISTER_NUM_0 + \
							   HSD_DITHER_REGISTER_NUM_0 + IDCTI_REGISTER_NUM_0 + IEDGE_SMOOTH_REGISTER_NUM_0 + IPQ_DECONTOUR_REGISTER_NUM_0 + \
							   MAIN_DITHER_REGISTER_NUM_0 + MPEGNR_REGISTER_NUM_0 + NR_REGISTER_NUM_ALL + PEAKING_REGISTER_NUM_0 + PROFILE_REGISTER_NUM_ALL + \
							   RGB2YUV_REGISTER_NUM_ALL + RINGFILTER_REGISTER_NUM_0 + SCALEDOWN_REGISTER_NUM_ALL + MDOMAIN_CAP_REGISTER_NUM_ALL + MAIN_TOP_CTL_REGISTER_NUM_0 + MAIN_TOP_CTL_REGISTER_NUM_1)

#define I2RND_REGISTER_SIZE  8
#define REGISTER_VALUE_SIZE  4

#define I2RND_DMA_LENGTH 24 //for 96-bytes align unit 64 bits, 24*8 = 192 bytes //16
#define I2RND_DMA_WATERLEVEL 96

#define I2RND_BLOCK_SIZE ((I2RND_REGISTER_NUMBER + ((I2RND_REGISTER_NUMBER%2) ? 1 : 0)) / 2 * 16) //queue size(total block size) = (regisrer num/2) * 128bits = (regisrer num/2) * 16bytes
//#define I2RND_QUEUE_SIZE (I2RND_BLOCK_SIZE*(I2RND_CMD_NUMBER+1)) //queue size(total block size) = (regisrer num/2) * 128bits = (regisrer num/2) * 16bytes

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif


//i2rnd perind define
#define I2RND_VO_CLOCK_180MHZ 180 //for under 2k1k interlace
#define I2RND_VO_CLOCK_360MHZ 360 //for under 2k1k
#define I2RND_VO_CLOCK_600MHZ 600 //for 4k2k
#define I2RND_VO_CLOCK_594MHZ 594 //for 4k2k
#define I2RND_VO_FIX_H_TOTAL_4K2K 4020
#define I2RND_VO_FIX_V_TOTAL_4K2K 2210
#define I2RND_VO_FIX_V_TOTAL_4K1K 1130
#define I2RND_VO_FIX_H_TOTAL_2K1K 2100
#define I2RND_VO_FIX_V_TOTAL_2K1K_P 1130
#define I2RND_VO_FIX_V_TOTAL_2K1K_I 590
#define VO_FIXED_4K2K_HTOTAL 4400
#define VO_FIXED_4K2K_VTOTAL 2250
#define I2RND_XTAL_CLOCK 27000000 //crystal clock: 27MHz
#define I2RND_TMASTER_TIME 40 //writing time of each register is about 40ns

#define I2RND_ENABLE_ADDR_SIZE 4
#define I2RND_WRITE_CMD_ADDR_SIZE 4
#define I2RND_READ_CMD_ADDR_SIZE 4
#define I2RND_PST_POINT_COUNTER_SIZE 4
#define I2RND_FORCE_WRITE_CMD_SIZE 4
#define I2RND_S1_WRITE_CMD_ADDR_SIZE 4
#define I2RND_S1_READ_CMD_ADDR_SIZE 4
#define I2RND_S1_PST_POINT_COUNTER_SIZE 4
#define I2RND_S0_PST_CHECK_STAGE_SIZE 4
#define I2RND_INFO_SIZE (I2RND_ENABLE_ADDR_SIZE + I2RND_WRITE_CMD_ADDR_SIZE + I2RND_READ_CMD_ADDR_SIZE + I2RND_PST_POINT_COUNTER_SIZE + I2RND_FORCE_WRITE_CMD_SIZE + I2RND_S1_WRITE_CMD_ADDR_SIZE + I2RND_S1_READ_CMD_ADDR_SIZE + I2RND_S1_PST_POINT_COUNTER_SIZE + I2RND_S0_PST_CHECK_STAGE_SIZE)

//i2run sub size condition
#define I2RUN_SUB_SIZE_CONDITION_WIDTH 1920
#define I2RUN_SUB_SIZE_CONDITION_LENGTH 1080

/* Swap bytes in 32 bit value.  */

#define __bswap_constant_32(x) \
  ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
   (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#define __bswap_32(x) __bswap_constant_32 (x)

/*===================================  Types ================================*/
#ifndef UINT8
typedef unsigned char  __UINT8;
#define UINT8 __UINT8
#endif
#ifndef UINT16
typedef unsigned short __UINT16;
#define UINT16 __UINT16
#endif
#ifndef UINT32
typedef unsigned int   __UINT32;
#define UINT32 __UINT32
#endif


/*================================== Variables ==============================*/
//static unsigned int vo_fix_htotal_main_4k2k = I2RND_VO_FIX_H_TOTAL_4K2K;
//static unsigned int vo_fix_vtotal_main_4k2k = I2RND_VO_FIX_V_TOTAL_4K2K;



/*======================== End of File =======================================*/
/**
*
* @}
*/




