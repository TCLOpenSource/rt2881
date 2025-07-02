#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include "../include/ScalerFunctionInclude.h"
#include "../include/ScalerTx.h"
#include "../include/hdmitx_phy_struct_def.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0Include.h"
#include "../include/Hdmi21MacTx/ScalerHdmi21MacTxInclude.h"
#include "../include/Hdmi21MacTx0/ScalerHdmi21MacTx0Include.h"
#include <rbus/hdmitx_phy_reg.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_driver.h>
#include "hdmitx_phy_frl.h"
#define _rtd_part_outl rtd_part_outl
#define _rtd_maskl rtd_maskl
#define _rtd_outl rtd_outl

hdmi21tx_disp_pll_freq_T hdmi21tx_disp_pll_freq[] =
{
	{HDMI21TX_PHY_2K60_3G3L,0x22,0x8},
	{HDMI21TX_PHY_4K60_6G,0x20,0x4},
	{HDMI21TX_PHY_4K60_8G,0x20,0x4},
	{HDMI21TX_PHY_4K60_10G,0x20,0x4},
	{HDMI21TX_PHY_4K60_12G,0x20,0x4},
};

hdmi21tx_disp_pll_setting_T hdmi21tx_disp_pll_setting[] =
{
	{HDMI21TX_PHY_2K60_3G3L,0x2,0x6},
	{HDMI21TX_PHY_4K60_6G,0x6,0x3},
	{HDMI21TX_PHY_4K60_8G,0x2,0x6},
	{HDMI21TX_PHY_4K60_10G,0x6,0x3},
	{HDMI21TX_PHY_4K60_12G,0x6,0x3},
};

hdmi21tx_disp_pll_flow_T hdmi21tx_disp_pll_flow[] =
{
	{HDMI21TX_PHY_2K60_3G3L,0x1},
	{HDMI21TX_PHY_4K60_6G,0x1},
	{HDMI21TX_PHY_4K60_8G,0x0},
	{HDMI21TX_PHY_4K60_10G,0x0},
	{HDMI21TX_PHY_4K60_12G,0x0},
};

hdmi21tx_phy_clk_divider_T hdmi21tx_phy_clk_divider[] =
{
	{HDMI21TX_PHY_2K60_3G3L,0x6},
	{HDMI21TX_PHY_4K60_6G,0x4},
	{HDMI21TX_PHY_4K60_8G,0x6},
	{HDMI21TX_PHY_4K60_10G,0x6},
	{HDMI21TX_PHY_4K60_12G,0x4},
};

hdmi21tx_phy_output_setting_T hdmi21tx_phy_output_setting[] =
{
	{HDMI21TX_PHY_2K60_3G3L,0x1,0x12,0x12,0x12,0x12,0x0,0x0,0x0,0x0,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x2,0x2,0x2,0x2},
	{HDMI21TX_PHY_4K60_6G,0x1,0x12,0x12,0x12,0x12,0x0,0x0,0x0,0x0,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x2,0x2,0x2,0x2},
	{HDMI21TX_PHY_4K60_8G,0x1,0x12,0x12,0x12,0x12,0x0,0x0,0x0,0x0,0x24,0x24,0x24,0x24,0x24,0x24,0x2,0x2,0x2,0x2},
	{HDMI21TX_PHY_4K60_10G,0x1,0x12,0x12,0x12,0x12,0x0,0x0,0x0,0x0,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x2,0x2,0x2,0x2},
	{HDMI21TX_PHY_4K60_12G,0x3,0x18,0x18,0x18,0x18,0x6,0x6,0x6,0x6,0x24,0x24,0x24,0x24,0x24,0x24,0x3,0x3,0x3,0x3},
};

hdmi21tx_tmds_pll_freq_T hdmi21tx_tmds_pll_freq[] =
{
	{HDMI21TX_PHY_2K60_3G3L,0x0,0x2,0x6c,0xe3,0x1},
	{HDMI21TX_PHY_4K60_6G,0x0,0x2,0x6c,0xe3,0x0},
	{HDMI21TX_PHY_4K60_8G,0x1,0x2,0x47,0x98,0x0},
	{HDMI21TX_PHY_4K60_10G,0x1,0x2,0x59,0x4be,0x0},
	{HDMI21TX_PHY_4K60_12G,0x1,0x0,0x6c,0xe3,0x0},
};

