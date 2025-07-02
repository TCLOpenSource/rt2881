#ifndef _HDMITX_PHY_STRUCT_DEF_H_
#define _HDMITX_PHY_STRUCT_DEF_H_

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include "ScalerFunctionInclude.h"

//#define VSIF_VIDEO_FORMAT_BIT 5
//#define VSIF_3D_FORMAT_BIT 4

#define FFE_LEVEL_NUM 4

#define  HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve16_shift                    (16)
#define  HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve16_mask                     ((UINT32)0x00010000)
#define  HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve16(data)                    (0x00010000&((UINT32)(data)<<16))
#define  HDMITX_PHY_HDMITXPHY_CTRL01_get_reg_hdmitx_reserve16(data)                ((0x00010000&(data))>>16)

#define  HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve9_shift                    (9)
#define  HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve9_mask                     (0x00000200)
#define  HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve9(data)                    (0x00000200&((data)<<9))
#define  HDMITX_PHY_HDMITXPHY_CTRL01_get_reg_hdmitx_reserve9(data)                ((0x00000200&(data))>>9)

#define  HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve6_shift                    (6)
#define  HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve6_mask                     (0x00000040)
#define  HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve6(data)                    (0x00000040&((data)<<6))
#define  HDMITX_PHY_HDMITXPHY_CTRL01_get_reg_hdmitx_reserve6(data)                ((0x00000040&(data))>>6)

#define  HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve5_shift                    (5)
#define  HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve5_mask                     (0x00000020)
#define  HDMITX_PHY_HDMITXPHY_CTRL01_reg_hdmitx_reserve5(data)                    (0x00000020&((data)<<5))
#define  HDMITX_PHY_HDMITXPHY_CTRL01_get_reg_hdmitx_reserve5(data)                ((0x00000020&(data))>>5)

typedef enum _HDMI_TX_AMP_FINE_TUNE_INDEX{
    HDMI21_TX_FFE_3G=0,
    HDMI21_TX_FFE_6G,
    HDMI21_TX_FFE_8G,
    HDMI21_TX_FFE_10G,
    HDMI21_TX_FFE_12G,
    HDMI_TX_AMP_FINE_TUNE_INDEX_NUM,
    HDMI_TX_AMP_FINE_TUNE_INDEX_NONE=0xff,
} HDMI_TX_AMP_FINE_TUNE_INDEX;


typedef enum _HDMI_TX_FFE_TABLE_LIST{
    HDMI21_TX_FFE_TABLE_LIST_TV001=0,
    HDMI21_TX_FFE_TABLE_LIST_TV006,
    HDMI21_TX_FFE_TABLE_LIST_NUM,
}HDMI_TX_FFE_TABLE_LIST;

enum hdmi_tx_pll_disp_freq_index {

	PLL_DISP_FREQ_27M = 0,
	PLL_DISP_FREQ_54M,
	PLL_DISP_FREQ_74M25,
    PLL_DISP_FREQ_108M,
    PLL_DISP_FREQ_148M5,
    PLL_DISP_FREQ_297M,
    PLL_DISP_FREQ_NUM
};

enum hdmi_tx_deep_color_index {
	DEEP_COLOR_8BIT_MODE = 0,
	DEEP_COLOR_10BIT_MODE,
	DEEP_COLOR_12BIT_MODE,
    DEEP_COLOR_MODE_NUM
};


enum hdmi_tx_vsif_video_format {
	HDMI_TX_NO_ADDITIONAL_VIDEO_FORMAT = 0x0,
	HDMI_TX_EXTENDED_RESOLUTION_4K2K_FORMAT = 0x1,
	HDMI_TX_EXTENDED_RESOLUTION_3D_FORMAT = 0x2,
};

enum hdmi_tx_vsif_4k2k_vic {
	HDMI_TX_2160P30_VIC = 0x1,
	HDMI_TX_2160P24_VIC = 0x3,
};

typedef struct {
	UINT8  vfch_num;
	UINT8  fp_swen;
	UINT8  fp;
	UINT8  pp_swen;
	UINT8  swen;
	UINT8  default_ph;
} deep_color_param_st;

typedef struct {
	UINT8  dly_nor_h;
	UINT8  dly_nor_v;
	UINT8  dly_disp_h;
	UINT8  dly_disp_v;
} disp_timing_param_st;

typedef struct {
	UINT16 disp_ncode;
	UINT16 disp_fcode;
	UINT8  Reserve9_clk_tmds_div;
	UINT16 tmds_div;
	UINT16 pix_div;
	UINT16 plldisp_n;
	UINT16 plldisp_k;
} pll_disp_param_st;

