#ifndef __SCALER_FUNCTION_INCLUDE_H__
#define __SCALER_FUNCTION_INCLUDE_H__

//****************************************************************************
// INCLUDE HEADER FILES
//****************************************************************************
//#include <arch.h>
//#include <mach.h>
//#include <rtk_io.h>

//#include <rtk_kdriver/timer.h>
//#include <timer_event.h>

#include <rbus/hdmitx_misc_reg.h>
#include <rbus/hdmitx20_hpd_detection_reg.h>
#include <rbus/hdmitx20_mac0_reg.h>
#include <rbus/hdmitx20_mac1_reg.h>
#include <rbus/hdmitx20_on_region_reg.h>
#include <rbus/hdmitx21_on_region_reg.h>
#include <rbus/hdmitx21_mac_reg.h>
#include <rbus/hdmitx21_packet_control_reg.h>
#include <rbus/hdmitx21_link_training_reg.h>


#include <rbus/stb_reg.h>
#include <rbus/timer_reg.h>

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************


//------------------------------------------
// Definitions of data type
//------------------------------------------
#define BYTE UINT8

//------------------------------------------
// Definitions of register access
//------------------------------------------
#define rtd_getbits(offset, Mask) (rtd_inl(offset) & Mask)

//#define HIBYTE(val) (val >> 8)
//#define LOBYTE(val) (val & 0xff)
#define LOWORD(l)           ((UINT16)(l))
#define HIWORD(l)           ((UINT16)(((UINT32)(l) >> 16) & 0xFFFF))


//------------------------------------------
// Definitions of data type
//------------------------------------------

// -----------------------------------------
//#define H5X_HDMITX_FIXME
#if 0
//#define H5X_HDMITX_BYPASS_HPD_EDID
#ifdef H5X_HDMITX_BYPASS_HPD_EDID
  #undef _HDMI_HDCP14_TX0_SUPPORT
  #define _HDMI_HDCP14_TX0_SUPPORT _OFF
#endif
#endif

#define TC_HDMI20 0
#define TC_HDMI21 1

// define TX verify mode
#define DV_SCRIPT 0
#define TX_DRIVER 1
#define HWSD_SCRIPT 2
#define TEST_MODE TX_DRIVER //HWSD_SCRIPT// DV_SCRIPT //

#define BYPASS_CMU_PI_WHEN_PLL_RESET // [PHY] bypass cmu_pi during PLL setting change to fix PLL locking fail issue -- 20191025

//#define TC_DSC_ENABLE 0   // UINT8 ScalerHdmiTxGetDSCEn(void);
#define AUDIO_BYPASS 0
//#define TEST_NOT_ENABLE_FRL
//#define TEST_WAIT_OC_DONE
//#define _DEBUG_HOLD_LT3_MODE

typedef enum
{
    DSC_SRC_TXSOC=0,
    DSC_SRC_DISPD,
}EnumHDMITXDscSrc;

#define HDMITX_DSC_SRC DSC_SRC_TXSOC

//#ifdef H5X_HDMITX_FIXME
// NOT HDMI TX HW register
#if 1 // NOT HDMITX HW REG.
#define P0_9A_FW_USE_1 0

#define VIRTUAL_DUMMY_REGISTER 0x0

#define P63_10_HDCP_RSA_CTRL0 VIRTUAL_DUMMY_REGISTER
#define P63_11_HDCP_RSA_CTRL1 VIRTUAL_DUMMY_REGISTER
#define P63_12_HDCP_RSA_NP_INV_3 VIRTUAL_DUMMY_REGISTER
#define P63_16_HDCP_RSA_DATA_ADDR VIRTUAL_DUMMY_REGISTER
#define P63_17_HDCP_RSA_DATA_IN_DATA7 VIRTUAL_DUMMY_REGISTER
#define P63_20_HDCP_RSA_DATA_OUT_DATA7 VIRTUAL_DUMMY_REGISTER
#define P63_28_HDCP_RSA_TX_FUNTION1 VIRTUAL_DUMMY_REGISTER
#define P63_29_HDCP_RSA_TX_FUNTION0 VIRTUAL_DUMMY_REGISTER
#define P63_35_HDCP_SHA_0 VIRTUAL_DUMMY_REGISTER
#define P63_92_HDCP_DUMMY1 VIRTUAL_DUMMY_REGISTER

