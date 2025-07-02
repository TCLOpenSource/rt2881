#ifndef _HDMI_TX_DSC_PPS_STRUCT_DEF_H_INCLUDED_
#define _HDMI_TX_DSC_PPS_STRUCT_DEF_H_INCLUDED_

#include "../include/ScalerFunctionInclude.h"

#define NO_DSC_PPS_SETTING_INDEX 0xFF
typedef enum _DSC_12G_PPS_SETTING_INDEX {
   DSC_12G_BBP_DEFAULT=0,
   DSC_12G_BBP_8_125,
   DSC_12G_BBP_9_9375,
   DSC_12G_BBP_12_000,
   DSC_12G_BBP_15_000,
   DSC_12G_BBP_NUM,
} DSC_12G_PPS_SETTING_INDEX;

typedef enum _HDMI_TX_DSC_PPS_SETTING_INDEX {
    HDMI21_TX_3840_2160P_444_60HZ = 0,
    HDMI21_TX_3840_2160P_444_120Hz,
    HDMI21_TX_5120_2160P_420_60HZ,
    HDMI21_TX_7680_4320P_420_30HZ,
    HDMI21_TX_7680_4320P_444_30Hz,
    HDMI21_TX_7680_4320P_444_60Hz,
    HDMI21_TX_7680_4320P_444_60Hz_12G4L,
    HDMI21_TX_DV_3840_2160P_444_60HZ,
    HDMI21_TX_DV_5120_2160P_420_60HZ,
    HDMI21_TX_DV_7680_4320P_420_30HZ,
    HDMI21_TX_DV_7680_4320P_444_60HZ,
    HDMI_TX_DSC_PPS_SETTING_NUM
} HDMI_TX_DSC_PPS_SETTING_INDEX;

typedef struct {
    unsigned char dsc_pps[128];
} dsc_pps_st;

void ScalerHdmiTx0_H5xDsc_inputSrcConfig(EnumHDMITXDscSrc dsc_src);
void ScalerHdmiTx0DscPPSConfig(UINT32 format_idx);
void ScalerHdmiTx0_H5xDsc_encClkConfig(UINT32 format_idx);
void ScalerHdmiTx0_H5xDsc_encReset(void);
void ScalerHdmiTx0_H5xDsc_PpsConfig(UINT32 setting_idx, UINT8 deep_color, UINT8 deep_depth);
void ScalerHdmiTX0_H5xDsc_DPFConfig(UINT32 setting_idx, UINT8 deep_color, UINT8 deep_depth);
void ScalerHdmi21MacTx0DscTimingGenSetting(void);

void ScalerDscEncoderSetParameter(EnumOutputPort enumOutputPort, StructDscEncoderParameter *param);
StructDscEncoderParameter *ScalerDscEncoderGetParameter(EnumOutputPort enumOutputPort);

#endif