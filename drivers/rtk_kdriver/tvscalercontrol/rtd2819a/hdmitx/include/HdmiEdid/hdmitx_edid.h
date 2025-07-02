#ifndef __HDMITX_EDID_H__
#define __HDMITX_EDID_H__

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
// ID Code      : ScalerHdmiMacTx0.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_ctrl.h>
#include "../ScalerFunctionInclude.h"
//internel fanction
void ScalerHdmiMacTx0GetEdidVicTable(EnumEdidCtaDataBlock enumVDBType, UINT8 *pucVICTable, UINT16 *pusCtaDataBlockAddr);
void ScalerHdmiMacTx0SetSinkTmdsSupportClk(UINT8 ucHdmiVsdbMaxTMDSClk, UINT8 ucHfVsdbMaxTMDSClk);
void ScalerHdmiMacTx0EdidModifyEnable(BOOLEAN bEnable, EnumHdmiEdidModifyEvent enumEdidModifyEvent);
void ScalerHdmiMacTx0EdidChangeNotify(void);
UINT8 ScalerHdmiMacTx0RepeaterGetEdidTimingSupport(EnumTxOutTimingSupportType txOutTimingSupport);
#define HDMITX_CLK_DIFF         9


//banked function
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
typedef struct _EDID_DB_TYPE{
    UINT8 db_type:4;
    UINT8 bSupport420:4;
}EDID_DB_TYPE;
typedef struct _EDID_SUPPORT_TIMING_TYPE{
    UINT8 totalCnt;
    EDID_DB_TYPE db_type[EDID_SUPPORT_LIST_NUM];
    //BOOLEAN bSupport420[EDID_SUPPORT_LIST_NUM];
    UINT8 tx_index[EDID_SUPPORT_LIST_NUM];
}EDID_SUPPORT_TIMING_TYPE;
typedef enum _EDID_BLOCK_INDEX{
    EDID_BLOCK_0=0,
    EDID_BLOCK_1,
    EDID_BLOCK_2,
    EDID_BLOCK_3,
    EDID_EXT_DID_BLOCK_1,
    EDID_BLOCK_MAX
}EDID_BLOCK_INDEX;
#endif
typedef enum _TX_OUT_TIMING_PRIORITY_TYPE{
    TX_OUT_RGB_12B =0,
    TX_OUT_Y444_12B,
    TX_OUT_RGB_10B,
    TX_OUT_Y444_10B,
    TX_OUT_Y422_12B,
    TX_OUT_Y422_10B,
    TX_OUT_Y420_12B,
    TX_OUT_Y420_10B,
    TX_OUT_RGB_8B,
    TX_OUT_Y444_8B,
    TX_OUT_Y422_8B,
    TX_OUT_Y420_8B,
    TX_OUT_TIMING_PRIORITY_MAX_NUM
}TX_OUT_TIMING_PRIORITY_TYPE;
typedef enum _TX_OUT_TIMING_TYPE{
    TX_OUT_RGB_TIMING =0,
    TX_OUT_Y420_TIMING,
    TX_OUT_SYNC_RX_TIMING,
    TX_OUT_TIMING_MAX_NUM
}TX_OUT_TIMING_TYPE;

typedef enum _HD21_576i_480i_INDEX_TYPE{
    HD21_720_480I=0,
    HD21_720_576I,
    HD21_1440_480I,
    HD21_1440_576I,
}HD21_576i_480i_INDEX_TYPE;

typedef enum _HD20_576i_480i_INDEX_TYPE{
    HD20_720_480I=0,
    HD20_720_576I,
    HD20_1440_480I,
    HD20_1440_576I,
}HD20_576i_480i_INDEX_TYPE;
typedef struct {
    EnumHdmi21FrlType enFrlTyp;
    UINT32 u32BandWidth;
} hdmi_tx_bandwidth_st;
typedef struct {
    TX_OUT_TIMING_PRIORITY_TYPE enTimingPriority;
    UINT8 u8OutputDepth;
} hdmi_tx_outputDepth_st;

typedef struct {
    EnumHdmi21FrlType enFrlTyp;
    UINT16 maxPll;
} hdmi_tx_max_pll_st;
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
#ifdef FIX_ME_HDMITX_BRING_UP // [TBD]
#define _HDMI21_SCDC_SLAVE_ADDR                                                0x54 //0xA8>>1
#define _HDMI21_SCDC_OFFSET_1                                                  0x1
#define _HDMI21_SCDC_OFFSET_2                                                  0x2
#define _HDMI21_SCDC_OFFSET_10                                                 0x10
#define _HDMI21_SCDC_OFFSET_30                                                 0x30
#define _HDMI21_SCDC_OFFSET_31                                                 0x31
#define _HDMI21_SCDC_OFFSET_35                                                 0x35
#define _HDMI21_SCDC_OFFSET_40                                                 0x40
#define _HDMI21_SCDC_OFFSET_41                                                 0x41
#define _HDMI21_SCDC_OFFSET_42                                                 0x42
#define _HDMI21_SCDC_OFFSET_50                                                 0x50
#define _HDMI21_SCDC_OFFSET_59                                                 0x59
#endif // #ifndef FIX_ME_HDMITX_BRING_UP // [TBD]
typedef enum _EDID_CHECK_STATE{
    EDID_CHECK_BF_MODIFY=0,
    EDID_CHECK_AFT_MODIFY,
    EDID_CHECK_STATE_NUM
} EDID_CHECK_STATE;

