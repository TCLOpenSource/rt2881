/*=============================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2005
  * All rights reserved.
  * ============================================ */

/*================= File Description ================= */
/**
 * @file
 * The file is a declaration and definition header file
 *
 * @author 	$Author:  $
 * @date 	$Date:  $
 * @version 	$Revision:  $
 * @ingroup
 */

 /**
 * @addtogroup
 * @{
 */
//Kernel Header file
#ifndef BUILD_QUICK_SHOW
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/pageremap.h>
#include <linux/semaphore.h>
#include <linux/spinlock_types.h>/*For spinlock*/
//#include <mach/io.h>
#else
#include <sysdefs.h>
#include <mach/rtk_platform.h>
#include <no_os/printk.h>
#include <no_os/semaphore.h>
#include <no_os/spinlock.h>
#include <no_os/spinlock_types.h>
#include <no_os/slab.h>
#include <timer.h>
#include <rtk_kdriver/rmm/rmm.h>
#include <rtd_log/rtd_module_log.h>
#include <no_os/pageremap.h>
#endif


//RBUS Header file
#include <rbus/mdomain_cap_reg.h>
#include <rbus/mdom_sub_cap_reg.h>
#include <rbus/mdomain_disp_reg.h>
#include <rbus/smartfit_reg.h>
//#include <rbus/sync_processor_reg.h>
#include <rbus/vgip_reg.h>
#include <rbus/sub_vgip_reg.h>
#include <rbus/dma_vgip_reg.h>
#include <rbus/ppoverlay_reg.h>
//#include <rbus/scaler/rbusODReg.h>
#include <rbus/offms_reg.h>
#include <rbus/onms_reg.h>
//#include <rbus/rbusScaleupReg.h>
#include <rbus/scaleup_reg.h>
#include <rbus/tvsb2_reg.h>
#include <rbus/timer_reg.h>

//TVScaler Header file
#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/scaler/source.h>
#include <tvscalercontrol/scalerdrv/mode.h>
#include <tvscalercontrol/scalerdrv/zoom_smoothtoggle_lib.h>
#include <tvscalercontrol/scalerdrv/auto.h>
#include <tvscalercontrol/scalerdrv/syncproc.h>
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <tvscalercontrol/scalerdrv/scalerip.h>
#include <tvscalercontrol/scalerdrv/framesync.h>
#include <tvscalercontrol/scalerdrv/scaler_hdr_ctrl.h>
#include <tvscalercontrol/vip/nr.h>
#include <tvscalercontrol/vip/di_ma.h>
#include <tvscalercontrol/vip/scalerColor_tv006.h>
#include <tvscalercontrol/hdmirx/hdmifun.h>
#include <tvscalercontrol/pcbsource/pcbSource.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <tvscalercontrol/scalerdrv/scaler_i2rnd.h>
#ifdef ENABLE_DRIVER_I3DDMA
#include <tvscalercontrol/i3ddma/i3ddma.h>
#include <tvscalercontrol/i3ddma/i3ddma_drv.h>
#endif
#include <tvscalercontrol/scaler/vipinclude.h>
#include <scaler_vbedev.h>
#include <scaler_vscdev.h>
#include <scaler_vpqmemcdev.h>
#include <tvscalercontrol/hdmirx/hdmi_vfe.h>
#include <vo/rtk_vo.h>

#include <tvscalercontrol/scaler/scalercolorlib.h>

#include <tvscalercontrol/vdc/video.h>
// #include <rbus/ptol_reg.h>
#include <rtd_log/rtd_module_log.h>

#include <tvscalercontrol/vdc/video.h>

#if IS_ENABLED(CONFIG_RTK_KDRV_QOS_CONTROL)
#include <rtk_kdriver/rtk_qos_export.h>
#endif

// for register dump
#include <tvscalercontrol/io/ioregdrv.h>
#undef rtd_outl
#define rtd_outl(x, y)     								IoReg_Write32(x,y)
#undef rtd_inl
#define rtd_inl(x)     									IoReg_Read32(x)
#undef rtd_maskl
#define rtd_maskl(x, y, z)     							IoReg_Mask32(x,y,z)
#undef rtd_setbits
#define rtd_setbits(offset, Mask) rtd_outl(offset, (rtd_inl(offset) | Mask))
#undef rtd_clearbits
#define rtd_clearbits(offset, Mask) rtd_outl(offset, ((rtd_inl(offset) & ~(Mask))))

extern unsigned int current_hdmi_onms_watchdog_setting;
extern unsigned char current_hdmi_onms_is_i3ddma;
extern void scalerVIP_HDRCID_Reset(void);

#define GET_USER_INPUT_SRC() (Scaler_GetUserInputSrc(SLR_MAIN_DISPLAY))


#ifndef BUILD_QUICK_SHOW
static struct semaphore Offline_Semaphore;
#endif

unsigned char Scaler_Pre_Go_4K120_Mode = HDMI_NON_4K120;//save scaler last time run hfr mode or not
unsigned char Scaler_Pre_Go_4k60_VRR_TWO_PATH = FALSE;//save scaler last time run get_hdmi_vrr_4k60_mode or not
unsigned char Scaler_Pre_Go_Sub_NNSR_TWO_PATCH = FALSE;
//static unsigned short HDMIDiditalMeasureIVH;

//static ModeTableType *tINPUTMODE_HDMI_TABLE = NULL;
//static ModeTableType *tINPUTMODE_PRESET_TABLE = NULL;
//static ModeTableType *tINPUTMODE_YPBPR_TABLE = NULL;
static StructModeUserFIFODataType *tINPUTMODE_USERMODE_TABLE = NULL;
//static unsigned char *not_support_timing_array = NULL;
static unsigned char *user_fifo_mode = NULL;
//static ModeSkipTimingType *skip_timing_array = NULL;
static unsigned char hdmi_mode_flag = 0;

static void (*p_mode_customer_extra_condition)(unsigned char sync_type, unsigned char mode_index) = NULL;
static unsigned char (*p_mode_customer_judge_confuse_mode)(unsigned char mode_index, unsigned char sync_type, unsigned short usHighPulsePeriod) = NULL;
static unsigned char (*p_mode_customer_user_def_confuse_mode)(unsigned char mode_index) = NULL;
static unsigned char (*p_mode_customer_user_def_unsopport_mode)(unsigned char mode_index) = NULL;

static unsigned char (*p_hdmi_mode_customer_judge_confuse_mode)(unsigned char mode_index) = NULL;
static unsigned char (*p_mode_customer_user_def_modetable_size)(unsigned char mode) = NULL;
#ifndef BUILD_QUICK_SHOW
static unsigned char (*p_mode_customer_user_def_modetable_acceptive_size)(void) = NULL;
#endif
static unsigned char drvif_hdmi_mode_customer_judge_confuse_mode(UINT8 mode_index);

#define NO_DVS_INFO_MAX 6

static ModeTableType tINPUTMODE_PRESET_TABLE[] =
{
	//Mode 00~71 =>VGA
	//Mode 72~86 =>YPbPr
	//--------------------------------------------------
	// Preset Modes
	//--------------------------------------------------
	{   // Mode 0 : 640 x 350 x 70 Hz
		//OK
		_MODE_640x350_70HZ,
		0 | _SYNC_HP_VN,							// Polarity Flags,
		640, 350,									// InputWidth, InputHeight,
		315, 70000,									// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		800, 449,									// IHTotal, IVTotal,
		144, 61,									// IHStartPos, IVStartPos,
	},

	{   // Mode 1 : 640 x 350 x 85 Hz
		//OK
		_MODE_640x350_85HZ,
		0 | _SYNC_HP_VN,							// Polarity Flags,
		640, 350,									// InputWidth, InputHeight,
		378, 85000,									// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		832, 445,									// IHTotal, IVTotal,
		159,59,										// IHStartPos, IVStartPos,
	},

	{   // Mode 2 : 640 x 400 x 56 Hz
		//OK
		_MODE_640x400_56HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 400,													// InputWidth, InputHeight,
		248, 56400,													// IHFreq in kHz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in kHz, IVFreqTolerance in 0.001Hz,
		848, 440,													// IHTotal, IVTotal,
		142+2, 24,													// IHStartPos, IVStartPos,
	},

	{   // Mode 3 : 640 x 400 x 70 Hz
		//OK
	//confuse with 720x400x70
		_MODE_640x400_70HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 400,													// InputWidth, InputHeight,
		314, 70000,													// IHFreq in kHz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in kHz, IVFreqTolerance in 0.001Hz,
		800, 440,													// IHTotal, IVTotal,
		143, 36,													// IHStartPos, IVStartPos,
	},

	{   // Mode 4 : 640 x 400 x 85 Hz
		//OK
	//confuse with 720x400x85
		_MODE_640x400_85HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 400,													// InputWidth, InputHeight,
		380, 85000,													// IHFreq in kHz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in kHz, IVFreqTolerance in 0.001Hz,
		831, 445,													// IHTotal, IVTotal,
		160, 44,													// IHStartPos, IVStartPos,
	},

	{   // Mode 5 : 720 x 400 x 50 Hz
		//OK
		_MODE_720x400_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,				// Polarity Flags,
		720, 400,									// InputWidth, InputHeight,
		314, 50000,									// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		900, 629,									// IHTotal, IVTotal,
		162, 126,									// IHStartPos, IVStartPos,
	},

	{   // Mode 6 : 720 x 400 x 70 Hz
		//OK
	//confuse with 640x400x70
		_MODE_720x400_70HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP | _SYNC_HN_VN,		// Polarity Flags,
		720, 400,											// InputWidth, InputHeight,
		315, 70000,											// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,					// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		901, 449,											// IHTotal, IVTotal,
		152, 36,											// IHStartPos, IVStartPos,
	},

	{   // Mode 7 : 720 x 400 x 85 Hz
		//OK
	//confuse with 640x400x85
		_MODE_720x400_85HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 400,													// InputWidth, InputHeight,
		380, 85000,													// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE + 5, _VFREQ_TOLERANCE + 500,					// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		935, 446,													// IHTotal, IVTotal,
		179, 44,													// IHStartPos, IVStartPos,
	},

	{   // Mode 8 : 720 x 480 x 60 Hz
		//OK
		_MODE_720x480_60HZ,
		0 | _SYNC_HN_VN,							// Polarity Flags,
		1440, 480,									// InputWidth, InputHeight,
		315, 60000,									// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1716, 525,									// IHTotal, IVTotal,
		244, 36,									// IHStartPos, IVStartPos,
	},

	{   // Mode 9 : 640 x 480 x 50 Hz
		//OK
		_MODE_640x480_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,				// Polarity Flags,
		640, 480,									// InputWidth, InputHeight,
		246, 49600,									// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		800, 497,									// IHTotal, IVTotal,
		144, 13,									// IHStartPos, IVStartPos,
	},

	{   // Mode 10 : 640 x 480 x 60 Hz
		//OK
		_MODE_640x480_60HZ,
		0 | _SYNC_HN_VN,							// Polarity Flags,
		640, 480,									// InputWidth, InputHeight,
		315, 60000,									// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		800, 525,									// IHTotal, IVTotal,
		144, 35,									// IHStartPos, IVStartPos,
	},

	{   // Mode 11 : 640 x 480 x 67 Hz
		//OK
		_MODE_640x480_67HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 480,													// InputWidth, InputHeight,
		350, 66600,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		864, 525,													// HTotal, VTotal,
		159,38,														// HStartPos, VStartPos,
	},

	{   // Mode 12 : 640 x 480 x 70Hz
		//OK
		_MODE_640x480_70HZ,
		0 | _SYNC_HN_VN ,							// Polarity Flags,
		640, 480,									// InputWidth, InputHeight,
		360, 70000,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		840, 515,									// HTotal, VTotal,
		167,27,										// HStartPos, VStartPos,
	},

	{   // Mode 13 : 640 x 480 x 72 Hz
		//OK
		_MODE_640x480_72HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 480,													// InputWidth, InputHeight,
		378, 72800,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		832, 520,													// HTotal, VTotal,
		167,   27,													// HStartPos, VStartPos,
	},

	{   // Mode 14 : 640 x 480 x 75 Hz
		//OK
		_MODE_640x480_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 480,													// InputWidth, InputHeight,
		375, 75000,													// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		840, 500,													// IHTotal, IVTotal,
		183,15,														// IHStartPos, IVStartPos,
	},

	{   // Mode 15 : 640 x 480 x 85 Hz
		//OK
		_MODE_640x480_85HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 480,													// InputWidth, InputHeight,
		433, 85000,													// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		832, 510,													// IHTotal, IVTotal,
		135, 24,													// IHStartPos, IVStartPos,
	},

	{   // Mode 16 : 800 x 600 x 50 Hz
		//OK
		_MODE_800x600_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP ,				// Polarity Flags,
		800, 600,									// InputWidth, InputHeight,
		309, 49900,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		992, 621,									// HTotal, VTotal,
		167,  17,									// HStartPos, VStartPos,
	},

	{   // Mode 17 : 800 x 600 x 56 Hz
		//OK
		_MODE_800x600_56HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		800, 600,													// InputWidth, InputHeight,
		351, 56200,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1024, 625,													// HTotal, VTotal,
		199, 23,													// HStartPos, VStartPos,
	},

	{   // Mode 18 : 800 x 600 x 60 Hz
		//OK
		_MODE_800x600_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		800, 600,													// InputWidth, InputHeight,
		378, 60300,													// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1056, 628,													// IHTotal, IVTotal,
		215,26,														// IHStartPos, IVStartPos,
	},

	{   // Mode 19 : 800 x 600 x 72 Hz
		//OK
		_MODE_800x600_72HZ,
		0 | _SYNC_HP_VP,											// Polarity Flags,
		800, 600,													// InputWidth, InputHeight,
		480, 72000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1040, 666,													// HTotal, VTotal,
		183,28,														// HStartPos, VStartPos,
	},

	{   // Mode 20 : 800 x 600 x 75 Hz
		//OK
		_MODE_800x600_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		800, 600,													// InputWidth, InputHeight,
		468, 75000,													// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1056, 625,													// IHTotal, IVTotal,
		239,23,														// IHStartPos, IVStartPos,
	},

	{   // Mode 21 : 800 x 600 x 85 Hz
		//OK
		_MODE_800x600_85HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		800, 600,													// InputWidth, InputHeight,
		536, 85000,													// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1048, 631,													// IHTotal, IVTotal,
		215, 29,													// IHStartPos, IVStartPos,
	},

	{   // Mode 22 : 832 x 624 x 75 Hz
		//OK
		_MODE_832x624_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN ,				// Polarity Flags,
		832, 624,									// InputWidth, InputHeight,
		497, 74500,									// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1152, 667,									// IHTotal, IVTotal,
		287, 38,									// IHStartPos, IVStartPos,
	},

	{   // Mode 23 : 1024 x 768 x 50 Hz
		//OK
		_MODE_1024x768_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,				// Polarity Flags,
		1024, 768,									// InputWidth, InputHeight,
		396, 49900,									// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1312,793,									// IHTotal, IVTotal,
		247,21,										// IHStartPos, IVStartPos,
	},

	{   // Mode 24 : 1024 x 768 x 60 Hz
		//OK
		_MODE_1024x768_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN,			// Polarity Flags,
		1024, 768,								// InputWidth, InputHeight,
		483, 60000,								// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE-6, _VFREQ_TOLERANCE,	// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1344,806,								// IHTotal, IVTotal,
		295,28,									// IHStartPos, IVStartPos,
	},

	{   // Mode 25 : 1024 x 768 x 66 Hz
		//OK
		_MODE_1024x768_66HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN,			// Polarity Flags,
		1024, 768,								// InputWidth, InputHeight,
		539, 66100,								// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,		// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1328,816,								// IHTotal, IVTotal,
		287,35,									// IHStartPos, IVStartPos,
	},

	{   // Mode 26 : 1024 x 768 x 70 Hz
		//OK
		_MODE_1024x768_70HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN,			// Polarity Flags,
		1024, 768,								// InputWidth, InputHeight,
		564, 70000,								// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,		// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1328, 806,								// HTotal, VTotal,
		279,28,									// HStartPos, VStartPos,
	},

	{   // Mode 27 : 1024 x 768 x 75 Hz
		//OK
		_MODE_1024x768_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HP_VP,	// Polarity Flags,
		1024, 768,										// InputWidth, InputHeight,
		600, 75000,										// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,				// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1312, 800,										// IHTotal, IVTotal,
		271,30,											// IHStartPos, IVStartPos,
	},

	{   // Mode 28 : 1024 x 768 x 85 Hz
		//OK
		_MODE_1024x768_85HZ,
		0 | _SYNC_HP_VP,											// Polarity Flags,
		1024, 768,													// InputWidth, InputHeight,
		687, 85000,													// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1376, 808,													// IHTotal, IVTotal,
		304, 39,													// IHStartPos, IVStartPos,
	},

	{   // Mode 29 : 1152 x 864 x 60 Hz
		_MODE_1152x864_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1152, 864,													// InputWidth, InputHeight,
		537, 60000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1520,895,													// HTotal, VTotal,
		304,30,														// HStartPos, VStartPos,
	},

	{	// Mode 30 : 1152 x 864 x 75 Hz
		_MODE_1152x864_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1152, 864,													// InputWidth, InputHeight,
		675, 75000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE, 						// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1600,900,													// HTotal, VTotal,
		384,35,														// HStartPos, VStartPos,
	},

	{   // Mode 34 : 1280 x 720 x 50 Hz
		//OK
		_MODE_1280x720_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,			// Polarity Flags,
		1280, 720,								// InputWidth, InputHeight,
		370, 49800,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1632, 744,								// HTotal, VTotal,
		303,20,									// HStartPos, VStartPos,
	},

	{   // Mode 35 : 1280 x 720 x 60 Hz
		//OK
		_MODE_1280x720_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		447, 60000,														// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1650, 746,													// HTotal, VTotal,
		330,24,														// HStartPos, VStartPos,
	},

	{   // Mode 36 : 1280 x 720 x 85 Hz
		//OK
		_MODE_1280x720_85HZ,
		0 | _SYNC_HP_VP ,						// Polarity Flags,
		1280, 720,								// InputWidth, InputHeight,
		643, 84800,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1712, 759,								// HTotal, VTotal,
		351, 35,									// HStartPos, VStartPos,
	},

	{   // Mode 37 : 1280 x 768 x 50 Hz
//need check confuse with 1024x768x50, 3532
		_MODE_1280x768_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,			// Polarity Flags,
		1280, 768,								// InputWidth, InputHeight,
		395, 49900,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1648, 793,								// HTotal, VTotal,
		312-4,24,									// HStartPos, VStartPos,
	},

	{   // Mode 38 : 1280 x 768 x 60 Hz
		//OK
		_MODE_1280x768_60HZ,
		0 | _SYNC_HN_VP|_SYNC_HN_VN|_SYNC_HP_VP|_SYNC_HP_VN,	// Polarity Flags,
		1280, 768,												// InputWidth, InputHeight,
		477, 60000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,					// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1664, 799,												// HTotal, VTotal,
		319, 26,													// HStartPos, VStartPos,
	},
	{   // Mode 39 : 1280 x 768 x 60 Hz_RB
		//OK
		_MODE_1280x768_60HZ_RB,
		0 | _SYNC_HN_VP|_SYNC_HN_VN|_SYNC_HP_VP|_SYNC_HP_VN,	// Polarity Flags,
		1280, 768,												// InputWidth, InputHeight,
		474, 60000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,					// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1440, 790,												// HTotal, VTotal,
		112, 19,													// HStartPos, VStartPos,
	},
	{   // Mode 40 : 1280 x 768 x 75 Hz
		//OK
		_MODE_1280x768_75HZ,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 768,								// InputWidth, InputHeight,
		601, 75000,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1696, 801,								// HTotal, VTotal,
		335,33,									// HStartPos, VStartPos,
	},

	{   // Mode 41 : 1280 x 768 x 85 Hz
		//OK
		_MODE_1280x768_85HZ,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 768,								// InputWidth, InputHeight,
		686, 84800,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1711, 809,								// HTotal, VTotal,
		351,37,									// HStartPos, VStartPos,
	},

	{   // Mode 42 : 1280 x 800 x 60 Hz
		_MODE_1280x800_60HZ,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 800,								// InputWidth, InputHeight,
		497, 59800,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1680, 831,								// HTotal, VTotal,
		328, 28,									// HStartPos, VStartPos,
	},

	{   // Mode 43 : 1280 x 800 x 60 Hz (Reduce blanking)
		_MODE_1280x800_60HZ_RB,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 800,								// InputWidth, InputHeight,
		494, 60100,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1440, 823,								// HTotal, VTotal,
		112, 18,									// HStartPos, VStartPos,
	},
//#ifdef IS_MACARTHUR
	{   // Mode 44 : 1280 x 800 x 75 Hz
		_MODE_1280x800_75HZ,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 800,								// InputWidth, InputHeight,
		628, 74900,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1696, 838,								// HTotal, VTotal,
		336, 34,									// HStartPos, VStartPos,
	},
//#endif
	{   // Mode 45 : 1280 x 960 x 50 Hz
		//OK
		_MODE_1280x960_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,			// Polarity Flags,
		1280, 960,								// InputWidth, InputHeight,
		494, 49800,									// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1680, 991,								// IHTotal, IVTotal,
		327,27,									// IHStartPos, IVStartPos,
	},

	{   // Mode 46 : 1280 x 960 x 60 Hz
		//OK
		_MODE_1280x960_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 960,													// InputWidth, InputHeight,
		600, 60000,														// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1800, 1000,													// IHTotal, IVTotal,
		423,38,														// IHStartPos, IVStartPos,
	},

	{   // Mode 47 : 1280 x 960 x 75 Hz
		_MODE_1280x960_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 960,													// InputWidth, InputHeight,
		750, 75000,														// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1728, 1005,													// HTotal, VTotal,
		360,42,														// HStartPos, VStartPos,
	},

	{   // Mode 48 : 1280 x 960 x 85 Hz
		//OK
		_MODE_1280x960_85HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,			// Polarity Flags,
		1280, 960,								// InputWidth, InputHeight,
		857, 84800,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1728, 1011,								// HTotal, VTotal,
		358,47,									// HStartPos, VStartPos,
	},

	{   // Mode 49 : 1280 x 1024 x 50 Hz
		//OK
		_MODE_1280x1024_50HZ,
		0 | _SYNC_HP_VP ,						// Polarity Flags,
		1280, 1024,								// InputWidth, InputHeight,
		526, 49800,									// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1680, 1057,								// IHTotal, IVTotal,
		327,29,									// IHStartPos, IVStartPos,
	},

	{   // Mode 50 : 1280 x 1024 x 60 Hz
		//OK
		_MODE_1280x1024_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 1024,													// InputWidth, InputHeight,
		640, 60000,														// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1688, 1066,													// IHTotal, IVTotal,
		360, 41,														// IHStartPos, IVStartPos,
	},

	{   // Mode 51 : 1280 x 1024 x 72 Hz
		_MODE_1280x1024_72HZ,
		0|_SYNC_HP_VP,							// Polarity Flags,
		1280, 1024,								// InputWidth, InputHeight,
		780, 72000,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1712, 1064,								// HTotal, VTotal,
		380,39,									// HStartPos, VStartPos,
	},

	{   // Mode 52 : 1280 x 1024 x 75 Hz
		//OK
		_MODE_1280x1024_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 1024,													// InputWidth, InputHeight,
		800, 75000,														// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1688, 1066,													// IHTotal, IVTotal,
		391,40,														// IHStartPos, IVStartPos,
	},

	{   // Mode 53 : 1280 x 1024 x 85 Hz
		//OK
		_MODE_1280x1024_85HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,				// Polarity Flags,
		1280, 1024,									// InputWidth, InputHeight,
		911, 85000,										// IHFreq in kHz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,		// IHFreqTolerance in kHz, IVFreqTolerance in 0.001Hz,
		1728, 1072,									// IHTotal, IVTotal,
		384, 47,										// IHStartPos, IVStartPos,
	},

	{   // Mode 54 : 1360 x 768 x 60HZ
		//OK
		_MODE_1360x768_60HZ,
		0 | _SYNC_HP_VP,								// Polarity Flags,
		1360, 768,									// InputWidth, InputHeight,
		475, 59800,										// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE ,		// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1792, 795,									// IHTotal, IVTotal,
		367, 23,										// HStartPos, VStartPos,
	},

//#if 1//VGA1366x768@60
	{   // Mode 55 : 1366 x 768 x 60HZ
		_MODE_1366x768_60HZ,
		0 | _SYNC_HP_VP | _SYNC_HP_VN| _SYNC_HN_VP | _SYNC_HN_VN,	// Polarity Flags,
		1366, 768,													// InputWidth, InputHeight,
		477, 59800,														// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1792, 798,													// HTotal, VTotal,
		356,27,//358, 22,													// HStartPos, VStartPos,
    	},
//#endif

	{   // Mode 56 : 1440 x 900 x 60 Hz
		//OK
		_MODE_1440x900_60HZ,
		0 |_SYNC_HP_VP ,							// Polarity Flags,
		1440, 900,								// InputWidth, InputHeight,
		559, 59800,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		1904, 934,								// HTotal, VTotal,
		383, 30,									// HStartPos, VStartPos,
	},

	{   // Mode 57 : 1440 x 900 x 60 Hz (Reduced Blanking)
		//OK
		_MODE_1440x900_60RHZ,
		0 | _SYNC_HP_VN,							// Polarity Flags,
		1440, 900,								// InputWidth, InputHeight,
		555, 59900,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		1600, 926,								// HTotal, VTotal,
		111,16,									// HStartPos, VStartPos,
	},

	{   // Mode 58 : 1440 x 900 x 75 Hz
		//OK
		_MODE_1440x900_75HZ,
		0 | _SYNC_HN_VP |_SYNC_HP_VP ,			// Polarity Flags,
		1440, 900,								// InputWidth, InputHeight,
		706, 75000,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		1936, 942,								// HTotal, VTotal,
		399, 38,									// HStartPos, VStartPos,
	},

	{   // Mode 59 : 1440 x 900 x 85 Hz
		//OK
		_MODE_1440x900_85HZ,
		0 | _SYNC_HN_VP |_SYNC_HP_VP ,			// Polarity Flags,
		1440, 900,								// InputWidth, InputHeight,
		808, 85300,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		1952, 948,								// HTotal, VTotal,
		408, 45,									// HStartPos, VStartPos,
	},

	{   // Mode : 1600 x 900 x 60 Hz (Reduced Blanking)
		_MODE_1600x900_60HZ_RB,
		0 | _SYNC_HP_VN,							// Polarity Flags,
		1600, 900,								// InputWidth, InputHeight,
		600, 60000,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		1800, 1000,								// HTotal, VTotal,
		176,99,									// HStartPos, VStartPos,
	},

	{   // Mode 61 : 1600 x 1200 x 50 Hz
//need check clock, 3521
		_MODE_1600x1200_50HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1600, 1200,													// InputWidth, InputHeight,
		617, 49900,														// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		2128, 1238,													// HTotal, VTotal,
		431,34,														// HStartPos, VStartPos,
	},

	{   // Mode 62 : 1600 x 1200 x 60 Hz
		//OK
		_MODE_1600x1200_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1600, 1200,													// InputWidth, InputHeight,
		750, 60000,														// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		2160, 1250,													// HTotal, VTotal,
		495,48,														// HStartPos, VStartPos,
	},

	{   // Mode 63 : 1600 x 1200 x 65 Hz
		//OK
		_MODE_1600x1200_65HZ,
		0 | _SYNC_HP_VP,							// Polarity Flags,
		1600, 1200,								// InputWidth, InputHeight,
		812, 65000,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		2160, 1250,								// HTotal, VTotal,
		494,48,									// HStartPos, VStartPos,
	},

	{   // Mode 64 : 1600 x 1200 x 70 Hz
		//OK
		_MODE_1600x1200_70HZ,
		0 | _SYNC_HP_VP,							// Polarity Flags,
		1600, 1200,								// InputWidth, InputHeight,
		879, 70400,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		2160, 1250,								// HTotal, VTotal,
		496,49,									// HStartPos, VStartPos,
	},

	{   // Mode 65 : 1680 x 1050 x 50 Hz
		_MODE_1680x1050_50HZ,
		0 | _SYNC_HP_VP ,						// Polarity Flags,
		1680, 1050,								// InputWidth, InputHeight,
		541, 49900,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		2216-10, 1083,								// HTotal, VTotal,
		440-4, 31,									// HStartPos, VStartPos,
	},

	{   // Mode 66 : 1680 x 1050 x 60 Hz
//confuse 1400x1050x60, 3344
		_MODE_1680x1050_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1680, 1050,										// InputWidth, InputHeight,
		653, 60000,											// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		2238, 1089,										// HTotal, VTotal,
		456, 36,											// HStartPos, VStartPos,
	},

	{   // Mode 67 : 1680 x 1050 x 60 Hz (Reduced Blanking)
//confuse 1400x1050x60, 3343
		_MODE_1680x1050_60RHZ,
		0 | _SYNC_HP_VN,							// Polarity Flags,
		1680, 1050,								// InputWidth, InputHeight,
		647, 60000,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		1840, 1080,								// HTotal, VTotal,
		112, 27,									// HStartPos, VStartPos,
	},

	{   // Mode 68 : 1680 x 1050 x 75 Hz
		//confuse 1400x1050x75, 3332
		_MODE_1680x1050_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1680, 1050,													// InputWidth, InputHeight,
		823, 75000,														// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		2272, 1099,													// HTotal, VTotal,
		472, 45,														// HStartPos, VStartPos,
	},

	{   // Mode 69 : 1680 x 1050 x 85 Hz
		//confuse 1400x1050x85, 3344
		_MODE_1680x1050_85HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1680, 1050,													// InputWidth, InputHeight,
		944, 85500,														// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		2288, 1105,													// HTotal, VTotal,
		480, 52,														// HStartPos, VStartPos,
	},

	{   // Mode 70 : 1400 x 1050 x 50 Hz
		//OK
		_MODE_1400x1050_50HZ,
		0 | _SYNC_HP_VP | _SYNC_HN_VP,			// Polarity Flags,
		1400, 1050,								// InputWidth, InputHeight,
		541, 49900,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		1848, 1083,								// HTotal, VTotal,
		367, 29,									// HStartPos, VStartPos,
	},

	{   // Mode 71 : 1400 x 1050 x 60 Hz (Reduced Blanking)
		//OK
		_MODE_1400x1050_60RHZ,
		0 | _SYNC_HP_VN,							// Polarity Flags,
		1400, 1050,								// InputWidth, InputHeight,
		647, 59900,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		1560, 1080,								// HTotal, VTotal,
		111,22,									// HStartPos, VStartPos,
	},

	{   // Mode 72 : 1400 x 1050 x 60 Hz
		//OK
		_MODE_1400x1050_60HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,			// Polarity Flags,
		1400, 1050,								// InputWidth, InputHeight,
		653, 60000,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		1864, 1089,								// HTotal, VTotal,
		376, 36,									// HStartPos, VStartPos,
	},

	{   // Mode 73 : 1400 x 1050 x 75 Hz
		_MODE_1400x1050_75HZ,
		0 | _SYNC_HP_VP,							// Polarity Flags,
		1400, 1050,								// InputWidth, InputHeight,
		822, 74800,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		1896, 1099,								// HTotal, VTotal,
		391, 45,									// HStartPos, VStartPos,
	},
	{   // Mode 74 : 1920 x 1080 x 50 Hz
		//OK
		_MODE_1920x1080_50HZ,
		0 | _SYNC_HP_VN | _SYNC_HP_VP,			// Polarity Flags,
		1920, 1080,								// InputWidth, InputHeight,
		556, 49900,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		2544, 1114,								// HTotal, VTotal,
		510, 30,									// HStartPos, VStartPos,
	},
#if 0 //IC limit, unsupport clk > 162MHz
	{   // Mode 75 : 1920 x 1080 x 60 @173.1Mhz
		_MODE_1920x1080_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,        // Polarity Flags,
		1920, 1080,                                                                 // InputWidth, InputHeight,
		675, 60000,                                                                     // HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                   // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		2576, 1120,                                                                 // HTotal, VTotal,
		533,36,                                                                       // HStartPos, VStartPos,
    },
#endif
    {   // Mode 76 : 1920 x 1080 x 60	@138.5Mhz XBOX360
	    	_MODE_1920x1080_60HZ_138,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,        // Polarity Flags,
		1920, 1080,                                                            // InputWidth, InputHeight,
		666, 60000,                                                               // HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,             // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		2080, 1111,                                                           // HTotal, VTotal,
		112,25,                                                                 // HStartPos, VStartPos,
    },

    {   // Mode 77 : 1920 x 1080 x 60	@148.5Mhz(Reduced Blanking)
	    	_MODE_1920x1080_60HZ_148,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,        // Polarity Flags,
		1920, 1080,                                                            // InputWidth, InputHeight,
		675, 60000,                                                               // HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,             // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		2200, 1125,                                                           // HTotal, VTotal,
		192,41,                                                                 // HStartPos, VStartPos,
    },

	{   // Mode 78 : 1920 x 1080 x 75 Hz
		_MODE_1920x1080_75HZ,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1920, 1080,								// InputWidth, InputHeight,
		846, 74900,									// HFreq in kHz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
		2608, 1130,								// HTotal, VTotal,
		549, 46,									// HStartPos, VStartPos,
	},
	{   // Mode 81 : RGB 480p
		_MODE_VGA_480P,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 480,														// InputWidth, InputHeight,
		315, 60000,														// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
//?		800, 525,														// IHTotal, IVTotal,
		800-32-6, 525,														// IHTotal, IVTotal,
//?		146, 0x001F,													// HStartPos, VStartPos,
		107+1, 29,													// HStartPos, VStartPos,
	},

	{   // Mode 82 : RGB 576p
		_MODE_VGA_576P,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		1440, 576,													// InputWidth, InputHeight,
		312, 50000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1728, 625,														// HTotal, VTotal,
		264, 44,													// HStartPos, VStartPos,
	},

	{   // Mode 83: RGB 720px50Hz
		// MSPG-7800S T26 (1280x720P/50)
		_MODE_VGA_720P50,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		375, 50000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1980, 750,														// HTotal, VTotal,
		260, 25,													// HStartPos, VStartPos,
	},

	{   // Mode 84 : RGB 720px60Hz
		// MSPG-7800S T22 (1280x720P/60)
		_MODE_VGA_720P60,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		450, 60000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1650, 750,														// HTotal, VTotal,
		260, 25,													// HStartPos, VStartPos,
	},
	{   // Mode 87 :  VGA 1080p50
	_MODE_VGA_1080P50,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 1080,                                                         // InputWidth, InputHeight,
        562, 50000,                                                           // IHFreq in 100Hz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
        2640, 1125,                                                         // HTotal, VTotal,
        192, 41,                                                            // HStartPos, VStartPos,
    },

	{   // Mode 88 :  VGA 1080p60
    	_MODE_VGA_1080P60,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 1080,                                                         // InputWidth, InputHeight,
        675, 60000,                                                           // IHFreq in 100Hz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
        2200, 1118,                                                         // HTotal, VTotal,
        215, 30,                                                            // HStartPos, VStartPos,
    },

	{	// Mode : 848 x 480 x 60 Hz
		_MODE_848x480_60HZ,
		0 | _SYNC_HN_VN,							// Polarity Flags,
		848, 480,									// InputWidth, InputHeight,
		310, 60000,									// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE, 		// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1064, 525,									// IHTotal, IVTotal,
		224, 31,									// IHStartPos, IVStartPos,
	},

//[Code Sync][AlanLi][0980527][1]
//add MAC timing
	{   // Mode 89 : 1024 x 576 x 60 Hz //for MAC
	    	_MODE_1024x576_60HZ,
		 0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
		 1024, 576,                                                                                                  // InputWidth, InputHeight,
		 630, 60100,                                                                                                  // IHFreq in kHz, IVFreq in 0.001Hz,
		 _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                                // IHFreqTolerance in kHz, IVFreqTolerance in 0.001Hz,
		 1300, 599,                                                                                                // IHTotal, IVTotal,
		 195, 27,
   	},
//new interlace timing begin
	{   // Mode 85 : RGB 1080ix50Hz
		// MSPG-7800S T27 (1920x1080I/50)
		_MODE_VGA_1080I25,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		1920, 540,													// InputWidth, InputHeight,
		281, 50000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
 		0x0a50, /*0x022d*/ 0x232,													// HTotal, VTotal,
		192, 18,													// HStartPos, VStartPos,
	},

	{   // Mode 86 : RGB 1080ix60Hz
		// MSPG-7800S T23 (1920x1080I/60)
		_MODE_VGA_1080I30,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		1920, 540,													// InputWidth, InputHeight,
		337, 60000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		0x0898, /*0x022A*/ 0x0232,													// HTotal, VTotal,
		192, 40,													// HStartPos, VStartPos,
	},
#if 0//480i/576i confuse with 1440x480i/576i  need set 1030[8:7]=0x3 , 2410[18:16]=0x0 , 2424[28]=0

	{   // Mode 79 : RGB 480i
		_MODE_VGA_480I,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		//		0x02d0, 0x0f0,													// InputWidth, InputHeight,
		720, 240,													// InputWidth, InputHeight,
		157, 600,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		0x035a, 262,														// HTotal, VTotal,
		//?		0x007c, 0x0010,													// HStartPos, VStartPos,
		0x007e, 0x000c,													// HStartPos, VStartPos,
	},

	{   // Mode 80 : RGB 576i
		_MODE_VGA_576I,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		720, 289,													// InputWidth, InputHeight,
		156, 500,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		0x0360, 312,														// HTotal, VTotal,
	//?		0x008e-1, 0x0014-3,													// HStartPos, VStartPos,
		0x008d, 0x0011,													// HStartPos, VStartPos,
	},

#else
	{
		// MSPG-7800S T29 (1440x480I/60)
		_MODE_VGA_480I,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		1440, 240,													// InputWidth, InputHeight,
		157, 60000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1716, 262,														// HTotal, VTotal,
		238, 18,													// HStartPos, VStartPos,
	},

	{
		// MSPG-7800S T30 (1440x576I/50)
		_MODE_VGA_576I,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		1440, 288,													// InputWidth, InputHeight,
		156, 50000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1728, 312,														// HTotal, VTotal,
		264, 20,													// HStartPos, VStartPos,
	},
#endif

	{//1024x768I/43.48Hz
		_MODE_1024x768I_43HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		1024, 384,													// InputWidth, InputHeight,
		355, 87000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1264, 408,														// HTotal, VTotal,
		232, 24,													// HStartPos, VStartPos,
	},

	{	// Mode: 1280 x 1024 x 60 Hz
		_MODE_1280x1024_60HZ_MSPG,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 1024, 												// InputWidth, InputHeight,
		640, 60000,														// IHFreq in 100Hz, IVFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE, 					// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.001Hz,
		1708, 1056, 												// IHTotal, IVTotal,
		360, 41,														// IHStartPos, IVStartPos,
	},
	{   // Mode : 1280 x 768 x 50 Hz
		_MODE_1280x768_50HZ_MSPG,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,			// Polarity Flags,
		1280, 768,								// InputWidth, InputHeight,
		395, 49900,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1648, 791,								// HTotal, VTotal,
		312-4,24,									// HStartPos, VStartPos,
	},
	{   // Mode 38 : 1280 x 768 x 60 Hz
		//OK
		_MODE_1280x768_60HZ_MSPG,
		0 | _SYNC_HN_VP|_SYNC_HN_VN|_SYNC_HP_VP|_SYNC_HP_VN,	// Polarity Flags,
		1280, 768,												// InputWidth, InputHeight,
		477, 60000,													// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,					// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1680, 795,												// HTotal, VTotal,
		319, 26,													// HStartPos, VStartPos,
	},

	{	// Mode  : 1280 x 768 x 75 Hz
		_MODE_1280x768_75HZ_MSPG,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 768,								// InputWidth, InputHeight,
		601, 75000,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE, // HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1612, 802,								// HTotal, VTotal,
		335,33, 								// HStartPos, VStartPos,
	},

	{   // Mode : 1280 x 768 x 85 Hz
		_MODE_1280x768_85HZ_MSPG,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 768,								// InputWidth, InputHeight,
		686, 84800,									// IHFreq in 100Hz, VFreq in 0.001Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
		1728, 807,								// HTotal, VTotal,
		351,37,									// HStartPos, VStartPos,
	},

};

