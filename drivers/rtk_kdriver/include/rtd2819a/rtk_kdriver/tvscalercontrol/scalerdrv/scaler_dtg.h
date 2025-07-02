#ifndef _SCALER_DTG_H_
#define _SCALER_DTG_H_

#define DTG_PRINT_ENABLE

#ifdef DTG_PRINT_ENABLE
#define DTG_PRINT(fmt, args...) \
    do              \
    {               \
        rtd_pr_vbe_notice("[DTG]"fmt, ##args);\
    }while(0)
#else
#define DTG_PRINT(fmt, args...)
#endif
typedef enum dtg_handler
{
    DTG_SET,
    DTG_GET
}DTG_HANDLER;

typedef enum dtg_type
{
    DTG_MASTER,
    DTG_MASTER2,
    DTG_UZU,
    DTG_D2PRE,
    DTG_OSD123,
    DTG_OSD4,
    DTG_OSD5,
    DTG_D2POST,
    DTG_D2POST2,
    DTG_MAX,
}DTG_TYPE;

typedef enum dtg_inherit_type
{
    DTG_INHERIT_UZU,
    DTG_INHERIT_OSD123,
    DTG_INHERIT_OSD4,
    DTG_INHERIT_OSD5,
    DTG_INHERIT_D2POST
}DTG_INHERIT_TYPE;

typedef enum dtg_double_buffer_dx
{
    DOUBLE_BUFFER_D0,
    DOUBLE_BUFFER_D1,
    DOUBLE_BUFFER_D2,
    DOUBLE_BUFFER_D3,
    DOUBLE_BUFFER_D4,
    DOUBLE_BUFFER_D5,
    DOUBLE_BUFFER_D6,
    DOUBLE_BUFFER_D7,
    DOUBLE_BUFFER_D8,
    DOUBLE_BUFFER_D9,
    DOUBLE_BUFFER_D10,
    DOUBLE_BUFFER_D11,
    DOUBLE_BUFFER_D12,
    DOUBLE_BUFFER_D13,
    DOUBLE_BUFFER_D14,
    DOUBLE_BUFFER_D15,
    DOUBLE_BUFFER_D16,
    DOUBLE_BUFFER_D17,
    DOUBLE_BUFFER_D18,
    DOUBLE_BUFFER_D19
}DTG_DOUBLE_BUFFER_DX;

typedef enum DTG_DOUBLE_BUFFER_OPERATOR
{
    DTG_SET_EN,
    DTG_SET_DISABLE,
    DTG_GET_DB,
    DTG_SET_APPLY,
    DTG_GET_APPLY,
    DTG_SET_SEL,
    DTG_GET_SEL,
    DTG_SET_VSYNC_SEL,
    DTG_GET_VSYNC_SEL
}DTG_DOUBLE_BUFFER_OPERATOR;

typedef enum fsync_select
{
    SYNC_MASTER_DTG,
    SYNC_MASTER2_DTG    
}FSYNC_SELECT;

typedef enum d2post_in_sel
{
    /*
    0:OSD123 or D2PRE_BYPASS
    (when osd_split=1, it will be D2PRE bypass)
    1:OSD4
    2:OSD5
    3:OSD4 & OSD5(Dual Display)
    */
    OSD123_OR_BYPASS_D2PRE,
    OSD4,
    OSD5,
    OSD4_AND_OSD5
}D2POST_IN_SEL;

typedef enum osd_split_type
{
    OSD_SPLIT_DISABLE,
    OSD_SPLIT_ENABLE,
}OSD_SPLIT_TYPE;

typedef enum pif_mode
{
    ONE_P_FAST_MODE,
    NORMAL_MODE,
    WI_DSCE,
}PIF_MODE;

typedef struct dtg_timing_info
{
    unsigned int htotal;
    unsigned int vtotal;
    unsigned int hstart;
    unsigned int hend;
    unsigned int vstart;
    unsigned int vend;
    unsigned int htotal_last_line;
    unsigned int dv_length;
    unsigned int dh_width;
    unsigned int free_run_htotal;
    unsigned int free_run_vtotal;
    unsigned int free_run_htotal_last_line;
}DTG_TIMING_INFO;

typedef struct dtg_fraction_info
{
    unsigned char rm;
    unsigned char mul;
    unsigned char remove_half_ivs;
    unsigned char remove_half_ivs_alignmode;
    DTG_INHERIT_TYPE inherit;
}DTG_FRACTION_INFO;

typedef struct dtg_control_info
{
    unsigned int mode_revised;
    unsigned char fsync_select;
    unsigned char frame_cnt_reset_en;
    unsigned char line_cnt_sync;
    unsigned char frc2fsync_by_hw;
    unsigned char fsync_en;
    unsigned char dtg_en;
    unsigned char fix_last_line_new;
    unsigned char fsync_fll_mode;
    unsigned char frc_fsync_status;
    unsigned char wde_to_free_run_dprx;
    unsigned char wde_to_free_run_hdmi;
    unsigned char wde_to_free_run;
    unsigned char frc2fsync_en;
    unsigned char osd_split;
    unsigned char free_vh_total_en;
    unsigned char frc_on_fsync;
    unsigned char shpnr_line_mode_sel;
}DTG_CONTROL_INFO;

typedef struct dtg_speedup_info
{
    unsigned int speedup_dv_total;
    unsigned char frc2fsync_speedup_by_vtotal_en;
}DTG_SPEEDUP_INFO;

typedef struct dtg_disp_ctrl4
{
    unsigned char dual_display_mode;
    unsigned char dual_display_dsce_en;
    unsigned char dual_display_den_combine;
    unsigned char d2pre_to_osd4_in_sel;
    unsigned char d2pre_to_osd_in_sel;
    unsigned char d2post_in_sel;
}DTG_DISP_CTRL4;

typedef struct dtg_stage1_smt_ctrl
{
    unsigned char dispd_smt2_dtg_sel;
    unsigned char dispd_smt2_protect;
    unsigned char dispd_smt1_dtg_sel;
    unsigned char dispd_stage1_smooth_toggle_protect;
}DTG_STAGE1_SMT_CTRL;

typedef struct dtg_stage2_smt_ctrl
{
    unsigned char dispd_smt3_dtg_sel;
    unsigned char dispd_stage2_smooth_toggle_protect;
}DTG_STAGE2_SMT_CTRL;

typedef struct dtg_delay_info
{
    unsigned int dtg_delay_line;
    unsigned int dtg_delay_pixel;
}DTG_DELAY_INFO;

typedef struct uzu_dtg_control3
{
    unsigned int max_line_delay;
    unsigned int line_delay_sw_mode;
    unsigned int align_output_timing;
    unsigned int mode_vs_latch_en;
}UZU_DTG_CONTROL3;

typedef struct time_borrow_info
{
    unsigned int time_borrow_sw_mode;
    unsigned int time_borrow_v;
    unsigned int time_borrow_h;
}TIME_BORROW_INFO;

typedef struct h_v_start_end
{
    unsigned int h_start;
    unsigned int h_end;
    unsigned int v_start;
    unsigned int v_end;
    unsigned int h_width;  //for sub den
    unsigned int v_len;    //for sub den
}H_V_START_END_INFO;

typedef struct iv2dv_delay
{
    unsigned int iv2dv_line;
    unsigned int iv2dv_line_2;
    unsigned int iv2dv_xclk_en;
    unsigned int iv2dv_ih_slf_gen_en;
    unsigned int iv2dv_ih_slf_width;
    unsigned int iv2dv_pixel;
    unsigned int iv2dv_pixel_2;
}IV2DV_DELAY_INFO;

typedef struct abnormal_dvs
{
    unsigned char abnormal_long_en;
    unsigned char abnormal_measure_en;
    unsigned char abnormal_short_en;
    unsigned int short_line_th;
    unsigned int long_line_th;
}ABNORMAL_DVS_INFO;

typedef struct display_control
{
    unsigned char disp2_wd_to_free_run;
    unsigned char wd_to_background;
    unsigned char wd_to_free_run;
    unsigned char hbd_en;
    unsigned char bd_trans;
    unsigned char force_bg;
    unsigned char disp_en;
}DISPLAY_CONTROL_INFO;

typedef struct fix_last_line_systhesis_n_ratio
{
    unsigned char dv_rst_sscg_sel;
    unsigned int max_multiple_synthesisN;
    unsigned int synthesisN;
    unsigned int synthesisN_ratio;
}FIX_LAST_LINE_SYSTHESIS_N_RATIO;

typedef struct fix_last_line_mode_lock_status
{
    unsigned char offset_ft_lock_en;
    unsigned char t_s;
    unsigned int lock_th;
}FIX_LAST_LINE_MODE_LOCK_STATUS;

typedef struct i2d_ctrl_0
{
    unsigned char i2d_special_dly_en;
    unsigned char i2d_special_dly_mod;
    unsigned char i2d_dly_new_mode_en;
    unsigned int i2d_dly_lower;
    unsigned int i2d_dly_n_line_mode;
    unsigned int i2d_dly_upper;
}I2D_CTRL_0;

typedef struct i2d_ctrl_1
{
    unsigned char i2d_sw_mode_en;
    unsigned int i2d_dly_sub_line_max;
    unsigned char i2d_sw_go;
    unsigned char i2d_measure_done;
    unsigned int i2d_dly_add_line_max;
}I2D_CTRL_1;

typedef struct dispd_smooth_toggle1
{
    unsigned char dispd_stage1_clk_mode_revised2;
    unsigned char dispd_stage1_smooth_toggle_src_sel2;
    unsigned char dispd_stage1_smooth_toggle_apply2;
    unsigned char dispd_stage1_clk_mode_revised;
    unsigned char dispd_stage1_smooth_toggle_src_sel;
    unsigned char dispd_stage1_smooth_toggle_apply;
}DISPD_SMOOTH_TOGGLE1;

typedef struct dispd_smooth_toggle2
{
    unsigned char dispd_stage2_clk_mode_revised;
    unsigned char dispd_stage2_smooth_toggle_src_sel;
    unsigned char dispd_stage2_smooth_toggle_apply;
}DISPD_SMOOTH_TOGGLE2;

typedef struct dtg_clk_info
{
    unsigned char clken_disp_stage1;
    unsigned char fract_en;
    unsigned char fract_a;
    unsigned char fract_b;
    unsigned char root_sel;
    unsigned char gate_sel;
    unsigned char clk_sel; //only osd123 osd4 osd5 need set this by app
    unsigned char lout_gate_sel; //d2post
    unsigned char osd_f1p_gate_sel;
}DTG_CLK_INFO;

typedef struct uvc_pll_info
{
    unsigned int f_code;
    unsigned int n_code;
    unsigned int o_code;
    unsigned int pre_div;
}UVC_PLL_INFO;

DTG_APP_TYPE scaler_dtg_get_app_type(void);
void scaler_dtg_set_app_type(DTG_APP_TYPE app_type);
int scaler_dtg_get_control(DTG_TYPE dtgType, DTG_CONTROL_INFO *control);
int scaler_dtg_set_control(DTG_TYPE dtgType, DTG_CONTROL_INFO *control);
int scaler_dtg_get_timing(DTG_TYPE dtgType, DTG_TIMING_INFO *timing);
int scaler_dtg_set_timing(DTG_TYPE dtgType, DTG_TIMING_INFO *timing);
int scaler_dtg_get_fraction(DTG_TYPE dtgType, DTG_FRACTION_INFO *fraction);
int scaler_dtg_set_fraction(DTG_TYPE dtgType, DTG_FRACTION_INFO *fraction);
int scaler_dtg_set_frc2fs_den_target(DTG_TYPE dtgType, unsigned int *den_target);
int scaler_dtg_get_frc2fs_den_target(DTG_TYPE dtgType, unsigned int *den_target);
int scaler_dtg_set_frc2fs_speedup(DTG_TYPE dtgType, DTG_SPEEDUP_INFO *speedup);
int scaler_dtg_get_frc2fs_speedup(DTG_TYPE dtgType, DTG_SPEEDUP_INFO *speedup);
int scaler_dtg_set_disp_ctr4(DTG_DISP_CTRL4 *dtg_disp_ctrl4);
int scaler_dtg_get_disp_ctr4(DTG_DISP_CTRL4 *dtg_disp_ctrl4);
int scaler_dtg_get_input_frame_cnt(DTG_TYPE dtgType, unsigned int *frame_cnt);
int scaler_dtg_get_output_frame_cnt(DTG_TYPE dtgType, unsigned int *frame_cnt);
int scaler_dtg_set_double_buffer_enable(DTG_DOUBLE_BUFFER_DX dx);
int scaler_dtg_set_double_buffer_disable(DTG_DOUBLE_BUFFER_DX dx);
int scaler_dtg_get_double_buffer(DTG_DOUBLE_BUFFER_DX dx, unsigned char *value);
int scaler_dtg_set_double_buffer_apply(DTG_DOUBLE_BUFFER_DX dx);
int scaler_dtg_get_double_buffer_apply(DTG_DOUBLE_BUFFER_DX dx, unsigned char *value);
int scaler_dtg_set_double_buffer_vsync_sel(DTG_DOUBLE_BUFFER_DX dx, unsigned char *value);
int scaler_dtg_get_double_buffer_vsync_sel(DTG_DOUBLE_BUFFER_DX dx, unsigned char *value);
int scaler_dtg_set_stage1_sm_ctrl(DTG_STAGE1_SMT_CTRL *stage1_smt);
int scaler_dtg_get_stage1_sm_ctrl(DTG_STAGE1_SMT_CTRL *stage1_smt);
int scaler_dtg_set_stage2_sm_ctrl(DTG_STAGE2_SMT_CTRL *stage2_smt);
int scaler_dtg_get_stage2_sm_ctrl(DTG_STAGE2_SMT_CTRL *stage2_smt);
int scaler_dtg_get_line_cnt(DTG_TYPE dtgType, unsigned int *line_cnt);
int scaler_dtg_get_line_cnt_pre(DTG_TYPE dtgType, unsigned int *line_cnt);
int scaler_dtg_get_line_cnt_dly(DTG_TYPE dtgType, unsigned int *line_cnt);
int scaler_dtg_set_line_delay(DTG_TYPE dtgType, DTG_DELAY_INFO *dtg_delay_info);
int scaler_dtg_get_line_delay(DTG_TYPE dtgType, DTG_DELAY_INFO *dtg_delay_info);
int scaler_dtg_get_srnn_pre_delay(DTG_DELAY_INFO *dtg_delay_info);
int scaler_dtg_set_srnn_pre_delay(DTG_DELAY_INFO *dtg_delay_info);
int scaler_dtg_get_db_latch_line(unsigned int *db_latch_line);
int scaler_dtg_set_db_latch_line(unsigned int *db_latch_line);
int scaler_dtg_set_uzu_control3(UZU_DTG_CONTROL3 *control3);
int scaler_dtg_get_uzu_control3(UZU_DTG_CONTROL3 *control3);
int scaler_dtg_set_time_borrow(TIME_BORROW_INFO *time_borrow);
int scaler_dtg_get_time_borrow(TIME_BORROW_INFO *time_borrow);
int scaler_dtg_set_main_den_start_end(H_V_START_END_INFO *start_end_info);
int scaler_dtg_get_main_den_start_end(H_V_START_END_INFO *start_end_info);
int scaler_dtg_set_main_background_start_end(H_V_START_END_INFO *start_end_info);
int scaler_dtg_get_main_background_start_end(H_V_START_END_INFO *start_end_info);
int scaler_dtg_set_main_active_start_end(H_V_START_END_INFO *start_end_info);
int scaler_dtg_get_main_active_start_end(H_V_START_END_INFO *start_end_info);
int scaler_dtg_set_sub_den_start_end(H_V_START_END_INFO *start_end_info);
int scaler_dtg_get_sub_den_start_end(H_V_START_END_INFO *start_end_info);
int scaler_dtg_set_sub_background_start_end(H_V_START_END_INFO *start_end_info);
int scaler_dtg_get_sub_background_start_end(H_V_START_END_INFO *start_end_info);
int scaler_dtg_set_sub_active_start_end(H_V_START_END_INFO *start_end_info);
int scaler_dtg_get_sub_active_start_end(H_V_START_END_INFO *start_end_info);
int scaler_dtg_set_iv2dv(DTG_TYPE dtgType, IV2DV_DELAY_INFO *iv2dv_info);
int scaler_dtg_get_iv2dv(DTG_TYPE dtgType, IV2DV_DELAY_INFO *iv2dv_info);
int scaler_dtg_set_main_display_control(DISPLAY_CONTROL_INFO *display_control);
int scaler_dtg_get_main_display_control(DISPLAY_CONTROL_INFO *display_control);
int scaler_dtg_set_sub_display_control(DISPLAY_CONTROL_INFO *display_control);
int scaler_dtg_get_sub_display_control(DISPLAY_CONTROL_INFO *display_control);
int scaler_dtg_set_systhesis_n_ratio(FIX_LAST_LINE_SYSTHESIS_N_RATIO *ratio);
int scaler_dtg_get_systhesis_n_ratio(FIX_LAST_LINE_SYSTHESIS_N_RATIO *ratio);
int scaler_dtg_set_fix_last_line_mode_lock_status(FIX_LAST_LINE_MODE_LOCK_STATUS *status);
int scaler_dtg_get_fix_last_line_mode_lock_status(FIX_LAST_LINE_MODE_LOCK_STATUS *status);
int scaler_dtg_set_i2d_ctrl_0(I2D_CTRL_0 *ctrl_0);
int scaler_dtg_get_i2d_ctrl_0(I2D_CTRL_0 *ctrl_0);
int scaler_dtg_set_i2d_ctrl_1(I2D_CTRL_1 *ctrl_1);
int scaler_dtg_get_i2d_ctrl_1(I2D_CTRL_1 *ctrl_1);
int scaler_dtg_set_dispd_smooth_toggle1(DISPD_SMOOTH_TOGGLE1 *toggle1);
int scaler_dtg_get_dispd_smooth_toggle1(DISPD_SMOOTH_TOGGLE1 *toggle1);
int scaler_dtg_set_dispd_smooth_toggle2(DISPD_SMOOTH_TOGGLE2 *toggle2);
int scaler_dtg_get_dispd_smooth_toggle2(DISPD_SMOOTH_TOGGLE2 *toggle2);
int scaler_dtg_set_clk(DTG_TYPE dtg_type, DTG_CLK_INFO *dtg_clk_info);
int scaler_dtg_get_clk(DTG_TYPE dtg_type, DTG_CLK_INFO *dtg_clk_info);
int scaler_dtg_set_abonormal_dvs(DTG_TYPE dtgType, ABNORMAL_DVS_INFO *abnormal_info);
int scaler_dtg_get_abonormal_dvs(DTG_TYPE dtgType, ABNORMAL_DVS_INFO *abnormal_info);
int scaler_dtg_set_dpll_fsync_tracking_enable(void);
int scaler_dtg_set_dpll_fsync_tracking_disable(void);
int scaler_dtg_set_dpll_sys_dclkss_enable(void);
int scaler_dtg_set_dpll_sys_dclkss_disable(void);
int scaler_dtg_clear_abnormal_dvs_status(DTG_TYPE type);
int scaler_dtg_get_abnormal_dvs_short_flag(DTG_TYPE dtg_type);
int scaler_dtg_get_abnormal_dvs_long_flag(DTG_TYPE dtg_type);
unsigned int scaler_dtg_get_time_for_90K(void);
int scaler_dtg_set_frc2fsync_interrupt(DTG_TYPE type, unsigned char enable);
int scaler_dtg_set_fsync_ch_sel(DTG_TYPE dtg_type, unsigned char fsync_ch_sel);
int scaler_dtg_set_dsce_region_pixel(DTG_TYPE dtg_type, unsigned int *pixel);
int scaler_dtg_get_dsce_region_pixel(DTG_TYPE dtg_type, unsigned int *pixel);
int scaler_dtg_set_dsce_region_dh_start_end(DTG_TYPE dtg_type, unsigned int *dh_start, unsigned int *dh_end);
int scaler_dtg_get_dsce_region_dh_start_end(DTG_TYPE dtg_type, unsigned int *dh_start, unsigned int *dh_end);
int scaler_dtg_set_double_buffer_read_sel(DTG_DOUBLE_BUFFER_DX dx, unsigned char *value);
int scaler_dtg_get_uvc_pll_info(UVC_PLL_INFO *info);
unsigned char *scaler_dtg_get_dtg_type_str(DTG_TYPE type);
int scaler_dtg_set_all_double_buffer_disable(void);
#endif
