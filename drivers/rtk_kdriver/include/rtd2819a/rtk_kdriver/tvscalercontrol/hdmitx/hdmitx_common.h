#ifndef _HDMITX_COMMON_H_
#define _HDMITX_COMMON_H_
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_share.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_driver.h>
#include <rtd_log/rtd_module_log.h>
#include <linux/delay.h>

/************************
// enum type define
//
*************************/
typedef enum
{
    _VENDOR_MODEL = 0x00,
    _EDID_VERSION,
    _EDID_SEG_READ_SUPPORT,
    _YCC444_SUPPORT,
    _YCC422_SUPPORT,
    _FIRST_EXT_DTD_ADDR,
    _LPCM_FS_SUPPORT,
    _3D_LPCM_FS_SUPPORT,
    _VCDB_QY,
    _VCDB_QS,
    _YCC420_SUPPORT,
    _DRR_SUPPORT,
    _AI_SUPPORT,
    _DC48_SUPPORT,
    _DC36_SUPPORT,
    _DC30_SUPPORT,
    _LATENCY_FIELD_PRESENT,
    _ILATENCY_FIELD_PRESENT,
    _HDMI_VIDEO_PRESENT,
    _DC_Y444_SUPPORT,
    _SCDC_PRESENT,
    _RR_CAPABLE,
    _LTE_340MCSC_SCRAMBLE,
    _DC48_420_SUPPORT,
    _DC36_420_SUPPORT,
    _DC30_420_SUPPORT,
    _HDMI_MAX_TMDS_CLK,
    _MAX_FRL_RATE,
    _VRR_MIN,
    _VRR_MAX,
    _DSC_VERSION_12,
    _DSC_NATIVE_420,
    _DSC_ALL_BPP,
    _DSC_16BPC,
    _DSC_12BPC,
    _DSC_10BPC,
    _DSC_MAX_FRL_RATE,
    _DSC_MAX_SLICES,
    _DSC_TOTAL_CHUNKKBYTE,
    _DID_EXT_SUPPORT,
    _EDID_EXT_BLOCK_COUNT,
    _SRC_PHYSICAL_ADDR,
    _VSD_8K_SUPPORT,
    _FAPA_End_Extended,
    _FVA,
    _QMS,
    _CINEMAVRR,
    _NEG_MVRR,
    _MDelta,
    _Independent_view,
    _Dual_View,
    _3D_OSD_Disparity,
    _BT2020_RGB,
    _BT2020_YCC,
    _BT2020_cYCC,
    _opRGB,
    _opYCC601,
    _sYCC601,
    _xvYCC709,
    _xvYCC601,
    _ST2113RGB23,
    _ICTCP,
    _HDR_SMPTE_ST2084,
    _HDR_HLG,
    _EDID_FEATURE_TYPE_NUM
} EnumEdidFeatureType;

typedef enum{
    _TXOUT_BOARD_DDR_SUPPORT,
    _TXOUT_ALL_VDB_SUPPORT,
    _TXOUT_ALL_420VDB_SUPPORT,
    _TXOUT_ALL_420CMDB_SUPPORT,
    _TXOUT_ALL_DTD_SUPPORT,
    _TXOUT_ALL_EXT_DTD_SUPPORT,
    _TXOUT_ALL_DID_T1_SUPPORT,
    _TXOUT_TIMING_SUPPORT_TYPE_NUM
}EnumTxOutTimingSupportType;


typedef enum{
    _POWER_ACTION_AC_ON_TO_NORMAL=0,
    _POWER_ACTION_PS_TO_NORMAL,
    _POWER_ACTION_OFF_TO_NORMAL,
    _POWER_ACTION_OFF_TO_FAKE_OFF,
    _POWER_ACTION_NORMAL_TO_PS,
    _POWER_ACTION_NORMAL_TO_OFF,
    _POWER_ACTION_AC_ON_TO_OFF,
    _POWER_ACTION_PS_TO_OFF
}EnumPowerAction;

typedef enum _EnumColorIMETryType{
    /* 0  */ _COLORIMETRY_RGB_SRGB=0,
    /* 1  */ _COLORIMETRY_RGB_XRRGB,
    /* 2  */ _COLORIMETRY_RGB_SCRGB,
    /* 3  */ _COLORIMETRY_RGB_ADOBERGB,
    /* 4  */ _COLORIMETRY_RGB_DCI_P3,
    /* 5  */ _COLORIMETRY_YCC_ITUR_BT709,
    /* 6  */ _COLORIMETRY_YCC_SMPTE_170M,
    /* 7  */ _COLORIMETRY_YCC_XVYCC601,
    /* 8  */ _COLORIMETRY_YCC_XVYCC709,
    /* 9  */ _COLORIMETRY_EXT,
    /* 10 */ _COLORMETRY_RESERVED
}EnumColorIMETryType;


typedef enum _EnumColorIMETryExtType{
    /* 0  */ _COLORIMETRY_EXT_RGB_SRGB=0,
    /* 1  */ _COLORIMETRY_EXT_RGB_XRRGB,
    /* 2  */ _COLORIMETRY_EXT_RGB_SCRGB,
    /* 3  */ _COLORIMETRY_EXT_RGB_ADOBERGB,
    /* 4  */ _COLORIMETRY_EXT_RGB_DCI_P3,
    /* 5  */ _COLORIMETRY_EXT_RGB_ITUR_BT2020,
    /* 6  */ _COLORIMETRY_EXT_YCC_ITUR_BT709,
    /* 7  */ _COLORIMETRY_EXT_YCC_XVYCC601,
    /* 8  */ _COLORIMETRY_EXT_YCC_XVYCC709,
    /* 9  */ _COLORIMETRY_EXT_YCC_SYCC601,
    /* 10 */ _COLORIMETRY_EXT_YCC_ADOBEYCC601,
    /* 11 */ _COLORIMETRY_EXT_YCC_ITUR_BT2020_CL,
    /* 12 */ _COLORIMETRY_EXT_YCC_ITUR_BT2020_NCL,
    /* 13 */ _COLORIMETRY_EXT_RESERVED
}EnumColorIMETryExtType;