#define PB_17_DP_HDCP_CONTROL VIRTUAL_DUMMY_REGISTER
#define PB_1A_HDCP_IRQ VIRTUAL_DUMMY_REGISTER
#define PB_1E_HDCP_INTGT_VRF_ANS_MSB VIRTUAL_DUMMY_REGISTER
#define PB_1F_HDCP_INTGT_VRF_ANS_LSB VIRTUAL_DUMMY_REGISTER
#define PB_20_HDCP_DEBUG VIRTUAL_DUMMY_REGISTER
#define PB_30_SHA_CONTRL VIRTUAL_DUMMY_REGISTER
#define PB_31_SHA_DATA1 VIRTUAL_DUMMY_REGISTER
#define PB_32_SHA_DATA2 VIRTUAL_DUMMY_REGISTER
#define PB_33_SHA_DATA3 VIRTUAL_DUMMY_REGISTER
#define PB_34_SHA_DATA4 VIRTUAL_DUMMY_REGISTER
#define PB_36_SHA_0 VIRTUAL_DUMMY_REGISTER
#define PB_37_SHA_1 VIRTUAL_DUMMY_REGISTER
#define PB_38_SHA_2 VIRTUAL_DUMMY_REGISTER
#define PB_39_SHA_3 VIRTUAL_DUMMY_REGISTER

// [FIXME] 20190314
#define PB8_1F_SOURCE_SEL_4 VIRTUAL_DUMMY_REGISTER

// [FIXME] 20190314
#define PBB_5B_DP_HDCP_TX_SHA_CTRL VIRTUAL_DUMMY_REGISTER
#define PBB_70_HPD_CTRL VIRTUAL_DUMMY_REGISTER
#define PBB_71_HPD_IRQ VIRTUAL_DUMMY_REGISTER
#define PBB_90_HDCP22_CTRL VIRTUAL_DUMMY_REGISTER
#define PBB_E0_DPTX_CLK_GEN VIRTUAL_DUMMY_REGISTER
#endif // NOT HDMITX HW REG.

#if 1 // NOT FOR HDMI TX DRIVER FLOW
#define _CABLE_IN 0
#define _CABLE_OUT 1
#define _DP_TX1_HDMI_TX0 0
#define _DP_INFO_FREESYNC 0
#define _DP_INFO_HDR 0
#define _FUNCTION_ON 1
#define _HDMI_OUTPUT_TX0 _P0_NO_PORT
#define _RGB2YUV_ITU709_LIMIT 0
#define _RGB2YUV_ITU709_FULL 1

#define _AUDIO_CHANNEL_COUNT_2 2

#define _XTAL27000K 27000000

#define _DSC_BYPASS_PIXEL_CLK (ScalerDscDecoderGetElement(_P0_OUTPUT_PORT, _DSC_DECODER_PIXEL_CLK))

#define _SUPPORT_422_TO_444 _ON
#define _SUPPORT_444_TO_420 _ON
#define _SUPPORT_422_TO_420 _ON

// [FIXME] 20190318
#define _HW_HDMI21_SET_FRL_RATE 0//5 // H5X support MAX FRC_RATE=12Gps/lane, 4 lane
#define _HW_HDMI21_SET_FFE_LEVEL 0 //H5X support FFE 0~3

// [FIXME] 20190314
#define _EDID_ADDRESS 0x50 //0xA0 >>1
#define _HDCP_ADDRESS 0x3A //0x74 >>1
#define _SCDC_ADDRESS 0x54 //0xA8 >>1
#define _EDDC_SEG_POINT 0x30 // 0x60 >> 1

#define _HDMI_TX0_IIC          0//_P1_PCB_EDID_IIC
#define _HDMI21_TX0_IIC      0

#define _HDMI_21_TX_TOTAL_NUMBER 1

#define _OUTPUT_PORT_HDMI_TX0 _P0_NO_PORT
#define _OUTPUT_PORT_HDMI_TX1 _P1_NO_PORT
#define _OUTPUT_PORT_HDMI_TX2 _P2_NO_PORT
#define _OUTPUT_PORT_HDMI_TX3 _P3_NO_PORT
#define _NO_OUTPUT_PORT _P0_NO_PORT

typedef enum _HDMITX_PORT_NUM{
    _TX0 = 0,
    _TX1,
    _TX2,
    _TX3,
    _TX_MAP_NONE=255,
}HDMITX_PORT_NUM;

#define _HW_P0_HDMI_PHY_TX_MAPPING _TX0
#define _HW_P1_HDMI_PHY_TX_MAPPING _TX0
#define _HW_P2_HDMI_PHY_TX_MAPPING _TX0
#define _HW_P3_HDMI_PHY_TX_MAPPING _TX0
#define _HW_P4_HDMI_PHY_TX_MAPPING _TX0

#define _HW_P0_HDMI_MAC_TX_MAPPING _TX0
#define _HW_P1_HDMI_MAC_TX_MAPPING _TX0
#define _HW_P2_HDMI_MAC_TX_MAPPING _TX0
#define _HW_P3_HDMI_MAC_TX_MAPPING _TX0
#define _HW_P4_HDMI_MAC_TX_MAPPING _TX0

