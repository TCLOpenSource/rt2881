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
// ID Code      : hdmitx_lunchbank38_ex.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h> 
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h> 
#include <rbus/hdmitx21_packet_control_reg.h>
#include "../include/Hdmi21MacTx/ScalerHdmi21MacTxInclude.h"
extern UINT8 HDMITX_DTG_Wait_Den_Stop_Done(void);
extern UINT8 ScalerHdmiTx_Wait_VerticalDataEnable(void);

//--------------------------------------------------
// Description  : HDMI2.1a Tx SBTM Enable setting
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmi21MacTx0FillSBTMPacket(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    #define EMP_INFO_LEN 28
    UINT8 ucSBNum = 0;
    UINT8 ucTemp = 0;
    UINT8 pucEmpSBTMBackup[EMP_INFO_LEN+3] =
        {
        0x7f,                 // HB0, 0x7F (EMP)
        0xc0,                 // HB1, B[7]: First, B[6]: Last, B[5:0]: Rsvd
        0x00,                      // HB2, Sequence Index, First=0
        0x96, // PB0: B[7]: New=1, B[6]: End=0, B[5:4]: DS_Type=1, B[3]: AFR=0, B[2]: VFR=1, B[1] Sync=1, B[0] Rsvd(0)
        0x00, // PB1: Rsvd(0)
        0x01, // PB2: Organization_ID=1
        0x00, // PB3: Data_Set_Tag=3 (MSB)
        0x03, // PB4: Data_Set_Tag=3 (LSB)
        0x00, // PB5: Data_Set_Length=3(MSB)
        0x03, // PB6: Data_Set_Length=3(LSB)
        0x55, // PB7: MD0= ([7:6] GRDM_lum =1 [5:4] GRDM_min=1 [3:2] SBTM_type=1 [1:0] SBTM_mode=1)
        0x01, // PB8: MD1= (FrmPBLimitInt [13:8])
        0x05, // PB9: MD2= (FrmPBLimitInt [7:0])
        0x00, // PB10: MD3
        0x00, // PB11: MD4
        0x00, // PB12: MD5
        0x00, // PB13: MD6
        0x00, // PB14: MD7
        0x00, // PB15: MD8
        0x00, // PB16: MD9
        0x00, // PB17: MD10
        0x00, // PB18: MD11
        0x00, // PB19: MD12
        0x00, // PB20: MD13
        0x00, // PB21: MD14
        0x00, // PB22: MD15
        0x00, // PB23: MD16
        0x00, // PB24: MD17
        0x00, // PB25: MD18
        0x00, // PB26: MD19
        0x00, // PB27: MD20
        }; // SPD payload len=28, +3(header)

    InfoMessageHDMITx("[HDMI_TX21] ScalerHdmi21MacTx0FillSBTMPacket\n");

    do
    {
        DebugMessageHDMITx("[HDMI_TX21] SBTM Data[%d]=%x\n",ucTemp, pucEmpSBTMBackup[ucTemp]);
        //ucChecksum -= pucEmpVtemBackup[ucTemp];
        ucTemp ++;
    }
    while(ucTemp < (EMP_INFO_LEN+3)); // SPD payload len=25, +3(InfoFrame type, version, len)

    // Pacekt Header
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_3_reg, pucEmpSBTMBackup[0]); // HB0: EMP type (0x7f)
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_2_reg, pucEmpSBTMBackup[1]); // HB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_1_reg, pucEmpSBTMBackup[2]); // HB2 sequence index

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-25 + 1(checksum)
    {
        rtd_outl((HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_7_reg, pucEmpSBTMBackup[3]); // WORD0: PB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_6_reg, pucEmpSBTMBackup[4]); // WORD0: PB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_5_reg, pucEmpSBTMBackup[5]); // WORD0: PB2
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg, pucEmpSBTMBackup[6]); // WORD0: PB3
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_11_reg, pucEmpSBTMBackup[7]); // WORD1: PB4
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_10_reg, pucEmpSBTMBackup[8]); // WORD1: PB5
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_9_reg, pucEmpSBTMBackup[9]); // WORD1: PB6

    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_15_reg, pucEmpSBTMBackup[10]); // WORD2: PB7: MD0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_14_reg, pucEmpSBTMBackup[11]); // WORD2: PB8: MD1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_13_reg, pucEmpSBTMBackup[12]); // WORD2: PB9: MD2
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_12_reg, pucEmpSBTMBackup[13]); // WORD2: PB10: MD3
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_19_reg, pucEmpSBTMBackup[14]); // WORD3: PB11: MD4
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_18_reg, pucEmpSBTMBackup[15]); // WORD3: PB12: MD5
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_17_reg, pucEmpSBTMBackup[16]); // WORD3: PB13: MD6

    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_23_reg, pucEmpSBTMBackup[17]); // WORD4: PB14: MD7
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_22_reg, pucEmpSBTMBackup[18]); // WORD4: PB15: MD8
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_21_reg, pucEmpSBTMBackup[19]); // WORD4: PB16: MD9
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_20_reg, pucEmpSBTMBackup[20]); // WORD4: PB17: MD10
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_27_reg, pucEmpSBTMBackup[21]); // WORD5: PB18: MD11
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_26_reg, pucEmpSBTMBackup[22]); // WORD5: PB19: MD12
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_25_reg, pucEmpSBTMBackup[23]); // WORD5: PB20: MD13

    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_31_reg, pucEmpSBTMBackup[24]); // WORD6: PB21: MD14
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_30_reg, pucEmpSBTMBackup[25]); // WORD6: PB22: MD15
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_29_reg, pucEmpSBTMBackup[26]); // WORD6: PB23: MD16
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_28_reg, pucEmpSBTMBackup[27]); // WORD6: PB24: MD17
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_35_reg, pucEmpSBTMBackup[28]); // WORD7: PB25: MD18
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_34_reg, pucEmpSBTMBackup[29]); // WORD7: PB26: MD19
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_33_reg, pucEmpSBTMBackup[30]); // WORD7: PB27: MD20
#endif//#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    return;
}

//--------------------------------------------------
// Description  : Hdmi Tx HDR Packet
// Input Value  :
// Output Value :
//--------------------------------------------------

