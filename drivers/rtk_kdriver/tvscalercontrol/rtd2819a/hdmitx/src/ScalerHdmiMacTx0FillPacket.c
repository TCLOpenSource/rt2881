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
// ID Code      : ScalerHdmiMacTx0.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_init.h>
//#include "../include/ScalerTx.h"
#include "../include/ScalerHdmiMacTx0FillPacket.h"

BOOLEAN ScalerHdmiMacTx0PktDataApplyToPSRAM(void);
extern BOOLEAN ScalerHdmi21MacTx0PktDataApplyToPSRAM(void);

//****************************************************************************
// CODE TABLES
//****************************************************************************
UINT16 tVIC_TABLE[_HDMI_VIC_TABLE_NUM] =
{
    12, 40, 30, 0, 1, 1, // (Frame rate/5)/(Hactive/16)/(Vactive/16)/P(0)vsI(1)/VIC/PictureAspectRatio
    11, 40, 30, 0, 1, 1,
    6, 120, 67, 1, 5, 2,
    5, 120, 67, 1, 20, 2,
    10, 120, 33, 1, 39, 2,
    10, 120, 67, 1, 40, 2,
    12, 120, 67, 1, 46, 2,
    11, 120, 67, 1, 46, 2,

    // HDMI 2.0 timing
    4, 105, 45, 0, 79, 3,
    5, 105, 45, 0, 80, 3,
    6, 105, 45, 0, 81, 3,
    10, 105, 45, 0, 82, 3,
    12, 105, 45, 0, 83, 3,
    11, 105, 45, 0, 83, 3,
    20, 105, 45, 0, 84, 3,
    24, 105, 45, 0, 85, 3,
    23, 105, 45, 0, 85, 3,
    4, 160, 67, 0, 86, 3,
    5, 160, 67, 0, 87, 3,
    6, 160, 67, 0, 88, 3,
    10, 160, 67, 0, 89, 3,
    12, 160, 67, 0, 90, 3,
    11, 160, 67, 0, 90, 3,
    20, 160, 67, 0, 91, 3,
    24, 160, 67, 0, 92, 3,
    23, 160, 67, 0, 92, 3,
    4, 0, 135, 0, 98, 3,      // (Hactive/16) = 256 outof range
    5, 0, 135, 0, 99, 3,
    6, 0, 135, 0, 100, 3,
    10, 0, 135, 0, 101, 3,
    12, 0, 135, 0, 102, 3,
    11, 0, 135, 0, 102, 3,
};

// Dual-Aspect Ratio Timing
// FIXME
UINT16 tVIC_TABLE_DUAL[_HDMI_VIC_TABLE_DUAL_NUM] =
{
    12, 45, 30, 0, 2, 3, 1, // (Frame rate/5)/(Hactive/16)/(Vactive/16)/P(0)vsI(1)/VIC1/VIC2/PictureAspectRatio
    11, 45, 30, 0, 2, 3, 1,
    12, 80, 45, 0, 4, 69, 2,
    11, 80, 45, 0, 4, 69, 2,
    6, 45, 30, 1, 6, 7, 1,
    12, 45, 15, 0, 8, 9, 1,
    6, 180, 30, 1, 10, 11, 1,
    12, 180, 15, 0, 12, 13, 1,
    11, 180, 15, 0, 12, 13, 1,
    12, 90, 30, 0, 14, 15, 1,
    11, 90, 30, 0, 14, 15, 1,
    12, 120, 67, 0, 16, 76, 2,
    11, 120, 67, 0, 16, 76, 2,
    10, 45, 36, 0, 17, 18, 1,
    10, 80, 45, 0, 19, 68, 2,
    5, 45, 36, 1, 21, 22, 1,
    10, 45, 18, 0, 23, 24, 1,
    5, 180, 36, 1, 25, 26, 1,
    10, 180, 18, 0, 27, 28, 1,
    10, 90, 36, 0, 29, 30, 1,
    10, 120, 67, 0, 31, 75, 2,
    4, 120, 67, 0, 32, 72, 2,
    5, 120, 67, 0, 33, 73, 2,
    6, 120, 67, 0, 34, 74, 2,
    12, 180, 30, 0, 35, 36, 1,
    11, 180, 30, 0, 35, 36, 1,
    10, 180, 36, 0, 37, 38, 1,
    20, 80, 45, 0, 41, 70, 2,
    20, 45, 36, 0, 42, 43, 1,
    10, 45, 36, 1, 44, 45, 1,
    24, 80, 45, 0, 47, 71, 2,
    23, 80, 45, 0, 47, 71, 2,
    24, 45, 30, 0, 48, 49, 1,
    23, 45, 30, 0, 48, 49, 1,
    12, 45, 30, 1, 50, 51, 1,
    11, 45, 30, 1, 50, 51, 1,
    40, 45, 36, 0, 52, 53, 1,
    20, 45, 36, 1, 54, 55, 1,
    48, 45, 30, 0, 56, 57, 1,
    47, 45, 30, 0, 56, 57, 1,
    24, 45, 30, 1, 58, 59, 1,
    23, 45, 30, 1, 58, 59, 1,
    4, 80, 45, 0, 60, 65, 2,
    5, 80, 45, 0, 61, 66, 2,
    6, 80, 45, 0, 62, 67, 2,
    24, 120, 67, 0, 63, 78, 2,
    23, 120, 67, 0, 63, 78, 2,
    20, 120, 67, 0, 64, 77, 2,

    // HDMI 2.0 timing // (Frame rate/5)/(Hactive/16)/(Vactive/16)/P(0)vsI(1)/VIC1/VIC2/PictureAspectRatio
    4, 240, 135, 0, 93, 103, 2,
    5, 240, 135, 0, 94, 104, 2,
    6, 240, 135, 0, 95, 105, 2,
    10, 240, 135, 0, 96, 106, 2,
    12, 240, 135, 0, 97, 107, 2,
    11, 240, 135, 0, 97, 107, 2,
    20, 240, 135, 0, 117, 119, 2,
    19, 240, 135, 0, 117, 119, 2,
    24, 240, 135, 0, 118, 120, 2,
    23, 240, 135, 0, 118, 120, 2,
    4, 480, 270, 0, 194, 202, 2,
    5, 480, 270, 0, 195, 203, 2,
    6, 480, 270, 0, 196, 204, 2,
    9, 480, 270, 0, 197, 205, 2,
    10, 480, 270, 0, 198, 206, 2,
    11, 480, 270, 0, 199, 207, 2,
    12, 480, 270, 0, 199, 207, 2,
};



//****************************************************************************
// local variable declaration
//****************************************************************************
//#include <rbus/dsce_misc_reg.h>
//#include <rbus/ppoverlay_reg.h>

#include "../include/ScalerFunctionInclude.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0.h"
#include "../include/HdmiMacTx/ScalerHdmiMacTxInclude.h"
#include "../include/HdmiPhyTx/ScalerHdmiPhyTxInclude.h"
#include "../include/ScalerTx.h"
#include "../include/HdmiMacTx/ScalerHdmiMacTx.h"
#include "../include/HdmiEdid/hdmitx_edid.h"
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
//#include <rtk_kdriver/tvscalercontrol/hdmirx/drvif_hdmirx.h>
#include <rtk_kdriver/measure/rtk_measure.h>

#endif
#ifdef CONFIG_ENABLE_DPRX
#include <rtk_kdriver/dprx/drvif_dprx.h>
#endif

//#include <scaler/scalerstruct.h>
//#include <scaler/scalerdrv.h>

//****************************************************************************
// Macro/Definition
//****************************************************************************


