#ifndef __SCALER_HDMI21_MAC_TX_INCLUDE_H__
#define __SCALER_HDMI21_MAC_TX_INCLUDE_H__

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
// ID Code      : ScalerHdmi21MacTxInclude.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include "../ScalerFunctionInclude.h"
#include "../HdmiMacTx/ScalerHdmiMacTxInclude.h"

#if(_HDMI21_TX_SUPPORT == _ON)

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#define GET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(x)                              (g_pstHdmi21MacTxInfo[x].b2HDMI21LinkTrainingResult)
#define SET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(x, y)                           (g_pstHdmi21MacTxInfo[x].b2HDMI21LinkTrainingResult = (y))

#define GET_HDMI21_MAC_TX_BW_EVENT(x)                                          (g_pstHdmi21MacTxInfo[x].b4HDMI21BWEvent)
#define SET_HDMI21_MAC_TX_BW_EVENT(x, y)                                       (g_pstHdmi21MacTxInfo[x].b4HDMI21BWEvent = (y))
#define CLR_HDMI21_MAC_TX_BW_EVENT(x)                                          (g_pstHdmi21MacTxInfo[x].b4HDMI21BWEvent = (0))

#define GET_HDMI21_MAC_TX_RE_LINK_TRAINING(x)                                  (g_pstHdmi21MacTxInfo[x].b1HDMI21ReLinkTraing)
#define SET_HDMI21_MAC_TX_RE_LINK_TRAINING(x)                                  (g_pstHdmi21MacTxInfo[x].b1HDMI21ReLinkTraing = (1))
#define CLR_HDMI21_MAC_TX_RE_LINK_TRAINING(x)                                  (g_pstHdmi21MacTxInfo[x].b1HDMI21ReLinkTraing = (0))

#define GET_HDMI21_MAC_TX_BW_RESULT(x)                                         (g_pstHdmi21MacTxInfo[x].b4HDMI21BWResult)
#define SET_HDMI21_MAC_TX_BW_RESULT(x, y)                                      (g_pstHdmi21MacTxInfo[x].b4HDMI21BWResult = (y))
#define CLR_HDMI21_MAC_TX_BW_RESULT(x)                                         (g_pstHdmi21MacTxInfo[x].b4HDMI21BWResult = (0))

#if(_AUDIO_SUPPORT == _ON)
#define GET_HDMI21_MAC_TX_AUDIO_OFF(x)                                         (g_pstHdmi21MacTxInfo[x].b1HdmiAudioOff)
#define SET_HDMI21_MAC_TX_AUDIO_OFF(x)                                         (g_pstHdmi21MacTxInfo[x].b1HdmiAudioOff = 1)
#define CLR_HDMI21_MAC_TX_AUDIO_OFF(x)                                         (g_pstHdmi21MacTxInfo[x].b1HdmiAudioOff = 0)
#endif

#define GET_HDMI21_MAC_FORCE_LEGACY(x)                                         (g_pstHdmi21MacTxInfo[x].b1HDMI21ForceLegacy)
#define SET_HDMI21_MAC_FORCE_LEGACY(x)                                         (g_pstHdmi21MacTxInfo[x].b1HDMI21ForceLegacy = (1))
#define CLR_HDMI21_MAC_FORCE_LEGACY(x)                                         (g_pstHdmi21MacTxInfo[x].b1HDMI21ForceLegacy = (0))

#define GET_HDMI21_MAC_VIDEO_START(x)                                          (g_pstHdmi21MacTxInfo[x].b1HDMI21VideoStart)
#define SET_HDMI21_MAC_VIDEO_START(x)                                          (g_pstHdmi21MacTxInfo[x].b1HDMI21VideoStart = (1))
#define CLR_HDMI21_MAC_VIDEO_START(x)                                          (g_pstHdmi21MacTxInfo[x].b1HDMI21VideoStart = (0))

//--------------------------------------------------
// Macro of Link Training State
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_LINK_TRAINING_STATE(x)                               (g_pstHdmi21MacTxCtrlInfo[x].b4LtState)
#define SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(x, y)                            (g_pstHdmi21MacTxCtrlInfo[x].b4LtState = (y))

//--------------------------------------------------
// Macro of Rx Send Scdc Paramter
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(x)                         (g_pstHdmi21MacTxLTP[x].ucUpdateFlag10)
#define SET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(x, y)                      (g_pstHdmi21MacTxLTP[x].ucUpdateFlag10 = (y))
#define CLR_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(x)                         (g_pstHdmi21MacTxLTP[x].ucUpdateFlag10 = 0)

#define GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(x)                         (g_pstHdmi21MacTxLTP[x].ucTestConfig35)
#define SET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(x, y)                      (g_pstHdmi21MacTxLTP[x].ucTestConfig35 = (y))
#define CLR_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(x)                         (g_pstHdmi21MacTxLTP[x].ucTestConfig35 = 0)

#define GET_HDMI21_MAC_TX_FROM_RX_SCDC40_STATUSFLAG(x)                         (g_pstHdmi21MacTxLTP[x].ucStatusFlag40)
#define SET_HDMI21_MAC_TX_FROM_RX_SCDC40_STATUSFLAG(x, y)                      (g_pstHdmi21MacTxLTP[x].ucStatusFlag40 = (y))
#define CLR_HDMI21_MAC_TX_FROM_RX_SCDC40_STATUSFLAG(x)                         (g_pstHdmi21MacTxLTP[x].ucStatusFlag40 = 0)

#define GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(x)                         (g_pstHdmi21MacTxLTP[x].b4L0LTPRequest)
#define SET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(x, y)                      (g_pstHdmi21MacTxLTP[x].b4L0LTPRequest = (y))
#define CLR_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(x)                         (g_pstHdmi21MacTxLTP[x].b4L0LTPRequest = 0)

#define GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(x)                         (g_pstHdmi21MacTxLTP[x].b4L1LTPRequest)
#define SET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(x, y)                      (g_pstHdmi21MacTxLTP[x].b4L1LTPRequest = (y))
#define CLR_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(x)                         (g_pstHdmi21MacTxLTP[x].b4L1LTPRequest = 0)

#define GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(x)                         (g_pstHdmi21MacTxLTP[x].b4L2LTPRequest)
#define SET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(x, y)                      (g_pstHdmi21MacTxLTP[x].b4L2LTPRequest = (y))
#define CLR_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(x)                         (g_pstHdmi21MacTxLTP[x].b4L2LTPRequest = 0)

#define GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(x)                         (g_pstHdmi21MacTxLTP[x].b4L3LTPRequest)
#define SET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(x, y)                      (g_pstHdmi21MacTxLTP[x].b4L3LTPRequest = (y))
#define CLR_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(x)                         (g_pstHdmi21MacTxLTP[x].b4L3LTPRequest = 0)

