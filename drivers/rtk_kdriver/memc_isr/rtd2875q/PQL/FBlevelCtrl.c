#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_isr/PQL/MEMC_ParamTable.h"
#include "memc_isr/PQL/CalibMEMCInfo.h"
#include "memc_isr/PQL/FRC_glb_Context.h"
#include <memc_isr/PQL/FBLevelCtrl.h>
#include "memc_reg_def.h"

#define STATICS_SAD_NUM	(8)

VOID SceneChage_Analysis(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FB_static2move_swSC_det(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
void SC_Signal_Proc(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);

VOID FBTCCurveCalc(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBTCCurve_FastMotion(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBTCCurve_MVChaos(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBTCCurve_SingleFG_FastBG(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBTCCurve_DTL(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBTCCurve_SC(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBTCCurve_ME(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBTCCurve_GMV(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBGood_Scene(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBPan_Scene(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBCross_Scene(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBLevelCalc(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBTCCurve_TH_specialScene(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);

VOID  fbTC_dhClose_ctrl(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID  fbTC_deBlur_ctrl(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID  fbTC_deJudder_ctrl(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);

unsigned int AutoTh_CurveMapping(unsigned int iVal, unsigned int autoThL, unsigned int autoThH, unsigned char outPrecision);
unsigned int Multiply_2Val_protect(unsigned int multiA, unsigned int multiB, unsigned char divideBit_A);
VOID FBSystemCtrl_Proc(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);

#define Debug_log 0


extern _MEMC_PARAM_TABLE MEMC_PARAM_TABLE;
extern unsigned short scalerVIP_GetGreenHue(void);

//////////////////////////////////////////////////////////////////////////

VOID FBLevelCtrl_Init(_OUTPUT_FBLevelCtrl *pOutput)
{
	unsigned char u8_k = 0;
	for(u8_k = 0; u8_k < 32; u8_k ++)
	{
		pOutput->u25_badRgn_iirRgnSad[u8_k] = 0;
	}
	pOutput->u27_FBTempConsisPre            = 0;
	pOutput->u27_FBTempConsisPre_raw        = 0;

	pOutput->u1_FbLvl_dSCclr                = 0;
	pOutput->u8_FbLvl_dSCclr_holdCnt        = 0;

	pOutput->u8_Dehalo_FBLevel              = 0;
	pOutput->u16_judder_lvl                  = 0;
	pOutput->u16_blur_lvl                    = 0;
	pOutput->u1_deBlur_gmvU_isGood          = 0;

	pOutput->u8_HW_sc_badFrm_cnt            = 0;
	pOutput->u8_HW_sc_Frm_cnt               = 0;
	pOutput->u1_preFrm_isBadScene           = 0;
	pOutput->u1_swSC_static2move_true       = 0;
	pOutput->u8_swSC_static_frmCnt          = 0;
	pOutput->u8_swSC_disIIR_holdCnt         = 0;

	pOutput->u1_badRgn_rgnSAD_noIIR         = 0;
	pOutput->u1_fbLvl_tc_noIIR              = 0;
	pOutput->u1_fbLvl_result_noIIR          = 0;

	pOutput->u1_sc_signal                   = 0;
	pOutput->u8_sc_bypass_cnt               = 0;
	pOutput->u8_mc_phase_pre                = 0;
	pOutput->u8_static_cnt                  = 0;

	pOutput->u1_panx_scene               = 0;
	pOutput->u1_pany_scene                = 0;
	pOutput->u8_pan_scene_hold_cnt                  = 0;

	pOutput->u8_cross_scene                  = 0;
}

//Input signal and parameters are connected locally
VOID FBLevelCtrl_Proc(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
//	const _PQLCONTEXT *s_pContext = GetPQLContext();
	//LogPrintf(DBG_MSG, "\r\n");

	if (pParam->u1_FBLevelCtrl_en == 1)
	{
		SC_Signal_Proc(pParam, pOutput);

		FB_static2move_swSC_det(pParam, pOutput);
		SceneChage_Analysis(pParam, pOutput);

		{
			unsigned char  u1_bigFBlvl_disIIR = pParam->u1_FbLvl_disIIR_en == 1 && pOutput->u1_FbLvl_dSCclr == 1? 1 : 0;
			unsigned char  u1_SW_sc_disIIR	  = (pOutput->u8_swSC_disIIR_holdCnt > 0)? 1 : 0;
			unsigned char  u1_noIIR_en		  = (pParam->u1_scAnalysis_en == 1)? u1_SW_sc_disIIR : u1_bigFBlvl_disIIR;

			pOutput->u1_badRgn_rgnSAD_noIIR     = u1_noIIR_en; // for chip, u1_noIIR_en;
			pOutput->u1_fbLvl_tc_noIIR          = 0; // for chip, u1_noIIR_en;
			pOutput->u1_fbLvl_result_noIIR      = u1_noIIR_en; // for chip, u1_noIIR_en;
						                             // because FPGA PC-running is very slow, so there is no need to disable IIR.
		}

		FBTCCurveCalc(pParam, pOutput);

		FBLevelCalc(pParam, pOutput);
	}
	else
	{
	    pOutput->u8_FBLevel = 0;
	}

	// flbk effect ctrl.
	{
		if (pParam->u1_FbLvl_disableSCclr_en == 1)
		{
			unsigned char u1_disable_SCclr_en;
			if (pOutput->u8_FBLevel > pParam->u8_FbLvl_dSCclr_en2d_th)
			{
				u1_disable_SCclr_en = 1;
				pOutput->u8_FbLvl_dSCclr_holdCnt = pParam->u8_FbLvl_dSCclr_holdCnt;
			}
			else if (pOutput->u8_FBLevel < pParam->u8_FbLvl_dSCclr_d2en_th)
			{
				u1_disable_SCclr_en = 0;
				pOutput->u8_FbLvl_dSCclr_holdCnt = pOutput->u8_FbLvl_dSCclr_holdCnt<1? 0 : (pOutput->u8_FbLvl_dSCclr_holdCnt - 1);
			}
			else
			{
				u1_disable_SCclr_en = pOutput->u1_FbLvl_dSCclr;
				pOutput->u8_FbLvl_dSCclr_holdCnt = pOutput->u8_FbLvl_dSCclr_holdCnt<1? 0 : (pOutput->u8_FbLvl_dSCclr_holdCnt - 1);
			}
			pOutput->u1_FbLvl_dSCclr = (u1_disable_SCclr_en || pOutput->u8_FbLvl_dSCclr_holdCnt > 0);
		}
		else
		{
			pOutput->u1_FbLvl_dSCclr = 0;
		}

		pOutput->u3_FbLvl_dSCclr_wrt = (pOutput->u1_FbLvl_dSCclr == 1)? 0 : 1;
	}

	//For system fallback
	FBSystemCtrl_Proc(pParam, pOutput);

}

VOID FBTCCurveCalc(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int u32_RB_val;

	ReadRegister(HARDWARE_HARDWARE_58_reg, 0, 31, &u32_RB_val);  //bit 18~24

	if(((u32_RB_val >>  18) & 0x01) ==0)
	FBTCCurve_SC(pParam, pOutput);

	if (pOutput->u8_SCHoldCnt == 0 && pOutput->u8_SCNormCnt == pParam->u8_SC_NormCnt)
	{
		if(((u32_RB_val >>  19) & 0x01) ==0)
		FBTCCurve_TH_specialScene(pParam, pOutput);

		if(((u32_RB_val >>  20) & 0x01) ==0)
		FBPan_Scene(pParam, pOutput);

		FBTCCurve_FastMotion(pParam,pOutput);
		FBTCCurve_MVChaos(pParam,pOutput);
		FBTCCurve_DTL(pParam,pOutput);
		FBTCCurve_SingleFG_FastBG(pParam,pOutput);

		//bad region control
		if(((u32_RB_val >>  21) & 0x01) ==0){
			if(s_pContext->_output_frc_sceneAnalysis.u8_sad_overshoot_flag!=1)
			FBTCCurve_ME(pParam, pOutput);
		}
		//GMV control
		if(((u32_RB_val >>  22) & 0x01) ==0)
		FBTCCurve_GMV(pParam, pOutput);
		if(((u32_RB_val >>  23) & 0x01) ==0)
		FBCross_Scene(pParam, pOutput);
		if(((u32_RB_val >>  24) & 0x01) ==0)
		FBGood_Scene(pParam, pOutput);

	}
/*
	// user defined fb level, controled by gain. need overflow protection.
	{
		unsigned char u8_dft_fbLvl_gain      = s_pContext->_external_data.u8_flbk_lvl;
		pOutput->u27_TempConsis_Th_h = (Multiply_2Val_protect(pOutput->u27_TempConsis_Th_h, u8_dft_fbLvl_gain, 8) + 32) >> 6;
		pOutput->u27_TempConsis_Th_l = (Multiply_2Val_protect(pOutput->u27_TempConsis_Th_l, u8_dft_fbLvl_gain, 8) + 32) >> 6;
	}
*/
}

VOID FBTCCurve_MVChaos(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned char u8_i = 0;
	unsigned int log_en, SC_th, max_rSC = 0, max_rTC = 0, sum_rmv_1st_cnt = 0, sum_rmv_2nd_cnt = 0, u8_Gain_min = 0, u8_MVChaos_max_cnt = 0, u8_unconf_th = 0, u8_smooth_chg_num = 0;
	unsigned int algo_en, algo_mv_chaos_en, algo_mv_unconf_en, plot_en, u8_Gain;
	unsigned short u12_GMV_1st_unconf = s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb;
	unsigned short u12_GMV_2nd_unconf = s_pContext->_output_read_comreg.u12_me_GMV_2nd_unconf_rb;
	static unsigned char u8_smooth_chg_cnt = 0;

	ReadRegister(SOFTWARE3_SOFTWARE3_12_reg,  0,  5, &u8_Gain_min);
	ReadRegister(SOFTWARE3_SOFTWARE3_12_reg,  6,  6, &algo_mv_chaos_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_12_reg,  7,  7, &algo_mv_unconf_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_12_reg,  8, 15, &u8_unconf_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_12_reg, 16, 23, &u8_MVChaos_max_cnt);
	ReadRegister(SOFTWARE3_SOFTWARE3_12_reg, 24, 30, &SC_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_12_reg, 31, 31, &log_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_61_reg,  0,  7, &u8_smooth_chg_num);
	ReadRegister(SOFTWARE3_SOFTWARE3_61_reg, 30, 30, &plot_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_61_reg, 31, 31, &algo_en);

	if(!algo_en){
		return;
	}

	SC_th = SC_th * 100000;
	u8_smooth_chg_num = (u8_smooth_chg_num==0) ? 1 : u8_smooth_chg_num;
	
	for(u8_i=0; u8_i<32; u8_i++){
		sum_rmv_1st_cnt += s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[u8_i];
		sum_rmv_2nd_cnt += s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[u8_i];
		max_rSC = _MAX_(s_pContext->_output_read_comreg.u22_me_rSC_rb[u8_i], max_rSC);
		max_rTC = _MAX_(s_pContext->_output_read_comreg.u22_me_rTC_rb[u8_i], max_rTC);
	}

	if(algo_mv_chaos_en && s_pContext->_output_read_comreg.u27_me_aSC_rb > SC_th){
		pOutput->u1_MV_Chaos = true;
	}else{
		pOutput->u1_MV_Chaos = false;
	}

	if(algo_mv_unconf_en && u12_GMV_1st_unconf > u8_unconf_th && u12_GMV_2nd_unconf > u8_unconf_th && u12_GMV_2nd_unconf!=STILL_PICTURE_2ND_UNCONF){
		pOutput->u1_large_mv_unconf = true;
	}else{
		pOutput->u1_large_mv_unconf = false;
	}

	if(pOutput->u1_MV_Chaos == true || pOutput->u1_large_mv_unconf == true){
		pOutput->u8_MVChaos_cnt = u8_MVChaos_max_cnt;
	}else{
		pOutput->u8_MVChaos_cnt = (pOutput->u8_MVChaos_cnt==0) ? 0 : pOutput->u8_MVChaos_cnt-1;
	}

	if(pOutput->u8_MVChaos_cnt>0){
		u8_smooth_chg_cnt = (u8_smooth_chg_cnt<u8_smooth_chg_num) ? u8_smooth_chg_cnt+1 : u8_smooth_chg_num;
		u8_Gain = 256-((256-u8_Gain_min)*u8_smooth_chg_cnt/u8_smooth_chg_num);
		
		pOutput->u27_TempConsis_Th_h = (pOutput->u27_TempConsis_Th_h * u8_Gain + (1<<7))>>8;
		pOutput->u27_TempConsis_Th_l = (pOutput->u27_TempConsis_Th_l * u8_Gain + (1<<7))>>8;
		
		if(plot_en){
			WriteRegister(MC_MC_CC_reg, 25, 25, 1);
			WriteRegister(MC_MC_CC_reg, 27, 27, 1);
			WriteRegister(MC_MC_CC_reg,  0, 23, 0xff004c);
			WriteRegister(MC_MC_C8_reg,  0, 11, 1000);
			WriteRegister(MC_MC_C8_reg, 16, 27, 550);
		}
	}else{
		u8_smooth_chg_cnt = 0;
		if(plot_en){
			WriteRegister(MC_MC_CC_reg, 25, 25, 0);
			WriteRegister(MC_MC_CC_reg, 27, 27, 0);
		}
	}
	#if 0
	if(log_en){//d930[31]
		rtd_pr_memc_emerg("[%s] %d,%d, cnt,%d, gmv_uncont,%d,%d, diff,%u,%u, max_rSC,%u,%u, gmv_cnt,%u,%u, rmv_cnt,%u,%u, th,%u,%u\n", __FUNCTION__, pOutput->u1_MV_Chaos, pOutput->u1_large_mv_unconf, pOutput->u8_MVChaos_cnt,
			u12_GMV_1st_unconf, u12_GMV_2nd_unconf,
			s_pContext->_output_read_comreg.u27_me_aTC_rb, s_pContext->_output_read_comreg.u27_me_aSC_rb, max_rSC, max_rTC,
			s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb, s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb,
			sum_rmv_1st_cnt, sum_rmv_2nd_cnt, pOutput->u27_TempConsis_Th_h, pOutput->u27_TempConsis_Th_l);
	}
	#else
	if(log_en && pOutput->u8_MVChaos_cnt>0){ //d930[31]
		rtd_pr_memc_emerg("u8_Gain,%d u8_smooth_chg_cnt,%d, %u\n", u8_Gain, u8_smooth_chg_cnt, pOutput->u27_TempConsis_Th_h);
	}
	#endif
}

VOID FBTCCurve_FastMotion(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int u8_mv_th = 0, u8_Gain = 0, u8_FastMotion_maxcnt = 0, algo_en = 0, log_en = 0, mv_select = 0, u5_rmv_num_th = 0;
	//unsigned int u16_gmv_2nd_cnt_th = 0, u8_rmv_2nd_cnt_th = 0, u1_Only_Detect_MVY = 0, u8_uniform_panning_max_cnt = 0, u31_sad_th = 0;
	unsigned int u16_gmv_2nd_cnt_th = 0, u8_rmv_2nd_cnt_th = 0, u1_Only_Detect_MVY = 0, u8_uniform_panning_max_cnt = 0, u31_sad_th = 0, u8_MAD_th = 0, u1_MAD_check_en = 0;
	unsigned short u11_MaxGMV = 0;
	unsigned char u8_i = 0, u5_large_rmv_num = 0;
	//bool u1_detect_fastmotion = false, u1_detect_GMV_fast = false, u1_detect_RMV_fast = false;
	bool u1_detect_fastmotion = false, u1_detect_GMV_fast = false, u1_detect_RMV_fast = false, u1_small_MAD = false;
	bool u1_MV_exceed_searchrange = false;
	unsigned int u8_exceed_searchrange_mv = 0, plot_en;

	ReadRegister(SOFTWARE3_SOFTWARE3_08_reg,  0,  7, &u8_mv_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_08_reg,  8, 15, &u8_FastMotion_maxcnt);
	ReadRegister(SOFTWARE3_SOFTWARE3_08_reg, 16, 21, &u8_Gain);
	ReadRegister(SOFTWARE3_SOFTWARE3_08_reg, 22, 22, &u1_Only_Detect_MVY);
	ReadRegister(SOFTWARE3_SOFTWARE3_08_reg, 23, 23, &log_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_08_reg, 24, 28, &u5_rmv_num_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_08_reg, 29, 30, &mv_select);
	ReadRegister(SOFTWARE3_SOFTWARE3_08_reg, 31, 31, &algo_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_07_reg,  0, 15, &u16_gmv_2nd_cnt_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_07_reg, 16, 23, &u8_rmv_2nd_cnt_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_07_reg, 24, 31, &u8_uniform_panning_max_cnt);
	ReadRegister(SOFTWARE3_SOFTWARE3_02_reg,  0, 23, &u31_sad_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_02_reg, 24, 31, &u8_exceed_searchrange_mv);
	ReadRegister(SOFTWARE3_SOFTWARE3_61_reg, 29, 29, &plot_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_61_reg, 16, 23, &u8_MAD_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_61_reg, 28, 28, &u1_MAD_check_en);


	if(!algo_en){
		return;
	}

	if(u1_Only_Detect_MVY){
		u11_MaxGMV = _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb);
		if(s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb >= u16_gmv_2nd_cnt_th){
			u11_MaxGMV = _MAX_(u11_MaxGMV, _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb));
		}

	}else{
		u11_MaxGMV = _MAX_(_ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb), _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb));
		if(s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb >= u16_gmv_2nd_cnt_th){
			u11_MaxGMV = _MAX_(u11_MaxGMV, _MAX_(_ABS_(s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb), _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb)));
		}
	}

	for(u8_i=0; u8_i<32; u8_i++){
		if(u1_Only_Detect_MVY){
			if(_ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i]) >= u8_mv_th){
				u5_large_rmv_num++;
			}else if(s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[u8_i] >= u8_rmv_2nd_cnt_th){
				if(_ABS_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i]) >= u8_mv_th){
					u5_large_rmv_num++;
				}
			}
		}else{
			if(_ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i]) >= u8_mv_th || _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i]) >= u8_mv_th){
				u5_large_rmv_num++;
			}else if(s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[u8_i] >= u8_rmv_2nd_cnt_th){
				if(_ABS_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i]) >= u8_mv_th || _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i]) >= u8_mv_th){
					u5_large_rmv_num++;
				}
			}
		}
	}

	u1_detect_GMV_fast = (u11_MaxGMV >= u8_mv_th) ? true : false;
	u1_detect_RMV_fast = (u5_large_rmv_num >= u5_rmv_num_th) ? true : false;

	u1_MV_exceed_searchrange = (_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb) >= u8_exceed_searchrange_mv) ? true : false;

	pOutput->u1_detect_GMV_fast = u1_detect_GMV_fast;
	pOutput->u1_detect_RMV_fast = u1_detect_RMV_fast;
	pOutput->u5_large_rmv_num = u5_large_rmv_num;
	pOutput->u1_large_SAD = (s_pContext->_output_read_comreg.u30_me_aSAD_rb>u31_sad_th) ? true : false;

	if(mv_select==0){//gmv
		u1_detect_fastmotion = u1_detect_GMV_fast;
	}else if(mv_select==1){//rmv
		u1_detect_fastmotion = u1_detect_RMV_fast;
	}else{//gmv or rmv
		u1_detect_fastmotion = u1_detect_GMV_fast | u1_detect_RMV_fast;
	}
	
	if((u1_detect_fastmotion && pOutput->u1_large_SAD && u1_small_MAD==false) || u1_MV_exceed_searchrange){
		pOutput->u8_FastMotion_cnt = u8_FastMotion_maxcnt;
	}
	pOutput->u8_FastMotion_cnt = (pOutput->u8_FastMotion_cnt==0) ? 0 : pOutput->u8_FastMotion_cnt-1;

	if(pOutput->u8_FastMotion_cnt > 0){
		pOutput->u27_TempConsis_Th_h = (pOutput->u27_TempConsis_Th_h * u8_Gain + (1<<6))>>7;
		pOutput->u27_TempConsis_Th_l = (pOutput->u27_TempConsis_Th_l * u8_Gain + (1<<6))>>7;
		if(plot_en){
			WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 13, 13, 1);
			WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 15, 15, 1);
			WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 0, 12, 1920);
			WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 16, 28, 1080);
		}
	}else{
		if(plot_en){
			WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 13, 13, 0);
			WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 15, 15, 0);
		}
	}
	//	if(log_en){//d920[23]
	//		rtd_pr_memc_emerg("[%s] u8_FastMotion_cnt,%d, MaxGmv,%d, LargeRmv,%d, SR,%d, sad,%u, MAD,%d, th,%u,%u\n",__FUNCTION__, pOutput->u8_FastMotion_cnt, u11_MaxGMV, u5_large_rmv_num, u1_MV_exceed_searchrange, s_pContext->_output_read_comreg.u30_me_aSAD_rb, u1_small_MAD, pOutput->u27_TempConsis_Th_h, pOutput->u27_TempConsis_Th_l);
	//	}
	if(log_en){//d920[23]
		rtd_pr_memc_emerg("[%s] flg,%d,%d, detect,%d,%d, MaxGmv,%d, gmv,%d,%d, 2nd_cnt,%d, rmv,%d\n",__FUNCTION__, 
			u1_detect_fastmotion, pOutput->u8_FastMotion_cnt, u1_detect_GMV_fast, u1_detect_RMV_fast,
			u11_MaxGMV, s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb, 
			s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb,
			s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb,
			u5_large_rmv_num);
	}
}

VOID FBTCCurve_SingleFG_FastBG(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	char uniform_rmv_num = 0;
	int uniform_rmv_x_avg = 0, uniform_rmv_y_avg = 0, uniform_rmv_x_sum = 0, uniform_rmv_y_sum = 0;
	bool fast_rmv_match = false, rgn_uniform_match = false, sad_match = false, rmv_diff_match = false, dtl_match = false, u1_all_rgn_has_group = true, u1_only_one_group_mv = true;
	unsigned char u8_i = 0, start_id = 0, end_id = 31, fast_rgn_num_th = 0, fast_rgn_num = 0, rmv_no_uniform_rgn[32]={0};
	unsigned int algo_en=0, log_en=0, show_cross=0, fast_rmv_th=0, TempConsis_gain=0, rSAD_th=0, uniform_diff_th=0, gain_diff_avg=0, after_cnt_th=0, sc_cnt_th=0;
	unsigned int rSAD_sum = 0, rSAD_nouniform_avg = 0, rSAD_nouniform_num = 0, rSAD_nouniform_static_avg = 0, rSAD_nouniform_static_sum = 0;
	unsigned int nouniform_rmv_num = 0, uniform_rmv_diff_sum = 0, uniform_rmv_diff_avg = 0, uniform_nouniform_diff_sum = 0, uniform_nouniform_diff_avg = 0, uniform_rmv_avg = 0;
	unsigned int aDTL = s_pContext->_output_read_comreg.u25_me_aDTL_rb;
	unsigned short top_rim = s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP];
	unsigned short bot_rim = s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_BOT];
	unsigned short resolution_height = s_pContext->_output_rimctrl.u12_out_resolution[_RIM_BOT]+1;
	static unsigned int rSAD_nouniform_static[STATICS_SAD_NUM] = {0};

	ReadRegister(SOFTWARE3_SOFTWARE3_18_reg, 31, 31, &algo_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_18_reg, 30, 30, &log_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_18_reg, 29, 29, &show_cross);
	ReadRegister(SOFTWARE3_SOFTWARE3_18_reg, 16, 26, &rSAD_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_18_reg,  8, 15, &fast_rmv_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_18_reg,  0,  7, &TempConsis_gain);
	ReadRegister(SOFTWARE3_SOFTWARE3_19_reg,  0,  7, &uniform_diff_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_19_reg,  8, 15, &gain_diff_avg);
	ReadRegister(SOFTWARE3_SOFTWARE3_19_reg, 16, 23, &after_cnt_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_19_reg, 24, 31, &sc_cnt_th);
	rSAD_th *= 100;

	if(!algo_en){
		return;
	}

	//get the start and end region id for checking rmv
	if(top_rim>((resolution_height*5)>>4)){
		start_id = 16;
	}else if(top_rim>((resolution_height*1)>>4)){
		start_id = 8;
	}else{
		start_id = 0;
	}

	if(bot_rim<((resolution_height*11)>>4)){
		end_id = 15;
	}else if(bot_rim<((resolution_height*15)>>4)){
		end_id = 23;
	}else{
		end_id = 31;
	}

	if(start_id>=end_id){
		return;
	}

	fast_rgn_num_th = ((end_id+1-start_id)*205 + (1<<7))>>8; //80% region; ex:32 region, th=26

	// calculate the avg rmv with uniform mv region, avg sad with uniform and no-uniform mv region
	for(u8_i=start_id; u8_i<=end_id; u8_i++){
		if(s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[u8_i]!=0){
			rmv_no_uniform_rgn[u8_i] = RMV_NO_UNIFORM;
			rSAD_sum += s_pContext->_output_read_comreg.u25_me_rSAD_rb[u8_i];
			rSAD_nouniform_num++;
			u1_only_one_group_mv = false;
		}else{
			rmv_no_uniform_rgn[u8_i] = RMV_UNIFORM;
			uniform_rmv_x_sum += s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i];
			uniform_rmv_y_sum += s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i];
			uniform_rmv_num++;
		}
	}
	rSAD_nouniform_avg = (rSAD_nouniform_num==0) ? 0 : rSAD_sum/rSAD_nouniform_num;
	uniform_rmv_x_avg = (uniform_rmv_num==0) ? 0 : uniform_rmv_x_sum/uniform_rmv_num;
	uniform_rmv_y_avg = (uniform_rmv_num==0) ? 0 : uniform_rmv_y_sum/uniform_rmv_num;
	uniform_rmv_avg = _ABS_(uniform_rmv_x_avg)+_ABS_(uniform_rmv_y_avg); 

	// save static sad and calculate the average sad with no uniform rmv region
	for(u8_i=0; u8_i<STATICS_SAD_NUM-1; u8_i++){
		rSAD_nouniform_static[u8_i] = rSAD_nouniform_static[u8_i+1];
	}
	rSAD_nouniform_static[STATICS_SAD_NUM-1] = rSAD_nouniform_avg;
	for(u8_i=0; u8_i<STATICS_SAD_NUM; u8_i++){
		rSAD_nouniform_static_sum += rSAD_nouniform_static[u8_i];
	}
	rSAD_nouniform_static_avg = rSAD_nouniform_static_sum/STATICS_SAD_NUM;

	// check whether the rmv is uniform or not
	for(u8_i=start_id; u8_i<=end_id; u8_i++){
		if(rmv_no_uniform_rgn[u8_i]==RMV_NO_UNIFORM){
			if(u8_i%8!=0 && rmv_no_uniform_rgn[u8_i-1]!=RMV_UNIFORM){
				rmv_no_uniform_rgn[u8_i] = RMV_NO_UNIFORM_CONNECT;
				rmv_no_uniform_rgn[u8_i-1] = RMV_NO_UNIFORM_CONNECT;
			}else if(u8_i%8!=7 && rmv_no_uniform_rgn[u8_i+1]!=RMV_UNIFORM){
				rmv_no_uniform_rgn[u8_i] = RMV_NO_UNIFORM_CONNECT;
				rmv_no_uniform_rgn[u8_i+1] = RMV_NO_UNIFORM_CONNECT;
			}else if(u8_i<=end_id-8 && rmv_no_uniform_rgn[u8_i+8]!=RMV_UNIFORM){
				rmv_no_uniform_rgn[u8_i] = RMV_NO_UNIFORM_CONNECT;
				rmv_no_uniform_rgn[u8_i+8] = RMV_NO_UNIFORM_CONNECT;
			}else if(u8_i>=start_id+8 && rmv_no_uniform_rgn[u8_i-8]!=RMV_UNIFORM){
				rmv_no_uniform_rgn[u8_i] = RMV_NO_UNIFORM_CONNECT;
				rmv_no_uniform_rgn[u8_i-8] = RMV_NO_UNIFORM_CONNECT;
			}
		} 
	}

	uniform_rmv_num = 0;
	for(u8_i=start_id; u8_i<=end_id; u8_i++){
		// check whether the rmv is unifrom or not
		if(rmv_no_uniform_rgn[u8_i] == RMV_NO_UNIFORM){
			u1_all_rgn_has_group = false;
		}

		// get the sum of diff mv of uniform region mv and average rmv
		if(rmv_no_uniform_rgn[u8_i]==RMV_UNIFORM){
			uniform_rmv_num++;
			uniform_rmv_diff_sum += _ABS_(uniform_rmv_x_avg-s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i]) + 
									_ABS_(uniform_rmv_y_avg-s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i]);
		}
		// get the sum of diff mv of no-uniform region mv and average rmv
		else {//rmv_no_uniform_rgn[u8_i]!=RMV_UNIFORM
			nouniform_rmv_num++;
			uniform_nouniform_diff_sum += _ABS_(uniform_rmv_x_avg-s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i]) + 
											_ABS_(uniform_rmv_y_avg-s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i]);
		}

		// get the number of fast rmv region
		if(_ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i]) > fast_rmv_th ||
			_ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i]) > fast_rmv_th){
			fast_rgn_num++;
		}
	}
	// get the diff mv of uniform region mv and average rmv
	uniform_rmv_diff_avg = (uniform_rmv_num==0) ? 0 : uniform_rmv_diff_sum/uniform_rmv_num;
	// get the diff mv of no-uniform region mv and average rmv
	uniform_nouniform_diff_avg = (nouniform_rmv_num==0) ? 0 : uniform_nouniform_diff_sum/nouniform_rmv_num;

	// match when there is two group rmv
	rgn_uniform_match = (u1_only_one_group_mv==false && u1_all_rgn_has_group) ? true : false;
	// match when fast rmv region number > threshold
	fast_rmv_match = (fast_rgn_num>=fast_rgn_num_th) ? true : false;
	// match when no-uniform region sad > threshold
	sad_match = (rSAD_nouniform_static_avg>rSAD_th) ? true : false;
	// match when diff of uniform rmv is small and diff of uniform and no-uniform rmv is large
	rmv_diff_match = (uniform_rmv_diff_avg*gain_diff_avg <= uniform_rmv_avg && uniform_nouniform_diff_avg > uniform_diff_th) ? true : false;
	// match when detail is small for pattern #72
	dtl_match = (aDTL < 1500000) ? true : false; //don't apply FB in #045
	
	if((pOutput->u8_after_sc_cnt < sc_cnt_th || pOutput->u8_FastUniformBaground_cnt>0 || pOutput->u8_after_FastUniformBaground_cnt<after_cnt_th) && 
		rgn_uniform_match && fast_rmv_match && sad_match && rmv_diff_match && dtl_match){
		pOutput->u8_FastUniformBaground_cnt = (pOutput->u8_FastUniformBaground_cnt>=38) ? 38 : pOutput->u8_FastUniformBaground_cnt+1;
		if(show_cross){
			WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 13, 13, 1);
			WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 15, 15, 1);
			WriteRegister(KPOST_TOP_KPOST_TOP_00_reg,  0, 12, 1920);
			WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 16, 28, 1080);
		}
	}else{
		pOutput->u8_FastUniformBaground_cnt = (pOutput->u8_FastUniformBaground_cnt<=8) ? 0 : pOutput->u8_FastUniformBaground_cnt-1;
		if(show_cross){
			WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 13, 13, 0);
			WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 15, 15, 0);
		}
	}

	if(pOutput->u8_FastUniformBaground_cnt>=8){
		pOutput->u8_after_FastUniformBaground_cnt = 0;
		pOutput->u27_TempConsis_Th_h = (pOutput->u27_TempConsis_Th_h * TempConsis_gain)>>8;
		pOutput->u27_TempConsis_Th_l = (pOutput->u27_TempConsis_Th_l * TempConsis_gain)>>8;
	}else{
		pOutput->u8_after_FastUniformBaground_cnt = (pOutput->u8_after_FastUniformBaground_cnt==255) ? 255 : pOutput->u8_after_FastUniformBaground_cnt+1;
	}
		
	if(log_en){//d940[30]
		rtd_pr_memc_emerg("[%s] flg,%d,%d,%d,%d,%d,  cnt,%d,  sc,%d,%d, fast_rgn,%d,%d,  group,%d,%d, uniform,%d,%d,%d, dtl,%u, sad,%u,%u, id,%d,%d,\n", __FUNCTION__, 
			fast_rmv_match, rgn_uniform_match, sad_match, rmv_diff_match, dtl_match, pOutput->u8_FastUniformBaground_cnt, 
			pOutput->u8_after_sc_cnt, pOutput->u8_after_FastUniformBaground_cnt, fast_rgn_num, fast_rgn_num_th, u1_only_one_group_mv, u1_all_rgn_has_group, 
			uniform_rmv_diff_avg, uniform_rmv_avg, uniform_nouniform_diff_avg, aDTL, rSAD_nouniform_avg, rSAD_nouniform_static_avg, start_id, end_id);
	}
}


