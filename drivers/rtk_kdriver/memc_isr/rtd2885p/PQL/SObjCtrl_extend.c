#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_reg_def.h"
///////////////////////////////////////////////////////////////////////////////////////////

VOID SObjCtrl_Init_RTK2885pp(_OUTPUT_SOBJ_CTRL *pOutput)
{
    pOutput->u32_Sobj_med_lvl[0] = 0;
    pOutput->u32_Sobj_med_lvl[1] = 0;
    pOutput->u32_Sobj_med_lvl[2] = 0;
    pOutput->u32_Sobj_med_lvl[3] = 0;
    pOutput->u32_Sobj_rgn_i_clr = 0;
    pOutput->u32_Sobj_rgn_p_clr = 0;
}

VOID SObjCtrl_Proc_RTK2885pp(const _PARAM_SOBJ_CTRL *pParam, _OUTPUT_SOBJ_CTRL *pOutput)
{
    const _PQLCONTEXT *s_pContext = GetPQLContext();

    int i=0;
    unsigned char lut_idx = 0;
    unsigned char sobj_rgn_clr=0;
    unsigned char sobj_med_lvl=0;
    static unsigned char sobj_cnt_iir[32] = {0};
    static unsigned char rmv_flag = 0;
    static unsigned char rmv_cnt_flag = 0;
    static unsigned short sobj_rgn_cnt = 0;
    static unsigned char med_curve[2][8] = {{32, 128, 160, 0, 128, 255, 8, 32},
                                            {16,  16,  16, 0, 255, 255, 0,  0}};    
    int sobj_cnt_th = 16;
    int iir_coef =32;
    unsigned int print_in_func;
    
    if(pParam->u1_Sobj_en== 0) 
        return;
    
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    
    for (i = 0; i < 32; i ++)
    {    
        if((s_pContext->_output_read_comreg.u13_me_sobj_cnt_i_rb[i] > 0) && (s_pContext->_output_read_comreg.u13_me_sobj_cnt_p_rb[i] > 0))
            sobj_rgn_cnt = 64;
        else
            sobj_rgn_cnt = 0;
    
        //--------- ME1 sobj_rgn_clr ------------    
        sobj_rgn_clr = (s_pContext->_output_read_comreg.u13_me_sobj_cnt_i_rb[i] > sobj_cnt_th) ? 1 : 0;
        pOutput->u32_Sobj_rgn_i_clr = pOutput->u32_Sobj_rgn_i_clr | (sobj_rgn_clr << i);
        sobj_rgn_clr = (s_pContext->_output_read_comreg.u13_me_sobj_cnt_p_rb[i] > sobj_cnt_th) ? 1 : 0;
        pOutput->u32_Sobj_rgn_p_clr = pOutput->u32_Sobj_rgn_p_clr | (sobj_rgn_clr << i);
        
        //--------- MC med_fltr_lvl ------------
        sobj_cnt_iir[i] = (sobj_cnt_iir[i] * iir_coef + sobj_rgn_cnt * (64-iir_coef))/64;
        sobj_med_lvl = ((sobj_cnt_iir[i] > 0) && (sobj_rgn_cnt==0)) ? 2 : 1;
        sobj_med_lvl = _CLIP_(pOutput->u32_Sobj_med_lvl[i], 1, 4);

        pOutput->u32_Sobj_med_lvl[(i>>3)] = pOutput->u32_Sobj_med_lvl[(i>>3)] | (sobj_med_lvl << ((i & 7) * 4));

        // --------- detect scene of Sobj ------------
        if(_ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i]) > 30 || _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i]) > 30)
            rmv_flag ++;
        if((s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[i] + s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[i]) < 30)
            rmv_cnt_flag ++;   
       
    }    

    //------------ color detect -----------
    if(pParam->u1_Sobj_color_en && ((s_pContext->_output_read_comreg.u24_ippre_color_cnt1_rb>>15) > 70))
    {
         pOutput->u32_Sobj_rgn_i_clr = 0xffffffff;          
         pOutput->u32_Sobj_rgn_p_clr = 0xffffffff;
    }
        
    if((rmv_flag > 16) && (rmv_cnt_flag > 5))  
        lut_idx = 1;
    else
        lut_idx = 0;

    for (i = 0; i < 8; i ++)
        pOutput->u8_med_curve[i] = med_curve[lut_idx][i];

}
