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
// ID Code      : ScalerHdmi21MacTx0FillOacjet.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_driver.h>

//#include "scaler/scalerCommon.h"

//#include <rbus/ppoverlay_reg.h>
//#include <rbus/ppoverlay_outtg_reg.h>
#include <rbus/ppoverlay_txpretg_reg.h>

//#include "../include/ScalerFunctionInclude.h"
//#include "../include/HdmiMacTx/ScalerHdmiMacTx.h"
//#include "../include/HdmiPhyTx/ScalerHdmiPhyTxInclude.h"
//#include "../include/HdmiMacTx/ScalerHdmiMacTxInclude.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0Include.h"
//#include "../include/HdmiMacTx0/ScalerHdmiMacTx0.h"
#include "../include/Hdmi21MacTx/ScalerHdmi21MacTxInclude.h"
#include "../include/Hdmi21MacTx0/ScalerHdmi21MacTx0Include.h"
#include "../include/Hdmi21MacTx0/ScalerHdmi21MacTx0.h"
#include "../include/ScalerTx.h"

//#include "../include/hdmitx_phy_struct_def.h"
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
//#include <rtk_kdriver/tvscalercontrol/hdmirx/drvif_hdmirx.h>
#include <rtk_kdriver/measure/rtk_measure.h>

#endif
#ifdef CONFIG_ENABLE_DPRX
#include <rtk_kdriver/dprx/drvif_dprx.h>
#endif


//****************************************************************************
// Macro/Define
//****************************************************************************

extern UINT32 temp_value32;// = 0;
extern UINT16 temp_value16;// = 0;

UINT8 bHdmi21SwPktSramInit=0;

#define EMP_INFO_LEN 28
#define SPD_INFO_LEN 25
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
UINT8 pucEmpVtemBackup[11+3] =
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

#endif //#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
//static UINT8 ucNum = 0;
//static UINT8 ucPacketNum = 0;

extern UINT8 ucTemp;
extern UINT8 pucAVIInfoPacketData[AVI_INFO_LEN];
extern UINT8 pucEdidVDBVIC[31];
extern UINT8 pucEdid420VDBVIC[31];
extern UINT16 pusCtaDataBlockAddr[_CTA_TOTAL_DB + 1];




//static UINT16 vStart1, vStart2, vLen, lineCnt1;
//static UINT8 ret, outMode;

//****************************************************************************
// extern variable
//****************************************************************************
extern UINT32 stc;
extern UINT8 ucPreAVIChecksum;
extern UINT8 need_sw_avi_packet;
//****************************************************************************
// CODE TABLES
//****************************************************************************


//****************************************************************************
// local variable declaration
//****************************************************************************
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
extern UINT32  hdmiRepeaterSwPktEnMask;
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
extern StructHdmitxDisplayInfo g_scaler_display_info;
extern StructHdmiTxSearchVicTableInfo pStHdmiTxSearchVicTableInfo;
extern BOOLEAN g_bHdmi21MacTx0SWPktDFF0Used;
extern BOOLEAN g_bHdmi21MacTx0SWPktDFF1Used;

//extern UINT8 bForceLinkTrainingFlag;
extern UINT8 ucVTEMpacketEn;
extern UINT8 ucVRRFlagEn;
extern UINT8 ucTxHDRFlag;
extern MEASURE_TIMING_T timing_info;
extern UINT16 ScalerDpStreamGetElement(EnumOutputPort ucOutputPort, EnumMultiStreamElement enElement);
extern void ScalerHdmi21MacTx0FillRXAVIInfoPacket(void);
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
extern UINT8 ScalerHdmiTxRepeaterGetSourceInfo_DvVsifInfo(UINT8 *dvVsifInfo);
extern UINT8 ScalerHdmiTxRepeaterGetSourceInfo_empVtem(UINT8 *empVtem);
#endif
extern UINT8 HDMITX_wait_for_tgLineCount(UINT32 start, UINT32 end, UINT8 wait_zero);

//--------------------------------------------------
// Description  : wait scaler status/event
// Input Value  :
// Output Value :
//--------------------------------------------------
UINT8 Scaler_Wait_for_event_bank8_cloneIF(StructEventPollingInfo *eventInfo)
{
    UINT8 count = 0;
    UINT32 timeout = 0x1493E0;

    if(eventInfo == NULL){
        DebugMessageHDMITx("[HDMITX][ERR] NULL PTR@Scaler_Wait_for_event\n");
        return FALSE;
    }

#ifdef FIX_ME_HDMITX_BRING_UP // [TBD]
    //InfoMessageHDMITx("[HDMITX] Scaler_Wait_for_event[%x], Mask[%x]\n", (UINT32)eventInfo->addr, (UINT32)eventInfo->mask);
#endif

    do{
        if(rtd_inl(eventInfo->addr) & eventInfo->mask){
            rtd_maskl(eventInfo->addr, ~eventInfo->mask, eventInfo->mask);//clear
            count++;
        }

        if(count >= eventInfo->timecount)
        {
            //InfoMessageHDMITx("[HDMITX] Wait Done, Count[%d]@Timeout=%d\n", (UINT32)count, (UINT32)timeout);
            return TRUE;
        }
    }while(--timeout);

    FatalMessageHDMITx("[HDMITX][WARN] Pending_status timeout!@Addr=%x, Event=%x\n", (UINT32)eventInfo->addr, (UINT32)eventInfo->mask);
    return FALSE;
}

