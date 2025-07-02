#ifndef __HDMI_PHY_H_
#define __HDMI_PHY_H_

//TMDS clock define
//#define TMDS_6G		5688
//#define TMDS_5G		4740
//#define TMDS_4G		3792
//#define TMDS_3G		2844
//#define TMDS_1p5G	1422


#define HDMI20_PHY_ERROR_CORRECTION_EN	0
#define HDMI_HYSTERESIS_EN		 0 

//////////////////////////
//From Golden Excel
#if 1
typedef enum {
	TMDS_5p94G=0,
	TMDS_2p97G,
	TMDS_1p485G,
	TMDS_0p742G,
	TMDS_0p25G,
	TMDS_4p445G,
	TMDS_3p7125G,
	TMDS_2p2275G,
	TMDS_1p85625G,
	TMDS_1p11375G,
	TMDS_0p928125G,
	TMDS_0p54G,
	TMDS_0p405G,
	TMDS_0p3375G,
	TMDS_0p27G,
	TMDS_0p25G_ALT2,
	TMDS_0p3375G_ALT2,
	TMDS_0p27G_ALT2,
	OLT,
}TMDS_VALUE_PARAM_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int32_t P0_B1_2_1;
	u_int32_t P0_G1_3_1;
	u_int32_t P0_R1_4_1;
}KOFFSET_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int32_t P0_B1_2_1;
	u_int32_t P0_G1_3_1;
	u_int32_t P0_R1_4_1;
}KOFFSET_Patch_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int32_t P0_CK1_10_1;
	u_int8_t CK_3_13_1;
	u_int8_t CK_2_14_1;
	u_int32_t P0_CK2_16_1;
	u_int8_t CK_8_18_1;
	u_int8_t CK_6_20_1;
	u_int8_t CK_5_21_1;
	u_int32_t P0_B1_29_1;
	u_int8_t B_4_31_1;
	u_int8_t B_2_33_1;
	u_int32_t P0_B2_35_1;
	u_int8_t B_8_37_1;
	u_int8_t B_7_38_1;
	u_int8_t B_6_39_1;
	u_int8_t B_5_40_1;
	u_int32_t P0_B3_41_1;
	u_int8_t B_12_43_1;
	u_int32_t P0_G1_48_1;
	u_int8_t G_4_50_1;
	u_int8_t G_2_52_1;
	u_int32_t P0_G2_54_1;
	u_int8_t G_8_56_1;
	u_int8_t G_7_57_1;
	u_int8_t G_6_58_1;
	u_int8_t G_5_59_1;
	u_int32_t P0_G3_60_1;
	u_int8_t G_12_62_1;
	u_int32_t P0_R1_67_1;
	u_int8_t R_4_69_1;
	u_int8_t R_2_71_1;
	u_int32_t P0_R2_73_1;
	u_int8_t R_8_75_1;
	u_int8_t R_7_76_1;
	u_int8_t R_6_77_1;
	u_int8_t R_5_78_1;
	u_int32_t P0_R3_79_1;
	u_int8_t R_12_81_1;
	u_int16_t n_code_87_1;
	u_int32_t REG_DFE_MODE_P2_98_1;
	u_int32_t REG_DFE_EN_L2_P2_99_1;
	u_int32_t REG_DFE_EN_L0_P2_100_1;
	u_int32_t REG_DFE_EN_L1_P2_101_1;
	u_int32_t REG_DFE_INIT0_L2_P2_102_1;
	u_int32_t REG_DFE_INIT0_L0_P2_103_1;
	u_int32_t REG_DFE_INIT0_L1_P2_104_1;
	u_int32_t REG_DFE_MODE_P2_111_1;
	u_int32_t REG_DFE_LIMIT0_P2_114_1;
	u_int32_t REG_DFE_EN_L2_P2_117_1;
	u_int32_t REG_DFE_EN_L0_P2_118_1;
	u_int32_t REG_DFE_EN_L1_P2_119_1;
	u_int8_t adapt_mode_p2_120_1;
	u_int8_t tap1_init_lane0_p2_129_1;
	u_int8_t tap1_init_lane1_p2_130_1;
	u_int8_t tap1_init_lane2_p2_131_1;
	u_int8_t KP_151_1;
	u_int8_t KI_152_1;
	u_int8_t RATE_SEL_153_1;
}START_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int32_t REG_DFE_EN_L2_P2_2_2;
	u_int32_t REG_DFE_EN_L0_P2_3_2;
	u_int32_t REG_DFE_EN_L1_P2_4_2;
	u_int32_t REG_DFE_EN_L2_P2_6_2;
	u_int32_t REG_DFE_EN_L0_P2_7_2;
	u_int32_t REG_DFE_EN_L1_P2_8_2;
}ADAPTATION1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int32_t REG_DFE_EN_L2_P2_2_3;
	u_int32_t REG_DFE_EN_L0_P2_3_3;
	u_int32_t REG_DFE_EN_L1_P2_4_3;
	u_int32_t REG_DFE_EN_L2_P2_5_3;
	u_int32_t REG_DFE_EN_L0_P2_6_3;
	u_int32_t REG_DFE_EN_L1_P2_7_3;
}ADAPTATION2_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t KP_5_4;
	u_int8_t KI_6_4;
	u_int8_t RATE_SEL_7_4;
}DCDR_SET_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int32_t P0_CK1_10_1;
	u_int8_t CK_3_13_1;
	u_int8_t CK_2_14_1;
	u_int32_t P0_CK2_16_1;
	u_int8_t CK_8_18_1;
	u_int8_t CK_5_21_1;
	u_int32_t P0_B1_29_1;
	u_int8_t B_4_31_1;
	u_int8_t B_2_33_1;
	u_int32_t P0_B2_35_1;
	u_int8_t B_8_37_1;
	u_int8_t B_7_38_1;
	u_int8_t B_6_39_1;
	u_int8_t B_5_40_1;
	u_int32_t P0_B3_41_1;
	u_int8_t B_12_43_1;
	u_int8_t B_10_45_1;
	u_int32_t P0_G1_48_1;
	u_int8_t G_4_50_1;
	u_int8_t G_2_52_1;
	u_int32_t P0_G2_54_1;
	u_int8_t G_8_56_1;
	u_int8_t G_7_57_1;
	u_int8_t G_6_58_1;
	u_int8_t G_5_59_1;
	u_int32_t P0_G3_60_1;
	u_int8_t G_12_62_1;
	u_int8_t G_10_64_1;
	u_int32_t P0_R1_67_1;
	u_int8_t R_4_69_1;
	u_int8_t R_2_71_1;
	u_int32_t P0_R2_73_1;
	u_int8_t R_8_75_1;
	u_int8_t R_7_76_1;
	u_int8_t R_6_77_1;
	u_int8_t R_5_78_1;
	u_int32_t P0_R3_79_1;
	u_int8_t R_12_81_1;
	u_int8_t R_10_83_1;
	u_int16_t n_code_87_1;
	u_int32_t REG_DFE_INIT0_L2_P2_102_1;
	u_int32_t REG_DFE_INIT0_L0_P2_103_1;
	u_int32_t REG_DFE_INIT0_L1_P2_104_1;
	u_int32_t REG_DFE_LIMIT0_P2_114_1;
	u_int8_t tap0_init_lane0_p2_126_1;
	u_int8_t tap0_init_lane1_p2_127_1;
	u_int8_t tap0_init_lane2_p2_128_1;
	u_int8_t tap1_init_lane0_p2_129_1;
	u_int8_t tap1_init_lane1_p2_130_1;
	u_int8_t tap1_init_lane2_p2_131_1;
	u_int8_t KP_151_1;
	u_int8_t KI_152_1;
	u_int8_t RATE_SEL_153_1;
}START_Patch_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t KP_5_4;
	u_int8_t KI_6_4;
	u_int8_t RATE_SEL_7_4;
}DCDR_SET_Patch_T;