//--------------------------------------------------
// Enumerations of Quantization Range (RGB)
//--------------------------------------------------
typedef enum
{
    _RGB_QUANTIZATION_FULL_RANGE = 0x00,
    _RGB_QUANTIZATION_LIMIT_RANGE = 0x01,
    _RGB_QUANTIZATION_DEFAULT_RANGE = 0x02,
    _RGB_QUANTIZATION_RESERVED = 0x03,
} EnumColorRGBQuantization;

typedef enum
{
    _YCC_QUANTIZATION_LIMIT_RANGE = 0x00,
    _YCC_QUANTIZATION_FULL_RANGE = 0x01,
    _YCC_QUANTIZATION_RESERVED = 0x02,
} EnumColorYCCQuantization;

typedef enum _EnumOutputStreamType{
    _STREAM_TYPE_NONE=0,
    _STREAM_TYPE_SST,
    _STREAM_TYPE_DSC,
}EnumOutputStreamType;

typedef enum _EnumOutputPowerStatus{
    _POWER_STATUS_OFF = 0,
    _POWER_STATUS_NORMAL,
}EnumOutputPowerStatus;


typedef enum _EnumHdmi21FrlType{
    _HDMI21_FRL_NONE=0,
    _HDMI21_FRL_3G=1,
    _HDMI21_FRL_6G_3LANES,
    _HDMI21_FRL_6G_4LANES,
    _HDMI21_FRL_8G,
    _HDMI21_FRL_10G,
    _HDMI21_FRL_12G,
    _HDMI21_FRL_MAX_NUM
}EnumHdmi21FrlType;

typedef enum _EnumHdmitxTimingStdType{
    _HDMITX_STD_HDMI=0,
    _HDMITX_STD_VESA,
    _HDMITX_STD_STB6500,
}EnumHdmitxTimingStdType;

typedef enum _EDID_TIMING_DB{
    EDID_TIMING_ANY=0,
    EDID_TIMING_VDB,
    EDID_TIMING_420VDB,
    EDID_TIMING_420CMDB,
    EDID_TIMING_DTD,
    EDID_TIMING_EXT_DTD,
    EDID_TIMING_DID_DTD,
    EDID_TIMING_MODE_NUM,
}EDID_TIMING_DB;


typedef enum{
    TX_SW_PKT_AVI_INFO                    = (1<<0),
    TX_SW_PKT_VSIF_HDMI_14B        = (1<<1),
    TX_SW_PKT_VSIF_HDMI_ALLM      = (1<<2),
    TX_SW_PKT_VSIF_DV_VSIF            = (1<<3),
    TX_SW_PKT_VSIF_HDR10PLUS      = (1<<4),
    TX_SW_PKT_VSIF_CUVA_HDR        = (1<<5),
    TX_SW_PKT_SPD_FREE_SYNC       = (1<<6),
    TX_SW_PKT_DRM_HDR            = (1<<7),
    TX_SW_PKT_EMP_VTEM_VRR         = (1<<8),
    TX_SW_PKT_EMP_DV_VS_EMDS    = (1<<9),
    TX_SW_PKT_Gamut_MD                  = (1<<10),
}EnumHdmiRepeaterSwPktType;

typedef enum{
    TX_BYPASS_PKT_HD_OUI_CUNTOMER   = _BIT16,
    TX_BYPASS_PKT_HD_OUI_DOLBY      = _BIT15,
    TX_BYPASS_PKT_HD_OUI_HDR10P     = _BIT14,
    TX_BYPASS_PKT_HD_OUI_H14B          = _BIT13,
    TX_BYPASS_PKT_HD_OUI_HF             = _BIT12,
    TX_BYPASS_PKT_HD_AUDIO_INF      = _BIT9,
    TX_BYPASS_PKT_HD_GMP                  = _BIT8,
    TX_BYPASS_PKT_HD_RES                   = _BIT7,
    TX_BYPASS_PKT_HD_SPD                   = _BIT6,
    TX_BYPASS_PKT_HD_AUDIO_HBR     = _BIT5,
    TX_BYPASS_PKT_HD_AUDIO              = _BIT4,
    TX_BYPASS_PKT_HD_EMP                  = _BIT3,
    TX_BYPASS_PKT_HD_VSIF                 = _BIT2,
    TX_BYPASS_PKT_HD_AVI                   = _BIT1,
    TX_BYPASS_PKT_HD_DRAM               = _BIT0,
}EnumHdmiRepeaterBypassPktHdrType;




typedef enum{
    TX_EVENT_CHECK_EVENT=0,
    TX_EVENT_STATE_EVENT,
    TX_EVENT_DEINIT_EVENT,
    TX_EVENT_NUM,
}TX_EVENT_TYPE;



typedef enum{
    TX_TEST_CMD_NONE=0,
    TX_TEST_CMD_CHANGE_TIME,
    TX_TEST_CMD_CHANGE_LANE_MAPPING,
    TX_TEST_CMD_SEND_EVENT,
    TX_TEST_CMD_SET_VBY1_LOCK,
    TX_TEST_CMD_SET_PENDING_STATE,
    TX_TEST_CMD_SET_PENDING_TX_PKT,
    TX_TEST_CMD_NUM,
}ENUM_TX_TEST_CMD_TYPE;

/************************
// data struture type define
//
*************************/
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
    UINT8 video_code1;
    UINT8 video_code2;
    cea_timing_h_param_st h;
    cea_timing_v_param_st v;
    UINT16 vFreq;
    UINT16 pixelFreq;
} hdmi_tx_timing_gen_st;

typedef struct {
    UINT8 video_code;
    cea_timing_h_param_st h;
    cea_timing_v_param_st v;
    UINT16 vFreq;
    UINT16 pixelFreq;
} hdmi_repeater_timing_gen_st;

