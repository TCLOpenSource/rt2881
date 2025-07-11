/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2021
  * All rights reserved.
  * ========================================================================*/
  /*================= File Description =======================================*/
  /**
   * @file
   *  This file is for APLSatGain related functions.
   *
   * @author     Bill Chou
   * @date	  2023/02/03
   * @version	  1
   */

  /*============================ Module dependency	===========================*/

#include <linux/string.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <linux/kernel.h>
//#include <mach/io.h>
#include <linux/delay.h>

#include "tvscalercontrol/vip/APLSatGain.h"  
#include <tvscalercontrol/scaler/vipinclude.h> // for typedef that used in VIP_reg_def.h
#include <tvscalercontrol/vip/vip_reg_def.h>
#include <mach/rtk_log.h>
#include "rtk_vip_logger.h"
#define TAG_NAME "VPQ"

#undef VIPprintf
#define VIPprintf(fmt, args...)	VIPprintfPrivate(VIP_LOGLEVEL_DEBUG,VIP_LOGMODULE_VIP_LC_DEBUG,fmt,##args) 

#ifdef VIP_SUPPORT_APLSATGAIN
// Function Description : This function is for APLSatGain Enable
void drvif_color_set_APLSatGain_Enable(unsigned char enable)
{
	aplsatgain_aplsatgain_ctrl_0_RBUS aplsatgain_aplsatgain_ctrl_0_reg;
	aplsatgain_aplsatgain_double_buffer_ctrl_RBUS aplsatgain_aplsatgain_double_buffer_ctrl_reg;

	// double buffer enable
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg);
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.aplsatgain_db_en=1;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg, aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue);

	aplsatgain_aplsatgain_ctrl_0_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_CTRL_0_reg);
	aplsatgain_aplsatgain_ctrl_0_reg.aplsatgain_en = enable;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_CTRL_0_reg, aplsatgain_aplsatgain_ctrl_0_reg.regValue);

	//double buffer apply
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg);
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.aplsatgain_db_apply=1;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg, aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue);
}
/* APLSatGain factor set function */
void drvif_color_set_APLSatGain_APL_factor(DRV_APLSatGain_APL_factor *factor)
{
	aplsatgain_aplsatgain_ctrl_0_RBUS aplsatgain_aplsatgain_ctrl_0_reg;
	aplsatgain_aplsatgain_double_buffer_ctrl_RBUS aplsatgain_aplsatgain_double_buffer_ctrl_reg;

	// double buffer enable
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg);
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.aplsatgain_db_en=1;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg, aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue);

	aplsatgain_aplsatgain_ctrl_0_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_CTRL_0_reg);
	aplsatgain_aplsatgain_ctrl_0_reg.aplsatgain_apl_div_factor = factor->APL_div_factor;
	aplsatgain_aplsatgain_ctrl_0_reg.aplsatgain_apl_factor= factor->APL_factor;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_CTRL_0_reg, aplsatgain_aplsatgain_ctrl_0_reg.regValue);

	//double buffer apply
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg);
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.aplsatgain_db_apply=1;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg, aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue);
}

void drvif_color_set_APLSatGain_Ctrl(DRV_APLSatGain_ctrl *ctrl)
{
	aplsatgain_aplsatgain_ctrl_0_RBUS aplsatgain_aplsatgain_ctrl_0_reg;
	aplsatgain_aplsatgain_double_buffer_ctrl_RBUS aplsatgain_aplsatgain_double_buffer_ctrl_reg;

	// double buffer enable
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg);
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.aplsatgain_db_en=1;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg, aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue);

	aplsatgain_aplsatgain_ctrl_0_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_CTRL_0_reg);
	aplsatgain_aplsatgain_ctrl_0_reg.aplsatgain_input_add_en = ctrl->input_add_en;
	aplsatgain_aplsatgain_ctrl_0_reg.aplsatgain_video_source = ctrl->video_source;
	aplsatgain_aplsatgain_ctrl_0_reg.aplsatgain_sat_source = ctrl->Sat_source;
	aplsatgain_aplsatgain_ctrl_0_reg.aplsatgain_apl_source = ctrl->APL_source;
	aplsatgain_aplsatgain_ctrl_0_reg.aplsatgain_shift_input = ctrl->shift_input;
	aplsatgain_aplsatgain_ctrl_0_reg.aplsatgain_apl_mapping_en = ctrl->apl_mapping_en;
	
	IoReg_Write32(APLSATGAIN_APLSATGAIN_CTRL_0_reg, aplsatgain_aplsatgain_ctrl_0_reg.regValue);

	//double buffer apply
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg);
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.aplsatgain_db_apply=1;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg, aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue);	
}

void drvif_color_set_APLSatGain_DualDisplay_Ctrl(DRV_APLSatGain_dual_display_ctrl *ctrl)
{
	aplsatgain_aplsatgain_ctrl_0_RBUS aplsatgain_aplsatgain_ctrl_0_reg;
	aplsatgain_aplsatgain_double_buffer_ctrl_RBUS aplsatgain_aplsatgain_double_buffer_ctrl_reg;

	// double buffer enable
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg);
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.aplsatgain_db_en=1;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg, aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue);

	aplsatgain_aplsatgain_ctrl_0_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_CTRL_0_reg);
	aplsatgain_aplsatgain_ctrl_0_reg.aplsatgain_apl_panel_sel = ctrl->APL_panel_sel;
	aplsatgain_aplsatgain_ctrl_0_reg.aplsatgain_two_panel_use_same_apl_en = ctrl->two_panel_use_same_APL_en;
	
	IoReg_Write32(APLSATGAIN_APLSATGAIN_CTRL_0_reg, aplsatgain_aplsatgain_ctrl_0_reg.regValue);

	//double buffer apply
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg);
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.aplsatgain_db_apply=1;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg, aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue);	
}

