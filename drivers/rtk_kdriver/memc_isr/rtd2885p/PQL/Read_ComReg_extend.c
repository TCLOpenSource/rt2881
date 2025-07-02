#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_reg_def.h"
#include <rbus/ppoverlay_reg.h>

#define u32_fpga_offset 0x8000
///////////////////////////////////////////////////////////////////////////////////////////
void ReadComReg_IPPRE(const _PARAM_ReadComReg *pParam, _OUTPUT_ReadComReg *pOutput)
{
	_PQLPARAMETER *s_pParam = GetPQLParameter();
	unsigned int u27_ipme_h0Mot_rb = 0;
	unsigned int u27_ipme_h1Mot_rb = 0;
	unsigned int u27_ipme_v0Mot_rb = 0;
	unsigned int u27_ipme_v1Mot_rb = 0;
	unsigned char log_en = 0;
	unsigned int u32_RB_val, u32_i;

	M8P_rtme_iphme_top0_iphme_top_00_RBUS M8P_rtme_iphme_top0_iphme_top_00_rbus;
	M8P_rtme_iphme_top0_iphme_top_04_RBUS M8P_rtme_iphme_top0_iphme_top_04_rbus;
	M8P_rtme_iphme_top0_iphme_top_08_RBUS M8P_rtme_iphme_top0_iphme_top_08_rbus;
	M8P_rtme_iphme_top0_iphme_top_0c_RBUS M8P_rtme_iphme_top0_iphme_top_0c_rbus;
	M8P_rtme_iphme_top0_iphme_top_10_RBUS M8P_rtme_iphme_top0_iphme_top_10_rbus;
	M8P_rtme_iphme_top0_iphme_top_14_RBUS M8P_rtme_iphme_top0_iphme_top_14_rbus;
	M8P_rtme_iphme_top0_iphme_top_18_RBUS M8P_rtme_iphme_top0_iphme_top_18_rbus;
	M8P_rtme_iphme_top0_iphme_top_1c_RBUS M8P_rtme_iphme_top0_iphme_top_1c_rbus;
	M8P_rtme_iphme_top0_iphme_top_20_RBUS M8P_rtme_iphme_top0_iphme_top_20_rbus;
	M8P_rtme_iphme_top0_iphme_top_24_RBUS M8P_rtme_iphme_top0_iphme_top_24_rbus;
	M8P_rtme_iphme_top0_iphme_top_28_RBUS M8P_rtme_iphme_top0_iphme_top_28_rbus;
	M8P_rtme_iphme_top0_iphme_top_2c_RBUS M8P_rtme_iphme_top0_iphme_top_2c_rbus;
	M8P_rtme_iphme_top0_iphme_top_30_RBUS M8P_rtme_iphme_top0_iphme_top_30_rbus;
	M8P_rtme_iphme_top0_iphme_top_34_RBUS M8P_rtme_iphme_top0_iphme_top_34_rbus;
	M8P_rtme_iphme_top0_iphme_top_38_RBUS M8P_rtme_iphme_top0_iphme_top_38_rbus;
	M8P_rtme_iphme_top0_iphme_top_3c_RBUS M8P_rtme_iphme_top0_iphme_top_3c_rbus;
	M8P_rtme_iphme_top0_iphme_top_40_RBUS M8P_rtme_iphme_top0_iphme_top_40_rbus;
	M8P_rtme_iphme_top0_iphme_top_44_RBUS M8P_rtme_iphme_top0_iphme_top_44_rbus;
	M8P_rtme_iphme_top0_iphme_top_48_RBUS M8P_rtme_iphme_top0_iphme_top_48_rbus;
	M8P_rtme_iphme_top0_iphme_top_4c_RBUS M8P_rtme_iphme_top0_iphme_top_4c_rbus;
	M8P_rtme_iphme_top0_iphme_top_50_RBUS M8P_rtme_iphme_top0_iphme_top_50_rbus;
	M8P_rtme_iphme_top0_iphme_top_54_RBUS M8P_rtme_iphme_top0_iphme_top_54_rbus;
	M8P_rtme_iphme_top0_iphme_top_58_RBUS M8P_rtme_iphme_top0_iphme_top_58_rbus;
	M8P_rtme_iphme_top0_iphme_top_5c_RBUS M8P_rtme_iphme_top0_iphme_top_5c_rbus;
	M8P_rtme_iphme_top0_iphme_top_60_RBUS M8P_rtme_iphme_top0_iphme_top_60_rbus;
	M8P_rtme_iphme_top0_iphme_top_64_RBUS M8P_rtme_iphme_top0_iphme_top_64_rbus;
	M8P_rtme_iphme_top0_iphme_top_68_RBUS M8P_rtme_iphme_top0_iphme_top_68_rbus;
	M8P_rtme_iphme4_rtme_iphme4_fmdet_06_RBUS M8P_rtme_iphme4_rtme_iphme4_fmdet_06_rbus;
	M8P_rtme_iphme4_rtme_iphme4_fmdet_07_RBUS M8P_rtme_iphme4_rtme_iphme4_fmdet_07_rbus;
	M8P_rtme_iphme4_rtme_iphme4_fmdet_08_RBUS M8P_rtme_iphme4_rtme_iphme4_fmdet_08_rbus;
	M8P_rtme_iphme4_rtme_iphme4_fmdet_09_RBUS M8P_rtme_iphme4_rtme_iphme4_fmdet_09_rbus;
	unsigned int print_in_func;
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
//	log_en = (rtd_inl(0xb809d9fc)>>0)&0x1;
	M8P_rtme_iphme_top0_iphme_top_00_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_00_reg);
	M8P_rtme_iphme_top0_iphme_top_04_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_04_reg);
	M8P_rtme_iphme_top0_iphme_top_08_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_08_reg);
	M8P_rtme_iphme_top0_iphme_top_0c_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_0C_reg);
	M8P_rtme_iphme_top0_iphme_top_10_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_10_reg);
	M8P_rtme_iphme_top0_iphme_top_14_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_14_reg);
	M8P_rtme_iphme_top0_iphme_top_18_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_18_reg);
	M8P_rtme_iphme_top0_iphme_top_1c_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_1C_reg);
	M8P_rtme_iphme_top0_iphme_top_20_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_20_reg);
	M8P_rtme_iphme_top0_iphme_top_24_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_24_reg);
	M8P_rtme_iphme_top0_iphme_top_28_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_28_reg);
	M8P_rtme_iphme_top0_iphme_top_2c_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_2C_reg);
	M8P_rtme_iphme_top0_iphme_top_30_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_30_reg);
	M8P_rtme_iphme_top0_iphme_top_34_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_34_reg);
	M8P_rtme_iphme_top0_iphme_top_38_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_38_reg);
	M8P_rtme_iphme_top0_iphme_top_3c_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_3C_reg);
	M8P_rtme_iphme_top0_iphme_top_40_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_40_reg);
	M8P_rtme_iphme_top0_iphme_top_44_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_44_reg);
	M8P_rtme_iphme_top0_iphme_top_48_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_48_reg);
	M8P_rtme_iphme_top0_iphme_top_4c_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_4C_reg);
	M8P_rtme_iphme_top0_iphme_top_50_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_50_reg);
	M8P_rtme_iphme_top0_iphme_top_54_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_54_reg);
	M8P_rtme_iphme_top0_iphme_top_58_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_58_reg);
	M8P_rtme_iphme_top0_iphme_top_5c_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_5C_reg);
	M8P_rtme_iphme_top0_iphme_top_60_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_60_reg);
	M8P_rtme_iphme_top0_iphme_top_64_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_64_reg);
	M8P_rtme_iphme_top0_iphme_top_68_rbus.regValue = rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_68_reg);
	M8P_rtme_iphme4_rtme_iphme4_fmdet_06_rbus.regValue = rtd_inl(M8P_RTME_IPHME4_RTME_IPHME4_FMDET_06_reg);
	M8P_rtme_iphme4_rtme_iphme4_fmdet_07_rbus.regValue = rtd_inl(M8P_RTME_IPHME4_RTME_IPHME4_FMDET_07_reg);
	M8P_rtme_iphme4_rtme_iphme4_fmdet_08_rbus.regValue = rtd_inl(M8P_RTME_IPHME4_RTME_IPHME4_FMDET_08_reg);
	M8P_rtme_iphme4_rtme_iphme4_fmdet_09_rbus.regValue = rtd_inl(M8P_RTME_IPHME4_RTME_IPHME4_FMDET_09_reg);

	//////////////////////////////////////////////////////////////////////////
	// ipme, film detect information
	//////////////////////////////////////////////////////////////////////////
	u27_ipme_h0Mot_rb = M8P_rtme_iphme_top0_iphme_top_00_rbus.regr_ipme_fdet_mot_h0_rim;
	u27_ipme_h1Mot_rb = M8P_rtme_iphme_top0_iphme_top_08_rbus.regr_ipme_fdet_mot_h1_rim;
	u27_ipme_v0Mot_rb = M8P_rtme_iphme_top0_iphme_top_64_rbus.regr_ipme_fdet_mot_v0_rim;
	u27_ipme_v1Mot_rb = M8P_rtme_iphme_top0_iphme_top_68_rbus.regr_ipme_fdet_mot_v1_rim;

	if(log_en){
		rtd_pr_memc_emerg("//********** [MEMC IRQ test][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	//if (pParam->u1_ipme_aMot_rb_en == 1)
	if (1)
	{
		unsigned int u27_ipme_rimMot_rb = 0;
		pOutput->u27_ipme_aMot_rb    =  M8P_rtme_iphme_top0_iphme_top_04_rbus.regr_ipme_fdet_mot_all;

		//minus rim line motion from bad-DI, use 12rgn motion to avoid bad-DI affect, but keep this idea here
		u27_ipme_rimMot_rb = (u27_ipme_h0Mot_rb + u27_ipme_h1Mot_rb + u27_ipme_v0Mot_rb + u27_ipme_v1Mot_rb);
		u27_ipme_rimMot_rb = 0;
		pOutput->u27_ipme_aMot_rb = (pOutput->u27_ipme_aMot_rb < u27_ipme_rimMot_rb) ? 0 : (pOutput->u27_ipme_aMot_rb - u27_ipme_rimMot_rb);
	}
	if(log_en){
		rtd_pr_memc_emerg("//********** [MEMC IRQ test][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	//if(pParam->u1_ipme_5rMot_rb_en == 1)
	if (1)
	{
		unsigned short u16_top_v0 = 0;
		unsigned short u16_top_v1 = 0;
		unsigned short u16_bot_v0 = 0;
		unsigned short u16_bot_v1 = 0;
		unsigned short u16_lft_h0 = 0;
		unsigned short u16_lft_h1 = 0;
		unsigned short u16_rgt_h0 = 0;
		unsigned short u16_rgt_h1 = 0;
		unsigned short u16_ipme_res_x = 960;
		unsigned short u16_ipme_res_y = 540;
		unsigned short u16_LR_rimMot_t = 0;
		unsigned short u16_LR_rimMot_b = 0;
		unsigned short u16_TB_rimMot_l = 0;
		unsigned short u16_TB_rimMot_r = 0;

		u16_top_v0 = M8P_rtme_iphme4_rtme_iphme4_fmdet_06_rbus.ipme_fmdet_5region1_v0;
		u16_top_v1 = M8P_rtme_iphme4_rtme_iphme4_fmdet_06_rbus.ipme_fmdet_5region1_v1;
		u16_bot_v0 = M8P_rtme_iphme4_rtme_iphme4_fmdet_07_rbus.ipme_fmdet_5region2_v0;
		u16_bot_v1 = M8P_rtme_iphme4_rtme_iphme4_fmdet_07_rbus.ipme_fmdet_5region2_v1;
		u16_lft_h0 = M8P_rtme_iphme4_rtme_iphme4_fmdet_08_rbus.ipme_fmdet_5region3_h0;
		u16_lft_h1 = M8P_rtme_iphme4_rtme_iphme4_fmdet_08_rbus.ipme_fmdet_5region3_h1;
		u16_rgt_h0 = M8P_rtme_iphme4_rtme_iphme4_fmdet_09_rbus.ipme_fmdet_5region4_h0;
		u16_rgt_h1 = M8P_rtme_iphme4_rtme_iphme4_fmdet_09_rbus.ipme_fmdet_5region4_h1;

		pOutput->u27_ipme_5Mot_rb[0] =  M8P_rtme_iphme_top0_iphme_top_0c_rbus.regr_ipme_fdet_mot_5region1;
		u16_LR_rimMot_t = (u27_ipme_h0Mot_rb + u27_ipme_h1Mot_rb)*((u16_top_v1<u16_top_v0)?0:(u16_top_v1-u16_top_v0))/u16_ipme_res_y;
		#if !MIX_MODE_REGION_17
		u16_LR_rimMot_t = 0;
		#endif
		pOutput->u27_ipme_5Mot_rb[0]= (pOutput->u27_ipme_5Mot_rb[0] < u16_LR_rimMot_t) ? 0 : (pOutput->u27_ipme_5Mot_rb[0] - u16_LR_rimMot_t);

		pOutput->u27_ipme_5Mot_rb[1] =  M8P_rtme_iphme_top0_iphme_top_10_rbus.regr_ipme_fdet_mot_5region2;
		u16_LR_rimMot_b = (u27_ipme_h0Mot_rb + u27_ipme_h1Mot_rb)*((u16_bot_v1<u16_bot_v0)?0:(u16_bot_v1- u16_bot_v0))/u16_ipme_res_y;
		#if !MIX_MODE_REGION_17
		u16_LR_rimMot_b = 0;
		#endif
		pOutput->u27_ipme_5Mot_rb[1]= (pOutput->u27_ipme_5Mot_rb[1] < u16_LR_rimMot_b) ? 0 : (pOutput->u27_ipme_5Mot_rb[1] - u16_LR_rimMot_b);

		pOutput->u27_ipme_5Mot_rb[2] =  M8P_rtme_iphme_top0_iphme_top_14_rbus.regr_ipme_fdet_mot_5region3;
		u16_TB_rimMot_l = (u27_ipme_v0Mot_rb + u27_ipme_v1Mot_rb)*((u16_lft_h1< u16_lft_h0)?0:(u16_lft_h1- u16_lft_h0))/u16_ipme_res_x;
		#if !MIX_MODE_REGION_17
		u16_TB_rimMot_l = 0;
		#endif
		pOutput->u27_ipme_5Mot_rb[2]= (pOutput->u27_ipme_5Mot_rb[2] < u16_TB_rimMot_l) ? 0 : (pOutput->u27_ipme_5Mot_rb[2] - u16_TB_rimMot_l);

		pOutput->u27_ipme_5Mot_rb[3] =  M8P_rtme_iphme_top0_iphme_top_18_rbus.regr_ipme_fdet_mot_5region4;
		u16_TB_rimMot_r = (u27_ipme_v0Mot_rb + u27_ipme_v1Mot_rb)*((u16_rgt_h1< u16_rgt_h0)?0:(u16_rgt_h1- u16_rgt_h0))/u16_ipme_res_x;
		#if !MIX_MODE_REGION_17
		u16_TB_rimMot_r = 0;
		#endif
		pOutput->u27_ipme_5Mot_rb[3]= (pOutput->u27_ipme_5Mot_rb[3] < u16_TB_rimMot_r) ? 0 : (pOutput->u27_ipme_5Mot_rb[3] - u16_TB_rimMot_r);

		pOutput->u27_ipme_5Mot_rb[4] =  M8P_rtme_iphme_top0_iphme_top_1c_rbus.regr_ipme_fdet_mot_5region5;
	}
	if(log_en){
		rtd_pr_memc_emerg("//********** [MEMC IRQ test][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	//if(pParam->u1_ipme_16rMot_rb_en == 1)
	if (1)
	{
		pOutput->u27_ipme_16Mot_rb[0] =  M8P_rtme_iphme_top0_iphme_top_20_rbus.regr_ipme_fdet_mot_16region1;
		pOutput->u27_ipme_16Mot_rb[1] =  M8P_rtme_iphme_top0_iphme_top_24_rbus.regr_ipme_fdet_mot_16region2;
		pOutput->u27_ipme_16Mot_rb[2] =  M8P_rtme_iphme_top0_iphme_top_28_rbus.regr_ipme_fdet_mot_16region3;
		pOutput->u27_ipme_16Mot_rb[3] =  M8P_rtme_iphme_top0_iphme_top_2c_rbus.regr_ipme_fdet_mot_16region4;
		pOutput->u27_ipme_16Mot_rb[4] =  M8P_rtme_iphme_top0_iphme_top_30_rbus.regr_ipme_fdet_mot_16region5;
		pOutput->u27_ipme_16Mot_rb[5] =  M8P_rtme_iphme_top0_iphme_top_34_rbus.regr_ipme_fdet_mot_16region6;
		pOutput->u27_ipme_16Mot_rb[6] =  M8P_rtme_iphme_top0_iphme_top_38_rbus.regr_ipme_fdet_mot_16region7;
		pOutput->u27_ipme_16Mot_rb[7] =  M8P_rtme_iphme_top0_iphme_top_3c_rbus.regr_ipme_fdet_mot_16region8;
		pOutput->u27_ipme_16Mot_rb[8] =  M8P_rtme_iphme_top0_iphme_top_40_rbus.regr_ipme_fdet_mot_16region9;
		pOutput->u27_ipme_16Mot_rb[9] =  M8P_rtme_iphme_top0_iphme_top_44_rbus.regr_ipme_fdet_mot_16region10;
		pOutput->u27_ipme_16Mot_rb[10] =  M8P_rtme_iphme_top0_iphme_top_48_rbus.regr_ipme_fdet_mot_16region11;
		pOutput->u27_ipme_16Mot_rb[11] =  M8P_rtme_iphme_top0_iphme_top_4c_rbus.regr_ipme_fdet_mot_16region12;
	}
	if(log_en){
		rtd_pr_memc_emerg("//********** [MEMC IRQ test][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	#if !MIX_MODE_REGION_17
	{
		//use 12 rgn motion for mixmode to avoid outside of rim
		//top
		unsigned int u27_12rgn_top = 	pOutput->u27_ipme_16Mot_rb[0] +
								pOutput->u27_ipme_16Mot_rb[1] +
								pOutput->u27_ipme_16Mot_rb[2] +
								pOutput->u27_ipme_16Mot_rb[3];
		//bot
		unsigned int u27_12rgn_bot = 	pOutput->u27_ipme_16Mot_rb[8] +
								pOutput->u27_ipme_16Mot_rb[9] +
								pOutput->u27_ipme_16Mot_rb[10] +
								pOutput->u27_ipme_16Mot_rb[11];
		//lft
		unsigned int u27_12rgn_lft = 	pOutput->u27_ipme_16Mot_rb[0] +
								pOutput->u27_ipme_16Mot_rb[4] +
								pOutput->u27_ipme_16Mot_rb[8];
		//rgt
		unsigned int u27_12rgn_rgt = 	pOutput->u27_ipme_16Mot_rb[3] +
								pOutput->u27_ipme_16Mot_rb[7] +
								pOutput->u27_ipme_16Mot_rb[11];

		pOutput->u27_ipme_5Mot_rb[0]= u27_12rgn_top;
		pOutput->u27_ipme_5Mot_rb[1]= u27_12rgn_bot;
		pOutput->u27_ipme_5Mot_rb[2]= u27_12rgn_lft;
		pOutput->u27_ipme_5Mot_rb[3]= u27_12rgn_rgt;

	}
	#endif
	if(log_en){
		rtd_pr_memc_emerg("//********** [MEMC IRQ test][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	if(s_pParam->_param_filmDetectctrl.u1_mixMode_en == 0)
	{
		#if MIX_MODE_REGION_17
		pOutput->u27_ipme_aMot_rb	=	pOutput->u27_ipme_5Mot_rb[0]+pOutput->u27_ipme_5Mot_rb[4]+
										pOutput->u27_ipme_16Mot_rb[0] +
										pOutput->u27_ipme_16Mot_rb[1] +
										pOutput->u27_ipme_16Mot_rb[2] +
										pOutput->u27_ipme_16Mot_rb[3] +
										pOutput->u27_ipme_16Mot_rb[4] +
										pOutput->u27_ipme_16Mot_rb[5] +
										pOutput->u27_ipme_16Mot_rb[6] +
										pOutput->u27_ipme_16Mot_rb[7] +
										pOutput->u27_ipme_16Mot_rb[8] +
										pOutput->u27_ipme_16Mot_rb[9] +
										pOutput->u27_ipme_16Mot_rb[10]+
										pOutput->u27_ipme_16Mot_rb[11];
		#else
		pOutput->u27_ipme_aMot_rb	=	pOutput->u27_ipme_16Mot_rb[0] +
										pOutput->u27_ipme_16Mot_rb[1] +
										pOutput->u27_ipme_16Mot_rb[2] +
										pOutput->u27_ipme_16Mot_rb[3] +
										pOutput->u27_ipme_16Mot_rb[4] +
										pOutput->u27_ipme_16Mot_rb[5] +
										pOutput->u27_ipme_16Mot_rb[6] +
										pOutput->u27_ipme_16Mot_rb[7];
		#endif
	}

	if(log_en){
		rtd_pr_memc_emerg("//********** [MEMC IRQ test][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	if (pParam->u1_ipme_filmMode_rb_en == 1)
	{
		pOutput->u3_ipme_filmMode_rb = M8P_rtme_iphme_top0_iphme_top_60_rbus.regr_ipme_fdet_film_mode;
	}
    // ippre color cnt0
    ReadRegister(M8P_RTMC_IPPRE_IPPRE_50_reg, 0, 23, &u32_RB_val);
    pOutput->u24_ippre_color_cnt0_rb  =  u32_RB_val;

    // ippre color cnt1
    ReadRegister(M8P_RTMC_IPPRE_IPPRE_54_reg, 0, 23, &u32_RB_val);
    pOutput->u24_ippre_color_cnt1_rb  =  u32_RB_val;

    
    // ippre rgn uv cnt
    for (u32_i = 0; u32_i < 8; u32_i++)
    {
        ReadRegister(M8P_RTMC_IPPRE_IPPRE_60_reg + 4 * u32_i, 0, 31, &u32_RB_val);
        pOutput->u16_ippre_rgn_cnt_u_rb[u32_i] = u32_RB_val & 0xffff;
        pOutput->u16_ippre_rgn_cnt_v_rb[u32_i] = (u32_RB_val >> 16) & 0xffff;
    }

    // ipme fdet
    ReadRegister(M8P_RTME_IPHME_TOP0_IPHME_TOP_00_reg, 0, 26, &u32_RB_val);
    pOutput->u27_ipme_fdet_mot_h0_rim_rb  =  u32_RB_val;
    ReadRegister(M8P_RTME_IPHME_TOP0_IPHME_TOP_04_reg, 0, 26, &u32_RB_val);
    pOutput->u27_ipme_fdet_mot_all_rb  =  u32_RB_val;
    ReadRegister(M8P_RTME_IPHME_TOP0_IPHME_TOP_08_reg, 0, 26, &u32_RB_val);
    pOutput->u27_ipme_fdet_mot_h1_rim_rb  =  u32_RB_val;
    ReadRegister(M8P_RTME_IPHME_TOP0_IPHME_TOP_60_reg, 0, 5, &u32_RB_val);
    pOutput->u3_ipme_fdet_film_mode_rb  =  u32_RB_val & 0x7;
    pOutput->u2_ipme_fdet_phase_id_rb  =  (u32_RB_val >> 2) & 0x3;
    
    ReadRegister(M8P_RTME_IPHME_TOP0_IPHME_TOP_64_reg, 0, 26, &u32_RB_val);
    pOutput->u27_ipme_fdet_mot_v0_rim_rb  =  u32_RB_val;
    ReadRegister(M8P_RTME_IPHME_TOP0_IPHME_TOP_68_reg, 0, 26, &u32_RB_val);
    pOutput->u27_ipme_fdet_mot_v1_rim_rb  =  u32_RB_val;

    // ipme fdet mot 5 rgn
    for (u32_i = 0; u32_i < 5; u32_i++)
    {
        ReadRegister(M8P_RTME_IPHME_TOP0_IPHME_TOP_0C_reg + 4 * u32_i, 0, 26, &u32_RB_val);
        pOutput->u27_ipme_fdet_mot_5rgn_rb[u32_i]  =  u32_RB_val;
    }

    // ipme fdet mot 16 rgn
    for (u32_i = 0; u32_i < 16; u32_i++)
    {
        ReadRegister(M8P_RTME_IPHME_TOP0_IPHME_TOP_20_reg + 4 * u32_i, 0, 26, &u32_RB_val);
        pOutput->u27_ipme_fdet_mot_16rgn_rb[u32_i]  =  u32_RB_val;
    }

    // ipme0 film det
    for (u32_i = 0; u32_i < 12; u32_i++)
    {
        ReadRegister(M8P_RTME_IPHME_TOP0_IPHME_TOP_6C_reg + 4 * u32_i, 0, 31, &u32_RB_val);
        pOutput->u32_ipme0_film_det_rb[u32_i]  =  u32_RB_val;
    }

    // ipme1 film det
    for (u32_i = 0; u32_i < 12; u32_i++)
    {
        ReadRegister(M8P_RTME_IPHME_TOP0_IPHME_TOP_9C_reg + 4 * u32_i, 0, 31, &u32_RB_val);
        pOutput->u32_ipme1_film_det_rb[u32_i]  =  u32_RB_val;
    }

    // ipme2 film det
    for (u32_i = 0; u32_i < 12; u32_i++)
    {
        ReadRegister(M8P_RTME_IPHME_TOP0_IPHME_TOP_CC_reg + 4 * u32_i, 0, 31, &u32_RB_val);
        pOutput->u32_ipme2_film_det_rb[u32_i]  =  u32_RB_val;
    }

    // ipme fdet rgn32 apl
    for (u32_i = 0; u32_i < 32; u32_i++)
    {
        if (u32_i < 4)
        {
            ReadRegister(M8P_RTME_IPHME_TOP0_IPHME_TOP_FC_reg, 8 * (u32_i & 0x3), 7 + 8 * (u32_i & 0x3), &u32_RB_val);
        }
        else
        {
            ReadRegister(M8P_RTME_IPHME_TOP1_IPHME_TOP1_00_reg + (((u32_i - 4)>>2)<<2), 8 * (u32_i & 0x3), 7 + 8 * (u32_i & 0x3), &u32_RB_val);
        }
        
        pOutput->u32_ipme_fdet_rgn32_apl_rb[u32_i]  =  u32_RB_val;
    }

    // ipme cprs motion diff
    for (u32_i = 0; u32_i < 8; u32_i++)
    {
        ReadRegister(M8P_RTME_IPHME_TOP1_IPHME_TOP1_1C_reg + 4 * u32_i, 0, 31, &u32_RB_val);
        pOutput->u32_ipme_cprs_motion_diff_rb[u32_i]  =  u32_RB_val;
    }
    
    // ipme fdet mot 9 rgn
    for (u32_i = 0; u32_i < 9; u32_i++)
    {
        ReadRegister(M8P_RTME_IPHME_TOP1_IPHME_TOP1_FDET_MOT_9RGN_0_reg + 4 * u32_i, 0, 26, &u32_RB_val);
        pOutput->u27_ipme_fdet_mot_9rgn_rb[u32_i]  =  u32_RB_val;
    }
    
}

void ReadComReg_BBD(const _PARAM_ReadComReg *pParam, _OUTPUT_ReadComReg *pOutput)
{
	M8P_bbd_bbd_output_ex1_RBUS M8P_bbd_bbd_output_ex1_reg;
	M8P_bbd_bbd_output_ex2_RBUS M8P_bbd_bbd_output_ex2_reg;
	unsigned int u32_RB_val, u32_i;
	unsigned int print_in_func;

	M8P_bbd_bbd_output_ex1_reg.regValue = rtd_inl(M8P_BBD_BBD_OUTPUT_EX1_reg);
	M8P_bbd_bbd_output_ex2_reg.regValue = rtd_inl(M8P_BBD_BBD_OUTPUT_EX2_reg);

	pOutput->u12_BBD_ex[_RIM_TOP] = M8P_bbd_bbd_output_ex1_reg.regr_bbd_top_ex;
	pOutput->u12_BBD_ex[_RIM_BOT] = M8P_bbd_bbd_output_ex1_reg.regr_bbd_bottom_ex;
	pOutput->u12_BBD_ex[_RIM_RHT] = M8P_bbd_bbd_output_ex2_reg.regr_bbd_right_ex;
	pOutput->u12_BBD_ex[_RIM_LFT] = M8P_bbd_bbd_output_ex2_reg.regr_bbd_left_ex;

	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	for (u32_i = 0; u32_i < 4; u32_i++)
	{
		ReadRegister(M8P_BBD_BBD_OUTPUT_TOP_ACTIVE_LINE_reg + 4 *u32_i, 0, 31, &u32_RB_val);
		pOutput->u12_BBD_roughRim_rb[u32_i]  =  u32_RB_val        & 0x1fff;
		pOutput->u1_BBD_roughValid_rb[u32_i] = (u32_RB_val >> 13) & 0x01;
		pOutput->u12_BBD_fineRim_rb[u32_i]   = (u32_RB_val >> 16) & 0x1fff;
		pOutput->u1_BBD_fineValid_rb[u32_i]  = (u32_RB_val >> 29) & 0x01;

		ReadRegister(M8P_BBD_BBD_OUTPUT_TOP_ACTIVE_PIXEL_CNT_reg + 4 *u32_i, 0, 31, &u32_RB_val);
		if (u32_i < 2)
		{
			pOutput->u17_BBD_roughRim_cnt_rb[u32_i]  =  u32_RB_val        & 0x1fff;
			pOutput->u12_BBD_fineRim_cnt_rb[u32_i]   = (u32_RB_val >> 16) & 0x1fff;
		}
		else
		{
			pOutput->u17_BBD_roughRim_cnt_rb[u32_i]  =  u32_RB_val        & 0x1ffff;
			pOutput->u12_BBD_fineRim_cnt_rb[u32_i]   = (u32_RB_val >> 17) & 0xfff;
		}
	}
}

void ReadComReg_KPHASE(const _PARAM_ReadComReg *pParam, _OUTPUT_ReadComReg *pOutput)
{
	if (pParam->u1_kphase_rb_en == 1)
	{
		M8P_rthw_phase_phase_a4_RBUS M8P_rthw_phase_phase_a4_rbus;
		M8P_rthw_crtc1_crtc1_1c_RBUS M8P_rthw_crtc1_crtc1_1c_rbus;
		M8P_rthw_crtc1_crtc1_24_RBUS M8P_rthw_crtc1_crtc1_24_rbus;
		M8P_rthw_crtc1_crtc1_60_RBUS M8P_rthw_crtc1_crtc1_60_rbus;
		M8P_rthw_crtc1_crtc1_f8_RBUS M8P_rthw_crtc1_crtc1_f8_rbus;
		M8P_rthw_mc_ptg_kmc_top_00_RBUS M8P_rthw_mc_ptg_kmc_top_00_rbus;

		M8P_rthw_phase_phase_a4_rbus.regValue = rtd_inl(M8P_RTHW_PHASE_phase_A4_reg); // M8P_RTHW_PHASE_phase_9C_reg
		M8P_rthw_crtc1_crtc1_1c_rbus.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_1C_reg);
		M8P_rthw_crtc1_crtc1_24_rbus.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_24_reg);
		M8P_rthw_crtc1_crtc1_60_rbus.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_60_reg);
		M8P_rthw_crtc1_crtc1_f8_rbus.regValue = rtd_inl(M8P_RTHW_CRTC1_CRTC1_F8_reg);
		M8P_rthw_mc_ptg_kmc_top_00_rbus.regValue = rtd_inl(M8P_RTHW_MC_PTG_kmc_top_00_reg);

		pOutput->u4_kphase_inPhase = M8P_rthw_phase_phase_a4_rbus.regr_nowdma_in_phase;
		pOutput->u1_kphase_inLR = 0; // no use
		pOutput->u8_sys_N_rb = M8P_rthw_crtc1_crtc1_60_rbus.crtc1_n;
		pOutput->u8_sys_M_rb = M8P_rthw_crtc1_crtc1_60_rbus.crtc1_m;
		pOutput->u13_ip_vtrig_dly = M8P_rthw_crtc1_crtc1_1c_rbus.crtc1_ip_vtrig_dly;
		pOutput->u13_me2_org_vtrig_dly = M8P_rthw_crtc1_crtc1_1c_rbus.crtc1_me2_org_vtrig_dly;
		pOutput->u13_me2_vtrig_dly = M8P_rthw_crtc1_crtc1_24_rbus.crtc1_me2_vtrig_dly;
		pOutput->u13_dec_vtrig_dly = M8P_rthw_crtc1_crtc1_24_rbus.crtc1_dec_vtrig_dly;
		pOutput->u13_vtotal = M8P_rthw_crtc1_crtc1_f8_rbus.regr_crtc1_vspll_finer_v_total;
		pOutput->u16_inHTotal = M8P_rthw_mc_ptg_kmc_top_00_rbus.top_patt_htotal;
	}
}

void ReadComReg_LOGO(const _PARAM_ReadComReg *pParam, _OUTPUT_ReadComReg *pOutput)
{
    unsigned int u32_RB_val, u32_i;
    unsigned int print_in_func;
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    if (1)
    {
        for (u32_i = 0; u32_i < 32; u32_i++)
        {
            ReadRegister(M8P_RTME_LOGO3_LOGO2_00_reg + 4 * (u32_i >> 1), (u32_i & 0x1) * 14, 13 + (u32_i & 0x1) * 14, &u32_RB_val);
            pOutput->u14_logo_rg_cnt_rb[u32_i]  =  u32_RB_val;

            ReadRegister(M8P_RTME_LOGO3_LOGO2_40_reg + 4 * u32_i, 0, 31, &u32_RB_val);
            pOutput->u16_logo_rg_y_rb[u32_i]  =  u32_RB_val & 0xffff;
            pOutput->u16_no_logo_rg_y_rb[u32_i]  =  (u32_RB_val >> 16) & 0xffff;

            ReadRegister(M8P_RTME_LOGO4_LOGO4_00_reg + 4 * u32_i, 0, 31, &u32_RB_val);
            pOutput->u12_logo_rg_sobel_hor_rb[u32_i]  =  u32_RB_val & 0xfff;
            pOutput->u12_logo_rg_sobel_ver_rb[u32_i]  =  (u32_RB_val >> 12) & 0xfff;
        }
    }
}

void ReadComReg_HME1(const _PARAM_ReadComReg *pParam, _OUTPUT_ReadComReg *pOutput)
{
    unsigned int u32_RB_val, u32_i;
    const unsigned int u32_offset = 4;
    const unsigned int u32_bv_stats_offset[32] = {
        0, 0, 0, 1, 1, 1, 2, 2,
        2, 0, 0, 0, 1, 1, 1, 2,
        2, 2, 3, 3, 3, 4, 4, 4,
        5, 5, 5, 6, 6, 6, 7, 7,
    };
    const unsigned int u32_bv_stats_addr_offset[32] = {
        20, 10,  0, 20, 10,  0, 20, 10,
         0, 20, 10,  0, 20, 10,  0, 20,
        10,  0, 20, 10,  0, 20, 10,  0,
        20, 10,  0, 20, 10,  0, 10,  0,
    };
    unsigned int print_in_func;
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    // gmv_1st
    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_04_reg - u32_fpga_offset,  0, 10, &u32_RB_val);
    pOutput->s11_me_GMV_1st_vx_rb = ((u32_RB_val >> 10) &1) == 0? (u32_RB_val & 0x7ff) : ((u32_RB_val & 0x7ff)  - (1<<11));
    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_04_reg - u32_fpga_offset, 11, 20, &u32_RB_val);
    pOutput->s10_me_GMV_1st_vy_rb = ((u32_RB_val >>  9) &1) == 0? (u32_RB_val & 0x3ff) : ((u32_RB_val & 0x3ff)  - (1<<10));
    //pOutput->s11_me_GMV_1st_vx_rb = Calib_MV(pOutput->s11_me_GMV_1st_vx_rb);
    //pOutput->s10_me_GMV_1st_vy_rb = Calib_MV(pOutput->s10_me_GMV_1st_vy_rb);

    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_0C_reg - u32_fpga_offset,  0, 15, &u32_RB_val);
    pOutput->u17_me_GMV_1st_cnt_rb = u32_RB_val;
    //pOutput->u17_me_GMV_1st_cnt_rb = Calib_Data(pOutput->u17_me_GMV_1st_cnt_rb, g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);

    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_0C_reg - u32_fpga_offset, 16, 25, &u32_RB_val);
    pOutput->u12_me_GMV_1st_unconf_rb = u32_RB_val;

    // gmv_2nd
    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_04_reg - u32_fpga_offset, 21, 31, &u32_RB_val);
    pOutput->s11_me_GMV_2nd_vx_rb = ((u32_RB_val >> 10) &1) == 0? (u32_RB_val & 0x7ff) : ((u32_RB_val & 0x7ff)  - (1<<11));
    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_08_reg - u32_fpga_offset,  0,  9, &u32_RB_val);
    pOutput->s10_me_GMV_2nd_vy_rb = ((u32_RB_val >>  9) &1) == 0? (u32_RB_val & 0x3ff) : ((u32_RB_val & 0x3ff)  - (1<<10));
    //pOutput->s11_me_GMV_2nd_vx_rb = Calib_MV(pOutput->s11_me_GMV_2nd_vx_rb);
    //pOutput->s10_me_GMV_2nd_vy_rb = Calib_MV(pOutput->s10_me_GMV_2nd_vy_rb);

    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_10_reg - u32_fpga_offset,  0, 15, &u32_RB_val);
    pOutput->u17_me_GMV_2nd_cnt_rb = u32_RB_val;
    //pOutput->u17_me_GMV_2nd_cnt_rb = Calib_Data(pOutput->u17_me_GMV_2nd_cnt_rb, g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);

    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_10_reg - u32_fpga_offset, 16, 25, &u32_RB_val);
    pOutput->u12_me_GMV_2nd_unconf_rb = u32_RB_val;
    
    // gmv_3rd
    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_08_reg - u32_fpga_offset, 10, 20, &u32_RB_val);
    pOutput->s11_me_GMV_3rd_vx_rb = ((u32_RB_val >> 10) &1) == 0? (u32_RB_val & 0x7ff) : ((u32_RB_val & 0x7ff)  - (1<<11));
    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_08_reg - u32_fpga_offset, 21, 30, &u32_RB_val);
    pOutput->s10_me_GMV_3rd_vy_rb = ((u32_RB_val >>  9) &1) == 0? (u32_RB_val & 0x3ff) : ((u32_RB_val & 0x3ff)  - (1<<10));
    //pOutput->s11_me_GMV_3rd_vx_rb = Calib_MV(pOutput->s11_me_GMV_3rd_vx_rb);
    //pOutput->s10_me_GMV_3rd_vy_rb = Calib_MV(pOutput->s10_me_GMV_3rd_vy_rb);

    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_14_reg - u32_fpga_offset,  0, 15, &u32_RB_val);
    pOutput->u16_me_GMV_3rd_cnt_rb = u32_RB_val;
    //pOutput->u16_me_GMV_3rd_cnt_rb = Calib_Data(pOutput->u16_me_GMV_3rd_cnt_rb, g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);

    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_14_reg - u32_fpga_offset, 16, 25, &u32_RB_val);
    pOutput->u10_me_GMV_3rd_unconf_rb = u32_RB_val;


    // sc flag
    ReadRegister(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_50_reg - u32_fpga_offset, 25, 25, &u32_RB_val);
    pOutput->u1_me_sc_flag_rb = u32_RB_val;

    // sc pattern
    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_00_reg - u32_fpga_offset, 0, 31, &u32_RB_val);
    pOutput->u32_me_sc_pattern_rb = u32_RB_val;

    // zmv cnt
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_28_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_zmv_cnt_rb = u32_RB_val;

    // zmv dtl
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_2C_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_zmv_dtl_rb = u32_RB_val;

    // bvy_pcnt0
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_30_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_bvy_pcnt0_rb = u32_RB_val;

    // bvy_pcnt1
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_34_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_bvy_pcnt1_rb = u32_RB_val;

    // bvy_pcnt2
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_38_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_bvy_pcnt2_rb = u32_RB_val;

    // bvy_pcnt3
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_3C_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_bvy_pcnt3_rb = u32_RB_val;

    // bvy_ncnt0
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_40_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_bvy_ncnt0_rb = u32_RB_val;

    // bvy_ncnt1
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_44_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_bvy_ncnt1_rb = u32_RB_val;

    // bvy_ncnt2
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_48_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_bvy_ncnt2_rb = u32_RB_val;

    // bvy_ncnt3
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_4C_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_bvy_ncnt3_rb = u32_RB_val;

    // dtl th sum
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_50_reg - u32_fpga_offset, 0, 27, &u32_RB_val);
    pOutput->u28_me_dtl_th_sum_rb = u32_RB_val;

    // dtl th cnt
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_54_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_dtl_th_cnt_rb = u32_RB_val;

    // hpan cnt
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_58_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_hpan_cnt_rb = u32_RB_val;

    // vpan cnt
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_5C_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_vpan_cnt_rb = u32_RB_val;

    // zgmv cnt
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_60_reg - u32_fpga_offset, 0, 5, &u32_RB_val);
    pOutput->u6_me_zgmv_cnt_rb = u32_RB_val;

    // bv act cnt
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_64_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_bv_act_rb = u32_RB_val;

    // zmv sad dc cnt
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_68_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_zmv_sad_dc_cnt_rb = u32_RB_val;

    // zmv sad dc sum
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_70_reg - u32_fpga_offset, 0, 30, &u32_RB_val);
    pOutput->u31_me_zmv_sad_dc_sum_rb = u32_RB_val;

    // tcss th cnt1
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_78_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_tcss_th_cnt1_rb = u32_RB_val;

    // tcss th cnt2
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_7C_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_tcss_th_cnt2_rb = u32_RB_val;

    // tcss th cnt3
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_80_reg - u32_fpga_offset, 0, 19, &u32_RB_val);
    pOutput->u20_me_tcss_th_cnt3_rb = u32_RB_val;

    // tcss th sum1
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_84_reg - u32_fpga_offset, 0, 30, &u32_RB_val);
    pOutput->u31_me_tcss_th_sum1_rb = u32_RB_val;

    // tcss th sum2
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_88_reg - u32_fpga_offset, 0, 30, &u32_RB_val);
    pOutput->u31_me_tcss_th_sum2_rb = u32_RB_val;

    // tcss th sum3
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_8C_reg - u32_fpga_offset, 0, 30, &u32_RB_val);
    pOutput->u20_me_tcss_th_sum3_rb = u32_RB_val;

    // glb mot
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_90_reg - u32_fpga_offset, 0, 30, &u32_RB_val);
    pOutput->u30_me_glb_mot_rb = u32_RB_val;

    // glb sad
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_94_reg - u32_fpga_offset, 0, 30, &u32_RB_val);
    pOutput->u30_me_glb_sad_rb = u32_RB_val;

    // glb dtl
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_98_reg - u32_fpga_offset, 0, 25, &u32_RB_val);
    pOutput->u26_me_glb_dtl_rb = u32_RB_val;

    // glb scss
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_9C_reg - u32_fpga_offset, 0, 27, &u32_RB_val);
    pOutput->u28_me_glb_scss_rb = u32_RB_val;

    // glb tcss
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_A0_reg - u32_fpga_offset, 0, 27, &u32_RB_val);
    pOutput->u28_me_glb_tcss_rb = u32_RB_val;

    // glb apli
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_A4_reg - u32_fpga_offset, 0, 25, &u32_RB_val);
    pOutput->u26_me_glb_apli_rb = u32_RB_val;

    // glb aplp
    ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_A8_reg - u32_fpga_offset, 0, 25, &u32_RB_val);
    pOutput->u26_me_glb_aplp_rb = u32_RB_val;

    // mvmask cnt
    ReadRegister(M8P_RTME_HME1_TOP12_RTME_HME1_TOP12_14_reg - u32_fpga_offset, 0, 31, &u32_RB_val);
    pOutput->u15_me_mvmask_i_cnt_rb = u32_RB_val & 0x7fff;
    pOutput->u15_me_mvmask_p_cnt_rb = (u32_RB_val >> 15) & 0x7fff;


    // 32 rgn read
    for (u32_i = 0; u32_i < 32; u32_i ++)
    {
        // rmv_1st
        {
            ReadRegister(M8P_RTME_HME1_TOP5_KME_ME1_TOP4_00_reg + 8 * u32_i - u32_fpga_offset,  0, 10, &u32_RB_val);
            pOutput->s11_me_rMV_1st_vx_rb[u32_i] = ((u32_RB_val >> 10) &1) == 0? (u32_RB_val & 0x7ff) : ((u32_RB_val & 0x7ff)  - (1<<11));
            ReadRegister(M8P_RTME_HME1_TOP5_KME_ME1_TOP4_00_reg + 8 * u32_i - u32_fpga_offset, 11, 20, &u32_RB_val);
            pOutput->s10_me_rMV_1st_vy_rb[u32_i] = ((u32_RB_val >>  9) &1) == 0? (u32_RB_val & 0x3ff) : ((u32_RB_val & 0x3ff)  - (1<<10));
            //pOutput->s11_me_rMV_1st_vx_rb[u32_i] = Calib_MV(pOutput->s11_me_rMV_1st_vx_rb[u32_i]);
            //pOutput->s10_me_rMV_1st_vy_rb[u32_i] = Calib_MV(pOutput->s10_me_rMV_1st_vy_rb[u32_i]);

            ReadRegister(M8P_RTME_HME1_TOP6_KME_ME1_TOP5_00_reg + 4 * u32_i - u32_fpga_offset, 0, 9, &u32_RB_val);
            pOutput->u12_me_rMV_1st_cnt_rb[u32_i] = u32_RB_val;
            //pOutput->u12_me_rMV_1st_cnt_rb[u32_i] = Calib_Data(pOutput->u12_me_rMV_1st_cnt_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);

            ReadRegister(M8P_RTME_HME1_TOP6_KME_ME1_TOP5_80_reg + 4 * u32_i - u32_fpga_offset, 0, 9, &u32_RB_val);
            pOutput->u12_me_rMV_1st_unconf_rb[u32_i] = u32_RB_val;		
        }
        
        // rmv_2nd
        {
            ReadRegister(M8P_RTME_HME1_TOP5_KME_ME1_TOP4_00_reg + 8 * u32_i - u32_fpga_offset, 21, 31, &u32_RB_val);
            pOutput->s11_me_rMV_2nd_vx_rb[u32_i] = ((u32_RB_val >> 10) &1) == 0? (u32_RB_val & 0x7ff) : ((u32_RB_val & 0x7ff)  - (1<<11));
            ReadRegister(M8P_RTME_HME1_TOP5_KME_ME1_TOP4_00_reg + 8 * u32_i + u32_offset - u32_fpga_offset,  0,  9, &u32_RB_val);
            pOutput->s10_me_rMV_2nd_vy_rb[u32_i] = ((u32_RB_val >>  9) &1) == 0? (u32_RB_val & 0x3ff) : ((u32_RB_val & 0x3ff)  - (1<<10));
            //pOutput->s11_me_rMV_2nd_vx_rb[u32_i] = Calib_MV(pOutput->s11_me_rMV_2nd_vx_rb[u32_i]);
            //pOutput->s10_me_rMV_2nd_vy_rb[u32_i] = Calib_MV(pOutput->s10_me_rMV_2nd_vy_rb[u32_i]);

            ReadRegister(M8P_RTME_HME1_TOP6_KME_ME1_TOP5_00_reg + 4 * u32_i - u32_fpga_offset, 10, 19, &u32_RB_val);
            pOutput->u12_me_rMV_2nd_cnt_rb[u32_i] = u32_RB_val;
            //pOutput->u12_me_rMV_2nd_cnt_rb[u32_i] = Calib_Data(pOutput->u12_me_rMV_2nd_cnt_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);

            ReadRegister(M8P_RTME_HME1_TOP6_KME_ME1_TOP5_80_reg + 4 * u32_i - u32_fpga_offset, 10, 19, &u32_RB_val);
            pOutput->u12_me_rMV_2nd_unconf_rb[u32_i] = u32_RB_val;		
        }
        
        // rmv_3rd
        {
            ReadRegister(M8P_RTME_HME1_TOP5_KME_ME1_TOP4_00_reg + 8 * u32_i + u32_offset - u32_fpga_offset, 10, 20, &u32_RB_val);
            pOutput->s11_me_rMV_3rd_vx_rb[u32_i] = ((u32_RB_val >> 10) &1) == 0? (u32_RB_val & 0x7ff) : ((u32_RB_val & 0x7ff)  - (1<<11));
            ReadRegister(M8P_RTME_HME1_TOP5_KME_ME1_TOP4_00_reg + 8 * u32_i + u32_offset - u32_fpga_offset, 21, 30, &u32_RB_val);
            pOutput->s10_me_rMV_3rd_vy_rb[u32_i] = ((u32_RB_val >>  9) &1) == 0? (u32_RB_val & 0x3ff) : ((u32_RB_val & 0x3ff)  - (1<<10));
            //pOutput->s11_me_rMV_3rd_vx_rb[u32_i] = Calib_MV(pOutput->s11_me_rMV_3rd_vx_rb[u32_i]);
            //pOutput->s10_me_rMV_3rd_vy_rb[u32_i] = Calib_MV(pOutput->s10_me_rMV_3rd_vy_rb[u32_i]);

            ReadRegister(M8P_RTME_HME1_TOP6_KME_ME1_TOP5_00_reg + 4 * u32_i - u32_fpga_offset, 20, 29, &u32_RB_val);
            pOutput->u10_me_rMV_3rd_cnt_rb[u32_i] = u32_RB_val;
            //pOutput->u10_me_rMV_3rd_cnt_rb[u32_i] = Calib_Data(pOutput->u10_me_rMV_3rd_cnt_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);

            ReadRegister(M8P_RTME_HME1_TOP6_KME_ME1_TOP5_80_reg + 4 * u32_i - u32_fpga_offset, 20, 29, &u32_RB_val);
            pOutput->u10_me_rMV_3rd_unconf_rb[u32_i] = u32_RB_val;		
        }

        // rgn apli
        {
            ReadRegister(M8P_RTME_HME1_TOP10_KME_ME1_TOP9_00_reg + 4 * u32_i - u32_fpga_offset, 0, 19, &u32_RB_val);
            pOutput->u20_me_rAPLi_rb[u32_i] = u32_RB_val;
            //pOutput->u20_me_rAPLi_rb[u32_i] = Calib_Data(pOutput->u20_me_rAPLi_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);
        }

        // rgn aplp
        {
            ReadRegister(M8P_RTME_HME1_TOP10_KME_ME1_TOP9_80_reg + 4 * u32_i - u32_fpga_offset, 0, 19, &u32_RB_val);
            pOutput->u20_me_rAPLp_rb[u32_i] = u32_RB_val;
            //pOutput->u20_me_rAPLp_rb[u32_i] = Calib_Data(pOutput->u20_me_rAPLp_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);
        }

        // rgn freq cnt
        {
            ReadRegister(M8P_RTME_HME1_TOP12_me1_freq_statistic_avgbv_00_reg + 4 * u32_i - u32_fpga_offset, 21, 30, &u32_RB_val);
            pOutput->u10_me_freq_cnt_rb[u32_i] = u32_RB_val;
            //pOutput->u10_me_freq_cnt_rb = Calib_Data(pOutput->u10_me_freq_cnt_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);
        }

        // rgn freq avgbv
        {
            ReadRegister(M8P_RTME_HME1_TOP12_me1_freq_statistic_avgbv_00_reg + 4 * u32_i - u32_fpga_offset, 0, 10, &u32_RB_val);
            pOutput->s11_me_freq_avgbv_vx_rb[u32_i] = ((u32_RB_val >> 10) &1) == 0? (u32_RB_val & 0x7ff) : ((u32_RB_val & 0x7ff)  - (1<<11));
            ReadRegister(M8P_RTME_HME1_TOP12_me1_freq_statistic_avgbv_00_reg + 4 * u32_i - u32_fpga_offset, 11, 20, &u32_RB_val);
            pOutput->s10_me_freq_avgbv_vy_rb[u32_i] = ((u32_RB_val >>  9) &1) == 0? (u32_RB_val & 0x3ff) : ((u32_RB_val & 0x3ff)  - (1<<10));
            //pOutput->s11_me_freq_avgbv_vx_rb[u32_i] = Calib_MV(pOutput->s11_me_freq_avgbv_vx_rb[u32_i]);
            //pOutput->s10_me_freq_avgbv_vy_rb[u32_i] = Calib_MV(pOutput->s10_me_freq_avgbv_vy_rb[u32_i]);
        }

        
        // rgn bv stats
        {
            if (u32_i >= 9)
            {
                ReadRegister(M8P_RTME_HME1_TOP12_me1_bv_stats_rg_idx_9_10_11_cnt_reg + 4 * u32_bv_stats_offset[u32_i] - u32_fpga_offset, u32_bv_stats_addr_offset[u32_i], u32_bv_stats_addr_offset[u32_i] + 9, &u32_RB_val);
            }
            else
            {
                ReadRegister(M8P_RTME_HME1_TOP7_me1_bv_stats_rg_idx_0_1_2_cnt_reg + 4 * u32_bv_stats_offset[u32_i] - u32_fpga_offset, u32_bv_stats_addr_offset[u32_i], u32_bv_stats_addr_offset[u32_i] + 9, &u32_RB_val);
            }
            
            pOutput->u10_me_bv_stats_rb[u32_i] = u32_RB_val;
            //pOutput->u10_me_bv_stats_rb = Calib_Data(pOutput->u10_me_bv_stats_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);
        }

        // rgn prd 
        {
            if ((u32_i & 0x1) == 0)
            {
                ReadRegister(M8P_RTME_HME1_TOP12_me1_statis_prd_cnt_00_reg + 4 * (u32_i >> 1) - u32_fpga_offset, 0, 12, &u32_RB_val);
            }
            else
            {
                ReadRegister(M8P_RTME_HME1_TOP12_me1_statis_prd_cnt_00_reg + 4 * (u32_i >> 1) - u32_fpga_offset, 13, 25, &u32_RB_val);
            }
            pOutput->u13_me_rPRD_rb[u32_i] = u32_RB_val;
            //pOutput->u13_me_rPRD_rb = Calib_Data(pOutput->u13_me_rPRD_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);
        }

        // rgn sobj cnt i
        {
            ReadRegister(M8P_RTME_HME1_TOP11_RTME_HME1_SOBJ_CNT_00_reg + 4 * (u32_i >> 2) - u32_fpga_offset, u32_i * 8, u32_i * 8 + 7, &u32_RB_val);
            pOutput->u13_me_sobj_cnt_i_rb[u32_i] = u32_RB_val;
            //pOutput->u13_me_sobj_cnt_i_rb = Calib_Data(pOutput->u13_me_sobj_cnt_i_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);
        }

        // rgn sobj cnt p
        {
            ReadRegister(M8P_RTME_HME1_TOP11_RTME_HME1_SOBJ_CNT_08_reg + 4 * (u32_i >> 2) - u32_fpga_offset, u32_i * 8, u32_i * 8 + 7, &u32_RB_val);
            pOutput->u13_me_sobj_cnt_p_rb[u32_i] = u32_RB_val;
            //pOutput->u13_me_sobj_cnt_p_rb = Calib_Data(pOutput->u13_me_sobj_cnt_p_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);
        }

        // rgn gmv bv diff hist
        {
            if ((u32_i & 0x1) == 0)
            {
                ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP11_GMV_BV_HIST_2_reg + 4 * (u32_i >> 1) - u32_fpga_offset, 0, 14, &u32_RB_val);
            }
            else
            {
                ReadRegister(M8P_RTME_HME1_TOP7_KME_ME1_TOP11_GMV_BV_HIST_2_reg + 4 * (u32_i >> 1) - u32_fpga_offset, 16, 30, &u32_RB_val);
            }
            pOutput->u15_me_gmv_bv_diff_rb[u32_i] = u32_RB_val;
        }

        // rgn sad
        {
            ReadRegister(M8P_RTME_HME1_TOP8_ME1_STATIS_SAD_01_reg + 4 * u32_i - u32_fpga_offset, 0, 24, &u32_RB_val);
            pOutput->u25_me_rgn_sad_rb[u32_i] = u32_RB_val;
        }

        // rgn dtl
        {
            ReadRegister(M8P_RTME_HME1_TOP8_ME1_STATIS_DTL_01_reg + 4 * u32_i - u32_fpga_offset, 0, 19, &u32_RB_val);
            pOutput->u20_me_rgn_dtl_rb[u32_i] = u32_RB_val;
        }

        // rgn fb lvl
        {
            ReadRegister(M8P_RTME_HME1_TOP8_ME1_STATIS_DTL_01_reg + 4 * u32_i - u32_fpga_offset, 20, 31, &u32_RB_val);
            pOutput->u12_me_rgn_fblvl_rb[u32_i] = u32_RB_val;
        }

        // rgn scss
        {
            ReadRegister(M8P_RTME_HME1_TOP9_KME_ME1_TOP8_00_reg + 4 * u32_i - u32_fpga_offset, 0, 21, &u32_RB_val);
            pOutput->u12_me_rgn_scss_rb[u32_i] = u32_RB_val;
        }

        // rgn tcss
        {
            ReadRegister(M8P_RTME_HME1_TOP9_KME_ME1_TOP8_80_reg + 4 * u32_i - u32_fpga_offset, 0, 21, &u32_RB_val);
            pOutput->u12_me_rgn_tcss_rb[u32_i] = u32_RB_val;
        }
    }
}

