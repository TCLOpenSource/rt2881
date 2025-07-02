/*=============================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2005
  * All rights reserved.
  * ============================================ */

/*================= File Description ================= */
/**
 * @file
 * 	This file is for mode declarations.
 *
 * @author 	$Author: $
 * @date 	$Date: $
 * @version $Revision:  $
 * @ingroup
 */

/**
 * @addtogroup
 * @{
 */
#ifndef _MODE_H_
#define _MODE_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include <rtd_types.h>
#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerDrvCommon.h>
#else
	#include <scalercommon/scalerDrvCommon.h>
#endif

#define HDMI_VRR_ONLINEMEASURE_SUPPORT//for hdmi 2.1 VRR online measure support
#define VRR_MIN_FRAMERATE	24000

#ifdef CONFIG_SCALER_BRING_UP
#define DISABLE_ONLINE_CHECK
#endif

#ifdef DISABLE_ONLINE_CHECK
#define HDMI_4K120_VRR_ONLINE_ENABLE_MASK (0)//HDMI 4k120 vrr
#define HDMI_4K120_FREESYNC_ONLINE_ENABLE_MASK (0)//HDMI 4k120 free sync
#define HDMI_4K120_QMS_ONLINE_ENABLE_MASK (0)//HDMI 4k120 QMS
#define HDMI_4K120_ONLINE_ENABLE_MASK (0)//HDMI 4k120
#define HDMI_VRR_ONLINE_ENABLE_MASK  (0)//HDMI VRR
#define HDMI_QMS_ONLINE_ENABLE_MASK	 (0)//HDMI QMS
#define HDMI_FREESYNC_ONLINE_ENABLE_MASK  (0)//HDMI Freesync
#define HDMI_NORMAL_ONLINE_ENABLE_MASK  (0)//HDMI normal
#define DP_VRR_ONLINE_ENABLE_MASK  (0)//DP VRR
#define DP_QMS_ONLINE_ENABLE_MASK  (0)//DP QMS
#define DP_FREESYNC_ONLINE_ENABLE_MASK  (0)//DP Freesync
#define DP_NORMAL_ONLINE_ENABLE_MASK  (0)//DP normal
#define AVD_ONLINE_ENABLE_MASK  (0)//AV
#define VO_CP_FS_ONLINE_ENABLE_MASK  (0)//VO adaptive FS
#define VO_CP_FRC_ONLINE_ENABLE_MASK  (0)//VO adaptive FS
#define VO_FS_ONLINE_ENABLE_MASK  (0)//VO FS
#define VO_FRC_ONLINE_ENABLE_MASK  (0)//VO FRC
#else
#define HDMI_4K120_VRR_ONLINE_ENABLE_MASK (_BIT4 | _BIT5 | _BIT10  | _BIT11 | _BIT13 | _BIT22)//HDMI 4k120 vrr
#define HDMI_4K120_FREESYNC_ONLINE_ENABLE_MASK (_BIT4 | _BIT5 | _BIT10  | _BIT13 | _BIT22)//HDMI 4k120 free sync
#define HDMI_4K120_QMS_ONLINE_ENABLE_MASK (_BIT4 | _BIT5 | _BIT10  | _BIT11 | _BIT13 | _BIT22)//HDMI 4k120 QMS
#define HDMI_4K120_ONLINE_ENABLE_MASK (_BIT4 | _BIT5 | _BIT7 | _BIT8 | _BIT9 | _BIT10 | _BIT11 | _BIT13 | _BIT15 | _BIT22)//HDMI 4k120
#define HDMI_VRR_ONLINE_ENABLE_MASK  (_BIT2 | _BIT4 | _BIT5 | _BIT8 | _BIT9 | _BIT11 | _BIT13 | _BIT22)//HDMI VRR
#define HDMI_QMS_ONLINE_ENABLE_MASK	 (_BIT2 | _BIT4 | _BIT5 | _BIT8 | _BIT9 | _BIT11 | _BIT13)//HDMI QMS
#define HDMI_FREESYNC_ONLINE_ENABLE_MASK  (_BIT4 | _BIT5 | _BIT8 | _BIT9 | _BIT13 | _BIT22)//HDMI Freesync
#define HDMI_NORMAL_ONLINE_ENABLE_MASK  (_BIT2 | _BIT4 | _BIT5 | _BIT7 | _BIT8 | _BIT9 | _BIT10 | _BIT11 | _BIT13 | _BIT15 | _BIT22)//HDMI normal
#define DP_VRR_ONLINE_ENABLE_MASK  (_BIT2 | _BIT4 | _BIT5 | _BIT8 | _BIT9 | _BIT11 | _BIT13 | _BIT22)//DP VRR
#define DP_QMS_ONLINE_ENABLE_MASK  (_BIT2 | _BIT4 | _BIT5 | _BIT8 | _BIT9 | _BIT11 | _BIT13)//DP QMS
#define DP_FREESYNC_ONLINE_ENABLE_MASK  (_BIT4 | _BIT5 | _BIT8 | _BIT9 | _BIT13 | _BIT22)//DP Freesync
#define DP_NORMAL_ONLINE_ENABLE_MASK  (_BIT2 | _BIT4 | _BIT5 | _BIT7 | _BIT8 | _BIT9 | _BIT10 | _BIT11 | _BIT13 | _BIT15 | _BIT22)//DP normal
#define AVD_ONLINE_ENABLE_MASK  (_BIT3 | _BIT8 | _BIT9 | _BIT11 | _BIT13 | _BIT15)//AV
#define VO_CP_FS_ONLINE_ENABLE_MASK  (_BIT11 | _BIT13)//VO adaptive FS
#define VO_CP_FRC_ONLINE_ENABLE_MASK  (_BIT11 | _BIT13 | _BIT14 | _BIT15)//VO adaptive FS
#define VO_FS_ONLINE_ENABLE_MASK  (_BIT11 | _BIT13)//VO FS
#define VO_FRC_ONLINE_ENABLE_MASK  (_BIT11 | _BIT13)//VO FRC
#endif