#define _HW_P0_HDMI21_MAC_TX_MAPPING _TX0
#define _HW_P1_HDMI21_MAC_TX_MAPPING _TX0
#define _HW_P2_HDMI21_MAC_TX_MAPPING _TX0
#define _HW_P3_HDMI21_MAC_TX_MAPPING _TX0

#define _HDCP_2_2_DKEY_0 0
#define _HDCP_2_2_DKEY_1 1
#define _HDCP_2_2_DKEY_2 2

#define TL2 8

typedef enum _EnumScalerTimerEventHdmiTx0Type{
    _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_CHECK=0x30,
    _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_ENABLE,
    _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_DISABLE,
    _SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT,
    _SCALER_TIMER_EVENT_HDMI_TX0_HPD_TOGGLE_EVENT,
    _SCALER_TIMER_EVENT_HDMI_TX0_Z0_TOGGLE_EVENT,
    _SCALER_TIMER_EVENT_HDMI_TX0_AUDIO_CTS_CODE_CHECK,
    _SCALER_TIMER_EVENT_HDM21_TX0_FRL_PASS_TIMER,
    _SCALER_TIMER_EVENT_HDM21_TX0_LTS3_TIMER,
    _SCALER_TIMER_EVENT_HDMI_TX0_GCPTOHW_TIMER,
    _SCALER_TIMER_EVENT_NUM
}EnumScalerTimerEventHdmiTx0Type;


typedef enum _EnumScalerWdTimerEventHdmiTx0Type{
    _SCALER_WD_TIMER_EVENT_HDMI_TX0_HPD_FALLING_DEBOUNCE,
}EnumScalerWdTimerEventHdmiTx0Type;

#endif // NOT FOR HDMI TX DRIVER FLOW

#if 1 // NOT FOR HDMI TX DRIVER FLOW
#define _DP_FOUR_LANE 1

#define _OUTOUT_PORT_TYPE_NONE 0
#define _OUTOUT_PORT_TYPE_HDMI 1
#define _OUTOUT_PORT_TYPE_DVI 2
#define _OUTOUT_PORT_TYPE_DP 3
#define _OUTOUT_PORT_TYPE_VGA 4

#define _P0_NO_PORT _OUTOUT_PORT_TYPE_HDMI
#define _P1_NO_PORT _OUTOUT_PORT_TYPE_HDMI
#define _P2_NO_PORT _OUTOUT_PORT_TYPE_HDMI
#define _P3_NO_PORT _OUTOUT_PORT_TYPE_HDMI

#define _P0_OUTPUT_TYPE _OUTOUT_PORT_TYPE_HDMI
#define _P1_OUTPUT_TYPE _OUTOUT_PORT_TYPE_HDMI
#define _P2_OUTPUT_TYPE _OUTOUT_PORT_TYPE_HDMI
#define _P3_OUTPUT_TYPE _OUTOUT_PORT_TYPE_HDMI

#define _MAX_NO_PORT 1
//#define _HDMI_TX_HDCP_DEVICE_COUNT_MAX _MAX_NO_PORT
#define _HDMI_HDCP_MAX_RETRY_COUNT 3
#define _HDMI_HDCP_MAX_RI_RETRY_COUNT 3

typedef enum _EnumPortType{
    _PORT_TYPE_NONE = 0x00,
    _PORT_TYPE_HDMI,
    _PORT_TYPE_DVI,
    _PORT_TYPE_DP,
    _PORT_TYPE_VGA
} EnumPortType;

typedef enum _EnumDfpType{
    _DFP_TYPE_NONE = 0x00,
    _DFP_TYPE_HDMI,
    _DFP_TYPE_DVI,
    _DFP_TYPE_DP,
    _DFP_TYPE_VGA
} EnumDfpType;


typedef enum _EnumDfpVersion{
    _DFP_VERSIOIN_NONE = 0x00,
    _DFP_VERSION_1_4,
    _DFP_VERSION_2_0,
} EnumDfpVersion;


typedef enum _EnumOutputPort{
    _P0_OUTPUT_PORT=0,
    _P1_OUTPUT_PORT,
    _P2_OUTPUT_PORT,
    _P3_OUTPUT_PORT,
    _P4_OUTPUT_PORT
}EnumOutputPort;

typedef enum _EnumAudioFreqType{
    _AUDIO_FREQ_NO_AUDIO=0,
    _AUDIO_FREQ_32K,
    _AUDIO_FREQ_64K,
    _AUDIO_FREQ_128K,
    _AUDIO_FREQ_44_1K,
    _AUDIO_FREQ_88_2K,
    _AUDIO_FREQ_176_4K,
    _AUDIO_FREQ_48K,
    _AUDIO_FREQ_96K,
    _AUDIO_FREQ_192K,
    _AUDIO_FREQ_256K,
    _AUDIO_FREQ_352_8K,
    _AUDIO_FREQ_384K,
    _AUDIO_FREQ_512K,
    _AUDIO_FREQ_705_6K,
    _AUDIO_FREQ_768K
}EnumAudioFreqType;

