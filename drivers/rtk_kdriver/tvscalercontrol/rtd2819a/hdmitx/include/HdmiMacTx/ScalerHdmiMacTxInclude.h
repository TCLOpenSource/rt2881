#ifndef __SCALER_HDMI_MAC_TX_INCLUDE_H__
#define __SCALER_HDMI_MAC_TX_INCLUDE_H__

/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2011>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : ScalerHdmiMacTxInclude.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include "../ScalerFunctionInclude.h"

#if(_HDMI_TX_SUPPORT == _ON)

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
//----------------------------------------------------------------------------
// HPD Denounce Time (ms)
//----------------------------------------------------------------------------
#define _TOGGLE_DEBOUNCE_TIME                               90
#define _UNPLUG_DEBOUNCE_TIME                               (300 - _TOGGLE_DEBOUNCE_TIME)
#define _PLUG_DEBOUNCE_TIME                                 30
#define _HPD_FALLING_TIME                                   10
#define _GCP_TO_HW_TIME                                   10
//----------------------------------------------------------------------------
// Macro of HDMI Tx SSC
//----------------------------------------------------------------------------
#define _HDMI_TX_SSC_30K                                    30
#define _HDMI_TX_SSC_32K                                    32
#define _HDMI_TX_SSC_33K                                    33
#define _HDMI_TX_SSC_FREQ                                   _HDMI_TX_SSC_32K

//----------------------------------------------------------------------------
// Macro HDMI Tx SSC
//----------------------------------------------------------------------------
#define _HDMI_TX_CLK_GEN_BY_SYNC_PATTERN                    _ON

//----------------------------------------------------------------------------
// Aspect Radio
//----------------------------------------------------------------------------
#define _RATIO_4_3                                          1
#define _RATIO_16_9                                         2
#define _RATIO_OTHER                                        3
//----------------------------------------------------------------------------
// VIC table NUM
//----------------------------------------------------------------------------
#define _HDMI_VIC_TABLE_NUM                                 6*32
#define _HDMI_VIC_TABLE_DUAL_NUM                            7*65
//----------------------------------------------------------------------------
// SW Packet
//----------------------------------------------------------------------------
#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)
#define _HDMI_TX_SW_PKT0_ADDRESS                            0X00
#define _HDMI_TX_SW_PKT1_ADDRESS                            0X0E
#define _HDMI_TX_SW_PKT2_ADDRESS                            0X1C
#define _HDMI_TX_SW_PKT3_ADDRESS                            0X2A
#define _HDMI_TX_SW_PKT4_ADDRESS                            0X38
#define _HDMI_TX_SW_PKT5_ADDRESS                            0X46
#define _HDMI_TX_SW_PKT6_ADDRESS                            0XFF
#define _HDMI_TX_SW_PKT7_ADDRESS                            0XFF
#define _HDMI_TX_SW_PKT8_ADDRESS                            0XFF
#define _HDMI_TX_SW_PKT9_ADDRESS                            0XFF
#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
#define _HDMI_TX_SW_PKT0_ADDRESS                            0X00
#define _HDMI_TX_SW_PKT1_ADDRESS                            0X07
#define _HDMI_TX_SW_PKT2_ADDRESS                            0X0E
#define _HDMI_TX_SW_PKT3_ADDRESS                            0X15
#define _HDMI_TX_SW_PKT4_ADDRESS                            0X1C
#define _HDMI_TX_SW_PKT5_ADDRESS                            0X23
#define _HDMI_TX_SW_PKT6_ADDRESS                            0X2A
#define _HDMI_TX_SW_PKT7_ADDRESS                            0X31
#define _HDMI_TX_SW_PKT8_ADDRESS                            0X38
#define _HDMI_TX_SW_PKT9_ADDRESS                            0X3F
#endif

#define _HDMI_TX_SW_PKT_DFF0                                0
#define _HDMI_TX_SW_PKT_DFF1                                1

#define AVI_INFO_LEN        18//size match rx HDMI_AVI_T

