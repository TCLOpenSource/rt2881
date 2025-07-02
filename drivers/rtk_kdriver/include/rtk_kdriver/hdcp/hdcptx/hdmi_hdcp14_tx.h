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

#ifndef __SCALER_HDCP_14_TX_H__
#define __SCALER_HDCP_14_TX_H__

#include <hdcp/hdcp_common.h>
//----------------------------------------------------------------------------------------------------
// ID Code      : ScalerHdcp14Tx.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#define _HDCP14_SHA1_INPUT_SIZE                             64
#define _HDCP_TX_DEVICE_COUNT_MAX                           32

#define _HDMI_TX_HDCP_VERIFY_RI_TIMEOUT      2000   //2s
#define _HDMI_TX_HDCP_ENCRYPT_DATA_TIMEOUT   10   //10ms
#define _HDMI_TX_HDCP_DOWNSTREAM_R0_TIMEOUT   110   //110ms

#define _SCALER_TIMER_EVENT_HDMI_TX0_HDCP_VERIFY_RI           0xA0
#define _SCALER_TIMER_EVENT_HDMI_TX2_HDCP_WAITING_FOR_V_READY 0xA1
#define _SCALER_TIMER_EVENT_HDMI_TX2_HDCP_POLLING_V_READY     0xA2
#define _SCALER_TIMER_EVENT_HDMI_TX_HDCP_STATE_AUTH_DONE      0xA3
#define _SCALER_TIMER_EVENT_HDMI_TX_HDCP_STATE_AUTH_1_COMPARE_R0 0xA4


#define _SUCCESS            0
#define _FAIL                   1

#define _TRUE                  1
#define _FALSE                  0

#define _HDCP_ADDRESS           (0x74 >> 1)
#define _HDMI_HDCP_MAX_RI_RETRY_COUNT  3

#define _HDMI_HDCP_SHA1_INPUT_SIZE   64
#define SHA1_OUTPUT_SIZE                20
#define _HDCP14_KEY_320                 0
#define _HDCP14_KEY_280                 1
#define _HW_HDMI_HDCP14_KEY_TYPE        0

#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_EVENT(port)                 (g_stHdmiHdcp14Tx2AuthInfo[port].ucAuthDownstreamEvent)
#define SET_HDMI_HDCP14_TX2_DOWNSTREAM_EVENT(port, x)                (g_stHdmiHdcp14Tx2AuthInfo[port].ucAuthDownstreamEvent = (x))
#define CLR_HDMI_HDCP14_TX2_DOWNSTREAM_EVENT(port)                 (g_stHdmiHdcp14Tx2AuthInfo[port].ucAuthDownstreamEvent = _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_NONE)

#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_SUPPORTED(port)             (g_stHdmiHdcp14Tx2AuthInfo[port].b1IsHDCPSupported)
#define SET_HDMI_HDCP14_TX2_DOWNSTREAM_SUPPORTED(port)             (g_stHdmiHdcp14Tx2AuthInfo[port].b1IsHDCPSupported = _TRUE)
#define CLR_HDMI_HDCP14_TX2_DOWNSTREAM_SUPPORTED(port)             (g_stHdmiHdcp14Tx2AuthInfo[port].b1IsHDCPSupported = _FALSE)

#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_1_1_SUPPORTED(port)         (g_stHdmiHdcp14Tx2AuthInfo[port].b1IsHDCP1p1Supported)
#define SET_HDMI_HDCP14_TX2_DOWNSTREAM_1_1_SUPPORTED(port)         (g_stHdmiHdcp14Tx2AuthInfo[port].b1IsHDCP1p1Supported = _TRUE)
#define CLR_HDMI_HDCP14_TX2_DOWNSTREAM_1_1_SUPPORTED(port)         (g_stHdmiHdcp14Tx2AuthInfo[port].b1IsHDCP1p1Supported = _FALSE)

#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_REPEATER(port)              (g_stHdmiHdcp14Tx2AuthInfo[port].b1IsDownstreamRepeater)
#define SET_HDMI_HDCP14_TX2_DOWNSTREAM_REPEATER(port)              (g_stHdmiHdcp14Tx2AuthInfo[port].b1IsDownstreamRepeater = _TRUE)
#define CLR_HDMI_HDCP14_TX2_DOWNSTREAM_REPEATER(port)              (g_stHdmiHdcp14Tx2AuthInfo[port].b1IsDownstreamRepeater = _FALSE)

