/*============================================================================
* Copyright (c)      Realtek Semiconductor Corporation, 2024
* All rights reserved.
* ===========================================================================*/

//----------------------------------------------------------------------------
// ID Code      : txpre_dtg.c
// Update Note  :
//----------------------------------------------------------------------------
#include <rbus/ppoverlay_txpretg_reg.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/txpre/txpre_dtg.h>

void Set_txpretg_disp_en(unsigned int disp_en)
{
    ppoverlay_txpretg_txpretg_display_timing_ctrl1_RBUS txpretg_disp_timing_ctrl_reg;

    txpretg_disp_timing_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg);
    txpretg_disp_timing_ctrl_reg.disp_en = disp_en;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg, txpretg_disp_timing_ctrl_reg.regValue);
}

void Set_txpretg_disp_fsync_en(unsigned int en)
{
    ppoverlay_txpretg_txpretg_display_timing_ctrl1_RBUS txpretg_disp_timing_ctrl_reg;

    txpretg_disp_timing_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg);
    txpretg_disp_timing_ctrl_reg.disp_fsync_en = en;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg, txpretg_disp_timing_ctrl_reg.regValue);
}

void Set_txpretg_Reg_2field_mode(unsigned int en)
{
    ppoverlay_txpretg_txpretg_display_timing_ctrl1_RBUS txpretg_disp_timing_ctrl_reg;

    txpretg_disp_timing_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg);
    txpretg_disp_timing_ctrl_reg.reg_2field_mode = en;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg, txpretg_disp_timing_ctrl_reg.regValue);
}

void Set_txpretg_def_field(unsigned int en)
{
    ppoverlay_txpretg_txpretg_display_timing_ctrl1_RBUS txpretg_disp_timing_ctrl_reg;

    txpretg_disp_timing_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg);
    txpretg_disp_timing_ctrl_reg.def_field = en;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg, txpretg_disp_timing_ctrl_reg.regValue);
}

void Set_txpretg_Src_vs_vgip_sel(unsigned int mode)
{
    ppoverlay_txpretg_txpretg_display_timing_ctrl1_RBUS txpretg_disp_timing_ctrl_reg;

    txpretg_disp_timing_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg);
    txpretg_disp_timing_ctrl_reg.src_vs_vgip_sel = mode;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg, txpretg_disp_timing_ctrl_reg.regValue);
}

void Set_txpretg_Src_vs_sel(unsigned int mode)
{
    ppoverlay_txpretg_txpretg_display_timing_ctrl1_RBUS txpretg_disp_timing_ctrl_reg;

    txpretg_disp_timing_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg);
    txpretg_disp_timing_ctrl_reg.src_vs_sel = mode;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg, txpretg_disp_timing_ctrl_reg.regValue);
}

void Set_txpretg_Vs_neg_pol(unsigned int mode)
{
    ppoverlay_txpretg_txpretg_display_timing_ctrl1_RBUS txpretg_disp_timing_ctrl_reg;

    txpretg_disp_timing_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg);
    txpretg_disp_timing_ctrl_reg.vs_neg_pol = mode;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg, txpretg_disp_timing_ctrl_reg.regValue);
}

void Set_txpretg_first_vsync_gen(unsigned int mode)
{
    ppoverlay_txpretg_txpretg_display_timing_ctrl1_RBUS txpretg_disp_timing_ctrl_reg;

    txpretg_disp_timing_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg);
    txpretg_disp_timing_ctrl_reg.first_vsync_gen = mode;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg, txpretg_disp_timing_ctrl_reg.regValue);
}

void Set_txpretg_fix_last_line(unsigned int en)
{
    ppoverlay_txpretg_txpretg_display_timing_ctrl1_RBUS txpretg_disp_timing_ctrl_reg;

    txpretg_disp_timing_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg);
    txpretg_disp_timing_ctrl_reg.fix_last_line = en;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Timing_CTRL1_reg, txpretg_disp_timing_ctrl_reg.regValue);
}

