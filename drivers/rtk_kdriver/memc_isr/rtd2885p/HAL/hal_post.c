/**
 * @file hal_post.c
 * @brief This file is for post register setting
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
#include "memc_reg_def.h"
#ifndef BUILD_QUICK_SHOW
#include "linux/spinlock.h"
#else
#include <sysdefs.h>
#include <mach/rtk_platform.h>
#include <no_os/printk.h>
#include <no_os/semaphore.h>
#include <no_os/spinlock.h>
#include <no_os/spinlock_types.h>
#include <no_os/slab.h>
#include <timer.h>
#include <no_os/pageremap.h>
#endif
#include "memc_isr/PQL/PQLPlatformDefs.h"

#ifndef BUILD_QUICK_SHOW
static DEFINE_SPINLOCK(memc_task_Spinlock);/*Spin lock no context switch. This is for copy paramter*/
/**
 * @brief This function set post pattern size
 * @param [in] TIMING_PARA_ST stOutputTiming
 * @retval VOID
*/
#endif

#if REMOVE_REDUNDANT_API
VOID HAL_POST_SetPatternSize(TIMING_PARA_ST stOutputTiming)
{
	//reg_post_patt_htotal
	WriteRegister(KPOST_TOP_KPOST_TOP_10_reg,0,15,stOutputTiming.u32HTotal/4);
	//reg_post_patt_vtotal
	WriteRegister(KPOST_TOP_KPOST_TOP_10_reg,16,31,stOutputTiming.u32VTotal);
	//reg_post_patt_hact
	WriteRegister(KPOST_TOP_KPOST_TOP_14_reg,0,15,stOutputTiming.u32HActive);
	//reg_post_patt_vact
	WriteRegister(KPOST_TOP_KPOST_TOP_14_reg,16,31,stOutputTiming.u32VActive);
}

/**
 * @brief This function set post pr mode enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_POST_SetPREnable(BOOL bEnable)
{
	//reg_pr_mode_en
	WriteRegister(KPOST_TOP_KPOST_TOP_28_reg,0,0,(unsigned int)bEnable);
}

/**
 * @brief This function set post hde center value
 * @param [in] unsigned int u32CenterValue
 * @retval VOID
*/
VOID HAL_POST_SetHdeCenterValue(unsigned int u32CenterValue)
{
	//reg_hde_center_value
	WriteRegister(KPOST_TOP_KPOST_TOP_28_reg,1,12,u32CenterValue);
}

/**
 * @brief This function set post 4 port hde center value
 * @param [in] unsigned int u32CenterValue
 * @retval VOID
*/
VOID HAL_POST_Set4PortHdeCenterValue(unsigned int u32CenterValue)
{
	//reg_4port_hde_center_value
//	WriteRegister(FRC_TOP__KPOST_TOP__4port_hde_center_value_ADDR no mat,FRC_TOP__KPOST_TOP__4port_hde_center_value_BITSTART no mat,FRC_TOP__KPOST_TOP__4port_hde_center_value_BITEND no mat,u32CenterValue);
}

/**
 * @brief This function set v scaler bypass
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_POST_SetVscalerBypass(BOOL bEnable)
{
	//reg_post_vsc_bypass
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_vsc_bypass_ADDR no mat,FRC_TOP__KPOST_TOP__post_vsc_bypass_BITSTART no mat,FRC_TOP__KPOST_TOP__post_vsc_bypass_BITEND no mat,(unsigned int)bEnable);
}

/**
 * @brief This function set v scaler mode
 * @param [in] unsigned int u32Mode
 * @retval VOID
*/
VOID HAL_POST_SetVscalerMode(unsigned int u32Mode)
{
	//reg_post_vsc_mode
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_vsc_mode_ADDR no mat,FRC_TOP__KPOST_TOP__post_vsc_mode_BITSTART no mat,FRC_TOP__KPOST_TOP__post_vsc_mode_BITEND no mat,u32Mode);
}

/**
 * @brief This function set v active
 * @param [in] unsigned int u32Vactive
 * @retval VOID
*/
VOID HAL_POST_SetVActive(unsigned int u32Vactive)
{
	//reg_post_v_act
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_v_act_ADDR no mat,FRC_TOP__KPOST_TOP__post_v_act_BITSTART no mat,FRC_TOP__KPOST_TOP__post_v_act_BITEND no mat,u32Vactive);
}
#endif//REMOVE_REDUNDANT_API

