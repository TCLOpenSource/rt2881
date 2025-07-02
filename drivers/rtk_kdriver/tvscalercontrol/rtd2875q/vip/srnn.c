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
#include <rbus/mdomain_disp_reg.h>

#include <rbus/srnn_reg.h>
#include <rbus/srnn_3dlut_reg.h>
//#include <rbus/srnn_pre_post_reg.h>
#include <rbus/scaleup_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/ppoverlay_reg.h>

#include <tvscalercontrol/vip/srnn.h>
#include <tvscalercontrol/vip/srnn_table.h>
#include <tvscalercontrol/vip/scalerPQMaskCtrl.h>
#include <tvscalercontrol/vip/scalerColor.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <tvscalercontrol/vip/ai_pq.h>

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

//const unsigned char gBrustNumTbl[4][6] = { \
//	/* mode2 */ {90, 91, 92, 93, 93, 92}, \
//	/* mode3 */ {46, 46, 47, 48, 48, 47}, \
//	/* mode4 */ {26, 26, 27, 28, 28, 27}, \
//	/* mode5 */ {13, 14, 15, 16, 16, 15}, \
//}; 
const unsigned char gSuModeSizeMapping[6] = {1, 2, 3, 4, 4, 3};
const unsigned char gCyclePerClkx2[4] = {72, 36, 18, 9};
const unsigned char gPreRdLine[4] = {11, 7, 7, 7};
const unsigned char gModelRequire[4] = {250, 250, 109, 40};
const unsigned short gSrnnPLL = 1228;		// 1228.5MHz
const unsigned int gSrnnPreRdDefault = 0x1570;
unsigned char gSrnnblockallDbApply_flag = 0;		// debug usage
extern unsigned int drv_Calculate_m_pre_read_value(void);
extern RTK_AI_PQ_mode aipq_mode;

/*
const VIP_SRNN_Hx2_TABLE gHx2DefaultTable = { \
		{0xff1, 0x0d1, 0x37c, 0xfc2}, \
		{0xfc3, 0x380, 0x0cd, 0xff0}, \
};
*/
/*================================== GENERAL FUNCTIONS ==============================*/
unsigned char SrnnCtrl_Getdummy(void){
	srnn_srnn_blend_RBUS  srnn_srnn_blend_;
	unsigned char dummy;
	
	srnn_srnn_blend_.regValue = IoReg_Read32(SRNN_SRNN_BLEND_reg);
	dummy = srnn_srnn_blend_.sr_linebuf_pixel_delay%4; //dummy in merlin9

	return dummy;
}
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
#if  0 // remove rstn
	sys_reg_sys_srst4_RBUS sys_reg_sys_srst4_reg;

	sys_reg_sys_srst4_reg.regValue = 0;
	sys_reg_sys_srst4_reg.rstn_srnn = 1; //operation this bit
	sys_reg_sys_srst4_reg.write_data = En;
	
	IoReg_Write32(SYS_REG_SYS_SRST4_reg, sys_reg_sys_srst4_reg.regValue);
#endif
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
#if  0
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
#endif
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
	srnn_dm_srnn_db_ctrl_RBUS  srnn_dm_srnn_db_ctrl_;
	//srnn_pre_post_srnn_pre_pos_db_reg_ctl_RBUS  srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg;

	if (gSrnnTopInfo.ClkEn == 1){		// check aisr mode first, avoid rbus timeout
		srnn_dm_srnn_db_ctrl_.regValue = IoReg_Read32(SRNN_DM_SRNN_DB_CTRL_reg);
		srnn_dm_srnn_db_ctrl_.db_en = 1;
		IoReg_Write32(SRNN_DM_SRNN_DB_CTRL_reg, srnn_dm_srnn_db_ctrl_.regValue);
	}
	
	//srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg);
	//srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg.db_en = 1;
	//IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg, srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg.regValue);
}

/**
 * @brief
 * double buffer apply for srnn
 */
void SrnnCtrl_SetDoubleBufferApply(void){
	//SRNN_srnn_db_reg_ctl_reg 
	//SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg
	if(gSrnnblockallDbApply_flag==0){
		srnn_dm_srnn_db_ctrl_RBUS  srnn_dm_srnn_db_ctrl_;
		//srnn_pre_post_srnn_pre_pos_db_reg_ctl_RBUS  srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg;

		if (gSrnnTopInfo.ClkEn == 1){		// check srnn clk first , avoid rbus timeout
			srnn_dm_srnn_db_ctrl_.regValue = IoReg_Read32(SRNN_DM_SRNN_DB_CTRL_reg);
			srnn_dm_srnn_db_ctrl_.db_apply = 1;
			IoReg_Write32(SRNN_DM_SRNN_DB_CTRL_reg, srnn_dm_srnn_db_ctrl_.regValue);
		}
	}
	
	//srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg);
	//srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg.db_apply = 1;
	//IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_db_reg_ctl_reg, srnn_pre_post_srnn_pre_pos_db_reg_ctl_reg.regValue);
}

/**
 * @brief
 * set srnn_pre_rd_start
 */
void SrnnCtrl_SetSRNNPreRdStart(unsigned int pre_rd_sta, unsigned char isSkipDB){
#if 0
	srnn_pre_post_srnn_pre_pos_timing_ctrl_RBUS srnn_pre_post_srnn_pre_pos_timing_ctrl_reg;

	srnn_pre_post_srnn_pre_pos_timing_ctrl_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_TIMING_CTRL_reg);
        srnn_pre_post_srnn_pre_pos_timing_ctrl_reg.srnn_pre_rd_start = pre_rd_sta; 

        IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_TIMING_CTRL_reg, srnn_pre_post_srnn_pre_pos_timing_ctrl_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
#endif
}

/**
 * @brief
 * Set srnn tail scale up mode
 * @param SclaeMode
 * scale up mode => 0:1x, 1:2x, 2:3x, 3:4x, 4:1.33x, 5:1.5x
 */
void SrnnCtrl_SetSRNNScaleMode(unsigned char SclaeMode, unsigned char isSkipDB){
#if 0
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
#endif
}

/**
 * @brief
 * Merlin9 aisr model mapping to scale
 */
unsigned char SrnnCtrl_AISR_mapping_scale(unsigned char  aisr_mode){
	unsigned char scale = 0;
	if(aisr_mode==2 ||aisr_mode==3 ||aisr_mode==4 || aisr_mode==6){
		scale = 2;  //X2
	}
	else if(aisr_mode==5){
		scale = 3; //X3
	}
	else if(aisr_mode==7){
		scale = 4; //X4
	}
	return scale;
}

/**
 * @brief
 * Get srnn tail scale up mode
 * scale up mode => 0:1x, 1:2x, 2:3x, 3:4x, 4:1.33x, 5:1.5x
 */
unsigned char SrnnCtrl_AISR_mapping_su_mode(unsigned char  aisr_mode){
	unsigned char su_mode = 0;
	if(aisr_mode==2 ||aisr_mode==3 ||aisr_mode==4 || aisr_mode==6){
		su_mode = 1;  //X2
	}
	else if(aisr_mode==5){
		su_mode = 2; //X3
	}
	else if(aisr_mode==7){
		su_mode = 3; //X4
	}
	return su_mode;
}


/**
 * @brief
 * Get srnn tail scale up mode
 * scale up mode => 0:1x, 1:2x, 2:3x, 3:4x, 4:1.33x, 5:1.5x
 */
unsigned char SrnnCtrl_GetSRNNScaleMode(void){
	unsigned char su_mode = 0,aisr_mode=0;
	aisr_mode = SrnnCtrl_GetSRNNAISRMode();
	if(aisr_mode==2 ||aisr_mode==3 ||aisr_mode==4 || aisr_mode==6){
		su_mode = 1;  //X2
	}
	else if(aisr_mode==5){
		su_mode = 2; //X3
	}
	else if(aisr_mode==7){
		su_mode = 3; //X4
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
#if 0
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
#endif
}

/**
 * @brief
 * Set srnn csc in mode
 * @param mode
 * selected mode [0(YCbCr)/2(Y)]
 */
void SrnnCtrl_SetCSCInMode(unsigned char mode, unsigned char isSkipDB){
#if 0
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
#endif
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
 * 1 : UZU+NNSR; 0  : UZU only
 */
void SrnnCtrl_SetUZUSel(unsigned char mode, unsigned char isSkipDB){
	srnn_srnn_ctrl_RBUS  srnn_srnn_ctrl_;

	srnn_srnn_ctrl_.regValue = IoReg_Read32(SRNN_SRNN_CTRL_reg);
	if (mode && fwif_color_get_ai_sr_demo_mode())
		srnn_srnn_ctrl_.bbbsr_en = 3;
	else
		srnn_srnn_ctrl_.bbbsr_en= mode;
	IoReg_Write32(SRNN_SRNN_CTRL_reg, srnn_srnn_ctrl_.regValue);

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
  	srnn_srnn_pre_pos_data_in_sel_RBUS srnn_srnn_pre_pos_data_in_sel_;

   	 srnn_srnn_pre_pos_data_in_sel_.regValue = IoReg_Read32(SRNN_SRNN_PRE_POS_DATA_IN_SEL_reg);
   	 srnn_srnn_pre_pos_data_in_sel_.srnn_data_in_sel = mode;
   	 IoReg_Write32(SRNN_SRNN_PRE_POS_DATA_IN_SEL_reg,srnn_srnn_pre_pos_data_in_sel_.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/**
 * @brief
 * Set srnn hx2 en, must be same as reg_uzu_4k120_mode (DTG spec)
 * @param mode
 */
void SrnnCtrl_SetHx2En(unsigned char mode, unsigned char isSkipDB){
#if 0
	srnn_pre_post_srnn_pre_pos_hx2_ctrl0_RBUS  srnn_pre_post_srnn_pre_pos_hx2_ctrl0_reg;

	srnn_pre_post_srnn_pre_pos_hx2_ctrl0_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_HX2_Ctrl0_reg);
	srnn_pre_post_srnn_pre_pos_hx2_ctrl0_reg.srnn_hx2_en = mode;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_HX2_Ctrl0_reg, srnn_pre_post_srnn_pre_pos_hx2_ctrl0_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
#endif
}

/**
 * @brief
 * Set srnn hx2 coeff
 */
void SrnnCtrl_SetHx2Coeff(unsigned short* pCoeff0, unsigned short* pCoeff1, unsigned char isSkipDB){
#if 0
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
#endif
}

/* ----- ----- ----- ----- ----- PQ RELATED FUNCTIONS ----- ----- ----- ----- ----- */
/**
 * @brief
 * Set LPF mode
 * @param mode 0:off, 2: 1x5
 */
void SrnnCtrl_SetLPFMode(unsigned char mode, unsigned char isSkipDB){
#if 0
	srnn_pre_post_srnn_pre_pos_snr_sel_RBUS  srnn_pre_post_srnn_pre_pos_snr_sel_reg;

	srnn_pre_post_srnn_pre_pos_snr_sel_reg.regValue = IoReg_Read32(SRNN_PRE_POST_SRNN_PRE_POS_SNR_SEL_reg);
	srnn_pre_post_srnn_pre_pos_snr_sel_reg.snr_mode = mode;
	IoReg_Write32(SRNN_PRE_POST_SRNN_PRE_POS_SNR_SEL_reg, srnn_pre_post_srnn_pre_pos_snr_sel_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
#endif
}

/**
 * @brief
 * Set LPF coeffs
 */
void SrnnCtrl_SetLPFCoeff(unsigned char* pCoeff, unsigned char isSkipDB){
#if 0
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
#endif
}

/**
 * @brief
 * Set PSNR branch coring
 * @param Coring
 * coring val 4bits
 */
void SrnnCtrl_SetPSNRCoring(unsigned char Coring, unsigned char isSkipDB){
	srnn_srnn_post_process_0_RBUS  srnn_srnn_post_process_0_;

	srnn_srnn_post_process_0_.regValue = IoReg_Read32(SRNN_SRNN_POST_PROCESS_0_reg);
	srnn_srnn_post_process_0_.srnn_post_psnr_coring = Coring;
	IoReg_Write32(SRNN_SRNN_POST_PROCESS_0_reg, srnn_srnn_post_process_0_.regValue);

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
	srnn_srnn_post_process_0_RBUS  srnn_srnn_post_process_0_;

	srnn_srnn_post_process_0_.regValue = IoReg_Read32(SRNN_SRNN_POST_PROCESS_0_reg);
	srnn_srnn_post_process_0_.srnn_post_psnr_pos_gain = GainPos;
	srnn_srnn_post_process_0_.srnn_post_psnr_neg_gain = GainNeg;
	IoReg_Write32(SRNN_SRNN_POST_PROCESS_0_reg, srnn_srnn_post_process_0_.regValue);

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
	srnn_srnn_post_process_1_RBUS  srnn_srnn_post_process_1_;

	srnn_srnn_post_process_1_.regValue = IoReg_Read32(SRNN_SRNN_POST_PROCESS_1_reg);
	srnn_srnn_post_process_1_.srnn_post_psnr_clamp_up = ClampPos;
	srnn_srnn_post_process_1_.srnn_post_psnr_clamp_down = ClampNeg;
	IoReg_Write32(SRNN_SRNN_POST_PROCESS_1_reg, srnn_srnn_post_process_1_.regValue);

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
#if 0
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
#endif
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
#if 0
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
#endif
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
	srnn_wtable_act_sel1_RBUS srnn_wtable_act_sel1_reg;

	srnn_wtable_act_sel1_reg.regValue = IoReg_Read32(SRNN_Wtable_act_sel1_reg);
	srnn_wtable_act_sel1_reg.wtable_act1_mode = mode;
	IoReg_Write32(SRNN_Wtable_act_sel1_reg, srnn_wtable_act_sel1_reg.regValue);
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
		srnn_3dlut_srnn_3dlut_table0_format0_reg.table0_dma_en= En;
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
		srnn_3dlut_srnn_3dlut_table0_burst_reg.table0_burst_num = Num;
		
		srnn_3dlut_srnn_3dlut_table0_format1_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Format1_reg);
		srnn_3dlut_srnn_3dlut_table0_format1_reg.table0_burst_len = Len;

		IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_Table0_Burst_reg, srnn_3dlut_srnn_3dlut_table0_burst_reg.regValue);
		IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_Table0_Format1_reg, srnn_3dlut_srnn_3dlut_table0_format1_reg.regValue);
	}else{
		rtd_pr_vpq_warn("[%s] srnn clk off, do nothing\n", __FUNCTION__);
	}
}

/**
 * @brief
 * DMAApply for NNSR
 */
void SrnnLib_DMAApply(unsigned int PhyAddr, unsigned char isSkipDB){
	//SrnnCtrl_Set3DLUTDBCtrl(1, 0, 0, 1);
	
	//SrnnCtrl_Set3DLUTEn(1);
	//SrnnCtrl_SetDMATblEn(1);
	//SrnnCtrl_SetTblBrust(gSrnnTopInfo.ModeAISR);

	//SrnnCtrl_SetDMAWtableApply();
	//db en

	// change to merlin7 / 9 format
	int burst_num=0;
	int remain=0;
	int bitwidth=0, num_x=0, num_y=0;
	int burst_len=0, num_z=0;
	
	/* SRNN register */
	srnn_wdsr_ctrl_RBUS srnn_wdsr_ctrl_reg;
	srnn_wdsr_weig_sram_write_mode_RBUS srnn_wdsr_weig_sram_write_mode_reg;
	srnn_dm_srnn_db_ctrl_RBUS srnn_dm_srnn_db_ctrl_reg;
	srnn_wtable_act_sel1_RBUS srnn_wtable_act_sel1_reg;

	/* DMAtoSRNN register */
	/* DMAtoSRNN register */
	srnn_3dlut_srnn_3dlut_table0_format0_RBUS srnn_3dlut_srnn_3dlut_table0_format0_reg; //table0_num_x; table0_num_y;table0_bitwidth;table0_dma_en
	srnn_3dlut_srnn_3dlut_table0_format1_RBUS srnn_3dlut_srnn_3dlut_table0_format1_reg; //table0_num_z; table0_burst_len; table0_id
	srnn_3dlut_srnn_3dlut_table0_burst_RBUS srnn_3dlut_srnn_3dlut_table0_burst_reg; //brust num
	srnn_3dlut_srnn_3dlut_table0_dma_RBUS srnn_3dlut_srnn_3dlut_table0_dma_reg; //remain
	srnn_3dlut_srnn_3dlut_table0_status_RBUS srnn_3dlut_srnn_3dlut_table0_status_reg; //table0_wdone; frame_act_fw; frame_act; wtable_frame_cnt 
	srnn_3dlut_srnn_3dlut_db_ctl_RBUS srnn_3dlut_srnn_3dlut_db_ctl_reg;

	srnn_dm_srnn_db_ctrl_reg.regValue = IoReg_Read32(SRNN_DM_SRNN_DB_CTRL_reg);
	srnn_wtable_act_sel1_reg.regValue = IoReg_Read32(SRNN_Wtable_act_sel1_reg);
	srnn_wdsr_ctrl_reg.regValue = IoReg_Read32(SRNN_WDSR_CTRL_reg);
	srnn_wdsr_weig_sram_write_mode_reg.regValue = IoReg_Read32(SRNN_WDSR_WEIG_SRAM_WRITE_MODE_reg);
	srnn_3dlut_srnn_3dlut_table0_format0_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Format0_reg);
	srnn_3dlut_srnn_3dlut_table0_format1_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Format1_reg);
	srnn_3dlut_srnn_3dlut_table0_burst_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Burst_reg);
	srnn_3dlut_srnn_3dlut_table0_dma_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_DMA_reg);
	srnn_3dlut_srnn_3dlut_table0_status_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Status_reg);
	srnn_3dlut_srnn_3dlut_db_ctl_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_db_ctl_reg);
	

	SrnnCtrl_SetTblStartAddr(PhyAddr);
	if(gSrnnTopInfo.ModeAISR==2){
		burst_num = 16;
		remain = 23;
		bitwidth = 27;
		num_x = 2532;
		num_y = 1;
		burst_len = 32;
		num_z = 1;
	}
	else if(gSrnnTopInfo.ModeAISR==3||gSrnnTopInfo.ModeAISR==4){
		burst_num = 32;
		remain = 19;
		bitwidth = 27;
		num_x = 4944;
		num_y = 1;
		burst_len = 32;
		num_z = 1;		
	}
	else if(gSrnnTopInfo.ModeAISR==5){
		burst_num = 33;
		remain = 13;
		bitwidth = 27;
		num_x = 5064;
		num_y = 1;
		burst_len = 32;
		num_z = 1;		
	}
	else if(gSrnnTopInfo.ModeAISR==6){
		burst_num = 64;
		remain = 13;
		bitwidth = 27;
		num_x = 9768;
		num_y = 1;
		burst_len = 32;
		num_z = 1;
	}
	else if(gSrnnTopInfo.ModeAISR==7){
		burst_num = 66;
		remain = 10;
		bitwidth = 27;
		num_x = 10056;
		num_y = 1;
		burst_len = 32;
		num_z = 1;			
	}

	srnn_dm_srnn_db_ctrl_reg.db_en = 1;
	IoReg_Write32(SRNN_DM_SRNN_DB_CTRL_reg,srnn_dm_srnn_db_ctrl_reg.regValue);
	srnn_3dlut_srnn_3dlut_db_ctl_reg.db_en = 1;
	IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_db_ctl_reg,srnn_3dlut_srnn_3dlut_db_ctl_reg.regValue);

	srnn_wtable_act_sel1_reg.wtable_act1_mode = 3;
	IoReg_Write32(SRNN_Wtable_act_sel1_reg,srnn_wtable_act_sel1_reg.regValue);
	


	//set weight_fw_mode = 0
	srnn_wdsr_weig_sram_write_mode_reg.regValue = IoReg_Read32(SRNN_WDSR_WEIG_SRAM_WRITE_MODE_reg);
	srnn_wdsr_weig_sram_write_mode_reg.weight_fw_mode = 0;
	IoReg_Write32(SRNN_WDSR_WEIG_SRAM_WRITE_MODE_reg,srnn_wdsr_weig_sram_write_mode_reg.regValue);
	
	//set ai_srnn_3dlut_en=1
	srnn_wdsr_ctrl_reg.regValue = IoReg_Read32(SRNN_WDSR_CTRL_reg);
	srnn_wdsr_ctrl_reg.ai_sr_srnn_3dlut_en = 1;
	IoReg_Write32(SRNN_WDSR_CTRL_reg,srnn_wdsr_ctrl_reg.regValue);
	
	//set burst num=44
	srnn_3dlut_srnn_3dlut_table0_burst_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Burst_reg);
	srnn_3dlut_srnn_3dlut_table0_burst_reg.table0_burst_num = burst_num;
	IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_Table0_Burst_reg,srnn_3dlut_srnn_3dlut_table0_burst_reg.regValue);
	
	//set remain=0xb
	srnn_3dlut_srnn_3dlut_table0_dma_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_DMA_reg);
	srnn_3dlut_srnn_3dlut_table0_dma_reg.table0_remain = remain;
	IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_Table0_DMA_reg,srnn_3dlut_srnn_3dlut_table0_dma_reg.regValue);
	
	//set dma_en; bit_width=27; y=1; x=2532
	srnn_3dlut_srnn_3dlut_table0_format0_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Format0_reg);
	srnn_3dlut_srnn_3dlut_table0_format0_reg.table0_bitwidth = bitwidth;
	srnn_3dlut_srnn_3dlut_table0_format0_reg.table0_num_x = num_x ;
	srnn_3dlut_srnn_3dlut_table0_format0_reg.table0_num_y = num_y;
	srnn_3dlut_srnn_3dlut_table0_format0_reg.table0_dma_en = 1;
	IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_Table0_Format0_reg,srnn_3dlut_srnn_3dlut_table0_format0_reg.regValue);

	//set id; burst_len; z
	srnn_3dlut_srnn_3dlut_table0_format1_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Format1_reg);
	srnn_3dlut_srnn_3dlut_table0_format1_reg.table0_id = 0xc;
	srnn_3dlut_srnn_3dlut_table0_format1_reg.table0_burst_len = burst_len;
	srnn_3dlut_srnn_3dlut_table0_format1_reg.table0_num_z = num_z ;
	IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_Table0_Format1_reg,srnn_3dlut_srnn_3dlut_table0_format1_reg.regValue);

	//set db_en; db_read; wtable_apply
	srnn_3dlut_srnn_3dlut_db_ctl_reg.regValue = IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_db_ctl_reg);
	srnn_3dlut_srnn_3dlut_db_ctl_reg.wtable_act_sel = 1;
	
	srnn_3dlut_srnn_3dlut_db_ctl_reg.db_read = 1;
	srnn_3dlut_srnn_3dlut_db_ctl_reg.wtable_apply = 1;
	IoReg_Write32(SRNN_3DLUT_SRNN_3DLUT_db_ctl_reg,srnn_3dlut_srnn_3dlut_db_ctl_reg.regValue);
	
	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}

