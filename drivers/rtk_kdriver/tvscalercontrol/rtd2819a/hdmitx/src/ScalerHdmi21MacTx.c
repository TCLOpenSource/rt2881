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
// ID Code      : ScalerHdmi21MacTx.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>

#include "../include/ScalerFunctionInclude.h"
#include "../include/Hdmi21MacTx/ScalerHdmi21MacTx.h"
#include "../include/Hdmi21MacTx/ScalerHdmi21MacTxInclude.h"
#include "../include/Hdmi21MacTx0/ScalerHdmi21MacTx0Include.h"

#include "../include/ScalerTx.h"

extern void ScalerHdmi21MacTx0RepetitionDecision(void);

#if(_HDMI21_TX_SUPPORT == _ON)

//****************************************************************************
// CODE TABLES
//****************************************************************************
// Table 7-35 compressed video transport Primary Compressd Formats

//#ifdef H5X_HDMITX_FIXME // [FIX-ME] need allocate in IMEM ?
StructDSCHDMICOMPRESSTable t3G_3LANE_444TABLE[] =
{// Hactive , Vcative , Framrate , (Bpp x << 4) ,  HCactive ,  HCblank
    {3840, 2160,  60, 192, 1920, 280},
    {5120, 2160,  60, 173, 2307, 144}, // VIC 126
};

#ifdef NOT_USED_NOW
StructDSCHDMICOMPRESSTable t3G_3LANE_422TABLE[] =
{// Hactive , Vcative , Framrate , (Bpp x 10000) ,  HCactive ,  HCblank
    {5120, 2160,  60, 173, 2307, 144}, // VIC 126
    {7680, 4320,  24, 123, 2460, 816}, // VIC 194/202
    {7680, 4320,  25, 123, 2460, 732}, // VIC 195/203
    {7680, 4320,  30, 118, 2360, 144}, // VIC 196/204
};
#endif // #ifdef NOT_USED_NOW

StructDSCHDMICOMPRESSTable t6G_3LANE_444TABLE[] =
{
    {5120, 2160, 60, 175, 2334, 104},
    {10240, 4320, 24, 182, 4854, 912},
    {10240, 4320, 25, 181, 4827, 1536},
    {10240, 4320, 30, 177, 4720, 128},
};

#ifdef NOT_USED_NOW
StructDSCHDMICOMPRESSTable t6G_3LANE_422TABLE[] =
{
    {5120, 2160, 120, 175, 2334, 104},
    {7680, 4320, 48, 123, 2460, 816},
    {7680, 4320, 50, 123, 2460, 732},
    {7680, 4320, 60, 119, 2380, 116},
    {10240, 4320, 24, 182, 4854, 912},
    {10240, 4320, 25, 181, 4827, 1536},
    {10240, 4320, 30, 177, 4720, 128},
};
#endif // #ifdef NOT_USED_NOW

StructDSCHDMICOMPRESSTable t6G_4LANE_444TABLE[] =
{
    {3840, 2160,  60, 192, 1920, 280},
    {3840, 2160,  120, 192, 1920, 280},
    {3840, 2160,  144, 192, 1920, 80},
    {7680, 4320, 50, 157, 3142, 1180},
    {7680, 4320, 60, 159, 3182, 140},
};

#ifdef NOT_USED_NOW
StructDSCHDMICOMPRESSTable t6G_4LANE_422TABLE[] =
{
    {10240, 4320, 48, 126, 3360, 420},
    {10240, 4320, 50, 125, 3334, 892},
    {10240, 4320, 60, 117, 3120, 124},
};
#endif // #ifdef NOT_USED_NOW

StructDSCHDMICOMPRESSTable t8G_4LANE_444TABLE[] =
{
    {10240, 4320, 48, 163, 4347, 756},
    {10240, 4320, 50, 162, 4320, 1376},
    {10240, 4320, 60, 157, 4187, 124},
};

// 10G table 444 = 422 table
StructDSCHDMICOMPRESSTable t10G_4LANE_444TABLE[] =
{
    {7680, 4320, 100, 134, 2680, 784},
    {7680, 4320, 120, 130, 2600, 100},
};

#ifdef NOT_USED_NOW
StructDSCHDMICOMPRESSTable t12G_4LANE_422TABLE[] =
{
    {7680, 4320, 100, 125, 3334, 764},
    {7680, 4320, 120, 117, 3120, 124},
};
#endif // #ifdef NOT_USED_NOW

StructDSCHDMICOMPRESSTable t12G_4LANE_444TABLE[] =
{
    {3840, 2160,  60, 192, 1920, 280},
    {7680, 4320, 30, 157, 3840, 660},
    {7680, 4320, 48, 157, 3142, 1292},
    {7680, 4320, 50, 157, 3142, 1180},
    {7680, 4320, 60, 159, 3182, 140},
};

#ifdef NOT_USED_NOW
StructDSCHDMICOMPRESSTable t3G_4LANE_444TABLE[] =
{// Hactive , Vcative , Framrate , (Bpp x 10000) ,  HCactive ,  HCblank
    {5120, 2160,  60, 173, 2307, 144}, // VIC 126
};
#endif // #ifdef NOT_USED_NOW

// ------------------------------------------------
// DTD timing For EDID modify
// ------------------------------------------------
UINT8 tVIC_USE_FOR_DID_T1[35] =
{
    117, 118, 119, 120, 124, 125, 126, 127, 193, 194,
    195, 196, 197, 198, 199, 200, 201, 202, 203, 204,
    205, 206, 207, 208, 209, 210, 211, 212, 213, 214,
    215, 216, 217, 218, 219,
};

UINT8 tDEFAULT_TIMING_PATTERN_DTD[18] =
{
    0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0x55, 0x50, 0x21, 0x00, 0X00, 0x1E,
};

UINT8 tDEFAULT_TIMING_PATTERN_T1[20] =
{
    0x01, 0x3A, 0x00, 0x84, 0x7F, 0x07, 0x17, 0x01, 0x57, 0x00, 0x2B, 0x00, 0x37, 0x04, 0x2C, 0x00, 0X03, 0x00, 0X04, 0x00,
};


