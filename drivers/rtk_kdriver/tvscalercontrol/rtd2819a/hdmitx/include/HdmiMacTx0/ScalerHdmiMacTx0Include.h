#ifndef __SCALER_HDMI_MAC_TX0_INCLUDE_H__
#define __SCALER_HDMI_MAC_TX0_INCLUDE_H__

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
// ID Code      : ScalerHdmiMacTx0Include.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include "../ScalerFunctionInclude.h"
#include "../HdmiMacTx/ScalerHdmiMacTxInclude.h"
#include "../EdidCommon.h"

#if(_HDMI_MAC_TX0_SUPPORT == _ON)

#define _HDMI_MAC_TX0_CTS_TIMER_EVENT_FRAME_CNT                 0x20
extern BOOLEAN g_bHdmiMacTx0FindCtsTestEdid;
//--------------------------------------------------
// Macros of HDMI Tx Mode Infomation
//--------------------------------------------------
#define GET_HDMI_MAC_TX0_HPD_EVENT()                            (g_stHdmiMacTx0ModeInfo.enumHDMITxHPDEvent)
#define SET_HDMI_MAC_TX0_HPD_EVENT(x)                           (g_stHdmiMacTx0ModeInfo.enumHDMITxHPDEvent = (x))

#define GET_HDMI_MAC_TX0_HPD_STATUS()                           (g_stHdmiMacTx0ModeInfo.b1HDMIHPDStatus)
#define SET_HDMI_MAC_TX0_HPD_STATUS(x)                          (g_stHdmiMacTx0ModeInfo.b1HDMIHPDStatus = (x))

#define GET_HDMI_MAC_TX0_HPD_STATUS_CHANGE()                    (g_stHdmiMacTx0ModeInfo.b1HDMIHPDStatusChg)
#define SET_HDMI_MAC_TX0_HPD_STATUS_CHANGE()                    (g_stHdmiMacTx0ModeInfo.b1HDMIHPDStatusChg = _TRUE)
#define CLR_HDMI_MAC_TX0_HPD_STATUS_CHANGE()                    (g_stHdmiMacTx0ModeInfo.b1HDMIHPDStatusChg = _FALSE)

#define GET_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS()              (g_stHdmiMacTx0ModeInfo.b1HDMIHPDXmsDebounceStatus)
#define SET_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS()              (g_stHdmiMacTx0ModeInfo.b1HDMIHPDXmsDebounceStatus = _TRUE)
#define CLR_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS()              (g_stHdmiMacTx0ModeInfo.b1HDMIHPDXmsDebounceStatus = _FALSE)

#if(_HDMI_MAC_TX0_PURE == _ON)
#define GET_HDMI_MAC_TX0_HPD_Z0_TOGGLE_TIMEOUT()                (g_bHdmiMacTx0HPDZ0ToggleTimeout)
#define SET_HDMI_MAC_TX0_HPD_Z0_TOGGLE_TIMEOUT()                (g_bHdmiMacTx0HPDZ0ToggleTimeout = _TRUE)
#define CLR_HDMI_MAC_TX0_HPD_Z0_TOGGLE_TIMEOUT()                (g_bHdmiMacTx0HPDZ0ToggleTimeout = _FALSE)

#define GET_HDMI_MAC_TX0_Z0_TOGGLE_TIMEOUT()                    (g_bHdmiMacTx0Z0ToggleTimeout)
#define SET_HDMI_MAC_TX0_Z0_TOGGLE_TIMEOUT()                    (g_bHdmiMacTx0Z0ToggleTimeout = _TRUE)
#define CLR_HDMI_MAC_TX0_Z0_TOGGLE_TIMEOUT()                    (g_bHdmiMacTx0Z0ToggleTimeout = _FALSE)

#define GET_HDMI_MAC_TX0_Z0_TOGGLE_TIMER_ACTIVE()               (g_stHdmiMacTx0HPDTimerStatus.b1HDMIZ0ToggleTimer)
#define SET_HDMI_MAC_TX0_Z0_TOGGLE_TIMER_ACTIVE()               (g_stHdmiMacTx0HPDTimerStatus.b1HDMIZ0ToggleTimer = _TRUE)
#define CLR_HDMI_MAC_TX0_Z0_TOGGLE_TIMER_ACTIVE()               (g_stHdmiMacTx0HPDTimerStatus.b1HDMIZ0ToggleTimer = _FALSE)