//****************************************************************************
// CODE TABLES
//****************************************************************************
#define SPD_INFO_LEN 25
UINT8 pucDPRxSPDBackup[SPD_INFO_LEN+3] =
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
    };
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
#define _HDMI_VTEM_BYTE_MD3 14 // 11+3
static UINT8 pucEmpVtemBackup[_HDMI_VTEM_BYTE_MD3] =
    {
    0x7f, // HB0, 0x7F (EMP)
    0xc0, // HB1, B[7]: First, B[6]: Last, B[5:0]: Rsvd
    0x00, // HB2, Sequence Index, First=0
    0x84, // PB0: B[7]: New=1, B[6]: End=0, B[5:4]: DS_Type=0, B[3]: AFR=0, B[2]: VFR=1, B[1] Sync=0, B[0] Rsvd(0)
    0x00, // PB1: Rsvd(0)
    0x01, // PB2: Organization_ID=1
    0x00, // PB3: Data_Set_Tag=1 (MSB)
    0x01, // PB4: Data_Set_Tag=1 (LSB)
    0x00, // PB5: Data_Set_Length=4(MSB)
    0x04, // PB6: Data_Set_Length=4(LSB)
    0x03, // PB7: MD0=1 (B[1]: M_CONST=0, B[0]: VRR_EN=1)   //B[7:4]: FVA_Factor_M1, B[1]: M_CONST, B[0]: VRR_EN
    0x00, // PB8: MD1                                                               // B[7:0]: base frame rate[7:0]
    0x00, // PB9: MD2                                                               // B[2] RB, B[1:0]: base frame rate[9:8]
    0x00, // PB10: MD3                                                              // B[7:0]: base frame rate[7:0]
    }; // SPD payload len=28, +3(header)


//#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
static UINT8 pucDPRxHDRBackup[32] = {0x00};
//#endif // #if(_HDMI_HDR10_TX0_SUPPORT == _ON)

#if(_HDMI_FREESYNC_TX0_SUPPORT == _ON)
static UINT8 pucDPRxFreesyncBackup[32] = {0x00};
#endif

static UINT8 pucVSInfoPacketData[5] = {0x00,0x00,0x00,0x00,0x00};
//#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
static HDMI_DRM_T hdmi_drm;

#endif//#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT


//****************************************************************************
// Local/Static VARIABLE DECLARATIONS
//****************************************************************************



// Fill AVI info frame

UINT8 ucVIC = 0x00;



UINT8 pucAVIInfoPacketData[AVI_INFO_LEN] = {0};
UINT8 pucEdidVDBVIC[31] = {0};
UINT8 pucEdid420VDBVIC[31] = {0};
UINT16 pusCtaDataBlockAddr[_CTA_TOTAL_DB + 1] = {0};

//static UINT8 *pucDdcRamAddr = 0;

UINT8 ucTemp = 0;


UINT8 ucPreAVIChecksum = 0;
#if(_AUDIO_SUPPORT == _ON)
UINT8 pucAudioInfoPacketData[AUDIO_INFO_PACKET_SIZE] = {0x00,0x00,0x00,0x00};
UINT8 ucCA = 0;
UINT8 ucCC = 0;
#endif // #if(_AUDIO_SUPPORT == _ON)

#if(_HDMI_FREESYNC_TX0_SUPPORT == _ON)
static UINT8 ucStreamIndex = _ST_MAP_NONE;
#endif // #if(_HDMI_FREESYNC_TX0_SUPPORT == _ON)

#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
static UINT8 ucStreamIndex = _ST_MAP_NONE;
static UINT8  hdrtype = HDR_DM_MODE_NONE;
#endif

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
static UINT8  hdmiAviInfoPacket[13]={0,0,0,0,0,0,0,0,0,0,0,0,0};

//static UINT8  ret = ScalerHdmiTxRepeaterGetSourceInfo_aviInfoFrame(&hdmiAviInfoPacket[0]);

#endif
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
static UINT8  pucDolbyVSInfoPacketData[27] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // HB[2:0]+PB[13:0]
static UINT8  hdmiVtemPacket[EM_VTEM_INFO_LEN]={0,0,0,0,0,0,0,0,0,0,0,0,0};
#endif
static UINT8  bHdmi20SwPktSramInit=0;

//****************************************************************************
// Global VARIABLE DECLARATIONS
//****************************************************************************

StructHdmiTxSearchVicTableInfo pStHdmiTxSearchVicTableInfo;


//****************************************************************************
// External VARIABLE DECLARATIONS
//****************************************************************************
extern UINT8 need_sw_avi_packet;
extern UINT8 ucVTEMpacketEn;
extern BOOLEAN g_bHdmiMacTx04KVICSendByVSIF;
#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
extern BOOLEAN g_bHdmiMacTx0SWPktDFF0Used;
extern BOOLEAN g_bHdmiMacTx0SWPktDFF1Used;
#endif

extern UINT8 ucTxMuteEnFlag;
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
extern StructHdmiRepeaterSourceInfo hdmiRepeaterSrcInfo;
#endif
extern StructHdmitxDisplayInfo tx_disp_info[1];
extern MEASURE_TIMING_T timing_info;

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
extern UINT32  hdmiRepeaterSwPktEnMask;
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
extern StructHdmitxDisplayInfo g_scaler_display_info;
void ScalerHdmiMacTx0FillVtemPacket(void);
//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************


//--------------------------------------------------
// Description  : HDMI Tx Aspect Ratio Calculate
// Input Value  : StructHDMITxInputInfo
// Output Value : AspectRatio
//--------------------------------------------------
UINT8 ScalerHdmiMacTxAspectRatioCal(StructTimingInfo *pstInputInfo)
{
    UINT16 usVactive = pstInputInfo->usVHeight;
    UINT16 usHactive = pstInputInfo->usHWidth;

    // Interlaced timing, Vactive need be *2
    if(pstInputInfo->b1Interlace == _TRUE)
    {
        usVactive = (usVactive * 2);
    }

    if((((usHactive * 3) / 4 - 5) <= usVactive) && (((usHactive * 3) / 4 + 5) >= usVactive))
    {
        return _RATIO_4_3;
    }
    else if((((usHactive * 9) / 16 - 5) <= usVactive) && (((usHactive * 9) / 16 + 5) >= usVactive))
    {
        return _RATIO_16_9;
    }
    else
    {
        return _RATIO_OTHER;
    }
}




//--------------------------------------------------
// Description  : HDMI Tx Search CTA VIC Table to get VIC & Ratio
// Input Value  : StructHDMITxInputInfo, AspectRatio, VIC
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTxSearchVicTable(StructHdmiTxSearchVicTableInfo *pStHdmiTxSearchVicTableInfo)
{
    UINT16 usN = 0x00;
    StructTimingInfo * pstInputInfo = &pStHdmiTxSearchVicTableInfo->stInputInfo;
    UINT16 usVactive = pstInputInfo->usVHeight;
    UINT16 pucVicTarget[4] = {0};

    // Interlaced timing, Vactive need be *2
    if(pstInputInfo->b1Interlace == _TRUE)
    {
        usVactive = (usVactive * 2);
        InfoMessageHDMITx("[HDMI_TX] Interlaced mode@ScalerHdmiMacTxSearchVicTable()\n");
    }

    // Construct VIC search target table
    pucVicTarget[0] = (UINT16)(pstInputInfo->usVFreq / 5 / 10);

    //InfoMessageHDMITx("[HDMI_TX] H.Act=%x\n", (UINT32)value_u16);
    pucVicTarget[1] = (UINT16)(pstInputInfo->usHWidth / 16);

    //InfoMessageHDMITx("[HDMI_TX] V.Act=%x\n", (UINT32)value_u16);
    pucVicTarget[2] = (UINT16)(usVactive / 16);

    //InfoMessageHDMITx("[HDMI_TX] Interlace=%d\n", (UINT32)value_u16);
    pucVicTarget[3] = pstInputInfo->b1Interlace;


#if 0 //fix search VIC fail
    if(ucSourceColorSpace == _COLOR_SPACE_YCBCR420)
    {
        pucVicTarget[1] = pucVicTarget[1] * 2;
    }
#endif

    // Decide VIC Num based on Frame rate/Hactive/Vactive/PvsI
    // normal timing
    pStHdmiTxSearchVicTableInfo->pucVIC  = 0;
    while(usN < (_HDMI_VIC_TABLE_NUM))
    {
        if(memcmp(&tVIC_TABLE[usN], pucVicTarget, 8) == 0)
        {
            pStHdmiTxSearchVicTableInfo->pucVIC = tVIC_TABLE[usN + 4];
            pStHdmiTxSearchVicTableInfo->pucAspectRatio = tVIC_TABLE[usN + 5];

            break;
        }

        usN += 6;
    }

    if(pStHdmiTxSearchVicTableInfo->pucVIC == 0)
    {
        usN = 0;

        // Dual-Aspect Ratio Timing
        while(usN < (_HDMI_VIC_TABLE_DUAL_NUM))
        {
            if(memcmp(&tVIC_TABLE_DUAL[usN], pucVicTarget, 8) == 0)
            {
                pStHdmiTxSearchVicTableInfo->pucVIC = tVIC_TABLE_DUAL[usN + 4];
                pStHdmiTxSearchVicTableInfo->pucVIC2 = tVIC_TABLE_DUAL[usN + 5];
                pStHdmiTxSearchVicTableInfo->pucAspectRatio = tVIC_TABLE_DUAL[usN + 6];

                break;
            }

            usN += 7;
        }
    }

    FatalMessageHDMITx("[HDMITX] VIC1=[%d]\n", (UINT32)pStHdmiTxSearchVicTableInfo->pucVIC);

    // Post VIC search process
    switch(pStHdmiTxSearchVicTableInfo->pucVIC)
    {
        case 0x00:
            // Do something
            break;

        case 20:
            if(pstInputInfo->usVTotal == 625)
            {
                pStHdmiTxSearchVicTableInfo->pucVIC = 39;
            }
            break;
        case 93:
        case 114:
            // 4k24&4k48 no CTA861 case
            if((pstInputInfo->usHTotal != 5500) || (GET_TX_ORI_VTOTAL() != 2250)) {
                pStHdmiTxSearchVicTableInfo->pucVIC = 0;
                pStHdmiTxSearchVicTableInfo->pucVIC2 = 0;
            }
            break;
        case 95:
        case 97:
        case 118:
            // 4k30&4k60&4k120 no CTA861 case
            if((pstInputInfo->usHTotal != 4400) || (GET_TX_ORI_VTOTAL() != 2250)) {
                pStHdmiTxSearchVicTableInfo->pucVIC = 0;
                pStHdmiTxSearchVicTableInfo->pucVIC2 = 0;
            }
            break;
        case 94:
        case 96:
        case 117:
            // 4k25&4k50&4k100 no CTA861 case
            if((pstInputInfo->usHTotal != 5280) || (GET_TX_ORI_VTOTAL() != 2250)) {
                pStHdmiTxSearchVicTableInfo->pucVIC = 0;
                pStHdmiTxSearchVicTableInfo->pucVIC2 = 0;
            }
            break;

        default:
            break;
    }
    FatalMessageHDMITx("[HDMITX] total=[%d,%d]\n", (UINT32)pstInputInfo->usHTotal, (UINT32)GET_TX_ORI_VTOTAL());
    // [H5X_HDMITX_BYPASS_HPD_EDID]
    if(ScalerHdmiTxGetBypassLinkTrainingEn()){
        if(pStHdmiTxSearchVicTableInfo->pucVIC2 != 0){
            DebugMessageHDMITx("[HDMI_TX][ZEBU] FORCE VIC2=%d->0 !!\n", (UINT32)pStHdmiTxSearchVicTableInfo->pucVIC2);
            pStHdmiTxSearchVicTableInfo->pucVIC2 = 0;
        }
    }
}