VOID FBTCCurve_DTL(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	const unsigned char shif_bit_cal = 7;
	unsigned int algo_en=0, log_en=0, dtl_th=0, dtl_diff=0, TempConsis_gain=0;
	unsigned int aDTL = s_pContext->_output_read_comreg.u25_me_aDTL_rb;

	ReadRegister(SOFTWARE3_SOFTWARE3_16_reg, 15, 15, &algo_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_16_reg, 14, 14, &log_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_16_reg, 16, 23, &dtl_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_16_reg,  0,  9, &TempConsis_gain);
	dtl_th *= 10000;

	if(!algo_en){
		return;
	}

	if(aDTL>dtl_th){
		pOutput->u8_large_DTL_cnt = (pOutput->u8_large_DTL_cnt>=15) ? 15 : pOutput->u8_large_DTL_cnt+1;
	}else {
		pOutput->u8_large_DTL_cnt = (pOutput->u8_large_DTL_cnt<=8) ? 0 : pOutput->u8_large_DTL_cnt-1;
	}

	if(pOutput->u8_large_DTL_cnt>=8){
		// calculate the gain for temp consis
		// offset: 500000 -> TempConsis_gain:230*500000/1000000 + 128 = 243
		dtl_diff = (aDTL>dtl_th) ? aDTL-dtl_th : 0;
		TempConsis_gain = TempConsis_gain*dtl_diff/1000000 + (1<<shif_bit_cal);
		// adjust the temp consis threshold
		pOutput->u27_TempConsis_Th_h = (pOutput->u27_TempConsis_Th_h * TempConsis_gain)>>shif_bit_cal;
		pOutput->u27_TempConsis_Th_l = (pOutput->u27_TempConsis_Th_l * TempConsis_gain)>>shif_bit_cal;
	}

	if(log_en){//d940[30]
		rtd_pr_memc_emerg("[%s] FB_lvl,%d, cnt,%d, DTL,%u, gain,%u\n",__FUNCTION__, 
			s_pContext->_output_wrt_comreg.u8_FB_lvl, pOutput->u8_large_DTL_cnt, aDTL, TempConsis_gain);
	}
}

VOID FBTCCurve_SC(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int u1_sc_status = s_pContext->_output_read_comreg.u1_sc_status_rb;//pOutput->u1_SW_sc_true;
	unsigned int u32_RB_val, log_en;
	unsigned char u8_case_id = 0;
	ReadRegister(HARDWARE_HARDWARE_58_reg, 17, 17, &u32_RB_val);//bit 17
	ReadRegister(SOFTWARE3_SOFTWARE3_50_reg, 10, 10, &log_en);

	pOutput->u8_after_sc_cnt = (pOutput->u8_after_sc_cnt==255) ? 255 : pOutput->u8_after_sc_cnt+1;

	if (u1_sc_status == 1)
	{
		u8_case_id = 1;
		pOutput->u27_SC_TC_Th_h = (pParam->u27_Fb_tc_th_h * (pParam->u8_SC_Gain)) >> (pParam->u8_SC_slope);
		pOutput->u27_SC_TC_Th_l = (pParam->u27_Fb_tc_th_l * (pParam->u8_SC_Gain)) >> (pParam->u8_SC_slope);

		pOutput->u8_SCHoldCnt = pParam->u8_SC_HoldCnt;
		pOutput->u8_SCNormCnt = 0;
		pOutput->u8_FastUniformBaground_cnt = 0;
		pOutput->u8_after_sc_cnt = 0;
	}
	else if (s_pContext->_output_fblevelctrl.u8_SCHoldCnt != 0)
	{
		u8_case_id = 2;
		pOutput->u8_SCHoldCnt = (s_pContext->_output_fblevelctrl.u8_SCHoldCnt > 1) ? (s_pContext->_output_fblevelctrl.u8_SCHoldCnt-1) : 0;

		pOutput->u27_FBTempConsisPre = 0;
		pOutput->u20_FBPreResult     = 0;
	}
	else if (s_pContext->_output_fblevelctrl.u8_SCNormCnt < pParam->u8_SC_NormCnt)
	{
		unsigned int fb_sc_gain_Norm;
		u8_case_id = 3;
		pOutput->u8_SCNormCnt = pOutput->u8_SCNormCnt + 1;
		fb_sc_gain_Norm       = pParam->u8_SC_Gain * (pParam->u8_SC_NormCnt - pOutput->u8_SCNormCnt) + (1 << pParam->u8_SC_slope) * pOutput->u8_SCNormCnt;

		if(pOutput->u8_SCNormCnt == 0){
			u8_case_id = 4;
			rtd_pr_memc_notice( "[%s]SCNormalCnt is zero!!\n",__FUNCTION__);
			pOutput->u8_SCNormCnt = 1;
		}
		pOutput->u27_SC_TC_Th_h = (Multiply_2Val_protect(pParam->u27_Fb_tc_th_h, fb_sc_gain_Norm, 14) / pParam->u8_SC_NormCnt) >> pParam->u8_SC_slope;
		pOutput->u27_SC_TC_Th_l = (Multiply_2Val_protect(pParam->u27_Fb_tc_th_l, fb_sc_gain_Norm, 14) / pParam->u8_SC_NormCnt) >> pParam->u8_SC_slope;

		//LogPrintf(DBG_MSG, "fb_sc_norm_gain = %d, \r\n", fb_sc_gain_Norm);
		//LogPrintf(DBG_MSG, "sc_th_H = %d, sc_th_L = %d\r\n", pOutput->u27_SC_TC_Th_h, pOutput->u27_SC_TC_Th_l);
	}
	else
	{
		u8_case_id = 5;
		pOutput->u27_SC_TC_Th_h =  pParam->u27_Fb_tc_th_h;
		pOutput->u27_SC_TC_Th_l =  pParam->u27_Fb_tc_th_l;
	}

	pOutput->u27_TempConsis_Th_h = pOutput->u27_SC_TC_Th_h;
	pOutput->u27_TempConsis_Th_l = pOutput->u27_SC_TC_Th_l;
	//LogPrintf(DBG_MSG,"sc_status = %d\n", u1_sc_status);

	if(log_en){
		rtd_pr_memc_emerg( "[%s] Case,%d,  Th_H,%d,  Th_L,%d,  cnt,%d,%d\n",__FUNCTION__,u8_case_id, 
			pOutput->u27_TempConsis_Th_h, pOutput->u27_TempConsis_Th_l, pOutput->u8_SCHoldCnt, pOutput->u8_SCNormCnt);
	}

	#if Debug_log
	if(u32_RB_val==1){
		rtd_pr_memc_notice( "[%s]sc_status=%d[%d,%d]!!\n",__FUNCTION__,u1_sc_status,pOutput->u8_SCHoldCnt,pOutput->u8_SCNormCnt);
		rtd_pr_memc_notice( "[%s](Th_h,Th_l)=(%d,%d)\n",__FUNCTION__,pOutput->u27_TempConsis_Th_h,pOutput->u27_TempConsis_Th_l);
	}
	//WriteRegister(SOFTWARE1_SOFTWARE1_21_reg, 0, 31, pOutput->u27_TempConsis_Th_h);
	WriteRegister(SOFTWARE1_SOFTWARE1_22_reg, 0, 31, pOutput->u27_TempConsis_Th_l );
	#endif
}

VOID FBTCCurve_TH_specialScene(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	//unsigned int u32_RB_val;

	//static unsigned char cnt = 0;

	//WriteRegister(KPOST_TOP_KPOST_TOP_A0_reg, 12,15, s_pContext->_output_me_sceneAnalysis.u1_fb_special_chaos_true&&s_pContext->_output_me_sceneAnalysis.u1_chaos_true);

	unsigned int u32_RB_val, log_en;
	unsigned char u8_case_id = 0;
	ReadRegister(HARDWARE_HARDWARE_58_reg, 17, 17, &u32_RB_val);//bit 17
	ReadRegister(SOFTWARE3_SOFTWARE3_50_reg, 11, 11, &log_en);

	if( s_pContext->_output_me_sceneAnalysis.u8_chaos_apply != 0 || s_pContext->_output_frc_sceneAnalysis.u1_RgRotat_true ||s_pContext->_output_wrt_comreg.u1_GFB_specialScene_true)
	{
		u8_case_id += _BIT0;
		pOutput->u27_TempConsis_Th_h = 0x10000;
		pOutput->u27_TempConsis_Th_l = 0x8000;
	}
	if( s_pContext->_output_wrt_comreg.u1_GFB_specialScene_true)
	{	
		u8_case_id += _BIT1;
		pOutput->u27_TempConsis_Th_h = 0x4000;
		pOutput->u27_TempConsis_Th_l = 0x2000;
	}
	if(s_pContext->_output_wrt_comreg.u1_GFB_Special_apply)
	{	
		u8_case_id += _BIT2;
		pOutput->u27_TempConsis_Th_h = 0x8df88;
		pOutput->u27_TempConsis_Th_l = 0x6df88;
	}
	
#if 0
	// GMV
	unsigned int u32_RB_val;
	ReadRegister(0x000040A8, 25, 25, &u32_RB_val);
	if(u32_RB_val)
	{
		rtd_pr_memc_debug("Ph=%d: (%d,%d,%d,%d),(%d,%d,%d,%d)\n",
			s_pContext->_output_filmDetectctrl.u8_phase_Idx[_FILM_ALL],
			s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb, s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb, s_pContext->_output_read_comreg.u6_me_GMV_1st_cnt_rb, s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb,
			s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb, s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb, s_pContext->_output_read_comreg.u6_me_GMV_2nd_cnt_rb, s_pContext->_output_read_comreg.u12_me_GMV_2nd_unconf_rb);

	// 32 regions
		rtd_pr_memc_debug("[8-11]:(%d,%d,%d,%d),(%d,%d,%d,%d);(%d,%d,%d,%d),(%d,%d,%d,%d);(%d,%d,%d,%d),(%d,%d,%d,%d);(%d,%d,%d,%d),(%d,%d,%d,%d)\n",
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[8],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[8], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[8], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[8],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[8],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[8], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[8], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[8],
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[9],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[9], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[9], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[9],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[9],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[9], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[9], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[9],
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[10],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[10], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[10], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[10],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[10],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[10], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[10], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[10],
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[11],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[11], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[11], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[11],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[11],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[11], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[11], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[11]);
		rtd_pr_memc_debug("[12-15]:(%d,%d,%d,%d),(%d,%d,%d,%d);(%d,%d,%d,%d),(%d,%d,%d,%d);(%d,%d,%d,%d),(%d,%d,%d,%d);(%d,%d,%d,%d),(%d,%d,%d,%d)\n",
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[12],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[12], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[12], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[12],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[12],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[12], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[12], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[12],
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[13],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[13], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[13], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[13],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[13],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[13], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[13], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[13],
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[14],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[14], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[14], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[14],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[14],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[14], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[14], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[14],
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[15],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[15], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[15], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[15],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[15],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[15], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[15], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[15]);
		rtd_pr_memc_debug("[16-19]:(%d,%d,%d,%d),(%d,%d,%d,%d);(%d,%d,%d,%d),(%d,%d,%d,%d);(%d,%d,%d,%d),(%d,%d,%d,%d);(%d,%d,%d,%d),(%d,%d,%d,%d)\n",
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[16],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[16], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[16], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[16],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[16],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[16], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[16], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[16],
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[17],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[17], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[17], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[17],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[17],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[17], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[17], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[17],
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[18],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[18], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[18], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[18],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[18],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[18], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[18], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[18],
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[19],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[19], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[19], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[19],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[19],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[19], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[19], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[19]);
		rtd_pr_memc_debug("[20-23]:(%d,%d,%d,%d),(%d,%d,%d,%d);(%d,%d,%d,%d),(%d,%d,%d,%d);(%d,%d,%d,%d),(%d,%d,%d,%d);(%d,%d,%d,%d),(%d,%d,%d,%d)\n",
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[8],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[20], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[20], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[20],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[8],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[20], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[20], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[20],
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[9],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[21], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[21], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[21],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[9],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[21], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[21], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[21],
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[10],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[22], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[22], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[22],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[10],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[22], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[22], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[22],
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[11],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[23], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[23], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[23],
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[11],s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[23], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[23], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[23]);
	}
#endif

	/*if(cnt>30)
	{
		cnt = 0;

		WriteRegister(KPOST_TOP_KPOST_TOP_A4_reg, 0, 15, (pOutput->u27_TempConsis_Th_h>>8));
		WriteRegister(KPOST_TOP_KPOST_TOP_A4_reg, 16, 31, (pOutput->u27_TempConsis_Th_l>>8));

	}
	else
	{
		cnt++;
	}*/

	if(log_en){
		rtd_pr_memc_emerg( "[%s]  case,%d,  Th_H,%d,  Th_L,%d,  cond,%d,%d,%d\n",__FUNCTION__, u8_case_id, 
			pOutput->u27_TempConsis_Th_h, pOutput->u27_TempConsis_Th_l, s_pContext->_output_me_sceneAnalysis.u8_chaos_apply, s_pContext->_output_frc_sceneAnalysis.u1_RgRotat_true, s_pContext->_output_wrt_comreg.u1_GFB_specialScene_true);
	}
	
	#if Debug_log
	if(u32_RB_val){
		rtd_pr_memc_notice( "[%s][%d,%d,%d]!!\n",__FUNCTION__,s_pContext->_output_me_sceneAnalysis.u8_chaos_apply,s_pContext->_output_frc_sceneAnalysis.u1_RgRotat_true,s_pContext->_output_wrt_comreg.u1_GFB_specialScene_true);
		rtd_pr_memc_notice( "[%s](Th_h,Th_l)=(%d,%d)\n",__FUNCTION__,pOutput->u27_TempConsis_Th_h,pOutput->u27_TempConsis_Th_l);
	}
	WriteRegister(SOFTWARE1_SOFTWARE1_23_reg, 0, 31, pOutput->u27_TempConsis_Th_h);
	WriteRegister(SOFTWARE1_SOFTWARE1_24_reg, 0, 31, pOutput->u27_TempConsis_Th_l );
	#endif

}

VOID FBTCCurve_ME(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	PQL_OUTPUT_RESOLUTION output_Resolution = s_pContext->_external_data._output_resolution;
	unsigned char u8_TopRim_th = Convert_UHD_Param_To_Current_RES(16, PARAM_DIRECTION_Y);//4*MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.u8_scale_V; // 4k: u8_TopRim_th=16
	unsigned char u8_LftRim_th = Convert_UHD_Param_To_Current_RES(24, PARAM_DIRECTION_X);//6*MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.u8_scale_H; // 4k: u8_LftRim_th=24
	unsigned short u12_tmp_v = MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.s16_resolution_height>>2; // 4k: u12_tmp_v=540
	unsigned short u12_tmp_h = MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.s16_resolution_width>>3; // 4k: u12_tmp_h=480

	unsigned char  i, u8_bad_region_cnt_step, u8_BadRegion_cnt = 0;
	unsigned int u16_BadRegion_Gain;

	unsigned int u8_gain, u8_shift;
	unsigned long long u64_temp_h = 0, u64_temp_l = 0;
#ifndef CONFIG_ARM64
	unsigned long long tmp_u64_temp_h = 0, tmp_u64_temp_l = 0;
#endif
	#if IC_K5LP
	unsigned int u32_invalidMV=0x0;
	#else
	unsigned int u32_invalidMV=0x0;
	#endif
	#if (IC_K24)
	// set higher sad th for repeat pattern
	unsigned int repeat_cnt1 = 0, repeat_cnt2 = 0, repeat_cnt3 = 0, repeat_en = 0, repeat_gain = 0, u1_algo_en = 0, log_repeat = 0;
	unsigned int max_cnt = 0, force_en = 0, force_val = 0;
	const unsigned int repeat_total_num = Convert_UHD_Param_To_Current_RES(1024*32, PARAM_DIRECTION_AREA);
	#endif

	unsigned int u32_RB_val, log_en;
	ReadRegister(HARDWARE_HARDWARE_58_reg, 17, 17, &u32_RB_val);//bit 17
	ReadRegister(SOFTWARE3_SOFTWARE3_50_reg, 13, 13, &log_en);
	
	ReadRegister(SOFTWARE2_SOFTWARE2_19_reg, 0, 7, &u8_gain);
	ReadRegister(SOFTWARE2_SOFTWARE2_19_reg, 8,15, &u8_shift);


	// M8_repeat_condition
	#if (IC_K24)
	ReadRegister(KME_ALGO2_me1_freq_ctrl3_reg, KME_ALGO2_me1_freq_ctrl3_me1_freq_flow_total_en_shift, KME_ALGO2_me1_freq_ctrl3_me1_freq_flow_total_en_shift, &repeat_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_57_reg, 0, 0, &u1_algo_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_57_reg, 1, 1, &force_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_57_reg, 2, 8, &force_val);
	ReadRegister(SOFTWARE3_SOFTWARE3_57_reg, 9, 9, &log_repeat);
	repeat_cnt1 = s_pContext->_output_periodic_manage.u15_sum_freq_statistic_cnt1 *1000 / repeat_total_num;
	repeat_cnt2 = s_pContext->_output_periodic_manage.u15_sum_freq_statistic_cnt2 *1000 / repeat_total_num;
	repeat_cnt3 = s_pContext->_output_periodic_manage.u15_sum_freq_statistic_cnt3 *1000 / repeat_total_num;
	max_cnt = _MAX_(repeat_cnt1, _MAX_(repeat_cnt2, repeat_cnt3));

	// M8_repeat_condition
	if(repeat_en == 0 || u1_algo_en == 0){
		repeat_gain = 10;
	}
	else{
		if(max_cnt >= 700){ // 70%
			repeat_gain = 40;
		}
		else if(max_cnt >= 500){ // 50%
			repeat_gain = 30;
		}
		else if(max_cnt >= 300){ // 30%
			repeat_gain = 20;
		}
		else{
			repeat_gain = 10; // (max_cnt/100) +1;
		}
	}
	if(force_en){//d9e4[1]
		repeat_gain = force_val;//d9e4[2:6]
	}

	if(log_repeat){//d9e4[9]
		rtd_pr_memc_emerg("[%s][%d][cnt1,%d,][cnt2,%d,][cnt3,%d,][max_cnt,%d,][repeat_gain,%d,]\n\r", __FUNCTION__, __LINE__,
			repeat_cnt1, repeat_cnt2, repeat_cnt3, max_cnt, repeat_gain);
	}
	#endif//(IC_K24)

	for (i = 0; i < 32; i ++)
	{
		unsigned char  u8_rgnSad_iirA = 0;
		unsigned int u25_rgnSad_rb = s_pContext->_output_read_comreg.u25_me_rSAD_rb[i];
		unsigned int u20_rgnDtl_rb = s_pContext->_output_read_comreg.u20_me_rDTL_rb[i];
		unsigned char  u8_dtl_gain = 0xff;
		unsigned int u16_badRgn_dtl_thr1 = pParam->u8_badRgn_dtl_thr1 << 10;
		unsigned int u16_badRgn_dtl_thr2 = pParam->u8_badRgn_dtl_thr2 << 10;
		unsigned int ui_rng_SAD_th_h = 0, ui_rng_SAD_th_m = 0, ui_rng_SAD_th_l = 0;

		if(u16_badRgn_dtl_thr2 <= u16_badRgn_dtl_thr1)
		{
			u16_badRgn_dtl_thr2 = u16_badRgn_dtl_thr1 + 1;
		}
		if(u20_rgnDtl_rb < u16_badRgn_dtl_thr1)
		{
			u8_dtl_gain = pParam->u8_badRgn_dtl_gain2;
		}
		else if(u20_rgnDtl_rb > u16_badRgn_dtl_thr2)
		{
			u8_dtl_gain = pParam->u8_badRgn_dtl_gain1;
		}
		else
		{
			u8_dtl_gain = pParam->u8_badRgn_dtl_gain1 + (pParam->u8_badRgn_dtl_gain2 - pParam->u8_badRgn_dtl_gain1) *
				(u20_rgnDtl_rb - u16_badRgn_dtl_thr1) / (u16_badRgn_dtl_thr2 - u16_badRgn_dtl_thr1);
		}


		if(s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP] >= u8_TopRim_th)
		{
			if((i < 8) || (i >= 24 && i < 32))
			{
				ui_rng_SAD_th_h = (u12_tmp_v - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP]) * pParam->u23_BadRegionSAD_th_h / u12_tmp_v;
				ui_rng_SAD_th_m = (u12_tmp_v - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP]) * pParam->u23_BadRegionSAD_th_m / u12_tmp_v;
				ui_rng_SAD_th_l = (u12_tmp_v - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP]) * pParam->u23_BadRegionSAD_th / u12_tmp_v;
				u25_rgnSad_rb =u25_rgnSad_rb-u32_invalidMV;
			}
			else
			{
				ui_rng_SAD_th_h = pParam->u23_BadRegionSAD_th_h;
				ui_rng_SAD_th_m = pParam->u23_BadRegionSAD_th_m;
				ui_rng_SAD_th_l = pParam->u23_BadRegionSAD_th;
			}
		}
		else if(s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_LFT] >= u8_LftRim_th)
		{
			if((i == 0) || (i == 8) || (i == 16) || (i == 24) || (i == 7) || (i == 15) || (i == 23) || (i == 31))
			{
			ui_rng_SAD_th_h = (u12_tmp_h - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_LFT]) * pParam->u23_BadRegionSAD_th_h / u12_tmp_h;
			ui_rng_SAD_th_m = (u12_tmp_h - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_LFT]) * pParam->u23_BadRegionSAD_th_m / u12_tmp_h;
			ui_rng_SAD_th_l = (u12_tmp_h - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_LFT]) * pParam->u23_BadRegionSAD_th / u12_tmp_h;
			}
			else
			{
				ui_rng_SAD_th_h = pParam->u23_BadRegionSAD_th_h;
				ui_rng_SAD_th_m = pParam->u23_BadRegionSAD_th_m;
				ui_rng_SAD_th_l = pParam->u23_BadRegionSAD_th;
			}
			if((i < 8) || (i >= 24 && i < 32))
				u25_rgnSad_rb =u25_rgnSad_rb-u32_invalidMV;
		}
		else
		{
			ui_rng_SAD_th_h = pParam->u23_BadRegionSAD_th_h;
			ui_rng_SAD_th_m = pParam->u23_BadRegionSAD_th_m;
			ui_rng_SAD_th_l = pParam->u23_BadRegionSAD_th;
		}

		#if (IC_K24)
		ui_rng_SAD_th_h = ui_rng_SAD_th_h*repeat_gain/10;
		ui_rng_SAD_th_m = ui_rng_SAD_th_m*repeat_gain/10;
		ui_rng_SAD_th_l = ui_rng_SAD_th_l*repeat_gain/10;
		#endif
		
		u25_rgnSad_rb = (u25_rgnSad_rb * u8_dtl_gain) >> 8;

		u8_rgnSad_iirA = (pOutput->u1_badRgn_rgnSAD_noIIR == 1 || s_pContext->_output_read_comreg.u1_sc_status_rb == 1)? 0 : \
			                    ((u25_rgnSad_rb >= pOutput->u25_badRgn_iirRgnSad[i])? pParam->u8_badRgn_sad_iirA_up : pParam->u8_badRgn_sad_iirA_down);



		pOutput->u25_badRgn_iirRgnSad[i] = _2Val_RndBlending_(pOutput->u25_badRgn_iirRgnSad[i], u25_rgnSad_rb, u8_rgnSad_iirA, 8);
		u8_bad_region_cnt_step = (pOutput->u25_badRgn_iirRgnSad[i] > ui_rng_SAD_th_h)? 3 : \
	                             (pOutput->u25_badRgn_iirRgnSad[i] > ui_rng_SAD_th_m)? 2 : \
							     (pOutput->u25_badRgn_iirRgnSad[i] > ui_rng_SAD_th_l)? 1 : 0;

		if ((((i >= 10) && (i <= 13)) || ((i >= 18) && (i <= 21))) && (u8_bad_region_cnt_step != 0))
		{
			u8_bad_region_cnt_step = u8_bad_region_cnt_step + ((u8_bad_region_cnt_step * pParam->u8_badRgn_center_gain) >>4) + pParam->u8_badRgn_center_oft;
		}

		u8_BadRegion_cnt = _MIN_(u8_BadRegion_cnt + u8_bad_region_cnt_step, 255);
		
	}
	
	pOutput->u8_BadRegion_cnt = u8_BadRegion_cnt;

#if Debug_log
	if(u32_RB_val){
		rtd_pr_memc_notice( "[%s]sad=[\n",__FUNCTION__);
		for (i = 0; i < 32; i ++)
		{
			rtd_pr_memc_notice( "%d,",s_pContext->_output_read_comreg.u25_me_rSAD_rb[i]);
			if((i+1)%8==0){
				rtd_pr_memc_notice( "]\n");
			}
		}
		
		rtd_pr_memc_notice( "[%s]dtl=[\n",__FUNCTION__);
		for (i = 0; i < 32; i ++)
		{
			rtd_pr_memc_notice( "%d,",s_pContext->_output_read_comreg.u20_me_rDTL_rb[i]);
			if((i+1)%8==0){
				rtd_pr_memc_notice( "]\n");
			}
		}
	}
#endif

	// calculate gain.
	pOutput->u6_BadRegionCnt  = _CLIP_(u8_BadRegion_cnt, 0, 31);
	if(u8_BadRegion_cnt > 31)
		u8_BadRegion_cnt = 31;
	u16_BadRegion_Gain        = (pParam->u5_BadRegionGain[u8_BadRegion_cnt] << 4);

//	LogPrintf(DBG_MSG, "fb badRgn_gain = %d\r\n", u16_BadRegion_Gain);
	u16_BadRegion_Gain        = _2Val_RndBlending_(pOutput->u9_BadRegionGainPre, u16_BadRegion_Gain, pParam->u8_BadRegionGainIIR, 8);

	pOutput->u9_BadRegionGainPre = u16_BadRegion_Gain; // u5.4
	pOutput->u5_BadRegionGain    = u16_BadRegion_Gain >> 4;

	pOutput->u5_BadRegionGain    = (pParam->u1_BadRegionCtrl_en == 0)? 16 : _CLIP_UBOUND_(pOutput->u5_BadRegionGain, 16);
#if 1

	pOutput->u27_TempConsis_Th_h = (pOutput->u27_TempConsis_Th_h * pOutput->u5_BadRegionGain + (1<<3)) >> 4;
	pOutput->u27_TempConsis_Th_l = (pOutput->u27_TempConsis_Th_l * pOutput->u5_BadRegionGain + (1<<3)) >> 4;

	u64_temp_h = ((unsigned long long)u8_gain)*pOutput->u27_TempConsis_Th_h;
	u64_temp_l = ((unsigned long long)u8_gain)*pOutput->u27_TempConsis_Th_l;

#ifdef CONFIG_ARM64
	u64_temp_h = u64_temp_h/u8_shift;
	u64_temp_l = u64_temp_l/u8_shift;
#else
	tmp_u64_temp_h = u64_temp_h;
	tmp_u64_temp_l = u64_temp_l;
	do_div(tmp_u64_temp_h, u8_shift);
	do_div(tmp_u64_temp_l, u8_shift);
	u64_temp_h = tmp_u64_temp_h;
	u64_temp_l = tmp_u64_temp_l;
#endif
		
		pOutput->u27_TempConsis_Th_h = _MIN_(u64_temp_h, 0x7FFFFFF);
		pOutput->u27_TempConsis_Th_l = _MIN_(u64_temp_l, 0x7FFFFFF);
#else
		pOutput->u27_TempConsis_Th_h = (pOutput->u27_TempConsis_Th_h * pOutput->u5_BadRegionGain + (1<<3)) >> 4;
		pOutput->u27_TempConsis_Th_l = (pOutput->u27_TempConsis_Th_l * pOutput->u5_BadRegionGain + (1<<3)) >> 4;
#endif


	if(log_en){
		rtd_pr_memc_emerg("[%s]  badregion_cnt,%d,  th_H,%d,  th_L,%d,\n", __FUNCTION__, pOutput->u8_BadRegion_cnt, pOutput->u27_TempConsis_Th_h, pOutput->u27_TempConsis_Th_l);
	}
	
	#if Debug_log
	if(u32_RB_val){
		rtd_pr_memc_notice( "[%s][%d][%d][%d,%d]!!\n",__FUNCTION__,pOutput->u5_BadRegionGain,u16_BadRegion_Gain,u16_BadRegion_Gain,u8_BadRegion_cnt);
		rtd_pr_memc_notice( "[%s](Th_h,Th_l)=(%d,%d)\n",__FUNCTION__,pOutput->u27_TempConsis_Th_h,pOutput->u27_TempConsis_Th_l);
	}
	WriteRegister(SOFTWARE1_SOFTWARE1_27_reg, 0, 31, pOutput->u27_TempConsis_Th_h);
	WriteRegister(SOFTWARE1_SOFTWARE1_28_reg, 0, 31, pOutput->u27_TempConsis_Th_l );
	#endif
}

VOID FBTCCurve_GMV(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	PQL_OUTPUT_RESOLUTION output_Resolution = s_pContext->_external_data._output_resolution;
	unsigned char u8_TopRim_th = Convert_UHD_Param_To_Current_RES(16, PARAM_DIRECTION_Y);//4*MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.u8_scale_V; // 4k: u8_TopRim_th=16
	unsigned char u8_LftRim_th = Convert_UHD_Param_To_Current_RES(24, PARAM_DIRECTION_X);//6*MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.u8_scale_H; // 4k: u8_LftRim_th=24
	unsigned short u12_tmp_v = MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.s16_resolution_height>>2; // 4k: u12_tmp_v=540
	unsigned short u12_tmp_h = MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.s16_resolution_width>>3; // 4k: u12_tmp_h=480
	
	unsigned char  gmv_gain       = 0;
	unsigned int i;
	unsigned char u8_rmv_cnt = 0, u8_unconf_cnt = 0;
	unsigned char  gmv_ratio      = 128;
	unsigned int ui_rng_gmv_cnt_th = 0, ui_gmv_unconf_th = 0;

	unsigned int u32_RB_val, log_en;
	ReadRegister(HARDWARE_HARDWARE_58_reg, 17, 17, &u32_RB_val);//bit 17
	ReadRegister(SOFTWARE3_SOFTWARE3_50_reg, 14, 14, &log_en);

	for(i = 0; i < 32; i++)
	{
		if(s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP] >= u8_TopRim_th)	
		{
			if((i >= 0 && i < 8) || (i >= 24 && i < 32))
			{
				ui_rng_gmv_cnt_th = (u12_tmp_v - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP]) * pParam->u12_gmv_rng_cnt_th / u12_tmp_v;
				ui_gmv_unconf_th = (u12_tmp_v - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP]) * pParam->u12_gmv_unconf_cnt_th / u12_tmp_v;
			}
			else
			{
				ui_rng_gmv_cnt_th = pParam->u12_gmv_rng_cnt_th;
				ui_gmv_unconf_th = pParam->u12_gmv_unconf_cnt_th;
			}
		}
		else if(s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_LFT] >= u8_LftRim_th)	
		{
			if((i == 0) || (i == 8) || (i == 16) || (i == 24) || (i == 7) || (i == 15) || (i == 23) || (i == 31))
			{
				ui_rng_gmv_cnt_th = (u12_tmp_h - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_LFT]) * pParam->u12_gmv_rng_cnt_th / u12_tmp_h;
				ui_gmv_unconf_th = (u12_tmp_h - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_LFT]) * pParam->u12_gmv_unconf_cnt_th / u12_tmp_h;
			}
			else
			{
				ui_rng_gmv_cnt_th = pParam->u12_gmv_rng_cnt_th;
				ui_gmv_unconf_th = pParam->u12_gmv_unconf_cnt_th;
			}
		}
		else
		{
			ui_rng_gmv_cnt_th = pParam->u12_gmv_rng_cnt_th;
			ui_gmv_unconf_th = pParam->u12_gmv_unconf_cnt_th;
		}

		if(s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[i] > ui_rng_gmv_cnt_th)
		{
			u8_rmv_cnt++;
		}
		if(s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[i] < ui_gmv_unconf_th)
		{
			u8_unconf_cnt++;
		}
	}

	if (pParam->u1_gmvratio_use == 0)
	{
			gmv_ratio = _CLIP_(s_pContext->_output_frc_sceneAnalysis.s2m_12_cnt_iir*2, 0 , 127);
	}
	else
	{
			gmv_ratio = _CLIP_(u8_rmv_cnt * u8_unconf_cnt * 127 / 32 / 32, 0 , 127);
	}
	if (gmv_ratio <= pParam->u7_gmv_ratio_thL)
	{
		gmv_gain = pParam->u6_gmv_gain_min;
	}
	else if (gmv_ratio > pParam->u7_gmv_ratio_thH)
	{
		gmv_gain = pParam->u6_gmv_gain_max;
	}
	else
	{
		unsigned int slope = 0;

		if(pParam->u7_gmv_ratio_thH > pParam->u7_gmv_ratio_thL)
		{
			slope = ((pParam->u6_gmv_gain_max-pParam->u6_gmv_gain_min) << 8)/(pParam->u7_gmv_ratio_thH-pParam->u7_gmv_ratio_thL);
		}
		else
		{
			rtd_pr_memc_notice( "[%s]Invalid gmv_ratio_th(high:%d,low:%d)!\n",\
					__FUNCTION__,pParam->u7_gmv_ratio_thH,pParam->u7_gmv_ratio_thL);
			slope = ((pParam->u6_gmv_gain_max-pParam->u6_gmv_gain_min) << 8);
		}
		gmv_gain = pParam->u6_gmv_gain_min + (((gmv_ratio-pParam->u7_gmv_ratio_thL)* slope + (1<<7)) >> 8);
	}

	gmv_gain = _CLIP_(gmv_gain, pParam->u6_gmv_gain_min, pParam->u6_gmv_gain_max);

	pOutput->u4_gmv_gain = _2Val_RndBlending_(pOutput->u4_gmv_gain_pre, gmv_gain, pParam->u8_gmv_gain_iir, 8);
	pOutput->u4_gmv_gain = (pParam->u1_FbLvl_gmvCtrl_en == 0)? 16 : pOutput->u4_gmv_gain;

	pOutput->u4_gmv_gain_pre     = pOutput->u4_gmv_gain;
	pOutput->u27_TempConsis_Th_h = (pOutput->u27_TempConsis_Th_h * pOutput->u4_gmv_gain + (1<<3))>>4;
	pOutput->u27_TempConsis_Th_l = (pOutput->u27_TempConsis_Th_l * pOutput->u4_gmv_gain + (1<<3))>>4;

	if(log_en){
		rtd_pr_memc_emerg("[%s]  gmv_gain,%d,%d,%d,  gmv_ratio,%d,%d,%d,  th_H,%d,  th_L,%d,\n",__FUNCTION__, 
			gmv_gain, pParam->u6_gmv_gain_min, pParam->u6_gmv_gain_max, 
			gmv_ratio, pParam->u7_gmv_ratio_thL, pParam->u7_gmv_ratio_thH, 
			pOutput->u27_TempConsis_Th_h, pOutput->u27_TempConsis_Th_l);
	}

	#if Debug_log
	if(u32_RB_val){
		rtd_pr_memc_notice( "[%s][%d][%d][%d,%d]!!\n",__FUNCTION__,pOutput->u4_gmv_gain,gmv_ratio,u8_rmv_cnt,u8_unconf_cnt);
		rtd_pr_memc_notice( "[%s](Th_h,Th_l)=(%d,%d)\n",__FUNCTION__,pOutput->u27_TempConsis_Th_h,pOutput->u27_TempConsis_Th_l);
	}	
	WriteRegister(SOFTWARE1_SOFTWARE1_29_reg, 0, 31, pOutput->u27_TempConsis_Th_h);
	WriteRegister(SOFTWARE1_SOFTWARE1_30_reg, 0, 31, pOutput->u27_TempConsis_Th_l );
	#endif
}

