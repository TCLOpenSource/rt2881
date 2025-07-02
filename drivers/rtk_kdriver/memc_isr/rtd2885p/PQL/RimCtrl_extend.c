#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_reg_def.h"

///////////////////////////////////////////////////////////////////////////////////////////
extern unsigned char k23755_flash_flg;
extern unsigned int g_HisDiff;
extern unsigned int g_HueDiff;
extern unsigned char u8_SlowIn_MaxShiftX;
extern unsigned char u8_SlowIn_MaxShiftY;
bool u1_Panning_flg=0;
unsigned char u8_MC_Shrink_V = 0;
unsigned char u8_MC_Shrink_H = 0;

VOID RimCtrl_OutResolution_Parse_RTK2885pp(_OUTPUT_RimCtrl *pOutput)
{
    const _PQLCONTEXT *s_pContext   = GetPQLContext();
    PQL_OUTPUT_RESOLUTION outResolution_cur = s_pContext->_external_data._output_resolution;

    if (outResolution_cur != (PQL_OUTPUT_RESOLUTION)pOutput->u4_outResolution_pre)
    {
        pOutput->u4_outResolution_pre = outResolution_cur;

        switch (outResolution_cur)
        {
        case _PQL_OUT_1920x1080 :
            pOutput->u12_out_resolution[_RIM_LFT] = 0;
            pOutput->u12_out_resolution[_RIM_RHT] = 1919;
            pOutput->u12_out_resolution[_RIM_TOP] = 0;
            pOutput->u12_out_resolution[_RIM_BOT] = 1079;
            u8_SlowIn_MaxShiftX = 24;
            u8_SlowIn_MaxShiftY = 24;
            break;
        case _PQL_OUT_1920x540:
            pOutput->u12_out_resolution[_RIM_LFT] = 0;
            pOutput->u12_out_resolution[_RIM_RHT] = 1919;
            pOutput->u12_out_resolution[_RIM_TOP] = 0;
            pOutput->u12_out_resolution[_RIM_BOT] = 539;
            u8_SlowIn_MaxShiftX = 24;
            u8_SlowIn_MaxShiftY = 12;
            break;
        case _PQL_OUT_3840x2160:
            pOutput->u12_out_resolution[_RIM_LFT] = 0;
            pOutput->u12_out_resolution[_RIM_RHT] = 3839;
            pOutput->u12_out_resolution[_RIM_TOP] = 0;
            pOutput->u12_out_resolution[_RIM_BOT] = 2159;
            u8_SlowIn_MaxShiftX = 48;
            u8_SlowIn_MaxShiftY = 48;
            break;
        case _PQL_OUT_3840x1080:
            pOutput->u12_out_resolution[_RIM_LFT] = 0;
            pOutput->u12_out_resolution[_RIM_RHT] = 3839;
            pOutput->u12_out_resolution[_RIM_TOP] = 0;
            pOutput->u12_out_resolution[_RIM_BOT] = 1079;
            u8_SlowIn_MaxShiftX = 48;
            u8_SlowIn_MaxShiftY = 24;
            break;
        case _PQL_OUT_3840x540:
            pOutput->u12_out_resolution[_RIM_LFT] = 0;
            pOutput->u12_out_resolution[_RIM_RHT] = 3839;
            pOutput->u12_out_resolution[_RIM_TOP] = 0;
            pOutput->u12_out_resolution[_RIM_BOT] = 539;
            u8_SlowIn_MaxShiftX = 48;
            u8_SlowIn_MaxShiftY = 12;
            break;
        default: //_PQL_OUT_3840x2160
            pOutput->u12_out_resolution[_RIM_LFT] = 0;
            pOutput->u12_out_resolution[_RIM_RHT] = 3839;
            pOutput->u12_out_resolution[_RIM_TOP] = 0;
            pOutput->u12_out_resolution[_RIM_BOT] = 2159;
            u8_SlowIn_MaxShiftX = 48;
            u8_SlowIn_MaxShiftY = 48;
            break;
        }
    }
}

unsigned short RimCtrl_Get_HW_rim_RTK2885pp(unsigned char rimID, _OUTPUT_RimCtrl *pOutput)
{
    // const _PQLCONTEXT *s_pContext = GetPQLContext();

    // return u12_HW_rim;
    return 0;
}

unsigned short RimCtrl_Get_Rim_Bound_RTK2885pp(unsigned char u2_Rim_id, _OUTPUT_RimCtrl *pOutput)
{
    unsigned short u12_rim_bound = 0;
    switch(u2_Rim_id)
    {
        case _RIM_LFT :
            u12_rim_bound = (pOutput->u4_outResolution_pre == _PQL_OUT_1920x1080 || pOutput->u4_outResolution_pre == _PQL_OUT_1920x540) ? 320:  640; // else, Hactive = 3840.
            break;
        case _RIM_RHT :
            u12_rim_bound = (pOutput->u4_outResolution_pre == _PQL_OUT_1920x1080 || pOutput->u4_outResolution_pre == _PQL_OUT_1920x540) ? 1600:  3200;
            break;
        case _RIM_TOP :
            u12_rim_bound = (pOutput->u4_outResolution_pre == _PQL_OUT_1920x1080 || pOutput->u4_outResolution_pre == _PQL_OUT_3840x1080)?  180 : 
                            (pOutput->u4_outResolution_pre == _PQL_OUT_3840x2160)?  360 : 90; // else, Vactive = 540.
            break;
        case _RIM_BOT :
            u12_rim_bound = (pOutput->u4_outResolution_pre == _PQL_OUT_1920x1080 || pOutput->u4_outResolution_pre == _PQL_OUT_3840x1080) ? 900 : 
                            (pOutput->u4_outResolution_pre == _PQL_OUT_3840x2160)? 1800 : 450;
            break;
    }
    return u12_rim_bound;
}

