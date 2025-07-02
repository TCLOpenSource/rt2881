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

#ifndef _FRAMESYNC_H
#define _FRAMESYNC_H

//#include "rtd_types.h"

//--------------------------------------------------
// Adjust DV total state machine.
//--------------------------------------------------
// [Code Sync][LewisLee][0990823][1]
typedef enum {
	FLL_DYMANIC_ADJUST_DHTOTAL_NONE = 0x00,
	FLL_DYMANIC_ADJUST_DHTOTAL_TO_MAX,
	FLL_DYMANIC_ADJUST_DHTOTAL_TO_MIN,
} FLL_ADJUST_DVTOTAL_STATE;

typedef enum {
	FRAMESYNC_DYMANIC_ADJUST_DHTOTAL_NONE = 0x00,
	FRAMESYNC_DYMANIC_ADJUST_DHTOTAL_TO_MAX,
	FRAMESYNC_DYMANIC_ADJUST_DHTOTAL_TO_MIN,
} FRAMESYNC_DYMANIC_ADJUST_DVTOTAL_STATE;

//--------------------------------------------------
// Offset_Step is the DCLK frequency step for searching frame-sync.
//--------------------------------------------------
#define _OFFSET_STEP    24
#define _SEARCH_TIME    24

#define XTAL_FREQ 27000000
#define IVS_DVS_THRESHOLD 0x100
#define IVS_XTAL_CNT  1
#define IVS_XTAL_CNT2 2
#define DVS_XTAL_CNT  3

//--------------------------------------------------
// CRT VODMA PLL
//--------------------------------------------------
#define XTAL_TICK       27000000UL
#define VODMA_PLL_MCODE_BASIC 4

//--------------------------------------------------
// Global Variables
//--------------------------------------------------


//--------------------------------------------------
// Function Prototypes
//--------------------------------------------------
void drvif_framesync_reset_dv_total_flg(unsigned char flg);
unsigned char drvif_framesync_detect(void);
unsigned char drvif_framesync_do(void);
void framesync_set_enterlastline_at_frc_mode_flg(unsigned char flg);
unsigned char framesync_get_enterlastline_at_frc_mode_flg(void);
//unsigned char drvif_frc_fixlastline_frc_measure(void);
unsigned char drvif_framesync_offlinemeasureendevent(void); //forster modified 071224
void drvif_framesync_gatting(unsigned char enable);
unsigned char drvif_decide_VO_SD_video_frameSync_enable(void);
unsigned char drvif_framesync_gatting_do(void);
#ifdef CONFIG_FS_HS_BIAS
unsigned char drvif_framesync_hs_bias(void);
void drvif_framesync_hs_bias_release(void);
#endif //#ifdef CONFIG_FS_HS_BIAS

void drvif_framesync_5_4(void);
void drvif_framesync_5_4_release(void);

