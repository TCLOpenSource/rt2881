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
// ID Code      : hdmitx_lunchbank6_ex.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include "../include/ScalerTx.h"
#include <rbus/hdmitx_misc_reg.h>

#define _HDMI_VTEM_BYTE_MD3 14
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
static UINT8 pucEmpVtemBackup[_HDMI_VTEM_BYTE_MD3] =
{
    0x7f,                 // HB0, 0x7F (EMP)
    0xc0,                 // HB1, B[7]: First, B[6]: Last, B[5:0]: Rsvd
    0x00,                      // HB2, Sequence Index, First=0
    0x84, // PB0: B[7]: New=1, B[6]: End=0, B[5:4]: DS_Type=0, B[3]: AFR=0, B[2]: VFR=1, B[1] Sync=0, B[0] Rsvd(0)
    0x00, // PB1: Rsvd(0)
    0x01, // PB2: Organization_ID=1
    0x00, // PB3: Data_Set_Tag=1 (MSB)
    0x01, // PB4: Data_Set_Tag=1 (LSB)
    0x00, // PB5: Data_Set_Length=4(MSB)
    0x04, // PB6: Data_Set_Length=4(LSB)
    0x03, // PB7: MD0=1 (B[1]: M_CONST=0, B[0]: VRR_EN=1)
    0x00, // PB8: MD1
    0x00, // PB9: MD2
    0x00, // PB10: MD3
}; // SPD payload len=28, +3(header)
#endif
extern StructTimingInfo *g_pstHdmi21MacTxInputTimingInfo[_HDMI_21_TX_TOTAL_NUMBER];
extern StructTimingInfo *g_stHdmiMacTx0InputTimingInfo;

void lib_hdmitx_Vrrpacket_bypass_setting(void)
{
    UINT8 value_u8 = 0;
    value_u8 = (!SCRIPT_TX_SKIP_EMP_BYPASS() && ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_EMP) && (g_scaler_display_info.input_src  != 1)? 1: 0);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_emp_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_emp_en(value_u8));

}




//--------------------------------------------------
// Description  : Hdmi Tx SPD Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0FillSPDPacket(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    ucSBNum = 0;
    ucTemp = 0;
    ucChecksum = 0x00;

    InfoMessageHDMITx("[HDMI_TX] Fill SPD...\n");

    InfoMessageHDMITx("[HDMI_TX] SPD Data:");
    do
    {
        if(ucTemp % 16 == 0)
            InfoMessageHDMITx("\n");
        InfoMessageHDMITx("%x ", (UINT32)pucDPRxSPDBackup[ucTemp]);
        ucChecksum -= pucDPRxSPDBackup[ucTemp];
        ucTemp ++;
    }
    while(ucTemp < (SPD_INFO_LEN+3)); //  SPD payload len=25, +3(InfoFrame type, version, len)
    InfoMessageHDMITx(", checkSum=%x\n", (UINT32)ucChecksum);

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_18_reg, pucDPRxSPDBackup[0]); // HB0, InfoFrame type
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_17_reg, pucDPRxSPDBackup[1]); // HB1, version
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_16_reg, pucDPRxSPDBackup[2]); // HB2 Length=25

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_22_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_21_reg, pucDPRxSPDBackup[3]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_20_reg, pucDPRxSPDBackup[4); // WORD0: PB2
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg, pucDPRxSPDBackup[5]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_26_reg, pucDPRxSPDBackup[6]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_25_reg, pucDPRxSPDBackup[7]); // WORD1: PB5
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_24_reg, pucDPRxSPDBackup[8]); // WORD1: PB6

    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_30_reg, pucDPRxSPDBackup[9]); // WORD2: PB7
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_29_reg, pucDPRxSPDBackup[10]); // WORD2: PB8
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_28_reg, pucDPRxSPDBackup[11]); // WORD2: PB9
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_27_reg, pucDPRxSPDBackup[12]); // WORD2: PB10
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_34_reg, pucDPRxSPDBackup[13]); // WORD3: PB11
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_33_reg, pucDPRxSPDBackup[14]); // WORD3: PB12
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_32_reg, pucDPRxSPDBackup[15]); // WORD3: PB13

    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_38_reg, pucDPRxSPDBackup[16]); // WORD4: PB14
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_37_reg, pucDPRxSPDBackup[17]); // WORD4: PB15
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_36_reg, pucDPRxSPDBackup[18]); // WORD4: PB16
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_35_reg, pucDPRxSPDBackup[19]); // WORD4: PB17
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_42_reg, pucDPRxSPDBackup[20]); // WORD5: PB18
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_41_reg, pucDPRxSPDBackup[21]); // WORD5: PB19
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_40_reg, pucDPRxSPDBackup[22]); // WORD5: PB20

    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_46_reg, pucDPRxSPDBackup[23]); // WORD6: PB21
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_45_reg, pucDPRxSPDBackup[24]); // WORD6: PB22
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_44_reg, pucDPRxSPDBackup[25]); // WORD6: PB23
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_43_reg, pucDPRxSPDBackup[26]); // WORD6: PB24
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_50_reg, pucDPRxSPDBackup[27]); // WORD7: PB25

