/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2012>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : ScalerHdmiPhyTx.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>

#include "../include/ScalerFunctionInclude.h"
#include "../include/ScalerTx.h"
#include "../include/hdmitx_phy_struct_def.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0Include.h"
#include "../include/Hdmi21MacTx/ScalerHdmi21MacTxInclude.h"
#include "../include/Hdmi21MacTx0/ScalerHdmi21MacTx0Include.h"
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_driver.h>
#include "hdmitx_phy_tmds.h"
#include "hdmitx_phy_frl.h"

#include <rbus/hdmitx_phy_reg.h>

/************************
// Functoin/Macro Definition
//
*************************/



/************************
// Debug Functoin/Macro Definition
//
*************************/
//#define HDMITX_PTG_EN
//#define TVS_Support //dummy-all setting with TVS
//#define FFE_Support
//#define OTP_TRIM_EN // [DANBU-826] OTP setting
#ifdef OTP_TRIM_EN
#include <rbus/efuse_reg.h>

//0x180380D4[31:23]
#define OTP_TRIM_DONE_SHIFT (31) // 0x180380D4[31] HDMI TX Trim Done Bit
#define OTP_TRIM_DONE_MASK (0x80000000UL) // 0x180380D4[31] HDMI TX Trim Done Bit
#define OTP_TRIM_GET_TRIM_DONE(data) (((OTP_TRIM_DONE_MASK & (UINT32)data)) >> OTP_TRIM_DONE_SHIFT) // 0x180380D4[30] HDMI TX Trim Done Bit

#define OTP_TRIM_Z0_SEL_SETTING_SHIFT (28) // 0x180380D4[30:28]HDMI TX Z0 : Z0: 3 bit B[2:0], B[3] always=1
#define OTP_TRIM_Z0_SEL_SETTING_MASK (0x70000000UL)
#define OTP_TRIM_GET_Z0_SEL_SETTING(data) ((OTP_TRIM_Z0_SEL_SETTING_MASK & ((UINT32)data)) >> OTP_TRIM_Z0_SEL_SETTING_SHIFT) // , B[3] always=1

#define OTP_TRIM_P2S_LDO_SEL_SETTING_SHIFT (25)// 0x180380D4[27:25]HDMI TX P2S_LDO : P2S_LDO: 2 bit
#define OTP_TRIM_P2S_LDO_SEL_SETTING_MASK (0x0e000000UL)
#define OTP_TRIM_GET_P2S_LDO_SEL_SETTING(data) (((OTP_TRIM_P2S_LDO_SEL_SETTING_MASK & (UINT32)data)) >>  OTP_TRIM_P2S_LDO_SEL_SETTING_SHIFT)

#define OTP_TRIM_PLL_LDO_LV_SEL_SETTING_SHIFT (23)// 0x180380D4[24:23]HDMI TX PLL_LDO : PLL_LDO : 2 bit
#define OTP_TRIM_PLL_LDO_LV_SEL_SETTING_MASK (0x01800000UL)
#define OTP_TRIM_GET_PLL_LDO_LV_SEL_SETTING(data) (((OTP_TRIM_PLL_LDO_LV_SEL_SETTING_MASK & (UINT32)data)) >>  OTP_TRIM_PLL_LDO_LV_SEL_SETTING_SHIFT)
#endif

#define OC_DONE_TIMEOUT 10
#define H5X_CHIPID_VER_A 0x65830000

#define MIN_PLLDISP_FVCO           432000000UL
#define MAX_PLLDISP_FVCO           891000000UL
#define CLK_594M           594000000UL
#define CLK_600M           600000000UL
#define CLKIN_27M           27000000UL
#define MIN_TMDS_FVCO           1800000000UL
#define TMDS_VCOGAIN            3000000000UL
#define TMDS_50M         50000000UL
#define TMDS_100M       100000000UL
#define TMDS_200M       200000000UL
#define TMDS_400M       400000000UL
//#define PLLDISP_N_MIN           (MIN_PLLDISP_FVCO / CLKIN_27M)
//#define PLLDISP_N_MAX           (MAX_PLLDISP_FVCO / CLKIN_27M)
/************************
// static/local variable Definition
//
*************************/
static UINT8 curFfeId=HDMI21_TX_FFE_TABLE_LIST_NUM;

// PLL
//static hdmi_tx_timing_gen_st *timing_gen=NULL;
static pll_div_param_st *div_param=NULL;
//code pll_analog_param_st *analog_param=NULL;

// swing
static hdmi_tx_swing_st *tx_swing=NULL;

// amp fine tune
static hdmi_tx_amp_fine_tune_st *hdmi_tx_FFE_table=NULL;

static UINT8 HDMITX_OUT_FFE_LEVEL=0; // FFE level 0~3

//static StructHdmitxDisplayInfo *p_tx_disp_info;

TMDS_PLL_PARAM *tmds_pll = NULL;
pll_div_param_st pll_cal_param;
static UINT32 tmds_clk = 0;
static UINT8 best_pixel_div = 0;
static UINT8 is_over_600M = 0;
static hdmi_tx_swing_st *tx_cal_swing=NULL;

/************************
// extern/global variable Definition
//
*************************/
#ifdef HDMITX_TIMING_TABLE_CONFIG
// HDMITX PHY timing table
extern hdmi_tx_timing_gen_st cea_timing_table[TX_TIMING_NUM];

// div_param
extern pll_div_param_st pll_div_deep_color_8bit_table[TX_TIMING_NUM];
extern pll_div_param_st pll_div_deep_color_10bit_table[TX_TIMING_NUM];
extern pll_div_param_st pll_div_deep_color_12bit_table[TX_TIMING_NUM];

//extern pll_analog_param_st pll_analog_deep_color_8bit_table[TX_TIMING_NUM];
//extern pll_analog_param_st pll_analog_deep_color_10bit_table[TX_TIMING_NUM];
//extern pll_analog_param_st pll_analog_deep_color_12bit_table[TX_TIMING_NUM];

// tx_swing
extern hdmi_tx_swing_st hdmi_tx_swing_dpc_8bit_table_tv001[TX_TIMING_NUM+1];
extern hdmi_tx_swing_st hdmi_tx_swing_dpc_10bit_table_tv001[TX_TIMING_NUM+1];
extern hdmi_tx_swing_st hdmi_tx_swing_dpc_12bit_table_tv001[TX_TIMING_NUM+1];
#endif

// amp_fine_tune
extern hdmi_tx_amp_fine_tune_st hdmi_tx_FFE_table_TV001[HDMI_TX_AMP_FINE_TUNE_INDEX_NUM+1];


extern StructTimingInfo g_stHdmiTx0InputTimingInfo;
extern StructHdmitxDisplayInfo tx_disp_info[1];
extern UINT8 HDMITX_TIMING_INDEX;
#ifdef HDMI_INPUT_SOURCE_CONFIG
extern MEASURE_TIMING_T timing_info;
#endif

extern UINT8 factor_div[DISP_PLL_DIV_TABLE_NUM];
extern DISP_PLL_DIV_PARAM disp_pll_div_hdmi20[DISP_PLL_DIV_TABLE_NUM];
extern DISP_PLL_DIV_PARAM disp_pll_div_hdmi20_420[DISP_PLL_DIV_TABLE_NUM];
extern DISP_PLL_DIV_PARAM disp_pll_div_hdmi21[DISP_PLL_DIV_TABLE_NUM];
extern DISP_PLL_DIV_PARAM disp_pll_div_hdmi21_2p[DISP_PLL_DIV_TABLE_NUM];
extern DISP_PLL_DIV_PARAM disp_pll_div_hdmi21_420[DISP_PLL_DIV_TABLE_NUM];
extern DISP_PLL_DIV_PARAM disp_pll_div_hdmi20_repeat;
extern DISP_PLL_DIV_PARAM disp_pll_div_hdmi21_repeat;
extern TMDS_PLL_PARAM tmds_pll_setting[_HDMI21_FRL_12G + 1];
extern hdmi_tx_dsc_clkratio_st dsc_clk_ratio_setting[DSCE_TIMING_NUM];
extern hdmi_tx_swing_st hdmi_tx_swing_hdmitx20_table[SWING_HDMI20_TABLE_NUM];
extern hdmi_tx_swing_st hdmi_tx_swing_hdmitx21_table[SWING_HDMI21_TABLE_NUM];
extern hdmi_tx_swing_st hdmi_tx_swing_hdmitx21_pcb002_table[SWING_HDMI21_TABLE_NUM];
extern void HDMI20Tx_TMDS_PLL_setting_2_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
extern void HDMI21Tx_TMDS_PLL_setting_2_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
extern void HDMI20Tx_TMDS_PLL_setting_3_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
extern void HDMI21Tx_TMDS_PLL_setting_3_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
extern void HDMI20Tx_PHY_Output_setting_1_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
extern void HDMI21Tx_PHY_Output_setting_1_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
extern void HDMITx_Default_PHY_setting_2_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
extern void HDMITx_Default_PHY_setting_3_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
extern void HDMI20Tx_DISP_PLL_setting_2_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
extern void HDMI21Tx_DISP_PLL_setting_2_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
extern void HDMI20Tx_DISP_PLL_setting_3_func(unsigned char nport, HDMI20TX_VALUE_PARAM_T value_param);
extern void HDMI21Tx_DISP_PLL_setting_3_func(unsigned char nport, HDMI21TX_VALUE_PARAM_T value_param);
extern void HDMITX_drv_set_hdmitx_gate_sel(void);


/************************
// extern function Definition
//
*************************/

/************************
// function Implementation
//
*************************/
EnumHdmi21FrlType ScalerHdmiTxGetOutputFrlType(void)
{
    return GET_HDMI21_MAC_TX_FRL_RATE(_TX0);
}



UINT8 ScalerHdmiTxGetOutputFfeLevel(void)
{
    return HDMITX_OUT_FFE_LEVEL;
}



UINT8 ScalerHdmiTxPhy_CheckFfeSwingTableCodeCorrect(void)
{
    hdmi_tx_amp_fine_tune_st *amp_fine_tune=NULL;

    // check FFE table
    // tv001
    amp_fine_tune = &hdmi_tx_FFE_table_TV001[HDMI_TX_AMP_FINE_TUNE_INDEX_NUM];
    if((amp_fine_tune->r[0].idrv != 0xff) || (amp_fine_tune->g[0].idrv != 0xff) || (amp_fine_tune->b[0].idrv != 0xff) || (amp_fine_tune->clk[0].idrv != 0xff)){
        FatalMessageHDMITx("[HDMITX][ERROR] FFE/SWING TABLE NOT MATCH R/G=[%x/%x]@TV001\n",
            (UINT32)amp_fine_tune->r[0].idrv, (UINT32)amp_fine_tune->g[0].idrv);
        FatalMessageHDMITx("[HDMITX][ERROR] FFE/SWING TABLE NOT MATCH B/CK=[%x/%x]@TV001\n",
            (UINT32)amp_fine_tune->b[0].idrv, (UINT32)amp_fine_tune->clk[0].idrv);
        return 0;
    }
#ifdef HDMITX_TIMING_TABLE_CONFIG
    hdmi_tx_swing_st *tx_swing=NULL;

    // check swing table
    // tv001
    tx_swing = &hdmi_tx_swing_dpc_8bit_table_tv001[TX_TIMING_NUM];
    if((tx_swing->r.em_en != 0xff) || (tx_swing->g.em_en != 0xff) || (tx_swing->r.em_en != 0xff) || (tx_swing->clk.em_en != 0xff)){
        FatalMessageHDMITx("[HDMITX][ERROR] FFE/SWING TABLE NOT MATCH@TV001_8Bit\n");
        return 0;
    }

    tx_swing = &hdmi_tx_swing_dpc_10bit_table_tv001[TX_TIMING_NUM];
    if((tx_swing->r.em_en != 0xff)|| (tx_swing->g.em_en != 0xff)|| (tx_swing->r.em_en != 0xff)|| (tx_swing->clk.em_en != 0xff)){
        FatalMessageHDMITx("[HDMITX][ERROR] FFE/SWING TABLE NOT MATCH@TV001_10Bit\n");
        return 0;
    }

    tx_swing = &hdmi_tx_swing_dpc_12bit_table_tv001[TX_TIMING_NUM];
    if((tx_swing->r.em_en != 0xff)|| (tx_swing->g.em_en != 0xff)|| (tx_swing->r.em_en != 0xff)|| (tx_swing->clk.em_en != 0xff)){
        FatalMessageHDMITx("[HDMITX][ERROR] FFE/SWING TABLE NOT MATCH@TV001_12Bit\n");
        return 0;
    }
#endif
    return 1;
}


