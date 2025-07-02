/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2024
  * All rights reserved.
  * ========================================================================*/

/*================= File Description =======================================*/
/**
 * @file
 * 	This file is for decoupling related functions.
 *
 * @author 	$Author$
 * @date 	$Date$
 * @version 	$Revision$
 */

/**
 * @addtogroup color
 * @{
 */

/*============================ Module dependency  ===========================*/


#include "rtk_vip_logger.h"
#include <rtd_log/rtd_module_log.h>
#include <linux/semaphore.h>

#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/vip/decoupling.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/vip/vip_reg_def.h>

#include <rbus/M8P_decoupling_reg.h>
#include <rbus/sys_reg_reg.h>



/*================================ Global Variables ==============================*/


/*================================ Definitions ==============================*/
#undef VIPprintf
#define VIPprintf(fmt, args...)	VIPprintfPrivate(VIP_LOGLEVEL_DEBUG,VIP_LOGMODULE_VIP_PCID_DEBUG,fmt,##args)

/*================================== Function ===============================*/
void drvif_color_set_dcp_ctrl_setting(DRV_decoupling_ctrl_t* dcp_ctrl)
{
	M8P_decoupling_decoupling_control_RBUS decoupling_decoupling_control_reg;

	decoupling_decoupling_control_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_CONTROL_reg);

	decoupling_decoupling_control_reg.dcp_en = dcp_ctrl->DCP_en;
	decoupling_decoupling_control_reg.dcp_num_count = dcp_ctrl->DCP_num_count;
	decoupling_decoupling_control_reg.dcp_smooth_en = dcp_ctrl->DCP_smooth_en;
	decoupling_decoupling_control_reg.dcp_select_subpixel = dcp_ctrl->DCP_select_subpixel;
	decoupling_decoupling_control_reg.filter_en = dcp_ctrl->DCP_filter_en;
	decoupling_decoupling_control_reg.dp_repeat_mode = dcp_ctrl->DP_repeat_mode;

	IoReg_Write32(M8P_DECOUPLING_DECOUPLING_CONTROL_reg, decoupling_decoupling_control_reg.regValue);

}

void drvif_color_get_dcp_ctrl_setting(DRV_decoupling_ctrl_t* ptr)
{
	M8P_decoupling_decoupling_control_RBUS decoupling_decoupling_control_reg;

	decoupling_decoupling_control_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_CONTROL_reg);

	ptr->DCP_en = decoupling_decoupling_control_reg.dcp_en;
	ptr->DCP_num_count = decoupling_decoupling_control_reg.dcp_num_count;
	ptr->DCP_smooth_en = decoupling_decoupling_control_reg.dcp_smooth_en;
	ptr->DCP_select_subpixel = decoupling_decoupling_control_reg.dcp_select_subpixel;
	ptr->DCP_filter_en = decoupling_decoupling_control_reg.filter_en;
	ptr->DP_repeat_mode = decoupling_decoupling_control_reg.dp_repeat_mode;

}

void drvif_color_set_dcp_gray_setting(DRV_decoupling_gray_th_setting_t* dcp_gray_setting)
{
	M8P_decoupling_decoupling_diff_gray_RBUS decoupling_decoupling_diff_gray_reg;
	M8P_decoupling_decoupling_low_gray_th_RBUS decoupling_decoupling_low_gray_th_reg;
	M8P_decoupling_decoupling_high_gray_th_RBUS decoupling_decoupling_high_gray_th_reg;

	decoupling_decoupling_diff_gray_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_DIFF_GRAY_reg);
	decoupling_decoupling_low_gray_th_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_LOW_GRAY_TH_reg);
	decoupling_decoupling_high_gray_th_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_HIGH_GRAY_TH_reg);

	decoupling_decoupling_diff_gray_reg.dcp_diff_gray = dcp_gray_setting->DCP_diff_gray;
	decoupling_decoupling_low_gray_th_reg.dcp_low_gray_th1 = dcp_gray_setting->DCP_gray_Low.DCP_gray_th1;
	decoupling_decoupling_low_gray_th_reg.dcp_low_gray_shift = dcp_gray_setting->DCP_gray_Low.DCP_gray_shift;
	decoupling_decoupling_high_gray_th_reg.dcp_high_gray_th1 = dcp_gray_setting->DCP_gray_High.DCP_gray_th1;
	decoupling_decoupling_high_gray_th_reg.dcp_high_gray_shift = dcp_gray_setting->DCP_gray_High.DCP_gray_shift;

	IoReg_Write32(M8P_DECOUPLING_DECOUPLING_DIFF_GRAY_reg,    decoupling_decoupling_diff_gray_reg.regValue);
	IoReg_Write32(M8P_DECOUPLING_DECOUPLING_LOW_GRAY_TH_reg,  decoupling_decoupling_low_gray_th_reg.regValue);
	IoReg_Write32(M8P_DECOUPLING_DECOUPLING_HIGH_GRAY_TH_reg, decoupling_decoupling_high_gray_th_reg.regValue);

}