typedef struct{
    UINT16 h_act;
    UINT16 h_blank;
    UINT16 h_front;
    UINT16 h_sync;
    UINT16 v_act;
    UINT16 v_blank;
    UINT16 v_front;
    UINT16 v_sync;
    UINT32 pixelClk;
}hdmi_timing_check_type;

typedef enum{
    TX_WIDTH_INFO = 0,
    TX_HEIGHT_INFO,
    TX_PROGRESSIVE_INFO,
    TX_FRAMERATE_INFO,
    TX_COLORTYPE_INFO,
    TX_COLORDEPTH_INFO,
    TX_FRLTYPE_INFO,
}HDMI_OUTPUT_TIMING_TABLE;

typedef union _HDMITX_INIT_SETTING_TYPE{
    UINT8  u8_val;
    struct {
        UINT8 bHdmitxSettingDoneFlag:1;
        UINT8 bHdmirxSettingDoneFlag:1;
        UINT8 bPktBypassEnableFlag:1;
        UINT8 bSkipAudioPktBypassFlag:1;
        UINT8 bSkipEMPPktBypassFlag:1;
        UINT8 bDtgFSyncEnable:1;
        UINT8 bVrrEnable:1;
        UINT8 bSkipEdidModify:1;
    } init_setting;
} HDMITX_INIT_SETTING_TYPE;

typedef struct{
    UINT16 VO_ACT_H_START ;
    UINT16 VO_ACT_V_START ;
    UINT16 VO_H_BACKPORCH ;
    UINT16 VO_V_BACKPORCH ;
    UINT16 DTG_H_DEN_STA ;
    UINT16 DTG_V_DEN_STA ;
    UINT16 DTG_H_PORCH ;
    UINT16 DTG_V_PORCH ;
    UINT16 DTG_IV2DV_HSYNC_WIDTH;
    UINT16 DTG_DH_WIDTH ;
    UINT16 DTG_DV_LENGTH ;
}scaler_hdmi_timing_info;


typedef struct {
    UINT16 width;
    UINT16 height;
    UINT8 progressive;          // 0 --> interlaced ; 1 --> progressive
    UINT16 frame_rate;
    UINT8 color_type; // I3DDMA_COLOR_SPACE_T: 0: RGB, 1: YUV422, 2: YUV444,3: YUV411(420)
    UINT8 color_depth; // I3DDMA_COLOR_DEPTH_T: 0: 8bit, 1: 10B, 2: 12B, 3: 16B
    UINT8 frl_type;
} hdmi_tx_output_timing_type;

typedef enum {
    HDMITX_MODE_DVI    = 0x0,    // DVI mode
    HDMITX_MODE_HDMI   = 0x1,    // HDMI 1.4/2.0
    HDMITX_MODE_HDMI21 = 0x2,    // HDMI 2.1
    HDMITX_MODE_DPRX   = 0x3,    // DPRX 
    HDMITX_MODE_UNKNOW = 0x4,
} HDMITX_DVI_MODE_T;

typedef enum {

    COLOR_DEPTH_8B =0,
    COLOR_DEPTH_10B,
    COLOR_DEPTH_12B,
    COLOR_DEPTH_16B,
    COLOR_DEPTH_6B
} COLOR_DEPTH_T;

typedef enum {
    HDMITX_COLOR_RGB   = 0x00,
    HDMITX_COLOR_YUV422,
    HDMITX_COLOR_YUV444,
    HDMITX_COLOR_YUV420,
    HDMITX_COLOR_YUV400, //Y ONLY
    HDMITX_COLOR_UNKNOW
} HDMITX_COLOR_SPACE_T;

typedef enum {
    VSYNC_MODE_NON_VRR   = 0x00,
    VSYNC_MODE_VRR, // HDMI Forum VRR
    VSYNC_MODE_FREESYNC, // AMD Freesync
    VSYNC_MODE_ADAPTIVE_SYNC, // DP Freesync
    VSYNC_MODE_QMS,
    VSYNC_MODE_NUM
} VSYNC_MODE_T;

typedef enum {
    DRMODE_SDR   = 0x00,
    DRMODE_HDR10 = 0x10,
    DRMODE_HLG = 0x11,
    DRMODE_HDR10PLUS = 0x20,
    DRMODE_DOLBY_STD = 0x30,
    DRMODE_DOLBY_LL = 0x31,
    DRMODE_DOLBY_VSEM = 0x32,
    DRMODE_NUM
} DR_MODE_T;

typedef enum {
	// C
    /* 0 */ COLORIMETRY_NOSPECIFIED = 0,
    /* 1 */ COLORIMETRY_601,
    /* 2 */ COLORIMETRY_709,
	// EC
    /* 3 */ COLORIMETRY_XYYCC601,
    /* 4 */ COLORIMETRY_XYYCC709,
    /* 5 */ COLORIMETRY_SYCC601,
    /* 6 */ COLORIMETRY_ADOBE_YCC601,
    /* 7 */ COLORIMETRY_ADOBE_RGB,
	/* 8 */ COLORIMETRY_BT2020_YCC,
	/* 9 */ COLORIMETRY_BT2020_RGB_YCBCR,
	// AEC
	/* 10*/ COLORIMETRY_ST2113_P3D65,
	/* 11*/ COLORIMETRY_ST2113_P3DCI,
	/* 12*/ COLORIMETRY_BT2100_ICC,
	/* 13*/ COLORIMETRY_MAX
} COLORIMETRY_T;

typedef enum {
    /* 0 */ COLOR_RAG_DEFAULT = 0,
    /* 1 */ COLOR_RAG_LIMIT,
    /* 2 */ COLOR_RAG_FULL,
    /* 3 */ COLOR_RAG_UNKNOW,
    /* 4 */ COLOR_RAG_MAX
} COLOR_RGB_YUV_RANGE_MODE_T;

typedef enum _HDMI_HDCP_ {
    NO_HDCP = 0,
    HDCP14,
    HDCP22,
    HDCP_OFF
} HDMI_HDCP_E;

