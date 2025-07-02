/**
 * @file hal_crtc.c
 * @brief This file is for crtc register setting
 * @date Nov.14.2014
 * @version V0.0.1
 * @author Martin Yan
 * @par Copyright (C) 2014 Real-Image Co. LTD
 * @par History:
 *			Version: 0.0.1
*/

#include "memc_isr/Common/kw_debug.h"
#include "memc_isr/Common/kw_common.h"
#include "memc_isr/Driver/regio.h"
#include "memc_isr/Common/memc_type.h"
#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/Platform/memc_change_size.h"

#include "memc_reg_def.h"

extern void MEMC_ChangeSize_CRTC_to_scaler(void);

#ifndef BUILD_QUICK_SHOW
static DEFINE_SPINLOCK(memc_task_Spinlock);
#endif
/**
 * @brief This function set frc timing
 * @param [in] TIMING_PARA_ST stFRCTiming
 * @retval VOID
*/
VOID HAL_CRTC_SetFRCTiming_RTK2885p(TIMING_PARA_ST stFRCTiming)
{
#if (IC_K6LP || IC_K8LP || IC_K24)
	//reg_crtc1_frc_hact[11:0] --- 2 pix mode
	WriteRegister(CRTC1_CRTC1_14_reg, 0, 11, stFRCTiming.u32HActive/2);
	//reg_crtc1_frc_vact[11:0]
	WriteRegister(CRTC1_CRTC1_14_reg, 12, 24, stFRCTiming.u32VActive);
	//reg_crtc1_frc_hs_bporch[11:0]
	WriteRegister(CRTC1_CRTC1_18_reg, 0, 11, stFRCTiming.u32HSBPorch/4);
	//reg_crtc1_frc_vs_bporch[11:0]
	WriteRegister(CRTC1_CRTC1_18_reg, 12, 23, stFRCTiming.u32VSBPorch);
#else
	//reg_crtc1_frc_hact[11:0]
	WriteRegister(CRTC1_CRTC1_14_reg, 0, 11, stFRCTiming.u32HActive/4);
	//reg_crtc1_frc_vact[11:0]
	WriteRegister(CRTC1_CRTC1_14_reg, 12, 24, stFRCTiming.u32VActive);
	//reg_crtc1_frc_hs_bporch[11:0]
	WriteRegister(CRTC1_CRTC1_18_reg, 0, 11, stFRCTiming.u32HSBPorch/4);
	//reg_crtc1_frc_vs_bporch[11:0]
	WriteRegister(CRTC1_CRTC1_18_reg, 12, 23, stFRCTiming.u32VSBPorch);
#endif
}
VOID HAL_CRTC_SetFRCTiming_RTK2885pp(TIMING_PARA_ST stFRCTiming)
{
	M8P_rthw_crtc1_crtc1_14_RBUS M8P_rthw_crtc1_crtc1_14;
	M8P_rthw_crtc1_crtc1_18_RBUS M8P_rthw_crtc1_crtc1_18;

	M8P_rthw_crtc1_crtc1_14.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_14_reg);
	M8P_rthw_crtc1_crtc1_18.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_18_reg);

	M8P_rthw_crtc1_crtc1_14.crtc1_frc_hact = stFRCTiming.u32HActive/4;
	M8P_rthw_crtc1_crtc1_14.crtc1_frc_vact = stFRCTiming.u32VActive;
	M8P_rthw_crtc1_crtc1_18.crtc1_frc_hs_bporch = stFRCTiming.u32HSBPorch/4;
	M8P_rthw_crtc1_crtc1_18.crtc1_frc_vs_bporch = stFRCTiming.u32VSBPorch;

	rtd_outl(M8P_RTHW_CRTC1_CRTC1_14_reg, M8P_rthw_crtc1_crtc1_14.regValue);
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_18_reg, M8P_rthw_crtc1_crtc1_18.regValue);


}
VOID HAL_CRTC_SetFRCTiming(TIMING_PARA_ST stFRCTiming)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_SetFRCTiming_RTK2885pp(stFRCTiming);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_SetFRCTiming_RTK2885p(stFRCTiming);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set osd timing
 * @param [in] u32Hact
 * @param [in] u32Vact
 * @retval VOID
*/
VOID HAL_CRTC_SetOSDTiming_RTK2885p(unsigned int u32Hact, unsigned int u32Vact)
{
	//reg_crtc1_osd_hact
	WriteRegister(CRTC1_CRTC1_0C_reg, 0, 11, u32Hact);
	//reg_crtc1_osd_vact
	WriteRegister(CRTC1_CRTC1_0C_reg, 12, 24, u32Vact);
}
VOID HAL_CRTC_SetOSDTiming_RTK2885pp(unsigned int u32Hact, unsigned int u32Vact)
{
	M8P_rthw_crtc1_crtc1_0c_RBUS M8P_rthw_crtc1_crtc1_0c;

	M8P_rthw_crtc1_crtc1_0c.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_0C_reg);
	M8P_rthw_crtc1_crtc1_0c.crtc1_osd_hact = u32Hact;
	M8P_rthw_crtc1_crtc1_0c.crtc1_osd_vact = u32Vact;
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_0C_reg, M8P_rthw_crtc1_crtc1_0c.regValue);
}
VOID HAL_CRTC_SetOSDTiming(unsigned int u32Hact, unsigned int u32Vact)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_SetOSDTiming_RTK2885pp(u32Hact, u32Vact);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_SetOSDTiming_RTK2885p(u32Hact, u32Vact);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set panel timing
 * @param [in] TIMING_PARA_ST stFRCTiming
 * @retval VOID
