/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2009
  * All rights reserved.
  * ========================================================================*/

/**
 * @file
 * 	This file is for gibi and od related functions.
 *
 * @author 	$Author$
 * @date 	$Date$
 * @version $Revision$
 */

/**
 * @addtogroup color
 * @{
 */

#ifndef _VALC_H
#define _VALC_H
/*============================ Module dependency  ===========================*/


/*================================ Definitions ==============================*/
#define VALC_TBL_LEN 1025
#define VALC_TBL_NORMAL_LEN_MULTICURVE 513
#define VALC_TBL_STRONG_LEN_MULTICURVE 257
#define VALC_TBL_WEAK_LEN_MULTICURVE 257
#define VALC_TBL_STRONG_START 513
#define VALC_TBL_WEAK_START 513+257

/*================================== Variables ==============================*/
extern unsigned int valcTbl[3][2][VALC_TBL_LEN+3];
extern unsigned char valcSmoothWeight[4];
/*===================================  Types ================================*/
enum {
	VALC_PATTERN_H1V1 = 0,
	VALC_PATTERN_H1V2,
	VALC_PATTERN_H2V1,
	VALC_PATTERN_H2V2,
	VALC_PATTERN_MAX
};

typedef enum {
	_VALC_COLOR_R = 0,
	_VALC_COLOR_G,
	_VALC_COLOR_B,
	_VALC_COLOR_R_MULTICURVE, // added for M8P
	_VALC_COLOR_G_MULTICURVE, // added for M8P
	_VALC_COLOR_B_MULTICURVE, // added for M8P
	_VALC_COLOR_ERR
} DRV_valc_channel_t;

// added for M8P
typedef enum {
	_VALC_TABLE_NORMAL,
	_VALC_TABLE_NORMAL_MULTICURVE, 
	_VALC_TABLE_STRONG_MULTICURVE, 
	_VALC_TABLE_WEAK_MULTICURVE,
	_VALC_TABLE_ERR
} DRV_valc_table_t;

typedef struct {
	unsigned char Cyclic_H;
	unsigned char Cyclic_V;
	unsigned char Pattern[8][16];
} DRV_valc_pattern_ctrl_t;

typedef struct {
	unsigned char Weight[16];
	unsigned char ClipSel;
} DRV_valc_hpf_ctrl_t;

typedef struct {
	unsigned char Weight[16];
	// unsigned char seg_len[4]; // only used in M8P flow
} DRV_valc_sat_ctrl_t;

typedef struct {
	DRV_valc_pattern_ctrl_t Pattern;
	DRV_valc_sat_ctrl_t SATCtrl;
	DRV_valc_hpf_ctrl_t HPFCtrl;
} DRV_valc_ctrl_t;

// NOTE by Ring.Chen @20241111
// Since M8 & M8P one bin, and Top level bin file differ by flow
// So we need to separate the additional parameters for M8P
// This should be merged if dependency is omitted.
// ------------------------------------------------------------------------
typedef struct {
	unsigned char seg_len[4]; // only used in M8P flow
} DRV_valc_sat_ctrl_seg_len_M8P_t;
// ------------------------------------------------------------------------

typedef struct {
	unsigned int valcWindowStartY;
	unsigned int valcWindowStartX;
	unsigned int valcWindowHeight;
	unsigned int valcWindowWidth;
} DRV_valc_window_mode_ctrl_t;

typedef struct{
	unsigned char enable;
	DRV_valc_window_mode_ctrl_t Boundary;
	unsigned char smooth_enable;
}DRV_valc_win_ctrl_t;

typedef struct {
	unsigned char valcSceneEnable;
	unsigned int  valcSceneStartY;
	unsigned int  valcSceneStartX;
	unsigned int  valcSceneHeight;
	unsigned int  valcSceneWidth;
	unsigned char valcSceneSmoothEnable;
	unsigned char valcSceneSmoothDirection;
} DRV_valc_multi_scene_n_ctrl_t;