/* ----- ----- ----- ----- ----- DMA/rbus read RERANGE FUNCTIONS ----- ----- ----- ----- ----- */
// for merlin9 bias setting
unsigned short  reg_ai_sr_head_bias_1;
unsigned short  reg_ai_sr_head_bias_0;
unsigned short  reg_ai_sr_head_bias_3;
unsigned short  reg_ai_sr_head_bias_2;
unsigned short  reg_ai_sr_head_bias_5;
unsigned short  reg_ai_sr_head_bias_4;
unsigned short  reg_ai_sr_head_bias_7;
unsigned short  reg_ai_sr_head_bias_6;
unsigned short  reg_ai_sr_body_0_1x3_bias_1;
unsigned short  reg_ai_sr_body_0_1x3_bias_0;
unsigned short  reg_ai_sr_body_0_1x3_bias_3;
unsigned short  reg_ai_sr_body_0_1x3_bias_2;
unsigned short  reg_ai_sr_body_0_1x3_bias_5;
unsigned short  reg_ai_sr_body_0_1x3_bias_4;
unsigned short  reg_ai_sr_body_0_1x3_bias_7;
unsigned short  reg_ai_sr_body_0_1x3_bias_6;
unsigned short  reg_ai_sr_body_1_1x3_bias_1;
unsigned short  reg_ai_sr_body_1_1x3_bias_0;
unsigned short  reg_ai_sr_body_1_1x3_bias_3;
unsigned short  reg_ai_sr_body_1_1x3_bias_2;
unsigned short  reg_ai_sr_body_1_1x3_bias_5;
unsigned short  reg_ai_sr_body_1_1x3_bias_4;
unsigned short  reg_ai_sr_body_1_1x3_bias_7;
unsigned short  reg_ai_sr_body_1_1x3_bias_6;
unsigned short  reg_ai_sr_body_2_1x3_bias_1;
unsigned short  reg_ai_sr_body_2_1x3_bias_0;
unsigned short  reg_ai_sr_body_2_1x3_bias_3;
unsigned short  reg_ai_sr_body_2_1x3_bias_2;
unsigned short  reg_ai_sr_body_2_1x3_bias_5;
unsigned short  reg_ai_sr_body_2_1x3_bias_4;
unsigned short  reg_ai_sr_body_2_1x3_bias_7;
unsigned short  reg_ai_sr_body_2_1x3_bias_6;
unsigned short  reg_ai_sr_body_3_1x3_bias_1;
unsigned short  reg_ai_sr_body_3_1x3_bias_0;
unsigned short  reg_ai_sr_body_3_1x3_bias_3;
unsigned short  reg_ai_sr_body_3_1x3_bias_2;
unsigned short  reg_ai_sr_body_3_1x3_bias_5;
unsigned short  reg_ai_sr_body_3_1x3_bias_4;
unsigned short  reg_ai_sr_body_3_1x3_bias_7;
unsigned short  reg_ai_sr_body_3_1x3_bias_6;
unsigned short  reg_ai_sr_body_4_1x3_bias_1;
unsigned short  reg_ai_sr_body_4_1x3_bias_0;
unsigned short  reg_ai_sr_body_4_1x3_bias_3;
unsigned short  reg_ai_sr_body_4_1x3_bias_2;
unsigned short  reg_ai_sr_body_4_1x3_bias_5;
unsigned short  reg_ai_sr_body_4_1x3_bias_4;
unsigned short  reg_ai_sr_body_4_1x3_bias_7;
unsigned short  reg_ai_sr_body_4_1x3_bias_6;
unsigned short  reg_ai_sr_body_5_1x3_bias_1;
unsigned short  reg_ai_sr_body_5_1x3_bias_0;
unsigned short  reg_ai_sr_body_5_1x3_bias_3;
unsigned short  reg_ai_sr_body_5_1x3_bias_2;
unsigned short  reg_ai_sr_body_5_1x3_bias_5;
unsigned short  reg_ai_sr_body_5_1x3_bias_4;
unsigned short  reg_ai_sr_body_5_1x3_bias_7;
unsigned short  reg_ai_sr_body_5_1x3_bias_6;
unsigned short  reg_ai_sr_tail_bias_1;
unsigned short  reg_ai_sr_tail_bias_0;
unsigned short  reg_ai_sr_tail_bias_3;
unsigned short  reg_ai_sr_tail_bias_2;
unsigned short  reg_ai_sr_tail_bias_5;
unsigned short  reg_ai_sr_tail_bias_4;
unsigned short  reg_ai_sr_tail_bias_7;
unsigned short  reg_ai_sr_tail_bias_6;
unsigned short  reg_ai_sr_tail_bias_9;
unsigned short  reg_ai_sr_tail_bias_8;
unsigned short  reg_ai_sr_tail_bias_11;
unsigned short  reg_ai_sr_tail_bias_10;
unsigned short  reg_ai_sr_tail_bias_13;
unsigned short  reg_ai_sr_tail_bias_12;
unsigned short  reg_ai_sr_tail_bias_15;
unsigned short  reg_ai_sr_tail_bias_14;
void drvif_bias_rbus_set(void){
        srnn_wdsr_head_layer_bias_0_1_RBUS srnn_wdsr_head_layer_bias_0_1_;
        srnn_wdsr_head_layer_bias_2_3_RBUS srnn_wdsr_head_layer_bias_2_3_;
        srnn_wdsr_head_layer_bias_4_5_RBUS srnn_wdsr_head_layer_bias_4_5_;
        srnn_wdsr_head_layer_bias_6_7_RBUS srnn_wdsr_head_layer_bias_6_7_;
        srnn_wdsr_res_block_1_bias_0_1_RBUS srnn_wdsr_res_block_1_bias_0_1_;
        srnn_wdsr_res_block_1_bias_2_3_RBUS srnn_wdsr_res_block_1_bias_2_3_;
        srnn_wdsr_res_block_1_bias_4_5_RBUS srnn_wdsr_res_block_1_bias_4_5_;
        srnn_wdsr_res_block_1_bias_6_7_RBUS srnn_wdsr_res_block_1_bias_6_7_;
        srnn_wdsr_res_block_2_bias_0_1_RBUS srnn_wdsr_res_block_2_bias_0_1_;
        srnn_wdsr_res_block_2_bias_2_3_RBUS srnn_wdsr_res_block_2_bias_2_3_;
        srnn_wdsr_res_block_2_bias_4_5_RBUS srnn_wdsr_res_block_2_bias_4_5_;
        srnn_wdsr_res_block_2_bias_6_7_RBUS srnn_wdsr_res_block_2_bias_6_7_;
        srnn_wdsr_res_block_3_bias_0_1_RBUS srnn_wdsr_res_block_3_bias_0_1_;
        srnn_wdsr_res_block_3_bias_2_3_RBUS srnn_wdsr_res_block_3_bias_2_3_;
        srnn_wdsr_res_block_3_bias_4_5_RBUS srnn_wdsr_res_block_3_bias_4_5_;
        srnn_wdsr_res_block_3_bias_6_7_RBUS srnn_wdsr_res_block_3_bias_6_7_;
        srnn_wdsr_res_block_4_bias_0_1_RBUS srnn_wdsr_res_block_4_bias_0_1_;
        srnn_wdsr_res_block_4_bias_2_3_RBUS srnn_wdsr_res_block_4_bias_2_3_;
        srnn_wdsr_res_block_4_bias_4_5_RBUS srnn_wdsr_res_block_4_bias_4_5_;
        srnn_wdsr_res_block_4_bias_6_7_RBUS srnn_wdsr_res_block_4_bias_6_7_;
        srnn_wdsr_res_block_5_bias_0_1_RBUS srnn_wdsr_res_block_5_bias_0_1_;
        srnn_wdsr_res_block_5_bias_2_3_RBUS srnn_wdsr_res_block_5_bias_2_3_;
        srnn_wdsr_res_block_5_bias_4_5_RBUS srnn_wdsr_res_block_5_bias_4_5_;
        srnn_wdsr_res_block_5_bias_6_7_RBUS srnn_wdsr_res_block_5_bias_6_7_;
        srnn_wdsr_res_block_6_bias_0_1_RBUS srnn_wdsr_res_block_6_bias_0_1_;
        srnn_wdsr_res_block_6_bias_2_3_RBUS srnn_wdsr_res_block_6_bias_2_3_;
        srnn_wdsr_res_block_6_bias_4_5_RBUS srnn_wdsr_res_block_6_bias_4_5_;
        srnn_wdsr_res_block_6_bias_6_7_RBUS srnn_wdsr_res_block_6_bias_6_7_;
        srnn_wdsr_tail_layer_bias_0_1_RBUS srnn_wdsr_tail_layer_bias_0_1_;
        srnn_wdsr_tail_layer_bias_2_3_RBUS srnn_wdsr_tail_layer_bias_2_3_;
        srnn_wdsr_tail_layer_bias_4_5_RBUS srnn_wdsr_tail_layer_bias_4_5_;
        srnn_wdsr_tail_layer_bias_6_7_RBUS srnn_wdsr_tail_layer_bias_6_7_;
        srnn_wdsr_tail_layer_bias_8_9_RBUS srnn_wdsr_tail_layer_bias_8_9_;
        srnn_wdsr_tail_layer_bias_10_11_RBUS srnn_wdsr_tail_layer_bias_10_11_;
        srnn_wdsr_tail_layer_bias_12_13_RBUS srnn_wdsr_tail_layer_bias_12_13_;
        srnn_wdsr_tail_layer_bias_14_15_RBUS srnn_wdsr_tail_layer_bias_14_15_;

        srnn_wdsr_head_layer_bias_0_1_.regValue= 0;
        srnn_wdsr_head_layer_bias_0_1_.head_layer_bias_1 = reg_ai_sr_head_bias_1;
        srnn_wdsr_head_layer_bias_0_1_.head_layer_bias_0 = reg_ai_sr_head_bias_0;
        IoReg_Write32(SRNN_WDSR_HEAD_LAYER_BIAS_0_1_reg,srnn_wdsr_head_layer_bias_0_1_.regValue);

        srnn_wdsr_head_layer_bias_2_3_.regValue= 0;
        srnn_wdsr_head_layer_bias_2_3_.head_layer_bias_3 = reg_ai_sr_head_bias_3;
        srnn_wdsr_head_layer_bias_2_3_.head_layer_bias_2 = reg_ai_sr_head_bias_2;
        IoReg_Write32(SRNN_WDSR_HEAD_LAYER_BIAS_2_3_reg,srnn_wdsr_head_layer_bias_2_3_.regValue);

        srnn_wdsr_head_layer_bias_4_5_.regValue= 0;
        srnn_wdsr_head_layer_bias_4_5_.head_layer_bias_5 = reg_ai_sr_head_bias_5;
        srnn_wdsr_head_layer_bias_4_5_.head_layer_bias_4 = reg_ai_sr_head_bias_4;
        IoReg_Write32(SRNN_WDSR_HEAD_LAYER_BIAS_4_5_reg,srnn_wdsr_head_layer_bias_4_5_.regValue);


        srnn_wdsr_head_layer_bias_6_7_.regValue= 0;
        srnn_wdsr_head_layer_bias_6_7_.head_layer_bias_7 = reg_ai_sr_head_bias_7;
        srnn_wdsr_head_layer_bias_6_7_.head_layer_bias_6 = reg_ai_sr_head_bias_6;
        IoReg_Write32(SRNN_WDSR_HEAD_LAYER_BIAS_6_7_reg,srnn_wdsr_head_layer_bias_6_7_.regValue);

        srnn_wdsr_res_block_1_bias_0_1_.regValue= 0;
        srnn_wdsr_res_block_1_bias_0_1_.resblk_1_layer3_bias_1 = reg_ai_sr_body_0_1x3_bias_1;
        srnn_wdsr_res_block_1_bias_0_1_.resblk_1_layer3_bias_0 = reg_ai_sr_body_0_1x3_bias_0;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_1_BIAS_0_1_reg,srnn_wdsr_res_block_1_bias_0_1_.regValue);

        srnn_wdsr_res_block_1_bias_2_3_.regValue= 0;
        srnn_wdsr_res_block_1_bias_2_3_.resblk_1_layer3_bias_3 = reg_ai_sr_body_0_1x3_bias_3;
        srnn_wdsr_res_block_1_bias_2_3_.resblk_1_layer3_bias_2 = reg_ai_sr_body_0_1x3_bias_2;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_1_BIAS_2_3_reg,srnn_wdsr_res_block_1_bias_2_3_.regValue);

        srnn_wdsr_res_block_1_bias_4_5_.regValue= 0;
        srnn_wdsr_res_block_1_bias_4_5_.resblk_1_layer3_bias_5 = reg_ai_sr_body_0_1x3_bias_5;
        srnn_wdsr_res_block_1_bias_4_5_.resblk_1_layer3_bias_4 = reg_ai_sr_body_0_1x3_bias_4;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_1_BIAS_4_5_reg,srnn_wdsr_res_block_1_bias_4_5_.regValue);

        srnn_wdsr_res_block_1_bias_6_7_.regValue= 0;
        srnn_wdsr_res_block_1_bias_6_7_.resblk_1_layer3_bias_7 = reg_ai_sr_body_0_1x3_bias_7;
        srnn_wdsr_res_block_1_bias_6_7_.resblk_1_layer3_bias_6 = reg_ai_sr_body_0_1x3_bias_6;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_1_BIAS_6_7_reg,srnn_wdsr_res_block_1_bias_6_7_.regValue);

        srnn_wdsr_res_block_2_bias_0_1_.regValue= 0;
        srnn_wdsr_res_block_2_bias_0_1_.resblk_2_layer3_bias_1 = reg_ai_sr_body_1_1x3_bias_1;
        srnn_wdsr_res_block_2_bias_0_1_.resblk_2_layer3_bias_0 = reg_ai_sr_body_1_1x3_bias_0;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_2_BIAS_0_1_reg,srnn_wdsr_res_block_2_bias_0_1_.regValue);

        srnn_wdsr_res_block_2_bias_2_3_.regValue= 0;
        srnn_wdsr_res_block_2_bias_2_3_.resblk_2_layer3_bias_3 = reg_ai_sr_body_1_1x3_bias_3;
        srnn_wdsr_res_block_2_bias_2_3_.resblk_2_layer3_bias_2 = reg_ai_sr_body_1_1x3_bias_2;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_2_BIAS_2_3_reg,srnn_wdsr_res_block_2_bias_2_3_.regValue);

        srnn_wdsr_res_block_2_bias_4_5_.regValue= 0;
        srnn_wdsr_res_block_2_bias_4_5_.resblk_2_layer3_bias_5 = reg_ai_sr_body_1_1x3_bias_5;
        srnn_wdsr_res_block_2_bias_4_5_.resblk_2_layer3_bias_4 = reg_ai_sr_body_1_1x3_bias_4;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_2_BIAS_4_5_reg,srnn_wdsr_res_block_2_bias_4_5_.regValue);

        srnn_wdsr_res_block_2_bias_6_7_.regValue= 0;
        srnn_wdsr_res_block_2_bias_6_7_.resblk_2_layer3_bias_7 = reg_ai_sr_body_1_1x3_bias_7;
        srnn_wdsr_res_block_2_bias_6_7_.resblk_2_layer3_bias_6 = reg_ai_sr_body_1_1x3_bias_6;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_2_BIAS_6_7_reg,srnn_wdsr_res_block_2_bias_6_7_.regValue);

        srnn_wdsr_res_block_3_bias_0_1_.regValue= 0;
        srnn_wdsr_res_block_3_bias_0_1_.resblk_3_layer3_bias_1 = reg_ai_sr_body_2_1x3_bias_1;
        srnn_wdsr_res_block_3_bias_0_1_.resblk_3_layer3_bias_0 = reg_ai_sr_body_2_1x3_bias_0;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_3_BIAS_0_1_reg,srnn_wdsr_res_block_3_bias_0_1_.regValue);

        srnn_wdsr_res_block_3_bias_2_3_.regValue= 0;
        srnn_wdsr_res_block_3_bias_2_3_.resblk_3_layer3_bias_3 = reg_ai_sr_body_2_1x3_bias_3;
        srnn_wdsr_res_block_3_bias_2_3_.resblk_3_layer3_bias_2 = reg_ai_sr_body_2_1x3_bias_2;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_3_BIAS_2_3_reg,srnn_wdsr_res_block_3_bias_2_3_.regValue);

        srnn_wdsr_res_block_3_bias_4_5_.regValue= 0;
        srnn_wdsr_res_block_3_bias_4_5_.resblk_3_layer3_bias_5 = reg_ai_sr_body_2_1x3_bias_5;
        srnn_wdsr_res_block_3_bias_4_5_.resblk_3_layer3_bias_4 = reg_ai_sr_body_2_1x3_bias_4;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_3_BIAS_4_5_reg,srnn_wdsr_res_block_3_bias_4_5_.regValue);
        
        srnn_wdsr_res_block_3_bias_6_7_.regValue= 0;
        srnn_wdsr_res_block_3_bias_6_7_.resblk_3_layer3_bias_7 = reg_ai_sr_body_2_1x3_bias_7;
        srnn_wdsr_res_block_3_bias_6_7_.resblk_3_layer3_bias_6 = reg_ai_sr_body_2_1x3_bias_6;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_3_BIAS_6_7_reg,srnn_wdsr_res_block_3_bias_6_7_.regValue);

        srnn_wdsr_res_block_4_bias_0_1_.regValue= 0;
        srnn_wdsr_res_block_4_bias_0_1_.resblk_4_layer3_bias_1 = reg_ai_sr_body_3_1x3_bias_1;
        srnn_wdsr_res_block_4_bias_0_1_.resblk_4_layer3_bias_0 = reg_ai_sr_body_3_1x3_bias_0;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_4_BIAS_0_1_reg,srnn_wdsr_res_block_4_bias_0_1_.regValue);

        srnn_wdsr_res_block_4_bias_2_3_.regValue= 0;
        srnn_wdsr_res_block_4_bias_2_3_.resblk_4_layer3_bias_3 = reg_ai_sr_body_3_1x3_bias_3;
        srnn_wdsr_res_block_4_bias_2_3_.resblk_4_layer3_bias_2 = reg_ai_sr_body_3_1x3_bias_2;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_4_BIAS_2_3_reg,srnn_wdsr_res_block_4_bias_2_3_.regValue);

        srnn_wdsr_res_block_4_bias_4_5_.regValue= 0;
        srnn_wdsr_res_block_4_bias_4_5_.resblk_4_layer3_bias_5 = reg_ai_sr_body_3_1x3_bias_5;
        srnn_wdsr_res_block_4_bias_4_5_.resblk_4_layer3_bias_4 = reg_ai_sr_body_3_1x3_bias_4;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_4_BIAS_4_5_reg,srnn_wdsr_res_block_4_bias_4_5_.regValue);

        srnn_wdsr_res_block_4_bias_6_7_.regValue= 0;
        srnn_wdsr_res_block_4_bias_6_7_.resblk_4_layer3_bias_7 = reg_ai_sr_body_3_1x3_bias_7;
        srnn_wdsr_res_block_4_bias_6_7_.resblk_4_layer3_bias_6 = reg_ai_sr_body_3_1x3_bias_6;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_4_BIAS_6_7_reg,srnn_wdsr_res_block_4_bias_6_7_.regValue);

        srnn_wdsr_res_block_5_bias_0_1_.regValue= 0;
        srnn_wdsr_res_block_5_bias_0_1_.resblk_5_layer3_bias_1 = reg_ai_sr_body_4_1x3_bias_1;
        srnn_wdsr_res_block_5_bias_0_1_.resblk_5_layer3_bias_0 = reg_ai_sr_body_4_1x3_bias_0;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_5_BIAS_0_1_reg,srnn_wdsr_res_block_5_bias_0_1_.regValue);

        srnn_wdsr_res_block_5_bias_2_3_.regValue= 0;
        srnn_wdsr_res_block_5_bias_2_3_.resblk_5_layer3_bias_3 = reg_ai_sr_body_4_1x3_bias_3;
        srnn_wdsr_res_block_5_bias_2_3_.resblk_5_layer3_bias_2 = reg_ai_sr_body_4_1x3_bias_2;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_5_BIAS_2_3_reg,srnn_wdsr_res_block_5_bias_2_3_.regValue);

        srnn_wdsr_res_block_5_bias_4_5_.regValue= 0;
        srnn_wdsr_res_block_5_bias_4_5_.resblk_5_layer3_bias_5 = reg_ai_sr_body_4_1x3_bias_5;
        srnn_wdsr_res_block_5_bias_4_5_.resblk_5_layer3_bias_4 = reg_ai_sr_body_4_1x3_bias_4;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_5_BIAS_4_5_reg,srnn_wdsr_res_block_5_bias_4_5_.regValue);

        srnn_wdsr_res_block_5_bias_6_7_.regValue= 0;
        srnn_wdsr_res_block_5_bias_6_7_.resblk_5_layer3_bias_7 = reg_ai_sr_body_4_1x3_bias_7;
        srnn_wdsr_res_block_5_bias_6_7_.resblk_5_layer3_bias_6 = reg_ai_sr_body_4_1x3_bias_6;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_5_BIAS_6_7_reg,srnn_wdsr_res_block_5_bias_6_7_.regValue);

        srnn_wdsr_res_block_6_bias_0_1_.regValue= 0;
        srnn_wdsr_res_block_6_bias_0_1_.resblk_6_layer3_bias_1 = reg_ai_sr_body_5_1x3_bias_1;
        srnn_wdsr_res_block_6_bias_0_1_.resblk_6_layer3_bias_0 = reg_ai_sr_body_5_1x3_bias_0;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_6_BIAS_0_1_reg,srnn_wdsr_res_block_6_bias_0_1_.regValue);

        srnn_wdsr_res_block_6_bias_2_3_.regValue= 0;
        srnn_wdsr_res_block_6_bias_2_3_.resblk_6_layer3_bias_3 = reg_ai_sr_body_5_1x3_bias_3;
        srnn_wdsr_res_block_6_bias_2_3_.resblk_6_layer3_bias_2 = reg_ai_sr_body_5_1x3_bias_2;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_6_BIAS_2_3_reg,srnn_wdsr_res_block_6_bias_2_3_.regValue);

        srnn_wdsr_res_block_6_bias_4_5_.regValue= 0;
        srnn_wdsr_res_block_6_bias_4_5_.resblk_6_layer3_bias_5 = reg_ai_sr_body_5_1x3_bias_5;
        srnn_wdsr_res_block_6_bias_4_5_.resblk_6_layer3_bias_4 = reg_ai_sr_body_5_1x3_bias_4;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_6_BIAS_4_5_reg,srnn_wdsr_res_block_6_bias_4_5_.regValue);


        srnn_wdsr_res_block_6_bias_6_7_.regValue= 0;
        srnn_wdsr_res_block_6_bias_6_7_.resblk_6_layer3_bias_7 = reg_ai_sr_body_5_1x3_bias_7;
        srnn_wdsr_res_block_6_bias_6_7_.resblk_6_layer3_bias_6 = reg_ai_sr_body_5_1x3_bias_6;
        IoReg_Write32(SRNN_WDSR_RES_BLOCK_6_BIAS_6_7_reg,srnn_wdsr_res_block_6_bias_6_7_.regValue);

        srnn_wdsr_tail_layer_bias_0_1_.regValue= 0;
        srnn_wdsr_tail_layer_bias_0_1_.tail_layer_bias_1 = reg_ai_sr_tail_bias_1;
        srnn_wdsr_tail_layer_bias_0_1_.tail_layer_bias_0 = reg_ai_sr_tail_bias_0;
        IoReg_Write32(SRNN_WDSR_TAIL_LAYER_BIAS_0_1_reg,srnn_wdsr_tail_layer_bias_0_1_.regValue);

        srnn_wdsr_tail_layer_bias_2_3_.regValue= 0;
        srnn_wdsr_tail_layer_bias_2_3_.tail_layer_bias_3 = reg_ai_sr_tail_bias_3;
        srnn_wdsr_tail_layer_bias_2_3_.tail_layer_bias_2 = reg_ai_sr_tail_bias_2;
        IoReg_Write32(SRNN_WDSR_TAIL_LAYER_BIAS_2_3_reg,srnn_wdsr_tail_layer_bias_2_3_.regValue);

        srnn_wdsr_tail_layer_bias_4_5_.regValue= 0;
        srnn_wdsr_tail_layer_bias_4_5_.tail_layer_bias_5 = reg_ai_sr_tail_bias_5;
        srnn_wdsr_tail_layer_bias_4_5_.tail_layer_bias_4 = reg_ai_sr_tail_bias_4;
        IoReg_Write32(SRNN_WDSR_TAIL_LAYER_BIAS_4_5_reg,srnn_wdsr_tail_layer_bias_4_5_.regValue);

        srnn_wdsr_tail_layer_bias_6_7_.regValue= 0;
        srnn_wdsr_tail_layer_bias_6_7_.tail_layer_bias_7 = reg_ai_sr_tail_bias_7;
        srnn_wdsr_tail_layer_bias_6_7_.tail_layer_bias_6 = reg_ai_sr_tail_bias_6;
        IoReg_Write32(SRNN_WDSR_TAIL_LAYER_BIAS_6_7_reg,srnn_wdsr_tail_layer_bias_6_7_.regValue);

        srnn_wdsr_tail_layer_bias_8_9_.regValue= 0;
        srnn_wdsr_tail_layer_bias_8_9_.tail_layer_bias_9 = reg_ai_sr_tail_bias_9;
        srnn_wdsr_tail_layer_bias_8_9_.tail_layer_bias_8 = reg_ai_sr_tail_bias_8;
        IoReg_Write32(SRNN_WDSR_TAIL_LAYER_BIAS_8_9_reg,srnn_wdsr_tail_layer_bias_8_9_.regValue);

        srnn_wdsr_tail_layer_bias_10_11_.regValue= 0;
        srnn_wdsr_tail_layer_bias_10_11_.tail_layer_bias_11 = reg_ai_sr_tail_bias_11;
        srnn_wdsr_tail_layer_bias_10_11_.tail_layer_bias_10 = reg_ai_sr_tail_bias_10;
        IoReg_Write32(SRNN_WDSR_TAIL_LAYER_BIAS_10_11_reg,srnn_wdsr_tail_layer_bias_10_11_.regValue);

        srnn_wdsr_tail_layer_bias_12_13_.regValue= 0;
        srnn_wdsr_tail_layer_bias_12_13_.tail_layer_bias_13 = reg_ai_sr_tail_bias_13;
        srnn_wdsr_tail_layer_bias_12_13_.tail_layer_bias_12 = reg_ai_sr_tail_bias_12;
        IoReg_Write32(SRNN_WDSR_TAIL_LAYER_BIAS_12_13_reg,srnn_wdsr_tail_layer_bias_12_13_.regValue);

        srnn_wdsr_tail_layer_bias_14_15_.regValue= 0;
        srnn_wdsr_tail_layer_bias_14_15_.tail_layer_bias_15 = reg_ai_sr_tail_bias_15;
        srnn_wdsr_tail_layer_bias_14_15_.tail_layer_bias_14 = reg_ai_sr_tail_bias_14;
        IoReg_Write32(SRNN_WDSR_TAIL_LAYER_BIAS_14_15_reg,srnn_wdsr_tail_layer_bias_14_15_.regValue);


}
void drvif_srnn_write_bias(unsigned char ai_sr_mode, unsigned short *param){
    if(ai_sr_mode==2){
        reg_ai_sr_head_bias_0 = param[72] ;
        reg_ai_sr_head_bias_1 = param[73] ;
        reg_ai_sr_head_bias_2 = param[74] ;
        reg_ai_sr_head_bias_3 = param[75] ;
        reg_ai_sr_head_bias_4 = param[76] ;
        reg_ai_sr_head_bias_5 = param[77] ;
        reg_ai_sr_head_bias_6 = param[78] ;
        reg_ai_sr_head_bias_7 = param[79] ;
        reg_ai_sr_body_0_1x3_bias_0 = param[2408] ;
        reg_ai_sr_body_0_1x3_bias_1 = param[2409] ;
        reg_ai_sr_body_0_1x3_bias_2 = param[2410] ;
        reg_ai_sr_body_0_1x3_bias_3 = param[2411] ;
        reg_ai_sr_body_0_1x3_bias_4 = param[2412] ;
        reg_ai_sr_body_0_1x3_bias_5 = param[2413] ;
        reg_ai_sr_body_0_1x3_bias_6 = param[2414] ;
        reg_ai_sr_body_0_1x3_bias_7 = param[2415] ;
        reg_ai_sr_body_2_1x3_bias_0 = param[4744] ;
        reg_ai_sr_body_2_1x3_bias_1 = param[4745] ;
        reg_ai_sr_body_2_1x3_bias_2 = param[4746] ;
        reg_ai_sr_body_2_1x3_bias_3 = param[4747] ;
        reg_ai_sr_body_2_1x3_bias_4 = param[4748] ;
        reg_ai_sr_body_2_1x3_bias_5 = param[4749] ;
        reg_ai_sr_body_2_1x3_bias_6 = param[4750] ;
        reg_ai_sr_body_2_1x3_bias_7 = param[4751] ;
        reg_ai_sr_body_4_1x3_bias_0 = param[7080] ;
        reg_ai_sr_body_4_1x3_bias_1 = param[7081] ;
        reg_ai_sr_body_4_1x3_bias_2 = param[7082] ;
        reg_ai_sr_body_4_1x3_bias_3 = param[7083] ;
        reg_ai_sr_body_4_1x3_bias_4 = param[7084] ;
        reg_ai_sr_body_4_1x3_bias_5 = param[7085] ;
        reg_ai_sr_body_4_1x3_bias_6 = param[7086] ;
        reg_ai_sr_body_4_1x3_bias_7 = param[7087] ;
        reg_ai_sr_body_1_1x3_bias_0 = param[2408] ;
        reg_ai_sr_body_1_1x3_bias_1 = param[2409] ;
        reg_ai_sr_body_1_1x3_bias_2 = param[2410] ;
        reg_ai_sr_body_1_1x3_bias_3 = param[2411] ;
        reg_ai_sr_body_1_1x3_bias_4 = param[2412] ;
        reg_ai_sr_body_1_1x3_bias_5 = param[2413] ;
        reg_ai_sr_body_1_1x3_bias_6 = param[2414] ;
        reg_ai_sr_body_1_1x3_bias_7 = param[2415] ;
        reg_ai_sr_body_3_1x3_bias_0 = param[4744] ;
        reg_ai_sr_body_3_1x3_bias_1 = param[4745] ;
        reg_ai_sr_body_3_1x3_bias_2 = param[4746] ;
        reg_ai_sr_body_3_1x3_bias_3 = param[4747] ;
        reg_ai_sr_body_3_1x3_bias_4 = param[4748] ;
        reg_ai_sr_body_3_1x3_bias_5 = param[4749] ;
        reg_ai_sr_body_3_1x3_bias_6 = param[4750] ;
        reg_ai_sr_body_3_1x3_bias_7 = param[4751] ;
        reg_ai_sr_body_5_1x3_bias_0 = param[7080] ;
        reg_ai_sr_body_5_1x3_bias_1 = param[7081] ;
        reg_ai_sr_body_5_1x3_bias_2 = param[7082] ;
        reg_ai_sr_body_5_1x3_bias_3 = param[7083] ;
        reg_ai_sr_body_5_1x3_bias_4 = param[7084] ;
        reg_ai_sr_body_5_1x3_bias_5 = param[7085] ;
        reg_ai_sr_body_5_1x3_bias_6 = param[7086] ;
        reg_ai_sr_body_5_1x3_bias_7 = param[7087] ;
        reg_ai_sr_tail_bias_0 = param[7376] ;
        reg_ai_sr_tail_bias_1 = param[7377] ;
        reg_ai_sr_tail_bias_2 = param[7378] ;
        reg_ai_sr_tail_bias_3 = param[7379] ;
    }
    else if(ai_sr_mode==3||ai_sr_mode==4){
        reg_ai_sr_head_bias_0 = param[72] ;
        reg_ai_sr_head_bias_1 = param[73] ;
        reg_ai_sr_head_bias_2 = param[74] ;
        reg_ai_sr_head_bias_3 = param[75] ;
        reg_ai_sr_head_bias_4 = param[76] ;
        reg_ai_sr_head_bias_5 = param[77] ;
        reg_ai_sr_head_bias_6 = param[78] ;
        reg_ai_sr_head_bias_7 = param[79] ;
        reg_ai_sr_body_0_1x3_bias_0 = param[4736] ;
        reg_ai_sr_body_0_1x3_bias_1 = param[4737] ;
        reg_ai_sr_body_0_1x3_bias_2 = param[4738] ;
        reg_ai_sr_body_0_1x3_bias_3 = param[4739] ;
        reg_ai_sr_body_0_1x3_bias_4 = param[4740] ;
        reg_ai_sr_body_0_1x3_bias_5 = param[4741] ;
        reg_ai_sr_body_0_1x3_bias_6 = param[4742] ;
        reg_ai_sr_body_0_1x3_bias_7 = param[4743] ;
        reg_ai_sr_body_2_1x3_bias_0 = param[9400] ;
        reg_ai_sr_body_2_1x3_bias_1 = param[9401] ;
        reg_ai_sr_body_2_1x3_bias_2 = param[9402] ;
        reg_ai_sr_body_2_1x3_bias_3 = param[9403] ;
        reg_ai_sr_body_2_1x3_bias_4 = param[9404] ;
        reg_ai_sr_body_2_1x3_bias_5 = param[9405] ;
        reg_ai_sr_body_2_1x3_bias_6 = param[9406] ;
        reg_ai_sr_body_2_1x3_bias_7 = param[9407] ;
        reg_ai_sr_body_4_1x3_bias_0 = param[14064] ;
        reg_ai_sr_body_4_1x3_bias_1 = param[14065] ;
        reg_ai_sr_body_4_1x3_bias_2 = param[14066] ;
        reg_ai_sr_body_4_1x3_bias_3 = param[14067] ;
        reg_ai_sr_body_4_1x3_bias_4 = param[14068] ;
        reg_ai_sr_body_4_1x3_bias_5 = param[14069] ;
        reg_ai_sr_body_4_1x3_bias_6 = param[14070] ;
        reg_ai_sr_body_4_1x3_bias_7 = param[14071] ;
        reg_ai_sr_body_1_1x3_bias_0 = param[4736] ;
        reg_ai_sr_body_1_1x3_bias_1 = param[4737] ;
        reg_ai_sr_body_1_1x3_bias_2 = param[4738] ;
        reg_ai_sr_body_1_1x3_bias_3 = param[4739] ;
        reg_ai_sr_body_1_1x3_bias_4 = param[4740] ;
        reg_ai_sr_body_1_1x3_bias_5 = param[4741] ;
        reg_ai_sr_body_1_1x3_bias_6 = param[4742] ;
        reg_ai_sr_body_1_1x3_bias_7 = param[4743] ;
        reg_ai_sr_body_3_1x3_bias_0 = param[9400] ;
        reg_ai_sr_body_3_1x3_bias_1 = param[9401] ;
        reg_ai_sr_body_3_1x3_bias_2 = param[9402] ;
        reg_ai_sr_body_3_1x3_bias_3 = param[9403] ;
        reg_ai_sr_body_3_1x3_bias_4 = param[9404] ;
        reg_ai_sr_body_3_1x3_bias_5 = param[9405] ;
        reg_ai_sr_body_3_1x3_bias_6 = param[9406] ;
        reg_ai_sr_body_3_1x3_bias_7 = param[9407] ;
        reg_ai_sr_body_5_1x3_bias_0 = param[14064] ;
        reg_ai_sr_body_5_1x3_bias_1 = param[14065] ;
        reg_ai_sr_body_5_1x3_bias_2 = param[14066] ;
        reg_ai_sr_body_5_1x3_bias_3 = param[14067] ;
        reg_ai_sr_body_5_1x3_bias_4 = param[14068] ;
        reg_ai_sr_body_5_1x3_bias_5 = param[14069] ;
        reg_ai_sr_body_5_1x3_bias_6 = param[14070] ;
        reg_ai_sr_body_5_1x3_bias_7 = param[14071] ;
        reg_ai_sr_tail_bias_0 = param[14360] ;
        reg_ai_sr_tail_bias_1 = param[14361] ;
        reg_ai_sr_tail_bias_2 = param[14362] ;
        reg_ai_sr_tail_bias_3 = param[14363] ;
    }
    else if(ai_sr_mode==5){
        reg_ai_sr_head_bias_0 = param[72] ;
        reg_ai_sr_head_bias_1 = param[73] ;
        reg_ai_sr_head_bias_2 = param[74] ;
        reg_ai_sr_head_bias_3 = param[75] ;
        reg_ai_sr_head_bias_4 = param[76] ;
        reg_ai_sr_head_bias_5 = param[77] ;
        reg_ai_sr_head_bias_6 = param[78] ;
        reg_ai_sr_head_bias_7 = param[79] ;
        reg_ai_sr_body_0_1x3_bias_0 = param[4736] ;
        reg_ai_sr_body_0_1x3_bias_1 = param[4737] ;
        reg_ai_sr_body_0_1x3_bias_2 = param[4738] ;
        reg_ai_sr_body_0_1x3_bias_3 = param[4739] ;
        reg_ai_sr_body_0_1x3_bias_4 = param[4740] ;
        reg_ai_sr_body_0_1x3_bias_5 = param[4741] ;
        reg_ai_sr_body_0_1x3_bias_6 = param[4742] ;
        reg_ai_sr_body_0_1x3_bias_7 = param[4743] ;
        reg_ai_sr_body_2_1x3_bias_0 = param[9400] ;
        reg_ai_sr_body_2_1x3_bias_1 = param[9401] ;
        reg_ai_sr_body_2_1x3_bias_2 = param[9402] ;
        reg_ai_sr_body_2_1x3_bias_3 = param[9403] ;
        reg_ai_sr_body_2_1x3_bias_4 = param[9404] ;
        reg_ai_sr_body_2_1x3_bias_5 = param[9405] ;
        reg_ai_sr_body_2_1x3_bias_6 = param[9406] ;
        reg_ai_sr_body_2_1x3_bias_7 = param[9407] ;
        reg_ai_sr_body_4_1x3_bias_0 = param[14064] ;
        reg_ai_sr_body_4_1x3_bias_1 = param[14065] ;
        reg_ai_sr_body_4_1x3_bias_2 = param[14066] ;
        reg_ai_sr_body_4_1x3_bias_3 = param[14067] ;
        reg_ai_sr_body_4_1x3_bias_4 = param[14068] ;
        reg_ai_sr_body_4_1x3_bias_5 = param[14069] ;
        reg_ai_sr_body_4_1x3_bias_6 = param[14070] ;
        reg_ai_sr_body_4_1x3_bias_7 = param[14071] ;
        reg_ai_sr_body_1_1x3_bias_0 = param[4736] ;
        reg_ai_sr_body_1_1x3_bias_1 = param[4737] ;
        reg_ai_sr_body_1_1x3_bias_2 = param[4738] ;
        reg_ai_sr_body_1_1x3_bias_3 = param[4739] ;
        reg_ai_sr_body_1_1x3_bias_4 = param[4740] ;
        reg_ai_sr_body_1_1x3_bias_5 = param[4741] ;
        reg_ai_sr_body_1_1x3_bias_6 = param[4742] ;
        reg_ai_sr_body_1_1x3_bias_7 = param[4743] ;
        reg_ai_sr_body_3_1x3_bias_0 = param[9400] ;
        reg_ai_sr_body_3_1x3_bias_1 = param[9401] ;
        reg_ai_sr_body_3_1x3_bias_2 = param[9402] ;
        reg_ai_sr_body_3_1x3_bias_3 = param[9403] ;
        reg_ai_sr_body_3_1x3_bias_4 = param[9404] ;
        reg_ai_sr_body_3_1x3_bias_5 = param[9405] ;
        reg_ai_sr_body_3_1x3_bias_6 = param[9406] ;
        reg_ai_sr_body_3_1x3_bias_7 = param[9407] ;
        reg_ai_sr_body_5_1x3_bias_0 = param[14064] ;
        reg_ai_sr_body_5_1x3_bias_1 = param[14065] ;
        reg_ai_sr_body_5_1x3_bias_2 = param[14066] ;
        reg_ai_sr_body_5_1x3_bias_3 = param[14067] ;
        reg_ai_sr_body_5_1x3_bias_4 = param[14068] ;
        reg_ai_sr_body_5_1x3_bias_5 = param[14069] ;
        reg_ai_sr_body_5_1x3_bias_6 = param[14070] ;
        reg_ai_sr_body_5_1x3_bias_7 = param[14071] ;
        reg_ai_sr_tail_bias_0 = param[14720] ;
        reg_ai_sr_tail_bias_1 = param[14721] ;
        reg_ai_sr_tail_bias_2 = param[14722] ;
        reg_ai_sr_tail_bias_3 = param[14723] ;
        reg_ai_sr_tail_bias_4 = param[14724] ;
        reg_ai_sr_tail_bias_5 = param[14725] ;
        reg_ai_sr_tail_bias_6 = param[14726] ;
        reg_ai_sr_tail_bias_7 = param[14727] ;
        reg_ai_sr_tail_bias_8 = param[14728] ;
    }
    else if(ai_sr_mode==6){
        reg_ai_sr_head_bias_0 = param[72] ;
        reg_ai_sr_head_bias_1 = param[73] ;
        reg_ai_sr_head_bias_2 = param[74] ;
        reg_ai_sr_head_bias_3 = param[75] ;
        reg_ai_sr_head_bias_4 = param[76] ;
        reg_ai_sr_head_bias_5 = param[77] ;
        reg_ai_sr_head_bias_6 = param[78] ;
        reg_ai_sr_head_bias_7 = param[79] ;
        reg_ai_sr_body_0_1x3_bias_0 = param[4736] ;
        reg_ai_sr_body_0_1x3_bias_1 = param[4737] ;
        reg_ai_sr_body_0_1x3_bias_2 = param[4738] ;
        reg_ai_sr_body_0_1x3_bias_3 = param[4739] ;
        reg_ai_sr_body_0_1x3_bias_4 = param[4740] ;
        reg_ai_sr_body_0_1x3_bias_5 = param[4741] ;
        reg_ai_sr_body_0_1x3_bias_6 = param[4742] ;
        reg_ai_sr_body_0_1x3_bias_7 = param[4743] ;
        reg_ai_sr_body_1_1x3_bias_0 = param[9400] ;
        reg_ai_sr_body_1_1x3_bias_1 = param[9401] ;
        reg_ai_sr_body_1_1x3_bias_2 = param[9402] ;
        reg_ai_sr_body_1_1x3_bias_3 = param[9403] ;
        reg_ai_sr_body_1_1x3_bias_4 = param[9404] ;
        reg_ai_sr_body_1_1x3_bias_5 = param[9405] ;
        reg_ai_sr_body_1_1x3_bias_6 = param[9406] ;
        reg_ai_sr_body_1_1x3_bias_7 = param[9407] ;
        reg_ai_sr_body_2_1x3_bias_0 = param[14064] ;
        reg_ai_sr_body_2_1x3_bias_1 = param[14065] ;
        reg_ai_sr_body_2_1x3_bias_2 = param[14066] ;
        reg_ai_sr_body_2_1x3_bias_3 = param[14067] ;
        reg_ai_sr_body_2_1x3_bias_4 = param[14068] ;
        reg_ai_sr_body_2_1x3_bias_5 = param[14069] ;
        reg_ai_sr_body_2_1x3_bias_6 = param[14070] ;
        reg_ai_sr_body_2_1x3_bias_7 = param[14071] ;
        reg_ai_sr_body_3_1x3_bias_0 = param[18728] ;
        reg_ai_sr_body_3_1x3_bias_1 = param[18729] ;
        reg_ai_sr_body_3_1x3_bias_2 = param[18730] ;
        reg_ai_sr_body_3_1x3_bias_3 = param[18731] ;
        reg_ai_sr_body_3_1x3_bias_4 = param[18732] ;
        reg_ai_sr_body_3_1x3_bias_5 = param[18733] ;
        reg_ai_sr_body_3_1x3_bias_6 = param[18734] ;
        reg_ai_sr_body_3_1x3_bias_7 = param[18735] ;
        reg_ai_sr_body_4_1x3_bias_0 = param[23392] ;
        reg_ai_sr_body_4_1x3_bias_1 = param[23393] ;
        reg_ai_sr_body_4_1x3_bias_2 = param[23394] ;
        reg_ai_sr_body_4_1x3_bias_3 = param[23395] ;
        reg_ai_sr_body_4_1x3_bias_4 = param[23396] ;
        reg_ai_sr_body_4_1x3_bias_5 = param[23397] ;
        reg_ai_sr_body_4_1x3_bias_6 = param[23398] ;
        reg_ai_sr_body_4_1x3_bias_7 = param[23399] ;
        reg_ai_sr_body_5_1x3_bias_0 = param[28056] ;
        reg_ai_sr_body_5_1x3_bias_1 = param[28057] ;
        reg_ai_sr_body_5_1x3_bias_2 = param[28058] ;
        reg_ai_sr_body_5_1x3_bias_3 = param[28059] ;
        reg_ai_sr_body_5_1x3_bias_4 = param[28060] ;
        reg_ai_sr_body_5_1x3_bias_5 = param[28061] ;
        reg_ai_sr_body_5_1x3_bias_6 = param[28062] ;
        reg_ai_sr_body_5_1x3_bias_7 = param[28063] ;
        reg_ai_sr_tail_bias_0 = param[28352] ;
        reg_ai_sr_tail_bias_1 = param[28353] ;
        reg_ai_sr_tail_bias_2 = param[28354] ;
        reg_ai_sr_tail_bias_3 = param[28355] ;
    }
    else if(ai_sr_mode==7){
        reg_ai_sr_head_bias_0 = param[72] ;
        reg_ai_sr_head_bias_1 = param[73] ;
        reg_ai_sr_head_bias_2 = param[74] ;
        reg_ai_sr_head_bias_3 = param[75] ;
        reg_ai_sr_head_bias_4 = param[76] ;
        reg_ai_sr_head_bias_5 = param[77] ;
        reg_ai_sr_head_bias_6 = param[78] ;
        reg_ai_sr_head_bias_7 = param[79] ;
        reg_ai_sr_body_0_1x3_bias_0 = param[4736] ;
        reg_ai_sr_body_0_1x3_bias_1 = param[4737] ;
        reg_ai_sr_body_0_1x3_bias_2 = param[4738] ;
        reg_ai_sr_body_0_1x3_bias_3 = param[4739] ;
        reg_ai_sr_body_0_1x3_bias_4 = param[4740] ;
        reg_ai_sr_body_0_1x3_bias_5 = param[4741] ;
        reg_ai_sr_body_0_1x3_bias_6 = param[4742] ;
        reg_ai_sr_body_0_1x3_bias_7 = param[4743] ;
        reg_ai_sr_body_1_1x3_bias_0 = param[9400] ;
        reg_ai_sr_body_1_1x3_bias_1 = param[9401] ;
        reg_ai_sr_body_1_1x3_bias_2 = param[9402] ;
        reg_ai_sr_body_1_1x3_bias_3 = param[9403] ;
        reg_ai_sr_body_1_1x3_bias_4 = param[9404] ;
        reg_ai_sr_body_1_1x3_bias_5 = param[9405] ;
        reg_ai_sr_body_1_1x3_bias_6 = param[9406] ;
        reg_ai_sr_body_1_1x3_bias_7 = param[9407] ;
        reg_ai_sr_body_2_1x3_bias_0 = param[14064] ;
        reg_ai_sr_body_2_1x3_bias_1 = param[14065] ;
        reg_ai_sr_body_2_1x3_bias_2 = param[14066] ;
        reg_ai_sr_body_2_1x3_bias_3 = param[14067] ;
        reg_ai_sr_body_2_1x3_bias_4 = param[14068] ;
        reg_ai_sr_body_2_1x3_bias_5 = param[14069] ;
        reg_ai_sr_body_2_1x3_bias_6 = param[14070] ;
        reg_ai_sr_body_2_1x3_bias_7 = param[14071] ;
        reg_ai_sr_body_3_1x3_bias_0 = param[18728] ;
        reg_ai_sr_body_3_1x3_bias_1 = param[18729] ;
        reg_ai_sr_body_3_1x3_bias_2 = param[18730] ;
        reg_ai_sr_body_3_1x3_bias_3 = param[18731] ;
        reg_ai_sr_body_3_1x3_bias_4 = param[18732] ;
        reg_ai_sr_body_3_1x3_bias_5 = param[18733] ;
        reg_ai_sr_body_3_1x3_bias_6 = param[18734] ;
        reg_ai_sr_body_3_1x3_bias_7 = param[18735] ;
        reg_ai_sr_body_4_1x3_bias_0 = param[23392] ;
        reg_ai_sr_body_4_1x3_bias_1 = param[23393] ;
        reg_ai_sr_body_4_1x3_bias_2 = param[23394] ;
        reg_ai_sr_body_4_1x3_bias_3 = param[23395] ;
        reg_ai_sr_body_4_1x3_bias_4 = param[23396] ;
        reg_ai_sr_body_4_1x3_bias_5 = param[23397] ;
        reg_ai_sr_body_4_1x3_bias_6 = param[23398] ;
        reg_ai_sr_body_4_1x3_bias_7 = param[23399] ;
        reg_ai_sr_body_5_1x3_bias_0 = param[28056] ;
        reg_ai_sr_body_5_1x3_bias_1 = param[28057] ;
        reg_ai_sr_body_5_1x3_bias_2 = param[28058] ;
        reg_ai_sr_body_5_1x3_bias_3 = param[28059] ;
        reg_ai_sr_body_5_1x3_bias_4 = param[28060] ;
        reg_ai_sr_body_5_1x3_bias_5 = param[28061] ;
        reg_ai_sr_body_5_1x3_bias_6 = param[28062] ;
        reg_ai_sr_body_5_1x3_bias_7 = param[28063] ;
        reg_ai_sr_tail_bias_0 = param[29216] ;
        reg_ai_sr_tail_bias_1 = param[29217] ;
        reg_ai_sr_tail_bias_2 = param[29218] ;
        reg_ai_sr_tail_bias_3 = param[29219] ;
        reg_ai_sr_tail_bias_4 = param[29220] ;
        reg_ai_sr_tail_bias_5 = param[29221] ;
        reg_ai_sr_tail_bias_6 = param[29222] ;
        reg_ai_sr_tail_bias_7 = param[29223] ;
        reg_ai_sr_tail_bias_8 = param[29224] ;
        reg_ai_sr_tail_bias_9 = param[29225] ;
        reg_ai_sr_tail_bias_10 = param[29226] ;
        reg_ai_sr_tail_bias_11 = param[29227] ;
        reg_ai_sr_tail_bias_12 = param[29228] ;
        reg_ai_sr_tail_bias_13 = param[29229] ;
        reg_ai_sr_tail_bias_14 = param[29230] ;
        reg_ai_sr_tail_bias_15 = param[29231] ;
    }
    drvif_bias_rbus_set();
}
void frvif_bias_rubs_read(void){
        srnn_wdsr_head_layer_bias_0_1_RBUS srnn_wdsr_head_layer_bias_0_1_;
        srnn_wdsr_head_layer_bias_2_3_RBUS srnn_wdsr_head_layer_bias_2_3_;
        srnn_wdsr_head_layer_bias_4_5_RBUS srnn_wdsr_head_layer_bias_4_5_;
        srnn_wdsr_head_layer_bias_6_7_RBUS srnn_wdsr_head_layer_bias_6_7_;
        srnn_wdsr_res_block_1_bias_0_1_RBUS srnn_wdsr_res_block_1_bias_0_1_;
        srnn_wdsr_res_block_1_bias_2_3_RBUS srnn_wdsr_res_block_1_bias_2_3_;
        srnn_wdsr_res_block_1_bias_4_5_RBUS srnn_wdsr_res_block_1_bias_4_5_;
        srnn_wdsr_res_block_1_bias_6_7_RBUS srnn_wdsr_res_block_1_bias_6_7_;
        srnn_wdsr_res_block_2_bias_0_1_RBUS srnn_wdsr_res_block_2_bias_0_1_;
        srnn_wdsr_res_block_2_bias_2_3_RBUS srnn_wdsr_res_block_2_bias_2_3_;
        srnn_wdsr_res_block_2_bias_4_5_RBUS srnn_wdsr_res_block_2_bias_4_5_;
        srnn_wdsr_res_block_2_bias_6_7_RBUS srnn_wdsr_res_block_2_bias_6_7_;
        srnn_wdsr_res_block_3_bias_0_1_RBUS srnn_wdsr_res_block_3_bias_0_1_;
        srnn_wdsr_res_block_3_bias_2_3_RBUS srnn_wdsr_res_block_3_bias_2_3_;
        srnn_wdsr_res_block_3_bias_4_5_RBUS srnn_wdsr_res_block_3_bias_4_5_;
        srnn_wdsr_res_block_3_bias_6_7_RBUS srnn_wdsr_res_block_3_bias_6_7_;
        srnn_wdsr_res_block_4_bias_0_1_RBUS srnn_wdsr_res_block_4_bias_0_1_;
        srnn_wdsr_res_block_4_bias_2_3_RBUS srnn_wdsr_res_block_4_bias_2_3_;
        srnn_wdsr_res_block_4_bias_4_5_RBUS srnn_wdsr_res_block_4_bias_4_5_;
        srnn_wdsr_res_block_4_bias_6_7_RBUS srnn_wdsr_res_block_4_bias_6_7_;
        srnn_wdsr_res_block_5_bias_0_1_RBUS srnn_wdsr_res_block_5_bias_0_1_;
        srnn_wdsr_res_block_5_bias_2_3_RBUS srnn_wdsr_res_block_5_bias_2_3_;
        srnn_wdsr_res_block_5_bias_4_5_RBUS srnn_wdsr_res_block_5_bias_4_5_;
        srnn_wdsr_res_block_5_bias_6_7_RBUS srnn_wdsr_res_block_5_bias_6_7_;
        srnn_wdsr_res_block_6_bias_0_1_RBUS srnn_wdsr_res_block_6_bias_0_1_;
        srnn_wdsr_res_block_6_bias_2_3_RBUS srnn_wdsr_res_block_6_bias_2_3_;
        srnn_wdsr_res_block_6_bias_4_5_RBUS srnn_wdsr_res_block_6_bias_4_5_;
        srnn_wdsr_res_block_6_bias_6_7_RBUS srnn_wdsr_res_block_6_bias_6_7_;
        srnn_wdsr_tail_layer_bias_0_1_RBUS srnn_wdsr_tail_layer_bias_0_1_;
        srnn_wdsr_tail_layer_bias_2_3_RBUS srnn_wdsr_tail_layer_bias_2_3_;
        srnn_wdsr_tail_layer_bias_4_5_RBUS srnn_wdsr_tail_layer_bias_4_5_;
        srnn_wdsr_tail_layer_bias_6_7_RBUS srnn_wdsr_tail_layer_bias_6_7_;
        srnn_wdsr_tail_layer_bias_8_9_RBUS srnn_wdsr_tail_layer_bias_8_9_;
        srnn_wdsr_tail_layer_bias_10_11_RBUS srnn_wdsr_tail_layer_bias_10_11_;
        srnn_wdsr_tail_layer_bias_12_13_RBUS srnn_wdsr_tail_layer_bias_12_13_;
        srnn_wdsr_tail_layer_bias_14_15_RBUS srnn_wdsr_tail_layer_bias_14_15_;

        srnn_wdsr_head_layer_bias_0_1_.regValue = IoReg_Read32(SRNN_WDSR_HEAD_LAYER_BIAS_0_1_reg);
        reg_ai_sr_head_bias_1= srnn_wdsr_head_layer_bias_0_1_.head_layer_bias_1;
        reg_ai_sr_head_bias_0= srnn_wdsr_head_layer_bias_0_1_.head_layer_bias_0;

        srnn_wdsr_head_layer_bias_2_3_.regValue = IoReg_Read32(SRNN_WDSR_HEAD_LAYER_BIAS_2_3_reg);
        reg_ai_sr_head_bias_3= srnn_wdsr_head_layer_bias_2_3_.head_layer_bias_3;
        reg_ai_sr_head_bias_2= srnn_wdsr_head_layer_bias_2_3_.head_layer_bias_2;

        srnn_wdsr_head_layer_bias_4_5_.regValue = IoReg_Read32(SRNN_WDSR_HEAD_LAYER_BIAS_4_5_reg);
        reg_ai_sr_head_bias_5= srnn_wdsr_head_layer_bias_4_5_.head_layer_bias_5;
        reg_ai_sr_head_bias_4= srnn_wdsr_head_layer_bias_4_5_.head_layer_bias_4;

        srnn_wdsr_head_layer_bias_6_7_.regValue = IoReg_Read32(SRNN_WDSR_HEAD_LAYER_BIAS_6_7_reg);
        reg_ai_sr_head_bias_7 = srnn_wdsr_head_layer_bias_6_7_.head_layer_bias_7;
        reg_ai_sr_head_bias_6 = srnn_wdsr_head_layer_bias_6_7_.head_layer_bias_6;

        srnn_wdsr_res_block_1_bias_0_1_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_1_BIAS_0_1_reg);
        reg_ai_sr_body_0_1x3_bias_1 = srnn_wdsr_res_block_1_bias_0_1_.resblk_1_layer3_bias_1;
        reg_ai_sr_body_0_1x3_bias_0 = srnn_wdsr_res_block_1_bias_0_1_.resblk_1_layer3_bias_0;

        srnn_wdsr_res_block_1_bias_2_3_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_1_BIAS_2_3_reg);
        reg_ai_sr_body_0_1x3_bias_3 = srnn_wdsr_res_block_1_bias_2_3_.resblk_1_layer3_bias_3;
        reg_ai_sr_body_0_1x3_bias_2 = srnn_wdsr_res_block_1_bias_2_3_.resblk_1_layer3_bias_2;

        srnn_wdsr_res_block_1_bias_4_5_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_1_BIAS_4_5_reg);
        reg_ai_sr_body_0_1x3_bias_5 = srnn_wdsr_res_block_1_bias_4_5_.resblk_1_layer3_bias_5;
        reg_ai_sr_body_0_1x3_bias_4 = srnn_wdsr_res_block_1_bias_4_5_.resblk_1_layer3_bias_4;

        srnn_wdsr_res_block_1_bias_6_7_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_1_BIAS_6_7_reg);
        reg_ai_sr_body_0_1x3_bias_7 = srnn_wdsr_res_block_1_bias_6_7_.resblk_1_layer3_bias_7;
        reg_ai_sr_body_0_1x3_bias_6 = srnn_wdsr_res_block_1_bias_6_7_.resblk_1_layer3_bias_6;

        srnn_wdsr_res_block_2_bias_0_1_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_2_BIAS_0_1_reg);
        reg_ai_sr_body_1_1x3_bias_1 = srnn_wdsr_res_block_2_bias_0_1_.resblk_2_layer3_bias_1;
        reg_ai_sr_body_1_1x3_bias_0 = srnn_wdsr_res_block_2_bias_0_1_.resblk_2_layer3_bias_0;

        srnn_wdsr_res_block_2_bias_2_3_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_2_BIAS_2_3_reg);
        reg_ai_sr_body_1_1x3_bias_3 = srnn_wdsr_res_block_2_bias_2_3_.resblk_2_layer3_bias_3;
        reg_ai_sr_body_1_1x3_bias_2 = srnn_wdsr_res_block_2_bias_2_3_.resblk_2_layer3_bias_2;

        srnn_wdsr_res_block_2_bias_4_5_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_2_BIAS_4_5_reg);
        reg_ai_sr_body_1_1x3_bias_5 = srnn_wdsr_res_block_2_bias_4_5_.resblk_2_layer3_bias_5;
        reg_ai_sr_body_1_1x3_bias_4 = srnn_wdsr_res_block_2_bias_4_5_.resblk_2_layer3_bias_4;

        srnn_wdsr_res_block_2_bias_6_7_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_2_BIAS_6_7_reg);
        reg_ai_sr_body_1_1x3_bias_7 = srnn_wdsr_res_block_2_bias_6_7_.resblk_2_layer3_bias_7;
        reg_ai_sr_body_1_1x3_bias_6 = srnn_wdsr_res_block_2_bias_6_7_.resblk_2_layer3_bias_6;

        srnn_wdsr_res_block_3_bias_0_1_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_3_BIAS_0_1_reg);
        reg_ai_sr_body_2_1x3_bias_1 = srnn_wdsr_res_block_3_bias_0_1_.resblk_3_layer3_bias_1;
        reg_ai_sr_body_2_1x3_bias_0 = srnn_wdsr_res_block_3_bias_0_1_.resblk_3_layer3_bias_0;

        srnn_wdsr_res_block_3_bias_2_3_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_3_BIAS_2_3_reg);
        reg_ai_sr_body_2_1x3_bias_3 = srnn_wdsr_res_block_3_bias_2_3_.resblk_3_layer3_bias_3;
        reg_ai_sr_body_2_1x3_bias_2 = srnn_wdsr_res_block_3_bias_2_3_.resblk_3_layer3_bias_2;

        srnn_wdsr_res_block_3_bias_4_5_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_3_BIAS_4_5_reg);
        reg_ai_sr_body_2_1x3_bias_5 = srnn_wdsr_res_block_3_bias_4_5_.resblk_3_layer3_bias_5;
        reg_ai_sr_body_2_1x3_bias_4 = srnn_wdsr_res_block_3_bias_4_5_.resblk_3_layer3_bias_4;

        srnn_wdsr_res_block_3_bias_6_7_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_3_BIAS_6_7_reg);
        reg_ai_sr_body_2_1x3_bias_7 = srnn_wdsr_res_block_3_bias_6_7_.resblk_3_layer3_bias_7;
        reg_ai_sr_body_2_1x3_bias_6 = srnn_wdsr_res_block_3_bias_6_7_.resblk_3_layer3_bias_6;

        srnn_wdsr_res_block_4_bias_0_1_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_4_BIAS_0_1_reg);
        reg_ai_sr_body_3_1x3_bias_1 = srnn_wdsr_res_block_4_bias_0_1_.resblk_4_layer3_bias_1;
        reg_ai_sr_body_3_1x3_bias_0 = srnn_wdsr_res_block_4_bias_0_1_.resblk_4_layer3_bias_0;

        srnn_wdsr_res_block_4_bias_2_3_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_4_BIAS_2_3_reg);
        reg_ai_sr_body_3_1x3_bias_3= srnn_wdsr_res_block_4_bias_2_3_.resblk_4_layer3_bias_3;
        reg_ai_sr_body_3_1x3_bias_2= srnn_wdsr_res_block_4_bias_2_3_.resblk_4_layer3_bias_2;

        srnn_wdsr_res_block_4_bias_4_5_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_4_BIAS_4_5_reg);
        reg_ai_sr_body_3_1x3_bias_5= srnn_wdsr_res_block_4_bias_4_5_.resblk_4_layer3_bias_5;
        reg_ai_sr_body_3_1x3_bias_4= srnn_wdsr_res_block_4_bias_4_5_.resblk_4_layer3_bias_4;

        srnn_wdsr_res_block_4_bias_6_7_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_4_BIAS_6_7_reg);
        reg_ai_sr_body_3_1x3_bias_7= srnn_wdsr_res_block_4_bias_6_7_.resblk_4_layer3_bias_7;
        reg_ai_sr_body_3_1x3_bias_6= srnn_wdsr_res_block_4_bias_6_7_.resblk_4_layer3_bias_6;

        srnn_wdsr_res_block_5_bias_0_1_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_5_BIAS_0_1_reg);
        reg_ai_sr_body_4_1x3_bias_1= srnn_wdsr_res_block_5_bias_0_1_.resblk_5_layer3_bias_1;
        reg_ai_sr_body_4_1x3_bias_0= srnn_wdsr_res_block_5_bias_0_1_.resblk_5_layer3_bias_0;

        srnn_wdsr_res_block_5_bias_2_3_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_5_BIAS_2_3_reg);
        reg_ai_sr_body_4_1x3_bias_3= srnn_wdsr_res_block_5_bias_2_3_.resblk_5_layer3_bias_3;
        reg_ai_sr_body_4_1x3_bias_2= srnn_wdsr_res_block_5_bias_2_3_.resblk_5_layer3_bias_2;

        srnn_wdsr_res_block_5_bias_4_5_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_5_BIAS_4_5_reg);
        reg_ai_sr_body_4_1x3_bias_5 = srnn_wdsr_res_block_5_bias_4_5_.resblk_5_layer3_bias_5;
        reg_ai_sr_body_4_1x3_bias_4 = srnn_wdsr_res_block_5_bias_4_5_.resblk_5_layer3_bias_4;

        srnn_wdsr_res_block_5_bias_6_7_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_5_BIAS_6_7_reg);
        reg_ai_sr_body_4_1x3_bias_7= srnn_wdsr_res_block_5_bias_6_7_.resblk_5_layer3_bias_7;
        reg_ai_sr_body_4_1x3_bias_6= srnn_wdsr_res_block_5_bias_6_7_.resblk_5_layer3_bias_6;

        srnn_wdsr_res_block_6_bias_0_1_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_6_BIAS_0_1_reg);
        reg_ai_sr_body_5_1x3_bias_1= srnn_wdsr_res_block_6_bias_0_1_.resblk_6_layer3_bias_1;
        reg_ai_sr_body_5_1x3_bias_0= srnn_wdsr_res_block_6_bias_0_1_.resblk_6_layer3_bias_0;

        srnn_wdsr_res_block_6_bias_2_3_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_6_BIAS_2_3_reg);
        reg_ai_sr_body_5_1x3_bias_3= srnn_wdsr_res_block_6_bias_2_3_.resblk_6_layer3_bias_3;
        reg_ai_sr_body_5_1x3_bias_2= srnn_wdsr_res_block_6_bias_2_3_.resblk_6_layer3_bias_2;

        srnn_wdsr_res_block_6_bias_4_5_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_6_BIAS_4_5_reg);
        reg_ai_sr_body_5_1x3_bias_5= srnn_wdsr_res_block_6_bias_4_5_.resblk_6_layer3_bias_5;
        reg_ai_sr_body_5_1x3_bias_4= srnn_wdsr_res_block_6_bias_4_5_.resblk_6_layer3_bias_4;

        srnn_wdsr_res_block_6_bias_6_7_.regValue = IoReg_Read32(SRNN_WDSR_RES_BLOCK_6_BIAS_6_7_reg);
        reg_ai_sr_body_5_1x3_bias_7 = srnn_wdsr_res_block_6_bias_6_7_.resblk_6_layer3_bias_7;
        reg_ai_sr_body_5_1x3_bias_6 = srnn_wdsr_res_block_6_bias_6_7_.resblk_6_layer3_bias_6;

        srnn_wdsr_tail_layer_bias_0_1_.regValue = IoReg_Read32(SRNN_WDSR_TAIL_LAYER_BIAS_0_1_reg);
        reg_ai_sr_tail_bias_1 = srnn_wdsr_tail_layer_bias_0_1_.tail_layer_bias_1;
        reg_ai_sr_tail_bias_0 = srnn_wdsr_tail_layer_bias_0_1_.tail_layer_bias_0;

        srnn_wdsr_tail_layer_bias_2_3_.regValue = IoReg_Read32(SRNN_WDSR_TAIL_LAYER_BIAS_2_3_reg);
        reg_ai_sr_tail_bias_3= srnn_wdsr_tail_layer_bias_2_3_.tail_layer_bias_3;
        reg_ai_sr_tail_bias_2= srnn_wdsr_tail_layer_bias_2_3_.tail_layer_bias_2;

        srnn_wdsr_tail_layer_bias_4_5_.regValue = IoReg_Read32(SRNN_WDSR_TAIL_LAYER_BIAS_4_5_reg);
        reg_ai_sr_tail_bias_5= srnn_wdsr_tail_layer_bias_4_5_.tail_layer_bias_5;
        reg_ai_sr_tail_bias_4= srnn_wdsr_tail_layer_bias_4_5_.tail_layer_bias_4;

        srnn_wdsr_tail_layer_bias_6_7_.regValue = IoReg_Read32(SRNN_WDSR_TAIL_LAYER_BIAS_6_7_reg);
        reg_ai_sr_tail_bias_7= srnn_wdsr_tail_layer_bias_6_7_.tail_layer_bias_7;
        reg_ai_sr_tail_bias_6= srnn_wdsr_tail_layer_bias_6_7_.tail_layer_bias_6;

        srnn_wdsr_tail_layer_bias_8_9_.regValue = IoReg_Read32(SRNN_WDSR_TAIL_LAYER_BIAS_8_9_reg);
        reg_ai_sr_tail_bias_9= srnn_wdsr_tail_layer_bias_8_9_.tail_layer_bias_9;
        reg_ai_sr_tail_bias_8= srnn_wdsr_tail_layer_bias_8_9_.tail_layer_bias_8;

        srnn_wdsr_tail_layer_bias_10_11_.regValue = IoReg_Read32(SRNN_WDSR_TAIL_LAYER_BIAS_10_11_reg);
        reg_ai_sr_tail_bias_11= srnn_wdsr_tail_layer_bias_10_11_.tail_layer_bias_11;
        reg_ai_sr_tail_bias_10= srnn_wdsr_tail_layer_bias_10_11_.tail_layer_bias_10;

        srnn_wdsr_tail_layer_bias_12_13_.regValue = IoReg_Read32(SRNN_WDSR_TAIL_LAYER_BIAS_12_13_reg);
        reg_ai_sr_tail_bias_13= srnn_wdsr_tail_layer_bias_12_13_.tail_layer_bias_13;
        reg_ai_sr_tail_bias_12= srnn_wdsr_tail_layer_bias_12_13_.tail_layer_bias_12;

        srnn_wdsr_tail_layer_bias_14_15_.regValue = IoReg_Read32(SRNN_WDSR_TAIL_LAYER_BIAS_14_15_reg);
        reg_ai_sr_tail_bias_15= srnn_wdsr_tail_layer_bias_14_15_.tail_layer_bias_15;
        reg_ai_sr_tail_bias_14= srnn_wdsr_tail_layer_bias_14_15_.tail_layer_bias_14;

 
}
void drvif_srnn_read_bias(unsigned char ai_sr_mode, unsigned short *param){
    frvif_bias_rubs_read();
  if (ai_sr_mode==2){
        param[72] = reg_ai_sr_head_bias_0;
        param[73] = reg_ai_sr_head_bias_1;
        param[74] = reg_ai_sr_head_bias_2;
        param[75] = reg_ai_sr_head_bias_3;
        param[76] = reg_ai_sr_head_bias_4;
        param[77] = reg_ai_sr_head_bias_5;
        param[78] = reg_ai_sr_head_bias_6;
        param[79] = reg_ai_sr_head_bias_7;
        param[2408] = reg_ai_sr_body_0_1x3_bias_0;
        param[2409] = reg_ai_sr_body_0_1x3_bias_1;
        param[2410] = reg_ai_sr_body_0_1x3_bias_2;
        param[2411] = reg_ai_sr_body_0_1x3_bias_3;
        param[2412] = reg_ai_sr_body_0_1x3_bias_4;
        param[2413] = reg_ai_sr_body_0_1x3_bias_5;
        param[2414] = reg_ai_sr_body_0_1x3_bias_6;
        param[2415] = reg_ai_sr_body_0_1x3_bias_7;
        param[4744] = reg_ai_sr_body_2_1x3_bias_0;
        param[4745] = reg_ai_sr_body_2_1x3_bias_1;
        param[4746] = reg_ai_sr_body_2_1x3_bias_2;
        param[4747] = reg_ai_sr_body_2_1x3_bias_3;
        param[4748] = reg_ai_sr_body_2_1x3_bias_4;
        param[4749] = reg_ai_sr_body_2_1x3_bias_5;
        param[4750] = reg_ai_sr_body_2_1x3_bias_6;
        param[4751] = reg_ai_sr_body_2_1x3_bias_7;
        param[7080] = reg_ai_sr_body_4_1x3_bias_0;
        param[7081] = reg_ai_sr_body_4_1x3_bias_1;
        param[7082] = reg_ai_sr_body_4_1x3_bias_2;
        param[7083] = reg_ai_sr_body_4_1x3_bias_3;
        param[7084] = reg_ai_sr_body_4_1x3_bias_4;
        param[7085] = reg_ai_sr_body_4_1x3_bias_5;
        param[7086] = reg_ai_sr_body_4_1x3_bias_6;
        param[7087] = reg_ai_sr_body_4_1x3_bias_7;
        param[7376] = reg_ai_sr_tail_bias_0;
        param[7377] = reg_ai_sr_tail_bias_1;
        param[7378] = reg_ai_sr_tail_bias_2;
        param[7379] = reg_ai_sr_tail_bias_3;

    }
    else if(ai_sr_mode==3 || ai_sr_mode==4 ){
        param[72] = reg_ai_sr_head_bias_0;
        param[73] = reg_ai_sr_head_bias_1;
        param[74] = reg_ai_sr_head_bias_2;
        param[75] = reg_ai_sr_head_bias_3;
        param[76] = reg_ai_sr_head_bias_4;
        param[77] = reg_ai_sr_head_bias_5;
        param[78] = reg_ai_sr_head_bias_6;
        param[79] = reg_ai_sr_head_bias_7;
        param[4736] = reg_ai_sr_body_0_1x3_bias_0;
        param[4737] = reg_ai_sr_body_0_1x3_bias_1;
        param[4738] = reg_ai_sr_body_0_1x3_bias_2;
        param[4739] = reg_ai_sr_body_0_1x3_bias_3;
        param[4740] = reg_ai_sr_body_0_1x3_bias_4;
        param[4741] = reg_ai_sr_body_0_1x3_bias_5;
        param[4742] = reg_ai_sr_body_0_1x3_bias_6;
        param[4743] = reg_ai_sr_body_0_1x3_bias_7;
        param[9400] = reg_ai_sr_body_2_1x3_bias_0;
        param[9401] = reg_ai_sr_body_2_1x3_bias_1;
        param[9402] = reg_ai_sr_body_2_1x3_bias_2;
        param[9403] = reg_ai_sr_body_2_1x3_bias_3;
        param[9404] = reg_ai_sr_body_2_1x3_bias_4;
        param[9405] = reg_ai_sr_body_2_1x3_bias_5;
        param[9406] = reg_ai_sr_body_2_1x3_bias_6;
        param[9407] = reg_ai_sr_body_2_1x3_bias_7;
        param[14064] = reg_ai_sr_body_4_1x3_bias_0;
        param[14065] = reg_ai_sr_body_4_1x3_bias_1;
        param[14066] = reg_ai_sr_body_4_1x3_bias_2;
        param[14067] = reg_ai_sr_body_4_1x3_bias_3;
        param[14068] = reg_ai_sr_body_4_1x3_bias_4;
        param[14069] = reg_ai_sr_body_4_1x3_bias_5;
        param[14070] = reg_ai_sr_body_4_1x3_bias_6;
        param[14071] = reg_ai_sr_body_4_1x3_bias_7;
        param[14360] = reg_ai_sr_tail_bias_0;
        param[14361] = reg_ai_sr_tail_bias_1;
        param[14362] = reg_ai_sr_tail_bias_2;
        param[14363] = reg_ai_sr_tail_bias_3;
    }
    else if(ai_sr_mode==5){
        param[72] = reg_ai_sr_head_bias_0;
        param[73] = reg_ai_sr_head_bias_1;
        param[74] = reg_ai_sr_head_bias_2;
        param[75] = reg_ai_sr_head_bias_3;
        param[76] = reg_ai_sr_head_bias_4;
        param[77] = reg_ai_sr_head_bias_5;
        param[78] = reg_ai_sr_head_bias_6;
        param[79] = reg_ai_sr_head_bias_7;
        param[4736] = reg_ai_sr_body_0_1x3_bias_0;
        param[4737] = reg_ai_sr_body_0_1x3_bias_1;
        param[4738] = reg_ai_sr_body_0_1x3_bias_2;
        param[4739] = reg_ai_sr_body_0_1x3_bias_3;
        param[4740] = reg_ai_sr_body_0_1x3_bias_4;
        param[4741] = reg_ai_sr_body_0_1x3_bias_5;
        param[4742] = reg_ai_sr_body_0_1x3_bias_6;
        param[4743] = reg_ai_sr_body_0_1x3_bias_7;
        param[9400] = reg_ai_sr_body_2_1x3_bias_0;
        param[9401] = reg_ai_sr_body_2_1x3_bias_1;
        param[9402] = reg_ai_sr_body_2_1x3_bias_2;
        param[9403] = reg_ai_sr_body_2_1x3_bias_3;
        param[9404] = reg_ai_sr_body_2_1x3_bias_4;
        param[9405] = reg_ai_sr_body_2_1x3_bias_5;
        param[9406] = reg_ai_sr_body_2_1x3_bias_6;
        param[9407] = reg_ai_sr_body_2_1x3_bias_7;
        param[14064] = reg_ai_sr_body_4_1x3_bias_0;
        param[14065] = reg_ai_sr_body_4_1x3_bias_1;
        param[14066] = reg_ai_sr_body_4_1x3_bias_2;
        param[14067] = reg_ai_sr_body_4_1x3_bias_3;
        param[14068] = reg_ai_sr_body_4_1x3_bias_4;
        param[14069] = reg_ai_sr_body_4_1x3_bias_5;
        param[14070] = reg_ai_sr_body_4_1x3_bias_6;
        param[14071] = reg_ai_sr_body_4_1x3_bias_7;
        param[14720] = reg_ai_sr_tail_bias_0;
        param[14721] = reg_ai_sr_tail_bias_1;
        param[14722] = reg_ai_sr_tail_bias_2;
        param[14723] = reg_ai_sr_tail_bias_3;
        param[14724] = reg_ai_sr_tail_bias_4;
        param[14725] = reg_ai_sr_tail_bias_5;
        param[14726] = reg_ai_sr_tail_bias_6;
        param[14727] = reg_ai_sr_tail_bias_7;
        param[14728] = reg_ai_sr_tail_bias_8;


    }
    else if(ai_sr_mode==6){
        param[72] = reg_ai_sr_head_bias_0;
        param[73] = reg_ai_sr_head_bias_1;
        param[74] = reg_ai_sr_head_bias_2;
        param[75] = reg_ai_sr_head_bias_3;
        param[76] = reg_ai_sr_head_bias_4;
        param[77] = reg_ai_sr_head_bias_5;
        param[78] = reg_ai_sr_head_bias_6;
        param[79] = reg_ai_sr_head_bias_7;
        param[4736] = reg_ai_sr_body_0_1x3_bias_0;
        param[4737] = reg_ai_sr_body_0_1x3_bias_1;
        param[4738] = reg_ai_sr_body_0_1x3_bias_2;
        param[4739] = reg_ai_sr_body_0_1x3_bias_3;
        param[4740] = reg_ai_sr_body_0_1x3_bias_4;
        param[4741] = reg_ai_sr_body_0_1x3_bias_5;
        param[4742] = reg_ai_sr_body_0_1x3_bias_6;
        param[4743] = reg_ai_sr_body_0_1x3_bias_7;
        param[9400] = reg_ai_sr_body_1_1x3_bias_0;
        param[9401] = reg_ai_sr_body_1_1x3_bias_1;
        param[9402] = reg_ai_sr_body_1_1x3_bias_2;
        param[9403] = reg_ai_sr_body_1_1x3_bias_3;
        param[9404] = reg_ai_sr_body_1_1x3_bias_4;
        param[9405] = reg_ai_sr_body_1_1x3_bias_5;
        param[9406] = reg_ai_sr_body_1_1x3_bias_6;
        param[9407] = reg_ai_sr_body_1_1x3_bias_7;
        param[14064] = reg_ai_sr_body_2_1x3_bias_0;
        param[14065] = reg_ai_sr_body_2_1x3_bias_1;
        param[14066] = reg_ai_sr_body_2_1x3_bias_2;
        param[14067] = reg_ai_sr_body_2_1x3_bias_3;
        param[14068] = reg_ai_sr_body_2_1x3_bias_4;
        param[14069] = reg_ai_sr_body_2_1x3_bias_5;
        param[14070] = reg_ai_sr_body_2_1x3_bias_6;
        param[14071] = reg_ai_sr_body_2_1x3_bias_7;
        param[18728] = reg_ai_sr_body_3_1x3_bias_0;
        param[18729] = reg_ai_sr_body_3_1x3_bias_1;
        param[18730] = reg_ai_sr_body_3_1x3_bias_2;
        param[18731] = reg_ai_sr_body_3_1x3_bias_3;
        param[18732] = reg_ai_sr_body_3_1x3_bias_4;
        param[18733] = reg_ai_sr_body_3_1x3_bias_5;
        param[18734] = reg_ai_sr_body_3_1x3_bias_6;
        param[18735] = reg_ai_sr_body_3_1x3_bias_7;
        param[23392] = reg_ai_sr_body_4_1x3_bias_0;
        param[23393] = reg_ai_sr_body_4_1x3_bias_1;
        param[23394] = reg_ai_sr_body_4_1x3_bias_2;
        param[23395] = reg_ai_sr_body_4_1x3_bias_3;
        param[23396] = reg_ai_sr_body_4_1x3_bias_4;
        param[23397] = reg_ai_sr_body_4_1x3_bias_5;
        param[23398] = reg_ai_sr_body_4_1x3_bias_6;
        param[23399] = reg_ai_sr_body_4_1x3_bias_7;
        param[28056] = reg_ai_sr_body_5_1x3_bias_0;
        param[28057] = reg_ai_sr_body_5_1x3_bias_1;
        param[28058] = reg_ai_sr_body_5_1x3_bias_2;
        param[28059] = reg_ai_sr_body_5_1x3_bias_3;
        param[28060] = reg_ai_sr_body_5_1x3_bias_4;
        param[28061] = reg_ai_sr_body_5_1x3_bias_5;
        param[28062] = reg_ai_sr_body_5_1x3_bias_6;
        param[28063] = reg_ai_sr_body_5_1x3_bias_7;
        param[28352] = reg_ai_sr_tail_bias_0;
        param[28353] = reg_ai_sr_tail_bias_1;
        param[28354] = reg_ai_sr_tail_bias_2;
        param[28355] = reg_ai_sr_tail_bias_3;
    }
    else if(ai_sr_mode==7){
        param[72] = reg_ai_sr_head_bias_0;
        param[73] = reg_ai_sr_head_bias_1;
        param[74] = reg_ai_sr_head_bias_2;
        param[75] = reg_ai_sr_head_bias_3;
        param[76] = reg_ai_sr_head_bias_4;
        param[77] = reg_ai_sr_head_bias_5;
        param[78] = reg_ai_sr_head_bias_6;
        param[79] = reg_ai_sr_head_bias_7;
        param[4736] = reg_ai_sr_body_0_1x3_bias_0;
        param[4737] = reg_ai_sr_body_0_1x3_bias_1;
        param[4738] = reg_ai_sr_body_0_1x3_bias_2;
        param[4739] = reg_ai_sr_body_0_1x3_bias_3;
        param[4740] = reg_ai_sr_body_0_1x3_bias_4;
        param[4741] = reg_ai_sr_body_0_1x3_bias_5;
        param[4742] = reg_ai_sr_body_0_1x3_bias_6;
        param[4743] = reg_ai_sr_body_0_1x3_bias_7;
        param[9400] = reg_ai_sr_body_1_1x3_bias_0;
        param[9401] = reg_ai_sr_body_1_1x3_bias_1;
        param[9402] = reg_ai_sr_body_1_1x3_bias_2;
        param[9403] = reg_ai_sr_body_1_1x3_bias_3;
        param[9404] = reg_ai_sr_body_1_1x3_bias_4;
        param[9405] = reg_ai_sr_body_1_1x3_bias_5;
        param[9406] = reg_ai_sr_body_1_1x3_bias_6;
        param[9407] = reg_ai_sr_body_1_1x3_bias_7;
        param[14064] = reg_ai_sr_body_2_1x3_bias_0;
        param[14065] = reg_ai_sr_body_2_1x3_bias_1;
        param[14066] = reg_ai_sr_body_2_1x3_bias_2;
        param[14067] = reg_ai_sr_body_2_1x3_bias_3;
        param[14068] = reg_ai_sr_body_2_1x3_bias_4;
        param[14069] = reg_ai_sr_body_2_1x3_bias_5;
        param[14070] = reg_ai_sr_body_2_1x3_bias_6;
        param[14071] = reg_ai_sr_body_2_1x3_bias_7;
        param[18728] = reg_ai_sr_body_3_1x3_bias_0;
        param[18729] = reg_ai_sr_body_3_1x3_bias_1;
        param[18730] = reg_ai_sr_body_3_1x3_bias_2;
        param[18731] = reg_ai_sr_body_3_1x3_bias_3;
        param[18732] = reg_ai_sr_body_3_1x3_bias_4;
        param[18733] = reg_ai_sr_body_3_1x3_bias_5;
        param[18734] = reg_ai_sr_body_3_1x3_bias_6;
        param[18735] = reg_ai_sr_body_3_1x3_bias_7;
        param[23392] = reg_ai_sr_body_4_1x3_bias_0;
        param[23393] = reg_ai_sr_body_4_1x3_bias_1;
        param[23394] = reg_ai_sr_body_4_1x3_bias_2;
        param[23395] = reg_ai_sr_body_4_1x3_bias_3;
        param[23396] = reg_ai_sr_body_4_1x3_bias_4;
        param[23397] = reg_ai_sr_body_4_1x3_bias_5;
        param[23398] = reg_ai_sr_body_4_1x3_bias_6;
        param[23399] = reg_ai_sr_body_4_1x3_bias_7;
        param[28056] = reg_ai_sr_body_5_1x3_bias_0;
        param[28057] = reg_ai_sr_body_5_1x3_bias_1;
        param[28058] = reg_ai_sr_body_5_1x3_bias_2;
        param[28059] = reg_ai_sr_body_5_1x3_bias_3;
        param[28060] = reg_ai_sr_body_5_1x3_bias_4;
        param[28061] = reg_ai_sr_body_5_1x3_bias_5;
        param[28062] = reg_ai_sr_body_5_1x3_bias_6;
        param[28063] = reg_ai_sr_body_5_1x3_bias_7;
        param[29216] = reg_ai_sr_tail_bias_0;
        param[29217] = reg_ai_sr_tail_bias_1;
        param[29218] = reg_ai_sr_tail_bias_2;
        param[29219] = reg_ai_sr_tail_bias_3;
        param[29220] = reg_ai_sr_tail_bias_4;
        param[29221] = reg_ai_sr_tail_bias_5;
        param[29222] = reg_ai_sr_tail_bias_6;
        param[29223] = reg_ai_sr_tail_bias_7;
        param[29224] = reg_ai_sr_tail_bias_8;
        param[29225] = reg_ai_sr_tail_bias_9;
        param[29226] = reg_ai_sr_tail_bias_10;
        param[29227] = reg_ai_sr_tail_bias_11;
        param[29228] = reg_ai_sr_tail_bias_12;
        param[29229] = reg_ai_sr_tail_bias_13;
        param[29230] = reg_ai_sr_tail_bias_14;
        param[29231] = reg_ai_sr_tail_bias_15;
    }
}
unsigned int drvif_dMA_srnn_write_table(unsigned short *pinArray,unsigned short *poutArray,int block_number,int input_channel,int bottle_channel,int scale){
    unsigned int counter = 0;
    int i,ch,resblk,out_ch,weig_row,in_ch,idx_0,idx_1,idx;
    int idx_bias_0,idx_bias_1,bias_0,bias_1;
    int conv_1x3_idx_tmp_0,conv_1x3_idx_tmp_1,conv_1x3_idx_tmp_2,conv_1x3_idx_tmp_3;
    int each_3x3 = bottle_channel*input_channel*9+bottle_channel;
    int each_1x3 = bottle_channel*input_channel*3+input_channel;
    int head_layer_n = input_channel*(9+1);
    for (ch=0;ch<input_channel;ch++){
        poutArray[counter]=(pinArray[ch*9+2]);counter++;
        poutArray[counter]=(pinArray[ch*9+1]);counter++;
        poutArray[counter]=(pinArray[ch*9+0]);counter++;
        poutArray[counter]=(pinArray[ch*9+5]);counter++;
        poutArray[counter]=(pinArray[ch*9+4]);counter++;
        poutArray[counter]=(pinArray[ch*9+3]);counter++;
        poutArray[counter]=(pinArray[ch*9+8]);counter++;
        poutArray[counter]=(pinArray[ch*9+7]);counter++;
        poutArray[counter]=(pinArray[ch*9+6]);counter++;
    }
    for (resblk=0;resblk<block_number;resblk++){
        for (out_ch=0;out_ch<bottle_channel;out_ch=out_ch+2){
            for (weig_row=0;weig_row<3;weig_row++){
                for(in_ch=0;in_ch<input_channel;in_ch++){
                    idx_0 = head_layer_n + each_3x3*resblk + each_1x3*resblk + out_ch*9*input_channel + in_ch*9+weig_row*3 ;
                    poutArray[counter]=(pinArray[idx_0+2]);counter++;
                    poutArray[counter]=(pinArray[idx_0+1]);counter++;
                    poutArray[counter]=(pinArray[idx_0+0]);counter++;

                }
                for(in_ch=0;in_ch<input_channel;in_ch++){
                    idx_1 = head_layer_n + each_3x3*resblk + each_1x3*resblk + (out_ch+1)*9*input_channel + in_ch*9+weig_row*3 ;
                    poutArray[counter]=(pinArray[idx_1+2]);counter++;
                    poutArray[counter]=(pinArray[idx_1+1]);counter++;
                    poutArray[counter]=(pinArray[idx_1+0]);counter++;

                } 
                idx_bias_0 = head_layer_n +  each_3x3*(resblk+1)  - bottle_channel + each_1x3*resblk + out_ch ;
                idx_bias_1 = head_layer_n +  each_3x3*(resblk+1)  - bottle_channel + each_1x3*resblk + out_ch +1  ;
       
                bias_0 =  pinArray[idx_bias_0] ;
                bias_1 =  pinArray[idx_bias_1] ;
                if (weig_row==0){
                    poutArray[counter]=(0);counter++;
                    poutArray[counter]=(0);counter++;
                    poutArray[counter]=((bias_0&0x3f));counter++;
                }
                else if(weig_row==1){
                    poutArray[counter]=(0);counter++;
                    poutArray[counter]=(0);counter++;
                    poutArray[counter]=((((bias_1&0x7)<<3)|(bias_0>>6)));counter++;
                }
                else if(weig_row==2){
                    poutArray[counter]=(0);counter++;
                    poutArray[counter]=(0);counter++;
                    poutArray[counter]=((bias_1>>3));counter++;
                }
            }
        }
    }

    for (resblk=0;resblk<block_number;resblk++){
        for (in_ch=0;in_ch<bottle_channel;in_ch=in_ch+2){
            for (i=0;i<3;i++){
                if (i==0){
                    conv_1x3_idx_tmp_0 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 0*bottle_channel*3 + in_ch*3 ;
                    conv_1x3_idx_tmp_1 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 0*bottle_channel*3 + (in_ch+1)*3 ;
                    conv_1x3_idx_tmp_2 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 1*bottle_channel*3 + in_ch*3 ;
                    conv_1x3_idx_tmp_3 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 1*bottle_channel*3 + (in_ch+1)*3 ;
                }
                else if(i==1){
                    conv_1x3_idx_tmp_0 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 3*bottle_channel*3 + in_ch*3 ;
                    conv_1x3_idx_tmp_1 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 3*bottle_channel*3 + (in_ch+1)*3 ;
                    conv_1x3_idx_tmp_2 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 4*bottle_channel*3 + in_ch*3 ;
                    conv_1x3_idx_tmp_3 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 4*bottle_channel*3 + (in_ch+1)*3 ;
                }
                else if(i==2){
                    conv_1x3_idx_tmp_0 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 6*bottle_channel*3 + in_ch*3 ;
                    conv_1x3_idx_tmp_1 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 6*bottle_channel*3 + (in_ch+1)*3 ;
                    conv_1x3_idx_tmp_2 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 7*bottle_channel*3 + in_ch*3 ;
                    conv_1x3_idx_tmp_3 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 7*bottle_channel*3 + (in_ch+1)*3 ;
                }
                poutArray[counter]=(pinArray[conv_1x3_idx_tmp_0+2]);counter++;
                poutArray[counter]=(pinArray[conv_1x3_idx_tmp_0+1]);counter++;
                poutArray[counter]=(pinArray[conv_1x3_idx_tmp_0+0]);counter++;
        

                poutArray[counter]=(pinArray[conv_1x3_idx_tmp_1+2]);counter++;
                poutArray[counter]=(pinArray[conv_1x3_idx_tmp_1+1]);counter++;
                poutArray[counter]=(pinArray[conv_1x3_idx_tmp_1+0]);counter++;
            
                poutArray[counter]=(pinArray[conv_1x3_idx_tmp_2+2]);counter++;
                poutArray[counter]=(pinArray[conv_1x3_idx_tmp_2+1]);counter++;
                poutArray[counter]=(pinArray[conv_1x3_idx_tmp_2+0]);counter++;

                poutArray[counter]=(pinArray[conv_1x3_idx_tmp_3+2]);counter++;
                poutArray[counter]=(pinArray[conv_1x3_idx_tmp_3+1]);counter++;
                poutArray[counter]=(pinArray[conv_1x3_idx_tmp_3+0] );counter++;
            }
        }
        for (in_ch=0;in_ch<bottle_channel;in_ch=in_ch+2){
            conv_1x3_idx_tmp_0 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 2*bottle_channel*3 + in_ch*3;
            conv_1x3_idx_tmp_1 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 2*bottle_channel*3 + (in_ch+1)*3;
            conv_1x3_idx_tmp_2 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 5*bottle_channel*3 + in_ch*3;
            conv_1x3_idx_tmp_3 = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + 5*bottle_channel*3 + (in_ch+1)*3;

            poutArray[counter]=(pinArray[conv_1x3_idx_tmp_0+2]);counter++;
            poutArray[counter]=(pinArray[conv_1x3_idx_tmp_0+1]);counter++;
            poutArray[counter]=(pinArray[conv_1x3_idx_tmp_0+0]);counter++;
            
            poutArray[counter]=(pinArray[conv_1x3_idx_tmp_1+2]);counter++;
            poutArray[counter]=(pinArray[conv_1x3_idx_tmp_1+1]);counter++;
            poutArray[counter]=(pinArray[conv_1x3_idx_tmp_1+0]);counter++;
        
            poutArray[counter]=(pinArray[conv_1x3_idx_tmp_2+2]);counter++;
            poutArray[counter]=(pinArray[conv_1x3_idx_tmp_2+1]);counter++;
            poutArray[counter]=(pinArray[conv_1x3_idx_tmp_2+0]);counter++;
            
            poutArray[counter]=(pinArray[conv_1x3_idx_tmp_3+2]);counter++;
            poutArray[counter]=(pinArray[conv_1x3_idx_tmp_3+1]);counter++;
            poutArray[counter]=(pinArray[conv_1x3_idx_tmp_3+0]);counter++;
        }
    }

    for (out_ch=0;out_ch<scale*scale;out_ch++){
        for (in_ch=0;in_ch<input_channel;in_ch++){
            idx = head_layer_n + (each_3x3)*block_number + (each_1x3)*block_number + out_ch*input_channel*9 + in_ch*9;
            poutArray[counter]=(pinArray[idx+2]);counter++;
            poutArray[counter]=(pinArray[idx+1]);counter++;
            poutArray[counter]=(pinArray[idx+0]);counter++;
            poutArray[counter]=(pinArray[idx+5]);counter++;
            poutArray[counter]=(pinArray[idx+4]);counter++;
            poutArray[counter]=(pinArray[idx+3]);counter++;
            poutArray[counter]=(pinArray[idx+8]);counter++;
            poutArray[counter]=(pinArray[idx+7]);counter++;
            poutArray[counter]=(pinArray[idx+6]);counter++;
        }
    }
    return counter;
}
/**
 * @brief
 * merlin9 rbus read once
 */