void TMDS_Main_Seq(unsigned char nport, TMDS_VALUE_PARAM_T value_param);

void ORIGINAL_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void KOFFSET_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void ORIGINAL_2_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void ORIGINAL_3_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void ORIGINAL_4_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void ORIGINAL_Patch_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void KOFFSET_Patch_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void ORIGINAL_Patch_4_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
#endif


////////////////////////

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
	PHY_PROC_RECOVER_LO,
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

extern unsigned char newbase_hdmi_scdc_get_frl_mode(unsigned char port);
extern void newbase_rxphy_set_frl_mode(unsigned char port, unsigned char frl_mode);
extern unsigned char newbase_rxphy_get_frl_mode(unsigned char port);



extern void newbase_rxphy_init_struct(unsigned char port);
extern int  newbase_rxphy_isr(void);

extern void newbase_rxphy_force_clear_clk_st(unsigned char port);
extern unsigned int newbase_rxphy_get_clk(unsigned char port);
extern unsigned int newbase_rxphy_get_clk_pre(unsigned char port);
extern int newbase_hdmi_get_frl_clock(unsigned char port, unsigned char frl_mode);
extern unsigned char newbase_rxphy_is_clk_stable(unsigned char port);
extern void newbase_rxphy_set_apply_clock(unsigned char port,unsigned int b);
extern unsigned int newbase_rxphy_get_apply_clock(unsigned char port);
extern unsigned char newbase_rxphy_get_setphy_done(unsigned char port);
extern unsigned char newbase_rxphy_is_tmds_mode(unsigned char port);
extern void newbase_rxphy_reset_setphy_proc(unsigned char port);
extern void newbase_hdmi_dfe_recovery_6g_long_cable(unsigned char port);
extern void newbase_hdmi_dfe_recovery_midband_long_cable(unsigned char port);
extern void newbase_hdmi_dfe_recovery_mi_speed(unsigned char port);
extern void newbase_hdmi_dfe_recovery_hi_speed(unsigned char port);
extern void newbase_hdmi_dfe_recovery_lo_speed(unsigned char port);



