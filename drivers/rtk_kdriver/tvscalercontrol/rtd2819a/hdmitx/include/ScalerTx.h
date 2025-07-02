#ifndef __SCALERTX_H__
#define __SCALERTX_H__

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
// ID Code      : ScalerTx.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include "ScalerFunctionInclude.h"
#include "HdmiMacTx/ScalerHdmiMacTxInclude.h"
#include "HdmiMacTx/ScalerHdmiMacTx.h"
#include "../include/hdmitx_phy_struct_def.h"
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_share.h>
//#include <scaler/scalerstruct.h>
//#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
//#include <scaler/scalerDrvCommon.h>
//#include <rtk_kdriver/tvscalercontrol/hdmirx/drvif_hdmirx.h>
//#endif
//****************************************************************************
// NUM DEFINITTIONS
//****************************************************************************
#define EDID_EXTENSION_TAG 0x2
#define EDID_BLOCK_MAP_TAG 0xf0
#define EDID_DID_EXTENSION_TAG 0x70
#define EDID_BLOCK1_TAG_BASE 0x80
#define EDID_BLOCK2_TAG_BASE 0x100
#define EDID_BLOCK3_TAG_BASE 0x180
#define EDID_BLOCK2_EXT_DTD_BASE 0x102
#define EDID_BLOCK2_EXT_DTD_ADDR 0x104
#define EDID_BLOCK3_EXT_DTD_BASE 0x182
#define EDID_BLOCK3_EXT_DTD_ADDR 0x184
#define EDID_MAX_BUFFER_SIZE 0x200 // 0x400//
#define EDID_BLOCK1_BASIC_SUPPORT_ADDR 0x83
#define EDID_BLOCK2_BASIC_SUPPORT_ADDR 0x103
#define EDID_BLOCK3_BASIC_SUPPORT_ADDR 0x183
#define EDID_SUPPORT_NON_DSC_PIXELCLOCK 13600
#define TX_OUT_PIXEL_CLOCK_MAX_W_DDR 14240
#define TX_OUT_PIXEL_CLOCK_MAX_WO_DDR 6000
#define TX_OUT_PIXEL_CLOCK_MAX_WO_SCDC 3400

// PLL*24*18/16 <= data rate
#define HDMITX_FRL_NONE_MAX_PLL 6000
#define HDMITX_FRL_3G_MAX_PLL 3333
#define HDMITX_FRL_6G3L_MAX_PLL 6666
#define HDMITX_FRL_6G4L_MAX_PLL 8888
#define HDMITX_FRL_8G_MAX_PLL 11851
#define HDMITX_FRL_10G_MAX_PLL 14814
#define HDMITX_FRL_12G_MAX_PLL 17777

#define EDID_SUPPORT_LIST_NUM 64
#define EDID_STR_MAX 24

#define AUDIO_INFO_PACKET_SIZE 4

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT // [FIX-ME] Copy From Hdmi_emp.h (drivers\rtk_kdriver\tvscalercontrol\hdmirx)
#define EM_CVTEM_INFO_OFST              0
#define EM_VTEM_INFO_OFST               192
#define EM_VSEM_INFO_OFST               224

#define EM_CVTEM_INFO_LEN               192
#define EM_VTEM_INFO_LEN                32
#define EM_VSEM_INFO_LEN                32

#define EM_HDR_EMP_CNT_OFST             0
#define EM_HDR_EMP_INFO_OFST            4
#define MAX_EM_HDR_INFO_LEN             736

#define NO_VTEM_PKT_CNT_MAX         10


typedef struct {
    UINT8    FVA_Factor_M1;     //MD0[7:4]
    UINT8    M_CONST;           //MD0[1]
    UINT8    VRR_EN;            //MD0[0]
    UINT8    Base_Vfront;       //MD1[7:0]
    UINT8    RB;                //MD2[2]
    UINT16   Base_Refresh_Rate; //MD2[1:0] ~ MD3[7:0]
    UINT8    reserved;
} HDMI_EM_VTEM_T;
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT // [FIX-ME] Copy From Hdmi_emp.h (drivers\rtk_kdriver\tvscalercontrol\hdmirx)


