/*==========================================================================
    * Copyright (c)      Realtek Semiconductor Corporation, 2020
  * All rights reserved.
  * ========================================================================*/

/*============================ Module dependency  ===========================*/
#ifndef BUILD_QUICK_SHOW
#include <linux/string.h>
#include <linux/hrtimer.h>
#include <linux/semaphore.h>
#include <asm/cacheflush.h>
#include <linux/version.h>
#else
#include <include/string.h>
#include <timer.h>
#include <no_os/semaphore.h>
#endif

#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
#else
	#include <scalercommon/scalerCommon.h>
#endif

#include <tvscalercontrol/io/ioregdrv.h>
#include "rtk_vip_logger.h"
//#include <mach/io.h>

#include <rbus/srnn_reg.h>
#include <rbus/srnn_3dlut_reg.h>
#include <rbus/srnn_pre_post_reg.h>
#include <rbus/M8P_srnn_pre_post_reg.h>
#include <rbus/scaleup_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/ppoverlay_reg.h>

#include <tvscalercontrol/vip/srnn.h>
#include <tvscalercontrol/vip/srnn_table.h>
#include <tvscalercontrol/vip/scalerPQMaskCtrl.h>
#include <tvscalercontrol/vip/scalerColor.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <tvscalercontrol/vip/ai_pq.h>
#include <rtk_kdriver/rtk_otp_region_api.h>
#include <tvscalercontrol/scalerdrv/scaler_nonlibdma.h>

#include <mach/rtk_platform.h>
/*================================ Definitions ==============================*/
#define TAG_NAME "SRNN"
//#define NUM_PARA 1324
//#define VPQ_COMPILER_ERROR_ENABLE
/*================================== Variables ==============================*/
//unsigned int NNSR_data[8500]={0};
//unsigned short p_out[35000]={0};

VIP_SRNN_ARCH_INFO gSrnnArchInfo = {0};
VIP_SRNN_TOP_INFO gSrnnTopInfo = {0};
//VIP_SRNN_DMA_CTRL gSrnnDmaCtrl = {0};
unsigned short gSrnnParamTmp[46000] = {0};
const unsigned char gBrustNumTbl[4][6] = { \
	/* mode2 */ {90, 91, 92, 93, 93, 92}, \
	/* mode3 */ {46, 46, 47, 48, 48, 47}, \
	/* mode4 */ {26, 26, 27, 28, 28, 27}, \
	/* mode5 */ {13, 14, 15, 16, 16, 15}, \
}; 
const unsigned char gSuModeSizeMapping[6] = {1, 2, 3, 4, 4, 3};
const unsigned char gCyclePerClkx2[4] = {72, 36, 18, 9};
const unsigned char gPreRdLine[4] = {11, 7, 7, 7};
const unsigned char gModelRequire[4] = {250, 250, 109, 40};
const unsigned short gSrnnPLL = 1228;		// 1228.5MHz
const unsigned int gSrnnPreRdDefault = 0x1570;

extern RTK_AI_PQ_mode aipq_mode;

/*
const VIP_SRNN_Hx2_TABLE gHx2DefaultTable = { \
		{0xff1, 0x0d1, 0x37c, 0xfc2}, \
		{0xfc3, 0x380, 0x0cd, 0xff0}, \
};
*/
/*================================== GENERAL FUNCTIONS ==============================*/

/**
 * @brief
 * set srnn clk en
 * @param En
 */
void SrnnCtrl_SetSRNNClkEn(unsigned char En){
	sys_reg_sys_clken4_RBUS sys_reg_sys_clken4_reg;

	sys_reg_sys_clken4_reg.regValue = 0;
	sys_reg_sys_clken4_reg.clken_srnn = 1; //operation this bit
	sys_reg_sys_clken4_reg.write_data = En; //clken_srnn enable or disable

	IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, sys_reg_sys_clken4_reg.regValue);
}

/**
 * @brief
 * set srnn srst en
 * @param En
 */
void SrnnCtrl_SetSRNNSRSTEn(unsigned char En){
	sys_reg_sys_srst4_RBUS sys_reg_sys_srst4_reg;

	sys_reg_sys_srst4_reg.regValue = 0;
	sys_reg_sys_srst4_reg.rstn_srnn = 1; //operation this bit
	sys_reg_sys_srst4_reg.write_data = En;
	
	IoReg_Write32(SYS_REG_SYS_SRST4_reg, sys_reg_sys_srst4_reg.regValue);
}

/**
 * @brief
 * set srnn ai_sr_line_delay
 */
void SrnnCtrl_SetSRNNAISRLineDelay(unsigned char line_delay, unsigned char isSkipDB){
	ppoverlay_double_buffer_ctrl2_RBUS ppoverlay_double_buffer_ctrl2_reg;
        ppoverlay_uzudtg_control3_RBUS ppoverlay_uzudtg_control3_reg;
		
	ppoverlay_uzudtg_control3_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control3_reg);
        ppoverlay_uzudtg_control3_reg.ai_sr_line_delay = line_delay; 

        IoReg_Write32(PPOVERLAY_uzudtg_control3_reg, ppoverlay_uzudtg_control3_reg.regValue);

	if (isSkipDB == 0) {
		/*UZU-dtg db apply */
		ppoverlay_double_buffer_ctrl2_reg.regValue= IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
		ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
	}
}

/**
 * @brief
 * set srnn clk fract
 * @param Fact_a
 * numerator
 * @param Fact_b
 * denominator
 */
void SrnnCtrl_SetSRNNClkFract(unsigned char En, unsigned char Fract_a, unsigned char Fract_b){
        sys_reg_sys_dispclk_fract_RBUS sys_reg_sys_dispclk_fract_reg;
		
	sys_reg_sys_dispclk_fract_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLK_FRACT_reg);
        sys_reg_sys_dispclk_fract_reg.srnn_fract_en = En; 
        sys_reg_sys_dispclk_fract_reg.srnn_fract_a = Fract_a;
	sys_reg_sys_dispclk_fract_reg.srnn_fract_b = Fract_b;

	//rtd_printk(KERN_EMERG, TAG_NAME," NNSR clk_fract_en =%d  \n", En);
        //rtd_printk(KERN_EMERG, TAG_NAME," NNSR clk_fract =%d / %d  \n", Fract_a,  Fract_b);
        
        IoReg_Write32(SYS_REG_SYS_DISPCLK_FRACT_reg, sys_reg_sys_dispclk_fract_reg.regValue);

		if (gSrnnTopInfo.ClkEn == 1)	// if srnn is en,  dtg clk is en
			IoReg_SetBits(PPOVERLAY_dispd_smooth_toggle1_reg, PPOVERLAY_dispd_smooth_toggle1_dispd_stage1_smooth_toggle_apply_mask);
}

/**
 * @brief
 * set srnn aisrmode
 * @param mode
 */
void SrnnCtrl_SetSRNNAISRMode(unsigned char mode, unsigned char isSkipDB){
	ppoverlay_double_buffer_ctrl2_RBUS ppoverlay_double_buffer_ctrl2_reg;
        ppoverlay_uzudtg_control3_RBUS ppoverlay_uzudtg_control3_reg;
		
	ppoverlay_uzudtg_control3_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control3_reg);
        ppoverlay_uzudtg_control3_reg.ai_sr_mode = mode; 

        IoReg_Write32(PPOVERLAY_uzudtg_control3_reg, ppoverlay_uzudtg_control3_reg.regValue);

	if (isSkipDB == 0) {
		/*UZU-dtg db apply */
		ppoverlay_double_buffer_ctrl2_reg.regValue= IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
		ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
	}
}

/**
 * @brief
 * get srnn aisrmode
 * @param mode
 */
unsigned char SrnnCtrl_GetSRNNAISRMode(void){
	unsigned char mode;
        ppoverlay_uzudtg_control3_RBUS ppoverlay_uzudtg_control3_reg;
		
	ppoverlay_uzudtg_control3_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control3_reg);
        mode = ppoverlay_uzudtg_control3_reg.ai_sr_mode; 

	return mode;
}

/**
 * @brief
 * double buffer en for srnn
 */
void SrnnCtrl_SetDoubleBufferEn(void){
	//SRNN_srnn_db_reg_ctl_reg 
	//SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg
	srnn_srnn_db_reg_ctl_RBUS  srnn_srnn_db_reg_ctl_reg;
	srnn_pre_post_srnn_pre_pos_db_reg_ctl_RBUS  srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg;

	if (gSrnnTopInfo.ClkEn == 1){		// check aisr mode first, avoid rbus timeout
		srnn_srnn_db_reg_ctl_reg.regValue = IoReg_Read32(SRNN_srnn_db_reg_ctl_reg);
		srnn_srnn_db_reg_ctl_reg.db_en = 1;
		IoReg_Write32(SRNN_srnn_db_reg_ctl_reg, srnn_srnn_db_reg_ctl_reg.regValue);
	}
	
	srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg);
	srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg.db_en = 1;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg, srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg.regValue);

}

/**
 * @brief
 * double buffer apply for srnn
 */
void SrnnCtrl_SetDoubleBufferApply(void){
	//SRNN_srnn_db_reg_ctl_reg 
	//SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg
	srnn_srnn_db_reg_ctl_RBUS  srnn_srnn_db_reg_ctl_reg;
	srnn_pre_post_srnn_pre_pos_db_reg_ctl_RBUS  srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg;

	if (gSrnnTopInfo.ClkEn == 1){		// check srnn clk first , avoid rbus timeout
		srnn_srnn_db_reg_ctl_reg.regValue = IoReg_Read32(SRNN_srnn_db_reg_ctl_reg);
		srnn_srnn_db_reg_ctl_reg.db_apply = 1;
		IoReg_Write32(SRNN_srnn_db_reg_ctl_reg, srnn_srnn_db_reg_ctl_reg.regValue);
	}
	
	srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg);
	srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg.db_apply = 1;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg, srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg.regValue);

}

/**
 * @brief
 * set srnn_pre_rd_start
 */
void SrnnCtrl_SetSRNNPreRdStart(unsigned int pre_rd_sta, unsigned char isSkipDB){
	srnn_pre_post_srnn_pre_pos_timing_ctrl_RBUS srnn_pre_post_srnn_pre_pos_timing_ctrl_reg;

	srnn_pre_post_srnn_pre_pos_timing_ctrl_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_TIMING_CTRL_reg);
        srnn_pre_post_srnn_pre_pos_timing_ctrl_reg.srnn_pre_rd_start = pre_rd_sta; 

        IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_TIMING_CTRL_reg, srnn_pre_post_srnn_pre_pos_timing_ctrl_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Set srnn tail scale up mode
 * @param SclaeMode
 * scale up mode => 0:1x, 1:2x, 2:3x, 3:4x, 4:1.33x, 5:1.5x
 */