StructHdmi21EdidTimingReference t3G_3LANE_EDIDTIMING[1] =
{
    {95, {0x04, 0x74, 0x00, 0x30, 0xF2, 0x70, 0x5A, 0x80, 0xB0, 0x58, 0x8A, 0x00 ,0x54, 0x4F, 0x21, 0x00, 0x00, 0x1A, 0x00 , 0x00}},
};


StructHdmi21EdidTimingReference t3G_4LANE_EDIDTIMING[1] =
{
    {95, {0x04, 0x74, 0x00, 0x30, 0xF2, 0x70, 0x5A, 0x80, 0xB0, 0x58, 0x8A, 0x00 ,0x54, 0x4F, 0x21, 0x00, 0x00, 0x1A, 0x00 , 0x00}},
};


StructHdmi21EdidTimingReference t6G_3LANE_EDIDTIMING[1] =
{
    {97, {0x08, 0xE8, 0x00, 0x30, 0xF2, 0x70, 0x5A, 0x80, 0xB0, 0x58, 0x8A, 0x00 ,0x54, 0x4F, 0x21, 0x00, 0x00, 0x1A, 0x00 , 0x00}},
};

// over 5k need DID EXT
StructHdmi21EdidTimingReference t6G_4LANE_EDIDTIMING[1] =
{
    {126, {0x09, 0x22, 0x01, 0x84, 0xFF, 0x13, 0x7B, 0x01, 0xA3, 0x00, 0x57, 0x00 ,0x6F, 0x08, 0x59, 0x00, 0x07, 0x00, 0x09, 0x00}},
};

// over 8k need DID EXT
StructHdmi21EdidTimingReference t8G_4LANE_EDIDTIMING[1] =
{
    {196, {0x0F, 0xD0, 0x01, 0x84, 0xFF, 0x1D, 0x27, 0x05, 0x27, 0x02, 0xAF, 0x00 ,0xDF, 0x10, 0x4F, 0x00, 0x0F, 0x00, 0x13, 0x00}},
};

// over 10k need DID EXT
StructHdmi21EdidTimingReference t10G_4LANE_EDIDTIMING[1] =
{
    {212, {0x13, 0x44, 0x02, 0x84, 0xFF, 0x27, 0xF7, 0x02, 0x1F, 0x03, 0xAF, 0x00 ,0xDF, 0x10, 0xB3, 0x00, 0x0F, 0x00, 0x13, 0x00}},
};

// ------------------------------------------------
// FRL Mode ACR N/ CTS Value Table
// ------------------------------------------------
UINT32 tHDMI21_TX_AUDIO_32K_MULTI_N_CTS[] =
{
    4224, 8448, 16896, 33792, 67584, 135168, 171875,  // 3G - > 32k, 64, 128k, 256k, 512k, 1024k, CTS
    4032, 8064, 16128, 32256, 64512, 129024, 328125, // 6G
    3456, 6912, 13824, 27648, 55296, 110592, 437500, // 8G
    3456, 6912, 13824, 27648, 55296, 110592, 468750, // 10G
    3072, 6144, 12288, 24576, 49152, 98304, 500000,  // 12G
};

UINT32 tHDMI21_TX_AUDIO_44_1K_MULTI_N_CTS[] =
{
    5292, 10584, 21168, 42336, 84672, 169344, 156250, // 3G - > 44.1k, 88.2, 176.4k, 352.8k, 705.6k, 1411.2k, CTS
    5292, 10584, 21168, 42336, 84672, 169344, 312500, // 6G
    3969, 7938, 15876, 31752, 63504, 127008, 312500, // 8G
    3969, 7938, 15876, 31752, 63504, 127008, 390625, // 10G
    3969, 7938, 15876, 31752, 63504, 127008, 468750,  // 12G
};

UINT32 tHDMI21_TX_AUDIO_48K_MULTI_N_CTS[] =
{
    5760, 11520, 23040, 46080, 92160, 184320, 156250, // 3G - > 48k, 96, 192k, 384k, 768k, 1536k, CTS
    6048, 12096, 24192, 48384, 96768, 193536, 328125, // 6G
    6048, 12096, 24192, 48384, 96768, 193536, 437500, // 8G
    5184, 10368, 20736, 41472, 82944, 165888, 468750, // 10G
    4752, 9504, 19008, 38016, 76032, 152064, 515625, // 12G
};
//#endif

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************

StructHdmi21MacTxInfoType g_pstHdmi21MacTxInfo[_HDMI_21_TX_TOTAL_NUMBER] = {{0}};
StructHdmi21CtrlInfo g_pstHdmi21MacTxCtrlInfo[_HDMI_21_TX_TOTAL_NUMBER] = {{0}};
StructLTPParameter g_pstHdmi21MacTxLTP[_HDMI_21_TX_TOTAL_NUMBER] = {{0}};
StructHdmi21BandwidthCheckParameter g_pstHdmi21MacTxBandWidthParameter[_HDMI_21_TX_TOTAL_NUMBER] = {{0}};
StructDSCHDMITxInfoType g_pstHdmi21MacTxStreamInfo[_HDMI_21_TX_TOTAL_NUMBER] = {{0}};
// FOR HDMI TX INPUT INFORMATION
StructHDMITxInputInfo g_pstHdmi21MacTxInputInfo[_HDMI_21_TX_TOTAL_NUMBER] = {{0}};
StructTimingInfo *g_pstHdmi21MacTxInputTimingInfo[_HDMI_21_TX_TOTAL_NUMBER]={NULL};
StructStreamDisplayCompensateInfo *g_pstHdmi21MacTxDPFCompensateInfo[_HDMI_21_TX_TOTAL_NUMBER]={NULL};
#if(_DSC_SUPPORT == _ON)
// For Dsc Encoder output
StructTimingInfo g_pstHdmi21MacTxDscTimingInfo[_HDMI_21_TX_TOTAL_NUMBER] = {{0}};
// PPS DATA
UINT8 g_ppucHdmi21MacTxPPSData[_HDMI_21_TX_TOTAL_NUMBER][_HDMI21_PPS_PACKET_TOTAL_SIZE] = {{0}};
// DSC encoder timing infor
StructDSCHDMICompressTimingInfor g_pstHdmi21MacTxDscCompressInputInfo[_HDMI_21_TX_TOTAL_NUMBER] = {{0}};
#endif
StructHDMITxAudioInfo g_pstHdmi21MacTxAudioInfo[_HDMI_21_TX_TOTAL_NUMBER] = {{0}};
StructDSCHDMICompressTimingInfor g_pstHdmi21MacTxHCTiminginfo[_HDMI_21_TX_TOTAL_NUMBER] = {{0}};
static StructStreamDisplayCompensateInfo g_stStreamDisplayCompensateInfo;