*/
VOID HAL_CRTC_SetPanelTiming_RTK2885p(PANEL_PARAMETERS_ST stPanelTiming)
{
#if (IC_K6LP || IC_K8LP || IC_K24)
	//reg_crtc1_out_htotal[11:0] --- 2 pix mode
	WriteRegister(CRTC1_CRTC1_00_reg, 0, 11, stPanelTiming.u32Htotal/2);
	//reg_crtc1_out_vtotal[12:0]
	WriteRegister(CRTC1_CRTC1_00_reg, 12, 24, stPanelTiming.u32Vtotal);
	//reg_crtc1_out_hact[11:0]
	WriteRegister(CRTC1_CRTC1_04_reg, 0, 11, stPanelTiming.u32Hactive/4);
	//reg_crtc1_out_vact[12:0]
	WriteRegister(CRTC1_CRTC1_04_reg, 12, 24, stPanelTiming.u32Vactive);
	//reg_crtc1_out_hs_width[6:0]
	WriteRegister(CRTC1_CRTC1_00_reg, 25, 31, stPanelTiming.u32HSWidth/4);
	//reg_crtc1_out_vs_width[6:0]
	WriteRegister(CRTC1_CRTC1_04_reg, 25, 31, stPanelTiming.u32VSWidth);
	//reg_crtc1_out_hs_bporch[11:0]
	WriteRegister(CRTC1_CRTC1_08_reg, 0, 11, stPanelTiming.u32HSBPorch/4);
	//reg_crtc1_out_vs_bporch[11:0]
	WriteRegister(CRTC1_CRTC1_08_reg, 12, 23, stPanelTiming.u32VSBPorch);
	//reg_crtc1_vspll_max_v_total
	WriteRegister(CRTC1_CRTC1_64_reg, 0, 12, stPanelTiming.u32VtotalMax);
#else
	//reg_crtc1_out_htotal[11:0]
	WriteRegister(CRTC1_CRTC1_00_reg, 0, 11, stPanelTiming.u32Htotal/4);
	//reg_crtc1_out_vtotal[12:0]
	WriteRegister(CRTC1_CRTC1_00_reg, 12, 24, stPanelTiming.u32Vtotal);
	//reg_crtc1_out_hact[11:0]
	WriteRegister(CRTC1_CRTC1_04_reg, 0, 11, stPanelTiming.u32Hactive/4);
	//reg_crtc1_out_vact[12:0]
	WriteRegister(CRTC1_CRTC1_04_reg, 12, 24, stPanelTiming.u32Vactive);
	//reg_crtc1_out_hs_width[6:0]
	WriteRegister(CRTC1_CRTC1_00_reg, 25, 31, stPanelTiming.u32HSWidth/4);
	//reg_crtc1_out_vs_width[6:0]
	WriteRegister(CRTC1_CRTC1_04_reg, 25, 31, stPanelTiming.u32VSWidth);
	//reg_crtc1_out_hs_bporch[11:0]
	WriteRegister(CRTC1_CRTC1_08_reg, 0, 11, stPanelTiming.u32HSBPorch/4);
	//reg_crtc1_out_vs_bporch[11:0]
	WriteRegister(CRTC1_CRTC1_08_reg, 12, 23, stPanelTiming.u32VSBPorch);
	//reg_crtc1_vspll_max_v_total
	WriteRegister(CRTC1_CRTC1_64_reg, 0, 12, stPanelTiming.u32VtotalMax);
	//reg_crtc1_vspll_min_v_total
	//WriteRegister(CRTC1_CRTC1_64_reg,13,25,stPanelTiming.u32VtotalMin);
#endif
}
VOID HAL_CRTC_SetPanelTiming_RTK2885pp(PANEL_PARAMETERS_ST stPanelTiming)
{
	M8P_rthw_crtc1_crtc1_00_RBUS M8P_rthw_crtc1_crtc1_00;
	M8P_rthw_crtc1_crtc1_04_RBUS M8P_rthw_crtc1_crtc1_04;
	M8P_rthw_crtc1_crtc1_08_RBUS M8P_rthw_crtc1_crtc1_08;
	M8P_rthw_crtc1_crtc1_64_RBUS M8P_rthw_crtc1_crtc1_64;

	M8P_rthw_crtc1_crtc1_00.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_00_reg);
	M8P_rthw_crtc1_crtc1_04.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_04_reg);
	M8P_rthw_crtc1_crtc1_08.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_08_reg);
	M8P_rthw_crtc1_crtc1_64.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_64_reg);
	
	M8P_rthw_crtc1_crtc1_00.crtc1_out_htotal = stPanelTiming.u32Htotal/4;
	M8P_rthw_crtc1_crtc1_00.crtc1_out_vtotal = stPanelTiming.u32Vtotal;
	M8P_rthw_crtc1_crtc1_00.crtc1_out_hs_width = stPanelTiming.u32HSWidth/4;
	M8P_rthw_crtc1_crtc1_04.crtc1_out_hact = stPanelTiming.u32Hactive/4;
	M8P_rthw_crtc1_crtc1_04.crtc1_out_vact = stPanelTiming.u32Vactive;
	M8P_rthw_crtc1_crtc1_04.crtc1_out_vs_width = stPanelTiming.u32VSWidth;
	M8P_rthw_crtc1_crtc1_08.crtc1_out_hs_bporch = stPanelTiming.u32HSBPorch/4;
	M8P_rthw_crtc1_crtc1_08.crtc1_out_vs_bporch = stPanelTiming.u32VSBPorch;
	M8P_rthw_crtc1_crtc1_64.crtc1_vspll_max_v_total = stPanelTiming.u32VtotalMax;
	
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_00_reg, M8P_rthw_crtc1_crtc1_00.regValue);
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_04_reg, M8P_rthw_crtc1_crtc1_04.regValue);
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_08_reg, M8P_rthw_crtc1_crtc1_08.regValue);
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_64_reg, M8P_rthw_crtc1_crtc1_64.regValue);
}
VOID HAL_CRTC_SetPanelTiming(PANEL_PARAMETERS_ST stPanelTiming)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_SetPanelTiming_RTK2885pp(stPanelTiming);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_SetPanelTiming_RTK2885p(stPanelTiming);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if REMOVE_REDUNDANT_API
/**
 * @brief This function get ME orig vtrig delay
 * @param [in] VOID
 * @retval unsigned int u32ME_Orig_Vtrig_dly
*/
unsigned int HAL_CRTC_GetMEOrigVtrigDelay(VOID)
{
	unsigned int u32ME_Orig_Vtrig_dly = 0;
	ReadRegister(CRTC1_CRTC1_1C_reg, 13, 25, &u32ME_Orig_Vtrig_dly);
	return u32ME_Orig_Vtrig_dly;
}

/**
 * @brief This function set ME orig vtrig delay
 * @param [in] unsigned int u32Val
 * @retval VOID
*/
VOID HAL_CRTC_SetMEOrigVtrigDelay(unsigned int u32Val)
{
	WriteRegister(CRTC1_CRTC1_1C_reg, 13, 25, u32Val);
}

/**
 * @brief This function get ME vtrig delay
 * @param [in] VOID
 * @retval unsigned int u32ME_Vtrig_dly
*/
unsigned int HAL_CRTC_GetMEVtrigDelay(VOID)
{
	unsigned int u32ME_Vtrig_dly = 0;
	ReadRegister(CRTC1_CRTC1_24_reg, 0, 5, &u32ME_Vtrig_dly);
	return u32ME_Vtrig_dly;
}

/**
 * @brief This function get MC vtrig delay
 * @param [in] VOID
 * @retval unsigned int u32MC_Vtrig_dly
*/
unsigned int HAL_CRTC_GetMCVtrigDelay(VOID)
{
	unsigned int u32MC_Vtrig_dly = 0;
	ReadRegister(CRTC1_CRTC1_28_reg, 8, 15, &u32MC_Vtrig_dly);
	return u32MC_Vtrig_dly;
}
#endif// REMOVE_REDUNDANT_API

