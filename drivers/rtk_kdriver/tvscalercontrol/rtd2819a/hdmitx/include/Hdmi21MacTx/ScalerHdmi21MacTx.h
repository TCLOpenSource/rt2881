#ifndef __SCALER_HDMI21_MAC_TX_H__
#define __SCALER_HDMI21_MAC_TX_H__

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
// ID Code      : ScalerHdmi21MacTx.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include "ScalerHdmi21MacTxInclude.h"

#if(_HDMI21_TX_SUPPORT == _ON)
//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#define COMP_TABLE_INDEX_MAX    5

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************


//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************


//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
extern UINT32 ScalerHdmi21MacTxGetCompressTableIndex(EnumOutputPort enumOutputPort, EnumDCompressTableIndex enumIndex, UINT8 ucNum);
#endif // End of #if(_HDMI_TX_SUPPORT == _ON)

#endif // #ifndef __SCALER_HDMI21_MAC_TX_H__
