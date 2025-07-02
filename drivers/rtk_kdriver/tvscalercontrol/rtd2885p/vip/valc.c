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
#include <tvscalercontrol/vip/valc.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/vip/vip_reg_def.h>
#include <tvscalercontrol/vip/color.h>
#include <rbus/valc_reg.h>
#include <rbus/M8P_valc_reg.h>

// for checking Merlin8 or Merlin8p
#include <mach/rtk_platform.h>
// README
// 1BIN flag usage template
/*
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		// merlin8p IC , run merlin8p flow
	}
*/

// TODO LIST
// For VALC Merlin8 vs Merlin8p has following differences
// 1. VALC SAT protection         (update to new algorithm completely different from M8) 
// FIXED : drvif_color_set_valc_SAT_protect (DONE)
// NOTE  : M8P VALC SAT protection is completely different anything related to SAT protection need to be rechecked
// M8  reg related to SAT protection        PCID_VALC_SAT_0 ~ PCID_VALC_SAT_1
// M8P reg related to SAT protection        PCID_VALC_SAT_0 ~ VALC_SAT_CTRL_3 & PCID_VALC_SAT_SEG_LEN

// 2. VALC WINDIW mode (boundary) (new feature compared to M8)
// M8P reg related to VALC window mode      VALC_WINDOW_MODE_CTRL
//                                          VALC_WINDOW_MODE_CTRL_2 ~ VALC_WINDOW_MODE_CTRL_3
// ADDED : drvif_color_set_valc_window_mode_enable        (DONE)
//         drvif_color_set_valc_window_mode_smooth_enable (DONE)
//         drvif_color_set_valc_window_mode_setting       (DONE)
//         drvif_color_set_valc_window_mode_smooth_weight (DONE)

// 3. VALC MULTI SCENE mode       (new feature compared to M8)
// M8P reg related to VALC MULTI SCENE mode VALC_MULTI_SCENE_GLOBAL_CTRL
//                                          VALC_MULTI_SCENE1_CTRL & VALC_MULTI_SCENE1_CTRL_2
//                                          VALC_MULTI_SCENE2_CTRL & VALC_MULTI_SCENE2_CTRL_2
//                                          VALC_MULTI_SCENE3_CTRL & VALC_MULTI_SCENE3_CTRL_2
//                                          VALC_MULTI_SCENE4_CTRL & VALC_MULTI_SCENE4_CTRL_2
// ADDED : drvif_color_set_valc_multi_scene_global_enable     (DONE)
//         drvif_color_set_valc_multi_scene_inner_enable      (DONE)
//         drvif_color_set_valc_multi_scene_inner_setting     (DONE)
// 	       drvif_color_set_valc_multi_scene_all_inner_setting (DONE)
 
// 4. VALC MULTI_CURVE mode       (new feature compared to M8)
// M8P reg related to VALC MULTI_CURVE mode VALC_MULTI_CURVE_CTRL
// 										    VALC_MULTI_CURVE_CTRL_2
// ADDED : drvif_color_set_valc_curve_mode_enable  (DONE)
//         drvif_color_set_valc_curve_mode_setting (DONE)

// 5. VALC read/write table       (update to new algorithm completely different from M8)
// FIXED : drvif_color_set_valc_table (DONE)
//         drvif_color_get_valc_table (DONE)
// ADD   : drvif_color_get_specific_valc_table => get only normal or strong or weak table (DONE)

/*================================ Global Variables ==============================*/
unsigned int valcTbl[3][2][VALC_TBL_LEN+3] = {{{0}}};
unsigned char valcSmoothWeight[4] = {0};
/*================================ Definitions ==============================*/
#undef VIPprintf
#define VIPprintf(fmt, args...)	VIPprintfPrivate(VIP_LOGLEVEL_DEBUG,VIP_LOGMODULE_VIP_PCID_DEBUG,fmt,##args)

/*================================== Function ===============================*/


void drvif_color_set_valc_enable(unsigned char bEnable)
{
	// put the block at the front position
	unsigned long flags;//for spin_lock_irqsave
	valc_pcid_valc_ctrl_RBUS valc_pcid_valc_ctrl_reg;
	ppoverlay_display_timing_ctrl2_RBUS display_timing_ctrl2_reg;

	if (drvif_color_get_HW_MEX_STATUS(MEX_VALC) != 1) {
		rtd_pr_vpq_info("HW_MEX,  MEX_VALC != 1, EN\n");
		return;
	}

	display_timing_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL2_reg);
	if( bEnable != 0 && (display_timing_ctrl2_reg.hcic_enable != 0 || display_timing_ctrl2_reg.valc_en == 0) )
	{
		// hcic and valc cannot be enabled at the same time
		rtd_pr_vpq_info("[Warning][%s][%s] DTG hcic/valc = %d/%d, force disable hcic and enable valc\n",
			__FILE__, __func__, display_timing_ctrl2_reg.hcic_enable, display_timing_ctrl2_reg.valc_en);
		/*	set stage2 smooth toggle protect */
		IoReg_SetBits(PPOVERLAY_dispd_stage2_sm_ctrl_reg, PPOVERLAY_dispd_stage2_sm_ctrl_dispd_stage2_smooth_toggle_protect_mask);

		display_timing_ctrl2_reg.hcic_enable = 0;
		display_timing_ctrl2_reg.valc_en = 1;
		IoReg_Write32(PPOVERLAY_Display_Timing_CTRL2_reg, display_timing_ctrl2_reg.regValue);

		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
		/*	set stage2 smooth toggle protect */
		IoReg_SetBits(PPOVERLAY_dispd_smooth_toggle2_reg, PPOVERLAY_dispd_smooth_toggle2_dispd_stage2_smooth_toggle_apply_mask);
		spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
		up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	}

	valc_pcid_valc_ctrl_reg.regValue = IoReg_Read32(VALC_PCID_VALC_CTRL_reg);
	valc_pcid_valc_ctrl_reg.valc_func_en = bEnable;
	IoReg_Write32(VALC_PCID_VALC_CTRL_reg, valc_pcid_valc_ctrl_reg.regValue);
}

void drvif_color_set_valc_SpatialPattern(DRV_valc_pattern_ctrl_t* pPATCtrl)
{
	unsigned int i=0;
	unsigned int value=0;

	valc_valc_sp_ctrl_RBUS valc_valc_sp_ctrl_reg;
	valc_valc_sp_0_RBUS valc_valc_sp_0_reg;
	valc_valc_sp_1_RBUS valc_valc_sp_1_reg;
	valc_valc_sp_2_RBUS valc_valc_sp_2_reg;
	valc_valc_sp_l3_RBUS valc_valc_sp_l3_reg;
	valc_valc_db_RBUS valc_valc_db_reg;

	valc_valc_db_reg.regValue = IoReg_Read32(VALC_VALC_DB_reg);
	valc_valc_db_reg.db_en = 1;
	IoReg_Write32(VALC_VALC_DB_reg, valc_valc_db_reg.regValue);


	valc_valc_sp_ctrl_reg.regValue = IoReg_Read32(VALC_VALC_SP_CTRL_reg);
	valc_valc_sp_0_reg.regValue = IoReg_Read32(VALC_VALC_SP_0_reg);
	valc_valc_sp_1_reg.regValue = IoReg_Read32(VALC_VALC_SP_1_reg);
	valc_valc_sp_2_reg.regValue = IoReg_Read32(VALC_VALC_SP_2_reg);
	valc_valc_sp_l3_reg.regValue = IoReg_Read32(VALC_VALC_SP_L3_reg);

	valc_valc_sp_ctrl_reg.valc_sp_h = pPATCtrl->Cyclic_H;
	valc_valc_sp_ctrl_reg.valc_sp_v = pPATCtrl->Cyclic_V;

	// line0
	value=0;
	for(i=0; i<16; i++)
	{
		value = ((value << 1) + pPATCtrl->Pattern[0][i]);
	}
	valc_valc_sp_0_reg.valc_sp_line0 = value;

	// line1
	value=0;
	for(i=0; i<16; i++)
	{
		value = ((value << 1) + pPATCtrl->Pattern[1][i]);
	}
	valc_valc_sp_0_reg.valc_sp_line1 = value;

	// line2
	value=0;
	for(i=0; i<16; i++)
	{
		value = ((value << 1) + pPATCtrl->Pattern[2][i]);
	}
	valc_valc_sp_1_reg.valc_sp_line2 = value;

	// line3
	value=0;
	for(i=0; i<16; i++)
	{
		value = ((value << 1) + pPATCtrl->Pattern[3][i]);
	}
	valc_valc_sp_1_reg.valc_sp_line3 = value;

	// line4
	value=0;
	for(i=0; i<16; i++)
	{
		value = ((value << 1) + pPATCtrl->Pattern[4][i]);
	}
	valc_valc_sp_2_reg.valc_sp_line4 = value;

	// line5
	value=0;
	for(i=0; i<16; i++)
	{
		value = ((value << 1) + pPATCtrl->Pattern[5][i]);
	}
	valc_valc_sp_2_reg.valc_sp_line5 = value;

	// line6
	value=0;
	for(i=0; i<16; i++)
	{
		value = ((value << 1) + pPATCtrl->Pattern[6][i]);
	}
	valc_valc_sp_l3_reg.valc_sp_line6 = value;

	// line7
	value=0;
	for(i=0; i<16; i++)
	{
		value = ((value << 1) + pPATCtrl->Pattern[7][i]);
	}
	valc_valc_sp_l3_reg.valc_sp_line7 = value;

	IoReg_Write32(VALC_VALC_SP_CTRL_reg, valc_valc_sp_ctrl_reg.regValue);
	IoReg_Write32(VALC_VALC_SP_0_reg, valc_valc_sp_0_reg.regValue);
	IoReg_Write32(VALC_VALC_SP_1_reg, valc_valc_sp_1_reg.regValue);
	IoReg_Write32(VALC_VALC_SP_2_reg, valc_valc_sp_2_reg.regValue);
	IoReg_Write32(VALC_VALC_SP_L3_reg, valc_valc_sp_l3_reg.regValue);

	//double buffer apply
	valc_valc_db_reg.regValue = IoReg_Read32(VALC_VALC_DB_reg);
	valc_valc_db_reg.db_apply = 1;
	IoReg_Write32(VALC_VALC_DB_reg, valc_valc_db_reg.regValue);
}