#define GET_HDMI_MAC_TX0_HPD_TOGGLE_TIMER_ACTIVE()              (g_stHdmiMacTx0HPDTimerStatus.b1HDMIHPDToggleTimer)
#define SET_HDMI_MAC_TX0_HPD_TOGGLE_TIMER_ACTIVE()              (g_stHdmiMacTx0HPDTimerStatus.b1HDMIHPDToggleTimer = _TRUE)
#define CLR_HDMI_MAC_TX0_HPD_TOGGLE_TIMER_ACTIVE()              (g_stHdmiMacTx0HPDTimerStatus.b1HDMIHPDToggleTimer = _FALSE)
#endif

#define GET_HDMI_MAC_TX0_HPD_DEBOUNCE_TIMER_ACTIVE()            (g_stHdmiMacTx0HPDTimerStatus.b1HDMIHPDDebounceTimer)
#define SET_HDMI_MAC_TX0_HPD_DEBOUNCE_TIMER_ACTIVE()            (g_stHdmiMacTx0HPDTimerStatus.b1HDMIHPDDebounceTimer = _TRUE)
#define CLR_HDMI_MAC_TX0_HPD_DEBOUNCE_TIMER_ACTIVE()            (g_stHdmiMacTx0HPDTimerStatus.b1HDMIHPDDebounceTimer = _FALSE)

#define GET_HDMI_MAC_TX0_HPD_TOGGLE_TIMEOUT()                   (g_bHdmiMacTx0HPDToggleTimeout)
#define SET_HDMI_MAC_TX0_HPD_TOGGLE_TIMEOUT()                   (g_bHdmiMacTx0HPDToggleTimeout = _TRUE)
#define CLR_HDMI_MAC_TX0_HPD_TOGGLE_TIMEOUT()                   (g_bHdmiMacTx0HPDToggleTimeout = _FALSE)

#define GET_HDMI_MAC_TX0_HPD_LOW_DEGLITCH()                     (g_bHdmiMacTx0HPDLowDeglitch)
#define SET_HDMI_MAC_TX0_HPD_LOW_DEGLITCH()                     (g_bHdmiMacTx0HPDLowDeglitch = _TRUE)
#define CLR_HDMI_MAC_TX0_HPD_LOW_DEGLITCH()                     (g_bHdmiMacTx0HPDLowDeglitch = _FALSE)

#define GET_HDMI_MAC_TX0_HPD_TOGGLE_DETECTED_FROM_IRQ()         (g_bHdmiMacTx2HPDToggleDetectedDFromIrq)
#define SET_HDMI_MAC_TX0_HPD_TOGGLE_DETECTED_FROM_IRQ()         (g_bHdmiMacTx2HPDToggleDetectedDFromIrq = _TRUE)
#define CLR_HDMI_MAC_TX0_HPD_TOGGLE_DETECTED_FROM_IRQ()         (g_bHdmiMacTx2HPDToggleDetectedDFromIrq = _FALSE)

#define GET_HDMI_MAC_TX0_MODE_STATE()                           (g_stHdmiMacTx0ModeInfo.b4ModeState)
#define SET_HDMI_MAC_TX0_MODE_STATE(x)                          (g_stHdmiMacTx0ModeInfo.b4ModeState = (x))

#define GET_HDMI_MAC_TX0_MODE_STATE_CHANGE()                    (g_stHdmiMacTx0ModeInfo.b1ModeStateChanged)
#define SET_HDMI_MAC_TX0_MODE_STATE_CHANGE()                    (g_stHdmiMacTx0ModeInfo.b1ModeStateChanged = _TRUE)
#define CLR_HDMI_MAC_TX0_MODE_STATE_CHANGE()                    (g_stHdmiMacTx0ModeInfo.b1ModeStateChanged = _FALSE)

#define GET_HDMI_MAC_TX0_STREAM_SOURCE()                        (g_stHdmiMacTx0ModeInfo.b2StreamSource)
#define SET_HDMI_MAC_TX0_STREAM_SOURCE(x)                       (g_stHdmiMacTx0ModeInfo.b2StreamSource = (x))

#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)
#define GET_HDMI_MAC_TX0_SCRAMBLE_RETRY()                       (g_stHdmiMacTx0ModeInfo.b1HDMITxScrambleRetry)
#define SET_HDMI_MAC_TX0_SCRAMBLE_RETRY()                       (g_stHdmiMacTx0ModeInfo.b1HDMITxScrambleRetry = _TRUE)
#define CLR_HDMI_MAC_TX0_SCRAMBLE_RETRY()                       (g_stHdmiMacTx0ModeInfo.b1HDMITxScrambleRetry = _FALSE)