VOID FBGood_Scene(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	PQL_OUTPUT_RESOLUTION output_Resolution = s_pContext->_external_data._output_resolution;
	unsigned char u8_TopRim_th = Convert_UHD_Param_To_Current_RES(16, PARAM_DIRECTION_Y);//4*MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.u8_scale_V; // 4k: u8_TopRim_th=16
	unsigned char u8_LftRim_th = Convert_UHD_Param_To_Current_RES(24, PARAM_DIRECTION_X);//6*MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.u8_scale_H; // 4k: u8_LftRim_th=24
	unsigned short u12_tmp_v = MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.s16_resolution_height>>2; // 4k: u12_tmp_v=540
	unsigned short u12_tmp_h = MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.s16_resolution_width>>3; // 4k: u12_tmp_h=480
	unsigned int i;
	unsigned char u8_GoodRegion_cnt = 0;
	unsigned int ui_Rng_SAD_th = 0, ui_gmv_unconf_th = 0;
	#if IC_K5LP
	unsigned int u32_invalidMV=0x0;
	#else
	unsigned int u32_invalidMV=0x0;
	#endif

	unsigned int u32_RB_val, log_en;
	#if 0 //for YE Test temp test
	static unsigned int u32_ID_Log_en=0;//,ES_Test_Value=0;
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 28,  28, &u32_ID_Log_en);//YE Test debug dummy register
	#endif
	
	ReadRegister(HARDWARE_HARDWARE_58_reg, 17, 17, &u32_RB_val);//bit 17
	ReadRegister(SOFTWARE3_SOFTWARE3_50_reg, 16, 16, &log_en);

	for(i = 0; i < 32; i++)
	{
		unsigned int u25_rgnSad_rb = s_pContext->_output_read_comreg.u25_me_rSAD_rb[i];
		unsigned int u12_rgn_unconf_rb = s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[i];
		if(s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP] >= u8_TopRim_th)		
		{
			if((i >= 0 && i < 8) || (i >= 24 && i < 32))
			{
				ui_Rng_SAD_th = (u12_tmp_v - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP]) * pParam->u20_good_rgn_sad_th / u12_tmp_v;
				ui_gmv_unconf_th = (u12_tmp_v - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP]) * pParam->u12_good_rgn_gmv_unconf_th / u12_tmp_v;		

				u25_rgnSad_rb =u25_rgnSad_rb-u32_invalidMV;
			}
			else
			{
				ui_Rng_SAD_th = pParam->u20_good_rgn_sad_th;
				ui_gmv_unconf_th = pParam->u12_good_rgn_gmv_unconf_th;
			}
		}
		else if(s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_LFT] >= u8_LftRim_th)
		{
			if((i == 0) || (i == 8) || (i == 16) || (i == 24) || (i == 7) || (i == 15) || (i == 23) || (i == 31))
			{
				ui_Rng_SAD_th = (u12_tmp_h - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_LFT]) * pParam->u20_good_rgn_sad_th / u12_tmp_h;
				ui_gmv_unconf_th = (u12_tmp_h - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_LFT]) * pParam->u12_good_rgn_gmv_unconf_th / u12_tmp_h;
				u25_rgnSad_rb =u25_rgnSad_rb-u32_invalidMV;
			}
			else
			{
				ui_Rng_SAD_th = pParam->u20_good_rgn_sad_th;
				ui_gmv_unconf_th = pParam->u12_good_rgn_gmv_unconf_th;
			}
		}
		else
		{
			ui_Rng_SAD_th = pParam->u20_good_rgn_sad_th;
			ui_gmv_unconf_th = pParam->u12_good_rgn_gmv_unconf_th;
		}

		if((u25_rgnSad_rb < ui_Rng_SAD_th) &&  (u12_rgn_unconf_rb < ui_gmv_unconf_th))
		{
			u8_GoodRegion_cnt++;
		}
		#if 1
		if(0)//(u32_ID_Log_en==1)
		{
		rtd_pr_memc_debug("[MEMC GSA] , Sad_rb=%d, unconf=%d, \n ",u25_rgnSad_rb,u12_rgn_unconf_rb);
		}
		#endif
	}
	pOutput->u8_good_rgn_cnt = u8_GoodRegion_cnt;
	if(u8_GoodRegion_cnt >= pParam->u8_good_rgn_num_th1)
	{
		pOutput->u8_good_scene_cnt++;
	}
	else
	{
		pOutput->u8_good_scene_cnt = 0;
	}

	if(pOutput->u8_good_scene_cnt > pParam->u4_good_scene_hold_cnt || s_pContext->_output_frc_sceneAnalysis.u1_RgRotat_true || s_pContext->_output_dh_close.u1_lbmc_switch_close||s_pContext->_output_me_sceneAnalysis.u2_panning_flag >= 1)
	{
		pOutput->u1_good_scene = 1;
	}
	else
	{
		pOutput->u1_good_scene = 0;
	}

	if(u8_GoodRegion_cnt <= pParam->u8_good_rgn_num_th2)
	{
		pOutput->u1_bad_scene = 1;
		pOutput->u8_good_scene_cnt = 0;
	}
	else
	{
		pOutput->u1_bad_scene = 0;
	}

	if(pOutput->u1_bad_scene)
	{
		pOutput->u8_fblvl_map_y2 = pParam->u8_FBLevelMapY2;
		pOutput->u12_fblvl_iir_up = pParam->u12_FBLevelIIRUp;
	}

	if(pOutput->u1_good_scene)
	{
		pOutput->u8_fblvl_map_y2 = pParam->u8_fblvl_map_y2_good;
		pOutput->u12_fblvl_iir_up = pParam->u12_fblvl_iir_up_good;
	}

	if(0)//(u32_ID_Log_en==1)
	{
		rtd_pr_memc_debug("[MEMC GSB] , GR_cnt=%d, \n  ",u8_GoodRegion_cnt);
	}

	if(log_en){
		rtd_pr_memc_emerg( "[%s] bad_scene,%d,  good_scene,%d,  u8_GoodRegion_cnt,%d,  u8_good_scene_cnt,%d,  th_H,%d,  th_L,%d,\n",__FUNCTION__, 
			pOutput->u1_bad_scene, pOutput->u1_good_scene, u8_GoodRegion_cnt, pOutput->u8_good_scene_cnt, 
			pOutput->u27_TempConsis_Th_h,pOutput->u27_TempConsis_Th_l);
	}

	#if Debug_log
	if(u32_RB_val){
		rtd_pr_memc_notice( "[%s]GoodRegion_cnt=%d!!\n",__FUNCTION__,u8_GoodRegion_cnt);
		rtd_pr_memc_notice( "[%s][%d][%d,%d]\n",__FUNCTION__,pOutput->u8_fblvl_map_y2,pOutput->u1_bad_scene,pOutput->u1_good_scene);		
	}
	
	WriteRegister(SOFTWARE1_SOFTWARE1_34_reg, 0, 31, pOutput->u12_fblvl_iir_up);
	WriteRegister(SOFTWARE1_SOFTWARE1_35_reg, 0, 31, ui_Rng_SAD_th );
	WriteRegister(SOFTWARE1_SOFTWARE1_36_reg, 0, 31, ui_gmv_unconf_th );
	WriteRegister(SOFTWARE1_SOFTWARE1_37_reg, 0, 31, pOutput->u8_good_rgn_cnt );
	WriteRegister(SOFTWARE1_SOFTWARE1_38_reg, 0, 31, pOutput->u8_good_scene_cnt );
	#endif
}

VOID FBPan_Scene(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	int u11_gmv_mvx = (s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb)>>2;
	int u10_gmv_mvy = (s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb)>>2;
	unsigned char u6_gmv_cnt = s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb;
	int s32_mvx_avg = 0, s32_mvy_avg = 0;
	unsigned char i = 0, panx_cnt = 0, pany_cnt = 0;
	unsigned char u1_gmv_mode;
	unsigned int u32_RB_val, u32_gmv_cnt_thl, log_en;

	ReadRegister(KME_ME1_TOP2_KME_ME1_TOP2_1C_reg, 2, 2, &u32_RB_val);
	ReadRegister(SOFTWARE3_SOFTWARE3_50_reg, 12, 12, &log_en);
	u1_gmv_mode = u32_RB_val;
	if(u1_gmv_mode ==0)
		u32_gmv_cnt_thl =25;
	else
		u32_gmv_cnt_thl = 15000;

	if(pParam->u1_FB_pan_apply_en)
	{

		if ((u6_gmv_cnt>u32_gmv_cnt_thl) && (_ABS_(u11_gmv_mvx)>3 || _ABS_(u10_gmv_mvy)>3))
		{
			for (i = 0; i < 32; i ++)
			{
				s32_mvx_avg += (s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i]>>2);
				s32_mvy_avg += (s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i]>>2);
			}
			s32_mvx_avg = (s32_mvx_avg>>5);
			s32_mvy_avg = (s32_mvy_avg>>5);

			for (i = 0; i < 32; i ++)
			{
				if(_ABS_(u11_gmv_mvx)>3 && _ABS_DIFF_(s32_mvx_avg,(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i]>>2))<3)
				{
					panx_cnt++;
				}

				if(_ABS_(u10_gmv_mvy)>3 && _ABS_DIFF_(s32_mvy_avg,(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i]>>2))<3)
				{
					pany_cnt++;
				}

			}
		}

		if(panx_cnt>28)
		{
			pOutput->u1_panx_scene = 1;
		}
		else
		{
			pOutput->u1_panx_scene = 0;
		}

		if(pany_cnt>28)
		{
			pOutput->u1_pany_scene = 1;
		}
		else
		{
			pOutput->u1_pany_scene = 0;
		}

		if((pOutput->u1_panx_scene == 1) || (pOutput->u1_pany_scene == 1) ||s_pContext->_output_me_sceneAnalysis.u2_panning_flag == 1)
		{
			pOutput->u8_pan_scene_hold_cnt = 4;
		}
		else if(pOutput->u8_pan_scene_hold_cnt != 0)
		{
			pOutput->u8_pan_scene_hold_cnt--;
		}
		else
		{
			pOutput->u8_pan_scene_hold_cnt = 0;
		}


		if(pOutput->u8_pan_scene_hold_cnt>0)
		{
			pOutput->u27_TempConsis_Th_l =(pOutput->u27_TempConsis_Th_l * (pParam->u8_SC_Gain)) >> (pParam->u8_SC_slope);
			pOutput->u27_TempConsis_Th_h = (pOutput->u27_TempConsis_Th_h * (pParam->u8_SC_Gain)) >> (pParam->u8_SC_slope);
		}
	}

	if(log_en){
		rtd_pr_memc_emerg( "[%s]  pan_cnt,%d,  th_H,%d,  th_L,%d\n",__FUNCTION__, pOutput->u8_pan_scene_hold_cnt, pOutput->u27_TempConsis_Th_h, pOutput->u27_TempConsis_Th_l);
	}
	#if 0//Debug_log
	//WriteRegister(SOFTWARE1_SOFTWARE1_25_reg, 0, 31, pOutput->u27_TempConsis_Th_h);
	//WriteRegister(SOFTWARE1_SOFTWARE1_26_reg, 0, 31, pOutput->u27_TempConsis_Th_l );
	#endif
}

VOID FBCross_Scene(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	int s11_1st_gmv_mvx = (s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb)>>2;
	//int s10_1st_gmv_mvy = (s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb)>>2;  //un-used
	unsigned int  u32_1st_gmv_cnt  = s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb;

	int s11_2nd_gmv_mvx = (s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb)>>2;
	//int s10_2nd_gmv_mvy = (s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb)>>2;  //un-used
	unsigned int  u32_2nd_gmv_cnt  = s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb;

	unsigned int u32_i;
	unsigned char u8_bad_conf_num = 0;
	static unsigned char u8_avg_bad_conf_num = 0;
	static int	s11_pre_2nd_gmv_mvx= 0;

	unsigned int gmv_1st_2nd_diff_th=7000, u8_bad_conf_num_diff_th= 12, gmv_speed_th = 10, u8_cross_scene_add= 5,u32_2nd_gmv_cnt_thl=600;

	int slope = 0;
	int cross_gain = 0, cross_gain_min = 64, cross_gain_max = 256;
	int cross_thL = 0, cross_thH = 48;

	unsigned int u32_dh_pfvconf_thl=0x1d000;
	//ReadRegister(SOFTWARE1_SOFTWARE1_40_reg, 0, 31, &gmv_1st_2nd_diff_th);
	//ReadRegister(SOFTWARE1_SOFTWARE1_41_reg, 0, 31, &u32_2nd_gmv_cnt_thl);

	unsigned int u32_RB_val, log_en;
	ReadRegister(HARDWARE_HARDWARE_58_reg, 17, 17, &u32_RB_val);//bit 17
	ReadRegister(SOFTWARE3_SOFTWARE3_50_reg, 15, 15, &log_en);

	//=== dh occl statistics
	for (u32_i = 0; u32_i < 32; u32_i ++)
	{
		if (s_pContext->_output_read_comreg.u20_dh_pfvconf[u32_i] > u32_dh_pfvconf_thl)
		{
			u8_bad_conf_num++;
		}
	}
	u8_avg_bad_conf_num = (u8_avg_bad_conf_num*2+u8_bad_conf_num*6)>>3;
	//================


	if (pOutput->u8_pan_scene_hold_cnt >4 && _ABS_DIFF_(s11_pre_2nd_gmv_mvx , s11_2nd_gmv_mvx) >= 1)  //  panning & sec. mv acceleration/deceleration
	{
		if (u32_2nd_gmv_cnt >= u32_2nd_gmv_cnt_thl)  // sec. mv consistent
		{
			pOutput->u8_cross_scene += 1;
		}
	}
	else if (_ABS_DIFF_(u32_1st_gmv_cnt, u32_2nd_gmv_cnt) <= gmv_1st_2nd_diff_th && u8_avg_bad_conf_num >= u8_bad_conf_num_diff_th)  // only h cross motion
	{
		if ((s11_1st_gmv_mvx > gmv_speed_th && s11_2nd_gmv_mvx < -gmv_speed_th) || (s11_1st_gmv_mvx < -gmv_speed_th && s11_2nd_gmv_mvx > gmv_speed_th))
		{
			pOutput->u8_cross_scene += u8_cross_scene_add;

		}
	}
	else if (pOutput->u8_cross_scene > 0)
		pOutput->u8_cross_scene--;

	//record
	s11_pre_2nd_gmv_mvx = s11_2nd_gmv_mvx;
	//==============
	// map curve
	pOutput->u8_cross_scene = _CLIP_(pOutput->u8_cross_scene, cross_thL, cross_thH);
	slope = ((cross_gain_min-cross_gain_max) << 8)/(cross_thH-cross_thL);  //	negative
	cross_gain = cross_gain_max + (((pOutput->u8_cross_scene-cross_thL) * slope + (1<<7)) >> 8);
	if(pParam->u1_FB_cross_apply_en)
	{
		pOutput->u27_TempConsis_Th_h = (pOutput->u27_TempConsis_Th_h * cross_gain + (1<<7))>>8;
		pOutput->u27_TempConsis_Th_l = (pOutput->u27_TempConsis_Th_l * cross_gain + (1<<7))>>8;
	}
	//============

	if(log_en){
		rtd_pr_memc_emerg( "[%s] cross_scene,%d,  cross_gain,%d,  th_H,%d,  th_L,%d,\n",__FUNCTION__,pOutput->u8_cross_scene, cross_gain, pOutput->u27_TempConsis_Th_h,pOutput->u27_TempConsis_Th_l);
	}
	
	#if Debug_log
	if(u32_RB_val){
		rtd_pr_memc_notice( "[%s][%d][%d]!\n",__FUNCTION__,pOutput->u8_cross_scene,cross_gain);
		rtd_pr_memc_notice( "[%s](Th_h,Th_l)=(%d,%d)\n",__FUNCTION__,pOutput->u27_TempConsis_Th_h,pOutput->u27_TempConsis_Th_l);
	}
	
	WriteRegister(SOFTWARE1_SOFTWARE1_31_reg, 0, 31, pOutput->u8_cross_scene);
	WriteRegister(SOFTWARE1_SOFTWARE1_32_reg, 0, 31, pOutput->u27_TempConsis_Th_h );
	WriteRegister(SOFTWARE1_SOFTWARE1_33_reg, 0, 31, pOutput->u27_TempConsis_Th_l );
	#endif
}

VOID FBLevelCalc(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	_PQLPARAMETER *s_pParam       = GetPQLParameter();

	unsigned int u32_result_check[6]={0};
	unsigned int log_en;

	unsigned int u32_RB_val;
	#if 0 //for YE Test temp test
	static unsigned int u32_ID_Log_en=0;//,ES_Test_Value=0;
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 27,  27, &u32_ID_Log_en);//YE Test debug dummy register
	#endif

	ReadRegister(SOFTWARE3_SOFTWARE3_06_reg, 5, 5, &log_en);


	ReadRegister(HARDWARE_HARDWARE_58_reg, 17, 17, &u32_RB_val);//bit 17

	// temp consistency mapping u8_FBResult
	{
		// TC limit
		unsigned int u27_mv_tempconsis  = s_pContext->_output_read_comreg.u27_me_aTC_rb;
		unsigned int u27_mv_tempconsis_limit = 0;
		unsigned char  u8_tc_iirA;

		u27_mv_tempconsis_limit = (pOutput->u1_panx_scene == 1 || pOutput->u1_pany_scene == 1)? \
			(_MIN_(_MIN_(pParam->u32_FBTempConsis_limit_val,pOutput->u27_FBTempConsisPre_raw*pParam->u8_FBTempConsis_limit_gain>>2), u27_mv_tempconsis)): \
			((u27_mv_tempconsis>pParam->u32_FBTempConsis_limit_val)&&(u27_mv_tempconsis>pOutput->u27_FBTempConsisPre_raw*pParam->u8_FBTempConsis_limit_gain>>2)? \
			_MIN_(pParam->u32_FBTempConsis_limit_val,pOutput->u27_FBTempConsisPre_raw*pParam->u8_FBTempConsis_limit_gain>>2):u27_mv_tempconsis);

		// TC iir.
		u8_tc_iirA =  pOutput->u1_fbLvl_tc_noIIR == 1 ? 0 : (u27_mv_tempconsis_limit > pOutput->u27_FBTempConsisPre? pParam->u8_FBTempConsisIIR_up : pParam->u8_FBTempConsis_IIR_down);
		pOutput->u27_FBTempConsis = _2Val_RndBlending_(pOutput->u27_FBTempConsisPre,u27_mv_tempconsis_limit,u8_tc_iirA,8);
		pOutput->u8_FBResult      = AutoTh_CurveMapping(pOutput->u27_FBTempConsis, pOutput->u27_TempConsis_Th_l, pOutput->u27_TempConsis_Th_h, 8);
       // rtd_pr_memc_debug("u8_FBResult_first=%d u27_mv_tempconsis_limit=%d  u27_FBTempConsis=%d\n",pOutput->u8_FBResult,u27_mv_tempconsis_limit,pOutput->u27_FBTempConsis);
  //      rtd_pr_memc_debug("u1_panx_scene=%d u1_pany_scene=%d\n",pOutput->u1_panx_scene,pOutput->u1_pany_scene);
    //    rtd_pr_memc_debug("u8_FBResult_first=%d\n",pOutput->u8_FBResult);

		u32_result_check[0]=pOutput->u8_FBResult;

		if(log_en){
			/*rtd_pr_memc_crit("FB Result,%u ,Consis,%u,%u,%u, TempConsis,%u,%u,%u,\n", 
				pOutput->u8_FBResult, pOutput->u27_FBTempConsis, pOutput->u27_TempConsis_Th_l, pOutput->u27_TempConsis_Th_h, 
				pOutput->u27_FBTempConsisPre, u27_mv_tempconsis_limit, u8_tc_iirA);*/
		}



		#if 0
		if(u32_ID_Log_en==1){
		rtd_pr_memc_debug("[YE MEMC FBR] ,XXX, u8_FBResult=%d,u27_FBTempConsis=%d,u27_TempConsis_Th_l=%d, u27_TempConsis_Th_h=%d,rimRatio=%d, mv_tempconsis=%d, mv_tempconsis_limit=%d, FBTempConsisIIR_up=%d,FBTempConsis_IIR_down=%d, u27_me_aTC_rb=%d, \n  ",
		pOutput->u8_FBResult ,pOutput->u27_FBTempConsis,pOutput->u27_TempConsis_Th_l,pOutput->u27_TempConsis_Th_h,
		u32_rimRatio,u27_mv_tempconsis,u27_mv_tempconsis_limit,pParam->u8_FBTempConsisIIR_up,pParam->u8_FBTempConsis_IIR_down,s_pContext->_output_read_comreg.u27_me_aTC_rb);
		}
		#endif

#if 0
		LogPrintf(DBG_MSG, "FB_tc_iirA = %d, sc = %d \r\n", u8_tc_iirA, s_pContext->_output_read_comreg.u1_sc_status_rb);
		LogPrintf(DBG_MSG, "FB_tc_cur = %d, FB_tc_pre = %d \r\n", u27_mv_tempconsis, pOutput->u27_FBTempConsisPre);
#endif

		pOutput->u27_FBTempConsisPre     = pOutput->u27_FBTempConsis;
		pOutput->u27_FBTempConsisPre_raw = u27_mv_tempconsis;
	}

	// option for fastmotion
	{
		unsigned char u1_fastMotion_gfb_en = s_pParam->_param_frc_sceneAnalysis.u1_fastMotion_gfb_en;//((U32_DATA>>30) & 0x1);

		unsigned char u8_fastMotion_glbfb_min = s_pParam->_param_frc_sceneAnalysis.u8_fastMotion_glbfb_min;//(U32_DATA>>24)&0xff;

		if(s_pContext->_output_frc_sceneAnalysis.u1_fastMotion_det)
		{
			if(u1_fastMotion_gfb_en == 1)
			{
				pOutput->u8_FBResult = (pOutput->u8_FBResult < u8_fastMotion_glbfb_min) ? u8_fastMotion_glbfb_min : (pOutput->u8_FBResult);
			}
		}
	}


	u32_result_check[1]=pOutput->u8_FBResult;

	
 	//rtd_pr_memc_debug("u8_FBResult=%d u1_fastMotion_det=%d u1_fastMotion_gfb_en=%d u8_fastMotion_glbfb_min=%d\n",pOutput->u8_FBResult,s_pContext->_output_frc_sceneAnalysis.u1_fastMotion_det,s_pParam->_param_frc_sceneAnalysis.u1_fastMotion_gfb_en,s_pParam->_param_frc_sceneAnalysis.u8_fastMotion_glbfb_min);

	// fb_result IIR
	{
		unsigned int u20_cur_FBResult   = pOutput->u8_FBResult << 12; // u8.12
		unsigned int u12_FBlvl_iirA     = 0;

	 	u12_FBlvl_iirA     = pOutput->u1_fbLvl_result_noIIR == 1? 0 : \
		                        (pOutput->u8_pan_scene_hold_cnt>0)?(pParam->u12_FBLevelIIRDown):(u20_cur_FBResult > pOutput->u20_FBPreResult? pOutput->u12_fblvl_iir_up : pParam->u12_FBLevelIIRDown);


		u20_cur_FBResult          = _2Val_FlrBlending_(pOutput->u20_FBPreResult, (pOutput->u8_FBResult << 12), u12_FBlvl_iirA, 12);

		pOutput->u20_FBPreResult  = u20_cur_FBResult; // u8.12
		pOutput->u8_FBResult      = (u20_cur_FBResult >> 12) + ((u20_cur_FBResult >> 11) & 0x01); // u8.0, rounding.

       // rtd_pr_memc_debug("u8_FBResult=%d u20_cur_FBResult=%d u12_FBlvl_iirA=%d\n",pOutput->u8_FBResult,u20_cur_FBResult,u12_FBlvl_iirA);
        
   //     rtd_pr_memc_debug("u8_FBResult_IIR=%d\n",pOutput->u8_FBResult);
#if 0
		LogPrintf(DBG_MSG, "FB_lvl_iirA = %d\r\n", u12_FBlvl_iirA);
		LogPrintf(DBG_MSG, "FB_lvl_cur = %d, FB_lvl_pre = %d, iir_result = %d,8bit_iir = %d, \r\n", u20_FBLevel_Temp, pOutput->u20_FBPreResult, pOutput->u20_FBPreResult, pOutput->u20_FBResult);
#endif
	}


	u32_result_check[2]=pOutput->u8_FBResult;

	// other module fall back level calc.
 	fbTC_dhClose_ctrl(pParam, pOutput);

	u32_result_check[3]=pOutput->u8_FBResult;

	#if Debug_log
	if(u32_RB_val){
		rtd_pr_memc_notice( "[%s]1.FBLevel=%d[%d]!\n",__FUNCTION__,pOutput->u8_FBLevel,pOutput->u27_FBTempConsis);
	}
	#endif
	fbTC_deBlur_ctrl(pParam, pOutput);


	u32_result_check[4]=pOutput->u8_FBResult;


	#if Debug_log
	if(u32_RB_val){
		rtd_pr_memc_notice( "[%s]2.FBLevel=%d[%d]!\n",__FUNCTION__,pOutput->u8_FBLevel,pOutput->u27_FBTempConsis);
	}
	#endif
	fbTC_deJudder_ctrl(pParam, pOutput);
	

	u32_result_check[5]=pOutput->u8_FBResult;


	#if Debug_log
	if(u32_RB_val){
		rtd_pr_memc_notice( "[%s]3.FBLevel=%d[%d]!\n",__FUNCTION__,pOutput->u8_FBLevel,pOutput->u27_FBTempConsis);
	}
	#endif
 //   rtd_pr_memc_debug("u8_FBResult_beforefinal=%d\n",pOutput->u8_FBResult);

	//fb_result map to final fb_level
	if (pOutput->u8_FBResult <= pParam->u8_FBLevelMapX1)  //32
	{
		pOutput->u8_FBLevel = 0;
	}
	else if (pOutput->u8_FBResult > pParam->u8_FBLevelMapX2)  //192
	{
		//pOutput->u8_FBLevel = _FB_LEVEL_MAX_;
		pOutput->u8_FBLevel = pOutput->u8_fblvl_map_y2;
	}
	else
	{
		unsigned int u32_slope = 0;

		if(pParam->u8_FBLevelMapX2 > pParam->u8_FBLevelMapX1){
			u32_slope = ((pOutput->u8_fblvl_map_y2-pParam->u8_FBLevelMapY1)<<8)/(pParam->u8_FBLevelMapX2-pParam->u8_FBLevelMapX1);
		}else{
			rtd_pr_memc_notice( "[%s]Invalid FBLevelMapX(x1:%d,x2:%d)!\n",\
				__FUNCTION__,pParam->u8_FBLevelMapX1,pParam->u8_FBLevelMapX2);
			u32_slope = ((pParam->u8_FBLevelMapY2-pParam->u8_FBLevelMapY1)<<8);
		}
		pOutput->u8_FBLevel = pParam->u8_FBLevelMapY1 + (((pOutput->u8_FBResult-pParam->u8_FBLevelMapX1)*u32_slope)>>8);
	}

	//pOutput->u8_FBLevel = (pOutput->u8_FBLevel > _FB_LEVEL_MAX_) ? _FB_LEVEL_MAX_ : pOutput->u8_FBLevel;
	pOutput->u8_FBLevel = _CLIP_UBOUND_(pOutput->u8_FBLevel, pOutput->u8_fblvl_map_y2);
    
	#if Debug_log
	if(u32_RB_val){
		rtd_pr_memc_notice( "[%s]4.FBLevel=%d[%d,%d]!\n",__FUNCTION__,pOutput->u8_FBLevel,pOutput->u8_FBResult,pOutput->u27_FBTempConsis);
	}	
	#endif
	if(u32_RB_val){//d5e8[17]
		rtd_pr_memc_emerg( "[%s]4.FBLevel=%d[%d,%d]! check,%d,%d,%d,%d,%d,%d\n",__FUNCTION__,pOutput->u8_FBLevel,pOutput->u8_FBResult,pOutput->u27_FBTempConsis,
			u32_result_check[0],u32_result_check[1],u32_result_check[2],u32_result_check[3],u32_result_check[4],u32_result_check[5]);
	}


	
  //  rtd_pr_memc_debug("result_final=%d u8_FBLevel=%d \n",pOutput->u8_FBResult,pOutput->u8_FBLevel);

    //rtd_pr_memc_debug("result=%d y1=%d y2=%d x1=%d x2=%d slope=%d\n",pOutput->u8_FBResult,pParam->u8_FBLevelMapY1,pOutput->u8_fblvl_map_y2,pParam->u8_FBLevelMapX1,pParam->u8_FBLevelMapX2,((pParam->u8_FBLevelMapY2-pParam->u8_FBLevelMapY1)<<8));
}

unsigned int AutoTh_CurveMapping(unsigned int iVal, unsigned int autoThL, unsigned int autoThH, unsigned char outPrecision)
{
	unsigned int u32_rtVal = 0;
	if (iVal < autoThL)
	{
		u32_rtVal = 0;
	}
	else if (iVal >= autoThH)
	{
		u32_rtVal = (1 << outPrecision);
	}
	else
	{
		if(autoThH > autoThL){
		u32_rtVal = ((iVal - autoThL) << outPrecision) / (autoThH - autoThL);
		}else{
			rtd_pr_memc_notice( "[%s]Invalid autoThr(H:%d,L:%d)!\n",\
								__FUNCTION__,autoThH,autoThL);
			u32_rtVal = (iVal - autoThL) << outPrecision;
		}
	}

	return u32_rtVal;
}