typedef enum _EnumDPRxHDCPUpstreamAuthEvent{
    _DP_RX_HDCP2_UPSTREAM_AUTH_EVENT_R0_PASS=0,
    _DP_RX_HDCP2_UPSTREAM_AUTH_EVENT_STOP_ENCRYPTION,
    _DP_RX_HDCP2_UPSTREAM_AUTH_EVENT_TIMEOUT,
    _DP_RX_HDCP2_STATE_AUTH_DONE,
}EnumDPRxHDCP2UpstreamAuthEvent;


typedef enum _EnumDPRxHDCP2UpstreamAuthEvent{
    _DP_RX_HDCP_UPSTREAM_AUTH_EVENT_R0_PASS=0,
    _DP_RX_HDCP2_UPSTREAM_AUTH_EVENT_START_HANDSHAKE,
    _DP_RX_HDCP_UPSTREAM_AUTH_EVENT_STOP_ENCRYPTION,
    _DP_RX_HDCP_UPSTREAM_AUTH_EVENT_TIMEOUT,
    _DP_RX_HDCP_STATE_AUTH_DONE,
}EnumDPRxHDCPUpstreamAuthEvent;


typedef enum _EnumDPRxHDCP2RepeaterState{
    _DP_RX_HDCP2_REPEATER_STATE_AUTH_DONE=0
}EnumDPRxHDCP2RepeaterState;


typedef struct{
    UINT8 pucHdcp2Lc[16];
}RxHdcp2DownLoadKeyType;



typedef enum _EnumDownStreamType{
    _DOWN_STREAM_NONE=0,
    _DOWN_STREAM_VESA,
    _DOWN_STREAM_DSC,
}EnumDownStreamType;


typedef enum _EnumEdidReadType{
    _EDID_ORIGIN_READ=0,
    _EDID_SEGMENT_READ
}EnumEdidReadType;
typedef struct {
    UINT8           addr;
    UINT8          sub_addr_len;
    UINT16          read_len;
    UINT16          wr_len;
} I2C_HDMITX_PARA_T;
typedef enum _EnumEdidVersionType{
    _EDID_VER13=0,
    _EDID_VER14,
    _EDID_VER20,
}EnumEdidVersionType;

 typedef enum
{
    _DSC_DECODER_PIXEL_CLK,
    _DSC_DECODER_COLOR_SPACE,
    _DSC_DECODER_COLOR_DEPTH,
    _DSC_DECODER_NATIVE_420,
    _DSC_DECODER_PIC_WIDTH,
    _DSC_DECODER_SLICE_WIDTH,
    _DSC_DECODER_BPP,
    _DSC_DECODER_CHUNK_SIZE,
 } EnumDscDecoderElement;


// _ST4_EXIST -- NOT SUPPORT ST4 AUDIO NOW
#define GET_AUDIO_ST4_CHANNEL_COUNT() (0)
#define GET_AUDIO_ST4_CODING_TYPE() (0)
#define GET_AUDIO_ST4_SAMPLING_FREQ() (0)

// _HDMI_FREESYNC_TX0_SUPPORT -- NOT SUPPORT AMD_SPD_INFO (FREE_SYNC) NOW
#define ScalerMultiStreamGetFreeSyncHDRData(x,y...)
#define GET_ST_DP_AMD_SPD_INFO_FRAME_RECEIVED(x) (0)
#define GET_ST_DP_AMD_SPD_INFO_FRAME_CHANGE(x) (0)
#define CLR_ST_DP_AMD_SPD_INFO_FRAME_CHANGE(x)
#define GET_ST_FREESYNC_INFO_FM_LENGTH(x) (0)
#define GET_ST_FREESYNC_INFO_FM_VERSION(x) (0)

// _HDMI_HDR10_TX0_SUPPORT -- NOT SUPPORT HDR10 NOW
#define _MULTI_ST4 0
#define ScalerMultiStreamDpSdpGetData(x,y...)
#define GET_ST_DP_HDR_INFO_FRAME_RECEIVED(x) (0)
#define GET_ST_DP_HDR_INFO_FRAME_CHANGE(x) (0)
#define CLR_ST_DP_HDR_INFO_FRAME_CHANGE(x)
#define GET_ST_HDR10_INFO_FM_LENGTH(x) (0)
#define GET_ST_HDR10_INFO_FM_VERSION(x) (0)

#define GET_DP_HDCP2_RX0_MODE() (0)
#define GET_DP_HDCP2_RX0_CONTENT_STREAM_MANAGEMENT() (0)
#define GET_DP_HDCP2_RX0_AUTH_STATE() (0)