void drvif_rbus_single_read(int weight_sram_sel,int weight_write_en, int weight_read_en,int weight_fw_mode,int weight_out_ch_sel,int weight_in_ch_sel,int weight_3x3_sel,unsigned short* ouput){//,unsigned short *weight_0,unsigned short *weight_1,  unsigned short *weight_2, unsigned short *bias){
    srnn_wdsr_weig_sram_write_mode_RBUS srnn_wdsr_weig_sram_write_mode_; // WDSR_WEIG_SRAM_WRITE_MODE_reg
    srnn_wdsr_weig_sram_write_waddr_RBUS  srnn_wdsr_weig_sram_write_waddr_; // WDSR_WEIG_SRAM_WRITE_WADDR_reg
    srnn_wdsr_weig_sram_weig_0_RBUS srnn_wdsr_weig_sram_weig_0_; // WDSR_WEIG_SRAM_WEIG_0_reg
    srnn_wdsr_weig_sram_weig_1_RBUS srnn_wdsr_weig_sram_weig_1_; // WDSR_WEIG_SRAM_WEIG_1_reg
    srnn_wdsr_weig_sram_bias_RBUS srnn_wdsr_weig_sram_bias_; // WDSR_WEIG_SRAM_BIAS_reg
    //int time_out;
    srnn_wdsr_weig_sram_write_mode_.regValue = IoReg_Read32(SRNN_WDSR_WEIG_SRAM_WRITE_MODE_reg);
    srnn_wdsr_weig_sram_write_mode_.weight_sram_sel = weight_sram_sel;
    srnn_wdsr_weig_sram_write_mode_.weight_write_en = 0;
    srnn_wdsr_weig_sram_write_mode_.weight_read_en = 0;
    srnn_wdsr_weig_sram_write_mode_.weight_fw_mode = weight_fw_mode;
    IoReg_Write32(SRNN_WDSR_WEIG_SRAM_WRITE_MODE_reg,srnn_wdsr_weig_sram_write_mode_.regValue);

    srnn_wdsr_weig_sram_write_waddr_.regValue = IoReg_Read32(SRNN_WDSR_WEIG_SRAM_WRITE_WADDR_reg); 
    srnn_wdsr_weig_sram_write_waddr_.weight_out_ch_sel = weight_out_ch_sel;
    srnn_wdsr_weig_sram_write_waddr_.weight_in_ch_sel = weight_in_ch_sel;
    srnn_wdsr_weig_sram_write_waddr_.weight_3x3_sel = weight_3x3_sel;
    IoReg_Write32(SRNN_WDSR_WEIG_SRAM_WRITE_WADDR_reg,srnn_wdsr_weig_sram_write_waddr_.regValue);


    srnn_wdsr_weig_sram_write_mode_.regValue = IoReg_Read32(SRNN_WDSR_WEIG_SRAM_WRITE_MODE_reg);
    srnn_wdsr_weig_sram_write_mode_.weight_sram_sel = weight_sram_sel;
    srnn_wdsr_weig_sram_write_mode_.weight_write_en = weight_write_en;
    srnn_wdsr_weig_sram_write_mode_.weight_read_en = weight_read_en;
    srnn_wdsr_weig_sram_write_mode_.weight_fw_mode = weight_fw_mode;
    IoReg_Write32(SRNN_WDSR_WEIG_SRAM_WRITE_MODE_reg,srnn_wdsr_weig_sram_write_mode_.regValue);

    /* check weight_read_en has been clear */
	/*
    time_out = 1000;
    do {
        srnn_wdsr_weig_sram_write_mode_.regValue = IoReg_Read32(SRNN_WDSR_WEIG_SRAM_WRITE_MODE_reg);
        time_out --;

    } while((time_out>5) && (srnn_wdsr_weig_sram_write_mode_.weight_read_en == 1));

    if (time_out <= 10){
        rtd_printk(KERN_ERR, TAG_NAME, "NNSR  read timeout! count %d \n",time_out);
    }*/
    srnn_wdsr_weig_sram_weig_0_.regValue = IoReg_Read32(SRNN_WDSR_WEIG_SRAM_WEIG_0_reg); 
    //*weight_1 = srnn_wdsr_weig_sram_weig_0_.weight_1;
    //*weight_0 = srnn_wdsr_weig_sram_weig_0_.weight_0;
    ouput[0]  = srnn_wdsr_weig_sram_weig_0_.weight_0;
    ouput[1]  = srnn_wdsr_weig_sram_weig_0_.weight_1;
    srnn_wdsr_weig_sram_weig_1_.regValue = IoReg_Read32(SRNN_WDSR_WEIG_SRAM_WEIG_1_reg); 
    //*weight_2 = srnn_wdsr_weig_sram_weig_1_.weight_2;
     ouput[2] = srnn_wdsr_weig_sram_weig_1_.weight_2;

    srnn_wdsr_weig_sram_bias_.regValue = IoReg_Read32(SRNN_WDSR_WEIG_SRAM_BIAS_reg); 
    //*bias = srnn_wdsr_weig_sram_bias_.bias ;
    ouput[3] = srnn_wdsr_weig_sram_bias_.bias;
 

}
/**
 * @brief
 * merlin9 all rbus
 */