static UINT8 ucTXn = 0;
//static StructHdmitxDisplayInfo *p_tx_disp_info;
extern StructHdmitxDisplayInfo tx_disp_info[1];
extern StructTimingInfo g_stHdmiTx0InputTimingInfo;
//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************


#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
#if(_AUDIO_SUPPORT == _ON)
//extern UINT32 drvif_Hdmi_get_audio_sample_freq(void);
#endif
#endif

//--------------------------------------------------
// Description  : Scaler control DSC encode or Decode
// Input Value  : EnumOutputPort , EnumOutputDfpDeviceType
// Output Value : TRUE(go to link on) or FALE(wait dsc finish)
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTxStreamManagement(EnumOutputPort enumOutputPort)
{
    // reset data stream management state when dprx prx stream type reset to none
    if(GET_OUTPUT_STREAM_TYPE(enumOutputPort) == _STREAM_TYPE_NONE)
    {
        if(GET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(ScalerHdmi21MacTxPxTxMapping(enumOutputPort)) != _DP_RX_SOURCE_JUDGE)
        {
            // Disable Encoder Power down
            ScalerDscPower(enumOutputPort, _JUDGE_TO_READY);
            SET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(ScalerHdmi21MacTxPxTxMapping(enumOutputPort), _DP_RX_SOURCE_JUDGE);
            CLR_HDMI21_MAC_TX_DECODER_EN(ScalerHdmi21MacTxPxTxMapping(enumOutputPort));

            DebugMessageHDMI21Tx("[HDMI21_TX] DSC: STREAM reset\n");
        }
        return _FALSE;
    }

#if(_DSC_HDMI20_FORCE_DECODER == _OFF)
    if(GET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(ScalerHdmi21MacTxPxTxMapping(enumOutputPort)) == _LINK_TRAING_LEGACY)
    {
        ScalerDscVesaMuxSel(enumOutputPort, _JUDGE_TO_READY);
        ScalerDscPower(enumOutputPort, _JUDGE_TO_READY);
        SET_HDMI21_MAC_TX_INPUT_FROM(ScalerHdmi21MacTxPxTxMapping(enumOutputPort), _INPUT_FROM_DP);

        DebugMessageHDMI21Tx("[HDMI21_TX] DSC: HDMI sink in 2.0 mode ,DP RX DSC stream in ,Illegal !!\n");

        return _FALSE;
    }
#endif
    {
        switch(GET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(ScalerHdmi21MacTxPxTxMapping(enumOutputPort)))
        {
            case _DP_RX_SOURCE_JUDGE:

                ScalerDscClrBwEvent(enumOutputPort);
                SET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(ScalerHdmi21MacTxPxTxMapping(enumOutputPort), _TX_STREAM_READY);
                SET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(ScalerHdmi21MacTxPxTxMapping(enumOutputPort), _DOWN_STREAM_VESA);
                ScalerDscVesaMuxSel(enumOutputPort, _JUDGE_TO_READY);
                SET_HDMI21_MAC_TX_INPUT_FROM(ScalerHdmi21MacTxPxTxMapping(enumOutputPort), _INPUT_FROM_DP);

                return _FALSE;
                break;

            case _TX_STREAM_READY:

                return _TRUE;

            default:
                return _TRUE;
                break;
        }
    }
    return _FALSE;
}

//--------------------------------------------------
// Description  : HDMI 2p1 Tx to Px Mapping
// Input Value  : EnumOutputPort
// Output Value : TX Number
//--------------------------------------------------
UINT8 ScalerHdmi21MacTxPxTxMapping(EnumOutputPort enumOutputPort)
{
#if 1 // H5X Only have 1 TX
    return _HW_P0_HDMI21_MAC_TX_MAPPING;
#else
    UINT8 ucTxPortTemp = _TX_MAP_NONE;

    switch(enumOutputPort)
    {
#if(_P0_HDMI21_SUPPORT == _ON)

        case _P0_OUTPUT_PORT:

            ucTxPortTemp = _HW_P0_HDMI21_MAC_TX_MAPPING;

            break;
#endif

#if(_P1_HDMI21_SUPPORT == _ON)

        case _P1_OUTPUT_PORT:

            ucTxPortTemp = _HW_P1_HDMI21_MAC_TX_MAPPING;

            break;
#endif

#if(_P2_HDMI21_SUPPORT == _ON)

        case _P2_OUTPUT_PORT:

            ucTxPortTemp = _HW_P2_HDMI21_MAC_TX_MAPPING;

            break;
#endif

#if(_P3_HDMI21_SUPPORT == _ON)

        case _P3_OUTPUT_PORT:

            ucTxPortTemp = _HW_P3_HDMI21_MAC_TX_MAPPING;

            break;
#endif

        default:
            break;
    }
    return ucTxPortTemp;
#endif
}

