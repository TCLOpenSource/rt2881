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
#include "../include/ScalerTx.h"
#include "../include/hdmitx_phy_struct_def.h"
#include "../include/HdmiEdid/hdmitx_edid.h"
#include "../include/EdidCommon.h"
#include <rtk_kdriver/pcbMgr.h>
//#include <rbus/ppoverlay_reg.h>
//#include <rbus/ppoverlay_outtg_reg.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_driver.h>
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0Include.h"
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
#include <rtk_kdriver/tvscalercontrol/hdmirx/hdmifun.h>
#include <rtk_kdriver/measure/rtk_measure.h>
#include <rtk_kdriver/tvscalercontrol/hdmirx/hdmifun.h>

#endif



//#include "rtk_i2c.h"
/*************************************************/
UINT8  pucSpecialTv4EDIDCheckAddr[28] = {0x08, 0x09, 0x7E, 0xFF, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x80, 0x81, 0x82, 0x83, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0};
UINT8  pucSpecialTv4EDIDCheckValue[28] = {0x04, 0x72, 0x01, 0x0B, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x48, 0x32, 0x37, 0x37, 0x48, 0x55, 0x0A, 0x02, 0x03, 0x21, 0xF5, 0x55, 0x02, 0x00, 0xB8, 0xA1, 0x38, 0x14, 0x40};
UINT8  pucSpecialTvEDIDCheckValue[11] = {0x00, 0x00, 0x00, 0xFC, 0x00, 0x4c, 0x47, 0x20, 0x54, 0x56, 0x0a};
UINT8  pucSpecialTv2EDIDCheckValue[12] = {0x00, 0x00, 0x00, 0xFC, 0x00, 0x52, 0x54, 0x4b, 0x20, 0x54, 0x56, 0x0a};
UINT8  pucSpecialTv3EDIDCheckValue[16] = {0x00, 0x00, 0x00, 0xFC, 0x0, 0x4c, 0x53, 0x32, 0x38, 0x41, 0x47, 0x37, 0x30, 0x30, 0x4e, 0xa};
UINT8  pucSpecialTv5EDIDCheckValue[18] = {0x00, 0x00, 0x00, 0xFC, 0x00, 0x53, 0x41, 0x4D, 0x53, 0x55, 0x4E, 0x47, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20};
UINT8  pucSpecialTv6EDIDCheckValue[2] = {0x4D, 0xD9};
#if 0 // TEST: Modify VG28UQL1A: 56 47 32 38 55 51 4C 31 41 0A
UINT8  pucVg28uqlEDIDCheckValue[15] = {0x00, 0x00, 0x00, 0xFC, 0x00, 0x56, 0x47, 0x32, 0x38, 0x55, 0x51, 0x4c,0x31, 0x41, 0x0a};
#endif
#if(_HDMI_TX_CTS_TEST == _ON)
#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)
UINT8   pucCTS4kMNTEDIDCheckValue[14] = {0x00, 0x00, 0x00, 0xFC, 0x00, 0x48, 0x44, 0x4D, 0x49, 0x20, 0x54, 0x50, 0x41, 0x0A};
#endif
UINT8  pucCTSIntMNTEDIDCheckValue[14] = {0x00, 0x00, 0x00, 0xFC, 0x00, 0x48, 0x44, 0x4D, 0x49, 0x20, 0x4C, 0x4C, 0x43, 0x0A};
#endif
/************************************************/
//#include <mach/platform.h>

UINT8 bHD21_480P_576P = 0;
extern BOOLEAN g_bHdmiMacTx0FindCtsTestEdid;
#define EDID_SET_HDMI_MAC_TX0_FIND_CTS_TEST_EDID()                   (g_bHdmiMacTx0FindCtsTestEdid = _TRUE)
#define EDID_CLR_HDMI_MAC_TX0_FIND_CTS_TEST_EDID()                   (g_bHdmiMacTx0FindCtsTestEdid = _FALSE)

#define HDMITX_MAX_VRR_RATE 0X90
#define HDMITX_MIN_VRR_RATE 0X30

#define READ_EDID_FAIL_RETRY_COUNT (5)

/************************
// Functoin/Macro Definition
//
*************************/
BOOLEAN g_bHdmiMacTx0SegmentReadable;
BOOLEAN g_bHdmiMacTx0FindSpecialTv;
BOOLEAN g_bHdmiMacTx0FindSpecialTv2;
BOOLEAN g_bHdmiMacTx0FindSpecialTv3;
BOOLEAN g_bHdmiMacTx0FindSpecialTv4;
BOOLEAN g_bHdmiMacTx0FindSpecialTv5;
BOOLEAN g_bHdmiMacTx0FindSpecialTv6;
//BOOLEAN g_bHdmiMacTx0FindCtsTestEdid;
extern BOOLEAN g_bHdmiMacTx0FindCtsTestEdid;
StructHDMIEDIDInfo g_stHdmiMacTx0EDIDInfo  = {0};
StructHDMICTSEDIDInfo g_stHdmiMacTx0CtsEdidInfo;
UINT8  g_ucOutputTxInternalEDIDChecksumFixed=0;
UINT16 usCtaExtBasicSupportAddr = EDID_BLOCK1_BASIC_SUPPORT_ADDR;
UINT16 usExtTagBaseAddr = EDID_BLOCK1_TAG_BASE;
#if(_HDMI_TX_CTS_TEST == _ON)
BOOLEAN g_bHdmiMacTx0FindAudioCtsTestEdid;
#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)
BOOLEAN g_bHdmiMacTx0Find4kCtsTestEdid;
#endif
#endif // End of #if(_HDMI_TX_CTS_TEST == _ON)
extern UINT16 pusCtaDataBlockAddr[_CTA_TOTAL_DB + 1];
UINT32 g_TxPixelClock1024;

#define SET_HDMI_MAC_TX0_DID_PRFER_TIMING_ADDR(x)               (g_stHdmiMacTx0EDIDInfo.usDidPreferTimingAddr = (x))
#define GET_HDMI_MAC_TX0_DID_PRFER_TIMING_ADDR()                (g_stHdmiMacTx0EDIDInfo.usDidPreferTimingAddr)

#define SET_HDMI_MAC_TX0_FIND_SPECIAL_TV4()                (g_bHdmiMacTx0FindSpecialTv4 = _TRUE)
#define CLR_HDMI_MAC_TX0_FIND_SPECIAL_TV4()                (g_bHdmiMacTx0FindSpecialTv4 = _FALSE)
#define GET_HDMI_MAC_TX0_FIND_SPECIAL_TV4()                (g_bHdmiMacTx0FindSpecialTv4)

#define SET_HDMI_MAC_TX0_FIND_SPECIAL_TV()                (g_bHdmiMacTx0FindSpecialTv = _TRUE)
#define CLR_HDMI_MAC_TX0_FIND_SPECIAL_TV()                (g_bHdmiMacTx0FindSpecialTv = _FALSE)
#define GET_HDMI_MAC_TX0_FIND_SPECIAL_TV()                (g_bHdmiMacTx0FindSpecialTv)

#define SET_HDMI_MAC_TX0_FIND_SPECIAL_TV2()                (g_bHdmiMacTx0FindSpecialTv2 = _TRUE)
#define CLR_HDMI_MAC_TX0_FIND_SPECIAL_TV2()                (g_bHdmiMacTx0FindSpecialTv2 = _FALSE)
#define GET_HDMI_MAC_TX0_FIND_SPECIAL_TV2()                (g_bHdmiMacTx0FindSpecialTv2)

#define SET_HDMI_MAC_TX0_FIND_SPECIAL_TV3()                (g_bHdmiMacTx0FindSpecialTv3 = _TRUE)
#define CLR_HDMI_MAC_TX0_FIND_SPECIAL_TV3()                (g_bHdmiMacTx0FindSpecialTv3 = _FALSE)
#define GET_HDMI_MAC_TX0_FIND_SPECIAL_TV3()                (g_bHdmiMacTx0FindSpecialTv3)

#define SET_HDMI_MAC_TX0_FIND_SPECIAL_TV5()                (g_bHdmiMacTx0FindSpecialTv5 = _TRUE)
#define CLR_HDMI_MAC_TX0_FIND_SPECIAL_TV5()                (g_bHdmiMacTx0FindSpecialTv5 = _FALSE)
#define GET_HDMI_MAC_TX0_FIND_SPECIAL_TV5()                (g_bHdmiMacTx0FindSpecialTv5)

#define SET_HDMI_MAC_TX0_FIND_SPECIAL_TV6()                (g_bHdmiMacTx0FindSpecialTv6 = _TRUE)
#define CLR_HDMI_MAC_TX0_FIND_SPECIAL_TV6()                (g_bHdmiMacTx0FindSpecialTv6 = _FALSE)
#define GET_HDMI_MAC_TX0_FIND_SPECIAL_TV6()                (g_bHdmiMacTx0FindSpecialTv6)

#define SET_HDMI_MAC_TX0_EDID_MAX_RFL_RATE(x)                   (g_stHdmiMacTx0EDIDInfo.b4Max_FRL_LR = (x))
#define GET_HDMI_MAC_TX0_EDID_MAX_RFL_RATE()                    (g_stHdmiMacTx0EDIDInfo.b4Max_FRL_LR)

#define GET_HDMI_MAC_TX0_DTD_NUM_NEED_REPLACED()                (g_stHdmiMacTx0CtsEdidInfo.b4DTDNumNeedReplaced)
#define SET_HDMI_MAC_TX0_DTD_NUM_NEED_REPLACED(x)               (g_stHdmiMacTx0CtsEdidInfo.b4DTDNumNeedReplaced = (x))
#define CLR_HDMI_MAC_TX0_DTD_NUM_NEED_REPLACED()                (g_stHdmiMacTx0CtsEdidInfo.b4DTDNumNeedReplaced = 0)

#if(_HDMI_TX_CTS_TEST == _ON)

#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)
#define GET_HDMI_MAC_TX0_FIND_4K_CTS_TEST_EDID()                (g_bHdmiMacTx0Find4kCtsTestEdid)
#define SET_HDMI_MAC_TX0_FIND_4K_CTS_TEST_EDID()                (g_bHdmiMacTx0Find4kCtsTestEdid = _TRUE)
#define CLR_HDMI_MAC_TX0_FIND_4K_CTS_TEST_EDID()                (g_bHdmiMacTx0Find4kCtsTestEdid = _FALSE)
#endif
#define GET_HDMI_MAC_TX0_FIND_AUDIO_CTS_TEST_EDID()             (g_bHdmiMacTx0FindAudioCtsTestEdid)
#define SET_HDMI_MAC_TX0_FIND_AUDIO_CTS_TEST_EDID()             (g_bHdmiMacTx0FindAudioCtsTestEdid = _TRUE)
#define CLR_HDMI_MAC_TX0_FIND_AUDIO_CTS_TEST_EDID()             (g_bHdmiMacTx0FindAudioCtsTestEdid = _FALSE)

#endif // End of #if(_HDMI_TX_CTS_TEST == _ON)
BOOLEAN ScalerHdmiMacTx0EdidIICRead(UINT8 ucReadType);
void ScalerHdmiMacTx0EdidAnalysis(void);
void ScalerHdmiMacTx0PlugNotify(void);
void ScalerEdidGetCtaExtDbAddress(UINT16 *pusCtaDataBlockAddr);
/************************
// Debug Functoin/Macro Definition
//
*************************/
extern void ScalerHdmiMacTx0RepeaterEdidModifyVSVDB_HDR10(void);
extern void ScalerHdmiMacTx0RepeaterEdidModifySHDR_DB(void);
extern UINT8 ScalerEdidCheckSumCorrectCheck(UINT8 *pucDdcRamAddr,UINT16 usStartAddr);

#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
extern void ScalerHdmiMacTx0RepeaterEdidInfoDump(void);
extern void ScalerHdmiMacTx0RepeaterEdidInfoUpdate(EnumOutputPort enumOutputPort);
extern UINT8 ScalerHdmiMacTx0RepeaterGetEdidSupport(EnumEdidFeatureType enumEdidFeature);
extern void ScalerHdmiMacTx0RepeaterClrSinkTxSupportTimingCount(void);
#endif
extern void cmpBypassModeMatchTiming(UINT8 index);
extern void timingMatchForSourceMode(TX_OUT_MODE_SELECT_TYPE mode);
extern void checkVrrVstart(void);
extern unsigned char drvif_Hdmi_Multi_port_GetFrlMode(unsigned char port);

#define EdidMacTxPxMapping(x) (_P0_OUTPUT_PORT)
#define EdidMacPxTxMapping(x) (_HW_P0_HDMI_MAC_TX_MAPPING)
#define HDMITX_OutputTiming_For4N_64 (64)
#define HDMITX_OutputTiming_For4N_120 (120)
#define HDMITX_OutputTiming_For4N_124 (124)
#define HDMITX_OutputTiming_For4N_128 (128)
#define HDMITX_OutputTiming_For4N_132 (132)
#define HDMITX_OutputTiming_For4N_136 (136)
#define HDMITX_OutputTiming_For4N_238 (238)
#define HDMITX_OutputTiming_For4N_264 (264)
#define HDMITX_OutputTiming_For4N_268 (268)
#define HDMITX_OutputTiming_For4N_858 (858)
#define HDMITX_OutputTiming_For4N_864 (864)
#define HDMITX_OutputTiming_For4N_868 (868)
#define HDMITX_OutputTiming_For4N_872 (872)
#define HDMITX_OutputTiming_For4N_240 (240)
#define HDMITX_OutputTiming_For4N_1716 (1716)
#define HDMITX_OutputTiming_For4N_1720 (1720)
#define HDMITX_OutputTiming_For4N_1728 (1728)
#define HDMITX_OutputTiming_For4N_1736 (1736)
UINT8 bHD_480i_576i;
UINT8 bHD21_720_480i;
UINT8 bHD20_needRetiming;
UINT8 bHD21_needUnvalid;
UINT8 needRetiming_index;
UINT8 needUnvalid_index;


#define SET_HDMI_HD_480i_576i()                   (bHD_480i_576i = _TRUE)
#define CLR_HDMI_HD_480i_576i()                   (bHD_480i_576i = _FALSE)


#define SET_HDMI_HD21_720_480i()                (bHD21_720_480i = _TRUE)
#define CLR_HDMI_HD21_720_480i()                (bHD21_720_480i = _FALSE)


#define SET_HDMI_HD20_needRetiming()           (bHD20_needRetiming = _TRUE)
#define CLR_HDMI_HD20_needRetiming()           (bHD20_needRetiming = _FALSE)


#define SET_HDMI_HD21_needUnvalid()           (bHD21_needUnvalid = _TRUE)
#define CLR_HDMI_HD21_needUnvalid()           (bHD21_needUnvalid = _FALSE)
//--------------------------------------------------
// Description  : HDMI Tx0 Handler
// Input Value  : None
// Output Value : None
//--------------------------------------------------
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
//extern TX_TIMING_INDEX timeIdx;
static   hdmi_tx_timing_gen_st * p_tx_cea_timing_repeater_table;

 UINT8 CDF_no_support_timing_list[] = {8, 9, 14, 15, 23, 24, 48, 49, 114, 116};

// {vic, {blank, active, sync, front, back}, {space, blank, act_video, active, sync, front, back}, vFreq, pixelFreq}, HxVxFR
 hdmi_tx_timing_gen_st  cea_timing_table_repeater[TX_TIMING_NUM] = {
    {0, 0, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, 0, 0},//default
    {1, 0, {160, 640, 96, 16, 48}, {1, 45, 480, 480, 2, 10, 33}, 600, 251}, //HDMI_TX_640_480P_60HZ
    {2, 3, {138, 720, 62, 16, 60}, {1, 45, 480, 480, 6, 9, 30}, 600, 270},  //HDMI_TX_720_480P_60HZ
    {48, 49, {138, 720, 62, 16, 60}, {1, 45, 480, 480, 6, 9, 30}, 1200, 540},  //HDMI_TX_720_480P_120HZ
    {56, 57, {138, 720, 62, 16, 60}, {1, 45, 480, 480, 6, 9, 30}, 2400, 1080},  //HDMI_TX_720_480P_240HZ
    {17, 18, {144, 720, 64, 12, 68}, {1, 49, 576, 576, 5, 5, 39}, 500, 270}, //HDMI_TX_720_576P_50HZ
    {42, 43, {144, 720, 64, 12, 68}, {1, 49, 576, 576, 5, 5, 39}, 1000, 540}, //HDMI_TX_720_576P_100HZ
    {52, 53, {144, 720, 64, 12, 68}, {1, 49, 576, 576, 5, 5, 39}, 2000, 1080}, //HDMI_TX_720_576P_200HZ
    {23, 24, {288, 1440, 126, 24, 138}, {1, 24, 288, 288, 3, 2, 19}, 500, 270}, // HDMI_TX_1440_288P_50HZ
    {8, 9, {276, 1440, 124, 38, 114}, {1, 22, 240, 240, 3, 4, 15}, 600, 270}, // HDMI_TX_1440_240P_60HZ
    {27, 28, {576, 2880, 252, 48, 276}, {1, 24, 288, 288, 3, 2, 19}, 500, 540}, // HDMI_TX_2880_288P_50HZ
    {29, 30, {288, 1440, 128, 24, 136}, {1, 49, 576, 576, 5, 5, 39}, 500, 540}, //HDMI_TX_1440_576P_50HZ
    {37, 38, {576, 2880, 256, 48, 272}, {1, 49, 576, 576, 5, 5, 39}, 500, 1080}, //HDMI_TX_2880_576P_50HZ
    {12, 13, {552, 2880, 248, 76, 228}, {1, 22, 240, 240, 3, 4, 15}, 600, 540}, // HDMI_TX_2880_240P_60HZ
    {14, 15, {276, 1440, 124, 32, 120}, {1, 45, 480, 480, 6, 9, 30}, 600, 540},   // HDMI_TX_1440_480P_60HZ
    {35, 36, {552, 2880, 248, 64, 240}, {1, 45, 480, 480, 6, 9, 30}, 600, 1080}, // HDMI_TX_2880_480P_60HZ
    {60, 65, {2020, 1280, 40, 1760, 220}, {1, 30, 720, 720, 5, 5, 20}, 240, 594},  //HDMI_TX_1280_720P_24HZ
    {61, 66, {2680, 1280, 40, 2420, 220}, {1, 30, 720, 720, 5, 5, 20}, 250, 742},  //HDMI_TX_1280_720P_25HZ
    {62, 67, {2020, 1280, 40, 1760, 220}, {1, 30, 720, 720, 5, 5, 20}, 300, 742},  //HDMI_TX_1280_720P_30HZ
    {108, 109, {1220, 1280, 40, 960, 220}, {1, 30, 720, 720, 5, 5, 20}, 480, 900},  //HDMI_TX_1280_720P_48HZ
    {19, 68, {700, 1280, 40, 440, 220}, {1, 30, 720, 720, 5, 5, 20}, 500, 742},  //HDMI_TX_1280_720P_50HZ
    {4, 69, {370, 1280, 40, 110, 220}, {1, 30, 720, 720, 5, 5, 20}, 600, 742},   //HDMI_TX_1280_720P_60HZ
    {41, 70, {700, 1280, 40, 440, 220}, {1, 30, 720, 720, 5, 5, 20}, 500, 1485},  //HDMI_TX_1280_720P_100HZ
    {47, 71, {370, 1280, 40, 110, 220}, {1, 30, 720, 720, 5, 5, 20}, 1200, 1485},   //HDMI_TX_1280_720P_120HZ
    {32, 72, {830, 1920, 44, 638, 148}, {1, 45, 1080, 1080, 5, 4, 36}, 240, 742},    //HDMI_TX_1920_1080P_24HZ
    {33, 73, {720, 1920, 44, 528, 148}, {1, 45, 1080, 1080, 5, 4, 36}, 250, 742},    //HDMI_TX_1920_1080P_25HZ
    {34, 74, {280, 1920, 44, 88, 148}, {1, 45, 1080, 1080, 5, 4, 36}, 300, 742},    //HDMI_TX_1920_1080P_30HZ
    {111, 112, {830, 1920, 44, 638, 148}, {1, 45, 1080, 1080, 5, 4, 36}, 480, 1485},   //HDMI_TX_1920_1080P_48HZ
    {31, 75, {720, 1920, 44, 528, 148}, {1, 45, 1080, 1080, 5, 4, 36}, 500, 1485},   //HDMI_TX_1920_1080P_50HZ
    {16, 76, {280, 1920, 44, 88, 148}, {1, 45, 1080, 1080, 5, 4, 36}, 600, 1485},    //HDMI_TX_1920_1080P_60HZ
    {64, 77, {720, 1920, 44, 528, 148}, {1, 45, 1080, 1080, 5, 4, 36}, 1000, 2970},   //HDMI_TX_1920_1080P_100HZ
    {63, 78, {280, 1920, 44, 88, 148}, {1, 45, 1080, 1080, 5, 4, 36}, 1200, 2970},    //HDMI_TX_1920_1080P_120HZ
    {0, 0, {160, 1920, 32, 48, 80}, {1, 77, 1080, 1080, 5, 3, 69}, 1440, 3465},    //HDMI_TX_1920_1080P_144HZ_CVT_RB
    {0, 0, {784, 1920, 216, 176, 392}, {1, 168, 1080, 1080, 5, 3, 160}, 2400, 8095},   //HDMI_TX_1920_1080P_240HZ_CVT
    {0, 0, {160, 1920, 32, 48, 80}, {1, 135, 1080, 1080, 5, 3, 127}, 2400, 6065},   //HDMI_TX_1920_1080P_240HZ_CVT_RB
    {79, 0, {1620, 1680, 40, 1360, 220}, {1, 30, 720, 720, 5, 5, 20}, 240, 594},   //HDMI_TX_1680_720P_24HZ
    {80, 0, {1488, 1680, 40, 1228, 220}, {1, 30, 720, 720, 5, 5, 20}, 250, 594},   //HDMI_TX_1680_720P_25HZ
    {81, 0, {960, 1680, 40, 700, 220}, {1, 30, 720, 720, 5, 5, 20}, 300, 594},   //HDMI_TX_1680_720P_30HZ
    {110, 0, {1070, 1680, 40, 810, 220}, {1, 30, 720, 720, 5, 5, 20}, 480, 990},   //HDMI_TX_1680_720P_48HZ
    {82, 0, {520, 1680, 40, 260, 220}, {1, 30, 720, 720, 5, 5, 20}, 500, 825},   //HDMI_TX_1680_720P_50HZ
    {83, 0, {520, 1680, 40, 260, 220}, {1, 30, 720, 720, 5, 5, 20}, 600, 990},   //HDMI_TX_1680_720P_60HZ
    {84, 0, {320, 1680, 40, 60, 220}, {1, 105, 720, 720, 5, 5, 95}, 1000, 1650},   //HDMI_TX_1680_720P_100HZ
    {85, 0, {320, 1680, 40, 60, 220}, {1, 105, 720, 720, 5, 5, 95}, 1200, 1980},   //HDMI_TX_1680_720P_120HZ
    {86, 0, {1190, 2560, 44, 998, 148}, {1, 20, 1080, 1080, 5, 4, 11}, 240, 990},    //HDMI_TX_2560_1080P_24HZ
    {87, 0, {640, 2560, 44, 448, 148}, {1, 45, 1080, 1080, 5, 4, 36}, 250, 900},    //HDMI_TX_2560_1080P_25HZ
    {88, 0, {960, 2560, 44, 768, 148}, {1, 45, 1080, 1080, 5, 4, 36}, 300, 1188},    //HDMI_TX_2560_1080P_30HZ
    {113, 0, {1190, 2560, 44, 998, 148}, {1, 20, 1080, 1080, 5, 4, 11}, 480, 1980},    //HDMI_TX_2560_1080P_48HZ
    {89, 0, {740, 2560, 44, 548, 148}, {1, 45, 1080, 1080, 5, 4, 36}, 500, 1856},    //HDMI_TX_2560_1080P_50HZ
    {90, 0, {440, 2560, 44, 248, 148}, {1, 20, 1080, 1080, 5, 4, 11}, 600, 1980},    //HDMI_TX_2560_1080P_60HZ
    {91, 0, {410, 2560, 44, 218, 148}, {1, 170, 1080, 1080, 5, 4, 161}, 1000, 3712},    //HDMI_TX_2560_1080P_100HZ
    {92, 0, {740, 2560, 44, 548, 148}, {1, 170, 1080, 1080, 5, 4, 161}, 1200, 4950},    //HDMI_TX_2560_1080P_120HZ
    {0, 0, {160, 2560, 32, 48, 80}, {1, 77, 1080, 1080, 10, 3, 64}, 1440, 4530},    //HDMI_TX_2560_1080P_144HZ_CVT_RB
    {0, 0, {160, 2560, 32, 48, 80}, {1, 41, 1440, 1440, 5, 3, 33}, 600, 2415},    //HDMI_TX_2560_1440P_60HZ_CVT_RB
    {0, 0, {928, 2560, 272, 192, 464}, {1, 53, 1440, 1440, 5, 3, 45}, 600, 3122},    //HDMI_TX_2560_1440P_60HZ   
    {0, 0, {160, 2560, 32, 48, 80}, {1, 85, 1440, 1440, 5, 3, 77}, 1200, 4977},    //HDMI_TX_2560_1440P_120HZ CTV_RB
    {0, 0, {1008, 2560, 280, 224, 504}, {1, 105, 1440, 1440, 5, 3, 97}, 1200, 6612},    //HDMI21_TX_2560_1440P_120HZ CVT
    {0, 0, {1024, 2560, 280, 232, 512}, {1, 128, 1440, 1440, 5, 3, 120}, 1440, 8087},    //HDMI_TX_2560_1440P_144HZ CVT_CAL
    {0, 0, {106, 2560, 32, 18, 56}, {1, 103, 1440, 1440, 5, 3, 95}, 1440, 5924},    //HDMI_TX_2560_1440P_144HZ tmds 592M
    {0, 0, {80, 2560, 32, 8, 40}, {1, 120, 1440, 1440, 8, 89, 23}, 1440, 5930},    //HDMI_TX_2560_1440P_144HZ_RBV2
    {93, 103, {1660, 3840, 88, 1276, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 240, 2970},  //HDMI_TX_3840_2160P_24HZ
    {94, 104, {1440, 3840, 88, 1056, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 250, 2970},//HDMI_TX_3840_2160P_25HZ
    {95, 105, {560, 3840, 88, 176, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 300, 2970},  //HDMI_TX_3840_2160P_30HZ
    {114, 116, {1660, 3840, 88, 1276, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 480, 5940},  //HDMI_TX_3840_2160P_48HZ
    {96, 106, {1440, 3840, 88, 1056, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 500, 5940},    //HDMI_TX_3840_2160P_50HZ
    {97, 107, {560, 3840, 88, 176, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 600, 5940},  //HDMI_TX_3840_2160P_60HZ
    {117, 119, {1440, 3840, 88, 1056, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 1000, 11880},    //HDMI_TX_3840_2160P_100HZ
    {118, 120, {560, 3840, 88, 176, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 1200, 11880},//HDMI_TX_3840_2160P_120HZ,
    {0, 0, {160, 3840, 32, 48, 80}, {1, 154, 2160, 2160, 5, 3, 146}, 1440, 13327},          //HDMI_TX_3840_2160P_144HZ_CVT_RB,
    {0, 0, {80, 3840, 32, 8, 40}, {1, 154, 2160, 2160, 8, 140, 6}, 1440, 13062},          //HDMI_TX_3840_2160P_144HZ_CVT_RBv2,
    {0, 0, {560, 3840, 88, 176, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 1440, 14254},          //HDMI_TX_3840_2160P_144HZ_VUP,
    {0, 0, {1568, 3840, 432, 352, 784}, {1, 190, 2160, 2160, 5, 3, 182}, 1440, 18292},          //HDMI_TX_3840_2160P_144HZ_CVT,
     // 4k60/120 cvt rbv1 rbv2
    {97, 107, {1472, 3840, 424, 312, 736}, {1, 77, 2160, 2160, 5, 3, 69}, 600, 7127},          //HDMI_TX_3840_2160P_60HZ_CVT,
    {97, 107, {160, 3840, 32, 48, 80}, {1, 62, 2160, 2160, 5, 3, 54}, 600, 5332},          //HDMI_TX_3840_2160P_60HZ_CVT_RB,
    {97, 107, {80, 3840, 32, 8, 40}, {1, 62, 2160, 2160, 8, 48, 6}, 600, 5226},          //HDMI_TX_3840_2160P_60HZ_CVT_RBv2,
    {118, 120, {1552, 3840, 424, 352, 776}, {1, 156, 2160, 2160, 5, 3, 148}, 1200, 14982},          //HDMI_TX_3840_2160P_120HZ_CVT,
    {118, 120, {160, 3840, 32, 48, 80}, {1, 127, 2160, 2160, 5, 3, 119}, 1200, 10977},          //HDMI_TX_3840_2160P_120HZ_CVT_RB,
    {118, 120, {80, 3840, 32, 8, 40}, {1, 127, 2160, 2160, 8, 113, 6}, 1200, 10758},          //HDMI_TX_3840_2160P_120HZ_CVT_RBv2,

    // 2.5k240 2k360 rbv1 rbv2 861.6
    {0, 0, {160, 2560, 32, 48, 80}, {1, 179, 1440, 1440, 5, 3, 171}, 2400, 10567},          //HDMI_TX_2560_1440P_240HZ_CVT_RB,
    {0, 0, {80, 2560, 32, 8, 40}, {1, 179, 1440, 1440, 8, 165, 6}, 2400, 10257},          //HDMI_TX_2560_1440P_240HZ_CVT_RBv2,
    {0, 0, {128, 2560, 32, 64, 32}, {1, 185, 1440, 1440, 8, 29, 148}, 2400, 10483},          //HDMI_TX_2560_1440P_240HZ_861_OVT,
    {0, 0, {160, 1920, 32, 48, 80}, {1, 215, 1080, 1080, 5, 3, 207}, 3600, 9695},          //HDMI_TX_1920_1080P_360HZ_CVT_RB,
    {0, 0, {80, 1920, 32, 8, 40}, {1, 215, 1080, 1080, 8, 201, 6}, 3600, 9324},          //HDMI_TX_1920_1080P_360HZ_CVT_RBv2,
    {0, 0, {128, 1920, 32, 64, 32}, {1, 220, 1080, 1080, 8, 32, 180}, 3600, 9584},          //HDMI_TX_1920_1080P_360HZ_861_OVT,

    {0, 0, {160, 1280, 32, 48, 80}, {1, 52, 720, 720, 5, 3, 44}, 1440, 1600},   //HDMI_TX_1280_720P_144HZ_CVT_RB
    {0, 0, {80, 1280, 32, 8, 40}, {1, 52, 720, 720, 8, 38, 6}, 1440, 1511},   //HDMI_TX_1280_720P_144HZ_CVT_RBv2
    {0, 0, {160, 2560, 32, 48, 80}, {1, 119, 1440, 1440, 5, 3, 111}, 1650, 6995},          //HDMI_TX_2560_1440P_165HZ_CVT_RB,
    {0, 0, {80, 2560, 32, 8, 40}, {1, 119, 1440, 1440, 8, 105, 6}, 1650, 6791},          //HDMI_TX_2560_1440P_165HZ_CVT_RBv2,
    {0, 0, {160, 2560, 32, 48, 80}, {1, 123, 1440, 1440, 5, 3, 115}, 1700, 7225},          //HDMI_TX_2560_1440P_170HZ_CVT_RB,
    {0, 0, {80, 2560, 32, 8, 40}, {1, 123, 1440, 1440, 8, 109, 6}, 1700, 7014},          //HDMI_TX_2560_1440P_170HZ_CVT_RBv2,
    {0, 0, {160, 2560, 32, 48, 80}, {1, 130, 1440, 1440, 5, 3, 122}, 1800, 7685},          //HDMI_TX_2560_1440P_180HZ_CVT_RB,
    {0, 0, {80, 2560, 32, 8, 40}, {1, 130, 1440, 1440, 8, 116, 6}, 1800, 7460},          //HDMI_TX_2560_1440P_180HZ_CVT_RBv2,
    {0, 0, {160, 3440, 32, 48, 80}, {1, 41, 1440, 1440, 10, 3, 28}, 600, 3197},   //HDMI_TX_3440_1440P_60HZ_CVT_RB,
    {0, 0, {80, 3440, 32, 8, 40}, {1, 41, 1440, 1440, 8, 27, 6}, 600, 3127},   //HDMI_TX_3440_1440P_60HZ_CVT_RBv2,
    {0, 0, {160, 3440, 32, 48, 80}, {1, 85, 1440, 1440, 10, 3, 72}, 1200, 6587},   //HDMI_TX_3440_1440P_120HZ_CVT_RB,
    {0, 0, {80, 3440, 32, 8, 40}, {1, 85, 1440, 1440, 8, 71, 6}, 1200, 6441},   //HDMI_TX_3440_1440P_120HZ_CVT_RBv2,
    {0, 0, {160, 3440, 32, 48, 80}, {1, 103, 1440, 1440, 10, 3, 90}, 1440, 7997},   //HDMI_TX_3440_1440P_144HZ_CVT_RB,
    {0, 0, {80, 3440, 32, 8, 40}, {1, 103, 1440, 1440, 8, 89, 6}, 1440, 7821},   //HDMI_TX_3440_1440P_144HZ_CVT_RBv2,
    {0, 0, {160, 3440, 32, 48, 80}, {1, 119, 1440, 1440, 10, 3, 106}, 1650, 9260},   //HDMI_TX_3440_1440P_165HZ_CVT_RB,
    {0, 0, {80, 3440, 32, 8, 40}, {1, 119, 1440, 1440, 8, 105, 6}, 1650, 9054},   //HDMI_TX_3440_1440P_165HZ_CVT_RBv2,
    {0, 0, {160, 3440, 32, 48, 80}, {1, 123, 1440, 1440, 10, 3, 110}, 1700, 9565},   //HDMI_TX_3440_1440P_170HZ_CVT_RB,
    {0, 0, {80, 3440, 32, 8, 40}, {1, 123, 1440, 1440, 8, 109, 6}, 1700, 9352},   //HDMI_TX_3440_1440P_170HZ_CVT_RBv2,
    {0, 0, {160, 3440, 32, 48, 80}, {1, 130, 1440, 1440, 10, 3, 117}, 1800, 10172},   //HDMI_TX_3440_1440P_180HZ_CVT_RB,
    {0, 0, {80, 3440, 32, 8, 40}, {1, 130, 1440, 1440, 8, 116, 6}, 1800, 9947},   //HDMI_TX_3440_1440P_180HZ_CVT_RBv2,
    {0, 0, {160, 3440, 32, 48, 80}, {1, 179, 1440, 1440, 10, 3, 166}, 2400, 13987},   //HDMI_TX_3440_1440P_240HZ_CVT_RB,
    {0, 0, {80, 3440, 32, 8, 40}, {1, 179, 1440, 1440, 8, 165, 6}, 2400, 13677},   //HDMI_TX_3440_1440P_240HZ_CVT_RBv2,
    //interlace
    #if 0
    {50, 51, {276, 1440, 124, 38, 114}, {1, 22, 240, 240, 3, 4, 15}, 1200, 540},             // HDMI_TX_1440_480I_120HZ
    {58, 59, {276, 1440, 124, 38, 114}, {1, 22, 240, 240, 3, 4, 15}, 2400, 1080},             // HDMI_TX_1440_480I_240HZ
    {44, 45, {288, 1440, 126, 24, 138}, {1, 24, 288, 288, 3, 2, 19}, 1000, 540},             // HDMI_TX_1440_576I_100HZ
    {54, 55, {288, 1440, 126, 24, 138}, {1, 24, 288, 288, 3, 2, 19}, 2000, 1080},             // HDMI_TX_1440_576I_200HZ
    {25, 26, {576, 2880, 252, 48, 276}, {1, 24, 288, 288, 3, 2, 19}, 500, 540},             // HDMI_TX_2880_576I_50HZ
    {10, 11, {552, 2880, 248, 76, 228}, {1, 22, 240, 240, 3, 4, 15}, 600, 540},             // HDMI_TX_2880_480I_60HZ
    #endif
    {6, 7, {276, 1440, 124, 38, 114}, {1, 22, 240, 240, 3, 4, 15}, 600, 270},             // HDMI_TX_1440_480I_60HZ
    {21, 22, {288, 1440, 126, 24, 138}, {1, 24, 288, 288, 3, 2, 19}, 500, 270},             // HDMI_TX_1440_576I_50HZ
    {20, 0, {720, 1920, 44, 528, 148}, {1, 22, 540, 540, 5, 2, 15}, 500, 742},             // HDMI_TX_1920_1080I_50HZ
    {5, 0, {280, 1920, 44, 88, 148}, {1, 22, 540, 540, 5, 2, 15}, 600, 742},             // HDMI_TX_1920_1080I_60HZ
    {40, 0, {720, 1920, 44, 528, 148}, {1, 22, 540, 540, 5, 2, 15}, 1000, 1485},             // HDMI_TX_1920_1080I_100HZ
    {46, 0, {280, 1920, 44, 88, 148}, {1, 22, 540, 540, 5, 2, 15}, 1200, 1485},             // HDMI_TX_1920_1080I_120HZ
    //DMT, CVT, CVT RB V1/V2
    {0, 0, {160, 1280, 32, 48, 80}, {1, 22, 768, 768, 7, 3, 12}, 600, 682},   //HDMI_TX_1280_768P_60HZ_CVT_RB
    {0, 0, {384, 1280, 128, 64, 192}, {1, 30, 768, 768, 7, 3, 20}, 600, 795},   //HDMI_TX_1280_768P_60HZ_DMT
    {0, 0, {160, 1280, 32, 48, 80}, {1, 23, 800, 800, 6, 3, 14}, 600, 710},   //HDMI_TX_1280_800P_60HZ_CVT_RB
    {0, 0, {400, 1280, 128, 72, 200}, {1, 31, 800, 800, 6, 3, 22}, 600, 835},   //HDMI_TX_1280_800P_60HZ_DMT
    {0, 0, {520, 1280, 112, 96, 312}, {1, 40, 960, 960, 3, 1, 36}, 600, 1080},   //HDMI_TX_1280_960P_60HZ_DMT
    {0, 0, {408, 1280, 112, 48, 248}, {1, 42, 1024, 1024, 3, 1, 38}, 600, 1080},   //HDMI_TX_1280_1024P_60HZ_DMT
    {0, 0, {432, 1360, 112, 64, 256}, {1, 27, 768, 768, 6, 3, 18}, 600, 855},   //HDMI_TX_1360_768P_60HZ_DMT
    {0, 0, {134, 1366, 56, 14, 64}, {1, 32, 768, 768, 3, 1, 28}, 600, 720},   //HDMI_TX_1366_768P_60HZ_CVT_RB
    {0, 0, {426, 1366, 143, 70, 213}, {1, 30, 768, 768, 3, 3, 24}, 600, 855},   //HDMI_TX_1366_768P_60HZ_DMT
    {0, 0, {160, 1400, 32, 48, 80}, {1, 30, 1050, 1050, 4, 3, 23}, 600, 1010},   //HDMI_TX_1400_1050P_60HZ_CVT_RB
    {0, 0, {464, 1400, 144, 88, 232}, {1, 39, 1050, 1050, 4, 3, 32}, 600, 1217},   //HDMI_TX_1400_1050P_60HZ_DMT
    {0, 0, {160, 1440, 32, 48, 80}, {1, 26, 900, 900, 6, 3, 17}, 600, 887},   //HDMI_TX_1440_900P_60HZ_CVT_RB
    {0, 0, {464, 1440, 152, 80, 232}, {1, 34, 900, 900, 6, 3, 25}, 600, 1065},   //HDMI_TX_1440_900P_60HZ_DMT
    {0, 0, {200, 1600, 80, 24, 96}, {1, 100, 900, 900, 3, 1, 96}, 600, 1080},   //HDMI_TX_1600_900P_60HZ_CVT_RB
    {0, 0, {560, 1600, 192, 64, 304}, {1, 50, 1200, 1200, 3, 1, 46}, 600, 1620},   //HDMI_TX_1600_1200P_60HZ_DMT
    {0, 0, {160, 1680, 32, 48, 80}, {1, 30, 1050, 1050, 6, 3, 21}, 600, 1190},   //HDMI_TX_1680_1050P_60HZ_CVT_RB
    {0, 0, {560, 1680, 176, 104, 280}, {1, 39, 1050, 1050, 6, 3, 30}, 600, 1462},   //HDMI_TX_1680_1050P_60HZ_DMT
    {0, 0, {656, 1792, 200, 128, 328}, {1, 50, 1344, 1344, 3, 1, 46}, 600, 2047},   //HDMI_TX_1792_1344P_60HZ_DMT
    {0, 0, {672, 1856, 224, 96, 352}, {1, 47, 1392, 1392, 3, 1, 43}, 600, 2182},   //HDMI_TX_1856_1392P_60HZ_DMT
    {0, 0, {160, 1920, 32, 48, 80}, {1, 35, 1200, 1200, 6, 3, 26}, 600, 1540},   //HDMI_TX_1920_1200P_60HZ_CVT_RB
    {0, 0, {672, 1920, 200, 136, 336}, {1, 45, 1200, 1200, 6, 3, 36}, 600, 1932},   //HDMI_TX_1920_1200P_60HZ_DMT
    {0, 0, {680, 1920, 208, 128, 344}, {1, 60, 1440, 1440, 3, 1, 56}, 600, 2340},   //HDMI_TX_1920_1440P_60HZ_DMT
    {0, 0, {202, 2048, 80, 26, 96}, {1, 48, 1152, 1152, 3, 1, 44}, 600, 1620},   //HDMI_TX_2048_1152P_60HZ_CVT_RB
    {0, 0, {160, 2560, 32, 48, 80}, {1, 46, 1600, 1600, 6, 3, 37}, 600, 2685},   //HDMI_TX_2560_1600P_60HZ_CVT_RB
    {0, 0, {944, 2560, 280, 192, 472}, {1, 58, 1600, 1600, 6, 3, 49}, 600, 4385},   //HDMI_TX_2560_1600P_60HZ_DMT
    {0, 0, {1248, 3440, 368, 256, 624}, {1, 53, 1440, 1440, 10, 3, 40}, 600, 4195},   //HDMI_TX_3440_1440P_60HZ_CVT
    #if 0
    {0, 0, {560, 1600, 192, 64, 304}, {1, 50, 1200, 1200, 3, 1, 46}, 650, 1755},   //HDMI_TX_1600_1200P_65HZ_DMT
    {0, 0, {560, 1600, 192, 64, 304}, {1, 50, 1200, 1200, 3, 1, 46}, 700, 1890},   //HDMI_TX_1600_1200P_70HZ_DMT
    {0, 0, {416, 1280, 128, 80, 208}, {1, 37, 768, 768, 7, 3, 27}, 750, 1022},   //HDMI_TX_1280_768P_75HZ_DMT
    {0, 0, {416, 1280, 128, 80, 208}, {1, 38, 800, 800, 6, 3, 29}, 750, 1065},   //HDMI_TX_1280_800P_75HZ_DMT
    {0, 0, {408, 1280, 144, 16, 248}, {1, 42, 1024, 1024, 3, 1, 38}, 750, 1350},   //HDMI_TX_1280_1024P_75HZ_DMT
    {0, 0, {496, 1400, 144, 104, 248}, {1, 49, 1050, 1050, 4, 3, 42}, 750, 1560},   //HDMI_TX_1400_1050P_75HZ_DMT
    {0, 0, {496, 1440, 152, 96, 248}, {1, 42, 900, 900, 6, 3, 33}, 750, 1367},   //HDMI_TX_1440_900P_75HZ_DMT
    {0, 0, {560, 1600, 192, 64, 304}, {1, 50, 1200, 1200, 3, 1, 46}, 750, 2025},   //HDMI_TX_1600_1200P_75HZ_DMT
    {0, 0, {592, 1680, 176, 120, 296}, {1, 49, 1050, 1050, 6, 3, 40}, 750, 1870},   //HDMI_TX_1680_1050P_75HZ_DMT
    {0, 0, {664, 1792, 216, 96, 352}, {1, 73, 1344, 1344, 3, 1, 69}, 750, 2610},   //HDMI_TX_1792_1344P_75HZ_DMT
    {0, 0, {704, 1856, 224, 128, 352}, {1, 108, 1392, 1392, 3, 1, 104}, 750, 2880},   //HDMI_TX_1856_1392P_75HZ_DMT
    {0, 0, {688, 1920, 208, 136, 344}, {1, 55, 1200, 1200, 6, 3, 46}, 750, 2452},   //HDMI_TX_1920_1200P_75HZ_DMT
    {0, 0, {720, 1920, 224, 144, 352}, {1, 60, 1440, 1440, 3, 1, 56}, 750, 2970},   //HDMI_TX_1920_1440P_75HZ_DMT
    {0, 0, {976, 2560, 280, 208, 488}, {1, 72, 1600, 1600, 6, 3, 63}, 750, 4432},   //HDMI_TX_2560_1600P_75HZ_DMT
    {0, 0, {432, 1280, 136, 80, 216}, {1, 41, 768, 768, 7, 3, 31}, 850, 1175},   //HDMI_TX_1280_768P_85HZ_DMT
    {0, 0, {432, 1280, 136, 80, 216}, {1, 43, 800, 800, 6, 3, 34}, 850, 1225},   //HDMI_TX_1280_800P_85HZ_DMT
    {0, 0, {448, 1280, 160, 64, 224}, {1, 51, 960, 960, 3, 1, 47}, 850, 1485},   //HDMI_TX_1280_960P_85HZ_DMT
    {0, 0, {448, 1280, 160, 64, 224}, {1, 48, 1024, 1024, 3, 1, 44}, 850, 1575},   //HDMI_TX_1280_1024P_85HZ_DMT
    {0, 0, {512, 1400, 152, 104, 256}, {1, 55, 1050, 1050, 4, 3, 48}, 850, 1795},   //HDMI_TX_1400_1050P_85HZ_DMT
    {0, 0, {512, 1400, 152, 104, 256}, {1, 48, 900, 900, 6, 3, 39}, 850, 1570},   //HDMI_TX_1440_900P_85HZ_DMT
    {0, 0, {560, 1600, 192, 64, 304}, {1, 50, 1200, 1200, 3, 1, 46}, 850, 2295},   //HDMI_TX_1600_1200P_85HZ_DMT
    {0, 0, {608, 1680, 176, 128, 304}, {1, 55, 1050, 1050, 6, 3, 46}, 850, 2147},   //HDMI_TX_1680_1050P_85HZ_DMT
    {0, 0, {704, 1920, 208, 144, 352}, {1, 62, 1200, 1200, 6, 3, 53}, 850, 2812},   //HDMI_TX_1920_1200P_85HZ_DMT
    {0, 0, {976, 2560, 280, 208, 488}, {1, 82, 1600, 1600, 6, 3, 73}, 850, 5052},   //HDMI_TX_2560_1600P_85HZ_DMT
    #endif
    {0, 0, {160, 1280, 32, 48, 80}, {1, 45, 768, 768, 7, 3, 35}, 1200, 1402},   //HDMI_TX_1280_768P_120HZ_CVT_RB
    {0, 0, {160, 1280, 32, 48, 80}, {1, 47, 800, 800, 6, 3, 38}, 1200, 1462},   //HDMI_TX_1280_800P_120HZ_CVT_RB
    {0, 0, {160, 1280, 32, 48, 80}, {1, 57, 960, 960, 4, 3, 50}, 1200, 1755},   //HDMI_TX_1280_960P_120HZ_CVT_RB
    {0, 0, {160, 1280, 32, 48, 80}, {1, 60, 1024, 1024, 7, 3, 50}, 1200, 1872},   //HDMI_TX_1280_1024P_120HZ_CVT_RB
    {0, 0, {160, 1400, 32, 48, 80}, {1, 62, 1050, 1050, 4, 3, 55}, 1200, 2080},   //HDMI_TX_1400_1050P_120HZ_CVT_RB
    {0, 0, {160, 1440, 32, 48, 80}, {1, 53, 900, 900, 6, 3, 44}, 1200, 1827},   //HDMI_TX_1440_900P_120HZ_CVT_RB
    {0, 0, {160, 1600, 32, 48, 80}, {1, 71, 1200, 1200, 4, 3, 64}, 1200, 2682},   //HDMI_TX_1600_1200P_120HZ_CVT_RB
    {0, 0, {160, 1680, 32, 48, 80}, {1, 62, 1050, 1050, 6, 3, 53}, 1200, 2455},   //HDMI_TX_1680_1050P_120HZ_CVT_RB
    {0, 0, {160, 1792, 32, 48, 80}, {1, 79, 1344, 1344, 4, 3, 72}, 1200, 3332},   //HDMI_TX_1792_1344P_120HZ_CVT_RB
    {0, 0, {160, 1856, 32, 48, 80}, {1, 82, 1392, 1392, 4, 3, 75}, 1200, 3565},   //HDMI_TX_1856_1392P_120HZ_CVT_RB
    {0, 0, {160, 1920, 32, 48, 80}, {1, 71, 1200, 1200, 6, 3, 62}, 1200, 3170},   //HDMI_TX_1920_1200P_120HZ_CVT_RB
    {0, 0, {160, 1920, 32, 48, 80}, {1, 85, 1440, 1440, 4, 3, 78}, 1200, 3805},   //HDMI_TX_1920_1440P_120HZ_CVT_RB
    {0, 0, {160, 2560, 32, 48, 80}, {1, 94, 1600, 1600, 6, 3, 85}, 1200, 5527},   //HDMI_TX_2560_1600P_120HZ_CVT_RB
    {0, 0, {1360, 3440, 384, 296, 680}, {1, 105, 1440, 1440, 10, 3, 92}, 1200, 8897},   //HDMI_TX_3440_1440P_120HZ_CVT
    {0, 0, {1024, 2560, 280, 232, 512}, {1, 141, 1600, 1600, 6, 3, 132}, 1440, 8982},   //HDMI_TX_2560_1600P_144HZ_DMT_CAL
    {0, 0, {224, 3840, 32, 160, 32}, {1, 295, 1080, 1080, 8, 281, 6}, 240, 1341},  //HDMI_TX_3840_1080P_24HZ
    {0, 0, {224, 3840, 32, 160, 32}, {1, 240, 1080, 1080, 8, 226, 6}, 250, 1341},  //HDMI_TX_3840_1080P_25HZ
    {0, 0, {224, 3840, 32, 160, 32}, {1, 20, 1080, 1080, 8, 6, 6}, 300, 1341},  //HDMI_TX_3840_1080P_30HZ
    {0, 0, {160, 3840, 32, 96, 32}, {1, 320, 1080, 1080, 8, 293, 19}, 480, 2688},  //HDMI_TX_3840_1080P_48HZ
    {0, 0, {160, 3840, 32, 96, 32}, {1, 264, 1080, 1080, 8, 237, 19}, 500, 2688},  //HDMI_TX_3840_1080P_50HZ
    {0, 0, {160, 3840, 32, 96, 32}, {1, 40, 1080, 1080, 8, 13, 19}, 600, 2688},  //HDMI_TX_3840_1080P_60HZ
    {0, 0, {96, 3840, 32, 32, 32}, {1, 300, 1080, 1080, 8, 244, 48}, 1000, 5431},  //HDMI_TX_3840_1080P_100HZ
    {0, 0, {96, 3840, 32, 32, 32}, {1, 70, 1080, 1080, 8, 14, 48}, 1200, 5431},  //HDMI_TX_3840_1080P_120HZ
    {0, 0, {200, 3840, 32, 136, 32}, {1, 95, 1080, 1080, 8, 27, 60}, 1440, 6835},  //HDMI_TX_3840_1080P_144HZ
    {0, 0, {160, 3840, 32, 96, 32}, {1, 378, 1080, 1080, 8, 261, 109}, 2000, 11664},  //HDMI_TX_3840_1080P_200HZ
    {0, 0, {160, 3840, 32, 96, 32}, {1, 135, 1080, 1080, 8, 18, 109}, 2400, 11664},  //HDMI_TX_3840_1080P_240HZ
    {0, 0, {1328, 3440, 376, 288, 664}, {1, 87, 1440, 1440, 10, 3, 74}, 1000, 7280},   //HDMI_TX_3440_1440P_100HZ_CVT

    {98, 0, {1404, 4096, 88, 1020, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 240, 2970},  //HDMI_TX_4096_2160P_24HZ
    {99, 0, {1184, 4096, 88, 968, 128}, {1, 90, 2160, 2160, 10, 8, 72}, 250, 2970},//HDMI_TX_4096_2160P_25HZ
    {100, 0, {304, 4096, 88, 88, 128}, {1, 90, 2160, 2160, 10, 8, 72}, 300, 2970},  //HDMI_TX_4096_2160P_30HZ
    {115, 0, {1404, 4096, 88, 1020, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 480, 5940},  //HDMI_TX_4096_2160P_48HZ
    {101, 0, {1184, 4096, 88, 968, 128}, {1, 90, 2160, 2160, 10, 8, 72}, 500, 5940},    //HDMI_TX_4096_2160P_50HZ
    {102, 0, {304, 4096, 88, 88, 128}, {1, 90, 2160, 2160, 10, 8, 72}, 600, 5940},  //HDMI_TX_4096_2160P_60HZ
    {218, 0, {1184, 4096, 88, 800, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 1000, 11880},    //HDMI_TX_4096_2160P_100HZ
    {219, 0, {304, 4096, 88, 88, 128}, {1, 90, 2160, 2160, 10, 8, 72}, 1200, 11880},//HDMI_TX_4096_2160P_120HZ,
    {0, 0, {160, 4096, 32, 48, 80}, {1, 154, 2160, 2160, 10, 3, 141}, 1440, 14180},          //HDMI_TX_4096_2160P_144HZ_CVT_RB,

    {121, 0, {2380, 5120, 88, 1996, 296}, {1, 40, 2160, 2160, 10, 8, 22}, 240, 3960},  //TX_TIMING_5120X2160P24
    {122, 0, {2080, 5120, 88, 1696, 296}, {1, 40, 2160, 2160, 10, 8, 22}, 250, 3960},//TX_TIMING_5120X2160P25
    {123, 0, {880, 5120, 88, 664, 128}, {1, 40, 2160, 2160, 10, 8, 22}, 300, 3960},  //TX_TIMING_5120X2160P30
    {124, 0, {1130, 5120, 88, 746, 296}, {1, 315, 2160, 2160, 10, 8, 297}, 480, 7425},  //TX_TIMING_5120X2160P48
    {125, 0, {1480, 5120, 88, 1096, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 500, 7425},    //TX_TIMING_5120X2160P50
    {126, 0, {380, 5120, 88, 164, 128}, {1, 90, 2160, 2160, 10, 8, 72}, 600, 7425},  //TX_TIMING_5120X2160P60
    {127, 0, {1480, 5120, 88, 1096, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 1000, 14850},  //TX_TIMING_5120X2160P100
    {193, 0, {380, 5120, 88, 164, 128}, {1, 90, 2160, 2160, 10, 8, 72}, 1200, 14850},    //TX_TIMING_5120X2160P120

    {0, 0, {160, 5120, 32, 48, 80}, {1, 19, 1440, 1440, 10, 3, 6}, 240, 1847},//TX_TIMING_5120X1440P24,
    {0, 0, {160, 5120, 32, 48, 80}, {1, 19, 1440, 1440, 10, 3, 6}, 250, 1925},//TX_TIMING_5120X1440P25,
    {0, 0, {160, 5120, 32, 48, 80}, {1, 31, 1440, 1440, 10, 3, 8}, 300, 2312},//TX_TIMING_5120X1440P30,
    {0, 0, {160, 5120, 32, 48, 80}, {1, 33, 1440, 1440, 10, 3, 20}, 480, 3732},//TX_TIMING_5120X1440P48,
    {0, 0, {160, 5120, 32, 48, 80}, {1, 34, 1440, 1440, 10, 3, 21}, 500, 3890},//TX_TIMING_5120X1440P50,
    {0, 0, {160, 5120, 32, 48, 80}, {1, 41, 1440, 1440, 10, 3, 28}, 600, 4690},//TX_TIMING_5120X1440P60,
    {0, 0, {160, 5120, 32, 48, 80}, {1, 70, 1440, 1440, 10, 3, 57}, 1000, 7972},//TX_TIMING_5120X1440P100,
    {0, 0, {160, 5120, 32, 48, 80}, {1, 85, 1440, 1440, 10, 3, 72}, 1200, 9660},//TX_TIMING_5120X1440P120,

    {0, 0, {160, 5120, 32, 48, 80}, {1, 33, 2880, 2880, 5, 3, 25}, 240, 3690},//TX_TIMING_5120X2880P24,
    {0, 0, {160, 5120, 32, 48, 80}, {1, 34, 2880, 2880, 5, 3, 26}, 250, 3845},//TX_TIMING_5120X2880P25,
    {0, 0, {160, 5120, 32, 48, 80}, {1, 41, 2880, 2880, 5, 3, 33}, 300, 4625},//TX_TIMING_5120X2880P30,
    {0, 0, {160, 5120, 32, 48, 80}, {1, 66, 2880, 2880, 5, 3, 58}, 480, 7465},//TX_TIMING_5120X2880P48,
    {0, 0, {160, 5120, 32, 48, 80}, {1, 68, 2880, 2880, 5, 3, 60}, 500, 7782},//TX_TIMING_5120X2880P50,
    {0, 0, {160, 5120, 32, 48, 80}, {1, 82, 2880, 2880, 5, 3, 74}, 600, 9382},//TX_TIMING_5120X2880P60,

    ///////dsce
    {97, 107, {560, 3840, 88, 176, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 600, 5940},  //HDMI_TX_DSC_3840_2160P_60HZ
    {118, 120, {560, 3840, 88, 176, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 1200, 11880},  //HDMI_TX_DSC_3840_2160P_120HZ
    {0, 0, {160, 3840, 32, 48, 80}, {1, 154, 2160, 2160, 5, 3, 146}, 1440, 13327},          //HDMI_TX_3840_2160P_144HZ_CVT_RB
};
#if 0
UINT8   strEdidFeatureType[_EDID_FEATURE_TYPE_NUM][EDID_STR_MAX]=
{ "VENDOR_MODEL",
    "EDID_VERSION",
    "EDID_SEG_READ_SUPPORT",
    "EDID_EXT_BLOCK_COUNT",
    "YCC444_SUPPORT",
    "YCC422_SUPPORT",
    "FIRST_EXT_DTD_ADDR",
    "LPCM_FS_SUPPORT",
    "3D_LPCM_FS_SUPPORT",
    "VCDB_QY",
    "VCDB_QS",
    "YCC420_SUPPORT",
    "DRR_SUPPORT",
    "AI_SUPPORT",
    "DC48_SUPPORT",
    "DC36_SUPPORT",
    "DC30_SUPPORT",
    "LATENCY_FIELD_PRESENT",
    "ILATENCY_FIELD_PRESENT",
    "HDMI_VIDEO_PRESENT",
    "DC_Y444_SUPPORT",
    "SCDC_PRESENT",
    "RR_CAPABLE",
    "LTE_340MCSC_SCRAMBLE",
    "DC48_420_SUPPORT",
    "DC36_420_SUPPORT",
    "DC30_420_SUPPORT",
    "HDMI_MAX_TMDS_CLK",
    "MAX_FRL_RATE",
    "VRR_MIN",
    "VRR_MAX",
    "DSC_VERSION_12",
    "DSC_NATIVE_420",
    "DSC_ALL_BPP",
    "DSC_16BPC",
    "DSC_12BPC",
    "DSC_10BPC",
    "DSC_MAX_FRL_RATE",
    "DSC_MAX_SLICES",
    "DSC_TOTAL_CHUNKKBYTE",
    "DID_EXT_SUPPORT",
    "SRC_PHYSICAL_ADDR",
    "VSD_8K_SUPPORT"};
#endif

//static UINT8 ucMatchCnt=0;

static UINT8 repeater_edid_support_table[_EDID_FEATURE_TYPE_NUM];
static UINT8 repeater_timing_support_table[_TXOUT_TIMING_SUPPORT_TYPE_NUM];
hdmi_timing_check_type  edid_timing ={0};
static UINT8 repeater_sink_tx_support_timing_cnt[EDID_TIMING_MODE_NUM] ={0};
EDID_SUPPORT_TIMING_TYPE repeater_support_timing_table={0};
extern hdmi_timing_check_type  dtd_timing;



#endif // #ifdef CONFIG_HDMITX_REPEATER_SUPPORT


/************************
// static/local variable Definition
//
*************************/

static UINT16 usDidSExtStartAddr = 0;
static UINT16 usDidSExtEndAddr = 0;
UINT8 ucY420CMDBAdjust[EDID_Y420CMDB_LEN] = {0x00};
EDID_CHECK_STATE g_ucHdmiMacTx0ModifyEdidState;
BOOLEAN g_bHdmiMacTx0ModifyEdidEnable = _FALSE;
UINT8 g_sinkEDID4K444Support = 0;
UINT8 g_sinkEDID4K420Support = 0;
#ifdef NOT_USED_NOW
UINT8 g_pucHdmiMacTx0EdidChecksum[4]; // [0] base checksum, [1] Extersion 1 checksum ...
#endif
UINT8 g_ucHdmiMacTx0ModifyEdidEvent;
UINT8 pEdidData[16];
UINT8 subEdidAddr[6];
UINT8 EDID_HDMI20[EDID_BUFFER_SIZE] = {0x00};
static UINT32 u32TmpValue2 = 0;
UINT8 *pucDdcRamAddr = 0;
extern UINT8  bCheckEdidForHdmitxSupport;
BOOLEAN bEdidHdmiFormat = _FALSE;
static UINT8 ucExtVdbBlockNum = 0;
//extern void (*cb_HdmiTxTimerEventHandler[_SCALER_TIMER_EVENT_NUM])(UINT32);
UINT16 pusExtVdbBlockAddr[_CTA_EXT_VDB_DB] = {0};
static UINT8 ucExt420VdbBlockNum = 0;
UINT16 pusExt420VdbBlockAddr[_CTA_EXT_420VDB_DB] = {0};
#ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
extern UINT8 edid_dtd_flag;
#endif

I2C_HDMITX_PARA_T i2c_para_info;
//I2C_XFER_INFO_T xfer_info;
#ifdef ENABLE_INTERNAL_HDMIRX_PTG
static BOOLEAN bDtdSupport4K60 = 0;
static BOOLEAN bSVDSupport4K60 = 0;
#endif

UINT8 tx_output_colorSpace = HDMITX_COLOR_UNKNOW;
extern  UINT8 tx_output_colordepth;
TX_TIMING_INDEX time_mode=TX_TIMING_NUM;
extern unsigned int bypass_port;


/* ***************************************
  Description  : Get TX timing table detail info
  Input Value  :
                        TX_TIMING_INDEX: tx timing index
                        hdmi_tx_timing_gen_st * : tx timing_table
  Return Value :
                        hdmi_tx_timing_gen_st *: TX timing table info
                        return: 0: SUCCESS
****************************************/
UINT8 ScalerHdmiTxPhy_GetTxTimingTable(UINT8 index, hdmi_tx_timing_gen_st *time_mode)
{

    if(time_mode == NULL){
        DebugMessageHDMITx("[HDMITX] NULL PTR@GetTxTimingTable\n");
        return -1;
    }

    if(index < TX_TIMING_NUM){
        memcpy((UINT8*)time_mode, (UINT8*)&cea_timing_table_repeater[(UINT8)index], sizeof(hdmi_tx_timing_gen_st));
        return 0;
    }else{
        DebugMessageHDMITx("[TX] Index over [%d]\n", (UINT32)index);
        //time_mode = NULL;
        return -1;
    }
}

#if((_DP_TX_SUPPORT == _ON) || (_HDMI_TX_SUPPORT == _ON) || (_VGA_TX_SUPPORT == _ON))
//extern PCB_ENUM_VAL HDMI_TX_I2C_BUSID;

extern INT32 i2c_master_recv_ex(UINT8 id, UINT8 addr,UINT8 *p_sub_addr, UINT8 sub_addr_len, UINT8 *p_read_buff, UINT32 read_len);
extern INT32 i2c_master_send_ex(UINT8 id, UINT8 addr, UINT8 *write_buff, UINT32 write_len);
INT32 _i2c_master_recv_ex(UINT8 id, UINT8 addr, UINT8 *p_sub_addr, UINT8 sub_addr_len, UINT8 *p_read_buff, UINT32 read_len)
{
    // fix compiler error when build I2C driver
    // [H5X_HDMITX_BYPASS_HPD_EDID]
    if(ScalerHdmiTxGetBypassLinkTrainingEn())
        return 0;
    else
        return i2c_master_recv_ex(id, addr, p_sub_addr, sub_addr_len, p_read_buff, read_len);
}

// ScalerMcuHwIICRead(0xA0, 0x01, ucEdidIndex, 16, &MCU_DDCRAM_P0[ucEdidIndex], _HDMI_TX0_IIC)
INT32 ScalerMcuHwIICRead(UINT8 id, UINT8 addr,
    UINT8 *p_sub_addr, UINT8 sub_addr_len,
    UINT8 *p_read_buff, UINT32 read_len, UINT16 flags,UINT8 port)
{
    // UINT8 id, UINT8 addr, UINT8 *p_sub_addr, UINT8 sub_addr_len, UINT8 *p_read_buff, unsigned int read_len

    if( _i2c_master_recv_ex(id, addr, p_sub_addr, sub_addr_len, p_read_buff, read_len) >= 0 )
    {
        return _SUCCESS;
    }
    else
    {
        return _FAIL;
    }
}

static INT32 _i2c_master_send_ex(UINT8 id, UINT8 addr, UINT8 *write_buff, UINT32 write_len)
{
    // [H5X_HDMITX_BYPASS_HPD_EDID]
    if(ScalerHdmiTxGetBypassLinkTrainingEn())
        return 0;
    else
        return i2c_master_send_ex(id, addr, write_buff, write_len);
}

// SCDS data access
// 0xA8, 0x01, 0x20, 0x01, &ucScdcSetValue, _HDMI_TX0_IIC
INT32 ScalerMcuHwIICWrite(UINT8 id, UINT8 addr,
    UINT8 *write_buff, UINT32 write_len,
    UINT8 flag, UINT8 port)
{
    // UINT8 id, UINT8 addr, UINT8 *write_buff, unsigned int write_len
    if(_i2c_master_send_ex(id, addr, write_buff, write_len) >= 0)
        return _SUCCESS;
    else
        return _FAIL;
}

#ifdef NOT_USED_NOW
UINT8 ScalerDpRx0GetDpcdBitInfo(UINT8 par1, UINT8 par2, UINT8 par3, UINT8 par4)
{
    // check if RX 4 lane or not (3 lane)
//#ifndef H5X_HDMITX_FIXME
//      #error "FIX-ME@ScalerDpRx0GetDpcdBitInfo()"
//#endif
    return _DP_FOUR_LANE;
}

//(0x74, 0x01, 0x40, 0x01, pData, _HDMI_TX0_IIC)
INT8 ScalerMcuHwIICHDCPBurstRead(UINT8 addr,
    UINT8 *p_sub_addr, UINT8 sub_addr_len,
    UINT8 *p_read_buff, UINT32 read_len, UINT16 flags, UINT8 port)
{
    if( _i2c_master_recv_ex( addr, p_sub_addr, sub_addr_len, p_read_buff, read_len) >= 0 )
        return _SUCCESS;
    else
        return _FAIL;
}
#endif // #ifdef NOT_USED_NOW

// (ucSegmentPoint, _EDID_ADDRESS, usEdidByteCnt, 16, &pucEdidResidual[0], _HDMI_TX0_IIC)

extern INT32 i2c_master_EDDC_ex(UINT8 id, UINT8 addr, UINT8 *p_sub_addr, UINT8 sub_addr_len, UINT8 *p_read_buff, UINT32 read_len);
INT32 ScalerMcuHwIICSegmentRead(UINT8 segmentPoint, UINT8 id, UINT8 addr,
    UINT8 *p_sub_addr, UINT8 sub_addr_len,
    UINT8 *p_read_buff, UINT32 read_len, UINT16 flags, UINT8 port)
{
    // EDID block 2, 3 (address: 0x100 ~ 0x1ff)
    if(segmentPoint == 1)
    {
#if 1//def _MARK2_FIXME_H5X_I2C_EDDC_API // [MARK2][NOT USED IN ZEBU VERIFY]  FIX-ME -- I2C EDDC API not exist on Mark2 branch yet
        if(i2c_master_EDDC_ex(id, addr, p_sub_addr, sub_addr_len, p_read_buff, read_len) >= 0 ){
            //NoteMessageHDMITx("[HDMITX] segRead2 %d/%x/%d/%d OK!\n", segmentPoint, *p_sub_addr, sub_addr_len, read_len);
            return _SUCCESS;
        }else
#endif // #if 0 // [MARK2] Disable for Bring up
        {
            FatalMessageHDMITx("[HDMITX][EDID] I2C segment read Fail!\n");
            return _FAIL;
        }
    }

    if( _i2c_master_recv_ex(id, addr, p_sub_addr, sub_addr_len, p_read_buff, read_len) >= 0 )
        return _SUCCESS;
    else
        return _FAIL;
}

#endif
#if(_HDMI_TX_SUPPORT == _ON)

//--------------------------------------------------
// Description  : ScalerHdmiMacTxCtaExtDbParse
// Input Value  : enumOutputPort
// Output Value :
//--------------------------------------------------
#ifdef NOT_USED_NOW
void ScalerHdmiMacTxEdidBlockChecksumBackup(EnumOutputPort enumOutputPort, BYTE ucBlockIndex, BYTE *pucDdcRamAddr)
{
    switch(EdidMacPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:

            g_pucHdmiMacTx0EdidChecksum[ucBlockIndex] = pucDdcRamAddr[ucBlockIndex * 0x80 + 0x7F];
            break;
#endif

#if(_HDMI_MAC_TX1_SUPPORT == _ON)
        case _TX1:

            g_pucHdmiMacTx1EdidChecksum[ucBlockIndex] = pucDdcRamAddr[ucBlockIndex * 0x80 + 0x7F];
            break;
#endif

#if(_HDMI_MAC_TX2_SUPPORT == _ON)
        case _TX2:

            g_pucHdmiMacTx2EdidChecksum[ucBlockIndex] = pucDdcRamAddr[ucBlockIndex * 0x80 + 0x7F];
            break;
#endif
        default:
            break;
    }
}
#endif
//--------------------------------------------------
// Description  : Edid Set Feature
// Input Value  : enumOutputPort, enumEdidFeature
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0EdidSetFeature(EnumEdidFeatureType enumEdidFeature, UINT8 ucValue)
{
    switch(enumEdidFeature)
    {
        case _VENDOR_MODEL:

            g_stHdmiMacTx0EDIDInfo.ucVendorModel = ucValue;
            DebugMessageHDMITx("[HDMI_TX] _VENDOR_MODEL=%d\n", (UINT32)ucValue);
            break;

        case _EDID_VERSION:

            g_stHdmiMacTx0EDIDInfo.b2EdidVer = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID Version=%d\n", (UINT32)ucValue);
            break;

        case _DID_EXT_SUPPORT:
            g_stHdmiMacTx0EDIDInfo.b1DidExist = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID exist DID Ext=%d\n", (UINT32)ucValue);
            break;

        case _EDID_EXT_BLOCK_COUNT:
            g_stHdmiMacTx0EDIDInfo.b8Edid_ExtBlockCount = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID Ext Block Count=%d\n", (UINT32)ucValue);
            break;

        case _EDID_SEG_READ_SUPPORT:

            g_bHdmiMacTx0SegmentReadable = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID SegRead Support=%d\n", (UINT32)ucValue);
            break;

        case _YCC444_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.b1YCbCr444Support = ucValue;
            break;

        case _YCC422_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.b1YCbCr422Support = ucValue;
            break;

        case _FIRST_EXT_DTD_ADDR:

            g_stHdmiMacTx0EDIDInfo.usHDMITxEDIDExtDTDStartAddr = ucValue;
            break;

        case _LPCM_FS_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.ucLpcmFsSupport = ucValue;
            break;

        case _3D_LPCM_FS_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.uc3DLpcmFsSupport = ucValue;
            break;

        case _VCDB_QY:

            g_stHdmiMacTx0EDIDInfo.b1QYSupport = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID Support QY@01\n");
            break;

        case _VCDB_QS:

            g_stHdmiMacTx0EDIDInfo.b1QSSupport = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID Support QS@02\n");
            break;

        case _YCC420_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.b1YCbCr420Support = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID Support420@03\n");
            break;

        case _DRR_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.b1DrrSupport = ucValue;
            break;

        case _AI_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.b1AISupport = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID AI Support@04\n");
            break;

        case _DC48_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.b1DC_48bitSupport = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID DC48 Support@05\n");
            break;

        case _DC36_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.b1DC_36bitSupport = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID DC36 Support@06\n");
            break;

        case _DC30_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.b1DC_30bitSupport = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID DC30 Support@07\n");
            break;
        case _LATENCY_FIELD_PRESENT:

            g_stHdmiMacTx0EDIDInfo.b1LagencyFeild_Present = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID LagencyFeild_Present Support@05\n");
            break;

        case _ILATENCY_FIELD_PRESENT:

            g_stHdmiMacTx0EDIDInfo.b1ILagencyFeild_Present = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID ILagencyFeild_Present Support@06\n");
            break;

        case _HDMI_VIDEO_PRESENT:

            g_stHdmiMacTx0EDIDInfo.b1HDMI_Video_Present = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID HDMI_Video_Present Support@07\n");
            break;
        case _DC_Y444_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.b1DC_Y444Support = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID Y444 Support@08\n");
            break;

        case _SCDC_PRESENT:

            g_stHdmiMacTx0EDIDInfo.b1SCDC_Present = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID SCDC Support@09\n");
            break;

        case _RR_CAPABLE:

            g_stHdmiMacTx0EDIDInfo.b1RR_Capable = ucValue;
            break;

        case _LTE_340MCSC_SCRAMBLE:

            g_stHdmiMacTx0EDIDInfo.b1LTE_340Mcsc_scramble = ucValue;
            break;

        case _DC48_420_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.b1DC_48bitSupportFor420 = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID DC_48bit_420 Support@10\n");
            break;

        case _DC36_420_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.b1DC_36bitSupportFor420 = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID DC_36bit_420 Support@11\n");
            break;

        case _DC30_420_SUPPORT:

            g_stHdmiMacTx0EDIDInfo.b1DC_30bitSupportFor420 = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID DC_30bit_420 Support@12\n");
            break;

        case _HDMI_MAX_TMDS_CLK:

            g_stHdmiMacTx0EDIDInfo.ucMaxTMDSClkSupport = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID HDMI_MAX_TMDS_CLK=%d\n", (UINT32)ucValue);
            break;

        case _MAX_FRL_RATE:

            g_stHdmiMacTx0EDIDInfo.b4Max_FRL_LR = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID MAX FRL RATE=%d\n", (UINT32)ucValue);
            break;

        case _VRR_MIN:

            g_stHdmiMacTx0EDIDInfo.b6VRR_min = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID VRR_MIN=%d\n", (UINT32)ucValue);
            break;

        case _VRR_MAX:

            g_stHdmiMacTx0EDIDInfo.b10VRR_max = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID VRR_MAX=%d\n", (UINT32)ucValue);
            break;

        case _DSC_VERSION_12:

            g_stHdmiMacTx0EDIDInfo.b1DSC12_Support = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID DSC_1p2 support@14\n");
            break;

        case _DSC_NATIVE_420:

            g_stHdmiMacTx0EDIDInfo.b1DSC_Native420_Support = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID _DSC_NATIVE_420@15\n");
            break;

        case _DSC_ALL_BPP:

            g_stHdmiMacTx0EDIDInfo.b1DSC_All_bpp = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID _DSC_ALL_BPP@16\n");
            break;

        case _DSC_16BPC:

            g_stHdmiMacTx0EDIDInfo.b1DSC_16bpc = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID _DSC_16BPC@17\n");
            break;

        case _DSC_12BPC:

            g_stHdmiMacTx0EDIDInfo.b1DSC_12bpc = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID _DSC_12BPC@18\n");
            break;

        case _DSC_10BPC:

            g_stHdmiMacTx0EDIDInfo.b1DSC_10bpc = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID _DSC_10BPC@19\n");
            break;

        case _DSC_MAX_FRL_RATE:

            g_stHdmiMacTx0EDIDInfo.b4DSC_Max_LR = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID DSC MAX FRL RATE=%d\n", (UINT32)ucValue);
            break;

        case _DSC_MAX_SLICES:

            g_stHdmiMacTx0EDIDInfo.b4DSC_Max_Slices = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID DSC MAX SLICE=%d\n", (UINT32)ucValue);
            break;

        case _DSC_TOTAL_CHUNKKBYTE:

            g_stHdmiMacTx0EDIDInfo.b4DSC_TotalChunkKBytes = ucValue;

            DebugMessageHDMITx("[HDMI_TX] EDID DSC Total chunkbyte=%d\n", (UINT32)ucValue);
            break;

        case _SRC_PHYSICAL_ADDR:
            g_stHdmiMacTx0EDIDInfo.ucSRC_PHY_ADDR = ucValue;
           DebugMessageHDMITx("[HDMI_TX] EDID SRC_PHYSICAL_ADDR=%x\n", (UINT32)ucValue);
            break;

        case _VSD_8K_SUPPORT:
            g_stHdmiMacTx0EDIDInfo.b1vsd8kSupport = ucValue;
           DebugMessageHDMITx("[HDMI_TX] EDID VSD_8K=%d\n", (UINT32)ucValue);
            break;
        case _FAPA_End_Extended:

            g_stHdmiMacTx0EDIDInfo.b1FAPA_End_Extended = ucValue;
            break;
        case _QMS:

            g_stHdmiMacTx0EDIDInfo.b1QMS = ucValue;
            break;
        case _MDelta:

            g_stHdmiMacTx0EDIDInfo.b1MDelta = ucValue;
            break;
        case _CINEMAVRR:

            g_stHdmiMacTx0EDIDInfo.b1CinemaVRR = ucValue;
            break;
        case _NEG_MVRR:

            g_stHdmiMacTx0EDIDInfo.b1NEG_MVRR = ucValue;
            break;
        case _FVA:

            g_stHdmiMacTx0EDIDInfo.b1FVA = ucValue;
            break;
        case _3D_OSD_Disparity:

            g_stHdmiMacTx0EDIDInfo.b13D_OSD_Disparity = ucValue;
            break;
        case _Dual_View:

            g_stHdmiMacTx0EDIDInfo.b1Dual_View = ucValue;
            break;
        case _Independent_view:

            g_stHdmiMacTx0EDIDInfo.b1Independent_view = ucValue;
            break;

        case _BT2020_cYCC:

            g_stHdmiMacTx0EDIDInfo.b1BT2020cYCC = ucValue;
            break;
        case _opRGB:

            g_stHdmiMacTx0EDIDInfo.b1opRGB = ucValue;
            break;
        case _opYCC601:

            g_stHdmiMacTx0EDIDInfo.b1opYCC601 = ucValue;
            break;

        case _ST2113RGB23:

            g_stHdmiMacTx0EDIDInfo.b1ST2113RGB23 = ucValue;
            break;
        case _ICTCP:

            g_stHdmiMacTx0EDIDInfo.b1ICTCP = ucValue;
            break;
        case _HDR_SMPTE_ST2084:

            g_stHdmiMacTx0EDIDInfo.b1HDR2084 = ucValue;
            break;
        case _HDR_HLG:

            g_stHdmiMacTx0EDIDInfo.b1HDRHLG = ucValue;
            break;
        default:
            break;
    }
}

//--------------------------------------------------
// Description  : Edid Get Feature
// Input Value  : enumOutputPort, enumEdidFeature
// Output Value :
//--------------------------------------------------
UINT8 ScalerHdmiMacTx0EdidGetFeature(EnumEdidFeatureType enumEdidFeature)
{
    switch(enumEdidFeature)
    {
        case _VENDOR_MODEL:

            return g_stHdmiMacTx0EDIDInfo.ucVendorModel;
            break;

        case _EDID_VERSION:

            return g_stHdmiMacTx0EDIDInfo.b2EdidVer;
            break;

        case _EDID_SEG_READ_SUPPORT:

            return g_bHdmiMacTx0SegmentReadable;
            break;

        case _DID_EXT_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1DidExist;
            break;

        case _EDID_EXT_BLOCK_COUNT:
            return g_stHdmiMacTx0EDIDInfo.b8Edid_ExtBlockCount;
            break;

        case _YCC444_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1YCbCr444Support;
            break;

        case _YCC422_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1YCbCr422Support;
            break;

        case _FIRST_EXT_DTD_ADDR:

            return g_stHdmiMacTx0EDIDInfo.usHDMITxEDIDExtDTDStartAddr;
            break;

        case _LPCM_FS_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.ucLpcmFsSupport;
            break;

        case _3D_LPCM_FS_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.uc3DLpcmFsSupport;
            break;

        case _VCDB_QY:

            return g_stHdmiMacTx0EDIDInfo.b1QYSupport;
            break;

        case _VCDB_QS:

            return g_stHdmiMacTx0EDIDInfo.b1QSSupport;
            break;

        case _YCC420_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1YCbCr420Support;
            break;

        case _DRR_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1DrrSupport;
            break;

        case _AI_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1AISupport;
            break;

        case _DC48_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1DC_48bitSupport;
            break;

        case _DC36_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1DC_36bitSupport;
            break;

        case _DC30_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1DC_30bitSupport;
            break;
        case _LATENCY_FIELD_PRESENT:

            return g_stHdmiMacTx0EDIDInfo.b1LagencyFeild_Present;
            break;

        case _ILATENCY_FIELD_PRESENT:

            return g_stHdmiMacTx0EDIDInfo.b1ILagencyFeild_Present;
            break;

        case _HDMI_VIDEO_PRESENT:

            return g_stHdmiMacTx0EDIDInfo.b1HDMI_Video_Present;
            break;
        case _DC_Y444_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1DC_Y444Support;
            break;

        case _SCDC_PRESENT:

            return g_stHdmiMacTx0EDIDInfo.b1SCDC_Present;
            break;

        case _RR_CAPABLE:

            return g_stHdmiMacTx0EDIDInfo.b1RR_Capable;
            break;

        case _LTE_340MCSC_SCRAMBLE:

            return g_stHdmiMacTx0EDIDInfo.b1LTE_340Mcsc_scramble;
            break;

        case _DC48_420_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1DC_48bitSupportFor420;
            break;

        case _DC36_420_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1DC_36bitSupportFor420;
            break;

        case _DC30_420_SUPPORT:

            return g_stHdmiMacTx0EDIDInfo.b1DC_30bitSupportFor420;
            break;

        case _HDMI_MAX_TMDS_CLK:

            return g_stHdmiMacTx0EDIDInfo.ucMaxTMDSClkSupport;
            break;

        case _MAX_FRL_RATE:
            return g_stHdmiMacTx0EDIDInfo.b4Max_FRL_LR;
            break;

        case _VRR_MIN:
            return g_stHdmiMacTx0EDIDInfo.b6VRR_min;
            break;

        case _VRR_MAX:
            return g_stHdmiMacTx0EDIDInfo.b10VRR_max;
            break;

        case _DSC_VERSION_12:

            return g_stHdmiMacTx0EDIDInfo.b1DSC12_Support;
            break;

        case _DSC_NATIVE_420:

            return g_stHdmiMacTx0EDIDInfo.b1DSC_Native420_Support;
            break;

        case _DSC_ALL_BPP:

            return g_stHdmiMacTx0EDIDInfo.b1DSC_All_bpp;
            break;

        case _DSC_16BPC:

            return g_stHdmiMacTx0EDIDInfo.b1DSC_16bpc;
            break;

        case _DSC_12BPC:

            return g_stHdmiMacTx0EDIDInfo.b1DSC_12bpc;
            break;

        case _DSC_10BPC:

            return g_stHdmiMacTx0EDIDInfo.b1DSC_10bpc;
            break;

        case _DSC_MAX_FRL_RATE:

            return g_stHdmiMacTx0EDIDInfo.b4DSC_Max_LR;
            break;

        case _DSC_MAX_SLICES:

            return g_stHdmiMacTx0EDIDInfo.b4DSC_Max_Slices;
            break;

        case _DSC_TOTAL_CHUNKKBYTE:

            return g_stHdmiMacTx0EDIDInfo.b4DSC_TotalChunkKBytes;
            break;

        case _SRC_PHYSICAL_ADDR:
            return g_stHdmiMacTx0EDIDInfo.ucSRC_PHY_ADDR;
            break;

        case _VSD_8K_SUPPORT:
            return g_stHdmiMacTx0EDIDInfo.b1vsd8kSupport;
            break;
        case _FAPA_End_Extended:

            return g_stHdmiMacTx0EDIDInfo.b1FAPA_End_Extended;
            break;
        case _FVA:

            return g_stHdmiMacTx0EDIDInfo.b1FVA;
            break;
        case _QMS:

            return g_stHdmiMacTx0EDIDInfo.b1QMS;
            break;
        case _CINEMAVRR:

            return g_stHdmiMacTx0EDIDInfo.b1CinemaVRR;
            break;
        case _NEG_MVRR:

            return g_stHdmiMacTx0EDIDInfo.b1NEG_MVRR;
            break;
        case _MDelta:

            return g_stHdmiMacTx0EDIDInfo.b1MDelta;
            break;
        case _Dual_View:

            return g_stHdmiMacTx0EDIDInfo.b1Dual_View;
            break;
        case _3D_OSD_Disparity:

            return g_stHdmiMacTx0EDIDInfo.b13D_OSD_Disparity;
            break;
        case _Independent_view:

            return g_stHdmiMacTx0EDIDInfo.b1Independent_view;
            break;

        case _BT2020_cYCC:

            return g_stHdmiMacTx0EDIDInfo.b1BT2020cYCC;
            break;
        case _opRGB:

            return g_stHdmiMacTx0EDIDInfo.b1opRGB;
            break;
        case _opYCC601:

            return g_stHdmiMacTx0EDIDInfo.b1opYCC601;
            break;

        case _ST2113RGB23:

            return g_stHdmiMacTx0EDIDInfo.b1ST2113RGB23;
            break;
        case _ICTCP:

            return g_stHdmiMacTx0EDIDInfo.b1ICTCP;
            break;
        case _HDR_SMPTE_ST2084:

            return g_stHdmiMacTx0EDIDInfo.b1HDR2084;
            break;
        case _HDR_HLG:

            return g_stHdmiMacTx0EDIDInfo.b1HDRHLG;
        break;
        default:
            break;
    }
    return 0;
}
//--------------------------------------------------
// Description  : Edid Get Feature
// Input Value  : enumOutputPort, enumEdidFeature
// Output Value :
//--------------------------------------------------
BYTE ScalerHdmiMacTxEdidGetFeature(EnumOutputPort enumOutputPort, EnumEdidFeatureType enumEdidFeature)
{
    switch(EdidMacPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:

            return ScalerHdmiMacTx0EdidGetFeature(enumEdidFeature);
            break;
#endif
        default:

            break;
    }
    return _FALSE;
}

//--------------------------------------------------
// Description  : Edid Get VIC Table
// Input Value  : enumOutputPort, enumEdidFeature
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTxGetEdidVicTable(EnumOutputPort enumOutputPort, EnumEdidCtaDataBlock enumVDBType, BYTE *pucVICTable, UINT16 *pusCtaDataBlockAddr)
{
    switch(EdidMacPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiMacTx0GetEdidVicTable(enumVDBType, pucVICTable, pusCtaDataBlockAddr);
            break;
#endif

        default:

            break;
    }
}
//--------------------------------------------------
// Description  : HDMI Check HDMI Format Edid
// Input Value  : enumOutputPort, pucDdcRamAddr
// Output Value :
//--------------------------------------------------

void ScalerHdmiMacTxCheckSinkEdidFormat(EnumOutputPort enumOutputPort, UINT16 *pusCtaDataBlockAddr)
{
    UINT16 usDetailTimeOffset= 0x00;
    UINT16 usEDIDExtBase= 0x7E;
    UINT16 usEDIDFormatBase = 0x14;
    UINT8 ucOffset = 0x00;
    UINT16 usNewDataAdd = 0x84;
    UINT8 ucVSDBLength= 0x05;
    UINT8 ucEdidExtBlockNum;
    UINT16 usExtDetailTimeBase = 0x82; // Detailed Timing address of Extension Block
    UINT8 u8TmpValue = 0;
    bEdidHdmiFormat = _FALSE;
    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacPxTxMapping(_TX0));
    if(ScalerOutputGetDeviceCurrentDeviceType(enumOutputPort) == _DFP_TYPE_HDMI)
    {
        usDetailTimeOffset = 0x80 + pucDdcRamAddr[usExtDetailTimeBase];

        // For HDMI 1.4b CTS - Test ID 7-33: Interoperability With DVI
        if((pucDdcRamAddr[usEDIDExtBase] & 0x03) >= 0x01)
        {
            ScalerHdmiMacTx0EdidSetFeature( _FIRST_EXT_DTD_ADDR, (pucDdcRamAddr[0x82] + 0x80));

            // [RTKREQ-854][CTS] HDMI 1.4b 7-33 Iter-05: HDMI 4 block EDID
            ucEdidExtBlockNum = pucDdcRamAddr[usEDIDExtBase];
            DebugMessageHDMITx("[HDMITX][EDID][2] ExtBlock Num/Tag=%d/%x\n", (UINT32)ucEdidExtBlockNum, (UINT32)pucDdcRamAddr[EDID_BLOCK1_TAG_BASE]);
#if 1
            if(((ucEdidExtBlockNum == 2) || (ucEdidExtBlockNum == 3)) && (pucDdcRamAddr[EDID_BLOCK1_TAG_BASE] == EDID_BLOCK_MAP_TAG)){
                if(pucDdcRamAddr[EDID_BLOCK2_TAG_BASE] == EDID_EXTENSION_TAG){ // EDID extension tag of block 2
                    usExtDetailTimeBase = EDID_BLOCK2_EXT_DTD_BASE; // Detailed Timing address of extension Block 2
                    usNewDataAdd = EDID_BLOCK2_EXT_DTD_ADDR; // Detail timing info base address of extension block 2
                    usDetailTimeOffset = EDID_BLOCK2_TAG_BASE + pucDdcRamAddr[usExtDetailTimeBase];
                    DebugMessageHDMITx("[HDMITX][EDID][2] Get Block2 extension data[%x->%x]\n", (UINT32)usNewDataAdd, (UINT32)usDetailTimeOffset);
                }
                else if((ucEdidExtBlockNum == 3) && (pucDdcRamAddr[EDID_BLOCK3_TAG_BASE] == EDID_EXTENSION_TAG)){
                    usExtDetailTimeBase = EDID_BLOCK3_EXT_DTD_BASE; // Detailed Timing address of extension Block 3
                    usNewDataAdd = EDID_BLOCK3_EXT_DTD_ADDR; // Detail timing info base address of extension block 3
                    usDetailTimeOffset = EDID_BLOCK3_TAG_BASE + pucDdcRamAddr[usExtDetailTimeBase];
                    DebugMessageHDMITx("[HDMITX][EDID][2] Get Block3 extension data[%x->%x]\n", (UINT32)usNewDataAdd, (UINT32)usDetailTimeOffset);
                }
            }
#endif
            // Find the Vendor Specific Data Block and check the length
            while(usNewDataAdd < usDetailTimeOffset)
            {
                // VSDB
                if((pucDdcRamAddr[usNewDataAdd] & 0xE0) == 0x60)
                {
                    ucVSDBLength = (pucDdcRamAddr[usNewDataAdd] & 0x1F);
                    break;
                }
                else
                {
                    ucOffset = (pucDdcRamAddr[usNewDataAdd] & 0x1F);  // Length of data block
                    usNewDataAdd = (usNewDataAdd + ucOffset + 1);
                }

                if(usNewDataAdd <= (0x80)) // Should not happen. if overflow, then break.
                {
                    break;
                }
                else if((usNewDataAdd >= usDetailTimeOffset) && (usExtDetailTimeBase == EDID_BLOCK2_EXT_DTD_BASE)){ // Read EDID block3
                    if((ucEdidExtBlockNum == 3) && (pucDdcRamAddr[EDID_BLOCK3_TAG_BASE] == EDID_EXTENSION_TAG)){
                        usExtDetailTimeBase = EDID_BLOCK3_EXT_DTD_BASE; // Detailed Timing address of extension Block 3
                        usNewDataAdd = EDID_BLOCK3_EXT_DTD_ADDR; // Detail timing info base address of extension block 3
                        usDetailTimeOffset = EDID_BLOCK3_TAG_BASE + pucDdcRamAddr[usExtDetailTimeBase];
                        DebugMessageHDMITx("[HDMITX][EDID][2] Get Block3 extension data[%x->%x]\n", (UINT32)usNewDataAdd, (UINT32)usDetailTimeOffset);
                    }
                }
            }
        }

        // [RTKREQ-854] dump EDID info
        u8TmpValue = (pucDdcRamAddr[usEDIDFormatBase] & _BIT7);
        if(u8TmpValue == _BIT7){
            u8TmpValue = 1;
        }
        else{
            u8TmpValue = 0;
        }
        DebugMessageHDMITx("[HDMITX][EDID] Digital/Ext/Size/VsdbAddr/DTD_Base=%d/%d/",(UINT32)u8TmpValue, (UINT32)pucDdcRamAddr[usEDIDExtBase]);
        DebugMessageHDMITx("%d/%x",(UINT32)ucVSDBLength, (UINT32)pusCtaDataBlockAddr[_CTA_HDMI_VSDB]);
        DebugMessageHDMITx("/%x\n",(UINT32)usDetailTimeOffset);
        // If EDID is digital & Extension BOOLEAN is 0x01 & HDMI VSDB exist
        if(((pucDdcRamAddr[usEDIDFormatBase] & _BIT7) == _BIT7) && (pucDdcRamAddr[usEDIDExtBase] >= 0x01) && (ucVSDBLength >= 0x03) && (pusCtaDataBlockAddr[_CTA_HDMI_VSDB] != 0x00))
        {
            SET_HDMI_EDID_HDMI_MODE();//hdmi
        }
        else if((pucDdcRamAddr[usEDIDFormatBase] & _BIT7) == _BIT7) // If EDID is digital without Extension
        {
            CLR_HDMI_EDID_HDMI_MODE();//dvi:disable scrameble
        }

        switch(EdidMacPxTxMapping(enumOutputPort))
        {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
            case _TX0:
                SET_HDMI_MAC_TX0_SUPPORT(GET_HDMI_EDID_HDMI_MODE());
                break;
#endif
            default:
                break;
        }
    }
}

//--------------------------------------------------
// Description  : Edid Set Tmds Max Clk
// Input Value  : enumOutputPort, enumEdidFeature
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTxSetSinkTmdsSupportClk(EnumOutputPort enumOutputPort, BYTE ucHdmiVsdbMaxTMDSClk, BYTE ucHfVsdbMaxTMDSClk)
{

    DebugMessageHDMITx("[HDMITX] ScalerHdmiMacTxSetSinkTmdsSupportClk() \n");
    switch(EdidMacPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiMacTx0SetSinkTmdsSupportClk(ucHdmiVsdbMaxTMDSClk, ucHfVsdbMaxTMDSClk);
            break;
#endif

        default:

            break;
    }
}
//--------------------------------------------------
// Description  : HDMI Tx Set Modify Event
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTxEdidModifyEnable(EnumOutputPort enumOutputPort, BOOLEAN bEnable, EnumHdmiEdidModifyEvent enumEdidModifyEvent)
{
    switch(EdidMacPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:
            ScalerHdmiMacTx0EdidModifyEnable(bEnable, enumEdidModifyEvent);
            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }
}

#ifdef NOT_USED_NOW
//--------------------------------------------------
// Description  : Hdmi Edid's DID Extension Data Block Parse
// Input Value  : enumOutputPort, pusCtaDataBlockAddr
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTxEdidDidExtDbParse(EnumOutputPort enumOutputPort, UINT16 *pusDidDataBlockAddr, BYTE *pucDdcRamAddr)
{
    UINT16 usDbStartAddr = 0x00;
    BYTE ucDataBlockIndex = 0x00;
    BYTE ucEDIDBlockBytesNum = 0x00;
    BOOLEAN bDidExist = _FALSE;
    BYTE ucI = 0x00;

    for(ucDataBlockIndex = _DID_T1_DB; ucDataBlockIndex <= _DID_TOTAL_DB; ucDataBlockIndex++)
    {
        if(pusDidDataBlockAddr[ucDataBlockIndex] != 0x00)
        {
            bDidExist = _TRUE;

            usDbStartAddr = pusDidDataBlockAddr[ucDataBlockIndex];
            ucEDIDBlockBytesNum = pucDdcRamAddr[usDbStartAddr + 2];

            switch(ucDataBlockIndex)
            {
                case _DID_T1_DB:

                    // Catch the First Prefer Timing Type 1 address
                    for(ucI = 0; ucI < (ucEDIDBlockBytesNum / 20); ucI++)
                    {
                        if((pucDdcRamAddr[usDbStartAddr + 6 + (20 * ucI)] & 0x80) == 0x80)
                        {
                            ScalerHdmiMacTxSetDidPreferTimingAddr(enumOutputPort, usDbStartAddr + 3 + (20 * ucI));
                            break;
                        }
                    }
                    break;

                default:
                    break;
            }
        }
    }

    ScalerHdmiMacTx0EdidSetFeature( _DID_EXT_SUPPORT, bDidExist);
}



//--------------------------------------------------
// Description  : Hdmi 2p1 Set Stream Source Type
// Input Value  : port
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
void ScalerHdmiMacTxSetDidPreferTimingAddr(EnumOutputPort enumOutputPort, UINT16 usAddress)
{
    switch(EdidMacPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:
            SET_HDMI_MAC_TX0_DID_PRFER_TIMING_ADDR(usAddress);
            break;
#endif

        default:

            break;
    }
}
#endif // #ifdef NOT_USED_NOW
//--------------------------------------------------
// Description  : Hdmi Edid's CTA Extension Data Block Parse
// Input Value  : enumOutputPort, pusCtaDataBlockAddr
// Output Value :
//--------------------------------------------------

void ScalerHdmiMacTxEdidCtaExtDbParse(EnumOutputPort enumOutputPort, UINT16 *pusCtaDataBlockAddr)
{
    UINT16 usDbStartAddr = 0x00;
    UINT8 u8TmpValue = 0;
    UINT8 ucDataBlockIndex = 0x00;
    UINT8 ucEDIDBlockBytesNum = 0x00;
    UINT8 ucI= 0x00;
    UINT8 ucVicCode = 0x00;
    UINT8 ucHdmiVsdbMaxTMDSClk = 0;
    UINT8 ucHfVsdbMaxTMDSClk = 0;
    UINT16 usVideoDbStartAddr;
    UINT8 value_u8 = 0;

    usVideoDbStartAddr = pusCtaDataBlockAddr[_CTA_VDB];

    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacPxTxMapping(_TX0));
    // *****************************************************
    // Part1: Anasyis Edid from Single Data Block
    // *****************************************************
    // CTA Extension Header Global Declarations (Basic Support)
    #if 1
    if(pucDdcRamAddr[usCtaExtBasicSupportAddr] & _BIT5)
        ScalerHdmiMacTx0EdidSetFeature( _YCC444_SUPPORT, _TRUE);
    if(pucDdcRamAddr[usCtaExtBasicSupportAddr]& _BIT4)
        ScalerHdmiMacTx0EdidSetFeature( _YCC422_SUPPORT, _TRUE);
    u8TmpValue = ScalerHdmiMacTxEdidGetFeature(enumOutputPort, _YCC444_SUPPORT);
    DebugMessageHDMITx("[HDMITX] BasicSupport=%x, YCC 444/422 SUPPORT=",(UINT32)pucDdcRamAddr[usCtaExtBasicSupportAddr]);
    DebugMessageHDMITx("%d",(UINT32)u8TmpValue);
    u8TmpValue = ScalerHdmiMacTxEdidGetFeature(enumOutputPort, _YCC422_SUPPORT);
    DebugMessageHDMITx("/%d\n",(UINT32)u8TmpValue);
    #endif
    for(ucDataBlockIndex = _CTA_VDB; ucDataBlockIndex <= _CTA_TOTAL_DB; ucDataBlockIndex++)
    {
        if(pusCtaDataBlockAddr[ucDataBlockIndex] != 0x00)
        {
            usDbStartAddr = pusCtaDataBlockAddr[ucDataBlockIndex];
            ucEDIDBlockBytesNum = (pucDdcRamAddr[usDbStartAddr] & 0x1F);

            switch(ucDataBlockIndex)
            {
                case _CTA_VDB:
                    break;

                case _CTA_ADB:

                    for(ucI = 0; ucI < (ucEDIDBlockBytesNum / 3); ucI++)
                    {
                        // LPCM SAD
                        switch(pucDdcRamAddr[(usDbStartAddr + 1 + (ucI * 3))] & (_BIT6 | _BIT5 | _BIT4 | _BIT3))
                        {
                            case _BIT3:  // lpcm SAD

                                ScalerHdmiMacTx0EdidSetFeature( _LPCM_FS_SUPPORT, (pucDdcRamAddr[(usDbStartAddr + 2 + (ucI * 3))] & 0x7F));
                                break;

                            case (_BIT6 | _BIT5 | _BIT4 | _BIT3):  // Extersion SAD

                                if((pucDdcRamAddr[(usDbStartAddr + 3 + (ucI * 3))] & (_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3)) == (_BIT6 | _BIT5 | _BIT3)) // 3D SAD
                                {
                                    ScalerHdmiMacTx0EdidSetFeature( _3D_LPCM_FS_SUPPORT, (pucDdcRamAddr[(usDbStartAddr + 2 + (ucI * 3))] & 0x7F));
                                }
                                break;

                            default:
                                break;
                        }
                    }
                    break;

                case _CTA_VCDB:

                    if((pucDdcRamAddr[usDbStartAddr + 2] & 0x80) == _BIT7)
                    {
                        ScalerHdmiMacTx0EdidSetFeature( _VCDB_QY, _TRUE);
                    }

                    if((pucDdcRamAddr[usDbStartAddr + 2] & 0x40) == _BIT6)
                    {
                        ScalerHdmiMacTx0EdidSetFeature( _VCDB_QS, _TRUE);
                    }
                    break;

                case _CTA_420VDB:
                    u8TmpValue = 0;
                    if(ucEDIDBlockBytesNum > 0)
                    {
                        u8TmpValue = ucEDIDBlockBytesNum -1;
                    }
                    for(ucI = 1; ucI < u8TmpValue; ucI++)
                    {
                        ucVicCode = pucDdcRamAddr[(usDbStartAddr + (ucI + 1))];

#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
                        DebugMessageHDMITx("[HDMITX] 420VDB[%d] VIC=%x\n", (UINT32)ucI, (UINT32)ucVicCode);
#endif
                        if((ucVicCode == 0x60) || (ucVicCode == 0x61) || (ucVicCode == 0x65) ||
                           (ucVicCode == 0x66) || (ucVicCode == 0x6A) || (ucVicCode == 0x6B))
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _YCC420_SUPPORT, _TRUE);
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
                            DebugMessageHDMITx("[HDMITX] 420VDB Support 420\n");
#endif
                            break;
                        }
                    }
                    break;

                case _CTA_420CMDB:

                    if(usVideoDbStartAddr != 0x00)
                    {
                        BYTE ucByteOffsetValue = 0;
                        BYTE ucBitOffsetValue = 0;
                        UINT16 usVideoDbBytesNum = (pucDdcRamAddr[usVideoDbStartAddr] & 0x1F);

                        for(ucI = 1; ucI <= usVideoDbBytesNum; ucI++)
                        {
                            ucVicCode = pucDdcRamAddr[(usVideoDbStartAddr + (ucI))];

#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
                            DebugMessageHDMITx("[HDMITX] 420CMDB[%d] VIC=0x%x\n", (UINT32)ucI, (UINT32)ucVicCode);
#endif
                            // Find 4K 60/50 Timing in Video Data Block First
                            if((ucVicCode == 0x60) || (ucVicCode == 0x61) || (ucVicCode == 0x65) ||
                               (ucVicCode == 0x66) || (ucVicCode == 0x6A) || (ucVicCode == 0x6B))
                            {
                                ucByteOffsetValue = ((ucI - 1) / 8) + 1;
                                ucBitOffsetValue = (ucI - 1) % 8;

                                // Check if 420 Capability BOOLEAN mapping to 4K 60/50 Timing is 1
                                if((pucDdcRamAddr[usDbStartAddr + ucByteOffsetValue + 1] != 0x00) &&
                                   (((pucDdcRamAddr[usDbStartAddr + ucByteOffsetValue + 1] >> ucBitOffsetValue) & _BIT0) == _BIT0))
                                {
                                    ScalerHdmiMacTx0EdidSetFeature( _YCC420_SUPPORT, _TRUE);
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
                                    DebugMessageHDMITx("[HDMITX] 420CMDB Support 420\n");
#endif
                                    break;
                                }
                            }
                        }
                    }
                    break;

                case _CTA_CDB:
                    if((pucDdcRamAddr[usDbStartAddr + 2] & 0x20) == _BIT5)
                    {
                        ScalerHdmiMacTx0EdidSetFeature( _BT2020_cYCC, _TRUE);
                    }
                     if((pucDdcRamAddr[usDbStartAddr + 2] & 0x10) == _BIT4)
                    {
                        ScalerHdmiMacTx0EdidSetFeature( _opRGB, _TRUE);
                    }

                    if((pucDdcRamAddr[usDbStartAddr + 2] & 0x8) == _BIT3)
                    {
                        ScalerHdmiMacTx0EdidSetFeature( _opYCC601, _TRUE);
                    }
                      if((pucDdcRamAddr[usDbStartAddr + 3] & 0x80) == _BIT7)
                    {
                        ScalerHdmiMacTx0EdidSetFeature( _ST2113RGB23, _TRUE);
                    }

                    if((pucDdcRamAddr[usDbStartAddr + 3] & 0x40) == _BIT6)
                    {
                        ScalerHdmiMacTx0EdidSetFeature( _ICTCP, _TRUE);
                    }

                    break;

                case _CTA_SHDR_DB:
                    if((pucDdcRamAddr[usDbStartAddr + 2] & 0x4) == _BIT2)
                    {
                        ScalerHdmiMacTx0EdidSetFeature( _HDR_SMPTE_ST2084, _TRUE);
                    }
                    if((pucDdcRamAddr[usDbStartAddr + 2] & 0x8) == _BIT3)
                    {
                        ScalerHdmiMacTx0EdidSetFeature( _HDR_HLG, _TRUE);
                    }
                    break;

                case _CTA_DHDR_DB:
                    break;

                case _CTA_FS_DB:

                    if(ucEDIDBlockBytesNum >= 5)
                    {
                        // Support Free Sync mode
                        if((pucDdcRamAddr[usDbStartAddr + 5] & _BIT0) == _BIT0)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DRR_SUPPORT, _TRUE);
                        }
                    }
                    DebugMessageHDMITx("[HDMITX] FS_DB, _DRR_SUPPORT=%d\n", (UINT32)ScalerHdmiMacTx0EdidGetFeature(_DRR_SUPPORT));
                    break;

                case _CTA_HDMI_VSDB:
                    if(ucEDIDBlockBytesNum >= 6)
                    {
                        UINT8 ucSrcPhyAddr;

                        // Components of Source Physical Address
                        ucSrcPhyAddr = *((UINT8*)&pucDdcRamAddr[usDbStartAddr + 4]);
                        ScalerHdmiMacTx0EdidSetFeature( _SRC_PHYSICAL_ADDR,  (ucSrcPhyAddr & ~_BIT0));

                        if((pucDdcRamAddr[(usDbStartAddr + 6)] & _BIT7) == _BIT7) // BIT7: Support AI
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _AI_SUPPORT, _TRUE);
                        }

                        if((pucDdcRamAddr[(usDbStartAddr + 6)] & _BIT6) == _BIT6) // BIT6: Support DP 48 bits
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DC48_SUPPORT, _TRUE);
                        }

                        if((pucDdcRamAddr[(usDbStartAddr + 6)] & _BIT5) == _BIT5) // BIT5: Support DP 36 bits
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DC36_SUPPORT, _TRUE);
                        }

                        if((pucDdcRamAddr[(usDbStartAddr + 6)] & _BIT4) == _BIT4) // BIT4: Support DP 30 bits
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DC30_SUPPORT, _TRUE);
                        }

                        if((pucDdcRamAddr[(usDbStartAddr + 6)] & _BIT3) == _BIT3) // BIT3: Support DC_Y444
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _YCC444_SUPPORT, _TRUE);
                            ScalerHdmiMacTx0EdidSetFeature( _DC_Y444_SUPPORT, _TRUE);
                        }
                    }

                    if(ucEDIDBlockBytesNum >= 7)
                    {
                        ucHdmiVsdbMaxTMDSClk = (pucDdcRamAddr[(usDbStartAddr + 7)]);
                    }
                    if(ucEDIDBlockBytesNum >= 8)
                    {
                        if((pucDdcRamAddr[(usDbStartAddr + 8)] & _BIT7) == _BIT7) // BIT7: latenct present
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _LATENCY_FIELD_PRESENT, _TRUE);
                        }
                        if((pucDdcRamAddr[(usDbStartAddr + 8)] & _BIT6) == _BIT6) // BIT6: ilatenct present
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _ILATENCY_FIELD_PRESENT, _TRUE);
                        }
                        if((pucDdcRamAddr[(usDbStartAddr + 8)] & _BIT5) == _BIT5) // BIT5: hdmi video present
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _HDMI_VIDEO_PRESENT, _TRUE);
                        }
                    }
                    break;

                case _CTA_HF_EEODB:
                    // HF-EEODB (_EDID_EXT_BLOCK_COUNT): 0x84~0x86
                    DebugMessageHDMITx("[HDMITX]2 EEODB Byte[2:0]=%x|", (UINT32)pucDdcRamAddr[(usDbStartAddr+2)]);
                    DebugMessageHDMITx("%x|", (UINT32)pucDdcRamAddr[(usDbStartAddr+1)]);
                    DebugMessageHDMITx("%x], ", (UINT32)pucDdcRamAddr[(usDbStartAddr)]);
                    DebugMessageHDMITx("Len=%x\n", (UINT32)ucEDIDBlockBytesNum);
                    if(ucEDIDBlockBytesNum > 0)
                    {
                         if(pucDdcRamAddr[(usDbStartAddr+1)] == 0x78)// Byte[0x85]=0x78
                        {
                            value_u8 = pucDdcRamAddr[(usDbStartAddr+2)];
                            ScalerHdmiMacTx0EdidSetFeature( _EDID_EXT_BLOCK_COUNT, value_u8);
                            InfoMessageHDMITx("[HDMITX] EEODB CNT=%d\n", (UINT32)ScalerHdmiMacTx0EdidGetFeature(_EDID_EXT_BLOCK_COUNT));
                        }
                     }
                    break;

                case _CTA_HF_VSDB:
                case _CTA_HF_SCDB:

                    DebugMessageHDMITx("[HDMITX] VSDB/SCDB ByteNum=%d\n", (UINT32)ucEDIDBlockBytesNum);
                    if(ucEDIDBlockBytesNum >= 6)
                    {
                        if(((pucDdcRamAddr[(usDbStartAddr + 6)] & _BIT7) >> 7) == 1)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _SCDC_PRESENT, _TRUE);
                        }

                        if((pucDdcRamAddr[(usDbStartAddr + 6)] & _BIT6) >> 6)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _RR_CAPABLE, _TRUE);
                        }

                        if((pucDdcRamAddr[(usDbStartAddr + 6)] & _BIT3) >> 3)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _LTE_340MCSC_SCRAMBLE, _TRUE);
                        }
                         if((pucDdcRamAddr[(usDbStartAddr + 6)] & _BIT2) >> 2)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _Independent_view, _TRUE);
                        }
                         if((pucDdcRamAddr[(usDbStartAddr + 6)] & _BIT1) >> 1)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _Dual_View, _TRUE);
                        }
                         if((pucDdcRamAddr[(usDbStartAddr + 6)] & _BIT0))
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _3D_OSD_Disparity, _TRUE);
                        }
                    }

                    if(ucEDIDBlockBytesNum >= 7)
                    {
                        if((pucDdcRamAddr[(usDbStartAddr + 7)] & _BIT2) == _BIT2)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DC48_420_SUPPORT, _TRUE);
                            ScalerHdmiMacTx0EdidSetFeature( _YCC420_SUPPORT, _TRUE);
                        }

                        if((pucDdcRamAddr[(usDbStartAddr + 7)] & _BIT1) == _BIT1)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DC36_420_SUPPORT, _TRUE);
                            ScalerHdmiMacTx0EdidSetFeature( _YCC420_SUPPORT, _TRUE);
                        }

                        if((pucDdcRamAddr[(usDbStartAddr + 7)] & _BIT0) == _BIT0)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DC30_420_SUPPORT, _TRUE);
                            ScalerHdmiMacTx0EdidSetFeature( _YCC420_SUPPORT, _TRUE);
                        }

#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
                        //NoteMessageHDMITx("[HDMITX] CTA_HF_VSDB %d Byte7=%x",(UINT32) usDbStartAddr,(UINT32)pucDdcRamAddr[(usDbStartAddr + 7)]);
                        //NoteMessageHDMITx(" YCC420=%d\n",(UINT32)ScalerHdmiMacTxEdidGetFeature(enumOutputPort, _YCC420_SUPPORT));
#endif

                        if(((pucDdcRamAddr[(usDbStartAddr + 7)] & (_BIT7 | _BIT6 | _BIT5 | _BIT4)) >> 4) > 0x00)
                        {
                            value_u8 = (g_stHdmiMacTx0EDIDInfo.b1SCDC_Present? ((pucDdcRamAddr[(usDbStartAddr + 7)] & (_BIT7 | _BIT6 | _BIT5 | _BIT4)) >> 4): 0);
                            DebugMessageHDMITx("[HDMITX] VSDB _MAX_FRL_RATE=%d\n", (UINT32)value_u8);
                            ScalerHdmiMacTx0EdidSetFeature( _MAX_FRL_RATE, value_u8);
                        }

                        ucHfVsdbMaxTMDSClk = (pucDdcRamAddr[(usDbStartAddr + 5)]);
                    }
                    if(ucEDIDBlockBytesNum >= 8)
                    {
                        if(((pucDdcRamAddr[(usDbStartAddr + 8)] & _BIT7) >> 7) == 1)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _FAPA_End_Extended, _TRUE);
                        }

                        if((pucDdcRamAddr[(usDbStartAddr + 8)] & _BIT6) >> 6)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _QMS, _TRUE);
                        }
                        if((pucDdcRamAddr[(usDbStartAddr + 8)] & _BIT5) >> 5)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _MDelta, _TRUE);
                        }
                        if((pucDdcRamAddr[(usDbStartAddr + 8)] & _BIT4) >> 4)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _CINEMAVRR, _TRUE);
                        }
                        if((pucDdcRamAddr[(usDbStartAddr + 8)] & _BIT3) >> 3)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _NEG_MVRR, _TRUE);
                        }
                        if((pucDdcRamAddr[(usDbStartAddr + 8)] & _BIT2) >> 2)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _FVA, _TRUE);
                        }
                    }
                    if(ucEDIDBlockBytesNum >= 10)
                    {
                        BYTE vrr_min, vrr_max_b9_8, vrr_max_b7_0;

                        vrr_min = pucDdcRamAddr[(usDbStartAddr + 9)] & 0x3f;
                        vrr_max_b9_8 = ((pucDdcRamAddr[(usDbStartAddr + 9)] & ( _BIT7 | _BIT6)) >> 6);
                        vrr_max_b7_0 = pucDdcRamAddr[(usDbStartAddr + 10)];

                        ScalerHdmiMacTx0EdidSetFeature( _VRR_MIN, vrr_min);
                        ScalerHdmiMacTx0EdidSetFeature( _VRR_MAX, (UINT16)(vrr_max_b9_8 << 8)|vrr_max_b7_0);

                        InfoMessageHDMITx("[TX]VRR %d/%d\n", (UINT32)vrr_min, (UINT32)((UINT16)(vrr_max_b9_8 << 8)|vrr_max_b7_0));
                    }

                    if(ucEDIDBlockBytesNum >= 11)
                    {

                        if((pucDdcRamAddr[(usDbStartAddr + 11)] & _BIT0) == _BIT0)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DSC_10BPC, _TRUE);
                        }

                        if((pucDdcRamAddr[(usDbStartAddr + 11)] & _BIT1) == _BIT1)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DSC_12BPC, _TRUE);
                        }
                        if((pucDdcRamAddr[(usDbStartAddr + 11)] & _BIT2) == _BIT2)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DSC_16BPC, _TRUE);
                        }
                        if((pucDdcRamAddr[(usDbStartAddr + 11)] & _BIT3) == _BIT3)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DSC_ALL_BPP, _TRUE);
                        }

                        if((pucDdcRamAddr[(usDbStartAddr + 11)] & _BIT6) == _BIT6)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DSC_NATIVE_420, _TRUE);
                        }
                        if((pucDdcRamAddr[(usDbStartAddr + 11)] & _BIT7) == _BIT7)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DSC_VERSION_12, _TRUE);
                        }
                    }

                    if(ucEDIDBlockBytesNum >= 12)
                    {

                        if(((pucDdcRamAddr[(usDbStartAddr + 12)] & (_BIT3 | _BIT2 | _BIT1 | _BIT0))) > 0x00)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DSC_MAX_SLICES, (pucDdcRamAddr[(usDbStartAddr + 12)] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
                        }

                        if(((pucDdcRamAddr[(usDbStartAddr + 12)] & (_BIT7 | _BIT6 | _BIT5 | _BIT4)) >> 4) > 0x00)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DSC_MAX_FRL_RATE, ((pucDdcRamAddr[(usDbStartAddr + 12)] & (_BIT7 | _BIT6 | _BIT5 | _BIT4)) >> 4));
                        }
                    }

                    if(ucEDIDBlockBytesNum >= 13)
                    {

                        if(((pucDdcRamAddr[(usDbStartAddr + 13)] & (_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0))) > 0x00)
                        {
                            ScalerHdmiMacTx0EdidSetFeature( _DSC_TOTAL_CHUNKKBYTE, (pucDdcRamAddr[(usDbStartAddr + 13)] & (_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0)));
                        }
                    }
                    break;

                default:
                    break;
            }
        }
    }


    // *****************************************************
    // Part2: Anasyis Edid from 2 above Data Block
    // *****************************************************
    // Detect EDID Interface Format
    ScalerHdmiMacTxCheckSinkEdidFormat(enumOutputPort, pusCtaDataBlockAddr);

    // Detect Hdmi/Dvi Max Tmds Clk

    ScalerHdmiMacTxSetSinkTmdsSupportClk(enumOutputPort, ucHdmiVsdbMaxTMDSClk, ucHfVsdbMaxTMDSClk);
}

//--------------------------------------------------
// Description  : Hdmi Tx Read & Reload Downstream Edid
// Input Value  :
// Output Value :
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0ReloadEdid(void)
{

    int ucRetryTimes = READ_EDID_FAIL_RETRY_COUNT;
    UINT8 ucReadType = _EDID_ORIGIN_READ;

    // Read EDID 3 times at most
    for(ucRetryTimes = READ_EDID_FAIL_RETRY_COUNT; ucRetryTimes > 0; ucRetryTimes--)
    {
        // Check Sink Capability for Segment Read,_HDCP_ADDRESS

        subEdidAddr[0] = 0x00;
        if(ScalerMcuHwIICSegmentRead(0, HDMI_TX_I2C_BUSID, _EDID_ADDRESS, subEdidAddr,0x01,&ScalerOutputGetPxPortDdcRamAddr(ScalerHdmiMacTxPxMapping(_TX0))[0], 0x02, 0x01, _HDMI_TX0_IIC) == _SUCCESS)
        {
            ucReadType = _EDID_SEGMENT_READ;
        }

        ScalerHdmiMacTx0EdidSetFeature(_EDID_SEG_READ_SUPPORT, ucReadType);

        if(ScalerHdmiMacTx0EdidIICRead(ucReadType) == _TRUE)
        {
            return _TRUE;
        }else{
            FatalMessageHDMITx("[HDMI_TX] Retry Edid IICRead %d \n",(UINT32)ucRetryTimes);
            mdelay(20);
        }
    }

    return _FALSE;
}
//--------------------------------------------------
// Description  : Find edid ExtensionTag Block for modify DTD timing
// Input Value  :
// Output Value :
//--------------------------------------------------
void FindExtensionTagBlock(void){
    UINT16 usEDIDExtBase= 0x7E;
    UINT8 ucEdidExtBlockNum;

    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacPxTxMapping(_TX0));
    usCtaExtBasicSupportAddr = EDID_BLOCK1_BASIC_SUPPORT_ADDR;
    usExtTagBaseAddr = EDID_BLOCK1_TAG_BASE;
    ucEdidExtBlockNum = pucDdcRamAddr[usEDIDExtBase];
    if(((ucEdidExtBlockNum == 2) || (ucEdidExtBlockNum == 3)) && (pucDdcRamAddr[EDID_BLOCK1_TAG_BASE] == EDID_BLOCK_MAP_TAG)){
        if(pucDdcRamAddr[EDID_BLOCK2_TAG_BASE] == EDID_EXTENSION_TAG){
            usCtaExtBasicSupportAddr = EDID_BLOCK2_BASIC_SUPPORT_ADDR;
            usExtTagBaseAddr = EDID_BLOCK2_TAG_BASE;
        }
        else if((ucEdidExtBlockNum == 3) && (pucDdcRamAddr[EDID_BLOCK3_TAG_BASE] == EDID_EXTENSION_TAG)){
            usCtaExtBasicSupportAddr = EDID_BLOCK3_BASIC_SUPPORT_ADDR;
            usExtTagBaseAddr = EDID_BLOCK3_TAG_BASE;
        }
    }
}


//--------------------------------------------------
// Description  : Get HDMI Downstream EDID
// Input Value  :
// Output Value :
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0GetDSEDID(void)
{

        BOOLEAN ret = ScalerHdmiMacTx0ReloadEdid();
       if(ret == _TRUE)
        {
            SET_HDMI_MAC_TX0_EDID_MODIFY_STATE(EDID_CHECK_BF_MODIFY);
  #ifdef CONFIG_HDMITX_REPEATER_SUPPORT // EDID support timing reset
            FindExtensionTagBlock();
            ScalerHdmiMacTx0RepeaterClrSinkTxSupportTimingCount();
  #endif
            ScalerHdmiMacTx0EdidAnalysis();
            ScalerHdmiMacTx0EdidModifyEnable(_ENABLE, _INITIAL_MODIFY);
            ScalerHdmiMacTx0PlugNotify();
        }
        return ret;
}

//--------------------------------------------------
// Description  : Hdmi Tx periodic Check EDID checksum
// Input Value  :
// Output Value : _TRUE / _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0CheckEdidDifference(void)
{
#if 0
    UINT8 ucI = 0;
    UINT8 ucJ = 0;
    UINT8 ucEdidBlock = 0;
    UINT8 pucChecksum[4] = {0};  // [0] base checksum, [1] Extersion 1 checksum ...
    // Read Realtime EDID Checksum for SegRead or Original Mode
    if(ScalerHdmiMacTx0EdidGetFeature(_EDID_SEG_READ_SUPPORT) == _EDID_SEGMENT_READ)
    {
        // Try to Read Edid 3 times
        for(ucI = 0; ucI < 3; ucI++)
        {
            UINT8 ucSegmentPoint = 0x00;
            subEdidAddr[0] = 0x7E;
            if(ScalerMcuHwIICRead(_EDID_ADDRESS, subEdidAddr, 0x01, &ucEdidBlock,0x01) == _TRUE)
            {
                for(ucJ = 0; ucJ <= ucEdidBlock; ucJ++)
                {
                    subEdidAddr[0] = ((0x80 * ucJ) + 0x7F);
                    if(ScalerMcuHwIICSegmentRead(ucSegmentPoint,  _EDID_ADDRESS, subEdidAddr ,0x01, &pucChecksum[ucJ],0x01) == _FAIL)
                    {
                        DebugMessageHDMITx("[HDMI_TX] EDID Read Checksum Fail\n");
                        break;
                    }

                    if(ucJ % 2 != 0)
                    {
                        ucSegmentPoint++;
                    }
                }

                break;
            }

            if(ucI == 2)
            {
                DebugMessageHDMITx("[HDMI_TX] EDID Read Block# Fail\n");
                return _FALSE;
            }
        }
    }
    else
    {
        // Try to Read Edid 3 times
        for(ucI = 0; ucI < 3; ucI++)
        {
            subEdidAddr[0] = 0x7E;
            if(ScalerMcuHwIICRead(_EDID_ADDRESS,  subEdidAddr, 0x01, &ucEdidBlock, 0x01) == _TRUE)
            {
                if(ucEdidBlock > 1)
                {
                    return _FALSE;
                }

                for(ucJ = 0; ucJ <= ucEdidBlock; ucJ++)
                {
                    subEdidAddr[0] = ucJ * 0x80 + 0x7F;
                    if(ScalerMcuHwIICRead(_EDID_ADDRESS,subEdidAddr,  0x01, &pucChecksum[ucJ],0x01) == _FALSE)
                    {
                        DebugMessageHDMITx("[HDMI_TX] EDID Read Checksum Fail\n");
                        break;
                    }
                }

                break;
            }

            if(ucI == 2)
            {
                DebugMessageHDMITx("[HDMI_TX] EDID Read Block# Fail\n");
                return _FALSE;
            }
        }
    }

    // Compare ext EDID block num is the same
    if(ScalerOutputGetPxPortDdcRamAddr(EdidMacPxTxMapping(_TX0))[0x7E] != ucEdidBlock)
    {
        DebugMessageHDMITx("[HDMI_TX] EDID Changed\n");
        return _TRUE;
    }
    else //Compare Checksum (for Edid Base + CTA Extersion)
    {
        for(pEdidData[0] = 0; pEdidData[0] <= ucEdidBlock; pEdidData[0]++)
        {
            if(pEdidData[0] > ucEdidBlock)
            {
                break;
            }
            if(g_pucHdmiMacTx0EdidChecksum[pEdidData[0]] != pucChecksum[pEdidData[0]])
            {
                DebugMessageHDMITx("[HDMI_TX] EDID Changed\n");
                return _TRUE;
            }
        }

        return _FALSE;
    }
#endif
    return _FALSE;
}
//--------------------------------------------------
// Description  : Hdmi Edid Modify Issue Detect for Initial Modify
// Input Value  :
// Output Value :
//--------------------------------------------------
extern UINT8 pucEdidVDBVIC[31];
//static INT8 mntName[2][11-5];

void ScalerHdmiMacTx0SpecialSinkEdidDetect(void)
{
#if(_HDMI_TX_CTS_TEST == _ON)
    UINT16 ucDTDAddress= 0x36;
    UINT8 ucDTDNum= 0x00;
    UINT16 usDTDPixelClk= 0x00;
    UINT16 usDTDHactive= 0x00;
    UINT16 usDTDVactive= 0x00;
    UINT8 ucDTDNumis1080i= 0x00;
#endif
    memset(pucEdidVDBVIC, 0x00, 31);
    memset(pusCtaDataBlockAddr, 0x00, 17);
    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacPxTxMapping(_TX0));

    // Distinguish special MNT(Acer H277HU)
    pEdidData[0] = 0;

    while(pEdidData[0] < 28)
    {
        if(pucDdcRamAddr[(UINT16)pucSpecialTv4EDIDCheckAddr[pEdidData[0]]] != pucSpecialTv4EDIDCheckValue[pEdidData[0]])
        {
            break;
        }

        pEdidData[0] ++;

        if(pEdidData[0] == 28)
        {
            SET_HDMI_MAC_TX0_FIND_SPECIAL_TV4();
        }
    }


    // Support HDMITX 8K output EDID : Monitor Name  on Descriptor 3, 4
    if((memcmp(&pucDdcRamAddr[0x6c], pucSpecialTvEDIDCheckValue, 10) == 0x00)|| (memcmp(&pucDdcRamAddr[0x5a], pucSpecialTvEDIDCheckValue, 10) == 0x00)){
        SET_HDMI_MAC_TX0_FIND_SPECIAL_TV();
        DebugMessageHDMITx("[HDMI_TX] EDID FIND TV1\n");
    }else{
        CLR_HDMI_MAC_TX0_FIND_SPECIAL_TV();
    }


    //Support HDMITX 8K output EDID : Monitor Name  on Descriptor 3, 4
    if((memcmp(&pucDdcRamAddr[0x6c], pucSpecialTv2EDIDCheckValue, 8) == 0x00)|| (memcmp(&pucDdcRamAddr[0x5a], pucSpecialTv2EDIDCheckValue, 8) == 0x00)){
        SET_HDMI_MAC_TX0_FIND_SPECIAL_TV2();
        DebugMessageHDMITx("[HDMI_TX] EDID FIND TV2\n");
    }else{
        CLR_HDMI_MAC_TX0_FIND_SPECIAL_TV2();
    }


    if((memcmp(&pucDdcRamAddr[0x6c], pucSpecialTv3EDIDCheckValue, 16) == 0x00)|| (memcmp(&pucDdcRamAddr[0x5a], pucSpecialTv3EDIDCheckValue, 16) == 0x00)){
        SET_HDMI_MAC_TX0_FIND_SPECIAL_TV3();
        DebugMessageHDMITx("[HDMI_TX] EDID FIND  TV3\n");
    }else{
        CLR_HDMI_MAC_TX0_FIND_SPECIAL_TV3();
    }

    if((memcmp(&pucDdcRamAddr[0x6c], pucSpecialTv5EDIDCheckValue, 18) == 0x00)|| (memcmp(&pucDdcRamAddr[0x5a], pucSpecialTv5EDIDCheckValue, 18) == 0x00)){
        SET_HDMI_MAC_TX0_FIND_SPECIAL_TV5();
        FatalMessageHDMITx("[HDMI_TX] EDID FIND  TV5\n");
    }else{
        CLR_HDMI_MAC_TX0_FIND_SPECIAL_TV5();
    }
    if((memcmp(&pucDdcRamAddr[0x08], pucSpecialTv6EDIDCheckValue, 2) == 0x00)){
        SET_HDMI_MAC_TX0_FIND_SPECIAL_TV6();
        DebugMessageHDMITx("[HDMI_TX] EDID FIND TV6\n");
    }else{
        CLR_HDMI_MAC_TX0_FIND_SPECIAL_TV6();
    }
#if 0 // TEST: Modify VG28UQL1A: 56 47 32 38 55 51 4C 31 41 0A
    if(memcmp(&pucDdcRamAddr[0x6c], pucVg28uqlEDIDCheckValue, 10) == 0x00){
        pucDdcRamAddr[0x6c+13] = 0x44; // A->B
        FatalMessageHDMITx("[HDMI_TX] EDID FIND VG28UQL");
        FatalMessageHDMITx("%d", (UINT32)pucDdcRamAddr[0x6c+12]);
        FatalMessageHDMITx("%d\n", (UINT32)pucDdcRamAddr[0x6c+13]);

        ScalerEdidCheckSumCal(pucDdcRamAddr, 0x00);
        pucDdcRamAddr[0x7F] = g_ucOutputTxInternalEDIDChecksumFixed;
    }
#endif

    #if 0
    // check monitor name
    if(!GET_HDMI_MAC_TX0_FIND_SPECIAL_TV() && !GET_HDMI_MAC_TX0_FIND_SPECIAL_TV2()){
        memcpy(&mntName[0], &pucDdcRamAddr[0x5a+5], 11-5);
        memcpy(&mntName[1], &pucDdcRamAddr[0x6c+5], 11-5);

        NoteMessageHDMITx("[HDMITX][EDID] Name[0][%s]=%x %x ",(UINT32)mntName[0], (UINT32)pucDdcRamAddr[0x5a+0]);
        NoteMessageHDMITx(" %x %x",(UINT32)pucDdcRamAddr[0x5a+1],(UINT32)pucDdcRamAddr[0x5a+2]);
        NoteMessageHDMITx(" %x %x",(UINT32)pucDdcRamAddr[0x5a+3], (UINT32)pucDdcRamAddr[0x6c+4]);
        NoteMessageHDMITx(" %x %x",(UINT32)pucDdcRamAddr[0x5a+5],(UINT32)pucDdcRamAddr[0x5a+6]);
        NoteMessageHDMITx(" %x %x",(UINT32)pucDdcRamAddr[0x5a+7], (UINT32)pucDdcRamAddr[0x6c+8]);
        NoteMessageHDMITx(" %x %x\n",(UINT32)pucDdcRamAddr[0x5a+9], (UINT32)pucDdcRamAddr[0x5a+10]);
        NoteMessageHDMITx("[HDMITX][EDID] Name[1][%s]=%x %x",(UINT32)mntName[1], (UINT32)pucDdcRamAddr[0x6c+0]);
        NoteMessageHDMITx(" %x %x",(UINT32)pucDdcRamAddr[0x6c+1],(UINT32)pucDdcRamAddr[0x6c+2]);
        NoteMessageHDMITx(" %x %x",(UINT32)pucDdcRamAddr[0x6c+3],(UINT32)pucDdcRamAddr[0x6c+4]);
        NoteMessageHDMITx(" %x %x",(UINT32)pucDdcRamAddr[0x6c+5],(UINT32)pucDdcRamAddr[0x6c+6]);
        NoteMessageHDMITx(" %x %x",(UINT32)pucDdcRamAddr[0x6c+7],(UINT32)pucDdcRamAddr[0x6c+8]);
        NoteMessageHDMITx("%x %x\n",(UINT32)pucDdcRamAddr[0x6c+9], (UINT32)pucDdcRamAddr[0x6c+10]);

    }
    #endif
#if 0
    // [H5X] check VSD 8K video support
    ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
    ScalerHdmiMacTxGetEdidVicTable(_P0_OUTPUT_PORT, _CTA_VDB, &pucEdidVDBVIC[0], pusCtaDataBlockAddr);
    for(u8TmpValue=0; u8TmpValue<31; u8TmpValue++){
        DebugMessageHDMITx("[HDMI_TX] VSD VIC[%d]=%d\n", (UINT32)u8TmpValue, (UINT32)pucEdidVDBVIC[u8TmpValue]);
        if((pucEdidVDBVIC[u8TmpValue] >= 194) && (pucEdidVDBVIC[u8TmpValue] <= 199)){ // 8K support
            bvsd8kSupport = 1;
            DebugMessageHDMITx("[HDMI_TX][Support 8K] VSD VIC[%d]=%d\n", (UINT32)u8TmpValue, (UINT32)pucEdidVDBVIC[u8TmpValue]);
            break;
        }
    }
    ScalerHdmiMacTx0EdidSetFeature(_P0_OUTPUT_PORT, _VSD_8K_SUPPORT,  bvsd8kSupport);
#endif
#if(_HDMI_TX_CTS_TEST == _ON)
    // Distinguish General CTS test EDID : Display Product Name = HDMI
    EDID_CLR_HDMI_MAC_TX0_FIND_CTS_TEST_EDID();

    if(memcmp(&pucDdcRamAddr[0x5A], pucCTSIntMNTEDIDCheckValue, 10) == 0x00)
    {
        EDID_SET_HDMI_MAC_TX0_FIND_CTS_TEST_EDID();
    }

#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)
    // Distinguish CTS test EDID : Display Product Name = HDMI TPA
    if(memcmp(&pucDdcRamAddr[0x5A], pucCTS4kMNTEDIDCheckValue, 14) == 0x00)
    {
        SET_HDMI_MAC_TX0_FIND_4K_CTS_TEST_EDID();
    }
#endif

    // Distinguish CTS test EDID : Display Product Name = HDMI LLC
    if(memcmp(&pucDdcRamAddr[0x5A], pucCTSIntMNTEDIDCheckValue, 14) == 0x00)
    {
        SET_HDMI_MAC_TX0_FIND_AUDIO_CTS_TEST_EDID();

        for(ucDTDNum = 0; ucDTDNum < 4; ucDTDNum++)
        {
            usDTDPixelClk = (pucDdcRamAddr[ucDTDAddress + 1] << 8) | (pucDdcRamAddr[ucDTDAddress]);

            // the Descriptor is DTD and Pixel Clk <= 74.25MHz
            if((usDTDPixelClk != 0) && (usDTDPixelClk <= 7425))
            {
                usDTDHactive = pucDdcRamAddr[ucDTDAddress + 4];
                usDTDHactive = usDTDHactive  & 0xF0;
                usDTDHactive = usDTDHactive * 16;//<< 4;
                usDTDHactive = usDTDHactive  | (pucDdcRamAddr[ucDTDAddress + 2]);
                usDTDVactive = pucDdcRamAddr[ucDTDAddress + 7];
                usDTDVactive = usDTDVactive & 0xF0;
                usDTDVactive = usDTDVactive * 16;//<< 4;
                usDTDVactive = usDTDVactive | (pucDdcRamAddr[ucDTDAddress + 5]);
                if((usDTDHactive == 1920) && (usDTDVactive == 540))
                {
                    ucDTDNumis1080i |= (_BIT0 << ucDTDNum);
                }
            }
            ucDTDAddress = ucDTDAddress + 18;
        }
        SET_HDMI_MAC_TX0_DTD_NUM_NEED_REPLACED(ucDTDNumis1080i);
    }
#endif // #if(_HDMI_TX_CTS_TEST == _ON)
}


//--------------------------------------------------
// Description  : Edid Set Tmds Max Clk
// Input Value  : enumOutputPort, enumEdidFeature
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0SetSinkTmdsSupportClk(UINT8 ucHdmiVsdbMaxTMDSClk, UINT8 ucHfVsdbMaxTMDSClk)
{
    if((ucHfVsdbMaxTMDSClk != 0) || (ucHdmiVsdbMaxTMDSClk != 0))
    {
        if(ucHdmiVsdbMaxTMDSClk > ucHfVsdbMaxTMDSClk)
        {
            ScalerHdmiMacTx0EdidSetFeature(_HDMI_MAX_TMDS_CLK, ucHdmiVsdbMaxTMDSClk);
        }
        else
        {
            ScalerHdmiMacTx0EdidSetFeature(_HDMI_MAX_TMDS_CLK, ucHfVsdbMaxTMDSClk);
        }
    }
    else
    {
        if(GET_HDMI_MAC_TX0_SUPPORT() == _TRUE)
        {
            ScalerHdmiMacTx0EdidSetFeature(_HDMI_MAX_TMDS_CLK, 60);
        }
        else
        {
            ScalerHdmiMacTx0EdidSetFeature(_HDMI_MAX_TMDS_CLK, 33);
        }
    }
}


//--------------------------------------------------
// Description  : HDMI Tx Set Modify Event
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0EdidModifyEnable(BOOLEAN bEnable, EnumHdmiEdidModifyEvent enumEdidModifyEvent)
{
    if(bEnable == _ENABLE)
    {
        SET_HDMI_MAC_TX0_MODIFY_EDID_ENABLE();
        SET_HDMI_MAC_TX0_EDID_MODIFY_EVENT(enumEdidModifyEvent);
    }
    else
    {
        CLR_HDMI_MAC_TX0_MODIFY_EDID_ENABLE();
        SET_HDMI_MAC_TX0_EDID_MODIFY_EVENT(_NO_EDID_MODIFY);
    }
}

//--------------------------------------------------
// Description  : Hdmi Tx EDID Analysis
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0EdidAnalysis(void)
{
    // Initial Edid Info Structure
    memset(&g_stHdmiMacTx0EDIDInfo, 0, sizeof(g_stHdmiMacTx0EDIDInfo));

#if(_HDMI_TX_CTS_TEST == _ON)
    memset(&g_stHdmiMacTx0CtsEdidInfo, 0, sizeof(g_stHdmiMacTx0CtsEdidInfo));
#endif
    ScalerEdidGeneralStructParse(EdidMacTxPxMapping(_TX0));
    ScalerEdidExtBlockDbParse(EdidMacTxPxMapping(_TX0));
    ScalerHdmiMacTx0SpecialSinkEdidDetect();
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
    ScalerHdmiMacTx0RepeaterEdidInfoDump();
#endif
}

//--------------------------------------------------
// Description  : Hdmi Tx Edid Fs Capability Modify
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0EdidFreesyncCapibilityModify(UINT8 *pucDdcRamAddr, UINT16 *pusCtaDataBlockAddr)
{

    DebugMessageHDMITx("[HDMITX] _DRR_SUPPORT=%d@ScalerHdmiMacTx0EdidFreesyncCapibilityModify()\n", (UINT32)ScalerHdmiMacTx0EdidGetFeature(_DRR_SUPPORT));

#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
    // don't change AMD Free Sync support EDID
    if(ScalerHdmiMacTx0RepeaterGetEdidSupport(_DRR_SUPPORT) == _TRUE)
        return;
#endif

    // Freesync Support Bit tie 0
    if(ScalerHdmiMacTx0EdidGetFeature(_DRR_SUPPORT) == _TRUE)
    {
        UINT16 usFreesyncDbStartAddr = pusCtaDataBlockAddr[_CTA_FS_DB];

        // Modify EDID to No Support Free Sync mode
        pucDdcRamAddr[usFreesyncDbStartAddr + 5] = 0x00;

        // Calculate EDID Checksum
        ScalerEdidCheckSumCal(pucDdcRamAddr, 0x80);

        // Modify EDID Checksum
        pucDdcRamAddr[0xFF] = g_ucOutputTxInternalEDIDChecksumFixed;

    }
}

#ifdef NOT_USED_NOW
#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Edid Fs Capability Modify
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0EdidVideoBandwidthModify(UINT8 *pucDdcRamAddr, UINT16 *pusCtaDataBlockAddr, UINT16 *pusDidDataBlockAddr)
{
    UINT8 pucEdidVDBVIC[31] = {0};
    StructHdmi21EdidTimingReference pstEdidTimingReference[(sizeof(t3G_3LANE_EDIDTIMING) / sizeof(StructHdmi21EdidTimingReference))];

    UINT8 ucTargetIndex = 0;
    BOOLEAN bDidReplace = 0;
    BOOLEAN bFinish = _FALSE;

    // Get Vic Index form Sink VDB
    ScalerHdmiMacTx0GetEdidVicTable(_CTA_VDB, &pucEdidVDBVIC[0], pusCtaDataBlockAddr);

    // ---------------------------------------------------------------------------
    // Decide which VIC Timing Pattern can Replace to Sink's EDID
    // ---------------------------------------------------------------------------
    // Mapping Timing Reference

    if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmi21MacTxPxTxMapping(EdidMacPxTxMapping(_TX0))) == _HDMI21_FRL_3G)
    {
        memcpy(pstEdidTimingReference, t3G_3LANE_EDIDTIMING, sizeof(t3G_3LANE_EDIDTIMING));
    }
#if 0
    else if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmi21MacTxPxTxMapping(EdidMacPxTxMapping(_TX0))) == _HDMI21_FRL_3G_4LANES)
    {
        memcpy(pstEdidTimingReference, t3G_4LANE_EDIDTIMING, sizeof(t3G_4LANE_EDIDTIMING));
    }
#endif
    else if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmi21MacTxPxTxMapping(EdidMacPxTxMapping(_TX0))) == _HDMI21_FRL_6G_3LANES)
    {
        memcpy(pstEdidTimingReference, t6G_3LANE_EDIDTIMING, sizeof(t6G_3LANE_EDIDTIMING));
    }
    else if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmi21MacTxPxTxMapping(EdidMacPxTxMapping(_TX0))) == _HDMI21_FRL_6G_4LANES)
    {
        memcpy(pstEdidTimingReference, t6G_4LANE_EDIDTIMING, sizeof(t6G_4LANE_EDIDTIMING));
    }
    else if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmi21MacTxPxTxMapping(EdidMacPxTxMapping(_TX0))) == _HDMI21_FRL_8G)
    {
        memcpy(pstEdidTimingReference, t8G_4LANE_EDIDTIMING, sizeof(t8G_4LANE_EDIDTIMING));
    }
    else if(GET_HDMI21_MAC_TX_FRL_RATE(ScalerHdmi21MacTxPxTxMapping(EdidMacPxTxMapping(_TX0))) == _HDMI21_FRL_10G)
    {
        memcpy(pstEdidTimingReference, t10G_4LANE_EDIDTIMING, sizeof(t10G_4LANE_EDIDTIMING));
    }

    // Compare Ref Vic with Sink's Vic
    for(pEdidData[0] = 0; pEdidData[0] < (sizeof(pstEdidTimingReference) / sizeof(StructHdmi21EdidTimingReference)); pEdidData[0]++)  // check code avaible?
    {
        for(pEdidData[1] = 0; pEdidData[1] < 31; pEdidData[1]++)
        {
            if(pstEdidTimingReference[pEdidData[0]].ucVic == pucEdidVDBVIC[pEdidData[1]])
            {
                ucTargetIndex = pEdidData[0];
                bFinish = _TRUE;
                break;
            }
        }

        if(bFinish == _TRUE)
        {
            break;
        }
    }

    // ---------------------------------------------------------------------------
    // Replace VIC Pattern to Sink's EDID
    // ---------------------------------------------------------------------------
    // Check where to fill, DTD or TimingType1?
    for(pEdidData[0] = 0; pEdidData[0] < sizeof(tVIC_USE_FOR_DID_T1); pEdidData[0]++)
    {
        if(tVIC_USE_FOR_DID_T1[pEdidData[0]] == pstEdidTimingReference[ucTargetIndex].ucVic)
        {
            bDidReplace = 1;
        }
    }

    // Replace Edid Timing Info
    if(bDidReplace == 0)
    {
        // if no Match Any Vic between Reference & Sink Capability -> Set Default DTD into base block
        if(bFinish == _FALSE)
        {
            memcpy(&pucDdcRamAddr[0x36], tDEFAULT_TIMING_PATTERN_DTD, 18);

            // Update Checksum
            ScalerEdidCheckSumCal(pucDdcRamAddr, 0x00);
            pucDdcRamAddr[0x7F] = g_ucOutputTxInternalEDIDChecksumFixed;

            DebugMessageHDMITx("[HDMI_TX] Replace DTD with Default Pattern@A01\n");
            return;
        }

        // Replace with DTD
        memcpy(&pucDdcRamAddr[0x36], pstEdidTimingReference[ucTargetIndex].pucTimingPattern, 18);

        // Update Checksum
        ScalerEdidCheckSumCal(pucDdcRamAddr, 0x00);
        pucDdcRamAddr[0x7F] = g_ucOutputTxInternalEDIDChecksumFixed;

        DebugMessageHDMITx("[HDMI_TX] Replace DTD with VIC=%d\n", (UINT32)pstEdidTimingReference[ucTargetIndex].ucVic);
    }
    else if(ScalerHdmiMacTx0EdidGetFeature(_DID_EXT_SUPPORT) == _TRUE) // Modify DID Ext
    {
        UINT16 usDidSExtStartAddr = ScalerEdidGetDidExtStartAddress(pucDdcRamAddr);
        UINT16 usDidSExtEndAddr = (usDidSExtStartAddr + pucDdcRamAddr[usDidSExtStartAddr + 1] + 4);
        UINT16 usDidPreferTimingAddr = GET_HDMI_MAC_TX0_DID_PRFER_TIMING_ADDR();
        UINT16 usT1Addr = pusDidDataBlockAddr[_DID_T1_DB];

        // if no Match Any Vic between Reference & Sink Capability -> Set Default T1 into DID Ext
        if(bFinish == _FALSE)
        {
            memcpy(&pucDdcRamAddr[usT1Addr + 3], tDEFAULT_TIMING_PATTERN_T1, 20);

            // Update DID Ext Checksum
            ScalerEdidDidCheckSumCal(pucDdcRamAddr, usDidSExtStartAddr, usDidSExtEndAddr);
            pucDdcRamAddr[usDidSExtEndAddr] = g_ucOutputTxInternalEDIDChecksumFixed;

            // Update EDID Ext Checksum
            ScalerEdidCheckSumCal(pucDdcRamAddr, usDidSExtStartAddr - 1);
            pucDdcRamAddr[usDidSExtStartAddr - 0x01 + 0x7F] = g_ucOutputTxInternalEDIDChecksumFixed;

            DebugMessageHDMITx("[HDMI_TX] Replace T1 with Default Pattern@A02\n");
            return;
        }

        // Replace with TimingType1
        if(usDidPreferTimingAddr != 0x00)
        {
            memcpy(&pucDdcRamAddr[usDidPreferTimingAddr], pstEdidTimingReference[ucTargetIndex].pucTimingPattern, 20);
        }
        else
        {
            memcpy(&pucDdcRamAddr[usT1Addr + 3], pstEdidTimingReference[ucTargetIndex].pucTimingPattern, 20);
        }

        // Update DID Ext Checksum
        ScalerEdidDidCheckSumCal(pucDdcRamAddr, usDidSExtStartAddr, usDidSExtEndAddr);
        pucDdcRamAddr[usDidSExtEndAddr] = g_ucOutputTxInternalEDIDChecksumFixed;

        // Update EDID Ext Checksum
        ScalerEdidCheckSumCal(pucDdcRamAddr, usDidSExtStartAddr - 1);
        pucDdcRamAddr[usDidSExtStartAddr - 0x01 + 0x7F] = g_ucOutputTxInternalEDIDChecksumFixed;

        DebugMessageHDMITx("[HDMI_TX] Replace T1 with VIC=%d\n", (UINT32)pstEdidTimingReference[ucTargetIndex].ucVic);
    }
    else // no DID Ext can mod -> Modify DEFAULT to DTD
    {
        memcpy(&pucDdcRamAddr[0x36], tDEFAULT_TIMING_PATTERN_DTD, 18);

        // Update Checksum
        ScalerEdidCheckSumCal(pucDdcRamAddr, 0x00);
        pucDdcRamAddr[0x7F] = g_ucOutputTxInternalEDIDChecksumFixed;

        DebugMessageHDMITx("[HDMI_TX] Replace DTD with Default Pattern (no DID Ext can Mod)@A03\n");
    }
}
#endif




//--------------------------------------------------
// Description  : Hdmi Tx Edid Special Case Modify Proc
// Input Value  :
// Output Value :
//--------------------------------------------------

static UINT16 usHActive = 0;
static UINT16 usPixelClk = 0;
static UINT16 usHdmiVsdbStartAddr = 0;
static UINT16 usHfVsdbStartAddr = 0;
static UINT16 us420VideoDbStartAddr = 0;
UINT16 calcHActive(void)
{
    UINT16 usHActive = 0;

    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacPxTxMapping(_TX0));

    usHActive = (UINT16)pucDdcRamAddr[0x3a];
    usHActive = usHActive & 0xF0;
    usHActive = usHActive * 16;//<< 4;
    usHActive = usHActive | pucDdcRamAddr[0x38];


    return usHActive;
}
UINT16 calcPixelClk(void)
{
    UINT16 usPixelClk = 0;

    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacPxTxMapping(_TX0));

    usPixelClk = (UINT16)pucDdcRamAddr[0x37];
    usPixelClk = usHActive * 256;//<< 8;
    usPixelClk = usHActive | pucDdcRamAddr[0x36];
    return usPixelClk;
}


void ScalerHdmiMacTx0SpecialSinkEdidModify(UINT16 *pusCtaDataBlockAddr)
{
    UINT16 u16TmpValue = 0;
    UINT8 u8TmpValue = 0;
    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacPxTxMapping(_TX0));
    // TMDS clock >= 480MHz, color depth must be <= 8bit or bandwidth may not enough
    if(ScalerHdmiMacTx0EdidGetFeature(_HDMI_MAX_TMDS_CLK) >= 96)
    {
        // EDID Revision = 1.4
        if(ScalerHdmiMacTx0EdidGetFeature(_EDID_VERSION) == _EDID_VER14)
        {
            // Video Signal Interface = Digital and Color depth > 8bit
            if((pucDdcRamAddr[0x14] & (_BIT7)) == _BIT7)
            {
                u8TmpValue = _BIT6 | _BIT5;
                u8TmpValue = u8TmpValue | _BIT4;
                u8TmpValue = u8TmpValue & pucDdcRamAddr[0x14];
                if(u8TmpValue > _BIT5)
                {
                    // modify color depth = 8bit, MCU_DDCRAM_Px[0x14] = 0x80
                    u8TmpValue = _BIT6 | _BIT5;
                    u8TmpValue = u8TmpValue | _BIT4;
                    u8TmpValue = ~u8TmpValue;
                    u8TmpValue = u8TmpValue & pucDdcRamAddr[0x14];
                    pucDdcRamAddr[0x14] = u8TmpValue;

                    // Calucate EDID Checksum
                    ScalerEdidCheckSumCal(pucDdcRamAddr, 0x00);
                    pucDdcRamAddr[0x7F] = g_ucOutputTxInternalEDIDChecksumFixed;

                    DebugMessageHDMITx("[HDMI_TX] EDID modify color depth = Un-Define@%d\n", (UINT32)g_ucOutputTxInternalEDIDChecksumFixed);
                }
            }
        }
        else
        {
            if(pucDdcRamAddr[0x14] != 0x80)
            {
                pucDdcRamAddr[0x14] = 0x80;

                // Calucate EDID Checksum
                ScalerEdidCheckSumCal(pucDdcRamAddr, 0x00);
                pucDdcRamAddr[0x7F] = g_ucOutputTxInternalEDIDChecksumFixed;

                DebugMessageHDMITx("[HDMI_TX] EDID(1.3) modify@%d\n", (UINT32)g_ucOutputTxInternalEDIDChecksumFixed);
            }
        }
    }

    // if Found Special ACER MNT, need to correct the EDID
    if(GET_HDMI_MAC_TX0_FIND_SPECIAL_TV4() == _TRUE)
    {
        CLR_HDMI_MAC_TX0_FIND_SPECIAL_TV4();

        // disable CEA Externsion gengernal info 444/422 support
        pucDdcRamAddr[0x83] &= 0xCF;

        // modify detailed timing pixel clock -> 198MHz
        pucDdcRamAddr[0xE9] = 0x58;
        pucDdcRamAddr[0xEA] = 0x4D;

        ScalerEdidCheckSumCal(pucDdcRamAddr, 0x80);
        pucDdcRamAddr[0xFF] = g_ucOutputTxInternalEDIDChecksumFixed;

        DebugMessageHDMITx("[HDMI_TX] EDID ACER EDID Found and Modify@A01\n");
    }


    if((pusCtaDataBlockAddr[_CTA_420VDB] != 0x00) && (g_bHdmiMacTx0FindCtsTestEdid == _FALSE))
    {

        usHActive = 0;
        usPixelClk = 0;
        usHdmiVsdbStartAddr = 0;
        usHfVsdbStartAddr = 0;
        us420VideoDbStartAddr = 0;
        // Get Detailed Timing Info
        usHActive = calcHActive();
        usPixelClk = calcPixelClk();
        // EDID Modify When: Clk < 300M, Hactive == 3840 -> 4k Lower than 30Hz
        if((usPixelClk < 0x7530) && (usHActive == 0x0F00))
        {
            if(pusCtaDataBlockAddr[_CTA_HDMI_VSDB] != 0x00)
            {
                usHdmiVsdbStartAddr = pusCtaDataBlockAddr[_CTA_HDMI_VSDB];
                pucDdcRamAddr[usHdmiVsdbStartAddr + 5] = 0x78; // Max_TMDS_Clock=120*5=600M
            }
            else
            {
                DebugMessageHDMITx("[HDMI_TX] EDID No HDMI VSDB Exist!@A02\n");
            }

            if(pusCtaDataBlockAddr[_CTA_HF_VSDB] != 0x00/* || pusCtaDataBlockAddr[_CTA_HF_SCDB] != 0x00*/)
            {
                usHfVsdbStartAddr = pusCtaDataBlockAddr[_CTA_HF_VSDB];
                pucDdcRamAddr[usHfVsdbStartAddr + 5] = 0x78;

                // Calculate DTD Table Counts
                pEdidData[0] = ((0xFF - (UINT8)GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR()) / 0x12);

                // Shift DTD Timing Table
                for(; pEdidData[0] > 0; pEdidData[0]--)
                {
                    u16TmpValue = ((pEdidData[0]) * 0x12);
                    u16TmpValue = u16TmpValue + GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR();
                    // DTD Clear Length = Min_of[0x12, (0xFF - DTD Table_Last_Addr +1]
                    if((0xFF - u16TmpValue) < 0x12)
                    {
                        memset(&pucDdcRamAddr[u16TmpValue], 0, (0xFF - u16TmpValue));
                    }
                    else
                    {
                        memset(&pucDdcRamAddr[u16TmpValue], 0, 0x12);
                    }

                    // if DTD Write End Point Point < 0xFF
                    if((u16TmpValue + 0x12) < 0xFF)
                    {
                        u16TmpValue2 = (pEdidData[0] - 1);
                        u16TmpValue2 = u16TmpValue2* 0x12;
                        u16TmpValue2 = u16TmpValue2 + GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR();
                        memcpy(&pucDdcRamAddr[u16TmpValue], &pucDdcRamAddr[u16TmpValue2], 0x12);
                    }
                }

                // Shift Original Prefer Timing to DTD #1
               memcpy(&pucDdcRamAddr[GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR()], &pucDdcRamAddr[0x36], 0x12);
            }
            else
            {
                DebugMessageHDMITx("[HDMI_TX] EDID No HF-VSDB Exist!@A03\n");

                // Calculate DTD Table Counts
                pEdidData[0] = ((0xFF - (UINT8)(GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR() + 0x08)) / 0x12);

                // Shift DTD Timing Table
                for(; pEdidData[0] > 0; pEdidData[0]--)
                {
                    u16TmpValue = ((pEdidData[0]) * 0x12);
                    u16TmpValue = u16TmpValue + GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR();
                    // DTD Clear Length = Min_of[0x12, (0xFF - DTD Table_Last_Addr +1]
                    if((0xFF -u16TmpValue ) < 0x12)
                    {
                        memset(&pucDdcRamAddr[u16TmpValue], 0, (0xFF - u16TmpValue));
                    }
                    else
                    {
                        memset(&pucDdcRamAddr[u16TmpValue], 0, 0x12);
                    }

                    // if DTD Write End Point Point < 0xFF
                    if((u16TmpValue + 26) < 0xFF)
                    {
                        u16TmpValue2 = (pEdidData[0] - 1);
                        u16TmpValue2 = u16TmpValue2* 0x12;
                        u16TmpValue2 = u16TmpValue2 + GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR();
                        memcpy(&pucDdcRamAddr[u16TmpValue + 8], &pucDdcRamAddr[u16TmpValue2], 0x12);
                    }
                }

                // Shift Original Prefer Timing to DTD #1
                memcpy(&pucDdcRamAddr[GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR() + 0x08], &pucDdcRamAddr[0x36], 0x12);

                // Add HF-VSDB
                pucDdcRamAddr[GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR()] = 0x67;
                pucDdcRamAddr[GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR() + 1] = 0xD8;
                pucDdcRamAddr[GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR() + 2] = 0x5D;
                pucDdcRamAddr[GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR() + 3] = 0xC4;
                pucDdcRamAddr[GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR() + 4] = 0x01;
                pucDdcRamAddr[GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR() + 5] = 0x78;
                pucDdcRamAddr[GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR() + 6] = 0x00;
                pucDdcRamAddr[GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR() + 7] = 0x00;

                // Modify Totel Data Blocks Length
                pucDdcRamAddr[0x82] = pucDdcRamAddr[0x82] + 0x12;

                // Update first Cta Block DTD Start addr
                SET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR(pucDdcRamAddr[0x82] + 0x80);
            }

            // Change Prefet Timing to 3840x2160 @ 60Hz
            pucDdcRamAddr[0x36] = 0x08;
            pucDdcRamAddr[0x37] = 0xE8;
            pucDdcRamAddr[0x38] = 0x00;
            pucDdcRamAddr[0x39] = 0x30;
            pucDdcRamAddr[0x3A] = 0xF2;
            pucDdcRamAddr[0x3B] = 0x70;
            pucDdcRamAddr[0x3C] = 0x5A;
            pucDdcRamAddr[0x3D] = 0x80;
            pucDdcRamAddr[0x3E] = 0xB0;
            pucDdcRamAddr[0x3F] = 0x58;
            pucDdcRamAddr[0x40] = 0x8A;
            pucDdcRamAddr[0x41] = 0x00;
            pucDdcRamAddr[0x42] = 0xBA;
            pucDdcRamAddr[0x43] = 0x89;
            pucDdcRamAddr[0x44] = 0x21;
            pucDdcRamAddr[0x45] = 0x00;
            pucDdcRamAddr[0x46] = 0x00;
            pucDdcRamAddr[0x47] = 0x1E;

            us420VideoDbStartAddr = pusCtaDataBlockAddr[_CTA_420VDB];
            u8TmpValue = (pucDdcRamAddr[us420VideoDbStartAddr] & 0x1F);
            if(u8TmpValue > 1){
                for(pEdidData[0] = 2; pEdidData[0] <= u8TmpValue - 1; pEdidData[0]++)
                {
                    pucDdcRamAddr[us420VideoDbStartAddr + pEdidData[0]] = 0x00;
                }
            }
            // Calucate EDID Checksum
            ScalerEdidCheckSumCal(pucDdcRamAddr, 0x00);
            pucDdcRamAddr[0x7F] = g_ucOutputTxInternalEDIDChecksumFixed;

            // Calucate EDID Checksum
            ScalerEdidCheckSumCal(pucDdcRamAddr, 0x80);
            pucDdcRamAddr[0xFF] = g_ucOutputTxInternalEDIDChecksumFixed;

            DebugMessageHDMITx("[HDMI_TX] EDID MNT EDID Modify=%x\n", (UINT32)pucDdcRamAddr[0x7F]);
        }
    }

#if(_HDMI_TX_CTS_TEST == _ON)
#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)

    // if Found CTS TEST EDID, need to add 4K timing
    if(GET_HDMI_MAC_TX0_FIND_4K_CTS_TEST_EDID() == _TRUE)
    {
        CLR_HDMI_MAC_TX0_FIND_4K_CTS_TEST_EDID();

        // Add 4K60 594MHz timing
        pucDdcRamAddr[0x36] = 0x08;
        pucDdcRamAddr[0x37] = 0xE8;
        pucDdcRamAddr[0x38] = 0x00;
        pucDdcRamAddr[0x39] = 0x30;
        pucDdcRamAddr[0x3A] = 0xF2;
        pucDdcRamAddr[0x3B] = 0x70;
        pucDdcRamAddr[0x3C] = 0x5A;
        pucDdcRamAddr[0x3D] = 0x80;
        pucDdcRamAddr[0x3E] = 0xB0;
        pucDdcRamAddr[0x3F] = 0x58;
        pucDdcRamAddr[0x40] = 0x8A;
        pucDdcRamAddr[0x41] = 0x00;
        pucDdcRamAddr[0x42] = 0xBA;
        pucDdcRamAddr[0x43] = 0x89;
        pucDdcRamAddr[0x44] = 0x21;
        pucDdcRamAddr[0x45] = 0x00;
        pucDdcRamAddr[0x46] = 0x00;
        pucDdcRamAddr[0x47] = 0x1E;

        // SCDC Present Flag=1
        pucDdcRamAddr[0xA2] = 0x80;

        DebugMessageHDMITx("[HDMI_TX] CTS 4K TEST EDID Found and Modify@A04\n");
    }
#endif

    // no support 8ch 192k in 1080i60, so remove 1080i60 in EDID
    if(GET_HDMI_MAC_TX0_FIND_AUDIO_CTS_TEST_EDID() == _TRUE)
    {
        UINT16 usVideoDbStartAddr = pusCtaDataBlockAddr[_CTA_VDB];
        UINT16 usVideDbVic5Addr = 0;
        UINT8 ucDTDNum = 0;
        BOOLEAN bDTDNeedReplaced = 0;

        UINT8 ucEDIDBlockBytesNum = (pucDdcRamAddr[usVideoDbStartAddr] & 0x1F);

        CLR_HDMI_MAC_TX0_FIND_AUDIO_CTS_TEST_EDID();

        if(GET_HDMI_MAC_TX0_DTD_NUM_NEED_REPLACED() != 0x00)
        {
            for(ucDTDNum = 0; ucDTDNum < 4; ucDTDNum++)
            {
                bDTDNeedReplaced = (BOOLEAN)(GET_HDMI_MAC_TX0_DTD_NUM_NEED_REPLACED() & (_BIT0 << ucDTDNum));
                if(bDTDNeedReplaced == _TRUE)
                {
                    // Replaced Prefer Timing(1080i) to 1920x1080 @ 60Hz
                    pucDdcRamAddr[0x36 + (ucDTDNum * 18)] = 0x02;
                    pucDdcRamAddr[0x37 + (ucDTDNum * 18)] = 0x3A;
                    pucDdcRamAddr[0x38 + (ucDTDNum * 18)] = 0x80;
                    pucDdcRamAddr[0x39 + (ucDTDNum * 18)] = 0x18;
                    pucDdcRamAddr[0x3A + (ucDTDNum * 18)] = 0x71;
                    pucDdcRamAddr[0x3B + (ucDTDNum * 18)] = 0x38;
                    pucDdcRamAddr[0x3C + (ucDTDNum * 18)] = 0x2D;
                    pucDdcRamAddr[0x3D + (ucDTDNum * 18)] = 0x40;
                    pucDdcRamAddr[0x3E + (ucDTDNum * 18)] = 0x58;
                    pucDdcRamAddr[0x3F + (ucDTDNum * 18)] = 0x2C;
                    pucDdcRamAddr[0x40 + (ucDTDNum * 18)] = 0x45;
                    pucDdcRamAddr[0x41 + (ucDTDNum * 18)] = 0x00;
                    pucDdcRamAddr[0x42 + (ucDTDNum * 18)] = 0x55;
                    pucDdcRamAddr[0x43 + (ucDTDNum * 18)] = 0x50;
                    pucDdcRamAddr[0x44 + (ucDTDNum * 18)] = 0x21;
                    pucDdcRamAddr[0x45 + (ucDTDNum * 18)] = 0x00;
                    pucDdcRamAddr[0x46 + (ucDTDNum * 18)] = 0x00;
                    pucDdcRamAddr[0x47 + (ucDTDNum * 18)] = 0x1E;
                }
            }

            // Calucate EDID Checksum
            ScalerEdidCheckSumCal(pucDdcRamAddr, 0x00);
            pucDdcRamAddr[0x7F] = g_ucOutputTxInternalEDIDChecksumFixed;
        }
        // Search for VIC #5 at Video DB
        for(pEdidData[0] = 1; pEdidData[0] <= ucEDIDBlockBytesNum; pEdidData[0]++)
        {
            pEdidData[1] = pucDdcRamAddr[usVideoDbStartAddr + (pEdidData[0])];

#if(_HDMI_TX_CTS_TEST == _ON)
            if(pEdidData[1] == 0x05)
            {
                usVideDbVic5Addr = usVideoDbStartAddr + pEdidData[0];
                break;
            }
#endif
        }
        // Tie 0 for Vic #5
        if(usVideDbVic5Addr != 0x00)
        {
            pucDdcRamAddr[usVideDbVic5Addr] = 0x00;

            // Calucate EDID Checksum
            ScalerEdidCheckSumCal(pucDdcRamAddr, 0x80);
            pucDdcRamAddr[0xFF] = g_ucOutputTxInternalEDIDChecksumFixed;
        }

        DebugMessageHDMITx("[HDMI_TX] CTS Audio 8ch TEST EDID Found and Modify@A05\n");
    }
#endif
}

//--------------------------------------------------
// Description  : Hdmi Tx EDID Modify
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0EdidModify(void)
{
    UINT16 pusDidDataBlockAddr[_DID_TOTAL_DB + 1] = {0};

    UINT8 *pucDdcRamAddr = 0;
    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacPxTxMapping(_TX0));

    ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
    ScalerEdidGetDidExtDbAddress(pusDidDataBlockAddr, pucDdcRamAddr);

    DebugMessageHDMITx("[HDMI_TX] Edid Modify Event=%d\n", (UINT32)GET_HDMI_MAC_TX0_EDID_MODIFY_EVENT());

    switch(GET_HDMI_MAC_TX0_EDID_MODIFY_EVENT())
    {
        case _INITIAL_MODIFY:

            // CTS or Case by case Issue Edid Modify
            ScalerHdmiMacTx0SpecialSinkEdidModify(pusCtaDataBlockAddr);

            // Amd Fressync DB Modify
            ScalerHdmiMacTx0EdidFreesyncCapibilityModify(pucDdcRamAddr, pusCtaDataBlockAddr);

            // 4K Reduce Edid Modifity
#if(_4K2K_EDID_MODIFY_SUPPORT == _ON)
            ScalerEdidReduceBlankTimingModify(EdidMacPxTxMapping(_TX0), pucDdcRamAddr);
#endif

            break;

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
#ifdef NOT_USED_NOW
        case _VIDEO_BANDWIDTH_MODIFY:

            ScalerHdmiMacTx0EdidVideoBandwidthModify(pucDdcRamAddr, pusCtaDataBlockAddr, pusDidDataBlockAddr);
            break;

        case _AUDIO_BANDWIDTH_MODIFY:
            // Realtime Edid Audio Fs Modify due to BW Decision (TODO)
            break;
#endif // #ifdef NOT_USED_NOW
#endif // #if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)

        default:
            break;
    }
}


//--------------------------------------------------
// Description  : Hdmi Tx EDID Modify
// Input Value  :
// Output Value :
//--------------------------------------------------
#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
void ScalerHdmiMacTx0EdidModify_forO18(void)
{
    UINT8  scdcPresent = ScalerHdmiMacTxEdidGetFeature(_P0_OUTPUT_PORT, _SCDC_PRESENT);
    UINT8  maxFrlRate = ScalerHdmiMacTxEdidGetFeature(_P0_OUTPUT_PORT, _MAX_FRL_RATE);
    if(scdcPresent == 0)
        ScalerHdmiMacTx0EdidSetFeature( _SCDC_PRESENT, _TRUE);
    if(maxFrlRate != 6)
        ScalerHdmiMacTx0EdidSetFeature( _MAX_FRL_RATE, 6);
    return;
}
#endif // #if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
#endif
//--------------------------------------------------
// Description  : Hdmi Tx Get EDID VIC TABLE
// Input Value  : VDB TYPE
// Output Value : VIC TABLE
//--------------------------------------------------

void ScalerHdmiMacTx0GetEdidVicTable(EnumEdidCtaDataBlock enumVDBType, UINT8 *pucVICTable, UINT16 *pusCtaDataBlockAddr)
{
    UINT8 ucVDBLength = 0;
    UINT8 ucVICIndex = 0;
    UINT8 ucVICStartOffset = 0;
    UINT16 usVDBAddress = 0;

    if(enumVDBType == _CTA_VDB)
    {
        usVDBAddress = pusCtaDataBlockAddr[_CTA_VDB];

        ucVICStartOffset = 1;
    }
    else if(enumVDBType == _CTA_420VDB)
    {
        usVDBAddress = pusCtaDataBlockAddr[_CTA_420VDB];

        ucVICStartOffset = 2;
    }
    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacPxTxMapping(_TX0));
    ucVDBLength = pucDdcRamAddr[usVDBAddress]& 0x1F;

    if(enumVDBType == _CTA_420VDB)
    {
        if(ucVDBLength > 0)
        {
            ucVDBLength = ucVDBLength -1;//420VDB byte2 is tag,so need -1
        }
    }
    DebugMessageHDMITx("[HDMITX] VDB[%d] Len=%d@", (UINT32)enumVDBType, (UINT32)ucVDBLength);
    DebugMessageHDMITx("Addr[%x]\n", (UINT32)usVDBAddress);
    for(ucVICIndex = 0; ucVICIndex < ucVDBLength; ucVICIndex++)
    {
        pucVICTable[ucVICIndex] = pucDdcRamAddr[usVDBAddress + ucVICIndex + ucVICStartOffset];
    }
}


//--------------------------------------------------
// Description  : Hdmi Tx Set Config
// Input Value  : VDB TYPE
// Output Value : VIC TABLE
//--------------------------------------------------
void ScalerHdmiMacTx0PlugNotify(void)
{

    ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);

    // Set Device Version
    if(GET_HDMI_MAC_TX0_SUPPORT() == _FALSE)
    {
        SET_OUTPUT_DFP_DEVICE_CURRENT_TYPE(EdidMacPxTxMapping(_TX0), _DFP_TYPE_DVI);

        SET_OUTPUT_DFP_DEVICE_VERSION(EdidMacPxTxMapping(_TX0), _DFP_VERSIOIN_NONE);
    }
    else
    {
        SET_OUTPUT_DFP_DEVICE_CURRENT_TYPE(EdidMacPxTxMapping(_TX0), _DFP_TYPE_HDMI);

        if(pusCtaDataBlockAddr[_CTA_HF_VSDB] != 0x00)
        {
            SET_OUTPUT_DFP_DEVICE_VERSION(EdidMacPxTxMapping(_TX0), _DFP_VERSION_2_0);
        }
        else if((pusCtaDataBlockAddr[_CTA_420VDB] != 0x00) && (g_bHdmiMacTx0FindCtsTestEdid == _FALSE))
        {
            SET_OUTPUT_DFP_DEVICE_VERSION(EdidMacPxTxMapping(_TX0), _DFP_VERSION_2_0);
        }
        else
        {
            SET_OUTPUT_DFP_DEVICE_VERSION(EdidMacPxTxMapping(_TX0), _DFP_VERSION_1_4);
        }
    }

    // Set DownStream EDID Valid
    SET_OUTPUT_DFP_DEVICE_VALID(EdidMacPxTxMapping(_TX0));

    // Set DownStream SinkCount = 1
    SET_OUTPUT_DFP_DEVICE_CURRENT_SINK_COUNT(EdidMacPxTxMapping(_TX0), 1);

    // Set DFP Device Capability and DownStream Device Change
    ScalerHdmiMacTx0EdidChangeNotify();
}



//--------------------------------------------------
// Description  : Hdmi Tx Edid Change status reset
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0EdidChangeReset(void)
{
    CLR_OUTPUT_DFP_DEVICE_CAP_CHANGE(EdidMacPxTxMapping(_TX0));
    CLR_OUTPUT_DFP_DEVICE_CHANGE(EdidMacPxTxMapping(_TX0));
}


//--------------------------------------------------
// Description  : Hdmi Tx Edid Change Modify
// Input Value  : VDB TYPE
// Output Value : VIC TABLE
//--------------------------------------------------
void ScalerHdmiMacTx0EdidChangeNotify(void)
{
    // Set DFP Device Capability Change
    SET_OUTPUT_DFP_DEVICE_CAP_CHANGE(EdidMacPxTxMapping(_TX0));

    // Set DownStream Device Change
    SET_OUTPUT_DFP_DEVICE_CHANGE(EdidMacPxTxMapping(_TX0));
}
#ifdef NOT_USED_NOW

UINT16 ScalerEdidGetMaxDdcramSize(EnumOutputPort ucOutputPort)
{

        switch(EdidMacPxTxMapping(ucOutputPort))
        {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
            case _TX0:
                return 0x200;//512
                break;
#endif

            case _TX_MAP_NONE:
            default:

                break;
            }
    return 0x00;
}
#endif


UINT16 ScalerEdidDidCheckSumCal(UINT16 usStartAddr, UINT16 usEndAddr)
{
    UINT8 ucDBLengthDeviation = 0;
    UINT8 count =0;
    UINT32  checkSum =0;
    ucDBLengthDeviation = usEndAddr - usStartAddr;
    for(count =0 ;count < ucDBLengthDeviation; count++)
    {
        checkSum += pucDdcRamAddr[usStartAddr + count] ;
    }
    return (0xFF & (0x100 - (checkSum &0xFF)));
}

UINT16 ScalerEdidCheckSumCal(UINT8 *pucDdcRamAddr,UINT16 usStartAddr)
{
    UINT8 count =0;
    UINT32  checkSum =0;

    for(count =0 ;count < 0x7F; count++)
    {
        checkSum += pucDdcRamAddr[usStartAddr + count] ;
    }
    g_ucOutputTxInternalEDIDChecksumFixed = (0xFF & (0x100 - (checkSum &0xFF)));
    return (0xFF & (0x100 - (checkSum &0xFF)));
}

void ScalerEdidGeneralStructParse(EnumOutputPort ucOutputPort)
{
    UINT8 main_ver = 0;
    UINT8 sub_ver = 0;
    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacPxTxMapping(_TX0));
    //vendor model?
    //0x08 0x09 Manufacturer Name
    //0x0A 0x0B product code
    main_ver = pucDdcRamAddr[0x12];
    sub_ver = pucDdcRamAddr[0x13];
    DebugMessageHDMITx("[HDMITX] EDID Ver: %x", (UINT32)pucDdcRamAddr[0x18]);
    DebugMessageHDMITx("%x", (UINT32)pucDdcRamAddr[0x19]);
    DebugMessageHDMITx("(%x)\n", (UINT32)main_ver);
    DebugMessageHDMITx("(%x)\n", (UINT32)sub_ver);
    if(main_ver == 0x02)///main version
    {
        ScalerHdmiMacTx0EdidSetFeature(_EDID_VERSION,_EDID_VER20);
    }
    else
    {
        if(sub_ver == 0x03)//sub version
        {
            ScalerHdmiMacTx0EdidSetFeature(_EDID_VERSION,_EDID_VER13);
        }
        else
        {
            ScalerHdmiMacTx0EdidSetFeature(_EDID_VERSION,_EDID_VER14);
        }
    }
    return;
}

void ScalerEdidExtBlockDbParse(EnumOutputPort ucOutputPort)
{

    //Get Ext DB Addr
    ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
    //Get Ext DB support featrue
    ScalerHdmiMacTxEdidCtaExtDbParse(ucOutputPort,pusCtaDataBlockAddr);

    return;
}


void FindDIDExtDTDTimingAddr(UINT16 usTmpBaseAddr)
{
    UINT16 usModifyEndDBAddr = 0;
    UINT16 u16TmpValue = 0;
    UINT8 ucOffset = 0x00;
    UINT16 usModifyStartDBAddr = 0;
    usModifyStartDBAddr = usTmpBaseAddr + 5;
    u16TmpValue = pucDdcRamAddr[usTmpBaseAddr + 2];//DID block lenth
    usModifyEndDBAddr = usModifyStartDBAddr + u16TmpValue;
    while(usModifyStartDBAddr < usModifyEndDBAddr){
        if(pucDdcRamAddr[usModifyStartDBAddr] == 0x3){//only find tag = 0x3 DTD timing
            pusCtaDataBlockAddr[_DID_DTDDB] = usModifyStartDBAddr;
            break;
        }
        ucOffset = pucDdcRamAddr[usModifyStartDBAddr + 2] + 2; 
        usModifyStartDBAddr = usModifyStartDBAddr + ucOffset + 1;
         if(usModifyEndDBAddr <= (usModifyStartDBAddr)) // Should not happen. if overflow, then break.
        {
            break;
        }
    }
}
//static UINT16 usDetailTimeOffset;
//static UINT16 usNewDataAdd;
//static UINT8 ucOffset;

void ScalerEdidGetCtaExtDbAddress(UINT16 *pusCtaDataBlockAddr)
{
    UINT16 usTmpBaseAddr = 0x00;
    UINT16 usDetailTimeOffset = 0x00;
    UINT16 usEDIDExtBase= 0x7E;
    UINT8 ucOffset = 0x00;
    UINT16 usExtDetailTimeBase = 0x82; // Detailed Timing address of Extension Block
    UINT16 usNewDataAdd;
    UINT8 ucEdidExtBlockNum;
    BOOLEAN bEdidCtaVdbBlock = _FALSE;
    BOOLEAN bEdidCta420VdbBlock = _FALSE;
    UINT8 tmpDdcRamAddr2;
    UINT8 tmpDdcRamAddr;

    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacPxTxMapping(_TX0));

    usExtDetailTimeBase = 0x82;
    usDetailTimeOffset = 0x00;
    usEDIDExtBase = 0x7E;
    usNewDataAdd = 0x84;
    ucExtVdbBlockNum = 0;
    ucExt420VdbBlockNum = 0;
    usDetailTimeOffset = 0x80 + pucDdcRamAddr[usExtDetailTimeBase]; //offset before Detail Timing Description
    if((pucDdcRamAddr[usEDIDExtBase] & 0x03) >= 0x01)//CTA extension exist
    {
        // [RTKREQ-854][CTS] HDMI 1.4b 7-33 Iter-05: HDMI 4 block EDID
        ucEdidExtBlockNum = pucDdcRamAddr[usEDIDExtBase];
        DebugMessageHDMITx("[HDMITX][EDID][1] ExtBlock Num=%d", (UINT32)ucEdidExtBlockNum);
        DebugMessageHDMITx(" BlockTag=%x", (UINT32)pucDdcRamAddr[EDID_BLOCK1_TAG_BASE]);
        DebugMessageHDMITx(" BlockTag2=/%x\n", (UINT32)pucDdcRamAddr[EDID_BLOCK2_TAG_BASE]);
        DebugMessageHDMITx(" BlockTag2=/%x\n", (UINT32)pucDdcRamAddr[EDID_BLOCK3_TAG_BASE]);
        if(((ucEdidExtBlockNum == 2) || (ucEdidExtBlockNum == 3)) && (pucDdcRamAddr[EDID_BLOCK1_TAG_BASE] == EDID_BLOCK_MAP_TAG)){
            if(pucDdcRamAddr[EDID_BLOCK2_TAG_BASE] == EDID_EXTENSION_TAG){ // EDID extension tag of block 2
                usExtDetailTimeBase = EDID_BLOCK2_EXT_DTD_BASE; // Detailed Timing address of extension Block 2
                usNewDataAdd = EDID_BLOCK2_EXT_DTD_ADDR; // Detail timing info base address of extension block 2
                usDetailTimeOffset = EDID_BLOCK2_TAG_BASE + pucDdcRamAddr[usExtDetailTimeBase];
                DebugMessageHDMITx("[HDMITX][EDID][1] Get Block2 extension data[%d", (UINT32)usNewDataAdd);
                DebugMessageHDMITx("->%d]\n", (UINT32)usDetailTimeOffset);
            }
            else if((ucEdidExtBlockNum == 3) && (pucDdcRamAddr[EDID_BLOCK3_TAG_BASE] == EDID_EXTENSION_TAG)){
                usExtDetailTimeBase = EDID_BLOCK3_EXT_DTD_BASE; // Detailed Timing address of extension Block 3
                usNewDataAdd = EDID_BLOCK3_EXT_DTD_ADDR; // Detail timing info base address of extension block 3
                usDetailTimeOffset = EDID_BLOCK3_TAG_BASE + pucDdcRamAddr[usExtDetailTimeBase];
                DebugMessageHDMITx("[HDMITX][EDID][1] Get Block3 extension data[%d->%d]\n", (UINT32)usNewDataAdd, (UINT32)usDetailTimeOffset);
            }
        }
        usTmpBaseAddr = 0x80 * ucEdidExtBlockNum;
         if(pucDdcRamAddr[usTmpBaseAddr] == EDID_DID_EXTENSION_TAG){
            //block3 DID dtd ext
            //find timing tag 0x3 addr
            usDidSExtStartAddr = usTmpBaseAddr + 1;
            usDidSExtEndAddr = (usDidSExtStartAddr + pucDdcRamAddr[usDidSExtStartAddr + 1] + 4);
            FindDIDExtDTDTimingAddr(usTmpBaseAddr);
        }

        // HF-EEODB: 0x84~0x86
        if(usNewDataAdd == 0x84){
            DebugMessageHDMITx("[HDMITX] Byte[0x84:0x86]=%x|", (UINT32)pucDdcRamAddr[(usNewDataAdd)]);
            DebugMessageHDMITx("%x|", (UINT32)pucDdcRamAddr[(usNewDataAdd+1)]);
            DebugMessageHDMITx("%x]\n", (UINT32)pucDdcRamAddr[(usNewDataAdd+2)]);
            tmpDdcRamAddr2 = *(pucDdcRamAddr + usNewDataAdd);
            if(((tmpDdcRamAddr2 & 0xe0) == 0xe0) && (pucDdcRamAddr[usNewDataAdd+1] == 0x78))//_CTA_HF_EEODB
            {
                pusCtaDataBlockAddr[_CTA_HF_EEODB] = usNewDataAdd;
                DebugMessageHDMITx("EEODB Match!\n");
            }
        }

        while(usNewDataAdd < usDetailTimeOffset)
        {
            tmpDdcRamAddr2 = *(pucDdcRamAddr + usNewDataAdd);
            tmpDdcRamAddr = tmpDdcRamAddr2 & 0xE0;
            if((tmpDdcRamAddr) == 0x40 ) //_CTA_VDB
            {
                if(!bEdidCtaVdbBlock){
                    pusCtaDataBlockAddr[_CTA_VDB] = usNewDataAdd;
                    bEdidCtaVdbBlock = _TRUE;
                }else{
                    pusExtVdbBlockAddr[ucExtVdbBlockNum] = usNewDataAdd;
                    ucExtVdbBlockNum ++;
                }
            }
            else if((tmpDdcRamAddr) == 0x20 )//_CTA_ADB
            {
                pusCtaDataBlockAddr[_CTA_ADB] = usNewDataAdd;
            }
            else if((tmpDdcRamAddr) == 0x60)//_CTA_HF_VSDB
            {
                tmpDdcRamAddr = *(pucDdcRamAddr + usNewDataAdd +1) ;
                //Hdmi 2.0 vsdb
                if(tmpDdcRamAddr== 0xD8)
                {
                    if(pucDdcRamAddr[usNewDataAdd+2] ==0x5D)
                        if(pucDdcRamAddr[usNewDataAdd+3] ==0xC4)
                            pusCtaDataBlockAddr[_CTA_HF_VSDB] = usNewDataAdd;
                }
                else if(tmpDdcRamAddr== 0x03)
                {//hdmi 1.4 vsdb
                    if(pucDdcRamAddr[usNewDataAdd+2] ==0x0c)
                        if(pucDdcRamAddr[usNewDataAdd+3] ==0x00)
                            pusCtaDataBlockAddr[_CTA_HDMI_VSDB] = usNewDataAdd;
                }
                else if(tmpDdcRamAddr== 0x1A)
                {
                  if(pucDdcRamAddr[usNewDataAdd+2] ==0x00)
                        if(pucDdcRamAddr[usNewDataAdd+3] ==0x00)
                            pusCtaDataBlockAddr[_CTA_FS_DB] = usNewDataAdd;
                }
            }
            else if((tmpDdcRamAddr) == 0xE0 )//extension tag
            {
                if(pucDdcRamAddr[usNewDataAdd+1] == 0 )//_CTA_VCDB
                {
                        pusCtaDataBlockAddr[_CTA_VCDB] = usNewDataAdd;
                }
                else if(pucDdcRamAddr[usNewDataAdd+1] == 1 )//_CTA_VSVDB_HDR10
                {
                        if(pucDdcRamAddr[usNewDataAdd+2] ==0x8B && pucDdcRamAddr[usNewDataAdd+3] ==0x84 && pucDdcRamAddr[usNewDataAdd+4] ==0x90){
                            pusCtaDataBlockAddr[_CTA_VSVDB_HDR10] = usNewDataAdd;
                        }
                }
                else if(pucDdcRamAddr[usNewDataAdd+1] == 5)//_CTA_CDB
                {
                        pusCtaDataBlockAddr[_CTA_CDB] = usNewDataAdd;
                }
                else if(pucDdcRamAddr[usNewDataAdd+1] == 6)//_CTA_SHDR_DB
                {
                        pusCtaDataBlockAddr[_CTA_SHDR_DB] = usNewDataAdd;
                }
                else if(pucDdcRamAddr[usNewDataAdd+1] == 7)//_CTA_DHDR_DB
                {
                        pusCtaDataBlockAddr[_CTA_DHDR_DB] = usNewDataAdd;
                }
                else if(pucDdcRamAddr[usNewDataAdd+1] == 14)//_CTA_420VDB
                {
                        if(!bEdidCta420VdbBlock){
                            pusCtaDataBlockAddr[_CTA_420VDB] = usNewDataAdd;
                            bEdidCta420VdbBlock = _TRUE;
                        }else{
                            pusExt420VdbBlockAddr[ucExt420VdbBlockNum] = usNewDataAdd;
                            ucExt420VdbBlockNum ++;
                        }
                }
                else if(pucDdcRamAddr[usNewDataAdd+1] == 15)//_CTA_420CMDB
                {
                        pusCtaDataBlockAddr[_CTA_420CMDB] = usNewDataAdd;
                }
                else if(pucDdcRamAddr[usNewDataAdd+1] == 121)//_CTA_HF_SCDB
                {
                        pusCtaDataBlockAddr[_CTA_HF_SCDB] = usNewDataAdd;
                }
            }

            ucOffset = (pucDdcRamAddr[usNewDataAdd] & 0x1F);  // Length of data block
            usNewDataAdd = (usNewDataAdd + ucOffset + 1);


            if(usNewDataAdd <= (0x80)) // Should not happen. if overflow, then break.
            {
                break;
            }
            else if((usNewDataAdd >= usDetailTimeOffset) && (usExtDetailTimeBase == EDID_BLOCK2_EXT_DTD_BASE)){ // Read EDID block3
                if((ucEdidExtBlockNum == 3) && (pucDdcRamAddr[EDID_BLOCK3_TAG_BASE] == EDID_EXTENSION_TAG)){
                    usExtDetailTimeBase = EDID_BLOCK3_EXT_DTD_BASE; // Detailed Timing address of extension Block 3
                    usNewDataAdd = EDID_BLOCK3_EXT_DTD_ADDR; // Detail timing info base address of extension block 3
                    usDetailTimeOffset = EDID_BLOCK3_TAG_BASE + pucDdcRamAddr[usExtDetailTimeBase];
                    ucExtVdbBlockNum = 0;
                    bEdidCtaVdbBlock = _FALSE;
                    ucExt420VdbBlockNum = 0;
                    bEdidCta420VdbBlock = _FALSE;
                    DebugMessageHDMITx("[HDMITX][EDID][1] Get Block3 extension data[%d->%d]\n", (UINT32)usNewDataAdd, (UINT32)usDetailTimeOffset);
                }
            }
        }


    }

    return;
}

void CorrectAllEdidBlockChecksum(void)
{
    UINT8 i;
    UINT16 usEDIDExtBase= 0x7E;
    UINT8 ucEdidExtBlockNum;
    UINT16 usEdidModifyStartAddr=0;
    UINT16 usCheckSum=0;

    ucEdidExtBlockNum = pucDdcRamAddr[usEDIDExtBase];

    for(i=0; i <= ucEdidExtBlockNum; i++){
        usEdidModifyStartAddr = i * 0x80;
        usCheckSum = ScalerEdidCheckSumCal(pucDdcRamAddr, usEdidModifyStartAddr);
        pucDdcRamAddr[usEdidModifyStartAddr | 0x7f] = g_ucOutputTxInternalEDIDChecksumFixed;
        DebugMessageHDMITx("[HDMITX] [%d]block checksum[%d] \n", (UINT32)i,(UINT32)usCheckSum);
    }
}
void ModifyEdidProcess(void)
{
    if(GET_HDMI_MAC_TX0_MODIFY_EDID_ENABLE() == _ENABLE)
    {
        //ScalerHdmiMacTx0EdidModify();

        ScalerHdmiMacTx0EdidModifyEnable(_DISABLE, _NO_EDID_MODIFY);

        SET_HDMI_MAC_TX0_EDID_MODIFY_STATE(EDID_CHECK_AFT_MODIFY);

  #ifdef CONFIG_HDMITX_REPEATER_SUPPORT
        if((HDMITX_MISC_HDMITX_MISC_INT_get_dummy_18_12(rtd_inl(HDMITX_MISC_HDMITX_MISC_INT_reg)) & _BIT0) == 0) {
            FindExtensionTagBlock();
            ScalerHdmiMacTx0RepeaterEdidInfoUpdate(EdidMacTxPxMapping(_TX0));
            CorrectAllEdidBlockChecksum();
        }
  #endif

        // Notic SRC to Read Edid
        ScalerHdmiMacTx0EdidChangeNotify();
    }
}
#endif
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
UINT8 hdmitx_InputSrcPort = 0xff;
UINT8 hdmitx_Port = 0xff;
TX_INPUT_SRC_SELECT_TYPE hdmitx_InputSrc = TX_INPUT_SOURCE_NUM;
#if (IS_ENABLED(CONFIG_ENABLE_DPRX)) && !IS_ENABLED(ENABLE_INTERNAL_HDMIRX_PTG)//CONFIG_HDMITX_AP_TEST
extern HDMI_DP_TIMING_T* drvif_Dprx_GetRawTimingInfo(UINT8 port);
#endif

UINT8 ScalerHdmiMacTx0RepeaterSetSinkTxSupportTimingCount(EDID_TIMING_DB edid_timing_type, UINT8 value);
UINT8 ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(EDID_TIMING_DB edid_timing_type);
TX_TIMING_INDEX ScalerHdmiMacTx0RepeaterGetTxOutputModeTiming(TX_OUT_MODE_SELECT_TYPE mode);
extern UINT8 HDMITX_TIMING_INDEX;
TX_OUT_MODE_SELECT_TYPE repeater_tx_out_mode=TX_OUT_SOURCE_MODE;
TX_TIMING_INDEX repeater_tx_out_timing_user=TX_TIMING_1080P60;
extern UINT8 ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(EDID_TIMING_DB edid_timing_type);
//static BOOLEAN boolTmpValue = 0;
UINT16 pixelClockLimit;
UINT32 SourceOutputPixelClk;
MEASURE_TIMING_T timing_info;


#define ABS(x,y)						((x > y) ? (x-y) : (y-x))


UINT16 ScalerHdmiMacTx0GetDsSize_HDMI_DP_TIMING_T(void)
{
    return sizeof(HDMI_DP_TIMING_T);
}


UINT16 ScalerHdmiMacTx0GetDsSize_StructDisplayInfo(void)
{
    return sizeof(StructHdmitxDisplayInfo);
}


UINT16 ScalerHdmiMacTx0GetDsSize_StructHDMITxOutputTimingInfo(void)
{
    return sizeof(StructHDMITxOutputTimingInfo);
}
void getDTDTimingInfo(UINT16 ucDTDAddress)
{
        UINT32 u32TmpValue = 0;
        UINT16 u16TmpValue = 0;
        edid_timing.pixelClk = 0;
        u32TmpValue = pucDdcRamAddr[ucDTDAddress + 1];
        u32TmpValue = u32TmpValue << 8;
        u32TmpValue = u32TmpValue | (pucDdcRamAddr[ucDTDAddress]);
        edid_timing.pixelClk = u32TmpValue;
        //NoteMessageHDMITx("a getDTDTimingInfo @%d\n",(UINT32)edid_timing.pixelClk);
        //edid_timing.pixelClk = edid_timing.pixelClk | ((pucDdcRamAddr[ucDTDAddress + 1] << 8));
        //edid_timing.pixelClk = edid_timing.pixelClk | (pucDdcRamAddr[ucDTDAddress]);

        edid_timing.h_act = (pucDdcRamAddr[ucDTDAddress + 4] & 0xF0);
        edid_timing.h_act = edid_timing.h_act  << 4;
        u16TmpValue = ucDTDAddress + 2;
        edid_timing.h_act = edid_timing.h_act | (pucDdcRamAddr[u16TmpValue]);

        edid_timing.h_blank =(pucDdcRamAddr[ucDTDAddress + 4] & 0x0F);
        edid_timing.h_blank = edid_timing.h_blank << 8;
        edid_timing.h_blank = edid_timing.h_blank | (pucDdcRamAddr[ucDTDAddress + 3]);

        edid_timing.v_act = ((pucDdcRamAddr[ucDTDAddress + 7] & 0xF0) << 4) | (pucDdcRamAddr[ucDTDAddress + 5]);
        edid_timing.v_blank = ((pucDdcRamAddr[ucDTDAddress + 7] & 0x0F) << 8) | (pucDdcRamAddr[ucDTDAddress + 6]);
}

void get20ByteDTDTimingInfo(UINT16 ucDTDAddress)
{
        UINT32 u32TmpValue = 0;
        edid_timing.pixelClk = 0;
        u32TmpValue = pucDdcRamAddr[ucDTDAddress + 2];
        u32TmpValue = u32TmpValue << 16;
        u32TmpValue2 = pucDdcRamAddr[ucDTDAddress + 1];
        u32TmpValue2 = u32TmpValue2 << 8;
        u32TmpValue = u32TmpValue |u32TmpValue2;
        u32TmpValue = u32TmpValue | (pucDdcRamAddr[ucDTDAddress]);
        u32TmpValue += 1;
        edid_timing.pixelClk = u32TmpValue;
        //edid_timing.pixelClk = edid_timing.pixelClk | ((pucDdcRamAddr[ucDTDAddress + 2] << 16));
        //edid_timing.pixelClk = edid_timing.pixelClk | ((pucDdcRamAddr[ucDTDAddress + 1] << 8));
        //NoteMessageHDMITx("a get20ByteDTDTimingInfo @%d\n",(UINT32)edid_timing.pixelClk);
        //edid_timing.pixelClk = edid_timing.pixelClk | (pucDdcRamAddr[ucDTDAddress]);
        //edid_timing.pixelClk += 1;
 
        edid_timing.h_act = (pucDdcRamAddr[ucDTDAddress + 5] << 8);
        edid_timing.h_act = edid_timing.h_act | (pucDdcRamAddr[ucDTDAddress + 4]);
        edid_timing.h_act += 1;

        edid_timing.h_blank = (pucDdcRamAddr[ucDTDAddress + 7] << 8);
        edid_timing.h_blank = edid_timing.h_blank | (pucDdcRamAddr[ucDTDAddress + 6]);
        edid_timing.h_blank += 1;

        edid_timing.v_act = (pucDdcRamAddr[ucDTDAddress + 13] << 8);
        edid_timing.v_act = edid_timing.v_act | (pucDdcRamAddr[ucDTDAddress + 12]);
        edid_timing.v_act += 1;

        edid_timing.v_blank = (pucDdcRamAddr[ucDTDAddress + 15] << 8);
        edid_timing.v_blank = edid_timing.v_blank | (pucDdcRamAddr[ucDTDAddress + 14]);
        edid_timing.v_blank += 1;
}

UINT8 matchCDFNoSupportTiming(UINT8 vic)
{
    UINT8 i = 0;
    for(i = 0; i < sizeof(CDF_no_support_timing_list); i++)
    {
        if(vic == CDF_no_support_timing_list[i])
            return TRUE;
    }
    return FALSE;
}

UINT8 ScalerHdmiMacTx0RepeaterCheckTimingListExist(EDID_TIMING_DB edid_timing_type, UINT8 time_mode,BOOLEAN bSupport420)
{
    UINT8  i = 0;
    for(i = 0; i < repeater_support_timing_table.totalCnt; i ++) {
        if((repeater_support_timing_table.tx_index[i] == time_mode)
            && (repeater_support_timing_table.db_type[i].bSupport420 == bSupport420)
            #ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
            && (repeater_support_timing_table.db_type[i].db_type == edid_timing_type)
            #endif
            )
            return TRUE;
    }
    return FALSE;
}

/*
    Description  : Push Sink EDID support timing list to repeater_support_timing_table
    Input Value  :
        edid_timing_type:
            EDID_TIMING_ANY, EDID_TIMING_VDB, EDID_TIMING_420VDB, EDID_TIMING_DTD, EDID_TIMING_EXT_DTD,
        time_mode: tx timing mode index
    Output Value :
*/

void ScalerHdmiMacTx0RepeaterPushSinkTxSupportTimingList(EDID_TIMING_DB edid_timing_type, UINT8 time_mode,BOOLEAN bSupport420)
{

    if(time_mode > TX_TIMING_NUM){
        DebugMessageHDMITx("[HDMITX] INVALID TIME_MODE[%d]@ScalerHdmiMacTx0RepeaterPushSinkTxSupportTimingList()\n", (UINT32)time_mode);
        return;
    }
    if(repeater_support_timing_table.totalCnt >= EDID_SUPPORT_LIST_NUM) {
        FatalMessageHDMITx("[HDMITX] list totalcnt over\n");
        return;
    }

    // if time_mode have exist in timinglist, return
    if(ScalerHdmiMacTx0RepeaterCheckTimingListExist(edid_timing_type, time_mode, bSupport420)) {
        DebugMessageHDMITx("[HDMITX] index[%d] exist\n", (UINT32)time_mode);
        return;
    }

    DebugMessageHDMITx("[HDMITX] Push[%d]", (UINT32)repeater_support_timing_table.totalCnt);
    DebugMessageHDMITx("Type %d, b420:%d,",(UINT32)edid_timing_type,(UINT32)bSupport420);
    DebugMessageHDMITx("Index[%d]", (UINT32)time_mode);
    DebugMessageHDMITx("@VIC[%d/%d]\n", (UINT32)cea_timing_table_repeater[time_mode].video_code1, (UINT32)cea_timing_table_repeater[time_mode].video_code2);

    repeater_support_timing_table.db_type[repeater_support_timing_table.totalCnt].db_type= edid_timing_type;
    repeater_support_timing_table.tx_index[repeater_support_timing_table.totalCnt] = time_mode;
    repeater_support_timing_table.db_type[repeater_support_timing_table.totalCnt].bSupport420= bSupport420;
    repeater_support_timing_table.totalCnt++;
    return;
}

/*
    Description  : Pop Sink EDID support timing list
    Input Value  :
        edid_timing_type:
        typedef enum _EDID_TIMING_DB{
            EDID_TIMING_ANY=0,
            EDID_TIMING_VDB,
            EDID_TIMING_420VDB,
            EDID_TIMING_420CMDB,
            EDID_TIMING_DTD,
            EDID_TIMING_EXT_DTD,
            EDID_TIMING_DID_DTD,
            EDID_TIMING_MODE_NUM,
        }EDID_TIMING_DB;
        index: query timing list index
    Output Value :
        return: -1: search fail
*/

TX_TIMING_INDEX ScalerHdmiMacTx0RepeaterPopSinkTxSupportTimingList(EDID_TIMING_DB edid_timing_type, UINT8 index, UINT8 *db_type, BOOLEAN *bSupport420)
{
    UINT8  matchIndex = 0;
    UINT8 value_u8 = 0;

    if(index >= ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(edid_timing_type)){
        DebugMessageHDMITx("[HDMITX] INVALID SUPPORT Type[%x],", (UINT32)edid_timing_type);
        DebugMessageHDMITx("IDX[%d]=", (UINT32)index);
        DebugMessageHDMITx("%d\n", (UINT32)ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(edid_timing_type));
        return TX_TIMING_NUM;
    }

    if(edid_timing_type == EDID_TIMING_ANY){
        FatalMessageHDMITx("[HDMITX] POP Type[%d]=%d,b420:%d@Index[%d]\n", (UINT32)edid_timing_type ,repeater_support_timing_table.tx_index[index],
            (UINT32)repeater_support_timing_table.db_type[index].bSupport420, (UINT32)index);
        if(db_type)
            *db_type = repeater_support_timing_table.db_type[index].db_type;
        if(bSupport420)
            *bSupport420 = repeater_support_timing_table.db_type[index].bSupport420;
        return repeater_support_timing_table.tx_index[index];
    }

    for(value_u8=0; value_u8 < repeater_support_timing_table.totalCnt; value_u8++){
        if(repeater_support_timing_table.db_type[value_u8].db_type== edid_timing_type){
            if(matchIndex == index){
                NoteMessageHDMITx("[HDMITX] POP Type[%d] =%d b420:%d@Index[%d]\n", (UINT32)edid_timing_type , (UINT32)repeater_support_timing_table.tx_index[value_u8],
                    (UINT32)repeater_support_timing_table.db_type[value_u8].bSupport420, (UINT32)value_u8);
                if(bSupport420)
                    *bSupport420 = repeater_support_timing_table.db_type[value_u8].bSupport420;
                if(db_type)
                    *db_type = repeater_support_timing_table.db_type[value_u8].db_type;
                return repeater_support_timing_table.tx_index[value_u8];
            }
            matchIndex++;
        }
    }
    //FatalMessageHDMITx("[HDMITX] SHOULD NOT HAPPEN, FAIL SEARCH SUPPORT TIMING INDEX[%d]=", (UINT32)edid_timing_type);
    FatalMessageHDMITx("[HDMITX] FAIL SEARCH INDEX[%d]=", (UINT32)edid_timing_type);
    FatalMessageHDMITx("%d/%d\n", (UINT32)index, (UINT32)ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(edid_timing_type));
    return TX_TIMING_NUM;
}


//--------------------------------------------------
// Description  : Dump Short Video Descriptor VIC
// Input Value  : _CTA_VDB, _CTA_420VDB
// Output Value :
//--------------------------------------------------

void ScalerHdmiMacTx0RepeaterEdidVdbVicDump(EnumEdidCtaDataBlock data_type, UINT8 ucExtVdbBlockNum)
{
    UINT8 ucVDBLength = 0;
    UINT8 value_u8 = 0;
    UINT8 ucVICStartOffset = 0;
    UINT16 usVDBAddress = 0;
    UINT8 ucFoundVdbCnt=0;
    EDID_TIMING_DB timingType;
    UINT8 ucVic;

    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacTxPxMapping(_TX0));
    ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);


    if(data_type == _CTA_VDB){
        ucVICStartOffset = 1;
        timingType = EDID_TIMING_VDB;
        if(!ucExtVdbBlockNum)
            usVDBAddress = pusCtaDataBlockAddr[data_type];
        else
            usVDBAddress = pusExtVdbBlockAddr[ucExtVdbBlockNum - 1];
    }else if(data_type == _CTA_420VDB){
        ucVICStartOffset = 2;
        timingType = EDID_TIMING_420VDB;
        if(!ucExtVdbBlockNum)
            usVDBAddress = pusCtaDataBlockAddr[data_type];
        else
            usVDBAddress = pusExt420VdbBlockAddr[ucExtVdbBlockNum - 1];

    }else{
        DebugMessageHDMITx("[HDMITX] NOT SUPPORT VDB[%d]@EdidVdbVicDump()\n", (UINT32)data_type);
        return;
    }

    ucVDBLength = pucDdcRamAddr[usVDBAddress] & 0x1F;
    if(data_type == _CTA_420VDB)
    {
         if(ucVDBLength > 0){
            ucVDBLength = ucVDBLength -1;//420VDB byte2 is tag,so need -1
        }
    }
    DebugMessageHDMITx("[HDMITX] VDB[%d] Len=%d@", (UINT32)data_type, (UINT32)ucVDBLength);
    DebugMessageHDMITx("Addr[%x]:\n",  (UINT32)usVDBAddress);

    for(value_u8 = 0; value_u8 < ucVDBLength; value_u8++)
    {
        ucVic = pucDdcRamAddr[usVDBAddress + value_u8 + ucVICStartOffset];
        if(ucVic != 0)
            ucFoundVdbCnt++;
        DebugMessageHDMITx("[%2d]:%3d\n", (UINT32)value_u8, (UINT32)ucVic);
    }

    ScalerHdmiMacTx0RepeaterSetSinkTxSupportTimingCount(timingType, ucFoundVdbCnt);
    return;
}


void ScalerHdmiMacTx0RepeaterEdidDIDExtDTDInfoDump(EDID_BLOCK_INDEX edidBlock)
{
    EDID_TIMING_DB timingType;
    UINT8 ucDTDNum;
    UINT16 ucDTDAddress;
    UINT16 usExtDetailTimeBase = 0x82; // Detailed Timing address of Extension Block
    UINT8 ucFoundExtDtdCnt=0;
    UINT8 ucExtDtdNum;
    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacTxPxMapping(_TX0));

    usExtDetailTimeBase = pusCtaDataBlockAddr[_DID_DTDDB];//modify ExtensionTag Block DTD
    ucDTDAddress = usExtDetailTimeBase + 3;
    ucExtDtdNum = pucDdcRamAddr[usExtDetailTimeBase + 2] / 20;
    timingType = EDID_TIMING_DID_DTD;
    DebugMessageHDMITx("[HDMI_TX] dump DTD[%d] Num=%d", (UINT32)edidBlock, (UINT32)ucExtDtdNum);
    DebugMessageHDMITx("@ADDR[%x(/%x)]\n",(UINT32)ucDTDAddress, (UINT32)GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR());

    for(ucDTDNum = 0; ucDTDNum < ucExtDtdNum; ucDTDNum++)
    {
        get20ByteDTDTimingInfo(ucDTDAddress);
        if(edid_timing.pixelClk && edid_timing.h_act && edid_timing.v_act){
            ucFoundExtDtdCnt++;
            DebugMessageHDMITx("[HDMI_TX] DTD[%d][%d]",(UINT32)edidBlock, (UINT32)ucDTDNum);
            DebugMessageHDMITx("%dx%d",(UINT32)edid_timing.h_act, (UINT32)edid_timing.v_act);
            DebugMessageHDMITx("(%dx%d)MHz",(UINT32)(edid_timing.h_act+edid_timing.h_blank), (UINT32)(edid_timing.v_act+edid_timing.v_blank));
            DebugMessageHDMITx("@%d\n",(UINT32)edid_timing.pixelClk/10);
        }

        ucDTDAddress = ucDTDAddress + 20;
    }

    ScalerHdmiMacTx0RepeaterSetSinkTxSupportTimingCount(timingType, ucFoundExtDtdCnt);

    if(ucFoundExtDtdCnt == 0)
        DebugMessageHDMITx("[HDMI_TX] Not DTD be Found@Block[%d]!!\n", (UINT32)edidBlock);

    return;
}
//--------------------------------------------------
// Description  : Dump Extened Detailed Timing Descriptor
// Input Value  : EDID_BLOCK_0, EDID_BLOCK_1
// Output Value :
//--------------------------------------------------


void ScalerHdmiMacTx0RepeaterEdidDTDInfoDump(EDID_BLOCK_INDEX edidBlock)
{
    EDID_TIMING_DB timingType;
    UINT8 ucDTDNum;
    UINT16 ucDTDAddress;
    UINT16 usExtDetailTimeBase = 0x82; // Detailed Timing address of Extension Block
    UINT16 usDetailTimeOffset = 0x00;
    UINT8 ucFoundExtDtdCnt=0;
    UINT8 ucExtDtdNum;
    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacTxPxMapping(_TX0));
    usExtDetailTimeBase = 0x82;
    if(edidBlock == EDID_BLOCK_0){
        ucDTDAddress = 0x36;
        ucExtDtdNum = 4;
        timingType = EDID_TIMING_DTD;
    }else if(edidBlock == EDID_BLOCK_1){
        usExtDetailTimeBase = usExtTagBaseAddr + 0x2;//modify ExtensionTag Block DTD
        usDetailTimeOffset = usExtTagBaseAddr + pucDdcRamAddr[usExtDetailTimeBase];
        ucDTDAddress = usDetailTimeOffset;
        ucExtDtdNum = (usExtTagBaseAddr + 0x7f - ucDTDAddress) / 18;
        timingType = EDID_TIMING_EXT_DTD;
    }else{
        DebugMessageHDMITx("[HDMI_TX] DTD[%d] Not Support Yet!!\n", (UINT32)edidBlock);
        return;
    }

    DebugMessageHDMITx("[HDMI_TX] dump DTD[%d] Num=%d", (UINT32)edidBlock, (UINT32)ucExtDtdNum);
    DebugMessageHDMITx("@ADDR[%x(/%x)]\n",(UINT32)ucDTDAddress, (UINT32)GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR());

    for(ucDTDNum = 0; ucDTDNum < ucExtDtdNum; ucDTDNum++)
    {
        getDTDTimingInfo(ucDTDAddress);
        if(edid_timing.pixelClk && edid_timing.h_act && edid_timing.v_act){
            ucFoundExtDtdCnt++;
            DebugMessageHDMITx("[HDMI_TX] DTD[%d][%d]",(UINT32)edidBlock, (UINT32)ucDTDNum);
            DebugMessageHDMITx("%dx%d",(UINT32)edid_timing.h_act, (UINT32)edid_timing.v_act);
            DebugMessageHDMITx("(%dx%d)MHz",(UINT32)(edid_timing.h_act+edid_timing.h_blank), (UINT32)(edid_timing.v_act+edid_timing.v_blank));
            DebugMessageHDMITx("@%d\n",(UINT32)edid_timing.pixelClk/10);
        }

        ucDTDAddress = ucDTDAddress + 18;
    }

    ScalerHdmiMacTx0RepeaterSetSinkTxSupportTimingCount(timingType, ucFoundExtDtdCnt);

    if(ucFoundExtDtdCnt == 0)
        DebugMessageHDMITx("[HDMI_TX] Not DTD be Found@Block[%d]!!\n", (UINT32)edidBlock);

    return;
}


//--------------------------------------------------
// Description  : Dump EDID features and timing mode info
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0RepeaterEdidInfoDump(void)
{
    int i;
    #ifdef ENABLE_HDMITX_DEBUG_LOG//for debug
    char mntName[2][11-5];
    memcpy(&mntName[0], &pucDdcRamAddr[0x5a+5], 11-5);
    memcpy(&mntName[1], &pucDdcRamAddr[0x6c+5], 11-5);
    NoteMessageHDMITx("[HDMITX][EDID] RX Name: %s|%s\n", mntName[0], mntName[1]);
    #endif
    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacTxPxMapping(_TX0));
    DebugMessageHDMITx("[HDMI_TX] Edid Info[%d]:\n", (UINT32)GET_HDMI_MAC_TX0_EDID_MODIFY_STATE());
#if 0
    for(i=0; i < _EDID_FEATURE_TYPE_NUM; i++){
        DebugMessageHDMITx("[HDMI_TX] [%s]", strEdidFeatureType[i]);
        DebugMessageHDMITx("=%d\n", (UINT32)ScalerHdmiMacTx0EdidGetFeature(i));
    }
#endif
    // EDID support timing info
    DebugMessageHDMITx("[HDMI_TX] DTD Info:\n");
    ScalerHdmiMacTx0RepeaterEdidDTDInfoDump(EDID_BLOCK_0);
    ScalerHdmiMacTx0RepeaterEdidDTDInfoDump(EDID_BLOCK_1);
    if(pusCtaDataBlockAddr[_DID_DTDDB]){
        ScalerHdmiMacTx0RepeaterEdidDIDExtDTDInfoDump(EDID_EXT_DID_BLOCK_1);
    }
    ScalerHdmiMacTx0RepeaterEdidVdbVicDump(_CTA_VDB, 0);
    ScalerHdmiMacTx0RepeaterEdidVdbVicDump(_CTA_420VDB, 0);
    if(ucExtVdbBlockNum){
        for(i=ucExtVdbBlockNum; i > 0; i--)
            ScalerHdmiMacTx0RepeaterEdidVdbVicDump(_CTA_VDB, i);
    }
    if(ucExt420VdbBlockNum){
        for(i=ucExt420VdbBlockNum; i > 0; i--)
            ScalerHdmiMacTx0RepeaterEdidVdbVicDump(_CTA_420VDB, i);
    }
    DebugMessageHDMITx("[HDMI_TX] %s Support Timing[%d]:\n", GET_HDMI_MAC_TX0_EDID_MODIFY_STATE()==EDID_CHECK_BF_MODIFY? "Sink": "Sink/TX", (UINT32)ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(EDID_TIMING_ANY));
    DebugMessageHDMITx("[VDB/420VDB]=%d/%d\n", (UINT32)ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(EDID_TIMING_VDB), (UINT32)ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(EDID_TIMING_420VDB));
    DebugMessageHDMITx("[DTD/EXT_DTD]=%d/%d\n", (UINT32)ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(EDID_TIMING_DTD), (UINT32)ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(EDID_TIMING_EXT_DTD));
    DebugMessageHDMITx("[DID_DTD]=%d\n", (UINT32)ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(EDID_TIMING_DID_DTD));
    DebugMessageHDMITx("[ext vdb]=%d [ext 420vdb]=%d\n", (UINT32)ucExtVdbBlockNum, (UINT32)ucExt420VdbBlockNum);
    return;
}

//--------------------------------------------------
// Description  : clear EDID support timing count
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0RepeaterClrSinkTxSupportTimingCount(void)
{
    UINT8 value_u8 = 0;
    for(value_u8=0; value_u8<EDID_TIMING_MODE_NUM; value_u8++)
        repeater_sink_tx_support_timing_cnt[value_u8]=0;

    repeater_support_timing_table.totalCnt=0;
    return;
}


/****************************************
  Description  : Set EDID support timing count for each timing DB
  Input Value  :
    edid_timing_type:
        typedef enum _EDID_TIMING_DB{
            EDID_TIMING_ANY=0,
            EDID_TIMING_VDB,
            EDID_TIMING_420VDB,
            EDID_TIMING_420CMDB,
            EDID_TIMING_DTD,
            EDID_TIMING_EXT_DTD,
            EDID_TIMING_DID_DTD,
            EDID_TIMING_MODE_NUM,
        }EDID_TIMING_DB;
    value: timing list count (0~255)
  Output Value :
    return: 0: success, 1: false
****************************************/
UINT8 ScalerHdmiMacTx0RepeaterSetSinkTxSupportTimingCount(EDID_TIMING_DB edid_timing_type, UINT8 value)
{
    if(edid_timing_type > EDID_TIMING_MODE_NUM){
        DebugMessageHDMITx("[HDMITX] INVALID Set TIMING_TYPE[%d]@SetSinkTxSupportTimingCount()\n", (UINT32)edid_timing_type);
        return 1;
    }
    repeater_sink_tx_support_timing_cnt[edid_timing_type] = value;
#if 0
    InfoMessageHDMITx("[HDMITX] support type[%x]", (UINT32)edid_timing_type) ;
    InfoMessageHDMITx("=%x\n", (UINT32)value);
#endif
    return 0;
}

/* ***************************************
  Description  : Get EDID support timing count for each timing DB
  Input Value  :
    edid_timing_type:
        typedef enum _EDID_TIMING_DB{
            EDID_TIMING_ANY=0,
            EDID_TIMING_VDB,
            EDID_TIMING_420VDB,
            EDID_TIMING_420CMDB,
            EDID_TIMING_DTD,
            EDID_TIMING_EXT_DTD,
            EDID_TIMING_DID_DTD,
            EDID_TIMING_MODE_NUM,
        }EDID_TIMING_DB;
  Output Value :
    return: Sink/TX support timing count
****************************************/
UINT8 ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(EDID_TIMING_DB edid_timing_type)
{
    UINT16 u16TmpValue = 0;
    UINT8 value_u8 = 0;
    if(edid_timing_type > EDID_TIMING_MODE_NUM){
        DebugMessageHDMITx("[HDMITX] INVALID TIMING_TYPE[%d]@GetSinkTxSupportTimingCount()\n", (UINT32)edid_timing_type);
        return 0;
    }

    if(edid_timing_type == EDID_TIMING_ANY){
#if 1
#ifdef HDMITX_SW_CONFIG
        if(lib_hdmitx_get_edidPolicy_support() != _EDID_RX_EDID_Policy_RX_from_flash){
            repeater_sink_tx_support_timing_cnt[EDID_TIMING_ANY] = repeater_support_timing_table.totalCnt;
            DebugMessageHDMITx("EDID Type[ANY] total=%x\n", (UINT32)repeater_support_timing_table.totalCnt);
        }else
#endif
	{
            u16TmpValue = 0;
            for(value_u8=EDID_TIMING_VDB; value_u8 < EDID_TIMING_MODE_NUM; value_u8++){
                u16TmpValue += repeater_sink_tx_support_timing_cnt[value_u8];
            #if 0
                InfoMessageHDMITx("EDID Type[%d]", (UINT32)i);
                InfoMessageHDMITx("=%d", (UINT32)repeater_sink_tx_support_timing_cnt[i]);
                InfoMessageHDMITx("/%d\n", (UINT32)totalCount);
            #endif
            }
            repeater_sink_tx_support_timing_cnt[EDID_TIMING_ANY] = u16TmpValue;
        }
#else
        UINT8 i, totalCount=0;
        for(i=EDID_TIMING_VDB; i < EDID_TIMING_MODE_NUM; i++){
            totalCount += repeater_sink_tx_support_timing_cnt[i];
        #if 0
            InfoMessageHDMITx("EDID Type[%d]", (UINT32)i);
            InfoMessageHDMITx("=%d", (UINT32)repeater_sink_tx_support_timing_cnt[i]);
            InfoMessageHDMITx("/%d\n", (UINT32)totalCount);
        #endif
        }
        repeater_sink_tx_support_timing_cnt[EDID_TIMING_ANY] = totalCount;
#endif
    }

    return repeater_sink_tx_support_timing_cnt[edid_timing_type];
}

/* ***************************************
  Description  : Get Sink/TX support timing list
  Input Value  :
    edid_timing_type:
    typedef enum _EDID_TIMING_DB{
        EDID_TIMING_ANY=0,
        EDID_TIMING_VDB,
        EDID_TIMING_420VDB,
        EDID_TIMING_420CMDB,
        EDID_TIMING_DTD,
        EDID_TIMING_EXT_DTD,
        EDID_TIMING_DID_DTD,
        EDID_TIMING_MODE_NUM,
    }EDID_TIMING_DB;
    index: timing index (0~255)
  Output Value :
    timing_table: return Sink/TX support timing table (hdmi_repeater_timing_gen_st *)(null mean search fail)
****************************************/

INT8 ScalerHdmiMacTx0RepeaterGetSinkTxSupportTiming(EDID_TIMING_DB edid_timing_type, UINT8 index, hdmi_tx_timing_gen_st *time_mode)
{
    UINT8 u8TmpValue = 0;
    if(time_mode == NULL){
        DebugMessageHDMITx("[HDMITX] NULL PTR@SinkTxSupportTiming\n");
        return -1;
    }

    if((edid_timing_type >= EDID_TIMING_MODE_NUM)|| (index > ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(edid_timing_type))){
        DebugMessageHDMITx("[HDMITX] EDID timing[%d] ", (UINT32)edid_timing_type);
        DebugMessageHDMITx(" index[%d] Not Exist\n", (UINT32)index);
        return -1;
    }

    u8TmpValue = ScalerHdmiMacTx0RepeaterPopSinkTxSupportTimingList(edid_timing_type, index, 0, 0);
    if(u8TmpValue < TX_TIMING_NUM){
        //time_mode = &cea_timing_table_repeater[(UINT8)tx_index];
        memcpy(time_mode, &cea_timing_table_repeater[(UINT8)u8TmpValue], sizeof(hdmi_tx_timing_gen_st));
        DebugMessageHDMITx("[HDMITX]Index[%d] ", (UINT32)u8TmpValue);
        DebugMessageHDMITx("VIC[%d/%d]",  (UINT32)time_mode->video_code1, (UINT32)time_mode->video_code2);
        DebugMessageHDMITx(" %d", (UINT32)time_mode->h.active);
        DebugMessageHDMITx(" x%d", (UINT32)time_mode->v.active);
        DebugMessageHDMITx("@%dM\n", (UINT32)time_mode->pixelFreq);
        return 0;
    }else{
        return -1;
    }
}


/****************************************
  Description  : Check video timing support by TX output or not
  Input Value  :
    ucVic: compare to TX support timing mode
  Output Value :
    return: TX_TIMING_NUM: TX not support
****************************************/
UINT8 ScalerHdmiMacTx0RepeaterTimingVicCheckSupport(UINT8 ucVic, UINT8 support420, EDID_TIMING_DB timingType)
{
    UINT8 i;
    UINT8 ret_num = TX_TIMING_NUM;
    UINT16 tmpPixelClk;
    if(ScalerHdmiMacTx0EdidGetFeature(_SCDC_PRESENT) == _FALSE)
        pixelClockLimit = TX_OUT_PIXEL_CLOCK_MAX_WO_SCDC;
    else if(ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE) == 0)
        pixelClockLimit = ((UINT16)ScalerHdmiMacTx0EdidGetFeature(_HDMI_MAX_TMDS_CLK) * 50);
    else
        pixelClockLimit = getLimitPixelClkWithDDR();

    // [FIX-ME] TX output HDMI21 mode when Sink device support HDMI21 (Need consider input source type too)
    //if(ScalerHdmiMacTxEdidGetFeature(_P0_OUTPUT_PORT, _MAX_FRL_RATE) >= _HDMI21_FRL_6G_4LANES)
        //txTimingCheckStart = TX_TIMING_1080P60;
    if(matchCDFNoSupportTiming(ucVic))
        return TX_TIMING_NUM;

    for(i=0; i<TX_TIMING_NUM; i++){
        if((ucVic != 0) && ((ucVic == cea_timing_table_repeater[i].video_code1) || (ucVic == cea_timing_table_repeater[i].video_code2))){
	   if((timingType == EDID_TIMING_VDB) && (ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE) == _HDMI21_FRL_6G_4LANES)) {
		 tmpPixelClk = cea_timing_table_repeater[i].pixelFreq;
	    }else if((timingType == EDID_TIMING_420VDB) || support420){
                tmpPixelClk = cea_timing_table_repeater[i].pixelFreq/2;
            }else{
                tmpPixelClk = cea_timing_table_repeater[i].pixelFreq;
            }
            if(!lib_hdmitx_is_hdmi_21_support() && (timingType == EDID_TIMING_VDB) && support420)
                tmpPixelClk = cea_timing_table_repeater[i].pixelFreq;
            if(tmpPixelClk <= pixelClockLimit){
                if(lib_hdmitx_is_hdmi_limit_support() && (cea_timing_table_repeater[i].pixelFreq > TX_OUT_PIXEL_CLOCK_MAX_WO_DDR)) {
#if 0//def CONFIG_RTK_HDMI_RX
                    if(!matchLimitTimingTable(cea_timing_table_repeater[i].h.active, cea_timing_table_repeater[i].v.active, cea_timing_table_repeater[i].vFreq))
                        break;
#endif
                }
                DebugMessageHDMITx("[HDMITX] VIC[%d] Support@%d\n", (UINT32)ucVic, (UINT32)i);
                ScalerHdmiMacTx0RepeaterPushSinkTxSupportTimingList(timingType, i, support420);
                ret_num = i;
                if(lib_hdmitx_get_safemode_support() == TRUE)
                    return ret_num;
            }
        }
    }
    return ret_num;
}
UINT16 calEdidBlockBaseAddr(UINT16 usDBAddress){

    if((usDBAddress/EDID_BLOCK1_TAG_BASE)==1)
        return EDID_BLOCK1_TAG_BASE;
    if((usDBAddress/EDID_BLOCK2_TAG_BASE)==1)
        return EDID_BLOCK2_TAG_BASE;
    if((usDBAddress/EDID_BLOCK3_TAG_BASE)==1)
        return EDID_BLOCK3_TAG_BASE;
    return EDID_BLOCK1_TAG_BASE;
}
void moveEdidByteForward(UINT8 ucDBLengthDeviation,UINT16 usModifyStartDBAddr, UINT16 usModifyEndDBAddr)
{
    UINT16 u16TmpValue = 0;
    UINT16 i = 0;
    u16TmpValue = usModifyEndDBAddr - ucDBLengthDeviation;
     DebugMessageHDMITx("@Addr[%x] %x %x\n",(UINT32)usModifyStartDBAddr,(UINT32)usModifyEndDBAddr,(UINT32)u16TmpValue);
    for(i = usModifyStartDBAddr; i < usModifyEndDBAddr; i++){
        if(i< u16TmpValue)
            pucDdcRamAddr[i] = pucDdcRamAddr[ucDBLengthDeviation+i];
        else
            pucDdcRamAddr[i] = 0x0;
    }
}

void adjustEdidDbOffset(UINT16 usDBAddress,UINT8 lenth)
{
    UINT16 usModifyEndDBAddr = 0;
    UINT16 usTmpBaseAddr = 0x00;
    UINT16 usModifyStartDBAddr = 0;
    UINT8 ucDBLengthDeviation = 0;
    UINT16 usExtDetailTimeBase = 0x82; // Detailed Timing address of Extension Block
    if(usDBAddress == 0x00){
        return ;
    }
    usTmpBaseAddr = calEdidBlockBaseAddr(usDBAddress);
    if(pucDdcRamAddr[usTmpBaseAddr] != EDID_EXTENSION_TAG)
    {//This function is currently only applicable to ext tags Edid Block
        return ;
    }
    // 1.modify DB lenth
    ucDBLengthDeviation = (pucDdcRamAddr[usDBAddress] & 0x1F) - lenth;
    pucDdcRamAddr[usDBAddress] &= 0xE0;
    pucDdcRamAddr[usDBAddress] |= lenth;
    DebugMessageHDMITx("DBOffset %d %d %d",(UINT32)ucDBLengthDeviation,(UINT32)lenth,(UINT32)(pucDdcRamAddr[usDBAddress] & 0x1F));
    DebugMessageHDMITx("@Addr[%x]\n",(UINT32)usDBAddress);
    // 2.modify DTD offset
    usExtDetailTimeBase = usTmpBaseAddr + 0x2;
    pucDdcRamAddr [usExtDetailTimeBase] -= ucDBLengthDeviation;


    // 3.Move DB forward
    usModifyStartDBAddr = usDBAddress + lenth + 1;
    usModifyEndDBAddr = usTmpBaseAddr + 0x7f;
    moveEdidByteForward(ucDBLengthDeviation, usModifyStartDBAddr, usModifyEndDBAddr);
    // 4. reupdate Ext DB Addr
    ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
}


/****************************************
  Description  : Filter TX not support output timing from EDID
  Input Value  :
    data_type: _CTA_VDB, _CTA_420VDB
  Output Value :
****************************************/
void modifyY420CMDB(void)
{
    UINT16 usCMDBAddress = 0;
    UINT8 ucCMDBLength = 0;

    usCMDBAddress = pusCtaDataBlockAddr[_CTA_420CMDB];
    if(usCMDBAddress == 0x00){
        return ;
    }
    ucCMDBLength = pucDdcRamAddr[usCMDBAddress] & 0x1F;
    ucCMDBLength = ucCMDBLength -1;
    if(ucCMDBLength <=0)
        return ;
    else {
        memcpy(&pucDdcRamAddr[usCMDBAddress + 2], &ucY420CMDBAdjust[0], ucCMDBLength);
    }
}

BOOLEAN decideVDBTimingIsSupport420(EnumEdidCtaDataBlock data_type,UINT8 vdb_index)
{
    UINT8 ucCMDBLength = 0;
    UINT16 usCMDBAddress = 0;
    UINT16 u16TmpValue = 0;
    UINT8 u8TmpValue = 0;
    UINT16 u16TmpValue2 = 0;
    BOOLEAN boolTmpValue = 0;
    //for _CTA_420VDB
    if(data_type == _CTA_420VDB)
        return _TRUE;

    //for _CTA_VDB
    usCMDBAddress = pusCtaDataBlockAddr[_CTA_420CMDB];
    if(usCMDBAddress == 0x00){
        return _FALSE;
    }

    ucCMDBLength = pucDdcRamAddr[usCMDBAddress] & 0x1F;
    if(ucCMDBLength <1)
        return _FALSE;
    else if(ucCMDBLength ==1)
        return _TRUE;
    else{
           u16TmpValue = vdb_index/8;
           u16TmpValue2 = vdb_index % 8;
           if(u16TmpValue < ucCMDBLength - 1)
            {
                u8TmpValue = pucDdcRamAddr[usCMDBAddress + u16TmpValue + 2];
               u8TmpValue =  u8TmpValue>>u16TmpValue2;
               boolTmpValue = u8TmpValue & 0x1;
               return boolTmpValue;
           }
    }
    return _FALSE;
}

void ScalerHdmiMacTx0RepeaterEdidModifyVDBTiming(EnumEdidCtaDataBlock data_type, UINT8 ucExtVdbBlockNum)
{
    UINT16 usEdidModifyStartAddr=0;
    UINT8 ucVDBLength = 0;
    UINT8 value_u8 = 0;
    UINT8 ucVICStartOffset = 0;
    UINT16 usVDBAddress = 0;
    UINT16 u16TmpValue = 0;
    UINT16 u16TmpValue2 = 0;
    EDID_TIMING_DB timingType;
    UINT8 ucVic;
    BOOLEAN boolTmpValue = 0;
    UINT8 ucVDBAdjustIndex = 0;
    BOOLEAN bY420CMDBAdjust = FALSE;
    UINT8 originalVic = 0;
    UINT8 tx_ret;

    if(data_type == _CTA_VDB){
        ucVICStartOffset = 1;
        timingType = EDID_TIMING_VDB;
        if(!ucExtVdbBlockNum)
            usVDBAddress = pusCtaDataBlockAddr[data_type];
        else
            usVDBAddress = pusExtVdbBlockAddr[ucExtVdbBlockNum - 1];
    }else if(data_type == _CTA_420VDB){
        ucVICStartOffset = 2;
        timingType = EDID_TIMING_420VDB;
        if(!ucExtVdbBlockNum)
            usVDBAddress = pusCtaDataBlockAddr[data_type];
        else
            usVDBAddress = pusExt420VdbBlockAddr[ucExtVdbBlockNum - 1];
    }else{
        DebugMessageHDMITx("[HDMITX] NOT SUPPORT VDB TYPE[%d]\n", (UINT32)data_type);
        return;
    }

    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacTxPxMapping(_TX0));
    memset(pucEdidVDBVIC, 0, EDID_VDB_VIC_LEN);
    memset(ucY420CMDBAdjust, 0, EDID_Y420CMDB_LEN);
    ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
    //ScalerHdmiMacTxGetEdidVicTable(ScalerHdmi21MacTxPxMapping(_TX0), _CTA_VDB, &pucEdidVDBVIC[0], pusCtaDataBlockAddr);

    ucVDBLength = pucDdcRamAddr[usVDBAddress] & 0x1F;
    if(data_type == _CTA_420VDB)
    {
         if(ucVDBLength > 0){
            ucVDBLength = ucVDBLength -1;//420VDB byte2 is tag,so need -1
         }
    }
    if(ucVDBLength){
        DebugMessageHDMITx("[HDMITX] VDB[%d] Len=%d", (UINT32)data_type, (UINT32)ucVDBLength);
        DebugMessageHDMITx("ext %d @Addr[%x]\n",(UINT32)ucExtVdbBlockNum,(UINT32)usVDBAddress);

        for(value_u8 = 0; value_u8 < ucVDBLength; value_u8++)
        {
            originalVic = pucDdcRamAddr[usVDBAddress + value_u8 + ucVICStartOffset];
            // if SVD >= 129 and SVD <= 192 then 7-BOOLEAN VIC is defined and is a native code
            if((originalVic >= 129) && (originalVic <= 192)){
                ucVic = originalVic & 0x7f;
            }else{
                ucVic = originalVic;
            }
            if(!ucExtVdbBlockNum)
                boolTmpValue = decideVDBTimingIsSupport420(data_type,value_u8);
            else{
                if(data_type == _CTA_420VDB)
                    boolTmpValue = _TRUE;
                else
                    boolTmpValue = _FALSE;
            }
            if((tx_ret = ScalerHdmiMacTx0RepeaterTimingVicCheckSupport(ucVic, boolTmpValue, timingType)) == TX_TIMING_NUM){
                usEdidModifyStartAddr = usVDBAddress + value_u8 + ucVICStartOffset;
                pucDdcRamAddr[usEdidModifyStartAddr] = 0x1;
                DebugMessageHDMITx("[HDMITX] SVD[%d]:", (UINT32)value_u8);
                DebugMessageHDMITx("VIC[%d] Not Support\n", (UINT32)ucVic);
            }else{
#ifdef ENABLE_INTERNAL_HDMIRX_PTG
                if(ucVic == 97){
                    bSVDSupport4K60 = 1;
                }
#endif
                //ScalerHdmiMacTx0RepeaterPushSinkTxSupportTimingList(timingType, tx_ret,boolTmpValue);
                pucEdidVDBVIC[ucVDBAdjustIndex] = originalVic;
                if(data_type == _CTA_VDB && boolTmpValue){//vdb && 420 means 420cmdb need modify
                    u16TmpValue = ucVDBAdjustIndex/8;
                    u16TmpValue2 = ucVDBAdjustIndex % 8;
                    ucY420CMDBAdjust[u16TmpValue]|=0x1<<u16TmpValue2;
                    bY420CMDBAdjust = TRUE;
                }
                ucVDBAdjustIndex++;
            }
        }
    }
    if(usEdidModifyStartAddr){
            memcpy(&pucDdcRamAddr[usVDBAddress + ucVICStartOffset], &pucEdidVDBVIC[0], ucVDBLength);
             if(data_type == _CTA_420VDB){
                ucVDBLength = ucVDBAdjustIndex + 1;
             }else{
                ucVDBLength = ucVDBAdjustIndex;
             }
            adjustEdidDbOffset(usVDBAddress, ucVDBLength);
            if(bY420CMDBAdjust){
                modifyY420CMDB();
            }
    }
    return;
}

UINT16 calcDTDTimingvFreq(void)
{
    UINT16 htotal = 0;
    UINT16 vtotal = 0;
    UINT16 vfreq = 0;
    UINT32 u32TmpValue = 0;
    //compute vfreq is 0.1 precision
    htotal = edid_timing.h_act + edid_timing.h_blank;
    vtotal = edid_timing.v_act + edid_timing.v_blank;
    u32TmpValue = (UINT32)htotal * vtotal / 10;
    vfreq = (UINT16)((UINT32)edid_timing.pixelClk * 10000 / u32TmpValue);
    return vfreq;
}

/****************************************
  Description  : Check TX support timing mode
  Input Value  :
    input: pixelClk, h_act, v_act
  Output Value :
    return: TX_TIMING_NUM: TX not support
****************************************/

UINT8 ScalerHdmiMacTx0RepeaterTimingDtdCheckSupport(hdmi_timing_check_type *check_timing, EDID_TIMING_DB timingType)
{
    UINT8 i,txTimingCheckStart=0;
    UINT8 match = TX_TIMING_NUM;
    UINT16 dtd_vfeq = 0;
    UINT32 u32TmpValue = 0;
    BOOLEAN boolTmpValue = 0;

    if(check_timing == NULL){
        FatalMessageHDMITx("[HDMITX] NULL PTR@DtdCheckSupport()\n");
        return TX_TIMING_NUM;
    }

    if(ScalerHdmiMacTx0EdidGetFeature(_SCDC_PRESENT) == _FALSE)
        pixelClockLimit = TX_OUT_PIXEL_CLOCK_MAX_WO_SCDC;
    else if(ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE) == 0)
        pixelClockLimit = ((UINT16)ScalerHdmiMacTx0EdidGetFeature(_HDMI_MAX_TMDS_CLK) * 50);
    else
        pixelClockLimit = getLimitPixelClkWithDDR();


    // [FIX-ME] TX output HDMI21 mode when Sink device support HDMI21 (Need consider input source type too)
    //if(ScalerHdmiMacTxEdidGetFeature(_P0_OUTPUT_PORT, _MAX_FRL_RATE) >= _HDMI21_FRL_6G_4LANES)
        //txTimingCheckStart = TX_TIMING_1080P60;
    dtd_vfeq = calcDTDTimingvFreq();

    if(lib_hdmitx_is_hdmi_limit_support() && (edid_timing.pixelClk/10 > TX_OUT_PIXEL_CLOCK_MAX_WO_DDR)) {
#if 0//def CONFIG_ENABLE_HDMIRX
        if(!matchLimitTimingTable(edid_timing.h_act, edid_timing.v_act, dtd_vfeq))
            return TX_TIMING_NUM;
#endif
    }

    for(i=txTimingCheckStart; i<TX_TIMING_NUM; i++){

        u32TmpValue = (UINT32)ABS(cea_timing_table_repeater[i].vFreq, dtd_vfeq) * 1000; // v_freq diff < 0.9%
        boolTmpValue = ((u32TmpValue / cea_timing_table_repeater[i].vFreq) < HDMITX_CLK_DIFF);  // v_freq diff < 0.9%
        boolTmpValue = boolTmpValue && (check_timing->h_act == cea_timing_table_repeater[i].h.active);
        boolTmpValue = boolTmpValue && (check_timing->v_act == cea_timing_table_repeater[i].v.active);
        boolTmpValue = boolTmpValue && (check_timing->pixelClk / 10 <= pixelClockLimit);
        if(boolTmpValue){
            if(matchCDFNoSupportTiming(cea_timing_table_repeater[i].video_code1))
                continue;
            //InfoMessageHDMITx("[HDMITX] DTD[%dx",(UINT32)check_timing->h_act);
            //InfoMessageHDMITx("%d", (UINT32)check_timing->v_act);
            //InfoMessageHDMITx("(%d)",(UINT32)(check_timing->h_act+check_timing->h_blank));
            //InfoMessageHDMITx("(x%d)", (UINT32)(check_timing->v_act+check_timing->v_blank));
            DebugMessageHDMITx("@%d/%d ",(UINT32)check_timing->pixelClk / 10, (UINT32)dtd_vfeq);
            DebugMessageHDMITx("(/%d)] ", (UINT32)pixelClockLimit);
            DebugMessageHDMITx("Support@%d\n",(UINT32)i);
#ifdef ENABLE_INTERNAL_HDMIRX_PTG
            UINT8 tx_ret;
            if((tx_ret == HDMI_TX_3840_2160P_60HZ) ||(tx_ret == HDMI_TX_3840_2160P_120HZ)){
                bDtdSupport4K60 = 1;
            }
#endif
            ScalerHdmiMacTx0RepeaterPushSinkTxSupportTimingList(timingType, i,_FALSE);
            match = 1;
        }
    }

    return match;
}

/****************************************
  Description  : Filter TX not support output timing from EDID
  Input Value  :
    edidBlock: EDID_BLOCK_0(DTD), EDID_BLOCK_1(EXT_DTD)
  Output Value :
****************************************/
void ScalerHdmiMacTx0RepeaterEdidModifyDTDTiming_forDisplayIDExtBlock(EDID_BLOCK_INDEX edidBlock)
{
    UINT16 usEdidModifyStartAddr=0;
    UINT16 usModifyEndDBAddr = 0;
    UINT16 usModifyStartDBAddr = 0;
    UINT8 ucDBLengthDeviation = 0;
    UINT8 ucDTDNum;
    UINT16 ucDTDAddress;
    UINT16 usExtDetailTimeBase = 0x82; // Detailed Timing address of Extension Block
    EDID_TIMING_DB timingType;
    UINT8 ucFoundExtDtdCnt=0;
    UINT8 ucExtDtdNum;
    UINT8 tx_ret;

    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacTxPxMapping(_TX0));

    // 1.usExtDetailTimeBase:tag = 0x3 baseaddr 2. first ucDTDAddress 3.ucExtDtdNum 4. timingType
    usExtDetailTimeBase = pusCtaDataBlockAddr[_DID_DTDDB];;//modify ExtensionTag Block DTD
    ucDTDAddress = usExtDetailTimeBase + 3;
    ucExtDtdNum = pucDdcRamAddr[usExtDetailTimeBase + 2] / 20;
    timingType = EDID_TIMING_DID_DTD;


    DebugMessageHDMITx("[HDMI_TX][Modify] DTD[%d] Num=%d",(UINT32)edidBlock, (UINT32)ucExtDtdNum);
    DebugMessageHDMITx("@ADDR[%x(/%x)]\n",(UINT32)ucDTDAddress, (UINT32)GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR());
    for(ucDTDNum = 0; ucDTDNum < ucExtDtdNum; ucDTDNum++)
    {
        get20ByteDTDTimingInfo(ucDTDAddress);
        if (lib_hdmitx_Golden_Timing_adjust(ucDTDAddress)) {
              usEdidModifyStartAddr = ucDTDAddress; // for checksum re-calculate
        }
        if(edid_timing.pixelClk && edid_timing.h_act && edid_timing.v_act){
            if((tx_ret = ScalerHdmiMacTx0RepeaterTimingDtdCheckSupport(&edid_timing, timingType)) == TX_TIMING_NUM){
                usEdidModifyStartAddr = ucDTDAddress;
                //NoteMessageHDMITx("[HDMI_TX] Not Support: DTD[%d][%d]",(UINT32)edidBlock, (UINT32)ucDTDNum);
                //NoteMessageHDMITx(" %dx%d",(UINT32)edid_timing.h_act, (UINT32)edid_timing.v_act);
                //NoteMessageHDMITx("(%dx%d)", (UINT32)(edid_timing.h_act+edid_timing.h_blank), (UINT32)(edid_timing.v_act+edid_timing.v_blank));
                //NoteMessageHDMITx("@%dMHz\n",(UINT32)edid_timing.pixelClk/10);
                usModifyStartDBAddr = ucDTDAddress;
                ucDBLengthDeviation = 20;
                usModifyEndDBAddr = usDidSExtEndAddr;
                moveEdidByteForward(ucDBLengthDeviation, usModifyStartDBAddr, usModifyEndDBAddr);
                continue;

            }
            ucFoundExtDtdCnt++;
        }

        ucDTDAddress = ucDTDAddress + 20;
    }

    // modify DTD length = DTDNum x 20
    pucDdcRamAddr[usExtDetailTimeBase + 2] = ucFoundExtDtdCnt * 20;

    if(ucFoundExtDtdCnt == 0){
        DebugMessageHDMITx("[HDMI_TX][Modify] Not DTD be Found@Block[%d]!!\n", (UINT32)edidBlock);
    }
    if(usEdidModifyStartAddr){
        pucDdcRamAddr[usDidSExtEndAddr] = ScalerEdidDidCheckSumCal(usDidSExtStartAddr, usDidSExtEndAddr);
    }
    return;
}

void ScalerHdmiMacTx0RepeaterEdidModifyDTDTiming(EDID_BLOCK_INDEX edidBlock)
{
    //UINT16 usEdidModifyStartAddr=0;
    UINT16 usModifyEndDBAddr = 0;
    UINT16 usModifyStartDBAddr = 0;
    UINT8 ucDTDNum;
    UINT16 ucDTDAddress;
    UINT16 usExtDetailTimeBase = 0x82; // Detailed Timing address of Extension Block
    UINT16 usDetailTimeOffset = 0x00;
    UINT8 ucDBLengthDeviation = 0;
    EDID_TIMING_DB timingType;
    UINT8 ucFoundExtDtdCnt=0;
    UINT8 ucExtDtdNum;
    UINT8 tx_ret;

    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacTxPxMapping(_TX0));

    if(edidBlock == EDID_BLOCK_0){
        ucDTDAddress = 0x36;
        ucExtDtdNum = 4;
        timingType = EDID_TIMING_DTD;
    }else if(edidBlock == EDID_BLOCK_1){
        usExtDetailTimeBase = usExtTagBaseAddr + 0x2;//modify ExtensionTag Block DTD
        usDetailTimeOffset = usExtTagBaseAddr + pucDdcRamAddr[usExtDetailTimeBase];
        ucDTDAddress = usDetailTimeOffset;
        ucExtDtdNum = (usExtTagBaseAddr + 0x7f - ucDTDAddress) / 18;
        timingType = EDID_TIMING_EXT_DTD;
    }else{
        DebugMessageHDMITx("[HDMI_TX][Modify] DTD[%d] Not Support Yet!!\n", (UINT32)edidBlock);
        return;
    }

    DebugMessageHDMITx("[HDMI_TX][Modify] DTD[%d] Num=%d",(UINT32)edidBlock, (UINT32)ucExtDtdNum);
    DebugMessageHDMITx("@ADDR[%x(/%x)]\n",(UINT32)ucDTDAddress, (UINT32)GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR());
    for(ucDTDNum = 0; ucDTDNum < ucExtDtdNum; ucDTDNum++)
    {


        getDTDTimingInfo(ucDTDAddress);
        if(edid_timing.pixelClk && edid_timing.h_act && edid_timing.v_act){
            ucFoundExtDtdCnt++;
            if((tx_ret = ScalerHdmiMacTx0RepeaterTimingDtdCheckSupport(&edid_timing, timingType)) == TX_TIMING_NUM){
                //usEdidModifyStartAddr = ucDTDAddress;
                //NoteMessageHDMITx("[HDMI_TX] Not Support: DTD[%d][%d]",(UINT32)edidBlock, (UINT32)ucDTDNum);
                //NoteMessageHDMITx(" %dx%d",(UINT32)edid_timing.h_act, (UINT32)edid_timing.v_act);
                //NoteMessageHDMITx("(%dx%d)", (UINT32)(edid_timing.h_act+edid_timing.h_blank), (UINT32)(edid_timing.v_act+edid_timing.v_blank));
                //NoteMessageHDMITx("@%dMHz\n",(UINT32)edid_timing.pixelClk/10);
                if(edidBlock == EDID_BLOCK_0)
                    memset(&pucDdcRamAddr[ucDTDAddress], 0, 0x12);
                else{
                    usModifyStartDBAddr = ucDTDAddress;
                    ucDBLengthDeviation = 18;
                    usModifyEndDBAddr = usExtTagBaseAddr + 0x7f;
                    moveEdidByteForward(ucDBLengthDeviation, usModifyStartDBAddr, usModifyEndDBAddr);
                    continue;
                }
            }
        }

        ucDTDAddress = ucDTDAddress + 18;
    }

    if(ucFoundExtDtdCnt == 0){
        DebugMessageHDMITx("[HDMI_TX][Modify] Not DTD be Found@Block[%d]!!\n", (UINT32)edidBlock);
    }

    return;
}


/****************************************
  Description  : Filter TX not support output timing from EDID
  Input Value  :
  Output Value :
****************************************/
void ScalerHdmiMacTx0RepeaterEdidModifyTiming(void)
{
    int i;
    // filter EDID video timing not list on cea_timing_table_repeater[]
    // filter Block[0] DTD timing table(0x36)
    if(ScalerHdmiMacTx0RepeaterGetEdidTimingSupport(_TXOUT_ALL_DTD_SUPPORT) == _FALSE)
        ScalerHdmiMacTx0RepeaterEdidModifyDTDTiming(EDID_BLOCK_0);

    // filter Block[1] (EXt)DTD timing table(0x80 + value[0x82])
    if(ScalerHdmiMacTx0RepeaterGetEdidTimingSupport(_TXOUT_ALL_EXT_DTD_SUPPORT) == _FALSE)
        ScalerHdmiMacTx0RepeaterEdidModifyDTDTiming(EDID_BLOCK_1);

    if((ScalerHdmiMacTx0RepeaterGetEdidTimingSupport(_TXOUT_ALL_DID_T1_SUPPORT) == _FALSE) && (pusCtaDataBlockAddr[_DID_DTDDB] != 0X0))
        ScalerHdmiMacTx0RepeaterEdidModifyDTDTiming_forDisplayIDExtBlock(EDID_EXT_DID_BLOCK_1);

    // filter SVD timing table(0x84)
    if(ScalerHdmiMacTx0RepeaterGetEdidTimingSupport(_TXOUT_ALL_VDB_SUPPORT) == _FALSE){
        ScalerHdmiMacTx0RepeaterEdidModifyVDBTiming(_CTA_VDB, 0);
        if(ucExtVdbBlockNum){
            for(i=ucExtVdbBlockNum; i > 0; i--)
                ScalerHdmiMacTx0RepeaterEdidModifyVDBTiming(_CTA_VDB, i);
        }
    }
    if(ScalerHdmiMacTx0RepeaterGetEdidTimingSupport(_TXOUT_ALL_420VDB_SUPPORT) == _FALSE){
        ScalerHdmiMacTx0RepeaterEdidModifyVDBTiming(_CTA_420VDB, 0);
        if(ucExt420VdbBlockNum){
            for(i=ucExt420VdbBlockNum; i > 0; i--)
                ScalerHdmiMacTx0RepeaterEdidModifyVDBTiming(_CTA_420VDB, i);
        }
    }

    return;
}


/****************************************
  Description  : Modify EDID features from EDID buffer
  Input Value  :
    enumEdidFeature: modify EDID feature
    ucValue: modify value
  Output Value :
****************************************/
void ScalerHdmiMacTx0RepeaterEdidModifyFeature(EnumEdidFeatureType enumEdidFeature, UINT8 ucValue)
{
    UINT8 value_u8 = 0;
    UINT16 usDbStartAddr = 0x00;
    UINT16 usEdidModifyStartAddr=0;

    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacTxPxMapping(_TX0));

    if(enumEdidFeature >= _EDID_FEATURE_TYPE_NUM){
        DebugMessageHDMITx("[HDMI_TX] INVALID EDID Feature[%d]\n", (UINT32)enumEdidFeature);
        return;
    }

    //NoteMessageHDMITx("[HDMI_TX][Modify] EDID Feature[%s]", strEdidFeatureType[enumEdidFeature]);
    //NoteMessageHDMITx("=%d\n", (UINT32)ucValue);

    switch(enumEdidFeature)
    {
        case _EDID_VERSION:
            usEdidModifyStartAddr = 0x12;
            pucDdcRamAddr[0x12] = (ucValue == _EDID_VER20? 2: 1);///main version
            pucDdcRamAddr[0x13] = (ucValue == _EDID_VER20? 0: ucValue == _EDID_VER14? 4: 3);//sub version
            break;

        case _YCC444_SUPPORT:
        case _YCC422_SUPPORT:
            // YCC 444
            usEdidModifyStartAddr = usCtaExtBasicSupportAddr;
            value_u8 = pucDdcRamAddr[usCtaExtBasicSupportAddr];
            if(enumEdidFeature == _YCC444_SUPPORT) // YCC 444
                pucDdcRamAddr[usCtaExtBasicSupportAddr] = ((value_u8 & ~_BIT5)| (ucValue? _BIT5: 0)); // BIT5: Support Y444
            else if(enumEdidFeature == _YCC422_SUPPORT) // YCC 422
                pucDdcRamAddr[usCtaExtBasicSupportAddr] = ((value_u8 & ~_BIT4)| (ucValue? _BIT4: 0)); // BIT4: Support Y422
            break;

        // _CTA_HDMI_VSDB
        case _DC_Y444_SUPPORT:
        case _AI_SUPPORT:
        case _DC48_SUPPORT:
        case _DC36_SUPPORT:
        case _DC30_SUPPORT:
            ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
            usDbStartAddr = pusCtaDataBlockAddr[_CTA_HDMI_VSDB];
            usEdidModifyStartAddr = usDbStartAddr;
            if(usDbStartAddr){
                value_u8 = pucDdcRamAddr[(usDbStartAddr + 6)];

                if(enumEdidFeature == _AI_SUPPORT) // AI
                    pucDdcRamAddr[(usDbStartAddr + 6)] = ((value_u8 & ~_BIT7)| (ucValue? _BIT7: 0)); // BIT7: Support AI
                else if(enumEdidFeature == _DC48_SUPPORT) // DC48
                    pucDdcRamAddr[(usDbStartAddr + 6)] = ((value_u8 & ~_BIT6)| (ucValue? _BIT6: 0)); // BIT6: Support DP 48 bits
                else if(enumEdidFeature == _DC36_SUPPORT) // DC36
                    pucDdcRamAddr[(usDbStartAddr + 6)] = ((value_u8 & ~_BIT5)| (ucValue? _BIT5: 0)); // BIT5: Support DP 36 bits
                else if(enumEdidFeature == _DC30_SUPPORT) // DC30
                    pucDdcRamAddr[(usDbStartAddr + 6)] = ((value_u8 & ~_BIT4)| (ucValue? _BIT4: 0)); // BIT4: Support DP 30 bits
                else if(enumEdidFeature == _DC_Y444_SUPPORT) // DC Y444
                    pucDdcRamAddr[(usDbStartAddr + 6)] = ((value_u8 & ~_BIT3)| (ucValue? _BIT3: 0)); // BIT3: Support DC_Y444
            }
            break;

        case _SCDC_PRESENT:
        case _RR_CAPABLE:
        case _LTE_340MCSC_SCRAMBLE:
        case _Independent_view:
        case _Dual_View:
        case _3D_OSD_Disparity:
            ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
            if(pusCtaDataBlockAddr[_CTA_HF_SCDB])
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_SCDB];
            else
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_VSDB];
            usEdidModifyStartAddr = usDbStartAddr;
            if(usDbStartAddr){
                value_u8 = pucDdcRamAddr[(usDbStartAddr + 6)];

                if(enumEdidFeature == _SCDC_PRESENT) // _SCDC_PRESENT
                    pucDdcRamAddr[(usDbStartAddr + 6)] = ((value_u8 & ~_BIT7)| (ucValue? _BIT7: 0)); // BIT7: Support SCDC
                else if(enumEdidFeature == _RR_CAPABLE) // _RR_CAPABLE
                    pucDdcRamAddr[(usDbStartAddr + 6)] = ((value_u8 & ~_BIT6)| (ucValue? _BIT6: 0)); // BIT6: Support RR
                else if(enumEdidFeature == _LTE_340MCSC_SCRAMBLE) // _LTE_340MCSC_SCRAMBLE
                    pucDdcRamAddr[(usDbStartAddr + 6)] = ((value_u8 & ~_BIT3)| (ucValue? _BIT3: 0)); // BIT3: Support 340MCSC scramble
                else if(enumEdidFeature == _Independent_view) // _Independent_view
                    pucDdcRamAddr[(usDbStartAddr + 6)] = ((value_u8 & ~_BIT2)| (ucValue? _BIT2: 0)); // BIT1: Support _Dual_View

                else if(enumEdidFeature == _Dual_View) // _Dual_View
                    pucDdcRamAddr[(usDbStartAddr + 6)] = ((value_u8 & ~_BIT1)| (ucValue? _BIT1: 0)); // BIT1: Support _Dual_View
                else if(enumEdidFeature == _3D_OSD_Disparity) // _3D_OSD_Disparity
                    pucDdcRamAddr[(usDbStartAddr + 6)] = ((value_u8 & ~_BIT0)| (ucValue? _BIT0: 0)); // BIT0: Support _3D_OSD_Disparity


            }
            break;

        case _DC48_420_SUPPORT:
        case _DC36_420_SUPPORT:
        case _DC30_420_SUPPORT:
        case _MAX_FRL_RATE:
            ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
            if(pusCtaDataBlockAddr[_CTA_HF_SCDB])
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_SCDB];
            else
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_VSDB];
            usEdidModifyStartAddr = usDbStartAddr;
            if(usDbStartAddr){
                value_u8 = pucDdcRamAddr[(usDbStartAddr + 7)];

                if(enumEdidFeature == _DC48_420_SUPPORT) // _DC48_420_SUPPORT
                    pucDdcRamAddr[(usDbStartAddr + 7)] = ((value_u8 & ~_BIT2)| (ucValue? _BIT2: 0)); // BIT2: Support DC48
                else if(enumEdidFeature == _DC36_420_SUPPORT) // _DC36_420_SUPPORT
                    pucDdcRamAddr[(usDbStartAddr + 7)] = ((value_u8 & ~_BIT1)| (ucValue? _BIT1: 0)); // BIT1: Support DC36
                else if(enumEdidFeature == _DC30_420_SUPPORT) // _DC30_420_SUPPORT
                    pucDdcRamAddr[(usDbStartAddr + 7)] = ((value_u8 & ~_BIT0)| (ucValue? _BIT0: 0)); // BIT0: Support DC30
                else if(enumEdidFeature == _MAX_FRL_RATE) // _MAX_FRL_RATE
                    pucDdcRamAddr[(usDbStartAddr + 7)] = ((value_u8 & ~(_BIT7 | _BIT6 | _BIT5 | _BIT4))| ((ucValue & 0xf) << 4)); // BIT[7:4]: MAX_FRL_RATE
            }
            break;

        case _FAPA_End_Extended:
        case _QMS:
        case _MDelta:
        case _CINEMAVRR:
        case _NEG_MVRR:
        case _FVA:
            ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
            if(pusCtaDataBlockAddr[_CTA_HF_SCDB])
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_SCDB];
            else
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_VSDB];
            usEdidModifyStartAddr = usDbStartAddr;
            if(usDbStartAddr){
                value_u8 = pucDdcRamAddr[(usDbStartAddr + 8)];

                if(enumEdidFeature == _FAPA_End_Extended) // _FAPA_End_Extended
                    pucDdcRamAddr[(usDbStartAddr + 8)] = ((value_u8 & ~_BIT7)| (ucValue? _BIT7: 0)); // BIT2: Support _FAPA_End_Extended
                else if(enumEdidFeature == _QMS) // _QMS
                    pucDdcRamAddr[(usDbStartAddr + 8)] = ((value_u8 & ~_BIT6)| (ucValue? _BIT6: 0)); // BIT1: Support _QMS
                else if(enumEdidFeature == _MDelta) // _MDelta
                    pucDdcRamAddr[(usDbStartAddr + 8)] = ((value_u8 & ~_BIT5)| (ucValue? _BIT5: 0)); // BIT0: Support _MDelta
                else if(enumEdidFeature == _CINEMAVRR) // _CINEMAVRR
                    pucDdcRamAddr[(usDbStartAddr + 8)] = ((value_u8 & ~_BIT4)| (ucValue? _BIT4: 0)); // BIT0: Support _CINEMAVRR
                else if(enumEdidFeature == _NEG_MVRR) // _NEG_MVRR
                    pucDdcRamAddr[(usDbStartAddr + 8)] = ((value_u8 & ~_BIT3)| (ucValue? _BIT3: 0)); // BIT0: Support _NEG_MVRR
                else if(enumEdidFeature == _FVA) // _FVA
                    pucDdcRamAddr[(usDbStartAddr + 8)] = ((value_u8 & ~_BIT2)| (ucValue? _BIT2: 0)); // BIT0: Support _FVA

            }
            break;
        case _VRR_MIN:
        case _VRR_MAX:
            ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
            if(pusCtaDataBlockAddr[_CTA_HF_SCDB])
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_SCDB];
            else
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_VSDB];
            usEdidModifyStartAddr = usDbStartAddr;
            if(usDbStartAddr && ((pucDdcRamAddr[usDbStartAddr] & 0x1f) >= 10)){
                if(enumEdidFeature == _VRR_MIN){
                    pucDdcRamAddr[(usDbStartAddr + 9)] = ucValue & 0x3f; // Byte_9 B[5:0] _VRR_MIN
                }else if(enumEdidFeature == _VRR_MAX){
                    pucDdcRamAddr[(usDbStartAddr + 10)] = ucValue; // (Byte_9 B[7:6] | Byte_10 B[7:0]) _VRR_MAX (VRR_MAX support < 255hz)
                }
            }
            break;

        case _DSC_VERSION_12:
        case _DSC_NATIVE_420:
        case _DSC_ALL_BPP:
        case _DSC_16BPC:
        case _DSC_12BPC:
        case _DSC_10BPC:
            ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
            if(pusCtaDataBlockAddr[_CTA_HF_SCDB])
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_SCDB];
            else
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_VSDB];
            usEdidModifyStartAddr = usDbStartAddr;
            if(usDbStartAddr){
                value_u8 = pucDdcRamAddr[(usDbStartAddr + 11)];
                value_u8 = (value_u8 & ~( _BIT5 | _BIT4)); // BIT4 BIT5: not used
                 if(enumEdidFeature == _DSC_VERSION_12){
                    pucDdcRamAddr[(usDbStartAddr + 11)] = ((value_u8 & ~_BIT7)| (ucValue? _BIT7: 0)); // BIT7: Support DSC_VERSION_12
                }else if(enumEdidFeature == _DSC_NATIVE_420){
                    pucDdcRamAddr[(usDbStartAddr + 11)] = ((value_u8 & ~_BIT6)| (ucValue? _BIT6: 0)); // BIT6: Support _DSC_NATIVE_420
                }else if(enumEdidFeature == _DSC_ALL_BPP){
                    pucDdcRamAddr[(usDbStartAddr + 11)] = ((value_u8 & ~_BIT3)| (ucValue? _BIT3: 0)); // BIT3: Support _DSC_ALL_BPP
                }else if(enumEdidFeature == _DSC_16BPC){
                    pucDdcRamAddr[(usDbStartAddr + 11)] = ((value_u8 & ~_BIT2)| (ucValue? _BIT2: 0)); // BIT2: Support _DSC_16BPC
                }else if(enumEdidFeature == _DSC_12BPC){
                    pucDdcRamAddr[(usDbStartAddr + 11)] = ((value_u8 & ~_BIT1)| (ucValue? _BIT1: 0)); // BIT1: Support _DSC_12BPC
                }else if(enumEdidFeature == _DSC_10BPC){
                    pucDdcRamAddr[(usDbStartAddr + 11)] = ((value_u8 & ~_BIT0)| (ucValue? _BIT0: 0)); // BIT0: Support _DSC_10BPC
                }
            }
            break;

        case _DSC_MAX_FRL_RATE:
        case _DSC_MAX_SLICES:
        case _DSC_TOTAL_CHUNKKBYTE:
            ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
            if(pusCtaDataBlockAddr[_CTA_HF_SCDB])
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_SCDB];
            else
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_VSDB];
            usEdidModifyStartAddr = usDbStartAddr;
            if(usDbStartAddr){
                if(enumEdidFeature == _DSC_MAX_FRL_RATE){
                    value_u8 = pucDdcRamAddr[(usDbStartAddr + 12)];
                    pucDdcRamAddr[(usDbStartAddr + 12)] = ((value_u8 & ~(_BIT7 | _BIT6 | _BIT5 | _BIT4))| ((ucValue & 0xf) << 4)); // BIT[7:4]: DSC_MAX_FRL_RATE
               }else if(enumEdidFeature == _DSC_MAX_SLICES){
                    value_u8 = pucDdcRamAddr[(usDbStartAddr + 12)];
                    pucDdcRamAddr[(usDbStartAddr + 12)] = ((value_u8 & ~(_BIT3 | _BIT2 | _BIT1 | _BIT0))| (ucValue & 0xf)); // BIT[3:0]: _DSC_MAX_SLICES

               }else if(enumEdidFeature == _DSC_TOTAL_CHUNKKBYTE){
                    value_u8 = pucDdcRamAddr[(usDbStartAddr + 13)];
                    pucDdcRamAddr[(usDbStartAddr + 13)] = ucValue;

               }
            }
            break;

        case _EDID_EXT_BLOCK_COUNT:
            ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
            usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_EEODB];
            usEdidModifyStartAddr = usDbStartAddr;
            if(usDbStartAddr){
                value_u8 = pucDdcRamAddr[usDbStartAddr+2]; // EEODB count
                pucDdcRamAddr[usDbStartAddr+2] = ucValue ; // EEODB Byte[2]: Length (set to N if repeater not support EEODB more than N blocks: ex: N=3, support EDID total=4 blocks)
                InfoMessageHDMITx("[HDMI_TX][Modify] HF-EEODB Cnt =%x->", (UINT32)value_u8);
                InfoMessageHDMITx("%x\n", (UINT32)pucDdcRamAddr[usDbStartAddr+2]);
            }
            break;

        case _HDMI_MAX_TMDS_CLK:
            // _CTA_HDMI_VSDB
            ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
            usDbStartAddr = pusCtaDataBlockAddr[_CTA_HDMI_VSDB];
            usEdidModifyStartAddr = usDbStartAddr;
            if(usDbStartAddr){
                pucDdcRamAddr[(usDbStartAddr + 7)] = ucValue; // HDMI_MAX_TMDS_CLK
                InfoMessageHDMITx("[HDMI_TX][Modify] HDMI VSDB TMDS_CLK@Addr[%x]\n", (UINT32)usEdidModifyStartAddr);
            }

            // _CTA_HF_VSDB
            ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
            if(pusCtaDataBlockAddr[_CTA_HF_SCDB])
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_SCDB];
            else
                usDbStartAddr = pusCtaDataBlockAddr[_CTA_HF_VSDB];
            usEdidModifyStartAddr = usDbStartAddr;
            if(usDbStartAddr){
                pucDdcRamAddr[(usDbStartAddr + 5)] = ucValue; // HDMI_MAX_TMDS_CLK
                InfoMessageHDMITx("[HDMI_TX][Modify] HF VSDB TMDS_CLK@Addr[%x]\n", (UINT32)usEdidModifyStartAddr);
            }
            break;

        case _YCC420_SUPPORT:
            g_stHdmiMacTx0EDIDInfo.b1YCbCr420Support = ucValue;
            DebugMessageHDMITx("[HDMI_TX] EDID Support420@_YCC420_SUPPORT\n");
            break;
        case _BT2020_cYCC:
        case _opRGB:
        case _opYCC601:
        case _ST2113RGB23:
        case _ICTCP:
            ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
            usDbStartAddr = pusCtaDataBlockAddr[_CTA_CDB];
            usEdidModifyStartAddr = usDbStartAddr;
            if(usDbStartAddr){

                if(enumEdidFeature == _BT2020_cYCC) {// _BT2020_cYCC
                    value_u8 = pucDdcRamAddr[(usDbStartAddr + 2)];
                    pucDdcRamAddr[(usDbStartAddr + 2)] = ((value_u8 & ~_BIT5)| (ucValue? _BIT5: 0));
                }else if(enumEdidFeature == _opRGB){ // _opRGB
                    value_u8 = pucDdcRamAddr[(usDbStartAddr + 2)];
                    pucDdcRamAddr[(usDbStartAddr + 2)] = ((value_u8 & ~_BIT4)| (ucValue? _BIT4: 0));
                }else if(enumEdidFeature == _opYCC601){ // _opYCC601
                    value_u8 = pucDdcRamAddr[(usDbStartAddr + 2)];
                    pucDdcRamAddr[(usDbStartAddr + 2)] = ((value_u8 & ~_BIT3)| (ucValue? _BIT3: 0));
                } else if(enumEdidFeature == _ST2113RGB23) {// _ST2113RGB23
                    value_u8 = pucDdcRamAddr[(usDbStartAddr + 3)];
                    pucDdcRamAddr[(usDbStartAddr + 3)] = ((value_u8 & ~_BIT7)| (ucValue? _BIT7: 0));
                }else if(enumEdidFeature == _ICTCP) {// _ICTCP
                    value_u8 = pucDdcRamAddr[(usDbStartAddr + 3)];
                    pucDdcRamAddr[(usDbStartAddr + 3)] = ((value_u8 & ~_BIT6)| (ucValue? _BIT6: 0));
                }
            }
            break;
        case _VSD_8K_SUPPORT:
            // 8K timing will  be filtered on ScalerHdmiMacTx0RepeaterEdidModifyTiming()
            break;
        case _LATENCY_FIELD_PRESENT:
        case _ILATENCY_FIELD_PRESENT:
            break;
        case _HDMI_VIDEO_PRESENT:
        case _DID_EXT_SUPPORT:
        case _DRR_SUPPORT:
        case _VENDOR_MODEL:
        case _EDID_SEG_READ_SUPPORT:
        case _FIRST_EXT_DTD_ADDR:
        case _LPCM_FS_SUPPORT:
        case _3D_LPCM_FS_SUPPORT:
        case _VCDB_QY:
        case _VCDB_QS:
        case _SRC_PHYSICAL_ADDR:
        default:
           DebugMessageHDMITx("[HDMITX]Feature[%d]IS NOt SUPPORTED!\n", (UINT32)enumEdidFeature);
            break;
    }

}

void ScalerHdmiMacTx0RepeaterEdidSupportDump(void)
{
#if 0
    UINT8 i;

    DebugMessageHDMITx("\n[HDMI_TX] REPEATER NOT SUPPORT EDID FEATURES:\n");
    for(i=0; i<_EDID_FEATURE_TYPE_NUM; i++){
        if(repeater_edid_support_table[i] == _FALSE)
            NoteMessageHDMITx("[%d] %s\n", (UINT32)i, strEdidFeatureType[i]);
    }
    DebugMessageHDMITx("-------------------------------\n\n");
#endif
    return;
}


/****************************************
  Description  : Config repeater EDID support capabilities
  Input Value  :
  Output Value :
****************************************/
void ScalerHdmiMacTx0RepeaterEdidSupportConfig(void)
{
    UINT8 i;

    // TX timing support control
    if(lib_hdmitx_is_VRR_Function_support() == _ENABLE)
        repeater_timing_support_table[_TXOUT_BOARD_DDR_SUPPORT] = _TRUE; // [FIX-ME] check support DDR by system API
    else
        repeater_timing_support_table[_TXOUT_BOARD_DDR_SUPPORT] = _FALSE; // [FIX-ME] check support DDR by system API
    repeater_timing_support_table[_TXOUT_ALL_VDB_SUPPORT] = _FALSE;
    repeater_timing_support_table[_TXOUT_ALL_420VDB_SUPPORT] = _FALSE;
    repeater_timing_support_table[_TXOUT_ALL_420CMDB_SUPPORT] = _FALSE;
    repeater_timing_support_table[_TXOUT_ALL_DTD_SUPPORT] = _FALSE;
    repeater_timing_support_table[_TXOUT_ALL_EXT_DTD_SUPPORT] = _FALSE;
    repeater_timing_support_table[_TXOUT_ALL_DID_T1_SUPPORT] = _FALSE;

    // EDID feature value init
    for(i=0; i<_EDID_FEATURE_TYPE_NUM; i++){
        if(i ==_EDID_VERSION)
            repeater_edid_support_table[i] = 1; // EDID v1.4
        else if(i == _HDMI_MAX_TMDS_CLK)
            repeater_edid_support_table[i] = 120; // TMDS Clock=120*5=600M
        else if(i == _MAX_FRL_RATE)
            repeater_edid_support_table[i] = 6; // FRL rate=6(12G)
        else if(i == _VRR_MIN)
            repeater_edid_support_table[i] = (repeater_timing_support_table[_TXOUT_BOARD_DDR_SUPPORT]? HDMITX_MIN_VRR_RATE: 0x0); // VRR_MIN=48hz for DDR, 0Hz for no DDR platform
        else if(i == _VRR_MAX)
            repeater_edid_support_table[i] = (repeater_timing_support_table[_TXOUT_BOARD_DDR_SUPPORT]? HDMITX_MAX_VRR_RATE: 0x0); // VRR_MAX=120hz for DDR, 0Hz for no DDR platform
        else if(i == _DSC_MAX_FRL_RATE)
            repeater_edid_support_table[i] = 0; // DSC FRL=0 (Not Support DSC)
        else if(i == _EDID_EXT_BLOCK_COUNT)
            repeater_edid_support_table[i] = 7; // EDID Externsion Block Count support: Max=7 (total EDID blocks=8)
        else
            repeater_edid_support_table[i] = _TRUE; // support all others EDID features
    }

    // EDID featuresTX not support
    repeater_edid_support_table[_AI_SUPPORT] = _FALSE;
    repeater_edid_support_table[_DC48_SUPPORT] = _FALSE;
    repeater_edid_support_table[_RR_CAPABLE] = _FALSE;
    repeater_edid_support_table[_LTE_340MCSC_SCRAMBLE] = _FALSE;
    repeater_edid_support_table[_LATENCY_FIELD_PRESENT] = _FALSE;
    repeater_edid_support_table[_ILATENCY_FIELD_PRESENT] = _FALSE;
    repeater_edid_support_table[_DC48_420_SUPPORT] = _FALSE;
    repeater_edid_support_table[_DSC_VERSION_12] = _FALSE;
    repeater_edid_support_table[_DSC_NATIVE_420] = _FALSE;
    repeater_edid_support_table[_DSC_ALL_BPP] = _FALSE;
    repeater_edid_support_table[_DSC_16BPC] = _FALSE;
    repeater_edid_support_table[_DSC_12BPC] = _FALSE;
    repeater_edid_support_table[_DSC_10BPC] = _FALSE;
    repeater_edid_support_table[_DSC_MAX_FRL_RATE] = 0;
    repeater_edid_support_table[_DSC_MAX_SLICES] = 0;
    repeater_edid_support_table[_DSC_TOTAL_CHUNKKBYTE] = 0;
    repeater_edid_support_table[_VSD_8K_SUPPORT] = _FALSE;
    repeater_edid_support_table[_DC48_SUPPORT] = _FALSE;
    if(lib_hdmitx_is_hdmi_422_support() == _FALSE)
        repeater_edid_support_table[_YCC422_SUPPORT] = _FALSE;
    //for HF-VSDB block
    repeater_edid_support_table[_Independent_view] = _FALSE;
    repeater_edid_support_table[_Dual_View] = _FALSE;
    repeater_edid_support_table[_3D_OSD_Disparity] = _FALSE;
    repeater_edid_support_table[_FAPA_End_Extended] = _FALSE;
    repeater_edid_support_table[_FVA] = _FALSE;
    repeater_edid_support_table[_QMS] = _FALSE;
    repeater_edid_support_table[_CINEMAVRR] = _FALSE;
    repeater_edid_support_table[_MDelta] = _FALSE;
    repeater_edid_support_table[_NEG_MVRR] = _FALSE;
    // limit EDID externsion block count to 3 (total EDID block=4) in repeater application (RX HW constrain)
#ifdef HDMITX_SW_CONFIG
    if(lib_hdmitx_get_edidPolicy_support() == _EDID_RX_EDID_Policy_RX_mixed_default_and_TX_by_RTD2801)
        repeater_edid_support_table[_EDID_EXT_BLOCK_COUNT] = 7; // EEODB <= 1 (i.e. ignore EDID Extension block count)
    else
#endif
        repeater_edid_support_table[_EDID_EXT_BLOCK_COUNT] = 3; 
    repeater_edid_support_table[_BT2020_cYCC] = _FALSE;
    repeater_edid_support_table[_opRGB] = _FALSE;
    repeater_edid_support_table[_opYCC601] = _FALSE;
    repeater_edid_support_table[_ST2113RGB23] = _FALSE;
    repeater_edid_support_table[_ICTCP] = _FALSE;

    if(!lib_hdmitx_is_hdmi_21_support())
        repeater_edid_support_table[_MAX_FRL_RATE] = 0; // FRL rate=0(tmds)
#if 0 // EDID modify verify
    // EDID features is support by condition
    // pixel bandwidth constrain
    repeater_edid_support_table[_YCC444_SUPPORT] = _FALSE; // [TEST]
    repeater_edid_support_table[_DC_Y444_SUPPORT] =_FALSE; // [TEST]
    repeater_edid_support_table[_DC36_SUPPORT] = _FALSE; // [TEST]
    repeater_edid_support_table[_DC30_SUPPORT] = _FALSE; // [TEST]

    // pixel align constrain
    repeater_edid_support_table[_YCC422_SUPPORT] = _FALSE; // [FIX-ME] disable due to video abnormal occurred when support Y422 is enabled
    repeater_edid_support_table[_YCC420_SUPPORT] = _FALSE; // [TEST]
    repeater_edid_support_table[_DC36_420_SUPPORT] = _FALSE; // [TEST]
    repeater_edid_support_table[_DC30_420_SUPPORT] = _FALSE; // [TEST]

    // for debug purpose
    repeater_edid_support_table[_DRR_SUPPORT] = _FALSE; // [TEST]
    repeater_edid_support_table[_SCDC_PRESENT] = _FALSE; // [TEST]
    repeater_edid_support_table[_LTE_340MCSC_SCRAMBLE] = _FALSE; // [TEST]
    repeater_edid_support_table[_EDID_VERSION] = 0; // [TEST] Version 1.3
    repeater_edid_support_table[_HDMI_MAX_TMDS_CLK] = 119; // [TEST] 119*5=595M
    repeater_edid_support_table[_MAX_FRL_RATE] = 5; // [TEST] FRL=5
    repeater_edid_support_table[_DSC_MAX_FRL_RATE] = 0; // [TEST] DSC FRL=0
#endif // #if 0 // EDID modify veify

    ScalerHdmiMacTx0RepeaterEdidSupportDump();
    return;
}

UINT8 ScalerHdmiMacTx0RepeaterGetEdidSupport(EnumEdidFeatureType enumEdidFeature)
{
    if(enumEdidFeature < _EDID_FEATURE_TYPE_NUM)
        return repeater_edid_support_table[enumEdidFeature];
    else
        return 0;
}

UINT8 ScalerHdmiMacTx0RepeaterSetEdidSupport(EnumEdidFeatureType enumEdidFeature, UINT8 value)
{
    if(enumEdidFeature >= _EDID_FEATURE_TYPE_NUM){
    DebugMessageHDMITx("[HDMI_TX] INVALID EDID FEATURE TYPE[%d]", (UINT32)enumEdidFeature);
    DebugMessageHDMITx("[%d]\n",  (UINT32)value);
        return -1;
    }
    repeater_edid_support_table[enumEdidFeature] = value;
    return 0;
}

UINT8 ScalerHdmiMacTx0RepeaterGetEdidTimingSupport(EnumTxOutTimingSupportType txOutTimingSupport)
{
    if(txOutTimingSupport < _TXOUT_TIMING_SUPPORT_TYPE_NUM)
        return repeater_timing_support_table[txOutTimingSupport];
    else
        return 0;
}


char ScalerHdmiMacTx0RepeaterSetEdidTimingSupport(EnumTxOutTimingSupportType txOutTimingSupport, UINT8 value)
{
    NoteMessageHDMITx("[HDMITX]EDID TIMING TYPE[%d]", (UINT32)txOutTimingSupport);
    NoteMessageHDMITx("[%d]\n", (UINT32)value);

    if(txOutTimingSupport >= _TXOUT_TIMING_SUPPORT_TYPE_NUM){
        return -1;
    }

    repeater_timing_support_table[txOutTimingSupport] = value;

    if(txOutTimingSupport == _TXOUT_BOARD_DDR_SUPPORT){
        repeater_edid_support_table[_VRR_MIN] = (repeater_timing_support_table[_TXOUT_BOARD_DDR_SUPPORT]? HDMITX_MIN_VRR_RATE: 0x0); // VRR_MIN=48hz for DDR, 0Hz for no DDR platform
        repeater_edid_support_table[_VRR_MAX] = (repeater_timing_support_table[_TXOUT_BOARD_DDR_SUPPORT]? HDMITX_MAX_VRR_RATE: 0x0); // VRR_MAX=120hz for DDR, 0Hz for no DDR platform
        //FatalMessageHDMITx("[HDMI_TX] EDID VRR MIN/MAX=%d/%d\n", (UINT32)repeater_edid_support_table[_VRR_MIN], (UINT32)repeater_edid_support_table[_VRR_MAX]);
    }

    return 0;
}

void ScalerHdmiMacTx0RepeaterEdidModifyVSDB(EnumOutputPort enumOutputPort)
{
    UINT8 value_u8 = 0;
    UINT8 vicLen = 0;
    //UINT8 tmpVicOffset = 0;
    UINT8 bModifyed = 0;
    UINT8 tmpLen = 0;
    UINT8 Hdmi3dLen = 0;
    UINT8 i;
    UINT8 ucVSDBLength;
    UINT16 usDbStartAddr = 0x00;
    UINT16 u16TmpValue = 0;
    ucVSDBLength = pucDdcRamAddr[pusCtaDataBlockAddr[_CTA_HDMI_VSDB]]& 0x1F;
    if(ucVSDBLength < 8)
        return;

    memset(pucEdidVDBVIC, 0, EDID_VDB_VIC_LEN);//pucEdidVDBVIC :Share this buffer for dmem
    if(ScalerHdmiMacTxEdidGetFeature(enumOutputPort, _HDMI_VIDEO_PRESENT) == _TRUE){
        value_u8 = 0;//As a cursor for pucEdidVDBVIC buffer [31 byte:use u8 ok]
        u16TmpValue = 0;//As a cursor for pucDdcRamAddr buffer[512 byte:use u16 ok]
        usDbStartAddr = pusCtaDataBlockAddr[_CTA_HDMI_VSDB];
        u16TmpValue = usDbStartAddr;
        //for First 9 bytes
        memcpy(&pucEdidVDBVIC[value_u8], &pucDdcRamAddr[usDbStartAddr], 9);
        value_u8  += 9;
        u16TmpValue += 9;
        if((ScalerHdmiMacTxEdidGetFeature(enumOutputPort, _LATENCY_FIELD_PRESENT) == _TRUE))
        {//for latency 2 bytes
            if((ScalerHdmiMacTx0RepeaterGetEdidSupport(_LATENCY_FIELD_PRESENT) == _FALSE))
            {
                pucEdidVDBVIC[(value_u8 - 1)] = pucEdidVDBVIC[(value_u8 - 1)] & 0x7f;
                u16TmpValue += 2;
                bModifyed = 1;
            }
            else
            {
                memcpy(&pucEdidVDBVIC[value_u8], &pucDdcRamAddr[u16TmpValue], 2);
                u16TmpValue += 2;
                value_u8 += 2;
            }
        }
         if((ScalerHdmiMacTxEdidGetFeature(enumOutputPort, _ILATENCY_FIELD_PRESENT) == _TRUE))
        {//for i latency 2 bytes
            if((ScalerHdmiMacTx0RepeaterGetEdidSupport(_ILATENCY_FIELD_PRESENT) == _FALSE))
            {
                pucEdidVDBVIC[(value_u8 - 1)] = pucEdidVDBVIC[(value_u8 - 1)] & 0xbf;
                u16TmpValue += 2;
                bModifyed = 1;
            }
            else
            {
                memcpy(&pucEdidVDBVIC[value_u8], &pucDdcRamAddr[u16TmpValue], 2);
                u16TmpValue += 2;
                value_u8 += 2;
            }
        }
#ifndef _ENABLE_HDMI_VSDB_3D_SUPPORT //TX not support 3D timing mode

        if(((pucDdcRamAddr[u16TmpValue] & _BIT7) == _BIT7)||((pucDdcRamAddr[u16TmpValue] & _BIT6) == _BIT6)||((pucDdcRamAddr[u16TmpValue] & _BIT5) == _BIT5)){
            pucDdcRamAddr[u16TmpValue] = pucDdcRamAddr[u16TmpValue] & 0x1f;
            Hdmi3dLen = (pucDdcRamAddr[u16TmpValue+1] & 0x1f);
            if(Hdmi3dLen) {
                pucDdcRamAddr[u16TmpValue+1] = pucDdcRamAddr[u16TmpValue+1] & 0xe0;
                bModifyed = 1;
            }
        }
#endif
        //for 3d present 1 bytes
        pucEdidVDBVIC[value_u8]= pucDdcRamAddr[u16TmpValue];
        value_u8 ++;
        u16TmpValue++;

        //for 3d len/vic len 1 bytes
       pucEdidVDBVIC[value_u8]= pucDdcRamAddr[u16TmpValue];
       value_u8++;
       u16TmpValue++;
#ifndef _ENABLE_HDMI_VSDB_4096_SUPPORT //TX not support 4096 timing mode
        //for hdmi vic x 4 bytes
        vicLen = (pucDdcRamAddr[u16TmpValue  -1] & 0xe0)>>5;
        for(i = 0; i < vicLen; i++){
            if(pucDdcRamAddr[(u16TmpValue  +i)] == 0x4){
                bModifyed = 1;
            }else{
                pucEdidVDBVIC[(value_u8 +i)] = pucDdcRamAddr[(u16TmpValue  +i)];
                tmpLen++;
            }
        }
        pucEdidVDBVIC[value_u8-1] = pucEdidVDBVIC[value_u8-1] & 0x1f;
        pucEdidVDBVIC[value_u8-1] = pucEdidVDBVIC[value_u8-1]  | (tmpLen<<5);
        value_u8 += tmpLen;
        u16TmpValue += tmpLen;
#endif
        if(bModifyed){
            memcpy(&pucDdcRamAddr[usDbStartAddr], pucEdidVDBVIC, value_u8);
            tmpLen = value_u8-1;
            adjustEdidDbOffset(usDbStartAddr, tmpLen);
        }
    }

}

void ScalerHdmiMacTx0RepeaterEdidModifyAMDVSDB(void)
{
    UINT16 usDbStartAddr = 0x00;
    if(!GET_HDMI_MAC_TX0_FIND_SPECIAL_TV3()){
        usDbStartAddr = pusCtaDataBlockAddr[_CTA_FS_DB];
        // for Support AMD Free Sync mode,if Byte[1] FreeSync Support=1,should Byte[4] VCP Code=0;
        pucDdcRamAddr[usDbStartAddr + 8] = 0x00;
    }
}
void ScalerHdmiMacTx0RepeaterEdidModifyAMDVSDBForHDR(void)
{
    UINT16 usDbStartAddr = 0x00;
    if(!lib_hdmitx_is_HDR10_Function_support())
        return;

     if(pusCtaDataBlockAddr[_CTA_FS_DB] == 0x00)
        return;

     usDbStartAddr = pusCtaDataBlockAddr[_CTA_FS_DB];
     //only Version2
     if(pucDdcRamAddr[usDbStartAddr + 4] != 0x2)
        return;

     if(ScalerHdmiMacTx0EdidGetFeature(_HDR_SMPTE_ST2084) == _FALSE && ScalerHdmiMacTx0EdidGetFeature(_HDR_HLG) == _FALSE)
        return;

     //set  SVDB_Byte5 = 0x00
     pucDdcRamAddr[usDbStartAddr + 9] = 0x00;
     //set SVDB_Byte1[1] = 1
     pucDdcRamAddr[usDbStartAddr + 5] = pucDdcRamAddr[usDbStartAddr + 5] | 0x2;
}
/****************************************
  Description  : Filter TX not support EDID feature and timing mode from EDID buffer
  Input Value  :
  Output Value :
****************************************/
void ScalerHdmiMacTx0RepeaterEdidInfoUpdate(EnumOutputPort enumOutputPort)
{
    UINT8 i;
    // clear EDID video timing not list on cea_timing_table_repeater[]
    ScalerHdmiMacTx0RepeaterEdidModifyTiming();

    ScalerHdmiMacTx0RepeaterEdidModifyVSDB(enumOutputPort);

    if(!lib_hdmitx_is_HDR10_PLUS_Function_support()){
        ScalerHdmiMacTx0RepeaterEdidModifyVSVDB_HDR10();
        ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
    }

    if(!lib_hdmitx_is_HDR10_Function_support()){
        ScalerHdmiMacTx0RepeaterEdidModifySHDR_DB();
        ScalerEdidGetCtaExtDbAddress(pusCtaDataBlockAddr);
    }

    ScalerHdmiMacTx0RepeaterEdidModifyAMDVSDBForHDR();
    // sync EDID features to repeater capability
    for(i=0; i < _EDID_FEATURE_TYPE_NUM; i++){
        if(((i == _EDID_VERSION)|| (i == _HDMI_MAX_TMDS_CLK)|| (i == _MAX_FRL_RATE)|| (i == _DSC_MAX_FRL_RATE) || (i == _VRR_MAX)|| (i == _EDID_EXT_BLOCK_COUNT)|| (i == _DSC_MAX_SLICES)|| (i == _DSC_TOTAL_CHUNKKBYTE))
            && (ScalerHdmiMacTxEdidGetFeature(enumOutputPort, i) > ScalerHdmiMacTx0RepeaterGetEdidSupport(i)))
        {
            ScalerHdmiMacTx0RepeaterEdidModifyFeature(i, ScalerHdmiMacTx0RepeaterGetEdidSupport(i));
        }
        else if((i == _VRR_MIN) && (ScalerHdmiMacTxEdidGetFeature(enumOutputPort, i) < ScalerHdmiMacTx0RepeaterGetEdidSupport(i)))
        {
            ScalerHdmiMacTx0RepeaterEdidModifyFeature(i, ScalerHdmiMacTx0RepeaterGetEdidSupport(i));
        }
        else if((i == _DRR_SUPPORT)&&(ScalerHdmiMacTxEdidGetFeature(enumOutputPort, _DRR_SUPPORT) == _TRUE))
        {
            ScalerHdmiMacTx0RepeaterEdidModifyAMDVSDB();
        }
        else if((ScalerHdmiMacTxEdidGetFeature(enumOutputPort, i) == _TRUE) && (ScalerHdmiMacTx0RepeaterGetEdidSupport(i) == _FALSE)){
            ScalerHdmiMacTx0RepeaterEdidModifyFeature(i, _FALSE);
        }

    }
    // update & dump EDID info
    ScalerHdmiMacTx0EdidAnalysis();

#ifdef _SUPPORT_HDMITX_CTS_FOR_QD980 // modify EDID feature for CTS test
    ScalerHdmiMacTx0EdidSetFeature( _SCDC_PRESENT, _TRUE);
    ScalerHdmiMacTx0EdidSetFeature( _MAX_FRL_RATE, 6);
#endif

    return;
}


StructHDMITxOutputTimingInfo* p_getTxOutputTimingInfo;

void recal_hdmitx_pixel_clock(void)
{
        g_TxPixelClock1024 /= (timing_info.h_total);
        g_TxPixelClock1024 *= (p_getTxOutputTimingInfo->TxOutputHTotal);
        
}

void update_hdmitx_HD21_480ior576i_output_timing(void)
{
    SET_HDMI_HD21_needUnvalid();
    if(p_getTxOutputTimingInfo->TxOutputHwid == 720){
        if(p_getTxOutputTimingInfo->TxOutputVlen == 240){
            p_getTxOutputTimingInfo->TxOutputHSync = HDMITX_OutputTiming_For4N_64;
            p_getTxOutputTimingInfo->TxOutputHsta = HDMITX_OutputTiming_For4N_124;
            p_getTxOutputTimingInfo->TxOutputHTotal = HDMITX_OutputTiming_For4N_868;
            SET_HDMI_HD21_720_480i();
            needUnvalid_index = HD21_720_480I;
        }else{
            p_getTxOutputTimingInfo->TxOutputHSync = HDMITX_OutputTiming_For4N_64;
            p_getTxOutputTimingInfo->TxOutputHsta = HDMITX_OutputTiming_For4N_136;
            p_getTxOutputTimingInfo->TxOutputHTotal = HDMITX_OutputTiming_For4N_872;
            needUnvalid_index = HD21_720_576I;
        }
    }else if(p_getTxOutputTimingInfo->TxOutputHwid == 1440){
        if(p_getTxOutputTimingInfo->TxOutputVlen == 240){
            p_getTxOutputTimingInfo->TxOutputHSync = HDMITX_OutputTiming_For4N_124;
            p_getTxOutputTimingInfo->TxOutputHsta = HDMITX_OutputTiming_For4N_240;
            p_getTxOutputTimingInfo->TxOutputHTotal = HDMITX_OutputTiming_For4N_1720;
            needUnvalid_index = HD21_1440_480I;
        }else{
            p_getTxOutputTimingInfo->TxOutputHSync = HDMITX_OutputTiming_For4N_128;
            p_getTxOutputTimingInfo->TxOutputHsta = HDMITX_OutputTiming_For4N_268;
            p_getTxOutputTimingInfo->TxOutputHTotal = HDMITX_OutputTiming_For4N_1736;
            needUnvalid_index = HD21_1440_576I;
        }
    }
    recal_hdmitx_pixel_clock();
    p_getTxOutputTimingInfo->pixelFreq = g_TxPixelClock1024 / 100000;
}
void update_hdmitx_HD20_480ior576i_output_timing(void)
{
 #if 0
    UINT8 ucHWUnvalid = 0;
    UINT8 ucHstaUnvalid = 0;
    UINT8 ucPreHstaUnvalid = 0;

    ucHWUnvalid = p_getTxOutputTimingInfo->TxOutputHSync % 4;
    ucPreHstaUnvalid = p_getTxOutputTimingInfo->TxOutputHsta % 2;
    if(ucHWUnvalid){
        p_getTxOutputTimingInfo->TxOutputHSync += (4 - ucHWUnvalid);
        p_getTxOutputTimingInfo->TxOutputHsta += (4 - ucHWUnvalid);
        ucHstaUnvalid = p_getTxOutputTimingInfo->TxOutputHsta % 2;
        if(ucHstaUnvalid)
            p_getTxOutputTimingInfo->TxOutputHsta -=ucHstaUnvalid;
        else
            p_getTxOutputTimingInfo->TxOutputHsta -=(4 - ucHWUnvalid);
        bHD20_needRetiming = 1;
    }
    #endif
    SET_HDMI_HD20_needRetiming();
    if(p_getTxOutputTimingInfo->TxOutputHwid == 720){
        if(p_getTxOutputTimingInfo->TxOutputVlen == 240){
            p_getTxOutputTimingInfo->TxOutputHSync = HDMITX_OutputTiming_For4N_64;
            p_getTxOutputTimingInfo->TxOutputHsta = HDMITX_OutputTiming_For4N_120;
            p_getTxOutputTimingInfo->TxOutputHTotal = HDMITX_OutputTiming_For4N_858;
            needRetiming_index = HD20_720_480I;
        }else{
            p_getTxOutputTimingInfo->TxOutputHSync = HDMITX_OutputTiming_For4N_64;
            p_getTxOutputTimingInfo->TxOutputHsta = HDMITX_OutputTiming_For4N_132;
            p_getTxOutputTimingInfo->TxOutputHTotal = HDMITX_OutputTiming_For4N_864;
            needRetiming_index = HD20_720_576I;
        }
    }else if(p_getTxOutputTimingInfo->TxOutputHwid == 1440){
        if(p_getTxOutputTimingInfo->TxOutputVlen == 240){
            CLR_HDMI_HD20_needRetiming();
            p_getTxOutputTimingInfo->TxOutputHSync = HDMITX_OutputTiming_For4N_124;
            p_getTxOutputTimingInfo->TxOutputHsta = HDMITX_OutputTiming_For4N_238;
            p_getTxOutputTimingInfo->TxOutputHTotal = HDMITX_OutputTiming_For4N_1716;
            needRetiming_index = HD20_1440_480I;
        }else{
            p_getTxOutputTimingInfo->TxOutputHSync = HDMITX_OutputTiming_For4N_128;
            p_getTxOutputTimingInfo->TxOutputHsta = HDMITX_OutputTiming_For4N_264;
            p_getTxOutputTimingInfo->TxOutputHTotal = HDMITX_OutputTiming_For4N_1728;
            needRetiming_index = HD20_1440_576I;
        }
    }
    recal_hdmitx_pixel_clock();
    p_getTxOutputTimingInfo->pixelFreq = g_TxPixelClock1024 / 100000;

}
void update_hdmitx_output_timing_unvalid(void)
{
    UINT8 ucHWUnvalid = 0;
    UINT8 unvalid_flag = 0;
    ucHWUnvalid = p_getTxOutputTimingInfo->TxOutputHSync % 4;
    if(ucHWUnvalid) {
        p_getTxOutputTimingInfo->TxOutputHSync += (4 - ucHWUnvalid);
        p_getTxOutputTimingInfo->TxOutputHsta += (4 - ucHWUnvalid);
        p_getTxOutputTimingInfo->TxOutputHTotal += (4 - ucHWUnvalid);
        //ucHWUnvalid = 4 - ucHWUnvalid;
        unvalid_flag = 1;
    }
    ucHWUnvalid = p_getTxOutputTimingInfo->TxOutputHsta % 4;
    if(ucHWUnvalid) {
        p_getTxOutputTimingInfo->TxOutputHsta += (4 - ucHWUnvalid);
        p_getTxOutputTimingInfo->TxOutputHTotal += (4 - ucHWUnvalid);
        //ucHWUnvalid = 4 - ucHWUnvalid;
        unvalid_flag = 1;
    }
    ucHWUnvalid = p_getTxOutputTimingInfo->TxOutputHTotal % 4;
    if(ucHWUnvalid) {
        p_getTxOutputTimingInfo->TxOutputHTotal += (4 - ucHWUnvalid);
        //ucHWUnvalid = 4 - ucHWUnvalid;
        unvalid_flag = 1;
    }

    if(unvalid_flag) {
        recal_hdmitx_pixel_clock();
        p_getTxOutputTimingInfo->pixelFreq = g_TxPixelClock1024 / 100000;
    }

    return;
}
BOOLEAN judge_hdmitx_480ior576i(void)
{
    if((p_getTxOutputTimingInfo->scanType)
        &&  (p_getTxOutputTimingInfo->TxOutputHwid == 1440 || p_getTxOutputTimingInfo->TxOutputHwid == 720)
        &&  (p_getTxOutputTimingInfo->TxOutputVlen == 240 || p_getTxOutputTimingInfo->TxOutputVlen == 288)){
        SET_HDMI_HD_480i_576i();
        return _TRUE;
    }else{
        CLR_HDMI_HD_480i_576i();
        return _FALSE;
    }

}
void update_hdmitx_480ior576i_output_timing(void)
{
    CLR_HDMI_HD21_720_480i();
    CLR_HDMI_HD20_needRetiming();
    CLR_HDMI_HD21_needUnvalid();

#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
        if(drvif_Hdmi_Multi_port_GetFrlMode(bypass_port) != 0){
            update_hdmitx_HD21_480ior576i_output_timing();
        }else
#endif
        {
            update_hdmitx_HD20_480ior576i_output_timing();
        }
}
void ScalerHdmiMacTx0RepeaterOutputTimingInfo(void)
{
    UINT16 u16TmpValue = 0;
    UINT8 value_u8 = 0;
    if(!p_getTxOutputTimingInfo)
    {
        DebugMessageHDMITx("[HDMITX] OutputTimingInfo addr is null\n");
        return;
    }
    memset(p_getTxOutputTimingInfo, 0 ,sizeof(StructHDMITxOutputTimingInfo));
    memset(&dtd_timing, 0, sizeof(hdmi_timing_check_type));
    value_u8 = ScalerHdmiMacTx0RepeaterGetTxOutputModeTiming((TX_OUT_MODE_SELECT_TYPE)get_hdmitxOutputMode());
    //value_u8 = (timeIdx < TX_TIMING_NUM? timeIdx: hdmitx_ctrl_get_default_timing_type());
    //HDMITX_TIMING_INDEX = value_u8;
    NoteMessageHDMITx("[HDMITX] id-%d,hdmitx support 21=%d\n",(UINT32)value_u8, (UINT32)lib_hdmitx_is_hdmi_21_support());
    p_getTxOutputTimingInfo->TxOutputTimingMode =value_u8;
    if(value_u8 >= TX_TIMING_NUM) {
        DebugMessageHDMITx("[HDMITX] timing id invaild\n");
        return;
    }
    p_tx_cea_timing_repeater_table = &cea_timing_table_repeater[0]+value_u8;
   if((timing_info.run_vrr &&  (g_scaler_display_info.input_src == 1)) ||  (lib_hdmitx_industrial_ctrl_enable())||(get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE) || (value_u8 == TX_TIMING_DEFAULT)) {
        if(get_hdmitxOutputMode() != TX_OUT_THROUGH_MODE) {
            p_getTxOutputTimingInfo->colorspace = tx_output_colorSpace;
            p_getTxOutputTimingInfo->colordepth = tx_output_colordepth;
        } else {
            p_getTxOutputTimingInfo->colorspace = timing_info.colorspace;
            p_getTxOutputTimingInfo->colordepth = timing_info.colordepth;
        }
        if((timing_info.run_vrr &&  (g_scaler_display_info.input_src == 1)) || lib_hdmitx_industrial_ctrl_enable()){
            p_getTxOutputTimingInfo->colorspace = timing_info.colorspace;
            p_getTxOutputTimingInfo->colordepth = timing_info.colordepth;
        }
        p_getTxOutputTimingInfo->TxOutputHsta = timing_info.h_act_sta;
        p_getTxOutputTimingInfo->TxOutputVsta = timing_info.v_act_sta;
        p_getTxOutputTimingInfo->TxOutputHwid = timing_info.h_act_len;
        p_getTxOutputTimingInfo->TxOutputVlen = timing_info.v_act_len;
        p_getTxOutputTimingInfo->TxOutputHSync = timing_info.IHSyncPulseCount;
        p_getTxOutputTimingInfo->TxOutputVSync = timing_info.IVSyncPulseCount;
        p_getTxOutputTimingInfo->TxOutputHTotal = timing_info.h_total;
        p_getTxOutputTimingInfo->TxOutputVTotal =  timing_info.v_total;
        p_getTxOutputTimingInfo->vFreq = timing_info.v_freq /10;
        p_getTxOutputTimingInfo->pixelFreq = timing_info.pll_pixelclockx1024 / 100000;

        p_getTxOutputTimingInfo->scanType = timing_info.is_interlace;
        if(timing_info.run_vrr && (value_u8 != TX_TIMING_DEFAULT) && (g_scaler_display_info.input_src == 1)) {
            if(p_getTxOutputTimingInfo->TxOutputVSync == 1)
                p_getTxOutputTimingInfo->TxOutputVSync = 2;
            p_getTxOutputTimingInfo->TxOutputVsta = p_tx_cea_timing_repeater_table->v.back + p_tx_cea_timing_repeater_table->v.sync;
        }
        if(judge_hdmitx_480ior576i()){
            update_hdmitx_480ior576i_output_timing();
        }else{
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
        if((g_scaler_display_info.input_src == 0) && (drvif_Hdmi_Multi_port_GetFrlMode(bypass_port) != 0))
            update_hdmitx_output_timing_unvalid();
#endif
        }

    }
#ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
    else if((get_hdmitxOutputMode() == TX_OUT_SOURCE_MODE) && (edid_dtd_flag == 1)) {
        p_getTxOutputTimingInfo->colorspace = tx_output_colorSpace;
        p_getTxOutputTimingInfo->colordepth = tx_output_colordepth;//HDMITX_GetTxOutputColorDepth(value_u8);
        p_getTxOutputTimingInfo->TxOutputHsta = dtd_timing.h_blank - dtd_timing.h_front;
        p_getTxOutputTimingInfo->TxOutputVsta = dtd_timing.v_blank - dtd_timing.v_front;
        p_getTxOutputTimingInfo->TxOutputHwid = dtd_timing.h_act;
        p_getTxOutputTimingInfo->TxOutputVlen =dtd_timing.v_act;
        p_getTxOutputTimingInfo->TxOutputHSync = dtd_timing.h_sync;
        p_getTxOutputTimingInfo->TxOutputVSync =dtd_timing.v_sync;
        p_getTxOutputTimingInfo->TxOutputHTotal = dtd_timing.h_act + dtd_timing.h_blank;
        p_getTxOutputTimingInfo->TxOutputVTotal =  dtd_timing.v_act + dtd_timing.v_blank;
        p_getTxOutputTimingInfo->vFreq = timing_info.v_freq /10;

        p_getTxOutputTimingInfo->pixelFreq = g_TxPixelClock1024 / 100000;

        p_getTxOutputTimingInfo->scanType = timing_info.is_interlace;
    }
#endif
    else{
        p_getTxOutputTimingInfo->colorspace = tx_output_colorSpace;
        p_getTxOutputTimingInfo->colordepth = tx_output_colordepth;//HDMITX_GetTxOutputColorDepth(value_u8);
        p_getTxOutputTimingInfo->TxOutputHsta = p_tx_cea_timing_repeater_table->h.back + p_tx_cea_timing_repeater_table->h.sync;
        p_getTxOutputTimingInfo->TxOutputVsta =p_tx_cea_timing_repeater_table->v.back + p_tx_cea_timing_repeater_table->v.sync;
        p_getTxOutputTimingInfo->TxOutputHwid = p_tx_cea_timing_repeater_table->h.active;
        p_getTxOutputTimingInfo->TxOutputVlen =p_tx_cea_timing_repeater_table->v.active;
        p_getTxOutputTimingInfo->TxOutputHSync = p_tx_cea_timing_repeater_table->h.sync;
        p_getTxOutputTimingInfo->TxOutputVSync =p_tx_cea_timing_repeater_table->v.sync;
        u16TmpValue = p_tx_cea_timing_repeater_table->h.active + p_tx_cea_timing_repeater_table->h.sync;
        u16TmpValue = u16TmpValue + p_tx_cea_timing_repeater_table->h.front;
        u16TmpValue = u16TmpValue + p_tx_cea_timing_repeater_table->h.back;
        p_getTxOutputTimingInfo->TxOutputHTotal = u16TmpValue;
        u16TmpValue = p_tx_cea_timing_repeater_table->v.active + p_tx_cea_timing_repeater_table->v.sync;
        u16TmpValue = u16TmpValue + p_tx_cea_timing_repeater_table->v.front;
        u16TmpValue = u16TmpValue + p_tx_cea_timing_repeater_table->v.back;
        p_getTxOutputTimingInfo->TxOutputVTotal =  u16TmpValue;
        if(GET_ADJUST_VTOTAL_FLAG()) {
            if(p_getTxOutputTimingInfo->TxOutputVTotal >= GET_ADJUST_OUTPUT_VTOTAL()) {
                u16TmpValue = p_getTxOutputTimingInfo->TxOutputVTotal - GET_ADJUST_OUTPUT_VTOTAL();
                if(p_getTxOutputTimingInfo->TxOutputVsta > u16TmpValue)
                    p_getTxOutputTimingInfo->TxOutputVsta -= u16TmpValue;
            } else {
                u16TmpValue =  GET_ADJUST_OUTPUT_VTOTAL() - p_getTxOutputTimingInfo->TxOutputVTotal;
                p_getTxOutputTimingInfo->TxOutputVsta += u16TmpValue;
            }
            p_getTxOutputTimingInfo->TxOutputVTotal = GET_ADJUST_OUTPUT_VTOTAL();
        }
        if(timing_info.run_vrr) {
            checkVrrVstart();
        }
        p_getTxOutputTimingInfo->vFreq = timing_info.v_freq /10;

        p_getTxOutputTimingInfo->pixelFreq = g_TxPixelClock1024 / 100000;

        p_getTxOutputTimingInfo->scanType = timing_info.is_interlace;
    }


    DebugMessageHDMITx("[HDMITX]  start(%d/%d)\n",(UINT32)p_getTxOutputTimingInfo->TxOutputHsta,(UINT32)p_getTxOutputTimingInfo->TxOutputVsta);
    DebugMessageHDMITx("[HDMITX]  total(%d/%d)\n",(UINT32)p_getTxOutputTimingInfo->TxOutputHTotal,(UINT32)p_getTxOutputTimingInfo->TxOutputVTotal);
    DebugMessageHDMITx("[HDMITX]  sync(%d/%d)\n",(UINT32)p_getTxOutputTimingInfo->TxOutputHSync,(UINT32)p_getTxOutputTimingInfo->TxOutputVSync);
    return;
}

extern UINT8 ucHaveEEODB;

/****************************************
  Description  : update RX EDID info and toggle RX HPD status(option)
  Input Value  :
  Output Value :
****************************************/
StructHDMITxEdidBufInfo stHdmiTxEdidInfo;
StructHDMITxEdidBufInfo *ApGetScalerHdmiMacTx0RepeaterEdid(void)
{
    UINT16 usEDIDExtBase= 0x7E;
    UINT8 ucEdidExtBlockNum;
    UINT16 u16TmpValue = 0;
    stHdmiTxEdidInfo.pEdidBufAddr = ScalerOutputGetPxPortDdcRamAddr(EdidMacTxPxMapping(_TX0));
    pucDdcRamAddr = stHdmiTxEdidInfo.pEdidBufAddr;
    if(ucHaveEEODB)
        ucEdidExtBlockNum = pucDdcRamAddr[0x86];
    else
        ucEdidExtBlockNum = pucDdcRamAddr[usEDIDExtBase];
    stHdmiTxEdidInfo.u16EdidLen = 0x80 *(ucEdidExtBlockNum+1);
    if(stHdmiTxEdidInfo.u16EdidLen <= EDID_BUFFER_SIZE){
        u16TmpValue = stHdmiTxEdidInfo.u16EdidLen;
    }else{
        u16TmpValue = EDID_BUFFER_SIZE;
    }
    //FatalMessageHDMITx("[HDMITX] EDID to AP DBnum= %d Len=%d %d\n",(UINT32)ucEdidExtBlockNum, (UINT32)stHdmiTxEdidInfo.u16EdidLen,u16TmpValue);
    #if 0//def CONFIG_DUMP_EDIDINFO //Remove for reducing bank size
    for(edidnum = 0; edidnum < u16TmpValue; edidnum++)
    {
        if((edidnum % 16)==0)
            InfoMessageHDMITx("\n ");
        InfoMessageHDMITx("%x, ", (UINT32)pucDdcRamAddr[edidnum]);
    }
    InfoMessageHDMITx("\n ");
    #endif
    return &stHdmiTxEdidInfo;
}

#if 0
/*
    Description  : Set TX output mode
    Input Value  :  TX_OUT_MODE_SELECT_TYPE
    Output Value : 0: success
*/
UINT8 ScalerHdmiMacTx0RepeaterSetTxOutputMode(TX_OUT_MODE_SELECT_TYPE mode)
{
    if(mode >= TX_OUT_MODE_NUM){
        DebugMessageHDMITx("[HDMITX] INVALID TX_OUT_MODE[%d]@SetTxOutputMode()\n", (UINT32)mode);
        return 1;
    }

    if(repeater_tx_out_mode != mode){
        NoteMessageHDMITx("[HDMITX] TXOUTMODE=%d\n", (UINT32)mode);
        repeater_tx_out_mode = mode;
    }

    return 0;
}


/*
    Description  : Get TX output mode
    Input Value  :
    Output Value : TX_OUT_MODE_SELECT_TYPE
*/
TX_OUT_MODE_SELECT_TYPE ScalerHdmiMacTx0RepeaterGetTxOutputMode(void)__banked
{
    return repeater_tx_out_mode;
}
#endif

/*
    Description  : Set User TX output mode timing
    Input Value  :
    Output Value : 0: success
*/
UINT8 ScalerHdmiMacTx0RepeaterSetTxUserTimingMode(TX_TIMING_INDEX time_mode)
{
    if(time_mode >= TX_TIMING_NUM){
        DebugMessageHDMITx("[HDMITX] INVALID TIME MODE[%d]@ScalerHdmiMacTx0RepeaterSetTxUserTimingMode()\n", (UINT32)time_mode);
        return TX_TIMING_NUM;
    }

    DebugMessageHDMITx("[HDMITX] Set TX User Mode Timing=%d\n", (UINT32)time_mode);
    repeater_tx_out_timing_user = time_mode;
    return 0;
}


/*
    Description  : Get User TX output mode timing
    Input Value  :
    Output Value : TX_TIMING_INDEX
*/
TX_TIMING_INDEX ScalerHdmiMacTx0RepeaterGetTxUserTimingMode(void)
{
    return repeater_tx_out_timing_user;
}
extern unsigned int bypass_port;

/*****************************************************
  Description  : Check TX output timing mode by RX timing info
  Input Value  :
  Output Value : TX_TIMING_INDEX
    return: TX_TIMING_NUM: TX not support
*****************************************************/
TX_TIMING_INDEX ScalerHdmiMacTx0RepeaterTimingRxInputCheckSupport(void)
{

    UINT16 tmpPixelClk;
    UINT8 txTimingCheckStart=0;
    UINT8 u8TmpValue = 0;
    UINT32 u32TmpValue = 0;
    BOOLEAN boolTmpValue = 0;
#if (IS_ENABLED(CONFIG_RTK_HDMI_RX) || IS_ENABLED(CONFIG_ENABLE_DPRX)) && !IS_ENABLED(ENABLE_INTERNAL_HDMIRX_PTG)//CONFIG_HDMITX_AP_TEST
        if(g_scaler_display_info.input_src == 0){
            #if IS_ENABLED(CONFIG_RTK_HDMI_RX)
            drvif_Hdmi_TimingInfo_for_hdmitx(bypass_port,&timing_info);
            #endif
        }else{
            #if IS_ENABLED(CONFIG_ENABLE_DPRX)
            ptm_timing_info = drvif_Dprx_GetRawTimingInfo(0);
            #endif
        }


    bHD21_480P_576P = 0;
#else
    timing_info.v_total = 2250;
    timing_info.h_total = 4400;
    timing_info.v_act_len = 2160;
    timing_info.h_act_len = 3840;
    timing_info.v_freq = 600;
#endif

    if(ScalerHdmiMacTx0EdidGetFeature(_SCDC_PRESENT) == _FALSE)
        pixelClockLimit = TX_OUT_PIXEL_CLOCK_MAX_WO_SCDC;
    else if(ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE) == 0)
        pixelClockLimit = ((UINT16)ScalerHdmiMacTx0EdidGetFeature(_HDMI_MAX_TMDS_CLK) * 50);
    else
        pixelClockLimit = getLimitPixelClkWithDDR();

    // [FIX-ME] TX output HDMI21 mode when Sink device support HDMI21 (Need consider input source type too)
    //if(ScalerHdmiMacTxEdidGetFeature(_P0_OUTPUT_PORT, _MAX_FRL_RATE) >= _HDMI21_FRL_6G_4LANES)
        //txTimingCheckStart = TX_TIMING_1080P60;

    for(u8TmpValue=txTimingCheckStart; u8TmpValue<TX_TIMING_NUM; u8TmpValue++){
        //bypass mode only compare vfeq/hactive/vactive/pixel_clk<EDID max
        u32TmpValue = (UINT32)ABS(cea_timing_table_repeater[u8TmpValue].vFreq, timing_info.v_freq/10) * 1000; // v_freq diff < 0.9%
        boolTmpValue = ((u32TmpValue / cea_timing_table_repeater[u8TmpValue].vFreq) < HDMITX_CLK_DIFF);  // v_freq diff < 0.9%
        boolTmpValue = boolTmpValue && (timing_info.h_act_len == cea_timing_table_repeater[u8TmpValue].h.active);
        boolTmpValue = boolTmpValue && (timing_info.v_act_len == cea_timing_table_repeater[u8TmpValue].v.active);
        if(HDMITX_COLOR_YUV420 == timing_info.colorspace){
            tmpPixelClk = timing_info.pll_pixelclockx1024/200000;
        }else{
            tmpPixelClk = timing_info.pll_pixelclockx1024/100000;
        }
        if(HDMITX_COLOR_YUV422 == timing_info.colorspace) {
            //tmpPixelClk = tmpPixelClk;
        } else {
            if(timing_info.colordepth == HDMITX_10BIT)
                tmpPixelClk = tmpPixelClk  /4* 5;
            else if(timing_info.colordepth == HDMITX_12BIT)
                tmpPixelClk = tmpPixelClk  /2 * 3;
        }

        boolTmpValue = boolTmpValue && (tmpPixelClk <= pixelClockLimit);

        if(boolTmpValue){
            g_TxPixelClock1024 = timing_info.pll_pixelclockx1024;
            InfoMessageHDMITx("[HDMITX] RX Source[%dx%d@%d(/%d)] Support@%d\n",(UINT32)timing_info.h_act_len, (UINT32)timing_info.v_act_len, (UINT32)tmpPixelClk,(UINT32)pixelClockLimit, (UINT32)u8TmpValue);
            cmpBypassModeMatchTiming(u8TmpValue);
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
            if((g_scaler_display_info.input_src == 0) && (drvif_Hdmi_Multi_port_GetFrlMode(bypass_port) != 0)){
                if((u8TmpValue == TX_TIMING_720X480P60) || (u8TmpValue == TX_TIMING_720X576P50)){
                    bHD21_480P_576P = 1;
                }
            }
#endif
            return u8TmpValue;
        }
    }

    InfoMessageHDMITx("[HDMITX] RX Source[%dx%d@%d|%d Not Support\n", (UINT32)timing_info.h_act_len, (UINT32)timing_info.v_act_len,(UINT32)tmpPixelClk, (UINT32)pixelClockLimit);
    g_TxPixelClock1024 = timing_info.pll_pixelclockx1024;
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
    if((tmpPixelClk > pixelClockLimit) && (g_scaler_display_info.input_src == 0) && (drvif_Hdmi_Multi_port_GetFrlMode(bypass_port) == 0))
        return TX_TIMING_NUM;
    else
#endif
        return TX_TIMING_DEFAULT;
}


/*
    Description  : Get TX output mode timing
    Input Value  : TX_OUT_MODE_SELECT_TYPE
    Output Value : TX output timing index for specified TX output mode
*/
TX_TIMING_INDEX ScalerHdmiMacTx0RepeaterGetTxOutputModeTiming(TX_OUT_MODE_SELECT_TYPE mode)
{
    if(mode >= TX_OUT_MODE_NUM){
        DebugMessageHDMITx("[HDMITX] INVALID TX_OUT_MODE[%d]@ScalerHdmiMacTx0RepeaterGetTxOutputModeTiming()\n", (UINT32)mode);
        return TX_TIMING_NUM;
    }

    // SHOULD NOT HAPPEN
    #if 0
    if(ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(EDID_TIMING_ANY) == 0){
        DebugMessageHDMITx("[HDMITX] ABNORMAL TIMING_COUNT\n");
        return TX_TIMING_NUM;
    }
    #endif

    FatalMessageHDMITx("[HDMITX] OutputModeTiming(%d)\n", (UINT32) mode);;
    FatalMessageHDMITx("[HDMI] SCDC/MAX_FRL=%x/%x\n",(UINT32)ScalerHdmiMacTx0EdidGetFeature(_SCDC_PRESENT),(UINT32)ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE));
    memset(&timing_info, 0, sizeof(MEASURE_TIMING_T));

#if (IS_ENABLED(CONFIG_RTK_HDMI_RX) || IS_ENABLED(CONFIG_ENABLE_DPRX)) && !IS_ENABLED(ENABLE_INTERNAL_HDMIRX_PTG)//CONFIG_HDMITX_AP_TEST
    if((mode == TX_OUT_SOURCE_MODE)|| (mode == TX_OUT_THROUGH_MODE)){
        if(g_scaler_display_info.input_src == 0){
            #if IS_ENABLED(CONFIG_RTK_HDMI_RX)
            drvif_Hdmi_TimingInfo_for_hdmitx(bypass_port,&timing_info);
            #endif
        }else{
            #if defined(CONFIG_ENABLE_DPRX)
            ptm_timing_info = drvif_Dprx_GetRawTimingInfo(0);
            #endif
        }

    }else if(mode == TX_OUT_TEST_MODE){
        SourceOutputPixelClk = 0;
        //test mode input as 2k60
        timing_info.v_total = 1125;
        timing_info.h_total = 2200;
        timing_info.v_act_len = 1080;
        timing_info.h_act_len = 1920;
        timing_info.v_freq = 6000;
    }

#else
#ifdef ENABLE_INTERNAL_HDMIRX_PTG
    if ((ScalerHdmiMacTx0EdidGetFeature(_SCDC_PRESENT) == _FALSE) || (!bDtdSupport4K60 && !bSVDSupport4K60))
    {
        InfoMessageHDMITx("[HDMITX] sink not support 4k60 ,return 2k60 timing\n");
        return HDMI_TX_1920_1080P_60HZ;
    }
#endif
  #ifdef ENABLE_HDMITX_PTG_AUTO_RUN // [FOR-TEST] HDMITX output mode == TX_OUTPUT_TIMING_DEFAULT
    if(TX_OUTPUT_TIMING_DEFAULT == TX_TIMING_HDMI21_1080P60_RGB_8BIT_3G){   // 1080p60 3G Test Pass
        InfoMessageHDMITx("[HDMI21_TX] FORCE HDMI21 1080p60@3G Output\n");
        return HDMI21_TX_1920_1080P_60HZ_3G;
    }
    else if(TX_OUTPUT_TIMING_DEFAULT == TX_TIMING_HDMI21_1080P60_RGB_8BIT_6G4L){    // 1080p60 6G4L Test Pass
        InfoMessageHDMITx("[HDMI21_TX] FORCE HDMI21 1080p60@6G4L Output\n");
        return HDMI21_TX_1920_1080P_60HZ_6G4L;
    }
    else if(TX_OUTPUT_TIMING_DEFAULT == TX_TIMING_HDMI21_4K2KP60_RGB_8BIT_6G4L){    // 4k2kp60 6G4L Test Pass
        InfoMessageHDMITx("[HDMI21_TX] FORCE HDMI21 4k2kp60@6G4L Output\n");
        return HDMI21_TX_3840_2160P_444_60HZ_6G4L;
    }
    else if(TX_OUTPUT_TIMING_DEFAULT == TX_TIMING_HDMI21_4K2KP60_RGB_8BIT_12G){ // 4k2kp60 12G Test Pass
        InfoMessageHDMITx("[HDMI21_TX] FORCE HDMI21 4k2kp60@12G Output\n");
        return HDMI21_TX_3840_2160P_444_60HZ_12G;
    }
    else if(TX_OUTPUT_TIMING_DEFAULT == TX_TIMING_HDMI21_4K2KP120_RGB_8BIT){
        InfoMessageHDMITx("[HDMI21_TX] FORCE HDMI21 4k2kp120@8G Output\n");
        return HDMI21_TX_3840_2160P_444_120HZ;
    }
    else if(TX_OUTPUT_TIMING_DEFAULT == TX_TIMING_HDMI21_4K2KP120_RGB_8BIT){
        InfoMessageHDMITx("[HDMI21_TX] FORCE HDMI21 4k2kp120@12G Output\n");
        return HDMI21_TX_3840_2160P_444_120HZ;
    }
    else if(TX_OUTPUT_TIMING_DEFAULT == TX_TIMING_HDMI21_4K2KP144_RGB_8BIT_10G){
        InfoMessageHDMITx("[HDMI21_TX] FORCE HDMI21 4k2kp144@10G Output\n");
        return HDMI21_TX_3840_2160P_444_144HZ_12G;
    }
    else if(TX_OUTPUT_TIMING_DEFAULT == TX_TIMING_HDMI21_4K2KP144_RGB_8BIT_12G){
        InfoMessageHDMITx("[HDMI21_TX] FORCE HDMI21 4k2kp144@12G Output\n");
        return HDMI21_TX_3840_2160P_444_144HZ_12G;
    }

  #endif // #ifdef ENABLE_HDMITX_PTG_AUTO_RUN // [FOR-TEST] HDMITX output mode == TX_OUTPUT_TIMING_DEFAULT
    timing_info.v_total = 2250;
    timing_info.h_total = 4400;
    timing_info.v_act_len = 2160;
    timing_info.h_act_len = 3840;
    timing_info.v_freq = 600;

#endif // #if defined(CONFIG_ENABLE_HDMIRX) && !defined(ENABLE_INTERNAL_HDMIRX_PTG)//CONFIG_HDMITX_AP_TEST

#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
    if((!lib_hdmitx_is_hdmi_21_support()) && (g_scaler_display_info.input_src == 0) && (drvif_Hdmi_Multi_port_GetFrlMode(bypass_port) != 0))
    {//when hdmi only support 2.0 but rx send 2.1timing,need show no signal
        return TX_TIMING_NUM;
    }
#endif
    if(lib_hdmitx_is_hdmi_limit_support() && (timing_info.pll_pixelclockx1024/100000 > TX_OUT_PIXEL_CLOCK_MAX_WO_DDR)) {
#if 0//def CONFIG_ENABLE_HDMIRX		
        if(!matchLimitTimingTable(timing_info.h_act_len, timing_info.v_act_len, timing_info.v_freq/10))
            return TX_TIMING_NUM;
#endif
    }

    tx_output_colorSpace =  timing_info.colorspace;
    tx_output_colordepth = timing_info.colordepth;
    time_mode = TX_TIMING_NUM;
    if(((!(timing_info.run_vrr &&  (g_scaler_display_info.input_src == 1))) || ! lib_hdmitx_industrial_ctrl_enable())&& ((mode == TX_OUT_SOURCE_MODE)|| (mode == TX_OUT_TEST_MODE))){
        timingMatchForSourceMode(mode);
    }else if(mode == TX_OUT_USER_MODE){
        time_mode = ScalerHdmiMacTx0RepeaterGetTxUserTimingMode();
    }else { // TX_OUT_THROUGH_MODE
        time_mode = ScalerHdmiMacTx0RepeaterTimingRxInputCheckSupport();
    }

    return time_mode;

}

#endif // #ifdef CONFIG_HDMITX_REPEATER_SUPPORT

