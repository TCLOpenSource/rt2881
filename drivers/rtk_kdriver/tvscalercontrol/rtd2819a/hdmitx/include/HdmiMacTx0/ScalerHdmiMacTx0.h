#ifndef __SCALER_HDMI_MAC_TX0_H__
#define __SCALER_HDMI_MAC_TX0_H__

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
#include "../ScalerFunctionInclude.h"
#include "../HdmiMacTx/ScalerHdmiMacTxInclude.h"
#include "ScalerHdmiMacTx0Include.h"

#if(_HDMI_MAC_TX0_SUPPORT == _ON)
//****************************************************************************
// include header files
//****************************************************************************
//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#define _HDMI_MAC_TX0_PRE_KEEP_OUT_SIZE                               50

// Feature for STREAM off flow
#define _HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING                       _OFF // _ON //

// HDMITX21 FRL error check
#define HDMITX21_MAC_HDMI21_FRL_ERR 0xf8 // sram_ovf(_BIT7)| borrow_fail(_BIT6)| blank_video_err(_BIT5)| sup_blk_err(_BIT4)| input_tb_ovf (_BIT3)

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
typedef enum
{
    _HDMI_TX_NOT_HDCP_AUTH = 0x00,
    _HDMI_TX_HDCP_AUTH_DISCONNECT,
    _HDMI_TX_HDCP_AUTH_HANDLE,
    _HDMI_TX_HDCP_AUTH_AND_CLR_AUTH_STATE,
} EnumHDMITxHdcpState;

typedef enum
{
    _HDMI_TX_GCP_TOHW_INIT = 0x00,
    _HDMI_TX_GCP_TOHW_JUMP,
    _HDMI_TX_GCP_TOHW_DONE,
} EnumHDMITxGcpToHwState;
//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************


//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
extern void ScalerHdmiMacTx0AcOnInitial(void);
extern void ScalerHdmiMacTx0HPDProc(void);
extern void ScalerHdmiMacTx0JudgeHandler(void);
extern void ScalerHdmiMacTx0ModifyEdid(void);
extern void ScalerHdmiMacTx0ModeHandler(void);
extern void ScalerHdmiMacTx0StateChangeEvent(EnumHDMITxModeStatus enumModeState);
extern BOOLEAN ScalerHdmiMacTx0StreamHandler(void);

extern void ScalerHdmiMacTx0ReduceTMDSClk(void);
extern void ScalerHdmiMacTx0ColorSpaceConvert(void);
#if(_HDMI20_MAC_TX0_SUPPORT == _ON)
extern void ScalerHdmiMacTx0ConvertTo420(void);
#endif
extern void ScalerHdmiMacTx0GetStreamTimingInfo(UINT8 ucOutputPort);
extern void ScalerHdmiMacTx0MeasureInputInfo(UINT8 ucOutputPort);

extern void ScalerHdmiMacTx0Reset(void);

extern BOOLEAN ScalerHdmiMacTx0MonitorFIFOStatus(void);
extern BOOLEAN ScalerHdmiMacTx0OutputConfiguration(void);
extern BOOLEAN ScalerHdmiMacTx0PeriodicSRAMPacking(void);
extern  BOOLEAN ScalerHdmiMacTx0FillPktData(UINT16 usPeriodicSRAMAddress, EnumHDMITxPacketType enumHdmiPktType, BOOLEAN bDffSel);

//extern void ScalerHdmiMacTx0FillAVIInfoPacket(void);