void drvif_color_set_valc_HPF_protect(DRV_valc_hpf_ctrl_t* pHPFCtrl)
{
	valc_pcid_valc_ctrl_RBUS valc_pcid_valc_ctrl_reg;
	valc_pcid_valc_hpf_0_RBUS valc_pcid_valc_hpf_0_reg;
	valc_pcid_valc_hpf_1_RBUS valc_pcid_valc_hpf_1_reg;

	valc_pcid_valc_ctrl_reg.regValue = IoReg_Read32(VALC_PCID_VALC_CTRL_reg);
	valc_pcid_valc_hpf_0_reg.regValue = IoReg_Read32(VALC_PCID_VALC_HPF_0_reg);
	valc_pcid_valc_hpf_1_reg.regValue = IoReg_Read32(VALC_PCID_VALC_HPF_1_reg);

	valc_pcid_valc_ctrl_reg.valc_hpf_clip = pHPFCtrl->ClipSel;

	valc_pcid_valc_hpf_1_reg.valc_hpf_w0  = pHPFCtrl->Weight[0];
	valc_pcid_valc_hpf_1_reg.valc_hpf_w1  = pHPFCtrl->Weight[1];
	valc_pcid_valc_hpf_1_reg.valc_hpf_w2  = pHPFCtrl->Weight[2];
	valc_pcid_valc_hpf_1_reg.valc_hpf_w3  = pHPFCtrl->Weight[3];
	valc_pcid_valc_hpf_1_reg.valc_hpf_w4  = pHPFCtrl->Weight[4];
	valc_pcid_valc_hpf_1_reg.valc_hpf_w5  = pHPFCtrl->Weight[5];
	valc_pcid_valc_hpf_1_reg.valc_hpf_w6  = pHPFCtrl->Weight[6];
	valc_pcid_valc_hpf_1_reg.valc_hpf_w7  = pHPFCtrl->Weight[7];

	valc_pcid_valc_hpf_0_reg.valc_hpf_w8  = pHPFCtrl->Weight[8];
	valc_pcid_valc_hpf_0_reg.valc_hpf_w9  = pHPFCtrl->Weight[9];
	valc_pcid_valc_hpf_0_reg.valc_hpf_w10 = pHPFCtrl->Weight[10];
	valc_pcid_valc_hpf_0_reg.valc_hpf_w11 = pHPFCtrl->Weight[11];
	valc_pcid_valc_hpf_0_reg.valc_hpf_w12 = pHPFCtrl->Weight[12];
	valc_pcid_valc_hpf_0_reg.valc_hpf_w13 = pHPFCtrl->Weight[13];
	valc_pcid_valc_hpf_0_reg.valc_hpf_w14 = pHPFCtrl->Weight[14];
	valc_pcid_valc_hpf_0_reg.valc_hpf_w15 = pHPFCtrl->Weight[15];

	IoReg_Write32(VALC_PCID_VALC_CTRL_reg, valc_pcid_valc_ctrl_reg.regValue);
	IoReg_Write32(VALC_PCID_VALC_HPF_0_reg, valc_pcid_valc_hpf_0_reg.regValue);
	IoReg_Write32(VALC_PCID_VALC_HPF_1_reg, valc_pcid_valc_hpf_1_reg.regValue);
}