void SrnnCtrl_SetSRNNScaleMode(unsigned char SclaeMode, unsigned char isSkipDB){
	srnn_srnn_su_ratio_sel_RBUS  srnn_srnn_su_ratio_sel_reg;

	if (gSrnnTopInfo.ClkEn == 1){
		srnn_srnn_su_ratio_sel_reg.regValue = IoReg_Read32(SRNN_SRNN_SU_RATIO_SEL_reg);
		srnn_srnn_su_ratio_sel_reg.nnsr_scaleup_ratio_select = SclaeMode;
		IoReg_Write32(SRNN_SRNN_SU_RATIO_SEL_reg, srnn_srnn_su_ratio_sel_reg.regValue);
	}
	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Get srnn tail scale up mode
 * scale up mode => 0:1x, 1:2x, 2:3x, 3:4x, 4:1.33x, 5:1.5x
 */
unsigned char SrnnCtrl_GetSRNNScaleMode(void){
	unsigned char su_mode = 0;
	srnn_srnn_su_ratio_sel_RBUS  srnn_srnn_su_ratio_sel_reg;
	if (gSrnnTopInfo.ClkEn == 1){
		srnn_srnn_su_ratio_sel_reg.regValue = IoReg_Read32(SRNN_SRNN_SU_RATIO_SEL_reg);
		su_mode = srnn_srnn_su_ratio_sel_reg.nnsr_scaleup_ratio_select;
	}
	return su_mode;
}


/**
 * @brief
 * Set srnn input size
 * @param V
 * input height
 * @param H
 * input width
 */
void SrnnCtrl_SetSRNNInputSize(unsigned short V, unsigned short H, unsigned char isSkipDB){
	srnn_srnn_input_size_RBUS  srnn_srnn_input_size_reg;
	if (gSrnnTopInfo.ClkEn == 1){
		srnn_srnn_input_size_reg.regValue = IoReg_Read32(SRNN_SRNN_INPUT_SIZE_reg);
		srnn_srnn_input_size_reg.srnn_hor_input_size = H;
		srnn_srnn_input_size_reg.srnn_ver_input_size = V;
		IoReg_Write32(SRNN_SRNN_INPUT_SIZE_reg, srnn_srnn_input_size_reg.regValue);
	}
	
	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Set srnn csc in mode
 * @param mode
 * selected mode [0(YCbCr)/2(Y)]
 */
void SrnnCtrl_SetCSCInMode(unsigned char mode, unsigned char isSkipDB){
	srnn_srnn_color_mode_RBUS  srnn_srnn_color_mode_reg;
	if (gSrnnTopInfo.ClkEn == 1){
		srnn_srnn_color_mode_reg.regValue = IoReg_Read32(SRNN_SRNN_COLOR_MODE_reg);
		srnn_srnn_color_mode_reg.color_input_mode = mode;
		IoReg_Write32(SRNN_SRNN_COLOR_MODE_reg, srnn_srnn_color_mode_reg.regValue);
	}

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Set srnn block disable
 */
void SrnnCtrl_SetBlockDisable(unsigned char* pDisable, unsigned char isSkipDB){
	srnn_srnn_block_disable_RBUS  srnn_srnn_block_disable_reg;
	if (gSrnnTopInfo.ClkEn == 1){
		srnn_srnn_block_disable_reg.regValue = IoReg_Read32(SRNN_SRNN_block_disable_reg);
		srnn_srnn_block_disable_reg.block_1_conv_disable = pDisable[0];
		srnn_srnn_block_disable_reg.block_2_conv_disable = pDisable[1];
		srnn_srnn_block_disable_reg.block_3_conv_disable = pDisable[2];
		srnn_srnn_block_disable_reg.block_4_conv_disable = pDisable[3];
		srnn_srnn_block_disable_reg.block_5_conv_disable = pDisable[4];
		srnn_srnn_block_disable_reg.block_6_conv_disable = pDisable[5];
		srnn_srnn_block_disable_reg.block_7_conv_disable = pDisable[6];
		srnn_srnn_block_disable_reg.block_8_conv_disable = pDisable[7];
		IoReg_Write32(SRNN_SRNN_block_disable_reg, srnn_srnn_block_disable_reg.regValue);
	}
	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Set srnn block feature reduction
 */
void SrnnCtrl_SetFeatureReduce(unsigned char* pReduce, unsigned char isSkipDB){
	srnn_srnn_feature_reduce_RBUS  srnn_srnn_feature_reduce_reg;
	if (gSrnnTopInfo.ClkEn == 1){
		srnn_srnn_feature_reduce_reg.regValue = IoReg_Read32(SRNN_SRNN_feature_reduce_reg);
		srnn_srnn_feature_reduce_reg.block_1_feature_reduce = pReduce[0];
		srnn_srnn_feature_reduce_reg.block_2_feature_reduce = pReduce[1];
		srnn_srnn_feature_reduce_reg.block_3_feature_reduce = pReduce[2];
		srnn_srnn_feature_reduce_reg.block_4_feature_reduce = pReduce[3];
		srnn_srnn_feature_reduce_reg.block_5_feature_reduce = pReduce[4];
		srnn_srnn_feature_reduce_reg.block_6_feature_reduce = pReduce[5];
		srnn_srnn_feature_reduce_reg.block_7_feature_reduce = pReduce[6];
		srnn_srnn_feature_reduce_reg.block_8_feature_reduce = pReduce[7];
		IoReg_Write32(SRNN_SRNN_feature_reduce_reg, srnn_srnn_feature_reduce_reg.regValue);
	}
	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/* ----- ----- ----- ----- ----- UZU FUNCTIONS ----- ----- ----- ----- ----- */
/**
 * @brief
 * Set 4 tap uzu/srnn blending mode
 * @param mode
 * 1 : UZU+NNSR; 2 : UZU only
 */
void SrnnCtrl_SetUZUSel(unsigned char mode, unsigned char isSkipDB){
	srnn_pre_post_srnn_pre_pos_uzu_sel_RBUS  srnn_pre_post_srnn_pre_pos_uzu_sel_reg;

	srnn_pre_post_srnn_pre_pos_uzu_sel_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_UZU_SEL_reg);
	srnn_pre_post_srnn_pre_pos_uzu_sel_reg.nnsr_uzu_select = mode;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_UZU_SEL_reg, srnn_pre_post_srnn_pre_pos_uzu_sel_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Set srnn data in, main path(0) or sub path(1)
 * @param mode
 */
void SrnnCtrl_SetDataInSel(unsigned char mode, unsigned char isSkipDB){
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		srnn_pre_post_srnn_pre_pos_data_in_sel_RBUS  srnn_pre_post_srnn_pre_pos_data_in_sel_reg;

		srnn_pre_post_srnn_pre_pos_data_in_sel_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_DATA_IN_SEL_reg);
		srnn_pre_post_srnn_pre_pos_data_in_sel_reg.srnn_data_in_sel = mode;
		IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_DATA_IN_SEL_reg, srnn_pre_post_srnn_pre_pos_data_in_sel_reg.regValue);

		if (isSkipDB == 0) {
			/*db apply */
			SrnnCtrl_SetDoubleBufferApply();
		}
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		M8P_srnn_pre_post_srnn_pre_pos_data_in_sel_RBUS  srnn_pre_post_srnn_pre_pos_data_in_sel_reg;

		srnn_pre_post_srnn_pre_pos_data_in_sel_reg.regValue = IoReg_Read32(M8P_SRNN_PRE_POST_SRNN_PRE_POS_DATA_IN_SEL_reg);
		srnn_pre_post_srnn_pre_pos_data_in_sel_reg.srnn_unshuffle = mode;
		srnn_pre_post_srnn_pre_pos_data_in_sel_reg.srnn_input_444_to_422 = mode;
		IoReg_Write32(M8P_SRNN_PRE_POST_SRNN_PRE_POS_DATA_IN_SEL_reg, srnn_pre_post_srnn_pre_pos_data_in_sel_reg.regValue);

		if (isSkipDB == 0) {
			/*db apply */
			SrnnCtrl_SetDoubleBufferApply();
		}
	}
	
}


/**
 * @brief
 * Set srnn hx2 en, must be same as reg_uzu_4k120_mode (DTG spec)
 * @param mode
 */
void SrnnCtrl_SetHx2En(unsigned char mode, unsigned char isSkipDB){
	srnn_pre_post_srnn_pre_pos_hx2_ctrl0_RBUS  srnn_pre_post_srnn_pre_pos_hx2_ctrl0_reg;

	srnn_pre_post_srnn_pre_pos_hx2_ctrl0_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_HX2_Ctrl0_reg);
	srnn_pre_post_srnn_pre_pos_hx2_ctrl0_reg.srnn_hx2_en = mode;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_HX2_Ctrl0_reg, srnn_pre_post_srnn_pre_pos_hx2_ctrl0_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Set srnn hx2 coeff
 */
void SrnnCtrl_SetHx2Coeff(unsigned short* pCoeff0, unsigned short* pCoeff1, unsigned char isSkipDB){
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph0_c0_RBUS  srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph0_c0_reg;
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph0_c2_RBUS  srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph0_c2_reg;
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph1_c0_RBUS  srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph1_c0_reg;
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph1_c2_RBUS  srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph1_c2_reg;
	
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph0_c0_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_HX2_FIR_Coef_Ph0_C0_reg);
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph0_c2_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_HX2_FIR_Coef_Ph0_C2_reg);
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph1_c0_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_HX2_FIR_Coef_Ph1_C0_reg);
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph1_c2_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_HX2_FIR_Coef_Ph1_C2_reg);

	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph0_c0_reg.srnn_hx2_ph0_c0 = pCoeff0[0];
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph0_c0_reg.srnn_hx2_ph0_c1 = pCoeff0[1];
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph0_c2_reg.srnn_hx2_ph0_c2 = pCoeff0[2];
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph0_c2_reg.srnn_hx2_ph0_c3 = pCoeff0[3];

	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph1_c0_reg.srnn_hx2_ph1_c0 = pCoeff1[0];
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph1_c0_reg.srnn_hx2_ph1_c1 = pCoeff1[1];
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph1_c2_reg.srnn_hx2_ph1_c2 = pCoeff1[2];
	srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph1_c2_reg.srnn_hx2_ph1_c3 = pCoeff1[3];

	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_HX2_FIR_Coef_Ph0_C0_reg, srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph0_c0_reg.regValue);
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_HX2_FIR_Coef_Ph0_C2_reg, srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph0_c2_reg.regValue);
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_HX2_FIR_Coef_Ph1_C0_reg, srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph1_c0_reg.regValue);
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_HX2_FIR_Coef_Ph1_C2_reg, srnn_pre_post_srnn_pre_pos_hx2_fir_coef_ph1_c2_reg.regValue);
	
	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/* ----- ----- ----- ----- ----- PQ RELATED FUNCTIONS ----- ----- ----- ----- ----- */
/**
 * @brief
 * Set LPF mode
 * @param mode 0:off, 2: 1x5
 */
void SrnnCtrl_SetLPFMode(unsigned char mode, unsigned char isSkipDB){
	srnn_pre_post_srnn_pre_pos_snr_sel_RBUS  srnn_pre_post_srnn_pre_pos_snr_sel_reg;

	srnn_pre_post_srnn_pre_pos_snr_sel_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_SNR_SEL_reg);
	srnn_pre_post_srnn_pre_pos_snr_sel_reg.snr_mode = mode;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_SNR_SEL_reg, srnn_pre_post_srnn_pre_pos_snr_sel_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Set LPF coeffs
 */
void SrnnCtrl_SetLPFCoeff(unsigned char* pCoeff, unsigned char isSkipDB){
	srnn_pre_post_srnn_pre_pos_snr_r0_coeff00_RBUS  srnn_pre_post_srnn_pre_pos_snr_r0_coeff00_reg;
	
	srnn_pre_post_srnn_pre_pos_snr_r0_coeff00_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_SNR_R0_coeff00_reg);
	srnn_pre_post_srnn_pre_pos_snr_r0_coeff00_reg.snr_r0_c0 = pCoeff[0];
	srnn_pre_post_srnn_pre_pos_snr_r0_coeff00_reg.snr_r0_c1 = pCoeff[1];
	srnn_pre_post_srnn_pre_pos_snr_r0_coeff00_reg.snr_r0_c2 = pCoeff[2];

	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_SNR_R0_coeff00_reg, srnn_pre_post_srnn_pre_pos_snr_r0_coeff00_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Set PSNR branch coring
 * @param Coring
 * coring val 4bits
 */
void SrnnCtrl_SetPSNRCoring(unsigned char Coring, unsigned char isSkipDB){
	srnn_pre_post_srnn_pre_pos_psnr_post_process_1_RBUS  srnn_pre_post_srnn_pre_pos_psnr_post_process_1_reg;

	srnn_pre_post_srnn_pre_pos_psnr_post_process_1_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_PSNR_post_process_1_reg);
	srnn_pre_post_srnn_pre_pos_psnr_post_process_1_reg.srnn_post_psnr_coring = Coring;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_PSNR_post_process_1_reg, srnn_pre_post_srnn_pre_pos_psnr_post_process_1_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Set PSNR branch pos/neg gain
 * @param GainPos
 * gain for positive vals 0+1+7
 * @param GainNeg
 * gain for negative vals 0+1+7
 */
void SrnnCtrl_SetPSNRGain(unsigned char GainPos, unsigned char GainNeg, unsigned char isSkipDB){
	srnn_pre_post_srnn_pre_pos_psnr_post_process_1_RBUS  srnn_pre_post_srnn_pre_pos_psnr_post_process_1_reg;

	srnn_pre_post_srnn_pre_pos_psnr_post_process_1_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_PSNR_post_process_1_reg);
	srnn_pre_post_srnn_pre_pos_psnr_post_process_1_reg.srnn_post_psnr_pos_gain = GainPos;
	srnn_pre_post_srnn_pre_pos_psnr_post_process_1_reg.srnn_post_psnr_neg_gain = GainNeg;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_PSNR_post_process_1_reg, srnn_pre_post_srnn_pre_pos_psnr_post_process_1_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Set PSNR branch pos/neg clamp
 * @param ClampPos
 * clamp for positive vals 1+1+9
 * @param ClampNeg
 * clamp for negative vals 1+1+9
 */
void SrnnCtrl_SetPSNRClamp(unsigned short ClampPos, unsigned short ClampNeg, unsigned char isSkipDB){
	srnn_pre_post_srnn_pre_pos_psnr_post_process_2_RBUS  srnn_pre_post_srnn_pre_pos_psnr_post_process_2_reg;

	srnn_pre_post_srnn_pre_pos_psnr_post_process_2_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_PSNR_post_process_2_reg);
	srnn_pre_post_srnn_pre_pos_psnr_post_process_2_reg.srnn_post_psnr_clamp_up = ClampPos;
	srnn_pre_post_srnn_pre_pos_psnr_post_process_2_reg.srnn_post_psnr_clamp_down = ClampNeg;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_PSNR_post_process_2_reg, srnn_pre_post_srnn_pre_pos_psnr_post_process_2_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/* ----- ----- ----- ----- ----- PQMASK RELATED FUNCTIONS ----- ----- ----- ----- ----- */
/**
 * @brief
 * Set PQMask in gain & offset
 * @param Gain
 * gain 0+1+7
 * @param Offset
 * offset 1+0+7
 */