static ModeTableType tINPUTMODE_HDMI_TABLE[] =
{
	// if we move YPbPr mode to another place,
	// remember to change the mapping of mode numbe in CModeSetup()
	// function, because 480i & 576i should enter IP, others should not.

    {
    // Mode 83 : 480i
        _MODE_480I,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        720, 242,                                                           // InputWidth, InputHeight,
        157, 60000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        858, 262,                                                           // HTotal, VTotal,
        70, 14,                                                             // HStartPos, VStartPos,
    },
    {   // Mode 84 : 576i
        _MODE_576I,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        720, 288,                                                           // InputWidth, InputHeight,
        156, 50000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        864, 312,                                                           // HTotal, VTotal,
        80, 20,                                                             // HStartPos, VStartPos,
    },
    {   // Mode 85 : 480p
        _MODE_480P,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        720, 482,                                                           // InputWidth, InputHeight,
        315, 60000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        858, 525,                                                           // HTotal, VTotal,
        70, 34,                                                             // HStartPos, VStartPos,
    },
    {   // Mode 86 : 576p
        _MODE_576P,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        720, 576,                                                           // InputWidth, InputHeight,
        312, 50000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        864, 624,                                                           // HTotal, VTotal,
        80, 42,                                                             // HStartPos, VStartPos,
    },
    {   // Mode 87 : 720p50
        _MODE_720P50,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1280, 720,                                                          // InputWidth, InputHeight,
        376, 50200,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        1980, 750,                                                          // HTotal, VTotal,
        246, 22,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 88 : 720p60
        _MODE_720P60,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1280, 720,                                                          // InputWidth, InputHeight,
        450, 60000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        1650, 750,                                                          // HTotal, VTotal,
        244,22,                                                             // HStartPos, VStartPos,
    },
    {   // Mode 89: 1080i25
        _MODE_1080I25,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 540,                                                          // InputWidth, InputHeight,
        281, 50000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        2640, 562,                                                          // HTotal, VTotal,
        186 - 4, 18,                                                        // HStartPos, VStartPos,
    },
    {   // Mode 90 : 1080i30
        _MODE_1080I30,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 540,                                                          // InputWidth, InputHeight,
        337, 60000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        2200, 562,                                                          // HTotal, VTotal,
        182, 16 + 2,                                                        // HStartPos, VStartPos,
    },
    {   // Mode 91 : 1080p50
        _MODE_1080P50,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 1080,                                                         // InputWidth, InputHeight,
        563, 50100,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        2640, 1125,                                                         // HTotal, VTotal,
        178, 38,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 92 : 1080p60
        _MODE_1080P60,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 1080,//1026+48+4+2+2,                                         // InputWidth, InputHeight,
        675, 60000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        2200, 1125,                                                         // HTotal, VTotal,
        180,38,                                                             // HStartPos, VStartPos,
    },
    // CSW+ 0971225 Add YPbPr timing 1080P/25HZ/24HZ/23.97HZ/29.97HZ/30HZ
    {   // Mode 93 : 1080p23 (23.976 Hz)
        _MODE_1080P23,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 1080,                                                         // InputWidth, InputHeight,
        0x010d, 23900,                                                      // IHFreq in 100Hz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
        0x0ABE, 0x0465,                                                     // HTotal, VTotal,
        182, 39,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 94 : 1080p24 (24 Hz)
        _MODE_1080P24,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 1080,                                                         // InputWidth, InputHeight,
        0x010e, 24000,                                                      // IHFreq in 100Hz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, 500,                                              // HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
        0x0ABE, 0x0465,                                                     // HTotal, VTotal,
        198, 43,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 95 : 1080p25 (25 Hz)
        _MODE_1080P25,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 1080,                                                         // InputWidth, InputHeight,
        0x0119, 25000,                                                      // IHFreq in 100Hz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, 500,                                              // HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
        0x0A50, 0x0465,                                                     // HTotal, VTotal,
        184, 39,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 96 : 1080p29 (29.976 Hz)
        _MODE_1080P29,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 1080,                                                         // InputWidth, InputHeight,
        0x0151, 29900,                                                      // IHFreq in 100Hz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
        0x0898, 0x0465,                                                     // HTotal, VTotal,
        182, 39,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 97 : 1080p30 (30 Hz)
        _MODE_1080P30,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 1080,                                                         // InputWidth, InputHeight,
        0x0151, 30000,                                                      // IHFreq in 100Hz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
        0x0898, 0x0465,                                                     // HTotal, VTotal,
        182, 39,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 98 : 1080pm50(960x1080)
        _MODE_1080PM50,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        0x03A4, 0x0418,                                                     // InputWidth, InputHeight,
        0x0232, 50000,                                                      // IHFreq in 100Hz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
        0x0528, 0x045e,                                                     // HTotal, VTotal,
#if _HDMI_VGIP_SMART_FIT
        0x000e, 0x0010,                                                     // HStartPos, VStartPos,
#else
        0x016E - 0x001, 0x0036,                                             // HStartPos, VStartPos,
#endif
    },
    {   // Mode 99 : 1080pm60(960x1080)
        _MODE_1080PM60,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        0x03A4, 0x0418,                                                     // InputWidth, InputHeight,
        675, 60000,                                                         // IHFreq in 100Hz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in 100Hz, VFreqTolerance in 0.001Hz,
        0x044c, 0x045e,                                                     // HTotal, VTotal,
#if _HDMI_VGIP_SMART_FIT
        0x000e, 0x0010,                                                     // HStartPos, VStartPos,
#else
        0x0af - 0x0026, 0x0036,                                             // HStartPos, VStartPos,
#endif
    },
    // Support 4k2k
    {   // Mode 102 : 480p(1440x480)
        _MODE_4k2kI30,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        3840,1080,                                                          // InputWidth, InputHeight,
        315, 60000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        4400, 1125,                                                         // HTotal, VTotal,
        381, 42,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 118 //cloud modify 20150828
        _MODE_4k2kP24,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        3840, 2160,                                                         // InputWidth, InputHeight,
        539, 24000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        5500, 2250,                                                         // HTotal, VTotal,
        381, 81,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 103 : 480p(1440x480)
        _MODE_4k2kP30,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        3840, 2160,                                                         // InputWidth, InputHeight,
        674, 300,                                                           // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        4400, 2250,                                                         // HTotal, VTotal,
        381, 81,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 119 //cloud modify 20150828
        _MODE_4k2kP25,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        3840, 2160,                                                         // InputWidth, InputHeight,
        562, 25000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        5280, 2250,                                                         // HTotal, VTotal,
        381, 81,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 120 //not modify
        _MODE_4k2kP48,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        0, 0,                                                               // InputWidth, InputHeight,
        562, 25000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        0, 0,                                                               // HTotal, VTotal,
        381, 81,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 121 //cloud modify 20150828
        _MODE_4k2kP50,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        3840, 2160,                                                         // InputWidth, InputHeight,
        562, 50000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        5280, 2250,                                                         // HTotal, VTotal,
        381, 81,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 116 : 4k2k (38400x2160)
        _MODE_4k2kP60,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        3840, 2160,                                                         // InputWidth, InputHeight,
        1349, 60000,                                                        // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        4400, 2250,                                                         // HTotal, VTotal,
        381, 81,                                                            // HStartPos, VStartPos,
    },
    {   // Mode 116 : 4k2k (38400x2160)(420)
        _MODE_4k2kP60,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        3840, 2160,                                                         // InputWidth, InputHeight,
        675, 60000,                                                         // HFreq in kHz, VFreq in 0.001Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in 0.001Hz,
        4400, 2250,                                                         // HTotal, VTotal,
        383, 81,                                                            // HStartPos, VStartPos,
    },
};

unsigned int get_result_with_hdmi_mask(unsigned int original_value)
{
	unsigned int check_hdmi_mask = drvif_Hdmi_CheckHdmiOnmsMask();
	unsigned int is_need_mask = 0;
	unsigned int final_result = original_value;
	is_need_mask = check_hdmi_mask & original_value; // calculate onms_en mask
	final_result = original_value - is_need_mask; // calculte final value for onms_wdg/int register

	if(original_value != final_result)
	{
		rtd_pr_hdmi_info("[online] Original onms_en:%08x, through mask:%08x, final onms_en value:%08x\n", original_value, is_need_mask, final_result);
	}
	return final_result;
}

unsigned char get_scaler_run_hfr_mode(void)
{
	return Scaler_Pre_Go_4K120_Mode;
}

void set_scaler_run_hfr_mode(unsigned char mode)
{//save last time run 4k120, we need reset setting at reset mode
	// Scaler_Pre_Go_4K120_Mode = mode;//mac9q does not need two path
}

unsigned char get_scaler_run_4k60_vrr_two_path(void)
{
	return Scaler_Pre_Go_4k60_VRR_TWO_PATH;
}

void set_scaler_run_4k60_vrr_two_path(unsigned char enable)
{//save last time run vrr 4k 60 m domain two path , we need reset m domain sub setting at reset  mode
	// Scaler_Pre_Go_4k60_VRR_TWO_PATH = enable;//mac9q does not need two path
}

unsigned char get_scaler_run_sub_nnsr_two_path(void)
{
	return Scaler_Pre_Go_Sub_NNSR_TWO_PATCH;
}

void set_scaler_run_sub_nnsr_two_path(unsigned char enable)
{
    // Scaler_Pre_Go_Sub_NNSR_TWO_PATCH = enable;//mac9q does not need two path
}



#ifndef BUILD_QUICK_SHOW
unsigned char drvif_mode_customer_identify_regTable(MODE_IDENTIFY_TYPE index, void *ptr)
{
	if (!ptr)
		return FALSE;

	switch(index)
	{
		case CUSTOMER_EXTRA_CONDITION:
			p_mode_customer_extra_condition = (void (*)(unsigned char, unsigned char))ptr;
		break;

		case JUDGE_CONFUSE_MODE:
			p_mode_customer_judge_confuse_mode = (unsigned char (*)(unsigned char, unsigned char, short unsigned int))ptr;
		break;

		case HDMI_JUDGE_CONFUSE_MODE:
			p_hdmi_mode_customer_judge_confuse_mode =(unsigned char (*)(unsigned char))ptr;
		break;
		case HDMI_JUDGE_UNSUPPORT_MODE:
			p_mode_customer_user_def_unsopport_mode=(unsigned char (*)(unsigned char))ptr;
			break;
		case USER_DEF_CONFUSE_MODE:
			p_mode_customer_user_def_confuse_mode = (unsigned char (*)(unsigned char))ptr;
		break;

		case USER_DEF_MODETABLE_SIZE:
			p_mode_customer_user_def_modetable_size = (unsigned char (*)(unsigned char))ptr;
		break;

		case USER_DEF_MODETABLE_ACCEPTIVE_SIZE:
			p_mode_customer_user_def_modetable_acceptive_size = (unsigned char (*)(void))ptr;
		break;
		case MODE_VGA_USER_FIFO:
			user_fifo_mode = (unsigned char *) ptr;
			break;
		case USER_DEF_MODETABLE_ACCEPTIVE:
			tINPUTMODE_USERMODE_TABLE =  (StructModeUserFIFODataType*)ptr;

		default:
			break;
	}
	return TRUE;
}
#endif

unsigned char get_hdmi_idntify_mode(void)
{
	return hdmi_mode_flag;
}

void set_hdmi_identify_mode(unsigned char mode) // 0 dvi  1 hdmi
{
	hdmi_mode_flag = mode;
}

unsigned char hdmi_mode_identifymode(unsigned char mode_index)
{
 	unsigned int ihFreq = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IHFREQ);
 	unsigned int ivFreq = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVFREQ_1000);
	unsigned int preset_table_size = sizeof(tINPUTMODE_PRESET_TABLE)/sizeof(ModeTableType);
	unsigned int hdmi_table_size = sizeof(tINPUTMODE_HDMI_TABLE)/sizeof(ModeTableType);

	if (get_hdmi_idntify_mode())
	{
		if (mode_index >= hdmi_table_size) {
			return FALSE;
		}
		if(ABS(ihFreq , tINPUTMODE_HDMI_TABLE[mode_index].IHFreq) > tINPUTMODE_HDMI_TABLE[mode_index].IHFreqTolerance)
			return FALSE;
		if(ABS(ivFreq , tINPUTMODE_HDMI_TABLE[mode_index].IVFreq_1000) > tINPUTMODE_HDMI_TABLE[mode_index].IVFreqTolerance_1000)
			return FALSE;


        return drvif_hdmi_mode_customer_judge_confuse_mode(mode_index);
	}
	else  //dvi mode
	{
		if (mode_index >= preset_table_size) {
			return FALSE;
		}
		if(ABS(ihFreq , tINPUTMODE_PRESET_TABLE[mode_index].IHFreq) > tINPUTMODE_PRESET_TABLE[mode_index].IHFreqTolerance)
			return FALSE;
		if(ABS(ivFreq , tINPUTMODE_PRESET_TABLE[mode_index].IVFreq_1000) > tINPUTMODE_PRESET_TABLE[mode_index].IVFreqTolerance_1000)
			return FALSE;
	}

	return drvif_hdmi_mode_customer_judge_confuse_mode(mode_index);

 }

unsigned int Hdmi_Search_Mode(unsigned int *p_mid)
{
		unsigned char search_cnt=0;
		UINT16 dvi_end_index = 0 , hdmi_end_index=0;
		//UINT32 enter_confuse_condition = FALSE;
		UINT16 mode_id;
		//unsigned char result = 0;

		dvi_end_index = sizeof(tINPUTMODE_PRESET_TABLE)/sizeof(ModeTableType) -1; //p_mode_customer_user_def_modetable_size(0) - 1 ;
		hdmi_end_index = sizeof(tINPUTMODE_HDMI_TABLE)/sizeof(ModeTableType) -1; //p_mode_customer_user_def_modetable_size(1) - 1 ;

//		if(drvif_IsHDMI() == MODE_HDMI)
		{
			set_hdmi_identify_mode(MODE_HDMI);
			for(search_cnt = 0; search_cnt <= hdmi_end_index; search_cnt++){		// search HDMI table first
				if (hdmi_mode_identifymode(search_cnt)) {
					mode_id = tINPUTMODE_HDMI_TABLE[search_cnt].ModeID;
					if (mode_id == _MODE_1080P23) //23hz is the same 24hz
						continue;

					//USER:LewisLee DATE:2017/07/20
					//it will update at Scaler_SET_VSCDispinfo_WithVFEHMDI()
//					Scaler_DispSetInputInfo(SLR_INPUT_MODE_CURR,mode_id);
					rtd_pr_hdmi_debug("\n hdmi mode_searchmode OK.%d, mode index=%d mode id=%d\n", __LINE__, search_cnt,mode_id);
					*p_mid = mode_id;
					return search_cnt;
				}
			}
		}

		set_hdmi_identify_mode(MODE_DVI);
		for (search_cnt = 0; search_cnt <= dvi_end_index; search_cnt++) {	// search DVI table
		    if (hdmi_mode_identifymode(search_cnt)) {
			mode_id = tINPUTMODE_PRESET_TABLE[search_cnt].ModeID;
                        //rtd_pr_hdmi_info("=========Hdmi_Search_Mode=====DVI=====mode_id=%d\n",mode_id);

			//USER:LewisLee DATE:2017/07/20
			//it will update at Scaler_SET_VSCDispinfo_WithVFEHMDI()
//			Scaler_DispSetInputInfo(SLR_INPUT_MODE_CURR,mode_id);
			rtd_pr_hdmi_debug("\n dvi mode_searchmode OK.%d,mode index=%d mode id=%d\n", __LINE__, search_cnt,mode_id);
			*p_mid = mode_id;
			return search_cnt;
			}
		}

		//USER:LewisLee DATE:2017/07/20
		//it will update at Scaler_SET_VSCDispinfo_WithVFEHMDI()
//		Scaler_DispSetInputInfo(SLR_INPUT_MODE_CURR, _MODE_NEW);
		*p_mid = _MODE_NEW;
		rtd_pr_hdmi_debug("\n hdmi Hdmi_Search_Mode NOT SUPPORT \n");
		return _MODE_NOSUPPORT;
}

#ifndef BUILD_QUICK_SHOW
unsigned int Hdmi_get_startpos(unsigned int tbl_index, unsigned int *hstart, unsigned int *vstart)
{
    if ( _MODE_NOSUPPORT==tbl_index )
    {
        *hstart = 0;
        *vstart = 0;
        return 0;
    }

    if ( get_hdmi_idntify_mode() )
    {
        *hstart = tINPUTMODE_HDMI_TABLE[tbl_index].IHStartPos;
        *vstart = tINPUTMODE_HDMI_TABLE[tbl_index].IVStartPos;
    }
    else
    {
        *hstart = tINPUTMODE_PRESET_TABLE[tbl_index].IHStartPos;
        *vstart = tINPUTMODE_PRESET_TABLE[tbl_index].IVStartPos;
    }
    return 1;
}

void drvif_mode_clear_offms_status(void)
{
	offms_sp_ms3stus_RBUS offms_sp_ms3stus_reg;

	offms_sp_ms3stus_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Stus_reg);
	offms_sp_ms3stus_reg.off_msdone = 1;		//bit31
	offms_sp_ms3stus_reg.off_vs_per_to_long = 1;//bit10
	offms_sp_ms3stus_reg.off_vs_per_to = 1;		//bit7
	offms_sp_ms3stus_reg.off_vs_high_to = 1;	//bit6
	offms_sp_ms3stus_reg.off_vs_per_of = 1;		//bit5
	offms_sp_ms3stus_reg.off_vs_pol_chg = 1;	//bit3
	offms_sp_ms3stus_reg.off_hs_per_of = 1;		//bit2
	offms_sp_ms3stus_reg.off_hs_pol_chg = 1;	//bit0
	IoReg_Write32(OFFMS_SP_MS3Stus_reg, offms_sp_ms3stus_reg.regValue);
}

/**
 * This function is used to start off-line measure.
 *
 * @param [input] : None
 * @param st263xInfo->Hsync: [output] This para. record the measure result of Hsync period
 * @param st263xInfo->Vsync: [output] This para. record the measure result of Vsync lines
 * @param st263xInfo->YPbPrSyncType [output] This para. indicate that when mode change or nosignal occur, sync type will set to NO_SIGNAL
 *
 *
 * @return : None
 * @Note : This function should be called only after progress the YPbPr_OffLineMeasure and get the return TRUE.
 *
 */
 unsigned char drvif_mode_offlinemeasure(unsigned char source, unsigned char mode, StructDisplayInfo* p_dispinfo, ModeInformationType* p_timinginfo)
 {
//	unsigned char VsTimeOutFlg = 0;
	unsigned char timeoutcnt = 10;   // Tracking timeout 150ms
	unsigned int temp;
	unsigned short ihTotal;
	unsigned short ivHeight;
	unsigned short ihWidth;
	offms_sp_ms3ctrl_RBUS offms_sp_ms3ctrl_reg;
	offms_sp_ms3stus_RBUS offms_sp_ms3stus_reg;
	offms_sp_ms3rst0_RBUS offms_sp_ms3rst0_reg;
	offms_sp_ms3rst1_RBUS offms_sp_ms3rst1_reg;
	offms_sp_ms3rst2_RBUS offms_sp_ms3rst2_reg;
#if TEST_PATTERN_GEN || TEST_UZD_BY_IPG
	dds_mix_w2_RBUS dds_pll_dds_mix_w2_reg;
#endif

	//rtd_pr_offms_info("#####[%s(%d)] Get source=%x, mode = %x in off-line measure\n",__func__,__LINE__, source, mode);

	if (p_dispinfo == NULL || p_timinginfo == NULL) {
		rtd_pr_offms_info("#####[%s(%d)] fail, point is null\n",__func__,__LINE__);
		return _MODE_NOSIGNAL;
	}

	if (mode == ANALOG_MODE_MEASUREMENT) {
		p_timinginfo->IHCount = 0;
		p_timinginfo->IVCount = 0;
	}

#if TEST_PATTERN_GEN || TEST_UZD_BY_IPG
	dds_pll_dds_mix_w2_reg.regValue = IoReg_Read32(DDS_PLL_DDS_MIX_W2_reg);
	dds_pll_dds_mix_w2_reg.apll_free_run = 1;
	IoReg_Write32(DDS_PLL_DDS_MIX_W2_reg, dds_pll_dds_mix_w2_reg.regValue);
	//IoReg_Mask32(SYNCPROCESSOR_SP_MS3CTRL_VADDR , 0xfff0ffff,(0 & 0x0f) << 16); // pattern gen

	offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
	offms_sp_ms3ctrl_reg.off_meas_to_sel = _OFF_MEAS_TO_SEL;	//bit22	// 1 VS period measure time out selec 1:76ms 0:38ms
	offms_sp_ms3ctrl_reg.off_meas_source = 0;				//bit19~16  // pattern gen
	offms_sp_ms3ctrl_reg.off_blk_2frame_en = OFF_BLK_2FRAME_EN;
	IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);

#else
	//  set measure source
	offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
	offms_sp_ms3ctrl_reg.off_meas_to_sel = _OFF_MEAS_TO_SEL;	//bit22	// 1 VS period measure time out selec 1:76ms 0:38ms
	offms_sp_ms3ctrl_reg.off_meas_source = source;				//bit19~16 //VGA:4 YPP:5
	offms_sp_ms3ctrl_reg.off_blk_2frame_en = OFF_BLK_2FRAME_EN;

	if (_OFFLINE_MS_SRC_VDC == source)
		offms_sp_ms3ctrl_reg.off_clk_sel = 0; //VDC source use crystal clock to measure.

	IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);

#endif

	//frank@0807 clear offline measure error status
	drvif_mode_clear_offms_status();

	//set measure mode & start measure
	offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
	offms_sp_ms3ctrl_reg.off_sycms_mode = mode;
	offms_sp_ms3ctrl_reg.off_online_en = 1;
	if(CONFIG_OFFLINE_XTAL_FREQ == CONFIG_ONLINE_OFFLINE_XTAL_FREQ_27M)
		offms_sp_ms3ctrl_reg.off_clk_sel = 0;//27M
	else
		offms_sp_ms3ctrl_reg.off_clk_sel = 1;//24M
	offms_sp_ms3ctrl_reg.off_start_ms = _ENABLE;		//bit9	// enable offline measure
	IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);


#if 1//merlin6 remove off_popup_meas and off_continuous_popup
	do {
		usleep_range(40000,40000);//delay 40ms
		offms_sp_ms3stus_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Stus_reg);

		if(offms_sp_ms3stus_reg.off_msdone == 0){
			drvif_mode_clear_offms_status();
		}else{
			break; // off-line measure is ready
		}
	}while (--timeoutcnt);

	if (timeoutcnt == 0x00) {
		rtd_pr_offms_info("#####[%s(%d)] ERROR1\n",__func__,__LINE__);
		goto error_result;
	}