//--------------------------------------------------
// Macro of FRL Link Rate
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_FRL_RATE(x)                                          (g_pstHdmi21MacTxCtrlInfo[x].b4FrlRate)
#define SET_HDMI21_MAC_TX_FRL_RATE(x, y)                                       (g_pstHdmi21MacTxCtrlInfo[x].b4FrlRate = (y))
#define CLR_HDMI21_MAC_TX_FRL_RATE(x)                                          (g_pstHdmi21MacTxCtrlInfo[x].b4FrlRate = 0)

#define GET_HDMI21_MAC_TX_FIRST_FRL_RATE(x)                                    (g_pstHdmi21MacTxCtrlInfo[x].b4FirstFrlRate)
#define SET_HDMI21_MAC_TX_FIRST_FRL_RATE(x, y)                                 (g_pstHdmi21MacTxCtrlInfo[x].b4FirstFrlRate = (y))
#define CLR_HDMI21_MAC_TX_FIRST_FRL_RATE(x)                                    (g_pstHdmi21MacTxCtrlInfo[x].b4FirstFrlRate = 0)

//--------------------------------------------------
// Macro of FFE Level
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_FFE_LEVEL(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4FfeLevel)
#define SET_HDMI21_MAC_TX_FFE_LEVEL(x, y)                                      (g_pstHdmi21MacTxCtrlInfo[x].b4FfeLevel = (y))
#define CLR_HDMI21_MAC_TX_FFE_LEVEL(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4FfeLevel = 0)

#define GET_HDMI21_MAC_TX_LANE0_FFE(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4Lane0Ffe)
#define ADD_HDMI21_MAC_TX_LANE0_FFE(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4Lane0Ffe++)
#define CLR_HDMI21_MAC_TX_LANE0_FFE(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4Lane0Ffe = 0)

#define GET_HDMI21_MAC_TX_LANE1_FFE(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4Lane1Ffe)
#define ADD_HDMI21_MAC_TX_LANE1_FFE(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4Lane1Ffe++)
#define CLR_HDMI21_MAC_TX_LANE1_FFE(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4Lane1Ffe = 0)

#define GET_HDMI21_MAC_TX_LANE2_FFE(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4Lane2Ffe)
#define ADD_HDMI21_MAC_TX_LANE2_FFE(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4Lane2Ffe++)
#define CLR_HDMI21_MAC_TX_LANE2_FFE(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4Lane2Ffe = 0)

#define GET_HDMI21_MAC_TX_LANE3_FFE(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4Lane3Ffe)
#define ADD_HDMI21_MAC_TX_LANE3_FFE(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4Lane3Ffe++)
#define CLR_HDMI21_MAC_TX_LANE3_FFE(x)                                         (g_pstHdmi21MacTxCtrlInfo[x].b4Lane3Ffe = 0)

//--------------------------------------------------
// Macro of FFE Mode
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_LANE0_FFEMODE(x)                                     (g_pstHdmi21MacTxCtrlInfo[x].b4Lane0FfeMode)
#define SET_HDMI21_MAC_TX_LANE0_FFEMODE(x, y)                                  (g_pstHdmi21MacTxCtrlInfo[x].b4Lane0FfeMode = (y))
#define CLR_HDMI21_MAC_TX_LANE0_FFEMODE(x)                                     (g_pstHdmi21MacTxCtrlInfo[x].b4Lane0FfeMode = 0)

#define GET_HDMI21_MAC_TX_LANE1_FFEMODE(x)                                     (g_pstHdmi21MacTxCtrlInfo[x].b4Lane1FfeMode)
#define SET_HDMI21_MAC_TX_LANE1_FFEMODE(x, y)                                  (g_pstHdmi21MacTxCtrlInfo[x].b4Lane1FfeMode = (y))
#define CLR_HDMI21_MAC_TX_LANE1_FFEMODE(x)                                     (g_pstHdmi21MacTxCtrlInfo[x].b4Lane1FfeMode = 0)

#define GET_HDMI21_MAC_TX_LANE2_FFEMODE(x)                                     (g_pstHdmi21MacTxCtrlInfo[x].b4Lane2FfeMode)
#define SET_HDMI21_MAC_TX_LANE2_FFEMODE(x, y)                                  (g_pstHdmi21MacTxCtrlInfo[x].b4Lane2FfeMode = (y))
#define CLR_HDMI21_MAC_TX_LANE2_FFEMODE(x)                                     (g_pstHdmi21MacTxCtrlInfo[x].b4Lane2FfeMode = 0)

#define GET_HDMI21_MAC_TX_LANE3_FFEMODE(x)                                     (g_pstHdmi21MacTxCtrlInfo[x].b4Lane3FfeMode)
#define SET_HDMI21_MAC_TX_LANE3_FFEMODE(x, y)                                  (g_pstHdmi21MacTxCtrlInfo[x].b4Lane3FfeMode = (y))
#define CLR_HDMI21_MAC_TX_LANE3_FFEMODE(x)                                     (g_pstHdmi21MacTxCtrlInfo[x].b4Lane3FfeMode = 0)

//--------------------------------------------------
// Macro of FRL No Timeout Flag
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_FRL_NOTIMEOUT(x)                                     (g_pstHdmi21MacTxCtrlInfo[x].b1FLTNoTimeOut)
#define SET_HDMI21_MAC_TX_FRL_NOTIMEOUT(x)                                     (g_pstHdmi21MacTxCtrlInfo[x].b1FLTNoTimeOut = 1)
#define CLR_HDMI21_MAC_TX_FRL_NOTIMEOUT(x)                                     (g_pstHdmi21MacTxCtrlInfo[x].b1FLTNoTimeOut = 0)

//--------------------------------------------------
// Macro of FRL DSC MAX Flag
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_FRL_DSC_MAX(x)                                       (g_pstHdmi21MacTxCtrlInfo[x].b1FLTDscFrlMax)
#define SET_HDMI21_MAC_TX_FRL_DSC_MAX(x)                                       (g_pstHdmi21MacTxCtrlInfo[x].b1FLTDscFrlMax = 1)
#define CLR_HDMI21_MAC_TX_FRL_DSC_MAX(x)                                       (g_pstHdmi21MacTxCtrlInfo[x].b1FLTDscFrlMax = 0)

//--------------------------------------------------
// Macro of FRL MAX Flag
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_FRL_MAX(x)                                           (g_pstHdmi21MacTxCtrlInfo[x].b1FLTFrlMax)
#define SET_HDMI21_MAC_TX_FRL_MAX(x)                                           (g_pstHdmi21MacTxCtrlInfo[x].b1FLTFrlMax = 1)
#define CLR_HDMI21_MAC_TX_FRL_MAX(x)                                           (g_pstHdmi21MacTxCtrlInfo[x].b1FLTFrlMax = 0)