void drvif_color_set_APLSatGain_Tbl_Sel(unsigned char tbl)
{
	aplsatgain_aplsatgain_newapl_0_RBUS aplsatgain_aplsatgain_newapl_0_reg;
	aplsatgain_aplsatgain_double_buffer_ctrl_RBUS aplsatgain_aplsatgain_double_buffer_ctrl_reg;

	// double buffer enable
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg);
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.aplsatgain_db_en=1;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg, aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue);

	aplsatgain_aplsatgain_newapl_0_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_NEWAPL_0_reg);
	aplsatgain_aplsatgain_newapl_0_reg.aplsatgain_hw_tbl = tbl;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_NEWAPL_0_reg, aplsatgain_aplsatgain_newapl_0_reg.regValue);

	//double buffer apply
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg);
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.aplsatgain_db_apply=1;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg, aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue);	
}

/* APLSatGain ctrl set function */
void drvif_color_set_APLSatGain_Ctrl_Common(DRV_APLSatGain_Ctrl_Common *ptr)
{
	aplsatgain_aplsatgain_ctrl_common_RBUS aplsatgain_aplsatgain_ctrl_common_reg;
	aplsatgain_aplsatgain_newapl_0_RBUS aplsatgain_aplsatgain_newapl_0_reg;
	aplsatgain_aplsatgain_double_buffer_ctrl_RBUS aplsatgain_aplsatgain_double_buffer_ctrl_reg;

	// double buffer enable
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg);
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.aplsatgain_db_en=1;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg, aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue);

	aplsatgain_aplsatgain_ctrl_common_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_CTRL_COMMON_reg);
	aplsatgain_aplsatgain_ctrl_common_reg.aplsatgain_shift_gain = ptr->shift_gain;
	aplsatgain_aplsatgain_ctrl_common_reg.aplsatgain_sat_mode = ptr->sat_mode;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_CTRL_COMMON_reg, aplsatgain_aplsatgain_ctrl_common_reg.regValue);

	aplsatgain_aplsatgain_newapl_0_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_NEWAPL_0_reg);
	aplsatgain_aplsatgain_newapl_0_reg.aplsatgain_apl_blend_weight = ptr->APL_blend_weight;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_NEWAPL_0_reg, aplsatgain_aplsatgain_newapl_0_reg.regValue);

	//double buffer apply
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg);
	aplsatgain_aplsatgain_double_buffer_ctrl_reg.aplsatgain_db_apply=1;
	IoReg_Write32(APLSATGAIN_APLSATGAIN_DOUBLE_BUFFER_CTRL_reg, aplsatgain_aplsatgain_double_buffer_ctrl_reg.regValue);	
}

