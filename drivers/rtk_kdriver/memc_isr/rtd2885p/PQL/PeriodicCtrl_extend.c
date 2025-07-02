#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_reg_def.h"

VOID PeriodicCtrl_Init_RTK2885pp(_OUTPUT_PERIODIC_CTRL *pOutput)
{
	M8P_rtme_hme1_top0_rtme_hme1_top0_b0_RBUS M8P_rtme_hme1_top0_rtme_hme1_top0_b0_reg;
	M8P_rtme_hme1_top1_rtme_hme1_top1_b0_RBUS M8P_rtme_hme1_top1_rtme_hme1_top1_b0_reg;
	M8P_rtme_hme1_top2_rtme_hme1_top2_b0_RBUS M8P_rtme_hme1_top2_rtme_hme1_top2_b0_reg;
	M8P_rtme_hme1_top3_rtme_hme1_top3_90_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_90_reg;

	M8P_rtme_hme1_top0_rtme_hme1_top0_b0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP0_RTME_HME1_TOP0_B0_reg);
	M8P_rtme_hme1_top1_rtme_hme1_top1_b0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP1_RTME_HME1_TOP1_B0_reg);
	M8P_rtme_hme1_top2_rtme_hme1_top2_b0_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP2_RTME_HME1_TOP2_B0_reg);
	M8P_rtme_hme1_top3_rtme_hme1_top3_90_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_90_reg);

	pOutput->u1_layer0_frq_en_ori = M8P_rtme_hme1_top0_rtme_hme1_top0_b0_reg.me1_layer0_frq_en;
	pOutput->u1_layer1_frq_en_ori = M8P_rtme_hme1_top1_rtme_hme1_top1_b0_reg.me1_layer1_frq_en;
	pOutput->u1_layer2_frq_en_ori = M8P_rtme_hme1_top2_rtme_hme1_top2_b0_reg.me1_layer2_frq_en;
	pOutput->u1_layer3_frq_en_ori = M8P_rtme_hme1_top3_rtme_hme1_top3_90_reg.me1_layer3_frq_en;

	pOutput->u1_detect_mode = 0;
}