void drvif_color_set_valc_SAT_protect(DRV_valc_sat_ctrl_t* pSATCtrl)
{
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
		valc_pcid_valc_sat_0_RBUS valc_pcid_valc_sat_0_reg;
		valc_pcid_valc_sat_1_RBUS valc_pcid_valc_sat_1_reg;

		valc_pcid_valc_sat_0_reg.regValue = IoReg_Read32(VALC_PCID_VALC_SAT_0_reg);
		valc_pcid_valc_sat_1_reg.regValue = IoReg_Read32(VALC_PCID_VALC_SAT_1_reg);

		valc_pcid_valc_sat_1_reg.valc_sat_w0  = pSATCtrl->Weight[0];
		valc_pcid_valc_sat_1_reg.valc_sat_w1  = pSATCtrl->Weight[1];
		valc_pcid_valc_sat_1_reg.valc_sat_w2  = pSATCtrl->Weight[2];
		valc_pcid_valc_sat_1_reg.valc_sat_w3  = pSATCtrl->Weight[3];
		valc_pcid_valc_sat_1_reg.valc_sat_w4  = pSATCtrl->Weight[4];
		valc_pcid_valc_sat_1_reg.valc_sat_w5  = pSATCtrl->Weight[5];
		valc_pcid_valc_sat_1_reg.valc_sat_w6  = pSATCtrl->Weight[6];
		valc_pcid_valc_sat_1_reg.valc_sat_w7  = pSATCtrl->Weight[7];

		valc_pcid_valc_sat_0_reg.valc_sat_w8  = pSATCtrl->Weight[8];
		valc_pcid_valc_sat_0_reg.valc_sat_w9  = pSATCtrl->Weight[9];
		valc_pcid_valc_sat_0_reg.valc_sat_w10 = pSATCtrl->Weight[10];
		valc_pcid_valc_sat_0_reg.valc_sat_w11 = pSATCtrl->Weight[11];
		valc_pcid_valc_sat_0_reg.valc_sat_w12 = pSATCtrl->Weight[12];
		valc_pcid_valc_sat_0_reg.valc_sat_w13 = pSATCtrl->Weight[13];
		valc_pcid_valc_sat_0_reg.valc_sat_w14 = pSATCtrl->Weight[14];
		valc_pcid_valc_sat_0_reg.valc_sat_w15 = pSATCtrl->Weight[15];

		IoReg_Write32(VALC_PCID_VALC_SAT_0_reg, valc_pcid_valc_sat_0_reg.regValue);
		IoReg_Write32(VALC_PCID_VALC_SAT_1_reg, valc_pcid_valc_sat_1_reg.regValue);
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
		M8P_valc_pcid_valc_sat_0_RBUS       valc_pcid_valc_sat_0_reg;
		M8P_valc_pcid_valc_sat_1_RBUS       valc_pcid_valc_sat_1_reg;
		M8P_valc_pcid_valc_sat_2_RBUS       valc_pcid_valc_sat_2_reg;
		M8P_valc_pcid_valc_sat_3_RBUS       valc_pcid_valc_sat_3_reg;
		// M8P_valc_pcid_valc_sat_seg_len_RBUS valc_pcid_valc_sat_seg_len_reg;

		valc_pcid_valc_sat_0_reg.regValue       = IoReg_Read32(M8P_VALC_PCID_VALC_SAT_0_reg);
		valc_pcid_valc_sat_1_reg.regValue       = IoReg_Read32(M8P_VALC_PCID_VALC_SAT_1_reg);
		valc_pcid_valc_sat_2_reg.regValue       = IoReg_Read32(M8P_VALC_PCID_VALC_SAT_2_reg);
		valc_pcid_valc_sat_3_reg.regValue       = IoReg_Read32(M8P_VALC_PCID_VALC_SAT_3_reg);
		// valc_pcid_valc_sat_seg_len_reg.regValue = IoReg_Read32(M8P_VALC_PCID_VALC_SAT_SEG_LEN_reg);

		// Use M8P to simulate M8
		// M8 weight = 0  -> M8P weight =  0
		// M8 weight = 1  -> M8P weight = 31  = (( 1+1) << 4) - 1
		// M8 weight = 2  -> M8P weight = 47  = (( 2+1) << 4) - 1
		// ...
		// M8 weight = 15 -> M8P weight = 255 = ((15+1) << 4) - 1
		valc_pcid_valc_sat_3_reg.valc_sat_w0  = ((pSATCtrl->Weight[0]  + 1) << 4) - 1;
		valc_pcid_valc_sat_3_reg.valc_sat_w1  = ((pSATCtrl->Weight[1]  + 1) << 4) - 1;
		valc_pcid_valc_sat_3_reg.valc_sat_w2  = ((pSATCtrl->Weight[2]  + 1) << 4) - 1;
		valc_pcid_valc_sat_3_reg.valc_sat_w3  = ((pSATCtrl->Weight[3]  + 1) << 4) - 1;

		valc_pcid_valc_sat_2_reg.valc_sat_w4  = ((pSATCtrl->Weight[4]  + 1) << 4) - 1;
		valc_pcid_valc_sat_2_reg.valc_sat_w5  = ((pSATCtrl->Weight[5]  + 1) << 4) - 1;
		valc_pcid_valc_sat_2_reg.valc_sat_w6  = ((pSATCtrl->Weight[6]  + 1) << 4) - 1;
		valc_pcid_valc_sat_2_reg.valc_sat_w7  = ((pSATCtrl->Weight[7]  + 1) << 4) - 1;

		valc_pcid_valc_sat_1_reg.valc_sat_w8  = ((pSATCtrl->Weight[8]  + 1) << 4) - 1;
		valc_pcid_valc_sat_1_reg.valc_sat_w9  = ((pSATCtrl->Weight[9]  + 1) << 4) - 1;
		valc_pcid_valc_sat_1_reg.valc_sat_w10 = ((pSATCtrl->Weight[10] + 1) << 4) - 1;
		valc_pcid_valc_sat_1_reg.valc_sat_w11 = ((pSATCtrl->Weight[11] + 1) << 4) - 1;

		valc_pcid_valc_sat_0_reg.valc_sat_w12 = ((pSATCtrl->Weight[12] + 1) << 4) - 1;
		valc_pcid_valc_sat_0_reg.valc_sat_w13 = ((pSATCtrl->Weight[13] + 1) << 4) - 1;
		valc_pcid_valc_sat_0_reg.valc_sat_w14 = ((pSATCtrl->Weight[14] + 1) << 4) - 1;
		valc_pcid_valc_sat_0_reg.valc_sat_w15 = ((pSATCtrl->Weight[15] + 1) << 4) - 1;

		// Use M8P to simulate M8
		// M8 has no seg len setting simply write same value to all 4 seg len registers
		// valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen1 = 0xB;
		// valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen2 = 0xB;
		// valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen3 = 0xB;
		// valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen4 = 0xB;
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		// merlin8p IC , run merlin8p flow
		M8P_valc_pcid_valc_sat_0_RBUS       valc_pcid_valc_sat_0_reg;
		M8P_valc_pcid_valc_sat_1_RBUS       valc_pcid_valc_sat_1_reg;
		M8P_valc_pcid_valc_sat_2_RBUS       valc_pcid_valc_sat_2_reg;
		M8P_valc_pcid_valc_sat_3_RBUS       valc_pcid_valc_sat_3_reg;
		// M8P_valc_pcid_valc_sat_seg_len_RBUS valc_pcid_valc_sat_seg_len_reg;

		valc_pcid_valc_sat_0_reg.regValue       = IoReg_Read32(M8P_VALC_PCID_VALC_SAT_0_reg);
		valc_pcid_valc_sat_1_reg.regValue       = IoReg_Read32(M8P_VALC_PCID_VALC_SAT_1_reg);
		valc_pcid_valc_sat_2_reg.regValue       = IoReg_Read32(M8P_VALC_PCID_VALC_SAT_2_reg);
		valc_pcid_valc_sat_3_reg.regValue       = IoReg_Read32(M8P_VALC_PCID_VALC_SAT_3_reg);
		// valc_pcid_valc_sat_seg_len_reg.regValue = IoReg_Read32(M8P_VALC_PCID_VALC_SAT_SEG_LEN_reg);

		valc_pcid_valc_sat_3_reg.valc_sat_w0  = pSATCtrl->Weight[0];
		valc_pcid_valc_sat_3_reg.valc_sat_w1  = pSATCtrl->Weight[1];
		valc_pcid_valc_sat_3_reg.valc_sat_w2  = pSATCtrl->Weight[2];
		valc_pcid_valc_sat_3_reg.valc_sat_w3  = pSATCtrl->Weight[3];

		valc_pcid_valc_sat_2_reg.valc_sat_w4  = pSATCtrl->Weight[4];
		valc_pcid_valc_sat_2_reg.valc_sat_w5  = pSATCtrl->Weight[5];
		valc_pcid_valc_sat_2_reg.valc_sat_w6  = pSATCtrl->Weight[6];
		valc_pcid_valc_sat_2_reg.valc_sat_w7  = pSATCtrl->Weight[7];

		valc_pcid_valc_sat_1_reg.valc_sat_w8  = pSATCtrl->Weight[8];
		valc_pcid_valc_sat_1_reg.valc_sat_w9  = pSATCtrl->Weight[9];
		valc_pcid_valc_sat_1_reg.valc_sat_w10 = pSATCtrl->Weight[10];
		valc_pcid_valc_sat_1_reg.valc_sat_w11 = pSATCtrl->Weight[11];

		valc_pcid_valc_sat_0_reg.valc_sat_w12 = pSATCtrl->Weight[12];
		valc_pcid_valc_sat_0_reg.valc_sat_w13 = pSATCtrl->Weight[13];
		valc_pcid_valc_sat_0_reg.valc_sat_w14 = pSATCtrl->Weight[14];
		valc_pcid_valc_sat_0_reg.valc_sat_w15 = pSATCtrl->Weight[15];

		// valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen1 = pSATCtrl->seg_len[0];
		// valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen2 = pSATCtrl->seg_len[1];
		// valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen3 = pSATCtrl->seg_len[2];
		// valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen4 = pSATCtrl->seg_len[3];
	}	
}

void drvif_color_set_valc_SAT_protect_seg_len_M8P(DRV_valc_sat_ctrl_seg_len_M8P_t* pSATCtrl){
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
		VIPprintf("[Error][%s][%s] This function is for M8P IC. But current IC is M8. Simply return and do nothing.\n", __FILE__, __func__);
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
		M8P_valc_pcid_valc_sat_seg_len_RBUS valc_pcid_valc_sat_seg_len_reg;

		valc_pcid_valc_sat_seg_len_reg.regValue = IoReg_Read32(M8P_VALC_PCID_VALC_SAT_SEG_LEN_reg);

		// Use M8P to simulate M8
		// M8 has no seg len setting simply write same value to all 4 seg len registers
		valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen1 = 0xB;
		valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen2 = 0xB;
		valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen3 = 0xB;
		valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen4 = 0xB;
		VIPprintf("[Error][%s][%s] This function is for M8P IC. Current IC is M8P and Simulate M8. Set 0xB to all seg len.\n", __FILE__, __func__);
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		// merlin8p IC , run merlin8p flow
		M8P_valc_pcid_valc_sat_seg_len_RBUS valc_pcid_valc_sat_seg_len_reg;
		
		valc_pcid_valc_sat_seg_len_reg.regValue = IoReg_Read32(M8P_VALC_PCID_VALC_SAT_SEG_LEN_reg);

		valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen1 = pSATCtrl->seg_len[0];
		valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen2 = pSATCtrl->seg_len[1];
		valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen3 = pSATCtrl->seg_len[2];
		valc_pcid_valc_sat_seg_len_reg.valc_sat_seglen4 = pSATCtrl->seg_len[3];
		VIPprintf("[Error][%s][%s] This function is for M8P IC. Current IC is M8P. Setting properly.\n", __FILE__, __func__);
	}	
}


void drvif_color_set_Func_valc_enable(unsigned char bEnable, unsigned char db_en)
{	
	valc_pcid_valc_ctrl_RBUS valc_pcid_valc_ctrl_reg;
	valc_valc_db_RBUS valc_valc_db_reg;

	valc_valc_db_reg.regValue = IoReg_Read32(VALC_VALC_DB_reg);
	valc_valc_db_reg.db_en = db_en;
	valc_valc_db_reg.db_read_level = 1;
	IoReg_Write32(VALC_VALC_DB_reg, valc_valc_db_reg.regValue);	

	valc_pcid_valc_ctrl_reg.regValue = IoReg_Read32(VALC_PCID_VALC_CTRL_reg);
	valc_pcid_valc_ctrl_reg.valc_func_en = bEnable;
	IoReg_Write32(VALC_PCID_VALC_CTRL_reg, valc_pcid_valc_ctrl_reg.regValue);
}