VOID fbTC_dhClose_ctrl(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	unsigned int u27_tc_thL, u27_tc_thH;
	unsigned char  u8_Dh_FbLvl_cur;
	if (pParam->u1_Fblvl_Dh_tcTh_mapEn == 0)
	{
		u27_tc_thL = pOutput->u27_TempConsis_Th_l;
		u27_tc_thH = pOutput->u27_TempConsis_Th_h;
	}
	else
	{
		u27_tc_thL = pOutput->u27_TempConsis_Th_l*pParam->u8_FbLvl_Dh_tc_thLgain >> 6/*_MIN_(pOutput->u27_TempConsis_Th_l*pParam->u8_FbLvl_Dh_tc_thLgain >> 6, 0x7ffffff)*/;
		u27_tc_thH = pOutput->u27_TempConsis_Th_h*pParam->u8_FbLvl_Dh_tc_thHgain >> 6/*_MIN_(pOutput->u27_TempConsis_Th_h*pParam->u8_FbLvl_Dh_tc_thHgain >> 6, 0x7ffffff)*/;
	}

	u8_Dh_FbLvl_cur            = _MIN_(0xFF, AutoTh_CurveMapping(pOutput->u27_FBTempConsis, u27_tc_thL, u27_tc_thH, 8));
	pOutput->u8_Dehalo_FBLevel = _2Val_FlrBlending_(pOutput->u8_Dehalo_FBLevel, u8_Dh_FbLvl_cur, pParam->u8_DehaloFBlvl_iir, 8); //u8_DehaloFBlvl_iir =100
//	rtd_pr_memc_debug("pOutput->u8_Dehalo_FBLevel=%d u27_FBTempConsis=%d u27_tc_thL=%d u27_tc_thH=%d\n",pOutput->u8_Dehalo_FBLevel,pOutput->u27_FBTempConsis,u27_tc_thL,u27_tc_thH);
	

#if 0
	LogPrintf(DBG_MSG, "FB_Dh, TC = %d, thL = %d, thH = %d\r\n", pOutput->u27_FBTempConsis, u27_tc_thL, u27_tc_thH);
	LogPrintf(DBG_MSG, "FB_Dh, 1frm = %d, iir = %d\r\n---------------\r\n", u8_Dh_FbLvl_cur, pOutput->u8_Dehalo_FBLevel);
#endif
}
VOID fbTC_deBlur_ctrl(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int u27_TempConsis_ThL = s_pContext->_output_fblevelctrl.u27_TempConsis_Th_l;
	unsigned int u27_TempConsis_ThH = s_pContext->_output_fblevelctrl.u27_TempConsis_Th_h;
	unsigned int u12_gmv_unconf_rb   = s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb;

	unsigned char  u1_Scene_isGood    = 0;
	unsigned int u16_blur_FBlvl_cur  = 0;
	unsigned int u27_tc_thL         = u27_TempConsis_ThL*pParam->u8_FbLvl_blur_tc_thLgain >> 6/*_CLIP_(u27_TempConsis_ThL*pParam->u8_FbLvl_blur_tc_thLgain >> 6, 0, 0x7ffffff)*/;
	unsigned int u27_tc_thH         = u27_TempConsis_ThH*pParam->u8_FbLvl_blur_tc_thHgain >> 6/*_CLIP_(u27_TempConsis_ThH*pParam->u8_FbLvl_blur_tc_thHgain >> 6, 0, 0x7ffffff)*/;

	u16_blur_FBlvl_cur        = _MIN_(0x80, AutoTh_CurveMapping(pOutput->u27_FBTempConsis, u27_tc_thL, u27_tc_thH, _PHASE_FLBK_LVL_PRECISION));

	if(pOutput->u1_deBlur_gmvU_isGood == 0 && (u12_gmv_unconf_rb <= pParam->u12_FbLvl_blur_gmvUnconf_th0))
	{
		u1_Scene_isGood = 1;
	}
	else if(pOutput->u1_deBlur_gmvU_isGood == 1 && (u12_gmv_unconf_rb >= pParam->u12_FbLvl_blur_gmvUnconf_th1))
	{
		u1_Scene_isGood = 0;
	}
	else
	{
		u1_Scene_isGood = pOutput->u1_deBlur_gmvU_isGood;
	}

	u16_blur_FBlvl_cur              = u1_Scene_isGood == 1? u16_blur_FBlvl_cur : _MAX_(u16_blur_FBlvl_cur, pParam->u5_FbLvl_blur_badscene_dftLvl);
	u16_blur_FBlvl_cur              = _2Val_FlrBlending_(pOutput->u16_blur_lvl, (u16_blur_FBlvl_cur << 4), pParam->u8_FbLvl_blur_iir, 8); // ux.4
	pOutput->u16_blur_lvl           = (pParam->u1_Fblvl_blur_ctrlEn == 1)? u16_blur_FBlvl_cur : 0;
	pOutput->u1_deBlur_gmvU_isGood = u1_Scene_isGood;
}

VOID fbTC_deJudder_ctrl(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	unsigned int u16_judder_FBlvl_cur = 0;
	unsigned int u27_tc_thL = pOutput->u27_TempConsis_Th_l*pParam->u8_FbLvl_judder_tc_thLgain >> 6/*_CLIP_(pOutput->u27_TempConsis_Th_l*pParam->u8_FbLvl_judder_tc_thLgain >> 6, 0, 0x7ffffff)*/;
	unsigned int u27_tc_thH = pOutput->u27_TempConsis_Th_h*pParam->u8_FbLvl_judder_tc_thHgain >> 6/*_CLIP_(pOutput->u27_TempConsis_Th_h*pParam->u8_FbLvl_judder_tc_thHgain >> 6, 0, 0x7ffffff)*/;

	u16_judder_FBlvl_cur     = _MIN_(0x80, AutoTh_CurveMapping(pOutput->u27_FBTempConsis, u27_tc_thL, u27_tc_thH, _PHASE_FLBK_LVL_PRECISION));
	u16_judder_FBlvl_cur     = _2Val_FlrBlending_(pOutput->u16_judder_lvl, (u16_judder_FBlvl_cur<<4), pParam->u8_FbLvl_judder_iir, 8); // UX.4
	pOutput->u16_judder_lvl  = (pParam->u1_Fblvl_judder_ctrlEn == 1)? u16_judder_FBlvl_cur : 0;
}
VOID SceneChage_Analysis(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int u27_tc_rb     = s_pContext->_output_read_comreg.u27_me_aTC_rb;
	unsigned int u28_sadAll_rb = s_pContext->_output_read_comreg.u30_me_aSAD_rb;
	unsigned char u1_HW_sc_true  = pOutput->u1_HW_sc_true || s_pContext->_output_read_comreg.u1_sc_status_rb;

	// frame cnt statistic in current
	unsigned char u1_badScene_true, u8_badFrm_cnt, u8_frm_cnt;
	{
		unsigned char u1_scene_good2bad = u27_tc_rb >= pParam->u27_scAnalysis_TC_th0 || u28_sadAll_rb >= pParam->u28_scAnalysis_sadAll_th0;
		unsigned char u1_scene_bad2good = u27_tc_rb <= pParam->u27_scAnalysis_TC_th1 && u28_sadAll_rb <= pParam->u28_scAnalysis_sadAll_th1;
		u1_badScene_true = _AB_Threshold_Status(pOutput->u1_preFrm_isBadScene, u1_scene_good2bad, u1_scene_bad2good);

		u8_badFrm_cnt = (u1_badScene_true == 1 && u1_HW_sc_true == 1)? (pOutput->u8_HW_sc_badFrm_cnt + 1) :(pOutput->u8_HW_sc_badFrm_cnt);
		u8_frm_cnt    = (u1_HW_sc_true == 1)? pOutput->u8_HW_sc_Frm_cnt + 1 : 0;
	}

	// generate of SW_scene change
	pOutput->u1_SW_sc_true = 0;
	if(u1_HW_sc_true == 1)
	{
		if(u8_frm_cnt == pParam->u4_scAnalysis_frmCnt_th)
		{
			pOutput->u1_SW_sc_true        = u8_badFrm_cnt < pParam->u4_scAnalysis_badFrmCnt_th? 1 : 0;
			pOutput->u1_HW_sc_true        = 0;
			pOutput->u8_HW_sc_badFrm_cnt  = 0;
			pOutput->u8_HW_sc_Frm_cnt     = 0;
			pOutput->u1_preFrm_isBadScene = 0;
		}
		else
		{
			pOutput->u1_SW_sc_true        = 0;
			pOutput->u1_HW_sc_true        = u1_HW_sc_true;
			pOutput->u8_HW_sc_badFrm_cnt  = u8_badFrm_cnt;
			pOutput->u8_HW_sc_Frm_cnt     = u8_frm_cnt;
			pOutput->u1_preFrm_isBadScene = u1_badScene_true;
		}
	}
	else
	{
		pOutput->u1_SW_sc_true        = 0;
		pOutput->u1_HW_sc_true        = 0;
		pOutput->u8_HW_sc_badFrm_cnt  = 0;
		pOutput->u8_HW_sc_Frm_cnt     = 0;
		pOutput->u1_preFrm_isBadScene = 0;
	}

	// status update
	pOutput->u1_SW_sc_true = (pParam->u1_scAnalysis_en == 0)? s_pContext->_output_read_comreg.u1_sc_status_rb : (pOutput->u1_SW_sc_true || pOutput->u1_swSC_static2move_true);

	if(pOutput->u1_SW_sc_true == 1)
	{
		pOutput->u8_swSC_disIIR_holdCnt = pParam->u8_FbLvl_disIIR_holdCnt;
	}
	else
	{
		pOutput->u8_swSC_disIIR_holdCnt = (pOutput->u8_swSC_disIIR_holdCnt > 0)? (pOutput->u8_swSC_disIIR_holdCnt- 1) : 0;
	}
}
VOID FB_static2move_swSC_det(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
#if 0
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int u27_ipme_motion  = s_pContext->_output_read_comreg.u27_fdet_mot_all_rb;
	unsigned char  u8_cadence_phId  = s_pContext->_output_filmDetectctrl.u8_PhaseIdx;
	unsigned char  u8_cadence_Id    = s_pContext->_output_filmDetectctrl.u8_CurCadence;
	unsigned char  u8_staticFrm_cnt = pOutput->u8_swSC_static_frmCnt;
	unsigned char  u1_staticScene_true = 0,u1_cad_motionSeq = 0,u1_curFrame_state = 0;

	if(u8_cadenceId >= _FRC_CADENCE_NUM_){
		LogPrintf(DBG_WARNING,"[%s]Invalid Cadence:%d,Resume to '_CAD_VIDEO'!\n",__FUNCTION__,u8_cadenceId);
		u8_cadenceId = _CAD_VIDEO;
	}
	u1_staticScene_true = u27_ipme_motion < pParam->u27_scAnalysis_static_motionTh? 1 : 0;
	u1_cad_motionSeq    = cadence_seqFlag(frc_cadTable[u8_cadence_Id], cadence_prePhase(frc_cadTable[u8_cadence_Id],u8_cadence_phId, 1));
	u1_curFrame_state   = u1_cad_motionSeq == 0? 0 : (u1_staticScene_true == 1? 1 : 2); // 0: no consider, 1: static, 2: move

	pOutput->u1_swSC_static2move_true = (u1_curFrame_state == 2 && u8_staticFrm_cnt >= pParam->u8_scAnalysis_st2mo_stPeriodTh)? 1 : 0;
	pOutput->u8_swSC_static_frmCnt    =  u1_curFrame_state == 0? u8_staticFrm_cnt : (u1_curFrame_state == 1? _MIN_(u8_staticFrm_cnt + 1, 255) : 0);
#endif
}


VOID SC_Signal_Proc(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
#if 0
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned char u1_hw_sc_signal = s_pContext->_output_read_comreg.u1_sc_status_rb;
	unsigned int u8_mc_phase = 0;
	unsigned char  u1_phase_start_en;

	ReadRegister(MC_MC_F8_reg,0,6, &u8_mc_phase);
	u1_phase_start_en = (u8_mc_phase < pOutput->u8_mc_phase_pre) ? 1 : 0;

	if (s_pContext->_output_read_comreg.u27_fdet_mot_all_rb < pParam->u32_static_mot_th)
	{
		pOutput->u8_static_cnt ++;
		pOutput->u8_static_cnt = _CLIP_(pOutput->u8_static_cnt, 0, 30);
	}
	else
	{
		pOutput->u8_static_cnt = 0;
	}


	if (pOutput->u1_sc_signal == 1)
	{
		if (pOutput->u8_sc_bypass_cnt > 0)
		{
			pOutput->u8_sc_bypass_cnt --;
		}
		else
		{
			if (u1_phase_start_en == 1)
			{
				if (pOutput->u8_static_cnt >= pParam->u8_static_cnt_th)
				{
					pOutput->u8_sc_bypass_cnt = pParam->u8_sc_bypass_cnt;
				}
				else
				{
					pOutput->u1_sc_signal = 0;
				}
			}
		}
	}

	if (u1_hw_sc_signal == 1)
	{
		pOutput->u1_sc_signal = 1;
		pOutput->u8_sc_bypass_cnt = pParam->u8_sc_bypass_cnt;
	}

	pOutput->u8_mc_phase_pre = u8_mc_phase;
#endif
}

unsigned int Multiply_2Val_protect(unsigned int multiA, unsigned int multiB, unsigned char divideBit_A)
{
	//multiplication distributive property
	unsigned int multiA_h = multiA >> divideBit_A;
	unsigned int multiA_l = multiA  - (multiA_h << divideBit_A);

	unsigned int multi_h = multiA_h * multiB;
	unsigned int multi_l = multiA_l * multiB;

	// protect.
	unsigned int retV;
	if ((unsigned int)(multi_h + (multi_l >> divideBit_A)) > (unsigned int)((1 << (32 - divideBit_A)) - 1))
	{
		retV = 0xFFFFFFFF;
	}
	else
	{
		retV = (multi_h << divideBit_A) + multi_l;
	}

	return retV;
}


//Input signal and parameters are connected locally
extern unsigned char DAT_flash_flg;
VOID FBSystemCtrl_Proc(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	unsigned char  u8_flbk_lvl_FrameRepeatMode = 0;
	unsigned char  u8_flbk_lvl_HDR_PicChg = 0;
	unsigned char  u8_flbk_lvl_OSDBrightnessBigChg = 0;
	unsigned char  u8_flbk_lvl_OSDContrastBigChg = 0;
	unsigned char  u8_flbk_lvl_cad = 0;
	unsigned char  u8_flbk_lvl_voseek = 0;
	unsigned char  u8_flbk_lvl_OSDPictureModeChg = 0;
	unsigned char  u8_flbk_lvl_EPG = 0;
	unsigned char  u8_flbk_lvl_CMR_pattern = 0;//k23
	unsigned char  u8_flbk_lvl_flash = 0; // K24
	static unsigned char  u8_flash_fb_cnt = 0; // K24
	unsigned int log_en;
	ReadRegister(SOFTWARE3_SOFTWARE3_50_reg, 20, 20, &log_en);

	if(s_pContext->_external_data.u8_InputVSXral_FrameRepeatMode > 0)
	{
		u8_flbk_lvl_FrameRepeatMode = _FB_LEVEL_MAX_;
	}

	if(s_pContext->_output_frc_sceneAnalysis.u8_HDR_PicChg_prt_apply > 0)
		u8_flbk_lvl_HDR_PicChg = _FB_LEVEL_MAX_;

	if(s_pContext->_output_frc_sceneAnalysis.u8_OSDBrightnessBigChg_prt_apply > 0)
		u8_flbk_lvl_OSDBrightnessBigChg = _MIN_(_FB_LEVEL_MAX_, (_FB_LEVEL_MAX_/5)*s_pContext->_output_frc_sceneAnalysis.u8_OSDBrightnessBigChg_prt_apply);

	if(s_pContext->_output_frc_sceneAnalysis.u8_OSDContrastBigChg_prt_apply > 0)
		u8_flbk_lvl_OSDContrastBigChg = _MIN_(_FB_LEVEL_MAX_, (_FB_LEVEL_MAX_/5)*s_pContext->_output_frc_sceneAnalysis.u8_OSDContrastBigChg_prt_apply);

	if(s_pContext->_output_filmDetectctrl.u8_flbk_lvl_cad_cnt > 0)
		u8_flbk_lvl_cad = _MIN_(_FB_LEVEL_MAX_, _FB_LEVEL_MAX_*s_pContext->_output_filmDetectctrl.u8_flbk_lvl_cad_cnt/4);

	if(s_pContext->_output_frc_sceneAnalysis.u8_VOSeek_prt_apply > 0)
		u8_flbk_lvl_voseek =  _MIN_(_FB_LEVEL_MAX_*s_pContext->_output_frc_sceneAnalysis.u8_VOSeek_prt_apply/12, _FB_LEVEL_MAX_);

	if(s_pContext->_output_me_sceneAnalysis.u8_EPG_apply > 0)
		u8_flbk_lvl_EPG = _FB_LEVEL_MAX_;

	if(s_pContext->_output_me_sceneAnalysis.u1_CMR_pattern_apply > 0)//k23
		u8_flbk_lvl_CMR_pattern = _FB_LEVEL_MAX_;

//	if((16>=s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply) && (s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply>=6))
//		u8_flbk_lvl_OSDPictureModeChg = _FB_LEVEL_MAX_;
//	if((5>=s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply) && (s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply>0))
//		u8_flbk_lvl_OSDPictureModeChg = _MIN_(_FB_LEVEL_MAX_, (_FB_LEVEL_MAX_/5)*s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply);
	if(s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply>0)
		u8_flbk_lvl_OSDPictureModeChg = 0xF0;

	if(DAT_flash_flg>0){
		if(u8_flash_fb_cnt < 5)
			u8_flash_fb_cnt++;
	}
	else{
		if(u8_flash_fb_cnt > 0)
			u8_flash_fb_cnt--;
	}
	u8_flbk_lvl_flash = _MIN_(0x80, 0x80*u8_flash_fb_cnt/5);
	pOutput->u8_FBSystem =  _MAX_(_MAX_(_MAX_(_MAX_(_MAX_(_MAX_(_MAX_(_MAX_(_MAX_(u8_flbk_lvl_FrameRepeatMode
							, u8_flbk_lvl_HDR_PicChg)
							, u8_flbk_lvl_OSDBrightnessBigChg)
							, u8_flbk_lvl_OSDContrastBigChg)
							, u8_flbk_lvl_cad)
							, u8_flbk_lvl_voseek)
							, u8_flbk_lvl_EPG)
							, u8_flbk_lvl_OSDPictureModeChg)
							, u8_flbk_lvl_CMR_pattern )
							, u8_flbk_lvl_flash );
	/*pOutput->u8_FBSystem =  _MAX_(_MAX_(_MAX_(_MAX_(_MAX_(_MAX_(_MAX_(u8_flbk_lvl_FrameRepeatMode
							, u8_flbk_lvl_HDR_PicChg)
							, u8_flbk_lvl_OSDBrightnessBigChg)
							, u8_flbk_lvl_OSDContrastBigChg)
							, u8_flbk_lvl_cad)
							, u8_flbk_lvl_voseek)
		                                   , u8_flbk_lvl_EPG)
							, u8_flbk_lvl_OSDPictureModeChg);*/


	if(log_en){
		rtd_pr_memc_emerg("[%s] u8_FBSystem,%d, val,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\n",__FUNCTION__, pOutput->u8_FBSystem,
			u8_flbk_lvl_FrameRepeatMode, u8_flbk_lvl_HDR_PicChg, u8_flbk_lvl_OSDBrightnessBigChg, 
			u8_flbk_lvl_OSDContrastBigChg, u8_flbk_lvl_cad, u8_flbk_lvl_voseek, 
			u8_flbk_lvl_EPG, u8_flbk_lvl_OSDPictureModeChg, u8_flbk_lvl_CMR_pattern, u8_flbk_lvl_flash);
	}
}

VOID MC_RMV_blending(const _PARAM_bRMV_rFB_CTRL *pParam, _OUTPUT_bRMV_rFB_CTRL *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	
	int i = 0, tmp1 = 0, tmp2 = 0, rmv_alpha_tmp = 0, mvx_temp = 0, mvy_temp = 0;
	bool opposite_direction_x = 0, opposite_direction_y = 0;
	int opposite_direction_cnt = 0, rmv_varience_x = 0, rmv_varience_y = 0, RMV_varience = 0;

	bool   gmv_alpha_auto = pParam->u1_gmv_alpha_auto;
	unsigned char  gmv_alpha_force_value = pParam->u8_gmv_alpha_force_value;
	//unsigned int regional_cnt_thl = pParam->u12_regional_cnt_thl;
	//unsigned int regional_unconf_thl = pParam->u12_regional_unconf_thl;
	//unsigned char  regioonal_unconf_cnt_th = pParam->u8_regioonal_unconf_cnt_th;
	unsigned char  rmv_alpha_mode = pParam->u2_rmv_alpha_mode;
	unsigned char  oppo_dir_cnt_thl = pParam->u8_oppo_dir_cnt_thl;
	unsigned char  rmv_var_thl = pParam->u8_rmv_var_thl << 3;
	unsigned char  gmv_cur_gain = pParam->u8_gmv_cur_gain;
	unsigned char  gmv_cur_slp = pParam->u8_gmv_cur_slp;

	unsigned char curve_gain = pParam->u8_rmv_alpha_curve_gain;
	unsigned char curve_th0 = pParam->u8_rmv_alpha_curve_th0;
	unsigned char curve_slp = pParam->u8_rmv_alpha_curve_slp;
	unsigned char curve_min = pParam->u8_rmv_alpha_curve_min;
	unsigned char curve_max = pParam->u8_rmv_alpha_curve_max;	

	unsigned char gmv_alpha = 0;
	//unsigned int regioonal_unconf_cnt = 0;
	//unsigned int regional_cnt, regional_unconf;
	unsigned char global_fb_lvl = s_pContext->_output_fblevelctrl.u8_FBSystem; // <---------------------------------------

	//rmv_alpha_mode = 2;
	//gmv_alpha_auto = 0;
	//gmv_alpha_force_value = 64;
	//curve_gain = 16;
	//curve_th0 = 5;
	//curve_slp = 6;
	//curve_min = 0;
	//curve_max = 255; 


	// calculate gmv-alpha
	//for(i=0; i<32; i++)
	//{
	//	regional_cnt = s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[i];
	//	regional_unconf = s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[i];	
	//	if(regional_cnt > regional_cnt_thl && regional_unconf < regional_unconf_thl)
	//		regioonal_unconf_cnt++;
	//}
	
	//o----- calculate gmv-alpha ------o

	for(i=0; i<32; i++)
	{	
		opposite_direction_x = (s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i] * s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb < 0) ? 1 : 0;
		opposite_direction_y = (s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i] * s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb < 0) ? 1 : 0;
		
		if(opposite_direction_x || opposite_direction_y)
			opposite_direction_cnt++;
		
		rmv_varience_x += _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i] , s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb);
		rmv_varience_y += _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i] , s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb);		
	}
	rmv_varience_x = rmv_varience_x / 32;
	rmv_varience_y = rmv_varience_y / 32;
	RMV_varience = _MAX_(rmv_varience_x, rmv_varience_y);

	if(opposite_direction_cnt < oppo_dir_cnt_thl || RMV_varience > rmv_var_thl )
		gmv_cur_gain = 0;	
	
	gmv_alpha = (gmv_cur_gain * s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb) / (s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb + 1);
	gmv_alpha = gmv_alpha >> gmv_cur_slp;
	gmv_alpha = (gmv_alpha > 255) ? 255 : gmv_alpha;
	gmv_alpha = gmv_alpha_auto ? gmv_alpha : gmv_alpha_force_value;

	//o----- calculate rmv-alpha ------o
	//o----- calculate rmv-blending ------o

	for(i=0; i<32; i++)
	{	
		switch(rmv_alpha_mode)
		{
		case 0:
			tmp1 = (s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[i] * curve_gain) - curve_th0;
			tmp2 = (tmp1 << 5) >> curve_slp;
			rmv_alpha_tmp = _CLIP_(tmp2, curve_min, curve_max);
			break;
		case 1:
			rmv_alpha_tmp = (s_pContext->_output_read_comreg.u12_region_fb_cnt[i] > 32) ? global_fb_lvl : 0;
			break;
		case 2:
			rmv_alpha_tmp = (s_pContext->_output_read_comreg.u12_region_fb_cnt[i]==0) ? 0 : (s_pContext->_output_read_comreg.u32_region_fb_sum[i] / (s_pContext->_output_read_comreg.u12_region_fb_cnt[i] + 1));
			break;
		}
		pOutput->u8_fb_rmv_alpha[i] = rmv_alpha_tmp;	
		//o------ gmv*alpha + rmv*(1-alpha) ------o 
		mvx_temp = (s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i] * (255-gmv_alpha) + s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb * gmv_alpha) >> 8;	
		mvy_temp = (s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i] * (255-gmv_alpha) + s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb * gmv_alpha) >> 8;	
		pOutput->u11_fb_rgn00_mvx[i] = mvx_temp & 0x7ff;
		pOutput->u10_fb_rgn00_mvy[i] = mvy_temp & 0x3ff;	
		
	}
}


#if 1
VOID MC_ReginaolFB(const _PARAM_bRMV_rFB_CTRL *pParam, _OUTPUT_bRMV_rFB_CTRL *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	int i = 0;
	unsigned int tmp1 = 0, big_rTC = 0, big_rMV = 0;
	int tmp2 = 0; 
	unsigned char rFB_alpha_tmp = 0, rFB_level_tmp = 0;
	bool  big_rMV_flag = 0, big_rTC_flag = 0;
	bool  rFB_alpha_auto = pParam->u1_rFB_alpha_auto;
	bool  rFB_level_auto = pParam->u1_rFB_level_auto;
	unsigned char rFB_alpha_force_value = pParam->u8_rFB_alpha_force_value;
	unsigned char rFB_level_force_value = pParam->u8_rFB_level_force_value;
	unsigned char rFB_alpha_mode = pParam->u2_rFB_alpha_mode;
	unsigned char curve_gain = pParam->u8_rFB_alpha_curve_gain;
	unsigned char curve_th0 = pParam->u8_rFB_alpha_curve_th0;
	unsigned char curve_slp = pParam->u8_rFB_alpha_curve_slp;
	unsigned char curve_min = 0;//pParam->u8_rFB_alpha_curve_min;
	unsigned char curve_max = 0xff;//pParam->u8_rFB_alpha_curve_max;

	unsigned char lvl_curve_gain = pParam->u8_rFB_level_curve_gain;
	unsigned char lvl_curve_th0 = pParam->u8_rFB_level_curve_th0;
	unsigned char lvl_curve_slp = pParam->u8_rFB_level_curve_slp;
	unsigned char lvl_curve_min = 0;//pParam->u8_rFB_level_curve_min;
	unsigned char lvl_curve_max = 0xff;//pParam->u8_rFB_level_curve_max;
	unsigned char rFB_alpha_by_dtl_en = pParam->u1_rFB_alpha_by_dtl_en;
	unsigned char rFB_alpha_by_rMV_cnt_en = pParam->u1_rFB_alpha_by_rMV_cnt_en;

	unsigned char big_rMV_th = pParam->u8_big_rMV_thl;
	bool rFB_big_rMV_en = pParam->u1_rFB_big_rMV_en;
	bool rFB_big_rTC_en = pParam->u1_rFB_big_rTC_en;
	//for test
	unsigned char u8_th0 = pParam->u8_th0;
	unsigned char u8_th1 = pParam->u8_th1;
	unsigned char u8_th2 = pParam->u8_th2;
	unsigned char u8_th3 = pParam->u8_th3;
       //bool  rFB_panning_en = pParam->u1_rFB_panning_en;

	unsigned char global_fb_lvl = s_pContext->_output_fblevelctrl.u8_FBSystem; // <---------------------------------------
	
	//unsigned char panning_flag = s_pContext->_output_me_sceneAnalysis.u2_panning_flag;

	int rmv_x  =0;
	int rmv_y   =0;
	int rmv_x_delta =0;
	int rmv_cnt = 0;
	int rmv_unconf = 0;
	bool rmv_cond = 0;
	int rmv_2nd_cnt = 0;  
	int rmv_2nd_unconf = 0;	
	//unsigned char global_fb_lvl = s_pContext->_output_fblevelctrl.u8_FBSystem; // <---------------------------------------
	


	#if 1 //YE test for edge shaking
	unsigned int BOT_ratio = Convert_Current_RES_RATIO_To_UHD(((_ABS_DIFF_(s_pContext->_output_rimctrl.u12_out_resolution[_RIM_BOT] , s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_BOT]))<<10), PARAM_DIRECTION_Y)>>11;
	//unsigned int TOP_ratio = Convert_Current_RES_RATIO_To_UHD(((_ABS_DIFF_( s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP] , s_pContext->_output_rimctrl.u12_out_resolution[_RIM_TOP]))<<10), PARAM_DIRECTION_Y)>>11;
	unsigned int LFT_ratio = Convert_Current_RES_RATIO_To_UHD(((_ABS_DIFF_(s_pContext->_output_rimctrl.u12_out_resolution[_RIM_LFT] , s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_LFT]))<<10), PARAM_DIRECTION_X)>>11;
	//_OUTPUT_RimCtrl

	static unsigned int ES_Test_29 =0,ES_Test_30 =0,ES_Test_28 =0,ES_Test_25 =0;//,ES_Test_26 =0;//ES_Test_GFB =0;ES_Test_27 =0,
	static unsigned int ES_Test_0=0,ES_Test_1=0,ES_Test_2=0,ES_Test_3=0,ES_Test_4=0,
				 ES_Test_6=0, ES_Test_7=0, ES_Test_8=0, ES_Test_9=0, ES_Test_10=0,ES_Test_11=0,
				 ES_Test_12=0,ES_Test_13=0,ES_Test_24=0;//,ES_Test_Solution2=0;
	//unsigned int rmv_cnt_temp=0,rmv_unconf_temp=3, rmv_x_temp=80,rmv_x_delta_temp=110;
	unsigned int u25_rgnSad_rb = s_pContext->_output_read_comreg.u25_me_rSAD_rb[i];
	unsigned int u20_rgnDtl_rb = s_pContext->_output_read_comreg.u20_me_rDTL_rb[i];
	
	//unsigned int u22_rTC_rb = s_pContext->_output_read_comreg.u22_me_rTC_rb[i];
	unsigned int u13_rPRD_rb = s_pContext->_output_read_comreg.u13_me_rPRD_rb[i];

	
	unsigned char  ES_Flag_364=0,ES_Flag_354=0,ES_Flag_317=0,ES_Flag_317_temp=0,ES_Flag_439=0,ES_Flag_21=0,
		    ES_Flag_380=0,ES_Flag_413=0,ES_Flag_421=0,Rassia_Flag=0,ES_Flag_040=0,ES_Flag_059=0,ES_Flag_043=0,
		    ES_Flag_035=0,ES_Flag_009_2=0,ES_Flag_072_1=0,ES_Flag_072_2=0,ES_Flag_149=0,ES_Flag_151=0,
		    ES_Flag_182_1=0,ES_Flag_182_2=0,ES_Flag_208=0,ES_Flag_219=0,ES_Flag_316=0,ES_Flag_319=0
		    ,ES_Flag_009_1=0,ES_Flag_358=0,ES_Flag_109=0;
		    //,ES_Flag_009_1=0;//,ES_Flag_072_2=0;

	

	unsigned char j=0, k=0;
	//unsigned char L_To_R_Paning=0,R_To_L_Paning=0;//,ES_317_cnt=0;


	//static unsigned int ES_317_cnt=0;
	
	short u11_rmv_00_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[0],
		    u11_rmv_07_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[7],
	          u11_rmv_08_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[8],
                //u11_rmv_09_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[9],
                u11_rmv_10_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[10],
                u11_rmv_11_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[11],
                u11_rmv_12_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[12], 
                u11_rmv_13_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[13], 
                //u11_rmv_14_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[14], 
                u11_rmv_15_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[15], 
	          u11_rmv_16_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[16],
	          u11_rmv_17_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[17],
	          //u11_rmv_18_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[18],
	          u11_rmv_19_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[19],
	          u11_rmv_20_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[20],
		    u11_rmv_21_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[21],
		    u11_rmv_22_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[22],
		    u11_rmv_23_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[23],
		   // u11_rmv_25_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[25],

		    u11_rmv_27_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[27],
		    u11_rmv_28_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[28],		    
		    //u11_rmv_29_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[29],
		    //u11_rmv_30_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[30],
		    //u11_rmv_31_mvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[31],

		    u10_rmv_00_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[0],
		    u10_rmv_07_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[7],		    
		    u10_rmv_08_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[8],
		    //u10_rmv_09_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[9],
		    u10_rmv_10_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[10],
		    u10_rmv_11_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[11],
		    u10_rmv_12_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[12],
		    u10_rmv_13_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[13],			    
		    //u10_rmv_14_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[14],
		    u10_rmv_15_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[15],
		    
		    u10_rmv_16_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[16],
		    u10_rmv_17_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[17],
		    u10_rmv_19_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[19],

		     u10_rmv_20_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[20],
     		     u10_rmv_21_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[21],
		     u10_rmv_22_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[22],
		     u10_rmv_23_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[23],

		    //u10_rmv_25_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[25],
    		    u10_rmv_27_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[27],
    		    u10_rmv_28_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[28];
		     //u10_rmv_31_mvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[31];

	#if 1 //20200909
	static short  rmv_08_temp[120]={0},rmv_20_y_temp[120]={0};
	static short  gmv_mvx_temp[120]={0};
	static short  gmv_mvy_temp[120]={0};
	#endif
	
	unsigned int u26_aAPLp_rb; //current
	//static unsigned char	ES_Flag_317_Panning_reverse=0;
	static short rmv_08_x_delta=0;
	//static short rmv_08_x_delta_L_to_R_cnt=0,rmv_08_x_delta_R_to_L_cnt=0; // rmv_08_x_current=0,rmv_08_x_pre=0,
	static short rmv_20_y_delta=0;