VOID RimCtrl_HW_extract_RTK2885pp(const _PARAM_RimCtrl *pParam, _OUTPUT_RimCtrl *pOutput)
{
    // const _PQLCONTEXT *s_pContext = GetPQLContext();
}

VOID RimCtrl_ME1_Rim_Convert_RTK2885pp(const _PARAM_RimCtrl *pParam, _OUTPUT_RimCtrl *pOutput)
{
    int pix_scaledown_list[5] = {32, 16, 8, 4, 2};
    int blk_scaledown_list[5] = {128, 64, 32, 16, 8};
    int pix_shift[5] = {1, 1, 1, 1, 1};
    int blk_shift[5] = {1, 1, 1, 1, 1};
    int blk_t_shift[5] = {0, 0, 0, 0, 0};
    //int blk_b_shift[5] = {1,  1, 1, 0, 0};

    //--------------- block_rim ---------------
    pOutput->u12_hme_layer0_blk_rim[_RIM_TOP] = (pOutput->u12_Rim_Result[_RIM_TOP] + blk_scaledown_list[0]/2) / blk_scaledown_list[0] + (blk_shift[0]*pOutput->y_shift_coef) + blk_t_shift[0];
    pOutput->u12_hme_layer0_blk_rim[_RIM_BOT] = (pOutput->u12_Rim_Result[_RIM_BOT] + blk_scaledown_list[0]/2) / blk_scaledown_list[0] + (blk_shift[0]*pOutput->y_shift_coef) + blk_t_shift[0] - 1;
    pOutput->u12_hme_layer0_blk_rim[_RIM_RHT] = (pOutput->u12_Rim_Result[_RIM_RHT] + blk_scaledown_list[0]/2) / blk_scaledown_list[0] + (blk_shift[0]*pOutput->x_shift_coef) - 1;
    pOutput->u12_hme_layer0_blk_rim[_RIM_LFT] = (pOutput->u12_Rim_Result[_RIM_LFT] + blk_scaledown_list[0]/2) / blk_scaledown_list[0] + (blk_shift[0]*pOutput->x_shift_coef);

    pOutput->u12_hme_layer1_blk_rim[_RIM_TOP] = (pOutput->u12_Rim_Result[_RIM_TOP] + blk_scaledown_list[1]/2) / blk_scaledown_list[1] + (blk_shift[1]*pOutput->y_shift_coef) + blk_t_shift[1];
    pOutput->u12_hme_layer1_blk_rim[_RIM_BOT] = (pOutput->u12_Rim_Result[_RIM_BOT] + blk_scaledown_list[1]/2) / blk_scaledown_list[1] + (blk_shift[1]*pOutput->y_shift_coef) + blk_t_shift[1] - 1;
    pOutput->u12_hme_layer1_blk_rim[_RIM_RHT] = (pOutput->u12_Rim_Result[_RIM_RHT] + blk_scaledown_list[1]/2) / blk_scaledown_list[1] + (blk_shift[1]*pOutput->x_shift_coef) - 1;
    pOutput->u12_hme_layer1_blk_rim[_RIM_LFT] = (pOutput->u12_Rim_Result[_RIM_LFT] + blk_scaledown_list[1]/2) / blk_scaledown_list[1] + (blk_shift[1]*pOutput->x_shift_coef);

    pOutput->u12_hme_layer2_blk_rim[_RIM_TOP] = (pOutput->u12_Rim_Result[_RIM_TOP] + blk_scaledown_list[2]/2) / blk_scaledown_list[2] + (blk_shift[2]*pOutput->y_shift_coef) + blk_t_shift[2];
    pOutput->u12_hme_layer2_blk_rim[_RIM_BOT] = (pOutput->u12_Rim_Result[_RIM_BOT] + blk_scaledown_list[2]/2) / blk_scaledown_list[2] + (blk_shift[2]*pOutput->y_shift_coef) + blk_t_shift[2] - 1;
    pOutput->u12_hme_layer2_blk_rim[_RIM_RHT] = (pOutput->u12_Rim_Result[_RIM_RHT] + blk_scaledown_list[2]/2) / blk_scaledown_list[2] + (blk_shift[2]*pOutput->x_shift_coef) - 1;
    pOutput->u12_hme_layer2_blk_rim[_RIM_LFT] = (pOutput->u12_Rim_Result[_RIM_LFT] + blk_scaledown_list[2]/2) / blk_scaledown_list[2] + (blk_shift[2]*pOutput->x_shift_coef);

    pOutput->u12_hme_layer3_blk_rim[_RIM_TOP] = (pOutput->u12_Rim_Result[_RIM_TOP] + blk_scaledown_list[3]/2) / blk_scaledown_list[3] + (blk_shift[3]*pOutput->y_shift_coef) + blk_t_shift[3];
    pOutput->u12_hme_layer3_blk_rim[_RIM_BOT] = (pOutput->u12_Rim_Result[_RIM_BOT] + blk_scaledown_list[3]/2) / blk_scaledown_list[3] + (blk_shift[3]*pOutput->y_shift_coef) + blk_t_shift[3] - 1;
    pOutput->u12_hme_layer3_blk_rim[_RIM_RHT] = (pOutput->u12_Rim_Result[_RIM_RHT] + blk_scaledown_list[3]/2) / blk_scaledown_list[3] + (blk_shift[3]*pOutput->x_shift_coef) - 1;
    pOutput->u12_hme_layer3_blk_rim[_RIM_LFT] = (pOutput->u12_Rim_Result[_RIM_LFT] + blk_scaledown_list[3]/2) / blk_scaledown_list[3] + (blk_shift[3]*pOutput->x_shift_coef);

    //--------------- block_rim ---------------
    pOutput->u12_hme_layer0_pix_rim[_RIM_TOP] = (pOutput->u12_Rim_Result[_RIM_TOP] + pix_scaledown_list[0]/2) / pix_scaledown_list[0] + (pix_shift[0]*pOutput->y_shift_coef);
    pOutput->u12_hme_layer0_pix_rim[_RIM_BOT] = (pOutput->u12_Rim_Result[_RIM_BOT] + pix_scaledown_list[0]/2) / pix_scaledown_list[0] + (pix_shift[0]*pOutput->y_shift_coef) - 1;
    pOutput->u12_hme_layer0_pix_rim[_RIM_RHT] = (pOutput->u12_Rim_Result[_RIM_RHT] + pix_scaledown_list[0]/2) / pix_scaledown_list[0] + (pix_shift[0]*pOutput->x_shift_coef) - 1;
    pOutput->u12_hme_layer0_pix_rim[_RIM_LFT] = (pOutput->u12_Rim_Result[_RIM_LFT] + pix_scaledown_list[0]/2) / pix_scaledown_list[0] + (pix_shift[0]*pOutput->x_shift_coef);

    pOutput->u12_hme_layer1_pix_rim[_RIM_TOP] = (pOutput->u12_Rim_Result[_RIM_TOP] + pix_scaledown_list[1]/2) / pix_scaledown_list[1] + (pix_shift[1]*pOutput->y_shift_coef);
    pOutput->u12_hme_layer1_pix_rim[_RIM_BOT] = (pOutput->u12_Rim_Result[_RIM_BOT] + pix_scaledown_list[1]/2) / pix_scaledown_list[1] + (pix_shift[1]*pOutput->y_shift_coef) - 1;
    pOutput->u12_hme_layer1_pix_rim[_RIM_RHT] = (pOutput->u12_Rim_Result[_RIM_RHT] + pix_scaledown_list[1]/2) / pix_scaledown_list[1] + (pix_shift[1]*pOutput->x_shift_coef) - 1;
    pOutput->u12_hme_layer1_pix_rim[_RIM_LFT] = (pOutput->u12_Rim_Result[_RIM_LFT] + pix_scaledown_list[1]/2) / pix_scaledown_list[1] + (pix_shift[1]*pOutput->x_shift_coef);

    pOutput->u12_hme_layer2_pix_rim[_RIM_TOP] = (pOutput->u12_Rim_Result[_RIM_TOP] + pix_scaledown_list[2]/2) / pix_scaledown_list[2] + (pix_shift[2]*pOutput->y_shift_coef);
    pOutput->u12_hme_layer2_pix_rim[_RIM_BOT] = (pOutput->u12_Rim_Result[_RIM_BOT] + pix_scaledown_list[2]/2) / pix_scaledown_list[2] + (pix_shift[2]*pOutput->y_shift_coef) - 1;
    pOutput->u12_hme_layer2_pix_rim[_RIM_RHT] = (pOutput->u12_Rim_Result[_RIM_RHT] + pix_scaledown_list[2]/2) / pix_scaledown_list[2] + (pix_shift[2]*pOutput->x_shift_coef) - 1;
    pOutput->u12_hme_layer2_pix_rim[_RIM_LFT] = (pOutput->u12_Rim_Result[_RIM_LFT] + pix_scaledown_list[2]/2) / pix_scaledown_list[2] + (pix_shift[2]*pOutput->x_shift_coef);

    pOutput->u12_hme_layer3_pix_rim[_RIM_TOP] = (pOutput->u12_Rim_Result[_RIM_TOP] + pix_scaledown_list[3]/2) / pix_scaledown_list[3] + (pix_shift[3]*pOutput->y_shift_coef);
    pOutput->u12_hme_layer3_pix_rim[_RIM_BOT] = (pOutput->u12_Rim_Result[_RIM_BOT] + pix_scaledown_list[3]/2) / pix_scaledown_list[3] + (pix_shift[3]*pOutput->y_shift_coef) - 1;
    pOutput->u12_hme_layer3_pix_rim[_RIM_RHT] = (pOutput->u12_Rim_Result[_RIM_RHT] + pix_scaledown_list[3]/2) / pix_scaledown_list[3] + (pix_shift[3]*pOutput->x_shift_coef) - 1;
    pOutput->u12_hme_layer3_pix_rim[_RIM_LFT] = (pOutput->u12_Rim_Result[_RIM_LFT] + pix_scaledown_list[3]/2) / pix_scaledown_list[3] + (pix_shift[3]*pOutput->x_shift_coef);
    
    if(pParam->u1_log_en && pOutput->u12_fr_cnt==60)
    {
        rtd_pr_memc_emerg("//********** [%s][%d][layer0_blk] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_hme_layer0_blk_rim[_RIM_TOP], pOutput->u12_hme_layer0_blk_rim[_RIM_BOT], pOutput->u12_hme_layer0_blk_rim[_RIM_LFT], pOutput->u12_hme_layer0_blk_rim[_RIM_RHT]);  
        rtd_pr_memc_emerg("//********** [%s][%d][layer1_blk] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_hme_layer1_blk_rim[_RIM_TOP], pOutput->u12_hme_layer1_blk_rim[_RIM_BOT], pOutput->u12_hme_layer1_blk_rim[_RIM_LFT], pOutput->u12_hme_layer1_blk_rim[_RIM_RHT]);
        rtd_pr_memc_emerg("//********** [%s][%d][layer2_blk] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_hme_layer2_blk_rim[_RIM_TOP], pOutput->u12_hme_layer2_blk_rim[_RIM_BOT], pOutput->u12_hme_layer2_blk_rim[_RIM_LFT], pOutput->u12_hme_layer2_blk_rim[_RIM_RHT]);
        rtd_pr_memc_emerg("//********** [%s][%d][layer3_blk] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_hme_layer3_blk_rim[_RIM_TOP], pOutput->u12_hme_layer3_blk_rim[_RIM_BOT], pOutput->u12_hme_layer3_blk_rim[_RIM_LFT], pOutput->u12_hme_layer3_blk_rim[_RIM_RHT]);
        rtd_pr_memc_emerg("//********** [%s][%d][layer0_pix] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_hme_layer0_pix_rim[_RIM_TOP], pOutput->u12_hme_layer0_pix_rim[_RIM_BOT], pOutput->u12_hme_layer0_pix_rim[_RIM_LFT], pOutput->u12_hme_layer0_pix_rim[_RIM_RHT]);  
        rtd_pr_memc_emerg("//********** [%s][%d][layer1_pix] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_hme_layer1_pix_rim[_RIM_TOP], pOutput->u12_hme_layer1_pix_rim[_RIM_BOT], pOutput->u12_hme_layer1_pix_rim[_RIM_LFT], pOutput->u12_hme_layer1_pix_rim[_RIM_RHT]);
        rtd_pr_memc_emerg("//********** [%s][%d][layer2_pix] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_hme_layer2_pix_rim[_RIM_TOP], pOutput->u12_hme_layer2_pix_rim[_RIM_BOT], pOutput->u12_hme_layer2_pix_rim[_RIM_LFT], pOutput->u12_hme_layer2_pix_rim[_RIM_RHT]);
        rtd_pr_memc_emerg("//********** [%s][%d][layer3_pix] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_hme_layer3_pix_rim[_RIM_TOP], pOutput->u12_hme_layer3_pix_rim[_RIM_BOT], pOutput->u12_hme_layer3_pix_rim[_RIM_LFT], pOutput->u12_hme_layer3_pix_rim[_RIM_RHT]);
    }

    
}

VOID RimCtrl_ME2_Rim_Convert_RTK2885pp(const _PARAM_RimCtrl *pParam, _OUTPUT_RimCtrl *pOutput)
{
    pOutput->u12_me2_blk_rim[_RIM_TOP] = (pOutput->u12_Rim_Result[_RIM_TOP] + 4) / 8 + pOutput->y_shift_coef;
    pOutput->u12_me2_blk_rim[_RIM_BOT] = (pOutput->u12_Rim_Result[_RIM_BOT] + 4) / 8 - pOutput->y_shift_coef - 1;
    pOutput->u12_me2_blk_rim[_RIM_RHT] = (pOutput->u12_Rim_Result[_RIM_RHT] + 4) / 8 - pOutput->x_shift_coef - 1;
    pOutput->u12_me2_blk_rim[_RIM_LFT] = (pOutput->u12_Rim_Result[_RIM_LFT] + 4) / 8 + pOutput->x_shift_coef;

    if(pParam->u1_log_en && pOutput->u12_fr_cnt==60)
    {
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_me2_blk_rim[_RIM_TOP], pOutput->u12_me2_blk_rim[_RIM_BOT], pOutput->u12_me2_blk_rim[_RIM_LFT], pOutput->u12_me2_blk_rim[_RIM_RHT]);  
    }
}

VOID RimCtrl_Dh_Rim_Convert_RTK2885pp(const _PARAM_RimCtrl *pParam, _OUTPUT_RimCtrl *pOutput)
{
    
    pOutput->u12_dh_blk_rim[_RIM_TOP] = pOutput->u12_Rim_Result[_RIM_TOP] / 8 + 1*pOutput->y_shift_coef;
    pOutput->u12_dh_blk_rim[_RIM_BOT] = pOutput->u12_Rim_Result[_RIM_BOT] / 8 - 1*pOutput->y_shift_coef;
    pOutput->u12_dh_blk_rim[_RIM_RHT] = pOutput->u12_Rim_Result[_RIM_RHT] / 8 - 1*pOutput->x_shift_coef;
    pOutput->u12_dh_blk_rim[_RIM_LFT] = pOutput->u12_Rim_Result[_RIM_LFT] / 8 + 1*pOutput->x_shift_coef;

    if(pParam->u1_log_en && pOutput->u12_fr_cnt==60)
    {
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_dh_blk_rim[_RIM_TOP], pOutput->u12_dh_blk_rim[_RIM_BOT], pOutput->u12_dh_blk_rim[_RIM_LFT], pOutput->u12_dh_blk_rim[_RIM_RHT]);  
    }
}

VOID RimCtrl_MC_Rim_Convert_RTK2885pp(const _PARAM_RimCtrl *pParam, _OUTPUT_RimCtrl *pOutput)
{
    const _PQLCONTEXT *s_pContext = GetPQLContext();
    
    unsigned short u12_rim0_rim1_range[4];
    unsigned short u8_base_offset;
    signed short s11_gmv_x, s10_gmv_y;

    /*
    pOutput->u12_mc_rim0[_RIM_LFT] = pOutput->u12_Rim_Result[_RIM_LFT] + pParam->u8_mc_ptRim_shrink + u8_MC_Shrink_H;
    pOutput->u12_mc_rim0[_RIM_RHT] = pOutput->u12_Rim_Result[_RIM_RHT] - pParam->u8_mc_ptRim_shrink - u8_MC_Shrink_H;
    pOutput->u12_mc_rim0[_RIM_TOP] = pOutput->u12_Rim_Result[_RIM_TOP] + pParam->u8_mc_ptRim_shrink + u8_MC_Shrink_V;
    pOutput->u12_mc_rim0[_RIM_BOT] = pOutput->u12_Rim_Result[_RIM_BOT] - pParam->u8_mc_ptRim_shrink - u8_MC_Shrink_V;

    
    pOutput->u12_mc_rim1[_RIM_LFT] = pOutput->u12_Rim_Result[_RIM_LFT];
    pOutput->u12_mc_rim1[_RIM_RHT] = pOutput->u12_Rim_Result[_RIM_RHT];
    pOutput->u12_mc_rim1[_RIM_TOP] = pOutput->u12_Rim_Result[_RIM_TOP];
    pOutput->u12_mc_rim1[_RIM_BOT] = pOutput->u12_Rim_Result[_RIM_BOT];
    */
    pOutput->u12_mc_i_rim0[_RIM_TOP] = pOutput->u12_Rim_0[_RIM_TOP]; //pOutput->u12_Rim_Result[_RIM_TOP];
    pOutput->u12_mc_i_rim0[_RIM_BOT] = pOutput->u12_Rim_0[_RIM_BOT]; //pOutput->u12_Rim_Result[_RIM_BOT];
    pOutput->u12_mc_i_rim0[_RIM_RHT] = pOutput->u12_Rim_0[_RIM_RHT]; //pOutput->u12_Rim_Result[_RIM_RHT];
    pOutput->u12_mc_i_rim0[_RIM_LFT] = pOutput->u12_Rim_0[_RIM_LFT]; //pOutput->u12_Rim_Result[_RIM_LFT];
    pOutput->u12_mc_i_rim1[_RIM_TOP] = pOutput->u12_Rim_1[_RIM_TOP]; //pOutput->u12_Rim_Result[_RIM_TOP];
    pOutput->u12_mc_i_rim1[_RIM_BOT] = pOutput->u12_Rim_1[_RIM_BOT]; //pOutput->u12_Rim_Result[_RIM_BOT];
    pOutput->u12_mc_i_rim1[_RIM_RHT] = pOutput->u12_Rim_1[_RIM_RHT]; //pOutput->u12_Rim_Result[_RIM_RHT];
    pOutput->u12_mc_i_rim1[_RIM_LFT] = pOutput->u12_Rim_1[_RIM_LFT]; //pOutput->u12_Rim_Result[_RIM_LFT];
    pOutput->u12_mc_p_rim0[_RIM_TOP] = pOutput->u12_Rim_0[_RIM_TOP]; //pOutput->u12_Rim_Result[_RIM_TOP];
    pOutput->u12_mc_p_rim0[_RIM_BOT] = pOutput->u12_Rim_0[_RIM_BOT]; //pOutput->u12_Rim_Result[_RIM_BOT];
    pOutput->u12_mc_p_rim0[_RIM_RHT] = pOutput->u12_Rim_0[_RIM_RHT]; //pOutput->u12_Rim_Result[_RIM_RHT];
    pOutput->u12_mc_p_rim0[_RIM_LFT] = pOutput->u12_Rim_0[_RIM_LFT]; //pOutput->u12_Rim_Result[_RIM_LFT];
    pOutput->u12_mc_p_rim1[_RIM_TOP] = pOutput->u12_Rim_1[_RIM_TOP]; //pOutput->u12_Rim_Result[_RIM_TOP];
    pOutput->u12_mc_p_rim1[_RIM_BOT] = pOutput->u12_Rim_1[_RIM_BOT]; //pOutput->u12_Rim_Result[_RIM_BOT];
    pOutput->u12_mc_p_rim1[_RIM_RHT] = pOutput->u12_Rim_1[_RIM_RHT]; //pOutput->u12_Rim_Result[_RIM_RHT];
    pOutput->u12_mc_p_rim1[_RIM_LFT] = pOutput->u12_Rim_1[_RIM_LFT]; //pOutput->u12_Rim_Result[_RIM_LFT];

    u12_rim0_rim1_range[_RIM_TOP] = _CLIP_(_ABS_(pOutput->u12_Rim_0[_RIM_TOP] - pOutput->u12_Rim_1[_RIM_TOP]), 0, 9);
    u12_rim0_rim1_range[_RIM_BOT] = _CLIP_(_ABS_(pOutput->u12_Rim_0[_RIM_BOT] - pOutput->u12_Rim_1[_RIM_BOT]), 0, 9);
    u12_rim0_rim1_range[_RIM_RHT] = _CLIP_(_ABS_(pOutput->u12_Rim_0[_RIM_RHT] - pOutput->u12_Rim_1[_RIM_RHT]), 0, 9);
    u12_rim0_rim1_range[_RIM_LFT] = _CLIP_(_ABS_(pOutput->u12_Rim_0[_RIM_LFT] - pOutput->u12_Rim_1[_RIM_LFT]), 0, 9);

    u8_base_offset = 0;
    if (s_pContext->_output_mc_lbmcswitch.u8_lf_lbmcMode != 8)
    {
        pOutput->u1_mc_invalid_rim_oosr_clear_en = 1;
        u8_base_offset = 2;
    }
    else
    {
        pOutput->u1_mc_invalid_rim_oosr_clear_en = 0;
        u8_base_offset = 0;
    }
    pOutput->u1_mc_fetch_data_rim_clip = 1;
    pOutput->u1_mc_fetch_data_rim_type = 0;

    s11_gmv_x = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
    s10_gmv_y = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
    pOutput->u12_mc_fetch_data_rim_offset[_RIM_TOP] = _CLIP_(_MAX_(_ABS_(s10_gmv_y) -  8, 0) /  8, 0, u12_rim0_rim1_range[_RIM_TOP] / 4 + u8_base_offset);
    pOutput->u12_mc_fetch_data_rim_offset[_RIM_BOT] = _CLIP_(_MAX_(_ABS_(s10_gmv_y) -  8, 0) /  8, 0, u12_rim0_rim1_range[_RIM_BOT] / 4 + u8_base_offset);
    pOutput->u12_mc_fetch_data_rim_offset[_RIM_RHT] = _CLIP_(_MAX_(_ABS_(s11_gmv_x) - 32, 0) / 16, 0, u12_rim0_rim1_range[_RIM_RHT] / 4 + u8_base_offset);
    pOutput->u12_mc_fetch_data_rim_offset[_RIM_LFT] = _CLIP_(_MAX_(_ABS_(s11_gmv_x) - 32, 0) / 16, 0, u12_rim0_rim1_range[_RIM_LFT] / 4 + u8_base_offset);

    if(pParam->u1_log_en && pOutput->u12_fr_cnt==60)
    {
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_mc_i_rim0[_RIM_TOP], pOutput->u12_mc_i_rim0[_RIM_BOT], pOutput->u12_mc_i_rim0[_RIM_LFT], pOutput->u12_mc_i_rim0[_RIM_RHT]);                  
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_mc_i_rim1[_RIM_TOP], pOutput->u12_mc_i_rim1[_RIM_BOT], pOutput->u12_mc_i_rim1[_RIM_LFT], pOutput->u12_mc_i_rim1[_RIM_RHT]);
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_mc_p_rim0[_RIM_TOP], pOutput->u12_mc_p_rim0[_RIM_BOT], pOutput->u12_mc_p_rim0[_RIM_LFT], pOutput->u12_mc_p_rim0[_RIM_RHT]);                  
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_mc_p_rim1[_RIM_TOP], pOutput->u12_mc_p_rim1[_RIM_BOT], pOutput->u12_mc_p_rim1[_RIM_LFT], pOutput->u12_mc_p_rim1[_RIM_RHT]);
    }


}

VOID RimCtrl_Logo_Rim_Convert_RTK2885pp(const _PARAM_RimCtrl *pParam, _OUTPUT_RimCtrl *pOutput)
{
    int i=0;
    int tolerence=0;
        
    for (i = 0; i < _RIM_NUM; i ++)
    {
        if(i==1 || i==2)
            tolerence = -tolerence;
        pOutput->u12_logo_layer1_rim[i] = pOutput->u12_Rim_Result[i] / 2 + tolerence;
        pOutput->u12_logo_layer2_rim[i] = pOutput->u12_Rim_Result[i] / 4 + tolerence;
        pOutput->u12_logo_layer3_rim[i] = pOutput->u12_Rim_Result[i] / 8 + tolerence;
    }

    if(pParam->u1_log_en && pOutput->u12_fr_cnt==60)
    {
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_logo_layer1_rim[_RIM_TOP], pOutput->u12_logo_layer1_rim[_RIM_BOT], pOutput->u12_logo_layer1_rim[_RIM_LFT], pOutput->u12_logo_layer1_rim[_RIM_RHT]);                  
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_logo_layer2_rim[_RIM_TOP], pOutput->u12_logo_layer2_rim[_RIM_BOT], pOutput->u12_logo_layer2_rim[_RIM_LFT], pOutput->u12_logo_layer2_rim[_RIM_RHT]);
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d][%d][%d][%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_logo_layer3_rim[_RIM_TOP], pOutput->u12_logo_layer3_rim[_RIM_BOT], pOutput->u12_logo_layer3_rim[_RIM_LFT], pOutput->u12_logo_layer3_rim[_RIM_RHT]);                  
    }    
}