char drvif_color_set_DTG_valc_enable(unsigned char bEnable)
{
	ppoverlay_display_timing_ctrl2_RBUS display_timing_ctrl2_reg;
	unsigned long flags;//for spin_lock_irqsave
	int cnt = 4;
	char ret = 0;
	unsigned int t0, t1, t2;

	t0 = drvif_Get_90k_Lo_clk();
	if (drvif_color_get_HW_MEX_STATUS(MEX_VALC) != 1) 
	{
		rtd_pr_vpq_emerg("HW_MEX,  MEX_VALC != 1, EN\n");
		return 1;
	}

	display_timing_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL2_reg);
	
	// hcic and valc cannot be enabled at the same time
	/*	set stage2 smooth toggle protect */
	IoReg_SetBits(PPOVERLAY_dispd_stage2_sm_ctrl_reg, PPOVERLAY_dispd_stage2_sm_ctrl_dispd_stage2_smooth_toggle_protect_mask);

	if (bEnable == 1)
	{
		display_timing_ctrl2_reg.hcic_enable = 0;	// hcic should be disable while valc enable
		display_timing_ctrl2_reg.valc_en = 1;
	}
	else
	{
		display_timing_ctrl2_reg.valc_en = 0;
	}
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL2_reg, display_timing_ctrl2_reg.regValue);

	down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
	/*	set stage2 smooth toggle protect */
	IoReg_SetBits(PPOVERLAY_dispd_smooth_toggle2_reg, PPOVERLAY_dispd_smooth_toggle2_dispd_stage2_smooth_toggle_apply_mask);	
	//IoReg_SetBits(PPOVERLAY_Double_Buffer_CTRL_reg, PPOVERLAY_Double_Buffer_CTRL_dreg_dbuf_en_mask);
	spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
	up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/

	t1 = drvif_Get_90k_Lo_clk();

	if ((PPOVERLAY_Double_Buffer_CTRL_get_dreg_dbuf_en(IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg)) == 1) &&
		(PPOVERLAY_dispd_stage2_sm_ctrl_get_dispd_stage2_smooth_toggle_protect(IoReg_Read32(PPOVERLAY_dispd_stage2_sm_ctrl_reg)) == 1))
	{
		while ((PPOVERLAY_dispd_smooth_toggle2_get_dispd_stage2_smooth_toggle_apply(IoReg_Read32(PPOVERLAY_dispd_smooth_toggle2_reg)) == 1) || (cnt>0))
		{
			usleep_range(5000, 10000);
			cnt--;
		}
		if (cnt <= 0)
		{
			rtd_pr_vpq_emerg("drvif_color_set_DTG_valc_enable, cant wait DB aply done\n");
			ret = -1;
		}
	}

	t2 = drvif_Get_90k_Lo_clk();

	rtd_pr_vpq_info("drvif_color_set_DTG_valc_enable, ret=%d, t0=%d, t1=%d, t2=%d, bEnable=%d,\n", ret, t0, t1, t2, bEnable);

	return ret;
}