void ReadComReg_ME15(const _PARAM_ReadComReg *pParam, _OUTPUT_ReadComReg *pOutput)
{
    unsigned int u32_RB_val, u32_i;
    const unsigned int u32_invld_offset[32] = {
        0, 0, 0, 1, 1, 1, 2, 2,
        2, 3, 3, 3, 4, 4, 4, 5,
        5, 5, 6, 6, 6, 7, 7, 7,
        8, 8, 8, 9, 9, 9,10,10,
    };
    const unsigned int u32_invld_addr_offset[32] = {
         0, 10, 20,  0, 10, 20,  0, 10,
        20,  0, 10, 20,  0, 10, 20,  0,
        10, 20,  0, 10, 20,  0, 10, 20,
         0, 10, 20,  0, 10, 20,  0, 10,
    };
    unsigned int print_in_func;
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    for (u32_i = 0; u32_i < 32; u32_i ++)
    {
        // invalid cnt
        ReadRegister(M8P_RTME_ME15_1_RTME_ME15_10_reg + 4 * u32_invld_offset[u32_i] - u32_fpga_offset, u32_invld_addr_offset[u32_i], u32_invld_addr_offset[u32_i] + 9, &u32_RB_val);
        pOutput->u10_me15_invld_cnt_rb[u32_i] = u32_RB_val;
        //pOutput->u10_me15_invld_cnt_rb[u32_i] = Calib_Data(pOutput->u10_me15_invld_cnt_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);
        
        // invalid remove cnt
        ReadRegister(M8P_RTME_ME15_1_RTME_ME15_3C_reg + 4 * u32_invld_offset[u32_i] - u32_fpga_offset, u32_invld_addr_offset[u32_i], u32_invld_addr_offset[u32_i] + 9, &u32_RB_val);
        pOutput->u10_me15_invld_rm_cnt_rb[u32_i] = u32_RB_val;
        //pOutput->u10_me15_invld_rm_cnt_rb[u32_i] = Calib_Data(pOutput->u10_me15_invld_rm_cnt_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);

        // invalid mv
        ReadRegister(M8P_RTME_ME15_1_RTME_ME15_68_reg + 4 * u32_i - u32_fpga_offset,  10, 20, &u32_RB_val);
        pOutput->s11_me15_invld_mv_vx_rb[u32_i] = ((u32_RB_val >> 10) &1) == 0? (u32_RB_val & 0x7ff) : ((u32_RB_val & 0x7ff)  - (1<<11));
        ReadRegister(M8P_RTME_ME15_1_RTME_ME15_68_reg + 4 * u32_i - u32_fpga_offset,  0, 9, &u32_RB_val);
        pOutput->s10_me15_invld_mv_vy_rb[u32_i] = ((u32_RB_val >>  9) &1) == 0? (u32_RB_val & 0x3ff) : ((u32_RB_val & 0x3ff)  - (1<<10));

    }
}

