#ifndef __HDMITX_PHY_TMDS_H_
#define __HDMITX_PHY_TMDS_H_

#define u_int8_t unsigned char
#define u_int16_t unsigned short
typedef enum {
	HDMI20TX_PHY_480P_8bit_270M=0,
	HDMI20TX_PHY_720P_8bit_742M,
	HDMI20TX_PHY_1080P_8bit_1p5G,
	HDMI20TX_PHY_4K30_8bit_3G,
	HDMI20TX_PHY_4K60_8bit_6G,
}HDMI20TX_VALUE_PARAM_T;

typedef struct {
	HDMI20TX_VALUE_PARAM_T value_param;
	u_int8_t reg_ncode_t_ssc_disp_2_1;
	u_int8_t REG_PIXEL_DIV_6_1;
	u_int8_t REG_TMDS_DIV_8_1;
}hdmi20tx_disp_pll_freq_T;

typedef struct {
	HDMI20TX_VALUE_PARAM_T value_param;
	u_int8_t REG_PLLDISP_IP_1_2;
	u_int8_t REG_PLLDISP_CS_2_2;
	u_int8_t REG_PLLDISP_RS_4_2;
}hdmi20tx_disp_pll_setting_T;

typedef struct {
	HDMI20TX_VALUE_PARAM_T value_param;
	u_int8_t reg_clkratio_hdmitx_4p_2p_3_1;
}hdmi20tx_phy_clk_divider_T;

typedef struct {
	HDMI20TX_VALUE_PARAM_T value_param;
	u_int8_t REG_TMDS_IDRVA_12_1;
	u_int8_t REG_TMDS_IDRVB_13_1;
	u_int8_t REG_TMDS_IDRVC_14_1;
	u_int8_t REG_TMDS_IDRVCK_15_1;
	u_int8_t REG_TMDS_IEMA_17_1;
	u_int8_t REG_TMDS_IEMB_18_1;
	u_int8_t REG_TMDS_IEMC_19_1;
	u_int8_t REG_TMDS_IEMCK_20_1;
	u_int8_t REG_TMDS_IPDRV_23_1;
	u_int8_t REG_TMDS_IPDRVCK_24_1;
	u_int8_t REG_TMDS_IPDRVEMCK_26_1;
	u_int8_t REG_TMDS_IPDRVEMPRE_27_1;
	u_int8_t REG_TMDS_IPDRVEMPRECK_28_1;
	u_int8_t REG_TMDS_CSELA_40_1;
	u_int8_t REG_TMDS_CSELB_41_1;
	u_int8_t REG_TMDS_CSELC_42_1;
}hdmi20tx_phy_output_setting_T;

typedef struct {
	HDMI20TX_VALUE_PARAM_T value_param;
	u_int8_t REG_PLL_M1_1_1;
	u_int8_t REG_PLL_M2_2_1;
}hdmi20tx_tmds_pll_freq_T;

typedef struct {
	HDMI20TX_VALUE_PARAM_T value_param;
	u_int8_t REG_PLL_IP_1_2;
	u_int8_t REG_PLL_RS_2_2;
	u_int8_t REG_PLL_CS_3_2;
	u_int8_t REG_P2S_MODE_4_2;
	u_int8_t REG_PLL_VCOSEL_6_2;
}hdmi20tx_tmds_pll_setting_T;

typedef struct {
	HDMI20TX_VALUE_PARAM_T value_param;
	u_int8_t REG_PLL_KVCO_RES_7_3;
	u_int8_t REG_PLL_CKSEL_8_3;
	u_int8_t REG_HDMITX_RESERVE_21_3;
}hdmi20tx_tmds_pll_flow_T;

typedef struct {
	HDMI20TX_VALUE_PARAM_T value_param;
	u_int8_t REG_TMDS_RSELCK_6_3;
}hdmitx_phy_OTP_setting_T;

//void HDMI20TX_Main_Seq(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);

//void Header_0_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
//void HDMITx_Default_PHY_setting_1_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
//void HDMITx_Default_PHY_setting_2_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
//void HDMITx_Default_PHY_setting_3_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
void HDMI20Tx_DISP_PLL_setting_1_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
void HDMI20Tx_DISP_PLL_setting_2_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
void HDMI20Tx_DISP_PLL_setting_3_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
void HDMI20Tx_TMDS_PLL_setting_1_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
void HDMI20Tx_TMDS_PLL_setting_2_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
void HDMI20Tx_TMDS_PLL_setting_3_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
void HDMI20Tx_PHY_Output_setting_1_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
void HDMI20Tx_MAC_Ratio_setting_1_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);

#endif