void ScalerHdmi21MacTx0FillSPDPacket(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
	UINT8  pucDPRxSPDBackup[SPD_INFO_LEN+3] =
	{
	    0x83,                 // HB0, InfoFrame type=0x03 (0x80+InfoFrame type)
	    0x01,                 // HB1, infoFrame version=1
	    SPD_INFO_LEN,   // HB2, length of following SPD InfoFrame=25byte
	    0x72, // PB1: VN1, r
	    0x65, // PB2: VN2, e
	    0x61, // PB3: VN3, a
	    0x6c, // PB4: VN4, l
	    0x74, // PB5: VN5, t
	    0x65, // PB6: VN6, e
	    0x6b, // PB7: VN7, k
	    0x00, // PB8: VN8, (NULL)
	    0x68, // PB9: PD1, h
	    0x35, // PB10: PD2, 5
	    0x64, // PB11: PD3, d
	    0x00, // PB12: PD4, (NULL)
	    0x00, // PB13: PD5, (NULL)
	    0x00, // PB14: PD6, (NULL)
	    0x00, // PB15: PD7, (NULL)
	    0x00, // PB16: PD8, (NULL)
	    0x00, // PB17: PD9, (NULL)
	    0x00, // PB18: PD10, (NULL)
	    0x00, // PB19: PD11, (NULL)
	    0x00, // PB20: PD12, (NULL)
	    0x00, // PB21: PD13, (NULL)
	    0x00, // PB22: PD14, (NULL)
	    0x00, // PB23: PD15, (NULL)
	    0x00, // PB24: PD16, (NULL)
	    0x0d, // PB25: Source Info, PMP
	}; // SPD payload len=25, +3(Info Frame type + version, len)
    DebugMessageHDMITx("[HDMI_TX] ScalerHdmi21MacTx0FillSPDPacket\n");

    do
    {
        DebugMessageHDMITx("[HDMI_TX] SPD Data[%d]=%x\n",(UINT32)ucTemp, (UINT32)pucDPRxSPDBackup[ucTemp]);
        ucChecksum -= pucDPRxSPDBackup[ucTemp];
        ucTemp ++;
    }
    while(ucTemp < (SPD_INFO_LEN+3)); // SPD payload len=25, +3(InfoFrame type, version, len)

    // Pacekt Header
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_3_reg, pucDPRxSPDBackup[0]); // HB0: InfoFrame type (0x83)
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_2_reg, pucDPRxSPDBackup[1]); // HB1 infoFrame version (1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_1_reg, pucDPRxSPDBackup[2]); // HB2 Length=25


    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-25 + 1(checksum)
    {
        rtd_outl((HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_6_reg, pucDPRxSPDBackup[3]); // WORD0: PB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_5_reg, pucDPRxSPDBackup[4]); // WORD0: PB2
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg, pucDPRxSPDBackup[5]); // WORD0: PB3
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_11_reg, pucDPRxSPDBackup[6]); // WORD1: PB4
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_10_reg, pucDPRxSPDBackup[7]); // WORD1: PB5
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_9_reg, pucDPRxSPDBackup[8]); // WORD1: PB6

    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_15_reg, pucDPRxSPDBackup[9]); // WORD2: PB7
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_14_reg, pucDPRxSPDBackup[10]); // WORD2: PB8
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_13_reg, pucDPRxSPDBackup[11]); // WORD2: PB9
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_12_reg, pucDPRxSPDBackup[12]); // WORD2: PB10
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_19_reg, pucDPRxSPDBackup[13]); // WORD3: PB11
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_18_reg, pucDPRxSPDBackup[14]); // WORD3: PB12
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_17_reg, pucDPRxSPDBackup[15]); // WORD3: PB13

    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_23_reg, pucDPRxSPDBackup[16]); // WORD4: PB14
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_22_reg, pucDPRxSPDBackup[17]); // WORD4: PB15
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_21_reg, pucDPRxSPDBackup[18]); // WORD4: PB16
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_20_reg, pucDPRxSPDBackup[19]); // WORD4: PB17
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_27_reg, pucDPRxSPDBackup[20]); // WORD5: PB18
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_26_reg, pucDPRxSPDBackup[21]); // WORD5: PB19
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_25_reg, pucDPRxSPDBackup[22]); // WORD5: PB20

    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_31_reg, pucDPRxSPDBackup[23]); // WORD6: PB21
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_30_reg, pucDPRxSPDBackup[24]); // WORD6: PB22
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_29_reg, pucDPRxSPDBackup[25]); // WORD6: PB23
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_28_reg, pucDPRxSPDBackup[26]); // WORD6: PB24
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_35_reg, pucDPRxSPDBackup[27]); // WORD7: PB25
#endif//#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    return;
}

//--------------------------------------------------
// Description  : Hdmi Tx Dynamic HDR EMP Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTx0DHDRPacket(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    //send EMP DHDR packet

    ScalerHdmiTx_Wait_VerticalDataEnable();

    for(ucPacketNum = 0; ucPacketNum < 64; ucPacketNum++)
    {
        if(ucPacketNum == (64 - 1))
        {
            rtd_outl(HDMITX_MISC_HDMITX_EMP_HEADER_reg ,0x00002100);// DP header pattern 0x21
        }
        else
        {
            rtd_outl(HDMITX_MISC_HDMITX_EMP_HEADER_reg ,0x00802100);// BOOLEAN 23 is change BOOLEAN, when last change to 0
        }
        for(ucNum = 0; ucNum < 8; ucNum++)
        {
             if(ucPacketNum == 0 && ucNum == 0) //fill
            {
                rtd_outl(HDMITX_MISC_HDMITX_EMP_DATA0_reg ,0x07fc0001);// length LSB MSB and set tag LSB MSB CTA-861G table 46/47 ; length need to be total -4 for reset data (1024 -4)
            }
            else
            {
                //rtd_outl(HDMITX_MISC_HDMITX_EMP_DATA0_reg + ucNum*4 ,ucNum*0x01010101 + ucPacketNum);
         	  temp_value16 = ucNum;
		  temp_value16 = temp_value16 << 8;
		  temp_value16 = temp_value16 | ucNum;
		  temp_value32  = temp_value16;
		  temp_value32 = temp_value32 << 16;
		  temp_value32 = temp_value32 | temp_value16;
		  temp_value32 = temp_value32 + ucPacketNum;
		  temp_value8 = ucNum << 2;

		 rtd_outl(HDMITX_MISC_HDMITX_EMP_DATA0_reg + temp_value8, temp_value32);
            }
        }
    }
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_DHDR_1_reg, ~(_BIT7 | _BIT6), _BIT7 );

    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_DHDR_CTRL_reg, ~(_BIT7 | _BIT6 | _BIT5), _BIT6);
#endif//#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    return;
}


#ifdef CONFIG_ENABLE_ZEBU_BRING_UP 
void ScalerHdmi21MacTx0SetAviInfoFramePkt(EnumColorSpace colorFmt)
{
    if((colorFmt != _COLOR_SPACE_RGB) && (colorFmt != _COLOR_SPACE_YCBCR444)){
        ErrorMessageHDMI21Tx("[HDMI_TX21] NOT SUPPORT COLOR SPACE %d@ScalerHdmi21MacTx0SetAviInfoFramePkt\n", colorFmt);
        return;
    }

    NoteMessageHDMI21Tx("[HDMI_TX21] Set AVI COLOR SPACE=%d\n", colorFmt);

    Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_COLOR_SPACE, colorFmt);
    SET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0, colorFmt);

    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~_BIT6, 0x00);

    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT0, 0x00);
    // Packet0: AVI Packet
    if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT0_ADDRESS, _HDMI_TX_AVI_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
    {
        ErrorMessageHDMI21Tx("[HDMI_TX21]AVI info Packet send fail@ScalerHdmi21MacTx0SetAviInfoFramePkt\n");
        return;
    }
    else
    {
        // enable avi pkt
        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT0, _BIT0);

        InfoMessageHDMITx("[HDMI_TX21] Set AVI info Packet to %d@%x\n", colorFmt, rtd_inl(TIMER_VCPU_CLK90K_LO_reg));
    }

    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~_BIT6, _BIT6);


    return;
}
#endif // #ifdef CONFIG_ENABLE_ZEBU_BRING_UP 

