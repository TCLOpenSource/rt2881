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
// ID Code      : hdmitx_lib_ex.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h> 
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>

extern char ScalerHdmiTxRepeaterSetBypassPktHdrEn(EnumHdmiRepeaterBypassPktHdrType bypassPktHdrType, UINT8 enable);

UINT8 lib_hdmitx_is_HDR10_PLUS_Function_support(void)
{
#ifdef HDMITX_SW_CONFIG
    FatalMessageHDMITx("[HDMITX]10p=%d\n",(UINT32)GetHDR10Plus_Enable());
    return GetHDR10Plus_Enable();
#endif
    return 1;
}
UINT8 lib_hdmitx_is_HDR10_Function_support(void)
{
#ifdef HDMITX_SW_CONFIG
    FatalMessageHDMITx("[HDMITX]10=%d\n",(UINT32)GetHDR10_Enable());
    return GetHDR10_Enable();
#endif
    return 1;
}
void lib_hdmitx_packet_config_init(void)
{
    UINT32 value_u32 = 0;
    // TX enable bypass infoframe packet
    value_u32 = (TX_BYPASS_PKT_HD_AUDIO_INF| TX_BYPASS_PKT_HD_GMP| TX_BYPASS_PKT_HD_RES| TX_BYPASS_PKT_HD_SPD| TX_BYPASS_PKT_HD_AUDIO_HBR| TX_BYPASS_PKT_HD_AUDIO| TX_BYPASS_PKT_HD_EMP| TX_BYPASS_PKT_HD_VSIF);
    // [DANBU-718] set VSIF packet enable config: transfer vsif oui info frame packet(s) which is (are) set to enable
    value_u32 |= (TX_BYPASS_PKT_HD_OUI_CUNTOMER| TX_BYPASS_PKT_HD_OUI_H14B| TX_BYPASS_PKT_HD_OUI_HF|TX_BYPASS_PKT_HD_OUI_DOLBY);
    if(lib_hdmitx_is_HDR10_PLUS_Function_support())
        value_u32 |= (TX_BYPASS_PKT_HD_OUI_HDR10P);
    if(lib_hdmitx_is_HDR10_Function_support())
        value_u32 |= (TX_BYPASS_PKT_HD_DRAM);
    ScalerHdmiTxRepeaterSetBypassPktHdrEn((UINT32)value_u32, _ENABLE);


}