//--------------------------------------------------
// Description  : HDMI 2p1 Px to Tx Mapping
// Input Value  : TX Number
// Output Value : EnumOutputPort
//--------------------------------------------------
EnumOutputPort ScalerHdmi21MacTxPxMapping(UINT8 ucTxIndex)
{
#if 1 // H5X Only have 1 TX
        return _P0_OUTPUT_PORT;
#else
    UINT8 ucPxPortTemp = _NO_OUTPUT_PORT;

    switch(ucTxIndex)
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)
        case _TX0:

            if(_HW_P0_HDMI21_MAC_TX_MAPPING == _TX0)
            {
                ucPxPortTemp = _P0_OUTPUT_PORT;
            }
            else if(_HW_P1_HDMI21_MAC_TX_MAPPING == _TX0)
            {
                ucPxPortTemp = _P1_OUTPUT_PORT;
            }
            else if(_HW_P2_HDMI21_MAC_TX_MAPPING == _TX0)
            {
                ucPxPortTemp = _P2_OUTPUT_PORT;
            }
            else if(_HW_P3_HDMI21_MAC_TX_MAPPING == _TX0)
            {
                ucPxPortTemp = _P3_OUTPUT_PORT;
            }

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)
        case _TX1:

            if(_HW_P0_HDMI21_MAC_TX_MAPPING == _TX1)
            {
                ucPxPortTemp = _P0_OUTPUT_PORT;
            }
            else if(_HW_P1_HDMI21_MAC_TX_MAPPING == _TX1)
            {
                ucPxPortTemp = _P1_OUTPUT_PORT;
            }
            else if(_HW_P2_HDMI21_MAC_TX_MAPPING == _TX1)
            {
                ucPxPortTemp = _P2_OUTPUT_PORT;
            }
            else if(_HW_P3_HDMI21_MAC_TX_MAPPING == _TX1)
            {
                ucPxPortTemp = _P3_OUTPUT_PORT;
            }

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)
        case _TX2:

            if(_HW_P0_HDMI21_MAC_TX_MAPPING == _TX2)
            {
                ucPxPortTemp = _P0_OUTPUT_PORT;
            }
            else if(_HW_P1_HDMI21_MAC_TX_MAPPING == _TX2)
            {
                ucPxPortTemp = _P1_OUTPUT_PORT;
            }
            else if(_HW_P2_HDMI21_MAC_TX_MAPPING == _TX2)
            {
                ucPxPortTemp = _P2_OUTPUT_PORT;
            }
            else if(_HW_P3_HDMI21_MAC_TX_MAPPING == _TX2)
            {
                ucPxPortTemp = _P3_OUTPUT_PORT;
            }

            break;
#endif

        default:
            break;
    }
    return ucPxPortTemp;
#endif
}

//--------------------------------------------------
// Description  : Hdmi 2p1 check Deep Color SRAM
// Input Value  : port
// Output Value : _STABLE or _UNSTABLE
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTxCheckDPCSRAM(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            return ScalerHdmi21MacTx0CheckDPCSRAM();

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            return ScalerHdmi21MacTx1CheckDPCSRAM();

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            return ScalerHdmi21MacTx2CheckDPCSRAM();

            break;
#endif

        case _TX_MAP_NONE:
        default:
            break;
    }
    return _UNSTABLE;
}

//--------------------------------------------------
// Description  : Hdmi 2p1 StreamHandler
// Input Value  : port
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTxStreamHandler(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            return ScalerHdmi21MacTx0StreamHandler();

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            return ScalerHdmi21MacTx1StreamHandler();

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            return ScalerHdmi21MacTx2StreamHandler();

            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : HDMI CAL FRL BW enought or not
// Input Value  : NONE
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTxBwCheck(EnumOutputPort enumOutputPort)
{
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)
    EnumHdmi21DataFlowMeteringResult enumHdmi21Tx0DataFlowMeteringResult = _HDMI21_DATA_FLOW_METERING_NONE;
#endif

    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)
        case _TX0:
            ScalerHdmi21MacTx0DataFlowMeterParameterSet();
            enumHdmi21Tx0DataFlowMeteringResult = ScalerHdmi21MacTx0DataFlowMetering();

            DebugMessageHDMI21Tx("[HDMI21_TX] ScalerHdmi21MacTxBwCheck Result=%d!!\n", enumHdmi21Tx0DataFlowMeteringResult);

            if((enumHdmi21Tx0DataFlowMeteringResult == _HDMI21_DATA_FLOW_METERING_PASS_NO_BORROW) ||
               (enumHdmi21Tx0DataFlowMeteringResult == _HDMI21_DATA_FLOW_METERING_PASS_BORROW) ||
               (enumHdmi21Tx0DataFlowMeteringResult == _HDMI21_DATA_FLOW_METERING_OVER))
            {
                return _TRUE;
            }
            else
            {
                return _FALSE;
            }
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)
        case _TX1:

            return _TRUE;

#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)
        case _TX2:

            return _TRUE;

#endif

        default:
            break;
    }
    return _TRUE;
}

#if(_AUDIO_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Get Audio Info From STx Audio
// Input Value  : EnumOutputPort, EnumAudioInfoType
// Output Value : Node
//--------------------------------------------------
UINT32 ScalerHdmi21MacTxGetAudioFormat(EnumOutputPort enumOutputPort, EnumAudioInfoType enumAudioInfoType)
{
    switch(enumAudioInfoType)
    {
        case _AUDIO_CHANNEL_COUNT:
            return ScalerStreamAudioGetChannelCount(enumOutputPort);
            break;

        case _AUDIO_CODING_TYPE:
            return ScalerStreamAudioGetCodingType(enumOutputPort);
            break;

        case _AUDIO_SAMPLING_FREQUENCY_TYPE:
            return ScalerStreamAudioGetSamplingFrequencyType(enumOutputPort);
            break;

        case _AUDIO_SAMPLING_FREQUENCY_HZ:
		#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
        #if(_AUDIO_SUPPORT == _ON)
            return drvif_Hdmi_get_audio_sample_freq();
        #endif
		#else
		return 0;
		#endif
            break;

        default:
            break;
    }

    return 0x00;
}

#endif // #if(_AUDIO_SUPPORT == _ON)

//--------------------------------------------------
// Description  : HDMI Tx Fine Tune
// Input Value  : OutputPort
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTxPktFineTune(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)
        case _TX0:
            ScalerHdmi21MacTx0PktFineTune();
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)
        case _TX1:
            ScalerHdmi21MacTx1PktFineTune();
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)
        case _TX2:
            ScalerHdmi21MacTx2PktFineTune();
#endif

        default:
            break;
    }
}

StructStreamDisplayCompensateInfo *ScalerDpStreamGetDisplayCompensateInfo(UINT8 ucOutputPort)
{
    g_stStreamDisplayCompensateInfo.b1TimingCompensationFlag = 0;
    g_stStreamDisplayCompensateInfo.b2HSyncCompensate = 0;
    g_stStreamDisplayCompensateInfo.b2HFrontPorchCompensate = 0;
    g_stStreamDisplayCompensateInfo.b2HBackPorchCompensate = 0;
    g_stStreamDisplayCompensateInfo.b2HWidthCompensate = 0;
    g_stStreamDisplayCompensateInfo.b4HDirectionCompensate = 0;

    return &g_stStreamDisplayCompensateInfo;
}


