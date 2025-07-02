#ifndef __HDMITX_LIB_H__
#define __HDMITX_LIB_H__
#include <rtk_kdriver/tvscalercontrol/hdmitx/rtk_hdmitx.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_ctrl.h>
/************************
// Number/Functoin Definition
//
*************************/
#define _LPCM_ASP                                           0x00
#define _HBR_ASP                                            0x01


/************************
// Enum type define
//
*************************/
typedef enum
{
    DSC_SRC_TXSOC=0,
    DSC_SRC_DISPD,
}EnumHDMITXDscSrc;

typedef enum _HDMI_TX_FFE_TABLE_LIST{
    HDMI21_TX_FFE_TABLE_LIST_TV001=0,
    HDMI21_TX_FFE_TABLE_LIST_TV006,
    HDMI21_TX_FFE_TABLE_LIST_NUM,
}HDMI_TX_FFE_TABLE_LIST;

typedef enum _EnumOutputPort{
    _P0_OUTPUT_PORT=0,
    _P1_OUTPUT_PORT,
    _P2_OUTPUT_PORT,
    _P3_OUTPUT_PORT,
    _P4_OUTPUT_PORT
}EnumOutputPort;

typedef enum _EnumAudioFreqType{
    _AUDIO_FREQ_NO_AUDIO=0,
    _AUDIO_FREQ_32K,
    _AUDIO_FREQ_64K,
    _AUDIO_FREQ_128K,
    _AUDIO_FREQ_44_1K,
    _AUDIO_FREQ_88_2K,
    _AUDIO_FREQ_176_4K,
    _AUDIO_FREQ_48K,
    _AUDIO_FREQ_96K,
    _AUDIO_FREQ_192K,
    _AUDIO_FREQ_256K,
    _AUDIO_FREQ_352_8K,
    _AUDIO_FREQ_384K,
    _AUDIO_FREQ_512K,
    _AUDIO_FREQ_705_6K,
    _AUDIO_FREQ_768K
}EnumAudioFreqType;




/************************
// data struture type define
//
*************************/



/************************
// Debug Functoin/Macro Definition
//
*************************/
extern StructHDMITxOutputTimingInfo* p_getTxOutputTimingInfo;
#define set_hdmitxtiminginfo_addr(addr) (p_getTxOutputTimingInfo = addr)

/************************
// Implementation Function define
//
*************************/
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
// Query HDMITX support timing in specified output mode
TX_TIMING_INDEX ScalerHdmiRepeaterLibGetTxOutputModeTiming(TX_REPEATER_OUT_MODE_SELECT_TYPE mode);
//dump Edid Feature Support
void ScalerHdmiRepeaterLibGetTxEdidSupportDump(void);
// Get TX Edid Feature Support
UINT8 ScalerHdmiRepeaterLibGetTxEdidSupport(EnumEdidFeatureType enumEdidFeature);
// Set TX Edid Feature Support
INT8 ScalerHdmiRepeaterLibSetTxEdidSupport(EnumEdidFeatureType enumEdidFeature, UINT8 value);
// Set TX User timing mode
UINT8 ScalerHdmiRepeaterLibSetTxUserTimingMode(TX_TIMING_INDEX time_mode);
// Get TX timing table detail info
INT8 ScalerHdmiRepeaterLibGetTxTimingTable(TX_TIMING_INDEX index, hdmi_tx_timing_gen_st *time_mode);

 #ifdef HDMITX_TIMING_TABLE_CONFIG
// Get TX output timing table
INT8 HDMITX_GetTxOutputTimingTable(TX_TIMING_INDEX index, hdmi_tx_output_timing_type *time_mode);
#endif

// write modified Sink EDID to RX [and restart RX HPD] when sink EDID is changed
void ScalerHdmiRepeaterLibSendEdidToRx(BOOLEAN apply);

// Scaler Set HDMITX output timing mode (from AP)
void ScalerHdmiRepeaterLibSetTxTimingMode(TX_TIMING_INDEX index);

// Scaler MAY set HDMITX packet avmute enable to avoid sink device detect unstable signal from HDMITX to avoid RX abnormal display occurred
void ScalerHdmiRepeaterLibSetTxAvMute(BOOLEAN enable);

// Get HDMITX GCP packet avmute apply status (next frame)
UINT8 ScalerHdmiRepeaterLibGetTxAvMuteApplyStatus(void);

void ScalerHdmiTx_Set_ClkGateSel(void);
#endif // #ifdef CONFIG_HDMITX_REPEATER_SUPPORT

/************************
// Extern function define
//
*************************/

#endif // #ifndef __HDMITX_LIB_H__