#if(_HDMI_TX_CTS_TEST == _ON)
typedef struct
{
    UINT8 b4DTDNumNeedReplaced : 4;
    UINT16 usVideoDBVic5Addr;
}StructHDMICTSEDIDInfo;
#endif

typedef struct
{
    // Vendor
    UINT8 ucVendorModel;

    // General EDID Info
    UINT16 usHDMITxEDIDExtDTDStartAddr;
    UINT16 usDidPreferTimingAddr;
    UINT16 ucSRC_PHY_ADDR: 8;
    UINT8 b2EdidVer : 2;
    UINT8 b1YCbCr444Support : 1;
    UINT8 b1YCbCr422Support : 1;
    UINT8 b1EdidSegReadSupport : 1;
    UINT8 b1vsd8kSupport : 1;

    // 420 Relatived DB
    UINT8 b1YCbCr420Support : 1;

    // Viedo Capability DB
    UINT8 b1QYSupport : 1;
    UINT8 b1QSSupport : 1;

    // Freesync DB
    UINT8 b1DrrSupport : 1;

    // Audio DB
    UINT8 ucLpcmFsSupport : 7;
    UINT8 uc3DLpcmFsSupport : 7;

    // HDR Related DB

    // MISC
    UINT8 b1DidExist : 1;
    // --------------------
    // HDMI Relative
    // --------------------
    // HDMI VSDB
    UINT8 b1AISupport : 1;
    UINT8 b1DC_48bitSupport : 1;
    UINT8 b1DC_36bitSupport : 1;
    UINT8 b1DC_30bitSupport : 1;
    UINT8 b1DC_Y444Support : 1;
    UINT8 b1LagencyFeild_Present : 1;
    UINT8 b1ILagencyFeild_Present : 1;
    UINT8 b1HDMI_Video_Present : 1;
    // HF VSDB
    UINT8 b1DC_48bitSupportFor420 : 1;
    UINT8 b1DC_36bitSupportFor420 : 1;
    UINT8 b1DC_30bitSupportFor420 : 1;
    UINT8 b1SCDC_Present : 1;
    UINT8 b1LTE_340Mcsc_scramble : 1;
    UINT8 b1RR_Capable : 1;
    UINT8 b1DSC12_Support : 1;
    UINT8 b4Max_FRL_LR : 4;
    UINT8 b1DSC_Native420_Support : 1;
    UINT8 ucMaxTMDSClkSupport;
    UINT8 b1DSC_All_bpp : 1;
    UINT8 b1DSC_10bpc : 1;
    UINT8 b1DSC_12bpc : 1;
    UINT8 b1DSC_16bpc : 1;
    UINT8 b4DSC_Max_LR : 4;
    UINT8 b1FVA_Support : 1;
    UINT8 b4DSC_Max_Slices : 4;
    UINT8 b4DSC_TotalChunkKBytes : 6;
    UINT8 b6VRR_min : 6;
    UINT8 b8Edid_ExtBlockCount : 8;
    UINT16 b10VRR_max: 10;
    UINT8 b1FAPA_End_Extended : 1;
    UINT8 b1QMS : 1;
    UINT8 b1MDelta : 1;
    UINT8 b1CinemaVRR : 1;
    UINT8 b1NEG_MVRR : 1;
    UINT8 b1FVA : 1;
    UINT8 b13D_OSD_Disparity : 1;
    UINT8 b1Dual_View : 1;
    UINT8 b1Independent_view : 1;
    UINT8 b1BT2020RGB : 1;
    UINT8 b1BT2020YCC : 1;
    UINT8 b1BT2020cYCC : 1;
    UINT8 b1opRGB : 1;
    UINT8 b1opYCC601 : 1;
    UINT8 b1sYCC601 : 1;
    UINT8 b1xvYCC709 : 1;
    UINT8 b1xvYCC601 : 1;
    UINT8 b1ST2113RGB23 : 1;
    UINT8 b1ICTCP : 1;
    UINT8 b1HDR2084 : 1;
    UINT8 b1HDRHLG : 1;
}StructHDMIEDIDInfo;
#define GET_HDMI_MAC_TX0_MODIFY_EDID_ENABLE()                   (g_bHdmiMacTx0ModifyEdidEnable)
#define SET_HDMI_MAC_TX0_MODIFY_EDID_ENABLE()                   (g_bHdmiMacTx0ModifyEdidEnable = _TRUE)
#define CLR_HDMI_MAC_TX0_MODIFY_EDID_ENABLE()                   (g_bHdmiMacTx0ModifyEdidEnable = _FALSE)