//	static signed short gmv_x_delta_L_to_R_cnt=0,gmv_x_delta_R_to_L_cnt=0; // gmv_x_delta=0,rmv_08_x_current=0,rmv_08_x_pre=0,

	int u11_gmv_mvx = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	int u10_gmv_mvy = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;

	static unsigned char gmv_317_over_50_flag =0;//,gmv_rassia_flag=0;
	static unsigned int gmv_317_over_50_cnt=0;//,gmv_rassia_cnt=0;

	//static int cnt =0;
	//cnt++;

	unsigned int u25_rgnSad_rb_R[32],u20_rgnDtl_rb_R[32],u22_rTC_rb_R[32],u13_rPRD_rb_R[32] ;
	static unsigned int u25_rgnSad_rb_R_avg=0,u20_rgnDtl_rb_R_avg=0,u22_rTC_rb_R_avg=0,u13_rPRD_rb_R_avg=0;
	
	static unsigned int u25_rgnSad_rb_R_avg_temp=0,u20_rgnDtl_rb_R_avg_temp=0,u22_rTC_rb_R_avg_temp=0,u13_rPRD_rb_R_avg_temp=0;
	
	static unsigned int u25_rgnSad_rb_R_32total=0,u20_rgnDtl_rb_R_32total=0,u22_rTC_rb_R_32total=0,u13_rPRD_rb_R_32total=0;

	static unsigned int rFB_alpha_tmp_gain=0,rFB_level_tmp_gain=0;
	static unsigned int rFB_alpha_tmp_offset=0,rFB_level_tmp_offset=0,rFB_alpha_divisor=1,rFB_level_divisor=1;
	static unsigned int ES_Flag_317_Cnt=0,ES_Flag_354_Cnt=0,ES_Flag_380_Cnt=0,ES_Flag_072_1_Cnt=0,ES_Flag_072_2_Cnt=0,
			      ES_Flag_149_Cnt=0,ES_Flag_151_Cnt=0,ES_Flag_182_1_Cnt,ES_Flag_182_2_Cnt=0,ES_Flag_208_Cnt=0
			     ,ES_Flag_316_Cnt=0,ES_Flag_319_Cnt=0,ES_Flag_009_2_Cnt=0//,ES_Flag_009_1_Cnt=0
			      ,ES_Flag_358_Cnt=0 ,ES_Flag_421_Cnt=0,ES_Flag_109_Cnt=0,ES_Flag_364_Cnt=0,ES_Flag_59_Cnt=0; //,ES_Flag_219_Cnt=0
	static unsigned int  u32_RFB_ID=0;//,u32_RP_ID=0;
	static unsigned int rFB_LFB_ctrl_Test_level=0;

	unsigned int rSCss=0, rTCss =0;

	bool b_ES_Chaos=0;
	unsigned int log_en3 = 0, debug_en = 0, debug_region = 0;
    unsigned int Res_V  = 0;
	Res_V= (s_pContext->_output_rimctrl.u4_outResolution_pre == _PQL_OUT_1920x1080 || s_pContext->_output_rimctrl.u4_outResolution_pre == _PQL_OUT_3840x1080) ? 1080 : 
	                         			(s_pContext->_output_rimctrl.u4_outResolution_pre == _PQL_OUT_3840x2160) ? 2160 : 540;		

	ReadRegister(SOFTWARE2_SOFTWARE2_63_reg, 0, 31, &log_en3);
	ReadRegister(SOFTWARE2_SOFTWARE2_56_reg, 0, 0, &debug_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_57_reg, 0, 31, &debug_region);

	//static unsigned char u8_ES_rChaos_cnt=0;
	//unsigned char ES_mvDiff=0;
	//static unsigned char ES_big_relMV_cnt=0;

	//******
      //s_pContext->_output_me_sceneAnalysis.b_ES_Chaos_tmp, s_pContext->_output_me_sceneAnalysis.u8_ES_rChaos_cnt_tmp[i];
      //s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[i], s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[i]);
	//******
	
 	//ES_mvDiff=s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[i];
 	//ES_big_relMV_cnt=s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[i];

	
	 b_ES_Chaos=s_pContext->_output_me_sceneAnalysis.b_ES_Chaos_tmp;
       //u8_ES_rChaos_cnt=	 s_pContext->_output_me_sceneAnalysis.u8_ES_rChaos_cnt_tmp[i];
	
	// bool  rFB_panning_en = pParam->u1_rFB_panning_en;

	#if 0
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 0, 3, &rmv_cnt_temp);		// YE Test  0~3    0~15
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 6, 9, &rmv_unconf_temp);		// YE Test  6~9    0~15
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 10, 17, &rmv_x_temp);		// YE Test   10~17  0~255
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 18, 25, &rmv_x_delta_temp);		// YE Test    18~25  0~255
	#endif

	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 0, 0, &ES_Test_0);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 1, 1, &ES_Test_1);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 2, 2, &ES_Test_2);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 3, 3, &ES_Test_3);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 4, 4, &ES_Test_4);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 6, 6, &ES_Test_6);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 7, 7, &ES_Test_7);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 8, 8, &ES_Test_8);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 9, 9, &ES_Test_9);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 10, 10, &ES_Test_10);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 11, 11, &ES_Test_11);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 12, 12, &ES_Test_12);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 13, 13, &ES_Test_13);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 18, 19, &rFB_alpha_tmp_gain);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 20, 21, &rFB_level_tmp_gain);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 24, 24, &ES_Test_24);	
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 25, 25, &ES_Test_25);		// YE Test 
	//ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 26, 26, &ES_Test_26);		// YE Test 

	//ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 16, 23, &ES_Test_GFB);		// YE Test 

	//ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 26, 26, &ES_Test_26);		// YE Test 
	//ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 27, 27, &ES_Test_27);		// YE Test 
	
	//ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 28, 28, &ES_Test_28);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 29, 29, &ES_Test_29);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 30, 30, &ES_Test_30);		// YE Test //not use

	ReadRegister(SOFTWARE1_SOFTWARE1_23_reg, 0, 7, &rFB_alpha_tmp_gain);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_23_reg, 8, 15, &rFB_alpha_tmp_offset);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_23_reg, 16, 23, &rFB_level_tmp_gain);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_23_reg, 24, 31, &rFB_level_tmp_offset);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_24_reg, 0, 7, &rFB_alpha_divisor);		// YE Test 
	ReadRegister(SOFTWARE1_SOFTWARE1_24_reg, 8, 15, &rFB_level_divisor);		// YE Test 

	ReadRegister(SOFTWARE1_SOFTWARE1_24_reg, 16, 23, &rFB_LFB_ctrl_Test_level);		// YE Test
	
	#endif


	pOutput->u8_ES_Test_0_flag=ES_Test_0;//20200730

	
	for(i=0; i<32; i++)
	{
		u26_aAPLp_rb=s_pContext->_output_read_comreg.u26_me_aAPLp_rb;
		u25_rgnSad_rb_R[i] = s_pContext->_output_read_comreg.u25_me_rSAD_rb[i];
		u20_rgnDtl_rb_R[i] = s_pContext->_output_read_comreg.u20_me_rDTL_rb[i];
 		u22_rTC_rb_R[i] = s_pContext->_output_read_comreg.u22_me_rTC_rb[i];
		u13_rPRD_rb_R[i] = s_pContext->_output_read_comreg.u13_me_rPRD_rb[i];	
		
		rSCss = (s_pContext->_output_read_comreg.u22_me_rSC_rb[i]);  //20200907
		rTCss = (s_pContext->_output_read_comreg.u22_me_rTC_rb[i]);//20200907	
		
		//o------ calculation of rFB alpha ------o
		switch(rFB_alpha_mode)
		{
		case 0:
			tmp1 = ((s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[i] << 7) * curve_gain ); 
			tmp1 = (rFB_alpha_by_rMV_cnt_en == 0) ? tmp1 : (tmp1 / (s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[i] + 1));
			tmp1 = (tmp1 << 5) >> curve_slp;
			tmp1 = (rFB_alpha_by_dtl_en == 0) ? tmp1 : ((tmp1 << 6) / ((s_pContext->_output_read_comreg.u20_me_rDTL_rb[i] >> 6) + 1));
			tmp2 = tmp1 - curve_th0;
			rFB_alpha_tmp = _CLIP_(tmp2, curve_min, curve_max);
			break;
		case 1:
			rFB_alpha_tmp = (s_pContext->_output_read_comreg.u12_region_fb_cnt[i] > 32) ? global_fb_lvl : 0;
			break;
		case 2:
			rFB_alpha_tmp = (s_pContext->_output_read_comreg.u12_region_fb_cnt[i]==0) ? 0 : (s_pContext->_output_read_comreg.u32_region_fb_sum[i] / (s_pContext->_output_read_comreg.u12_region_fb_cnt[i] + 1));
			break;
		}

		//o------ calculation of rFB level ------o
		big_rMV = _MAX_(_ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i]), _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i]));
		big_rTC = s_pContext->_output_read_comreg.u22_me_rTC_rb[i];
		big_rMV_flag = (big_rMV > big_rMV_th) ? 1 : 0;
		big_rTC_flag = ((big_rTC>>8) > u8_th0) ? 1 : 0;

		tmp1 = s_pContext->_output_read_comreg.u32_region_fb_sum[i] / (s_pContext->_output_read_comreg.u12_region_fb_cnt[i] + 1);
		tmp1 = (tmp1 * lvl_curve_gain) >> lvl_curve_slp;
		tmp2 =  tmp1 - lvl_curve_th0;
		tmp2 = (rFB_big_rMV_en && big_rMV_flag) ? (tmp2 + big_rMV * u8_th2 / u8_th3) : tmp2;
		tmp2 = (rFB_big_rTC_en && big_rTC_flag) ? (tmp2 + big_rTC / u8_th1) : tmp2;
		rFB_level_tmp = _CLIP_(tmp2, lvl_curve_min, lvl_curve_max);
		
		//o------ region panning ------o
                #if 0 //YE Test // it's ori K7 settings
		if( rFB_panning_en && ((s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[i] >> 2) > pParam->u8_rFB_panning_cnt_thl) && (s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[i] < pParam->u8_rFB_panning_unconf_thl) )
		{
			rFB_alpha_tmp = 255;
			rFB_level_tmp = 0;
		}
                #endif
	
		rmv_x  = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i];
		rmv_y  = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i];

		if(i>0)
			rmv_x_delta = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i]-s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i-1];
		else
			rmv_x_delta = 0;
	
		rmv_cnt 		= s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[i];
	 	rmv_unconf		=s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[i] ;

		rmv_2nd_cnt        = s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[i];    //YE Test 20200731
		rmv_2nd_unconf   =  s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[i];  //YE Test 20200731
		
		  //YE Test ori 
		#if 1
		rmv_cond  =  (rmv_cnt> (pParam->u8_rFB_panning_cnt_thl << 2)) && (rmv_unconf<= pParam->u8_rFB_panning_unconf_thl) ;
		rmv_cond =   rmv_cond ||( ( rmv_cnt == 0) && (rmv_unconf < 3));
		rmv_cond =  rmv_cond &&((_ABS_(rmv_x) >80)||(_ABS_(rmv_x_delta) >110));
		#endif 
		

		//YE Test for value test
		//	WriteRegister(FRC_TOP__software1__reg_software_56_ADDR, 24, 31, 0x3f); // u8_rFB_panning_cnt_thl
		//	WriteRegister(FRC_TOP__software1__reg_software_56_ADDR, 16, 23, 0x1f); // u8_rFB_panning_unconf_thl

//		rmv_cond  =  (rmv_cnt> (pParam->u8_rFB_panning_cnt_thl << 2)) && (rmv_unconf<= pParam->u8_rFB_panning_unconf_thl) ;
		#if 0
		rmv_cond  =  (rmv_cnt> (pParam->u8_rFB_panning_cnt_thl << 2)) && (rmv_unconf<= pParam->u8_rFB_panning_unconf_thl) ;
		rmv_cond =   rmv_cond ||( ( rmv_cnt == rmv_cnt_temp) && (rmv_unconf < rmv_unconf_temp));  //it's strange   364
		rmv_cond =  rmv_cond &&((_ABS_(rmv_x) >rmv_x_temp)||(_ABS_(rmv_x_delta) >rmv_x_delta_temp));
		#endif
		/*if(pParam->u8_rmv_alpha_curve_gain==1)
			{
				if(  (panning_flag>0) ||(_ABS_(rmv_x) >80)||(_ABS_(rmv_x_delta) >110)||((s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[i] == 0) && (s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[i] < 3)) )
		{
			rFB_alpha_tmp = 255;
			rFB_level_tmp = 0;
		}
			}
		*/
		//if(pParam->u8_rmv_alpha_curve_gain==1)
		//{

		#if 1
				#if 1
				if(ES_Test_11==1)
				{
				//	rtd_pr_memc_debug("[YE_MEMC_RFB_A] ,rFB_alpha_tmp=%d, rFB_alpha_tmp_gain=%d ,rFB_alpha_tmp_offset=%d,rFB_level_tmp=%d,rFB_level_tmp_gain=%d,rFB_level_tmp_offset=%d,\n"
				//	,rFB_alpha_tmp,rFB_alpha_tmp_gain,rFB_alpha_tmp_offset,rFB_level_tmp,rFB_level_tmp_gain,rFB_level_tmp_offset);
				 
				}
				#endif


			if(ES_Test_0==0)	
			{
			rFB_alpha_tmp=rFB_alpha_tmp*(rFB_alpha_tmp_gain/rFB_alpha_divisor)+rFB_alpha_tmp_offset;
			rFB_level_tmp=rFB_level_tmp*(rFB_level_tmp_gain/rFB_level_divisor)+rFB_level_tmp_offset;
			//pOutput->u8_ES_Test_0_flag=0; 
			pOutput->u8_ES_flag=0;
			pOutput->u8_ES_DEHALO_flag=0;
			//pOutput->u8_rFB_LFB_ctrl_level=rFB_LFB_ctrl_Test_level;
			#if 1
			if (rFB_LFB_ctrl_Test_level==0)  // level 0~5 LFB only : 6~10 LFB+RFB  : 11~15 RFB only
					{
						pOutput->u8_rFB_LFB_ctrl_level=0; //it's ori default value
						rFB_alpha_tmp=0;  //do LFB only
						rFB_level_tmp=0;  //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==1)
					{
						pOutput->u8_rFB_LFB_ctrl_level=1;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==2)
					{
						pOutput->u8_rFB_LFB_ctrl_level=2;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==3)
					{
						pOutput->u8_rFB_LFB_ctrl_level=3;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==4)
					{
						pOutput->u8_rFB_LFB_ctrl_level=4;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==5)
					{
						pOutput->u8_rFB_LFB_ctrl_level=5;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==6)
					{
						pOutput->u8_rFB_LFB_ctrl_level=6;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==7)
					{
						pOutput->u8_rFB_LFB_ctrl_level=7;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==8)
					{
						pOutput->u8_rFB_LFB_ctrl_level=8;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}				
				else if(rFB_LFB_ctrl_Test_level==9)
					{
						pOutput->u8_rFB_LFB_ctrl_level=9;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==10)
					{
						pOutput->u8_rFB_LFB_ctrl_level=10;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==11)
					{
						pOutput->u8_rFB_LFB_ctrl_level=0;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==12)
					{
						pOutput->u8_rFB_LFB_ctrl_level=1;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==13)
					{
						pOutput->u8_rFB_LFB_ctrl_level=2;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==14)
					{
						pOutput->u8_rFB_LFB_ctrl_level=3;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==15)
					{
						pOutput->u8_rFB_LFB_ctrl_level=4;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==16)
					{
						pOutput->u8_rFB_LFB_ctrl_level=5;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==17)
					{
						pOutput->u8_rFB_LFB_ctrl_level=6;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==18)
					{
						pOutput->u8_rFB_LFB_ctrl_level=7;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==19)
					{
						pOutput->u8_rFB_LFB_ctrl_level=8;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==20)
					{
						pOutput->u8_rFB_LFB_ctrl_level=9;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==21)
					{
						pOutput->u8_rFB_LFB_ctrl_level=10;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==22)
					{
						pOutput->u8_rFB_LFB_ctrl_level=0;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==23)
					{
						pOutput->u8_rFB_LFB_ctrl_level=1;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==24)
					{
						pOutput->u8_rFB_LFB_ctrl_level=2;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==25)
					{
						pOutput->u8_rFB_LFB_ctrl_level=3;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==26)
					{
						pOutput->u8_rFB_LFB_ctrl_level=4;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==27)
					{
						pOutput->u8_rFB_LFB_ctrl_level=5;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==28)
					{
						pOutput->u8_rFB_LFB_ctrl_level=6;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==29)
					{
						pOutput->u8_rFB_LFB_ctrl_level=7;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==30)
					{
						pOutput->u8_rFB_LFB_ctrl_level=8;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==31)
					{
						pOutput->u8_rFB_LFB_ctrl_level=9;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==32)
					{
						pOutput->u8_rFB_LFB_ctrl_level=10;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==128) //disable LFB
					{
						rFB_alpha_tmp=255; 
						rFB_level_tmp=0; 
					}
				#endif

			


				
			/*
			if(((rFB_alpha_tmp_gain==127)&&(rFB_alpha_tmp_offset==255))||(rFB_alpha_tmp>=255))
			rFB_alpha_tmp=255;

			if(((rFB_level_tmp_gain==127)&&(rFB_level_tmp_offset==255))||(rFB_level_tmp>=255))
			rFB_level_tmp=255;
			*/
			}
		#endif


			#if 0 //it just for Test for level detection only , use original RFB case and level control

			
			if (rFB_LFB_ctrl_Test_level==0)  // level 0~5 LFB only : 6~10 LFB+RFB  : 11~15 RFB only
					{
						pOutput->u8_rFB_LFB_ctrl_level=0; //it's ori default value
						rFB_alpha_tmp=0;  //do LFB only
						rFB_level_tmp=0;  //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==1)
					{
						pOutput->u8_rFB_LFB_ctrl_level=1;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==2)
					{
						pOutput->u8_rFB_LFB_ctrl_level=2;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==3)
					{
						pOutput->u8_rFB_LFB_ctrl_level=3;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==4)
					{
						pOutput->u8_rFB_LFB_ctrl_level=4;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==5)
					{
						pOutput->u8_rFB_LFB_ctrl_level=5;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==6)
					{
						pOutput->u8_rFB_LFB_ctrl_level=6;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==7)
					{
						pOutput->u8_rFB_LFB_ctrl_level=7;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==8)
					{
						pOutput->u8_rFB_LFB_ctrl_level=8;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}				
				else if(rFB_LFB_ctrl_Test_level==9)
					{
						pOutput->u8_rFB_LFB_ctrl_level=9;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==10)
					{
						pOutput->u8_rFB_LFB_ctrl_level=10;
						rFB_alpha_tmp=0; //do LFB only
						rFB_level_tmp=0; //do LFB only
					}
				else if(rFB_LFB_ctrl_Test_level==11)
					{
						pOutput->u8_rFB_LFB_ctrl_level=0;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==12)
					{
						pOutput->u8_rFB_LFB_ctrl_level=1;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==13)
					{
						pOutput->u8_rFB_LFB_ctrl_level=2;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==14)
					{
						pOutput->u8_rFB_LFB_ctrl_level=3;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==15)
					{
						pOutput->u8_rFB_LFB_ctrl_level=4;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==16)
					{
						pOutput->u8_rFB_LFB_ctrl_level=5;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==17)
					{
						pOutput->u8_rFB_LFB_ctrl_level=6;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==18)
					{
						pOutput->u8_rFB_LFB_ctrl_level=7;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==19)
					{
						pOutput->u8_rFB_LFB_ctrl_level=8;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==20)
					{
						pOutput->u8_rFB_LFB_ctrl_level=9;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==21)
					{
						pOutput->u8_rFB_LFB_ctrl_level=10;
						rFB_alpha_tmp=128; 
					}
				else if(rFB_LFB_ctrl_Test_level==22)
					{
						pOutput->u8_rFB_LFB_ctrl_level=0;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==23)
					{
						pOutput->u8_rFB_LFB_ctrl_level=1;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==24)
					{
						pOutput->u8_rFB_LFB_ctrl_level=2;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==25)
					{
						pOutput->u8_rFB_LFB_ctrl_level=3;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==26)
					{
						pOutput->u8_rFB_LFB_ctrl_level=4;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==27)
					{
						pOutput->u8_rFB_LFB_ctrl_level=5;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==28)
					{
						pOutput->u8_rFB_LFB_ctrl_level=6;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==29)
					{
						pOutput->u8_rFB_LFB_ctrl_level=7;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==30)
					{
						pOutput->u8_rFB_LFB_ctrl_level=8;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==31)
					{
						pOutput->u8_rFB_LFB_ctrl_level=9;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==32)
					{
						pOutput->u8_rFB_LFB_ctrl_level=10;
						rFB_alpha_tmp=255; 
					}
				else if(rFB_LFB_ctrl_Test_level==128) //disable LFB
					{
						rFB_alpha_tmp=255; 
						rFB_level_tmp=0; 
					}
				

			if((rmv_2nd_unconf>4000)&&(rmv_x_delta>1))
				{
					rFB_LFB_ctrl_Test_level=8;					
				}
			else if(rmv_2nd_unconf<=4000)
				{
					rFB_LFB_ctrl_Test_level=128;
				}
			else
				{
					rFB_LFB_ctrl_Test_level=128;
				}
			#endif	


#if RTK_MEMC_Performance_tunging_from_tv001
ES_Test_4=0; //YE Test TV002 doesn't need it
#endif


if(ES_Test_4==1)	 //YE Test to control ES Enable/Disable
	{	


		#if 0  //mark it all to avoid TV006 doesn't accept judder
		//u8_RFB_ID = 99
		if((((u11_gmv_mvx<100)&&(u11_gmv_mvx>50))
		&&((u10_gmv_mvy<50)&&(u10_gmv_mvy>15)))
		&&(gmv_rassia_cnt==0))
		{
			gmv_rassia_cnt=3000;
		}
		else if(gmv_rassia_cnt>0)
		{
			gmv_rassia_cnt--;
		}
		
		if(gmv_rassia_cnt>0)
			gmv_rassia_flag=1;
		else
			gmv_rassia_flag=0;
		#endif

	#if 0 //mark it all to avoid TV006 doesn't accept judder
		if((u25_rgnSad_rb>15000)
		&&((u20_rgnDtl_rb<80000)&&(u20_rgnDtl_rb>60000))
		&&(gmv_rassia_flag==1)
		&&((u26_aAPLp_rb<6200000)&&(u26_aAPLp_rb>4700000))
		&&(BOT_ratio==0))
		{
		Rassia_Flag=1;
		u32_RFB_ID=99;
		}
	else {
		Rassia_Flag=0;
		if(BOT_ratio !=0)
			{
			pOutput->u8_ES_Rassia_ID_flag=0;
			
			}
		}
	#endif



#if 1 //20200914

		if((u25_rgnSad_rb>2400)
		&&((u20_rgnDtl_rb<50000)&&(u20_rgnDtl_rb>5000))
	 	&&((u25_rgnSad_rb_R[10]<60000)&&(u25_rgnSad_rb_R[10]>0))
		&&((u20_rgnDtl_rb_R[10]<50000)&&(u20_rgnDtl_rb_R[10]>2000))
		&&((u13_rPRD_rb_R[10]<400)&&(u13_rPRD_rb_R[10]>0))		
	 	&&((u25_rgnSad_rb_R[11]<100000)&&(u25_rgnSad_rb_R[11]>0))
		&&((u20_rgnDtl_rb_R[11]<50000)&&(u20_rgnDtl_rb_R[11]>2500))
		&&((u13_rPRD_rb_R[11]<400)&&(u13_rPRD_rb_R[11]>0))
	 	&&((u25_rgnSad_rb_R[16]<40000)&&(u25_rgnSad_rb_R[16]>1000))
		&&((u20_rgnDtl_rb_R[16]<50000)&&(u20_rgnDtl_rb_R[16]>2000))
		&&((u13_rPRD_rb_R[16]<400)&&(u13_rPRD_rb_R[16]>0))		
		&&((u11_rmv_08_mvx<-30)&&(u11_rmv_08_mvx>-190))
		&&((u10_rmv_08_mvy<10)&&(u10_rmv_08_mvy>-10))
		&&((u11_rmv_10_mvx<80)&&(u11_rmv_10_mvx>-160))
		&&((u10_rmv_10_mvy<50)&&(u10_rmv_10_mvy>-30))
		&&((u11_rmv_11_mvx<-30)&&(u11_rmv_11_mvx>-190))
		&&((u10_rmv_11_mvy<40)&&(u10_rmv_11_mvy>-30))				
		//&&(u10_gmv_mvy==0)
		//&&((u11_gmv_mvx<100)&&(u11_gmv_mvx>-150))	
		//&&((u10_gmv_mvy<40)&&(u10_gmv_mvy>-30))		
		&&((u11_gmv_mvx<100)&&(u11_gmv_mvx>-50))	//20200914
		&&((u10_gmv_mvy<30)&&(u10_gmv_mvy>-30))	//20200914		
		&&(u13_rPRD_rb<=320)
		&&((u26_aAPLp_rb<2700000)&&(u26_aAPLp_rb>900000))
		&&(BOT_ratio==140)
		&&((rSCss<=90000)&&(rSCss>=5000)) //r11
		&& ((rTCss<=10000)&&(rTCss>=1000)) //r11		
		&&((s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[11]<=160)&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[11]>=5)) //r11
		&&((s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[11]<=12)&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[11]>=4))//r11
		&&((rmv_cnt<1100)&&(rmv_cnt>100)) //r11
		&&((rmv_2nd_cnt<500)&&(rmv_2nd_cnt>0))//r11
		&&((rmv_unconf<26)&&(rmv_unconf>0))//r11
		&&((rmv_2nd_unconf<4096)&&(rmv_2nd_unconf>0))) //r11
		//&&((BOT_ratio<142)&&(BOT_ratio>138)))
		{
		ES_Flag_421=1;
		u32_RFB_ID=421;
		ES_Flag_421_Cnt=2500;
		}
	else if (ES_Flag_421_Cnt>0)
		{
		ES_Flag_421=1;
		u32_RFB_ID=421;
		ES_Flag_421_Cnt--;
		}
	//else if((ES_Flag_421_Cnt>0)||((BOT_ratio<138)||(BOT_ratio>142))) 
	else if((ES_Flag_421_Cnt>0)||(BOT_ratio!=140)) 	
		{
		ES_Flag_421=0;
		pOutput->u8_ES_421_ID_flag=0;
		}
#endif


#if 0      //mark it all to avoid TV006 doesn't accept judder   // 20200910 conflic with #72
		if(((u20_rgnDtl_rb<38000)&&(u20_rgnDtl_rb>5000))
		&&(((u11_rmv_17_mvx<200)&&(u11_rmv_17_mvx>100))
		||((u11_rmv_18_mvx<200)&&(u11_rmv_18_mvx>100))
		||((u11_rmv_19_mvx<140)&&(u11_rmv_19_mvx>80)))
		&&((u13_rPRD_rb<=250)&&(u13_rPRD_rb>50))
		&&((u26_aAPLp_rb<1400000)&&(u26_aAPLp_rb>1000000))
		&&(BOT_ratio==0))
		{
		ES_Flag_413=1;
		u32_RFB_ID=413;
		}
	else{
		ES_Flag_413=0;
		if(BOT_ratio!=0) 
			{
			pOutput->u8_ES_413_ID_flag=0;
			}
		}
#endif



	//	ES_Flag_21 021_Spider 
	#if 0  //mark it all to avoid TV006 doesn't accept judder
	if   ((u25_rgnSad_rb<400)
		&&((u20_rgnDtl_rb<13500)&&(u20_rgnDtl_rb>7900))
		&&(((u11_rmv_17_mvx<-30)&&(u11_rmv_17_mvx>-110))
		||((u11_rmv_18_mvx<-30)&&(u11_rmv_18_mvx>-110))
		||((u11_rmv_19_mvx<-30)&&(u11_rmv_19_mvx>-110))
		||((u11_rmv_20_mvx<-30)&&(u11_rmv_20_mvx>-110))
		||((u11_rmv_21_mvx<-30)&&(u11_rmv_21_mvx>-110))
		||((u11_rmv_22_mvx<-30)&&(u11_rmv_22_mvx>-110)))
		&&((u11_rmv_15_mvx<5)&&(u11_rmv_15_mvx>-30))
		&&(u13_rPRD_rb<=30)
		&&((u26_aAPLp_rb<2000000)&&(u26_aAPLp_rb>1600000))
		&&((BOT_ratio<145)&&(BOT_ratio>135)))
		{
		ES_Flag_21=1;
		u32_RFB_ID=21;
		}
	else{
		ES_Flag_21=0;
			if((BOT_ratio<135)||(BOT_ratio>145))
				{
				pOutput->u8_ES_21_ID_flag=0;
				}
		}
	#endif

	//ES_Flag_21=ES_Test_27;


#if 1 //20200909
		if(((u25_rgnSad_rb_R[11]<70000)&&(u25_rgnSad_rb_R[11]>0))
		&&((u20_rgnDtl_rb_R[11]<80000)&&(u20_rgnDtl_rb_R[11]>0))
		//&&((u13_rPRD_rb_R[11]<600)&&(u13_rPRD_rb_R[11]>0))	
		&&((u25_rgnSad_rb_R[12]<70000)&&(u25_rgnSad_rb_R[12]>0))
		&&((u20_rgnDtl_rb_R[12]<90000)&&(u20_rgnDtl_rb_R[12]>0))
		//&&((u13_rPRD_rb_R[12]<650)&&(u13_rPRD_rb_R[12]>0))				
	 	&&((u25_rgnSad_rb_R[13]<10000)&&(u25_rgnSad_rb_R[13]>0))
		&&((u20_rgnDtl_rb_R[13]<20000)&&(u20_rgnDtl_rb_R[13]>0))
		//&&((u13_rPRD_rb_R[13]<500)&&(u13_rPRD_rb_R[13]>0))
		&&((u25_rgnSad_rb_R_avg<250000)&&(u25_rgnSad_rb_R_avg>0))
		&&((u20_rgnDtl_rb_R_avg<23000)&&(u20_rgnDtl_rb_R_avg>10000))
		&&((u13_rPRD_rb_R_avg<120)&&(u13_rPRD_rb_R_avg>30))
		&&((u11_rmv_11_mvx<30)&&(u11_rmv_11_mvx>-70))
		&&((u11_rmv_12_mvx<30)&&(u11_rmv_12_mvx>-35))
		&&((u11_rmv_13_mvx<35)&&(u11_rmv_13_mvx>-65))
		&&((u10_rmv_11_mvy<7)&&(u10_rmv_11_mvy>-8))		
		&&((u10_rmv_12_mvy<20)&&(u10_rmv_12_mvy>-20))
		&&((u10_rmv_13_mvy<20)&&(u10_rmv_13_mvy> -30))
 		&&((u11_gmv_mvx<8)&&(u11_gmv_mvx>-4))
		//&&((u10_gmv_mvy<10)&&(u10_gmv_mvy>-10))
		&&(u10_gmv_mvy==0)
		&&((u26_aAPLp_rb<1200000)&&(u26_aAPLp_rb>800000))
		//&&((BOT_ratio<141)&&(BOT_ratio>135))
		&&(BOT_ratio==138)
		//&&(rSCss<=15) && (rTCss==0) //r11
		//&&((rSCss<=15000)&&(rSCss>=0)) //r11
		//&& ((rTCss<=5000)&&(rTCss>=500)) //r11
		&&((rSCss<=12500)&&(rSCss>=2500)) //r11
		&& ((rTCss<=3000)&&(rTCss>=500)) //r11		
		&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[11]<=50)//&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[11]>=0)) //r11
		&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[11]<=11))//&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[11]>=0))) //r11
		//&&(b_ES_Chaos==1)
		//&&((s_pContext->_output_me_sceneAnalysis.u8_ES_rChaos_cnt_tmp[11]<=15)&&(s_pContext->_output_me_sceneAnalysis.u8_ES_rChaos_cnt_tmp[11]>=2))) //not working
		//&&((u8_ES_rChaos_cnt<=15)&&(u8_ES_rChaos_cnt>=2))) //r11
		{
			ES_Flag_380=1;
			u32_RFB_ID=380;
			ES_Flag_380_Cnt=2500;
		}
	else if (ES_Flag_380_Cnt>0)
		{
			ES_Flag_380=1;
			u32_RFB_ID=380;
			ES_Flag_380_Cnt--;
		}
	//else if((ES_Flag_380_Cnt==0)||((BOT_ratio>141)||(BOT_ratio<135)))
	else if((ES_Flag_380_Cnt==0)||(BOT_ratio!=138))		
		{
			ES_Flag_380=0;
		}
	 	