/* APLSatGain Gain_by_APL set function */
void drvif_color_set_APLSatGain_Gain_by_APL(DRV_APLSatGain_gain_by_apl *curve, unsigned char table_sel)
{
	aplsatgain_aplsatgain_apl_gain_tbl0_0_RBUS aplsatgain_aplsatgain_apl_gain_tbl0_0_reg;
	aplsatgain_aplsatgain_apl_gain_tbl0_1_RBUS aplsatgain_aplsatgain_apl_gain_tbl0_1_reg;
	aplsatgain_aplsatgain_apl_gain_tbl0_2_RBUS aplsatgain_aplsatgain_apl_gain_tbl0_2_reg;
	aplsatgain_aplsatgain_apl_gain_tbl0_3_RBUS aplsatgain_aplsatgain_apl_gain_tbl0_3_reg;
	aplsatgain_aplsatgain_apl_gain_tbl0_4_RBUS aplsatgain_aplsatgain_apl_gain_tbl0_4_reg;
	
	aplsatgain_aplsatgain_apl_gain_tbl1_0_RBUS aplsatgain_aplsatgain_apl_gain_tbl1_0_reg;
	aplsatgain_aplsatgain_apl_gain_tbl1_1_RBUS aplsatgain_aplsatgain_apl_gain_tbl1_1_reg;
	aplsatgain_aplsatgain_apl_gain_tbl1_2_RBUS aplsatgain_aplsatgain_apl_gain_tbl1_2_reg;
	aplsatgain_aplsatgain_apl_gain_tbl1_3_RBUS aplsatgain_aplsatgain_apl_gain_tbl1_3_reg;
	aplsatgain_aplsatgain_apl_gain_tbl1_4_RBUS aplsatgain_aplsatgain_apl_gain_tbl1_4_reg;

	if(table_sel == 0)
	{
		aplsatgain_aplsatgain_apl_gain_tbl0_0_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl0_0_reg);
		aplsatgain_aplsatgain_apl_gain_tbl0_1_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl0_1_reg);
		aplsatgain_aplsatgain_apl_gain_tbl0_2_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl0_2_reg);
		aplsatgain_aplsatgain_apl_gain_tbl0_3_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl0_3_reg);
		aplsatgain_aplsatgain_apl_gain_tbl0_4_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl0_4_reg);
		
		aplsatgain_aplsatgain_apl_gain_tbl0_0_reg.aplsatgain_gain_by_apl_tbl0_0 = curve->gain_by_apl_curve_tbl0[0];
		aplsatgain_aplsatgain_apl_gain_tbl0_0_reg.aplsatgain_gain_by_apl_tbl0_1 = curve->gain_by_apl_curve_tbl0[1];
		aplsatgain_aplsatgain_apl_gain_tbl0_0_reg.aplsatgain_gain_by_apl_tbl0_2 = curve->gain_by_apl_curve_tbl0[2];
		aplsatgain_aplsatgain_apl_gain_tbl0_0_reg.aplsatgain_gain_by_apl_tbl0_3 = curve->gain_by_apl_curve_tbl0[3];
		aplsatgain_aplsatgain_apl_gain_tbl0_1_reg.aplsatgain_gain_by_apl_tbl0_4 = curve->gain_by_apl_curve_tbl0[4];
		aplsatgain_aplsatgain_apl_gain_tbl0_1_reg.aplsatgain_gain_by_apl_tbl0_5 = curve->gain_by_apl_curve_tbl0[5];
		aplsatgain_aplsatgain_apl_gain_tbl0_1_reg.aplsatgain_gain_by_apl_tbl0_6 = curve->gain_by_apl_curve_tbl0[6];
		aplsatgain_aplsatgain_apl_gain_tbl0_1_reg.aplsatgain_gain_by_apl_tbl0_7 = curve->gain_by_apl_curve_tbl0[7];
		aplsatgain_aplsatgain_apl_gain_tbl0_2_reg.aplsatgain_gain_by_apl_tbl0_8 = curve->gain_by_apl_curve_tbl0[8];
		aplsatgain_aplsatgain_apl_gain_tbl0_2_reg.aplsatgain_gain_by_apl_tbl0_9 = curve->gain_by_apl_curve_tbl0[9];
		aplsatgain_aplsatgain_apl_gain_tbl0_2_reg.aplsatgain_gain_by_apl_tbl0_10 = curve->gain_by_apl_curve_tbl0[10];
		aplsatgain_aplsatgain_apl_gain_tbl0_2_reg.aplsatgain_gain_by_apl_tbl0_11 = curve->gain_by_apl_curve_tbl0[11];
		aplsatgain_aplsatgain_apl_gain_tbl0_3_reg.aplsatgain_gain_by_apl_tbl0_12 = curve->gain_by_apl_curve_tbl0[12];
		aplsatgain_aplsatgain_apl_gain_tbl0_3_reg.aplsatgain_gain_by_apl_tbl0_13 = curve->gain_by_apl_curve_tbl0[13];
		aplsatgain_aplsatgain_apl_gain_tbl0_3_reg.aplsatgain_gain_by_apl_tbl0_14 = curve->gain_by_apl_curve_tbl0[14];
		aplsatgain_aplsatgain_apl_gain_tbl0_3_reg.aplsatgain_gain_by_apl_tbl0_15 = curve->gain_by_apl_curve_tbl0[15];
		aplsatgain_aplsatgain_apl_gain_tbl0_4_reg.aplsatgain_gain_by_apl_tbl0_16 = curve->gain_by_apl_curve_tbl0[16];
		
		IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl0_0_reg, aplsatgain_aplsatgain_apl_gain_tbl0_0_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl0_1_reg, aplsatgain_aplsatgain_apl_gain_tbl0_1_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl0_2_reg, aplsatgain_aplsatgain_apl_gain_tbl0_2_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl0_3_reg, aplsatgain_aplsatgain_apl_gain_tbl0_3_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl0_4_reg, aplsatgain_aplsatgain_apl_gain_tbl0_4_reg.regValue);
	}
	else
	{
		aplsatgain_aplsatgain_apl_gain_tbl1_0_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl1_0_reg);
		aplsatgain_aplsatgain_apl_gain_tbl1_1_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl1_1_reg);
		aplsatgain_aplsatgain_apl_gain_tbl1_2_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl1_2_reg);
		aplsatgain_aplsatgain_apl_gain_tbl1_3_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl1_3_reg);
		aplsatgain_aplsatgain_apl_gain_tbl1_4_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl1_4_reg);

		aplsatgain_aplsatgain_apl_gain_tbl1_0_reg.aplsatgain_gain_by_apl_tbl1_0 = curve->gain_by_apl_curve_tbl1[0];
		aplsatgain_aplsatgain_apl_gain_tbl1_0_reg.aplsatgain_gain_by_apl_tbl1_1 = curve->gain_by_apl_curve_tbl1[1];
		aplsatgain_aplsatgain_apl_gain_tbl1_0_reg.aplsatgain_gain_by_apl_tbl1_2 = curve->gain_by_apl_curve_tbl1[2];
		aplsatgain_aplsatgain_apl_gain_tbl1_0_reg.aplsatgain_gain_by_apl_tbl1_3 = curve->gain_by_apl_curve_tbl1[3];
		aplsatgain_aplsatgain_apl_gain_tbl1_1_reg.aplsatgain_gain_by_apl_tbl1_4 = curve->gain_by_apl_curve_tbl1[4];
		aplsatgain_aplsatgain_apl_gain_tbl1_1_reg.aplsatgain_gain_by_apl_tbl1_5 = curve->gain_by_apl_curve_tbl1[5];
		aplsatgain_aplsatgain_apl_gain_tbl1_1_reg.aplsatgain_gain_by_apl_tbl1_6 = curve->gain_by_apl_curve_tbl1[6];
		aplsatgain_aplsatgain_apl_gain_tbl1_1_reg.aplsatgain_gain_by_apl_tbl1_7 = curve->gain_by_apl_curve_tbl1[7];
		aplsatgain_aplsatgain_apl_gain_tbl1_2_reg.aplsatgain_gain_by_apl_tbl1_8 = curve->gain_by_apl_curve_tbl1[8];
		aplsatgain_aplsatgain_apl_gain_tbl1_2_reg.aplsatgain_gain_by_apl_tbl1_9 = curve->gain_by_apl_curve_tbl1[9];
		aplsatgain_aplsatgain_apl_gain_tbl1_2_reg.aplsatgain_gain_by_apl_tbl1_10 = curve->gain_by_apl_curve_tbl1[10];
		aplsatgain_aplsatgain_apl_gain_tbl1_2_reg.aplsatgain_gain_by_apl_tbl1_11 = curve->gain_by_apl_curve_tbl1[11];
		aplsatgain_aplsatgain_apl_gain_tbl1_3_reg.aplsatgain_gain_by_apl_tbl1_12 = curve->gain_by_apl_curve_tbl1[12];
		aplsatgain_aplsatgain_apl_gain_tbl1_3_reg.aplsatgain_gain_by_apl_tbl1_13 = curve->gain_by_apl_curve_tbl1[13];
		aplsatgain_aplsatgain_apl_gain_tbl1_3_reg.aplsatgain_gain_by_apl_tbl1_14 = curve->gain_by_apl_curve_tbl1[14];
		aplsatgain_aplsatgain_apl_gain_tbl1_3_reg.aplsatgain_gain_by_apl_tbl1_15 = curve->gain_by_apl_curve_tbl1[15];
		aplsatgain_aplsatgain_apl_gain_tbl1_4_reg.aplsatgain_gain_by_apl_tbl1_16 = curve->gain_by_apl_curve_tbl1[16];
		
		IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl1_0_reg, aplsatgain_aplsatgain_apl_gain_tbl1_0_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl1_1_reg, aplsatgain_aplsatgain_apl_gain_tbl1_1_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl1_2_reg, aplsatgain_aplsatgain_apl_gain_tbl1_2_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl1_3_reg, aplsatgain_aplsatgain_apl_gain_tbl1_3_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_GAIN_tbl1_4_reg, aplsatgain_aplsatgain_apl_gain_tbl1_4_reg.regValue);
	}
}

