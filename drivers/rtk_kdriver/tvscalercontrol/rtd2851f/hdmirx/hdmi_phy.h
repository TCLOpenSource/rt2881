#ifndef __HDMI_PHY_H_
#define __HDMI_PHY_H_

//TMDS clock define
#define TMDS_6G		5688
#define TMDS_5G		4740
#define TMDS_4G		3792
#define TMDS_3G		2844
#define TMDS_1p5G	1422

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
	PHY_PROC_RECOVER_LO,
	PHY_PROC_DONE,
} HDMIRX_PHY_PROC_T;

typedef struct {
	unsigned char vth;
	unsigned char tap0;
	unsigned char tap0max;
	unsigned char tap0min;
	signed char tap1;
	signed char le;//Fix Coverity Issue:314901
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

/////////////////////////////////////////////////////
//pure HDMI port
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
	TMDS_OLT_5p94G,
	TMDS_0p25G_ALT2,
	TMDS_0p3375G_ALT2,
	TMDS_0p27G_ALT2,	
	TMDS_CP_5p94G,
	TMDS_VALUE_MAX
}TMDS_VALUE_PARAM_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t CK_CKXTAL_SEL_24_1;
	u_int8_t CK_SEL_CKIN_25_1;
}APHY_Fix_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t CK_SEL_BAND_CAP_2_1;
	u_int8_t CK_POW_CP2_3_1;
	u_int8_t CK_SEL_ICP_4_1;
	u_int8_t CK_SEL_LPF_RES_5_1;
	u_int16_t CK_SEL_M_DIV_6_1;
	u_int8_t B_PI_ISEL_11_1;
	u_int8_t B_PI_ISEL_11_1_16;
	u_int8_t B_PI_ISEL_11_1_32;
	u_int8_t B_PI_SEL_DATARATE_12_1;
	u_int8_t B_PI_SEL_DATARATE_12_1_16;
	u_int8_t B_PI_SEL_DATARATE_12_1_32;
	u_int8_t B_DEMUX_DIV5_10_SEL_13_1;
	u_int8_t B_DEMUX_DIV5_10_SEL_13_1_16;
	u_int8_t B_DEMUX_DIV5_10_SEL_13_1_32;
	u_int8_t B_DEMUX_RATE_SEL_14_1;
	u_int8_t B_DEMUX_RATE_SEL_14_1_16;
	u_int8_t B_DEMUX_RATE_SEL_14_1_32;
	u_int8_t B_DFE_TAP_EN_15_1;
	u_int8_t B_DFE_TAP_EN_15_1_16;
	u_int8_t B_DFE_TAP_EN_15_1_32;
	u_int8_t B_DFE_TAP_EN_16_1;
	u_int8_t B_DFE_TAP_EN_16_1_16;
	u_int8_t B_DFE_TAP_EN_16_1_32;
	u_int8_t B_LE_IHALF_LE_18_1;
	u_int8_t B_LE_IHALF_LE_18_1_16;
	u_int8_t B_LE_IHALF_LE_18_1_32;
	u_int8_t B_LE_ISEL_LE1_21_1;
	u_int8_t B_LE_ISEL_LE1_21_1_16;
	u_int8_t B_LE_ISEL_LE1_21_1_32;
	u_int8_t B_LE_ISEL_LE2_22_1;
	u_int8_t B_LE_ISEL_LE2_22_1_16;
	u_int8_t B_LE_ISEL_LE2_22_1_32;
	u_int8_t B_LE_ISEL_NC_23_1;
	u_int8_t B_LE_ISEL_NC_23_1_16;
	u_int8_t B_LE_ISEL_NC_23_1_32;
	u_int8_t B_LE_ISEL_TAP0_24_1;
	u_int8_t B_LE_ISEL_TAP0_24_1_16;
	u_int8_t B_LE_ISEL_TAP0_24_1_32;
	u_int8_t B_LE_RLSEL_LE2_25_1;
	u_int8_t B_LE_RLSEL_LE2_25_1_16;
	u_int8_t B_LE_RLSEL_LE2_25_1_32;
	u_int8_t B_LE_RLSEL_LE11_26_1;
	u_int8_t B_LE_RLSEL_LE11_26_1_16;
	u_int8_t B_LE_RLSEL_LE11_26_1_32;
	u_int8_t B_LE_RLSEL_LE12_27_1;
	u_int8_t B_LE_RLSEL_LE12_27_1_16;
	u_int8_t B_LE_RLSEL_LE12_27_1_32;
	u_int8_t B_LE_RLSEL_NC1_28_1;
	u_int8_t B_LE_RLSEL_NC1_28_1_16;
	u_int8_t B_LE_RLSEL_NC1_28_1_32;
	u_int8_t B_LE_RLSEL_NC2_29_1;
	u_int8_t B_LE_RLSEL_NC2_29_1_16;
	u_int8_t B_LE_RLSEL_NC2_29_1_32;
	u_int8_t B_LE_RLSEL_TAP0_30_1;
	u_int8_t B_LE_RLSEL_TAP0_30_1_16;
	u_int8_t B_LE_RLSEL_TAP0_30_1_32;
	u_int8_t B_LE_RSSEL_LE2_31_1;
	u_int8_t B_LE_RSSEL_LE2_31_1_16;
	u_int8_t B_LE_RSSEL_LE2_31_1_32;
	u_int8_t B_LE_RSSEL_LE11_32_1;
	u_int8_t B_LE_RSSEL_LE11_32_1_16;
	u_int8_t B_LE_RSSEL_LE11_32_1_32;
	u_int8_t B_LE_RSSEL_LE12_33_1;
	u_int8_t B_LE_RSSEL_LE12_33_1_16;
	u_int8_t B_LE_RSSEL_LE12_33_1_32;
	u_int8_t B_LE_RSSEL_TAP0_34_1;
	u_int8_t B_LE_RSSEL_TAP0_34_1_16;
	u_int8_t B_LE_RSSEL_TAP0_34_1_32;
	u_int8_t B_POW_NC1_LEQ_38_1;
	u_int8_t B_POW_NC1_LEQ_38_1_16;
	u_int8_t B_POW_NC1_LEQ_38_1_32;
}APHY_Param_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t tap1_min_p2_6_1;
	u_int8_t tap0_init_lane0_p2_11_1;
	u_int8_t tap0_init_lane0_p2_11_1_16;
	u_int8_t tap0_init_lane0_p2_11_1_32;
	u_int8_t le_min_lane0_p2_12_1;
	u_int8_t le_min_lane0_p2_12_1_16;
	u_int8_t le_min_lane0_p2_12_1_32;
	u_int8_t le_init_lane0_p2_13_1;
	u_int8_t le_init_lane0_p2_13_1_16;
	u_int8_t le_init_lane0_p2_13_1_32;
	u_int8_t tap1_init_lane0_p2_14_1;
	u_int8_t tap1_init_lane0_p2_14_1_16;
	u_int8_t tap1_init_lane0_p2_14_1_32;
	u_int8_t adapt_mode_p2_20_1;
}DFE_ini_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t KP_3_1;
	u_int8_t KI_4_1;
	u_int8_t RATE_SEL_5_1;
	u_int8_t ST_MODE_6_1;	
}DPHY_Param_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t B_DFE_ADAPT_EN_2_1;
	u_int8_t B_DFE_ADAPT_EN_2_1_16;
	u_int8_t B_DFE_ADAPT_EN_2_1_32;
	u_int8_t timer_ctrl_en_lane0_p2_3_1;
	u_int8_t timer_ctrl_en_lane0_p2_3_1_16;
	u_int8_t timer_ctrl_en_lane0_p2_3_1_32;
	u_int8_t dfe_adapt_en_lane0_p2_4_1;
	u_int8_t dfe_adapt_en_lane0_p2_4_1_16;
	u_int8_t dfe_adapt_en_lane0_p2_4_1_32;
	u_int8_t dfe_adapt_en_lane0_p2_5_1;
	u_int8_t dfe_adapt_en_lane0_p2_5_1_16;
	u_int8_t dfe_adapt_en_lane0_p2_5_1_32;
	u_int8_t dfe_adapt_en_lane0_p2_6_1;
	u_int8_t dfe_adapt_en_lane0_p2_6_1_16;
	u_int8_t dfe_adapt_en_lane0_p2_6_1_32;
	u_int8_t dfe_adapt_en_lane0_p2_7_1;
	u_int8_t dfe_adapt_en_lane0_p2_7_1_16;
	u_int8_t dfe_adapt_en_lane0_p2_7_1_32;
	u_int8_t dfe_adapt_en_lane0_p2_8_1;
	u_int8_t dfe_adapt_en_lane0_p2_8_1_16;
	u_int8_t dfe_adapt_en_lane0_p2_8_1_32;
}LEQ_TAP0_Adapt_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t tap1_min_p2_7_1;
	u_int8_t tap2_min_p2_9_1;
	u_int8_t vth_init_lane0_p2_10_1;
	u_int8_t vth_init_lane0_p2_10_1_16;
	u_int8_t vth_init_lane0_p2_10_1_32;
}Manual_DFE_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t B_DFE_ADAPT_EN_2_1;
	u_int8_t B_DFE_ADAPT_EN_2_1_16;
	u_int8_t B_DFE_ADAPT_EN_2_1_32;
	u_int8_t timer_ctrl_en_lane0_p2_3_1;
	u_int8_t timer_ctrl_en_lane0_p2_3_1_16;
	u_int8_t timer_ctrl_en_lane0_p2_3_1_32;
	u_int8_t dfe_adapt_en_lane0_p2_4_1;
	u_int8_t dfe_adapt_en_lane0_p2_4_1_16;
	u_int8_t dfe_adapt_en_lane0_p2_4_1_32;
	u_int8_t dfe_adapt_en_lane0_p2_5_1;
	u_int8_t dfe_adapt_en_lane0_p2_5_1_16;
	u_int8_t dfe_adapt_en_lane0_p2_5_1_32;
	u_int8_t dfe_adapt_en_lane0_p2_6_1;
	u_int8_t dfe_adapt_en_lane0_p2_6_1_16;
	u_int8_t dfe_adapt_en_lane0_p2_6_1_32;
	u_int8_t dfe_adapt_en_lane0_p2_7_1;
	u_int8_t dfe_adapt_en_lane0_p2_7_1_16;
	u_int8_t dfe_adapt_en_lane0_p2_7_1_32;
	u_int8_t dfe_adapt_en_lane0_p2_8_1;
	u_int8_t dfe_adapt_en_lane0_p2_8_1_16;
	u_int8_t dfe_adapt_en_lane0_p2_8_1_32;
}TAP0_2_Adapt_1_T;

