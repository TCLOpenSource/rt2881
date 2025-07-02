#include "mach.h"
#include "usb_mac.h"
#include "usb_uvc_api.h"
#include "usb_utility.h"
#include "user_data.h"
#include "usb_uvc_utility.h"
#include "software_feature.h"

#ifdef CONFIG_RTK_KDRV_RTICE_AT_COMMAND
#endif

//#pragma nogcse
//PRAGMA_CODESEG(BANK37);
//PRAGMA_CONSTSEG(BANK37);

extern UINT8 u8_uvc_temp;
extern UINT8 g_recv_buf[150];
extern struct dwc3 g_rtk_dwc3;
extern UINT8 ret;
extern UINT8 g_uvc_set_cur_last_control_selector;
extern UINT8 g_last_uvc_error_val;
extern struct DMEM_COPY_TO_SRAM_STRUCT dwc_info;
extern UINT16 i;

#ifdef CONFIG_SUPPORT_UVC_PROCESSING_UNIT_CONTROL
UINT8 is_uvc_processing_unit_support_control_utility(void)
{
    switch(u8_uvc_temp) {
    case UVC_PU_BRIGHTNESS_CONTROL:
    case UVC_PU_CONTRAST_CONTROL:
    case UVC_PU_HUE_CONTROL:
    case UVC_PU_SATURATION_CONTROL:
        return 1;
    default:
        return 0;
    }
}

void get_uvc_processing_unit_support_control_utility(void)
{
    g_recv_buf[1] = 0x0;
    g_rtk_dwc3.ep0_setup_buf_len = 2;
    switch(u8_uvc_temp){
    case UVC_PU_BRIGHTNESS_CONTROL:
        g_recv_buf[0] = 0;
        break;
    case UVC_PU_CONTRAST_CONTROL:
        g_recv_buf[0] = 0;
        break;
    case UVC_PU_HUE_CONTROL:
        g_recv_buf[0] = 0;
        break;
    case UVC_PU_SATURATION_CONTROL:
        g_recv_buf[0] = 0;
        break;
    default:
        g_last_uvc_error_val = 0x6;
        ret = -EOPNOTSUPP;
        break;
    }
}

void rtk_uvc_process_processing_unit_in(struct usb_ctrlrequest *ctrl_req)  __banked
{
    u8_uvc_temp = (ctrl_req->wValue >> 8) & 0xF;

    if(is_uvc_processing_unit_support_control_utility()) {
        g_recv_buf[1] = 0;
        if(ctrl_req->bRequest == UVC_GET_INFO)
            g_rtk_dwc3.ep0_setup_buf_len = 1;
        else
            g_rtk_dwc3.ep0_setup_buf_len = 2;
        switch (ctrl_req->bRequest) {
        case UVC_GET_LEN:
            g_recv_buf[0] = 0x2;
            break;
        case UVC_GET_CUR:
            get_uvc_processing_unit_support_control_utility();
            break;
        case UVC_GET_INFO:
            g_recv_buf[0] = 0x3;
            break;
        case UVC_GET_MIN:
            g_recv_buf[0] = 0;
            break;
        case UVC_GET_MAX:
            g_recv_buf[0] = 255;
            break;
        case UVC_GET_RES:
            g_recv_buf[0] = 1;
            break;
        case UVC_GET_DEF:
            g_recv_buf[0] = 128;
            break;
        default:
            g_last_uvc_error_val = 0x7;
            ret = -EOPNOTSUPP;
            break;
        }
    } else {
        ret = -EOPNOTSUPP;
        g_last_uvc_error_val = 0x6;
    }
    dwc_info.src = g_recv_buf;
}
#else
void rtk_uvc_process_processing_unit_in(struct usb_ctrlrequest *ctrl_req)  __banked
{
    u8_uvc_temp = (ctrl_req->wValue >> 8) & 0xF;
    switch (ctrl_req->bRequest) {
        case UVC_GET_LEN:
        case UVC_GET_CUR:
        case UVC_GET_INFO:
        case UVC_GET_MIN:
        case UVC_GET_MAX:
        case UVC_GET_RES:
        case UVC_GET_DEF:
            switch(u8_uvc_temp){
            default:
                g_last_uvc_error_val = 0x6;
                ret = -EOPNOTSUPP;
                break;
            }
            break;
        default:
            g_last_uvc_error_val = 0x7;
            ret = -EOPNOTSUPP;
            break;
    }
    dwc_info.src = g_recv_buf;
}

#endif

#ifdef CONFIG_SUPPORT_UVC_PROCESSING_UNIT_CONTROL
UINT8 is_uvc_processing_unit_support_control(void)  __banked
{
    return is_uvc_processing_unit_support_control_utility();
}

void set_uvc_processing_unit_support_control(void)  __banked
{
    switch(g_uvc_set_cur_last_control_selector) {
#ifdef CONFIG_SUPPORT_UVC_PROCESSING_UNIT_CONTROL
    case UVC_PU_BRIGHTNESS_CONTROL:
        drvif_SQE_module_set_uvc_brightness(g_recv_buf[0]);
        break;
    case UVC_PU_CONTRAST_CONTROL:
        drvif_SQE_module_set_uvc_contrast(g_recv_buf[0]);
        break;
    case UVC_PU_HUE_CONTROL:
        drvif_SQE_module_set_uvc_hue(g_recv_buf[0]);
        break;
    case UVC_PU_SATURATION_CONTROL:
        drvif_SQE_module_set_uvc_sat(g_recv_buf[0]);
        break;
#endif
    default:
        ret = -EINVAL;
        break;
    }
}

void get_uvc_processing_unit_support_control(void)  __banked
{
    get_uvc_processing_unit_support_control_utility();
}


#else
UINT8 is_uvc_processing_unit_support_control(void)  __banked
{
    return 0;
}

void set_uvc_processing_unit_support_control(void)  __banked
{

}
void get_uvc_processing_unit_support_control(void)  __banked
{

}

#endif
