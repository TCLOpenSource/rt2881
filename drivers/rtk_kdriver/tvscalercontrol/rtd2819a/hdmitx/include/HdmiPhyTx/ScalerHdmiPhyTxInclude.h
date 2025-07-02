#ifndef __SCALER_HDMI_PHY_TX_INCLUDE_H__
#define __SCALER_HDMI_PHY_TX_INCLUDE_H__

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
// ID Code      : ScalerHdmiPhyTxInclude.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include "../ScalerFunctionInclude.h"
#include "../HdmiMacTx/ScalerHdmiMacTxInclude.h"

#if(_HDMI_TX_SUPPORT == _ON)
//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
//--------------------------------------------------
// Macros of HDMI Tx Third Tracking
//--------------------------------------------------


//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************

//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************

extern EnumOutputPort ScalerHdmiPhyTxPxMapping(UINT8 ucTxIndex);
extern UINT8 ScalerHdmiPhyTxPxTxMapping(EnumOutputPort enumOutputPort);
extern void ScalerHdmiPhyTxInitial(EnumOutputPort enumOutputPort);
extern void ScalerHdmiPhyTxEnable3rdTracking(EnumOutputPort enumOutputPort);
extern void ScalerHdmiPhyTxThirdSetPICode(EnumOutputPort enumOutputPort);
extern void ScalerHdmiPhyTxTimerReset(EnumOutputPort enumOutputPort);

#if(_HDMI_PURE_SUPPORT == _ON)
extern void ScalerHdmiPhyTxCMUPowerOff(EnumOutputPort enumOutputPort);
extern void ScalerHdmiPhyTxPowerOn(EnumOutputPort enumOutputPort);
extern void ScalerHdmiPhyTxPowerOff(EnumOutputPort enumOutputPort);
extern void ScalerHdmiPhyTxSetZ0Detect(EnumOutputPort enumOutputPort, BOOLEAN bEnable);
extern void ScalerHdmiPhyTxSetCMUSignal(EnumOutputPort enumOutputPort, BOOLEAN bEnable);
extern void ScalerHdmiPhyTxSetIBHNTrim(EnumOutputPort enumOutputPort, UINT8 ucIBHNTRIMCurrent);
#endif
#if(_HDMI_DP_PLUS_SUPPORT == _ON)
extern void ScalerHdmiPhyTxLanesSet(EnumOutputPort enumOutputPort, BOOLEAN bEnable);
extern void ScalerHdmiPhyTxDpppReset(EnumOutputPort enumOutputPort);
#endif
#endif // End of #if(_HDMI_MAC_TX0_SUPPORT == _ON)

#endif // #ifndef __SCALER_HDMI_PHY_TX_INCLUDE_H__