void drvif_color_set_valc_table(unsigned int* pTbl, DRV_valc_channel_t ChannelSel)
{
	int i = 0;
	unsigned char bFuncEnable = 0;
	M8P_valc_pcid_valc_ctrl_RBUS valc_pcid_valc_ctrl_reg;
	M8P_valc_valc_lut_addr_RBUS  valc_valc_lut_addr_reg;
	M8P_valc_valc_lut_data_RBUS  valc_valc_lut_data_reg;

	if(ChannelSel >= _VALC_COLOR_ERR){
		VIPprintf("[Error][%s][%s] ChannelSel is invalid!\n", __FILE__, __func__);
		return;
	}

	if(!pTbl)
	{
		VIPprintf("[Error][%s][%s] Table Ptr is null!\n", __FILE__, __func__);
		return;
	}

	// reset the table internal status
	valc_pcid_valc_ctrl_reg.regValue = IoReg_Read32(VALC_PCID_VALC_CTRL_reg);
	bFuncEnable = valc_pcid_valc_ctrl_reg.valc_func_en;
	valc_pcid_valc_ctrl_reg.valc_func_en = 0;
	IoReg_Write32(VALC_PCID_VALC_CTRL_reg, valc_pcid_valc_ctrl_reg.regValue);
	valc_valc_lut_addr_reg.regValue = IoReg_Read32(VALC_VALC_LUT_ADDR_reg);
	valc_valc_lut_addr_reg.valc_lut_ax_en = 0;
	IoReg_Write32(VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

	if (drvif_color_get_HW_MEX_STATUS(MEX_VALC) != 1) {
		rtd_pr_vpq_info("HW_MEX,  MEX_VALC != 1, TBL\n");
		return;
	}
	
	// Route to different flow
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow

		// setup table access setting
		valc_valc_lut_addr_reg.regValue = IoReg_Read32(VALC_VALC_LUT_ADDR_reg);
		valc_valc_lut_addr_reg.valc_lut_sel = ChannelSel;
		valc_valc_lut_addr_reg.valc_lut_ax_en = 1;
		IoReg_Write32(VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

		for(i=0; i<VALC_TBL_LEN; i++)
		{
			valc_valc_lut_data_reg.valc_lut_data = pTbl[i];
			IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
			VIPprintf("[VPQ][VALC][W]Table[%d]=0x%x\n", i, pTbl[i]);
		}
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow

		// setup table access setting
		valc_valc_lut_addr_reg.regValue = IoReg_Read32(VALC_VALC_LUT_ADDR_reg);
		valc_valc_lut_addr_reg.valc_lut_sel = ChannelSel;
		valc_valc_lut_addr_reg.valc_lut_ax_en = 1;
		IoReg_Write32(VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

		for(i=0; i<VALC_TBL_LEN; i++)
		{
			valc_valc_lut_data_reg.valc_lut_data = pTbl[i];
			IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
			VIPprintf("[VPQ][VALC][W]Table[%d]=0x%x\n", i, pTbl[i]);
		}
		// Dummy write is required in merlin8p IC
		IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
		VIPprintf("[VPQ][VALC][W]Table[%d]=0x%x\n", 1025, pTbl[VALC_TBL_LEN-1]);
		IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
		VIPprintf("[VPQ][VALC][W]Table[%d]=0x%x\n", 1026, pTbl[VALC_TBL_LEN-1]);
		IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
		VIPprintf("[VPQ][VALC][W]Table[%d]=0x%x\n", 1027, pTbl[VALC_TBL_LEN-1]);
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		// merlin8p IC , run merlin8p flow

		// Flow for multi-curve mode off
		if(ChannelSel <= _VALC_COLOR_B){
			// setup table access setting
			valc_valc_lut_addr_reg.regValue = IoReg_Read32(VALC_VALC_LUT_ADDR_reg);
			valc_valc_lut_addr_reg.valc_lut_sel = ChannelSel;
			valc_valc_lut_addr_reg.valc_lut_ax_en = 1;
			IoReg_Write32(VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

			for(i=0; i<VALC_TBL_LEN; i++)
			{
				valc_valc_lut_data_reg.valc_lut_data = pTbl[i];
				IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
				VIPprintf("[VPQ][VALC][W]Table[%d]=0x%x\n", i, pTbl[i]);
			}
			// Dummy write is required in merlin8p IC
			IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
			VIPprintf("[VPQ][VALC][W]Table[%d]=0x%x\n", 1025, pTbl[VALC_TBL_LEN-1]);
			IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
			VIPprintf("[VPQ][VALC][W]Table[%d]=0x%x\n", 1026, pTbl[VALC_TBL_LEN-1]);
			IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
			VIPprintf("[VPQ][VALC][W]Table[%d]=0x%x\n", 1027, pTbl[VALC_TBL_LEN-1]);
		}
		// Flow for multi-curve mode on
		else{
			unsigned int idx = 0;
			// Since the real channel is from 0 to 2
			// But we need to distinguish multi-curve mode on and off
			// So need to minus 3 to get the real channel
			ChannelSel = ChannelSel - _VALC_COLOR_R_MULTICURVE; 
			
			// setup table access setting
			valc_valc_lut_addr_reg.regValue = IoReg_Read32(VALC_VALC_LUT_ADDR_reg);
			valc_valc_lut_addr_reg.valc_lut_sel = ChannelSel;
			valc_valc_lut_addr_reg.valc_lut_ax_en = 1;
			IoReg_Write32(VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

			// Write the table data, order is {normal, normal, strong, weak}
			for(i=0; i<((VALC_TBL_LEN+3) >> 2); i++)
			{
				idx = ( (2*i-1) < 0 ) ? 0 : ( 2*i-1 );
				valc_valc_lut_data_reg.valc_lut_data = pTbl[idx];
				IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
				VIPprintf("[VPQ][VALC][W]Table[%d]=0x%x\n", idx, pTbl[idx]);
				valc_valc_lut_data_reg.valc_lut_data = pTbl[2*i];
				IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
				VIPprintf("[VPQ][VALC][W]Table[%d]=0x%x\n", 2*i, pTbl[2*i]);
				valc_valc_lut_data_reg.valc_lut_data = pTbl[VALC_TBL_STRONG_START+i];
				IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
				VIPprintf("[VPQ][VALC][W]Table_strong[%d]=0x%x\n", i, pTbl[VALC_TBL_STRONG_START+i]);
				valc_valc_lut_data_reg.valc_lut_data = pTbl[VALC_TBL_WEAK_START+i];
				IoReg_Write32(VALC_VALC_LUT_DATA_reg, valc_valc_lut_data_reg.regValue);
				VIPprintf("[VPQ][VALC][W]Table_weak[%d]=0x%x\n", i, pTbl[VALC_TBL_WEAK_START+i]);
			}
		}

	}

	// Disable the table access
	valc_valc_lut_addr_reg.regValue = IoReg_Read32(VALC_VALC_LUT_ADDR_reg);
	valc_valc_lut_addr_reg.valc_lut_ax_en = 0;
	IoReg_Write32(VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

	// restore the function enable
	valc_pcid_valc_ctrl_reg.regValue = IoReg_Read32(VALC_PCID_VALC_CTRL_reg);
	valc_pcid_valc_ctrl_reg.valc_func_en = bFuncEnable;
	IoReg_Write32(VALC_PCID_VALC_CTRL_reg, valc_pcid_valc_ctrl_reg.regValue);
}

void drvif_color_get_valc_table(unsigned int* pTbl, DRV_valc_channel_t ChannelSel)
{
	int i=0;
	unsigned char bFuncEnable = 0;
	M8P_valc_pcid_valc_ctrl_RBUS valc_pcid_valc_ctrl_reg;
	M8P_valc_valc_lut_addr_RBUS  valc_valc_lut_addr_reg;
	M8P_valc_valc_lut_data_RBUS  valc_valc_lut_data_reg;

	if(ChannelSel >= _VALC_COLOR_ERR){
		VIPprintf("[Error][%s][%s] ChannelSel is invalid!\n", __FILE__, __func__);
		return;
	}

	if(!pTbl)
	{
		VIPprintf("[Error][%s][%s] Table Ptr is null!\n", __FILE__, __func__);
		return;
	}

	// reset the table internal status
	valc_pcid_valc_ctrl_reg.regValue = IoReg_Read32(VALC_PCID_VALC_CTRL_reg);
	bFuncEnable = valc_pcid_valc_ctrl_reg.valc_func_en;
	valc_pcid_valc_ctrl_reg.valc_func_en = 0;
	IoReg_Write32(VALC_PCID_VALC_CTRL_reg, valc_pcid_valc_ctrl_reg.regValue);
	valc_valc_lut_addr_reg.regValue = IoReg_Read32(VALC_VALC_LUT_ADDR_reg);
	valc_valc_lut_addr_reg.valc_lut_ax_en = 0;
	IoReg_Write32(VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow

		// setup table access setting
		valc_valc_lut_addr_reg.regValue = IoReg_Read32(VALC_VALC_LUT_ADDR_reg);
		valc_valc_lut_addr_reg.valc_lut_sel = ChannelSel;
		valc_valc_lut_addr_reg.valc_lut_ax_en = 1;
		IoReg_Write32(VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

		for(i=0; i<VALC_TBL_LEN; i++)
		{
			valc_valc_lut_data_reg.regValue = IoReg_Read32(VALC_VALC_LUT_DATA_reg);
			pTbl[i] = valc_valc_lut_data_reg.valc_lut_data;
			VIPprintf("[R]Table[%4d]=0x%08x\n", i, pTbl[i]);
		}
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow

		// setup table access setting
		valc_valc_lut_addr_reg.regValue = IoReg_Read32(VALC_VALC_LUT_ADDR_reg);
		valc_valc_lut_addr_reg.valc_lut_sel = ChannelSel;
		valc_valc_lut_addr_reg.valc_tbl_sel = _VALC_TABLE_NORMAL;
		valc_valc_lut_addr_reg.valc_lut_ax_en = 1;
		IoReg_Write32(VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

		for(i=0; i<VALC_TBL_LEN; i++)
		{
			valc_valc_lut_data_reg.regValue = IoReg_Read32(VALC_VALC_LUT_DATA_reg);
			pTbl[i] = valc_valc_lut_data_reg.valc_lut_data;
			VIPprintf("[R]Table[%4d]=0x%08x\n", i, pTbl[i]);
		}
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		// merlin8p IC , run merlin8p flow
		if(ChannelSel <= _VALC_COLOR_B){
			// setup table access setting
			valc_valc_lut_addr_reg.regValue = IoReg_Read32(VALC_VALC_LUT_ADDR_reg);
			valc_valc_lut_addr_reg.valc_lut_sel = ChannelSel;
			valc_valc_lut_addr_reg.valc_tbl_sel = _VALC_TABLE_NORMAL;
			valc_valc_lut_addr_reg.valc_lut_ax_en = 1;
			IoReg_Write32(VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

			for(i=0; i<VALC_TBL_LEN; i++)
			{
				valc_valc_lut_data_reg.regValue = IoReg_Read32(VALC_VALC_LUT_DATA_reg);
				pTbl[i] = valc_valc_lut_data_reg.valc_lut_data;
				VIPprintf("[R]Table[%4d]=0x%08x\n", i, pTbl[i]);
			}
		}else{
			// Since the real channel is from 0 to 2
			// But we need to distinguish multi-curve mode on and off
			// So need to minus 3 to get the real channel
			ChannelSel = ChannelSel - _VALC_COLOR_R_MULTICURVE;
			drvif_color_get_specific_valc_table(pTbl, ChannelSel, _VALC_TABLE_NORMAL_MULTICURVE);
			drvif_color_get_specific_valc_table(pTbl, ChannelSel, _VALC_TABLE_STRONG_MULTICURVE);
			drvif_color_get_specific_valc_table(pTbl, ChannelSel, _VALC_TABLE_WEAK_MULTICURVE);
		}
			
	}

	// Disable the table access
	valc_valc_lut_addr_reg.regValue = IoReg_Read32(VALC_VALC_LUT_ADDR_reg);
	valc_valc_lut_addr_reg.valc_lut_ax_en = 0;
	IoReg_Write32(VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

	// restore the function enable
	valc_pcid_valc_ctrl_reg.regValue = IoReg_Read32(VALC_PCID_VALC_CTRL_reg);
	valc_pcid_valc_ctrl_reg.valc_func_en = bFuncEnable;
	IoReg_Write32(VALC_PCID_VALC_CTRL_reg, valc_pcid_valc_ctrl_reg.regValue);

}

void drvif_color_get_specific_valc_table(unsigned int* pTbl, DRV_valc_channel_t ChannelSel, DRV_valc_table_t TableSel){
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		int i=0, arr_start=0, arr_len=0;
		unsigned char bFuncEnable = 0;
		M8P_valc_pcid_valc_ctrl_RBUS valc_pcid_valc_ctrl_reg;
		M8P_valc_valc_lut_addr_RBUS  valc_valc_lut_addr_reg;
		M8P_valc_valc_lut_data_RBUS  valc_valc_lut_data_reg;
		if(ChannelSel >= _VALC_COLOR_ERR){
			VIPprintf("[Error][%s][%s] ChannelSel is invalid!\n", __FILE__, __func__);
			return;
		}

		if(!pTbl)
		{
			VIPprintf("[Error][%s][%s] Table Ptr is null!\n", __FILE__, __func__);
			return;
		}

		if(ChannelSel > _VALC_COLOR_B){
			ChannelSel = ChannelSel - _VALC_COLOR_R_MULTICURVE;
		}
		// setup table access setting
		valc_valc_lut_addr_reg.regValue = IoReg_Read32(M8P_VALC_VALC_LUT_ADDR_reg);
		valc_valc_lut_addr_reg.valc_lut_sel = ChannelSel;
		valc_valc_lut_addr_reg.valc_tbl_sel = TableSel;
		valc_valc_lut_addr_reg.valc_lut_ax_en = 1;
		IoReg_Write32(M8P_VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

		switch (TableSel)
		{
			case _VALC_TABLE_NORMAL:
				arr_start = 0;
				arr_len = VALC_TBL_LEN;
				break;
			case _VALC_TABLE_NORMAL_MULTICURVE:
				arr_start = 0;
				arr_len = VALC_TBL_NORMAL_LEN_MULTICURVE;
				break;
			case _VALC_TABLE_STRONG_MULTICURVE:
				arr_start = VALC_TBL_STRONG_START;
				arr_len = VALC_TBL_STRONG_LEN_MULTICURVE;
				break;
			case _VALC_TABLE_WEAK_MULTICURVE:
				arr_start = VALC_TBL_WEAK_START;
				arr_len = VALC_TBL_WEAK_LEN_MULTICURVE;
				break;
			default:
				VIPprintf("[Error][%s][%s] TableSel is invalid!\n", __FILE__, __func__);
				return;
		}

		for(i=0; i < arr_len; i++)
		{
			valc_valc_lut_data_reg.regValue = IoReg_Read32(M8P_VALC_VALC_LUT_DATA_reg);
			pTbl[arr_start+i] = valc_valc_lut_data_reg.valc_lut_data;
			VIPprintf("[R]Table[%4d]=0x%08x\n", i, pTbl[i]);
		}

		// Disable the table access
		valc_valc_lut_addr_reg.regValue = IoReg_Read32(M8P_VALC_VALC_LUT_ADDR_reg);
		valc_valc_lut_addr_reg.valc_lut_ax_en = 0;
		IoReg_Write32(M8P_VALC_VALC_LUT_ADDR_reg, valc_valc_lut_addr_reg.regValue);

		// restore the function enable
		valc_pcid_valc_ctrl_reg.regValue = IoReg_Read32(VALC_PCID_VALC_CTRL_reg);
		valc_pcid_valc_ctrl_reg.valc_func_en = bFuncEnable;
		IoReg_Write32(M8P_VALC_PCID_VALC_CTRL_reg, valc_pcid_valc_ctrl_reg.regValue);
	}
}


void drvif_color_set_valc_location(unsigned char loctioan)
{
	valc_pcid_valc_ctrl_RBUS valc_pcid_valc_ctrl_reg;
	valc_pcid_valc_ctrl_reg.regValue = IoReg_Read32(VALC_PCID_VALC_CTRL_reg);
	if(loctioan >= 2) loctioan=2;
	valc_pcid_valc_ctrl_reg.valc_location = loctioan;

	IoReg_Write32(VALC_PCID_VALC_CTRL_reg, valc_pcid_valc_ctrl_reg.regValue);
}

unsigned char driver_color_get_DTG_valc_enable(void)
{
	ppoverlay_display_timing_ctrl2_RBUS display_timing_ctrl2_reg;
	display_timing_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL2_reg);

	return display_timing_ctrl2_reg.valc_en;
}

unsigned char driver_color_get_valc_enable(void)
{
	valc_pcid_valc_ctrl_RBUS valc_pcid_valc_ctrl_reg;
	valc_pcid_valc_ctrl_reg.regValue = IoReg_Read32(VALC_PCID_VALC_CTRL_reg);
	if( valc_pcid_valc_ctrl_reg.valc_func_en != 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void drvif_color_set_valc_window_mode_enable(unsigned char bEnable)
{
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		// merlin8p IC , run merlin8p flow
		M8P_valc_valc_db_RBUS valc_valc_db_reg;
		M8P_valc_valc_window_mode_ctrl_RBUS valc_valc_window_mode_ctrl_reg;
		unsigned int timeoutcnt= 100;
		
		valc_valc_db_reg.regValue = IoReg_Read32(M8P_VALC_VALC_DB_reg);
		valc_valc_db_reg.db_en= 1;
		IoReg_Write32(M8P_VALC_VALC_DB_reg, valc_valc_db_reg.regValue);

		valc_valc_window_mode_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_WINDOW_MODE_CTRL_reg);
		valc_valc_window_mode_ctrl_reg.valc_window_en= bEnable;
		IoReg_Write32(M8P_VALC_VALC_WINDOW_MODE_CTRL_reg, valc_valc_window_mode_ctrl_reg.regValue);

		valc_valc_db_reg.regValue = IoReg_Read32(M8P_VALC_VALC_DB_reg);
		valc_valc_db_reg.db_apply= 1;
		IoReg_Write32(M8P_VALC_VALC_DB_reg, valc_valc_db_reg.regValue);

		timeoutcnt = 100;
		while ((IoReg_Read32(M8P_VALC_VALC_DB_reg) & M8P_VALC_VALC_DB_db_apply_mask) && timeoutcnt > 10) {
			timeoutcnt--;
			msleep_interruptible(10);
		}
	}
}

void drvif_color_set_valc_window_mode_smooth_enable(unsigned char bEnable)
{
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		// merlin8p IC , run merlin8p flow
		M8P_valc_valc_db_RBUS valc_valc_db_reg;
		M8P_valc_valc_window_mode_ctrl_RBUS valc_valc_window_mode_ctrl_reg;
		unsigned int timeoutcnt= 100;
		
		valc_valc_db_reg.regValue = IoReg_Read32(M8P_VALC_VALC_DB_reg);
		valc_valc_db_reg.db_en= 1;
		IoReg_Write32(M8P_VALC_VALC_DB_reg, valc_valc_db_reg.regValue);

		valc_valc_window_mode_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_WINDOW_MODE_CTRL_reg);
		valc_valc_window_mode_ctrl_reg.valc_window_smooth_en = bEnable;
		IoReg_Write32(M8P_VALC_VALC_WINDOW_MODE_CTRL_reg, valc_valc_window_mode_ctrl_reg.regValue);

		valc_valc_db_reg.regValue = IoReg_Read32(M8P_VALC_VALC_DB_reg);
		valc_valc_db_reg.db_apply= 1;
		IoReg_Write32(M8P_VALC_VALC_DB_reg, valc_valc_db_reg.regValue);

		timeoutcnt = 100;
		while ((IoReg_Read32(M8P_VALC_VALC_DB_reg) & M8P_VALC_VALC_DB_db_apply_mask) && timeoutcnt > 10) {
			timeoutcnt--;
			msleep_interruptible(10);
		}
	}
}

void drvif_color_set_valc_window_mode_setting(DRV_valc_win_ctrl_t* VALC_Win_Ctrl)
{	
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		M8P_valc_valc_window_mode_ctrl_RBUS valc_valc_window_mode_ctrl_reg;
		M8P_valc_valc_window_mode_ctrl_2_RBUS valc_valc_window_mode_ctrl_2_reg;

		valc_valc_window_mode_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_WINDOW_MODE_CTRL_reg);
		valc_valc_window_mode_ctrl_2_reg.regValue = IoReg_Read32(M8P_VALC_VALC_WINDOW_MODE_CTRL_2_reg);

		valc_valc_window_mode_ctrl_reg.valc_window_startx = VALC_Win_Ctrl->Boundary.valcWindowStartX;
		valc_valc_window_mode_ctrl_reg.valc_window_starty = VALC_Win_Ctrl->Boundary.valcWindowStartY;
		IoReg_Write32(M8P_VALC_VALC_WINDOW_MODE_CTRL_reg, valc_valc_window_mode_ctrl_reg.regValue);

		valc_valc_window_mode_ctrl_2_reg.valc_window_height = VALC_Win_Ctrl->Boundary.valcWindowHeight;
		valc_valc_window_mode_ctrl_2_reg.valc_window_width = VALC_Win_Ctrl->Boundary.valcWindowWidth;
		IoReg_Write32(M8P_VALC_VALC_WINDOW_MODE_CTRL_2_reg, valc_valc_window_mode_ctrl_reg.regValue);	

		drvif_color_set_valc_window_mode_enable(VALC_Win_Ctrl->enable);
	}
}

void drvif_color_set_valc_window_mode_smooth_weight(unsigned char* pTbl){
	M8P_valc_valc_window_mode_ctrl_3_RBUS valc_valc_window_mode_ctrl_3_reg;

	valc_valc_window_mode_ctrl_3_reg.regValue = IoReg_Read32(M8P_VALC_VALC_WINDOW_MODE_CTRL_3_reg);
	
	valc_valc_window_mode_ctrl_3_reg.valc_window_smooth_weight0 = pTbl[0];
	valc_valc_window_mode_ctrl_3_reg.valc_window_smooth_weight1 = pTbl[1];
	valc_valc_window_mode_ctrl_3_reg.valc_window_smooth_weight2 = pTbl[2];
	valc_valc_window_mode_ctrl_3_reg.valc_window_smooth_weight3 = pTbl[3];
	IoReg_Write32(M8P_VALC_VALC_WINDOW_MODE_CTRL_3_reg, valc_valc_window_mode_ctrl_3_reg.regValue);
}

void drvif_color_set_valc_multi_scene_global_enable(unsigned char multi_en, unsigned char overlap_en, unsigned char window_out_en)
{
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		M8P_valc_valc_db_RBUS valc_valc_db_reg;
		M8P_valc_valc_multi_scene_global_ctrl_RBUS valc_valc_multi_scene_global_ctrl_reg;
		unsigned int timeoutcnt = 100;
		
		valc_valc_db_reg.regValue = IoReg_Read32(M8P_VALC_VALC_DB_reg);
		valc_valc_db_reg.db_en = 1;
		IoReg_Write32(M8P_VALC_VALC_DB_reg, valc_valc_db_reg.regValue);

		valc_valc_multi_scene_global_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE_GLOBAL_CTRL_reg);
		valc_valc_multi_scene_global_ctrl_reg.valc_multi_scene_func_en = multi_en;
		valc_valc_multi_scene_global_ctrl_reg.valc_multi_scene_overlap_en = overlap_en;
		valc_valc_multi_scene_global_ctrl_reg.valc_multi_scene_out_en = window_out_en;
		IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE_GLOBAL_CTRL, valc_valc_multi_scene_global_ctrl_reg.regValue);

		valc_valc_db_reg.regValue = IoReg_Read32(M8P_VALC_VALC_DB_reg);
		valc_valc_db_reg.db_apply = 1;
		IoReg_Write32(M8P_VALC_VALC_DB_reg, valc_valc_db_reg.regValue);

		timeoutcnt = 100;
		while ((IoReg_Read32(M8P_VALC_VALC_DB_reg) & M8P_VALC_VALC_DB_db_apply_mask) && timeoutcnt > 10) {
			timeoutcnt--;
			msleep_interruptible(10);
		}
	}
}

void drvif_color_set_valc_multi_scene_inner_enable(unsigned char bEnable, unsigned char scene_sel)
{
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		M8P_valc_valc_db_RBUS valc_valc_db_reg;
		unsigned int timeoutcnt= 100;
		
		valc_valc_db_reg.regValue = IoReg_Read32(M8P_VALC_VALC_DB_reg);
		valc_valc_db_reg.db_en= 1;
		IoReg_Write32(M8P_VALC_VALC_DB_reg, valc_valc_db_reg.regValue);

		if(scene_sel == 1)
		{	
			M8P_valc_valc_multi_scene1_ctrl_RBUS valc_valc_multi_scene1_ctrl_reg;
			valc_valc_multi_scene1_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE1_CTRL_reg);
			valc_valc_multi_scene1_ctrl_reg.valc_multi_scene1_en= bEnable;
			IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE1_CTRL_reg, valc_valc_multi_scene1_ctrl_reg.regValue);
		}
		else if(scene_sel == 2)
		{
			M8P_valc_valc_multi_scene2_ctrl_RBUS valc_valc_multi_scene2_ctrl_reg;
			valc_valc_multi_scene2_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE2_CTRL_reg);
			valc_valc_multi_scene2_ctrl_reg.valc_multi_scene2_en= bEnable;
			IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE2_CTRL_reg, valc_valc_multi_scene2_ctrl_reg.regValue);
		}
		else if(scene_sel == 3)
		{
			M8P_valc_valc_multi_scene3_ctrl_RBUS valc_valc_multi_scene3_ctrl_reg;
			valc_valc_multi_scene3_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE3_CTRL_reg);
			valc_valc_multi_scene3_ctrl_reg.valc_multi_scene3_en= bEnable;
			IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE3_CTRL_reg, valc_valc_multi_scene3_ctrl_reg.regValue);
		}
		else if(scene_sel == 4)
		{	
			M8P_valc_valc_multi_scene4_ctrl_RBUS valc_valc_multi_scene4_ctrl_reg;
			valc_valc_multi_scene4_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE4_CTRL_reg);
			valc_valc_multi_scene4_ctrl_reg.valc_multi_scene4_en= bEnable;
			IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE4_CTRL_reg, valc_valc_multi_scene4_ctrl_reg.regValue);
		}
		else
		{
			rtd_pr_vpq_emerg("drvif_color_set_valc_multi_scene_inner_enable, scene_sel error\n");
		}

		valc_valc_db_reg.regValue = IoReg_Read32(M8P_VALC_VALC_DB_reg);
		valc_valc_db_reg.db_apply= 1;
		IoReg_Write32(M8P_VALC_VALC_DB_reg, valc_valc_db_reg.regValue);

		timeoutcnt = 100;
		while ((IoReg_Read32(M8P_VALC_VALC_DB_reg) & M8P_VALC_VALC_DB_db_apply_mask) && timeoutcnt > 10) {
			timeoutcnt--;
			msleep_interruptible(10);
		}
	}
}