void ScalerHdmiTxPhy_PowerOnDefaultSetting(void)
{
//    int value;
//    unsigned char ldo_ext=1;    // external LDO LV

    //HDMI 2.1 12G

    //H5X HDMITX Power-on Flow
    //[b80ba808][hdmitx_p]Bit 5:0 REG_MBIAS_RINT_CAL= 0x20
    //                    Bit8 =1 REG_MBIAS_POW = 1 MBIAS ENABLE
    //                                          => 1: enable
    //                    Bit12:20 REG_MBIAS =0x124
    //                    BIT <0>  ENVBGUP :Increase Bandgap Voltage =>0: Original
    //                    0: Original 1: Increase
    //                    BIT<2:1> RGB2 : bandgap voltage adjust=> b'10 (2)
    //                    BIT<5:3> RGBLOOP2 : bandgap voltage adjust=>b'100(4)
    //                    BIT<8:6> RGB :bandgap voltage adjust=>b'100(4)

    //REG_MBIAS_POW =>0x180ba808 Bit8 =1 REG_MBIAS_POW = 1 MBIAS ENABLE

    //rtd_part_outl(0xB80BA808,8,8,1);   // REG_MBIAS_POW =>0x180ba808 Bit8 =1 REG_MBIAS_POW = 1 MBIAS ENABLE

    FatalMessageHDMITx("[HDMITxPHY]PowerOnDefault Setting:GOLDENEXCEL_20240828_driver\n");

	HDMITx_Default_PHY_setting_2_func(0,0);
	HDMITx_Default_PHY_setting_3_func(0,0);



#ifdef OTP_TRIM_EN // PLL LDO
    if(OTP_TRIM_GET_TRIM_DONE(rtd_inl(EFUSE_DATAO_53_reg))){ // 0x180380D4[30]HDMI TX Trim Done Bit
        // 0x180380d4[24:23]HDMI TX PLL_LDO : PLL_LDO: 2 bit
        value = OTP_TRIM_GET_PLL_LDO_LV_SEL_SETTING(rtd_inl(EFUSE_DATAO_53_reg));
		// 0xB8047824 B[25:24]
		rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL09_reg, ~HDMITX_PHY_HDMITXPHY_CTRL09_reg_pll_ldo_lv_sel_mask, HDMITX_PHY_HDMITXPHY_CTRL09_reg_pll_ldo_lv_sel(value));
		DebugMessageHDMITx("[HDMITX] pll_ldo_lv_sel = %x\n", (UINT32)value);   
    } 
#endif

//#ifdef OTP_TRIM_EN // DCC, b8038148[24:23]
//    if(((UINT32)rtd_inl(EFUSE_DATAO_12_reg) & _BIT30) >> 30){ 
//        value = (frlType == _HDMI21_FRL_12G) ? (((UINT32)rtd_inl(EFUSE_DATAO_12_reg) & _BIT31) >> 31) : 0; // // 0xB8038030[31]
//    }else
//
//    value = 0x0;
//    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL01_reg, ~HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve5_mask, HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve5(value));//DCC
//#endif

#ifdef OTP_TRIM_EN // OTP setting
    if(OTP_TRIM_GET_TRIM_DONE(rtd_inl(EFUSE_DATAO_53_reg)))  {// 0x180380D4[31]HDMI TX Trim Done Bit
        value = OTP_TRIM_GET_P2S_LDO_SEL_SETTING(rtd_inl(EFUSE_DATAO_53_reg)); // 0x180380D4[27:25]HDMI TX P2S_LDO : P2S_LDO: 3 bit
    #ifdef HDMITX_SW_CONFIG		
        if(((Get_HDMITX_Resistor_Z0_Select() == _HDMITX_Resistor_Select_0ohm_AZ_TVS) || (Get_HDMITX_Resistor_Z0_Select() == _HDMITX_Resistor_Select_2ohm_AZ_TVS)) && (frlType == _HDMI21_FRL_12G))
            value = 0x4;
	#endif
		// 0xB804780C B[22:20]
		rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL03_reg, ~HDMITX_PHY_HDMITXPHY_CTRL03_reg_p2s_ldo_sel_mask, HDMITX_PHY_HDMITXPHY_CTRL03_reg_p2s_ldo_sel(value));
		DebugMessageHDMITx("[HDMITX] p2s ldo sel = %x\n", (UINT32)value);	
    }
#endif


#ifdef OTP_TRIM_EN // [DANBU-826]: OTP setting
    if(OTP_TRIM_GET_TRIM_DONE(rtd_inl(EFUSE_DATAO_53_reg))){ // 0x180380d4[31]HDMI TX Trim Done Bit
        value = OTP_TRIM_GET_Z0_SEL_SETTING(rtd_inl(EFUSE_DATAO_53_reg))| 0x8; // 0x180380d4[30:28]HDMI TX Z0 : Z0: 3 bit(B[2:0]), B[3] always=1
    #ifdef HDMITX_SW_CONFIG				
        if(Get_HDMITX_Resistor_Z0_Select() == _HDMITX_Resistor_Select_2ohm)
            value ++ ;
        else if((Get_HDMITX_Resistor_Z0_Select() == _HDMITX_Resistor_Select_0ohm_AZ_TVS) && (frlType == _HDMI21_FRL_12G))
            value -- ;
	#endif
		DebugMessageHDMITx("[HDMITX] TX Z0 sel = %x\n", (UINT32)value);

		// HDMITX_PHY_HDMITXPHY_CTRL18
		// 0xB8047848 B[31:16]
		rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL18_reg, ~HDMITX_PHY_HDMITXPHY_CTRL18_reg_tmds_rsela_mask, HDMITX_PHY_HDMITXPHY_CTRL18_reg_tmds_rsela(value));
		rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL18_reg, ~HDMITX_PHY_HDMITXPHY_CTRL18_reg_tmds_rselb_mask, HDMITX_PHY_HDMITXPHY_CTRL18_reg_tmds_rselb(value));
		rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL18_reg, ~HDMITX_PHY_HDMITXPHY_CTRL18_reg_tmds_rselc_mask, HDMITX_PHY_HDMITXPHY_CTRL18_reg_tmds_rselc(value));
		rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL18_reg, ~HDMITX_PHY_HDMITXPHY_CTRL18_reg_tmds_rselck_mask, HDMITX_PHY_HDMITXPHY_CTRL18_reg_tmds_rselck(value));	
    } 
#endif	
    return;
}

void ScalerHdmiTxPhy_PowerOff(void)
{
    //REG_TMDS_POW=0  => 0x180BA844 Bit28
    //													enable P2S & Mbias
    //rtd_part_outl(0xB80BA844,28,28,0);   //REG_TMDS_POW=0  => 0x180BA844 Bit28	 disble P2S & Mbias
    // HDMITX_PHY_HDMITXPHY_CTRL17
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL17_reg, ~HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_pow_mask, HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_pow(0x0));
	rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL17_reg, ~HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_powa_mask, HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_powa(0x0));
	rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL17_reg, ~HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_powb_mask, HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_powb(0x0));
	rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL17_reg, ~HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_powc_mask, HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_powc(0x0));
	rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL17_reg, ~HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_powck_mask, HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_powck(0x0));

	rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL09_reg, ~HDMITX_PHY_HDMITXPHY_CTRL09_reg_pll_pow_mask, HDMITX_PHY_HDMITXPHY_CTRL09_reg_pll_pow(0x0));// => 1: power on

    //PLLDISP
    //REG_PLLDISP_POW =>    0x180BA818	Bit4 =1
    //											CLK_TMDS ENABLE
    //											1: enable
    //											0: disable
    //rtd_part_outl(0xB80BA818,4,4,1);   //REG_PLLDISP_POW =>    0x180BA818	Bit4 =1 CLK_TMDS ENABLE

    // HDMITX_PHY_HDMITXPHY_CTRL06
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL06_reg, ~HDMITX_PHY_HDMITXPHY_CTRL06_reg_plldisp_pow_mask, HDMITX_PHY_HDMITXPHY_CTRL06_reg_plldisp_pow(0x0));
    //

    // disable TMDS clock
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL02_reg, ~HDMITX_PHY_HDMITXPHY_CTRL02_reg_hdmi_ck_en_mask, HDMITX_PHY_HDMITXPHY_CTRL02_reg_hdmi_ck_en(0));	 // REG_HDMI_CK_EN =>0x180ba808 Bit28 =1 REG_HDMI_CK_EN = 1 HDMI_CK ENABLE
    // disable mbias power
    //spec jing zhou
    //rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL02_reg, ~HDMITX_PHY_HDMITXPHY_CTRL02_reg_mbias_pow_mask, HDMITX_PHY_HDMITXPHY_CTRL02_reg_mbias_pow(0));	 // REG_MBIAS_POW =>0x180ba808 Bit8 =1 REG_MBIAS_POW = 1 MBIAS ENABLE
	// disable ldo_pow clock
	rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL24_reg, ~HDMITX_PHY_HDMITXPHY_CTRL24_reg_pll_ldo_pow_mask, HDMITX_PHY_HDMITXPHY_CTRL24_reg_pll_ldo_pow(0));
    return;
}



#ifdef HDMITX_TIMING_TABLE_CONFIG
EnumHdmi21FrlType ScalerHdmiTxPhy_GetPLLTimingInfo_frlRate(TX_TIMING_INDEX format_idx, I3DDMA_COLOR_DEPTH_T colorDepthIdx)
{
    EnumHdmi21FrlType frlType;
    pll_div_param_st *div_param=NULL;
    NoteMessageHDMITx("[HDMITX][BANK][7] GetPLLTimingInfo_frlRate() ...\n");

    if(format_idx > TX_TIMING_NUM){
        FatalMessageHDMITx("[HDMITX][ERROR] format_idx[%d] OVER RANGE[%d]@GetPLLTimingInfo_frlRate()\n", (UINT32)format_idx, (UINT32)TX_TIMING_NUM);
        return 0;
    }

    timing_gen = &cea_timing_table[format_idx];
    if((timing_gen->video_code == 0) || (get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE))
        frlType = ScalerHdmiTxGetTargetFrlRate();
    else {
        switch (colorDepthIdx) {
            case I3DDMA_COLOR_DEPTH_10B:
                div_param = &pll_div_deep_color_10bit_table[format_idx];
                frlType = div_param->frlRate;
                break;
            case I3DDMA_COLOR_DEPTH_12B:
                div_param = &pll_div_deep_color_12bit_table[format_idx];
                frlType = div_param->frlRate;
                break;
            default:
                div_param = &pll_div_deep_color_8bit_table[format_idx];
                frlType = div_param->frlRate;
                break;
        }
    }
    return frlType;
}
#endif


UINT8 ScalerHdmiTxPhy_GetCurrentFeeTableId(void)
{
    NoteMessageHDMITx("[HDMITX][BANK][7] GetCurrentFeeTableId() ...\n");
    return curFfeId;
}