#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, pucDPRxSPDBackup[0]); // HB0, InfoFrame type
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, pucDPRxSPDBackup[1]); // HB1, version
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, pucDPRxSPDBackup[2]); // HB2 Length=26


    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, pucDPRxSPDBackup[3]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_5_reg, pucDPRxSPDBackup[4]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_4_reg, pucDPRxSPDBackup[5]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_11_reg, pucDPRxSPDBackup[6]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_10_reg, pucDPRxSPDBackup[7]); // WORD1: PB5
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_9_reg, pucDPRxSPDBackup[8]); // WORD1: PB6

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_15_reg, pucDPRxSPDBackup[9]); // WORD2: PB7
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_14_reg, pucDPRxSPDBackup[10]); // WORD2: PB8
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_13_reg, pucDPRxSPDBackup[11]); // WORD2: PB9
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_12_reg, pucDPRxSPDBackup[12]); // WORD2: PB10
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_19_reg, pucDPRxSPDBackup[13]); // WORD3: PB11
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_18_reg, pucDPRxSPDBackup[14]); // WORD3: PB12
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_17_reg, pucDPRxSPDBackup[15]); // WORD3: PB13

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_23_reg, pucDPRxSPDBackup[16]); // WORD4: PB14
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_22_reg, pucDPRxSPDBackup[17]); // WORD4: PB15
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_21_reg, pucDPRxSPDBackup[18]); // WORD4: PB16
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_20_reg, pucDPRxSPDBackup[19]); // WORD4: PB17
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_27_reg, pucDPRxSPDBackup[20]); // WORD5: PB18
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_26_reg, pucDPRxSPDBackup[21]); // WORD5: PB19
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_25_reg, pucDPRxSPDBackup[22]); // WORD5: PB20

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_31_reg, pucDPRxSPDBackup[23]); // WORD6: PB21
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_30_reg, pucDPRxSPDBackup[24]); // WORD6: PB22
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_29_reg, pucDPRxSPDBackup[25]); // WORD6: PB23
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_28_reg, pucDPRxSPDBackup[26]); // WORD6: PB24
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_35_reg, pucDPRxSPDBackup[27]); // WORD7: PB25

#endif // #elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    return;
#endif//#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
}

//--------------------------------------------------
// Description  : Hdmi Tx VTEM Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0FillVtemPacket(void)
{
    UINT8 ucSBNum = 0;
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT

    UINT8  ret=0;

    #define EMP_INFO_LEN 28
    ucTemp = 0;
    ucSBNum = 0;

    InfoMessageHDMITx("[HDMI_TX] Fill VTEM...\n");

    pucEmpVtemBackup[0xa] = (ScalerHdmiTxGetVRRFlagEn()? 0x3: 0x0); // // PB7: MD0=1 (B[1]: M_CONST=0, B[0]: VRR_EN=1)

#if 1//#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
    if(ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_EMP_VTEM_VRR) && ScalerHdmiTxRepeaterGetSourceInfo_vtemVrrEn()){
        value_u8 = ScalerHdmiTxRepeaterGetSourceInfo_empVtem(&hdmiVtemPacket[0]);
        NoteMessageHDMI21Tx("[HDMI_TX] Get VTEM in Through Mode =%d\n", (UINT32)value_u8);
        if(value_u8 == 0){
            memcpy(&pucEmpVtemBackup[0], &hdmiVtemPacket[0], _HDMI_VTEM_BYTE_MD3);
        }
    }