void drvif_color_set_valc_multi_scene_inner_setting(DRV_valc_multi_scene_n_ctrl_t* VALC_Muti_Scene_Ctrl, unsigned char scene_sel)
{
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		if(scene_sel == 1){

			M8P_valc_valc_multi_scene1_ctrl_RBUS valc_valc_multi_scene1_ctrl_reg;
			M8P_valc_valc_multi_scene1_ctrl_2_RBUS valc_valc_multi_scene1_ctrl_2_reg;

			valc_valc_multi_scene1_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE1_CTRL_reg);
			valc_valc_multi_scene1_ctrl_2_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE1_CTRL_2_reg);
			
			valc_valc_multi_scene1_ctrl_reg.valc_multi_scene1_smooth_en = VALC_Muti_Scene_Ctrl->valcSceneSmoothEnable;
			valc_valc_multi_scene1_ctrl_reg.valc_multi_scene1_smooth_direction = VALC_Muti_Scene_Ctrl->valcSceneSmoothDirection;
			valc_valc_multi_scene1_ctrl_reg.valc_multi_scene1_startx= VALC_Muti_Scene_Ctrl->valcSceneStartX;
			valc_valc_multi_scene1_ctrl_reg.valc_multi_scene1_starty= VALC_Muti_Scene_Ctrl->valcSceneStartY;
			IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE1_CTRL_reg, valc_valc_multi_scene1_ctrl_reg.regValue);

			valc_valc_multi_scene1_ctrl_2_reg.valc_multi_scene1_height= VALC_Muti_Scene_Ctrl->valcSceneHeight;
			valc_valc_multi_scene1_ctrl_2_reg.valc_multi_scene1_width= VALC_Muti_Scene_Ctrl->valcSceneWidth;
			IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE1_CTRL_2_reg, valc_valc_multi_scene1_ctrl_2_reg.regValue);	

			drvif_color_set_valc_multi_scene_inner_enable(VALC_Muti_Scene_Ctrl->valcSceneEnable, 1);

		}
		else if(scene_sel == 2){

			M8P_valc_valc_multi_scene2_ctrl_RBUS valc_valc_multi_scene2_ctrl_reg;
			M8P_valc_valc_multi_scene2_ctrl_2_RBUS valc_valc_multi_scene2_ctrl_2_reg;

			valc_valc_multi_scene2_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE2_CTRL_reg);
			valc_valc_multi_scene2_ctrl_2_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE2_CTRL_2_reg);
			
			valc_valc_multi_scene2_ctrl_reg.valc_multi_scene2_smooth_en = VALC_Muti_Scene_Ctrl->valcSceneSmoothEnable;
			valc_valc_multi_scene2_ctrl_reg.valc_multi_scene2_smooth_direction = VALC_Muti_Scene_Ctrl->valcSceneSmoothDirection;
			valc_valc_multi_scene2_ctrl_reg.valc_multi_scene2_startx= VALC_Muti_Scene_Ctrl->valcSceneStartX;
			valc_valc_multi_scene2_ctrl_reg.valc_multi_scene2_starty= VALC_Muti_Scene_Ctrl->valcSceneStartY;
			IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE2_CTRL_reg, valc_valc_multi_scene2_ctrl_reg.regValue);

			valc_valc_multi_scene2_ctrl_2_reg.valc_multi_scene2_height= VALC_Muti_Scene_Ctrl->valcSceneHeight;
			valc_valc_multi_scene2_ctrl_2_reg.valc_multi_scene2_width= VALC_Muti_Scene_Ctrl->valcSceneWidth;
			IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE2_CTRL_2_reg, valc_valc_multi_scene2_ctrl_2_reg.regValue);

		}else if(scene_sel == 3){

			M8P_valc_valc_multi_scene3_ctrl_RBUS valc_valc_multi_scene3_ctrl_reg;
			M8P_valc_valc_multi_scene3_ctrl_2_RBUS valc_valc_multi_scene3_ctrl_2_reg;

			valc_valc_multi_scene3_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE3_CTRL_reg);
			valc_valc_multi_scene3_ctrl_2_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE3_CTRL_2_reg);
			
			valc_valc_multi_scene3_ctrl_reg.valc_multi_scene3_smooth_en = VALC_Muti_Scene_Ctrl->valcSceneSmoothEnable;
			valc_valc_multi_scene3_ctrl_reg.valc_multi_scene3_smooth_direction = VALC_Muti_Scene_Ctrl->valcSceneSmoothDirection;
			valc_valc_multi_scene3_ctrl_reg.valc_multi_scene3_startx= VALC_Muti_Scene_Ctrl->valcSceneStartX;
			valc_valc_multi_scene3_ctrl_reg.valc_multi_scene3_starty= VALC_Muti_Scene_Ctrl->valcSceneStartY;
			IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE3_CTRL_reg, valc_valc_multi_scene3_ctrl_reg.regValue);

			valc_valc_multi_scene3_ctrl_2_reg.valc_multi_scene3_height= VALC_Muti_Scene_Ctrl->valcSceneHeight;
			valc_valc_multi_scene3_ctrl_2_reg.valc_multi_scene3_width= VALC_Muti_Scene_Ctrl->valcSceneWidth;
			IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE3_CTRL_2_reg, valc_valc_multi_scene3_ctrl_2_reg.regValue);

		}else if(scene_sel == 4){

			M8P_valc_valc_multi_scene4_ctrl_RBUS valc_valc_multi_scene4_ctrl_reg;
			M8P_valc_valc_multi_scene4_ctrl_2_RBUS valc_valc_multi_scene4_ctrl_2_reg;

			valc_valc_multi_scene4_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE4_CTRL_reg);
			valc_valc_multi_scene4_ctrl_2_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_SCENE4_CTRL_2_reg);
			
			valc_valc_multi_scene4_ctrl_reg.valc_multi_scene4_smooth_en = VALC_Muti_Scene_Ctrl->valcSceneSmoothEnable;
			valc_valc_multi_scene4_ctrl_reg.valc_multi_scene4_smooth_direction = VALC_Muti_Scene_Ctrl->valcSceneSmoothDirection;
			valc_valc_multi_scene4_ctrl_reg.valc_multi_scene4_startx= VALC_Muti_Scene_Ctrl->valcSceneStartX;
			valc_valc_multi_scene4_ctrl_reg.valc_multi_scene4_starty= VALC_Muti_Scene_Ctrl->valcSceneStartY;
			IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE4_CTRL_reg, valc_valc_multi_scene4_ctrl_reg.regValue);

			valc_valc_multi_scene4_ctrl_2_reg.valc_multi_scene4_height= VALC_Muti_Scene_Ctrl->valcSceneHeight;
			valc_valc_multi_scene4_ctrl_2_reg.valc_multi_scene4_width= VALC_Muti_Scene_Ctrl->valcSceneWidth;
			IoReg_Write32(M8P_VALC_VALC_MULTI_SCENE4_CTRL_2_reg, valc_valc_multi_scene4_ctrl_2_reg.regValue);
			
		}else{
			rtd_pr_vpq_emerg("drvif_color_set_valc_multi_scene_inner_setting, scene_sel error\n");
		}
	}
}