UINT8 ScalerHdmiTx_Wait_VerticalDataEnable(void)
{
    UINT16 vStart,vLen,vStart1,vStart2,ret;

    vStart = PPOVERLAY_TXPRETG_TXPRETG_DV_DEN_Start_End_get_dv_den_sta(rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_DV_DEN_Start_End_reg)); 
    vLen = PPOVERLAY_TXPRETG_TXPRETG_DV_DEN_Start_End_get_dv_den_end(rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_DV_DEN_Start_End_reg)) - vStart;
    //lineCnt1 = PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_get_line_cnt_rt(rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_reg));
    vStart1 = vStart+2; // v-start+2
    vStart2 = ((vLen>>3)); // 1/8 v-end
    ret = (vStart1 >= vStart2? FALSE: HDMITX_wait_for_tgLineCount(vStart1,vStart2,0));
    if(ret == FALSE)
        FatalMessageHDMITx("[HDMI21_TX] Wait VDE FAIL!!");
    else
        DebugMessageHDMITx("[HDMI21_TX] Wait VDE Pass");

    return ret;
}

//--------------------------------------------------
// Description  : Hdmi 2p1 SW Packet Set
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTx0PeriodicSRAMPacking(void)
{
    UINT32 tx21SwPktEnMask=0;
    EnumHDMITxPacketType enumHdmiPktType = _HDMI_TX_NULL;
    // opcr_rprwcmd=1  Write Mode
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~(_BIT2 | _BIT1), _BIT2);

    // HW bypass REG table to periodic SRAM directly
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~(_BIT6 | _BIT1), 0x00);

    // HDMI21 SW Packet(0~9) SRAM init
    if(bHdmi21SwPktSramInit == 0){
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT0_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI21_TX] SW PKT0 Init Fail!!\n");
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT1_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI21_TX] SW PKT1 Init Fail!!\n");
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT2_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI21_TX] SW PKT2 Init Fail!!\n");
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT3_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI21_TX] SW PKT3 Init Fail!!\n");
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT4_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI21_TX] SW PKT4 Init Fail!!\n");
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT5_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI21_TX] SW PKT5 Init Fail!!\n");
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT6_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI21_TX] SW PKT6 Init Fail!!\n");
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT7_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI21_TX] SW PKT7 Init Fail!!\n");
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT8_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI21_TX] SW PKT8 Init Fail!!\n");
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT9_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI21_TX] SW PKT9 Init Fail!!\n");
        bHdmi21SwPktSramInit = 1;
        DebugMessageHDMITx("[HDMI21_TX] HDMI21 SW PKT Init Done!!\n");
    }


#if !defined(ENABLE_INTERNAL_HDMIRX_PTG) && !defined(SCALER_AUTO_RUN)
    if((get_hdmitxOutputMode() != TX_OUT_THROUGH_MODE) || (SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE) || (g_scaler_display_info.input_src != 0) || need_sw_avi_packet)
#endif
    { // enable SW packet only when not enable RX AVI info frame bypass to TX
        // Packet0: AVI Inforframe Packet
        ErrorMessageHDMITx("[HDMI_TX] AVI InfoFrame !!\n");
        if(need_sw_avi_packet)
            enumHdmiPktType = _HDMI_TX_SW_BYPASS_RX_AVI_INFOFRAME;
        else
             enumHdmiPktType = _HDMI_TX_AVI_INFOFRAME;
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT0_ADDRESS, enumHdmiPktType, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            return _FALSE;
        }
        tx21SwPktEnMask |= _BIT0;
    }

    if(SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE){ // enable SW packet only when not enable RX SPD info frame bypass to TX
        // Packet6: SPD Inforframe Packet
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT6_ADDRESS, _HDMI_TX_SPD_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            return _FALSE;
        }
        tx21SwPktEnMask |= _BIT6;
    }


    if(((SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE) && ScalerHdmiTxGetVTEMpacektEn()) || (timing_info.run_vrr && (g_scaler_display_info.input_src == 1))){ // enable SW packet only when not enable RX EMP VTEM info frame bypass to TX
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
        if(ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_EMP_VTEM_VRR))
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
        {
            // Packet8: EMP VTEM Packet
            if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT8_ADDRESS, _HDMI_TX_VTEM, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                return _FALSE;
            }
            tx21SwPktEnMask |= _BIT8;
        }
    }

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    // Packet2: HDMI VSIF ALLM  SW Packet
    if(ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_VSIF_HDMI_ALLM) && ScalerHdmiTxRepeaterGetSourceInfo_Allm())
#else
    if(SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE) // enable SW packet only when not enable RX EMP VTEM info frame bypass to TX
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    {
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT2_ADDRESS, _HDMI_TX_VSIF, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            return _FALSE;
        }
        tx21SwPktEnMask |= _BIT2;
    }
    else
    {
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT2_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            return _FALSE;
        }
    }

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    // Packet3: HDR Packet
    if(ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_DRM_HDR) && (ScalerHdmiTxRepeaterGetSourceInfo_hdrMode() == HDR_HDR10_HDMI)){
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT3_ADDRESS, _HDMI_TX_STATIC_HDR_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            return _FALSE;
        }
        tx21SwPktEnMask |= _BIT3;
    }
    else
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    {
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT3_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            return _FALSE;
        }
    }

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    // Packet4: Dolby Vision VSIF Dolby Vision LL SW Packet
    if(ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_VSIF_DV_VSIF) && (ScalerHdmiTxRepeaterGetSourceInfo_DvVsifMode() != DOLBY_HDMI_VSIF_DISABLE)){
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT4_ADDRESS, _HDMI_TX_DV_VSIF, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            return _FALSE;
        }
        tx21SwPktEnMask |= _BIT4;
    }else
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    {
        if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT4_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            return _FALSE;
        }
    }

    // Enable the transmission of Periodic RAM0-5
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~(tx21SwPktEnMask & 0xff), tx21SwPktEnMask);
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_41_reg, ~((tx21SwPktEnMask >> 8) & 0x3), ((tx21SwPktEnMask >> 8) & 0x3));

    // disable HW bypass REG table to periodic SRAM directly
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~(_BIT6 | _BIT1), _BIT6);
    return _TRUE;
}