#define _ON_MEAS_TO_SEL_38MS		0
#define _ON_MEAS_TO_SEL_76MS		1

#define CONFIG_ONLINE_OFFLINE_XTAL_FREQ_24M 24000000
#define CONFIG_ONLINE_OFFLINE_XTAL_FREQ_27M 27000000
#define CONFIG_OFFLINE_XTAL_FREQ CONFIG_ONLINE_OFFLINE_XTAL_FREQ_27M
#define CONFIG_ONLINE_XTAL_FREQ CONFIG_ONLINE_OFFLINE_XTAL_FREQ_27M
#define CONFIG_HS_PER_TIMEOUT_TH_TOLERANCE 30  // gating function will let den timeout to 3 lines in HDMI2.1 below 720p timing
//--------------------------------------------------
// display ratio
//--------------------------------------------------
#define _DISPLAY_RATIO_90          	(90)
#define _DISPLAY_RATIO_92          	(92)
#define _DISPLAY_RATIO_95          	(95)
#define _DISPLAY_RATIO_96          	(96)
#define _DISPLAY_RATIO_97          	(97)
#define _DISPLAY_RATIO_98          	(98)
#define _DISPLAY_RATIO_99          	(99)
#define _DISPLAY_RATIO_100          	(100)

#define _HDMI_VGIP_SMART_FIT	1

//--------------------------------------------------
// Defination of Mode Polarity
//--------------------------------------------------
#define _SYNC_HN_VN	_BIT0
#define _SYNC_HP_VN	_BIT1
#define _SYNC_HN_VP	_BIT2
#define _SYNC_HP_VP	_BIT3