//--------------------------------------------------
// Macro of FRL SetPhy Flag LTS2 => LTS3
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_FRL_SETPHY(x)                                        (g_pstHdmi21MacTxCtrlInfo[x].b1FirstSetphy)
#define SET_HDMI21_MAC_TX_FRL_SETPHY(x)                                        (g_pstHdmi21MacTxCtrlInfo[x].b1FirstSetphy = 1)
#define CLR_HDMI21_MAC_TX_FRL_SETPHY(x)                                        (g_pstHdmi21MacTxCtrlInfo[x].b1FirstSetphy = 0)

//--------------------------------------------------
// Macro of link traing Lts3 timer Start Flag
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_LTS3_TIMER_START(x)                                  (g_pstHdmi21MacTxCtrlInfo[x].b1Lts3TimerStart)
#define SET_HDMI21_MAC_TX_LTS3_TIMER_START(x)                                  (g_pstHdmi21MacTxCtrlInfo[x].b1Lts3TimerStart = 1)
#define CLR_HDMI21_MAC_TX_LTS3_TIMER_START(x)                                  (g_pstHdmi21MacTxCtrlInfo[x].b1Lts3TimerStart = 0)

//--------------------------------------------------
// Macro of link traing Lts3 timer total counter 200 msec
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(x)                                (g_pstHdmi21MacTxCtrlInfo[x].ucLts3TimerCounter)
#define ADD_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(x)                                (g_pstHdmi21MacTxCtrlInfo[x].ucLts3TimerCounter++)
#define CLR_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(x)                                (g_pstHdmi21MacTxCtrlInfo[x].ucLts3TimerCounter = 0)

//--------------------------------------------------
// Macro of link traing Lts3 timer Start Flag
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_LTS_PASS_POLLING(x)                                  (g_pstHdmi21MacTxCtrlInfo[x].b1LtsPassPolling)
#define SET_HDMI21_MAC_TX_LTS_PASS_POLLING(x)                                  (g_pstHdmi21MacTxCtrlInfo[x].b1LtsPassPolling = 1)
#define CLR_HDMI21_MAC_TX_LTS_PASS_POLLING(x)                                  (g_pstHdmi21MacTxCtrlInfo[x].b1LtsPassPolling = 0)

// for CTS LTP_3/LTP_F test
#define GET_HDMI21_MAC_TX_CTS_IGNORE_LTP_EN(x)                                  (bSkipCtsLtpEnFlag)
#define SET_HDMI21_MAC_TX_CTS_IGNORE_LTP_EN(x)                                  (bSkipCtsLtpEnFlag = 1)
#define CLR_HDMI21_MAC_TX_CTS_IGNORE_LTP_EN(x)                                  (bSkipCtsLtpEnFlag = 0)

// for CTS Link Training retrain
#define GET_HDMI21_MAC_TX_CTS_LT_RETRAIN_EN(x)                                  (bCtsRetrainEnFlag)
#define SET_HDMI21_MAC_TX_CTS_LT_RETRAIN_EN(x)                                  (bCtsRetrainEnFlag = 1)
#define CLR_HDMI21_MAC_TX_CTS_LT_RETRAIN_EN(x)                                  (bCtsRetrainEnFlag = 0)


//--------------------------------------------------
// Define Output Stream Info Macro
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(x)                              (g_pstHdmi21MacTxStreamInfo[x].b4TXStreamState)
#define SET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(x, y)                           (g_pstHdmi21MacTxStreamInfo[x].b4TXStreamState = (y))

#define GET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(x)                                   (g_pstHdmi21MacTxStreamInfo[x].b4TXDownStreamType)
#define SET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(x, y)                                (g_pstHdmi21MacTxStreamInfo[x].b4TXDownStreamType = (y))

#define GET_HDMI21_MAC_TX_INPUT_FROM(x)                                        (g_pstHdmi21MacTxStreamInfo[x].b4TXInputFrom)
#define SET_HDMI21_MAC_TX_INPUT_FROM(x, y)                                     (g_pstHdmi21MacTxStreamInfo[x].b4TXInputFrom = (y))

#define GET_HDMI21_MAC_TX_INPUT_PIXEL_MODE(x)                                  (g_pstHdmi21MacTxStreamInfo[x].b4TXInputPixelMode)
#define SET_HDMI21_MAC_TX_INPUT_PIXEL_MODE(x, y)                               (g_pstHdmi21MacTxStreamInfo[x].b4TXInputPixelMode = (y))

#define GET_HDMI21_MAC_TX_INFOR_OK(x)                                          (g_pstHdmi21MacTxStreamInfo[x].b1TXInputInforOK)
#define SET_HDMI21_MAC_TX_INFOR_OK(x)                                          (g_pstHdmi21MacTxStreamInfo[x].b1TXInputInforOK = (1))
#define CLR_HDMI21_MAC_TX_INFOR_OK(x)                                          (g_pstHdmi21MacTxStreamInfo[x].b1TXInputInforOK = (0))

#define GET_HDMI21_MAC_TX_DECODER_EN(x)                                        (g_pstHdmi21MacTxStreamInfo[x].b1TXDecoderEnable)
#define SET_HDMI21_MAC_TX_DECODER_EN(x)                                        (g_pstHdmi21MacTxStreamInfo[x].b1TXDecoderEnable = (1))
#define CLR_HDMI21_MAC_TX_DECODER_EN(x)                                        (g_pstHdmi21MacTxStreamInfo[x].b1TXDecoderEnable = (0))

#define GET_HDMI21_MAC_TX_ENCODER_EN(x)                                        (g_pstHdmi21MacTxStreamInfo[x].b1TXEncoderEnable)
#define SET_HDMI21_MAC_TX_ENCODER_EN(x)                                        (g_pstHdmi21MacTxStreamInfo[x].b1TXEncoderEnable = (1))
#define CLR_HDMI21_MAC_TX_ENCODER_EN(x)                                        (g_pstHdmi21MacTxStreamInfo[x].b1TXEncoderEnable = (0))
//--------------------------------------------------
// Macros of HDMI Tx Video Input Infomation
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(x)                                   (g_pstHdmi21MacTxInputInfo[x].usHDMITxInputPixelClk)
#define SET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(x, y)                                (g_pstHdmi21MacTxInputInfo[x].usHDMITxInputPixelClk = (y))
#define CLR_HDMI21_MAC_TX_INPUT_PIXEL_CLK(x)                                   (g_pstHdmi21MacTxInputInfo[x].usHDMITxInputPixelClk = 0)

#define GET_HDMI21_MAC_TX_DEEP_COLOR_CLK(x)                                    (g_pstHdmi21MacTxInputInfo[x].usHDMITxDeepColorClk)
#define SET_HDMI21_MAC_TX_DEEP_COLOR_CLK(x, y)                                 (g_pstHdmi21MacTxInputInfo[x].usHDMITxDeepColorClk = (y))