UINT16 BANK6_Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_INFO infoList)
{
    //UINT8 channel=0;
    StructHdmitxDisplayInfo *p_tx_disp_info = &tx_disp_info[0];

    switch (infoList)
    {
        case SCALER_INFO_COLOR_SPACE:
        	return p_tx_disp_info->color_space;
            default:
                FatalMessageHDMITx("[disp_info] Don't know how to get disp_info!!!\n");
                return 0;
    }

    //return 0;
}



// Get input video info from scaler
UINT16 BANK6_ScalerDpStreamGetElement(EnumOutputPort ucOutputPort, EnumMultiStreamElement enElement)
{
    UINT16 ret=0;

#if 0 // TEST
        DebugMessageHDMITx("[HDMITX] Get Idx[%d]\n", enElement);
#endif

    // Get input video info from scaler
    switch(enElement){
        case _COLOR_SPACE:
            ret = BANK6_Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_SPACE);
            break;

        default:
            FatalMessageHDMITx("[HDMITX] NOT SUPPORT ELEMENT[%d]@BANK6_ScalerDpStreamGetElement()\n", (UINT32)enElement);
            break;
    }

#if 0 // TEST
    DebugMessageHDMITx("[HDMITX] Get Idx[%d]=%d\n", enElement, ret);
#endif

    return ret;
}


void ScalerHdmiMacTx0FillRXAVIInfoPacket(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT

    UINT8 ucSBNum = 0;
    UINT8              *p_buf = NULL;
    UINT8              i = 0;
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
    p_buf = (UINT8 *)GET_HDMIRX_AVI_PTR();
#endif

    if(p_buf == NULL){
        FatalMessageHDMITx("[HDMI_TX] FillRXAVIInfoPacket: NUll\n");
        return;
    }
    for(i=0; i<AVI_INFO_LEN; i++){
        pucAVIInfoPacketData[i] = (UINT8)p_buf[i];
        FatalMessageHDMITx("%x ", (UINT32)pucAVIInfoPacketData[i]);
    }
    FatalMessageHDMITx("\n ");
#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)
    UINT8 ucChecksum = 0x00;

    // Pacekt Header
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_18_reg, 0x82); // HB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_17_reg, 0x02); // HB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_16_reg, 0x0D); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_22_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_21_reg, pucAVIInfoPacketData[0]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_20_reg, pucAVIInfoPacketData[1]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg, pucAVIInfoPacketData[2]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_26_reg, pucAVIInfoPacketData[3]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_25_reg, pucAVIInfoPacketData[4]); // WORD1: PB5

#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, pucAVIInfoPacketData[0]); // HB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, pucAVIInfoPacketData[1]); // HB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, pucAVIInfoPacketData[2]); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }
    ucPreAVIChecksum = pucAVIInfoPacketData[3];

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, pucAVIInfoPacketData[3]); // WORD0: PB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, pucAVIInfoPacketData[4]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_5_reg, pucAVIInfoPacketData[5]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_4_reg, pucAVIInfoPacketData[6]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_11_reg, pucAVIInfoPacketData[7]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_10_reg, pucAVIInfoPacketData[8]); // WORD1: PB5
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_9_reg, pucAVIInfoPacketData[9]); // WORD1: PB6
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_8_reg, pucAVIInfoPacketData[10]); // WORD1: PB7
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_15_reg, pucAVIInfoPacketData[11]); // WORD1: PB8
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_14_reg, pucAVIInfoPacketData[12]); // WORD1: PB9
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_13_reg, pucAVIInfoPacketData[13]); // WORD1: PB10
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_12_reg, pucAVIInfoPacketData[14]); // WORD1: PB11
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_19_reg, pucAVIInfoPacketData[15]); // WORD1: PB12
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_18_reg, pucAVIInfoPacketData[16]); // WORD1: PB13
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_17_reg, pucAVIInfoPacketData[17]); // WORD1: PB14

#endif // #elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
#endif
}
void ScalerHdmi21MacTx0FillRXAVIInfoPacket(void) 
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT

    UINT8 ucSBNum = 0;
    UINT8              *p_buf = NULL;
    UINT8              i = 0;
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
    p_buf = (UINT8 *)GET_HDMIRX_AVI_PTR();
#endif
    if(p_buf == NULL){
        FatalMessageHDMITx("[HDMI21_TX] FillRXAVIInfoPacket: NUll\n");
        return;
    }
    for(i=0; i<AVI_INFO_LEN; i++){
        pucAVIInfoPacketData[i] = (UINT8)p_buf[i];
        FatalMessageHDMITx("%x ", (UINT32)pucAVIInfoPacketData[i]);
    }
    FatalMessageHDMITx("\n ");
    // Pacekt Header
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_3_reg, pucAVIInfoPacketData[0]); // HB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_2_reg, pucAVIInfoPacketData[1]); // HB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_1_reg, pucAVIInfoPacketData[2]); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }
    ucPreAVIChecksum = pucAVIInfoPacketData[3];

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_7_reg, pucAVIInfoPacketData[3]); // WORD0: PB0
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_6_reg, pucAVIInfoPacketData[4]); // WORD0: PB1
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_5_reg, pucAVIInfoPacketData[5]); // WORD0: PB2
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg, pucAVIInfoPacketData[6]); // WORD0: PB3
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_11_reg, pucAVIInfoPacketData[7]); // WORD1: PB4
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_10_reg, pucAVIInfoPacketData[8]); // WORD1: PB5
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_9_reg, pucAVIInfoPacketData[9]); // WORD1: PB6
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_8_reg, pucAVIInfoPacketData[10]); // WORD1: PB7
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_15_reg, pucAVIInfoPacketData[11]); // WORD1: PB8
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_14_reg, pucAVIInfoPacketData[12]); // WORD1: PB9
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_13_reg, pucAVIInfoPacketData[13]); // WORD1: PB10
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_12_reg, pucAVIInfoPacketData[14]); // WORD1: PB11
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_19_reg, pucAVIInfoPacketData[15]); // WORD1: PB12
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_18_reg, pucAVIInfoPacketData[16]); // WORD1: PB13
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_17_reg, pucAVIInfoPacketData[17]); // WORD1: PB14
#endif
}
void ScalerHdmiMacTx0CheckAviUpdate(UINT8 HdmiType)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT

    UINT8              *p_rxAVIbuf = NULL;
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
    p_rxAVIbuf = (UINT8 *)GET_HDMIRX_AVI_PTR();