void drvif_color_get_dcp_gray_setting(DRV_decoupling_gray_th_setting_t* ptr)
{
	M8P_decoupling_decoupling_diff_gray_RBUS decoupling_decoupling_diff_gray_reg;
	M8P_decoupling_decoupling_low_gray_th_RBUS decoupling_decoupling_low_gray_th_reg;
	M8P_decoupling_decoupling_high_gray_th_RBUS decoupling_decoupling_high_gray_th_reg;

	decoupling_decoupling_diff_gray_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_DIFF_GRAY_reg);
	decoupling_decoupling_low_gray_th_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_LOW_GRAY_TH_reg);
	decoupling_decoupling_high_gray_th_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_HIGH_GRAY_TH_reg);

	ptr->DCP_diff_gray = decoupling_decoupling_diff_gray_reg.dcp_diff_gray;
	ptr->DCP_gray_Low.DCP_gray_th1 = decoupling_decoupling_low_gray_th_reg.dcp_low_gray_th1;
	ptr->DCP_gray_Low.DCP_gray_shift = decoupling_decoupling_low_gray_th_reg.dcp_low_gray_shift;
	ptr->DCP_gray_High.DCP_gray_th1 = decoupling_decoupling_high_gray_th_reg.dcp_high_gray_th1;
	ptr->DCP_gray_High.DCP_gray_shift = decoupling_decoupling_high_gray_th_reg.dcp_high_gray_shift;

}

void drvif_color_set_dcp_comp_setting(DRV_decoupling_comp_setting_t* dcp_comp_setting)
{
	M8P_decoupling_decoupling_r_gray_comp_RBUS decoupling_decoupling_r_gray_comp_reg;
	M8P_decoupling_decoupling_g_gray_comp_RBUS decoupling_decoupling_g_gray_comp_reg;
	M8P_decoupling_decoupling_b_gray_comp_RBUS decoupling_decoupling_b_gray_comp_reg;

	decoupling_decoupling_r_gray_comp_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_R_GRAY_COMP_reg);
	decoupling_decoupling_g_gray_comp_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_G_GRAY_COMP_reg);
	decoupling_decoupling_b_gray_comp_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_B_GRAY_COMP_reg);

	decoupling_decoupling_r_gray_comp_reg.dcp_r_p1 = dcp_comp_setting->DCP_comp_R.DCP_comp_p1;
	decoupling_decoupling_r_gray_comp_reg.dcp_r_p2 = dcp_comp_setting->DCP_comp_R.DCP_comp_p2;
	decoupling_decoupling_g_gray_comp_reg.dcp_g_p1 = dcp_comp_setting->DCP_comp_G.DCP_comp_p1;
	decoupling_decoupling_g_gray_comp_reg.dcp_g_p2 = dcp_comp_setting->DCP_comp_G.DCP_comp_p2;
	decoupling_decoupling_b_gray_comp_reg.dcp_b_p1 = dcp_comp_setting->DCP_comp_B.DCP_comp_p1;
	decoupling_decoupling_b_gray_comp_reg.dcp_b_p2 = dcp_comp_setting->DCP_comp_B.DCP_comp_p2;

	IoReg_Write32(M8P_DECOUPLING_DECOUPLING_R_GRAY_COMP_reg, decoupling_decoupling_r_gray_comp_reg.regValue);
	IoReg_Write32(M8P_DECOUPLING_DECOUPLING_G_GRAY_COMP_reg, decoupling_decoupling_g_gray_comp_reg.regValue);
	IoReg_Write32(M8P_DECOUPLING_DECOUPLING_B_GRAY_COMP_reg, decoupling_decoupling_b_gray_comp_reg.regValue);
		
}

