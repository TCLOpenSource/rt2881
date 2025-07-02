#ifndef _HDMITX_H_
#define _HDMITX_H_
//#include <scaler/scalerCommon.h>
//#include <mach/rtk_log.h>
#include <ioctrl/scaler/hdmitx_cmd_id.h>

#define TAG_NAME_HDMITX "HDMITX"
#define ROSTimeDly msleep


//int HDMITX_state_handler(void *data);
//int HDMITX_event_handler(void *data);

typedef enum{
  DCLK_SRC_DPLL=0,
  DCLK_SRC_HDMITX_DPLL=1
}DCLK_SRC_TYPE;


typedef union
{
    UINT32 regValue;
    struct {
        UINT32 uclimitPortNum: 3; // o: no limit, x: only output to RX portNum[x]
        UINT8 bCheckEdidInfo: 1; // check MNT name & portNum
        UINT8 bBypassLinkTraining: 1; // force bypass HPD/EDID/LinkTraining control flow
        UINT8 bEnablePtg: 1; // enable show TXSOC PTG
        UINT8 bTogglePtg: 1; // toggle TXSOC PTG
        UINT8 bEnableHdcp22: 1; // Enable HDCP2.2
        UINT8 ucDscMode: 3; // DSC mode: 0: disable, 1: default, 2: 12G (bbp=8.125), 3: 12G (bbp=9.9375), 4: 12G (bbp=12.000), 5: 12G (bbp=15.000)
        UINT8 bDisableHdcp22Rc: 1; // Disable HDCP22 RC
        UINT32 reserved: 20;
    };
}HdmiTxConfigPara1;

typedef union
{
    UINT32 regValue;
    struct {
        UINT32 ucLaneSrcCfg: 8; // 0xe4: H5 DemoDongle, 0xb1:  one board
        UINT32 ucTxPnSwapCfg: 4; // 0x0: default not swap, 0xf: all lane need swap
        UINT32 ucPlatformType:2; // 0: General, 1: O18, 2: O20
        UINT32 vrr_frame_rate:6; // VRR: TX output frame rate: 47~60
        UINT32 vrr_mode:2; // VRR Mode: 0: disable, 1: jump/2: sequence loop mode
        UINT32 vrr_loop_delay:7; // VRR loop delay: 0~127 vsync
        UINT32 vrr_24hz: 1; // VRR min frame rate support 24hz
        UINT32 reserved: 2;
    };
}HdmiTxConfigPara2;
typedef enum{
    TX_TIMING_INIT_STATE=0,
    TX_TIMING_NEED_CHANGE,
    TX_TIMING_FREERUN_CHANGE_DONE,
    TX_TIMING_WAIT_SETTING_DONE,
    TX_TIMING_SETTING_DONE,
} TX_STATUS;
#ifdef _MARK2_ZEBU_BRING_UP_LOCAL_RUN // [MARK2] FIX-ME -- only for local verify
extern bool kernel_finished(void);
#endif
UINT8 ScalerHdmiTxGetTxRunFlag(void);
extern UINT8 getVscInitdoneFlag(void);

void showTimingList(void);
void setHdmitxConfig(UINT32 param1, UINT32 param2);

void setHdmitxTiming(UINT32 frame_rate, UINT32 color_depth, UINT32 color_type);
void setHdmitxPatternGen(UINT32 width);
TX_STATUS ScalerHdmiTx_Get_Timing_Status(void);
void ScalerHdmiTx_Set_DtgClkSource(int index, DCLK_SRC_TYPE type);

void setHdmitxAudioChanel(UINT8 ch_num);
UINT8 getHdmitxAudioChanel(void);
void setHdmitxAudioType(UINT8 audio_type);
UINT8 getHdmitxAudioType(void);
void setHdmitxAudioMute(UINT8 audio_mute);
UINT8 getHdmitxAudioMute(void);
void setHdmitxAudioFreq(UINT32 audio_freq);
UINT32 getHdmitxAudioFreq(void);
extern UINT8 hdmitx_get_otp_status(void);


#endif // #ifndef _HDMITX_H_