void drvif_rbus_srnn_read_table(unsigned short *pinArray,int block_number,int input_channel,int bottle_channel,int scale){
    
    int each_3x3 = bottle_channel*input_channel*9+bottle_channel;
    int each_1x3 = bottle_channel*input_channel*3+input_channel;
    int head_layer_n = input_channel*(9+1);
    int sram_sel = 0;
    int out_ch,in_ch,weight_sel,idx,resblk;
    unsigned short tmp_array[4];

    	 each_3x3 = bottle_channel*input_channel*9+bottle_channel;
      	each_1x3 = bottle_channel*input_channel*3+input_channel;
	head_layer_n = input_channel*(9+1);
	resblk=0;
	idx=0;
		
    for (out_ch=0;out_ch<input_channel;out_ch++){
        in_ch=0;
        for (weight_sel=0;weight_sel<3;weight_sel++){
            drvif_rbus_single_read(sram_sel,0,1,1,out_ch,in_ch,weight_sel,tmp_array);
            pinArray[out_ch*9+weight_sel*3] = tmp_array[0];
            pinArray[out_ch*9+weight_sel*3+1]= tmp_array[1];
            pinArray[out_ch*9+weight_sel*3+2]= tmp_array[2];
        }
    }

    for (resblk=0;resblk<block_number;resblk++){
        sram_sel = resblk*2+2;
        for (out_ch=0;out_ch<bottle_channel;out_ch++){
            for (in_ch=0;in_ch<input_channel;in_ch++){
                for (weight_sel=0;weight_sel<3;weight_sel++){
                    idx =  head_layer_n+each_3x3*resblk + each_1x3*resblk + out_ch*9*input_channel + in_ch*9+weight_sel*3;
                    drvif_rbus_single_read(sram_sel,0,1,1,out_ch,in_ch,weight_sel,tmp_array);
                    pinArray[idx]= tmp_array[0];
                    pinArray[idx+1]= tmp_array[1];
                    pinArray[idx+2]= tmp_array[2];
                }
            }      
        }
    }

    for (resblk=0;resblk<block_number;resblk++){
        sram_sel = resblk*2+2;
        for (out_ch=0;out_ch<bottle_channel;out_ch=out_ch+2){
            idx = head_layer_n +  each_3x3*(resblk+1)  - bottle_channel + each_1x3*resblk + out_ch;
            drvif_rbus_single_read(sram_sel,0,1,1,out_ch,8,0,tmp_array);
            drvif_rbus_single_read(sram_sel,0,1,1,out_ch+1,8,1,tmp_array);
            pinArray[idx] = tmp_array[3];
            drvif_rbus_single_read(sram_sel,0,1,1,out_ch+1,8,2,tmp_array);
            pinArray[idx+1] = tmp_array[3];
        }
    }
    for (resblk=0;resblk<block_number;resblk++){
        sram_sel = resblk*2+2+1;
        for (out_ch=0;out_ch<input_channel;out_ch++){
            for (in_ch=0;in_ch<bottle_channel;in_ch++){
                for (weight_sel=0;weight_sel<1;weight_sel++){
                    idx = head_layer_n + (each_3x3)*(resblk+1) + (each_1x3)*resblk + out_ch*bottle_channel*3 + in_ch*3;
                    drvif_rbus_single_read(sram_sel,0,1,1,out_ch,in_ch,weight_sel,tmp_array);
                    pinArray[idx]= tmp_array[0];
                    pinArray[idx+1]= tmp_array[1];
                    pinArray[idx+2]= tmp_array[2];
                }
            }
        }
                                
    }
    sram_sel =1;
    for (out_ch=0;out_ch<scale*scale;out_ch++){
        for (in_ch=0;in_ch<input_channel;in_ch++){
            for (weight_sel=0;weight_sel<3;weight_sel++){
                idx = head_layer_n + (each_3x3)*block_number + (each_1x3)*block_number + out_ch*input_channel*9 + in_ch*9+weight_sel*3;
                drvif_rbus_single_read(sram_sel,0,1,1,out_ch,in_ch,weight_sel,tmp_array);
                pinArray[idx]= tmp_array[0];
                pinArray[idx+1]= tmp_array[1];
                pinArray[idx+2]= tmp_array[2];
            }
        }
    }
}