//--------------------------------------------------
// HDCP Info
//--------------------------------------------------
#define _HDMI_TX_HDCP_DEVICE_COUNT_MAX                      127
#define _HDMI_TX_HDCP_REAUXTH_VIDEO_OUTPUT                  _ON

#define _HDMI_TX_HDCP_ENCRYPT_DATA_TIMEOUT                  10
#define _HDMI_TX_HDCP_AUTH_GO_TIMEOUT                       500
#define _HDMI_TX_HDCP_AUTH_GO_TIMEOUT_SECOND                8000

#if(_HW_HDMI_HDCP14_KEY_TYPE == _HDCP14_KEY_320)
#define _HDMI_TX_HDCP14_KEY_SIZE                            320
#else
#define _HDMI_TX_HDCP14_KEY_SIZE                            280
#endif

//--------------------------------------------------
// Definitions of HDMI HDCP Info
//--------------------------------------------------
#define _HDMI_HDCP_SHA1_INPUT_SIZE                          64
#define _HDMI_HDCP_CAPA_CHECK_COUNT                         5
#define _HDMI_HDCP_MAX_SLAVE_ACCESS_COUNT                   18 // # decide from CTS TE
#define _HDMI_HDCP_MAX_RI_RETRY_COUNT                       3
#define _HDMI_HDCP_MAX_RI_RETRY_COUNT                       3

#if(_HDCP2_TX_SUPPORT == _ON)
//--------------------------------------------------
// HDCP Info
//--------------------------------------------------
#define _HDMI_TX_HDCP2_DEVICE_COUNT_MAX                     31
#define _HDMI_HDCP2_CAPA_CHECK_COUNT                        5

#define _HDMI_TX_HDCP2_LC_RETRY_MAX_NUMBER                  1024

#define _HDMI_TX_HDCP2_REAUXTH_VIDEO_OUTPUT                 _ON

#define GET_HDMI_TX0_HDCP2_LOAD_KEY_EN()                    (BOOLEAN)(g_ucHdmiMacTx0Hdcp2LoadKey & _BIT7)
#define SET_HDMI_TX0_HDCP2_LOAD_KEY_EN()                    (g_ucHdmiMacTx0Hdcp2LoadKey |= _BIT7)
#define CLR_HDMI_TX0_HDCP2_LOAD_KEY_EN()                    (g_ucHdmiMacTx0Hdcp2LoadKey &= ~_BIT7)

#define GET_HDMI_TX0_HDCP2_LOAD_KEY_FINISH()                (BOOLEAN)(g_ucHdmiMacTx0Hdcp2LoadKey & _BIT6)
#define SET_HDMI_TX0_HDCP2_LOAD_KEY_FINISH()                (g_ucHdmiMacTx0Hdcp2LoadKey |= _BIT6)
#define CLR_HDMI_TX0_HDCP2_LOAD_KEY_FINISH()                (g_ucHdmiMacTx0Hdcp2LoadKey &= ~_BIT6)

#define GET_HDMI_TX1_HDCP2_LOAD_KEY_EN()                    (BOOLEAN)(g_ucHdmiMacTx1Hdcp2LoadKey & _BIT7)
#define SET_HDMI_TX1_HDCP2_LOAD_KEY_EN()                    (g_ucHdmiMacTx1Hdcp2LoadKey |= _BIT7)
#define CLR_HDMI_TX1_HDCP2_LOAD_KEY_EN()                    (g_ucHdmiMacTx1Hdcp2LoadKey &= ~_BIT7)

#define GET_HDMI_TX1_HDCP2_LOAD_KEY_FINISH()                (BOOLEAN)(g_ucHdmiMacTx1Hdcp2LoadKey & _BIT6)
#define SET_HDMI_TX1_HDCP2_LOAD_KEY_FINISH()                (g_ucHdmiMacTx1Hdcp2LoadKey |= _BIT6)
#define CLR_HDMI_TX1_HDCP2_LOAD_KEY_FINISH()                (g_ucHdmiMacTx1Hdcp2LoadKey &= ~_BIT6)