#endif
    if(p_rxAVIbuf == NULL){
        FatalMessageHDMITx("[HDMI_TX] FillRXAVIInfoPacket: NUll\n");
        return;
    }
    if(ucPreAVIChecksum != p_rxAVIbuf[3]){
        FatalMessageHDMITx("[HDMI_TX] Avi info Updatel\n");
        if(HdmiType == TC_HDMI20){
            rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT6, 0x00);
            ScalerHdmiMacTx0FillRXAVIInfoPacket();
             if(ScalerHdmiMacTx0PktDataApplyToPSRAM() == _FALSE)
            {
                ErrorMessageHDMITx("[HDMI_TX] AviUpdate not write\n");
                return;
            }

            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt0_en_mask),HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt0_en_mask);
            rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT6, _BIT6);
        }else{
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~(_BIT6 | _BIT1), 0x00);
            ScalerHdmi21MacTx0FillRXAVIInfoPacket();
             if(ScalerHdmi21MacTx0PktDataApplyToPSRAM() == _FALSE)
            {
                ErrorMessageHDMITx("[HD21_TX] AviUpdate not write\n");
                return;
            }
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_pr_spkt0_en_mask), HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_pr_spkt0_en_mask);
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~(_BIT6 | _BIT1), _BIT6);

        }
    }
#endif
}
//--------------------------------------------------
// Description  : Hdmi Tx AVI Infoframe Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0FillAVIInfoPacket(void)
{
    UINT8 ucVIC1 = 0x00;
    UINT8 ucVIC2 = 0x00;
    UINT8 ucSBNum = 0x00;
    UINT8 ucChecksum = 0x00;
    UINT8 ucAVIIFVIC = 0x00;
    UINT8 ucAspectRatio = 0x00;
    UINT8 ucRepeatNum = 0x00;

    InfoMessageHDMITx("[HDMI_TX] Fill AVI...\n");

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    // bypass AVI info frame
    if(ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_AVI_INFO) && (get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE)){
        #define _HDMI_AVI_INFO_BYTE_PB5 5
        value_u8 = ScalerHdmiTxRepeaterGetSourceInfo_aviInfoFrame(&hdmiAviInfoPacket[0]);
        NoteMessageHDMI21Tx("[HDMI_TX] Get AVI Info Frame in Through Mode =%d\n", (UINT32)value_u8);
        if(value_u8 == 0){
            memcpy(&pucAVIInfoPacketData[0], &hdmiAviInfoPacket[3], _HDMI_AVI_INFO_BYTE_PB5);
        }
    }else //  if(get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE){