#if REMOVE_DUMMY_API
/**
 * @brief This function set h scaler bypass
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_POST_SetHscalerBypass(BOOL bEnable)
{
	//reg_post_hsc_bypass
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_bypass_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_bypass_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_bypass_BITEND no mat,(unsigned int)bEnable);
}
#endif //REMOVE_DUMMY_API

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set h scaler mode
 * @param [in] unsigned int u32Mode
 * @retval VOID
*/
VOID HAL_POST_SetHscalerMode(unsigned int u32Mode)
{
	//reg_post_hsc_mode_y
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_mode_y_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_mode_y_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_mode_y_BITEND no mat,u32Mode);
	//reg_post_hsc_mode_c
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_mode_c_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_mode_c_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_mode_c_BITEND no mat,u32Mode);
}
#endif//REMOVE_REDUNDANT_API

#if REMOVE_DUMMY_API
/**
 * @brief This function set h scaler mode of C
 * @param [in] unsigned int u32Mode
 * @retval VOID
*/
VOID HAL_POST_SetHscalerMode_C(unsigned int u32Mode)
{
	//reg_post_hsc_mode_c
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_mode_c_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_mode_c_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_mode_c_BITEND no mat,u32Mode);
}
#endif

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set h scaler offset
 * @param [in] unsigned int u32Offset
 * @retval VOID
*/
VOID HAL_POST_SetHscalerOffset(unsigned int u32Offset)
{
	//reg_post_hsc_ofst_y
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_ofst_y_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_ofst_y_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_ofst_y_BITEND no mat,u32Offset);
	//reg_post_hsc_ofst_c
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_ofst_c_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_ofst_c_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_ofst_c_BITEND no mat,u32Offset);
}
#endif//REMOVE_REDUNDANT_API

#if REMOVE_DUMMY_API
/**
 * @brief This function set h scaler offset of C
 * @param [in] unsigned int u32Offset
 * @retval VOID
*/
VOID HAL_POST_SetHscalerOffset_C(unsigned int u32Offset)
{
	//reg_post_hsc_ofst_c
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_ofst_c_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_ofst_c_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_ofst_c_BITEND no mat,u32Offset);
}
#endif

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set h scaler coef
 * @param [in] unsigned int u32Coef
 * @retval VOID
*/
VOID HAL_POST_SetHscalerCoef(unsigned int u32Coef)
{
	//reg_post_hsc_coef_y
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_coef_y_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_coef_y_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_coef_y_BITEND no mat,u32Coef);
	//reg_post_hsc_coef_c
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_coef_c_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_coef_c_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_coef_c_BITEND no mat,u32Coef);
}
#endif//REMOVE_REDUNDANT_API

#if REMOVE_DUMMY_API
/**
 * @brief This function set h scaler coef of C
 * @param [in] unsigned int u32Coef
 * @retval VOID
*/
VOID HAL_POST_SetHscalerCoef_C(unsigned int u32Coef)
{
	//reg_post_hsc_coef_c
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_coef_c_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_coef_c_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_coef_c_BITEND no mat,u32Coef);
}
#endif //REMOVE_DUMMY_API

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set h scaler coef1
 * @param [in] unsigned int u32Coef
 * @retval VOID
*/
VOID HAL_POST_SetHscalerCoef1(unsigned int u32Coef)
{
	//reg_post_hsc_coef_y1
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_coef_y1_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_coef_y1_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_coef_y1_BITEND no mat,u32Coef);
	//reg_post_hsc_coef_c1
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_coef_c1_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_coef_c1_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_coef_c1_BITEND no mat,u32Coef);
}
#endif//REMOVE_REDUNDANT_API

#if REMOVE_DUMMY_API
/**
 * @brief This function set h scaler coef1 of C
 * @param [in] unsigned int u32Coef
 * @retval VOID
*/
VOID HAL_POST_SetHscalerCoef1_C(unsigned int u32Coef)
{
	//reg_post_hsc_coef_c1
// MEMC_K6L_BRING_UP //	WriteRegister(FRC_TOP__KPOST_TOP__post_hsc_coef_c1_ADDR no mat,FRC_TOP__KPOST_TOP__post_hsc_coef_c1_BITSTART no mat,FRC_TOP__KPOST_TOP__post_hsc_coef_c1_BITEND no mat,u32Coef);
}
#endif //REMOVE_DUMMY_API