#define GET_HDMI_TX2_HDCP2_LOAD_KEY_EN()                    (BOOLEAN)(g_ucHdmiMacTx2Hdcp2LoadKey & _BIT7)
#define SET_HDMI_TX2_HDCP2_LOAD_KEY_EN()                    (g_ucHdmiMacTx2Hdcp2LoadKey |= _BIT7)
#define CLR_HDMI_TX2_HDCP2_LOAD_KEY_EN()                    (g_ucHdmiMacTx2Hdcp2LoadKey &= ~_BIT7)

#define GET_HDMI_TX2_HDCP2_LOAD_KEY_FINISH()                (BOOLEAN)(g_ucHdmiMacTx2Hdcp2LoadKey & _BIT6)
#define SET_HDMI_TX2_HDCP2_LOAD_KEY_FINISH()                (g_ucHdmiMacTx2Hdcp2LoadKey |= _BIT6)
#define CLR_HDMI_TX2_HDCP2_LOAD_KEY_FINISH()                (g_ucHdmiMacTx2Hdcp2LoadKey &= ~_BIT6)
#endif // End of #if(_HDCP2_TX_SUPPORT == _ON)

#define GET_HDMI_MAC_TX_UPF_SEND_HDCP14()                   (g_bHdmiMacTx2UpfSendHdcp14)
#define SET_HDMI_MAC_TX_UPF_SEND_HDCP14()                   (g_bHdmiMacTx2UpfSendHdcp14 = _TRUE)
#define CLR_HDMI_MAC_TX_UPF_SEND_HDCP14()                   (g_bHdmiMacTx2UpfSendHdcp14 = _FALSE)


#define ScalerHdmiMacTxPxTxMapping(x) (_HW_P0_HDMI_MAC_TX_MAPPING)


//--------------------------------------------------
// HDMI Tx define for debug
//--------------------------------------------------
#define _HDMI_PLUG_EVENT_CHECK_EDID                         _OFF

#define _HDMI_TX_CTS_TEST                                   _ON

#if(_AUDIO_SUPPORT == _ON)
//--------------------------------------------------
// HDMI Tx define for Audio Type
//--------------------------------------------------
#define _LPCM_ASP                                           0x00
#define _HBR_ASP                                            0x01
#define _3D_LPCM_ASP                                        0x02

#if(_HW_HDMI_AUDIO_STRUCT == _HDMI_AUDIO_GEN_2)
#define _DEFAULT_AUDIO_PKT_MODE                             _AUD_FIX_MODE //_AUD_DYN_MODE
#define _HDMI_TX_ACR_CLAMP_SUPPORT                          _OFF// _ON
#else
#define _DEFAULT_AUDIO_PKT_MODE                             _AUD_FIX_MODE
#define _HDMI_TX_ACR_CLAMP_SUPPORT                          _OFF
#endif

#define _AUD_FIX_MODE                                       0x00
#define _AUD_DYN_MODE                                       0x01
#endif

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
//--------------------------------------------------
// Structure of HDMI Tx
//--------------------------------------------------
typedef enum
{
    _HDMI_TX_SOURCE_NONE = 0x00,
    _HDMI_TX_SOURCE_HDMI,
    _HDMI_TX_SOURCE_HDMI21,
    _HDMI_TX_SOURCE_DVI,
} EnumHDMITxInputSource;

typedef enum
{
    _HDMI_TX_NO_HPD_EVENT = 0x00,
    _HDMI_TX_HPD_PLUG_EVENT,
    _HDMI_TX_HPD_UNPLUG_EVENT,
    _HDMI_TX_HPD_TOGGLE_EVENT,
    _HDMI_TX_HPD_Z0_TOGGLE_EVENT,
    _HDMI_TX_HPD_RISING_EVENT,
    _HDMI_TX_HPD_EDID_CHANGE_EVENT,
    _HDMI_TX_Z0_TOGGLE_EVENT,
} EnumHDMITxHPDEvent;

