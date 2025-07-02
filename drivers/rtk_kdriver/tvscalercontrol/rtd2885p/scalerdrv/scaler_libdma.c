/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2024
  * All rights reserved.
  * ========================================================================*/

/*================= File Description =======================================*/
/**
 * @file
 * 	This file is for scaler libdma register control.
 *
 * @author 	$Author$
 * @date		$Date$
 * @version	$Revision$
 * @ingroup	clock
 */

/**
 * @addtogroup clock
 * @{
 */

/*============================ Module dependency  ===========================*/
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <tvscalercontrol/scalerdrv/scaler_libdma.h>
#include <rbus/M8P_mdomain_cap_reg.h>
#include <rbus/M8P_mdomain_disp_reg.h>
#include <rbus/M8P_mdom_sub_cap_reg.h>
#include <rbus/M8P_mdom_sub_disp_reg.h>

#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
#else
	#include <rtk_kdriver/scalercommon/scalerCommon.h>
#endif

#include <tvscalercontrol/io/ioregdrv.h>
/*===================================  Types ================================*/
/*================================== Variables ==============================*/
/*================================ Definitions ==============================*/
/*================================== Function ===============================*/
void libdma_set_disp_reg_doublebuffer_enable(unsigned char display, unsigned int enable)
{
    if (display == SLR_MAIN_DISPLAY){
        M8P_mdomain_disp_disp0_db_ctrl_RBUS m8p_mdomain_disp_disp0_db_ctrl_reg;
        m8p_mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
        m8p_mdomain_disp_disp0_db_ctrl_reg.disp0_db_en = enable;
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg, m8p_mdomain_disp_disp0_db_ctrl_reg.regValue);
    } else {
        M8P_mdom_sub_disp_disp1_db_ctrl_RBUS m8p_mdom_sub_disp_disp1_db_ctrl_reg;
        m8p_mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg);
        m8p_mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en = enable;
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg, m8p_mdom_sub_disp_disp1_db_ctrl_reg.regValue);
    }
}

void libdma_set_disp_reg_doublebuffer_apply(unsigned char display)
{
    if (display == SLR_MAIN_DISPLAY){
        M8P_mdomain_disp_disp0_db_ctrl_RBUS m8p_mdomain_disp_disp0_db_ctrl_reg;
        m8p_mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
        m8p_mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply = 1;
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg, m8p_mdomain_disp_disp0_db_ctrl_reg.regValue);
    } else {
        M8P_mdom_sub_disp_disp1_db_ctrl_RBUS m8p_mdom_sub_disp_disp1_db_ctrl_reg;
        m8p_mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg);
        m8p_mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg, m8p_mdom_sub_disp_disp1_db_ctrl_reg.regValue);
    }
}

void libdma_set_mdomain_single_buffer_enable(unsigned char display, unsigned char enable, unsigned char single_buffer_num)
{
    if (display == SLR_MAIN_DISPLAY){
        M8P_mdomain_cap_cap0_buf_control_RBUS M8P_mdomain_cap_cap0_buf_control_reg;
        M8P_mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg);
        M8P_mdomain_cap_cap0_buf_control_reg.cap0_buf_force_en = enable;
        M8P_mdomain_cap_cap0_buf_control_reg.cap0_buf_force_addr = single_buffer_num;
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg, M8P_mdomain_cap_cap0_buf_control_reg.regValue);
    } else{
        M8P_mdom_sub_cap_cap1_buf_control_RBUS M8P_mdom_sub_cap_cap1_buf_control_reg;
        M8P_mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg);
        M8P_mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_en = enable;
        M8P_mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_addr = single_buffer_num;
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg, M8P_mdom_sub_cap_cap1_buf_control_reg.regValue);
    }
}

void libdma_qs_sync_mdomain_buffer_adder(unsigned char display)
{
    if (display == SLR_MAIN_DISPLAY){
        drvif_memory_set_memtag_startaddr(IoReg_Read32(M8P_MDOMAIN_CAP_cap0_third_buf_addr_reg), MEMIDX_MAIN_THIRD);
        drvif_memory_set_memtag_startaddr(IoReg_Read32(M8P_MDOMAIN_CAP_cap0_second_buf_addr_reg), MEMIDX_MAIN_SEC);
        drvif_memory_set_memtag_startaddr(IoReg_Read32(M8P_MDOMAIN_CAP_cap0_first_buf_addr_reg), MEMIDX_MAIN);
    } else {
        drvif_memory_set_memtag_startaddr(IoReg_Read32(M8P_MDOM_SUB_CAP_cap1_third_buf_addr_reg), MEMIDX_SUB_THIRD);
        drvif_memory_set_memtag_startaddr(IoReg_Read32(M8P_MDOM_SUB_CAP_cap1_second_buf_addr_reg), MEMIDX_SUB_SEC);
        drvif_memory_set_memtag_startaddr(IoReg_Read32(M8P_MDOM_SUB_CAP_cap1_first_buf_addr_reg), MEMIDX_SUB);
    }

}
/*======================== End of File =======================================*/
/**
*
* @}
*/