hdmi21tx_tmds_pll_setting_T hdmi21tx_tmds_pll_setting[] =
{
	{HDMI21TX_PHY_2K60_3G3L,0x13,0x0},
	{HDMI21TX_PHY_4K60_6G,0x16,0x1},
	{HDMI21TX_PHY_4K60_8G,0x12,0x1},
	{HDMI21TX_PHY_4K60_10G,0x12,0x1},
	{HDMI21TX_PHY_4K60_12G,0x17,0x1},
};

hdmi21tx_tmds_pll_flow_T hdmi21tx_tmds_pll_flow[] =
{
	{HDMI21TX_PHY_2K60_3G3L,0x0},
	{HDMI21TX_PHY_4K60_6G,0x0},
	{HDMI21TX_PHY_4K60_8G,0x0},
	{HDMI21TX_PHY_4K60_10G,0x1},
	{HDMI21TX_PHY_4K60_12G,0x1},
};

//void HDMI21TX_Main_Seq(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
//{
//	HDMI_EMG("Version:Mac9q_HDMI21Tx_PHY_CPFT_20240828_driver\n");
//	if(value_param==HDMI21TX_PHY_2K60_3G3L )
//	{
//		Header_0_func(nport, value_param);
//		HDMITx_Default_PHY_setting_1_func(nport, value_param);
//		HDMITx_Default_PHY_setting_2_func(nport, value_param);
//		HDMITx_Default_PHY_setting_3_func(nport, value_param);
//		HDMI21Tx_DISP_PLL_setting_1_func(nport, value_param);
//		HDMI21Tx_DISP_PLL_setting_2_func(nport, value_param);
//		HDMI21Tx_DISP_PLL_setting_3_func(nport, value_param);
//		HDMI21Tx_TMDS_PLL_setting_1_func(nport, value_param);
//		HDMI21Tx_TMDS_PLL_setting_2_func(nport, value_param);
//		HDMI21Tx_TMDS_PLL_setting_3_func(nport, value_param);
//		HDMI21Tx_PHY_Output_setting_1_func(nport, value_param);
//		HDMI21Tx_MAC_Ratio_setting_1_func(nport, value_param);
//	}
//	else if(value_param==HDMI21TX_PHY_2K60_3G3L || value_param==HDMI21TX_PHY_4K60_6G || value_param==HDMI21TX_PHY_4K60_8G || value_param==HDMI21TX_PHY_4K60_10G || value_param==HDMI21TX_PHY_4K60_12G || value_param==HDMI21TX_PHY_4K60_12G_TVS )
//	{
//		Header_0_func(nport, value_param);
//		HDMITx_Default_PHY_setting_1_func(nport, value_param);
//		HDMITx_Default_PHY_setting_2_func(nport, value_param);
//		HDMITx_Default_PHY_setting_3_func(nport, value_param);
//		HDMI21Tx_DISP_PLL_setting_1_func(nport, value_param);
//		HDMI21Tx_DISP_PLL_setting_2_func(nport, value_param);
//		HDMI21Tx_DISP_PLL_setting_3_func(nport, value_param);
//		HDMI21Tx_TMDS_PLL_setting_1_func(nport, value_param);
//		HDMI21Tx_TMDS_PLL_setting_2_func(nport, value_param);
//		HDMI21Tx_TMDS_PLL_setting_3_func(nport, value_param);
//		HDMI21Tx_PHY_Output_setting_1_func(nport, value_param);
//		HDMI21Tx_MAC_Ratio_setting_1_func(nport, value_param);
//	}
//}	
//void Header_0_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
//{}

void HDMITx_Default_PHY_setting_1_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
{
	_rtd_outl(0xb8000128, 0x0000000E); //RSTN_hdmitx RSTN_hdmitx_phy RSTN_hdmitx_off
	_rtd_outl(0xb8000138, 0x00000003); //enable clk of hdmi dsce/ hdmitx bit
	_rtd_outl(0xb8000138, 0x00000002); //disable clk of hdmi dsce/ hdmitx bit
	_rtd_outl(0xb8000128, 0x0000000f); //Reset rlease clk of hdmi dsce/ hdmitx bit
	_rtd_outl(0xb8000138, 0x00000003); //enable clk of hdmi dsce/ hdmitx bit
}

