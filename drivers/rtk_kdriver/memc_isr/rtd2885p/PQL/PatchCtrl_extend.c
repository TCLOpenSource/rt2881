
#include "memc_isr/include/memc_lib.h"
#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_reg_def.h"
#include "memc_isr/PQL/PatchCtrl_extend.h"

extern VOID patch_MEMC_bypass_switch(const _PARAM_PATCH_CTRL *pParam, _OUTPUT_PATCH_CTRL *pOutput);
extern VOID patch_MEMC_bypass_switch_HDMI(const _PARAM_PATCH_CTRL *pParam, _OUTPUT_PATCH_CTRL *pOutput);
////////////////////////////////////////////////////////////////////////

#if 0 //define in Patch_Manage.c
VOID patch_MEMC_bypass_switch(const _PARAM_PATCH_CTRL *pParam, _OUTPUT_PATCH_CTRL *pOutput)
{
	unsigned int memc_switch_en;
	static unsigned int memc_switch_frm_cnt = 1;
	static unsigned int memc_bypass_state = 0;
	unsigned int frm_num = 14;
//	unsigned int test_val, test_val2, test_val3, test_val4;
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	PQL_INPUT_FRAME_RATE in_fmRate = s_pContext->_external_data._input_frameRate;
	unsigned char me_clk_state = 0;

#ifdef MEMC_BRING_UP
	return;
#endif

	ReadRegister(SOFTWARE1_SOFTWARE1_63_reg, 30, 30, &memc_switch_en);
	ReadRegister(SOFTWARE1_SOFTWARE1_62_reg, 28, 31, &frm_num);
	me_clk_state = (rtd_inl(SYS_REG_SYS_CLKEN3_reg)>>30)&0x1;

	if(((MEMC_Lib_get_memc_bypass_to_enable_flag() == 1) || (memc_switch_frm_cnt >= 2)) && (DTV_PowerSaving_stage == 0)){
		g_memc_switch_state = 1;
	}else if((MEMC_Lib_get_memc_enable_to_bypass_flag() == 1) || (DTV_PowerSaving_stage != 0)){
		g_memc_switch_state = 2;
	}else{
		g_memc_switch_state = 0;
	}

	if(memc_switch_en == 1 && (g_memc_switch_state == 1))
	{
		if(MEMC_Lib_get_memc_bypass_to_enable_flag() == 1)
		{
			memc_switch_frm_cnt = frm_num;
			MEMC_Lib_set_memc_bypass_to_enable_flag(0);
			//set Repeat Mode
			Mid_Mode_SetMEMCFrameRepeatEnable(1);
			rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch][AA][%d,%d,%d,%x][%x,%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg), rtd_inl(RTHW_PHASE_phase_A4_reg), rtd_inl(RTHW_PHASE_phase_A8_reg));
		}
		else if(memc_switch_frm_cnt == frm_num-1)
		{
			if(VR360_en == 1){
				//set Repeat Mode
				Mid_Mode_SetMEMCFrameRepeatEnable(1);
				rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch][Repeat mode][%d,%d,%d,%x][%x,%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg), rtd_inl(RTHW_PHASE_phase_A4_reg), rtd_inl(RTHW_PHASE_phase_A8_reg));
				rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch][BB][%d,%d,%d,%d][%x,%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg), rtd_inl(RTHW_PHASE_phase_A4_reg), rtd_inl(RTHW_PHASE_phase_A8_reg));
			}else{
				if((u8_MEMCMode == 0) || (u8_MEMCMode == 4 && dejudder == 0) || (me_clk_state == 0)){
					//set Repeat Mode
					Mid_Mode_SetMEMCFrameRepeatEnable(1);
					g_power_saving_flag = 1;
					rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch][Repeat mode][%d,%d,%d,%x][%x,%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg), rtd_inl(RTHW_PHASE_phase_A4_reg), rtd_inl(RTHW_PHASE_phase_A8_reg));
				}else{
					//set Blending
					Mid_Mode_SetMEMCFrameRepeatEnable(0);
					g_power_saving_flag = 1;
					rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch][Blending mode][%d,%d,%d,%x][%x,%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg), rtd_inl(RTHW_PHASE_phase_A4_reg), rtd_inl(RTHW_PHASE_phase_A8_reg));
				}
			}
		}
		else if(memc_switch_frm_cnt == frm_num-2)
		{
			memc_switch_frm_cnt = 1;
		}
		if((memc_switch_frm_cnt > 2) && (memc_switch_frm_cnt <= frm_num))
		{
//FPGA_BRINGUP			rtd_pr_memc_debug( "$$$   [jerry_MEMC_bypass_switch_test_02][%d,%d,%d,%d][%x]   $$$\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg), rtd_inl(KPHASE_kphase_90_reg));
			rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch][CC][%d,%d,%d,%x][%x,%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg), rtd_inl(RTHW_PHASE_phase_A4_reg), rtd_inl(RTHW_PHASE_phase_A8_reg));
			memc_switch_frm_cnt--;
		}

		memc_bypass_state = 1;
	}
	else if(memc_switch_en == 1 && (g_memc_switch_state == 2)){
		if(DTV_PowerSaving_stage == 1){
			//set Repeat Mode
			Mid_Mode_SetMEMCFrameRepeatEnable(1);
	//		rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch][ME_CLK_off][BB][,%x,%x,%x,%x,%x,%x,%x,%x,]\n", rtd_inl(0xb809aea8), rtd_inl(0xb80996b0)&0x7, rtd_inl(0xb8099750)&0x3, (rtd_inl(0xb8099798)>>8)&0x1, rtd_inl(RTHW_MC_DMA_MC_LF_P_DMA_RD_Ctrl_reg), rtd_inl(RTHW_MC_DMA_MC_HF_P_DMA_RD_Ctrl_reg), (rtd_inl(0xb809c5c0)>>9)&0x1, (rtd_inl(0xb8099628)>>14)&0x1);
			DTV_PowerSaving_stage = 2;
		}else if(DTV_PowerSaving_stage == 2){
			//ME CLK off
			rtd_outl(SYS_REG_SYS_CLKEN3_reg, 0x40000000);//IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);
			rtd_outl(SYS_REG_SYS_SRST3_reg, 0x40000000);//IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);
			//MC func CLK off
			rtd_outl(SYS_REG_SYS_CLKEN4_reg, 0x00000004);
			rtd_outl(SYS_REG_SYS_SRST4_reg, 0x00000004);

//FPGA_BRINGUP			rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch][ME_CLK_off][CC][,%x,%x,%x,%x,%x,%x,%x,%x,]\n", rtd_inl(0xb809aea8), rtd_inl(0xb80996b0)&0x7, rtd_inl(0xb8099750)&0x3, (rtd_inl(0xb8099798)>>8)&0x1, rtd_inl(RTHW_MC_DMA_MC_LF_P_DMA_RD_Ctrl_reg), rtd_inl(RTHW_MC_DMA_MC_HF_P_DMA_RD_Ctrl_reg), (rtd_inl(0xb809c5c0)>>9)&0x1, (rtd_inl(0xb8099628)>>14)&0x1);
			DTV_PowerSaving_stage = 3;
		}
		else if(DTV_PowerSaving_stage == 3)
		{
			MEMC_Lib_set_memc_freeze_status_flag(0);
//FPGA_BRINGUP			rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch][ME_CLK_off][DD][,%x,%x,%x,%x,%x,%x,%x,%x,]\n", rtd_inl(0xb809aea8), rtd_inl(0xb80996b0)&0x7, rtd_inl(0xb8099750)&0x3, (rtd_inl(0xb8099798)>>8)&0x1, rtd_inl(RTHW_MC_DMA_MC_LF_P_DMA_RD_Ctrl_reg), rtd_inl(RTHW_MC_DMA_MC_HF_P_DMA_RD_Ctrl_reg), (rtd_inl(0xb809c5c0)>>9)&0x1, (rtd_inl(0xb8099628)>>14)&0x1);
			DTV_PowerSaving_stage = 0;
		}
		memc_switch_frm_cnt = 1;
		
		MEMC_Lib_set_memc_enable_to_bypass_flag(0);
		
//FPGA_BRINGUP		rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch][DD][%d,%d,%d,%d][%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg), rtd_inl(KPHASE_kphase_90_reg));
	}
	else if(memc_switch_en == 0){
		if(memc_bypass_state == 1){
			if((u8_MEMCMode == 0) || (u8_MEMCMode == 4 && dejudder == 0) || (me_clk_state == 0)){
				//set Repeat Mode
				Mid_Mode_SetMEMCFrameRepeatEnable(1);
				g_power_saving_flag = 1;
//FPGA_BRINGUP				rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch][Repeat mode][%d,%d,%d,%x][%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg), rtd_inl(KPHASE_kphase_90_reg));
			}else{
				//set Blending
				Mid_Mode_SetMEMCFrameRepeatEnable(0);
				g_power_saving_flag = 1;
//FPGA_BRINGUP				rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch][Blending mode][%d,%d,%d,%x][%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg), rtd_inl(KPHASE_kphase_90_reg));
			}
			memc_switch_frm_cnt = 1;
			memc_bypass_state = 0;
		}
	}
}

