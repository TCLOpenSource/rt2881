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
// ID Code      : hdmitx_lunchbank9_ex.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include "../include/ScalerTx.h"
#include "../include/HdmiEdid/hdmitx_edid.h"
hdmi_tx_max_pll_st Max_Pll_Table[_HDMI21_FRL_MAX_NUM] = {
    {_HDMI21_FRL_NONE, HDMITX_FRL_NONE_MAX_PLL},
    {_HDMI21_FRL_3G, HDMITX_FRL_3G_MAX_PLL},
    {_HDMI21_FRL_6G_3LANES, HDMITX_FRL_6G3L_MAX_PLL},
    {_HDMI21_FRL_6G_4LANES, HDMITX_FRL_6G4L_MAX_PLL},
    {_HDMI21_FRL_8G, HDMITX_FRL_8G_MAX_PLL},
    {_HDMI21_FRL_10G, HDMITX_FRL_10G_MAX_PLL},
    {_HDMI21_FRL_12G, HDMITX_FRL_12G_MAX_PLL},
};
hdmi_timing_check_type golden_timing[] = {{3840, 160, 112, 32, 2160, 60, 50, 5, 0x01F37F}};

extern UINT16 calcDTDTimingvFreq(void);
extern UINT8 ScalerHdmiMacTx0EdidGetFeature(EnumEdidFeatureType enumEdidFeature);
#ifdef HDMITX_SW_CONFIG
extern UINT8 industrial_ctrl_enable(void);
#endif
extern hdmi_timing_check_type  edid_timing;

extern UINT8 *pucDdcRamAddr;
extern UINT8 g_edid_policy;
UINT8 lib_hdmitx_industrial_ctrl_enable(void)
{
#ifdef HDMITX_SW_CONFIG
    return industrial_ctrl_enable();
#endif
    return 0;
}
UINT8 lib_hdmitx_is_hdmi_21_support(void)
{
#ifdef HDMITX_SW_CONFIG
    if (GetHDMITX_Ver() < _HDMITX_VER_2_1) {
        return 0;
    }
#endif
    return 1;
}
UINT8 lib_hdmitx_is_hdmi_limit_support(void)
{
#ifdef HDMITX_SW_CONFIG
    if (GetHDMITX_Ver() == _HDMITX_VER_2_1_LIMITED) {
        return 1;
    }
#endif
    return 0;
}
UINT16 getLimitPixelClkWithDDR(void)
{
	UINT16 max_pll;
	max_pll = ((lib_hdmitx_is_hdmi_21_support())? TX_OUT_PIXEL_CLOCK_MAX_W_DDR: TX_OUT_PIXEL_CLOCK_MAX_WO_DDR);
	if(Max_Pll_Table[ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE)].maxPll > max_pll)
		return max_pll;
	else
		return Max_Pll_Table[ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE)].maxPll;
}
UINT8 lib_hdmitx_is_hdmi_422_support(void)
{
#ifdef HDMITX_SW_CONFIG
    if (Get_Chip_version() == Version_A) {
        return 0;
    }
#endif
    return 1;
}


UINT8 lib_hdmitx_is_VRR_Function_support(void)
{
#ifdef HDMITX_SW_CONFIG
    return Get_VRR_Function_Enable();
#else
    return 1;
#endif
}

#ifdef HDMITX_SW_CONFIG
EnumRX_EDID_Policy lib_hdmitx_get_edidPolicy_support(void)
{
    return g_edid_policy;
}
#endif

UINT8 lib_hdmitx_get_safemode_support(void)
{
#ifdef HDMITX_SW_CONFIG
    return GetOutputSafeMode();
#else
    return 0;
#endif
}