#endif

    InfoMessageHDMITx("[HDMI_TX] VTEM Data: MD0(VRR_EN)=%x\n", (UINT32) pucEmpVtemBackup[0xa]);
    do
    {
        InfoMessageHDMITx("%x ", (UINT32)pucEmpVtemBackup[ucTemp]);
        //ucChecksum -= pucEmpVtemBackup[ucTemp];
        ucTemp ++;
    }
    while(ucTemp < (_HDMI_VTEM_BYTE_MD3/*EMP_INFO_LEN+3*/)); // SPD payload len=25, +3(InfoFrame type, version, len)
    InfoMessageHDMITx("\n");
#endif//#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, 0x7f); // HB0: EMP type (0x7f)
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, 0xc0); // HB1, B[7]: First, B[6]: Last, B[5:0]: Rsvd
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, 0x00); // HB2, Sequence Index, First=0


    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, 0x84); // PB0: B[7]: New=1, B[6]: End=0, B[5:4]: DS_Type=0, B[3]: AFR=0, B[2]: VFR=1, B[1] Sync=0, B[0] Rsvd(0)
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, 0x00); // PB1: Rsvd(0)
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_5_reg, 0x01); // PB2: Organization_ID=1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_4_reg, 0x00); // PB3: Data_Set_Tag=1 (MSB)

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_11_reg, 0x01); // PB4: Data_Set_Tag=1 (LSB)
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_10_reg, 0x00); // PB5: Data_Set_Length=4(MSB)
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_9_reg, 0x04);  // PB6: Data_Set_Length=4(LSB)

    if(g_stHdmiMacTx0InputTimingInfo == NULL) {
        rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_15_reg, 0x01); // PB7: MD0=1 (B[1]: M_CONST=0, B[0]: VRR_EN=1)
        rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_14_reg, 0x00); // B[7:0]: base VFront[7:0]
        rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_13_reg, 0x00); // B[2] RB, B[1:0]: base frame rate[9:8]
        rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_12_reg, 0x00); // B[7:0]: base frame rate[7:0]
    } else {
        rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_15_reg, 0x01); // PB7: MD0=1 (B[1]: M_CONST=0, B[0]: VRR_EN=1)
        ucSBNum  = (UINT8)(g_stHdmiMacTx0InputTimingInfo->usVTotal -g_stHdmiMacTx0InputTimingInfo->usVHeight - g_stHdmiMacTx0InputTimingInfo->usVStart);
        rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_14_reg, ucSBNum); // B[7:0]: base VFront[7:0]
        ucSBNum = ((g_stHdmiMacTx0InputTimingInfo->usVFreq / 10) >> 8) & 0x3;
        rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_13_reg, ucSBNum); // B[2] RB, B[1:0]: base frame rate[9:8]
        ucSBNum = (g_stHdmiMacTx0InputTimingInfo->usVFreq / 10) & 0xff;
        rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_12_reg, ucSBNum); // B[7:0]: base frame rate[7:0]
    }
#endif // #if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    return;

}

//--------------------------------------------------
// Description  : HDMI2.1 Tx VTEM VRR Enable setting
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmi21MacTx0FillVtemPacket(void)
{
    UINT8 ucNum = 0;
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    InfoMessageHDMITx("[HDMI_TX21] ScalerHdmi21MacTx0FillVtemPacket\n");

    pucEmpVtemBackup[0xa] = (ScalerHdmiTxGetVRRFlagEn()? 0x3: 0x0); // // PB7: MD0=1 (B[1]: M_CONST=0, B[0]: VRR_EN=1)

    if(ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_EMP_VTEM_VRR) && ScalerHdmiTxRepeaterGetSourceInfo_vtemVrrEn()){
        #define _HDMI_VTEM_BYTE_MD3 14
        UINT8 hdmiVtemPacket[_HDMI_VTEM_BYTE_MD3]={0,0,0,0,0,0,0,0,0,0,0,0,0};
        temp_value8 = ScalerHdmiTxRepeaterGetSourceInfo_empVtem(&hdmiVtemPacket[0]);
        NoteMessageHDMI21Tx("[HDMI_TX] Get VTEM in Through Mode %s\n", temp_value8 == 0? "Success": "Fail!");
        if(temp_value8 == 0){
            memcpy(&pucEmpVtemBackup[0], &hdmiVtemPacket[0], _HDMI_VTEM_BYTE_MD3);
        }
    }

