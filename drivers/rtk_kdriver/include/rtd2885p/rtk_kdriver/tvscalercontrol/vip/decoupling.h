/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2024
  * All rights reserved.
  * ========================================================================*/

/**
 * @file
 * 	This file is for Decoupling related functions.
 *
 * @author 	$Author$
 * @date 	$Date$
 * @version $Revision$
 */

/**
 * @addtogroup color
 * @{
 */

#ifndef _DECOUPLING_H
#define _DECOUPLING_H
/*============================ Module dependency  ===========================*/

/*================================ Definitions ==============================*/

/*================================== Variables ==============================*/

/*===================================  Types ================================*/
typedef struct {
	unsigned char DCP_en;
	unsigned char DCP_smooth_en;
	unsigned char DCP_filter_en;
	unsigned char DCP_select_subpixel;
	unsigned char DCP_num_count;
	unsigned char DP_repeat_mode;
} DRV_decoupling_ctrl_t;

typedef struct {
	unsigned int DCP_gray_th1;
	unsigned int DCP_gray_shift;;
} DRV_decoupling_gray_th_t;

typedef struct {
	unsigned int DCP_diff_gray;
	DRV_decoupling_gray_th_t DCP_gray_Low;
	DRV_decoupling_gray_th_t DCP_gray_High;
} DRV_decoupling_gray_th_setting_t;

typedef struct {
	unsigned int DCP_comp_p1;
	unsigned int DCP_comp_p2;;
} DRV_decoupling_comp_t;

typedef struct {
	DRV_decoupling_comp_t DCP_comp_R;
	DRV_decoupling_comp_t DCP_comp_G;
	DRV_decoupling_comp_t DCP_comp_B;
} DRV_decoupling_comp_setting_t;

typedef struct {
	char DCP_R_offset;
	char DCP_L_offset;
} DRV_decoupling_offset_ctrl_t;

typedef struct {
	unsigned char enable;
	unsigned char H_filter_coef_sum;
	unsigned char H_filter_coef0;
	unsigned char H_filter_coef1;
	unsigned char H_filter_coef2;
	unsigned char H_filter_coef3;
	unsigned char H_filter_th;
} DRV_decoupling_H_filter_ctrl_t;

/*=================================== Functions ================================*/
void drvif_color_set_dcp_ctrl_setting(DRV_decoupling_ctrl_t* dcp_ctrl);
void drvif_color_get_dcp_ctrl_setting(DRV_decoupling_ctrl_t* ptr);
void drvif_color_set_dcp_gray_setting(DRV_decoupling_gray_th_setting_t* dcp_gray_setting);
void drvif_color_get_dcp_gray_setting(DRV_decoupling_gray_th_setting_t* ptr);
void drvif_color_set_dcp_comp_setting(DRV_decoupling_comp_setting_t* dcp_comp_setting);
void drvif_color_get_dcp_comp_setting(DRV_decoupling_comp_setting_t* ptr);
void drvif_color_set_dcp_offset_setting(DRV_decoupling_offset_ctrl_t* dcp_offset);
void drvif_color_get_dcp_offset_setting(DRV_decoupling_offset_ctrl_t* ptr);
void drvif_color_set_dcp_H_filter_ctrl_setting(DRV_decoupling_H_filter_ctrl_t* dcp_H_filter_ctrl);
void drvif_color_get_dcp_H_filter_ctrl_setting(DRV_decoupling_H_filter_ctrl_t* ptr);
void drvif_color_set_dcp_H_filter_en(unsigned char enable);
void drvif_color_set_dcp_en(unsigned char enable);
unsigned char driver_color_get_dcp_enable(void);


#endif