#define GET_HDMI_HDCP14_TX2_AUTH_STATE(port)                       (g_stHdmiHdcp14Tx2AuthInfo[port].b4AuthState)
#define SET_HDMI_HDCP14_TX2_AUTH_STATE(port, x)                      (g_stHdmiHdcp14Tx2AuthInfo[port].b4AuthState = (x))

#define GET_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port)                (g_stHdmiHdcp14Tx2AuthInfo[port].b1AuthStateChange)
#define SET_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port)                (g_stHdmiHdcp14Tx2AuthInfo[port].b1AuthStateChange = _TRUE)
#define CLR_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port)                (g_stHdmiHdcp14Tx2AuthInfo[port].b1AuthStateChange = _FALSE)

#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_DEVICE_COUNT(port)          (g_stHdmiHdcp14Tx2AuthInfo[port].ucBInfoDevice & 0x7F)
#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_DEVICE_COUNT_MAX(port)      (unsigned char)(g_stHdmiHdcp14Tx2AuthInfo[port].ucBInfoDevice & _BIT7)

#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_DEPTH(port)                 (g_stHdmiHdcp14Tx2AuthInfo[port].ucBInfoDepth & 0x07)
#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_DEPTH_MAX(port)             (unsigned char)(g_stHdmiHdcp14Tx2AuthInfo[port].ucBInfoDepth & _BIT3)

#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port)          (g_stHdmiHdcp14Tx2AuthInfo[port].ucBInfoDevice)
#define SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port, x)         (g_stHdmiHdcp14Tx2AuthInfo[port].ucBInfoDevice = x)
#define CLR_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port)          (g_stHdmiHdcp14Tx2AuthInfo[port].ucBInfoDevice = 0x00)

#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port)           (g_stHdmiHdcp14Tx2AuthInfo[port].ucBInfoDepth)
#define SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port, x)          (g_stHdmiHdcp14Tx2AuthInfo[port].ucBInfoDepth = x)
#define CLR_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port)           (g_stHdmiHdcp14Tx2AuthInfo[port].ucBInfoDepth = 0x00)

#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_R0_TIMEOUT(port)            (g_stHdmiHdcp14Tx2AuthInfo[port].b1R0Timeout)
#define SET_HDMI_HDCP14_TX2_DOWNSTREAM_R0_TIMEOUT(port)            (g_stHdmiHdcp14Tx2AuthInfo[port].b1R0Timeout = _TRUE)
#define CLR_HDMI_HDCP14_TX2_DOWNSTREAM_R0_TIMEOUT(port)            (g_stHdmiHdcp14Tx2AuthInfo[port].b1R0Timeout = _FALSE)

#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_TIMEOUT(port)       (g_stHdmiHdcp14Tx2AuthInfo[port].b1VReadyTimeout)
#define SET_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_TIMEOUT(port)       (g_stHdmiHdcp14Tx2AuthInfo[port].b1VReadyTimeout = _TRUE)
#define CLR_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_TIMEOUT(port)       (g_stHdmiHdcp14Tx2AuthInfo[port].b1VReadyTimeout = _FALSE)

#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_POLLING_V_READY(port)       (g_stHdmiHdcp14Tx2AuthInfo[port].b1PollingVReady)
#define SET_HDMI_HDCP14_TX2_DOWNSTREAM_POLLING_V_READY(port)       (g_stHdmiHdcp14Tx2AuthInfo[port].b1PollingVReady = _TRUE)
#define CLR_HDMI_HDCP14_TX2_DOWNSTREAM_POLLING_V_READY(port)       (g_stHdmiHdcp14Tx2AuthInfo[port].b1PollingVReady = _FALSE)

#define GET_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_BIT(port)           (g_stHdmiHdcp14Tx2AuthInfo[port].b1VReadyBit)
#define SET_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_BIT(port)           (g_stHdmiHdcp14Tx2AuthInfo[port].b1VReadyBit = _TRUE)
#define CLR_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_BIT(port)           (g_stHdmiHdcp14Tx2AuthInfo[port].b1VReadyBit = _FALSE)

#define GET_HDMI_HDCP14_TX2_AUTH_START(port)                       (g_stHdmiHdcp14Tx2AuthInfo[port].b1AuthStart)
#define SET_HDMI_HDCP14_TX2_AUTH_START(port)                       (g_stHdmiHdcp14Tx2AuthInfo[port].b1AuthStart = _TRUE)
#define CLR_HDMI_HDCP14_TX2_AUTH_START(port)                       (g_stHdmiHdcp14Tx2AuthInfo[port].b1AuthStart = _FALSE)

