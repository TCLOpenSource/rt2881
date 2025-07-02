/*============================================================================
* Copyright (c)      Realtek Semiconductor Corporation, 2024
* All rights reserved.
* ===========================================================================*/

//----------------------------------------------------------------------------
// ID Code      : txpre_display.c
// Update Note  :
//----------------------------------------------------------------------------
#include <rtd_log/rtd_module_log.h>
#include <tvscalercontrol/txpre/txpre_dtg.h>
#include <tvscalercontrol/txpre/txpre_display.h>
#include <scaler/scalerCommon.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <rbus/hdmitx_phy_reg.h>

extern void ScalerHdmiTx_Set_ClkGateSel(void);

void Txpretg_set_disp_setting(StructTxPreInfo *txpre_info)
{
    //Disp_en disable
    Set_txpretg_disp_en(0);

    //Reset to free run mode
    Txpretg_reset_to_free_run();

    //Enable D1 double buffer
    //Txpretg_D1_double_buffer_en(1);

    /** Hint **/
    //B804b000 bit 16(Src_vs_sel)/18(Src_vs_vgip_sel)
    //If [18] = 1, use vgip input vs
    //If [18] = 0, [16] = 1, use vgip_out_vs
    //If [18] = 0, [16] = 0, use uzd_out_vs
    Set_txpretg_Src_vs_vgip_sel(1);
    Set_txpretg_Src_vs_sel(0);

    Txpretg_set_display_timing_gen(txpre_info);

    ScalerHdmiTx_Set_ClkGateSel();

    if(txpre_info->Interlace) {
        rtd_pr_hdmitx_info("[txpre] Interlace source = %d\n", txpre_info->Interlace);
        Set_txpretg_Reg_2field_mode(1);
        Set_txpretg_def_field(0);
    }

    /** Hint **/
    // Polarity use two bit to separate H/V sync polarity
    // Polarity = 3 --> H positive , V positive
    // Polarity = 2 --> H positive , V negative
    // Polarity = 1 --> H negative , V positive
    // Polarity = 0 --> H negative , V negative
    if(txpre_info->Polarity == 2 || txpre_info->Polarity == 0) {
        rtd_pr_hdmitx_info("[txpre] Vsync negetive source = %d\n", txpre_info->Polarity);
        Set_txpretg_Vs_neg_pol(1);
    }

    //Set double buffer apply
    //Txpretg_D1_double_buffer_apply();

    //Check double buffer apply finish or not
    /*while(Txpretg_get_double_buffer_apply_status(D1) != 1) {
        continue;
    }*/

    //Disp_en enable
    Set_txpretg_disp_en(1);
}

void Txpretg_reset_to_free_run(void)
{
    hdmitx_phy_sys_hdmitx_disp_ss_RBUS hdmitx_disp_ss_reg;

    Txpretg_tracing_control_en(0);

    hdmitx_disp_ss_reg.regValue = IoReg_Read32(HDMITX_PHY_SYS_HDMITX_DISP_SS_reg);
    hdmitx_disp_ss_reg.reg_disp_ss_en = 0;
    hdmitx_disp_ss_reg.reg_disp_ss_load = 0;
    IoReg_Write32(HDMITX_PHY_SYS_HDMITX_DISP_SS_reg, hdmitx_disp_ss_reg.regValue);

    //Disable D1 double buffer
    Txpretg_D1_double_buffer_en(0);

    //Disable D2 double buffer
    Txpretg_D2_double_buffer_en(0);

    Set_txpretg_fix_last_line(0);

    Set_txpretg_first_vsync_gen(1); // following frame disp_en

    Set_txpretg_disp_fsync_en(0);
}