void ScalerHdmiTxPhy_SetFeeTable(HDMI_TX_FFE_TABLE_LIST tableId)
{

    NoteMessageHDMITx("[HDMITX][BANK][7] Set FEE Table[%d]\n", (UINT32)tableId);
    curFfeId = tableId;

    hdmi_tx_FFE_table = (hdmi_tx_amp_fine_tune_st*)&hdmi_tx_FFE_table_TV001;

    return;
}



#ifdef NOT_USED_NOW
TX_TIMING_INDEX ScalerHdmiTxGetVideoFormatIndex_byParam(UINT32 width, UINT32 height, UINT8 isProg, UINT32 frame_rate, UINT8 bDscEn, UINT8 colorSpace)
{

    InfoMessageHDMITx("[HDMITX] GetIdxBy %dx%d%c%d, std=%d, Color=%d, DSC=%d\n",
            (UINT32)width, (UINT32)height, isProg? 'P': 'i', (UINT32)frame_rate, (UINT32)ScalerHdmiTxGetTimingStandardType(), (UINT32)colorSpace, (UINT32)bDscEn);

    if (height > 0 && height <= 484) { /* NTSC */
        if(width <= 640)
            return (ScalerHdmiTxGetTargetFrlRate() != 0? HDMI21_TX_640_480P_60HZ: HDMI_TX_640_480P_60HZ);
            if (width > 1920)
            goto resolution_4k2k;
        else if (width > 1280)
            goto resolution_1080;
        else if (width > 720)
            goto resolution_720p;

        return (!isProg ? HDMI_TX_720_1440_480I_60HZ : (ScalerHdmiTxGetTargetFrlRate() != 0? HDMI21_TX_720_480P_60HZ: HDMI_TX_720_480P_60HZ));
    } else if (height > 484 && height <= 576) { /* PAL */
            if (width > 1920)
            goto resolution_4k2k;
        else if (width > 1280)
            goto resolution_1080;
        else if (width > 720)
            goto resolution_720p;

        return (!isProg ? HDMI_TX_720_1440_576I_50HZ : (
            HDMI_TX_720_576P_50HZ));
    } else if (height > 576 && height <= 720) { /* 720P60 */
            if (width > 1920)
            goto resolution_4k2k;
        else if (width > 1280)
            goto resolution_1080;
        else if((width == 800) && (frame_rate > 600))
            return HDMI_TX_800_600P_72HZ;

resolution_720p:
        return (frame_rate < 590?  HDMI_TX_1280_720P_50HZ: (ScalerHdmiTxGetTargetFrlRate() != 0? HDMI21_TX_1280_720P_60HZ : HDMI_TX_1280_720P_60HZ));
    } else if (height > 720 && height <= 1088) { /* 1080I60 */
            if (width > 1920)
            goto resolution_4k2k;
resolution_1080:
        /* decide mode is 1080i50/1080i60 by mode and frame_rate variable */
        if(!isProg ){
            if(frame_rate < 590)
                return HDMI_TX_1920_1080I_50HZ;
            else
                return HDMI_TX_1920_1080I_60HZ;
        }else{ // decide mode is 1080p24/1080p48/1080p50/1080p60
            if(ScalerHdmiTxGetTargetFrlRate() != 0){ // HDMI2.1
                return (ScalerHdmiTxGetTargetFrlRate() == _HDMI21_FRL_3G? HDMI21_TX_1920_1080P_60HZ_3G: HDMI21_TX_1920_1080P_60HZ_6G4L);
            }else{
                if(frame_rate < 470)
                    return HDMI_TX_1920_1080P_24HZ;
                else if(frame_rate < 500)
                    return HDMI_TX_1920_1080P_48HZ;
                else if(frame_rate < 590)
                    return (
                    HDMI_TX_1920_1080P_50HZ);
                else
                    return (
                    HDMI_TX_1920_1080P_60HZ);
            }
        }
    } else if ((height > 1088) && (height <= 2160)) {
resolution_4k2k:
            if(!bDscEn){
            if(ScalerHdmiTxGetTargetFrlRate() != 0){ // HDMI 2.1
                if(frame_rate < 1190){ // HDMI2.1 4kp60
                    if(ScalerHdmiTxGetTargetFrlRate() == _HDMI21_FRL_12G){ // FRL rate=12G
                        return HDMI21_TX_3840_2160P_444_60HZ_12G;
                    }else if(ScalerHdmiTxGetTargetFrlRate() == _HDMI21_FRL_10G){
                        if(frame_rate < 480) // 47Hz
                            return (colorSpace == 3? HDMI21_TX_3840_2160P_420_47HZ_10G: HDMI21_TX_3840_2160P_444_47HZ_10G);
                        else if(frame_rate < 500) // 48Hz
                            return (colorSpace == 3? HDMI21_TX_3840_2160P_420_48HZ_10G: HDMI21_TX_3840_2160P_444_48HZ_10G);
                        else if(frame_rate < 590) // 50Hz
                            return (colorSpace == 3? HDMI21_TX_3840_2160P_420_50HZ_10G: HDMI21_TX_3840_2160P_444_50HZ_10G);
                        else
                            return (colorSpace == 3? HDMI21_TX_3840_2160P_420_60HZ_10G: HDMI21_TX_3840_2160P_444_60HZ_10G);
                    }else{ // FRL rate=6G
                        if(frame_rate < 480) // 47Hz
                            return (colorSpace == 3? HDMI21_TX_3840_2160P_420_47HZ_6G4L: HDMI21_TX_3840_2160P_444_47HZ_6G4L); // 0: RGB888, 1: YUV422, 2: YUV444, 3: YUV 420
                        else if(frame_rate < 500) // 48Hz
                            return (colorSpace == 3? HDMI21_TX_3840_2160P_420_48HZ_6G4L: HDMI21_TX_3840_2160P_444_48HZ_6G4L); // 0: RGB888, 1: YUV422, 2: YUV444, 3: YUV 420
                        else if(frame_rate < 590) // 50Hz
                            return (colorSpace == 3? HDMI21_TX_3840_2160P_420_50HZ_6G4L: HDMI21_TX_3840_2160P_444_50HZ_6G4L); // 0: RGB888, 1: YUV422, 2: YUV444, 3: YUV 420
                        else
                            return (colorSpace == 3? HDMI21_TX_3840_2160P_420_60HZ_6G4L: HDMI21_TX_3840_2160P_444_60HZ_6G4L); // 0: RGB888, 1: YUV422, 2: YUV444, 3: YUV 420
                    }
                }else{ // HDMI2.1 4kp120
                    return HDMI21_TX_3840_2160P_444_120HZ_8G;
                }
            }else{ // HDMI 2.0 4kp60
                if(colorSpace == 3){ // YUV420: // 0: RGB888, 1: YUV422, 2: YUV444, 3: YUV 420
                    return HDMI_TX_3840_2160P_420_60HZ;
                }else{ // YUV444
                    if(frame_rate < 470)
                        return (
                        HDMI_TX_3840_2160P_444_30HZ);
                    else if(frame_rate < 500)
                        return HDMI_TX_3840_2160P_444_48HZ;
                    else if(frame_rate < 590)
                        return (
                        HDMI_TX_3840_2160P_444_50HZ);
                    else
                        return (
                        HDMI_TX_3840_2160P_444_60HZ);
                }
            }
        }
    }
    else {
        DebugMessageHDMITx("[HDMITX] no suitable timing W(%d) H(%d) FR(%d) DSC(%d)\n", (UINT32)width, (UINT32)height, (UINT32)frame_rate, (UINT32)bDscEn);
    }

    return 0;
}
#endif // #ifdef NOT_USED_NOW



TX_TIMING_INDEX ScalerHdmiTxGetTx0VideoFormatIndex(void)
{
    TX_TIMING_INDEX format_idx;
    UINT16 width, height, frame_rate;
    UINT8 isProg, bDscEn,  colorBitIndex, colorSpace;
    width = g_stHdmiTx0InputTimingInfo.usHWidth;
    height = g_stHdmiTx0InputTimingInfo.usVHeight;
    isProg = !g_stHdmiTx0InputTimingInfo.b1Interlace;
    frame_rate = g_stHdmiTx0InputTimingInfo.usVFreq;
    bDscEn = GET_HDMI21_MAC_TX_DSCE_EN(_TX0);
    colorSpace = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_SPACE);
    colorBitIndex = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_DEPTH);

#ifdef HDMITX_TIMING_TABLE_CONFIG
#ifdef FIX_ME_HDMITX_BRING_UP
    if(width == 1920) {
	switch(ScalerHdmiTxGetTargetFrlRate())
		{
		case _HDMI21_FRL_3G:
			format_idx = HDMI21_TX_1920_1080P_60HZ_3G;
			break;
		case _HDMI21_FRL_6G_4LANES:
			format_idx = HDMI21_TX_1920_1080P_60HZ_6G4L;
			break;
		default:
			format_idx = HDMI_TX_1920_1080P_60HZ;
			break;
		}
    }
    else if(frame_rate > 1210) {
        switch(ScalerHdmiTxGetTargetFrlRate())
        	{
		case _HDMI21_FRL_10G:
			format_idx = HDMI21_TX_3840_2160P_444_144HZ_10G;
			break;
		case _HDMI21_FRL_12G:
			format_idx = HDMI21_TX_3840_2160P_444_144HZ_12G;
			break;
		default:
			format_idx = HDMI21_TX_3840_2160P_444_144HZ_12G;
			break;
        	}
    }
    else if(frame_rate > 610) {
        switch(ScalerHdmiTxGetTargetFrlRate())
        	{
		case _HDMI21_FRL_8G:
			format_idx = HDMI21_TX_3840_2160P_444_120HZ;
			break;
		case _HDMI21_FRL_12G:
			format_idx = HDMI21_TX_3840_2160P_444_120HZ;
			break;
		default:
			format_idx = HDMI21_TX_3840_2160P_444_120HZ;
			break;
        	}
    }
    else if(frame_rate > 310) {
        switch(ScalerHdmiTxGetTargetFrlRate())
        	{
		case _HDMI21_FRL_6G_4LANES:
			format_idx = HDMI21_TX_3840_2160P_444_60HZ_6G4L;
			break;
		case _HDMI21_FRL_12G:
			format_idx = HDMI21_TX_3840_2160P_444_60HZ_12G;
			break;
		default:
			format_idx = HDMI_TX_3840_2160P_444_60HZ;
			break;
        	}
    }
    else
        format_idx = HDMI_TX_3840_2160P_444_30HZ;
#else
    format_idx = ScalerHdmiTxGetVideoFormatIndex_byParam(width, height, TRUE, frame_rate, bDscEn, colorFmt);
#endif // #ifdef FIX_ME_HDMITX_BRING_UP
#endif
    format_idx = HDMITX_TIMING_INDEX;

    FatalMessageHDMITx("[HDMITX] FmtIdx1=%d\n", (UINT32)format_idx);
    FatalMessageHDMITx("[HDMITX] WxH=%dx%d\n", (UINT32)width, (UINT32)height);
    FatalMessageHDMITx("[HDMITX] FrameRate/DSC=%d/%d\n", (UINT32)frame_rate, (UINT32)bDscEn);
    FatalMessageHDMITx("[HDMITX] Color/BitIdx=%d/%d\n", (UINT32)colorSpace, (UINT32)colorBitIndex);
    FatalMessageHDMITx("[HDMITX] isProg=%d\n", (UINT32)isProg);
    return format_idx;
}



TX_TIMING_INDEX ScalerHdmiTxGetScalerVideoFormatIndex(void)
{
    TX_TIMING_INDEX format_idx;
    UINT8 bDscEn, colorFmt, colorBitIndex;
    UINT16 width, height, frame_rate;
    width = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_ACT_WID);
    height = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_ACT_LEN);
    frame_rate = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_VFREQ);
    colorFmt = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_SPACE);
    colorBitIndex = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_DEPTH);
    bDscEn = (GET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(_TX0) == _DOWN_STREAM_DSC? TRUE: FALSE);

