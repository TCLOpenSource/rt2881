#ifndef __SCALER_HDMI_MAC_TX0_FILL_PACKET_H__
#define __SCALER_HDMI_MAC_TX0_FILL_PACKET_H__

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

//----------------------------------------------------------------------------------------------------
// ID Code      : ScalerTx.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include "ScalerFunctionInclude.h"
#include "ScalerTx.h"
//****************************************************************************
// NUM DEFINITTIONS
//****************************************************************************




//****************************************************************************
// STRUCT  DEFINITTIONS
//****************************************************************************



//****************************************************************************
// MACRO/DEFINITION
//****************************************************************************


//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************



//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************



//****************************************************************************
// FUNCTION define
//****************************************************************************
UINT8 ScalerTimerPollingFlagProc_bankIF(StructEventPollingInfo *eventInfo);
UINT8 ScalerHdmiMacTxAspectRatioCal(StructTimingInfo *pstInputInfo);
void ScalerHdmiMacTxSearchVicTable(StructHdmiTxSearchVicTableInfo *pStHdmiTxSearchVicTableInfo);
BOOLEAN ScalerHdmiMacTx0PeriodicSRAMPacking(void);

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
UINT8 ScalerHdmiTxRepeaterGetSourceInfo_DvVsifInfo(UINT8 *dvVsifInfo);
UINT8 ScalerHdmiTxRepeaterGetSourceInfo_empVtem(UINT8 *empVtem);
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT

#endif // #ifndef __SCALER_HDMI_MAC_TX0_FILL_PACKET_H__
