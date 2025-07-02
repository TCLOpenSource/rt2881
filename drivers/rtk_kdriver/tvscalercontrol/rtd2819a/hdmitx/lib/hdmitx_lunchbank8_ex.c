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
// ID Code      : hdmitx_lunchbank8_ex.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>

UINT8 lib_hdmitx_is_hdmi_bypass_support(void)
{
#ifdef HDMITX_SW_CONFIG
    if (Get_HDMI_PATH_SELECTION() == _hdmi_path_select_thru_SRNN) {
        return 0;
    }
#endif
    return 1;
}

#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx HDR Set
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0HDRSet(void)
{
    INT32 stc;
    UINT8 ucStreamIndex = _ST_MAP_NONE;
    stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
    SLR_VOINFO *pVOInfoNew = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());

    ucStreamIndex = ScalerDpStreamPxSTMapping(ScalerHdmiMacTxPxMapping(_TX0));

    // update HDMITX quantization range info
    if(ScalerHdmiTxGetUpdateVoInfoFlag() && pVOInfoNew)
        DebugMessageHDMITx("[HDMI_TX] Got video_full_range_flag=%d\n", (UINT32)pVOInfoNew->video_full_range_flag);

    // check HDR Packet Change
    if(GET_STREAM_HDR_INFO_FRAME_RECEIVED() == _TRUE)
    {
        if(((UINT32)(stc - lastDumpStc_HDRSet)) > 450000){ // dump period=1sec
            DebugMessageHDMITx("[HDMI_TX21]HDR active\n");
            lastDumpStc_HDRSet = stc;
        }
        if(GET_STREAM_HDR_INFO_FRAME_CHANGE() == _TRUE || ((ScalerHdmiTxGetUpdateVoInfoFlag() || (ScalerHdmiTxGetHDRFlag()==0)) && (pVOInfoNew !=NULL)))
        {
            DebugMessageHDMITx("[HDMI_TX]HDR Packet Changed\n");

            CLR_DP_STREAM_HDR_INFO_FRAME_CHANGE(ucStreamIndex);

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)
            // disable hdr pkt
            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~_BIT5, 0x00);