void HDMITx_Default_PHY_setting_2_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
{
	_rtd_part_outl(0xb8047808, 28, 28, 1); //CLK_TMDS=1
	_rtd_part_outl(0xb8047820, 30, 28, 0x0); //REG_PLL_CKSEL
	_rtd_part_outl(0xb8047820, 25, 24, 0x0); //REG_PLL_CP
	_rtd_part_outl(0xb8047820, 5, 0, 0x10); //REG_PLL_IP
	_rtd_part_outl(0xb8047824, 21, 20, 0x3); //LDO_2.2V outout voltage select
	_rtd_part_outl(0xb8047818, 25, 24, 0x2); //REG_PLLDISP_LDOSEL
	_rtd_part_outl(0xb8047824, 18, 16, 0x1); //REG_PLL_M1
	_rtd_part_outl(0xb8047824, 15, 12, 0x0); //REG_PLL_M2
	_rtd_part_outl(0xb8047844, 28, 28, 0x1); //REG_TMDS_POW
	_rtd_part_outl(0xb804790c, 10, 10, 0x1); //REG_PLL_LDO_POW
	_rtd_part_outl(0xb8047824, 4, 4, 0x1); //REG_PLL_POW
	_rtd_part_outl(0xb8047824, 2, 0, 0x4); //REG_PLL_RS
	_rtd_part_outl(0xb8047828, 28, 28, 0x0); //REG_PLL_RSTB
	_rtd_part_outl(0xb8047828, 20, 20, 0x0); //REG_PLL_TST_POW
	_rtd_part_outl(0xb8047828, 19, 19, 0x0); //(REG_PLL_VCOGAIN)//P2S_RSTB_Mannual 0:reset 1:active
	_rtd_part_outl(0xb804782c, 28, 28, 0x0); //REG_PLL_WDRST
	_rtd_part_outl(0xb804782c, 24, 24, 0x0); //REG_PLL_WDSET
	_rtd_outl(0xb804785c, 0xccfdb0e4); //PLL TMDS target M code
	_rtd_part_outl(0xb8047800, 20, 20, 0x0); //REG_CMU_BYPASS_PI
	udelay(1000);
	_rtd_part_outl(0xb8047820, 16, 16, 0x0); //REG_PLL_DIVNSEL
	_rtd_part_outl(0xb8047824, 28, 28, 0x1); //REG_PLL_KVCO
	_rtd_part_outl(0xb8047820, 13, 12, 0x0); //REG_PLL_EN_CAP
	_rtd_part_outl(0xb8047828, 2, 0, 0x3); //REG_PLL_VC_BD_SEL//<2>P2S input clock div2 enable for CK/B/G/R 0:div1 1:div2
	_rtd_part_outl(0xb8047828, 18, 16, 0x0); //REG_PLL_VC2_BD_SEL
	_rtd_part_outl(0xb8047828, 13, 12, 0x3); //REG_PLL_VC2_RES
	_rtd_part_outl(0xb8047820, 8, 8, 0x0); //REG_PLL_EXT_LDO_LV
	_rtd_part_outl(0xb8047800, 21, 21, 0x0); //External CLK_V10 control(0: internal LDO \xE2CLK_V10��)
	_rtd_part_outl(0xb804781c, 16, 16, 0x0); //REG_PLLDISP_RSTB
	_rtd_part_outl(0xb804781c, 11, 11, 0x0); //REG_PLLDISP_VCORSTB
	_rtd_part_outl(0xb8047828, 28, 28, 0x0); //REG_PLL_RSTB
	_rtd_part_outl(0xb8047828, 24, 24, 0x1); //REG_PLL_TSPC_SEL
	_rtd_part_outl(0xb8047850, 8, 8, 0x1); //REG_TSPC_SEL
	udelay(2000);
}

void HDMITx_Default_PHY_setting_3_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
{
	_rtd_part_outl(0xb8047824, 25, 24, 0x3); //REG_PLL_LDO_LV_SEL
	_rtd_part_outl(0xb804780c, 22, 20, 0x3); //REG_P2S_LDO_SEL
	_rtd_part_outl(0xb8047848, 31, 28, 0xa); //REG_TMDS_RSELA=101 ohm
	_rtd_part_outl(0xb8047848, 27, 24, 0xa); //REG_TMDS_RSELB=101 ohm
	_rtd_part_outl(0xb8047848, 23, 20, 0xa); //REG_TMDS_RSELC=101 ohm
	_rtd_part_outl(0xb8047848, 19, 16, 0xa); //REG_TMDS_RSELCK=101 ohm
	_rtd_part_outl(0xb8047804, 5, 5, 0x1); //Bypass DCC, 0: DCC on; 1: DCC off
	_rtd_part_outl(0xb804781c, 26, 24, 0x4); //<2>P2S RSTB MODE SEL
	//0: internal delay 1: manual (REG_PLL_VCOGAIN)
	//<1:0> internal del sel
	//00:5us 01:20us 10:40us 11:80us
	
	_rtd_part_outl(0xb8047810, 21, 20, 0x3); //P2S DCC reset mode 00:LV 01:LV/2 10:X 11: GND
}

