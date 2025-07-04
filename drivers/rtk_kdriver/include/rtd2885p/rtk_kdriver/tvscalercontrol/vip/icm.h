 /*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2006
  * All rights reserved.
  * ========================================================================*/

/*================= File Description =======================================*/
/**
 * @file
 * 	This file is for color related functions.
 *
 * @author 	$Author$
 * @date 	$Date$
 * @version $Revision$
 */

/**
 * @addtogroup color
 * @{
 */

/*============================ Module dependency  ===========================*/
#ifndef _ICM_H
#define _ICM_H

/*#include "rtd_types.h"*/

#ifdef __cplusplus
extern "C" {
#endif


/*===================================  Types ================================*/


/*================================== Variables ==============================*/


/*================================ Definitions ==============================*/

typedef struct {
	int H; /* or H*/
	int S; /* or S'*/
	int I; /* or Y*/
} COLORELEM;

enum {
	_HUE_ELEM = 0,
	_SAT_ELEM,
	_ITN_ELEM,
};/* WhichElem;*/

#define HUERANGE        6144
#define SATRANGE        4096
#define ITNRANGE        4096
#define SATRANGEEX      8192
#define ITNRANGEEX      8192
#define YUVRANGE		4096

#define HUESEGMAX	60
#define SATSEGMAX	12
#define ITNSEGMAX	8
#define ITN_SUBTRACT_CONST	1752

#if 0	/* move to vipCommon.h*/
#define VIP_ICM_TBL_X 5
#define VIP_ICM_TBL_Y 434 /*12(itn) x 12(sat) x 3(h/s/i) + 2(global settings) = 434*/
#define VIP_ICM_TBL_Z 64
#endif
/*#define SLR_MAIN_DISPLAY 0*/

#define	ICM_Global_hue 0
#define ICM_Global_Sat 1
#define ICM_Global_Itn 2
#if 0	// move to vipcommon
#define ICM_PQMASK_HUE_GAIN_CURVE_LEN	(48)
#define ICM_PQMASK_SAT_GAIN_CURVE_LEN	(8)
#endif
typedef struct {
	COLORELEM elem[ITNSEGMAX][SATSEGMAX][HUESEGMAX];
} COLORELEM_TAB_T;

typedef struct {
	unsigned char axis;
	short offset;
	short h_sta;
	short h_end;
	short s_sta;
	short s_end;
	short i_sta;
	short i_end;
} COLORELEM_BLOCK_ADJ_PARAM;

typedef struct {
	short dHue[7];
	short dSat[7];
	short dItn[7];

	unsigned char h_fixed_pillar_sta[7];
	unsigned char h_fixed_pillar_end[7];
	unsigned char s_fixed_pillar_sta[7];
	unsigned char s_fixed_pillar_end[7];
	unsigned char i_fixed_pillar_sta[7];
	unsigned char i_fixed_pillar_end[7];
} COLORELEM_COLOR_MANAGEMENT_PARAM_TV006;

typedef struct {
	short dHue[3];

	unsigned char h_fixed_pillar_sta[3];
	unsigned char h_fixed_pillar_end[3];
	unsigned char s_fixed_pillar_sta[3];
	unsigned char s_fixed_pillar_end[3];
	unsigned char i_fixed_pillar_sta[3];
	unsigned char i_fixed_pillar_end[3];
} COLORELEM_PREFERRED_COLOR_PARAM_TV006;

typedef struct _ICM_DMA_DEBUG_INFO {
	unsigned int wait_front_porch_timeout;
	unsigned int clear_err_status_timeout;
	unsigned int check_phase_timeout;
	unsigned int break_at_porch_out;
	unsigned int break_at_phase_ok;
	unsigned int phase_ok_at_linecount;
	unsigned int final_porch_check;
	unsigned int reserved_7;
	unsigned int reserved_8;
	unsigned int total_cost_time;
} ICM_DMA_DEBUG_INFO;

typedef struct {
	unsigned char RGB_offset_en;
	int RGB_offset_curve[3][HUESEGMAX];
	unsigned char RGB_offset_S_gain[SATSEGMAX];
	unsigned char RGB_offset_I_gain[ITNSEGMAX];
} ICM_RGB_Offset_table;

#define ICM_DMA_DEBUG_INFO_SIZE 8
#define CLIP(min, max, val) ((val < min) ? min : (val > max ? max : val))

/*================================== Function ===============================*/

/*============================================================================*/
/**
 * @param <display> {display channel, main=0 / sub=1}
 * @return {none}
 */
/*============================================================================*/
void drvif_color_icm_set_bypass_onoff(unsigned char display, unsigned char bEable);
void drvif_color_icm_hue_histo_init(void);
void drvif_color_icm_readAIReg(void);
//void drvif_color_icm_table_select(unsigned char display, unsigned short *tICM_ini, unsigned char which_table);
void  drvif_color_icm_hue_histo_init(void);
//unsigned char drvif_color_icm_global_adjust(int g_dhue_cur, int *g_dsatbysat_cur, int *g_ditnbyitn_cur, unsigned char bSubtractPrevious);
unsigned char drvif_color_icm_Set_global(int g_dhue_cur, int *g_dsatbysat_cur, int *g_ditnbyitn_cur);
unsigned char drvif_color_set_icm_Enable(unsigned char enable_Flag);

unsigned char drvif_color_icm_Get_Global_Ctrl(unsigned char bFromVIPTable, int *g_dhue_cur, int *g_dsatbysat_cur, int *g_ditnbyitn_cur);
void drvif_color_Icm_Enable(unsigned char bEable);
unsigned char drvif_color_Icm_Read3DTable(COLORELEM ICM_TAB_ACCESS[ITNSEGMAX][SATSEGMAX][HUESEGMAX]);
int drvif_color_icm_FormatValue(int value, unsigned char type, unsigned char tab_mode, unsigned char IfWrite);

/*for TV035_1*/
void drvif_color_icm_user_function(unsigned char display, unsigned short *tICM_ini, unsigned char which_table, unsigned char skin_tone, unsigned char color_wheel, unsigned char detail_enhance, unsigned dynamic_tint);
void drvif_color_icm_SKIN_TONE(unsigned char display, unsigned short *tICM_ini, unsigned char which_table, unsigned short nValue);

unsigned char drvif_color_icm_Adjust_Block(unsigned char nAxis, int nOffset, int adjust_len, int h_sta, int h_end, unsigned char h_total, int s_sta, int s_end, unsigned char s_total, int i_sta, int i_end, unsigned char i_total);
unsigned char drvif_color_icm_SramBlockAccessSpeedup(unsigned int *buf, unsigned char if_write_ic, int h_sta, int h_end, int s_sta, int s_end, int i_sta, int i_end);
unsigned char drvif_color_icm_GetBufFromICMTab(unsigned int *buf, COLORELEM ICM_TAB_ACCESS[ITNSEGMAX][SATSEGMAX][HUESEGMAX],	int h_sta, int h_end, int s_sta, int s_end, int i_sta, int i_end);
unsigned char drvif_color_icm_SramBlockWriteSpeedup(unsigned int *buf, int h_sta, int h_end, int s_sta, int s_end, int i_sta, int i_end);
unsigned char drvif_color_icm_SramBlockReadSpeedup(unsigned int *buf, int h_sta, int h_end, int s_sta, int s_end, int i_sta, int i_end, unsigned char nAxis, int nOffset);
//unsigned char drvif_color_icm_SetBufToICMTab(unsigned int *buf, COLORELEM ICM_TAB_ACCESS[ITNSEGMAX][SATSEGMAX][HUESEGMAX], int h_sta, int h_end, int s_sta, int s_end, int i_sta, int i_end);
int drvif_color_icm_ReadHueValue(int hue, int ii, int is, int ih);

unsigned char drvif_color_icm_Adjust_Protection(unsigned int *buf, int nOffset, int adjust_len, int h_sta, int h_end, unsigned char h_total, int s_sta, int s_end, unsigned char s_total, int i_sta, int i_end, unsigned char i_total);
void drvif_color_set_Icm_RGB_mode(unsigned char value);

unsigned char drvif_color_icm_set_ctrl(unsigned short table_mode, unsigned short hsi_grid_sel, unsigned short out_grid_process, unsigned short y_ctrl);
unsigned char drvif_color_icm_set_uv_coring(unsigned short u_coring, unsigned short v_coring);
unsigned char drvif_color_icm_save_global_of_viptable(unsigned short dhue_cur, unsigned short *dsatbysat_cur, unsigned short *ditnbyitn_cur);
unsigned char drvif_color_icm_set_pillar(unsigned short *h_pillar, unsigned short *s_pillar, unsigned short *i_pillar,  unsigned char h_pillar_num, unsigned char s_pillar_num, unsigned char i_pillar_num);
unsigned char drvif_color_icm_Write3DTable(COLORELEM ICM_TAB_ACCESS[ITNSEGMAX][SATSEGMAX][HUESEGMAX]);
unsigned char drvif_color_icm_Write3DTable_By_DMA(COLORELEM ICM_TAB_ACCESS[ITNSEGMAX][SATSEGMAX][HUESEGMAX], unsigned int addr, unsigned int* vir_addr,  unsigned char h_pillar_num, unsigned char s_pillar_num, unsigned char i_pillar_num);

void drvif_color_Icm_Enable(unsigned char bEable);

void drvif_color_Icm_ctrl_YUV2RGB(unsigned char data_format_sel, unsigned char data_range_sel);

unsigned char drvif_color_icm_Set_RGB_Offset_Enable(unsigned char enable_Flag);
unsigned char drvif_color_icm_Set_RGB_Offset_Table(int rgb_offset_curve[3][HUESEGMAX], unsigned char rgb_offset_s_gain[SATSEGMAX], unsigned char rgb_offset_i_gain[ITNSEGMAX]);

void drvif_color_icm_AI_debugbox_fadeInOut(int debugbox_en,unsigned int debugbox_width,unsigned int debugbox_color,int debugbox_blendingRatio);

void drvif_color_icm_Set_pqmask_hue(unsigned int bEnable, unsigned int gain);
void drvif_color_icm_Set_pqmask_sat(unsigned int bEnable, unsigned int gain, unsigned int offset);
void drvif_color_icm_Set_pqmask_int(unsigned int bEnable, unsigned int gain, unsigned int offset);
void drvif_color_icm_Set_pqmask_debug_gain(unsigned char Gain);
void drvif_color_icm_pqmask_source_mux(unsigned char int_sel,
									   unsigned char obj1_s_sel,
									   unsigned char obj2_s_sel,
									   unsigned char obj1_h_sel,
									   unsigned char obj2_h_sel);

void drvif_color_icm_pqmask_enable(unsigned char pqmask_i_en, 
								   unsigned char pqmask_obj1_s_en,
								   unsigned char pqmask_obj2_s_en, 
								   unsigned char pqmask_obj1_h_en,
								   unsigned char pqmask_obj2_h_en);

void drvif_color_icm_pqmask_debug(unsigned char debug_en,
								  unsigned char debug_mode,
								  unsigned char debug_gain,
								  unsigned int sat_debug_high,
								  unsigned int sat_debug_low,
								  unsigned int int_debug_high,
								  unsigned int int_debug_low);

void drvif_color_icm_pqmask_inputAdjust(unsigned int in_i_gain,
										unsigned int in_i_offset,
										unsigned int in_s_gain,
										unsigned int in_s_offset);

void drvif_color_icm_pqmask_SatGainBySat(unsigned char obj1_sat_bysat_en,
										 unsigned char boj1_sat_bysat_valueRange_mode,
										 unsigned char obj2_sat_bysat_en,
										 unsigned char boj2_sat_bysat_valueRange_mode,										 
										 unsigned char *p_obj1_sat_bysat_GainCurve,
										 unsigned char *p_obj2_sat_bysat_GainCurve);

void drvif_color_icm_pqmask_Sat_Final_Ctrl(unsigned int obj1_GlbBldRatio,
										   unsigned int obj2_GlbBldRatio);

void drvif_color_icm_pqmask_SatHue_byhue_Ctrl(unsigned char obj1_hue_byhue_en,
										      unsigned char obj2_hue_byhue_en,
										      unsigned char *p_obj1_hue_byhue_curve,
										   	  unsigned char *p_obj2_hue_byhue_curve,
										   	  unsigned char obj1_sat_byhue_en,
											  unsigned char obj2_sat_byhue_en,
											  unsigned char *p_obj1_sat_GainByHue_Cureve,										 
											  unsigned char *p_obj2_sat_GainByHue_Cureve,
										      unsigned int obj1_target_hue,
										      unsigned int obj2_target_hue);


#ifdef __cplusplus
}
#endif

#endif