//--------------------------------------------------
// Description  : Hdmi Tx Fill SW pkt data
// Input Value  : usPeriodicSRAMAddress, enumHdmiPktType, bDffSel
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTx0FillPktData(UINT16 usPeriodicSRAMAddress, EnumHDMITxPacketType enumHdmiPktType, BOOLEAN bDffSel)
{
    //bDffSel = bDffSel;

    // Set Periodic SRAM Address
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_1_reg, usPeriodicSRAMAddress);

    // if update pkt by align_vde mode, select move REG table to SW DFF0 or DFF1
    if(rtd_getbits(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, _BIT6) == _BIT6)
    {
        if(bDffSel == _HDMI_TX_SW_PKT_DFF0)
        {
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~(_BIT7 | _BIT1), 0x00);
        }
        else
        {
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~(_BIT7 | _BIT1), _BIT7);
        }
    }

    switch(enumHdmiPktType)
    {
        case _HDMI_TX_NULL:
            ScalerHdmi21MacTx0FillNullPacket();
            break;

        case _HDMI_TX_AVI_INFOFRAME:
            ScalerHdmi21MacTx0FillAVIInfoPacket();
            break;
        case _HDMI_TX_SW_BYPASS_RX_AVI_INFOFRAME:
            ScalerHdmi21MacTx0FillRXAVIInfoPacket();
            break;
#if(_AUDIO_SUPPORT == _ON)
        case _HDMI_TX_AUDIO_INFOFRAME:
            ScalerHdmi21MacTx0FillAudioInfoPacket();
            break;
#endif

        case _HDMI_TX_SPD_INFOFRAME:
            ScalerHdmi21MacTx0FillSPDPacket();
            break;

        case _HDMI_TX_VSIF:
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
            ScalerHdmi21MacTx0FillVendorSpecificInfoPacket();
#endif
            break;

        case _HDMI_TX_DV_VSIF:
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
            ScalerHdmi21MacTx0FillDolbyVisionVsifPacket();
#endif
            break;

        case _HDMI_TX_STATIC_HDR_INFOFRAME:
#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
            ScalerHdmi21MacTx0FillHDRPacket();
#endif
            break;

        case _HDMI_TX_GCP:
            break;

        case _HDMI_TX_FREESYNC_INFOFRAME:
            break;

        case _HDMI_TX_VTEM:
            ScalerHdmi21MacTx0FillVtemPacket();
            break;

        case _HDMI_TX_AUDIO_METADATA:
            break;

        case _HDMI_TX_SBTM:
            ScalerHdmi21MacTx0FillSBTMPacket();
            break;

        default:
            break;
    }

    if(ScalerHdmi21MacTx0PktDataApplyToPSRAM() == _FALSE)
    {
        DebugMessageHDMI21Tx("[HDMI21_TX]  SW Packet (%d) not write\n", enumHdmiPktType);

        return _FALSE;
    }

    return _TRUE;
}