#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx HDR Set
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTx0HDRSet(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    UINT8 ucStreamIndex = _ST_MAP_NONE;
    static UINT32 lastDumpStc = 0;
    stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);

	SLR_VOINFO *pVOInfoNew = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
    ucStreamIndex = ScalerDpStreamPxSTMapping(ScalerHdmiMacTxPxMapping(_TX0));

    // update HDMITX quantization range info
    if(ScalerHdmiTxGetUpdateVoInfoFlag() && pVOInfoNew)
        DebugMessageHDMITx("[HDMI21_TX] Got video_full_range_flag=%d\n", pVOInfoNew->video_full_range_flag);

    // check HDR Packet Change
    if(GET_STREAM_HDR_INFO_FRAME_RECEIVED() == _TRUE)
    {

        if(((UINT32)(stc - lastDumpStc)) > 450000){ // dump period=1sec
            DebugMessageHDMI21Tx("[HDMI_TX21]HDR active@%d\n", __LINE__);
            lastDumpStc = stc;
        }
        if((GET_STREAM_HDR_INFO_FRAME_CHANGE() == _TRUE) || ((ScalerHdmiTxGetUpdateVoInfoFlag() || ScalerHdmiTxGetHDRFlag()==0) && pVOInfoNew !=NULL))
        {
            DebugMessageHDMI21Tx("[HDMI_TX21]HDR Packet Changed@%d\n", __LINE__);

            CLR_DP_STREAM_HDR_INFO_FRAME_CHANGE(ucStreamIndex);

            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~_BIT6, 0x00);
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT3, 0x00);

            // Packet3: HDR Packet
            if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT3_ADDRESS, _HDMI_TX_STATIC_HDR_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                ErrorMessageHDMI21Tx("[HDMI_TX21]HDR Packet send fail@%d\n", __LINE__);
                return;
            }
            else
            {
                // enable hdr pkt
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT3, _BIT3);
            }
            NoteMessageHDMI21Tx("[HDMI_TX21]colour_primaries = %d, matrix_coefficiets=%d\n",pHDMITX_VOInfo->colour_primaries,pHDMITX_VOInfo->matrix_coefficiets);
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
                Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_YCC_QUANTIZATION_RANGE, pHDMITX_VOInfo->video_full_range_flag);
                ScalerHdmiTxSetUpdateVoInfo(_DISABLE);
            }

            // Set Colorimetry
            SET_HDMI21_MAC_TX_INPUT_COLORIMETRY(ucStreamIndex, ScalerDpStreamGetElement(_P0_OUTPUT_PORT, _COLORIMETRY));
            // Set EXT_Colorimetry
            SET_HDMI21_MAC_TX_INPUT_EXT_COLORIMETRY(ucStreamIndex, ScalerDpStreamGetElement(_P0_OUTPUT_PORT, _EXT_COLORIMETRY));


            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT0, 0x00); //resend AVI infoframe
            // Packet0: AVI Packet
            if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT0_ADDRESS, _HDMI_TX_AVI_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {

                ErrorMessageHDMI21Tx("[HDMI_TX21]AVI info Packet send fail in HDR@%d\n", __LINE__);
                return;
            }
            else
            {
                // enable avi pkt
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT0, _BIT0);
            }
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~_BIT6, _BIT6);

            ScalerHdmiTxSetHDRFlag(TRUE);

            InfoMessageHDMITx("[HDMI_TX21] Set AVI info Packet to HDR@%x\n", rtd_inl(TIMER_VCPU_CLK90K_LO_reg));
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

            // Set Colorimetry
            SET_HDMI21_MAC_TX_INPUT_COLORIMETRY(ucStreamIndex, ScalerDpStreamGetElement(_P0_OUTPUT_PORT, _COLORIMETRY));
            // Set EXT_Colorimetry
            SET_HDMI21_MAC_TX_INPUT_EXT_COLORIMETRY(ucStreamIndex, ScalerDpStreamGetElement(_P0_OUTPUT_PORT, _EXT_COLORIMETRY));

            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~_BIT6, 0x00);

            // Packet3: Set DRM InfoFrame Packet to SDR mode
            if(ScalerHdmiTxGetHDRFlag())
            {
                // disable hdr pkt
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT3, 0x00);

                if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT3_ADDRESS, _HDMI_TX_STATIC_HDR_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE){
                    ErrorMessageHDMI21Tx("[HDMI_TX21] HDR Packet send fail@ScalerHdmi21MacTx0HDRSet\n");
                }else{
                	// enable hdr pkt
                    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT3, _BIT3);
                    InfoMessageHDMITx("[HDMI_TX21] Change DRM info to SDR\n");
                }
            }

            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT0, 0x00);
            // Packet0: AVI Packet
            if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT0_ADDRESS, _HDMI_TX_AVI_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                ErrorMessageHDMI21Tx("[HDMI_TX21]AVI info Packet send fail in SDR@%d\n", __LINE__);
                return;
            }
            else
            {
                // enable avi pkt
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT0, _BIT0);

                InfoMessageHDMITx("[HDMI_TX21] Set AVI info Packet to SDR@%x\n", rtd_inl(TIMER_VCPU_CLK90K_LO_reg));
            }

            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~_BIT6, _BIT6);

            ScalerHdmiTxSetHDRFlag(FALSE);
        }
        pHDMITX_VOInfo =NULL;
    }
#endif//#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
}