/* APLSatGain Gain_by_sat set function */
void drvif_color_set_APLSatGain_Gain_by_sat(DRV_APLSatGain_gain_by_sat *curve, unsigned char table_sel)
{
	aplsatgain_aplsatgain_sat_gain_tbl0_0_RBUS aplsatgain_aplsatgain_sat_gain_tbl0_0_reg;
	aplsatgain_aplsatgain_sat_gain_tbl0_1_RBUS aplsatgain_aplsatgain_sat_gain_tbl0_1_reg;
	aplsatgain_aplsatgain_sat_gain_tbl0_2_RBUS aplsatgain_aplsatgain_sat_gain_tbl0_2_reg;
	aplsatgain_aplsatgain_sat_gain_tbl0_3_RBUS aplsatgain_aplsatgain_sat_gain_tbl0_3_reg;
	aplsatgain_aplsatgain_sat_gain_tbl0_4_RBUS aplsatgain_aplsatgain_sat_gain_tbl0_4_reg;
	aplsatgain_aplsatgain_sat_gain_tbl0_5_RBUS aplsatgain_aplsatgain_sat_gain_tbl0_5_reg;
	aplsatgain_aplsatgain_sat_gain_tbl0_6_RBUS aplsatgain_aplsatgain_sat_gain_tbl0_6_reg;
	aplsatgain_aplsatgain_sat_gain_tbl0_7_RBUS aplsatgain_aplsatgain_sat_gain_tbl0_7_reg;
	aplsatgain_aplsatgain_sat_gain_tbl0_8_RBUS aplsatgain_aplsatgain_sat_gain_tbl0_8_reg;
	aplsatgain_aplsatgain_sat_gain_tbl1_0_RBUS aplsatgain_aplsatgain_sat_gain_tbl1_0_reg;
	aplsatgain_aplsatgain_sat_gain_tbl1_1_RBUS aplsatgain_aplsatgain_sat_gain_tbl1_1_reg;
	aplsatgain_aplsatgain_sat_gain_tbl1_2_RBUS aplsatgain_aplsatgain_sat_gain_tbl1_2_reg;
	aplsatgain_aplsatgain_sat_gain_tbl1_3_RBUS aplsatgain_aplsatgain_sat_gain_tbl1_3_reg;
	aplsatgain_aplsatgain_sat_gain_tbl1_4_RBUS aplsatgain_aplsatgain_sat_gain_tbl1_4_reg;
	aplsatgain_aplsatgain_sat_gain_tbl1_5_RBUS aplsatgain_aplsatgain_sat_gain_tbl1_5_reg;
	aplsatgain_aplsatgain_sat_gain_tbl1_6_RBUS aplsatgain_aplsatgain_sat_gain_tbl1_6_reg;
	aplsatgain_aplsatgain_sat_gain_tbl1_7_RBUS aplsatgain_aplsatgain_sat_gain_tbl1_7_reg;
	aplsatgain_aplsatgain_sat_gain_tbl1_8_RBUS aplsatgain_aplsatgain_sat_gain_tbl1_8_reg;
	

	if(table_sel == 0)
	{
		aplsatgain_aplsatgain_sat_gain_tbl0_0_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_0_reg);
		aplsatgain_aplsatgain_sat_gain_tbl0_1_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_1_reg);
		aplsatgain_aplsatgain_sat_gain_tbl0_2_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_2_reg);
		aplsatgain_aplsatgain_sat_gain_tbl0_3_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_3_reg);
		aplsatgain_aplsatgain_sat_gain_tbl0_4_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_4_reg);
		aplsatgain_aplsatgain_sat_gain_tbl0_5_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_5_reg);
		aplsatgain_aplsatgain_sat_gain_tbl0_6_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_6_reg);
		aplsatgain_aplsatgain_sat_gain_tbl0_7_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_7_reg);
		aplsatgain_aplsatgain_sat_gain_tbl0_8_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_8_reg);
		

		aplsatgain_aplsatgain_sat_gain_tbl0_0_reg.aplsatgain_gain_by_sat_tbl0_0 = curve->gain_by_sat_curve_tbl0[0];
		aplsatgain_aplsatgain_sat_gain_tbl0_0_reg.aplsatgain_gain_by_sat_tbl0_1 = curve->gain_by_sat_curve_tbl0[1];
		aplsatgain_aplsatgain_sat_gain_tbl0_1_reg.aplsatgain_gain_by_sat_tbl0_2 = curve->gain_by_sat_curve_tbl0[2];
		aplsatgain_aplsatgain_sat_gain_tbl0_1_reg.aplsatgain_gain_by_sat_tbl0_3 = curve->gain_by_sat_curve_tbl0[3];
		aplsatgain_aplsatgain_sat_gain_tbl0_2_reg.aplsatgain_gain_by_sat_tbl0_4 = curve->gain_by_sat_curve_tbl0[4];
		aplsatgain_aplsatgain_sat_gain_tbl0_2_reg.aplsatgain_gain_by_sat_tbl0_5 = curve->gain_by_sat_curve_tbl0[5];
		aplsatgain_aplsatgain_sat_gain_tbl0_3_reg.aplsatgain_gain_by_sat_tbl0_6 = curve->gain_by_sat_curve_tbl0[6];
		aplsatgain_aplsatgain_sat_gain_tbl0_3_reg.aplsatgain_gain_by_sat_tbl0_7 = curve->gain_by_sat_curve_tbl0[7];
		aplsatgain_aplsatgain_sat_gain_tbl0_4_reg.aplsatgain_gain_by_sat_tbl0_8 = curve->gain_by_sat_curve_tbl0[8];
		aplsatgain_aplsatgain_sat_gain_tbl0_4_reg.aplsatgain_gain_by_sat_tbl0_9 = curve->gain_by_sat_curve_tbl0[9];
		aplsatgain_aplsatgain_sat_gain_tbl0_5_reg.aplsatgain_gain_by_sat_tbl0_10 = curve->gain_by_sat_curve_tbl0[10];
		aplsatgain_aplsatgain_sat_gain_tbl0_5_reg.aplsatgain_gain_by_sat_tbl0_11 = curve->gain_by_sat_curve_tbl0[11];
		aplsatgain_aplsatgain_sat_gain_tbl0_6_reg.aplsatgain_gain_by_sat_tbl0_12 = curve->gain_by_sat_curve_tbl0[12];
		aplsatgain_aplsatgain_sat_gain_tbl0_6_reg.aplsatgain_gain_by_sat_tbl0_13 = curve->gain_by_sat_curve_tbl0[13];
		aplsatgain_aplsatgain_sat_gain_tbl0_7_reg.aplsatgain_gain_by_sat_tbl0_14 = curve->gain_by_sat_curve_tbl0[14];
		aplsatgain_aplsatgain_sat_gain_tbl0_7_reg.aplsatgain_gain_by_sat_tbl0_15 = curve->gain_by_sat_curve_tbl0[15];
		aplsatgain_aplsatgain_sat_gain_tbl0_8_reg.aplsatgain_gain_by_sat_tbl0_16 = curve->gain_by_sat_curve_tbl0[16];
		
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_0_reg, aplsatgain_aplsatgain_sat_gain_tbl0_0_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_1_reg, aplsatgain_aplsatgain_sat_gain_tbl0_1_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_2_reg, aplsatgain_aplsatgain_sat_gain_tbl0_2_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_3_reg, aplsatgain_aplsatgain_sat_gain_tbl0_3_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_4_reg, aplsatgain_aplsatgain_sat_gain_tbl0_4_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_5_reg, aplsatgain_aplsatgain_sat_gain_tbl0_5_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_6_reg, aplsatgain_aplsatgain_sat_gain_tbl0_6_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_7_reg, aplsatgain_aplsatgain_sat_gain_tbl0_7_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl0_8_reg, aplsatgain_aplsatgain_sat_gain_tbl0_8_reg.regValue);
	}
	else
	{
		aplsatgain_aplsatgain_sat_gain_tbl1_0_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_0_reg);
		aplsatgain_aplsatgain_sat_gain_tbl1_1_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_1_reg);
		aplsatgain_aplsatgain_sat_gain_tbl1_2_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_2_reg);
		aplsatgain_aplsatgain_sat_gain_tbl1_3_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_3_reg);
		aplsatgain_aplsatgain_sat_gain_tbl1_4_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_4_reg);
		aplsatgain_aplsatgain_sat_gain_tbl1_5_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_5_reg);
		aplsatgain_aplsatgain_sat_gain_tbl1_6_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_6_reg);
		aplsatgain_aplsatgain_sat_gain_tbl1_7_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_7_reg);
		aplsatgain_aplsatgain_sat_gain_tbl1_8_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_8_reg);

		aplsatgain_aplsatgain_sat_gain_tbl1_0_reg.aplsatgain_gain_by_sat_tbl1_0 = curve->gain_by_sat_curve_tbl1[0];
		aplsatgain_aplsatgain_sat_gain_tbl1_0_reg.aplsatgain_gain_by_sat_tbl1_1 = curve->gain_by_sat_curve_tbl1[1];
		aplsatgain_aplsatgain_sat_gain_tbl1_1_reg.aplsatgain_gain_by_sat_tbl1_2 = curve->gain_by_sat_curve_tbl1[2];
		aplsatgain_aplsatgain_sat_gain_tbl1_1_reg.aplsatgain_gain_by_sat_tbl1_3 = curve->gain_by_sat_curve_tbl1[3];
		aplsatgain_aplsatgain_sat_gain_tbl1_2_reg.aplsatgain_gain_by_sat_tbl1_4 = curve->gain_by_sat_curve_tbl1[4];
		aplsatgain_aplsatgain_sat_gain_tbl1_2_reg.aplsatgain_gain_by_sat_tbl1_5 = curve->gain_by_sat_curve_tbl1[5];
		aplsatgain_aplsatgain_sat_gain_tbl1_3_reg.aplsatgain_gain_by_sat_tbl1_6 = curve->gain_by_sat_curve_tbl1[6];
		aplsatgain_aplsatgain_sat_gain_tbl1_3_reg.aplsatgain_gain_by_sat_tbl1_7 = curve->gain_by_sat_curve_tbl1[7];
		aplsatgain_aplsatgain_sat_gain_tbl1_4_reg.aplsatgain_gain_by_sat_tbl1_8 = curve->gain_by_sat_curve_tbl1[8];
		aplsatgain_aplsatgain_sat_gain_tbl1_4_reg.aplsatgain_gain_by_sat_tbl1_9 = curve->gain_by_sat_curve_tbl1[9];
		aplsatgain_aplsatgain_sat_gain_tbl1_5_reg.aplsatgain_gain_by_sat_tbl1_10 = curve->gain_by_sat_curve_tbl1[10];
		aplsatgain_aplsatgain_sat_gain_tbl1_5_reg.aplsatgain_gain_by_sat_tbl1_11 = curve->gain_by_sat_curve_tbl1[11];
		aplsatgain_aplsatgain_sat_gain_tbl1_6_reg.aplsatgain_gain_by_sat_tbl1_12 = curve->gain_by_sat_curve_tbl1[12];
		aplsatgain_aplsatgain_sat_gain_tbl1_6_reg.aplsatgain_gain_by_sat_tbl1_13 = curve->gain_by_sat_curve_tbl1[13];
		aplsatgain_aplsatgain_sat_gain_tbl1_7_reg.aplsatgain_gain_by_sat_tbl1_14 = curve->gain_by_sat_curve_tbl1[14];
		aplsatgain_aplsatgain_sat_gain_tbl1_7_reg.aplsatgain_gain_by_sat_tbl1_15 = curve->gain_by_sat_curve_tbl1[15];
		aplsatgain_aplsatgain_sat_gain_tbl1_8_reg.aplsatgain_gain_by_sat_tbl1_16 = curve->gain_by_sat_curve_tbl1[16];
		
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_0_reg, aplsatgain_aplsatgain_sat_gain_tbl1_0_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_1_reg, aplsatgain_aplsatgain_sat_gain_tbl1_1_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_2_reg, aplsatgain_aplsatgain_sat_gain_tbl1_2_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_3_reg, aplsatgain_aplsatgain_sat_gain_tbl1_3_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_4_reg, aplsatgain_aplsatgain_sat_gain_tbl1_4_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_5_reg, aplsatgain_aplsatgain_sat_gain_tbl1_5_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_6_reg, aplsatgain_aplsatgain_sat_gain_tbl1_6_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_7_reg, aplsatgain_aplsatgain_sat_gain_tbl1_7_reg.regValue);
		IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_GAIN_tbl1_8_reg, aplsatgain_aplsatgain_sat_gain_tbl1_8_reg.regValue);
	}
}