#endif


#if 1  //20200909
	if(((u25_rgnSad_rb<80000)&&(u20_rgnDtl_rb>7500))//20200522 change for K7
		&&((u11_rmv_08_mvx<-30)&&(u11_rmv_08_mvx>-250))
		&&((u10_rmv_08_mvy<30)&&(u10_rmv_08_mvy>-40))
		&&((u11_rmv_16_mvx<-30)&&(u11_rmv_16_mvx>-250))
		&&((u10_rmv_16_mvy<35)&&(u10_rmv_16_mvy>-25))		
		&&((u11_rmv_20_mvx<50)&&(u11_rmv_20_mvx>-110)) 
		&&((u10_rmv_20_mvy<60)&&(u10_rmv_20_mvy>-45))			
		&&(u13_rPRD_rb<240)
		&&((u25_rgnSad_rb_R[19]<120000)&&(u25_rgnSad_rb_R[19]>5000))
		&&((u20_rgnDtl_rb_R[19]<55000)&&(u20_rgnDtl_rb_R[19]>10000))
		&&((u13_rPRD_rb_R[19]<400)&&(u13_rPRD_rb_R[19]>20))		
		&&((u26_aAPLp_rb<3800000)&&(u26_aAPLp_rb>2400000))
		&&((u11_gmv_mvx<-45)&&(u11_gmv_mvx>-200))	
		&&((u10_gmv_mvy<20)&&(u10_gmv_mvy>-20))		
		&&(rSCss<=60000) //r19
		&& ((rTCss<=10000)&&(rTCss>=2000)) //r19
		&&((s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[19]<=155)&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[19]>=10)) //r19
		&&((s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[19]<=18)&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[19]>=6))//r19		
		&&(BOT_ratio==140))
		{
		ES_Flag_364=1;
		u32_RFB_ID=364;
		ES_Flag_364_Cnt=2000;
		}
	else if (ES_Flag_364_Cnt>0)
		{
		ES_Flag_364=1;
		u32_RFB_ID=364;
		ES_Flag_364_Cnt--;
		}
	else if ((ES_Flag_364_Cnt==0)||(BOT_ratio!=140))
		{
		ES_Flag_364=0;
		}
#endif



	//YE Test K7 sometimes it doesn't work due to MV value is unstable GFB ?
	if(((u20_rgnDtl_rb<87500)&&(u20_rgnDtl_rb>20000))
		&&((u11_rmv_08_mvx<125)&&(u11_rmv_08_mvx>80))
		&&((u11_rmv_16_mvx<130)&&(u11_rmv_16_mvx>80))
		&&((u11_rmv_20_mvx<120)&&(u11_rmv_20_mvx>10))
		&&((u26_aAPLp_rb<1745000)&&(u26_aAPLp_rb>700000))
		&&((BOT_ratio<83)&&(BOT_ratio>63)))
		{
		ES_Flag_354=1;
		u32_RFB_ID=354;
		ES_Flag_354_Cnt=2000;
		}
	else if (ES_Flag_354_Cnt>0)
		{
		ES_Flag_354=1;
		u32_RFB_ID=354;
		ES_Flag_354_Cnt--;
		}
	else if ((ES_Flag_354_Cnt==0)||((BOT_ratio>83)||(BOT_ratio<63)))
		{
		ES_Flag_354=0;
		}





	#if 0   //mark it all to avoid tv006 doesn't accept judder //YE Test temp mark


#if RTK_MEMC_Performance_tunging_from_tv001
	if(((BOT_ratio<174)&&(BOT_ratio>154))&&((u11_rmv_12_mvx<-230)&&(u11_rmv_12_mvx>-320)))
#else
	if(((BOT_ratio<174)||(BOT_ratio>154))&&((u11_rmv_12_mvx<-230)&&(u11_rmv_12_mvx>-320)))
#endif		
		{
			ES_Flag_439=1;
			u32_RFB_ID=439;
		}
		else	{
			ES_Flag_439=0;
			if((BOT_ratio<154)||(BOT_ratio>174))
				{
					pOutput->u8_ES_439_ID_flag=0;	
					//u32_RFB_ID=0;
				}
		}
	#endif


		//if(ES_Flag_317_Panning_reverse==1)
		/*
	if((_ABS_(rmv_08_x_delta)>80)
		&&(u11_rmv_08_mvx>-200))
	*/

	if((u11_gmv_mvx>50)&&(gmv_317_over_50_cnt==0))
		{
		gmv_317_over_50_cnt=1500;
		}
	else if(gmv_317_over_50_cnt>0)
		{
		gmv_317_over_50_cnt--;
		}
	else //20200908
		{
			gmv_317_over_50_cnt=0;
		}
	
	if(gmv_317_over_50_cnt>0)
		gmv_317_over_50_flag=1;
	else
		gmv_317_over_50_flag=0;
	
	//if((_ABS_(rmv_08_x_delta)>80)
		//&&(u11_rmv_08_mvx>-220)
		
	if((_ABS_(rmv_08_x_delta)>25)
		&&(u11_rmv_08_mvx>-220)		
		&&(gmv_317_over_50_flag==1))
		//&&((u11_gmv_mvx<-1)&&(u10_gmv_mvy>5)))
		{	
		ES_Flag_317_temp=1;
		}
	else {
		ES_Flag_317_temp=0;
		}

	if(((u25_rgnSad_rb<72000)&&(u25_rgnSad_rb>3000))
		&&((u20_rgnDtl_rb<25000)&&(u20_rgnDtl_rb>8000))
		&&(ES_Flag_317_temp==1)
		&&((u26_aAPLp_rb<3000000)&&(u26_aAPLp_rb>1500000))
		&&(u13_rPRD_rb<70)
		//&&((BOT_ratio<140)||(BOT_ratio>136))
		&&(BOT_ratio==138)
		&&((u25_rgnSad_rb_R[19]<70000)&&(u25_rgnSad_rb_R[19]>3000))
		&&((u20_rgnDtl_rb_R[19]<35000)&&(u20_rgnDtl_rb_R[19]>0))
		&&(u13_rPRD_rb_R[19]<250)
		&&((u25_rgnSad_rb_R[20]<95000)&&(u25_rgnSad_rb_R[20]>3000))
		&&((u20_rgnDtl_rb_R[20]<45000)&&(u20_rgnDtl_rb_R[20]>0))
		&&(u13_rPRD_rb_R[20]<300)
		&&((u11_rmv_19_mvx<150)&&(u11_rmv_19_mvx>0))
		&&((u11_rmv_20_mvx<150)&&(u11_rmv_20_mvx>0))
		&&((u10_rmv_19_mvy<20)&&(u10_rmv_19_mvy>-60))
		&&((u10_rmv_20_mvy<20)&&(u10_rmv_20_mvy> -60))		
		//&&((u11_gmv_mvx<135)&&(u11_gmv_mvx>35))	
		//&&((u10_gmv_mvy<5)&&(u10_gmv_mvy>-40))		
		&&((u11_gmv_mvx<135)&&(u11_gmv_mvx>30))	
		&&((u10_gmv_mvy<5)&&(u10_gmv_mvy>-40))				
		&&((rSCss<=40000)&&(rSCss>=3000)) //r19
		&& ((rTCss<=20000)&&(rTCss>=2000)) //r19
		&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[19]<=55)//&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[19]>=0)) //r19
		&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[19]<=19))//&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[19]>=0)))//r19
		//&&(BOT_ratio==138))
		{
		ES_Flag_317=1;
		u32_RFB_ID=317;
		ES_Flag_317_Cnt=2800;
		}
	else if(ES_Flag_317_Cnt>0)
		{
		   ES_Flag_317=1;
		   u32_RFB_ID=317;
		   ES_Flag_317_Cnt--;
		}
	//else if((ES_Flag_317_Cnt==0)||((BOT_ratio>140)||(BOT_ratio<136))){
	else if((ES_Flag_317_Cnt==0)||((BOT_ratio!=138))){		
		ES_Flag_317_Cnt=0;
		ES_Flag_317=0;
		gmv_317_over_50_cnt=0; //20200908
		}


	 if((u25_rgnSad_rb_R[i]<320000)
		&&(u20_rgnDtl_rb_R[i]<100000)//&&(u20_rgnDtl_rb_R[i]>20000))
		&&((u26_aAPLp_rb<4200000)&&(u26_aAPLp_rb>3400000))
		&&(s_pContext->_output_me_sceneAnalysis.u2_panning_flag==1)
		&&(u11_gmv_mvx==0)
		&&((u10_gmv_mvy<-10)&&(u10_gmv_mvy>-38))
		&&(BOT_ratio<3))
		{
		ES_Flag_040=1;
		u32_RFB_ID=40;
		}
	else{
		ES_Flag_040=0;
		}


#if 1 //20200911 rerune to fix conflict with #72
	if(1)  
		{
	      if(((u25_rgnSad_rb_R[11]<110000)&&(u25_rgnSad_rb_R[11]>3000))
		&&((u20_rgnDtl_rb_R[11]<25000)&&(u20_rgnDtl_rb_R[11]>7000))
		&&((u13_rPRD_rb_R[11]<200)&&(u13_rPRD_rb_R[11]>60))
	      &&((u25_rgnSad_rb_R[12]<200000)&&(u25_rgnSad_rb_R[12]>1000))
		&&((u20_rgnDtl_rb_R[12]<25000)&&(u20_rgnDtl_rb_R[12]>7000))
		&&((u13_rPRD_rb_R[12]<200)&&(u13_rPRD_rb_R[12]>60))
		&&(u25_rgnSad_rb_R[0]<2) &&(u20_rgnDtl_rb_R[0]<2)&&(u13_rPRD_rb_R[0]<2)
	      &&(u25_rgnSad_rb_R[7]<2)&&(u20_rgnDtl_rb_R[7]<2)&&(u13_rPRD_rb_R[7]<2)
		&&(u25_rgnSad_rb_R[24]<2)&&(u20_rgnDtl_rb_R[24]<2)&&(u13_rPRD_rb_R[24]<2)		
	      	//&&(u25_rgnSad_rb_R[31]<2)&&(u20_rgnDtl_rb_R[31]<2)&&(u13_rPRD_rb_R[31]<2)		     	
	      	//&&((u26_aAPLp_rb<4360000)&&(u26_aAPLp_rb>4320000))
      	 	&&((u11_rmv_00_mvx<70)&&(u11_rmv_00_mvx> -20))
		&&((u11_rmv_07_mvx<70)&&(u11_rmv_07_mvx> -20))		
		&&((u11_rmv_11_mvx<10)&&(u11_rmv_11_mvx> -55))
		&&((u11_rmv_12_mvx<30)&&(u11_rmv_12_mvx> -30))		
		&&((u10_rmv_00_mvy<100)&&(u10_rmv_00_mvy> -20))
		&&((u10_rmv_07_mvy<100)&&(u10_rmv_07_mvy> -20))		
		&&((u10_rmv_11_mvy<40)&&(u10_rmv_11_mvy> -30))
		&&((u10_rmv_12_mvy<10)&&(u10_rmv_12_mvy> -60))		
		&&((u11_gmv_mvx<10)&&(u11_gmv_mvx>-30))
		&&((u10_gmv_mvy<15)&&(u10_gmv_mvy>-10))
		&&((rSCss<=85000)&&(rSCss>=4000)) //r11
		&& ((rTCss<=45000)&&(rTCss>=2000)) //r11
		&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[11]<=80)//&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[11]>=0)) //r11
		&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[11]<=12)//&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[11]>=0))//r11		
		&&(BOT_ratio<2)) //new add
		{
		ES_Flag_059=1;
		u32_RFB_ID=59;
		ES_Flag_59_Cnt=1000;
		}
		else if (ES_Flag_59_Cnt>0)
		{
			ES_Flag_059=1;
			u32_RFB_ID=59;
			ES_Flag_59_Cnt--;
		}
		else if((ES_Flag_59_Cnt==0)||(BOT_ratio>=2))
		{
			ES_Flag_059=0;
		}
		}	
#endif



	      if((u25_rgnSad_rb_R[i]<100000)
		&&((u20_rgnDtl_rb_R[i]<75000)&&(u13_rPRD_rb_R[i]<600))
		&&((u26_aAPLp_rb<5800000)&&(u26_aAPLp_rb>5200000))
		&&((u20_rgnDtl_rb<7000)&&(u20_rgnDtl_rb>4000))
		&&(u13_rPRD_rb<60)
		&&(((u11_rmv_21_mvx<130)&&(u11_rmv_21_mvx>100))
		||((u11_rmv_22_mvx<130)&&(u11_rmv_22_mvx>100))
		||((u11_rmv_23_mvx<130)&&(u11_rmv_23_mvx>100)))
		&&(u10_gmv_mvy<2))
		{
		ES_Flag_043=1;
		u32_RFB_ID=43;
		}
	else{
		ES_Flag_043=0;
		}

#if 0 // YE Test don't need it , video is ok
	      if((u25_rgnSad_rb_R[i]<200000)
		&&((u20_rgnDtl_rb_R[i]<60000)&&(u13_rPRD_rb_R[i]<400))
		&&((u26_aAPLp_rb<5000000)&&(u26_aAPLp_rb>3000000))
		&&((u20_rgnDtl_rb<50000)&&(u20_rgnDtl_rb>30000))
		&&(u13_rPRD_rb<200)
		&&(u11_rmv_23_mvx<190)
		&&(u11_rmv_31_mvx<50)
		&&((u10_rmv_25_mvy<3)&&(u10_rmv_25_mvy>-2))
		&&((u10_rmv_31_mvy<3)&&(u10_rmv_31_mvy>-2))
		&&(u11_gmv_mvx<150)
		&&((u10_gmv_mvy<2)&&(u10_gmv_mvy>-6))
		&&(BOT_ratio<3)) //new add
		{
		ES_Flag_035=1;
		u32_RFB_ID=35;
		}
	else{
		ES_Flag_035=0;
		}
#endif

#if 0
		//20200722 fix
	      if(((u25_rgnSad_rb_R[8]<60000)&&(u25_rgnSad_rb_R[8]>0))
		&&((u20_rgnDtl_rb_R[8]<60000)&&(u20_rgnDtl_rb_R[8]>5000))
		&&((u13_rPRD_rb_R[8]<450)&&(u13_rPRD_rb_R[8]>0))
		&&((u25_rgnSad_rb_R[9]<60000)&&(u25_rgnSad_rb_R[9]>0))
		&&((u20_rgnDtl_rb_R[9]<60000)&&(u20_rgnDtl_rb_R[9]>5000))
		&&((u13_rPRD_rb_R[9]<450)&&(u13_rPRD_rb_R[9]>0))
		&&((u25_rgnSad_rb_R_avg<140000)&&(u25_rgnSad_rb_R_avg>10000))
		&&((u20_rgnDtl_rb_R_avg<45000)&&(u20_rgnDtl_rb_R_avg>20000))
		&&((u13_rPRD_rb_R_avg<280)&&(u13_rPRD_rb_R_avg>100))
		&&((u11_rmv_08_mvx<15)&&(u11_rmv_08_mvx>-80))
		&&((u11_rmv_09_mvx<15)&&(u11_rmv_09_mvx>-80))
		&&((u10_rmv_08_mvy<60)&&(u10_rmv_08_mvy>0))
		&&((u10_rmv_09_mvy<50)&&(u10_rmv_09_mvy> -20))
 		&&((u11_gmv_mvx<30)&&(u11_gmv_mvx>-125))
		&&((u10_gmv_mvy<80)&&(u10_gmv_mvy>-45))
		&&((BOT_ratio<10)&&(BOT_ratio>7)))
		{
			ES_Flag_009_1=1;
			u32_RFB_ID=901;
			ES_Flag_009_1_Cnt=1200;
		}
	else if (ES_Flag_009_1_Cnt>0)
		{
			ES_Flag_009_1=1;
			u32_RFB_ID=901;
			ES_Flag_009_1_Cnt--;
		}
	else if((ES_Flag_009_1_Cnt==0)||((BOT_ratio>10)||(BOT_ratio<7)))
		{
			ES_Flag_009_1_Cnt=0;
		}
#endif

		//20200722 modify detection
	      if(((u25_rgnSad_rb_R[23]<160000)&&(u25_rgnSad_rb_R[23]>0))
		&&((u20_rgnDtl_rb_R[23]<70000)&&(u20_rgnDtl_rb_R[23]>8000))
		&&((u13_rPRD_rb_R[23]<550))
		&&((u26_aAPLp_rb<4200000)&&(u26_aAPLp_rb>3700000))
		&&((u20_rgnDtl_rb<70000)&&(u20_rgnDtl_rb>30000))
		&&(u13_rPRD_rb<550)
		&&((u11_rmv_00_mvx<15)&&(u11_rmv_00_mvx> -15))
		&&((u11_rmv_23_mvx<95)&&(u11_rmv_23_mvx> -135))
		&&((u10_rmv_00_mvy<15)&&(u10_rmv_00_mvy> -30))
		&&((u10_rmv_23_mvy<100)&&(u10_rmv_23_mvy> -110))
		&&((u11_gmv_mvx<50)&&(u11_gmv_mvx>-15))
		&&((u10_gmv_mvy<35)&&(u10_gmv_mvy>-50))
		&&((BOT_ratio<10)&&(BOT_ratio>7))) //new add
		{
		ES_Flag_009_2=1;
		u32_RFB_ID=902;
		ES_Flag_009_2_Cnt=1200;
		}
	else if(ES_Flag_009_2_Cnt>0)
		{
		ES_Flag_009_2=1;
		u32_RFB_ID=902;
		ES_Flag_009_2_Cnt--;
		}
	else if ((ES_Flag_009_2_Cnt==0)||((BOT_ratio>10)||(BOT_ratio<7))){
		ES_Flag_009_2=0;
		}



		
	      if(((u25_rgnSad_rb_R[20]<450000)&&(u25_rgnSad_rb_R[20]>100000))
		&&((u20_rgnDtl_rb_R[20]<110000)&&(u20_rgnDtl_rb_R[20]>60000))
		&&((u13_rPRD_rb_R[20]<750)&&(u13_rPRD_rb_R[20]>450))
		&&((u26_aAPLp_rb<6000000)&&(u26_aAPLp_rb>5000000))
		&&((u20_rgnDtl_rb<1700)&&(u20_rgnDtl_rb>600))
		&&(u13_rPRD_rb<15)
		&&((u11_rmv_00_mvx<-15)&&(u11_rmv_00_mvx> -50))
		&&((u11_rmv_20_mvx<-10)&&(u11_rmv_20_mvx> -40))
		&&((u10_rmv_00_mvy<8)&&(u10_rmv_00_mvy> -33))
		&&((u10_rmv_20_mvy<40)&&(u10_rmv_20_mvy> -30))
		&&((u11_gmv_mvx<-15)&&(u11_gmv_mvx>-40))
		&&((u10_gmv_mvy<10)&&(u10_gmv_mvy>-30))
		&&(BOT_ratio<2)) //new add
		{
		ES_Flag_072_1=1;
		u32_RFB_ID=7201;
		ES_Flag_072_1_Cnt=800;
		}
	else if (ES_Flag_072_1_Cnt>0)
		{
			ES_Flag_072_1=1;
			u32_RFB_ID=7201;
			ES_Flag_072_1_Cnt--;
		}
	else if((ES_Flag_072_1_Cnt==0)||(BOT_ratio>=2))
		{
			ES_Flag_072_1=0;
		}

#if 1 // 20200911 re tune 
	      if(((u25_rgnSad_rb_R[11]<90000)&&(u25_rgnSad_rb_R[11]>5000))
		&&((u20_rgnDtl_rb_R[11]<55000)&&(u20_rgnDtl_rb_R[11]>10000))
		&&((u13_rPRD_rb_R[11]<900)&&(u13_rPRD_rb_R[11]>100))	
		&&((u25_rgnSad_rb_R[12]<280000)&&(u25_rgnSad_rb_R[12]>60000))
		&&((u20_rgnDtl_rb_R[12]<80000)&&(u20_rgnDtl_rb_R[12]>40000))
		&&((u13_rPRD_rb_R[12]<1400)&&(u13_rPRD_rb_R[12]>300))		
		&&((u26_aAPLp_rb<5100000)&&(u26_aAPLp_rb>4900000))
		&&((u20_rgnDtl_rb<3000)&&(u20_rgnDtl_rb>0))
		&&(u13_rPRD_rb<20)
		&&((u11_rmv_00_mvx<10)&&(u11_rmv_00_mvx> -35))
		&&((u11_rmv_11_mvx<25)&&(u11_rmv_11_mvx> -35))
		&&((u11_rmv_12_mvx<10)&&(u11_rmv_12_mvx> -30))		
		&&((u10_rmv_00_mvy<10)&&(u10_rmv_00_mvy> -50))
		&&((u10_rmv_11_mvy<10)&&(u10_rmv_11_mvy> -50))
		&&((u10_rmv_12_mvy<10)&&(u10_rmv_12_mvy> -50))		
		&&((u11_gmv_mvx<40)&&(u11_gmv_mvx>-20))
		&&((u10_gmv_mvy<0)&&(u10_gmv_mvy>-50))
		&&(BOT_ratio<2)) //new add
		{
		ES_Flag_072_2=1;
		u32_RFB_ID=7202;
		ES_Flag_072_2_Cnt=1200;
		}
	else if (ES_Flag_072_2_Cnt>0)
		{
			ES_Flag_072_2=1;
			u32_RFB_ID=7202;
			ES_Flag_072_2_Cnt--;
		}
	else if((ES_Flag_072_2_Cnt==0)||(BOT_ratio>=2))
		{
			ES_Flag_072_2=0;
		}
#endif




#if 1
	if(1)  //20200909 open // +++++++++  Temp mark 
		{
	      if(((u25_rgnSad_rb_R[27]<100000)&&(u25_rgnSad_rb_R[27]>15000))
		&&((u20_rgnDtl_rb_R[27]<60000)&&(u20_rgnDtl_rb_R[27]>30000))
		&&((u13_rPRD_rb_R[27]<300)&&(u13_rPRD_rb_R[27]>50))
	      &&((u25_rgnSad_rb_R[28]<110000)&&(u25_rgnSad_rb_R[28]>20000))
		&&((u20_rgnDtl_rb_R[28]<110000)&&(u20_rgnDtl_rb_R[28]>30000))
		&&((u13_rPRD_rb_R[28]<850)&&(u13_rPRD_rb_R[28]>100))
	      &&((u25_rgnSad_rb_R[0]<360000)&&(u25_rgnSad_rb_R[0]>20000))
		&&((u20_rgnDtl_rb_R[0]<55000)&&(u20_rgnDtl_rb_R[0]>40000))
		&&((u13_rPRD_rb_R[0]<250)&&(u13_rPRD_rb_R[0]>60))
		&&((u26_aAPLp_rb<3900000)&&(u26_aAPLp_rb>3800000))			
		&&((u11_rmv_00_mvx<40)&&(u11_rmv_00_mvx> -30))
		&&((u11_rmv_27_mvx<30)&&(u11_rmv_27_mvx> -60))
		&&((u11_rmv_28_mvx<30)&&(u11_rmv_28_mvx> -60))		
		&&((u10_rmv_00_mvy<10)&&(u10_rmv_00_mvy> -20))
		&&((u10_rmv_27_mvy<20)&&(u10_rmv_27_mvy> -120))
		&&((u10_rmv_28_mvy<20)&&(u10_rmv_28_mvy> -30))	
		&&((u11_gmv_mvx<10)&&(u11_gmv_mvx>-30))
		&&((u10_gmv_mvy<15)&&(u10_gmv_mvy>-10))
		&&((rSCss<=80000)&&(rSCss>=8000)) //r27
		&& ((rTCss<=10000)&&(rTCss>=2000)) //r27
		&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[27]<=35)//&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[27]>=0)) //r27
		&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[27]<=14)//&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[27]>=0))//r27		
		&&(BOT_ratio==20)) //new add
		{
		ES_Flag_149=1;
		u32_RFB_ID=149;
		ES_Flag_149_Cnt=2000;
		}
		else if (ES_Flag_149_Cnt>0)
		{
			ES_Flag_149=1;
			u32_RFB_ID=149;
			ES_Flag_149_Cnt--;
		}
		else if((ES_Flag_149_Cnt==0)||(BOT_ratio!=20))
		{
			ES_Flag_149=0;
		}
		}		
#endif



	      if(((u25_rgnSad_rb_R[20]<130000)&&(u25_rgnSad_rb_R[20]>1000))
		&&((u20_rgnDtl_rb_R[20]<30000)&&(u20_rgnDtl_rb_R[20]>1500))
		&&(u13_rPRD_rb_R[20]<180)
		&&((u26_aAPLp_rb<3800000)&&(u26_aAPLp_rb>2400000))
		&&((u20_rgnDtl_rb<40000)&&(u20_rgnDtl_rb>15000))
		&&(u13_rPRD_rb<180)
		&&((u11_rmv_00_mvx<110)&&(u11_rmv_00_mvx>10))
		&&((u11_rmv_20_mvx<30)&&(u11_rmv_20_mvx> -180))
		&&((u10_rmv_00_mvy<30)&&(u10_rmv_00_mvy> -10))
		&&((u10_rmv_20_mvy<70)&&(u10_rmv_20_mvy> -50))
		&&((u11_gmv_mvx<100)&&(u11_gmv_mvx>5))
		&&((u10_gmv_mvy<20)&&(u10_gmv_mvy>-15))
		&&((BOT_ratio<22)&&(BOT_ratio>18))) //new add
		{
			ES_Flag_151=1;
			u32_RFB_ID=151;
			ES_Flag_151_Cnt=1200;
		}
	else if (ES_Flag_151_Cnt>0)
		{
			ES_Flag_151=1;
			u32_RFB_ID=151;
			ES_Flag_151_Cnt--;
		}
	else if((ES_Flag_151_Cnt==0)||((BOT_ratio>22)||(BOT_ratio<18)))
		{
			ES_Flag_151=0;
		}


		#if 1  //20200911 conflict with #241 #340
		//20200806 new 18201 //fix 182 conflict with 244
		
		  if(((u25_rgnSad_rb_R[19]<140000)&&(u25_rgnSad_rb_R[19]>0))
		&&((u20_rgnDtl_rb_R[19]<65000)&&(u20_rgnDtl_rb_R[19]>35000))
		&&((u13_rPRD_rb_R[19]<450)&&(u13_rPRD_rb_R[19]>200))
		&&((u25_rgnSad_rb_R[20]<75000)&&(u25_rgnSad_rb_R[20]>10000))
		&&((u20_rgnDtl_rb_R[20]<65000)&&(u20_rgnDtl_rb_R[20]>30000))
		&&((u13_rPRD_rb_R[20]<480)&&(u13_rPRD_rb_R[20]>200))
		&&((u25_rgnSad_rb_R_avg<60000)&&(u25_rgnSad_rb_R_avg>20000))
		&&((u20_rgnDtl_rb_R_avg<50000)&&(u20_rgnDtl_rb_R_avg>25000))
		&&((u13_rPRD_rb_R_avg<350)&&(u13_rPRD_rb_R_avg>150))
		&&((u11_rmv_00_mvx<-20)&&(u11_rmv_00_mvx>-70))
		&&((u11_rmv_19_mvx<30)&&(u11_rmv_19_mvx>-80))
		&&((u11_rmv_20_mvx<40)&&(u11_rmv_20_mvx> -90))
		&&((u11_rmv_21_mvx<80)&&(u11_rmv_21_mvx> -90))
		&&((u10_rmv_00_mvy<45)&&(u10_rmv_00_mvy> 5))
		&&((u10_rmv_19_mvy<45)&&(u10_rmv_19_mvy> -10))
		&&((u10_rmv_20_mvy<90)&&(u10_rmv_20_mvy> -50))
		&&((u10_rmv_21_mvy<80)&&(u10_rmv_21_mvy> -50))
		&&((u11_gmv_mvx<0)&&(u11_gmv_mvx>-60))
		&&((u10_gmv_mvy<45)&&(u10_gmv_mvy>0))
		//&&((u11_gmv_mvx<10)&&(u11_gmv_mvx>-60))
		//&&((u10_gmv_mvy<45)&&(u10_gmv_mvy>-10))		
		&&(BOT_ratio<2)
		&&((rmv_cnt<1100)&&(rmv_cnt>220))
		&&((rmv_2nd_cnt<500)&&(rmv_2nd_cnt>0))
		&&((rmv_unconf<45)&&(rmv_unconf>0))
		&&((rmv_2nd_unconf<4096)&&(rmv_2nd_unconf>0))
		//&&(_ABS_(rmv_x_delta) >15)
		//&&(_ABS_(rmv_20_y_delta)>30)
		&&(_ABS_(rmv_x_delta) >15)
		&&(_ABS_(rmv_20_y_delta)>30)		
		//&&((rSCss<=8000)&&(rSCss>=0)) //r19
		//&& ((rTCss<=10000)&&(rTCss>=500))) //r19
		&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[19]<=30)//&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[19]>=0)) //r19
		&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[19]<=15))//&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[19]>=0)))//r19			
		{
			ES_Flag_182_1=1;
			u32_RFB_ID=982; //it means 18201
			ES_Flag_182_1_Cnt=2000;//1200;
		}
		else if (ES_Flag_182_1_Cnt>0)
		{
			ES_Flag_182_1=1;
			u32_RFB_ID=982; //it means 18201
			ES_Flag_182_1_Cnt--;
		}
		else if((ES_Flag_182_1_Cnt==0)||(BOT_ratio>=2))
		{
			ES_Flag_182_1=0;
		}
		#endif



#if 1 //20200914 18202 retune
		if(((u25_rgnSad_rb_R[11]<260000)&&(u25_rgnSad_rb_R[11]>40000))
		&&((u20_rgnDtl_rb_R[11]<90000)&&(u20_rgnDtl_rb_R[11]>20000))
		&&((u13_rPRD_rb_R[11]<550)&&(u13_rPRD_rb_R[11]>55))
		&&((u25_rgnSad_rb_R[12]<160000)&&(u25_rgnSad_rb_R[12]>70000))
		&&((u20_rgnDtl_rb_R[12]<50000)&&(u20_rgnDtl_rb_R[12]>42000))
		&&((u13_rPRD_rb_R[12]<380)&&(u13_rPRD_rb_R[12]>200))
		&&((u25_rgnSad_rb_R[0]<100000)&&(u25_rgnSad_rb_R[0]>40000))
		&&((u20_rgnDtl_rb_R[0]<50000)&&(u20_rgnDtl_rb_R[0]>30000))
		&&((u13_rPRD_rb_R[0]<350)&&(u13_rPRD_rb_R[0]>100))
		&&((u11_rmv_11_mvx<55)&&(u11_rmv_11_mvx>30))
		&&((u11_rmv_12_mvx<60)&&(u11_rmv_12_mvx>30))
		&&((u10_rmv_11_mvy<10)&&(u10_rmv_11_mvy> 0))
		&&((u10_rmv_12_mvy<10)&&(u10_rmv_12_mvy> 0))
		&&((u11_gmv_mvx<75)&&(u11_gmv_mvx>45))
		//&&(((u11_gmv_mvx<70)&&(u11_gmv_mvx>0))	//	
		&&((u10_gmv_mvy<15)&&(u10_gmv_mvy>3))
		&&(BOT_ratio<2)
		&&((rmv_cnt<950)&&(rmv_cnt>200))//r11
		&&((rmv_2nd_cnt<450)&&(rmv_2nd_cnt>0))//r11
		&&((rmv_unconf<25)&&(rmv_unconf>0)))//r11
		//&&((rmv_2nd_unconf<4096)&&(rmv_2nd_unconf>5))	//r11	
		//&&((rSCss<=85000)&&(rSCss>=2000))) //r11
		//&& ((rTCss<=30000)&&(rTCss>=0)) //r11
		//&&((s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[11]<=60)&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[11]>=0)) //r11
		//&&((s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[11]<=9)&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[11]>=0)))//r11		
		{
			ES_Flag_182_2=1;
			u32_RFB_ID=882; //it means 18202
			ES_Flag_182_2_Cnt=2500;
		}
		else if (ES_Flag_182_2_Cnt>0)
		{
			ES_Flag_182_2=1;
			u32_RFB_ID=882; //it means 18202
			ES_Flag_182_2_Cnt--;
		}
		else if((ES_Flag_182_2_Cnt==0)||(BOT_ratio>2))
		{
			ES_Flag_182_2=0;
		}
#endif