#define GET_HDMI_MAC_TX0_SCDC_PROC_STATE()                      (g_stHdmiMacTx0ModeInfo.ucHDMITxScrambleStatus)
#define SET_HDMI_MAC_TX0_SCDC_PROC_STATE(x)                     (g_stHdmiMacTx0ModeInfo.ucHDMITxScrambleStatus = (x))

#define GET_HDMI_MAC_TX0_SINK_SCRAMBLE_STATUS()                 (g_stHdmiMacTx0ModeInfo.ucHDMITxSinkScrambleStatus)
#define SET_HDMI_MAC_TX0_SINK_SCRAMBLE_STATUS(x)                (g_stHdmiMacTx0ModeInfo.ucHDMITxSinkScrambleStatus = (x))
#endif

#define GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS()                    (g_stHdmiMacTx0ModeInfo.b1HDMITxDownstreamStatus)
#define SET_HDMI_MAC_TX0_DOWNSTREAM_STATUS()                    (g_stHdmiMacTx0ModeInfo.b1HDMITxDownstreamStatus = _HIGH)
#define CLR_HDMI_MAC_TX0_DOWNSTREAM_STATUS()                    (g_stHdmiMacTx0ModeInfo.b1HDMITxDownstreamStatus = _LOW)

#define GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS_CHANGE()             (g_bHdmiMacTx0DownstreamPortStatusChange)
#define SET_HDMI_MAC_TX0_DOWNSTREAM_STATUS_CHANGE()             (g_bHdmiMacTx0DownstreamPortStatusChange = _TRUE)
#define CLR_HDMI_MAC_TX0_DOWNSTREAM_STATUS_CHANGE()             (g_bHdmiMacTx0DownstreamPortStatusChange = _FALSE)

//--------------------------------------------------
// Macros of HDMI Tx Video Input Infomation
//--------------------------------------------------
#define GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK()                      (g_stHdmiMacTx0InputInfo.usHDMITxInputPixelClk)
#define SET_HDMI_MAC_TX0_INPUT_PIXEL_CLK(x)                     (g_stHdmiMacTx0InputInfo.usHDMITxInputPixelClk = (x))
#define CLR_HDMI_MAC_TX0_INPUT_PIXEL_CLK()                      (g_stHdmiMacTx0InputInfo.usHDMITxInputPixelClk = 0)

#define GET_HDMI_MAC_TX0_DEEP_COLOR_CLK()                       (g_stHdmiMacTx0InputInfo.usHDMITxDeepColorClk)
#define SET_HDMI_MAC_TX0_DEEP_COLOR_CLK(x)                      (g_stHdmiMacTx0InputInfo.usHDMITxDeepColorClk = (x))

#define GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH()                    (g_stHdmiMacTx0InputInfo.b5HDMITxColorDepth)
#define SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(x)                   (g_stHdmiMacTx0InputInfo.b5HDMITxColorDepth = (x))

#define GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE()                    (g_stHdmiMacTx0InputInfo.b3HDMITxColorSpace)
#define SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(x)                   (g_stHdmiMacTx0InputInfo.b3HDMITxColorSpace = (x))

#define GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM()                     (g_stHdmiMacTx0InputInfo.b2HDMITxRepeatNum)
#define SET_HDMI_MAC_TX0_INPUT_REPEAT_NUM(x)                    (g_stHdmiMacTx0InputInfo.b2HDMITxRepeatNum = (x))

#define GET_HDMI_MAC_TX0_SUPPORT()                              (g_stHdmiMacTx0InputInfo.b1HDMITxSupport)
#define SET_HDMI_MAC_TX0_SUPPORT(x)                             (g_stHdmiMacTx0InputInfo.b1HDMITxSupport = (x))
#define CLR_HDMI_MAC_TX0_SUPPORT()                              (g_stHdmiMacTx0InputInfo.b1HDMITxSupport = 0)

#define GET_HDMI_MAC_TX0_INPUT_FRAME_RATE()                     (g_stHdmiMacTx0InputInfo.ucFrameRate)
#define SET_HDMI_MAC_TX0_INPUT_FRAME_RATE(x)                    (g_stHdmiMacTx0InputInfo.ucFrameRate = (x))