void Set_txpretg_dh_width(unsigned int dh_width)
{
    ppoverlay_txpretg_txpretg_dh_width_RBUS txpretg_dh_width_reg;

    txpretg_dh_width_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_DH_Width_reg);
    txpretg_dh_width_reg.dh_width = dh_width;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_DH_Width_reg, txpretg_dh_width_reg.regValue);
}

void Set_txpretg_dh_total(unsigned int dh_total)
{
    ppoverlay_txpretg_txpretg_dh_total_length_RBUS txpretg_dh_total_reg;

    txpretg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_DH_Total_Length_reg);
    txpretg_dh_total_reg.dh_total = dh_total;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_DH_Total_Length_reg, txpretg_dh_total_reg.regValue);
}

void Set_txpretg_dv_length(unsigned int dv_length)
{
    ppoverlay_txpretg_txpretg_dv_length_RBUS txpretg_dv_length_reg;

    txpretg_dv_length_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_DV_Length_reg);
    txpretg_dv_length_reg.dv_length = dv_length;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_DV_Length_reg, txpretg_dv_length_reg.regValue);
}

void Set_txpretg_dv_total(unsigned int dv_total)
{
    ppoverlay_txpretg_txpretg_dv_total_RBUS txpretg_dv_total_reg;

    txpretg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_DV_total_reg);
    txpretg_dv_total_reg.dv_total = dv_total;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_DV_total_reg, txpretg_dv_total_reg.regValue);
}

void Set_txpretg_dh_den_start_end(unsigned int dh_den_sta, unsigned int dh_den_end)
{
    ppoverlay_txpretg_txpretg_dh_den_start_end_RBUS txpretg_dh_den_start_end_reg;

    txpretg_dh_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_DH_DEN_Start_End_reg);
    txpretg_dh_den_start_end_reg.dh_den_sta = dh_den_sta;
    txpretg_dh_den_start_end_reg.dh_den_end = dh_den_end;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_DH_DEN_Start_End_reg, txpretg_dh_den_start_end_reg.regValue);
}

void Set_txpretg_dv_den_start_end(unsigned int dv_den_sta, unsigned int dv_den_end)
{
    ppoverlay_txpretg_txpretg_dv_den_start_end_RBUS txpretg_dv_den_start_end_reg;

    txpretg_dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_DV_DEN_Start_End_reg);
    txpretg_dv_den_start_end_reg.dv_den_sta = dv_den_sta;
    txpretg_dv_den_start_end_reg.dv_den_end = dv_den_end;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_DV_DEN_Start_End_reg, txpretg_dv_den_start_end_reg.regValue);
}

void Set_txpretg_fs_iv_dv_pixel_delay(unsigned int iv2dv_pixel)
{
    ppoverlay_txpretg_txpretg_fs_iv_dv_fine_tuning1_RBUS txpretg_fs_iv_dv_pixel_delay_reg;

    txpretg_fs_iv_dv_pixel_delay_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_FS_IV_DV_Fine_Tuning1_reg);
    txpretg_fs_iv_dv_pixel_delay_reg.iv2dv_pixel = iv2dv_pixel;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_FS_IV_DV_Fine_Tuning1_reg, txpretg_fs_iv_dv_pixel_delay_reg.regValue);
}

void Set_txpretg_fs_iv_dv_line_delay(unsigned int iv2dv_line)
{
    ppoverlay_txpretg_txpretg_fs_iv_dv_fine_tuning2_RBUS txpretg_fs_iv_dv_line_delay_reg;

    txpretg_fs_iv_dv_line_delay_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_FS_IV_DV_Fine_Tuning2_reg);
    txpretg_fs_iv_dv_line_delay_reg.iv2dv_line = iv2dv_line;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_FS_IV_DV_Fine_Tuning2_reg, txpretg_fs_iv_dv_line_delay_reg.regValue);
}