void drvif_color_get_dcp_comp_setting(DRV_decoupling_comp_setting_t* ptr)
{
	M8P_decoupling_decoupling_r_gray_comp_RBUS decoupling_decoupling_r_gray_comp_reg;
	M8P_decoupling_decoupling_g_gray_comp_RBUS decoupling_decoupling_g_gray_comp_reg;
	M8P_decoupling_decoupling_b_gray_comp_RBUS decoupling_decoupling_b_gray_comp_reg;

	decoupling_decoupling_r_gray_comp_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_R_GRAY_COMP_reg);
	decoupling_decoupling_g_gray_comp_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_G_GRAY_COMP_reg);
	decoupling_decoupling_b_gray_comp_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_B_GRAY_COMP_reg);

	ptr->DCP_comp_R.DCP_comp_p1 = decoupling_decoupling_r_gray_comp_reg.dcp_r_p1;
	ptr->DCP_comp_R.DCP_comp_p2 = decoupling_decoupling_r_gray_comp_reg.dcp_r_p2;
	ptr->DCP_comp_G.DCP_comp_p1 = decoupling_decoupling_g_gray_comp_reg.dcp_g_p1;
	ptr->DCP_comp_G.DCP_comp_p2 = decoupling_decoupling_g_gray_comp_reg.dcp_g_p2;
	ptr->DCP_comp_B.DCP_comp_p1 = decoupling_decoupling_b_gray_comp_reg.dcp_b_p1;
	ptr->DCP_comp_B.DCP_comp_p2 = decoupling_decoupling_b_gray_comp_reg.dcp_b_p2;

		
}

void drvif_color_set_dcp_offset_setting(DRV_decoupling_offset_ctrl_t* dcp_offset)
{
	char L_offset=0;
	char R_offset=0;
	M8P_decoupling_decoupling_offset_RBUS decoupling_decoupling_offset_reg;

	decoupling_decoupling_offset_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_OFFSET_reg);

	L_offset = dcp_offset->DCP_L_offset;
	R_offset = dcp_offset->DCP_R_offset;
	
	if(L_offset < 0)
		L_offset = L_offset + 32;
	if(R_offset < 0)
		R_offset = R_offset + 32;
	
	decoupling_decoupling_offset_reg.dcp_l_offset = L_offset;
	decoupling_decoupling_offset_reg.dcp_r_offset = R_offset;

	IoReg_Write32(M8P_DECOUPLING_DECOUPLING_OFFSET_reg, decoupling_decoupling_offset_reg.regValue);

}

void drvif_color_get_dcp_offset_setting(DRV_decoupling_offset_ctrl_t* ptr)
{
	M8P_decoupling_decoupling_offset_RBUS decoupling_decoupling_offset_reg;

	decoupling_decoupling_offset_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_OFFSET_reg);

	ptr->DCP_L_offset = decoupling_decoupling_offset_reg.dcp_l_offset;
	ptr->DCP_R_offset = decoupling_decoupling_offset_reg.dcp_r_offset;

}

void drvif_color_set_dcp_en(unsigned char enable)
{
	M8P_decoupling_decoupling_control_RBUS decoupling_decoupling_control_reg;
	M8P_decoupling_decoupling_double_buffer_ctrl_RBUS decoupling_decoupling_double_buffer_ctrl_reg;

	// set dcp function enable
	decoupling_decoupling_control_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_CONTROL_reg);
	decoupling_decoupling_control_reg.dcp_en = enable;
	IoReg_Write32(M8P_DECOUPLING_DECOUPLING_CONTROL_reg, decoupling_decoupling_control_reg.regValue);
	

	// double buffer apply
	decoupling_decoupling_double_buffer_ctrl_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_DOUBLE_BUFFER_CTRL_reg);
	decoupling_decoupling_double_buffer_ctrl_reg.decoupling_db_apply = 1;
	IoReg_Write32(M8P_DECOUPLING_DECOUPLING_DOUBLE_BUFFER_CTRL_reg, decoupling_decoupling_double_buffer_ctrl_reg.regValue);

}