#ifdef HDMITX_TIMING_TABLE_CONFIG
#ifdef FIX_ME_HDMITX_BRING_UP
    if(width == 1920) {
	switch(ScalerHdmiTxGetTargetFrlRate())
		{
		case _HDMI21_FRL_3G:
			format_idx = HDMI21_TX_1920_1080P_60HZ_3G;
			break;
		case _HDMI21_FRL_6G_4LANES:
			format_idx = HDMI21_TX_1920_1080P_60HZ_6G4L;
			break;
		default:
			format_idx = HDMI_TX_1920_1080P_60HZ;
			break;
		}
    }
    else if(frame_rate > 1210) {
        switch(ScalerHdmiTxGetTargetFrlRate())
        	{
		case _HDMI21_FRL_10G:
			format_idx = HDMI21_TX_3840_2160P_444_144HZ_10G;
			break;
		case _HDMI21_FRL_12G:
			format_idx = HDMI21_TX_3840_2160P_444_144HZ_12G;
			break;
		default:
			format_idx = HDMI21_TX_3840_2160P_444_144HZ_12G;
			break;
        	}
    }
    else if(frame_rate > 610) {
        switch(ScalerHdmiTxGetTargetFrlRate())
        	{
		case _HDMI21_FRL_8G:
			format_idx = HDMI21_TX_3840_2160P_444_120HZ;
			break;
		case _HDMI21_FRL_12G:
			format_idx = HDMI21_TX_3840_2160P_444_120HZ;
			break;
		default:
			format_idx = HDMI21_TX_3840_2160P_444_120HZ;
			break;
        	}
    }
    else if(frame_rate > 310) {
        switch(ScalerHdmiTxGetTargetFrlRate())
        	{
		case _HDMI21_FRL_6G_4LANES:
			format_idx = HDMI21_TX_3840_2160P_444_60HZ_6G4L;
			break;
		case _HDMI21_FRL_12G:
			format_idx = HDMI21_TX_3840_2160P_444_60HZ_12G;
			break;
		default:
			format_idx = HDMI_TX_3840_2160P_444_60HZ;
			break;
        	}
    }
    else
        format_idx = HDMI_TX_3840_2160P_444_30HZ;
#else
    format_idx = ScalerHdmiTxGetVideoFormatIndex_byParam(width, height, TRUE, frame_rate, bDscEn, colorFmt);
#endif // #ifdef FIX_ME_HDMITX_BRING_UP
#endif
    format_idx = HDMITX_TIMING_INDEX;

    DebugMessageHDMITx("[HDMITX] FmtIdx2=%d\n", (UINT32)format_idx);
    DebugMessageHDMITx("[HDMITX] WxH=%dx%d\n", (UINT32)width, (UINT32)height);
    DebugMessageHDMITx("[HDMITX] FrameRate/DSC=%d/%d\n", (UINT32)frame_rate, (UINT32)bDscEn);
    DebugMessageHDMITx("[HDMITX] Color/BitIdx=%d/%d\n", (UINT32)colorFmt, (UINT32)colorBitIndex);
    return format_idx;
}


void ScalerHdmiTxPhy_CalDispFVCO(void)
{
    UINT8 i = 0;
    UINT32 u32TmpValue = 0;
    UINT8 deep_depth = 0;
    UINT8 color_fmt = 0;
    UINT32 pixel_clk = 0;
    UINT32 disp_fvco = 0;
    UINT32 pll_temp_data = 0;
    UINT32 min_tolarence = CLK_594M;

    tmds_clk = 0;
    best_pixel_div = 0;
    is_over_600M = 0;

    // 1.get rx pixel clock(594M->59400M)
    //   tmds_clk: 8bit:x1 10bit:x1.25, 12bit:x1.5
    pixel_clk = (UINT32)hdmitxGetTxOutputPixelClock();//timing_info.pll_pixelclockx1024;

    color_fmt = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_SPACE);
    //if(color_fmt == HDMITX_COLOR_YUV420) { //yuv420
    //    pixel_clk = pixel_clk / 2;
    //}

    //hdmi21 2pixel mode
    if(pixel_clk > CLK_600M) {
        pixel_clk = pixel_clk / 2;
        is_over_600M = 1;
    } else if(GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() > 0) {
        FatalMessageHDMITx("[HDMITxPHY]2.0 repeat mode=%d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM());
        if(GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() == 1)
            pixel_clk = pixel_clk * 2;
        else
            FatalMessageHDMITx("[HDMITxPHY]2.0 below 12.5M,no support\n");
    } else if(GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) > 0) {
        FatalMessageHDMITx("[HDMITxPHY]2.1 repeat mode=%d\n", (UINT32)GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0));
        if(GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) == 1)
            pixel_clk = pixel_clk * 2;
        else
            FatalMessageHDMITx("[HDMITxPHY]2.1 below 12.5M,no support\n");
    }

    deep_depth = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_DEPTH);
    if(color_fmt == HDMITX_COLOR_YUV422) { //422 10/12bit pll equel 444 8bit
        tmds_clk = pixel_clk ;
    } else {
        switch (deep_depth) {
            case 1: //10bit
                tmds_clk = pixel_clk * 5UL /4;
                break;
            case 2: //12bit
                tmds_clk = pixel_clk * 3UL / 2;
                break;
            default:
                tmds_clk = pixel_clk ;
                break;
        }
    }

    FatalMessageHDMITx("[HDMITxPHY]PLL setting: pixel_clk/tmds_clk(%d/%d)\n ", (UINT32)pixel_clk, (UINT32)tmds_clk);

    // 2.decide PLLDISP_FVCO(432-891M)
    for(i = 0; i < DISP_PLL_DIV_TABLE_NUM; i++) {
        pll_temp_data = (tmds_clk / 10) * factor_div[i]; //tmds_clk/10 to void over UINT32

        if((pll_temp_data < (MIN_PLLDISP_FVCO/10)) || (pll_temp_data > (MAX_PLLDISP_FVCO/10))) {
            continue;
        }

       // find best pixel clock, most close to 594M
        if(pll_temp_data > CLK_594M/10) {
            u32TmpValue = pll_temp_data - CLK_594M/10;
        } else {
            u32TmpValue = CLK_594M/10 - pll_temp_data;
        }
        if(u32TmpValue < min_tolarence) {
            min_tolarence = u32TmpValue;
            best_pixel_div = i;
        }
    }
    if(min_tolarence == CLK_594M/10) {
        FatalMessageHDMITx("[HDMITxPHY]over 12G,need DSC mode\n");
        return;
    }

    u32TmpValue = (UINT32)factor_div[best_pixel_div];
    disp_fvco = u32TmpValue * tmds_clk; // disp_fvco max is 891000000,no over UINT32
    FatalMessageHDMITx("[HDMITxPHY]pixel_mul/disp_fvco(%d/%d)\n ", (UINT32)u32TmpValue, (UINT32)disp_fvco);

    // 3. cal disp_ncode(M+3)/disp_fcode(F)
    // 27M*(M+3+ F/2048)=  PLLDISP_FVCO
    u32TmpValue = disp_fvco / CLKIN_27M;
    pll_cal_param.disp.disp_ncode = (UINT16)u32TmpValue;
    u32TmpValue = disp_fvco - u32TmpValue * CLKIN_27M;
    u32TmpValue = u32TmpValue * 32UL / 421875; //f=tmp*2048/27M, only for no over UINT32
    pll_cal_param.disp.disp_fcode = (UINT16)u32TmpValue;

}
void ScalerHdmiTxPhy_CalDispDiv(void)
{
    UINT8 deep_depth = 0;
    UINT8 color_fmt = 0;
    DISP_PLL_DIV_PARAM *disp_pll_div = NULL;

    color_fmt = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_SPACE);//diff from REL1247
    deep_depth = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_DEPTH);//diff from REL1247
    // 4. decide n/k/tmds_div/pixel_div
    pll_cal_param.disp.Reserve9_clk_tmds_div = 0;
    pll_cal_param.disp.plldisp_n = 0;
    pll_cal_param.disp.plldisp_k = 0;

    if(ScalerHdmiTxGetOutputFrlType() ==0) {
        if((color_fmt == HDMITX_COLOR_YUV420) && !is_over_600M) // 4k60 420 pixel_clk=594M, tmds_clk=297M
            disp_pll_div = &disp_pll_div_hdmi20_420[0] + best_pixel_div;
        else if(GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() == 1)
            disp_pll_div = &disp_pll_div_hdmi20_repeat;
        else
            disp_pll_div = &disp_pll_div_hdmi20[0] + best_pixel_div;

        if(GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() == 1) {
            if(factor_div[best_pixel_div] == 16)
                pll_cal_param.disp.plldisp_k = 1;
            else if(factor_div[best_pixel_div] == 32)
                pll_cal_param.disp.plldisp_k = 3;
        } else if(factor_div[best_pixel_div] == 32)
            pll_cal_param.disp.plldisp_k = 1;

        pll_cal_param.disp.tmds_div = disp_pll_div->tmds_div;

    } else {
        if(is_over_600M && (color_fmt != HDMITX_COLOR_YUV420))
            disp_pll_div = &disp_pll_div_hdmi21_2p[0] + best_pixel_div;
        else if(!is_over_600M && (color_fmt == HDMITX_COLOR_YUV420))
            disp_pll_div = &disp_pll_div_hdmi21_420[0] + best_pixel_div;
        else if(GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) == 1)
            disp_pll_div = &disp_pll_div_hdmi21_repeat;
        else
            disp_pll_div = &disp_pll_div_hdmi21[0] + best_pixel_div;

        if(factor_div[best_pixel_div] == 16)
            pll_cal_param.disp.plldisp_k = 1;
        else if(factor_div[best_pixel_div] == 32)
            pll_cal_param.disp.plldisp_k = 3;

        pll_cal_param.disp.tmds_div = disp_pll_div->tmds_div;

    }

    switch (deep_depth) {
        case 1: //10bit
            pll_cal_param.disp.pix_div = disp_pll_div->pixel_10bit_div;
            break;
        case 2: //12bit
            pll_cal_param.disp.pix_div = disp_pll_div->pixel_12bit_div;
            break;
        default:
            pll_cal_param.disp.pix_div = disp_pll_div->pixel_8bit_div;
            break;
    }
    if(color_fmt == HDMITX_COLOR_YUV422) { //422 10/12bit pll equel 444 8bit
        pll_cal_param.disp.pix_div = disp_pll_div->pixel_8bit_div;
    }

    if(GET_HDMI21_MAC_TX_DSCE_EN(_TX0)) {
        pll_cal_param.disp.tmds_div = 0;
    }
    FatalMessageHDMITx("[HDMITxPHY]M/F(%d/%d) ",(UINT32)pll_cal_param.disp.disp_ncode, (UINT32)pll_cal_param.disp.disp_fcode);
    FatalMessageHDMITx("n/k(%d/%d) ",(UINT32)pll_cal_param.disp.plldisp_n, (UINT32)pll_cal_param.disp.plldisp_k);
    FatalMessageHDMITx("tmds_div/pixel_div(%d/%d)\n",(UINT32)pll_cal_param.disp.tmds_div, (UINT32)pll_cal_param.disp.pix_div);
}

void ScalerHdmiTxPhy_CalDispPLL(void)
{
    ScalerHdmiTxPhy_CalDispFVCO();
    ScalerHdmiTxPhy_CalDispDiv();
}