void drvif_srnn_model_rbus_read(unsigned char ai_sr_mode, unsigned short *param){
    if(ai_sr_mode==2){
        
        drvif_rbus_srnn_read_table(param,3,8,24,2);
        drvif_srnn_read_bias(ai_sr_mode, param);

    }
    else if(ai_sr_mode==3 || ai_sr_mode==4){
         drvif_rbus_srnn_read_table(param,3,8,48,2);
         drvif_srnn_read_bias(ai_sr_mode, param);
    }
    else if(ai_sr_mode==5){
        drvif_rbus_srnn_read_table(param,3,8,48,3);
        drvif_srnn_read_bias(ai_sr_mode, param);
    }
    else if(ai_sr_mode==6){
        drvif_rbus_srnn_read_table(param,6,8,48,2);
        drvif_srnn_read_bias(ai_sr_mode, param);
    }
    else if(ai_sr_mode==7){
         drvif_rbus_srnn_read_table(param,6,8,48,4);
         drvif_srnn_read_bias(ai_sr_mode, param);
    }
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

		*vir_addr_8 = ((gSrnnParamTmp[idx]&0x1)<<7) + (gSrnnParamTmp[idx+1]>>2);
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx+1]&0x3)<<6) + (gSrnnParamTmp[idx+2]>>3);
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx+2]&0x7)<<5) + (gSrnnParamTmp[idx+3]>>4);
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx+3]&0xf)<<4) + (gSrnnParamTmp[idx+4]>>5);
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx+4]&0x1f)<<3) + (gSrnnParamTmp[idx+5]>>6);
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx+5]&0x3f)<<2) + (gSrnnParamTmp[idx+6]>>7);
		vir_addr_8++;

		*vir_addr_8 = ((gSrnnParamTmp[idx+6]&0x7f)<<1) + (gSrnnParamTmp[idx+7]>>8);
		vir_addr_8++;

		*vir_addr_8 = gSrnnParamTmp[idx+7]&0xff;
		vir_addr_8++;
	}
}