//--------------------------------------------------
// Description  : Hdmi AVI InfoFrame
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------
void ScalerHdmi21MacTx0FillAVIInfoPacket(void)
{
    UINT8 ucSBNum = 0x00;
    UINT8 ucChecksum = 0x00;
    UINT8 ucAVIIFVIC = 0x0;
    UINT8 ucAspectRatio= 0x0;
    UINT8 ucVIC1 = 0x00;
    UINT8 ucVIC2 = 0x00;
    UINT8 ucRepeatNum = 0x00;
    UINT8 ucAviInfo_version=0;
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT  // HDMI repeater mode
    // bypass AVI info frame
    if(ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_AVI_INFO) && (get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE)){
        #define _HDMI_AVI_INFO_BYTE_PB5 5
        UINT8 hdmiAviInfoPacket[13]={0,0,0,0,0,0,0,0,0,0,0,0,0};
        UINT8 ret = ScalerHdmiTxRepeaterGetSourceInfo_aviInfoFrame(&hdmiAviInfoPacket[0]);
        NoteMessageHDMI21Tx("[HDMI21_TX] Get AVI Info Frame in Through Mode %s\n", ret == 0? "Success": "Fail!");
        if(ret == 0){
            ucAviInfo_version = hdmiAviInfoPacket[1];
            memcpy(&pucAVIInfoPacketData[0], &hdmiAviInfoPacket[3], _HDMI_AVI_INFO_BYTE_PB5);
        }
    }else //  if(get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE){
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT  //HDMI repeater mode
    {

        if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
            DebugMessageHDMI21Tx("[HDMI21_TX] g_pstHdmi21MacTxInputTimingInfo is NULL@ScalerHdmi21MacTx0FillAVIInfoPacket\n");
            return;
        }

        memset(&pucAVIInfoPacketData[0], 0x0 ,AVI_INFO_LEN);
        ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);

        pStHdmiTxSearchVicTableInfo.ucSourceColorSpace = ScalerDpStreamGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _COLOR_SPACE);
        memcpy((UINT8*)&pStHdmiTxSearchVicTableInfo.stInputInfo, (UINT8*)g_pstHdmi21MacTxInputTimingInfo[_TX0], sizeof(StructTimingInfo));
        pStHdmiTxSearchVicTableInfo.pucAspectRatio = ucAspectRatio;
        pStHdmiTxSearchVicTableInfo.pucVIC = ucVIC1;
        pStHdmiTxSearchVicTableInfo.pucVIC2 = ucVIC2;

        DebugMessageHDMITx("[HDMI21_TX] Fill AVI:\n");
        DebugMessageHDMITx("[HDMI21_TX] FR=%d\n", (UINT32)pStHdmiTxSearchVicTableInfo.stInputInfo.usVFreq);
        DebugMessageHDMITx("[HDMI21_TX] H=%d\n", (UINT32)pStHdmiTxSearchVicTableInfo.stInputInfo.usHWidth);
        DebugMessageHDMITx("[HDMI21_TX] V=%d\n", (UINT32)pStHdmiTxSearchVicTableInfo.stInputInfo.usVHeight);
        DebugMessageHDMITx("[HDMI21_TX] InterlaceI=%d\n", (UINT32)pStHdmiTxSearchVicTableInfo.stInputInfo.b1Interlace);
        ScalerHdmiMacTxSearchVicTable(&pStHdmiTxSearchVicTableInfo);

        ucVIC1 = pStHdmiTxSearchVicTableInfo.pucVIC;
        ucVIC2 = pStHdmiTxSearchVicTableInfo.pucVIC2;
        ucAspectRatio = pStHdmiTxSearchVicTableInfo.pucAspectRatio;

        InfoMessageHDMI21Tx("[HDMI21_TX]  ucVIC=%d,%d, ", (UINT32)ucVIC1, (UINT32)ucVIC2);
        InfoMessageHDMI21Tx("Color=%d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE());

        // if Dual-Aspect Ratio Timing, select VIC1 or VIC2 by ref. EDID VDB
        if(ucVIC2 != 0x00)
        {
            #if 0 // avi info no need to match vic again
            ScalerHdmiMacTxGetEdidVicTable(ScalerHdmi21MacTxPxMapping(_TX0), _CTA_VDB, &pucEdidVDBVIC[0], pusCtaDataBlockAddr);
            ScalerHdmiMacTxGetEdidVicTable(ScalerHdmi21MacTxPxMapping(_TX0), _CTA_420VDB, &pucEdid420VDBVIC[0], pusCtaDataBlockAddr);

            for(pEdidData[0] = 0; pEdidData[0] < 31; pEdidData[0]++)
            {
                if((pucEdidVDBVIC[pEdidData[0]] == ucVIC1) || (pucEdid420VDBVIC[pEdidData[0]] == ucVIC1))
                {
                    ucAVIIFVIC = ucVIC1;

                    break;
                }
                else if((pucEdidVDBVIC[pEdidData[0]] == ucVIC2) || (pucEdid420VDBVIC[pEdidData[0]] == ucVIC2))
                {
                    ucAVIIFVIC = ucVIC2;


                    if(ucAspectRatio == _RATIO_4_3)
                    {
                        // if ucVIC1 is 4:3, then ucVIC2 is 16:9
                        ucAspectRatio = _RATIO_16_9;
                    }
                    else
                    {
                        // if ucVIC1 is 16:9, then ucVIC2 is 64:27
                        ucAspectRatio = _RATIO_OTHER;
                    }

                    break;
                }
            }
            #endif
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
             if(GET_HDMIRX_AVI_VIC() == ucVIC2) //intput vic == match output vic2,keep bypass vic2
#else	
		if(1)
#endif
            {
                ucAVIIFVIC = ucVIC2;
                if(ucAspectRatio == _RATIO_4_3)
                {
                    // if ucVIC1 is 4:3, then ucVIC2 is 16:9
                    ucAspectRatio = _RATIO_16_9;
                }
                else
                {
                    // if ucVIC1 is 16:9, then ucVIC2 is 64:27
                    ucAspectRatio = _RATIO_OTHER;
                }
            }
             
            if(ucAVIIFVIC == 0x00)
            {
                ucAVIIFVIC = ucVIC1;
            }
        }
        else
        {
            // normal Timing
            ucAVIIFVIC = ucVIC1;
        }

        if(ucVIC1 == 0x00)
        {
            ucAspectRatio = ScalerHdmiMacTxAspectRatioCal(g_pstHdmi21MacTxInputTimingInfo[_TX0]);
        }

        InfoMessageHDMI21Tx("[HDMI21_TX]  ucAspectradio=%d\n",(UINT32) ucAspectRatio);

        // ucRepeatNum = GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM();
        // DebugMessageHDMI21Tx("HDMI_TX2: ucRepeatNum", ucRepeatNum);

        // Set Y2 Y1 Y0 PB1[7:5]  based on Misc Color Space
        pucAVIInfoPacketData[0] |= (GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) << 5);

        // Set A0=1 PB1[4] Active format information(R3~R0 present)
        pucAVIInfoPacketData[0] |= (1 << 4);

        // Set B1 B0 PB1[3:2] = 00 and Set S1 S0 PB0[1:0] = 00

        if(GET_HDMI21_MAC_TX_INPUT_COLORIMETRY(_TX0) != _COLORIMETRY_EXT) // COLORIMETRY Refer to MISC Data
        {
            switch(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0))
            {
                case _COLOR_SPACE_RGB:

                    switch(GET_HDMI21_MAC_TX_INPUT_COLORIMETRY(_TX0))
                    {
                        case _COLORIMETRY_RGB_SRGB:
                        case _COLORIMETRY_RGB_XRRGB:
                        case _COLORIMETRY_RGB_SCRGB:
                            // Set C1 C0 = 00
                            pucAVIInfoPacketData[1] |= (0x00 << 6);

                            // Set EC2 EC1 EC0 = 000
                            pucAVIInfoPacketData[2] |= (0x00 << 4);

                            break;

                        case _COLORIMETRY_RGB_ADOBERGB:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 100
                            pucAVIInfoPacketData[2] |= (0x04 << 4);

                            break;

                        case _COLORIMETRY_RGB_DCI_P3:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 111
                            pucAVIInfoPacketData[2] |= (0x07 << 4);

                            break;

                        case _COLORMETRY_RESERVED:
                            // Set C1 C0 = 01
                            pucAVIInfoPacketData[1] |= (0x01 << 6);

                            // Set EC2 EC1 EC0 = 000
                            pucAVIInfoPacketData[2] |= (0x00 << 4);

                            break;

                        default:
                            break;
                    }
                    break;

                case _COLOR_SPACE_YCBCR422:
                case _COLOR_SPACE_YCBCR444:
                case _COLOR_SPACE_YCBCR420:

                    switch(GET_HDMI21_MAC_TX_INPUT_COLORIMETRY(_TX0))
                    {
                        case _COLORIMETRY_YCC_ITUR_BT709:
                            // Set C1 C0 = 10
                            pucAVIInfoPacketData[1] |= (0x02 << 6);

                            // Set EC2 EC1 EC0 = 000
                            pucAVIInfoPacketData[2] |= (0x00 << 4);

                            break;

                        case _COLORIMETRY_YCC_SMPTE_170M:
                            // Set C1 C0 = 01
                            pucAVIInfoPacketData[1] |= (0x01 << 6);

                            // Set EC2 EC1 EC0 = 000
                            pucAVIInfoPacketData[2] |= (0x00 << 4);

                            break;

                        case _COLORIMETRY_YCC_XVYCC601:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 000
                            pucAVIInfoPacketData[2] |= (0x00 << 4);

                            break;

                        case _COLORIMETRY_YCC_XVYCC709:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 001
                            pucAVIInfoPacketData[2] |= (0x01 << 4);

                            break;

                        case _COLORMETRY_RESERVED:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 111
                            pucAVIInfoPacketData[2] |= (0x07 << 4);

                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
        }
        else // COLORIMETRY Refer to VSC Data
        {
            switch(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0))
            {
                case _COLOR_SPACE_RGB:

                    switch(GET_HDMI21_MAC_TX_INPUT_EXT_COLORIMETRY(_TX0))
                    {
                        case _COLORIMETRY_EXT_RGB_SRGB:
                        case _COLORIMETRY_EXT_RGB_XRRGB:
                        case _COLORIMETRY_EXT_RGB_SCRGB:
                            // Set C1 C0 = 00
                            pucAVIInfoPacketData[1] |= (0x00 << 6);

                            // Set EC2 EC1 EC0 = 000
                            pucAVIInfoPacketData[2] |= (0x00 << 4);

                            break;

                        case _COLORIMETRY_EXT_RGB_ADOBERGB:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 100
                            pucAVIInfoPacketData[2] |= (0x04 << 4);

                            break;

                        case _COLORIMETRY_EXT_RGB_DCI_P3:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 111
                            pucAVIInfoPacketData[2] |= (0x07 << 4);

                            break;


                        case _COLORIMETRY_EXT_RGB_ITUR_BT2020:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 110
                            pucAVIInfoPacketData[2] |= (0x06 << 4);

                            break;

                        case _COLORIMETRY_EXT_RESERVED:
                            // Set C1 C0 = 01
                            pucAVIInfoPacketData[1] |= (0x01 << 6);

                            // Set EC2 EC1 EC0 = 000
                            pucAVIInfoPacketData[2] |= (0x00 << 4);

                            break;

                        default:
                            break;
                    }
                    break;

                case _COLOR_SPACE_YCBCR422:
                case _COLOR_SPACE_YCBCR444:
                case _COLOR_SPACE_YCBCR420:

                    switch(GET_HDMI21_MAC_TX_INPUT_EXT_COLORIMETRY(_TX0))
                    {
                        case _COLORIMETRY_EXT_YCC_ITUR_BT709:
                            // Set C1 C0 = 10
                            pucAVIInfoPacketData[1] |= (0x02 << 6);

                            // Set EC2 EC1 EC0 = 000
                            pucAVIInfoPacketData[2] |= (0x00 << 4);

                            break;

                        case _COLORIMETRY_EXT_YCC_XVYCC601:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 000
                            pucAVIInfoPacketData[2] |= (0x00 << 4);

                            break;

                        case _COLORIMETRY_EXT_YCC_XVYCC709:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 001
                            pucAVIInfoPacketData[2] |= (0x01 << 4);

                            break;

                        case _COLORIMETRY_EXT_YCC_SYCC601:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 010
                            pucAVIInfoPacketData[2] |= (0x02 << 4);

                            break;

                        case _COLORIMETRY_EXT_YCC_ADOBEYCC601:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 011
                            pucAVIInfoPacketData[2] |= (0x03 << 4);

                            break;

                        case _COLORIMETRY_EXT_YCC_ITUR_BT2020_CL:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 101
                            pucAVIInfoPacketData[2] |= (0x05 << 4);

                            break;

                        case _COLORIMETRY_EXT_YCC_ITUR_BT2020_NCL:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 110
                            pucAVIInfoPacketData[2] |= (0x06 << 4);

                            break;

                        case _COLORIMETRY_EXT_RESERVED:
                            // Set C1 C0 = 11
                            pucAVIInfoPacketData[1] |= (0x03 << 6);

                            // Set EC2 EC1 EC0 = 111
                            pucAVIInfoPacketData[2] |= (0x07 << 4);

                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
        }

        // YUV to RGB
        if((GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_RGB) && (ScalerDpStreamGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _COLOR_SPACE) != _COLOR_SPACE_RGB))
        {
            // Set C1 C0 = 00
            pucAVIInfoPacketData[1] |= (0x00 << 6);

            // Set EC2 EC1 EC0 = 000
            pucAVIInfoPacketData[2] |= (0x00 << 4);
        }

        // RGB to YUV
        if((GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) != _COLOR_SPACE_RGB) && (ScalerDpStreamGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _COLOR_SPACE) == _COLOR_SPACE_RGB))
        {
            // RGB2YUV Refer to tCOLOR_ST2_CONV_RGB2YUV_ITU709
            // ColorMetry: _COLORIMETRY_EXT_YCC_ITUR_BT709
            // Set C1 C0 = 10
            pucAVIInfoPacketData[1] |= (0x02 << 6);

            // Set EC2 EC1 EC0 = 000
            pucAVIInfoPacketData[2] |= (0x00 << 4);
        }

        // Set M1 M0 PB2[5:4] = ucAspectradio
        pucAVIInfoPacketData[1] |= (ucAspectRatio << 4);

        // Set R3 R2 R1 R0 PB2[3:0]= 1000 (Same As M1 Mo)
        pucAVIInfoPacketData[1] |= (0x08);

        // Set VIC = 0x01 PB4
        pucAVIInfoPacketData[3] = ucAVIIFVIC;

        if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_RGB)
        {
            if(ScalerHdmiMacTxEdidGetFeature(ScalerHdmi21MacTxPxMapping(_TX0), _VCDB_QS) == _TRUE)
            {
                // YQ field should be same as Q field when RGB
                if(GET_HDMI21_MAC_TX_INPUT_COLOR_RGB_QUANTIZATION_RANGE(_TX0) == _RGB_QUANTIZATION_FULL_RANGE)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_FULL << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_FULL << 6);
                }
                else if(GET_HDMI21_MAC_TX_INPUT_COLOR_RGB_QUANTIZATION_RANGE(_TX0) == _RGB_QUANTIZATION_LIMIT_RANGE)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_LIMITED << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_LIMITED << 6);
                }
                else if(GET_HDMI21_MAC_TX_INPUT_COLOR_RGB_QUANTIZATION_RANGE(_TX0) == _RGB_QUANTIZATION_DEFAULT_RANGE)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_DEFAULT << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_LIMITED << 6);
                }
                else if(GET_HDMI21_MAC_TX_INPUT_COLOR_RGB_QUANTIZATION_RANGE(_TX0) == _RGB_QUANTIZATION_RESERVED)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_RESERVED << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_RESERVED << 6);
                }
            }
            else
            {
                if(GET_HDMI21_MAC_TX_INPUT_COLOR_RGB_QUANTIZATION_RANGE(_TX0) == _RGB_QUANTIZATION_FULL_RANGE)
                {
                    if((ucAVIIFVIC == 0x00) || (ucAVIIFVIC == 0x01))
                    {
                        pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_FULL << 2);
                        pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_FULL << 6);
                    }
                    else
                    {
                        // HDMI20 SPEC Table7-7 indicate "not allowed", so write as default Limited range
                        pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_DEFAULT << 2);
                        pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_LIMITED << 6);
                    }
                }
                else if(GET_HDMI21_MAC_TX_INPUT_COLOR_RGB_QUANTIZATION_RANGE(_TX0) == _RGB_QUANTIZATION_LIMIT_RANGE)
                {
                    if((ucAVIIFVIC == 0x00) || (ucAVIIFVIC == 0x01))
                    {
                        // HDMI20 SPEC Table7-7 indicate "not allowed", so write as default Full range
                        pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_DEFAULT << 2);
                        pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_FULL << 6);
                    }
                    else
                    {
                        pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_LIMITED << 2);
                        pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_LIMITED << 6);
                    }
                }
                else
                {
                    // write 0x00
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_DEFAULT << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_LIMITED << 6);
                }
            }
        }
        else
        {
            if(ScalerHdmiMacTxEdidGetFeature(ScalerHdmi21MacTxPxMapping(_TX0), _VCDB_QY) == _TRUE)
            {
                if(GET_HDMI21_MAC_TX_INPUT_COLOR_YCC_QUANTIZATION_RANGE(_TX0) == _YCC_QUANTIZATION_FULL_RANGE)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_DEFAULT << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_FULL << 6);
                }
                else if(GET_HDMI21_MAC_TX_INPUT_COLOR_YCC_QUANTIZATION_RANGE(_TX0) == _YCC_QUANTIZATION_LIMIT_RANGE)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_DEFAULT << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_LIMITED << 6);
                }
                else if(GET_HDMI21_MAC_TX_INPUT_COLOR_YCC_QUANTIZATION_RANGE(_TX0) == _YCC_QUANTIZATION_RESERVED)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_RESERVED << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_RESERVED << 6);
                }
            }
            else
            {
                if((GET_HDMI21_MAC_TX_INPUT_COLOR_YCC_QUANTIZATION_RANGE(_TX0) == _YCC_QUANTIZATION_FULL_RANGE) || (GET_HDMI21_MAC_TX_INPUT_COLOR_YCC_QUANTIZATION_RANGE(_TX0) == _YCC_QUANTIZATION_LIMIT_RANGE))
                {
                    // when YCC, RGB Quantization = DEFAULT
                    if((ucAVIIFVIC == 0x00) || (ucAVIIFVIC == 0x01))
                    {
                        // IT format default "Full", so write as default Full range
                        pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_DEFAULT << 2);
                        pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_FULL << 6);
                    }
                    else
                    {
                        // CE format default "limited", so write as default Limited range
                        pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_DEFAULT << 2);
                        pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_LIMITED << 6);
                    }
                }
                else
                {
                    // write 0x00
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_DEFAULT << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_LIMITED << 6);
                }
            }
        }

        pucAVIInfoPacketData[4] |= (ucRepeatNum & 0x0F);

        if((ScalerDpStreamGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _COLOR_SPACE) != _COLOR_SPACE_YCBCR420) && (GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR420))
        {
            pucAVIInfoPacketData[2] = 0x00;  // Set Extended Colorimetry: svYCC601 Set RGB Default Range

            pucAVIInfoPacketData[4] &= 0x3F; // Set YQ1, YQ0=0 Set YCC Limit Range?

            DebugMessageHDMI21Tx("[HDMI21_TX]  Write 420 message\n");
        }

        DebugMessageHDMI21Tx("[HDMI21_TX]  COLORIMETRY=%d\n", (UINT32)GET_HDMI21_MAC_TX_INPUT_COLORIMETRY(_TX0));
        DebugMessageHDMI21Tx("[HDMI21_TX]  EXT_COLORIMETRY=%d\n", (UINT32)GET_HDMI21_MAC_TX_INPUT_EXT_COLORIMETRY(_TX0));
        DebugMessageHDMI21Tx("[HDMI21_TX]  RGB_QUANTIZATION=%d\n", (UINT32)GET_HDMI21_MAC_TX_INPUT_COLOR_RGB_QUANTIZATION_RANGE(_TX0));
        DebugMessageHDMI21Tx("[HDMI21_TX]  YCC_QUANTIZATION=%d\n", (UINT32)GET_HDMI21_MAC_TX_INPUT_COLOR_YCC_QUANTIZATION_RANGE(_TX0));

        // HB1: AVI InfoFrame Version (3: VIC >=128, 2: else)
        ucAviInfo_version = ((ucVIC1 >= 128)? 0x3: 0x02);
    } // if(get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE){ } else {
    ucChecksum -= 0x82;
    ucChecksum -= ucAviInfo_version;
    ucChecksum -= 0x0D;