void ScalerHdmiTxPhy_CalTmdsPLL(void)
{
    UINT8 color_fmt = 0;
    UINT8 m1=1;
    UINT32 clk_tmds_phy = 0;

    color_fmt = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_SPACE);//diff from REL1247
    // 5. tmds pll setting
    if(ScalerHdmiTxGetOutputFrlType() ==0) {
        pll_cal_param.tmds.pll_ckin_sel = 0;
        pll_cal_param.tmds.pll_divnsel = 0;
        pll_cal_param.tmds.hdmi21_mode = 0;
    } else {
        pll_cal_param.tmds.pll_ckin_sel = 1;
        pll_cal_param.tmds.pll_divnsel = 1;
        pll_cal_param.tmds.hdmi21_mode = 1;
    }

    tmds_pll = &tmds_pll_setting[0] + ScalerHdmiTxGetOutputFrlType();
    pll_cal_param.tmds.tmds_ncode = tmds_pll->tmds_ncode;
    pll_cal_param.tmds.tmds_fcode = tmds_pll->tmds_fcode;

    pll_cal_param.tmds.pll_m1 = 0;
    if(ScalerHdmiTxGetOutputFrlType() ==0) {
        if((color_fmt == HDMITX_COLOR_YUV420) && !is_over_600M) // 4k60 420 pixel_clk=594M, tmds_clk=297M
            clk_tmds_phy = tmds_clk / 2;
        else
            clk_tmds_phy = tmds_clk;
        while((UINT32)m1*clk_tmds_phy < MIN_TMDS_FVCO/10) {
            m1 = m1 * 2;
            pll_cal_param.tmds.pll_m1 ++ ;
        }
        pll_cal_param.tmds.pll_m2 = 0;
        pll_cal_param.tmds.Reserve6_pllm1b2 = 0;
        if((UINT32)m1*clk_tmds_phy >= TMDS_VCOGAIN/10) //over 3G,vcogain=1
            pll_cal_param.tmds.pll_vcogain = 1;
        else
            pll_cal_param.tmds.pll_vcogain = 0;
    } else {
        pll_cal_param.tmds.pll_m1 = 0;
        pll_cal_param.tmds.pll_m2 = 0;
        pll_cal_param.tmds.Reserve6_pllm1b2 = 0;
        pll_cal_param.tmds.pll_vcogain = 1;
    }

    FatalMessageHDMITx("[HDMITxPHY]tmds pll M/F(%d/%d), ",(UINT32)pll_cal_param.tmds.tmds_ncode, (UINT32)pll_cal_param.tmds.tmds_fcode);
    FatalMessageHDMITx("hdmi21_mode=%d, pll_ckin_sel=%d, ",(UINT32)pll_cal_param.tmds.hdmi21_mode, (UINT32)pll_cal_param.tmds.pll_ckin_sel);
    FatalMessageHDMITx("pll_divnsel=%d, Reserve6_pllm1b2=%d, ",(UINT32)pll_cal_param.tmds.pll_divnsel, (UINT32)pll_cal_param.tmds.Reserve6_pllm1b2);
    FatalMessageHDMITx("M1/M2(%d/%d), ",(UINT32)pll_cal_param.tmds.pll_m1, (UINT32)pll_cal_param.tmds.pll_m2);
    FatalMessageHDMITx("pll_vcogain=%d\n",(UINT32)pll_cal_param.tmds.pll_vcogain);

}


void ScalerHdmiTxPhy_CalClkRatio(void)
{
    UINT8 color_fmt = 0;
    color_fmt = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_SPACE);//diff from REL1247
    // 6. clk ratio setting
    if(ScalerHdmiTxGetOutputFrlType() ==0) {
        if(color_fmt == HDMITX_COLOR_YUV420) {
            if(is_over_600M) {
                pll_cal_param.tmds.mac_div = 0;
                pll_cal_param.dsc_clkratio.txsoc = 4;
                pll_cal_param.dsc_clkratio.txsoc_420 = 4;
            } else {
                    pll_cal_param.tmds.mac_div = 4;
                    pll_cal_param.dsc_clkratio.txsoc = 6;
                    pll_cal_param.dsc_clkratio.txsoc_420 = 6;
            }

        } else {
            pll_cal_param.tmds.mac_div = 0;
            pll_cal_param.dsc_clkratio.txsoc = 4;
            pll_cal_param.dsc_clkratio.txsoc_420 = 4;
        }

        pll_cal_param.dsc_clkratio.dsce_cclk = 0;
        pll_cal_param.dsc_clkratio.dsce_pclk = 0;
        pll_cal_param.dsc_clkratio.dsce_sclk = 0;
    } else {
        if(is_over_600M && (color_fmt != HDMITX_COLOR_YUV420)) {
            pll_cal_param.tmds.mac_div = 4;
            pll_cal_param.dsc_clkratio.txsoc = 0;
            pll_cal_param.dsc_clkratio.txsoc_420 = 4;
         } else if(!is_over_600M && (color_fmt == HDMITX_COLOR_YUV420)) {
            pll_cal_param.tmds.mac_div = 7;
            pll_cal_param.dsc_clkratio.txsoc = 6;
            pll_cal_param.dsc_clkratio.txsoc_420 = 7;
        } else {
            pll_cal_param.tmds.mac_div = 6;
            pll_cal_param.dsc_clkratio.txsoc = 4;
            pll_cal_param.dsc_clkratio.txsoc_420 = 6;
        }

        pll_cal_param.dsc_clkratio.dsce_cclk = 0;
        pll_cal_param.dsc_clkratio.dsce_pclk = 0;
        pll_cal_param.dsc_clkratio.dsce_sclk = 0;
    }
    if(GET_HDMI21_MAC_TX_DSCE_EN(_TX0)) {
        UINT8 dsc_num = HDMITX_TIMING_INDEX - TX_TIMING_DSCE_START;
        if((HDMITX_TIMING_INDEX < TX_TIMING_DSCE_START) || (dsc_num >= DSCE_TIMING_NUM)) {
            FatalMessageHDMITx("[HDMITX] dsce timing index fail[%d, %d]\n", HDMITX_TIMING_INDEX, TX_TIMING_DSCE_START);
            return;
        }
        pll_cal_param.tmds.mac_div = 0;
        pll_cal_param.dsc_clkratio.txsoc = dsc_clk_ratio_setting[dsc_num].txsoc;
        pll_cal_param.dsc_clkratio.txsoc_420 = dsc_clk_ratio_setting[dsc_num].txsoc_420;
        pll_cal_param.dsc_clkratio.dsce_cclk = dsc_clk_ratio_setting[dsc_num].dsce_cclk;
        pll_cal_param.dsc_clkratio.dsce_pclk = dsc_clk_ratio_setting[dsc_num].dsce_pclk;
        pll_cal_param.dsc_clkratio.dsce_sclk = dsc_clk_ratio_setting[dsc_num].dsce_sclk;
    }
    pll_cal_param.frlRate = ScalerHdmiTxGetOutputFrlType();

    FatalMessageHDMITx("[HDMITxPHY]clkratio mac_div=%d, txsoc=%d, ",(UINT32)pll_cal_param.tmds.mac_div, (UINT32)pll_cal_param.dsc_clkratio.txsoc);
    FatalMessageHDMITx("txsoc_420=%d, dsce_sclk=%d, ",(UINT32)pll_cal_param.dsc_clkratio.txsoc_420, (UINT32)pll_cal_param.dsc_clkratio.dsce_sclk);
    FatalMessageHDMITx("dsce_cclk=%d, dsce_pclk=%d \n",(UINT32)pll_cal_param.dsc_clkratio.dsce_cclk, (UINT32)pll_cal_param.dsc_clkratio.dsce_pclk);

}


void ScalerHdmiTxPhy_SetPLL(void)
{
    UINT8 value=0;
    UINT8 dsc_en;
    TX_TIMING_INDEX format_idx;
    EnumHdmi21FrlType frlType;
    UINT32 pixel_clk = 0;
	HDMI20TX_VALUE_PARAM_T value_tmds;
	HDMI21TX_VALUE_PARAM_T value_frl;
    //UINT8 deep_depth = 0;
#ifndef CONFIG_MACH_RTD2801_ZEBU
 //   UINT8 timeout =OC_DONE_TIMEOUT;
#endif
    //deep_depth = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_DEPTH);
    //NoteMessageHDMITx("[HDMITX][BANK][7] ScalerHdmiTxPhy_SetPLL\n");
    FatalMessageHDMITx("[HDMITX][BANK][7] Bryan 1011  ScalerHdmiTxPhy_SetPLL\n");

    if((GET_HDMI_MAC_TX0_MODE_STATE() != _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON) && (GET_HDMI_MAC_TX0_MODE_STATE() != _HDMI_TX_MODE_STATUS_LINK_ON))
    {
        dsc_en = GET_HDMI21_MAC_TX_DSCE_EN(_TX0);
        format_idx = ScalerHdmiTxGetScalerVideoFormatIndex();
    }else{
        dsc_en = GET_HDMI21_MAC_TX_DSCE_EN(_TX0);
        format_idx = ScalerHdmiTxGetTx0VideoFormatIndex();
    }
    //deep_depth = ScalerDpStreamGetElement(_P0_OUTPUT_PORT, _COLOR_DEPTH);
    //deep_color = (deep_depth > 8? 1: 0);

    if(format_idx >= TX_TIMING_NUM){
        FatalMessageHDMITx("[HDMITX][ERROR] format_idx[%d] OVER RANGE[%d]@ScalerHdmiTxPhy_SetPLL()\n", (UINT32)format_idx, (UINT32)TX_TIMING_NUM);
        return;
    }

#ifdef HDMITX_TIMING_TABLE_CONFIG
    timing_gen = &cea_timing_table[format_idx];
    if((timing_gen->video_code == 0) || (get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE)) {
        ScalerHdmiTxPhy_CalDispPLL();
        ScalerHdmiTxPhy_CalTmdsPLL();
        ScalerHdmiTxPhy_CalClkRatio();
        div_param = &pll_cal_param;
    } else {
        switch (deep_depth) {
            case 1: //10bit
                div_param = &pll_div_deep_color_10bit_table[0] + format_idx;
                //analog_param = &pll_analog_deep_color_10bit_table[0] + format_idx;
                break;
            case 2: //12bit
                div_param = &pll_div_deep_color_12bit_table[0] + format_idx;
                //analog_param = &pll_analog_deep_color_12bit_table[0] + format_idx;
                break;
            default:
                div_param = &pll_div_deep_color_8bit_table[0] + format_idx;
                //analog_param = &pll_analog_deep_color_8bit_table[0] + format_idx;
                break;
        }
    }

    frlType = ScalerHdmiTxGetOutputFrlType();
    colorSpace = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_SPACE);
    pixelFreq = (timing_gen->pixelFreq / (colorSpace == 3? 2: 1)); // pixelFreq /=2 in 420 mode(colorSpace=3)
    FatalMessageHDMITx("[HDMITX] SetPLL, Idx=%d(/%d)\n", (UINT32)format_idx, (UINT32)TX_TIMING_NUM);
    FatalMessageHDMITx("[HDMITX] VIC=%d\n", (UINT32)timing_gen->video_code);
    FatalMessageHDMITx("[HDMITX] Color/Bit=%d/%d\n", (UINT32)colorSpace, (UINT32)deep_depth);
    InfoMessageHDMITx("[HDMITX] DSC/FRL=%d/%d\n", (UINT32)dsc_en, (UINT32)frlType);
    FatalMessageHDMITx("[HDMITX] Pixel/FRL=%d/%d\n", (UINT32)pixelFreq, (UINT32)div_param->frlRate);
#else
    frlType = ScalerHdmiTxGetOutputFrlType();
	FatalMessageHDMITx("HDMITX_TIMING_INDEX=%d\n",(UINT32)HDMITX_TIMING_INDEX);
    ScalerHdmiTxPhy_CalDispPLL();
    ScalerHdmiTxPhy_CalTmdsPLL();
    ScalerHdmiTxPhy_CalClkRatio();
    div_param = &pll_cal_param;
