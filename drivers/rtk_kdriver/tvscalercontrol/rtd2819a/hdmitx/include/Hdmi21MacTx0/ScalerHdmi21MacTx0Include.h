#ifndef __SCALER_HDMI21_MAC_TX0_INCLUDE_H__
#define __SCALER_HDMI21_MAC_TX0_INCLUDE_H__

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
// ID Code      : ScalerHdmi21MacTx0Include.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------



#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#if(_AUDIO_SUPPORT == _ON)
//--------------------------------------------------
// Macros of HDMI Audio
//--------------------------------------------------
#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
#define GET_HDMI21_MAC_TX0_CTS_CODE_CHECK_EVENT()               (g_bHdmi21MacTx0CtsCodeCheckEvent)
#define SET_HDMI21_MAC_TX0_CTS_CODE_CHECK_EVENT()               (g_bHdmi21MacTx0CtsCodeCheckEvent = _TRUE)
#define CLR_HDMI21_MAC_TX0_CTS_CODE_CHECK_EVENT()               (g_bHdmi21MacTx0CtsCodeCheckEvent = _FALSE)

#define GET_HDMI21_MAC_TX0_CTS_CODE_FW_MODE()                   (g_bHdmi21MacTx0CtsCodeFwMode)
#define SET_HDMI21_MAC_TX0_CTS_CODE_FW_MODE()                   (g_bHdmi21MacTx0CtsCodeFwMode = _TRUE)
#define CLR_HDMI21_MAC_TX0_CTS_CODE_FW_MODE()                   (g_bHdmi21MacTx0CtsCodeFwMode = _FALSE)
#endif

#define SET_HDMI21_MAC_TX0_AUDIO_FIFO_ENABLE()                  (rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_23_reg, ~_BIT0, _BIT0))
#define CLR_HDMI21_MAC_TX0_AUDIO_FIFO_ENABLE()                  (rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_23_reg, ~_BIT0, 0x00))
#endif

//--------------------------------------------------
// Macros of HDMI SW Packet
//--------------------------------------------------
#define GET_HDMI21_MAC_TX0_SW_PKT_DFF0_USED()                   (g_bHdmi21MacTx0SWPktDFF0Used)
#define SET_HDMI21_MAC_TX0_SW_PKT_DFF0_USED()                   (g_bHdmi21MacTx0SWPktDFF0Used = _TRUE)
#define CLR_HDMI21_MAC_TX0_SW_PKT_DFF0_USED()                   (g_bHdmi21MacTx0SWPktDFF0Used = _FALSE)

#define GET_HDMI21_MAC_TX0_SW_PKT_DFF1_USED()                   (g_bHdmi21MacTx0SWPktDFF1Used)
#define SET_HDMI21_MAC_TX0_SW_PKT_DFF1_USED()                   (g_bHdmi21MacTx0SWPktDFF1Used = _TRUE)
#define CLR_HDMI21_MAC_TX0_SW_PKT_DFF1_USED()                   (g_bHdmi21MacTx0SWPktDFF1Used = _FALSE)


//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************


//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************


//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
extern void ScalerHdmi21MacTx0LinktrainingReset(void);
extern void ScalerHdmi21MacTx0Linktraining(void);
extern BOOLEAN ScalerHdmi21MacTx0VideoMuxStatus(void);
extern BOOLEAN ScalerHdmi21MacTx0CheckDPCSRAM(void);
extern BOOLEAN ScalerHdmi21MacTx0LinktrainingPassCheck(void);
extern EnumHdmi21DataFlowMeteringResult ScalerHdmi21MacTx0DataFlowMetering(void);
extern void ScalerHdmi21MacTx0DataFlowMeterParameterSet(void);
extern EnumLinkBWDecisionResultType ScalerHdmi21MacTx0BwDecision(void);
extern BOOLEAN ScalerHdmi21MacTx0StreamHandler(void);
extern void ScalerHdmi21MacTx0ConvertTo420(void);
extern void ScalerHdmi21MacTx0PktFineTune(void);
extern void ScalerHdmi21MacTx0Reset(void);
extern void ScalerHdmi21MacTx0Initial(void);
#endif // End of #if(_HDMI_TX_SUPPORT == _ON)

#endif // #ifndef __SCALER_HDMI21_MAC_TX0_INCLUDE_H__