#if 0//defined(CONFIG_RTK_HDMI_RX) || defined(CONFIG_ENABLE_DPRX)
    //sync avi info from src for pq demand
    pucAVIInfoPacketData[4] &= 0xCF;
    pucAVIInfoPacketData[2] &= 0xf;
    pucAVIInfoPacketData[1] &= 0x3f;
    if(g_scaler_display_info.input_src == 0){
        pucAVIInfoPacketData[4] |= (GET_HDMIRX_AVI_CN()<<4);
        pucAVIInfoPacketData[2] |= (GET_HDMIRX_AVI_ITC()<<7);
        pucAVIInfoPacketData[2] |= (GET_HDMIRX_AVI_EC()<<4);
        pucAVIInfoPacketData[1] |= (GET_HDMIRX_AVI_C()<<6);
    }else{
        pucAVIInfoPacketData[4] |= (DRVIF_DPRX_GET_CN()<<4);
        pucAVIInfoPacketData[2] |= (DRVIF_DPRX_GET_ITC()<<7);
        pucAVIInfoPacketData[2] |= (DRVIF_DPRX_GET_EC()<<4);
        pucAVIInfoPacketData[1] |= (DRVIF_DPRX_GET_C()<<6);
    }
    
#endif

    ucChecksum -= pucAVIInfoPacketData[0];
    ucChecksum -= pucAVIInfoPacketData[1];
    ucChecksum -= pucAVIInfoPacketData[2];
    ucChecksum -= pucAVIInfoPacketData[3];
    ucChecksum -= pucAVIInfoPacketData[4];