void Mac8p_Pure_HDMI_TMDS_Main_Seq(unsigned char nport, TMDS_VALUE_PARAM_T value_param);

void DPHY_Fix_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void DPHY_Param_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void DPHY_Init_Flow_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void APHY_Fix_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void APHY_Param_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void APHY_Init_Flow_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void DCDR_settings_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void DFE_ini_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void Koffset_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void DCDR_settings_2_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void DCDR_settings_3_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void LEQ_TAP0_Adapt_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void TAP0_2_Adapt_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void Manual_DFE_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void Manual_DFE_Patch_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
#endif

///////////////////////////////////////////////////////

//HDMI_combo_DP_Port//////
#if 1

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t CK_10_42_1;
	u_int8_t ACDR_CK_5_88_1;
	u_int8_t ACDR_B_10_173_1;
	u_int8_t G_15_201_1;
	u_int8_t ACDR_G_10_246_1;
	u_int8_t R_15_274_1;
	u_int8_t ACDR_R_10_319_1;
}APHY_Fix_tmds_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t CK_10_44_1;
	u_int8_t ACDR_CK_5_88_1;
	u_int8_t G_15_201_1;
	u_int8_t R_15_274_1;
}APHY_Fix_tmds_Patch_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t CK_19_12_1;
	u_int8_t CK_19_13_1;
	u_int8_t P0_CK_DEMUX_RSTB_101_1;
}APHY_Init_Flow_tmds_1_T;


typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t CK_18_5_1;
	u_int8_t ACDR_B_5_13_1;
	u_int8_t ACDR_G_5_14_1;
	u_int8_t ACDR_R_5_15_1;
	u_int8_t ACDR_B_6_17_1;
	u_int8_t ACDR_G_6_18_1;
	u_int8_t ACDR_R_6_19_1;
	u_int8_t ACDR_CK_9_20_1;
	u_int8_t ACDR_CK_10_23_1;
	u_int8_t ACDR_CK_11_24_1;
	u_int8_t B_2_25_1;
	u_int8_t B_2_26_1;
	u_int8_t B_2_27_1;
	u_int8_t B_2_28_1;
	u_int8_t B_3_29_1;
	u_int8_t B_3_30_1;
	u_int8_t B_4_31_1;
	u_int8_t B_4_32_1;
	u_int8_t B_4_33_1;
	u_int8_t B_5_34_1;
	u_int8_t B_5_35_1;
	u_int8_t B_6_36_1;
	u_int8_t B_7_38_1;
	u_int8_t B_7_39_1;
	u_int8_t B_7_40_1;
	u_int8_t B_7_42_1;
	u_int8_t B_9_45_1;
	u_int8_t B_14_51_1;
	u_int8_t B_14_53_1;
	u_int8_t B_16_56_1;
	u_int8_t B_16_57_1;
	u_int8_t B_18_58_1;
	u_int8_t B_19_60_1;
	u_int8_t ACDR_B_9_63_1;
	u_int8_t ACDR_B_10_65_1;
	u_int8_t ACDR_B_11_67_1;
	u_int8_t G_2_70_1;
	u_int8_t G_2_71_1;
	u_int8_t G_2_72_1;
	u_int8_t G_2_73_1;
	u_int8_t G_3_74_1;
	u_int8_t G_3_75_1;
	u_int8_t G_4_76_1;
	u_int8_t G_4_77_1;
	u_int8_t G_4_78_1;
	u_int8_t G_5_79_1;
	u_int8_t G_5_80_1;
	u_int8_t G_6_81_1;
	u_int8_t G_7_83_1;
	u_int8_t G_7_84_1;
	u_int8_t G_7_85_1;
	u_int8_t G_7_87_1;
	u_int8_t G_9_90_1;
	u_int8_t G_14_96_1;
	u_int8_t G_14_98_1;
	u_int8_t G_16_101_1;
	u_int8_t G_16_102_1;
	u_int8_t G_18_103_1;
	u_int8_t G_19_105_1;
	u_int8_t ACDR_G_9_108_1;
	u_int8_t ACDR_G_10_110_1;
	u_int8_t ACDR_G_11_112_1;
	u_int8_t R_2_115_1;
	u_int8_t R_2_116_1;
	u_int8_t R_2_117_1;
	u_int8_t R_2_118_1;
	u_int8_t R_3_119_1;
	u_int8_t R_3_120_1;
	u_int8_t R_4_121_1;
	u_int8_t R_4_122_1;
	u_int8_t R_4_123_1;
	u_int8_t R_5_124_1;
	u_int8_t R_5_125_1;
	u_int8_t R_6_126_1;
	u_int8_t R_7_128_1;
	u_int8_t R_7_129_1;
	u_int8_t R_7_130_1;
	u_int8_t R_7_132_1;
	u_int8_t R_9_135_1;
	u_int8_t R_14_141_1;
	u_int8_t R_14_143_1;
	u_int8_t R_16_146_1;
	u_int8_t R_16_147_1;
	u_int8_t R_18_148_1;
	u_int8_t R_19_150_1;
	u_int8_t ACDR_R_9_153_1;
	u_int8_t ACDR_R_10_155_1;
	u_int8_t ACDR_R_11_157_1;
}APHY_Para_tmds_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t LEQ_INIT_B_4_0_62_1;
	u_int8_t TAP0_INIT_B_7_0_63_1;
	u_int8_t LEQ_INIT_G_4_0_84_1;
	u_int8_t TAP0_INIT_G_7_0_85_1;
	u_int8_t LEQ_INIT_R_4_0_106_1;
	u_int8_t TAP0_INIT_R_7_0_107_1;
	u_int8_t LEQ_INIT_CK_4_0_128_1;
	u_int8_t TAP0_INIT_CK_7_0_129_1;
}DFE_ini_tmds_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t b_pi_m_mode_50_1;
	u_int8_t g_pi_m_mode_55_1;
	u_int8_t r_pi_m_mode_60_1;
	u_int8_t ck_pi_m_mode_65_1;
}DPHY_Fix_tmds_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t b_kp_2_1;
	u_int8_t g_kp_4_1;
	u_int8_t r_kp_6_1;
	u_int8_t ck_kp_8_1;
	u_int8_t b_bbw_kp_10_1;
	u_int8_t g_bbw_kp_11_1;
	u_int8_t r_bbw_kp_12_1;
	u_int8_t ck_bbw_kp_13_1;
	u_int8_t b_ki_14_1;
	u_int8_t g_ki_15_1;
	u_int8_t r_ki_16_1;
	u_int8_t ck_ki_17_1;
	u_int8_t b_st_mode_22_1;
	u_int8_t g_st_mode_23_1;
	u_int8_t r_st_mode_24_1;
	u_int8_t ck_st_mode_25_1;
	u_int8_t b_rate_sel_30_1;
	u_int8_t g_rate_sel_31_1;
	u_int8_t r_rate_sel_32_1;
	u_int8_t b_clk_div2_en_33_1;
	u_int8_t g_clk_div2_en_34_1;
	u_int8_t r_clk_div2_en_35_1;
	u_int8_t b_acdr_pll_config_1_40_1;
	u_int8_t b_acdr_pll_config_2_41_1;
	u_int8_t b_acdr_pll_config_3_42_1;
	u_int8_t b_acdr_cdr_config_1_44_1;
	u_int8_t b_acdr_cdr_config_2_45_1;
	u_int8_t b_acdr_cdr_config_3_46_1;
	u_int8_t b_acdr_manual_config_1_48_1;
	u_int8_t b_acdr_manual_config_2_49_1;
	u_int8_t b_acdr_manual_config_3_50_1;
	u_int8_t g_acdr_pll_config_1_52_1;
	u_int8_t g_acdr_pll_config_2_53_1;
	u_int8_t g_acdr_pll_config_3_54_1;
	u_int8_t g_acdr_cdr_config_1_56_1;
	u_int8_t g_acdr_cdr_config_2_57_1;
	u_int8_t g_acdr_cdr_config_3_58_1;
	u_int8_t g_acdr_manual_config_1_60_1;
	u_int8_t g_acdr_manual_config_2_61_1;
	u_int8_t g_acdr_manual_config_3_62_1;
	u_int8_t r_acdr_pll_config_1_64_1;
	u_int8_t r_acdr_pll_config_2_65_1;
	u_int8_t r_acdr_pll_config_3_66_1;
	u_int8_t r_acdr_cdr_config_1_68_1;
	u_int8_t r_acdr_cdr_config_2_69_1;
	u_int8_t r_acdr_cdr_config_3_70_1;
	u_int8_t r_acdr_manual_config_1_72_1;
	u_int8_t r_acdr_manual_config_2_73_1;
	u_int8_t r_acdr_manual_config_3_74_1;
	u_int8_t ck_acdr_pll_config_1_76_1;
	u_int8_t ck_acdr_pll_config_2_77_1;
	u_int8_t ck_acdr_pll_config_3_78_1;
	u_int8_t ck_acdr_cdr_config_1_80_1;
	u_int8_t ck_acdr_cdr_config_2_81_1;
	u_int8_t ck_acdr_cdr_config_3_82_1;
	u_int8_t ck_acdr_manual_config_1_84_1;
	u_int8_t ck_acdr_manual_config_2_85_1;
	u_int8_t ck_acdr_manual_config_3_86_1;
}DPHY_Para_tmds_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t b_offset_rst_n_27_1;
	u_int8_t g_offset_rst_n_28_1;
	u_int8_t r_offset_rst_n_29_1;
	u_int8_t B_7_40_1;
	u_int8_t b_offset_en_de_58_1;
	u_int8_t b_offset_en_do_59_1;
	u_int8_t b_offset_en_ee_60_1;
	u_int8_t b_offset_en_eo_61_1;
	u_int8_t b_offset_en_opo_62_1;
	u_int8_t b_offset_en_ope_63_1;
	u_int8_t G_7_103_1;
	u_int8_t g_offset_en_de_121_1;
	u_int8_t g_offset_en_do_122_1;
	u_int8_t g_offset_en_ee_123_1;
	u_int8_t g_offset_en_eo_124_1;
	u_int8_t g_offset_en_opo_125_1;
	u_int8_t g_offset_en_ope_126_1;
	u_int8_t R_7_166_1;
	u_int8_t r_offset_en_de_184_1;
	u_int8_t r_offset_en_do_185_1;
	u_int8_t r_offset_en_ee_186_1;
	u_int8_t r_offset_en_eo_187_1;
	u_int8_t r_offset_en_opo_188_1;
	u_int8_t r_offset_en_ope_189_1;
	u_int8_t B_7_220_1;
	u_int8_t b_offset_en_eq_227_1;
	u_int8_t G_7_234_1;
	u_int8_t g_offset_en_eq_241_1;
	u_int8_t R_7_248_1;
	u_int8_t r_offset_en_eq_255_1;
	u_int8_t b_offset_en_de_263_1;
	u_int8_t b_offset_en_do_264_1;
	u_int8_t b_offset_en_ee_265_1;
	u_int8_t b_offset_en_eo_266_1;
	u_int8_t b_offset_en_opo_267_1;
	u_int8_t b_offset_en_ope_268_1;
	u_int8_t g_offset_en_de_286_1;
	u_int8_t g_offset_en_do_287_1;
	u_int8_t g_offset_en_ee_288_1;
	u_int8_t g_offset_en_eo_289_1;
	u_int8_t g_offset_en_opo_290_1;
	u_int8_t g_offset_en_ope_291_1;
	u_int8_t r_offset_en_de_309_1;
	u_int8_t r_offset_en_do_310_1;
	u_int8_t r_offset_en_ee_311_1;
	u_int8_t r_offset_en_eo_312_1;
	u_int8_t r_offset_en_opo_313_1;
	u_int8_t r_offset_en_ope_314_1;
	u_int8_t B_13_381_1;
	u_int8_t G_13_383_1;
	u_int8_t R_13_385_1;
}Koffset_tmds_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t EQFE_EN_CK_36_1;
	u_int8_t DFE_ADAPT_EN_CK_6_0_37_1;
	u_int8_t DFE_ADAPT_EN_CK_6_0_38_1;
	u_int8_t DFE_ADAPT_EN_CK_6_0_40_1;
	u_int8_t DFE_ADAPT_EN_CK_6_0_41_1;
	u_int8_t EQFE_EN_B_59_1;
	u_int8_t DFE_ADAPT_EN_B_6_0_60_1;
	u_int8_t DFE_ADAPT_EN_B_6_0_61_1;
	u_int8_t DFE_ADAPT_EN_B_6_0_63_1;
	u_int8_t DFE_ADAPT_EN_B_6_0_64_1;
	u_int8_t EQFE_EN_G_82_1;
	u_int8_t DFE_ADAPT_EN_G_6_0_83_1;
	u_int8_t DFE_ADAPT_EN_G_6_0_84_1;
	u_int8_t DFE_ADAPT_EN_G_6_0_86_1;
	u_int8_t DFE_ADAPT_EN_G_6_0_87_1;
	u_int8_t EQFE_EN_R_105_1;
	u_int8_t DFE_ADAPT_EN_R_6_0_106_1;
	u_int8_t DFE_ADAPT_EN_R_6_0_107_1;
	u_int8_t DFE_ADAPT_EN_R_6_0_109_1;
	u_int8_t DFE_ADAPT_EN_R_6_0_110_1;
}LEQ_VTH_Tap0_3_4_Adapt_tmds_1_T;