#define SET_HDMI_MAC_TX0_BW_AUDIO_MUTE()
#define SET_HDMI21_MAC_TX_BW_AUDIO_MUTE(x)
//#define ScalerHdmi21MacTx0HVPol()

// NOT BE USED VARIABLE
#define SET_ST4_COLOR_SPACE(x)

// HDCP control
#define GET_DP_RX0_HDCP_AUTH_STATE() (_DP_RX_HDCP_STATE_AUTH_DONE)
#define GET_DP_RX0_HDCP2_AUTH_STATE() (_DP_RX_HDCP2_REPEATER_STATE_AUTH_DONE)
#define GET_DP_RX0_HDCP2_CONTENT_STREAM_MANAGEMENT() (0)
#define GET_DP_RX0_HDCP2_MODE() (0)
#define ScalerDpRx0GetDpcdInfo(x,y...) (0)
#define ScalerDpRx0HdcpLoadReceiverId(x,y)
#define ScalerSyncDpHdcpGetUpstreamEncryptStatus() (0)
#define ScalerSyncHdcpGetUpstreamEvent(x) (0)
#define ScalerSyncHdcp2GetUpstreamEvent(x) (0)
#define ScalerHdcp2DkeyCalculate(x,y...)
#define ScalerHdcp2HPrimeCalculate(x,y...) (0)
#define ScalerHdcp2RxSha256Calculate(x,y) (0)
#define ScalerHdcp2LPrimeCalculate(x,y...) (0)
#define ScalerHdcp2VprimeCalculate(x,y...) (0)
#define ScalerHdcp2RepeaterHmacSha256Calculate(x,y,...) (0)
#define ScalerTxCableStatusUpdate(x,y)

//#define ScalerOutputGetPxPortDdcRamAddr(x) ((UINT8 *)VIRTUAL_DUMMY_REGISTER)
//#define ScalerEdidGetCtaExtDbAddress(x,y)
#define ScalerEdidGetDidExtStartAddress(x) (VIRTUAL_DUMMY_REGISTER)
#define ScalerEdidGetDidExtDbAddress(x,y)
#define ScalerEdidGetExtDbAddress(x,y)

#define ScalerEdidReduceBlankTimingModify(x,y)
//#define ScalerEdidGeneralStructParse(x)
//#define ScalerEdidExtBlockDbParse(x)
#define ScalerMcuLoadDefaultEdidByTxPort(x)

#define ScalerDpStreamSTis3DVideo(x) (0)
#define ScalerDpStreamGetAudioFwTrackingStableStatus(x) (0)
#define ScalerDpStreamGetInterlaceVttEven(x) (0)
#define ScalerDpStreamPxSTMapping(x) (0)

#define ScalerDpStreamDpSdpGetData(x,y,z)
#define ScalerDpStreamGetOneFrameDelay(x) (0)
#define ScalerDpStreamAudioVerticalBlanking(x) (0)

//#define ScalerEdidGetMaxDdcramSize(x) (256)
//#define ScalerEdidCheckSumCorrectCheck(x,y) (0)
//#define ScalerEdidCheckSumCal(x,y)
//#define ScalerEdidDidCheckSumCal(x,y,z)

#define ScalerOutputGetDeviceCurrentDeviceType(x) (1)
#define ScalerHdcpTxGetUpstreamEncryptStatus(x) (0)
#define ScalerDpAuxRxGetDpcdInfo(x,y...) (0)
#define ScalerDpStreamHdcpOutputVideoProc(x,y)

#define ScalerPowerGroupHDMITxIsolation(x,y)
#define ScalerPowerGroupHDMITxPowerCut(x,y)
#define ScalerPowerGroupHDMITxPowerCutReset(x,y)

#define ScalerHdmiPhyTx0TimerReset()
#define ScalerGDIPhyHdmiTxHdmi21SetTxFfeLevel(a,b,c,d)

#define ScalerDscPower(x,y) // FIXME
#define ScalerDscVesaMuxSel(x,y) // FIXME
#define ScalerDscClrBwEvent(x) // FIXME

#define ScalerDscGetBwEvent(x) (0) // FIXME
#define ScalerDscBandWidthCheck(x) (0) // FIXME
#define ScalerDscEncoderHandler(x) (0) // FIXME

#define ScalerDscDecoderHandler(x) (0) // FIXME
#define ScalerDscDecoderGetPpsDataPreCheck(x) (0) // FIXME
#define ScalerDscDecoderStreamCapabilityCheck(x) (0) // FIXME
#define ScalerDscDecoderGetPixelMode(x) (2) // 2: 4 pixel mode
#define ScalerDscDecoderGetDisplayFormat(x) (0) // FIXME

#define ScalerDpStreamSetTxCurrentOperatePixelMode(x,y)
#define ScalerGDIPhyHdmiTxHdmi21RingCMUPowerOn(x,y)
#define ScalerGDIPhyHdmiTxHdmi21MLPHYSet(x)


