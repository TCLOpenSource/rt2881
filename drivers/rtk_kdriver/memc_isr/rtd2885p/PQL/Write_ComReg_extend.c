#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_isr/PQL/IPR.h"
#include "memc_isr/HAL/hal_mc.h"
#include "memc_isr/Common/memc_type.h"
#include "memc_isr/HAL/hal_ip.h"
#include "memc_isr/HAL/hal_post.h"
#include "memc_isr/include/memc_lib.h"
#include "memc_isr/include/memc_identify_ctrl.h"
//#include <tvscalercontrol/scaler/scalerstruct.h>
#include "memc_reg_def.h"
#include <rbus/od_reg.h>

#ifdef BUILD_QUICK_SHOW
#include <div64.h>
#endif

#define TAG_NAME "MEMC"
#define STATICS_COV_NUM	(18)

#define u32_fpga_offset 0x8000

extern unsigned int g_cadence_change_case;
extern VOID MEMC_Lib_ME_overflow_Check(VOID);
extern void Identify_ctrl_wrtAction(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
extern void Identify_ctrl_Status_wrtAction(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
extern VOID HAL_MC_SetVarLPFEn(unsigned int u32En);
extern unsigned char Scaler_MEMC_Get_SW_OPT_Flag(void);
extern VOID MEMC_Lib_MC_IndexCheck(VOID);
extern VOID MEMC_LibResetFlow(VOID);
extern VOID MEMC_Lib_VR360_Flow(VOID);
extern VOID MEMC_Lib_CadenceChangeCtrl(VOID);
extern VOID MEMC_LibSet_PQC_DB_apply(VOID);
extern VOID MEMC_LibSet_MCDMA_DB_apply(VOID);
extern VOID MEMC_Lib_SetLineMode(unsigned char u1_enable);
extern unsigned char MEMC_Lib_get_LineMode_flag(VOID);
extern VOID MEMC_LibSet_PQC_DB_en(unsigned int u1_enable);
extern VOID MEMC_LibSet_MCDMA_DB_en(unsigned int u1_enable);
extern VOID MEMC_Lib_DelayTime_Calc(VOID);
extern VOID Mid_Mode_MEDEN_Checking(VOID);
extern VOID MEMC_Lib_SetLbmcVtrigDelayMode(unsigned char u1_enable);
extern unsigned char MEMC_Lib_get_LbmcTrig_flag(VOID);
extern VOID MEMC_Lib_MEMC_Self_Check(VOID);
extern void update_switch_on_off(void);
extern void MEMC_LBMC_Apply(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
extern VOID MEMC_DisableMute(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
extern VOID MEMC_HDMI_PowerSaving(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
extern VOID FB_NoNeedToDoFRC_wrtAction(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);

unsigned short Data_Mapping_RTK2885pp(unsigned short character)
{
	unsigned short Ret = 0;
	switch (character)
	{
		case '0' :
			Ret = 0xe667;
			break;
		case '1' :
			Ret = 0x0810;
			break;
		case '2' :
			Ret = 0xc3c3;
			break;
		case '3' :
			Ret = 0xe143;
			break;
		case '4' :
			Ret = 0x21c4;
			break;
		case '5' :
			Ret = 0xe187;
			break;
		case '6' :
			Ret = 0xe387;
			break;
		case '7' :
			Ret = 0x2043;
			break;
		case '8' :
			Ret = 0xe3c7;
			break;
		case '9' :
			Ret = 0xe1c7;
			break;
		case 'A' :
			Ret = 0x23c7;
			break;
		case 'a' :
			Ret = 0x23c7;
			break;
		case 'B' :
			Ret = 0xe9d3;
			break;
		case 'b' :
			Ret = 0xe9d3;
			break;
		case 'C' :
			Ret = 0xc207;
			break;
		case 'c' :
			Ret = 0xc207;
			break;
		case 'D' :
			Ret = 0xe8d3;
			break;
		case 'd' :
			Ret = 0xe8d3;
			break;
		case 'E' :
			Ret = 0xc287;
			break;
		case 'e' :
			Ret = 0xc287;
			break;
		case 'F' :
			Ret = 0x0287;
			break;
		case 'f' :
			Ret = 0x0287;
			break;
		case 'G' :
			Ret = 0xe307;
			break;
		case 'H' :
			Ret = 0x23c4;
			break;
		case 'I' :
			Ret = 0xc813;
			break;
		case 'J' :
			Ret = 0xe240;
			break;
		case 'K' :
			Ret = 0x12a4;
			break;
		case 'L' :
			Ret = 0xc204;
			break;
		case 'M' :
			Ret = 0x226c;
			break;
		case 'N' :
			Ret = 0x324c;
			break;
		case 'O' :
			Ret = 0xe247;
			break;
		case 'P' :
			Ret = 0x03c7;
			break;
		case 'Q' :
			Ret = 0xf247;
			break;
		case 'R' :
			Ret = 0x13c7;
			break;
		case 'S' :
			Ret = 0xf18f;
			break;
		case 'T' :
			Ret = 0x0813;
			break;
		case 'U' :
			Ret = 0xe244;
			break;
		case 'V' :
			Ret = 0x0624;
			break;
		case 'W' :
			Ret = 0x3644;
			break;
		case 'X' :
			Ret = 0x1428;
			break;
		case 'Y' :
			Ret = 0x0828;
			break;
		case 'Z' :
			Ret = 0xc423;
			break;
		case '+' :
			Ret = 0x0990;
			break;
		case '-' :
			Ret = 0x0180;
			break;
		default:
			Ret = 0x0000;
			break;
	}
	return Ret;
}

void Wrt_segment_API_RTK2885pp(unsigned char seg_ID, unsigned int char_ID, short data, unsigned char color, unsigned int x, unsigned int y)
{
	unsigned short character = 0;
	M8P_rthw_post_top_kpost_top_0c_RBUS M8P_rthw_post_top_kpost_top_0c_reg;
	M8P_rthw_post_top_kpost_top_10_RBUS M8P_rthw_post_top_kpost_top_10_reg;
	M8P_rthw_post_top_kpost_top_28_RBUS M8P_rthw_post_top_kpost_top_28_reg;
	M8P_rthw_post_top_kpost_top_2c_RBUS M8P_rthw_post_top_kpost_top_2c_reg;
	M8P_rthw_post_top_kpost_top_30_RBUS M8P_rthw_post_top_kpost_top_30_reg;
	M8P_rthw_post_top_kpost_top_34_RBUS M8P_rthw_post_top_kpost_top_34_reg;
	M8P_rthw_post_top_kpost_top_38_RBUS M8P_rthw_post_top_kpost_top_38_reg;
	M8P_rthw_post_top_kpost_top_3c_RBUS M8P_rthw_post_top_kpost_top_3c_reg;
	M8P_rthw_post_top_kpost_top_40_RBUS M8P_rthw_post_top_kpost_top_40_reg;
	M8P_rthw_post_top_kpost_top_44_RBUS M8P_rthw_post_top_kpost_top_44_reg;
	M8P_rthw_post_top_kpost_top_48_RBUS M8P_rthw_post_top_kpost_top_48_reg;
	M8P_rthw_post_top_kpost_top_4c_RBUS M8P_rthw_post_top_kpost_top_4c_reg;
	M8P_rthw_post_top_kpost_top_50_RBUS M8P_rthw_post_top_kpost_top_50_reg;
	M8P_rthw_post_top_kpost_top_54_RBUS M8P_rthw_post_top_kpost_top_54_reg;
	M8P_rthw_post_top_kpost_top_58_RBUS M8P_rthw_post_top_kpost_top_58_reg;
	M8P_rthw_post_top_kpost_top_5c_RBUS M8P_rthw_post_top_kpost_top_5c_reg;
	M8P_rthw_post_top_kpost_top_60_RBUS M8P_rthw_post_top_kpost_top_60_reg;
	M8P_rthw_post_top_kpost_top_64_RBUS M8P_rthw_post_top_kpost_top_64_reg;
	M8P_rthw_post_top_kpost_top_68_RBUS M8P_rthw_post_top_kpost_top_68_reg;
	M8P_rthw_post_top_kpost_top_6c_RBUS M8P_rthw_post_top_kpost_top_6c_reg;
	M8P_rthw_post_top_kpost_top_88_RBUS M8P_rthw_post_top_kpost_top_88_reg;
	M8P_rthw_post_top_kpost_top_8c_RBUS M8P_rthw_post_top_kpost_top_8c_reg;
	M8P_rthw_post_top_kpost_top_90_RBUS M8P_rthw_post_top_kpost_top_90_reg;
	M8P_rthw_post_top_kpost_top_94_RBUS M8P_rthw_post_top_kpost_top_94_reg;
	M8P_rthw_post_top_kpost_top_98_RBUS M8P_rthw_post_top_kpost_top_98_reg;
	M8P_rthw_post_top_kpost_top_9c_RBUS M8P_rthw_post_top_kpost_top_9c_reg;
	M8P_rthw_post_top_kpost_top_a0_RBUS M8P_rthw_post_top_kpost_top_a0_reg;
	M8P_rthw_post_top_kpost_top_a4_RBUS M8P_rthw_post_top_kpost_top_a4_reg;
	M8P_rthw_post_top_kpost_top_a8_RBUS M8P_rthw_post_top_kpost_top_a8_reg;
	M8P_rthw_post_top_kpost_top_ac_RBUS M8P_rthw_post_top_kpost_top_ac_reg;
	M8P_rthw_post_top_kpost_top_b0_RBUS M8P_rthw_post_top_kpost_top_b0_reg;
	M8P_rthw_post_top_kpost_top_c4_RBUS M8P_rthw_post_top_kpost_top_c4_reg;
	M8P_rthw_post_top_kpost_top_c8_RBUS M8P_rthw_post_top_kpost_top_c8_reg;
	M8P_rthw_post_top2_kpost_top2_00_RBUS M8P_rthw_post_top2_kpost_top2_00_reg;
	M8P_rthw_post_top2_kpost_top2_04_RBUS M8P_rthw_post_top2_kpost_top2_04_reg;
	M8P_rthw_post_top2_kpost_top2_08_RBUS M8P_rthw_post_top2_kpost_top2_08_reg;
	M8P_rthw_post_top2_kpost_top2_0c_RBUS M8P_rthw_post_top2_kpost_top2_0c_reg;
	M8P_rthw_post_top2_kpost_top2_10_RBUS M8P_rthw_post_top2_kpost_top2_10_reg;
	M8P_rthw_post_top2_kpost_top2_14_RBUS M8P_rthw_post_top2_kpost_top2_14_reg;
	M8P_rthw_post_top2_kpost_top2_18_RBUS M8P_rthw_post_top2_kpost_top2_18_reg;
	M8P_rthw_post_top2_kpost_top2_1c_RBUS M8P_rthw_post_top2_kpost_top2_1c_reg;
	M8P_rthw_post_top2_kpost_top2_20_RBUS M8P_rthw_post_top2_kpost_top2_20_reg;
	M8P_rthw_post_top2_kpost_top2_24_RBUS M8P_rthw_post_top2_kpost_top2_24_reg;
	M8P_rthw_post_top2_kpost_top2_28_RBUS M8P_rthw_post_top2_kpost_top2_28_reg;
	M8P_rthw_post_top2_kpost_top2_2c_RBUS M8P_rthw_post_top2_kpost_top2_2c_reg;
	M8P_rthw_post_top2_kpost_top2_30_RBUS M8P_rthw_post_top2_kpost_top2_30_reg;
	M8P_rthw_post_top2_kpost_top2_34_RBUS M8P_rthw_post_top2_kpost_top2_34_reg;
	M8P_rthw_post_top2_kpost_top2_38_RBUS M8P_rthw_post_top2_kpost_top2_38_reg;
	M8P_rthw_post_top2_kpost_top2_3c_RBUS M8P_rthw_post_top2_kpost_top2_3c_reg;
	M8P_rthw_post_top2_kpost_top2_40_RBUS M8P_rthw_post_top2_kpost_top2_40_reg;
	M8P_rthw_post_top2_kpost_top2_44_RBUS M8P_rthw_post_top2_kpost_top2_44_reg;
	M8P_rthw_post_top2_kpost_top2_48_RBUS M8P_rthw_post_top2_kpost_top2_48_reg;
	M8P_rthw_post_top2_kpost_top2_4c_RBUS M8P_rthw_post_top2_kpost_top2_4c_reg;
	M8P_rthw_post_top2_kpost_top2_50_RBUS M8P_rthw_post_top2_kpost_top2_50_reg;
	M8P_rthw_post_top2_kpost_top2_54_RBUS M8P_rthw_post_top2_kpost_top2_54_reg;
	M8P_rthw_post_top2_kpost_top2_58_RBUS M8P_rthw_post_top2_kpost_top2_58_reg;
	M8P_rthw_post_top2_kpost_top2_5c_RBUS M8P_rthw_post_top2_kpost_top2_5c_reg;
	M8P_rthw_post_top2_kpost_top2_60_RBUS M8P_rthw_post_top2_kpost_top2_60_reg;
	M8P_rthw_post_top2_kpost_top2_64_RBUS M8P_rthw_post_top2_kpost_top2_64_reg;
	M8P_rthw_post_top2_kpost_top2_68_RBUS M8P_rthw_post_top2_kpost_top2_68_reg;
	M8P_rthw_post_top2_kpost_top2_6c_RBUS M8P_rthw_post_top2_kpost_top2_6c_reg;
	M8P_rthw_post_top2_kpost_top2_70_RBUS M8P_rthw_post_top2_kpost_top2_70_reg;
	M8P_rthw_post_top2_kpost_top2_74_RBUS M8P_rthw_post_top2_kpost_top2_74_reg;
	M8P_rthw_post_top2_kpost_top2_78_RBUS M8P_rthw_post_top2_kpost_top2_78_reg;
	M8P_rthw_post_top2_kpost_top2_7c_RBUS M8P_rthw_post_top2_kpost_top2_7c_reg;
	M8P_rthw_post_top2_kpost_top2_80_RBUS M8P_rthw_post_top2_kpost_top2_80_reg;
	M8P_rthw_post_top2_kpost_top2_84_RBUS M8P_rthw_post_top2_kpost_top2_84_reg;
	M8P_rthw_post_top2_kpost_top2_88_RBUS M8P_rthw_post_top2_kpost_top2_88_reg;
	M8P_rthw_post_top2_kpost_top2_8c_RBUS M8P_rthw_post_top2_kpost_top2_8c_reg;
	M8P_rthw_post_top2_kpost_top2_90_RBUS M8P_rthw_post_top2_kpost_top2_90_reg;
	M8P_rthw_post_top2_kpost_top2_94_RBUS M8P_rthw_post_top2_kpost_top2_94_reg;
	M8P_rthw_post_top2_kpost_top2_98_RBUS M8P_rthw_post_top2_kpost_top2_98_reg;
	M8P_rthw_post_top2_kpost_top2_9c_RBUS M8P_rthw_post_top2_kpost_top2_9c_reg;
	M8P_rthw_post_top2_kpost_top2_a0_RBUS M8P_rthw_post_top2_kpost_top2_a0_reg;
	M8P_rthw_post_top2_kpost_top2_a4_RBUS M8P_rthw_post_top2_kpost_top2_a4_reg;
	M8P_rthw_post_top2_kpost_top2_a8_RBUS M8P_rthw_post_top2_kpost_top2_a8_reg;
	M8P_rthw_post_top2_kpost_top2_ac_RBUS M8P_rthw_post_top2_kpost_top2_ac_reg;
	M8P_rthw_post_top2_kpost_top2_b0_RBUS M8P_rthw_post_top2_kpost_top2_b0_reg;
	M8P_rthw_post_top2_kpost_top2_b4_RBUS M8P_rthw_post_top2_kpost_top2_b4_reg;
	M8P_rthw_post_top2_kpost_top2_b8_RBUS M8P_rthw_post_top2_kpost_top2_b8_reg;
	M8P_rthw_post_top2_kpost_top2_bc_RBUS M8P_rthw_post_top2_kpost_top2_bc_reg;
	M8P_rthw_post_top2_kpost_top2_c0_RBUS M8P_rthw_post_top2_kpost_top2_c0_reg;
	M8P_rthw_post_top2_kpost_top2_c4_RBUS M8P_rthw_post_top2_kpost_top2_c4_reg;
	M8P_rthw_post_top2_kpost_top2_c8_RBUS M8P_rthw_post_top2_kpost_top2_c8_reg;
	M8P_rthw_post_top2_kpost_top2_cc_RBUS M8P_rthw_post_top2_kpost_top2_cc_reg;
	M8P_rthw_post_top2_kpost_top2_d0_RBUS M8P_rthw_post_top2_kpost_top2_d0_reg;
	M8P_rthw_post_top2_kpost_top2_d4_RBUS M8P_rthw_post_top2_kpost_top2_d4_reg;
	M8P_rthw_post_top2_kpost_top2_d8_RBUS M8P_rthw_post_top2_kpost_top2_d8_reg;
	M8P_rthw_post_top2_kpost_top2_dc_RBUS M8P_rthw_post_top2_kpost_top2_dc_reg;

	M8P_rthw_post_top_kpost_top_0c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_0C_reg);
	M8P_rthw_post_top_kpost_top_10_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_10_reg);
	M8P_rthw_post_top_kpost_top_28_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_28_reg);
	M8P_rthw_post_top_kpost_top_2c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_2C_reg);
	M8P_rthw_post_top_kpost_top_30_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_30_reg);
	M8P_rthw_post_top_kpost_top_34_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_34_reg);
	M8P_rthw_post_top_kpost_top_38_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_38_reg);
	M8P_rthw_post_top_kpost_top_3c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_3C_reg);
	M8P_rthw_post_top_kpost_top_40_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_40_reg);
	M8P_rthw_post_top_kpost_top_44_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_44_reg);
	M8P_rthw_post_top_kpost_top_48_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_48_reg);
	M8P_rthw_post_top_kpost_top_4c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_4C_reg);
	M8P_rthw_post_top_kpost_top_50_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_50_reg);
	M8P_rthw_post_top_kpost_top_54_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_54_reg);
	M8P_rthw_post_top_kpost_top_58_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_58_reg);
	M8P_rthw_post_top_kpost_top_5c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_5C_reg);
	M8P_rthw_post_top_kpost_top_60_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_60_reg);
	M8P_rthw_post_top_kpost_top_64_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_64_reg);
	M8P_rthw_post_top_kpost_top_68_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_68_reg);
	M8P_rthw_post_top_kpost_top_6c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_6C_reg);
	M8P_rthw_post_top_kpost_top_88_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_88_reg);
	M8P_rthw_post_top_kpost_top_8c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_8C_reg);
	M8P_rthw_post_top_kpost_top_90_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_90_reg);
	M8P_rthw_post_top_kpost_top_94_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_94_reg);
	M8P_rthw_post_top_kpost_top_98_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_98_reg);
	M8P_rthw_post_top_kpost_top_9c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_9C_reg);
	M8P_rthw_post_top_kpost_top_a0_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_A0_reg);
	M8P_rthw_post_top_kpost_top_a4_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_A4_reg);
	M8P_rthw_post_top_kpost_top_a8_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_A8_reg);
	M8P_rthw_post_top_kpost_top_ac_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_AC_reg);
	M8P_rthw_post_top_kpost_top_b0_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_B0_reg);
	M8P_rthw_post_top_kpost_top_c4_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_C4_reg);
	M8P_rthw_post_top_kpost_top_c8_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_C8_reg);
	M8P_rthw_post_top2_kpost_top2_00_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_00_reg);
	M8P_rthw_post_top2_kpost_top2_04_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_04_reg);
	M8P_rthw_post_top2_kpost_top2_08_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_08_reg);
	M8P_rthw_post_top2_kpost_top2_0c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_0C_reg);
	M8P_rthw_post_top2_kpost_top2_10_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_10_reg);
	M8P_rthw_post_top2_kpost_top2_14_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_14_reg);
	M8P_rthw_post_top2_kpost_top2_18_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_18_reg);
	M8P_rthw_post_top2_kpost_top2_1c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_1C_reg);
	M8P_rthw_post_top2_kpost_top2_20_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_20_reg);
	M8P_rthw_post_top2_kpost_top2_24_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_24_reg);
	M8P_rthw_post_top2_kpost_top2_28_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_28_reg);
	M8P_rthw_post_top2_kpost_top2_2c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_2C_reg);
	M8P_rthw_post_top2_kpost_top2_30_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_30_reg);
	M8P_rthw_post_top2_kpost_top2_34_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_34_reg);
	M8P_rthw_post_top2_kpost_top2_38_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_38_reg);
	M8P_rthw_post_top2_kpost_top2_3c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_3C_reg);
	M8P_rthw_post_top2_kpost_top2_40_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_40_reg);
	M8P_rthw_post_top2_kpost_top2_44_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_44_reg);
	M8P_rthw_post_top2_kpost_top2_48_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_48_reg);
	M8P_rthw_post_top2_kpost_top2_4c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_4C_reg);
	M8P_rthw_post_top2_kpost_top2_50_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_50_reg);
	M8P_rthw_post_top2_kpost_top2_54_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_54_reg);
	M8P_rthw_post_top2_kpost_top2_58_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_58_reg);
	M8P_rthw_post_top2_kpost_top2_5c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_5C_reg);
	M8P_rthw_post_top2_kpost_top2_60_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_60_reg);
	M8P_rthw_post_top2_kpost_top2_64_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_64_reg);
	M8P_rthw_post_top2_kpost_top2_68_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_68_reg);
	M8P_rthw_post_top2_kpost_top2_6c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_6C_reg);
	M8P_rthw_post_top2_kpost_top2_70_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_70_reg);
	M8P_rthw_post_top2_kpost_top2_74_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_74_reg);
	M8P_rthw_post_top2_kpost_top2_78_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_78_reg);
	M8P_rthw_post_top2_kpost_top2_7c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_7C_reg);
	M8P_rthw_post_top2_kpost_top2_80_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_80_reg);
	M8P_rthw_post_top2_kpost_top2_84_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_84_reg);
	M8P_rthw_post_top2_kpost_top2_88_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_88_reg);
	M8P_rthw_post_top2_kpost_top2_8c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_8C_reg);
	M8P_rthw_post_top2_kpost_top2_90_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_90_reg);
	M8P_rthw_post_top2_kpost_top2_94_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_94_reg);
	M8P_rthw_post_top2_kpost_top2_98_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_98_reg);
	M8P_rthw_post_top2_kpost_top2_9c_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_9C_reg);
	M8P_rthw_post_top2_kpost_top2_a0_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_A0_reg);
	M8P_rthw_post_top2_kpost_top2_a4_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_A4_reg);
	M8P_rthw_post_top2_kpost_top2_a8_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_A8_reg);
	M8P_rthw_post_top2_kpost_top2_ac_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_AC_reg);
	M8P_rthw_post_top2_kpost_top2_b0_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_B0_reg);
	M8P_rthw_post_top2_kpost_top2_b4_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_B4_reg);
	M8P_rthw_post_top2_kpost_top2_b8_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_B8_reg);
	M8P_rthw_post_top2_kpost_top2_bc_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_BC_reg);
	M8P_rthw_post_top2_kpost_top2_c0_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_C0_reg);
	M8P_rthw_post_top2_kpost_top2_c4_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_C4_reg);
	M8P_rthw_post_top2_kpost_top2_c8_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_C8_reg);
	M8P_rthw_post_top2_kpost_top2_cc_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_CC_reg);
	M8P_rthw_post_top2_kpost_top2_d0_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_D0_reg);
	M8P_rthw_post_top2_kpost_top2_d4_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_D4_reg);
	M8P_rthw_post_top2_kpost_top2_d8_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_D8_reg);
	M8P_rthw_post_top2_kpost_top2_dc_reg.regValue = rtd_inl(M8P_RTHW_POST_TOP2_KPOST_TOP2_DC_reg);

	if(M8P_rthw_post_top_kpost_top_0c_reg.seg_length == 0 || M8P_rthw_post_top_kpost_top_0c_reg.seg_width == 0){
		M8P_rthw_post_top_kpost_top_0c_reg.seg_length = 24;
		M8P_rthw_post_top_kpost_top_0c_reg.seg_width = 4;
	}
	if(color > 7){
		color = color%8;
	}

	if(data == 0xa){
		data = 'A';
	}
	else if(data == 0xb){
		data = 'B';
	}
	else if(data == 0xc){
		data = 'C';
	}
	else if(data == 0xd){
		data = 'D';
	}
	else if(data == 0xe){
		data = 'E';
	}
	else if(data == 0xf){
		data = 'F';
	}
	else if((data < 10) && (data >= 0)){
		data = data + '0';
	}
	character = Data_Mapping_RTK2885pp(data);

	if(seg_ID == 0)
	{
		M8P_rthw_post_top_kpost_top_ac_reg.seg_en_inst0 = 0;
		M8P_rthw_post_top_kpost_top_60_reg.seg_en_inst1 = 0;
		M8P_rthw_post_top_kpost_top_64_reg.seg_en_inst2 = 0;
		M8P_rthw_post_top_kpost_top_68_reg.seg_en_inst3 = 0;
		M8P_rthw_post_top_kpost_top_6c_reg.seg_en_inst4 = 0;
		M8P_rthw_post_top_kpost_top_b0_reg.seg_en_inst5 = 0;
		M8P_rthw_post_top_kpost_top_c4_reg.seg_en_inst6 = 0;
		M8P_rthw_post_top_kpost_top_c8_reg.seg_en_inst7 = 0;
	}
	else if(seg_ID == 1)
	{
		M8P_rthw_post_top_kpost_top_ac_reg.seg_en_inst0 = 1;
		M8P_rthw_post_top_kpost_top_10_reg.seg_x_inst0 = (x == 0) ? 120 : x;
		M8P_rthw_post_top_kpost_top_10_reg.seg_y_inst0 = (y == 0) ? 40 : y;
		if(char_ID == 0){
			M8P_rthw_post_top_kpost_top_ac_reg.seg_color_char0_inst0 = color;
			M8P_rthw_post_top_kpost_top_44_reg.seg_data_char0_inst0 = character;
		}
		else if(char_ID == 1){
			M8P_rthw_post_top_kpost_top_ac_reg.seg_color_char1_inst0 = color;
			M8P_rthw_post_top_kpost_top_44_reg.seg_data_char1_inst0 = character;
		}
		else if(char_ID == 2){
			M8P_rthw_post_top_kpost_top_ac_reg.seg_color_char2_inst0 = color;
			M8P_rthw_post_top_kpost_top_48_reg.seg_data_char2_inst0 = character;
		}
		else if(char_ID == 3){
			M8P_rthw_post_top_kpost_top_ac_reg.seg_color_char3_inst0 = color;
			M8P_rthw_post_top_kpost_top_48_reg.seg_data_char3_inst0 = character;
		}
		else if(char_ID == 4){
			M8P_rthw_post_top_kpost_top_ac_reg.seg_color_char4_inst0 = color;
			M8P_rthw_post_top2_kpost_top2_00_reg.seg_data_char4_inst0 = character;
		}
		else if(char_ID == 5){
			M8P_rthw_post_top_kpost_top_ac_reg.seg_color_char5_inst0 = color;
			M8P_rthw_post_top2_kpost_top2_00_reg.seg_data_char5_inst0 = character;
		}
		else if(char_ID == 6){
			M8P_rthw_post_top_kpost_top_ac_reg.seg_color_char6_inst0 = color;
			M8P_rthw_post_top2_kpost_top2_04_reg.seg_data_char6_inst0 = character;
		}
		else if(char_ID == 7){
			M8P_rthw_post_top_kpost_top_ac_reg.seg_color_char7_inst0 = color;
			M8P_rthw_post_top2_kpost_top2_04_reg.seg_data_char7_inst0 = character;
		}
		else if(char_ID == 8){
			M8P_rthw_post_top2_kpost_top2_c0_reg.seg_color_char8_inst0 = color;
			M8P_rthw_post_top2_kpost_top2_08_reg.seg_data_char8_inst0 = character;
		}
		else if(char_ID == 9){
			M8P_rthw_post_top2_kpost_top2_c0_reg.seg_color_char9_inst0 = color;
			M8P_rthw_post_top2_kpost_top2_08_reg.seg_data_char9_inst0 = character;
		}
		else if(char_ID == 10){
			M8P_rthw_post_top2_kpost_top2_c0_reg.seg_color_char10_inst0 = color;
			M8P_rthw_post_top2_kpost_top2_0c_reg.seg_data_char10_inst0 = character;
		}
		else if(char_ID == 11){
			M8P_rthw_post_top2_kpost_top2_c0_reg.seg_color_char11_inst0 = color;
			M8P_rthw_post_top2_kpost_top2_0c_reg.seg_data_char11_inst0 = character;
		}
		else if(char_ID == 12){
			M8P_rthw_post_top2_kpost_top2_c0_reg.seg_color_char12_inst0 = color;
			M8P_rthw_post_top2_kpost_top2_10_reg.seg_data_char12_inst0 = character;
		}
		else if(char_ID == 13){
			M8P_rthw_post_top2_kpost_top2_c0_reg.seg_color_char13_inst0 = color;
			M8P_rthw_post_top2_kpost_top2_10_reg.seg_data_char13_inst0 = character;
		}
		else if(char_ID == 14){
			M8P_rthw_post_top2_kpost_top2_c0_reg.seg_color_char14_inst0 = color;
			M8P_rthw_post_top2_kpost_top2_14_reg.seg_data_char14_inst0 = character;
		}
		else if(char_ID == 15){
			M8P_rthw_post_top2_kpost_top2_c0_reg.seg_color_char15_inst0 = color;
			M8P_rthw_post_top2_kpost_top2_14_reg.seg_data_char15_inst0 = character;
		}
	}
	else if(seg_ID == 2)
	{
		M8P_rthw_post_top_kpost_top_60_reg.seg_en_inst1 = 1;
		M8P_rthw_post_top_kpost_top_28_reg.seg_x_inst1 = (x == 0) ? 120 : x;
		M8P_rthw_post_top_kpost_top_28_reg.seg_y_inst1 = (y == 0) ? 200 : y;
		if(char_ID == 0){
			M8P_rthw_post_top_kpost_top_60_reg.seg_color_char0_inst1 = color;
			M8P_rthw_post_top_kpost_top_4c_reg.seg_data_char0_inst1 = character;
		}
		else if(char_ID == 1){
			M8P_rthw_post_top_kpost_top_60_reg.seg_color_char1_inst1 = color;
			M8P_rthw_post_top_kpost_top_4c_reg.seg_data_char1_inst1 = character;
		}
		else if(char_ID == 2){
			M8P_rthw_post_top_kpost_top_60_reg.seg_color_char2_inst1 = color;
			M8P_rthw_post_top_kpost_top_50_reg.seg_data_char2_inst1 = character;
		}
		else if(char_ID == 3){
			M8P_rthw_post_top_kpost_top_60_reg.seg_color_char3_inst1 = color;
			M8P_rthw_post_top_kpost_top_50_reg.seg_data_char3_inst1 = character;
		}
		else if(char_ID == 4){
			M8P_rthw_post_top_kpost_top_60_reg.seg_color_char4_inst1 = color;
			M8P_rthw_post_top2_kpost_top2_18_reg.seg_data_char4_inst1 = character;
		}
		else if(char_ID == 5){
			M8P_rthw_post_top_kpost_top_60_reg.seg_color_char5_inst1 = color;
			M8P_rthw_post_top2_kpost_top2_18_reg.seg_data_char5_inst1 = character;
		}
		else if(char_ID == 6){
			M8P_rthw_post_top_kpost_top_60_reg.seg_color_char6_inst1 = color;
			M8P_rthw_post_top2_kpost_top2_1c_reg.seg_data_char6_inst1 = character;
		}
		else if(char_ID == 7){
			M8P_rthw_post_top_kpost_top_60_reg.seg_color_char7_inst1 = color;
			M8P_rthw_post_top2_kpost_top2_1c_reg.seg_data_char7_inst1 = character;
		}
		else if(char_ID == 8){
			M8P_rthw_post_top2_kpost_top2_c4_reg.seg_color_char8_inst1 = color;
			M8P_rthw_post_top2_kpost_top2_20_reg.seg_data_char8_inst1 = character;
		}
		else if(char_ID == 9){
			M8P_rthw_post_top2_kpost_top2_c4_reg.seg_color_char9_inst1 = color;
			M8P_rthw_post_top2_kpost_top2_20_reg.seg_data_char9_inst1 = character;
		}
		else if(char_ID == 10){
			M8P_rthw_post_top2_kpost_top2_c4_reg.seg_color_char10_inst1 = color;
			M8P_rthw_post_top2_kpost_top2_24_reg.seg_data_char10_inst1 = character;
		}
		else if(char_ID == 11){
			M8P_rthw_post_top2_kpost_top2_c4_reg.seg_color_char11_inst1 = color;
			M8P_rthw_post_top2_kpost_top2_24_reg.seg_data_char11_inst1 = character;
		}
		else if(char_ID == 12){
			M8P_rthw_post_top2_kpost_top2_c4_reg.seg_color_char12_inst1 = color;
			M8P_rthw_post_top2_kpost_top2_28_reg.seg_data_char12_inst1 = character;
		}
		else if(char_ID == 13){
			M8P_rthw_post_top2_kpost_top2_c4_reg.seg_color_char13_inst1 = color;
			M8P_rthw_post_top2_kpost_top2_28_reg.seg_data_char13_inst1 = character;
		}
		else if(char_ID == 14){
			M8P_rthw_post_top2_kpost_top2_c4_reg.seg_color_char14_inst1 = color;
			M8P_rthw_post_top2_kpost_top2_2c_reg.seg_data_char14_inst1 = character;
		}
		else if(char_ID == 15){
			M8P_rthw_post_top2_kpost_top2_c4_reg.seg_color_char15_inst1 = color;
			M8P_rthw_post_top2_kpost_top2_2c_reg.seg_data_char15_inst1 = character;
		}
	}
	else if(seg_ID == 3)
	{
		M8P_rthw_post_top_kpost_top_64_reg.seg_en_inst2 = 1;
		M8P_rthw_post_top_kpost_top_2c_reg.seg_x_inst2 = (x == 0) ? 120 : x;
		M8P_rthw_post_top_kpost_top_2c_reg.seg_y_inst2 = (y == 0) ? 360 : y;
		if(char_ID == 0){
			M8P_rthw_post_top_kpost_top_64_reg.seg_color_char0_inst2 = color;
			M8P_rthw_post_top_kpost_top_54_reg.seg_data_char0_inst2 = character;
		}
		else if(char_ID == 1){
			M8P_rthw_post_top_kpost_top_64_reg.seg_color_char1_inst2 = color;
			M8P_rthw_post_top_kpost_top_54_reg.seg_data_char1_inst2 = character;
		}
		else if(char_ID == 2){
			M8P_rthw_post_top_kpost_top_64_reg.seg_color_char2_inst2 = color;
			M8P_rthw_post_top_kpost_top_58_reg.seg_data_char2_inst2 = character;
		}
		else if(char_ID == 3){
			M8P_rthw_post_top_kpost_top_64_reg.seg_color_char3_inst2 = color;
			M8P_rthw_post_top_kpost_top_58_reg.seg_data_char3_inst2 = character;
		}
		else if(char_ID == 4){
			M8P_rthw_post_top_kpost_top_64_reg.seg_color_char4_inst2 = color;
			M8P_rthw_post_top2_kpost_top2_30_reg.seg_data_char4_inst2 = character;
		}
		else if(char_ID == 5){
			M8P_rthw_post_top_kpost_top_64_reg.seg_color_char5_inst2 = color;
			M8P_rthw_post_top2_kpost_top2_30_reg.seg_data_char5_inst2 = character;
		}
		else if(char_ID == 6){
			M8P_rthw_post_top_kpost_top_64_reg.seg_color_char6_inst2 = color;
			M8P_rthw_post_top2_kpost_top2_34_reg.seg_data_char6_inst2 = character;
		}
		else if(char_ID == 7){
			M8P_rthw_post_top_kpost_top_64_reg.seg_color_char7_inst2 = color;
			M8P_rthw_post_top2_kpost_top2_34_reg.seg_data_char7_inst2 = character;
		}
		else if(char_ID == 8){
			M8P_rthw_post_top2_kpost_top2_c8_reg.seg_color_char8_inst2 = color;
			M8P_rthw_post_top2_kpost_top2_38_reg.seg_data_char8_inst2 = character;
		}
		else if(char_ID == 9){
			M8P_rthw_post_top2_kpost_top2_c8_reg.seg_color_char9_inst2 = color;
			M8P_rthw_post_top2_kpost_top2_38_reg.seg_data_char9_inst2 = character;
		}
		else if(char_ID == 10){
			M8P_rthw_post_top2_kpost_top2_c8_reg.seg_color_char10_inst2 = color;
			M8P_rthw_post_top2_kpost_top2_3c_reg.seg_data_char10_inst2 = character;
		}
		else if(char_ID == 11){
			M8P_rthw_post_top2_kpost_top2_c8_reg.seg_color_char11_inst2 = color;
			M8P_rthw_post_top2_kpost_top2_3c_reg.seg_data_char11_inst2 = character;
		}
		else if(char_ID == 12){
			M8P_rthw_post_top2_kpost_top2_c8_reg.seg_color_char12_inst2 = color;
			M8P_rthw_post_top2_kpost_top2_40_reg.seg_data_char12_inst2 = character;
		}
		else if(char_ID == 13){
			M8P_rthw_post_top2_kpost_top2_c8_reg.seg_color_char13_inst2 = color;
			M8P_rthw_post_top2_kpost_top2_40_reg.seg_data_char13_inst2 = character;
		}
		else if(char_ID == 14){
			M8P_rthw_post_top2_kpost_top2_c8_reg.seg_color_char14_inst2 = color;
			M8P_rthw_post_top2_kpost_top2_44_reg.seg_data_char14_inst2 = character;
		}
		else if(char_ID == 15){
			M8P_rthw_post_top2_kpost_top2_c8_reg.seg_color_char15_inst2 = color;
			M8P_rthw_post_top2_kpost_top2_44_reg.seg_data_char15_inst2 = character;
		}
	}
	else if(seg_ID == 4)
	{
		M8P_rthw_post_top_kpost_top_68_reg.seg_en_inst3 = 1;
		M8P_rthw_post_top_kpost_top_30_reg.seg_x_inst3 = (x == 0) ? 120 : x;
		M8P_rthw_post_top_kpost_top_30_reg.seg_y_inst3 = (y == 0) ? 520 : y;
		if(char_ID == 0){
			M8P_rthw_post_top_kpost_top_68_reg.seg_color_char0_inst3 = color;
			M8P_rthw_post_top_kpost_top_5c_reg.seg_data_char0_inst3 = character;
		}
		else if(char_ID == 1){
			M8P_rthw_post_top_kpost_top_68_reg.seg_color_char1_inst3 = color;
			M8P_rthw_post_top_kpost_top_5c_reg.seg_data_char1_inst3 = character;
		}
		else if(char_ID == 2){
			M8P_rthw_post_top_kpost_top_68_reg.seg_color_char2_inst3 = color;
			M8P_rthw_post_top_kpost_top_88_reg.seg_data_char2_inst3 = character;
		}
		else if(char_ID == 3){
			M8P_rthw_post_top_kpost_top_68_reg.seg_color_char3_inst3 = color;
			M8P_rthw_post_top_kpost_top_88_reg.seg_data_char3_inst3 = character;
		}
		else if(char_ID == 4){
			M8P_rthw_post_top_kpost_top_68_reg.seg_color_char4_inst3 = color;
			M8P_rthw_post_top2_kpost_top2_48_reg.seg_data_char4_inst3 = character;
		}
		else if(char_ID == 5){
			M8P_rthw_post_top_kpost_top_68_reg.seg_color_char5_inst3 = color;
			M8P_rthw_post_top2_kpost_top2_48_reg.seg_data_char5_inst3 = character;
		}
		else if(char_ID == 6){
			M8P_rthw_post_top_kpost_top_68_reg.seg_color_char6_inst3 = color;
			M8P_rthw_post_top2_kpost_top2_4c_reg.seg_data_char6_inst3 = character;
		}
		else if(char_ID == 7){
			M8P_rthw_post_top_kpost_top_68_reg.seg_color_char7_inst3 = color;
			M8P_rthw_post_top2_kpost_top2_4c_reg.seg_data_char7_inst3 = character;
		}
		else if(char_ID == 8){
			M8P_rthw_post_top2_kpost_top2_cc_reg.seg_color_char8_inst3 = color;
			M8P_rthw_post_top2_kpost_top2_50_reg.seg_data_char8_inst3 = character;
		}
		else if(char_ID == 9){
			M8P_rthw_post_top2_kpost_top2_cc_reg.seg_color_char9_inst3 = color;
			M8P_rthw_post_top2_kpost_top2_50_reg.seg_data_char9_inst3 = character;
		}
		else if(char_ID == 10){
			M8P_rthw_post_top2_kpost_top2_cc_reg.seg_color_char10_inst3 = color;
			M8P_rthw_post_top2_kpost_top2_54_reg.seg_data_char10_inst3 = character;
		}
		else if(char_ID == 11){
			M8P_rthw_post_top2_kpost_top2_cc_reg.seg_color_char11_inst3 = color;
			M8P_rthw_post_top2_kpost_top2_54_reg.seg_data_char11_inst3 = character;
		}
		else if(char_ID == 12){
			M8P_rthw_post_top2_kpost_top2_cc_reg.seg_color_char12_inst3 = color;
			M8P_rthw_post_top2_kpost_top2_58_reg.seg_data_char12_inst3 = character;
		}
		else if(char_ID == 13){
			M8P_rthw_post_top2_kpost_top2_cc_reg.seg_color_char13_inst3 = color;
			M8P_rthw_post_top2_kpost_top2_58_reg.seg_data_char13_inst3 = character;
		}
		else if(char_ID == 14){
			M8P_rthw_post_top2_kpost_top2_cc_reg.seg_color_char14_inst3 = color;
			M8P_rthw_post_top2_kpost_top2_5c_reg.seg_data_char14_inst3 = character;
		}
		else if(char_ID == 15){
			M8P_rthw_post_top2_kpost_top2_cc_reg.seg_color_char15_inst3 = color;
			M8P_rthw_post_top2_kpost_top2_5c_reg.seg_data_char15_inst3 = character;
		}
	}
	else if(seg_ID == 5)
	{
		M8P_rthw_post_top_kpost_top_6c_reg.seg_en_inst4 = 1;
		M8P_rthw_post_top_kpost_top_34_reg.seg_x_inst4 = (x == 0) ? 120 : x;
		M8P_rthw_post_top_kpost_top_34_reg.seg_y_inst4 = (y == 0) ? 680 : y;
		if(char_ID == 0){
			M8P_rthw_post_top_kpost_top_6c_reg.seg_color_char0_inst4 = color;
			M8P_rthw_post_top_kpost_top_8c_reg.seg_data_char0_inst4 = character;
		}
		else if(char_ID == 1){
			M8P_rthw_post_top_kpost_top_6c_reg.seg_color_char1_inst4 = color;
			M8P_rthw_post_top_kpost_top_8c_reg.seg_data_char1_inst4 = character;
		}
		else if(char_ID == 2){
			M8P_rthw_post_top_kpost_top_6c_reg.seg_color_char2_inst4 = color;
			M8P_rthw_post_top_kpost_top_90_reg.seg_data_char2_inst4 = character;
		}
		else if(char_ID == 3){
			M8P_rthw_post_top_kpost_top_6c_reg.seg_color_char3_inst4 = color;
			M8P_rthw_post_top_kpost_top_90_reg.seg_data_char3_inst4 = character;
		}
		else if(char_ID == 4){
			M8P_rthw_post_top_kpost_top_6c_reg.seg_color_char4_inst4 = color;
			M8P_rthw_post_top2_kpost_top2_60_reg.seg_data_char4_inst4 = character;
		}
		else if(char_ID == 5){
			M8P_rthw_post_top_kpost_top_6c_reg.seg_color_char5_inst4 = color;
			M8P_rthw_post_top2_kpost_top2_60_reg.seg_data_char5_inst4 = character;
		}
		else if(char_ID == 6){
			M8P_rthw_post_top_kpost_top_6c_reg.seg_color_char6_inst4 = color;
			M8P_rthw_post_top2_kpost_top2_64_reg.seg_data_char6_inst4 = character;
		}
		else if(char_ID == 7){
			M8P_rthw_post_top_kpost_top_6c_reg.seg_color_char7_inst4 = color;
			M8P_rthw_post_top2_kpost_top2_64_reg.seg_data_char7_inst4 = character;
		}
		else if(char_ID == 8){
			M8P_rthw_post_top2_kpost_top2_d0_reg.seg_color_char8_inst4 = color;
			M8P_rthw_post_top2_kpost_top2_68_reg.seg_data_char8_inst4 = character;
		}
		else if(char_ID == 9){
			M8P_rthw_post_top2_kpost_top2_d0_reg.seg_color_char9_inst4 = color;
			M8P_rthw_post_top2_kpost_top2_68_reg.seg_data_char9_inst4 = character;
		}
		else if(char_ID == 10){
			M8P_rthw_post_top2_kpost_top2_d0_reg.seg_color_char10_inst4 = color;
			M8P_rthw_post_top2_kpost_top2_6c_reg.seg_data_char10_inst4 = character;
		}
		else if(char_ID == 11){
			M8P_rthw_post_top2_kpost_top2_d0_reg.seg_color_char11_inst4 = color;
			M8P_rthw_post_top2_kpost_top2_6c_reg.seg_data_char11_inst4 = character;
		}
		else if(char_ID == 12){
			M8P_rthw_post_top2_kpost_top2_d0_reg.seg_color_char12_inst4 = color;
			M8P_rthw_post_top2_kpost_top2_70_reg.seg_data_char12_inst4 = character;
		}
		else if(char_ID == 13){
			M8P_rthw_post_top2_kpost_top2_d0_reg.seg_color_char13_inst4 = color;
			M8P_rthw_post_top2_kpost_top2_70_reg.seg_data_char13_inst4 = character;
		}
		else if(char_ID == 14){
			M8P_rthw_post_top2_kpost_top2_d0_reg.seg_color_char14_inst4 = color;
			M8P_rthw_post_top2_kpost_top2_74_reg.seg_data_char14_inst4 = character;
		}
		else if(char_ID == 15){
			M8P_rthw_post_top2_kpost_top2_d0_reg.seg_color_char15_inst4 = color;
			M8P_rthw_post_top2_kpost_top2_74_reg.seg_data_char15_inst4 = character;
		}
	}
	else if(seg_ID == 6)
	{
		M8P_rthw_post_top_kpost_top_b0_reg.seg_en_inst5 = 1;
		M8P_rthw_post_top_kpost_top_38_reg.seg_x_inst5 = (x == 0) ? 120 : x;
		M8P_rthw_post_top_kpost_top_38_reg.seg_y_inst5 = (y == 0) ? 840 : y;
		if(char_ID == 0){
			M8P_rthw_post_top_kpost_top_b0_reg.seg_color_char0_inst5 = color;
			M8P_rthw_post_top_kpost_top_94_reg.seg_data_char0_inst5 = character;
		}
		else if(char_ID == 1){
			M8P_rthw_post_top_kpost_top_b0_reg.seg_color_char1_inst5 = color;
			M8P_rthw_post_top_kpost_top_94_reg.seg_data_char1_inst5 = character;
		}
		else if(char_ID == 2){
			M8P_rthw_post_top_kpost_top_b0_reg.seg_color_char2_inst5 = color;
			M8P_rthw_post_top_kpost_top_98_reg.seg_data_char2_inst5 = character;
		}
		else if(char_ID == 3){
			M8P_rthw_post_top_kpost_top_b0_reg.seg_color_char3_inst5 = color;
			M8P_rthw_post_top_kpost_top_98_reg.seg_data_char3_inst5 = character;
		}
		else if(char_ID == 4){
			M8P_rthw_post_top_kpost_top_b0_reg.seg_color_char4_inst5 = color;
			M8P_rthw_post_top2_kpost_top2_78_reg.seg_data_char4_inst5 = character;
		}
		else if(char_ID == 5){
			M8P_rthw_post_top_kpost_top_b0_reg.seg_color_char5_inst5 = color;
			M8P_rthw_post_top2_kpost_top2_78_reg.seg_data_char5_inst5 = character;
		}
		else if(char_ID == 6){
			M8P_rthw_post_top_kpost_top_b0_reg.seg_color_char6_inst5 = color;
			M8P_rthw_post_top2_kpost_top2_7c_reg.seg_data_char6_inst5 = character;
		}
		else if(char_ID == 7){
			M8P_rthw_post_top_kpost_top_b0_reg.seg_color_char7_inst5 = color;
			M8P_rthw_post_top2_kpost_top2_7c_reg.seg_data_char7_inst5 = character;
		}
		else if(char_ID == 8){
			M8P_rthw_post_top2_kpost_top2_d4_reg.seg_color_char8_inst5 = color;
			M8P_rthw_post_top2_kpost_top2_80_reg.seg_data_char8_inst5 = character;
		}
		else if(char_ID == 9){
			M8P_rthw_post_top2_kpost_top2_d4_reg.seg_color_char9_inst5 = color;
			M8P_rthw_post_top2_kpost_top2_80_reg.seg_data_char9_inst5 = character;
		}
		else if(char_ID == 10){
			M8P_rthw_post_top2_kpost_top2_d4_reg.seg_color_char10_inst5 = color;
			M8P_rthw_post_top2_kpost_top2_84_reg.seg_data_char10_inst5 = character;
		}
		else if(char_ID == 11){
			M8P_rthw_post_top2_kpost_top2_d4_reg.seg_color_char11_inst5 = color;
			M8P_rthw_post_top2_kpost_top2_84_reg.seg_data_char11_inst5 = character;
		}
		else if(char_ID == 12){
			M8P_rthw_post_top2_kpost_top2_d4_reg.seg_color_char12_inst5 = color;
			M8P_rthw_post_top2_kpost_top2_88_reg.seg_data_char12_inst5 = character;
		}
		else if(char_ID == 13){
			M8P_rthw_post_top2_kpost_top2_d4_reg.seg_color_char13_inst5 = color;
			M8P_rthw_post_top2_kpost_top2_88_reg.seg_data_char13_inst5 = character;
		}
		else if(char_ID == 14){
			M8P_rthw_post_top2_kpost_top2_d4_reg.seg_color_char14_inst5 = color;
			M8P_rthw_post_top2_kpost_top2_8c_reg.seg_data_char14_inst5 = character;
		}
		else if(char_ID == 15){
			M8P_rthw_post_top2_kpost_top2_d4_reg.seg_color_char15_inst5 = color;
			M8P_rthw_post_top2_kpost_top2_8c_reg.seg_data_char15_inst5 = character;
		}
	}
	else if(seg_ID == 7)
	{
		M8P_rthw_post_top_kpost_top_c4_reg.seg_en_inst6 = 1;
		M8P_rthw_post_top_kpost_top_3c_reg.seg_x_inst6 = (x == 0) ? 120 : x;
		M8P_rthw_post_top_kpost_top_3c_reg.seg_y_inst6 = (y == 0) ? 1000 : y;
		if(char_ID == 0){
			M8P_rthw_post_top_kpost_top_c4_reg.seg_color_char0_inst6 = color;
			M8P_rthw_post_top_kpost_top_9c_reg.seg_data_char0_inst6 = character;
		}
		else if(char_ID == 1){
			M8P_rthw_post_top_kpost_top_c4_reg.seg_color_char1_inst6 = color;
			M8P_rthw_post_top_kpost_top_9c_reg.seg_data_char1_inst6 = character;
		}
		else if(char_ID == 2){
			M8P_rthw_post_top_kpost_top_c4_reg.seg_color_char2_inst6 = color;
			M8P_rthw_post_top_kpost_top_a0_reg.seg_data_char2_inst6 = character;
		}
		else if(char_ID == 3){
			M8P_rthw_post_top_kpost_top_c4_reg.seg_color_char3_inst6 = color;
			M8P_rthw_post_top_kpost_top_a0_reg.seg_data_char3_inst6 = character;
		}
		else if(char_ID == 4){
			M8P_rthw_post_top_kpost_top_c4_reg.seg_color_char4_inst6 = color;
			M8P_rthw_post_top2_kpost_top2_90_reg.seg_data_char4_inst6 = character;
		}
		else if(char_ID == 5){
			M8P_rthw_post_top_kpost_top_c4_reg.seg_color_char5_inst6 = color;
			M8P_rthw_post_top2_kpost_top2_90_reg.seg_data_char5_inst6 = character;
		}
		else if(char_ID == 6){
			M8P_rthw_post_top_kpost_top_c4_reg.seg_color_char6_inst6 = color;
			M8P_rthw_post_top2_kpost_top2_94_reg.seg_data_char6_inst6 = character;
		}
		else if(char_ID == 7){
			M8P_rthw_post_top_kpost_top_c4_reg.seg_color_char7_inst6 = color;
			M8P_rthw_post_top2_kpost_top2_94_reg.seg_data_char7_inst6 = character;
		}
		else if(char_ID == 8){
			M8P_rthw_post_top2_kpost_top2_d8_reg.seg_color_char8_inst6 = color;
			M8P_rthw_post_top2_kpost_top2_98_reg.seg_data_char8_inst6 = character;
		}
		else if(char_ID == 9){
			M8P_rthw_post_top2_kpost_top2_d8_reg.seg_color_char9_inst6 = color;
			M8P_rthw_post_top2_kpost_top2_98_reg.seg_data_char9_inst6 = character;
		}
		else if(char_ID == 10){
			M8P_rthw_post_top2_kpost_top2_d8_reg.seg_color_char10_inst6 = color;
			M8P_rthw_post_top2_kpost_top2_9c_reg.seg_data_char10_inst6 = character;
		}
		else if(char_ID == 11){
			M8P_rthw_post_top2_kpost_top2_d8_reg.seg_color_char11_inst6 = color;
			M8P_rthw_post_top2_kpost_top2_9c_reg.seg_data_char11_inst6 = character;
		}
		else if(char_ID == 12){
			M8P_rthw_post_top2_kpost_top2_d8_reg.seg_color_char12_inst6 = color;
			M8P_rthw_post_top2_kpost_top2_a0_reg.seg_data_char12_inst6 = character;
		}
		else if(char_ID == 13){
			M8P_rthw_post_top2_kpost_top2_d8_reg.seg_color_char13_inst6 = color;
			M8P_rthw_post_top2_kpost_top2_a0_reg.seg_data_char13_inst6 = character;
		}
		else if(char_ID == 14){
			M8P_rthw_post_top2_kpost_top2_d8_reg.seg_color_char14_inst6 = color;
			M8P_rthw_post_top2_kpost_top2_a4_reg.seg_data_char14_inst6 = character;
		}
		else if(char_ID == 15){
			M8P_rthw_post_top2_kpost_top2_d8_reg.seg_color_char15_inst6 = color;
			M8P_rthw_post_top2_kpost_top2_a4_reg.seg_data_char15_inst6 = character;
		}
	}
	else if(seg_ID == 8)
	{
		M8P_rthw_post_top_kpost_top_c8_reg.seg_en_inst7 = 1;
		M8P_rthw_post_top_kpost_top_40_reg.seg_x_inst7 = (x == 0) ? 120 : x;
		M8P_rthw_post_top_kpost_top_40_reg.seg_y_inst7 = (y == 0) ? 1160 : y;
		if(char_ID == 0){
			M8P_rthw_post_top_kpost_top_c8_reg.seg_color_char0_inst7 = color;
			M8P_rthw_post_top_kpost_top_a4_reg.seg_data_char0_inst7 = character;
		}
		else if(char_ID == 1){
			M8P_rthw_post_top_kpost_top_c8_reg.seg_color_char1_inst7 = color;
			M8P_rthw_post_top_kpost_top_a4_reg.seg_data_char1_inst7 = character;
		}
		else if(char_ID == 2){
			M8P_rthw_post_top_kpost_top_c8_reg.seg_color_char2_inst7 = color;
			M8P_rthw_post_top_kpost_top_a8_reg.seg_data_char2_inst7 = character;
		}
		else if(char_ID == 3){
			M8P_rthw_post_top_kpost_top_c8_reg.seg_color_char3_inst7 = color;
			M8P_rthw_post_top_kpost_top_a8_reg.seg_data_char3_inst7 = character;
		}
		else if(char_ID == 4){
			M8P_rthw_post_top_kpost_top_c8_reg.seg_color_char4_inst7 = color;
			M8P_rthw_post_top2_kpost_top2_a8_reg.seg_data_char4_inst7 = character;
		}
		else if(char_ID == 5){
			M8P_rthw_post_top_kpost_top_c8_reg.seg_color_char5_inst7 = color;
			M8P_rthw_post_top2_kpost_top2_a8_reg.seg_data_char5_inst7 = character;
		}
		else if(char_ID == 6){
			M8P_rthw_post_top_kpost_top_c8_reg.seg_color_char6_inst7 = color;
			M8P_rthw_post_top2_kpost_top2_ac_reg.seg_data_char6_inst7 = character;
		}
		else if(char_ID == 7){
			M8P_rthw_post_top_kpost_top_c8_reg.seg_color_char7_inst7 = color;
			M8P_rthw_post_top2_kpost_top2_ac_reg.seg_data_char7_inst7 = character;
		}
		else if(char_ID == 8){
			M8P_rthw_post_top2_kpost_top2_dc_reg.seg_color_char8_inst7 = color;
			M8P_rthw_post_top2_kpost_top2_b0_reg.seg_data_char8_inst7 = character;
		}
		else if(char_ID == 9){
			M8P_rthw_post_top2_kpost_top2_dc_reg.seg_color_char9_inst7 = color;
			M8P_rthw_post_top2_kpost_top2_b0_reg.seg_data_char9_inst7 = character;
		}
		else if(char_ID == 10){
			M8P_rthw_post_top2_kpost_top2_dc_reg.seg_color_char10_inst7 = color;
			M8P_rthw_post_top2_kpost_top2_b4_reg.seg_data_char10_inst7 = character;
		}
		else if(char_ID == 11){
			M8P_rthw_post_top2_kpost_top2_dc_reg.seg_color_char11_inst7 = color;
			M8P_rthw_post_top2_kpost_top2_b4_reg.seg_data_char11_inst7 = character;
		}
		else if(char_ID == 12){
			M8P_rthw_post_top2_kpost_top2_dc_reg.seg_color_char12_inst7 = color;
			M8P_rthw_post_top2_kpost_top2_b8_reg.seg_data_char12_inst7 = character;
		}
		else if(char_ID == 13){
			M8P_rthw_post_top2_kpost_top2_dc_reg.seg_color_char13_inst7 = color;
			M8P_rthw_post_top2_kpost_top2_b8_reg.seg_data_char13_inst7 = character;
		}
		else if(char_ID == 14){
			M8P_rthw_post_top2_kpost_top2_dc_reg.seg_color_char14_inst7 = color;
			M8P_rthw_post_top2_kpost_top2_bc_reg.seg_data_char14_inst7 = character;
		}
		else if(char_ID == 15){
			M8P_rthw_post_top2_kpost_top2_dc_reg.seg_color_char15_inst7 = color;
			M8P_rthw_post_top2_kpost_top2_bc_reg.seg_data_char15_inst7 = character;
		}
	}

	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_0C_reg, M8P_rthw_post_top_kpost_top_0c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_10_reg, M8P_rthw_post_top_kpost_top_10_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_28_reg, M8P_rthw_post_top_kpost_top_28_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_2C_reg, M8P_rthw_post_top_kpost_top_2c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_30_reg, M8P_rthw_post_top_kpost_top_30_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_34_reg, M8P_rthw_post_top_kpost_top_34_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_38_reg, M8P_rthw_post_top_kpost_top_38_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_3C_reg, M8P_rthw_post_top_kpost_top_3c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_40_reg, M8P_rthw_post_top_kpost_top_40_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_44_reg, M8P_rthw_post_top_kpost_top_44_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_48_reg, M8P_rthw_post_top_kpost_top_48_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_4C_reg, M8P_rthw_post_top_kpost_top_4c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_50_reg, M8P_rthw_post_top_kpost_top_50_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_54_reg, M8P_rthw_post_top_kpost_top_54_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_58_reg, M8P_rthw_post_top_kpost_top_58_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_5C_reg, M8P_rthw_post_top_kpost_top_5c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_60_reg, M8P_rthw_post_top_kpost_top_60_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_64_reg, M8P_rthw_post_top_kpost_top_64_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_68_reg, M8P_rthw_post_top_kpost_top_68_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_6C_reg, M8P_rthw_post_top_kpost_top_6c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_88_reg, M8P_rthw_post_top_kpost_top_88_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_8C_reg, M8P_rthw_post_top_kpost_top_8c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_90_reg, M8P_rthw_post_top_kpost_top_90_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_94_reg, M8P_rthw_post_top_kpost_top_94_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_98_reg, M8P_rthw_post_top_kpost_top_98_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_9C_reg, M8P_rthw_post_top_kpost_top_9c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_A0_reg, M8P_rthw_post_top_kpost_top_a0_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_A4_reg, M8P_rthw_post_top_kpost_top_a4_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_A8_reg, M8P_rthw_post_top_kpost_top_a8_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_AC_reg, M8P_rthw_post_top_kpost_top_ac_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_B0_reg, M8P_rthw_post_top_kpost_top_b0_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_C4_reg, M8P_rthw_post_top_kpost_top_c4_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_C8_reg, M8P_rthw_post_top_kpost_top_c8_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_00_reg, M8P_rthw_post_top2_kpost_top2_00_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_04_reg, M8P_rthw_post_top2_kpost_top2_04_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_08_reg, M8P_rthw_post_top2_kpost_top2_08_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_0C_reg, M8P_rthw_post_top2_kpost_top2_0c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_10_reg, M8P_rthw_post_top2_kpost_top2_10_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_14_reg, M8P_rthw_post_top2_kpost_top2_14_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_18_reg, M8P_rthw_post_top2_kpost_top2_18_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_1C_reg, M8P_rthw_post_top2_kpost_top2_1c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_20_reg, M8P_rthw_post_top2_kpost_top2_20_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_24_reg, M8P_rthw_post_top2_kpost_top2_24_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_28_reg, M8P_rthw_post_top2_kpost_top2_28_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_2C_reg, M8P_rthw_post_top2_kpost_top2_2c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_30_reg, M8P_rthw_post_top2_kpost_top2_30_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_34_reg, M8P_rthw_post_top2_kpost_top2_34_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_38_reg, M8P_rthw_post_top2_kpost_top2_38_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_3C_reg, M8P_rthw_post_top2_kpost_top2_3c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_40_reg, M8P_rthw_post_top2_kpost_top2_40_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_44_reg, M8P_rthw_post_top2_kpost_top2_44_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_48_reg, M8P_rthw_post_top2_kpost_top2_48_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_4C_reg, M8P_rthw_post_top2_kpost_top2_4c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_50_reg, M8P_rthw_post_top2_kpost_top2_50_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_54_reg, M8P_rthw_post_top2_kpost_top2_54_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_58_reg, M8P_rthw_post_top2_kpost_top2_58_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_5C_reg, M8P_rthw_post_top2_kpost_top2_5c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_60_reg, M8P_rthw_post_top2_kpost_top2_60_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_64_reg, M8P_rthw_post_top2_kpost_top2_64_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_68_reg, M8P_rthw_post_top2_kpost_top2_68_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_6C_reg, M8P_rthw_post_top2_kpost_top2_6c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_70_reg, M8P_rthw_post_top2_kpost_top2_70_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_74_reg, M8P_rthw_post_top2_kpost_top2_74_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_78_reg, M8P_rthw_post_top2_kpost_top2_78_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_7C_reg, M8P_rthw_post_top2_kpost_top2_7c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_80_reg, M8P_rthw_post_top2_kpost_top2_80_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_84_reg, M8P_rthw_post_top2_kpost_top2_84_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_88_reg, M8P_rthw_post_top2_kpost_top2_88_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_8C_reg, M8P_rthw_post_top2_kpost_top2_8c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_90_reg, M8P_rthw_post_top2_kpost_top2_90_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_94_reg, M8P_rthw_post_top2_kpost_top2_94_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_98_reg, M8P_rthw_post_top2_kpost_top2_98_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_9C_reg, M8P_rthw_post_top2_kpost_top2_9c_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_A0_reg, M8P_rthw_post_top2_kpost_top2_a0_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_A4_reg, M8P_rthw_post_top2_kpost_top2_a4_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_A8_reg, M8P_rthw_post_top2_kpost_top2_a8_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_AC_reg, M8P_rthw_post_top2_kpost_top2_ac_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_B0_reg, M8P_rthw_post_top2_kpost_top2_b0_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_B4_reg, M8P_rthw_post_top2_kpost_top2_b4_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_B8_reg, M8P_rthw_post_top2_kpost_top2_b8_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_BC_reg, M8P_rthw_post_top2_kpost_top2_bc_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_C0_reg, M8P_rthw_post_top2_kpost_top2_c0_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_C4_reg, M8P_rthw_post_top2_kpost_top2_c4_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_C8_reg, M8P_rthw_post_top2_kpost_top2_c8_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_CC_reg, M8P_rthw_post_top2_kpost_top2_cc_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_D0_reg, M8P_rthw_post_top2_kpost_top2_d0_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_D4_reg, M8P_rthw_post_top2_kpost_top2_d4_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_D8_reg, M8P_rthw_post_top2_kpost_top2_d8_reg.regValue);
	rtd_outl(M8P_RTHW_POST_TOP2_KPOST_TOP2_DC_reg, M8P_rthw_post_top2_kpost_top2_dc_reg.regValue);
}

VOID Wrt_16segment_show_RTK2885pp(const _PARAM_WRT_COM_REG *pParam)
{
	unsigned int seg_en = 0, u8_i = 0, u8_j = 0;
	unsigned short data_val = 0;
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned char cur_cadence_id = s_pContext->_output_filmDetectctrl.u8_cur_cadence_Id[_FILM_ALL];
	unsigned char det_cadence_id = s_pContext->_output_filmDetectctrl.u8_det_cadence_Id[_FILM_ALL];
	unsigned char motion_seq = (s_pContext->_output_filmDetectctrl.u32_Mot_sequence[_FILM_ALL] & 0x1);
	unsigned int me_write = 0, me1_read_i = 0, me1_read_p = 0, me2_read_i = 0, me2_read_p = 0;
	unsigned int mc_write = 0, mc_read_i = 0, mc_read_p = 0;
	unsigned int me15_read_i = 0, me15_read_p = 0;
	unsigned int me2_phase = 0, mc_phase = 0;
	unsigned int in_phase = 0, out_phase = 0;
	unsigned int in_film_phase = 0, out_film_phase = 0;
	unsigned int me1_last = 0; // , u8_k = 0;
	unsigned int idx_me1_pfv_w = 0, idx_me15_todh_w = 0, idx_mvinfo_w = 0, me15_tomclogo_w = 0;
	M8P_rthw_phase_phase_90_RBUS M8P_rthw_phase_phase_90_reg;
	M8P_rthw_phase_phase_94_RBUS M8P_rthw_phase_phase_94_reg;
	M8P_rthw_phase_phase_98_RBUS M8P_rthw_phase_phase_98_reg;
	M8P_rthw_phase_phase_9c_RBUS M8P_rthw_phase_phase_9c_reg;
	unsigned int me_write_v2 = 0, me1_read_i_v2 = 0, me1_read_p_v2 = 0, me2_read_i_v2 = 0, me2_read_p_v2 = 0;
	unsigned int mc_write_v2 = 0, mc_read_i_v2 = 0, mc_read_p_v2 = 0;
	unsigned int me15_read_i_v2 = 0, me15_read_p_v2 = 0;
	unsigned int me2_phase_v2 = 0, mc_phase_v2 = 0;
	unsigned int in_phase_v2 = 0, out_phase_v2 = 0;
	unsigned int in_film_phase_v2 = 0, out_film_phase_v2 = 0;
	unsigned int me1_last_v2 = 0; // , u8_k = 0;
	unsigned int idx_me1_pfv_w_v2 = 0, idx_me15_todh_w_v2 = 0, idx_mvinfo_w_v2 = 0, me15_tomclogo_w_v2 = 0;
	unsigned int dbg_seg_show_en, reset_en;

	//seg_en = (rtd_inl(HARDWARE_HARDWARE_63_reg)>>28)&0x1;
	ReadRegister(HARDWARE_HARDWARE_63_reg, 28, 28, &seg_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_00_reg, 31, 31, &dbg_seg_show_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_00_reg, 30, 30, &reset_en);
	M8P_rthw_phase_phase_90_reg.regValue = rtd_inl(M8P_RTHW_PHASE_phase_90_reg);
	M8P_rthw_phase_phase_94_reg.regValue = rtd_inl(M8P_RTHW_PHASE_phase_94_reg);
	M8P_rthw_phase_phase_98_reg.regValue = rtd_inl(M8P_RTHW_PHASE_phase_98_reg);
	M8P_rthw_phase_phase_9c_reg.regValue = rtd_inl(M8P_RTHW_PHASE_phase_9C_reg);
	
	out_film_phase = M8P_rthw_phase_phase_94_reg.regr_table_out_film_phase; // ReadRegister(M8P_RTHW_PHASE_phase_94_reg, 16, 21, &out_film_phase);
	out_phase = M8P_rthw_phase_phase_94_reg.regr_table_out_phase; // ReadRegister(M8P_RTHW_PHASE_phase_A8_reg, 24, 29, &out_phase);
	in_film_phase = M8P_rthw_phase_phase_90_reg.regr_table_in_film_phase; // ReadRegister(M8P_RTHW_PHASE_phase_A4_reg, 20, 25, &in_film_phase);
	in_phase = M8P_rthw_phase_phase_90_reg.regr_table_in_phase; // ReadRegister(M8P_RTHW_PHASE_phase_A4_reg, 16, 19, &in_phase);
	me_write = M8P_rthw_phase_phase_90_reg.regr_table_in_me_index; // ReadRegister(M8P_RTHW_PHASE_phase_A4_reg, 12, 15, &me_write);
	me1_read_i = M8P_rthw_phase_phase_98_reg.regr_table_me1_i_idx; // ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 28, 31, &me1_read_i);
	me1_read_p = M8P_rthw_phase_phase_98_reg.regr_table_me1_p_idx; // ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 24, 27, &me1_read_p);
	me15_read_i = M8P_rthw_phase_phase_98_reg.regr_table_me15_i_idx; // ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 20, 23, &me15_read_i);
	me15_read_p = M8P_rthw_phase_phase_98_reg.regr_table_me15_p_idx; // ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 16, 19, &me15_read_p);
	me2_read_i = M8P_rthw_phase_phase_98_reg.regr_table_me2_i_idx; // ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 12, 15, &me2_read_i);
	me2_read_p = M8P_rthw_phase_phase_98_reg.regr_table_me2_p_idx; // ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 8, 11, &me2_read_p);
	mc_write = M8P_rthw_phase_phase_90_reg.regr_table_in_mc_index; // ReadRegister(M8P_RTHW_PHASE_phase_A4_reg, 4, 7, &mc_write);
	mc_read_i = M8P_rthw_phase_phase_94_reg.regr_table_mc_i_idx; // ReadRegister(M8P_RTHW_PHASE_phase_A8_reg, 12, 15, &mc_read_i);
	mc_read_p = M8P_rthw_phase_phase_94_reg.regr_table_mc_p_idx; // ReadRegister(M8P_RTHW_PHASE_phase_A8_reg, 8, 11, &mc_read_p);
	me2_phase = M8P_rthw_phase_phase_98_reg.regr_table_me2_phase; // ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 0, 6, &me2_phase);
	mc_phase = M8P_rthw_phase_phase_94_reg.regr_table_mc_phase; // ReadRegister(M8P_RTHW_PHASE_phase_A8_reg, 0, 6, &mc_phase);
	me1_last = M8P_rthw_phase_phase_98_reg.regr_table_me1_last; // ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 7, 7, &me1_last);
	idx_me1_pfv_w = M8P_rthw_phase_phase_9c_reg.regr_table_idx_me1_pfv_w;
	idx_me15_todh_w = M8P_rthw_phase_phase_9c_reg.regr_table_idx_me15_todh_w;
	idx_mvinfo_w = M8P_rthw_phase_phase_9c_reg.regr_table_idx_mvinfo_w;
	me15_tomclogo_w = M8P_rthw_phase_phase_9c_reg.regr_table_me15_tomclogo_w;

	ReadRegister(M8P_RTHW_PHASE_phase_94_reg-(0x00008000), 16, 21, &out_film_phase_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_94_reg-(0x00008000), 24, 29, &out_phase_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_90_reg-(0x00008000), 20, 25, &in_film_phase_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_90_reg-(0x00008000), 16, 19, &in_phase_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_90_reg-(0x00008000), 12, 15, &me_write_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_98_reg-(0x00008000), 28, 31, &me1_read_i_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_98_reg-(0x00008000), 24, 27, &me1_read_p_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_98_reg-(0x00008000), 20, 23, &me15_read_i_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_98_reg-(0x00008000), 16, 19, &me15_read_p_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_98_reg-(0x00008000), 12, 15, &me2_read_i_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_98_reg-(0x00008000), 8, 11, &me2_read_p_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_90_reg-(0x00008000), 4, 7, &mc_write_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_94_reg-(0x00008000), 12, 15, &mc_read_i_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_94_reg-(0x00008000), 8, 11, &mc_read_p_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_98_reg-(0x00008000), 0, 6, &me2_phase_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_94_reg-(0x00008000), 0, 6, &mc_phase_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_98_reg-(0x00008000), 7, 7, &me1_last_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_9C_reg-(0x00008000), 30, 31, &idx_me1_pfv_w_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_9C_reg-(0x00008000), 11, 11, &idx_me15_todh_w_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_9C_reg-(0x00008000), 8, 9, &idx_mvinfo_w_v2);
	ReadRegister(M8P_RTHW_PHASE_phase_9C_reg-(0x00008000), 3, 5, &me15_tomclogo_w_v2);

	if(seg_en == 1)
	{
		// seg_1
		data_val = motion_seq;
		Wrt_segment_API_RTK2885pp(1, 0, data_val, 0, 0, 0);

		data_val = det_cadence_id;
		Wrt_segment_API_RTK2885pp(1, 1, data_val, 1, 0, 0);

		data_val = cur_cadence_id;
		Wrt_segment_API_RTK2885pp(1, 2, data_val, 2, 0, 0);

		data_val = out_film_phase;
		Wrt_segment_API_RTK2885pp(1, 3, data_val, 3, 0, 0);

		data_val = me_write;
		Wrt_segment_API_RTK2885pp(1, 4, data_val, 4, 0, 0);

		data_val = me1_read_i;
		Wrt_segment_API_RTK2885pp(1, 5, data_val, 5, 0, 0);

		data_val = me1_read_p;
		Wrt_segment_API_RTK2885pp(1, 6, data_val, 6, 0, 0);

		data_val = me1_last;
		Wrt_segment_API_RTK2885pp(1, 7, data_val, 7, 0, 0);

		data_val = me15_read_i;
		Wrt_segment_API_RTK2885pp(1, 8, data_val, 0, 0, 0);

		data_val = me15_read_p;
		Wrt_segment_API_RTK2885pp(1, 9, data_val, 1, 0, 0);

		data_val = me2_read_i;
		Wrt_segment_API_RTK2885pp(1, 10, data_val, 2, 0, 0);

		data_val = me2_read_p;
		Wrt_segment_API_RTK2885pp(1, 11, data_val, 3, 0, 0);

		data_val = (me2_phase>>4);
		Wrt_segment_API_RTK2885pp(1, 12, data_val, 4, 0, 0);

		data_val = (me2_phase&0xf);
		Wrt_segment_API_RTK2885pp(1, 13, data_val, 4, 0, 0);

		// seg_2
		data_val = mc_write;
		Wrt_segment_API_RTK2885pp(2, 0, data_val, 0, 0, 0);

		data_val = mc_read_i;
		Wrt_segment_API_RTK2885pp(2, 1, data_val, 1, 0, 0);

		data_val = mc_read_p;
		Wrt_segment_API_RTK2885pp(2, 2, data_val, 2, 0, 0);

		data_val = (mc_phase>>4);
		Wrt_segment_API_RTK2885pp(2, 4, data_val, 3, 0, 0);

		data_val = (mc_phase&0xf);
		Wrt_segment_API_RTK2885pp(2, 5, data_val, 3, 0, 0);

		data_val = idx_me1_pfv_w;
		Wrt_segment_API_RTK2885pp(2, 8, data_val, 4, 0, 0);

		data_val = idx_me15_todh_w;
		Wrt_segment_API_RTK2885pp(2, 9, data_val, 5, 0, 0);

		data_val = idx_mvinfo_w;
		Wrt_segment_API_RTK2885pp(2, 10, data_val, 6, 0, 0);

		data_val = me15_tomclogo_w;
		Wrt_segment_API_RTK2885pp(2, 11, data_val, 7, 0, 0);

		data_val = (g_cadence_change_case/100)%10;
		Wrt_segment_API_RTK2885pp(2, 13, data_val, 3, 0, 0);

		data_val = (g_cadence_change_case/10)%10;
		Wrt_segment_API_RTK2885pp(2, 14, data_val, 3, 0, 0);

		data_val = g_cadence_change_case%10;
		Wrt_segment_API_RTK2885pp(2, 15, data_val, 3, 0, 0);

		// seg_3
		/*
		data_val = cur_cadence_id + '0';
		Wrt_segment_API_RTK2885pp(3, 0, data_val, 0, 0, 0);

		data_val = out_film_phase_v2 + '0';
		Wrt_segment_API_RTK2885pp(3, 1, data_val, 1, 0, 0);

		data_val = in_phase_v2 + '0';
		Wrt_segment_API_RTK2885pp(3, 2, data_val, 2, 0, 0);

		data_val = out_phase_v2 + '0';
		Wrt_segment_API_RTK2885pp(3, 3, data_val, 3, 0, 0);

		data_val = me_write_v2 + '0';
		Wrt_segment_API_RTK2885pp(3, 4, data_val, 4, 0, 0);

		data_val = me1_read_i_v2 + '0';
		Wrt_segment_API_RTK2885pp(3, 5, data_val, 5, 0, 0);

		data_val = me1_read_p_v2 + '0';
		Wrt_segment_API_RTK2885pp(3, 6, data_val, 6, 0, 0);

		data_val = me1_last_v2 + '0';
		Wrt_segment_API_RTK2885pp(3, 7, data_val, 7, 0, 0);

		data_val = me15_read_i_v2 + '0';
		Wrt_segment_API_RTK2885pp(3, 8, data_val, 0, 0, 0);

		data_val = me15_read_p_v2 + '0';
		Wrt_segment_API_RTK2885pp(3, 9, data_val, 1, 0, 0);

		data_val = me2_read_i_v2 + '0';
		Wrt_segment_API_RTK2885pp(3, 10, data_val, 2, 0, 0);

		data_val = me2_read_p_v2 + '0';
		Wrt_segment_API_RTK2885pp(3, 11, data_val, 3, 0, 0);

		data_val = (me2_phase_v2>>4) + '0';
		Wrt_segment_API_RTK2885pp(3, 12, data_val, 4, 0, 0);

		data_val = (me2_phase_v2&0xf) + '0';
		Wrt_segment_API_RTK2885pp(3, 13, data_val, 4, 0, 0);

		// seg_4
		data_val = mc_write_v2 + '0';
		Wrt_segment_API_RTK2885pp(4, 0, data_val, 0, 0, 0);

		data_val = mc_read_i_v2 + '0';
		Wrt_segment_API_RTK2885pp(4, 1, data_val, 1, 0, 0);

		data_val = mc_read_p_v2 + '0';
		Wrt_segment_API_RTK2885pp(4, 2, data_val, 2, 0, 0);

		data_val = (mc_phase_v2>>4) + '0';
		Wrt_segment_API_RTK2885pp(4, 4, data_val, 3, 0, 0);

		data_val = (mc_phase_v2&0xf) + '0';
		Wrt_segment_API_RTK2885pp(4, 5, data_val, 3, 0, 0);

		data_val = idx_me1_pfv_w_v2 + '0';
		Wrt_segment_API_RTK2885pp(4, 8, data_val, 4, 0, 0);

		data_val = idx_me15_todh_w_v2 + '0';
		Wrt_segment_API_RTK2885pp(4, 9, data_val, 5, 0, 0);

		data_val = idx_mvinfo_w_v2 + '0';
		Wrt_segment_API_RTK2885pp(4, 10, data_val, 6, 0, 0);

		data_val = me15_tomclogo_w_v2 + '0';
		Wrt_segment_API_RTK2885pp(4, 11, data_val, 7, 0, 0);
		*/
	}
	else if (dbg_seg_show_en == 1)
	{
		Wrt_segment_dbgmode_sel_RTK2885pp();
	}
	else
	{
		Wrt_segment_API_RTK2885pp(0, 0, 0, 0, 0, 0);
	}

	if (reset_en == 1)
	{
		for (u8_i=0; u8_i < 8; u8_i++)
		{
			for (u8_j=0; u8_j < 16; u8_j++)
			{
				Wrt_segment_API_RTK2885pp(u8_i+1, u8_j, -1, 0, 0, 0);
			}
		}
		Wrt_segment_API_RTK2885pp(0, 0, 0, 0, 0, 0);
		WriteRegister(SOFTWARE3_SOFTWARE3_00_reg, 30, 30, 0);
	}
}

void Wrt_segment_val_RTK2885pp(unsigned char seg_ID, int val, unsigned char color, unsigned int x,unsigned int y, unsigned int start_idx,unsigned int end_idx,unsigned int show_mode)
{
#ifdef BUILD_QUICK_SHOW
	return;
#else
	unsigned char u8_i = 0;
	char data_val[16] = {0};
	int num;

	if (show_mode == 0)
	{
		sprintf(data_val, "%d", val);
	}
	else
	{
		sprintf(data_val, "%x", val);
	}
	
	for (u8_i=15; u8_i >= 0; u8_i--)
	{
		if (data_val[u8_i] > 0)
		{
			num = u8_i;
			break;
		}
	}

	// Reverse the string to get the correct order
	for (u8_i=start_idx; u8_i < end_idx; u8_i++)
	{
		if (num >= 0)
		{
			Wrt_segment_API_RTK2885pp(seg_ID, 15 - u8_i, data_val[num--], color, x, y);
		}
		else
		{
			Wrt_segment_API_RTK2885pp(seg_ID, 15 - u8_i, 0, color, x, y);
		}
		
	}
#endif
}

void Wrt_segment_dbgmode_sel_RTK2885pp(void)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int u32_dbg_mode, u32_dbg_rgn_sel, u32_dbg_color, u32_i, print_result;


	ReadRegister(SOFTWARE3_SOFTWARE3_00_reg, 0, 7, &u32_dbg_mode);
	ReadRegister(SOFTWARE3_SOFTWARE3_00_reg, 16, 20, &u32_dbg_rgn_sel);
	ReadRegister(SOFTWARE3_SOFTWARE3_00_reg, 13, 15, &u32_dbg_color);
	ReadRegister(SOFTWARE3_SOFTWARE3_00_reg, 29, 29, &print_result);
	
	if (u32_dbg_mode == 0)
	{
		Wrt_segment_API_RTK2885pp(0, 0, 0, 0, 0, 0);
		return;
	}

	// -------------------------start of dbg mode select--------------------------------
	// show gmv
	else if ((u32_dbg_mode >= 1) && (u32_dbg_mode <= 3))
	{
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb, u32_dbg_color, 0, 0, 0, 7, 0);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb, u32_dbg_color, 0, 0, 9, 15, 0);
		Wrt_segment_API_RTK2885pp(1, 8, 0, u32_dbg_color, 0, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb, u32_dbg_color, 0, 0, 0, 7, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb, u32_dbg_color, 0, 0, 9, 15, 0);
		Wrt_segment_API_RTK2885pp(2, 8, 0, u32_dbg_color, 0, 0);

		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb, u32_dbg_color, 0, 0, 0, 7, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb, u32_dbg_color, 0, 0, 9, 15, 0);
		Wrt_segment_API_RTK2885pp(3, 8, 0, u32_dbg_color, 0, 0);
		Wrt_segment_val_RTK2885pp(4, s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb, u32_dbg_color, 0, 0, 0, 7, 0);
		Wrt_segment_val_RTK2885pp(4, s_pContext->_output_read_comreg.u12_me_GMV_2nd_unconf_rb, u32_dbg_color, 0, 0, 9, 15, 0);
		Wrt_segment_API_RTK2885pp(4, 8, 0, u32_dbg_color, 0, 0);
		
		Wrt_segment_val_RTK2885pp(5, s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb, u32_dbg_color, 0, 0, 0, 7, 0);
		Wrt_segment_val_RTK2885pp(5, s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb, u32_dbg_color, 0, 0, 9, 15, 0);
		Wrt_segment_API_RTK2885pp(5, 8, 0, u32_dbg_color, 0, 0);
		Wrt_segment_val_RTK2885pp(6, s_pContext->_output_read_comreg.u16_me_GMV_3rd_cnt_rb, u32_dbg_color, 0, 0, 0, 7, 0);
		Wrt_segment_val_RTK2885pp(6, s_pContext->_output_read_comreg.u10_me_GMV_3rd_unconf_rb, u32_dbg_color, 0, 0, 9, 15, 0);
		Wrt_segment_API_RTK2885pp(6, 8, 0, u32_dbg_color, 0, 0);
		
	}

	// show rmv
	else if ((u32_dbg_mode >= 4) && (u32_dbg_mode <= 12))
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 31);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 7, 0);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 9, 15, 0);
		Wrt_segment_API_RTK2885pp(1, 8, 0, u32_dbg_color, 0, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 7, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 8, 15, 0);
		Wrt_segment_API_RTK2885pp(2, 8, 0, u32_dbg_color, 0, 0);

		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 7, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 9, 15, 0);
		Wrt_segment_API_RTK2885pp(3, 8, 0, u32_dbg_color, 0, 0);
		Wrt_segment_val_RTK2885pp(4, s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 7, 0);
		Wrt_segment_val_RTK2885pp(4, s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 8, 15, 0);
		Wrt_segment_API_RTK2885pp(4, 8, 0, u32_dbg_color, 0, 0);

		Wrt_segment_val_RTK2885pp(5, s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 7, 0);
		Wrt_segment_val_RTK2885pp(5, s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 9, 15, 0);
		Wrt_segment_API_RTK2885pp(5, 8, 0, u32_dbg_color, 0, 0);
		Wrt_segment_val_RTK2885pp(6, s_pContext->_output_read_comreg.u10_me_rMV_3rd_cnt_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 7, 0);
		Wrt_segment_val_RTK2885pp(6, s_pContext->_output_read_comreg.u10_me_rMV_3rd_unconf_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 9, 15, 0);
		Wrt_segment_API_RTK2885pp(6, 8, 0, u32_dbg_color, 0, 0);
	}
	
	// show u16_ippre_rgn_cnt_u_rb
	else if ((u32_dbg_mode >= 13) && (u32_dbg_mode <= 14))
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 7);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u16_ippre_rgn_cnt_u_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u16_ippre_rgn_cnt_v_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}
	
	// show u27_ipme_fdet_mot_5rgn_rb
	else if (u32_dbg_mode == 15)
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 4);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u27_ipme_fdet_mot_5rgn_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}
	
	// show u27_ipme_fdet_mot_9rgn_rb
	else if (u32_dbg_mode == 16)
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 8);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u27_ipme_fdet_mot_9rgn_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}

	// show u27_ipme_fdet_mot_16rgn_rb
	else if (u32_dbg_mode == 17)
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 15);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u27_ipme_fdet_mot_16rgn_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}

	// show u32_ipme_film_det_rb
	else if ((u32_dbg_mode >= 18) && (u32_dbg_mode <= 20))
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 11);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u32_ipme0_film_det_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u32_ipme1_film_det_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.u32_ipme2_film_det_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}

	// show u32_ipme_fdet_rgn32_apl_rb
	else if (u32_dbg_mode == 21)
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 31);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u32_ipme_fdet_rgn32_apl_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}

	// show u32_ipme_cprs_motion_diff_rb
	else if (u32_dbg_mode == 22)
	{
		for (u32_i = 0; u32_i < 8; u32_i ++)
		{
			Wrt_segment_val_RTK2885pp(u32_i+1, s_pContext->_output_read_comreg.u32_ipme_cprs_motion_diff_rb[u32_i], u32_dbg_color, 0, 0, 0, 15, 0);
		}
	}

	// show u14_logo_rg_cnt_rb
	else if ((u32_dbg_mode >= 23) && (u32_dbg_mode <= 27))
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 31);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u14_logo_rg_cnt_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u16_logo_rg_y_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.u16_no_logo_rg_y_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(4, s_pContext->_output_read_comreg.u12_logo_rg_sobel_hor_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(5, s_pContext->_output_read_comreg.u12_logo_rg_sobel_ver_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}

	// show u20_me_rAPLi_rb
	else if ((u32_dbg_mode >= 28) && (u32_dbg_mode <= 29))
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 31);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u20_me_rAPLi_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u20_me_rAPLp_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}


	// show u10_me_freq_cnt_rb
	else if ((u32_dbg_mode >= 30) && (u32_dbg_mode <= 31))
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 31);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.s10_me_freq_avgbv_vy_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.s11_me_freq_avgbv_vx_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.u10_me_freq_cnt_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}

	
	// show u10_me_bv_stats_rb
	else if (((u32_dbg_mode >= 32) && (u32_dbg_mode <= 33)) || (u32_dbg_mode == 36))
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 31);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u10_me_bv_stats_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u13_me_rPRD_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.u15_me_gmv_bv_diff_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}

	// show u13_me_sobj_cnt_i_rb
	else if ((u32_dbg_mode >= 34) && (u32_dbg_mode <= 35))
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 31);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u13_me_sobj_cnt_i_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u13_me_sobj_cnt_p_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}


	// show u25_me_rgn_sad_rb
	else if ((u32_dbg_mode >= 37) && (u32_dbg_mode <= 41))
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 31);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u25_me_rgn_sad_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u20_me_rgn_dtl_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.u12_me_rgn_fblvl_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(4, s_pContext->_output_read_comreg.u12_me_rgn_scss_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(5, s_pContext->_output_read_comreg.u12_me_rgn_tcss_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}


	// show u10_me15_invld_cnt_rb
	else if ((u32_dbg_mode >= 42) && (u32_dbg_mode <= 46))
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 31);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.s10_me15_invld_mv_vy_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.s11_me15_invld_mv_vx_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.u10_me15_invld_cnt_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(4, s_pContext->_output_read_comreg.u10_me15_invld_rm_cnt_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(5, s_pContext->_output_read_comreg.u12_dh_cov_cnt_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(6, s_pContext->_output_read_comreg.u12_dh_ucov_cnt_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}

	// show u15_mc_local_lpf_cnt_rb
	else if (u32_dbg_mode == 47)
	{
		u32_dbg_rgn_sel = _CLIP_(u32_dbg_rgn_sel, 0, 31);
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u15_mc_local_lpf_cnt_rb[u32_dbg_rgn_sel], u32_dbg_color, 0, 0, 0, 15, 0);
	}

	// rim
	else if (u32_dbg_mode == 48)
	{
		for (u32_i=0; u32_i<4; u32_i++)
		{
			Wrt_segment_val_RTK2885pp(u32_i+1, s_pContext->_output_read_comreg.u12_BBD_roughRim_rb[u32_i], u32_dbg_color, 0, 0, 0, 3, 0);
			Wrt_segment_API_RTK2885pp(u32_i+1, 4, 0, u32_dbg_color, 0, 0);
			Wrt_segment_API_RTK2885pp(u32_i+1, 5, 0, u32_dbg_color, 0, 0);
			Wrt_segment_val_RTK2885pp(u32_i+1, s_pContext->_output_read_comreg.u12_BBD_fineRim_rb[u32_i], u32_dbg_color, 0, 0, 6, 9, 0);
			Wrt_segment_API_RTK2885pp(u32_i+1,10, 0, u32_dbg_color, 0, 0);
			Wrt_segment_API_RTK2885pp(u32_i+1,11, 0, u32_dbg_color, 0, 0);
			Wrt_segment_val_RTK2885pp(u32_i+1, s_pContext->_output_read_comreg.u12_BBD_ex[u32_i], u32_dbg_color, 0, 0, 12, 15, 0);

			Wrt_segment_val_RTK2885pp(u32_i+5, s_pContext->_output_read_comreg.u12_BBD_fineRim_cnt_rb[u32_i], u32_dbg_color, 0, 0, 0, 4, 0);
			Wrt_segment_API_RTK2885pp(u32_i+5, 5, 0, u32_dbg_color, 0, 0);
			Wrt_segment_val_RTK2885pp(u32_i+5, s_pContext->_output_read_comreg.u1_BBD_fineValid_rb[u32_i], u32_dbg_color, 0, 0, 6, 6, 0);
			Wrt_segment_API_RTK2885pp(u32_i+5, 7, 0, u32_dbg_color, 0, 0);
			Wrt_segment_val_RTK2885pp(u32_i+5, s_pContext->_output_read_comreg.u17_BBD_roughRim_cnt_rb[u32_i], u32_dbg_color, 0, 0, 8, 13, 0);
			Wrt_segment_API_RTK2885pp(u32_i+5, 14, 0, u32_dbg_color, 0, 0);
			Wrt_segment_val_RTK2885pp(u32_i+5, s_pContext->_output_read_comreg.u1_BBD_roughValid_rb[u32_i], u32_dbg_color, 0, 0, 15, 15, 0);
		}
	}

	// sc
	else if (u32_dbg_mode == 49)
	{
		for (u32_i=0; u32_i<32; u32_i++)
		{
			if (u32_i < 16)
			{
				Wrt_segment_API_RTK2885pp(1, u32_i, ((s_pContext->_output_read_comreg.u32_me_sc_pattern_rb >> u32_i) & 0x1) + '0', u32_dbg_color, 0, 0);
			}
			else
			{
				Wrt_segment_API_RTK2885pp(2, u32_i-16, ((s_pContext->_output_read_comreg.u32_me_sc_pattern_rb >> u32_i) & 0x1) + '0', u32_dbg_color, 0, 0);

			}
		}
	}

	// color cnt
	else if (u32_dbg_mode == 50)
	{
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u24_ippre_color_cnt0_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u24_ippre_color_cnt1_rb, u32_dbg_color, 0, 0, 0, 15, 0);
	}


	// bvy pcnt
	else if (u32_dbg_mode == 51)
	{
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u20_me_bvy_pcnt0_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u20_me_bvy_pcnt1_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.u20_me_bvy_pcnt2_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(4, s_pContext->_output_read_comreg.u20_me_bvy_pcnt3_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(5, s_pContext->_output_read_comreg.u20_me_bvy_ncnt0_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(6, s_pContext->_output_read_comreg.u20_me_bvy_ncnt1_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(7, s_pContext->_output_read_comreg.u20_me_bvy_ncnt2_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(8, s_pContext->_output_read_comreg.u20_me_bvy_ncnt3_rb, u32_dbg_color, 0, 0, 0, 15, 0);
	}

	// me1 stat 1
	else if (u32_dbg_mode == 52)
	{
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u28_me_dtl_th_sum_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u28_me_dtl_th_sum_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.u20_me_hpan_cnt_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(4, s_pContext->_output_read_comreg.u20_me_vpan_cnt_rb, u32_dbg_color, 0, 0, 0, 15, 0);
	}

	// me1 stat 2
	else if (u32_dbg_mode == 53)
	{
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u6_me_zgmv_cnt_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u20_me_bv_act_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.u20_me_zmv_cnt_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(4, s_pContext->_output_read_comreg.u20_me_zmv_dtl_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(5, s_pContext->_output_read_comreg.u20_me_zmv_sad_dc_cnt_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(6, s_pContext->_output_read_comreg.u31_me_zmv_sad_dc_sum_rb, u32_dbg_color, 0, 0, 0, 15, 0);
	}

	// me1 stat 3
	else if (u32_dbg_mode == 54)
	{
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u20_me_tcss_th_cnt1_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u20_me_tcss_th_cnt2_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.u20_me_tcss_th_cnt3_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(4, s_pContext->_output_read_comreg.u31_me_tcss_th_sum1_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(5, s_pContext->_output_read_comreg.u31_me_tcss_th_sum2_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(6, s_pContext->_output_read_comreg.u20_me_tcss_th_sum3_rb, u32_dbg_color, 0, 0, 0, 15, 0);
	}

	// me1 stat 4
	else if (u32_dbg_mode == 55)
	{
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u30_me_glb_mot_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u30_me_glb_sad_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.u26_me_glb_dtl_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(4, s_pContext->_output_read_comreg.u28_me_glb_scss_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(5, s_pContext->_output_read_comreg.u28_me_glb_tcss_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(6, s_pContext->_output_read_comreg.u26_me_glb_apli_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(7, s_pContext->_output_read_comreg.u26_me_glb_aplp_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		if (print_result)
		{
			rtd_pr_memc_emerg("//********** [MEMC regr][u30_me_glb_mot_rb][%d] **********//\n", s_pContext->_output_read_comreg.u30_me_glb_mot_rb);
			rtd_pr_memc_emerg("//********** [MEMC regr][u30_me_glb_sad_rb][%d] **********//\n", s_pContext->_output_read_comreg.u30_me_glb_sad_rb);
			rtd_pr_memc_emerg("//********** [MEMC regr][u26_me_glb_dtl_rb][%d] **********//\n", s_pContext->_output_read_comreg.u26_me_glb_dtl_rb);
			rtd_pr_memc_emerg("//********** [MEMC regr][u28_me_glb_scss_rb][%d] **********//\n", s_pContext->_output_read_comreg.u28_me_glb_scss_rb);
			rtd_pr_memc_emerg("//********** [MEMC regr][u28_me_glb_tcss_rb][%d] **********//\n", s_pContext->_output_read_comreg.u28_me_glb_tcss_rb);
			rtd_pr_memc_emerg("//********** [MEMC regr][u26_me_glb_apli_rb][%d] **********//\n", s_pContext->_output_read_comreg.u26_me_glb_apli_rb);
			rtd_pr_memc_emerg("//********** [MEMC regr][u26_me_glb_aplp_rb][%d] **********//\n", s_pContext->_output_read_comreg.u26_me_glb_aplp_rb);
		}
	}

	// mv mask
	else if (u32_dbg_mode == 56)
	{
		Wrt_segment_val_RTK2885pp(1, s_pContext->_output_read_comreg.u15_me_mvmask_i_cnt_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(2, s_pContext->_output_read_comreg.u15_me_mvmask_p_cnt_rb, u32_dbg_color, 0, 0, 0, 15, 0);
		Wrt_segment_val_RTK2885pp(3, s_pContext->_output_read_comreg.u20_me_bvy_pcnt2_rb, u32_dbg_color, 0, 0, 0, 15, 0);
	}
}


void WriteComReg_IPPRE_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	
	// define section
	M8P_rtme_iphme0_rtme_iphme0_20_RBUS M8P_rtme_iphme0_rtme_iphme0_20_reg;
	M8P_rtme_iphme0_rtme_iphme0_24_RBUS M8P_rtme_iphme0_rtme_iphme0_24_reg;
	M8P_rtme_iphme0_rtme_iphme0_28_RBUS M8P_rtme_iphme0_rtme_iphme0_28_reg;
	M8P_rtme_iphme0_rtme_iphme0_2c_RBUS M8P_rtme_iphme0_rtme_iphme0_2c_reg;
	
	M8P_rtme_iphme1_rtme_iphme1_20_RBUS M8P_rtme_iphme1_rtme_iphme1_20_reg;
	M8P_rtme_iphme1_rtme_iphme1_24_RBUS M8P_rtme_iphme1_rtme_iphme1_24_reg;
	M8P_rtme_iphme1_rtme_iphme1_28_RBUS M8P_rtme_iphme1_rtme_iphme1_28_reg;
	M8P_rtme_iphme1_rtme_iphme1_2c_RBUS M8P_rtme_iphme1_rtme_iphme1_2c_reg;
	
	M8P_rtme_iphme2_rtme_iphme2_20_RBUS M8P_rtme_iphme2_rtme_iphme2_20_reg;
	M8P_rtme_iphme2_rtme_iphme2_24_RBUS M8P_rtme_iphme2_rtme_iphme2_24_reg;
	M8P_rtme_iphme2_rtme_iphme2_28_RBUS M8P_rtme_iphme2_rtme_iphme2_28_reg;
	M8P_rtme_iphme2_rtme_iphme2_2c_RBUS M8P_rtme_iphme2_rtme_iphme2_2c_reg;
	
	M8P_rtme_iphme3_rtme_iphme3_20_RBUS M8P_rtme_iphme3_rtme_iphme3_20_reg;
	M8P_rtme_iphme3_rtme_iphme3_24_RBUS M8P_rtme_iphme3_rtme_iphme3_24_reg;
	M8P_rtme_iphme3_rtme_iphme3_28_RBUS M8P_rtme_iphme3_rtme_iphme3_28_reg;
	M8P_rtme_iphme3_rtme_iphme3_2c_RBUS M8P_rtme_iphme3_rtme_iphme3_2c_reg;
	// define section end
	unsigned int print_in_func;
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	// read section
	M8P_rtme_iphme0_rtme_iphme0_20_reg.regValue = rtd_inl(M8P_RTME_IPHME0_RTME_IPHME0_20_reg);
	M8P_rtme_iphme0_rtme_iphme0_24_reg.regValue = rtd_inl(M8P_RTME_IPHME0_RTME_IPHME0_24_reg);
	M8P_rtme_iphme0_rtme_iphme0_28_reg.regValue = rtd_inl(M8P_RTME_IPHME0_RTME_IPHME0_28_reg);
	M8P_rtme_iphme0_rtme_iphme0_2c_reg.regValue = rtd_inl(M8P_RTME_IPHME0_RTME_IPHME0_2C_reg);
	
	M8P_rtme_iphme1_rtme_iphme1_20_reg.regValue = rtd_inl(M8P_RTME_IPHME1_RTME_IPHME1_20_reg);
	M8P_rtme_iphme1_rtme_iphme1_24_reg.regValue = rtd_inl(M8P_RTME_IPHME1_RTME_IPHME1_24_reg);
	M8P_rtme_iphme1_rtme_iphme1_28_reg.regValue = rtd_inl(M8P_RTME_IPHME1_RTME_IPHME1_28_reg);
	M8P_rtme_iphme1_rtme_iphme1_2c_reg.regValue = rtd_inl(M8P_RTME_IPHME1_RTME_IPHME1_2C_reg);
	
	M8P_rtme_iphme2_rtme_iphme2_20_reg.regValue = rtd_inl(M8P_RTME_IPHME2_RTME_IPHME2_20_reg);
	M8P_rtme_iphme2_rtme_iphme2_24_reg.regValue = rtd_inl(M8P_RTME_IPHME2_RTME_IPHME2_24_reg);
	M8P_rtme_iphme2_rtme_iphme2_28_reg.regValue = rtd_inl(M8P_RTME_IPHME2_RTME_IPHME2_28_reg);
	M8P_rtme_iphme2_rtme_iphme2_2c_reg.regValue = rtd_inl(M8P_RTME_IPHME2_RTME_IPHME2_2C_reg);
	
	M8P_rtme_iphme3_rtme_iphme3_20_reg.regValue = rtd_inl(M8P_RTME_IPHME3_RTME_IPHME3_20_reg);
	M8P_rtme_iphme3_rtme_iphme3_24_reg.regValue = rtd_inl(M8P_RTME_IPHME3_RTME_IPHME3_24_reg);
	M8P_rtme_iphme3_rtme_iphme3_28_reg.regValue = rtd_inl(M8P_RTME_IPHME3_RTME_IPHME3_28_reg);
	M8P_rtme_iphme3_rtme_iphme3_2c_reg.regValue = rtd_inl(M8P_RTME_IPHME3_RTME_IPHME3_2C_reg);
	// read section end

	// apply section
	if (pParam->u1_PeriodicCtrl_lpf_wrt_en == 1)
	{
		M8P_rtme_iphme0_rtme_iphme0_20_reg.ipme_layer0_lpf_5x5_coef_00 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_20_reg.ipme_layer0_lpf_5x5_coef_01 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_20_reg.ipme_layer0_lpf_5x5_coef_02 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_20_reg.ipme_layer0_lpf_5x5_coef_03 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_20_reg.ipme_layer0_lpf_5x5_coef_04 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_20_reg.ipme_layer0_lpf_5x5_coef_05 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_20_reg.ipme_layer0_lpf_5x5_coef_06 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_20_reg.ipme_layer0_lpf_5x5_coef_07 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_24_reg.ipme_layer0_lpf_5x5_coef_10 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_24_reg.ipme_layer0_lpf_5x5_coef_11 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_24_reg.ipme_layer0_lpf_5x5_coef_12 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_24_reg.ipme_layer0_lpf_5x5_coef_13 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_24_reg.ipme_layer0_lpf_5x5_coef_14 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_24_reg.ipme_layer0_lpf_5x5_coef_15 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_24_reg.ipme_layer0_lpf_5x5_coef_16 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_24_reg.ipme_layer0_lpf_5x5_coef_17 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_28_reg.ipme_layer0_lpf_5x5_coef_20 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_28_reg.ipme_layer0_lpf_5x5_coef_21 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_28_reg.ipme_layer0_lpf_5x5_coef_22 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_28_reg.ipme_layer0_lpf_5x5_coef_23 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_28_reg.ipme_layer0_lpf_5x5_coef_24 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_28_reg.ipme_layer0_lpf_5x5_coef_25 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_28_reg.ipme_layer0_lpf_5x5_coef_26 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_28_reg.ipme_layer0_lpf_5x5_coef_27 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_2c_reg.ipme_layer0_lpf_5x5_coef_30 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_2c_reg.ipme_layer0_lpf_5x5_coef_31 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_2c_reg.ipme_layer0_lpf_5x5_coef_32 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_2c_reg.ipme_layer0_lpf_5x5_coef_33 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_2c_reg.ipme_layer0_lpf_5x5_coef_34 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_2c_reg.ipme_layer0_lpf_5x5_coef_35 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_2c_reg.ipme_layer0_lpf_5x5_coef_36 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme0_rtme_iphme0_2c_reg.ipme_layer0_lpf_5x5_coef_37 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		
		M8P_rtme_iphme1_rtme_iphme1_20_reg.ipme_layer1_lpf_5x5_coef_00 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_20_reg.ipme_layer1_lpf_5x5_coef_01 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_20_reg.ipme_layer1_lpf_5x5_coef_02 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_20_reg.ipme_layer1_lpf_5x5_coef_03 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_20_reg.ipme_layer1_lpf_5x5_coef_04 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_20_reg.ipme_layer1_lpf_5x5_coef_05 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_20_reg.ipme_layer1_lpf_5x5_coef_06 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_20_reg.ipme_layer1_lpf_5x5_coef_07 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_24_reg.ipme_layer1_lpf_5x5_coef_10 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_24_reg.ipme_layer1_lpf_5x5_coef_11 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_24_reg.ipme_layer1_lpf_5x5_coef_12 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_24_reg.ipme_layer1_lpf_5x5_coef_13 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_24_reg.ipme_layer1_lpf_5x5_coef_14 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_24_reg.ipme_layer1_lpf_5x5_coef_15 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_24_reg.ipme_layer1_lpf_5x5_coef_16 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_24_reg.ipme_layer1_lpf_5x5_coef_17 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_28_reg.ipme_layer1_lpf_5x5_coef_20 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_28_reg.ipme_layer1_lpf_5x5_coef_21 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_28_reg.ipme_layer1_lpf_5x5_coef_22 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_28_reg.ipme_layer1_lpf_5x5_coef_23 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_28_reg.ipme_layer1_lpf_5x5_coef_24 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_28_reg.ipme_layer1_lpf_5x5_coef_25 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_28_reg.ipme_layer1_lpf_5x5_coef_26 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_28_reg.ipme_layer1_lpf_5x5_coef_27 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_2c_reg.ipme_layer1_lpf_5x5_coef_30 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_2c_reg.ipme_layer1_lpf_5x5_coef_31 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_2c_reg.ipme_layer1_lpf_5x5_coef_32 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_2c_reg.ipme_layer1_lpf_5x5_coef_33 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_2c_reg.ipme_layer1_lpf_5x5_coef_34 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_2c_reg.ipme_layer1_lpf_5x5_coef_35 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_2c_reg.ipme_layer1_lpf_5x5_coef_36 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme1_rtme_iphme1_2c_reg.ipme_layer1_lpf_5x5_coef_37 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		
		M8P_rtme_iphme2_rtme_iphme2_20_reg.ipme_layer2_lpf_5x5_coef_00 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_20_reg.ipme_layer2_lpf_5x5_coef_01 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_20_reg.ipme_layer2_lpf_5x5_coef_02 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_20_reg.ipme_layer2_lpf_5x5_coef_03 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_20_reg.ipme_layer2_lpf_5x5_coef_04 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_20_reg.ipme_layer2_lpf_5x5_coef_05 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_20_reg.ipme_layer2_lpf_5x5_coef_06 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_20_reg.ipme_layer2_lpf_5x5_coef_07 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_24_reg.ipme_layer2_lpf_5x5_coef_10 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_24_reg.ipme_layer2_lpf_5x5_coef_11 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_24_reg.ipme_layer2_lpf_5x5_coef_12 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_24_reg.ipme_layer2_lpf_5x5_coef_13 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_24_reg.ipme_layer2_lpf_5x5_coef_14 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_24_reg.ipme_layer2_lpf_5x5_coef_15 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_24_reg.ipme_layer2_lpf_5x5_coef_16 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_24_reg.ipme_layer2_lpf_5x5_coef_17 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_28_reg.ipme_layer2_lpf_5x5_coef_20 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_28_reg.ipme_layer2_lpf_5x5_coef_21 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_28_reg.ipme_layer2_lpf_5x5_coef_22 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_28_reg.ipme_layer2_lpf_5x5_coef_23 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_28_reg.ipme_layer2_lpf_5x5_coef_24 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_28_reg.ipme_layer2_lpf_5x5_coef_25 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_28_reg.ipme_layer2_lpf_5x5_coef_26 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_28_reg.ipme_layer2_lpf_5x5_coef_27 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_2c_reg.ipme_layer2_lpf_5x5_coef_30 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_2c_reg.ipme_layer2_lpf_5x5_coef_31 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_2c_reg.ipme_layer2_lpf_5x5_coef_32 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_2c_reg.ipme_layer2_lpf_5x5_coef_33 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_2c_reg.ipme_layer2_lpf_5x5_coef_34 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_2c_reg.ipme_layer2_lpf_5x5_coef_35 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_2c_reg.ipme_layer2_lpf_5x5_coef_36 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		M8P_rtme_iphme2_rtme_iphme2_2c_reg.ipme_layer2_lpf_5x5_coef_37 = (s_pContext->_output_periodic_manage.u4_lpf_5x5_coef > 0)?8:0;
		
		M8P_rtme_iphme3_rtme_iphme3_20_reg.ipme_layer3_lpf_5x5_coef_00 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_20_reg.ipme_layer3_lpf_5x5_coef_01 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_20_reg.ipme_layer3_lpf_5x5_coef_02 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_20_reg.ipme_layer3_lpf_5x5_coef_03 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_20_reg.ipme_layer3_lpf_5x5_coef_04 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_20_reg.ipme_layer3_lpf_5x5_coef_05 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_20_reg.ipme_layer3_lpf_5x5_coef_06 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_20_reg.ipme_layer3_lpf_5x5_coef_07 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_24_reg.ipme_layer3_lpf_5x5_coef_10 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_24_reg.ipme_layer3_lpf_5x5_coef_11 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_24_reg.ipme_layer3_lpf_5x5_coef_12 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_24_reg.ipme_layer3_lpf_5x5_coef_13 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_24_reg.ipme_layer3_lpf_5x5_coef_14 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_24_reg.ipme_layer3_lpf_5x5_coef_15 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_24_reg.ipme_layer3_lpf_5x5_coef_16 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_24_reg.ipme_layer3_lpf_5x5_coef_17 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_28_reg.ipme_layer3_lpf_5x5_coef_20 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_28_reg.ipme_layer3_lpf_5x5_coef_21 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_28_reg.ipme_layer3_lpf_5x5_coef_22 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_28_reg.ipme_layer3_lpf_5x5_coef_23 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_28_reg.ipme_layer3_lpf_5x5_coef_24 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_28_reg.ipme_layer3_lpf_5x5_coef_25 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_28_reg.ipme_layer3_lpf_5x5_coef_26 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_28_reg.ipme_layer3_lpf_5x5_coef_27 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_2c_reg.ipme_layer3_lpf_5x5_coef_30 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_2c_reg.ipme_layer3_lpf_5x5_coef_31 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_2c_reg.ipme_layer3_lpf_5x5_coef_32 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_2c_reg.ipme_layer3_lpf_5x5_coef_33 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_2c_reg.ipme_layer3_lpf_5x5_coef_34 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_2c_reg.ipme_layer3_lpf_5x5_coef_35 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_2c_reg.ipme_layer3_lpf_5x5_coef_36 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
		M8P_rtme_iphme3_rtme_iphme3_2c_reg.ipme_layer3_lpf_5x5_coef_37 = s_pContext->_output_periodic_manage.u4_lpf_5x5_coef;
	}
	// apply section end

	
	// write section
	rtd_outl(M8P_RTME_IPHME0_RTME_IPHME0_20_reg, M8P_rtme_iphme0_rtme_iphme0_20_reg.regValue);
	rtd_outl(M8P_RTME_IPHME0_RTME_IPHME0_24_reg, M8P_rtme_iphme0_rtme_iphme0_24_reg.regValue);
	rtd_outl(M8P_RTME_IPHME0_RTME_IPHME0_28_reg, M8P_rtme_iphme0_rtme_iphme0_28_reg.regValue);
	rtd_outl(M8P_RTME_IPHME0_RTME_IPHME0_2C_reg, M8P_rtme_iphme0_rtme_iphme0_2c_reg.regValue);

	rtd_outl(M8P_RTME_IPHME1_RTME_IPHME1_20_reg, M8P_rtme_iphme1_rtme_iphme1_20_reg.regValue);
	rtd_outl(M8P_RTME_IPHME1_RTME_IPHME1_24_reg, M8P_rtme_iphme1_rtme_iphme1_24_reg.regValue);
	rtd_outl(M8P_RTME_IPHME1_RTME_IPHME1_28_reg, M8P_rtme_iphme1_rtme_iphme1_28_reg.regValue);
	rtd_outl(M8P_RTME_IPHME1_RTME_IPHME1_2C_reg, M8P_rtme_iphme1_rtme_iphme1_2c_reg.regValue);

	rtd_outl(M8P_RTME_IPHME2_RTME_IPHME2_20_reg, M8P_rtme_iphme2_rtme_iphme2_20_reg.regValue);
	rtd_outl(M8P_RTME_IPHME2_RTME_IPHME2_24_reg, M8P_rtme_iphme2_rtme_iphme2_24_reg.regValue);
	rtd_outl(M8P_RTME_IPHME2_RTME_IPHME2_28_reg, M8P_rtme_iphme2_rtme_iphme2_28_reg.regValue);
	rtd_outl(M8P_RTME_IPHME2_RTME_IPHME2_2C_reg, M8P_rtme_iphme2_rtme_iphme2_2c_reg.regValue);

	rtd_outl(M8P_RTME_IPHME3_RTME_IPHME3_20_reg, M8P_rtme_iphme3_rtme_iphme3_20_reg.regValue);
	rtd_outl(M8P_RTME_IPHME3_RTME_IPHME3_24_reg, M8P_rtme_iphme3_rtme_iphme3_24_reg.regValue);
	rtd_outl(M8P_RTME_IPHME3_RTME_IPHME3_28_reg, M8P_rtme_iphme3_rtme_iphme3_28_reg.regValue);
	rtd_outl(M8P_RTME_IPHME3_RTME_IPHME3_2C_reg, M8P_rtme_iphme3_rtme_iphme3_2c_reg.regValue);
	// write section end
	
	IPME_LPF_wrtAction_RTK2885pp(pParam, pOutput);
}

void WriteComReg_BBD_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
}

void WriteComReg_LOGO_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	// define section
	M8P_rtme_logo0_logo0_a4_RBUS logo0_a4_reg;
	M8P_rtme_logo0_logo0_a8_RBUS logo0_a8_reg;
	M8P_rtme_logo0_logo0_ac_RBUS logo0_ac_reg;
	M8P_rtme_logo0_logo0_b0_RBUS logo0_b0_reg;
	M8P_rtme_logo0_logo0_dc_RBUS logo0_dc_reg;
	M8P_rtme_logo0_logo0_e0_RBUS logo0_e0_reg;
	
	M8P_rtme_logo0_logo0_c4_RBUS M8P_rtme_logo0_logo0_c4;   // logo detection temporal counter
	M8P_rtme_hme1_top7_me1_bv_stats_mv_center_threshold_RBUS M8P_rtme_hme1_top7_me1_bv_stats_mv_center_threshold;   // logo longterm   
	
	// define section end
	unsigned int print_in_func;
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	// read section
	logo0_a8_reg.regValue = rtd_inl(M8P_RTME_LOGO0_LOGO0_A8_reg);
	logo0_a4_reg.regValue = rtd_inl(M8P_RTME_LOGO0_LOGO0_A4_reg);
	logo0_ac_reg.regValue = rtd_inl(M8P_RTME_LOGO0_LOGO0_AC_reg);
	logo0_b0_reg.regValue = rtd_inl(M8P_RTME_LOGO0_LOGO0_B0_reg);
	logo0_dc_reg.regValue = rtd_inl(M8P_RTME_LOGO0_LOGO0_DC_reg);
	logo0_e0_reg.regValue = rtd_inl(M8P_RTME_LOGO0_LOGO0_E0_reg);

	M8P_rtme_logo0_logo0_c4.regValue = rtd_inl(M8P_RTME_LOGO0_LOGO0_C4_reg);	// logo detection temporal counter
	M8P_rtme_hme1_top7_me1_bv_stats_mv_center_threshold.regValue = rtd_inl(M8P_RTME_HME1_TOP7_me1_bv_stats_mv_center_threshold_reg);	// logo longterm
	// read section end


	// apply section
	if ((pParam->u1_Wrt_ComReg_LOGO_en == 1) && (s_pContext->_output_rimctrl.u1_RimChange == 1))
	{
		logo0_a8_reg.logo_layer1_rim_v0 = s_pContext->_output_rimctrl.u12_logo_layer1_rim[_RIM_TOP];
		logo0_a8_reg.logo_layer1_rim_v1 = s_pContext->_output_rimctrl.u12_logo_layer1_rim[_RIM_BOT];
		logo0_a4_reg.logo_layer1_rim_h0 = s_pContext->_output_rimctrl.u12_logo_layer1_rim[_RIM_LFT];
		logo0_a4_reg.logo_layer1_rim_h1 = s_pContext->_output_rimctrl.u12_logo_layer1_rim[_RIM_RHT];
		logo0_b0_reg.logo_layer2_rim_v0 = s_pContext->_output_rimctrl.u12_logo_layer2_rim[_RIM_TOP];
		logo0_b0_reg.logo_layer2_rim_v1 = s_pContext->_output_rimctrl.u12_logo_layer2_rim[_RIM_BOT];
		logo0_ac_reg.logo_layer2_rim_h0 = s_pContext->_output_rimctrl.u12_logo_layer2_rim[_RIM_LFT];
		logo0_ac_reg.logo_layer2_rim_h1 = s_pContext->_output_rimctrl.u12_logo_layer2_rim[_RIM_RHT];
		logo0_ac_reg.logo_layer3_rim_v0 = s_pContext->_output_rimctrl.u12_logo_layer3_rim[_RIM_TOP];
		logo0_b0_reg.logo_layer3_rim_v1 = s_pContext->_output_rimctrl.u12_logo_layer3_rim[_RIM_BOT];
		logo0_a4_reg.logo_layer3_rim_h0 = s_pContext->_output_rimctrl.u12_logo_layer3_rim[_RIM_LFT];
		logo0_a8_reg.logo_layer3_rim_h1 = s_pContext->_output_rimctrl.u12_logo_layer3_rim[_RIM_RHT];				
	}

	if (pParam->u1_LogoCtrl_logo_clear_wrt_en == 1)
	{
		if (pParam->u1_LogoCtrl_sobel_logo_clear_wrt_en == 1 && s_pContext->_output_logo_ctrl.u32_sobel_logo_clear_flag != 0)
		{
			logo0_dc_reg.logo_clear_en = 1;
			logo0_e0_reg.logo_clear_flag_en = s_pContext->_output_logo_ctrl.u32_sobel_logo_clear_flag;
		}
	}

	// logo temporal counter apply
	if( s_pContext->_output_logo_ctrl.zmv_cnt_sum_cond)
	{
		M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y0 = 0;
		M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y1 = 0;
		M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y2 = 0;
	}
	else if( s_pContext->_output_logo_ctrl.small_gmv_cond)
	{
		if(s_pContext->_output_logo_ctrl.input_frame_cond)
		{
			M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y0 = 1;
			M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y1 = 1;
			M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y2 = 1;
		}
		else
		{
			M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y0 = 0;
			M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y1 = 0;
			M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y2 = 0;
		}
	}
	else if( s_pContext->_output_logo_ctrl.hammock_condition)
	{
		if(s_pContext->_output_logo_ctrl.input_frame_cond)
		{
			M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y0 = 1;
			M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y1 = 1;
			M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y2 = 1;
		}
		else
		{
			M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y0 = 0;
			M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y1 = 0;
			M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y2 = 0;
		}	
	}
	else
	{
		M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y0 = 0;
		M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y1 = 3;
		M8P_rtme_logo0_logo0_c4.logo_blkhsty_pstep_y2 = 5;	
	}	


	// logo longterm
	// me1_bv_stats_mv_centerX = 3
	M8P_rtme_hme1_top7_me1_bv_stats_mv_center_threshold.me1_bv_stats_mv_centerx = 0;
	M8P_rtme_hme1_top7_me1_bv_stats_mv_center_threshold.me1_bv_stats_mv_centery = 0;
	M8P_rtme_hme1_top7_me1_bv_stats_mv_center_threshold.me1_bv_stats_mv_threshold = 2;
	// apply section end

	// write section
	rtd_outl(M8P_RTME_LOGO0_LOGO0_A8_reg, logo0_a8_reg.regValue);
	rtd_outl(M8P_RTME_LOGO0_LOGO0_A4_reg, logo0_a4_reg.regValue);
	rtd_outl(M8P_RTME_LOGO0_LOGO0_AC_reg, logo0_ac_reg.regValue);
	rtd_outl(M8P_RTME_LOGO0_LOGO0_B0_reg, logo0_b0_reg.regValue);
	rtd_outl(M8P_RTME_LOGO0_LOGO0_DC_reg, logo0_dc_reg.regValue);
	rtd_outl(M8P_RTME_LOGO0_LOGO0_E0_reg, logo0_e0_reg.regValue);

	rtd_outl(M8P_RTME_LOGO0_LOGO0_C4_reg, M8P_rtme_logo0_logo0_c4.regValue);	// logo detection temporal counter
	rtd_outl(M8P_RTME_HME1_TOP7_me1_bv_stats_mv_center_threshold_reg, M8P_rtme_hme1_top7_me1_bv_stats_mv_center_threshold.regValue);	// Logo longterm
	// write section end

}

VOID dehaloLogo_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();


	// dehalo logo boundary reg
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_00_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_00;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_04_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_04;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_08_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_08;

	// dehalo logo sad_ofs_th 32rgn reg
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_10_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_10;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_14_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_14;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_18_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_18;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_1c_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_1c;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_20_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_20;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_24_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_24;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_28_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_28;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_2c_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_2c;

	// dehalo logo sad_mvd_th 32rgn reg
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_30_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_30;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_34_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_34;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_38_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_38;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_3c_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_3c;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_40_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_40;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_44_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_44;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_48_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_48;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_4c_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_4c;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_50_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_50;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_54_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_54;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_58_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_58;	

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_74_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_74;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_78_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_78;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_7c_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_7c;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_80_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_80;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_84_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_84;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_88_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_88;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_8c_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_8c;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_90_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_90;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_94_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_94;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_98_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_98;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_9c_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_9c;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a0_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_a0;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a4_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_a4;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a8_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_a8;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ac_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_ac;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b0_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_b0;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b4_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_b4;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b8_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_b8;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_bc_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_bc;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c0_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_c0;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c4_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_c4;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c8_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_c8;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_cc_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_cc;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d0_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_d0;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d4_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_d4;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d8_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_d8;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_dc_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_dc;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e0_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_e0;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e4_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_e4;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e8_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_e8;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ec_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_ec;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f0_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_f0;
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f4_RBUS M8P_rtme_dehalo_logo_rtme_dehalo_logo_f4;

	// dehalo logo boundary reg
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_00.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_00_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_04.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_04_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_08.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_08_reg);

	// dehalo logo sad_ofs_th 32rgn reg
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_10.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_10_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_14.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_14_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_18.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_18_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_1c.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_1C_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_20.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_20_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_24.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_24_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_28.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_28_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_2c.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_2C_reg);
	
	// dehalo logo sad_mvd_th 32rgn reg
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_30.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_30_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_34.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_34_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_38.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_38_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_3c.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_3C_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_40.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_40_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_44.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_44_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_48.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_48_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_4c.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_4C_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_50.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_50_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_54.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_54_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_58.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_58_reg);

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_74.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_74_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_78.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_78_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_7c.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_7C_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_80.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_80_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_84.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_84_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_88.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_88_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_8c.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_8C_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_90.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_90_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_94.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_94_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_98.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_98_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_9c.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_9C_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a0.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_A0_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a4.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_A4_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a8.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_A8_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ac.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_AC_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b0.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_B0_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b4.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_B4_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b8.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_B8_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_bc.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_BC_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c0.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_C0_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c4.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_C4_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c8.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_C8_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_cc.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_CC_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d0.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_D0_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d4.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_D4_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d8.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_D8_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_dc.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_DC_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e0.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_E0_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e4.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_E4_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e8.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_E8_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ec.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_EC_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f0.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_F0_reg);
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f4.regValue = rtd_inl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_F4_reg);

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_00.dh_logo_x0 = s_pContext->_output_logo_ctrl.dh_logo_boundary_x[0];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_04.dh_logo_x1 = s_pContext->_output_logo_ctrl.dh_logo_boundary_x[1];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_04.dh_logo_x2 = s_pContext->_output_logo_ctrl.dh_logo_boundary_x[2];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_04.dh_logo_x3 = s_pContext->_output_logo_ctrl.dh_logo_boundary_x[3];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_08.dh_logo_x4 = s_pContext->_output_logo_ctrl.dh_logo_boundary_x[4];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_08.dh_logo_x5 = s_pContext->_output_logo_ctrl.dh_logo_boundary_x[5];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_08.dh_logo_x6 = s_pContext->_output_logo_ctrl.dh_logo_boundary_x[6];
	
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_10.dh_logo_sad_ofs_idx0 = s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[0];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_30.dh_logo_sad_mvd_th_idx0 = s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[0];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_78.dh_logo_mv_th_idx0 = s_pContext->_output_logo_ctrl.dh_logo_mv_th[0];
//	M8P_rtme_dehalo_logo_rtme_dehalo_logo_78.dh_logo_zmv_cnt_th_idx0 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[0];
//	M8P_rtme_dehalo_logo_rtme_dehalo_logo_78.dh_logo_mvd_3x3_cnt_th_idx0 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[0];
//	M8P_rtme_dehalo_logo_rtme_dehalo_logo_78.dh_logo_mvd_3x3_th_idx0 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[0];
//	M8P_rtme_dehalo_logo_rtme_dehalo_logo_78.dh_logo_large_mv_cnt_th_idx0 = s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[0];
//	M8P_rtme_dehalo_logo_rtme_dehalo_logo_78.dh_logo_zmv_cnt_diff_th_idx0 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[0];
//	M8P_rtme_dehalo_logo_rtme_dehalo_logo_78.dh_logo_var_cnt_th_idx0 = s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[0];	

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_10.dh_logo_sad_ofs_idx1 = s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[1];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_30.dh_logo_sad_mvd_th_idx1 = s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[1];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_7c.dh_logo_mv_th_idx1 = s_pContext->_output_logo_ctrl.dh_logo_mv_th[1];
//	M8P_rtme_dehalo_logo_rtme_dehalo_logo_7c.dh_logo_zmv_cnt_th_idx1 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[1];
//	M8P_rtme_dehalo_logo_rtme_dehalo_logo_7c.dh_logo_mvd_3x3_cnt_th_idx1 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[1];
//	M8P_rtme_dehalo_logo_rtme_dehalo_logo_7c.dh_logo_mvd_3x3_th_idx1 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[1];
//	M8P_rtme_dehalo_logo_rtme_dehalo_logo_7c.dh_logo_large_mv_cnt_th_idx1 = s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[1];
//	M8P_rtme_dehalo_logo_rtme_dehalo_logo_7c.dh_logo_zmv_cnt_diff_th_idx1 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[1];
//	M8P_rtme_dehalo_logo_rtme_dehalo_logo_7c.dh_logo_var_cnt_th_idx1 = s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[1];	

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_10.dh_logo_sad_ofs_idx2 = s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[2];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_30.dh_logo_sad_mvd_th_idx2 = s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[2];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_80.dh_logo_mv_th_idx2 = s_pContext->_output_logo_ctrl.dh_logo_mv_th[2];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_80.dh_logo_zmv_cnt_th_idx2 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[2];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_80.dh_logo_mvd_3x3_cnt_th_idx2 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[2];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_80.dh_logo_mvd_3x3_th_idx2 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[2];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_80.dh_logo_large_mv_cnt_th_idx2 = s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[2];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_80.dh_logo_zmv_cnt_diff_th_idx2 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[2];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_80.dh_logo_var_cnt_th_idx2 = s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[2];		
	  
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_10.dh_logo_sad_ofs_idx3 = s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[3];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_34.dh_logo_sad_mvd_th_idx3 = s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[3];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_84.dh_logo_mv_th_idx3 = s_pContext->_output_logo_ctrl.dh_logo_mv_th[3];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_84.dh_logo_zmv_cnt_th_idx3 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[3];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_84.dh_logo_mvd_3x3_cnt_th_idx3 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[3];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_84.dh_logo_mvd_3x3_th_idx3 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[3];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_84.dh_logo_large_mv_cnt_th_idx3 = s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[3];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_84.dh_logo_zmv_cnt_diff_th_idx3 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[3];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_84.dh_logo_var_cnt_th_idx3 = s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[3];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_14.dh_logo_sad_ofs_idx4 = s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[4];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_34.dh_logo_sad_mvd_th_idx4 = s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[4];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_88.dh_logo_mv_th_idx4 = s_pContext->_output_logo_ctrl.dh_logo_mv_th[4];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_88.dh_logo_zmv_cnt_th_idx4 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[4];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_88.dh_logo_mvd_3x3_cnt_th_idx4 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[4];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_88.dh_logo_mvd_3x3_th_idx4 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[4];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_88.dh_logo_large_mv_cnt_th_idx4 = s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[4];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_88.dh_logo_zmv_cnt_diff_th_idx4 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[4];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_88.dh_logo_var_cnt_th_idx4 = s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[4];	

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_14.dh_logo_sad_ofs_idx5 = s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[5];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_34.dh_logo_sad_mvd_th_idx5 = s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[5];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_8c.dh_logo_mv_th_idx5 = s_pContext->_output_logo_ctrl.dh_logo_mv_th[5];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_8c.dh_logo_zmv_cnt_th_idx5 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[5];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_8c.dh_logo_mvd_3x3_cnt_th_idx5 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[5];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_8c.dh_logo_mvd_3x3_th_idx5 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[5];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_8c.dh_logo_large_mv_cnt_th_idx5 = s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[5];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_8c.dh_logo_zmv_cnt_diff_th_idx5 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[5];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_8c.dh_logo_var_cnt_th_idx5 = s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[5];	

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_14.dh_logo_sad_ofs_idx6 = s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[6];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_38.dh_logo_sad_mvd_th_idx6= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[6];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_90.dh_logo_mv_th_idx6 = s_pContext->_output_logo_ctrl.dh_logo_mv_th[6];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_90.dh_logo_zmv_cnt_th_idx6 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[6];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_90.dh_logo_mvd_3x3_cnt_th_idx6 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[6];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_90.dh_logo_mvd_3x3_th_idx6 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[6];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_90.dh_logo_large_mv_cnt_th_idx6 = s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[6];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_90.dh_logo_zmv_cnt_diff_th_idx6 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[6];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_90.dh_logo_var_cnt_th_idx6 = s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[6];	

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_14.dh_logo_sad_ofs_idx7 = s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[7];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_38.dh_logo_sad_mvd_th_idx7= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[7];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_94.dh_logo_mv_th_idx7 = s_pContext->_output_logo_ctrl.dh_logo_mv_th[7];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_94.dh_logo_zmv_cnt_th_idx7 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[7];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_94.dh_logo_mvd_3x3_cnt_th_idx7 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[7];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_94.dh_logo_mvd_3x3_th_idx7 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[7];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_94.dh_logo_large_mv_cnt_th_idx7 = s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[7];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_94.dh_logo_zmv_cnt_diff_th_idx7 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[7];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_94.dh_logo_var_cnt_th_idx7 = s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[7];		

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_18.dh_logo_sad_ofs_idx8= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[8];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_38.dh_logo_sad_mvd_th_idx8= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[8];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_98.dh_logo_mv_th_idx8 = s_pContext->_output_logo_ctrl.dh_logo_mv_th[8];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_98.dh_logo_zmv_cnt_th_idx8 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[8];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_98.dh_logo_mvd_3x3_cnt_th_idx8 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[8];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_98.dh_logo_mvd_3x3_th_idx8 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[8];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_98.dh_logo_large_mv_cnt_th_idx8 = s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[8];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_98.dh_logo_zmv_cnt_diff_th_idx8 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[8];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_98.dh_logo_var_cnt_th_idx8 = s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[8];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_18.dh_logo_sad_ofs_idx9= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[9];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_3c.dh_logo_sad_mvd_th_idx9= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[9];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_9c.dh_logo_mv_th_idx9 = s_pContext->_output_logo_ctrl.dh_logo_mv_th[9];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_9c.dh_logo_zmv_cnt_th_idx9 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[9];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_9c.dh_logo_mvd_3x3_cnt_th_idx9 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[9];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_9c.dh_logo_mvd_3x3_th_idx9 = s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[9];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_9c.dh_logo_large_mv_cnt_th_idx9 = s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[9];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_9c.dh_logo_zmv_cnt_diff_th_idx9 = s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[9];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_9c.dh_logo_var_cnt_th_idx9 = s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[9];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_18.dh_logo_sad_ofs_idx10= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[10];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_3c.dh_logo_sad_mvd_th_idx10= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[10];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a0.dh_logo_mv_th_idx10= s_pContext->_output_logo_ctrl.dh_logo_mv_th[10];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a0.dh_logo_zmv_cnt_th_idx10= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[10];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a0.dh_logo_mvd_3x3_cnt_th_idx10= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[10];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a0.dh_logo_mvd_3x3_th_idx10= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[10];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a0.dh_logo_large_mv_cnt_th_idx10= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[10];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a0.dh_logo_zmv_cnt_diff_th_idx10= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[10];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a0.dh_logo_var_cnt_th_idx10= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[10];		

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_18.dh_logo_sad_ofs_idx11= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[11];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_3c.dh_logo_sad_mvd_th_idx11= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[11];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a4.dh_logo_mv_th_idx11= s_pContext->_output_logo_ctrl.dh_logo_mv_th[11];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a4.dh_logo_zmv_cnt_th_idx11= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[11];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a4.dh_logo_mvd_3x3_cnt_th_idx11= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[11];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a4.dh_logo_mvd_3x3_th_idx11= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[11];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a4.dh_logo_large_mv_cnt_th_idx11= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[11];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a4.dh_logo_zmv_cnt_diff_th_idx11= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[11];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a4.dh_logo_var_cnt_th_idx11= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[11];	

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_1c.dh_logo_sad_ofs_idx12= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[12];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_40.dh_logo_sad_mvd_th_idx12= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[12];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a8.dh_logo_mv_th_idx12= s_pContext->_output_logo_ctrl.dh_logo_mv_th[12];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a8.dh_logo_zmv_cnt_th_idx12= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[12];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a8.dh_logo_mvd_3x3_cnt_th_idx12= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[12];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a8.dh_logo_mvd_3x3_th_idx12= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[12];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a8.dh_logo_large_mv_cnt_th_idx12= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[12];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a8.dh_logo_zmv_cnt_diff_th_idx12= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[12];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_a8.dh_logo_var_cnt_th_idx12= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[12];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_1c.dh_logo_sad_ofs_idx13= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[13];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_40.dh_logo_sad_mvd_th_idx13= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[13];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ac.dh_logo_mv_th_idx13= s_pContext->_output_logo_ctrl.dh_logo_mv_th[13];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ac.dh_logo_zmv_cnt_th_idx13= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[13];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ac.dh_logo_mvd_3x3_cnt_th_idx13= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[13];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ac.dh_logo_mvd_3x3_th_idx13= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[13];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ac.dh_logo_large_mv_cnt_th_idx13= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[13];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ac.dh_logo_zmv_cnt_diff_th_idx13= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[13];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ac.dh_logo_var_cnt_th_idx13= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[13];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_1c.dh_logo_sad_ofs_idx14= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[14];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_40.dh_logo_sad_mvd_th_idx14= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[14];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b0.dh_logo_mv_th_idx14= s_pContext->_output_logo_ctrl.dh_logo_mv_th[14];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b0.dh_logo_zmv_cnt_th_idx14= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[14];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b0.dh_logo_mvd_3x3_cnt_th_idx14= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[14];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b0.dh_logo_mvd_3x3_th_idx14= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[14];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b0.dh_logo_large_mv_cnt_th_idx14= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[14];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b0.dh_logo_zmv_cnt_diff_th_idx14= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[14];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b0.dh_logo_var_cnt_th_idx14= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[14];	

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_1c.dh_logo_sad_ofs_idx15= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[15];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_44.dh_logo_sad_mvd_th_idx15= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[15];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b4.dh_logo_mv_th_idx15= s_pContext->_output_logo_ctrl.dh_logo_mv_th[15];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b4.dh_logo_zmv_cnt_th_idx15= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[15];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b4.dh_logo_mvd_3x3_cnt_th_idx15= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[15];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b4.dh_logo_mvd_3x3_th_idx15= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[15];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b4.dh_logo_large_mv_cnt_th_idx15= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[15];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b4.dh_logo_zmv_cnt_diff_th_idx15= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[15];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b4.dh_logo_var_cnt_th_idx15= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[15];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_20.dh_logo_sad_ofs_idx16= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[16];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_44.dh_logo_sad_mvd_th_idx16= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[16];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b8.dh_logo_mv_th_idx16= s_pContext->_output_logo_ctrl.dh_logo_mv_th[16];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b8.dh_logo_zmv_cnt_th_idx16= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[16];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b8.dh_logo_mvd_3x3_cnt_th_idx16= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[16];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b8.dh_logo_mvd_3x3_th_idx16= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[16];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b8.dh_logo_large_mv_cnt_th_idx16= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[16];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b8.dh_logo_zmv_cnt_diff_th_idx16= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[16];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_b8.dh_logo_var_cnt_th_idx16= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[16];		

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_20.dh_logo_sad_ofs_idx17= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[17];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_44.dh_logo_sad_mvd_th_idx17= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[17];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_bc.dh_logo_mv_th_idx17= s_pContext->_output_logo_ctrl.dh_logo_mv_th[17];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_bc.dh_logo_zmv_cnt_th_idx17= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[17];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_bc.dh_logo_mvd_3x3_cnt_th_idx17= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[17];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_bc.dh_logo_mvd_3x3_th_idx17= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[17];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_bc.dh_logo_large_mv_cnt_th_idx17= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[17];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_bc.dh_logo_zmv_cnt_diff_th_idx17= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[17];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_bc.dh_logo_var_cnt_th_idx17= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[17];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_20.dh_logo_sad_ofs_idx18= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[18];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_48.dh_logo_sad_mvd_th_idx18= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[18];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c0.dh_logo_mv_th_idx18= s_pContext->_output_logo_ctrl.dh_logo_mv_th[18];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c0.dh_logo_zmv_cnt_th_idx18= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[18];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c0.dh_logo_mvd_3x3_cnt_th_idx18= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[18];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c0.dh_logo_mvd_3x3_th_idx18= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[18];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c0.dh_logo_large_mv_cnt_th_idx18= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[18];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c0.dh_logo_zmv_cnt_diff_th_idx18= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[18];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c0.dh_logo_var_cnt_th_idx18= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[18];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_20.dh_logo_sad_ofs_idx19= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[19];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_48.dh_logo_sad_mvd_th_idx19= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[19];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c4.dh_logo_mv_th_idx19= s_pContext->_output_logo_ctrl.dh_logo_mv_th[19];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c4.dh_logo_zmv_cnt_th_idx19= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[19];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c4.dh_logo_mvd_3x3_cnt_th_idx19= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[19];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c4.dh_logo_mvd_3x3_th_idx19= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[19];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c4.dh_logo_large_mv_cnt_th_idx19= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[19];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c4.dh_logo_zmv_cnt_diff_th_idx19= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[19];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c4.dh_logo_var_cnt_th_idx19= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[19];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_24.dh_logo_sad_ofs_idx20= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[20];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_48.dh_logo_sad_mvd_th_idx20= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[20];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c8.dh_logo_mv_th_idx20= s_pContext->_output_logo_ctrl.dh_logo_mv_th[20];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c8.dh_logo_zmv_cnt_th_idx20= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[20];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c8.dh_logo_mvd_3x3_cnt_th_idx20= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[20];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c8.dh_logo_mvd_3x3_th_idx20= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[20];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c8.dh_logo_large_mv_cnt_th_idx20= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[20];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c8.dh_logo_zmv_cnt_diff_th_idx20= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[20];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_c8.dh_logo_var_cnt_th_idx20= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[20];	

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_24.dh_logo_sad_ofs_idx21= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[21];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_4c.dh_logo_sad_mvd_th_idx21= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[21];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_cc.dh_logo_mv_th_idx21= s_pContext->_output_logo_ctrl.dh_logo_mv_th[21];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_cc.dh_logo_zmv_cnt_th_idx21= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[21];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_cc.dh_logo_mvd_3x3_cnt_th_idx21= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[21];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_cc.dh_logo_mvd_3x3_th_idx21= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[21];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_cc.dh_logo_large_mv_cnt_th_idx21= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[21];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_cc.dh_logo_zmv_cnt_diff_th_idx21= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[21];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_cc.dh_logo_var_cnt_th_idx21= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[21];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_24.dh_logo_sad_ofs_idx22= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[22];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_4c.dh_logo_sad_mvd_th_idx22= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[22];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d0.dh_logo_mv_th_idx22= s_pContext->_output_logo_ctrl.dh_logo_mv_th[22];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d0.dh_logo_zmv_cnt_th_idx22= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[22];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d0.dh_logo_mvd_3x3_cnt_th_idx22= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[22];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d0.dh_logo_mvd_3x3_th_idx22= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[22];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d0.dh_logo_large_mv_cnt_th_idx22= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[22];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d0.dh_logo_zmv_cnt_diff_th_idx22= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[22];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d0.dh_logo_var_cnt_th_idx22= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[22];		

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_24.dh_logo_sad_ofs_idx23= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[23];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_4c.dh_logo_sad_mvd_th_idx23= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[23];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d4.dh_logo_mv_th_idx23= s_pContext->_output_logo_ctrl.dh_logo_mv_th[23];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d4.dh_logo_zmv_cnt_th_idx23= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[23];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d4.dh_logo_mvd_3x3_cnt_th_idx23= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[23];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d4.dh_logo_mvd_3x3_th_idx23= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[23];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d4.dh_logo_large_mv_cnt_th_idx23= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[23];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d4.dh_logo_zmv_cnt_diff_th_idx23= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[23];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d4.dh_logo_var_cnt_th_idx23= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[23];	

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_28.dh_logo_sad_ofs_idx24= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[24];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_50.dh_logo_sad_mvd_th_idx24= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[24];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d8.dh_logo_mv_th_idx24= s_pContext->_output_logo_ctrl.dh_logo_mv_th[24];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d8.dh_logo_zmv_cnt_th_idx24= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[24];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d8.dh_logo_mvd_3x3_cnt_th_idx24= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[24];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d8.dh_logo_mvd_3x3_th_idx24= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[24];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d8.dh_logo_large_mv_cnt_th_idx24= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[24];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d8.dh_logo_zmv_cnt_diff_th_idx24= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[24];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_d8.dh_logo_var_cnt_th_idx24= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[24];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_28.dh_logo_sad_ofs_idx25= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[25];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_50.dh_logo_sad_mvd_th_idx25= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[25];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_dc.dh_logo_mv_th_idx25= s_pContext->_output_logo_ctrl.dh_logo_mv_th[25];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_dc.dh_logo_zmv_cnt_th_idx25= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[25];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_dc.dh_logo_mvd_3x3_cnt_th_idx25= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[25];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_dc.dh_logo_mvd_3x3_th_idx25= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[25];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_dc.dh_logo_large_mv_cnt_th_idx25= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[25];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_dc.dh_logo_zmv_cnt_diff_th_idx25= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[25];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_dc.dh_logo_var_cnt_th_idx25= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[25];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_28.dh_logo_sad_ofs_idx26= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[26];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_50.dh_logo_sad_mvd_th_idx26= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[26];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e0.dh_logo_mv_th_idx26= s_pContext->_output_logo_ctrl.dh_logo_mv_th[26];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e0.dh_logo_zmv_cnt_th_idx26= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[26];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e0.dh_logo_mvd_3x3_cnt_th_idx26= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[26];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e0.dh_logo_mvd_3x3_th_idx26= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[26];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e0.dh_logo_large_mv_cnt_th_idx26= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[26];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e0.dh_logo_zmv_cnt_diff_th_idx26= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[26];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e0.dh_logo_var_cnt_th_idx26= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[26];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_28.dh_logo_sad_ofs_idx27= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[27];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_54.dh_logo_sad_mvd_th_idx27= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[27];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e4.dh_logo_mv_th_idx27= s_pContext->_output_logo_ctrl.dh_logo_mv_th[27];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e4.dh_logo_zmv_cnt_th_idx27= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[27];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e4.dh_logo_mvd_3x3_cnt_th_idx27= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[27];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e4.dh_logo_mvd_3x3_th_idx27= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[27];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e4.dh_logo_large_mv_cnt_th_idx27= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[27];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e4.dh_logo_zmv_cnt_diff_th_idx27= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[27];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e4.dh_logo_var_cnt_th_idx27= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[27];	

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_2c.dh_logo_sad_ofs_idx28= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[28];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_54.dh_logo_sad_mvd_th_idx28= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[28];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e8.dh_logo_mv_th_idx28= s_pContext->_output_logo_ctrl.dh_logo_mv_th[28];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e8.dh_logo_zmv_cnt_th_idx28= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[28];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e8.dh_logo_mvd_3x3_cnt_th_idx28= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[28];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e8.dh_logo_mvd_3x3_th_idx28= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[28];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e8.dh_logo_large_mv_cnt_th_idx28= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[28];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e8.dh_logo_zmv_cnt_diff_th_idx28= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[28];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_e8.dh_logo_var_cnt_th_idx28= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[28];	

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_2c.dh_logo_sad_ofs_idx29= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[29];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_54.dh_logo_sad_mvd_th_idx29= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[29];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ec.dh_logo_mv_th_idx29= s_pContext->_output_logo_ctrl.dh_logo_mv_th[29];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ec.dh_logo_zmv_cnt_th_idx29= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[29];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ec.dh_logo_mvd_3x3_cnt_th_idx29= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[29];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ec.dh_logo_mvd_3x3_th_idx29= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[29];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ec.dh_logo_large_mv_cnt_th_idx29= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[29];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ec.dh_logo_zmv_cnt_diff_th_idx29= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[29];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_ec.dh_logo_var_cnt_th_idx29= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[29];   

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_2c.dh_logo_sad_ofs_idx30= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[30];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_58.dh_logo_sad_mvd_th_idx30= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[30];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f0.dh_logo_mv_th_idx30= s_pContext->_output_logo_ctrl.dh_logo_mv_th[30];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f0.dh_logo_zmv_cnt_th_idx30= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[30];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f0.dh_logo_mvd_3x3_cnt_th_idx30= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[30];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f0.dh_logo_mvd_3x3_th_idx30= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[30];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f0.dh_logo_large_mv_cnt_th_idx30= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[30];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f0.dh_logo_zmv_cnt_diff_th_idx30= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[30];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f0.dh_logo_var_cnt_th_idx30= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[30];

	M8P_rtme_dehalo_logo_rtme_dehalo_logo_2c.dh_logo_sad_ofs_idx31= s_pContext->_output_logo_ctrl.dh_logo_sad_ofs[31];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_58.dh_logo_sad_mvd_th_idx31= s_pContext->_output_logo_ctrl.dh_logo_sad_mvd_th[31];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f4.dh_logo_mv_th_idx31= s_pContext->_output_logo_ctrl.dh_logo_mv_th[31];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f4.dh_logo_zmv_cnt_th_idx31= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_th[31];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f4.dh_logo_mvd_3x3_cnt_th_idx31= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_cnt_th[31];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f4.dh_logo_mvd_3x3_th_idx31= s_pContext->_output_logo_ctrl.dh_logo_mvd_3x3_th[31];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f4.dh_logo_large_mv_cnt_th_idx31= s_pContext->_output_logo_ctrl.dh_logo_large_mv_cnt_th[31];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f4.dh_logo_zmv_cnt_diff_th_idx31= s_pContext->_output_logo_ctrl.dh_logo_zmv_cnt_diff_th[31];
	M8P_rtme_dehalo_logo_rtme_dehalo_logo_f4.dh_logo_var_cnt_th_idx31= s_pContext->_output_logo_ctrl.dh_logo_var_cnt_th[31];

	// write dehalo logo registers
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_00_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_00.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_04_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_04.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_08_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_08.regValue);

	// dehalo logo sad_ofs_th 32rgn reg
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_10_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_10.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_14_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_14.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_18_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_18.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_1C_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_1c.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_20_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_20.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_24_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_24.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_28_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_28.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_2C_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_2c.regValue);

	// dehalo logo sad_mvd_th 32rgn reg
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_30_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_30.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_34_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_34.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_38_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_38.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_3C_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_3c.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_40_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_40.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_44_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_44.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_48_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_48.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_4C_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_4c.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_50_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_50.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_54_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_54.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_58_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_58.regValue);

	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_74_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_74.regValue);	
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_78_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_78.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_7C_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_7c.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_80_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_80.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_84_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_84.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_88_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_88.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_8C_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_8c.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_90_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_90.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_94_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_94.regValue);	
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_98_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_98.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_9C_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_9c.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_A0_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_a0.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_A4_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_a4.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_A8_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_a8.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_AC_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_ac.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_B0_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_b0.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_B4_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_b4.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_B8_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_b8.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_BC_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_bc.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_C0_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_c0.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_C4_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_c4.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_C8_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_c8.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_CC_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_cc.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_D0_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_d0.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_D4_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_d4.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_D8_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_d8.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_DC_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_dc.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_E0_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_e0.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_E4_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_e4.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_E8_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_e8.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_EC_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_ec.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_F0_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_f0.regValue);
	rtd_outl(M8P_RTME_DEHALO_LOGO_RTME_DEHALO_LOGO_F4_reg, M8P_rtme_dehalo_logo_rtme_dehalo_logo_f4.regValue);

}
   
void WriteComReg_HME1_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	// define section
	M8P_rtme_hme1_top0_rtme_hme1_top0_54_RBUS hme1_top0_54_reg;
	M8P_rtme_hme1_top0_rtme_hme1_top0_58_RBUS hme1_top0_58_reg;
	M8P_rtme_hme1_top0_rtme_hme1_top0_5c_RBUS hme1_top0_5c_reg;
	M8P_rtme_hme1_top0_rtme_hme1_top0_b0_RBUS hme1_top0_b0_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top1_54_RBUS hme1_top1_54_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top1_58_RBUS hme1_top1_58_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top1_5c_RBUS hme1_top1_5c_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top1_b0_RBUS hme1_top1_b0_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_54_RBUS hme1_top2_54_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_58_RBUS hme1_top2_58_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_5c_RBUS hme1_top2_5c_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_b0_RBUS hme1_top2_b0_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_54_RBUS hme1_top3_54_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_58_RBUS hme1_top3_58_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_5c_RBUS hme1_top3_5c_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_90_RBUS hme1_top3_90_reg;

	M8P_rtme_hme1_top11_me1_freq_avgbv_00_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_00_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_01_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_01_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_02_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_02_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_03_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_03_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_04_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_04_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_05_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_05_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_06_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_06_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_07_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_07_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_10_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_10_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_11_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_11_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_12_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_12_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_13_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_13_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_14_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_14_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_15_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_15_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_16_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_16_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_17_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_17_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_20_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_20_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_21_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_21_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_22_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_22_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_23_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_23_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_24_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_24_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_25_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_25_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_26_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_26_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_27_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_27_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_30_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_30_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_31_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_31_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_32_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_32_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_33_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_33_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_34_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_34_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_35_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_35_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_36_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_36_reg;
	M8P_rtme_hme1_top11_me1_freq_avgbv_37_RBUS M8P_rtme_hme1_top11_me1_freq_avgbv_37_reg;
	M8P_rtme_hme1_top4_rtme_hme1_sobj_clr_00_RBUS M8P_rtme_hme1_top4_rtme_hme1_sobj_clr_00_reg;
	M8P_rtme_hme1_top4_rtme_hme1_sobj_clr_01_RBUS M8P_rtme_hme1_top4_rtme_hme1_sobj_clr_01_reg;
	// define section end
	unsigned int print_in_func;
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	// read section
	hme1_top0_54_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_54_reg);
	hme1_top0_58_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_58_reg);
	hme1_top0_5c_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_5C_reg);
	hme1_top0_b0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_B0_reg);
	hme1_top1_54_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_54_reg);
	hme1_top1_58_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_58_reg);
	hme1_top1_5c_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_5C_reg);
	hme1_top1_b0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_B0_reg);
	hme1_top2_54_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_54_reg);
	hme1_top2_58_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_58_reg);
	hme1_top2_5c_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_5C_reg);	
	hme1_top2_b0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_B0_reg);
	hme1_top3_54_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_54_reg);
	hme1_top3_58_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_58_reg);
	hme1_top3_5c_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_5C_reg);
	hme1_top3_90_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_90_reg);

	M8P_rtme_hme1_top11_me1_freq_avgbv_00_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_00_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_01_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_01_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_02_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_02_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_03_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_03_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_04_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_04_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_05_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_05_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_06_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_06_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_07_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_07_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_10_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_10_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_11_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_11_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_12_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_12_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_13_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_13_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_14_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_14_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_15_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_15_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_16_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_16_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_17_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_17_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_20_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_20_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_21_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_21_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_22_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_22_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_23_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_23_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_24_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_24_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_25_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_25_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_26_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_26_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_27_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_27_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_30_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_30_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_31_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_31_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_32_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_32_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_33_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_33_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_34_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_34_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_35_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_35_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_36_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_36_reg);
	M8P_rtme_hme1_top11_me1_freq_avgbv_37_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_37_reg);
	M8P_rtme_hme1_top4_rtme_hme1_sobj_clr_00_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP4_RTME_HME1_SOBJ_CLR_00_reg);
	M8P_rtme_hme1_top4_rtme_hme1_sobj_clr_01_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP4_RTME_HME1_SOBJ_CLR_01_reg);
	// read section end


	// apply section
	if ((pParam->u1_RimCtrl_wrt_en== 1) && (s_pContext->_output_rimctrl.u1_RimChange == 1))
	{
		// layer0_block
		hme1_top0_54_reg.me1_layer0_blk_rim_top= s_pContext->_output_rimctrl.u12_hme_layer0_blk_rim[_RIM_TOP];
		hme1_top0_54_reg.me1_layer0_blk_rim_bot= s_pContext->_output_rimctrl.u12_hme_layer0_blk_rim[_RIM_BOT];
		hme1_top0_54_reg.me1_layer0_blk_rim_lft= s_pContext->_output_rimctrl.u12_hme_layer0_blk_rim[_RIM_LFT];
		hme1_top0_58_reg.me1_layer0_blk_rim_rht= s_pContext->_output_rimctrl.u12_hme_layer0_blk_rim[_RIM_RHT];
		// layer0_pixel
		hme1_top0_58_reg.me1_layer0_pix_rim_top= s_pContext->_output_rimctrl.u12_hme_layer0_pix_rim[_RIM_TOP];
		hme1_top0_58_reg.me1_layer0_pix_rim_bot= s_pContext->_output_rimctrl.u12_hme_layer0_pix_rim[_RIM_BOT];
		hme1_top0_5c_reg.me1_layer0_pix_rim_lft= s_pContext->_output_rimctrl.u12_hme_layer0_pix_rim[_RIM_LFT];
		hme1_top0_5c_reg.me1_layer0_pix_rim_rht= s_pContext->_output_rimctrl.u12_hme_layer0_pix_rim[_RIM_RHT];
		// layer1_block
		hme1_top1_54_reg.me1_layer1_blk_rim_top= s_pContext->_output_rimctrl.u12_hme_layer1_blk_rim[_RIM_TOP];
		hme1_top1_54_reg.me1_layer1_blk_rim_bot= s_pContext->_output_rimctrl.u12_hme_layer1_blk_rim[_RIM_BOT];
		hme1_top1_54_reg.me1_layer1_blk_rim_lft= s_pContext->_output_rimctrl.u12_hme_layer1_blk_rim[_RIM_LFT];
		hme1_top1_58_reg.me1_layer1_blk_rim_rht= s_pContext->_output_rimctrl.u12_hme_layer1_blk_rim[_RIM_RHT];
		// layer1_pixel
		hme1_top1_58_reg.me1_layer1_pix_rim_top= s_pContext->_output_rimctrl.u12_hme_layer1_pix_rim[_RIM_TOP];
		hme1_top1_58_reg.me1_layer1_pix_rim_bot= s_pContext->_output_rimctrl.u12_hme_layer1_pix_rim[_RIM_BOT];
		hme1_top1_5c_reg.me1_layer1_pix_rim_lft= s_pContext->_output_rimctrl.u12_hme_layer1_pix_rim[_RIM_LFT];
		hme1_top1_5c_reg.me1_layer1_pix_rim_rht= s_pContext->_output_rimctrl.u12_hme_layer1_pix_rim[_RIM_RHT];
		// layer2_block
		hme1_top2_54_reg.me1_layer2_blk_rim_top= s_pContext->_output_rimctrl.u12_hme_layer2_blk_rim[_RIM_TOP];
		hme1_top2_54_reg.me1_layer2_blk_rim_bot= s_pContext->_output_rimctrl.u12_hme_layer2_blk_rim[_RIM_BOT];
		hme1_top2_54_reg.me1_layer2_blk_rim_lft= s_pContext->_output_rimctrl.u12_hme_layer2_blk_rim[_RIM_LFT];
		hme1_top2_58_reg.me1_layer2_blk_rim_rht= s_pContext->_output_rimctrl.u12_hme_layer2_blk_rim[_RIM_RHT];
		// layer2_pixel
		hme1_top2_58_reg.me1_layer2_pix_rim_top= s_pContext->_output_rimctrl.u12_hme_layer2_pix_rim[_RIM_TOP];
		hme1_top2_58_reg.me1_layer2_pix_rim_bot= s_pContext->_output_rimctrl.u12_hme_layer2_pix_rim[_RIM_BOT];
		hme1_top2_5c_reg.me1_layer2_pix_rim_lft= s_pContext->_output_rimctrl.u12_hme_layer2_pix_rim[_RIM_LFT];
		hme1_top2_5c_reg.me1_layer2_pix_rim_rht= s_pContext->_output_rimctrl.u12_hme_layer2_pix_rim[_RIM_RHT];	
		// layer3_block
		hme1_top3_54_reg.me1_layer3_blk_rim_top= s_pContext->_output_rimctrl.u12_hme_layer3_blk_rim[_RIM_TOP];
		hme1_top3_54_reg.me1_layer3_blk_rim_bot= s_pContext->_output_rimctrl.u12_hme_layer3_blk_rim[_RIM_BOT];
		hme1_top3_54_reg.me1_layer3_blk_rim_lft= s_pContext->_output_rimctrl.u12_hme_layer3_blk_rim[_RIM_LFT];
		hme1_top3_58_reg.me1_layer3_blk_rim_rht= s_pContext->_output_rimctrl.u12_hme_layer3_blk_rim[_RIM_RHT];
		// layer3_pixel
		hme1_top3_58_reg.me1_layer3_pix_rim_top= s_pContext->_output_rimctrl.u12_hme_layer3_pix_rim[_RIM_TOP];
		hme1_top3_58_reg.me1_layer3_pix_rim_bot= s_pContext->_output_rimctrl.u12_hme_layer3_pix_rim[_RIM_BOT];
		hme1_top3_5c_reg.me1_layer3_pix_rim_lft= s_pContext->_output_rimctrl.u12_hme_layer3_pix_rim[_RIM_LFT];
		hme1_top3_5c_reg.me1_layer3_pix_rim_rht= s_pContext->_output_rimctrl.u12_hme_layer3_pix_rim[_RIM_RHT];
	}

	if (pParam->u1_PeriodicCtrl_avgbv_wrt_en == 1)
	{
		M8P_rtme_hme1_top11_me1_freq_avgbv_00_reg.me1_freq_avgbv_en_00 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[ 0] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_01_reg.me1_freq_avgbv_en_01 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[ 1] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_02_reg.me1_freq_avgbv_en_02 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[ 2] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_03_reg.me1_freq_avgbv_en_03 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[ 3] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_04_reg.me1_freq_avgbv_en_04 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[ 4] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_05_reg.me1_freq_avgbv_en_05 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[ 5] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_06_reg.me1_freq_avgbv_en_06 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[ 6] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_07_reg.me1_freq_avgbv_en_07 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[ 7] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_10_reg.me1_freq_avgbv_en_10 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[ 8] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_11_reg.me1_freq_avgbv_en_11 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[ 9] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_12_reg.me1_freq_avgbv_en_12 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[10] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_13_reg.me1_freq_avgbv_en_13 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[11] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_14_reg.me1_freq_avgbv_en_14 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[12] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_15_reg.me1_freq_avgbv_en_15 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[13] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_16_reg.me1_freq_avgbv_en_16 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[14] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_17_reg.me1_freq_avgbv_en_17 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[15] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_20_reg.me1_freq_avgbv_en_20 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[16] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_21_reg.me1_freq_avgbv_en_21 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[17] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_22_reg.me1_freq_avgbv_en_22 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[18] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_23_reg.me1_freq_avgbv_en_23 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[19] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_24_reg.me1_freq_avgbv_en_24 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[10] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_25_reg.me1_freq_avgbv_en_25 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[21] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_26_reg.me1_freq_avgbv_en_26 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[22] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_27_reg.me1_freq_avgbv_en_27 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[23] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_30_reg.me1_freq_avgbv_en_30 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[24] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_31_reg.me1_freq_avgbv_en_31 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[25] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_32_reg.me1_freq_avgbv_en_32 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[26] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_33_reg.me1_freq_avgbv_en_33 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[27] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_34_reg.me1_freq_avgbv_en_34 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[28] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_35_reg.me1_freq_avgbv_en_35 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[29] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_36_reg.me1_freq_avgbv_en_36 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[20] < 100)?1:0;
		M8P_rtme_hme1_top11_me1_freq_avgbv_37_reg.me1_freq_avgbv_en_37 = (s_pContext->_output_periodic_manage.u32_avgbv_unconf[31] < 100)?1:0;
	}

	if (pParam->u1_PeriodicCtrl_frq_wrt_en == 1)
	{
		hme1_top0_b0_reg.me1_layer0_frq_en = s_pContext->_output_periodic_manage.u1_layer0_frq_en;
		hme1_top1_b0_reg.me1_layer1_frq_en = s_pContext->_output_periodic_manage.u1_layer1_frq_en;
		hme1_top2_b0_reg.me1_layer2_frq_en = s_pContext->_output_periodic_manage.u1_layer2_frq_en;
		hme1_top3_90_reg.me1_layer3_frq_en = s_pContext->_output_periodic_manage.u1_layer3_frq_en;
	}

	if(pParam->u1_SobjCtrl_wrt_en == 1)	
	{
		M8P_rtme_hme1_top4_rtme_hme1_sobj_clr_00_reg.regValue = s_pContext->_output_sobj_ctrl.u32_Sobj_rgn_i_clr;
		M8P_rtme_hme1_top4_rtme_hme1_sobj_clr_01_reg.regValue = s_pContext->_output_sobj_ctrl.u32_Sobj_rgn_p_clr;
	}	
	// apply section end

	// write section
	rtd_outl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_54_reg, hme1_top0_54_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_58_reg, hme1_top0_58_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_5C_reg, hme1_top0_5c_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_B0_reg, hme1_top0_b0_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_54_reg, hme1_top1_54_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_58_reg, hme1_top1_58_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_5C_reg, hme1_top1_5c_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_B0_reg, hme1_top1_b0_reg.regValue);	
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_54_reg, hme1_top2_54_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_58_reg, hme1_top2_58_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_5C_reg, hme1_top2_5c_reg.regValue);	
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_B0_reg, hme1_top2_b0_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_54_reg, hme1_top3_54_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_58_reg, hme1_top3_58_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_5C_reg, hme1_top3_5c_reg.regValue);	
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_90_reg, hme1_top3_90_reg.regValue);

	
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_00_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_00_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_01_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_01_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_02_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_02_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_03_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_03_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_04_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_04_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_05_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_05_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_06_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_06_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_07_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_07_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_10_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_10_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_11_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_11_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_12_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_12_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_13_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_13_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_14_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_14_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_15_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_15_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_16_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_16_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_17_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_17_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_20_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_20_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_21_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_21_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_22_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_22_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_23_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_23_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_24_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_24_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_25_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_25_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_26_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_26_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_27_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_27_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_30_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_30_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_31_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_31_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_32_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_32_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_33_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_33_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_34_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_34_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_35_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_35_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_36_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_36_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP11_me1_freq_avgbv_37_reg, M8P_rtme_hme1_top11_me1_freq_avgbv_37_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP4_RTME_HME1_SOBJ_CLR_00_reg, M8P_rtme_hme1_top4_rtme_hme1_sobj_clr_00_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP4_RTME_HME1_SOBJ_CLR_01_reg, M8P_rtme_hme1_top4_rtme_hme1_sobj_clr_01_reg.regValue);
	// write section end


	HME1_3DRS_wrtAction_RTK2885pp(pParam, pOutput);
	HME1_Near_Freq_wrtAction_RTK2885pp(pParam, pOutput);
	HME1_Mvmask_wrtAction_RTK2885pp(pParam, pOutput);
	HME1_Freq_push_wrtAction_RTK2885pp(pParam, pOutput);




	

}

void WriteComReg_ME15_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	unsigned int print_in_func;
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	ME15_Invalid_wrtAction_RTK2885pp(pParam, pOutput);
}

void WriteComReg_ME2_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	// define section
	M8P_rtme_me2_calc0_rtme_me2_calc0_60_RBUS me2_calc0_60_reg;
	M8P_rtme_me2_calc0_rtme_me2_calc0_64_RBUS me2_calc0_64_reg;
	// define section end
	unsigned int print_in_func;
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	// read section
	me2_calc0_60_reg.regValue = rtd_inl(M8P_RTME_ME2_CALC0_RTME_ME2_CALC0_60_reg);
	me2_calc0_64_reg.regValue = rtd_inl(M8P_RTME_ME2_CALC0_RTME_ME2_CALC0_64_reg);
	// read section end


	if ((pParam->u1_Wrt_ComReg_ME2_en == 1) && (s_pContext->_output_rimctrl.u1_RimChange == 1))
	{
		// apply section
		me2_calc0_60_reg.me2_blk_rim_top = s_pContext->_output_rimctrl.u12_me2_blk_rim[_RIM_TOP];
		me2_calc0_60_reg.me2_blk_rim_bot = s_pContext->_output_rimctrl.u12_me2_blk_rim[_RIM_BOT];
		me2_calc0_64_reg.me2_blk_rim_lft = s_pContext->_output_rimctrl.u12_me2_blk_rim[_RIM_LFT];
		me2_calc0_64_reg.me2_blk_rim_rht = s_pContext->_output_rimctrl.u12_me2_blk_rim[_RIM_RHT];
		// apply section end
	}

	// write section
	rtd_outl(M8P_RTME_ME2_CALC0_RTME_ME2_CALC0_60_reg, me2_calc0_60_reg.regValue);
	rtd_outl(M8P_RTME_ME2_CALC0_RTME_ME2_CALC0_64_reg, me2_calc0_64_reg.regValue);
	// write section end

	ME2_3DRS_wrtAction_RTK2885pp(pParam, pOutput);
	ME2_BrokenFix_wrtAction_RTK2885pp(pParam, pOutput);
}

void WriteComReg_DH_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	// define section
	M8P_rtme_dehalo0_rtme_dehalo0_dc_RBUS dehalo0_dc_reg;
	M8P_rtme_dehalo0_rtme_dehalo0_e0_RBUS dehalo0_e0_reg;
	// define section end
	unsigned int print_in_func;
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	// read section
	dehalo0_dc_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_DC_reg);
	dehalo0_e0_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_E0_reg);
	// read section end


	if ((pParam->u1_Wrt_ComReg_DH_en == 1) && (s_pContext->_output_rimctrl.u1_RimChange == 1))
	{
		// apply section
		dehalo0_dc_reg.dh_rim_blk_top = s_pContext->_output_rimctrl.u12_dh_blk_rim[_RIM_TOP];
		dehalo0_dc_reg.dh_rim_blk_bot = s_pContext->_output_rimctrl.u12_dh_blk_rim[_RIM_BOT];
		dehalo0_dc_reg.dh_rim_blk_lft = s_pContext->_output_rimctrl.u12_dh_blk_rim[_RIM_LFT];
		dehalo0_e0_reg.dh_rim_blk_rht = s_pContext->_output_rimctrl.u12_dh_blk_rim[_RIM_RHT];
		// apply section end
	}

	// write section
	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_DC_reg, dehalo0_dc_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_E0_reg, dehalo0_e0_reg.regValue);
	// write section end

	Dehalo_OCCL_wrtAction_RTK2885pp(pParam, pOutput);
	Dehalo_Occl_Mvpred_wetAction_RTK2885pp(pParam, pOutput);
}

void WriteComReg_MC_RTK2885pp(const _PARAM_WRT_COM_REG *pParam)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	// define section
	M8P_rtmc_rtmc_10_RBUS M8P_rtmc_rtmc_10_reg;
	M8P_rtmc_rtmc_14_RBUS M8P_rtmc_rtmc_14_reg;
	M8P_rtmc_rtmc_18_RBUS M8P_rtmc_rtmc_18_reg;
	M8P_rtmc_rtmc_68_RBUS M8P_rtmc_rtmc_68_reg;
	M8P_rtmc_rtmc_1c_RBUS M8P_rtmc_rtmc_1c_reg;
	M8P_rtmc_rtmc_ec_RBUS M8P_rtmc_rtmc_ec_reg;
	M8P_rtmc_rtmc_f0_RBUS M8P_rtmc_rtmc_f0_reg;
	M8P_rtmc_rtmc_f4_RBUS M8P_rtmc_rtmc_f4_reg;
	M8P_rtmc_rtmc_f8_RBUS M8P_rtmc_rtmc_f8_reg;
	M8P_rtmc_rtmc_fc_RBUS M8P_rtmc_rtmc_fc_reg;
	M8P_rtmc_lbmc_lbmc_ctrl_RBUS M8P_rtmc_lbmc_lbmc_ctrl_reg;
	M8P_rtmc_mc_sobj_04_RBUS M8P_rtmc_mc_sobj_04_reg;
	M8P_rtmc_mc_sobj_08_RBUS M8P_rtmc_mc_sobj_08_reg;
	M8P_rtmc_mc_sobj_0c_RBUS M8P_rtmc_mc_sobj_0c_reg;
	M8P_rtmc_mc_sobj_10_RBUS M8P_rtmc_mc_sobj_10_reg;
	M8P_rtmc_mc_sobj_01_RBUS M8P_rtmc_mc_sobj_01_reg;
	M8P_rtmc_mc_sobj_02_RBUS M8P_rtmc_mc_sobj_02_reg;   
	
	// define section end
	unsigned int print_in_func;
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	// read section
	M8P_rtmc_rtmc_10_reg.regValue = rtd_inl(M8P_RTMC_RTMC_10_reg);
	M8P_rtmc_rtmc_14_reg.regValue = rtd_inl(M8P_RTMC_RTMC_14_reg);
	M8P_rtmc_rtmc_18_reg.regValue = rtd_inl(M8P_RTMC_RTMC_18_reg);
	M8P_rtmc_rtmc_68_reg.regValue = rtd_inl(M8P_RTMC_RTMC_68_reg);
	M8P_rtmc_rtmc_1c_reg.regValue = rtd_inl(M8P_RTMC_RTMC_1C_reg);
	M8P_rtmc_rtmc_ec_reg.regValue = rtd_inl(M8P_RTMC_RTMC_EC_reg);
	M8P_rtmc_rtmc_f0_reg.regValue = rtd_inl(M8P_RTMC_RTMC_F0_reg);
	M8P_rtmc_rtmc_f4_reg.regValue = rtd_inl(M8P_RTMC_RTMC_F4_reg);
	M8P_rtmc_rtmc_f8_reg.regValue = rtd_inl(M8P_RTMC_RTMC_F8_reg);
	M8P_rtmc_rtmc_fc_reg.regValue = rtd_inl(M8P_RTMC_RTMC_FC_reg);
	M8P_rtmc_lbmc_lbmc_ctrl_reg.regValue = rtd_inl(M8P_RTMC_LBMC_LBMC_CTRL_reg);
	M8P_rtmc_mc_sobj_04_reg.regValue = rtd_inl(M8P_RTMC_MC_SOBJ_04_reg);
	M8P_rtmc_mc_sobj_08_reg.regValue = rtd_inl(M8P_RTMC_MC_SOBJ_08_reg);
	M8P_rtmc_mc_sobj_0c_reg.regValue = rtd_inl(M8P_RTMC_MC_SOBJ_0C_reg);
	M8P_rtmc_mc_sobj_10_reg.regValue = rtd_inl(M8P_RTMC_MC_SOBJ_10_reg);
	M8P_rtmc_mc_sobj_01_reg.regValue = rtd_inl(M8P_RTMC_MC_SOBJ_01_reg);
	M8P_rtmc_mc_sobj_02_reg.regValue = rtd_inl(M8P_RTMC_MC_SOBJ_02_reg);	
	// read section end


	// apply section
	if ((pParam->u1_RimCtrl_wrt_en == 1) && (s_pContext->_output_rimctrl.u1_RimChange == 1))
	{
		M8P_rtmc_rtmc_10_reg.mc_top_i_rim0 = s_pContext->_output_rimctrl.u12_mc_i_rim0[_RIM_TOP];
		M8P_rtmc_rtmc_10_reg.mc_bot_i_rim0 = s_pContext->_output_rimctrl.u12_mc_i_rim0[_RIM_BOT];
		M8P_rtmc_rtmc_14_reg.mc_rht_i_rim0 = s_pContext->_output_rimctrl.u12_mc_i_rim0[_RIM_RHT];
		M8P_rtmc_rtmc_14_reg.mc_lft_i_rim0 = s_pContext->_output_rimctrl.u12_mc_i_rim0[_RIM_LFT];
		M8P_rtmc_rtmc_18_reg.mc_top_i_rim1 = s_pContext->_output_rimctrl.u12_mc_i_rim1[_RIM_TOP];
		M8P_rtmc_rtmc_18_reg.mc_bot_i_rim1 = s_pContext->_output_rimctrl.u12_mc_i_rim1[_RIM_BOT];
		M8P_rtmc_rtmc_1c_reg.mc_rht_i_rim1 = s_pContext->_output_rimctrl.u12_mc_i_rim1[_RIM_RHT];
		M8P_rtmc_rtmc_1c_reg.mc_lft_i_rim1 = s_pContext->_output_rimctrl.u12_mc_i_rim1[_RIM_LFT];
		M8P_rtmc_rtmc_f0_reg.mc_top_p_rim0 = s_pContext->_output_rimctrl.u12_mc_p_rim0[_RIM_TOP];
		M8P_rtmc_rtmc_f0_reg.mc_bot_p_rim0 = s_pContext->_output_rimctrl.u12_mc_p_rim0[_RIM_BOT];
		M8P_rtmc_rtmc_f4_reg.mc_rht_p_rim0 = s_pContext->_output_rimctrl.u12_mc_p_rim0[_RIM_RHT];
		M8P_rtmc_rtmc_f4_reg.mc_lft_p_rim0 = s_pContext->_output_rimctrl.u12_mc_p_rim0[_RIM_LFT];
		M8P_rtmc_rtmc_f8_reg.mc_top_p_rim1 = s_pContext->_output_rimctrl.u12_mc_p_rim1[_RIM_TOP];
		M8P_rtmc_rtmc_f8_reg.mc_bot_p_rim1 = s_pContext->_output_rimctrl.u12_mc_p_rim1[_RIM_BOT];
		M8P_rtmc_rtmc_fc_reg.mc_rht_p_rim1 = s_pContext->_output_rimctrl.u12_mc_p_rim1[_RIM_RHT];
		M8P_rtmc_rtmc_fc_reg.mc_lft_p_rim1 = s_pContext->_output_rimctrl.u12_mc_p_rim1[_RIM_LFT];

		M8P_rtmc_rtmc_68_reg.mc_invalid_rim_oosr_clear_en = s_pContext->_output_rimctrl.u1_mc_invalid_rim_oosr_clear_en;
		M8P_rtmc_rtmc_68_reg.mc_fetch_data_rim_clip = s_pContext->_output_rimctrl.u1_mc_fetch_data_rim_clip;
		M8P_rtmc_rtmc_68_reg.mc_fetch_data_rim_type = s_pContext->_output_rimctrl.u1_mc_fetch_data_rim_type;

		M8P_rtmc_rtmc_ec_reg.mc_fetch_data_rim_offset_top = s_pContext->_output_rimctrl.u12_mc_fetch_data_rim_offset[_RIM_TOP];
		M8P_rtmc_rtmc_ec_reg.mc_fetch_data_rim_offset_bot = s_pContext->_output_rimctrl.u12_mc_fetch_data_rim_offset[_RIM_BOT];
		M8P_rtmc_rtmc_ec_reg.mc_fetch_data_rim_offset_rht = s_pContext->_output_rimctrl.u12_mc_fetch_data_rim_offset[_RIM_RHT];
		M8P_rtmc_rtmc_ec_reg.mc_fetch_data_rim_offset_lft = s_pContext->_output_rimctrl.u12_mc_fetch_data_rim_offset[_RIM_LFT];
	}


	if(1) //--------------LBMC_Ctrl
	{
		M8P_rtmc_lbmc_lbmc_ctrl_reg.lbmc_mode_lf = s_pContext->_output_mc_lbmcswitch.u8_lf_lbmcMode + 8;
		M8P_rtmc_lbmc_lbmc_ctrl_reg.lbmc_mode_hf = s_pContext->_output_mc_lbmcswitch.u8_hf_lbmcMode + 8;
	}

	if(pParam->u1_SobjCtrl_wrt_en) //--------------Sobj_Ctrl
	{
		//-------- 32 regional level of median filter  -------------
		M8P_rtmc_mc_sobj_04_reg.regValue = s_pContext->_output_sobj_ctrl.u32_Sobj_med_lvl[0];
		M8P_rtmc_mc_sobj_08_reg.regValue = s_pContext->_output_sobj_ctrl.u32_Sobj_med_lvl[1];
		M8P_rtmc_mc_sobj_0c_reg.regValue = s_pContext->_output_sobj_ctrl.u32_Sobj_med_lvl[2];
		M8P_rtmc_mc_sobj_10_reg.regValue = s_pContext->_output_sobj_ctrl.u32_Sobj_med_lvl[3];
		//-------- median mapping curve --------------
		M8P_rtmc_mc_sobj_01_reg.mc_medflt_curve_map_x0	= s_pContext->_output_sobj_ctrl.u8_med_curve[0];
		M8P_rtmc_mc_sobj_01_reg.mc_medflt_curve_map_x1	= s_pContext->_output_sobj_ctrl.u8_med_curve[1];
		M8P_rtmc_mc_sobj_01_reg.mc_medflt_curve_map_x2	= s_pContext->_output_sobj_ctrl.u8_med_curve[2];
		M8P_rtmc_mc_sobj_01_reg.mc_medflt_curve_map_y0	= s_pContext->_output_sobj_ctrl.u8_med_curve[3];
		M8P_rtmc_mc_sobj_02_reg.mc_medflt_curve_map_y1	= s_pContext->_output_sobj_ctrl.u8_med_curve[4];
		M8P_rtmc_mc_sobj_02_reg.mc_medflt_curve_map_y2	= s_pContext->_output_sobj_ctrl.u8_med_curve[5];
		M8P_rtmc_mc_sobj_02_reg.mc_medflt_curve_map_slope0= s_pContext->_output_sobj_ctrl.u8_med_curve[6];
		M8P_rtmc_mc_sobj_02_reg.mc_medflt_curve_map_slope1= s_pContext->_output_sobj_ctrl.u8_med_curve[7];  
	}	
	// apply section end


	// write section
	rtd_outl(M8P_RTMC_RTMC_10_reg, M8P_rtmc_rtmc_10_reg.regValue);
	rtd_outl(M8P_RTMC_RTMC_14_reg, M8P_rtmc_rtmc_14_reg.regValue);
	rtd_outl(M8P_RTMC_RTMC_18_reg, M8P_rtmc_rtmc_18_reg.regValue);
	rtd_outl(M8P_RTMC_RTMC_1C_reg, M8P_rtmc_rtmc_1c_reg.regValue);
	rtd_outl(M8P_RTMC_RTMC_68_reg, M8P_rtmc_rtmc_68_reg.regValue);
	rtd_outl(M8P_RTMC_RTMC_EC_reg, M8P_rtmc_rtmc_ec_reg.regValue);
	rtd_outl(M8P_RTMC_RTMC_F0_reg, M8P_rtmc_rtmc_f0_reg.regValue);
	rtd_outl(M8P_RTMC_RTMC_F4_reg, M8P_rtmc_rtmc_f4_reg.regValue);
	rtd_outl(M8P_RTMC_RTMC_F8_reg, M8P_rtmc_rtmc_f8_reg.regValue);
	rtd_outl(M8P_RTMC_RTMC_FC_reg, M8P_rtmc_rtmc_fc_reg.regValue);
	rtd_outl(M8P_RTMC_LBMC_LBMC_CTRL_reg, M8P_rtmc_lbmc_lbmc_ctrl_reg.regValue);
	rtd_outl(M8P_RTMC_MC_SOBJ_04_reg, M8P_rtmc_mc_sobj_04_reg.regValue);
	rtd_outl(M8P_RTMC_MC_SOBJ_08_reg, M8P_rtmc_mc_sobj_08_reg.regValue);
	rtd_outl(M8P_RTMC_MC_SOBJ_0C_reg, M8P_rtmc_mc_sobj_0c_reg.regValue);
	rtd_outl(M8P_RTMC_MC_SOBJ_10_reg, M8P_rtmc_mc_sobj_10_reg.regValue);
	rtd_outl(M8P_RTMC_MC_SOBJ_01_reg, M8P_rtmc_mc_sobj_01_reg.regValue);
	rtd_outl(M8P_RTMC_MC_SOBJ_02_reg, M8P_rtmc_mc_sobj_02_reg.regValue);	
	// write section end

	MC_VAR_LPF_wrtAction_RTK2885pp(pParam);
	MC_Deflicker_wrtAction_RTK2885pp(pParam);
	MC_Gfblvl_wrtAction_RTK2885pp(pParam);
}

void WriteComReg_LogPrint_RTK2885pp(void)
{
	 const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int cur_cadence = 0;
	unsigned int me_write = 0, me1_read_i = 0, me1_read_p = 0, me2_read_i = 0, me2_read_p = 0;
	unsigned int mc_write = 0, mc_read_i = 0, mc_read_p = 0;
	unsigned int me15_read_i = 0, me15_read_p = 0;
	unsigned int me2_phase = 0, mc_phase = 0;
	unsigned int log_en = 0, log_en2 = 0;
	PQL_OUTPUT_FRAME_RATE  out_fmRate = 0;
	PQL_INPUT_FRAME_RATE	in_fmRate = 0;
	unsigned int in_phase = 0, out_phase = 0;
	unsigned int in_film_phase = 0, out_film_phase = 0;
	unsigned int me1_last = 0;

	cur_cadence = s_pContext->_output_filmDetectctrl.u8_cur_cadence_Id[_FILM_ALL];
	ReadRegister(M8P_RTHW_PHASE_phase_A8_reg, 16, 21, &out_film_phase);
	ReadRegister(M8P_RTHW_PHASE_phase_A8_reg, 24, 29, &out_phase);
	ReadRegister(M8P_RTHW_PHASE_phase_A4_reg, 20, 25, &in_film_phase);
	ReadRegister(M8P_RTHW_PHASE_phase_A4_reg, 16, 19, &in_phase);
	ReadRegister(M8P_RTHW_PHASE_phase_A4_reg, 12, 15, &me_write);
	ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 28, 31, &me1_read_i);
	ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 24, 27, &me1_read_p);
	ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 20, 23, &me15_read_i);
	ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 16, 19, &me15_read_p);
	ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 12, 15, &me2_read_i);
	ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 8, 11, &me2_read_p);
	ReadRegister(M8P_RTHW_PHASE_phase_A4_reg, 4, 7, &mc_write);
	ReadRegister(M8P_RTHW_PHASE_phase_A8_reg, 12, 15, &mc_read_i);
	ReadRegister(M8P_RTHW_PHASE_phase_A8_reg, 8, 11, &mc_read_p);
	ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 0, 6, &me2_phase);
	ReadRegister(M8P_RTHW_PHASE_phase_A8_reg, 0, 6, &mc_phase);
	ReadRegister(M8P_RTHW_PHASE_phase_AC_reg, 7, 7, &me1_last);
	ReadRegister(SOFTWARE3_SOFTWARE3_63_reg, 1, 1, &log_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_63_reg, 2, 2, &log_en2);
	in_fmRate = s_pContext->_external_data._input_frameRate;
	out_fmRate = s_pContext->_external_data._output_frameRate;

	#if 1
	if(log_en){
		rtd_pr_memc_emerg("[%s][now_dma][,%d,%d,][,%d,%d,][ME1_idx,%d,%d,%d,,%d,][ME15,%d,%d,][ME2_idx,%d,%d,,%d,][MC_idx,%d,%d,%d,,%d,][Motion,%d,][Time,%d,]\n\r", __FUNCTION__,
			in_phase, out_phase, in_film_phase, out_film_phase,
			me_write, me1_read_i, me1_read_p, me1_last,
			me15_read_i, me15_read_p,
			me2_read_i, me2_read_p, me2_phase,
			mc_write, mc_read_i, mc_read_p, mc_phase,
			s_pContext->_output_read_comreg.u27_ipme_aMot_rb, 
			rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
	}
	if(log_en2){
		rtd_pr_memc_emerg("[%s][out_interrupt][,%d,%d,][,%d,][ME1_idx,%d,%d,%d,ME15,%d,%d,ME2_idx,%d,%d,][MC_idx,%d,%d,%d,][phase,%d,][Motion,%d,][Seq,%d,%x][FrameRate,%d,%d,][Time,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_filmDetectctrl.u8_det_cadence_Id[_FILM_ALL], cur_cadence, s_pContext->_output_filmDetectctrl.u8_phT_phase_Idx_out[_FILM_ALL],
			me_write, me1_read_i, me1_read_p, me15_read_i, me15_read_p, me2_read_i, me2_read_p,
			mc_write, mc_read_i, mc_read_p, mc_phase,
			s_pContext->_output_read_comreg.u27_ipme_aMot_rb, 
			s_pContext->_output_filmDetectctrl.u32_Mot_sequence[_FILM_ALL]&0x1, s_pContext->_output_filmDetectctrl.u32_Mot_sequence[_FILM_ALL]&0xFFFF, 
			in_fmRate, out_fmRate, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
	}
	#endif
}

void WriteComReg_MISC_intputInterrupt_RTK2885pp(const _PARAM_WRT_COM_REG *pParam)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int u32_dbg_mode, u32_dbgshow_gain;
	signed int u32_dbgshow_u[32], u32_dbgshow_v[32];
	unsigned int u32_i=0;
	for (u32_i = 0; u32_i < 32; u32_i ++)
	{
		u32_dbgshow_u[u32_i] = 0;
		u32_dbgshow_v[u32_i] = 0;
	}

	ReadRegister(SOFTWARE3_SOFTWARE3_00_reg, 0, 7, &u32_dbg_mode);
	ReadRegister(SOFTWARE3_SOFTWARE3_00_reg, 8, 12, &u32_dbgshow_gain);

	
	if (u32_dbg_mode == 0)
	{
		return;
	}

	// -------------------------start of dbg mode select--------------------------------
	// show gmv_mv
	else if (u32_dbg_mode == 1)
	{
		u32_dbgshow_u[0] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb) << u32_dbgshow_gain) >> 5);
		u32_dbgshow_v[0] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb) << u32_dbgshow_gain) >> 5);
		u32_dbgshow_u[1] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb) << u32_dbgshow_gain) >> 5);
		u32_dbgshow_v[1] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb) << u32_dbgshow_gain) >> 5);
		u32_dbgshow_u[2] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb) << u32_dbgshow_gain) >> 5);
		u32_dbgshow_v[2] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb) << u32_dbgshow_gain) >> 5);
		
	}

	// show gmv_cnt
	else if (u32_dbg_mode == 2)
	{
		u32_dbgshow_u[0] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb) << u32_dbgshow_gain) >> 9);
		u32_dbgshow_v[0] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb) << u32_dbgshow_gain) >> 9);
		u32_dbgshow_u[1] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb) << u32_dbgshow_gain) >> 9);
		u32_dbgshow_v[1] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb) << u32_dbgshow_gain) >> 9);
		u32_dbgshow_u[2] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u16_me_GMV_3rd_cnt_rb) << u32_dbgshow_gain) >> 9);
		u32_dbgshow_v[2] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u16_me_GMV_3rd_cnt_rb) << u32_dbgshow_gain) >> 9);
	}

	// show gmv_ucf
	else if (u32_dbg_mode == 3)
	{
		u32_dbgshow_u[0] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb) << u32_dbgshow_gain) >> 3);
		u32_dbgshow_v[0] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb) << u32_dbgshow_gain) >> 3);
		u32_dbgshow_u[1] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_GMV_2nd_unconf_rb) << u32_dbgshow_gain) >> 3);
		u32_dbgshow_v[1] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_GMV_2nd_unconf_rb) << u32_dbgshow_gain) >> 3);
		u32_dbgshow_u[2] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me_GMV_3rd_unconf_rb) << u32_dbgshow_gain) >> 3);
		u32_dbgshow_v[2] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me_GMV_3rd_unconf_rb) << u32_dbgshow_gain) >> 3);
	}

	// show rmv_mv grp0
	else if (u32_dbg_mode == 4)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u32_i]) << u32_dbgshow_gain) >> 5);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u32_i]) << u32_dbgshow_gain) >> 5);
		}
	}

	// show rmv_mv grp1
	else if (u32_dbg_mode == 5)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u32_i]) << u32_dbgshow_gain) >> 5);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u32_i]) << u32_dbgshow_gain) >> 5);
		}
	}

	// show rmv_mv grp2
	else if (u32_dbg_mode == 6)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u32_i]) << u32_dbgshow_gain) >> 5);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u32_i]) << u32_dbgshow_gain) >> 5);
		}
	}

	// show rmv_cnt grp0
	else if (u32_dbg_mode == 7)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 3);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 3);
		}
	}

	// show rmv_cnt grp1
	else if (u32_dbg_mode == 8)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 3);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 3);
		}
	}

	// show rmv_cnt grp2
	else if (u32_dbg_mode == 9)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me_rMV_3rd_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 3);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me_rMV_3rd_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 3);
		}
	}

	// show rmv_ucf grp0
	else if (u32_dbg_mode == 10)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[u32_i]) << u32_dbgshow_gain) >> 3);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[u32_i]) << u32_dbgshow_gain) >> 3);
		}
	}

	// show rmv_ucf grp1
	else if (u32_dbg_mode == 11)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[u32_i]) << u32_dbgshow_gain) >> 3);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[u32_i]) << u32_dbgshow_gain) >> 3);
		}
	}

	// show rmv_ucf grp2
	else if (u32_dbg_mode == 12)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me_rMV_3rd_unconf_rb[u32_i]) << u32_dbgshow_gain) >> 3);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me_rMV_3rd_unconf_rb[u32_i]) << u32_dbgshow_gain) >> 3);
		}
	}
	
	// show u16_ippre_rgn_cnt_u_rb
	else if (u32_dbg_mode == 13)
	{
		for (u32_i = 0; u32_i < 8; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u16_ippre_rgn_cnt_u_rb[u32_i]) << u32_dbgshow_gain) >> 9);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u16_ippre_rgn_cnt_u_rb[u32_i]) << u32_dbgshow_gain) >> 9);
		}
	}
	
	// show u16_ippre_rgn_cnt_v_rb
	else if (u32_dbg_mode == 14)
	{
		for (u32_i = 0; u32_i < 8; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u16_ippre_rgn_cnt_v_rb[u32_i]) << u32_dbgshow_gain) >> 9);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u16_ippre_rgn_cnt_v_rb[u32_i]) << u32_dbgshow_gain) >> 9);
		}
	}
	
	// show u27_ipme_fdet_mot_5rgn_rb
	else if (u32_dbg_mode == 15)
	{
		for (u32_i = 0; u32_i < 5; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u27_ipme_fdet_mot_5rgn_rb[u32_i]) << u32_dbgshow_gain) >> 20);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u27_ipme_fdet_mot_5rgn_rb[u32_i]) << u32_dbgshow_gain) >> 20);
		}
	}
	
	// show u27_ipme_fdet_mot_9rgn_rb
	else if (u32_dbg_mode == 16)
	{
		for (u32_i = 0; u32_i < 9; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u27_ipme_fdet_mot_9rgn_rb[u32_i]) << u32_dbgshow_gain) >> 20);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u27_ipme_fdet_mot_9rgn_rb[u32_i]) << u32_dbgshow_gain) >> 20);
		}
	}

	// show u27_ipme_fdet_mot_16rgn_rb
	else if (u32_dbg_mode == 17)
	{
		for (u32_i = 0; u32_i < 16; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u27_ipme_fdet_mot_16rgn_rb[u32_i]) << u32_dbgshow_gain) >> 20);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u27_ipme_fdet_mot_16rgn_rb[u32_i]) << u32_dbgshow_gain) >> 20);
		}
	}

	// show u32_ipme0_film_det_rb
	else if (u32_dbg_mode == 18)
	{
		for (u32_i = 0; u32_i < 12; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u32_ipme0_film_det_rb[u32_i]) << u32_dbgshow_gain) >> 25);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u32_ipme0_film_det_rb[u32_i]) << u32_dbgshow_gain) >> 25);
		}
	}

	// show u32_ipme1_film_det_rb
	else if (u32_dbg_mode == 19)
	{
		for (u32_i = 0; u32_i < 12; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u32_ipme1_film_det_rb[u32_i]) << u32_dbgshow_gain) >> 25);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u32_ipme1_film_det_rb[u32_i]) << u32_dbgshow_gain) >> 25);
		}
	}

	// show u32_ipme2_film_det_rb
	else if (u32_dbg_mode == 20)
	{
		for (u32_i = 0; u32_i < 12; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u32_ipme2_film_det_rb[u32_i]) << u32_dbgshow_gain) >> 25);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u32_ipme2_film_det_rb[u32_i]) << u32_dbgshow_gain) >> 25);
		}
	}

	// show u32_ipme_fdet_rgn32_apl_rb
	else if (u32_dbg_mode == 21)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u32_ipme_fdet_rgn32_apl_rb[u32_i]) << u32_dbgshow_gain) >> 25);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u32_ipme_fdet_rgn32_apl_rb[u32_i]) << u32_dbgshow_gain) >> 25);
		}
	}

	// show u32_ipme_cprs_motion_diff_rb
	else if (u32_dbg_mode == 22)
	{
		for (u32_i = 0; u32_i < 8; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u32_ipme_cprs_motion_diff_rb[u32_i]) << u32_dbgshow_gain) >> 25);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u32_ipme_cprs_motion_diff_rb[u32_i]) << u32_dbgshow_gain) >> 25);
		}
	}

	// show u14_logo_rg_cnt_rb
	else if (u32_dbg_mode == 23)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u14_logo_rg_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 7);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u14_logo_rg_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 7);
		}
	}

	// show u16_logo_rg_y_rb
	else if (u32_dbg_mode == 24)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u16_logo_rg_y_rb[u32_i]) << u32_dbgshow_gain) >> 9);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u16_logo_rg_y_rb[u32_i]) << u32_dbgshow_gain) >> 9);
		}
	}

	// show u16_no_logo_rg_y_rb
	else if (u32_dbg_mode == 25)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u16_no_logo_rg_y_rb[u32_i]) << u32_dbgshow_gain) >> 9);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u16_no_logo_rg_y_rb[u32_i]) << u32_dbgshow_gain) >> 9);
		}
	}

	// show u12_logo_rg_sobel_hor_rb
	else if (u32_dbg_mode == 26)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_logo_rg_sobel_hor_rb[u32_i]) << u32_dbgshow_gain) >> 5);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_logo_rg_sobel_hor_rb[u32_i]) << u32_dbgshow_gain) >> 5);
		}
	}

	// show u12_logo_rg_sobel_ver_rb
	else if (u32_dbg_mode == 27)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_logo_rg_sobel_ver_rb[u32_i]) << u32_dbgshow_gain) >> 5);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_logo_rg_sobel_ver_rb[u32_i]) << u32_dbgshow_gain) >> 5);
		}
	}

	// show u20_me_rAPLi_rb
	else if (u32_dbg_mode == 28)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u20_me_rAPLi_rb[u32_i]) << u32_dbgshow_gain) >> 13);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u20_me_rAPLi_rb[u32_i]) << u32_dbgshow_gain) >> 13);
		}
	}

	// show u20_me_rAPLp_rb
	else if (u32_dbg_mode == 29)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u20_me_rAPLp_rb[u32_i]) << u32_dbgshow_gain) >> 13);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u20_me_rAPLp_rb[u32_i]) << u32_dbgshow_gain) >> 13);
		}
	}

	// show u10_me_freq_cnt_rb
	else if (u32_dbg_mode == 30)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me_freq_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 3);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me_freq_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 3);
		}
	}

	
	// show freq_avgbv
	else if (u32_dbg_mode == 31)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s11_me_freq_avgbv_vx_rb[u32_i]) << u32_dbgshow_gain) >> 5);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s10_me_freq_avgbv_vy_rb[u32_i]) << u32_dbgshow_gain) >> 5);
		}
	}

	// show u10_me_bv_stats_rb
	else if (u32_dbg_mode == 32)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me_bv_stats_rb[u32_i]) << u32_dbgshow_gain) >> 3);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me_bv_stats_rb[u32_i]) << u32_dbgshow_gain) >> 3);
		}
	}

	// show u13_me_rPRD_rb
	else if (u32_dbg_mode == 33)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u13_me_rPRD_rb[u32_i]) << u32_dbgshow_gain) >> 6);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u13_me_rPRD_rb[u32_i]) << u32_dbgshow_gain) >> 6);
		}
	}

	// show u13_me_sobj_cnt_i_rb
	else if (u32_dbg_mode == 34)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u13_me_sobj_cnt_i_rb[u32_i]) << u32_dbgshow_gain) >> 6);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u13_me_sobj_cnt_i_rb[u32_i]) << u32_dbgshow_gain) >> 6);
		}
	}

	// show u13_me_sobj_cnt_p_rb
	else if (u32_dbg_mode == 35)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u13_me_sobj_cnt_p_rb[u32_i]) << u32_dbgshow_gain) >> 6);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u13_me_sobj_cnt_p_rb[u32_i]) << u32_dbgshow_gain) >> 6);
		}
	}

	// show u15_me_gmv_bv_diff_rb
	else if (u32_dbg_mode == 36)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u15_me_gmv_bv_diff_rb[u32_i]) << u32_dbgshow_gain) >> 8);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u15_me_gmv_bv_diff_rb[u32_i]) << u32_dbgshow_gain) >> 8);
		}
	}

	// show u25_me_rgn_sad_rb
	else if (u32_dbg_mode == 37)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u25_me_rgn_sad_rb[u32_i]) << u32_dbgshow_gain) >> 18);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u25_me_rgn_sad_rb[u32_i]) << u32_dbgshow_gain) >> 18);
		}
	}

	// show u20_me_rgn_dtl_rb
	else if (u32_dbg_mode == 38)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u20_me_rgn_dtl_rb[u32_i]) << u32_dbgshow_gain) >> 13);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u20_me_rgn_dtl_rb[u32_i]) << u32_dbgshow_gain) >> 13);
		}
	}

	// show u12_me_rgn_fblvl_rb
	else if (u32_dbg_mode == 39)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rgn_fblvl_rb[u32_i]) << u32_dbgshow_gain) >> 5);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rgn_fblvl_rb[u32_i]) << u32_dbgshow_gain) >> 5);
		}
	}

	// show u12_me_rgn_scss_rb
	else if (u32_dbg_mode == 40)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rgn_scss_rb[u32_i]) << u32_dbgshow_gain) >> 5);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rgn_scss_rb[u32_i]) << u32_dbgshow_gain) >> 5);
		}
	}

	// show u12_me_rgn_tcss_rb
	else if (u32_dbg_mode == 41)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rgn_tcss_rb[u32_i]) << u32_dbgshow_gain) >> 5);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_me_rgn_tcss_rb[u32_i]) << u32_dbgshow_gain) >> 5);
		}
	}

	// show u10_me15_invld_cnt_rb
	else if (u32_dbg_mode == 42)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me15_invld_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 3);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me15_invld_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 3);
		}
	}

	// show u10_me15_invld_rm_cnt_rb
	else if (u32_dbg_mode == 43)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me15_invld_rm_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 3);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u10_me15_invld_rm_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 3);
		}
	}

	// show me15_invld_mv
	else if (u32_dbg_mode == 44)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s11_me15_invld_mv_vx_rb[u32_i]) << u32_dbgshow_gain) >> 5);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.s10_me15_invld_mv_vy_rb[u32_i]) << u32_dbgshow_gain) >> 5);
		}
	}

	// show u12_dh_cov_cnt_rb
	else if (u32_dbg_mode == 45)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_dh_cov_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 5);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_dh_cov_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 5);
		}
	}

	// show u12_dh_ucov_cnt_rb
	else if (u32_dbg_mode == 46)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_dh_ucov_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 5);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u12_dh_ucov_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 5);
		}
	}

	// show u15_mc_local_lpf_cnt_rb
	else if (u32_dbg_mode == 47)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u15_mc_local_lpf_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 8);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_read_comreg.u15_mc_local_lpf_cnt_rb[u32_i]) << u32_dbgshow_gain) >> 8);
		}
	}

	// show long term logo
	else if (u32_dbg_mode == 48)
	{
		for (u32_i = 0; u32_i < 32; u32_i ++)
		{
			u32_dbgshow_u[u32_i] = (signed int)(((signed long long)(s_pContext->_output_logo_ctrl.long_term_logo_counter[u32_i]) << u32_dbgshow_gain) >> 3);
			u32_dbgshow_v[u32_i] = (signed int)(((signed long long)(s_pContext->_output_logo_ctrl.long_term_logo_counter[u32_i]) << u32_dbgshow_gain) >> 3);
		}
	}	
	
	// --------------------------------end of dbg mode select------------------------------
	// clip uv
	for (u32_i = 0; u32_i < 32; u32_i ++)
	{
		u32_dbgshow_u[u32_i] = _CLIP_(u32_dbgshow_u[u32_i], -128, 127) & 0xff;
		u32_dbgshow_v[u32_i] = _CLIP_(u32_dbgshow_v[u32_i], -128, 127) & 0xff;
	}

	// plot uv
	for (u32_i = 0; u32_i < 32; u32_i ++)
	{
		WriteRegister(M8P_RTHW_ME_DBGSHOW_RTHW_ME_DBGSHOW_1C_reg + 4 * (u32_i >> 2), (u32_i & 0x3) * 8, 7 + (u32_i & 0x3) * 8, u32_dbgshow_u[u32_i]);
		WriteRegister(M8P_RTHW_ME_DBGSHOW_RTHW_ME_DBGSHOW_3C_reg + 4 * (u32_i >> 2), (u32_i & 0x3) * 8, 7 + (u32_i & 0x3) * 8, u32_dbgshow_v[u32_i]);
	}
}

void WriteComReg_MISC_outputInterrupt_RTK2885pp(const _PARAM_WRT_COM_REG *pParam)
{

}

VOID MC_VAR_LPF_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned char var_lpf_sample_mode, var_lpf_dynamic_gain_en, var_lpf_dynamic_gain_min_value, var_lpf_gain;
	M8P_rtmc2_rtmc2_04_RBUS M8P_rtmc2_rtmc2_04_reg;
	M8P_rtmc2_rtmc2_2c_RBUS M8P_rtmc2_rtmc2_2c_reg;
	M8P_rtmc2_rtmc2_30_RBUS M8P_rtmc2_rtmc2_30_reg;
	M8P_rtmc2_rtmc2_34_RBUS M8P_rtmc2_rtmc2_34_reg;
	M8P_rtmc2_rtmc2_38_RBUS M8P_rtmc2_rtmc2_38_reg;
	M8P_rtmc2_rtmc2_3c_RBUS M8P_rtmc2_rtmc2_3c_reg;
	M8P_rtmc2_rtmc2_40_RBUS M8P_rtmc2_rtmc2_40_reg;
	M8P_rtmc2_rtmc2_44_RBUS M8P_rtmc2_rtmc2_44_reg;
	M8P_rtmc2_rtmc2_24_RBUS M8P_rtmc2_rtmc2_24_reg;
	unsigned int print_in_func;

	if (pParam->u1_mc_var_lpf_wrt_en == 0)
		return;
	
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	M8P_rtmc2_rtmc2_04_reg.regValue = rtd_inl(M8P_RTMC2_RTMC2_04_reg);
	M8P_rtmc2_rtmc2_24_reg.regValue = rtd_inl(M8P_RTMC2_RTMC2_2C_reg);
	M8P_rtmc2_rtmc2_24_reg.regValue = rtd_inl(M8P_RTMC2_RTMC2_30_reg);
	M8P_rtmc2_rtmc2_24_reg.regValue = rtd_inl(M8P_RTMC2_RTMC2_34_reg);
	M8P_rtmc2_rtmc2_24_reg.regValue = rtd_inl(M8P_RTMC2_RTMC2_38_reg);
	M8P_rtmc2_rtmc2_24_reg.regValue = rtd_inl(M8P_RTMC2_RTMC2_3C_reg);
	M8P_rtmc2_rtmc2_24_reg.regValue = rtd_inl(M8P_RTMC2_RTMC2_40_reg);
	M8P_rtmc2_rtmc2_24_reg.regValue = rtd_inl(M8P_RTMC2_RTMC2_44_reg);
	M8P_rtmc2_rtmc2_24_reg.regValue = rtd_inl(M8P_RTMC2_RTMC2_24_reg);
	var_lpf_sample_mode = s_pContext->_output_me_sceneAnalysis.u2_var_lpf_sample_mode;
	var_lpf_dynamic_gain_en = s_pContext->_output_me_sceneAnalysis.u1_var_lpf_dynamic_gain_en;
	var_lpf_dynamic_gain_min_value = s_pContext->_output_me_sceneAnalysis.u5_var_lpf_dynamic_gain_min_value;
	var_lpf_gain = s_pContext->_output_me_sceneAnalysis.u6_var_lpf_gain;
	if (var_lpf_sample_mode == 0)
	{
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef0  = 0;
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef1  =11;
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef2  = 9;
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef3  = 7;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef4  = 6;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef5  =10;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef6  = 9;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef7  = 6;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef8  = 5;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef9  = 4;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef10 = 9;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef11 = 5;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef12 = 4;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef13 = 3;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef14 = 2;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef15 = 0;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef16 = 0;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef17 = 0;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef18 = 0;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef19 = 0;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef20 = 0;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef21 = 0;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef22 = 0;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef23 = 0;
		M8P_rtmc2_rtmc2_44_reg.mc_varfir_coef24 = 0;
		M8P_rtmc2_rtmc2_44_reg.mc_varfir_coef25 = 0;
	}
	else if (var_lpf_sample_mode == 1)
	{
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef0  = 0;
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef1  = 5;
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef2  = 5;
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef3  = 5;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef4  = 4;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef5  = 4;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef6  = 4;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef7  = 3;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef8  = 3;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef9  = 6;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef10 = 5;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef11 = 5;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef12 = 4;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef13 = 4;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef14 = 3;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef15 = 3;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef16 = 5;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef17 = 4;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef18 = 4;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef19 = 4;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef20 = 3;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef21 = 3;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef22 = 0;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef23 = 0;
		M8P_rtmc2_rtmc2_44_reg.mc_varfir_coef24 = 0;
		M8P_rtmc2_rtmc2_44_reg.mc_varfir_coef25 = 0;
	}
	else if (var_lpf_sample_mode == 2)
	{
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef0  = 0;
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef1  = 6;
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef2  = 6;
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef3  = 5;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef4  = 5;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef5  = 5;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef6  = 4;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef7  = 4;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef8  = 4;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef9  = 4;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef10 = 4;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef11 = 3;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef12 = 3;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef13 = 3;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef14 = 3;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef15 = 3;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef16 = 3;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef17 = 6;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef18 = 5;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef19 = 4;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef20 = 4;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef21 = 3;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef22 = 5;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef23 = 4;
		M8P_rtmc2_rtmc2_44_reg.mc_varfir_coef24 = 3;
		M8P_rtmc2_rtmc2_44_reg.mc_varfir_coef25 = 3;
	}
	else
	{
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef0  = 0;
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef1  = 6;
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef2  = 5;
		M8P_rtmc2_rtmc2_2c_reg.mc_varfir_coef3  = 5;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef4  = 5;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef5  = 4;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef6  = 4;
		M8P_rtmc2_rtmc2_30_reg.mc_varfir_coef7  = 3;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef8  = 3;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef9  = 6;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef10 = 5;
		M8P_rtmc2_rtmc2_34_reg.mc_varfir_coef11 = 5;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef12 = 4;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef13 = 4;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef14 = 4;
		M8P_rtmc2_rtmc2_38_reg.mc_varfir_coef15 = 3;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef16 = 3;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef17 = 3;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef18 = 5;
		M8P_rtmc2_rtmc2_3c_reg.mc_varfir_coef19 = 4;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef20 = 3;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef21 = 3;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef22 = 0;
		M8P_rtmc2_rtmc2_40_reg.mc_varfir_coef23 = 0;
		M8P_rtmc2_rtmc2_44_reg.mc_varfir_coef24 = 0;
		M8P_rtmc2_rtmc2_44_reg.mc_varfir_coef25 = 0;
	}

	if (((M8P_rtmc2_rtmc2_24_reg.mc_var_lpf_sample_mode == 1) && (var_lpf_sample_mode == 3)) || \
		((M8P_rtmc2_rtmc2_24_reg.mc_var_lpf_sample_mode == 3) && (var_lpf_sample_mode == 1)))
	{
		M8P_rtmc2_rtmc2_24_reg.mc_var_lpf_sample_mode = 2;
		rtd_outl(M8P_RTMC2_RTMC2_24_reg, M8P_rtmc2_rtmc2_24_reg.regValue);
	}

	M8P_rtmc2_rtmc2_24_reg.mc_var_lpf_sample_mode = var_lpf_sample_mode;
	M8P_rtmc2_rtmc2_24_reg.mc_var_lpf_dynamic_gain_en = var_lpf_dynamic_gain_en;
	M8P_rtmc2_rtmc2_24_reg.mc_var_lpf_dynamic_gain_min_value = var_lpf_dynamic_gain_min_value;
	M8P_rtmc2_rtmc2_24_reg.mc_var_lpf_gain = var_lpf_gain;

	if ((_ABS_(s_pContext->_output_me_sceneAnalysis.s10_global_bgmvy) + _ABS_(s_pContext->_output_me_sceneAnalysis.s11_global_bgmvx)) < 32)
	{
		M8P_rtmc2_rtmc2_04_reg.mc_var_lpf_coef_occl_en = 0;
	}
   else
	{
		M8P_rtmc2_rtmc2_04_reg.mc_var_lpf_coef_occl_en = 1;
   }


	rtd_outl(M8P_RTMC2_RTMC2_04_reg, M8P_rtmc2_rtmc2_04_reg.regValue);
	rtd_outl(M8P_RTMC2_RTMC2_2C_reg, M8P_rtmc2_rtmc2_2c_reg.regValue);
	rtd_outl(M8P_RTMC2_RTMC2_30_reg, M8P_rtmc2_rtmc2_30_reg.regValue);
	rtd_outl(M8P_RTMC2_RTMC2_34_reg, M8P_rtmc2_rtmc2_34_reg.regValue);
	rtd_outl(M8P_RTMC2_RTMC2_38_reg, M8P_rtmc2_rtmc2_38_reg.regValue);
	rtd_outl(M8P_RTMC2_RTMC2_3C_reg, M8P_rtmc2_rtmc2_3c_reg.regValue);
	rtd_outl(M8P_RTMC2_RTMC2_40_reg, M8P_rtmc2_rtmc2_40_reg.regValue);
	rtd_outl(M8P_RTMC2_RTMC2_44_reg, M8P_rtmc2_rtmc2_44_reg.regValue);
	rtd_outl(M8P_RTMC2_RTMC2_24_reg, M8P_rtmc2_rtmc2_24_reg.regValue);
}

VOID Dehalo_OCCL_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned short global_fgmv, global_fgbg_mvdiff;
	unsigned char anchor_mvd, u8_i;
	M8P_rtme_dehalo1_rtme_dehalo1_08_RBUS M8P_rtme_dehalo1_rtme_dehalo1_08_reg;
	M8P_rtme_dehalo1_rtme_dehalo1_10_RBUS M8P_rtme_dehalo1_rtme_dehalo1_10_reg;
	M8P_rtme_dehalo0_rtme_dehalo0_18_RBUS M8P_rtme_dehalo0_rtme_dehalo0_18_reg;
	M8P_rtme_dehalo1_rtme_dehalo1_0c_RBUS M8P_rtme_dehalo1_rtme_dehalo1_0c_reg;
	M8P_rtme_dehalo0_rtme_dehalo0_30_RBUS M8P_rtme_dehalo0_rtme_dehalo0_30_reg;
	M8P_rtme_dehalo0_rtme_dehalo0_20_RBUS M8P_rtme_dehalo0_rtme_dehalo0_20_reg;
	M8P_rtme_dehalo0_rtme_dehalo0_34_RBUS M8P_rtme_dehalo0_rtme_dehalo0_34_reg;
	M8P_rtme_dehalo0_rtme_dehalo0_3c_RBUS M8P_rtme_dehalo0_rtme_dehalo0_3c_reg;
	M8P_rtme_dehalo0_rtme_dehalo0_28_RBUS M8P_rtme_dehalo0_rtme_dehalo0_28_reg;
	M8P_rtme_dehalo1_rtme_dehalo1_00_RBUS M8P_rtme_dehalo1_rtme_dehalo1_00_reg;
	M8P_rtme_dehalo0_rtme_dehalo0_e4_RBUS M8P_rtme_dehalo0_rtme_dehalo0_e4_reg;
	M8P_rtme_dehalo0_rtme_dehalo0_38_RBUS M8P_rtme_dehalo0_rtme_dehalo0_38_reg;
	M8P_rtme_dehalo0_rtme_dehalo0_2c_RBUS M8P_rtme_dehalo0_rtme_dehalo0_2c_reg;
	M8P_rtme_dehalo0_rtme_dehalo0_1c_RBUS M8P_rtme_dehalo0_rtme_dehalo0_1c_reg;
	unsigned int print_in_func;
#ifdef v20_algo_en
	int wrt_val;
#endif

	if (pParam->u1_dehalo_occl_wrt_en == 0)
		return;
	
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	M8P_rtme_dehalo1_rtme_dehalo1_08_reg.regValue = rtd_inl(M8P_RTME_DEHALO1_RTME_DEHALO1_08_reg);
	M8P_rtme_dehalo1_rtme_dehalo1_0c_reg.regValue = rtd_inl(M8P_RTME_DEHALO1_RTME_DEHALO1_0C_reg);
	M8P_rtme_dehalo1_rtme_dehalo1_10_reg.regValue = rtd_inl(M8P_RTME_DEHALO1_RTME_DEHALO1_10_reg);
	M8P_rtme_dehalo0_rtme_dehalo0_18_reg.regValue = rtd_inl(M8P_RTME_DEHALO1_RTME_DEHALO1_18_reg);
	M8P_rtme_dehalo0_rtme_dehalo0_30_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_30_reg);
	M8P_rtme_dehalo0_rtme_dehalo0_20_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_20_reg);
	M8P_rtme_dehalo0_rtme_dehalo0_34_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_34_reg);
	M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_3C_reg);
	M8P_rtme_dehalo0_rtme_dehalo0_28_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_28_reg);
	M8P_rtme_dehalo1_rtme_dehalo1_00_reg.regValue = rtd_inl(M8P_RTME_DEHALO1_RTME_DEHALO1_00_reg);
	M8P_rtme_dehalo0_rtme_dehalo0_e4_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_E4_reg);
	M8P_rtme_dehalo0_rtme_dehalo0_38_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_38_reg);
	M8P_rtme_dehalo0_rtme_dehalo0_2c_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_2C_reg);
	M8P_rtme_dehalo0_rtme_dehalo0_1c_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_1C_reg);
	
	global_fgmv = _ABS_(s_pContext->_output_me_sceneAnalysis.s10_global_fgmvy) + _ABS_(s_pContext->_output_me_sceneAnalysis.s11_global_fgmvx);
	if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff > 80)
	{
		M8P_rtme_dehalo1_rtme_dehalo1_08_reg.dh_occl_refine_by_i_p_adpt_mvdiff_en = 1;
		M8P_rtme_dehalo1_rtme_dehalo1_08_reg.dh_occl_refine_by_i_p_adpt_mvdiff_gain= 1;
	}
	else
	{
		M8P_rtme_dehalo1_rtme_dehalo1_08_reg.dh_occl_refine_by_i_p_adpt_mvdiff_en = 0;
		M8P_rtme_dehalo1_rtme_dehalo1_08_reg.dh_occl_refine_by_i_p_adpt_mvdiff_gain= 0;
	}

	if ((global_fgmv > 48) || \
		(s_pContext->_output_me_sceneAnalysis.u6_temporal_adpt_cnt > 0) || \
		(M8P_rtme_dehalo1_rtme_dehalo1_0c_reg.dh_apl_upsample_en > 0))
	{
		M8P_rtme_dehalo0_rtme_dehalo0_30_reg.dh_occl_refine_by_i_p_mvdiff_bv0_diff_gain = 0;
		M8P_rtme_dehalo1_rtme_dehalo1_08_reg.dh_occl_refine_by_i_p_adpt_mvdiff_en = 0;
		M8P_rtme_dehalo1_rtme_dehalo1_08_reg.dh_occl_refine_by_i_p_adpt_mvdiff_gain= 0;
	}
	else
	{
		M8P_rtme_dehalo0_rtme_dehalo0_30_reg.dh_occl_refine_by_i_p_mvdiff_bv0_diff_gain = 1;
	}

	if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 48)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_20_reg.dh_occl_refine_mvdiff_thr_max_value = 6;
		M8P_rtme_dehalo0_rtme_dehalo0_20_reg.dh_rmvdiff_thr_max = 64;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 96)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_20_reg.dh_occl_refine_mvdiff_thr_max_value = 7;
		M8P_rtme_dehalo0_rtme_dehalo0_20_reg.dh_rmvdiff_thr_max = 128;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 128)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_20_reg.dh_occl_refine_mvdiff_thr_max_value = 8;
		M8P_rtme_dehalo0_rtme_dehalo0_20_reg.dh_rmvdiff_thr_max = 192;
	}
	else
	{
		M8P_rtme_dehalo0_rtme_dehalo0_20_reg.dh_occl_refine_mvdiff_thr_max_value = 8;
		M8P_rtme_dehalo0_rtme_dehalo0_20_reg.dh_rmvdiff_thr_max = 256;
	}

	if (s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff < 32)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_34_reg.dh_occl_bilateral_lpf_mvdiff_gain= 8;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff < 48)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_34_reg.dh_occl_bilateral_lpf_mvdiff_gain = 6;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff < 64)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_34_reg.dh_occl_bilateral_lpf_mvdiff_gain = 4;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff < 96)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_34_reg.dh_occl_bilateral_lpf_mvdiff_gain = 3;
	}
	else
	{
		M8P_rtme_dehalo0_rtme_dehalo0_34_reg.dh_occl_bilateral_lpf_mvdiff_gain = 2;
	}


	// occl post proc
	if ((global_fgmv < 32) || (s_pContext->_output_me_sceneAnalysis.u6_mean_unstable_score< 32))
	{
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_en = 1;
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_mode= 2;
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_pk_mode= 2;
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_ori_pk_mode= 0;
		M8P_rtme_dehalo0_rtme_dehalo0_30_reg.dh_occl_refine_cov_coring = 4;
		M8P_rtme_dehalo0_rtme_dehalo0_30_reg.dh_occl_refine_ucov_coring = 4;
	}
	else
	{
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_en = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_30_reg.dh_occl_refine_cov_coring = 20;
		M8P_rtme_dehalo0_rtme_dehalo0_30_reg.dh_occl_refine_ucov_coring = 20;
	}

	if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff >= 144)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_en = 1;
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_mode= 0;
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_pk_mode= 0;
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_ori_pk_mode= 2;
		M8P_rtme_dehalo0_rtme_dehalo0_30_reg.dh_occl_refine_cov_coring = 4;
		M8P_rtme_dehalo0_rtme_dehalo0_30_reg.dh_occl_refine_ucov_coring = 4;
	}

	if ((s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 48) && (s_pContext->_output_me_sceneAnalysis.u10_t_gmv_mvdiff < 10))
	{
		M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_dia_invalid_remove_en = 0;
	}
	else
	{
		M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_dia_invalid_remove_en = 1;
	}

	if ((s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff > 64) && (s_pContext->_output_me_sceneAnalysis.u6_unstable_score_cnt <=4))
	{
		M8P_rtme_dehalo1_rtme_dehalo1_10_reg.dh_select_replace_grp_mv_mode = 0;
	}
	else
	{
		M8P_rtme_dehalo1_rtme_dehalo1_10_reg.dh_select_replace_grp_mv_mode = 1;
	}


	
	if ((s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 48) && (s_pContext->_output_me_sceneAnalysis.u10_t_gmv_mvdiff < 10))
	{
		M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_dia_invalid_remove_en = 0;
	}
	else
	{
		M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_dia_invalid_remove_en = 1;
	}

	if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 16)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_dia_mode = 3;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = 2;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_me1_mvdiff_coring_gain = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_bv0_mvdiff_coring_gain = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_i_p_mvdiff_coring_gain = 0;
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_by_bgmv_mvdiff_coring_gain = 6;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 32)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_dia_mode = 3;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = 2;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_me1_mvdiff_coring_gain = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_bv0_mvdiff_coring_gain = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_i_p_mvdiff_coring_gain = ((M8P_rtme_dehalo1_rtme_dehalo1_0c_reg.dh_apl_upsample_en > 0) || s_pContext->_output_me_sceneAnalysis.u1_freq_scene) ? 4 : 1;
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_by_bgmv_mvdiff_coring_gain = 6;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 48)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_dia_mode = 2;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = 2;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_me1_mvdiff_coring_gain = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_bv0_mvdiff_coring_gain = 2;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_i_p_mvdiff_coring_gain = ((M8P_rtme_dehalo1_rtme_dehalo1_0c_reg.dh_apl_upsample_en > 0) || s_pContext->_output_me_sceneAnalysis.u1_freq_scene) ? 8 : 2;
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_by_bgmv_mvdiff_coring_gain = 8;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 64)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_dia_mode = 1;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = 4;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_me1_mvdiff_coring_gain = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_bv0_mvdiff_coring_gain = 4;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_i_p_mvdiff_coring_gain = ((M8P_rtme_dehalo1_rtme_dehalo1_0c_reg.dh_apl_upsample_en > 0) || s_pContext->_output_me_sceneAnalysis.u1_freq_scene) ? 10 : 4;
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_by_bgmv_mvdiff_coring_gain = 12;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 96)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_dia_mode = 1;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = 8;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_me1_mvdiff_coring_gain = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_bv0_mvdiff_coring_gain = 8;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_i_p_mvdiff_coring_gain = ((M8P_rtme_dehalo1_rtme_dehalo1_0c_reg.dh_apl_upsample_en > 0) || s_pContext->_output_me_sceneAnalysis.u1_freq_scene) ? 12 : 8;
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_by_bgmv_mvdiff_coring_gain = 14;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 128)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_dia_mode = 1;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = 16;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_me1_mvdiff_coring_gain = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_bv0_mvdiff_coring_gain = 16;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_i_p_mvdiff_coring_gain = ((M8P_rtme_dehalo1_rtme_dehalo1_0c_reg.dh_apl_upsample_en > 0) || s_pContext->_output_me_sceneAnalysis.u1_freq_scene) ? 14 : 12;
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_by_bgmv_mvdiff_coring_gain = 16;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 256)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_dia_mode = 1;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = s_pContext->_output_me_sceneAnalysis.u10_mvdiff >> 3;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_me1_mvdiff_coring_gain = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_bv0_mvdiff_coring_gain = 16;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_i_p_mvdiff_coring_gain = 16;
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_by_bgmv_mvdiff_coring_gain = 16;
	}
	else
	{
		M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_dia_mode = 1;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = s_pContext->_output_me_sceneAnalysis.u10_mvdiff >> 3;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_me1_mvdiff_coring_gain = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_bv0_mvdiff_coring_gain = 32;
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_i_p_mvdiff_coring_gain = 32;
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_by_bgmv_mvdiff_coring_gain = 16;
	}

	if (s_pContext->_output_me_sceneAnalysis.u10_med_mvdiff < 64)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_e4_reg.dh_lfb_occl_gain = 16;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_med_mvdiff < 100)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_e4_reg.dh_lfb_occl_gain = 8;
	}
	else
	{
		M8P_rtme_dehalo0_rtme_dehalo0_e4_reg.dh_lfb_occl_gain = 4;
	}

	anchor_mvd = _MAX_(s_pContext->_output_me_sceneAnalysis.u10_med_mvdiff, s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff);
	if (anchor_mvd < 16)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = (s_pContext->_output_me_sceneAnalysis.u1_freq_scene == 0) ? 0 : 4;
	}
	else if (anchor_mvd < 32)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = (s_pContext->_output_me_sceneAnalysis.u1_freq_scene == 0) ? 0 : 4;
	}
	else if (anchor_mvd < 48)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = (s_pContext->_output_me_sceneAnalysis.u1_freq_scene == 0) ? 0 : 6;
	}
	else if (anchor_mvd < 64)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = (s_pContext->_output_me_sceneAnalysis.u1_freq_scene == 0) ? 4 : 8;
	}
	else if (anchor_mvd < 96)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = 8;
	}
	else if (anchor_mvd < 128)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = 16;
	}
	else
	{
		M8P_rtme_dehalo0_rtme_dehalo0_28_reg.dh_occl_refine_bgmv_mvdiff_coring_gain = anchor_mvd >> 3;
	}


	if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 64)
	{
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_dia_chaos_remove_en = 1;
	}
	else
	{
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_dia_chaos_remove_en = 0;
	}

	if (s_pContext->_output_me_sceneAnalysis.u6_global_unstb_cnt > 32)
	{
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_bgmv_rev_retimer_en = 1;
	}
	else
	{
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_bgmv_rev_retimer_en = 0;
	}

	if (s_pContext->_output_me_sceneAnalysis.u1_apl_upsample_en)
	{
		M8P_rtme_dehalo1_rtme_dehalo1_0c_reg.dh_apl_upsample_en=1;
		M8P_rtme_dehalo0_rtme_dehalo0_38_reg.dh_occl_select_mv_mode1=0;
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_en=1;
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_mode=0;
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_pk_mode=0;
		M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.dh_occl_post_proc_ori_pk_mode=2;
		M8P_rtme_dehalo0_rtme_dehalo0_30_reg.dh_occl_refine_cov_coring = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_30_reg.dh_occl_refine_ucov_coring = 0;
	}
	else
	{
		M8P_rtme_dehalo1_rtme_dehalo1_0c_reg.dh_apl_upsample_en=0;
		M8P_rtme_dehalo0_rtme_dehalo0_38_reg.dh_occl_select_mv_mode1=3;
	}

	if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 96)
	{
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_by_bgmv_mvdiff_mvd_gain=0;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 128)
	{
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_by_bgmv_mvdiff_mvd_gain=2;
	}
	else
	{
		M8P_rtme_dehalo1_rtme_dehalo1_00_reg.dh_occl_refine_by_bgmv_mvdiff_mvd_gain=8;
	}

	global_fgbg_mvdiff = _ABS_DIFF_(s_pContext->_output_me_sceneAnalysis.s10_global_fgmvy, s_pContext->_output_me_sceneAnalysis.s10_global_bgmvy) + \
						 _ABS_DIFF_(s_pContext->_output_me_sceneAnalysis.s11_global_fgmvx, s_pContext->_output_me_sceneAnalysis.s11_global_bgmvx);
	M8P_rtme_dehalo0_rtme_dehalo0_2c_reg.dh_occl_refine_by_bgmv_mvdiff_max_value = _CLIP_((global_fgbg_mvdiff * 3) >> 1, 32, 128);
	M8P_rtme_dehalo0_rtme_dehalo0_2c_reg.dh_occl_refine_by_bgmv_mvdiff_min_value = 32;//_CLIP_((global_fgbg_mvdiff >> 2, 32, 128);

	if ((s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 32) && \
		!s_pContext->_output_me_sceneAnalysis.u1_apl_upsample_en)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_1c_reg.dh_post_proc_mv_lpf_en = 0;
	}
	else
	{
		M8P_rtme_dehalo0_rtme_dehalo0_1c_reg.dh_post_proc_mv_lpf_en = 1;
	}

	M8P_rtme_dehalo0_rtme_dehalo0_30_reg.dh_occl_refine_by_i_p_mvdiff_gain = _CLIP_(16 - _CLIP_((s_pContext->_output_me_sceneAnalysis.u10_mvdiff - 64) >> 3, 0, 1024), 4, 16);

	M8P_rtme_dehalo0_rtme_dehalo0_18_reg.dh_occl_gain = _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb) > 150 ? 0 : 255;
	
	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		WriteRegister(M8P_RTME_DEHALO1_RTME_DEHALO1_34_reg+ 4 * (u8_i >> 2), 
			(u8_i & 0x3) * 8,
			7 + (u8_i & 0x3) * 8,
			_CLIP_((s_pContext->_output_me_sceneAnalysis.u10_rmv_mvdiff[u8_i] * 3) >> 1, 32, 128));
	}

	rtd_outl(M8P_RTME_DEHALO1_RTME_DEHALO1_08_reg, M8P_rtme_dehalo1_rtme_dehalo1_08_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO1_RTME_DEHALO1_0C_reg, M8P_rtme_dehalo1_rtme_dehalo1_0c_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO1_RTME_DEHALO1_10_reg, M8P_rtme_dehalo1_rtme_dehalo1_10_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO1_RTME_DEHALO1_18_reg, M8P_rtme_dehalo0_rtme_dehalo0_18_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_30_reg, M8P_rtme_dehalo0_rtme_dehalo0_30_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_20_reg, M8P_rtme_dehalo0_rtme_dehalo0_20_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_34_reg, M8P_rtme_dehalo0_rtme_dehalo0_34_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_3C_reg, M8P_rtme_dehalo0_rtme_dehalo0_3c_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_28_reg, M8P_rtme_dehalo0_rtme_dehalo0_28_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO1_RTME_DEHALO1_00_reg, M8P_rtme_dehalo1_rtme_dehalo1_00_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_E4_reg, M8P_rtme_dehalo0_rtme_dehalo0_e4_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_38_reg, M8P_rtme_dehalo0_rtme_dehalo0_38_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_2C_reg, M8P_rtme_dehalo0_rtme_dehalo0_2c_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_1C_reg, M8P_rtme_dehalo0_rtme_dehalo0_1c_reg.regValue);
#ifdef v20_algo_en
	for (u8_i=0; u8_i<32; u8_i++)
	{
		wrt_val = s_pContext->_output_me_sceneAnalysis.u8_final_rgn_unstable_score[u8_i];
		wrt_val = (_CLIP_(wrt_val >> 2, 0, 63) * _CLIP_(s_pContext->_output_me_sceneAnalysis.u16_avg_gmv - 32, 0, 64)) >> 6;
		if (s_pContext->_output_me_sceneAnalysis.u1_large_rgn_unstb[u8_i])
			wrt_val = wrt_val >> 1;

		if (u8_i < 6)
		{
			WriteRegister(M8P_RTME_DEHALO1_RTME_DEHALO1_1C_reg,
				u8_i * 5,
				u8_i * 5 + 4,
				wrt_val);
		}
		else if (u8_i < 12)
		{
			WriteRegister(M8P_RTME_DEHALO1_RTME_DEHALO1_20_reg,
				(u8_i - 6) * 5,
				(u8_i - 6) * 5 + 4,
				wrt_val);
		}
		else if (u8_i < 18)
		{
			WriteRegister(M8P_RTME_DEHALO1_RTME_DEHALO1_24_reg,
				(u8_i - 12) * 5,
				(u8_i - 12) * 5 + 4,
				wrt_val);
		}
		else if (u8_i < 24)
		{
			WriteRegister(M8P_RTME_DEHALO1_RTME_DEHALO1_28_reg,
				(u8_i - 18) * 5,
				(u8_i - 18) * 5 + 4,
				wrt_val);
		}
		else if (u8_i < 20)
		{
			WriteRegister(M8P_RTME_DEHALO1_RTME_DEHALO1_2C_reg,
				(u8_i - 24) * 5,
				(u8_i - 24) * 5 + 4,
				wrt_val);
		}
		else
		{
			WriteRegister(M8P_RTME_DEHALO1_RTME_DEHALO1_30_reg,
				(u8_i - 30) * 5,
				(u8_i - 30) * 5 + 4,
				wrt_val);
		}
	}
#endif
}


VOID Dehalo_Occl_Mvpred_wetAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	int u11_gmv_mvx = 0;//regr mvx
	int u10_gmv_mvy = 0;//regr mvy
	M8P_rtme_dehalo0_rtme_dehalo0_f4_RBUS M8P_rtme_dehalo0_rtme_dehalo0_f4_reg;
	M8P_rtme_dehalo0_rtme_dehalo0_ec_RBUS M8P_rtme_dehalo0_rtme_dehalo0_ec_reg;
	unsigned int print_in_func;  

	if (pParam->u1_dh_occl_mvpred_wrt_en == 0)
		return;
	
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}

	M8P_rtme_dehalo0_rtme_dehalo0_f4_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_F4_reg);
	M8P_rtme_dehalo0_rtme_dehalo0_ec_reg.regValue = rtd_inl(M8P_RTME_DEHALO0_RTME_DEHALO0_EC_reg);

	u11_gmv_mvx = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	u10_gmv_mvy = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;

	if((_ABS_(u11_gmv_mvx)+_ABS_(u10_gmv_mvy))<15)
	{
		M8P_rtme_dehalo0_rtme_dehalo0_f4_reg.dh_occl_mvpred_en = 0;
		M8P_rtme_dehalo0_rtme_dehalo0_ec_reg.dh_fetchmv_mv_refine_en = 0;
	}
	else
	{
		M8P_rtme_dehalo0_rtme_dehalo0_f4_reg.dh_occl_mvpred_en = 1;
		M8P_rtme_dehalo0_rtme_dehalo0_ec_reg.dh_fetchmv_mv_refine_en = 1;
	}

	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_F4_reg, M8P_rtme_dehalo0_rtme_dehalo0_f4_reg.regValue);
	rtd_outl(M8P_RTME_DEHALO0_RTME_DEHALO0_EC_reg, M8P_rtme_dehalo0_rtme_dehalo0_ec_reg.regValue); 

}

VOID ME15_Invalid_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned short gmv_grp0;
	unsigned char u8_i;
	M8P_rtme_me15_0_rtme_me15_2c_RBUS M8P_rtme_me15_0_rtme_me15_2c_reg;
	M8P_rtme_me15_0_rtme_me15_1c_RBUS M8P_rtme_me15_0_rtme_me15_1c_reg;
	M8P_rtme_me15_0_rtme_me15_24_RBUS M8P_rtme_me15_0_rtme_me15_24_reg;
	M8P_rtme_me15_0_rtme_me15_18_RBUS M8P_rtme_me15_0_rtme_me15_18_reg;
	M8P_rtme_me15_0_rtme_me15_20_RBUS M8P_rtme_me15_0_rtme_me15_20_reg;
	M8P_rtme_me15_0_rtme_me15_14_RBUS M8P_rtme_me15_0_rtme_me15_14_reg;
	unsigned int print_in_func;
#ifdef v20_algo_en
	unsigned char gain;
	int wrt_val;
#endif

	if (pParam->u1_me15_invalid_wrt_en == 0)
		return;
	
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	
	M8P_rtme_me15_0_rtme_me15_2c_reg.regValue = rtd_inl(M8P_RTME_ME15_0_RTME_ME15_2C_reg - u32_fpga_offset);
	M8P_rtme_me15_0_rtme_me15_1c_reg.regValue = rtd_inl(M8P_RTME_ME15_0_RTME_ME15_1C_reg - u32_fpga_offset);
	M8P_rtme_me15_0_rtme_me15_24_reg.regValue = rtd_inl(M8P_RTME_ME15_0_RTME_ME15_24_reg - u32_fpga_offset);
	M8P_rtme_me15_0_rtme_me15_18_reg.regValue = rtd_inl(M8P_RTME_ME15_0_RTME_ME15_18_reg - u32_fpga_offset);
	M8P_rtme_me15_0_rtme_me15_20_reg.regValue = rtd_inl(M8P_RTME_ME15_0_RTME_ME15_20_reg - u32_fpga_offset);
	M8P_rtme_me15_0_rtme_me15_14_reg.regValue = rtd_inl(M8P_RTME_ME15_0_RTME_ME15_14_reg - u32_fpga_offset);

	gmv_grp0 = _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb) + _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb);
	
	if (s_pContext->_output_me_sceneAnalysis.u6_temporal_adpt_cnt > 0)
	{
		M8P_rtme_me15_0_rtme_me15_2c_reg.me15_occl_mv_invalid_cnt_sad_diff_thr = _CLIP_((s_pContext->_output_me_sceneAnalysis.u10_mvdiff - 64) >> 2, 16, 32);
	}
	else
	{
		M8P_rtme_me15_0_rtme_me15_2c_reg.me15_occl_mv_invalid_cnt_sad_diff_thr = _CLIP_((s_pContext->_output_me_sceneAnalysis.u10_mvdiff - 64) >> 2, 8, 16);
	}

	M8P_rtme_me15_0_rtme_me15_1c_reg.me15_invalid_dyn_cnt_thr_mvdiff_gain = _MAX_(64 - (s_pContext->_output_me_sceneAnalysis.u10_mvdiff >> 1), 0);

	M8P_rtme_me15_0_rtme_me15_24_reg.me15_rmvdiff_coring= _CLIP_(((short)s_pContext->_output_me_sceneAnalysis.u10_mvdiff - 64) >> 2, 8, 64);

	if (s_pContext->_output_me_sceneAnalysis.u10_med_mvdiff < 32)
	{
		M8P_rtme_me15_0_rtme_me15_18_reg.me15_invalid_dyn_cnt_thr_en = 0;
#ifdef v20_algo_en
		M8P_rtme_me15_0_rtme_me15_18_reg.me15_invalid_cnt_thr= ((gmv_grp0 < 4) || s_pContext->_output_me_sceneAnalysis.u1_freq_scene)? 5 : 4;
#else
		M8P_rtme_me15_0_rtme_me15_18_reg.me15_invalid_cnt_thr= 4;
#endif
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_med_mvdiff < 48)
	{
		M8P_rtme_me15_0_rtme_me15_18_reg.me15_invalid_dyn_cnt_thr_en = 0;
		M8P_rtme_me15_0_rtme_me15_18_reg.me15_invalid_cnt_thr= 4;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_med_mvdiff < 64)
	{
		M8P_rtme_me15_0_rtme_me15_18_reg.me15_invalid_dyn_cnt_thr_en = 1;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_max_min_sad_diff_thr = 8;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_sad_diff_thr= 16;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_dtl_thr = s_pContext->_output_me_sceneAnalysis.u1_freq_scene ? 256 : 128;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_rmvdiff_gain = 4;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_med_mvdiff < 96)
	{
		M8P_rtme_me15_0_rtme_me15_18_reg.me15_invalid_dyn_cnt_thr_en = 1;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_max_min_sad_diff_thr = 32;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_sad_diff_thr= s_pContext->_output_me_sceneAnalysis.u1_freq_scene ? 16 : 64;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_dtl_thr = s_pContext->_output_me_sceneAnalysis.u1_freq_scene ? 256 : 48;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_rmvdiff_gain = 6;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_med_mvdiff < 128)
	{
		M8P_rtme_me15_0_rtme_me15_18_reg.me15_invalid_dyn_cnt_thr_en = 1;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_max_min_sad_diff_thr = 48;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_sad_diff_thr= s_pContext->_output_me_sceneAnalysis.u1_freq_scene ? 16 : 96;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_dtl_thr = s_pContext->_output_me_sceneAnalysis.u1_freq_scene ? 256 : 32;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_rmvdiff_gain = 8;
	}
	else
	{
		M8P_rtme_me15_0_rtme_me15_18_reg.me15_invalid_dyn_cnt_thr_en = 1;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_max_min_sad_diff_thr = 64;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_sad_diff_thr= s_pContext->_output_me_sceneAnalysis.u1_freq_scene ? 16 : 128;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_dtl_thr = s_pContext->_output_me_sceneAnalysis.u1_freq_scene ? 256 : 16;
		M8P_rtme_me15_0_rtme_me15_20_reg.me15_invalid_dyn_cnt_thr_rmvdiff_gain = 8;
	}

	if ((s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff > 256) || (s_pContext->_output_me_sceneAnalysis.u6_global_unstb_cnt > 32))
	{
		M8P_rtme_me15_0_rtme_me15_18_reg.me15_invalid_dyn_cnt_thr_en = 0;
		M8P_rtme_me15_0_rtme_me15_18_reg.me15_invalid_cnt_thr= 1;
	}


	if (s_pContext->_output_me_sceneAnalysis.u10_med_mvdiff < 32)
	{
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_mvdiff_thr = _MAX_(s_pContext->_output_me_sceneAnalysis.u10_med_mvdiff >> 1, 4);
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_dtl_coring_gain = 0;
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_me1_sad_max_value = 0;
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_sad_diff_gain = 1;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff < 64)
	{
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_mvdiff_thr = 0;
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_dtl_coring_gain = 0;
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_me1_sad_max_value = 0;
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_sad_diff_gain = 1;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff < 128)
	{
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_mvdiff_thr = _MAX_(s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff >> 2, 4);;
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_dtl_coring_gain = 1;
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_me1_sad_max_value = 0;
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_sad_diff_gain = 32;
	}
	else
	{
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_mvdiff_thr = _MAX_(s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff >> 1, 4);;
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_dtl_coring_gain = 1;
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_me1_sad_max_value = 0;
		M8P_rtme_me15_0_rtme_me15_14_reg.me15_occl_mv_sad_diff_gain = 32;
	}

	M8P_rtme_me15_0_rtme_me15_2c_reg.me15_occl_mv_invalid_cnt_thr = _CLIP_((s_pContext->_output_me_sceneAnalysis.u10_mvdiff - 64) >> 2, 6, 32);


	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		WriteRegister(M8P_RTME_ME15_0_RTME_ME15_30_reg + 4 * u8_i,
			0,
			9,
			s_pContext->_output_me_sceneAnalysis.s10_rgn_bgmvy[u8_i] & 0x3ff);
		WriteRegister(M8P_RTME_ME15_0_RTME_ME15_30_reg + 4 * u8_i,
			10,
			20,
			s_pContext->_output_me_sceneAnalysis.s11_rgn_bgmvx[u8_i] & 0x7ff);
	}
#ifdef v20_algo_en
	for (u8_i=0; u8_i<32; u8_i++)
	{
		wrt_val = s_pContext->_output_me_sceneAnalysis.u8_final_rgn_unstable_score[u8_i];
		if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 128)
			gain = 2;
		else if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 150)
			gain = 1;
		else
			gain = 0;

		wrt_val = _CLIP_(wrt_val << gain, 0, 63);
		WriteRegister(M8P_RTME_ME15_0_RTME_ME15_30_reg+ 4 * u8_i,
			21,
			26,
			wrt_val);
	}
#endif
	rtd_outl(M8P_RTME_ME15_0_RTME_ME15_2C_reg, M8P_rtme_me15_0_rtme_me15_2c_reg.regValue);
	rtd_outl(M8P_RTME_ME15_0_RTME_ME15_1C_reg, M8P_rtme_me15_0_rtme_me15_1c_reg.regValue);
	rtd_outl(M8P_RTME_ME15_0_RTME_ME15_24_reg, M8P_rtme_me15_0_rtme_me15_24_reg.regValue);
	rtd_outl(M8P_RTME_ME15_0_RTME_ME15_18_reg, M8P_rtme_me15_0_rtme_me15_18_reg.regValue);
	rtd_outl(M8P_RTME_ME15_0_RTME_ME15_20_reg, M8P_rtme_me15_0_rtme_me15_20_reg.regValue);
	rtd_outl(M8P_RTME_ME15_0_RTME_ME15_14_reg, M8P_rtme_me15_0_rtme_me15_14_reg.regValue);
}


VOID HME1_Near_Freq_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	M8P_rtme_hme1_top1_rtme_hme1_top1_b8_RBUS M8P_rtme_hme1_top1_rtme_hme1_top1_b8_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_b8_RBUS M8P_rtme_hme1_top2_rtme_hme1_top2_b8_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_98_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_98_reg;
	unsigned int print_in_func;

	if (pParam->u1_hme1_near_freq_wrt_en == 0)
		return;
	
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	M8P_rtme_hme1_top1_rtme_hme1_top1_b8_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_B8_reg- u32_fpga_offset);
	M8P_rtme_hme1_top2_rtme_hme1_top2_b8_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_B8_reg- u32_fpga_offset);
	M8P_rtme_hme1_top3_rtme_hme1_top3_98_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_98_reg- u32_fpga_offset);

	if (s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff > 128)
	{
		M8P_rtme_hme1_top1_rtme_hme1_top1_b8_reg.me1_layer1_freq_w_sad_gain_pre_lyr_cand_en = 1;
		M8P_rtme_hme1_top2_rtme_hme1_top2_b8_reg.me1_layer2_freq_w_sad_gain_pre_lyr_cand_en = 1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_98_reg.me1_layer3_freq_w_sad_gain_pre_lyr_cand_en = 1;
	}
	else
	{
		M8P_rtme_hme1_top1_rtme_hme1_top1_b8_reg.me1_layer1_freq_w_sad_gain_pre_lyr_cand_en = 0;
		M8P_rtme_hme1_top2_rtme_hme1_top2_b8_reg.me1_layer2_freq_w_sad_gain_pre_lyr_cand_en = 0;
		M8P_rtme_hme1_top3_rtme_hme1_top3_98_reg.me1_layer3_freq_w_sad_gain_pre_lyr_cand_en = 0;
	}
	
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_B8_reg, M8P_rtme_hme1_top1_rtme_hme1_top1_b8_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_B8_reg, M8P_rtme_hme1_top2_rtme_hme1_top2_b8_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_98_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_98_reg.regValue);
	
}

VOID HME1_3DRS_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned short anchor_mvdiff;
	M8P_rtme_hme1_top0_rtme_hme1_top0_14_RBUS M8P_rtme_hme1_top0_rtme_hme1_top0_14_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top1_14_RBUS M8P_rtme_hme1_top1_rtme_hme1_top1_14_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_14_RBUS M8P_rtme_hme1_top2_rtme_hme1_top2_14_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_14_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_14_reg;
	M8P_rtme_hme1_top0_rtme_hme1_top0_04_RBUS M8P_rtme_hme1_top0_rtme_hme1_top0_04_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top1_04_RBUS M8P_rtme_hme1_top1_rtme_hme1_top1_04_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_04_RBUS M8P_rtme_hme1_top2_rtme_hme1_top2_04_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_04_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_04_reg;
	M8P_rtme_hme1_top0_rtme_hme1_top0_48_RBUS M8P_rtme_hme1_top0_rtme_hme1_top0_48_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top1_48_RBUS M8P_rtme_hme1_top1_rtme_hme1_top1_48_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_48_RBUS M8P_rtme_hme1_top2_rtme_hme1_top2_48_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_48_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_48_reg;

	M8P_rtme_hme1_top0_rtme_hme1_top0_00_RBUS M8P_rtme_hme1_top0_rtme_hme1_top0_00_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top1_00_RBUS M8P_rtme_hme1_top1_rtme_hme1_top1_00_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_00_RBUS M8P_rtme_hme1_top2_rtme_hme1_top2_00_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_00_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_00_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top1_cc_RBUS M8P_rtme_hme1_top1_rtme_hme1_top1_cc_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_cc_RBUS M8P_rtme_hme1_top2_rtme_hme1_top2_cc_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_ac_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_ac_reg;
	
	M8P_rtme_hme1_top4_rtme_hme1_top4_70_RBUS M8P_rtme_hme1_top4_rtme_hme1_top4_70_reg;
	unsigned int print_in_func;
#ifdef v20_algo_en
	unsigned char u8_i, tmp_value;
	unsigned short max_fb_cnt=0;
	M8P_rtme_hme1_top0_rtme_hme1_top0_c4_RBUS M8P_rtme_hme1_top0_rtme_hme1_top0_c4_reg;
	M8P_rtme_hme1_top0_rtme_hme1_top0_c0_RBUS M8P_rtme_hme1_top0_rtme_hme1_top0_c0_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top1_c4_RBUS M8P_rtme_hme1_top1_rtme_hme1_top1_c4_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top1_c0_RBUS M8P_rtme_hme1_top1_rtme_hme1_top1_c0_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_c4_RBUS M8P_rtme_hme1_top2_rtme_hme1_top2_c4_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_c0_RBUS M8P_rtme_hme1_top2_rtme_hme1_top2_c0_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_88_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_88_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_a0_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_a0_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_a4_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_a4_reg;

	M8P_rtme_hme1_top0_rtme_hme1_top0_a0_RBUS M8P_rtme_hme1_top0_rtme_hme1_top0_a0_reg;
	M8P_rtme_hme1_top0_rtme_hme1_top0_a4_RBUS M8P_rtme_hme1_top0_rtme_hme1_top0_a4_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top0_a0_RBUS M8P_rtme_hme1_top1_rtme_hme1_top0_a0_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top0_a4_RBUS M8P_rtme_hme1_top1_rtme_hme1_top0_a4_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top0_a0_RBUS M8P_rtme_hme1_top2_rtme_hme1_top0_a0_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top0_a4_RBUS M8P_rtme_hme1_top2_rtme_hme1_top0_a4_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_80_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_80_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_84_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_84_reg;
#endif
	
	if (pParam->u1_hme1_3drs_wrt_en == 0)
		return;

	
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	
	M8P_rtme_hme1_top0_rtme_hme1_top0_14_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_14_reg- u32_fpga_offset);
	M8P_rtme_hme1_top1_rtme_hme1_top1_14_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_14_reg- u32_fpga_offset);
	M8P_rtme_hme1_top2_rtme_hme1_top2_14_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_14_reg- u32_fpga_offset);
	M8P_rtme_hme1_top3_rtme_hme1_top3_14_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_14_reg- u32_fpga_offset);
	
	M8P_rtme_hme1_top0_rtme_hme1_top0_04_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_04_reg- u32_fpga_offset);
	M8P_rtme_hme1_top1_rtme_hme1_top1_04_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_04_reg- u32_fpga_offset);
	M8P_rtme_hme1_top2_rtme_hme1_top2_04_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_04_reg- u32_fpga_offset);
	M8P_rtme_hme1_top3_rtme_hme1_top3_04_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_04_reg- u32_fpga_offset);

	M8P_rtme_hme1_top0_rtme_hme1_top0_48_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_48_reg- u32_fpga_offset);
	M8P_rtme_hme1_top1_rtme_hme1_top1_48_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_48_reg- u32_fpga_offset);
	M8P_rtme_hme1_top2_rtme_hme1_top2_48_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_48_reg- u32_fpga_offset);
	M8P_rtme_hme1_top3_rtme_hme1_top3_48_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_48_reg- u32_fpga_offset);

	M8P_rtme_hme1_top0_rtme_hme1_top0_00_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_00_reg- u32_fpga_offset);
	M8P_rtme_hme1_top1_rtme_hme1_top1_00_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_00_reg- u32_fpga_offset);
	M8P_rtme_hme1_top2_rtme_hme1_top2_00_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_00_reg- u32_fpga_offset);
	M8P_rtme_hme1_top3_rtme_hme1_top3_00_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_00_reg- u32_fpga_offset);

	
	M8P_rtme_hme1_top1_rtme_hme1_top1_cc_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_CC_reg- u32_fpga_offset);
	M8P_rtme_hme1_top2_rtme_hme1_top2_cc_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_CC_reg- u32_fpga_offset);
	M8P_rtme_hme1_top3_rtme_hme1_top3_ac_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_AC_reg- u32_fpga_offset);
	
	M8P_rtme_hme1_top4_rtme_hme1_top4_70_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP4_RTME_HME1_TOP4_70_reg- u32_fpga_offset);
#ifdef v20_algo_en
	M8P_rtme_hme1_top0_rtme_hme1_top0_c4_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_C4_reg- u32_fpga_offset);
	M8P_rtme_hme1_top0_rtme_hme1_top0_c0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_C0_reg- u32_fpga_offset);
	M8P_rtme_hme1_top1_rtme_hme1_top1_c4_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_C4_reg- u32_fpga_offset);
	M8P_rtme_hme1_top1_rtme_hme1_top1_c0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_C0_reg- u32_fpga_offset);
	M8P_rtme_hme1_top2_rtme_hme1_top2_c4_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_C4_reg- u32_fpga_offset);
	M8P_rtme_hme1_top2_rtme_hme1_top2_c0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_C0_reg- u32_fpga_offset);
	M8P_rtme_hme1_top3_rtme_hme1_top3_88_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_88_reg- u32_fpga_offset);
	M8P_rtme_hme1_top3_rtme_hme1_top3_a0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_A0_reg- u32_fpga_offset);
	M8P_rtme_hme1_top3_rtme_hme1_top3_a4_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_A4_reg- u32_fpga_offset);

	M8P_rtme_hme1_top0_rtme_hme1_top0_a0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_A0_reg- u32_fpga_offset);
	M8P_rtme_hme1_top0_rtme_hme1_top0_a4_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_A4_reg- u32_fpga_offset);
	M8P_rtme_hme1_top1_rtme_hme1_top0_a0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP0_A0_reg- u32_fpga_offset);
	M8P_rtme_hme1_top1_rtme_hme1_top0_a4_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP0_A4_reg- u32_fpga_offset);
	M8P_rtme_hme1_top2_rtme_hme1_top0_a0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP0_A0_reg- u32_fpga_offset);
	M8P_rtme_hme1_top2_rtme_hme1_top0_a4_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP0_A4_reg- u32_fpga_offset);
	M8P_rtme_hme1_top3_rtme_hme1_top3_80_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_80_reg- u32_fpga_offset);
	M8P_rtme_hme1_top3_rtme_hme1_top3_84_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_84_reg- u32_fpga_offset);
#endif

	if (s_pContext->_output_me_sceneAnalysis.u16_gmv_max_mv < 32)
	{
		M8P_rtme_hme1_top0_rtme_hme1_top0_14_reg.me1_layer0_zmv_en= 0;
		M8P_rtme_hme1_top1_rtme_hme1_top1_14_reg.me1_layer1_zmv_en= 0;
		M8P_rtme_hme1_top2_rtme_hme1_top2_14_reg.me1_layer2_zmv_en= 0;
		M8P_rtme_hme1_top3_rtme_hme1_top3_14_reg.me1_layer3_zmv_en= 0;

		M8P_rtme_hme1_top2_rtme_hme1_top2_48_reg.me1_layer2_med9flt_logo_en= 0;
		M8P_rtme_hme1_top3_rtme_hme1_top3_48_reg.me1_layer3_med9flt_logo_en= 0;

		M8P_rtme_hme1_top3_rtme_hme1_top3_04_reg.me1_layer3_sad_logo_en = 0;

		M8P_rtme_hme1_top1_rtme_hme1_top1_00_reg.me1_layer1_rand_max_value= 3;
		M8P_rtme_hme1_top2_rtme_hme1_top2_00_reg.me1_layer2_rand_max_value= 2;
		M8P_rtme_hme1_top3_rtme_hme1_top3_00_reg.me1_layer3_rand_max_value= 1;
	}
	else if (s_pContext->_output_me_sceneAnalysis.u16_gmv_max_mv < 64)
	{
		M8P_rtme_hme1_top0_rtme_hme1_top0_14_reg.me1_layer0_zmv_en= 1;
		M8P_rtme_hme1_top1_rtme_hme1_top1_14_reg.me1_layer1_zmv_en= 1;
		M8P_rtme_hme1_top2_rtme_hme1_top2_14_reg.me1_layer2_zmv_en= 1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_14_reg.me1_layer3_zmv_en= 1;

		
		M8P_rtme_hme1_top2_rtme_hme1_top2_48_reg.me1_layer2_med9flt_logo_en= 0;
		M8P_rtme_hme1_top3_rtme_hme1_top3_48_reg.me1_layer3_med9flt_logo_en= 0;
		
		M8P_rtme_hme1_top3_rtme_hme1_top3_04_reg.me1_layer3_sad_logo_en = 0;

		M8P_rtme_hme1_top1_rtme_hme1_top1_00_reg.me1_layer1_rand_max_value= 4;
		M8P_rtme_hme1_top2_rtme_hme1_top2_00_reg.me1_layer2_rand_max_value= 3;
		M8P_rtme_hme1_top3_rtme_hme1_top3_00_reg.me1_layer3_rand_max_value= 2;
	}
	else
	{
		M8P_rtme_hme1_top0_rtme_hme1_top0_14_reg.me1_layer0_zmv_en= 1;
		M8P_rtme_hme1_top1_rtme_hme1_top1_14_reg.me1_layer1_zmv_en= 1;
		M8P_rtme_hme1_top2_rtme_hme1_top2_14_reg.me1_layer2_zmv_en= 1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_14_reg.me1_layer3_zmv_en= 1;

		M8P_rtme_hme1_top2_rtme_hme1_top2_48_reg.me1_layer2_med9flt_logo_en= 1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_48_reg.me1_layer3_med9flt_logo_en= 1;


		M8P_rtme_hme1_top3_rtme_hme1_top3_04_reg.me1_layer3_sad_logo_en = 1;

		M8P_rtme_hme1_top1_rtme_hme1_top1_00_reg.me1_layer1_rand_max_value= 5;
		M8P_rtme_hme1_top2_rtme_hme1_top2_00_reg.me1_layer2_rand_max_value= 4;
		M8P_rtme_hme1_top3_rtme_hme1_top3_00_reg.me1_layer3_rand_max_value= 3;
	}



	if (_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb) > 150)
	{
		M8P_rtme_hme1_top0_rtme_hme1_top0_14_reg.me1_layer0_zmv_en= 0;
		M8P_rtme_hme1_top1_rtme_hme1_top1_14_reg.me1_layer1_zmv_en= 0;
		M8P_rtme_hme1_top2_rtme_hme1_top2_14_reg.me1_layer2_zmv_en= 0;
		M8P_rtme_hme1_top3_rtme_hme1_top3_14_reg.me1_layer3_zmv_en= 0;
	}

	
	if (s_pContext->_output_me_sceneAnalysis.u1_freq_scene || (s_pContext->_output_me_sceneAnalysis.u5_small_unstb_cnt > 15))
	{
		M8P_rtme_hme1_top0_rtme_hme1_top0_48_reg.me1_layer0_med9flt_en= 1;
		M8P_rtme_hme1_top1_rtme_hme1_top1_48_reg.me1_layer1_med9flt_en= 1;
		M8P_rtme_hme1_top2_rtme_hme1_top2_48_reg.me1_layer2_med9flt_en= 1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_48_reg.me1_layer3_med9flt_en= 1;

		M8P_rtme_hme1_top0_rtme_hme1_top0_48_reg.me1_layer0_med9flt_dtl_thr = 255;
		M8P_rtme_hme1_top1_rtme_hme1_top1_48_reg.me1_layer1_med9flt_dtl_thr = 255;
		M8P_rtme_hme1_top2_rtme_hme1_top2_48_reg.me1_layer2_med9flt_dtl_thr = 255;
		M8P_rtme_hme1_top3_rtme_hme1_top3_48_reg.me1_layer3_med9flt_dtl_thr = 255;
	}
	else
	{
#ifdef v20_algo_en
		M8P_rtme_hme1_top0_rtme_hme1_top0_48_reg.me1_layer0_med9flt_en= 1;
		M8P_rtme_hme1_top1_rtme_hme1_top1_48_reg.me1_layer1_med9flt_en= 0;
		M8P_rtme_hme1_top2_rtme_hme1_top2_48_reg.me1_layer2_med9flt_en= 0;
		M8P_rtme_hme1_top3_rtme_hme1_top3_48_reg.me1_layer3_med9flt_en= 1;
#else
		M8P_rtme_hme1_top0_rtme_hme1_top0_48_reg.me1_layer0_med9flt_en= 1;
		M8P_rtme_hme1_top1_rtme_hme1_top1_48_reg.me1_layer1_med9flt_en= 1;
		M8P_rtme_hme1_top2_rtme_hme1_top2_48_reg.me1_layer2_med9flt_en= 1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_48_reg.me1_layer3_med9flt_en= 1;
#endif

		M8P_rtme_hme1_top0_rtme_hme1_top0_48_reg.me1_layer0_med9flt_dtl_thr = 32;
		M8P_rtme_hme1_top1_rtme_hme1_top1_48_reg.me1_layer1_med9flt_dtl_thr = 32;
		M8P_rtme_hme1_top2_rtme_hme1_top2_48_reg.me1_layer2_med9flt_dtl_thr = 32;
		M8P_rtme_hme1_top3_rtme_hme1_top3_48_reg.me1_layer3_med9flt_dtl_thr = 32;
	}

	// gmv rmv grp diff thr
	M8P_rtme_hme1_top4_rtme_hme1_top4_70_reg.me1_calc_statis_mv_rmv_mvd_y_max = _CLIP_(s_pContext->_output_me_sceneAnalysis.u10_mvdiff >> 2, 16, 64);
	M8P_rtme_hme1_top4_rtme_hme1_top4_70_reg.me1_calc_statis_mv_gmv_mvd_y_max = _CLIP_(s_pContext->_output_me_sceneAnalysis.u10_mvdiff >> 1, 32, 64);
	M8P_rtme_hme1_top4_rtme_hme1_top4_70_reg.me1_calc_statis_mv_rmv_mvd_y_min = _CLIP_(((short)s_pContext->_output_me_sceneAnalysis.u10_mvdiff - 64) >> 3, 8, 32);
	M8P_rtme_hme1_top4_rtme_hme1_top4_70_reg.me1_calc_statis_mv_gmv_mvd_y_min = _CLIP_(((short)s_pContext->_output_me_sceneAnalysis.u10_mvdiff - 64) >> 3, 8, 32);

	if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff > 100)
	{
		M8P_rtme_hme1_top1_rtme_hme1_top1_cc_reg.me1_layer1_adpt_pre_layer_cand_en=1;
		M8P_rtme_hme1_top2_rtme_hme1_top2_cc_reg.me1_layer2_adpt_pre_layer_cand_en=1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_ac_reg.me1_layer3_adpt_pre_layer_cand_en=1;
	}
	else
	{
		M8P_rtme_hme1_top1_rtme_hme1_top1_cc_reg.me1_layer1_adpt_pre_layer_cand_en=0;
		M8P_rtme_hme1_top2_rtme_hme1_top2_cc_reg.me1_layer2_adpt_pre_layer_cand_en=0;
		M8P_rtme_hme1_top3_rtme_hme1_top3_ac_reg.me1_layer3_adpt_pre_layer_cand_en=0;
	}

	anchor_mvdiff = _MAX_(s_pContext->_output_me_sceneAnalysis.u10_gmv_grp01_mvdiff, s_pContext->_output_me_sceneAnalysis.u10_med_mvdiff);
	M8P_rtme_hme1_top0_rtme_hme1_top0_00_reg.me1_layer0_rand_value= _CLIP_(anchor_mvdiff >> 4, 2, 32);
	M8P_rtme_hme1_top1_rtme_hme1_top1_00_reg.me1_layer1_rand_value= _CLIP_(anchor_mvdiff >> 4, 2, 16);
	M8P_rtme_hme1_top2_rtme_hme1_top2_00_reg.me1_layer2_rand_value= _CLIP_(anchor_mvdiff >> 4, 2, 8);
	M8P_rtme_hme1_top3_rtme_hme1_top3_00_reg.me1_layer3_rand_value= _CLIP_(anchor_mvdiff >> 4, 2, 8);

	// M8P_rtme_hme1_top0_rtme_hme1_top0_04_reg.me1_layer0_sad_5_obme_en = (s_pContext->_output_me_sceneAnalysis.u10_mvdiff > 150) ? 0 : 1;
	// TODO, ip_pi_mvdiff_pnt
	//M8P_rtme_hme1_top0_rtme_hme1_top0_04_reg.me1_layer0_sad_5_obme_en = (s_pContext->_output_me_sceneAnalysis.u10_mvdiff > 150) ? 0 : 1


	if (s_pContext->_output_me_sceneAnalysis.u1_fast_motion_cond || \
		(s_pContext->_output_me_sceneAnalysis.u6_temporal_adpt_cnt < 1) || \
		(s_pContext->_output_me_sceneAnalysis.u6_global_unstb_cnt > 32))
	{
		M8P_rtme_hme1_top1_rtme_hme1_top1_04_reg.me1_layer1_sad_ac_dc_mode = 1;
		M8P_rtme_hme1_top2_rtme_hme1_top2_04_reg.me1_layer2_sad_ac_dc_mode = 1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_04_reg.me1_layer3_sad_ac_dc_mode = 1;
	}
	else
	{
		M8P_rtme_hme1_top1_rtme_hme1_top1_04_reg.me1_layer1_sad_ac_dc_mode = 2;
		M8P_rtme_hme1_top2_rtme_hme1_top2_04_reg.me1_layer2_sad_ac_dc_mode = 2;
		M8P_rtme_hme1_top3_rtme_hme1_top3_04_reg.me1_layer3_sad_ac_dc_mode = 2;
	}
#ifdef v20_algo_en
	if (s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt > 32)
	{
		M8P_rtme_hme1_top0_rtme_hme1_top0_c4_reg.me1_layer0_mvd_adpt_en=1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_88_reg.me1_layer3_lfb_pre_gain_en=0;
	}
	else
	{
		M8P_rtme_hme1_top0_rtme_hme1_top0_c4_reg.me1_layer0_mvd_adpt_en=0;
		M8P_rtme_hme1_top3_rtme_hme1_top3_88_reg.me1_layer3_lfb_pre_gain_en=1;
	}
	M8P_rtme_hme1_top3_rtme_hme1_top3_88_reg.me1_layer3_lfb_pre_gain_coring = (1200 * (64 - s_pContext->_output_me_sceneAnalysis.u6_global_unstb_cnt) + 32) >> 6;
	for (u8_i=0; u8_i<32;u8_i++)
	{
		max_fb_cnt = _MAX_(max_fb_cnt, s_pContext->_output_read_comreg.u12_me_rgn_fblvl_rb[u8_i]);
	}
	
	
	if (((s_pContext->_output_read_comreg.u32_me_sc_pattern_rb & 0x7f) == 0) || (max_fb_cnt > 64))
	{
		if (s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt > 32)
		{
			M8P_rtme_hme1_top0_rtme_hme1_top0_c4_reg.me1_layer0_mvd_adpt_scan0_en=1;
			M8P_rtme_hme1_top1_rtme_hme1_top1_c4_reg.me1_layer1_mvd_adpt_scan0_en=1;
			M8P_rtme_hme1_top2_rtme_hme1_top2_c4_reg.me1_layer2_mvd_adpt_scan0_en=1;
			M8P_rtme_hme1_top3_rtme_hme1_top3_a4_reg.me1_layer3_mvd_adpt_scan0_en=1;
		}
		else
		{
			M8P_rtme_hme1_top0_rtme_hme1_top0_c4_reg.me1_layer0_mvd_adpt_scan0_en=0;
			M8P_rtme_hme1_top1_rtme_hme1_top1_c4_reg.me1_layer1_mvd_adpt_scan0_en=0;
			M8P_rtme_hme1_top2_rtme_hme1_top2_c4_reg.me1_layer2_mvd_adpt_scan0_en=0;
			M8P_rtme_hme1_top3_rtme_hme1_top3_a4_reg.me1_layer3_mvd_adpt_scan0_en=0;
		}
		M8P_rtme_hme1_top0_rtme_hme1_top0_c4_reg.me1_layer0_mvd_adpt_gain = (4 * (64 - s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt) + \
																		  8 * s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt + 32) >> 6;
		M8P_rtme_hme1_top1_rtme_hme1_top1_c4_reg.me1_layer1_mvd_adpt_gain = (4 * (64 - s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt) + \
																		 16 * s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt + 32) >> 6;
		M8P_rtme_hme1_top2_rtme_hme1_top2_c4_reg.me1_layer2_mvd_adpt_gain = (4 * (64 - s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt) + \
																		 32 * s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt + 32) >> 6;
		tmp_value = s_pContext->_output_me_sceneAnalysis.u1_apl_var_cond ? 32 : 64;
		M8P_rtme_hme1_top3_rtme_hme1_top3_a4_reg.me1_layer3_mvd_adpt_gain = (4 * (64 - s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt) + \
																		 tmp_value * s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt + 32) >> 6;


		M8P_rtme_hme1_top0_rtme_hme1_top0_c0_reg.me1_layer0_sad_ac_dc_min_apl_diff = (16 * (64 - s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt) + \
																				  16 * s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt + 32) >> 6;
		tmp_value = s_pContext->_output_me_sceneAnalysis.u1_apl_var_cond ? 32 : 16;
		M8P_rtme_hme1_top1_rtme_hme1_top1_c0_reg.me1_layer1_sad_ac_dc_min_apl_diff = (16 * (64 - s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt) + \
																				  tmp_value  * s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt + 32) >> 6;
		tmp_value = s_pContext->_output_me_sceneAnalysis.u1_apl_var_cond ? 48 : 16;
		M8P_rtme_hme1_top2_rtme_hme1_top2_c0_reg.me1_layer2_sad_ac_dc_min_apl_diff = (16 * (64 - s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt) + \
																				  tmp_value  * s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt + 32) >> 6;
		tmp_value = s_pContext->_output_me_sceneAnalysis.u1_apl_var_cond ? 64 : 16;
		M8P_rtme_hme1_top3_rtme_hme1_top3_a0_reg.me1_layer3_sad_ac_dc_min_apl_diff = (16 * (64 - s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt) + \
																				  tmp_value * s_pContext->_output_me_sceneAnalysis.u6_global_mvd_unstb_cnt + 32) >> 6;
		M8P_rtme_hme1_top0_rtme_hme1_top0_c4_reg.me1_layer0_mvd_adpt_mv_gain = _CLIP_(max_fb_cnt >> 6, 1, 4);
		M8P_rtme_hme1_top1_rtme_hme1_top1_c4_reg.me1_layer1_mvd_adpt_mv_gain = _CLIP_(max_fb_cnt >> 6, 1, 4);
		M8P_rtme_hme1_top2_rtme_hme1_top2_c4_reg.me1_layer2_mvd_adpt_mv_gain = _CLIP_(max_fb_cnt >> 6, 1, 4);
		M8P_rtme_hme1_top3_rtme_hme1_top3_a4_reg.me1_layer3_mvd_adpt_mv_gain = _CLIP_(max_fb_cnt >> 6, 1, 4);
	}
	else
	{
		M8P_rtme_hme1_top0_rtme_hme1_top0_c4_reg.me1_layer0_mvd_adpt_scan0_en=0;
		M8P_rtme_hme1_top1_rtme_hme1_top1_c4_reg.me1_layer1_mvd_adpt_scan0_en=0;
		M8P_rtme_hme1_top2_rtme_hme1_top2_c4_reg.me1_layer2_mvd_adpt_scan0_en=0;
		M8P_rtme_hme1_top3_rtme_hme1_top3_a4_reg.me1_layer3_mvd_adpt_scan0_en=0;
		 
		M8P_rtme_hme1_top0_rtme_hme1_top0_c4_reg.me1_layer0_mvd_adpt_gain = 4;
		M8P_rtme_hme1_top1_rtme_hme1_top1_c4_reg.me1_layer1_mvd_adpt_gain = 4;
		M8P_rtme_hme1_top2_rtme_hme1_top2_c4_reg.me1_layer2_mvd_adpt_gain = 4;
		M8P_rtme_hme1_top3_rtme_hme1_top3_a4_reg.me1_layer3_mvd_adpt_gain = 4;

		M8P_rtme_hme1_top0_rtme_hme1_top0_c0_reg.me1_layer0_sad_ac_dc_min_apl_diff = 16;
		M8P_rtme_hme1_top1_rtme_hme1_top1_c0_reg.me1_layer1_sad_ac_dc_min_apl_diff = 16;
		M8P_rtme_hme1_top2_rtme_hme1_top2_c0_reg.me1_layer2_sad_ac_dc_min_apl_diff = 16;
		M8P_rtme_hme1_top3_rtme_hme1_top3_a0_reg.me1_layer3_sad_ac_dc_min_apl_diff = 16;

		M8P_rtme_hme1_top0_rtme_hme1_top0_c4_reg.me1_layer0_mvd_adpt_mv_gain = 1;
		M8P_rtme_hme1_top1_rtme_hme1_top1_c4_reg.me1_layer1_mvd_adpt_mv_gain = 1;
		M8P_rtme_hme1_top2_rtme_hme1_top2_c4_reg.me1_layer2_mvd_adpt_mv_gain = 1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_a4_reg.me1_layer3_mvd_adpt_mv_gain = 1;
	}

	M8P_rtme_hme1_top0_rtme_hme1_top0_a0_reg.me1_layer0_lfb_gmv_gain_base64_th = s_pContext->_output_me_sceneAnalysis.u8_lfb_base64_th;
	M8P_rtme_hme1_top0_rtme_hme1_top0_a4_reg.me1_layer0_lfb_gmv_grp1_max_th = s_pContext->_output_me_sceneAnalysis.u9_lfb_gmv_grp1_max_th;
	M8P_rtme_hme1_top1_rtme_hme1_top0_a0_reg.me1_layer1_lfb_gmv_gain_base64_th = s_pContext->_output_me_sceneAnalysis.u8_lfb_base64_th;
	M8P_rtme_hme1_top1_rtme_hme1_top0_a4_reg.me1_layer1_lfb_gmv_grp1_max_th = s_pContext->_output_me_sceneAnalysis.u9_lfb_gmv_grp1_max_th;
	M8P_rtme_hme1_top2_rtme_hme1_top0_a0_reg.me1_layer2_lfb_gmv_gain_base64_th = s_pContext->_output_me_sceneAnalysis.u8_lfb_base64_th;
	M8P_rtme_hme1_top2_rtme_hme1_top0_a4_reg.me1_layer2_lfb_gmv_grp1_max_th = s_pContext->_output_me_sceneAnalysis.u9_lfb_gmv_grp1_max_th;
	M8P_rtme_hme1_top3_rtme_hme1_top3_80_reg.me1_layer3_lfb_gmv_gain_base64_th = s_pContext->_output_me_sceneAnalysis.u8_lfb_base64_th;
	M8P_rtme_hme1_top3_rtme_hme1_top3_84_reg.me1_layer3_lfb_gmv_grp1_max_th = s_pContext->_output_me_sceneAnalysis.u9_lfb_gmv_grp1_max_th;

	
#endif
	

	rtd_outl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_14_reg, M8P_rtme_hme1_top0_rtme_hme1_top0_14_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_14_reg, M8P_rtme_hme1_top1_rtme_hme1_top1_14_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_14_reg, M8P_rtme_hme1_top2_rtme_hme1_top2_14_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_14_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_14_reg.regValue);
	
	rtd_outl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_04_reg, M8P_rtme_hme1_top0_rtme_hme1_top0_04_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_04_reg, M8P_rtme_hme1_top1_rtme_hme1_top1_04_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_04_reg, M8P_rtme_hme1_top2_rtme_hme1_top2_04_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_04_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_04_reg.regValue);

	rtd_outl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_48_reg, M8P_rtme_hme1_top0_rtme_hme1_top0_48_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_48_reg, M8P_rtme_hme1_top1_rtme_hme1_top1_48_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_48_reg, M8P_rtme_hme1_top2_rtme_hme1_top2_48_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_48_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_48_reg.regValue);

	rtd_outl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_00_reg, M8P_rtme_hme1_top0_rtme_hme1_top0_00_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_00_reg, M8P_rtme_hme1_top1_rtme_hme1_top1_00_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_00_reg, M8P_rtme_hme1_top2_rtme_hme1_top2_00_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_00_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_00_reg.regValue);

	
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_CC_reg, M8P_rtme_hme1_top1_rtme_hme1_top1_cc_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_CC_reg, M8P_rtme_hme1_top2_rtme_hme1_top2_cc_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_AC_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_ac_reg.regValue);
	
	rtd_outl(M8P_RTME_HME1_TOP4_RTME_HME1_TOP4_70_reg, M8P_rtme_hme1_top4_rtme_hme1_top4_70_reg.regValue);

#ifdef v20_algo_en
	rtd_outl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_C4_reg, M8P_rtme_hme1_top0_rtme_hme1_top0_c4_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_C0_reg, M8P_rtme_hme1_top0_rtme_hme1_top0_c0_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_C4_reg, M8P_rtme_hme1_top1_rtme_hme1_top1_c4_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_C0_reg, M8P_rtme_hme1_top1_rtme_hme1_top1_c0_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_C4_reg, M8P_rtme_hme1_top2_rtme_hme1_top2_c4_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_C0_reg, M8P_rtme_hme1_top2_rtme_hme1_top2_c0_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_88_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_88_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_A0_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_a0_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_A4_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_a4_reg.regValue);

	rtd_outl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_A0_reg, M8P_rtme_hme1_top0_rtme_hme1_top0_a0_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_A4_reg, M8P_rtme_hme1_top0_rtme_hme1_top0_a4_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP0_A0_reg, M8P_rtme_hme1_top1_rtme_hme1_top0_a0_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP0_A4_reg, M8P_rtme_hme1_top1_rtme_hme1_top0_a4_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP0_A0_reg, M8P_rtme_hme1_top2_rtme_hme1_top0_a0_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP0_A4_reg, M8P_rtme_hme1_top2_rtme_hme1_top0_a4_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_80_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_80_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_84_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_84_reg.regValue);
	
#endif

}

VOID HME1_Mvmask_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	int gmv_mv_max = 0;
	int absgrp0mvxy=0,absgrp1mvxy=0,absgrp2mvxy=0;//regr 
	int  u11_gmv_mvx = 0;
	int  u10_gmv_mvy = 0; 

	M8P_rtme_hme1_top4_rtme_hme1_top4_94_RBUS  M8P_rtme_hme1_top4_rtme_hme1_top4_94_reg;
	M8P_rtme_hme1_top4_rtme_hme1_top4_98_RBUS  M8P_rtme_hme1_top4_rtme_hme1_top4_98_reg;
	unsigned int print_in_func;
	if (pParam->u1_hme1_mvmask_wrt_en == 0)
	return;  

	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}

	M8P_rtme_hme1_top4_rtme_hme1_top4_94_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP4_RTME_HME1_TOP4_94_reg);
	M8P_rtme_hme1_top4_rtme_hme1_top4_98_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP4_RTME_HME1_TOP4_98_reg);


	u11_gmv_mvx = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	u10_gmv_mvy = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
	absgrp0mvxy = _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb) + _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb);
	absgrp1mvxy = _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb) + _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb);
	absgrp2mvxy = _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb) + _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb);
	gmv_mv_max = MAX(absgrp0mvxy,absgrp1mvxy);
	gmv_mv_max = MAX(gmv_mv_max,absgrp2mvxy);

	
	if(gmv_mv_max>160)
	{
		M8P_rtme_hme1_top4_rtme_hme1_top4_98_reg.me1_mvmask_pre_same_th = 65;
		M8P_rtme_hme1_top4_rtme_hme1_top4_98_reg.me1_mvmask_pre_diff_th = 75;
		M8P_rtme_hme1_top4_rtme_hme1_top4_94_reg.me1_mvmask_lr_diff_th = 75;

	}
	else
	{
		M8P_rtme_hme1_top4_rtme_hme1_top4_98_reg.me1_mvmask_pre_same_th = 40;
		M8P_rtme_hme1_top4_rtme_hme1_top4_98_reg.me1_mvmask_pre_diff_th = 50;
		M8P_rtme_hme1_top4_rtme_hme1_top4_94_reg.me1_mvmask_lr_diff_th = 50;

	}
	
	rtd_outl(M8P_RTME_HME1_TOP4_RTME_HME1_TOP4_94_reg, M8P_rtme_hme1_top4_rtme_hme1_top4_94_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP4_RTME_HME1_TOP4_98_reg, M8P_rtme_hme1_top4_rtme_hme1_top4_98_reg.regValue);
}

VOID HME1_Freq_push_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{

	const _PQLCONTEXT *s_pContext = GetPQLContext();
	int  th_push = 0;
	bool max_cond = 0,min_cond = 0,layer1_vfrq_flag = 0,layer2_vfrq_flag = 0,layer3_vfrq_flag = 0,vfrq_flag = 0,gmv_cond = 0,close_push_frq = 0;
	int  u11_gmv_mvx = 0;
	//int  u10_gmv_mvy = 0;
	int rgn32sum_frq_flag_sum = 0,rgn32sum_frq_flag_smallsum = 0;//regr
	int rgnIdx = 0;
	int frq_flag[32]={0};
	int layer1_frq_h_cnt=0,layer2_frq_h_cnt=0,layer3_frq_h_cnt=0,layer1_frq_v_cnt=0,layer2_frq_v_cnt=0,layer3_frq_v_cnt=0;//regr
	unsigned int u27_ipme_h0Mot_rb = 0;
	unsigned int u27_ipme_h1Mot_rb = 0;
	unsigned int u27_ipme_v0Mot_rb = 0;
	unsigned int u27_ipme_v1Mot_rb = 0;
	M8P_rtme_hme1_top3_rtme_hme1_top3_60_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_60_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_98_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_98_reg; 
	M8P_rtme_hme1_top3_rtme_hme1_top3_9c_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_9c_reg; 
	M8P_rtme_iphme_top0_iphme_top_00_RBUS M8P_rtme_iphme_top0_iphme_top_00_rbus;
	M8P_rtme_iphme_top0_iphme_top_64_RBUS M8P_rtme_iphme_top0_iphme_top_64_rbus;
	M8P_rtme_iphme_top0_iphme_top_68_RBUS M8P_rtme_iphme_top0_iphme_top_68_rbus;
	unsigned int print_in_func;
	
	if (pParam->u1_hme1_freqpush_wrt_en == 0)
		return;

	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}

	
	M8P_rtme_hme1_top3_rtme_hme1_top3_60_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_60_reg);
	M8P_rtme_hme1_top3_rtme_hme1_top3_98_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_98_reg);
	M8P_rtme_hme1_top3_rtme_hme1_top3_9c_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_9C_reg);
	M8P_rtme_iphme_top0_iphme_top_00_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_00_reg);	
	M8P_rtme_iphme_top0_iphme_top_64_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_64_reg);
	M8P_rtme_iphme_top0_iphme_top_68_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_68_reg);

	for(rgnIdx = 0;rgnIdx<32;rgnIdx++)
	{
		frq_flag[rgnIdx] = s_pContext->_output_read_comreg.u10_me_freq_cnt_rb[rgnIdx];
		rgn32sum_frq_flag_sum = rgn32sum_frq_flag_sum + frq_flag[rgnIdx];//regr 32rgn frq_flag
		
		if(frq_flag[rgnIdx]<100)
			rgn32sum_frq_flag_smallsum++;
		
	}
	// lack of layer1_frq_h_cnt...
	
	 // th_push =   (pOutput->u27_ipme_h0Mot_rb - pOutput->u27_ipme_h1Mot_rb)*(pOutput->u27_ipme_v0Mot_rb - pOutput->u27_ipme_v1Mot_rb)*0.8/256;
	th_push =   (u27_ipme_h0Mot_rb - u27_ipme_h1Mot_rb)*(u27_ipme_v0Mot_rb - u27_ipme_v1Mot_rb)*0.8/256;
	max_cond = rgn32sum_frq_flag_sum > th_push;
	min_cond = rgn32sum_frq_flag_smallsum == 32 ;
	layer1_vfrq_flag = layer1_frq_h_cnt < (layer1_frq_v_cnt*0.05);
	layer2_vfrq_flag = layer2_frq_h_cnt < (layer2_frq_v_cnt*0.05);
	layer3_vfrq_flag = layer3_frq_h_cnt < (layer3_frq_v_cnt*0.05);
	

	vfrq_flag = (layer1_vfrq_flag || layer2_vfrq_flag || layer3_vfrq_flag) && (layer3_frq_v_cnt>300);

	gmv_cond = u11_gmv_mvx<10; //abs?

	close_push_frq = max_cond || min_cond || gmv_cond || (1-vfrq_flag);
	
	if(close_push_frq == 1 )
	{
		M8P_rtme_hme1_top3_rtme_hme1_top3_60_reg.me1_layer3_push_frq_en = 0;
		M8P_rtme_hme1_top3_rtme_hme1_top3_98_reg.me1_layer3_freq_pre_lyr_dia_en = 1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_9c_reg.me1_layer3_data_diff_pnt_freq_coef_en = 1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_98_reg.me1_layer3_freq_w_sad_gain_en = 1;
	}
	else 
	{
		M8P_rtme_hme1_top3_rtme_hme1_top3_60_reg.me1_layer3_push_frq_en = 1;
		M8P_rtme_hme1_top3_rtme_hme1_top3_98_reg.me1_layer3_freq_pre_lyr_dia_en = 0;
		M8P_rtme_hme1_top3_rtme_hme1_top3_9c_reg.me1_layer3_data_diff_pnt_freq_coef_en = 0;
		M8P_rtme_hme1_top3_rtme_hme1_top3_98_reg.me1_layer3_freq_w_sad_gain_en = 0;
	}


	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_60_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_60_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_98_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_98_reg.regValue);
	rtd_outl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_9C_reg, M8P_rtme_hme1_top3_rtme_hme1_top3_9c_reg.regValue);

}


VOID ME2_3DRS_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned char u3_div;
#ifdef V20_ALGO_EN
	unsigned char u8_i;
#endif
	short mvdiff;
	M8P_rtme_me2_calc0_rtme_me2_calc0_04_RBUS M8P_rtme_me2_calc0_rtme_me2_calc0_04_reg;
	M8P_rtme_me2_calc1_rtme_me2_calc1_08_RBUS M8P_rtme_me2_calc1_rtme_me2_calc1_08_reg;
	M8P_rtme_me2_calc1_rtme_me2_calc1_14_RBUS M8P_rtme_me2_calc1_rtme_me2_calc1_14_reg;
	M8P_rtme_me2_calc1_rtme_me2_calc1_10_RBUS M8P_rtme_me2_calc1_rtme_me2_calc1_10_reg;
	M8P_rtme_me2_calc0_rtme_me2_calc0_60_RBUS M8P_rtme_me2_calc0_rtme_me2_calc0_60_reg;
	unsigned int print_in_func;


	if (pParam->u1_me2_3drs_wrt_en == 0)
		return;
	
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	M8P_rtme_me2_calc0_rtme_me2_calc0_04_reg.regValue = rtd_inl(M8P_RTME_ME2_CALC0_RTME_ME2_CALC0_04_reg);
#ifdef V20_ALGO_EN
	M8P_rtme_me2_calc1_rtme_me2_calc1_08_reg.regValue = rtd_inl(M8P_RTME_ME2_CALC1_RTME_ME2_CALC1_08_reg);
	M8P_rtme_me2_calc1_rtme_me2_calc1_14_reg.regValue = rtd_inl(M8P_RTME_ME2_CALC1_RTME_ME2_CALC1_14_reg);
	M8P_rtme_me2_calc1_rtme_me2_calc1_10_reg.regValue = rtd_inl(M8P_RTME_ME2_CALC1_RTME_ME2_CALC1_10_reg);
#endif
	M8P_rtme_me2_calc0_rtme_me2_calc0_60_reg.regValue = rtd_inl(M8P_RTME_ME2_CALC0_RTME_ME2_CALC0_60_reg);
	
	mvdiff = s_pContext->_output_me_sceneAnalysis.u10_mvdiff;

	
	if (((s_pContext->_output_me_sceneAnalysis.u5_small_unstb_cnt > 15) && \
		 (s_pContext->_output_me_sceneAnalysis.u6_global_unstb_cnt < 32)) || \
		(s_pContext->_output_me_sceneAnalysis.u6_global_unstb_cnt > 32))
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_04_reg.me2_bv1_pure_sad_coef = 0;
	}
	else
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_04_reg.me2_bv1_pure_sad_coef = 3;
	}


	if (((s_pContext->_output_me_sceneAnalysis.u10_mvdiff > 300) && (s_pContext->_output_me_sceneAnalysis.u6_temporal_adpt_cnt> 0)) || \
		((s_pContext->_output_me_sceneAnalysis.u10_mvdiff > 100) && (s_pContext->_output_me_sceneAnalysis.u6_unstable_score >= 127)))
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_04_reg.me2_bv0_pure_sad_coef = 3;
	}
	else
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_04_reg.me2_bv0_pure_sad_coef = 0;
	}

	if (mvdiff < 32)
	{
		M8P_rtme_me2_calc1_rtme_me2_calc1_14_reg.me2_m_cand_adpt_gain = 9;
		M8P_rtme_me2_calc1_rtme_me2_calc1_10_reg.me2_t_cand_adpt_gain = 9;
	}
	else if (mvdiff < 64)
	{
		M8P_rtme_me2_calc1_rtme_me2_calc1_14_reg.me2_m_cand_adpt_gain = 10;
		M8P_rtme_me2_calc1_rtme_me2_calc1_10_reg.me2_t_cand_adpt_gain = 10;
	}
	else if (mvdiff < 128)
	{
		M8P_rtme_me2_calc1_rtme_me2_calc1_14_reg.me2_m_cand_adpt_gain = 11;
		M8P_rtme_me2_calc1_rtme_me2_calc1_10_reg.me2_t_cand_adpt_gain = 11;
	}
	else
	{
		M8P_rtme_me2_calc1_rtme_me2_calc1_14_reg.me2_m_cand_adpt_gain = 12;
		M8P_rtme_me2_calc1_rtme_me2_calc1_10_reg.me2_t_cand_adpt_gain = 12;
	}

	if (s_pContext->_output_me_sceneAnalysis.u1_freq_scene)
	{
		M8P_rtme_me2_calc1_rtme_me2_calc1_14_reg.me2_m_cand_adpt_gain = 12;
	}

	u3_div = (s_pContext->_output_me_sceneAnalysis.u6_temporal_adpt_cnt< 1) ? 1 : 2;
	M8P_rtme_me2_calc1_rtme_me2_calc1_14_reg.me2_m_cand_adpt_mvd_coring = _CLIP_((mvdiff - 32) >> u3_div, 0, 127);
	M8P_rtme_me2_calc1_rtme_me2_calc1_10_reg.me2_t_cand_adpt_mvd_coring = _CLIP_(mvdiff >> 2, 0, 127);
	M8P_rtme_me2_calc1_rtme_me2_calc1_08_reg.me2_adptpnt_z_mvdiff_coring = _CLIP_(mvdiff >> 2, 0, 127);

	if (mvdiff >= 180)
	{
		M8P_rtme_me2_calc1_rtme_me2_calc1_10_reg.me2_t_cand_adpt_mvd_coring = _CLIP_(mvdiff >> 1, 0, 255);
		M8P_rtme_me2_calc1_rtme_me2_calc1_08_reg.me2_adptpnt_z_mvdiff_coring = _CLIP_(mvdiff >> 1, 0, 255);
	}

	if ((s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 96) && \
		(_ABS_(s_pContext->_output_me_sceneAnalysis.s10_global_fgmvy) < 12) && \
		(_ABS_(s_pContext->_output_me_sceneAnalysis.s10_global_fgmvy) > 0))
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_60_reg.me2_retimer_h_offset=10;
		M8P_rtme_me2_calc0_rtme_me2_calc0_60_reg.me2_retimer_v_offset=10;
	}
	else if (_ABS_(s_pContext->_output_me_sceneAnalysis.s10_global_fgmvy) >= 16)
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_60_reg.me2_retimer_h_offset=8;
		M8P_rtme_me2_calc0_rtme_me2_calc0_60_reg.me2_retimer_v_offset=8;
	}
	else
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_60_reg.me2_retimer_h_offset=9;
		M8P_rtme_me2_calc0_rtme_me2_calc0_60_reg.me2_retimer_v_offset=9;
	}

	if ((s_pContext->_output_me_sceneAnalysis.u10_mvdiff > 50) && s_pContext->_output_me_sceneAnalysis.u1_apl_upsample_en)
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_60_reg.me2_retimer_h_offset=8;
		M8P_rtme_me2_calc0_rtme_me2_calc0_60_reg.me2_retimer_v_offset=8;
	}

	if (s_pContext->_output_me_sceneAnalysis.u1_fast_motion_cond || (s_pContext->_output_me_sceneAnalysis.u6_temporal_adpt_cnt < 1))
	{
		M8P_rtme_me2_calc1_rtme_me2_calc1_10_reg.me2_t_cand_adpt_en = 0;
	}
	else
	{
		M8P_rtme_me2_calc1_rtme_me2_calc1_10_reg.me2_t_cand_adpt_en = 1;
	}

	if ((s_pContext->_output_me_sceneAnalysis.u10_mvdiff > 150) && (s_pContext->_output_me_sceneAnalysis.u6_mean_unstable_score < 16))
	{
		M8P_rtme_me2_calc1_rtme_me2_calc1_08_reg.me2_cross_mvdiff_mode = 0;
	}
	else
	{
		M8P_rtme_me2_calc1_rtme_me2_calc1_08_reg.me2_cross_mvdiff_mode = 1;
	}

	if (s_pContext->_output_me_sceneAnalysis.u6_global_unstb_cnt > 32)
	{
		M8P_rtme_me2_calc1_rtme_me2_calc1_10_reg.me2_t_cand_adpt_en = 1;
		M8P_rtme_me2_calc1_rtme_me2_calc1_14_reg.me2_m_cand_adpt_en = 0;
		M8P_rtme_me2_calc1_rtme_me2_calc1_08_reg.me2_cross_mvdiff_mode = 0;
	}
	else
	{
		M8P_rtme_me2_calc1_rtme_me2_calc1_14_reg.me2_m_cand_adpt_en = 1;
	}
#ifdef V20_ALGO_EN
	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		WriteRegister(M8P_RTME_ME2_CALC1_RTME_ME2_CALC1_18_reg+ 4 * u8_i,
			0,
			9,
			s_pContext->_output_me_sceneAnalysis.s10_rgn_bgmvy[u8_i] & 0x3ff);
		WriteRegister(M8P_RTME_ME2_CALC1_RTME_ME2_CALC1_18_reg+ 4 * u8_i,
			10,
			20,
			s_pContext->_output_me_sceneAnalysis.s11_rgn_bgmvx[u8_i] & 0x7ff);
	}
	unsigned char u8_i;
	int wrt_val;
	for (u8_i=0; u8_i<32; u8_i++)
	{
		wrt_val = s_pContext->_output_me_sceneAnalysis.u8_final_rgn_unstable_score[u8_i];
		wrt_val = _CLIP_((wrt_val - 32 + 4) >> 3, _CLIP_((s_pContext->_output_me_sceneAnalysis.u10_mvdiff - 96) >> 5, 1, 4), 16);
		WriteRegister(M8P_RTME_ME2_CALC1_RTME_ME2_CALC1_18_reg+ 4 * u8_i,
			21,
			26,
			wrt_val);
	}
#endif
	
	rtd_outl(M8P_RTME_ME2_CALC0_RTME_ME2_CALC0_04_reg, M8P_rtme_me2_calc0_rtme_me2_calc0_04_reg.regValue);
#ifdef V20_ALGO_EN
	rtd_outl(M8P_RTME_ME2_CALC1_RTME_ME2_CALC1_08_reg, M8P_rtme_me2_calc1_rtme_me2_calc1_08_reg.regValue);
	rtd_outl(M8P_RTME_ME2_CALC1_RTME_ME2_CALC1_14_reg, M8P_rtme_me2_calc1_rtme_me2_calc1_14_reg.regValue);
	rtd_outl(M8P_RTME_ME2_CALC1_RTME_ME2_CALC1_10_reg, M8P_rtme_me2_calc1_rtme_me2_calc1_10_reg.regValue);
#endif
	rtd_outl(M8P_RTME_ME2_CALC0_RTME_ME2_CALC0_60_reg, M8P_rtme_me2_calc0_rtme_me2_calc0_60_reg.regValue);
}

VOID ME2_BrokenFix_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	int mvdiff= 0;//regr center 24rgn rmv0-rmv1
	int rgnIdx=0,i=0,j=0;
	signed short  rmvx_1st[32]={0},rmvy_1st[32]={0};
	signed short  rmvx_2nd[32]={0},rmvy_2nd[32]={0};

	M8P_rtme_me2_calc0_rtme_me2_calc0_98_RBUS M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg;
	unsigned int print_in_func;
	
	if (pParam->u1_me2_brokenfix_wrt_en == 0)
		return;

	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}

	
	for(rgnIdx=0;rgnIdx<32;rgnIdx++)
	{
		rmvx_1st[rgnIdx]=s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[rgnIdx];
		rmvy_1st[rgnIdx]=s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[rgnIdx];
		rmvx_2nd[rgnIdx]=s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[rgnIdx];
		rmvy_2nd[rgnIdx]=s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[rgnIdx];		
	}

	for(i=1;i<8;i++)
	{
		for(j=1;j<4;j++)
		{
			mvdiff = mvdiff+ABS(rmvx_1st[i+j*8],rmvx_2nd[i+j*8])+ABS(rmvy_1st[i+j*8],rmvy_2nd[i+j*8]);
		}
	}

	M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.regValue = rtd_inl(M8P_RTME_ME2_CALC0_RTME_ME2_CALC0_98_reg);


	if(mvdiff>88)
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_phase_mv_gmvdiff_max_en = 1;
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_extra_cost_en = 1;
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_broken_fix_en = 1;
	}
	else
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_phase_mv_gmvdiff_max_en = 0;
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_extra_cost_en = 0;
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_broken_fix_en = 0;
	}

	if(mvdiff>180)
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_extra_cost_ipdiff_th= 55;
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_extra_cost_gmvdiff_th= 50;
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_phase_mv_gmvdiff_th= 50;
	}
	else if(mvdiff>120)
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_phase_mv_gmvdiff_max_en = mvdiff/4+5;
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_extra_cost_en = mvdiff/4+5;
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_broken_fix_en = mvdiff/4+5;
	}
	else if(mvdiff>60)
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_extra_cost_ipdiff_th= 25;
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_extra_cost_gmvdiff_th= 30;
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_phase_mv_gmvdiff_th= 30;
	}
	else
	{
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_extra_cost_ipdiff_th= 18;
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_extra_cost_gmvdiff_th= 20;
		M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.me2_phase_mv_gmvdiff_th= 20;
	}


	rtd_outl(M8P_RTME_ME2_CALC0_RTME_ME2_CALC0_98_reg, M8P_rtme_me2_calc0_rtme_me2_calc0_98_reg.regValue);

}

VOID IPME_LPF_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned char u8_i;
	M8P_rtme_iphme3_rtme_iphme3_04_RBUS M8P_rtme_iphme3_rtme_iphme3_04_reg;
	M8P_rtme_iphme3_rtme_iphme3_08_RBUS M8P_rtme_iphme3_rtme_iphme3_08_reg;
	M8P_rtme_iphme3_rtme_iphme3_18_RBUS M8P_rtme_iphme3_rtme_iphme3_18_reg;
	M8P_rtme_iphme2_rtme_iphme2_04_RBUS M8P_rtme_iphme2_rtme_iphme2_04_reg;
	M8P_rtme_iphme2_rtme_iphme2_08_RBUS M8P_rtme_iphme2_rtme_iphme2_08_reg;
	M8P_rtme_iphme1_rtme_iphme1_04_RBUS M8P_rtme_iphme1_rtme_iphme1_04_reg;
	M8P_rtme_iphme1_rtme_iphme1_08_RBUS M8P_rtme_iphme1_rtme_iphme1_08_reg;
	unsigned int print_in_func;

	if (pParam->u1_ipme_lpf_wrt_en == 0)
		return;
	
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}

	M8P_rtme_iphme3_rtme_iphme3_04_reg.regValue = rtd_inl(M8P_RTME_IPHME3_RTME_IPHME3_04_reg);
	M8P_rtme_iphme3_rtme_iphme3_08_reg.regValue = rtd_inl(M8P_RTME_IPHME3_RTME_IPHME3_08_reg);
	M8P_rtme_iphme3_rtme_iphme3_18_reg.regValue = rtd_inl(M8P_RTME_IPHME3_RTME_IPHME3_18_reg);
	M8P_rtme_iphme2_rtme_iphme2_04_reg.regValue = rtd_inl(M8P_RTME_IPHME2_RTME_IPHME2_04_reg);
	M8P_rtme_iphme2_rtme_iphme2_08_reg.regValue = rtd_inl(M8P_RTME_IPHME2_RTME_IPHME2_08_reg);
	M8P_rtme_iphme1_rtme_iphme1_04_reg.regValue = rtd_inl(M8P_RTME_IPHME1_RTME_IPHME1_04_reg);
	M8P_rtme_iphme1_rtme_iphme1_08_reg.regValue = rtd_inl(M8P_RTME_IPHME1_RTME_IPHME1_08_reg);
	
	// h coef
	M8P_rtme_iphme3_rtme_iphme3_04_reg.ipme_layer3_h_coef0 = (42 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme3_rtme_iphme3_04_reg.ipme_layer3_h_coef1 = (64 * (16 - s_pContext->_output_flicker_ctrl.ipme_lpf_coef) + 42 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme3_rtme_iphme3_04_reg.ipme_layer3_h_coef2 = 128 - M8P_rtme_iphme3_rtme_iphme3_04_reg.ipme_layer3_h_coef0 - M8P_rtme_iphme3_rtme_iphme3_04_reg.ipme_layer3_h_coef1;
	M8P_rtme_iphme3_rtme_iphme3_18_reg.ipme_layer3_h_coef5 = (42 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme3_rtme_iphme3_18_reg.ipme_layer3_h_coef4 = (64 * (16 - s_pContext->_output_flicker_ctrl.ipme_lpf_coef) + 42 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme3_rtme_iphme3_04_reg.ipme_layer3_h_coef3 = 128 - M8P_rtme_iphme3_rtme_iphme3_18_reg.ipme_layer3_h_coef5 - M8P_rtme_iphme3_rtme_iphme3_18_reg.ipme_layer3_h_coef4;


	M8P_rtme_iphme2_rtme_iphme2_04_reg.ipme_layer2_h_coef0 = (64 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme2_rtme_iphme2_04_reg.ipme_layer2_h_coef1 = 128 - M8P_rtme_iphme2_rtme_iphme2_04_reg.ipme_layer2_h_coef0;
	M8P_rtme_iphme2_rtme_iphme2_04_reg.ipme_layer2_h_coef3 = (64 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme2_rtme_iphme2_04_reg.ipme_layer2_h_coef2 = 128 - M8P_rtme_iphme2_rtme_iphme2_04_reg.ipme_layer2_h_coef3;

	M8P_rtme_iphme1_rtme_iphme1_04_reg.ipme_layer1_h_coef0 = (64 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme1_rtme_iphme1_04_reg.ipme_layer1_h_coef1 = 128 - M8P_rtme_iphme1_rtme_iphme1_04_reg.ipme_layer1_h_coef0;
	M8P_rtme_iphme1_rtme_iphme1_04_reg.ipme_layer1_h_coef3 = (64 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme1_rtme_iphme1_04_reg.ipme_layer1_h_coef2 = 128 - M8P_rtme_iphme1_rtme_iphme1_04_reg.ipme_layer1_h_coef3;

	// v coef
	M8P_rtme_iphme3_rtme_iphme3_08_reg.ipme_layer3_v_coef0 = (42 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme3_rtme_iphme3_08_reg.ipme_layer3_v_coef1 = (128 * (16 - s_pContext->_output_flicker_ctrl.ipme_lpf_coef) + 42 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme3_rtme_iphme3_08_reg.ipme_layer3_v_coef2 = 128 - M8P_rtme_iphme3_rtme_iphme3_08_reg.ipme_layer3_v_coef0 - M8P_rtme_iphme3_rtme_iphme3_08_reg.ipme_layer3_v_coef1;
	M8P_rtme_iphme3_rtme_iphme3_18_reg.ipme_layer3_v_coef5 = (42 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme3_rtme_iphme3_18_reg.ipme_layer3_v_coef4 = (128 * (16 - s_pContext->_output_flicker_ctrl.ipme_lpf_coef) + 42 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme3_rtme_iphme3_08_reg.ipme_layer3_v_coef3 = 128 - M8P_rtme_iphme3_rtme_iphme3_18_reg.ipme_layer3_v_coef5 - M8P_rtme_iphme3_rtme_iphme3_18_reg.ipme_layer3_v_coef4;

	M8P_rtme_iphme2_rtme_iphme2_08_reg.ipme_layer2_v_coef0 = (64 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme2_rtme_iphme2_08_reg.ipme_layer2_v_coef1 = 128 - M8P_rtme_iphme2_rtme_iphme2_08_reg.ipme_layer2_v_coef0;
	M8P_rtme_iphme2_rtme_iphme2_08_reg.ipme_layer2_v_coef3 = (64 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme2_rtme_iphme2_08_reg.ipme_layer2_v_coef2 = 128 - M8P_rtme_iphme2_rtme_iphme2_08_reg.ipme_layer2_v_coef3;

	M8P_rtme_iphme1_rtme_iphme1_08_reg.ipme_layer1_v_coef0 = (64 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme1_rtme_iphme1_08_reg.ipme_layer1_v_coef1 = 128 - M8P_rtme_iphme1_rtme_iphme1_08_reg.ipme_layer1_v_coef0;
	M8P_rtme_iphme1_rtme_iphme1_08_reg.ipme_layer1_v_coef3 = (64 * s_pContext->_output_flicker_ctrl.ipme_lpf_coef + 8) >> 4;
	M8P_rtme_iphme1_rtme_iphme1_08_reg.ipme_layer1_v_coef2 = 128 - M8P_rtme_iphme1_rtme_iphme1_08_reg.ipme_layer1_v_coef3;


	
	if ((s_pContext->_output_flicker_ctrl.flicker_panning_cnt > 15) && \
		(s_pContext->_output_flicker_ctrl.flicker_intp_cnt > 15))
	{
		for (u8_i=0;u8_i<32;u8_i++)
		{
			WriteRegister(M8P_RTME_IPHME3_RTME_IPHME3_20_reg+ 4 * (u8_i >> 3),
				((u8_i & 0x7)<<2),
				((u8_i & 0x7)<<2) + 3,
				8);
			WriteRegister(M8P_RTME_IPHME2_RTME_IPHME2_20_reg+ 4 * (u8_i >> 3),
				((u8_i & 0x7)<<2),
				((u8_i & 0x7)<<2) + 3,
				8);
			WriteRegister(M8P_RTME_IPHME1_RTME_IPHME1_20_reg+ 4 * (u8_i >> 3),
				((u8_i & 0x7)<<2),
				((u8_i & 0x7)<<2) + 3,
				8);
			WriteRegister(M8P_RTME_IPHME0_RTME_IPHME0_20_reg+ 4 * (u8_i >> 3),
				((u8_i & 0x7)<<2),
				((u8_i & 0x7)<<2) + 3,
				8);
		}
	}

	rtd_outl(M8P_RTME_IPHME3_RTME_IPHME3_04_reg, M8P_rtme_iphme3_rtme_iphme3_04_reg.regValue);
	rtd_outl(M8P_RTME_IPHME3_RTME_IPHME3_08_reg, M8P_rtme_iphme3_rtme_iphme3_08_reg.regValue);
	rtd_outl(M8P_RTME_IPHME3_RTME_IPHME3_18_reg, M8P_rtme_iphme3_rtme_iphme3_18_reg.regValue);
	rtd_outl(M8P_RTME_IPHME2_RTME_IPHME2_04_reg, M8P_rtme_iphme2_rtme_iphme2_04_reg.regValue);
	rtd_outl(M8P_RTME_IPHME2_RTME_IPHME2_08_reg, M8P_rtme_iphme2_rtme_iphme2_08_reg.regValue);
	rtd_outl(M8P_RTME_IPHME1_RTME_IPHME1_04_reg, M8P_rtme_iphme1_rtme_iphme1_04_reg.regValue);
	rtd_outl(M8P_RTME_IPHME1_RTME_IPHME1_08_reg, M8P_rtme_iphme1_rtme_iphme1_08_reg.regValue);
}

VOID MC_Deflicker_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	M8P_rtmc_rtmc_04_RBUS M8P_rtmc_rtmc_04_reg;
	M8P_rtmc2_mc_local_lpf_ctrl3_RBUS M8P_rtmc2_mc_local_lpf_ctrl3_reg;
	M8P_rtmc2_mc_local_lpf_ctrl4_RBUS M8P_rtmc2_mc_local_lpf_ctrl4_reg;
	M8P_rtmc_rtmc_74_RBUS M8P_rtmc_rtmc_74_reg;
	unsigned int print_in_func;

	if (pParam->u1_mc_deflicker_wrt_en == 0)
		return;
	
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	M8P_rtmc_rtmc_04_reg.regValue = rtd_inl(M8P_RTMC_RTMC_04_reg);
	M8P_rtmc2_mc_local_lpf_ctrl3_reg.regValue = rtd_inl(M8P_RTMC2_mc_local_lpf_ctrl3_reg);
	M8P_rtmc2_mc_local_lpf_ctrl4_reg.regValue = rtd_inl(M8P_RTMC2_mc_local_lpf_ctrl4_reg);
	M8P_rtmc_rtmc_74_reg.regValue = rtd_inl(M8P_RTMC_RTMC_74_reg);

	if (s_pContext->_output_flicker_ctrl.flicker_intp_cnt > 15)
	{
		M8P_rtmc_rtmc_04_reg.lbmc_intp_rounding_en = 1;
	}
	else
	{
		M8P_rtmc_rtmc_04_reg.lbmc_intp_rounding_en = 0;
	}

	M8P_rtmc2_mc_local_lpf_ctrl3_reg.mc_local_lpf_pxl_diff_gain = (s_pContext->_output_flicker_ctrl.flicker_cnt3 < 16) ? 3 : 4;

	if ((s_pContext->_output_flicker_ctrl.flicker_panning_cnt > 15) && \
		(s_pContext->_output_flicker_ctrl.flicker_intp_cnt > 15))
	{
		M8P_rtmc_rtmc_74_reg.mc_bld_w_type = 2;
	}
	else
	{
		M8P_rtmc_rtmc_74_reg.mc_bld_w_type = 3;
	}
	
	rtd_outl(M8P_RTMC_RTMC_04_reg, M8P_rtmc_rtmc_04_reg.regValue);
	rtd_outl(M8P_RTMC2_mc_local_lpf_ctrl3_reg, M8P_rtmc2_mc_local_lpf_ctrl3_reg.regValue);
	rtd_outl(M8P_RTMC2_mc_local_lpf_ctrl4_reg, M8P_rtmc2_mc_local_lpf_ctrl4_reg.regValue);
	rtd_outl(M8P_RTMC_RTMC_74_reg, M8P_rtmc_rtmc_74_reg.regValue);
	
}

VOID MC_Gfblvl_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	static int hold_gfb=0,cond_gfb=0;
	M8P_rtmc_rtmc_74_RBUS M8P_rtmc_rtmc_74_reg;
	unsigned int print_in_func;
	
	if (pParam->u1_mc_gfb_wrt_en == 0)
		return;

	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	
	M8P_rtmc_rtmc_74_reg.regValue = rtd_inl(M8P_RTMC_RTMC_74_reg);
   
	if(s_pContext->_output_fadeinout_ctrl.u1_detect == 1 || s_pContext->_output_fblevelctrl.u1_gfb_detect == 1)
	{
		M8P_rtmc_rtmc_74_reg.mc_gfb_lvl = 255;
		hold_gfb = 20;
		cond_gfb = 1;
	}
	else if(hold_gfb>0)
	{
		M8P_rtmc_rtmc_74_reg.mc_gfb_lvl = 255;
		hold_gfb--;
		cond_gfb = 1;
	}
	else if(cond_gfb>0)
	{
		M8P_rtmc_rtmc_74_reg.mc_gfb_lvl = 0;
		cond_gfb = 0;
	}
	
	rtd_outl(M8P_RTMC_RTMC_74_reg, M8P_rtmc_rtmc_74_reg.regValue);
}

VOID RimCtrl_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam)
{
	//CONFIG_NEED_TUNNING
}

VOID Identification_Pattern_preProcess_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	//new identify flow
	#if NEW_IDENTIFY_ENTRANCE
	Identify_ctrl_Status_wrtAction(pParam, pOutput);
	#endif
}

VOID Identification_Pattern_wrtAction_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	unsigned int debug_en = 0, debug_type = 0;
	ReadRegister(SOFTWARE2_SOFTWARE2_56_reg, 8, 8, &debug_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_56_reg, 9, 9, &debug_type);

	//new identify flow
#if NEW_IDENTIFY_ENTRANCE
	Identify_ctrl_wrtAction(pParam, pOutput);//UNIFORM_IDENRIFY
#endif

	if(debug_en == 1){
		if(debug_type == 1){
			HAL_MC_SetVarLPFEn(3);
		}else{
			HAL_MC_SetVarLPFEn(0);
		}
	}
}

VOID Wrt_ComReg_Init_RTK2885pp(_OUTPUT_WRT_COM_REG *pOutput)
{
	pOutput->u8_lf_lbmcMode_pre = _MC_NORMAL_;
	pOutput->u8_hf_lbmcMode_pre = _MC_NORMAL_;
}

VOID Wrt_ComReg_Proc_intputInterrupt_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	unsigned int MEMC_RepeatMode = HAL_MC_GetRepeatMode();
	
	if (pParam->u1_Wrt_ComReg_Proc_en==false){
		return;
	}

	if(MEMC_RepeatMode==false)
	{
		if (pParam->u1_Wrt_ComReg_Proc_en)
			WriteComReg_IPPRE_RTK2885pp(pParam, pOutput);

		if (pParam->u1_Wrt_ComReg_BBD_en)
			WriteComReg_BBD_RTK2885pp(pParam, pOutput);
		
		if (pParam->u1_Wrt_ComReg_LOGO_en)
			WriteComReg_LOGO_RTK2885pp(pParam, pOutput);

		if (pParam->u1_Wrt_ComReg_HME1_en)
			WriteComReg_HME1_RTK2885pp(pParam, pOutput);

		if (pParam->u1_Wrt_ComReg_ME15_en)
		{
			WriteComReg_ME15_RTK2885pp(pParam, pOutput);
			dehaloLogo_wrtAction_RTK2885pp(pParam, pOutput);
		}

		if (pParam->u1_Wrt_ComReg_MISC_intputInterrupt_en)
			WriteComReg_MISC_intputInterrupt_RTK2885pp(pParam);
		
		FB_NoNeedToDoFRC_wrtAction(pParam, pOutput);

		// Q-17543
		MEMC_Lib_ME_overflow_Check();	
	}
}

VOID Wrt_ComReg_Proc_outputInterrupt_RTK2885pp(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	unsigned int log_en = 0;
	unsigned char LineMode_flag_chg = 0, LbmcTrig_flag_chg = 0;
	static unsigned char LineMode_flag_pre = 0, LbmcTrig_stage_pre = 0, line_mode_states = 0;
	static unsigned int mc_write_pre = 0;
	unsigned int mc_write = 0, bsize = 0;
	
	unsigned int algo_en[32]={0};
	unsigned char u8_i = 0;
	unsigned int MEMC_RepeatMode = HAL_MC_GetRepeatMode();
	LineMode_flag_chg = (LineMode_flag_pre == MEMC_Lib_get_LineMode_flag()) ? 0 : 1;
	LbmcTrig_flag_chg = (LbmcTrig_stage_pre == MEMC_Lib_get_LbmcTrig_flag()) ? 0 : 1;
	ReadRegister(SOFTWARE1_SOFTWARE1_60_reg, 25, 25, &log_en);
	mc_write = HAL_MC_GetKphaseMCWriteIndex();
	bsize = HAL_MC_GetKphaseMCIndexBsize();


	for(u8_i = 0; u8_i < 32; u8_i++){
		ReadRegister(SOFTWARE3_SOFTWARE3_37_reg, u8_i, u8_i, &algo_en[u8_i]);
	}

	if(pParam->u1_Wrt_ComReg_Proc_en && MEMC_RepeatMode==false)
	{

	#if 1//Merlin8p
	
		Identification_Pattern_preProcess_RTK2885pp(pParam, pOutput);
		// fbCtrl_wrtAction(pParam, pOutput);
		
		// rtd_pr_memc_emerg("//********** [MEMC IRQ test][%s][%d] **********//\n", __FUNCTION__, __LINE__);


		if (pParam->u1_Wrt_ComReg_ME2_en)
			WriteComReg_ME2_RTK2885pp(pParam, pOutput);

		if (pParam->u1_Wrt_ComReg_DH_en)
			WriteComReg_DH_RTK2885pp(pParam, pOutput);

		if (pParam->u1_Wrt_ComReg_MC_en)
			WriteComReg_MC_RTK2885pp(pParam);

		if (pParam->u1_Wrt_ComReg_MISC_outputInterrupt_en)
			WriteComReg_MISC_outputInterrupt_RTK2885pp(pParam);

		Identification_Pattern_wrtAction_RTK2885pp(pParam, pOutput);
	#else
		#if PQL_DYNAMIC_ADJUST_FIX_ISSUE
		if(algo_en[0]){
		Identification_Pattern_preProcess(pParam,pOutput);
		}
		#endif

		if(algo_en[1]){
		fbCtrl_wrtAction(pParam, pOutput);
		}
		if(algo_en[2]){
		dhClose_wrtAction(pParam);
		}
		if(algo_en[3]){
			if(new_dehalo_dynamic == 0){
				ME_obmeSelection_wrtAction(pParam);
			}else{
				ME_obme1Selection_wrtAction(pParam);
				ME_obme2Selection_wrtAction(pParam);
			}
		}

		MC_VarLpf_Coef_WrtAction();
		MovingText_wrtAction();
		
		if(algo_en[4]){
		mv_accord_wrtAction(pParam);
		}
		if(algo_en[5]){
		//dhLocalFbLvl_wrtAction(pParam);
		dynamicME_wrtAction(pParam,pOutput);
		}
	#if TV010_SMALL_OBJECT_ALGO
		if(algo_en[6]){
		MC_blend_for_small_object(pParam);
		}
		if(algo_en[7]){
		FixTriBall_TV010();
		}
	#endif
		
		if(algo_en[8]){
		Identification_Pattern_Update(pParam, pOutput);
		}
	  
#ifdef BG_MV_GET
		if(algo_en[9]){
		gmvCtrl_wrtAction(pParam);//ed
		}
#endif
		if(algo_en[10]){
		ME_cond_zmv_wrtAction();
		}
		if(algo_en[11]){
		blk_logo_480_wrtAction(pOutput);
		}
		if(algo_en[12]){
		logo_deflicker_wrtAction(pParam, pOutput);
		}
		if(algo_en[13]){
		// lucaswang merlin8
		MC_Local_Deflicker_wrtAction(pParam, pOutput);
		}
		// lucaswang merlin8 end
#if PQL_DYNAMIC_ADJUST_FIX_ISSUE
		if(algo_en[14]){
		scCtrl_wrtAction(pParam, pOutput);
		}
		if(algo_en[15]){
		sc_enCtrl_wrtAction(pParam, pOutput);
		}
		if(algo_en[16]){
		Mc2_var_lpf_wrtAction(pParam,pOutput);
		}
		if(algo_en[17]){
		MC_Unsharp_Mask_wrtAction(pParam,pOutput);
		}
		if(algo_en[18]){
		MC_SObj_wrtAction(pParam,pOutput);
		}
		if(algo_en[19]){
		ME2_SObj_wrtAction(pParam,pOutput);
		}
		//IPPRE_POST_CSCMode_wrtAction(pParam,pOutput);
		//MC_blend_alpha_wrtAction(pParam,pOutput);
 
		if(algo_en[20]){
		Dh_Local_FB_wrtAction(pParam,pOutput);
		}
		if(algo_en[21]){
		Dh_OCCL_Ext_wrtAction(pParam,pOutput);
		}
		if(algo_en[22]){
		DW_fmdet_4region_En_wrtAction(pParam,pOutput);
		}
		if(algo_en[23]){
		Identification_Pattern_wrtAction(pParam,pOutput);
		}
		if(algo_en[24]){
		LowFrameProc();
		}
		if(algo_en[25]){
		Dh_MV_Corr_wrtAction(pParam,pOutput);
		}
		if(algo_en[26]){
		Pred_MV_Cand_dis(pParam,pOutput) ;			//pred mv cand disable
		}
#endif
	#endif//Merlin8p
	}

	#if 1//Merlin8p
	Wrt_16segment_show_RTK2885pp(pParam);
	#else
	Wrt_7segment_show(pParam);
	debug_info_show(pParam);
	#endif//Merlin8p
	if(MEMC_RepeatMode==false){
	MEMC_HDMI_PowerSaving(pParam, pOutput);
	}

	//-----line mode state machine
	if(LineMode_flag_chg){
		if(MEMC_Lib_get_LineMode_flag() == 1){
			line_mode_states = 1;
		}else{
			line_mode_states = bsize+3; // 7;
		}
	}

	if(line_mode_states == 1){
		if(MEMC_Lib_get_LineMode_flag() == 1)
			MEMC_Lib_SetLineMode(1);
		else
			MEMC_Lib_SetLineMode(0);
	}
	if(mc_write != mc_write_pre && line_mode_states > 0){
		line_mode_states--;
	}
	mc_write_pre = mc_write;

	if(log_en){
		rtd_pr_memc_notice("[%s][,%d,][,%d,%d,%d,]\n\r", __FUNCTION__, __LINE__, LineMode_flag_chg, mc_write, line_mode_states);
	}
	//-----

	//-----lbmc trig state machine
	if(LbmcTrig_flag_chg){
		if(MEMC_Lib_get_LbmcTrig_flag() == 1)
			MEMC_Lib_SetLbmcVtrigDelayMode(1);
		else
			MEMC_Lib_SetLbmcVtrigDelayMode(0);
	}
	//-----

	MEMC_DisableMute(pParam, pOutput);
	MEMC_Lib_MC_IndexCheck();
	MEMC_Lib_DelayTime_Calc();
	MEMC_LibResetFlow();
	

	if(((get_product_type() != PRODUCT_TYPE_DIAS)&& (Scaler_MEMC_Get_SW_OPT_Flag() == 1)) && MEMC_RepeatMode==false)//dias cadence always keep video
	{
		MEMC_Lib_CadenceChangeCtrl();
	}
	
	/*
	if(Freeze_en)
		MEMC_Lib_Freeze(1);
	else
		MEMC_Lib_Freeze(0);
	*/
	MEMC_Lib_VR360_Flow();
	if(MEMC_RepeatMode==false){
		#if 1//Merlin8p
		#else
	MC_RMV_blending_ReginaolFB_wrtAction();
	Over_SearchRange_wrtAction();
	ME_Median_Filter_wrtAction();
	
	//== normal check for dehalo check== 
	MEMC_me1_bgmask_newalgo_WrtAction(pParam ,pOutput); 
	MEMC_dehalo_newalgo_bgmaskBase_WrtAction(pParam,pOutput);
	//new dehalo setting
	MEMC_dehalo_newalgo_bgmaskBase_WrtAction_addT(pParam,pOutput);
	MEMC_dehalo_newalgo_bgmaskBase_WrtAction_add(pParam ,pOutput);

	//rtd_pr_memc_debug("MEMC_me1_bgmask_newalgo_write");

	MEMC_dehalo_newalgo_me0_test(pParam,pOutput);//me0 test
	//MEMC_dehalo_newalgo_general_diff_WrtAction(pParam,pOutput);
	
	MEMC_SmallObject_wrtAction(pParam);
	#endif
	MEMC_Performance_Checking_Database();
	}
	
	MEMC_LBMC_Apply(pParam, pOutput);
	MEMC_LibSet_PQC_DB_apply();
	MEMC_LibSet_MCDMA_DB_apply();

	//update status
	LineMode_flag_pre = MEMC_Lib_get_LineMode_flag();
	LbmcTrig_stage_pre = MEMC_Lib_get_LbmcTrig_flag();

	if(MEMC_RepeatMode==false){
	Mid_Mode_MEDEN_Checking();
	#if 1//Merlin8p
	#else
	ME2_SObj_wrtAction(pParam,pOutput);
	VarLpf_Coef_wrtAction();
	obme_test_tbl();
	#endif
	//MEMC Self check
	MEMC_Lib_MEMC_Self_Check();
	#if 1//Merlin8p
	#else
	MEMC_Lib_Cadence55_speical_setting(pParam,pOutput);

	//debug show(logo, deflicker, unstable rmv...)
	Debug_Show_Region32_indicator();
	#endif
	}
}