typedef enum{
    DRIVER_HDMITX_INPUT_SRC_HDMIRX = 0,
    DRIVER_HDMITX_INPUT_SRC_DPRX,
    DRIVER_HDMITX_INPUT_SRC_DTG,
    DRIVER_HDMITX_INPUT_SRC_MAX
}DRIVER_HDMITX_INPUT_SRC_T;

typedef struct {
    HDMI_HDCP_E	hdcprx_Type;
    HDMI_HDCP_E	hdcptx_Type;
}HDMI_RX_TX_HDPC_T;

typedef struct {
    UINT8 hdcp1tx_id[5];
    UINT8 hdcp1rx_id[5];
    UINT8 hdcp2rx_id[5];
    UINT8 hdcp2tx_burn;
}HDMI_HDCP_ID_LIST_T;

typedef struct {
    UINT8   is_interlace : 1;
    UINT8   pixel_repeat : 3;
    UINT8   polarity : 2;
    UINT8   run_dsc : 2;    // 0: do not run dsc flow, 1: run dsc flow
    VSYNC_MODE_T   run_vrr; // 0: non VRR, 
    UINT16  v_total;
    UINT16  h_total;
    UINT16  v_act_len;
    UINT16  h_act_len;
    UINT16  v_act_sta;
    UINT16  h_act_sta;
    UINT16  v_freq;            // unit : 0.01 Hz
    UINT16  h_freq;            // unit : Hz
    DR_MODE_T dr_mode; // 0x0: SDR, 0x1: HDR10, 0x11: HLG; 0x20: HDR10PLUS ; 0x30: Dolby_Vision STD, 0x31: Dolby Vision Low Latency,  0x32: Dolby Vision VSEM.
    UINT16  IVSyncPulseCount;  // Input VSYNC High Period Measurement Result
    UINT16  IHSyncPulseCount;  // Input HSYNC High Period Measurement Result
    HDMITX_DVI_MODE_T     mode; // 0: DVI mode, 1: HDMI14/20 mode, 2: HDMI21 mode, 3: DRPX mode, 4: Unknow
    COLOR_DEPTH_T colordepth; // 0: 8bit, 1:10bit, 2:12bit, 3:16bit, 4: 6bit
    HDMITX_COLOR_SPACE_T colorspace; // 0: RGB, 1: YUV422, 2: YUV444, 3: YUV420, 4: YUV400(Y ONLY)
    COLOR_RGB_YUV_RANGE_MODE_T colorrange; // 0: default, 1: limit, 2: full, 3: unknow
    COLORIMETRY_T  colorimetry; //
    UINT8  ext_colorimetry;
    HDMI_HDCP_E  hdcpType;//0: no hdcp, 1: hdcp13/14, 2: hdcp22/23, 3: HDCP_OFF
    UINT16  tmds_clk_b;
    UINT32  pll_pixelclockx1024;
    UINT32 cur_sys_time;
} HDMI_DP_TIMING_T;

/************************
// Functoin/Macro Definition
//
*************************/
#define HDMITX_MDELAY(x) mdelay(x)

#define ScalerHdmiTxGetBypassLinkTrainingEn() (ucTxBypassLinkTrainingEn)
//#define ScalerHdmiTx_Get_Timing_Status() (tx_status)
#define hdmitxGetTxOutputPixelClock()   (g_TxPixelClock1024)

#define SET_HDMI_4K144_RBV2_FLAG()                (g_hdmi4k144RBv2 = _TRUE)
#define CLR_HDMI_4K144_RBV2_FLAG()                (g_hdmi4k144RBv2 = _FALSE)
#define GET_HDMI_4K144_RBV2_FLAG()                (g_hdmi4k144RBv2)

#define SET_HDMITX_AP_COLOR_FORMAT(x)                (ap_hdmitx_color_format= x)
#define GET_HDMITX_AP_COLOR_FORMAT()                (ap_hdmitx_color_format)
#define SET_HDMITX_AP_COLOR_DEPTH(x)                (ap_hdmitx_color_depth= x)
#define GET_HDMITX_AP_COLOR_DEPTH()                (ap_hdmitx_color_depth)
#define SET_HDMITX_AP_FRL_MODE(x)                (ap_hdmitx_frl_mode= x)
#define GET_HDMITX_AP_FRL_MODE()                (ap_hdmitx_frl_mode)

extern UINT8 lib_hdmitx_is_VRR_Function_support(void);
#ifdef HDMITX_SW_CONFIG
extern EnumRX_EDID_Policy lib_hdmitx_get_edidPolicy_support(void);
#endif
extern UINT8 lib_hdmitx_get_safemode_support(void);
extern UINT8 lib_hdmitx_is_hdmi_21_support(void);
UINT8 lib_hdmitx_industrial_ctrl_enable(void);
extern UINT8 lib_hdmitx_is_hdmi_limit_support(void);
extern UINT8 lib_hdmitx_is_hdmi_422_support(void);
extern UINT8 lib_hdmitx_is_hdmi_bypass_support(void);
extern UINT8 lib_hdmitx_Golden_Timing_adjust(UINT16 ucDTDAddress);
extern void lib_hdmitx_packet_config_init(void);
extern void lib_hdmitx_Vrrpacket_bypass_setting(void);
extern UINT16 getLimitPixelClkWithDDR(void);
extern UINT8 lib_hdmitx_is_HDR10_PLUS_Function_support(void);
extern UINT8 lib_hdmitx_is_HDR10_Function_support(void);
extern void ScalerHdmi21MacTx0FillVtemPacket(void);
extern void ScalerHdmi21MacTx0FillSBTMPacket(void);
extern void ScalerHdmi21MacTx0FillSPDPacket(void);
extern void ScalerHdmi21MacTx0DHDRPacket(void);
extern void ScalerHdmi21MacTx0EmpPacket(void);
#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
extern void ScalerHdmi21MacTx0HDRSet(void);
extern void ScalerHdmi21MacTx0FillHDRPacket(void);
#endif
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
extern void ScalerHdmi21MacTx0FillVendorSpecificInfoPacket(void);
extern void ScalerHdmi21MacTx0FillDolbyVisionVsifPacket(void);
#endif
extern TX_STATUS ScalerHdmiTx_Get_Timing_Status(void);