VOID RimCtrl_Film5RgnPos_Convert_RTK2885pp(const _PARAM_RimCtrl *pParam, _OUTPUT_RimCtrl *pOutput)
{     
}

VOID RimCtrl_Film_Rim_Convert_RTK2885pp(const _PARAM_RimCtrl *pParam, _OUTPUT_RimCtrl *pOutput)
{
}

VOID RimCtrl_Output_RTK2885pp(const _PARAM_RimCtrl *pParam, _OUTPUT_RimCtrl *pOutput)
{
    // const _PQLCONTEXT *s_pContext = GetPQLContext();
    
    if(pParam->u1_log_en && pOutput->u12_fr_cnt==60)
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d][%d][%d][%d], [%d], [%d] **********//\n", __FUNCTION__, __LINE__, pOutput->u12_Rim_Result[_RIM_TOP], pOutput->u12_Rim_Result[_RIM_BOT], pOutput->u12_Rim_Result[_RIM_LFT], pOutput->u12_Rim_Result[_RIM_RHT], pOutput->u1_RimChange, pOutput->u12_fr_cnt);

    if (pOutput->u1_RimChange == 1)
    {
        RimCtrl_ME1_Rim_Convert_RTK2885pp(pParam, pOutput);
        RimCtrl_ME2_Rim_Convert_RTK2885pp(pParam, pOutput);    
        RimCtrl_Dh_Rim_Convert_RTK2885pp(pParam, pOutput);
        RimCtrl_MC_Rim_Convert_RTK2885pp(pParam, pOutput);
        RimCtrl_Logo_Rim_Convert_RTK2885pp(pParam, pOutput);
        RimCtrl_Film_Rim_Convert_RTK2885pp(pParam, pOutput);
    }
}