typedef enum
{
    _HDMI_TX_Z0_ALL_LOW = 0x00,
    _HDMI_TX_Z0_CLK_LOW,
    _HDMI_TX_Z0_NOT_ALL_LOW,
} EnumHDMITxZ0Detect;

typedef struct
{
    UINT8 b1HDMIZ0ToggleTimer : 1;
    UINT8 b1HDMIHPDToggleTimer : 1;
    UINT8 b1HDMIHPDDebounceTimer : 1;
}StructHDMITxHPDTimerStatus;

typedef struct
{
    UINT8 b1HDMIHPDStatus : 1;
    UINT8 b4ModeState : 4;
    UINT8 b1ModeStateChanged : 1;
    UINT8 b2StreamSource : 2;
    UINT8 b1HDMIHPDXmsDebounceStatus : 1;
    UINT8 b1HDMITxScrambleEvent : 1;
    UINT8 b1HDMITxDisScrambleEvent : 1;
    UINT8 b1HDMITxScrambleEnableEvent : 1;
    UINT8 b1HDMITxScrambleRetry : 1;
    UINT8 b1HDMITxDownstreamStatus : 1;
    UINT8 b1HDMIHPDStatusChg : 1;
    UINT8 ucHDMITxScrambleStatus;
    UINT8 ucHDMITxSinkScrambleStatus;
    EnumHDMITxHPDEvent enumHDMITxHPDEvent;
} StructHDMITxModeInfoType;

typedef struct
{
    UINT16 usHDMITxDeepColorClk;
    UINT8 b3HDMITxColorSpace : 3;
    UINT8 b5HDMITxColorDepth : 5;
    UINT8 b5HDMITxExtColorimetry : 5;
    UINT8 b2HDITxYccQuantizationRange : 2;
    UINT8 b1HDMITxSupport : 1;
    UINT8 b2HDMITxRepeatNum : 2;
    UINT8 b4HDMITxColorimetry : 4;
    UINT8 b1HDMITxDownStreamInfoReady : 1;
    UINT8 b1HDMITxDownStremaInfoReadyChange : 1;
    UINT8 b2HDMITxRgbQuantizationRange : 2;
    UINT8 b1HDMITxInterlace : 1;
    UINT8 b5HdcpSlaveAccessCnt : 5;
    UINT8 ucFrameRate;
    UINT16 usHDMITxInputPixelClk;
    UINT8 ucHDMITxHdcpRekeyWinSize;
    UINT8 b1HDMITxHdcpReauthFromLinkOn : 1;
} StructHDMITxInputInfo;

typedef struct
{
    UINT8 b4HdmiTxAudioChNum : 4;
    UINT8 b4HdmiTxAudioFormat : 4;
    UINT8 b1HdmiTxAudioBandwidthError : 1;
    UINT8 ucHdmiTxAudioFrequency;
} StructHDMITxAudioInfo;

typedef struct
{
    UINT8 b1HdmiTx3rdTrackingCtrlFixed : 1;
    UINT8 b1HdmiTxGetinto3rdTrackingFixed : 1;
    UINT8 b1HdmiTxTrackingTimeoutFlagFixed : 1;
} StructHDMITx3rdTracking;
//--------------------------------------------------
// Enumerations of HDMI Tx Mode State
//--------------------------------------------------
typedef enum
{
    _HDMI_TX_MODE_STATUS_INITIAL = 0x00,
    _HDMI_TX_MODE_STATUS_LINK_OFF,
    _HDMI_TX_MODE_STATUS_LINK_ON,
    _HDMI_TX_MODE_STATUS_UNPLUG,
    _HDMI_TX_MODE_STATUS_POWER_SAVING,
    _HDMI_TX_MODE_STATUS_DC_OFF,
    _HDMI_TX_MODE_STATUS_LINK_TRAINING,
    _HDMI_TX_MODE_STATUS_BANDWIDTH_DECISION,
    _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON,
} EnumHDMITxModeStatus;