#define GET_HDMI_MAC_TX0_INPUT_INTERLACE()                      (g_stHdmiMacTx0InputInfo.b1HDMITxInterlace)
#define SET_HDMI_MAC_TX0_INPUT_INTERLACE()                      (g_stHdmiMacTx0InputInfo.b1HDMITxInterlace = _TRUE)
#define CLR_HDMI_MAC_TX0_INPUT_INTERLACE()                      (g_stHdmiMacTx0InputInfo.b1HDMITxInterlace = _FALSE)

#define GET_HDMI_MAC_TX0_INPUT_COLORIMETRY()                    (g_stHdmiMacTx0InputInfo.b4HDMITxColorimetry)
#define SET_HDMI_MAC_TX0_INPUT_COLORIMETRY(x)                   (g_stHdmiMacTx0InputInfo.b4HDMITxColorimetry = (x))

#define GET_HDMI_MAC_TX0_INPUT_EXT_COLORIMETRY()                (g_stHdmiMacTx0InputInfo.b5HDMITxExtColorimetry)
#define SET_HDMI_MAC_TX0_INPUT_EXT_COLORIMETRY(x)               (g_stHdmiMacTx0InputInfo.b5HDMITxExtColorimetry = (x))

#define GET_HDMI_MAC_TX0_INPUT_COLOR_RGB_QUANTIZATION_RANGE()   (g_stHdmiMacTx0InputInfo.b2HDMITxRgbQuantizationRange)
#define SET_HDMI_MAC_TX0_INPUT_COLOR_RGB_QUANTIZATION_RANGE(x)  (g_stHdmiMacTx0InputInfo.b2HDMITxRgbQuantizationRange = (x))

#define GET_HDMI_MAC_TX0_INPUT_COLOR_YCC_QUANTIZATION_RANGE()   (g_stHdmiMacTx0InputInfo.b2HDITxYccQuantizationRange)
#define SET_HDMI_MAC_TX0_INPUT_COLOR_YCC_QUANTIZATION_RANGE(x)  (g_stHdmiMacTx0InputInfo.b2HDITxYccQuantizationRange = (x))

#define GET_HDMI_MAC_TX0_HDCP_REKEY_WIN_SIZE()                  (g_stHdmiMacTx0InputInfo.ucHDMITxHdcpRekeyWinSize)
#define SET_HDMI_MAC_TX0_HDCP_REKEY_WIN_SIZE(x)                 (g_stHdmiMacTx0InputInfo.ucHDMITxHdcpRekeyWinSize = (x))

#define GET_HDMI_MAC_TX0_4K_VIC_SEND_BY_VSIF()                  (g_bHdmiMacTx04KVICSendByVSIF)
#define SET_HDMI_MAC_TX0_4K_VIC_SEND_BY_VSIF()                  (g_bHdmiMacTx04KVICSendByVSIF = _TRUE)
#define CLR_HDMI_MAC_TX0_4K_VIC_SEND_BY_VSIF()                  (g_bHdmiMacTx04KVICSendByVSIF = _FALSE)

//--------------------------------------------------
// Macros of HDMI Tx Audio Input Infomation
//--------------------------------------------------
#define GET_HDMI_MAC_TX0_AUDIO_FORMAT()                         (g_stHdmiMacTx0AudioInfo.b4HdmiTxAudioFormat)
#define SET_HDMI_MAC_TX0_AUDIO_FORMAT(x)                        (g_stHdmiMacTx0AudioInfo.b4HdmiTxAudioFormat = (x))

#define GET_HDMI_MAC_TX0_AUDIO_CH_NUM()                         (g_stHdmiMacTx0AudioInfo.b4HdmiTxAudioChNum)
#define SET_HDMI_MAC_TX0_AUDIO_CH_NUM(x)                        (g_stHdmiMacTx0AudioInfo.b4HdmiTxAudioChNum = (x))

#define GET_HDMI_MAC_TX0_AUDIO_FREQ()                           (g_stHdmiMacTx0AudioInfo.ucHdmiTxAudioFrequency)
#define SET_HDMI_MAC_TX0_AUDIO_FREQ(x)                          (g_stHdmiMacTx0AudioInfo.ucHdmiTxAudioFrequency = (x))

