/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2021
  * All rights reserved.
  * ========================================================================*/

/*================= File Description =======================================*/
/**
 * @file
 * 	This file is for APLSatGain related functions.
 *
 * @author 		Bill Chou
 * @date 		2023/02/03
 * @version 	1
 */
#define VIP_SUPPORT_APLSATGAIN

#ifdef VIP_SUPPORT_APLSATGAIN
#ifndef _APLSATGAIN_H
#define _APLSATGAIN_H

#define APLSATGAIN_Table_NUM 1

typedef struct {
	unsigned short APL_div_factor;
	unsigned short APL_factor;
} DRV_APLSatGain_APL_factor;

typedef struct {
	unsigned char shift_gain;
	unsigned char sat_mode;
	unsigned char APL_blend_weight;
}DRV_APLSatGain_Ctrl_Common;

typedef struct {
	unsigned short gain_by_apl_curve_tbl0[17];
	unsigned short gain_by_apl_curve_tbl1[17];
} DRV_APLSatGain_gain_by_apl;

typedef struct {
	unsigned short gain_by_sat_curve_tbl0[17];
	unsigned short gain_by_sat_curve_tbl1[17];
} DRV_APLSatGain_gain_by_sat;

typedef struct {
	unsigned char shift_input;
	unsigned char input_add_en;
	unsigned char video_source;
	unsigned char Sat_source;
	unsigned char APL_source;
	unsigned char apl_mapping_en;
} DRV_APLSatGain_ctrl;

typedef struct {
	unsigned char APL_panel_sel;
	unsigned char two_panel_use_same_APL_en;
} DRV_APLSatGain_dual_display_ctrl;

typedef struct {
	unsigned short APL_index[17];
} DRV_APLSatGain_APL_index;

typedef struct {
	unsigned short SAT_index[17];
} DRV_APLSatGain_SAT_index;

typedef struct {
	unsigned char enable;
	DRV_APLSatGain_Ctrl_Common ctrl;
	DRV_APLSatGain_ctrl ctrl2;
	DRV_APLSatGain_gain_by_apl gain_by_apl;
	DRV_APLSatGain_gain_by_sat gain_by_sat;
} DRV_APLSatgain_Table;

void drvif_color_set_APLSatGain_Enable(unsigned char enable);
void drvif_color_set_APLSatGain_APL_factor(DRV_APLSatGain_APL_factor *factor);
void drvif_color_set_APLSatGain_Ctrl_Common(DRV_APLSatGain_Ctrl_Common *ptr);
void drvif_color_set_APLSatGain_Gain_by_APL(DRV_APLSatGain_gain_by_apl *curve, unsigned char table_sel);
void drvif_color_set_APLSatGain_Gain_by_sat(DRV_APLSatGain_gain_by_sat *curve, unsigned char table_sel);
void drvif_color_set_APLSatGain_Ctrl(DRV_APLSatGain_ctrl *ctrl);
void drvif_color_set_APLSatGain_DualDisplay_Ctrl(DRV_APLSatGain_dual_display_ctrl *ctrl);
void drvif_color_set_APLSatGain_Tbl_Sel(unsigned char tbl);
void drvif_color_set_APLSatGain_APL_Index(DRV_APLSatGain_APL_index *ptr);
void drvif_color_set_APLSatGain_SAT_Index(DRV_APLSatGain_SAT_index *ptr);

unsigned short drvif_color_get_APLSatGain_RGBAPL(void);
unsigned short drvif_color_get_APLSatGain_YAPL(void);
unsigned short drvif_color_get_APLSatGain_BlendAPL(void);
unsigned short drvif_color_get_APLSatGain_FrameSat(void);
unsigned short drvif_color_get_APLSatGain_BlankingGrey(void);
unsigned short drvif_color_get_APLSatGain_RGBAPL2(void);
unsigned short drvif_color_get_APLSatGain_YAPL2(void);
unsigned short drvif_color_get_APLSatGain_BlendAPL2(void);
unsigned short drvif_color_get_APLSatGain_FrameSat2(void);
unsigned short drvif_color_get_APLSatGain_BlankingGrey2(void);


#endif
#endif