typedef struct {
	TMDS_VALUE_PARAM_T value_param;
	u_int8_t DFE_ADAPT_EN_CK_6_0_23_1;
	u_int8_t DFE_ADAPT_EN_CK_6_0_24_1;
	u_int8_t DFE_ADAPT_EN_CK_6_0_25_1;
	u_int8_t DFE_ADAPT_EN_CK_6_0_26_1;
	u_int8_t DFE_ADAPT_EN_CK_6_0_27_1;
	u_int8_t DFE_ADAPT_EN_B_6_0_32_1;
	u_int8_t DFE_ADAPT_EN_B_6_0_33_1;
	u_int8_t DFE_ADAPT_EN_B_6_0_34_1;
	u_int8_t DFE_ADAPT_EN_B_6_0_35_1;
	u_int8_t DFE_ADAPT_EN_B_6_0_36_1;
	u_int8_t DFE_ADAPT_EN_G_6_0_41_1;
	u_int8_t DFE_ADAPT_EN_G_6_0_42_1;
	u_int8_t DFE_ADAPT_EN_G_6_0_43_1;
	u_int8_t DFE_ADAPT_EN_G_6_0_44_1;
	u_int8_t DFE_ADAPT_EN_G_6_0_45_1;
	u_int8_t DFE_ADAPT_EN_R_6_0_50_1;
	u_int8_t DFE_ADAPT_EN_R_6_0_51_1;
	u_int8_t DFE_ADAPT_EN_R_6_0_52_1;
	u_int8_t DFE_ADAPT_EN_R_6_0_53_1;
	u_int8_t DFE_ADAPT_EN_R_6_0_54_1;
}Tap0_to_Tap4_Adapt_tmds_1_T;

