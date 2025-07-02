#ifndef __HDMITX_PHY_BGIC_STRUCT_DEF_H__
#define __HDMITX_PHY_BGIC_STRUCT_DEF_H__
//#include <typedefs.h>

typedef enum _HDMITX_BGIC_VIDEO_FORMAT_INDEX {
    /* FRC/Panel video timing */
    HDMITX_BGIC_1920_1080P_444_60HZ_12G=0,          /* 2k1kp60 444 , [FRC] 2k1kp60 OSD@Slave1 */
    HDMITX_BGIC_3840_2160P_444_120HZ_12G,           /* 4k2kp120 444, [FRC][Panel] 4k2kp120 444@Master */
    HDMITX_BGIC_3840_4320P_444_60HZ_12G,             /* 4k4kp60  444, [FRC] 8k4kp60 420@Master, [Panel] 8k4kp60 444@Master/Slave1 */
    HDMITX_BGIC_3840_8640P_444_30HZ_12G,             /* 4k8kp30  444, [FRC] 8k4kp30 444@Master */
    HDMITX_BGIC_4192_4320P_444_60HZ_12G,             /* 4192x4320p60 444, [FRC] 8k4kp60 422/444 compress Master */
    /* Panel video timing */
    HDMITX_BGIC_3840_2160P_444_60HZ_12G,             /* 4k2kp60  444, [Panel] 4k2kp60 444@Master */
    HDMITX_BGIC_3840_1080P_444_60HZ_12G,            /* 4k1kp60  444, [Panel] 4k1kp60 444@Master */
    /* Panel DSC video timing */
    HDMITX_BGIC_DSC_TIMING_START,
    HDMITX_BGIC_3840_2160P_444_60HZ_DSC_12G = HDMITX_BGIC_DSC_TIMING_START,     /* 4k2kp60 444 DSC , [Panel] 4k2kp60 444 DSC@Master */
    HDMITX_BGIC_3840_2160P_444_120HZ_DSC_12G,    /* 4k2kp120 444 DSC , [Panel] 4k2kp120 444 DSC@Master */
    HDMITX_BGIC_3840_4320P_444_60HZ_DSC_12G,     /* 4k4kp60 444 DSC , [Panel] 8k4kp60 444 DSC@Master/Slave1 */
    HDMITX_BGIC_1920_4320P_444_120HZ_DSC_12G,   /* 2k4kp120 444 DSC, [Panel] 8k4kp120 444 DSC@Master/Slave1 */
    HDMITX_BGIC_VIDEO_TIMING_NUM,
    HDMITX_BGIC_VIDEO_TIMING_AUTO
}HDMITX_BGIC_VIDEO_FORMAT_INDEX;


typedef enum{
    HDMITX_LANE_MODE_4Lane=0,
    HDMITX_LANE_MODE_2Lane,
    HDMITX_LANE_MODE_1Lane,
    HDMITX_LANE_MODE_UNKNOW
}EnumHdmiTx_laneMode;

// =============== HDMITX PHY Struct Define ====================
typedef struct {
	UINT16 blank;
	UINT16 active;
	UINT16 sync;
	UINT16 front;
	UINT16 back;
} cea_timing_h_param_st;

typedef struct {
	UINT16 space;
	UINT16 blank;
	UINT16 act_video;
	UINT16 active;
	UINT16 sync;
	UINT16 front;
	UINT16 back;
} cea_timing_v_param_st;

typedef struct {
    UINT16 video_code;
    cea_timing_h_param_st h;
    cea_timing_v_param_st v;
    UINT16 pixelFreq;
} hdmi_tx_timing_gen_st;

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
    EnumHdmiTx_laneMode lane_mode; // 0: 4lane, 1:2 lane, 2: 1lane
} pll_div_param_st;
// =============== HDMITX PHY Struct Define End ====================

typedef struct {
    UINT8 dsc_pps[96];
} hdmitx_dsc_pps_st;

typedef struct {
    UINT16 red;
    UINT16 green;
    UINT16 blue;
} hdmitx_ptg_color_st;


#endif // #ifndef __HDMITX_PHY_BGIC_STRUCT_DEF_H__