void SrnnCtrl_SetPQMaskInGainOffset(unsigned char Gain, unsigned char Offset, unsigned char Pqmask_idx, unsigned char isSkipDB){
	srnn_pq_mask_pqin_gain_offset1_RBUS  srnn_pq_mask_pqin_gain_offset1_reg;
	srnn_pq_mask_pqin_gain_offset2_RBUS  srnn_pq_mask_pqin_gain_offset2_reg;

	if (gSrnnTopInfo.ClkEn == 1){
			switch(Pqmask_idx){
#ifndef BUILD_QUICK_SHOW
			case 0:
				srnn_pq_mask_pqin_gain_offset1_reg.regValue = IoReg_Read32(SRNN_PQ_mask_PQIN_Gain_offset1_reg);
				srnn_pq_mask_pqin_gain_offset1_reg.nnsr_pq_in_1_gain = Gain;
				srnn_pq_mask_pqin_gain_offset1_reg.nnsr_pq_in_1_offset = Offset;
				IoReg_Write32(SRNN_PQ_mask_PQIN_Gain_offset1_reg, srnn_pq_mask_pqin_gain_offset1_reg.regValue);
				break;
			case 1:
				srnn_pq_mask_pqin_gain_offset1_reg.regValue = IoReg_Read32(SRNN_PQ_mask_PQIN_Gain_offset1_reg);
				srnn_pq_mask_pqin_gain_offset1_reg.nnsr_pq_in_2_gain = Gain;
				srnn_pq_mask_pqin_gain_offset1_reg.nnsr_pq_in_2_offset = Offset;
				IoReg_Write32(SRNN_PQ_mask_PQIN_Gain_offset1_reg, srnn_pq_mask_pqin_gain_offset1_reg.regValue);
				break;
			case 2:
				srnn_pq_mask_pqin_gain_offset2_reg.regValue = IoReg_Read32(SRNN_PQ_mask_PQIN_Gain_offset2_reg);
				srnn_pq_mask_pqin_gain_offset2_reg.nnsr_pq_in_3_gain = Gain;
				srnn_pq_mask_pqin_gain_offset2_reg.nnsr_pq_in_3_offset = Offset;
				IoReg_Write32(SRNN_PQ_mask_PQIN_Gain_offset2_reg, srnn_pq_mask_pqin_gain_offset2_reg.regValue);
				break;
#else	// in qs, there are no pq mask
			case 0:
				srnn_pq_mask_pqin_gain_offset1_reg.regValue = IoReg_Read32(SRNN_PQ_mask_PQIN_Gain_offset1_reg);
				srnn_pq_mask_pqin_gain_offset1_reg.nnsr_pq_in_1_gain = 0;
				srnn_pq_mask_pqin_gain_offset1_reg.nnsr_pq_in_1_offset = 0;
				IoReg_Write32(SRNN_PQ_mask_PQIN_Gain_offset1_reg, srnn_pq_mask_pqin_gain_offset1_reg.regValue);
				break;
			case 1:
				srnn_pq_mask_pqin_gain_offset1_reg.regValue = IoReg_Read32(SRNN_PQ_mask_PQIN_Gain_offset1_reg);
				srnn_pq_mask_pqin_gain_offset1_reg.nnsr_pq_in_2_gain = 0;
				srnn_pq_mask_pqin_gain_offset1_reg.nnsr_pq_in_2_offset = 0;
				IoReg_Write32(SRNN_PQ_mask_PQIN_Gain_offset1_reg, srnn_pq_mask_pqin_gain_offset1_reg.regValue);
				break;
			case 2:
				srnn_pq_mask_pqin_gain_offset2_reg.regValue = IoReg_Read32(SRNN_PQ_mask_PQIN_Gain_offset2_reg);
				srnn_pq_mask_pqin_gain_offset2_reg.nnsr_pq_in_3_gain = 0;
				srnn_pq_mask_pqin_gain_offset2_reg.nnsr_pq_in_3_offset = 0;
				IoReg_Write32(SRNN_PQ_mask_PQIN_Gain_offset2_reg, srnn_pq_mask_pqin_gain_offset2_reg.regValue);
				break;
#endif
			default:
				break;

		}
	}
	
	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Set PQMask out gain & offset
 * @param Gain
 * gain 0+1+7
 * @param Offset
 * offset 1+1+6
 */
void SrnnCtrl_SetPQMaskOutGainOffset(unsigned char Gain, unsigned char Offset, unsigned char isSkipDB){
	srnn_pre_post_srnn_pre_pos_pq_mask_gain_offset_RBUS  srnn_pre_post_srnn_pre_pos_pq_mask_gain_offset_reg;

	srnn_pre_post_srnn_pre_pos_pq_mask_gain_offset_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_PQ_mask_Gain_offset_reg);
#ifndef BUILD_QUICK_SHOW
	srnn_pre_post_srnn_pre_pos_pq_mask_gain_offset_reg.srnn_pq_out_gain = Gain;
	srnn_pre_post_srnn_pre_pos_pq_mask_gain_offset_reg.srnn_pq_out_offset = Offset;
#else	// in qs, there are no pq mask
	srnn_pre_post_srnn_pre_pos_pq_mask_gain_offset_reg.srnn_pq_out_gain = 0;
	srnn_pre_post_srnn_pre_pos_pq_mask_gain_offset_reg.srnn_pq_out_offset = 64;
#endif
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_PQ_mask_Gain_offset_reg, srnn_pre_post_srnn_pre_pos_pq_mask_gain_offset_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/* ----- ----- ----- ----- ----- DMA2SRNN RELATED FUNCTIONS ----- ----- ----- ----- ----- */
/**
 * @brief
 * Set 3DLUT enable
 * @param mode
 * 3DLUT enable
 */
void SrnnCtrl_Set3DLUTEn(unsigned char mode){
	srnn_wdsr_ctrl_RBUS  srnn_wdsr_ctrl_reg;
	if (gSrnnTopInfo.ClkEn == 1){
		srnn_wdsr_ctrl_reg.regValue = IoReg_Read32(SRNN_WDSR_CTRL_reg);
		srnn_wdsr_ctrl_reg.ai_sr_srnn_3dlut_en = mode;
		IoReg_Write32(SRNN_WDSR_CTRL_reg, srnn_wdsr_ctrl_reg.regValue);
	}
	else{
		rtd_pr_vpq_warn("[%s] srnn clk off, do nothing\n", __FUNCTION__);
	}
}

/**
 * @brief
 * Set wtable_act1_mode
 */
void SrnnCtrl_SetWtableAct1Mode(unsigned char mode){
	srnn_pre_post_wtable_act_sel1_RBUS  srnn_pre_post_wtable_act_sel1_reg;

	srnn_pre_post_wtable_act_sel1_reg.regValue = IoReg_Read32(SRNN_PRE_POST_Wtable_act_sel1_reg);
	srnn_pre_post_wtable_act_sel1_reg.wtable_act1_mode = mode;
	IoReg_Write32(SRNN_PRE_POST_Wtable_act_sel1_reg, srnn_pre_post_wtable_act_sel1_reg.regValue);

}

/**
 * @brief
 * Set 3DLUT db ctrl wtable apply
 */
void SrnnCtrl_SetDMAWtableApply(void){
	srnn_3dlut_srnn_3dlut_db_ctl_RBUS  srnn_3dlut_srnn_3dlut_db_ctl_reg;
	if (gSrnnTopInfo.ClkEn == 1){
		srnn_3dlut_srnn_3dlut_db_ctl_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_db_ctl_reg);
		srnn_3dlut_srnn_3dlut_db_ctl_reg.wtable_apply = 1;
		IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_db_ctl_reg, srnn_3dlut_srnn_3dlut_db_ctl_reg.regValue);
	}else{
		rtd_pr_vpq_warn("[%s] srnn clk off, do nothing\n", __FUNCTION__);
	}
}

/**
 * @brief
 * Set 3DLUT db ctrl
 */
void SrnnCtrl_Set3DLUTDBCtrl(unsigned char ActSel, unsigned char BpEn, unsigned char DbRead, unsigned char DbEn){
	srnn_3dlut_srnn_3dlut_db_ctl_RBUS  srnn_3dlut_srnn_3dlut_db_ctl_reg;
	if (gSrnnTopInfo.ClkEn == 1){
		srnn_3dlut_srnn_3dlut_db_ctl_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_db_ctl_reg);
		srnn_3dlut_srnn_3dlut_db_ctl_reg.wtable_act_sel = ActSel;
		srnn_3dlut_srnn_3dlut_db_ctl_reg.wtable_act_bp_en = BpEn;
		srnn_3dlut_srnn_3dlut_db_ctl_reg.db_read = DbRead;
		srnn_3dlut_srnn_3dlut_db_ctl_reg.db_en = DbEn;
		IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_db_ctl_reg, srnn_3dlut_srnn_3dlut_db_ctl_reg.regValue);
	}else{
		rtd_pr_vpq_warn("[%s] srnn clk off, do nothing\n", __FUNCTION__);
	}
}

/**
 * @brief
 * Set Table DMA enable
 */
void SrnnCtrl_SetDMATblEn(unsigned char En){
	srnn_3dlut_srnn_3dlut_table0_format0_RBUS  srnn_3dlut_srnn_3dlut_table0_format0_reg;
	if (gSrnnTopInfo.ClkEn == 1){
		srnn_3dlut_srnn_3dlut_table0_format0_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Format0_reg);
		srnn_3dlut_srnn_3dlut_table0_format0_reg.table_dma_en = En;
		IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_Table0_Format0_reg, srnn_3dlut_srnn_3dlut_table0_format0_reg.regValue);
	}else{
		rtd_pr_vpq_warn("[%s] srnn clk off, do nothing\n", __FUNCTION__);
	}
}

/**
 * @brief
 * Set table mstart
 */
void SrnnCtrl_SetTblStartAddr(unsigned int Addr){
	srnn_3dlut_srnn_3dlut_table0_addr_RBUS  srnn_3dlut_srnn_3dlut_table0_addr_reg;
	if (gSrnnTopInfo.ClkEn == 1){
		srnn_3dlut_srnn_3dlut_table0_addr_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Addr_reg);
		srnn_3dlut_srnn_3dlut_table0_addr_reg.table0_mstart = (Addr>> 4);
		IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_Table0_Addr_reg, srnn_3dlut_srnn_3dlut_table0_addr_reg.regValue);
	}else{
		rtd_pr_vpq_warn("[%s] srnn clk off, do nothing\n", __FUNCTION__);
	}
}

/**
 * @brief
 * Set table brust setting
 * @param Len
 * brust len
 * @param Num
 * brust num
 */
void SrnnCtrl_SetTblBrust(unsigned char Len, unsigned char Num){
	srnn_3dlut_srnn_3dlut_table0_burst_RBUS  srnn_3dlut_srnn_3dlut_table0_burst_reg;
	srnn_3dlut_srnn_3dlut_table0_format1_RBUS srnn_3dlut_srnn_3dlut_table0_format1_reg;
	if (gSrnnTopInfo.ClkEn == 1){
		srnn_3dlut_srnn_3dlut_table0_burst_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Burst_reg);
		srnn_3dlut_srnn_3dlut_table0_burst_reg.table_burst_num = Num;
		
		srnn_3dlut_srnn_3dlut_table0_format1_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Format1_reg);
		srnn_3dlut_srnn_3dlut_table0_format1_reg.table_burst_len = Len;

		IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_Table0_Burst_reg, srnn_3dlut_srnn_3dlut_table0_burst_reg.regValue);
		IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_Table0_Format1_reg, srnn_3dlut_srnn_3dlut_table0_format1_reg.regValue);
	}else{
		rtd_pr_vpq_warn("[%s] srnn clk off, do nothing\n", __FUNCTION__);
	}
}

/**
 * @brief
 * Set table brust setting
 * @param Len
 * brust len
 * @param Num
 * brust num
 */
