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
#include <linux/pageremap.h>
#include <linux/semaphore.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/spinlock_types.h>/*For spinlock*/
//#include <mach/io.h>
#endif
//RBUS Header file
#include <rbus/mdomain_cap_reg.h>
#include <rbus/mdomain_disp_reg.h>
#include <rbus/smartfit_reg.h>
//#include <rbus/sync_processor_reg.h>
#include <rbus/vgip_reg.h>
#include <rbus/sub_vgip_reg.h>
#include <rbus/dma_vgip_reg.h>
#include <rbus/ppoverlay_reg.h>
//#include <rbus/dds_pll_reg.h>
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

#include <tvscalercontrol/vip/nr.h>
#include <tvscalercontrol/vip/di_ma.h>
#include <tvscalercontrol/vip/scalerColor.h>
#include <tvscalercontrol/hdmirx/hdmifun.h>
#include <tvscalercontrol/adcsource/ypbpr.h>
#include <tvscalercontrol/pcbsource/pcbSource.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <tvscalercontrol/adcsource/vga.h>
#include <tvscalercontrol/scalerdrv/scaler_i2rnd.h>
#ifdef ENABLE_DRIVER_I3DDMA
#include <tvscalercontrol/i3ddma/i3ddma.h>
#include <tvscalercontrol/i3ddma/i3ddma_drv.h>
#endif
#include <tvscalercontrol/scaler/vipinclude.h>
#include <tvscalercontrol/adcsource/adcctrl.h>
#include <scaler_vbedev.h>
#include <scaler_vscdev.h>
#include <scaler_vpqmemcdev.h>
#include <tvscalercontrol/hdmirx/hdmi_vfe.h>
#include <tvscalercontrol/vo/rtk_vo.h>
#include <rbus/mdomain_vi_main_reg.h>
#include <tvscalercontrol/scaler/scalercolorlib.h>
#ifndef BUILD_QUICK_SHOW
#include <tvscalercontrol/scalerdrv/scaler_hdr_ctrl.h>
#include <tvscalercontrol/vdc/video.h>
#else
#include <sysdefs.h>
#include <mach/rtk_platform.h>
#include <no_os/printk.h>
#include <no_os/semaphore.h>
#include <timer.h>
#endif
//#include <rbus/adc_reg.h>
//#include <rbus/ptol_reg.h>  //fix by jinjun_gong
#include <rtd_log/rtd_module_log.h>

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

#ifndef UT_flag
extern unsigned int current_hdmi_onms_watchdog_setting;
extern unsigned char current_hdmi_onms_is_i3ddma;
#endif // UT_flag

#define GET_USER_INPUT_SRC() (Scaler_GetUserInputSrc(SLR_MAIN_DISPLAY))


static struct semaphore Offline_Semaphore;

//static unsigned short HDMIDiditalMeasureIVH;

//static ModeTableType *tINPUTMODE_HDMI_TABLE = NULL;
//static ModeTableType *tINPUTMODE_PRESET_TABLE = NULL;
//static ModeTableType *tINPUTMODE_YPBPR_TABLE = NULL;
static StructModeUserFIFODataType *tINPUTMODE_USERMODE_TABLE = NULL;
//static unsigned char *not_support_timing_array = NULL;
static unsigned char *user_fifo_mode = NULL;
//static ModeSkipTimingType *skip_timing_array = NULL;
static unsigned char hdmi_mode_flag = 0;
unsigned char HsyncPolarityIndicator = 0;

static void (*p_mode_customer_extra_condition)(unsigned char sync_type, unsigned char mode_index) = NULL;
static unsigned char (*p_mode_customer_judge_confuse_mode)(unsigned char mode_index, unsigned char sync_type, unsigned short usHighPulsePeriod) = NULL;
static unsigned char (*p_mode_customer_user_def_confuse_mode)(unsigned char mode_index) = NULL;
static unsigned char (*p_mode_customer_user_def_unsopport_mode)(unsigned char mode_index) = NULL;

static unsigned char (*p_hdmi_mode_customer_judge_confuse_mode)(unsigned char mode_index) = NULL;
static unsigned char (*p_mode_customer_user_def_modetable_size)(unsigned char mode) = NULL;
static unsigned char (*p_mode_customer_user_def_modetable_acceptive_size)(void) = NULL;

#ifdef CONFIG_SUPPORT_SRC_ADC
extern unsigned char get_ADC_Input_Source(void);
#ifdef CONFIG_SUPPORT_SRC_VGA
static unsigned char drvif_mode_customer_judge_confuse_mode(UINT8 mode_index, UINT8 sync_type, UINT16 usHighPulsePeriod);
#endif//#ifdef CONFIG_SUPPORT_SRC_VGA
#endif//#ifdef CONFIG_SUPPORT_SRC_ADC

static unsigned char drvif_hdmi_mode_customer_judge_confuse_mode(UINT8 mode_index);
void drvif_mode_print_mode_ID(unsigned char mode);
#ifdef CONFIG_I2RND_ENABLE
extern unsigned char check_Sub_I2rnd_done(void);
extern unsigned char vsc_i2rnd_dualvo_sub_stage;
#endif

#define NO_DVS_INFO_MAX 6

#ifdef ENABLE_DRIVER_I3DDMA
#define SR_MAX_YPBPR_MODE						15
// no overscan mode table
static ModeTableType tINPUTMODE_YPBPR_TABLE_3D[SR_MAX_YPBPR_MODE] =
{
	{
	// Mode 83 : 480i
		_MODE_480I,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 242,														// InputWidth, InputHeight,
		157, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		858, 262,														// HTotal, VTotal,
		70, 14,														// HStartPos, VStartPos,
	},

	{   // Mode 84 : 576i
		_MODE_576I,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 288,														// InputWidth, InputHeight,
		156, 500,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		864, 312,														// HTotal, VTotal,
		80, 20,														// HStartPos, VStartPos,
	},

	{   // Mode 85 : 480p

		_MODE_480P,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 482,														// InputWidth, InputHeight,
		315, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		858, 525,														// HTotal, VTotal,
		70, 34,														// HStartPos, VStartPos,
	},

	{   // Mode 86 : 576p
		_MODE_576P,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 576,														// InputWidth, InputHeight,
		312, 500,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		864, 624,														// HTotal, VTotal,
		80, 42,														// HStartPos, VStartPos,
	},

	{   // Mode 87 : 720p50

		_MODE_720P50,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		376, 502,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1980, 750,													// HTotal, VTotal,
		246, 22,														// HStartPos, VStartPos,
	},

	{   // Mode 88 : 720p60
		_MODE_720P60,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		450, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1650, 750,													// HTotal, VTotal,
		244,22,														// HStartPos, VStartPos,
	},

	{   // Mode 89: 1080i25
		_MODE_1080I25,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 540,													// InputWidth, InputHeight,
		281, 500,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2640, 562,													// HTotal, VTotal,
		186 - 4, 18,														// HStartPos, VStartPos,
	},

	{   // Mode 90 : 1080i30
		_MODE_1080I30,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 540,												// InputWidth, InputHeight,
		337, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2200, 562,													// HTotal, VTotal,
		182, 16 + 2,														// HStartPos, VStartPos,
	},

	{   // Mode 91 : 1080p50
		_MODE_1080P50,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		563, 501,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2640, 1125,													// HTotal, VTotal,
		178, 38,														// HStartPos, VStartPos,
	},

	{   // Mode 92 : 1080p60
		_MODE_1080P60,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,//1026+48+4+2+2,													// InputWidth, InputHeight,
		675, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2200, 1125,													// HTotal, VTotal,
		180,38,														// HStartPos, VStartPos,
	},

	// CSW+ 0971225 Add YPbPr timing 1080P/25HZ/24HZ/23.97HZ/29.97HZ/30HZ
	{   // Mode 93 : 1080p23 (23.976 Hz)
		_MODE_1080P23,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x010d, 0x00ef,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0ABE, 0x0465,												// HTotal, VTotal,
		182, 39,											// HStartPos, VStartPos,
	},

	{   // Mode 94 : 1080p24 (24 Hz)
		_MODE_1080P24,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x010e, 0x00f0,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, 5,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0ABE, 0x0465,												// HTotal, VTotal,
		198, 43,												// HStartPos, VStartPos,
	},

	{   // Mode 95 : 1080p25 (25 Hz)
		_MODE_1080P25,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x0119, 0x00fa,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, 5,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0A50, 0x0465,												// HTotal, VTotal,
		184, 39,												// HStartPos, VStartPos,
	},

	{   // Mode 96 : 1080p29 (29.976 Hz)
		_MODE_1080P29,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x0151, 0x012b,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0898, 0x0465,												// HTotal, VTotal,
		182, 39,												// HStartPos, VStartPos,
	},

	{   // Mode 97 : 1080p30 (30 Hz)
		_MODE_1080P30,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x0151, 0x012c,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0898, 0x0465,												// HTotal, VTotal,
		182, 39,												// HStartPos, VStartPos,
	},
};


typedef struct _YPBPR_3D_POS_ADJ_INFO {

    UINT8        timing;
    INT8          h_start;
    INT8          v_start;
} YPBPR_3D_POS_ADJ_INFO;

static YPBPR_3D_POS_ADJ_INFO m_Adj_Pos_YPbPr_3D[SR_MAX_YPBPR_MODE] = {
    {_MODE_480I		, -4	, 1},
    {_MODE_480P		, -2	, 0},
    {_MODE_576I		, 0	, 1},
    {_MODE_576P		, 0	, 1},
    {_MODE_720P50	, 0	, 1},
    {_MODE_720P60	, 2	, 1},
    {_MODE_1080I25	, 0	, 0},
    {_MODE_1080I30	, 0	, 0},
    {_MODE_1080P50	, 4	, 0},
    {_MODE_1080P60	, 2	, 0},
    {_MODE_1080P23	, 0	, 0},
    {_MODE_1080P24	, 0	, 0},
    {_MODE_1080P25	, 0	, 0},
    {_MODE_1080P29	, 0	, 0},
    {_MODE_1080P30	, 0	, 0}
};

int Mode_Get_ypbpr_3D_adjustment_hStart(unsigned int mode)
{
	mode -= _MODE_480I;
	if(mode < SR_MAX_YPBPR_MODE)
		return m_Adj_Pos_YPbPr_3D[mode].h_start;
	else{
		rtd_pr_adc_debug("ERR: OVER RANGE@%s\n", __FUNCTION__);
		return 0;
	}
}

int Mode_Get_ypbpr_3D_adjustment_vStart(unsigned int mode)
{
	mode -= _MODE_480I;
	if(mode < SR_MAX_YPBPR_MODE)
		return m_Adj_Pos_YPbPr_3D[mode].v_start;
	else{
		rtd_pr_adc_debug("ERR: OVER RANGE@%s\n", __FUNCTION__);
		return 0;
	}
}
#endif




static ModeTableType tINPUTMODE_YPBPR_TABLE[] =
{

	// if we move YPbPr mode to another place,
	// remember to change the mapping of mode numbe in CModeSetup()
	// function, because 480i & 576i should enter IP, others should not.

	{
	// Mode 83 : 480i
		_MODE_480I,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 240,														// InputWidth, InputHeight,
		157, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		858, 262,														// HTotal, VTotal,
		118, 16,														// HStartPos, VStartPos,
	},

	{   // Mode 84 : 576i
		_MODE_576I,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 288,														// InputWidth, InputHeight,
		156, 500,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		864, 312,														// HTotal, VTotal,
		127, 21,														// HStartPos, VStartPos,

	},

	{   // Mode 85 : 480p

		_MODE_480P,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 480,														// InputWidth, InputHeight,
		315, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		858, 525,														// HTotal, VTotal,
		118, 36,														// HStartPos, VStartPos,

	},

	{   // Mode 86 : 576p
		_MODE_576P,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 576,														// InputWidth, InputHeight,
		312, 500,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		864, 624,														// HTotal, VTotal,
		128, 42,														// HStartPos, VStartPos,

	},

	{   // Mode 87 : 720p50

		_MODE_720P50,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		376, 502,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1980, 750,													// HTotal, VTotal,
		295, 23,														// HStartPos, VStartPos,

	},

	{   // Mode 88 : 720p60
		_MODE_720P60,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		450, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1650, 750,													// HTotal, VTotal,
		295,23,														// HStartPos, VStartPos,
	},

	{   // Mode 89: 1080i25

		_MODE_1080I25,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 540,													// InputWidth, InputHeight,
		281, 500,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2640, 562,													// HTotal, VTotal,
		231, 18,														// HStartPos, VStartPos,

	},

	{   // Mode 90 : 1080i30

		_MODE_1080I30,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 540,												// InputWidth, InputHeight,
		337, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2200, 562,													// HTotal, VTotal,
		231, 18,														// HStartPos, VStartPos,

	},

	{   // Mode 91 : 1080p50

		_MODE_1080P50,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		563, 501,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2640, 1125,													// HTotal, VTotal,
		230, 39,														// HStartPos, VStartPos,

	},


	{   // Mode 92 : 1080p60

		_MODE_1080P60,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,//1026+48+4+2+2,													// InputWidth, InputHeight,
		675, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2200, 1125,													// HTotal, VTotal,
		230,39,														// HStartPos, VStartPos,
	},
	{
		_MODE_720P25,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		187, 250,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE, 					// HFreqTolerance in kHz, VFreqTolerance in Hz,
		3960, 750,													// HTotal, VTotal,
		295,23, 													// HStartPos, VStartPos,
	},
	{
		_MODE_720P30,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		225, 300,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE, 					// HFreqTolerance in kHz, VFreqTolerance in Hz,
		3300, 750,													// HTotal, VTotal,
		295,23, 													// HStartPos, VStartPos,
	},
	// CSW+ 0971225 Add YPbPr timing 1080P/25HZ/24HZ/23.97HZ/29.97HZ/30HZ
	{   // Mode 93 : 1080p23 (23.976 Hz)

		_MODE_1080P23,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x010d, 0x00ef,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0ABE, 0x0465,												// HTotal, VTotal,
		231, 39,											// HStartPos, VStartPos,

	},

	{   // Mode 94 : 1080p24 (24 Hz)

		_MODE_1080P24,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x010e, 0x00f0,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, 5,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0ABE, 0x0465,												// HTotal, VTotal,
		247, 43,												// HStartPos, VStartPos,

	},

	{   // Mode 95 : 1080p25 (25 Hz)

		_MODE_1080P25,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x0119, 0x00fa,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, 5,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0A50, 0x0465,												// HTotal, VTotal,
		231, 39,												// HStartPos, VStartPos,
	},

	{   // Mode 96 : 1080p29 (29.976 Hz)

		_MODE_1080P29,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x0151, 0x012b,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0898, 0x0465,												// HTotal, VTotal,
		231, 39,												// HStartPos, VStartPos,
	},

	{   // Mode 97 : 1080p30 (30 Hz)

		_MODE_1080P30,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x0151, 0x012c,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0898, 0x0465,												// HTotal, VTotal,
		231, 39,												// HStartPos, VStartPos,
	}
};

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
		315, 700,									// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		800, 449,									// IHTotal, IVTotal,
		144, 61,									// IHStartPos, IVStartPos,
	},

	{   // Mode 1 : 640 x 350 x 85 Hz
		//OK
		_MODE_640x350_85HZ,
		0 | _SYNC_HP_VN,							// Polarity Flags,
		640, 350,									// InputWidth, InputHeight,
		378, 850,									// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		832, 445,									// IHTotal, IVTotal,
		159,59,										// IHStartPos, IVStartPos,
	},

	{   // Mode 2 : 640 x 400 x 56 Hz
		//OK
		_MODE_640x400_56HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 400,													// InputWidth, InputHeight,
		248, 564,													// IHFreq in kHz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in kHz, IVFreqTolerance in Hz,
		848, 440,													// IHTotal, IVTotal,
		142+2, 24,													// IHStartPos, IVStartPos,
	},

	{   // Mode 3 : 640 x 400 x 70 Hz
		//OK
	//confuse with 720x400x70
		_MODE_640x400_70HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 400,													// InputWidth, InputHeight,
		314, 700,													// IHFreq in kHz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in kHz, IVFreqTolerance in Hz,
		800, 440,													// IHTotal, IVTotal,
		143, 36,													// IHStartPos, IVStartPos,
	},

	{   // Mode 4 : 640 x 400 x 85 Hz
		//OK
	//confuse with 720x400x85
		_MODE_640x400_85HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 400,													// InputWidth, InputHeight,
		380, 850,													// IHFreq in kHz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in kHz, IVFreqTolerance in Hz,
		831, 445,													// IHTotal, IVTotal,
		160, 44,													// IHStartPos, IVStartPos,
	},

	{   // Mode 5 : 720 x 400 x 50 Hz
		//OK
		_MODE_720x400_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,				// Polarity Flags,
		720, 400,									// InputWidth, InputHeight,
		314, 500,									// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		900, 629,									// IHTotal, IVTotal,
		162, 126,									// IHStartPos, IVStartPos,
	},

	{   // Mode 6 : 720 x 400 x 70 Hz
		//OK
	//confuse with 640x400x70
		_MODE_720x400_70HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP | _SYNC_HN_VN,		// Polarity Flags,
		720, 400,											// InputWidth, InputHeight,
		315, 700,											// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,					// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		900, 449,											// IHTotal, IVTotal,
		164, 36,											// IHStartPos, IVStartPos,
	},

	{   // Mode 7 : 720 x 400 x 85 Hz
		//OK
	//confuse with 640x400x85
		_MODE_720x400_85HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 400,													// InputWidth, InputHeight,
		380, 850,													// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE + 5, _VFREQ_TOLERANCE + 5,					// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		935, 446,													// IHTotal, IVTotal,
		179, 44,													// IHStartPos, IVStartPos,
	},

	{   // Mode 8 : 720 x 480 x 60 Hz
		//OK
		_MODE_720x480_60HZ,
		0 | _SYNC_HN_VN,							// Polarity Flags,
		1440, 480,									// InputWidth, InputHeight,
		315, 600,									// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1716, 525,									// IHTotal, IVTotal,
		244, 36,									// IHStartPos, IVStartPos,
	},

	{   // Mode 9 : 640 x 480 x 50 Hz
		//OK
		_MODE_640x480_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,				// Polarity Flags,
		640, 480,									// InputWidth, InputHeight,
		246, 496,									// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		800, 497,									// IHTotal, IVTotal,
		144, 13,									// IHStartPos, IVStartPos,
	},

	{   // Mode 10 : 640 x 480 x 60 Hz
		//OK
		_MODE_640x480_60HZ,
		0 | _SYNC_HN_VN,							// Polarity Flags,
		640, 480,									// InputWidth, InputHeight,
		315, 600,									// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		800, 525,									// IHTotal, IVTotal,
		144, 35,									// IHStartPos, IVStartPos,
	},

	{   // Mode 11 : 640 x 480 x 67 Hz
		//OK
		_MODE_640x480_67HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 480,													// InputWidth, InputHeight,
		350, 666,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		864, 525,													// HTotal, VTotal,
		159,38,														// HStartPos, VStartPos,
	},

	{   // Mode 12 : 640 x 480 x 70Hz
		//OK
		_MODE_640x480_70HZ,
		0 | _SYNC_HN_VN ,							// Polarity Flags,
		640, 480,									// InputWidth, InputHeight,
		360, 700,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		840, 515,									// HTotal, VTotal,
		167,27,										// HStartPos, VStartPos,
	},

	{   // Mode 13 : 640 x 480 x 72 Hz
		//OK
		_MODE_640x480_72HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 480,													// InputWidth, InputHeight,
		378, 728,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		832, 520,													// HTotal, VTotal,
		167,   27,													// HStartPos, VStartPos,
	},

	{   // Mode 14 : 640 x 480 x 75 Hz
		//OK
		_MODE_640x480_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 480,													// InputWidth, InputHeight,
		375, 750,													// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		840, 500,													// IHTotal, IVTotal,
		183,15,														// IHStartPos, IVStartPos,
	},

	{   // Mode 15 : 640 x 480 x 85 Hz
		//OK
		_MODE_640x480_85HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 480,													// InputWidth, InputHeight,
		433, 850,													// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		832, 510,													// IHTotal, IVTotal,
		135, 24,													// IHStartPos, IVStartPos,
	},

	{   // Mode 16 : 800 x 600 x 50 Hz
		//OK
		_MODE_800x600_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP ,				// Polarity Flags,
		800, 600,									// InputWidth, InputHeight,
		309, 499,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		992, 621,									// HTotal, VTotal,
		167,  17,									// HStartPos, VStartPos,
	},

	{   // Mode 17 : 800 x 600 x 56 Hz
		//OK
		_MODE_800x600_56HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		800, 600,													// InputWidth, InputHeight,
		351, 562,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1024, 625,													// HTotal, VTotal,
		199, 23,													// HStartPos, VStartPos,
	},

	{   // Mode 18 : 800 x 600 x 60 Hz
		//OK
		_MODE_800x600_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		800, 600,													// InputWidth, InputHeight,
		378, 603,													// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1056, 628,													// IHTotal, IVTotal,
		215,26,														// IHStartPos, IVStartPos,
	},

	{   // Mode 19 : 800 x 600 x 72 Hz
		//OK
		_MODE_800x600_72HZ,
		0 | _SYNC_HP_VP,											// Polarity Flags,
		800, 600,													// InputWidth, InputHeight,
		480, 720,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1040, 666,													// HTotal, VTotal,
		183,28,														// HStartPos, VStartPos,
	},

	{   // Mode 20 : 800 x 600 x 75 Hz
		//OK
		_MODE_800x600_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		800, 600,													// InputWidth, InputHeight,
		468, 750,													// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1056, 625,													// IHTotal, IVTotal,
		239,23,														// IHStartPos, IVStartPos,
	},

	{   // Mode 21 : 800 x 600 x 85 Hz
		//OK
		_MODE_800x600_85HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		800, 600,													// InputWidth, InputHeight,
		536, 850,													// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1048, 631,													// IHTotal, IVTotal,
		215, 29,													// IHStartPos, IVStartPos,
	},

	{   // Mode 22 : 832 x 624 x 75 Hz
		//OK
		_MODE_832x624_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN ,				// Polarity Flags,
		832, 624,									// InputWidth, InputHeight,
		497, 745,									// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1152, 667,									// IHTotal, IVTotal,
		287, 38,									// IHStartPos, IVStartPos,
	},

	{   // Mode 23 : 1024 x 768 x 50 Hz
		//OK
		_MODE_1024x768_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,				// Polarity Flags,
		1024, 768,									// InputWidth, InputHeight,
		396, 499,									// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1312,793,									// IHTotal, IVTotal,
		247,21,										// IHStartPos, IVStartPos,
	},

	{   // Mode 24 : 1024 x 768 x 60 Hz
		//OK
		_MODE_1024x768_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN,			// Polarity Flags,
		1024, 768,								// InputWidth, InputHeight,
		483, 600,								// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE-6, _VFREQ_TOLERANCE,	// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1344,806,								// IHTotal, IVTotal,
		295,28,									// IHStartPos, IVStartPos,
	},

	{   // Mode 25 : 1024 x 768 x 66 Hz
		//OK
		_MODE_1024x768_66HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN,			// Polarity Flags,
		1024, 768,								// InputWidth, InputHeight,
		539, 661,								// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,		// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1328,816,								// IHTotal, IVTotal,
		287,35,									// IHStartPos, IVStartPos,
	},

	{   // Mode 26 : 1024 x 768 x 70 Hz
		//OK
		_MODE_1024x768_70HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN,			// Polarity Flags,
		1024, 768,								// InputWidth, InputHeight,
		564, 700,								// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,		// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1328, 806,								// HTotal, VTotal,
		279,28,									// HStartPos, VStartPos,
	},

	{   // Mode 27 : 1024 x 768 x 75 Hz
		//OK
		_MODE_1024x768_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HP_VP,	// Polarity Flags,
		1024, 768,										// InputWidth, InputHeight,
		600, 750,										// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,				// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1312, 800,										// IHTotal, IVTotal,
		271,30,											// IHStartPos, IVStartPos,
	},

	{   // Mode 28 : 1024 x 768 x 85 Hz
		//OK
		_MODE_1024x768_85HZ,
		0 | _SYNC_HP_VP,											// Polarity Flags,
		1024, 768,													// InputWidth, InputHeight,
		687, 850,													// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1376, 808,													// IHTotal, IVTotal,
		304, 39,													// IHStartPos, IVStartPos,
	},

	{   // Mode 29 : 1152 x 864 x 60 Hz
		// MSPG-7800S T328 (1152x864/60)
		_MODE_1152x864_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1152, 864,													// InputWidth, InputHeight,
		543, 600,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1472,905,													// HTotal, VTotal,
		288,40,														// HStartPos, VStartPos,
	},

	{	// Mode 30 : 1152 x 864 x 75 Hz
		_MODE_1152x864_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1152, 864,													// InputWidth, InputHeight,
		675, 750,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE, 						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1600,900,													// HTotal, VTotal,
		384,35,														// HStartPos, VStartPos,
	},

	{   // Mode 34 : 1280 x 720 x 50 Hz
		//OK
		_MODE_1280x720_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,			// Polarity Flags,
		1280, 720,								// InputWidth, InputHeight,
		370, 498,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1632, 744,								// HTotal, VTotal,
		303,20,									// HStartPos, VStartPos,
	},

	{   // Mode 35 : 1280 x 720 x 60 Hz
		//OK
		_MODE_1280x720_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		447, 600,														// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1650, 746,													// HTotal, VTotal,
		330,24,														// HStartPos, VStartPos,
	},

	{   // Mode 36 : 1280 x 720 x 85 Hz
		//OK
		_MODE_1280x720_85HZ,
		0 | _SYNC_HP_VP ,						// Polarity Flags,
		1280, 720,								// InputWidth, InputHeight,
		643, 848,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1712, 759,								// HTotal, VTotal,
		351, 35,									// HStartPos, VStartPos,
	},

	{   // Mode 37 : 1280 x 768 x 50 Hz
//need check confuse with 1024x768x50, 3532
		_MODE_1280x768_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,			// Polarity Flags,
		1280, 768,								// InputWidth, InputHeight,
		395, 499,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1648, 793,								// HTotal, VTotal,
		312-4,24,									// HStartPos, VStartPos,
	},

	{   // Mode 38 : 1280 x 768 x 60 Hz
		//OK
		_MODE_1280x768_60HZ,
		0 | _SYNC_HN_VP|_SYNC_HN_VN|_SYNC_HP_VP|_SYNC_HP_VN,	// Polarity Flags,
		1280, 768,												// InputWidth, InputHeight,
		477, 600,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,					// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1664, 799,												// HTotal, VTotal,
		319, 26,													// HStartPos, VStartPos,
	},
	{   // Mode 39 : 1280 x 768 x 60 Hz_RB
		//OK
		_MODE_1280x768_60HZ_RB,
		0 | _SYNC_HN_VP|_SYNC_HN_VN|_SYNC_HP_VP|_SYNC_HP_VN,	// Polarity Flags,
		1280, 768,												// InputWidth, InputHeight,
		474, 600,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,					// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1440, 790,												// HTotal, VTotal,
		112, 19,													// HStartPos, VStartPos,
	},
	{   // Mode 40 : 1280 x 768 x 75 Hz
		//OK
		_MODE_1280x768_75HZ,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 768,								// InputWidth, InputHeight,
		601, 750,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1696, 801,								// HTotal, VTotal,
		335,33,									// HStartPos, VStartPos,
	},

	{   // Mode 41 : 1280 x 768 x 85 Hz
		//OK
		_MODE_1280x768_85HZ,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 768,								// InputWidth, InputHeight,
		686, 848,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1711, 809,								// HTotal, VTotal,
		351,37,									// HStartPos, VStartPos,
	},

	{   // Mode 42 : 1280 x 800 x 60 Hz
		_MODE_1280x800_60HZ,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 800,								// InputWidth, InputHeight,
		497, 598,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1680, 831,								// HTotal, VTotal,
		328, 28,									// HStartPos, VStartPos,
	},

	{   // Mode 43 : 1280 x 800 x 60 Hz (Reduce blanking)
		_MODE_1280x800_60HZ_RB,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 800,								// InputWidth, InputHeight,
		494, 601,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1440, 823,								// HTotal, VTotal,
		112, 18,									// HStartPos, VStartPos,
	},