/**
 * @brief This function set frc_vs_bporch
 * @param [in] unsigned int u32FRC_VS_Bporch
 * @retval VOID
*/
VOID HAL_CRTC_SetFRCVSBporch_RTK2885p(unsigned int u32FRC_VS_Bporch)
{
	//reg_crtc1_frc_vs_bporch[23:12]
	crtc1_crtc1_18_RBUS crtc1_crtc1_18_reg;
	crtc1_crtc1_18_reg.regValue = rtd_inl(CRTC1_CRTC1_18_reg);

	crtc1_crtc1_18_reg.crtc1_frc_vs_bporch = u32FRC_VS_Bporch;
	rtd_outl(CRTC1_CRTC1_18_reg, crtc1_crtc1_18_reg.regValue);
}
VOID HAL_CRTC_SetFRCVSBporch_RTK2885pp(unsigned int u32FRC_VS_Bporch)
{
	M8P_rthw_crtc1_crtc1_18_RBUS M8P_rthw_crtc1_crtc1_18_reg;

	M8P_rthw_crtc1_crtc1_18_reg.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_18_reg);
	M8P_rthw_crtc1_crtc1_18_reg.crtc1_frc_vs_bporch = u32FRC_VS_Bporch;
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_18_reg, M8P_rthw_crtc1_crtc1_18_reg.regValue);
}
VOID HAL_CRTC_SetFRCVSBporch(unsigned int u32FRC_VS_Bporch)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_SetFRCVSBporch_RTK2885pp(u32FRC_VS_Bporch);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_SetFRCVSBporch_RTK2885p(u32FRC_VS_Bporch);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set frc vtrig delay
 * @param [in] unsigned int u32FRC_Vtrig_dly
 * @retval VOID
*/
VOID HAL_CRTC_SetFRCVtrigDelay_RTK2885p(unsigned int u32FRC_Vtrig_dly)
{
	//reg_crtc1_frc_vtrig_dly[9:0]
	unsigned long flags;
	
	#ifndef BUILD_QUICK_SHOW
	spin_lock_irqsave(&memc_task_Spinlock, flags);
	#endif
	
	WriteRegister(CRTC1_CRTC1_20_reg, 20, 29, u32FRC_Vtrig_dly);
	
	#ifndef BUILD_QUICK_SHOW
	spin_unlock_irqrestore(&memc_task_Spinlock, flags);
	#endif
}
VOID HAL_CRTC_SetFRCVtrigDelay_RTK2885pp(unsigned int u32FRC_Vtrig_dly)
{
	//reg_crtc1_frc_vtrig_dly[9:0]
	unsigned long flags;
	M8P_rthw_crtc1_crtc1_20_RBUS M8P_rthw_crtc1_crtc1_20;
	
	#ifndef BUILD_QUICK_SHOW
	spin_lock_irqsave(&memc_task_Spinlock, flags);
	#endif

	M8P_rthw_crtc1_crtc1_20.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_20_reg);
	M8P_rthw_crtc1_crtc1_20.crtc1_frc_vtrig_dly = u32FRC_Vtrig_dly;
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_20_reg, M8P_rthw_crtc1_crtc1_20.regValue);
	
	#ifndef BUILD_QUICK_SHOW
	spin_unlock_irqrestore(&memc_task_Spinlock, flags);
	#endif
}
VOID HAL_CRTC_SetFRCVtrigDelay(unsigned int u32FRC_Vtrig_dly)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_SetFRCVtrigDelay_RTK2885pp(u32FRC_Vtrig_dly);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_SetFRCVtrigDelay_RTK2885p(u32FRC_Vtrig_dly);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get frc vtrig delay
 * @param [in] unsigned int u32FRC_Vtrig_dly
 * @retval VOID
*/
unsigned int HAL_CRTC_GetFRCVtrigDelay_RTK2885p(VOID)
{
	unsigned int u32FRC_Vtrig_dly;
	ReadRegister(CRTC1_CRTC1_20_reg, 20, 29, &u32FRC_Vtrig_dly);
	return u32FRC_Vtrig_dly;
}
unsigned int HAL_CRTC_GetFRCVtrigDelay_RTK2885pp(VOID)
{
	M8P_rthw_crtc1_crtc1_20_RBUS M8P_rthw_crtc1_crtc1_20;
	unsigned int u32FRC_Vtrig_dly;

	M8P_rthw_crtc1_crtc1_20.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_20_reg);
	u32FRC_Vtrig_dly = M8P_rthw_crtc1_crtc1_20.crtc1_frc_vtrig_dly;
	return u32FRC_Vtrig_dly;
}
unsigned int HAL_CRTC_GetFRCVtrigDelay(VOID)
{
	unsigned int u32_VtrigDelay = 0;
	if(RUN_MERLIN8P()){
		u32_VtrigDelay = HAL_CRTC_GetFRCVtrigDelay_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_VtrigDelay = HAL_CRTC_GetFRCVtrigDelay_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_VtrigDelay;
}

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set frc vtrig delay
 * @param [in] unsigned int u32FRC_Vtrig_dly
 * @retval VOID
*/
VOID HAL_CRTC_SetLbmeVtrigDelay(unsigned int u32Lbme_Vtrig_dly)
{
	//reg_crtc1_lbme_vtrig_dly[12:0]
	WriteRegister(CRTC1_CRTC1_88_reg, 19, 31, u32Lbme_Vtrig_dly);
	WriteRegister(CRTC1_CRTC1_24_reg, 19, 31, u32Lbme_Vtrig_dly);
}

/**
 * @brief This function set dehalo vtrig delay
 * @param [in] unsigned int u32DEHALO_Vtrig_dly
 * @retval VOID
*/
VOID HAL_CRTC_SetDehaloVtrigDelay(unsigned int u32DEHALO_Vtrig_dly)
{
	//reg_crtc1_dehalo_vtrig_dly[7:0]
	WriteRegister(CRTC1_CRTC1_28_reg, 0, 7, u32DEHALO_Vtrig_dly);
}

/**
 * @brief This function set dec vtrig delay
 * @param [in] unsigned int u32DEC_Vtrig_dly
 * @retval VOID
*/
VOID HAL_CRTC_SetDecVtrigDelay(unsigned int u32DEC_Vtrig_dly)
{
	//reg_crtc1_dec_vtrig_dly[7:0]
	WriteRegister(CRTC1_CRTC1_24_reg, 6, 18, u32DEC_Vtrig_dly);
}

/**
 * @brief This function set mc vtrig delay
 * @param [in] unsigned int u32MC_Vtrig_dly
 * @retval VOID
*/
VOID HAL_CRTC_SetMCVtrigDelay(unsigned int u32MC_Vtrig_dly)
{
	//reg_crtc1_mc_vtrig_dly[7:0]
	WriteRegister(CRTC1_CRTC1_28_reg, 8, 15, u32MC_Vtrig_dly);
}
#endif// REMOVE_REDUNDANT_API

/**
 * @brief This function set mc vtrig delay long
 * @param [in] unsigned int u32MC_Vtrig_dly_long
 * @retval VOID
*/
VOID HAL_CRTC_SetMCVtrigDelayLong_RTK2885p(unsigned int u32MC_Vtrig_dly_long)
{
	//reg_crtc1_mc_vtrig_dly_long[28:16]
	unsigned long flags;
	
	#ifndef BUILD_QUICK_SHOW
	spin_lock_irqsave(&memc_task_Spinlock, flags);
	#endif
	
	WriteRegister(CRTC1_CRTC1_98_reg, 16, 28, u32MC_Vtrig_dly_long);

	#ifndef BUILD_QUICK_SHOW
	spin_unlock_irqrestore(&memc_task_Spinlock, flags);
	#endif
}
VOID HAL_CRTC_SetMCVtrigDelayLong_RTK2885pp(unsigned int u32MC_Vtrig_dly_long)
{
	unsigned long flags;
	M8P_rthw_crtc1_crtc1_98_RBUS M8P_rthw_crtc1_crtc1_98;
	
	#ifndef BUILD_QUICK_SHOW
	spin_lock_irqsave(&memc_task_Spinlock, flags);
	#endif

	M8P_rthw_crtc1_crtc1_98.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_98_reg);
	M8P_rthw_crtc1_crtc1_98.crtc1_mc_vtrig_dly_long = u32MC_Vtrig_dly_long;
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_98_reg, M8P_rthw_crtc1_crtc1_98.regValue);

	#ifndef BUILD_QUICK_SHOW
	spin_unlock_irqrestore(&memc_task_Spinlock, flags);
	#endif
}
VOID HAL_CRTC_SetMCVtrigDelayLong(unsigned int u32MC_Vtrig_dly_long)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_SetMCVtrigDelayLong_RTK2885pp(u32MC_Vtrig_dly_long);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_SetMCVtrigDelayLong_RTK2885p(u32MC_Vtrig_dly_long);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get mc vtrig delay long
 * @param [in] unsigned int u32MC_Vtrig_dly_long
 * @retval VOID