#if 0
    NoteMessageHDMITx("[HDMI_TX] HB=%x %x %x\n", pucEmpVtemBackup[0], pucEmpVtemBackup[1], pucEmpVtemBackup[2]);
    NoteMessageHDMITx("[HDMI_TX] PB[7:0]=%x %x %x %x %x %x %x %x\n",
                                            pucEmpVtemBackup[10], pucEmpVtemBackup[9], pucEmpVtemBackup[8], pucEmpVtemBackup[7],
                                            pucEmpVtemBackup[6], pucEmpVtemBackup[5], pucEmpVtemBackup[4], pucEmpVtemBackup[3]);
    NoteMessageHDMITx("[HDMI_TX] PB[10:7]=%x %x %x\n", pucEmpVtemBackup[13], pucEmpVtemBackup[12], pucEmpVtemBackup[11]);
#endif
    do
    {
        DebugMessageHDMITx("[HDMI_TX21] VTEM Data[%d]=%x\n",(UINT32)ucNum, (UINT32)pucEmpVtemBackup[ucNum]);
        //ucChecksum -= pucEmpVtemBackup[ucTemp];
        ucNum ++;
    }
    while(ucNum < (11+3)); // SPD payload len=25, +3(InfoFrame type, version, len)
#endif//#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    // Pacekt Header
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_3_reg, 0x7f); // HB0: EMP type (0x7f)
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_2_reg, 0xc0); // HB1, B[7]: First, B[6]: Last, B[5:0]: Rsvd
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_1_reg, 0x00); // HB2, Sequence Index, First=0

    // Clear PB Data
    for(ucNum = 0; ucNum < 32; ucNum++)  // PB0-25 + 1(checksum)
    {
        rtd_outl((HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg + ucNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_7_reg, 0x84); // PB0: B[7]: New=1, B[6]: End=0, B[5:4]: DS_Type=0, B[3]: AFR=0, B[2]: VFR=1, B[1] Sync=0, B[0] Rsvd(0)
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_6_reg, 0x00); // PB1: Rsvd(0)
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_5_reg, 0x01); // PB2: Organization_ID=1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg, 0x00); // PB3: Data_Set_Tag=1 (MSB)
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_11_reg, 0x01); // PB4: Data_Set_Tag=1 (LSB)
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_10_reg, 0x00); // PB5: Data_Set_Length=4(MSB)
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_9_reg, 0x04); // PB6: Data_Set_Length=4(LSB)

    if(g_pstHdmi21MacTxInputTimingInfo[_TX0] == NULL) {
        rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_15_reg, 0x01); // PB7: MD0=1 (B[1]: M_CONST=0, B[0]: VRR_EN=1)
        rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_14_reg, 0x00);  // B[7:0]: base VFront[7:0]
        rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_13_reg, 0x00); // B[2] RB, B[1:0]: base frame rate[9:8]
        rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_12_reg, 0x00); // B[7:0]: base frame rate[7:0]
    } else {
        rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_15_reg, 0x01); // PB7: MD0=1 (B[1]: M_CONST=0, B[0]: VRR_EN=1)
        ucNum  = (UINT8)(g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVTotal -g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVStart);
        rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_14_reg, ucNum);  // B[7:0]: base VFront[7:0]
        ucNum = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVFreq / 10) >> 8) & 0x3;
        rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_13_reg, ucNum); // B[2] RB, B[1:0]: base frame rate[9:8]
        ucNum = (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVFreq / 10) & 0xff;
        rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_12_reg, ucNum); // B[7:0]: base frame rate[7:0]
    }
    return;
}

UINT8 ScalerHdmiTxRepeaterGetSourceInfo_hdrDrmInfo(UINT8 *hdmi_drmInfo)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    if(hdmi_drmInfo != NULL){
        memcpy(hdmi_drmInfo, (UINT8*)&hdmiRepeaterSrcInfo.hdmi_drmInfo, sizeof(HDMI_DRM_T));
        return 0;
    }
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    return 1;
}

