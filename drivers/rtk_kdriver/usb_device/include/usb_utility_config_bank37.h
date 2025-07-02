#ifndef __USB_UTILITY_CONFIG_BANK37_H
#define __USB_UTILITY_CONFIG_BANK37_H

// PD Funciton
typedef enum {
    _PD_FUNCTION_DISABLE = 0,        // Disable
    _PD_FUNCTION_SINK_ONLY,          // Sink only (Rd+BMC)
    _PD_FUNCTION_SOURCE_ONLY,        // Source only
    _PD_FUNCTION_DRP,                // DRP
    _PD_FUNCTION_SINK_ONLY_Rd_only   // Sink only (Rd only)
} EnumPD_FUNCTION;

// PCM Audio
typedef enum {
    _AUDIO_48K_ONLY = 0x00,    // 48K only
    _AUDIO_WITH_96K,           // 48K/96K
    _AUDIO_WITH_192K,          // 48K/96K/192K
} EnumAudioPCM_MODE;

#define Version_A    0x0000
#define Version_B    0x0010


UINT8 usb_sw_cfg_GetPD_Mode_BANK37(void);
UINT8 usb_sw_cfg_Get_Chip_version_BANK37(void);
UINT8 usb_sw_cfg_Get_UVC_Format_MJPEG_Enable_BANK37(void);
UINT8 usb_sw_cfg_Get_UVC_Format_YUV2_Enable_BANK37(void);
UINT8 usb_sw_cfg_GetUVC_Enable_BANK37(void);
UINT8 usb_sw_cfg_Get_UVC_Format_RGB32_Enable_BANK37(void);
UINT8 usb_sw_cfg_Get_UVC_Format_RGB24_Enable_BANK37(void);
UINT8 usb_sw_cfg_Get_UVC_Format_P010_Enable_BANK37(void);
UINT8 usb_sw_cfg_Get_UVC_Format_NV12_Enable_BANK37(void);
UINT8 usb_sw_cfg_Get_UVC_Format_I420_Enable_BANK37(void);
UINT8 usb_sw_cfg_Get_UVC_Format_M420_Enable_BANK37(void);
UINT8 usb_sw_cfg_GetAudio_PCM_Audio_Function_BANK37(void);
UINT8 usb_sw_cfg_GetUAC_UpStream_UAC2_Enable_BANK37(void);
UINT8 usb_sw_cfg_GetUAC_UpStream_UAC1_Enable_BANK37(void);
UINT8 usb_sw_cfg_GetUAC_DownStream_Enable_BANK37(void);
UINT8 usb_sw_cfg_GetUAC_Enable_BANK37(void);
UINT8 usb_sw_cfg_Get_PID_BANK37(void);
UINT8 usb_sw_cfg_Get_CID_BANK37(void);
UINT8 usb_sw_cfg_Get_UVC_Control_Processing_Unit_Enable_BANK37(void);
UINT8 usb_sw_cfg_Get_USB_HID_Enable_BANK37(void);

#endif