extern void ScalerHdmiMacTx0FillSPDPacket(void);

#define CONFIG_WAIT_HDMIRX_READY 1

#ifdef CONFIG_WAIT_HDMIRX_READY
#define GET_HDMIRX_AVI_VIC() 1
#define GET_HDMIRX_DPC_DEFAULT_PH() 1
#endif
/************************
// Debug Functoin/Macro Definition
//
*************************/
extern UINT8 ucTxBypassLinkTrainingEn;
extern TX_STATUS tx_status;
extern UINT8 txInBypassMode;
extern HDMITX_INIT_SETTING_TYPE hdmitx_init_setting;
extern UINT32 g_TxPixelClock1024;
extern UINT8 bWaitLinkTrainingReady;
extern UINT8 bForceLinkTrainingFlag;
extern UINT8 g_hdmi4k144RBv2;
extern UINT8 g_tx_check_tx_status;
extern unsigned long long HDMI_TX_I2C_BUSID;
extern StructHdmitxDisplayInfo g_scaler_display_info;

extern I3DDMA_COLOR_SPACE_T ap_hdmitx_color_format;
extern I3DDMA_COLOR_DEPTH_T ap_hdmitx_color_depth;
extern EnumHdmi21FrlType ap_hdmitx_frl_mode;

// [From RTD2806]
#define FIX_ME_HDMITX_BRING_UP
//#define CHIP_PLATFORM_BRIDGE

//#define ENABLE_HDMITX_PTG_AUTO_RUN
//#define _DEBUG_HALT_TX_WHEN_FIFO_ERROR

// TX test
#ifdef ENABLE_TIMER_EVENT_TEST // [CMD]: change TX output timing event
#define HDMITX_TEST_EN()    _TRUE
extern UINT8 gPendingTxPkt;
#define DEBUG_TX_Get_SkipTxPkt() (gPendingTxPkt)
#else
#define HDMITX_TEST_EN()    _FALSE
#define DEBUG_TX_Get_SkipTxPkt() _FALSE
#endif
#define HDMITX_HDMI20_MAX_PLL 6000
#define HDMITX_8BIT 0
#define HDMITX_10BIT 1
#define HDMITX_12BIT 2
#define HDMITX_16BIT 3
#define HDMITX_FRL_12G_DATA 48000000
#define HDMITX_FRL_10G_DATA 40000000
#define HDMITX_FRL_8G_DATA 32000000
#define HDMITX_FRL_6G4L_DATA 23400000 //workround for 2.5k144 12bit 23.49G (98%) fail
#define HDMITX_FRL_6G3L_DATA 18000000
#define HDMITX_FRL_3G_DATA 9000000
#define RBUS_CLK_250M                       2500000//Unit:0.1KHz
#define AUDIO_CLK_17TH_POWER_OF_2                       131072// 2^17
#define AUDIO_CLK_FIFO_RANGE                       500
#if 0 //fix by zhaodong
#define CLK_DIV_REG                                        0xB8017858

#define CTRL06_REG                                         0xB8017818
#define CTRL06_get_reg_plldisp_pow(data)                   ((0x00000010&((UINT32)data))>>4)

#define DSC_SRC_SEL_REG                                    0xB80178F8
#define DSC_SRC_SEL_reg_insel_mode(data)                   (0x00010000&(((UINT32)data)<<16))
#define DSC_SRC_SEL_reg_dispd_fifo_reset_mask              (0x00000004UL)
#define DSC_SRC_SEL_reg_dispd_fifo_en_mask                 (0x00000002UL)
#define DSC_SRC_SEL_reg_dispd_fifo_reset(data)             (0x00000004&(((UINT32)data)<<2))
#define DSC_SRC_SEL_reg_dispd_fifo_en(data)                (0x00000002&(((UINT32)data)<<1))

#define CTRL17_reg                                         0xB8017844
#define CTRL17_get_reg_tmds_pow(data)                      ((0x10000000&((UINT32)data))>>28)
#define CTRL17_reg_tmds_pow_mask                           (0x10000000UL)
#define CTRL17_reg_tmds_pow(data)                          (0x10000000&(((UINT32)data)<<28))

#define FSYNC_TRACKING_CTRL4_reg                           0xB80178EC
#define FSYNC_TRACKING_CTRL4_reg_new_fll_tracking_offset_inverse_mask (0x00100000UL)
#define FSYNC_TRACKING_CTRL4_reg_new_fll_tracking_offset_inverse(data) (0x00100000&(((UINT32)data)<<20))
#define FSYNC_TRACKING_CTRL4_reg_new_fll_tracking_mul_mask (0x0007FFFFUL)
#define FSYNC_TRACKING_CTRL4_reg_new_fll_tracking_mul(data) (0x0007FFFF&((UINT32)data))

#define SSC2_reg                                           0xB80178B4
#define SSC2_reg_bypass_pi_disp_mask                       (0x00000001UL)
#define SSC2_get_reg_bypass_pi_disp(data)                  (0x00000001&((UINT32)data))

#define FSYNC_TRACKING_CTRL_reg                            0xB80178C8
#define FSYNC_TRACKING_CTRL_reg_fll_tracking_en_mask       (0x00000010UL)
#define FSYNC_TRACKING_CTRL_reg_fll_tracking_en(data)      (0x00000010&(((UINT32)data)<<4))

#define SS_reg                                             0xB80178C4
#define SS_reg_disp_ss_en_mask                             (0x00000001UL)
#define SS_reg_disp_ss_en(data)                            (0x00000001&((UINT32)data))
#define SS_reg_disp_ss_load_mask                           (0x00000004UL)
#define SS_reg_disp_ss_load(data)                          (0x00000004&(((UINT32)data)<<2))