VOID PeriodicCtrl_Proc_RTK2885pp(const _PARAM_PERIODIC_CTRL *pParam, _OUTPUT_PERIODIC_CTRL *pOutput)
{
    const _PQLCONTEXT *s_pContext = GetPQLContext();
    unsigned int u8_rgn_idx_x = 0, u8_rgn_idx_y = 0, u32_rgn_blk_cnt = 0, u32_total_blk_cnt = 0;
    unsigned int u10_region_w[8];
    unsigned int u10_region_h[4];
    unsigned int u32_total_frq_cnt = 0;
    unsigned short u5_frq_region_cnt = 0, u5_frq_region_small_cnt = 0, u5_frq_region_cnt2 = 0;
    bool u1_repeat_coef_cond = 0, u1_sum_cond = 0, u1_repeat_not_many = 0, u1_gmv_z_stable = 0;
    unsigned int u32_avgbv_mvdx[32] = {0}, u32_avgbv_mvdy[32] = {0};
    unsigned short u4_sc_th = 0;
    unsigned int u32_avgbv_unconf_sum = 0;
    unsigned char u8_i = 0, u8_j = 0;
    
    M8P_rtme_hme1_top3_rtme_hme1_top3_64_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_64_reg;
    M8P_rtme_hme1_top3_rtme_hme1_top3_68_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_68_reg;
    M8P_rtme_hme1_top3_rtme_hme1_top3_6c_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_6c_reg;
    M8P_rtme_hme1_top3_rtme_hme1_top3_70_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_70_reg;
    M8P_rtme_hme1_top3_rtme_hme1_top3_74_RBUS M8P_rtme_hme1_top3_rtme_hme1_top3_74_reg;

    
    unsigned int print_in_func;
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    M8P_rtme_hme1_top3_rtme_hme1_top3_64_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_64_reg);
    M8P_rtme_hme1_top3_rtme_hme1_top3_68_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_68_reg);
    M8P_rtme_hme1_top3_rtme_hme1_top3_6c_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_6C_reg);
    M8P_rtme_hme1_top3_rtme_hme1_top3_70_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_70_reg);
    M8P_rtme_hme1_top3_rtme_hme1_top3_74_reg.regValue = rtd_inl(M8P_RTME_HME1_TOP3_RTME_HME1_TOP3_74_reg);

    u10_region_w[0] = M8P_rtme_hme1_top3_rtme_hme1_top3_64_reg.me1_layer3_rgn_h1 - M8P_rtme_hme1_top3_rtme_hme1_top3_64_reg.me1_layer3_rgn_h0;
    u10_region_w[1] = M8P_rtme_hme1_top3_rtme_hme1_top3_64_reg.me1_layer3_rgn_h2 - M8P_rtme_hme1_top3_rtme_hme1_top3_64_reg.me1_layer3_rgn_h1;
    u10_region_w[2] = M8P_rtme_hme1_top3_rtme_hme1_top3_68_reg.me1_layer3_rgn_h3 - M8P_rtme_hme1_top3_rtme_hme1_top3_64_reg.me1_layer3_rgn_h2;
    u10_region_w[3] = M8P_rtme_hme1_top3_rtme_hme1_top3_68_reg.me1_layer3_rgn_h4 - M8P_rtme_hme1_top3_rtme_hme1_top3_68_reg.me1_layer3_rgn_h3;
    u10_region_w[4] = M8P_rtme_hme1_top3_rtme_hme1_top3_68_reg.me1_layer3_rgn_h5 - M8P_rtme_hme1_top3_rtme_hme1_top3_68_reg.me1_layer3_rgn_h4;
    u10_region_w[5] = M8P_rtme_hme1_top3_rtme_hme1_top3_6c_reg.me1_layer3_rgn_h6 - M8P_rtme_hme1_top3_rtme_hme1_top3_68_reg.me1_layer3_rgn_h5;
    u10_region_w[6] = M8P_rtme_hme1_top3_rtme_hme1_top3_6c_reg.me1_layer3_rgn_h7 - M8P_rtme_hme1_top3_rtme_hme1_top3_6c_reg.me1_layer3_rgn_h6;
    u10_region_w[7] = M8P_rtme_hme1_top3_rtme_hme1_top3_6c_reg.me1_layer3_rgn_h8 - M8P_rtme_hme1_top3_rtme_hme1_top3_6c_reg.me1_layer3_rgn_h7;

    u10_region_h[0] = M8P_rtme_hme1_top3_rtme_hme1_top3_70_reg.me1_layer3_rgn_v1 - M8P_rtme_hme1_top3_rtme_hme1_top3_70_reg.me1_layer3_rgn_v0;
    u10_region_h[1] = M8P_rtme_hme1_top3_rtme_hme1_top3_70_reg.me1_layer3_rgn_v2 - M8P_rtme_hme1_top3_rtme_hme1_top3_70_reg.me1_layer3_rgn_v1;
    u10_region_h[2] = M8P_rtme_hme1_top3_rtme_hme1_top3_74_reg.me1_layer3_rgn_v3 - M8P_rtme_hme1_top3_rtme_hme1_top3_70_reg.me1_layer3_rgn_v2;
    u10_region_h[3] = M8P_rtme_hme1_top3_rtme_hme1_top3_74_reg.me1_layer3_rgn_v4 - M8P_rtme_hme1_top3_rtme_hme1_top3_74_reg.me1_layer3_rgn_v3;

    for (u8_i = 0; u8_i < 32; u8_i ++)
    {
        u8_rgn_idx_x = (unsigned char)(u8_i % 8);
        u8_rgn_idx_y = (unsigned char)(u8_i / 8);

        u32_rgn_blk_cnt = u10_region_w[u8_rgn_idx_x] * u10_region_h[u8_rgn_idx_y];
        u32_total_blk_cnt += u32_rgn_blk_cnt;

        if (s_pContext->_output_read_comreg.u10_me_freq_cnt_rb[u8_i] > (u32_rgn_blk_cnt / 3))
            u5_frq_region_cnt++;
            
        if (s_pContext->_output_read_comreg.u10_me_freq_cnt_rb[u8_i] > (u32_rgn_blk_cnt / 60))
            u5_frq_region_small_cnt++;
        
        if (s_pContext->_output_read_comreg.u10_me_freq_cnt_rb[u8_i] > 50)
            u5_frq_region_cnt2++;
        
        //s11_me_freq_avgbv_vx_rb
        //s10_me_freq_avgbv_vy_rb

        u32_total_frq_cnt += s_pContext->_output_read_comreg.u10_me_freq_cnt_rb[u8_i];
    }

    if (u5_frq_region_cnt > 1)
        u1_repeat_coef_cond = true;

    if (u32_total_frq_cnt > (u32_total_blk_cnt / 5))
        u1_sum_cond = true;
    
    if (u5_frq_region_cnt2 < 28)
        u1_repeat_not_many = true;
    
    if ((_ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb) <= 2) && 
        (_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb) <= 2) && 
        (s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb > 30000))
        u1_gmv_z_stable = true;
    
    pOutput->u8_gmv_z_stable_pre = ((7 * pOutput->u8_gmv_z_stable_pre) + 1 * (u1_gmv_z_stable?255:0) + 4) / 8;

    if (pOutput->u8_gmv_z_stable_pre > 128 && u5_frq_region_small_cnt > 0)
        u1_repeat_coef_cond = true;
    
    if ((u1_repeat_coef_cond || u1_sum_cond) && u1_repeat_not_many)
    {
        pOutput->u1_detect_mode = 1;
        pOutput->u4_lpf_5x5_coef = (pOutput->u4_lpf_5x5_coef < 8)?(pOutput->u4_lpf_5x5_coef + 1):8;
    }
    else
    {
        pOutput->u1_detect_mode = 0;
        pOutput->u4_lpf_5x5_coef = (pOutput->u4_lpf_5x5_coef > 0)?(pOutput->u4_lpf_5x5_coef - 1):0;
    }

    // if avgbv not stable
    ///////////////////////////////////////////////////////////////////////////
    for (u8_i = 0; u8_i < 32; u8_i ++)
    {
        for (u8_j = 4; u8_j > 0; u8_j --)
        {
            pOutput->s11_avgbv_mvx[u8_j][u8_i] = pOutput->s11_avgbv_mvx[u8_j - 1][u8_i];
            pOutput->s10_avgbv_mvy[u8_j][u8_i] = pOutput->s10_avgbv_mvy[u8_j - 1][u8_i];
        }
        pOutput->s11_avgbv_mvx[0][u8_i] = s_pContext->_output_read_comreg.s11_me_freq_avgbv_vx_rb[u8_i];
        pOutput->s10_avgbv_mvy[0][u8_i] = s_pContext->_output_read_comreg.s10_me_freq_avgbv_vy_rb[u8_i];
    }

    for (u8_i = 0; u8_i < 32; u8_i ++)
    {
        for (u8_j = 0; u8_j < 4; u8_j ++)
        {
            u32_avgbv_mvdx[u8_i] += _CLIP_(_ABS_(pOutput->s11_avgbv_mvx[u8_j][u8_i] - pOutput->s11_avgbv_mvx[u8_j + 1][u8_i]) - 5, 0, 128);
            u32_avgbv_mvdy[u8_i] += _CLIP_(_ABS_(pOutput->s10_avgbv_mvy[u8_j][u8_i] - pOutput->s10_avgbv_mvy[u8_j + 1][u8_i]) - 5, 0, 128);
        }
    }
    
    for (u8_i = 0; u8_i < 32; u8_i ++)
    {
        pOutput->u32_avgbv_unconf[u8_i] = u32_avgbv_mvdx[u8_i] + u32_avgbv_mvdy[u8_i];
        u32_avgbv_unconf_sum += pOutput->u32_avgbv_unconf[u8_i];
    }

    // if SC
    ///////////////////////////////////////////////////////////////////////////
    if ((_ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb) < 2) && 
        (_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb) < 2) && 
        (s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb > 25000))
        u4_sc_th = 0;
    else
        u4_sc_th = 7;

    if (((s_pContext->_output_read_comreg.u32_me_sc_pattern_rb & u4_sc_th) == 0) && (u32_avgbv_unconf_sum < u5_frq_region_cnt2 * 210))
    {
        pOutput->u1_layer0_frq_en = pOutput->u1_layer0_frq_en_ori;
        pOutput->u1_layer1_frq_en = pOutput->u1_layer1_frq_en_ori;
        pOutput->u1_layer2_frq_en = pOutput->u1_layer2_frq_en_ori;
        pOutput->u1_layer3_frq_en = pOutput->u1_layer3_frq_en_ori;
    }
    else
    {
        pOutput->u1_layer0_frq_en = 0;
        pOutput->u1_layer1_frq_en = 0;
        pOutput->u1_layer2_frq_en = 0;
        pOutput->u1_layer3_frq_en = 0;
    }   
}