//--------------------------------------------------
// Enumerations of Scramble Status
//--------------------------------------------------
typedef enum
{
    _HDMI_TX_SCDC_PROC_STATE_INITIAL = 0x00,
    _HDMI_TX_SCDC_PROC_STATE_SCRAMBLE_ENABLE,
    _HDMI_TX_SCDC_PROC_STATE_SCRAMBLE_CHECK,
    _HDMI_TX_SCDC_PROC_STATE_SCRAMBLE_DISABLE,
    _HDMI_TX_SCDC_PROC_STATE_STANDBY,
} EnumHDMITxScrambleStatus;

//--------------------------------------------------
// Enumerations of Sink Scramble Status
//--------------------------------------------------
typedef enum
{
    _HDMI_TX_SINK_NO_READ = 0x00,
    _HDMI_TX_SINK_SCRAMBLE_ON = 0x01,
    _HDMI_TX_SINK_SCRAMBLE_OFF = 0x02,
} EnumHDMITxSinkcrambleStatus;

//--------------------------------------------------
// Enumerations of Hdmi Packet Type
//--------------------------------------------------
typedef enum
{
    _HDMI_TX_AVI_INFOFRAME = 0x00,
    _HDMI_TX_AUDIO_INFOFRAME,
    _HDMI_TX_VSIF,
    _HDMI_TX_STATIC_HDR_INFOFRAME,
    _HDMI_TX_GCP,
    _HDMI_TX_GCP_MUTE_CTRL,
    _HDMI_TX_FREESYNC_INFOFRAME,
    _HDMI_TX_VTEM,
    _HDMI_TX_AUDIO_METADATA,
    _HDMI_TX_SPD_INFOFRAME,
    _HDMI_TX_SBTM,
    _HDMI_TX_DV_VSIF,
    _HDMI_TX_SW_BYPASS_RX_AVI_INFOFRAME,
    _HDMI_TX_NULL = 0xFF,
} EnumHDMITxPacketType;

//--------------------------------------------------
//HDCP Auth State
//--------------------------------------------------
typedef enum
{
    _HDMI_TX_HDCP_STATE_IDLE = 0,
    _HDMI_TX_HDCP_STATE_AUTH_1,
    _HDMI_TX_HDCP_STATE_AUTH_1_COMPARE_R0,
    _HDMI_TX_HDCP_STATE_AUTH_2,
    _HDMI_TX_HDCP_STATE_AUTH_DONE,
} EnumHdmiTxHDCPAuthState;

typedef struct
{
    UINT8 ucAuthDownstreamEvent;
    UINT8 ucBInfoDevice;
    UINT8 ucBInfoDepth;

    UINT8 b4AuthState : 4;
    UINT8 b3CapaCheck : 3;
    UINT8 b1AuthStateChange : 1;

    UINT8 b3RiRetryCount : 3;
    UINT8 b1IsHDCPSupported : 1;
    UINT8 b1IsHDCP1p1Supported : 1;
    UINT8 b1IsDownstreamRepeater : 1;
    UINT8 b1R0Timeout : 1;
    UINT8 b1VReadyTimeout : 1;

    UINT8 b1PollingVReady : 1;
    UINT8 b1VReadyBit : 1;
    UINT8 b1AuthStart : 1;
    UINT8 b1AuthHold : 1;
    UINT8 b1EncryptDataTimeout : 1;
    UINT8 b1AuthGo : 1;
    UINT8 b1HpdLowEvent : 1;
    UINT8 b1HpdLowEventReAuth : 1;

    UINT8 b1FirstAuth : 1;
    UINT8 b1ReportRID : 1;
    UINT8 b1TimeoutProtect : 1;
} StructHdmiTxHDCPAuthInfo;