//--------------------------------------------------
// Description  : Hdmi Tx HDR Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTx0FillHDRPacket(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    UINT8 ucSBNum = 0;
    UINT8 ucTemp = 0;
    UINT8 ucChecksum = 0x00;
    UINT8 pucDPRxHDRBackup[32] = {0x00};

    UINT8 ucStreamIndex = _ST_MAP_NONE;
    UINT8 hdrtype = HDR_DM_MODE_NONE;
    SLR_VOINFO *pVOInfoNew = NULL;
    seamless_change_sync_info *seamless_info = NULL;
    static UINT8 last_hdrType=0;

    HDMI_DRM_T hdmi_drm;
    // bypass HDR metadata
    if((ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_DRM_HDR) == TRUE) && (ScalerHdmiTxRepeaterGetSourceInfo_hdrMode() == HDR_HDR10_HDMI)){
        if(ScalerHdmiTxRepeaterGetSourceInfo_hdrDrmInfo((UINT8*)&hdmi_drm) != 0){
            FatalMessageHDMITx("[HDMI_TX21] Get HDMI DRM DM Fail!!\n");
            return;
        }else{
            memcpy(&pucDPRxHDRBackup[0], &hdmi_drm.ver, 28);
        }
    }else
    {
        seamless_info = (seamless_change_sync_info *)Scaler_GetShareMemVirAddr(SCALERIOC_SEAMLESS_CHANGE_SHAREMEMORY_INFO_SYNC_FLAG);
        if(seamless_info == NULL)
        {
            ErrorMessageHDMI21Tx("[HDMI_TX21] seamless_info NULL@%d\n", __LINE__);
            return;
        }
        hdrtype = Scaler_ChangeUINT32Endian(seamless_info->vdec_cur_hdr_type);

        pVOInfoNew = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
        ucStreamIndex = ScalerDpStreamPxSTMapping(ScalerHdmiMacTxPxMapping(_TX0));
        if(hdrtype && ((pVOInfoNew == NULL)|| (pHDMITX_VOInfo == NULL)))
        {
            ErrorMessageHDMI21Tx("[HDMI_TX21] VO/HDMI Info NULL(%x/%x)\n", (UINT32)pVOInfoNew, (UINT32)pHDMITX_VOInfo);
            return ;
        }

        memset(&pucDPRxHDRBackup, 0, sizeof(pucDPRxHDRBackup));

        if(hdrtype)
            memcpy(pHDMITX_VOInfo, pVOInfoNew, sizeof(SLR_VOINFO));

        // Get Packet data from ST
        //ScalerDpStreamDpSdpGetData(ucStreamIndex, _DP_INFO_HDR, pucDPRxHDRBackup);

        pucDPRxHDRBackup[0] = 1; //infoFrame version
        if(hdrtype == HDR_DM_HDR10)
        {
            pucDPRxHDRBackup[1] = 26; // length of following HDR Metadata InfoFrame
            pucDPRxHDRBackup[2] = 2; // HDR10 EOTF =2
            pucDPRxHDRBackup[3] = 0; // Static Metadata Descriptor ID =0
            pucDPRxHDRBackup[4] = (pHDMITX_VOInfo->dispXY0 >> 16) & 0xFF; //x0 LSB
            pucDPRxHDRBackup[5] = (pHDMITX_VOInfo->dispXY0 >> 24) ; //x0 MSB
            pucDPRxHDRBackup[6] = pHDMITX_VOInfo->dispXY0  & 0xFF; //y0 LSB
            pucDPRxHDRBackup[7] = (pHDMITX_VOInfo->dispXY0 >> 8) & 0xFF; //y0 MSB
            pucDPRxHDRBackup[8] = (pHDMITX_VOInfo->dispXY1 >> 16) & 0xFF; //x1 LSB
            pucDPRxHDRBackup[9] = (pHDMITX_VOInfo->dispXY1 >> 24) ; //x1 MSB
            pucDPRxHDRBackup[10] = pHDMITX_VOInfo->dispXY1  & 0xFF; //y1 LSB
            pucDPRxHDRBackup[11] = (pHDMITX_VOInfo->dispXY1 >> 8) & 0xFF; //y1 MSB
            pucDPRxHDRBackup[12] = (pHDMITX_VOInfo->dispXY2 >> 16) & 0xFF; //x2 LSB
            pucDPRxHDRBackup[13] = (pHDMITX_VOInfo->dispXY2 >> 24) ; //x2 MSB
            pucDPRxHDRBackup[14] = pHDMITX_VOInfo->dispXY2  & 0xFF; //y2 LSB
            pucDPRxHDRBackup[15] = (pHDMITX_VOInfo->dispXY2 >> 8) & 0xFF; //y2 MSB

            pucDPRxHDRBackup[16] = (pHDMITX_VOInfo->whitePointXY >> 16) & 0xFF; //white x LSB
            pucDPRxHDRBackup[17] = (pHDMITX_VOInfo->whitePointXY >> 24) ; //white x MSB
            pucDPRxHDRBackup[18] = pHDMITX_VOInfo->whitePointXY  & 0xFF; //white y LSB
            pucDPRxHDRBackup[19] = (pHDMITX_VOInfo->whitePointXY >> 8) & 0xFF; //white y MSB

            pucDPRxHDRBackup[20] = (pHDMITX_VOInfo->maxL /10000 ) & 0xFF; //max luminance LSB
            pucDPRxHDRBackup[21] = ((pHDMITX_VOInfo->maxL /10000 ) >> 8) & 0xFF; //max luminance MSB
            pucDPRxHDRBackup[22] = pHDMITX_VOInfo->minL  & 0xFF; //min luminance LSB
            pucDPRxHDRBackup[23] = (pHDMITX_VOInfo->minL >> 8) & 0xFF; //min luminance MSB

            pucDPRxHDRBackup[24] = pHDMITX_VOInfo->MaxCLL & 0xFF; //max content light level LSB
            pucDPRxHDRBackup[25] = (pHDMITX_VOInfo->MaxCLL >> 8) & 0xFF; //max content light level MSB
            pucDPRxHDRBackup[26] = pHDMITX_VOInfo->MaxFALL  & 0xFF; //max frame-average light level LSB
            pucDPRxHDRBackup[27] = (pHDMITX_VOInfo->MaxFALL >> 8) & 0xFF; //max frame-average light level MSB
        }
        else if(hdrtype == HDR_DM_HLG)
        {
            pucDPRxHDRBackup[1] = 1; // length of following HDR Metadata InfoFrame
            pucDPRxHDRBackup[2] = 3; // HLG EOTF =3
        }
        else { // HDR_DM_MODE_NONE
            pucDPRxHDRBackup[1] = 3; // length of following HDR Metadata InfoFrame
            pucDPRxHDRBackup[2] = 0; // PB[1] SDR EOTF =0
            pucDPRxHDRBackup[3] = 0; // PB[2] Static_Metadata_Descriptor_ID (3 bits)
            pucDPRxHDRBackup[4] = 0; // PB[3] Static_Metadata_Descriptor
        }
    }

    InfoMessageHDMI21Tx("[HDMI21_TX]  Repeater Mode[%d@%d], DRM T/V/L[%x/%x/%x] EOTF=%d\n",
            get_hdmitxOutputMode(), ScalerHdmiTxRepeaterGetSourceInfo_hdrMode(), hdmi_drm.type_code, hdmi_drm.ver, hdmi_drm.len, hdmi_drm.eEOTFtype);
    InfoMessageHDMI21Tx("[HDMI21_TX]  pucAVIInfoPacketData[0]=%x\n", pucDPRxHDRBackup[0]);
    InfoMessageHDMI21Tx("[HDMI21_TX]  pucAVIInfoPacketData[1]=%x\n", pucDPRxHDRBackup[1]);
    InfoMessageHDMI21Tx("[HDMI21_TX]  pucAVIInfoPacketData[2]=%x\n", pucDPRxHDRBackup[2]);
    InfoMessageHDMI21Tx("[HDMI21_TX]  pucAVIInfoPacketData[3]=%x\n", pucDPRxHDRBackup[3]);
    InfoMessageHDMI21Tx("[HDMI21_TX]  pucAVIInfoPacketData[4]=%x\n", pucDPRxHDRBackup[4]);

    do
    {
        DebugMessageHDMITx("[HDMI_TX] HDR Data %d =%x\n",ucTemp, pucDPRxHDRBackup[ucTemp]);
        ucChecksum -= pucDPRxHDRBackup[ucTemp];
        ucTemp ++;
    }
    while(ucTemp < 28);

    if(last_hdrType != hdrtype){
       InfoMessageHDMITx("[HDMI_TX21] HDR type[%d->%d]\n", last_hdrType, hdrtype);
       last_hdrType = hdrtype;
    }

    ucChecksum -= 0x87;

    // Pacekt Header
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_3_reg, 0x87); // HB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_2_reg, pucDPRxHDRBackup[0]); // HB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_1_reg, pucDPRxHDRBackup[1]); // HB2 Length=26


    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_6_reg, pucDPRxHDRBackup[2]); // WORD0: PB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_5_reg, pucDPRxHDRBackup[3]); // WORD0: PB2
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg, pucDPRxHDRBackup[4]); // WORD0: PB3
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_11_reg, pucDPRxHDRBackup[5]); // WORD1: PB4
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_10_reg, pucDPRxHDRBackup[6]); // WORD1: PB5
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_9_reg, pucDPRxHDRBackup[7]); // WORD1: PB6

    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_15_reg, pucDPRxHDRBackup[8]); // WORD2: PB7
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_14_reg, pucDPRxHDRBackup[9]); // WORD2: PB8
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_13_reg, pucDPRxHDRBackup[10]); // WORD2: PB9
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_12_reg, pucDPRxHDRBackup[11]); // WORD2: PB10
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_19_reg, pucDPRxHDRBackup[12]); // WORD3: PB11
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_18_reg, pucDPRxHDRBackup[13]); // WORD3: PB12
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_17_reg, pucDPRxHDRBackup[14]); // WORD3: PB13

    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_23_reg, pucDPRxHDRBackup[15]); // WORD4: PB14
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_22_reg, pucDPRxHDRBackup[16]); // WORD4: PB15
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_21_reg, pucDPRxHDRBackup[17]); // WORD4: PB16
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_20_reg, pucDPRxHDRBackup[18]); // WORD4: PB17
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_27_reg, pucDPRxHDRBackup[19]); // WORD5: PB18
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_26_reg, pucDPRxHDRBackup[20]); // WORD5: PB19
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_25_reg, pucDPRxHDRBackup[21]); // WORD5: PB20

    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_31_reg, pucDPRxHDRBackup[22]); // WORD6: PB21
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_30_reg, pucDPRxHDRBackup[23]); // WORD6: PB22
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_29_reg, pucDPRxHDRBackup[24]); // WORD6: PB23
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_28_reg, pucDPRxHDRBackup[25]); // WORD6: PB24
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_35_reg, pucDPRxHDRBackup[26]); // WORD7: PB25
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_34_reg, pucDPRxHDRBackup[27]); // WORD7: PB26
#endif//#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
}
#endif // End of #if(_HDMI_HDR10_TX0_SUPPORT == _ON)



