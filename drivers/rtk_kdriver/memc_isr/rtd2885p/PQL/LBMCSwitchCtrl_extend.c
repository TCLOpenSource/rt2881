#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_reg_def.h"

#if 1//can move Mc_lbmcSwitch.c
static VOID LbmcMode_Detect_onlyCnt_RTK2885pp(const _PARAM_MC_LBMC_SWITCH *pParam, _OUTPUT_MC_LBMC_SWITCH *pOutput, unsigned char band_Type)
{
    const _PQLCONTEXT *s_pContext = GetPQLContext();

    signed short max_mvy=0, min_mvy=0;
    signed short min_t=0, max_t=0;
    signed short max_rmvy=0, min_rmvy=0;
    int i=0;

    for(i=0; i<32; i++)
    {
        min_t = _MIN_(_MIN_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[i]), s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[i]);
        max_t = _MAX_(_MAX_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[i]), s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[i]);
        if(min_t < min_rmvy)
            min_rmvy = min_t;
        if(max_t > max_rmvy)
            max_rmvy = max_t;       
    }

    max_mvy = _MAX_(_MAX_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb, s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb), s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb);
    max_mvy = _MAX_(max_mvy, max_rmvy);
    min_mvy = _MIN_(_MIN_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb, s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb), s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb);
    min_mvy = _MIN_(min_mvy, min_rmvy);

    if((s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb < -76) || ((max_mvy < 0) && (min_mvy <- 38)))
    {
        pOutput->u8_lf_lbmcMode = _MC_SINGLE_UP_;
        pOutput->u8_hf_lbmcMode = _MC_SINGLE_UP_;
    }
    else if((s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb > 76) || ((min_mvy > 0) && (max_mvy > 38))) 
    {   
        pOutput->u8_lf_lbmcMode = _MC_SINGLE_DOWN_;
        pOutput->u8_hf_lbmcMode = _MC_SINGLE_DOWN_;
    }
    else
    {   
        pOutput->u8_lf_lbmcMode = _MC_NORMAL_;
        pOutput->u8_hf_lbmcMode = _MC_NORMAL_;
    }

    if(pParam->u1_lbmc_log_en)
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d] , [%d] , [%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u8_lf_lbmcMode, max_mvy, min_mvy);    
}
//////////////////////////////////////////////////////////////////////////

VOID LBMCSwitchCtrl_Init_RTK2885pp( _OUTPUT_MC_LBMC_SWITCH *pOutput)
{
    pOutput->u8_lf_lbmcMode = _MC_NORMAL_;
    pOutput->u8_hf_lbmcMode = _MC_NORMAL_;
}

VOID LBMCSwitchCtrl_Proc_RTK2885pp(const _PARAM_MC_LBMC_SWITCH *pParam, _OUTPUT_MC_LBMC_SWITCH *pOutput)
{
    /*const _PQLCONTEXT *s_pContext = GetPQLContext();*/

    if (pParam->u8_lbmcSwitch_bypass == 0)
    {
        LbmcMode_Detect_onlyCnt_RTK2885pp(pParam, pOutput, _LBMC_LF_);
        //LbmcMode_Detect_onlyCnt_RTK2885pp(pParam, pOutput, _LBMC_HF_);
    }
    else
    {
        pOutput->u8_lf_lbmcMode = pParam->u8_lbmcSwitch_bypass - 1;
        pOutput->u8_hf_lbmcMode = pParam->u8_lbmcSwitch_bypass - 1;
    }

    if(pParam->u1_lbmc_log_en)
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u8_lf_lbmcMode);

}
#endif