#define SET_HDMI_MAC_TX0_AUDIO_BW_ERROR()                       (g_stHdmiMacTx0AudioInfo.b1HdmiTxAudioBandwidthError = 1)
#define CLR_HDMI_MAC_TX0_AUDIO_BW_ERROR()                       (g_stHdmiMacTx0AudioInfo.b1HdmiTxAudioBandwidthError = 0)
#define GET_HDMI_MAC_TX0_AUDIO_BW_ERROR()                       (g_stHdmiMacTx0AudioInfo.b1HdmiTxAudioBandwidthError)

//--------------------------------------------------
// Macros of HDMI EDID Infomation
//--------------------------------------------------


#if(_HDMI_TX_CTS_TEST == _ON)
#define GET_HDMI_MAC_TX0_FIND_CTS_TEST_EDID()                   (g_bHdmiMacTx0FindCtsTestEdid)
#define SET_HDMI_MAC_TX0_FIND_CTS_TEST_EDID()                   (g_bHdmiMacTx0FindCtsTestEdid = _TRUE)
#define CLR_HDMI_MAC_TX0_FIND_CTS_TEST_EDID()                   (g_bHdmiMacTx0FindCtsTestEdid = _FALSE)


#endif // End of #if(_HDMI_TX_CTS_TEST == _ON)

#if((_HDMI_HDCP14_TX0_SUPPORT == _ON) || (_HDMI_HDCP2_TX0_SUPPORT == _ON))
#define GET_HDMI_MAC_TX0_HDCP_RETRY_COUNT()                     (g_ucHdmiMacTx0HdcpRetryCount)
#define ADD_HDMI_MAC_TX0_HDCP_RETRY_COUNT()                     (g_ucHdmiMacTx0HdcpRetryCount++)
#define CLR_HDMI_MAC_TX0_HDCP_RETRY_COUNT()                     (g_ucHdmiMacTx0HdcpRetryCount = 0)

#define GET_HDMI_MAC_TX0_HDCP_REPORT_RID_LIST()                 (g_bHdmiMacTx0HdcpReportRID)
#define SET_HDMI_MAC_TX0_HDCP_REPORT_RID_LIST()                 (g_bHdmiMacTx0HdcpReportRID = _TRUE)
#define CLR_HDMI_MAC_TX0_HDCP_REPORT_RID_LIST()                 (g_bHdmiMacTx0HdcpReportRID = _FALSE)

#define GET_HDMI_MAC_TX0_HDCP_AUDIO_MUTE()                      (g_bHdmiMacTx0HdcpAudioMute)
#define SET_HDMI_MAC_TX0_HDCP_AUDIO_MUTE()                      (g_bHdmiMacTx0HdcpAudioMute = _TRUE)
#define CLR_HDMI_MAC_TX0_HDCP_AUDIO_MUTE()                      (g_bHdmiMacTx0HdcpAudioMute = _FALSE)


#define GET_HDMI_MAC_TX0_HDCP_FIFO_CLR_EVENT()                  (g_bHdmiMacTx0HdcpFifoClrEvent)
#define SET_HDMI_MAC_TX0_HDCP_FIFO_CLR_EVENT()                  (g_bHdmiMacTx0HdcpFifoClrEvent = _TRUE)
#define CLR_HDMI_MAC_TX0_HDCP_FIFO_CLR_EVENT()                  (g_bHdmiMacTx0HdcpFifoClrEvent = _FALSE)

#define GET_HDMI_MAC_TX0_HPD_RASING_HAPPENED()                  ((GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_TOGGLE_EVENT) ||\
                                                                 (GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_Z0_TOGGLE_EVENT) ||\
                                                                 (GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_RISING_EVENT) ||\
                                                                 (GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_Z0_TOGGLE_EVENT))

#define GET_HDMI_MAC_TX0_HDCP_SLAVE_ACCESS_CNT()                (g_stHdmiMacTx0InputInfo.b5HdcpSlaveAccessCnt)
#define ADD_HDMI_MAC_TX0_HDCP_SLAVE_ACCESS_CNT()                (g_stHdmiMacTx0InputInfo.b5HdcpSlaveAccessCnt++)
#define CLR_HDMI_MAC_TX0_HDCP_SLAVE_ACCESS_CNT()                (g_stHdmiMacTx0InputInfo.b5HdcpSlaveAccessCnt = 0)