#endif
    {
        if(!g_stHdmiMacTx0InputTimingInfo){
            DebugMessageHDMI21Tx("[HDMI_TX] g_stHdmiMacTx0InputTimingInfo is NULL@ScalerHdmiMacTx0FillAVIInfoPacket()\n");
            return;
        }
        memset(&pucAVIInfoPacketData[0], 0x0 ,AVI_INFO_LEN);
        ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);

        CLR_HDMI_MAC_TX0_4K_VIC_SEND_BY_VSIF();

        pStHdmiTxSearchVicTableInfo.ucSourceColorSpace = BANK6_ScalerDpStreamGetElement(ScalerHdmiMacTxPxMapping(_TX0), _COLOR_SPACE);
        memcpy((UINT8*)&pStHdmiTxSearchVicTableInfo.stInputInfo, (UINT8*)g_stHdmiMacTx0InputTimingInfo, sizeof(StructTimingInfo));
        pStHdmiTxSearchVicTableInfo.pucAspectRatio = ucAspectRatio;
        pStHdmiTxSearchVicTableInfo.pucVIC = ucVIC1;
        pStHdmiTxSearchVicTableInfo.pucVIC2 = ucVIC2;

        DebugMessageHDMITx("[HDMI_TX] Fill AVI:\n");
        DebugMessageHDMITx("[HDMI_TX] FR=%d\n", (UINT32)pStHdmiTxSearchVicTableInfo.stInputInfo.usVFreq);
        DebugMessageHDMITx("[HDMI_TX] H=%d\n", (UINT32)pStHdmiTxSearchVicTableInfo.stInputInfo.usHWidth);
        DebugMessageHDMITx("[HDMI_TX] V=%d\n", (UINT32)pStHdmiTxSearchVicTableInfo.stInputInfo.usVHeight);
        DebugMessageHDMITx("[HDMI_TX] InterlaceI=%d\n", (UINT32)pStHdmiTxSearchVicTableInfo.stInputInfo.b1Interlace);
        ScalerHdmiMacTxSearchVicTable(&pStHdmiTxSearchVicTableInfo);

        ucVIC1 = pStHdmiTxSearchVicTableInfo.pucVIC;
        ucVIC2 = pStHdmiTxSearchVicTableInfo.pucVIC2;
        ucAspectRatio = pStHdmiTxSearchVicTableInfo.pucAspectRatio;

        InfoMessageHDMITx("[HDMI_TX] ucVIC1=%d, Color=%d\n", (UINT32)ucVIC1, (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE());

        // if Dual-Aspect Ratio Timing, select VIC1 or VIC2 by ref. EDID VDB
        if(ucVIC2 != 0x00)
        {
            #if 0 //  avi info no need to match vic again
            ScalerHdmiMacTx0GetEdidVicTable(_CTA_VDB, &pucEdidVDBVIC[0], pusCtaDataBlockAddr);
            ScalerHdmiMacTx0GetEdidVicTable(_CTA_420VDB, &pucEdid420VDBVIC[0], pusCtaDataBlockAddr);

            for(pEdidData[0] = 0; pEdidData[0] < 31; pEdidData[0]++)
            {
                // if SVD >= 129 and SVD <= 192 then 7-BOOLEAN VIC is defined and is a native code
                if((pucEdidVDBVIC[pEdidData[0]] >= 129) && (pucEdidVDBVIC[pEdidData[0]] <= 192))
                {
                    pucEdidVDBVIC[pEdidData[0]] = pucEdidVDBVIC[pEdidData[0]] - 0x80;
                }

                if((pucEdid420VDBVIC[pEdidData[0]] >= 129) && (pucEdid420VDBVIC[pEdidData[0]] <= 192))
                {
                    pucEdid420VDBVIC[pEdidData[0]] = pucEdid420VDBVIC[pEdidData[0]] - 0x80;
                }

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
            if(GET_HDMIRX_AVI_VIC() == ucVIC2) { //intput vic == match output vic2,keep bypass vic2
#else
		if(1) {
#endif
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
            ucAspectRatio = ScalerHdmiMacTxAspectRatioCal(g_stHdmiMacTx0InputTimingInfo);
        }

        DebugMessageHDMITx("[HDMI_TX] ucAspectradio=%d\n", (UINT32)ucAspectRatio);

        ucRepeatNum = GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM();
        DebugMessageHDMITx("[HDMI_TX] ucRepeatNum=%d\n", (UINT32)ucRepeatNum);

        // VIC95/94/93/98 send by HDMI VSIF
        if((ucAVIIFVIC == 95) || (ucAVIIFVIC == 94) || (ucAVIIFVIC == 93) || (ucAVIIFVIC == 98))
        {
            SET_HDMI_MAC_TX0_4K_VIC_SEND_BY_VSIF();
        }

        // Set Y2 Y1 Y0 PB1[7:5]  based on Misc Color Space
        pucAVIInfoPacketData[0] |= (GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() << 5);

        // Set A0=1 PB1[4] Active format information(R3~R0 present)
        pucAVIInfoPacketData[0] |= (1 << 4);

        // Set B1 B0 PB1[3:2] = 00 and Set S1 S0 PB0[1:0] = 00

        if(GET_HDMI_MAC_TX0_INPUT_COLORIMETRY() != _COLORIMETRY_EXT) // COLORIMETRY Refer to MISC Data
        {
            switch(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE())
            {
                case _COLOR_SPACE_RGB:

                    switch(GET_HDMI_MAC_TX0_INPUT_COLORIMETRY())
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

                    switch(GET_HDMI_MAC_TX0_INPUT_COLORIMETRY())
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
            switch(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE())
            {
                case _COLOR_SPACE_RGB:

                    switch(GET_HDMI_MAC_TX0_INPUT_EXT_COLORIMETRY())
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

                    switch(GET_HDMI_MAC_TX0_INPUT_EXT_COLORIMETRY())
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
        if((GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_RGB) && (BANK6_ScalerDpStreamGetElement(ScalerHdmiMacTxPxMapping(_TX0), _COLOR_SPACE) != _COLOR_SPACE_RGB))
        {
            // Set C1 C0 = 00
            pucAVIInfoPacketData[1] |= (0x00 << 6);

            // Set EC2 EC1 EC0 = 000
            pucAVIInfoPacketData[2] |= (0x00 << 4);
        }

        // RGB to YUV
        if((GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() != _COLOR_SPACE_RGB) && (BANK6_ScalerDpStreamGetElement(ScalerHdmiMacTxPxMapping(_TX0), _COLOR_SPACE) == _COLOR_SPACE_RGB))
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
        if(GET_HDMI_MAC_TX0_4K_VIC_SEND_BY_VSIF() == _TRUE)
        {
            pucAVIInfoPacketData[3] = 0x00;
        }
        else
        {
            pucAVIInfoPacketData[3] = ucAVIIFVIC;
        }

        if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_RGB)
        {
            if(GET_HDMI_MAC_TX0_EDID_QS_SUPPORT() == _TRUE)
            {
                // YQ field should be same as Q field when RGB
                if(GET_HDMI_MAC_TX0_INPUT_COLOR_RGB_QUANTIZATION_RANGE() == _RGB_QUANTIZATION_FULL_RANGE)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_FULL << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_FULL << 6);
                }
                else if(GET_HDMI_MAC_TX0_INPUT_COLOR_RGB_QUANTIZATION_RANGE() == _RGB_QUANTIZATION_LIMIT_RANGE)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_LIMITED << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_LIMITED << 6);
                }
                else if(GET_HDMI_MAC_TX0_INPUT_COLOR_RGB_QUANTIZATION_RANGE() == _RGB_QUANTIZATION_DEFAULT_RANGE)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_DEFAULT << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_LIMITED << 6);
                }
                else if(GET_HDMI_MAC_TX0_INPUT_COLOR_RGB_QUANTIZATION_RANGE() == _RGB_QUANTIZATION_RESERVED)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_RESERVED << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_RESERVED << 6);
                }
            }
            else
            {
                if(GET_HDMI_MAC_TX0_INPUT_COLOR_RGB_QUANTIZATION_RANGE() == _RGB_QUANTIZATION_FULL_RANGE)
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
                else if(GET_HDMI_MAC_TX0_INPUT_COLOR_RGB_QUANTIZATION_RANGE() == _RGB_QUANTIZATION_LIMIT_RANGE)
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
            if(GET_HDMI_MAC_TX0_EDID_QS_SUPPORT() == _TRUE)
            {
                if(GET_HDMI_MAC_TX0_INPUT_COLOR_YCC_QUANTIZATION_RANGE() == _YCC_QUANTIZATION_FULL_RANGE)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_DEFAULT << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_FULL << 6);
                }
                else if(GET_HDMI_MAC_TX0_INPUT_COLOR_YCC_QUANTIZATION_RANGE() == _YCC_QUANTIZATION_LIMIT_RANGE)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_DEFAULT << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_LIMITED << 6);
                }
                else if(GET_HDMI_MAC_TX0_INPUT_COLOR_YCC_QUANTIZATION_RANGE() == _YCC_QUANTIZATION_RESERVED)
                {
                    pucAVIInfoPacketData[2] |= (_AVI_RGB_QUANTIZATION_RESERVED << 2);
                    pucAVIInfoPacketData[4] |= (_AVI_YCC_QUANTIZATION_RESERVED << 6);
                }
            }
            else
            {
                if((GET_HDMI_MAC_TX0_INPUT_COLOR_YCC_QUANTIZATION_RANGE() == _YCC_QUANTIZATION_FULL_RANGE) || (GET_HDMI_MAC_TX0_INPUT_COLOR_YCC_QUANTIZATION_RANGE() == _YCC_QUANTIZATION_LIMIT_RANGE))
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

        if((BANK6_ScalerDpStreamGetElement(ScalerHdmiMacTxPxMapping(_TX0), _COLOR_SPACE) != _COLOR_SPACE_YCBCR420) && (GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420))
        {
            pucAVIInfoPacketData[2] = 0x00;  // Set Extended Colorimetry: svYCC601 Set RGB Default Range

            pucAVIInfoPacketData[4] &= 0x3F; // Set YQ1, YQ0=0 Set YCC Limit Range?

            DebugMessageHDMITx("[HDMI_TX] Write 420 message\n");
        }

        DebugMessageHDMITx("[HDMI_TX] COLORIMETRY=%d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLORIMETRY());
        DebugMessageHDMITx("[HDMI_TX] EXT_COLORIMETRY=%d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_EXT_COLORIMETRY());
        DebugMessageHDMITx("[HDMI_TX] RGB_QUANTIZATION=%d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_RGB_QUANTIZATION_RANGE());
        DebugMessageHDMITx("[HDMI_TX] YCC_QUANTIZATION=%d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_YCC_QUANTIZATION_RANGE());
    }

    ucChecksum -= 0x82;
    ucChecksum -= 0x02;
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

    InfoMessageHDMITx("[HDMI_TX] AVIInfoPacketData: 0x82 0x02 0x0d\n");
#if 0
    for(i=0; i<5; i++)
        InfoMessageHDMITx("%x ", (UINT32)pucAVIInfoPacketData[i]);

    InfoMessageHDMITx(", checksum=%x\n", (UINT32)ucChecksum);
#endif
#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)
    // Pacekt Header
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_18_reg, 0x82); // HB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_17_reg, 0x02); // HB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_16_reg, 0x0D); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_22_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_21_reg, pucAVIInfoPacketData[0]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_20_reg, pucAVIInfoPacketData[1]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg, pucAVIInfoPacketData[2]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_26_reg, pucAVIInfoPacketData[3]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_25_reg, pucAVIInfoPacketData[4]); // WORD1: PB5

#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, 0x82); // HB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, 0x02); // HB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, 0x0D); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, pucAVIInfoPacketData[0]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_5_reg, pucAVIInfoPacketData[1]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_4_reg, pucAVIInfoPacketData[2]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_11_reg, pucAVIInfoPacketData[3]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_10_reg, pucAVIInfoPacketData[4]); // WORD1: PB5

#endif // #elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
}

//--------------------------------------------------
// Description  : Hdmi Tx Null Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0FillNullPacket(void)
{
    UINT8 ucSBNum = 0x00;
    UINT8 value_u8;

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)
    value_u8 = HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_56_get_dummy_7_0(rtd_inl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_56_reg));
#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
    value_u8 = HDMITX20_MAC1_HDMI21_PKT_CTRL_1_get_sw_pkt_addr(rtd_inl(HDMITX20_MAC1_HDMI21_PKT_CTRL_1_reg));
#endif
    DebugMessageHDMITx("[HDMI_TX] Fill NULL[%d]...\n", (UINT32)value_u8);


#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_18_reg, 0x00); // HB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_17_reg, 0x00); // HB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_16_reg, 0x00); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg + ucSBNum*4), 0x00);
    }

#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, 0x00); // HB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, 0x00); // HB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, 0x00); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

#endif // #elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
}





#if(_HDMI_TX_CTS_TEST == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Set SW pkt to transmit GCP Pkt
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0FWFillGCPPacket(void)
{
    UINT8 ucSBNum = 0x00;
    //InfoMessageHDMITx("[HDMI_TX] Fill GCP...\n");
#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_18_reg, 0x03); // HB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_17_reg, 0x00); // HB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_16_reg, 0x00); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    // SubPacket0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_22_reg, 0x10); // HDMI: WORD0: PB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_21_reg, 0x04); // HDMI: WORD0: PB1

    // SubPacket1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_30_reg, 0x10); // HDMI: WORD2: PB7
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_29_reg, 0x04); // HDMI: WORD2: PB8

    // SubPacket2
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_38_reg, 0x10); // HDMI: WORD4: PB14
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_37_reg, 0x04); // HDMI: WORD4: PB15

    // SubPacket3
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_46_reg, 0x10); // HDMI: WORD6: PB21
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_45_reg, 0x04); // HDMI: WORD6: PB22