#if(_HDMI_FREESYNC_TX0_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Freesync Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0FillFreesyncPacket(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    ucSBNum = 0;
    ucTemp = 0;
    ucChecksum = 0x00;

    ucStreamIndex = ScalerDpStreamPxSTMapping(ScalerHdmiMacTxPxMapping(_TX0));

    InfoMessageHDMITx("[HDMI_TX] Fill FreeSync...\n");

    // Get Packet data from ST
    ScalerDpStreamDpSdpGetData(ucStreamIndex, _DP_INFO_FREESYNC, pucDPRxFreesyncBackup);

    do
    {
        DebugMessageHDMITx("[HDMI_TX] Freesync Data=%x\n", (UINT32)pucDPRxFreesyncBackup[ucTemp]);
        ucChecksum -= pucDPRxFreesyncBackup[ucTemp];
        ucTemp ++;
    }
    while(ucTemp < (GET_DP_STREAM_FREESYNC_INFO_FM_LENGTH(ucStreamIndex) + 2));

    ucChecksum -= 0x83;

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_18_reg, 0x83); // HB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_17_reg, GET_DP_STREAM_FREESYNC_INFO_FM_VERSION(ucStreamIndex)); // HB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_16_reg, GET_DP_STREAM_FREESYNC_INFO_FM_LENGTH(ucStreamIndex)); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_22_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_21_reg, pucDPRxFreesyncBackup[2]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_20_reg, pucDPRxFreesyncBackup[3]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg, pucDPRxFreesyncBackup[4]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_26_reg, pucDPRxFreesyncBackup[5]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_25_reg, pucDPRxFreesyncBackup[6]); // WORD1: PB5
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_24_reg, pucDPRxFreesyncBackup[7]); // WORD1: PB6

    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_30_reg, pucDPRxFreesyncBackup[8]); // WORD2: PB7
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_29_reg, pucDPRxFreesyncBackup[9]); // WORD2: PB8

    if(GET_DP_STREAM_FREESYNC_INFO_FM_LENGTH(ucStreamIndex) == 10)
    {
        rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_28_reg, pucDPRxFreesyncBackup[10]); // WORD2: PB9
        rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_27_reg, pucDPRxFreesyncBackup[11]); // WORD2: PB10
    }

#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, 0x83); // HB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, GET_DP_STREAM_FREESYNC_INFO_FM_VERSION(ucStreamIndex)); // HB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, GET_DP_STREAM_FREESYNC_INFO_FM_LENGTH(ucStreamIndex)); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, pucDPRxFreesyncBackup[2]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_5_reg, pucDPRxFreesyncBackup[3]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_4_reg, pucDPRxFreesyncBackup[4]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_11_reg, pucDPRxFreesyncBackup[5]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_10_reg, pucDPRxFreesyncBackup[6]); // WORD1: PB5
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_9_reg, pucDPRxFreesyncBackup[7]); // WORD1: PB6

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_15_reg, pucDPRxFreesyncBackup[8]); // WORD2: PB7
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_14_reg, pucDPRxFreesyncBackup[9]); // WORD2: PB8

    if(GET_DP_STREAM_FREESYNC_INFO_FM_LENGTH(ucStreamIndex) == 10)
    {
        rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_13_reg, pucDPRxFreesyncBackup[10]); // WORD2: PB9
        rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_12_reg, pucDPRxFreesyncBackup[11]); // WORD2: PB10
    }

#endif // #elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
#endif//#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
}
#endif // End of #if(_HDMI_FREESYNC_TX0_SUPPORT == _ON)
#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx HDR Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0FillHDRPacket(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    ucSBNum = 0;
    ucTemp = 0;
    ucChecksum = 0x00;

    SLR_VOINFO *pVOInfoNew = NULL;
    seamless_change_sync_info *seamless_info = NULL;

//#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
    HDMI_DRM_T hdmi_drm;
    // bypass HDR metadata
    if((ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_DRM_HDR) == TRUE) && (ScalerHdmiTxRepeaterGetSourceInfo_hdrMode() == HDR_HDR10_HDMI)){
        if(ScalerHdmiTxRepeaterGetSourceInfo_hdrDrmInfo((UINT8 *)&hdmi_drm) != 0){
            FatalMessageHDMITx("[HDMI_TX] Get HDMI DRM DM Fail!!\n");
            return;
        }else{
            memcpy(&pucDPRxHDRBackup[0], ((UINT8*)&hdmi_drm)+1, 28);
        }
    }else