//****************************************************************************
// STRUCT  DEFINITTIONS
//****************************************************************************
typedef struct
{
    UINT8 b4Reserved: 4;
    UINT8 b1DfpDeviceEditReadFlag: 1;
    UINT8 b2StreamType: 2;
    UINT8 b1PowerStatus: 1;
}StructHdmiOutputStatus;

typedef enum
{
    DOLBY_HDMI_VSIF_DISABLE = 0,
    DOLBY_HDMI_VSIF_STD = 1,//dolby vsif standard foramt
    DOLBY_HDMI_VSIF_LL = 2,//dolby vsif low latecy foramt
    DOLBY_HDMI_h14B_VSIF = 3,//hdmi 1.4 vsif dolby format
}DOLBY_HDMI_VSIF_T;

typedef struct{
    UINT32 addr;
    UINT32 mask;
    UINT8 status;
    UINT32 time_90k;
    UINT8 timecount;
}StructEventPollingInfo;
//#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
typedef struct{
    UINT8 hdmiVsifAllmMode;
    UINT8 empDvMode;
    UINT8 empVtemVrrMode;
    UINT8 sdpAmdFreeSyncMode;
    HDR_MODE hdr_mode;
    DOLBY_HDMI_VSIF_T dvVsifMode;
    #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    HDMI_VSI_T hdmi_dvVsiInfo;
    HDMI_AVI_T hdmi_aviInfo;
    HDMI_DRM_T hdmi_drmInfo;
    UINT8 hdmi_empVtemBuf[EM_VTEM_INFO_LEN];
    #endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
}StructHdmiRepeaterSourceInfo;
//#endif
//****************************************************************************
// MACRO/DEFINITION
//****************************************************************************
#define CLR_OUTPUT_POWER_STATUS() (g_stHdmiOutputStatus.b1PowerStatus = _POWER_STATUS_OFF)
#define SET_OUTPUT_POWER_STATUS(x) (g_stHdmiOutputStatus.b1PowerStatus = (x))
#define GET_OUTPUT_POWER_STATUS() (g_stHdmiOutputStatus.b1PowerStatus)

#define CLR_OUTPUT_STREAM_TYPE(x) (g_stHdmiOutputStatus.b2StreamType = _STREAM_TYPE_NONE)
#define SET_OUTPUT_STREAM_TYPE(x) (g_stHdmiOutputStatus.b2StreamType = (x))
#define GET_OUTPUT_STREAM_TYPE(x) (g_stHdmiOutputStatus.b2StreamType)

#define CLR_OUTPUT_DFP_DEVICE_EDID_READ_FLG(x) (g_stHdmiOutputStatus.b1DfpDeviceEditReadFlag = _FALSE)
#define SET_OUTPUT_DFP_DEVICE_EDID_READ_FLG(x) (g_stHdmiOutputStatus.b1DfpDeviceEditReadFlag = _TRUE)
#define GET_OUTPUT_DFP_DEVICE_EDID_READ_FLG(x) (g_stHdmiOutputStatus.b1DfpDeviceEditReadFlag)

#define ScalerHdmiTxGetMute() (ucTxMuteEnFlag)
#define ScalerHdmiTxGetVRRFlagEn() (ucVRRFlagEn)
#define ScalerHdmiTxGetVTEMpacektEn() (ucVTEMpacketEn)

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
#define ScalerHdmiTxRepeaterGetSwPktEn(swPktType) (hdmiRepeaterSwPktEnMask & (UINT32)swPktType? _TRUE: _FALSE)
#define ScalerHdmiTxRepeaterGetSourceInfo_Allm() (hdmiRepeaterSrcInfo.hdmiVsifAllmMode)
#define ScalerHdmiTxRepeaterGetSourceInfo_hdrMode() (hdmiRepeaterSrcInfo.hdr_mode)
#define ScalerHdmiTxRepeaterGetSourceInfo_DvVsifMode() (hdmiRepeaterSrcInfo.dvVsifMode)
#define ScalerHdmiTxRepeaterGetSourceInfo_vtemVrrEn() (hdmiRepeaterSrcInfo.empVtemVrrMode)
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT

#define ScalerHdmiMacTxPxMapping(x) (_P0_OUTPUT_PORT)
#define ScalerHdmiTxRepeaterGetBypassPktHdrEn(bypassPktHdrType) (hdmiRepeaterBypassPktHdrEnMask & (UINT32)bypassPktHdrType? _TRUE: _FALSE)
#define GET_STREAM_HDR_INFO_FRAME_RECEIVED() (0)

#define ScalerHdmiTxGetTargetFrlRate() (HDMITX_OUT_FRL_RATE)
#define SCALER_HDMITX_SET_TARGET_FRLRATE(x) (HDMITX_OUT_FRL_RATE=x)

#ifdef HDMITX_TIMING_TABLE_CONFIG
extern EnumHdmi21FrlType ScalerHdmiTxPhy_GetPLLTimingInfo_frlRate(TX_TIMING_INDEX format_idx, I3DDMA_COLOR_DEPTH_T colorDepthIdx);
#else
#define ScalerHdmiTxPhy_GetPLLTimingInfo_frlRate(x,y) (HDMITX_OUT_FRL_RATE)
#endif

#define ScalerHdmiTxGetFrlNewModeEnable(x) (_DISABLE)

//#define SEC(x) (90000*x)
#define ScalerTimerDelayXms msleep

#define DELAY_5US(x) udelay(5)
#define HDMITX_DETEC_TASKID ROS_TASK_HDMITX_STATE_TASK

//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************
extern StructHdmiOutputStatus g_stHdmiOutputStatus;

extern UINT8 ucTxMuteEnFlag;
extern UINT8 ucVRRFlagEn;
extern UINT8 ucVTEMpacketEn;
//#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
extern StructHdmiRepeaterSourceInfo hdmiRepeaterSrcInfo;
extern UINT32  hdmiRepeaterBypassPktHdrEnMask;
//#endif

extern EnumHdmi21FrlType HDMITX_OUT_FRL_RATE;

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
extern UINT32  hdmiRepeaterSwPktEnMask;
#endif

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************


//****************************************************************************
// FUNCTION define
//****************************************************************************
UINT8 Scaler_Wait_for_LineCount(void);

extern UINT8 HDMITX_DTG_Wait_Den_Stop_Done(void);
extern UINT8 HDMITX_DTG_Wait_Den_Start(void);
extern UINT8 HDMITX_DTG_Wait_vsync_start(void);

//#endif

UINT16 ScalerDpStreamGetElement(EnumOutputPort ucOutputPort, EnumMultiStreamElement enElement);
StructTimingInfo *ScalerDpStreamGetDisplayTimingInfo(UINT8 ucOutputPort);
//StructStreamDisplayCompensateInfo *ScalerDpStreamGetDisplayCompensateInfo(UINT8 ucOutputPort);
void ScalerColorStream422To444(UINT8 ucOutputPort, UINT8 enable);
UINT8 ScalerColorStream422to444Support(UINT8 ucOutputPort);
UINT8 ScalerColorStream444to420Support(UINT8 ucOutputPort);
UINT8 ScalerColorStream422to420Support(UINT8 ucOutputPort);
void ScalerColorStreamYuv2Rgb(UINT8 ucOutputPort, UINT8 enable);
UINT16 ScalerDscDecoderGetElement(EnumOutputPort ucOutputPort, EnumDscDecoderElement enElement);
#ifdef NOT_USED_NOW
UINT8 ScalerHdmiPhyTx0SetIBHNTrim(UINT8 ucIBHNTRIMCurrent);
UINT8 ScalerHdmiPhyTx0GetIBHNTrim(void);
UINT8 ScalerHdmiPhyTx0PowerOn(void);
UINT8 ScalerHdmiPhyTx0CMUPowerOff(void);
UINT8 ScalerHdmiPhyTx0SetCMUSignal(BOOLEAN enable);
void ScalerColorStream444To420(UINT8 ucOutputPort, UINT8 enable);
void ScalerColorStream422To420(UINT8 ucOutputPort, UINT8 enable);
void ScalerColorStreamYuv2RgbClamp(UINT8 ucOutputPort, UINT8 enable);
void ScalerColorStreamRgb2Yuv(UINT8 ucOutputPort, UINT8 enable, EnumColorSpace type);
#endif
void ScalerTxHdmiTx0PhyConfig(void);
TX_TIMING_INDEX ScalerHdmiTxGetVideoFormatIndex_byParam(UINT16 width, UINT16 height, UINT8 isProg, UINT16 frame_rate, UINT8 bDscEn, UINT8 colorSpace);
TX_TIMING_INDEX ScalerHdmiTxGetTx0VideoFormatIndex(void);
TX_TIMING_INDEX ScalerHdmiTxGetScalerVideoFormatIndex(void);
UINT16 Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_INFO infoList);
void Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_INFO infoList, UINT16 value);