#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, 0x03); // HB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, 0x00); // HB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, 0x00); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    // SubPacket0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, 0x10); // HDMI: WORD0: PB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, 0x04); // HDMI: WORD0: PB1

    // SubPacket1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_15_reg, 0x10); // HDMI: WORD2: PB7
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_14_reg, 0x04); // HDMI: WORD2: PB8

    // SubPacket2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_23_reg, 0x10); // HDMI: WORD4: PB14
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_22_reg, 0x04); // HDMI: WORD4: PB15

    // SubPacket3
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_31_reg, 0x10); // HDMI: WORD6: PB21
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_30_reg, 0x04); // HDMI: WORD6: PB22

#endif
}


//--------------------------------------------------
// Description  : Hdmi Tx Set SW pkt to transmit GCP Pkt with mute control
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0FWFillGCPPacket_MuteCtrl(void)
{

    UINT8 ucSBNum = 0x00;
    UINT8 enable = ScalerHdmiTxGetMute();

    NoteMessageHDMITx("[HDMITX] GCP MuteEn=%d\n", (UINT32)enable);

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_18_reg, 0x03); // HB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_17_reg, 0x00); // HB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_16_reg, 0x00); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    // SubPacket0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_22_reg, (enable? 0x01: 0x10)); // HDMI: WORD0: PB0, SB0 B[0]: Set_AVMUTE, B[4]: Clear_AVMUTE
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_21_reg, 0x04); // HDMI: WORD0: PB1

    // SubPacket1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_30_reg, 0x10); // HDMI: WORD2: PB7
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_29_reg, 0x04); // HDMI: WORD2: PB8

    // SubPacket2
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_38_reg, 0x10); // HDMI: WORD4: PB14
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_37_reg, 0x04); // HDMI: WORD4: PB15

    // SubPacket3
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_46_reg, 0x10); // HDMI: WORD6: PB21
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_45_reg, 0x04); // HDMI: WORD6: PB22

#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, 0x03); // HB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, 0x00); // HB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, 0x00); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    // SubPacket0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, (enable? 0x01: 0x10)); // HDMI: WORD0: PB0, SB0 B[0]: Set_AVMUTE, B[4]: Clear_AVMUTE
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, 0x04); // HDMI: WORD0: PB1

    // SubPacket1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_15_reg, 0x10); // HDMI: WORD2: PB7
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_14_reg, 0x04); // HDMI: WORD2: PB8

    // SubPacket2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_23_reg, 0x10); // HDMI: WORD4: PB14
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_22_reg, 0x04); // HDMI: WORD4: PB15

    // SubPacket3
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_31_reg, 0x10); // HDMI: WORD6: PB21
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_30_reg, 0x04); // HDMI: WORD6: PB22

#endif
}

#endif


#if(_AUDIO_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Audio Infoframe Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0FillAudioInfoPacket(void)
{
    UINT8 ucSBNum = 0x00;
    UINT8 ucChecksum = 0x00;
    UINT8 ucCA = 0;
    UINT8 ucCC = 0;
    FatalMessageHDMITx("[HDMI_TX] Fill Audio...\n");

    memset(&pucAudioInfoPacketData, 0, AUDIO_INFO_PACKET_SIZE);

    // Set CT3:0 PB1[7:4] (LPCM)
    pucAudioInfoPacketData[0] |= (0x00 << 4);

    // Set CC  PB1[2:0] based on CH num
    ucCC = ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_CHANNEL_COUNT);

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

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_18_reg, 0x84); // HB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_17_reg, 0x01); // HB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_16_reg, 0x0A); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_22_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_21_reg, pucAudioInfoPacketData[0]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_20_reg, pucAudioInfoPacketData[1]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_19_reg, pucAudioInfoPacketData[2]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_26_reg, pucAudioInfoPacketData[3]); // WORD1: PB4

#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, 0x84); // HB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, 0x01); // HB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, 0x0A); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, pucAudioInfoPacketData[0]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_5_reg, pucAudioInfoPacketData[1]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_4_reg, pucAudioInfoPacketData[2]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_11_reg, pucAudioInfoPacketData[3]); // WORD1: PB4

#endif
}
#endif

extern StructHdmitxDisplayInfo g_scaler_display_info;
//--------------------------------------------------
// Description  : Hdmi Tx PSRAM Packet Write Process
// Input Value  :
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0PktDataApplyToPSRAM(void)
{
    UINT16 usPollingTimeout = 0;
    StructEventPollingInfo eventInfo;
    DebugMessageHDMITx("[HDMITX] ScalerHdmiMacTx0PktDataApplyToPSRAM...\n");

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)
    // opcr_enrbpktsram=1  Enable Write
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_55_reg, ~(_BIT7 | _BIT4 | _BIT3), _BIT7);

    usPollingTimeout = 2;

    eventInfo.time_90k = usPollingTimeout;
    eventInfo.timecount = 1;
    eventInfo.addr = HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_55_reg;
    eventInfo.mask = _BIT4;
    eventInfo.status = _BIT4;
    if(HDMITX_ScalerTimerPollingFlagProc(eventInfo.time_90k, eventInfo.addr, eventInfo.mask, eventInfo.status) == _FALSE)
    {
        return _FALSE;
    }

    // Clr opcr_enrbpktsram_done
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_55_reg, ~(_BIT7 | _BIT4 | _BIT3), _BIT4);

    return _TRUE;

#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    // if update pkt by align_vde mode, need check last time update pkt done flag
    if(rtd_getbits(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, _BIT6) == _BIT6)
    {
        usPollingTimeout = 1000 / GET_HDMI_MAC_TX0_INPUT_FRAME_RATE();

        if(rtd_getbits(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, _BIT7) == 0x00)
        {
            if(GET_HDMI_MAC_TX0_SW_PKT_DFF0_USED() == _TRUE)
            {
                eventInfo.time_90k = usPollingTimeout;
                eventInfo.timecount = 1;
                eventInfo.addr = HDMITX20_MAC1_HDMI21_PKT_CTRL_0_reg;
                eventInfo.mask = _BIT2;
                eventInfo.status = _BIT2;
                if(HDMITX_ScalerTimerPollingFlagProc(eventInfo.time_90k, eventInfo.addr, eventInfo.mask, eventInfo.status) == _TRUE)
                {
                    rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_0_reg, ~(_BIT2 | _BIT1 | _BIT0), _BIT2);
                }
                else
                {
                    FatalMessageHDMITx("[HDMI_TX] Update SW PKT DFF0 FAIL\n");
                    return _FALSE;
                }
            }

            SET_HDMI_MAC_TX0_SW_PKT_DFF0_USED();
        }
        else
        {
            if(GET_HDMI_MAC_TX0_SW_PKT_DFF1_USED() == _TRUE)
            {
                eventInfo.time_90k = usPollingTimeout;
                eventInfo.timecount = 1;
                eventInfo.addr = HDMITX20_MAC1_HDMI21_PKT_CTRL_0_reg;
                eventInfo.mask = _BIT1;
                eventInfo.status = _BIT1;
                if(HDMITX_ScalerTimerPollingFlagProc(eventInfo.time_90k, eventInfo.addr, eventInfo.mask, eventInfo.status) == _TRUE)
                {
                    rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_0_reg, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);
                }
                else
                {
                    FatalMessageHDMITx("[HDMI_TX] Update SW PKT DFF1 FAIL\n");
                    return _FALSE;
                }
            }

            SET_HDMI_MAC_TX0_SW_PKT_DFF1_USED();
        }
    }

    // Enable Write
    rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_0_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT3);

    usPollingTimeout = 2;

    if(rtd_getbits(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, _BIT6) != _BIT6)
    {
        // check if sw pkt bypass to SRAM Done
        eventInfo.time_90k = usPollingTimeout;
        eventInfo.timecount = 1;
        eventInfo.addr = HDMITX20_MAC1_HDMI21_PKT_CTRL_0_reg;
        eventInfo.mask = _BIT0;
        eventInfo.status = _BIT0;
        if(HDMITX_ScalerTimerPollingFlagProc(eventInfo.time_90k, eventInfo.addr, eventInfo.mask, eventInfo.status) == _FALSE)
        {
            FatalMessageHDMITx("[HDMI_TX] sw pkt bypass to SRAM Done FAIL\n");
            return _FALSE;
        }

        // Clr Done Flag
        rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_0_reg, ~(_BIT2 | _BIT1 | _BIT0), _BIT0);
    }

    return _TRUE;