void HDMI21Tx_DISP_PLL_setting_1_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
{
	_rtd_part_outl(0xb80478ac, 0, 0, 0x0); //OC_EN_DISP
	_rtd_part_outl(0xb80478b0, 31, 24, 0x13); //reg_ncode_t_ssc_disp
	_rtd_part_outl(0xb80478ac, 26, 16, 0x0); //reg_ncode_t_ssc_disp
	_rtd_part_outl(0xb8047818, 22, 20, 0x0); //PLLDISP_N
	_rtd_part_outl(0xb8047818, 29, 28, 0x0); //REG_PLLDISP_K
	_rtd_part_outl(0xb804780c, 11, 4, hdmi21tx_disp_pll_freq[value_param].REG_PIXEL_DIV_6_1); //REG_PIXEL_DIV
	_rtd_part_outl(0xb8047804, 9, 9, 0x0); //REG_HDMITX_RESERVE[9]=0 (/1), 1.25 divider for CLK_TMDS path
	_rtd_part_outl(0xb8047830, 15, 12, hdmi21tx_disp_pll_freq[value_param].REG_TMDS_DIV_8_1); //REG_TMDS_DIV
	_rtd_part_outl(0xb80478b4, 0, 0, 0x1); //reg_bypass_pi_disp=disp_fcode? 0: 1
}

void HDMI21Tx_DISP_PLL_setting_2_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
{
	_rtd_part_outl(0xb8047814, 3, 0, hdmi21tx_disp_pll_setting[value_param].REG_PLLDISP_IP_1_2); //REG_PLLDISP_IP
	_rtd_part_outl(0xb8047810, 13, 12, 0x0); //REG_PLLDISP_CS
	_rtd_part_outl(0xb8047810, 17, 16, 0x0); //REG_PLLDISP_CP
	_rtd_part_outl(0xb804781c, 22, 20, hdmi21tx_disp_pll_setting[value_param].REG_PLLDISP_RS_4_2); //REG_PLLDISP_RS
	_rtd_part_outl(0xb80478b4, 2, 2, 0x0); //reg_en_pi_debug_disp
	_rtd_part_outl(0xb80478b4, 6, 4, 0x3); //reg_pi_cur_sel_disp
}

void HDMI21Tx_DISP_PLL_setting_3_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
{
	_rtd_part_outl(0xb8047818, 4, 4, 0x1); //REG_PLLDISP_POW
	_rtd_part_outl(0xb804781c, 11, 11, hdmi21tx_disp_pll_flow[value_param].REG_PLLDISP_VCORSTB_2_3); //PLLDISP_VCORSTB
	_rtd_part_outl(0xb804781c, 16, 16, 0x1); //REG_PLLDISP_RSTB
	udelay(1000);
	//hdmitx disp pll M code setting
	_rtd_part_outl(0xb80478ac, 0, 0, 0x1); //OC_EN_DISP
	udelay(1000);
	_rtd_part_outl(0xb80478ac, 0, 0, 0x0); //OC_EN_DISP
	//hdmitx disp pll M code setting done
}

void HDMI21Tx_TMDS_PLL_setting_1_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
{
	_rtd_part_outl(0xb8047824, 18, 16, hdmi21tx_tmds_pll_freq[value_param].REG_PLL_M1_1_1); //REG_PLL_M1
	_rtd_part_outl(0xb8047824, 15, 12, hdmi21tx_tmds_pll_freq[value_param].REG_PLL_M2_2_1); //REG_PLL_M2
	_rtd_part_outl(0xb804785c, 19, 12, hdmi21tx_tmds_pll_freq[value_param].reg_ncode_t_tmds_3_1); //reg_ncode_t_tmds
	_rtd_part_outl(0xb804785c, 10, 0, hdmi21tx_tmds_pll_freq[value_param].reg_fcode_t_tmds_4_1); //reg_fcode_t_tmds
	_rtd_part_outl(0xb8047804, 16, 16, hdmi21tx_tmds_pll_freq[value_param].REG_HDMITX_RESERVE_5_1); //P2S clock select, 0:/2, 1:/1
}