#if 1 //20200911 fix conflict with 103
	if(1) 
		{
		      if(((u25_rgnSad_rb_R[21]<60000)&&(u25_rgnSad_rb_R[21]>5000))
			&&((u20_rgnDtl_rb_R[21]<35000)&&(u20_rgnDtl_rb_R[21]>4000))
			&&((u13_rPRD_rb_R[21]<250)&&(u13_rPRD_rb_R[21]>0))
			&&((u25_rgnSad_rb_R[22]<60000)&&(u25_rgnSad_rb_R[22]>5000))
			&&((u20_rgnDtl_rb_R[22]<35000)&&(u20_rgnDtl_rb_R[22]>8000))
			&&((u13_rPRD_rb_R[22]<250)&&(u13_rPRD_rb_R[22]>0))
			&&((u25_rgnSad_rb_R[0]<35000)&&(u25_rgnSad_rb_R[0]>5000))
			&&((u20_rgnDtl_rb_R[0]<35000)&&(u20_rgnDtl_rb_R[0]>20000))
			&&((u13_rPRD_rb_R[0]<230)&&(u13_rPRD_rb_R[0]>150))	
			&&((u25_rgnSad_rb_R_avg<16000)&&(u25_rgnSad_rb_R_avg>0))
			&&((u20_rgnDtl_rb_R_avg<12000)&&(u20_rgnDtl_rb_R_avg>0))
			&&((u13_rPRD_rb_R_avg<250)&&(u13_rPRD_rb_R_avg>0))
			&&((u11_rmv_21_mvx<150)&&(u11_rmv_21_mvx>-20))
			&&((u11_rmv_22_mvx<120)&&(u11_rmv_22_mvx>-20))
			&&((u10_rmv_21_mvy<40)&&(u10_rmv_21_mvy> -30))
			&&((u10_rmv_22_mvy<60)&&(u10_rmv_22_mvy> -20))
			&&((u11_gmv_mvx<20)&&(u11_gmv_mvx>-45))
			&&((u10_gmv_mvy<20)&&(u10_gmv_mvy>-45))			
			&&(BOT_ratio<2)
			&&(rSCss<=110000) //r21
			&& ((rTCss<=17000)&&(rTCss>=2000)) //r21
			&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[21]<=75)//&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[21]>=0)) //r21
			&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[21]<=15))//&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[21]>=0)))//r21				
			{
				ES_Flag_208=1;
				u32_RFB_ID=208;
				ES_Flag_208_Cnt=1500;
			}
			else if (ES_Flag_208_Cnt>0)
			{
				ES_Flag_208=1;
				u32_RFB_ID=208;
				ES_Flag_208_Cnt--;
			}
			else if((ES_Flag_208_Cnt==0)||(BOT_ratio>=2))
			{
				ES_Flag_208=0;
			}
		}
#endif

	
#if 0// YE Test don't need it , video is ok 20200910
	      if(((u25_rgnSad_rb_R[21]<105000)&&(u25_rgnSad_rb_R[21]>5000))
		&&((u20_rgnDtl_rb_R[21]<90000)&&(u20_rgnDtl_rb_R[21]>30000))
		&&((u13_rPRD_rb_R[21]<700)&&(u13_rPRD_rb_R[21]>200))
		&&((u25_rgnSad_rb_R[22]<140000)&&(u25_rgnSad_rb_R[22]>10000))
		&&((u20_rgnDtl_rb_R[22]<95000)&&(u20_rgnDtl_rb_R[22]>25000))
		&&((u13_rPRD_rb_R[22]<750)&&(u13_rPRD_rb_R[22]>150))
		&&((u25_rgnSad_rb_R_avg<110000)&&(u25_rgnSad_rb_R_avg>0))
		&&((u20_rgnDtl_rb_R_avg<85000)&&(u20_rgnDtl_rb_R_avg>55000))
		&&((u13_rPRD_rb_R_avg<600)&&(u13_rPRD_rb_R_avg>350))
		&&((u11_rmv_21_mvx<10)&&(u11_rmv_21_mvx>-80))
		&&((u11_rmv_22_mvx<10)&&(u11_rmv_22_mvx>-80))
		&&((u10_rmv_21_mvy<10)&&(u10_rmv_21_mvy> -40))
		&&((u10_rmv_22_mvy<10)&&(u10_rmv_22_mvy> -40))
		&&((u11_rmv_11_mvx<9)&&(u11_rmv_11_mvx>-10))
		&&((u10_rmv_11_mvy<6)&&(u10_rmv_11_mvy> -20))
 		&&((u11_gmv_mvx<8)&&(u11_gmv_mvx>-15))
		&&((u10_gmv_mvy<5)&&(u10_gmv_mvy>-25))
		&&(BOT_ratio<2))
		{
			ES_Flag_219=1;
			u32_RFB_ID=219;
			ES_Flag_219_Cnt=1000;
		}
	else if (ES_Flag_219_Cnt>0)
		{
			ES_Flag_219=1;
			u32_RFB_ID=219;
			ES_Flag_219_Cnt--;
		}
	else if((ES_Flag_219_Cnt==0)||(BOT_ratio>=2))
		{
			ES_Flag_219=0;
		}
#endif		
	      if(((u25_rgnSad_rb_R[22]<61000)&&(u25_rgnSad_rb_R[22]>1000))
		&&((u20_rgnDtl_rb_R[22]<42000)&&(u20_rgnDtl_rb_R[22]>3000))
		&&((u13_rPRD_rb_R[22]<240)&&(u13_rPRD_rb_R[22]>0))
		&&((u25_rgnSad_rb_R[23]<43000)&&(u25_rgnSad_rb_R[23]>1000))
		&&((u20_rgnDtl_rb_R[23]<43000)&&(u20_rgnDtl_rb_R[23]>1000))
		&&((u13_rPRD_rb_R[23]<330)&&(u13_rPRD_rb_R[23]>0))
		&&((u25_rgnSad_rb_R_avg<35000)&&(u25_rgnSad_rb_R_avg>3000))
		&&((u20_rgnDtl_rb_R_avg<26000)&&(u20_rgnDtl_rb_R_avg>9000))
		&&((u13_rPRD_rb_R_avg<120)&&(u13_rPRD_rb_R_avg>30))
		&&((u11_rmv_22_mvx<65)&&(u11_rmv_22_mvx>-5))
		&&((u11_rmv_23_mvx<90)&&(u11_rmv_23_mvx>-5))
		&&((u10_rmv_22_mvy<90)&&(u10_rmv_22_mvy>-5))
		&&((u10_rmv_23_mvy<80)&&(u10_rmv_23_mvy> -15))
 		&&((u11_gmv_mvx<70)&&(u11_gmv_mvx>10))
		&&((u10_gmv_mvy<80)&&(u10_gmv_mvy>10))
		&&((BOT_ratio<140)&&(BOT_ratio>136)))
		{
			ES_Flag_316=1;
			u32_RFB_ID=316;
			ES_Flag_316_Cnt=600;
		}
	else if (ES_Flag_316_Cnt>0)
		{
			ES_Flag_316=1;
			u32_RFB_ID=316;
			ES_Flag_316_Cnt--;
		}
	else if((ES_Flag_316_Cnt==0)||((BOT_ratio>140)||(BOT_ratio<136)))
		{
			ES_Flag_316=0;
		}

	if(1) //20200807 fix //conflict with 317
		{
		      if(((u25_rgnSad_rb_R[15]<60000)&&(u25_rgnSad_rb_R[15]>0))
 			&&((u20_rgnDtl_rb_R[15]<130000)&&(u20_rgnDtl_rb_R[15]>0))
			&&((u13_rPRD_rb_R[15]<500)&&(u13_rPRD_rb_R[15]>0)) 	
			&&((u25_rgnSad_rb_R[16]<60000)&&(u25_rgnSad_rb_R[16]>1000))
			&&((u20_rgnDtl_rb_R[16]<60000)&&(u20_rgnDtl_rb_R[16]>0))
			&&((u13_rPRD_rb_R[16]<430)&&(u13_rPRD_rb_R[16]>0))
			&&((u25_rgnSad_rb_R[17]<92000)&&(u25_rgnSad_rb_R[17]>0))
			&&((u20_rgnDtl_rb_R[17]<60000)&&(u20_rgnDtl_rb_R[17]>0))
			&&((u13_rPRD_rb_R[17]<450)&&(u13_rPRD_rb_R[17]>0))
			&&((u25_rgnSad_rb_R_avg<90000)&&(u25_rgnSad_rb_R_avg>5000))
			&&((u20_rgnDtl_rb_R_avg<40000)&&(u20_rgnDtl_rb_R_avg>30000))
			&&((u13_rPRD_rb_R_avg<220)&&(u13_rPRD_rb_R_avg>110))
			&&((u11_rmv_15_mvx<-2)&&(u11_rmv_15_mvx>-8))
			&&((u11_rmv_16_mvx<185)&&(u11_rmv_16_mvx>-30))
			&&((u11_rmv_17_mvx<190)&&(u11_rmv_17_mvx>-30))
			&&((u10_rmv_15_mvy<3)&&(u10_rmv_15_mvy>-9))
			&&((u10_rmv_16_mvy<60)&&(u10_rmv_16_mvy>-55))
			&&((u10_rmv_17_mvy<40)&&(u10_rmv_17_mvy> -50))
	 		////&&((u11_gmv_mvx<40)&&(u11_gmv_mvx>-15))
			&&((u11_gmv_mvx<55)&&(u11_gmv_mvx>5))  //20200807 modify
			&&((u10_gmv_mvy<8)&&(u10_gmv_mvy>-10))
			//&&((BOT_ratio<139)&&(BOT_ratio>135))
			&&((BOT_ratio==137))
			&&((rmv_cnt<1100)&&(rmv_cnt>400))
			&&((rmv_2nd_cnt<500)&&(rmv_2nd_cnt>0))
			&&((rmv_unconf<45)&&(rmv_unconf>0))
			&&((rmv_2nd_unconf<4096)&&(rmv_2nd_unconf>0)))
			{
				ES_Flag_319=1;
				u32_RFB_ID=319;
				ES_Flag_319_Cnt=1000;
			}
			else if (ES_Flag_319_Cnt>0)
			{
				ES_Flag_319=1;
				u32_RFB_ID=319;
				ES_Flag_319_Cnt--;
			}
			//else if((ES_Flag_319_Cnt==0)||((BOT_ratio>139)&&(BOT_ratio<135)))
			else if((ES_Flag_319_Cnt==0)||((BOT_ratio!=137)))
			{
				ES_Flag_319=0;
			}
		}


#if 1 //new 20200911	fix conflict with 317
	if(1)  
	{
		      if(((u25_rgnSad_rb_R[11]<52000)&&(u25_rgnSad_rb_R[11]>0))
			&&((u20_rgnDtl_rb_R[11]<50000)&&(u20_rgnDtl_rb_R[11]>0))
			&&((u13_rPRD_rb_R[11]<2400)&&(u13_rPRD_rb_R[11]>100))
			&&((u25_rgnSad_rb_R[12]<52000)&&(u25_rgnSad_rb_R[12]>0))
			&&((u20_rgnDtl_rb_R[12]<48000)&&(u20_rgnDtl_rb_R[12]>0))
			&&((u13_rPRD_rb_R[12]<2400)&&(u13_rPRD_rb_R[12]>0))
			&&((u25_rgnSad_rb_R[13]<55000)&&(u25_rgnSad_rb_R[13]>0))
			&&((u20_rgnDtl_rb_R[13]<50000)&&(u20_rgnDtl_rb_R[13]>8000))
			&&((u13_rPRD_rb_R[13]<2500)&&(u13_rPRD_rb_R[13]>0))		
			&&((u25_rgnSad_rb_R[14]<60000)&&(u25_rgnSad_rb_R[14]>0))
			&&((u20_rgnDtl_rb_R[14]<62000)&&(u20_rgnDtl_rb_R[14]>2000))
			&&((u13_rPRD_rb_R[14]<3000)&&(u13_rPRD_rb_R[14]>0))			
			//&&((u25_rgnSad_rb_R_avg<45000)&&(u25_rgnSad_rb_R_avg>0))
			//&&((u20_rgnDtl_rb_R_avg<45000)&&(u20_rgnDtl_rb_R_avg>5000))
			//&&((u13_rPRD_rb_R_avg<280)&&(u13_rPRD_rb_R_avg>0))
			&&((u11_rmv_11_mvx<-70)&&(u11_rmv_11_mvx>-200))
			&&((u11_rmv_12_mvx<-70)&&(u11_rmv_12_mvx>-220))
			&&((u11_rmv_13_mvx<-50)&&(u11_rmv_13_mvx>-200))
			&&((u10_rmv_11_mvy<0)&&(u10_rmv_11_mvy>-70))
			&&((u10_rmv_12_mvy<0)&&(u10_rmv_12_mvy> -70))
			&&((u10_rmv_13_mvy<0)&&(u10_rmv_13_mvy> -50))
	 		&&((u11_gmv_mvx<-120)&&(u11_gmv_mvx>-220))
			&&((u10_gmv_mvy<0)&&(u10_gmv_mvy>-20))
			&&((rSCss<=70000)&&(rSCss>=8000)) //r11
			&& ((rTCss<=14000)&&(rTCss>=2000)) //r11
			&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[11]<=55)//&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[11]>=0)) //r11
			&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[11]<=14)//&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[11]>=0))//r11			
			&&((BOT_ratio<139)&&(BOT_ratio>136)&&(LFT_ratio>10)))
			{
				ES_Flag_358=1;
				u32_RFB_ID=358;
				ES_Flag_358_Cnt=2000;
			}
		else if (ES_Flag_358_Cnt>0)
			{
				ES_Flag_358=1;
				u32_RFB_ID=358;
				ES_Flag_358_Cnt--;
			}
		else if((ES_Flag_358_Cnt==0)||((BOT_ratio>139)||(BOT_ratio<136)))
			{
				ES_Flag_358=0;
			}

	}
#endif



#if 1 //20200909
	      if(((u25_rgnSad_rb_R[11]<70000)&&(u25_rgnSad_rb_R[11]>40000))
		&&((u20_rgnDtl_rb_R[11]<30000)&&(u20_rgnDtl_rb_R[11]>20000))
		&&((u13_rPRD_rb_R[11]<180)&&(u13_rPRD_rb_R[11]>80))
		&&((u25_rgnSad_rb_R[12]<85000)&&(u25_rgnSad_rb_R[12]>0))
		&&((u20_rgnDtl_rb_R[12]<34000)&&(u20_rgnDtl_rb_R[12]>1000))
		&&((u13_rPRD_rb_R[12]<160)&&(u13_rPRD_rb_R[12]>0))
		&&((u25_rgnSad_rb_R_avg<40000)&&(u25_rgnSad_rb_R_avg>5000))
		&&((u20_rgnDtl_rb_R_avg<30000)&&(u20_rgnDtl_rb_R_avg>9000))
		&&((u13_rPRD_rb_R_avg<120)&&(u13_rPRD_rb_R_avg>10))
		&&((u11_rmv_11_mvx<35)&&(u11_rmv_11_mvx>-25))
		&&((u11_rmv_12_mvx<55)&&(u11_rmv_12_mvx>-15))
		&&((u10_rmv_11_mvy<55)&&(u10_rmv_11_mvy>-155))
		&&((u10_rmv_12_mvy<60)&&(u10_rmv_12_mvy> -155))
 		&&((u11_gmv_mvx<10)&&(u11_gmv_mvx>0))
		&&((u10_gmv_mvy<10)&&(u10_gmv_mvy>-60))
		//&&((BOT_ratio<142)&&(BOT_ratio>138))
		&&(BOT_ratio==140)
		&&((rSCss<=40000)&&(rSCss>=10000)) //r11
		&& ((rTCss<=6500)&&(rTCss>=2000)) //r11
		&&((s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[19]<=55)&&(s_pContext->_output_me_sceneAnalysis.mvDiff_tmp[19]>=5)) //r11
		&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[19]<=12)//&&(s_pContext->_output_me_sceneAnalysis.big_relMV_cnt_tmp[19]>=0))//r11		
		&&((rmv_cnt<1100)&&(rmv_cnt>400))
		&&((rmv_2nd_cnt<500)&&(rmv_2nd_cnt>0))
		&&((rmv_unconf<45)&&(rmv_unconf>0))
		&&((rmv_2nd_unconf<4096)&&(rmv_2nd_unconf>0)))
		{
			ES_Flag_109=1;
			u32_RFB_ID=109;
			ES_Flag_109_Cnt=2500;
		}
	else if (ES_Flag_109_Cnt>0)
		{
			ES_Flag_109=1;
			u32_RFB_ID=109;
			ES_Flag_109_Cnt--;
		}
	else if((ES_Flag_109_Cnt==0)||(BOT_ratio!=140))
		{
			ES_Flag_109=0;
		}
#endif
		}  //YE Test to control ES Enable/Disable

//		rtd_pr_memc_debug( "[jerry_MEMC_DisableMute_01][%d, %d](90K = %d)\n", memc_mute_cnt, memc_mute_state,rtd_inl(TIMER_SCPU_CLK90K_LO_reg));


		pOutput->u32_ES_RFB_ID_show=u32_RFB_ID&0xFFF;


		#if 1
		if(ES_Test_24==1)
		{
			#if 0 //mark for too much log when show ipr log
			rtd_pr_memc_debug("[MEMC_FLAG] ,RFB_ID=%d, ES_Test_0 =%d ,ES_Test_1 =%d,rmv_cond=%d, \n",u32_RFB_ID,ES_Test_0,ES_Test_1 ,rmv_cond);
			if(u32_RFB_ID>0){
				rtd_pr_memc_debug("[YE_MEMC_FLAG] ,RFB_ID=%d, ES_Test_0 =%d ,ES_Test_1 =%d,rmv_cond=%d, \n",u32_RFB_ID,ES_Test_0,ES_Test_1 ,rmv_cond);
			}
			#endif
			u32_RFB_ID=0;

		}
		if(ES_Test_30==1)
		{
		 #if 0
		rtd_pr_memc_debug("[YE_MEMC_RMV] ,gmv_rassia_cnt=%d ,gmv_rassia_flag=%d,u11_gmv_mvx=%d,gmv_mvy=%d,rmv_08_x_delta=%d, 8_mvx =%d ,08_mvy=%d,9_mvx =%d,10_mvx=%d,rmv_10_mvy=%d, 11_mvx=%d,rmv_11_mvy=%d,16_mvx=%d,17_mvx=%d, 18_mvx=%d, 19_mvx=%d,20_mvx=%d,20_mvy=%d, 21_mvx=%d,22_mvx=%d,23_mvx=%d,421_flag=%d, 21_flag=%d,rmv_20_y_delta=%d,\n"
		,gmv_rassia_cnt,gmv_rassia_flag,u11_gmv_mvx,u10_gmv_mvy,_ABS_(rmv_08_x_delta),u11_rmv_08_mvx,u10_rmv_08_mvy,u11_rmv_09_mvx,u11_rmv_10_mvx,u10_rmv_10_mvy,u11_rmv_11_mvx,u10_rmv_11_mvy,u11_rmv_16_mvx,u11_rmv_17_mvx,u11_rmv_18_mvx,u11_rmv_19_mvx,u11_rmv_20_mvx,u10_rmv_20_mvy,
		u11_rmv_21_mvx,u11_rmv_22_mvx,u11_rmv_23_mvx,ES_Flag_421,ES_Flag_21,_ABS_(rmv_20_y_delta));

		
		 rtd_pr_memc_debug("[YE_MEMC_INFO] ,i=%d,gmv_rassia_cnt=%d ,gmv_rassia_flag=%d,u11_gmv_mvx=%d,gmv_mvy=%d ,ES_Flag_317_Panning_reverse=%d,over_50_cnt=%d,317_over_50_flag=%d, sc_true=%d,rmv_cond =%d, rmv_cnt =%d,rmv_2nd_cnt=%d,  panning_cnt_thl =%d,  rmv_unconf=%d,rmv_2nd_unconf=%d,  panning_unconf_thl=%d, rmv_x=%d, rmv_x_delta=%d, alpha_tmp=%d, level_tmp=%d	,rgnSad_rb=%d, rgnDtl_rb=%d, rTC_rb=%d, rPRD_rb=%d, aAPLp_rb=%d, ES_364=%d, ES_354=%d, ES_317=%d,ES_317_cnt=%d,L_To_R=%d,R_To_L=%d, BOT_ratio=%d,Flag_317_temp=%d,ES_Flag_317_Cnt=%d, \n"
		,i,gmv_rassia_cnt,gmv_rassia_flag,u11_gmv_mvx,u10_gmv_mvy, ES_Flag_317_Panning_reverse,gmv_317_over_50_cnt,gmv_317_over_50_flag,s_pContext->_output_fblevelctrl.u1_HW_sc_true,rmv_cond,rmv_cnt,rmv_2nd_cnt,pParam->u8_rFB_panning_cnt_thl, rmv_unconf, rmv_2nd_unconf,pParam->u8_rFB_panning_unconf_thl,_ABS_(rmv_x),_ABS_(rmv_x_delta),rFB_alpha_tmp,rFB_level_tmp,
		 u25_rgnSad_rb,u20_rgnDtl_rb,u22_rTC_rb,u13_rPRD_rb,u26_aAPLp_rb,ES_Flag_364,ES_Flag_354,ES_Flag_317,ES_317_cnt,L_To_R_Paning,R_To_L_Paning,BOT_ratio,ES_Flag_317_temp,ES_Flag_317_Cnt);

		
		rtd_pr_memc_debug("[YE_MEMC_REV] ,i=%d ,gmv_mvx=%d,gmv_mvy=%d,8_mvx =%d ,rmv_08_temp[119]=%d ,rmv_08_temp[0]=%d ,rmv_08_x_delta=%d ,ES_Flag_317_Panning_reverse=%d, ES_317_cnt=%d ,L_to_R_cnt=%d,R_to_L_cnt=%d ,L_To_R=%d ,R_To_L=%d, \n "
		,j,u11_gmv_mvx,u10_gmv_mvy,u11_rmv_08_mvx, rmv_08_temp[119], rmv_08_temp[0], rmv_08_x_delta, ES_Flag_317_Panning_reverse, ES_317_cnt 
		,rmv_08_x_delta_L_to_R_cnt,rmv_08_x_delta_R_to_L_cnt,L_To_R_Paning,R_To_L_Paning);
		#endif
		}
		#endif
	



//ES_Test_Solution2=ES_Test_27;