void ScalerHdmiTxMuteEnable(UINT8 enable);
void ScalerHdmiTxSetMute(UINT8 enable);
void ScalerHdmiTxSetChipVersion(UINT8 value);
UINT8 ScalerHdmiTxGetChipVersion(void);
void ScalerHdmiTxSetFrlNewModeEnable(UINT8 enable);
void ScalerHdmiTxSetDSCMuteEn(UINT8 enable);
UINT8 ScalerHdmiTxGetDSCMuteEn(void);
void ScalerHdmiTxMuteEnable(UINT8 enable);


void ScalerMultiStreamSetEdidReadStatus(UINT8 ucOutputPort, UINT8 flag);
UINT8 ScalerMultiStreamGetEdidReadStatus(UINT8 ucOutputPort);

void ScalerHdmiTxSetUpdateVoInfo(UINT8 enable);
UINT8 ScalerHdmiTxGetUpdateVoInfoFlag(void);

UINT8 ScalerDpRx0GetDpcdBitInfo(UINT8 par1, UINT8 par2, UINT8 par3, UINT8 par4) ;

void ScalerSetTimerEvent(struct timer_list *timer);
INT32 ScalerTimerActiveTimerEvent(UINT32 time_ms, EnumScalerTimerEventHdmiTx0Type event);
INT32 ScalerTimerCancelTimerEvent(EnumScalerTimerEventHdmiTx0Type event);
INT32 ScalerTimerSearchActiveTimerEvent(EnumScalerTimerEventHdmiTx0Type event);
INT32 ScalerTimerReactiveTimerEvent(UINT32 time_ms, EnumScalerTimerEventHdmiTx0Type event);
INT32 HDMITX_ScalerTimerPollingFlagProc(UINT32 pts_90k, UINT32 regAddr, UINT32 mask, UINT8 status);
INT32 ScalerTimerWDActivateTimerEvent_EXINT0(UINT32 pts_90k, EnumScalerWdTimerEventHdmiTx0Type event);


#if(_HDMI_TX_SUPPORT == _ON)
void ScalerTxHdcpDownLoadHdmiTxAKey(UINT16 usSubAddr, UINT16 usLength, UINT8 *pucReadArray);
void ScalerTxHdcpDownLoadHdmiTxAKsv(UINT8 ucSubAddr, UINT8 ucLength, UINT8 *pucReadArray);
#endif

UINT8 ScalerHdmiPhyTx0Initial(void);
UINT8 ScalerHdmiPhyTx0Set(void);
UINT8 ScalerHdmiPhyTx0SetZ0Detect(BOOLEAN enable);
EnumHDMITxZ0Detect ScalerHdmiPhyTx0Z0Detect(void);

UINT8 ScalerHdmiPhyTx0PowerOff(void);

EnumDPRxHDCPUpstreamAuthEvent ScalerDpHdcp14RxGetUpstreamEvent(UINT8 ucOutputPort);
EnumDPRxHDCPUpstreamAuthEvent ScalerDpHdcp2RxGetUpstreamEvent(UINT8 ucOutputPort);
void ScalerHdcpTxStoredReceiverId(UINT8 *pucReceiverIdList, UINT8 device_count);

void ScalerDpStreamGetMsaTimingInfo(EnumOutputPort ucOutputPort, StructTimingInfo *stTimingInfo);
void ScalerHdmiTxHPDInitial(void);