//#endif // #ifdef CONFIG_HDMITX_REPEATER_SUPPORT
    {
        seamless_info = (seamless_change_sync_info *)Scaler_GetShareMemVirAddr(SCALERIOC_SEAMLESS_CHANGE_SHAREMEMORY_INFO_SYNC_FLAG);
        if(seamless_info == NULL)
        {
            FatalMessageHDMITx("[HDMI_TX] seamless_info NULL\n");
            return;
        }
        hdrtype = Scaler_ChangeUINT32Endian(seamless_info->vdec_cur_hdr_type);

        DebugMessageHDMITx("[HDMI_TX] ScalerHdmiMacTx0FillHDRPacket()\n");

        pVOInfoNew = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
        ucStreamIndex = ScalerDpStreamPxSTMapping(ScalerHdmiMacTxPxMapping(_TX0));
        if(hdrtype && ((pVOInfoNew == NULL)|| (pHDMITX_VOInfo == NULL)))
        {
            ErrorMessageHDMI21Tx("[HDMI_TX] VO/HDMI Info NULL(%x/%x)\n", (UINT32)pVOInfoNew, (UINT32)pHDMITX_VOInfo);
            return;
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
            pucDPRxHDRBackup[2] = 0; // PB[1]  SDR EOTF =0
            pucDPRxHDRBackup[3] = 0; // PB[2] Static_Metadata_Descriptor_ID (3 bits)
            pucDPRxHDRBackup[4] = 0; // PB[3] Static_Metadata_Descriptor
        }
    }

#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
    InfoMessageHDMI21Tx("[HDMI_TX]  Repeater Mode[%d@%d], DRM T/V/L[%x/%x/%x] EOTF=%d\n",
            (UINT32)get_hdmitxOutputMode(), (UINT32)ScalerHdmiTxRepeaterGetSourceInfo_hdrMode(),
            (UINT32)hdmi_drm.type_code, (UINT32)hdmi_drm.ver, (UINT32)hdmi_drm.len, (UINT32)hdmi_drm.eEOTFtype);
#endif // #ifdef CONFIG_HDMITX_REPEATER_SUPPORT
    InfoMessageHDMI21Tx("[HDMI_TX]  pucAVIInfoPacketData[0]=%x\n", (UINT32)pucDPRxHDRBackup[0]);
    InfoMessageHDMI21Tx("[HDMI_TX]  pucAVIInfoPacketData[1]=%x\n", (UINT32)pucDPRxHDRBackup[1]);
    InfoMessageHDMI21Tx("[HDMI_TX]  pucAVIInfoPacketData[2]=%x\n", (UINT32)pucDPRxHDRBackup[2]);
    InfoMessageHDMI21Tx("[HDMI_TX]  pucAVIInfoPacketData[3]=%x\n", (UINT32)pucDPRxHDRBackup[3]);
    InfoMessageHDMI21Tx("[HDMI_TX]  pucAVIInfoPacketData[4]=%x\n", (UINT32)pucDPRxHDRBackup[4]);

    do
    {
        DebugMessageHDMITx("[HDMI_TX] HDR Data %d =%x\n", (UINT32)ucTemp, (UINT32)pucDPRxHDRBackup[ucTemp]);
        ucChecksum -= pucDPRxHDRBackup[ucTemp];
        ucTemp ++;
    }
    while(ucTemp < 28);

    if(last_hdrType != hdrtype){
       InfoMessageHDMITx("[HDMI_TX] HDR type[%d->%d]\n", (UINT32)last_hdrType, (UINT32)hdrtype);
       last_hdrType = hdrtype;
    }

    ucChecksum -= 0x87;

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_18_reg, 0x87); // HB0
    //rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_17_reg, GET_DP_STREAM_HDR10_INFO_FM_VERSION(ucStreamIndex)); // HB1
    //rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_16_reg, GET_DP_STREAM_HDR10_INFO_FM_LENGTH(ucStreamIndex)); // HB2 Length=26
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_17_reg, pucDPRxHDRBackup[0]); // HB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_16_reg, pucDPRxHDRBackup[1]); // HB2 Length=26

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_22_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_21_reg, pucDPRxHDRBackup[2]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_20_reg, pucDPRxHDRBackup[3]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg, pucDPRxHDRBackup[4]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_26_reg, pucDPRxHDRBackup[5]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_25_reg, pucDPRxHDRBackup[6]); // WORD1: PB5
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_24_reg, pucDPRxHDRBackup[7]); // WORD1: PB6

    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_30_reg, pucDPRxHDRBackup[8]); // WORD2: PB7
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_29_reg, pucDPRxHDRBackup[9]); // WORD2: PB8
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_28_reg, pucDPRxHDRBackup[10]); // WORD2: PB9
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_27_reg, pucDPRxHDRBackup[11]); // WORD2: PB10
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_34_reg, pucDPRxHDRBackup[12]); // WORD3: PB11
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_33_reg, pucDPRxHDRBackup[13]); // WORD3: PB12
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_32_reg, pucDPRxHDRBackup[14]); // WORD3: PB13

    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_38_reg, pucDPRxHDRBackup[15]); // WORD4: PB14
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_37_reg, pucDPRxHDRBackup[16]); // WORD4: PB15
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_36_reg, pucDPRxHDRBackup[17]); // WORD4: PB16
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_35_reg, pucDPRxHDRBackup[18]); // WORD4: PB17
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_42_reg, pucDPRxHDRBackup[19]); // WORD5: PB18
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_41_reg, pucDPRxHDRBackup[20]); // WORD5: PB19
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_40_reg, pucDPRxHDRBackup[21]); // WORD5: PB20

    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_46_reg, pucDPRxHDRBackup[22]); // WORD6: PB21
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_45_reg, pucDPRxHDRBackup[23]); // WORD6: PB22
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_44_reg, pucDPRxHDRBackup[24]); // WORD6: PB23
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_43_reg, pucDPRxHDRBackup[25]); // WORD6: PB24
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_50_reg, pucDPRxHDRBackup[26]); // WORD7: PB25
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_49_reg, pucDPRxHDRBackup[27]); // WORD7: PB26