*/
unsigned int HAL_CRTC_GetMCVtrigDelayLong_RTK2885p(VOID)
{
	unsigned int u32MC_Vtrig_dly_long = 0;
	ReadRegister(CRTC1_CRTC1_98_reg, 16, 28, &u32MC_Vtrig_dly_long);
	return u32MC_Vtrig_dly_long;
}
unsigned int HAL_CRTC_GetMCVtrigDelayLong_RTK2885pp(VOID)
{
	unsigned int u32MC_Vtrig_dly_long = 0;
	M8P_rthw_crtc1_crtc1_98_RBUS M8P_rthw_crtc1_crtc1_98;

	M8P_rthw_crtc1_crtc1_98.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_98_reg);
	u32MC_Vtrig_dly_long = M8P_rthw_crtc1_crtc1_98.crtc1_mc_vtrig_dly_long;
	return u32MC_Vtrig_dly_long;
}
unsigned int HAL_CRTC_GetMCVtrigDelayLong(VOID)
{
	unsigned int u32_VtrigDelayLong = 0;

	if(RUN_MERLIN8P()){
		u32_VtrigDelayLong = HAL_CRTC_GetMCVtrigDelayLong_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_VtrigDelayLong = HAL_CRTC_GetMCVtrigDelayLong_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_VtrigDelayLong;
}

/**
 * @brief This function set lbmc vtrig delay
 * @param [in] unsigned int u32Lbmc_Vtrig_dly
 * @retval VOID
*/
VOID HAL_CRTC_SetLbmcVtrigDelay_RTK2885p(unsigned int u32Lbmc_Vtrig_dly)
{
	//reg_crtc1_lbmc_vtrig_dly[7:0]
	WriteRegister(CRTC1_CRTC1_28_reg, 16, 28, u32Lbmc_Vtrig_dly);
}
VOID HAL_CRTC_SetLbmcVtrigDelay_RTK2885pp(unsigned int u32Lbmc_Vtrig_dly)
{
	M8P_rthw_crtc1_crtc1_28_RBUS M8P_rthw_crtc1_crtc1_28;
	M8P_rthw_crtc1_crtc1_28.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_28_reg);
	M8P_rthw_crtc1_crtc1_28.crtc1_lbmc_vtrig_dly = u32Lbmc_Vtrig_dly;
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_28_reg, M8P_rthw_crtc1_crtc1_28.regValue);
}
VOID HAL_CRTC_SetLbmcVtrigDelay(unsigned int u32Lbmc_Vtrig_dly)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_SetLbmcVtrigDelay_RTK2885pp(u32Lbmc_Vtrig_dly);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_SetLbmcVtrigDelay_RTK2885p(u32Lbmc_Vtrig_dly);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set frc pre de l num
 * @param [in] unsigned int u32Frc_Prede_LNum
 * @retval VOID
*/
VOID HAL_CRTC_SetFrcPredeLNum(unsigned int u32Frc_Prede_LNum)
{
	//crtc1_frc_prede_lnum
	WriteRegister(CRTC1_CRTC1_14_reg, 30, 31, u32Frc_Prede_LNum);
}

#if 1
/**
 * @brief This function set me1 vactive
 * @param [in] unsigned int u32ME_Vactive
 * @retval VOID
*/
VOID HAL_CRTC_SetME1Vactive(unsigned int u32ME_Vactive)
{
	WriteRegister(CRTC1_CRTC1_8C_reg, 0, 12, u32ME_Vactive);	
}

/**
 * @brief This function set me2 vactive
 * @param [in] unsigned int u32ME_Vactive
 * @retval VOID
*/
VOID HAL_CRTC_SetME2Vactive(unsigned int u32ME_Vactive)
{
	WriteRegister(CRTC1_CRTC1_40_reg, 0, 12, u32ME_Vactive);	
}
#else
/**
 * @brief This function set me vactive
 * @param [in] unsigned int u32ME_Vactive
 * @retval VOID
*/
VOID HAL_CRTC_SetMEVactive(unsigned int u32ME_Vactive)
{
	//reg_crtc1_me_vact[12:0]
	WriteRegister(CRTC1_CRTC1_8C_reg,0,12,u32ME_Vactive);
	WriteRegister(CRTC1_CRTC1_40_reg,0,12,u32ME_Vactive);
}
#endif

/**
 * @brief This function set dehalo vactive
 * @param [in] unsigned int u32Dehalo_Vactive
 * @retval VOID
*/
VOID HAL_CRTC_SetDehaloVactive(unsigned int u32Dehalo_Vactive)
{
	//reg_crtc1_dehalo_vact[12:0]
	WriteRegister(CRTC1_CRTC1_40_reg, 13, 25, u32Dehalo_Vactive);
}

/**
 * @brief This function set mc vactive
 * @param [in] unsigned int u32MC_Vactive
 * @retval VOID
*/
VOID HAL_CRTC_SetMCVactive(unsigned int u32MC_Vactive)
{
	//reg_crtc1_mc_vact[12:0]
	WriteRegister(CRTC1_CRTC1_44_reg, 0, 12, u32MC_Vactive);
}