//#ifdef IS_MACARTHUR
	{   // Mode 44 : 1280 x 800 x 75 Hz
		_MODE_1280x800_75HZ,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 800,								// InputWidth, InputHeight,
		628, 749,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1696, 838,								// HTotal, VTotal,
		336, 34,									// HStartPos, VStartPos,
	},
//#endif
	{   // Mode 45 : 1280 x 960 x 50 Hz
		//OK
		_MODE_1280x960_50HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,			// Polarity Flags,
		1280, 960,								// InputWidth, InputHeight,
		494, 498,									// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1680, 991,								// IHTotal, IVTotal,
		327,27,									// IHStartPos, IVStartPos,
	},

	{   // Mode 46 : 1280 x 960 x 60 Hz
		//OK
		_MODE_1280x960_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 960,													// InputWidth, InputHeight,
		600, 600,														// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1800, 1000,													// IHTotal, IVTotal,
		423,38,														// IHStartPos, IVStartPos,
	},

	{   // Mode 47 : 1280 x 960 x 75 Hz
		_MODE_1280x960_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 960,													// InputWidth, InputHeight,
		750, 750,														// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1728, 1005,													// HTotal, VTotal,
		360,42,														// HStartPos, VStartPos,
	},

	{   // Mode 48 : 1280 x 960 x 85 Hz
		//OK
		_MODE_1280x960_85HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,			// Polarity Flags,
		1280, 960,								// InputWidth, InputHeight,
		857, 848,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1728, 1011,								// HTotal, VTotal,
		358,47,									// HStartPos, VStartPos,
	},

	{   // Mode 49 : 1280 x 1024 x 50 Hz
		//OK
		_MODE_1280x1024_50HZ,
		0 | _SYNC_HP_VP ,						// Polarity Flags,
		1280, 1024,								// InputWidth, InputHeight,
		526, 498,									// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1680, 1057,								// IHTotal, IVTotal,
		327,29,									// IHStartPos, IVStartPos,
	},

	{   // Mode 50 : 1280 x 1024 x 60 Hz
		//OK
		_MODE_1280x1024_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 1024,													// InputWidth, InputHeight,
		640, 600,														// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1688, 1066,													// IHTotal, IVTotal,
		360, 41,														// IHStartPos, IVStartPos,
	},

	{   // Mode 51 : 1280 x 1024 x 72 Hz
		_MODE_1280x1024_72HZ,
		0|_SYNC_HP_VP,							// Polarity Flags,
		1280, 1024,								// InputWidth, InputHeight,
		780, 720,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1712, 1064,								// HTotal, VTotal,
		380,39,									// HStartPos, VStartPos,
	},

	{   // Mode 52 : 1280 x 1024 x 75 Hz
		//OK
		_MODE_1280x1024_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 1024,													// InputWidth, InputHeight,
		800, 750,														// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1688, 1066,													// IHTotal, IVTotal,
		391,40,														// IHStartPos, IVStartPos,
	},

	{   // Mode 53 : 1280 x 1024 x 85 Hz
		//OK
		_MODE_1280x1024_85HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,				// Polarity Flags,
		1280, 1024,									// InputWidth, InputHeight,
		911, 850,										// IHFreq in kHz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,		// IHFreqTolerance in kHz, IVFreqTolerance in Hz,
		1728, 1072,									// IHTotal, IVTotal,
		384, 47,										// IHStartPos, IVStartPos,
	},

	{   // Mode 54 : 1360 x 768 x 60HZ
		//OK
		_MODE_1360x768_60HZ,
		0 | _SYNC_HP_VP,								// Polarity Flags,
		1360, 768,									// InputWidth, InputHeight,
		475, 598,										// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE , _VFREQ_TOLERANCE ,		// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1792, 795,									// IHTotal, IVTotal,
		367, 23,										// HStartPos, VStartPos,
	},

//#if 1//VGA1366x768@60
	{   // Mode 55 : 1366 x 768 x 60HZ
		_MODE_1366x768_60HZ,
		0 | _SYNC_HP_VP | _SYNC_HP_VN| _SYNC_HN_VP | _SYNC_HN_VN,	// Polarity Flags,
		1366, 768,													// InputWidth, InputHeight,
		477, 598,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE , _VFREQ_TOLERANCE,						// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1792, 798,													// HTotal, VTotal,
		356,27,//358, 22,													// HStartPos, VStartPos,
    	},
//#endif

	{   // Mode 56 : 1440 x 900 x 60 Hz
		//OK
		_MODE_1440x900_60HZ,
		0 |_SYNC_HP_VP ,							// Polarity Flags,
		1440, 900,								// InputWidth, InputHeight,
		559, 598,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1904, 934,								// HTotal, VTotal,
		383, 30,									// HStartPos, VStartPos,
	},

	{   // Mode 57 : 1440 x 900 x 60 Hz (Reduced Blanking)
		//OK
		_MODE_1440x900_60RHZ,
		0 | _SYNC_HP_VN,							// Polarity Flags,
		1440, 900,								// InputWidth, InputHeight,
		555, 599,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1600, 926,								// HTotal, VTotal,
		111,16,									// HStartPos, VStartPos,
	},

	{   // Mode 58 : 1440 x 900 x 75 Hz
		//OK
		_MODE_1440x900_75HZ,
		0 | _SYNC_HN_VP |_SYNC_HP_VP ,			// Polarity Flags,
		1440, 900,								// InputWidth, InputHeight,
		706, 750,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1936, 942,								// HTotal, VTotal,
		399, 38,									// HStartPos, VStartPos,
	},

	{   // Mode 59 : 1440 x 900 x 85 Hz
		//OK
		_MODE_1440x900_85HZ,
		0 | _SYNC_HN_VP |_SYNC_HP_VP ,			// Polarity Flags,
		1440, 900,								// InputWidth, InputHeight,
		808, 853,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1952, 948,								// HTotal, VTotal,
		408, 45,									// HStartPos, VStartPos,
	},

	{   // Mode : 1600 x 900 x 60 Hz (Reduced Blanking)
		_MODE_1600x900_60HZ_RB,
		0 | _SYNC_HP_VN,							// Polarity Flags,
		1600, 900,								// InputWidth, InputHeight,
		600, 600,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1800, 1000,								// HTotal, VTotal,
		176,99,									// HStartPos, VStartPos,
	},

	{   // Mode 61 : 1600 x 1200 x 50 Hz
//need check clock, 3521
		_MODE_1600x1200_50HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1600, 1200,													// InputWidth, InputHeight,
		617, 499,														// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		2128, 1238,													// HTotal, VTotal,
		431,34,														// HStartPos, VStartPos,
	},

	{   // Mode 62 : 1600 x 1200 x 60 Hz
		//OK
		_MODE_1600x1200_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1600, 1200,													// InputWidth, InputHeight,
		750, 600,														// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		2160, 1250,													// HTotal, VTotal,
		495,48,														// HStartPos, VStartPos,
	},

	{   // Mode 63 : 1600 x 1200 x 65 Hz
		//OK
		_MODE_1600x1200_65HZ,
		0 | _SYNC_HP_VP,							// Polarity Flags,
		1600, 1200,								// InputWidth, InputHeight,
		812, 650,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		2160, 1250,								// HTotal, VTotal,
		494,48,									// HStartPos, VStartPos,
	},

	{   // Mode 64 : 1600 x 1200 x 70 Hz
		//OK
		_MODE_1600x1200_70HZ,
		0 | _SYNC_HP_VP,							// Polarity Flags,
		1600, 1200,								// InputWidth, InputHeight,
		879, 704,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		2160, 1250,								// HTotal, VTotal,
		496,49,									// HStartPos, VStartPos,
	},

	{   // Mode 65 : 1680 x 1050 x 50 Hz
//need check
#if 0
		0 | _SYNC_HP_VP ,						// Polarity Flags,
		1680, 1050,								// InputWidth, InputHeight,
		541, 499,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2206, 1083,								// HTotal, VTotal,
		440, 31,									// HStartPos, VStartPos,
#else
		_MODE_1680x1050_50HZ,
		0 | _SYNC_HP_VP ,						// Polarity Flags,
		1680, 1050,								// InputWidth, InputHeight,
		541, 499,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2216-10, 1083,								// HTotal, VTotal,
		440-4, 31,									// HStartPos, VStartPos,
#endif
	},

	{   // Mode 66 : 1680 x 1050 x 60 Hz
//confuse 1400x1050x60, 3344
		_MODE_1680x1050_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1680, 1050,										// InputWidth, InputHeight,
		653, 600,											// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,			// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2238, 1089,										// HTotal, VTotal,
		456, 36,											// HStartPos, VStartPos,
	},

	{   // Mode 67 : 1680 x 1050 x 60 Hz (Reduced Blanking)
//confuse 1400x1050x60, 3343
		_MODE_1680x1050_60RHZ,
		0 | _SYNC_HP_VN,							// Polarity Flags,
		1680, 1050,								// InputWidth, InputHeight,
		647, 600,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1840, 1080,								// HTotal, VTotal,
		112, 27,									// HStartPos, VStartPos,
	},

	{   // Mode 68 : 1680 x 1050 x 75 Hz
		//confuse 1400x1050x75, 3332
		_MODE_1680x1050_75HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1680, 1050,													// InputWidth, InputHeight,
		823, 750,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2272, 1099,													// HTotal, VTotal,
		472, 45,														// HStartPos, VStartPos,
	},

	{   // Mode 69 : 1680 x 1050 x 85 Hz
		//confuse 1400x1050x85, 3344
		_MODE_1680x1050_85HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1680, 1050,													// InputWidth, InputHeight,
		944, 855,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2288, 1105,													// HTotal, VTotal,
		480, 52,														// HStartPos, VStartPos,
	},

	{   // Mode 70 : 1400 x 1050 x 50 Hz
		//OK
		_MODE_1400x1050_50HZ,
		0 | _SYNC_HP_VP | _SYNC_HN_VP,			// Polarity Flags,
		1400, 1050,								// InputWidth, InputHeight,
		541, 499,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1848, 1083,								// HTotal, VTotal,
		367, 29,									// HStartPos, VStartPos,
	},

	{   // Mode 71 : 1400 x 1050 x 60 Hz (Reduced Blanking)
		//OK
		_MODE_1400x1050_60RHZ,
		0 | _SYNC_HP_VN,							// Polarity Flags,
		1400, 1050,								// InputWidth, InputHeight,
		647, 599,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1560, 1080,								// HTotal, VTotal,
		111,22,									// HStartPos, VStartPos,
	},

	{   // Mode 72 : 1400 x 1050 x 60 Hz
		//OK
		_MODE_1400x1050_60HZ,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,			// Polarity Flags,
		1400, 1050,								// InputWidth, InputHeight,
		653, 600,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1864, 1089,								// HTotal, VTotal,
		376, 36,									// HStartPos, VStartPos,
	},

	{   // Mode 73 : 1400 x 1050 x 75 Hz
		_MODE_1400x1050_75HZ,
		0 | _SYNC_HP_VP,							// Polarity Flags,
		1400, 1050,								// InputWidth, InputHeight,
		822, 748,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1896, 1099,								// HTotal, VTotal,
		391, 45,									// HStartPos, VStartPos,
	},
	{   // Mode 74 : 1920 x 1080 x 50 Hz
		//OK
		_MODE_1920x1080_50HZ,
		0 | _SYNC_HP_VN | _SYNC_HP_VP,			// Polarity Flags,
		1920, 1080,								// InputWidth, InputHeight,
		556, 499,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2544, 1114,								// HTotal, VTotal,
		510, 30,									// HStartPos, VStartPos,
	},
#if 0 //IC limit, unsupport clk > 162MHz
	{   // Mode 75 : 1920 x 1080 x 60 @173.1Mhz
		_MODE_1920x1080_60HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,        // Polarity Flags,
		1920, 1080,                                                                 // InputWidth, InputHeight,
		675, 600,                                                                     // HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                   // HFreqTolerance in kHz, VFreqTolerance in Hz,
		2576, 1120,                                                                 // HTotal, VTotal,
		533,36,                                                                       // HStartPos, VStartPos,
    },
#endif
    {   // Mode 76 : 1920 x 1080 x 60	@138.5Mhz XBOX360
	    	_MODE_1920x1080_60HZ_138,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,        // Polarity Flags,
		1920, 1080,                                                            // InputWidth, InputHeight,
		666, 600,                                                               // HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,             // HFreqTolerance in kHz, VFreqTolerance in Hz,
		2080, 1111,                                                           // HTotal, VTotal,
		112,25,                                                                 // HStartPos, VStartPos,
    },

    {   // Mode 77 : 1920 x 1080 x 60	@148.5Mhz(Reduced Blanking)
	    	_MODE_1920x1080_60HZ_148,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,        // Polarity Flags,
		1920, 1080,                                                            // InputWidth, InputHeight,
		675, 600,                                                               // HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,             // HFreqTolerance in kHz, VFreqTolerance in Hz,
		2200, 1125,                                                           // HTotal, VTotal,
		192,41,                                                                 // HStartPos, VStartPos,
    },


	{   // Mode 78 : 1920 x 1080 x 75 Hz
		_MODE_1920x1080_75HZ,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1920, 1080,								// InputWidth, InputHeight,
		846, 749,									// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2608, 1130,								// HTotal, VTotal,
		549, 46,									// HStartPos, VStartPos,
	},
	{   // Mode 81 : RGB 480p
		_MODE_VGA_480P,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		640, 480,														// InputWidth, InputHeight,
		315, 600,														// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
//?		800, 525,														// IHTotal, IVTotal,
		800-32-6, 525,														// IHTotal, IVTotal,
//?		146, 0x001F,													// HStartPos, VStartPos,
		107+1, 29,													// HStartPos, VStartPos,
	},

	{   // Mode 82 : RGB 576p
		_MODE_VGA_576P,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		1440, 576,													// InputWidth, InputHeight,
		312, 500,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1728, 625,														// HTotal, VTotal,
		264, 44,													// HStartPos, VStartPos,
	},

	{   // Mode 83: RGB 720px50Hz
		// MSPG-7800S T26 (1280x720P/50)
		_MODE_VGA_720P50,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		0x0500, 0x02d0,													// InputWidth, InputHeight,
		0x0177, 0x01f4,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1980, 750,														// HTotal, VTotal,
		260, 25,													// HStartPos, VStartPos,
	},

	{   // Mode 84 : RGB 720px60Hz
		// MSPG-7800S T22 (1280x720P/60)
		_MODE_VGA_720P60,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		0x0500, 0x02d0,													// InputWidth, InputHeight,
		0x01C2, 0x0258,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1650, 750,														// HTotal, VTotal,
		260, 25,													// HStartPos, VStartPos,
	},

	{   // Mode 35 : 1280 x 720 x 60 Hz
		// MSPG-7800S T303 (1280x720P/60)
		_MODE_1280x720_60HZ_RB,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		450, 600,														// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1650, 750,													// HTotal, VTotal,
		300,25,														// HStartPos, VStartPos,
	},

	{   // Mode 87 :  VGA 1080p50
		_MODE_VGA_1080P50,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 1080,                                                         // InputWidth, InputHeight,
        0x0232, 0x01f4,                                                           // IHFreq in 100Hz, VFreq in Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
        2640, 1125,                                                         // HTotal, VTotal,
        192, 41,                                                            // HStartPos, VStartPos,
    },

	{   // Mode 88 :  VGA 1080p60
    	_MODE_VGA_1080P60,
        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
        1920, 1080,                                                         // InputWidth, InputHeight,
        0x02a3, 0x0258,                                                           // IHFreq in 100Hz, VFreq in Hz,
        _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
        0x0898, 0x045e,                                                         // HTotal, VTotal,
        215, 30,                                                            // HStartPos, VStartPos,
    },

	{	// Mode : 848 x 480 x 60 Hz
		_MODE_848x480_60HZ,
		0 | _SYNC_HN_VN,							// Polarity Flags,
		848, 480,									// InputWidth, InputHeight,
		310, 600,									// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE, 		// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1064, 525,									// IHTotal, IVTotal,
		224, 31,									// IHStartPos, IVStartPos,
	},

//[Code Sync][AlanLi][0980527][1]
//add MAC timing
	{   // Mode 89 : 1024 x 576 x 60 Hz //for MAC
	    	_MODE_1024x576_60HZ,
		 0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
		 1024, 576,                                                                                                  // InputWidth, InputHeight,
		 630, 601,                                                                                                  // IHFreq in kHz, IVFreq in Hz,
		 _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                                // IHFreqTolerance in kHz, IVFreqTolerance in Hz,
		 1300, 599,                                                                                                // IHTotal, IVTotal,
		 195, 27,
   	},
//new interlace timing begin
	{   // Mode 85 : RGB 1080ix50Hz
		// MSPG-7800S T27 (1920x1080I/50)
		_MODE_VGA_1080I25,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		0x0780, 0x021c,													// InputWidth, InputHeight,
		0x0119, 0x01F4,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
 		0x0a50, /*0x022d*/ 0x232,													// HTotal, VTotal,
		192, 18,													// HStartPos, VStartPos,
	},

	{   // Mode 86 : RGB 1080ix60Hz
		// MSPG-7800S T23 (1920x1080I/60)
		_MODE_VGA_1080I30,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		0x0780, 0x021c,													// InputWidth, InputHeight,
		0x0151, 0x0258,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0898, /*0x022A*/ 0x0232,													// HTotal, VTotal,
		192, 0x0010,													// HStartPos, VStartPos,
	},
#if 0//480i/576i confuse with 1440x480i/576i  need set 1030[8:7]=0x3 , 2410[18:16]=0x0 , 2424[28]=0

	{   // Mode 79 : RGB 480i
		_MODE_VGA_480I,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		//		0x02d0, 0x0f0,													// InputWidth, InputHeight,
		720, 0x0f0,													// InputWidth, InputHeight,
		0x009d, 0x0258,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x035a, 262,														// HTotal, VTotal,
		//?		0x007c, 0x0010,													// HStartPos, VStartPos,
		0x007e, 0x000c,													// HStartPos, VStartPos,
	},

	{   // Mode 80 : RGB 576i
		_MODE_VGA_576I,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		0x02d0, 0x0121,													// InputWidth, InputHeight,
		0x009c, 0x01f4,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
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
		157, 600,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1716, 262,														// HTotal, VTotal,
		238, 18,													// HStartPos, VStartPos,
	},

	{
		// MSPG-7800S T30 (1440x576I/50)
		_MODE_VGA_576I,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		1440, 288,													// InputWidth, InputHeight,
		156, 500,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1728, 312,														// HTotal, VTotal,
		264, 20,													// HStartPos, VStartPos,
	},
#endif

	{//1024x768I/43.48Hz
		_MODE_1024x768I_43HZ,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,		// Polarity Flags,
		1024, 384,													// InputWidth, InputHeight,
		355, 870,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1264, 408,														// HTotal, VTotal,
		232, 24,													// HStartPos, VStartPos,
	},

	{	// Mode: 1280 x 1024 x 60 Hz
		_MODE_1280x1024_60HZ_MSPG,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 1024, 												// InputWidth, InputHeight,
		640, 600,														// IHFreq in 100Hz, IVFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE, 					// IHFreqTolerance in 100Hz, IVFreqTolerance in 0.1Hz,
		1708, 1056, 												// IHTotal, IVTotal,
		360, 41,														// IHStartPos, IVStartPos,
	},
	{   // Mode : 1280 x 768 x 50 Hz
		_MODE_1280x768_50HZ_MSPG,
		0 | _SYNC_HN_VP | _SYNC_HP_VP,			// Polarity Flags,
		1280, 768,								// InputWidth, InputHeight,
		395, 499,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1648, 791,								// HTotal, VTotal,
		312-4,24,									// HStartPos, VStartPos,
	},
	{   // Mode 38 : 1280 x 768 x 60 Hz
		//OK
		_MODE_1280x768_60HZ_MSPG,
		0 | _SYNC_HN_VP|_SYNC_HN_VN|_SYNC_HP_VP|_SYNC_HP_VN,	// Polarity Flags,
		1280, 768,												// InputWidth, InputHeight,
		477, 600,													// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,					// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1680, 795,												// HTotal, VTotal,
		319, 26,													// HStartPos, VStartPos,
	},

	{	// Mode  : 1280 x 768 x 75 Hz
		_MODE_1280x768_75HZ_MSPG,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 768,								// InputWidth, InputHeight,
		601, 750,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE, // HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		1612, 802,								// HTotal, VTotal,
		335,33, 								// HStartPos, VStartPos,
	},

	{   // Mode : 1280 x 768 x 85 Hz
		_MODE_1280x768_85HZ_MSPG,
		0 | _SYNC_HN_VP,							// Polarity Flags,
		1280, 768,								// InputWidth, InputHeight,
		686, 848,									// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,	// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
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
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 242,														// InputWidth, InputHeight,
		157, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		858, 262,														// HTotal, VTotal,
		70, 14,														// HStartPos, VStartPos,
	},

	{   // Mode 84 : 576i
		_MODE_576I,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 288,														// InputWidth, InputHeight,
		156, 500,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		864, 312,														// HTotal, VTotal,
		80, 20,														// HStartPos, VStartPos,
	},

	{   // Mode 85 : 480p

		_MODE_480P,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 482,														// InputWidth, InputHeight,
		315, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		858, 525,														// HTotal, VTotal,
		70, 34,														// HStartPos, VStartPos,
	},

	{   // Mode 86 : 576p
		_MODE_576P,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		720, 576,														// InputWidth, InputHeight,
		312, 500,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		864, 624,														// HTotal, VTotal,
		80, 42,														// HStartPos, VStartPos,
	},

	{   // Mode 87 : 720p50

		_MODE_720P50,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		376, 502,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1980, 750,													// HTotal, VTotal,
		246, 22,														// HStartPos, VStartPos,
	},

	{   // Mode 88 : 720p60
		_MODE_720P60,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1280, 720,													// InputWidth, InputHeight,
		450, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		1650, 750,													// HTotal, VTotal,
		244,22,														// HStartPos, VStartPos,
	},

	{   // Mode 89: 1080i25
		_MODE_1080I25,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 540,													// InputWidth, InputHeight,
		281, 500,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2640, 562,													// HTotal, VTotal,
		186 - 4, 18,														// HStartPos, VStartPos,
	},

	{   // Mode 90 : 1080i30
		_MODE_1080I30,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 540,												// InputWidth, InputHeight,
		337, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2200, 562,													// HTotal, VTotal,
		182, 16 + 2,														// HStartPos, VStartPos,
	},

	{   // Mode 91 : 1080p50
		_MODE_1080P50,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		563, 501,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2640, 1125,													// HTotal, VTotal,
		178, 38,														// HStartPos, VStartPos,

	},


	{   // Mode 92 : 1080p60
		_MODE_1080P60,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,//1026+48+4+2+2,													// InputWidth, InputHeight,
		675, 600,														// HFreq in kHz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in kHz, VFreqTolerance in Hz,
		2200, 1125,													// HTotal, VTotal,
		180,38,														// HStartPos, VStartPos,

	},

	// CSW+ 0971225 Add YPbPr timing 1080P/25HZ/24HZ/23.97HZ/29.97HZ/30HZ
	{   // Mode 93 : 1080p23 (23.976 Hz)
		_MODE_1080P23,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x010d, 0x00ef,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0ABE, 0x0465,												// HTotal, VTotal,
		182, 39,											// HStartPos, VStartPos,
	},

	{   // Mode 94 : 1080p24 (24 Hz)
		_MODE_1080P24,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x010e, 0x00f0,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, 5,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0ABE, 0x0465,												// HTotal, VTotal,
		198, 43,												// HStartPos, VStartPos,

	},

	{   // Mode 95 : 1080p25 (25 Hz)
		_MODE_1080P25,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x0119, 0x00fa,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, 5,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0A50, 0x0465,												// HTotal, VTotal,
		184, 39,												// HStartPos, VStartPos,

	},

	{   // Mode 96 : 1080p29 (29.976 Hz)
		_MODE_1080P29,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x0151, 0x012b,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0898, 0x0465,												// HTotal, VTotal,
		182, 39,												// HStartPos, VStartPos,
	},

	{   // Mode 97 : 1080p30 (30 Hz)
		_MODE_1080P30,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		1920, 1080,												// InputWidth, InputHeight,
		0x0151, 0x012c,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,						// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0898, 0x0465,												// HTotal, VTotal,
		182, 39,												// HStartPos, VStartPos,

	},


	{   // Mode 98 : 1080pm50(960x1080)
		_MODE_1080PM50,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP, 	// Polarity Flags,
		0x03A4, 0x0418,												// InputWidth, InputHeight,
		0x0232, 0x01f4,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x0528, 0x045e,												// HTotal, VTotal,
#if _HDMI_VGIP_SMART_FIT
		0x000e, 0x0010,												// HStartPos, VStartPos,
#else
		0x016E - 0x001, 0x0036,											// HStartPos, VStartPos,
#endif
	},


	{   // Mode 99 : 1080pm60(960x1080)
		_MODE_1080PM60,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,	// Polarity Flags,
		0x03A4, 0x0418,												// InputWidth, InputHeight,
		0x02a3, 0x0258,												// IHFreq in 100Hz, VFreq in Hz,
		_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,							// HFreqTolerance in 100Hz, VFreqTolerance in 0.1Hz,
		0x044c, 0x045e,												// HTotal, VTotal,
#if _HDMI_VGIP_SMART_FIT
		0x000e, 0x0010,												// HStartPos, VStartPos,
#else
		0x0af - 0x0026, 0x0036,											// HStartPos, VStartPos,
#endif
	},