typedef struct {
	UINT16 tmds_ncode;
	UINT16 tmds_fcode;
	UINT16 pll_ckin_sel;
	UINT8  hdmi21_mode;
	UINT8  pll_divnsel;
	UINT8  Reserve6_pllm1b2;
	UINT16 pll_m1;
	UINT16 pll_m2;
	UINT16 pll_vcogain;
	UINT16 mac_div;
} pll_tmds_param_st;

typedef struct {
    UINT8 txsoc; // clock divider setting: 0(div 1), 4(div 2), 6(div 4), 7(div 8)
    UINT8 txsoc_420; // clock divider setting: 0(div 1), 4(div 2), 6(div 4), 7(div 8)
    UINT8 dsce_sclk; // clock divider setting: 0(div 1), 4(div 2), 6(div 4), 7(div 8)
    UINT8 dsce_cclk; // clock divider setting: 0(div 1), 4(div 2), 6(div 4), 7(div 8)
    UINT8 dsce_pclk; // clock divider setting: 0(div 1), 4(div 2), 6(div 4), 7(div 8)
} hdmi_tx_dsc_clkratio_st;

typedef struct {
    pll_disp_param_st disp;
    pll_tmds_param_st tmds;
    hdmi_tx_dsc_clkratio_st dsc_clkratio;
    UINT8 frlRate;
} pll_div_param_st;

typedef struct {
	UINT8 plldisp_ip;
	UINT8 plldisp_rs;
	UINT8 plldisp_cs;
	UINT8 plldisp_cp;
} pll_disp_analog_st;

typedef struct {
	UINT8 plltmds_ip;
	UINT8 plltmds_rs;
	UINT8 plltmds_cs;
	UINT8 plltmds_cp;
} pll_tmds_analog_st;

typedef struct {
	pll_disp_analog_st disp;
	pll_tmds_analog_st tmds;
} pll_analog_param_st;

typedef struct {
	UINT8 v_max;
	UINT8 h_max;
} hdmi_tx_max_packet_num_st;

typedef struct {
	UINT8 em_en;
	UINT8 empre_en;
	UINT8 rsel;
	UINT8 idrv;
	UINT8 iem;
	UINT8 iempre;
	UINT8 csel;
	UINT8 slew;
	UINT8 ipdrv;
	UINT8 ipdrvem;
	UINT8 ipdrvempre;
	UINT8 tmds_csel;
	UINT8 tmds_srem;
	UINT8 tmds_srempre;
} hdmi_tx_swing_r_st;

typedef struct {
	UINT8 em_en;
	UINT8 empre_en;
	UINT8 rsel;
	UINT8 idrv;
	UINT8 iem;
	UINT8 iempre;
	UINT8 csel;
	UINT8 slew;
	UINT8 ipdrv;
	UINT8 ipdrvem;
	UINT8 ipdrvempre;
	UINT8 tmds_csel;
	UINT8 tmds_srem;
	UINT8 tmds_srempre;
} hdmi_tx_swing_g_st;

typedef struct {
	UINT8 em_en;
	UINT8 empre_en;
	UINT8 rsel;
	UINT8 idrv;
	UINT8 iem;
	UINT8 iempre;
	UINT8 csel;
	UINT8 slew;
	UINT8 ipdrv;
	UINT8 ipdrvem;
	UINT8 ipdrvempre;
	UINT8 tmds_csel;
	UINT8 tmds_srem;
	UINT8 tmds_srempre;
} hdmi_tx_swing_b_st;

typedef struct {
	UINT8 em_en;
	UINT8 empre_en;
	UINT8 rsel;
	UINT8 idrv;
	UINT8 iem;
	UINT8 iempre;
	UINT8 csel;
	UINT8 slew;
	UINT8 ipdrv;
	UINT8 ipdrvem;
	UINT8 ipdrvempre;
	UINT8 tmds_csel;
	UINT8 tmds_srem;
	UINT8 tmds_srempre;
} hdmi_tx_swing_clk_st;

typedef struct {
	hdmi_tx_swing_r_st r;
	hdmi_tx_swing_g_st g;
	hdmi_tx_swing_b_st b;
	hdmi_tx_swing_clk_st clk;
	UINT8 ffeTableIndex;
} hdmi_tx_swing_st;


typedef struct {
	UINT8 idrv;
	UINT8 iem;
	UINT8 iempre;
} hdmi_tx_amp_fine_tune_type;