void SrnnLib_DMAApply(unsigned int PhyAddr, unsigned char isSkipDB){
	SrnnCtrl_Set3DLUTDBCtrl(1, 0, 0, 1);
	
	SrnnCtrl_Set3DLUTEn(1);
	SrnnCtrl_SetDMATblEn(1);

	SrnnCtrl_SetTblStartAddr(PhyAddr);
	SrnnCtrl_SetTblBrust(32, gBrustNumTbl[gSrnnTopInfo.ModeAISR-2][gSrnnTopInfo.ModeScale]);

	SrnnCtrl_SetDMAWtableApply();
	
	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/* ----- ----- ----- ----- ----- DMA merlin8p run merlin8 FUNCTIONS ----- ----- ----- ----- ----- */
/**
 * @brief
 * Reorder parameter to dma format merlin8p run merlin8
 */
unsigned int SrnnLib_DMA_write_table_head_merlin8p_4KNNSR(unsigned short* out_weight_vector, unsigned short* weight_vector, unsigned int counter){
	unsigned short i;

	// weight
	for (i=0; i < gSrnnArchInfo.ChB*(gSrnnArchInfo.ChF+3)*9; i++){
		out_weight_vector[counter] =  weight_vector[i];counter++;
	}
	// bias
	for (i=0; i < gSrnnArchInfo.ChF; i++){
		out_weight_vector[counter] =  weight_vector[gSrnnArchInfo.ChF*gSrnnArchInfo.ChIN*9+i];counter++;
	}
	
	// padding zeros
	out_weight_vector[counter] =  0;counter++;
	
	return counter;
}
/**
 * @brief
 * Reorder parameter to dma format merlin8p run merlin8
 */
unsigned int SrnnLib_DMA_write_table_head_merlin8p(unsigned short* out_weight_vector, unsigned short* weight_vector, unsigned int counter){
	unsigned short i;
	unsigned short output_channel;

	// weight
	for (output_channel=0; output_channel < gSrnnArchInfo.ChF; output_channel++){
		for (i=0; i < 1*9; i++){// y channel
			out_weight_vector[counter] =  weight_vector[i+output_channel*gSrnnArchInfo.ChIN*9];counter++;
		}
		for (i=0; i < 3*9; i++){// padding zeros for merlin8p structure
			out_weight_vector[counter] = 0; counter++;
		}
		for (i=1*9; i < gSrnnArchInfo.ChIN*9; i++){// cbcr + pqmask
			out_weight_vector[counter] =  weight_vector[i+output_channel*gSrnnArchInfo.ChIN*9];counter++;
		}
	}
	// bias
	for (i=0; i < gSrnnArchInfo.ChF; i++){
		out_weight_vector[counter] =  weight_vector[gSrnnArchInfo.ChF*gSrnnArchInfo.ChIN*9+i];counter++;
	}
	
	// padding zeros
	out_weight_vector[counter] =  0;counter++;
	
	return counter;
}

/**
 * @brief
 * Reorder parameter to dma format
 */
unsigned int SrnnLib_DMA_write_table_body_L2_merlin8p(unsigned short* out_weight_vector, unsigned short* weight_vector, unsigned int counter){
	unsigned short i, j;

	// weight
	for (i=0; i < gSrnnArchInfo.ChB*gSrnnArchInfo.ChF*9; i++){
		out_weight_vector[counter] =  weight_vector[i];counter++;
	}

	// bias
	for (i=0; i < gSrnnArchInfo.ChB; i=i+2){
		out_weight_vector[counter] =  weight_vector[gSrnnArchInfo.ChB*gSrnnArchInfo.ChF*9+i];counter++;
		out_weight_vector[counter] =  weight_vector[gSrnnArchInfo.ChB*gSrnnArchInfo.ChF*9+i+1];counter++;
		// padding zeros
		for (j=0; j < 7; j++){
			out_weight_vector[counter] =  0;counter++;
		}
	}

	return counter;
}
/**
 * @brief
 * Reorder parameter to dma format
 */
unsigned int SrnnLib_DMA_write_table_body_group_L2_merlin8p(unsigned short* out_weight_vector, unsigned short* weight_vector, unsigned int counter){
	unsigned short i, j;

	// weight
	for (i=0; i < gSrnnArchInfo.ChB*(gSrnnArchInfo.ChF/2)*9; i++){
		out_weight_vector[counter] =  weight_vector[i];counter++;
	}

	// bias
	for (i=0; i < gSrnnArchInfo.ChB;  i=i+2){
		out_weight_vector[counter] =  weight_vector[gSrnnArchInfo.ChB*(gSrnnArchInfo.ChF/2)*9+i];counter++;
		out_weight_vector[counter] =  weight_vector[gSrnnArchInfo.ChB*(gSrnnArchInfo.ChF/2)*9+i+1];counter++;
		// padding zeros
		for (j=0; j < 7; j++){
			out_weight_vector[counter] =  0;counter++;
		}
	}

	return counter;
}

/* ----- ----- ----- ----- ----- DMA RERANGE FUNCTIONS ----- ----- ----- ----- ----- */
/**
 * @brief
 * Reorder parameter to dma format
 */
unsigned int SrnnLib_DMA_write_table_head(unsigned short* out_weight_vector, unsigned short* weight_vector, unsigned int counter){
	unsigned short i;

	// weight
	for (i=0; i < gSrnnArchInfo.ChF*gSrnnArchInfo.ChIN*9; i++){
		out_weight_vector[counter] =  weight_vector[i];counter++;
	}

	// bias
	for (i=0; i < gSrnnArchInfo.ChF; i++){
		out_weight_vector[counter] =  weight_vector[gSrnnArchInfo.ChF*gSrnnArchInfo.ChIN*9+i];counter++;
	}
	
	// padding zeros
	out_weight_vector[counter] =  0;counter++;
	
	return counter;
}

/**
 * @brief
 * Reorder parameter to dma format
 */
unsigned int SrnnLib_DMA_write_table_body_L2(unsigned short* out_weight_vector, unsigned short* weight_vector, unsigned int counter){
	unsigned short i, j;

	// weight
	for (i=0; i < gSrnnArchInfo.ChB*gSrnnArchInfo.ChF*9; i++){
		out_weight_vector[counter] =  weight_vector[i];counter++;
	}

	// bias
	for (i=0; i < gSrnnArchInfo.ChB; i++){
		out_weight_vector[counter] =  weight_vector[gSrnnArchInfo.ChB*gSrnnArchInfo.ChF*9+i];counter++;
		// padding zeros
		for (j=0; j < 8; j++){
			out_weight_vector[counter] =  0;counter++;
		}
	}

	return counter;
}

/**
 * @brief
 * Reorder parameter to dma format
 */
unsigned int SrnnLib_DMA_write_table_body_L3(unsigned short* out_weight_vector, unsigned short* weight_vector, unsigned int counter){
	unsigned short i;

	// weight
	for (i=0; i < gSrnnArchInfo.ChF*gSrnnArchInfo.ChB*3; i++){
		out_weight_vector[counter] =  weight_vector[i];counter++;
	}

	// bias
	for (i=0; i < gSrnnArchInfo.ChF; i++){
		out_weight_vector[counter] =  weight_vector[gSrnnArchInfo.ChF*gSrnnArchInfo.ChB*3+i];counter++;
	}

	// padding zeros
	out_weight_vector[counter] =  0;counter++;

	return counter;
}


/**
 * @brief
 * Reorder parameter to dma format
 */
unsigned int SrnnLib_DMA_write_table_body_group_L2(unsigned short* out_weight_vector, unsigned short* weight_vector, unsigned int counter){
	unsigned short i, j;

	// weight
	for (i=0; i < gSrnnArchInfo.ChB*(gSrnnArchInfo.ChF/2)*9; i++){
		out_weight_vector[counter] =  weight_vector[i];counter++;
	}

	// bias
	for (i=0; i < gSrnnArchInfo.ChB; i++){
		out_weight_vector[counter] =  weight_vector[gSrnnArchInfo.ChB*(gSrnnArchInfo.ChF/2)*9+i];counter++;
		// padding zeros
		for (j=0; j < 8; j++){
			out_weight_vector[counter] =  0;counter++;
		}
	}

	return counter;
}

/**
 * @brief
 * Reorder parameter to dma format
 */
unsigned int SrnnLib_DMA_write_table_body_group_L3(unsigned short* out_weight_vector, unsigned short* weight_vector, unsigned int counter){
	unsigned short i;

	// weight
	for (i=0; i < gSrnnArchInfo.ChF*(gSrnnArchInfo.ChB/2)*3; i++){
		out_weight_vector[counter] =  weight_vector[i];counter++;
	}

	// bias
	for (i=0; i < gSrnnArchInfo.ChF; i++){
		out_weight_vector[counter] =  weight_vector[gSrnnArchInfo.ChF*(gSrnnArchInfo.ChB/2)*3+i];counter++;
	}

	// padding zeros
	out_weight_vector[counter] =  0;counter++;

	return counter;
}

/**
 * @brief
 * Reorder parameter to dma format
 */
unsigned int SrnnLib_DMA_write_table_tail(unsigned short* out_weight_vector, unsigned short* weight_vector, unsigned int counter){
	unsigned short i;

	// weight
	for (i=0; i < gSrnnArchInfo.ChPSNRTail*gSrnnArchInfo.ChF*9; i++){
		out_weight_vector[counter] =  weight_vector[i];counter++;
	}

	// bias
	for (i=0; i < gSrnnArchInfo.ChPSNRTail; i++){
		out_weight_vector[counter] =  weight_vector[gSrnnArchInfo.ChPSNRTail*gSrnnArchInfo.ChF*9+i];counter++;
	}

	// padding zeros if scalemode != x3, x1.5
	if (gSrnnArchInfo.ModeScale != 2 && gSrnnArchInfo.ModeScale != 5){
		for (i=0; i < 2; i++){
			out_weight_vector[counter] =  0;counter++;
		}
	}

	// make total ddr is multiply of max BL*word len(32x128)
	while(counter*9 % 4096 != 0){
		out_weight_vector[counter] =  0;counter++;
	}
	
	return counter;
}


/**
 * @brief
 * Reorder parameter to dma format for mode2/3
 */
unsigned int SrnnLib_DMA_SetMode_ForNormConv(unsigned short* param){
	unsigned int CounterParamTmp = 0;
	unsigned int CounterAcc = 0;
	unsigned char i = 0;
	if(get_mach_type()==MACH_ARCH_RTK2885P){
		// merlin8 IC , run merlin8 flow
		// Head
		CounterParamTmp = SrnnLib_DMA_write_table_head(gSrnnParamTmp, param, CounterParamTmp);
		CounterAcc += gSrnnArchInfo.ChF*gSrnnArchInfo.ChIN*9 + gSrnnArchInfo.ChF;	// weight + bias

		// Body
		for (i = 0; i < gSrnnArchInfo.NumBlock; i++){
			CounterParamTmp = SrnnLib_DMA_write_table_body_L2(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
			CounterAcc += gSrnnArchInfo.ChB*gSrnnArchInfo.ChF*9 + gSrnnArchInfo.ChB;	// weight + bias
			
			CounterParamTmp = SrnnLib_DMA_write_table_body_L3(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
			CounterAcc += gSrnnArchInfo.ChF*gSrnnArchInfo.ChB*3 + gSrnnArchInfo.ChF;	// weight + bias
		}

		// Tail
		CounterParamTmp = SrnnLib_DMA_write_table_tail(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
		CounterAcc += gSrnnArchInfo.ChPSNRTail*gSrnnArchInfo.ChF*9 + gSrnnArchInfo.ChPSNRTail;	// weight + bias
		
		return CounterParamTmp;
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
		// merlin8p IC, run merlin8 flow
		if (gSrnnTopInfo.SubPathEn == 1){//4K NNSR add three channel for unshuffle
			// 4KNNSR
			// Head
			CounterParamTmp = SrnnLib_DMA_write_table_head_merlin8p_4KNNSR(gSrnnParamTmp, param, CounterParamTmp);
			CounterAcc += gSrnnArchInfo.ChF*(gSrnnArchInfo.ChIN+3)*9 + gSrnnArchInfo.ChF;	// weight + bias

			// Body
			for (i = 0; i < gSrnnArchInfo.NumBlock; i++){
				CounterParamTmp = SrnnLib_DMA_write_table_body_L2_merlin8p(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
				CounterAcc += gSrnnArchInfo.ChB*gSrnnArchInfo.ChF*9 + gSrnnArchInfo.ChB;	// weight + bias
				
				CounterParamTmp = SrnnLib_DMA_write_table_body_L3(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
				CounterAcc += gSrnnArchInfo.ChF*gSrnnArchInfo.ChB*3 + gSrnnArchInfo.ChF;	// weight + bias
			}

			// Tail
			CounterParamTmp = SrnnLib_DMA_write_table_tail(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
			CounterAcc += gSrnnArchInfo.ChPSNRTail*gSrnnArchInfo.ChF*9 + gSrnnArchInfo.ChPSNRTail;	// weight + bias
			
			return CounterParamTmp;
		}
		else{
			// Head
			CounterParamTmp = SrnnLib_DMA_write_table_head_merlin8p(gSrnnParamTmp, param, CounterParamTmp);
			CounterAcc += gSrnnArchInfo.ChF*gSrnnArchInfo.ChIN*9 + gSrnnArchInfo.ChF;	// weight + bias

			// Body
			for (i = 0; i < gSrnnArchInfo.NumBlock; i++){
				CounterParamTmp = SrnnLib_DMA_write_table_body_L2_merlin8p(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
				CounterAcc += gSrnnArchInfo.ChB*gSrnnArchInfo.ChF*9 + gSrnnArchInfo.ChB;	// weight + bias
				
				CounterParamTmp = SrnnLib_DMA_write_table_body_L3(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
				CounterAcc += gSrnnArchInfo.ChF*gSrnnArchInfo.ChB*3 + gSrnnArchInfo.ChF;	// weight + bias
			}

			// Tail
			CounterParamTmp = SrnnLib_DMA_write_table_tail(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
			CounterAcc += gSrnnArchInfo.ChPSNRTail*gSrnnArchInfo.ChF*9 + gSrnnArchInfo.ChPSNRTail;	// weight + bias
			
			return CounterParamTmp;
		}
	}
	return CounterParamTmp;
}

/**
 * @brief
 * Reorder parameter to dma format for mode4/5
 */
unsigned int SrnnLib_DMA_SetMode_ForGroupConv(unsigned short* param){
	unsigned int CounterParamTmp = 0;
	unsigned int CounterAcc = 0;
	unsigned char i = 0;
	if(get_mach_type()==MACH_ARCH_RTK2885P){
	// merlin8 IC , run merlin8 flow
		// Head
		CounterParamTmp = SrnnLib_DMA_write_table_head(gSrnnParamTmp, param, CounterParamTmp);
		CounterAcc += gSrnnArchInfo.ChF*gSrnnArchInfo.ChIN*9 + gSrnnArchInfo.ChF;	// weight + bias

		// Body
		for (i = 0; i < gSrnnArchInfo.NumBlock; i++){
			CounterParamTmp = SrnnLib_DMA_write_table_body_group_L2(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
			CounterAcc += gSrnnArchInfo.ChB*(gSrnnArchInfo.ChF/2)*9 + gSrnnArchInfo.ChB;	// weight + bias
			
			CounterParamTmp = SrnnLib_DMA_write_table_body_group_L3(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
			CounterAcc += gSrnnArchInfo.ChF*(gSrnnArchInfo.ChB/2)*3 + gSrnnArchInfo.ChF;	// weight + bias
		}

		// Tail
		CounterParamTmp = SrnnLib_DMA_write_table_tail(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
		CounterAcc += gSrnnArchInfo.ChPSNRTail*gSrnnArchInfo.ChF*9 + gSrnnArchInfo.ChPSNRTail;	// weight + bias
		
		return CounterParamTmp;   
	}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
	// merlin8p IC, run merlin8 flow
		// Head
		if (gSrnnTopInfo.SubPathEn == 1){//4K NNSR add three channel for unshuffle
			CounterParamTmp = SrnnLib_DMA_write_table_head_merlin8p_4KNNSR(gSrnnParamTmp, param, CounterParamTmp);
			CounterAcc += gSrnnArchInfo.ChF*(gSrnnArchInfo.ChIN+3) + gSrnnArchInfo.ChF;	// weight + bias

			// Body
			for (i = 0; i < gSrnnArchInfo.NumBlock; i++){
				CounterParamTmp = SrnnLib_DMA_write_table_body_group_L2_merlin8p(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
				CounterAcc += gSrnnArchInfo.ChB*(gSrnnArchInfo.ChF/2)*9 + gSrnnArchInfo.ChB;	// weight + bias
				
				CounterParamTmp = SrnnLib_DMA_write_table_body_group_L3(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
				CounterAcc += gSrnnArchInfo.ChF*(gSrnnArchInfo.ChB/2)*3 + gSrnnArchInfo.ChF;	// weight + bias
			}

			// Tail
			CounterParamTmp = SrnnLib_DMA_write_table_tail(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
			CounterAcc += gSrnnArchInfo.ChPSNRTail*gSrnnArchInfo.ChF*9 + gSrnnArchInfo.ChPSNRTail;	// weight + bias
			
			return CounterParamTmp;
		}
		else{
			CounterParamTmp = SrnnLib_DMA_write_table_head_merlin8p(gSrnnParamTmp, param, CounterParamTmp);
			CounterAcc += gSrnnArchInfo.ChF*gSrnnArchInfo.ChIN*9 + gSrnnArchInfo.ChF;	// weight + bias

			// Body
			for (i = 0; i < gSrnnArchInfo.NumBlock; i++){
				CounterParamTmp = SrnnLib_DMA_write_table_body_group_L2_merlin8p(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
				CounterAcc += gSrnnArchInfo.ChB*(gSrnnArchInfo.ChF/2)*9 + gSrnnArchInfo.ChB;	// weight + bias
				
				CounterParamTmp = SrnnLib_DMA_write_table_body_group_L3(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
				CounterAcc += gSrnnArchInfo.ChF*(gSrnnArchInfo.ChB/2)*3 + gSrnnArchInfo.ChF;	// weight + bias
			}

			// Tail
			CounterParamTmp = SrnnLib_DMA_write_table_tail(gSrnnParamTmp, param+CounterAcc, CounterParamTmp);
			CounterAcc += gSrnnArchInfo.ChPSNRTail*gSrnnArchInfo.ChF*9 + gSrnnArchInfo.ChPSNRTail;	// weight + bias
			
			return CounterParamTmp;
		}
	}
	return CounterParamTmp;
}

/**
 * @brief
 * Reorder gSrnnParamTmp to specify addr
 */
void SrnnLib_DMA_ParamToDMAAddr(unsigned int* vir_addr, unsigned int counter){	
	unsigned int idx;
	unsigned char *vir_addr_8=NULL;

	vir_addr_8 = (unsigned char*)vir_addr;
	
	for(idx=0; idx<counter; idx+=8){
		*vir_addr_8 = gSrnnParamTmp[idx]>>1;
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx]&0xf)<<7) + (gSrnnParamTmp[idx+1]>>2);
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx+1]&0xf)<<6) + (gSrnnParamTmp[idx+2]>>3);
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx+2]&0xf)<<5) + (gSrnnParamTmp[idx+3]>>4);
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx+3]&0xf)<<4) + (gSrnnParamTmp[idx+4]>>5);
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx+4]&0xff)<<3) + (gSrnnParamTmp[idx+5]>>6);
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx+5]&0xff)<<2) + (gSrnnParamTmp[idx+6]>>7);
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx+6]&0xff)<<1) + (gSrnnParamTmp[idx+7]>>8);
		vir_addr_8++;

		*vir_addr_8 = gSrnnParamTmp[idx+7]&0xff;
		vir_addr_8++;
	}
}