//--------------------------------------------------
// Tolerance of IHFreq and IVFreq
//--------------------------------------------------
#define _HFREQ_TOLERANCE      10
#define _VFREQ_TOLERANCE      1000

//--------------------------------------------------
// Macro of Mode Stable Flag
//--------------------------------------------------

//clear on-line measure flags: clear over range, polarity change status
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
#define MODE_CLEAR_ONMS_FLAG()	IoReg_SetBits(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? ONMS_onms2_status_reg : ONMS_onms1_status_reg,  (_BIT4|_BIT3|_BIT1|_BIT0));
#define MODE_CLEAR_ONMS_FLAG_FOR_2DCVT3D()	/*IoReg_Write32(SYNC_PROCESSOR_SP_MS2STUS_VADDR , 0x3ff);*/
#else
#define MODE_CLEAR_ONMS_FLAG()	IoReg_SetBits(/*Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? SYNCPROCESSOR_SP_MS2STUS_VADDR : */ONMS_onms1_status_reg,  (_BIT14|_BIT15|_BIT8|_BIT9));
#define MODE_CLEAR_ONMS_FLAG_FOR_2DCVT3D()	//IoReg_Write32(SYNCPROCESSOR_SP_MS2STUS_VADDR , 0x3ff);
#endif

//extern unsigned short HDMIDiditalMeasureIVH;

//--------------------------------------------------
// table type for table registration
//--------------------------------------------------
typedef enum _MODE_IDENTIFY_TYPE{
	CUSTOMER_EXTRA_CONDITION = 0x00,
	JUDGE_CONFUSE_MODE,
	HDMI_JUDGE_CONFUSE_MODE,
	HDMI_JUDGE_UNSUPPORT_MODE,
	USER_DEF_CONFUSE_MODE,
	USER_DEF_MODETABLE_SIZE,
	USER_DEF_MODETABLE_ACCEPTIVE,
	USER_DEF_MODETABLE_ACCEPTIVE_SIZE,
	MODE_VGA_USER_FIFO,
} MODE_IDENTIFY_TYPE;

//--------------------------------------------------
// mode table structure
//--------------------------------------------------
typedef struct
{
	unsigned short ModeID;
	unsigned char PolarityFlag;			// Acceptable polarity flags (see _SYNC_HN_VN, _SYNC_HP_VN, _SYNC_HN_VP, _SYNC_HP_VP)
	unsigned short IHWidth;				// Input Horizontal Width
	unsigned short IVHeight;			// Input Vertical Height
	unsigned short IHFreq;				// Input Horizontal Frequency (Unit: 100Hz)
	unsigned int IVFreq_1000;			// Input Vertical Frequency in digit 3 (Unit: 0.001Hz)
	unsigned char IHFreqTolerance;		// Input Horizontal Frequency Tolerance (Unit: 100Hz)
	unsigned int IVFreqTolerance_1000;	// Input Vertical Frequency Tolerance in digit 3 (Unit: 0.001Hz)
	unsigned short IHTotal;				// Input Horizontal Total Length
	unsigned short IVTotal;				// Input Vertical Total Length
	unsigned short IHStartPos;			// Input Horizontal Start Position
	unsigned short IVStartPos;			// Input Vertical Start Position
#ifdef CONFIG_ASPECT_BY_ASPECT_VALUE
	unsigned char AspectRatioValueH;		// Aspect ratio value H
	unsigned char AspectRatioValueV;		// Aspect ratio value V
#endif
} ModeTableType;

//======= checkType =======//
#define H_HIGH_PULSE_WIDTH 	0x01
#define H_COUNT 				0x02

//======= chargeCondition =======//
#define OVER					0x01
#define UNDER					0x02
#define OVER_AND_EQUAL		0x04
#define UNDER_AND_EQUAL		0x08
#define BY_RANGE				0x10