typedef struct
{
    UINT8 ucSourceColorSpace;
    StructTimingInfo stInputInfo;
    UINT8 pucAspectRatio;
    UINT8 pucVIC;
    UINT8 pucVIC2;
}StructHdmiTxSearchVicTableInfo;


//--------------------------------------------------
//HDCP TX Authentication Downstream Event
//--------------------------------------------------
typedef enum
{
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_NONE = 0,
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_LINK_INTEGRITY_FAIL,
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_REAUTH,
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_MSG_QUERY_REAUTH,
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_PLUG_CHANGED,
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_PASS,
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_DEVICE_DEPTH_MAX,
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_HDCP_CAP_CHANGE,
} EnumHdmiTxHDCPDownstreamAuthStatus;

//--------------------------------------------------
// Definitions of HDCP Rx Status Type
//--------------------------------------------------
typedef enum
{
    _HDCP_1_4_TX_NONE = 0,
    _HDCP_1_4_TX_V_READY = _BIT5,
} EnumHdmiHdcpStatus;

#if(_HDCP2_TX_SUPPORT == _ON)
//--------------------------------------------------
// Definitions of HDCP2 Tx Status Type
//--------------------------------------------------
typedef enum
{
    _HDCP_2_2_TX_NONE = 0,
    _HDCP_2_2_TX_V_READY = _BIT2,
    // _HDCP_2_2_TX_H_PRIME_AVAILABLE = _BIT1,
    // _HDCP_2_2_TX_PAIRING_AVAILABLE = _BIT2,
    _HDCP_2_2_TX_REAUTH_REQUEST = _BIT3,
    // _HDCP_2_2_TX_LINK_INTEGRITY_FAIL = _BIT4,
} EnumHdmiHdcp2Status;

//--------------------------------------------------
//HDCP RX Authentication Downstream Event
//--------------------------------------------------
typedef enum
{
    _HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_NONE = 0x00,
    _HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_LINK_INTEGRITY_FAIL,
    _HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_REAUTH,
    _HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_MSG_QUERY_REAUTH,
    _HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_PLUG_CHANGED,
    _HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_PASS,
    _HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_DEVICE_DEPTH_EXCEED,
    _HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_HDCP_CAP_CHANGE,
} EnumHdmiTxHdcp2DownstreamAuthEvent;
#endif // End of #if(_HDCP2_TX_SUPPORT == _ON)


//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************
extern UINT8 g_pucHdmiMacTxHdcpDpk[_HDMI_TX_HDCP14_KEY_SIZE];
extern UINT16 g_usHdmiMacTxHdcpKsvFifoIndex;
extern UINT8 g_pucHdmiMacTxHdcpKsvFifo[_HDMI_TX_HDCP_DEVICE_COUNT_MAX * 5];

#if(_HDCP2_TX_SUPPORT == _ON)
extern UINT8 g_ucHdmiMacTx0Hdcp2LoadKey;
extern UINT8 g_ucHdmiMacTx1Hdcp2LoadKey;
extern UINT8 g_ucHdmiMacTx2Hdcp2LoadKey;
#endif
extern BOOLEAN g_bHdmiMacTx2UpfSendHdcp14;

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
//extern UINT8 ScalerHdmiMacTxPxTxMapping(EnumOutputPort enumOutputPort);
extern EnumOutputPort ScalerHdmiMacTxPxMapping(UINT8 ucTxIndex);
extern UINT8 ScalerHdmiMacTxEdidGetFeature(EnumOutputPort enumOutputPort, EnumEdidFeatureType enumEdidFeature);
extern void ScalerHdmiMacTxGetEdidVicTable(EnumOutputPort enumOutputPort, EnumEdidCtaDataBlock enumVDBType, UINT8 *pucVICTable, UINT16 *pusCtaDataBlockAddr);
extern void ScalerHdmiMacTxSetSinkTmdsSupportClk(EnumOutputPort enumOutputPort, UINT8 ucHdmiVsdbMaxTMDSClk, UINT8 ucHfVsdbMaxTMDSClk);
extern BOOLEAN ScalerHdmiMacTxGetDownStreamStatus(EnumOutputPort enumOutputPort);