unsigned short drvif_srnn_weight_blend_output_scaler(unsigned short pin_arry_a, unsigned short pin_arry_b, int alpha, int scale ,int int_bits, int fra_bits){
	int pos_neg_bound;
	int val_max;
	unsigned short pout_arry;
	signed short ptmp_arry_a, ptmp_arry_b;
	signed short poutmp_arry;

	pos_neg_bound = 1<<fra_bits;
	val_max = pos_neg_bound << 1;
		
	if(pin_arry_a>=pos_neg_bound)
		ptmp_arry_a = pin_arry_a-val_max;
	else 
		ptmp_arry_a = pin_arry_a;

	if(pin_arry_b>=pos_neg_bound)
		ptmp_arry_b = pin_arry_b-val_max;
	else 
		ptmp_arry_b = pin_arry_b;
	poutmp_arry = (ptmp_arry_a*( (1<<scale)-alpha) + ptmp_arry_b*alpha + (1<<(scale-1)))>>scale;

	if(poutmp_arry<0)
		pout_arry = (unsigned short)(poutmp_arry+val_max);
	else 
		pout_arry = (unsigned short)poutmp_arry;
	
	return pout_arry;
	
}

unsigned short drvif_srnn_weight_blend_output_scaler_sbit(unsigned short pin_arry_a, unsigned short pin_arry_b, int alpha, int scale ,int int_bits, int fra_bits, int shift_bits){
	int pos_neg_bound;
	int val_max;
	unsigned short pout_arry;
	signed short ptmp_arry_a, ptmp_arry_b;
	signed short poutmp_arry;
	unsigned char sign_a = 0, sign_b = 0;

	// remove shift bit first
	pin_arry_a = pin_arry_a & 0xff;
	pin_arry_b = pin_arry_b & 0xff;
	
	if (shift_bits == 1){
		pos_neg_bound = 1<<fra_bits;
		// check sign bit
		sign_a = ((pin_arry_a) & (1<<(fra_bits-1)))? 1 : 0;
		sign_b = ((pin_arry_b) & (1<<(fra_bits-1)))? 1 : 0;

		// if neg, add 1 sign bit back
		if (sign_a == 1)
			pin_arry_a = pin_arry_a + pos_neg_bound;

		if (sign_b == 1)
			pin_arry_b = pin_arry_b + pos_neg_bound;
	}
	else{
		pos_neg_bound = 1<<(fra_bits-1);
	}

	val_max = pos_neg_bound << 1;
	
	if (pin_arry_a >= pos_neg_bound)
		ptmp_arry_a = pin_arry_a - val_max;
	else
		ptmp_arry_a = pin_arry_a;
	
	if (pin_arry_b >= pos_neg_bound)
		ptmp_arry_b = pin_arry_b - val_max;
	else
		ptmp_arry_b = pin_arry_b;

	poutmp_arry = (ptmp_arry_a*( (1<<scale)-alpha) + ptmp_arry_b*alpha + (1<<(scale-1)))>>scale;

	if(poutmp_arry<0){
		poutmp_arry = poutmp_arry + val_max;
		if (shift_bits == 1){
			poutmp_arry = poutmp_arry - pos_neg_bound; 	// convert back to non-shift state
		}
	}


	if (shift_bits == 1){
		pout_arry = (unsigned short)(poutmp_arry + 0x100);	// add shift bit back
	}
	else{
		pout_arry = (unsigned short)(poutmp_arry);
	}
	
	return pout_arry;
	
}

/*================================== DMA CHECK FUNCTIONS ==============================*/
short drvif_NNSR_DMA_Err_Check(unsigned char showMSG)
{
	short ret_val = 0;
	if (gSrnnTopInfo.ClkEn == 1){
		short err_TBL[10] = {0};

		err_TBL[0] = SRNN_3DLUT_SRNN_3DLUT_Table_ErrStatus2Irq_get_table_err_status(IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table_ErrStatus2Irq_reg));

		if (err_TBL[0]  != 0) {
			ret_val = ret_val | _BIT0;
			if (showMSG == 1)
				rtd_pr_vpq_emerg("NNSR_DMA_Err_Check, TBL0=%x\n", err_TBL[0]);
		}
	}
	else{
		ret_val = 1;
		if (showMSG == 1)
			rtd_pr_vpq_warn("[%s] srnn clk off, do nothing\n", __FUNCTION__);
	}
	return ret_val;
}


void drvif_NNSR_Err_Reset(void)
{
	if (gSrnnTopInfo.ClkEn == 1){
		IoReg_SetBits(SRNN_3DLUT_SRNN_3DLUT_Table_ErrStatus2Irq_reg, SRNN_3DLUT_SRNN_3DLUT_Table_ErrStatus2Irq_table_err_clr_mask);
	}
}

unsigned char drvif_Get_NNSR_DMA_Table_Done(unsigned char tbl_idx, unsigned char showMSG)
{
	unsigned char ret = 0;
	if (gSrnnTopInfo.ClkEn == 1){
		// k24 only have one tbl
		if (tbl_idx == 0)
			ret = SRNN_3DLUT_SRNN_3DLUT_Table_Status_get_table_wdone(IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table_Status_reg));


		if (showMSG == 1)
			rtd_pr_vpq_emerg("NNSR_DMA_TBL_done, tbl_idx=%d, ret=%d,\n", tbl_idx, ret);
	}
	else{
		if (showMSG == 1)
			rtd_pr_vpq_warn("[%s] srnn clk off, do nothing\n", __FUNCTION__);
	}

	return ret;
}


void drvif_NNSR_Table_Done_Reset(unsigned char tbl_idx)
{
	if (gSrnnTopInfo.ClkEn == 1){
		// k24 only have one tbl
		if (tbl_idx == 0)
			IoReg_SetBits(SRNN_3DLUT_SRNN_3DLUT_Table_Status_reg, SRNN_3DLUT_SRNN_3DLUT_Table_Status_table_wdone_mask);
	}

}

char drvif_NNSR_Wait_for_DMA_Apply_Done(void)
{
	srnn_3dlut_srnn_3dlut_db_ctl_RBUS srnn_3dlut_srnn_3dlut_db_ctl_reg;
	char ret_val = 0;
	int apply_done_check = 0x600;//0x062500;
	if (gSrnnTopInfo.ClkEn == 1){
		srnn_3dlut_srnn_3dlut_db_ctl_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_db_ctl_reg);

		do {
			apply_done_check--;
			srnn_3dlut_srnn_3dlut_db_ctl_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_db_ctl_reg);
			if (srnn_3dlut_srnn_3dlut_db_ctl_reg.wtable_apply == 0)
				break;

			usleep_range(50, 100);
		} while (/*dmato3dtable_dmato3dtable_db_ctl_reg.wtable_apply == 1 && */apply_done_check > 0);

		if (apply_done_check <= 0) {
			rtd_pr_vpq_emerg("NNSR DMA, Wait DMA finish fail\n");
			ret_val = -1;
		}
	}
	else{
		rtd_pr_vpq_warn("[%s] srnn clk off, do nothing\n", __FUNCTION__);
		ret_val = -1;
	}
	return ret_val;
}

/*================================== DRV/FW FUNCTIONS ==============================*/
void drvif_set_srnn_init(unsigned char isSkipDB)
{
	srnn_pre_post_srnn_pre_pos_timing_ctrl_RBUS srnn_pre_post_srnn_pre_pos_timing_ctrl_reg;
	srnn_srnn_tail_timing_ctrl_RBUS srnn_srnn_tail_timing_ctrl_reg;
	
	srnn_pre_post_srnn_pre_pos_timing_ctrl_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_TIMING_CTRL_reg);
	srnn_pre_post_srnn_pre_pos_timing_ctrl_reg.srnn_tail_ready_pulse_mode = 0;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_TIMING_CTRL_reg, srnn_pre_post_srnn_pre_pos_timing_ctrl_reg.regValue);

	if (gSrnnTopInfo.ClkEn == 1){
		srnn_srnn_tail_timing_ctrl_reg.regValue = IoReg_Read32(SRNN_SRNN_TAIL_TIMING_CTRL_reg);
		srnn_srnn_tail_timing_ctrl_reg.srnn_tail_pix_based_sel = 2;
		srnn_srnn_tail_timing_ctrl_reg.srnn_tail_ready_pulse_mode = 0;
		IoReg_Write32(SRNN_SRNN_TAIL_TIMING_CTRL_reg, srnn_srnn_tail_timing_ctrl_reg.regValue);
	}

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
	
}