//extern void ScalerHdmiMacTx0FillVtemPacket(void);
#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
extern void ScalerHdmiMacTx0HDRSet(void);
//extern void ScalerHdmiMacTx0FillHDRPacket(void);
#endif
#if(_HDMI_FREESYNC_TX0_SUPPORT == _ON)
extern void ScalerHdmiMacTx0FreesyncSet(void);
//extern void ScalerHdmiMacTx0FillFreesyncPacket(void);
#endif
extern void ScalerHdmiMacTx0PowerOff(void);
extern void ScalerHdmiMacTx0InFIFO(void);
extern void ScalerHdmiMacTx0HVPol(void);
extern void ScalerHdmiMacTx0DPCSRAM(void);
extern void ScalerHdmiMacTx0Scheduler(void);
extern void ScalerHdmiMacTx0DataIslandPacking(void);
extern void ScalerHdmiMacTx0PktFineTune(void);
extern void ScalerHdmiMacTx0GCPPacket(void);
#if(_HDMI_TX_CTS_TEST == _ON)
//extern void ScalerHdmiMacTx0FWFillGCPPacket(void);
//extern void ScalerHdmiMacTx0FWFillGCPPacket_MuteCtrl(void);
void ScalerHdmiMacTx0SetSwGcpPktMuteEn(void);
#endif
//extern BOOLEAN ScalerHdmiMacTx0PktDataApplyToPSRAM(void);
//extern void ScalerHdmiMacTx0FillNullPacket(void);
//extern void ScalerHdmiMacTx0FillVendorSpecificInfoPacket(void);
//extern void ScalerHdmiMacTx0FillDolbyVisionVsifPacket(void);
#if(_AUDIO_SUPPORT == _ON)
extern void ScalerHdmiMacTx0ACRPacket(UINT8 audFmtChange);
//extern void ScalerHdmiMacTx0FillAudioInfoPacket(void);
extern BOOLEAN ScalerHdmiMacTx0AudioSet(void);
extern BOOLEAN ScalerHdmiMacTx0DetectHdcpRekeyWindowChg(void);
extern void ScalerHdmiMacTx0AudioPowerOff(void);
extern UINT8 ScalerHdmiMacTx0CalAudioSampleNuminOnePacket(void);
#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
extern UINT16 ScalerHdmiMacTx0CtsCodeCheckEventPeriod(void);
extern void ScalerHdmiMacTx0CtsCodeHandler(void);
extern void ScalerHdmiMacTx0UpdateFixedCTS(void);
#endif
#if(_HDMI_3D_AUDIO_TX0_SUPPORT == _ON)
extern void ScalerHdmiMacTx0AudioMetadataPacket(void);
#endif
#endif
extern BOOLEAN ScalerHdmiMacTx0ReloadEdid(void);
extern void ScalerHdmiMacTx0ColorDepthDecision(void);
extern void ScalerHdmiMacTx0RepetitionDecision(void);
extern void ScalerHdmiMacTx0DeepColorClkCal(void);
extern void ScalerHdmiMacTx0PowerGroupPSMode(BOOLEAN bOn);

#if(_HDMI_MAC_TX0_PURE == _ON)
void ScalerHdmiMacTx0SetClkDataTransmit(BOOLEAN bEnable);
void ScalerHdmiMacTx0SetClkDiv4(BOOLEAN bEnable);
#endif

#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)
extern void ScalerHdmiMacTx0SCDCProcHandler(void);
extern void ScalerHdmiMacTx0SCDCScrambleEnable(void);
extern void ScalerHdmiMacTx0SCDCScrambleCheck(void);
extern void ScalerHdmiMacTx0SCDCSetTMDSConfiguration(void);
extern void ScalerHdmiMacTx0SCDCScrambleDisable(void);
#endif

extern BOOLEAN ScalerHdmiMacTx0EdidIICRead(UINT8 ucReadType);
extern BOOLEAN ScalerHdmiMacTx0CheckEdidDifference(void);
extern void ScalerHdmiMacTx0SpecialSinkEdidDetect(void);
extern void ScalerHdmiMacTx0EdidFreesyncCapibilityModify(UINT8 *pucDdcRamAddr, UINT16 *pusCtaDataBlockAddr);


#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
extern void ScalerHdmiMacTx0EdidVideoBandwidthModify(UINT8 *pucDdcRamAddr, UINT16 *pusCtaDataBlockAddr, UINT16 *pusDidDataBlockAddr);
#endif

extern void ScalerHdmiMacTx0EdidAnalysis(void);
extern void ScalerHdmiMacTx0EdidModify(void);
extern void ScalerHdmiMacTx0PlugNotify(void);
void ScalerHdmiMacTx0UnplugNotify(void);
extern void ScalerHdmiMacTx0EdidChangeReset(void);

INT8 ScalerHdmiMacTx0RepeaterSetEdidTimingSupport(EnumTxOutTimingSupportType txOutTimingSupport, UINT8 value);
TX_TIMING_INDEX ScalerHdmiMacTx0RepeaterPopSinkTxSupportTimingList(EDID_TIMING_DB edid_timing_type, UINT8 index, UINT8 *db_type, BOOLEAN *bSupport420);
//TX_OUT_MODE_SELECT_TYPE ScalerHdmiMacTx0RepeaterGetTxOutputMode(void);
extern void ScalerHdmiMacTx0NCtsRegenConfig(void);
void ScalerHdmiMacTx0AudioSamplePktProc(UINT8 ucAudioSampleNuminOnePacket);
void ScalerHdmiMacTx0RepeaterShowEdidNotSupport(void);

#endif

#endif // #ifndef __SCALER_HDMI_MAC_TX0_H__