/* sw feature -Golden_Timing */
UINT8 lib_hdmitx_is_Golden_Timing_support(void)
{
#ifdef HDMITX_SW_CONFIG
    return Get_Golden_Timing_Enable();
#else
    return 1;
#endif
}
void modifyDTDTimingOver1360M_forDisplayID(UINT8 timing_num,UINT16 ucDTDAddress)
{
    UINT16 u16TmpValue = 0;
    UINT16 u16TmpValue2 = 0;
    edid_timing.h_blank = golden_timing[timing_num].h_blank;
    edid_timing.h_front = golden_timing[timing_num].h_front;
    edid_timing.h_sync = golden_timing[timing_num].h_sync;

    // H_Blank
    u16TmpValue = edid_timing.h_blank - 1;
    pucDdcRamAddr[ucDTDAddress + 7] = (u16TmpValue & 0xFF00) >> 8;
    pucDdcRamAddr[ucDTDAddress + 6] = u16TmpValue & 0xFF;

    // H_Front
    u16TmpValue = edid_timing.h_front - 1;
    pucDdcRamAddr[ucDTDAddress + 8] = u16TmpValue & 0xFF;

    u16TmpValue2 = pucDdcRamAddr[ucDTDAddress + 9] & 0x80;
    u16TmpValue = (u16TmpValue & 0xFF00) >> 8;
    u16TmpValue = u16TmpValue | u16TmpValue2;
    pucDdcRamAddr[ucDTDAddress + 9] = u16TmpValue & 0xFF;

    // H_Sync
    u16TmpValue = edid_timing.h_sync - 1;
    pucDdcRamAddr[ucDTDAddress + 11] = (u16TmpValue & 0xFF00) >> 8;
    pucDdcRamAddr[ucDTDAddress + 10] = u16TmpValue & 0xFF;

    edid_timing.v_blank = golden_timing[timing_num].v_blank;
    edid_timing.v_front = golden_timing[timing_num].v_front;
    edid_timing.v_sync = golden_timing[timing_num].v_sync;

    // V_Blank
    u16TmpValue = edid_timing.v_blank - 1;
    pucDdcRamAddr[ucDTDAddress + 15] = (u16TmpValue & 0xFF00) >> 8;
    pucDdcRamAddr[ucDTDAddress + 14] = u16TmpValue & 0xFF;

    // V_Front
    u16TmpValue = edid_timing.v_front - 1;
    pucDdcRamAddr[ucDTDAddress + 16] = u16TmpValue & 0xFF;

    u16TmpValue2 = pucDdcRamAddr[ucDTDAddress + 17] & 0x80;
    u16TmpValue = (u16TmpValue & 0xFF00) >> 8;
    u16TmpValue = u16TmpValue | u16TmpValue2;
    pucDdcRamAddr[ucDTDAddress + 17] = u16TmpValue & 0xFF;

    // V_Sync
    u16TmpValue = edid_timing.v_sync - 1;
    pucDdcRamAddr[ucDTDAddress + 19] = (u16TmpValue & 0xFF00) >> 8;
    pucDdcRamAddr[ucDTDAddress + 18] = u16TmpValue & 0xFF;

    // Pixel Clock, 1278.72Mhz, 0x01F37F=127871
    edid_timing.pixelClk = golden_timing[timing_num].pixelClk;
    pucDdcRamAddr[ucDTDAddress + 2] = (edid_timing.pixelClk >> 16) & 0xFF;
    pucDdcRamAddr[ucDTDAddress + 1] = (edid_timing.pixelClk >> 8) & 0xFF;;
    pucDdcRamAddr[ucDTDAddress] = edid_timing.pixelClk & 0xFF;
}
UINT8 lib_hdmitx_Golden_Timing_adjust(UINT16 ucDTDAddress)
{
    UINT16 dtd_vfeq = 0;
    if (lib_hdmitx_is_Golden_Timing_support()) {
        dtd_vfeq = calcDTDTimingvFreq();
        if((edid_timing.h_act == 3840) && (edid_timing.v_act == 2160) && (dtd_vfeq > 1430) && (dtd_vfeq < 1450) &&
            (edid_timing.pixelClk / 10 >= EDID_SUPPORT_NON_DSC_PIXELCLOCK)) {
            modifyDTDTimingOver1360M_forDisplayID(0,ucDTDAddress);
            return 1;
        }
    }
    return 0;
}
