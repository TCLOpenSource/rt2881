/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2009
  * All rights reserved.
  * ========================================================================*/

/*================= File Description =======================================*/
/**
 * @file
 * 	This file is for color related functions.
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
#include <tvscalercontrol/vip/pcid.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/vip/vip_reg_def.h>

#include <rbus/Block_color_reg.h>
//#include <rbus/lg_pod_reg.h>
#include <rbus/sys_reg_reg.h>
#include <tvscalercontrol/vip/color.h>


/*================================== Function ===============================*/
void drvif_color_set_bc_global_control_setting(DRV_BC_Global_Ctrl* pBcSetting)
{
	block_color_block_color_ctrl_RBUS block_color_block_color_ctrl_reg;
	block_color_block_color_ctrl_1_RBUS block_color_block_color_ctrl_1_reg;
	block_color_block_color_ctrl_2_RBUS block_color_block_color_ctrl_2_reg;
	block_color_block_color_db_ctrl_RBUS block_color_block_color_db_ctrl_reg;

	block_color_block_color_ctrl_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_CTRL_reg);
	block_color_block_color_ctrl_1_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_CTRL_1_reg);
	block_color_block_color_ctrl_2_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_CTRL_2_reg);
	block_color_block_color_db_ctrl_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_DB_CTRL_reg);

	block_color_block_color_ctrl_reg.block_color_en = pBcSetting->bc_en;
	block_color_block_color_ctrl_reg.block_color_mode = pBcSetting->bc_color_mode;
	block_color_block_color_ctrl_reg.blk_hnum = pBcSetting->bc_blk_hnum;
	block_color_block_color_ctrl_reg.blk_vnum = pBcSetting->bc_blk_vnum;
	
	block_color_block_color_ctrl_1_reg.startx = pBcSetting->bc_startx;
	block_color_block_color_ctrl_1_reg.starty = pBcSetting->bc_starty;

	block_color_block_color_ctrl_2_reg.blk_hsize = pBcSetting->bc_blk_hsize;
	block_color_block_color_ctrl_2_reg.blk_vsize = pBcSetting->bc_blk_vsize;

	block_color_block_color_db_ctrl_reg.db_en = pBcSetting->bc_db_en;
	block_color_block_color_db_ctrl_reg.db_apply= pBcSetting->bc_db_apply;

	IoReg_Write32(BLOCK_COLOR_BLOCK_COLOR_CTRL_reg, block_color_block_color_ctrl_reg.regValue);
	IoReg_Write32(BLOCK_COLOR_BLOCK_COLOR_CTRL_1_reg, block_color_block_color_ctrl_1_reg.regValue);
	IoReg_Write32(BLOCK_COLOR_BLOCK_COLOR_CTRL_2_reg, block_color_block_color_ctrl_2_reg.regValue);
	IoReg_Write32(BLOCK_COLOR_BLOCK_COLOR_DB_CTRL_reg, block_color_block_color_db_ctrl_reg.regValue);	
}

void drvif_color_get_bc_global_control_setting(DRV_BC_Global_Ctrl *ptr)
{

	block_color_block_color_ctrl_RBUS block_color_block_color_ctrl_reg;
	block_color_block_color_ctrl_1_RBUS block_color_block_color_ctrl_1_reg;
	block_color_block_color_ctrl_2_RBUS block_color_block_color_ctrl_2_reg;
	block_color_block_color_db_ctrl_RBUS block_color_block_color_db_ctrl_reg;

	block_color_block_color_ctrl_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_CTRL_reg);
	block_color_block_color_ctrl_1_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_CTRL_1_reg);
	block_color_block_color_ctrl_2_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_CTRL_2_reg);
	block_color_block_color_db_ctrl_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_DB_CTRL_reg);


	ptr->bc_en = block_color_block_color_ctrl_reg.block_color_en;
	ptr->bc_color_mode = block_color_block_color_ctrl_reg.block_color_mode;
	ptr->bc_blk_hnum = block_color_block_color_ctrl_reg.blk_hnum;
	ptr->bc_blk_vnum = block_color_block_color_ctrl_reg.blk_vnum;
	ptr->bc_blk_hsize = block_color_block_color_ctrl_2_reg.blk_hsize;
	ptr->bc_blk_vsize = block_color_block_color_ctrl_2_reg.blk_vsize;
	ptr->bc_startx = block_color_block_color_ctrl_1_reg.startx;
	ptr->bc_starty = block_color_block_color_ctrl_1_reg.starty;
	ptr->bc_db_en = block_color_block_color_db_ctrl_reg.db_en;

}

void fwif_color_set_bc_global_control_setting(DRV_BC_Global_Ctrl* pBcSetting)
{

	unsigned short spinBox_Width, spinBox_Height, h_num,v_num, h_s, h_e, v_s, v_e;

	DRV_BC_Global_Ctrl BC_setting;

	//drvif_color_Get_main_den_h_v_start_end(&h_s, &h_e, &v_s, &v_e);

	spinBox_Width = h_e - h_s;
	spinBox_Height = v_e - v_s;

	//spinBox_Width = 960;
	//spinBox_Height = 1080;

	h_num = pBcSetting->bc_blk_hnum;
	v_num = pBcSetting->bc_blk_vnum;

	if(h_num>32)
		h_num = 31;
	if(v_num>16)
		h_num = 16;

	BC_setting.bc_blk_hnum = h_num;
	BC_setting.bc_blk_vnum = v_num;
	BC_setting.bc_blk_hsize = spinBox_Width/h_num;
	BC_setting.bc_blk_vsize = spinBox_Height/v_num;
	BC_setting.bc_color_mode = pBcSetting->bc_color_mode;
	BC_setting.bc_startx = pBcSetting->bc_startx;
	BC_setting.bc_starty = pBcSetting->bc_starty;
	BC_setting.bc_en = pBcSetting->bc_en;
	
	
	drvif_color_set_bc_global_control_setting(&BC_setting);
	
}

