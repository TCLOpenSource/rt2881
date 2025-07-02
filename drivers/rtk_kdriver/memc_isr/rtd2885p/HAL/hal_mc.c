/**
 * @file hal_mc.c
 * @brief This file is for mc register setting
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
#include "memc_reg_def.h"
#include <tvscalercontrol/panel/panelapi.h>
#include <mach/rtk_platform.h>


//#if RTK_MEMC_Performance_tunging_from_tv001
#include "memc_isr/Platform/memc_change_size.h"
extern unsigned char Scaler_MEMC_GetPanelSizeByDisp(void);
//#endif


extern unsigned int Get_DISPLAY_PANEL_TYPE(void);

/**
 * @brief This function set kmc top in interrupt select
 * @param [in] 7 for input
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetInINTSel_RTK2885p(unsigned int u32Sel)
{
	//reg_kmc_in_int_sel
	WriteRegister(KMC_TOP_kmc_top_18_reg,0,2,u32Sel);
}
VOID HAL_KMC_TOP_SetInINTSel_RTK2885pp(unsigned int u32Sel)
{
	M8P_rthw_ippre_top_ippre1_b0_RBUS M8P_rthw_ippre_top_ippre1_b0;

	M8P_rthw_ippre_top_ippre1_b0.regValue = rtd_inl(M8P_RTHW_IPPRE_TOP_IPPRE1_B0_reg);
	M8P_rthw_ippre_top_ippre1_b0.kmc_in_int_sel = u32Sel;
	rtd_outl(M8P_RTHW_IPPRE_TOP_IPPRE1_B0_reg, M8P_rthw_ippre_top_ippre1_b0.regValue);
}
VOID HAL_KMC_TOP_SetInINTSel(unsigned int u32Sel)
{
	if(RUN_MERLIN8P()){
		HAL_KMC_TOP_SetInINTSel_RTK2885pp(u32Sel);
	}else if(RUN_MERLIN8()){
		HAL_KMC_TOP_SetInINTSel_RTK2885p(u32Sel);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set kmc top in interrupt source
 * @param [in] 1
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetInINTSource_RTK2885p(unsigned int u32Sor)
{
	//reg_kmc_int_source_sel
	WriteRegister(KMC_TOP_kmc_top_18_reg,19,19,u32Sor);
}
VOID HAL_KMC_TOP_SetInINTSource_RTK2885pp(unsigned int u32Sor)
{
	M8P_rthw_ippre_top_ippre1_b0_RBUS M8P_rthw_ippre_top_ippre1_b0;
	M8P_rthw_ippre_top_ippre1_b0.regValue = rtd_inl(M8P_RTHW_IPPRE_TOP_IPPRE1_B0_reg);
	M8P_rthw_ippre_top_ippre1_b0.kmc_int_source_sel = u32Sor;
	rtd_outl(M8P_RTHW_IPPRE_TOP_IPPRE1_B0_reg, M8P_rthw_ippre_top_ippre1_b0.regValue);
}
VOID HAL_KMC_TOP_SetInINTSource(unsigned int u32Sor)
{
	if(RUN_MERLIN8P()){
		HAL_KMC_TOP_SetInINTSource_RTK2885pp(u32Sor);
	}else if(RUN_MERLIN8()){
		HAL_KMC_TOP_SetInINTSource_RTK2885p(u32Sor);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set kmc top in interrupt horizantal number
 * @param [in] H number
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetInINTHNum_RTK2885p(unsigned int u32Hnum)
{
	//reg_kmc_in_vtotal_int
	WriteRegister(KMC_TOP_kmc_top_18_reg,7,18,u32Hnum);
}
VOID HAL_KMC_TOP_SetInINTHNum_RTK2885pp(unsigned int u32Hnum)
{
	M8P_rthw_ippre_top_ippre1_b4_RBUS M8P_rthw_ippre_top_ippre1_b4;
	M8P_rthw_ippre_top_ippre1_b4.regValue = rtd_inl(M8P_RTHW_IPPRE_TOP_IPPRE1_B4_reg);
	M8P_rthw_ippre_top_ippre1_b4.kmc_in_vtotal_int = u32Hnum;
	rtd_outl(M8P_RTHW_IPPRE_TOP_IPPRE1_B4_reg, M8P_rthw_ippre_top_ippre1_b4.regValue);
}
VOID HAL_KMC_TOP_SetInINTHNum(unsigned int u32Hnum)
{
	if(RUN_MERLIN8P()){
		HAL_KMC_TOP_SetInINTHNum_RTK2885pp(u32Hnum);
	}else if(RUN_MERLIN8()){
		HAL_KMC_TOP_SetInINTHNum_RTK2885p(u32Hnum);
	}//RUN_MERLIN8P & RUN_MERLIN8
}
#endif

/**
 * @brief This function set kmc top in interrupt enable
 * @param [in] u32IntType: 0 for H sync, 1 for V sync, 2 for Abnormal
 * @param [in] bEnable
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetInINTEnable_RTK2885p(unsigned int u32IntType, BOOL bEnable)
{
	unsigned int u32InINT_en = 0;
	ReadRegister(KMC_TOP_kmc_top_18_reg,24,27, &u32InINT_en);

	if(bEnable)
	{
		u32InINT_en = u32InINT_en | (1 << u32IntType);
	}
	else
	{
		u32InINT_en = u32InINT_en & (~(1 << u32IntType));
	}

	//reg_kmc_int_en
	WriteRegister(KMC_TOP_kmc_top_18_reg,24,27,u32InINT_en);
}
VOID HAL_KMC_TOP_SetInINTEnable_RTK2885pp(unsigned int u32IntType, BOOL bEnable)
{
	if(u32IntType == INT_TYPE_HSYNC){ // HS
		WriteRegister(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, 8, 8, bEnable);
	}
	else if(u32IntType == INT_TYPE_VSYNC){ // VS
		WriteRegister(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, 6, 6, bEnable);
	}
}
VOID HAL_KMC_TOP_SetInINTEnable(unsigned int u32IntType, BOOL bEnable)
{
	if(RUN_MERLIN8P()){
		HAL_KMC_TOP_SetInINTEnable_RTK2885pp(u32IntType, bEnable);
	}else if(RUN_MERLIN8()){
		HAL_KMC_TOP_SetInINTEnable_RTK2885p(u32IntType, bEnable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

BOOL HAL_KMC_TOP_GetInINTEnable_RTK2885p(unsigned int u32IntType)
{
       unsigned int v = 0;
       ReadRegister(KMC_TOP_kmc_top_18_reg,24,27,&v);
       return (v >> u32IntType) & 0x1;
}
BOOL HAL_KMC_TOP_GetInINTEnable_RTK2885pp(unsigned int u32IntType)
{
	unsigned int v = 0;

	if(u32IntType == INT_TYPE_HSYNC){ // HS
		ReadRegister(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, 8, 8, &v);
	}
	else if(u32IntType == INT_TYPE_VSYNC){ // VS
		ReadRegister(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, 6, 6, &v);
	}
	return v;
}
BOOL HAL_KMC_TOP_GetInINTEnable(unsigned int u32IntType)
{
	BOOL u1_InIntEnable = false;

	if(RUN_MERLIN8P()){
		u1_InIntEnable = HAL_KMC_TOP_GetInINTEnable_RTK2885pp(u32IntType);
	}else if(RUN_MERLIN8()){
		u1_InIntEnable = HAL_KMC_TOP_GetInINTEnable_RTK2885p(u32IntType);
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u1_InIntEnable;
}

/**
 * @brief This function set kmc top in interrupt clear
 * @param [in] eIntType: 0 for H sync, 1 for V sync, 2 for Abnormal
 * @param [in] bWclr
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetInINTWclr_RTK2885p(INT_TYPE eIntType, BOOL bWclr)
{
	unsigned int u32InINT_Clear = 0;

	ReadRegister(KMC_TOP_kmc_top_18_reg,20,23, &u32InINT_Clear);

	if(bWclr)
		u32InINT_Clear = u32InINT_Clear | ( 1 << (unsigned int)eIntType);
	else
		u32InINT_Clear = u32InINT_Clear & (~( 1 << (unsigned int)eIntType));

	//reg_kmc_int_en
	WriteRegister(KMC_TOP_kmc_top_18_reg,20,23, u32InINT_Clear);
}
VOID HAL_KMC_TOP_SetInINTWclr_RTK2885pp(INT_TYPE eIntType, BOOL bWclr)
{
	if(eIntType == INT_TYPE_HSYNC){ // INT_TYPE_HSYNC
		WriteRegister(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, 4, 4, bWclr);
	}
	else if(eIntType == INT_TYPE_VSYNC){ // INT_TYPE_VSYNC
		WriteRegister(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, 2, 2, bWclr);
	}
}
VOID HAL_KMC_TOP_SetInINTWclr(INT_TYPE eIntType, BOOL bWclr)
{
	if(RUN_MERLIN8P()){
		HAL_KMC_TOP_SetInINTWclr_RTK2885pp(eIntType, bWclr);
	}else if(RUN_MERLIN8()){
		HAL_KMC_TOP_SetInINTWclr_RTK2885p(eIntType, bWclr);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get kmc top in interrupt status
 * @retval unsigned int
*/
unsigned int HAL_KMC_TOP_GetInINTStatus_RTK2885p(INT_TYPE enIntType)
{
	unsigned int u32InINT_Sts = 0;
	//ReadRegister(KMC_TOP_kmc_top_F8_reg,24,27, &u32InINT_Sts);
	ReadRegister(KMC_TOP_kmc_top_F8_reg, 27, 30, &u32InINT_Sts);
	return u32InINT_Sts;
}
unsigned int HAL_KMC_TOP_GetInINTStatus_RTK2885pp(INT_TYPE enIntType)
{
	unsigned int u32InINT_Sts = 0;

	if(enIntType == INT_TYPE_HSYNC){ // HS
		ReadRegister(M8P_RTHW_POST_TOP_KPOST_TOP_BC_reg, 30, 30, &u32InINT_Sts);
	}
	else if(enIntType == INT_TYPE_VSYNC){ // VS
		ReadRegister(M8P_RTHW_POST_TOP_KPOST_TOP_BC_reg, 28, 28, &u32InINT_Sts);
	}
	return u32InINT_Sts;
}
unsigned int HAL_KMC_TOP_GetInINTStatus(INT_TYPE enIntType)
{
	unsigned int u32InINT_Sts = 0;

	if(RUN_MERLIN8P()){
		u32InINT_Sts = HAL_KMC_TOP_GetInINTStatus_RTK2885pp(enIntType);
	}else if(RUN_MERLIN8()){
		u32InINT_Sts = HAL_KMC_TOP_GetInINTStatus_RTK2885p(enIntType);
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32InINT_Sts;
}

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set kmc top pattern size
 * @param [in] TIMING_PARA_ST stInputTiming
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetPatternSize(TIMING_PARA_ST stInputTiming)
{
	//reg_top_patt_htotal
	WriteRegister(KMC_TOP_kmc_top_00_reg,0,15,stInputTiming.u32HTotal/2);
	//reg_top_patt_vtotal
	WriteRegister(KMC_TOP_kmc_top_00_reg,16,31,stInputTiming.u32VTotal/2);
	//reg_top_patt_hact
	WriteRegister(KMC_TOP_kmc_top_04_reg,0,15,stInputTiming.u32HActive/2);
	//reg_top_patt_vact
	WriteRegister(KMC_TOP_kmc_top_04_reg,16,31,stInputTiming.u32VActive/2);
}

/**
 * @brief This function set kmc knr 422 to 444 enable
 * @param [in] unsigned int bEnable
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetKnr422to444Enable(BOOL bEnable)
{
	//kmc_top__reg_knr_422to444
	WriteRegister(KMC_TOP_kmc_top_34_reg,12,12,(unsigned int)bEnable);
}

/**
 * @brief This function set kmc knr csc enable
 * @param [in] unsigned int bEnable
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetKnrCSCEnable(BOOL bEnable)
{
	//kmc_top__reg_knr_convert_on
	WriteRegister(KMC_TOP_kmc_top_58_reg,24,24,(unsigned int)bEnable);
}

/**
 * @brief This function set kmc knr csc mode
 * @param [in] unsigned int bEnable
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetKnrCSCMode(unsigned int u32Mode)
{
	//kmc_top__reg_knr_convert_mode
	WriteRegister(KMC_TOP_kmc_top_58_reg,25,28,u32Mode);
}

/**
 * @brief This function set kmc top knr trig gen mode
 * @param [in] unsigned int u32knr_trig_gen_mode
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetKnrTrigGenMode(unsigned int u32knr_trig_gen_mode)
{
	//reg_knr_row_trig_dly
	WriteRegister(KMC_TOP_kmc_top_30_reg,23,23,u32knr_trig_gen_mode);
}

/**
 * @brief This function set kmc top knr row trig delay
 * @param [in] unsigned int u32knr_row_trig_dly
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetKnrRowTrigDelay(unsigned int u32knr_row_trig_dly)
{
	//reg_knr_row_trig_dly
	WriteRegister(KMC_TOP_kmc_top_30_reg,0,11,u32knr_row_trig_dly);
}

/**
 * @brief This function set kmc top knr half v active
 * @param [in] unsigned int u32knr_half_vactive
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetKnrHalfVactive(unsigned int u32knr_half_vactive)
{
	//reg_knr_half_v_active
	WriteRegister(KMC_TOP_kmc_top_30_reg,12,22,u32knr_half_vactive);
}

/**
 * @brief This function set kmc top knr h active
 * @param [in] unsigned int u32knr_hactive
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetKnrHactive(unsigned int u32knr_hactive)
{
	//reg_knr_h_active
	WriteRegister(KMC_TOP_kmc_top_34_reg,0,11,u32knr_hactive);
}

/**
 * @brief This function set kmc top ipmc hde center value
 * @param [in] unsigned int u32ipmc_hde_center_value
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetIPMCHDECenterValue(unsigned int u32ipmc_hde_center_value)
{
	//reg_kmc_ipmc_hde_center_value
	WriteRegister(KMC_TOP_kmc_top_3C_reg,0,9,u32ipmc_hde_center_value);
}

/**
 * @brief This function set kmc top ipmc mode
 * @param [in] unsigned int u32ipmc_mode
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetIPMCMode(unsigned int u32ipmc_mode)
{
	//reg_kmc_ipmc_mode
	WriteRegister(KMC_TOP_kmc_top_3C_reg,20,23,u32ipmc_mode);
}

/**
 * @brief This function set kmc top ipmc hde active
 * @param [in] unsigned int u32ipmc_hde_active
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetIPMCHDEActive(unsigned int u32ipmc_hde_active)
{
	//reg_kmc_ipmc_hde_active
	WriteRegister(KMC_TOP_kmc_top_40_reg,0,10,u32ipmc_hde_active);
}

/**
 * @brief This function set kmc top ipmc v active
 * @param [in] unsigned int u32ipmc_v_active
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetIPMCVactive(unsigned int u32ipmc_v_active)
{
	//reg_kmc_ipmc_v_active
	WriteRegister(KMC_TOP_kmc_top_40_reg,11,22,u32ipmc_v_active);
}

/**
 * @brief This function set kmc top data mapping
 * @param [in] unsigned int u32Data_Mapping
 * @retval VOID
*/
VOID HAL_KMC_TOP_SetDataMapping(unsigned int u32Data_Mapping)
{
	//reg_knr_convert_map
	WriteRegister(KMC_TOP_kmc_top_58_reg,29,31,(unsigned int)u32Data_Mapping);
}

/**
 * @brief This function get kmc top data mapping
 * @param [in] VOID
 * @retval data mapping
*/
unsigned int HAL_KMC_TOP_GetDataMapping(VOID)
{
	unsigned int u32Data_Mapping = 0;
	//reg_knr_convert_map
	ReadRegister(KMC_TOP_kmc_top_58_reg,29,31,&u32Data_Mapping);
	return u32Data_Mapping;
}

/**
 * @brief This function set lbmc fhd mode
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetLBMCFHDMode(BOOL bEnable)
{
	WriteRegister(LBMC_LBMC_20_reg,30,30, (unsigned int)bEnable);
}

/**
 * @brief This function set lbmc H/V active
 * @param [in] unsigned int u32Hactive
 * @param [in] unsigned int u32Vactive
 * @retval VOID
*/
VOID HAL_MC_SetLBMCActive(unsigned int u32Hactive, unsigned int u32Vactive)
{
	//reg_lbmc_h_act[11:0]
	WriteRegister(LBMC_LBMC_20_reg,0,12,u32Hactive);
	//reg_lbmc_v_act[11:0]
	WriteRegister(LBMC_LBMC_20_reg,16,28,u32Vactive);
}

/**
 * @brief This function set lbmc 3d mode
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetLBMC3DMode(BOOL bEnable)
{
	//reg_lbmc_3dmode
	WriteRegister(LBMC_LBMC_24_reg,3,3,(unsigned int)bEnable);
}

/**
 * @brief This function set lbmc pr mode
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetLBMCPRMode(BOOL bEnable)
{
	//lbmc_pr_mod
	WriteRegister(LBMC_LBMC_20_reg,13,13,(unsigned int)bEnable);
}
#endif//REMOVE_REDUNDANT_API

/**
 * @brief This function set lbmc pc mode
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetLBMCPCMode_RTK2885p(BOOL bEnable)
{
	//reg_lbmcpc_mode
	WriteRegister(LBMC_LBMC_24_reg,4,4,(unsigned int)bEnable);
}
VOID HAL_MC_SetLBMCPCMode_RTK2885pp(BOOL bEnable)
{
	M8P_rtmc_lbmc_lbmc_ctrl_RBUS M8P_rtmc_lbmc_lbmc_ctrl;
	M8P_rtmc_lbmc_lbmc_ctrl.regValue = rtd_inl(M8P_RTMC_LBMC_LBMC_CTRL_reg);
	M8P_rtmc_lbmc_lbmc_ctrl.lbmcpc_mode = bEnable;
	rtd_outl(M8P_RTMC_LBMC_LBMC_CTRL_reg, M8P_rtmc_lbmc_lbmc_ctrl.regValue);
}
VOID HAL_MC_SetLBMCPCMode(BOOL bEnable)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetLBMCPCMode_RTK2885pp(bEnable);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetLBMCPCMode_RTK2885p(bEnable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if VFLIP_REALTE
/**
 * @brief This function set lbmc vflip enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetLBMCVflipEnable(BOOL bEnable)
{
	//reg_lbmc_vflip
	WriteRegister(LBMC_LBMC_24_reg,6,6,(unsigned int)bEnable);
}
#endif //VFLIP_REALTE

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set mc h half size
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetMCHhalfsize(BOOL bEnable)
{
	//reg_mc_h_half_size
	WriteRegister(MC_MC_30_reg,0,0,(unsigned int)bEnable);
}

/**
 * @brief This function set mc v half size
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetMCVhalfsize(BOOL bEnable)
{
	//reg_mc_v_half_size
	WriteRegister(MC_MC_30_reg,1,1,(unsigned int) bEnable);
}

/**
 * @brief This function set mc H/V active
 * @param [in] unsigned int u32Hactive
 * @param [in] unsigned int u32Vactive
 * @retval VOID
*/
VOID HAL_MC_SetMCActive(unsigned int u32Hactive, unsigned int u32Vactive)
{
	//reg_mc_h_act[11:0]
	WriteRegister(MC_MC_34_reg,0,11,u32Hactive);
	//reg_mc_v_act[11:0]
	WriteRegister(MC_MC_34_reg,16,27,u32Vactive);
}

/**
 * @brief This function set mc block size
 * @param [in] unsigned int u32Block_Size
 * @retval VOID
*/
VOID HAL_MC_SetMCBlockSize(unsigned int u32Block_Size)
{
	//reg_mc_blk_size[3:2]
	WriteRegister(MC_MC_38_reg,0,3,u32Block_Size);
}

/**
 * @brief This function get mc block size
 * @param [in] VOID
 * @retval unsigned int u32Block_Size
*/
unsigned int HAL_MC_GetMCBlockSize(VOID)
{
	unsigned int u32Block_Size = 0;
	//reg_mc_blk_size[3:2]
	ReadRegister(MC_MC_38_reg,0,3,&u32Block_Size);
	return u32Block_Size;
}

/**
 * @brief This function set mc mv scaler
 * @param [in] unsigned int u32MV_Scaler
 * @retval VOID
*/
VOID HAL_MC_SetMCMVScaler(unsigned int u32MV_Scaler)
{
	//reg_mc_mv_scale[1:0]
	WriteRegister(MC_MC_38_reg,4,7,u32MV_Scaler);
}

/**
 * @brief This function set mc block number
 * @param [in] unsigned int u32Block_Num
 * @retval VOID
*/
VOID HAL_MC_SetMCBlockNum(unsigned int u32Block_Num)
{
	//reg_mc_blk_num[9:0]
	WriteRegister(MC_MC_3C_reg,0,9,u32Block_Num);
#if 1
	WriteRegister(MC_MC_2C_reg,3,11,u32Block_Num);	
	WriteRegister(MC_MC_24_reg,9,17,u32Block_Num);		
#endif
}

/**
 * @brief This function set mc row number
 * @param [in] unsigned int u32Block_Num
 * @retval VOID
*/
VOID HAL_MC_SetMCRowNum(unsigned int u32Row_Num)
{
	//reg_mc_row_num[9:0]
	WriteRegister(MC_MC_3C_reg,16,25,u32Row_Num);
#if 1
	WriteRegister(MC_MC_2C_reg,12,20,u32Row_Num);
	WriteRegister(MC_MC_24_reg,18,26,u32Row_Num);		
#endif
}

#if 1
/**
 * @brief This function set mc mv pattern block size
 * @param [in] unsigned int u32Val
 * @retval VOID
*/
VOID HAL_MC_SetMCMVPatBlkSize(unsigned int u32Val)
{
	WriteRegister(MC_MC_28_reg,0,0,u32Val);
}

/**
 * @brief This function set mc mv pattern Row size
 * @param [in] unsigned int u32Val
 * @retval VOID
*/
VOID HAL_MC_SetMCMVPatRowSize(unsigned int u32Val)
{
	WriteRegister(MC_MC_28_reg,1,1,u32Val);
}

/**
 * @brief This function set mc dehalo half v timing
 * @param [in] unsigned int u32Val
 * @retval VOID
*/
VOID HAL_MC_SetMCDhHalfVTiming(unsigned int u32Val)
{
	WriteRegister(MC_MC_28_reg,29,29,u32Val);
}

/**
 * @brief This function set mc dehalo Row size
 * @param [in] unsigned int u32Val
 * @retval VOID
*/
VOID HAL_MC_SetMCDhRowSize(unsigned int u32Val)
{
	WriteRegister(MC_MC_2C_reg,0,1,u32Val);
}

/**
 * @brief This function set mc mv read interval
 * @param [in] unsigned int u32Val
 * @retval VOID
*/
VOID HAL_MC_SetMCMVRDIntval(unsigned int u32Val)
{
	WriteRegister(MC_MC_2C_reg,2,2,u32Val);
}
#endif
#endif//REMOVE_REDUNDANT_API

/**
 * @brief This function set mc blend mode
 * @param [in] unsigned int u32Mode
 * @retval VOID
*/
VOID HAL_MC_SetMCBlendMode_RTK2885p(unsigned int u32Mode)
{
#if (IC_K6LP || IC_K8LP || IC_K24) // merlin4_blend_mode
	if(u32Mode == 1){
		WriteRegister(LBMC_LBMC_24_reg, 4, 4, 0x1); //0xb8099924[4]
		WriteRegister(LBMC_LBMC_24_reg, 13, 13, 0x0); //0xb8099924[13] //IZ mode
		WriteRegister(LBMC_LBMC_24_reg, 10, 10, 0); //0xb8099924[10]
		WriteRegister(LBMC_LBMC_EC_reg, 1, 1, 1); // lbmc_db_apply
		#if (IC_K24)
//		WriteRegister(MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg, 3, 3, 1); // force_dma_raw_pc_mode_en
		#endif
		rtd_pr_memc_notice("[HAL_MC_SetMCBlendMode][Repeat mode][IZ][9924,%x,][90k=%x]\n\r", rtd_inl(LBMC_LBMC_24_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
	}
	else if(u32Mode == 2){
		WriteRegister(LBMC_LBMC_24_reg, 4, 4, 0x1); //0xb8099924[4]
		WriteRegister(LBMC_LBMC_24_reg, 13, 13, 0x1); //0xb8099924[13] //PZ mode
		WriteRegister(LBMC_LBMC_24_reg, 10, 10, 0); //0xb8099924[10]
		WriteRegister(LBMC_LBMC_EC_reg, 1, 1, 1); // lbmc_db_apply
		#if (IC_K24)
//		WriteRegister(MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg, 3, 3, 1); // force_dma_raw_pc_mode_en
		#endif
		rtd_pr_memc_notice("[HAL_MC_SetMCBlendMode][Repeat mode][PZ][9924,%x,][90k=%x]\n\r", rtd_inl(LBMC_LBMC_24_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
	}
	else{
		WriteRegister(LBMC_LBMC_24_reg, 4, 4, 0x0); //0xb8099924[4]
		WriteRegister(LBMC_LBMC_24_reg, 13, 13, 0x0); //0xb8099924[13] //reset
		WriteRegister(LBMC_LBMC_24_reg, 10, 10, 1); //0xb8099924[10]
		WriteRegister(LBMC_LBMC_EC_reg, 1, 1, 1); // lbmc_db_apply
		#if (IC_K24)
//		WriteRegister(MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg, 3, 3, 0); // force_dma_raw_pc_mode_en
		#endif
		rtd_pr_memc_notice("[HAL_MC_SetMCBlendMode][Blending mode][9924,%x,][90k=%x]\n\r", rtd_inl(LBMC_LBMC_24_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
	}
#else
	WriteRegister(MC_MC_B0_reg,0,2,u32Mode);
#endif
}
VOID HAL_MC_SetMCBlendMode_RTK2885pp(unsigned int u32Mode)
{
	M8P_rtmc_lbmc_lbmc_ctrl_RBUS M8P_rtmc_lbmc_lbmc_ctrl;
	M8P_rthw_lbmc_lbmc_db_RBUS M8P_rthw_lbmc_lbmc_db;
	M8P_rtmc_lbmc_lbmc_ctrl.regValue = rtd_inl(M8P_RTMC_LBMC_LBMC_CTRL_reg);
	M8P_rthw_lbmc_lbmc_db.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_DB_reg);
	if(u32Mode == 1){
		M8P_rtmc_lbmc_lbmc_ctrl.lbmcpc_mode = 1;
		M8P_rtmc_lbmc_lbmc_ctrl.lbmcpc_mode_sel_p = 0;
		M8P_rtmc_lbmc_lbmc_ctrl.force_iz_when_mc_phase_eq0 = 0;
		M8P_rthw_lbmc_lbmc_db.lbmc_db_rd_sel = 1;
		rtd_pr_memc_notice("[HAL_MC_SetMCBlendMode][Repeat mode][IZ][a618,%x,][90k=%x]\n\r", rtd_inl(M8P_RTMC_LBMC_LBMC_CTRL_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
	}
	else if(u32Mode == 2){
		M8P_rtmc_lbmc_lbmc_ctrl.lbmcpc_mode = 1;
		M8P_rtmc_lbmc_lbmc_ctrl.lbmcpc_mode_sel_p = 1;
		M8P_rtmc_lbmc_lbmc_ctrl.force_iz_when_mc_phase_eq0 = 0;
		M8P_rthw_lbmc_lbmc_db.lbmc_db_rd_sel = 1;
		rtd_pr_memc_notice("[HAL_MC_SetMCBlendMode][Repeat mode][PZ][a618,%x,][90k=%x]\n\r", rtd_inl(M8P_RTMC_LBMC_LBMC_CTRL_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
	}
	else{
		M8P_rtmc_lbmc_lbmc_ctrl.lbmcpc_mode = 0;
		M8P_rtmc_lbmc_lbmc_ctrl.lbmcpc_mode_sel_p = 0;
		M8P_rtmc_lbmc_lbmc_ctrl.force_iz_when_mc_phase_eq0 = 1;
		M8P_rthw_lbmc_lbmc_db.lbmc_db_rd_sel = 1;
		rtd_pr_memc_notice("[HAL_MC_SetMCBlendMode][Blending mode][a618,%x,][90k=%x]\n\r", rtd_inl(M8P_RTMC_LBMC_LBMC_CTRL_reg), rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
	}
	rtd_outl(M8P_RTMC_LBMC_LBMC_CTRL_reg, M8P_rtmc_lbmc_lbmc_ctrl.regValue);
	rtd_outl(M8P_RTHW_LBMC_LBMC_DB_reg, M8P_rthw_lbmc_lbmc_db.regValue);
}
VOID HAL_MC_SetMCBlendMode(unsigned int u32Mode)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetMCBlendMode_RTK2885pp(u32Mode);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetMCBlendMode_RTK2885p(u32Mode);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set mc scene change enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetMCSceneChangeEnable_RTK2885p(BOOL bEnable)
{
	//reg_mc_scene_change_fb
	//rtd_pr_memc_notice( "HAL_MC_SetMCSceneChangeEnable=%d\n",bEnable);

#if 1
	WriteRegister(MC_MC_30_reg,8,8,(unsigned int)bEnable);
#else
	WriteRegister(MC_MC_30_reg,10,10,(unsigned int)bEnable);
#endif
}
VOID HAL_MC_SetMCSceneChangeEnable_RTK2885pp(BOOL bEnable)
{
	M8P_rtmc_rtmc_00_RBUS M8P_rtmc_rtmc_00;
	M8P_rtmc_rtmc_00.regValue = rtd_inl(M8P_RTMC_RTMC_00_reg);
	M8P_rtmc_rtmc_00.mc_scene_change_mode = bEnable;
	rtd_outl(M8P_RTMC_RTMC_00_reg, M8P_rtmc_rtmc_00.regValue);
}
VOID HAL_MC_SetMCSceneChangeEnable(BOOL bEnable)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetMCSceneChangeEnable_RTK2885pp(bEnable);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetMCSceneChangeEnable_RTK2885p(bEnable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set mc scene change enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetMCSceneChange_fb_RTK2885p(BOOL bEnable)
{
	//reg_mc_scene_change_fb
	//rtd_pr_memc_notice( "HAL_MC_SetMCSceneChange_fb=%d\n",bEnable);
#if 1
	WriteRegister(MC_MC_30_reg,10,10,(unsigned int)bEnable);
#else
	WriteRegister(MC_MC_30_reg,10,10,(unsigned int)bEnable);
#endif
}
VOID HAL_MC_SetMCSceneChange_fb_RTK2885pp(BOOL bEnable)
{
	M8P_rtmc_rtmc_74_RBUS M8P_rtmc_rtmc_74;
	M8P_rtmc_rtmc_74.regValue = rtd_inl(M8P_RTMC_RTMC_74_reg);
	M8P_rtmc_rtmc_74.mc_bld_sc_en = bEnable;
	M8P_rtmc_rtmc_74.mc_bld_sc_z_type = 1;//pz
	rtd_outl(M8P_RTMC_RTMC_74_reg, M8P_rtmc_rtmc_74.regValue);
}
VOID HAL_MC_SetMCSceneChange_fb(BOOL bEnable)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetMCSceneChange_fb_RTK2885pp(bEnable);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetMCSceneChange_fb_RTK2885p(bEnable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set Demo Window multiview
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_Set_DemoWindow_Multiview_RTK2885pp(BOOL bEnable)
{
	M8P_rtmc_rtmc_20_RBUS M8P_rtmc_rtmc_20;
	M8P_rtmc_rtmc_20.regValue = rtd_inl(M8P_RTMC_RTMC_20_reg);
	M8P_rtmc_rtmc_20.mc_dw_multiview = bEnable;
	rtd_outl(M8P_RTMC_RTMC_20_reg, M8P_rtmc_rtmc_20.regValue);
}
VOID HAL_MC_Set_DemoWindow_Multiview(BOOL bEnable)
{
	if(RUN_MERLIN8P()){
		HAL_MC_Set_DemoWindow_Multiview_RTK2885pp(bEnable);
	}else if(RUN_MERLIN8()){
		//no hw
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set Demo Window data type
 * @param [in] u32Type: 0: iz; 1: pz; 2: avg; 3: phase blend;
 * @retval VOID
*/
VOID HAL_MC_Set_DemoWindow_Type_RTK2885pp(unsigned int u32Type)
{
	M8P_rtmc_rtmc_20_RBUS M8P_rtmc_rtmc_20;
	M8P_rtmc_rtmc_20.regValue = rtd_inl(M8P_RTMC_RTMC_20_reg);
	M8P_rtmc_rtmc_20.mc_dw_datamode = u32Type;
	rtd_outl(M8P_RTMC_RTMC_20_reg, M8P_rtmc_rtmc_20.regValue);
}
VOID HAL_MC_Set_DemoWindow_Type(unsigned int u32Type)
{
	if(RUN_MERLIN8P()){
		HAL_MC_Set_DemoWindow_Type_RTK2885pp(u32Type);
	}else if(RUN_MERLIN8()){
		//no hw
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set MC PT Zero type1
 * @param [in] u32Type: 0: iz; 1: pz; 2: avg; 3: phase blend; 4: I/P by phase; 5: med3
 * @retval VOID
*/
VOID HAL_MC_SetPTZeroType1_RTK2885p(unsigned int u32Type)
{
	//reg_mc_pt_z_type1
	WriteRegister(MC_MC_A4_reg,8,10,u32Type);
}
VOID HAL_MC_SetPTZeroType1(unsigned int u32Type)
{
	if(RUN_MERLIN8P()){
		//no hw
	}else if(RUN_MERLIN8()){
		HAL_MC_SetPTZeroType1_RTK2885p(u32Type);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if REMOVE_DUMMY_API
/**
 * @brief This function set MC PT Zero type2
 * @param [in] u32Type: 0: iz; 1: pz; 2: avg; 3: phase blend; 4: I/P by phase; 5: med3
 * @retval VOID
*/
VOID HAL_MC_SetPTZeroType2(unsigned int u32Type)
{
	//reg_mc_pt_z_type2
	WriteRegister(MC_MC_A4_reg,12,14,u32Type);
}
#endif //REMOVE_DUMMY_API

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set mc logo mode
 * @param [in] unsigned int u32LogoMode
 * @retval VOID
*/
VOID HAL_MC_SetLogoMode(unsigned int u32LogoMode)
{
	//reg_mc_logo_en
	WriteRegister(MC2_MC2_20_reg,0,1,u32LogoMode);
}

/**
 * @brief This function set mc block logo h size
 * @param [in] unsigned int u32Hsize
 * @retval VOID
*/
VOID HAL_MC_SetBlockLogoHSize(unsigned int u32Hsize)
{
	//reg_mc_blk_logo_h_size
	WriteRegister(MC2_MC2_20_reg,2,2,u32Hsize);
}

/**
 * @brief This function set mc pixel logo h size
 * @param [in] unsigned int u32Hsize
 * @retval VOID
*/
VOID HAL_MC_SetPixelLogoHSize(unsigned int u32Hsize)
{
	//reg_mc_pxl_logo_h_size
	WriteRegister(MC2_MC2_20_reg,4,4,u32Hsize);
}

/**
 * @brief This function set mc block logo v size
 * @param [in] unsigned int u32Vsize
 * @retval VOID
*/
VOID HAL_MC_SetBlockLogoVSize(unsigned int u32Vsize)
{
	//reg_mc_blk_logo_v_size
	WriteRegister(MC2_MC2_20_reg,3,3,u32Vsize);
}

/**
 * @brief This function set mc pixel logo v size
 * @param [in] unsigned int u32Vsize
 * @retval VOID
*/
VOID HAL_MC_SetPixelLogoVSize(unsigned int u32Vsize)
{
	//reg_mc_pxl_logo_v_size
	WriteRegister(MC2_MC2_20_reg,5,5,u32Vsize);
}

/**
 * @brief This function set kphase 3d fs enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetKphase3DFSEnable(BOOL bEnable)
{
	//reg_kphase_in_3d_fs_enable
	WriteRegister(KPHASE_kphase_00_reg,7,7,(unsigned int)bEnable);
	//reg_kphase_in_lr_ctl_for_index
	WriteRegister(KPHASE_kphase_10_reg,26,26,(unsigned int)bEnable);
}

/**
 * @brief This function set kphase 3d enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetKphase3DEnable(BOOL bEnable)
{
	//reg_kphase_out_3d_enable
	WriteRegister(KPHASE_kphase_0C_reg,31,31,(unsigned int)bEnable);
}

/**
 * @brief This function set 3d mode
 * @param [in] unsigned int u32Mode
 * @retval VOID
*/
VOID HAL_MC_SetKphase3DMode(unsigned int u32Mode)
{
	//reg_kphase_3d_mode
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KME_TOP__kphase_3d_mode_ADDR no mat,FRC_TOP__KME_TOP__kphase_3d_mode_BITSTART no mat,FRC_TOP__KME_TOP__kphase_3d_mode_BITEND no mat,u32Mode);
}
#endif//REMOVE_REDUNDANT_API

/**
 * @brief This function set me1 index bsize
 * @param [in] unsigned int u32Bsize
 * @retval VOID
*/
VOID HAL_MC_SetKphaseME1IndexBsize_RTK2885p(unsigned int u32Bsize)
{
	WriteRegister(KPHASE_kphase_10_reg,8,11,u32Bsize);
}
VOID HAL_MC_SetKphaseME1IndexBsize_RTK2885pp(unsigned int u32Bsize)
{
	M8P_rthw_phase_kphase_10_RBUS M8P_rthw_phase_kphase_10;
	M8P_rthw_phase_kphase_10.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_10_reg);
	M8P_rthw_phase_kphase_10.kphase_in_me_index_bsize = u32Bsize;
	rtd_outl(M8P_RTHW_PHASE_kphase_10_reg, M8P_rthw_phase_kphase_10.regValue);
}
VOID HAL_MC_SetKphaseME1IndexBsize(unsigned int u32Bsize)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetKphaseME1IndexBsize_RTK2885pp(u32Bsize);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetKphaseME1IndexBsize_RTK2885p(u32Bsize);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set me2 index bsize
 * @param [in] unsigned int u32Bsize
 * @retval VOID
*/
VOID HAL_MC_SetKphaseME2IndexBsize_RTK2885p(unsigned int u32Bsize)
{
	WriteRegister(KPHASE_kphase_78_reg,11,14,u32Bsize);
}
VOID HAL_MC_SetKphaseME2IndexBsize_RTK2885pp(unsigned int u32Bsize)
{
	M8P_rthw_phase_kphase_78_RBUS M8P_rthw_phase_kphase_78;
	M8P_rthw_phase_kphase_78.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_78_reg);
	M8P_rthw_phase_kphase_78.kphase_in_me2_index_bsize = u32Bsize;
	rtd_outl(M8P_RTHW_PHASE_kphase_78_reg, M8P_rthw_phase_kphase_78.regValue);
}
VOID HAL_MC_SetKphaseME2IndexBsize(unsigned int u32Bsize)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetKphaseME2IndexBsize_RTK2885pp(u32Bsize);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetKphaseME2IndexBsize_RTK2885p(u32Bsize);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get the mc Bsize
 * @param [in] VOID
 * @retval buffer_size
*/
unsigned int HAL_MC_GetKphaseMCIndexBsize_RTK2885p(VOID)
{
	unsigned int buffer_size = 0;

	ReadRegister(KPHASE_kphase_10_reg, 12, 15, &buffer_size);
	return buffer_size;
}
unsigned int HAL_MC_GetKphaseMCIndexBsize_RTK2885pp(VOID)
{
	M8P_rthw_phase_kphase_10_RBUS M8P_rthw_phase_kphase_10;
	unsigned int buffer_size = 0;

	M8P_rthw_phase_kphase_10.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_10_reg);
	buffer_size = M8P_rthw_phase_kphase_10.kphase_in_mc_index_bsize;
	return buffer_size;
}
unsigned int HAL_MC_GetKphaseMCIndexBsize(VOID)
{
	unsigned int u32_Bsize = 0;

	if(RUN_MERLIN8P()){
		u32_Bsize = HAL_MC_GetKphaseMCIndexBsize_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_Bsize = HAL_MC_GetKphaseMCIndexBsize_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_Bsize;
}

/**
 * @brief This function set mc index bsize
 * @param [in] unsigned int u32Bsize
 * @retval VOID
*/
VOID HAL_MC_SetKphaseMCIndexBsize_RTK2885p(unsigned int u32Bsize)
{
	//reg_kphase_in_mc_index_bsize
	WriteRegister(KPHASE_kphase_10_reg,12,15,u32Bsize);
}
VOID HAL_MC_SetKphaseMCIndexBsize_RTK2885pp(unsigned int u32Bsize)
{
	M8P_rthw_phase_kphase_10_RBUS M8P_rthw_phase_kphase_10;
	M8P_rthw_phase_kphase_10.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_10_reg);
	M8P_rthw_phase_kphase_10.kphase_in_mc_index_bsize = u32Bsize;
	rtd_outl(M8P_RTHW_PHASE_kphase_10_reg, M8P_rthw_phase_kphase_10.regValue);
}
VOID HAL_MC_SetKphaseMCIndexBsize(unsigned int u32Bsize)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetKphaseMCIndexBsize_RTK2885pp(u32Bsize);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetKphaseMCIndexBsize_RTK2885p(u32Bsize);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get the kphase mc write index
 * @param [in] VOID
 * @retval mc_write
*/
unsigned int HAL_MC_GetKphaseMCWriteIndex_RTK2885p(VOID)
{
	unsigned int mc_write = 0;

	ReadRegister(KPHASE_kphase_90_reg, 0, 2, &mc_write);
	return mc_write;
}
unsigned int HAL_MC_GetKphaseMCWriteIndex_RTK2885pp(VOID)
{
	M8P_rthw_phase_phase_a4_RBUS M8P_rthw_phase_phase_a4;
	unsigned int mc_write = 0;

	M8P_rthw_phase_phase_a4.regValue = rtd_inl(M8P_RTHW_PHASE_phase_A4_reg);
	mc_write = M8P_rthw_phase_phase_a4.regr_nowdma_in_mc_index;
	return mc_write;
}
unsigned int HAL_MC_GetKphaseMCWriteIndex(VOID)
{
	unsigned int u32_index = 0;

	if(RUN_MERLIN8P()){
		u32_index = HAL_MC_GetKphaseMCWriteIndex_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_index = HAL_MC_GetKphaseMCWriteIndex_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_index;
}

/**
 * @brief This function get the kphase mc read i index
 * @param [in] VOID
 * @retval mc_read
*/
unsigned int HAL_MC_GetKphaseMCRead_i_Index_RTK2885p(VOID)
{
	unsigned int mc_read = 0;

	ReadRegister(KPHASE_kphase_90_reg, 4, 6, &mc_read);
	return mc_read;
}
unsigned int HAL_MC_GetKphaseMCRead_i_Index_RTK2885pp(VOID)
{
	M8P_rthw_phase_phase_a8_RBUS M8P_rthw_phase_phase_a8;
	unsigned int mc_read = 0;

	M8P_rthw_phase_phase_a8.regValue = rtd_inl(M8P_RTHW_PHASE_phase_A8_reg);
	mc_read = M8P_rthw_phase_phase_a8.regr_nowdma_mc_i_idx;
	return mc_read;
}
unsigned int HAL_MC_GetKphaseMCRead_i_Index(VOID)
{
	unsigned int u32_index = 0;

	if(RUN_MERLIN8P()){
		u32_index = HAL_MC_GetKphaseMCRead_i_Index_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_index = HAL_MC_GetKphaseMCRead_i_Index_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_index;
}

/**
 * @brief This function get the kphase mc read p index
 * @param [in] VOID
 * @retval mc_read
*/
unsigned int HAL_MC_GetKphaseMCRead_p_Index_RTK2885p(VOID)
{
	unsigned int mc_read = 0;

	ReadRegister(KPHASE_kphase_90_reg, 8, 10, &mc_read);
	return mc_read;
}
unsigned int HAL_MC_GetKphaseMCRead_p_Index_RTK2885pp(VOID)
{
	M8P_rthw_phase_phase_a8_RBUS M8P_rthw_phase_phase_a8;
	unsigned int mc_read = 0;

	M8P_rthw_phase_phase_a8.regValue = rtd_inl(M8P_RTHW_PHASE_phase_A8_reg);
	mc_read = M8P_rthw_phase_phase_a8.regr_nowdma_mc_p_idx;
	return mc_read;
}
unsigned int HAL_MC_GetKphaseMCRead_p_Index(VOID)
{
	unsigned int u32_index = 0;

	if(RUN_MERLIN8P()){
		u32_index = HAL_MC_GetKphaseMCRead_p_Index_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_index = HAL_MC_GetKphaseMCRead_p_Index_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_index;
}

/**
 * @brief This function get the kphase output phase
 * @param [in] VOID
 * @retval outPhase
*/
unsigned int HAL_MC_GetKphaseOutPhase_RTK2885p(VOID)
{
	unsigned int outPhase;
	ReadRegister(KPHASE_kphase_9C_reg,0,5, &outPhase);
	return outPhase;
}
unsigned int HAL_MC_GetKphaseOutPhase_RTK2885pp(VOID)
{
	M8P_rthw_phase_phase_a8_RBUS M8P_rthw_phase_phase_a8;
	unsigned int outPhase;
	
	M8P_rthw_phase_phase_a8.regValue = rtd_inl(M8P_RTHW_PHASE_phase_A8_reg);
	outPhase = M8P_rthw_phase_phase_a8.regr_nowdma_out_phase;
	return outPhase;
}
unsigned int HAL_MC_GetKphaseOutPhase(VOID)
{
	unsigned int u32_Phase = 0;

	if(RUN_MERLIN8P()){
		u32_Phase = HAL_MC_GetKphaseOutPhase_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_Phase = HAL_MC_GetKphaseOutPhase_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_Phase;
}

/**
 * @brief This function get the kphase input phase
 * @param [in] VOID
 * @retval outPhase
*/
unsigned int HAL_MC_GetKphaseInPhase_RTK2885p(VOID)
{
	unsigned int in_phase;
	ReadRegister(KPHASE_kphase_9C_reg, 12, 15, &in_phase);
	return in_phase;
}
unsigned int HAL_MC_GetKphaseInPhase_RTK2885pp(VOID)
{
	M8P_rthw_phase_phase_a4_RBUS M8P_rthw_phase_phase_a4;
	unsigned int in_phase;
	
	M8P_rthw_phase_phase_a4.regValue = rtd_inl(M8P_RTHW_PHASE_phase_A4_reg);
	in_phase = M8P_rthw_phase_phase_a4.regr_nowdma_in_phase;
	return in_phase;
}
unsigned int HAL_MC_GetKphaseInPhase(VOID)
{
	unsigned int in_phase = 0;

	if(RUN_MERLIN8P()){
		in_phase = HAL_MC_GetKphaseInPhase_RTK2885pp();
	}else if(RUN_MERLIN8()){
		in_phase = HAL_MC_GetKphaseInPhase_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return in_phase;
}

/**
 * @brief This function get the kphase sw film phase
 * @param [in] VOID
 * @retval swFilmPhase
*/
unsigned int HAL_MC_GetKphaseSWFilmPhase_RTK2885p(VOID)
{
	unsigned int swFilmPhase;
	ReadRegister(KPHASE_kphase_0C_reg,0,5, &swFilmPhase);
	return swFilmPhase;
}
unsigned int HAL_MC_GetKphaseSWFilmPhase_RTK2885pp(VOID)
{
	M8P_rthw_phase_kphase_0c_RBUS M8P_rthw_phase_kphase_0c;
	unsigned int swFilmPhase;
	
	M8P_rthw_phase_kphase_0c.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_0C_reg);
	swFilmPhase = M8P_rthw_phase_kphase_0c.kphase_sw_film_phase;
	return swFilmPhase;
}
unsigned int HAL_MC_GetKphaseSWFilmPhase(VOID)
{
	unsigned int u32_Phase = 0;

	if(RUN_MERLIN8P()){
		u32_Phase = HAL_MC_GetKphaseSWFilmPhase_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_Phase = HAL_MC_GetKphaseSWFilmPhase_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_Phase;
}

/**
 * @brief This function set the kphase sw film phase
 * @param [in] unsigned int swFilmPhase
 * @retval VOID
*/
VOID HAL_MC_SetKphaseSWFilmPhase_RTK2885p(unsigned int swFilmPhase)
{
	WriteRegister(KPHASE_kphase_0C_reg,0,5, swFilmPhase);
}
VOID HAL_MC_SetKphaseSWFilmPhase_RTK2885pp(unsigned int swFilmPhase)
{
	M8P_rthw_phase_kphase_0c_RBUS M8P_rthw_phase_kphase_0c;
	M8P_rthw_phase_kphase_0c.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_0C_reg);
	M8P_rthw_phase_kphase_0c.kphase_sw_film_phase = swFilmPhase;
	rtd_outl(M8P_RTHW_PHASE_kphase_0C_reg, M8P_rthw_phase_kphase_0c.regValue);
}
VOID HAL_MC_SetKphaseSWFilmPhase(unsigned int swFilmPhase)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetKphaseSWFilmPhase_RTK2885pp(swFilmPhase);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetKphaseSWFilmPhase_RTK2885p(swFilmPhase);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get the kphase film phase type
 * @param [in] VOID
 * @retval FilmPhaseType
*/
unsigned int HAL_MC_GetKphaseFilmPhaseType_RTK2885p(void)
{
	unsigned int FilmPhaseType = 0;
	ReadRegister(KPHASE_kphase_0C_reg,18,23, &FilmPhaseType);
	return FilmPhaseType;
}
unsigned int HAL_MC_GetKphaseFilmPhaseType_RTK2885pp(void)
{
	unsigned int FilmPhaseType = 0;
	M8P_rthw_phase_kphase_0c_RBUS M8P_rthw_phase_kphase_0c;
	M8P_rthw_phase_kphase_0c.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_0C_reg);
	FilmPhaseType = M8P_rthw_phase_kphase_0c.kphase_film_phase_type;
	return FilmPhaseType;
}
unsigned int HAL_MC_GetKphaseFilmPhaseType(void)
{
	unsigned int u32_Type = 0;

	if(RUN_MERLIN8P()){
		u32_Type = HAL_MC_GetKphaseFilmPhaseType_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_Type = HAL_MC_GetKphaseFilmPhaseType_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_Type;
}

/**
 * @brief This function set the kphase film phase type
 * @param [in] unsigned int FilmPhaseType
 * @retval VOID
*/
VOID HAL_MC_SetKphaseFilmPhaseType_RTK2885p(unsigned int FilmPhaseType)
{
	WriteRegister(KPHASE_kphase_0C_reg,18,23, FilmPhaseType);
}
VOID HAL_MC_SetKphaseFilmPhaseType_RTK2885pp(unsigned int FilmPhaseType)
{
	M8P_rthw_phase_kphase_0c_RBUS M8P_rthw_phase_kphase_0c;
	M8P_rthw_phase_kphase_0c.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_0C_reg);
	M8P_rthw_phase_kphase_0c.kphase_film_phase_type = FilmPhaseType;
	rtd_outl(M8P_RTHW_PHASE_kphase_0C_reg, M8P_rthw_phase_kphase_0c.regValue);
}
VOID HAL_MC_SetKphaseFilmPhaseType(unsigned int FilmPhaseType)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetKphaseFilmPhaseType_RTK2885pp(FilmPhaseType);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetKphaseFilmPhaseType_RTK2885p(FilmPhaseType);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set the kphase sw film enable
 * @param [in] unsigned int swFilm_en
 * @retval VOID
*/
VOID HAL_MC_SetKphaseSWFilmEnable_RTK2885p(unsigned int swFilm_en)
{
	WriteRegister(KPHASE_kphase_0C_reg,24,24, swFilm_en);
}
VOID HAL_MC_SetKphaseSWFilmEnable_RTK2885pp(unsigned int swFilm_en)
{
	M8P_rthw_phase_kphase_0c_RBUS M8P_rthw_phase_kphase_0c;
	M8P_rthw_phase_kphase_0c.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_0C_reg);
	M8P_rthw_phase_kphase_0c.kphase_sw_film_en = swFilm_en;
	rtd_outl(M8P_RTHW_PHASE_kphase_0C_reg, M8P_rthw_phase_kphase_0c.regValue);
}
VOID HAL_MC_SetKphaseSWFilmEnable(unsigned int swFilm_en)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetKphaseSWFilmEnable_RTK2885pp(swFilm_en);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetKphaseSWFilmEnable_RTK2885p(swFilm_en);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get the kphase film mode
 * @param [in] VOID
 * @retval film_mode
*/
unsigned int HAL_MC_GetKphaseFilmMode_RTK2885p(VOID)
{
	unsigned int film_mode;
	ReadRegister(KPHASE_kphase_0C_reg,28,30, &film_mode);
	return film_mode;
}
unsigned int HAL_MC_GetKphaseFilmMode_RTK2885pp(VOID)
{
	M8P_rthw_phase_kphase_0c_RBUS M8P_rthw_phase_kphase_0c;
	unsigned int film_mode;
	
	M8P_rthw_phase_kphase_0c.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_0C_reg);
	film_mode = M8P_rthw_phase_kphase_0c.kphase_film_mode;
	return film_mode;
}
unsigned int HAL_MC_GetKphaseFilmMode(VOID)
{
	unsigned int u32_Mode = 0;

	if(RUN_MERLIN8P()){
		u32_Mode = HAL_MC_GetKphaseFilmMode_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_Mode = HAL_MC_GetKphaseFilmMode_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_Mode;
}

/**
 * @brief This function set the kphase film mode
 * @param [in] unsigned int film_mode
 * @retval VOID
*/
VOID HAL_MC_SetKphaseFilmMode_RTK2885p(unsigned int film_mode)
{
	WriteRegister(KPHASE_kphase_0C_reg,28,30, film_mode);
}
VOID HAL_MC_SetKphaseFilmMode_RTK2885pp(unsigned int film_mode)
{
	M8P_rthw_phase_kphase_0c_RBUS M8P_rthw_phase_kphase_0c;
	M8P_rthw_phase_kphase_0c.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_0C_reg);
	M8P_rthw_phase_kphase_0c.kphase_film_mode = film_mode;
	rtd_outl(M8P_RTHW_PHASE_kphase_0C_reg, M8P_rthw_phase_kphase_0c.regValue);
}
VOID HAL_MC_SetKphaseFilmMode(unsigned int film_mode)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetKphaseFilmMode_RTK2885pp(film_mode);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetKphaseFilmMode_RTK2885p(film_mode);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set decode data clear enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetKphaseDecDataClrEnable(BOOL bEnable)
{
	//reg_kphase_dec_data_vs_clr_en
	WriteRegister(KPHASE_kphase_78_reg,30,30,(unsigned int)bEnable);
}
#endif//REMOVE_REDUNDANT_API

/**
 * @brief This function set kphase force mc lf index
 * @param [in] unsigned int u32Idx
 * @retval VOID
*/
VOID HAL_MC_SetKphaseForceMCLFIdx_RTK2885p(unsigned int u32Idx)
{
	//reg_kphase_force_mc_lf_index
	WriteRegister(KPHASE_kphase_00_reg,20,23,u32Idx);
}
VOID HAL_MC_SetKphaseForceMCLFIdx_RTK2885pp(unsigned int u32Idx)
{
	M8P_rthw_phase_kphase_00_RBUS M8P_rthw_phase_kphase_00;
	M8P_rthw_phase_kphase_00.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_00_reg);
	M8P_rthw_phase_kphase_00.kphase_force_mc_lf_index = u32Idx;
	rtd_outl(M8P_RTHW_PHASE_kphase_00_reg, M8P_rthw_phase_kphase_00.regValue);
}
VOID HAL_MC_SetKphaseForceMCLFIdx(unsigned int u32Idx)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetKphaseForceMCLFIdx_RTK2885pp(u32Idx);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetKphaseForceMCLFIdx_RTK2885p(u32Idx);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set kphase force mc lf index enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetKphaseForceMCLFIdxEnable_RTK2885p(unsigned int u32Idx)
{
	//reg_kphase_force_mc_lf_index_en
	WriteRegister(KPHASE_kphase_00_reg,24,24,u32Idx);
}
VOID HAL_MC_SetKphaseForceMCLFIdxEnable_RTK2885pp(unsigned int u32Idx)
{
	M8P_rthw_phase_kphase_00_RBUS M8P_rthw_phase_kphase_00;
	M8P_rthw_phase_kphase_00.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_00_reg);
	M8P_rthw_phase_kphase_00.kphase_force_mc_lf_index_en = u32Idx;
	rtd_outl(M8P_RTHW_PHASE_kphase_00_reg, M8P_rthw_phase_kphase_00.regValue);
}
VOID HAL_MC_SetKphaseForceMCLFIdxEnable(unsigned int u32Idx)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetKphaseForceMCLFIdxEnable_RTK2885pp(u32Idx);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetKphaseForceMCLFIdxEnable_RTK2885p(u32Idx);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set kphase force mc hf index
 * @param [in] unsigned int u32Idx
 * @retval VOID
*/
VOID HAL_MC_SetKphaseForceMCHFIdx_RTK2885p(unsigned int u32Idx)
{
	//reg_kphase_force_mc_hf_index
	WriteRegister(KPHASE_kphase_00_reg,25,26,u32Idx);
}
VOID HAL_MC_SetKphaseForceMCHFIdx_RTK2885pp(unsigned int u32Idx)
{
	M8P_rthw_phase_kphase_00_RBUS M8P_rthw_phase_kphase_00;
	M8P_rthw_phase_kphase_00.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_00_reg);
	M8P_rthw_phase_kphase_00.kphase_force_mc_hf_index = u32Idx;
	rtd_outl(M8P_RTHW_PHASE_kphase_00_reg, M8P_rthw_phase_kphase_00.regValue);
}
VOID HAL_MC_SetKphaseForceMCHFIdx(unsigned int u32Idx)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetKphaseForceMCHFIdx_RTK2885pp(u32Idx);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetKphaseForceMCHFIdx_RTK2885p(u32Idx);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set kphase force mc hf index enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetKphaseForceMCHFIdxEnable_RTK2885p(unsigned int u32Idx)
{
	//reg_kphase_force_mc_hf_index_en
	WriteRegister(KPHASE_kphase_00_reg,27,27,u32Idx);
}
VOID HAL_MC_SetKphaseForceMCHFIdxEnable_RTK2885pp(unsigned int u32Idx)
{
	M8P_rthw_phase_kphase_00_RBUS M8P_rthw_phase_kphase_00;
	M8P_rthw_phase_kphase_00.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_00_reg);
	M8P_rthw_phase_kphase_00.kphase_force_mc_hf_index_en = u32Idx;
	rtd_outl(M8P_RTHW_PHASE_kphase_00_reg, M8P_rthw_phase_kphase_00.regValue);
}
VOID HAL_MC_SetKphaseForceMCHFIdxEnable(unsigned int u32Idx)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetKphaseForceMCHFIdxEnable_RTK2885pp(u32Idx);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetKphaseForceMCHFIdxEnable_RTK2885p(u32Idx);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set demo window Enable
 * @param [in] unsigned char ubyWinID
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_MC_SetDemoWindowEnable_RTK2885p(unsigned char ubyWinID, BOOL bEnable)
{
	unsigned int u32dw_en = 0;
	ReadRegister(MC_MC_60_reg,0,7, &u32dw_en);
	if(bEnable)
	{
		u32dw_en = u32dw_en | (1 << ubyWinID);
	}
	else
	{
		u32dw_en = u32dw_en & (~(1 << ubyWinID));
	}
	WriteRegister(MC_MC_60_reg,0,7, u32dw_en);
}
VOID HAL_MC_SetDemoWindowEnable_RTK2885pp(unsigned char ubyWinID, BOOL bEnable)
{
	M8P_rtmc_rtmc_20_RBUS M8P_rtmc_rtmc_20;
	M8P_rtmc_rtmc_20.regValue = rtd_inl(M8P_RTMC_RTMC_20_reg);
	if(ubyWinID==0){
		M8P_rtmc_rtmc_20.mc_dw_0_en = bEnable;
	}
	else if(ubyWinID==1){
		M8P_rtmc_rtmc_20.mc_dw_1_en = bEnable;
	}
	else if(ubyWinID==2){
		M8P_rtmc_rtmc_20.mc_dw_2_en = bEnable;
	}
	else if(ubyWinID==3){
		M8P_rtmc_rtmc_20.mc_dw_3_en = bEnable;
	}
	else if(ubyWinID==4){
		M8P_rtmc_rtmc_20.mc_dw_4_en = bEnable;
	}
	else if(ubyWinID==5){
		M8P_rtmc_rtmc_20.mc_dw_5_en = bEnable;
	}
	else if(ubyWinID==6){
		M8P_rtmc_rtmc_20.mc_dw_6_en = bEnable;
	}
	else if(ubyWinID==7){
		M8P_rtmc_rtmc_20.mc_dw_7_en = bEnable;
	}
	rtd_outl(M8P_RTMC_RTMC_20_reg, M8P_rtmc_rtmc_20.regValue);
}
VOID HAL_MC_SetDemoWindowEnable(unsigned char ubyWinID, BOOL bEnable)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetDemoWindowEnable_RTK2885pp(ubyWinID, bEnable);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetDemoWindowEnable_RTK2885p(ubyWinID, bEnable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get demo window Enable
 * @param [in] unsigned char ubyWinID
 * @retval BOOL bEnable
*/
BOOL HAL_MC_GetDemoWindowEnable_RTK2885p(unsigned char ubyWinID)
{
	unsigned int u32dw_en = 0;
	ReadRegister(MC_MC_60_reg,0,7, &u32dw_en);
	if(u32dw_en & (1 << ubyWinID))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL HAL_MC_GetDemoWindowEnable_RTK2885pp(unsigned char ubyWinID)
{
	unsigned int u32dw_en = 0;
	M8P_rtmc_rtmc_20_RBUS M8P_rtmc_rtmc_20;
	M8P_rtmc_rtmc_20.regValue = rtd_inl(M8P_RTMC_RTMC_20_reg);

	if(ubyWinID==0){
		u32dw_en = M8P_rtmc_rtmc_20.mc_dw_0_en;
	}
	else if(ubyWinID==1){
		u32dw_en = M8P_rtmc_rtmc_20.mc_dw_1_en;
	}
	else if(ubyWinID==2){
		u32dw_en = M8P_rtmc_rtmc_20.mc_dw_2_en;
	}
	else if(ubyWinID==3){
		u32dw_en = M8P_rtmc_rtmc_20.mc_dw_3_en;
	}
	else if(ubyWinID==4){
		u32dw_en = M8P_rtmc_rtmc_20.mc_dw_4_en;
	}
	else if(ubyWinID==5){
		u32dw_en = M8P_rtmc_rtmc_20.mc_dw_5_en;
	}
	else if(ubyWinID==6){
		u32dw_en = M8P_rtmc_rtmc_20.mc_dw_6_en;
	}
	else if(ubyWinID==7){
		u32dw_en = M8P_rtmc_rtmc_20.mc_dw_7_en;
	}
	return u32dw_en;
}
BOOL HAL_MC_GetDemoWindowEnable(unsigned char ubyWinID)
{
	BOOL u1_Enbale = false;

	if(RUN_MERLIN8P()){
		u1_Enbale = HAL_MC_GetDemoWindowEnable_RTK2885pp(ubyWinID);
	}else if(RUN_MERLIN8()){
		u1_Enbale = HAL_MC_GetDemoWindowEnable_RTK2885p(ubyWinID);
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u1_Enbale;
}

/**
 * @brief This function set demo window mode
 * @param [in] BOOL bMode: 0: inside MEMC OFF; 1: outside MEMC OFF
 * @retval VOID
*/
VOID HAL_MC_SetDemoWindowMode_RTK2885p(BOOL bMode)
{
	if(bMode == FALSE)
	{
		WriteRegister(MC_MC_60_reg,8,8, 0);
	}
	else
	{
		WriteRegister(MC_MC_60_reg,8,8, 1);
	}
}

VOID HAL_MC_SetDemoWindowMode_RTK2885pp(BOOL bMode)
{
	M8P_rtmc_rtmc_20_RBUS M8P_rtmc_rtmc_20;
	M8P_rtmc_rtmc_20.regValue = rtd_inl(M8P_RTMC_RTMC_20_reg);
	M8P_rtmc_rtmc_20.mc_dw_mode = bMode;
	rtd_outl(M8P_RTMC_RTMC_20_reg, M8P_rtmc_rtmc_20.regValue);
}

VOID HAL_MC_SetDemoWindowMode(BOOL bMode)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetDemoWindowMode_RTK2885pp(bMode);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetDemoWindowMode_RTK2885p(bMode);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get demo window mode
 * @retval BOOL bMode: 0: inside MEMC OFF; 1: outside MEMC OFF
*/
BOOL HAL_MC_GetDemoWindowMode_RTK2885p(VOID)
{
	unsigned int u32dw_mode = 0;
	ReadRegister(MC_MC_60_reg,8,8, &u32dw_mode);

	if(u32dw_mode == 0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
BOOL HAL_MC_GetDemoWindowMode_RTK2885pp(VOID)
{
	unsigned int u32dw_mode = 0;
	M8P_rtmc_rtmc_20_RBUS M8P_rtmc_rtmc_20;

	M8P_rtmc_rtmc_20.regValue = rtd_inl(M8P_RTMC_RTMC_20_reg);
	u32dw_mode = M8P_rtmc_rtmc_20.mc_dw_mode;
	return u32dw_mode;
}
BOOL HAL_MC_GetDemoWindowMode(VOID)
{
	BOOL u1_Mode = 0;

	if(RUN_MERLIN8P()){
		u1_Mode = HAL_MC_GetDemoWindowMode_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u1_Mode = HAL_MC_GetDemoWindowMode_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u1_Mode;
}

/**
 * @brief This function set demo window
 * @param [in] unsigned char ubyWinID
 * @param [in] unsigned int u32Left
 * @param [in] unsigned int u32Right
 * @param [in] unsigned int u32Top
 * @param [in] unsigned int u32Bottom
 * @retval VOID
*/
VOID HAL_MC_SetDemoWindowSettings_RTK2885p(unsigned char ubyWinID, unsigned int u32Left, unsigned int u32Right, unsigned int u32Top, unsigned int u32Bottom)
{
	if(ubyWinID == 0)
	{
		WriteRegister(MC_MC_68_reg,0,11, u32Left);
		WriteRegister(MC_MC_68_reg,16,27, u32Right);
		WriteRegister(MC_MC_64_reg,0,11, u32Top);
		WriteRegister(MC_MC_64_reg,16,27, u32Bottom);
	}
	else if(ubyWinID == 1)
	{
		WriteRegister(MC_MC_70_reg,0,11, u32Left);
		WriteRegister(MC_MC_70_reg,16,27, u32Right);
		WriteRegister(MC_MC_6C_reg,0,11, u32Top);
		WriteRegister(MC_MC_6C_reg,16,27, u32Bottom);
	}
	else if(ubyWinID == 2)
	{
		WriteRegister(MC_MC_78_reg,0,11, u32Left);
		WriteRegister(MC_MC_78_reg,16,27, u32Right);
		WriteRegister(MC_MC_74_reg,0,11, u32Top);
		WriteRegister(MC_MC_74_reg,16,27, u32Bottom);
	}
	else if(ubyWinID == 3)
	{
		WriteRegister(MC_MC_80_reg,0,11, u32Left);
		WriteRegister(MC_MC_80_reg,16,27, u32Right);
		WriteRegister(MC_MC_7C_reg,0,11, u32Top);
		WriteRegister(MC_MC_7C_reg,16,27, u32Bottom);
	}
	else if(ubyWinID == 4)
	{
		WriteRegister(MC_MC_88_reg,0,11, u32Left);
		WriteRegister(MC_MC_88_reg,16,27, u32Right);
		WriteRegister(MC_MC_84_reg,0,11, u32Top);
		WriteRegister(MC_MC_84_reg,16,27, u32Bottom);
	}
	else if(ubyWinID == 5)
	{
		WriteRegister(MC_MC_90_reg,0,11, u32Left);
		WriteRegister(MC_MC_90_reg,16,27, u32Right);
		WriteRegister(MC_MC_8C_reg,0,11, u32Top);
		WriteRegister(MC_MC_8C_reg,16,27, u32Bottom);
	}
	else if(ubyWinID == 6)
	{
		WriteRegister(MC_MC_98_reg,0,11, u32Left);
		WriteRegister(MC_MC_98_reg,16,27, u32Right);
		WriteRegister(MC_MC_94_reg,0,11, u32Top);
		WriteRegister(MC_MC_94_reg,16,27, u32Bottom);
	}
	else if(ubyWinID == 7)
	{
		WriteRegister(MC_MC_A0_reg,0,11, u32Left);
		WriteRegister(MC_MC_A0_reg,16,27, u32Right);
		WriteRegister(MC_MC_9C_reg,0,11, u32Top);
		WriteRegister(MC_MC_9C_reg,16,27, u32Bottom);
	}
}
VOID HAL_MC_SetDemoWindowSettings_RTK2885pp(unsigned char ubyWinID, unsigned int u32Left, unsigned int u32Right, unsigned int u32Top, unsigned int u32Bottom)
{
	M8P_rtmc_rtmc_24_RBUS M8P_rtmc_rtmc_24;
	M8P_rtmc_rtmc_28_RBUS M8P_rtmc_rtmc_28;
	M8P_rtmc_rtmc_2c_RBUS M8P_rtmc_rtmc_2c;
	M8P_rtmc_rtmc_30_RBUS M8P_rtmc_rtmc_30;
	M8P_rtmc_rtmc_34_RBUS M8P_rtmc_rtmc_34;
	M8P_rtmc_rtmc_38_RBUS M8P_rtmc_rtmc_38;
	M8P_rtmc_rtmc_3c_RBUS M8P_rtmc_rtmc_3c;
	M8P_rtmc_rtmc_40_RBUS M8P_rtmc_rtmc_40;
	M8P_rtmc_rtmc_44_RBUS M8P_rtmc_rtmc_44;
	M8P_rtmc_rtmc_48_RBUS M8P_rtmc_rtmc_48;
	M8P_rtmc_rtmc_4c_RBUS M8P_rtmc_rtmc_4c;
	M8P_rtmc_rtmc_50_RBUS M8P_rtmc_rtmc_50;
	M8P_rtmc_rtmc_54_RBUS M8P_rtmc_rtmc_54;
	M8P_rtmc_rtmc_58_RBUS M8P_rtmc_rtmc_58;
	M8P_rtmc_rtmc_5c_RBUS M8P_rtmc_rtmc_5c;
	M8P_rtmc_rtmc_60_RBUS M8P_rtmc_rtmc_60;

	if(ubyWinID == 0)
	{
		M8P_rtmc_rtmc_24.regValue = rtd_inl(M8P_RTMC_RTMC_24_reg);
		M8P_rtmc_rtmc_28.regValue = rtd_inl(M8P_RTMC_RTMC_28_reg);
		M8P_rtmc_rtmc_24.mc_dw_top0 = u32Top;
		M8P_rtmc_rtmc_24.mc_dw_bot0 = u32Bottom;
		M8P_rtmc_rtmc_28.mc_dw_lft0 = u32Left;
		M8P_rtmc_rtmc_28.mc_dw_rht0 = u32Right;
		rtd_outl(M8P_RTMC_RTMC_24_reg, M8P_rtmc_rtmc_24.regValue);
		rtd_outl(M8P_RTMC_RTMC_28_reg, M8P_rtmc_rtmc_28.regValue);
	}
	else if(ubyWinID == 1)
	{
		M8P_rtmc_rtmc_2c.regValue = rtd_inl(M8P_RTMC_RTMC_2C_reg);
		M8P_rtmc_rtmc_30.regValue = rtd_inl(M8P_RTMC_RTMC_30_reg);
		M8P_rtmc_rtmc_2c.mc_dw_top1 = u32Top;
		M8P_rtmc_rtmc_2c.mc_dw_bot1 = u32Bottom;
		M8P_rtmc_rtmc_30.mc_dw_lft1 = u32Left;
		M8P_rtmc_rtmc_30.mc_dw_rht1 = u32Right;
		rtd_outl(M8P_RTMC_RTMC_2C_reg, M8P_rtmc_rtmc_2c.regValue);
		rtd_outl(M8P_RTMC_RTMC_30_reg, M8P_rtmc_rtmc_30.regValue);
	}
	else if(ubyWinID == 2)
	{
		M8P_rtmc_rtmc_34.regValue = rtd_inl(M8P_RTMC_RTMC_34_reg);
		M8P_rtmc_rtmc_38.regValue = rtd_inl(M8P_RTMC_RTMC_38_reg);
		M8P_rtmc_rtmc_34.mc_dw_top2 = u32Top;
		M8P_rtmc_rtmc_34.mc_dw_bot2 = u32Bottom;
		M8P_rtmc_rtmc_38.mc_dw_lft2 = u32Left;
		M8P_rtmc_rtmc_38.mc_dw_rht2 = u32Right;
		rtd_outl(M8P_RTMC_RTMC_34_reg, M8P_rtmc_rtmc_34.regValue);
		rtd_outl(M8P_RTMC_RTMC_38_reg, M8P_rtmc_rtmc_38.regValue);
	}
	else if(ubyWinID == 3)
	{
		M8P_rtmc_rtmc_3c.regValue = rtd_inl(M8P_RTMC_RTMC_3C_reg);
		M8P_rtmc_rtmc_40.regValue = rtd_inl(M8P_RTMC_RTMC_40_reg);
		M8P_rtmc_rtmc_3c.mc_dw_top3 = u32Top;
		M8P_rtmc_rtmc_3c.mc_dw_bot3 = u32Bottom;
		M8P_rtmc_rtmc_40.mc_dw_lft3 = u32Left;
		M8P_rtmc_rtmc_40.mc_dw_rht3 = u32Right;
		rtd_outl(M8P_RTMC_RTMC_3C_reg, M8P_rtmc_rtmc_3c.regValue);
		rtd_outl(M8P_RTMC_RTMC_40_reg, M8P_rtmc_rtmc_40.regValue);
	}
	else if(ubyWinID == 4)
	{
		M8P_rtmc_rtmc_44.regValue = rtd_inl(M8P_RTMC_RTMC_44_reg);
		M8P_rtmc_rtmc_48.regValue = rtd_inl(M8P_RTMC_RTMC_48_reg);
		M8P_rtmc_rtmc_44.mc_dw_top4 = u32Top;
		M8P_rtmc_rtmc_44.mc_dw_bot4 = u32Bottom;
		M8P_rtmc_rtmc_48.mc_dw_lft4 = u32Left;
		M8P_rtmc_rtmc_48.mc_dw_rht4 = u32Right;
		rtd_outl(M8P_RTMC_RTMC_44_reg, M8P_rtmc_rtmc_44.regValue);
		rtd_outl(M8P_RTMC_RTMC_48_reg, M8P_rtmc_rtmc_48.regValue);
	}
	else if(ubyWinID == 5)
	{
		M8P_rtmc_rtmc_4c.regValue = rtd_inl(M8P_RTMC_RTMC_4C_reg);
		M8P_rtmc_rtmc_50.regValue = rtd_inl(M8P_RTMC_RTMC_50_reg);
		M8P_rtmc_rtmc_4c.mc_dw_top5 = u32Top;
		M8P_rtmc_rtmc_4c.mc_dw_bot5 = u32Bottom;
		M8P_rtmc_rtmc_50.mc_dw_lft5 = u32Left;
		M8P_rtmc_rtmc_50.mc_dw_rht5 = u32Right;
		rtd_outl(M8P_RTMC_RTMC_4C_reg, M8P_rtmc_rtmc_4c.regValue);
		rtd_outl(M8P_RTMC_RTMC_50_reg, M8P_rtmc_rtmc_50.regValue);
	}
	else if(ubyWinID == 6)
	{
		M8P_rtmc_rtmc_54.regValue = rtd_inl(M8P_RTMC_RTMC_54_reg);
		M8P_rtmc_rtmc_58.regValue = rtd_inl(M8P_RTMC_RTMC_58_reg);
		M8P_rtmc_rtmc_54.mc_dw_top6 = u32Top;
		M8P_rtmc_rtmc_54.mc_dw_bot6 = u32Bottom;
		M8P_rtmc_rtmc_58.mc_dw_lft6 = u32Left;
		M8P_rtmc_rtmc_58.mc_dw_rht6 = u32Right;
		rtd_outl(M8P_RTMC_RTMC_54_reg, M8P_rtmc_rtmc_54.regValue);
		rtd_outl(M8P_RTMC_RTMC_58_reg, M8P_rtmc_rtmc_58.regValue);
	}
	else if(ubyWinID == 7)
	{
		M8P_rtmc_rtmc_5c.regValue = rtd_inl(M8P_RTMC_RTMC_5C_reg);
		M8P_rtmc_rtmc_60.regValue = rtd_inl(M8P_RTMC_RTMC_60_reg);
		M8P_rtmc_rtmc_5c.mc_dw_top7 = u32Top;
		M8P_rtmc_rtmc_5c.mc_dw_bot7 = u32Bottom;
		M8P_rtmc_rtmc_60.mc_dw_lft7 = u32Left;
		M8P_rtmc_rtmc_60.mc_dw_rht7 = u32Right;
		rtd_outl(M8P_RTMC_RTMC_5C_reg, M8P_rtmc_rtmc_5c.regValue);
		rtd_outl(M8P_RTMC_RTMC_60_reg, M8P_rtmc_rtmc_60.regValue);
	}
}
VOID HAL_MC_SetDemoWindowSettings(unsigned char ubyWinID, unsigned int u32Left, unsigned int u32Right, unsigned int u32Top, unsigned int u32Bottom)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetDemoWindowSettings_RTK2885pp(ubyWinID, u32Left, u32Right, u32Top, u32Bottom);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetDemoWindowSettings_RTK2885p(ubyWinID, u32Left, u32Right, u32Top, u32Bottom);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get demo window
 * @param [in] unsigned char ubyWinID
 * @param [out] unsigned int u32Left
 * @param [out] unsigned int u32Right
 * @param [out] unsigned int u32Top
 * @param [out] unsigned int u32Bottom
 * @retval VOID
*/
VOID HAL_MC_GetDemoWindowSettings_RTK2885p(unsigned char ubyWinID, unsigned int *pu32Left, unsigned int *pu32Right, unsigned int *pu32Top, unsigned int *pu32Bottom)
{
	if(ubyWinID == 0)
	{
		ReadRegister(MC_MC_68_reg,0,11, pu32Left);
		ReadRegister(MC_MC_68_reg,16,27, pu32Right);
		ReadRegister(MC_MC_64_reg,0,11, pu32Top);
		ReadRegister(MC_MC_64_reg,16,27, pu32Bottom);
	}
	else if(ubyWinID == 1)
	{
		ReadRegister(MC_MC_70_reg,0,11, pu32Left);
		ReadRegister(MC_MC_70_reg,16,27, pu32Right);
		ReadRegister(MC_MC_6C_reg,0,11, pu32Top);
		ReadRegister(MC_MC_6C_reg,16,27, pu32Bottom);
	}
	else if(ubyWinID == 2)
	{
		ReadRegister(MC_MC_78_reg,0,11, pu32Left);
		ReadRegister(MC_MC_78_reg,16,27, pu32Right);
		ReadRegister(MC_MC_74_reg,0,11, pu32Top);
		ReadRegister(MC_MC_74_reg,16,27, pu32Bottom);
	}
	else if(ubyWinID == 3)
	{
		ReadRegister(MC_MC_80_reg,0,11, pu32Left);
		ReadRegister(MC_MC_80_reg,16,27, pu32Right);
		ReadRegister(MC_MC_7C_reg,0,11, pu32Top);
		ReadRegister(MC_MC_7C_reg,16,27, pu32Bottom);
	}
	else if(ubyWinID == 4)
	{
		ReadRegister(MC_MC_88_reg,0,11, pu32Left);
		ReadRegister(MC_MC_88_reg,16,27, pu32Right);
		ReadRegister(MC_MC_84_reg,0,11, pu32Top);
		ReadRegister(MC_MC_84_reg,16,27, pu32Bottom);
	}
	else if(ubyWinID == 5)
	{
		ReadRegister(MC_MC_90_reg,0,11, pu32Left);
		ReadRegister(MC_MC_90_reg,16,27, pu32Right);
		ReadRegister(MC_MC_8C_reg,0,11, pu32Top);
		ReadRegister(MC_MC_8C_reg,16,27, pu32Bottom);
	}
	else if(ubyWinID == 6)
	{
		ReadRegister(MC_MC_98_reg,0,11, pu32Left);
		ReadRegister(MC_MC_98_reg,16,27, pu32Right);
		ReadRegister(MC_MC_94_reg,0,11, pu32Top);
		ReadRegister(MC_MC_94_reg,16,27, pu32Bottom);
	}
	else if(ubyWinID == 7)
	{
		ReadRegister(MC_MC_A0_reg,0,11, pu32Left);
		ReadRegister(MC_MC_A0_reg,16,27, pu32Right);
		ReadRegister(MC_MC_9C_reg,0,11, pu32Top);
		ReadRegister(MC_MC_9C_reg,16,27, pu32Bottom);
	}
}
VOID HAL_MC_GetDemoWindowSettings_RTK2885pp(unsigned char ubyWinID, unsigned int *pu32Left, unsigned int *pu32Right, unsigned int *pu32Top, unsigned int *pu32Bottom)
{
	M8P_rtmc_rtmc_24_RBUS M8P_rtmc_rtmc_24;
	M8P_rtmc_rtmc_28_RBUS M8P_rtmc_rtmc_28;
	M8P_rtmc_rtmc_2c_RBUS M8P_rtmc_rtmc_2c;
	M8P_rtmc_rtmc_30_RBUS M8P_rtmc_rtmc_30;
	M8P_rtmc_rtmc_34_RBUS M8P_rtmc_rtmc_34;
	M8P_rtmc_rtmc_38_RBUS M8P_rtmc_rtmc_38;
	M8P_rtmc_rtmc_3c_RBUS M8P_rtmc_rtmc_3c;
	M8P_rtmc_rtmc_40_RBUS M8P_rtmc_rtmc_40;
	M8P_rtmc_rtmc_44_RBUS M8P_rtmc_rtmc_44;
	M8P_rtmc_rtmc_48_RBUS M8P_rtmc_rtmc_48;
	M8P_rtmc_rtmc_4c_RBUS M8P_rtmc_rtmc_4c;
	M8P_rtmc_rtmc_50_RBUS M8P_rtmc_rtmc_50;
	M8P_rtmc_rtmc_54_RBUS M8P_rtmc_rtmc_54;
	M8P_rtmc_rtmc_58_RBUS M8P_rtmc_rtmc_58;
	M8P_rtmc_rtmc_5c_RBUS M8P_rtmc_rtmc_5c;
	M8P_rtmc_rtmc_60_RBUS M8P_rtmc_rtmc_60;

	if(ubyWinID == 0)
	{
		M8P_rtmc_rtmc_24.regValue = rtd_inl(M8P_RTMC_RTMC_24_reg);
		M8P_rtmc_rtmc_28.regValue = rtd_inl(M8P_RTMC_RTMC_28_reg);
		*pu32Top	= M8P_rtmc_rtmc_24.mc_dw_top0;
		*pu32Bottom = M8P_rtmc_rtmc_24.mc_dw_bot0;
		*pu32Left	= M8P_rtmc_rtmc_28.mc_dw_lft0;
		*pu32Right	= M8P_rtmc_rtmc_28.mc_dw_rht0;
	}
	else if(ubyWinID == 1)
	{
		M8P_rtmc_rtmc_2c.regValue = rtd_inl(M8P_RTMC_RTMC_2C_reg);
		M8P_rtmc_rtmc_30.regValue = rtd_inl(M8P_RTMC_RTMC_30_reg);
		*pu32Top	= M8P_rtmc_rtmc_2c.mc_dw_top1;
		*pu32Bottom = M8P_rtmc_rtmc_2c.mc_dw_bot1;
		*pu32Left	= M8P_rtmc_rtmc_30.mc_dw_lft1;
		*pu32Right	= M8P_rtmc_rtmc_30.mc_dw_rht1;
	}
	else if(ubyWinID == 2)
	{
		M8P_rtmc_rtmc_34.regValue = rtd_inl(M8P_RTMC_RTMC_34_reg);
		M8P_rtmc_rtmc_38.regValue = rtd_inl(M8P_RTMC_RTMC_38_reg);
		*pu32Top	= M8P_rtmc_rtmc_34.mc_dw_top2;
		*pu32Bottom = M8P_rtmc_rtmc_34.mc_dw_bot2;
		*pu32Left	= M8P_rtmc_rtmc_38.mc_dw_lft2;
		*pu32Right	= M8P_rtmc_rtmc_38.mc_dw_rht2;
	}
	else if(ubyWinID == 3)
	{
		M8P_rtmc_rtmc_3c.regValue = rtd_inl(M8P_RTMC_RTMC_3C_reg);
		M8P_rtmc_rtmc_40.regValue = rtd_inl(M8P_RTMC_RTMC_40_reg);
		*pu32Top	= M8P_rtmc_rtmc_3c.mc_dw_top3;
		*pu32Bottom = M8P_rtmc_rtmc_3c.mc_dw_bot3;
		*pu32Left	= M8P_rtmc_rtmc_40.mc_dw_lft3;
		*pu32Right	= M8P_rtmc_rtmc_40.mc_dw_rht3;
	}
	else if(ubyWinID == 4)
	{
		M8P_rtmc_rtmc_44.regValue = rtd_inl(M8P_RTMC_RTMC_44_reg);
		M8P_rtmc_rtmc_48.regValue = rtd_inl(M8P_RTMC_RTMC_48_reg);
		*pu32Top	= M8P_rtmc_rtmc_44.mc_dw_top4;
		*pu32Bottom = M8P_rtmc_rtmc_44.mc_dw_bot4;
		*pu32Left	= M8P_rtmc_rtmc_48.mc_dw_lft4;
		*pu32Right	= M8P_rtmc_rtmc_48.mc_dw_rht4;
	}
	else if(ubyWinID == 5)
	{
		M8P_rtmc_rtmc_4c.regValue = rtd_inl(M8P_RTMC_RTMC_4C_reg);
		M8P_rtmc_rtmc_50.regValue = rtd_inl(M8P_RTMC_RTMC_50_reg);
		*pu32Top	= M8P_rtmc_rtmc_4c.mc_dw_top5;
		*pu32Bottom = M8P_rtmc_rtmc_4c.mc_dw_bot5;
		*pu32Left	= M8P_rtmc_rtmc_50.mc_dw_lft5;
		*pu32Right	= M8P_rtmc_rtmc_50.mc_dw_rht5;
	}
	else if(ubyWinID == 6)
	{
		M8P_rtmc_rtmc_54.regValue = rtd_inl(M8P_RTMC_RTMC_54_reg);
		M8P_rtmc_rtmc_58.regValue = rtd_inl(M8P_RTMC_RTMC_58_reg);
		*pu32Top	= M8P_rtmc_rtmc_54.mc_dw_top6;
		*pu32Bottom = M8P_rtmc_rtmc_54.mc_dw_bot6;
		*pu32Left	= M8P_rtmc_rtmc_58.mc_dw_lft6;
		*pu32Right	= M8P_rtmc_rtmc_58.mc_dw_rht6;
	}
	else if(ubyWinID == 7)
	{
		M8P_rtmc_rtmc_5c.regValue = rtd_inl(M8P_RTMC_RTMC_5C_reg);
		M8P_rtmc_rtmc_60.regValue = rtd_inl(M8P_RTMC_RTMC_60_reg);
		*pu32Top	= M8P_rtmc_rtmc_5c.mc_dw_top7;
		*pu32Bottom = M8P_rtmc_rtmc_5c.mc_dw_bot7;
		*pu32Left	= M8P_rtmc_rtmc_60.mc_dw_lft7;
		*pu32Right	= M8P_rtmc_rtmc_60.mc_dw_rht7;
	}
}

VOID HAL_MC_GetDemoWindowSettings(unsigned char ubyWinID, unsigned int *pu32Left, unsigned int *pu32Right, unsigned int *pu32Top, unsigned int *pu32Bottom)
{
	if(RUN_MERLIN8P()){
		HAL_MC_GetDemoWindowSettings_RTK2885pp(ubyWinID, pu32Left, pu32Right, pu32Top, pu32Bottom);
	}else if(RUN_MERLIN8()){
		HAL_MC_GetDemoWindowSettings_RTK2885p(ubyWinID, pu32Left, pu32Right, pu32Top, pu32Bottom);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set lbmc normal lf prefetch number
 * @param [in] BOOL bType: 0 for i, 1 for p
 * @param [in] ubyIdx(0~8)
 * @param [in] ubyPreFetch_Num
 * @retval VOID
*/
VOID HAL_MC_SetLBMCNormalLFPreFetchNum(BOOL bType, unsigned char ubyIdx, unsigned char ubyPreFetch_Num)
{
	if(!bType)	// i
	{
		switch(ubyIdx)
		{
			case 0:
				//nor_ilf_idx0_prefetch_num
				WriteRegister(LBMC_LBMC_2C_reg,0,6,ubyPreFetch_Num);
				break;
			case 1:
				//nor_ilf_idx1_prefetch_num
				WriteRegister(LBMC_LBMC_2C_reg,7,13,ubyPreFetch_Num);
				break;
			case 2:
				//nor_ilf_idx2_prefetch_num
				WriteRegister(LBMC_LBMC_2C_reg,14,20,ubyPreFetch_Num);
				break;
			case 3:
				//nor_ilf_idx3_prefetch_num
				WriteRegister(LBMC_LBMC_2C_reg,21,27,ubyPreFetch_Num);
				break;
			case 4:
				//nor_ilf_idx4_prefetch_num
				WriteRegister(LBMC_LBMC_30_reg,0,6,ubyPreFetch_Num);
				break;
			case 5:
				//nor_ilf_idx5_prefetch_num
				WriteRegister(LBMC_LBMC_30_reg,7,13,ubyPreFetch_Num);
				break;
			case 6:
				//nor_ilf_idx6_prefetch_num
				WriteRegister(LBMC_LBMC_30_reg,14,20,ubyPreFetch_Num);
				break;
			case 7:
				//nor_ilf_idx7_prefetch_num
				WriteRegister(LBMC_LBMC_30_reg,21,27,ubyPreFetch_Num);
				break;
			case 8:
				//nor_ilf_idx8_prefetch_num
				WriteRegister(LBMC_LBMC_34_reg,0,6,ubyPreFetch_Num);
				break;
			default:
				break;
		}
	}
	else	//	p
	{
		switch(ubyIdx)
		{
			case 0:
				//nor_plf_idx0_prefetch_num
				WriteRegister(LBMC_LBMC_38_reg,0,6,ubyPreFetch_Num);
				break;
			case 1:
				//nor_plf_idx1_prefetch_num
				WriteRegister(LBMC_LBMC_38_reg,7,13,ubyPreFetch_Num);
				break;
			case 2:
				//nor_plf_idx2_prefetch_num
				WriteRegister(LBMC_LBMC_38_reg,14,20,ubyPreFetch_Num);
				break;
			case 3:
				//nor_plf_idx3_prefetch_num
				WriteRegister(LBMC_LBMC_38_reg,21,27,ubyPreFetch_Num);
				break;
			case 4:
				//nor_plf_idx4_prefetch_num
				WriteRegister(LBMC_LBMC_3C_reg,0,6,ubyPreFetch_Num);
				break;
			case 5:
				//nor_plf_idx5_prefetch_num
				WriteRegister(LBMC_LBMC_3C_reg,7,13,ubyPreFetch_Num);
				break;
			case 6:
				//nor_plf_idx6_prefetch_num
				WriteRegister(LBMC_LBMC_3C_reg,14,20,ubyPreFetch_Num);
				break;
			case 7:
				//nor_plf_idx7_prefetch_num
				WriteRegister(LBMC_LBMC_3C_reg,21,27,ubyPreFetch_Num);
				break;
			case 8:
				//nor_plf_idx8_prefetch_num
				WriteRegister(LBMC_LBMC_40_reg,0,6,ubyPreFetch_Num);
				break;
			default:
				break;
		}
	}
}

/**
 * @brief This function set lbmc normal hf prefetch number
 * @param [in] BOOL bType: 0 for i, 1 for p
 * @param [in] ubyIdx(0~2)
 * @param [in] ubyPreFetch_Num
 * @retval VOID
*/
VOID HAL_MC_SetLBMCNormalHFPreFetchNum(BOOL bType, unsigned char ubyIdx, unsigned char ubyPreFetch_Num)
{
	if(!bType)	// i
	{
		switch(ubyIdx)
		{
			case 0:
				//nor_ihf_idx0_prefetch_num
				WriteRegister(LBMC_LBMC_44_reg,0,3,ubyPreFetch_Num);
				break;
			case 1:
				//nor_ihf_idx1_prefetch_num
				WriteRegister(LBMC_LBMC_44_reg,4,7,ubyPreFetch_Num);
				break;
			case 2:
				//nor_ihf_idx2_prefetch_num
				WriteRegister(LBMC_LBMC_44_reg,8,11,ubyPreFetch_Num);
				break;
			default:
				break;
		}
	}
	else	//	p
	{
		switch(ubyIdx)
		{
			case 0:
				//nor_phf_idx0_prefetch_num
				WriteRegister(LBMC_LBMC_44_reg,16,19,ubyPreFetch_Num);
				break;
			case 1:
				//nor_phf_idx1_prefetch_num
				WriteRegister(LBMC_LBMC_44_reg,20,23,ubyPreFetch_Num);
				break;
			case 2:
				//nor_phf_idx2_prefetch_num
				WriteRegister(LBMC_LBMC_44_reg,24,27,ubyPreFetch_Num);
				break;
			default:
				break;
		}
	}
}

/**
 * @brief This function set lbmc single lf prefetch number
 * @param [in] BOOL bType: 0 for i, 1 for p
 * @param [in] ubyIdx(0~2)
 * @param [in] ubyPreFetch_Num
 * @retval VOID
*/
VOID HAL_MC_SetLBMCSingleLFPreFetchNum(BOOL bType, unsigned char ubyIdx, unsigned char ubyPreFetch_Num)
{
	if(!bType)	// i
	{
		switch(ubyIdx)
		{
			case 0:
				//sing_ilf_idx0_prefetch_num
				WriteRegister(LBMC_LBMC_48_reg,0,6,ubyPreFetch_Num);
				break;
			case 1:
				//sing_ilf_idx1_prefetch_num
				WriteRegister(LBMC_LBMC_48_reg,7,13,ubyPreFetch_Num);
				break;
			case 2:
				//sing_ilf_idx2_prefetch_num
				WriteRegister(LBMC_LBMC_48_reg,14,20,ubyPreFetch_Num);
				break;
			case 3:
				//sing_ilf_idx3_prefetch_num
				WriteRegister(LBMC_LBMC_48_reg,21,27,ubyPreFetch_Num);
				break;
			case 4:
				//sing_ilf_idx4_prefetch_num
				WriteRegister(LBMC_LBMC_4C_reg,0,6,ubyPreFetch_Num);
				break;
			case 5:
				//sing_ilf_idx5_prefetch_num
				WriteRegister(LBMC_LBMC_4C_reg,7,13,ubyPreFetch_Num);
				break;
			case 6:
				//sing_ilf_idx6_prefetch_num
				WriteRegister(LBMC_LBMC_4C_reg,14,20,ubyPreFetch_Num);
				break;
			case 7:
				//sing_ilf_idx7_prefetch_num
				WriteRegister(LBMC_LBMC_4C_reg,21,27,ubyPreFetch_Num);
				break;
			case 8:
				//sing_ilf_idx8_prefetch_num
				WriteRegister(LBMC_LBMC_50_reg,0,6,ubyPreFetch_Num);
				break;
			default:
				break;
		}
	}
	else	//	p
	{
		switch(ubyIdx)
		{
			case 0:
				//sing_plf_idx0_prefetch_num
				WriteRegister(LBMC_LBMC_54_reg,0,6,ubyPreFetch_Num);
				break;
			case 1:
				//sing_plf_idx1_prefetch_num
				WriteRegister(LBMC_LBMC_54_reg,7,13,ubyPreFetch_Num);
				break;
			case 2:
				//sing_plf_idx2_prefetch_num
				WriteRegister(LBMC_LBMC_54_reg,14,20,ubyPreFetch_Num);
				break;
			case 3:
				//sing_plf_idx3_prefetch_num
				WriteRegister(LBMC_LBMC_54_reg,21,27,ubyPreFetch_Num);
				break;
			case 4:
				//sing_plf_idx4_prefetch_num
				WriteRegister(LBMC_LBMC_58_reg,0,6,ubyPreFetch_Num);
				break;
			case 5:
				//sing_plf_idx5_prefetch_num
				WriteRegister(LBMC_LBMC_58_reg,7,13,ubyPreFetch_Num);
				break;
			case 6:
				//sing_plf_idx6_prefetch_num
				WriteRegister(LBMC_LBMC_58_reg,14,20,ubyPreFetch_Num);
				break;
			case 7:
				//sing_plf_idx7_prefetch_num
				WriteRegister(LBMC_LBMC_58_reg,21,27,ubyPreFetch_Num);
				break;
			case 8:
				//sing_plf_idx8_prefetch_num
				WriteRegister(LBMC_LBMC_5C_reg,0,6,ubyPreFetch_Num);
				break;
			default:
				break;
		}
	}
}

/**
 * @brief This function set lbmc single hf prefetch number
 * @param [in] BOOL bType: 0 for i, 1 for p
 * @param [in] ubyIdx(0~2)
 * @param [in] ubyPreFetch_Num
 * @retval VOID
*/
VOID HAL_MC_SetLBMCSingleHFPreFetchNum(BOOL bType, unsigned char ubyIdx, unsigned char ubyPreFetch_Num)
{
	if(!bType)	// i
	{
		switch(ubyIdx)
		{
			case 0:
				//sing_ihf_idx0_prefetch_num
				WriteRegister(LBMC_LBMC_60_reg,0,4,ubyPreFetch_Num);
				break;
			case 1:
				//sing_ihf_idx1_prefetch_num
				WriteRegister(LBMC_LBMC_60_reg,5,9,ubyPreFetch_Num);
				break;
			case 2:
				//sing_ihf_idx2_prefetch_num
				WriteRegister(LBMC_LBMC_60_reg,10,14,ubyPreFetch_Num);
				break;
			default:
				break;
		}
	}
	else	//	p
	{
		switch(ubyIdx)
		{
			case 0:
				//sing_phf_idx0_prefetch_num
				WriteRegister(LBMC_LBMC_60_reg,16,20,ubyPreFetch_Num);
				break;
			case 1:
				//sing_phf_idx1_prefetch_num
				WriteRegister(LBMC_LBMC_60_reg,21,25,ubyPreFetch_Num);
				break;
			case 2:
				//sing_phf_idx2_prefetch_num
				WriteRegister(LBMC_LBMC_60_reg,26,30,ubyPreFetch_Num);
				break;
			default:
				break;
		}
	}
}

/**
 * @brief This function set mc rim 0
 * @param [in] unsigned int u32Top
 * @param [in] unsigned int u32Bottom
 * @param [in] unsigned int u32Left
 * @param [in] unsigned int u32Right
 * @retval VOID
*/
VOID HAL_MC_SetMCRim0(unsigned int u32Top, unsigned int u32Bottom, unsigned int u32Left, unsigned int u32Right)
{

#if RTK_MEMC_Performance_tunging_from_tv001
		mc_mc_50_RBUS mc_mc_50;
		mc_mc_54_RBUS mc_mc_54;
		mc_mc_50.regValue = rtd_inl(MC_MC_50_reg);
		mc_mc_54.regValue = rtd_inl(MC_MC_54_reg);
	
		mc_mc_50.mc_top_rim0 = u32Top;
		mc_mc_50.mc_bot_rim0 = u32Bottom;
		mc_mc_54.mc_lft_rim0 = u32Left;
		mc_mc_54.mc_rht_rim0 = u32Right;
	
		rtd_outl(MC_MC_50_reg, mc_mc_50.regValue);
		rtd_outl(MC_MC_54_reg, mc_mc_54.regValue);
#if 0
		//reg_mc_top_rim0
		WriteRegister(MC_MC_50_reg,0,12, u32Top);
		//reg_mc_bot_rim0
		WriteRegister(MC_MC_50_reg,16,28, u32Bottom);
		//reg_mc_lft_rim0
		WriteRegister(MC_MC_54_reg,0,12, u32Left);
		//reg_mc_rht_rim0
		WriteRegister(MC_MC_54_reg,16,28, u32Right);
#endif

#else
	//reg_mc_top_rim0
	WriteRegister(MC_MC_50_reg,0,11, u32Top);
	//reg_mc_bot_rim0
	WriteRegister(MC_MC_50_reg,16,27, u32Bottom);
	//reg_mc_lft_rim0
	WriteRegister(MC_MC_54_reg,0,11, u32Left);
	//reg_mc_rht_rim0
	WriteRegister(MC_MC_54_reg,16,27, u32Right);	
#endif
}

/**
 * @brief This function set mc rim 1
 * @param [in] unsigned int u32Top
 * @param [in] unsigned int u32Bottom
 * @param [in] unsigned int u32Left
 * @param [in] unsigned int u32Right
 * @retval VOID
*/
VOID HAL_MC_SetMCRim1(unsigned int u32Top, unsigned int u32Bottom, unsigned int u32Left, unsigned int u32Right)
{

#if RTK_MEMC_Performance_tunging_from_tv001
	mc_mc_58_RBUS mc_mc_58;
	mc_mc_5c_RBUS mc_mc_5C;
	mc_mc_58.regValue = rtd_inl(MC_MC_58_reg);
	mc_mc_5C.regValue = rtd_inl(MC_MC_5C_reg);

	mc_mc_58.mc_top_rim1 = u32Top;
	mc_mc_58.mc_bot_rim1 = u32Bottom;
	mc_mc_5C.mc_lft_rim1 = u32Left;
	mc_mc_5C.mc_rht_rim1 = u32Right;

	rtd_outl(MC_MC_58_reg, mc_mc_58.regValue);
	rtd_outl(MC_MC_5C_reg, mc_mc_5C.regValue);
#if 0
	//reg_mc_top_rim1
	WriteRegister(MC_MC_58_reg,0,12, u32Top);
	//reg_mc_bot_rim1
	WriteRegister(MC_MC_58_reg,16,28, u32Bottom);
	//reg_mc_lft_rim1
	WriteRegister(MC_MC_5C_reg,0,12, u32Left);
	//reg_mc_rht_rim1
	WriteRegister(MC_MC_5C_reg,16,28, u32Right);
#endif
#else
	//reg_mc_top_rim1
	WriteRegister(MC_MC_58_reg,0,11, u32Top);
	//reg_mc_bot_rim1
	WriteRegister(MC_MC_58_reg,16,27, u32Bottom);
	//reg_mc_lft_rim1
	WriteRegister(MC_MC_5C_reg,0,11, u32Left);
	//reg_mc_rht_rim1
	WriteRegister(MC_MC_5C_reg,16,27, u32Right);
#endif
}
#endif//REMOVE_REDUNDANT_API

/**
 * @brief This function get the mc var lp enable
 * @param [in] VOID
 * @retval unsigned int u32En
*/
unsigned int HAL_MC_GetVarLPFEn_RTK2885p(VOID)
{
	unsigned int u32En = 0;
	mc2_mc2_50_RBUS mc2_mc2_50_reg;
	mc2_mc2_50_reg.regValue = rtd_inl(MC2_MC2_50_reg);
	u32En = mc2_mc2_50_reg.mc_var_lpf_en;
	return u32En;
}
unsigned int HAL_MC_GetVarLPFEn_RTK2885pp(VOID)
{
	unsigned int u32En = 0;
	M8P_rtmc2_rtmc2_20_RBUS M8P_rtmc2_rtmc2_20;
	M8P_rtmc2_rtmc2_20.regValue = rtd_inl(M8P_RTMC2_RTMC2_20_reg);
	u32En = M8P_rtmc2_rtmc2_20.mc_var_lpf_en;
	return u32En;
}
unsigned int HAL_MC_GetVarLPFEn(VOID)
{
	unsigned int u32_Enable = 0;

	if(RUN_MERLIN8P()){
		u32_Enable = HAL_MC_GetVarLPFEn_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_Enable = HAL_MC_GetVarLPFEn_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_Enable;
}

/**
 * @brief This function set mc var lp enable
 * @param [in] unsigned int u32En
 * @retval VOID
*/
VOID HAL_MC_SetVarLPFEn_RTK2885p(unsigned int u32En)
{
	mc2_mc2_50_RBUS mc2_mc2_50_reg;
	//mc2_mc2_54_RBUS mc2_mc2_54_reg;
	mc2_mc2_58_RBUS mc2_mc2_58_reg;
	mc2_mc2_98_RBUS mc2_mc2_98_reg;
	mc_mc_08_RBUS mc_mc_08_reg;
	mc_mc_0c_RBUS mc_mc_0c_reg;
	//bool u1_dehalo = (((rtd_inl(KPOST_TOP_KPOST_TOP_A0_reg)>>28) & 0x1) == 0) ? 1 : 0;
	bool u1_dehalo = 0;

	unsigned int new_dehalo_dynamic = 0;
	ReadRegister(SOFTWARE2_SOFTWARE2_25_reg,30,30, &new_dehalo_dynamic);//default should be 0

		
	u1_dehalo = (((rtd_inl(KPOST_TOP_KPOST_TOP_A0_reg)>>28) & 0x1) == 0) ? 1 : 0;

	mc2_mc2_50_reg.regValue = rtd_inl(MC2_MC2_50_reg);
	//mc2_mc2_54_reg.regValue = rtd_inl(MC2_MC2_54_reg);
	mc2_mc2_58_reg.regValue = rtd_inl(MC2_MC2_58_reg);
	mc2_mc2_98_reg.regValue = rtd_inl(MC2_MC2_98_reg);
	mc_mc_08_reg.regValue = rtd_inl(MC_MC_08_reg);
	mc_mc_0c_reg.regValue = rtd_inl(MC_MC_0C_reg);

	//mc2_mc2_50_reg.mc_var_lpf_en = 3;
	mc2_mc2_50_reg.mc_var_lpf_en = 3;//3:Y and C LPF enable, 2: C_LPF only, 1:Y_LPF only, 0: Diable LPF
	//change note : deflicker use the same en to control the LPF of deflicker. So it need to control the slop/y_min/y_max to control the LPF on/off

	if(u32En==0){
		mc2_mc2_98_reg.mc_logo_5x5_cnt_th0 = 0x1a;
		mc2_mc2_98_reg.mc_logo_5x5_cnt_th1 = 0x1a;
	
		mc2_mc2_50_reg.mc_var_dbpt_lvl = 0;
		//mc2_mc2_54_reg.mc_var_mvd_y_min = 0;
		//mc2_mc2_54_reg.mc_var_mvd_y_max = 0;
		mc_mc_08_reg.mc_var_lfb_min = 0;
		mc_mc_08_reg.mc_var_lfb_max = 0;
		if(u1_dehalo){
		/*	mc_mc_0c_reg.mc_lvllpf_lpf_en = 0;
			mc_mc_0c_reg.mc_lvllpf_coef0 = 0;
			mc_mc_0c_reg.mc_lvllpf_coef1 = 0;
			mc_mc_0c_reg.mc_lvllpf_coef2 = 0;

			mc2_mc2_58_reg.mc_var_fir_slope = 8;
			mc2_mc2_58_reg.mc_var_fir_y_min = 8;
			mc2_mc2_58_reg.mc_var_fir_y_max = 15;
	*/
		
				mc_mc_0c_reg.mc_lvllpf_lpf_en = 1;
				mc_mc_0c_reg.mc_lvllpf_coef0 = 16;
				mc_mc_0c_reg.mc_lvllpf_coef1 = 16;
				mc_mc_0c_reg.mc_lvllpf_coef2 = 8;
			
			if(new_dehalo_dynamic == 0){
				mc2_mc2_58_reg.mc_var_fir_slope = 0x9;
				mc2_mc2_58_reg.mc_var_fir_y_min = 0xb;
				mc2_mc2_58_reg.mc_var_fir_y_max = 0xf;
			}
		}else{
			/*mc_mc_0c_reg.mc_lvllpf_lpf_en = 1;
			mc_mc_0c_reg.mc_lvllpf_coef0 = 0;
			mc_mc_0c_reg.mc_lvllpf_coef1 = 0;
			mc_mc_0c_reg.mc_lvllpf_coef2 = 0;

			mc2_mc2_58_reg.mc_var_fir_slope = 4;
			mc2_mc2_58_reg.mc_var_fir_y_min = 3;
			mc2_mc2_58_reg.mc_var_fir_y_max = 15;*/
				mc_mc_0c_reg.mc_lvllpf_lpf_en = 1;
				mc_mc_0c_reg.mc_lvllpf_coef0 = 0;
				mc_mc_0c_reg.mc_lvllpf_coef1 = 0;
				mc_mc_0c_reg.mc_lvllpf_coef2 = 0;
			
			if(new_dehalo_dynamic == 0){
				mc2_mc2_58_reg.mc_var_fir_slope = 0x9;
				mc2_mc2_58_reg.mc_var_fir_y_min = 0xb;
				mc2_mc2_58_reg.mc_var_fir_y_max = 0xf;
			}
		}
	}else{
		mc2_mc2_98_reg.mc_logo_5x5_cnt_th0 = 0x19;
		mc2_mc2_98_reg.mc_logo_5x5_cnt_th1 = 0x19;

		mc2_mc2_50_reg.mc_var_dbpt_lvl = 64;
		//mc2_mc2_54_reg.mc_var_mvd_y_min = 0;
		//mc2_mc2_54_reg.mc_var_mvd_y_max = 127;
		mc_mc_08_reg.mc_var_lfb_min = 0;
		mc_mc_08_reg.mc_var_lfb_max = 96;

		/*mc_mc_0c_reg.mc_lvllpf_lpf_en = 0;
		mc_mc_0c_reg.mc_lvllpf_coef0 = 0;
		mc_mc_0c_reg.mc_lvllpf_coef1 = 0;
		mc_mc_0c_reg.mc_lvllpf_coef2 = 0;

		mc2_mc2_58_reg.mc_var_fir_slope = 8;
		mc2_mc2_58_reg.mc_var_fir_y_min = 8;
		mc2_mc2_58_reg.mc_var_fir_y_max = 15;
		*/
			mc_mc_0c_reg.mc_lvllpf_lpf_en = 1;
			mc_mc_0c_reg.mc_lvllpf_coef0 = 16;
			mc_mc_0c_reg.mc_lvllpf_coef1 = 16;
			mc_mc_0c_reg.mc_lvllpf_coef2 = 8;
		if(new_dehalo_dynamic == 0){
			mc2_mc2_58_reg.mc_var_fir_slope = 0x9;
			mc2_mc2_58_reg.mc_var_fir_y_min = 0xb;
			mc2_mc2_58_reg.mc_var_fir_y_max = 0xf;
		}
	}

	rtd_outl(MC2_MC2_50_reg, mc2_mc2_50_reg.regValue);
	//rtd_outl(MC2_MC2_54_reg, mc2_mc2_54_reg.regValue);
	rtd_outl(MC2_MC2_58_reg, mc2_mc2_58_reg.regValue);
	rtd_outl(MC2_MC2_98_reg, mc2_mc2_98_reg.regValue);
	rtd_outl(MC_MC_08_reg, mc_mc_08_reg.regValue);
	//rtd_outl(MC_MC_0C_reg, mc_mc_0c_reg.regValue);
	if(new_dehalo_dynamic == 0){	
			rtd_outl(MC_MC_0C_reg, mc_mc_0c_reg.regValue);
	}	

}
VOID HAL_MC_SetVarLPFEn_RTK2885pp(unsigned int u32En)
{
	M8P_rtmc2_rtmc2_20_RBUS M8P_rtmc2_rtmc2_20;
	M8P_rtmc2_rtmc2_20.regValue = rtd_inl(M8P_RTMC2_RTMC2_20_reg);
	M8P_rtmc2_rtmc2_20.mc_var_lpf_en = u32En;
	rtd_outl(M8P_RTMC2_RTMC2_20_reg, M8P_rtmc2_rtmc2_20.regValue);
}
VOID HAL_MC_SetVarLPFEn(unsigned int u32En)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetVarLPFEn_RTK2885pp(u32En);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetVarLPFEn_RTK2885p(u32En);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set mc var lp 17 tap enable
 * @param [in] unsigned int BOOL
 * @retval VOID
*/
extern VOID MEMC_Lib_Set_VarLpf_Tap_RTK2885p(unsigned char u2_Tap);
/*VOID HAL_MC_SetVar17TapEn(BOOL bEnable)
{

#if RTK_MEMC_Performance_tunging_from_tv001
	//reg_mc_var17tap_en
	if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_2K){
		//WriteRegister(MC2_MC2_68_reg,18,19, 0x0);  //vartap_sel
		MEMC_Lib_Set_VarLpf_Tap_RTK2885p(0); // 9tap
	}
	else if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_3K){
		WriteRegister(MC2_MC2_68_reg,18,19, 0x1);  //vartap_sel
	}
	else {
		//WriteRegister(MC2_MC2_68_reg,19,19,(unsigned int)bEnable);
		if(bEnable){
			MEMC_Lib_Set_VarLpf_Tap_RTK2885p(2); // 33tap
		}else{
			MEMC_Lib_Set_VarLpf_Tap_RTK2885p(1); // 17tap
		}
	}
#else
		//reg_mc_var17tap_en
	if(Get_DISPLAY_PANEL_TYPE() == P_LVDS_2K1K){
		//WriteRegister(MC2_MC2_68_reg,18,19, 0x0);  //vartap_sel
		MEMC_Lib_Set_VarLpf_Tap_RTK2885p(0); // 9tap
	}
	else {
		//WriteRegister(MC2_MC2_68_reg,19,19,(unsigned int)bEnable);
		if(bEnable){
			MEMC_Lib_Set_VarLpf_Tap_RTK2885p(2); // 33tap
		}else{
			MEMC_Lib_Set_VarLpf_Tap_RTK2885p(1); // 17tap
		}
	}
#endif

}*/

VOID HAL_MC_Set_VarLPFTapSel_RTK2885p(unsigned char VarLPFTapSel)//0:9tap;1:17tap;2:33tap
{
	mc2_mc2_68_RBUS mc2_mc2_68_reg;
	unsigned int new_dehalo_dynamic = 0;
	ReadRegister(SOFTWARE2_SOFTWARE2_25_reg,30,30, &new_dehalo_dynamic);
	mc2_mc2_68_reg.regValue = rtd_inl(MC2_MC2_68_reg);

	//reg_mc_var17tap_en
	if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_2K){
		MEMC_Lib_Set_VarLpf_Tap_RTK2885p(0); // 9tap
	}
	else if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_3K){
		MEMC_Lib_Set_VarLpf_Tap_RTK2885p(1); // 17tap
	}
	else {
		//WriteRegister(MC2_MC2_68_reg,19,19,(unsigned int)bEnable);
		 if(new_dehalo_dynamic ==0){
			MEMC_Lib_Set_VarLpf_Tap_RTK2885p(VarLPFTapSel);


		}else{
			MEMC_Lib_Set_VarLpf_Tap_RTK2885p(2);

			
		}
	}
}
VOID HAL_MC_Set_VarLPFTapSel_RTK2885pp(unsigned char VarLPFTapSel)//0:9x5, 1:17x5, 2:33x5_1, 3s:33x5_2
{
	M8P_rtmc2_rtmc2_24_RBUS M8P_rtmc2_rtmc2_24;
	VarLPFTapSel = (VarLPFTapSel<VAR_LPF_NUM) ? VarLPFTapSel : VAR_LPF_33x5_1;
	M8P_rtmc2_rtmc2_24.regValue = rtd_inl(M8P_RTMC2_RTMC2_24_reg);
	M8P_rtmc2_rtmc2_24.mc_var_lpf_sample_mode = VarLPFTapSel;
	rtd_outl(M8P_RTMC2_RTMC2_24_reg, M8P_rtmc2_rtmc2_24.regValue);
}
VOID HAL_MC_Set_VarLPFTapSel(unsigned char VarLPFTapSel)
{
	if(RUN_MERLIN8P()){
		HAL_MC_Set_VarLPFTapSel_RTK2885pp(VarLPFTapSel);
	}else if(RUN_MERLIN8()){
		HAL_MC_Set_VarLPFTapSel_RTK2885p(VarLPFTapSel);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if REMOVE_REDUNDANT_API
/**
 * @brief This function force  local fallback off by force the GFB = 0
 * @param [in] unsigned int u32Mode
 * @retval VOID
*/
VOID HAL_MC_SetPRMode(unsigned int u32Mode)
{
	//reg_mc_v_l_r
	WriteRegister(MC_MC_30_reg,3,3,u32Mode);
}
#endif//REMOVE_REDUNDANT_API

/**
 * @brief This function set mc poly enable
 * @param [in] unsigned int u32Val
 * @retval VOID
*/
VOID HAL_MC_SetPolyEnable_RTK2885p(unsigned int u32Val)
{
	WriteRegister(MC_MC_40_reg,0,1,u32Val);
	WriteRegister(MC_MC_40_reg,6,7,u32Val);
}
VOID HAL_MC_SetPolyEnable_RTK2885pp(unsigned int u32Val)
{
	M8P_rtmc_rtmc_04_RBUS M8P_rtmc_rtmc_04;
	M8P_rtmc_rtmc_04.regValue = rtd_inl(M8P_RTMC_RTMC_04_reg);
	M8P_rtmc_rtmc_04.mc_poly_intp_en = u32Val;
	rtd_outl(M8P_RTMC_RTMC_04_reg, M8P_rtmc_rtmc_04.regValue);
}
VOID HAL_MC_SetPolyEnable(unsigned int u32Val)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetPolyEnable_RTK2885pp(u32Val);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetPolyEnable_RTK2885p(u32Val);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if REMOVE_REDUNDANT_API
#if 1
/**
 * @brief This function set block logo rim
 * @param [in] unsigned int u32Top
 * @param [in] unsigned int u32Bottom
 * @param [in] unsigned int u32Left
 * @param [in] unsigned int u32Right
 * @retval VOID
*/
VOID HAL_KMC_LOGO_Set_BlockRim(unsigned int u32Top, unsigned int u32Bottom, unsigned int u32Left, unsigned int u32Right)
{
	WriteRegister(MC2_MC2_9C_reg,20,29, u32Top);	
	WriteRegister(MC2_MC2_A0_reg,0,9, u32Bottom);	
	WriteRegister(MC2_MC2_9C_reg,0,9, u32Left);	
	WriteRegister(MC2_MC2_9C_reg,10,19, u32Right);	
}

/**
 * @brief This function set pixel pixel rim
 * @param [in] unsigned int u32Top
 * @param [in] unsigned int u32Bottom
 * @param [in] unsigned int u32Left
 * @param [in] unsigned int u32Right
 * @retval VOID
*/
VOID HAL_KMC_LOGO_Set_PixelRim(unsigned int u32Top, unsigned int u32Bottom, unsigned int u32Left, unsigned int u32Right)
{
	WriteRegister(MC2_MC2_A4_reg,20,29, u32Top);	
	WriteRegister(MC2_MC2_A8_reg,0,9, u32Bottom);	
	WriteRegister(MC2_MC2_A4_reg,0,9, u32Left);	
	WriteRegister(MC2_MC2_A4_reg,10,19, u32Right);	
}

/**
 * @brief This function set mc block logo htotal
 * @param [in] unsigned int u32Val
 * @retval VOID
*/
VOID HAL_MC_SetMCBLKLogoHtotal(unsigned int u32Val)
{
	WriteRegister(MC_MC_14_reg,0,13,u32Val);
}

/**
 * @brief This function set mc block logo hact
 * @param [in] unsigned int u32Val
 * @retval VOID
*/
VOID HAL_MC_SetMCBLKLogoHact(unsigned int u32Val)
{
	WriteRegister(MC_MC_14_reg,14,27,u32Val);
}

/**
 * @brief This function set mc block logo row num
 * @param [in] unsigned int u32Val
 * @retval VOID
*/
VOID HAL_MC_SetMCBLKLogoRowNum(unsigned int u32Val)
{
	WriteRegister(MC_MC_18_reg,8,15,u32Val);
}

/**
 * @brief This function set mc block logo row type
 * @param [in] unsigned int u32Val
 * @retval VOID
*/
VOID HAL_MC_SetMCBLKLogoRowType(unsigned int u32Val)
{
	WriteRegister(MC_MC_18_reg,6,7,u32Val);
}

/**
 * @brief This function set lbmc share hf enable
 * @param [in] unsigned int BOOL
 * @retval VOID
*/
VOID HAL_MC_SetLBMCShareHFEn(BOOL bEnable)
{
	WriteRegister(LBMC_LBMC_88_reg,13,13,(unsigned int)bEnable);
	WriteRegister(LBMC_LBMC_88_reg,22,22,(unsigned int)bEnable);
}
#endif
#endif//REMOVE_REDUNDANT_API

unsigned int HAL_MC_GetMC04_06_total_data_cnt(VOID)
{
	#if (IC_K4LP || IC_K3LP)
	unsigned int u32InINT_Sts = 0;
	ReadRegister(MC_DMA_MC_LF_P_DMA_RD_Ctrl_reg,0,21, &u32InINT_Sts);
	return u32InINT_Sts;
	#else
	return FALSE;
	#endif
}

/**
 * @brief This function get the enable of local fallback
 * @param [in] VOID
 * @retval BOOL bEnable
*/
BOOL HAL_MC_GetForceLFBEnable_RTK2885p(VOID)
{
	unsigned int bEnable = 0;
	ReadRegister(MC_MC_28_reg,14,14, &bEnable);
	return (BOOL)bEnable;
}
BOOL HAL_MC_GetForceLFBEnable_RTK2885pp(VOID)
{
	unsigned int bEnable = 0;
	M8P_rtmc_rtmc_70_RBUS M8P_rtmc_rtmc_70;
	M8P_rtmc_rtmc_70.regValue = rtd_inl(M8P_RTMC_RTMC_70_reg);
	bEnable = M8P_rtmc_rtmc_70.mc_lfb_lvl_force_en;
	return (BOOL)bEnable;
}
BOOL HAL_MC_GetForceLFBEnable(VOID)
{
	BOOL u1_Enable = false;

	if(RUN_MERLIN8P()){
		u1_Enable = HAL_MC_GetForceLFBEnable_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u1_Enable = HAL_MC_GetForceLFBEnable_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u1_Enable;
}

/**
 * @brief This function enable  local fallback
 * @param [in] BOOL bEnalbe
 * @retval VOID
*/
VOID HAL_MC_SetLFBEnable_RTK2885p(BOOL u1_force_en, unsigned char u8_Force_value)
{
	WriteRegister(MC_MC_28_reg,14,14, u1_force_en);
	WriteRegister(MC_MC_28_reg,15,22, u8_Force_value);
}
VOID HAL_MC_SetLFBEnable_RTK2885pp(BOOL u1_force_en, unsigned char u8_Force_value)
{
	M8P_rtmc_rtmc_70_RBUS M8P_rtmc_rtmc_70;
	M8P_rtmc_rtmc_70.regValue = rtd_inl(M8P_RTMC_RTMC_70_reg);
	M8P_rtmc_rtmc_70.mc_lfb_lvl_force_en = u1_force_en;
	M8P_rtmc_rtmc_70.mc_lfb_lvl_force_value = u8_Force_value;
	rtd_outl(M8P_RTMC_RTMC_70_reg, M8P_rtmc_rtmc_70.regValue);	
}
VOID HAL_MC_SetLFBEnable(BOOL u1_force_en, unsigned char u8_Force_value)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetLFBEnable_RTK2885pp(u1_force_en, u8_Force_value);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetLFBEnable_RTK2885p(u1_force_en, u8_Force_value);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set MC fb blend mode
 * @param [in] unsigned char gfb_type
 * @param [in] unsigned char lfb_type
 * @retval VOID
*/
VOID HAL_MC_Set_FB_BldMode_RTK2885p(unsigned char fb_type)
{
	WriteRegister(MC_MC_28_reg,11, 13, fb_type);	//0:iz, 1:pz, 2:zz_avg, 3:zz_bld, 4: fb_data = (phase>63) ? pz : iz, 5~:zz_median3
}
VOID HAL_MC_Set_FB_BldMode_RTK2885pp(unsigned char gfb_type, unsigned char lfb_type)
{
	M8P_rtmc_rtmc_74_RBUS M8P_rtmc_rtmc_74;
	M8P_rtmc_rtmc_74.regValue = rtd_inl(M8P_RTMC_RTMC_74_reg);
	M8P_rtmc_rtmc_74.mc_bld_lfb_z_type = lfb_type; //0:iz, 1:pz, 2:zz_avg, 3:zz_bld, 4: fb_data = (phase>63) ? pz : iz, 5~:zz_median3
	M8P_rtmc_rtmc_74.mc_bld_gfb_z_type = gfb_type; //0:iz, 1:pz, 2:zz_avg, 3:zz_bld, 4: fb_data = (phase>63) ? pz : iz, 5~:zz_median3
	rtd_outl(M8P_RTMC_RTMC_74_reg, M8P_rtmc_rtmc_74.regValue);
}
VOID HAL_MC_Set_FB_BldMode(unsigned char gfb_type, unsigned char lfb_type)
{
	if(RUN_MERLIN8P()){
		HAL_MC_Set_FB_BldMode_RTK2885pp(gfb_type, lfb_type);
	}else if(RUN_MERLIN8()){
		HAL_MC_Set_FB_BldMode_RTK2885p(gfb_type);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set lbmcpc_mode_sel_p
 * @param [in] unsigned char type
 * @retval VOID
*/
VOID HAL_LBMCPC_Mode_Sel_RTK2885p(unsigned char bType)
{
	WriteRegister(LBMC_LBMC_24_reg, 13, 13, bType);
}
VOID HAL_LBMCPC_Mode_Sel_RTK2885pp(unsigned char bType)
{
	M8P_rtmc_lbmc_lbmc_ctrl_RBUS M8P_rtmc_lbmc_lbmc_ctrl_reg;
	M8P_rtmc_lbmc_lbmc_ctrl_reg.regValue = rtd_inl(M8P_RTMC_LBMC_LBMC_CTRL_reg);
	M8P_rtmc_lbmc_lbmc_ctrl_reg.lbmcpc_mode_sel_p = bType;
	rtd_outl(M8P_RTMC_LBMC_LBMC_CTRL_reg, M8P_rtmc_lbmc_lbmc_ctrl_reg.regValue);
}
VOID HAL_LBMCPC_Mode_Sel(unsigned char bType)
{
	bType = (bType==0) ? 0 : 1;

	if(RUN_MERLIN8P()){
		HAL_LBMCPC_Mode_Sel_RTK2885pp(bType);
	}else if(RUN_MERLIN8()){
		HAL_LBMCPC_Mode_Sel_RTK2885p(bType);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get the kphase lut select
 * @param [in] VOID
 * @retval kphase_lut_sel
*/
unsigned int HAL_MC_GetKphaseLutSel_RTK2885p(VOID)
{
	unsigned int kphase_lut_sel;
	ReadRegister(KPHASE_kphase_10_reg,16,16, &kphase_lut_sel);
	return kphase_lut_sel;
}
unsigned int HAL_MC_GetKphaseLutSel_RTK2885pp(VOID)
{
	M8P_rthw_phase_kphase_10_RBUS M8P_rthw_phase_kphase_10;
	unsigned int kphase_lut_sel;
	
	M8P_rthw_phase_kphase_10.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_10_reg);
	kphase_lut_sel = M8P_rthw_phase_kphase_10.kphase_lut_sel;
	return kphase_lut_sel;
}
unsigned int HAL_MC_GetKphaseLutSel(VOID)
{
	unsigned int u32_KphaseLutSel = 0;

	if(RUN_MERLIN8P()){
		u32_KphaseLutSel = HAL_MC_GetKphaseLutSel_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_KphaseLutSel = HAL_MC_GetKphaseLutSel_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_KphaseLutSel;
}

/**
 * @brief This function set the kphase lut select
 * @param [in] unsigned int kphase_lut_sel
 * @retval VOID
*/
VOID HAL_MC_SetKphaseLutSel_RTK2885p(unsigned int kphase_lut_sel)
{
	WriteRegister(KPHASE_kphase_10_reg,16,16, kphase_lut_sel);
}
VOID HAL_MC_SetKphaseLutSel_RTK2885pp(unsigned int kphase_lut_sel)
{
	M8P_rthw_phase_kphase_10_RBUS M8P_rthw_phase_kphase_10;
	M8P_rthw_phase_kphase_10.regValue = rtd_inl(M8P_RTHW_PHASE_kphase_10_reg);
	M8P_rthw_phase_kphase_10.kphase_lut_sel = kphase_lut_sel;
	rtd_outl(M8P_RTHW_PHASE_kphase_10_reg, M8P_rthw_phase_kphase_10.regValue);
}
VOID HAL_MC_SetKphaseLutSel(unsigned int kphase_lut_sel)
{
	if(RUN_MERLIN8P()){
		HAL_MC_SetKphaseLutSel_RTK2885pp(kphase_lut_sel);
	}else if(RUN_MERLIN8()){
		HAL_MC_SetKphaseLutSel_RTK2885p(kphase_lut_sel);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get the lbmc repeat mode(lbmcpc_mode)
 * @param [in] VOID
 * @retval repeat_mode
*/
unsigned int HAL_MC_GetRepeatMode_RTK2885p(VOID)
{
	unsigned int repeat_mode;
	ReadRegister(LBMC_LBMC_24_reg, 4, 4, &repeat_mode);
	return repeat_mode;
}
unsigned int HAL_MC_GetRepeatMode_RTK2885pp(VOID)
{
	unsigned int repeat_mode;
	M8P_rtmc_lbmc_lbmc_ctrl_RBUS M8P_rtmc_lbmc_lbmc_ctrl;
	M8P_rtmc_lbmc_lbmc_ctrl.regValue = rtd_inl(M8P_RTMC_LBMC_LBMC_CTRL_reg);
	repeat_mode = M8P_rtmc_lbmc_lbmc_ctrl.lbmcpc_mode;
	return repeat_mode;
}
unsigned int HAL_MC_GetRepeatMode(VOID)
{
	unsigned int u32_RepeatMode = 0;

	if(RUN_MERLIN8P()){
		u32_RepeatMode = HAL_MC_GetRepeatMode_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_RepeatMode = HAL_MC_GetRepeatMode_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_RepeatMode;
}

/**
 * @brief This function get the lbmc pz sel(lbmcpc_mode_sel_p)
 * @param [in] VOID
 * @retval pz_sel
*/
unsigned int HAL_MC_GetPZSel_RTK2885p(VOID)
{
	unsigned int pz_sel;
	ReadRegister(LBMC_LBMC_24_reg, 13, 13, &pz_sel);
	return pz_sel;
}
unsigned int HAL_MC_GetPZSel_RTK2885pp(VOID)
{
	unsigned int pz_sel;
	M8P_rtmc_lbmc_lbmc_ctrl_RBUS M8P_rtmc_lbmc_lbmc_ctrl;
	M8P_rtmc_lbmc_lbmc_ctrl.regValue = rtd_inl(M8P_RTMC_LBMC_LBMC_CTRL_reg);
	pz_sel = M8P_rtmc_lbmc_lbmc_ctrl.lbmcpc_mode_sel_p;
	return pz_sel;
}
unsigned int HAL_MC_GetPZSel(VOID)
{
	unsigned int u32_PZ_sel = 0;

	if(RUN_MERLIN8P()){
		u32_PZ_sel = HAL_MC_GetPZSel_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_PZ_sel = HAL_MC_GetPZSel_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_PZ_sel;
}

/**
 * @brief This function get the mc LF I DMA enable
 * @param [in] VOID
 * @retval dma_enable
*/
unsigned int HAL_MC_GetMC_LF_I_DMA_Enable_RTK2885p(VOID)
{
	unsigned int dma_enable = rtd_inl(MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg)&0x1;

	return dma_enable;
}
unsigned int HAL_MC_GetMC_LF_I_DMA_Enable_RTK2885pp(VOID)
{
	unsigned int dma_enable;
	M8P_rthw_mc_dma_mc_lf_i_dma_rd_ctrl_RBUS M8P_rthw_mc_dma_mc_lf_i_dma_rd_ctrl;
	M8P_rthw_mc_dma_mc_lf_i_dma_rd_ctrl.regValue = rtd_inl(M8P_RTHW_MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg);
	dma_enable = M8P_rthw_mc_dma_mc_lf_i_dma_rd_ctrl.dma_enable;
	return dma_enable;
}
unsigned int HAL_MC_GetMC_LF_I_DMA_Enable(VOID)
{
	unsigned int u32_Enbale = false;

	if(RUN_MERLIN8P()){
		u32_Enbale = HAL_MC_GetMC_LF_I_DMA_Enable_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_Enbale = HAL_MC_GetMC_LF_I_DMA_Enable_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_Enbale;
}

/**
 * @brief This function get the mc LF I DMA enable
 * @param [in] VOID
 * @retval u32_En
*/
unsigned int HAL_MC_GetMCLogoVLpfEn_RTK2885p(VOID)
{
	unsigned int u32_En = 0;
	ReadRegister(MC2_MC2_98_reg, 8, 8, &u32_En);
	return u32_En;
}
unsigned int HAL_MC_GetMCLogoVLpfEn_RTK2885pp(VOID)
{
	unsigned int u32_En = 0;
	M8P_rtmc2_rtmc2_04_RBUS M8P_rtmc2_rtmc2_04;

	M8P_rtmc2_rtmc2_04.regValue = rtd_inl(M8P_RTMC2_RTMC2_04_reg);
	u32_En = M8P_rtmc2_rtmc2_04.mc_var_lpf_coef_logo_en;
	return u32_En;
}
unsigned int HAL_MC_GetMCLogoVLpfEn_Enable(VOID)
{
	unsigned int u32_En = 0;

	if(RUN_MERLIN8P()){
		u32_En = HAL_MC_GetMCLogoVLpfEn_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32_En = HAL_MC_GetMCLogoVLpfEn_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_En;
}

/**
 * @brief This function get the mc phase
 * @param [in] VOID
 * @retval mc_phase
*/
unsigned int HAL_MC_GetMCPhase_RTK2885p(VOID)
{
	unsigned int mc_phase = 0;
	ReadRegister(MC_MC_F8_reg, 0, 6, &mc_phase);
	return mc_phase;
}
unsigned int HAL_MC_GetMCPhase_RTK2885pp(VOID)
{
	unsigned int mc_phase = 0;
	M8P_rthw_mc_top_mc_status_RBUS M8P_rthw_mc_top_mc_status;
	M8P_rthw_mc_top_mc_status.regValue = rtd_inl(M8P_RTHW_MC_TOP_MC_STATUS_reg);
	mc_phase = M8P_rthw_mc_top_mc_status.regr_mc_phase;
	return mc_phase;
}
unsigned int HAL_MC_GetMCPhase(VOID)
{
	unsigned int mc_phase = 0;

	if(RUN_MERLIN8P()){
		mc_phase = HAL_MC_GetMCPhase_RTK2885pp();
	}else if(RUN_MERLIN8()){
		mc_phase = HAL_MC_GetMCPhase_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return mc_phase;
}

VOID HAL_PQL_TOP_SetInputIntp_en(BOOL bEnalbe)
{
	WriteRegister(HARDWARE_HARDWARE_63_reg,30,30, (unsigned int)bEnalbe);
}

VOID HAL_PQL_TOP_SetOnefiftIntp_en(BOOL bEnalbe)
{
	WriteRegister(HARDWARE_HARDWARE_63_reg,29,29, (unsigned int)bEnalbe);
}

VOID HAL_PQL_TOP_SetOutputIntp_en(BOOL bEnalbe)
{
	WriteRegister(HARDWARE_HARDWARE_63_reg,31,31, (unsigned int)bEnalbe);
}