// Support 4k2k
	{   // Mode 102 : 480p(1440x480)
 		_MODE_4k2kI30,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
       	3840,1080,															// InputWidth, InputHeight,
       	315, 600,                                                           // HFreq in kHz, VFreq in Hz,
       	_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in Hz,
       	4400, 1125,                                                           // HTotal, VTotal,
       	381, 42,															// HStartPos, VStartPos,
    },


	{   // Mode 118 //cloud modify 20150828
 		_MODE_4k2kP24,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
       	3840, 2160,															// InputWidth, InputHeight,
       	539, 240,                                                           // HFreq in kHz, VFreq in Hz,
       	_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in Hz,
       	5500, 2250,                                                           // HTotal, VTotal,
       	381, 81,															// HStartPos, VStartPos,
              },


	{   // Mode 103 : 480p(1440x480)
 		_MODE_4k2kP30,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
       	3840, 2160,															// InputWidth, InputHeight,
       	674, 300,                                                           // HFreq in kHz, VFreq in Hz,
       	_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in Hz,
       	4400, 2250,                                                           // HTotal, VTotal,
       	381, 81,															// HStartPos, VStartPos,
    },
	{   // Mode 119 //cloud modify 20150828
 		_MODE_4k2kP25,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
       	3840, 2160,															// InputWidth, InputHeight,
       	562, 250,                                                           // HFreq in kHz, VFreq in Hz,
       	_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in Hz,
       	5280, 2250,                                                           // HTotal, VTotal,
       	381, 81,															// HStartPos, VStartPos,
              },
              {   // Mode 120 //not modify
 		_MODE_4k2kP48,
		0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
       	0, 0,															// InputWidth, InputHeight,
       	562, 250,                                                           // HFreq in kHz, VFreq in Hz,
       	_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in Hz,
       	0, 0,                                                           // HTotal, VTotal,
       	381, 81,															// HStartPos, VStartPos,
              },
	{   // Mode 121 //cloud modify 20150828
 	_MODE_4k2kP50,
	0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
       	3840, 2160,															// InputWidth, InputHeight,
       	562, 500,                                                           // HFreq in kHz, VFreq in Hz,
       	_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in Hz,
       	5280, 2250,                                                           // HTotal, VTotal,
       	381, 81,															// HStartPos, VStartPos,
              },


	{   // Mode 116 : 4k2k (38400x2160)
 	_MODE_4k2kP60,
	0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
       	3840, 2160,															// InputWidth, InputHeight,
       	1349, 600,                                                           // HFreq in kHz, VFreq in Hz,
       	_HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in Hz,
       	4400, 2250,                                                           // HTotal, VTotal,
       	381, 81,															// HStartPos, VStartPos,
    	},

                {   // Mode 116 : 4k2k (38400x2160)(420)
                       _MODE_4k2kP60,
                        0 | _SYNC_HN_VN | _SYNC_HP_VN | _SYNC_HN_VP | _SYNC_HP_VP,          // Polarity Flags,
                       3840, 2160,                                                                                                                    // InputWidth, InputHeight,
                       675, 600,                                                           // HFreq in kHz, VFreq in Hz,
                       _HFREQ_TOLERANCE, _VFREQ_TOLERANCE,                                 // HFreqTolerance in kHz, VFreqTolerance in Hz,
                       4400, 2250,                                                           // HTotal, VTotal,
                       383, 81,                                                                                                                   // HStartPos, VStartPos,
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
 	unsigned int ivFreq = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVFREQ);
	unsigned int preset_table_size = sizeof(tINPUTMODE_PRESET_TABLE)/sizeof(ModeTableType);
	unsigned int hdmi_table_size = sizeof(tINPUTMODE_HDMI_TABLE)/sizeof(ModeTableType);

	if (get_hdmi_idntify_mode())
	{
		if (mode_index >= hdmi_table_size) {
			return FALSE;
		}
		if(ABS(ihFreq , tINPUTMODE_HDMI_TABLE[mode_index].IHFreq) > tINPUTMODE_HDMI_TABLE[mode_index].IHFreqTolerance)
			return FALSE;
		if(ABS(ivFreq , tINPUTMODE_HDMI_TABLE[mode_index].IVFreq) > tINPUTMODE_HDMI_TABLE[mode_index].IVFreqTolerance)
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
		if(ABS(ivFreq , tINPUTMODE_PRESET_TABLE[mode_index].IVFreq) > tINPUTMODE_PRESET_TABLE[mode_index].IVFreqTolerance)
			return FALSE;
	}

	return drvif_hdmi_mode_customer_judge_confuse_mode(mode_index);

 }

unsigned char get_adc_hsync_polarity_indicator(void)
{
	return HsyncPolarityIndicator;
}

unsigned char adc_mode_identifymode(int src, unsigned char mode_index, ModeInformationType* p_timinginfo)
{
#ifdef CONFIG_SUPPORT_SRC_ADC
 	unsigned int ihFreq = p_timinginfo->IHFreq;
 	unsigned int ivFreq = p_timinginfo->IVFreq;
#endif
#ifdef CONFIG_SUPPORT_SRC_VGA
	unsigned short usHighPulsePeriod = 0;
	sync_processor_sp1_stbrst1_RBUS  sync_processor_sp1_stbrst1_reg;
	unsigned char result;
#ifdef CONFIG_SUPPORT_SRC_ADC
	if(src == _SRC_VGA)
	{
		if((_ENABLE == Scaler_Get_VGA_Support_SOG()) && (SOG_MODE == VGA_Detect_SyncTypeMode()))
		{
			if(p_mode_customer_extra_condition) p_mode_customer_extra_condition(SOG_MODE, mode_index);
		}

		if(ABS(ihFreq , tINPUTMODE_PRESET_TABLE[mode_index].IHFreq) > tINPUTMODE_PRESET_TABLE[mode_index].IHFreqTolerance)
			return FALSE;
		if(ABS(ivFreq , tINPUTMODE_PRESET_TABLE[mode_index].IVFreq) > tINPUTMODE_PRESET_TABLE[mode_index].IVFreqTolerance)
			return FALSE;
		//VGA only use SP1
		if(VGA_SP_PATH == SYNC_PROCESSOR1)
		{
			sync_processor_sp1_stbrst1_reg.regValue = IoReg_Read32(SYNC_PROCESSOR_SP1_StbRst1_reg);
			usHighPulsePeriod = sync_processor_sp1_stbrst1_reg.s1_stable_plswid_out;
		}

		#if 0
		if(p_mode_customer_judge_confuse_mode)
		{
			return p_mode_customer_judge_confuse_mode(mode_index, VGA_Detect_SyncTypeMode(), usHighPulsePeriod);
		}
		#endif
		result = drvif_mode_customer_judge_confuse_mode(mode_index, VGA_Detect_SyncTypeMode(), usHighPulsePeriod);
		if(result){
			rtd_pr_adc_info("#####[%s(%d)]adc_mode_identifymode final mode_index = %d\n",__func__,__LINE__,mode_index);
		}
		return result;
	}
	else
#endif//#ifdef CONFIG_SUPPORT_SRC_VGA
		if(src == _SRC_YPBPR)
	{
			if(ABS(ihFreq , tINPUTMODE_YPBPR_TABLE[mode_index].IHFreq) > tINPUTMODE_YPBPR_TABLE[mode_index].IHFreqTolerance)
				return FALSE;
			if(ABS(ivFreq , tINPUTMODE_YPBPR_TABLE[mode_index].IVFreq) > tINPUTMODE_YPBPR_TABLE[mode_index].IVFreqTolerance)
				return FALSE;
 	}
	else
#endif
		return FALSE;

	return TRUE;
 }

/**
 * This function is used for mode searching
 *
 * @param : [input] None
 * @param st263xInfo->Mode_Curr: [output] Indicate the current mode.
 *
 */

unsigned char mode_searchmodetable(int src, StructDisplayInfo* p_dispinfo, ModeInformationType* p_timinginfo)
{
	unsigned char search_cnt = 0;
	unsigned short start_index = 0, end_index = 0;
	unsigned short mode_id;

	if ( src == _SRC_VGA){
		start_index =  0;
		#if 0
		if(p_mode_customer_user_def_modetable_size)
			end_index = p_mode_customer_user_def_modetable_size(0) - 1 ;//_MODE_VGA_1080I30;
		else
			end_index = 0;
		#else
		end_index = sizeof(tINPUTMODE_PRESET_TABLE)/sizeof(ModeTableType) -1;
		#endif

		rtd_pr_adc_info("#####[%s(%d)]start_index=%d  end_index=%d tabsize=%zu size=%zu\n",__func__,__LINE__,start_index,end_index,sizeof(tINPUTMODE_PRESET_TABLE),sizeof(ModeTableType));
		for(search_cnt = start_index; search_cnt <= end_index; search_cnt++){
			if(adc_mode_identifymode(src, search_cnt, p_timinginfo)){

				mode_id = tINPUTMODE_PRESET_TABLE[search_cnt].ModeID;

				switch(mode_id)
				{
					case _MODE_VGA_480I:
					case _MODE_VGA_576I:
					case _MODE_VGA_1080I25:
					case _MODE_VGA_1080I30:
						Scaler_SetDispStatusFromSource(p_dispinfo, SLR_DISP_INTERLACE,TRUE);
					break;
					default:
						Scaler_SetDispStatusFromSource(p_dispinfo, SLR_DISP_INTERLACE,FALSE);
					break;
				}

				//frank@1125 Mark below VGA SD timing go 10 bits flow to solve mantis#6837 3D mode fail
				//frank@08012012 SD enable bit10 path setting
				if(((mode_id >= _MODE_480I && mode_id <= _MODE_576P)||(mode_id <= _MODE_800x600_85HZ))/*&&(get_mainsubonpr_fuc()==FALSE)*/){
#ifdef ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE
					if(Scaler_Get3DMode((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)) == SLR_3DMODE_3D_SBS)
						Scaler_SetDispStatusFromSource(p_dispinfo, SLR_DISP_10BIT,FALSE);
					else
#endif
  					Scaler_SetDispStatusFromSource(p_dispinfo, SLR_DISP_10BIT,TRUE);
				}else
					Scaler_SetDispStatusFromSource(p_dispinfo, SLR_DISP_10BIT,FALSE);
#if TEST_PATTERN_GEN
				Scaler_SetDispStatusFromSource(p_dispinfo, SLR_DISP_INTERLACE,FALSE);
#endif
				rtd_pr_adc_info("VGA mode_searchmode OK,mode index=%d, mode id=%d\t=>\t",search_cnt,mode_id);
				drvif_mode_print_mode_ID(mode_id);
				rtd_pr_adc_info("Interlace mode =%x\n",Scaler_GetDispStatusFromSource(p_dispinfo,SLR_DISP_INTERLACE));

				p_timinginfo->ModeCurr = mode_id;
				return search_cnt;
			}
		}
	}
	else if ( src == _SRC_YPBPR){
		start_index =  0;
		end_index = sizeof(tINPUTMODE_YPBPR_TABLE)/sizeof(ModeTableType) -1;

		//rtd_pr_adc_info("#####[%s(%d)] start_index=%d  end_index=%d \n",__func__,__LINE__,start_index,end_index);

		for(search_cnt = start_index; search_cnt <= end_index; search_cnt++){
			if(adc_mode_identifymode(src, search_cnt, p_timinginfo)){
				mode_id = drvif_mode_get_mode_ID(search_cnt);//mode_id = tINPUTMODE_YPBPR_TABLE[search_cnt].ModeID;
				switch(mode_id)
				{
					case _MODE_480I:
					case _MODE_576I:
					case _MODE_1080I25:
					case _MODE_1080I30:
						Scaler_SetDispStatusFromSource(p_dispinfo, SLR_DISP_INTERLACE, TRUE);
					break;

					default:
						Scaler_SetDispStatusFromSource(p_dispinfo, SLR_DISP_INTERLACE, FALSE);
					break;
				}

				//frank@1125 Mark below YPbpr SD timing go 10 bits flow to solve mantis#6837 3D mode fail
				//frank@08012012 SD enable bit10 path setting

#ifdef ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE
				if(Scaler_Get3DMode((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)) == SLR_3DMODE_3D_SBS)
					Scaler_SetDispStatusFromSource(p_dispinfo, SLR_DISP_10BIT, FALSE);
				else
#endif
				if((mode_id >= _MODE_480I && mode_id <= _MODE_576P)/*&&(get_mainsubonpr_fuc()==FALSE)*/)
					Scaler_SetDispStatusFromSource(p_dispinfo, SLR_DISP_10BIT, TRUE);
				else
					Scaler_SetDispStatusFromSource(p_dispinfo, SLR_DISP_10BIT, FALSE);
#if TEST_PATTERN_GEN
				Scaler_SetDispStatusFromSource(p_dispinfo, SLR_DISP_INTERLACE, FALSE);
#endif


				//rtd_pr_adc_info("#####[%s(%d)] YPbPr mode_searchmode OK,mode index=%d mode id=%d\n",__func__, __LINE__, search_cnt, mode_id);
				//drvif_mode_print_mode_ID(mode_id);
				rtd_pr_adc_debug("Interlace mode =%x\n", Scaler_GetDispStatusFromSource(p_dispinfo, SLR_DISP_INTERLACE));

				p_timinginfo->ModeCurr = mode_id;
				return search_cnt;
			}
		}
	 }

	 rtd_pr_adc_emerg("#####[%s(%d)] No support in timing table \n",__func__,__LINE__);

	 return  _MODE_NOSUPPORT;
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
	unsigned char VsTimeOutFlg = 0;
	unsigned char timeoutcnt = 10;   // Tracking timeout 150ms
	unsigned short temp;
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

	offms_sp_ms3stus_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Stus_reg);
	offms_sp_ms3stus_reg.off_vs_per_to_long = 1;	//bit10
//	sp_ms3stus_reg.off_vs_pol_out = 1;		//bit9
//	sp_ms3stus_reg.off_hs_pol_out = 1;		//bit8
	offms_sp_ms3stus_reg.off_vs_per_to = 1;		//bit7
	offms_sp_ms3stus_reg.off_vs_high_to = 1;		//bit6
	offms_sp_ms3stus_reg.off_vs_per_of = 1;		//bit5
	/* MERLIN5-2207 remove */
	/* offms_sp_ms3stus_reg.off_vs_over_range = 1;	//bit4 */
	offms_sp_ms3stus_reg.off_vs_pol_chg = 1;		//bit3
	offms_sp_ms3stus_reg.off_hs_per_of = 1;		//bit2
	/* MERLIN5-2207 remove */
	/* offms_sp_ms3stus_reg.off_hs_over_range = 1;	//bit1 */
	offms_sp_ms3stus_reg.off_hs_pol_chg = 1;		//bit0
	IoReg_Write32(OFFMS_SP_MS3Stus_reg, offms_sp_ms3stus_reg.regValue);

	//set measure mode & start measure
	offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
	offms_sp_ms3ctrl_reg.off_sycms_mode = mode;
	offms_sp_ms3ctrl_reg.off_online_en = 0;
	if(CONFIG_OFFLINE_XTAL_FREQ == CONFIG_ONLINE_OFFLINE_XTAL_FREQ_27M)
		offms_sp_ms3ctrl_reg.off_clk_sel = 0;//27M
	else
		offms_sp_ms3ctrl_reg.off_clk_sel = 1;//24M
	offms_sp_ms3ctrl_reg.off_start_ms = _ENABLE;		//bit9	// enable offline measure
	IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);
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

		temp = (unsigned short) ((unsigned int) CONFIG_OFFLINE_XTAL_FREQ / (p_timinginfo->IHCount) / 100); //calcluate H Frequency in 100Hz unit.
		p_timinginfo->IHFreq = temp;
		temp = (unsigned short) ((unsigned int)(CONFIG_OFFLINE_XTAL_FREQ * 10) / ((unsigned int) (p_timinginfo->IHCount) * (p_timinginfo->IVCount)));//calcluate V Frequency in 1/10Hz unit.
		p_timinginfo->IVFreq = temp;
		//rtd_pr_offms_info("#####[%s(%d)] HSYNCPULSECOUNT:%d\n",__func__,__LINE__, p_timinginfo->IHSyncPulseCount);
		//rtd_pr_offms_info("#####[%s(%d)] IHCount:%d,IVCount:%d\n",__func__,__LINE__, p_timinginfo->IHCount, p_timinginfo->IVCount);
		//rtd_pr_offms_info("#####[%s(%d)] IHFreq:%d,IVFreq:%d\n",__func__,__LINE__, p_timinginfo->IHFreq, p_timinginfo->IVFreq);
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

	//rtd_pr_offms_info("#####[%s(%d)] =ulResult:%x=\n",__func__,__LINE__,offms_sp_ms3stus_reg.regValue);

	if (_OFFLINE_MS_SRC_RAWHV1 == source){
		HsyncPolarityIndicator = offms_sp_ms3stus_reg.off_hs_pol_out;
	}
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

extern unsigned char vsc_get_adaptivestream_flag(unsigned char display);

unsigned int drvif_mode_onms_int_setting(unsigned char channel, int src)
{
	unsigned int online_wd_int_setting = 0;

	if(src == _SRC_VO) {
		if(_CHANNEL1 == channel) {
			if(vsc_get_adaptivestream_flag(SLR_MAIN_DISPLAY)) {
				if(Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_FRAMESYNC))
					online_wd_int_setting = VO_CP_FS_ONLINE_ENABLE_MASK;
				else
					online_wd_int_setting = VO_CP_FRC_ONLINE_ENABLE_MASK;
			} else {
				if(Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_FRAMESYNC))
					online_wd_int_setting = VO_FS_ONLINE_ENABLE_MASK;
				else
					online_wd_int_setting = VO_FRC_ONLINE_ENABLE_MASK;
			}
		} else {
			online_wd_int_setting = VO_FRC_ONLINE_ENABLE_MASK;
		}
	}
	else if(src == _SRC_HDMI) {
		if(_CHANNEL1 == channel) {
			if(get_hdmi_4k_hfr_mode() == HDMI_NON_4K120) {//non 4k120 mode
				online_wd_int_setting = HDMI_VRR_ONLINE_ENABLE_MASK;
			} else {//4k120 mode
				online_wd_int_setting = HDMI_4K120_VRR_ONLINE_ENABLE_MASK;
			}
		} else {
			online_wd_int_setting = HDMI_NORMAL_ONLINE_ENABLE_MASK;
		}
	}
	else if(src == _SRC_MINI_DP) {
		if(_CHANNEL1 == channel) {
			online_wd_int_setting = DP_VRR_ONLINE_ENABLE_MASK;
		} else
			online_wd_int_setting = DP_NORMAL_ONLINE_ENABLE_MASK;
	}
#ifdef CONFIG_SUPPORT_SRC_ADC
	else if(src == _SRC_YPBPR  || src == _SRC_VGA) {
		if(get_ADC_Input_Source() == _SRC_VGA) {
			online_wd_int_setting = VGA_ONLINE_ENABLE_MASK;
		} else {
			online_wd_int_setting = YPP_ONLINE_ENABLE_MASK;
		}
	}
#endif
	else if(src == _SRC_CVBS) {
		online_wd_int_setting = AVD_ONLINE_ENABLE_MASK;
	}

	rtd_pr_onms_info("###func %s online_wd_int_setting:0x%x###\r\n",__FUNCTION__, online_wd_int_setting);

	return online_wd_int_setting;
}

void drvif_mode_onms_ctrl_setting(unsigned char channel, int src)
{
	onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;
	onms_onms1_hsync_meas_ctrl_RBUS onms_onms1_hsync_meas_ctrl_reg;
	onms_onms1_hsync_meas_ctrl_2_RBUS onms_onms1_hsync_meas_ctrl_2_reg;
	onms_onms1_hsync_meas_result_RBUS onms_onms1_hsync_meas_result_reg;
	onms_onms1_hs_period_RBUS onms_onms1_hs_period_reg;
	onms_onms1_vs_period_RBUS onms_onms1_vs_period_reg;
	onms_onms1_status_RBUS onms_onms1_status_reg;
	int timeout = 10;

	if(_CHANNEL1 == channel) {//main case new error status
		onms_onms1_ctrl_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_reg);
		if(onms_onms1_ctrl_reg.on1_start_ms) {
			onms_onms1_ctrl_reg.on1_popup_meas = 1;
			IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);
			onms_onms1_ctrl_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_reg);
			while(onms_onms1_ctrl_reg.on1_popup_meas && onms_onms1_ctrl_reg.on1_start_ms && timeout) {
				timeout--;
				msleep(10);
				onms_onms1_ctrl_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_reg);
			}
			onms_onms1_vs_period_reg.regValue = IoReg_Read32(ONMS_onms1_vs_period_reg);
			onms_onms1_hs_period_reg.regValue = IoReg_Read32(ONMS_onms1_hs_period_reg);

			//B802125c[30:24] = 8 (line diff)
			//B802125c[19:16] = 2 or 3 (line average)
			//B802125c[13:0] = hotal from b8021238[17:4]
			//B8021260[27:16] = source vtotal-10
			//B8021260[11:0] = 10 after Vsync line number, avoid clamp or macrovision
			onms_onms1_hsync_meas_ctrl_reg.regValue = IoReg_Read32(ONMS_onms1_hsync_meas_ctrl_reg);
#ifdef CONFIG_SUPPORT_SRC_ADC
			if (src == _SRC_YPBPR || src == _SRC_VGA){
				//ADC case setting is different
				onms_onms1_hsync_meas_ctrl_reg.on1_target_measure = Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_H_COUNT) + 1;
				onms_onms1_hsync_meas_ctrl_reg.on1_average_line = 3;
				switch(Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_MODE_CURR)){
					case _MODE_720P25:
					case _MODE_720P30:
					case _MODE_720P50:
					case _MODE_720P60:
					case _MODE_1080P24:
					case _MODE_1080P25:
					case _MODE_1080P30:
					case _MODE_1080I25:
					case _MODE_1080I30:
					case _MODE_1080P23:
					case _MODE_1080P29:
					case _MODE_1080P50:
					case _MODE_1080P60:
						onms_onms1_hsync_meas_ctrl_reg.on1_htotal_max_delta_new = 1;
						break;

					case _MODE_480I:
					case _MODE_576I:
					case _MODE_480P:
					case _MODE_576P:
					default:
						onms_onms1_hsync_meas_ctrl_reg.on1_htotal_max_delta_new = 4;
						break;
				}
			} else
#endif
			if ((src == _SRC_HDMI) || (src == _SRC_MINI_DP) || (src == _SRC_VO)) {
				onms_onms1_hsync_meas_ctrl_reg.on1_target_measure = onms_onms1_hs_period_reg.on1_htotal_h12b;
				onms_onms1_hsync_meas_ctrl_reg.on1_average_line = 3;
				onms_onms1_hsync_meas_ctrl_reg.on1_htotal_max_delta_new = 8;
			} else {
				onms_onms1_hsync_meas_ctrl_reg.on1_target_measure = onms_onms1_hs_period_reg.on1_htotal_h12b;
				if(src == _SRC_CVBS) {
					if(Scaler_DispGetInputInfo(SLR_INPUT_MODE_CURR) == _MODE_480I) {
						onms_onms1_hsync_meas_ctrl_reg.on1_average_line = 2;
						onms_onms1_hsync_meas_ctrl_reg.on1_htotal_max_delta_new = 5;
					} else {
						onms_onms1_hsync_meas_ctrl_reg.on1_average_line = 3;
						onms_onms1_hsync_meas_ctrl_reg.on1_htotal_max_delta_new = 4;
					}
				} else {
					onms_onms1_hsync_meas_ctrl_reg.on1_average_line = 3;
					onms_onms1_hsync_meas_ctrl_reg.on1_htotal_max_delta_new = 8;
				}
			}
			IoReg_Write32(ONMS_onms1_hsync_meas_ctrl_reg, onms_onms1_hsync_meas_ctrl_reg.regValue);

			onms_onms1_hsync_meas_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms1_hsync_meas_ctrl_2_reg);
#ifdef CONFIG_SUPPORT_SRC_ADC
			if (src == _SRC_YPBPR) {
				onms_onms1_hsync_meas_ctrl_2_reg.on1_vmeas_start = 30;
				onms_onms1_hsync_meas_ctrl_2_reg.on1_vmeas_end = onms_onms1_vs_period_reg.on1_vtotal - 30;
			} else
#endif
			{
				onms_onms1_hsync_meas_ctrl_2_reg.on1_vmeas_start = 16;
				onms_onms1_hsync_meas_ctrl_2_reg.on1_vmeas_end = onms_onms1_vs_period_reg.on1_vtotal - 16;
			}
			IoReg_Write32(ONMS_onms1_hsync_meas_ctrl_2_reg, onms_onms1_hsync_meas_ctrl_2_reg.regValue);

			onms_onms1_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS_onms1_hsync_meas_result_reg);
			onms_onms1_hsync_meas_result_reg.on1_hmax_delta_total = 0;	//write clear
			IoReg_Write32(ONMS_onms1_hsync_meas_result_reg, onms_onms1_hsync_meas_result_reg.regValue);

			onms_onms1_status_reg.regValue = 0;
			onms_onms1_status_reg.on1_hs_meas_over_range = 1;//write clean bit 21
			IoReg_Write32(ONMS_onms1_status_reg, onms_onms1_status_reg.regValue);
		}
	} else {
		rtd_pr_onms_info("[%s] ERROR!! only support channel 1.\n", __FUNCTION__);
	}
}

void drvif_mode_onlinemeasure_setting(unsigned char channel, int src, unsigned char wdgenable, unsigned char interruptenable)
{//This function is to set onlinemeasure watchdog and interrupt setting
	unsigned int online_wd_int_setting = 0;//decide wd and int monitor bit
	onms_onms1_watchdog_en_RBUS onms_onms1_watchdog_en_reg;
	onms_onms1_interrupt_en_RBUS onms_onms1_interrupt_en_reg;
	vgip_int_ctl_RBUS vgip_int_ctl_reg;
	ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;
	ppoverlay_display_timing_ctrl1_RBUS ppoverlay_display_timing_ctrl1_reg;

#ifndef CONFIG_DUAL_CHANNEL
	channel = _CHANNEL1;
#endif

#ifdef POLLING_ONLINE_STATUS_ENABLE//no need to enable interrupt when enable polling flow
	wdgenable = FALSE;
	interruptenable = FALSE;
#endif

	rtd_pr_onms_info("###func %s channel:%d src:%d wdg:%d int:%d###\r\n",__FUNCTION__, channel, src, wdgenable, interruptenable);

	if(_CHANNEL1 == channel)
	{//main case
		if((wdgenable == _DISABLE) && (interruptenable == _DISABLE))
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
			if(src == _SRC_HDMI) // Main path HDMI case will control & init. in drvif_hdmi_onlinemeasure_watchdog_early_enable_setting
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
			if(src != _SRC_HDMI)
			{
				onms_onms1_watchdog_en_reg.regValue = 0;
				onms_onms1_watchdog_en_reg.regValue = online_wd_int_setting;
				onms_onms1_watchdog_en_reg.on1_wd_to_main = _TRUE;		//Watch Dog select to Main Display
				IoReg_Write32(ONMS_onms1_watchdog_en_reg, onms_onms1_watchdog_en_reg.regValue);

				down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/

				ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);

				if (((src == _SRC_CVBS)
#ifdef CONFIG_SUPPORT_SRC_ADC
					|| (src == _SRC_YPBPR) || (src == _SRC_VGA)
#endif
					|| (src == _SRC_MINI_DP)) && (!Get_tv006_wb_pattern())) {
					ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _ENABLE;//Enable force BG
				} else {
					ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _DISABLE;//Disable forced bg for customer//_ENABLE;	//Main Watch Dog Enable (Force-to-Background)
				}
	                    ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = _ENABLE;//FrameSync Main Watch Dog Enable (FreeRun)
				IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
				up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
			}

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

		if(_ENABLE == interruptenable)
		{
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
			onms_onms1_interrupt_en_reg.on1_ie_to_main = _TRUE;		//Watch Dog select to Main Displ
			IoReg_Write32(ONMS_onms1_interrupt_en_reg, onms_onms1_interrupt_en_reg.regValue);
			vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
			vgip_int_ctl_reg.onms1_int_ie = 1;//Enable onlinemeasure interrupt
			IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);
		}
		else
		{
			//onms_onms1_interrupt_en_reg.regValue = IoReg_Read32(ONMS_onms1_interrupt_en_reg);
			onms_onms1_interrupt_en_reg.regValue = 0;
			onms_onms1_interrupt_en_reg.on1_ie_to_main = _FALSE;		//Watch Dog select to Main Display
			IoReg_Write32(ONMS_onms1_interrupt_en_reg, onms_onms1_interrupt_en_reg.regValue);
			vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
			vgip_int_ctl_reg.onms1_int_ie = 0;//Disable onlinemeasure interrupt
			IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);
		}
	}
	else
	{
		rtd_pr_onms_info("[%s] ERROR!! only support channel 1.\n", __FUNCTION__);
	}
}