#else
	do {
		msleep(10); //Important delay for offms, rzhen@20190518
		offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
		if(0 == offms_sp_ms3ctrl_reg.off_start_ms) // polling start bit	if 0 => measure ok
		{
			offms_sp_ms3stus_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Stus_reg);

			if(_OFF_MEAS_TO_SEL == _OFF_MEAS_TO_SEL_38MS)
				VsTimeOutFlg = offms_sp_ms3stus_reg.off_vs_per_to;
			else// if(_OFF_MEAS_TO_SEL == _OFF_MEAS_TO_SEL_76MS)
				VsTimeOutFlg = offms_sp_ms3stus_reg.off_vs_per_to_long;

			if(VsTimeOutFlg)
			{
//				rtd_pr_offms_info("drvif_mode_offlinemeasure ERROR:%x\n", sp_ms3stus_reg.regValue);
				//have error status,restart measure
				offms_sp_ms3stus_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Stus_reg);
				offms_sp_ms3stus_reg.off_vs_per_to_long = 1;
//				sp_ms3stus_reg.off_vs_pol_out = 1;
//				sp_ms3stus_reg.off_hs_pol_out = 1;
				offms_sp_ms3stus_reg.off_vs_per_to = 1;
				offms_sp_ms3stus_reg.off_vs_high_to = 1;
				offms_sp_ms3stus_reg.off_vs_per_of = 1;
				/* MERLIN5-2207 remove */
				/* offms_sp_ms3stus_reg.off_vs_over_range = 1; */
				offms_sp_ms3stus_reg.off_vs_pol_chg = 1;
				offms_sp_ms3stus_reg.off_hs_per_of = 1;
				/* MERLIN5-2207 remove */
				/* offms_sp_ms3stus_reg.off_hs_over_range = 1; */
				offms_sp_ms3stus_reg.off_hs_pol_chg = 1;
				IoReg_Write32(OFFMS_SP_MS3Stus_reg, offms_sp_ms3stus_reg.regValue);

				offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
				offms_sp_ms3ctrl_reg.off_sycms_mode = mode;
				offms_sp_ms3ctrl_reg.off_online_en = 0;
				offms_sp_ms3ctrl_reg.off_start_ms = _ENABLE;
				IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);
			}
			else
			{
				break; // off-line measure is ready
			}
		}
	} while (--timeoutcnt);

    //rtd_pr_offms_info("##########line:%d timeout:%d\n",__LINE__,timeoutcnt);

	if (timeoutcnt == 0x00) {
		rtd_pr_offms_info("#####[%s(%d)] ERROR1\n",__func__,__LINE__);
		goto error_result;
	}

	//pop up result
	timeoutcnt = 20;  //fine tune delay

	// pop-up period measure result
	offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
	offms_sp_ms3ctrl_reg.off_popup_meas = 1;	//bit12
	IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);

	do {
		offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
		if(0 == offms_sp_ms3ctrl_reg.off_popup_meas) // polling status bit 12
			break;
		msleep(2); // fine tune delay

	 } while (--timeoutcnt);
	//rtd_pr_offms_info("##########line:%d timeout:%d\n",__LINE__,timeoutcnt);

	if (timeoutcnt == 0x00){
		rtd_pr_offms_info("#####[%s(%d)] OFFMS POP Err\n",__func__,__LINE__);
		goto error_result;
	}
#endif
	// get measure result
	if (mode == ANALOG_MODE_MEASUREMENT){
		rtd_pr_offms_debug("\nANALOG_MODE_MEASUREMENT\n");

		offms_sp_ms3rst0_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Rst0_reg);
		offms_sp_ms3rst1_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Rst1_reg);
#if TEST_PATTERN_GEN
		p_timinginfo->IHCount = 4400;
		p_timinginfo->IVCount = 2250;
#else

		p_timinginfo->IHCount = offms_sp_ms3rst1_reg.off_hs_period_out_h12b;
		p_timinginfo->IVCount = offms_sp_ms3rst0_reg.off_vs_period_out;
#endif

		p_dispinfo->IHCount_PRE = p_timinginfo->IHCount;
		p_dispinfo->IVCount_PRE = p_timinginfo->IVCount;

#if TEST_PATTERN_GEN
        p_timinginfo->IHSyncPulseCount = 5;
#else
		offms_sp_ms3rst2_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Rst2_reg);
		p_timinginfo->IHSyncPulseCount = offms_sp_ms3rst2_reg.off_hs_high_out;
#endif
		p_timinginfo->IVTotal = p_timinginfo->IVCount;
		p_dispinfo->IVSyncPulseCount = offms_sp_ms3rst2_reg.off_vs_high_out;

		if((p_timinginfo->IHCount == 0) || (p_timinginfo->IVCount == 0)){
			rtd_pr_offms_info("#####[%s(%d)] p_timinginfo->IHCount=0x%x, p_timinginfo->IVCount=0x%x\n",__func__,__LINE__,p_timinginfo->IHCount, p_timinginfo->IVCount);
			goto error_result;
		}

		temp = (unsigned int) CONFIG_OFFLINE_XTAL_FREQ / p_timinginfo->IHCount / 100; //calcluate H Frequency in 100Hz unit.
		p_timinginfo->IHFreq = temp;
		temp = ((unsigned int)(CONFIG_OFFLINE_XTAL_FREQ * 10) / ((unsigned int) (p_timinginfo->IHCount) * (p_timinginfo->IVCount))) * 100;//calcluate V Frequency in 1/1000Hz unit.
		p_timinginfo->IVFreq_1000 = temp;
		//rtd_pr_offms_info("#####[%s(%d)] HSYNCPULSECOUNT:%d\n",__func__,__LINE__, p_timinginfo->IHSyncPulseCount);
		//rtd_pr_offms_info("#####[%s(%d)] IHCount:%d,IVCount:%d\n",__func__,__LINE__, p_timinginfo->IHCount, p_timinginfo->IVCount);
		//rtd_pr_offms_info("#####[%s(%d)] IHFreq:%d,IVFreq_1000:%d\n",__func__,__LINE__, p_timinginfo->IHFreq, p_timinginfo->IVFreq_1000);
		/* reset h/v sync invert should be set in other place
		vgip_chn1_ctrl_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_CTRL_reg);
		vgip_chn1_ctrl_reg.ch1_vs_inv = 0;
		vgip_chn1_ctrl_reg.ch1_hs_inv = 0;
		IoReg_Write32(VGIP_VGIP_CHN1_CTRL_reg, vgip_chn1_ctrl_reg.regValue);
		*/

	}
	else{//DIGITAL MODE
//no use
		offms_sp_ms3rst0_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Rst0_reg);
		offms_sp_ms3rst1_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Rst1_reg);
		offms_sp_ms3rst2_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Rst2_reg);
		ihTotal = offms_sp_ms3rst1_reg.off_hs_period_out_h12b;;
		ivHeight = offms_sp_ms3rst0_reg.off_vs_period_out;
		ihWidth = offms_sp_ms3rst2_reg.off_hs_high_out;
		p_timinginfo->IHTotal = ihTotal;
		p_timinginfo->IVHeight = ivHeight;
		p_timinginfo->IHWidth = ihWidth;

		if((ihWidth == 0) || (ivHeight == 0)){//1221 kist
			rtd_pr_offms_info("\n(stModeInfo.IHWidth == %x) || (stModeInfo.IVHeight == %x)\n",ihWidth,ivHeight);
			goto error_result;
		}

		ihWidth++;//match resolution(cause measure result will lose one line)
		ivHeight++;//match resolution(cause measure result will lose one line)
		p_timinginfo->IHWidth = ihWidth;
		p_timinginfo->IVHeight = ivHeight;

		if (ihWidth%2){
			rtd_pr_offms_debug("IN_ERROR_IHWidth:%d\n",ihWidth);
			ihWidth++;
			p_timinginfo->IHWidth = ihWidth;
		}

		//HDMIDiditalMeasureIVH = ivHeight;
		Scaler_SetHDMIDiditalMeasureIVH(ivHeight);

		rtd_pr_offms_debug("\nIHTotal:%d\n",p_timinginfo->IHTotal);
		rtd_pr_offms_debug("IVHeight:%d\n",p_timinginfo->IVHeight);
		rtd_pr_offms_debug("IHWidth:%d\n",p_timinginfo->IHWidth);
	}

#if 0
	if(GET_SOURCE_FROM(srcinput_pt[p_dispinfo->input_src].src_type) == _SRC_FROM_TMDS){
		if(IoReg_Read32(OFFMS_SP_MS3Stus_reg) & _BIT7){
			rtd_pr_offms_debug("Error Flag:%x\n", IoReg_Read32(OFFMS_SP_MS3Stus_reg));
			goto error_result;
		}
	}
#endif

	offms_sp_ms3stus_reg.regValue =  IoReg_Read32(OFFMS_SP_MS3Stus_reg);
	p_dispinfo->IVPolarityIndicator = offms_sp_ms3stus_reg.off_vs_pol_out<<1;

	offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
	offms_sp_ms3ctrl_reg.off_clk_sel = 0;//27M//reset
	IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);

	return _MODE_SUCCESS;

error_result:

	rtd_pr_offms_info("#####[%s(%d)] fail, No Signal!!!\n",__func__,__LINE__);
	offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
	offms_sp_ms3ctrl_reg.off_clk_sel = 0;//27M//reset
	IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);
	return _MODE_NOSIGNAL;
 }
#endif

unsigned int drvif_mode_onms_int_setting(unsigned char channel, int src)
{
	unsigned int online_wd_int_setting = 0;

	if(src == _SRC_VO) {
		if(_CHANNEL1 == channel) {
			if(get_vsc_run_adaptive_stream(SLR_MAIN_DISPLAY)) {
				if(Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_FRAMESYNC)) {
					online_wd_int_setting = VO_CP_FS_ONLINE_ENABLE_MASK;
				}
				else {
					online_wd_int_setting = VO_CP_FRC_ONLINE_ENABLE_MASK;
				}
			} else {
				if(Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_FRAMESYNC)) {
					online_wd_int_setting = VO_FS_ONLINE_ENABLE_MASK;
				}
				else {
					online_wd_int_setting = VO_FRC_ONLINE_ENABLE_MASK;
				}
			}
		} else {
			online_wd_int_setting = VO_FRC_ONLINE_ENABLE_MASK;
		}
	}
	else if(src == _SRC_HDMI) {
		if(_CHANNEL1 == channel) {
			if(get_scaler_qms_mode_flag()) {//qms case similar with vrr
				online_wd_int_setting = HDMI_QMS_ONLINE_ENABLE_MASK;
			} else if(vbe_disp_get_VRR_timingMode_flag()) {
				online_wd_int_setting = HDMI_VRR_ONLINE_ENABLE_MASK;
			} else if(vbe_disp_get_freesync_mode_flag()) {
				online_wd_int_setting = HDMI_FREESYNC_ONLINE_ENABLE_MASK;
			} else {
				online_wd_int_setting = HDMI_NORMAL_ONLINE_ENABLE_MASK;
			}
		} else {
			online_wd_int_setting = HDMI_NORMAL_ONLINE_ENABLE_MASK;
		}
	}
	else if(src == _SRC_MINI_DP) {
		if(_CHANNEL1 == channel) {
			if(vbe_disp_get_VRR_timingMode_flag()) {
				online_wd_int_setting = DP_VRR_ONLINE_ENABLE_MASK;
			} else if(vbe_disp_get_freesync_mode_flag()) {
				online_wd_int_setting = DP_FREESYNC_ONLINE_ENABLE_MASK;
			} else {
				online_wd_int_setting = DP_NORMAL_ONLINE_ENABLE_MASK;
			}
		} else
			online_wd_int_setting = DP_NORMAL_ONLINE_ENABLE_MASK;
	}
	else if(src == _SRC_CVBS) {
		online_wd_int_setting = AVD_ONLINE_ENABLE_MASK;
	}

	rtd_pr_onms_info("###func %s online_wd_int_setting:0x%x###\r\n",__FUNCTION__, online_wd_int_setting);

	return online_wd_int_setting;
}

void drvif_mode_onms_ctrl_setting(unsigned char channel, int src)
{
	onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;
	// onms_onms1_vs_period_RBUS onms_onms1_vs_period_reg;
	// onms_onms1_hs_period_RBUS onms_onms1_hs_period_reg;
	onms_onms1_hsync_meas_ctrl_RBUS onms_onms1_hsync_meas_ctrl_reg;
	onms_onms1_hsync_meas_ctrl_2_RBUS onms_onms1_hsync_meas_ctrl_2_reg;
	onms_onms1_status_RBUS onms_onms1_status_reg;
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	onms_onms2_status_RBUS onms_onms2_status_reg;
	onms_onms2_hsync_meas_ctrl_RBUS onms_onms2_hsync_meas_ctrl_reg;
	onms_onms2_hsync_meas_ctrl_2_RBUS onms_onms2_hsync_meas_ctrl_2_reg;
	onms_onms2_ctrl_RBUS onms_onms2_ctrl_reg;
	// onms_onms2_vs_period_RBUS onms_onms2_vs_period_reg;
	// onms_onms2_hs_period_RBUS onms_onms2_hs_period_reg;
#endif
	onms_onms1_hsync_meas_result_RBUS onms_onms1_hsync_meas_result_reg;
	onms_onms2_hsync_meas_result_RBUS onms_onms2_hsync_meas_result_reg;

	int timeout = 10;
	unsigned char de_only_mode = 0;

	if((src == _SRC_HDMI) || (src == _SRC_MINI_DP) || (src == _SRC_VO))
		de_only_mode = 1;

	if(_CHANNEL1 == channel) {//main case new error status
		onms_onms1_ctrl_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_reg);
		if(onms_onms1_ctrl_reg.on1_start_ms) {
			if(!de_only_mode)
			{
				onms_onms1_ctrl_reg.on1_popup_meas = 1;
				IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);
				onms_onms1_ctrl_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_reg);
				while(onms_onms1_ctrl_reg.on1_popup_meas && onms_onms1_ctrl_reg.on1_start_ms && timeout)
				{
					timeout--;
					msleep(10);
					onms_onms1_ctrl_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_reg);
				}
				// onms_onms1_vs_period_reg.regValue = IoReg_Read32(ONMS_onms1_vs_period_reg);
				// onms_onms1_hs_period_reg.regValue = IoReg_Read32(ONMS_onms1_hs_period_reg);
			}


			//B802125c[30:24] = 8 (line diff)
			//B802125c[19:16] = 2 or 3 (line average)
			//B802125c[13:0] = hotal from b8021238[17:4]
			//B8021260[27:16] = source vtotal-10
			//B8021260[11:0] = 10 after Vsync line number, avoid clamp or macrovision
			onms_onms1_hsync_meas_ctrl_reg.regValue = IoReg_Read32(ONMS_onms1_hsync_meas_ctrl_reg);

				if (de_only_mode) {
					onms_onms1_demode_hs_period_RBUS onms_onms1_demode_hs_period_reg;
					onms_onms1_demode_hs_period_reg.regValue = IoReg_Read32(ONMS_Onms1_Demode_Hs_period_reg);
					onms_onms1_hsync_meas_ctrl_reg.on1_target_measure = onms_onms1_demode_hs_period_reg.on1_de_hs_period;
					onms_onms1_hsync_meas_ctrl_reg.on1_average_line = 3;
					onms_onms1_hsync_meas_ctrl_reg.on1_htotal_max_delta_new = 8;
				} else { // sync mode
					// onms_onms1_hsync_meas_ctrl_reg.on1_target_measure = onms_onms1_hs_period_reg.on1_htotal_h12b;
					onms_onms1_hsync_meas_ctrl_reg.on1_average_line = 3;
					onms_onms1_hsync_meas_ctrl_reg.on1_htotal_max_delta_new = 8;
				}
				IoReg_Write32(ONMS_onms1_hsync_meas_ctrl_reg, onms_onms1_hsync_meas_ctrl_reg.regValue);

				if(!de_only_mode)
				{
					onms_onms1_hsync_meas_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms1_hsync_meas_ctrl_2_reg);
					onms_onms1_hsync_meas_ctrl_2_reg.on1_vmeas_start = 16;
					// onms_onms1_hsync_meas_ctrl_2_reg.on1_vmeas_end = onms_onms1_vs_period_reg.on1_vtotal - 16;
					IoReg_Write32(ONMS_onms1_hsync_meas_ctrl_2_reg, onms_onms1_hsync_meas_ctrl_2_reg.regValue);
				}

				onms_onms1_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS_onms1_hsync_meas_result_reg);
				onms_onms1_hsync_meas_result_reg.on1_hmax_delta_total = 0;	//write clear
				IoReg_Write32(ONMS_onms1_hsync_meas_result_reg, onms_onms1_hsync_meas_result_reg.regValue);

				onms_onms1_status_reg.regValue = 0;
				onms_onms1_status_reg.on1_hs_meas_over_range = 1;//write clean bit 21
				IoReg_Write32(ONMS_onms1_status_reg, onms_onms1_status_reg.regValue);
		}
	}
#ifndef BUILD_QUICK_SHOW
    else {//sub case new error status
		onms_onms2_ctrl_reg.regValue = IoReg_Read32(ONMS_onms2_ctrl_reg);
		if(onms_onms2_ctrl_reg.on2_start_ms) {
			if(!de_only_mode)
			{
				onms_onms2_ctrl_reg.on2_popup_meas = 1;
				IoReg_Write32(ONMS_onms2_ctrl_reg, onms_onms2_ctrl_reg.regValue);
				onms_onms2_ctrl_reg.regValue = IoReg_Read32(ONMS_onms2_ctrl_reg);
				while(onms_onms2_ctrl_reg.on2_popup_meas && onms_onms2_ctrl_reg.on2_start_ms && timeout)
				{
					timeout--;
					msleep(10);
					onms_onms2_ctrl_reg.regValue = IoReg_Read32(ONMS_onms2_ctrl_reg);
				}
				// onms_onms2_vs_period_reg.regValue = IoReg_Read32(ONMS_onms2_vs_period_reg);
				// onms_onms2_hs_period_reg.regValue = IoReg_Read32(ONMS_onms2_hs_period_reg);
			}


			//B802125c[30:24] = 8 (line diff)
			//B802125c[19:16] = 2 or 3 (line average)
			//B802125c[13:0] = hotal from b8021238[17:4]
			//B8021260[27:16] = source vtotal-10
			//B8021260[11:0] = 10 after Vsync line number, avoid clamp or macrovision
			onms_onms2_hsync_meas_ctrl_reg.regValue = IoReg_Read32(ONMS_onms2_hsync_meas_ctrl_reg);
			if (de_only_mode) {
				onms_onms2_demode_hs_period_RBUS onms_onms2_demode_hs_period_reg;
				onms_onms2_demode_hs_period_reg.regValue = IoReg_Read32(ONMS_Onms2_Demode_Hs_period_reg);
				onms_onms2_hsync_meas_ctrl_reg.on2_target_measure = onms_onms2_demode_hs_period_reg.on2_de_hs_period;
				onms_onms2_hsync_meas_ctrl_reg.on2_average_line = 3;
				onms_onms2_hsync_meas_ctrl_reg.on2_htotal_max_delta_new = 8;
			} else {
				// onms_onms2_hsync_meas_ctrl_reg.on2_target_measure = onms_onms2_hs_period_reg.on2_htotal_h12b;
				onms_onms2_hsync_meas_ctrl_reg.on2_average_line = 3;
				onms_onms2_hsync_meas_ctrl_reg.on2_htotal_max_delta_new = 8;
			}
			IoReg_Write32(ONMS_onms2_hsync_meas_ctrl_reg, onms_onms2_hsync_meas_ctrl_reg.regValue);

			if(!de_only_mode)
			{
				onms_onms2_hsync_meas_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms2_hsync_meas_ctrl_2_reg);
				onms_onms2_hsync_meas_ctrl_2_reg.on2_vmeas_start = 16;
				// onms_onms2_hsync_meas_ctrl_2_reg.on2_vmeas_end = onms_onms2_vs_period_reg.on2_vtotal - 16;
				IoReg_Write32(ONMS_onms2_hsync_meas_ctrl_2_reg, onms_onms2_hsync_meas_ctrl_2_reg.regValue);
			}

			onms_onms2_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS_onms2_hsync_meas_result_reg);
			onms_onms2_hsync_meas_result_reg.on2_hmax_delta_total = 0; //write clear
			IoReg_Write32(ONMS_onms2_hsync_meas_result_reg, onms_onms2_hsync_meas_result_reg.regValue);

			onms_onms2_status_reg.regValue = 0;
			onms_onms2_status_reg.on2_hs_meas_over_range = 1;//write clean bit 21
			IoReg_Write32(ONMS_onms2_status_reg, onms_onms2_status_reg.regValue);
		}
	}
#endif
}