void drvif_color_set_APLSatGain_APL_Index(DRV_APLSatGain_APL_index *ptr)
{
	aplsatgain_aplsatgain_apl_index_0_RBUS aplsatgain_aplsatgain_apl_index_0_reg;
	aplsatgain_aplsatgain_apl_index_1_RBUS aplsatgain_aplsatgain_apl_index_1_reg;
	aplsatgain_aplsatgain_apl_index_2_RBUS aplsatgain_aplsatgain_apl_index_2_reg;
	aplsatgain_aplsatgain_apl_index_3_RBUS aplsatgain_aplsatgain_apl_index_3_reg;
	aplsatgain_aplsatgain_apl_index_4_RBUS aplsatgain_aplsatgain_apl_index_4_reg;
	aplsatgain_aplsatgain_apl_index_5_RBUS aplsatgain_aplsatgain_apl_index_5_reg;
	aplsatgain_aplsatgain_apl_index_6_RBUS aplsatgain_aplsatgain_apl_index_6_reg;
	aplsatgain_aplsatgain_apl_index_7_RBUS aplsatgain_aplsatgain_apl_index_7_reg;
	aplsatgain_aplsatgain_apl_index_8_RBUS aplsatgain_aplsatgain_apl_index_8_reg;

	aplsatgain_aplsatgain_apl_index_0_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_INDEX_0_reg);
	aplsatgain_aplsatgain_apl_index_1_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_INDEX_1_reg);
	aplsatgain_aplsatgain_apl_index_2_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_INDEX_2_reg);
	aplsatgain_aplsatgain_apl_index_3_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_INDEX_3_reg);
	aplsatgain_aplsatgain_apl_index_4_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_INDEX_4_reg);
	aplsatgain_aplsatgain_apl_index_5_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_INDEX_5_reg);
	aplsatgain_aplsatgain_apl_index_6_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_INDEX_6_reg);
	aplsatgain_aplsatgain_apl_index_7_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_INDEX_7_reg);
	aplsatgain_aplsatgain_apl_index_8_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_INDEX_8_reg);
	
	aplsatgain_aplsatgain_apl_index_0_reg.aplsatgain_apl_index0 = ptr->APL_index[0];
	aplsatgain_aplsatgain_apl_index_0_reg.aplsatgain_apl_index1 = ptr->APL_index[1];
	aplsatgain_aplsatgain_apl_index_1_reg.aplsatgain_apl_index2 = ptr->APL_index[2];
	aplsatgain_aplsatgain_apl_index_1_reg.aplsatgain_apl_index3 = ptr->APL_index[3];
	aplsatgain_aplsatgain_apl_index_2_reg.aplsatgain_apl_index4 = ptr->APL_index[4];
	aplsatgain_aplsatgain_apl_index_2_reg.aplsatgain_apl_index5 = ptr->APL_index[5];
	aplsatgain_aplsatgain_apl_index_3_reg.aplsatgain_apl_index6 = ptr->APL_index[6];
	aplsatgain_aplsatgain_apl_index_3_reg.aplsatgain_apl_index7 = ptr->APL_index[7];
	aplsatgain_aplsatgain_apl_index_4_reg.aplsatgain_apl_index8 = ptr->APL_index[8];
	aplsatgain_aplsatgain_apl_index_4_reg.aplsatgain_apl_index9 = ptr->APL_index[9];
	aplsatgain_aplsatgain_apl_index_5_reg.aplsatgain_apl_index10 = ptr->APL_index[10];
	aplsatgain_aplsatgain_apl_index_5_reg.aplsatgain_apl_index11 = ptr->APL_index[11];
	aplsatgain_aplsatgain_apl_index_6_reg.aplsatgain_apl_index12 = ptr->APL_index[12];
	aplsatgain_aplsatgain_apl_index_6_reg.aplsatgain_apl_index13 = ptr->APL_index[13];
	aplsatgain_aplsatgain_apl_index_7_reg.aplsatgain_apl_index14 = ptr->APL_index[14];
	aplsatgain_aplsatgain_apl_index_7_reg.aplsatgain_apl_index15 = ptr->APL_index[15];
	aplsatgain_aplsatgain_apl_index_8_reg.aplsatgain_apl_index16 = ptr->APL_index[16];
	
	IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_INDEX_0_reg, aplsatgain_aplsatgain_apl_index_0_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_INDEX_1_reg, aplsatgain_aplsatgain_apl_index_1_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_INDEX_2_reg, aplsatgain_aplsatgain_apl_index_2_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_INDEX_3_reg, aplsatgain_aplsatgain_apl_index_3_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_INDEX_4_reg, aplsatgain_aplsatgain_apl_index_4_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_INDEX_5_reg, aplsatgain_aplsatgain_apl_index_5_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_INDEX_6_reg, aplsatgain_aplsatgain_apl_index_6_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_INDEX_7_reg, aplsatgain_aplsatgain_apl_index_7_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_APL_INDEX_8_reg, aplsatgain_aplsatgain_apl_index_8_reg.regValue);
}