void drvif_hdmi_onlinemeasure_watchdog_early_enable_setting(void)
{//This function is to set onlinemeasure watchdog and interrupt setting
	unsigned int online_wd_int_setting = 0;//decide wd and int monitor bit
	onms_onms1_watchdog_en_RBUS onms_onms1_watchdog_en_reg;
	ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;
	ppoverlay_display_timing_ctrl1_RBUS ppoverlay_display_timing_ctrl1_reg;

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
			rtd_pr_onms_info("[%s][HDMI] Update onms wdg from %x to %x\n", __FUNCTION__, original_value, onms_onms1_watchdog_en_reg.regValue);
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

	ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = _ENABLE;//FrameSync Main Watch Dog Enable (FreeRun)
	IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
	up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	//FrameSync Watch Dog Enable (FreeRun)
	ppoverlay_display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
	ppoverlay_display_timing_ctrl1_reg.wde_to_free_run = _ENABLE;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, ppoverlay_display_timing_ctrl1_reg.regValue);
}

void drvif_hdmi_dma_onlinemeasure_watchdog_early_enable_setting(void)
{//for online watchdog early enable to protection output timing
	int timeout = 10;
	unsigned int online_wd_int_setting = 0;
	onms_onms3_ctrl_RBUS onms_onms3_ctrl_reg;
	onms_onms3_vs_period_RBUS onms_onms3_vs_period_reg;
	onms_onms3_hs_period_RBUS onms_onms3_hs_period_reg;
	onms_onms3_hsync_meas_ctrl_RBUS onms_onms3_hsync_meas_ctrl_reg;
	onms_onms3_hsync_meas_ctrl_2_RBUS onms_onms3_hsync_meas_ctrl_2_reg;
	onms_onms3_status_RBUS onms_onms3_status_reg;

	onms_onms3_watchdog_en_RBUS onms_onms3_watchdog_en_reg;
	ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;
	ppoverlay_display_timing_ctrl1_RBUS ppoverlay_display_timing_ctrl1_reg;
	onms_onms3_hsync_meas_result_RBUS onms_onms3_hsync_meas_result_reg;


	rtd_pr_onms_info("###func %s ###\r\n",__FUNCTION__);


	onms_onms3_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_reg);
	if(onms_onms3_ctrl_reg.on3_start_ms) {

		//rtd_pr_onms_info("\r\n#####p_timinginfo->IHCount = 0x%8x ####\r\n", p_timinginfo->IHCount);
		//rtd_pr_onms_info("\r\n#####on3_htotal_h12b = 0x%8x ####\r\n", onms_onms3_hs_period_reg.on3_htotal_h12b);

		onms_onms3_ctrl_reg.on3_popup_meas = 1;
		IoReg_Write32(ONMS_onms3_ctrl_reg, onms_onms3_ctrl_reg.regValue);
		onms_onms3_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_reg);
		while(onms_onms3_ctrl_reg.on3_popup_meas && onms_onms3_ctrl_reg.on3_start_ms && timeout)
		{
			timeout--;
			msleep(10);
			onms_onms3_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_reg);
		}
		onms_onms3_vs_period_reg.regValue = IoReg_Read32(ONMS_onms3_vs_period_reg);
		onms_onms3_hs_period_reg.regValue = IoReg_Read32(ONMS_onms3_hs_period_reg);


		//onms_onms3_demode_hs_period_RBUS onms_onms3_demode_hs_period_reg;
		//onms_onms3_demode_hs_period_reg.regValue = IoReg_Read32(ONMS_Onms3_Demode_Hs_period_reg);
		onms_onms3_hs_period_reg.regValue = IoReg_Read32(ONMS_onms3_hs_period_reg);
		//B802145c[13:0] = hotal from b8021238[17:4]
		onms_onms3_hsync_meas_ctrl_reg.on3_target_measure = onms_onms3_hs_period_reg.on3_htotal_h12b;
		//B802145c[19:16] = 2 or 3 (line average)
		onms_onms3_hsync_meas_ctrl_reg.on3_average_line = 3;
		//B802145c[30:24] = 8 (line diff)
		onms_onms3_hsync_meas_ctrl_reg.on3_htotal_max_delta_new = 8;
		IoReg_Write32(ONMS_onms3_hsync_meas_ctrl_reg, onms_onms3_hsync_meas_ctrl_reg.regValue);

		onms_onms3_hsync_meas_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms3_hsync_meas_ctrl_2_reg);
		//B8021460[27:16] = source vtotal-10
		//B8021460[11:0] = 10 after Vsync line number, avoid clamp or macrovision
		onms_onms3_hsync_meas_ctrl_2_reg.on3_vmeas_start = 16;
		onms_onms3_hsync_meas_ctrl_2_reg.on3_vmeas_end = onms_onms3_vs_period_reg.on3_vtotal - 16;
		IoReg_Write32(ONMS_onms3_hsync_meas_ctrl_2_reg, onms_onms3_hsync_meas_ctrl_2_reg.regValue);

		onms_onms3_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS_onms3_hsync_meas_result_reg);
		onms_onms3_hsync_meas_result_reg.on3_hmax_delta_total = 0;	//write clear
		IoReg_Write32(ONMS_onms3_hsync_meas_result_reg, onms_onms3_hsync_meas_result_reg.regValue);

		onms_onms3_status_reg.regValue = 0;
		onms_onms3_status_reg.on3_hs_meas_over_range = 1;//write clean bit 21
		IoReg_Write32(ONMS_onms3_status_reg, onms_onms3_status_reg.regValue);
	}

	//decide wd and int setting
	if(get_hdmi_4k_hfr_mode() == HDMI_NON_4K120)
	{//non 4k120 mode
		online_wd_int_setting = HDMI_VRR_ONLINE_ENABLE_MASK;
	}
	else
	{//4k120 mode
		online_wd_int_setting = HDMI_4K120_VRR_ONLINE_ENABLE_MASK;
	}

	onms_onms3_watchdog_en_reg.regValue = 0;
	onms_onms3_watchdog_en_reg.regValue = online_wd_int_setting;
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
	IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
	up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	//FrameSync Watch Dog Enable (FreeRun)
	ppoverlay_display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
	//fix me later. Ben_Wang add cause data framesync always triiger wdg to free run.
	ppoverlay_display_timing_ctrl1_reg.wde_to_free_run = _ENABLE;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, ppoverlay_display_timing_ctrl1_reg.regValue);
}

void drvif_mode_dma_onlinemeasure_setting(unsigned char wdgenable, unsigned char interruptenable)
{//This function is to set onlinemeasure watchdog and interrupt setting
	int timeout = 10;
	unsigned int online_wd_int_setting = 0;
	onms_onms3_ctrl_RBUS onms_onms3_ctrl_reg;
	onms_onms3_vs_period_RBUS onms_onms3_vs_period_reg;
	onms_onms3_hs_period_RBUS onms_onms3_hs_period_reg;
	onms_onms3_hsync_meas_ctrl_RBUS onms_onms3_hsync_meas_ctrl_reg;
	onms_onms3_hsync_meas_ctrl_2_RBUS onms_onms3_hsync_meas_ctrl_2_reg;
	onms_onms3_status_RBUS onms_onms3_status_reg;

	onms_onms3_watchdog_en_RBUS onms_onms3_watchdog_en_reg;
	onms_onms3_interrupt_en_RBUS onms_onms3_interrupt_en_reg;
	vgip_int_ctl_RBUS vgip_int_ctl_reg;
	ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;
	ppoverlay_display_timing_ctrl1_RBUS ppoverlay_display_timing_ctrl1_reg;
	VSC_INPUT_TYPE_T src;
	onms_onms3_hsync_meas_result_RBUS onms_onms3_hsync_meas_result_reg;

	src = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);

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

			onms_onms3_ctrl_reg.on3_popup_meas = 1;
			IoReg_Write32(ONMS_onms3_ctrl_reg, onms_onms3_ctrl_reg.regValue);
			onms_onms3_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_reg);
			while(onms_onms3_ctrl_reg.on3_popup_meas && onms_onms3_ctrl_reg.on3_start_ms && timeout)
			{
				timeout--;
				msleep(10);
				onms_onms3_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_reg);
			}
			onms_onms3_vs_period_reg.regValue = IoReg_Read32(ONMS_onms3_vs_period_reg);
			onms_onms3_hs_period_reg.regValue = IoReg_Read32(ONMS_onms3_hs_period_reg);

#ifdef CONFIG_SUPPORT_SRC_ADC
			if (src == VSC_INPUTSRC_ADC){//for K3LG-963 rzhen@2016-09-09
				onms_onms3_hsync_meas_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_hsync_meas_ctrl_reg);
				//ADC case setting is different
				//B802145c[13:0] = hotal from b8021108[17:4] rzhen@2016-09-13
				onms_onms3_hsync_meas_ctrl_reg.on3_target_measure = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_COUNT) + 1;
				//B802145c[19:16] = 2 or 3 (line average)
				onms_onms3_hsync_meas_ctrl_reg.on3_average_line = 3;
				switch(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MODE_CURR)){
					case _MODE_720P25:
					case _MODE_720P30:
					case _MODE_720P50:
					case _MODE_720P60:
					case _MODE_1080P24:
					case _MODE_1080P25:
					case _MODE_1080P30:
					case _MODE_1080I25:
					case _MODE_1080I30:
					case _MODE_1080P23:
					case _MODE_1080P29:
					case _MODE_1080P50:
					case _MODE_1080P60:
						onms_onms3_hsync_meas_ctrl_reg.on3_htotal_max_delta_new = 1;
						break;

					case _MODE_480I:
					case _MODE_576I:
					case _MODE_480P:
					case _MODE_576P:
					default:
						onms_onms3_hsync_meas_ctrl_reg.on3_htotal_max_delta_new = 4;
						break;
				}
			}
			else
#endif
			{
				//B802145c[13:0] = hotal from b8021238[17:4]
				onms_onms3_hsync_meas_ctrl_reg.on3_target_measure = onms_onms3_hs_period_reg.on3_htotal_h12b;
				//B802145c[19:16] = 2 or 3 (line average)
				onms_onms3_hsync_meas_ctrl_reg.on3_average_line = 3;
				//B802145c[30:24] = 8 (line diff)
				onms_onms3_hsync_meas_ctrl_reg.on3_htotal_max_delta_new = 8;
			}
			IoReg_Write32(ONMS_onms3_hsync_meas_ctrl_reg, onms_onms3_hsync_meas_ctrl_reg.regValue);


			onms_onms3_hsync_meas_ctrl_2_reg.regValue = IoReg_Read32(ONMS_onms3_hsync_meas_ctrl_2_reg);
			//B8021460[27:16] = source vtotal-10
			//B8021460[11:0] = 10 after Vsync line number, avoid clamp or macrovision
#ifdef CONFIG_SUPPORT_SRC_ADC
			if (src == VSC_INPUTSRC_ADC) {
				onms_onms3_hsync_meas_ctrl_2_reg.on3_vmeas_start = 30;
				onms_onms3_hsync_meas_ctrl_2_reg.on3_vmeas_end = onms_onms3_vs_period_reg.on3_vtotal - 30;
			} else
#endif
			{
				onms_onms3_hsync_meas_ctrl_2_reg.on3_vmeas_start = 16;
				onms_onms3_hsync_meas_ctrl_2_reg.on3_vmeas_end = onms_onms3_vs_period_reg.on3_vtotal - 16;
			}

			IoReg_Write32(ONMS_onms3_hsync_meas_ctrl_2_reg, onms_onms3_hsync_meas_ctrl_2_reg.regValue);

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
			if(get_hdmi_4k_hfr_mode() == HDMI_NON_4K120)
			{//non 4k120 mode
				online_wd_int_setting = HDMI_VRR_ONLINE_ENABLE_MASK;
			}
			else
			{//4k120 mode
				online_wd_int_setting = HDMI_4K120_VRR_ONLINE_ENABLE_MASK;
			}
		}
		else if(src == VSC_INPUTSRC_DP)
		{
			online_wd_int_setting = DP_VRR_ONLINE_ENABLE_MASK;
		}
#ifdef CONFIG_SUPPORT_SRC_ADC
		else if(src == VSC_INPUTSRC_ADC)
		{
			if(get_ADC_Input_Source() == _SRC_VGA)
			{
				online_wd_int_setting = VGA_ONLINE_ENABLE_MASK;
			}
			else
			{
				online_wd_int_setting = YPP_ONLINE_ENABLE_MASK;
			}
		}
#endif
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
			rtd_pr_onms_info("[%s][HDMI] write %x\n", __FUNCTION__, onms_onms3_watchdog_en_reg.regValue);
			IoReg_Write32(ONMS_onms3_watchdog_en_reg, onms_onms3_watchdog_en_reg.regValue);
			down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
			ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);

			if (((src == VSC_INPUTSRC_AVD)
#ifdef CONFIG_SUPPORT_SRC_ADC
				|| (src == VSC_INPUTSRC_ADC)
#endif
				|| (src == VSC_INPUTSRC_DP)) && (!Get_tv006_wb_pattern())) {
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
		pr_info("[%s][HDMI]  Update current_hdmi_onms_watchdog_setting=%x, current_hdmi_onms_is_i3ddma=%x\n", __FUNCTION__, current_hdmi_onms_watchdog_setting,  current_hdmi_onms_is_i3ddma);
	}
#endif // UT_flag

}


#define _CLEAR_ERROR_STATUS	1
void drvif_mode_disableonlinemeasure(unsigned char channel)
{
	onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;
	//onms_onms1_demode_ctrl_RBUS onms_onms1_demode_ctrl_reg;

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

		//onms_onms1_demode_ctrl_reg.regValue =  IoReg_Read32(ONMS_Onms1_Demode_ctrl_reg);
		//onms_onms1_demode_ctrl_reg.on1_demode_en = 0;
		//IoReg_Write32(ONMS_Onms1_Demode_ctrl_reg, onms_onms1_demode_ctrl_reg.regValue);

		drvif_mode_disable_dma_onlinemeasure();//disable dma online measure

	}
}
void drvif_mode_enableonlinemeasure(unsigned char channel)
{
	// start online measure
	unsigned char src;
	onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;
	onms_onms1_hsync_meas_ctrl_RBUS onms_onms1_hsync_meas_ctrl_reg;
	onms_onms1_hsync_meas_ctrl_2_RBUS onms_onms1_hsync_meas_ctrl_2_reg;
	unsigned short Htotal, Vtotal;//input h v total
#ifdef _CLEAR_ERROR_STATUS
	onms_onms1_status_RBUS onms_onms1_status_reg;
	//SP_MS1Stus_RBUS SP_MS1Stus_reg;
#ifdef CONFIG_SUPPORT_SRC_ADC
	sync_processor_sp1_stbrst0_RBUS sync_processor_sp1_stbrst0_reg;
#endif //#ifdef CONFIG_SUPPORT_SRC_ADC
#endif //#ifdef _CLEAR_ERROR_STATUS
	onms_onms1_hsync_meas_result_RBUS onms_onms1_hsync_meas_result_reg;

	onms_onms1_ctrl_1_RBUS onms_onms1_ctrl_1_reg;
#ifndef CONFIG_DUAL_CHANNEL
	channel = _CHANNEL1;
#endif
	src = get_scaler_connect_source(channel);
	rtd_pr_onms_info("###func %s channel:%d src:%d###\r\n",__FUNCTION__, channel, src);
	Vtotal = Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_V_LEN);
	Htotal = Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_H_LEN);

	onms_onms1_hsync_meas_ctrl_reg.regValue = 0;
	IoReg_Write32(ONMS_onms1_hsync_meas_ctrl_reg, onms_onms1_hsync_meas_ctrl_reg.regValue);
	onms_onms1_hsync_meas_ctrl_2_reg.regValue = 0;
	IoReg_Write32(ONMS_onms1_hsync_meas_ctrl_2_reg, onms_onms1_hsync_meas_ctrl_2_reg.regValue);

	// set over range threshold (VSYNC_MAX_DELTA = 2-HSync, HSYNC_MAX_DELTA = 4-clock)
	onms_onms1_ctrl_reg.regValue = 0;//reset value//IoReg_Read32(ONMS_onms1_ctrl_reg);
	onms_onms1_ctrl_reg.on1_blk_2frame_en = 1;
	onms_onms1_ctrl_1_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_1_reg);
	onms_onms1_ctrl_1_reg.on1_vtotal_uf_th = 0;//reset vtotal under flow setting

	//V counter overflow judgment for ONMS	   //H counter overflow judgment for ONMS
	//00: 2048								   //00: 4096
	//01: 4096								   //01: 8192
	//10: 8192								   //10: 16384
	//11: 16383 							   //11: 32768
	onms_onms1_ctrl_1_reg.on1_vtotal_of_th = 16383;//max

	if(Htotal <= 4096)
		onms_onms1_ctrl_reg.on1_htotal_of_th = 0;
	else if(Htotal <= 8192)
		onms_onms1_ctrl_reg.on1_htotal_of_th = 1;
	else if(Htotal <= 16384)
		onms_onms1_ctrl_reg.on1_htotal_of_th = 2;
	else
		onms_onms1_ctrl_reg.on1_htotal_of_th = 3;

	//onms_onms1_ctrl_reg.on1_ms_src_sel = ((Scaler_InputSrcGetMainChType() == _SRC_VO) && Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC)? 1: 0); // Measure Source from VGIP 0: input, 1: Output
	onms_onms1_ctrl_reg.on1_input_align = 0;//default disable input align function for k4lp new adding @Crixus 20170518
	onms_onms1_ctrl_reg.on1_meas_to_sel= _ON1_MEAS_TO_SEL;

	if(src == _SRC_CVBS){
		if(Scaler_DispGetInputInfo(SLR_INPUT_MODE_CURR) == _MODE_480I){
			onms_onms1_ctrl_reg.on1_htotal_max_delta = 3;
			onms_onms1_ctrl_reg.on1_vtotal_max_delta = 3;
		}else{
			onms_onms1_ctrl_reg.on1_htotal_max_delta = 4;
			onms_onms1_ctrl_reg.on1_vtotal_max_delta = 2;
		}
	}else{
		onms_onms1_ctrl_reg.on1_htotal_max_delta = 4;
		onms_onms1_ctrl_reg.on1_vtotal_max_delta = 5;//only for demode
	}

	if ((src == _SRC_HDMI) || (src == _SRC_MINI_DP)) {
		onms_onms1_ctrl_reg.on1_hd21_ms_en = 1;
		onms_onms1_ctrl_reg.on1_continuous_popup = 1;
	}

	IoReg_Write32(ONMS_onms1_ctrl_1_reg, onms_onms1_ctrl_1_reg.regValue);
	IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);

	// enable online measure
	if(src == _SRC_YPBPR || src == _SRC_VGA)
	{//ADC case setting is different
#ifdef CONFIG_SUPPORT_SRC_ADC
		onms_onms1_ctrl_reg.on1_sycms_clk = 0;
		switch(Scaler_DispGetInputInfoByDisp(channel, SLR_INPUT_MODE_CURR))
		{
			case _MODE_480I:
			case _MODE_576I:
				onms_onms1_ctrl_reg.on1_htotal_max_delta = 8;
				break;

			case _MODE_480P:
			case _MODE_576P:
				onms_onms1_ctrl_reg.on1_htotal_max_delta = 4;
				break;

			case _MODE_720P25:
			case _MODE_720P30:
			case _MODE_720P50:
			case _MODE_720P60:
			case _MODE_1080P24:
			case _MODE_1080P25:
			case _MODE_1080P30:
			case _MODE_1080I25:
			case _MODE_1080I30:
			case _MODE_1080P23:
			case _MODE_1080P29:
				onms_onms1_ctrl_reg.on1_htotal_max_delta = 3;

				break;
			case _MODE_1080P50:
			case _MODE_1080P60:
				onms_onms1_ctrl_reg.on1_htotal_max_delta = 2;
				break;

			default:
                onms_onms1_ctrl_reg.on1_htotal_max_delta = 8;
				break;

		}
#endif
	}
	else if(src == _SRC_HDMI)
	{//HDMI case setting is different
		onms_onms1_ctrl_reg.on1_sycms_clk = 1;

		switch(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MODE_CURR))
		{
			case _MODE_480I:
			case _MODE_576I:
				onms_onms1_ctrl_reg.on1_htotal_max_delta = 8;
				break;

			case _MODE_480P:
			case _MODE_576P:
				onms_onms1_ctrl_reg.on1_htotal_max_delta = 4;
				break;

			case _MODE_720P50:
			case _MODE_720P60:
			case _MODE_1080P24:
			case _MODE_1080P25:
			case _MODE_1080P30:
			case _MODE_1080I25:
			case _MODE_1080I30:
			case _MODE_1080P23:
			case _MODE_1080P29:
				onms_onms1_ctrl_reg.on1_htotal_max_delta = 3;

				break;
			case _MODE_1080P50:
			case _MODE_1080P60:
			case _MODE_4k2kI30:
			case _MODE_4k2kP30:
			case _MODE_4k2kP24:
			case _MODE_4k2kP25:
			case _MODE_4k2kP48:
			case _MODE_4k2kP50:
			case _MODE_4k2kP60:
				onms_onms1_ctrl_reg.on1_htotal_max_delta = 2;
				break;

			default:
				onms_onms1_ctrl_reg.on1_htotal_max_delta = 8;
				break;

		}
	}
#ifdef CONFIG_SUPPORT_SRC_ADC
	else
#endif
	{
		onms_onms1_ctrl_reg.on1_sycms_clk = 1;
	}
	onms_onms1_ctrl_reg.on1_ms_conti = 1;
	onms_onms1_ctrl_reg.on1_start_ms = 0;
	onms_onms1_ctrl_reg.on1_continuous_popup = ((src == _SRC_HDMI) || (src == _SRC_MINI_DP)) ? 1:0;//disable continue pop for demode
	IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);
	onms_onms1_ctrl_reg.on1_start_ms = 1;
	IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);

#ifdef _CLEAR_ERROR_STATUS
	//write clear error status
#ifdef CONFIG_SUPPORT_SRC_ADC
	if (src == _SRC_YPBPR || src == _SRC_VGA) { //only YPP need to clear sync_processor
		sync_processor_sp1_stbrst0_reg.regValue = IoReg_Read32(SYNC_PROCESSOR_SP1_StbRst0_reg);
		sync_processor_sp1_stbrst0_reg.s1_stb_per_chg = 1;
		sync_processor_sp1_stbrst0_reg.s1_cap_32unlock = 1;
		sync_processor_sp1_stbrst0_reg.s1_cap_unlock = 1;
		sync_processor_sp1_stbrst0_reg.s1_cap_unlock_no_vs = 1;
		IoReg_Write32(SYNC_PROCESSOR_SP1_StbRst0_reg, sync_processor_sp1_stbrst0_reg.regValue);
	}
#endif//#ifdef CONFIG_SUPPORT_SRC_ADC

	onms_onms1_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS_onms1_hsync_meas_result_reg);
	onms_onms1_hsync_meas_result_reg.on1_hmax_delta_total = 0;	//write clear
	IoReg_Write32(ONMS_onms1_hsync_meas_result_reg, onms_onms1_hsync_meas_result_reg.regValue);

	//write clear status, if can not clear, need add delay
	onms_onms1_status_reg.regValue = IoReg_Read32(ONMS_onms1_status_reg);
	onms_onms1_status_reg.on1_vs_per_to_long = 1;	//Time-Out (76ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit20
	onms_onms1_status_reg.on1_vs_pol_out = 1; // VSYNC Polarity Indicator
	onms_onms1_status_reg.on1_hs_pol_out = 1; //HSYNC Polarity Indicator
	onms_onms1_status_reg.on1_vs_per_over_range = 1; //VSYNC_OVER_RANGE//Bit15
	onms_onms1_status_reg.on1_hs_per_over_range = 1; //HSYNC_OVER_RANGE//Bit14
	onms_onms1_status_reg.on1_vs_per_timeout = 1; //Time-Out (40ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit13
	onms_onms1_status_reg.on1_vs_high_timeout = 1;	//Time-Out bit of Input VSYNC High Pulse Measurement//Bit12
	onms_onms1_status_reg.on1_vs_per_overflow = 1;	//Over-flow bit of Input HSYNC Period Measurement//Bit11
	onms_onms1_status_reg.on1_hs_per_overflow = 1; //Over-flow bit of Input VSYNC Period Measurement//Bit10
	onms_onms1_status_reg.on1_vs_pol_chg = 1; //Input VSYNC Polarity change//Bit9
	onms_onms1_status_reg.on1_hs_pol_chg = 1; //Input HSYNC Polarity change//Bit8
	onms_onms1_status_reg.on1_vtotalde_chg = 1; // Total active line number changes
	onms_onms1_status_reg.on1_vtotal_chg = 1; // Total line number changes
	onms_onms1_status_reg.on1_hact_chg = 1; // Total active pixel number of first 8 active lines changes
	onms_onms1_status_reg.on1_htotal_chg = 1; //Total average pixel number of first 8 lines changes
	onms_onms1_status_reg.on1_hs_active_pixel_var = 1;//Bit3
	onms_onms1_status_reg.on1_hs_meas_over_range = 1;//Bit21 Hs period measure over range
	IoReg_Write32(ONMS_onms1_status_reg, onms_onms1_status_reg.regValue);
#endif //#ifdef _CLEAR_ERROR_STATUS
}

void drvif_mode_disable_dma_onlinemeasure(void)
{
	onms_onms3_ctrl_RBUS onms_onms3_ctrl_reg;
	//onms_onms3_demode_ctrl_RBUS onms_onms3_demode_ctrl_reg;
	rtd_pr_onms_info("#####Disable dma onlinemeasure######\r\n");
	onms_onms3_ctrl_reg.regValue =  IoReg_Read32(ONMS_onms3_ctrl_reg);
	onms_onms3_ctrl_reg.on3_start_ms = 0;
	onms_onms3_ctrl_reg.on3_ms_conti = 0;
	onms_onms3_ctrl_reg.on3_continuous_popup = 0;
	IoReg_Write32(ONMS_onms3_ctrl_reg, onms_onms3_ctrl_reg.regValue);

	//onms_onms3_demode_ctrl_reg.regValue =  IoReg_Read32(ONMS_Onms3_Demode_ctrl_reg);
	//onms_onms3_demode_ctrl_reg.on3_demode_en = 0;
	//IoReg_Write32(ONMS_Onms3_Demode_ctrl_reg, onms_onms3_demode_ctrl_reg.regValue);
}

