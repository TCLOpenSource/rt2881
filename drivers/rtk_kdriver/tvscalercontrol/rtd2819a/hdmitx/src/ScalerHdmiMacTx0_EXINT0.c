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
// ID Code      : ScalerHdmiMacTx0_EXINT0.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------


#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>

#ifdef NOT_USED_NOW

#include "../include/ScalerFunctionInclude.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0.h"
#include "../include/ScalerTx.h"

#if(_HDMI_MAC_TX0_SUPPORT == _ON)
//****************************************************************************
// CODE TABLES
//****************************************************************************


//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************


//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************

//--------------------------------------------------
// Description  : HDMI Tx PHY Power off
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0IntHandle_EXINT0(void)
{
#if(_HDMI_MAC_TX0_PURE == _ON)
    // HPD Falling INT
    if((rtd_getbits(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL1_reg, _BIT0) == _BIT0) && (rtd_getbits(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL1_reg, _BIT1) == _BIT1))
    {
        rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL1_reg, ~(_BIT1 | _BIT0), 0x00);

        // Enable WD 10ms timer
        ScalerTimerWDActivateTimerEvent_EXINT0(_HPD_FALLING_TIME, _SCALER_WD_TIMER_EVENT_HDMI_TX0_HPD_FALLING_DEBOUNCE);
    }

    // HPD Rising INT
    if((rtd_getbits(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL0_reg, _BIT0) == _BIT0) && (rtd_getbits(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL0_reg, _BIT1) == _BIT1))
    {
        rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL0_reg, ~(_BIT1 | _BIT0), 0x00);

        SET_HDMI_MAC_TX0_HPD_TOGGLE_DETECTED_FROM_IRQ();
    }
#endif
}

#endif // End of #if(_HDMI_MAC_TX0_SUPPORT == _ON)
#endif // #ifdef NOT_USED_NOW