void drvif_mode_enable_hsync_period_timeout_wdg_isr_impl(unsigned char online_path)
{
	unsigned int ulTimeout_cnt = 0;

#ifdef DISABLE_ONLINE_CHECK
	return;
#endif

	if(online_path == VGIP_PATH_I1)
	{
		rtd_pr_onms_info("[%s] %d (90K:%d) VGIP_PATH_I1, old 0xB8021268=0x%x, 0xB802126C=0x%x, 0xB8021270=0x%x\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), IoReg_Read32(ONMS_onms1_status_reg), IoReg_Read32(ONMS_onms1_watchdog_en_reg), IoReg_Read32(ONMS_onms1_interrupt_en_reg));

		//Check hs_per_timeout flag is triggered or not
		if(ONMS_onms1_status_get_on1_hs_per_timeout(IoReg_Read32(ONMS_onms1_status_reg)))
		{
			//Clear hs_per_timeout flag
			IoReg_SetBits(ONMS_onms1_status_reg, ONMS_onms1_status_on1_hs_per_timeout_mask);

			//Check hs_per_timeout flag is cleared correctly
			while((ONMS_onms1_status_get_on1_hs_per_timeout(IoReg_Read32(ONMS_onms1_status_reg))) && (ulTimeout_cnt < 25))
			{
				ulTimeout_cnt ++;
				mdelay(1);

				IoReg_SetBits(ONMS_onms1_status_reg, ONMS_onms1_status_on1_hs_per_timeout_mask);
			}

			rtd_pr_onms_info("[%s] %d (90K:%d) Clear on1_hs_per_timeout pass, ulTimeout_cnt=%d, 0xB8021268=0x%x\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), ulTimeout_cnt, IoReg_Read32(ONMS_onms1_status_reg));

			//Timeout, don't enable hs_per_timeout wdg & ISR
			if (ulTimeout_cnt >= 25)
			{
				rtd_pr_onms_err("[%s] %d (90K:%d) Error! Clear on1_hs_per_timeout fail! 0xB8021268=0x%x\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), IoReg_Read32(ONMS_onms1_status_reg));
				return;
			}
		}

		//Enable hs_per_timeout wdg & ISR
		IoReg_SetBits(ONMS_onms1_watchdog_en_reg, ONMS_onms1_watchdog_en_on1_wd_hs_per_timeout_mask);
		IoReg_SetBits(ONMS_onms1_interrupt_en_reg, ONMS_onms1_interrupt_en_on1_ie_hs_per_timeout_mask);
		rtd_pr_onms_info("[%s] %d (90K:%d) VGIP_PATH_I1, new 0xB8021268=0x%x, 0xB802126C=0x%x, 0xB8021270=0x%x\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), IoReg_Read32(ONMS_onms1_status_reg), IoReg_Read32(ONMS_onms1_watchdog_en_reg), IoReg_Read32(ONMS_onms1_interrupt_en_reg));
	}
	else if(online_path == VGIP_PATH_I2)
	{
		rtd_pr_onms_info("[%s] %d (90K:%d) VGIP_PATH_I2, old 0xB8021368=0x%x, 0xB802136C=0x%x, 0xB8021370=0x%x\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), IoReg_Read32(ONMS_onms2_status_reg), IoReg_Read32(ONMS_onms2_watchdog_en_reg), IoReg_Read32(ONMS_onms2_interrupt_en_reg));

		//Check hs_per_timeout flag is triggered or not
		if(ONMS_onms2_status_get_on2_hs_per_timeout(IoReg_Read32(ONMS_onms2_status_reg)))
		{
			//Clear hs_per_timeout flag
			IoReg_SetBits(ONMS_onms2_status_reg, ONMS_onms2_status_on2_hs_per_timeout_mask);

			//Check hs_per_timeout flag is cleared correctly
			while((ONMS_onms2_status_get_on2_hs_per_timeout(IoReg_Read32(ONMS_onms2_status_reg))) && (ulTimeout_cnt < 25))
			{
				ulTimeout_cnt ++;
				mdelay(2);

				IoReg_SetBits(ONMS_onms2_status_reg, ONMS_onms2_status_on2_hs_per_timeout_mask);
			}

			rtd_pr_onms_info("[%s] %d (90K:%d) Clear on2_hs_per_timeout pass, ulTimeout_cnt=%d, 0xB8021368=0x%x\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), ulTimeout_cnt, IoReg_Read32(ONMS_onms2_status_reg));

			//Timeout, don't enable hs_per_timeout wdg & ISR
			if (ulTimeout_cnt >= 25)
			{
				rtd_pr_onms_err("[%s] %d (90K:%d) Error! Clear on2_hs_per_timeout fail! 0xB8021368=0x%x\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), IoReg_Read32(ONMS_onms2_status_reg));
				return;
			}
		}

		//Enable hs_per_timeout wdg & ISR
		IoReg_SetBits(ONMS_onms2_watchdog_en_reg, ONMS_onms2_watchdog_en_on2_wd_hs_per_timeout_mask);
		IoReg_SetBits(ONMS_onms2_interrupt_en_reg, ONMS_onms2_interrupt_en_on2_ie_hs_per_timeout_mask);
		rtd_pr_onms_info("[%s] %d (90K:%d) VGIP_PATH_I2, new 0xB8021368=0x%x, 0xB802136C=0x%x, 0xB8021370=0x%x\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), IoReg_Read32(ONMS_onms2_status_reg), IoReg_Read32(ONMS_onms2_watchdog_en_reg), IoReg_Read32(ONMS_onms2_interrupt_en_reg));
	}
	else if(online_path == VGIP_PATH_I3)
	{
		rtd_pr_onms_info("[%s] %d (90K:%d) VGIP_PATH_I3, old 0xB8021468=0x%x, 0xB802146C=0x%x, 0xB8021470=0x%x\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), IoReg_Read32(ONMS_onms3_status_reg), IoReg_Read32(ONMS_onms3_watchdog_en_reg), IoReg_Read32(ONMS_onms3_interrupt_en_reg));

		//Check hs_per_timeout flag is triggered or not
		if(ONMS_onms3_status_get_on3_hs_per_timeout(IoReg_Read32(ONMS_onms3_status_reg)))
		{
			//Clear hs_per_timeout flag
			IoReg_SetBits(ONMS_onms3_status_reg, ONMS_onms3_status_on3_hs_per_timeout_mask);

			//Check hs_per_timeout flag is cleared correctly
			while((ONMS_onms3_status_get_on3_hs_per_timeout(IoReg_Read32(ONMS_onms3_status_reg))) && (ulTimeout_cnt < 25))
			{
				ulTimeout_cnt ++;
				mdelay(2);

				IoReg_SetBits(ONMS_onms3_status_reg, ONMS_onms3_status_on3_hs_per_timeout_mask);
			}

			rtd_pr_onms_info("[%s] %d (90K:%d) Clear on3_hs_per_timeout pass, ulTimeout_cnt=%d, 0xB8021468=0x%x\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), ulTimeout_cnt, IoReg_Read32(ONMS_onms3_status_reg));

			//Timeout, don't enable hs_per_timeout wdg & ISR
			if (ulTimeout_cnt >= 25)
			{
				rtd_pr_onms_err("[%s] %d (90K:%d) Error! Clear on3_hs_per_timeout fail! 0xB8021468=0x%x\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), IoReg_Read32(ONMS_onms3_status_reg));
				return;
			}
		}

		//Enable hs_per_timeout wdg & ISR
		IoReg_SetBits(ONMS_onms3_watchdog_en_reg, ONMS_onms3_watchdog_en_on3_wd_hs_per_timeout_mask);
		IoReg_SetBits(ONMS_onms3_interrupt_en_reg, ONMS_onms3_interrupt_en_on3_ie_hs_per_timeout_mask);
		rtd_pr_onms_info("[%s] %d (90K:%d) VGIP_PATH_I3, new 0xB8021468=0x%x, 0xB802146C=0x%x, 0xB8021470=0x%x\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), IoReg_Read32(ONMS_onms3_status_reg), IoReg_Read32(ONMS_onms3_watchdog_en_reg), IoReg_Read32(ONMS_onms3_interrupt_en_reg));
	}
	else
	{
		rtd_pr_onms_err("[%s] %d (90K:%d) Error! Unknown online_path: %d\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), online_path);
	}
}
void (*drvif_mode_enable_hsync_period_timeout_wdg_isr)(unsigned char online_path) = drvif_mode_enable_hsync_period_timeout_wdg_isr_impl;

extern unsigned char vsc_get_adaptivestream_flag(unsigned char display);
void drvif_mode_onlinemeasure_setting(unsigned char channel, int src, unsigned char wdgenable, unsigned char interruptenable)
{//This function is to set onlinemeasure watchdog and interrupt setting
	unsigned int online_wd_int_setting = 0;//decide wd and int monitor bit
	onms_onms1_watchdog_en_RBUS onms_onms1_watchdog_en_reg;
	onms_onms1_interrupt_en_RBUS onms_onms1_interrupt_en_reg;
	vgip_int_ctl_RBUS vgip_int_ctl_reg;
	ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;
	ppoverlay_display_timing_ctrl1_RBUS ppoverlay_display_timing_ctrl1_reg;

#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	onms_onms2_watchdog_en_RBUS onms_onms2_watchdog_en_reg;
	ppoverlay_mp_layout_force_to_background_RBUS ppoverlay_mp_layout_force_to_background_reg;
	onms_onms2_interrupt_en_RBUS onms_onms2_interrupt_en_reg;
#endif

#ifndef CONFIG_DUAL_CHANNEL
	channel = _CHANNEL1;
#endif

#ifdef POLLING_ONLINE_STATUS_ENABLE//no need to enable interrupt when enable polling flow
	wdgenable = FALSE;
	interruptenable = FALSE;
#endif

	rtd_pr_onms_info("###func %s channel:%d src:%d wdg:%d int:%d###\r\n",__FUNCTION__, channel, src, wdgenable, interruptenable);

	if((wdgenable == _DISABLE) && (interruptenable == _DISABLE) && (_CHANNEL1 == channel))
	{//disable dma online measure
		drvif_mode_dma_onlinemeasure_setting(_DISABLE, _DISABLE);
	}

	// patch for structure g_pcbSourceTable in pcbSource.c
	if((src == _SRC_HDMI) && (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_DP))
	{
		src = _SRC_MINI_DP;
		rtd_pr_onms_info("###func %s update src from HDMI to DP, src:%d###\r\n",__FUNCTION__, src);
	}

	if(wdgenable || interruptenable)
	{
		if((_CHANNEL1 == channel) && (src == _SRC_HDMI)) // Main path HDMI case will control & init. in drvif_hdmi_onlinemeasure_watchdog_early_enable_setting
		{
			rtd_pr_onms_info("Main path HDMI onms ctrl setting is done already in drvif_hdmi_onlinemeasure_watchdog_early_enable_setting\n");
		}
		else
		{
			drvif_mode_onms_ctrl_setting(channel, src);
		}

		online_wd_int_setting = drvif_mode_onms_int_setting(channel, src);
	}

	if(_ENABLE == wdgenable)
	{
		if(_CHANNEL1 == channel)
		{
			if(src != _SRC_HDMI)
			{
				onms_onms1_watchdog_en_reg.regValue = 0;
				onms_onms1_watchdog_en_reg.regValue = online_wd_int_setting;
				onms_onms1_watchdog_en_reg.on1_wd_to_main = _TRUE;		//Watch Dog select to Main Display
				IoReg_Write32(ONMS_onms1_watchdog_en_reg, onms_onms1_watchdog_en_reg.regValue);

				down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/

				ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);

				if (((src == _SRC_YPBPR) || (src == _SRC_VGA) || (src == _SRC_CVBS) || (src == _SRC_MINI_DP))&&(!Get_tv006_wb_pattern())) {
					ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _ENABLE;//Enable force BG
				} else {
					ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _DISABLE;//Disable forced bg for customer//_ENABLE;	//Main Watch Dog Enable (Force-to-Background)
				}
				ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = _ENABLE;//FrameSync Main Watch Dog Enable (FreeRun)
				IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
				up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
			}
		}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
		else// if(_CHANNEL2 == channel)
		{
			onms_onms2_watchdog_en_reg.regValue = 0;
			onms_onms2_watchdog_en_reg.regValue = online_wd_int_setting;
			if (src == _SRC_HDMI) {
				{// Only for HDMI source, check particular device or debugging command
					unsigned int original_value = onms_onms2_watchdog_en_reg.regValue;
					onms_onms2_watchdog_en_reg.regValue = get_result_with_hdmi_mask(original_value);
					if(onms_onms2_watchdog_en_reg.regValue != original_value)
					{
						rtd_pr_onms_info("[%s][HDMI] channel: %d, Update onms wdg from %x to %x\n", __FUNCTION__, channel, original_value, onms_onms2_watchdog_en_reg.regValue);
					}
				}
			}
			onms_onms2_watchdog_en_reg.on2_wd_to_sub = _TRUE;		//Watch Dog select to Sub Display
			IoReg_Write32(ONMS_onms2_watchdog_en_reg, onms_onms2_watchdog_en_reg.regValue);

			//add semaphore for sub disp @Crixus 20150901
			down(get_forcebg_semaphore());

			ppoverlay_mp_layout_force_to_background_reg.regValue = IoReg_Read32(PPOVERLAY_MP_Layout_Force_to_Background_reg);
			if ((src == _SRC_YPBPR) || (src == _SRC_VGA) || (src == _SRC_CVBS) || (src == _SRC_HDMI) || (src == _SRC_MINI_DP)) {
				ppoverlay_mp_layout_force_to_background_reg.sub_wd_to_background = _ENABLE;//Enable forced bg for customer //_ENABLE;	//Sub Watch Dog Enable (Force-to-Background)
			} else {
				ppoverlay_mp_layout_force_to_background_reg.sub_wd_to_background = _DISABLE;//Disable forced bg for customer //_ENABLE;	//Sub Watch Dog Enable (Force-to-Background)
			}

			ppoverlay_mp_layout_force_to_background_reg.sub_wd_to_free_run = _DISABLE;	//FrameSync Sub Watch Dog Enable (FreeRun), sub can't enable free run
			IoReg_Write32(PPOVERLAY_MP_Layout_Force_to_Background_reg, ppoverlay_mp_layout_force_to_background_reg.regValue);
			up(get_forcebg_semaphore());
		}
#endif

		//FrameSync Watch Dog Enable (FreeRun)
		ppoverlay_display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
		//fix me later. Ben_Wang add cause data framesync always triiger wdg to free run.
#if 0
		if(Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_FRAMESYNC))
			ppoverlay_display_timing_ctrl1_reg.wde_to_free_run = _DISABLE;
		else
#endif
		ppoverlay_display_timing_ctrl1_reg.wde_to_free_run = _ENABLE;
		IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, ppoverlay_display_timing_ctrl1_reg.regValue);
	}
	else// if(_DISABLE == enable)
	{
		if(_CHANNEL1 == channel)
		{
			onms_onms1_watchdog_en_reg.regValue = 0;
			onms_onms1_watchdog_en_reg.on1_wd_to_main = _FALSE;		//Watch Dog select to Main Display
			IoReg_Write32(ONMS_onms1_watchdog_en_reg, onms_onms1_watchdog_en_reg.regValue);
			down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
			ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);
			ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _DISABLE;	//Main Watch Dog Enable (Force-to-Background)
			ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = _DISABLE;	//FrameSync Main Watch Dog Enable (FreeRun)
			IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
			up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
		else// if(_CHANNEL2 == channel)
		{
			onms_onms2_watchdog_en_reg.regValue = 0;
			onms_onms2_watchdog_en_reg.on2_wd_to_sub = _FALSE;		//Watch Dog select to SUB Display
			IoReg_Write32(ONMS_onms2_watchdog_en_reg, onms_onms2_watchdog_en_reg.regValue);
			//add semaphore for sub disp @Crixus 20150901
			down(get_forcebg_semaphore());
			ppoverlay_mp_layout_force_to_background_reg.regValue = IoReg_Read32(PPOVERLAY_MP_Layout_Force_to_Background_reg);
			ppoverlay_mp_layout_force_to_background_reg.sub_wd_to_background = _DISABLE;//Disable forced bg for customer //_ENABLE;	//Sub Watch Dog Enable (Force-to-Background)
			ppoverlay_mp_layout_force_to_background_reg.sub_wd_to_free_run = _DISABLE;	//FrameSync Sub Watch Dog Enable (FreeRun), sub can't enable free run
			IoReg_Write32(PPOVERLAY_MP_Layout_Force_to_Background_reg, ppoverlay_mp_layout_force_to_background_reg.regValue);
			up(get_forcebg_semaphore());
		}
#endif
	}

	if(_ENABLE == interruptenable)
	{
		if(_CHANNEL1 == channel)
		{
			//onms_onms1_interrupt_en_reg.regValue = IoReg_Read32(ONMS_onms1_interrupt_en_reg);
			onms_onms1_interrupt_en_reg.regValue = 0;
			onms_onms1_interrupt_en_reg.regValue = online_wd_int_setting;
			if (src == _SRC_HDMI) {
				{// Only for HDMI source, check particular device or debugging command
					unsigned int original_value = onms_onms1_interrupt_en_reg.regValue;
					onms_onms1_interrupt_en_reg.regValue = get_result_with_hdmi_mask(original_value);
					if(onms_onms1_interrupt_en_reg.regValue != original_value)
					{
						rtd_pr_onms_info("[%s][HDMI] channel: %d, Update onms int from %x to %x\n", __FUNCTION__, channel, original_value, onms_onms1_interrupt_en_reg.regValue);
					}
				}
			}
			onms_onms1_interrupt_en_reg.on1_ie_to_main = _TRUE;		//Watch Dog select to Main Display
			IoReg_Write32(ONMS_onms1_interrupt_en_reg, onms_onms1_interrupt_en_reg.regValue);

			if(((src == _SRC_HDMI) && (get_scaler_qms_mode_flag() == FALSE)) || (src == _SRC_MINI_DP))
				drvif_mode_enable_hsync_period_timeout_wdg_isr(VGIP_PATH_I1);

			vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
			vgip_int_ctl_reg.onms1_int_ie = 1;//Enable onlinemeasure interrupt
			IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);
		}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
		else// if(_CHANNEL2 == channel)
		{
			//onms_onms2_interrupt_en_reg.regValue = IoReg_Read32(ONMS_onms1_interrupt_en_reg);
			onms_onms2_interrupt_en_reg.regValue = 0;
			onms_onms2_interrupt_en_reg.regValue = online_wd_int_setting;
			if (src == _SRC_HDMI) {
				{// Only for HDMI source, check particular device or debugging command
					unsigned int original_value = onms_onms2_interrupt_en_reg.regValue;
					onms_onms2_interrupt_en_reg.regValue = get_result_with_hdmi_mask(original_value);
					if(onms_onms2_interrupt_en_reg.regValue != original_value)
					{
						rtd_pr_onms_info("[%s][HDMI] channel: %d, Update onms int from %x to %x\n", __FUNCTION__, channel, original_value, onms_onms2_interrupt_en_reg.regValue);
					}
				}
			}
			onms_onms2_interrupt_en_reg.on2_ie_to_main = _TRUE;		//Watch Dog select to Main Display
			IoReg_Write32(ONMS_onms2_interrupt_en_reg, onms_onms2_interrupt_en_reg.regValue);

			if(((src == _SRC_HDMI) && (get_scaler_qms_mode_flag() == FALSE)) || (src == _SRC_MINI_DP))
				drvif_mode_enable_hsync_period_timeout_wdg_isr(VGIP_PATH_I2);

			vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
			vgip_int_ctl_reg.onms2_int_ie = 1;//Enable onlinemeasure interrupt
			IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);
		}
#endif
	}
	else
	{
		if(_CHANNEL1 == channel)
		{
			//onms_onms1_interrupt_en_reg.regValue = IoReg_Read32(ONMS_onms1_interrupt_en_reg);
			onms_onms1_interrupt_en_reg.regValue = 0;
			onms_onms1_interrupt_en_reg.on1_ie_to_main = _FALSE;		//Watch Dog select to Main Display
			IoReg_Write32(ONMS_onms1_interrupt_en_reg, onms_onms1_interrupt_en_reg.regValue);
			vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
			vgip_int_ctl_reg.onms1_int_ie = 0;//Disable onlinemeasure interrupt
			IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);
		}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
		else// if(_CHANNEL2 == channel)
		{
			//onms_onms2_interrupt_en_reg.regValue = IoReg_Read32(ONMS_onms1_interrupt_en_reg);
			onms_onms2_interrupt_en_reg.regValue = 0;
			onms_onms2_interrupt_en_reg.on2_ie_to_main = _FALSE;		//Watch Dog select to Main Display
			IoReg_Write32(ONMS_onms2_interrupt_en_reg, onms_onms2_interrupt_en_reg.regValue);
			vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
			vgip_int_ctl_reg.onms2_int_ie = 0;//Disable onlinemeasure interrupt
			IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);
		}
#endif
	}
}

void drvif_hdmi_onlinemeasure_watchdog_early_enable_setting(void)
{//This function is to set onlinemeasure watchdog setting
	unsigned int online_wd_int_setting = 0;//decide wd and int monitor bit
	onms_onms1_watchdog_en_RBUS onms_onms1_watchdog_en_reg;
	ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;
	ppoverlay_display_timing_ctrl1_RBUS ppoverlay_display_timing_ctrl1_reg;
	ppoverlay_display_timing_ctrl6_RBUS ppoverlay_display_timing_ctrl6_reg;
	
	rtd_pr_onms_info("###func %s ###\r\n",__FUNCTION__);

	// ONMS ctrl & init. setting
	drvif_mode_onms_ctrl_setting(_CHANNEL1, _SRC_HDMI);
	online_wd_int_setting = drvif_mode_onms_int_setting(_CHANNEL1, _SRC_HDMI);

	// ONMS watchdog enable
	onms_onms1_watchdog_en_reg.regValue = 0;
	onms_onms1_watchdog_en_reg.regValue = online_wd_int_setting;
	{// Only for HDMI source, check particular device or debugging command
		unsigned int original_value = onms_onms1_watchdog_en_reg.regValue;
		onms_onms1_watchdog_en_reg.regValue = get_result_with_hdmi_mask(original_value);
		if(onms_onms1_watchdog_en_reg.regValue != original_value)
		{
			rtd_pr_onms_info("[%s][HDMI] channel:1, Update onms wdg from %x to %x\n", __FUNCTION__, original_value, onms_onms1_watchdog_en_reg.regValue);
		}
	}
	onms_onms1_watchdog_en_reg.on1_wd_to_main = _TRUE;		//Watch Dog select to Main Display
	IoReg_Write32(ONMS_onms1_watchdog_en_reg, onms_onms1_watchdog_en_reg.regValue);
	down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);

	if (!Get_tv006_wb_pattern()) {
		ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _ENABLE;//Enable force BG
	} else {
		ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _DISABLE;//Disable forced bg for customer//_ENABLE;	//Main Watch Dog Enable (Force-to-Background)
	}

	ppoverlay_main_display_control_rsv_reg.disp2_main_wd_to_free_run = _ENABLE;
	ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = _ENABLE;//FrameSync Main Watch Dog Enable (FreeRun)
	IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
	up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	//FrameSync Watch Dog Enable (FreeRun)
	ppoverlay_display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
	ppoverlay_display_timing_ctrl1_reg.wde_to_free_run = _ENABLE;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, ppoverlay_display_timing_ctrl1_reg.regValue);

	ppoverlay_display_timing_ctrl6_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL6_reg);
	ppoverlay_display_timing_ctrl6_reg.disp2_wde_to_free_run = _ENABLE;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL6_reg, ppoverlay_display_timing_ctrl6_reg.regValue);
}

unsigned int get_hdmi_online_wd_int_setting(void)
{
    unsigned int online_wd_int_setting = 0;

    //decide wd and int setting
    if(get_hdmi_4k_hfr_mode() == HDMI_NON_4K120)
    {//non 4k120 mode
        if(get_scaler_qms_mode_flag())
        {//qms case similar with vrr
            online_wd_int_setting = HDMI_QMS_ONLINE_ENABLE_MASK;
        }
        else if(vbe_disp_get_VRR_timingMode_flag())
        {
            online_wd_int_setting = HDMI_VRR_ONLINE_ENABLE_MASK;
        }
        else if(vbe_disp_get_freesync_mode_flag())
        {
            online_wd_int_setting = HDMI_FREESYNC_ONLINE_ENABLE_MASK;
        }
        else
        {
            online_wd_int_setting = HDMI_NORMAL_ONLINE_ENABLE_MASK;
        }
    }
    else
    {//4k120 mode
        if(get_scaler_qms_mode_flag())
        {//qms case similar with vrr
            online_wd_int_setting = HDMI_4K120_QMS_ONLINE_ENABLE_MASK;
        }
        else if(vbe_disp_get_VRR_timingMode_flag())
        {
            online_wd_int_setting = HDMI_4K120_VRR_ONLINE_ENABLE_MASK;
        }
        else if(vbe_disp_get_freesync_mode_flag())
        {
            online_wd_int_setting = HDMI_4K120_FREESYNC_ONLINE_ENABLE_MASK;
        }
        else if(get_i3ddma_2p_mode_flag())
        {
            online_wd_int_setting = HDMI_NORMAL_ONLINE_ENABLE_MASK;
        }
        else
        {
            online_wd_int_setting = HDMI_4K120_ONLINE_ENABLE_MASK;
        }
    }

    return online_wd_int_setting;
}


void drvif_hdmi_dma_onlinemeasure_watchdog_early_enable_setting(void)
{//for online watchdog early enable to protection output timing
	onms_onms3_ctrl_RBUS onms_onms3_ctrl_reg;
	//onms_onms3_hs_period_RBUS onms_onms3_hs_period_reg;
	onms_onms3_demode_hs_period_RBUS onms_onms3_demode_hs_period_reg;
	onms_onms3_hsync_meas_ctrl_RBUS onms_onms3_hsync_meas_ctrl_reg;
	onms_onms3_status_RBUS onms_onms3_status_reg;
	onms_onms3_watchdog_en_RBUS onms_onms3_watchdog_en_reg;
	ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;
	ppoverlay_display_timing_ctrl1_RBUS ppoverlay_display_timing_ctrl1_reg;
	onms_onms3_hsync_meas_result_RBUS onms_onms3_hsync_meas_result_reg;
	ppoverlay_display_timing_ctrl6_RBUS ppoverlay_display_timing_ctrl6_reg;
	
	rtd_pr_onms_info("###func %s ###\r\n",__FUNCTION__);
	onms_onms3_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_reg);
	if(onms_onms3_ctrl_reg.on3_start_ms) {
		//rtd_pr_onms_info("\r\n#####on3_htotal_h12b = 0x%8x ####\r\n", onms_onms3_hs_period_reg.on3_htotal_h12b);

		onms_onms3_hsync_meas_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_hsync_meas_ctrl_reg);
		onms_onms3_demode_hs_period_reg.regValue = IoReg_Read32(ONMS_onms3_Demode_Hs_period_reg);
		//B802145c[13:0] = hotal from b8021238[17:4]
		onms_onms3_hsync_meas_ctrl_reg.on3_target_measure = onms_onms3_demode_hs_period_reg.on3_de_hs_period;
		//B802145c[19:16] = 2 or 3 (line average)
		onms_onms3_hsync_meas_ctrl_reg.on3_average_line = 3;
		//B802145c[30:24] = 8 (line diff)
		onms_onms3_hsync_meas_ctrl_reg.on3_htotal_max_delta_new = 8;
		IoReg_Write32(ONMS_onms3_hsync_meas_ctrl_reg, onms_onms3_hsync_meas_ctrl_reg.regValue);

		onms_onms3_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS_onms3_hsync_meas_result_reg);
		onms_onms3_hsync_meas_result_reg.on3_hmax_delta_total = 0;	//write clear
		IoReg_Write32(ONMS_onms3_hsync_meas_result_reg, onms_onms3_hsync_meas_result_reg.regValue);

		onms_onms3_status_reg.regValue = 0;
		onms_onms3_status_reg.on3_hs_meas_over_range = 1;//write clean bit 21
		IoReg_Write32(ONMS_onms3_status_reg, onms_onms3_status_reg.regValue);
	}

	onms_onms3_watchdog_en_reg.regValue = 0;
	onms_onms3_watchdog_en_reg.regValue = get_hdmi_online_wd_int_setting();
	{// Only for HDMI source, check particular device or debugging command
		unsigned int original_value = onms_onms3_watchdog_en_reg.regValue;
		onms_onms3_watchdog_en_reg.regValue = get_result_with_hdmi_mask(original_value);
		if(onms_onms3_watchdog_en_reg.regValue != original_value)
		{
			rtd_pr_onms_info("[%s][HDMI] Update onms wd from %x to %x\n", __FUNCTION__, original_value, onms_onms3_watchdog_en_reg.regValue);
		}
	}
	onms_onms3_watchdog_en_reg.on3_wd_to_main = _TRUE;	   //Watch Dog select to Main Display
	IoReg_Write32(ONMS_onms3_watchdog_en_reg, onms_onms3_watchdog_en_reg.regValue);
	down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);

	if (!Get_tv006_wb_pattern()) {
		ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _ENABLE;//Enable force BG
	} else {
		ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _DISABLE;//Disable forced bg for customer//_ENABLE;	//Main Watch Dog Enable (Force-to-Background)
	}

	ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = _ENABLE;	//FrameSync Main Watch Dog Enable (FreeRun)
	ppoverlay_main_display_control_rsv_reg.disp2_main_wd_to_free_run = _ENABLE;
	IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
	up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	//FrameSync Watch Dog Enable (FreeRun)
	ppoverlay_display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
	//fix me later. Ben_Wang add cause data framesync always triiger wdg to free run.
	ppoverlay_display_timing_ctrl1_reg.wde_to_free_run = _ENABLE;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, ppoverlay_display_timing_ctrl1_reg.regValue);

	ppoverlay_display_timing_ctrl6_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL6_reg);
	ppoverlay_display_timing_ctrl6_reg.disp2_wde_to_free_run = _ENABLE;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL6_reg, ppoverlay_display_timing_ctrl6_reg.regValue);

#ifndef UT_flag
	//Update watchdog mask
	current_hdmi_onms_watchdog_setting |= IoReg_Read32(ONMS_onms3_watchdog_en_reg);;
	current_hdmi_onms_watchdog_setting |=ONMS_onms3_watchdog_en_on3_wd_vtotalde_chg_mask;
	current_hdmi_onms_watchdog_setting |=ONMS_onms3_watchdog_en_on3_wd_hs_active_pixel_var_mask;

	current_hdmi_onms_is_i3ddma = TRUE;
	pr_info("[%s][HDMI] Update current_hdmi_onms_watchdog_setting=%x, current_hdmi_onms_is_i3ddma=%x\n", __FUNCTION__, current_hdmi_onms_watchdog_setting, current_hdmi_onms_is_i3ddma);
#endif
}

void drvif_mode_dma_onlinemeasure_setting(unsigned char wdgenable, unsigned char interruptenable)
{//This function is to set onlinemeasure watchdog and interrupt setting
	VSC_INPUT_TYPE_T src;
	int timeout = 10;
	unsigned int online_wd_int_setting = 0;
	onms_onms3_ctrl_RBUS onms_onms3_ctrl_reg;
	unsigned char de_only_mode = 0;
	// onms_onms3_vs_period_RBUS onms_onms3_vs_period_reg;
	// onms_onms3_hs_period_RBUS onms_onms3_hs_period_reg;
	onms_onms3_hsync_meas_ctrl_RBUS onms_onms3_hsync_meas_ctrl_reg;
	onms_onms3_hsync_meas_ctrl_2_RBUS onms_onms3_hsync_meas_ctrl_2_reg;
	onms_onms3_status_RBUS onms_onms3_status_reg;
	onms_onms3_demode_hs_period_RBUS onms_onms3_demode_hs_period_reg;

	onms_onms3_watchdog_en_RBUS onms_onms3_watchdog_en_reg;
	onms_onms3_interrupt_en_RBUS onms_onms3_interrupt_en_reg;
	vgip_int_ctl_RBUS vgip_int_ctl_reg;
	ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;
	ppoverlay_display_timing_ctrl1_RBUS ppoverlay_display_timing_ctrl1_reg;
	onms_onms3_hsync_meas_result_RBUS onms_onms3_hsync_meas_result_reg;

	src = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);
	if((src == VSC_INPUTSRC_HDMI) || (src == VSC_INPUTSRC_DP))
		de_only_mode = 1;

#ifdef POLLING_ONLINE_STATUS_ENABLE//no need to enable interrupt when enable polling flow
	wdgenable = FALSE;
	interruptenable = FALSE;
#endif

	rtd_pr_onms_info("###func %s src:%d wdg:%d int:%d###\r\n",__FUNCTION__, src, wdgenable, interruptenable);

	if(wdgenable || interruptenable)
	{	//dma online measure new status
		onms_onms3_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_reg);
		if((onms_onms3_ctrl_reg.on3_start_ms) && (src != VSC_INPUTSRC_HDMI)) {

			//rtd_pr_onms_info("\r\n#####p_timinginfo->IHCount = 0x%8x ####\r\n", p_timinginfo->IHCount);
			//rtd_pr_onms_info("\r\n#####on3_htotal_h12b = 0x%8x ####\r\n", onms_onms3_hs_period_reg.on3_htotal_h12b);
			if(!de_only_mode)
			{
				onms_onms3_ctrl_reg.on3_popup_meas = 1;
				IoReg_Write32(ONMS_onms3_ctrl_reg, onms_onms3_ctrl_reg.regValue);
				onms_onms3_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_reg);
				while(onms_onms3_ctrl_reg.on3_popup_meas && onms_onms3_ctrl_reg.on3_start_ms && timeout)
				{
					timeout--;
					msleep(10);
					onms_onms3_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_reg);
				}
			}

			onms_onms3_hsync_meas_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_hsync_meas_ctrl_reg);
			onms_onms3_demode_hs_period_reg.regValue = IoReg_Read32(ONMS_onms3_Demode_Hs_period_reg);
			//B802145c[13:0] = hotal from 0xB8021494[17:4]
			onms_onms3_hsync_meas_ctrl_reg.on3_target_measure = onms_onms3_demode_hs_period_reg.on3_de_hs_period;
			//B802145c[19:16] = 2 or 3 (line average)
			onms_onms3_hsync_meas_ctrl_reg.on3_average_line = 3;
			//B802145c[30:24] = 8 (line diff)
			onms_onms3_hsync_meas_ctrl_reg.on3_htotal_max_delta_new = 8;
			IoReg_Write32(ONMS_onms3_hsync_meas_ctrl_reg, onms_onms3_hsync_meas_ctrl_reg.regValue);

			if(!de_only_mode)
			{
				// onms_onms3_vs_period_reg.regValue = IoReg_Read32(ONMS_onms3_vs_period_reg);
				onms_onms3_hsync_meas_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms3_hsync_meas_ctrl_2_reg);
				//B8021460[27:16] = source vtotal-10
				//B8021460[11:0] = 10 after Vsync line number, avoid clamp or macrovision
				onms_onms3_hsync_meas_ctrl_2_reg.on3_vmeas_start = 16;
				// onms_onms3_hsync_meas_ctrl_2_reg.on3_vmeas_end = onms_onms3_vs_period_reg.on3_vtotal - 16;
				IoReg_Write32(ONMS_onms3_hsync_meas_ctrl_2_reg, onms_onms3_hsync_meas_ctrl_2_reg.regValue);
			}

			onms_onms3_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS_onms3_hsync_meas_result_reg);
			onms_onms3_hsync_meas_result_reg.on3_hmax_delta_total = 0;	//write clear
			IoReg_Write32(ONMS_onms3_hsync_meas_result_reg, onms_onms3_hsync_meas_result_reg.regValue);

			onms_onms3_status_reg.regValue = 0;
			onms_onms3_status_reg.on3_hs_meas_over_range = 1;//write clean bit 21
			IoReg_Write32(ONMS_onms3_status_reg, onms_onms3_status_reg.regValue);
		}

		//decide wd and int setting
		if(src == VSC_INPUTSRC_HDMI)
		{
			online_wd_int_setting = get_hdmi_online_wd_int_setting();
		}
		else if(src == VSC_INPUTSRC_DP)
		{
			if(vbe_disp_get_VRR_timingMode_flag())
			{
				online_wd_int_setting = DP_VRR_ONLINE_ENABLE_MASK;
			}
			else if(vbe_disp_get_freesync_mode_flag())
			{
				online_wd_int_setting = DP_FREESYNC_ONLINE_ENABLE_MASK;
			}
			else
			{
				online_wd_int_setting = DP_NORMAL_ONLINE_ENABLE_MASK;
			}
		}
		else if(src == VSC_INPUTSRC_AVD)
		{
			online_wd_int_setting = AVD_ONLINE_ENABLE_MASK;
		}
	}

	if(_ENABLE == wdgenable)
	{
		if(src != VSC_INPUTSRC_HDMI)
		{
			onms_onms3_watchdog_en_reg.regValue = 0;
			onms_onms3_watchdog_en_reg.regValue = online_wd_int_setting;
			onms_onms3_watchdog_en_reg.on3_wd_to_main = _TRUE;	   //Watch Dog select to Main Display
			IoReg_Write32(ONMS_onms3_watchdog_en_reg, onms_onms3_watchdog_en_reg.regValue);
			down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
			ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);

			if (((src == VSC_INPUTSRC_AVD) || (src == VSC_INPUTSRC_DP))&&(!Get_tv006_wb_pattern())) {
				ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _ENABLE;//Enable force BG
			} else {
				ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _DISABLE;//Disable forced bg for customer//_ENABLE;	//Main Watch Dog Enable (Force-to-Background)
			}

			ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = _ENABLE;	//FrameSync Main Watch Dog Enable (FreeRun)
			IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
			up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
			//FrameSync Watch Dog Enable (FreeRun)
			ppoverlay_display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
			//fix me later. Ben_Wang add cause data framesync always triiger wdg to free run.
			ppoverlay_display_timing_ctrl1_reg.wde_to_free_run = _ENABLE;
			IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, ppoverlay_display_timing_ctrl1_reg.regValue);
		}
	}
	else// if(_DISABLE == enable)
	{
		onms_onms3_watchdog_en_reg.regValue = 0;
		onms_onms3_watchdog_en_reg.on3_wd_to_main = _FALSE;		//Watch Dog select to Main Display
		IoReg_Write32(ONMS_onms3_watchdog_en_reg, onms_onms3_watchdog_en_reg.regValue);
		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);
		ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _DISABLE;	//Main Watch Dog Enable (Force-to-Background)
		ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = _DISABLE;	//FrameSync Main Watch Dog Enable (FreeRun)
		IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
		up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	}

	if(_ENABLE == interruptenable)
	{
		//onms_onms1_interrupt_en_reg.regValue = IoReg_Read32(ONMS_onms1_interrupt_en_reg);
		onms_onms3_interrupt_en_reg.regValue = 0;
		onms_onms3_interrupt_en_reg.regValue = online_wd_int_setting;
		if (src == VSC_INPUTSRC_HDMI) {
			{// Only for HDMI source, check particular device or debugging command
				unsigned int original_value = onms_onms3_interrupt_en_reg.regValue;
				onms_onms3_interrupt_en_reg.regValue = get_result_with_hdmi_mask(original_value);
				if(onms_onms3_interrupt_en_reg.regValue != original_value)
				{
					rtd_pr_onms_info("[%s][HDMI] Update onms int from %x to %x\n", __FUNCTION__, original_value, onms_onms3_interrupt_en_reg.regValue);
				}
			}
		}
		onms_onms3_interrupt_en_reg.on3_ie_to_main = _TRUE;		//Watch Dog select to Main Display
		IoReg_Write32(ONMS_onms3_interrupt_en_reg, onms_onms3_interrupt_en_reg.regValue);

		if(((src == VSC_INPUTSRC_HDMI) && (get_scaler_qms_mode_flag() == FALSE)) || (src == VSC_INPUTSRC_DP))
			drvif_mode_enable_hsync_period_timeout_wdg_isr(VGIP_PATH_I3);

		vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
		vgip_int_ctl_reg.onms3_int_ie = 1;//Enable onlinemeasure interrupt
		IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);
	}
	else
	{
		//onms_onms1_interrupt_en_reg.regValue = IoReg_Read32(ONMS_onms1_interrupt_en_reg);
		onms_onms3_interrupt_en_reg.regValue = 0;
		onms_onms3_interrupt_en_reg.on3_ie_to_main = _FALSE;		//Watch Dog select to Main Display
		IoReg_Write32(ONMS_onms3_interrupt_en_reg, onms_onms3_interrupt_en_reg.regValue);
		vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
		vgip_int_ctl_reg.onms3_int_ie = 0;//Disable onlinemeasure interrupt
		IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);
	}
#ifndef UT_flag
	if ((src == VSC_INPUTSRC_HDMI)&&(wdgenable || interruptenable)) {
		current_hdmi_onms_watchdog_setting |= IoReg_Read32(ONMS_onms3_watchdog_en_reg);;
		current_hdmi_onms_watchdog_setting |=ONMS_onms3_watchdog_en_on3_wd_vtotalde_chg_mask;
		current_hdmi_onms_watchdog_setting |=ONMS_onms3_watchdog_en_on3_wd_hs_active_pixel_var_mask;

		current_hdmi_onms_is_i3ddma= TRUE;
		pr_info("[%s][HDMI] Update current_hdmi_onms_watchdog_setting=%x, current_hdmi_onms_is_i3ddma=%x\n", __FUNCTION__, current_hdmi_onms_watchdog_setting, current_hdmi_onms_is_i3ddma);
#ifndef BUILD_QUICK_SHOW
		if ( get_scaler_qms_mode_flag()) {
			extern int qms_driver_enable;
			extern void on3_vtotal_delta_set(int delta, int ie);

			// to supress QMS flow to demo the FBG=1 across timing change
			if (qms_driver_enable == 0) { 
				// set On3_demode_vtotal_delta to trigger watchdog interrupt, then trigger setup IMD again.
				on3_vtotal_delta_set(500, 1);
			}
		}
#endif                
	}
#endif // #ifndef UT_flag
}


void drvif_mode_onms4_setting(unsigned char wdgenable, unsigned char interruptenable)
{//This function is to set onlinemeasure watchdog and interrupt setting
	VSC_INPUT_TYPE_T src;
	unsigned int online_wd_int_setting = 0;
	onms_onms4_ctrl_RBUS onms_onms4_ctrl_reg;
	// onms_onms4_vs_period_RBUS onms_onms4_vs_period_reg;
	// onms_onms4_hs_period_RBUS onms_onms4_hs_period_reg;
	onms_onms4_hsync_meas_ctrl_RBUS onms_onms4_hsync_meas_ctrl_reg;
	// onms_onms4_hsync_meas_ctrl_2_RBUS onms_onms4_hsync_meas_ctrl_2_reg;
	onms_onms4_status_RBUS onms_onms4_status_reg;
	onms_onms4_demode_hs_period_RBUS onms_onms4_demode_hs_period_reg;

	onms_onms4_watchdog_en_RBUS onms_onms4_watchdog_en_reg;
	onms_onms4_interrupt_en_RBUS onms_onms4_interrupt_en_reg;
	vgip_int_ctl_RBUS vgip_int_ctl_reg;
	ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;
	ppoverlay_display_timing_ctrl1_RBUS ppoverlay_display_timing_ctrl1_reg;
	onms_onms4_hsync_meas_result_RBUS onms_onms4_hsync_meas_result_reg;

	src = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);