unsigned char drvif_color_get_BC_Enable_verify(void)
{
	block_color_block_color_ctrl_RBUS block_color_block_color_ctrl_reg;
	block_color_block_color_ctrl_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_CTRL_reg);
	
	return block_color_block_color_ctrl_reg.block_color_en;
}

void drvif_color_set_bc_rgb2y_coef_setting_verify(DRV_BC_COLOR_RGB2Y_COEF_Ctrl* pBcY2RSetting)
{
	block_color_block_color_rgb2y_coef_RBUS block_color_block_color_rgb2y_coef_reg;

	block_color_block_color_rgb2y_coef_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_RGB2Y_COEF_reg);

	block_color_block_color_rgb2y_coef_reg.block_color_b_coeff = pBcY2RSetting->bc_color_b_coeff;
	block_color_block_color_rgb2y_coef_reg.block_color_g_coeff = pBcY2RSetting->bc_color_g_coeff;
	block_color_block_color_rgb2y_coef_reg.block_color_r_coeff = pBcY2RSetting->bc_color_r_coeff;

	IoReg_Write32(BLOCK_COLOR_BLOCK_COLOR_RGB2Y_COEF_reg, block_color_block_color_rgb2y_coef_reg.regValue);

}

void drvif_color_get_bc_data_verify(unsigned int* pRetTbl_ch0, unsigned int* pRetTbl_ch1, unsigned int* pRetTbl_ch2)
{
	unsigned int i=0,j=0;
	unsigned char h_num, v_num;
	bool dual_display = 0;
	//bool Read_flag = 0, timecount=0;;

	block_color_block_color_ctrl_RBUS block_color_block_color_ctrl_reg;
	block_color_block_color_fw_read_RBUS block_color_block_color_fw_read_reg;
	block_color_block_color_read_data_RBUS block_color_block_color_read_data_reg;

	block_color_block_color_ctrl_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_CTRL_reg);
	block_color_block_color_fw_read_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_FW_READ_reg);
	block_color_block_color_read_data_reg.regValue =  IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_READ_DATA_reg);
	
	h_num = block_color_block_color_ctrl_reg.blk_hnum + 1;
	v_num = block_color_block_color_ctrl_reg.blk_vnum + 1;

	//while(Read_flag)
	/*timecount = 100;
	while(timecount>10)
	{
		Read_flag = block_color_block_color_fw_read_reg.block_color_done;
		if(Read_flag == 1)
			break;
	
	}*/
	if(dual_display == 0)
	{
		for(i=0;i<v_num;i++)
		{
			for(j=0;j<h_num;j++)
			{

				block_color_block_color_fw_read_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_FW_READ_reg);

				
				block_color_block_color_fw_read_reg.block_color_col_addr = j;
				block_color_block_color_fw_read_reg.block_color_row_addr = i;
				

				//msleep_interruptible(10);
				IoReg_Write32(BLOCK_COLOR_BLOCK_COLOR_FW_READ_reg, block_color_block_color_fw_read_reg.regValue);
				

				block_color_block_color_read_data_reg.regValue =  IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_READ_DATA_reg);
				
				*(pRetTbl_ch0 + (i*(h_num+1)) + j) = block_color_block_color_read_data_reg.block_color_data_ch0;
				*(pRetTbl_ch1 + (i*(h_num+1)) + j) = block_color_block_color_read_data_reg.block_color_data_ch1;
				*(pRetTbl_ch2 + (i*(h_num+1)) + j) = block_color_block_color_read_data_reg.block_color_data_ch2;
				//pRetTbl_ch1[i][j] = block_color_block_color_read_data_reg.block_color_data_ch1;
				//pRetTbl_ch2[i][j] = block_color_block_color_read_data_reg.block_color_data_ch2;
			
			}
		}
	}
	else
	{
		for(i=0;i<v_num;i++)
		{
			for(j=0;j<h_num*2;j++)
			{

				block_color_block_color_fw_read_reg.regValue = IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_FW_READ_reg);

				if(j>=h_num)
					block_color_block_color_fw_read_reg.block_color_col_addr = j;
				else
					block_color_block_color_fw_read_reg.block_color_col_addr = (j - h_num) +15;
				
				block_color_block_color_fw_read_reg.block_color_row_addr = i;
				

				//msleep_interruptible(10);
				IoReg_Write32(BLOCK_COLOR_BLOCK_COLOR_FW_READ_reg, block_color_block_color_fw_read_reg.regValue);
				

				block_color_block_color_read_data_reg.regValue =  IoReg_Read32(BLOCK_COLOR_BLOCK_COLOR_READ_DATA_reg);
				
				*(pRetTbl_ch0 + (i*(h_num+1)*2) + j) = block_color_block_color_read_data_reg.block_color_data_ch0;
				*(pRetTbl_ch1 + (i*(h_num+1)*2) + j) = block_color_block_color_read_data_reg.block_color_data_ch1;
				*(pRetTbl_ch2 + (i*(h_num+1)*2) + j) = block_color_block_color_read_data_reg.block_color_data_ch2;
			
			}
		}	
	}
	

	//block_color_block_color_fw_read_reg.block_color_done = 0;

	//IoReg_Write32(BLOCK_COLOR_BLOCK_COLOR_FW_READ_reg, block_color_block_color_fw_read_reg.regValue);	

}