#define GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(x)                                 (g_pstHdmi21MacTxInputInfo[x].b5HDMITxColorDepth)
#define SET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(x, y)                              (g_pstHdmi21MacTxInputInfo[x].b5HDMITxColorDepth = (y))

#define GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(x)                                 (g_pstHdmi21MacTxInputInfo[x].b3HDMITxColorSpace)
#define SET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(x, y)                              (g_pstHdmi21MacTxInputInfo[x].b3HDMITxColorSpace = (y))

#define GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(x)                                  (g_pstHdmi21MacTxInputInfo[x].b2HDMITxRepeatNum)
#define SET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(x, y)                               (g_pstHdmi21MacTxInputInfo[x].b2HDMITxRepeatNum = (y))

#define GET_HDMI21_MAC_TX_SUPPORT(x)                                           (g_pstHdmi21MacTxInputInfo[x].b1HDMITxSupport)
#define SET_HDMI21_MAC_TX_SUPPORT(x, y)                                        (g_pstHdmi21MacTxInputInfo[x].b1HDMITxSupport = (y))
#define CLR_HDMI21_MAC_TX_SUPPORT(x)                                           (g_pstHdmi21MacTxInputInfo[x].b1HDMITxSupport = 0)

#define GET_HDMI21_MAC_TX_INPUT_FRAME_RATE(x)                                  (g_pstHdmi21MacTxInputInfo[x].ucFrameRate)
#define SET_HDMI21_MAC_TX_INPUT_FRAME_RATE(x, y)                               (g_pstHdmi21MacTxInputInfo[x].ucFrameRate = (y))

#define GET_HDMI21_MAC_TX_INPUT_INTERLACE(x)                                   (g_pstHdmi21MacTxInputInfo[x].b1HDMITxInterlace)
#define SET_HDMI21_MAC_TX_INPUT_INTERLACE(x)                                   (g_pstHdmi21MacTxInputInfo[x].b1HDMITxInterlace = _TRUE)
#define CLR_HDMI21_MAC_TX_INPUT_INTERLACE(x)                                   (g_pstHdmi21MacTxInputInfo[x].b1HDMITxInterlace = _FALSE)

#define GET_HDMI21_MAC_TX_INPUT_COLORIMETRY(x)                                 (g_pstHdmi21MacTxInputInfo[x].b4HDMITxColorimetry)
#define SET_HDMI21_MAC_TX_INPUT_COLORIMETRY(x, y)                              (g_pstHdmi21MacTxInputInfo[x].b4HDMITxColorimetry = (y))

#define GET_HDMI21_MAC_TX_INPUT_EXT_COLORIMETRY(x)                             (g_pstHdmi21MacTxInputInfo[x].b5HDMITxExtColorimetry)
#define SET_HDMI21_MAC_TX_INPUT_EXT_COLORIMETRY(x, y)                          (g_pstHdmi21MacTxInputInfo[x].b5HDMITxExtColorimetry = (y))

#define GET_HDMI21_MAC_TX_INPUT_COLOR_RGB_QUANTIZATION_RANGE(x)                (g_pstHdmi21MacTxInputInfo[x].b2HDMITxRgbQuantizationRange)
#define SET_HDMI21_MAC_TX_INPUT_COLOR_RGB_QUANTIZATION_RANGE(x, y)             (g_pstHdmi21MacTxInputInfo[x].b2HDMITxRgbQuantizationRange = (y))

#define GET_HDMI21_MAC_TX_INPUT_COLOR_YCC_QUANTIZATION_RANGE(x)                (g_pstHdmi21MacTxInputInfo[x].b2HDITxYccQuantizationRange)
#define SET_HDMI21_MAC_TX_INPUT_COLOR_YCC_QUANTIZATION_RANGE(x, y)             (g_pstHdmi21MacTxInputInfo[x].b2HDITxYccQuantizationRange = (y))

#define GET_HDMI21_MAC_TX_HDCP_REKEY_WIN_SIZE(x)                               (g_pstHdmi21MacTxInputInfo[x].ucHDMITxHdcpRekeyWinSize)
#define SET_HDMI21_MAC_TX_HDCP_REKEY_WIN_SIZE(x, y)                            (g_pstHdmi21MacTxInputInfo[x].ucHDMITxHdcpRekeyWinSize = (y))

//--------------------------------------------------
// Macros of HDMI Tx Audio
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_AUDIO_FORMAT(x)                                      (g_pstHdmi21MacTxAudioInfo[x].b4HdmiTxAudioFormat)
#define SET_HDMI21_MAC_TX_AUDIO_FORMAT(x, y)                                   (g_pstHdmi21MacTxAudioInfo[x].b4HdmiTxAudioFormat = (y))

#define GET_HDMI21_MAC_TX_AUDIO_CH_NUM(x)                                      (g_pstHdmi21MacTxAudioInfo[x].b4HdmiTxAudioChNum)
#define SET_HDMI21_MAC_TX_AUDIO_CH_NUM(x, y)                                   (g_pstHdmi21MacTxAudioInfo[x].b4HdmiTxAudioChNum = (y))

#define GET_HDMI21_MAC_TX_AUDIO_FREQ(x)                                        (g_pstHdmi21MacTxAudioInfo[x].ucHdmiTxAudioFrequency)
#define SET_HDMI21_MAC_TX_AUDIO_FREQ(x, y)                                     (g_pstHdmi21MacTxAudioInfo[x].ucHdmiTxAudioFrequency = (y))

#define SET_HDMI21_MAC_TX_AUDIO_BW_ERROR(x)                                    (g_pstHdmi21MacTxAudioInfo[x].b1HdmiTxAudioBandwidthError = 1)
#define CLR_HDMI21_MAC_TX_AUDIO_BW_ERROR(x)                                    (g_pstHdmi21MacTxAudioInfo[x].b1HdmiTxAudioBandwidthError = 0)
#define GET_HDMI21_MAC_TX_AUDIO_BW_ERROR(x)                                    (g_pstHdmi21MacTxAudioInfo[x].b1HdmiTxAudioBandwidthError)

//--------------------------------------------------
// Macro of HDMI2.1 Bandwidth parameter
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_BORROW(x)                                            (g_pstHdmi21MacTxBandWidthParameter[x].b1HdmiInBorrow)
#define SET_HDMI21_MAC_TX_BORROW(x)                                            (g_pstHdmi21MacTxBandWidthParameter[x].b1HdmiInBorrow = 1)
#define CLR_HDMI21_MAC_TX_BORROW(x)                                            (g_pstHdmi21MacTxBandWidthParameter[x].b1HdmiInBorrow = 0)