void drvif_color_set_valc_multi_scene_all_inner_setting(DRV_valc_multi_scene_ctrl_t* VALC_Muti_Scene_Ctrl)
{	
	int i;
	drvif_color_set_valc_multi_scene_global_enable(VALC_Muti_Scene_Ctrl->valcMultiSceneFuncEnable, 
												   VALC_Muti_Scene_Ctrl->valcMultiSceneOverlapEnable, 
												   VALC_Muti_Scene_Ctrl->valcMultiSceneOutEnable);
	for(i=1; i<=4; i++)
	{
		drvif_color_set_valc_multi_scene_inner_setting(&(VALC_Muti_Scene_Ctrl->valcMultiSceneN[i-1]), i);
	}
}

void drvif_color_set_valc_curve_mode_enable(unsigned char multi_curve_en, unsigned char multi_curve_sat_en)
{
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		M8P_valc_valc_db_RBUS valc_valc_db_reg;
		M8P_valc_valc_multi_curve_ctrl_RBUS valc_valc_multi_curve_ctrl_reg;
		unsigned int timeoutcnt= 100;
		
		valc_valc_db_reg.regValue = IoReg_Read32(M8P_VALC_VALC_DB_reg);
		valc_valc_db_reg.db_en= 1;
		IoReg_Write32(M8P_VALC_VALC_DB_reg, valc_valc_db_reg.regValue);

		valc_valc_multi_curve_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_CURVE_CTRL_reg);
		valc_valc_multi_curve_ctrl_reg.valc_multi_curve_en= multi_curve_en;
		valc_valc_multi_curve_ctrl_reg.valc_multi_curve_sat_en= multi_curve_sat_en;
		IoReg_Write32(M8P_VALC_VALC_MULTI_CURVE_CTRL_reg, valc_valc_multi_curve_ctrl_reg.regValue);

		valc_valc_db_reg.regValue = IoReg_Read32(M8P_VALC_VALC_DB_reg);
		valc_valc_db_reg.db_apply= 1;
		IoReg_Write32(M8P_VALC_VALC_DB_reg, valc_valc_db_reg.regValue);

		timeoutcnt = 100;
		while ((IoReg_Read32(M8P_VALC_VALC_DB_reg) & M8P_VALC_VALC_DB_db_apply_mask) && timeoutcnt > 10) {
			timeoutcnt--;
			msleep_interruptible(10);
		}
	}
}