void Set_txpretg_dh_act_start_end(unsigned int dh_act_sta, unsigned int dh_act_end)
{
    ppoverlay_txpretg_txpretg_dh_act_start_end_RBUS txpretg_dh_act_start_end_reg;

    txpretg_dh_act_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_DH_ACT_Start_End_reg);
    txpretg_dh_act_start_end_reg.dh_act_sta = dh_act_sta;
    txpretg_dh_act_start_end_reg.dh_act_end = dh_act_end;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_DH_ACT_Start_End_reg, txpretg_dh_act_start_end_reg.regValue);
}

void Set_txpretg_dv_act_start_end(unsigned int dv_act_sta, unsigned int dv_act_end)
{
    ppoverlay_txpretg_txpretg_dv_act_start_end_RBUS txpretg_dv_act_start_end_reg;

    txpretg_dv_act_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_DV_ACT_Start_End_reg);
    txpretg_dv_act_start_end_reg.dv_act_sta = dv_act_sta;
    txpretg_dv_act_start_end_reg.dv_act_end = dv_act_end;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_DV_ACT_Start_End_reg, txpretg_dv_act_start_end_reg.regValue);
}

void Set_txpretg_display_border_color(unsigned int d_bd_r, unsigned int d_bd_g, unsigned int d_bd_b)
{
    ppoverlay_txpretg_txpretg_display_border_color_RBUS txpretg_display_border_color;
    ppoverlay_txpretg_txpretg_display_border_color_2_RBUS txpretg_display_border_color2;

    txpretg_display_border_color.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Border_Color_reg);
    txpretg_display_border_color2.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Border_Color_2_reg);
    txpretg_display_border_color.d_bd_g = d_bd_g;
    txpretg_display_border_color.d_bd_b = d_bd_b;
    txpretg_display_border_color2.d_bd_r = d_bd_r;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Border_Color_reg, txpretg_display_border_color.regValue);
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Border_Color_2_reg, txpretg_display_border_color2.regValue);
}

void Set_txpretg_display_background_color(unsigned int d_bg_r, unsigned int d_bg_g, unsigned int d_bg_b)
{
    ppoverlay_txpretg_txpretg_display_background_color_RBUS txpretg_display_background_color;
    ppoverlay_txpretg_txpretg_display_background_color_2_RBUS txpretg_display_background_color2;

    txpretg_display_background_color.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Background_Color_reg);
    txpretg_display_background_color2.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Display_Background_Color_2_reg);
    txpretg_display_background_color.d_bg_g = d_bg_g;
    txpretg_display_background_color.d_bg_b = d_bg_b;
    txpretg_display_background_color2.d_bg_r = d_bg_r;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Background_Color_reg, txpretg_display_background_color.regValue);
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Display_Background_Color_2_reg, txpretg_display_background_color2.regValue);
}

void Txpretg_D1_double_buffer_en(unsigned int en)
{
    ppoverlay_txpretg_txpretg_double_buffer_ctrl_1_RBUS txpretg_double_buffer_ctrl_1_reg;

    txpretg_double_buffer_ctrl_1_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_1_reg);
    txpretg_double_buffer_ctrl_1_reg.txpretg_db_en = en;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_1_reg, txpretg_double_buffer_ctrl_1_reg.regValue);
}

void Txpretg_D1_double_buffer_read_sel(unsigned int sel)
{
    ppoverlay_txpretg_txpretg_double_buffer_ctrl_1_RBUS txpretg_double_buffer_ctrl_1_reg;

    txpretg_double_buffer_ctrl_1_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_1_reg);
    txpretg_double_buffer_ctrl_1_reg.txpretg_db_read_sel = sel;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_1_reg, txpretg_double_buffer_ctrl_1_reg.regValue);
}

void Txpretg_D1_double_buffer_apply(void)
{
    ppoverlay_txpretg_txpretg_double_buffer_ctrl_1_RBUS txpretg_double_buffer_ctrl_1_reg;

    txpretg_double_buffer_ctrl_1_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_1_reg);
    txpretg_double_buffer_ctrl_1_reg.txpretg_db_apply = 1;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_1_reg, txpretg_double_buffer_ctrl_1_reg.regValue);
}