UINT16 ScalerEdidGetMaxDdcramSize(EnumOutputPort ucOutputPort);
UINT16 ScalerEdidCheckSumCal(UINT8 *pucDdcRamAddr,UINT16 usStartAddr);

void ScalerEdidGeneralStructParse(EnumOutputPort ucOutputPort);
void ScalerEdidExtBlockDbParse(EnumOutputPort ucOutputPort);

void ScalerHDCP2TxCipherEncrypt(UINT8 *ksArray,UINT8 *lc128Array, UINT8 *rivArray);
void ScalerHDCP2TxRegCTRL(void);
UINT8 GET_STREAM_HDR_INFO_FRAME_CHANGE(void);
//EnumHdmi21FrlType ScalerHdmiTxGetTargetFrlRate(void);
EnumHdmitxTimingStdType ScalerHdmiTxGetTimingStandardType(void);
void ScalerHdmiTxSetFllTrackingEnable(UINT8 enable);
UINT8 ScalerHdmiTxGetFllTrackingEnable(void);

void ScalerHdmiTxSetFastTimingSwitchEn(UINT8 enable);
UINT8 ScalerHdmiTxGetFastTimingSwitchEn(void);
void ScalerHdmiTxSetPixelFormatChange(UINT32 status);
UINT32 ScalerHdmiTxGetPixelFormatChange(void);
#ifdef NOT_USED_NOW
INT8 ScalerHdmiTxSendTxConnectStatus(UINT8 bUnPlugFlag);
#endif
UINT8 ScalerHdmiTxGetHdmiTxOutputSupport(void);
void ScalerHdmiTxSetHDRFlag(UINT8 bHDRFlag);
UINT8 ScalerHdmiTxGetHDRFlag(void);

void ScalerHdmiTxSetDSCEn(UINT8 enable);
UINT8 ScalerHdmiTxGetDSCEn(void);
void ScalerHdmiTxSetVTEMpacektEn(UINT8 enable);
void ScalerHdmiTxSetVRRFlagEn(UINT8 enable);

#if(_4K2K_EDID_MODIFY_SUPPORT == _ON)
void ScalerTxEdidReduceModifiedFor4K(UINT8 *pucDdcRamAddr);
#endif

#if((_4K2K_EDID_MODIFY_SUPPORT == _ON) || (_HDMI_TX_SUPPORT == _ON))
BOOLEAN ScalerTxEDIDCheckSumCal(UINT8 *pucDdcRamAddr, UINT16 usStartAddr);
#endif

//#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
void ScalerHdmiTxRepeaterClrSourceInfo(void);

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
UINT8 ScalerHdmiTxRepeaterSetSwPktEn(EnumHdmiRepeaterSwPktType swPktType, UINT8 enable);
void ScalerHdmiTxRepeaterSetSourceInfo_aviInfoFrame(void);
void ScalerHdmiTxRepeaterSetSourceInfo_Allm(UINT8 enable);
void ScalerHdmiTxRepeaterSetSourceInfo_hdrMode(HDR_MODE hdr_mode);
void ScalerHdmiTxRepeaterSetSourceInfo_DvVsifMode(DOLBY_HDMI_VSIF_T mode);
void ScalerHdmiTxRepeaterSetSourceInfo_vtemVrrEn(UINT8 enable);
#endif

UINT8 ScalerHdmiTxRepeaterGetSourceInfo_aviInfoFrame(UINT8 *aviInfoFrame);


UINT16 ScalerDpStreamGetElement(EnumOutputPort ucOutputPort, EnumMultiStreamElement enElement);
//UINT8 ScalerHdmiTxRepeaterGetSourceInfo_hdrDrmInfo_bankIF(UINT8 *hdmi_drmInfo);
UINT8 ScalerStreamAudioGetChannelCount(EnumOutputPort ucOutputPort);
UINT8 ScalerStreamAudioGetCodingType(EnumOutputPort ucOutputPort);
UINT8 ScalerStreamAudioGetSamplingFrequencyType(EnumOutputPort ucOutputPort);
UINT8 ScalerStreamGetAudioMuteFlag(EnumOutputPort ucOutputPort);
#endif // #ifndef __SCALERTX_H__
