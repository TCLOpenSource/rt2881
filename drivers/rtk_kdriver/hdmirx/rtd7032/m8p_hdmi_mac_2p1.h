#ifndef __M8P_HDMI_MAC_2P1_H__
#define __M8P_HDMI_MAC_2P1_H__

#include <rbus/M8P_hdmi_top_common_reg.h>

#include "hdmi_mac_2p1.h"

typedef enum {
    HD21_POLARITY_MANUAL_MEAS,
    HD21_POLARITY_AUDIO_MEAS,
} HD21_MEASURE_POLARITY_MEAS_MODE;


#ifdef CONFIG_RTK_KDRV_HDMI_ENABLE_HDMI21
    //------------------------------------------------------------------
    // Measure
    //------------------------------------------------------------------
    extern void lib_hdmi_hd21_meas_clear_interlace_reg(unsigned char nport);
    extern HDMI_INT_PRO_CHECK_RESULT lib_hdmi_hd21_meas_get_interlace(unsigned char nport);
    extern unsigned char lib_hdmi_hd21_meas_get_vs_polarity(unsigned char nport);
    extern unsigned char lib_hdmi_hd21_meas_get_current_polarity(unsigned char nport);
    extern unsigned char lib_hdmi_hd21_meas_get_original_polarity(unsigned char nport);
    extern unsigned char lib_hdmi_hd21_fec_get_vs_polar_invert(unsigned char nport);
    extern void lib_hdmi_hd21_fec_measure_enable(unsigned char nport, unsigned char is_dsc_mode, unsigned char enable);
    extern void lib_hdmi_hd21_fec_measure_restart(unsigned char nport, unsigned char is_dsc_mode);
    extern unsigned char lib_hdmi_hd21_vsync_checked(unsigned char nport);
    extern void lib_hd21_measure(unsigned char port, unsigned char is_dsc_mode, MEASURE_TIMING_T *update_timing);
    extern void lib_hdmi_hd21_clr_measure_last_vs_cnt(unsigned char nport);
    extern HDMI21_MEASURE_RESULT lib_hdmi21_hd21_measure(unsigned char nport, unsigned char is_dsc_mode, MEASURE_TIMING_T *update_timing);
    extern void lib_hdmi_hd21_meas_set_frame_pop_mode(unsigned char nport, unsigned char mode);
    extern unsigned char lib_hdmi_hd21_meas_get_frame_pop_mode(unsigned char nport);



    extern void m8p_lib_hdmi_hd21_clk_div_sel(unsigned char nport, unsigned char pixel_mode);
    extern void m8p_lib_hdmi_hd21_timing_gen_en_vs_align(unsigned char nport, unsigned int delay_ms);
    extern void m8p_lib_hdmi_hd21_crt_on(void);
    extern void m8p_lib_hdmi_hd21_crt_off(void);
    extern void m8p_lib_hdmi_hd21_mac_crt_common_reset(unsigned char nport);
    extern void m8p_lib_hdmi_hd21_mac_crt_perport_on(unsigned char nport);
    extern HD21_MEASURE_POLARITY_MEAS_MODE lib_hdmi_hd21_get_polarity_meas_mode(unsigned char nport);
    extern void m8p_lib_hdmi_hd21_set_polarity_meas_mode(unsigned char nport, HD21_MEASURE_POLARITY_MEAS_MODE mode);
    extern void m8p_lib_hdmi_hd21_vdrop_set(unsigned char nport, unsigned char enable, unsigned int htotal);