void Txpretg_D2_double_buffer_en(unsigned int en)
{
    ppoverlay_txpretg_txpretg_double_buffer_ctrl_1_RBUS txpretg_double_buffer_ctrl_1_reg;

    txpretg_double_buffer_ctrl_1_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_1_reg);
    txpretg_double_buffer_ctrl_1_reg.txpretg_i2d_db_en = en;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_1_reg, txpretg_double_buffer_ctrl_1_reg.regValue);
}

void Txpretg_D2_double_buffer_read_sel(unsigned int sel)
{
    ppoverlay_txpretg_txpretg_double_buffer_ctrl_1_RBUS txpretg_double_buffer_ctrl_1_reg;

    txpretg_double_buffer_ctrl_1_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_1_reg);
    txpretg_double_buffer_ctrl_1_reg.txpretg_i2d_db_read_sel = sel;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_1_reg, txpretg_double_buffer_ctrl_1_reg.regValue);
}

void Txpretg_D2_double_buffer_apply(void)
{
    ppoverlay_txpretg_txpretg_double_buffer_ctrl_1_RBUS txpretg_double_buffer_ctrl_1_reg;

    txpretg_double_buffer_ctrl_1_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_1_reg);
    txpretg_double_buffer_ctrl_1_reg.txpretg_i2d_db_apply = 1;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_1_reg, txpretg_double_buffer_ctrl_1_reg.regValue);
}

unsigned int Txpretg_get_double_buffer_apply_status(unsigned short index)
{
    ppoverlay_txpretg_txpretg_double_buffer_ctrl_2_RBUS txpretg_double_buffer_ctrl_2_reg;

    txpretg_double_buffer_ctrl_2_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Double_Buffer_CTRL_2_reg);

    if(index == D1)
        return txpretg_double_buffer_ctrl_2_reg.txpretg_reg_is_db;
    else if(index == D2)
        return txpretg_double_buffer_ctrl_2_reg.txpretg_i2d_reg_is_db;

    return 1;
}

void Txpretg_tracing_control_en(unsigned int en)
{
    ppoverlay_txpretg_txpretg_tracking_control_RBUS txpretg_tracking_ctrl_reg;

    txpretg_tracking_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Tracking_control_reg);
    txpretg_tracking_ctrl_reg.tracking_en = en;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Tracking_control_reg, txpretg_tracking_ctrl_reg.regValue);
}

void Txpretg_set_tracking_cnt_i(unsigned int tracking_cnt_i)
{
    ppoverlay_txpretg_txpretg_tracking_control_RBUS txpretg_tracking_ctrl_reg;

    txpretg_tracking_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_Tracking_control_reg);
    txpretg_tracking_ctrl_reg.tracking_cnt_i = tracking_cnt_i;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_Tracking_control_reg, txpretg_tracking_ctrl_reg.regValue);
}

void Txpretg_set_tracking_cnt_i_number(unsigned int tracking_cnt_i_number)
{
    ppoverlay_txpretg_txpretg_tracking_conter1_frac_RBUS txpretg_tracking_conter1_frac_reg;

    txpretg_tracking_conter1_frac_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_tracking_conter1_frac_reg);
    txpretg_tracking_conter1_frac_reg.tracking_cnt_i_numer = tracking_cnt_i_number;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_tracking_conter1_frac_reg, txpretg_tracking_conter1_frac_reg.regValue);
}

void Txpretg_set_tracking_cnt_i_denom(unsigned int tracking_cnt_i_denom)
{
    ppoverlay_txpretg_txpretg_tracking_conter1_frac_RBUS txpretg_tracking_conter1_frac_reg;

    txpretg_tracking_conter1_frac_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_tracking_conter1_frac_reg);
    txpretg_tracking_conter1_frac_reg.tracking_cnt_i_denom = tracking_cnt_i_denom;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_tracking_conter1_frac_reg, txpretg_tracking_conter1_frac_reg.regValue);
}