#if(_DSC_ENCODER_SUPPORT == _ON)
//--------------------------------------------------
// Description  : HDMI Tx Get timing info from DP
// Input Value  : OutputPort
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTxGetEncoderTimingInfo(EnumOutputPort enumOutputPort)
{
    ucTXn = ScalerHdmi21MacTxPxTxMapping(enumOutputPort);

//#ifdef H5X_HDMITX_FIXME // [FIX-ME]
    // Get Pixel Clk
    SET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _PIXEL_CLOCK));

    DebugMessageHDMI21Tx("[HDMI21_TX][DSC] input TMDS clk=%d\n", GET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(ucTXn));

    // Get Frame Rate
    SET_HDMI21_MAC_TX_INPUT_FRAME_RATE(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _FRAME_RATE));

    // GET Interlace Mode
    if(ScalerDpStreamGetElement(enumOutputPort, _INTERLACE_INFO) == _TRUE)
    {
        SET_HDMI21_MAC_TX_INPUT_INTERLACE(ucTXn);
        SET_HDMI21_MAC_TX_INPUT_FRAME_RATE(ucTXn, GET_HDMI21_MAC_TX_INPUT_FRAME_RATE(ucTXn) / 2);
    }
    else
    {
        CLR_HDMI21_MAC_TX_INPUT_INTERLACE(ucTXn);
    }

    // SET COLOR SPACE
    SET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _COLOR_SPACE));

    // Set Color Depth
    if(ScalerDpStreamGetElement(enumOutputPort, _COLOR_DEPTH) == _DP_STREAM_COLOR_DEPTH_6_BITS)
    {
        SET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(ucTXn, _DP_STREAM_COLOR_DEPTH_8_BITS);
    }
    else
    {
        SET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _COLOR_DEPTH));
    }
    if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(ucTXn) == _COLOR_SPACE_YCBCR422)
        SET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(ucTXn, _DP_STREAM_COLOR_DEPTH_8_BITS);

    // Get Colorimetry
    SET_HDMI21_MAC_TX_INPUT_COLORIMETRY(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _COLORIMETRY));

    // Get EXT_Colorimetry
    SET_HDMI21_MAC_TX_INPUT_EXT_COLORIMETRY(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _EXT_COLORIMETRY));

    // Get RGB QUANTIZATION RANGE
    SET_HDMI21_MAC_TX_INPUT_COLOR_RGB_QUANTIZATION_RANGE(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _COLOR_RGB_QUANTIZATION_RANGE));

    // Get YCC QUANTIZATION RANGE
    SET_HDMI21_MAC_TX_INPUT_COLOR_YCC_QUANTIZATION_RANGE(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _COLOR_YCC_QUANTIZATION_RANGE));

    // measure timing infor
    g_pstHdmi21MacTxInputTimingInfo[ucTXn] = ScalerDpStreamGetDisplayTimingInfo(enumOutputPort);
    g_pstHdmi21MacTxInputTimingInfo[ucTXn]->b1Interlace = GET_HDMI21_MAC_TX_INPUT_INTERLACE(ucTXn);
    g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usVFreq = (UINT16)(GET_HDMI21_MAC_TX_INPUT_FRAME_RATE(ucTXn)*10);
//#endif

    return;
}
#endif // #ifdef NOT_USED_NOW


//--------------------------------------------------
// Description  : HDMI Tx Get timing info from DP
// Input Value  : OutputPort
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTxGetDecoderTimingInfo(EnumOutputPort enumOutputPort)
{
    NoteMessageHDMI21Tx("[HDMI21_TX] _DSC_DECODER_SUPPORT NOT SUPPORT!!\n");
    return;
}

//--------------------------------------------------
// Description  : HDMI Tx Get timing info from DP
// Input Value  : OutputPort
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTxGetDpTimingInfo(EnumOutputPort enumOutputPort)
{
    ucTXn = ScalerHdmi21MacTxPxTxMapping(enumOutputPort);

    // Get Pixel Clk
    SET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _PIXEL_CLOCK));

    DebugMessageHDMI21Tx("[HDMI21_TX] Input Pixel Clk=%d\n", (UINT32)GET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(ucTXn));

    // Get Frame Rate
    SET_HDMI21_MAC_TX_INPUT_FRAME_RATE(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _FRAME_RATE));

    // GET Interlace Mode
    if(ScalerDpStreamGetElement(enumOutputPort, _INTERLACE_INFO) == _TRUE)
    {
        SET_HDMI21_MAC_TX_INPUT_INTERLACE(ucTXn);
        SET_HDMI21_MAC_TX_INPUT_FRAME_RATE(ucTXn, GET_HDMI21_MAC_TX_INPUT_FRAME_RATE(ucTXn) / 2);
    }
    else
    {
        CLR_HDMI21_MAC_TX_INPUT_INTERLACE(ucTXn);
    }

    // SET COLOR SPACE
    SET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _COLOR_SPACE));

    // Set Color Depth
    if(ScalerDpStreamGetElement(enumOutputPort, _COLOR_DEPTH) == _DP_STREAM_COLOR_DEPTH_6_BITS)
    {
        SET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(ucTXn, _DP_STREAM_COLOR_DEPTH_8_BITS);
    }
    else
    {
        SET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _COLOR_DEPTH));
    }

    if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(ucTXn) == _COLOR_SPACE_YCBCR422)
        SET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(ucTXn, _DP_STREAM_COLOR_DEPTH_8_BITS);

    // GET Pixel Mode
    SET_HDMI21_MAC_TX_INPUT_PIXEL_MODE(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _OUTPUT_PIXEL_MODE));

    // Get Colorimetry
    SET_HDMI21_MAC_TX_INPUT_COLORIMETRY(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _COLORIMETRY));

    // Get EXT_Colorimetry
    SET_HDMI21_MAC_TX_INPUT_EXT_COLORIMETRY(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _EXT_COLORIMETRY));

    // Get RGB QUANTIZATION RANGE
    SET_HDMI21_MAC_TX_INPUT_COLOR_RGB_QUANTIZATION_RANGE(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _COLOR_RGB_QUANTIZATION_RANGE));

    // Get YCC QUANTIZATION RANGE
    SET_HDMI21_MAC_TX_INPUT_COLOR_YCC_QUANTIZATION_RANGE(ucTXn, ScalerDpStreamGetElement(enumOutputPort, _COLOR_YCC_QUANTIZATION_RANGE));

    // measure timing infor
    g_pstHdmi21MacTxInputTimingInfo[ucTXn] = ScalerDpStreamGetDisplayTimingInfo(enumOutputPort);
    if(g_pstHdmi21MacTxInputTimingInfo[ucTXn]) {
	    g_pstHdmi21MacTxInputTimingInfo[ucTXn]->b1Interlace = GET_HDMI21_MAC_TX_INPUT_INTERLACE(ucTXn);
	    g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usVFreq = (UINT16)(GET_HDMI21_MAC_TX_INPUT_FRAME_RATE(ucTXn)*10);
    }
    g_pstHdmi21MacTxDPFCompensateInfo[ucTXn] = ScalerDpStreamGetDisplayCompensateInfo(enumOutputPort);

    return;
}