typedef struct {
	unsigned char valcMultiSceneOutEnable;
	unsigned char valcMultiSceneOverlapEnable;
	unsigned char valcMultiSceneFuncEnable;
	DRV_valc_multi_scene_n_ctrl_t valcMultiSceneN[4];
} DRV_valc_multi_scene_ctrl_t;

typedef struct {
	unsigned char valcMultiCurveEnable;
	unsigned char valcMultiCurveSatEnable;
	unsigned char valcMultiCurve_ttp_len;
	unsigned int  valcMultiCurve_ttp;
	
	unsigned char valcMultiCurve_b_low_L;
	unsigned char valcMultiCurve_b_low_R;
	unsigned char valcMultiCurve_b_high_L;
	unsigned char valcMultiCurve_b_high_R;
	unsigned char valcMultiCurve_s_low_L;
	unsigned char valcMultiCurve_s_low_R;
	unsigned char valcMultiCurve_s_high_L;
	unsigned char valcMultiCurve_s_high_R;
	unsigned char valcMultiCurve_bm;
	unsigned char valcMultiCurve_cm;
	unsigned char valcMultiCurve_sm;
} DRV_valc_multi_curve_ctrl_t;

/*=================================== Functions ================================*/
void drvif_color_set_valc_enable(unsigned char bEnable);
void drvif_color_set_valc_SpatialPattern(DRV_valc_pattern_ctrl_t* pPATCtrl);
void drvif_color_set_valc_HPF_protect(DRV_valc_hpf_ctrl_t* pHPFCtrl);
void drvif_color_set_valc_SAT_protect(DRV_valc_sat_ctrl_t* pSATCtrl);

// NOTE by Ring.Chen @20241111
// Since M8 & M8P one bin, and Top level bin file differ by flow
// So we need to separate the additional parameters for M8P
// This should be merged if dependency is omitted.
// ------------------------------------------------------------------------
void drvif_color_set_valc_SAT_protect_seg_len_M8P(DRV_valc_sat_ctrl_seg_len_M8P_t* pSATCtrl);
// ------------------------------------------------------------------------

void drvif_color_set_Func_valc_enable(unsigned char bEnable, unsigned char db_en);
char drvif_color_set_DTG_valc_enable(unsigned char bEnable);

void drvif_color_set_valc_table(unsigned int* pTbl, DRV_valc_channel_t ChannelSel);
void drvif_color_get_valc_table(unsigned int* pTbl, DRV_valc_channel_t ChannelSel);
void drvif_color_get_specific_valc_table(unsigned int* pTbl, DRV_valc_channel_t ChannelSel, DRV_valc_table_t TableSel); // added for M8P

unsigned char driver_color_get_valc_enable(void);
unsigned char driver_color_get_DTG_valc_enable(void);
void drvif_color_set_valc_location(unsigned char loctioan);

void drvif_color_set_valc_window_mode_enable(unsigned char bEnable);
void drvif_color_set_valc_window_mode_smooth_enable(unsigned char bEnable);
void drvif_color_set_valc_window_mode_setting(DRV_valc_win_ctrl_t* VALC_Win_Ctrl);

void drvif_color_set_valc_multi_scene_global_enable(unsigned char multi_en, unsigned char overlap_en, unsigned char window_out_en);
void drvif_color_set_valc_multi_scene_inner_enable(unsigned char bEnable, unsigned char scene_sel);
void drvif_color_set_valc_multi_scene_inner_setting(DRV_valc_multi_scene_n_ctrl_t* VALC_Muti_Scene_Ctrl, unsigned char scene_sel);
void drvif_color_set_valc_multi_scene_all_inner_setting(DRV_valc_multi_scene_ctrl_t* VALC_Muti_Scene_Ctrl);

void drvif_color_set_valc_curve_mode_enable(unsigned char multi_curve_en, unsigned char multi_curve_sat_en);
void drvif_color_set_valc_curve_mode_setting(DRV_valc_multi_curve_ctrl_t* VALC_Multi_Curve_Ctrl);
#endif

