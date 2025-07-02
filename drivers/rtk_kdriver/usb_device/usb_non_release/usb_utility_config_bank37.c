#include "arch.h"
#include "mach.h"
//#include <rbus/combo_u3_dp_aphy_reg.h>
#include "software_feature.h"

//#pragma nogcse
//PRAGMA_CODESEG(BANK37);
//PRAGMA_CONSTSEG(BANK37);

UINT8 usb_sw_cfg_GetPD_Mode_BANK37(void)
{
    return GetPD_Mode();
}

UINT8 usb_sw_cfg_Get_Chip_version_BANK37(void)
{
    return Get_Chip_version();
}

UINT8 usb_sw_cfg_Get_UVC_Format_MJPEG_Enable_BANK37(void)
{
    return Get_UVC_Format_MJPEG_Enable();
}

UINT8 usb_sw_cfg_Get_UVC_Format_YUV2_Enable_BANK37(void)
{
    return Get_UVC_Format_YUV2_Enable();
}

UINT8 usb_sw_cfg_GetUVC_Enable_BANK37(void)
{
    return GetUVC_Enable();
}

UINT8 usb_sw_cfg_Get_UVC_Format_RGB32_Enable_BANK37(void)
{
    return Get_UVC_Format_RGB32_Enable();
}

UINT8 usb_sw_cfg_Get_UVC_Format_RGB24_Enable_BANK37(void)
{
    return Get_UVC_Format_RGB24_Enable();
}

UINT8 usb_sw_cfg_Get_UVC_Format_P010_Enable_BANK37(void)
{
    return Get_UVC_Format_P010_Enable();
}

UINT8 usb_sw_cfg_Get_UVC_Format_NV12_Enable_BANK37(void)
{
    return Get_UVC_Format_NV12_Enable();
}

UINT8 usb_sw_cfg_Get_UVC_Format_I420_Enable_BANK37(void)
{
    return Get_UVC_Format_I420_Enable();
}

UINT8 usb_sw_cfg_Get_UVC_Format_M420_Enable_BANK37(void)
{
    return Get_UVC_Format_M420_Enable();
}

UINT8 usb_sw_cfg_GetAudio_PCM_Audio_Function_BANK37(void)
{
    return GetAudio_PCM_Audio_Function();
}

UINT8 usb_sw_cfg_GetUAC_UpStream_UAC2_Enable_BANK37(void)
{
    return GetUAC_UpStream_UAC2_Enable();
}

UINT8 usb_sw_cfg_GetUAC_UpStream_UAC1_Enable_BANK37(void)
{
    return GetUAC_UpStream_UAC1_Enable();
}

UINT8 usb_sw_cfg_GetUAC_DownStream_Enable_BANK37(void)
{
    return GetUAC_DownStream_Enable();
}

UINT8 usb_sw_cfg_GetUAC_Enable_BANK37(void)
{
    return GetUAC_Enable();
}

UINT8 usb_sw_cfg_Get_PID_BANK37(void)
{
    return Get_PID();
}

UINT8 usb_sw_cfg_Get_CID_BANK37(void)
{
    return Get_CID();
}

UINT8 usb_sw_cfg_Get_UVC_Control_Processing_Unit_Enable_BANK37(void)
{
    return Get_UVC_Control_Processing_Unit_Enable();
}

UINT8 usb_sw_cfg_Get_USB_HID_Enable_BANK37(void)
{
    return Get_USB_HID_Enable();
}

UINT8 usb_sw_cfg_Get_Ext_DP_MUX_Select(void)
{
    return Get_Ext_DP_MUX_Select();
}