#ifdef POLLING_ONLINE_STATUS_ENABLE//no need to enable interrupt when enable polling flow
	wdgenable = FALSE;
	interruptenable = FALSE;
#endif

	rtd_pr_onms_info("###func %s src:%d wdg:%d int:%d###\r\n",__FUNCTION__, src, wdgenable, interruptenable);

	if(wdgenable || interruptenable)
	{	//dma online measure new status
		onms_onms4_ctrl_reg.regValue = IoReg_Read32(ONMS_onms4_ctrl_reg);
		if((onms_onms4_ctrl_reg.on4_start_ms) && (src != VSC_INPUTSRC_HDMI)) {

			onms_onms4_hsync_meas_ctrl_reg.regValue = IoReg_Read32(ONMS_onms4_hsync_meas_ctrl_reg);
			onms_onms4_demode_hs_period_reg.regValue = IoReg_Read32(ONMS_onms4_Demode_Hs_period_reg);
			//B802145c[13:0] = hotal from 0xB8021494[17:4]
			onms_onms4_hsync_meas_ctrl_reg.on4_target_measure = onms_onms4_demode_hs_period_reg.on4_de_hs_period;
			//B802145c[19:16] = 2 or 3 (line average)
			onms_onms4_hsync_meas_ctrl_reg.on4_average_line = 3;
			//B802145c[30:24] = 8 (line diff)
			onms_onms4_hsync_meas_ctrl_reg.on4_htotal_max_delta_new = 8;
			IoReg_Write32(ONMS_onms4_hsync_meas_ctrl_reg, onms_onms4_hsync_meas_ctrl_reg.regValue);

			onms_onms4_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS_onms4_hsync_meas_result_reg);
			onms_onms4_hsync_meas_result_reg.on4_hmax_delta_total = 0;	//write clear
			IoReg_Write32(ONMS_onms4_hsync_meas_result_reg, onms_onms4_hsync_meas_result_reg.regValue);

			onms_onms4_status_reg.regValue = 0;
			onms_onms4_status_reg.on4_hs_meas_over_range = 1;//write clean bit 21
			IoReg_Write32(ONMS_onms4_status_reg, onms_onms4_status_reg.regValue);
		}

		//decide wd and int setting
		if(src == VSC_INPUTSRC_HDMI)
		{
			online_wd_int_setting = get_hdmi_online_wd_int_setting();
		}
		else if(src == VSC_INPUTSRC_DP)
		{
			if(vbe_disp_get_VRR_timingMode_flag())
			{
				online_wd_int_setting = DP_VRR_ONLINE_ENABLE_MASK;
			}
			else if(vbe_disp_get_freesync_mode_flag())
			{
				online_wd_int_setting = DP_FREESYNC_ONLINE_ENABLE_MASK;
			}
			else
			{
				online_wd_int_setting = DP_NORMAL_ONLINE_ENABLE_MASK;
			}
		}
	}

	if(_ENABLE == wdgenable)
	{
		if(src != VSC_INPUTSRC_HDMI)
		{
			onms_onms4_watchdog_en_reg.regValue = 0;
			onms_onms4_watchdog_en_reg.regValue = online_wd_int_setting;
			onms_onms4_watchdog_en_reg.on4_wd_to_main = _TRUE;	   //Watch Dog select to Main Display
			IoReg_Write32(ONMS_onms4_watchdog_en_reg, onms_onms4_watchdog_en_reg.regValue);
			down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
			ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);

			if (((src == VSC_INPUTSRC_AVD) || (src == VSC_INPUTSRC_DP))&&(!Get_tv006_wb_pattern())) {
				ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _ENABLE;//Enable force BG
			} else {
				ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _DISABLE;//Disable forced bg for customer//_ENABLE;	//Main Watch Dog Enable (Force-to-Background)
			}

			ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = _ENABLE;	//FrameSync Main Watch Dog Enable (FreeRun)
			IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
			up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
			//FrameSync Watch Dog Enable (FreeRun)
			ppoverlay_display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
			//fix me later. Ben_Wang add cause data framesync always triiger wdg to free run.
			ppoverlay_display_timing_ctrl1_reg.wde_to_free_run = _ENABLE;
			IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, ppoverlay_display_timing_ctrl1_reg.regValue);
		}
	}
	else// if(_DISABLE == enable)
	{
		onms_onms4_watchdog_en_reg.regValue = 0;
		onms_onms4_watchdog_en_reg.on4_wd_to_main = _FALSE;		//Watch Dog select to Main Display
		IoReg_Write32(ONMS_onms4_watchdog_en_reg, onms_onms4_watchdog_en_reg.regValue);
		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);
		ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _DISABLE;	//Main Watch Dog Enable (Force-to-Background)
		ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = _DISABLE;	//FrameSync Main Watch Dog Enable (FreeRun)
		IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
		up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	}

	if(_ENABLE == interruptenable)
	{
		//onms_onms1_interrupt_en_reg.regValue = IoReg_Read32(ONMS_onms1_interrupt_en_reg);
		onms_onms4_interrupt_en_reg.regValue = 0;
		onms_onms4_interrupt_en_reg.regValue = online_wd_int_setting;
		if (src == VSC_INPUTSRC_HDMI) {
			{// Only for HDMI source, check particular device or debugging command
				unsigned int original_value = onms_onms4_interrupt_en_reg.regValue;
				onms_onms4_interrupt_en_reg.regValue = get_result_with_hdmi_mask(original_value);
				if(onms_onms4_interrupt_en_reg.regValue != original_value)
				{
					rtd_pr_onms_info("[%s][HDMI] Update onms int from %x to %x\n", __FUNCTION__, original_value, onms_onms4_interrupt_en_reg.regValue);
				}
			}
		}
		onms_onms4_interrupt_en_reg.on4_ie_to_main = _TRUE;		//Watch Dog select to Main Display
		IoReg_Write32(ONMS_onms4_interrupt_en_reg, onms_onms4_interrupt_en_reg.regValue);

		if(((src == VSC_INPUTSRC_HDMI) && (get_scaler_qms_mode_flag() == FALSE)) || (src == VSC_INPUTSRC_DP))
			drvif_mode_enable_hsync_period_timeout_wdg_isr(VGIP_PATH_I3);

		vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
		vgip_int_ctl_reg.onms4_int_ie = 1;//Enable onlinemeasure interrupt
		IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);
	}
	else
	{
		//onms_onms1_interrupt_en_reg.regValue = IoReg_Read32(ONMS_onms1_interrupt_en_reg);
		onms_onms4_interrupt_en_reg.regValue = 0;
		onms_onms4_interrupt_en_reg.on4_ie_to_main = _FALSE;		//Watch Dog select to Main Display
		IoReg_Write32(ONMS_onms4_interrupt_en_reg, onms_onms4_interrupt_en_reg.regValue);
		vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
		vgip_int_ctl_reg.onms4_int_ie = 0;//Disable onlinemeasure interrupt
		IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);
	}
}

#ifndef BUILD_QUICK_SHOW

#define _CLEAR_ERROR_STATUS	1
void drvif_mode_disableonlinemeasure(unsigned char channel)
{
	onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;
	// onms_onms1_demode_ctrl_RBUS onms_onms1_demode_ctrl_reg;
	onms_onms1_ctrl_4_RBUS onms_onms1_ctrl_4_reg;

#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	onms_onms2_ctrl_RBUS onms_onms2_ctrl_reg;
	// onms_onms2_demode_ctrl_RBUS onms_onms2_demode_ctrl_reg;
#endif

#ifndef CONFIG_DUAL_CHANNEL
	channel = _CHANNEL1;
#endif
	rtd_pr_onms_info("#####Disable onlinemeasure channel:%d######\r\n",channel);

	if(channel == _CHANNEL1)
	{
		onms_onms1_ctrl_reg.regValue =  IoReg_Read32(ONMS_onms1_ctrl_reg);
		onms_onms1_ctrl_reg.on1_start_ms = 0;
		onms_onms1_ctrl_reg.on1_ms_conti = 0;
		onms_onms1_ctrl_reg.on1_continuous_popup = 0;
		IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);

		// onms_onms1_demode_ctrl_reg.regValue =  IoReg_Read32(ONMS_Onms1_Demode_ctrl_reg);
		// onms_onms1_demode_ctrl_reg.on1_demode_en = 0;
		// IoReg_Write32(ONMS_Onms1_Demode_ctrl_reg, onms_onms1_demode_ctrl_reg.regValue);

		/* hs_per_timeout */
		onms_onms1_ctrl_4_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_4_reg);
		onms_onms1_ctrl_4_reg.on1_hs_per_timeout_en = 0;
		IoReg_Write32(ONMS_onms1_ctrl_4_reg, onms_onms1_ctrl_4_reg.regValue);

		drvif_mode_disable_dma_onlinemeasure();//disable dma online measure

	}
#ifdef CONFIG_DUAL_CHANNEL
	else
	{
		onms_onms2_ctrl_reg.regValue = IoReg_Read32(ONMS_onms2_ctrl_reg);
		onms_onms2_ctrl_reg.on2_start_ms = 0;
		onms_onms2_ctrl_reg.on2_ms_conti = 0;
		onms_onms2_ctrl_reg.on2_continuous_popup = 0;
		IoReg_Write32(ONMS_onms2_ctrl_reg, onms_onms2_ctrl_reg.regValue);

		// onms_onms2_demode_ctrl_reg.regValue =  IoReg_Read32(ONMS_Onms2_Demode_ctrl_reg);
		// onms_onms2_demode_ctrl_reg.on2_demode_en = 0;
		// IoReg_Write32(ONMS_Onms2_Demode_ctrl_reg, onms_onms2_demode_ctrl_reg.regValue);
	}
#endif
}
#endif

void drvif_mode_enableonlinemeasure(unsigned char channel, unsigned char online_path)
{
	// start online measure
	unsigned char src;
	unsigned char de_only_mode = 0;//decide source use de only mode or not
	onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;
	onms_onms1_hsync_meas_ctrl_RBUS onms_onms1_hsync_meas_ctrl_reg;
	onms_onms1_hsync_meas_ctrl_2_RBUS onms_onms1_hsync_meas_ctrl_2_reg;
	unsigned short Htotal, Vtotal, Vstart, Vactive;//input h v total & v start & v active
	unsigned int Vfreq;//input v freq.
#ifdef _CLEAR_ERROR_STATUS
	onms_onms1_status_RBUS onms_onms1_status_reg;
#endif //#ifdef _CLEAR_ERROR_STATUS

#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	onms_onms2_ctrl_RBUS onms_onms2_ctrl_reg;
#ifdef _CLEAR_ERROR_STATUS
	onms_onms2_status_RBUS onms_onms2_status_reg;
#endif //#ifdef _CLEAR_ERROR_STATUS
	onms_onms2_hsync_meas_ctrl_RBUS onms_onms2_hsync_meas_ctrl_reg;
	onms_onms2_hsync_meas_ctrl_2_RBUS onms_onms2_hsync_meas_ctrl_2_reg;
#endif
	onms_onms1_hsync_meas_result_RBUS onms_onms1_hsync_meas_result_reg;
	onms_onms2_hsync_meas_result_RBUS onms_onms2_hsync_meas_result_reg;

	// onms_onms1_ctrl_1_RBUS onms_onms1_ctrl_1_reg;
	// onms_onms2_ctrl_1_RBUS onms_onms2_ctrl_1_reg;

	onms_onms1_ctrl_2_RBUS onms_onms1_ctrl_2_reg;
	onms_onms2_ctrl_2_RBUS onms_onms2_ctrl_2_reg;

	onms_onms1_ctrl_3_RBUS onms_onms1_ctrl_3_reg;
	onms_onms1_ctrl_4_RBUS onms_onms1_ctrl_4_reg;
#ifndef CONFIG_DUAL_CHANNEL
	channel = _CHANNEL1;
#endif
	src = get_scaler_connect_source(channel);
	rtd_pr_onms_info("###func %s channel:%d src:%d###\r\n",__FUNCTION__, channel, src);
	Vtotal  = Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_V_LEN);
	Htotal  = Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_H_LEN);
	Vstart  = Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_IPV_ACT_STA_PRE);
	Vactive = Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_IPV_ACT_LEN_PRE);
	Vfreq   = Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_V_FREQ_1000);

	if((src == _SRC_HDMI) || (src == _SRC_MINI_DP) || (src == _SRC_VO))
		de_only_mode = 1;//hdmi, DP and vo use demode

	if(_CHANNEL1 == channel)
	{
		onms_onms1_demode_ctrl_RBUS onms_onms1_demode_ctrl_reg;
		onms_onms1_demode_ctrl_reg.regValue = IoReg_Read32(ONMS_Onms1_Demode_ctrl_reg);
		if(de_only_mode)
		{//enable demode
			// onms_onms1_demode_ctrl_reg.on1_demode_en = 1;
			if(CONFIG_ONLINE_XTAL_FREQ == CONFIG_ONLINE_OFFLINE_XTAL_FREQ_27M)
				onms_onms1_demode_ctrl_reg.on1_xtal_sel = 0;//27M
			else
				onms_onms1_demode_ctrl_reg.on1_xtal_sel = 1;//24M
			onms_onms1_demode_ctrl_reg.on1_demode_vtotal_delta = 0;
		}
		else
		{
			// onms_onms1_demode_ctrl_reg.on1_demode_en = 0;
			if(CONFIG_ONLINE_XTAL_FREQ == CONFIG_ONLINE_OFFLINE_XTAL_FREQ_27M)
				onms_onms1_demode_ctrl_reg.on1_xtal_sel = 0;//27M
			else
				onms_onms1_demode_ctrl_reg.on1_xtal_sel = 1;//24M
			onms_onms1_demode_ctrl_reg.on1_demode_vtotal_delta = 0;
		}
		IoReg_Write32(ONMS_Onms1_Demode_ctrl_reg, onms_onms1_demode_ctrl_reg.regValue);

		onms_onms1_hsync_meas_ctrl_reg.regValue = 0;
		IoReg_Write32(ONMS_onms1_hsync_meas_ctrl_reg, onms_onms1_hsync_meas_ctrl_reg.regValue);
		onms_onms1_hsync_meas_ctrl_2_reg.regValue = 0;
		IoReg_Write32(ONMS_onms1_hsync_meas_ctrl_2_reg, onms_onms1_hsync_meas_ctrl_2_reg.regValue);

		// set over range threshold (VSYNC_MAX_DELTA = 2-HSync, HSYNC_MAX_DELTA = 4-clock)
		onms_onms1_ctrl_reg.regValue = 0;//reset value//IoReg_Read32(ONMS_onms1_ctrl_reg);
		onms_onms1_ctrl_reg.on1_blk_2frame_en = 1;
		// onms_onms1_ctrl_1_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_1_reg);
		// onms_onms1_ctrl_1_reg.on1_vtotal_uf_th = 0;//reset vtotal under flow setting

		//V counter overflow judgment for ONMS	   //H counter overflow judgment for ONMS
		//00: 2048								   //00: 4096
		//01: 4096								   //01: 8192
		//10: 8192								   //10: 16384
		//11: 16383 							   //11: 32768

		onms_onms1_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_2_reg);
		if(Vtotal && Vfreq)
		{//3 line
			onms_onms1_ctrl_2_reg.on1_hgclk_gate_th = (unsigned int)( 81000000000UL / (unsigned long)Vtotal / (unsigned long)Vfreq );
		}
		else
		{
			onms_onms1_ctrl_2_reg.on1_hgclk_gate_th = 12900;//24hz 480i
		}
		IoReg_Write32(ONMS_onms1_ctrl_2_reg, onms_onms1_ctrl_2_reg.regValue);

		if(de_only_mode)
		{//demode
			onms_onms1_demode_vs_uf_RBUS onms_onms1_demode_vs_uf_reg;
			onms_onms1_demode_vs_of_RBUS onms_onms1_demode_vs_of_reg;
			onms_onms1_demode_vs_uf_reg.regValue = IoReg_Read32(ONMS_Onms1_Demode_Vs_UF_reg);
			onms_onms1_demode_vs_of_reg.regValue = IoReg_Read32(ONMS_Onms1_Demode_Vs_OF_reg);
			if(get_vsc_run_adaptive_stream(SLR_MAIN_DISPLAY)) // 4k2k
			{//need to check the fix v total value
				onms_onms1_demode_vs_uf_reg.on1_de_vsper_uf_th = 0;
				onms_onms1_demode_vs_of_reg.on1_de_vsper_of_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)11)/240;//29700000 = 27000000 * 1.1 //HD SD suggest
				onms_onms1_ctrl_reg.on1_htotal_of_th = 1;
			}
			else
			{
				if((src == _SRC_HDMI) || (src == _SRC_MINI_DP))
				{//hdmi
#ifdef HDMI_VRR_ONLINEMEASURE_SUPPORT
					if(is_vrr_or_freesync_mode() || get_scaler_qms_mode_flag())//vrr case input is hdmi 2.1 need HDMI provide the api
					{//qms case similar with vrr
						onms_onms1_ctrl_reg.on1_hd21_ms_en = 1;//hdmi2.1
						if(Vfreq)
							onms_onms1_demode_vs_uf_reg.on1_de_vsper_uf_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 90 / Vfreq * 10;	//120 hz max 24300000 = 27000000*0.9 //HD SD suggest
						else
							onms_onms1_demode_vs_uf_reg.on1_de_vsper_uf_th = 0;
						onms_onms1_demode_vs_of_reg.on1_de_vsper_of_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)11)/240;//24 hz minimax //29700000 = 27000000 * 1.1 //HD SD suggest
					}
					else
#endif
					{
						//V counter overflow judgment for ONMS     //H counter overflow judgment for ONMS
						//00: 2048                                 //00: 4096
						//01: 4096                                 //01: 8192
						//10: 8192                                 //10: 16384
						//11: 16383                                //11: 32768
						if(Vfreq)
						{
							onms_onms1_demode_vs_uf_reg.on1_de_vsper_uf_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 90 / Vfreq * 10;//120 hz max 24300000 = 27000000*0.9 //HD SD suggest
							onms_onms1_demode_vs_of_reg.on1_de_vsper_of_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / Vfreq * 10;//29700000 = 27000000 * 1.1 //HD SD suggest
						}
						else
						{
							onms_onms1_demode_vs_uf_reg.on1_de_vsper_uf_th = 0;
							onms_onms1_demode_vs_of_reg.on1_de_vsper_of_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)11)/600;
						}
					}
				}
				else
				{//vdec
					onms_onms1_demode_vs_uf_reg.on1_de_vsper_uf_th = 0;
					if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_JPEG)
					{
						if(Vfreq)
						{
							onms_onms1_demode_vs_of_reg.on1_de_vsper_of_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / Vfreq * 10;
						}
						else
						{
							onms_onms1_demode_vs_of_reg.on1_de_vsper_of_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)11)/600;
						}
					}
					else
					{
						//for film mode and adaptive stream case, so set 24hz as minumum framerate
						onms_onms1_demode_vs_of_reg.on1_de_vsper_of_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)11)/240;
					}
				}
			}
			IoReg_Write32(ONMS_Onms1_Demode_Vs_UF_reg, onms_onms1_demode_vs_uf_reg.regValue);
			IoReg_Write32(ONMS_Onms1_Demode_Vs_OF_reg, onms_onms1_demode_vs_of_reg.regValue);
		}

		if(Htotal <= 4096)
			onms_onms1_ctrl_reg.on1_htotal_of_th = 0;
		else if(Htotal <= 8192)
			onms_onms1_ctrl_reg.on1_htotal_of_th = 1;
		else if(Htotal <= 16384)
			onms_onms1_ctrl_reg.on1_htotal_of_th = 2;
		else
			onms_onms1_ctrl_reg.on1_htotal_of_th = 3;

		if(online_path == VGIP_PATH_I1){
			onms_onms1_ctrl_reg.on1_in_sel = 0;
			onms_onms1_ctrl_reg.on1_ms_src_sel = 0;
		} else { // VGIP_PATH_I5
			onms_onms1_ctrl_reg.on1_in_sel = 1;
			onms_onms1_ctrl_reg.on1_ms_src_sel = 0;
		}

		onms_onms1_ctrl_reg.on1_meas_to_sel= _ON_MEAS_TO_SEL_76MS;
		IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);

		onms_onms1_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_2_reg);
		onms_onms1_ctrl_3_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_3_reg);
		onms_onms1_ctrl_4_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_4_reg);
		if((src == _SRC_HDMI) || (src == _SRC_MINI_DP)) {
			/* vs_timeout */
			if (Vfreq != 0) {
				unsigned int framerate = (is_vrr_or_freesync_mode() || get_scaler_qms_mode_flag()) ? VRR_MIN_FRAMERATE : Vfreq;
				unsigned int threshold = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / framerate * 10 / 2048;

				onms_onms1_ctrl_2_reg.on1_vs_timeout_th    = threshold;
				onms_onms1_ctrl_2_reg.on1_vs_timeout_th_en = (threshold > 0) ? 1 : 0;
			}

			/* vs_bporch_timeout */
			if (Vfreq != 0) {
				onms_onms1_ctrl_3_reg.on1_vs_bporch_timeout_th = (Vtotal != 0) ? ( (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / Vfreq * 10 * Vstart / Vtotal ) : 0;
			}

			/* hs_per_timeout */
			if (Vfreq != 0) {
				unsigned int threshold = ( Vtotal > 0 ) ? ( (unsigned int)CONFIG_ONLINE_XTAL_FREQ * CONFIG_HS_PER_TIMEOUT_TH_TOLERANCE / Vfreq * 100 / Vtotal ) : 0;

				onms_onms1_ctrl_4_reg.on1_vact_num = Vactive;
				onms_onms1_ctrl_4_reg.on1_hs_per_timeout_th = threshold;
				onms_onms1_ctrl_4_reg.on1_hs_per_timeout_en = (Vactive > 0 && threshold > 0) ? 1 : 0;
			}
		} else {
			onms_onms1_ctrl_2_reg.on1_vs_timeout_th_en = 0;
			onms_onms1_ctrl_2_reg.on1_vs_timeout_th    = 0;

			onms_onms1_ctrl_3_reg.on1_vs_bporch_timeout_th = 0;

			onms_onms1_ctrl_4_reg.on1_hs_per_timeout_en = 0;
			onms_onms1_ctrl_4_reg.on1_vact_num = 0;
			onms_onms1_ctrl_4_reg.on1_hs_per_timeout_th = 0;
		}

		IoReg_Write32(ONMS_onms1_ctrl_2_reg, onms_onms1_ctrl_2_reg.regValue);
		IoReg_Write32(ONMS_onms1_ctrl_3_reg, onms_onms1_ctrl_3_reg.regValue);
		IoReg_Write32(ONMS_onms1_ctrl_4_reg, onms_onms1_ctrl_4_reg.regValue);

		// enable online measure
		onms_onms1_ctrl_reg.on1_sycms_clk = 1;
		onms_onms1_ctrl_reg.on1_htotal_max_delta = 8;
		onms_onms1_ctrl_reg.on1_ms_conti = 1;
		onms_onms1_ctrl_reg.on1_start_ms = 0;
		if(de_only_mode)
			onms_onms1_ctrl_reg.on1_continuous_popup = 1;//enable continue pop for demode
		else
			onms_onms1_ctrl_reg.on1_continuous_popup = 0;//disable continue pop for demode
		IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);
		onms_onms1_ctrl_reg.on1_start_ms = 1;
		IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);

#ifdef _CLEAR_ERROR_STATUS
		//write clear error status
		onms_onms1_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS_onms1_hsync_meas_result_reg);
		onms_onms1_hsync_meas_result_reg.on1_hmax_delta_total = 0;	//write clear
		IoReg_Write32(ONMS_onms1_hsync_meas_result_reg, onms_onms1_hsync_meas_result_reg.regValue);

		//write clear status, if can not clear, need add delay
		onms_onms1_status_reg.regValue = IoReg_Read32(ONMS_onms1_status_reg);
		onms_onms1_status_reg.on1_vs_per_to_long = 1;	//Time-Out (76ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit20
		onms_onms1_status_reg.on1_vs_pol_out = 1; // VSYNC Polarity Indicator
		// onms_onms1_status_reg.on1_hs_pol_out = 1; //HSYNC Polarity Indicator
		onms_onms1_status_reg.on1_vs_per_over_range = 1; //VSYNC_OVER_RANGE//Bit15
		onms_onms1_status_reg.on1_hs_per_over_range = 1; //HSYNC_OVER_RANGE//Bit14
		onms_onms1_status_reg.on1_vs_per_timeout = 1; //Time-Out (40ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit13
		onms_onms1_status_reg.on1_vs_high_timeout = 1;	//Time-Out bit of Input VSYNC High Pulse Measurement//Bit12
		onms_onms1_status_reg.on1_vs_per_overflow = 1;	//Over-flow bit of Input HSYNC Period Measurement//Bit11
		onms_onms1_status_reg.on1_hs_per_overflow = 1; //Over-flow bit of Input VSYNC Period Measurement//Bit10
		onms_onms1_status_reg.on1_vs_pol_chg = 1; //Input VSYNC Polarity change//Bit9
		// onms_onms1_status_reg.on1_hs_pol_chg = 1; //Input HSYNC Polarity change//Bit8
		onms_onms1_status_reg.on1_vtotalde_chg = 1; // Total active line number changes
		// onms_onms1_status_reg.on1_vtotal_chg = 1; // Total line number changes
		onms_onms1_status_reg.on1_hact_chg = 1; // Total active pixel number of first 8 active lines changes
		onms_onms1_status_reg.on1_htotal_chg = 1; //Total average pixel number of first 8 lines changes
		onms_onms1_status_reg.on1_hs_active_pixel_var = 1;//Bit3
		onms_onms1_status_reg.on1_vs_per_underflow = 1;	//Under-flow bit of Input HSYNC Period Measurement//Bit2
		onms_onms1_status_reg.on1_hs_meas_over_range = 1;//Bit21 Hs period measure over range
		onms_onms1_status_reg.on1_hgclk_gate_over_range = 1;//Bit22 Measure source clk of gating period use xclk for ONMS1  Threshold setting at 124C
		IoReg_Write32(ONMS_onms1_status_reg, onms_onms1_status_reg.regValue);
#endif //#ifdef _CLEAR_ERROR_STATUS
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else// if(_CHANNEL2 == info->channel)
	{
		onms_onms2_demode_ctrl_RBUS onms_onms2_demode_ctrl_reg;
		onms_onms2_demode_ctrl_reg.regValue = IoReg_Read32(ONMS_Onms2_Demode_ctrl_reg);
		if(de_only_mode)
		{//enable demode
			// onms_onms2_demode_ctrl_reg.on2_demode_en = 1;
			if(CONFIG_ONLINE_XTAL_FREQ == CONFIG_ONLINE_OFFLINE_XTAL_FREQ_27M)
				onms_onms2_demode_ctrl_reg.on2_xtal_sel = 0;//27M
			else
				onms_onms2_demode_ctrl_reg.on2_xtal_sel = 1;//24M
			onms_onms2_demode_ctrl_reg.on2_demode_vtotal_delta = 0;
		}
		else
		{
			// onms_onms2_demode_ctrl_reg.on2_demode_en = 0;
			if(CONFIG_ONLINE_XTAL_FREQ == CONFIG_ONLINE_OFFLINE_XTAL_FREQ_27M)
				onms_onms2_demode_ctrl_reg.on2_xtal_sel = 0;//27M
			else
				onms_onms2_demode_ctrl_reg.on2_xtal_sel = 1;//24M
			onms_onms2_demode_ctrl_reg.on2_demode_vtotal_delta = 0;
		}
		IoReg_Write32(ONMS_Onms2_Demode_ctrl_reg, onms_onms2_demode_ctrl_reg.regValue);

		onms_onms2_hsync_meas_ctrl_reg.regValue = 0;
		IoReg_Write32(ONMS_onms2_hsync_meas_ctrl_reg, onms_onms2_hsync_meas_ctrl_reg.regValue);
		onms_onms2_hsync_meas_ctrl_2_reg.regValue = 0;
		IoReg_Write32(ONMS_onms2_hsync_meas_ctrl_2_reg, onms_onms2_hsync_meas_ctrl_2_reg.regValue);

		// set over range threshold (VSYNC_MAX_DELTA = 2-HSync, HSYNC_MAX_DELTA = 4-clock)
		onms_onms2_ctrl_reg.regValue = 0;//reset setting//IoReg_Read32(ONMS_onms2_ctrl_reg);
		onms_onms2_ctrl_reg.on2_blk_2frame_en = 1;
		// onms_onms2_ctrl_1_reg.regValue = IoReg_Read32(ONMS_onms2_ctrl_1_reg);
		// onms_onms2_ctrl_1_reg.on2_vtotal_uf_th = 0;//reset vtotal under flow setting

		//V counter overflow judgment for ONMS	   //H counter overflow judgment for ONMS
		//00: 2048								   //00: 4096
		//01: 4096								   //01: 8192
		//10: 8192								   //10: 16384
		//11: 16383 							   //11: 32768


		onms_onms2_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms2_ctrl_2_reg);
		if(Vtotal && Vfreq)
		{//3 line
			onms_onms2_ctrl_2_reg.on2_hgclk_gate_th = (unsigned int)810000000 / (unsigned int)Vtotal * 100 / Vfreq;
		}
		else
		{
			onms_onms2_ctrl_2_reg.on2_hgclk_gate_th = 12900;//24hz 480i
		}
		IoReg_Write32(ONMS_onms2_ctrl_2_reg, onms_onms2_ctrl_2_reg.regValue);

		if(de_only_mode)
		{//demode
			onms_onms2_demode_vs_uf_RBUS onms_onms2_demode_vs_uf_reg;
			onms_onms2_demode_vs_of_RBUS onms_onms2_demode_vs_of_reg;
			onms_onms2_demode_vs_uf_reg.regValue = IoReg_Read32(ONMS_Onms2_Demode_Vs_UF_reg);
			onms_onms2_demode_vs_of_reg.regValue = IoReg_Read32(ONMS_Onms2_Demode_Vs_OF_reg);
			if(src == _SRC_HDMI)
			{//hdmi
#ifdef HDMI_VRR_ONLINEMEASURE_SUPPORT
				if(vbe_disp_get_VRR_timingMode_flag())//vrr case input is hdmi 2.1 need HDMI provide the api
				{
					onms_onms2_ctrl_reg.on2_hd21_ms_en = 1;//hdmi2.1
					onms_onms2_demode_vs_uf_reg.on2_de_vsper_uf_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)9)/600;//60 hz max 24300000 = 27000000*0.9 //HD SD suggest
					onms_onms2_demode_vs_of_reg.on2_de_vsper_of_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)11)/240;//24 hz minimax //29700000 = 27000000 * 1.1 //HD SD suggest
				}
				else