extern void newbase_hdmi_open_err_detect(unsigned char port);
extern void newbase_hdmi_reset_thr_cnt(unsigned char port);
extern unsigned char newbase_hdmi_err_detect_stop(unsigned char port);
extern unsigned char newbase_hdmi_get_err_recovery(unsigned char port);
extern void newbase_hdmi_err_detect_add(unsigned char port);

extern unsigned char newbase_hdmi_get_longcable_flag(unsigned char port);
extern void newbase_hdmi_set_longcable_flag(unsigned char port, unsigned char enable);
extern unsigned char newbase_hdmi_get_badcable_flag(unsigned char port);
extern void newbase_hdmi_set_badcable_flag(unsigned char port, unsigned char enable);
extern unsigned char newbase_hdmi_detect_6g_longcable(unsigned char port,unsigned int r_err,unsigned int g_err,unsigned int b_err);
extern unsigned char newbase_hdmi_char_err_detection(unsigned char port,unsigned int *r_err, unsigned int *g_err, unsigned int *b_err);
extern unsigned char newbase_hdmi_bit_err_detection(unsigned char port, unsigned int *r_err, unsigned int *g_err, unsigned int *b_err);
extern unsigned char newbase_hdmi_set_6G_long_cable_enable(unsigned char port, unsigned char enable);

extern void newbase_hdmi_ced_error_cnt_reset(unsigned char port);
extern void newbase_hdmi_ced_error_cnt_accumulated(unsigned char port, unsigned int *b_err, unsigned int *g_err, unsigned int *r_err);
extern unsigned short newbase_hdmi_get_ced_error_cnt(unsigned char port , unsigned char ch);
extern unsigned char newbase_hdmi_is_bit_error_occured(unsigned char port);

extern void newbase_hdmi_set_eq_mode(unsigned char nport, unsigned char mode);
extern void newbase_hdmi_manual_eq(unsigned char nport, unsigned char eq_ch0, unsigned char eq_ch1, unsigned char eq_ch2);
extern  void newbase_hdmi_manual_eq_ch(unsigned char nport,unsigned char ch,unsigned char eq_ch);
extern HDMIRX_PHY_STRUCT_T* newbase_rxphy_get_status(unsigned char port);
extern void newbase_hdmi_rxphy_handler(unsigned char port);
extern void newbase_set_CLK_Wake_Up(unsigned char value);
extern unsigned char newbase_get_CLK_Wake_Up(void);

// for Factory mode control : sometimes we need to run special EQ when working on factory mode because there is some special device
extern void newbase_hdmi_set_factory_or_power_only_mode_enable(unsigned char enable);
extern unsigned char newbase_hdmi_get_factory_or_power_only_mode_enable(void);

#ifdef CONFIG_POWER_SAVING_MODE
extern void newbase_hdmi_phy_pw_saving(unsigned char port);
#endif

#if BIST_PHY_SCAN
extern int newbase_get_phy_scan_done(unsigned char nport);
#endif

extern unsigned int lib_hdmi_get_clock(unsigned char nport);
extern void lib_hdmi_stb_tmds_clk_wakeup_en(unsigned char en);
extern void lib_hdmi_phy_init(void);
extern void lib_hdmi_z0_set(unsigned char port, unsigned char lane, unsigned char enable, unsigned char CLK_Wake_Up);
extern void lib_hdmi_z300_sel(unsigned char port, unsigned char mode);
extern void lib_hdmi_ck_md_ref_sel(unsigned char port);
extern void lib_hdmi_mac_release(unsigned char nport, unsigned char frl_mode);

//self test
#if BIST_PHY_SCAN
void debug_hdmi_phy_scan(unsigned char nport);
#endif
void debug_hdmi_dfe_scan(unsigned char nport,unsigned char lane_mode);
void debug_hdmi_dump_msg(unsigned char nport);
// Add


#define LN_CK 0x08
#define LN_B   0x01
#define LN_G   0x02
#define LN_R   0x04
#define LN_ALL (LN_CK|LN_B|LN_G|LN_R)

#define MAX_LE                  24
#define IS_VALID_LE(x)          (x>=0 && x <= MAX_LE)


#define DFE_INFO(fmt, args...)              rtd_pr_hdmi_info("[DFE][INFO]"fmt, ##args)
#define DFE_WARN(fmt, args...)              rtd_pr_hdmi_warn("[DFE][WARN]"fmt, ##args)

//------------------------------------------------------------------
// HPD/DET
//------------------------------------------------------------------
extern void lib_hdmi_set_hdmi_hpd(unsigned char hpd_idx, unsigned char en);
extern unsigned char lib_hdmi_get_hdmi_hpd(unsigned char hpd_idx);
extern unsigned char lib_hdmi_get_hdmi_5v_det(unsigned char det_idx);
extern void newbase_set_qs_rxphy_status(unsigned char port);

//error correction
//extern void newbase_hdmi_init_disparity_ltp_var(unsigned char nport);
//extern void lib_hdmi20_phy_error_handling(unsigned char nport);
extern int newbase_hdmi_dfe_fw_le_scan(unsigned char port);
extern void lib_ap_set_hdmi_ext_trace_z0(unsigned int z0);
#endif  // __HDMI_PHY_H_