//--------------------------------------------------
// Description  : HDMI Tx Get timing info from ST
// Input Value  : OutputPort
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTxGetStreamTimingInfo(EnumOutputPort enumOutputPort)
{
    UINT8 ucTXn = 0;
    UINT8 uci = 0;

    ucTXn = ScalerHdmi21MacTxPxTxMapping(enumOutputPort);

    // measure timing & element
    switch(GET_HDMI21_MAC_TX_INPUT_FROM(ucTXn))
    {
        case _INPUT_FROM_DP:
        case _INPUT_FROM_DP_DSC_BYPASS:

            for(uci = 0; uci < 3; uci++)
            {
                ScalerHdmi21MacTxGetDpTimingInfo(enumOutputPort);
                if(ucTXn == ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
                {
                    break;
                }
            }

            break;

        case _INPUT_FROM_DECODER:

            for(uci = 0; uci < 3; uci++)
            {
                ScalerHdmi21MacTxGetDecoderTimingInfo(enumOutputPort);

                if(ucTXn == ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
                {
                    break;
                }
            }
            break;
#if(_DSC_ENCODER_SUPPORT == _ON)
        case _INPUT_FROM_ENCODER:

            for(uci = 0; uci < 3; uci++)
            {
                ScalerHdmi21MacTxGetEncoderTimingInfo(enumOutputPort);

                if(ucTXn == ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
                {
                    break;
                }
            }

            break;
#endif
        default:

            break;
    }

    ScalerHdmi21MacTx0RepetitionDecision();

    DebugMessageHDMI21Tx("[HDMI21_TX] TX-%d\n", (UINT32)ucTXn);
    if(g_pstHdmi21MacTxInputTimingInfo[ucTXn]){
        DebugMessageHDMI21Tx("[HDMI21_TX] Input Timing H Polarity=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[ucTXn]->b1HSP);
        DebugMessageHDMI21Tx("[HDMI21_TX] Input Timing V Polarity=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[ucTXn]->b1VSP);
        DebugMessageHDMI21Tx("[HDMI21_TX] Input Timing HFreq=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usHFreq);
        DebugMessageHDMI21Tx("[HDMI21_TX] Input Timing HStart=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usHStart);
        DebugMessageHDMI21Tx("[HDMI21_TX] Input Timing HSW=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usHSWidth);
        DebugMessageHDMI21Tx("[HDMI21_TX] Input Timing HWidth=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usHWidth);
        DebugMessageHDMI21Tx("[HDMI21_TX] Input Timing HTotal=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usHTotal);
        DebugMessageHDMI21Tx("[HDMI21_TX] Input Timing VFreq=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usVFreq);
        DebugMessageHDMI21Tx("[HDMI21_TX] Input Timing VStart=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usVStart);
        DebugMessageHDMI21Tx("[HDMI21_TX] Input Timing VSW=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usVSWidth);
        DebugMessageHDMI21Tx("[HDMI21_TX] Input Timing VHeight=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usVHeight);
        DebugMessageHDMI21Tx("[HDMI21_TX] Input Timing VTotal=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usVTotal);
    }
    else{
        ErrorMessageHDMI21Tx("[HDMI21_TX] TimingInfo is NULL!!\n");
    }
    // FOR encoder test
    SET_HDMI21_MAC_TX_INFOR_OK(ucTXn);

    return;
}

//--------------------------------------------------
// Description  : HDMI2.1 Tx Reset
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmi21MacTxReset(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            ScalerHdmi21MacTx0Reset();

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            ScalerHdmi21MacTx1Reset();

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            ScalerHdmi21MacTx2Reset();

            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }
}

//--------------------------------------------------
// Description  : HDMI2.1 Tx Initial
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmi21MacTxInitial(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            ScalerHdmi21MacTx0Initial();

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            ScalerHdmi21MacTx1Initial();

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            ScalerHdmi21MacTx2Initial();

            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }
}

#if(_DSC_ENCODER_SUPPORT == _ON)
//--------------------------------------------------
// Description  : HDMI Tx Get Compress table 7-35 all parameter
// Input Value  : OutputPort
// Output Value : StructDSCHDMICOMPRESSTable => index value of this structure
//--------------------------------------------------
UINT32 ScalerHdmi21MacTxGetCompressTableIndex(EnumOutputPort enumOutputPort, EnumDCompressTableIndex enumIndex, UINT8 ucNum)
{
    StructDSCHDMICOMPRESSTable pstCompressTable[COMP_TABLE_INDEX_MAX] = {{0},{0},{0},{0},{0}};

    if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmiMacTxPxTxMapping(enumOutputPort)) == _HDMI21_FRL_3G)
    {
        unsigned int compTableNum = sizeof(t3G_3LANE_444TABLE) / sizeof(StructDSCHDMICOMPRESSTable);
        if (ucNum >= compTableNum) {
            InfoMessageHDMI21Tx("[HDMI21_TX] Get Index[%d] Num(%d) >= t3G_3LANE_444TABLE(%d)\n", enumOutputPort, ucNum, compTableNum);
            return 0;
        }
        else {
            memcpy(pstCompressTable, t3G_3LANE_444TABLE, sizeof(t3G_3LANE_444TABLE));
        }
    }
    else if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmiMacTxPxTxMapping(enumOutputPort)) == _HDMI21_FRL_6G_3LANES)
    {
        unsigned int compTableNum = sizeof(t6G_3LANE_444TABLE) / sizeof(StructDSCHDMICOMPRESSTable);
        if (ucNum >= compTableNum) {
            InfoMessageHDMI21Tx("[HDMI21_TX] Get Index[%d] Num(%d) >= t6G_3LANE_444TABLE(%d)\n", enumOutputPort, ucNum, compTableNum);
            return 0;
        }
        else {
            memcpy(pstCompressTable, t6G_3LANE_444TABLE, sizeof(t6G_3LANE_444TABLE));
        }
    }
#if 0
    if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmiMacTxPxTxMapping(enumOutputPort)) == _HDMI21_FRL_3G_4LANES)
    {
        unsigned int compTableNum = sizeof(t3G_4LANE_444TABLE) / sizeof(StructDSCHDMICOMPRESSTable);
        if (ucNum >= compTableNum) {
            InfoMessageHDMITx("[HDMI21_TX] Get Index[%d] Num(%d) >= t3G_4LANE_444TABLE(%d)\n", ucNum, compTableNum);
            return 0;
        }
        else {
            memcpy(pstCompressTable, t3G_4LANE_444TABLE, sizeof(t3G_4LANE_444TABLE));
        }
    }
#endif
    else if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmiMacTxPxTxMapping(enumOutputPort)) == _HDMI21_FRL_6G_4LANES)
    {
        unsigned int compTableNum = sizeof(t6G_4LANE_444TABLE) / sizeof(StructDSCHDMICOMPRESSTable);
        if (ucNum >= compTableNum) {
            InfoMessageHDMI21Tx("[HDMI21_TX] Get Index[%d] Num(%d) >= t6G_4LANE_444TABLE(%d)\n", enumOutputPort, ucNum, compTableNum);
            return 0;
        }
        else {
            memcpy(pstCompressTable, t6G_4LANE_444TABLE, sizeof(t6G_4LANE_444TABLE));
        }
    }
    else if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmiMacTxPxTxMapping(enumOutputPort)) == _HDMI21_FRL_8G)
    {
        unsigned int compTableNum = sizeof(t8G_4LANE_444TABLE) / sizeof(StructDSCHDMICOMPRESSTable);
        if (ucNum >= compTableNum) {
            InfoMessageHDMI21Tx("[HDMI21_TX] Get Index[%d] Num(%d) >= t8G_4LANE_444TABLE(%d)\n", enumOutputPort, ucNum, compTableNum);
            return 0;
        }
        else {
            memcpy(pstCompressTable, t8G_4LANE_444TABLE, sizeof(t8G_4LANE_444TABLE));
        }
    }
    else if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmiMacTxPxTxMapping(enumOutputPort)) == _HDMI21_FRL_10G)
    {
        unsigned int compTableNum = sizeof(t10G_4LANE_444TABLE) / sizeof(StructDSCHDMICOMPRESSTable);
        if (ucNum >= compTableNum) {
            InfoMessageHDMI21Tx("[HDMI21_TX] Get Index[%d] Num(%d) >= t10G_4LANE_444TABLE(%d)\n", enumOutputPort, ucNum, compTableNum);
            return 0;
        }
        else {
            memcpy(pstCompressTable, t10G_4LANE_444TABLE, sizeof(t10G_4LANE_444TABLE));
        }
    }
    else if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmiMacTxPxTxMapping(enumOutputPort)) == _HDMI21_FRL_12G)
    {
        unsigned int compTableNum = sizeof(t12G_4LANE_444TABLE) / sizeof(StructDSCHDMICOMPRESSTable);
        if (ucNum >= compTableNum) {
            InfoMessageHDMI21Tx("[HDMI21_TX] Get Index[%d] Num(%d) >= t12G_4LANE_444TABLE(%d)\n", enumOutputPort, ucNum, compTableNum);
            return 0;
        }
        else {
            memcpy(pstCompressTable, t12G_4LANE_444TABLE, sizeof(t12G_4LANE_444TABLE));
        }
    }

    switch(enumIndex)
    {
        case _HACTIVE:

            return pstCompressTable[ucNum].usHactive;
            break;

        case _VACTIVE:

            return pstCompressTable[ucNum].usVcative;
            break;

        case _COMPRESS_FRAME_RATE:

            return pstCompressTable[ucNum].ucFramrate;
            break;

        case _BPP:

            return pstCompressTable[ucNum].ulBpp_multi_16;
            break;

        case _HCACTIVE:

            return pstCompressTable[ucNum].usHCactive;
            break;

        case _HBLANK:
            return pstCompressTable[ucNum].usHCblank;
            break;

        default:

            break;
    }
    return 0;
}