/**
 * @brief This function set pixel logo vactive
 * @param [in] unsigned int u32Plogo_Vactive
 * @retval VOID
*/
VOID HAL_CRTC_SetPLogoVactive(unsigned int u32Plogo_Vactive)
{
	WriteRegister(CRTC1_CRTC1_48_reg, 21, 31, u32Plogo_Vactive);	
}

/**
 * @brief This function set me1 row type
 * @param [in] unsigned int u32ME_Row_Type
 * @retval VOID
*/
VOID HAL_CRTC_SetME1RowType(unsigned int u32ME_Row_Type)
{
	WriteRegister(CRTC1_CRTC1_8C_reg, 13, 14, u32ME_Row_Type);
}

/**
 * @brief This function set me2 row type
 * @param [in] unsigned int u32ME_Row_Type
 * @retval VOID
*/
VOID HAL_CRTC_SetME2RowType(unsigned int u32ME_Row_Type)
{
	WriteRegister(CRTC1_CRTC1_40_reg, 26, 27, u32ME_Row_Type);
}

/**
 * @brief This function set dehalo row type
 * @param [in] unsigned int u32Dehalo_Row_Type
 * @retval VOID
*/
VOID HAL_CRTC_SetDehaloRowType(unsigned int u32Dehalo_Row_Type)
{
	//reg_crtc1_dehalo_row_type[1:0]
	WriteRegister(CRTC1_CRTC1_40_reg, 28, 29, u32Dehalo_Row_Type);
}

/**
 * @brief This function set mc row type
 * @param [in] unsigned int u32MC_Row_Type
 * @retval VOID
*/
VOID HAL_CRTC_SetMCRowType(unsigned int u32MC_Row_Type)
{
	//reg_crtc1_mc_row_type[1:0]
	WriteRegister(CRTC1_CRTC1_40_reg, 30, 31, u32MC_Row_Type);
}

/**
 * @brief This function set plogo row type
 * @param [in] unsigned int u32Plogo_Row_Type
 * @retval VOID
*/
VOID HAL_CRTC_SetPlogoRowType(unsigned int u32Plogo_Row_Type)
{
	//reg_crtc1_plogo_row_type
	WriteRegister(CRTC1_CRTC1_48_reg, 16, 17, u32Plogo_Row_Type);
}

/**
 * @brief This function set plogo fetch row type
 * @param [in] unsigned int u32Plogo_fetch_Row_Type
 * @retval VOID
*/
VOID HAL_CRTC_SetPlogoFetchRowType(unsigned int u32Plogo_fetch_Row_Type)
{
	//reg_crtc1_plogo_fetch_fetch_row_type
	WriteRegister(CRTC1_CRTC1_48_reg, 18, 19, u32Plogo_fetch_Row_Type);
}

/**
 * @brief This function set frc prede pnum
 * @param [in] unsigned int u32FRC_prede_pnum
 * @retval VOID
*/
VOID HAL_CRTC_SetFRCPreDEPNum(unsigned int u32FRC_prede_pnum)
{
	//reg_crtc1_frc_prede_pnum[4:0]
	WriteRegister(CRTC1_CRTC1_14_reg, 25, 29, u32FRC_prede_pnum);
}

/**
 * @brief This function set panel vtrig delay
 * @param [in] unsigned int u32Panel_Vtrig_dly
 * @retval VOID
*/
VOID HAL_CRTC_SetOutVtrigDelay(unsigned int u32Panel_Vtrig_dly)
{
	//reg_crtc1_out_vtrig_dly[9:0]
	WriteRegister(CRTC1_CRTC1_20_reg, 0, 9, u32Panel_Vtrig_dly);
}
#endif// REMOVE_REDUNDANT_API

/**
 * @brief This function set n to m
 * @param [in] unsigned int u32N
 * @param [in] unsigned int u32M
 * @retval VOID
*/
VOID HAL_CRTC_SetN2M_RTK2885p(unsigned int u32N, unsigned int u32M)
{
	#if (IC_K6LP || IC_K8LP || IC_K24)
	crtc1_crtc1_60_RBUS crtc1_crtc1_60;
	crtc1_crtc1_60.regValue = rtd_inl(CRTC1_CRTC1_60_reg);
	crtc1_crtc1_60.crtc1_n = u32N;
	crtc1_crtc1_60.crtc1_m = u32M;
	rtd_outl(CRTC1_CRTC1_60_reg, crtc1_crtc1_60.regValue);
	#else
	unsigned int u32Val = 0;
	ReadRegister(CRTC1_CRTC1_60_reg, 0, 31, &u32Val);
	u32Val = (u32Val & 0xFF0000FF) | (u32N << 8) | (u32M << 16);
	WriteRegister(CRTC1_CRTC1_60_reg, 0, 31, u32Val);
	#endif
}
VOID HAL_CRTC_SetN2M_RTK2885pp(unsigned int u32N, unsigned int u32M)
{
	M8P_rthw_crtc1_crtc1_60_RBUS M8P_rthw_crtc1_crtc1_60;
	M8P_rthw_crtc1_crtc1_60.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_60_reg);
	M8P_rthw_crtc1_crtc1_60.crtc1_n = u32N;
	M8P_rthw_crtc1_crtc1_60.crtc1_m = u32M;
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_60_reg, M8P_rthw_crtc1_crtc1_60.regValue);
}
VOID HAL_CRTC_SetN2M(unsigned int u32N, unsigned int u32M)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_SetN2M_RTK2885pp(u32N, u32M);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_SetN2M_RTK2885p(u32N, u32M);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set CRTC slave mode enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_CRTC_SetSlaveModeEnable(BOOL bEnable)
{
	//reg_crtc1_slave_mode_on
	WriteRegister(CRTC1_CRTC1_28_reg, 29, 29, (unsigned int)bEnable);
}