//--------------------------------------------------
// Macro of HDMI2.1 DSCE control
//--------------------------------------------------
#define GET_HDMI21_MAC_TX_DSCE_EN(x)                                            (g_pstHdmi21MacTxBandWidthParameter[x].b1HdmiInEnableDscEncoder)
#define SET_HDMI21_MAC_TX_DSCE_EN(x, y)                                            (g_pstHdmi21MacTxBandWidthParameter[x].b1HdmiInEnableDscEncoder = y)
#define CLR_HDMI21_MAC_TX_DSCE_EN(x)                                            (g_pstHdmi21MacTxBandWidthParameter[x].b1HdmiInEnableDscEncoder = 0)

//--------------------------------------------------
// Macro of HDMI2.1 SCDC offset
//--------------------------------------------------
#define _LTP1                                                                  1
#define _LTP2                                                                  2
#define _LTP3                                                                  3
#define _LTP4                                                                  4
#define _LTP5                                                                  5
#define _LTP6                                                                  6
#define _LTP7                                                                  7
#define _LTP8                                                                  8

#define _HDMI21_SCDC_SLAVE_ADDR                                                0x54 //0xA8>>1
#define _HDMI21_SCDC_OFFSET_1                                                  0x1
#define _HDMI21_SCDC_OFFSET_2                                                  0x2
#define _HDMI21_SCDC_OFFSET_10                                                 0x10
#define _HDMI21_SCDC_OFFSET_30                                                 0x30
#define _HDMI21_SCDC_OFFSET_31                                                 0x31
#define _HDMI21_SCDC_OFFSET_35                                                 0x35
#define _HDMI21_SCDC_OFFSET_40                                                 0x40
#define _HDMI21_SCDC_OFFSET_41                                                 0x41
#define _HDMI21_SCDC_OFFSET_42                                                 0x42
#define _HDMI21_SCDC_OFFSET_50                                                 0x50
#define _HDMI21_SCDC_OFFSET_59                                                 0x59

/*
#define _HDMI21_FFE_VOLTAGE_SWING                                              400 // mV single end ,can't big than 500mV
#define _HDMI21_FFE_RES                                                        25 // mV single end

#define _HDMI21_TX_FFE0_C_N1                                                   54 // 0.054 *1000
#define _HDMI21_TX_FFE0_C                                                      865 // 0.865*1000
#define _HDMI21_TX_FFE0_C_P1                                                   81 // 0.081 *1000

#define _HDMI21_TX_FFE1_C_N1                                                   54 // 0.054 *1000
#define _HDMI21_TX_FFE1_C                                                      838 // 0.838*1000
#define _HDMI21_TX_FFE1_C_P1                                                   109 // 0.109 *1000

#define _HDMI21_TX_FFE2_C_N1                                                   54 // 0.054 *1000
#define _HDMI21_TX_FFE2_C                                                      811 // 0.811*1000
#define _HDMI21_TX_FFE2_C_P1                                                   135 // 0.135 *1000

#define _HDMI21_TX_FFE3_C_N1                                                   54 // 0.054 *1000
#define _HDMI21_TX_FFE3_C                                                      784 // 0.784*1000
#define _HDMI21_TX_FFE3_C_P1                                                   163 // 0.163 *1000
*/

#define _HDMI21_FFE_VOLTAGE_SWING                                              300 // mV single end ,can't big than 500mV
#define _HDMI21_FFE_RES                                                        25 // mV single end

#define _HDMI21_TX_FFE0_C_N1                                                   71 // 0.071 *1000
#define _HDMI21_TX_FFE0_C                                                      823 // 0.823*1000
#define _HDMI21_TX_FFE0_C_P1                                                   106 // 0.106 *1000

#define _HDMI21_TX_FFE1_C_N1                                                   71 // 0.071 *1000
#define _HDMI21_TX_FFE1_C                                                      788 // 0.788*1000
#define _HDMI21_TX_FFE1_C_P1                                                   142 // 0.142 *1000

#define _HDMI21_TX_FFE2_C_N1                                                   71 // 0.071 *1000
#define _HDMI21_TX_FFE2_C                                                      753 // 0.753*1000
#define _HDMI21_TX_FFE2_C_P1                                                   177 // 0.177 *1000

#define _HDMI21_TX_FFE3_C_N1                                                   71 // 0.071 *1000
#define _HDMI21_TX_FFE3_C                                                      717 // 0.717*1000
#define _HDMI21_TX_FFE3_C_P1                                                   213 // 0.213 *1000


#define _HDMI21_TX_MAIN_MAX                                                    ((_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0) * 50 / 2 + 800) // 0.01 mA
#define _HDMI21_TX_PRE_MAX                                                     ((_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0)  * 50 / 2)              // 0.01 mA
#define _HDMI21_TX_POST_MAX                                                    ((_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0)  * 50 / 2)             // 0.01 mA

//--------------------------------------------------
// Definitions of FFE Level
//--------------------------------------------------
#define _HDMI21_LANE0                                                          0
#define _HDMI21_LANE1                                                          1
#define _HDMI21_LANE2                                                          2
#define _HDMI21_LANE3                                                          3

//--------------------------------------------------
// Definitions bpp table7-35
//--------------------------------------------------
#define _HDMI21_BPP_DEFAULT                                                    192 // bpp =12
#define _HDMI21_DSC_DPF_SYNCWIDTH                                              3
#define _HDMI21_DSC_DPF_HFRONT                                                 11
#define _HDMI21_DSC_HBALNK_TOLERANCE                                           80
//--------------------------------------------------
// Definitions pps data one time sw lens
//--------------------------------------------------
#define _HDMI21_PPS_PACKET_LEN                                                 8
#define _HDMI21_PPS_PACKET_TOTAL_SIZE                                          128

//--------------------------------------------------
// Definitions of Data Flow Metering
//--------------------------------------------------
#define _HDMI21_TX_FRL_BORROW_MAX                                              400

// FOR ENCODER TEST
#define _DSC_HDMI21_FORCE_ENCODER                                              _OFF
#define _DSC_HDMI20_FORCE_DECODER                                              _OFF
#define _DSC_HDMI21_LINK_TRAINING_DOWN_SPEED                                   _OFF

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
typedef struct
{
    UINT8 b4HDMI21BWResult : 4;
    UINT8 b4HDMI21BWEvent : 4;
    UINT8 b2HDMI21LinkTrainingResult : 2;
    UINT8 b1HDMI21ReLinkTraing : 1;
    UINT8 b1HDMI21ForceLegacy : 1;
    UINT8 b1HDMI21VideoStart : 1;
#if(_AUDIO_SUPPORT == _ON)
    // Audio off
    UINT8 b1HdmiAudioOff : 1;
#endif

} StructHdmi21MacTxInfoType;