void ReadComReg_ME2(const _PARAM_ReadComReg *pParam, _OUTPUT_ReadComReg *pOutput)
{
    unsigned int print_in_func;
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
}

void ReadComReg_DH(const _PARAM_ReadComReg *pParam, _OUTPUT_ReadComReg *pOutput)
{
    unsigned int u32_RB_val, u32_i;

    unsigned int print_in_func;
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    for (u32_i = 0; u32_i < 32; u32_i ++)
    {
        // dh cov cnt
        ReadRegister(M8P_RTME_DEHALO0_RTME_DEHALO0_4C_reg + 4 * (u32_i >> 1), 0, 11, &u32_RB_val);
        pOutput->u12_dh_cov_cnt_rb[u32_i] = u32_RB_val;
        //pOutput->u12_dh_cov_cnt_rb[u32_i] = Calib_Data(pOutput->u12_dh_cov_cnt_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);
        
        // dh ucov cnt
        ReadRegister(M8P_RTME_DEHALO0_RTME_DEHALO0_4C_reg + 4 * (u32_i >> 1), 12, 23, &u32_RB_val);
        pOutput->u12_dh_ucov_cnt_rb[u32_i] = u32_RB_val;
        //pOutput->u12_dh_ucov_cnt_rb[u32_i] = Calib_Data(pOutput->u12_dh_ucov_cnt_rb[u32_i], g_ME1_VBUF_GAIN, g_ME1_VBUF_SHIFT);

    }
}

