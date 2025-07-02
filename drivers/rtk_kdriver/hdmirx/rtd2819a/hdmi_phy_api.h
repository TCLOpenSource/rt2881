#ifndef __HDMI_PHY_API_H__
#define __HDMI_PHY_API_H__

#ifdef CONFIG_RTK_KDRV_HDMI_ENABLE_PHY
//For external use
#include "hdmi_phy.h"
#include "hdmi_phy_2p1.h"
#else
//For bring-up use
typedef enum {
	PHY_PROC_INIT,
	PHY_PROC_RECORD,
	PHY_PROC_CLOSE_ADP,
	PHY_PROC_FW_LE_SCAN,

	PHY_PROC_RECOVER_6G_LONGCABLE,
	PHY_PROC_RECOVER_6G_LONGCABLE_RECORD,
	PHY_PROC_RECOVER_6G_LONGCABLE_CLOSE_ADP,

	PHY_PROC_RECOVER_MI,
	PHY_PROC_RECOVER_HI,
	PHY_PROC_RECOVER_MID_BAND_LONGCABLE,

	PHY_PROC_DONE,
} HDMIRX_PHY_PROC_T;

typedef struct {
	unsigned char vth;
	unsigned char tap0;
	unsigned char tap0max;
	unsigned char tap0min;
	signed char tap1;
	signed char le;//Fix Coverity Issue:314901
	signed char le_add_tap1;
	unsigned char lemax;
	unsigned char lemin;
	signed char tap2;
	signed char tap3;
	signed char tap4;
}HDMIRX_PHY_DFE_T;

typedef struct {
	unsigned int clk;
	unsigned int clk_pre;
	unsigned int clk_debounce_count;
	unsigned int clk_unstable_count;
	unsigned int noclk_debounce_count;
	unsigned int clk_tolerance;
	unsigned char clk_stable;
	unsigned int apply_clk;
	unsigned char rxphy_40x;
	unsigned char lane_num;
	unsigned char frl_mode;  //0: TMDS mode 1: FRL mode
	unsigned char pre_frl_mode;
	unsigned char do_koffset;
	HDMIRX_PHY_PROC_T phy_proc_state;

	unsigned int char_err_loop;
	unsigned int char_err_occurred;
	unsigned char detect_6g_longcable_enable;
	unsigned char longcable_flag;
	unsigned char badcable_flag;	
	unsigned char le_re_adaptive;
	unsigned int bit_err_loop;
	unsigned int bit_err_occurred;
	unsigned int dfe_thr_chk_cnt;
	unsigned int ced_thr_chk_cnt;	
	unsigned int error_detect_count;
	unsigned char recovery;
	HDMIRX_PHY_DFE_T dfe_t[4];
	unsigned short char_error_cnt[4];// character error detection value(accumulated)

	//hdmi2.1
	unsigned char ltp_state;  //0: ltp state 1: video state
	//disp err
	unsigned char disp_start;
	unsigned int disp_max_cnt;
	//unsigned int disp_err_pre[4];
	//unsigned int disp_err_after[4];
	unsigned int disp_err0[4];
	unsigned int disp_err1[4];
	unsigned int disp_err2[4];

	unsigned int ced_err0[4];
	unsigned int ced_err1[4];
	unsigned int ced_err2[4];
	
	unsigned int disp_BER1[4];
	unsigned int disp_BER2[4];
	unsigned int disp_BER2_temp[4];	
	unsigned char disp_adp_stage[4];
	
	//unsigned int disp_err_diff1[4];
	//unsigned int disp_err_diff2[4];
	unsigned int disp_err_zero_cnt[4];
	unsigned int disp_BER_zero_cnt[4];

	//unsigned short disp_err_t[4];
	//unsigned short err_occur_t1[4];
	//unsigned short err_occur_t2[4];
	//unsigned short err_occur_t3[4];
	unsigned int disp_timer_cnt[4];

	//unsigned char wrong_direction_cnt[4];
	unsigned char adp_stage[4];
	int tap1_adp_step[4];
	int tap2_adp_step[4];	
	int le_adp_step[4];
	unsigned int ltp_err_diff1[4];
	unsigned int ltp_err_diff2[4];

#if PHY_ADAPTIVE_BY_LTP_PAT_AFTER_LTP_PASS
	//ltp err
	unsigned char ltp_err_start[4];
	unsigned char ltp_err_end[4];
	unsigned int ltp_err_pre[4];
	unsigned int ltp_err_zero_cnt[4];
#endif


} HDMIRX_PHY_STRUCT_T;