#ifndef CONFIG_HDMITX_REPEATER_SUPPORT
    InfoMessageHDMI21Tx("[HDMI21_TX]  Repeater Mode[%d], AVI Ver[%d]\n", get_hdmitxOutputMode(), ucAviInfo_version);
#endif
    DebugMessageHDMI21Tx("[HDMI21_TX]  pucAVIInfoPacketData[0]=%x\n", (UINT32)pucAVIInfoPacketData[0]);
    DebugMessageHDMI21Tx("[HDMI21_TX]  pucAVIInfoPacketData[1]=%x\n", (UINT32)pucAVIInfoPacketData[1]);
    DebugMessageHDMI21Tx("[HDMI21_TX]  pucAVIInfoPacketData[2]=%x\n", (UINT32)pucAVIInfoPacketData[2]);
    DebugMessageHDMI21Tx("[HDMI21_TX]  pucAVIInfoPacketData[3]=%x\n", (UINT32)pucAVIInfoPacketData[3]);
    DebugMessageHDMI21Tx("[HDMI21_TX]  pucAVIInfoPacketData[4]=%x\n", (UINT32)pucAVIInfoPacketData[4]);

    // Pacekt Header
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_3_reg, 0x82); // HB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_2_reg, ucAviInfo_version); // HB1: AVI InfoFrame Version (3: VIC >=128, 2: else)
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_1_reg, 0x0D); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_6_reg, pucAVIInfoPacketData[0]); // WORD0: PB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_5_reg, pucAVIInfoPacketData[1]); // WORD0: PB2
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg, pucAVIInfoPacketData[2]); // WORD0: PB3
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_11_reg, pucAVIInfoPacketData[3]); // WORD1: PB4
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_10_reg, pucAVIInfoPacketData[4]); // WORD1: PB5
//#endif
    return;
}