void Txpretg_set_display_timing_gen(StructTxPreInfo *txpre_info)
{
    unsigned int vtotal = 0;

    //Set htotal
    rtd_pr_hdmitx_info("[txpre] H total = %d\n", txpre_info->TXPRE_OUTPUT_HTOTAL-1);
    Set_txpretg_dh_total((unsigned int)(txpre_info->TXPRE_OUTPUT_HTOTAL-1));

    //Set h sync width
    Set_txpretg_dh_width((unsigned int)(txpre_info->TXPRE_OUTPUT_HSYNC-1));

    //Set h den start end
    Set_txpretg_dh_den_start_end((unsigned int)(txpre_info->TXPRE_OUTPUT_DEN_HSTA), (unsigned int)(txpre_info->TXPRE_OUTPUT_DEN_HEND));

    //Set h act start end
    Set_txpretg_dh_act_start_end((unsigned int)(txpre_info->TXPRE_OUTPUT_ACT_HSTA), (unsigned int)(txpre_info->TXPRE_OUTPUT_ACT_HSTA + txpre_info->TXPRE_OUTPUT_ACT_WID));

    //Set Vtotal
    if(txpre_info->Interlace) {
        if(txpre_info->TXPRE_OUTPUT_VTOTAL%2 == 0) {
            vtotal = (unsigned int)(txpre_info->TXPRE_OUTPUT_VTOTAL*2);
            rtd_pr_hdmitx_info("[txpre] V total = %d\n", vtotal);
            Set_txpretg_dv_total(vtotal);
        } else {
            vtotal = (unsigned int)(txpre_info->TXPRE_OUTPUT_VTOTAL*2-1);
            rtd_pr_hdmitx_info("[txpre] V total = %d\n", vtotal);
            Set_txpretg_dv_total(vtotal);
        }
    } else {
        rtd_pr_hdmitx_info("[txpre] V total = %d\n", txpre_info->TXPRE_OUTPUT_VTOTAL-1);
        Set_txpretg_dv_total((unsigned int)(txpre_info->TXPRE_OUTPUT_VTOTAL-1));
    }

    //Set v sync legth
    Set_txpretg_dv_length((unsigned int)(txpre_info->TXPRE_OUTPUT_VSYNC-1));

    //Set v den start end
    Set_txpretg_dv_den_start_end((unsigned int)(txpre_info->TXPRE_OUTPUT_DEN_VSTA), (unsigned int)(txpre_info->TXPRE_OUTPUT_DEN_VEND));

    //Set v act start end
    Set_txpretg_dv_act_start_end((unsigned int)(txpre_info->TXPRE_OUTPUT_ACT_VSTA), (unsigned int)(txpre_info->TXPRE_OUTPUT_ACT_VSTA + txpre_info->TXPRE_OUTPUT_ACT_LEN));

    //Set display border color, default use black
    Set_txpretg_display_border_color(0, 0, 0);
}

void Txpretg_set_VRR_timing_framesync(StructTxPreInfo *txpre_info)
{
    rtd_pr_hdmitx_info("[txpre] VRR mode set timing sync\n");

    //Disp_en disable
    Set_txpretg_disp_en(0);
    
    //Enable D2 double buffer
    //Txpretg_D2_double_buffer_en(1);

    Set_txpretg_fix_last_line(1);

    Set_txpretg_first_vsync_gen(0); // following frame sync (uzd)

    Txpretg_set_iv2dv_delay(txpre_info);

    Txpretg_set_fix_last_line_tracking(txpre_info);
    
    //Set double buffer apply
    //Txpretg_D2_double_buffer_apply();

    //Check double buffer apply finish or not
    /*while(Txpretg_get_double_buffer_apply_status(D2) != 1) {
        continue;
    }*/
    
    //Fsync enable
    //Txpretg_D1_double_buffer_en(1);
    Set_txpretg_disp_fsync_en(1);
    //Txpretg_D1_double_buffer_apply();

    //Disp_en enable
    Set_txpretg_disp_en(1);
}

void Txpretg_set_line_tracking_timing_freerun(StructTxPreInfo *txpre_info)
{
    //Disp_en disable
    Set_txpretg_disp_en(0);
    
    //Enable D2 double buffer
    //Txpretg_D2_double_buffer_en(1);

    Set_txpretg_first_vsync_gen(0); // following frame sync (uzd)

    Txpretg_set_iv2dv_delay(txpre_info);

    Txpretg_set_fix_last_line_tracking(txpre_info);
    
    //Set double buffer apply
    //Txpretg_D2_double_buffer_apply();

    //Check double buffer apply finish or not
    /*while(Txpretg_get_double_buffer_apply_status(D2) != 1) {
        continue;
    }*/

    //Disp_en enable
    Set_txpretg_disp_en(1);
}