#else
    //------------------------------------------------------------------
    // HDMI 2.1 Control
    //------------------------------------------------------------------
    #define lib_hdmi_is_hdmi_21_available(x)         (0)
    #define lib_hdmi_hd21_clk_div_en(nport, enable)
    #define lib_hdmi_hd21_is_clk_div_en(nport)    (0)
    #define lib_hdmi_hd21_timing_gen_en_vs_align(...)
    #define lib_hdmi_hd21_timing_gen_reset_toggle(nport)
    #define lib_hdmi_hd21_get_timing_gen_status(nport)    (0xFFFFFFFF)
    #define lib_hdmi_hd21_meas_clear_interlace_reg(nport)
    #define lib_hdmi_hd21_meas_get_interlace(...)    0
    #define lib_hdmi_hd21_meas_get_vs_polarity(...)    0
    #define lib_hdmi_hd21_meas_get_current_polarity(...)    0
    #define lib_hdmi_hd21_meas_get_original_polarity(...)    0
    #define lib_hdmi_hd21_fec_get_vs_polar_invert(...)    0
    #define lib_hdmi_hd21_fec_measure_enable(...);
    #define lib_hdmi_hd21_fec_measure_restart(...)
    #define lib_hdmi_hd21_clr_fec_frl_vs(port)
    #define lib_hdmi_hd21_mac_crt_common_reset(port)
    #define lib_hdmi_hd21_vsync_checked(port)    0
    #define lib_hdmi_hd21_fec_rs_cnt_keep_enable(...)
    #define lib_hd21_measure(...)
    #define lib_hdmi_hd21_clr_measure_last_vs_cnt(...)
    #define lib_hdmi21_hysteresis_en(enable)
    #define lib_hdmi21_hd21_measure(...)
    #define lib_hdmi_hd21_meas_set_frame_pop_mode(...)
    #define lib_hdmi_hd21_meas_get_frame_pop_mode(port)     0
    #define m8p_lib_hdmi_hd21_vdrop_set(...)

    //------------------------------------------------------------------
    // Error Status
    //------------------------------------------------------------------
    #define lib_hdmi_hd21_fw_disparity_error_reset(nport)
    #define lib_hdmi_hd21_fw_symbol_error_reset(nport)
    #define lib_hdmi_hd21_fw_disparity_symbol_error_enable(nport)
    #define lib_hdmi_hd21_fw_disparity_symbol_error_disable(nport)
    #define lib_hdmi_hd21_fw_disparity_error_status(...)
    #define lib_hdmi_hd21_fw_symbol_error_status(...)
    #define lib_hdmi_hd21_get_rs_err_cnt(nport)    (0)

    //------------------------------------------------------------------
    // High Level API
    //------------------------------------------------------------------
    #define newbase_hdmi_hd21_init_fw_vs_polarity(port)
    #define newbase_hdmi_hd21_check_data_align(...)  (0)
    #define newbase_hdmi_hd21_mac_enable(port)
    #define newbase_hdmi_hd21_mac_disable(port)
    #define newbase_hdmi_hd21_is_data_align_lock(port)   (FALSE)
    #define newbase_hdmi_hd21_get_frl_lane(frl_mode)     (HDMI_3LANE)
    #define newbase_hdmi_hd21_get_frl_rate(frl_mode)    (3)
    #define newbase_hdmi_hd21_link_training_fsm(port)
    #define newbase_hdmi_hd21_lt_fsm_status_init(port)
    #define _hdmi_hd21_frl_mode_str(frl_mode)    "MODE_TMDS"
    #define _hdmi_hd21_frl_lt_fsm_str(fsm)    "HDMI2.1_NOT_SUPPORT"
    #define newbase_hdmi_hd21_clk_gen(...)
    #define newbase_hdmi_hd21_check_rsed_upd_flag(port)
    #define newbase_hdmi_hd21_is_timing_change(...)    0
    #define newbase_hdmi_hd21_get_yuv420_in_sel()  (0)
    #define newbase_hdmi_hd21_stb_dpm_init()  (0)
    extern FRL_LT_FSM_STATUS_T lt_fsm_status[HDMI_PORT_TOTAL_NUM];
    #define newbase_hdmi_hd21_qs_init_fw_polarity(port)

    #define SET_FRL_LT_FSM(port, fsm)       (0)
    #define GET_FRL_LT_FSM(port)            (0)
#endif

#endif  // __HDMI_MAC_2P1_H__