void drvif_mode_enable_dma_onlinemeasure(void)
{
	// start online measure
	onms_onms3_ctrl_RBUS onms_onms3_ctrl_reg;
	onms_onms3_hsync_meas_ctrl_RBUS onms_onms3_hsync_meas_ctrl_reg;
	onms_onms3_hsync_meas_ctrl_2_RBUS onms_onms3_hsync_meas_ctrl_2_reg;
	VSC_INPUT_TYPE_T src;
#ifdef CONFIG_SUPPORT_SRC_ADC
	StructDisplayInfo *p_dispinfo;
#endif

#ifdef _CLEAR_ERROR_STATUS
	onms_onms3_status_RBUS onms_onms3_status_reg;
	//SP_MS1Stus_RBUS SP_MS1Stus_reg;
#endif //#ifdef _CLEAR_ERROR_STATUS
	onms_onms3_hsync_meas_result_RBUS onms_onms3_hsync_meas_result_reg;
	onms_onms3_ctrl_1_RBUS onms_onms3_ctrl_1_reg;
	unsigned short Vtotal, Htotal;//input h v total

	src = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);
	rtd_pr_onms_info("#####Enable dma onlinemeasure######\r\n");
	Vtotal = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN);
	Htotal = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN);

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

	onms_onms3_ctrl_reg.on3_meas_to_sel= _ON1_MEAS_TO_SEL;
	onms_onms3_ctrl_reg.on3_htotal_max_delta = 4;
	onms_onms3_ctrl_reg.on3_input_align = 0;//default disable input align function for k4lp new adding @Crixus 20170518

	onms_onms3_ctrl_reg.on3_vtotal_max_delta = 5;//only for hs mode

	if((src == VSC_INPUTSRC_HDMI) || (src == VSC_INPUTSRC_DP))
	{
		if(1/*vbe_disp_get_VRR_timingMode_flag()*/)//vrr case input is hdmi 2.1 need HDMI provide the api
		{
			onms_onms3_ctrl_reg.on3_hd21_ms_en = 1;//hdmi2.1
		}
	}

	IoReg_Write32(ONMS_onms3_ctrl_reg, onms_onms3_ctrl_reg.regValue);

	onms_onms3_ctrl_1_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_1_reg);
	onms_onms3_ctrl_1_reg.on3_vtotal_uf_th = 0;
	//V counter overflow judgment for ONMS     //H counter overflow judgment for ONMS
	//00: 2048                                 //00: 4096
	//01: 4096                                 //01: 8192
	//10: 8192                                 //10: 16384
	//11: 16383                                //11: 32768
	onms_onms3_ctrl_1_reg.on3_vtotal_of_th = 16383;//max
	IoReg_Write32(ONMS_onms3_ctrl_1_reg, onms_onms3_ctrl_1_reg.regValue);


	// enable online measure
	if(src == VSC_INPUTSRC_ADC)
	{//ADC case setting is different
#ifdef CONFIG_SUPPORT_SRC_ADC
#ifndef BUILD_QUICK_SHOW
		onms_onms3_ctrl_reg.on3_sycms_clk = 0;
		p_dispinfo = Get_ADC_Dispinfo();
		switch(p_dispinfo->ucMode_Curr)
		{
			case _MODE_480I:
			case _MODE_576I:
				onms_onms3_ctrl_reg.on3_htotal_max_delta = 8;
				break;

			case _MODE_480P:
			case _MODE_576P:
				onms_onms3_ctrl_reg.on3_htotal_max_delta = 4;
				break;

			case _MODE_720P25:
			case _MODE_720P30:
			case _MODE_720P50:
			case _MODE_720P60:
			case _MODE_1080P24:
			case _MODE_1080P25:
			case _MODE_1080P30:
			case _MODE_1080I25:
			case _MODE_1080I30:
			case _MODE_1080P23:
			case _MODE_1080P29:
				onms_onms3_ctrl_reg.on3_htotal_max_delta = 3;

				break;
			case _MODE_1080P50:
			case _MODE_1080P60:
				onms_onms3_ctrl_reg.on3_htotal_max_delta = 2;
				break;

			default:
				break;

		}
#endif
#endif
	}
	else if(src == VSC_INPUTSRC_HDMI)
	{//HDMI case setting is different
		onms_onms3_ctrl_reg.on3_sycms_clk = 1;

		switch(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MODE_CURR))
		{
			case _MODE_480I:
			case _MODE_576I:
				onms_onms3_ctrl_reg.on3_htotal_max_delta = 8;
				break;

			case _MODE_480P:
			case _MODE_576P:
				onms_onms3_ctrl_reg.on3_htotal_max_delta = 4;
				break;

			case _MODE_720P50:
			case _MODE_720P60:
			case _MODE_1080P24:
			case _MODE_1080P25:
			case _MODE_1080P30:
			case _MODE_1080I25:
			case _MODE_1080I30:
			case _MODE_1080P23:
			case _MODE_1080P29:
				onms_onms3_ctrl_reg.on3_htotal_max_delta = 3;

				break;
			case _MODE_1080P50:
			case _MODE_1080P60:
			case _MODE_4k2kI30:
			case _MODE_4k2kP30:
			case _MODE_4k2kP24:
			case _MODE_4k2kP25:
			case _MODE_4k2kP48:
			case _MODE_4k2kP50:
			case _MODE_4k2kP60:
				onms_onms3_ctrl_reg.on3_htotal_max_delta = 2;
				break;

			default:
				onms_onms3_ctrl_reg.on3_htotal_max_delta = 8;
				break;

		}
	}
#ifdef CONFIG_SUPPORT_SRC_ADC
	else
#endif
	{
		onms_onms3_ctrl_reg.on3_sycms_clk = 1;
	}
	onms_onms3_ctrl_reg.on3_ms_conti = 1;
	onms_onms3_ctrl_reg.on3_start_ms = 0;
	onms_onms3_ctrl_reg.on3_continuous_popup = ((src == VSC_INPUTSRC_HDMI) || (src == VSC_INPUTSRC_DP)) ? 1:0;//disable continue pop for demode
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
		onms_onms3_status_reg.on3_hs_pol_out = 1; //HSYNC Polarity Indicator
		onms_onms3_status_reg.on3_vs_per_over_range = 1; //VSYNC_OVER_RANGE//Bit15
		onms_onms3_status_reg.on3_hs_per_over_range = 1; //HSYNC_OVER_RANGE//Bit14
		onms_onms3_status_reg.on3_vs_per_timeout = 1; //Time-Out (40ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit13
		onms_onms3_status_reg.on3_vs_high_timeout = 1;	//Time-Out bit of Input VSYNC High Pulse Measurement//Bit12
		onms_onms3_status_reg.on3_vs_per_overflow = 1;	//Over-flow bit of Input HSYNC Period Measurement//Bit11
		onms_onms3_status_reg.on3_hs_per_overflow = 1; //Over-flow bit of Input VSYNC Period Measurement//Bit10
		onms_onms3_status_reg.on3_vs_pol_chg= 1; //Input VSYNC Polarity change//Bit9
		onms_onms3_status_reg.on3_hs_pol_chg = 1; //Input HSYNC Polarity change//Bit8
		onms_onms3_status_reg.on3_vtotalde_chg = 1; // Total active line number changes
		onms_onms3_status_reg.on3_vtotal_chg = 1; // Total line number changes
		onms_onms3_status_reg.on3_hact_chg = 1; // Total active pixel number of first 8 active lines changes
		onms_onms3_status_reg.on3_htotal_chg = 1; //Total average pixel number of first 8 lines changes
		onms_onms3_status_reg.on3_hs_active_pixel_var = 1;//Bit3
		onms_onms3_status_reg.on3_hs_meas_over_range = 1;//Bit21 Hs period measure over range
		IoReg_Write32(ONMS_onms3_status_reg, onms_onms3_status_reg.regValue);
#endif //#ifdef _CLEAR_ERROR_STATUS
}

void drvif_mode_disable_main_sub_onlinemeasure_only(unsigned char channel)
{
	onms_onms1_watchdog_en_RBUS onms_onms1_watchdog_en_reg;
	onms_onms1_interrupt_en_RBUS onms_onms1_interrupt_en_reg;
	vgip_int_ctl_RBUS vgip_int_ctl_reg;
	ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;
	onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;
	//onms_onms1_demode_ctrl_RBUS onms_onms1_demode_ctrl_reg;

#ifndef CONFIG_DUAL_CHANNEL
	channel = _CHANNEL1;
#endif

	rtd_pr_onms_info("###func %s Only disable channel:%d ###\n",__FUNCTION__, channel);

	if(channel == _CHANNEL1)
	{
		/* Disable online measure watchdog */
		onms_onms1_watchdog_en_reg.regValue = 0;
		onms_onms1_watchdog_en_reg.on1_wd_to_main = _FALSE;		//Watch Dog select to Main Display
		IoReg_Write32(ONMS_onms1_watchdog_en_reg, onms_onms1_watchdog_en_reg.regValue);
		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);
		ppoverlay_main_display_control_rsv_reg.main_wd_to_background = _DISABLE;	//Main Watch Dog Enable (Force-to-Background)
		ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = _DISABLE;	//FrameSync Main Watch Dog Enable (FreeRun)
		IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
		up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/

		/* Disable online measure interrupt */
		//onms_onms1_interrupt_en_reg.regValue = IoReg_Read32(ONMS_onms1_interrupt_en_reg);
		onms_onms1_interrupt_en_reg.regValue = 0;
		onms_onms1_interrupt_en_reg.on1_ie_to_main = _FALSE;		//Watch Dog select to Main Display
		IoReg_Write32(ONMS_onms1_interrupt_en_reg, onms_onms1_interrupt_en_reg.regValue);
		vgip_int_ctl_reg.regValue = IoReg_Read32(VGIP_INT_CTL_reg);
		vgip_int_ctl_reg.onms1_int_ie = 0;//Disable onlinemeasure interrupt
		IoReg_Write32(VGIP_INT_CTL_reg, vgip_int_ctl_reg.regValue);

		/* Disable online measure */
		onms_onms1_ctrl_reg.regValue =  IoReg_Read32(ONMS_onms1_ctrl_reg);
		onms_onms1_ctrl_reg.on1_start_ms = 0;
		onms_onms1_ctrl_reg.on1_ms_conti = 0;
		onms_onms1_ctrl_reg.on1_continuous_popup = 0;
		IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);

		//onms_onms1_demode_ctrl_reg.regValue =  IoReg_Read32(ONMS_Onms1_Demode_ctrl_reg);
		//onms_onms1_demode_ctrl_reg.on1_demode_en = 0;
		//IoReg_Write32(ONMS_Onms1_Demode_ctrl_reg, onms_onms1_demode_ctrl_reg.regValue);
	}
	else
	{
		rtd_pr_onms_info("[%s] ERROR!! only support channel 1.\n", __FUNCTION__);
	}
}

unsigned char drvif_mode_check_onlinemeasure_status(unsigned char channel)
{//Return FALSE: Online measure has error status.
	unsigned int check_status_mask = 0;
	unsigned char src;
	onms_onms1_status_RBUS onms_onms1_status_reg;
	onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;
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
			if(get_hdmi_4k_hfr_mode() == HDMI_NON_4K120)
			{//non 4k120 mode
				check_status_mask = HDMI_VRR_ONLINE_ENABLE_MASK;
			}
			else
			{//4k120 mode
				check_status_mask = HDMI_4K120_VRR_ONLINE_ENABLE_MASK;
			}
			if(onms_onms1_status_reg.regValue & check_status_mask)
				return FALSE;
		}
		else if(src == _SRC_MINI_DP)
		{
			check_status_mask = DP_VRR_ONLINE_ENABLE_MASK;
			if(onms_onms1_status_reg.regValue & check_status_mask)
				return FALSE;
		}
		else if(src == _SRC_YPBPR || src == _SRC_VGA)
		{
#ifdef CONFIG_SUPPORT_SRC_ADC
			if(get_ADC_Input_Source() == _SRC_VGA)
			{
				check_status_mask = VGA_ONLINE_ENABLE_MASK;
			}
			else
			{
				check_status_mask = YPP_ONLINE_ENABLE_MASK;
			}
			if(onms_onms1_status_reg.regValue & check_status_mask)
				return FALSE;
#endif
		}
		else if(src == _SRC_CVBS)
		{
			check_status_mask = AVD_ONLINE_ENABLE_MASK;
			if(onms_onms1_status_reg.regValue & check_status_mask)
				return FALSE;
		}

		if((check_status_mask != 0) && (onms_onms1_ctrl_reg.on1_start_ms == 0))
		{
			rtd_pr_vsc_info("[%s] %d Enable online measure fail, re-run scaler. onms1_ctrl=0x%x current_status=0x%x check_status=0x%x\n", __FUNCTION__, __LINE__, onms_onms1_ctrl_reg.regValue, onms_onms1_status_reg.regValue, check_status_mask);
			return FALSE;
		}
	}

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
		if(get_hdmi_4k_hfr_mode() == HDMI_NON_4K120)
		{//non 4k120 mode
			check_status_mask = HDMI_VRR_ONLINE_ENABLE_MASK;
		}
		else
		{//4k120 mode
			check_status_mask = HDMI_4K120_VRR_ONLINE_ENABLE_MASK;
		}
		if(onms_onms3_status_reg.regValue & check_status_mask)
			 return FALSE;
	}
	else if(src == VSC_INPUTSRC_DP)
	{
		check_status_mask = DP_VRR_ONLINE_ENABLE_MASK;
		if(onms_onms3_status_reg.regValue & check_status_mask)
			 return FALSE;
	}
	else if(src == VSC_INPUTSRC_ADC)
	{
#ifdef CONFIG_SUPPORT_SRC_ADC
		if(get_ADC_Input_Source() == _SRC_VGA)
		{
			check_status_mask = VGA_ONLINE_ENABLE_MASK;
		}
		else
		{
			check_status_mask = YPP_ONLINE_ENABLE_MASK;
		}
		if(onms_onms3_status_reg.regValue & check_status_mask)
			return FALSE;
#endif
	}
	else if(src == VSC_INPUTSRC_AVD)
	{
		check_status_mask = AVD_ONLINE_ENABLE_MASK;
		if(onms_onms3_status_reg.regValue & check_status_mask)
			return FALSE;
	}

	return TRUE;
}



void mode_set_dispinfo_from_table(int src, StructDisplayInfo* p_dispinfo, ModeInformationType* p_timinginfo)
{
#ifdef CONFIG_SUPPORT_SRC_ADC
	//unsigned char channel = Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL);
	unsigned short mode = p_dispinfo->ucMode_Table_Index;

	StructScreenModeInfo *stScreenModeCurrInfo = NULL;

	//without overscan
	if (src == _SRC_VGA)
	{
		p_dispinfo->ucMode_Curr = tINPUTMODE_PRESET_TABLE[mode].ModeID;

		if(p_timinginfo){
			stScreenModeCurrInfo = get_vga_auto_data(p_timinginfo->ModeCurr);
		}

 		if(stScreenModeCurrInfo && stScreenModeCurrInfo->Open_Count){
			p_dispinfo->IPH_ACT_STA = stScreenModeCurrInfo->H_Start_Pos_After_Auto;
			p_dispinfo->IPV_ACT_STA = stScreenModeCurrInfo->V_Start_Pos_After_Auto;
		}else{
			p_dispinfo->IPH_ACT_STA = tINPUTMODE_PRESET_TABLE[mode].IHStartPos;
			p_dispinfo->IPV_ACT_STA = tINPUTMODE_PRESET_TABLE[mode].IVStartPos;
		}

		p_dispinfo->IPH_ACT_WID = tINPUTMODE_PRESET_TABLE[mode].IHWidth;
		p_dispinfo->IPV_ACT_LEN = tINPUTMODE_PRESET_TABLE[mode].IVHeight;
		p_dispinfo->IHFreq = tINPUTMODE_PRESET_TABLE[mode].IHFreq;
		p_dispinfo->IVFreq = tINPUTMODE_PRESET_TABLE[mode].IVFreq;
		p_dispinfo->INPUT_ADC_CLOCK = tINPUTMODE_PRESET_TABLE[mode].IHTotal;
		p_dispinfo->IHTotal = tINPUTMODE_PRESET_TABLE[mode].IHTotal;
		p_dispinfo->IVTotal = tINPUTMODE_PRESET_TABLE[mode].IVTotal;
	}
	else if(src == _SRC_YPBPR)
	{
		p_dispinfo->ucMode_Curr = tINPUTMODE_YPBPR_TABLE[mode].ModeID;
#if TEST_PATTERN_GEN
		p_dispinfo->IPH_ACT_STA = 380;
		p_dispinfo->IPH_ACT_WID = 3840;
		p_dispinfo->IPV_ACT_STA = 80;
		p_dispinfo->IPV_ACT_LEN = 2160;
		p_dispinfo->IHFreq = 675;
		p_dispinfo->IVFreq = 300;
		p_dispinfo->INPUT_ADC_CLOCK = 4400;
		p_dispinfo->IHTotal = 4400;
		p_dispinfo->IVTotal = 2250;
#else
#if 0//Fix error by Will//defined(ENABLE_DRIVER_I3DDMA) || defined(ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE)
  		// get no overscan mode table when IDMA is enabled
		if((src == _SRC_YPBPR) && (Scaler_Get3DMode((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)) != SLR_3DMODE_2D) && (Scaler_Get3DMode((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)) != SLR_3DMODE_2D_CVT_3D))
		{
			p_dispinfo->IPH_ACT_STA = tINPUTMODE_YPBPR_TABLE_3D[mode].IHStartPos;
			p_dispinfo->IPH_ACT_WID = tINPUTMODE_YPBPR_TABLE_3D[mode].IHWidth;
			p_dispinfo->IPV_ACT_STA = tINPUTMODE_YPBPR_TABLE_3D[mode].IVStartPos;
			p_dispinfo->IPV_ACT_LEN = tINPUTMODE_YPBPR_TABLE_3D[mode].IVHeight;
		}else
#endif
		{
			p_dispinfo->IPH_ACT_STA = tINPUTMODE_YPBPR_TABLE[mode].IHStartPos;
			p_dispinfo->IPH_ACT_WID = tINPUTMODE_YPBPR_TABLE[mode].IHWidth;
			p_dispinfo->IPV_ACT_STA = tINPUTMODE_YPBPR_TABLE[mode].IVStartPos;
			p_dispinfo->IPV_ACT_LEN = tINPUTMODE_YPBPR_TABLE[mode].IVHeight;
		}
  		p_dispinfo->IHFreq = tINPUTMODE_YPBPR_TABLE[mode].IHFreq;
		p_dispinfo->IVFreq = tINPUTMODE_YPBPR_TABLE[mode].IVFreq;
		p_dispinfo->INPUT_ADC_CLOCK = tINPUTMODE_YPBPR_TABLE[mode].IHTotal;
		p_dispinfo->IHTotal = tINPUTMODE_YPBPR_TABLE[mode].IHTotal;
		p_dispinfo->IVTotal = tINPUTMODE_YPBPR_TABLE[mode].IVTotal;
		if(get_force_i3ddma_enable(SLR_MAIN_DISPLAY) == true)
		{
			p_dispinfo->color_space = COLOR_YUV444;
			p_dispinfo->color_depth = 1;
		}
#endif
	}
	else
#endif
		return;

#if 0//Fix Error by WIll
	 //some timing(uncompatiable)  measured hstart is largger or smaller than table defined , we record hstart after measuring in auto function
	if ((src == _SRC_VGA) && (stVGAModeCurrInfo[mode].Open_Count ==1))
	{
	//egger20120704 modify for VGA radom cause  front porch less than 3 and trash screen
		if(stVGAModeCurrInfo[mode].H_Start_Pos_After_Auto<((tINPUTMODE_PRESET_TABLE[mode].IHStartPos)*13/10))
		{
			Scaler_DispSetInputInfo(SLR_INPUT_IPH_ACT_STA, stVGAModeCurrInfo[mode].H_Start_Pos_After_Auto);//eeprom auto positon result
		}
		else
		{
			rtd_pr_adc_info("H_Start_Pos_After_Auto : %x, is Abnormal Value\n", stVGAModeCurrInfo[mode].H_Start_Pos_After_Auto);//preset table value
		}
		Scaler_DispSetInputInfo(SLR_INPUT_IPV_ACT_STA, stVGAModeCurrInfo[mode].V_Start_Pos_After_Auto);//Using V_Start_Pos_After_Auto

	}
#endif

	if (p_timinginfo) {
	    p_dispinfo->IHSyncPulseCount = p_timinginfo->IHSyncPulseCount;
	    p_dispinfo->Hsync = p_timinginfo->IHCount;
	    p_dispinfo->Vsync = p_timinginfo->IVCount;
	} else
	    rtd_pr_adc_debug("[%s:%d][ERROR] invalid pointer dereference!! \n",__FUNCTION__,__LINE__);
	p_dispinfo->IPH_ACT_STA_PRE = p_dispinfo->IPH_ACT_STA;
	p_dispinfo->IPV_ACT_STA_PRE = p_dispinfo->IPV_ACT_STA;
	p_dispinfo->IPH_ACT_WID_PRE = p_dispinfo->IPH_ACT_WID;
	p_dispinfo->IPV_ACT_LEN_PRE = p_dispinfo->IPV_ACT_LEN;

	//rtd_pr_adc_debug("**HS=%x,HW=%x,VS=%x,VW=%x\n",Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_STA),Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID),Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_STA),Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN));
	//rtd_pr_adc_debug("**IHFreq=%x,IVFreq=%x,IHTotal=%x,IVTotal=%x,ADC_CLOCK:%x\n",Scaler_DispGetInputInfo(SLR_INPUT_H_FREQ),Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ),Scaler_DispGetInputInfo(SLR_INPUT_H_LEN),Scaler_DispGetInputInfo(SLR_INPUT_V_LEN),Scaler_DispGetInputInfo(SLR_INPUT_ADC_CLOCK));
	rtd_pr_adc_debug("**HS=%x,HW=%x,VS=%x,VW=%x\n",
				p_dispinfo->IPH_ACT_STA,
				p_dispinfo->IPH_ACT_WID,
				p_dispinfo->IPV_ACT_STA,
				p_dispinfo->IPV_ACT_LEN);
	rtd_pr_adc_debug("**IHFreq=%x,IVFreq=%x,IHTotal=%x,IVTotal=%x,ADC_CLOCK:%x\n",
				p_dispinfo->IHFreq,
				p_dispinfo->IVFreq,
				p_dispinfo->IHTotal,
				p_dispinfo->IVTotal,
				p_dispinfo->INPUT_ADC_CLOCK);

	if((p_dispinfo->IPV_ACT_STA + p_dispinfo->IPV_ACT_LEN) > p_dispinfo->IVTotal ){
		rtd_pr_adc_debug("\n/*================================\n");
		rtd_pr_adc_debug("  Note:Mode Table ERROR!!!\n  (ivst:%d+ivlen:%d)>ivtotal:%d\n",p_dispinfo->IPV_ACT_STA, p_dispinfo->IPV_ACT_LEN, p_dispinfo->IVTotal);
		rtd_pr_adc_debug("================================*/\n");
	}
}


void drvif_mode_disablevgip(unsigned char display)
{
	// Disable Sample Clock in VGIP
	fw_scaler_set_sample(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL), _DISABLE);
}



void drvif_aspectratio_disablefrc(UINT8 display)
{
#ifdef CONFIG_DUAL_CHANNEL
	drvif_memory_DisableMCap(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL));
	drvif_memory_EnableMDisp_Or_DisableMDisp(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL), _DISABLE);//frank@0203 disable display
#else
	drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);
	drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_MAIN_DISPLAY, _DISABLE);//frank@0203 disable display
#endif
#if 0
	if(TRUE==get_delayframe_fuc())
	{
		delay_framebuffer_free();
	}
#endif
	drvif_memory_free_block(MEMIDX_MAIN);		// free first main block
		drvif_memory_free_block(MEMIDX_MAIN_SEC);	// free second main block
#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE
	if(_ENABLE == Scaler_Get_Vflip_Enable())
		drvif_memory_free_block(MEMIDX_MAIN_THIRD);
	else if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_VFLIP3))
		drvif_memory_free_block(MEMIDX_MAIN_THIRD);
#else //#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE
	if (Get_PANEL_VFLIP_ENABLE())
		drvif_memory_free_block(MEMIDX_MAIN_THIRD);
#endif //#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE
	if (Get_PANEL_VFLIP_ENABLE())
	{
#ifdef CONFIG_DUAL_CHANNEL
		IoReg_ClearBits(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg: MDOMAIN_CAP_DDR_In1Ctrl_reg, MDOMAIN_CAP_DDR_In1Ctrl_in1_v_flip_3buf_en_mask);
		IoReg_ClearBits(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_DISP_DDR_SubCtrl_reg: MDOMAIN_DISP_DDR_MainCtrl_reg, MDOMAIN_DISP_DDR_MainCtrl_main_v_flip_3buf_en_mask);
#else
		IoReg_ClearBits(/*Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg : */MDOMAIN_CAP_DDR_In1Ctrl_reg, MDOMAIN_CAP_DDR_In1Ctrl_in1_v_flip_3buf_en_mask);
		IoReg_ClearBits(/*Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_DISP_DDR_SubCtrl_reg : */MDOMAIN_DISP_DDR_MainCtrl_reg, MDOMAIN_DISP_DDR_MainCtrl_main_v_flip_3buf_en_mask);
#endif
	}
#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE
	else if(_ENABLE == Scaler_Get_Enable_3Buffer_NewRuleCondition())
	{
#ifdef CONFIG_DUAL_CHANNEL
		IoReg_ClearBits(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg: MDOMAIN_CAP_DDR_In1Ctrl_reg, MDOMAIN_CAP_DDR_In1Ctrl_in1_v_flip_3buf_en_mask);
		IoReg_ClearBits(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_DISP_DDR_SubCtrl_reg: MDOMAIN_DISP_DDR_MainCtrl_reg, MDOMAIN_DISP_DDR_MainCtrl_main_v_flip_3buf_en_mask);
#else //#ifdef CONFIG_DUAL_CHANNEL
		IoReg_ClearBits(/*Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg : */MDOMAIN_CAP_DDR_In1Ctrl_reg, MDOMAIN_CAP_DDR_In1Ctrl_in1_v_flip_3buf_en_mask);
		IoReg_ClearBits(/*Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_DISP_DDR_SubCtrl_reg : */MDOMAIN_DISP_DDR_MainCtrl_reg, MDOMAIN_DISP_DDR_MainCtrl_main_v_flip_3buf_en_mask);
#endif //#ifdef CONFIG_DUAL_CHANNEL
	}