void Txpretg_set_iv2dv_delay(StructTxPreInfo *txpre_info)
{
    unsigned int iv2dv_line;

    //Enable D1 double buffer
    Txpretg_D1_double_buffer_en(1);

    /** Hint **/
    // We use 4k two line as fs buffer middle water level
    // Line delay = (3840*2) / Input_h_active_size
    iv2dv_line = (3840*2) / (txpre_info->IPH_ACT_WID);
    Set_txpretg_fs_iv_dv_pixel_delay(0);
    Set_txpretg_fs_iv_dv_line_delay(iv2dv_line);

    //Set double buffer apply
    Txpretg_D1_double_buffer_apply();
}

unsigned int Txpretg_get_tx_dpll_clk(StructTxPreInfo *txpre_info)
{
    unsigned int dclk = 0;
    unsigned int htotal = 0;
    unsigned int vtotal = 0;
    unsigned int frame_rate = 0;

    rtd_pr_hdmitx_info("[txpre] TX pre clk info: htotal = %d, vtotal = %d, frame_rate = %d\n", txpre_info->TXPRE_OUTPUT_HTOTAL, txpre_info->TXPRE_OUTPUT_VTOTAL, txpre_info->TXPRE_OUTPUT_FRAMERATE);
    
    if(txpre_info->pixelclock != 0) {
        dclk = txpre_info->pixelclock;
    }
    else {
        htotal = txpre_info->TXPRE_OUTPUT_HTOTAL;
        vtotal = txpre_info->TXPRE_OUTPUT_VTOTAL;
        frame_rate = txpre_info->TXPRE_OUTPUT_FRAMERATE / 10;

        dclk = (htotal * vtotal * frame_rate) / 100000;
    }
    
    rtd_pr_hdmitx_info("[txpre] TX ouput dpll clk = %d\n", dclk);
    return dclk;
}