// sys_reg_reg.h
#define  SYS_REG_SYS_SRST4_reg                                                   0xB8000120
#define  SYS_REG_SYS_SRST4_rstn_hdcp14_mask                                      (0x00001000UL)
#define  SYS_REG_SYS_SRST4_rstn_hdmitx_mask                                      (0x00400000UL)
#define  SYS_REG_SYS_SRST4_rstn_hdmitx_phy_mask                                  (0x00200000UL)
#define  SYS_REG_SYS_SRST4_rstn_hdmitx_off_mask                                  (0x00100000UL)
#define  SYS_REG_SYS_SRST4_write_data_mask                                       (0x00000001UL)

#define  SYS_REG_SYS_CLKEN4_reg                                                  0xB8000130
#define  SYS_REG_SYS_CLKEN4_clken_hdcp14_mask                                    (0x00001000UL)
#define  SYS_REG_SYS_CLKEN4_clken_hdmitx_mask                                    (0x00400000UL)
#define  SYS_REG_SYS_CLKEN4_write_data_mask                                      (0x00000001UL)

// vgip_reg.h
#define  VGIP_VGIP_CHN1_CTRL_reg                                                 0xB8020210
#define  VGIP_VGIP_CHN1_CTRL_ch1_ivrun_mask                                      (0x80000000UL)
#define  VGIP_VGIP_CHN1_CTRL_ch1_ivrun(data)                                     (0x80000000&(((UINT32)data)<<31))
#define  VGIP_VGIP_CHN1_CTRL_get_ch1_ivrun(data)                                 ((0x80000000&((UINT32)data))>>31)


// ppoverlay_reg.h
#define  PPOVERLAY_DV_DEN_Start_End_reg                                          0xB8027018
#define  PPOVERLAY_DV_DEN_Start_End_get_dv_den_end(data)                         (0x00000FFF&((UINT32)data))

#define  PPOVERLAY_new_meas0_linecnt_real_reg                                    0xB8027200
#define  PPOVERLAY_new_meas0_linecnt_real_get_line_cnt_rt(data)                  (0x00003FFF&((UINT32)data))

#define  PPOVERLAY_DV_Length_reg                                                 0xB802700C
#define  PPOVERLAY_DV_Length_get_dv_length(data)                                 (0x000000FF&((UINT32)data))

#define  PPOVERLAY_DTG_d1_output_mux_reg                                         0xB8027580
#define  PPOVERLAY_DTG_d1_output_mux_get_d1_output_mux_sel(data)                 (0x00000001&((UINT32)data))

#define  PPOVERLAY_Display_Timing_CTRL1_reg                                      0xB8027000
#define  PPOVERLAY_Display_Timing_CTRL1_disp_en(data)                            (0x00000001&((UINT32)data))

#define  PPOVERLAY_DH_Width_reg                                                  0xB8027004
#define  PPOVERLAY_DH_Width_get_dh_width(data)                                   (0x000000FF&((UINT32)data))
#define  PPOVERLAY_DH_Width_dh_width_mask                                        (0x000000FFUL)
#define  PPOVERLAY_DH_Width_dh_width(data)                                       (0x000000FF&((UINT32)data))


#define  PPOVERLAY_DV_Length_reg                                                 0xB802700C
#define  PPOVERLAY_DV_Length_get_dv_length(data)                                 (0x000000FF&((UINT32)data))
#define  PPOVERLAY_DV_Length_dv_length_mask                                      (0x000000FFUL)
#define  PPOVERLAY_DV_Length_dv_length(data)                                     (0x000000FF&((UINT32)data))

#define  PPOVERLAY_DV_total_reg                                                  0xB8027010
#define  PPOVERLAY_DV_total_dv_total_mask                                        (0x00000FFFUL)
#define  PPOVERLAY_DV_total_dv_total(data)                                       (0x00000FFF&((UINT32)data))

#define  PPOVERLAY_DH_DEN_Start_End_reg                                          0xB8027014
#define  PPOVERLAY_DH_DEN_Start_End_get_dh_den_sta(data)                         ((0x1FFF0000&((UINT32)data))>>16)
#define  PPOVERLAY_DH_DEN_Start_End_dh_den_sta_mask                              (0x1FFF0000UL)
#define  PPOVERLAY_DH_DEN_Start_End_dh_den_end_mask                              (0x00001FFFUL)
#define  PPOVERLAY_DH_DEN_Start_End_dh_den_sta(data)                             (0x1FFF0000&(((UINT32)data)<<16))
#define  PPOVERLAY_DH_DEN_Start_End_dh_den_end(data)                             (0x00001FFF&((UINT32)data))

#define  PPOVERLAY_DV_DEN_Start_End_reg                                          0xB8027018
#define  PPOVERLAY_DV_DEN_Start_End_get_dv_den_sta(data)                         ((0x0FFF0000&((UINT32)data))>>16)
#define  PPOVERLAY_DV_DEN_Start_End_dv_den_sta_mask                              (0x0FFF0000UL)
#define  PPOVERLAY_DV_DEN_Start_End_dv_den_end_mask                              (0x00000FFFUL)
#define  PPOVERLAY_DV_DEN_Start_End_dv_den_sta(data)                             (0x0FFF0000&(((UINT32)data)<<16))
#define  PPOVERLAY_DV_DEN_Start_End_dv_den_end(data)                             (0x00000FFF&((UINT32)data))


#define  PPOVERLAY_Double_Buffer_CTRL_1_reg                                      0xB8027020
#define  PPOVERLAY_Double_Buffer_CTRL_1_dtg_db_read_sel_mask                     (0x00000002UL)
#define  PPOVERLAY_Double_Buffer_CTRL_1_dtg_db_read_sel(data)                    (0x00000002&(((UINT32)data)<<1))

#define  PPOVERLAY_DH_Total_Length_reg                                           0xB8027008
#define  PPOVERLAY_DH_Total_Length_dh_total_mask                                 (0x1FFF0000UL)
#define  PPOVERLAY_DH_Total_Length_dh_total(data)                                (0x1FFF0000&(((UINT32)data)<<16))