#endif
				{
					//V counter overflow judgment for ONMS     //H counter overflow judgment for ONMS
					//00: 2048                                 //00: 4096
					//01: 4096                                 //01: 8192
					//10: 8192                                 //10: 16384
					//11: 16383                                //11: 32768
					onms_onms2_demode_vs_uf_reg.on2_de_vsper_uf_th = 0;
					if(Vfreq == 0)
						onms_onms2_demode_vs_of_reg.on2_de_vsper_of_th = 0;
					else
						onms_onms2_demode_vs_of_reg.on2_de_vsper_of_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / Vfreq * 10;//29700000 = 27000000 * 1.1 //HD SD suggest
				}
			}
			else
			{//vdec
					onms_onms2_demode_vs_uf_reg.on2_de_vsper_uf_th = 0;
					if(Vfreq == 0)
						onms_onms2_demode_vs_of_reg.on2_de_vsper_of_th = 0;
					else
						onms_onms2_demode_vs_of_reg.on2_de_vsper_of_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / Vfreq * 10;//29700000 = 27000000 * 1.1 //HD SD suggest
			}

			IoReg_Write32(ONMS_Onms2_Demode_Vs_UF_reg, onms_onms2_demode_vs_uf_reg.regValue);
			IoReg_Write32(ONMS_Onms2_Demode_Vs_OF_reg, onms_onms2_demode_vs_of_reg.regValue);
		}

		if(Htotal <= 4096)
			onms_onms2_ctrl_reg.on2_htotal_of_th = 0;
		else if(Htotal <= 8192)
			onms_onms2_ctrl_reg.on2_htotal_of_th = 1;
		else if(Htotal <= 16384)
			onms_onms2_ctrl_reg.on2_htotal_of_th = 2;
		else
			onms_onms2_ctrl_reg.on2_htotal_of_th = 3;

		if(online_path == VGIP_PATH_I2){
			onms_onms2_ctrl_reg.on2_in_sel = 0;
			onms_onms2_ctrl_reg.on2_ms_src_sel = 0;
		} else { // VGIP_PATH_I6
			onms_onms2_ctrl_reg.on2_in_sel = 1;
			onms_onms2_ctrl_reg.on2_ms_src_sel = 0;
		}

		onms_onms2_ctrl_reg.on2_meas_to_sel = _ON_MEAS_TO_SEL_76MS;
		IoReg_Write32(ONMS_onms2_ctrl_reg, onms_onms2_ctrl_reg.regValue);

		// enable online measure
		onms_onms2_ctrl_reg.on2_sycms_clk = 1;
		onms_onms2_ctrl_reg.on2_htotal_max_delta = 8;
		onms_onms2_ctrl_reg.on2_ms_conti = 1;
		onms_onms2_ctrl_reg.on2_start_ms = 0;
		if(de_only_mode)
			onms_onms2_ctrl_reg.on2_continuous_popup = 1;//enable continue pop for demode
		else
			onms_onms2_ctrl_reg.on2_continuous_popup = 0;//disable continue pop for demode
		IoReg_Write32(ONMS_onms2_ctrl_reg, onms_onms2_ctrl_reg.regValue);
		onms_onms2_ctrl_reg.on2_start_ms = 1;
		IoReg_Write32(ONMS_onms2_ctrl_reg, onms_onms2_ctrl_reg.regValue);

#ifdef _CLEAR_ERROR_STATUS
		//write clear error status
		onms_onms2_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS_onms2_hsync_meas_result_reg);
		onms_onms2_hsync_meas_result_reg.on2_hmax_delta_total = 0;	//write clear
		IoReg_Write32(ONMS_onms2_hsync_meas_result_reg, onms_onms2_hsync_meas_result_reg.regValue);

		onms_onms2_status_reg.regValue = IoReg_Read32(ONMS_onms2_status_reg);
		onms_onms2_status_reg.on2_vs_per_to_long = 1;	//Time-Out (76ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit20
		onms_onms2_status_reg.on2_vs_pol_out = 1; // VSYNC Polarity Indicator
		// onms_onms2_status_reg.on2_hs_pol_out = 1; // HSYNC Polarity Indicator
		onms_onms2_status_reg.on2_vs_per_over_range = 1; //VSYNC_OVER_RANGE//Bit15
		onms_onms2_status_reg.on2_hs_per_over_range = 1; //HSYNC_OVER_RANGE//Bit14
		onms_onms2_status_reg.on2_vs_per_timeout = 1; //Time-Out (40ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit13
		onms_onms2_status_reg.on2_vs_high_timeout = 1;	//Time-Out bit of Input VSYNC High Pulse Measurement//Bit12
		onms_onms2_status_reg.on2_vs_per_overflow = 1;	//Over-flow bit of Input HSYNC Period Measurement//Bit11
		onms_onms2_status_reg.on2_hs_per_overflow = 1; //Over-flow bit of Input VSYNC Period Measurement//Bit10
		onms_onms2_status_reg.on2_vs_pol_chg = 1; //Input VSYNC Polarity change//Bit9
		// onms_onms2_status_reg.on2_hs_pol_chg = 1; //Input HSYNC Polarity change//Bit8
		// onms_onms2_status_reg.on2_vtotal_chg = 1; //Total line number changes
		onms_onms2_status_reg.on2_vtotalde_chg = 1; //Total active line number changes
		onms_onms2_status_reg.on2_hact_chg = 1; //Total active pixel number of first 8 active lines changes
		onms_onms2_status_reg.on2_htotal_chg = 1; //Total average pixel number of first 8 lines changes
		onms_onms2_status_reg.on2_hs_active_pixel_var = 1;//Bit3
		onms_onms2_status_reg.on2_hs_meas_over_range = 1;//Bit21 Hs period measure over range
		onms_onms2_status_reg.on2_hgclk_gate_over_range = 1;//Bit22 Measure source clk of gating period use xclk for ONMS1  Threshold setting at 134C
		IoReg_Write32(ONMS_onms2_status_reg, onms_onms2_status_reg.regValue);
#endif //#ifdef _CLEAR_ERROR_STATUS
	}
#endif //#ifdef CONFIG_DUAL_CHANNEL
}

#if 1//ndef BUILD_QUICK_SHOW
void drvif_mode_disable_dma_onlinemeasure(void)
{
	onms_onms3_ctrl_RBUS onms_onms3_ctrl_reg;
	// onms_onms3_demode_ctrl_RBUS onms_onms3_demode_ctrl_reg;
	onms_onms3_ctrl_4_RBUS onms_onms3_ctrl_4_reg;

	rtd_pr_onms_info("#####Disable dma onlinemeasure######\r\n");
	onms_onms3_ctrl_reg.regValue =  IoReg_Read32(ONMS_onms3_ctrl_reg);
	onms_onms3_ctrl_reg.on3_start_ms = 0;
	onms_onms3_ctrl_reg.on3_ms_conti = 0;
	onms_onms3_ctrl_reg.on3_continuous_popup = 0;
	IoReg_Write32(ONMS_onms3_ctrl_reg, onms_onms3_ctrl_reg.regValue);

	// onms_onms3_demode_ctrl_reg.regValue =  IoReg_Read32(ONMS_Onms3_Demode_ctrl_reg);
	// onms_onms3_demode_ctrl_reg.on3_demode_en = 0;
	// IoReg_Write32(ONMS_Onms3_Demode_ctrl_reg, onms_onms3_demode_ctrl_reg.regValue);

	/* hs_per_timeout */
	onms_onms3_ctrl_4_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_4_reg);
	onms_onms3_ctrl_4_reg.on3_hs_per_timeout_en = 0;
	IoReg_Write32(ONMS_onms3_ctrl_4_reg, onms_onms3_ctrl_4_reg.regValue);
}

void drvif_mode_enable_dma_onlinemeasure(void)
{
	// start online measure
	unsigned char de_only_mode = 0;//decide source use de only mode or not
	onms_onms3_ctrl_RBUS onms_onms3_ctrl_reg;
	onms_onms3_ctrl_2_RBUS onms_onms3_ctrl_2_reg;
	onms_onms3_ctrl_3_RBUS onms_onms3_ctrl_3_reg;
	onms_onms3_ctrl_4_RBUS onms_onms3_ctrl_4_reg;
	onms_onms3_hsync_meas_ctrl_RBUS onms_onms3_hsync_meas_ctrl_reg;
	onms_onms3_hsync_meas_ctrl_2_RBUS onms_onms3_hsync_meas_ctrl_2_reg;
	VSC_INPUT_TYPE_T src;

	onms_onms3_demode_ctrl_RBUS onms_onms3_demode_ctrl_reg;

#ifdef _CLEAR_ERROR_STATUS
	onms_onms3_status_RBUS onms_onms3_status_reg;
#endif //#ifdef _CLEAR_ERROR_STATUS
	onms_onms3_hsync_meas_result_RBUS onms_onms3_hsync_meas_result_reg;
	// onms_onms3_ctrl_1_RBUS onms_onms3_ctrl_1_reg;
	unsigned short Vtotal, Htotal, Vstart, Vactive;//input h v total & v start & v active
	unsigned int Vfreq;//input v freq

	Vtotal  = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN);
	Htotal  = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN);
	Vstart  = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_STA_PRE);
	Vactive = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE);
	Vfreq   = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000);

	src = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);
	rtd_pr_onms_info("#####Enable dma onlinemeasure src:%d######\r\n", src);

	if((src == VSC_INPUTSRC_HDMI) || (src == VSC_INPUTSRC_DP))
		de_only_mode = 1;

	onms_onms3_demode_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_Demode_ctrl_reg);
	if(de_only_mode)
	{//enable demode
		// onms_onms3_demode_ctrl_reg.on3_demode_en = 1;
		if(CONFIG_ONLINE_XTAL_FREQ == CONFIG_ONLINE_OFFLINE_XTAL_FREQ_27M)
			onms_onms3_demode_ctrl_reg.on3_xtal_sel = 0;
		else
			onms_onms3_demode_ctrl_reg.on3_xtal_sel = 1;
		onms_onms3_demode_ctrl_reg.on3_demode_vtotal_delta = 0;
	}
	else
	{
		// onms_onms3_demode_ctrl_reg.on3_demode_en = 0;
		if(CONFIG_ONLINE_XTAL_FREQ == CONFIG_ONLINE_OFFLINE_XTAL_FREQ_27M)
			onms_onms3_demode_ctrl_reg.on3_xtal_sel = 0;
		else
			onms_onms3_demode_ctrl_reg.on3_xtal_sel = 1;
		onms_onms3_demode_ctrl_reg.on3_demode_vtotal_delta = 0;
	}
	IoReg_Write32(ONMS_onms3_Demode_ctrl_reg, onms_onms3_demode_ctrl_reg.regValue);

	onms_onms3_hsync_meas_ctrl_reg.regValue = 0;
	IoReg_Write32(ONMS_onms3_hsync_meas_ctrl_reg, onms_onms3_hsync_meas_ctrl_reg.regValue);
	onms_onms3_hsync_meas_ctrl_2_reg.regValue = 0;
	IoReg_Write32(ONMS_onms3_hsync_meas_ctrl_2_reg, onms_onms3_hsync_meas_ctrl_2_reg.regValue);

	onms_onms3_ctrl_reg.regValue = 0;//reset to 0//IoReg_Read32(ONMS_onms3_ctrl_reg);
	onms_onms3_ctrl_reg.on3_blk_2frame_en = 1;//mask the 3 frame first.
	if(Htotal <= 4096)
		onms_onms3_ctrl_reg.on3_htotal_of_th = 0;
	else if(Htotal <= 8192)
		onms_onms3_ctrl_reg.on3_htotal_of_th = 1;
	else if(Htotal <= 16384)
		onms_onms3_ctrl_reg.on3_htotal_of_th = 2;
	else
		onms_onms3_ctrl_reg.on3_htotal_of_th = 3;

	onms_onms3_ctrl_reg.on3_meas_to_sel= _ON_MEAS_TO_SEL_76MS;
	IoReg_Write32(ONMS_onms3_ctrl_reg, onms_onms3_ctrl_reg.regValue);

	onms_onms3_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_2_reg);
	if(Vtotal && Vfreq)
	{//3 line
		onms_onms3_ctrl_2_reg.on3_hgclk_gate_th = (unsigned int)( 81000000000UL / (unsigned long)Vtotal / (unsigned long)Vfreq );
	}
	else
	{
		onms_onms3_ctrl_2_reg.on3_hgclk_gate_th = 12900;//24hz 480i
	}
	IoReg_Write32(ONMS_onms3_ctrl_2_reg, onms_onms3_ctrl_2_reg.regValue);

	if((src == VSC_INPUTSRC_HDMI) || (src == VSC_INPUTSRC_DP))
	{//demode
		onms_onms3_demode_vs_uf_RBUS onms_onms3_demode_vs_uf_reg;
		onms_onms3_demode_vs_of_RBUS onms_onms3_demode_vs_of_reg;
		onms_onms3_demode_vs_uf_reg.regValue = IoReg_Read32(ONMS_onms3_Demode_Vs_UF_reg);
		onms_onms3_demode_vs_of_reg.regValue = IoReg_Read32(ONMS_onms3_Demode_Vs_OF_reg);
#ifdef HDMI_VRR_ONLINEMEASURE_SUPPORT
		if(is_vrr_or_freesync_mode() || get_scaler_qms_mode_flag())//vrr case input is hdmi 2.1 need HDMI provide the api
		{//qms case similar with vrr
			rtd_pr_onms_info("[Eric][%s][%d]hdmi_framerate=%d\n", __FUNCTION__, __LINE__, Vfreq);

			if(Vfreq)
				onms_onms3_demode_vs_uf_reg.on3_de_vsper_uf_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 90 / Vfreq * 10;//120 hz max 24300000 = 27000000*0.9 //HD SD suggest
			else
				onms_onms3_demode_vs_uf_reg.on3_de_vsper_uf_th = 0;
			onms_onms3_demode_vs_of_reg.on3_de_vsper_of_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)11)/240;//24 hz minimax //29700000 = 27000000 * 1.1 //HD SD suggest
		}
		else
#endif
		{
			//V counter overflow judgment for ONMS     //H counter overflow judgment for ONMS
			//00: 2048                                 //00: 4096
			//01: 4096                                 //01: 8192
			//10: 8192                                 //10: 16384
			//11: 16383                                //11: 32768
			if(Vfreq)
			{
				onms_onms3_demode_vs_uf_reg.on3_de_vsper_uf_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 98 / Vfreq * 10;//for panel compability
				onms_onms3_demode_vs_of_reg.on3_de_vsper_of_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / Vfreq * 10;//29700000 = 27000000 * 1.1 //HD SD suggest
			}
			else
			{
				onms_onms3_demode_vs_uf_reg.on3_de_vsper_uf_th = 0;
				onms_onms3_demode_vs_of_reg.on3_de_vsper_of_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)11)/600;
			}
		}
		IoReg_Write32(ONMS_onms3_Demode_Vs_UF_reg, onms_onms3_demode_vs_uf_reg.regValue);
		IoReg_Write32(ONMS_onms3_Demode_Vs_OF_reg, onms_onms3_demode_vs_of_reg.regValue);
	}

	onms_onms3_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_2_reg);
	onms_onms3_ctrl_3_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_3_reg);
	onms_onms3_ctrl_4_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_4_reg);
	if((src == VSC_INPUTSRC_HDMI) || (src == VSC_INPUTSRC_DP)) {
		/* vs_timeout */
		if (Vfreq != 0) {
			unsigned int framerate = (is_vrr_or_freesync_mode() || get_scaler_qms_mode_flag()) ? VRR_MIN_FRAMERATE : Vfreq;
			unsigned int threshold = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / framerate * 10 / 2048;

			onms_onms3_ctrl_2_reg.on3_vs_timeout_th    = threshold;
			onms_onms3_ctrl_2_reg.on3_vs_timeout_th_en = (threshold > 0) ? 1 : 0;
		}

		/* vs_bporch_timeout */
		if (Vfreq != 0) {
			onms_onms3_ctrl_3_reg.on3_vs_bporch_timeout_th = (Vtotal != 0) ? ( (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / Vfreq * 10 * Vstart / Vtotal ) : 0;
		}

		/* hs_per_timeout */
		if (Vfreq != 0) {
			unsigned int threshold = ( Vtotal > 0 ) ? ( (unsigned int)CONFIG_ONLINE_XTAL_FREQ * CONFIG_HS_PER_TIMEOUT_TH_TOLERANCE / Vfreq * 100 / Vtotal ) : 0;

			onms_onms3_ctrl_4_reg.on3_vact_num = Vactive;
			onms_onms3_ctrl_4_reg.on3_hs_per_timeout_th = threshold;
			onms_onms3_ctrl_4_reg.on3_hs_per_timeout_en = (Vactive > 0 && threshold > 0) ? 1 : 0;
		}
	} else {
		onms_onms3_ctrl_2_reg.on3_vs_timeout_th_en = 0;
		onms_onms3_ctrl_2_reg.on3_vs_timeout_th    = 0;

		onms_onms3_ctrl_3_reg.on3_vs_bporch_timeout_th = 0;

		onms_onms3_ctrl_4_reg.on3_hs_per_timeout_en = 0;
		onms_onms3_ctrl_4_reg.on3_vact_num = 0;
		onms_onms3_ctrl_4_reg.on3_hs_per_timeout_th = 0;
	}

	IoReg_Write32(ONMS_onms3_ctrl_2_reg, onms_onms3_ctrl_2_reg.regValue);
	IoReg_Write32(ONMS_onms3_ctrl_3_reg, onms_onms3_ctrl_3_reg.regValue);
	IoReg_Write32(ONMS_onms3_ctrl_4_reg, onms_onms3_ctrl_4_reg.regValue);


	// enable online measure
	onms_onms3_ctrl_reg.on3_sycms_clk = 1;
	onms_onms3_ctrl_reg.on3_htotal_max_delta = 8;
	onms_onms3_ctrl_reg.on3_ms_conti = 1;
	onms_onms3_ctrl_reg.on3_start_ms = 0;
	if(de_only_mode)
	{
		onms_onms3_ctrl_reg.on3_continuous_popup = 1;//enable continue pop for demode
		onms_onms3_ctrl_reg.on3_hd21_ms_en = 1;//hdmi2.1
	}
	else
	{
		onms_onms3_ctrl_reg.on3_continuous_popup = 0;//disable continue pop for demode
		onms_onms3_ctrl_reg.on3_hd21_ms_en = 0;//hdmi2.1
	}
	IoReg_Write32(ONMS_onms3_ctrl_reg, onms_onms3_ctrl_reg.regValue);
	onms_onms3_ctrl_reg.on3_start_ms = 1;
	IoReg_Write32(ONMS_onms3_ctrl_reg, onms_onms3_ctrl_reg.regValue);

#ifdef _CLEAR_ERROR_STATUS
		onms_onms3_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS_onms3_hsync_meas_result_reg);
		onms_onms3_hsync_meas_result_reg.on3_hmax_delta_total = 0;	//write clear
		IoReg_Write32(ONMS_onms3_hsync_meas_result_reg, onms_onms3_hsync_meas_result_reg.regValue);

		//write clear status, if can not clear, need add delay
		onms_onms3_status_reg.regValue = IoReg_Read32(ONMS_onms3_status_reg);
		onms_onms3_status_reg.on3_vs_per_to_long = 1;	//Time-Out (76ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit20
		onms_onms3_status_reg.on3_vs_pol_out = 1; // VSYNC Polarity Indicator
		// onms_onms3_status_reg.on3_hs_pol_out = 1; //HSYNC Polarity Indicator
		onms_onms3_status_reg.on3_vs_per_over_range = 1; //VSYNC_OVER_RANGE//Bit15
		onms_onms3_status_reg.on3_hs_per_over_range = 1; //HSYNC_OVER_RANGE//Bit14
		onms_onms3_status_reg.on3_vs_per_timeout = 1; //Time-Out (40ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit13
		onms_onms3_status_reg.on3_vs_high_timeout = 1;	//Time-Out bit of Input VSYNC High Pulse Measurement//Bit12
		onms_onms3_status_reg.on3_vs_per_overflow = 1;	//Over-flow bit of Input HSYNC Period Measurement//Bit11
		onms_onms3_status_reg.on3_hs_per_overflow = 1; //Over-flow bit of Input VSYNC Period Measurement//Bit10
		onms_onms3_status_reg.on3_vs_pol_chg= 1; //Input VSYNC Polarity change//Bit9
		// onms_onms3_status_reg.on3_hs_pol_chg = 1; //Input HSYNC Polarity change//Bit8
		onms_onms3_status_reg.on3_vtotalde_chg = 1; // Total active line number changes
		// onms_onms3_status_reg.on3_vtotal_chg = 1; // Total line number changes
		onms_onms3_status_reg.on3_hact_chg = 1; // Total active pixel number of first 8 active lines changes
		onms_onms3_status_reg.on3_htotal_chg = 1; //Total average pixel number of first 8 lines changes
		onms_onms3_status_reg.on3_hs_active_pixel_var = 1;//Bit3
		onms_onms3_status_reg.on3_vs_per_underflow = 1;	//Under-flow bit of Input HSYNC Period Measurement//Bit2
		onms_onms3_status_reg.on3_hs_meas_over_range = 1;//Bit21 Hs period measure over range
		onms_onms3_status_reg.on3_hgclk_gate_over_range = 1;//BIT22 hgclk gate over range
		IoReg_Write32(ONMS_onms3_status_reg, onms_onms3_status_reg.regValue);
#endif //#ifdef _CLEAR_ERROR_STATUS
}
#endif

void drvif_mode_disable_onms4(void)
{
	onms_onms4_ctrl_RBUS onms_onms4_ctrl_reg;
	// onms_onms4_demode_ctrl_RBUS onms_onms4_demode_ctrl_reg;
	onms_onms4_ctrl_4_RBUS onms_onms4_ctrl_4_reg;

	rtd_pr_onms_info("#####Disable onms4 onlinemeasure######\r\n");
	onms_onms4_ctrl_reg.regValue =  IoReg_Read32(ONMS_onms4_ctrl_reg);
	onms_onms4_ctrl_reg.on4_start_ms = 0;
	onms_onms4_ctrl_reg.on4_ms_conti = 0;
	onms_onms4_ctrl_reg.on4_continuous_popup = 0;
	IoReg_Write32(ONMS_onms4_ctrl_reg, onms_onms4_ctrl_reg.regValue);

	/* hs_per_timeout */
	onms_onms4_ctrl_4_reg.regValue = IoReg_Read32(ONMS_onms4_ctrl_4_reg);
	onms_onms4_ctrl_4_reg.on4_hs_per_timeout_en = 0;
	IoReg_Write32(ONMS_onms4_ctrl_4_reg, onms_onms4_ctrl_4_reg.regValue);
}

void drvif_mode_enable_onms4(void)
{
	// start online measure
	unsigned char de_only_mode = 0;//decide source use de only mode or not
	onms_onms4_ctrl_RBUS onms_onms4_ctrl_reg;
	onms_onms4_ctrl_2_RBUS onms_onms4_ctrl_2_reg;
	onms_onms4_ctrl_3_RBUS onms_onms4_ctrl_3_reg;
	onms_onms4_ctrl_4_RBUS onms_onms4_ctrl_4_reg;
	onms_onms4_hsync_meas_ctrl_RBUS onms_onms4_hsync_meas_ctrl_reg;
	onms_onms4_hsync_meas_ctrl_2_RBUS onms_onms4_hsync_meas_ctrl_2_reg;
	VSC_INPUT_TYPE_T src;

	onms_onms4_demode_ctrl_RBUS onms_onms4_demode_ctrl_reg;

#ifdef _CLEAR_ERROR_STATUS
	onms_onms4_status_RBUS onms_onms4_status_reg;
#endif //#ifdef _CLEAR_ERROR_STATUS
	onms_onms4_hsync_meas_result_RBUS onms_onms4_hsync_meas_result_reg;
	unsigned short Vtotal, Htotal, Vstart, Vactive;//input h v total & v start & v active
	unsigned int Vfreq;//input v freq

	Vtotal  = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN);
	Htotal  = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN);
	Vstart  = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_STA_PRE);
	Vactive = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE);
	Vfreq   = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000);

	src = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);
	rtd_pr_onms_info("#####Enable dma onlinemeasure src:%d######\r\n", src);

	if((src == VSC_INPUTSRC_HDMI) || (src == VSC_INPUTSRC_DP))
		de_only_mode = 1;

	onms_onms4_demode_ctrl_reg.regValue = IoReg_Read32(ONMS_onms4_Demode_ctrl_reg);

	// onms_onms4_demode_ctrl_reg.on4_demode_en = 1;
	if(CONFIG_ONLINE_XTAL_FREQ == CONFIG_ONLINE_OFFLINE_XTAL_FREQ_27M)
		onms_onms4_demode_ctrl_reg.on4_xtal_sel = 0;
	else
		onms_onms4_demode_ctrl_reg.on4_xtal_sel = 1;
	onms_onms4_demode_ctrl_reg.on4_demode_vtotal_delta = 0;

	IoReg_Write32(ONMS_onms4_Demode_ctrl_reg, onms_onms4_demode_ctrl_reg.regValue);

	onms_onms4_hsync_meas_ctrl_reg.regValue = 0;
	IoReg_Write32(ONMS_onms4_hsync_meas_ctrl_reg, onms_onms4_hsync_meas_ctrl_reg.regValue);
	onms_onms4_hsync_meas_ctrl_2_reg.regValue = 0;
	IoReg_Write32(ONMS_onms4_hsync_meas_ctrl_2_reg, onms_onms4_hsync_meas_ctrl_2_reg.regValue);

	onms_onms4_ctrl_reg.regValue = 0;//reset to 0//IoReg_Read32(ONMS_onms4_ctrl_reg);
	onms_onms4_ctrl_reg.on4_blk_2frame_en = 1;//mask the 3 frame first.
	if(Htotal <= 4096)
		onms_onms4_ctrl_reg.on4_htotal_of_th = 0;
	else if(Htotal <= 8192)
		onms_onms4_ctrl_reg.on4_htotal_of_th = 1;
	else if(Htotal <= 16384)
		onms_onms4_ctrl_reg.on4_htotal_of_th = 2;
	else
		onms_onms4_ctrl_reg.on4_htotal_of_th = 3;

	onms_onms4_ctrl_reg.on4_meas_to_sel= _ON_MEAS_TO_SEL_76MS;
	onms_onms4_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms4_ctrl_2_reg);
	if(Vtotal && Vfreq)
	{//3 line
		onms_onms4_ctrl_2_reg.on4_hgclk_gate_th = (unsigned int)( 81000000000UL / (unsigned long)Vtotal / (unsigned long)Vfreq );
	}
	else
	{
		onms_onms4_ctrl_2_reg.on4_hgclk_gate_th = 12900;//24hz 480i
	}
	IoReg_Write32(ONMS_onms4_ctrl_2_reg, onms_onms4_ctrl_2_reg.regValue);
	IoReg_Write32(ONMS_onms4_ctrl_reg, onms_onms4_ctrl_reg.regValue);

	if((src == VSC_INPUTSRC_HDMI) || (src == VSC_INPUTSRC_DP))
	{//demode
		onms_onms4_demode_vs_uf_RBUS onms_onms4_demode_vs_uf_reg;
		onms_onms4_demode_vs_of_RBUS onms_onms4_demode_vs_of_reg;
		onms_onms4_demode_vs_uf_reg.regValue = IoReg_Read32(ONMS_onms4_Demode_Vs_UF_reg);
		onms_onms4_demode_vs_of_reg.regValue = IoReg_Read32(ONMS_onms4_Demode_Vs_OF_reg);
#ifdef HDMI_VRR_ONLINEMEASURE_SUPPORT
		if(is_vrr_or_freesync_mode() || get_scaler_qms_mode_flag())//vrr case input is hdmi 2.1 need HDMI provide the api
		{//qms case similar with vrr
			rtd_pr_onms_info("[Eric][%s][%d]hdmi_framerate=%d\n", __FUNCTION__, __LINE__, Vfreq);

			if(Vfreq)
				onms_onms4_demode_vs_uf_reg.on4_de_vsper_uf_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 90 / Vfreq * 10;//120 hz max 24300000 = 27000000*0.9 //HD SD suggest
			else
				onms_onms4_demode_vs_uf_reg.on4_de_vsper_uf_th = 0;
			onms_onms4_demode_vs_of_reg.on4_de_vsper_of_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)11)/240;//24 hz minimax //29700000 = 27000000 * 1.1 //HD SD suggest
		}
		else
#endif
		{
			//V counter overflow judgment for ONMS     //H counter overflow judgment for ONMS
			//00: 2048                                 //00: 4096
			//01: 4096                                 //01: 8192
			//10: 8192                                 //10: 16384
			//11: 16383                                //11: 32768
			if(Vfreq)
			{
				onms_onms4_demode_vs_uf_reg.on4_de_vsper_uf_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 98 / Vfreq * 10;//for panel compability
				onms_onms4_demode_vs_of_reg.on4_de_vsper_of_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / Vfreq * 10;//29700000 = 27000000 * 1.1 //HD SD suggest
			}
			else
			{
				onms_onms4_demode_vs_uf_reg.on4_de_vsper_uf_th = 0;
				onms_onms4_demode_vs_of_reg.on4_de_vsper_of_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)11)/600;
			}
		}
		IoReg_Write32(ONMS_onms4_Demode_Vs_UF_reg, onms_onms4_demode_vs_uf_reg.regValue);
		IoReg_Write32(ONMS_onms4_Demode_Vs_OF_reg, onms_onms4_demode_vs_of_reg.regValue);
	}

	onms_onms4_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms4_ctrl_2_reg);
	onms_onms4_ctrl_3_reg.regValue = IoReg_Read32(ONMS_onms4_ctrl_3_reg);
	onms_onms4_ctrl_4_reg.regValue = IoReg_Read32(ONMS_onms4_ctrl_4_reg);
	if((src == VSC_INPUTSRC_HDMI) || (src == VSC_INPUTSRC_DP)) {
		/* vs_timeout */
		if (Vfreq != 0) {
			unsigned int framerate = (is_vrr_or_freesync_mode() || get_scaler_qms_mode_flag()) ? VRR_MIN_FRAMERATE : Vfreq;
			unsigned int threshold = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / framerate * 10 / 2048;

			onms_onms4_ctrl_2_reg.on4_vs_timeout_th    = threshold;
			onms_onms4_ctrl_2_reg.on4_vs_timeout_th_en = (threshold > 0) ? 1 : 0;
		}

		/* vs_bporch_timeout */
		if (Vfreq != 0) {
			onms_onms4_ctrl_3_reg.on4_vs_bporch_timeout_th = (Vtotal != 0) ? ( (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / Vfreq * 10 * Vstart / Vtotal ) : 0;
		}

		/* hs_per_timeout */
		if (Vfreq != 0) {
			unsigned int threshold = ( Vtotal > 0 ) ? ( (unsigned int)CONFIG_ONLINE_XTAL_FREQ * CONFIG_HS_PER_TIMEOUT_TH_TOLERANCE / Vfreq * 100 / Vtotal ) : 0;

			onms_onms4_ctrl_4_reg.on4_vact_num = Vactive;
			onms_onms4_ctrl_4_reg.on4_hs_per_timeout_th = threshold;
			onms_onms4_ctrl_4_reg.on4_hs_per_timeout_en = (Vactive > 0 && threshold > 0) ? 1 : 0;
		}
	} else {
		onms_onms4_ctrl_2_reg.on4_vs_timeout_th_en = 0;
		onms_onms4_ctrl_2_reg.on4_vs_timeout_th    = 0;

		onms_onms4_ctrl_3_reg.on4_vs_bporch_timeout_th = 0;

		onms_onms4_ctrl_4_reg.on4_hs_per_timeout_en = 0;
		onms_onms4_ctrl_4_reg.on4_vact_num = 0;
		onms_onms4_ctrl_4_reg.on4_hs_per_timeout_th = 0;
	}

	IoReg_Write32(ONMS_onms4_ctrl_2_reg, onms_onms4_ctrl_2_reg.regValue);
	IoReg_Write32(ONMS_onms4_ctrl_3_reg, onms_onms4_ctrl_3_reg.regValue);
	IoReg_Write32(ONMS_onms4_ctrl_4_reg, onms_onms4_ctrl_4_reg.regValue);


	// enable online measure
	onms_onms4_ctrl_reg.on4_sycms_clk = 1;
	onms_onms4_ctrl_reg.on4_htotal_max_delta = 8;
	onms_onms4_ctrl_reg.on4_ms_conti = 1;
	onms_onms4_ctrl_reg.on4_start_ms = 0;

	onms_onms4_ctrl_reg.on4_continuous_popup = 1;//enable continue pop for demode
	onms_onms4_ctrl_reg.on4_hd21_ms_en = 1;//hdmi2.1

	IoReg_Write32(ONMS_onms4_ctrl_reg, onms_onms4_ctrl_reg.regValue);
	onms_onms4_ctrl_reg.on4_start_ms = 1;
	IoReg_Write32(ONMS_onms4_ctrl_reg, onms_onms4_ctrl_reg.regValue);

#ifdef _CLEAR_ERROR_STATUS
	onms_onms4_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS_onms4_hsync_meas_result_reg);
	onms_onms4_hsync_meas_result_reg.on4_hmax_delta_total = 0;	//write clear
	IoReg_Write32(ONMS_onms4_hsync_meas_result_reg, onms_onms4_hsync_meas_result_reg.regValue);

	//write clear status, if can not clear, need add delay
	onms_onms4_status_reg.regValue = IoReg_Read32(ONMS_onms4_status_reg);
	onms_onms4_status_reg.on4_vs_per_to_long = 1;	//Time-Out (76ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit20
	onms_onms4_status_reg.on4_vs_pol_out = 1; // VSYNC Polarity Indicator
	// onms_onms4_status_reg.on4_hs_pol_out = 1; //HSYNC Polarity Indicator
	onms_onms4_status_reg.on4_vs_per_over_range = 1; //VSYNC_OVER_RANGE//Bit15
	onms_onms4_status_reg.on4_hs_per_over_range = 1; //HSYNC_OVER_RANGE//Bit14
	onms_onms4_status_reg.on4_vs_per_timeout = 1; //Time-Out (40ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit13
	onms_onms4_status_reg.on4_vs_high_timeout = 1;	//Time-Out bit of Input VSYNC High Pulse Measurement//Bit12
	onms_onms4_status_reg.on4_vs_per_overflow = 1;	//Over-flow bit of Input HSYNC Period Measurement//Bit11
	onms_onms4_status_reg.on4_hs_per_overflow = 1; //Over-flow bit of Input VSYNC Period Measurement//Bit10
	onms_onms4_status_reg.on4_vs_pol_chg= 1; //Input VSYNC Polarity change//Bit9
	// onms_onms4_status_reg.on4_hs_pol_chg = 1; //Input HSYNC Polarity change//Bit8
	onms_onms4_status_reg.on4_vtotalde_chg = 1; // Total active line number changes
	// onms_onms4_status_reg.on4_vtotal_chg = 1; // Total line number changes
	onms_onms4_status_reg.on4_hact_chg = 1; // Total active pixel number of first 8 active lines changes
	onms_onms4_status_reg.on4_htotal_chg = 1; //Total average pixel number of first 8 lines changes
	onms_onms4_status_reg.on4_hs_active_pixel_var = 1;//Bit3
	onms_onms4_status_reg.on4_vs_per_underflow = 1;	//Under-flow bit of Input HSYNC Period Measurement//Bit2
	onms_onms4_status_reg.on4_hs_meas_over_range = 1;//Bit21 Hs period measure over range
	onms_onms4_status_reg.on4_hgclk_gate_over_range = 1;//BIT22 hgclk gate over range
	IoReg_Write32(ONMS_onms4_status_reg, onms_onms4_status_reg.regValue);
