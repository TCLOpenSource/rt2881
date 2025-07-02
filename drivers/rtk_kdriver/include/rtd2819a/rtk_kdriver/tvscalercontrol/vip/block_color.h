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

#ifndef _BLOCKCOLOR_VERIFY_H
#define _BLOCKCOLOR_VERIFY_H
/*============================ Module dependency  ===========================*/

/*================================ Definitions ==============================*/

/*================================== Variables ==============================*/

/*===================================  Types ================================*/
typedef struct {
	unsigned char bc_en;
	unsigned char bc_color_mode;
	unsigned char bc_blk_hnum;
	unsigned char bc_blk_vnum;
	unsigned short bc_blk_hsize;
	unsigned short bc_blk_vsize;
	unsigned short bc_startx;
	unsigned short bc_starty;
	unsigned char bc_db_en;
	unsigned char bc_db_apply;
	unsigned char bc_db_read_level;
} DRV_BC_Global_Ctrl;

typedef struct {
	unsigned char bc_color_r_coeff;
	unsigned char bc_color_g_coeff;
	unsigned char bc_color_b_coeff;
} DRV_BC_COLOR_RGB2Y_COEF_Ctrl;


/*=================================== Functions ================================*/
void drvif_color_set_bc_global_control_setting(DRV_BC_Global_Ctrl* pBcSetting);
void drvif_color_set_bc_rgb2y_coef_setting(DRV_BC_COLOR_RGB2Y_COEF_Ctrl* pBcY2RSetting);
void drvif_color_get_bc_data(unsigned int* pRetTbl_ch0, unsigned int* pRetTbl_ch1, unsigned int* pRetTbl_ch2);
unsigned char drvif_color_get_BC_Enable(void);
void drvif_color_get_bc_global_control_setting(DRV_BC_Global_Ctrl *ptr);
void fwif_color_set_bc_global_control_setting(DRV_BC_Global_Ctrl* pBcSetting);

#endif