typedef struct
{
	unsigned char modeIdx;
	unsigned char checkType;
	unsigned short checkValue;
	unsigned char checkValueMax;
	unsigned char chargeCondition;
	unsigned short thresholdValue;
}ModeSkipTimingType;

enum _MODE_RESULT
{
	_MODE_NOSIGNAL	= 0xFF,
	_MODE_NOSUPPORT	= 0xFE,
	_MODE_NOCABLE		= 0xFD,
	_MODE_SUCCESS		= 0xFC,
	_MODE_DETECT		= 0xFB,
	_MDOE_UNDKOWN	= 0xFA,
};

unsigned char drvif_mode_offlinemeasure(unsigned char source, unsigned char mode, StructDisplayInfo* p_dispinfo, ModeInformationType* p_timinginfo);
void drvif_mode_clear_offms_status(void);

void drvif_mode_enableonlinemeasure(unsigned char channel, unsigned char online_path);
void drvif_mode_disableonlinemeasure(unsigned char channel);
void drvif_mode_onlinemeasure_setting(unsigned char channel, int src, unsigned char wdgenable, unsigned char interruptenable);
unsigned char drvif_mode_check_onlinemeasure_status(unsigned char channel);

void save_online_enable_time(unsigned char display);
void wait_ONMS_ready(unsigned char display, unsigned int vfreq);
unsigned int get_hdmi_online_freq(unsigned char display, unsigned char online_path);

void drvif_mode_disable_dma_onlinemeasure(void);
void drvif_mode_enable_dma_onlinemeasure(void);
void drvif_mode_dma_onlinemeasure_setting(unsigned char wdgenable, unsigned char interruptenable);
unsigned char drvif_mode_check_dma_onlinemeasure_status(void);

void drvif_mode_disable_onms4(void);
void drvif_mode_enable_onms4(void);
void drvif_mode_onms4_setting(unsigned char wdgenable, unsigned char interruptenable);
unsigned char drvif_mode_check_onms4_status(void);

unsigned char drvif_mode_checkstableBeforStable(void);
void drvif_mode_disablevgip(unsigned char display);
void drvif_mode_resetmode(unsigned char display);
unsigned char hdmi_mode_identifymode(unsigned char mode_index);

void clear_online_status(unsigned char display);//Clear online status

unsigned char drvif_mode_customer_identify_regTable(MODE_IDENTIFY_TYPE index, void *ptr);
void set_hdmi_identify_mode(unsigned char mode);
unsigned char get_hdmi_idntify_mode(void);
unsigned int Hdmi_Search_Mode(unsigned int *mode_id);
unsigned int Hdmi_get_startpos(unsigned int tbl_index, unsigned int *hstart, unsigned int *vstart);

unsigned int Hdmi_get_startpos(unsigned int tbl_index, unsigned int *hstart, unsigned int *vstart);

unsigned char check_online_result(unsigned char display, unsigned char online_path);//return false error

unsigned char get_scaler_run_hfr_mode(void);
void set_scaler_run_hfr_mode(unsigned char mode);//save last time run 4k120, we need reset setting at reset mode
unsigned char get_scaler_run_4k60_vrr_two_path(void);
void set_scaler_run_4k60_vrr_two_path(unsigned char enable);//save last time run vrr 4k 60 m domain two path , we need reset m domain sub setting at reset  mode
void drvif_hdmi_dma_onlinemeasure_watchdog_early_enable_setting(void);//for online watchdog early enable to protection output timing
void drvif_hdmi_onlinemeasure_watchdog_early_enable_setting(void);//This function is to set onlinemeasure watchdog and interrupt setting
extern void (*drvif_mode_enable_hsync_period_timeout_wdg_isr)(unsigned char online_path);
void set_scaler_run_sub_nnsr_two_path(unsigned char enable);
unsigned char get_scaler_run_sub_nnsr_two_path(void);

#ifdef __cplusplus
}
#endif
/**
 *
 * @}
 */

#endif /* #define _MODE_H_ */

