#include "arch.h"
#include "mach.h"
//#include <rbus/combo_u3_dp_aphy_reg.h>
#include "software_feature.h"

//#pragma nogcse
//PRAGMA_CODESEG(BANK41);
//PRAGMA_CONSTSEG(BANK41);

UINT8 usb_sw_cfg_GetUAC_UpStream_UAC2_Enable_BANK41(void)
{
    return GetUAC_UpStream_UAC2_Enable();
}

UINT8 usb_sw_cfg_GetUAC_UpStream_UAC1_Enable_BANK41(void)
{
    return GetUAC_UpStream_UAC1_Enable();
}

UINT8 usb_sw_cfg_GetUAC_DownStream_Enable_BANK41(void)
{
    return GetUAC_DownStream_Enable();
}

UINT8 usb_sw_cfg_Get_PID_BANK41(void)
{
    return Get_PID();
}

UINT8 usb_sw_cfg_Get_CID_BANK41(void)
{
    return Get_CID();
}

UINT8 usb_sw_cfg_GetPD_Mode_BANK41(void)
{
    return GetPD_Mode();
}

UINT8 usb_sw_cfg_GetUAC_Enable_BANK41(void)
{
    return GetUAC_Enable();
}

UINT8 usb_sw_cfg_GetAudio_PCM_Audio_Function_BANK41(void)
{
    return GetAudio_PCM_Audio_Function();
}

UINT8 usb_sw_cfg_GetUVC_Enable_BANK41(void)
{
    return GetUVC_Enable();
}

UINT8 usb_sw_cfg_Get_Chip_version_BANK41(void)
{
    return Get_Chip_version();
}

UINT8 usb_sw_cfg_GetTypeC_Fuction(void)
{
    return GetTypeC_Fuction();
}