#define  PHY_PROC_DONE    0
#define  HDMI_14    0
#define  HDMI_20    0
#define  HDMI_21    0

#define LN_CK 0x08
#define LN_B   0x01
#define LN_G   0x02
#define LN_R   0x04
#define LN_ALL (LN_CK|LN_B|LN_G|LN_R)

#define lib_hdmi_phy_init()
#define newbase_hdmi_ced_error_cnt_reset(port)
#define newbase_rxphy_init_struct(port)
#define newbase_rxphy_is_tmds_mode(port)    0
#define newbase_hdmi_set_6G_long_cable_enable(port, enable)
#define newbase_rxphy_get_phy_proc_state(port)    0
#define newbase_rxphy_is_clk_stable(port)	 0
#define newbase_rxphy_get_clk(port) 	0
#define newbase_rxphy_get_setphy_done(port)    0
#define newbase_hdmi_open_err_detect(port)    0
#define newbase_hdmi_reset_thr_cnt(port)    0
#define newbase_rxphy_reset_setphy_proc(port)
#define newbase_hdmi_err_detect_stop(port)    0
#define newbase_hdmi_get_longcable_flag(port)    0
#define newbase_hdmi_get_err_recovery(port)    0
#define newbase_hdmi_dfe_recovery_hi_speed(port)
#define newbase_hdmi_is_bit_error_occured(port)    0
#define newbase_hdmi_dfe_recovery_mi_speed(port)
#define newbase_hdmi_dfe_recovery_midband_long_cable(port)
#define newbase_get_phy_scan_done(port)    0
#define newbase_rxphy_get_clk_pre(port)    0
#define lib_hdmi_z0_set(port, lane, enable, CLK_Wake_Up)
#define newbase_hdmi_set_longcable_flag(port, enable)
#define newbase_hdmi_phy_pw_saving(port)

//#define lib_hdmi_phy_enter_power_saving_mode(nport, mode,br_swap)
//#define lib_hdmi_phy_exit_power_saving_mode(nport, mode,br_swap)

#define newbase_hdmi_bit_err_detection(port,r_err,g_err,b_err)    0
#define newbase_hdmi_char_err_detection(port,r_err,g_err,b_err)

//#define lib_hdmi_phy_reset_power_saving_state(port)
//#define lib_hdmi_phy_dump_pow_saving_reg(port)

#define newbase_set_qs_rxphy_status(port)
#define newbase_rxphy_force_clear_clk_st(port)
#define newbase_hdmi_set_eq_mode(nport, mode)
#define newbase_hdmi_manual_eq(nport,eq_ch0,eq_ch1,eq_ch2)
#define newbase_hdmi_manual_eq_ch(nport,ch,eq_ch)
#define newbase_hdmi_get_ced_error_cnt(port,ch)    0

#define newbase_hdmi_dfe_recovery_6g_long_cable(port)

#define newbase_rxphy_get_frl_mode(port)    port

#define newbase_hdmi_rxphy_handler(port)

#define newbase_rxphy_get_status(port)      0
#define newbase_hdmi_get_factory_or_power_only_mode_enable()        0
#define newbase_hdmi_set_factory_or_power_only_mode_enable(enable)   
#define IS_VALID_LE(x)          0
#define newbase_hdmi21_phy_pw_saving(port)
#define HDMI_HYSTERESIS_EN  0
#define lib_hdmi_get_hdmi_hpd(hpd_idx)      0
#define lib_hdmi_set_hdmi_hpd(hdp_idx, value)
#define lib_hdmi_get_hdmi_5v_det(det_idx)   0
#define newbase_hdmi_get_badcable_flag(port)    0
#define lib_hdmi21_stb_tmds_clk_wakeup_en(port,en)
#define newbase_set_CLK_Wake_Up(value)
#define newbase_get_CLK_Wake_Up()   0
#define lib_hdmi21_phy_init()
#define newbase_rxphy_set_phy_for_aksv(port)

#define newbase_hdmi_scdc_get_frl_mode(...)  0
#define newbase_rxphy_set_frl_mode(...)
#define newbase_hdmi_frl_set_phy(...)
#define newbase_hdmi_dfe_recovery_lo_speed(...)
#define lib_hdmi21_hysteresis_en(...)
#define lib_hdmi_hysteresis_en(...)
#define newbase_hdmi21_rxphy_set_frl_info(...)
#endif

#endif  //__HDMI_PHY_API_H__