#define GET_DP_STREAM_FREESYNC_INFO_FM_VERSION(x)
#define GET_DP_STREAM_FREESYNC_INFO_FM_LENGTH(x)

#define GET_DP_STREAM_HDR_INFO_FRAME_RECEIVED(x) (1)
#define GET_DP_STREAM_HDR_INFO_FRAME_CHANGE(x) (0)
#define CLR_DP_STREAM_HDR_INFO_FRAME_CHANGE(x)

#define GET_DP_STREAM_HDR10_INFO_FM_VERSION(x) (0)
#define GET_DP_STREAM_HDR10_INFO_FM_LENGTH(x) (0)

#define GET_DP_STREAM_AMD_SPD_INFO_FRAME_RECEIVED(x)
#define GET_DP_STREAM_AMD_SPD_INFO_FRAME_CHANGE(x)
#define CLR_DP_STREAM_AMD_SPD_INFO_FRAME_CHANGE(x)

// DFP DEVICE INFO NOT USED in current application
#define SET_OUTPUT_DFP_DEVICE_CURRENT_TYPE(x,y)
#define SET_OUTPUT_DFP_DEVICE_VERSION(x,y)
#define SET_OUTPUT_DFP_DEVICE_VALID(x)
#define CLR_OUTPUT_DFP_DEVICE_VALID(x)
#define SET_OUTPUT_DFP_DEVICE_CURRENT_SINK_COUNT(x,y)

#define CLR_OUTPUT_DFP_DEVICE_CAP_CHANGE(port) (bOutoutDfpDeviceCapChange &= ~( 1 << port))
#define SET_OUTPUT_DFP_DEVICE_CAP_CHANGE(port) (bOutoutDfpDeviceCapChange |= (1 << port))
#define GET_OUTPUT_DFP_DEVICE_CAP_CHANGE(port) (bOutoutDfpDeviceCapChange & (1 << port)? _TRUE: _FALSE)

#define CLR_OUTPUT_DFP_DEVICE_CHANGE(port) (bOutputDfpDeviceChange &= ~(1 << port))
#define SET_OUTPUT_DFP_DEVICE_CHANGE(port) (bOutputDfpDeviceChange |= (1 << port))
#define GET_OUTPUT_DFP_DEVICE_CHANGE(port) (bOutputDfpDeviceChange & (1 << port)? _TRUE: _FALSE)

//--------------------------------------------------
// Macros of HDMI Tx HDP Infomation
//--------------------------------------------------
extern BOOLEAN g_bHDMITx0FirstHPD;
#define GET_HDMI_TX0_HPD_FIRST_PLUG()        (g_bHDMITx0FirstHPD)
#define SET_HDMI_TX0_HPD_FIRST_PLUG()        (g_bHDMITx0FirstHPD = _TRUE)
#define CLR_HDMI_TX0_HPD_FIRST_PLUG()        (g_bHDMITx0FirstHPD = _FALSE)
extern UINT8 pEdidData[16];
//extern UINT8 MCU_DDCRAM_P0[0x100];
extern UINT8 subEdidAddr[6];
extern UINT8 bOutoutDfpDeviceCapChange;
extern UINT8 bOutputDfpDeviceChange;

#define _CTA_TOTAL_DB (0x10) // [FIX-ME]
#define _DID_TOTAL_DB (0x10) // [FIX-ME]
#define _DID_T1_DB (0x10) // [FIX-ME]
#define _CTA_EXT_VDB_DB (0x4) // [FIX-ME]
#define _CTA_EXT_420VDB_DB (0x4) // [FIX-ME]
#define _ST_MAP_NONE 0
#define _AUTOINC 0
#define _D0_INPUT_PORT 0
#define EDID_VDB_VIC_LEN        31
#define EDID_Y420CMDB_LEN        4
#define PDATA_WORD(x) (*((UINT16*)&pEdidData[x]))
#define PDATA_DWORD(x) (*((UINT32*)&pEdidData[x]))
#define TO_DWORD(byte_hh,byte_hl,byte_lh,byte_ll) (((UINT32)(UINT8)(byte_hh) << 24)|\
                                                                                ((UINT32)(UINT8)(byte_hl) << 16)|\
                                                                                ((UINT32)(UINT8)(byte_lh) << 8)|\
                                                                                ((UINT32)(UINT8)(byte_ll) << 0))

#endif // NOT FOR HDMI TX DRIVER FLOW

// ---------------------------------------
//#endif // #ifdef H5X_HDMITX_FIXME