void ReadComReg_MC(const _PARAM_ReadComReg *pParam, _OUTPUT_ReadComReg *pOutput)
{
    unsigned int u32_RB_val, u32_i;

    unsigned int print_in_func;
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    for (u32_i = 0; u32_i < 32; u32_i ++)
    {
        // mc deflicker 32 rgn cnt
        if ((u32_i & 0x1) == 0)
        {
            ReadRegister(M8P_RTMC2_mc_local_lpf_cnt0_reg + 4 * (u32_i >> 1), 0, 14, &u32_RB_val);
        }
        else
        {
            ReadRegister(M8P_RTMC2_mc_local_lpf_cnt0_reg + 4 * (u32_i >> 1), 15, 29, &u32_RB_val);
        }
        pOutput->u15_mc_local_lpf_cnt_rb[u32_i] = u32_RB_val;
    }
    
}

VOID ReadComReg_Init_RTK2885pp(_OUTPUT_ReadComReg *pOutput)
{
}

VOID ReadComReg_Proc_inputInterrupt_RTK2885pp(const _PARAM_ReadComReg *pParam, _OUTPUT_ReadComReg *pOutput)
{
    // unsigned int u32_RB_val;
    // _PQLPARAMETER *s_pParam       = GetPQLParameter();

    ReadComReg_IPPRE(pParam, pOutput);
    ReadComReg_BBD(pParam, pOutput);
    ReadComReg_KPHASE(pParam, pOutput);
    ReadComReg_LOGO(pParam, pOutput);
    ReadComReg_HME1(pParam, pOutput);
    ReadComReg_ME15(pParam, pOutput);
    //if (pParam->u1_BBD_log_en == 1)
    //    rtd_pr_memc_emerg("//********** [%s][%d] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_BBD_fineRim_rb[_RIM_TOP], pOutput->u12_BBD_fineRim_rb[_RIM_BOT], pOutput->u12_BBD_fineRim_rb[_RIM_LFT], pOutput->u12_BBD_fineRim_rb[_RIM_RHT]);	
}

VOID ReadComReg_Proc_oneFifth_OutInterrupt_RTK2885pp(const _PARAM_ReadComReg *pParam, _OUTPUT_ReadComReg *pOutput)
{
	// unsigned int u32_rb_val;
}

VOID ReadComReg_Proc_outputInterrupt_RTK2885pp(const _PARAM_ReadComReg *pParam, _OUTPUT_ReadComReg *pOutput)
{
	// unsigned int u32_RB_val;
	
	ReadComReg_ME2(pParam, pOutput);
	ReadComReg_DH(pParam, pOutput);
	ReadComReg_MC(pParam, pOutput);
}