typedef struct {
	hdmi_tx_amp_fine_tune_type r[FFE_LEVEL_NUM];
	hdmi_tx_amp_fine_tune_type g[FFE_LEVEL_NUM];
	hdmi_tx_amp_fine_tune_type b[FFE_LEVEL_NUM];
	hdmi_tx_amp_fine_tune_type clk[FFE_LEVEL_NUM];
} hdmi_tx_amp_fine_tune_st;

#define DISP_PLL_DIV_TABLE_NUM  6
#define SWING_HDMI20_TABLE_NUM  4
#define SWING_HDMI21_TABLE_NUM  6
#define DSCE_TIMING_NUM	3
typedef struct {
    UINT8 tmds_div;
    UINT8 pixel_8bit_div;
    UINT8 pixel_10bit_div;
    UINT8 pixel_12bit_div;
} DISP_PLL_DIV_PARAM;

typedef struct {
    UINT16 tmds_ncode;
    UINT16 tmds_fcode;
}TMDS_PLL_PARAM;

#ifdef CONFIG_ENABLE_HDMITX_PHY
#include "HdmiPhyTx/ScalerHdmiPhyTxInclude.h"
void ScalerHdmiTxPhy_PowerOnDefaultSetting(void);
void ScalerHdmiTxPhy_PowerOff(void);
void ScalerHdmiTxPhy_SetPLL(void);
void ScalerHdmiTxSetPhyAndSwing(void);
void ScalerHdmiTxPhy_SetSwing(void);
void ScalerHdmi21SetTxFfeLevel(UINT8 laneNum, UINT8 ffeLevel, UINT8 ffeMode);
UINT8 ScalerHdmiTxPhy_GetCurrentFeeTableId(void);
void ScalerHdmiTxPhy_SetFeeTable(HDMI_TX_FFE_TABLE_LIST tableId);
BOOLEAN ScalerHdmiPhyTxSet(EnumOutputPort enumOutputPort);
TX_TIMING_INDEX ScalerHdmiTxGetScalerVideoFormatIndex(void);
void ScalerTxHdmiTx0LtspGap(void);
extern EnumHDMITxZ0Detect ScalerHdmiPhyTxZ0Detect(EnumOutputPort enumOutputPort);
extern UINT8 ScalerHdmiPhyTxGetIBHNTrim(EnumOutputPort enumOutputPort);
//void ScalerHdmiPhyTxSetIBHNTrim(EnumOutputPort enumOutputPort, UINT8 ucIBHNTRIMCurrent);
//void ScalerHdmiPhyTxPowerOn(EnumOutputPort enumOutputPort);
UINT8 ScalerHdmiTxPhy_CheckFfeSwingTableCodeCorrect(void);
#else
#define ScalerHdmiPhyTxSet(enumOutputPort)          (0)
#define ScalerHdmiTxGetScalerVideoFormatIndex_bankIF()      (0)
#define ScalerTxHdmiTx0LtspGap()
#define ScalerHdmiTxPhy_PowerOnDefaultSetting()
#define ScalerHdmiTxPhy_PowerOff()
#define ScalerHdmiTxPhy_SetPLL()
#define ScalerHdmiTxSetPhyAndSwing()
#define ScalerHdmiTxPhy_SetSwing()
#define ScalerHdmi21SetTxFfeLevel(laneNum, ffeLevel, ffeMode)
#define ScalerHdmiTxPhy_GetCurrentFeeTableId()        (0)
#define ScalerHdmiTxPhy_SetFeeTable(tableId)
#define ScalerHdmiPhyTxSetIBHNTrim(enumOutputPort, ucIBHNTRIMCurrent)
#define ScalerHdmiPhyTxPowerOn(enumOutputPort)
#define ScalerHdmiPhyTxInitial(enumOutputPort)
#define ScalerHdmiTxPhy_CheckFfeSwingTableCodeCorrect()   (0)
#define ScalerHdmiPhyTxCMUPowerOff(enumOutputPort)
#define ScalerHdmiPhyTxSetZ0Detect(enumOutputPort, bEnable)
#define ScalerHdmiPhyTxSetCMUSignal(enumOutputPort, bEnable)
#define ScalerHdmiPhyTxGetIBHNTrim(enumOutputPort)   (0)
#define ScalerHdmiPhyTxZ0Detect(enumOutputPort)     (0)
#define ScalerHdmiPhyTxEnable3rdTracking(enumOutputPort)
#define ScalerHdmiPhyTxTimerReset(enumOutputPort)
#define ScalerHdmiPhyTxThirdSetPICode(enumOutputPort)

#endif
#endif // _HDMITX_PHY_STRUCT_DEF_H_
