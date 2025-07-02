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
// ID Code      : ScalerHdmiPhyTx.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>

#include "../include/ScalerFunctionInclude.h"
#include "../include/HdmiPhyTx/ScalerHdmiPhyTx.h"
#include "../include/HdmiMacTx/ScalerHdmiMacTxInclude.h"
#include "../include/ScalerTx.h"

#if(_HDMI_TX_SUPPORT == _ON)
//****************************************************************************
// CODE TABLES
//****************************************************************************


//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
#define ScalerHdmiPhyTx0SetIBHNTrim(x);
#define ScalerHdmiPhyTx0GetIBHNTrim() (0);
#define ScalerHdmiPhyTx0PowerOn();
#define ScalerHdmiPhyTx0CMUPowerOff();
#define ScalerHdmiPhyTx0SetCMUSignal(x);
//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************

//--------------------------------------------------
// Description  : ScalerHdmiPhyPxTxMapping
// Input Value  : None
// Output Value : None
//--------------------------------------------------
UINT8 ScalerHdmiPhyTxPxTxMapping(EnumOutputPort enumOutputPort)
{
#if 1 // H5X only have 1 TX port
    return _HW_P0_HDMI_PHY_TX_MAPPING;
#else
    UINT8 ucTxPortTemp = _TX_MAP_NONE;

    switch(enumOutputPort)
    {
#if(_P1_HDMI_SUPPORT == _ON)
        case _P1_OUTPUT_PORT:
            ucTxPortTemp = _HW_P1_HDMI_PHY_TX_MAPPING;
            break;
#endif

#if(_P2_HDMI_SUPPORT == _ON)
        case _P2_OUTPUT_PORT:
            ucTxPortTemp = _HW_P2_HDMI_PHY_TX_MAPPING;
            break;
#endif

#if(_P3_HDMI_SUPPORT == _ON)
        case _P3_OUTPUT_PORT:
            ucTxPortTemp = _HW_P3_HDMI_PHY_TX_MAPPING;
            break;
#endif

#if(_P4_HDMI_SUPPORT == _ON)
        case _P4_OUTPUT_PORT:
            ucTxPortTemp = _HW_P4_HDMI_PHY_TX_MAPPING;
            break;
#endif

        default:
            break;
    }
    return ucTxPortTemp;
#endif
}

//--------------------------------------------------
// Description  : ScalerHdmiPhyTxPxMapping
// Input Value  : TX index
// Output Value : P PORT OUT
//--------------------------------------------------
EnumOutputPort ScalerHdmiPhyTxPxMapping(UINT8 ucTxIndex)
{
        return _P0_OUTPUT_PORT;
}


//--------------------------------------------------
// Description  : HDMI Tx Phy Initial
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiPhyTxInitial(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiPhyTx0Initial();

            break;
#endif

        default:

            break;
    }
}

//--------------------------------------------------
// Description  : Hdmi Tx PHY Set
// Input Value  :
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmiPhyTxSet(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            return ScalerHdmiPhyTx0Set();

#endif

        default:

            break;
    }
    return _FALSE;
}




//--------------------------------------------------
// Description  : Hdmi Tx Enable Third PI Tracking
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiPhyTxEnable3rdTracking(EnumOutputPort enumOutputPort)
{
#if(HDMI_PHY_TX_ENABLE_3RD_TRACKING == _ON)
    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiPhyTx0Enable3rdTracking();

            break;
#endif

        default:

            break;
    }
#endif
}

//--------------------------------------------------
// Description  : Hdmi Tx Third PI Code Set
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiPhyTxThirdSetPICode(EnumOutputPort enumOutputPort)
{
#if(HDMI_PHY_TX_ENABLE_3RD_TRACKING == _ON)
    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiPhyTx0ThirdSetPICode();

            break;
#endif

        default:

            break;
    }
#endif
}

//--------------------------------------------------
// Description  : HDMI Tx PHY Timer Reset
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiPhyTxTimerReset(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiPhyTx0TimerReset();

            break;
#endif

        default:

            break;
    }
}

#if(_HDMI_PURE_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx CMU Power Off
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiPhyTxCMUPowerOff(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiPhyTx0CMUPowerOff();

            break;
#endif

// Linqian modify in 20181222 start
#if((_HDMI_PHY_TX2_SUPPORT == _ON) && (_HW_PHY_TX_TYPE == _HW_PHY_TX_GEN_2))
        case _TX2:
            ScalerGDIPhyHdmiTx2CMUPowerOff();

            break;
#endif
// Linqian modify in 20181222 end

        default:

            break;
    }
}

//--------------------------------------------------
// Description  : HDMI Tx PHY Power on
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiPhyTxPowerOn(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiPhyTx0PowerOn();

            break;
#endif

// Linqian modify in 20181222 start
#if((_HDMI_PHY_TX2_SUPPORT == _ON) && (_HW_PHY_TX_TYPE == _HW_PHY_TX_GEN_2))
        case _TX2:
            ScalerGDIPhyHdmiTx2PowerOn();

            break;
#endif
// Linqian modify in 20181222 end

        default:

            break;
    }
}