#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
//--------------------------------------------------
// Description  : Hdmi Tx Vendor Specific Infoframe Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTx0FillVendorSpecificInfoPacket(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    UINT8 ucSBNum = 0x00;
    UINT8 ucChecksum = 0x00;
    UINT8 pucVSInfoPacketData[5] = {0x00,0x00,0x00,0x00,0x00};

    if((ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_VSIF_HDMI_ALLM) == TRUE) && (ScalerHdmiTxRepeaterGetSourceInfo_Allm())){
        // PB[3:1]: HF-VSIF OUI=0xc45dd8
        pucVSInfoPacketData[0] = 0xd8;
        pucVSInfoPacketData[1] = 0x5d;
        pucVSInfoPacketData[2] = 0xc4;

        // PB[4]: Version=1
        pucVSInfoPacketData[3] = 0x01;

        // PB[5]: B[0]: 3D, B[1]: ALLM_Mode
        pucVSInfoPacketData[4] = _BIT1;
    }

    ucChecksum -= 0x81;
    ucChecksum -= 0x01;
    ucChecksum -= 0x05;

    ucChecksum -= pucVSInfoPacketData[0];
    ucChecksum -= pucVSInfoPacketData[1];
    ucChecksum -= pucVSInfoPacketData[2];
    ucChecksum -= pucVSInfoPacketData[3];
    ucChecksum -= pucVSInfoPacketData[4];

    InfoMessageHDMITx("[HDMI_TX21] VSIF PB1=%x\n", pucVSInfoPacketData[0]);
    InfoMessageHDMITx("[HDMI_TX21] VSIF PB2=%x\n", pucVSInfoPacketData[1]);
    InfoMessageHDMITx("[HDMI_TX21] VSIF PB3=%x\n", pucVSInfoPacketData[2]);
    InfoMessageHDMITx("[HDMI_TX21] VSIF PB4=%x\n", pucVSInfoPacketData[3]);
    InfoMessageHDMITx("[HDMI_TX21] VSIF PB5=%x\n", pucVSInfoPacketData[4]);

    // Pacekt Header
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_3_reg, 0x81); // HB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_2_reg, 0x01); // HB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_1_reg, 0x05); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_6_reg, pucVSInfoPacketData[0]); // WORD0: PB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_5_reg, pucVSInfoPacketData[1]); // WORD0: PB2
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg, pucVSInfoPacketData[2]); // WORD0: PB3

    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_11_reg, pucVSInfoPacketData[3]); // WORD1: PB4
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_10_reg, pucVSInfoPacketData[4]); // WORD1: PB5

#endif//#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    return;
}


//--------------------------------------------------
// Description  : Hdmi Tx Vendor Specific Infoframe Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTx0FillDolbyVisionVsifPacket(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    UINT8 ucSBNum = 0x00;
    UINT8 ucChecksum = 0x00;
    UINT8 pucVSInfoPacketData[27] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // HB[2:0]+PB[13:0]

    if((ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_VSIF_DV_VSIF) == TRUE) && (ScalerHdmiTxRepeaterGetSourceInfo_DvVsifMode() != DOLBY_HDMI_VSIF_DISABLE)){
        if(ScalerHdmiTxRepeaterGetSourceInfo_DvVsifInfo(&pucVSInfoPacketData[0]) != 0){
            FatalMessageHDMITx("[HDMI_TX21] Get DV VSIF Info Fail!!\n");
            return;
        }
    }else{ // DV VSIF Disable
        ucChecksum -= 0x81;
        ucChecksum -= 0x01;
        ucChecksum -= 0x1b;
        pucVSInfoPacketData[3] = ucChecksum;
    }

    InfoMessageHDMITx("[HDMI_TX21] DV VSIF HB0=%x\n", pucVSInfoPacketData[0]); // HB0 (0x81)
    InfoMessageHDMITx("[HDMI_TX21] DV VSIF HB1=%x\n", pucVSInfoPacketData[1]); // HB1 (0x01)
    InfoMessageHDMITx("[HDMI_TX21] DV VSIF HB2=%x\n", pucVSInfoPacketData[2]); // HB2 (0x1b)
    InfoMessageHDMITx("[HDMI_TX21] DV VSIF PB0=%x\n", pucVSInfoPacketData[3]); // PB[0]: checksum
    InfoMessageHDMITx("[HDMI_TX21] DV VSIF PB1=%x\n", pucVSInfoPacketData[4]); // PB[3:1]=0x00d046 (0x46)
    InfoMessageHDMITx("[HDMI_TX21] DV VSIF PB2=%x\n", pucVSInfoPacketData[5]); // PB[3:1]=0x00d046 (0xd0)
    InfoMessageHDMITx("[HDMI_TX21] DV VSIF PB3=%x\n", pucVSInfoPacketData[6]); // PB[3:1]=0x00d046 (0x00)
    InfoMessageHDMITx("[HDMI_TX21] DV VSIF PB4=%x\n", pucVSInfoPacketData[7]); // PB[4]: DV mode(1), B[0]: LL mode(1)
    InfoMessageHDMITx("[HDMI_TX21] DV VSIF PB5=%x\n", pucVSInfoPacketData[8]); // PB[5]:
    InfoMessageHDMITx("[HDMI_TX21] DV VSIF PB6=%x\n", pucVSInfoPacketData[9]); // PB[6]:

    // Pacekt Header
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_3_reg, 0x81); // HB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_2_reg, 0x01); // HB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_1_reg, 0x1b); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_7_reg, pucVSInfoPacketData[3]); // WORD0: PB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_6_reg, pucVSInfoPacketData[4]); // WORD0: PB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_5_reg, pucVSInfoPacketData[5]); // WORD0: PB2
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg, pucVSInfoPacketData[6]); // WORD0: PB3
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_11_reg, pucVSInfoPacketData[7]); // WORD1: PB4
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_10_reg, pucVSInfoPacketData[8]); // WORD1: PB5
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_9_reg, pucVSInfoPacketData[9]); // WORD1: PB6

    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_15_reg, pucVSInfoPacketData[10]); // WORD2: PB7: MD0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_14_reg, pucVSInfoPacketData[11]); // WORD2: PB8: MD1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_13_reg, pucVSInfoPacketData[12]); // WORD2: PB9: MD2
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_12_reg, pucVSInfoPacketData[13]); // WORD2: PB10: MD3
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_19_reg, pucVSInfoPacketData[14]); // WORD3: PB11: MD4
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_18_reg, pucVSInfoPacketData[15]); // WORD3: PB12: MD5
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_17_reg, pucVSInfoPacketData[16]); // WORD3: PB13: MD6
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT

    return;
}