VOID RimCtrl_Init_RTK2885pp(_OUTPUT_RimCtrl *pOutput)
{

}

VOID RimCtrl_DynamicBlackTh_RTK2885pp(VOID)
{
    // const _PQLCONTEXT *s_pContext = GetPQLContext();
    
    // unsigned char u8_darklevel = (s_pContext->_external_info.u8_DarkLevel/*+u4_black_offset*/);
}


VOID RimCtrl_SC_rim_RTK2885pp(_OUTPUT_RimCtrl *pOutput)
{
}

VOID RimCtrl_post_RTK2885pp(const _PARAM_RimCtrl *pParam, _OUTPUT_RimCtrl *pOutput)
{
    const _PQLCONTEXT *s_pContext = GetPQLContext();
    
    int i=0;
    int panning_idx=0;
    int rim_diff=0;
    int rim_sign[4]={1,-1,1,-1};
    static unsigned short rim_0[4]={0,0,0,0}, rim_1[4]={0,0,0,0};
    static unsigned short rim_in_cnt[4]={0,0,0,0}, rim_out_cnt[4]={0,0,0,0};
    static bool force_hw = 1; 
    // ----------------- threshold -------------
    int in_range = 0;
    int out_range =3;
    int debounce_in_th = 5;
    int debounce_out_th = 1;
    

    /* ******************for FPGA bring-up test
    static unsigned int fr_cnt = 0;
    unsigned char fr_cycle = 20;
    unsigned char step = 10;
    if((fr_cnt % fr_cycle) == 0)
    {
        if(fr_cnt==0)
        {
            pOutput->u12_Rim_Result[_RIM_LFT] = 0;
            pOutput->u12_Rim_Result[_RIM_RHT] = 3839;
            pOutput->u12_Rim_Result[_RIM_TOP] = 0;
            pOutput->u12_Rim_Result[_RIM_BOT] = 2159;
        }
        else
        {
            pOutput->u12_Rim_Result[_RIM_LFT] += step;
            pOutput->u12_Rim_Result[_RIM_RHT] -= step; 
            pOutput->u12_Rim_Result[_RIM_TOP] += step;
            pOutput->u12_Rim_Result[_RIM_BOT] -= step;                
        }
    }

    fr_cnt++;
    if(fr_cnt == (100 * fr_cycle))
        fr_cnt = 0;
    */

    if(pParam->u1_log_en)
        rtd_pr_memc_emerg("//********** [%s][%d] = [%d] , [%d] , [%d]  **********//\n", __FUNCTION__, __LINE__, force_hw, rim_in_cnt[0], rim_out_cnt[0]);    

    //--------- Rim init form read-only register ---------
    for (i = 0; i < _RIM_NUM; i ++)
    {
        //pOutput->u12_Rim_Result[i] = s_pContext->_output_read_comreg.u12_BBD_fineRim_rb[i];    
        //pOutput->u12_Rim_0[i] = s_pContext->_output_read_comreg.u12_BBD_fineRim_rb[i];
        //pOutput->u12_Rim_1[i] = s_pContext->_output_read_comreg.u12_BBD_ex[i];
        //pOutput->u12_Rim_Result[i] = pOutput->u12_Rim_0[i];
        rim_0[i] = s_pContext->_output_read_comreg.u12_BBD_fineRim_rb[i];
        rim_1[i] = s_pContext->_output_read_comreg.u12_BBD_ex[i];           
    }
    
    //--------- Rim debounce ---------
    for (i = 0; i < _RIM_NUM; i ++)
    {
        rim_diff = (rim_0[i] - pOutput->u12_Rim_0[i]) * rim_sign[i];   
        
        if(rim_diff > in_range)
        {
            rim_out_cnt[i] = 0;
            rim_in_cnt[i] += 1;
        }
        else if(rim_diff < out_range) 
        {
            rim_in_cnt[i] = 0;
            rim_out_cnt[i] += 1;
        }   
        
        if((rim_in_cnt[i] >= debounce_in_th) || (rim_out_cnt[i] >= debounce_out_th) || (force_hw == 1))
        {
            pOutput->u12_Rim_0[i] = rim_0[i];
            pOutput->u12_Rim_1[i] = rim_1[i];
            pOutput->u1_RimChange = 1;
        }
        else
        {
            pOutput->u1_RimChange = 0;
        }
        
        pOutput->u12_Rim_Result[i] = pOutput->u12_Rim_1[i];
 
        //xxxxxxxxxxxxxxxxxxx   for FPGA verification   xxxxxxxxxxxxxxxxxx
        if(pParam->u1_rim_force_en)
        {
            pOutput->u12_Rim_0[0] = pParam->u12_rim_froce_top;
            pOutput->u12_Rim_0[1] = pParam->u12_rim_froce_bot;
            pOutput->u12_Rim_0[2] = pParam->u12_rim_froce_rht;
            pOutput->u12_Rim_0[3] = pParam->u12_rim_froce_lft; 
            pOutput->u12_Rim_1[0] = pParam->u12_rim_froce_top;
            pOutput->u12_Rim_1[1] = pParam->u12_rim_froce_bot;
            pOutput->u12_Rim_1[2] = pParam->u12_rim_froce_rht;
            pOutput->u12_Rim_1[3] = pParam->u12_rim_froce_lft;           
        }
        
        pOutput->u12_Rim_Result[i] = (pParam->u1_sel_rim==0) ? pOutput->u12_Rim_0[i] : pOutput->u12_Rim_1[i];
        
        //--------- store global variable ----------
        rim_in_cnt[i] = (rim_in_cnt[i] >=  debounce_in_th)  ?  debounce_in_th  : rim_in_cnt[i];            
        rim_out_cnt[i] = (rim_out_cnt[i] >= debounce_out_th)  ? debounce_out_th : rim_out_cnt[i];
        force_hw = pParam->u1_rim_debounce_en ? 0 : 1;
        
    }
    

    //--------- calc shift/offset -----------
    if(_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb) < _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb))
        panning_idx = 1;
    else
        panning_idx = 0;

    pOutput->x_shift_coef = panning_idx;
    pOutput->y_shift_coef = 1 - panning_idx;        

}