#define GET_HDMI_MAC_TX0_HDCP_REAUTH_FROM_LINK_ON()             (g_stHdmiMacTx0InputInfo.b1HDMITxHdcpReauthFromLinkOn)
#define SET_HDMI_MAC_TX0_HDCP_REAUTH_FROM_LINK_ON()             (g_stHdmiMacTx0InputInfo.b1HDMITxHdcpReauthFromLinkOn = _TRUE)
#define CLR_HDMI_MAC_TX0_HDCP_REAUTH_FROM_LINK_ON()             (g_stHdmiMacTx0InputInfo.b1HDMITxHdcpReauthFromLinkOn = _FALSE)

#define GET_HDMI_MAC_TX0_HDCP_AUTH_GO()                         (g_bHdmiMacTx0HdcpAuthGo)
#define SET_HDMI_MAC_TX0_HDCP_AUTH_GO()                         (g_bHdmiMacTx0HdcpAuthGo = _TRUE)
#define CLR_HDMI_MAC_TX0_HDCP_AUTH_GO()                         (g_bHdmiMacTx0HdcpAuthGo = _FALSE)


#endif

#if(_AUDIO_SUPPORT == _ON)
#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
#define GET_HDMI_MAC_TX0_CTS_CODE_CHECK_EVENT()                 (g_bHdmiMacTx0CtsCodeCheckEvent)
#define SET_HDMI_MAC_TX0_CTS_CODE_CHECK_EVENT()                 (g_bHdmiMacTx0CtsCodeCheckEvent = _TRUE)
#define CLR_HDMI_MAC_TX0_CTS_CODE_CHECK_EVENT()                 (g_bHdmiMacTx0CtsCodeCheckEvent = _FALSE)

#define GET_HDMI_MAC_TX0_CTS_CODE_FW_MODE()                     (g_bHdmiMacTx0CtsCodeFwMode)
#define SET_HDMI_MAC_TX0_CTS_CODE_FW_MODE()                     (g_bHdmiMacTx0CtsCodeFwMode = _TRUE)
#define CLR_HDMI_MAC_TX0_CTS_CODE_FW_MODE()                     (g_bHdmiMacTx0CtsCodeFwMode = _FALSE)
#endif

#define SET_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE()                    (rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_55_reg, ~(_BIT7 | _BIT4 | _BIT3 | _BIT0), _BIT0))
#define CLR_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE()                    (rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_55_reg, ~(_BIT7 | _BIT4 | _BIT3 | _BIT0), 0x00))

#define SET_HDMITX_AUDIO_INPUT_TYPE(x)                                  (g_ucHdmiMacTx0AudioInputType = x)
#define GET_HDMITX_AUDIO_INPUT_TYPE()                                  (g_ucHdmiMacTx0AudioInputType)
#else
#define GET_HDMITX_AUDIO_INPUT_TYPE() (_TX_AUDIO_IN_RX_PACKET)
#endif

//--------------------------------------------------
// Macros of HDMI SW Packet
//--------------------------------------------------
#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
#define GET_HDMI_MAC_TX0_SW_PKT_DFF0_USED()                     (g_bHdmiMacTx0SWPktDFF0Used)
#define SET_HDMI_MAC_TX0_SW_PKT_DFF0_USED()                     (g_bHdmiMacTx0SWPktDFF0Used = _TRUE)
#define CLR_HDMI_MAC_TX0_SW_PKT_DFF0_USED()                     (g_bHdmiMacTx0SWPktDFF0Used = _FALSE)

#define GET_HDMI_MAC_TX0_SW_PKT_DFF1_USED()                     (g_bHdmiMacTx0SWPktDFF1Used)
#define SET_HDMI_MAC_TX0_SW_PKT_DFF1_USED()                     (g_bHdmiMacTx0SWPktDFF1Used = _TRUE)
#define CLR_HDMI_MAC_TX0_SW_PKT_DFF1_USED()                     (g_bHdmiMacTx0SWPktDFF1Used = _FALSE)
#endif


//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************


//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************
extern StructHDMITxInputInfo g_stHdmiMacTx0InputInfo;
extern StructTimingInfo *g_stHdmiMacTx0InputTimingInfo;
extern BOOLEAN g_bHdmiMacTx1SegmentReadable;