int mode2_dynamic_info[13] = {80,1808,1832,2408,2416,4144,4168,4744,4752,6480,6504,7080,7380};
int mode3_dynamic_info[13] = {80,3536,3584,4736,4744,8200,8248,9400,9408,12864,12912,14064,14364};
int mode5_dynamic_info[13] = {80,3536,3584,4736,4744,8200,8248,9400,9408,12864,12912,14064,14729};
int mode6_dynamic_info[25] = {80,3536,3584,4736,4744,8200,8248,9400,9408,12864,12912,14064,14072,17528,17576,18728,18736,22192,22240,23392,23400,26856,26904,28056,28356};
int mode7_dynamic_info[25] = {80,3536,3584,4736,4744,8200,8248,9400,9408,12864,12912,14064,14072,17528,17576,18728,18736,22192,22240,23392,23400,26856,26904,28056,29232};
unsigned char  judge_fixed_dynamic(unsigned char ai_sr_mode, int counter){
    int i ;
    unsigned char ret=1;
    if (ai_sr_mode==2){
         for (i=0;i<13;i++){
            if(counter>=mode2_dynamic_info[i]){
                ret = !ret ;
            }
         }
    }
    else if (ai_sr_mode==3 || ai_sr_mode==4){
         for (i=0;i<13;i++){
            if(counter>=mode3_dynamic_info[i]){
                ret = !ret ;
            }
         }
    }
    else if (ai_sr_mode==5){
         for (i=0;i<13;i++){
            if(counter>=mode5_dynamic_info[i]){
                ret = !ret ;
            }
         }
    }
    else if (ai_sr_mode==6){
         for (i=0;i<25;i++){
            if(counter>=mode6_dynamic_info[i]){
                ret = !ret ;
            }
         }
    }
    else if (ai_sr_mode==7){
         for (i=0;i<25;i++){
            if(counter>=mode7_dynamic_info[i]){
                ret = !ret ;
            }
         }
    }
    return ret;
}