/**
 * @brief This function set lbmc normal lf delay
 * @param [in] unsigned char ubyIdx (0~8)
 * @param [in] unsigned char ubyDly
 * @retval VOID
*/
VOID HAL_CRTC_SetLBMCNormalLFDly(unsigned char ubyIdx, unsigned char ubyDly)
{
	switch(ubyIdx)
	{
		case 0:
			//reg_crtc1_lbmc_lfidx0_dly
			WriteRegister(CRTC1_CRTC1_2C_reg, 0, 7, ubyDly);
			break;
		case 1:
			//reg_crtc1_lbmc_lfidx1_dly
			WriteRegister(CRTC1_CRTC1_2C_reg, 8, 15, ubyDly);
			break;
		case 2:
			//reg_crtc1_lbmc_lfidx2_dly
			WriteRegister(CRTC1_CRTC1_2C_reg, 16, 23, ubyDly);
			break;
		case 3:
			//reg_crtc1_lbmc_lfidx3_dly
			WriteRegister(CRTC1_CRTC1_2C_reg, 24, 31, ubyDly);
			break;
		case 4:
			//reg_crtc1_lbmc_lfidx4_dly
			WriteRegister(CRTC1_CRTC1_30_reg, 0, 7, ubyDly);
			break;
		case 5:
			//reg_crtc1_lbmc_lfidx5_dly
			WriteRegister(CRTC1_CRTC1_30_reg, 8, 15, ubyDly);
			break;
		case 6:
			//reg_crtc1_lbmc_lfidx6_dly
			WriteRegister(CRTC1_CRTC1_30_reg, 16, 23, ubyDly);
			break;
		case 7:
			//reg_crtc1_lbmc_lfidx7_dly
			WriteRegister(CRTC1_CRTC1_30_reg, 24, 31, ubyDly);
			break;
		case 8:
			//reg_crtc1_lbmc_lfidx8_dly
			WriteRegister(CRTC1_CRTC1_34_reg, 0, 7, ubyDly);
			break;
		default:
			break;
	}
}

/**
 * @brief This function set lbmc normal hf delay
 * @param [in] unsigned char ubyIdx (0~3)
 * @param [in] unsigned char ubyDly
 * @retval VOID
*/
VOID HAL_CRTC_SetLBMCNormalHFDly(unsigned char ubyIdx, unsigned char ubyDly)
{
	switch(ubyIdx)
	{
		case 0:
			//reg_crtc1_lbmc_hfyidx0_dly
			WriteRegister(CRTC1_CRTC1_34_reg, 8, 15, ubyDly);
			break;
		case 1:
			//reg_crtc1_lbmc_hfyidx1_dly
			WriteRegister(CRTC1_CRTC1_34_reg, 16, 23, ubyDly);
			break;
		case 2:
			//reg_crtc1_lbmc_hfyidx2_dly
			WriteRegister(CRTC1_CRTC1_34_reg, 24, 31, ubyDly);
			break;
		case 3:
			//reg_crtc1_lbmc_hfcidx0_dly
			WriteRegister(CRTC1_CRTC1_38_reg, 0, 7, ubyDly);
			break;
		default:
			break;
	}
}

/**
 * @brief This function set lbmc signle lf delay
 * @param [in] unsigned char ubyIdx (0~8)
 * @param [in] unsigned char ubyDly
 * @retval VOID
*/
VOID HAL_CRTC_SetLBMCSingleLFDly(unsigned char ubyIdx, unsigned char ubyDly)
{
	switch(ubyIdx)
	{
		case 0:
			//reg_crtc1_lbmc_s_lfidx0_dly
			WriteRegister(CRTC1_CRTC1_74_reg, 0, 7, ubyDly);
			break;
		case 1:
			//reg_crtc1_lbmc_s_lfidx1_dly
			WriteRegister(CRTC1_CRTC1_74_reg, 8, 15, ubyDly);
			break;
		case 2:
			//reg_crtc1_lbmc_s_lfidx2_dly
			WriteRegister(CRTC1_CRTC1_74_reg, 16, 23, ubyDly);
			break;
		case 3:
			//reg_crtc1_lbmc_s_lfidx3_dly
			WriteRegister(CRTC1_CRTC1_74_reg, 24, 31, ubyDly);
			break;
		case 4:
			//reg_crtc1_lbmc_s_lfidx4_dly
			WriteRegister(CRTC1_CRTC1_78_reg, 0, 7, ubyDly);
			break;
		case 5:
			//reg_crtc1_lbmc_s_lfidx5_dly
			WriteRegister(CRTC1_CRTC1_78_reg, 8, 15, ubyDly);
			break;
		case 6:
			//reg_crtc1_lbmc_s_lfidx6_dly
			WriteRegister(CRTC1_CRTC1_78_reg, 16, 23, ubyDly);
			break;
		case 7:
			//reg_crtc1_lbmc_s_lfidx7_dly
			WriteRegister(CRTC1_CRTC1_78_reg, 24, 31, ubyDly);
			break;
		case 8:
			//reg_crtc1_lbmc_s_lfidx8_dly
			WriteRegister(CRTC1_CRTC1_7C_reg, 0, 7, ubyDly);
			break;
		default:
			break;
	}
}

/**
 * @brief This function set lbmc single hf delay
 * @param [in] unsigned char ubyIdx (0~3)
 * @param [in] unsigned char ubyDly
 * @retval VOID
*/
VOID HAL_CRTC_SetLBMCSingleHFDly(unsigned char ubyIdx, unsigned char ubyDly)
{
	switch(ubyIdx)
	{
		case 0:
			//reg_crtc1_lbmc_s_hfyidx0_dly
			WriteRegister(CRTC1_CRTC1_7C_reg, 16, 23, ubyDly);
			break;
		case 1:
			//reg_crtc1_lbmc_s_hfyidx1_dly
			WriteRegister(CRTC1_CRTC1_7C_reg, 24, 31, ubyDly);
			break;
		case 2:
			//reg_crtc1_lbmc_s_hfyidx2_dly
			WriteRegister(CRTC1_CRTC1_80_reg, 0, 7, ubyDly);
			break;
		case 3:
			//reg_crtc1_lbmc_s_hfyidx3_dly
			WriteRegister(CRTC1_CRTC1_80_reg, 8, 15, ubyDly);
			break;
		default:
			break;
	}
}

/**
 * @brief This function get vspll lock status
 * @retval True for lock
 * @retval False for unlock
*/
BOOL HAL_CRTC_GetVSPLLStatus(VOID)
{
#if (IC_K7LP || IC_K8LP || IC_K24)
	return FALSE;
#else
	unsigned int bPeriod_lock = FALSE;
	unsigned int bPhase_lock = FALSE;
	ReadRegister(CRTC1_CRTC1_F0_reg, 0, 0, &bPeriod_lock);
	ReadRegister(CRTC1_CRTC1_F0_reg, 1, 1, &bPhase_lock);
	if(bPeriod_lock && bPhase_lock)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
#endif
}
#endif//REMOVE_REDUNDANT_API