//--------------------------------------------------
// Description  : HDMI Tx Get Bpp from table 7-35
// Input Value  : OutputPort
// Output Value : BPP
//--------------------------------------------------
UINT32 ScalerHdmi21MacTxGetLimitBpp(EnumOutputPort enumOutputPort, UINT16 usHactive, UINT16 usVactive, UINT8 ucFrame)
{
    //UINT8 ucTXn = 0;
    UINT8 uci = 0;
    UINT32 ul_bpp = _HDMI21_BPP_DEFAULT;

    if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
        ErrorMessageHDMI21Tx("[HDMI21_TX] TimingInfo is NULL@TxGetLimitBpp\n");
        return 0;
    }

    //ucTXn = ScalerHdmi21MacTxPxTxMapping(enumOutputPort);
    // usHactive = g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usHWidth;
    // usVactive = g_pstHdmi21MacTxInputTimingInfo[ucTXn]->usVHeight;
    for(uci = 0; uci < COMP_TABLE_INDEX_MAX; uci++)
    {
        if((usHactive == ScalerHdmi21MacTxGetCompressTableIndex(enumOutputPort, _HACTIVE, uci)) &&
           (usVactive == ScalerHdmi21MacTxGetCompressTableIndex(enumOutputPort, _VACTIVE, uci)) &&
           (ucFrame == ScalerHdmi21MacTxGetCompressTableIndex(enumOutputPort, _COMPRESS_FRAME_RATE, uci)))
        {
            ul_bpp = ScalerHdmi21MacTxGetCompressTableIndex(enumOutputPort, _BPP, uci);
            break;
        }
    }

    if(uci == COMP_TABLE_INDEX_MAX)
        ErrorMessageHDMI21Tx("[HDMI21_TX] NO MATCHED _BPP, Default=%d\n", (UINT32)ul_bpp);
    else
        DebugMessageHDMI21Tx("[HDMI21_TX] TxGetLimitBpp=%d\n", (UINT32)ul_bpp);

    return ul_bpp;
}
//--------------------------------------------------
// Description  : HDMI Tx Get HCactive from table 7-35
// Input Value  : OutputPort
// Output Value : HCactive
//--------------------------------------------------