#endif //#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE

	/*frank@0817 mark below code to solve ATV 2d cvt 3D and change display ratio have noise image
	if (Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY) == SLR_MAIN_DISPLAY)
		IoReg_Mask32(MDOMAIN_DISP_DDR_MainPreVStart_reg, 0xfffff800, 400);*///frank@0428 add avoid frc transfer framesync fail
	drvif_memory_set_dbuffer_write(display);


	if(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_THRIP) || Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_RTNR)) {
		fw_scalerip_disable_onlyip(display);
		Scaler_DispSetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_THRIP,FALSE);
		//drvif_color_rtnr_off(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL)); // Change to use display avoid to use info make system error, 970616 weihao
	}

}


void drvif_mode_disablechannelproc(unsigned char display)
{
	// Disable Watch enable in D-domain
	down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	//frank@0810 mask interrupt status bit IoReg_ClearBits(display ? PPOVERLAY_MP_Layout_Force_to_Background_reg :  PPOVERLAY_Main_Display_Control_RSV_reg, _BIT15);
	if(display){
#ifdef CONFIG_DUAL_CHANNEL
		IoReg_ClearBits(PPOVERLAY_MP_Layout_Force_to_Background_reg, _BIT15);
#endif
	}else{
		IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ((IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg) & ~(_BIT15))));
	}
	up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	if(display == SLR_MAIN_DISPLAY) {
		//drvif_memory_sw_buffer_swap_action(SLR_MAIN_DISPLAY, MEM_SWBUFFER_SWAP_DISABLE);
		//drvif_memory_3D_mode_sw_buffer_swap_action(SLR_MAIN_DISPLAY, MEM_3D_SWBUFFER_SWAP_DISABLE);
		dvrif_memory_set_frc_style(SLR_MAIN_DISPLAY, _DISABLE);

		drvif_scaler_set_display_mode(DISPLAY_MODE_FRC); //set FRC mode
		//IoReg_ClearBits(PPOVERLAY_Display_Timing_CTRL1_reg, _BIT29); // makred by hsliao 20090410
#if 0
		if(TRUE==get_delayframe_fuc())
		{
			delay_framebuffer_free();
		}
#endif
		drvif_memory_free_block(MEMIDX_MAIN);		// free first main block
		drvif_memory_free_block(MEMIDX_MAIN_SEC);	// free second main block
#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE
		if(_ENABLE == Scaler_Get_Vflip_Enable())
			drvif_memory_free_block(MEMIDX_MAIN_THIRD);
		else if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_VFLIP3))
			drvif_memory_free_block(MEMIDX_MAIN_THIRD);
#else //#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE
		if (Get_PANEL_VFLIP_ENABLE())
			drvif_memory_free_block(MEMIDX_MAIN_THIRD);
#endif //#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE
	} else {
		//drvif_memory_sw_buffer_swap_action(SLR_SUB_DISPLAY, MEM_SWBUFFER_SWAP_DISABLE);
		//drvif_memory_3D_mode_sw_buffer_swap_action(SLR_SUB_DISPLAY, MEM_3D_SWBUFFER_SWAP_DISABLE);

		drvif_memory_free_block(MEMIDX_SUB);			// free first sub block
		drvif_memory_free_block(MEMIDX_SUB_SEC);		// free second sub block
#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE
		if(_ENABLE == Get_PANEL_VFLIP_ENABLE())
			drvif_memory_free_block(MEMIDX_SUB_THIRD);
		else if(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_VFLIP3))
			drvif_memory_free_block(MEMIDX_SUB_THIRD);
#else //#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE
		if (Get_PANEL_VFLIP_ENABLE())
			drvif_memory_free_block(MEMIDX_SUB_THIRD);
#endif //#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE
	}

	//if(GET_RTNR_MODE(display) && (display == SLR_MAIN_DISPLAY))
	//	drvif_color_rtnr_off(disp_info[display].channel);

	//below similar to dragon, using dragon FW
	if((Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DISPLAY) == SLR_SUB_DISPLAY) && ((Scaler_PipGetInfo(SLR_PIP_TYPE) >= SUBTYPE_MP2X2) && (Scaler_PipGetInfo(SLR_PIP_TYPE) <= SUBTYPE_MP1L7B)) && (Scaler_PipGetSubInfo(SLR_PIP_SUB_MPENABLE))) {

	} else {
		// Disable FRC
#ifdef CONFIG_DUAL_CHANNEL
		drvif_memory_DisableMCap(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL));
#else
		drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);
#endif
/* //remove vby1 define due to no sr mode on sirius
#ifdef VBY_ONE_PANEL //frank@03202013 add below code to disable DFRC IP
		if(display == SLR_MAIN_DISPLAY){
			IoReg_ClearBits(OD_ODDMA_CAP_CTL0_VADDR, _BIT19);//disable dfrc capture
			IoReg_ClearBits(OD_ODDMA_FRC_CTRL_VADDR, _BIT0);//disable dfrc display
		}
#endif
*/
		//if(Scaler_Get_Keep_In_BootUp_Mode_flag()==FALSE)
		{
#ifdef CONFIG_DUAL_CHANNEL
			drvif_memory_EnableMDisp_Or_DisableMDisp(display, _DISABLE);//frank@0203 disable display
#else
			drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_MAIN_DISPLAY, _DISABLE);//frank@0203 disable display
#endif
		}

		if (Get_PANEL_VFLIP_ENABLE())
		{
#ifdef CONFIG_DUAL_CHANNEL
			IoReg_ClearBits(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg: MDOMAIN_CAP_DDR_In1Ctrl_reg, MDOMAIN_CAP_DDR_In1Ctrl_in1_v_flip_3buf_en_mask);
			IoReg_ClearBits(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_DISP_DDR_SubCtrl_reg: MDOMAIN_DISP_DDR_MainCtrl_reg, MDOMAIN_DISP_DDR_MainCtrl_main_v_flip_3buf_en_mask);
#else
			IoReg_ClearBits(/*Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg : */MDOMAIN_CAP_DDR_In1Ctrl_reg, MDOMAIN_CAP_DDR_In1Ctrl_in1_v_flip_3buf_en_mask);
			IoReg_ClearBits(/*Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_DISP_DDR_SubCtrl_reg : */MDOMAIN_DISP_DDR_MainCtrl_reg, MDOMAIN_DISP_DDR_MainCtrl_main_v_flip_3buf_en_mask);
#endif
		}
#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE
		else if(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_VFLIP3))
		{
	#ifdef CONFIG_DUAL_CHANNEL
			IoReg_ClearBits(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg: MDOMAIN_CAP_DDR_In1Ctrl_reg, MDOMAIN_CAP_DDR_In1Ctrl_in1_v_flip_3buf_en_mask);
			IoReg_ClearBits(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_DISP_DDR_SubCtrl_reg: MDOMAIN_DISP_DDR_MainCtrl_reg, MDOMAIN_DISP_DDR_MainCtrl_main_v_flip_3buf_en_mask);
	#else //#ifdef CONFIG_DUAL_CHANNEL
			IoReg_ClearBits(MDOMAIN_CAP_DDR_In1Ctrl_reg,MDOMAIN_CAP_DDR_In1Ctrl_in1_v_flip_3buf_en_mask);
			IoReg_ClearBits(MDOMAIN_DISP_DDR_MainCtrl_reg, MDOMAIN_DISP_DDR_MainCtrl_main_v_flip_3buf_en_mask);
	#endif //#ifdef CONFIG_DUAL_CHANNEL
		}
#endif //#ifdef CONFIG_ENABLE_3BUFFER_NEW_RULE

		//if(Scaler_Get_Keep_In_BootUp_Mode_flag()==FALSE)
		{
			//if(Scaler_Get_Keep_In_BootUp_Mode_flag()==FALSE)
			{
				if (Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY) == SLR_MAIN_DISPLAY)
					IoReg_Mask32(MDOMAIN_DISP_DDR_MainPreVStart_reg, 0xfffff800, 400);//frank@0428 add avoid frc transfer framesync fail
			}
		}
		drvif_memory_set_dbuffer_write(display);
	}
#if 0
	// DeInterlace bypass IP & close IP
	if((scaler_MaincvtSub_get_vgip2CurState()==VGIP2_2X_CLK_VO_INIT ))
	{
		//do nothing
	}
	else
#endif
	{
		if(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_THRIP) || Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_RTNR)) {
			fw_scalerip_disable_ip(display);
			Scaler_DispSetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_THRIP,FALSE);
			//fixme:vip
			//drvif_color_rtnr_off(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL)); // Change to use display avoid to use info make system error, 970616 weihao
		}
	}
}

#ifndef BUILD_QUICK_SHOW
void drvif_mode_resetmode(unsigned char display)
{
	extern void fw_scaler_ctrl_dma_vgip_regen_vsync(unsigned char enable);
#ifdef CONFIG_HDR_SDR_SEAMLESS
	extern void i3ddma_cap1_interrupt_ctrl(unsigned char enable);
#ifdef CONFIG_ENABLE_HDR10_HDMI_AUTO_DETECT
	_RPC_system_setting_info  *RPC_VIP_sysInfo = (_RPC_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_system_info_structure);
#endif
#endif
    extern void (*reset_hdr_mode)(unsigned char);
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
#ifdef CONFIG_DUAL_CHANNEL
//	sub_vgip_vgip_chn2_act_hsta_width_RBUS vgip_chn2_act_hsta_width_reg;
	sub_vgip_vgip_chn2_delay_RBUS vgip_chn2_delay_reg;
	sub_vgip_vgip_chn2_ctrl_RBUS vgip_chn2_ctrl_reg;
	//onms_onms2_ctrl_RBUS onms_onms2_ctrl_reg;
	scaleup_ds_uzu_db_ctrl_RBUS scaleup_ds_uzu_db_ctrl_Reg;
	sub_vgip_vgip_chn2_double_buffer_ctrl_RBUS vgip_chn2_double_buffer_ctrl_reg;
#endif
	unsigned char src;
	unsigned long flags = 0;//for spin_lock_irqsave
    unsigned int *setup_di_done_flag = NULL;

	src = get_scaler_connect_source(display);

	reset_rotate_mode(display);

	// must clear 	s1_clk_en, then clear main_imd_smooth_toggle_clk_time2switch_en
	/* disable snr clk @ begin of reset mode for snr hw issue, elieli 20160825*/
	if (display == SLR_MAIN_DISPLAY) {
		//disable data path s1 clock
		drvif_color_Set_DRV_SNR_Clock(display, _ENABLE);//make sure clock enable

		/* Disable main VGIP online measure first */
		drvif_mode_disable_main_sub_onlinemeasure_only(display);

		/* Set main VGIP source select to VD/ATV, to make sure 2p related reg. clock enable */
		vgip_chn1_ctrl_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_CTRL_reg);
		vgip_chn1_ctrl_reg.ch1_in_sel = 2;
		IoReg_Write32(VGIP_VGIP_CHN1_CTRL_reg, vgip_chn1_ctrl_reg.regValue);

		two_pixel_mode_ctrl(0);//reset 2 pixel mode
		twopixel_2_oneline_ctrl(0);//reset 2 p to one line
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

	if(/*(src != _SRC_VO)
		&& */((Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_FAILURECOUNT) != 0) || (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_FAILURECOUNT_NOSUPPORT) != 0)))
	{
		//rtd_pr_vsc_info("[DBG] Ignore mode reset failCnt=%d/%d\n", Scaler_DispGetInputInfo(SLR_INPUT_FAILURECOUNT), Scaler_DispGetInputInfo(SLR_INPUT_FAILURECOUNT_NOSUPPORT));
		return;
	}
	Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_FAILURECOUNT, 1);//Avoid to do drvif_mode_resetmode to save time

	rtd_pr_vsc_notice("%s disp:%d \n", __FUNCTION__, display);


	if (display == SLR_MAIN_DISPLAY) {
		drivf_scaler_reset_freerun();
		if(vbe_disp_get_dynamic_memc_bypass_flag()==1){
			// rtd_pr_vsc_emerg("%s.%d\n", __FUNCTION__, __LINE__);
			// enable memc clk
			// rtd_pr_vsc_notice("[jerry_Scaler_MEMC_Bypass_CLK_OnOff_DDD]\n");
			// Scaler_MEMC_Bypass_CLK_OnOff(1);
		}
#ifdef CONFIG_HDR_SDR_SEAMLESS
#ifdef CONFIG_ENABLE_HDR10_HDMI_AUTO_DETECT
		i3ddma_cap1_interrupt_ctrl(FALSE);//disable i3ddma cap1 interrupt
		if (RPC_VIP_sysInfo) {
			RPC_VIP_sysInfo->HDR_info.Ctrl_Item[TV006_HDR_Seamless_Freeze_Status] = 0;
			RPC_VIP_sysInfo->HDR_info.Ctrl_Item[TV006_HDR_Seamless_Freeze_Cnt] = 0;
		}
#endif
#endif

	}

	// Disable all HW Watchdog
	if (display == SLR_MAIN_DISPLAY){
#ifdef CONFIG_I2RND_ENABLE
		//i2rnd case, only change to fw mode @Crixus 20160819
		if(Scaler_I2rnd_get_timing_enable() == _ENABLE)
			drvif_scaler_i2rnd_change_fw_mode();
		else
#endif
			drvif_scaler_i3ddma_vodma_fifo_setting(_DISABLE);//disable fifo mode @Crixus 20160816
	}

	drvif_mode_onlinemeasure_setting(display, src, FALSE, FALSE);//Disable online measure watchdog and interrupt
	drvif_mode_disableonlinemeasure(display);//Disable online measure

	fw_scaler_ctrl_vgip_regen_vsync(display, _DISABLE);
	if (display == SLR_MAIN_DISPLAY) {
		Set_Val_DP_Vsync_Regen_Pos(DP_NO_REGEN_VSYNC);//reset regen position
		fw_scaler_ctrl_dma_vgip_regen_vsync(_DISABLE);
		framesync_syncDoubleDVSReset();
		set_hdr_fake_porch(false);
		reset_hdr_mode(SLR_MAIN_DISPLAY);//disable dolby mode
		rest_dtg_ivs_source();
	}

	if(display == SLR_MAIN_DISPLAY) {
        setup_di_done_flag = (unsigned int *)Scaler_GetShareMemVirAddr(SCALERIOC_SETUP_DI_DONE);
        if(setup_di_done_flag)
        {
            *setup_di_done_flag = Scaler_ChangeUINT32Endian(0);
        }

		drvif_framesync_reset_dv_total_flg(_TRUE);
		drvif_framesync_gatting(_DISABLE);
		drvif_scaler_set_display_mode(DISPLAY_MODE_FRC); //set FRC mode

		//frank@1103 mark to reduce time
		//WaitFor_EN_STOP_MAIN();
		//frank@04152013  add disable ddomain double buffer code avoid I domain IVS loss ++
		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
		ppoverlay_double_buffer_ctrl_Reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
		ppoverlay_double_buffer_ctrl_Reg.dmainreg_dbuf_set = 0;
		ppoverlay_double_buffer_ctrl_Reg.dmainreg_dbuf_en=0;
		ppoverlay_double_buffer_ctrl_Reg.dsubreg_dbuf_set = 0;
		ppoverlay_double_buffer_ctrl_Reg.dsubreg_dbuf_en=0;
		ppoverlay_double_buffer_ctrl_Reg.dreg_dbuf_en = 0;
		ppoverlay_double_buffer_ctrl_Reg.dreg_dbuf_set=0;
		ppoverlay_double_buffer_ctrl_Reg.drm_multireg_dbuf_en = 0;
		ppoverlay_double_buffer_ctrl_Reg.drm_multireg_dbuf_set=0;
		ppoverlay_double_buffer_ctrl_Reg.drm_multireg_dbuf_vsync_sel=0;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_Reg.regValue);
		spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
		up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		//frank@04152013  add disable ddomain double buffer code avoid I domain IVS loss --
		framesync_lastlinefinetune_at_frc_mode_modify_dhtotal_value(_TRUE); //for reset DH total
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

		//drv_GameMode_iv2dv_delay(_DISABLE);  //mac7p disable iv2dv_dly apply can't clear
	}

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
	IoReg_ClearBits(display ? MDOMAIN_CAP_DDR_In2Ctrl_reg : MDOMAIN_CAP_DDR_In1Ctrl_reg, _BIT1);
#else
	IoReg_ClearBits(/*Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg : */MDOMAIN_CAP_DDR_In1Ctrl_reg, _BIT1);
#endif

#endif
#ifdef CONFIG_DUAL_CHANNEL
#ifdef CONFIG_I2RND_ENABLE
	if((display == SLR_MAIN_DISPLAY)&&(check_Sub_I2rnd_done() == FALSE)&&(Scaler_I2rnd_get_timing_enable() == _ENABLE))
	{
		drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);
	}
	if (display == SLR_SUB_DISPLAY)
	{
		drvif_memory_DisableMCap(SLR_SUB_DISPLAY);
	}
#else
	drvif_memory_DisableMCap(display);
#endif
	drvif_memory_EnableMDisp_Or_DisableMDisp(display, _DISABLE);
#else
	drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);
	drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_MAIN_DISPLAY, _DISABLE);
#endif

#if IS_ENABLED(CONFIG_RTK_KDRV_QOS_CONTROL)
	if (display == SLR_MAIN_DISPLAY) {
		//QOS mode reset
		rtk_qos_set_mode(RTK_QOS_NORMAL_MODE);
	}
#endif

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
	 } else {
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

	 if(get_platform_model() == PLATFORM_MODEL_2K) {
	 	//2k mode mdomain borrow 3 CMA address,4M*3=12M
		if(Get_Val_Mdomain_2k_memory_cma_flag()) {
			rtd_pr_vsc_info("M-domian 2k free memory start address\n");
			mdomain_2k_free_cma_memory();
			Set_Val_Mdomain_2k_memory_cma_flag(0);
		}
		//2k mode rotate borrow 1 CMA address,8M
		if(Get_Val_domain_2k_memory_addr()) {
			rtd_pr_vsc_info("M-domian rotate free memory start address %lx\n", Get_Val_domain_2k_memory_addr());
			pli_free(Get_Val_domain_2k_memory_addr());
			Set_Val_domain_2k_memory_addr(0);
		}
	 }
	// Disable Sample Clock in VGIP
	fw_scaler_set_sample(display, _DISABLE);


	if((src != _SRC_VO) || (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_THROUGH_I3DDMA) == FALSE)){
#ifdef CONFIG_I2RND_ENABLE
		if((display == SLR_MAIN_DISPLAY)&&(check_Sub_I2rnd_done() == FALSE)&&(Scaler_I2rnd_get_timing_enable() == _ENABLE))
#endif
		{
			// Disable SRC_Clock_Enbale bit
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
			IoReg_ClearBits(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? SUB_VGIP_VGIP_CHN2_CTRL_reg: VGIP_VGIP_CHN1_CTRL_reg, _BIT0);
#else
			IoReg_ClearBits(/*Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? SUB_VGIP_VGIP_CHN2_CTRL_reg: */VGIP_VGIP_CHN1_CTRL_reg, _BIT0);
#endif
		}
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
		IoReg_Write32(SUB_VGIP_VGIP_CHN2_CTRL_reg, vgip_chn2_ctrl_reg.regValue);

		#if 0 //mac7p no onms
		// Disable On-Line Measure
		onms_onms2_ctrl_reg.regValue = IoReg_Read32(ONMS_onms2_ctrl_reg);
		onms_onms2_ctrl_reg.on2_sycms_clk = 0;
		onms_onms2_ctrl_reg.on2_vs_meas_inv= 0;
		onms_onms2_ctrl_reg.on2_ms_conti= 0;
		onms_onms2_ctrl_reg.on2_start_ms= 0;

		IoReg_Write32(ONMS_onms2_ctrl_reg, onms_onms2_ctrl_reg.regValue);
		#endif
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

#if 0	/* move to vsc disconnect*/
#ifdef Mer2_MEMORY_ALLOC
	drv_memory_I_De_XC_DMA_Memory_Get_From_VBM(0, 0, 0, 1);
	drv_memory_Set_I_De_XC_Mem_Ready_Flag(0, 0);
	drv_memory_I_De_XC_DMA_Memory_Get_From_VBM(1, 0, 0, 1);
	drv_memory_Set_I_De_XC_Mem_Ready_Flag(1, 0);
#endif
#endif
	/* for PQ IP Enable monitor*/
	/*//mac7p pq compile fix
	if(SLR_MAIN_DISPLAY == display)
		Scaler_PQ_IP_Enable_Monitor_clear_Buff();
	*/

	if (SLR_SUB_DISPLAY == display)	// only sum path need to disable SDNR2
		drvif_color_Set_DRV_SNR_Clock(SLR_SUB_DISPLAY, 0);

	//Scaler_Disp3dSetInfo(SLR_DISP_3D_3DTYPE,SLR_3D_2D_ONLY);
	//Scaler_Set3DMode_Attr(SLR_3DMODE_2D);
}
#else
void drvif_mode_resetmode_for_quick_show(unsigned char display)
{
	extern void fw_scaler_ctrl_dma_vgip_regen_vsync(unsigned char enable);
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
	unsigned char src;
	unsigned long flags = 0;//for spin_lock_irqsave
    unsigned int *setup_di_done_flag = NULL;

	src = get_scaler_connect_source(display);

	// must clear 	s1_clk_en, then clear main_imd_smooth_toggle_clk_time2switch_en
	/* disable snr clk @ begin of reset mode for snr hw issue, elieli 20160825*/

	if (display == SLR_MAIN_DISPLAY) {
		//disable data path s1 clock
		drvif_color_Set_DRV_SNR_Clock(display, _DISABLE);
		two_pixel_mode_ctrl(0);
		twopixel_2_oneline_ctrl(0);
	}

	drivf_scaler_reset_freerun();

	// Disable all HW Watchdog
	drvif_mode_disable_dma_onlinemeasure();//disable dma online measure
	drvif_scaler_i3ddma_vodma_fifo_setting(_DISABLE);//disable fifo mode @Crixus 20160816
	drvif_mode_onlinemeasure_setting(display, src, FALSE, FALSE);//Disable online measure watchdog and interrupt
	drvif_mode_disableonlinemeasure(display);//Disable online measure

	fw_scaler_ctrl_vgip_regen_vsync(display, _DISABLE);
	Set_Val_DP_Vsync_Regen_Pos(DP_NO_REGEN_VSYNC);//reset regen position
	fw_scaler_ctrl_dma_vgip_regen_vsync(_DISABLE);
	//framesync_syncDoubleDVSReset();
	set_hdr_fake_porch(false);
	//reset_hdr_mode(SLR_MAIN_DISPLAY);//disable dolby mode
	rest_dtg_ivs_source();

	//drvif_framesync_reset_dv_total_flg(_TRUE);
	//drvif_framesync_gatting(_DISABLE);
	drvif_scaler_set_display_mode(DISPLAY_MODE_FRC); //set FRC mode

	// Avoid to show TV-Bus error for DI
	if(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_THRIP) || Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_RTNR))	// Change to use display avoid to use info make system error, 970616 weihao
		fw_scalerip_disable_ip(display);

	// Disable Measure Watchdog
	fw_scaler_enable_measure_wdg(display, _DISABLE);

#ifndef CONFIG_ENABLE_CHANGE_COLOR_STD_WITH_SMOOTH_TOGGLE
	//unfreeze display if it's freezed
	IoReg_ClearBits(/*Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg : */MDOMAIN_CAP_DDR_In1Ctrl_reg, _BIT1);
#endif


	drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);
	drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_MAIN_DISPLAY, _DISABLE);

	// Disable Sample Clock in VGIP
	fw_scaler_set_sample(display, _DISABLE);

	if((src != _SRC_VO) || (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_THROUGH_I3DDMA) == FALSE))
	{
        IoReg_ClearBits(/*Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? SUB_VGIP_VGIP_CHN2_CTRL_reg: */VGIP_VGIP_CHN1_CTRL_reg, _BIT0);
	}

	if((src == _SRC_VO) && (display == SLR_MAIN_DISPLAY) && (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_THROUGH_I3DDMA) == FALSE)){
		vgip_vgip_chn1_ctrl_RBUS vgip_chn1_ctrl_reg;
		vgip_chn1_ctrl_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_CTRL_reg);
		if(vgip_chn1_ctrl_reg.ch1_in_sel == 1){
			IoReg_ClearBits(VGIP_VGIP_CHN1_CTRL_reg, _BIT0);
		}
	}

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
}
#endif

/*
unsigned char drvif_mode_checkstable(void)
{
#if TEST_PATTERN_GEN
	return TRUE;
#endif

	if(drvif_mode_onlinemeasure(ANALOG_MODE_MEASUREMENT) != TRUE) {
		rtd_pr_adc_debug("\nMode on measure error\n\n");
		return FALSE;
	}

	return TRUE;
}
*/

//USER:LewisLee DATE:2010/03/26
//set abl margin, from small -> large
//let image more stable
void drvif_mode_abl_margin_Setting(void)
{
	unsigned char display;

	display = Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY);

	switch(Scaler_InputSrcGetType((SCALER_DISP_CHANNEL)display))
	{
#ifdef CONFIG_SUPPORT_SRC_ADC
		case _SRC_VGA:
			VGA_ABL_Margin_Setting();
		break;

		case _SRC_YPBPR:
			YPbPr_ABL_Margin_Setting();
		break;
#endif
		default:

		break;
	}
}

#if 0
unsigned short drvif_mode_getHDMIDiditalMeasureIVH(void)
{
	return HDMIDiditalMeasureIVH;
}
#endif

 unsigned char drvif_mode_get_mode_ID(unsigned char search_cnt)
{
	return tINPUTMODE_YPBPR_TABLE[search_cnt].ModeID;
}