void framesync_syncDoubleDVSReset(void);
void framesync_syncDoubleDVS_Frc_Tracking_Framesync(unsigned char display, unsigned char ucEnable);
void framesync_DoubleDvs_Wait_FS_Lock(unsigned char display);
unsigned char framesync_sync_fixlastline_mode(void);
void framesync_set_enterlastline_at_frc_mode_flg(unsigned char flg);
unsigned char framesync_get_enterlastline_at_frc_mode_flg(void);
void framesync_lastlinefinetune_at_frc_mode_dymanic_adjust_dhtotal(FLL_ADJUST_DVTOTAL_STATE state);
unsigned char framesync_get_lastlinefinetune_at_frc_mode_fixDclk_Flag(void);
void drvif_framesync_2Dcvt3D_reset_dv_total(void);
unsigned int hardware_measure_dvtotal(void);
unsigned int framesync_get_dvtotal_min(void);
unsigned int framesync_dynamic_adjust_dvtotal_den_relation(unsigned int CurDVtotal);
void framesync_mode_dymanic_adjust_dhtotal(FRAMESYNC_DYMANIC_ADJUST_DVTOTAL_STATE state);
unsigned char framesync_new_fixlastline_set_locked_threashold(unsigned short threshold_val);
unsigned char framesync_new_fixlastline_check_locked_status(void);
unsigned char framesync_new_fixlastline_threashold_check_enable(unsigned char bEnable);
unsigned char framesync_lastlinefinetune_at_new_mode(void);
void framesync_set_vtotal_by_vodma_freerun_period(void);
void Scaler_Set_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Enable_Flag(unsigned char ucEnable);
void Scaler_Set_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Upper(unsigned int ucValue);
void Scaler_Set_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Lower(unsigned int ucValue);
void Scaler_Set_FixLastLine_DVTotoal_Limit_ByVfreq_Enable_Flag(UINT8 ucEnable);
void Scaler_Set_FixLastLine_DVTotal_Limit_60Hz_Typical(UINT32 ucValue);
void Scaler_Set_FixLastLine_DVTotal_Limit_60Hz_Upper(UINT32 ucValue);
void Scaler_Set_FixLastLine_DVTotal_Limit_60Hz_Lower(UINT32 ucValue);
void Scaler_Set_FixLastLine_DVTotal_Limit_50Hz_Typical(UINT32 ucValue);
void Scaler_Set_FixLastLine_DVTotal_Limit_50Hz_Upper(UINT32 ucValue);
void Scaler_Set_FixLastLine_DVTotal_Limit_50Hz_Lower(UINT32 ucValue);
UINT8 Scaler_Get_FixLastLine_DVTotoal_Limit_ByVfreq_Enable_Flag(void);
void framesync_wait_vdc_stable(void);
UINT32 framesync_new_fixlastline_get_stable_xtal_cnt(UINT8 type);
void framesync_fixlastline_set_precision_vtotal(void);
unsigned char drv_framesync_check_iv2dv_phaseErr_on_fixlastline(void);
unsigned int drv_framesync_on_fixlastline_phaseErr_verify(void);
unsigned int drv_framesync_get_gamemode_iv2dv_linedelay(void);
void scaler_update_vo_vstat_htotal(unsigned char a_bEnableVCrop, unsigned char a_bUpdateReg);
void drvif_UHD_HDMI_increase_clk(void);
void framesync_clear_ibuffer_err_status(void);
unsigned int get_outputFramerate(unsigned int inputFrameRate, unsigned int removeRatio, unsigned int multipleRatio);
void framesync_set_iv2dv_delay(DTG_APP_TYPE app_type, unsigned int dispTimingMode);
unsigned int vbe_disp_vtotal_panel_boundary_protection(unsigned int DVtotal);
unsigned int decide_vbe_disp_vtotal(unsigned int vbe_dvtotal_status, unsigned int vbe_input_framerate_source, unsigned int check_update, unsigned int vfreq_for_vtotal, unsigned char display);
unsigned char data_fsync_go_expand_uzudtg_htotal(void);
unsigned int decide_expand_uzudtg_htotal(void);
unsigned int decide_uzudtg_vtotal_by_expand_uzudtg_htotal(unsigned int vbe_input_framerate_source, unsigned char display, unsigned int uzudtg_htotal);
void decide_uzudtg_v_den_by_expand_uzudtg_htotal(unsigned int uzudtg_vtotal, unsigned int *den_v_start, unsigned int *den_v_end);
unsigned int drvif_framesync_get_vo_clk(void);
unsigned int framesync_get_iv2dv_ih_slf_width(DTG_APP_TYPE app_type, VSC_INPUT_TYPE_T inputType);
unsigned int drv_memory_tran_iv2dv_for_vdec_lowdelay(DTG_APP_TYPE app_type, unsigned int ori_iv2dv);
#endif /* _FRAMESYNC_H */
//----------------------------------------------------------------------------------------------------

/**
 *
 * @}
 */