#endif //#ifdef _CLEAR_ERROR_STATUS
}

void drvif_mode_disable_main_sub_onlinemeasure_only(unsigned char channel)
{
	onms_onms1_watchdog_en_RBUS onms_onms1_watchdog_en_reg;
	onms_onms1_interrupt_en_RBUS onms_onms1_interrupt_en_reg;
	vgip_int_ctl_RBUS vgip_int_ctl_reg;
	ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;
	onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;
	// onms_onms1_demode_ctrl_RBUS onms_onms1_demode_ctrl_reg;
	onms_onms1_ctrl_4_RBUS onms_onms1_ctrl_4_reg;
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	onms_onms2_watchdog_en_RBUS onms_onms2_watchdog_en_reg;
	ppoverlay_mp_layout_force_to_background_RBUS ppoverlay_mp_layout_force_to_background_reg;
	onms_onms2_interrupt_en_RBUS onms_onms2_interrupt_en_reg;
	onms_onms2_ctrl_RBUS onms_onms2_ctrl_reg;
	// onms_onms2_demode_ctrl_RBUS onms_onms2_demode_ctrl_reg;
#endif

#ifndef CONFIG_DUAL_CHANNEL
	channel = _CHANNEL1;
#endif

	rtd_pr_onms_info("###func %s Only disable channel:%d ###\n",__FUNCTION__, channel);

	/* Disable online measure watchdog */
	if(_CHANNEL1 == channel)
	{
		onms_onms1_watchdog_en_reg.regValue = 0;
		onms_onms1_watchdog_en_reg.on1_wd_to_main = _FALSE;		//Watch Dog select to Main Display
		IoReg_Write32(ONMS_onms1_watchdog_en_reg, onms_onms1_watchdog_en_reg.regValue);
		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);
		ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _DISABLE;	//Main Watch Dog Enable (Force-to-Background)
		ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = _DISABLE;	//FrameSync Main Watch Dog Enable (FreeRun)
		IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
		up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else// if(_CHANNEL2 == channel)
	{
		onms_onms2_watchdog_en_reg.regValue = 0;
		onms_onms2_watchdog_en_reg.on2_wd_to_sub = _FALSE;		//Watch Dog select to SUB Display
		IoReg_Write32(ONMS_onms2_watchdog_en_reg, onms_onms2_watchdog_en_reg.regValue);
		//add semaphore for sub disp @Crixus 20150901
		down(get_forcebg_semaphore());
		ppoverlay_mp_layout_force_to_background_reg.regValue = IoReg_Read32(PPOVERLAY_MP_Layout_Force_to_Background_reg);
		ppoverlay_mp_layout_force_to_background_reg.sub_wd_to_background = _DISABLE;//Disable forced bg for customer //_ENABLE;	//Sub Watch Dog Enable (Force-to-Background)
		ppoverlay_mp_layout_force_to_background_reg.sub_wd_to_free_run = _DISABLE;	//FrameSync Sub Watch Dog Enable (FreeRun), sub can't enable free run
		IoReg_Write32(PPOVERLAY_MP_Layout_Force_to_Background_reg, ppoverlay_mp_layout_force_to_background_reg.regValue);
		up(get_forcebg_semaphore());
	}
#endif

	/* Disable online measure interrupt */
	if(_CHANNEL1 == channel)
	{
		//onms_onms1_interrupt_en_reg.regValue = IoReg_Read32(ONMS_onms1_interrupt_en_reg);
		onms_onms1_interrupt_en_reg.regValue = 0;
		onms_onms1_interrupt_en_reg.on1_ie_to_main = _FALSE;		//Watch Dog select to Main Display
		IoReg_Write32(ONMS_onms1_interrupt_en_reg, onms_onms1_interrupt_en_reg.regValue);
		vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
		vgip_int_ctl_reg.onms1_int_ie = 0;//Disable onlinemeasure interrupt
		IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else// if(_CHANNEL2 == channel)
	{
		//onms_onms2_interrupt_en_reg.regValue = IoReg_Read32(ONMS_onms1_interrupt_en_reg);
		onms_onms2_interrupt_en_reg.regValue = 0;
		onms_onms2_interrupt_en_reg.on2_ie_to_main = _FALSE;		//Watch Dog select to Main Display
		IoReg_Write32(ONMS_onms2_interrupt_en_reg, onms_onms2_interrupt_en_reg.regValue);
		vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
		vgip_int_ctl_reg.onms2_int_ie = 0;//Disable onlinemeasure interrupt
		IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);
	}
#endif

	/* Disable online measure */
	if(channel == _CHANNEL1)
	{
		onms_onms1_ctrl_reg.regValue =  IoReg_Read32(ONMS_onms1_ctrl_reg);
		onms_onms1_ctrl_reg.on1_start_ms = 0;
		onms_onms1_ctrl_reg.on1_ms_conti = 0;
		onms_onms1_ctrl_reg.on1_continuous_popup = 0;
		IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);

		// onms_onms1_demode_ctrl_reg.regValue =  IoReg_Read32(ONMS_Onms1_Demode_ctrl_reg);
		// onms_onms1_demode_ctrl_reg.on1_demode_en = 0;
		// IoReg_Write32(ONMS_Onms1_Demode_ctrl_reg, onms_onms1_demode_ctrl_reg.regValue);

		/* hs_per_timeout */
		onms_onms1_ctrl_4_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_4_reg);
		onms_onms1_ctrl_4_reg.on1_hs_per_timeout_en = 0;
		IoReg_Write32(ONMS_onms1_ctrl_4_reg, onms_onms1_ctrl_4_reg.regValue);
	}
#ifdef CONFIG_DUAL_CHANNEL
	else
	{
		onms_onms2_ctrl_reg.regValue = IoReg_Read32(ONMS_onms2_ctrl_reg);
		onms_onms2_ctrl_reg.on2_start_ms = 0;
		onms_onms2_ctrl_reg.on2_ms_conti = 0;
		onms_onms2_ctrl_reg.on2_continuous_popup = 0;
		IoReg_Write32(ONMS_onms2_ctrl_reg, onms_onms2_ctrl_reg.regValue);

		// onms_onms2_demode_ctrl_reg.regValue =  IoReg_Read32(ONMS_Onms2_Demode_ctrl_reg);
		// onms_onms2_demode_ctrl_reg.on2_demode_en = 0;
		// IoReg_Write32(ONMS_Onms2_Demode_ctrl_reg, onms_onms2_demode_ctrl_reg.regValue);
	}
#endif
}

unsigned char drvif_mode_check_onlinemeasure_status(unsigned char channel)
{//Return FALSE: Online measure has error status.
	unsigned int check_status_mask = 0;
	unsigned char src;
	onms_onms1_status_RBUS onms_onms1_status_reg;
	onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;
	onms_onms2_ctrl_RBUS onms_onms2_ctrl_reg;

#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	onms_onms2_status_RBUS onms_onms2_status_reg;
#endif

#ifndef CONFIG_DUAL_CHANNEL
	channel = _CHANNEL1;
#endif
	src = get_scaler_connect_source(channel);

	if(channel == _CHANNEL1)
	{
		onms_onms1_ctrl_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_reg);
		if(onms_onms1_ctrl_reg.on1_ms_conti != 1)
		{
			//Mean main online no enable
			return TRUE;
		}
		onms_onms1_status_reg.regValue = IoReg_Read32(ONMS_onms1_status_reg);

		if(src == _SRC_HDMI)//HDMI RX,skip onms status check when work on specified device
		{
			unsigned int original_value = onms_onms1_status_reg.regValue;
			onms_onms1_status_reg.regValue = get_result_with_hdmi_mask(original_value);
			if(onms_onms1_status_reg.regValue != original_value)
			{
				rtd_pr_onms_info("[%s][HDMI] channel: %d, Update onms status from %x to %x\n", __FUNCTION__, channel, original_value, onms_onms1_status_reg.regValue);
			}
		}

		if(src == _SRC_HDMI)
		{
			if(get_scaler_qms_mode_flag())
			{//qms case similar with vrr
				check_status_mask = HDMI_QMS_ONLINE_ENABLE_MASK;
			}
			else if(vbe_disp_get_VRR_timingMode_flag())
			{
				check_status_mask = HDMI_VRR_ONLINE_ENABLE_MASK;
			}
			else if(vbe_disp_get_freesync_mode_flag())
			{
				check_status_mask = HDMI_FREESYNC_ONLINE_ENABLE_MASK;
			}
			else
			{
				check_status_mask = HDMI_NORMAL_ONLINE_ENABLE_MASK;
			}
			if(onms_onms1_status_reg.regValue & check_status_mask) {
				rtd_pr_onms_info("[%s][HDMI] onms1 status: %x, mask: %x\n", __FUNCTION__, onms_onms1_status_reg.regValue, check_status_mask);
				return FALSE;
			}
		}
		else if(src == _SRC_MINI_DP)
		{
			if(vbe_disp_get_VRR_timingMode_flag())
			{
				check_status_mask = DP_VRR_ONLINE_ENABLE_MASK;
			}
			else if(vbe_disp_get_freesync_mode_flag())
			{
				check_status_mask = DP_FREESYNC_ONLINE_ENABLE_MASK;
			}
			else
			{
				check_status_mask = DP_NORMAL_ONLINE_ENABLE_MASK;
			}
			if(onms_onms1_status_reg.regValue & check_status_mask)
			{
				return FALSE;
			}
		}
		else if(src == _SRC_CVBS)
		{
			check_status_mask = AVD_ONLINE_ENABLE_MASK;
			if(onms_onms1_status_reg.regValue & check_status_mask)
				return FALSE;
		}
	}
#ifdef CONFIG_DUAL_CHANNEL
	else
	{
		onms_onms2_ctrl_reg.regValue = IoReg_Read32(ONMS_onms2_ctrl_reg);
		if(onms_onms2_ctrl_reg.on2_ms_conti != 1)
		{
			//Mean sub online no enable
			return TRUE;
		}
		onms_onms2_status_reg.regValue = IoReg_Read32(ONMS_onms2_status_reg);

		if(src == _SRC_HDMI)//HDMI RX,skip onms status check when work on specified device
		{
			unsigned int original_value = onms_onms2_status_reg.regValue;
			onms_onms2_status_reg.regValue = get_result_with_hdmi_mask(original_value);
			if(onms_onms2_status_reg.regValue != original_value)
			{
				rtd_pr_onms_info("[%s][HDMI] channel: %d, Update onms status from %x to %x\n", __FUNCTION__, channel, original_value, onms_onms2_status_reg.regValue);
			}
		}

		if(src == _SRC_HDMI)
		{
			check_status_mask = HDMI_NORMAL_ONLINE_ENABLE_MASK;
			if(onms_onms2_status_reg.regValue & check_status_mask)
				return FALSE;
		}
		else if(src == _SRC_MINI_DP)
		{
			check_status_mask = HDMI_NORMAL_ONLINE_ENABLE_MASK;
			if(onms_onms2_status_reg.regValue & check_status_mask)
				return FALSE;
		}
		else if(src == _SRC_CVBS)
		{
			check_status_mask = AVD_ONLINE_ENABLE_MASK;
			if(onms_onms2_status_reg.regValue & check_status_mask)
				return FALSE;
		}
	}
#endif
	return TRUE;
}

unsigned char drvif_mode_check_dma_onlinemeasure_status(void)
{//Return FALSE: Online measure has error status.
	unsigned int check_status_mask = 0;
	onms_onms3_status_RBUS onms_onms3_status_reg;
	onms_onms3_ctrl_RBUS onms_onms3_ctrl_reg;
	VSC_INPUT_TYPE_T src;
	src = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);
	onms_onms3_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_reg);

	if(onms_onms3_ctrl_reg.on3_ms_conti != 1)
	{
		//Mean dma online no enable
		return TRUE;
	}
	onms_onms3_status_reg.regValue = IoReg_Read32(ONMS_onms3_status_reg);
	if(src == VSC_INPUTSRC_HDMI)//HDMI RX,skip onms status check when work on specified device
	{
		unsigned int original_value = onms_onms3_status_reg.regValue;
		onms_onms3_status_reg.regValue = get_result_with_hdmi_mask(original_value);
		if(onms_onms3_status_reg.regValue != original_value)
		{
			rtd_pr_onms_info("[%s][HDMI] channel: dma, Update onms status from %x to %x\n", __FUNCTION__, original_value, onms_onms3_status_reg.regValue);
		}
	}

	if(src == VSC_INPUTSRC_HDMI)
	{
		check_status_mask = get_hdmi_online_wd_int_setting();
		if(onms_onms3_status_reg.regValue & check_status_mask)
			return FALSE;
	}
	else if(src == VSC_INPUTSRC_DP)
	{
		if(vbe_disp_get_VRR_timingMode_flag())
		{
			check_status_mask = DP_VRR_ONLINE_ENABLE_MASK;
		}
		else if(vbe_disp_get_freesync_mode_flag())
		{
			check_status_mask = DP_FREESYNC_ONLINE_ENABLE_MASK;
		}
		else
		{
			check_status_mask = DP_NORMAL_ONLINE_ENABLE_MASK;
		}
		if(onms_onms3_status_reg.regValue & check_status_mask)
			 return FALSE;
	}
	else if(src == VSC_INPUTSRC_AVD)
	{
		check_status_mask = AVD_ONLINE_ENABLE_MASK;
		if(onms_onms3_status_reg.regValue & check_status_mask)
			return FALSE;
	}

	return TRUE;
}

unsigned char drvif_mode_check_onms4_status(void)
{//Return FALSE: Online measure has error status.
	unsigned int check_status_mask = 0;
	onms_onms4_status_RBUS onms_onms4_status_reg;
	VSC_INPUT_TYPE_T src;
	src = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);

	if(ONMS_onms4_ctrl_get_on4_ms_conti(IoReg_Read32(ONMS_onms4_ctrl_reg)) == 0)
	{
		//Mean onms4 online no enable
		return TRUE;
	}

	onms_onms4_status_reg.regValue = IoReg_Read32(ONMS_onms4_status_reg);
	if(src == VSC_INPUTSRC_HDMI)//HDMI RX,skip onms status check when work on specified device
	{
		unsigned int original_value = onms_onms4_status_reg.regValue;
		onms_onms4_status_reg.regValue = get_result_with_hdmi_mask(original_value);
		if(onms_onms4_status_reg.regValue != original_value)
		{
			rtd_pr_onms_info("[%s][HDMI] channel: onms4, Update onms status from %x to %x\n", __FUNCTION__, original_value, onms_onms4_status_reg.regValue);
		}
	}

	if(src == VSC_INPUTSRC_HDMI)
	{
		check_status_mask = get_hdmi_online_wd_int_setting();
		if(onms_onms4_status_reg.regValue & check_status_mask)
			return FALSE;
	}
	else if(src == VSC_INPUTSRC_DP)
	{
		if(vbe_disp_get_VRR_timingMode_flag())
		{
			check_status_mask = DP_VRR_ONLINE_ENABLE_MASK;
		}
		else if(vbe_disp_get_freesync_mode_flag())
		{
			check_status_mask = DP_FREESYNC_ONLINE_ENABLE_MASK;
		}
		else
		{
			check_status_mask = DP_NORMAL_ONLINE_ENABLE_MASK;
		}
		if(onms_onms4_status_reg.regValue & check_status_mask)
			 return FALSE;
	}

	return TRUE;
}

#ifndef BUILD_QUICK_SHOW
void drvif_mode_disablevgip(unsigned char display)
{
	// Disable Sample Clock in VGIP
	fw_scaler_set_sample(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL), _DISABLE);
}

extern void HAL_VPQ_MEMC_Freeze(unsigned char enable);
void drvif_mode_resetmode(unsigned char display)
{

	extern void fw_scaler_ctrl_dma_vgip_regen_vsync(unsigned char dev, unsigned char enable);

#ifdef CONFIG_HDR_SDR_SEAMLESS
#if defined(CONFIG_ENABLE_HDR10_HDMI_AUTO_DETECT)|| defined(CONFIG_ENABLE_DOLBY_VISION_HDMI_AUTO_DETECT)
    extern void i3ddma_cap1_interrupt_ctrl(unsigned char enable);
	_RPC_system_setting_info  *RPC_VIP_sysInfo = (_RPC_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_system_info_structure);
#endif
#endif
//	vgip_data_path_select_RBUS data_path_select_reg;
//	vgip_vgip_chn1_act_hsta_width_RBUS vgip_chn1_act_hsta_width_reg;
	vgip_vgip_chn1_ctrl_RBUS vgip_chn1_ctrl_reg;
	onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;
	onms_onms3_ctrl_RBUS onms_onms3_ctrl_reg;
	vgip_vgip_chn1_delay_RBUS vgip_chn1_delay_reg;
	dma_vgip_dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;
	dma_vgip_dma_vgip_delay_RBUS dma_vgip_chn1_delay_reg;
	scaleup_dm_uzu_db_ctrl_RBUS scaleup_dm_uzu_db_ctrl_Reg;
	vgip_vgip_chn1_double_buffer_ctrl_RBUS vgip_chn1_double_buffer_ctrl_reg;
	ppoverlay_double_buffer_ctrl_RBUS ppoverlay_double_buffer_ctrl_Reg;
    ppoverlay_double_buffer_ctrl4_RBUS ppoverlay_double_buffer_ctrl4_Reg;

#ifdef CONFIG_DUAL_CHANNEL
//	sub_vgip_vgip_chn2_act_hsta_width_RBUS vgip_chn2_act_hsta_width_reg;
	sub_vgip_vgip_chn2_delay_RBUS vgip_chn2_delay_reg;
	sub_vgip_vgip_chn2_ctrl_RBUS vgip_chn2_ctrl_reg;
	onms_onms2_ctrl_RBUS onms_onms2_ctrl_reg;
	scaleup_ds_uzu_db_ctrl_RBUS scaleup_ds_uzu_db_ctrl_Reg;
	sub_vgip_vgip_chn2_double_buffer_ctrl_RBUS vgip_chn2_double_buffer_ctrl_reg;
#endif
	unsigned char src;
	unsigned long flags;//for spin_lock_irqsave
    unsigned int *setup_di_done_flag = NULL;

	src = get_scaler_connect_source(display);

    Set_Val_bQsmode(FALSE);
	reset_rotate_mode(SLR_MAIN_DISPLAY);

	// must clear 	s1_clk_en, then clear main_imd_smooth_toggle_clk_time2switch_en
	/* disable snr clk @ begin of reset mode for snr hw issue, elieli 20160825*/
	//disable data path s1 clock
	if (display == SLR_MAIN_DISPLAY) {
		//disable data path s1 clock
		drvif_color_Set_DRV_SNR_Clock(display, _ENABLE);//make sure clock enable

		/* Disable main VGIP online measure first */
		drvif_mode_disable_main_sub_onlinemeasure_only(display);

		/* Set main VGIP source select to VD/ATV, to make sure 2p related reg. clock enable */
		vgip_chn1_ctrl_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_CTRL_reg);
		vgip_chn1_ctrl_reg.ch1_in_sel = 2;
		IoReg_Write32(VGIP_VGIP_CHN1_CTRL_reg, vgip_chn1_ctrl_reg.regValue);

		drvif_color_Set_DRV_SNR_Clock(display, _DISABLE);
	}

	//if(get_zoom_reset_double_state(display) == _ENABLE)
	{
		set_zoom_reset_double_state(_DISABLE, display);//always disable double buffer
	}

	if (display == SLR_MAIN_DISPLAY) {
		reset_hdrtype_change_counter();
		drvif_Hdmi_SetDolbyVsifUpdateNoPktThreshold(NO_DVS_INFO_MAX);
		HDR_DolbyVision_OTT_set_nonDolby_to_dolby(false);
		HDR_DolbyVision_OTT_set_dolby_to_nonDolby(false);
	}

	if(get_scaler_run_hfr_mode() != HDMI_NON_4K120)
	{
		if(display == SLR_MAIN_DISPLAY)
			set_zoom_reset_double_state(_DISABLE, SLR_SUB_DISPLAY);//always disable double buffer//from main to do sub reset

		set_scaler_run_hfr_mode(HDMI_NON_4K120);
		//reset sub
		drvif_memory_DisableMCap(SLR_SUB_DISPLAY);
		drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_SUB_DISPLAY, _DISABLE);

		vgip_chn2_double_buffer_ctrl_reg.regValue =rtd_inl(SUB_VGIP_VGIP_CHN2_DOUBLE_BUFFER_CTRL_reg);
		vgip_chn2_double_buffer_ctrl_reg.ch2_db_en = 0;
		vgip_chn2_double_buffer_ctrl_reg.ch2_db_rdy = 0;

		//disable vgip isr
		drvif_scaler_vactive_end_irq(_DISABLE, SLR_SUB_DISPLAY);//disable VGIP interrupt (for auto ma)
		drvif_scaler_vactive_sta_irq(_DISABLE, SLR_SUB_DISPLAY);//disable VGIP interrupt (for auto ma)

		// Disable Sample Clock in VGIP
		fw_scaler_set_sample(SLR_SUB_DISPLAY, _DISABLE);

		rtd_outl(SUB_VGIP_VGIP_CHN2_DOUBLE_BUFFER_CTRL_reg, vgip_chn2_double_buffer_ctrl_reg.regValue);
		vgip_chn2_ctrl_reg.regValue = IoReg_Read32(SUB_VGIP_VGIP_CHN2_CTRL_reg);
		vgip_chn2_ctrl_reg.ch2_field_inv = 0;
		// FixMe
		//vgip_chn2_ctrl_reg.ch2_hs_syncedge = 0; //CSW+ 0961120 Reset to positive edge to latch the stable data
		vgip_chn2_ctrl_reg.ch2_vs_syncedge = 0;
		vgip_chn2_ctrl_reg.ch2_field_det_en = 0;
		vgip_chn2_ctrl_reg.ch2_vs_by_hs_en_n = 0;
		vgip_chn2_ctrl_reg.ch2_vs_inv = 0;
		vgip_chn2_ctrl_reg.ch2_hs_inv = 0;
		vgip_chn2_ctrl_reg.ch2_safe_mode = 0;
		vgip_chn2_ctrl_reg.ch2_vs_by_hs_en_n = 0;
		vgip_chn2_ctrl_reg.ch2_in_clk_en = 0;
		vgip_chn2_ctrl_reg.ch2_fdrop_en = 0;
		IoReg_Write32(SUB_VGIP_VGIP_CHN2_CTRL_reg, vgip_chn2_ctrl_reg.regValue);

		drvif_color_Set_DRV_SNR_Clock(SLR_SUB_DISPLAY, 0);

		drvif_memory_free_block(MEMIDX_SUB);   // free block befor getting a new one
		drvif_memory_free_block(MEMIDX_SUB_SEC);   // free block befor getting a new one
		drvif_memory_free_block(MEMIDX_SUB_THIRD);	// free block befor getting a new on
	} else if (get_scaler_run_4k60_vrr_two_path())
	{
		if(display == SLR_MAIN_DISPLAY)
			set_zoom_reset_double_state(_DISABLE, SLR_SUB_DISPLAY);//always disable double buffer//from main to do sub reset

		set_scaler_run_4k60_vrr_two_path(FALSE);
		//reset sub m domain
		drvif_memory_DisableMCap(SLR_SUB_DISPLAY);
		drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_SUB_DISPLAY, _DISABLE);
	}
    else if(get_scaler_run_sub_nnsr_two_path())
    {
        set_sub_for_srnn(SUB_UZD_SRNN, 0);
        set_scaler_run_sub_nnsr_two_path(FALSE);
        //reset sub m domain
        drvif_memory_DisableMCap(SLR_SUB_DISPLAY);
        drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_SUB_DISPLAY, _DISABLE);

        vgip_chn2_double_buffer_ctrl_reg.regValue =rtd_inl(SUB_VGIP_VGIP_CHN2_DOUBLE_BUFFER_CTRL_reg);
        vgip_chn2_double_buffer_ctrl_reg.ch2_db_en = 0;
        vgip_chn2_double_buffer_ctrl_reg.ch2_db_rdy = 0;
        rtd_outl(SUB_VGIP_VGIP_CHN2_DOUBLE_BUFFER_CTRL_reg, vgip_chn2_double_buffer_ctrl_reg.regValue);

        vgip_chn2_ctrl_reg.regValue = IoReg_Read32(SUB_VGIP_VGIP_CHN2_CTRL_reg);
        vgip_chn2_ctrl_reg.ch2_in_clk_en = 0;
        IoReg_Write32(SUB_VGIP_VGIP_CHN2_CTRL_reg, vgip_chn2_ctrl_reg.regValue);
    }

	//drv_memory_set_sub_gate_vo1(FALSE);


	if(/*(src != _SRC_VO)
		&& */((Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_FAILURECOUNT) != 0) || (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_FAILURECOUNT_NOSUPPORT) != 0)))
	{
		//rtd_pr_vsc_info("[DBG] Ignore mode reset failCnt=%d/%d\n", Scaler_DispGetInputInfo(SLR_INPUT_FAILURECOUNT), Scaler_DispGetInputInfo(SLR_INPUT_FAILURECOUNT_NOSUPPORT));
		return;
	}
	Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_FAILURECOUNT, 1);//Avoid to do drvif_mode_resetmode to save time
    scaler_vsc_set_gamemode_force_framerate_lowdelay_mode(0);

	rtd_pr_vsc_notice("%s disp:%d \n", __FUNCTION__, display);

	//drv_memory_set_sub_gate_vo1(FALSE);

	if (display == SLR_MAIN_DISPLAY) {
		drivf_scaler_reset_freerun(DTG_MASTER);
		//reset db_latch_line in porch for next NNSR mode change
		memory_frc_set_pre_read_v_start(SLR_MAIN_DISPLAY, drv_Calculate_m_pre_read_value());
		if(vbe_disp_get_dynamic_memc_bypass_flag()==1){
			// rtd_pr_vsc_emerg("%s.%d\n", __FUNCTION__, __LINE__);
			// enable memc clk
			// rtd_pr_vsc_notice("[jerry_Scaler_MEMC_Bypass_CLK_OnOff_DDD]\n");
			// Scaler_MEMC_Bypass_CLK_OnOff(1);
		}
#ifdef CONFIG_HDR_SDR_SEAMLESS
#if defined(CONFIG_ENABLE_HDR10_HDMI_AUTO_DETECT)|| defined(CONFIG_ENABLE_DOLBY_VISION_HDMI_AUTO_DETECT)
		i3ddma_cap1_interrupt_ctrl(FALSE);//disable i3ddma cap1 interrupt
		if (RPC_VIP_sysInfo) {
			rtd_pr_vsc_info("DM_freeze, reset mode, unfreeze,\n");
			HAL_VPQ_MEMC_Freeze(0);
			RPC_VIP_sysInfo->HDR_info.Ctrl_Item[TV006_HDR_Seamless_Freeze_Status] = 0;
			RPC_VIP_sysInfo->HDR_info.Ctrl_Item[TV006_HDR_Seamless_Freeze_Cnt] = 0;
		}
#endif
#endif

	}

	// Disable all HW Watchdog
	if (display == SLR_MAIN_DISPLAY){
		drvif_scaler_i3ddma_vodma_fifo_setting(_DISABLE);//disable fifo mode @Crixus 20160816
	}

	drvif_mode_onlinemeasure_setting(display, src, FALSE, FALSE);//Disable online measure watchdog and interrupt
	drvif_mode_disableonlinemeasure(display);//Disable online measure

	drvif_mode_onms4_setting(_DISABLE, _DISABLE);
	drvif_mode_disable_onms4();

	fw_scaler_ctrl_vgip_regen_vsync(display, _DISABLE);
	if (display == SLR_MAIN_DISPLAY) {
		Set_Val_DP_Vsync_Regen_Pos(I3DDMA_DEV, DP_NO_REGEN_VSYNC);//reset regen position
		fw_scaler_ctrl_dma_vgip_regen_vsync(I3DDMA_DEV, _DISABLE);
		framesync_syncDoubleDVSReset();
		set_hdr_fake_porch(false);
		reset_hdr_mode(SLR_MAIN_DISPLAY);//disable dolby mode
		rest_dtg_ivs_source();
	}

	if (display == SLR_SUB_DISPLAY) {
		IoReg_ClearBits(MDOM_SUB_CAP_Cap1_buf_control_reg, MDOM_SUB_CAP_Cap1_buf_control_cap1_input_fast_mask);//reset sub frc style
	}

	if(display == SLR_MAIN_DISPLAY) {
        setup_di_done_flag = (unsigned int *)Scaler_GetShareMemVirAddr(SCALERIOC_SETUP_DI_DONE);
        if(setup_di_done_flag)
        {
            *setup_di_done_flag = Scaler_ChangeUINT32Endian(0);
        }

		drvif_framesync_reset_dv_total_flg(_TRUE);
		//drvif_framesync_gatting(_DISABLE);
		drvif_scaler_set_display_mode(DISPLAY_MODE_FRC, scaler_dtg_get_app_type()); //set FRC mode

		//frank@1103 mark to reduce time
		//WaitFor_EN_STOP_MAIN();
		//frank@04152013  add disable ddomain double buffer code avoid I domain IVS loss ++
		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
		//IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, 0);
        ppoverlay_double_buffer_ctrl_Reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
		ppoverlay_double_buffer_ctrl_Reg.dmainreg_dbuf_set = 0;
		ppoverlay_double_buffer_ctrl_Reg.dmainreg_dbuf_en=0;
		ppoverlay_double_buffer_ctrl_Reg.dreg_dbuf_en = 0;
		ppoverlay_double_buffer_ctrl_Reg.dreg_dbuf_set=0;
		ppoverlay_double_buffer_ctrl_Reg.drm_multireg_dbuf_en = 0;
		ppoverlay_double_buffer_ctrl_Reg.drm_multireg_dbuf_set=0;
		ppoverlay_double_buffer_ctrl_Reg.drm_multireg_dbuf_vsync_sel=0;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_Reg.regValue);

		ppoverlay_double_buffer_ctrl4_Reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL4_reg);
		ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_set = 0;
		ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_en=0;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL4_reg, ppoverlay_double_buffer_ctrl4_Reg.regValue);
		spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
		up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		//frank@04152013  add disable ddomain double buffer code avoid I domain IVS loss --
		//drvif_color_rtnr_off(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL));//fixme:vip
		framesync_set_enterlastline_at_frc_mode_flg(_FALSE);
		//Scaler_OPC_Function_Enable(_DISABLE);