#endif

    // HDMITX_PHY_HDMITXPHY_CTRL06
    //rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL06_reg, ~HDMITX_PHY_HDMITXPHY_CTRL06_reg_plldisp_pow_mask, HDMITX_PHY_HDMITXPHY_CTRL06_reg_plldisp_pow(0x1));

//Golden excel: HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg_pi_cur_sel_disp already set 3
    // [DANBU-514] For FT TMDS 1.485G Need Enlarge PI BW (apply to all timing)
    //rtd_maskl(HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg, ~HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg_pi_cur_sel_disp_mask, HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg_pi_cur_sel_disp(0x3));
    //rtd_maskl(HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg, ~HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg_en_pi_debug_disp_mask, HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg_en_pi_debug_disp(0x0));


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //*****PLLDISP M/N Setting
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // PLL DISP target M code
    // HDMITX_PHY_HDMITX_DISP_PLL_SSC1

    //rtd_maskl(HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg, ~HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg_oc_en_disp_mask, HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg_oc_en_disp(0x0));
    rtd_maskl(HDMITX_PHY_HDMITX_DISP_PLL_SSC1_reg, ~HDMITX_PHY_HDMITX_DISP_PLL_SSC1_reg_ncode_t_ssc_disp_mask, HDMITX_PHY_HDMITX_DISP_PLL_SSC1_reg_ncode_t_ssc_disp((div_param->disp.disp_ncode)-3));

    //// PLL DISP target F code
    //HDMITX_PHY_HDMITX_DISP_PLL_SSC0
    rtd_maskl(HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg, ~HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg_fcode_t_ssc_disp_mask, HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg_fcode_t_ssc_disp(div_param->disp.disp_fcode));

    // [RTKREQ-729] fix 640x480p60 set fcode_t_ssc_disp fail
    rtd_maskl(HDMITX_PHY_HDMITX_DISP_SSC3_reg, ~HDMITX_PHY_HDMITX_DISP_SSC3_reg_ncode_ssc_disp_mask, HDMITX_PHY_HDMITX_DISP_SSC3_reg_ncode_ssc_disp((div_param->disp.disp_ncode)-3));
    rtd_maskl(HDMITX_PHY_HDMITX_DISP_SSC3_reg, ~HDMITX_PHY_HDMITX_DISP_SSC3_reg_fcode_ssc_disp_mask, HDMITX_PHY_HDMITX_DISP_SSC3_reg_fcode_ssc_disp(div_param->disp.disp_fcode));

    value = 0;//(div_param->disp.disp_fcode)? 0: 1;//if DTG FLL tracking, bypass_pi_disp always=0
    rtd_maskl(HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg, ~HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg_bypass_pi_disp_mask, HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg_bypass_pi_disp(value));

    //rtd_maskl(HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg, ~HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg_oc_en_disp_mask, HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg_oc_en_disp(0x1));

    //udelay(10);

    // [H5XBU-210] Reset DISP PLL after Set SSC N/F code
    //PLLDISP_VCORSTB   =>  0x180BA800 Bit24 = 1 PLL_VCO reset control  => 1: active
    //rtd_part_outl(0xB80BA800,24,24,0x1);
    // HDMITX_PHY_HDMITXPHY_CTRL00
    //Set by Golden Excel
    //rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL07_reg, ~HDMITX_PHY_HDMITXPHY_CTRL07_reg_plldisp_vcorstb_mask, HDMITX_PHY_HDMITXPHY_CTRL07_reg_plldisp_vcorstb(0x1));// => 1: active

    //REG_PLLDISP_RSTB   => 0x180BA81C Bit16 = 1
    //                                                          PLLDISP RSTB =0
    //                                                                  0: reset
    //                                                                  1: active
    //REG_PLLDISP_RSTB   => 0x180BA81C Bit16 = 1 PLLDISP  => 1: active
    //rtd_part_outl(0xB80BA81C,16,16,0x1);
    // HDMITX_PHY_HDMITXPHY_CTRL07
    //Set by Golden Excel    
    //rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL07_reg, ~HDMITX_PHY_HDMITXPHY_CTRL07_reg_plldisp_rstb_mask, HDMITX_PHY_HDMITXPHY_CTRL07_reg_plldisp_rstb(0x1));//	 1: active

    //*****delay > 20Us
    //ScalerTimerDelayXms(2);
    //udelay(10);

    // check until oc_done=1
//#ifndef CONFIG_MACH_RTD2801_ZEBU
//    timeout =OC_DONE_TIMEOUT;
//    while(timeout--){
//        if(rtd_inl(HDMITX_PHY_HDMITX_DISP_SSC5_reg) & HDMITX_PHY_HDMITX_DISP_SSC5_reg_oc_done_disp_mask)
//            break;
//        if(timeout == 0){
//            FatalMessageHDMITx("[HDMITX][ERROR] Set SSC oc_en_disp Timeout[%d]!!@SetPLL\n", (UINT32)OC_DONE_TIMEOUT);//normal situation
//            break;
//        }
//        HDMITX_MDELAY(1);
//    }
//#endif // #ifndef CONFIG_MACH_RTD2801_ZEBU
//#ifdef TEST_WAIT_OC_DONE
//    if(timeout < 0){
//    while((rtd_inl(0xb8005040) & _BIT16) == 0)
//        HDMITX_MDELAY(100);
//    }
//#endif

    //rtd_maskl(HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg, ~HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg_oc_en_disp_mask, HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg_oc_en_disp(0x0));

    //REG_PLLDISP_N  => 0x180BA818 Bit<22:20> [2:0] PLLDISP M divider , divided ratio => 0
    // HDMITX_PHY_HDMITXPHY_CTRL06
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL06_reg, ~HDMITX_PHY_HDMITXPHY_CTRL06_reg_plldisp_n_mask, HDMITX_PHY_HDMITXPHY_CTRL06_reg_plldisp_n(div_param->disp.plldisp_n));

    //REG_PLLDISP_IP  => 0x180BA814 Bit<3:0> PLLDISP CP Current selection,from 0000 to 1111  => 0x2  Bryan ???
    // HDMITX_PHY_HDMITXPHY_CTRL05
    FatalMessageHDMITx("[HDMITxPHY]frlType=%d\n",(UINT32)frlType);

    if(frlType == 0) {
        pixel_clk = (UINT32)hdmitxGetTxOutputPixelClock();//timing_info.pll_pixelclockx1024;
        if(pixel_clk <= TMDS_50M)
            value_tmds = HDMI20TX_PHY_480P_8bit_270M;// <50M(27M)
        else if((pixel_clk > TMDS_50M) && (pixel_clk <= TMDS_100M))
            value_tmds = HDMI20TX_PHY_720P_8bit_742M;//50M-100M(74.25M)
        else if((pixel_clk > TMDS_100M) && (pixel_clk <= TMDS_200M))
            value_tmds = HDMI20TX_PHY_1080P_8bit_1p5G;//100M-200M(148.5M)
        else if((pixel_clk > TMDS_200M) && (pixel_clk <= TMDS_400M))
            value_tmds = HDMI20TX_PHY_4K30_8bit_3G;//200M-400M(297M)
        else
            value_tmds = HDMI20TX_PHY_4K60_8bit_6G;//>400M(594M)  
        FatalMessageHDMITx("[HDMITxPHY]DISP_PLL=%d\n",(UINT32)value_tmds);
        HDMI20Tx_DISP_PLL_setting_2_func(0,value_tmds);//tmds
        HDMI20Tx_DISP_PLL_setting_3_func(0,value_tmds);//tmds        
    } else {
        switch(frlType){
          case _HDMI21_FRL_12G:
            value_frl = HDMI21TX_PHY_4K60_12G;
            break;
          case _HDMI21_FRL_10G:
            value_frl = HDMI21TX_PHY_4K60_10G;
            break;
          case _HDMI21_FRL_8G:
            value_frl = HDMI21TX_PHY_4K60_8G;
            break;
          case _HDMI21_FRL_6G_4LANES:
            value_frl = HDMI21TX_PHY_4K60_6G;
            break;
          case _HDMI21_FRL_6G_3LANES://no exist in golden excel case
            value_frl = HDMI21TX_PHY_4K60_6G;
            break;
          case _HDMI21_FRL_3G:
          default:
            value_frl = HDMI21TX_PHY_2K60_3G3L;
            break;
        }
        FatalMessageHDMITx("[HDMITxPHY]DISP_PLL=%d\n",(UINT32)value_frl);
        HDMI21Tx_DISP_PLL_setting_2_func(0,value_frl);//frl
        HDMI21Tx_DISP_PLL_setting_3_func(0,value_frl);//frl        
    }
    //REG_PLLDISP_K  => 0x180BA818 Bit<29:28> REG_PLLDISP_K[2:0] , divided ratio => 00: /1
    // HDMITX_PHY_HDMITXPHY_CTRL06
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL06_reg, ~HDMITX_PHY_HDMITXPHY_CTRL06_reg_plldisp_k_mask, HDMITX_PHY_HDMITXPHY_CTRL06_reg_plldisp_k(div_param->disp.plldisp_k));// divided ratio => 00: /1

    // HDMITX_PHY_HDMITXPHY_CTRL03
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL03_reg, ~HDMITX_PHY_HDMITXPHY_CTRL03_reg_pixel_div_mask, HDMITX_PHY_HDMITXPHY_CTRL03_reg_pixel_div(div_param->disp.pix_div));

    //REG_HDMITX_RESERVE[9]  => 0x180BA804 Bit<9> //1.25 divider for CLK_TMDS path => 0: divider by 1
    //rtd_part_outl(0xB80BA804,9, 9,0x0);  // Bryan 20190212
    // HDMITX_PHY_HDMITXPHY_CTRL01
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL01_reg, ~HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve9_mask, HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve9(div_param->disp.Reserve9_clk_tmds_div));


    // HDMITX_PHY_HDMITXPHY_CTRL12
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL12_reg, ~HDMITX_PHY_HDMITXPHY_CTRL12_reg_tmds_div_mask, HDMITX_PHY_HDMITXPHY_CTRL12_reg_tmds_div(div_param->disp.tmds_div));// 0x1 => : /2

    //*****delay > 20Us
    //ScalerTimer_DelayXms(2);
    //udelay(200);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //*****PLLDTMDS M/N Setting
    //
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // HDMITX_PHY_HDMITXPHY_CTRL01
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL01_reg, ~HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve16_mask, HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve16(frlType == _HDMI21_FRL_3G? 1: 0));
//DCC Control
//#ifdef OTP_TRIM_EN // [DANBU-826]: OTP setting
//    if(((UINT32)rtd_inl(EFUSE_DATAO_12_reg) & _BIT30) >> 30){ // 0xB8038030[30]
//        value = (frlType == _HDMI21_FRL_12G) ? (((UINT32)rtd_inl(EFUSE_DATAO_12_reg) & _BIT31) >> 31) : 0; // // 0xB8038030[31]
//    }else
//#endif
//    value = 0x0;
//    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL01_reg, ~HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve5_mask, HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve5(value));
    
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL09_reg, ~HDMITX_PHY_HDMITXPHY_CTRL09_reg_pll_m1_mask, HDMITX_PHY_HDMITXPHY_CTRL09_reg_pll_m1(div_param->tmds.pll_m1));

    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL09_reg, ~HDMITX_PHY_HDMITXPHY_CTRL09_reg_pll_m2_mask, HDMITX_PHY_HDMITXPHY_CTRL09_reg_pll_m2(div_param->tmds.pll_m2));

    rtd_maskl(HDMITX_PHY_HDMITX_TMDS_CTRL0_reg, ~HDMITX_PHY_HDMITX_TMDS_CTRL0_reg_ncode_t_tmds_mask, HDMITX_PHY_HDMITX_TMDS_CTRL0_reg_ncode_t_tmds((div_param->tmds.tmds_ncode)-3));
    rtd_maskl(HDMITX_PHY_HDMITX_TMDS_CTRL0_reg, ~HDMITX_PHY_HDMITX_TMDS_CTRL0_reg_fcode_t_tmds_mask, HDMITX_PHY_HDMITX_TMDS_CTRL0_reg_fcode_t_tmds(div_param->tmds.tmds_fcode));
 