void drvif_srnn_gSrnnTopInfo_reset(void)
{
	memset(&gSrnnTopInfo, 0, sizeof(VIP_SRNN_TOP_INFO));
}

void drvif_srnn_clk_enable(unsigned char clk_srnn_en)
{
	int chip_type_srnn_disable = -1;
	chip_type_srnn_disable = rtk_otp_field_read_int_by_name("chip_type_srnn_disable");
	if (chip_type_srnn_disable != 0){
		// Block sw clk flag if srnn otp is disable
		rtd_pr_vpq_emerg("[%s] chip_type_srnn_disable : %d \n", __FUNCTION__, chip_type_srnn_disable);

		gSrnnTopInfo.ClkEn = 0;
#ifndef BUILD_QUICK_SHOW
		scalerPQMask_SetPqmaskSrnnStatus(PQMASK_SRNN_STAT_SUSPEND);
#endif //BUILD_QUICK_SHOW
		return;
	}

	
#ifndef BUILD_QUICK_SHOW
	if (clk_srnn_en == 1){
		SrnnCtrl_SetSRNNClkEn(clk_srnn_en);
		SrnnCtrl_SetSRNNSRSTEn(clk_srnn_en);
		gSrnnTopInfo.ClkEn = clk_srnn_en;
		scalerPQMask_SetPqmaskSrnnStatus(PQMASK_SRNN_STAT_STANDBY);
		rtd_pr_vpq_info("[%s] NNSR dbg, clk_srnn_en = %d\n", __FUNCTION__, clk_srnn_en);
	}
	else{
#if defined(CONFIG_RTK_AI_DRV)
		//using PQmask task to control srnn clk off
		scalerPQMask_SetPqmaskSrnnStatus(PQMASK_SRNN_STAT_ASK_TO_SUSPEND);
		rtd_pr_vpq_info("[%s] NNSR dbg, PQMASK_SRNN_STAT_ASK_TO_SUSPEND \n", __FUNCTION__);

#else //CONFIG_RTK_AI_DRV

		// PQmask task: not active, disable clk directly
		SrnnCtrl_SetSRNNClkEn(clk_srnn_en);
		//SrnnCtrl_SetSRNNSRSTEn(clk_srnn_en);
		gSrnnTopInfo.ClkEn = clk_srnn_en;
		scalerPQMask_SetPqmaskSrnnStatus(PQMASK_SRNN_STAT_SUSPEND);
		
		rtd_pr_vpq_info("[%s] NNSR dbg, PQMASK_SRNN_STAT_SUSPEND \n", __FUNCTION__);
		rtd_pr_vpq_info("[%s] NNSR dbg, clk_srnn_en = %d\n", __FUNCTION__, clk_srnn_en);
#endif //CONFIG_RTK_AI_DRV
	}
#else //BUILD_QUICK_SHOW
	SrnnCtrl_SetSRNNClkEn(clk_srnn_en);
	SrnnCtrl_SetSRNNSRSTEn(clk_srnn_en);
	gSrnnTopInfo.ClkEn = clk_srnn_en;
#endif //BUILD_QUICK_SHOW

}

/**
 * @brief
 * srnn clk control for pqmask task
 */
void drvif_srnn_clk_enable_pqmasktask(unsigned char clk_srnn_en)
{
	SrnnCtrl_SetSRNNClkEn(clk_srnn_en);
	if (clk_srnn_en == 1){
		SrnnCtrl_SetSRNNSRSTEn(clk_srnn_en);	// no need to off
	}
	gSrnnTopInfo.ClkEn = clk_srnn_en;
	rtd_pr_vpq_info("[%s] NNSR dbg, clk_srnn_en = %d\n", __FUNCTION__, clk_srnn_en);
}

/**
 * @brief
 * Bypass srnn pq effect
 * @param srnn_en
 * 0: Bypass; 1: Enable
 */
void drvif_srnn_pq_enable_set(unsigned char srnn_en)
{
	if (srnn_en == 0){
		SrnnCtrl_SetUZUSel(2, 0);
	}
	else{
		SrnnCtrl_SetUZUSel(1, 0);
	}
}

extern void fwif_color_ChangeUINT32Endian(unsigned int *pwTemp, int nSize, unsigned char convert_endian_flag);
extern void IoReg_WriteData32 (unsigned int addr, unsigned int value);
extern void IoReg_Writespi32 (unsigned int addr, unsigned int value);

void drvif_color_Set_NNSR_model_By_DMA(unsigned char ai_sr_mode, unsigned char su_mode, unsigned short* param, unsigned int phy_addr, unsigned int* vir_addr, unsigned char isSkipDB){
	unsigned int counter = 0;

	gSrnnTopInfo.ModeAISR = ai_sr_mode;
	gSrnnTopInfo.ModeScale = su_mode;
	
	gSrnnArchInfo.ModeAISR = ai_sr_mode;
	gSrnnArchInfo.ChPSNRTail = gSuModeSizeMapping[su_mode]*gSuModeSizeMapping[su_mode];
	gSrnnArchInfo.ModeScale = su_mode;

	// copy table for debug and show table in rtice
	memset(srnn_current_table, 0, sizeof(unsigned short)*SRNN_WEIGHT_NUM_MAX);
	memcpy(srnn_current_table, param, sizeof(unsigned short)*drvif_srnn_get_weight_table_size(ai_sr_mode, su_mode));
	
	switch(ai_sr_mode){
		case 2:
			gSrnnArchInfo.NumBlock = 8;
			gSrnnArchInfo.ChIN = 6;
			gSrnnArchInfo.ChF = 8;
			gSrnnArchInfo.ChB = 48;
			counter = SrnnLib_DMA_SetMode_ForNormConv(param);
			break;
		case 3:
			gSrnnArchInfo.NumBlock = 4;
			gSrnnArchInfo.ChIN = 6;
			gSrnnArchInfo.ChF = 8;
			gSrnnArchInfo.ChB = 48;
			counter = SrnnLib_DMA_SetMode_ForNormConv(param);
			break;
		case 4:
			gSrnnArchInfo.NumBlock = 4;
			gSrnnArchInfo.ChIN = 6;
			gSrnnArchInfo.ChF = 8;
			gSrnnArchInfo.ChB = 48;
			counter = SrnnLib_DMA_SetMode_ForGroupConv(param);
			break;
		case 5:
			gSrnnArchInfo.NumBlock = 4;
			gSrnnArchInfo.ChIN = 3;
			gSrnnArchInfo.ChF = 8;
			gSrnnArchInfo.ChB = 24;
			counter = SrnnLib_DMA_SetMode_ForGroupConv(param);
			break;
		default:
			drvif_srnn_pq_enable_set(0);
			rtd_pr_vpq_info("Incorrect ai_sr_mode\n");
			break;
	}

	SrnnLib_DMA_ParamToDMAAddr(vir_addr, counter);
	// For FPGA
	//SrnnLib_DMA_ParamToDMAAddr(phy_addr, counter);

	// wtable_apply after srnn db apply
	//SrnnCtrl_SetWtableAct1Mode(0x3);
	
	SrnnLib_DMAApply(phy_addr, isSkipDB);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
};

/**
 * @brief
 * srnn weight blending
* @param alpha
 * range [0, 16]
 */
void drvif_srnn_weight_blend_1d(unsigned char aisrmode, unsigned short* param1, unsigned short* param2, unsigned short* param_out, int alpha){
	unsigned int i, j;
	int scale = SRNN_BLENDING_SCALE;
	int int_bits = 0;
	int fra_bits = 8;
	int shift_bits = 0;
	unsigned int counter = 0;

	if (alpha >= (1<<scale)){
		rtd_pr_vpq_emerg("[%s] Incorrect alpha = %d  \n", __FUNCTION__,  alpha);
		alpha = 16;
	}

	if (aisrmode == 0){
		rtd_pr_vpq_info(" nnsr_mode=%d ,no need to do weight blending \n", aisrmode);
	}
	else{
		// Head (non-sbit)
		if(get_mach_type()==MACH_ARCH_RTK2885P){
			// merlin8 IC , run merlin8 flow
			for (i=0; i < (gSrnnArchInfo.ChF*gSrnnArchInfo.ChIN*9 + gSrnnArchInfo.ChF); i++){
				param_out[counter] = drvif_srnn_weight_blend_output_scaler(param1[counter], param2[counter], alpha, scale ,int_bits, fra_bits); counter++;
			}
		}else if(get_mach_type()==MACH_ARCH_RTK2885P2){
			// merlin8p IC, run merlin8 flow
			if (gSrnnTopInfo.SubPathEn == 1){//4K NNSR add three channel for unshuffle
				for (i=0; i < (gSrnnArchInfo.ChF*(gSrnnArchInfo.ChIN+3)*9 + gSrnnArchInfo.ChF); i++){
					param_out[counter] = drvif_srnn_weight_blend_output_scaler(param1[counter], param2[counter], alpha, scale ,int_bits, fra_bits); counter++;
				}		
			}
			else{
				for (i=0; i < (gSrnnArchInfo.ChF*gSrnnArchInfo.ChIN*9 + gSrnnArchInfo.ChF); i++){
					param_out[counter] = drvif_srnn_weight_blend_output_scaler(param1[counter], param2[counter], alpha, scale ,int_bits, fra_bits); counter++;
				}
			}
		}
		
		
		// Body
		for (i = 0; i < gSrnnArchInfo.NumBlock; i++){
			// Normal Conv
			if (aisrmode >= 2 && aisrmode <=3){
				// L2 Weight (sbit)
				for (j=0; j < gSrnnArchInfo.ChB*gSrnnArchInfo.ChF*9; j++){
					shift_bits = (param1[counter] & 0x100) >> 8;	// check shift bit of param1
					param_out[counter] = drvif_srnn_weight_blend_output_scaler_sbit(param1[counter], param2[counter], alpha, scale ,int_bits, fra_bits, shift_bits); counter++;
				}
				// L2 Bias (non-sbit)
				for (j=0; j < gSrnnArchInfo.ChB; j++){
					param_out[counter] = drvif_srnn_weight_blend_output_scaler(param1[counter], param2[counter], alpha, scale ,int_bits, fra_bits); counter++;
				}
				
				// L3 Weight (sbit)
				for (j=0; j < gSrnnArchInfo.ChF*gSrnnArchInfo.ChB*3; j++){
					shift_bits = (param1[counter] & 0x100) >> 8;	// check shift bit of param1
					param_out[counter] = drvif_srnn_weight_blend_output_scaler_sbit(param1[counter], param2[counter], alpha, scale ,int_bits, fra_bits, shift_bits); counter++;
				}
				// L3 Bias (non-sbit)
				for (j=0; j < gSrnnArchInfo.ChF; j++){
					param_out[counter] = drvif_srnn_weight_blend_output_scaler(param1[counter], param2[counter], alpha, scale ,int_bits, fra_bits); counter++;
				}
			}
			// Group Conv
			else if (aisrmode >= 4 && aisrmode <=5){
				// L2 Weight (sbit)
				for (j=0; j < gSrnnArchInfo.ChB*(gSrnnArchInfo.ChF/2)*9; j++){
					shift_bits = (param1[counter] & 0x100) >> 8;	// check shift bit of param1
					param_out[counter] = drvif_srnn_weight_blend_output_scaler_sbit(param1[counter], param2[counter], alpha, scale ,int_bits, fra_bits, shift_bits); counter++;
				}
				// L2 Bias (non-sbit)
				for (j=0; j < gSrnnArchInfo.ChB; j++){
					param_out[counter] = drvif_srnn_weight_blend_output_scaler(param1[counter], param2[counter], alpha, scale ,int_bits, fra_bits); counter++;
				}
				
				// L3 Weight (sbit)
				for (j=0; j < gSrnnArchInfo.ChF*(gSrnnArchInfo.ChB/2)*3; j++){
					shift_bits = (param1[counter] & 0x100) >> 8;	// check shift bit of param1
					param_out[counter] = drvif_srnn_weight_blend_output_scaler_sbit(param1[counter], param2[counter], alpha, scale ,int_bits, fra_bits, shift_bits); counter++;
				}
				// L3 Bias (non-sbit)
				for (j=0; j < gSrnnArchInfo.ChF; j++){
					param_out[counter] = drvif_srnn_weight_blend_output_scaler(param1[counter], param2[counter], alpha, scale ,int_bits, fra_bits); counter++;
				}
			}
		}
		
		// Tail (non-sbit)
		for (i=0; i < (gSrnnArchInfo.ChPSNRTail*gSrnnArchInfo.ChF*9 + gSrnnArchInfo.ChPSNRTail); i++){
			param_out[counter] = drvif_srnn_weight_blend_output_scaler(param1[counter], param2[counter], alpha, scale ,int_bits, fra_bits); counter++;
		}
	}
}

/**
 * @brief
 * set srnn pq_misc_table
 */