//--------------------------------------------------
// Description  : Hdmi Tx Audio Infoframe Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
#if(_AUDIO_SUPPORT == _ON)
extern UINT8 pucAudioInfoPacketData[];
void ScalerHdmi21MacTx0FillAudioInfoPacket(void)
{
    UINT8 ucSBNum = 0x00;
    UINT8 ucChecksum = 0x00;
    UINT8 ucCA = 0;
    UINT8 ucCC = 0;

    memset(&pucAudioInfoPacketData, 0, AUDIO_INFO_PACKET_SIZE);

    // Set CT3:0 PB1[7:4] (LPCM)
    pucAudioInfoPacketData[0] |= (0x00 << 4);

    // Set CC  PB1[2:0] based on CH num
    ucCC = ((ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CODING_TYPE) == _3D_LPCM_ASP) ? 0 : ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CHANNEL_COUNT));

    pucAudioInfoPacketData[0] |= ucCC;

    // Set SF2:0 PB2[4:2] = 0 Refer to Stream Header

    // Set SS1:0 PB2[1:0] = 0 Refer to Stream Header

    // Set CXT PB3 = 0 Refer to Audio Coding Type (CT) field

    // Set CA PB4 based on Ch Num
    // Layout = 0 (Audio Ch Num<=2)
    if(ucCC <= _AUDIO_CHANNEL_COUNT_2)
    {
        ucCA = 0x00;
    }
    else
    {
        // Set Channel Allocation flag based on Ch num and CEA Spec
        if(ucCC == 2)
        {
            ucCA = 0x01;
        }
        else if(ucCC == 3)
        {
            ucCA = 0x03;
        }
        else if(ucCC == 4)
        {
            ucCA = 0x07;
        }
        else if(ucCC == 5)
        {
            ucCA = 0x0B;
        }
        else if(ucCC == 6)
        {
            ucCA = 0x0F;
        }
        else if(ucCC == 7)
        {
            ucCA = 0x13;
        }

        pucAudioInfoPacketData[3] = ucCA;

        // LSV PB5[6:3] Must Set 0
        // DM_INH PB5[7] Must Set 0
        // LFPB1 and LFPB0 PB5[1:0] Must Set 0
    }

    ucChecksum -= 0x84;
    ucChecksum -= 0x01;
    ucChecksum -= 0x0A;

    ucChecksum -= pucAudioInfoPacketData[0];
    ucChecksum -= pucAudioInfoPacketData[1];
    ucChecksum -= pucAudioInfoPacketData[2];
    ucChecksum -= pucAudioInfoPacketData[3];

    // Pacekt Header
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_3_reg, 0x84); // HB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_2_reg, 0x01); // HB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_1_reg, 0x0A); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_6_reg, pucAudioInfoPacketData[0]); // WORD0: PB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_5_reg, pucAudioInfoPacketData[1]); // WORD0: PB2
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg, pucAudioInfoPacketData[2]); // WORD0: PB3
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_11_reg, pucAudioInfoPacketData[3]); // WORD1: PB4
}
#endif // #if(_AUDIO_SUPPORT == _ON)