#define GET_HDMI_MAC_TX0_EDID_MODIFY_EVENT()                    (g_ucHdmiMacTx0ModifyEdidEvent)
#define SET_HDMI_MAC_TX0_EDID_MODIFY_EVENT(x)                   (g_ucHdmiMacTx0ModifyEdidEvent = (x))

#define GET_HDMI_MAC_TX0_EDID_MODIFY_STATE()                    (g_ucHdmiMacTx0ModifyEdidState)
#define SET_HDMI_MAC_TX0_EDID_MODIFY_STATE(x)                   (g_ucHdmiMacTx0ModifyEdidState = (x))

#define SET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR(x)             (g_stHdmiMacTx0EDIDInfo.usHDMITxEDIDExtDTDStartAddr = (x))
#define GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR()              (g_stHdmiMacTx0EDIDInfo.usHDMITxEDIDExtDTDStartAddr)

#define SET_HDMI_MAC_TX0_EDID_EXT_BLOCK_COUNT(x)             (g_stHdmiMacTx0EDIDInfo.b8Edid_ExtBlockCount = (x))
#define GET_HDMI_MAC_TX0_EDID_EXT_BLOCK_COUNT()              (g_stHdmiMacTx0EDIDInfo.b8Edid_ExtBlockCount)

#define SET_HDMI_MAC_TX0_EDID_QS_SUPPORT(x)             (g_stHdmiMacTx0EDIDInfo.b1QSSupport = (x))
#define GET_HDMI_MAC_TX0_EDID_QS_SUPPORT()              (g_stHdmiMacTx0EDIDInfo.b1QSSupport)

#define SET_HDMI_EDID_HDMI_MODE()                (bEdidHdmiFormat = _TRUE)
#define CLR_HDMI_EDID_HDMI_MODE()                (bEdidHdmiFormat = _FALSE)
#define GET_HDMI_EDID_HDMI_MODE()                (bEdidHdmiFormat)

#define SET_ADJUST_VTOTAL_FLAG(x)                        (adjust_vtotal_flag = (x))
#define GET_ADJUST_VTOTAL_FLAG()                        (adjust_vtotal_flag)
#define SET_ADJUST_OUTPUT_VTOTAL(x)                (adjust_timing_vtotal = (x))
#define GET_ADJUST_OUTPUT_VTOTAL()                  (adjust_timing_vtotal)
#define SET_ADJUST_RGB_VTOTAL(x)                (adjust_rgb_vtotal = (x))
#define GET_ADJUST_RGB_VTOTAL()                  (adjust_rgb_vtotal)
#define SET_ADJUST_420_VTOTAL(x)                (adjust_420_vtotal = (x))
#define GET_ADJUST_420_VTOTAL()                  (adjust_420_vtotal)
#define SET_ADJUST_VRR_VSTART(x)                                   (adjust_vrr_vstart= (x))
#define GET_ADJUST_VRR_VSTART()                                   (adjust_vrr_vstart)


extern StructHDMIEDIDInfo g_stHdmiMacTx0EDIDInfo;
extern UINT8 g_sinkEDID4K444Support;
extern UINT8 g_sinkEDID4K420Support;
extern UINT8 adjust_vtotal_flag;
extern UINT16 adjust_timing_vtotal;
extern UINT16 adjust_rgb_vtotal;
extern UINT16 adjust_420_vtotal;
extern UINT16 adjust_vrr_vstart;


#endif
StructHDMITxEdidBufInfo* ApGetScalerHdmiMacTx0RepeaterEdid(void);
void ScalerHdmiMacTx0RepeaterOutputTimingInfo(void);
void ScalerHdmiMacTx0RepeaterEdidSupportDump(void);
UINT8 ScalerHdmiMacTx0RepeaterSetEdidSupport(EnumEdidFeatureType enumEdidFeature, UINT8 value);
UINT8 ScalerHdmiMacTx0RepeaterGetEdidSupport(EnumEdidFeatureType enumEdidFeature);
BOOLEAN ScalerHdmiMacTx0EdidIICRead(UINT8 ucReadType);
UINT8 ScalerHdmiMacTx0RepeaterSetTxUserTimingMode(TX_TIMING_INDEX time_mode);
#endif // #ifndef __SCALER_HDMI_MAC_TX0_H__