#endif // #elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
}



//--------------------------------------------------
// Description  : Hdmi Tx Fill SW pkt data
// Input Value  : usPeriodicSRAMAddress, enumHdmiPktType, bDffSel
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0FillPktData(UINT16 usPeriodicSRAMAddress, EnumHDMITxPacketType enumHdmiPktType, BOOLEAN bDffSel)
{

    DebugMessageHDMITx("[HDMITX] ScalerHdmiMacTx0FillPktData\n");

    //bDffSel = bDffSel;
#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)

    // Set Periodic SRAM Address
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_56_reg, usPeriodicSRAMAddress);

#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    // Set Periodic SRAM Address
    rtd_outl(HDMITX20_MAC1_HDMI21_PKT_CTRL_1_reg, usPeriodicSRAMAddress);

    // if update pkt by align_vde mode, select move REG table to SW DFF0 or DFF1
    if(rtd_getbits(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, _BIT6) == _BIT6)
    {
        if(bDffSel == _HDMI_TX_SW_PKT_DFF0)
        {
            rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT7, 0x00);
        }
        else
        {
            rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT7, _BIT7);
        }
    }

#endif // #if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)

    switch(enumHdmiPktType)
    {
        case _HDMI_TX_NULL:
            ScalerHdmiMacTx0FillNullPacket();
            break;

        case _HDMI_TX_AVI_INFOFRAME:
            ScalerHdmiMacTx0FillAVIInfoPacket();
            break;
        case _HDMI_TX_SW_BYPASS_RX_AVI_INFOFRAME:
            ScalerHdmiMacTx0FillRXAVIInfoPacket();
            break;
#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2) // SPD Info Frame need use SW_PKT6 (support on Gen2)
        case _HDMI_TX_SPD_INFOFRAME:
            ScalerHdmiMacTx0FillSPDPacket();
            break;
#endif

#if(_AUDIO_SUPPORT == _ON)
        case _HDMI_TX_AUDIO_INFOFRAME:
            ScalerHdmiMacTx0FillAudioInfoPacket();
            break;
#endif

        case _HDMI_TX_VSIF:
            break;

        case _HDMI_TX_DV_VSIF:
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
            ScalerHdmiMacTx0FillDolbyVisionVsifPacket();
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
            break;

        case _HDMI_TX_STATIC_HDR_INFOFRAME:
#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
            ScalerHdmiMacTx0FillHDRPacket();
#elif defined(CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT)
            ScalerHdmiMacTx0FillHDRPacket_ZebuTest();
#endif
            break;

        case _HDMI_TX_GCP:
            ScalerHdmiMacTx0FWFillGCPPacket();
            break;

        case _HDMI_TX_GCP_MUTE_CTRL:
            ScalerHdmiMacTx0FWFillGCPPacket_MuteCtrl();
            break;

        case _HDMI_TX_FREESYNC_INFOFRAME:
#if(_HDMI_FREESYNC_TX0_SUPPORT == _ON)
            ScalerHdmiMacTx0FillFreesyncPacket();
#endif
            break;

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2) // EMP VTEM Frame need use SW_PKT8 (support on Gen2)
        case _HDMI_TX_VTEM:
            ScalerHdmiMacTx0FillVtemPacket();
            break;
#endif

        case _HDMI_TX_AUDIO_METADATA:
            break;

        default:
            break;
    }

    if(ScalerHdmiMacTx0PktDataApplyToPSRAM() == _FALSE)
    {
        FatalMessageHDMITx("[HDMI_TX] SW Packet %d not write\n", (UINT32)enumHdmiPktType);

        return _FALSE;
    }

    return _TRUE;
}

//--------------------------------------------------
// Description  : Hdmi Tx PSRAM Packet
// Input Value  :
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0PeriodicSRAMPacking(void)
{
    //UINT8 pktEnMask=0;
    EnumHDMITxPacketType enumHdmiPktType = _HDMI_TX_NULL;
    // opcr_rprwcmd=1  Write Mode
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_55_reg, ~(_BIT7 | _BIT5 | _BIT4 | _BIT3), _BIT5);

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_1)
    // opcr_wrdes=0 Select Periodic SRAM
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_55_reg, ~(_BIT7 | _BIT6 | _BIT4 | _BIT3), 0x00);
#elif(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
    // HW bypass REG table to periodic SRAM directly
    rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT6, 0x00);
#endif
    // HDMI20 SW Packet(0~9) SRAM init
    if(bHdmi20SwPktSramInit == 0){
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT0_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI_TX] SW PKT0 Init Fail!!\n");
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT1_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI_TX] SW PKT1 Init Fail!!\n");
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT2_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI_TX] SW PKT2 Init Fail!!\n");
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT3_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI_TX] SW PKT3 Init Fail!!\n");
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT4_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI_TX] SW PKT4 Init Fail!!\n");
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT5_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI_TX] SW PKT5 Init Fail!!\n");
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT6_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI_TX] SW PKT6 Init Fail!!\n");
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT7_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI_TX] SW PKT7 Init Fail!!\n");
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT8_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI_TX] SW PKT8 Init Fail!!\n");
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT9_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE) FatalMessageHDMITx("[HDMI_TX] SW PKT9 Init Fail!!\n");
        bHdmi20SwPktSramInit = 1;
        NoteMessageHDMITx("[HDMI_TX] HDMI20 SW PKT Init Done!!\n");
    }
    // [FIX-ME] decide bypass mode by both AP command and HW register status
#if !defined(ENABLE_INTERNAL_HDMIRX_PTG) && !defined(SCALER_AUTO_RUN)
    if((get_hdmitxOutputMode() != TX_OUT_THROUGH_MODE)|| (SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE) || (g_scaler_display_info.input_src != 0) || need_sw_avi_packet)