typedef enum
{
    _BW_RE_LINKTRAINING_DOWN = 0x00,
    _BW_RE_LINKTRAINING_UP,
    _BW_RE_DSC_ENCODER,
    _BW_RE_EDID,
    _BW_PASS,
    _BW_INITIAL,
} EnumLinkBWDecisionResultType;

typedef enum
{
    _LINK_TRAING_NONE = 0x00,
    _LINK_TRAING_LEGACY,
    _LINK_TRAING_PASS,
} EnumLinkTraingResultType;

typedef enum
{
    _INPUT_FROM_DP = 0x00,
    _INPUT_FROM_DECODER,
    _INPUT_FROM_ENCODER,
    _INPUT_FROM_DP_DSC_BYPASS,
} EnumHDMI21Input_From;

typedef struct
{
    UINT8 b4FrlRate : 4;
    UINT8 b4FirstFrlRate : 4;
    UINT8 b4FfeLevel : 4;

    UINT8 b4Lane0Ffe : 4;
    UINT8 b4Lane1Ffe : 4;
    UINT8 b4Lane2Ffe : 4;
    UINT8 b4Lane3Ffe : 4;

    UINT8 b4Lane0FfeMode : 4;
    UINT8 b4Lane1FfeMode : 4;
    UINT8 b4Lane2FfeMode : 4;
    UINT8 b4Lane3FfeMode : 4;

    UINT8 b4LtState : 4;
    UINT8 b1Hdmi21PhySet : 1;
    UINT8 b1FrlRateEvent : 1;
    UINT8 b1FltUpdateEvent : 1;
    UINT8 b1FrlStartEvent : 1;

    UINT8 b1Lane0Lock : 1;
    UINT8 b1Lane1Lock : 1;
    UINT8 b1Lane2Lock : 1;
    UINT8 b1Lane3Lock : 1;
    UINT8 b1LtRetry : 1;
    UINT8 b1ForceReduceFrlRate : 1;

    UINT8 b1PacketDetecting : 1;
    UINT8 b1AVIInfoFrameReceived : 1;
    UINT8 b1GCPReceived : 1;
    UINT8 b1FLTNoTimeOut : 1;
    UINT8 b1FLTDscFrlMax : 1;
    UINT8 b1FLTFrlMax : 1;
    UINT8 b1FirstSetphy : 1;
    UINT8 b1Lts3TimerStart : 1;
    UINT8 b1LtsPassPolling : 1;
    UINT8 ucLts3TimerCounter;
} StructHdmi21CtrlInfo;

// RX => TX link training parameter
typedef struct
{
    UINT8 b4L0LTPRequest : 4;
    UINT8 b4L1LTPRequest : 4;
    UINT8 b4L2LTPRequest : 4;
    UINT8 b4L3LTPRequest : 4;
    UINT8 b1LTPTimeoutflag : 1;
    UINT8 ucUpdateFlag10;
    UINT8 ucTestConfig35;
    UINT8 ucStatusFlag40;
    UINT8 ucStatusFlag41;
    UINT8 ucStatusFlag42;
} StructLTPParameter;

typedef enum
{
    _HDMI21_LT_NONE = 0x00,
    _HDMI21_LT_LTS1,
    _HDMI21_LT_LTS2,
    _HDMI21_LT_LTS3,
    _HDMI21_LT_LTS3_LFSR,
    _HDMI21_LT_LTS4,
    _HDMI21_LT_LTSP_GAP,
    _HDMI21_LT_LTSP_FRL_START,
    _HDMI21_LT_LTSL,
    _HDMI21_LT_LTSEND,
} EnumLinkTraingState;


// Bw parameter use
typedef enum
{
    _HDMI21_DATA_FLOW_METERING_NONE = 0x00,
    _HDMI21_DATA_FLOW_METERING_PASS_NO_BORROW,
    _HDMI21_DATA_FLOW_METERING_PASS_BORROW,
    _HDMI21_DATA_FLOW_METERING_OVER,
    _HDMI21_DATA_FLOW_METERING_VIDEO_FAIL,
    _HDMI21_DATA_FLOW_METERING_AUDIO_FAIL,
    _HDMI21_DATA_FLOW_METERING_DSC_SHALL_OFF,
} EnumHdmi21DataFlowMeteringResult;

typedef struct
{
    UINT8 b4NowHdmiLinkRate : 4;
    UINT8 b1HdmiInDscFormat : 1;
    UINT8 b1HdmiInEnableDscEncoder : 1;
    UINT8 b1HdmiInDscUse420 : 1;
    UINT8 b1HdmiInBorrow : 1;
    UINT8 b2HdmiInDscUseColorDepth : 2;
    UINT16 usHdmiInPixelClock;
    UINT16 usHdmiInHactive;
    UINT16 usHdmiInHblank;
    UINT8 b4HdmiInbpc : 4;
    UINT8 ucHdmiInAudioChannel;
    UINT8 ucHdmiInAudioRate;
    // Compress parameter
    UINT8 ucbpp;
    UINT8 b4Slices : 4;
} StructHdmi21BandwidthCheckParameter;


typedef enum
{
    _HDMI_2_1_NORMAL = 0,
    _HDMI_2_1_DE_EMPHASIS_ONLY = 1,
    _HDMI_2_1_PRE_SHOOT_ONLY = 2,
    _HDMI_2_1_NO_FFE = 3,
} EnumHDMI21FFEmode;

// FOR DSC DE & EN handler
typedef struct
{
    UINT8 b1TXtoHDMILinkOnStatus : 1;
    UINT8 b1TXInputInforOK : 1;
    UINT8 b1TXDecoderEnable : 1;
    UINT8 b1TXEncoderEnable : 1;
    UINT8 b4TXStreamState : 4;
    UINT8 b4TXDownStreamType : 4;
    UINT8 b4TXInputFrom : 4;
    UINT8 b4TXInputPixelMode : 4;
} StructDSCHDMITxInfoType;

typedef enum
{
    _DP_RX_SOURCE_JUDGE = 0x00,
    _DSC_DECODER_DE_COMPRESS,
    _DSC_ENCODER_COMPRESS,
    _TX_STREAM_READY,
} EnumDSCstreamStateType;

typedef struct
{
    UINT16 usHactive;
    UINT16 usVcative;
    UINT8 ucFramrate;
    UINT32 ulBpp_multi_16;
    UINT16 usHCactive;
    UINT16 usHCblank;
} StructDSCHDMICOMPRESSTable;

typedef struct
{
    UINT8 ucVic;
    UINT8 pucBYTET1[20];
} StructDSCHDMIEDIDDIDEXTTable;