//#define CONFIG_HDMITX_AP_TEST
//--------------------------------------------------
// Enumerations Multi Stream Field Type
//--------------------------------------------------
typedef enum
{
    _COLOR_SPACE,
    _COLOR_DEPTH,
    _PIXEL_CLOCK,
    _FRAME_RATE,
    _DIGITAL_QUANTIZATION_PREVALUE,
    _COLORIMETRY,
    _COLOR_RGB_QUANTIZATION_RANGE,
    _COLOR_YCC_QUANTIZATION_RANGE,
    _EXT_COLORIMETRY,
    _INTERLACE_INFO,
    _OUTPUT_PIXEL_MODE,
} EnumMultiStreamElement;


//--------------------------------------------------
// Enumerations of Quantization Range (RGB)
//--------------------------------------------------
typedef enum
{
    _AVI_RGB_QUANTIZATION_DEFAULT = 0x00,
    _AVI_RGB_QUANTIZATION_LIMITED = 0x01,
    _AVI_RGB_QUANTIZATION_FULL = 0x02,
    _AVI_RGB_QUANTIZATION_RESERVED = 0x03,
} EnumAviInfoRGBQuantization;

//--------------------------------------------------
// Enumerations of Quantization Range (YCC)
//--------------------------------------------------

typedef enum
{
    _AVI_YCC_QUANTIZATION_LIMITED = 0x00,
    _AVI_YCC_QUANTIZATION_FULL = 0x01,
    _AVI_YCC_QUANTIZATION_RESERVED=0x02,
} EnumAviInfoYCCQuantization;

//--------------------------------------------------
// Enumerations of Color Space
//--------------------------------------------------
typedef enum
{
    _COLOR_SPACE_RGB,
    _COLOR_SPACE_YCBCR422,
    _COLOR_SPACE_YCBCR444,
    _COLOR_SPACE_YCBCR420,
    _COLOR_SPACE_YPBPR,
    _COLOR_SPACE_Y_ONLY,
    _COLOR_SPACE_RAW,
} EnumColorSpace;

//--------------------------------------------------
// Enumerations of Audio Info Type
//--------------------------------------------------
typedef enum
{
    _AUDIO_CHANNEL_COUNT = 0x00,
    _AUDIO_CODING_TYPE,
    _AUDIO_SAMPLING_FREQUENCY_TYPE,
    _AUDIO_SAMPLING_FREQUENCY_HZ,
} EnumAudioInfoType;


typedef enum
{
    _TX_AUDIO_IN_RX_PACKET = 0x00,
    _TX_AUDIO_IN_RX_SPDIF,
    _TX_AUDIO_IN_NUM,
} EnumHdmiTxAudioInputType;

#if(_HDMI_TX_SUPPORT == _ON)
//--------------------------------------------------
// Structure of HDMI Tx
//--------------------------------------------------
// Struct for all Timing Info
typedef struct
{
    UINT8 b1HSP : 1;             // HSP
    UINT8 b1VSP : 1;             // VSP
    UINT8 b1Interlace : 1;       // Interlace
    UINT8 b1VideoField : 1;      // Field for video compensation
    UINT16 usHFreq;               // Horizontal Freq. (unit: 0.1kHz)
    UINT16 usHTotal;              // Horizontal Total length (unit: Pixel)
    UINT16 usHWidth;              // Horizontal Active Width (unit: Pixel)
    UINT16 usHStart;              // Horizontal Start (unit: Pixel)
    UINT16 usHSWidth;             // Horizontal Sync Pulse Count (unit: SyncProc Clock)

    UINT16 usVFreq;               // Vertical Freq. (unit: 0.1Hz)
    UINT16 usVTotal;              // Vertical Total length (unit: HSync)
    UINT16 usVHeight;             // Vertical Active Height (unit: HSync)
    UINT16 usVStart;              // Vertical Start (unit: HSync)
    UINT16 usVSWidth;             // Vertical Sync Width    (unit: HSync)
} StructTimingInfo;



typedef struct
{
    UINT16 usPixelClock;
    UINT16 usHWidth;
    UINT16 usHStart;
    UINT16 usHSWidth;
    UINT16 usHTotal;
    UINT16 usHCactive;
    UINT16 usHCblank;
    UINT16 usVHeight;
    UINT16 usVStart;
    UINT16 usVSWidth;
    UINT16 usVTotal;
    UINT16 usHCSWidth;
    UINT16 ucSilceCountCap;
    UINT16 ucPixelMode;
    UINT16 usBitsPerPixel;
    UINT16 usDscStrClock;
    UINT16 b2SliceCount;
    UINT16 b4BitsPerComponent;
    UINT16 b4DscVersionMinor;
    UINT16 b1BlockPredEnable;
    UINT16 b1UseYuvInput;
    UINT16 b1Simple422;
    UINT16 b1Native422;
    UINT16 b1Native420;
}StructDscEncoderParameter;



