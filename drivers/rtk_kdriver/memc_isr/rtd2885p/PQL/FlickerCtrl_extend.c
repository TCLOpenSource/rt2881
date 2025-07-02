#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_reg_def.h"

VOID FlickerCtrl_Init_RTK2885pp(_OUTPUT_FLICKER_CTRL *pOutput)
{
	pOutput->u1_detect = 0;
}

VOID FlickerCtrl_Proc_RTK2885pp(const _PARAM_FLICKER_CTRL *pParam, _OUTPUT_FLICKER_CTRL *pOutput)
{
    unsigned int print_in_func;
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    
    MEMC_MC_Deflicker_Calc(pParam, pOutput);
}

VOID MEMC_MC_Deflicker_Calc(const _PARAM_FLICKER_CTRL *pParam, _OUTPUT_FLICKER_CTRL *pOutput)
{
    const _PQLCONTEXT *s_pContext = GetPQLContext();
    unsigned char u8_i, gmv_rmv_grp0_diff_cnt=0, rmv_diff_cnt=0;
    unsigned short flicker_cnt1=0, flicker_cnt2=0, flicker_cnt3=0, gmv_rmv_grp0_diff, rmv_diff, hsty_mvd;
    short max_gmvx, max_gmvy, min_gmvx, min_gmvy;
    static unsigned char lpf_coef=0, flicker_intp_cnt = 0, panning_cnt=0;
    static short hsty_gmvy[15]={0}, hsty_gmvx[15]={0};
    bool panning_cond;
    unsigned int print_in_func;

    if (pParam->u1_mc_deflicker_calc_en == 0)
        return;
    
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    for (u8_i = 0; u8_i < 32; u8_i++)
    {
        if (s_pContext->_output_read_comreg.u15_mc_local_lpf_cnt_rb[u8_i] > 500)
            flicker_cnt1++;
        if (s_pContext->_output_read_comreg.u15_mc_local_lpf_cnt_rb[u8_i] > 1000)
            flicker_cnt2++;
        if (s_pContext->_output_read_comreg.u15_mc_local_lpf_cnt_rb[u8_i] > 2000)
            flicker_cnt3++;

        gmv_rmv_grp0_diff = _MAX_(_ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb, s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i]),
                                  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb, s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i]));
        rmv_diff = _MAX_(_ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i]),
                         _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i]));
        if (gmv_rmv_grp0_diff < 2)
            gmv_rmv_grp0_diff_cnt++;
        if (!((rmv_diff >= 2) && (s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[u8_i] > 50)))
            rmv_diff_cnt++;
    }

    
    lpf_coef = (lpf_coef * 7 + _CLIP_(flicker_cnt1, 0, 19) + 4) >> 3;
    if (s_pContext->_output_me_sceneAnalysis.u6_unstable_score_cnt > 0)
    {
        lpf_coef++;
    }
    else
    {
        lpf_coef--;
    }
    lpf_coef = _CLIP_(lpf_coef, 0, 8);
    
    if (flicker_cnt2 > 0)
    {
        flicker_intp_cnt += 2;
    }
    else
    {
        flicker_intp_cnt--;
    }
    flicker_intp_cnt = _CLIP_(flicker_intp_cnt , 0, 31);

    max_gmvy = _MAX_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb, hsty_gmvy[0]);
    max_gmvx = _MAX_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb, hsty_gmvx[0]);
    min_gmvy = _MIN_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb, hsty_gmvy[0]);
    min_gmvx = _MIN_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb, hsty_gmvx[0]);
    
    
    for (u8_i = 0; u8_i < 14; u8_i++)
    {
        hsty_gmvy[u8_i] = hsty_gmvy[u8_i+1];
        hsty_gmvx[u8_i] = hsty_gmvx[u8_i+1];
        max_gmvy = _MAX_(max_gmvy, hsty_gmvy[u8_i+1]);
        max_gmvx = _MAX_(max_gmvx, hsty_gmvx[u8_i+1]);
        min_gmvy = _MIN_(min_gmvy, hsty_gmvy[u8_i+1]);
        min_gmvx = _MIN_(min_gmvx, hsty_gmvx[u8_i+1]);
    }

    hsty_mvd = _MAX_((max_gmvy - min_gmvy), (max_gmvx - min_gmvx));

    panning_cond = (gmv_rmv_grp0_diff_cnt > 31) && \
                   (rmv_diff_cnt > 31) && \
                   (_MAX_(_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb), _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb)) > 3) && \
                   (hsty_mvd < 1);
    if (panning_cond)
        panning_cnt++;
    else
        panning_cnt=0;
    panning_cnt = _CLIP_(panning_cnt, 0, 64);
    
    pOutput->flicker_cnt1 = flicker_cnt1;
    pOutput->flicker_cnt2 = flicker_cnt2;
    pOutput->flicker_cnt3 = flicker_cnt3;
    pOutput->flicker_intp_cnt = flicker_intp_cnt;
    pOutput->flicker_panning_cnt = panning_cnt;
    pOutput->ipme_lpf_coef = lpf_coef;
}