//#ifndef CONFIG_MACH_RTD2801_ZEBU
//    // check until oc_done=1
//    timeout=OC_DONE_TIMEOUT;
//    while(timeout--){
//        if(rtd_inl(HDMITX_PHY_HDMITX_TMDS_CTRL2_reg) & HDMITX_PHY_HDMITX_TMDS_CTRL2_reg_oc_done_tmds_mask)
//            break;
//        if(timeout == 0){
//            DebugMessageHDMITx("[HDMITX][ERROR] Set TMDS oc_en_tmds Timeout@SetPLL!!\n");
//            break;
//        }
//        //msleep(1);
//        HDMITX_MDELAY(2);//delay3		
//    }
//#endif // #ifndef CONFIG_MACH_RTD2801_ZEBU
//#ifdef TEST_WAIT_OC_DONE
//    if(timeout < 0){
//    while((rtd_inl(0xb8005040) & _BIT16) == 0)
//        HDMITX_MDELAY(100);
//    }
//#endif

    // HDMITX_CLK_DIV setting
    rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_pixel_mac_mask, HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_pixel_mac(div_param->tmds.mac_div));

    if(div_param->tmds.hdmi21_mode && dsc_en){
        rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_hdmitx_ds444_2p_mask, HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_hdmitx_ds444_2p(div_param->dsc_clkratio.txsoc));
        rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_hdmitx_4p_2p_mask, HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_hdmitx_4p_2p(div_param->dsc_clkratio.txsoc_420));
        rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_dsce_s_mask, HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_dsce_s(div_param->dsc_clkratio.dsce_sclk));
        rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_dsce_c_mask, HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_dsce_c(div_param->dsc_clkratio.dsce_cclk));
        rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_dsce_p_mask, HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_dsce_p(div_param->dsc_clkratio.dsce_pclk));
    }else
    { // DSCE disable
        rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_hdmitx_ds444_2p_mask, HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_hdmitx_ds444_2p(div_param->dsc_clkratio.txsoc));
        rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_hdmitx_4p_2p_mask, HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_hdmitx_4p_2p(div_param->dsc_clkratio.txsoc_420));
    }

    // clkratio_active
    rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_active_mask, HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_active(1));
    HDMITX_drv_set_hdmitx_gate_sel();

    if(frlType == 0) {
        HDMI20Tx_TMDS_PLL_setting_2_func(0,value_tmds);//tmds
        HDMI20Tx_TMDS_PLL_setting_3_func(0,value_tmds);//tmds        
    } else {
        HDMI21Tx_TMDS_PLL_setting_2_func(0,value_frl);//frl
        HDMI21Tx_TMDS_PLL_setting_3_func(0,value_frl);//frl        
    }


    return;
}

void ScalerHdmiTxPhy_CalSwing(void)
{
	HDMI20TX_VALUE_PARAM_T tx_cal_swing_tmds=HDMI20TX_PHY_480P_8bit_270M;
	HDMI21TX_VALUE_PARAM_T tx_cal_swing_frl=HDMI21TX_PHY_2K60_3G3L;	
    UINT8 deep_depth = 0;
    UINT8 color_fmt = 0;
    EnumHdmi21FrlType frlType;
    UINT32 pixel_clk = 0;
    frlType = ScalerHdmiTxGetOutputFrlType();
    color_fmt = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_SPACE);//diff from RLE1247

    if(frlType == 0) {//HDMI20
        pixel_clk = (UINT32)hdmitxGetTxOutputPixelClock();//timing_info.pll_pixelclockx1024;
        deep_depth = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_DEPTH);
        if(color_fmt == HDMITX_COLOR_YUV422) { //422 10/12bit pll equel 444 8bit
            tmds_clk = pixel_clk ;
        } else {
            switch (deep_depth) {
                case 1: //10bit
                    tmds_clk = pixel_clk * 5UL /4;
                    break;
                case 2: //12bit
                    tmds_clk = pixel_clk * 3UL / 2;
                    break;
                default:
                    tmds_clk = pixel_clk ;
                    break;
            }
        }
		if(tmds_clk <= TMDS_50M)
			tx_cal_swing_tmds = HDMI20TX_PHY_480P_8bit_270M;//<50M	(27M)	
		else if(tmds_clk <= TMDS_100M)
			tx_cal_swing_tmds = HDMI20TX_PHY_720P_8bit_742M;// 50M~100M(74.25M)
        else if((tmds_clk > TMDS_100M) && (tmds_clk <= TMDS_200M))
			tx_cal_swing_tmds = HDMI20TX_PHY_1080P_8bit_1p5G;//100M-200M(148.5M)
        else if((tmds_clk > TMDS_200M) && (tmds_clk <= TMDS_400M))
			tx_cal_swing_tmds = HDMI20TX_PHY_4K30_8bit_3G;//200M-400M(297M)
        else
			tx_cal_swing_tmds = HDMI20TX_PHY_4K60_8bit_6G;//>400M(594M)		
        FatalMessageHDMITx("[HDMITxPHY]tx_cal_swing_tmds=%d\n",(UINT32)tx_cal_swing_tmds);
        HDMI20Tx_PHY_Output_setting_1_func(0,tx_cal_swing_tmds);	
    } else {//HDMI21
#ifdef TVS_Support
    #ifdef HDMITX_SW_CONFIG
        if((Get_HDMITX_Resistor_Z0_Select() == _HDMITX_Resistor_Select_0ohm_AZ_TVS) || (Get_HDMITX_Resistor_Z0_Select() == _HDMITX_Resistor_Select_2ohm_AZ_TVS))
            tx_cal_swing_frl = HDMI21TX_PHY_4K60_12G_TVS;
	#endif
	else{
#endif
	      switch(frlType){		  	
	          case _HDMI21_FRL_12G:
	            tx_cal_swing_frl = HDMI21TX_PHY_4K60_12G;
	            break;
	          case _HDMI21_FRL_10G:
	            tx_cal_swing_frl = HDMI21TX_PHY_4K60_10G;
	            break;
	          case _HDMI21_FRL_8G:
	            tx_cal_swing_frl = HDMI21TX_PHY_4K60_8G;
	            break;
	          case _HDMI21_FRL_6G_4LANES:
	            tx_cal_swing_frl = HDMI21TX_PHY_4K60_6G;
	            break;
	          case _HDMI21_FRL_6G_3LANES://no exist in golden excel case
	            tx_cal_swing_frl = HDMI21TX_PHY_4K60_6G;
	            break;
	          case _HDMI21_FRL_3G:
	          default:
	            tx_cal_swing_frl = HDMI21TX_PHY_2K60_3G3L;
	            break;
    }
#ifdef TVS_Support
	  }
    #endif

        FatalMessageHDMITx("[HDMITxPHY]tx_cal_swing_frl=%d\n",(UINT32)tx_cal_swing_frl);
        HDMI21Tx_PHY_Output_setting_1_func(0,tx_cal_swing_frl);
	}
}

void ScalerHdmiTxPhy_SetSwing(void)
{
//no use beg//follow RLE1247
    UINT8 ffeLevel=0, bLoadFfeTable=0;
    HDMI_TX_AMP_FINE_TUNE_INDEX ffeTableIdx=0;
#ifdef FFE_Support			
    UINT8 value=0;
#endif	
    TX_TIMING_INDEX format_idx=0;
//    EnumHdmi21FrlType frlType;

    FatalMessageHDMITx("[HDMITX][BANK][7] ScalerHdmiTxPhy_SetSwing:HDMITx_PHY_CPFT_20240828_driver\n");

    if((GET_HDMI_MAC_TX0_MODE_STATE() != _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON) && (GET_HDMI_MAC_TX0_MODE_STATE() != _HDMI_TX_MODE_STATUS_LINK_ON))
    {
        format_idx = ScalerHdmiTxGetScalerVideoFormatIndex();
    }else{
        format_idx = ScalerHdmiTxGetTx0VideoFormatIndex();
    }
    //deep_depth = ScalerDpStreamGetElement(_P0_OUTPUT_PORT, _COLOR_DEPTH);
    //deep_color = (deep_depth > 8? 1: 0);


    if(format_idx >= TX_TIMING_NUM){
        FatalMessageHDMITx("[HDMITX][ERROR] format_idx[%d] OVER RANGE[%d]@ScalerHdmiTxPhy_SetSwing()\n", (UINT32)format_idx, (UINT32)TX_TIMING_NUM);
        return;
    }

//    frlType = ScalerHdmiTxGetOutputFrlType();

#ifdef HDMITX_TIMING_TABLE_CONFIG
    UINT8 deep_depth = 0;
    deep_depth = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_DEPTH);
    timing_gen = &cea_timing_table[format_idx];
    InfoMessageHDMITx("[HDMITX] ScalerHdmiTxPhy_SetSwing(), Idx(VIC)=%d(%d)\n", (UINT32)format_idx, (UINT32)timing_gen->video_code);
    InfoMessageHDMITx("[HDMITX] dp/bit=%d/%d\n", (UINT32)deep_color, (UINT32)deep_depth);
    if((timing_gen->video_code == 0) || (get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE)) {
        ScalerHdmiTxPhy_CalSwing();
        tx_swing = tx_cal_swing;
    } else {
        switch (deep_depth)
        {
            case 1: //10bit
                tx_swing = &hdmi_tx_swing_dpc_10bit_table_tv001[0]+ format_idx;
                break;
            case 2: //12bit
                tx_swing = &hdmi_tx_swing_dpc_12bit_table_tv001[0]+ format_idx;
                break;
            default:
                tx_swing = &hdmi_tx_swing_dpc_8bit_table_tv001[0] + format_idx;
                break;
        }
    }
//no use end- folloe REL1247
#else
    ScalerHdmiTxPhy_CalSwing();
    tx_swing = tx_cal_swing;
#endif