unsigned short drvif_srnn_weight_blend_fixed_point(unsigned short pin_arry_a, unsigned short pin_arry_b, int alpha, int scale){
    int fra_bits = 8;
	int pos_neg_bound = 1<<fra_bits;
	unsigned short pout_arry;
	signed short ptmp_arry_a, ptmp_arry_b;
	signed short poutmp_arry;

	if(pin_arry_a>=pos_neg_bound)
		ptmp_arry_a = pin_arry_a-512;
	else 
		ptmp_arry_a = pin_arry_a;

	if(pin_arry_b>=pos_neg_bound)
		ptmp_arry_b = pin_arry_b-512;
	else 
		ptmp_arry_b = pin_arry_b;

	poutmp_arry = (ptmp_arry_a*( (1<<scale)-alpha) + ptmp_arry_b*alpha + (1<<(scale-1)))>>scale;

	if(poutmp_arry<0)
		pout_arry = (unsigned short)(poutmp_arry+512);
	else 
		pout_arry = (unsigned short)poutmp_arry;

	return pout_arry;
}
unsigned short drvif_srnn_weight_blend_dynamic_point(unsigned short pin_arry_a, unsigned short pin_arry_b, int alpha, int scale){
    int fra_bits = 5;
	int pos_neg_bound = 1<<fra_bits;
	unsigned short pout_arry;
    unsigned short pin_arry_a_remove_shift,pin_arry_b_remove_shift;
	signed short ptmp_arry_a, ptmp_arry_b;
	signed short poutmp_arry;
    int shift;
    shift = pin_arry_a/(1<<(fra_bits+1));
    pin_arry_a_remove_shift = pin_arry_a % (1<<(fra_bits+1));
    pin_arry_b_remove_shift = pin_arry_b % (1<<(fra_bits+1));

	if(pin_arry_a_remove_shift>=pos_neg_bound)
		ptmp_arry_a = pin_arry_a_remove_shift-64;
	else 
		ptmp_arry_a = pin_arry_a_remove_shift;

	if(pin_arry_b_remove_shift>=pos_neg_bound)
		ptmp_arry_b = pin_arry_b_remove_shift-64;
	else 
		ptmp_arry_b = pin_arry_b_remove_shift;

	poutmp_arry = (ptmp_arry_a*( (1<<scale)-alpha) + ptmp_arry_b*alpha + (1<<(scale-1)))>>scale;

	if(poutmp_arry<0)
		pout_arry = (unsigned short)(poutmp_arry+64);
	else 
		pout_arry = (unsigned short)poutmp_arry;

    pout_arry = pout_arry + (shift)*(1<<(fra_bits+1));
	return pout_arry;
}