void drvif_color_set_valc_curve_mode_setting(DRV_valc_multi_curve_ctrl_t* VALC_Multi_Curve_Ctrl){
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
		VIPprintf("Merlin8 Flow, not support this function\n");
		return;
	}else if(get_mach_type()==MACH_ARCH_RTK2885PP){
		M8P_valc_valc_multi_curve_ctrl_RBUS valc_valc_multi_curve_ctrl_reg;
		M8P_valc_valc_multi_curve_ctrl_2_RBUS valc_valc_multi_curve_ctrl_2_reg;

		valc_valc_multi_curve_ctrl_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_CURVE_CTRL_reg);
		valc_valc_multi_curve_ctrl_2_reg.regValue = IoReg_Read32(M8P_VALC_VALC_MULTI_CURVE_CTRL_2_reg);

		valc_valc_multi_curve_ctrl_reg.valc_multi_curve_ttp_len = VALC_Multi_Curve_Ctrl->valcMultiCurve_ttp_len;
		valc_valc_multi_curve_ctrl_reg.valc_multi_curve_ttp = VALC_Multi_Curve_Ctrl->valcMultiCurve_ttp;
		IoReg_Write32(M8P_VALC_VALC_MULTI_CURVE_CTRL_reg, valc_valc_multi_curve_ctrl_reg.regValue);

		valc_valc_multi_curve_ctrl_2_reg.valc_multi_curve_b_low_l  = VALC_Multi_Curve_Ctrl->valcMultiCurve_b_low_L;
		valc_valc_multi_curve_ctrl_2_reg.valc_multi_curve_b_low_r  = VALC_Multi_Curve_Ctrl->valcMultiCurve_b_low_R;
		valc_valc_multi_curve_ctrl_2_reg.valc_multi_curve_b_high_l = VALC_Multi_Curve_Ctrl->valcMultiCurve_b_high_L;
		valc_valc_multi_curve_ctrl_2_reg.valc_multi_curve_b_high_r = VALC_Multi_Curve_Ctrl->valcMultiCurve_b_high_R;
		valc_valc_multi_curve_ctrl_2_reg.valc_multi_curve_s_low_l  = VALC_Multi_Curve_Ctrl->valcMultiCurve_s_low_L;
		valc_valc_multi_curve_ctrl_2_reg.valc_multi_curve_s_low_r  = VALC_Multi_Curve_Ctrl->valcMultiCurve_s_low_R;
		valc_valc_multi_curve_ctrl_2_reg.valc_multi_curve_s_high_l = VALC_Multi_Curve_Ctrl->valcMultiCurve_s_high_L;
		valc_valc_multi_curve_ctrl_2_reg.valc_multi_curve_s_high_r = VALC_Multi_Curve_Ctrl->valcMultiCurve_s_high_R;
		valc_valc_multi_curve_ctrl_2_reg.valc_multi_curve_bm       = VALC_Multi_Curve_Ctrl->valcMultiCurve_bm;
		valc_valc_multi_curve_ctrl_2_reg.valc_multi_curve_cm       = VALC_Multi_Curve_Ctrl->valcMultiCurve_cm;
		valc_valc_multi_curve_ctrl_2_reg.valc_multi_curve_sm       = VALC_Multi_Curve_Ctrl->valcMultiCurve_sm;
		IoReg_Write32(M8P_VALC_VALC_MULTI_CURVE_CTRL_2_reg, valc_valc_multi_curve_ctrl_2_reg.regValue);
	}
}