void drvif_color_set_APLSatGain_SAT_Index(DRV_APLSatGain_SAT_index *ptr)
{
	aplsatgain_aplsatgain_sat_index_0_RBUS aplsatgain_aplsatgain_sat_index_0_reg;
	aplsatgain_aplsatgain_sat_index_1_RBUS aplsatgain_aplsatgain_sat_index_1_reg;
	aplsatgain_aplsatgain_sat_index_2_RBUS aplsatgain_aplsatgain_sat_index_2_reg;
	aplsatgain_aplsatgain_sat_index_3_RBUS aplsatgain_aplsatgain_sat_index_3_reg;
	aplsatgain_aplsatgain_sat_index_4_RBUS aplsatgain_aplsatgain_sat_index_4_reg;
	aplsatgain_aplsatgain_sat_index_5_RBUS aplsatgain_aplsatgain_sat_index_5_reg;
	aplsatgain_aplsatgain_sat_index_6_RBUS aplsatgain_aplsatgain_sat_index_6_reg;
	aplsatgain_aplsatgain_sat_index_7_RBUS aplsatgain_aplsatgain_sat_index_7_reg;
	aplsatgain_aplsatgain_sat_index_8_RBUS aplsatgain_aplsatgain_sat_index_8_reg;

	aplsatgain_aplsatgain_sat_index_0_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_INDEX_0_reg);
	aplsatgain_aplsatgain_sat_index_1_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_INDEX_1_reg);
	aplsatgain_aplsatgain_sat_index_2_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_INDEX_2_reg);
	aplsatgain_aplsatgain_sat_index_3_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_INDEX_3_reg);
	aplsatgain_aplsatgain_sat_index_4_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_INDEX_4_reg);
	aplsatgain_aplsatgain_sat_index_5_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_INDEX_5_reg);
	aplsatgain_aplsatgain_sat_index_6_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_INDEX_6_reg);
	aplsatgain_aplsatgain_sat_index_7_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_INDEX_7_reg);
	aplsatgain_aplsatgain_sat_index_8_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_SAT_INDEX_8_reg);
	
	aplsatgain_aplsatgain_sat_index_0_reg.aplsatgain_sat_index0 = ptr->SAT_index[0];
	aplsatgain_aplsatgain_sat_index_0_reg.aplsatgain_sat_index1 = ptr->SAT_index[1];
	aplsatgain_aplsatgain_sat_index_1_reg.aplsatgain_sat_index2 = ptr->SAT_index[2];
	aplsatgain_aplsatgain_sat_index_1_reg.aplsatgain_sat_index3 = ptr->SAT_index[3];
	aplsatgain_aplsatgain_sat_index_2_reg.aplsatgain_sat_index4 = ptr->SAT_index[4];
	aplsatgain_aplsatgain_sat_index_2_reg.aplsatgain_sat_index5 = ptr->SAT_index[5];
	aplsatgain_aplsatgain_sat_index_3_reg.aplsatgain_sat_index6 = ptr->SAT_index[6];
	aplsatgain_aplsatgain_sat_index_3_reg.aplsatgain_sat_index7 = ptr->SAT_index[7];
	aplsatgain_aplsatgain_sat_index_4_reg.aplsatgain_sat_index8 = ptr->SAT_index[8];
	aplsatgain_aplsatgain_sat_index_4_reg.aplsatgain_sat_index9 = ptr->SAT_index[9];
	aplsatgain_aplsatgain_sat_index_5_reg.aplsatgain_sat_index10 = ptr->SAT_index[10];
	aplsatgain_aplsatgain_sat_index_5_reg.aplsatgain_sat_index11 = ptr->SAT_index[11];
	aplsatgain_aplsatgain_sat_index_6_reg.aplsatgain_sat_index12 = ptr->SAT_index[12];
	aplsatgain_aplsatgain_sat_index_6_reg.aplsatgain_sat_index13 = ptr->SAT_index[13];
	aplsatgain_aplsatgain_sat_index_7_reg.aplsatgain_sat_index14 = ptr->SAT_index[14];
	aplsatgain_aplsatgain_sat_index_7_reg.aplsatgain_sat_index15 = ptr->SAT_index[15];
	aplsatgain_aplsatgain_sat_index_8_reg.aplsatgain_sat_index16 = ptr->SAT_index[16];
	
	IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_INDEX_0_reg, aplsatgain_aplsatgain_sat_index_0_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_INDEX_1_reg, aplsatgain_aplsatgain_sat_index_1_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_INDEX_2_reg, aplsatgain_aplsatgain_sat_index_2_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_INDEX_3_reg, aplsatgain_aplsatgain_sat_index_3_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_INDEX_4_reg, aplsatgain_aplsatgain_sat_index_4_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_INDEX_5_reg, aplsatgain_aplsatgain_sat_index_5_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_INDEX_6_reg, aplsatgain_aplsatgain_sat_index_6_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_INDEX_7_reg, aplsatgain_aplsatgain_sat_index_7_reg.regValue);
	IoReg_Write32(APLSATGAIN_APLSATGAIN_SAT_INDEX_8_reg, aplsatgain_aplsatgain_sat_index_8_reg.regValue);
}