//ES_Flag_421=1;
if(ES_Test_4==1)	  //YE Test to control ES Enable/Disable
		{		
		if(1)//if(0) for 0819_pak1 and pak3, Terrence
		//if(ES_Test_0==1)//for 0819_2, Terrence
			{
				//rFB_alpha_tmp = 255;
				//rFB_level_tmp = 0;

				if((rmv_cond==1)||(ES_Flag_21==1)||(ES_Flag_380==1)||(ES_Flag_413==1)||(ES_Flag_421==1)||(Rassia_Flag==1)||(ES_Test_29==1)||(ES_Flag_364==1)
					||(ES_Flag_040==1)||(ES_Flag_059==1)||(ES_Flag_043==1)||(ES_Flag_035==1)||(ES_Test_25==1)||(ES_Flag_317==1)||(ES_Flag_354==1)
					||(ES_Flag_009_2==1)||(ES_Flag_072_1==1)||(ES_Flag_072_2==1)||(ES_Flag_149==1)||(ES_Flag_151==1)||(ES_Flag_182_1==1)||(ES_Flag_182_2==1)
					||(ES_Flag_208==1)||(ES_Flag_219==1)||(ES_Flag_316==1)||(ES_Flag_319==1)||(ES_Flag_009_1==1)||(ES_Flag_358==1)||(ES_Flag_109==1))//||(ES_Test_26==1)
				{
					#if	1		
					if(ES_Flag_364==1)
					{
					 	if(((i>=8)&&(i<=10))||((i>=16)&&(i<=18))||(i==14)||(i==15)||(i==22)||(i==23))
					 	{
						rFB_alpha_tmp = 255;
						rFB_level_tmp = 0;
					 	}
					}
					#endif
					
					if(ES_Test_1==1)
						{  // new condition
							
							if((ES_Flag_364==1)||(ES_Flag_354==1)||(ES_Flag_317==1)||(ES_Flag_380==1)||(ES_Test_25==1)||(ES_Flag_040==1)||(ES_Flag_059==1)||(ES_Flag_043==1)||(ES_Flag_035==1)
								||(ES_Flag_009_2==1)||(ES_Flag_072_1==1)||(ES_Flag_072_2==1)||(ES_Flag_149==1)||(ES_Flag_151==1)||(ES_Flag_182_1==1)||(ES_Flag_182_2==1)
								||(ES_Flag_208==1)||(ES_Flag_219==1)||(ES_Flag_316==1)||(ES_Flag_319==1)||(ES_Flag_009_1==1)||(ES_Flag_009_1==1)||(ES_Flag_358==1)||(ES_Flag_421==1)
								||(ES_Flag_109==1))
								{
									
									rFB_alpha_tmp = 255;
									rFB_level_tmp = 0;
									pOutput->u8_ES_flag=1;
									
									if(ES_Flag_380==1)
									pOutput->u8_ES_380_ID_flag=1;
									else
									pOutput->u8_ES_380_ID_flag=0;	
									
									#if 0
									if(ES_Test_2==1)
									{
														 rtd_pr_memc_debug("[YE_MEMC] ,rmv_cond =%d, rmv_cnt =%d,  panning_cnt_thl =%d,  rmv_unconf=%d,  panning_unconf_thl=%d, rmv_x=%d, rmv_x_delta=%d, alpha_tmp=%d, level_tmp=%d	,rgnSad_rb=%d, rgnDtl_rb=%d, rTC_rb=%d, rPRD_rb=%d, aAPLp_rb=%d, ES_364=%d, ES_354=%d, ES_317=%d,ES_317_cnt=%d,L_To_R=%d,R_To_L=%d, BOT_ratio=%d,\n"
													 	 ,rmv_cond,rmv_cnt,pParam->u8_rFB_panning_cnt_thl, rmv_unconf, pParam->u8_rFB_panning_unconf_thl,_ABS_(rmv_x),_ABS_(rmv_x_delta),rFB_alpha_tmp,rFB_level_tmp,
													 	 u25_rgnSad_rb,u20_rgnDtl_rb,u22_rTC_rb,u13_rPRD_rb,u26_aAPLp_rb,ES_Flag_364,ES_Flag_354,ES_Flag_317,ES_317_cnt,L_To_R_Paning,R_To_L_Paning,BOT_ratio);
									}
									
									if(ES_Test_3==1)
									{
										 				rtd_pr_memc_debug("[YE_MEMC_2] ,rmv_00_mvx =%d ,08_mvx=%d, 08_mvy=%d, 09_mvx=%d, 09_mvy=%d, 16_mvx=%d, 16_mvy=%d, 17_mvx=%d, 17_mvy=%d, 20_mvx=%d, 20_mvy=%d,\n"
										 				,u11_rmv_00_mvx,u11_rmv_08_mvx,u10_rmv_08_mvy,u11_rmv_09_mvx,u10_rmv_09_mvy,u11_rmv_16_mvx,u10_rmv_16_mvy,u11_rmv_17_mvx,u10_rmv_17_mvy,u11_rmv_20_mvx,u10_rmv_20_mvy);
									}
									#endif
									#if 0
									if(ES_Test_3==1)
									{
                                                    		rtd_pr_memc_debug("[YE_MEMC_FLAG] ,u8_ES_439_ID_flag=%d,u8_ES_413_ID_flag=%d,ES_Test_0 =%d ,ES_Test_1 =%d,rmv_cond=%d,  ES_Flag_21 =%d,  ES_Flag_380=%d,  ES_Flag_413=%d, ES_Test_29=%d, ES_Flag_364=%d, ES_Flag_354=%d, ES_Flag_317=%d, ES_Test_28=%d, ES_Flag_439=%d, ES_Flag_421=%d, \n"
                                                    		,pOutput->u8_ES_439_ID_flag,pOutput->u8_ES_413_ID_flag,ES_Test_0,ES_Test_1 ,rmv_cond,ES_Flag_21,ES_Flag_380, ES_Flag_413, ES_Test_29,ES_Flag_364,ES_Flag_354,
                                                    		 ES_Flag_317,ES_Test_28,ES_Flag_439,ES_Flag_421);
                                                    
                                                    		rtd_pr_memc_debug("[YE_MEMC_RMV] ,u11_gmv_mvx=%d,gmv_mvy=%d,rmv_08_x_delta=%d, 8_mvx =%d ,08_mvy=%d,9_mvx =%d,10_mvx=%d, 11_mvx=%d,16_mvx=%d,17_mvx=%d, 18_mvx=%d, 19_mvx=%d,20_mvx=%d,20_mvy=%d, 21_mvx=%d,22_mvx=%d,23_mvx=%d,421_flag=%d, 21_flag=%d,\n"
                                                    		,u11_gmv_mvx,u10_gmv_mvy,_ABS_(rmv_08_x_delta),u11_rmv_08_mvx,u10_rmv_08_mvy,u11_rmv_09_mvx,u11_rmv_10_mvx,u11_rmv_11_mvx,u11_rmv_16_mvx,u11_rmv_17_mvx,u11_rmv_18_mvx,u11_rmv_19_mvx,u11_rmv_20_mvx,u10_rmv_20_mvy,
                                                    		u11_rmv_21_mvx,u11_rmv_22_mvx,u11_rmv_23_mvx,ES_Flag_421,ES_Flag_21);
                                                    
                                                    		 rtd_pr_memc_debug("[YE_MEMC_INFO]  ,u11_gmv_mvx=%d,gmv_mvy=%d, ,ES_Flag_317_Panning_reverse=%d,over_50_cnt=%d,317_over_50_flag=%d, sc_true=%d,rmv_cond =%d, rmv_cnt =%d,  panning_cnt_thl =%d,  rmv_unconf=%d,  panning_unconf_thl=%d, rmv_x=%d, rmv_x_delta=%d, alpha_tmp=%d, level_tmp=%d	,rgnSad_rb=%d, rgnDtl_rb=%d, rTC_rb=%d, rPRD_rb=%d, aAPLp_rb=%d, ES_364=%d, ES_354=%d, ES_317=%d,ES_317_cnt=%d,L_To_R=%d,R_To_L=%d, BOT_ratio=%d,\n"
                                                    		,u11_gmv_mvx,u10_gmv_mvy, ES_Flag_317_Panning_reverse,gmv_317_over_50_cnt,gmv_317_over_50_flag,s_pContext->_output_fblevelctrl.u1_HW_sc_true,rmv_cond,rmv_cnt,pParam->u8_rFB_panning_cnt_thl, rmv_unconf, pParam->u8_rFB_panning_unconf_thl,_ABS_(rmv_x),_ABS_(rmv_x_delta),rFB_alpha_tmp,rFB_level_tmp,
                                                    		 u25_rgnSad_rb,u20_rgnDtl_rb,u22_rTC_rb,u13_rPRD_rb,u26_aAPLp_rb,ES_Flag_364,ES_Flag_354,ES_Flag_317,ES_317_cnt,L_To_R_Paning,R_To_L_Paning,BOT_ratio);
                                                    
                                                    		rtd_pr_memc_debug("[YE_MEMC_REV] ,j =%d ,8_mvx =%d ,rmv_08_temp[119]=%d ,rmv_08_temp[0]=%d ,rmv_08_x_delta=%d ,ES_Flag_317_Panning_reverse=%d, ES_317_cnt=%d ,L_to_R_cnt=%d,R_to_L_cnt=%d ,L_To_R=%d ,R_To_L=%d, \n ",
                                                    		j,u11_rmv_08_mvx, rmv_08_temp[119], rmv_08_temp[0], rmv_08_x_delta, ES_Flag_317_Panning_reverse, ES_317_cnt 
                                                    		,rmv_08_x_delta_L_to_R_cnt,rmv_08_x_delta_R_to_L_cnt,L_To_R_Paning,R_To_L_Paning);
									}
									#endif
									
								}
							#if 0 //mark it all to avoid tv006 doesn't accept judder
							else if ((ES_Flag_439==1)||(ES_Flag_21==1)||(ES_Flag_413==1)||(Rassia_Flag==1)||(ES_Test_26==1))//||(ES_Flag_421==1))
								{
									pOutput->u8_ES_flag2=1;
									pOutput->u8_ES_DEHALO_flag=1;//YE Test 20200521 to detect dehalo control flag here;
									
									#if 0 //it's max reginal fallback
									rFB_alpha_tmp = 255;
									rFB_level_tmp = 255;
									#else //YE Test it will disable LFB
									//rFB_alpha_tmp = 255; //439 solution is mark here
									//rFB_level_tmp = 0;    //439 solution is mark here
									#endif
									//pOutput->u8_ES_flag=1;	
									if(ES_Flag_21==1){
									rFB_alpha_tmp = 255;
									rFB_level_tmp = 110;
									}

									
									if((ES_Flag_439==1)||(ES_Test_26==1))
									pOutput->u8_ES_439_ID_flag=1;	
									else if(ES_Flag_21==1)
									pOutput->u8_ES_21_ID_flag=1;
									else if(ES_Flag_413==1)
									pOutput->u8_ES_413_ID_flag=1;
									else if(ES_Flag_421==1)
									pOutput->u8_ES_421_ID_flag=1;
									else if(Rassia_Flag==1)
									pOutput->u8_ES_Rassia_ID_flag=1;
									else
									pOutput->u8_ES_flag=1;  //20200424
	
								//pOutput->u32_ES_GFB=ES_Test_GFB;
								
								//s_pContext->_output_wrt_comreg.u1_localfb_wrt_en=1;
								//s_pContext._param_wrt_comreg.
								//WriteRegister(FRC_TOP__PQL_0__pql_patch_dummy_e4_ADDR, 7, 7, 0x1);//u1_localfb_wrt_en;
								//s_pContext->_param_wrt_comreg.u1_FBLevelWr_en=1;


								#if 0 //move it to writeComReg
									#if 1
										WriteRegister(MC_MC_28_reg, 14, 14,0x1);
										WriteRegister(KMC_BI_bi_top_01_reg,2,3,0x2);
									#endif
								
								//WriteRegister(FRC_TOP__MC__mc_fb_lvl_force_value_ADDR,15,22,0xFF);

								
									#if 0 //ori setting
									WriteRegister(KME_DEHALO_KME_DEHALO_F0, 24, 31, 0xF0); 
									WriteRegister(KME_DEHALO_KME_DEHALO_F4, 24, 31, 0xFF);
									WriteRegister(KME_DEHALO_KME_DEHALO_F8, 24, 31, 0xF0); 
									WriteRegister(KME_DEHALO_KME_DEHALO_FC, 0, 7, 0xFF);

									#else //test setting

									if(ES_Flag_413==1){
										WriteRegister(KME_DEHALO_KME_DEHALO_F0_reg, 24, 31, 0x00); 
									      WriteRegister(KME_DEHALO_KME_DEHALO_F4_reg, 24, 31, 0x00);
									      WriteRegister(KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 0x00); 
									      WriteRegister(KME_DEHALO_KME_DEHALO_FC_reg, 0, 7, 0x00);
										}
									else if(ES_Flag_439==1){
									      WriteRegister(KME_DEHALO_KME_DEHALO_F0_reg, 24, 31, 0x0F); 
									     WriteRegister(KME_DEHALO_KME_DEHALO_F4_reg, 24, 31, 0xFF);
									     WriteRegister(KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 0x0F); 
									     WriteRegister(KME_DEHALO_KME_DEHALO_FC_reg, 0, 7, 0xFF);
										}
									else	{
									      WriteRegister(KME_DEHALO_KME_DEHALO_F0_reg, 24, 31, 0x0F); 
									     WriteRegister(KME_DEHALO_KME_DEHALO_F4_reg, 24, 31, 0xFF);
									     WriteRegister(KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 0x0F); 
									     WriteRegister(KME_DEHALO_KME_DEHALO_FC_reg, 0, 7, 0xFF);
										}
									#endif
								#endif
									#if 0
									if(ES_Test_2==1)
									{
														 rtd_pr_memc_debug("[YE_MEMC] ,rmv_cond =%d, rmv_cnt =%d,  panning_cnt_thl =%d,  rmv_unconf=%d,  panning_unconf_thl=%d, rmv_x=%d, rmv_x_delta=%d, alpha_tmp=%d, level_tmp=%d	,rgnSad_rb=%d, rgnDtl_rb=%d, rTC_rb=%d, rPRD_rb=%d, aAPLp_rb=%d, ES_364=%d, ES_354=%d, ES_317=%d,ES_317_cnt=%d,L_To_R=%d,R_To_L=%d, BOT_ratio=%d,\n"
													 	 ,rmv_cond,rmv_cnt,pParam->u8_rFB_panning_cnt_thl, rmv_unconf, pParam->u8_rFB_panning_unconf_thl,_ABS_(rmv_x),_ABS_(rmv_x_delta),rFB_alpha_tmp,rFB_level_tmp,
													 	 u25_rgnSad_rb,u20_rgnDtl_rb,u22_rTC_rb,u13_rPRD_rb,u26_aAPLp_rb,ES_Flag_364,ES_Flag_354,ES_Flag_317,ES_317_cnt,L_To_R_Paning,R_To_L_Paning,BOT_ratio);
									}
									#endif
									#if 0
									if(ES_Test_3==1)
									{
										rtd_pr_memc_debug("[YE_MEMC_FLAG] ,u8_ES_439_ID_flag=%d,u8_ES_413_ID_flag=%d,ES_Test_0 =%d ,ES_Test_1 =%d,rmv_cond=%d,  ES_Flag_21 =%d,  ES_Flag_380=%d,  ES_Flag_413=%d, ES_Test_29=%d, ES_Flag_364=%d, ES_Flag_354=%d, ES_Flag_317=%d, ES_Test_28=%d, ES_Flag_439=%d, ES_Flag_421=%d, \n"
										,pOutput->u8_ES_439_ID_flag,pOutput->u8_ES_413_ID_flag,ES_Test_0,ES_Test_1 ,rmv_cond,ES_Flag_21,ES_Flag_380, ES_Flag_413, ES_Test_29,ES_Flag_364,ES_Flag_354,
										 ES_Flag_317,ES_Test_28,ES_Flag_439,ES_Flag_421);

										rtd_pr_memc_debug("[YE_MEMC_RMV] ,gmv_rassia_cnt=%d ,gmv_rassia_flag=%d,u11_gmv_mvx=%d,gmv_mvy=%d,rmv_08_x_delta=%d, 8_mvx =%d ,08_mvy=%d,9_mvx =%d,10_mvx=%d, 11_mvx=%d,16_mvx=%d,17_mvx=%d, 18_mvx=%d, 19_mvx=%d,20_mvx=%d,20_mvy=%d, 21_mvx=%d,22_mvx=%d,23_mvx=%d,421_flag=%d, 21_flag=%d,\n"
										,gmv_rassia_cnt,gmv_rassia_flag,u11_gmv_mvx,u10_gmv_mvy,_ABS_(rmv_08_x_delta),u11_rmv_08_mvx,u10_rmv_08_mvy,u11_rmv_09_mvx,u11_rmv_10_mvx,u11_rmv_11_mvx,u11_rmv_16_mvx,u11_rmv_17_mvx,u11_rmv_18_mvx,u11_rmv_19_mvx,u11_rmv_20_mvx,u10_rmv_20_mvy,
										u11_rmv_21_mvx,u11_rmv_22_mvx,u11_rmv_23_mvx,ES_Flag_421,ES_Flag_21);

										 rtd_pr_memc_debug("[YE_MEMC_INFO] ,gmv_rassia_cnt=%d ,gmv_rassia_flag=%d,u11_gmv_mvx=%d,gmv_mvy=%d ,ES_Flag_317_Panning_reverse=%d,over_50_cnt=%d,317_over_50_flag=%d, sc_true=%d,rmv_cond =%d, rmv_cnt =%d,  panning_cnt_thl =%d,  rmv_unconf=%d,  panning_unconf_thl=%d, rmv_x=%d, rmv_x_delta=%d, alpha_tmp=%d, level_tmp=%d	,rgnSad_rb=%d, rgnDtl_rb=%d, rTC_rb=%d, rPRD_rb=%d, aAPLp_rb=%d, ES_364=%d, ES_354=%d, ES_317=%d,ES_317_cnt=%d,L_To_R=%d,R_To_L=%d, BOT_ratio=%d,Flag_317_temp=%d, \n"
										,gmv_rassia_cnt,gmv_rassia_flag,u11_gmv_mvx,u10_gmv_mvy, ES_Flag_317_Panning_reverse,gmv_317_over_50_cnt,gmv_317_over_50_flag,s_pContext->_output_fblevelctrl.u1_HW_sc_true,rmv_cond,rmv_cnt,pParam->u8_rFB_panning_cnt_thl, rmv_unconf, pParam->u8_rFB_panning_unconf_thl,_ABS_(rmv_x),_ABS_(rmv_x_delta),rFB_alpha_tmp,rFB_level_tmp,
										 u25_rgnSad_rb,u20_rgnDtl_rb,u22_rTC_rb,u13_rPRD_rb,u26_aAPLp_rb,ES_Flag_364,ES_Flag_354,ES_Flag_317,ES_317_cnt,L_To_R_Paning,R_To_L_Paning,BOT_ratio,ES_Flag_317_temp);

										rtd_pr_memc_debug("[YE_MEMC_REV] ,j =%d ,gmv_mvx=%d,gmv_mvy=%d,8_mvx =%d ,rmv_08_temp[119]=%d ,rmv_08_temp[0]=%d ,rmv_08_x_delta=%d ,ES_Flag_317_Panning_reverse=%d, ES_317_cnt=%d ,L_to_R_cnt=%d,R_to_L_cnt=%d ,L_To_R=%d ,R_To_L=%d, \n "
										,j,u11_gmv_mvx,u10_gmv_mvy,u11_rmv_08_mvx, rmv_08_temp[119], rmv_08_temp[0], rmv_08_x_delta, ES_Flag_317_Panning_reverse, ES_317_cnt 
										,rmv_08_x_delta_L_to_R_cnt,rmv_08_x_delta_R_to_L_cnt,L_To_R_Paning,R_To_L_Paning);

									}
									#endif
							}
							#endif
							else //20200422 new add
								{
												pOutput->u8_ES_flag2=0;
												pOutput->u8_ES_DEHALO_flag=0;
												pOutput->u8_ES_flag=0;
												pOutput->u8_ES_439_ID_flag=0;
												pOutput->u8_ES_21_ID_flag=0;
												pOutput->u8_ES_380_ID_flag=0;
												pOutput->u8_ES_413_ID_flag=0;
												pOutput->u8_ES_421_ID_flag=0;
												pOutput->u8_ES_Rassia_ID_flag=0;
												pOutput->u8_ES_040_ID_flag=0;
												//===========YE Test for 439

												#if 0//move it to writeComReg
												//WriteRegister(FRC_TOP__PQL_0__pql_patch_dummy_e4_ADDR, 7, 7, 0x0);//u1_localfb_wrt_en;
												WriteRegister(MC_MC_28_reg, 14, 14,0x0);
												WriteRegister(KMC_BI_bi_top_01_reg,2,3,0x1);
												//WriteRegister(FRC_TOP__MC__mc_fb_lvl_force_value_ADDR,15,22,0x0);
													 //it's ori settings
												WriteRegister(KME_DEHALO_KME_DEHALO_F0_reg, 24, 31, 0x08); 
								        			WriteRegister(KME_DEHALO_KME_DEHALO_F4_reg, 24, 31, 0x60);
												WriteRegister(KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 0x0A); 
											      WriteRegister(KME_DEHALO_KME_DEHALO_FC_reg, 0, 7, 0x80);
												#endif
								}
					    }
					else
					   {  //this is original settings

									if(ES_Test_3==1)
									{
										 rtd_pr_memc_debug("[YE_MEMC_FLAG] , !!!!JAY Setting !!!!,u8_ES_439_ID_flag=%d,u8_ES_413_ID_flag=%d,ES_Test_0 =%d ,ES_Test_1 =%d,rmv_cond=%d,  ES_Flag_21 =%d,  ES_Flag_380=%d,  ES_Flag_413=%d, ES_Test_29=%d, ES_Flag_364=%d, ES_Flag_354=%d, ES_Flag_317=%d, ES_Test_28=%d, ES_Flag_439=%d, ES_Flag_421=%d, \n"
										 ,pOutput->u8_ES_439_ID_flag,pOutput->u8_ES_413_ID_flag,ES_Test_0,ES_Test_1 ,rmv_cond,ES_Flag_21,ES_Flag_380, ES_Flag_413, ES_Test_29,ES_Flag_364,ES_Flag_354,
										 ES_Flag_317,ES_Test_28,ES_Flag_439,ES_Flag_421);
									}

									
								rFB_alpha_tmp = 255;
								rFB_level_tmp = 0;
								pOutput->u8_ES_flag=1;
									#if 0
									if(ES_Test_4==1)
									{
														 rtd_pr_memc_debug("[YE_MEMC] ,rmv_cond =%d, rmv_cnt =%d,  panning_cnt_thl =%d,  rmv_unconf=%d,  panning_unconf_thl=%d, rmv_x=%d, rmv_x_delta=%d, alpha_tmp=%d, level_tmp=%d	,rgnSad_rb=%d, rgnDtl_rb=%d, rTC_rb=%d, rPRD_rb=%d, aAPLp_rb=%d, ES_364=%d, ES_354=%d, ES_317=%d,ES_317_cnt=%d,L_To_R=%d,R_To_L=%d, BOT_ratio=%d,\n"
													 	 ,rmv_cond,rmv_cnt,pParam->u8_rFB_panning_cnt_thl, rmv_unconf, pParam->u8_rFB_panning_unconf_thl,_ABS_(rmv_x),_ABS_(rmv_x_delta),rFB_alpha_tmp,rFB_level_tmp,
													 	 u25_rgnSad_rb,u20_rgnDtl_rb,u22_rTC_rb,u13_rPRD_rb,u26_aAPLp_rb,ES_Flag_364,ES_Flag_354,ES_Flag_317,ES_317_cnt,L_To_R_Paning,R_To_L_Paning,BOT_ratio);
									}
									if(ES_Test_6==1)
									{
										 				rtd_pr_memc_debug("[YE_MEMC_2] ,rmv_00_mvx =%d ,08_mvx=%d, 08_mvy=%d, 09_mvx=%d, 09_mvy=%d, 16_mvx=%d, 16_mvy=%d, 17_mvx=%d, 17_mvy=%d, 20_mvx=%d, 20_mvy=%d,\n"
										 				,u11_rmv_00_mvx,u11_rmv_08_mvx,u10_rmv_08_mvy,u11_rmv_09_mvx,u10_rmv_09_mvy,u11_rmv_16_mvx,u10_rmv_16_mvy,u11_rmv_17_mvx,u10_rmv_17_mvy,u11_rmv_20_mvx,u10_rmv_20_mvy);
										 		
									}
									#endif
					   }
				} 
				else
				{
					pOutput->u8_ES_flag2=0;
					pOutput->u8_ES_DEHALO_flag=0;
					pOutput->u8_ES_flag=0;
					pOutput->u8_ES_439_ID_flag=0;
					pOutput->u8_ES_21_ID_flag=0;
					pOutput->u8_ES_380_ID_flag=0;
					pOutput->u8_ES_413_ID_flag=0;
					pOutput->u8_ES_421_ID_flag=0;
					pOutput->u8_ES_Rassia_ID_flag=0;
					pOutput->u8_ES_040_ID_flag=0;
					//===========YE Test for 439

					#if 0 //move it to writeComReg
					//WriteRegister(FRC_TOP__PQL_0__pql_patch_dummy_e4_ADDR, 7, 7, 0x0);//u1_localfb_wrt_en;
					WriteRegister(MC_MC_28_reg, 14, 14,0x0);
					WriteRegister(KMC_BI_bi_top_01_reg,2,3,0x1);
					//WriteRegister(FRC_TOP__MC__mc_fb_lvl_force_value_ADDR,15,22,0x0);
						 //it's ori settings
					WriteRegister(KME_DEHALO_KME_DEHALO_F0_reg, 24, 31, 0x08); 
	        			WriteRegister(KME_DEHALO_KME_DEHALO_F4_reg, 24, 31, 0x60);
					WriteRegister(KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 0x0A); 
				      WriteRegister(KME_DEHALO_KME_DEHALO_FC_reg, 0, 7, 0x80);
					#endif
								#if 0
								if(ES_Test_7==1)
								{
													 rtd_pr_memc_debug("[YE_MEMC] ,rmv_cond =%d, rmv_cnt =%d,  panning_cnt_thl =%d,  rmv_unconf=%d,  panning_unconf_thl=%d, rmv_x=%d, rmv_x_delta=%d, alpha_tmp=%d, level_tmp=%d	,rgnSad_rb=%d, rgnDtl_rb=%d, rTC_rb=%d, rPRD_rb=%d, aAPLp_rb=%d, ES_364=%d, ES_354=%d, ES_317=%d,ES_317_cnt=%d,L_To_R=%d,R_To_L=%d, BOT_ratio=%d,\n"
												 	 ,rmv_cond,rmv_cnt,pParam->u8_rFB_panning_cnt_thl, rmv_unconf, pParam->u8_rFB_panning_unconf_thl,_ABS_(rmv_x),_ABS_(rmv_x_delta),rFB_alpha_tmp,rFB_level_tmp,
												 	 u25_rgnSad_rb,u20_rgnDtl_rb,u22_rTC_rb,u13_rPRD_rb,u26_aAPLp_rb,ES_Flag_364,ES_Flag_354,ES_Flag_317,ES_317_cnt,L_To_R_Paning,R_To_L_Paning,BOT_ratio);
								}
								if(ES_Test_8==1)
								{
									 				rtd_pr_memc_debug("[YE_MEMC_2] ,rmv_00_mvx =%d ,08_mvx=%d, 08_mvy=%d, 09_mvx=%d, 09_mvy=%d, 16_mvx=%d, 16_mvy=%d, 17_mvx=%d, 17_mvy=%d, 20_mvx=%d, 20_mvy=%d,\n"
									 				,u11_rmv_00_mvx,u11_rmv_08_mvx,u10_rmv_08_mvy,u11_rmv_09_mvx,u10_rmv_09_mvy,u11_rmv_16_mvx,u10_rmv_16_mvy,u11_rmv_17_mvx,u10_rmv_17_mvy,u11_rmv_20_mvx,u10_rmv_20_mvy);
									 		
								}
								#endif
				}		
			}
		//}
		else
		{
			pOutput->u8_ES_flag2=0;
			pOutput->u8_ES_DEHALO_flag=0;
			pOutput->u8_ES_flag=0;
			pOutput->u8_ES_439_ID_flag=0;
			pOutput->u8_ES_21_ID_flag=0;
			pOutput->u8_ES_380_ID_flag=0;
			pOutput->u8_ES_413_ID_flag=0;
			pOutput->u8_ES_421_ID_flag=0;
			pOutput->u8_ES_Rassia_ID_flag=0;
			pOutput->u8_ES_040_ID_flag=0;
			/*
			WriteRegister(FRC_TOP__MC__mc_fb_lvl_force_en_ADDR, 14, 14,0x0);
			WriteRegister(FRC_TOP__KMC_BI__bi_fb_mode_ADDR,2,3,0x1);
			WriteRegister(FRC_TOP__KME_DEHALO__dh_dtl_curve_x1_ADDR, 24, 31, 0x08); 
			WriteRegister(FRC_TOP__KME_DEHALO__dh_dtl_curve_x2_ADDR, 24, 31, 0x60);
			*/
		}
}   //YE Test to control ES Enable/Disable

		if(ES_Test_2==1) // Just for debug only
			{
				WriteRegister(SOFTWARE1_SOFTWARE1_23_reg, 0, 7, 0xFF);		// YE Test 
				WriteRegister(SOFTWARE1_SOFTWARE1_23_reg, 8, 15, 0xFF);		// YE Test 
			}


		
		if(ES_Test_3==1) // Just for debug only
			{
				WriteRegister(SOFTWARE1_SOFTWARE1_23_reg, 0, 7, 0x00);		// YE Test 
				WriteRegister(SOFTWARE1_SOFTWARE1_23_reg, 8, 15, 0x00);		// YE Test 
			}
	
		if(((rFB_alpha_tmp_gain==255)&&(rFB_alpha_tmp_offset==255))||(rFB_alpha_tmp>=255))
			rFB_alpha_tmp=255;
		
		if(((rFB_level_tmp_gain==255)&&(rFB_level_tmp_offset==255))||(rFB_level_tmp>=255))
			rFB_level_tmp=255;

		//o------ output of rFB alpha & level ------o
		pOutput->u8_rFB_alpha[i] = rFB_alpha_auto ? rFB_alpha_tmp : rFB_alpha_force_value;
		if(debug_en == 1){
			pOutput->u8_rFB_alpha[i] = (((debug_region>>i)&0x1)==1) ? rFB_alpha_force_value : rFB_alpha_tmp;
		}
		pOutput->u8_rFB_level[i] = rFB_level_auto ? rFB_level_tmp : rFB_level_force_value;
		
		#if 1
		if(ES_Test_11==1)
		{
		rtd_pr_memc_debug("[YE_MEMC_RFB_B] ,i=%d,rFB_alpha_auto=%d, rFB_alpha_tmp=%d,rFB_alpha_force_value=%d, u8_rFB_alpha[i]=%d,rFB_level_auto=%d,rFB_level_tmp=%d,rFB_level_force_value=%d,u8_rFB_level[i]=%d,\n"
		,i,rFB_alpha_auto,rFB_alpha_tmp,rFB_alpha_force_value,pOutput->u8_rFB_alpha[i],rFB_level_auto,rFB_level_tmp,rFB_level_force_value,pOutput->u8_rFB_level[i]);
		 
		}
		#endif
	}

	if(((log_en3>>10)&0x1) == 1){
		rtd_pr_memc_debug("[u8_rFB_alpha][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n",
			pOutput->u8_rFB_alpha[0], pOutput->u8_rFB_alpha[1], pOutput->u8_rFB_alpha[2], pOutput->u8_rFB_alpha[3],
			pOutput->u8_rFB_alpha[4], pOutput->u8_rFB_alpha[5], pOutput->u8_rFB_alpha[6], pOutput->u8_rFB_alpha[7],
			pOutput->u8_rFB_alpha[8], pOutput->u8_rFB_alpha[9], pOutput->u8_rFB_alpha[10], pOutput->u8_rFB_alpha[11],
			pOutput->u8_rFB_alpha[12], pOutput->u8_rFB_alpha[13], pOutput->u8_rFB_alpha[14], pOutput->u8_rFB_alpha[15],
			pOutput->u8_rFB_alpha[16], pOutput->u8_rFB_alpha[17], pOutput->u8_rFB_alpha[18], pOutput->u8_rFB_alpha[19],
			pOutput->u8_rFB_alpha[20], pOutput->u8_rFB_alpha[21], pOutput->u8_rFB_alpha[22], pOutput->u8_rFB_alpha[23],
			pOutput->u8_rFB_alpha[24], pOutput->u8_rFB_alpha[25], pOutput->u8_rFB_alpha[26], pOutput->u8_rFB_alpha[27],
			pOutput->u8_rFB_alpha[28], pOutput->u8_rFB_alpha[29], pOutput->u8_rFB_alpha[30], pOutput->u8_rFB_alpha[31]);
	}
	if(((log_en3>>10)&0x1) == 1){
		rtd_pr_memc_debug("[u8_rFB_level][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n",
			pOutput->u8_rFB_level[0], pOutput->u8_rFB_level[1], pOutput->u8_rFB_level[2], pOutput->u8_rFB_level[3],
			pOutput->u8_rFB_level[4], pOutput->u8_rFB_level[5], pOutput->u8_rFB_level[6], pOutput->u8_rFB_level[7],
			pOutput->u8_rFB_level[8], pOutput->u8_rFB_level[9], pOutput->u8_rFB_level[10], pOutput->u8_rFB_level[11],
			pOutput->u8_rFB_level[12], pOutput->u8_rFB_level[13], pOutput->u8_rFB_level[14], pOutput->u8_rFB_level[15],
			pOutput->u8_rFB_level[16], pOutput->u8_rFB_level[17], pOutput->u8_rFB_level[18], pOutput->u8_rFB_level[19],
			pOutput->u8_rFB_level[20], pOutput->u8_rFB_level[21], pOutput->u8_rFB_level[22], pOutput->u8_rFB_level[23],
			pOutput->u8_rFB_level[24], pOutput->u8_rFB_level[25], pOutput->u8_rFB_level[26], pOutput->u8_rFB_level[27],
			pOutput->u8_rFB_level[28], pOutput->u8_rFB_level[29], pOutput->u8_rFB_level[30], pOutput->u8_rFB_level[31]);
	}

		for(k=0;k<32;k++)
			{
				u25_rgnSad_rb_R_32total=u25_rgnSad_rb_R_32total+u25_rgnSad_rb_R[k];
				u20_rgnDtl_rb_R_32total=u20_rgnDtl_rb_R_32total+u20_rgnDtl_rb_R[k];
				u22_rTC_rb_R_32total=u22_rTC_rb_R_32total+u22_rTC_rb_R[k];
				u13_rPRD_rb_R_32total=u13_rPRD_rb_R_32total+u13_rPRD_rb_R[k];
				if(k==31)
				{
					u25_rgnSad_rb_R_avg_temp=u25_rgnSad_rb_R_32total/32;
					u20_rgnDtl_rb_R_avg_temp=u20_rgnDtl_rb_R_32total/32;
					u22_rTC_rb_R_avg_temp=u22_rTC_rb_R_32total/32;
					u13_rPRD_rb_R_avg_temp=u13_rPRD_rb_R_32total/32;
				}
				u25_rgnSad_rb_R_avg=u25_rgnSad_rb_R_avg_temp;
				u20_rgnDtl_rb_R_avg =u20_rgnDtl_rb_R_avg_temp;
				u22_rTC_rb_R_avg=u22_rTC_rb_R_avg_temp;
				u13_rPRD_rb_R_avg=u13_rPRD_rb_R_avg_temp;
				#if 1
				if(ES_Test_30==1)
				{
					 rtd_pr_memc_debug("[YE_MEMC_INFO_R] ,i=%d,k=%d,rgnSad_rb_R[k]=%d,rgnSad_rb_R_32total=%d,rgnSad_rb_R_avg=%d,\
					 	     rgnDtl_rb_R[k]=%d,rgnDtl_rb_R_32total=%d,rgnDtl_rb_R_avg=%d,\
					 	     rTC_rb_R[k]=%d,rTC_rb_R_32total=%d,rTC_rb_R_avg=%d,\
					 	     rPRD_rb_R[k]=%d,rPRD_rb_R_32total=%d,rPRD_rb_R_avg=%d,\n"
				 	,i,k,u25_rgnSad_rb_R[k],u25_rgnSad_rb_R_32total,u25_rgnSad_rb_R_avg
				 	,u20_rgnDtl_rb_R[k],u20_rgnDtl_rb_R_32total,u20_rgnDtl_rb_R_avg
				 	,u22_rTC_rb_R[k],u22_rTC_rb_R_32total,u22_rTC_rb_R_avg
				 	,u13_rPRD_rb_R[k],u13_rPRD_rb_R_32total,u13_rPRD_rb_R_avg);
				}
				#endif

				
			}
			u25_rgnSad_rb_R_32total=0;
			u20_rgnDtl_rb_R_32total=0;
			u22_rTC_rb_R_32total=0;
			u13_rPRD_rb_R_32total=0;
			
			u25_rgnSad_rb_R_avg_temp=0;
			u20_rgnDtl_rb_R_avg_temp=0;
			u22_rTC_rb_R_avg_temp=0;
			u13_rPRD_rb_R_avg_temp=0;

			
	#if 0
	j++;
	if(j>=121){
		j=0;
	}
	#endif

	
	for(j = 0; j < 119; j++){
		rmv_08_temp[j]=rmv_08_temp[j+1];
		gmv_mvx_temp[j]=gmv_mvx_temp[j+1];
		gmv_mvy_temp[j]=gmv_mvy_temp[j+1];

		rmv_20_y_temp[j]=rmv_20_y_temp[j+1];
	}
	rmv_08_temp[119]=u11_rmv_08_mvx;
	rmv_08_x_delta = rmv_08_temp[119] - rmv_08_temp[0];

	//rmv_20_y_temp[60]=u10_rmv_20_mvy;
	//rmv_20_y_delta=rmv_20_y_temp[60]-rmv_20_y_temp[0];
	rmv_20_y_temp[30]=u10_rmv_20_mvy;
	rmv_20_y_delta=rmv_20_y_temp[30]-rmv_20_y_temp[0];	


	//static signed short gmv_x_delta=0,gmv_x_delta_L_to_R_cnt=0,gmv_x_delta_R_to_L_cnt=0; // rmv_08_x_current=0,rmv_08_x_pre=0,


	#if 0
	if(((gmv_mvx_temp[1]-gmv_mvx_temp[0])>0)&&(gmv_x_delta_L_to_R_cnt<=10)){
	gmv_x_delta_L_to_R_cnt++;
		if (gmv_x_delta_R_to_L_cnt>0)
			gmv_x_delta_R_to_L_cnt--;
	}

	if(((gmv_mvx_temp[1]-gmv_mvx_temp[0])<0)&&(gmv_x_delta_R_to_L_cnt<=10)){
	gmv_x_delta_R_to_L_cnt++;
		if(gmv_x_delta_L_to_R_cnt>0)
			gmv_x_delta_L_to_R_cnt--;
	}

		if((L_To_R_Paning==1)&&(gmv_x_delta_R_to_L_cnt>0))
			ES_Flag_317_Panning_reverse=1;
		else if((R_To_L_Paning==1)&&(gmv_x_delta_L_to_R_cnt>0))
			ES_Flag_317_Panning_reverse=1;
		else
			ES_Flag_317_Panning_reverse=0;

	
	if(gmv_x_delta_L_to_R_cnt>0)
		{
			L_To_R_Paning=1;
			gmv_x_delta_L_to_R_cnt=0;
		}
	else if(gmv_x_delta_R_to_L_cnt==1)
		{
			L_To_R_Paning=0;	
		}
	
	if(gmv_x_delta_R_to_L_cnt>0)
		{
			R_To_L_Paning=1;
			gmv_x_delta_R_to_L_cnt=0;
		}
	else if(gmv_x_delta_L_to_R_cnt==1)
		{
			R_To_L_Paning=0;	
		}
	#endif

#if 0
	//======YE Test 
//	if(((rmv_08_temp[119]-rmv_08_temp[0])>0)&&(rmv_08_x_delta_L_to_R_cnt<10)&&(rmv_08_x_delta_R_to_L_cnt>0)){
	if(((rmv_08_temp[1]-rmv_08_temp[0])>0)&&(rmv_08_x_delta_L_to_R_cnt<=10)){
	rmv_08_x_delta_L_to_R_cnt++;
		if (rmv_08_x_delta_R_to_L_cnt>0)
			rmv_08_x_delta_R_to_L_cnt--;
	}

	if(((rmv_08_temp[1]-rmv_08_temp[0])<0)&&(rmv_08_x_delta_R_to_L_cnt<=10)){
	rmv_08_x_delta_R_to_L_cnt++;
		if(rmv_08_x_delta_L_to_R_cnt>0)
			rmv_08_x_delta_L_to_R_cnt--;
	}

		if((L_To_R_Paning==1)&&(rmv_08_x_delta_R_to_L_cnt>0))
			ES_Flag_317_Panning_reverse=1;
		else if((R_To_L_Paning==1)&&(rmv_08_x_delta_L_to_R_cnt>0))
			ES_Flag_317_Panning_reverse=1;
		else
			ES_Flag_317_Panning_reverse=0;

	
	if(rmv_08_x_delta_L_to_R_cnt>0)
		{
			L_To_R_Paning=1;
			rmv_08_x_delta_L_to_R_cnt=0;
		}
	else if(rmv_08_x_delta_R_to_L_cnt==10)
		{
			L_To_R_Paning=0;	
		}
	
	if(rmv_08_x_delta_R_to_L_cnt>0)
		{
			R_To_L_Paning=1;
			rmv_08_x_delta_R_to_L_cnt=0;
		}
	else if(rmv_08_x_delta_L_to_R_cnt==10)
		{
			R_To_L_Paning=0;	
		}
#endif
	
	//=====================
			#if 0
				if(ES_Test_11==1)
				rtd_pr_memc_debug("[YE_MEMC_REV] ,j =%d ,8_mvx =%d ,rmv_08_temp[119]=%d ,rmv_08_temp[0]=%d ,rmv_08_x_delta=%d ,ES_Flag_317_Panning_reverse=%d, ES_317_cnt=%d ,L_to_R_cnt=%d,R_to_L_cnt=%d ,L_To_R=%d ,R_To_L=%d, \n ",
				j,u11_rmv_08_mvx, rmv_08_temp[119], rmv_08_temp[0], rmv_08_x_delta, ES_Flag_317_Panning_reverse, ES_317_cnt 
				,rmv_08_x_delta_L_to_R_cnt,rmv_08_x_delta_R_to_L_cnt,L_To_R_Paning,R_To_L_Paning);
			#endif

	


	//if( ((s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb > pParam->u8_rFB_panning_unconf_thl) && ((s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb >> 8) > pParam->u8_rFB_panning_cnt_thl)) )
			//((s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb <= 1) && ((s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb >> 8) > 110)) )
	//if(s_pContext->_output_me_sceneAnalysis.u2_panning_flag >= 1)
	//	pOutput->u1_rFB_panning = 1;
	//else
	//	pOutput->u1_rFB_panning = 0;

	//pOutput->u1_rFB_panning = (rFB_panning_en) ? (pOutput->u1_rFB_panning) : 0;
}

#endif