#define  PPOVERLAY_Display_Timing_CTRL1_reg                                      0xB8027000
#define  PPOVERLAY_Display_Timing_CTRL1_disp_fsync_en_mask                       (0x00000010UL)
#define  PPOVERLAY_Display_Timing_CTRL1_disp_fsync_en(data)                      (0x00000010&(((UINT32)data)<<4))
#define  PPOVERLAY_Display_Timing_CTRL1_disp_en_mask                             (0x00000001UL)
#endif

#define HDMITX_OUTPUT_MATCH_DTD_FIRST   1
#define DEBUG_TX_DUMMY_ADDR (0xb80194C0UL)

#define DEBUG_TX_TIME_CHANGE_MASK           _BIT31            // B[31]: timing mode change flag
#define DEBUG_TX_TIME_CHANGE_BIT_SHIFT  31
#define DEBUG_TX_TIME_MODE_MASK               (0x0ff00000UL)    // B[27:20] timing mode
#define DEBUG_TX_TIME_MODE_BIT_SHIFT      20
#define DEBUG_TX_Get_time_Change()         ((rtd_inl(DEBUG_TX_DUMMY_ADDR) & DEBUG_TX_TIME_CHANGE_MASK) >> DEBUG_TX_TIME_CHANGE_BIT_SHIFT)    // B[31] timing change flag
#define DEBUG_TX_Clr_time_Change()         rtd_maskl(DEBUG_TX_DUMMY_ADDR, ~DEBUG_TX_TIME_CHANGE_MASK, 0)    // B[31] timing change flag
#define DEBUG_TX_Get_time_Mode()            ((rtd_inl(DEBUG_TX_DUMMY_ADDR) & DEBUG_TX_TIME_MODE_MASK) >> 20)    // B[27:20] timing mode

#define PTG_TIMING_NUM  3
#define CTS_TIMING_INDEX  (PTG_TIMING_NUM-1)

// Event handler
#ifdef CONFIG_MACH_RTD2801_ZEBU
#define TIMER_HDMITX_STATE_CHECK 1    // reduce delay to 1ms for ZEBU platform
#define TIMER_HDMITX_LT_CHECK 1    // reduce delay to 1ms for ZEBU platform
#define TIMER_HDMITX_LT_STABLE_CHECK 1    // reduce delay to 1ms for ZEBU platform
#define TIMER_HDMITX_SETTING_DONE_STATE_CHECK 1    // timer period to 100ms for hdmitx state check after setting done
#else
#define TIMER_HDMITX_STATE_CHECK 10    // timer period to 10ms for hdmitx state check
#if 0 // [TEST] for CTS
#define TIMER_HDMITX_LT_CHECK 2    // timer period to 2ms for hdmitx link training status check
#else
#define TIMER_HDMITX_LT_CHECK 10    // timer period to 10ms for hdmitx link training status check
#endif
#define TIMER_HDMITX_LT_STABLE_CHECK 100    // timer period to 100ms for hdmitx link training stable status check
#define TIMER_HDMITX_SETTING_DONE_STATE_CHECK 100    // timer period to 100ms for hdmitx state check after setting done
#endif

#define EDID_BUFFER_SIZE        512
#define ScalerHdmiTxSetForceLinkTrainingEn(x) (bForceLinkTrainingFlag=x)
#define ScalerHdmiTxGetForceLinkTrainingEn(x) (bForceLinkTrainingFlag)


// link training timeout
#define _HDMI21_LINK_TRAINING_TIMEOUT_MS  300 // 200msec
#define _HDMI21_LINK_TRAINING_TIMEOUT         (_HDMI21_LINK_TRAINING_TIMEOUT_MS / TIMER_HDMITX_LT_CHECK) //timeout count = 200ms / 10ms = 20

// Macro of HDMI2.1 Link Training control
#define _TIMEOUT_HDMI21_WAIT_LT_READY_MS 100// max loop timeout(100ms) for wait link training state ready
#define _TIMEOUT_HDMI21_WAIT_LT_READY_CNT (_TIMEOUT_HDMI21_WAIT_LT_READY_MS/TIMER_HDMITX_LT_CHECK) // max loop count for wait link training state ready=100/10=10


#define ScalerHdmiTxSetForceLinkTrainingEn(x) (bForceLinkTrainingFlag=x)
#define ScalerHdmiTxGetForceLinkTrainingEn(x) (bForceLinkTrainingFlag)


#ifdef CONFIG_HDMITX_TX_SCRIPT_SUPPORT
#define SCRIPT_TX_SETING_DONE() (hdmitx_init_setting.init_setting.bHdmitxSettingDoneFlag)
#define SCRIPT_RX_SETING_DONE() (hdmitx_init_setting.init_setting.bHdmirxSettingDoneFlag)
#define SCRIPT_TX_ENABLE_PACKET_BYPASS() (hdmitx_init_setting.init_setting.bPktBypassEnableFlag)
#define SCRIPT_TX_SKIP_AVMUTE_BYPASS() (0)
#define SCRIPT_TX_SKIP_AUDIO_BYPASS() (hdmitx_init_setting.init_setting.bSkipAudioPktBypassFlag)
#define SCRIPT_TX_SKIP_EMP_BYPASS() (hdmitx_init_setting.init_setting.bSkipEMPPktBypassFlag)
#define SCRIPT_TX_DTG_FSYNC_EN() (hdmitx_init_setting.init_setting.bDtgFSyncEnable)
#define SCRIPT_TX_VRR_SUPPORT() (hdmitx_init_setting.init_setting.bVrrEnable)
#define SCRIPT_TX_SKIP_EDID_MODIFY() (hdmitx_init_setting.init_setting.bSkipEdidModify)
#else
#define SCRIPT_TX_SETING_DONE() (0)
#define SCRIPT_RX_SETING_DONE() (0)
#define SCRIPT_TX_ENABLE_PACKET_BYPASS() (txInBypassMode) // TX in bypass mode (RX input source is ready to display)
#define SCRIPT_TX_SKIP_AVMUTE_BYPASS() (0)
#define SCRIPT_TX_SKIP_AUDIO_BYPASS() (hdmitx_init_setting.init_setting.bSkipAudioPktBypassFlag)
#define SCRIPT_TX_SKIP_EMP_BYPASS() (0)
#define SCRIPT_TX_DTG_FSYNC_EN() (0)
#define SCRIPT_TX_VRR_SUPPORT() (0)
#define SCRIPT_TX_SKIP_EDID_MODIFY() (hdmitx_init_setting.init_setting.bSkipEdidModify)
#endif