unsigned char driver_color_get_dcp_enable(void)
{
	M8P_decoupling_decoupling_control_RBUS decoupling_decoupling_control_reg;
	
	decoupling_decoupling_control_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_CONTROL_reg);
	if( decoupling_decoupling_control_reg.dcp_en!= 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void drvif_color_set_dcp_H_filter_ctrl_setting(DRV_decoupling_H_filter_ctrl_t* dcp_H_filter_ctrl)
{
	M8P_decoupling_filter_offset_RBUS decoupling_filter_offset_reg;

	decoupling_filter_offset_reg.regValue = IoReg_Read32(M8P_DECOUPLING_FILTER_OFFSET_reg);

	decoupling_filter_offset_reg.h_filter_coef_sum = dcp_H_filter_ctrl->H_filter_coef_sum;
	decoupling_filter_offset_reg.h_filter_coef0 = dcp_H_filter_ctrl->H_filter_coef0;
	decoupling_filter_offset_reg.h_filter_coef1 = dcp_H_filter_ctrl->H_filter_coef1;
	decoupling_filter_offset_reg.h_filter_coef2 = dcp_H_filter_ctrl->H_filter_coef2;
	decoupling_filter_offset_reg.h_filter_coef3 = dcp_H_filter_ctrl->H_filter_coef3;
	decoupling_filter_offset_reg.h_filter_th= dcp_H_filter_ctrl->H_filter_th;

	IoReg_Write32(M8P_DECOUPLING_FILTER_OFFSET_reg, decoupling_filter_offset_reg.regValue);

}

void drvif_color_get_dcp_H_filter_ctrl_setting(DRV_decoupling_H_filter_ctrl_t* ptr)
{
	M8P_decoupling_filter_offset_RBUS decoupling_filter_offset_reg;

	decoupling_filter_offset_reg.regValue = IoReg_Read32(M8P_DECOUPLING_FILTER_OFFSET_reg);

	ptr->H_filter_coef_sum = decoupling_filter_offset_reg.h_filter_coef_sum;
	ptr->H_filter_coef0 = decoupling_filter_offset_reg.h_filter_coef0;
	ptr->H_filter_coef1 = decoupling_filter_offset_reg.h_filter_coef1;
	ptr->H_filter_coef2 = decoupling_filter_offset_reg.h_filter_coef2;
	ptr->H_filter_coef3 = decoupling_filter_offset_reg.h_filter_coef3;
	ptr->H_filter_th = decoupling_filter_offset_reg.h_filter_th;

}

void drvif_color_set_dcp_H_filter_en(unsigned char enable)
{
	M8P_decoupling_decoupling_control_RBUS decoupling_decoupling_control_reg;
	M8P_decoupling_decoupling_double_buffer_ctrl_RBUS decoupling_decoupling_double_buffer_ctrl_reg;

	// set dcp function enable
	decoupling_decoupling_control_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_CONTROL_reg);
	decoupling_decoupling_control_reg.dcp_en = enable;
	decoupling_decoupling_control_reg.filter_en = enable;
	IoReg_Write32(M8P_DECOUPLING_DECOUPLING_CONTROL_reg, decoupling_decoupling_control_reg.regValue);
	

	// double buffer apply
	decoupling_decoupling_double_buffer_ctrl_reg.regValue = IoReg_Read32(M8P_DECOUPLING_DECOUPLING_DOUBLE_BUFFER_CTRL_reg);
	decoupling_decoupling_double_buffer_ctrl_reg.decoupling_db_apply = 1;
	IoReg_Write32(M8P_DECOUPLING_DECOUPLING_DOUBLE_BUFFER_CTRL_reg, decoupling_decoupling_double_buffer_ctrl_reg.regValue);

}