typedef struct
{
    UINT16 usHCactive;               // Dsc dfg Horizontal Active Width (Tri byte)
    UINT16 usHCblank;                // Dsc dfg Horizontal blanking Width (Tri byte)
    UINT16 usHCfront;                // Dsc dfg Horizontal front porch Width (Tri byte)
    UINT16 usHCback;                 // Dsc dfg Horizontal back porch Width (Tri byte)
    UINT16 usHCStart;                // Dsc dfg Horizontal back porch Width (Tri byte)
} StructDSCHDMICompressTimingInfor;

typedef enum
{
    _HACTIVE = 0x00,
    _VACTIVE,
    _COMPRESS_FRAME_RATE,
    _BPP,
    _HCACTIVE,
    _HBLANK,
} EnumDCompressTableIndex;

typedef struct
{
    UINT8 ucVic;
    UINT8 pucTimingPattern[20];
} StructHdmi21EdidTimingReference;

typedef struct
{
    UINT8 b4DscVersionMajor : 4; // PPS0
    UINT8 b4DscVersionMinor : 4; // PPS0
    UINT8 ucPpsIdentifier; // PPS1
    UINT8 ucReservedPPS2; // PPS2
    UINT8 b4BitsPerComponent : 4; // PPS3
    UINT8 b4LinebufDepth : 4; // PPS3
    UINT8 b2ReservedPPS4 : 2; // PPS4
    UINT8 b1blockPredEnable : 1; // PPS4
    UINT8 b1ConvertRGB : 1; // PPS4
    UINT8 b1Simple422 : 1; // PPS4
    UINT8 b1VbrEnable : 1; // PPS4
    UINT8 b2BitsPerPixel_PPS4 : 2; // PPS4
    UINT8 ucBitsPerPixel_PPS5; // PPS5
    UINT16 usPicHeight; // PPS 6 7
    UINT16 usPicWidth; // PPS 8 9
    UINT16 usSliceHeight; // PPS 10 11
    UINT16 usSliceWidth; // PPS 12 13
    UINT16 usChunckSize; // PPS 14 15
    UINT8 b6ReservedPPS16 : 6; // PPS16
    UINT8 b2InitialXmitDelay_PPS16 : 2; // PPS16
    UINT8 ucInitialXmitDelay_PPS17; // PPS17
    UINT16 usIntialDecDelay; // PPS 18 19
    UINT8 ucReservedPPS20; // PPS20
    UINT8 b2ReservedPPS21 : 2; // PPS21
    UINT8 b6InitialScaleValue_PPS21 : 6; // PPS21
    UINT16 usScaleIncrementInterval; // PPS 22 23
    UINT8 b4ReservedPPS24 : 4; // PPS24
    UINT8 b4ScaleDecrementInterval_PPS24 : 4; // PPS24
    UINT8 ucScaleDecrementInterval_PPS25; // PPS25
    UINT8 ucReservedPPS26; // PPS26
    UINT8 b3ReservedPPS27 : 3; // PPS27
    UINT8 b5FirstLineBpgOffset_PPS27 : 5; // PPS27
    UINT16 usNflBpgOffset; // PPS 28 29
    UINT16 usSliceBpgOffset; // PPS 30 31
    UINT16 usInitialOffset; // PPS 32 33
    UINT16 usFinalOffset; // PPS 34 35
    UINT8 b3ReservedPPS36 : 3; // PPS36
    UINT8 b5FlatnessMinPQ_PPS36 : 5; // PPS36
    UINT8 b3ReservedPPS37 : 3; // PPS37
    UINT8 b5FlatnessMaxQP_PPS37 : 5; // PPS 37
    // PPS 38 ~ 87 RC PARAMETER SET
    UINT8 b6ReservedPPS88 : 6; // PPS88
    UINT8 b1Native420_PPS88 : 1; // PPS 88
    UINT8 b1Native422_PPS88 : 1; // PPS 88
    UINT8 b3ReservedPPS89 : 3; // PPS89
    UINT8 b5SecondLineBpgOffset_PPS89 : 5; // PPS 89
    UINT16 usNslBpgOffset; // PPS 90 91
    UINT16 usSecondLineOffsetAdj; // PPS 92 93
} StructDSCHDMIPPSData;

// FOR Data flow metering
typedef struct
{
    // input parameter
    // ===================================================
    UINT32 ulFreqPixelClock_kHz;
    // input H active
    UINT16 usHWidth;
    // input hblank
    UINT16 usHblank;

    // input color depth
    UINT8 ucbpc;
    // input color space
    UINT8 ucColorSpace;

    // Now use FRL rate
    UINT8 ucCurrentFrlRate;

    // Audio parameter
    UINT32 ulRateAudio_Hz;
    UINT8 ucAudioChannels; // register value + 1;
    UINT8 ucAudioCodeType;

    UINT8 b1DSCMode : 1; // VESA =0 , DSC = 1;

#if(_HDMI_TX_DSC_ENCODER_SUPPORT == _ON)
    UINT8 b4SliceNum : 4;
    UINT16 usSliceWidth;
    UINT16 usTargetBppx16;
#endif

    // Output parameter
    // ====================================================
    UINT8 ucCurrentFrlRateMaxAudioPacketNum;
    UINT8 ucMinFrlRateMaxAudioPacketNum;
    UINT8 ucCurrentFrlRateResult;
    UINT8 ucMinFrlRateResult;
    UINT8 ucMinFrlRate;
} StructDataFlowMeteringInfo;

//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************
extern StructHdmi21MacTxInfoType g_pstHdmi21MacTxInfo[_HDMI_21_TX_TOTAL_NUMBER];
extern StructHdmi21CtrlInfo g_pstHdmi21MacTxCtrlInfo[_HDMI_21_TX_TOTAL_NUMBER];
extern StructLTPParameter g_pstHdmi21MacTxLTP[_HDMI_21_TX_TOTAL_NUMBER];
extern StructHdmi21BandwidthCheckParameter g_pstHdmi21MacTxBandWidthParameter[_HDMI_21_TX_TOTAL_NUMBER];
extern StructDSCHDMITxInfoType g_pstHdmi21MacTxStreamInfo[_HDMI_21_TX_TOTAL_NUMBER];
extern StructHDMITxInputInfo g_pstHdmi21MacTxInputInfo[_HDMI_21_TX_TOTAL_NUMBER];
extern StructTimingInfo *g_pstHdmi21MacTxInputTimingInfo[_HDMI_21_TX_TOTAL_NUMBER];
extern StructStreamDisplayCompensateInfo *g_pstHdmi21MacTxDPFCompensateInfo[_HDMI_21_TX_TOTAL_NUMBER];
extern StructHDMITxAudioInfo g_pstHdmi21MacTxAudioInfo[_HDMI_21_TX_TOTAL_NUMBER];
#if(_DSC_SUPPORT == _ON)
extern StructTimingInfo g_pstHdmi21MacTxDscTimingInfo[_HDMI_21_TX_TOTAL_NUMBER];
#endif
extern StructDSCHDMICompressTimingInfor g_pstHdmi21MacTxHCTiminginfo[_HDMI_21_TX_TOTAL_NUMBER];