/*================================== DMA CHECK FUNCTIONS ==============================*/
short drvif_NNSR_DMA_Err_Check(unsigned char showMSG)
{
	short ret_val = 0;
	if (gSrnnTopInfo.ClkEn == 1){
		short err_TBL[10] = {0};

		err_TBL[0] = SRNN_3DLUT_SRNN_3DLUT_Table0_ErrStatus2Irq_get_table0_err_status(IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_ErrStatus2Irq_reg));
		err_TBL[1] = SRNN_3DLUT_SRNN_3DLUT_Table1_ErrStatus2Irq_get_table1_err_status(IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table1_ErrStatus2Irq_reg));

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
		IoReg_SetBits(SRNN_3DLUT_SRNN_3DLUT_Table0_ErrStatus2Irq_reg, SRNN_3DLUT_SRNN_3DLUT_Table0_ErrStatus2Irq_table0_err_clr_mask);
	}
}

unsigned char drvif_Get_NNSR_DMA_Table_Done(unsigned char tbl_idx, unsigned char showMSG)
{
	unsigned char ret = 0;
	if (gSrnnTopInfo.ClkEn == 1){
	
		if (tbl_idx == 0)
			ret = SRNN_3DLUT_SRNN_3DLUT_Table0_Status_get_table0_wdone(IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table0_Status_reg));
		else
			ret = SRNN_3DLUT_SRNN_3DLUT_Table1_Status_get_table1_wdone(IoReg_Read32(SRNN_3DLUT_SRNN_3DLUT_Table1_Status_reg));	


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
		
			if (tbl_idx == 0)
				IoReg_SetBits(SRNN_3DLUT_SRNN_3DLUT_Table0_Status_reg, SRNN_3DLUT_SRNN_3DLUT_Table0_Status_table0_wdone_mask);
			else
				IoReg_SetBits(SRNN_3DLUT_SRNN_3DLUT_Table1_Status_reg, SRNN_3DLUT_SRNN_3DLUT_Table1_Status_table1_wdone_mask);	
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
#if 0
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
#endif
}

void drvif_srnn_gSrnnTopInfo_reset(void)
{
	memset(&gSrnnTopInfo, 0, sizeof(VIP_SRNN_TOP_INFO));
}

void drvif_srnn_clk_enable(unsigned char clk_srnn_en)
{ 
#ifndef BUILD_QUICK_SHOW
	if (clk_srnn_en == 1){
		SrnnCtrl_SetSRNNClkEn(clk_srnn_en);
		SrnnCtrl_SetSRNNSRSTEn(clk_srnn_en);
		gSrnnTopInfo.ClkEn = clk_srnn_en;
		//scalerPQMask_SetPqmaskSrnnStatus(PQMASK_SRNN_STAT_STANDBY);
		rtd_pr_vpq_info("[%s] NNSR dbg, clk_srnn_en = %d\n", __FUNCTION__, clk_srnn_en);
	}
	else{
#if defined(CONFIG_RTK_AI_DRV)
		//using PQmask task to control srnn clk off
		//scalerPQMask_SetPqmaskSrnnStatus(PQMASK_SRNN_STAT_ASK_TO_SUSPEND);
		rtd_pr_vpq_info("[%s] NNSR dbg, PQMASK_SRNN_STAT_ASK_TO_SUSPEND \n", __FUNCTION__);

#else //CONFIG_RTK_AI_DRV

		// PQmask task: not active, disable clk directly
		SrnnCtrl_SetSRNNClkEn(clk_srnn_en);
		//SrnnCtrl_SetSRNNSRSTEn(clk_srnn_en);
		gSrnnTopInfo.ClkEn = clk_srnn_en;
		//scalerPQMask_SetPqmaskSrnnStatus(PQMASK_SRNN_STAT_SUSPEND);
		
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
		SrnnCtrl_SetUZUSel(0, 0);
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
	int scale=0;
	scale =  SrnnCtrl_AISR_mapping_scale(ai_sr_mode);
	gSrnnTopInfo.ModeAISR = ai_sr_mode;
	gSrnnTopInfo.ModeScale = SrnnCtrl_AISR_mapping_su_mode(ai_sr_mode);
	
	gSrnnArchInfo.ModeAISR = ai_sr_mode;
	gSrnnArchInfo.ChPSNRTail = gSuModeSizeMapping[su_mode]*gSuModeSizeMapping[su_mode];
	gSrnnArchInfo.ModeScale = SrnnCtrl_AISR_mapping_su_mode(ai_sr_mode);

	// copy table for debug and show table in rtice
	memset(srnn_current_table, 0, sizeof(unsigned short)*SRNN_WEIGHT_NUM_MAX);
	memcpy(srnn_current_table, param, sizeof(unsigned short)*drvif_srnn_get_weight_table_size(ai_sr_mode, su_mode));
	
	switch(ai_sr_mode){
		case 2:
			gSrnnArchInfo.NumBlock = 3;
			gSrnnArchInfo.ChIN = 1;
			gSrnnArchInfo.ChF = 8;
			gSrnnArchInfo.ChB = 24;
			counter = drvif_dMA_srnn_write_table(param,
				gSrnnParamTmp,
				gSrnnArchInfo.NumBlock,
				gSrnnArchInfo.ChF,
				gSrnnArchInfo.ChB,
				scale
				);
			break;
		case 3:
			gSrnnArchInfo.NumBlock = 3;
			gSrnnArchInfo.ChIN = 1;
			gSrnnArchInfo.ChF = 8;
			gSrnnArchInfo.ChB = 48;
			counter = drvif_dMA_srnn_write_table(param,
				gSrnnParamTmp,
				gSrnnArchInfo.NumBlock,
				gSrnnArchInfo.ChF,
				gSrnnArchInfo.ChB,
				scale
				);
			break;
		case 4:
			gSrnnArchInfo.NumBlock = 3;
			gSrnnArchInfo.ChIN = 1;
			gSrnnArchInfo.ChF = 8;
			gSrnnArchInfo.ChB = 48;
			counter = drvif_dMA_srnn_write_table(param,
				gSrnnParamTmp,
				gSrnnArchInfo.NumBlock,
				gSrnnArchInfo.ChF,
				gSrnnArchInfo.ChB,
				scale
				);
			break;
		case 5:
			gSrnnArchInfo.NumBlock = 3;
			gSrnnArchInfo.ChIN = 1;
			gSrnnArchInfo.ChF = 8;
			gSrnnArchInfo.ChB = 48;
			counter = drvif_dMA_srnn_write_table(param,
				gSrnnParamTmp,
				gSrnnArchInfo.NumBlock,
				gSrnnArchInfo.ChF,
				gSrnnArchInfo.ChB,
				scale
				);
			break;
		case 6:
			gSrnnArchInfo.NumBlock = 6;
			gSrnnArchInfo.ChIN = 1;
			gSrnnArchInfo.ChF = 8;
			gSrnnArchInfo.ChB = 48;
			counter = drvif_dMA_srnn_write_table(param,
				gSrnnParamTmp,
				gSrnnArchInfo.NumBlock,
				gSrnnArchInfo.ChF,
				gSrnnArchInfo.ChB,
				scale
				);
			break;
		case 7:
			gSrnnArchInfo.NumBlock = 6;
			gSrnnArchInfo.ChIN = 1;
			gSrnnArchInfo.ChF = 8;
			gSrnnArchInfo.ChB = 48;
			counter = drvif_dMA_srnn_write_table(param,
				gSrnnParamTmp,
				gSrnnArchInfo.NumBlock,
				gSrnnArchInfo.ChF,
				gSrnnArchInfo.ChB,
				scale
				);
			break;
		default:
			drvif_srnn_pq_enable_set(0);
			rtd_printk(KERN_INFO, TAG_NAME, "Incorrect ai_sr_mode\n" ); 
			break;
	}
	// write bias of merlin9
	drvif_srnn_write_bias(ai_sr_mode, param);
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
	int k, param_num;
	int scale = SRNN_BLENDING_SCALE;

	if (alpha >= (1<<scale)){
		rtd_pr_vpq_emerg("[%s] Incorrect alpha = %d  \n", __FUNCTION__,  alpha);
		alpha = 16;
	}

	if (aisrmode == 0){
		rtd_pr_vpq_info(" nnsr_mode=%d ,no need to do weight blending \n", aisrmode);
	}
	else{
		
		    param_num = drvif_srnn_get_weight_table_size(aisrmode,0);
		  
		    for(k=0;k<param_num;k++){
		        if(judge_fixed_dynamic(aisrmode,k)){
		            param_out[k] = drvif_srnn_weight_blend_fixed_point(param1[k], param2[k], (int)alpha,scale);
		        }
		        else{
		            param_out[k] = drvif_srnn_weight_blend_dynamic_point(param1[k], param2[k], (int)alpha,scale);
		        }
		    }
	}
}

/**
 * @brief
 * set srnn pq_misc_table
 */
void drvif_srnn_set_table(DRV_srnn_table *ptr, unsigned char isSkipDB){
	//unsigned char i = 0;
	if (gSrnnTopInfo.ClkEn == 1){
		SrnnCtrl_SetDoubleBufferEn();
		SrnnCtrl_SetPSNRCoring(ptr->PostProc.Coring, isSkipDB);
		SrnnCtrl_SetPSNRGain(ptr->PostProc.GainPos, ptr->PostProc.GainNeg, isSkipDB);
		SrnnCtrl_SetPSNRClamp(ptr->PostProc.ClampPos, ptr->PostProc.ClampNeg, isSkipDB);
		//SrnnCtrl_SetLPFMode(ptr->PostProc.LPF.Mode, isSkipDB);
		//SrnnCtrl_SetLPFCoeff(ptr->PostProc.LPF.Coeff0, isSkipDB);
		//for (i=0; i<3; i++){
		//	SrnnCtrl_SetPQMaskInGainOffset(ptr->PQmask.In[i].Gain, ptr->PQmask.In[i].Offset, i, isSkipDB);
		//}
		//SrnnCtrl_SetPQMaskOutGainOffset(ptr->PQmask.Out.Gain, ptr->PQmask.Out.Offset, isSkipDB);

		SrnnCtrl_SetBlockDisable(ptr->BlockCtrl.BlockDisable, isSkipDB);
		SrnnCtrl_SetFeatureReduce(ptr->BlockCtrl.FeatReduce, isSkipDB);
		
		//SrnnCtrl_SetHx2Coeff(ptr->Hx2.Coeff0, ptr->Hx2.Coeff1, isSkipDB);

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
	//unsigned char i = 0;
	
	if (gSrnnTopInfo.ClkEn == 1){
		SrnnCtrl_SetDoubleBufferEn();
		SrnnCtrl_SetPSNRCoring(ptr->PostProc.Coring, isSkipDB);
		SrnnCtrl_SetPSNRGain(ptr->PostProc.GainPos, ptr->PostProc.GainNeg, isSkipDB);
		SrnnCtrl_SetPSNRClamp(ptr->PostProc.ClampPos, ptr->PostProc.ClampNeg, isSkipDB);
		//not use in merlin9
		//SrnnCtrl_SetLPFMode(ptr->PostProc.LPF.Mode, isSkipDB);
		//SrnnCtrl_SetLPFCoeff(ptr->PostProc.LPF.Coeff0, isSkipDB);
		
		//for (i=0; i<3; i++){
		//	SrnnCtrl_SetPQMaskInGainOffset(ptr->PQmask.In[i].Gain, ptr->PQmask.In[i].Offset, i, isSkipDB);
		//}
		
		//SrnnCtrl_SetPQMaskOutGainOffset(ptr->PQmask.Out.Gain, ptr->PQmask.Out.Offset, isSkipDB);
		
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
	bGain = SRNN_SRNN_POST_PROCESS_0_get_srnn_post_psnr_pos_gain(IoReg_Read32(SRNN_SRNN_POST_PROCESS_0_reg));
	
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
    unsigned int ret=0;

    if (nnsr_mode==2){
        ret = 7380;
    }
    else if(nnsr_mode==3 || nnsr_mode==4){
         ret = 14364;       
    }
    else if(nnsr_mode==5){
         ret = 14729;       
    }
    else if(nnsr_mode==6){
         ret = 28356;       
    }
    else if(nnsr_mode==7){
         ret = 29232;       
    }else{
	rtd_pr_vpq_info("Incorrect ai_sr_mode\n");
    }
    return ret;
	
}
/**
 * @brief
 * Set wtable_act sta/end
 */
void SrnnCtrl_SetWtableActSta_End(unsigned short sta, unsigned short end, unsigned char isSkipDB){
	srnn_wtable_act_sel1_RBUS  srnn_pre_post_wtable_act_sel1_reg;

	srnn_pre_post_wtable_act_sel1_reg.regValue = IoReg_Read32(SRNN_Wtable_act_sel1_reg);
	srnn_pre_post_wtable_act_sel1_reg.wtable_act_sta = sta;
	srnn_pre_post_wtable_act_sel1_reg.wtable_act_end = end;
	IoReg_Write32(SRNN_Wtable_act_sel1_reg, srnn_pre_post_wtable_act_sel1_reg.regValue);

	if (isSkipDB == 0) {
		/*db apply */
		SrnnCtrl_SetDoubleBufferApply();
	}
}
/**
 * @brief
 * set srnn size related setting according to gSrnnTopInfo
 */
void drvif_srnn_reg_init(void){
	//scaleup_ds_uzu_input_size_RBUS ds_uzu_Input_Size_REG;
	SrnnCtrl_SetDoubleBufferEn();
	SrnnCtrl_SetWtableActSta_End(0, 200, 0);//enlarge wtable act
	
	if (gSrnnTopInfo.ModeAISR < 2){
		SrnnCtrl_SetDataInSel(0, 0);
		//SrnnCtrl_SetHx2En(0, 0);
	}
	else{
		SrnnCtrl_SetDataInSel(gSrnnTopInfo.SubPathEn, 0);
		//SrnnCtrl_SetSRNNInputSize(gSrnnTopInfo.SrnnIn_Height, gSrnnTopInfo.SrnnIn_Width, 0);
		//SrnnCtrl_SetHx2En(gSrnnTopInfo.Hx2En, 0);

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
 * set srnn clk fract by linerate ratio to avoid power ripple
 */
void drvif_srnn_set_clk_fract_by_linerate_ratio(void){
	unsigned int linerate_ratio = 10000;
	linerate_ratio = scaler_disp_get_stage1_linerate_ratio();		// 10000 = 1.0 ,  5000 = 0.5

	if (linerate_ratio > 10000){
		SrnnCtrl_SetSRNNClkFract(0, 0, 0);
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
	unsigned int linerate_ratio = 10000;
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
		linerate_ratio = scaler_disp_get_stage1_linerate_ratio();		// 10000 = 1.0 ,  5000 = 0.5
		if (linerate_ratio > 10000){
			linerate_ratio = 10000;
			rtd_pr_vpq_emerg("[%s] scaler_disp_get_stage1_linerate_ratio = %d > 10000\n", __FUNCTION__, linerate_ratio);
		}
		SrnnClk = (SrnnClk * linerate_ratio)/10000;
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
	mdomain_disp_ddr_mainprevstart_RBUS mdomain_disp_ddr_mainprevstart_reg;

	ppoverlay_uzudtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DH_TOTAL_reg);
	ppoverlay_uzudtg_control1_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control1_reg);
	mdomain_disp_ddr_mainprevstart_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainPreVStart_reg);

	mdom_pre_read = mdomain_disp_ddr_mainprevstart_reg.main_pre_rd_v_start;

	if ((((is_scaler_input_2p_mode(SLR_MAIN_DISPLAY))||force_enable_two_step_uzu() || get_hdmi_vrr_4k60_mode()) && (Get_DISPLAY_REFRESH_RATE() >= 120)) || (ppoverlay_uzudtg_control1_reg.uzu_4k_120_mode == 1)){
		// vrr or 4k120 case
		srnn_pre_rd = mdom_pre_read* ppoverlay_uzudtg_dh_total_reg.uzudtg_dh_total;
	}
	else{
		srnn_pre_rd = (mdom_pre_read * (ppoverlay_uzudtg_dh_total_reg.uzudtg_dh_total +1)) >> 1;
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
	srnn_demo_1_RBUS srnn_pre_pos_demo_v_reg;
	srnn_demo_2_RBUS srnn_pre_pos_demo_h_reg;
	srnn_pre_pos_demo_v_reg.regValue = IoReg_Read32(SRNN_DEMO_1_reg);
	srnn_pre_pos_demo_h_reg.regValue = IoReg_Read32(SRNN_DEMO_2_reg);
	srnn_pre_pos_demo_v_reg.demo_sr_v_sta = v_sta;
	srnn_pre_pos_demo_v_reg.demo_sr_v_end= v_end;
	srnn_pre_pos_demo_h_reg.demo_sr_h_sta= h_sta;
	srnn_pre_pos_demo_h_reg.demo_sr_h_end = h_end;
	IoReg_Write32(SRNN_DEMO_1_reg, srnn_pre_pos_demo_v_reg.regValue);
	IoReg_Write32(SRNN_DEMO_2_reg, srnn_pre_pos_demo_h_reg.regValue);

}

void drvif_srnn_demo_window_mode(unsigned char demo_sr_mode)
{
	srnn_srnn_ctrl_RBUS  srnn_srnn_ctrl_;
	srnn_srnn_ctrl_.regValue = IoReg_Read32(SRNN_SRNN_CTRL_reg);
	srnn_srnn_ctrl_.bbbsr_en= demo_sr_mode;
	IoReg_Write32(SRNN_SRNN_CTRL_reg, srnn_srnn_ctrl_.regValue);
}
