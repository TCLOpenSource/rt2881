#ifndef __SCALER_HDMI_MAC_TX_H__
#define __SCALER_HDMI_MAC_TX_H__

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
// ID Code      : ScalerHdmiMacTx.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include "../ScalerFunctionInclude.h"
#include "../HdmiMacTx/ScalerHdmiMacTxInclude.h"

#if(_HDMI_TX_SUPPORT == _ON)
//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************


//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
typedef struct {
    UINT8 hs_st_adj;
    UINT8 hs_end_adj;
    UINT8 de_st_adj;
    UINT8 de_end_adj;
    UINT8 fix_vs_center_adj;
} hdmi_tx_fixed_htotal_retiming_adj_st;

//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************
extern UINT8 g_pucHdcp14TxAksvBackup[_HDMI_TX_HDCP14_KEY_SIZE];

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
extern void ScalerHdmiMacTxSetDidPreferTimingAddr(EnumOutputPort enumOutputPort, UINT16 usAddress);

void ScalerHdmiTx0_Mac_asyncFifoEnable(UINT8 enable);
void ScalerHdmiTx0_Mac_interruptEnable(UINT8 enable);
void ScalerHdmiTx0_MacConfig(UINT8 hdmiMode);
void ScalerHdmiTx0_LanePNSwap(void);
void ScalerHdmiTx0_LaneSrcSel_config(UINT8 value);
void ScalerHdmiTx0_LanePnSwap_config(UINT8 value);
void ScalerHdmiTx0_LaneSrcSel(void);
void ScalerHdmiTx0_MacConfig_LaneSet(UINT8 hdmiMode);

#endif // End of #if(_HDMI_TX_SUPPORT == _ON)

#endif // #ifndef __SCALER_HDMI_MAC_TX_H__