#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, 0x87); // HB0
    //rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, GET_DP_STREAM_HDR10_INFO_FM_VERSION(ucStreamIndex)); // HB1
    //rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, GET_DP_STREAM_HDR10_INFO_FM_LENGTH(ucStreamIndex)); // HB2 Length=26
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, pucDPRxHDRBackup[0]); // HB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, pucDPRxHDRBackup[1]); // HB2 Length=26


    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, pucDPRxHDRBackup[2]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_5_reg, pucDPRxHDRBackup[3]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_4_reg, pucDPRxHDRBackup[4]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_11_reg, pucDPRxHDRBackup[5]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_10_reg, pucDPRxHDRBackup[6]); // WORD1: PB5
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_9_reg, pucDPRxHDRBackup[7]); // WORD1: PB6

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_15_reg, pucDPRxHDRBackup[8]); // WORD2: PB7
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_14_reg, pucDPRxHDRBackup[9]); // WORD2: PB8
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_13_reg, pucDPRxHDRBackup[10]); // WORD2: PB9
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_12_reg, pucDPRxHDRBackup[11]); // WORD2: PB10
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_19_reg, pucDPRxHDRBackup[12]); // WORD3: PB11
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_18_reg, pucDPRxHDRBackup[13]); // WORD3: PB12
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_17_reg, pucDPRxHDRBackup[14]); // WORD3: PB13

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_23_reg, pucDPRxHDRBackup[15]); // WORD4: PB14
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_22_reg, pucDPRxHDRBackup[16]); // WORD4: PB15
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_21_reg, pucDPRxHDRBackup[17]); // WORD4: PB16
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_20_reg, pucDPRxHDRBackup[18]); // WORD4: PB17
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_27_reg, pucDPRxHDRBackup[19]); // WORD5: PB18
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_26_reg, pucDPRxHDRBackup[20]); // WORD5: PB19
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_25_reg, pucDPRxHDRBackup[21]); // WORD5: PB20

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_31_reg, pucDPRxHDRBackup[22]); // WORD6: PB21
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_30_reg, pucDPRxHDRBackup[23]); // WORD6: PB22
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_29_reg, pucDPRxHDRBackup[24]); // WORD6: PB23
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_28_reg, pucDPRxHDRBackup[25]); // WORD6: PB24
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_35_reg, pucDPRxHDRBackup[26]); // WORD7: PB25
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_34_reg, pucDPRxHDRBackup[27]); // WORD7: PB26

#endif // #elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
}
//#endif // End of #if(_HDMI_HDR10_TX0_SUPPORT == _ON)