void drvif_srnn_set_table(DRV_srnn_table *ptr, unsigned char isSkipDB){
	unsigned char i = 0;
	if (gSrnnTopInfo.ClkEn == 1){
		SrnnCtrl_SetDoubleBufferEn();
		SrnnCtrl_SetPSNRCoring(ptr->PostProc.PSNR_Post.Coring, isSkipDB);
		SrnnCtrl_SetPSNRGain(ptr->PostProc.PSNR_Post.GainPos, ptr->PostProc.PSNR_Post.GainNeg, isSkipDB);
		SrnnCtrl_SetPSNRClamp(ptr->PostProc.PSNR_Post.ClampPos, ptr->PostProc.PSNR_Post.ClampNeg, isSkipDB);
		SrnnCtrl_SetLPFMode(ptr->PostProc.LPF.Mode, isSkipDB);
		SrnnCtrl_SetLPFCoeff(ptr->PostProc.LPF.Coeff0, isSkipDB);
		for (i=0; i<3; i++){
			SrnnCtrl_SetPQMaskInGainOffset(ptr->PQmask.In[i].Gain, ptr->PQmask.In[i].Offset, i, isSkipDB);
		}
		SrnnCtrl_SetPQMaskOutGainOffset(ptr->PQmask.Out.Gain, ptr->PQmask.Out.Offset, isSkipDB);

		SrnnCtrl_SetBlockDisable(ptr->BlockCtrl.BlockDisable, isSkipDB);
		SrnnCtrl_SetFeatureReduce(ptr->BlockCtrl.FeatReduce, isSkipDB);
		
		SrnnCtrl_SetHx2Coeff(ptr->Hx2.Coeff0, ptr->Hx2.Coeff1, isSkipDB);

		if (isSkipDB == 0) {
			/*db apply */
			SrnnCtrl_SetDoubleBufferApply();
		}
	}
}

/**
 * @brief
 * set srnn ctrl table from VIPSRNN flow
 */
void drvif_srnn_set_vipsrnn_ctrl_table(DRV_SRNN_CTRL_TABLE *ptr, unsigned char isSkipDB){
	unsigned char i = 0;
	
	if (gSrnnTopInfo.ClkEn == 1){
		SrnnCtrl_SetDoubleBufferEn();
		SrnnCtrl_SetPSNRCoring(ptr->PostProc.PSNR_Post.Coring, isSkipDB);
		SrnnCtrl_SetPSNRGain(ptr->PostProc.PSNR_Post.GainPos, ptr->PostProc.PSNR_Post.GainNeg, isSkipDB);
		SrnnCtrl_SetPSNRClamp(ptr->PostProc.PSNR_Post.ClampPos, ptr->PostProc.PSNR_Post.ClampNeg, isSkipDB);
		SrnnCtrl_SetLPFMode(ptr->PostProc.LPF.Mode, isSkipDB);
		SrnnCtrl_SetLPFCoeff(ptr->PostProc.LPF.Coeff0, isSkipDB);
		for (i=0; i<3; i++){
			SrnnCtrl_SetPQMaskInGainOffset(ptr->PQmask.In[i].Gain, ptr->PQmask.In[i].Offset, i, isSkipDB);
		}
		SrnnCtrl_SetPQMaskOutGainOffset(ptr->PQmask.Out.Gain, ptr->PQmask.Out.Offset, isSkipDB);
		
		if (isSkipDB == 0) {
			/*db apply */
			SrnnCtrl_SetDoubleBufferApply();
		}
	}
}

/**
 * @brief
 * reset NNSR DMA error status before write table
 */
char fwif_color_reset_NNSR_DMA_Error_Status(unsigned char tbl_idx, unsigned char showMSG){
	char ret = 0;
	unsigned char check_times = 0, check_times_max = 30;
	
	do {
		drvif_NNSR_Err_Reset();
		drvif_NNSR_Table_Done_Reset(tbl_idx);
		check_times++;
	} while(((drvif_NNSR_DMA_Err_Check(0) != 0) || (drvif_Get_NNSR_DMA_Table_Done(tbl_idx, 0) != 0)) && (check_times<check_times_max));

	if (check_times >= check_times_max) {
		rtd_pr_vpq_emerg("NNSR_DMA, can't reset DM2 DMA Err status, check times = %d,\n", check_times);
		drvif_NNSR_DMA_Err_Check(showMSG);
		drvif_Get_NNSR_DMA_Table_Done(tbl_idx, showMSG);
		ret = -1;
	}

	return ret;
}
/**
 * @brief
 * check NNSR DMA apply done after write table
 */
char fwif_color_check_NNSR_DMA_Apply_Done(unsigned char tbl_idx, unsigned char showMSG){
	char ret = 0;
	
	if (drvif_NNSR_Wait_for_DMA_Apply_Done() < 0) {
		rtd_pr_vpq_emerg("NNSR_DMA, DMA wait not finish.\n");
		drvif_srnn_pq_enable_set(0);
		ret = -1;
	} 
	else {
		if ((drvif_NNSR_DMA_Err_Check(showMSG) != 0) || (drvif_Get_NNSR_DMA_Table_Done(tbl_idx, showMSG) != 1)) {
			drvif_srnn_pq_enable_set(0);
			ret = -1;
			rtd_pr_vpq_emerg("NNSR_DMA, error status ~= 0. or table done != 1.\n");
		} 
		else {
			ret = 0;
			rtd_pr_vpq_info("NNSR_DMA, write done\n");
		}
	}

	return ret;
}

/**
 * @brief
 * For tv006, compatible with k8 format
 */
unsigned char drvif_srnn_get_y_delta_gain(void)
{
	unsigned char bGain = 0;
	// assume pos gain = neg gain
	bGain = SRNN_PRE_POST_SRNN_PRE_POS_PSNR_post_process_1_get_srnn_post_psnr_pos_gain(IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_PSNR_post_process_1_reg));
	
	// remapping gain from k24 to k8
	bGain = bGain >> 3;
	
	return bGain;
}

/**
 * @brief
 * For tv006, compatible with k8 format
 * k8 : 0~63 bit : [0+2+4] range[0, 4)
 * k24 : 0~255 bit : [0+1+7] range[0, 2)
 */
void drvif_srnn_set_y_delta_gain(unsigned char bGain)
{
	// remapping gain from k8 to k24
	bGain = (bGain>31)?(31):bGain;
	bGain = bGain << 3;
	
	SrnnCtrl_SetPSNRGain(bGain, bGain, 0);
	
}

/**
 * @brief
 * get table size by nnsr_mode and scale ratio
 */
unsigned int drvif_srnn_get_weight_table_size(unsigned char nnsr_mode, unsigned char su_mode)
{
	if (nnsr_mode >= 2 && nnsr_mode <= 5 && su_mode < SRNN_SCALE_MODE_MAX){
		return gSrnnWeightNum[nnsr_mode-2][su_mode];
	}
	else{
		rtd_pr_vpq_emerg("\nNNSR get_weight_table_size failed.\n");
		return 0;
	}
	
}

/**
 * @brief
 * set srnn size related setting according to gSrnnTopInfo
 */
void drvif_srnn_reg_init(void){
	//scaleup_ds_uzu_input_size_RBUS ds_uzu_Input_Size_REG;
	SrnnCtrl_SetDoubleBufferEn();
	
	if (gSrnnTopInfo.ModeAISR < 2){
		SrnnCtrl_SetDataInSel(0, 0);
		SrnnCtrl_SetHx2En(0, 0);
	}
	else{
		SrnnCtrl_SetDataInSel(gSrnnTopInfo.SubPathEn, 0);
		SrnnCtrl_SetSRNNInputSize(gSrnnTopInfo.SrnnIn_Height, gSrnnTopInfo.SrnnIn_Width, 0);
		SrnnCtrl_SetHx2En(gSrnnTopInfo.Hx2En, 0);

		if(gSrnnTopInfo.ModeAISR == 5)
			SrnnCtrl_SetCSCInMode(2,0);
		else
			SrnnCtrl_SetCSCInMode(0,0);

		if (gSrnnTopInfo.SubPathEn == 1){
			scaleup_ds_uzu_input_size_RBUS scaleup_ds_uzu_input_size_reg;	
			scaleup_ds_uzu_db_ctrl_RBUS ds_uzu_db_ctrl_reg;
			scaleup_ds_uzu_input_size_reg.regValue = IoReg_Read32(SCALEUP_DS_UZU_Input_Size_reg);
			scaleup_ds_uzu_input_size_reg.hor_input_size = gSrnnTopInfo.SrnnIn_Width;
			scaleup_ds_uzu_input_size_reg.ver_input_size = gSrnnTopInfo.SrnnIn_Height;
			IoReg_Write32(SCALEUP_DS_UZU_Input_Size_reg, scaleup_ds_uzu_input_size_reg.regValue);

			ds_uzu_db_ctrl_reg.regValue = IoReg_Read32(SCALEUP_DS_UZU_DB_CTRL_reg);
			ds_uzu_db_ctrl_reg.db_apply = 1;
			IoReg_Write32(SCALEUP_DS_UZU_DB_CTRL_reg, ds_uzu_db_ctrl_reg.regValue);
		}

	}
}


/**
 * @brief
 * set srnn size related setting according to gSrnnTopInfo
 */
void drvif_srnn_set_Hx2(unsigned char En){
	gSrnnTopInfo.Hx2En = En;
	SrnnCtrl_SetHx2En(gSrnnTopInfo.Hx2En, 0);
}

extern unsigned int scaler_disp_get_stage1_linerate_ratio(void);

/**
 * @brief
 * calculate the srnn clk fract_A => range [0, 63] 
 * assume fract_B = 63
 * srnn_clk = srnn_pll * (fract_A+1)/(fract_B+1)
 * iFrameRate: SLR_INPUT_V_FREQ_1000
 */
unsigned char drvif_srnn_get_current_clk_fractA(unsigned char aisrmode, unsigned char sumode, unsigned int iFrameRate){
	unsigned int linerate_ratio = 10000;
	unsigned int dpll_clk= 0;
	unsigned short output_htotal = 0, output_vtotal = 0;
	unsigned int fract_a = 63;
	ppoverlay_uzudtg_dv_total_RBUS ppoverlay_uzudtg_dv_total_reg;
	ppoverlay_uzudtg_dh_total_RBUS ppoverlay_uzudtg_dh_total_reg;

	ppoverlay_uzudtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DV_TOTAL_reg); 
	output_vtotal = ppoverlay_uzudtg_dv_total_reg.uzudtg_dv_total;
	ppoverlay_uzudtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DH_TOTAL_reg);
	output_htotal = ppoverlay_uzudtg_dh_total_reg.uzudtg_dh_total; 

	linerate_ratio = scaler_disp_get_stage1_linerate_ratio();		// 10000 = 1.0 ,  5000 = 0.5
	
	if (sumode >= SRNN_SCALE_MODE_4_3x){
		//	if srnn scaleup ratio == 1.33x or 1.5x, need higher clk to meet constraint
		fract_a = 63;
		rtd_pr_vpq_info("[%s] gSrnnTopInfo.ModeScale = %d \n", __FUNCTION__, SRNN_SCALE_MODE_4_3x);
	}
	else if (gSrnnTopInfo.HSRType == VIP_SRNN_HSR_DTG){
		// need satisfy : srnn_clk/d_d1p_clk < 2*Mode_cycle
		dpll_clk = (output_htotal * iFrameRate / 1000) * output_vtotal / 1000000;

		if ((gCyclePerClkx2[aisrmode-2]*64*dpll_clk/gSrnnPLL) >= 1){
			fract_a = (gCyclePerClkx2[aisrmode-2]*64*dpll_clk/gSrnnPLL) - 1;

			if (fract_a > 63){
				fract_a = 63;
			}
		}
		else{
			fract_a = 63;
			rtd_pr_vpq_emerg("[%s] cannot satisfy formula. \n", __FUNCTION__);
		}
		rtd_pr_vpq_info("[%s] clk formula=%d. \n", __FUNCTION__, (gCyclePerClkx2[aisrmode-2]*64*dpll_clk/gSrnnPLL));
			
	}
	else if (linerate_ratio > 10000){
		fract_a = 63;
		rtd_pr_vpq_emerg("[%s] scaler_disp_get_stage1_linerate_ratio = %d > 10000\n", __FUNCTION__, linerate_ratio);
	
	}
	else{
		// by linerate
		fract_a = ((64*linerate_ratio)/10000)+1;	// ceiling
		if (fract_a > 63){
			//rtd_pr_vpq_info("[%s] fract_a = %d > 63\n", __FUNCTION__, fract_a);
			fract_a = 63;
		}
		rtd_pr_vpq_info("[%s] scaler_disp_get_stage1_linerate_ratio = %d\n", __FUNCTION__, linerate_ratio);
		
	}
	
	rtd_pr_vpq_info("[%s] fract_a = %d\n", __FUNCTION__, fract_a);

	return fract_a;

}


/**
 * @brief
 * set srnn clk fract by linerate ratio to avoid power ripple
 */