#endif // #ifdef CONFIG_HDMITX_REPEATER_SUPPORT
//--------------------------------------------------
// Description  : HDMI2.1 Tx DSC encoder output setting
// Input Value  : None
// Output Value : None
//--------------------------------------------------
#if(_DSC_SUPPORT == _ON)
void ScalerHdmi21MacTx0DscPPSPacket(void)
{
    UINT8 ucNum = 0;
    UINT8 ucFrontHiByte = 0;
    UINT8 ucFrontLoByte = 0;
    UINT8 ucHsyncWidthHiByte = 0;
    UINT8 ucHsyncWidthLoByte = 0;
    UINT8 ucHBackHiByte = 0;
    UINT8 ucHBackLoByte = 0;
    UINT8 ucHCActiveTriHiByte = 0;
    UINT8 ucHCActiveTriLoByte = 0;
    UINT8 ucPacketNum = 0;
    UINT32 uiPacketData = 0;
    UINT8 ucPPSCount = 0;
    INT32 slice_width;
    INT32 ucChunckNum;
    UINT16 usHCActive = 0;

    //StructTimingInfo stTimingInfo;
#if 0
    BYTE ucWriteLen = 0;
#if(_DSC_ENCODER_SUPPORT == _ON)
    if(GET_HDMI21_MAC_TX_INPUT_FROM(_TX0) == _INPUT_FROM_ENCODER) // PPS FW mode
    {
        // PPS mode SEL from Dsc Encoder : FW write
        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_CTRL_reg, ~(_BIT7 | _BIT6 | _BIT5), (_BIT7| _BIT5));

        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_DSC_ENC_0_reg, ~( _BIT5), _BIT5);
        for(ucNum = 0; ucNum < (_HDMI21_PPS_PACKET_TOTAL_SIZE / _HDMI21_PPS_PACKET_LEN); ucNum ++) // for 128 byte pps  16* 8
        {
            for(ucWriteLen = 0; ucWriteLen < _HDMI21_PPS_PACKET_LEN; ucWriteLen++)
            {
                rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_DSC_ENC_8_reg - (ucWriteLen*4), g_ppucHdmi21MacTxPPSData[_TX0][ucWriteLen + (ucNum * _HDMI21_PPS_PACKET_LEN)]);

                //DebugMessageHDMI21Tx("[HDMI21_TX] HDMI21_TX0: PPS offset =%d\n", ucWriteLen + (ucNum * _HDMI21_PPS_PACKET_LEN));
                //DebugMessageHDMI21Tx("[HDMI21_TX] HDMI21_TX0: PPS data =%x\n", g_ppucHdmi21MacTxPPSData[_TX0][ucWriteLen + (ucNum * _HDMI21_PPS_PACKET_LEN)]);
            }
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_DSC_ENC_0_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), ucNum);
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_DSC_ENC_0_reg, ~(_BIT7), _BIT7);
        }
        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_EMP_BYPASS_7_reg, ~(_BIT5), _BIT5);

        DebugMessageHDMI21Tx("[HDMI21_TX] DSC encode mode PPS data\n");
    }
    else // bypass pps HW mode
#endif
    {
        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_CTRL_reg, ~(_BIT7 | _BIT6), 0x00);
        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_EMP_BYPASS_7_reg, ~(_BIT5), _BIT5);
        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_EMP_reg, ~(_BIT5), _BIT5);

        DebugMessageHDMI21Tx("[HDMI21_TX] DSC Bypass mode\n");
    }

    // new field = 1
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_0_reg, ~_BIT7, _BIT7);
    //ROSTimeDly(30);
    //ScalerTimerDelayXms(30);
    msleep(30);

    for(ucNum = 0; ucNum < (_HDMI21_PPS_PACKET_TOTAL_SIZE / _HDMI21_PPS_PACKET_LEN); ucNum ++) // for 128 byte pps  16* 8
    {
        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_DEBUG_0_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), ucNum);
        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_DEBUG_0_reg, ~(_BIT7), _BIT7);
        for(ucWriteLen = 0; ucWriteLen < _HDMI21_PPS_PACKET_LEN; ucWriteLen++)
        {
            DebugMessageHDMI21Tx("[HDMI21_TX] HDMI21_TX0: PPS offset =%d\n", ucWriteLen + (ucNum * _HDMI21_PPS_PACKET_LEN));
            DebugMessageHDMI21Tx("[HDMI21_TX] HDMI21_TX0: PPS data =%x\n", rtd_inl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_DEBUG_8_reg - (ucWriteLen*4)));
        }
    }

#if(_DSC_ENCODER_SUPPORT == _ON)
    if(GET_HDMI21_MAC_TX_INPUT_FROM(_TX0) == _INPUT_FROM_ENCODER) // EMP encoder mode
    {
        if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
            NoteMessageHDMI21Tx("[HDMI21_TX] g_pstHdmi21MacTxInputTimingInfo is NULL@%s,%d\n", __FUNCTION__, __LINE__);
            return;
        }

        // HFront =  Htotal - H start - Hactive;
        ucFrontHiByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth) & 0xFF00) >> 8;
        ucFrontLoByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth) & 0x00FF);

        ucHsyncWidthHiByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth) & 0xFF00) >> 8;
        ucHsyncWidthLoByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth) & 0x00FF);

        ucHCActiveTriHiByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth) & 0xFF00) >> 8;
        ucHCActiveTriLoByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth) & 0x00FF);

        ucHBackHiByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth) & 0xFF00) >> 8;
        ucHBackLoByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth) & 0x00FF);
    }
    else // EMP bypass mode , must get org timing
#endif
    {
        // From MSA 4k need Fix it
        //ScalerDpStreamGetMsaTimingInfo(ScalerHdmi21MacTxPxMapping(_TX0), &stTimingInfo);
        /*
        ucFrontHiByte = 0x00;
        ucFrontLoByte = 0x30;

        ucHsyncWidthHiByte = 0x00;
        ucHsyncWidthLoByte = 0x20;

        ucHCActiveTriHiByte = 0x0F;
        ucHCActiveTriLoByte = 0x00;

        ucHBackHiByte = 0x00;
        ucHBackLoByte = 0x50;
        */
        ucFrontHiByte = 0x00;
        ucFrontLoByte = g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCfront;

        ucHsyncWidthHiByte = 0x00;
        ucHsyncWidthLoByte = _HDMI21_DSC_DPF_SYNCWIDTH * 4;

        ucHCActiveTriHiByte = (g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCactive & 0xFF00) >> 8;
        ucHCActiveTriLoByte = (g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCactive & 0x00FF);

        ucHBackHiByte = 0x00;
        ucHBackLoByte = g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCback;

        DebugMessageHDMI21Tx("[HDMI21_TX] EMP pack BYPASS mode !! \n");
    }

    // Hfront : pixel
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_6_reg, ucFrontHiByte);
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_7_reg, ucFrontLoByte);

    // H sync Width : pixel
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_8_reg, ucHsyncWidthHiByte);
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_9_reg, ucHsyncWidthLoByte);

    // H back : pixel
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_10_reg, ucHBackHiByte);
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_11_reg, ucHBackLoByte);

    // H Active : pixel
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_12_reg, ucHCActiveTriHiByte);
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_13_reg, ucHCActiveTriLoByte);

    //ScalerTimerDelayXms((1000 / GET_HDMI21_MAC_TX_INPUT_FRAME_RATE(_TX0)) * 2);
    //if(Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_HDMITX_DSC_SRC) == DSC_SRC_TXSOC)
    //    Scaler_Wait_for_event(TXSOCTG_TXSOC_pending_status_reg, TXSOCTG_TXSOC_pending_status_mv_den_end_event_mask,2);
    //else // DSC input from DSC_SRC_DISPD
        HDMITX_DTG_Wait_Den_Stop_Done();//Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,2);
    // ScalerTimerDelayXms(30);


    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_CTRL_reg, ~_BIT6, _BIT6);
    DebugMessageHDMI21Tx("[HDMI21_TX], HDMITX21_PACKET_CONTROL_HDMI21_PPS_CTRL_reg= %x\n" , rtd_inl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_CTRL_reg));
    DebugMessageHDMI21Tx("[HDMI21_TX], HDMITX21_PACKET_CONTROL_HDMI21_PPS_DSC_ENC_0_reg = %x \n",rtd_inl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_DSC_ENC_0_reg));