#elif defined(CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT)
//--------------------------------------------------
// Description  : Hdmi Tx HDR Packet for ZEBU platform quick DRM InfoFrame packet verify
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0FillHDRPacket_ZebuTest(void)
{
    ucSBNum = 0;
    ucTemp = 0;
    ucChecksum = 0x00;

    // bypass HDR metadata
    if((ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_DRM_HDR) == TRUE) && (ScalerHdmiTxRepeaterGetSourceInfo_hdrMode() == HDR_HDR10_HDMI)){
        if(ScalerHdmiTxRepeaterGetSourceInfo_hdrDrmInfo((UINT8 *)&hdmi_drm) != 0){
            FatalMessageHDMITx("[HDMI_TX] Get HDMI DRM DM Fail!!\n");
            return;
        }else{
            memcpy(&pucDPRxHDRBackup[0], ((UINT8*)&hdmi_drm)+1, 28);
        }
    }

    DebugMessageHDMI21Tx("[HDMI_TX]  Repeater/HDR Mode[%d@%d]\n", (UINT32)get_hdmitxOutputMode(), (UINT32)ScalerHdmiTxRepeaterGetSourceInfo_hdrMode());
    DebugMessageHDMI21Tx("DRM T/V/L[%x/%x", (UINT32)hdmi_drm.type_code, (UINT32)hdmi_drm.ver);
    DebugMessageHDMI21Tx("/%x] EOTF=%d\n", (UINT32)hdmi_drm.len, (UINT32)hdmi_drm.eEOTFtype);

    InfoMessageHDMITx("[HDMI_TX] Fill HDR[%d] Data: \n0x87 ", (UINT32)ScalerHdmiTxRepeaterGetSourceInfo_hdrMode());
    do
    {
        InfoMessageHDMITx("%x ", (UINT32)pucDPRxHDRBackup[ucTemp]);
        ucChecksum -= pucDPRxHDRBackup[ucTemp];
        ucTemp ++;
        if((ucTemp % 16) == 0)
            InfoMessageHDMITx("\n");
    }
    while(ucTemp < 28);
    ucChecksum -= 0x87;
    InfoMessageHDMI21Tx(", CheckSum=%x\n", (UINT32)ucChecksum);

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, 0x87); // HB0
    //rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, GET_DP_STREAM_HDR10_INFO_FM_VERSION(ucStreamIndex)); // HB1
    //rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, GET_DP_STREAM_HDR10_INFO_FM_LENGTH(ucStreamIndex)); // HB2 Length=26
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, pucDPRxHDRBackup[0]); // HB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, pucDPRxHDRBackup[1]); // HB2 Length=26


    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, pucDPRxHDRBackup[2]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_5_reg, pucDPRxHDRBackup[3]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_4_reg, pucDPRxHDRBackup[4]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_11_reg, pucDPRxHDRBackup[5]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_10_reg, pucDPRxHDRBackup[6]); // WORD1: PB5
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_9_reg, pucDPRxHDRBackup[7]); // WORD1: PB6

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_15_reg, pucDPRxHDRBackup[8]); // WORD2: PB7
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_14_reg, pucDPRxHDRBackup[9]); // WORD2: PB8
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_13_reg, pucDPRxHDRBackup[10]); // WORD2: PB9
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_12_reg, pucDPRxHDRBackup[11]); // WORD2: PB10
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_19_reg, pucDPRxHDRBackup[12]); // WORD3: PB11
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_18_reg, pucDPRxHDRBackup[13]); // WORD3: PB12
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_17_reg, pucDPRxHDRBackup[14]); // WORD3: PB13

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_23_reg, pucDPRxHDRBackup[15]); // WORD4: PB14
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_22_reg, pucDPRxHDRBackup[16]); // WORD4: PB15
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_21_reg, pucDPRxHDRBackup[17]); // WORD4: PB16
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_20_reg, pucDPRxHDRBackup[18]); // WORD4: PB17
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_27_reg, pucDPRxHDRBackup[19]); // WORD5: PB18
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_26_reg, pucDPRxHDRBackup[20]); // WORD5: PB19
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_25_reg, pucDPRxHDRBackup[21]); // WORD5: PB20

    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_31_reg, pucDPRxHDRBackup[22]); // WORD6: PB21
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_30_reg, pucDPRxHDRBackup[23]); // WORD6: PB22
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_29_reg, pucDPRxHDRBackup[24]); // WORD6: PB23
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_28_reg, pucDPRxHDRBackup[25]); // WORD6: PB24
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_35_reg, pucDPRxHDRBackup[26]); // WORD7: PB25
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_34_reg, pucDPRxHDRBackup[27]); // WORD7: PB26
}
#endif // #elif defined(CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT)