void Txpretg_set_tracking_cnt_d(unsigned int tracking_cnt_d)
{
    ppoverlay_txpretg_txpretg_tracking_conter2_RBUS txpretg_tracking_conter2_reg;

    txpretg_tracking_conter2_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_tracking_conter2_reg);
    txpretg_tracking_conter2_reg.tracking_cnt_d = tracking_cnt_d;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_tracking_conter2_reg, txpretg_tracking_conter2_reg.regValue);
}

void Txpretg_set_tracking_Max_multi_systhesisN(unsigned int Max_multi_systhesisN)
{
    ppoverlay_txpretg_txpretg_tracking_systhesisn_ratio_RBUS txpretg_tracking_systhesisn_ratio_reg;

    txpretg_tracking_systhesisn_ratio_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_tracking_systhesisN_ratio_reg);
    txpretg_tracking_systhesisn_ratio_reg.max_multiple_synthesisn = Max_multi_systhesisN;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_tracking_systhesisN_ratio_reg, txpretg_tracking_systhesisn_ratio_reg.regValue);
}

void Txpretg_set_tracking_systhesisN(unsigned int systhesisN)
{
    ppoverlay_txpretg_txpretg_tracking_systhesisn_ratio_RBUS txpretg_tracking_systhesisn_ratio_reg;

    txpretg_tracking_systhesisn_ratio_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_tracking_systhesisN_ratio_reg);
    txpretg_tracking_systhesisn_ratio_reg.synthesisn = systhesisN;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_tracking_systhesisN_ratio_reg, txpretg_tracking_systhesisn_ratio_reg.regValue);
}

void Txpretg_set_tracking_systhesisN_ratio(unsigned int synthesisN_ratio)
{
    ppoverlay_txpretg_txpretg_tracking_systhesisn_ratio_RBUS txpretg_tracking_systhesisn_ratio_reg;

    txpretg_tracking_systhesisn_ratio_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_tracking_systhesisN_ratio_reg);
    txpretg_tracking_systhesisn_ratio_reg.synthesisn_ratio = synthesisN_ratio;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_tracking_systhesisN_ratio_reg, txpretg_tracking_systhesisn_ratio_reg.regValue);
}

void Txpretg_set_tracking_offset_ft_lock_en(unsigned int en)
{
    ppoverlay_txpretg_txpretg_tracking_mode_lock_status_RBUS txpretg_tracking_offset_ft_lock_reg;

    txpretg_tracking_offset_ft_lock_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_tracking_mode_lock_status_reg);
    txpretg_tracking_offset_ft_lock_reg.offset_ft_lock_en = en;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_tracking_mode_lock_status_reg, txpretg_tracking_offset_ft_lock_reg.regValue);
}

void Txpretg_set_tracking_synthesisn_ratio_shift(unsigned int synthesisn_ratio_shift)
{
    ppoverlay_txpretg_txpretg_tracking_mode_lock_status_RBUS txpretg_tracking_offset_ft_lock_reg;

    txpretg_tracking_offset_ft_lock_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_tracking_mode_lock_status_reg);
    txpretg_tracking_offset_ft_lock_reg.synthesisn_ratio_shift = synthesisn_ratio_shift;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_tracking_mode_lock_status_reg, txpretg_tracking_offset_ft_lock_reg.regValue);
}

void Txpretg_set_tracking_lock_th(unsigned int lock_th)
{
    ppoverlay_txpretg_txpretg_tracking_mode_lock_status_RBUS txpretg_tracking_offset_ft_lock_reg;

    txpretg_tracking_offset_ft_lock_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_tracking_mode_lock_status_reg);
    txpretg_tracking_offset_ft_lock_reg.lock_th = lock_th;
    IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_tracking_mode_lock_status_reg, txpretg_tracking_offset_ft_lock_reg.regValue);
}