void Mac8p_TMDS_setting_Main_Seq(unsigned char nport, TMDS_VALUE_PARAM_T value_param);

void DFE_ini_tmds_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void DPHY_Fix_tmds_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void DPHY_Para_tmds_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void APHY_Fix_tmds_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void APHY_Para_tmds_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void DPHY_Init_Flow_tmds_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void APHY_Init_Flow_tmds_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void DCDR_settings_tmds_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void DCDR_settings_tmds_2_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void Koffset_tmds_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void LEQ_VTH_Tap0_3_4_Adapt_tmds_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void Tap0_to_Tap4_Adapt_tmds_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void DCDR_settings_tmds_3_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void Manual_DFE_tmds_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void APHY_Fix_tmds_Patch_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void APHY_Fix_tmds_Patch_2_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void APHY_Fix_tmds_Patch_3_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
void Manual_DFE_tmds_Patch_1_func(unsigned char nport, TMDS_VALUE_PARAM_T value_param);
#endif
///////////////////////////////////////////////////////////

extern void newbase_rxphy_isr_set(unsigned char port, unsigned char en);
extern void newbase_rxphy_init_struct(unsigned char port);
extern int  newbase_rxphy_isr(void);
extern void newbase_rxphy_isr_enable(char enable);   // use to enable / disable hdmi phy thread

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
extern unsigned char newbase_rxphy_get_frl_mode(unsigned char port);
extern unsigned char newbase_rxphy_get_phy_proc_state(unsigned char port);

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