#endif
            rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT6, 0x00);

            // Packet3: HDR Packet
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT3_ADDRESS, _HDMI_TX_STATIC_HDR_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                FatalMessageHDMITx("[HDMI_TX] DRM info Packet send fail in HDR\n");
                return;
            }
            else
            {
                // enable hdr pkt
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~_BIT5, _BIT5);
            }


            NoteMessageHDMI21Tx("[HDMI_TX21]colour_primaries = %d, matrix_coefficiets=%d\n", (UINT32)pHDMITX_VOInfo->colour_primaries, (UINT32)pHDMITX_VOInfo->matrix_coefficiets);
            if( pHDMITX_VOInfo->colour_primaries == 9 || pHDMITX_VOInfo->matrix_coefficiets == 9) //refer to fwif_color_VIP_get_VO_Info() in scalerColor.c
            {
                Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_COLOR_IMETRY, _COLORIMETRY_EXT);
                Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_EXT_COLORIMETRY, _COLORIMETRY_EXT_YCC_ITUR_BT2020_NCL);
            }
            else if(pHDMITX_VOInfo->matrix_coefficiets == 10)
            {
                Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_COLOR_IMETRY, _COLORIMETRY_EXT);
                Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_EXT_COLORIMETRY, _COLORIMETRY_EXT_YCC_ITUR_BT2020_CL);
            }

            if(ScalerHdmiTxGetUpdateVoInfoFlag() && pVOInfoNew){
                Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_YCC_QUANTIZATION_RANGE, pVOInfoNew->video_full_range_flag);
                ScalerHdmiTxSetUpdateVoInfo(_DISABLE);
            }

            SET_HDMI_MAC_TX0_INPUT_COLORIMETRY( ScalerDpStreamGetElement(_P0_OUTPUT_PORT, _COLORIMETRY));
            // Set EXT_Colorimetry
            SET_HDMI_MAC_TX0_INPUT_EXT_COLORIMETRY( ScalerDpStreamGetElement(_P0_OUTPUT_PORT, _EXT_COLORIMETRY));

            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(_BIT2),0x00);

            // Packet0: AVI Packet
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT0_ADDRESS, _HDMI_TX_AVI_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                FatalMessageHDMITx("[HDMI_TX]AVI info Packet send fail in HDR\n");
                return;
            }
            else
            {
                // enable avi pkt
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(_BIT2),_BIT2);
            }
            rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT6, _BIT6);

            ScalerHdmiTxSetHDRFlag(TRUE);

        }


    }
    else
    {
        if(ScalerHdmiTxGetHDRFlag()|| (ScalerHdmiTxGetUpdateVoInfoFlag() && pVOInfoNew))
        {
            Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_COLOR_IMETRY, _COLORIMETRY_RGB_SRGB);
            Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_EXT_COLORIMETRY, _COLORIMETRY_EXT_RGB_SRGB);
            if(ScalerHdmiTxGetUpdateVoInfoFlag() && pVOInfoNew){
                Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_YCC_QUANTIZATION_RANGE, pVOInfoNew->video_full_range_flag);
                ScalerHdmiTxSetUpdateVoInfo(_DISABLE);
            }

            SET_HDMI_MAC_TX0_INPUT_COLORIMETRY(ScalerDpStreamGetElement(_P0_OUTPUT_PORT, _COLORIMETRY));
            // Set EXT_Colorimetry
            SET_HDMI_MAC_TX0_INPUT_EXT_COLORIMETRY(ScalerDpStreamGetElement(_P0_OUTPUT_PORT, _EXT_COLORIMETRY));

            rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT6, 0);

        	// Set DRM InfoFrame Packet to SDR mode
            if(ScalerHdmiTxGetHDRFlag())
            {
#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)
                // disable hdr pkt
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~_BIT5, 0x00);
#endif

                if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT3_ADDRESS, _HDMI_TX_STATIC_HDR_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE){
                    FatalMessageHDMITx("[HDMI_TX] DRM info Packet send fail in HDR@ScalerHdmiMacTx0HDRSet()\n");
                }else{
                    // enable hdr pkt
                    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~_BIT5, _BIT5);
                    InfoMessageHDMITx("[HDMI_TX] Change DRM info to SDR\n");
                }
            }

            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(_BIT2),0x00);
            // Packet0: AVI Packet
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT0_ADDRESS, _HDMI_TX_AVI_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                FatalMessageHDMITx("[HDMI_TX]AVI info Packet send fail in HDR\n");
                return;
            }
            else
            {
                // enable avi pkt
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(_BIT2),_BIT2);
            }

            rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT6, _BIT6);

            ScalerHdmiTxSetHDRFlag(FALSE);
        }
        pHDMITX_VOInfo = NULL;
    }
}
#endif // #if(_HDMI_HDR10_TX0_SUPPORT == _ON)



#if(_HDMI_FREESYNC_TX0_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Freesync Set
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0FreesyncSet(void)
{
    UINT8 ucStreamIndex = _ST_MAP_NONE;

    ucStreamIndex = ScalerDpStreamPxSTMapping(ScalerHdmiMacTxPxMapping(_TX0));

    // check Freesync Packet Change
    if(GET_DP_STREAM_AMD_SPD_INFO_FRAME_RECEIVED(ucStreamIndex) == _TRUE)
    {
        if(GET_DP_STREAM_AMD_SPD_INFO_FRAME_CHANGE(ucStreamIndex) == _TRUE)
        {
            DebugMessageHDMITx("[HDMI_TX] Freesync Packet Changed\n");

            CLR_DP_STREAM_AMD_SPD_INFO_FRAME_CHANGE(ucStreamIndex);

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)
            // disable freesync pkt
            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~_BIT6, 0x00);
#endif

            // Packet5: Freesync Packet
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT5_ADDRESS, _HDMI_TX_FREESYNC_INFOFRAME, _HDMI_TX_SW_PKT_DFF1) == _FALSE)
            {
                return;
            }
        }

        // enable freesync pkt
        rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~_BIT6, _BIT6);
    }
    else
    {
        // disable freesync pkt
        rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~_BIT6, 0x00);
    }
}
#endif // #if(_HDMI_FREESYNC_TX0_SUPPORT == _ON)

