#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_isr/PQL/FRC_glb_Context.h"
#include <memc_isr/PQL/FBLevelCtrl.h>
#include "memc_reg_def.h"
//#ifdef BUILD_QUICK_SHOW
//#include <div64.h>
//#endif

extern VOID GFBCtrl_Proc_RTK2885pp(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput);
VOID FBLevelCtrl_Init_RTK2885pp(_OUTPUT_FBLevelCtrl *pOutput)
{
	pOutput->u8_cross_scene              = 0;
}

VOID FBLevelCalc_RTK2885pp(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	GFBCtrl_Proc_RTK2885pp(pParam, pOutput);	
}

//Input signal and parameters are connected locally
VOID FBSystemCtrl_Proc_RTK2885pp(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
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

//	if((16>=s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply) && (s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply>=6))
//		u8_flbk_lvl_OSDPictureModeChg = _FB_LEVEL_MAX_;
//	if((5>=s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply) && (s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply>0))
//		u8_flbk_lvl_OSDPictureModeChg = _MIN_(_FB_LEVEL_MAX_, (_FB_LEVEL_MAX_/5)*s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply);
	if(s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply>0)
		u8_flbk_lvl_OSDPictureModeChg = 0xF0;
	pOutput->u8_FBSystem =  _MAX_(_MAX_(_MAX_(_MAX_(_MAX_(_MAX_(_MAX_(u8_flbk_lvl_FrameRepeatMode
							, u8_flbk_lvl_HDR_PicChg)
							, u8_flbk_lvl_OSDBrightnessBigChg)
							, u8_flbk_lvl_OSDContrastBigChg)
							, u8_flbk_lvl_cad)
							, u8_flbk_lvl_voseek)
		                                   , u8_flbk_lvl_EPG)
							, u8_flbk_lvl_OSDPictureModeChg);

}

VOID GFBCtrl_Proc_RTK2885pp(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
	// const _PQLCONTEXT *s_pContext = GetPQLContext();
	// unsigned int u32_RB_val;
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int u11_GMV_1st_mvx = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	unsigned int u10_GMV_1st_mvy = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
	unsigned int u16_GMV_1st_cnt = s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb;
	//unsigned int u10_GMV_1st_ucf = s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb;	
	unsigned int u11_GMV_2nd_mvx = s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb;
	unsigned int u10_GMV_2nd_mvy = s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb;
	unsigned int u16_GMV_2nd_cnt = s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb;
	//unsigned int u10_GMV_2nd_ucf = s_pContext->_output_read_comreg.u12_me_GMV_2nd_unconf_rb;	

	unsigned int glb_apli =  s_pContext->_output_read_comreg.u26_me_glb_apli_rb;
	//unsigned int glb_aplp =  s_pContext->_output_read_comreg.u26_me_glb_aplp_rb;
	unsigned int glb_sad =  s_pContext->_output_read_comreg.u30_me_glb_sad_rb;
	unsigned int glb_scss =  s_pContext->_output_read_comreg.u28_me_glb_scss_rb;
	//unsigned int glb_tcss =  s_pContext->_output_read_comreg.u28_me_glb_tcss_rb;
	unsigned int glb_dtl =  s_pContext->_output_read_comreg.u26_me_glb_dtl_rb;
	unsigned int rgnIdx = 0,rgn_dtl[32]={0},rgn_sad[32]={0},sad_dtl_coef[32]={0},sum_sad_dtl_coef = 0,glb_sad_dtl_coef = 0;
	int gmv_1st_max= 0,gmv_2nd_max= 0 ;
	bool gfb_in_cond_1 = 0,gfb_in_cond_2 = 0,gfb_in = 0,gfb_cond2_test=0;
	static int glb_in_cnt = 0;
	gmv_1st_max = _MAX_(u11_GMV_1st_mvx,u10_GMV_1st_mvy);
	gmv_2nd_max = _MAX_(u11_GMV_2nd_mvx,u10_GMV_2nd_mvy);
	for(rgnIdx=0;rgnIdx<32;rgnIdx++)
	{
		rgn_dtl[rgnIdx] = s_pContext->_output_read_comreg.u20_me_rgn_dtl_rb[rgnIdx];
		rgn_sad[rgnIdx] = s_pContext->_output_read_comreg.u25_me_rgn_sad_rb[rgnIdx];
		if(rgn_dtl[rgnIdx]>0)
		{
			sad_dtl_coef[rgnIdx] = rgn_sad[rgnIdx]/rgn_dtl[rgnIdx];
		}
		sum_sad_dtl_coef = sum_sad_dtl_coef + sad_dtl_coef[rgnIdx];
	}
	if(glb_dtl>0)
		glb_sad_dtl_coef = glb_sad/glb_dtl;

	gfb_in_cond_1 = (glb_sad>1300000 && (gmv_1st_max>150 || gmv_1st_max<10) && glb_scss>700000 && sum_sad_dtl_coef>150 && glb_sad_dtl_coef>3 && (glb_apli<2900000 || glb_apli>5000000) && u16_GMV_1st_cnt>7500);
	gfb_cond2_test = (glb_scss>540000 && gmv_1st_max>38)==0;
	gfb_in_cond_2 = (gfb_in_cond_1 || (glb_dtl<300000 && glb_sad_dtl_coef>4 && u16_GMV_1st_cnt>13000 && gfb_cond2_test)) ;
	gfb_in = (gfb_in_cond_2 || (glb_dtl>550000 && sum_sad_dtl_coef>55 && gmv_1st_max>80 && u16_GMV_1st_cnt>8000 && gmv_2nd_max>140 && u16_GMV_2nd_cnt>6500 && glb_dtl<800000 && glb_scss>750000));
	if(gfb_in == 1)
	{
		pOutput->u1_gfb_detect = 1;
		glb_in_cnt = 20;
	}
	else if(glb_in_cnt>0)
	{
		pOutput->u1_gfb_detect = 1;
		glb_in_cnt--;
	}
	else
	{
		pOutput->u1_gfb_detect = 0;
	}

}


VOID FBLevelCtrl_Proc_RTK2885pp(const _PARAM_FBLevelCtrl *pParam, _OUTPUT_FBLevelCtrl *pOutput)
{
//	const _PQLCONTEXT *s_pContext = GetPQLContext();
	//LogPrintf(DBG_MSG, "\r\n");

	if (pParam->u1_FBLevelCtrl_en == 1)
	{
		FBLevelCalc_RTK2885pp(pParam, pOutput);
	}
	else
	{
		pOutput->u8_FBLevel = 0;
	}

	//For system fallback
	FBSystemCtrl_Proc_RTK2885pp(pParam, pOutput);
	//For global fallback
	



	
}