#endif
    { // enable SW packet only when not enable RX AVI info frame bypass to TX
        // Packet0: AVI Infoframe Packet
        FatalMessageHDMITx("[HDMI_TX] AVI InfoFrame !! %d %d %d\n",get_hdmitxOutputMode(),g_scaler_display_info.input_src,need_sw_avi_packet);
        if(need_sw_avi_packet)
            enumHdmiPktType = _HDMI_TX_SW_BYPASS_RX_AVI_INFOFRAME;
        else
             enumHdmiPktType = _HDMI_TX_AVI_INFOFRAME;
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT0_ADDRESS, enumHdmiPktType, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            FatalMessageHDMITx("[HDMI_TX] AVI InfoFrame Fail!!\n");
            return _FALSE;
        }
        else{
            // enable HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg::SW_PKT0
            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt0_en_mask), HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt0_en_mask);
        }
    }

    if(SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE){ // enable SW packet only when not enable RX Audio info frame bypass to TX
        // Packet1: Null Packet or Audio inforframe
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT1_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            FatalMessageHDMITx("[HDMI_TX] SW1 NULL Packet Fail!!\n");
            return _FALSE;
        }
        else{
            // enable HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg::SW_PKT1
            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt1_en_mask), HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt1_en_mask);
        }
    }

    // Packet2: Null Packet or Vendor Specific InforFrame Packet


    if(SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE){ // enable SW packet only when not enable RX VSIF info frame bypass to TX
        // For 3D timing or HDMI14 4k timing
    #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
        if(ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_VSIF_HDMI_ALLM)&& ScalerHdmiTxRepeaterGetSourceInfo_Allm()){
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT2_ADDRESS, _HDMI_TX_VSIF, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                return _FALSE;
            }
            else{
                // enable HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg::SW_PKT2
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt2_en_mask), HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt2_en_mask);
            }
        }else
    #endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
        if((ScalerDpStreamSTis3DVideo(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE) || (GET_HDMI_MAC_TX0_4K_VIC_SEND_BY_VSIF() == _TRUE))
        {
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT2_ADDRESS, _HDMI_TX_VSIF, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                FatalMessageHDMITx("[HDMI_TX] SW2 NULL Packet Fail!!\n");
                return _FALSE;
            }
            else{
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt2_en_mask), HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt2_en_mask);
            }
        }
        else
        {
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT2_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                FatalMessageHDMITx("[HDMI_TX] SW2 NULL Packet Fail(2)!!\n");
                return _FALSE;
            }
            else{
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt2_en_mask), HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt2_en_mask);
            }
        }
    }

    if(SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE){ // enable SW packet only when not enable RX DRM info frame bypass to TX
        // Packet3: Null Packet or HDR Packet
    //#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
    #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
        if(ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_DRM_HDR) && (ScalerHdmiTxRepeaterGetSourceInfo_hdrMode() == HDR_HDR10_HDMI)){
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT3_ADDRESS, _HDMI_TX_STATIC_HDR_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                return _FALSE;
            }
            else{
                // enable HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg::SW_PKT3
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt3_en_mask), HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt3_en_mask);
            }
        }else
    #endif
        if(GET_STREAM_HDR_INFO_FRAME_RECEIVED() == _TRUE)
        {
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT3_ADDRESS, _HDMI_TX_STATIC_HDR_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                return _FALSE;
            }
            else{
                // enable HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg::SW_PKT3
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt3_en_mask), HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt3_en_mask);
            }
        }
        else
    //#endif // #if(_HDMI_HDR10_TX0_SUPPORT == _ON)
        {

            DebugMessageHDMITx("[HDMI_TX] NO HDR Packet\n");
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT3_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                FatalMessageHDMITx("[HDMI_TX] SW3 NULL Packet Fail!!\n");
                return _FALSE;
            }
            else{
                // enable HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg::SW_PKT3
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt3_en_mask), HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt3_en_mask);
            }
        }
    }

    if(g_scaler_display_info.input_src== 0){
    // Packet4: Null Packet or GCP Packet for 4K
#if(_HDMI_TX_CTS_TEST == _ON)
    // solution for Goodway HDMI2.0 Tx 4K60 CTS Fail
    if(SCRIPT_TX_SKIP_AVMUTE_BYPASS() && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 5000) && (GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_8_BITS))
    {
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT4_ADDRESS, _HDMI_TX_GCP, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            FatalMessageHDMITx("[HDMI_TX] SW4 GCP Packet Fail!!\n");
            return _FALSE;
        }else{
            // enable HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg::SW_PKT4
            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt4_en_mask), HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt4_en_mask);
            InfoMessageHDMITx("[HDMI_TX] SW4 GCP Packet OK!!\n");
        }
    }
    else
#endif
    {
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT4_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            FatalMessageHDMITx("[HDMI_TX] SW4 Null Packet Fail!!\n");
            return _FALSE;
        }
        else{
            // enable HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg::SW_PKT4
            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt4_en_mask), HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt4_en_mask);
        }
    }
    }
    if(SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE){ // enable SW packet only when not enable RX FREESYNC info frame bypass to TX
        // Packet5: Null Packet or Freesync Packet
    #if(_HDMI_FREESYNC_TX0_SUPPORT == _ON)
        if(GET_DP_STREAM_AMD_SPD_INFO_FRAME_RECEIVED(ScalerDpStreamPxSTMapping(ScalerHdmiMacTxPxMapping(_TX0))) == _TRUE)
        {
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT5_ADDRESS, _HDMI_TX_FREESYNC_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                FatalMessageHDMITx("[HDMI_TX] SW5 FREESYNC Packet Fail!!\n");
                return _FALSE;
            }
            else{
                // enable HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg::SW_PKT5
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt5_en_mask), HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt5_en_mask);
            }
        }
        else
    #endif
        {
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT5_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                FatalMessageHDMITx("[HDMI_TX] SW5 NULL Packet Fail!!\n");
                return _FALSE;
            }
            else{
                // enable HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg::SW_PKT5
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt5_en_mask), HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_pr_spkt5_en_mask);
            }
        }
    }

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2) // SPD Info Frame need use SW_PKT6 (support on Gen2)
    if(SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE){ // enable SW packet only when not enable RX SPD info frame bypass to TX
        // Packet6: SPD Infoframe Packet
        // disable spkt6 transmission
        rtd_maskl(HDMITX20_MAC1_HDMI21_SW_PKT_42_reg, ~HDMITX20_MAC1_HDMI21_SW_PKT_42_pr_spkt6_en_mask, HDMITX20_MAC1_HDMI21_SW_PKT_42_pr_spkt6_en(0));
        if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT6_ADDRESS, _HDMI_TX_SPD_INFOFRAME, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
        {
            FatalMessageHDMITx("[HDMI_TX] SW6 SPD Packet Fail!!\n");
            return _FALSE;
        }
        // enable spkt6 transmission
        rtd_maskl(HDMITX20_MAC1_HDMI21_SW_PKT_42_reg, ~HDMITX20_MAC1_HDMI21_SW_PKT_42_pr_spkt6_en_mask, HDMITX20_MAC1_HDMI21_SW_PKT_42_pr_spkt6_en(1));
    }

    if(((SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE) && ScalerHdmiTxGetVTEMpacektEn()) || (timing_info.run_vrr && (g_scaler_display_info.input_src == 1))){ // enable SW packet only when not enable RX EMP VTEM info frame bypass to TX
    #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
        if(ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_EMP_VTEM_VRR))
    #endif

        {
            // Packet8: EMP VTEM Packet
            // disable spkt8 transmission
            rtd_maskl(HDMITX20_MAC1_HDMI21_SW_PKT_42_reg, ~HDMITX20_MAC1_HDMI21_SW_PKT_42_pr_spkt8_en_mask, HDMITX20_MAC1_HDMI21_SW_PKT_42_pr_spkt8_en(0));
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT8_ADDRESS, _HDMI_TX_VTEM, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                FatalMessageHDMITx("[HDMI_TX] SW8 VTEM Packet Fail!!\n");
                return _FALSE;
            }
            // enable spkt8 transmission
            rtd_maskl(HDMITX20_MAC1_HDMI21_SW_PKT_42_reg, ~HDMITX20_MAC1_HDMI21_SW_PKT_42_pr_spkt8_en_mask, HDMITX20_MAC1_HDMI21_SW_PKT_42_pr_spkt8_en(1));
            DebugMessageHDMITx("[HDMI_TX] SW8 VTEM Packet Enable!!\n");
        }
    }

    if(SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE){ // enable SW packet only when not enable RX DV VSIF info frame bypass to TX
    #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
        // Packet9: Dolby Vision VSIF Dolby Vision LL SW Packet
        //InfoMessageHDMITx("[HDMI_TX] DV En=%x, SW_PKT=%d\n", (UINT32)TX_SW_PKT_VSIF_DV_VSIF, (UINT32)hdmiRepeaterSwPktEnMask);
        //InfoMessageHDMITx("[HDMI_TX] SW_PKT(DV VSIF)=%x\n", (UINT32)ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_VSIF_DV_VSIF));
        if(ScalerHdmiTxRepeaterGetSwPktEn(TX_SW_PKT_VSIF_DV_VSIF) && (ScalerHdmiTxRepeaterGetSourceInfo_DvVsifMode() != DOLBY_HDMI_VSIF_DISABLE)){
            rtd_maskl(HDMITX20_MAC1_HDMI21_SW_PKT_42_reg, ~HDMITX20_MAC1_HDMI21_SW_PKT_42_pr_spkt9_en_mask, HDMITX20_MAC1_HDMI21_SW_PKT_42_pr_spkt9_en(0));
            if(ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT9_ADDRESS, _HDMI_TX_DV_VSIF, _HDMI_TX_SW_PKT_DFF0) == _FALSE)
            {
                return _FALSE;
            }
            // enable spkt9 transmission
            rtd_maskl(HDMITX20_MAC1_HDMI21_SW_PKT_42_reg, ~HDMITX20_MAC1_HDMI21_SW_PKT_42_pr_spkt9_en_mask, HDMITX20_MAC1_HDMI21_SW_PKT_42_pr_spkt9_en(1));
        }
      #endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    #endif // #if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2) // SPD Info Frame need use SW_PKT6 (support on Gen2)
    }

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
    // Enable the transmission of Periodic RAM6-9
    //rtd_maskl(HDMITX20_MAC1_HDMI21_SW_PKT_42_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT7 | _BIT6 | _BIT5 | _BIT4));

    // disable HW bypass REG table to periodic SRAM directly
    rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT6, _BIT6);
#endif // #if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)

    return _TRUE;
}