#if(_HDMI_MAC_TX0_PURE == _ON)
extern BOOLEAN g_bHdmiMacTx0HPDZ0ToggleTimeout;
extern BOOLEAN g_bHdmiMacTx0Z0ToggleTimeout;
#endif
extern StructHDMITxHPDTimerStatus g_stHdmiMacTx0HPDTimerStatus;
extern BOOLEAN g_bHdmiMacTx0HPDToggleTimeout;
extern BOOLEAN g_bHdmiMacTx0DownstreamPortStatusChange;
extern BOOLEAN g_bHdmiMacTx0DownstreamPortStatusChange;
extern BOOLEAN g_bHdmiMacTx2HPDToggleDetectedDFromIrq;

#if((_HDMI_HDCP14_TX0_SUPPORT == _ON) || (_HDMI_HDCP2_TX0_SUPPORT == _ON))
extern BOOLEAN g_bHdmiMacTx0HdcpReportRID;
extern UINT8 g_ucHdmiMacTx0HdcpRetryCount;
extern BOOLEAN g_bHdmiMacTx0HdcpAudioMute;
extern BOOLEAN g_bHdmiMacTx0HdcpFifoClrEvent;
extern UINT16 g_usHdmiMacTx0HdcpKsvFifoStartIndex;
extern UINT16 g_usHdmiMacTx0HdcpKsvFifoLength;
extern BOOLEAN g_bHdmiMacTx0HdcpAuthGo;
#endif

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
extern StructHDMITxModeInfoType g_stHdmiMacTx0ModeInfo;
extern void ScalerHdmiMacTx0Handler(void);
extern void ScalerHdmiMacTx0Initial(void);
extern void ScalerHdmiMacTx0PowerProc(EnumPowerAction enumPowerAction);
extern BOOLEAN ScalerHdmiMacTx0GetHpdStatus(void);
extern void ScalerHdmiMacTx0SmallFIFOSet(void);
#if(_HDMI_MAC_TX0_DP_PLUS == _ON)
extern void ScalerHdmiMacTx0SetDPTxClkDiv(UINT8 ucDivNum);
#endif
extern void ScalerHdmiMacTx0SetClkDiv4(BOOLEAN bEnable);
extern BOOLEAN ScalerHdmiMacTx0CableDetect(void);
extern void ScalerHdmiMacTx0TopPowerReset(void);
extern void ScalerHdmiMacTx0RegRecovery(void);
#if(_HW_PHY_TX_TYPE == _HW_PHY_TX_GEN_2)
extern void ScalerHdmiMacTx0SetSmallFIFOQuadRate(void);
#endif
extern void ScalerHdmiMacTx0SetSmallFIFODoubleRate(void);
extern void ScalerHdmiMacTx0SetSmallFIFOFullRate(void);
extern void ScalerHdmiMacTx0SetSmallFIFOHalfRate(void);
extern BOOLEAN ScalerHdmiMacTx0CableDetect(void);

#if(_AUDIO_SUPPORT == _ON)
extern EnumHdmiTxAudioInputType g_ucHdmiMacTx0AudioInputType;
#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
extern void ScalerHdmiMacTx0CtsCodeCheckEventSet(void);
#endif
#endif

extern void ScalerHdmiMacTx0EdidSetFeature(EnumEdidFeatureType enumEdidFeature, UINT8 ucValue);
extern UINT8 ScalerHdmiMacTx0EdidGetFeature(EnumEdidFeatureType enumEdidFeature);
extern void ScalerHdmiMacTx0SetSinkTmdsSupportClk(UINT8 ucHdmiVsdbMaxTMDSClk, UINT8 ucHfVsdbMaxTMDSClk);

#if((_HDMI_HDCP14_TX0_SUPPORT == _ON) || (_HDMI_HDCP2_TX0_SUPPORT == _ON))
extern void ScalerHdmiMacTx0HdcpEncryptSignalProc(BOOLEAN bEnable);
extern BOOLEAN ScalerHdmiMacTx0GetHdcpEncStatus(void);
extern void ScalerHdmiMacTx0HdcpCapPollinEvent(void);
#endif
extern void ScalerHdmiMacTx0EdidModifyEnable(BOOLEAN bEnable, EnumHdmiEdidModifyEvent enumEdidModifyEvent);
extern void ScalerHdmiMacTx0GetEdidVicTable(EnumEdidCtaDataBlock enumVDBType, UINT8 *pucVICTable, UINT16 *pusCtaDataBlockAddr);

#endif // End of #if(_HDMI_MAC_TX0_SUPPORT == _ON)

#endif // #ifndef __SCALER_HDMI_MAC_TX0_INCLUDE_H__