/**
 * @brief This function set crtc lr enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_CRTC_SetLREnable_RTK2885p(BOOL bEnable)
{
#if (IC_K7LP || IC_K8LP || IC_K24)
#else
	WriteRegister(CRTC1_CRTC1_90_reg, 29, 29, (unsigned int)bEnable);
#endif
}
VOID HAL_CRTC_SetLREnable_RTK2885pp(BOOL bEnable)
{
	M8P_rthw_crtc1_crtc1_90_RBUS M8P_rthw_crtc1_crtc1_90;
	M8P_rthw_crtc1_crtc1_90.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_90_reg);
	M8P_rthw_crtc1_crtc1_90.crtc1_3d_lr_enable = bEnable;
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_90_reg, M8P_rthw_crtc1_crtc1_90.regValue);
}
VOID HAL_CRTC_SetLREnable(BOOL bEnable)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_SetLREnable_RTK2885pp(bEnable);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_SetLREnable_RTK2885p(bEnable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set crtc lr cycle
 * @param [in] unsigned int u32Cycle
 * @retval VOID
*/
VOID HAL_CRTC_SetLRCycle_RTK2885p(unsigned int u32Cycle)
{
#if (IC_K7LP || IC_K8LP || IC_K24)
#else
	WriteRegister(CRTC1_CRTC1_90_reg, 26, 28, u32Cycle);
#endif
}
VOID HAL_CRTC_SetLRCycle_RTK2885pp(unsigned int u32Cycle)
{
	M8P_rthw_crtc1_crtc1_90_RBUS M8P_rthw_crtc1_crtc1_90;
	M8P_rthw_crtc1_crtc1_90.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_90_reg);
	M8P_rthw_crtc1_crtc1_90.crtc1_lr_p_cycle = u32Cycle;
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_90_reg, M8P_rthw_crtc1_crtc1_90.regValue);
}
VOID HAL_CRTC_SetLRCycle(unsigned int u32Cycle)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_SetLRCycle_RTK2885pp(u32Cycle);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_SetLRCycle_RTK2885p(u32Cycle);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set crtc lr pattern
 * @param [in] unsigned int u32Pattern
 * @retval VOID
*/
VOID HAL_CRTC_SetLRPattern_RTK2885p(unsigned int u32Pattern)
{
#if (IC_K7LP || IC_K8LP || IC_K24)
#else
	WriteRegister(CRTC1_CRTC1_90_reg, 0, 7, u32Pattern);
#endif
}
VOID HAL_CRTC_SetLRPattern_RTK2885pp(unsigned int u32Pattern)
{
	M8P_rthw_crtc1_crtc1_90_RBUS M8P_rthw_crtc1_crtc1_90;
	M8P_rthw_crtc1_crtc1_90.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_90_reg);
	M8P_rthw_crtc1_crtc1_90.crtc1_lr_p_patt = u32Pattern;
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_90_reg, M8P_rthw_crtc1_crtc1_90.regValue);
}
VOID HAL_CRTC_SetLRPattern(unsigned int u32Pattern)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_SetLRPattern_RTK2885pp(u32Pattern);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_SetLRPattern_RTK2885p(u32Pattern);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set crtc me to dehalo vtrig delay
 * @param [in] unsigned int u32Pre_dly
 * @param [in] unsigned int u32Pst_dly
 * @param [in] unsigned int u32Dob_dly
 * @retval VOID
*/
VOID HAL_CRTC_SetME2DHVtrigDly(unsigned int u32Pre_dly, unsigned int u32Pst_dly, unsigned int u32Dob_dly)
{
#if (IC_K7LP || IC_K8LP || IC_K24)
#else
	WriteRegister(CRTC1_CRTC1_94_reg, 0, 7, u32Pre_dly);	
	WriteRegister(CRTC1_CRTC1_94_reg, 8, 15, u32Pst_dly);	
	WriteRegister(CRTC1_CRTC1_94_reg, 16, 23, u32Dob_dly);	
#endif
}
#endif//REMOVE_REDUNDANT_API

VOID HAL_CRTC_Set_LBMC_neg_en_RTK2885p(unsigned char u1_enable)
{
#if (IC_K24)
	crtc1_crtc1_a8_RBUS crtc1_crtc1_a8_reg;
	crtc1_crtc1_a8_reg.regValue = rtd_inl(CRTC1_CRTC1_A8_reg);
	crtc1_crtc1_a8_reg.crtc1_vtrig_lbmc_neg_en = u1_enable;
	rtd_outl(CRTC1_CRTC1_A8_reg, crtc1_crtc1_a8_reg.regValue);
#endif
}
VOID HAL_CRTC_Set_LBMC_neg_en_RTK2885pp(unsigned char u1_enable)
{
	M8P_rthw_crtc1_crtc1_a8_RBUS M8P_rthw_crtc1_crtc1_a8;
	M8P_rthw_crtc1_crtc1_a8.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_A8_reg);
	M8P_rthw_crtc1_crtc1_a8.crtc1_vtrig_lbmc_neg_en = u1_enable;
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_A8_reg, M8P_rthw_crtc1_crtc1_a8.regValue);
}
VOID HAL_CRTC_Set_LBMC_neg_en(unsigned char u1_enable)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_Set_LBMC_neg_en_RTK2885pp(u1_enable);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_Set_LBMC_neg_en_RTK2885p(u1_enable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

VOID HAL_CRTC_Set_LBMC_neg_vtrig_dly_RTK2885p(unsigned int dly)
{
#if (IC_K24)
	crtc1_crtc1_a8_RBUS crtc1_crtc1_a8_reg;
	crtc1_crtc1_a8_reg.regValue = rtd_inl(CRTC1_CRTC1_A8_reg);
	crtc1_crtc1_a8_reg.crtc1_lbmc_neg_vtrig_dly = dly;
	rtd_outl(CRTC1_CRTC1_A8_reg, crtc1_crtc1_a8_reg.regValue);
#endif
}
VOID HAL_CRTC_Set_LBMC_neg_vtrig_dly_RTK2885pp(unsigned int dly)
{
	M8P_rthw_crtc1_crtc1_a8_RBUS M8P_rthw_crtc1_crtc1_a8;
	M8P_rthw_crtc1_crtc1_a8.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_A8_reg);
	M8P_rthw_crtc1_crtc1_a8.crtc1_lbmc_neg_vtrig_dly = dly;
	rtd_outl(M8P_RTHW_CRTC1_CRTC1_A8_reg, M8P_rthw_crtc1_crtc1_a8.regValue);
}
VOID HAL_CRTC_Set_LBMC_neg_vtrig_dly(unsigned int dly)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_Set_LBMC_neg_vtrig_dly_RTK2885pp(dly);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_Set_LBMC_neg_vtrig_dly_RTK2885p(dly);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