void HDMI21Tx_TMDS_PLL_setting_2_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
{
	_rtd_part_outl(0xb8047820, 5, 0, hdmi21tx_tmds_pll_setting[value_param].REG_PLL_IP_1_2); //REG_PLL_IP
	_rtd_part_outl(0xb8047824, 2, 0, 0x2); //REG_PLL_RS
	_rtd_part_outl(0xb8047820, 21, 20, 0x3); //REG_PLL_CS
	_rtd_part_outl(0xb804780c, 17, 16, hdmi21tx_tmds_pll_setting[value_param].REG_P2S_MODE_4_2); //RL6702 set 0 REG_P2S_MODE[17:16]=1:CK_RD positive trigger for P2S circuit , below 8G set 0; others set 1 
	_rtd_part_outl(0xb804785c, 25, 25, 0x0); //reg_bypass_pi_tmds
	_rtd_part_outl(0xb8047828, 4, 4, 0x0); //REG_PLL_VCOSEL
	_rtd_part_outl(0xb804785c, 30, 30, 0x1); //reg_pow_sscd_tmds
}

void HDMI21Tx_TMDS_PLL_setting_3_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
{
	_rtd_part_outl(0xb804781c, 1, 0, 0x1); //REG_PLL_CKIN_SEL=1,CKIN_27M, for HDMI2.1
	_rtd_part_outl(0xb8047824, 4, 4, 0x1); //REG_PLL_POW
	_rtd_part_outl(0xb8047800, 12, 12, 0x1); //REG_HDMI21_MODE
	_rtd_part_outl(0xb8047820, 16, 16, 0x1); //REG_PLL_DIVNSEL
	_rtd_part_outl(0xb8047804, 6, 6, 0x0); //POW_RXON, 0: RX_Detect enable by REG_TMDS_POW
	_rtd_part_outl(0xb8047828, 19, 19, 0x0); //(REG_PLL_VCOGAIN)//P2S_RSTB_Mannual 0:reset 1:active
	_rtd_part_outl(0xb804790c, 9, 8, hdmi21tx_tmds_pll_flow[value_param].REG_PLL_KVCO_RES_7_3); //Mac9q will change name to REG_PLL_KVCO_RES
	_rtd_part_outl(0xb8047820, 30, 28, 0x0); //REG_PLL_CKSEL, 0:/1postive edge ;4:/4 postive edge
	_rtd_part_outl(0xb8047828, 28, 28, 0x1); //REG_PLL_RSTB
	udelay(500);
	_rtd_part_outl(0xb8047800, 20, 20, 0x1); //REG_CMU_BYPASS_PI
	udelay(500);
	_rtd_part_outl(0xb8047828, 28, 28, 0x0); //REG_PLL_RSTB
	udelay(500);
	_rtd_part_outl(0xb8047824, 28, 28, 0x0); //REG_PLL_KVCO
	_rtd_part_outl(0xb8047828, 28, 28, 0x1); //REG_PLL_RSTB
	udelay(500);
	//hdmitx tmds pll MF code setting start
	_rtd_part_outl(0xb804785c, 29, 29, 0x0); //reg_oc_en_tmds
	_rtd_part_outl(0xb804785c, 29, 29, 0x1); //reg_oc_en_tmds
	udelay(1000);
	_rtd_part_outl(0xb804785c, 29, 29, 0x0); //reg_oc_en_tmds
	udelay(1000);
	//hdmitx tmds pll MF code setting done
	_rtd_part_outl(0xb8047824, 28, 28, 0x1); //REG_PLL_KVCO
	_rtd_part_outl(0xb8047800, 20, 20, 0x0); //REG_CMU_BYPASS_PI
	udelay(500);
	_rtd_part_outl(0xb8047804, 5, 5, 0x0); //Bypass DCC, 0: DCC on; 1: DCC off
	udelay(500);
	_rtd_part_outl(0xb8047828, 19, 19, 0x1); //(REG_PLL_VCOGAIN)//P2S_RSTB_Mannual 0:reset 1:active
}