#else
    // PPS mode SEL from DP RX RS BUS: HW write
    ScalerHdmiTx_Wait_VerticalDataEnable();
    // HFront =  Htotal - H start - Hactive;
    ucFrontHiByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth) & 0xFF00) >> 8;
    ucFrontLoByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth) & 0x00FF);

    ucHsyncWidthHiByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth) & 0xFF00) >> 8;
    ucHsyncWidthLoByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth) & 0x00FF);

    slice_width = ((g_ppucHdmi21MacTxPPSData[_TX0][0xC] << 8) + (g_ppucHdmi21MacTxPPSData[_TX0][0xD]));

    ucChunckNum = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth / slice_width;

    // HC byte = bpp x slice_width x slice count / 8 (bit to byte) , bpp x 16 ,so div 16 recover orginal
    usHCActive = g_ppucHdmi21MacTxPPSData[_TX0][0x5] * (slice_width) /16 /8 + ((g_ppucHdmi21MacTxPPSData[_TX0][0x5] * (slice_width) %(16*8)) > 0 ? 1:0);
    usHCActive *= ucChunckNum;

    ucHCActiveTriHiByte = (usHCActive & 0xFF00) >> 8;//g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth
    ucHCActiveTriLoByte = (usHCActive & 0x00FF);

    ucHBackHiByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth) & 0xFF00) >> 8;
    ucHBackLoByte = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth) & 0x00FF);

    for(ucPacketNum = 0; ucPacketNum < 4; ucPacketNum++)
    {
        rtd_outl(HDMITX_MISC_HDMITX_EMP_HEADER_reg ,0x007f1000);


        for(ucNum = 0; ucNum < 8; ucNum++)
        {
            uiPacketData = 0; //reset
            if(ucPPSCount < 128)
            {
                uiPacketData = ((uiPacketData + g_ppucHdmi21MacTxPPSData[_TX0][ucPPSCount + 3])  <<8);

                uiPacketData = ((uiPacketData + g_ppucHdmi21MacTxPPSData[_TX0][ucPPSCount + 2])  <<8);

                uiPacketData = ((uiPacketData + g_ppucHdmi21MacTxPPSData[_TX0][ucPPSCount + 1])  <<8);

                uiPacketData = (uiPacketData + g_ppucHdmi21MacTxPPSData[_TX0][ucPPSCount])  ;

                ucPPSCount += 4;

            }
            //DebugMessageHDMI21Tx("[HDMI21_TX] %s,ucPacketNum = %d, ucNum = %d, uiPacketData= %x\n" ,__FUNCTION__,ucPacketNum,ucNum, uiPacketData);
            rtd_outl(HDMITX_MISC_HDMITX_EMP_DATA0_reg + ucNum*4 ,uiPacketData);
        }
    }

    // Hfront : pixel
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_6_reg, ucFrontHiByte);
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_7_reg, ucFrontLoByte);

    // H sync Width : pixel
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_8_reg, ucHsyncWidthHiByte);
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_9_reg, ucHsyncWidthLoByte);

    // H back : pixel
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_10_reg, ucHBackHiByte);
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_11_reg, ucHBackLoByte);

    // H Active : pixel
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_12_reg, ucHCActiveTriHiByte);
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_HDMI_13_reg, ucHCActiveTriLoByte);

    //DebugMessageHDMI21Tx("[HDMI21_TX] %s, HDMITX21_PACKET_CONTROL_HDMI21_PPS_CTRL_reg= %x\n" ,__FUNCTION__, rtd_inl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_CTRL_reg));
    //DebugMessageHDMI21Tx("[HDMI21_TX] %s, HDMITX21_PACKET_CONTROL_HDMI21_PPS_DSC_ENC_0_reg = %x \n",__FUNCTION__,rtd_inl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_DSC_ENC_0_reg));

#endif
/*
    DebugMessageHDMI21Tx("[HDMI21_TX] EMP pack setting ucFrontHiByte =%d\n", ucFrontHiByte);
    DebugMessageHDMI21Tx("[HDMI21_TX] EMP pack setting ucFrontLoByte =%d\n", ucFrontLoByte);
    DebugMessageHDMI21Tx("[HDMI21_TX] EMP pack setting ucHsyncWidthHiByte =%d\n", ucHsyncWidthHiByte);
    DebugMessageHDMI21Tx("[HDMI21_TX] EMP pack setting ucHsyncWidthLoByte =%d\n", ucHsyncWidthLoByte);
    DebugMessageHDMI21Tx("[HDMI21_TX] EMP pack setting ORG Tri- BYTE ucHCActiveHiByte =%d\n", ucHCActiveTriHiByte);
    DebugMessageHDMI21Tx("[HDMI21_TX] EMP pack setting ORG Tri- BYTE ucHCActiveLoByte =%d\n", ucHCActiveTriLoByte);
    DebugMessageHDMI21Tx("[HDMI21_TX] EMP pack setting ucHBackHiByte =%d\n", ucHBackHiByte);
    DebugMessageHDMI21Tx("[HDMI21_TX] EMP pack setting ucHBackLoByte =%d\n", ucHBackLoByte);
*/
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_CTRL_reg, ~(_BIT7 | _BIT6 | _BIT5), (_BIT6 | _BIT5));
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_DSC_ENC_0_reg, ~(_BIT6), _BIT6);

    return;
}
#endif
//--------------------------------------------------
// Description  : Hdmi 2p1 EMP Packet Set
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------
void ScalerHdmi21MacTx0EmpPacket(void)
{
#if(_DSC_SUPPORT == _ON)
    if(GET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(_TX0) == _DOWN_STREAM_DSC)
    {
        // DSC encoder enable
        ScalerHdmi21MacTx0DscPPSPacket();

        //FatalMessageHDMITx("[HDMI21_TX] ScalerHdmi21MacTx0DscPPSPacket\n");

        //FatalMessageHDMITx("[HDMI21_TX] Read usUnvaild reg time for 2.1 LO=%x\n",rtd_inl(TIMER_SCPU_CLK90K_LO_reg));

    }
#endif // #ifdef NOT_USED_NOW
    if(0)
    {//new spec in HDMI2.1a
        //ScalerHdmi21MacTx0SBTMPacket();
    }

    if(0)
    {
        //send dynamic HDR EMP packet
        //ScalerHdmi21MacTx0DHDRPacket();
    }
    return;
}