unsigned int HAL_CRTC_Get_LBMC_neg_vtrig_dly_RTK2885p(VOID)
{
#if (IC_K24)
	crtc1_crtc1_a8_RBUS crtc1_crtc1_a8_reg;
	crtc1_crtc1_a8_reg.regValue = rtd_inl(CRTC1_CRTC1_A8_reg);
	return crtc1_crtc1_a8_reg.crtc1_lbmc_neg_vtrig_dly;
#endif
}
unsigned int HAL_CRTC_Get_LBMC_neg_vtrig_dly_RTK2885pp(VOID)
{
	M8P_rthw_crtc1_crtc1_a8_RBUS M8P_rthw_crtc1_crtc1_a8;
	M8P_rthw_crtc1_crtc1_a8.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_A8_reg);
	return M8P_rthw_crtc1_crtc1_a8.crtc1_lbmc_neg_vtrig_dly;
}
unsigned int HAL_CRTC_Get_LBMC_neg_vtrig_dly(VOID)
{
	unsigned int u32_LBMC_neg_vtrig = 0;

	if(RUN_MERLIN8P()){
		u32_LBMC_neg_vtrig = HAL_CRTC_Get_LBMC_neg_vtrig_dly_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_LBMC_neg_vtrig = HAL_CRTC_Get_LBMC_neg_vtrig_dly_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_LBMC_neg_vtrig;
}

VOID HAL_CRTC_Set_DMA_Raw_PCMode_en_RTK2885p(unsigned char u1_enable)
{
#if (IC_K24)
	mc_dma_mc_lf_i_dma_rd_ctrl_RBUS mc_dma_mc_lf_i_dma_rd_ctrl_reg;
	mc_dma_mc_lf_i_dma_rd_ctrl_reg.regValue = rtd_inl(MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg);
	mc_dma_mc_lf_i_dma_rd_ctrl_reg.force_dma_raw_pc_mode_en = u1_enable;
	rtd_outl(MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg, mc_dma_mc_lf_i_dma_rd_ctrl_reg.regValue);
#endif
}
VOID HAL_CRTC_Set_DMA_Raw_PCMode_en_RTK2885pp(unsigned char u1_enable)
{
	M8P_rthw_mc_dma_mc_lf_i_dma_rd_ctrl_RBUS M8P_rthw_mc_dma_mc_lf_i_dma_rd_ctrl;
	M8P_rthw_mc_dma_mc_lf_i_dma_rd_ctrl.regValue = rtd_inl(M8P_RTHW_MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg);
	M8P_rthw_mc_dma_mc_lf_i_dma_rd_ctrl.force_dma_raw_pc_mode_en = u1_enable;
	rtd_outl(M8P_RTHW_MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg, M8P_rthw_mc_dma_mc_lf_i_dma_rd_ctrl.regValue);
}
VOID HAL_CRTC_Set_DMA_Raw_PCMode_en(unsigned char u1_enable)
{
	if(RUN_MERLIN8P()){
		HAL_CRTC_Set_DMA_Raw_PCMode_en_RTK2885pp(u1_enable);
	}else if(RUN_MERLIN8()){
		HAL_CRTC_Set_DMA_Raw_PCMode_en_RTK2885p(u1_enable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get frc hact
 * @param VOID
 * @retval u32FRC_hact
*/
unsigned int HAL_CRTC_GetFRCHact_RTK2885p(VOID)
{
	unsigned int u32FRC_hact;
	ReadRegister(CRTC1_CRTC1_14_reg, 0, 11, &u32FRC_hact);
	return u32FRC_hact;
}
unsigned int HAL_CRTC_GetFRCHact_RTK2885pp(VOID)
{
	unsigned int u32FRC_hact;
	M8P_rthw_crtc1_crtc1_14_RBUS M8P_rthw_crtc1_crtc1_14;
	M8P_rthw_crtc1_crtc1_14.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_14_reg);
	u32FRC_hact = M8P_rthw_crtc1_crtc1_14.crtc1_frc_hact;
	return u32FRC_hact;
}
unsigned int HAL_CRTC_GetFRCHact(VOID)
{
	unsigned int u32_FRC_Hact = 0;

	if(RUN_MERLIN8P()){
		u32_FRC_Hact = HAL_CRTC_GetFRCHact_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_FRC_Hact = HAL_CRTC_GetFRCHact_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_FRC_Hact;
}

unsigned int g_crtc1_frc_vtrig_dly=0;
void HAL_CRTC_set_crtc1_frc_vtrig_dly_initial(int value){
	g_crtc1_frc_vtrig_dly=value;
	rtd_pr_memc_emerg("[%s][g_crtc1_frc_vtrig_dly=%d]\n",__FUNCTION__,g_crtc1_frc_vtrig_dly);
}
unsigned int HAL_CRTC_get_crtc1_frc_vtrig_dly_initial(void){
	rtd_pr_memc_emerg("[%s][g_crtc1_frc_vtrig_dly=%d]\n",__FUNCTION__,g_crtc1_frc_vtrig_dly);
	return g_crtc1_frc_vtrig_dly;
}

unsigned int g_crtc1_mc_vtrig_dly_long=0;
void HAL_CRTC_set_crtc1_mc_vtrig_dly_long_initial(int value){
	g_crtc1_mc_vtrig_dly_long=value;
	rtd_pr_memc_emerg("[%s][g_crtc1_mc_vtrig_dly_long=%d]\n",__FUNCTION__,g_crtc1_mc_vtrig_dly_long);
}

unsigned int HAL_CRTC_get_crtc1_mc_vtrig_dly_long_initial(void){
	rtd_pr_memc_emerg("[%s][g_crtc1_mc_vtrig_dly_long=%d]\n",__FUNCTION__,g_crtc1_mc_vtrig_dly_long);
	return g_crtc1_mc_vtrig_dly_long;
}


unsigned int g_memc_den_vdly=0;
void HAL_CRTC_Set_memc_den_vdly(int value){
	g_memc_den_vdly=value;
	//rtd_pr_memc_emerg("[%s][g_memc_den_vdly=%d]\n",__FUNCTION__,g_memc_den_vdly);
}

unsigned int HAL_CRTC_Get_memc_den_vdly(void){
	MEMC_ChangeSize_CRTC_to_scaler();
	rtd_pr_memc_emerg("[%s][g_memc_den_vdly=%d]\n",__FUNCTION__,g_memc_den_vdly);
	return g_memc_den_vdly;
}

unsigned int g_memc_den_hdly=0;
void HAL_CRTC_Set_memc_den_hdly(int value){
	g_memc_den_hdly=value;
	//rtd_pr_memc_emerg("[%s][g_memc_den_hdly=%d]\n",__FUNCTION__,g_memc_den_hdly);
}

unsigned int HAL_CRTC_Get_memc_den_hdly(void){
	MEMC_ChangeSize_CRTC_to_scaler();
	rtd_pr_memc_emerg("[%s][g_memc_den_hdly=%d]\n",__FUNCTION__,g_memc_den_hdly);
	return g_memc_den_hdly;
}

unsigned int g_memc_crtc1_out_htotal=0;

VOID HAL_CRTC_Set_crtc1_out_htotal(unsigned int dly)
{
#if 1
	g_memc_crtc1_out_htotal=dly;
	//rtd_pr_memc_emerg("[%s][crtc1_out_htotal=%d]\n",__FUNCTION__,g_memc_crtc1_out_htotal);
#endif	
}

unsigned int HAL_CRTC_Get_crtc1_out_htotal(VOID)
{
	MEMC_ChangeSize_CRTC_to_scaler();
	rtd_pr_memc_emerg("[%s][crtc1_out_htotal=%d]\n",__FUNCTION__,g_memc_crtc1_out_htotal);
	return g_memc_crtc1_out_htotal;
}