void HDMI21Tx_PHY_Output_setting_1_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
{
	_rtd_part_outl(0xb8047834, 28, 28, 0x1); //REG_TMDS_EMA
	_rtd_part_outl(0xb8047834, 24, 24, 0x1); //REG_TMDS_EMB
	_rtd_part_outl(0xb8047834, 20, 20, 0x1); //REG_TMDS_EMC
	_rtd_part_outl(0xb8047834, 16, 16, 0x1); //REG_TMDS_EMCK
	
	_rtd_part_outl(0xb8047834, 12, 12, 0x1); //REG_TMDS_EMPREA
	_rtd_part_outl(0xb8047834, 8, 8, 0x1); //REG_TMDS_EMPREB
	_rtd_part_outl(0xb8047834, 4, 4, 0x1); //REG_TMDS_EMPREC
	_rtd_part_outl(0xb8047834, 0, 0, 0x1); //REG_TMDS_EMPRECK
	
	_rtd_part_outl(0xb8047850, 13, 12, hdmi21tx_phy_output_setting[value_param].REG_TMDS_VPDRV_12_1); //REG_TMDS_VPDRV:Pre-driver voltage selection
	_rtd_part_outl(0xb8047838, 28, 24, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IDRVA_13_1); //REG_TMDS_IDRVA
	_rtd_part_outl(0xb8047838, 20, 16, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IDRVB_14_1); //REG_TMDS_IDRVB
	_rtd_part_outl(0xb8047838, 12, 8, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IDRVC_15_1); //REG_TMDS_IDRVC
	_rtd_part_outl(0xb8047838, 4, 0, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IDRVCK_16_1); //REG_TMDS_IDRVCK
	
	_rtd_part_outl(0xb804783c, 28, 24, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IEMA_18_1); //REG_TMDS_IEMA
	_rtd_part_outl(0xb804783c, 20, 16, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IEMB_19_1); //REG_TMDS_IEMB
	_rtd_part_outl(0xb804783c, 12, 8, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IEMC_20_1); //REG_TMDS_IEMC
	_rtd_part_outl(0xb804783c, 4, 0, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IEMCK_21_1); //REG_TMDS_IEMCK
	
	_rtd_part_outl(0xb8047858, 21, 16, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IPDRV_23_1); //REG_TMDS_IPDRV
	_rtd_part_outl(0xb8047858, 13, 8, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IPDRVCK_24_1); //REG_TMDS_IPDRVCK
	_rtd_part_outl(0xb8047858, 5, 0, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IPDRVEM_25_1); //REG_TMDS_IPDRVEM
	_rtd_part_outl(0xb8047854, 21, 16, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IPDRVEMCK_26_1); //REG_TMDS_IPDRVEMPRE
	_rtd_part_outl(0xb8047854, 13, 8, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IPDRVEMPRE_27_1); //REG_TMDS_IPDRVEMPRE
	_rtd_part_outl(0xb8047854, 5, 0, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IPDRVEMPRECK_28_1); //REG_TMDS_IPDRVEMPRECK
	
	_rtd_part_outl(0xb8047840, 31, 28, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IEMPREA_30_1); //REG_TMDS_IEMPREA
	_rtd_part_outl(0xb8047840, 27, 24, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IEMPREB_31_1); //REG_TMDS_IEMPREB
	_rtd_part_outl(0xb8047840, 23, 20, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IEMPREC_32_1); //REG_TMDS_IEMPREC
	_rtd_part_outl(0xb8047840, 19, 16, hdmi21tx_phy_output_setting[value_param].REG_TMDS_IEMPRECK_33_1); //REG_TMDS_IEMPRECK
	
	_rtd_part_outl(0xb8047850, 29, 28, 0x0); //REG_TMDS_SREMPREA
	_rtd_part_outl(0xb8047850, 25, 24, 0x0); //REG_TMDS_SREMPREB
	_rtd_part_outl(0xb8047850, 21, 20, 0x0); //REG_TMDS_SREMPREC
	_rtd_part_outl(0xb8047850, 17, 16, 0x0); //REG_TMDS_SREMPRECK
	
	_rtd_part_outl(0xb8047830, 29, 28, 0x0); //REG_TMDS_CSELA
	_rtd_part_outl(0xb8047830, 25, 24, 0x0); //REG_TMDS_CSELB
	_rtd_part_outl(0xb8047830, 21, 20, 0x0); //REG_TMDS_CSELC
	_rtd_part_outl(0xb8047830, 17, 16, 0x0); //REG_TMDS_CSELCK
	
	_rtd_part_outl(0xb804784c, 29, 28, 0x0); //REG_TMDS_SRA
	_rtd_part_outl(0xb804784c, 25, 24, 0x0); //REG_TMDS_SRB
	_rtd_part_outl(0xb804784c, 21, 20, 0x0); //REG_TMDS_SRC
	_rtd_part_outl(0xb804784c, 17, 16, 0x0); //REG_TMDS_SRCK
	
	_rtd_part_outl(0xb804784c, 13, 12, 0x0); //REG_TMDS_SREMA
	_rtd_part_outl(0xb804784c, 9, 8, 0x0); //REG_TMDS_SREMB
	_rtd_part_outl(0xb804784c, 5, 4, 0x0); //REG_TMDS_SREMC
	_rtd_part_outl(0xb804784c, 1, 0, 0x0); //REG_TMDS_SREMCK
	
	_rtd_part_outl(0xb804782c, 12, 12, 0x0); //REG_TMDS_ACCOUPLE_SEL=DC couple mode
	_rtd_part_outl(0xb804782c, 21, 20, 0x0); //TEST_SEL
	
	_rtd_part_outl(0xb8047834, 28, 28, 0x1); //REG_TMDS_EMA
	_rtd_part_outl(0xb8047834, 24, 24, 0x1); //REG_TMDS_EMB
	_rtd_part_outl(0xb8047834, 20, 20, 0x1); //REG_TMDS_EMC
	_rtd_part_outl(0xb8047834, 16, 16, 0x1); //REG_TMDS_EMCK
	
	_rtd_part_outl(0xb8047834, 12, 12, 0x1); //REG_TMDS_EMPREA
	_rtd_part_outl(0xb8047834, 8, 8, 0x1); //REG_TMDS_EMPREB
	_rtd_part_outl(0xb8047834, 4, 4, 0x1); //REG_TMDS_EMPREC
	_rtd_part_outl(0xb8047834, 0, 0, 0x1); //REG_TMDS_EMPRECK
	
	_rtd_part_outl(0xb8047844, 28, 28, 0x1); //REG_TMDS_POW
	
	_rtd_part_outl(0xb8047844, 12, 12, 0x1); //REG_TMDS_POWA
	_rtd_part_outl(0xb8047844, 8, 8, 0x1); //REG_TMDS_POWB
	_rtd_part_outl(0xb8047844, 4, 4, 0x1); //REG_TMDS_POWC
	_rtd_part_outl(0xb8047844, 0, 0, 0x1); //REG_TMDS_POWCK
}