/* APLSatGain APL get function */
unsigned short drvif_color_get_APLSatGain_RGBAPL(void)
{
	unsigned short apl = 0;

	aplsatgain_aplsatgain_apl_RBUS aplsatgain_aplsatgain_apl_reg;
	aplsatgain_aplsatgain_apl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_reg);

	apl = aplsatgain_aplsatgain_apl_reg.aplsatgain_rgb_apl;

	return apl;
}

unsigned short drvif_color_get_APLSatGain_YAPL(void)
{
	unsigned short apl = 0;

	aplsatgain_aplsatgain_newapl_2_RBUS aplsatgain_aplsatgain_newapl_2_reg;
	aplsatgain_aplsatgain_newapl_2_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_NEWAPL_2_reg);

	apl = aplsatgain_aplsatgain_newapl_2_reg.aplsatgain_y_apl;

	return apl;
}


unsigned short drvif_color_get_APLSatGain_BlendAPL(void)
{
	unsigned short apl = 0;

	aplsatgain_aplsatgain_newapl_1_RBUS aplsatgain_aplsatgain_newapl_1_reg;
	aplsatgain_aplsatgain_newapl_1_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_NEWAPL_1_reg);

	apl = aplsatgain_aplsatgain_newapl_1_reg.aplsatgain_blend_apl;

	return apl;
}

