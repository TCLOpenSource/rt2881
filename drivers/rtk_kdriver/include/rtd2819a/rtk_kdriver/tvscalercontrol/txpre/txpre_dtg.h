#ifndef _TX_PRE_DTGTX_H_
#define _TX_PRE_DTGTX_H_

/*============================================================================
* Copyright (c)      Realtek Semiconductor Corporation, 2024
* All rights reserved.
* ===========================================================================*/

//----------------------------------------------------------------------------
// ID Code      : txpre_dtg.h
// Update Note  :
//----------------------------------------------------------------------------
#define D1 1
#define D2 2


void Set_txpretg_disp_en(unsigned int disp_en);
void Set_txpretg_disp_fsync_en(unsigned int en);
void Set_txpretg_Reg_2field_mode(unsigned int en);
void Set_txpretg_def_field(unsigned int en);
void Set_txpretg_Src_vs_vgip_sel(unsigned int mode);
void Set_txpretg_Src_vs_sel(unsigned int mode);
void Set_txpretg_Vs_neg_pol(unsigned int mode);
void Set_txpretg_first_vsync_gen(unsigned int mode);
void Set_txpretg_fix_last_line(unsigned int en);
void Set_txpretg_dh_width(unsigned int dh_width);
void Set_txpretg_dh_total(unsigned int dh_total);
void Set_txpretg_dv_length(unsigned int dv_length);
void Set_txpretg_dv_total(unsigned int dv_total);
void Set_txpretg_dh_den_start_end(unsigned int dh_den_sta, unsigned int dh_den_end);
void Set_txpretg_dv_den_start_end(unsigned int dv_den_sta, unsigned int dv_den_end);
void Set_txpretg_fs_iv_dv_pixel_delay(unsigned int iv2dv_pixel);
void Set_txpretg_fs_iv_dv_line_delay(unsigned int iv2dv_line);
void Set_txpretg_dh_act_start_end(unsigned int dh_act_sta, unsigned int dh_act_end);
void Set_txpretg_dv_act_start_end(unsigned int dv_act_sta, unsigned int dv_act_end);
void Set_txpretg_display_border_color(unsigned int d_bd_r, unsigned int d_bd_g, unsigned int d_bd_b);
void Set_txpretg_display_background_color(unsigned int d_bd_r, unsigned int d_bd_g, unsigned int d_bd_b);
void Txpretg_D1_double_buffer_en(unsigned int en);
void Txpretg_D1_double_buffer_read_sel(unsigned int sel);
void Txpretg_D1_double_buffer_apply(void);
void Txpretg_D2_double_buffer_en(unsigned int en);
void Txpretg_D2_double_buffer_read_sel(unsigned int sel);
void Txpretg_D2_double_buffer_apply(void);
unsigned int Txpretg_get_double_buffer_apply_status(unsigned short index);
void Txpretg_tracing_control_en(unsigned int en);
void Txpretg_set_tracking_cnt_i(unsigned int tracking_cnt_i);
void Txpretg_set_tracking_cnt_i_number(unsigned int tracking_cnt_i_number);
void Txpretg_set_tracking_cnt_i_denom(unsigned int tracking_cnt_i_denom);
void Txpretg_set_tracking_cnt_d(unsigned int tracking_cnt_d);
void Txpretg_set_tracking_Max_multi_systhesisN(unsigned int Max_multi_systhesisN);
void Txpretg_set_tracking_systhesisN(unsigned int systhesisN);
void Txpretg_set_tracking_systhesisN_ratio(unsigned int synthesisN_ratio);
void Txpretg_set_tracking_offset_ft_lock_en(unsigned int en);
void Txpretg_set_tracking_synthesisn_ratio_shift(unsigned int synthesisn_ratio_shift);
void Txpretg_set_tracking_lock_th(unsigned int lock_th);


#endif