void Txpretg_set_fix_last_line_tracking(StructTxPreInfo *txpre_info)
{
    hdmitx_phy_sys_hdmitx_disp_ss_RBUS hdmitx_disp_ss_reg;
    hdmitx_phy_hdmitx_disp_pll_ssc2_RBUS hdmitx_disp_pll_ssc2_reg;
    hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl4_RBUS hdmitx_disp_dpll_in_fsync_tracking_ctrl4_reg;
    hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl_RBUS hdmitx_disp_dpll_in_fsync_tracking_ctrl_reg;
    unsigned int temp = 0;
    unsigned int tracking_cnt_i = 0;
    unsigned int tracking_cnt_d = 0;
    unsigned int tracking_cnt_i_number = 0;
    unsigned int tracking_cnt_i_denom = 0;
    unsigned int synthesisN_ratio = 0;
    unsigned int lock_th = 0xfff;

    //Fll tracking disable
    Txpretg_tracing_control_en(0);

    //Tracking cnt d = 2*DHtotal
    if(txpre_info->TXPRE_OUTPUT_HTOTAL%2 == 1)
        tracking_cnt_d = (unsigned int)(txpre_info->TXPRE_OUTPUT_HTOTAL + 1);
    else
        tracking_cnt_d = (unsigned int)(txpre_info->TXPRE_OUTPUT_HTOTAL);

    tracking_cnt_d = FLL_TRACKING_LINE_NUM*tracking_cnt_d;
    Txpretg_set_tracking_cnt_d(tracking_cnt_d);

    //Tracking cnt i = floor(2*Ihtotal*(Ivtotal/Dvtotal))
    temp = ((txpre_info->IVTotal*100000) / txpre_info->TXPRE_OUTPUT_VTOTAL);
    tracking_cnt_i = (FLL_TRACKING_LINE_NUM*txpre_info->IHTotal*temp)/100000;
    Txpretg_set_tracking_cnt_i(tracking_cnt_i);

    //Tracking cnt i number = 2*Ihtotal*Ivtotal - tracking_cnt_i*Dvtotal
    //Tracking cnt i denom = Dvtotal
    tracking_cnt_i_number = (unsigned int)(2*txpre_info->IHTotal*txpre_info->IVTotal) - (tracking_cnt_i*txpre_info->TXPRE_OUTPUT_VTOTAL);
    Txpretg_set_tracking_cnt_i_number(tracking_cnt_i_number);
    tracking_cnt_i_denom = (unsigned int)txpre_info->TXPRE_OUTPUT_VTOTAL;
    Txpretg_set_tracking_cnt_i_denom(tracking_cnt_i_denom);

    //Set tracking systhesisN
    Txpretg_set_tracking_Max_multi_systhesisN(MAX_MULTIPLE_SYNTHESISN);
    Txpretg_set_tracking_systhesisN(SYNTHESISN);

    //SynthesisN_ratio = (synthesisN/4096)*tracking_cnt_d*(Fdclk/250)
    synthesisN_ratio = (SYNTHESISN*tracking_cnt_d*Txpretg_get_tx_dpll_clk(txpre_info))/(4096*2500);
    if(synthesisN_ratio == 0)
        synthesisN_ratio = 1; //synthesisN_ratio cannot be zero.
    Txpretg_set_tracking_systhesisN_ratio(synthesisN_ratio);
        
    Txpretg_set_tracking_offset_ft_lock_en(0);
    Txpretg_set_tracking_synthesisn_ratio_shift(0);

    //Fll tracking en
    Txpretg_tracing_control_en(1);

    //Disable ss load & enable
    hdmitx_disp_ss_reg.regValue = IoReg_Read32(HDMITX_PHY_SYS_HDMITX_DISP_SS_reg);
    hdmitx_disp_ss_reg.reg_disp_ss_en = 0;
    hdmitx_disp_ss_reg.reg_disp_ss_load = 0;
    IoReg_Write32(HDMITX_PHY_SYS_HDMITX_DISP_SS_reg, hdmitx_disp_ss_reg.regValue);

    hdmitx_disp_dpll_in_fsync_tracking_ctrl4_reg.regValue = IoReg_Read32(HDMITX_PHY_HDMITX_DISP_DPLL_IN_FSYNC_TRACKING_CTRL4_reg);
    hdmitx_disp_dpll_in_fsync_tracking_ctrl4_reg.reg_new_fll_tracking_offset_inverse = 1;
    if(Txpretg_get_tx_dpll_clk(txpre_info) >= DPLL_CLK_4K)
        hdmitx_disp_dpll_in_fsync_tracking_ctrl4_reg.reg_new_fll_tracking_mul = FLL_MULTIPLIER_4K;
    else 
        hdmitx_disp_dpll_in_fsync_tracking_ctrl4_reg.reg_new_fll_tracking_mul = FLL_MULTIPLIER_2K;
    IoReg_Write32(HDMITX_PHY_HDMITX_DISP_DPLL_IN_FSYNC_TRACKING_CTRL4_reg, hdmitx_disp_dpll_in_fsync_tracking_ctrl4_reg.regValue);    

    //Bypass pi_disp
    hdmitx_disp_pll_ssc2_reg.regValue = IoReg_Read32(HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg);
    hdmitx_disp_pll_ssc2_reg.reg_bypass_pi_disp = 0;
    IoReg_Write32(HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg, hdmitx_disp_pll_ssc2_reg.regValue);

    Txpretg_set_tracking_offset_ft_lock_en(0);
    Txpretg_set_tracking_lock_th(lock_th);

    //HDMI phy sscg
    hdmitx_disp_dpll_in_fsync_tracking_ctrl_reg.regValue = IoReg_Read32(HDMITX_PHY_HDMITX_DISP_DPLL_IN_FSYNC_TRACKING_CTRL_reg);
    hdmitx_disp_dpll_in_fsync_tracking_ctrl_reg.reg_fll_tracking_en = 1;
    IoReg_Write32(HDMITX_PHY_HDMITX_DISP_DPLL_IN_FSYNC_TRACKING_CTRL_reg, hdmitx_disp_dpll_in_fsync_tracking_ctrl_reg.regValue);

    //Enable ss load & enable
    hdmitx_disp_ss_reg.regValue = IoReg_Read32(HDMITX_PHY_SYS_HDMITX_DISP_SS_reg);
    hdmitx_disp_ss_reg.reg_disp_ss_en = 1;
    hdmitx_disp_ss_reg.reg_disp_ss_load = 1;
    IoReg_Write32(HDMITX_PHY_SYS_HDMITX_DISP_SS_reg, hdmitx_disp_ss_reg.regValue);

    //rtd_pr_hdmitx_info("[txpre] check reg_fll_tracking_en = %x\n", IoReg_Read32(HDMITX_PHY_HDMITX_DISP_DPLL_IN_FSYNC_TRACKING_CTRL_reg));
}