// [Debug] TX test command for debug
#define DEBUG_TX_TEST_CMD_MASK       (0xf0000000UL)    // B[31:28] test cmd
#define DEBUG_TX_TEST_CMD_SHIFT      28

#define DEBUG_TX_TEST_MODE_MASK               (0x0ff00000UL)    // B[27:20] test mode
#define DEBUG_TX_TEST_MODE_BIT_SHIFT      20

#define DEBUG_TX_Get_test_cmd()             ((rtd_inl(DEBUG_TX_DUMMY_ADDR) & DEBUG_TX_TEST_CMD_MASK) >> DEBUG_TX_TEST_CMD_SHIFT)    // B[31:28] test cmd
#define DEBUG_TX_Clr_test_cmd()              rtd_maskl(DEBUG_TX_DUMMY_ADDR, ~DEBUG_TX_TEST_CMD_MASK, 0)    // B[31:28] test cmd

#define DEBUG_TX_Get_test_mode()          ((rtd_inl(DEBUG_TX_DUMMY_ADDR) & DEBUG_TX_TEST_MODE_MASK) >> DEBUG_TX_TEST_MODE_BIT_SHIFT)    // B[27:20] timing mode

#define GET_H_AUDIO_FSM(x) 1
#define AUDIO_FSM_AUDIO_CHECK 1
#define GET_HDMIRX_AUDIO_ST_CHANNEL_NUM() 2
#define newbase_hdmi_get_audio_sample_freq(x) 48000
#define GET_HDMIRX_AUDIO_ST_HBR_SAMPLE_COUNT() 0
#define IS_HDMIR_AUDIO_MUTE() 0

#define drvif_Hdmi_get_audio_sample_freq() 48000



#define TAG_NAME "HDMITX"

//#define ENABLE_HDMITX_DEBUG_LOG
#ifndef ENABLE_HDMITX_DEBUG_LOG
#define FatalMessageHDMITx(format, args...) rtd_pr_hdmitx_emerg( format, ## args)
#define ErrorMessageHDMITx(format, args...) rtd_pr_hdmitx_emerg( format, ## args)
#define WarnMessageHDMITx(format, args...) rtd_pr_hdmitx_warn( format, ## args)
#define NoteMessageHDMITx(format, args...) rtd_pr_hdmitx_notice( format, ## args)
#define InfoMessageHDMITx(format, args...) rtd_pr_hdmitx_info( format, ## args)

#define ErrorMessageHDMI21Tx(format, args...) rtd_pr_hdmitx_emerg( format, ## args)
#define WarnMessageHDMI21Tx(format, args...) rtd_pr_hdmitx_warn( format, ## args)
#define NoteMessageHDMI21Tx(format, args...) rtd_pr_hdmitx_notice( format, ## args)
#define InfoMessageHDMI21Tx(format, args...) rtd_pr_hdmitx_info( format, ## args)

#define ErrorMessageHDMI21TxLinkTraining(format, args...) rtd_pr_hdmitx_emerg( format, ## args)
#define NoteMessageHDMI21TxLinkTraining(format, args...) rtd_pr_hdmitx_notice( format, ## args)
//#define InfoMessageHDMI21TxLinkTraining(format, args...) rtd_pr_hdmitx_emerg(KERN_ERR,  format, ## args)
#define InfoMessageHDMI21TxLinkTraining(format, args...) rtd_pr_hdmitx_info( format, ## args)

#define DebugMessageHDMITx(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageDigital(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageDpTx(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageSystem(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageHDCP14(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageHDCP2(format, args...) rtd_pr_hdmitx_debug( format, ## args)

#define DebugMessageHDMI21Tx(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageHDMI21TxLinkTraining(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageWuzanne(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageInfo(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageTimerEvent(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageIgnore(format, args...) rtd_pr_hdmitx_debug( format, ## args)

#else

#if 1 // dump in FATAL mode

#define DebugMessageHDMITx(format, args...)  rtd_pr_hdmitx_warn( format, ## args)
#define WarnMessageHDMITx(format, args...)  rtd_pr_hdmitx_warn( format, ## args)
#define ErrorMessageHDMI21Tx(format, args...) rtd_pr_hdmitx_err( format, ## args)
#define WarnMessageHDMI21Tx(format, args...) rtd_pr_hdmitx_warn( format, ## args)
#define NoteMessageHDMI21Tx(format, args...) rtd_pr_hdmitx_notice( format, ## args)
#define InfoMessageHDMI21Tx(format, args...) rtd_pr_hdmitx_info( format, ## args)

#define ErrorMessageHDMI21TxLinkTraining(format, args...) rtd_pr_hdmitx_err( format, ## args)
#define NoteMessageHDMI21TxLinkTraining(format, args...) rtd_pr_hdmitx_notice( format, ## args)

#endif

//#define DebugMessageHDMITx(format, args...) rtd_pr_hdmitx_emerg(KERN_DEBUG,  TAG_NAME, format, ## args)
#define DebugMessageDigital(format, args...) cc( format, ## args)
#define DebugMessageDpTx(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageSystem(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageHDCP14(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageHDCP2(format, args...) rtd_pr_hdmitx_debug( format, ## args)

#define DebugMessageHDMI21Tx(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageHDMI21TxLinkTraining(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageWuzanne(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageInfo(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageTimerEvent(format, args...) rtd_pr_hdmitx_debug( format, ## args)
#define DebugMessageIgnore(format, args...) rtd_pr_hdmitx_debug( format, ## args)

#endif

#endif // #ifndef _HDMITX_COMMON_H_