//--------------------------------------------------
// Description  : Hdmi Tx PSRAM Packet Write Process
// Input Value  :
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTx0PktDataApplyToPSRAM(void)
{
    UINT16 usPollingTimeout = 0;
    StructEventPollingInfo eventInfo;

    // if update pkt by align_vde mode, need check last time update pkt done flag
    if(rtd_getbits(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, _BIT6) == _BIT6)
    {
        usPollingTimeout = 1000 / GET_HDMI21_MAC_TX_INPUT_FRAME_RATE(_TX0);

        if(rtd_getbits(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, _BIT7) == 0x00)
        {
            if(GET_HDMI21_MAC_TX0_SW_PKT_DFF0_USED() == _TRUE)
            {
                eventInfo.time_90k = usPollingTimeout;
                eventInfo.timecount = 1;
                eventInfo.addr = HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_reg;
                eventInfo.mask = _BIT2;
                eventInfo.status = _BIT2;
                if(Scaler_Wait_for_event_bank8_cloneIF(&eventInfo) == _TRUE)
                {
                    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_reg, ~(_BIT6 | _BIT2 | _BIT1 | _BIT0), _BIT2);
                }
                else
                {
                    ErrorMessageHDMI21Tx("[HDMI21_TX]  Update SW PKT DFF0 FAIL\n");

                    return _FALSE;
                }
            }

            SET_HDMI21_MAC_TX0_SW_PKT_DFF0_USED();
        }
        else
        {
            if(GET_HDMI21_MAC_TX0_SW_PKT_DFF1_USED() == _TRUE)
            {
                eventInfo.time_90k = usPollingTimeout;
                eventInfo.timecount = 1;
                eventInfo.addr = HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_reg;
                eventInfo.mask = _BIT1;
                eventInfo.status = _BIT1;
                if(Scaler_Wait_for_event_bank8_cloneIF(&eventInfo) == _TRUE)
                {
                    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_reg, ~(_BIT6 | _BIT2 | _BIT1 | _BIT0), _BIT1);
                }
                else
                {
                    ErrorMessageHDMI21Tx("[HDMI21_TX] Update SW PKT DFF1 FAIL\n");

                    return _FALSE;
                }
            }

            SET_HDMI21_MAC_TX0_SW_PKT_DFF1_USED();
        }
    }


#if 0 // TBD
    ScalerHdmiTx_Wait_VerticalDataEnable();
#endif
//#endif

    // write enable
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_reg, ~(_BIT6 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT3);

    usPollingTimeout = 2;

    if(rtd_getbits(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, _BIT6) != _BIT6)
    {
        eventInfo.time_90k = usPollingTimeout;
        eventInfo.timecount = 1;
        eventInfo.addr = HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_reg;
        eventInfo.mask = _BIT0;
        eventInfo.status = _BIT0;
        // check if sw pkt bypass to SRAM Done
        if(Scaler_Wait_for_event_bank8_cloneIF(&eventInfo) == _FALSE)
        {
            return _FALSE;
        }

        // Clr Done Flag
        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_reg, ~(_BIT6 | _BIT2 | _BIT1 | _BIT0), _BIT0);
    }

    return _TRUE;
}

//--------------------------------------------------
// Description  : Hdmi Tx Null Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTx0FillNullPacket(void)
{
    UINT8 ucSBNum = 0x00;

    // Pacekt Header
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_3_reg, 0x00); // HB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_2_reg, 0x00); // HB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_1_reg, 0x00); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }
}

//--------------------------------------------------
// Description  : Hdmi Tx Source-Based Tone mapping EMP Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTx0SBTMPacket(void)
{
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_41_reg, ~_BIT1, 0);//close first sw_pkt9

    if(ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT9_ADDRESS, _HDMI_TX_SBTM, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
    {
        InfoMessageHDMI21Tx("[HDMI_TX21]SBTM Packet send fail\n");
        return;
    }

    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_41_reg, ~_BIT1, _BIT1);//sw_pkt9 enable

}