#define GET_HDMI_HDCP14_TX2_RI_RETRY_COUNT(port)                   (g_stHdmiHdcp14Tx2AuthInfo[port].b3RiRetryCount)
#define ADD_HDMI_HDCP14_TX2_RI_RETRY_COUNT(port)                   (g_stHdmiHdcp14Tx2AuthInfo[port].b3RiRetryCount++)
#define SET_HDMI_HDCP14_TX2_RI_RETRY_COUNT(port, x)                  (g_stHdmiHdcp14Tx2AuthInfo[port].b3RiRetryCount = (x))
#define CLR_HDMI_HDCP14_TX2_RI_RETRY_COUNT(port)                   (g_stHdmiHdcp14Tx2AuthInfo[port].b3RiRetryCount = _FALSE)

#define GET_HDMI_HDCP14_TX2_ENCRYPT_DATA_TIMEOUT(port)             (g_stHdmiHdcp14Tx2AuthInfo[port].b1EncryptDataTimeout)
#define SET_HDMI_HDCP14_TX2_ENCRYPT_DATA_TIMEOUT(port)             (g_stHdmiHdcp14Tx2AuthInfo[port].b1EncryptDataTimeout = _TRUE)
#define CLR_HDMI_HDCP14_TX2_ENCRYPT_DATA_TIMEOUT(port)             (g_stHdmiHdcp14Tx2AuthInfo[port].b1EncryptDataTimeout = _FALSE)

#define GET_HDMI_HDCP14_TX2_ENCRYPT_AUTH_GO(port)                  (g_stHdmiHdcp14Tx2AuthInfo[port].b1AuthGo)
#define SET_HDMI_HDCP14_TX2_ENCRYPT_AUTH_GO(port)                  (g_stHdmiHdcp14Tx2AuthInfo[port].b1AuthGo = _TRUE)
#define CLR_HDMI_HDCP14_TX2_ENCRYPT_AUTH_GO(port)                  (g_stHdmiHdcp14Tx2AuthInfo[port].b1AuthGo = _FALSE)

#define GET_HDMI_HDCP14_TX2_TIME_OUT_PROTECT(port)                 (g_stHdmiHdcp14Tx2AuthInfo[port].b1TimeoutProtect)
#define SET_HDMI_HDCP14_TX2_TIME_OUT_PROTECT(port)                 (g_stHdmiHdcp14Tx2AuthInfo[port].b1TimeoutProtect = _TRUE)
#define CLR_HDMI_HDCP14_TX2_TIME_OUT_PROTECT(port)                 (g_stHdmiHdcp14Tx2AuthInfo[port].b1TimeoutProtect = _FALSE)

#define GET_HDMI_hdcp14_auth_cnt_STAT(port)                (hdcp14_auth_cnt[port])
#define SET_HDMI_hdcp14_auth_cnt_STAT(port, value)         (hdcp14_auth_cnt[port] = value)
#define ADD_HDMI_hdcp14_auth_cnt_STAT(port)                (hdcp14_auth_cnt[port]++)
#define CLR_HDMI_hdcp14_auth_cnt_STAT(port)                (hdcp14_auth_cnt[port] = 0)
//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
//--------------------------------------------------
// Enumerations of HDCP Type
//--------------------------------------------------
typedef enum {
    _HDCP_NONE = 0x00,
    _HDCP_14 = 0x14,
    _HDCP_22 = 0x22,
    _HDCP_14_REPEATER = 0x94,
    _HDCP_22_REPEATER = 0xA2,
} EnumHDCPType;



typedef enum {
    _OUTPUT_PORT_HDMI_TX0,
    _OUTPUT_PORT_HDMI_TX1,
    _OUTPUT_PORT_HDMI_TX2,
} EnumInterfaceTxPortNo;
#ifdef HDCP_REPEATER
typedef enum {

} EnumInputPort;
#endif
typedef struct {
    unsigned char ucAuthDownstreamEvent;
    unsigned char b1IsHDCPSupported;
    unsigned char b1IsHDCP1p1Supported;
    unsigned char b1IsDownstreamRepeater;
    unsigned char b4AuthState;
    unsigned char b1AuthStateChange;
    unsigned short ucBInfoDevice;
    unsigned char ucBInfoDepth;
    unsigned char b1R0Timeout;
    unsigned char b1VReadyTimeout;
    unsigned char b1PollingVReady;
    unsigned char b1VReadyBit;
    unsigned char b1AuthStart;
    unsigned char b3RiRetryCount;
    unsigned char b1EncryptDataTimeout;
    unsigned char b1AuthGo;
    unsigned char b1TimeoutProtect;
    unsigned char b1HdcpReportRidList;
} StructHdmiTxHDCPAuthInfo;