#if(_HDMI_PURE_SUPPORT == _ON)
extern void ScalerHdmiMacTxSetClkDataTransmit(EnumOutputPort enumOutputPort, BOOLEAN bEnable);
extern void ScalerHdmiMacTxSetClkDiv4(EnumOutputPort enumOutputPort, BOOLEAN bEnable);
#endif

extern void ScalerHdmiMacTxSetDPTxClkDiv(EnumOutputPort enumOutputPort, UINT8 ucDivNum);

extern void ScalerHdmiMacTxSetSmallFIFODoubleRate(EnumOutputPort enumOutputPort);
extern void ScalerHdmiMacTxSetSmallFIFOFullRate(EnumOutputPort enumOutputPort);
extern void ScalerHdmiMacTxSetSmallFIFOHalfRate(EnumOutputPort enumOutputPort);

#if(_AUDIO_SUPPORT == _ON)
extern UINT8 ScalerHdmiMacTxGetAudioFormat(EnumOutputPort enumOutputPort, EnumAudioInfoType enumAudioInfoType);
extern void ScalerHdmiMacTxDisableAudioFifo(EnumOutputPort enumOutputPort);
#endif
#if((_AUDIO_SUPPORT == _ON) || (_HDMI21_TX_SUPPORT == _ON))
extern EnumHDMITxInputSource ScalerHdmiMacTxGetStreamSource(EnumOutputPort enumOutputPort);
#endif

extern UINT8 ScalerHdmiMacTxGetAudioFormat(EnumOutputPort enumOutputPort, EnumAudioInfoType enumAudioInfoType);
#if(_DP_MST_SUPPORT == _ON)
extern BOOLEAN ScalerHdmiMacTxGetHDCPDownStreamSupported(EnumOutputPort enumOutputPort);
extern BOOLEAN ScalerHdmiMacTxGetHDCPDownStreamRepeater(EnumOutputPort enumOutputPort);


#endif // End of #if(_DP_MST_SUPPORT == _ON)

extern UINT16 ScalerHdmiMacTxGetInputPixelClk(EnumOutputPort enumOutputPort);
extern UINT8 ScalerHdmiMacTxGetInputColorDepth(EnumOutputPort enumOutputPort);
extern UINT8 ScalerHdmiMacTxGetInputRepeatNum(EnumOutputPort enumOutputPort);
extern UINT16 ScalerHdmiMacTxGetDeepColorClk(EnumOutputPort enumOutputPort);
extern void ScalerHdmiMacTxGetHdcpBksv(EnumOutputPort enumOutputPort, UINT8 *pucArray);
extern void ScalerHdmiMacTxSearchVicTable(StructHdmiTxSearchVicTableInfo *pStHdmiTxSearchVicTableInfo);
extern void ScalerHdmiMacTxEdidModifyEnable(EnumOutputPort enumOutputPort, BOOLEAN bEnable, EnumHdmiEdidModifyEvent enumEdidModifyEvent);
extern void ScalerHdmiMacTxEdidDidExtDbParse(EnumOutputPort enumOutputPort, UINT16 *pusDidDataBlockAddr, UINT8 *pucDdcRamAddr);

// banked IF
extern BYTE ScalerHdmiMacTxAspectRatioCal(StructTimingInfo *pstInputInfo);
extern void ScalerHdmiMacTxSearchVicTable(StructHdmiTxSearchVicTableInfo *pStHdmiTxSearchVicTableInfo);

#if((_HDMI_HDCP14_TX_SUPPORT == _ON) || (_HDMI_HDCP2_TX_SUPPORT == _ON))
extern BOOLEAN ScalerHdmiMacTxHdcpLeagalCheck(EnumOutputPort enumOutputPort);
#endif
#endif // End of #if(_HDMI_TX_SUPPORT == _ON)
#endif // #ifndef __SCALER_HDMI_MAC_TX_INCLUDE_H__