void drvif_mode_print_mode_ID(unsigned char mode)
{
	switch(mode)
	{
		case _MODE_640x350_70HZ:	rtd_pr_adc_notice("_MODE_640x350_70HZ\n");break;
		case _MODE_640x350_85HZ:	rtd_pr_adc_notice("_MODE_640x350_85HZ\n");break;
		case _MODE_640x400_56HZ:	rtd_pr_adc_notice("_MODE_640x400_56HZ\n");break;
		case _MODE_640x400_70HZ:	rtd_pr_adc_notice("_MODE_640x400_70HZ\n");break;
		case _MODE_640x400_85HZ:	rtd_pr_adc_notice("_MODE_640x400_85HZ\n");break;
		case _MODE_720x400_50HZ:	rtd_pr_adc_notice("_MODE_720x400_50HZ\n");break;
		case _MODE_720x400_70HZ:	rtd_pr_adc_notice("_MODE_720x400_70HZ\n");break;
		case _MODE_720x400_85HZ:	rtd_pr_adc_notice("_MODE_720x400_85HZ\n");break;
		case _MODE_720x480_60HZ:	rtd_pr_adc_notice("_MODE_720x480_60HZ\n");break;
		case _MODE_640x480_50HZ:	rtd_pr_adc_notice("_MODE_640x480_50HZ\n");break;
		case _MODE_640x480_60HZ:	rtd_pr_adc_notice("_MODE_640x480_60HZ\n");break;
		case _MODE_640x480_67HZ:	rtd_pr_adc_notice("_MODE_640x480_67HZ\n");break;
		case _MODE_640x480_70HZ:	rtd_pr_adc_notice("_MODE_640x480_70HZ\n");break;
		case _MODE_640x480_72HZ:	rtd_pr_adc_notice("_MODE_640x480_72HZ\n");break;
		case _MODE_640x480_75HZ:	rtd_pr_adc_notice("_MODE_640x480_75HZ\n");break;
		case _MODE_640x480_85HZ:	rtd_pr_adc_notice("_MODE_640x480_85HZ\n");break;
		case _MODE_800x600_50HZ:	rtd_pr_adc_notice("_MODE_800x600_50HZ\n");break;
		case _MODE_800x600_56HZ:	rtd_pr_adc_notice("_MODE_800x600_56HZ\n");break;
		case _MODE_800x600_60HZ:	rtd_pr_adc_notice("_MODE_800x600_60HZ\n");break;
		case _MODE_800x600_72HZ:	rtd_pr_adc_notice("_MODE_800x600_72HZ\n");break;
		case _MODE_800x600_75HZ:	rtd_pr_adc_notice("_MODE_800x600_75HZ\n");break;
		case _MODE_800x600_85HZ:	rtd_pr_adc_notice("_MODE_800x600_85HZ\n");break;
		case _MODE_832x624_75HZ:	rtd_pr_adc_notice("_MODE_832x624_75HZ\n");break;
		case _MODE_848x480_60HZ:	rtd_pr_adc_notice("_MODE_848x480_60HZ\n");break;
		case _MODE_1024x768I_43HZ:	rtd_pr_adc_notice("_MODE_1024x768I_43HZ\n");break;
		case _MODE_1024x768_50HZ:	rtd_pr_adc_notice("_MODE_1024x768_50HZ\n");break;
		case _MODE_1024x768_60HZ:	rtd_pr_adc_notice("_MODE_1024x768_60HZ\n");break;
		case _MODE_1024x768_66HZ:	rtd_pr_adc_notice("_MODE_1024x768_66HZ\n");break;
		case _MODE_1024x768_70HZ:	rtd_pr_adc_notice("_MODE_1024x768_70HZ\n");break;
		case _MODE_1024x768_75HZ:	rtd_pr_adc_notice("_MODE_1024x768_75HZ\n");break;
		case _MODE_1024x768_85HZ:	rtd_pr_adc_notice("_MODE_1024x768_85HZ\n");break;
		case _MODE_1152x864_60HZ:	rtd_pr_adc_notice("_MODE_1152x864_60HZ\n");break;
		case _MODE_1152x864_75HZ:	rtd_pr_adc_notice("_MODE_1152x864_75HZ\n");break;
		//case _MODE_1152x870_75HZ:	rtd_pr_adc_debug("_MODE_1152x870_75HZ\n");break;	//davidneedtocheckthis
		case _MODE_1152x900_66HZ:	rtd_pr_adc_notice("_MODE_1152x900_66HZ\n");break;
		case _MODE_1280x720_50HZ:	rtd_pr_adc_notice("_MODE_1280x720_50HZ\n");break;
		case _MODE_1280x720_60HZ:	rtd_pr_adc_notice("_MODE_1280x720_60HZ\n");break;
		case _MODE_1280x720_60HZ_RB:	rtd_pr_adc_notice("_MODE_1280x720_60HZ_RB\n");break;
		case _MODE_1280x720_85HZ:	rtd_pr_adc_notice("_MODE_1280x720_85HZ\n");break;
		case _MODE_1280x768_50HZ:	rtd_pr_adc_notice("_MODE_1280x768_50HZ\n");break;
		case _MODE_1280x768_60HZ:	rtd_pr_adc_notice("_MODE_1280x768_60HZ\n");break;
		case _MODE_1280x768_60HZ_RB:	rtd_pr_adc_notice("_MODE_1280x768_60HZ_RB\n");break;	//davidneedtocheckthis
		case _MODE_1280x768_75HZ:	rtd_pr_adc_notice("_MODE_1280x768_75HZ\n");break;
		case _MODE_1280x768_85HZ:	rtd_pr_adc_notice("_MODE_1280x768_85HZ\n");break;
		case _MODE_1280x800_60HZ:	rtd_pr_adc_notice("_MODE_1280x800_60HZ\n");break;
		case _MODE_1280x800_60HZ_RB:	rtd_pr_adc_notice("_MODE_1280x800_60HZ_RB\n");break;
		//case _MODE_1280x800_75HZ:	rtd_pr_adc_debug("_MODE_1280x800_75HZ\n");break;	//davidneedtocheckthis
		case _MODE_1280x960_50HZ:	rtd_pr_adc_notice("_MODE_1280x960_50HZ\n");break;
		case _MODE_1280x960_60HZ:	rtd_pr_adc_notice("_MODE_1280x960_60HZ\n");break;
		case _MODE_1280x960_75HZ:	rtd_pr_adc_notice("_MODE_1280x960_75HZ\n");break;
		case _MODE_1280x960_85HZ:	rtd_pr_adc_notice("_MODE_1280x960_85HZ\n");break;
		case _MODE_1280x1024_50HZ:	rtd_pr_adc_notice("_MODE_1280x1024_50HZ\n");break;
		case _MODE_1280x1024_60HZ:	rtd_pr_adc_notice("_MODE_1280x1024_60HZ\n");break;
		case _MODE_1280x1024_60HZ_RB:	rtd_pr_adc_notice("_MODE_1280x1024_60HZ_RB\n");break;
		case _MODE_1280x1024_72HZ:	rtd_pr_adc_notice("_MODE_1280x1024_72HZ\n");break;
		case _MODE_1280x1024_75HZ:	rtd_pr_adc_notice("_MODE_1280x1024_75HZ\n");break;
		case _MODE_1280x1024_85HZ:	rtd_pr_adc_notice("_MODE_1280x1024_85HZ\n");break;
		case _MODE_1360x768_60HZ:	rtd_pr_adc_notice("_MODE_1360x768_60HZ\n");break;
		case _MODE_1360x768_60HZ_RB:	rtd_pr_adc_notice("_MODE_1360x768_60HZ_RB\n");break;
		case _MODE_1366x768_60HZ:	rtd_pr_adc_notice("_MODE_1366x768_60HZ\n");break;
		case _MODE_1400x1050_50HZ:	rtd_pr_adc_notice("_MODE_1400x1050_50HZ\n");break;
		case _MODE_1400x1050_60RHZ:	rtd_pr_adc_notice("_MODE_1400x1050_60RHZ\n");break;
		case _MODE_1400x1050_60HZ:	rtd_pr_adc_notice("_MODE_1400x1050_60HZ\n");break;
		case _MODE_1400x1050_75HZ:	rtd_pr_adc_notice("_MODE_1400x1050_75HZ\n");break;
		case _MODE_1440x900_60HZ:	rtd_pr_adc_notice("_MODE_1440x900_60HZ\n");break;
		case _MODE_1440x900_60RHZ:	rtd_pr_adc_notice("_MODE_1440x900_60RHZ\n");break;
		case _MODE_1440x900_75HZ:	rtd_pr_adc_notice("_MODE_1440x900_75HZ\n");break;
		case _MODE_1440x900_85HZ:	rtd_pr_adc_notice("_MODE_1440x900_85HZ\n");break;
		case _MODE_1600x1200_50HZ:	rtd_pr_adc_notice("_MODE_1600x1200_50HZ\n");break;
		case _MODE_1600x1200_60HZ:	rtd_pr_adc_notice("_MODE_1600x1200_60HZ\n");break;
		case _MODE_1600x1200_65HZ:	rtd_pr_adc_notice("_MODE_1600x1200_65HZ\n");break;
		case _MODE_1600x1200_70HZ:	rtd_pr_adc_notice("_MODE_1600x1200_70HZ\n");break;
		case _MODE_1680x1050_50HZ:	rtd_pr_adc_notice("_MODE_1680x1050_50HZ\n");break;
		case _MODE_1680x1050_60HZ:	rtd_pr_adc_notice("_MODE_1680x1050_60HZ\n");break;
		case _MODE_1680x1050_60RHZ:	rtd_pr_adc_notice("_MODE_1680x1050_60RHZ\n");break;
		case _MODE_1680x1050_75HZ:	rtd_pr_adc_notice("_MODE_1680x1050_75HZ\n");break;
		case _MODE_1680x1050_85HZ:	rtd_pr_adc_notice("_MODE_1680x1050_85HZ\n");break;
		case _MODE_1920x1080_50HZ:	rtd_pr_adc_notice("_MODE_1920x1080_50HZ\n");break;
		case _MODE_1920x1080_60HZ:	rtd_pr_adc_notice("_MODE_1920x1080_60HZ\n");break;
		case _MODE_1920x1080_60HZ_138:	rtd_pr_adc_notice("_MODE_1920x1080_60HZ_138\n");break;
		case _MODE_1920x1080_60HZ_148:	rtd_pr_adc_notice("_MODE_1920x1080_60HZ_148\n");break;
		case _MODE_1920x1080_75HZ:	rtd_pr_adc_notice("_MODE_1920x1080_75HZ\n");break;
		case _MODE_VGA_480I:		rtd_pr_adc_notice("_MODE_VGA_480I\n");break;
		case _MODE_VGA_576I:		rtd_pr_adc_notice("_MODE_VGA_576I\n");break;
		case _MODE_VGA_480P:		rtd_pr_adc_notice("_MODE_VGA_480P\n");break;
		case _MODE_VGA_576P:		rtd_pr_adc_notice("_MODE_VGA_576P\n");break;
		case _MODE_VGA_720P50:		rtd_pr_adc_notice("_MODE_VGA_720P50\n");break;
		case _MODE_VGA_720P60:		rtd_pr_adc_notice("_MODE_VGA_720P60\n");break;
		case _MODE_VGA_1080I25:		rtd_pr_adc_notice("_MODE_VGA_1080I25\n");break;
		case _MODE_VGA_1080I30:		rtd_pr_adc_notice("_MODE_VGA_1080I30\n");break;
		case _MODE_VGA_1080P50:		rtd_pr_adc_notice("_MODE_VGA_1080P50\n");break;
		case _MODE_VGA_1080P60:		rtd_pr_adc_notice("_MODE_VGA_1080P60\n");break;
		case _MODE_1024x576_60HZ:	rtd_pr_adc_notice("_MODE_1024x576_60HZ\n");break;
		case _MODE_1920x1080_60Hz_173MAC:	rtd_pr_adc_notice("_MODE_1920x1080_60Hz_173MAC\n");break;
		case _MODE_480I:		rtd_pr_adc_notice("_MODE_480I\n");break;
		case _MODE_576I:		rtd_pr_adc_notice("_MODE_576I\n");break;
		case _MODE_480P:		rtd_pr_adc_notice("_MODE_480P\n");break;
		case _MODE_576P:		rtd_pr_adc_notice("_MODE_576P\n");break;
		case _MODE_720P25:		rtd_pr_adc_notice("_MODE_720P25\n");break;
		case _MODE_720P30:		rtd_pr_adc_notice("_MODE_720P30\n");break;
		case _MODE_720P50:		rtd_pr_adc_notice("_MODE_720P50\n");break;
		case _MODE_720P60:		rtd_pr_adc_notice("_MODE_720P60\n");break;
		case _MODE_1080I25:		rtd_pr_adc_notice("_MODE_1080I25\n");break;
		case _MODE_1080I30:		rtd_pr_adc_notice("_MODE_1080I30\n");break;
		case _MODE_1080P50:		rtd_pr_adc_notice("_MODE_1080P50\n");break;
		case _MODE_1080P60:		rtd_pr_adc_notice("_MODE_1080P60\n");break;
		case _MODE_1080P23:		rtd_pr_adc_notice("_MODE_1080P23\n");break;
		case _MODE_1080P24:		rtd_pr_adc_notice("_MODE_1080P24\n");break;
		case _MODE_1080P25:		rtd_pr_adc_notice("_MODE_1080P25\n");break;
		case _MODE_1080P29:		rtd_pr_adc_notice("_MODE_1080P29\n");break;
		case _MODE_1080P30:		rtd_pr_adc_notice("_MODE_1080P30\n");break;
		case _MODE_1600x900_60HZ_RB:		rtd_pr_adc_notice("_MODE_1600x900_60HZ_RB\n");break;
		case _MODE_1280x1024_60HZ_MSPG:	rtd_pr_adc_notice("_MODE_1280x1024_60HZ_MSPG\n");break;
		case _MODE_1280x768_50HZ_MSPG:	rtd_pr_adc_notice("_MODE_1280x768_50HZ_MSPG\n");break;
		case _MODE_1280x768_60HZ_MSPG:	rtd_pr_adc_notice("_MODE_1280x768_60HZ_MSPG\n");break;
		case _MODE_1280x768_75HZ_MSPG:	rtd_pr_adc_notice("_MODE_1280x768_75HZ_MSPG\n");break;
		case _MODE_1280x768_85HZ_MSPG:	rtd_pr_adc_notice("_MODE_1280x768_85HZ_MSPG\n");break;
		default:				rtd_pr_adc_err("unknown mode\n");break;
	}
}

