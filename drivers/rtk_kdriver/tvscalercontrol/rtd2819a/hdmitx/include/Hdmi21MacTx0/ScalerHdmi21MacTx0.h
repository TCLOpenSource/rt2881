#ifndef __SCALER_HDMI21_MAC_TX0_H__
#define __SCALER_HDMI21_MAC_TX0_H__

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
// ID Code      : ScalerHdmi21MacTx0.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------


#if(_HDMI21_MAC_TX0_SUPPORT == _ON)
//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#define _FRL_CNT_OFFSET_NONE         0
#define _FRL_CNT_OFFSET_2            2

#define _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE                               70

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
typedef struct {
    UINT8 hsw_unvalid;
    UINT8 h_bporch_unvalid;
    UINT8 h_fporch_unvalid;
} hdmi_tx_timing_unvalid_st;

//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************


//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
extern void ScalerHdmi21MacTx0SetDscEncoderParameter(void);
extern BOOLEAN ScalerHdmi21MacTx0OutputConfiguration(void);
extern void ScalerHdmi21MacTx0InputConverter(void);
extern void ScalerHdmi21MacTx0VideoSet(void);
extern void ScalerHdmi21MacTx0VideoPllSet(void);
extern void ScalerHdmi21MacTx0Scheduler(void);
extern BOOLEAN ScalerHdmi21MacTx0PeriodicSRAMPacking(void);
extern BOOLEAN ScalerHdmi21MacTx0FillPktData(UINT16 usPeriodicSRAMAddress, EnumHDMITxPacketType enumHdmiPktType, BOOLEAN bDffSel);
extern void ScalerHdmi21MacTx0FillAVIInfoPacket(void);
extern BOOLEAN ScalerHdmi21MacTx0PktDataApplyToPSRAM(void);
extern void ScalerHdmi21MacTx0FillNullPacket(void);
extern void ScalerHdmi21MacTx0SetFRLVideoStartCnt(UINT8 ucFrlFineTune);
extern BOOLEAN ScalerHdmi21MacTx0FRLVideoStartCntCheck(void);
extern void ScalerHdmi21MacTx0FRLVideoStartCntRetry(void);
extern BOOLEAN ScalerHdmi21MacTx0FRLSRAMCheck(void);
extern BOOLEAN ScalerHdmi21MacTx0DetectHdcpRekeyWindowChg(void);
#if(_AUDIO_SUPPORT == _ON)
extern BOOLEAN ScalerHdmi21MacTx0AudioSet(void);
extern UINT8 ScalerHdmi21MacTx0CalAudioSampleNuminOnePacket(void);
extern void ScalerHdmi21MacTx0AudioSamplePktProc(UINT8 ucAudioSampleNuminOnePacket);
extern void ScalerHdmi21MacTx0ACRPacket(UINT8 audFmtChange);
extern void ScalerHdmi21MacTx0FillAudioInfoPacket(void);
#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
extern void ScalerHdmi21MacTx0CtsCodeHandler(void);
#endif
#endif
extern void ScalerHdmi21MacTx0FRLReset(BOOLEAN bBorrowEnable);
extern void ScalerHdmi21MacTx0DscPPSPacket(void);
extern void ScalerHdmi21MacTx0DscSetting(void);

#endif // End of #if(_HDMI_TX_SUPPORT == _ON)


void ScalerHdmiTx0_Dsc_inputSrcConfig(EnumHDMITXDscSrc dsc_src);
extern void ScalerHdmi21MacTx0NCtsRegenConfig(void);


#endif // #ifndef __SCALER_HDMI21_MAC_TX0_H__