//Input signal and parameters are connected locally
VOID RimCtrl_Proc_RTK2885pp(const _PARAM_RimCtrl *pParam, _OUTPUT_RimCtrl *pOutput)
{
    // const _PQLCONTEXT *s_pContext = GetPQLContext();    
    
    RimCtrl_OutResolution_Parse_RTK2885pp(pOutput);
    if (pParam->u1_RimCtrl_en == 1)
    {
        int i = 0;
        unsigned int rimSize = 0;
        pOutput->u1_RimChange = 0;
        // unsigned int u32_rb_val = 0;

        //RimCtrl_DynamicBlackTh();
        //RimCtrl_HW_extract(pParam, pOutput);
        //RimCtrl_SC_rim(pOutput);
        RimCtrl_post_RTK2885pp(pParam, pOutput);
        RimCtrl_Output_RTK2885pp(pParam, pOutput); // adjust features base on result rim

        for (i = 0; i < _RIM_NUM; i ++)
        {
            pOutput->u12_Rim_Pre[i] = pOutput->u12_Rim_Result[i];
            pOutput->u12_Rim_Pre_Det[i] = pOutput->u12_Rim_Cur[i];
        }

        //calculate rim ratio
        rimSize = (pOutput->u12_Rim_Result[_RIM_RHT]-pOutput->u12_Rim_Result[_RIM_LFT] + 1)*
                  (pOutput->u12_Rim_Result[_RIM_BOT]-pOutput->u12_Rim_Result[_RIM_TOP] + 1);
                
        pOutput->u32_rimRatio = (rimSize == 0) ? 128 :(rimSize<<7) / 
                                ((pOutput->u12_out_resolution[_RIM_RHT]+1)*(pOutput->u12_out_resolution[_RIM_BOT]+1));

        
    } 
    else
    {
        pOutput->u1_RimChange = (pOutput->u1_RimControl_pre !=  pParam->u1_RimCtrl_en) ? 1 : 0;
        pOutput->u12_Rim_Result[_RIM_LFT] = pOutput->u12_out_resolution[_RIM_LFT];
        pOutput->u12_Rim_Result[_RIM_RHT] = pOutput->u12_out_resolution[_RIM_RHT];
        pOutput->u12_Rim_Result[_RIM_TOP] = pOutput->u12_out_resolution[_RIM_TOP];
        pOutput->u12_Rim_Result[_RIM_BOT] = pOutput->u12_out_resolution[_RIM_BOT];

        // RimCtrl_Output(pParam, pOutput);
    }
    pOutput->u1_RimControl_pre = pParam->u1_RimCtrl_en;

    pOutput->u12_fr_cnt++;

    if(pOutput->u12_fr_cnt>60)
        pOutput->u12_fr_cnt = 0;
    
}