unsigned short drvif_color_get_APLSatGain_FrameSat(void)
{
	unsigned short sat = 0;

	aplsatgain_aplsatgain_newapl_1_RBUS aplsatgain_aplsatgain_newapl_1_reg;
	aplsatgain_aplsatgain_newapl_1_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_NEWAPL_1_reg);

	sat = aplsatgain_aplsatgain_newapl_1_reg.aplsatgain_saturation;

	return sat;
}

unsigned short drvif_color_get_APLSatGain_BlankingGrey(void)
{
	unsigned short val = 0;

	aplsatgain_aplsatgain_blankinggrey_RBUS aplsatgain_aplsatgain_blankinggrey_reg;
	aplsatgain_aplsatgain_blankinggrey_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_BLANKINGGREY_reg);

	val = aplsatgain_aplsatgain_blankinggrey_reg.aplsatgain_blanking_grey;

	return val;
}
// get function for panel_2
unsigned short drvif_color_get_APLSatGain_RGBAPL2(void)
{
	unsigned short apl = 0;
	
	aplsatgain_aplsatgain_apl_RBUS aplsatgain_aplsatgain_apl_reg;
	aplsatgain_aplsatgain_apl_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_APL_reg);

	apl = aplsatgain_aplsatgain_apl_reg.aplsatgain_rgb_apl_panel2;

	return apl;
}

unsigned short drvif_color_get_APLSatGain_YAPL2(void)
{
	unsigned short apl = 0;
	
	aplsatgain_aplsatgain_newapl_2_RBUS aplsatgain_aplsatgain_newapl_2_reg;
	aplsatgain_aplsatgain_newapl_2_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_NEWAPL_2_reg);

	apl = aplsatgain_aplsatgain_newapl_2_reg.aplsatgain_y_apl_panel2;

	return apl;
}


unsigned short drvif_color_get_APLSatGain_BlendAPL2(void)
{
	unsigned short apl = 0;
	
	aplsatgain_aplsatgain_newapl_3_RBUS aplsatgain_aplsatgain_newapl_3_reg;
	aplsatgain_aplsatgain_newapl_3_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_NEWAPL_3_reg);

	apl = aplsatgain_aplsatgain_newapl_3_reg.aplsatgain_blend_apl_panel2;

	return apl;
}

unsigned short drvif_color_get_APLSatGain_FrameSat2(void)
{
	unsigned short sat = 0;
	
	aplsatgain_aplsatgain_newapl_3_RBUS aplsatgain_aplsatgain_newapl_3_reg;
	aplsatgain_aplsatgain_newapl_3_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_NEWAPL_3_reg);

	sat = aplsatgain_aplsatgain_newapl_3_reg.aplsatgain_saturation_panel2;

	return sat;
}

unsigned short drvif_color_get_APLSatGain_BlankingGrey2(void)
{
	unsigned short val = 0;
	
	aplsatgain_aplsatgain_blankinggrey_RBUS aplsatgain_aplsatgain_blankinggrey_reg;
	aplsatgain_aplsatgain_blankinggrey_reg.regValue = IoReg_Read32(APLSATGAIN_APLSATGAIN_BLANKINGGREY_reg);

	val = aplsatgain_aplsatgain_blankinggrey_reg.aplsatgain_blanking_grey_panel2;

	return val;
}

#endif