/**
 * @brief This function set csc enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_POST_SetCSCEnable_RTK2885p(BOOL bEnable)
{
	//reg_post_convert_on
	WriteRegister(KPOST_TOP_KPOST_TOP_94_reg,24,24,(unsigned int)bEnable);
}
VOID HAL_POST_SetCSCEnable_RTK2885pp(BOOL bEnable)
{
	M8P_rthw_post_top_kpost_top_84_RBUS M8P_rthw_post_top_kpost_top_84;
	M8P_rthw_post_top_kpost_top_84.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_84_reg);
	M8P_rthw_post_top_kpost_top_84.post_convert_on = bEnable;
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_84_reg, M8P_rthw_post_top_kpost_top_84.regValue);
}
VOID HAL_POST_SetCSCEnable(BOOL bEnable)
{
	if(RUN_MERLIN8P()){
		HAL_POST_SetCSCEnable_RTK2885pp(bEnable);
	}else if(RUN_MERLIN8()){
		HAL_POST_SetCSCEnable_RTK2885p(bEnable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set post csc mode
 * @param [in] CSC_MODE enCSC_Mode
 * @retval VOID
*/
VOID HAL_POST_SetCSCMode_RTK2885p(CSC_MODE enCSC_Mode)
{
	//reg_post_convert_mode
	WriteRegister(KPOST_TOP_KPOST_TOP_94_reg,25,28,(unsigned int)enCSC_Mode);
}
VOID HAL_POST_SetCSCMode_RTK2885pp(CSC_MODE enCSC_Mode)
{
	//RBUS_NOT_READY python has new rbus and algo for convert
	M8P_rthw_post_top_kpost_top_84_RBUS M8P_rthw_post_top_kpost_top_84;
	M8P_rthw_post_top_kpost_top_84.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_84_reg);
	enCSC_Mode = (enCSC_Mode==3) ? 7 : enCSC_Mode;
	M8P_rthw_post_top_kpost_top_84.post_convert_mode = enCSC_Mode;
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_84_reg, M8P_rthw_post_top_kpost_top_84.regValue);
}
VOID HAL_POST_SetCSCMode(CSC_MODE enCSC_Mode)
{
	if(RUN_MERLIN8P()){
		HAL_POST_SetCSCMode_RTK2885pp(enCSC_Mode);
	}else if(RUN_MERLIN8()){
		HAL_POST_SetCSCMode_RTK2885p(enCSC_Mode);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get post mute screen enable
 * @param [in] VOID
 * @retval BOOL bEnable
*/
BOOL HAL_POST_GetMuteScreenEnable_RTK2885p(VOID)
{
	unsigned int u32Enable = 0;
	ReadRegister(KPOST_TOP_KPOST_TOP_60_reg,30,30, &u32Enable);
	return (u32Enable==0) ? 0 : 1;
}
BOOL HAL_POST_GetMuteScreenEnable_RTK2885pp(VOID)
{
	unsigned int u32Enable = 0;
	ReadRegister(BLUE_SCREEN_REG,30,30, &u32Enable);
	return (u32Enable==0) ? 0 : 1;
}
BOOL HAL_POST_GetMuteScreenEnable(VOID)
{
	unsigned int u32Enable = 0;

	if(RUN_MERLIN8P()){
		u32Enable = HAL_POST_GetMuteScreenEnable_RTK2885pp();
	}else if(RUN_MERLIN8()){
		u32Enable = HAL_POST_GetMuteScreenEnable_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32Enable;
}

/**
 * @brief This function set post mute screen enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_POST_SetMuteScreenEnable_RTK2885p(BOOL bEnable)
{
	//reg_bluescreen
	WriteRegister(KPOST_TOP_KPOST_TOP_60_reg,30,30,(unsigned int)bEnable);
}
VOID HAL_POST_SetMuteScreenEnable_RTK2885pp(BOOL bEnable)
{
	WriteRegister(BLUE_SCREEN_REG,30,30,(unsigned int)bEnable);
}
VOID HAL_POST_SetMuteScreenEnable(BOOL bEnable)
{
	if(RUN_MERLIN8P()){
		HAL_POST_SetMuteScreenEnable_RTK2885pp(bEnable);
	}else if(RUN_MERLIN8()){
		HAL_POST_SetMuteScreenEnable_RTK2885p(bEnable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set post mute screen RGB
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_POST_SetMuteScreenRGBEnable_RTK2885p(unsigned int u32RGB)
{
	//reg_bluescreen_rgb
	WriteRegister(KPOST_TOP_KPOST_TOP_60_reg,0,29,u32RGB);
}
VOID HAL_POST_SetMuteScreenRGBEnable_RTK2885pp(unsigned int u32RGB)
{
	WriteRegister(BLUE_SCREEN_REG,0,29,u32RGB);
}
VOID HAL_POST_SetMuteScreenRGBEnable(unsigned int u32RGB)
{
	if(RUN_MERLIN8P()){
		HAL_POST_SetMuteScreenRGBEnable_RTK2885pp(u32RGB);
	}else if(RUN_MERLIN8()){
		HAL_POST_SetMuteScreenRGBEnable_RTK2885p(u32RGB);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set post data mapping
 * @param [in] unsigned int u32Data_Mapping
 * @retval VOID
*/
VOID HAL_POST_SetDataMapping(unsigned int u32Data_Mapping)
{
	//reg_post_convert_map
	WriteRegister(KPOST_TOP_KPOST_TOP_94_reg,29,31,(unsigned int)u32Data_Mapping);
}

/**
 * @brief This function get post data mapping
 * @param [in] VOID
 * @retval data mapping
*/
unsigned int HAL_POST_GetDataMapping(VOID)
{
	unsigned int u32Data_Mapping = 0;
	//reg_post_convert_map
	ReadRegister(KPOST_TOP_KPOST_TOP_94_reg,29,31,&u32Data_Mapping);
	return u32Data_Mapping;
}

/**
 * @brief This function set black insert line mode enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
extern unsigned char MEMC_instanboot_resume_check[10];
VOID HAL_POST_SetBlackInsertLineModeEnable(BOOL bEnable)
{
	//reg_blackinsert_line_en
	MEMC_instanboot_resume_check[4]=1;
	WriteRegister(KPOST_TOP_KPOST_TOP_08_reg,10,10,(unsigned int)bEnable);//not used
}

/**
 * @brief This function set black insert line mode pattern number
 * @param [in] unsigned int u32PatternNum
 * @retval VOID
*/
VOID HAL_POST_SetBlackInsertLineModePatternNum(unsigned int u32PatternNum)
{
	//reg_blackinsert_line_type
	MEMC_instanboot_resume_check[9]=1;
	WriteRegister(KPOST_TOP_KPOST_TOP_08_reg,11,14,u32PatternNum);
}

/**
 * @brief This function set black insert line mode pattern
 * @param [in] unsigned int u32Pattern
 * @retval VOID
*/
VOID HAL_POST_SetBlackInsertLineModeLPattern(unsigned int u32Pattern)
{
	//reg_blackinsert_line_patt_l
	MEMC_instanboot_resume_check[7]=1;
	WriteRegister(KPOST_TOP_KPOST_TOP_08_reg,15,30,u32Pattern);
}

/**
 * @brief This function set black insert line mode pattern
 * @param [in] unsigned int u32Pattern
 * @retval VOID
*/
VOID HAL_POST_SetBlackInsertLineModeRPattern(unsigned int u32Pattern)
{
	//reg_blackinsert_line_patt_r
	WriteRegister(KPOST_TOP_KPOST_TOP_0C_reg,15,30,u32Pattern);
}

/**
 * @brief This function set black insert frame mode enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_POST_SetBlackInsertFrameModeEnable(BOOL bEnable)
{
	//reg_blackinsert_frame_en
	MEMC_instanboot_resume_check[8]=1;
	WriteRegister(KPOST_TOP_KPOST_TOP_08_reg,31,31,(unsigned int)bEnable);
}
#endif//REMOVE_REDUNDANT_API

/**
 * @brief This function set kpost out interrupt select
 * @param [in] 7 for FRC, 6 for OSD, 5 for Out
 * @retval VOID
*/
VOID HAL_POST_SetOutINTSel_RTK2885p(unsigned int u32Sel)
{
	//reg_post_out_int_sel
	WriteRegister(KPOST_TOP_KPOST_TOP_04_reg,13,15,u32Sel);
}
VOID HAL_POST_SetOutINTSel_RTK2885pp(unsigned int u32Sel)
{
	M8P_rthw_post_top_kpost_top_04_RBUS M8P_rthw_post_top_kpost_top_04;
	M8P_rthw_post_top_kpost_top_04.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_04_reg);
	M8P_rthw_post_top_kpost_top_04.post_out_int_sel = u32Sel;
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_04_reg, M8P_rthw_post_top_kpost_top_04.regValue);
}
VOID HAL_POST_SetOutINTSel(unsigned int u32Sel)
{
	if(RUN_MERLIN8P()){
		HAL_POST_SetOutINTSel_RTK2885pp(u32Sel);
	}else if(RUN_MERLIN8()){
		HAL_POST_SetOutINTSel_RTK2885p(u32Sel);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set kpost out interrupt source
 * @param [in] 1
 * @retval VOID
*/
VOID HAL_POST_SetOutINTSource_RTK2885p(unsigned int u32Sor)//b809D008, bit[0]
{
	//reg_post_int_source_sel	
	unsigned long flags;//for spin_lock_irqsave
	spin_lock_irqsave(&memc_task_Spinlock, flags);
	WriteRegister(KPOST_TOP_KPOST_TOP_08_reg,0,0,u32Sor);
	spin_unlock_irqrestore(&memc_task_Spinlock, flags);
}
VOID HAL_POST_SetOutINTSource_RTK2885pp(unsigned int u32Sor)
{
	M8P_rthw_post_top_kpost_top_08_RBUS M8P_rthw_post_top_kpost_top_08;
	unsigned long flags;//for spin_lock_irqsave
	spin_lock_irqsave(&memc_task_Spinlock, flags);
	M8P_rthw_post_top_kpost_top_08.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg);
	M8P_rthw_post_top_kpost_top_08.post_int_source_sel = u32Sor;
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, M8P_rthw_post_top_kpost_top_08.regValue);
	spin_unlock_irqrestore(&memc_task_Spinlock, flags);
}
VOID HAL_POST_SetOutINTSource(unsigned int u32Sor)
{
	if(RUN_MERLIN8P()){
		HAL_POST_SetOutINTSource_RTK2885pp(u32Sor);
	}else if(RUN_MERLIN8()){
		HAL_POST_SetOutINTSource_RTK2885p(u32Sor);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if REMOVE_REDUNDANT_API
/**
 * @brief This function set kpost out interrupt horizantal number
 * @param [in] H number
 * @retval VOID
*/
VOID HAL_POST_SetOutINTHNum(unsigned int u32Hnum)
{
	//reg_post_out_vtotal_int
	WriteRegister(KPOST_TOP_KPOST_TOP_04_reg,20,31,u32Hnum);
}
#endif //REMOVE_REDUNDANT_API

/**
 * @brief This function set kpost out interrupt enable
 * @param [in] u32IntType: 0 for H sync, 1 for V sync
 * @param [in] bEnable
 * @retval VOID
*/
VOID HAL_POST_SetOutINTEnable_RTK2885p(unsigned int u32IntType, BOOL bEnable)
{
	unsigned int u32InINT_en = 0;
	unsigned long flags;//for spin_lock_irqsave
	spin_lock_irqsave(&memc_task_Spinlock, flags);
	ReadRegister(KPOST_TOP_KPOST_TOP_08_reg,5,8, &u32InINT_en);
	
	if(bEnable)
	{
		u32InINT_en = u32InINT_en | (1 << u32IntType);
	}
	else
	{
		u32InINT_en = u32InINT_en & (~(1 << u32IntType));
	}
	//rtd_pr_memc_notice( "%s %d\n",__func__,__LINE__);
	//rtd_pr_memc_notice( "[%d,%d,%d]\r\n",u32InINT_en, u32IntType, bEnable);
	//reg_post_int_en
	WriteRegister(KPOST_TOP_KPOST_TOP_08_reg,5,8,u32InINT_en);
	spin_unlock_irqrestore(&memc_task_Spinlock, flags);
}
VOID HAL_POST_SetOutINTEnable_RTK2885pp(unsigned int u32IntType, BOOL bEnable)
{
	unsigned long flags;//for spin_lock_irqsave
	spin_lock_irqsave(&memc_task_Spinlock, flags);
	if(u32IntType == INT_TYPE_HSYNC){ // 
		WriteRegister(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, 7, 7, bEnable);
	}
	else if(u32IntType == INT_TYPE_VSYNC){ // 
		WriteRegister(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, 5, 5, bEnable);
	}
	spin_unlock_irqrestore(&memc_task_Spinlock, flags);
}
VOID HAL_POST_SetOutINTEnable(unsigned int u32IntType, BOOL bEnable)
{
	if(RUN_MERLIN8P()){
		HAL_POST_SetOutINTEnable_RTK2885pp(u32IntType, bEnable);
	}else if(RUN_MERLIN8()){
		HAL_POST_SetOutINTEnable_RTK2885p(u32IntType, bEnable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

BOOL HAL_POST_GetOutINTEnable_RTK2885p(unsigned int u32IntType)//b809D008, bit[5:8]
{
	unsigned int v = 0;
       ReadRegister(KPOST_TOP_KPOST_TOP_08_reg,5,8,&v);
       return (v >> u32IntType) & 0x1;
}
BOOL HAL_POST_GetOutINTEnable_RTK2885pp(unsigned int u32IntType)
{
	unsigned int v = 0;

	if(u32IntType == INT_TYPE_HSYNC){ // 
		ReadRegister(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, 7, 7,&v);
	}
	else if(u32IntType == INT_TYPE_VSYNC){ // 
		ReadRegister(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, 5, 5,&v);
	}
	return v;
}
BOOL HAL_POST_GetOutINTEnable(unsigned int u32IntType)
{
	BOOL u1_Enable = false;

	if(RUN_MERLIN8P()){
		u1_Enable = HAL_POST_GetOutINTEnable_RTK2885pp(u32IntType);
	}else if(RUN_MERLIN8()){
		u1_Enable = HAL_POST_GetOutINTEnable_RTK2885p(u32IntType);
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u1_Enable;
}

/**
 * @brief This function set kpost top out interrupt clear
 * @param [in] eIntType: 0 for H sync, 1 for V sync
 * @param [in] bWclr
 * @retval VOID
*/
extern unsigned char MEMC_instanboot_resume_check[10];
VOID HAL_KPOST_TOP_SetOutINTWclr_RTK2885p(INT_TYPE eIntType, BOOL bWclr)//b809D008, bit 1:4
{
#ifndef BUILD_QUICK_SHOW
	unsigned int u32InINT_Clear = 0;
	unsigned long flags;//for spin_lock_irqsave
	MEMC_instanboot_resume_check[1]=1;
	spin_lock_irqsave(&memc_task_Spinlock, flags);
	ReadRegister(KPOST_TOP_KPOST_TOP_08_reg,1,4, &u32InINT_Clear);

	if(bWclr)
		u32InINT_Clear = u32InINT_Clear | ( 1 << (unsigned int)eIntType);
	else
		u32InINT_Clear = u32InINT_Clear & (~( 1 << (unsigned int)eIntType));
	
	//reg_kmc_int_en
	WriteRegister(KPOST_TOP_KPOST_TOP_08_reg,1,4, u32InINT_Clear);
	spin_unlock_irqrestore(&memc_task_Spinlock, flags);
#endif
}
VOID HAL_KPOST_TOP_SetOutINTWclr_RTK2885pp(INT_TYPE eIntType, BOOL bWclr)
{
#ifndef BUILD_QUICK_SHOW
	unsigned long flags;//for spin_lock_irqsave
	MEMC_instanboot_resume_check[1]=1;
	spin_lock_irqsave(&memc_task_Spinlock, flags);
	if(eIntType == 0){ // INT_TYPE_HSYNC
		WriteRegister(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, 3, 3, bWclr);
	}
	else if(eIntType == 1){ // INT_TYPE_VSYNC
		WriteRegister(M8P_RTHW_POST_TOP_KPOST_TOP_08_reg, 1, 1, bWclr);
	}
	spin_unlock_irqrestore(&memc_task_Spinlock, flags);
#endif
}
VOID HAL_KPOST_TOP_SetOutINTWclr(INT_TYPE eIntType, BOOL bWclr)
{
	if(RUN_MERLIN8P()){
		HAL_KPOST_TOP_SetOutINTWclr_RTK2885pp(eIntType, bWclr);
	}else if(RUN_MERLIN8()){
		HAL_KPOST_TOP_SetOutINTWclr_RTK2885p(eIntType, bWclr);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function get kost top out interrupt status
 * @retval unsigned int
*/
unsigned int HAL_KPOST_TOP_GetOutINTStatus_RTK2885p(INT_TYPE eIntType)
{
	unsigned int u32OutINT_Sts = 0;
	//ReadRegister(KPOST_TOP_KPOST_TOP_F0_reg,24,27, &u32OutINT_Sts);
	ReadRegister(KPOST_TOP_KPOST_TOP_F0_reg,27,30, &u32OutINT_Sts);
	return u32OutINT_Sts;
}
unsigned int HAL_KPOST_TOP_GetOutINTStatus_RTK2885pp(INT_TYPE eIntType)
{
	unsigned int u32OutINT_Sts = 0;

	if(eIntType == 0){ // INT_TYPE_HSYNC
		ReadRegister(M8P_RTHW_POST_TOP_KPOST_TOP_BC_reg, 29, 29, &u32OutINT_Sts);
	}
	else if(eIntType == 1){ // INT_TYPE_VSYNC
		ReadRegister(M8P_RTHW_POST_TOP_KPOST_TOP_BC_reg, 27, 27, &u32OutINT_Sts);
	}
	return u32OutINT_Sts;
}
unsigned int HAL_KPOST_TOP_GetOutINTStatus(INT_TYPE eIntType)
{
	unsigned int u32OutINT_Sts = 0;

	if(RUN_MERLIN8P()){
		u32OutINT_Sts = HAL_KPOST_TOP_GetOutINTStatus_RTK2885pp(eIntType);
	}else if(RUN_MERLIN8()){
		u32OutINT_Sts = HAL_KPOST_TOP_GetOutINTStatus_RTK2885p(eIntType);
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32OutINT_Sts;
}

/**
 * @brief This function set post cursor enable
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_POST_SetCursorEnable_RTK2885p(BOOL bEnable)
{
	//reg_post_cursor_en
	WriteRegister(KPOST_TOP_KPOST_TOP_00_reg,15,15,(unsigned int)bEnable);
}
VOID HAL_POST_SetCursorEnable_RTK2885pp(BOOL bEnable)
{
	M8P_rthw_post_top_kpost_top_00_RBUS M8P_rthw_post_top_kpost_top_00;
	M8P_rthw_post_top_kpost_top_00.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_00_reg);
	M8P_rthw_post_top_kpost_top_00.post_cursor_en = bEnable;
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_00_reg, M8P_rthw_post_top_kpost_top_00.regValue);
}
VOID HAL_POST_SetCursorEnable(BOOL bEnable)
{
	if(RUN_MERLIN8P()){
		HAL_POST_SetCursorEnable_RTK2885pp(bEnable);
	}else if(RUN_MERLIN8()){
		HAL_POST_SetCursorEnable_RTK2885p(bEnable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set post cursor show
 * @param [in] BOOL bEnable
 * @retval VOID
*/
VOID HAL_POST_SetCursorShow_RTK2885p(BOOL bEnable)
{
	//reg_post_cursor_show
	WriteRegister(KPOST_TOP_KPOST_TOP_00_reg,13,13,(unsigned int)bEnable);
}
VOID HAL_POST_SetCursorShow_RTK2885pp(BOOL bEnable)
{
	M8P_rthw_post_top_kpost_top_00_RBUS M8P_rthw_post_top_kpost_top_00;
	M8P_rthw_post_top_kpost_top_00.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_00_reg);
	M8P_rthw_post_top_kpost_top_00.post_cursor_show = bEnable;
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_00_reg, M8P_rthw_post_top_kpost_top_00.regValue);
}
VOID HAL_POST_SetCursorShow(BOOL bEnable)
{
	if(RUN_MERLIN8P()){
		HAL_POST_SetCursorShow_RTK2885pp(bEnable);
	}else if(RUN_MERLIN8()){
		HAL_POST_SetCursorShow_RTK2885p(bEnable);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set post cursor position
 * @param [in] reg_post_cursor_h
 * @param [in] reg_post_cursor_v
 * @retval VOID
*/
VOID HAL_POST_SetCursorPosition_RTK2885p(unsigned int u32Hposition, unsigned int u32Vposition)
{
	//reg_post_cursor_h
	WriteRegister(KPOST_TOP_KPOST_TOP_00_reg,0,12,(unsigned int)u32Hposition);
	//reg_post_cursor_v
	WriteRegister(KPOST_TOP_KPOST_TOP_00_reg,16,28,(unsigned int)u32Vposition);
}
VOID HAL_POST_SetCursorPosition_RTK2885pp(unsigned int u32Hposition, unsigned int u32Vposition)
{
	M8P_rthw_post_top_kpost_top_00_RBUS M8P_rthw_post_top_kpost_top_00;
	M8P_rthw_post_top_kpost_top_00.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_00_reg);
	M8P_rthw_post_top_kpost_top_00.post_cursor_h = u32Hposition;
	M8P_rthw_post_top_kpost_top_00.post_cursor_v = u32Vposition;
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_00_reg, M8P_rthw_post_top_kpost_top_00.regValue);
}
VOID HAL_POST_SetCursorPosition(unsigned int u32Hposition, unsigned int u32Vposition)
{
	if(RUN_MERLIN8P()){
		HAL_POST_SetCursorPosition_RTK2885pp(u32Hposition, u32Vposition);
	}else if(RUN_MERLIN8()){
		HAL_POST_SetCursorPosition_RTK2885p(u32Hposition, u32Vposition);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set post cursor width
 * @param [in] unsigned int u32Width(0~15)
 * @retval VOID
*/
VOID HAL_POST_SetCursorWidth_RTK2885p(unsigned int u32Width)
{
	//reg_post_cursor_h_width
	WriteRegister(KPOST_TOP_KPOST_TOP_20_reg,28,31,(unsigned int)u32Width);
}
VOID HAL_POST_SetCursorWidth_RTK2885pp(unsigned int u32Width)
{
	M8P_rthw_post_top_kpost_top_20_RBUS M8P_rthw_post_top_kpost_top_20;
	M8P_rthw_post_top_kpost_top_20.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_20_reg);
	M8P_rthw_post_top_kpost_top_20.post_cursor_h_width = u32Width;
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_20_reg, M8P_rthw_post_top_kpost_top_20.regValue);
}
VOID HAL_POST_SetCursorWidth(unsigned int u32Width)
{
	if(RUN_MERLIN8P()){
		HAL_POST_SetCursorWidth_RTK2885pp(u32Width);
	}else if(RUN_MERLIN8()){
		HAL_POST_SetCursorWidth_RTK2885p(u32Width);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

/**
 * @brief This function set post cursor color
 * @param [in] unsigned int u32Width(0~7)
 * @retval VOID
*/
VOID HAL_POST_SetCursorColor_RTK2885p(unsigned int u32Color)
{
	//reg_post_cursor_h_width
	WriteRegister(KPOST_TOP_KPOST_TOP_00_reg,29,31,(unsigned int)u32Color);
}
VOID HAL_POST_SetCursorColor_RTK2885pp(unsigned int u32Color)
{
	M8P_rthw_post_top_kpost_top_00_RBUS M8P_rthw_post_top_kpost_top_00;
	M8P_rthw_post_top_kpost_top_00.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_00_reg);
	M8P_rthw_post_top_kpost_top_00.post_cursor_color = u32Color;
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_00_reg, M8P_rthw_post_top_kpost_top_00.regValue);
}
VOID HAL_POST_SetCursorColor(unsigned int u32Color)
{
	if(RUN_MERLIN8P()){
		HAL_POST_SetCursorColor_RTK2885pp(u32Color);
	}else if(RUN_MERLIN8()){
		HAL_POST_SetCursorColor_RTK2885p(u32Color);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