//--------------------------------------------------
// Description  : Hdmi Tx Vendor Specific Infoframe Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0FillVendorSpecificInfoPacket(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    ucSBNum = 0x00;
    ucChecksum = 0x00;

    InfoMessageHDMITx("[HDMI_TX] Fill VSIF...\n");

    if((ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_VSIF_HDMI_ALLM) == TRUE) && (ScalerHdmiTxRepeaterGetSourceInfo_Allm() == TRUE)){
        // PB[3:1]: HF-VSIF OUI=0xc45dd8
        pucVSInfoPacketData[0] = 0xd8;
        pucVSInfoPacketData[1] = 0x5d;
        pucVSInfoPacketData[2] = 0xc4;

        // PB[4]: Version=1
        pucVSInfoPacketData[3] = 0x01;

        // PB[5]: B[0]: 3D, B[1]: ALLM_Mode
        pucVSInfoPacketData[4] = _BIT1;
    }else
    if((ScalerDpStreamSTis3DVideo(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE) || (GET_HDMI_MAC_TX0_4K_VIC_SEND_BY_VSIF() == _TRUE)){

        // Set Registration ID : 0x000C03
        pucVSInfoPacketData[0] = 0x03;

        // Set Registration ID : 0x000C03
        pucVSInfoPacketData[1] = 0x0C;

        // Set Registration ID : 0x000C03
        pucVSInfoPacketData[2] = 0x00;

        if(ScalerDpStreamSTis3DVideo(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE)
        {
            // HDMI Video Fomat field = 3D foramat
            pucVSInfoPacketData[3] = 0x40;

            // Set 3D_Structure
            pucVSInfoPacketData[4] = 0x00;
        }
        else
        {
            pStHdmiTxSearchVicTableInfo.ucSourceColorSpace = BANK6_ScalerDpStreamGetElement(ScalerHdmiMacTxPxMapping(_TX0), _COLOR_SPACE);
            memcpy((UINT8*)&pStHdmiTxSearchVicTableInfo.stInputInfo, (UINT8*)g_stHdmiMacTx0InputTimingInfo, sizeof(StructTimingInfo));
            pStHdmiTxSearchVicTableInfo.pucAspectRatio = ucAspectRatio;
            pStHdmiTxSearchVicTableInfo.pucVIC = ucVIC;
            pStHdmiTxSearchVicTableInfo.pucVIC2 = ucVIC2;
            ScalerHdmiMacTxSearchVicTable(&pStHdmiTxSearchVicTableInfo);

            if(ucVIC == 95)
            {
                ucVIC = 1;
            }
            else if(ucVIC == 94)
            {
                ucVIC = 2;
            }
            else if(ucVIC == 93)
            {
                ucVIC = 3;
            }
            else if(ucVIC == 98)
            {
                ucVIC = 4;
            }

            // HDMI Video Fomat field = Extended resolution format present
            pucVSInfoPacketData[3] = 0x20;

            // Set HDMI_VIC
            pucVSInfoPacketData[4] = ucVIC;
        }
    }

    ucChecksum -= 0x81;
    ucChecksum -= 0x01;
    ucChecksum -= 0x05;

    ucChecksum -= pucVSInfoPacketData[0];
    ucChecksum -= pucVSInfoPacketData[1];
    ucChecksum -= pucVSInfoPacketData[2];
    ucChecksum -= pucVSInfoPacketData[3];
    ucChecksum -= pucVSInfoPacketData[4];

    InfoMessageHDMITx("VSIF HD: 0x81 0x01 0x05, ");
    InfoMessageHDMITx("OUI: %x %x ", (UINT32)pucVSInfoPacketData[2], (UINT32)pucVSInfoPacketData[1]);
    InfoMessageHDMITx("%x\n", (UINT32)pucVSInfoPacketData[0]);
    InfoMessageHDMITx("PB: %x %x, ",  (UINT32)pucVSInfoPacketData[3], (UINT32)pucVSInfoPacketData[4]);
    InfoMessageHDMITx("checksum=%x\n", (UINT32)ucChecksum);

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_18_reg, 0x81); // HB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_17_reg, 0x01); // HB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_16_reg, 0x05); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_22_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_21_reg, pucVSInfoPacketData[0]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_20_reg, pucVSInfoPacketData[1]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg, pucVSInfoPacketData[2]); // WORD0: PB3

    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_26_reg, pucVSInfoPacketData[3]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_25_reg, pucVSInfoPacketData[4]); // WORD1: PB5

#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, 0x81); // HB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, 0x01); // HB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, 0x05); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, pucVSInfoPacketData[0]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_5_reg, pucVSInfoPacketData[1]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_4_reg, pucVSInfoPacketData[2]); // WORD0: PB3

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_11_reg, pucVSInfoPacketData[3]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_10_reg, pucVSInfoPacketData[4]); // WORD1: PB5

#endif
#endif//#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
}

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
UINT8 ScalerHdmiTxRepeaterGetSourceInfo_empVtem(UINT8 *empVtem)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    if(empVtem != NULL){
        memcpy(empVtem, (UINT8*)&hdmiRepeaterSrcInfo.hdmi_empVtemBuf, EM_VTEM_INFO_LEN);
        return 0;
    }
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    return 1;
}

UINT8 ScalerHdmiTxRepeaterGetSourceInfo_DvVsifInfo(UINT8 *dvVsifInfo)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    if(dvVsifInfo != NULL){
        memcpy(dvVsifInfo, (UINT8*)&hdmiRepeaterSrcInfo.hdmi_dvVsiInfo, sizeof(HDMI_VSI_T));
        return 0;
    }
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    return 1;
}


//--------------------------------------------------
// Description  : Hdmi Tx Vendor Specific Infoframe Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0FillDolbyVisionVsifPacket(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    ucSBNum = 0x00;
    ucChecksum = 0x00;

    InfoMessageHDMITx("[HDMI_TX] Fill DV VSIF...\n");

    if((ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_VSIF_DV_VSIF) == TRUE) && (ScalerHdmiTxRepeaterGetSourceInfo_DvVsifMode() != DOLBY_HDMI_VSIF_DISABLE)){
        if(ScalerHdmiTxRepeaterGetSourceInfo_DvVsifInfo(&pucDolbyVSInfoPacketData[0]) != 0){
            FatalMessageHDMITx("[HDMI_TX] Get DV VSIF Info Fail!!\n");
            return;
        }
    }else{ // DV VSIF Disable
        ucChecksum -= 0x81;
        ucChecksum -= 0x01;
        ucChecksum -= 0x1b;
        pucDolbyVSInfoPacketData[3] = ucChecksum;
    }

    InfoMessageHDMITx("HB=%x %x ", (UINT32)pucDolbyVSInfoPacketData[0], (UINT32)pucDolbyVSInfoPacketData[1]); // HB0 (0x81), HB1 (0x01)
    InfoMessageHDMITx("%x, checksum=%x\n", (UINT32)pucDolbyVSInfoPacketData[2], (UINT32)pucDolbyVSInfoPacketData[3]); // HB2 (0x1b), PB[0]: checksum // 0x4a
    InfoMessageHDMITx("OU=%x %x ", (UINT32)pucDolbyVSInfoPacketData[6], (UINT32)pucDolbyVSInfoPacketData[5]); // PB[3:2]=0x00d046 (0x00 0xd0)
    InfoMessageHDMITx("%x@DV/LL=%x\n", (UINT32)pucDolbyVSInfoPacketData[4], (UINT32)pucDolbyVSInfoPacketData[7]); // PB[1]=0x00d046 (0x46), PB[4]: DV mode(1), B[0]: LL mode(1)
    InfoMessageHDMITx("PB[6:5]=%x %x\n", (UINT32)pucDolbyVSInfoPacketData[9], (UINT32)pucDolbyVSInfoPacketData[8]); // PB[6:5]:0x0 0x3

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, 0x81); // HB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, 0x01); // HB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, 0x1b); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, pucDolbyVSInfoPacketData[3]); // WORD0: PB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, pucDolbyVSInfoPacketData[4]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_5_reg, pucDolbyVSInfoPacketData[5]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_4_reg, pucDolbyVSInfoPacketData[6]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_11_reg, pucDolbyVSInfoPacketData[7]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_10_reg, pucDolbyVSInfoPacketData[8]); // WORD1: PB5
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_9_reg, pucDolbyVSInfoPacketData[9]); // WORD1: PB6

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_15_reg, pucDolbyVSInfoPacketData[10]); // WORD2: PB7: MD0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_14_reg, pucDolbyVSInfoPacketData[11]); // WORD2: PB8: MD1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_13_reg, pucDolbyVSInfoPacketData[12]); // WORD2: PB9: MD2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_12_reg, pucDolbyVSInfoPacketData[13]); // WORD2: PB10: MD3
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_19_reg, pucDolbyVSInfoPacketData[14]); // WORD3: PB11: MD4
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_18_reg, pucDolbyVSInfoPacketData[15]); // WORD3: PB12: MD5
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_17_reg, pucDolbyVSInfoPacketData[16]); // WORD3: PB13: MD6

    return;
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
}
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