void drvif_srnn_set_clk_fract_by_linerate_ratio(void){
#if 0
	unsigned int linerate_ratio = 10000;
	linerate_ratio = scaler_disp_get_stage1_linerate_ratio();		// 10000 = 1.0 ,  5000 = 0.5

	if (linerate_ratio > 10000  || gSrnnTopInfo.ModeScale >= SRNN_SCALE_MODE_4_3x || gSrnnTopInfo.HSRType == VIP_SRNN_HSR_DTG){
		//  if srnn scaleup ratio == 1.33x or 1.5x, need higher clk to meet constraint
		SrnnCtrl_SetSRNNClkFract(0, 0, 0);
		rtd_pr_vpq_info("[%s] gSrnnTopInfo.ModeScale = %d \n", __FUNCTION__, SRNN_SCALE_MODE_4_3x);
		rtd_pr_vpq_emerg("[%s] scaler_disp_get_stage1_linerate_ratio = %d > 10000\n", __FUNCTION__, linerate_ratio);
	}
	else{
		unsigned char fract_a = 0;
		fract_a = ((64*linerate_ratio)/10000)+1;	// ceiling
		if (fract_a > 63){
			//rtd_pr_vpq_info("[%s] fract_a = %d > 63\n", __FUNCTION__, fract_a);
			fract_a = 63;
		}
		SrnnCtrl_SetSRNNClkFract(1, fract_a, 63);
		rtd_pr_vpq_info("[%s] scaler_disp_get_stage1_linerate_ratio = %d\n", __FUNCTION__, linerate_ratio);
		
	}
#endif
	unsigned char fract_a = 63;
	fract_a = drvif_srnn_get_current_clk_fractA(gSrnnTopInfo.ModeAISR, gSrnnTopInfo.ModeScale, gSrnnTopInfo.FrameRate);
	SrnnCtrl_SetSRNNClkFract(1, fract_a, 63);
}

extern unsigned int Get_DISPLAY_CLOCK_TYPICAL(void);
extern unsigned int drvif_clock_get_uzu_clk(void);

/**
 * @brief
 * set srnn ai sr line delay by formula
 */
void drvif_srnn_set_aisr_line_delay(unsigned char aisrmode, unsigned int In_Length, unsigned int Out_Length){
	unsigned int line_delay1 = 0/*, line_delay2 = 0*/;
	unsigned short output_htotal = 0, output_vtotal = 0;
	unsigned int dpll_clk= 0;
	//unsigned int linerate_ratio = 10000;
	unsigned int SrnnClk = gSrnnPLL;
	ppoverlay_uzudtg_dh_total_RBUS ppoverlay_uzudtg_dh_total_reg;
	ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;
	//sys_reg_sys_dclk_gate_sel0_RBUS sys_reg_sys_dclk_gate_sel0_reg;
	//sys_reg_sys_dispclksel_RBUS sys_reg_sys_dispclksel_reg;
	ppoverlay_uzudtg_dv_total_RBUS uzudtg_dv_total_reg;
	
	uzudtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DV_TOTAL_reg);	
	output_vtotal = uzudtg_dv_total_reg.uzudtg_dv_total;
	ppoverlay_uzudtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DH_TOTAL_reg);
        output_htotal = ppoverlay_uzudtg_dh_total_reg.uzudtg_dh_total; 
	ppoverlay_uzudtg_control1_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control1_reg);
	//sys_reg_sys_dclk_gate_sel0_reg.regValue = IoReg_Read32(SYS_REG_SYS_DCLK_GATE_SEL0_reg);
	//sys_reg_sys_dispclksel_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLKSEL_reg);

	
	if (aisrmode == 0){
		if ((((is_scaler_input_2p_mode(SLR_MAIN_DISPLAY))||force_enable_two_step_uzu() || get_hdmi_vrr_4k60_mode()) && (Get_DISPLAY_REFRESH_RATE() >= 120)) || (ppoverlay_uzudtg_control1_reg.uzu_4k_120_mode == 1)){
			// vrr or 4k120 case
			SrnnCtrl_SetSRNNAISRLineDelay(2, 0);
		}
		else{
			SrnnCtrl_SetSRNNAISRLineDelay(0, 0);
		}
	}
	else if (aisrmode == 1){
		SrnnCtrl_SetSRNNAISRLineDelay(7, 0);
	}
	else if (aisrmode >= 2 && aisrmode <= 5){
		// Formula_1 = (head_input_size * cycle per_clk + model require) / (Output htotal/2) / (srnn_clk/f2p_clk) * pre_rd_line
		// Formula_2 = pre_rd_line * (Output_vsize/input_vsize)
		// Formula = max(Formula_1, Formula_2) + reg_uzu_4k_120_mode*2 + 2
#if 0
		dpll_clk = Get_DISPLAY_CLOCK_TYPICAL() >> sys_reg_sys_dclk_gate_sel0_reg.dclk_s1_f2p_gate_sel;
		if (sys_reg_sys_dispclksel_reg.dclk_fract_en == 1){  
			dpll_clk = (dpll_clk *(sys_reg_sys_dispclksel_reg.dclk_fract_a+1))/(sys_reg_sys_dispclksel_reg.dclk_fract_b+1);
		}
		dpll_clk = dpll_clk/100000;		//	(f2p_clk*10 MHz)
		output_htotal = output_htotal >> 1;		// (Output htotal/2)
#endif
#if 0
		linerate_ratio = scaler_disp_get_stage1_linerate_ratio();		// 10000 = 1.0 ,  5000 = 0.5
		if (linerate_ratio > 10000 || gSrnnTopInfo.ModeScale >= SRNN_SCALE_MODE_4_3x || gSrnnTopInfo.HSRType == VIP_SRNN_HSR_DTG){
			linerate_ratio = 10000;
			rtd_pr_vpq_emerg("[%s] scaler_disp_get_stage1_linerate_ratio = %d > 10000\n", __FUNCTION__, linerate_ratio);
		}
		SrnnClk = (SrnnClk * linerate_ratio)/10000;
#endif
		SrnnClk = (gSrnnPLL * drvif_srnn_get_current_clk_fractA(aisrmode, gSrnnTopInfo.ModeScale, gSrnnTopInfo.FrameRate))/64; 	
		dpll_clk = (output_htotal * gSrnnTopInfo.FrameRate / 1000) * output_vtotal / 1000000;
		line_delay1 = ((((gSrnnTopInfo.SrnnIn_Width*gCyclePerClkx2[aisrmode-2])>>1) + gModelRequire[aisrmode-2]) * gPreRdLine[aisrmode-2] * dpll_clk) 
			/ ((output_htotal * SrnnClk));
		//line_delay2 = ((gPreRdLine[aisrmode-2]*Out_Length)<<1)/In_Length;

		// Rounding
		//line_delay1 = (line_delay1 & 1)?((line_delay1>>1)+1):(line_delay1>>1);
		//line_delay2 = (line_delay2 & 1)?((line_delay2>>1)+1):(line_delay2>>1);

		//line_delay1 += ppoverlay_uzudtg_control1_reg.uzu_4k_120_mode*2+2;
		//line_delay2 += ppoverlay_uzudtg_control1_reg.uzu_4k_120_mode*2+2;

		line_delay1 = (line_delay1 > 63)?63:line_delay1;
		//line_delay2 = (line_delay2 > 63)?63:line_delay2;
#if 0		
		if (line_delay1 > line_delay2){
			SrnnCtrl_SetSRNNAISRLineDelay(line_delay1, 0); 
		}
		else{
			SrnnCtrl_SetSRNNAISRLineDelay(line_delay2, 0); 
		}
#else	// no line delay 2
		rtd_pr_vpq_info("NNSR_delay,in_w=%d,fr=%d,dpll_clk=%d,out_len=%d,in_len=%d, aisrMode=%d, delay=%d,\n",
			gSrnnTopInfo.SrnnIn_Width, gSrnnTopInfo.FrameRate, dpll_clk, Out_Length, In_Length, aisrmode, line_delay1);

		SrnnCtrl_SetSRNNAISRLineDelay(line_delay1, 0); 
#endif
		/*
		rtd_pr_vpq_emerg("Get_DISPLAY_CLOCK_TYPICAL: %u \n",Get_DISPLAY_CLOCK_TYPICAL());
		rtd_pr_vpq_emerg("ppoverlay_uzudtg_dh_total_reg.uzudtg_dh_total: %u \n",ppoverlay_uzudtg_dh_total_reg.uzudtg_dh_total);
		rtd_pr_vpq_emerg("sys_reg_sys_dclk_gate_sel0_reg.dclk_s1_f2p_gate_sel: %u \n",sys_reg_sys_dclk_gate_sel0_reg.dclk_s1_f2p_gate_sel);
		rtd_pr_vpq_emerg("sys_reg_sys_dispclksel_reg.dclk_fract_en: %u \n",sys_reg_sys_dispclksel_reg.dclk_fract_en);
		rtd_pr_vpq_emerg("sys_reg_sys_dispclksel_reg.dclk_fract_a: %u \n",sys_reg_sys_dispclksel_reg.dclk_fract_a);
		rtd_pr_vpq_emerg("sys_reg_sys_dispclksel_reg.dclk_fract_b: %u \n",sys_reg_sys_dispclksel_reg.dclk_fract_b);
		rtd_pr_vpq_emerg("gSrnnTopInfo.SrnnIn_Width: %u \n",gSrnnTopInfo.SrnnIn_Width);
		rtd_pr_vpq_emerg("Out_Length: %u \n",Out_Length);
		rtd_pr_vpq_emerg("In_Length: %u \n",In_Length);
		rtd_pr_vpq_emerg("gPreRdLine[aisrmode-2]: %u \n",gPreRdLine[aisrmode-2]);
		rtd_pr_vpq_emerg("gCyclePerClkx2[aisrmode-2]: %u \n",gCyclePerClkx2[aisrmode-2]);
		rtd_pr_vpq_emerg("gModelRequire[aisrmode-2]: %u \n",gModelRequire[aisrmode-2]);
		rtd_pr_vpq_emerg("line_delay1: %u \n",line_delay1);
		rtd_pr_vpq_emerg("line_delay2: %u \n",line_delay2);
		*/
	}
	else{
		SrnnCtrl_SetSRNNAISRLineDelay(0, 0);
	}
}

/**
 * @brief
 * set srnn_pre_rd_start by formula for smt srnn toggle case
 */
void drvif_srnn_set_pre_rd_start(void){
	unsigned int srnn_pre_rd = 0;
	unsigned int mdom_pre_read = 0;
	ppoverlay_uzudtg_dh_total_RBUS ppoverlay_uzudtg_dh_total_reg;
	ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;
	//mdomain_disp_ddr_mainprevstart_RBUS mdomain_disp_ddr_mainprevstart_reg;

	ppoverlay_uzudtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DH_TOTAL_reg);
	ppoverlay_uzudtg_control1_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control1_reg);
	//mdomain_disp_ddr_mainprevstart_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainPreVStart_reg);

	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//mdom_pre_read = mdomain_disp_ddr_mainprevstart_reg.main_pre_rd_v_start;
		mdom_pre_read = nonlibdma_get_disp_ddr_prevstart_reg_pre_rd_v_start(0);
	}

	if ((((is_scaler_input_2p_mode(SLR_MAIN_DISPLAY))||force_enable_two_step_uzu() || get_hdmi_vrr_4k60_mode()) && (Get_DISPLAY_REFRESH_RATE() >= 120)) || (ppoverlay_uzudtg_control1_reg.uzu_4k_120_mode == 1)){
		// vrr or 4k120 case
		srnn_pre_rd = mdom_pre_read* ppoverlay_uzudtg_dh_total_reg.uzudtg_dh_total;
	}
	else{
		srnn_pre_rd = ((mdom_pre_read + 3) * (ppoverlay_uzudtg_dh_total_reg.uzudtg_dh_total +1)) >> 1;	// h porch should be enough, +3 h line, need more delay on some case
	}

	if (srnn_pre_rd > gSrnnPreRdDefault){
		SrnnCtrl_SetSRNNPreRdStart(srnn_pre_rd, 0);
	}
	else{
		SrnnCtrl_SetSRNNPreRdStart(gSrnnPreRdDefault, 0);
	}
	
}

void drvif_srnn_demo_window(unsigned int h_sta, unsigned int h_end, unsigned int v_sta, unsigned int v_end)
{
	srnn_pre_post_srnn_pre_pos_demo_v_RBUS srnn_pre_pos_demo_v_reg;
	srnn_pre_post_srnn_pre_pos_demo_h_RBUS srnn_pre_pos_demo_h_reg;
	srnn_pre_pos_demo_v_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_DEMO_V_reg);
	srnn_pre_pos_demo_h_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_DEMO_H_reg);
	srnn_pre_pos_demo_v_reg.nnsr_demo_sr_v_sta = v_sta;
	srnn_pre_pos_demo_v_reg.nnsr_demo_sr_v_end = v_end;
	srnn_pre_pos_demo_h_reg.nnsr_demo_sr_h_sta = h_sta;
	srnn_pre_pos_demo_h_reg.nnsr_demo_sr_h_end = h_end;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_DEMO_V_reg, srnn_pre_pos_demo_v_reg.regValue);
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_DEMO_H_reg, srnn_pre_pos_demo_h_reg.regValue);
	IoReg_SetBits(SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg, SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_db_apply_mask);
}

void drvif_srnn_demo_window_mode(unsigned char demo_sr_mode)
{
	srnn_pre_post_srnn_pre_pos_demo_sel_RBUS srnn_pre_pos_demo_sel_reg;
	srnn_pre_pos_demo_sel_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_DEMO_SEL_reg);
	srnn_pre_pos_demo_sel_reg.nnsr_demo_sr_mode = demo_sr_mode;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_DEMO_SEL_reg, srnn_pre_pos_demo_sel_reg.regValue);
	IoReg_SetBits(SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg, SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_db_apply_mask);
}