#ifdef FFE_Support

    ffeLevel = ScalerHdmiTxGetOutputFfeLevel();
    ffeTableIdx = (ffeLevel < FFE_LEVEL_NUM? tx_swing->ffeTableIndex: 0);
    // [FFE] change FFE level depend on board type and format index
    bLoadFfeTable = hdmi_tx_FFE_table && (ffeTableIdx != HDMI_TX_AMP_FINE_TUNE_INDEX_NONE);
     if(bLoadFfeTable)
    {
    // HDMITX_PHY_HDMITXPHY_CTRL14
	    value = hdmi_tx_FFE_table[ffeTableIdx].b[ffeLevel].idrv;
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL14_reg, ~HDMITX_PHY_HDMITXPHY_CTRL14_reg_tmds_idrva_mask, HDMITX_PHY_HDMITXPHY_CTRL14_reg_tmds_idrva(value));
	    value =hdmi_tx_FFE_table[ffeTableIdx].g[ffeLevel].idrv;
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL14_reg, ~HDMITX_PHY_HDMITXPHY_CTRL14_reg_tmds_idrvb_mask, HDMITX_PHY_HDMITXPHY_CTRL14_reg_tmds_idrvb(value));
	    value = hdmi_tx_FFE_table[ffeTableIdx].r[ffeLevel].idrv;
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL14_reg, ~HDMITX_PHY_HDMITXPHY_CTRL14_reg_tmds_idrvc_mask, HDMITX_PHY_HDMITXPHY_CTRL14_reg_tmds_idrvc(value));
    value = (bLoadFfeTable? hdmi_tx_FFE_table[ffeTableIdx].clk[ffeLevel].idrv: tx_swing->clk.idrv);
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL14_reg, ~HDMITX_PHY_HDMITXPHY_CTRL14_reg_tmds_idrvck_mask, HDMITX_PHY_HDMITXPHY_CTRL14_reg_tmds_idrvck(value));

    // HDMITX_PHY_HDMITXPHY_CTRL15
 	   value = hdmi_tx_FFE_table[ffeTableIdx].b[ffeLevel].iem);
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL15_reg, ~HDMITX_PHY_HDMITXPHY_CTRL15_reg_tmds_iema_mask, HDMITX_PHY_HDMITXPHY_CTRL15_reg_tmds_iema(value));
  	   value = hdmi_tx_FFE_table[ffeTableIdx].g[ffeLevel].iem;
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL15_reg, ~HDMITX_PHY_HDMITXPHY_CTRL15_reg_tmds_iemb_mask, HDMITX_PHY_HDMITXPHY_CTRL15_reg_tmds_iemb(value));
  	   value = hdmi_tx_FFE_table[ffeTableIdx].r[ffeLevel].iem;
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL15_reg, ~HDMITX_PHY_HDMITXPHY_CTRL15_reg_tmds_iemc_mask, HDMITX_PHY_HDMITXPHY_CTRL15_reg_tmds_iemc(value));
   	   value = hdmi_tx_FFE_table[ffeTableIdx].clk[ffeLevel].iem;
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL15_reg, ~HDMITX_PHY_HDMITXPHY_CTRL15_reg_tmds_iemck_mask, HDMITX_PHY_HDMITXPHY_CTRL15_reg_tmds_iemck(value));

    // HDMITX_PHY_HDMITXPHY_CTRL16
  	   value =hdmi_tx_FFE_table[ffeTableIdx].b[ffeLevel].iempre;
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL16_reg, ~HDMITX_PHY_HDMITXPHY_CTRL16_reg_tmds_iemprea_mask, HDMITX_PHY_HDMITXPHY_CTRL16_reg_tmds_iemprea(value));
  	   value = hdmi_tx_FFE_table[ffeTableIdx].g[ffeLevel].iempre;
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL16_reg, ~HDMITX_PHY_HDMITXPHY_CTRL16_reg_tmds_iempreb_mask, HDMITX_PHY_HDMITXPHY_CTRL16_reg_tmds_iempreb(value));
  	   value =hdmi_tx_FFE_table[ffeTableIdx].r[ffeLevel].iempre;
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL16_reg, ~HDMITX_PHY_HDMITXPHY_CTRL16_reg_tmds_iemprec_mask, HDMITX_PHY_HDMITXPHY_CTRL16_reg_tmds_iemprec(value));
   	   value = hdmi_tx_FFE_table[ffeTableIdx].clk[ffeLevel].iempre;
    rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL16_reg, ~HDMITX_PHY_HDMITXPHY_CTRL16_reg_tmds_iempreck_mask, HDMITX_PHY_HDMITXPHY_CTRL16_reg_tmds_iempreck(value));
     	}

#endif

    rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_active_mask, HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_active(1));
    DebugMessageHDMITx("[HDMITX] FFE ID/Fmt[%d/%d]\n", (UINT32)curFfeId, (UINT32)format_idx);
    DebugMessageHDMITx("[HDMITX] bLoadFfeTable=%d\n", (UINT32)bLoadFfeTable);
    DebugMessageHDMITx("[HDMITX] Idx/LV=%d/%d\n", (UINT32)ffeTableIdx, (UINT32)ffeLevel);

    return;
}
void ScalerHdmiTxSetPhyAndSwing(void)
{
    // HDMITX PHY PLL setting
    ScalerHdmiTxPhy_SetPLL();

    // HDMITX PHY swing setting
    ScalerHdmiTxPhy_SetSwing();
}

// H5X_HDMITX_BYPASS_HPD_EDID
extern void ScalerHdmi21MacTx0FRLReset(BOOLEAN bBorrowEnable);
extern UINT8 HDMITX_DTG_Wait_Den_Stop_Done(void);
void ScalerTxHdmiTx0LtspGap(void)
{
    // Send All Gap pattern
    DebugMessageHDMI21Tx("[HDMI21_TX] LTSP_GAP\n");

    // Enable RS
    rtd_maskl(HDMITX21_MAC_HDMI21_RS_0_reg, ~_BIT7, _BIT7);

    if(GET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(_TX0) == _TX_STREAM_READY)
    {
        if(GET_HDMI21_MAC_TX_DSCE_EN(_TX0))
            rtd_maskl(HDMITX21_MAC_HDMI21_FORMAT_TOP_reg, ~(_BIT7 | _BIT6), _BIT7); // DSC
        else
#ifdef CONFIG_ENABLE_ZEBU_BRING_UP // keep clock source in deep color mode (8bit deep color mode clock should same as VESA input source)
        rtd_maskl(HDMITX21_MAC_HDMI21_FORMAT_TOP_reg, ~(_BIT7 | _BIT6), _BIT6); // DEEP COLOR
#else
        rtd_maskl(HDMITX21_MAC_HDMI21_FORMAT_TOP_reg, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6)); // Bypass VESA data
#endif
    }
    else // FREE run clock
    {
        // TO DO
    }

    // reset FRL and not enable Borrow Mode
    ScalerHdmi21MacTx0FRLReset(_FALSE);

    // _HDMI21_FRL_3G_4LANES=2

    if(GET_HDMI21_MAC_TX_FRL_RATE(_TX0) > 2)
    {
        // Set Lane0~3 FRL,  16b/18b enable
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~(_BIT7 | _BIT5), (_BIT7 | _BIT5));

        rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT7 | _BIT6 | _BIT5 | _BIT4));
    }
    else
    {
        // Set Lane0~2 FRL, 16b/18b enable
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~(_BIT7 | _BIT5), _BIT7);

        rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT6 | _BIT5 | _BIT4));
    }

#if 1 //  [TEST] enable gap_en before enable FRL & 16b18b
#if 0//#ifndef H5X_HDMITX_BYPASS_HPD_EDID
     // Enable FRL Gap Pattern and HW auto Gen Gap after Vsync
    if(GET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(_TX0) == _TX_STREAM_READY)
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_19_reg, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
    }
    else
#endif
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_19_reg, ~(_BIT7 | _BIT6), _BIT7);
    }
#endif

    // _HDMI21_FRL_3G_4LANES=2 Scramble enable
    if(GET_HDMI21_MAC_TX_FRL_RATE(_TX0) > 2)
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_SCR_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT7 | _BIT6 | _BIT5 | _BIT4));
    }
    else
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_SCR_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT6 | _BIT5 | _BIT4));
    }

    // wait gap_en ready
    udelay(1);

    // Tx data from FRL module
    rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~_BIT3, _BIT3);


    // wait GCP packet done
    //DebugMessageHDMI21Tx("[HDMI21_TX] Wait FRL/GCP Setting Apply...\n");

    // [FIXME] wait den_end event
    //ScalerTimerDelayXms(30);
    //Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,1);
    //DebugMessageHDMI21Tx("[HDMI21_TX] write clear update\n");

    HDMITX_DTG_Wait_Den_Stop_Done();

    CLR_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0);
    SET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(_TX0, _LINK_TRAING_PASS);
    ScalerTimerActiveTimerEvent(TIMER_HDMITX_LT_STABLE_CHECK, _SCALER_TIMER_EVENT_HDM21_TX0_FRL_PASS_TIMER);

    return;
}

void ScalerHdmiTxPhy_SetFllTracking(unsigned char enable) //diff from REL1247
{
    hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl_RBUS hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl_reg;

    if(enable){
        hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl4_RBUS hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl4_reg;
        hdmitx_phy_hdmitx_disp_pll_ssc0_RBUS hdmitx_phy_hdmitx_disp_pll_ssc0_reg;
        hdmitx_phy_hdmitx_disp_pll_ssc1_RBUS hdmitx_phy_hdmitx_disp_pll_ssc1_reg;
        hdmitx_phy_sys_hdmitx_disp_ss_RBUS hdmitx_phy_sys_hdmitx_disp_ss_reg;
        UINT32 multiplier, y;

        //ss load & enable
        hdmitx_phy_sys_hdmitx_disp_ss_reg.regValue = rtd_inl(HDMITX_PHY_SYS_HDMITX_DISP_SS_reg);
        hdmitx_phy_sys_hdmitx_disp_ss_reg.reg_disp_ss_en=1;
        hdmitx_phy_sys_hdmitx_disp_ss_reg.reg_disp_ss_load=0;
        rtd_outl(HDMITX_PHY_SYS_HDMITX_DISP_SS_reg, hdmitx_phy_sys_hdmitx_disp_ss_reg.regValue);

        // new_fll_tracking_mul setting
        hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl4_reg.regValue = rtd_inl(HDMITX_PHY_HDMITX_DISP_DPLL_IN_FSYNC_TRACKING_CTRL4_reg);
        hdmitx_phy_hdmitx_disp_pll_ssc1_reg.regValue = rtd_inl(HDMITX_PHY_HDMITX_DISP_PLL_SSC1_reg);
        hdmitx_phy_hdmitx_disp_pll_ssc0_reg.regValue = rtd_inl(HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg);
        y = (2048*(hdmitx_phy_hdmitx_disp_pll_ssc1_reg.reg_ncode_t_ssc_disp+3))+hdmitx_phy_hdmitx_disp_pll_ssc0_reg.reg_fcode_t_ssc_disp;
        multiplier = (y*4096)/4095;
        hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl4_reg.reg_new_fll_tracking_mul= multiplier;
        rtd_outl(HDMITX_PHY_HDMITX_DISP_DPLL_IN_FSYNC_TRACKING_CTRL4_reg, hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl4_reg.regValue);

        // fll_tracking en
        hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl_reg.regValue = rtd_inl(HDMITX_PHY_HDMITX_DISP_DPLL_IN_FSYNC_TRACKING_CTRL_reg);
        hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl_reg.reg_fll_tracking_en=1;
        rtd_outl(HDMITX_PHY_HDMITX_DISP_DPLL_IN_FSYNC_TRACKING_CTRL_reg, hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl_reg.regValue);

        hdmitx_phy_sys_hdmitx_disp_ss_reg.regValue = rtd_inl(HDMITX_PHY_SYS_HDMITX_DISP_SS_reg);
        hdmitx_phy_sys_hdmitx_disp_ss_reg.reg_disp_ss_en=1;
        hdmitx_phy_sys_hdmitx_disp_ss_reg.reg_disp_ss_load=1;
        rtd_outl(HDMITX_PHY_SYS_HDMITX_DISP_SS_reg, hdmitx_phy_sys_hdmitx_disp_ss_reg.regValue);

        // disable  bypass_pi_disp for FLL tracking
        rtd_maskl(HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg, ~HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg_bypass_pi_disp_mask, HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg_bypass_pi_disp(0));

    }else{
        hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl_reg.regValue = rtd_inl(HDMITX_PHY_HDMITX_DISP_DPLL_IN_FSYNC_TRACKING_CTRL_reg);
        hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl_reg.reg_fll_tracking_en=0;
        rtd_outl(HDMITX_PHY_HDMITX_DISP_DPLL_IN_FSYNC_TRACKING_CTRL_reg, hdmitx_phy_hdmitx_disp_dpll_in_fsync_tracking_ctrl_reg.regValue);

        // restore bypass_pi_disp setting
        rtd_maskl(HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg, ~HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg_bypass_pi_disp_mask,
                        HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg_bypass_pi_disp(HDMITX_PHY_HDMITX_DISP_PLL_SSC0_get_reg_fcode_t_ssc_disp(rtd_inl(HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg))? 0: 1));
    }

    return;
}