#ifdef CONFIG_POWER_SAVING_MODE
extern void newbase_hdmi_phy_pw_saving(unsigned char port);
#endif

#if BIST_PHY_SCAN
extern int newbase_get_phy_scan_done(unsigned char nport);
#endif

extern unsigned int lib_hdmi_get_clock(unsigned char nport);
extern void lib_hdmi_phy_init(void);
extern void lib_hdmi_z0_set(unsigned char port, unsigned char lane, unsigned char enable);
extern void lib_hdmi_z300_sel(unsigned char port, unsigned char mode);
extern void lib_hdmi_mac_release(unsigned char nport, unsigned char frl_mode);


extern void newbase_hdmi_dump_dfe_para(unsigned char nport, unsigned char lane_mode);

extern HDMIRX_PHY_STRUCT_T phy_st[HDMI_PORT_TOTAL_NUM];
//self test
#if BIST_PHY_SCAN
void debug_hdmi_phy_scan(unsigned char nport);
#endif
void debug_hdmi_dfe_scan(unsigned char nport,unsigned char lane_mode);
void debug_hdmi_dump_msg(unsigned char nport);
// Add
extern void lib_hdm_set_cdr_kp(unsigned char nport );

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

extern int newbase_hdmi_dfe_fw_le_scan(unsigned char port);
#endif  // __HDMI_PHY_H_