extern UINT8 bCtsLtpEnFlag; // flag for LTP_3, LTP_F test
extern UINT8 bCtsRetrainEnFlag; // link training retrain control

//#ifdef H5X_HDMITX_FIXME // [FIX-ME] need allocate in IMEM ?
extern StructDSCHDMICOMPRESSTable t3G_3LANE_444TABLE[];
extern StructDSCHDMICOMPRESSTable t3G_3LANE_422TABLE[];
extern StructDSCHDMICOMPRESSTable t6G_3LANE_444TABLE[];
extern StructDSCHDMICOMPRESSTable t6G_3LANE_422TABLE[];
extern StructDSCHDMICOMPRESSTable t6G_4LANE_444TABLE[];
extern StructDSCHDMICOMPRESSTable t6G_4LANE_422TABLE[];
extern StructDSCHDMICOMPRESSTable t8G_4LANE_444TABLE[];
extern StructDSCHDMICOMPRESSTable t10G_4LANE_444TABLE[];
extern StructDSCHDMICOMPRESSTable t12G_4LANE_422TABLE[];
extern StructDSCHDMICOMPRESSTable t12G_4LANE_444TABLE[];
#if(_DSC_SUPPORT == _ON)
// PPS packet data
extern UINT8 g_ppucHdmi21MacTxPPSData[_HDMI_21_TX_TOTAL_NUMBER][_HDMI21_PPS_PACKET_TOTAL_SIZE];
#endif
extern UINT8 tVIC_USE_FOR_DID_T1[35];
extern UINT8 tDEFAULT_TIMING_PATTERN_DTD[18];
extern UINT8 tDEFAULT_TIMING_PATTERN_T1[20];
extern StructHdmi21EdidTimingReference t3G_3LANE_EDIDTIMING[1];
extern StructHdmi21EdidTimingReference t3G_4LANE_EDIDTIMING[1];
extern StructHdmi21EdidTimingReference t6G_3LANE_EDIDTIMING[1];
extern StructHdmi21EdidTimingReference t6G_4LANE_EDIDTIMING[1];
extern StructHdmi21EdidTimingReference t8G_4LANE_EDIDTIMING[1];
extern StructHdmi21EdidTimingReference t10G_4LANE_EDIDTIMING[1];

extern UINT32 tHDMI21_TX_AUDIO_32K_MULTI_N_CTS[];
extern UINT32 tHDMI21_TX_AUDIO_44_1K_MULTI_N_CTS[];
extern UINT32 tHDMI21_TX_AUDIO_48K_MULTI_N_CTS[];
//#endif

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
extern UINT8 ScalerHdmi21MacTxPxTxMapping(EnumOutputPort enumOutputPort);
extern EnumOutputPort ScalerHdmi21MacTxPxMapping(UINT8 ucTxIndex);
extern void ScalerHdmi21MacTxSetLinkTrainingState(EnumOutputPort enumOutputPort, EnumLinkTraingState enumState);
extern void ScalerHdmi21MacTxSetReLinkTraining(EnumOutputPort enumOutputPort);
extern void ScalerHdmi21MacTxLinktraining(EnumOutputPort enumOutputPort);
extern void ScalerHdmi21MacTxLinktrainingReset(EnumOutputPort enumOutputPort);
extern BOOLEAN ScalerHdmi21MacTxLinktrainingPassCheck(EnumOutputPort enumOutputPort);
extern void ScalerHdmi21MacTxLinktrainingResetToLTS1(EnumOutputPort enumOutputPort);
extern UINT8 ScalerHdmi21MacTxBwDecision(EnumOutputPort enumOutputPort);
extern BOOLEAN ScalerHdmi21MacTxCheckDPCSRAM(EnumOutputPort enumOutputPort);
extern BOOLEAN ScalerHdmi21MacTxStreamHandler(EnumOutputPort enumOutputPort);
extern BOOLEAN ScalerHdmi21MacTxBwCheck(EnumOutputPort enumOutputPort);
#if(_AUDIO_SUPPORT == _ON)
extern UINT32 ScalerHdmi21MacTxGetAudioFormat(EnumOutputPort enumOutputPort, EnumAudioInfoType enumAudioInfoType);
#endif
extern void ScalerHdmi21MacTxPktFineTune(EnumOutputPort enumOutputPort);
extern void ScalerHdmi21MacTxGetStreamTimingInfo(EnumOutputPort enumOutputPort);
extern void ScalerHdmi21MacTxReset(EnumOutputPort enumOutputPort);
extern void ScalerHdmi21MacTxInitial(EnumOutputPort enumOutputPort);
#if(_DSC_ENCODER_SUPPORT == _ON)
extern UINT32 ScalerHdmi21MacTxGetLimitBpp(EnumOutputPort enumOutputPort, UINT16 usHactive, UINT16 usVactive, UINT8 ucFrame);
extern UINT32 ScalerHdmi21MacTxGetHCactive(EnumOutputPort enumOutputPort, UINT16 usHactive, UINT16 usVactive, UINT8 ucFrame);
extern UINT32 ScalerHdmi21MacTxGetHCblank(EnumOutputPort enumOutputPort, UINT16 usHactive, UINT16 usVactive, UINT8 ucFrame);
#endif

extern BOOLEAN ScalerHdmi21MacTxGetVideoStatus(EnumOutputPort enumOutputPort);
extern UINT8 ScalerHdmi21MacTxGetInputStreamFrom(EnumOutputPort enumOutputPort);
extern BOOLEAN ScalerHdmi21MacTxCheckUrgentLinkTraining(void);

#if(_HDMI21_HDCP2_TX_SUPPORT == _ON)
extern void ScalerHdmi21MacTxHdcpEncryptSignalProc(EnumOutputPort enumOutputPort, BOOLEAN bEnable);
extern void ScalerHdmi21MacTxSetHdcp2KsValue(EnumOutputPort enumOutputPort, UINT8 ucOffset, UINT8 ucValue);
extern void ScalerHdmi21MacTxSetHdcp2LcValue(EnumOutputPort enumOutputPort, UINT8 ucOffset, UINT8 ucValue);
extern void ScalerHdmi21MacTxSetHdcp2RivValue(EnumOutputPort enumOutputPort, UINT8 ucOffset, UINT8 ucValue);
extern void ScalerHdmi21MacTxResetHdcp2AesCipher(EnumOutputPort enumOutputPort);
extern BOOLEAN ScalerHdmi21MacTxGetHdcpEncStatus(EnumOutputPort enumOutputPort);
#endif

#endif // End of #if(_HDMI_TX_SUPPORT == _ON)

#endif // #ifndef __SCALER_HDMI21_MAC_TX_INCLUDE_H__