#ifdef CONFIG_ENABLE_3D_FRAME_PACKING_ID_DOMAIN_NEW_RELATION
		Scaler_3D_FramePacking_Mode_Run_Framesync_Action(_FALSE);
#endif //#ifdef CONFIG_ENABLE_3D_FRAME_PACKING_ID_DOMAIN_NEW_RELATION

#ifdef CONFIG_ENABLE_FLL_DYNAMIC_ADJUST_DHTOTAL
		framesync_lastlinefinetune_at_frc_mode_dymanic_adjust_dhtotal(FLL_DYMANIC_ADJUST_DHTOTAL_NONE);
#endif //#ifdef CONFIG_ENABLE_FLL_DYNAMIC_ADJUST_DHTOTAL
#ifdef CONFIG_ENABLE_FRAMESYNC_MODE_DYNAMIC_ADJUST_DHTOTAL
		framesync_mode_dymanic_adjust_dhtotal(FRAMESYNC_DYMANIC_ADJUST_DHTOTAL_NONE);
#endif //#ifdef CONFIG_ENABLE_FRAMESYNC_MODE_DYNAMIC_ADJUST_DHTOTAL

		drv_GameMode_iv2dv_delay(_DISABLE);
	}
	drvif_scaler_dma_vgip_sta_irq_ctrl(0);//disable dma vgip start isr for qms

	// Avoid to show TV-Bus error for DI
	if(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_THRIP) || Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_RTNR))	// Change to use display avoid to use info make system error, 970616 weihao
		fw_scalerip_disable_ip(display);

	// Disable Measure Watchdog
	fw_scaler_enable_measure_wdg(display, _DISABLE);

	//fixme:vip
#if 0 //frank@1103 mark below code to reduce time
	//unfreeze di_freeze if it's freezed
	if ( display == _MAIN_DISPLAY )
		drvif_color_di_freeze(FALSE);
#endif

#ifndef CONFIG_ENABLE_CHANGE_COLOR_STD_WITH_SMOOTH_TOGGLE

	//unfreeze display if it's freezed
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	IoReg_ClearBits(display ? MDOM_SUB_CAP_Cap1_buf_control_reg : MDOMAIN_CAP_Cap0_buf_control_reg, _BIT1);
#else
	IoReg_ClearBits(MDOMAIN_CAP_Cap0_buf_control_reg, _BIT1);
#endif

#endif
#ifdef CONFIG_DUAL_CHANNEL
	drvif_memory_DisableMCap(display);
	drvif_memory_EnableMDisp_Or_DisableMDisp(display, _DISABLE);
#else
	drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);
	drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_MAIN_DISPLAY, _DISABLE);
#endif
	drvif_memory_set_dbuffer_write(display);

	if (display == SLR_MAIN_DISPLAY)
	{
		// Reset boundary pretection to global boundary
		IoReg_Write32(MDOMAIN_CAP_Cap0_BoundaryAddr_CTRL0_reg, 0x00000000);
        IoReg_Write32(MDOMAIN_CAP_Cap0_uplimit_0_reg, 0x00000000);

		// Disable PQC clk
		//mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);
		//mdomain_cap_smooth_tog_ctrl_0_reg.in1_pqc_clken = 1; // 1: disable
		//IoReg_Write32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg, mdomain_cap_smooth_tog_ctrl_0_reg.regValue);

	}
#ifdef CONFIG_DUAL_CHANNEL
	else
	{
		// Reset boundary pretection to global boundary
		IoReg_Write32(MDOM_SUB_CAP_Cap1_BoundaryAddr_CTRL0_reg, 0x00000000);
        IoReg_Write32(MDOM_SUB_CAP_Cap1_uplimit_0_reg, 0x00000000);

		// Disable PQC clk
		//mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);
		//mdomain_cap_smooth_tog_ctrl_0_reg.in2_pqc_clken = 1; // 1: disable
		//IoReg_Write32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg, mdomain_cap_smooth_tog_ctrl_0_reg.regValue);
	}
#endif

#if IS_ENABLED(CONFIG_RTK_KDRV_QOS_CONTROL)
	if (display == SLR_MAIN_DISPLAY) {
		//QOS mode reset
		rtk_qos_set_mode(RTK_QOS_NORMAL_MODE);
	}
#endif
#ifndef UT_flag
	 /*for cma memory control, when close sub, m domain memory should be free*/
	if (display == SLR_SUB_DISPLAY) {
		void *sub_1st_addr = get_sub_m_domain_1st_buf_address();
		void *sub_2nd_addr = get_sub_m_domain_2nd_buf_address();
		void *sub_3rd_addr = get_sub_m_domain_3rd_buf_address();

		wait_sub_disp_double_buffer_apply();

		if (sub_1st_addr != NULL) {
			rtd_pr_vsc_info("Release M-domain sub 1st buffer %px\n", sub_1st_addr);
			set_sub_m_domain_1st_buf_address(NULL);
			dvr_free(sub_1st_addr);
		}

		if (sub_2nd_addr != NULL) {
			rtd_pr_vsc_info("Release M-domain sub 2nd buffer %px\n", sub_2nd_addr);
			set_sub_m_domain_2nd_buf_address(NULL);
			dvr_free(sub_2nd_addr);
		}

		if (sub_3rd_addr != NULL) {
			rtd_pr_vsc_info("Release M-domain sub 3rd buffer %px\n", sub_3rd_addr);
			set_sub_m_domain_3rd_buf_address(NULL);
			dvr_free(sub_3rd_addr);
		}
	 } else if (display == SLR_MAIN_DISPLAY) {
	 	drvif_memory_free_block(MEMIDX_MAIN_SEC);    // free cma memory for 4k size
#ifndef CONFIG_MDOMAIN_RESERVED_MEMORY
	   drvif_memory_free_block(MEMIDX_MAIN);    // free block befor getting a new one
	   drvif_memory_free_block(MEMIDX_MAIN_SEC);    // free block befor getting a new one
	   drvif_memory_free_block(MEMIDX_MAIN_THIRD);
#else
	#ifdef CONFIG_ALL_SOURCE_DATA_FS
	drvif_memory_free_block(MEMIDX_MAIN);    // free block befor getting a new one
	drvif_memory_free_block(MEMIDX_MAIN_SEC);    // free block befor getting a new one
	drvif_memory_free_block(MEMIDX_MAIN_THIRD);
	#endif
#endif
	 }
#endif // #ifndef UT_flag
	// Disable Sample Clock in VGIP
	fw_scaler_set_sample(display, _DISABLE);


	if((src != _SRC_VO) || (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_THROUGH_I3DDMA) == FALSE)){
		// Disable SRC_Clock_Enbale bit
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
		IoReg_ClearBits(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? SUB_VGIP_VGIP_CHN2_CTRL_reg: VGIP_VGIP_CHN1_CTRL_reg, _BIT0);
#else
		IoReg_ClearBits(/*Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? SUB_VGIP_VGIP_CHN2_CTRL_reg: */VGIP_CHN1_CTRL_reg, _BIT0);
#endif
	}

	if((src == _SRC_VO) && (display == SLR_MAIN_DISPLAY) && (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_THROUGH_I3DDMA) == FALSE)){
		vgip_vgip_chn1_ctrl_RBUS vgip_chn1_ctrl_reg;
		vgip_chn1_ctrl_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_CTRL_reg);
		if(vgip_chn1_ctrl_reg.ch1_in_sel == 1){
			IoReg_ClearBits(VGIP_VGIP_CHN1_CTRL_reg, _BIT0);
		}

	}

	if(SLR_MAIN_DISPLAY == display)
	{
		vgip_chn1_double_buffer_ctrl_reg.regValue = rtd_inl(VGIP_VGIP_CHN1_DOUBLE_BUFFER_CTRL_reg);
		vgip_chn1_double_buffer_ctrl_reg.ch1_db_en = 0;
		vgip_chn1_double_buffer_ctrl_reg.ch1_db_rdy = 0;
		rtd_outl(VGIP_VGIP_CHN1_DOUBLE_BUFFER_CTRL_reg, vgip_chn1_double_buffer_ctrl_reg.regValue);

		//set polarity to default: H-positive, V-positive
		Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_POLARITY,_SYNC_HP_VP);
		vgip_chn1_ctrl_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_CTRL_reg);
		vgip_chn1_ctrl_reg.ch1_field_inv = 0;
		vgip_chn1_ctrl_reg.ch1_field_sync_edge = 0;

		//vgip_chn1_ctrl_reg.ch1_hs_syncedge = 0; //fix me
		vgip_chn1_ctrl_reg.ch1_vs_syncedge = 0;
		vgip_chn1_ctrl_reg.ch1_field_det_en = 0;
		vgip_chn1_ctrl_reg.ch1_field_sync_edge = 0;
		vgip_chn1_ctrl_reg.ch1_vs_inv = 0;
		vgip_chn1_ctrl_reg.ch1_hs_inv = 0;
		vgip_chn1_ctrl_reg.ch1_safe_mode = 0;
		vgip_chn1_ctrl_reg.ch1_vs_by_hs_en_n = 0;
		IoReg_Write32(VGIP_VGIP_CHN1_CTRL_reg, vgip_chn1_ctrl_reg.regValue);
		/* move to begin of reset mode for snr hw issue, elieli 20160825*/
/*
		//disable data path s1 clock
		data_path_select_reg.regValue = IoReg_Read32(VGIP_Data_Path_Select_reg);
		data_path_select_reg.s1_clk_en = 0;
		IoReg_Write32(VGIP_Data_Path_Select_reg, data_path_select_reg.regValue);
*/
		// Disable On-Line Measure
		onms_onms1_ctrl_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_reg);
		onms_onms1_ctrl_reg.on1_sycms_clk = 0;
		onms_onms1_ctrl_reg.on1_vs_meas_inv = 0;
		onms_onms1_ctrl_reg.on1_ms_conti = 0;
		onms_onms1_ctrl_reg.on1_start_ms = 0;
		IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);
		//restore Hsync/Vsync delay to zero
		vgip_chn1_delay_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_DELAY_reg);
		vgip_chn1_delay_reg.ch1_ihs_dly = 0;
		vgip_chn1_delay_reg.ch1_ivs_dly = 0;
		IoReg_Write32(VGIP_VGIP_CHN1_DELAY_reg, vgip_chn1_delay_reg.regValue);

		//Disable dma On-Line Measure
		onms_onms3_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_reg);
		onms_onms3_ctrl_reg.on3_sycms_clk = 0;
		onms_onms3_ctrl_reg.on3_vs_meas_inv = 0;
		onms_onms3_ctrl_reg.on3_ms_conti = 0;
		onms_onms3_ctrl_reg.on3_start_ms = 0;
		IoReg_Write32(ONMS_onms3_ctrl_reg, onms_onms3_ctrl_reg.regValue);

		dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
		dma_vgip_chn1_ctrl_reg.dma_field_inv = 0;
		dma_vgip_chn1_ctrl_reg.dma_field_sync_edge = 0;
		//dma_vgip_chn1_ctrl_reg.dma_hs_syncedge = 0; //CSW+ 0961120 Reset to positive edge to latch the stable data
		dma_vgip_chn1_ctrl_reg.dma_vs_syncedge = 0;
		dma_vgip_chn1_ctrl_reg.dma_field_det_en = 0;
		dma_vgip_chn1_ctrl_reg.dma_vs_by_hs_en_n = 0;
		dma_vgip_chn1_ctrl_reg.dma_vs_inv = 0;
		dma_vgip_chn1_ctrl_reg.dma_hs_inv = 0;
		dma_vgip_chn1_ctrl_reg.dma_safe_mode = 0;
		dma_vgip_chn1_ctrl_reg.dma_vs_by_hs_en_n = 0;
		IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

		dma_vgip_chn1_delay_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_DELAY_reg);
		dma_vgip_chn1_delay_reg.dma_ihs_dly = 0;
		dma_vgip_chn1_delay_reg.dma_ivs_dly = 0;
		IoReg_Write32(DMA_VGIP_DMA_VGIP_DELAY_reg, dma_vgip_chn1_delay_reg.regValue);

		// Disable UZU double buffer
		scaleup_dm_uzu_db_ctrl_Reg.regValue = IoReg_Read32(SCALEUP_DM_UZU_DB_CTRL_reg);
		scaleup_dm_uzu_db_ctrl_Reg.db_en = 0;
		IoReg_Write32(SCALEUP_DM_UZU_DB_CTRL_reg, scaleup_dm_uzu_db_ctrl_Reg.regValue);

		//Switch HDMI(ex: HDMI3 -> HDMI4) runs vsc_disconnect() that reset Scaler_FreeRun_To_FrameSync_By_Reset_VODMA status
		//But change video content at single HDMI, it also needs reset Scaler_FreeRun_To_FrameSync_By_Reset_VODMA status.
		//Add reset Scaler_FreeRun_To_FrameSync_By_Reset_VODMA status code
		Scaler_Set_FreeRun_To_FrameSync_By_Reset_VODMA(TRUE);
		Scaler_Set_FreeRun_To_FrameSync_By_HW_SpeedUp_ByMcode_HW_CTRL4(FALSE);
	}
#ifdef CONFIG_DUAL_CHANNEL
	else// if(_CHANNEL2 == GET_INPUT_CHANNEL(display))
	{
		vgip_chn2_double_buffer_ctrl_reg.regValue =rtd_inl(SUB_VGIP_VGIP_CHN2_DOUBLE_BUFFER_CTRL_reg);
		vgip_chn2_double_buffer_ctrl_reg.ch2_db_en = 0;
		vgip_chn2_double_buffer_ctrl_reg.ch2_db_rdy = 0;
		rtd_outl(SUB_VGIP_VGIP_CHN2_DOUBLE_BUFFER_CTRL_reg, vgip_chn2_double_buffer_ctrl_reg.regValue);

		//set polarity to default: H-positive, V-positive
		Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_POLARITY,_SYNC_HP_VP);

		vgip_chn2_ctrl_reg.regValue = IoReg_Read32(SUB_VGIP_VGIP_CHN2_CTRL_reg);
		vgip_chn2_ctrl_reg.ch2_field_inv = 0;
		// FixMe
		//vgip_chn2_ctrl_reg.ch2_hs_syncedge = 0; //CSW+ 0961120 Reset to positive edge to latch the stable data
		vgip_chn2_ctrl_reg.ch2_vs_syncedge = 0;
		vgip_chn2_ctrl_reg.ch2_field_det_en = 0;
		vgip_chn2_ctrl_reg.ch2_vs_by_hs_en_n = 0;
		vgip_chn2_ctrl_reg.ch2_vs_inv = 0;
		vgip_chn2_ctrl_reg.ch2_hs_inv = 0;
		vgip_chn2_ctrl_reg.ch2_safe_mode = 0;
		vgip_chn2_ctrl_reg.ch2_vs_by_hs_en_n = 0;
		vgip_chn2_ctrl_reg.ch2_fdrop_en = 0;
		IoReg_Write32(SUB_VGIP_VGIP_CHN2_CTRL_reg, vgip_chn2_ctrl_reg.regValue);


		// Disable On-Line Measure
		onms_onms2_ctrl_reg.regValue = IoReg_Read32(ONMS_onms2_ctrl_reg);
		onms_onms2_ctrl_reg.on2_sycms_clk = 0;
		onms_onms2_ctrl_reg.on2_vs_meas_inv= 0;
		onms_onms2_ctrl_reg.on2_ms_conti= 0;
		onms_onms2_ctrl_reg.on2_start_ms= 0;

		IoReg_Write32(ONMS_onms2_ctrl_reg, onms_onms2_ctrl_reg.regValue);

		//restore Hsync/Vsync delay to zero

		vgip_chn2_delay_reg.regValue = IoReg_Read32(SUB_VGIP_VGIP_CHN2_DELAY_reg);
		vgip_chn2_delay_reg.ch2_ihs_dly = 0;
		vgip_chn2_delay_reg.ch2_ivs_dly = 0;
		IoReg_Write32(SUB_VGIP_VGIP_CHN2_DELAY_reg, vgip_chn2_delay_reg.regValue);

		// Disable UZU double buffer
		scaleup_ds_uzu_db_ctrl_Reg.regValue = IoReg_Read32(SCALEUP_DS_UZU_DB_CTRL_reg);
		scaleup_ds_uzu_db_ctrl_Reg.db_en = 0;
		IoReg_Write32(SCALEUP_DS_UZU_DB_CTRL_reg, scaleup_ds_uzu_db_ctrl_Reg.regValue);

		drvif_scaler_sub_mdomain_switch_irq(_DISABLE); //close sub m domain interrrupt
	}
#endif
#if 0
	Scaler_DispSetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_INTERLACE,FALSE);
	Scaler_DispSetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_THRIP,FALSE);
	Scaler_DispSetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_10BIT,FALSE);

	// CSW+ 0970630 Reset frame sync flag
	Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_FRAMESYNC,FALSE);
#endif

#ifdef CONFIG_FS_HS_BIAS
	drvif_framesync_hs_bias_release();
#endif

#ifdef CONFIG_5_4_FS
	drvif_framesync_5_4_release();
#endif

#if 1	/* move to vsc disconnect*/
	drv_memory_I_De_XC_DMA_Memory_Get_From_VBM(0, 0, 0, 1);
	drv_memory_Set_I_De_XC_Mem_Ready_Flag(0, 0);
	drv_memory_I_De_XC_DMA_Memory_Get_From_VBM(1, 0, 0, 1);
	drv_memory_Set_I_De_XC_Mem_Ready_Flag(1, 0);
	scalerVIP_HDRCID_Reset();
#endif
	/* for PQ IP Enable monitor*/
	// use vpq tsk to monitor, use lv control, buff control is not neccessary
	//if(SLR_MAIN_DISPLAY == display)
		//Scaler_PQ_IP_Enable_Monitor_clear_Buff();
	if(SLR_MAIN_DISPLAY == display) {
		Scaler_color_Set_PQ_ByPass_Lv(255);
		drvif_color_set_DI_detection(0, 0 ,0);
	}
	if (SLR_SUB_DISPLAY == display)	// only sum path need to disable SDNR2
		drvif_color_Set_DRV_SNR_Clock(SLR_SUB_DISPLAY, 0);
	//Set BFI flag off
	fwif_color_set_BFI_En_by_InputFrameRate(_DISABLE);
	//Scaler_Disp3dSetInfo(SLR_DISP_3D_3DTYPE,SLR_3D_2D_ONLY);
	//Scaler_Set3DMode_Attr(SLR_3DMODE_2D);

	//For dolby vision
	reset_dolbyVision_HDMI_video_parameter();
}
#endif // #ifndef BUILD_QUICK_SHOW

static unsigned char drvif_hdmi_mode_customer_judge_confuse_mode(unsigned char mode_index)
{
	unsigned short IHCount = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IHCOUNT);
	unsigned short IVCount = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVCOUNT);
	unsigned int JudgeResult = TRUE;
	unsigned int preset_table_size = sizeof(tINPUTMODE_PRESET_TABLE)/sizeof(ModeTableType);
	unsigned int hdmi_table_size = sizeof(tINPUTMODE_HDMI_TABLE)/sizeof(ModeTableType);

	if(_TRUE == get_hdmi_idntify_mode()) // HDMI mode
	{
//		rtd_pr_hdmi_info("drvif_hdmi_mode_customer_judge_confuse_mode, HDMI %x.....\n", tINPUTMODE_HDMI_TABLE[mode_index].ModeID);
		if (mode_index >= hdmi_table_size) {
			return FALSE;
		}
		if(ABS(IHCount, tINPUTMODE_HDMI_TABLE[mode_index].IHTotal) > 10)
		{
//			JudgeResult = FALSE;
			return _FALSE;
		}
		else if(ABS(IVCount, tINPUTMODE_HDMI_TABLE[mode_index].IVTotal) > 10)
		{
//			JudgeResult = FALSE;
			return _FALSE;
		}

//		rtd_pr_hdmi_info("HDMI JudgeResult : %x\n", JudgeResult);
		return JudgeResult;
	}

	if (mode_index >= preset_table_size) {
			return FALSE;
	}
//	rtd_pr_hdmi_info("drvif_hdmi_mode_customer_judge_confuse_mode, DVI %x.....\n", tINPUTMODE_PRESET_TABLE[mode_index].ModeID);

	switch (tINPUTMODE_PRESET_TABLE[mode_index].ModeID)
	{
		case _MODE_1024x768_75HZ:
			//Lewis 2012/06/25, 1024x768x75 same as 1280x768x75
			if(IHCount > 1400) JudgeResult = FALSE;
		break;

		case _MODE_1280x768_75HZ:

		break;

		default:

		break;
	}

	return JudgeResult;
}

#ifndef BUILD_QUICK_SHOW 
void offline_semaphore_init(void)
{
	sema_init(&Offline_Semaphore, 1);
}

struct semaphore* get_offline_semaphore(void)
{
	return &Offline_Semaphore;
}
#endif

void clear_online_status(unsigned char display)
{
	onms_onms1_status_RBUS onms_onms1_status_reg;
	onms_onms2_status_RBUS onms_onms2_status_reg;
	onms_onms3_status_RBUS onms_onms3_status_reg;
	onms_onms4_status_RBUS onms_onms4_status_reg;

	if(display == SLR_MAIN_DISPLAY)
	{
		onms_onms4_status_reg.regValue = IoReg_Read32(ONMS_onms4_status_reg);
		IoReg_Write32(ONMS_onms4_status_reg, onms_onms4_status_reg.regValue);//Clear online dma status
		rtd_pr_onms_notice("####onms4 online error status:%08x, wd_en:%08x, int_en::%08x ####\n",
			onms_onms4_status_reg.regValue,
			IoReg_Read32(ONMS_onms4_watchdog_en_reg),
			IoReg_Read32(ONMS_onms4_interrupt_en_reg));

		onms_onms3_status_reg.regValue = IoReg_Read32(ONMS_onms3_status_reg);
		IoReg_Write32(ONMS_onms3_status_reg, onms_onms3_status_reg.regValue);//Clear online dma status
		rtd_pr_onms_notice("####dma online error status:%08x, wd_en:%08x, int_en::%08x ####\n",
			onms_onms3_status_reg.regValue,
			IoReg_Read32(ONMS_onms3_watchdog_en_reg),
			IoReg_Read32(ONMS_onms3_interrupt_en_reg));

		onms_onms1_status_reg.regValue = IoReg_Read32(ONMS_onms1_status_reg);
		IoReg_Write32(ONMS_onms1_status_reg, onms_onms1_status_reg.regValue);//Clear online main status
		rtd_pr_onms_notice("####main online error status:%08x, wd_en:%08x, int_en::%08x ####\n",
			onms_onms1_status_reg.regValue,
			IoReg_Read32(ONMS_onms1_watchdog_en_reg),
			IoReg_Read32(ONMS_onms1_interrupt_en_reg));
	}
	else if(display == SLR_SUB_DISPLAY)
	{
		onms_onms2_status_reg.regValue = IoReg_Read32(ONMS_onms2_status_reg);
		IoReg_Write32(ONMS_onms2_status_reg, onms_onms2_status_reg.regValue);//Clear online sub status
		rtd_pr_onms_notice("####sub online error status:%08x, wd_en:%08x, int_en::%08x ####\n",
			onms_onms2_status_reg.regValue,
			IoReg_Read32(ONMS_onms2_watchdog_en_reg),
			IoReg_Read32(ONMS_onms2_interrupt_en_reg));;
	}
}

static unsigned int record_Sub_online_enable_time = 0;//record TIMER_SCPU_CLK90K_LO_reg after enable sub online measure
static unsigned int record_Main_online_enable_time = 0;//record TIMER_SCPU_CLK90K_LO_reg after enable Main online measure

void save_online_enable_time(unsigned char display)
{//the api is use to save the 90k counter online measure enable time
	if(display == SLR_MAIN_DISPLAY)
		record_Main_online_enable_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);//save online measure satrt time for check_online_measure_result_unstable check
	else
		record_Sub_online_enable_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);//save online measure satrt time for check_online_measure_result_unstable check
}

void wait_ONMS_ready(unsigned char display, unsigned int vfreq)
{//this api is for wait online measure ready to pop frame rate result. frame_rate is *1000
	#define ONLINE_READY_WAIT_FRAME_NUM 3
	unsigned int enable_time;
	unsigned int cur_counter;
	unsigned int need_time;
	unsigned char timeout = 10;

	if(display == SLR_MAIN_DISPLAY)
		enable_time = record_Main_online_enable_time;
	else
		enable_time = record_Sub_online_enable_time;

	if(vfreq)
		need_time = 90000000 * ONLINE_READY_WAIT_FRAME_NUM / vfreq;//wait 3 frame
	else
	{
		rtd_pr_onms_err("##func:%s err frame rate zero###\r\n", __FUNCTION__);
		return;
	}

	rtd_pr_onms_notice("###func:%s display:%d ori:0x%x needwait time:0x%x##\r\n", __FUNCTION__, display, enable_time, need_time);

	while(timeout)
	{
		cur_counter = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);
		if(cur_counter < enable_time)
		{
			if((cur_counter + (0xFFFFFFFF - enable_time)) >= need_time)
				break;
		}
		else if ((cur_counter - enable_time) >= need_time)
				break;
		msleep(10);
		timeout --;
	}
	if(!timeout)
		rtd_pr_onms_err("###func:%s [display:%d] err wait timeout ###\r\n", __FUNCTION__, display);
}

#ifndef BUILD_QUICK_SHOW
unsigned char check_online_result(unsigned char display, unsigned char online_path)
{//return false: error
	VSC_INPUT_TYPE_T src;
	unsigned int onlne_frame_rate = 0;
	unsigned int offline_frame_rate;
	src = Get_DisplayMode_Src(display);

	if(src != VSC_INPUTSRC_HDMI)
		return TRUE;

	if(vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag() || get_scaler_qms_mode_flag())
		return TRUE;//vrr no check

	offline_frame_rate = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVFREQ_1000);

	onlne_frame_rate = get_hdmi_online_freq(display, online_path);

	if(onlne_frame_rate >= offline_frame_rate)
	{
		if(onlne_frame_rate >= offline_frame_rate + 1000)
		{
			rtd_pr_onms_info("\r## check_online_result fail (on:%d off:%d) ###\r\n", onlne_frame_rate, offline_frame_rate );
			return  FALSE;
		}
	}
	else
	{
		if(offline_frame_rate >= onlne_frame_rate + 1000)
		{
			rtd_pr_onms_info("\r## check_online_result fail (on:%d off:%d) ###\r\n", onlne_frame_rate, offline_frame_rate );
			return  FALSE;
		}
	}

	return TRUE;
}
#endif // #ifndef BUILD_QUICK_SHOW

unsigned int get_hdmi_online_freq(unsigned char display, unsigned char online_path)
{
	unsigned int onlne_frame_rate = 0;
	VSC_INPUT_TYPE_T src;
	src = Get_DisplayMode_Src(display);
	if(src != VSC_INPUTSRC_HDMI)
		return TRUE;

	switch(online_path)
	{
		case VGIP_PATH_I3:
		{
			onms_onms3_demode_vs_period_RBUS onms_onms3_demode_vs_period_reg;
			onms_onms3_demode_vs_period_reg.regValue = IoReg_Read32(ONMS_onms3_Demode_Vs_period_reg);
			if(onms_onms3_demode_vs_period_reg.on3_de_vs_period)
			{
				onlne_frame_rate = (unsigned int)( ( 27000000000UL + (unsigned long long)( onms_onms3_demode_vs_period_reg.on3_de_vs_period / 2 ) ) / (unsigned long long)onms_onms3_demode_vs_period_reg.on3_de_vs_period );
			}
			break;
		}

		case VGIP_PATH_I1:
		case VGIP_PATH_I5:
		{
			onms_onms1_demode_vs_period_RBUS onms_onms1_demode_vs_period_reg;
			onms_onms1_demode_vs_period_reg.regValue = IoReg_Read32(ONMS_Onms1_Demode_Vs_period_reg);
			if(onms_onms1_demode_vs_period_reg.on1_de_vs_period)
			{
				onlne_frame_rate = (unsigned int)( ( 27000000000UL + (unsigned long long)( onms_onms1_demode_vs_period_reg.on1_de_vs_period / 2 ) ) / (unsigned long long)onms_onms1_demode_vs_period_reg.on1_de_vs_period );
			}
			break;
		}

		case VGIP_PATH_I2:
		case VGIP_PATH_I6:
		{
			onms_onms2_demode_vs_period_RBUS onms_onms2_demode_vs_period_reg;
			onms_onms2_demode_vs_period_reg.regValue = IoReg_Read32(ONMS_Onms2_Demode_Vs_period_reg);
			if(onms_onms2_demode_vs_period_reg.on2_de_vs_period)
			{
				onlne_frame_rate = (unsigned int)( ( 27000000000UL + (unsigned long long)( onms_onms2_demode_vs_period_reg.on2_de_vs_period / 2 ) ) / (unsigned long long)onms_onms2_demode_vs_period_reg.on2_de_vs_period );
			}
			break;
		}

		case VGIP_PATH_I4:
		{
			onms_onms4_demode_vs_period_RBUS onms_onms4_demode_vs_period_reg;
			onms_onms4_demode_vs_period_reg.regValue = IoReg_Read32(ONMS_onms4_Demode_Vs_period_reg);
			if(onms_onms4_demode_vs_period_reg.on4_de_vs_period)
			{
				onlne_frame_rate = (unsigned int)((27000000000UL + (unsigned long long)(onms_onms4_demode_vs_period_reg.on4_de_vs_period / 2)) / (unsigned long long)onms_onms4_demode_vs_period_reg.on4_de_vs_period);
			}
			break;
		}

		default:
			break;

	}
	return onlne_frame_rate;
}