VOID patch_MEMC_bypass_switch_HDMI(const _PARAM_PATCH_CTRL *pParam, _OUTPUT_PATCH_CTRL *pOutput)
{
	rthw_phase_kphase_10_RBUS rthw_phase_kphase_10;
	unsigned int memc_switch_en;
	static unsigned int memc_switch_frm_cnt = 1;
	static unsigned int memc_bypass_state = 0;
	unsigned int frm_num = 14;
	unsigned int cur_cadence = 0;
	unsigned int buffer_size = 0;
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	PQL_INPUT_FRAME_RATE in_fmRate = s_pContext->_external_data._input_frameRate;

	cur_cadence = s_pContext->_output_filmDetectctrl.u8_cur_cadence_out[0];

	ReadRegister(SOFTWARE1_SOFTWARE1_63_reg, 30, 30, &memc_switch_en);
	ReadRegister(SOFTWARE1_SOFTWARE1_62_reg, 24, 27, &frm_num);
	rthw_phase_kphase_10.regValue = rtd_inl(RTHW_PHASE_kphase_10_reg);
	buffer_size = rthw_phase_kphase_10.kphase_in_mc_index_bsize;

#ifdef MEMC_BRING_UP
	return;
#endif

	if(memc_switch_en == 1){
		if(MEMC_Lib_HDMI_get_memc_bypass_to_enable_flag() == 1){
			memc_switch_frm_cnt = frm_num;
			MEMC_Lib_HDMI_set_memc_bypass_to_enable_flag(0);
			//set Repeat Mode
			Mid_Mode_SetMEMCFrameRepeatEnable(1);
			rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch_HDMI][AA][%d,%d,%d,%d][%x,%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg),  rtd_inl(RTHW_PHASE_phase_A4_reg), rtd_inl(RTHW_PHASE_phase_A8_reg));
		}
		else if(memc_switch_frm_cnt == frm_num-1){
			if((MEMC_LibGetMEMC_PCModeEnable()) || (u8_MEMCMode == 0) || (u8_MEMCMode == 4 && dejudder == 0)){
				//set Repeat Mode
				Mid_Mode_SetMEMCFrameRepeatEnable(1);
				g_power_saving_flag = 1;
				rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch_HDMI][Repeat mode][,%d,%d,%d,%x,][,%x,%x,][,%d,%d,%d,]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg),  rtd_inl(RTHW_PHASE_phase_A4_reg), rtd_inl(RTHW_PHASE_phase_A8_reg), MEMC_LibGetMEMC_PCModeEnable(), u8_MEMCMode, dejudder);
			}else{
				//set Blending
				Mid_Mode_SetMEMCFrameRepeatEnable(0);
				g_power_saving_flag = 1;
				rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch_HDMI][Blending mode][,%d,%d,%d,%x,][,%x,%x,][,%d,%d,%d,]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg),  rtd_inl(RTHW_PHASE_phase_A4_reg), rtd_inl(RTHW_PHASE_phase_A8_reg), MEMC_LibGetMEMC_PCModeEnable(), u8_MEMCMode, dejudder);
			}
		}
		else if(memc_switch_frm_cnt == frm_num-2){
			memc_switch_frm_cnt = 1;
		}

		if((memc_switch_frm_cnt > 2) && (memc_switch_frm_cnt <= frm_num)){
			rtd_pr_memc_debug( "[PowerSaving_MEMC_bypass_switch_HDMI][BB][%d,%d,%d,%d][%x,%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg),  rtd_inl(RTHW_PHASE_phase_A4_reg), rtd_inl(RTHW_PHASE_phase_A8_reg));
			memc_switch_frm_cnt--;
		}

		if(memc_switch_frm_cnt >= 2)
			g_memc_hdmi_switch_state = 1;
		else
			g_memc_hdmi_switch_state = 0;

		memc_bypass_state = 1;
	}
	else{
		if(memc_bypass_state == 1){
			if((MEMC_LibGetMEMC_PCModeEnable()) || (u8_MEMCMode == 0) || (u8_MEMCMode == 4 && dejudder == 0)){
				//set Repeat Mode
				Mid_Mode_SetMEMCFrameRepeatEnable(1);
				g_power_saving_flag = 1;
				rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch_HDMI][Repeat mode][IZ][%d,%d,%d,%x][%x,%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg),  rtd_inl(RTHW_PHASE_phase_A4_reg), rtd_inl(RTHW_PHASE_phase_A8_reg));
			}else{
				//set Blending
				Mid_Mode_SetMEMCFrameRepeatEnable(0);
				g_power_saving_flag = 1;
				rtd_pr_memc_notice("[PowerSaving_MEMC_bypass_switch_HDMI][Blending mode][%d,%d,%d,%x][%x,%x]\n\r", memc_switch_frm_cnt, in_fmRate, rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg),  rtd_inl(RTHW_PHASE_phase_A4_reg), rtd_inl(RTHW_PHASE_phase_A8_reg));
			}
			memc_bypass_state = 0;
		}
	}
}
#endif
////////////////////////////////////////////////////////////////////////

VOID Patch_detect_RTK2885pp(const _PARAM_PATCH_CTRL *pParam, _OUTPUT_PATCH_CTRL *pOutput)
{

}

VOID Patch_Action_RTK2885pp(const _PARAM_PATCH_CTRL *pParam, _OUTPUT_PATCH_CTRL *pOutput)
{
	patch_MEMC_bypass_switch(pParam, pOutput);

	patch_MEMC_bypass_switch_HDMI(pParam, pOutput);
}

VOID PatchCtrl_Init_RTK2885pp(_OUTPUT_PATCH_CTRL *pOutput)
{
    pOutput->u1_ID_001_apply = 0;
}

VOID PatchCtrl_Proc_RTK2885pp(const _PARAM_PATCH_CTRL *pParam, _OUTPUT_PATCH_CTRL *pOutput)
{
	//if(pParam->u1_patch_M_en == 0)
	//	return;
	
	Patch_detect_RTK2885pp(pParam, pOutput);
	Patch_Action_RTK2885pp(pParam, pOutput);
}