void HDMI21Tx_MAC_Ratio_setting_1_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param)
{
	_rtd_part_outl(0xb8047958, 31, 31, 0x1); //reg_clkratio_active
	_rtd_part_outl(0xb8047958, 23, 23, 0x0); //reg_hdmitxpll_divs2_sel
	_rtd_part_outl(0xb8047958, 22, 20, 0x6); //reg_clkratio_pixel_mac
	_rtd_part_outl(0xb8047958, 18, 16, 0x6); //reg_clkratio_txsoc_420
	_rtd_part_outl(0xb8047958, 14, 12, hdmi21tx_phy_clk_divider[value_param].reg_clkratio_hdmitx_ds444_2p_5_1); //reg_clkratio_txsoc
	_rtd_part_outl(0xb8047958, 10, 8, 0x0); //reg_clkratio_dsce_sclk
	_rtd_part_outl(0xb8047958, 6, 4, 0x0); //reg_clkratio_dsce_cclk
	_rtd_part_outl(0xb8047958, 2, 0, 0x0); //reg_clkratio_dsce_pclk
	_rtd_part_outl(0xb8047958, 31, 31, 0x1); //reg_clkratio_active
}


// For TV001 H5D board
// [TV001] H5D board TX AMP fine tune table
hdmi_tx_amp_fine_tune_st hdmi_tx_FFE_table_TV001[HDMI_TX_AMP_FINE_TUNE_INDEX_NUM+1] = {
/*
 * Discription : hdmi tx amp fine tune table
 * Table format : {{r level0}}  , {r level1}    , {r level2}    , {r level3}}
 *                   : {{g level0}} , {g level1}    , {g level2}   , {g level3}}
 *                   : {{b level0}} , {b level1}    , {b level2}   , {b level3}}
 *                   : {{clk level0}}, {clk level1} , {clk level2} , {clk level3}}
 *  	{r}	: {idrv, iem, iempre}
 *  	{g}	: {idrv, iem, iempre}
 *  	{b}	: {idrv, iem, iempre}
 *  	{clk}: {idrv, iem, iempre}
 * Table index  :
 *      HDMI_TX_AMP_FINE_TUNE_INDEX_NUM : see HDMI_TX_AMP_FINE_TUNE_INDEX
 */
    {                                               // HDMI21_TX_FFE_3G
        {{0x12, 2, 2},{0x0e, 3, 3},{0x0a, 4, 4},{0x06, 5, 0x5}}, // R channel: level {0},{1},{2},{3}
        {{0x12, 2, 2},{0x0e, 3, 3},{0x0a, 4, 4},{0x06, 5, 0x5}}, // G channel: level {0},{1},{2},{3}
        {{0x12, 2, 2},{0x0e, 3, 3},{0x0a, 4, 4},{0x06, 5, 0x5}}, // B channel: level {0},{1},{2},{3}
        {{0x12, 3, 2},{0x0e, 3, 3},{0x0a, 4, 4},{0x06, 5, 0x5}}, // CLK channel: level {0},{1},{2},{3}
    },//Mark2 FIX ME, FEE LEVEL table need to update
    {                                               // HDMI21_TX_FFE_6G
        {{0x11, 2, 1},{0xd, 3, 2},{0x9, 4, 3},{0x5, 5, 0x4}}, // R channel: level {0},{1},{2},{3}
        {{0x11, 2, 1},{0xd, 3, 2},{0x9, 4, 3},{0x5, 5, 0x4}}, // G channel: level {0},{1},{2},{3}
        {{0x11, 2, 1},{0xd, 3, 2},{0x9, 4, 3},{0x5, 5, 0x4}}, // B channel: level {0},{1},{2},{3}
        {{0x11, 2, 1},{0xd, 3, 2},{0x9, 4, 3},{0x5, 5, 0x4}}, // CLK channel: level {0},{1},{2},{3}
    },
    {                                               // HDMI21_TX_FFE_8G
        {{0x11, 4, 2},{0xd, 5, 3},{0x9, 6, 4},{0x5, 7, 0x5}}, // R channel: level {0},{1},{2},{3}
        {{0x11, 4, 2},{0xd, 5, 3},{0x9, 6, 4},{0x5, 7, 0x5}}, // G channel: level {0},{1},{2},{3}
        {{0x11, 4, 2},{0xd, 5, 3},{0x9, 6, 4},{0x5, 7, 0x5}}, // B channel: level {0},{1},{2},{3}
        {{0x11, 4, 2},{0xd, 5, 3},{0x9, 6, 4},{0x5, 7, 0x5}}, // CLK channel: level {0},{1},{2},{3}
    },
    {                                               // HDMI21_TX_FFE_10G
        {{0x14, 4, 2},{0x10, 5, 3},{0x0c, 6, 4},{0x08, 7, 0x5}}, // R channel: level {0},{1},{2},{3}
        {{0x14, 4, 2},{0x10, 5, 3},{0x0c, 6, 4},{0x08, 7, 0x5}}, // G channel: level {0},{1},{2},{3}
        {{0x14, 4, 2},{0x10, 5, 3},{0x0c, 6, 4},{0x08, 7, 0x5}}, // B channel: level {0},{1},{2},{3}
        {{0x14, 4, 2},{0x10, 5, 3},{0x0c, 6, 4},{0x08, 7, 0x5}}, // CLK channel: level {0},{1},{2},{3}
    },
    {                                               // HDMI21_TX_FFE_12G
        {{0x12, 3, 3},{0x0e, 4, 4},{0x0a, 5, 5},{0x06, 6, 0x6}}, // R channel: level {0},{1},{2},{3}
        {{0x12, 3, 3},{0x0e, 4, 4},{0x0a, 5, 5},{0x06, 6, 0x6}}, // G channel: level {0},{1},{2},{3}
        {{0x12, 3, 3},{0x0e, 4, 4},{0x0a, 5, 5},{0x06, 6, 0x6}}, // B channel: level {0},{1},{2},{3}
        {{0x12, 3, 3},{0x0e, 4, 4},{0x0a, 5, 5},{0x06, 6, 0x6}}, // CLK channel: level {0},{1},{2},{3}
    },
    {                                               // FFE TABLE CHECK CODE
        {{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff}}, // R channel: level {0},{1},{2},{3}
        {{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff}}, // G channel: level {0},{1},{2},{3}
        {{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff}}, // B channel: level {0},{1},{2},{3}
        {{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff}}, // CLK channel: level {0},{1},{2},{3}
    },
};