#ifdef CONFIG_SUPPORT_SRC_VGA
static unsigned char  drvif_mode_customer_judge_confuse_mode(UINT8 mode_index, UINT8 sync_type, UINT16 usHighPulsePeriod)
{
	UINT32 JudgeResult = TRUE;

	ModeInformationType *p_timinginfo;
	StructDisplayInfo *p_dispinfo;

	p_timinginfo = Get_ADC_Timinginfo();
	p_dispinfo = Get_ADC_Dispinfo();

	rtd_pr_adc_notice("#####[%s] start IHSyncPulseCount:0x%x, IVSyncPulseCount:0x%x, IHCount:0x%x, IVCount:0x%x, usHighPulsePeriod:0x%x, ModeID=%d \n",__func__, p_timinginfo->IHSyncPulseCount, p_dispinfo->IVSyncPulseCount, p_timinginfo->IHCount, p_timinginfo->IVCount, usHighPulsePeriod, tINPUTMODE_PRESET_TABLE[mode_index].ModeID);

	//drvif_mode_print_mode_ID(tINPUTMODE_PRESET_TABLE[mode_index].ModeID);
	switch(tINPUTMODE_PRESET_TABLE[mode_index].ModeID)
	{
	case _MODE_VGA_480P:
		//rzhen, _MODE_848x480_60HZ, 1018[10:0]=0x5a
		//MSPG-T306, IHSyncPulseCount:0x33e, IVSyncPulseCount:0x20a, IHCount:0x358, IVCount:0x20c, usHighPulsePeriod:0x1a
		//MSPG-T309, IHSyncPulseCount:0x33f, IVSyncPulseCount:0x20b, IHCount:0x358, IVCount:0x20c, usHighPulsePeriod:0x1a
		//C22294-T213,VESA848X480-60, IHSyncPulseCount:0x58, IVSyncPulseCount:0x8, IHCount:0x365, IVCount:0x204, usHighPulsePeriod:0x5a
		if ((ABS(usHighPulsePeriod ,0x5a) < 10)||(ABS(usHighPulsePeriod ,0x1a) < 5))
			JudgeResult = FALSE;
		break;

	case _MODE_VGA_480I:
		//MSPG-T265, 2880x480I, IHSyncPulseCount:0x637, IVSyncPulseCount:0x102, IHCount:0x6b3, IVCount:0x105, usHighPulsePeriod:0x7b
		//MSPG-T265, 2880x480I, IHSyncPulseCount:0x637, IVSyncPulseCount:0x102, IHCount:0x6b3, IVCount:0x105, usHighPulsePeriod:0x7c
		//MSPG-T265, 2880x480I, IHSyncPulseCount:0x637, IVSyncPulseCount:0x103, IHCount:0x6b3, IVCount:0x106, usHighPulsePeriod:0x7c
		//MSPG-T265, 2880x480I, IHSyncPulseCount:0x638, IVSyncPulseCount:0x103, IHCount:0x6b3, IVCount:0x106, usHighPulsePeriod:0x7c
		//MSPG-T260, VGA_480i, IHSyncPulseCount:0x635, IVSyncPulseCount:0x103, IHCount:0x6b1, IVCount:0x106, usHighPulsePeriod:0x7c
		//if (ABS(p_timinginfo->IHSyncPulseCount,0x634)>2)
		//	JudgeResult = FALSE;
		break;

	case _MODE_640x350_70HZ:
		//_MODE_640x350_70HZ confuse with _MODE_640x400_70HZ
		if (ABS(p_dispinfo->IVSyncPulseCount ,0x1bf) > 0x100)
			JudgeResult = FALSE;
		//In Astro VG859C Pattern, the sync is H:n, V:n, so it will make this two timing confuse
		if (0)////MSPG7800 T318//(tINPUTMODE_PRESET_TABLE[mode_index].PolarityFlag & Scaler_DispGetInputInfo(SLR_INPUT_POLARITY)) == 0)
			JudgeResult = FALSE;
		else if(SOG_MODE == sync_type)
			JudgeResult = FALSE;
		break;

	case _MODE_640x350_85HZ:
		//_MODE_640x350_85HZ confuse with _MODE_640x400_85HZ
		//rtd_pr_adc_emerg("p_timinginfo->IHSyncPulseCount = 0x%x\n",p_timinginfo->IHSyncPulseCount);
		if (ABS(p_timinginfo->IHSyncPulseCount,0x36)>10)
			JudgeResult = FALSE;
		break;

	case _MODE_640x400_70HZ:
		//MSPG-T320,720x400/70, IHSyncPulseCount:0x5b, IVSyncPulseCount:0x2, IHCount:0x359, IVCount:0x1c0, usHighPulsePeriod:0x5c
		//_MODE_640x400_70HZ confuse with _MODE_720x400_70HZ
		if (ABS(usHighPulsePeriod,0x67)>10)
			JudgeResult = FALSE;
		if ((ABS(p_timinginfo->IHCount,0x35a)>1)||(ABS(p_timinginfo->IHSyncPulseCount,0x2f0)>3))
			JudgeResult = FALSE;
		break;

	case _MODE_640x400_85HZ:
		if(ABS(p_timinginfo->IHCount,0x2c8)>1)
			JudgeResult = FALSE;
		break;

	case _MODE_640x480_60HZ:
		//rzhen, _MODE_640x480_60HZ, 1018[10:0]=0x67
		//rzhen, _MODE_848x480_60HZ, 1018[10:0]=0x5a
		//MSPG-T20,640x480/60, IHSyncPulseCount:0x2f2, IVSyncPulseCount:0x20a, IHCount:0x358, IVCount:0x20c, usHighPulsePeriod:0x67, ModeID=8
		if (ABS(usHighPulsePeriod ,0x67) > 10)
			JudgeResult = FALSE;
		break;

	case _MODE_640x480_67HZ:
		//lewis, _MODE_640x480_67HZ _MODE_640x480_70HZ
		if (ABS(usHighPulsePeriod ,0x56) > 10)
			JudgeResult = FALSE;
		break;

	case _MODE_640x480_70HZ:
		//MSPG-T307,848x480/70, IHSyncPulseCount:0x2e6, IVSyncPulseCount:0x1f1, IHCount:0x302, IVCount:0x1f3, usHighPulsePeriod:0x1d
		//MSPG-T310,852X480/70, IHSyncPulseCount:0x2e6, IVSyncPulseCount:0x1, IHCount:0x302, IVCount:0x1f3, usHighPulsePeriod:0x1d
		if (ABS(usHighPulsePeriod ,0x24) > 5)
			JudgeResult = FALSE;
		break;

	case _MODE_640x480_72HZ:
		//MSPG-T37, 640x480/72, IHSyncPulseCount:0x2a7, IVSyncPulseCount:0x204, IHCount:0x2c8, IVCount:0x207, usHighPulsePeriod:0x22
		//MSPG-T305, 656X496/72, IHSyncPulseCount:0x21, IVSyncPulseCount:0x2, IHCount:0x2c8, IVCount:0x207, usHighPulsePeriod:0x23
		if (ABS(p_dispinfo->IVSyncPulseCount ,0x204) > 0x10)
			JudgeResult = FALSE;
		break;

	case _MODE_640x480_75HZ:
		//lewis,  _MODE_640x480_70HZ with _MODE_640x480_75HZ
		if (ABS(usHighPulsePeriod ,0x37) > 10)
			JudgeResult = FALSE;
		break;

	case _MODE_720x400_50HZ:
		//rtd_pr_adc_emerg("p_timinginfo->IHCount = %x\n",p_timinginfo->IHCount);
		if(ABS(p_timinginfo->IHCount, 857) > 2)
			JudgeResult = FALSE;
		break;

	case _MODE_720x480_60HZ:
		//MSPG-T271,1440X480P/59.94,IHSyncPulseCount:0x31b, IVSyncPulseCount:0x206, IHCount:0x359, IVCount:0x20c, usHighPulsePeriod:0x3e
		//MSPG-T271,1440X480P/59.94,IHSyncPulseCount:0x31c, IVSyncPulseCount:0x206, IHCount:0x358, IVCount:0x20c, usHighPulsePeriod:0x3e
		//MSPG-T254,720X480/60,IHSyncPulseCount:0x31a, IVSyncPulseCount:0x206, IHCount:0x358, IVCount:0x20c, usHighPulsePeriod:0x3e
		//MSPG-T43,720X480/59.90,IHSyncPulseCount:0x31b, IVSyncPulseCount:0x206, IHCount:0x358, IVCount:0x20c, usHighPulsePeriod:0x3e
		//C22294-T604,720X480P/60,IHSyncPulseCount:0x31b, IVSyncPulseCount:0x206, IHCount:0x358, IVCount:0x20c, usHighPulsePeriod:0x3e
		if (ABS(usHighPulsePeriod ,0x3e) > 10)
			JudgeResult = FALSE;
		break;

	case _MODE_800x600_50HZ:
		//800x600/50, p_timinginfo->IHSyncPulseCount = 0x327
		//720x576/50, p_timinginfo->IHSyncPulseCount = 0x31f
		if(ABS(p_timinginfo->IHSyncPulseCount, 0x327) > 5)
			JudgeResult = FALSE;
		break;

	case _MODE_800x600_60HZ:
		if(ABS(usHighPulsePeriod, 0x56) > 10)
			return FALSE; //Lewis 20090213, confuse with _MODE_800x600_56HZ 2K tolerance
		break;

	case _MODE_832x624_75HZ:
		//MSPG-T327,832x624/75, IHSyncPulseCount:0x200, IVSyncPulseCount:0x297, IHCount:0x21e, IVCount:0x29a, usHighPulsePeriod:0x1e
		//MSPG-T320,720x400/70, IHSyncPulseCount:0x5b, IVSyncPulseCount:0x2, IHCount:0x359, IVCount:0x1c0, usHighPulsePeriod:0x5c
		if(ABS(usHighPulsePeriod,0x1e)>5)
			JudgeResult = FALSE;
		break;

	case _MODE_848x480_60HZ:
		//MSPG-T306, IHSyncPulseCount:0x33f, IVSyncPulseCount:0x20a, IHCount:0x358, IVCount:0x20c, usHighPulsePeriod:0x1a
		//MSPG-T306, IHSyncPulseCount:0x33e, IVSyncPulseCount:0x20a, IHCount:0x358, IVCount:0x20c, usHighPulsePeriod:0x19
		//MSPG-T306, IHSyncPulseCount:0x33e, IVSyncPulseCount:0x20a, IHCount:0x358, IVCount:0x20c, usHighPulsePeriod:0x1a
		//MSPG-T309, IHSyncPulseCount:0x33f, IVSyncPulseCount:0x20b, IHCount:0x358, IVCount:0x20c, usHighPulsePeriod:0x1a
		//MSPG-T309, IHSyncPulseCount:0x33e, IVSyncPulseCount:0x20b, IHCount:0x358, IVCount:0x20c, usHighPulsePeriod:0x19
		if(p_dispinfo->IVSyncPulseCount > 0x20a)
			JudgeResult = FALSE;
		break;

	case _MODE_1024x768_50HZ:
		//1024x768x50 same as 1280x768x50
		if(ABS(p_dispinfo->IVSyncPulseCount,3)>2)
			JudgeResult = FALSE;
		break;

	case _MODE_1280x768_50HZ:
		//1280x768x50  same as 1024x768x50
		if(ABS(p_dispinfo->IVSyncPulseCount,6)>2)
			JudgeResult = FALSE;
		//MSPG 335 IVCount = 0x317
		if (ABS(p_timinginfo->IVCount,0x317)<2)
			JudgeResult =  _FALSE;
		break;

	case _MODE_1024x768_60HZ:
		if((ABS(usHighPulsePeriod, 0x39) >9 ))
			JudgeResult = FALSE;
		break;

	case _MODE_1024x768_75HZ:
		//Lewis 20090311, 1024x768x75 same as 1280x768x75
		//rtd_pr_adc_emerg("p_timinginfo->IHCount = %x\n",p_timinginfo->IHCount);
		//HPulseCount:0x20 HCount:0x1c0 VCount:0x31f PulsePeriod:0x21 VPulseCount:0x3  1024*768 75hz
		//HPulseCount:0x19d HCount:0x1c0 VCount:0x321 PulsePeriod:0x23 VPulseCount:0x6 1280*768 75hz
		if(p_timinginfo->IHSyncPulseCount > 0x100)
			JudgeResult = FALSE;
		break;

	case _MODE_1024x768_85HZ:
		//MSPG-T314, 1360X768/85, IHSyncPulseCount:0x1e, IVSyncPulseCount:0x5, IHCount:0x183, IVCount:0x330, usHighPulsePeriod:0x1f
		//MSPG-T317, 1366X768/85, IHSyncPulseCount:0x1d, IVSyncPulseCount:0x5, IHCount:0x183, IVCount:0x330, usHighPulsePeriod:0x1f
		//MSPG-T337, 1280X768/85, IHSyncPulseCount:0x1f, IVSyncPulseCount:0x7, IHCount:0x189, IVCount:0x326, usHighPulsePeriod:0x20
		//MSPG-T67, 1024x768/85, IHSyncPulseCount:0x1b, IVSyncPulseCount:0x3, IHCount:0x188, IVCount:0x327, usHighPulsePeriod:0x1b
		//C22294-T17,1024x768/85,  IHSyncPulseCount:0x1b, IVSyncPulseCount:0x3, IHCount:0x188, IVCount:0x327, usHighPulsePeriod:0x1c
		//C22294-T57, 1024x768/85, IHSyncPulseCount:0x1a, IVSyncPulseCount:0x3, IHCount:0x188, IVCount:0x325, usHighPulsePeriod:0x1c
		//C22294-T314, 1024x768/85, IHSyncPulseCount:0x16a, IVSyncPulseCount:0x3, IHCount:0x188, IVCount:0x328, usHighPulsePeriod:0x1d
		if(ABS(usHighPulsePeriod,0x1b)>2)
			JudgeResult = FALSE;
		break;

	case _MODE_1280x720_50HZ:
		//lewis 20090214, compare 1280x720x50 & 720px50
		if((ABS(usHighPulsePeriod, 0x38) > 15))
			JudgeResult = FALSE;
		break;

	case _MODE_1280x720_60HZ:
		//MSPG-T22,1280x720p/60,IHSyncPulseCount:0xd, IVSyncPulseCount:0x5, IHCount:0x257, IVCount:0x2ed, usHighPulsePeriod:0xf
		//MSPG-T79,1280x720p/60,IHSyncPulseCount:0x22c, IVSyncPulseCount:0x4, IHCount:0x25a, IVCount:0x2eb, usHighPulsePeriod:0x2e
		//MSPG-T303,1280x720p/60,IHSyncPulseCount:0x1c, IVSyncPulseCount:0x5, IHCount:0x257, IVCount:0x2ed, usHighPulsePeriod:0x1d
		if((ABS(usHighPulsePeriod, 0x2e) > 5)&&(ABS(usHighPulsePeriod, 0xf) > 5))
			JudgeResult = FALSE;
		if(ABS(p_timinginfo->IHCount, 0x25a) > 2)
			JudgeResult = FALSE;
		break;

	case _MODE_VGA_720P60:
		//MSPG-T22,1280x720p/60,IHSyncPulseCount:0xd, IVSyncPulseCount:0x5, IHCount:0x257, IVCount:0x2ed, usHighPulsePeriod:0xf
		//MSPG-T303,1280x720p/60,IHSyncPulseCount:0x1c, IVSyncPulseCount:0x5, IHCount:0x257, IVCount:0x2ed, usHighPulsePeriod:0x1d
		if(ABS(usHighPulsePeriod, 0xf) > 5)
			JudgeResult = FALSE;
		break;

	case _MODE_1280x720_60HZ_RB:
		//MSPG-T303,1280x720p/60,IHSyncPulseCount:0x1c, IVSyncPulseCount:0x5, IHCount:0x257, IVCount:0x2ed, usHighPulsePeriod:0x1d
		break;

	case _MODE_1280x768_60HZ:
		//MSPG-T315,1366x768/WXGA60,IHSyncPulseCount:0x208, IVSyncPulseCount:0x31a, IHCount:0x234, IVCount:0x31d, usHighPulsePeriod:0x2d
		//C22294-T260,1366x768/60,IHSyncPulseCount:0x2c, IVSyncPulseCount:0x3, IHCount:0x234, IVCount:0x31d, usHighPulsePeriod:0x2d
		//MSPG-T335,1280x768/WXGA60,IHSyncPulseCount:0x207, IVSyncPulseCount:0x6, IHCount:0x235, IVCount:0x31a, usHighPulsePeriod:0x2e
		//MSPG-T88,1280x768/60,IHSyncPulseCount:0x2c,IVSyncPulseCount:0x3,IHCount:0x235, IVCount:0x31a, usHighPulsePeriod:0x2d
		//C22294-T221,1280x768/60,IHSyncPulseCount:0x209, IVSyncPulseCount:0x6, IHCount:0x234, IVCount:0x31d, usHighPulsePeriod:0x2b
		if(usHighPulsePeriod <= 0x15)
			JudgeResult = FALSE;

		if((2 * p_timinginfo->IHSyncPulseCount) > p_timinginfo->IHCount){
			if((p_dispinfo->IVSyncPulseCount - 0x6) > 0x100)
				JudgeResult = FALSE;
		}else{
			if((p_timinginfo->IVCount - 0x31a) > 0x2)
				JudgeResult = FALSE;
		}
		//MSPG 335 IVCount = 0x31b
		if (ABS(p_timinginfo->IVCount,0x31b)<2)
			JudgeResult =  _FALSE;
		break;

	case _MODE_1366x768_60HZ:
		//MSPG 335 IVCount = 0x31b
		if (ABS(p_timinginfo->IVCount,0x31b)<2)
			JudgeResult =  _FALSE;
		break;

	case _MODE_1280x768_60HZ_RB:
		//C22294-T286,1366x768/60-RB,IHSyncPulseCount:0x14, IVSyncPulseCount:0x31d, IHCount:0x231, IVCount:0x31f, usHighPulsePeriod:0x15
		//C22294-T220,1280x768/60,IHSyncPulseCount:0xb, IVSyncPulseCount:0x30f, IHCount:0x238, IVCount:0x315, usHighPulsePeriod:0xd
		//C22294-T348,CVT-1280x768/60,IHSyncPulseCount:0xc, IVSyncPulseCount:0x30f, IHCount:0x238, IVCount:0x315, usHighPulsePeriod:0xd

		if(ABS(usHighPulsePeriod ,0xc) > 5)
			JudgeResult = FALSE;
		break;

	case _MODE_1280x800_60HZ:
		if (ABS(p_timinginfo->IHSyncPulseCount,0x217)>10)
			JudgeResult = FALSE;
		break;

	case _MODE_1280x960_60HZ:
		//rtd_pr_adc_emerg("p_timinginfo->IHSyncPulseCount = 0x%x\n",p_timinginfo->IHSyncPulseCount);
		if ((ABS(p_timinginfo->IHSyncPulseCount,0x1b)>6)&(ABS(p_timinginfo->IHSyncPulseCount,0x1b)<10))
			JudgeResult = FALSE;
		break;

	case _MODE_1280x800_60HZ_RB:
		//usHighPulsePeriod = 0x0c
		if (usHighPulsePeriod >= 0x10)
			JudgeResult = FALSE;
		break;

	case _MODE_1280x1024_50HZ:
		//rtd_pr_adc_emerg("p_timinginfo->IHCount = %x\n",p_timinginfo->IHCount);
		if(ABS(p_timinginfo->IHCount, 511) > 2)
			JudgeResult = FALSE;
		break;

	case _MODE_1280x1024_60HZ:
		//MSPG T301: IHSyncPulseCount:0x17c, IVSyncPulseCount:0x41c, IHCount:0x1a9, IVCount:0x41f, usHighPulsePeriod:0x2e
		//C23291 T144: IHSyncPulseCount:0x1b, IVSyncPulseCount:0x3, IHCount:0x1a5, IVCount:0x429, usHighPulsePeriod:0x1c
		if(2 * p_timinginfo->IHSyncPulseCount >  p_timinginfo->IHCount){//negative polarity
			if((ABS(usHighPulsePeriod, 0x2e) > 6) || (ABS(p_timinginfo->IHCount, 421) > 5))
				JudgeResult = FALSE;
		}else{
			if((ABS(usHighPulsePeriod, 0x1c) > 6) || (ABS(p_timinginfo->IHCount, 421) > 2))
				JudgeResult = FALSE;
		}
		//MSPG T301 vtotal = 0x420
		if (ABS(p_timinginfo->IVTotal,0x420)<2)
			JudgeResult =  _FALSE;
		break;

	case _MODE_1360x768_60HZ:
		//Lewis 20090211, 1280x768x60 same as 1360x768x60, so use polarity to judge
		//if ((tINPUTMODE_PRESET_TABLE[mode_index].PolarityFlag & info->Polarity) == 0)
		//	JudgeResult = _FALSE;
		if (ABS(usHighPulsePeriod ,0x29) < 2)		// Should be VGA 1366*768*60 not VGA 1360*768*60		//VGA1366x768@60
			JudgeResult = FALSE;
		if (ABS(p_timinginfo->IHSyncPulseCount ,0x1f9) >5){//MSPG T312 can't display
			if (ABS(usHighPulsePeriod ,0x24) >5)		// Should be VGA 1366*768*60 not VGA 1360*768*60		//VGA1366x768@60
				JudgeResult = FALSE;
		}
		break;

	case _MODE_1400x1050_50HZ:
		//rtd_pr_adc_emerg("p_timinginfo->IHCount = %x\n",p_timinginfo->IHCount);
		if(ABS(p_timinginfo->IHCount, 498) > 2)
			JudgeResult = FALSE;
		break;

	case _MODE_1400x1050_60RHZ:
		//rtd_pr_adc_emerg("p_timinginfo->IHCount = %x\n",p_timinginfo->IHCount);
		if((ABS(p_timinginfo->IHCount, 416) > 2) ||(ABS(usHighPulsePeriod, 0x09) > 10))
			JudgeResult = FALSE;
		else
			YPbPr_SetClampPulse(SYNC_PROCESSOR1, 0x04, 0x09, 0x04, 0x09, CLAMP_ALL);
		break;

	case _MODE_1400x1050_60HZ:
		if(ABS(p_dispinfo->IVSyncPulseCount,3)>1)
			JudgeResult = FALSE;
		//rtd_pr_adc_emerg("p_timinginfo->IHCount = %x\n",p_timinginfo->IHCount);
		if((ABS(p_timinginfo->IHCount, 412) > 2) || (ABS(usHighPulsePeriod, 0x20) > 5))
			JudgeResult = FALSE;
		break;


	case _MODE_1440x900_60HZ:
		//IHsyncPulseCount : 443
		if(usHighPulsePeriod < 14 )
			JudgeResult = FALSE;
		break;

	case _MODE_1440x900_60RHZ:
		    //lewis, to prevent confuse with _MODE_1440x900_60HZ, the usHighPulsePeriod = 9
		    //IHsyncPulseCount : 9
		    if(usHighPulsePeriod >= 14 )
			     JudgeResult = FALSE;
		    else if (ABS(usHighPulsePeriod, 0x06) < 3)
			     JudgeResult = FALSE;
		    else
		           YPbPr_SetClampPulse(SYNC_PROCESSOR1, 0x04, 0x09, 0x04, 0x09, CLAMP_ALL);
		break;

	case _MODE_1600x1200_60HZ:
		//confuse with 1920x1200/60
		if(ABS(usHighPulsePeriod,0X20)>5)
			JudgeResult = FALSE;
		break;

	case _MODE_1680x1050_60HZ:
		//lewis 20090214, _MODE_1680x1050_60HZ with _MODE_1920x1080_60RHZ
		//rtd_pr_adc_emerg("p_timinginfo->IHCount = %x\n",p_timinginfo->IHCount);
		//rtd_pr_adc_emerg("p_timinginfo->IVCount = %x\n",p_timinginfo->IVCount);

		if(ABS(p_dispinfo->IVSyncPulseCount,5)>1)
			JudgeResult = FALSE;
		break;

	case _MODE_1680x1050_60RHZ:
		//lewis 20090214, _MODE_1680x1050_60RHZ with _MODE_1400x1050_60RHZ
	      //rtd_pr_adc_emerg("p_timinginfo->IHCount = %x\n",p_timinginfo->IHCount);
		if((ABS(p_timinginfo->IHCount, 0x1a0) > 3) || (ABS(usHighPulsePeriod, 0x05) > 2))
			JudgeResult = FALSE;
			//	YPbPr_SetClampPulse(SYNC_PROCESSOR1, 0x04, 0x09, 0x04, 0x09, CLAMP_ALL);
		break;

	case _MODE_1680x1050_50HZ:
		if(ABS(usHighPulsePeriod, 0x27) > 5)
			JudgeResult = FALSE;
		break;

	case _MODE_1920x1080_50HZ:
		//lewis 20090214, compare 1920x1080x50 & 1080px50
		if((ABS(usHighPulsePeriod, 0x25) > 10))
			JudgeResult = FALSE;
		break;
#if 0 //removed for 173MHz
	case _MODE_1920x1080_60HZ:
		//lewis 20090214, _MODE_1920x1080_50HZ with _MODE_1080P50
		if((ABS(usHighPulsePeriod, 0x26) > 10))
			JudgeResult = FALSE;
		break;
#endif
	case _MODE_1920x1080_60HZ_138:
		#if 0 //improve condition
		if(ABS(p_timinginfo->IVCount, tINPUTMODE_PRESET_TABLE[mode_index].IVTotal ) > 3)
			JudgeResult =  _FALSE;
		#else
		//p_timinginfo->IHSyncPulseCount = 0x5, _MODE_1920x1080_60HZ_138 usHighPulsePeriod = 0x6
		//p_timinginfo->IHSyncPulseCount = 0x17f, _MODE_1920x1080_60HZ_148 usHighPulsePeriod = 0x8
		//HPulseCount:0x5 HCount:0x194 VCount:0x456 PulsePeriod:0x6 VPulseCount:0x452
		if ((ABS(p_timinginfo->IHSyncPulseCount,0x5)>5)||(ABS(usHighPulsePeriod,0x5)>2))
			JudgeResult =  _FALSE;
		if(ABS(p_timinginfo->IVCount, 0x456) > 7)//148:0x464  138:0x456
			JudgeResult =  _FALSE;
		#endif
		break;

	case _MODE_1920x1080_60HZ_148:
		//p_timinginfo->IHSyncPulseCount = 0x17f, _MODE_1920x1080_60HZ_148 usHighPulsePeriod = 0x10=16  usHighPulsePeriod = 0x8=8
		//p_timinginfo->IHSyncPulseCount = 0x171, C23291 #3560 1920x1080_173MHz usHighPulsePeriod = 0x20=32
		//HPulseCount:0x7 HCount:0x18f VCount:0x464 PulsePeriod:0x8 VPulseCount:0x5
		//_MODE_1920x1080_60HZ_138  p_timinginfo->IHSyncPulseCount = 5 usHighPulsePeriod = 0x6=6
		if ((ABS(p_timinginfo->IHSyncPulseCount,0x17f)>5)&&(p_timinginfo->IHSyncPulseCount > 0x20))
			JudgeResult =  _FALSE;
		break;

	case _MODE_VGA_1080P60:
		//p_timinginfo->IHSyncPulseCount = 0x171, C23291 #3520 1920x1080_173MHz
		if (ABS(p_timinginfo->IHSyncPulseCount,0x171)<5)
			JudgeResult =  _FALSE;
		break;

	case _MODE_1280x768_75HZ:
		//MSPG T336
		if (ABS(p_timinginfo->IVCount,0x321)>0)
			JudgeResult =  _FALSE;
		break;

	case  _MODE_VGA_576I:
		//MSPG-T30,720x576I, IHSyncPulseCount:0x641, IVSyncPulseCount:0x134, IHCount:0x6bf, IVCount:0x137, usHighPulsePeriod:0x7e
		if (ABS(p_timinginfo->IHCount,0x690)<2 && ABS(p_timinginfo->IVCount,0x321)<2)
			JudgeResult =  _FALSE;
		break;

	case _MODE_1280x768_85HZ:
		//MSPG T337
		if (ABS(p_timinginfo->IVCount,0x327)<2)
			JudgeResult =  _FALSE;
		break;

	case _MODE_1024x576_60HZ:
		//MSPG T301
		if (ABS(p_timinginfo->IVCount,0x420)<2)
			JudgeResult =  _FALSE;
		break;

	default:
		//do nothing
		JudgeResult = TRUE;
		break;
	}
	rtd_pr_adc_notice("#####[%s] end JudgeResult:%d\n",__func__,JudgeResult);
	return JudgeResult;
}
#endif//#ifdef CONFIG_SUPPORT_SRC_VGA

static unsigned char drvif_hdmi_mode_customer_judge_confuse_mode(unsigned char mode_index)
{
	unsigned short IHCount = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IHCOUNT);
	unsigned short IVCount = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVCOUNT);
	unsigned int JudgeResult = TRUE;
	unsigned int preset_table_size = sizeof(tINPUTMODE_PRESET_TABLE)/sizeof(ModeTableType);
	unsigned int hdmi_table_size = sizeof(tINPUTMODE_HDMI_TABLE)/sizeof(ModeTableType);

	if(_TRUE == get_hdmi_idntify_mode()) // HDMI mode
	{
//		rtd_pr_onms_info("drvif_hdmi_mode_customer_judge_confuse_mode, HDMI %x.....\n", tINPUTMODE_HDMI_TABLE[mode_index].ModeID);
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

//		rtd_pr_onms_info("HDMI JudgeResult : %x\n", JudgeResult);
		return JudgeResult;
	}

	if (mode_index >= preset_table_size) {
			return FALSE;
	}
//	rtd_pr_onms_info("drvif_hdmi_mode_customer_judge_confuse_mode, DVI %x.....\n", tINPUTMODE_PRESET_TABLE[mode_index].ModeID);

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

void offline_semaphore_init(void)
{
	sema_init(&Offline_Semaphore, 1);
}

struct semaphore* get_offline_semaphore(void)
{
	return &Offline_Semaphore;
}



void clear_online_status(unsigned char display)
{
	onms_onms3_status_RBUS onms_onms3_status_reg;
	onms_onms1_status_RBUS onms_onms1_status_reg;

	if(display == SLR_MAIN_DISPLAY)
	{
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
}

#ifdef CONFIG_SUPPORT_SRC_VGA
unsigned char drvif_mode_online_result_check(StructDisplayInfo* p_dispinfo)
{
	onms_onms1_vshs_high_period_RBUS onms_onms1_vshs_high_period_reg;

	onms_onms1_vshs_high_period_reg.regValue = IoReg_Read32(ONMS_onms1_vshs_high_period_reg);

	rtd_pr_adc_notice("###### [%s(%d)]on1_vs_high_period=0x%08x, IVSyncPulseCount=0x%08x\n", __func__, __LINE__, onms_onms1_vshs_high_period_reg.on1_vs_high_period, p_dispinfo->IVSyncPulseCount);

	if(ABS(onms_onms1_vshs_high_period_reg.on1_vs_high_period, p_dispinfo->IVSyncPulseCount)>2){
		rtd_pr_adc_notice("###### [%s(%d)]0xB8021230=0x%08x\n", __func__, __LINE__, IoReg_Read32(0xB8021230));
		rtd_pr_adc_notice("###### [%s(%d)]0xB8021234=0x%08x\n", __func__, __LINE__, IoReg_Read32(0xB8021234));
		rtd_pr_adc_notice("###### [%s(%d)]0xB8021238=0x%08x\n", __func__, __LINE__, IoReg_Read32(0xB8021238));
		rtd_pr_adc_notice("###### [%s(%d)]0xB802110c=0x%08x\n", __func__, __LINE__, IoReg_Read32(0xB802110c));

		return false;
	}

	return true;
}
#endif

void vga_clock_online_setting(unsigned char display)
{//before clock adjust. we only check on3_vs_per_timeout bit13
	if(get_force_i3ddma_enable(display) == true)
	{
		onms_onms3_watchdog_en_RBUS onms_onms3_watchdog_en_reg;
		onms_onms3_interrupt_en_RBUS onms_onms3_interrupt_en_reg;

		if(display == SLR_MAIN_DISPLAY)
		{
			onms_onms3_watchdog_en_reg.regValue = 0;
			onms_onms3_watchdog_en_reg.on3_wd_vs_per_timeout = 1;
			IoReg_Write32(ONMS_onms3_watchdog_en_reg, onms_onms3_watchdog_en_reg.regValue);
			onms_onms3_interrupt_en_reg.regValue = 0;
			onms_onms3_interrupt_en_reg.on3_ie_vs_per_timeout = 1;
			IoReg_Write32(ONMS_onms3_interrupt_en_reg, onms_onms3_interrupt_en_reg.regValue);
		}
	}
	else
	{
		onms_onms1_watchdog_en_RBUS onms_onms1_watchdog_en_reg;
		onms_onms1_interrupt_en_RBUS onms_onms1_interrupt_en_reg;

		if(display == SLR_MAIN_DISPLAY)
		{
			onms_onms1_watchdog_en_reg.regValue = 0;
			onms_onms1_watchdog_en_reg.on1_wd_vs_per_timeout = 1;
			IoReg_Write32(ONMS_onms1_watchdog_en_reg, onms_onms1_watchdog_en_reg.regValue);
			onms_onms1_interrupt_en_reg.regValue = 0;
			onms_onms1_interrupt_en_reg.on1_ie_vs_per_timeout = 1;
			IoReg_Write32(ONMS_onms1_interrupt_en_reg, onms_onms1_interrupt_en_reg.regValue);
		}
	}
}

unsigned char vga_clock_check_onlinemeasure_status(unsigned char display)
{//Return FALSE: Online measure has error status. for clock chnage using
	if(display == SLR_MAIN_DISPLAY)
	{
		if(get_force_i3ddma_enable(display) == true)
		{
			onms_onms3_status_RBUS onms_onms3_status_reg;
			onms_onms3_ctrl_RBUS onms_onms3_ctrl_reg;

			onms_onms3_ctrl_reg.regValue = IoReg_Read32(ONMS_onms3_ctrl_reg);
			if((onms_onms3_ctrl_reg.on3_ms_conti != 1) || (onms_onms3_ctrl_reg.on3_start_ms != 1))
				return FALSE;//dma online already disable

			onms_onms3_status_reg.regValue = IoReg_Read32(ONMS_onms3_status_reg);
			if(onms_onms3_status_reg.on3_vs_per_timeout)
				return FALSE; //already signal unstable
		}
		else
		{
			onms_onms1_status_RBUS onms_onms1_status_reg;
			onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;

			//onms_onms1_ctrl_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_reg);
			//onms_onms1_ctrl_reg.on1_start_ms = 1;
			//IoReg_Write32(ONMS_onms1_ctrl_reg, onms_onms1_ctrl_reg.regValue);
			onms_onms1_ctrl_reg.regValue = IoReg_Read32(ONMS_onms1_ctrl_reg);
			if((onms_onms1_ctrl_reg.on1_ms_conti != 1) || (onms_onms1_ctrl_reg.on1_start_ms != 1)){
				rtd_pr_adc_err("error2-23[%s(%d)]on1_ms_conti=%d,on1_start_ms=%d\n",__FUNCTION__,__LINE__,onms_onms1_ctrl_reg.on1_ms_conti,onms_onms1_ctrl_reg.on1_start_ms);
				return FALSE;//main online already disable
			}
			onms_onms1_status_reg.regValue = IoReg_Read32(ONMS_onms1_status_reg);
			if(onms_onms1_status_reg.on1_vs_per_timeout)
				return FALSE; //already signal unstable
		}
	}

	return TRUE;
}



void vga_clock_clear_online_status(unsigned char display)
{//this is for clock adjust use. clear h error status
	if(display == SLR_MAIN_DISPLAY)
	{
		if(get_force_i3ddma_enable(display) == true)
		{
			onms_onms3_status_RBUS onms_onms3_status_reg;
			onms_onms3_status_RBUS cur_onms_onms3_status_reg;//for print

			cur_onms_onms3_status_reg.regValue = IoReg_Read32(ONMS_onms3_status_reg);

			onms_onms3_status_reg.regValue = 0;
			onms_onms3_status_reg.on3_hs_active_pixel_var = 1;//bit3
			onms_onms3_status_reg.on3_htotal_chg = 1;//bit4
			onms_onms3_status_reg.on3_hact_chg = 1;//bit5
			onms_onms3_status_reg.on3_hs_pol_chg = 1;//bit8
			onms_onms3_status_reg.on3_hs_per_overflow = 1;//bit10
			onms_onms3_status_reg.on3_hs_per_over_range = 1;//bit14
			onms_onms3_status_reg.on3_vs_per_over_range = 1;//bit15//h total change, so v total change
			onms_onms3_status_reg.on3_hs_pol_out = 1;//bit16
			onms_onms3_status_reg.on3_hs_meas_over_range = 1;//bit21
			IoReg_SetBits(ONMS_onms3_status_reg, onms_onms3_status_reg.regValue);

			rtd_pr_adc_notice("\r\n####vga_clock_clear_online_h_status dma online current status:%08x####\r\n", cur_onms_onms3_status_reg.regValue);
		}
		else
		{
			onms_onms1_status_RBUS onms_onms1_status_reg;
#ifndef UT_flag
			onms_onms1_status_RBUS cur_onms_onms1_status_reg;//for print

			cur_onms_onms1_status_reg.regValue = IoReg_Read32(ONMS_onms1_status_reg);
#endif
			onms_onms1_status_reg.regValue = 0;
			onms_onms1_status_reg.on1_hs_active_pixel_var = 1;//bit3
			onms_onms1_status_reg.on1_htotal_chg = 1;//bit4
			onms_onms1_status_reg.on1_hact_chg = 1;//bit5
			onms_onms1_status_reg.on1_hs_pol_chg = 1;//bit8
			onms_onms1_status_reg.on1_hs_per_overflow = 1;//bit10
			onms_onms1_status_reg.on1_hs_per_over_range = 1;//bit14
			onms_onms1_status_reg.on1_vs_per_over_range = 1;//bit15//h total change, so v total change
			onms_onms1_status_reg.on1_hs_pol_out = 1;//bit16
			onms_onms1_status_reg.on1_hs_meas_over_range = 1;//bit21
			IoReg_SetBits(ONMS_onms1_status_reg, onms_onms1_status_reg.regValue);
#ifndef UT_flag
			rtd_pr_adc_notice("\r\n####vga_clock_clear_online_h_status main online current status:%08x####\r\n", cur_onms_onms1_status_reg.regValue);
#endif
		}
	}
}

void scaler_mdom_stop(void)
{
	mdomain_vi_main_mdom_vi_doublebuffer_RBUS mdomain_vi_main_mdom_vi_doublebuffer_reg;
	//mdomain_cap_cap_in1_enable_RBUS mdomain_cap_cap_in1_enable_reg;


	//1.disable cap_en,disable vi_cap, disable disp_en
	drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);

	rtd_clearbits(MDOMAIN_VI_MAIN_DMACTL_reg, _BIT8);
	rtd_clearbits(MDOMAIN_VI_MAIN_GCTL_reg, _BIT4);
	mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg);
	mdomain_vi_main_mdom_vi_doublebuffer_reg.vi_db_apply = 1;
	rtd_outl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg, mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue);

	drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_MAIN_DISPLAY, _DISABLE);

	//2.clear sb2 arbitration
#if 0
	mdelay(40); // delay 40ms for bufer update

	while(((rtd_inl(TVSB2_TV_SB2_DCU1_ARB_SR0_reg) & (_BIT2|_BIT3|_BIT18|_BIT19)) != 0) && (timeout--))
	{
		rtd_pr_vsc_emerg("mdomain ip arbrition wait!\n");
		mdelay(1); // delay 1ms for bufer update
	}
	if(timeout == 0)
		rtd_pr_vsc_emerg("mdomain ip arbrition timeout!\n");

	rtd_clearbits(TVSB2_TV_SB2_DCU1_ARB_CR1_reg, (_BIT2|_BIT3));
#endif

	//3.capM block->seq
	rtd_clearbits(MDOMAIN_VI_MAIN_DMACTL_reg, _BIT31);
	mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg);
	mdomain_vi_main_mdom_vi_doublebuffer_reg.vi_db_apply = 1;
	rtd_outl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg, mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue);

	//4.dispM block->seq
	rtd_clearbits(MDOMAIN_DISP_DDR_Main_BM_DMA_reg, _BIT0);
	drvif_memory_set_dbuffer_write(SLR_MAIN_DISPLAY);

	//5.set sb2 arbitration
#if 0
	mdelay(40);

	timeout = 100;
	while(((rtd_inl(TVSB2_TV_SB2_DCU1_ARB_SR0_reg) & (_BIT2|_BIT3|_BIT18|_BIT19)) != 0) && (timeout--))
	{
		rtd_pr_vsc_emerg("mdomain ip arbrition wait!\n");
		mdelay(1); // delay 1ms for bufer update
	}
	if(timeout == 0)
	rtd_pr_vsc_emerg("mdomain ip arbrition timeout!\n");
	rtd_setbits(TVSB2_TV_SB2_DCU1_ARB_CR1_reg, (_BIT2|_BIT3));
#endif


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
{//this api is for wait online measure ready to pop frame rate result. frame_rate is *10
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
		need_time = 900000 * ONLINE_READY_WAIT_FRAME_NUM / vfreq;//wait 3 frame
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