UINT32 ScalerHdmi21MacTxGetHCactive(EnumOutputPort enumOutputPort, UINT16 usHactive, UINT16 usVactive, UINT8 ucFrame)
{
    //UINT8 ucTXn = 0;
    UINT8 uci = 0;
    UINT32 ul_hcactive = 0;

    if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
        ErrorMessageHDMI21Tx("[HDMI21_TX] TimingInfo is NULL@TxGetHCactive\n");
        return 0;
    }

    //ucTXn = ScalerHdmi21MacTxPxTxMapping(enumOutputPort);
    for(uci = 0; uci < COMP_TABLE_INDEX_MAX; uci++)
    {
        if((usHactive == ScalerHdmi21MacTxGetCompressTableIndex(enumOutputPort, _HACTIVE, uci)) &&
           (usVactive == ScalerHdmi21MacTxGetCompressTableIndex(enumOutputPort, _VACTIVE, uci)) &&
           (ucFrame == ScalerHdmi21MacTxGetCompressTableIndex(enumOutputPort, _COMPRESS_FRAME_RATE, uci)))
        {
            ul_hcactive = ScalerHdmi21MacTxGetCompressTableIndex(enumOutputPort, _HCACTIVE, uci);
            break;
        }
    }

    if(uci == COMP_TABLE_INDEX_MAX)
        ErrorMessageHDMI21Tx("[HDMI21_TX]  NO MATCHED _HCACTIVE!!, default=%d\n", (UINT32)ul_hcactive);
    else
        DebugMessageHDMI21Tx("[HDMI21_TX] TxGetLimitBpp=%d\n", (UINT32)ul_hcactive);

    return ul_hcactive;

}
//--------------------------------------------------
// Description  : HDMI Tx Get HCblank from table 7-35
// Input Value  : OutputPort
// Output Value : HCblank
//--------------------------------------------------
UINT32 ScalerHdmi21MacTxGetHCblank(EnumOutputPort enumOutputPort, UINT16 usHactive, UINT16 usVactive, UINT8 ucFrame)
{
    //UINT8 ucTXn = 0;
    UINT8 uci = 0;
    UINT32 ul_hcblank = 0;

    if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
        ErrorMessageHDMI21Tx("[HDMI21_TX] TimingInfo is NULL@TxGetHCblank\n");
        return 0;
    }

    //ucTXn = ScalerHdmi21MacTxPxTxMapping(enumOutputPort);
    for(uci = 0; uci < COMP_TABLE_INDEX_MAX; uci++)
    {
        if((usHactive == ScalerHdmi21MacTxGetCompressTableIndex(enumOutputPort, _HACTIVE, uci)) &&
           (usVactive == ScalerHdmi21MacTxGetCompressTableIndex(enumOutputPort, _VACTIVE, uci)) &&
           (ucFrame == ScalerHdmi21MacTxGetCompressTableIndex(enumOutputPort, _COMPRESS_FRAME_RATE, uci)))
        {
            ul_hcblank = ScalerHdmi21MacTxGetCompressTableIndex(enumOutputPort, _HBLANK, uci);
            break;
        }
    }

    if(uci == COMP_TABLE_INDEX_MAX)
        ErrorMessageHDMI21Tx("[HDMI21_TX] NO MATCHED _HBLANK, Default=%d\n", (UINT32)ul_hcblank);
    else
        DebugMessageHDMI21Tx("[HDMI21_TX] TxGetLimitBpp=%d\n", (UINT32)ul_hcblank);

    return ul_hcblank;

}
#endif

//--------------------------------------------------
// Description  : HDMI2.1 Tx Initial
// Input Value  : None
// Output Value : None
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTxGetVideoStatus(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            return ScalerHdmi21MacTx0VideoMuxStatus();

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            return ScalerHdmi21MacTx1VideoMuxStatus();

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            return ScalerHdmi21MacTx2VideoMuxStatus();

            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }
    return _FALSE;
}

//--------------------------------------------------
// Description  : HDMI2.1 Tx Initial
// Input Value  : None
// Output Value : None
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTxGetVideoStart(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            return GET_HDMI21_MAC_VIDEO_START(_TX0);

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            return GET_HDMI21_MAC_VIDEO_START(_TX1);

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            return GET_HDMI21_MAC_VIDEO_START(_TX2);

            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }
    return _FALSE;
}

//--------------------------------------------------
// Description  : HDMI2.1 Input From DPrx or DSC decoder or DSC encoder
// Input Value  : PORT
// Output Value : Input from where
//--------------------------------------------------
UINT8 ScalerHdmi21MacTxGetInputStreamFrom(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            return GET_HDMI21_MAC_TX_INPUT_FROM(_TX0);

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            return GET_HDMI21_MAC_TX_INPUT_FROM(_TX1);

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            return GET_HDMI21_MAC_TX_INPUT_FROM(_TX2);

            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }
    return _FALSE;
}
//--------------------------------------------------
// Description  : Hdmi21 in 200 msec timouit polling
// Input Value  : port
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTxCheckUrgentLinkTraining(void)
{
    if(GET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0) == _HDMI21_LT_LTS3)
    {
        return _TRUE;
    }
    else if(GET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0) == _HDMI21_LT_LTS4)
    {
        return _TRUE;
    }
    else
    {
        return _FALSE;
    }
}

#endif

