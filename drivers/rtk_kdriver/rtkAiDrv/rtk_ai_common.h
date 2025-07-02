#ifndef _RTK_AI_COMMON_H_
#define _RTK_AI_COMMON_H_

/************************************************************************
 *  Include files
 ************************************************************************/
#if defined(CONFIG_ARCH_RTK2875Q)
// SW NNIP,do not include nnip reg
#else
#include <rbus/nnwp_reg.h>
#endif
#include <rbus/sys_reg_reg.h>
#include <scaler/scalerCommon.h>
#include <tvscalercontrol/io/ioregdrv.h>

 /************************************************************************
 *  Definitions
 ************************************************************************/
#define BIT_AI_PQ						_BIT0
#define BIT_AI_AUDIO					_BIT1
#define BIT_AI_CAMERA					_BIT2
#define BIT_AI_AUDIO_DENOISE			_BIT3

#if defined(CONFIG_ARCH_RTK2875Q)
#define rtk_ai_enable_status( X )
#define rtk_ai_disable_status( X )
#else
#define rtk_ai_enable_status( X )	\
		if((IoReg_Read32(SYS_REG_SYS_CLKEN0_reg)&SYS_REG_SYS_CLKEN0_clken_nnip_mask)) IoReg_Write32(NNWP_nnwp_dmy1_rw_reg,IoReg_Read32(NNWP_nnwp_dmy1_rw_reg)|X)

#define rtk_ai_disable_status( X )	\
		if((IoReg_Read32(SYS_REG_SYS_CLKEN0_reg)&SYS_REG_SYS_CLKEN0_clken_nnip_mask)) IoReg_Write32(NNWP_nnwp_dmy1_rw_reg,IoReg_Read32(NNWP_nnwp_dmy1_rw_reg)&(~X))
#endif

#endif	/* _RTK_AI_COMMON_H_ */