//--------------------------------------------------
// Description  : HDMI Tx PHY Power off
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiPhyTxPowerOff(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiPhyTx0PowerOff();

            break;
#endif

// Linqian modify in 20181222 start
#if((_HDMI_PHY_TX2_SUPPORT == _ON) && (_HW_PHY_TX_TYPE == _HW_PHY_TX_GEN_2))
        case _TX2:

            ScalerGDIPhyHdmiTx2PowerOff();

            break;
#endif
// Linqian modify in 20181222 end

        default:

            break;
    }
}


//--------------------------------------------------
// Description  : Hdmi Tx Set Z0 detection circuit
// Input Value  : _ENABLE or _DISABLE
// Output Value :
//--------------------------------------------------
void ScalerHdmiPhyTxSetZ0Detect(EnumOutputPort enumOutputPort, BOOLEAN bEnable)
{
    //bEnable = bEnable;

    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiPhyTx0SetZ0Detect(bEnable);

            break;
#endif

// Linqian modify in 20181223 start
#if((_HDMI_PHY_TX2_SUPPORT == _ON) && (_HW_PHY_TX_TYPE == _HW_PHY_TX_GEN_2))
        case _TX2:

            ScalerGDIPhyHdmiTx2SetZ0Detect(bEnable);

            break;
#endif
// Linqian modify in 20181223 end

        default:

            break;
    }
}

//--------------------------------------------------
// Description  : Hdmi Tx Z0 detection
// Input Value  :
// Output Value : EnumHDMITxZ0Detect
//--------------------------------------------------
EnumHDMITxZ0Detect ScalerHdmiPhyTxZ0Detect(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            return ScalerHdmiPhyTx0Z0Detect();

            break;
#endif

// Linqian modify in 20181223 start
#if((_HDMI_PHY_TX2_SUPPORT == _ON) && (_HW_PHY_TX_TYPE == _HW_PHY_TX_GEN_2))
        case _TX2:

            return ScalerGDIPhyHdmiTx2Z0Detect();

            break;
#endif
// Linqian modify in 20181223 end

        default:
            break;
    }
    return _HDMI_TX_Z0_ALL_LOW;
}

//--------------------------------------------------
// Description  : Enable HDMI TX CMU Signal
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiPhyTxSetCMUSignal(EnumOutputPort enumOutputPort, BOOLEAN bEnable)
{
    //bEnable = bEnable;

    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiPhyTx0SetCMUSignal(bEnable);

            break;
#endif

// Linqian modify in 20181223 start
#if((_HDMI_PHY_TX2_SUPPORT == _ON) && (_HW_PHY_TX_TYPE == _HW_PHY_TX_GEN_2))
        case _TX2:

            ScalerGDIPhyHdmiTx2SetCMUSignal(bEnable);

            break;
#endif
// Linqian modify in 20181223 end

        default:

            break;
    }
}

//--------------------------------------------------
// Description  : Set HDMI TX IBHN Triming
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiPhyTxSetIBHNTrim(EnumOutputPort enumOutputPort, UINT8 ucIBHNTRIMCurrent)
{
    //ucIBHNTRIMCurrent = ucIBHNTRIMCurrent;
    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiPhyTx0SetIBHNTrim(ucIBHNTRIMCurrent);

            break;
#endif

        default:

            break;
    }
}

//--------------------------------------------------
// Description  : Get HDMI TX IBHN Triming
// Input Value  :
// Output Value :
//--------------------------------------------------
UINT8 ScalerHdmiPhyTxGetIBHNTrim(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_PHY_TX0_SUPPORT == _ON)
        case _TX0:

            return ScalerHdmiPhyTx0GetIBHNTrim();

            break;
#endif

        default:

            break;
    }
    return 0x00;
}




#if(_HDMI_DP_PLUS_SUPPORT == _ON)
//--------------------------------------------------
// Description  : ScalerHdmiPhyLanesSet
// Input Value  : _ENABLE or _DISABLE
// Output Value : None
//--------------------------------------------------
void ScalerHdmiPhyTxLanesSet(EnumOutputPort enumOutputPort, BOOLEAN bEnable)
{
    bEnable = bEnable;

    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
        default:

            break;
    }
}

//--------------------------------------------------
// Description  : Reset DP++
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiPhyTxDpppReset(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiPhyTxPxTxMapping(enumOutputPort))
    {
        default:

            break;
    }
}
#endif // #if(_HDMI_DP_PLUS_SUPPORT == _ON)
#endif // #if(_HDMI_PURE_SUPPORT == _ON)
#endif // #if(_HDMI_TX_SUPPORT == _ON)

