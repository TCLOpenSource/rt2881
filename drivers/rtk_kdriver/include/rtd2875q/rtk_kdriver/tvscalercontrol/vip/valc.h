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
#define VALC_TBL_LEN 4096

/*================================== Variables ==============================*/
extern unsigned int valcTbl[3][2][VALC_TBL_LEN];

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
	_VALC_COLOR_ERR
} DRV_valc_channel_t;

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
} DRV_valc_sat_ctrl_t;

typedef struct {
	DRV_valc_pattern_ctrl_t Pattern;
	DRV_valc_sat_ctrl_t SATCtrl;
	DRV_valc_hpf_ctrl_t HPFCtrl;
} DRV_valc_ctrl_t;

typedef struct{
	unsigned int Start_x;
	unsigned int Start_y;
	unsigned int Width;
	unsigned int Height;
}DRV_valc_demo_ctrl_t;

typedef struct{
	unsigned char enable;
	DRV_valc_demo_ctrl_t Boundary;
}DRV_valc_win_ctrl_t;

typedef struct {
	unsigned char Multi_func_en;
	unsigned char Overlap_en;
	unsigned char Window_out_en;
}DRV_valc_multi_scene_en_ctrl_t;

typedef struct{
	unsigned char enable;
	DRV_valc_demo_ctrl_t Boundary;
}DRV_valc_scene_1_ctrl_t;

typedef struct{
	unsigned char enable;
	DRV_valc_demo_ctrl_t Boundary;
}DRV_valc_scene_2_ctrl_t;

typedef struct {
	DRV_valc_win_ctrl_t Window;
	DRV_valc_multi_scene_en_ctrl_t Scene_en;
	DRV_valc_scene_1_ctrl_t Scene_1;
	DRV_valc_scene_2_ctrl_t Scene_2;
} DRV_valc_window_ctrl_t;

/*=================================== Functions ================================*/
void drvif_color_set_valc_enable(unsigned char bEnable);
void drvif_color_set_valc_SpatialPattern(DRV_valc_pattern_ctrl_t* pPATCtrl);
void drvif_color_set_valc_HPF_protect(DRV_valc_hpf_ctrl_t* pHPFCtrl);
void drvif_color_set_valc_SAT_protect(DRV_valc_sat_ctrl_t* pSATCtrl);

void drvif_color_set_Func_valc_enable(unsigned char bEnable, unsigned char db_en);
char drvif_color_set_DTG_valc_enable(unsigned char bEnable);

void drvif_color_set_valc_table(unsigned int* pTbl, DRV_valc_channel_t ChannelSel);
void drvif_color_get_valc_table(unsigned int* pTbl, DRV_valc_channel_t ChannelSel);
unsigned char driver_color_get_valc_enable(void);
unsigned char driver_color_get_DTG_valc_enable(void);
void drvif_color_set_valc_location(unsigned char loctioan);

void drvif_color_set_valc_window_mode_enable(unsigned char bEnable);
void drvif_color_set_valc_window_mode_setting(unsigned int startx, unsigned int starty, unsigned int width, unsigned int height);
void drvif_color_set_valc_multi_scene_global_enable(unsigned char multi_en, unsigned char overlap_en, unsigned char window_out_en);
void drvif_color_set_valc_multi_scene_1_enable(unsigned char bEnable);
void drvif_color_set_valc_multi_scene_1_setting(unsigned int startx, unsigned int starty, unsigned int width, unsigned int height);
void drvif_color_set_valc_multi_scene_2_enable(unsigned char bEnable);
void drvif_color_set_valc_multi_scene_2_setting(unsigned int startx, unsigned int starty, unsigned int width, unsigned int height);
#endif