typedef enum {
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_NONE = 0,
    _DP_RX_HDCP_UPSTREAM_AUTH_EVENT_R0_PASS,
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_PASS,
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_HDCP_CAP_CHANGE,
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_DEVICE_DEPTH_MAX,
    _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_REAUTH,
} HDMI_TX_HDCP_AUTH_ENENT;


typedef enum {
    _HDCP_1_4_TX_NONE = 0,
    _HDCP_1_4_TX_V_READY,
} EnumHdmiHdcpStatus;


typedef enum {
    _HDMI_TX_HDCP_STATE_IDLE,
    _HDMI_TX_HDCP_STATE_AUTH_1,
    _HDMI_TX_HDCP_STATE_AUTH_1_COMPARE_R0,
    _HDMI_TX_HDCP_STATE_AUTH_2,
    _HDMI_TX_HDCP_STATE_AUTH_DONE,
} EnumHdmiTxHDCPAuthState;


extern unsigned char tx14_key_align_offset;
extern StructHdmiTxHDCPAuthInfo g_stHdmiHdcp14Tx2AuthInfo[HDMI_TX_PORT_MAX_NUM];
//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************
#define AKSV_SIZE          (5)
#define AKSV_START     (tx14_key_align_offset)
#define HDCPTX14_KEY_SIZE          (320)
#define HDCPTX14_KEY_START     (tx14_key_align_offset + 5)

//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************
extern unsigned char g_pucHdcp14TxAksvBackup[5];
extern unsigned char g_pucHdmiHdcp14Tx2HdcpBksv[5];
extern unsigned short g_usHdmiHdcp14Tx2KsvFifoIndex;
extern unsigned short hdcp14_auth_cnt[HDMI_TX_PORT_MAX_NUM];
//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
extern unsigned char ScalerHdcpTxGetUpstreamEncryptStatus(EnumInterfaceTxPortNo enumInterfaceTxPortNo);
extern void ScalerHdmiHdcp14Tx2Handler(unsigned char port);
extern unsigned char ScalerHdmiHdcp14Tx2CheckDownstreamCapability(unsigned char port);
extern void ScalerHdmiHdcp14Tx2ChangeAuthState(unsigned char port, EnumHdmiTxHDCPAuthState enumHDCPAuthState);
extern void ScalerHdmiHdcp14Tx2TimeoutToReadR0Event(unsigned char port);
extern void ScalerHdmiHdcp14Tx2TimeoutForVReadyEvent(unsigned char port);
extern void ScalerHdmiHdcp14Tx2PollingVReadyEvent(unsigned char port);
extern void ScalerHdmiHdcp14Tx2VerifyRiEvent(unsigned char port);
extern unsigned char ScalerHdmiHdcp14Tx2Auth1CheckBksv(unsigned char port);
extern unsigned char ScalerHdmiHdcp14Tx2VerifyPj(unsigned char port);
extern unsigned char ScalerHdmiHdcp14Tx2Auth1Proc(unsigned char port);
extern void ScalerHdmiHdcp14Tx2Auth1WriteAnAkey(unsigned char port);
extern unsigned char ScalerHdmiHdcp14Tx2Auth1GenKm(unsigned char port);
extern unsigned char ScalerHdmiHdcp14Tx2Auth1CompareR0(unsigned char port);
extern unsigned char ScalerHdmiHdcp14Tx2Auth2Proc(unsigned char port);
extern void ScalerHdmiHdcp14Tx2Auth2WriteShaInput(unsigned char port);
extern void ScalerHdmiHdcp14Tx2Auth2ShaRun(unsigned char bIsFirst);
extern unsigned char ScalerHdmiHdcp14Tx2CheckBinfo(unsigned char port);
extern unsigned char ScalerHdmiHdcp14Tx2Auth2CompareV(unsigned char port);
extern EnumHdmiHdcpStatus ScalerHdmiHdcp14Tx2PollingStatus(void);
extern void ScalerHdmiHdcp14Tx2ResetInfo(unsigned char port);
extern unsigned char ScalerHdmiHdcp14Tx2ReCheckKsv(unsigned char port);
extern void ScalerHdcpTxStoredReceiverId(unsigned char *pucInputArray, unsigned char ucDeviceCount) ;
extern void hdcp14tx_init(void);
extern unsigned char is_flash_burned_hdcp14_tx_key(void);
extern void load_hdcp14_tx_key(void);
#endif
