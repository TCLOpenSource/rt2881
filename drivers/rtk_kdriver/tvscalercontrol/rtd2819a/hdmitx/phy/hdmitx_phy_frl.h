#ifndef __HDMITX_PHY_FRL_H_
#define __HDMITX_PHY_FRL_H_

#define u_int8_t unsigned char
#define u_int16_t unsigned short
typedef enum {
	HDMI21TX_PHY_2K60_3G3L=0,
	HDMI21TX_PHY_4K60_6G,
	HDMI21TX_PHY_4K60_8G,
	HDMI21TX_PHY_4K60_10G,
	HDMI21TX_PHY_4K60_12G,
}HDMI21TX_VALUE_PARAM_T;

typedef struct {
	HDMI21TX_VALUE_PARAM_T value_param;
	u_int8_t REG_PIXEL_DIV_6_1;
	u_int8_t REG_TMDS_DIV_8_1;
}hdmi21tx_disp_pll_freq_T;

typedef struct {
	HDMI21TX_VALUE_PARAM_T value_param;
	u_int8_t REG_PLLDISP_IP_1_2;
	u_int8_t REG_PLLDISP_RS_4_2;
}hdmi21tx_disp_pll_setting_T;

typedef struct {
	HDMI21TX_VALUE_PARAM_T value_param;
	u_int8_t REG_PLLDISP_VCORSTB_2_3;
}hdmi21tx_disp_pll_flow_T;

typedef struct {
	HDMI21TX_VALUE_PARAM_T value_param;
	u_int8_t reg_clkratio_hdmitx_ds444_2p_5_1;
}hdmi21tx_phy_clk_divider_T;

typedef struct {
	HDMI21TX_VALUE_PARAM_T value_param;
	u_int8_t REG_TMDS_VPDRV_12_1;
	u_int8_t REG_TMDS_IDRVA_13_1;
	u_int8_t REG_TMDS_IDRVB_14_1;
	u_int8_t REG_TMDS_IDRVC_15_1;
	u_int8_t REG_TMDS_IDRVCK_16_1;
	u_int8_t REG_TMDS_IEMA_18_1;
	u_int8_t REG_TMDS_IEMB_19_1;
	u_int8_t REG_TMDS_IEMC_20_1;
	u_int8_t REG_TMDS_IEMCK_21_1;
	u_int8_t REG_TMDS_IPDRV_23_1;
	u_int8_t REG_TMDS_IPDRVCK_24_1;
	u_int8_t REG_TMDS_IPDRVEM_25_1;
	u_int8_t REG_TMDS_IPDRVEMCK_26_1;
	u_int8_t REG_TMDS_IPDRVEMPRE_27_1;
	u_int8_t REG_TMDS_IPDRVEMPRECK_28_1;
	u_int8_t REG_TMDS_IEMPREA_30_1;
	u_int8_t REG_TMDS_IEMPREB_31_1;
	u_int8_t REG_TMDS_IEMPREC_32_1;
	u_int8_t REG_TMDS_IEMPRECK_33_1;
}hdmi21tx_phy_output_setting_T;

typedef struct {
	HDMI21TX_VALUE_PARAM_T value_param;
	u_int8_t REG_PLL_M1_1_1;
	u_int8_t REG_PLL_M2_2_1;
	u_int8_t reg_ncode_t_tmds_3_1;
	u_int16_t reg_fcode_t_tmds_4_1;
	u_int8_t REG_HDMITX_RESERVE_5_1;
}hdmi21tx_tmds_pll_freq_T;

typedef struct {
	HDMI21TX_VALUE_PARAM_T value_param;
	u_int8_t REG_PLL_IP_1_2;
	u_int8_t REG_P2S_MODE_4_2;
}hdmi21tx_tmds_pll_setting_T;

typedef struct {
	HDMI21TX_VALUE_PARAM_T value_param;
	u_int8_t REG_PLL_KVCO_RES_7_3;
}hdmi21tx_tmds_pll_flow_T;

//void HDMI21TX_Main_Seq(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);

//void Header_0_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
void HDMITx_Default_PHY_setting_1_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
void HDMITx_Default_PHY_setting_2_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
void HDMITx_Default_PHY_setting_3_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
void HDMI21Tx_DISP_PLL_setting_1_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
void HDMI21Tx_DISP_PLL_setting_2_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
void HDMI21Tx_DISP_PLL_setting_3_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
void HDMI21Tx_TMDS_PLL_setting_1_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
void HDMI21Tx_TMDS_PLL_setting_2_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
void HDMI21Tx_TMDS_PLL_setting_3_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
void HDMI21Tx_PHY_Output_setting_1_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
void HDMI21Tx_MAC_Ratio_setting_1_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);

#endif