typedef struct
{
    UINT16 usLcRetryCounter;
    UINT8 ucAuthDownstreamEvent;
    UINT8 b4AuthState : 4;
    UINT8 b1AuthStateChange : 1;
    UINT8 b1VReadyTimeout : 1;
    UINT8 b1PollingVReady : 1;
    UINT8 b1VReadyBit : 1;
    UINT8 b1AuthStart : 1;
    UINT8 b1EncryptDataTimeout : 1;
    UINT8 b1Hdcp2Version:1;
    UINT8 b1Hdcp2Capable:1;
    UINT8 b1Hdcp2Repeater:1;
    UINT8 b1CertificateTimeout:1;
    UINT8 b1HprimeTimeout:1;
    UINT8 b1LprimeTimeout:1;
    UINT8 b1WriteMTimeout:1;
    UINT8 b1LcAlreadySent:1;
    UINT8 b5RxInfoDevice:5;
    UINT8 b1RxInfoDeviceExceed:1;
    UINT8 b3RxInfoDepth:3;
    UINT8 b1RxInfoDepthExceed:1;
    UINT8 b1Hdcp20RepeaterDownstream:1;
    UINT8 b1Hdcp1DeviceDownstream:1;
    UINT8 b1AuthContentStreamManage:1;
    UINT8 b1MprimeTimeout:1;
    UINT8 b1PollingFail:1;
} StructHdcp2TxAuthInfo;


typedef struct {
    UINT8 b1TimingCompensationFlag;
    UINT8 b2HSyncCompensate;
    UINT8 b2HFrontPorchCompensate;
    UINT8 b2HBackPorchCompensate;
    UINT8 b2HWidthCompensate;
    UINT8 b4HDirectionCompensate;
}StructStreamDisplayCompensateInfo;

//--------------------------------------------------
//HDCP2 Auth State
//--------------------------------------------------
typedef enum
{
    _HDCP_2_2_TX_STATE_IDLE = 0,
    _HDCP_2_2_TX_STATE_AKE_INIT,
    _HDCP_2_2_TX_STATE_VERIFY_SIGNATURE,
    _HDCP_2_2_TX_STATE_NO_STORED_KM,
    _HDCP_2_2_TX_STATE_STORED_KM,
    _HDCP_2_2_TX_STATE_VERIFY_H_PRIME,
    _HDCP_2_2_TX_STATE_PAIRING,
    _HDCP_2_2_TX_STATE_LOCALITY_CHECK,
    _HDCP_2_2_TX_STATE_SKE,
    _HDCP_2_2_TX_STATE_VERIFY_V_PRIME,
    _HDCP_2_2_TX_STATE_AUTHEN_DONE,
} EnumHdcp2TxAuthState;



//--------------------------------------------------
// Enumerations of EDID modify event
//--------------------------------------------------
typedef enum
{
    _NO_EDID_MODIFY=0,
    _INITIAL_MODIFY,
    _BANDWIDTHW_MISMATCH_MODIFY,
    _VIDEO_BANDWIDTH_MODIFY,
    _AUDIO_BANDWIDTH_MODIFY,
}EnumHdmiEdidModifyEvent;


//--------------------------------------------------
// Enumerations of EDID CTA Data Block
//--------------------------------------------------
typedef enum
{
    _CTA_VDB=0,
    _CTA_ADB,
    _CTA_VCDB,
    _CTA_420VDB,
    _CTA_420CMDB,
    _CTA_CDB,
    _CTA_SHDR_DB,
    _CTA_DHDR_DB,
    _CTA_FS_DB,
    _CTA_HF_EEODB,
    _CTA_HF_VSDB,
    _CTA_HDMI_VSDB,
    _CTA_HF_SCDB,
    _CTA_VSVDB_HDR10,
    _DID_DTDDB,
}EnumEdidCtaDataBlock;

#endif // End of #if(_HDMI_TX_SUPPORT == _ON)

//--------------------------------------------------
// Definitions of Color Depth
//--------------------------------------------------
#define _DP_STREAM_COLOR_DEPTH_6_BITS                     6
#define _DP_STREAM_COLOR_DEPTH_7_BITS                     7
#define _DP_STREAM_COLOR_DEPTH_8_BITS                     8
#define _DP_STREAM_COLOR_DEPTH_10_BITS                    10
#define _DP_STREAM_COLOR_DEPTH_12_BITS                    12
#define _DP_STREAM_COLOR_DEPTH_14_BITS                    14
#define _DP_STREAM_COLOR_DEPTH_16_BITS                    16


//--------------------------------------------------
// Macros of HDMI Tx HDP Infomation
//--------------------------------------------------
//--------------------------------------------------
// Macro of Output Port Proc Done Flag
//--------------------------------------------------


//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************

//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************
extern UINT8 g_usDpHdcpRx0ReceiverIdListIndex;

//****************************************************************************
// VARIABLE FUNCTION
//****************************************************************************
#endif // #ifndef __SCALER_FUNCTION_INCLUDE_H__
