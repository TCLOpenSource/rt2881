#include "arch.h"
#include "mach.h"
#include <linux/string.h>
//#include "rtk_kdriver/rtd_types.h"
#include "usb_mac.h"
#include "usb_uac_ctrl.h"
#include "usb_uac_api.h"

//PRAGMA_CODESEG(BANK41);
//PRAGMA_CONSTSEG(BANK41);

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
extern struct dwc3 g_rtk_dwc3;

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
#if 0
UINT8 usb_control_uac_request_process(UAC_TYPE uac_type)
{
    //pm_printk(LOGGER_ERROR, "[UAC] usb_control_uac_request_process=%x\n", (UINT32)uac_type);
    return 0;
}

UINT8 usb_uac_set_timing_info(UAC_TYPE uac_type, UINT16 sample_freq, UINT16 bit_resolution)
{
    //pm_printk(LOGGER_ERROR, "[UAC] usb_uac_set_timing_info sample_freq=0x%x bit_resolution=0x%x\n", (UINT32)sample_freq, (UINT32)bit_resolution);
    usb_uac_sample_freq[uac_type]    = sample_freq;
    usb_uac_bit_resolution[uac_type] = bit_resolution;
    return 0;
}

UINT8 usb_uac_get_timing_info(UAC_TYPE uac_type, UINT16 *sample_freq, UINT16 *bit_resolution)
{
    *sample_freq    = usb_uac_sample_freq[uac_type];
    *bit_resolution = usb_uac_bit_resolution[uac_type];
    return 0;
}

void usb_uac_monitor(void) __banked
{
    //pm_printk(LOGGER_ERROR,"usb uac debug\n");
}
#endif

void rtk_uac_control_interface(struct usb_ctrlrequest *ctrl_req) __banked
{
#if 0
        switch (ctrl_req->bRequestType) {
        case USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
            switch (ctrl_req->bRequest) {
#ifdef UAC2
                case UAC2_CS_CUR:
                    c.dCUR = UAC1_DEF_PSRATE;
                    g_rtk_dwc3.ep0_setup_buf_len = (sizeof(c) < ctrl_req->wLength)? sizeof(c): ctrl_req->wLength;
                    dwc3_memcpy((void *)g_rtk_dwc3.ep0_setup_buf, (void *)&c, g_rtk_dwc3.ep0_setup_buf_len);
                    usb_uac_set_timing_info( _UAC_OUT0, UAC1_DEF_PSRATE, 24);
                    break;
                case UAC2_CS_RANGE:
                    r.dMIN = UAC1_DEF_CSRATE;
                    r.dMAX = r.dMIN;
                    r.dRES = 0;
                    r.wNumSubRanges = cpu_to_le16(1);
                    g_rtk_dwc3.ep0_setup_buf_len = ctrl_req->wLength;
                    dwc3_memcpy((void *)g_rtk_dwc3.ep0_setup_buf, (void *)&r, g_rtk_dwc3.ep0_setup_buf_len);
                    break;
#endif
                default:
                    break;
            }
            break;

        case USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
            switch (ctrl_req->bRequest) {
#ifdef UAC2
                case UAC2_CS_CUR:
                    dwc3_memset((void *)&c, 0, sizeof(struct cntrl_cur_lay3));
                    c.dCUR = UAC1_DEF_PSRATE;
                    g_rtk_dwc3.ep0_setup_buf_len = (sizeof(c) < ctrl_req->wLength)? sizeof(c): ctrl_req->wLength;
                    dwc3_memcpy((void *)g_rtk_dwc3.ep0_setup_buf, (void *)&c, g_rtk_dwc3.ep0_setup_buf_len);
                    break;
                case UAC2_CS_RANGE:
                    r.dMIN = UAC1_DEF_PSRATE;
                    r.dMAX = r.dMIN;
                    r.dRES = 0;
                    r.wNumSubRanges = cpu_to_le16(1);
                    g_rtk_dwc3.ep0_setup_buf_len = ctrl_req->wLength;
                    dwc3_memcpy((void *)g_rtk_dwc3.ep0_setup_buf, (void *)&r, g_rtk_dwc3.ep0_setup_buf_len);
                    break;
#endif
                default:
                    break;
            }
            break;

        default:
            //pm_printk(LOGGER_ERROR, "%s: invalid control ctrl_req->bRequestType:0x%02x, ctrl_req->bRequest:0x%02x, ctrl_req->wValue:0x%04x, ctrl_req->wIndex:0x%04x, ctrl_req->wLength:%d\n",
            //    __func__, ctrl_req->bRequestType, ctrl_req->bRequest, ctrl_req->wValue, ctrl_req->wIndex, ctrl_req->wLength);
            break;
        }
#endif
}

void rtk_uac_stream_interface(struct usb_ctrlrequest *ctrl_req) __banked
{
        //pm_printk(LOGGER_ERROR, "rtk_uac_stream_interface (bRequestType=%x) (bRequest=%x)\n", (UINT32)ctrl_req->bRequestType, (UINT32)ctrl_req->bRequest);

        switch (ctrl_req->bRequestType) {
#if 0
        case USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
            switch (ctrl_req->bRequest) {
                case UAC2_CS_CUR:
                case UAC2_CS_RANGE:
                default:
                    break;
            }
            break;
        case USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
            switch (ctrl_req->bRequest) {
                case UAC2_CS_CUR:
                case UAC2_CS_RANGE:
                default:
                    break;
            }
            break;
#endif
        case USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_ENDPOINT:
            switch (ctrl_req->bRequest) {
                case UAC2_CS_CUR:
                    if(ctrl_req->wValue == 0x100) {
                        g_rtk_dwc3.ep_out_in_number = ctrl_req->wIndex;
                        g_rtk_dwc3.ep0_setup_buf_len = ctrl_req->wLength;
                        g_rtk_dwc3.ep0_last_callback_id = USB_UAC_SET_CUR_ID;
                    }
                    break;
                case UAC2_CS_RANGE:
                default:
                    break;
            }
            break;
        case USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_ENDPOINT:
            switch (ctrl_req->bRequest) {
                case UAC2_CS_CUR:
                case UAC2_CS_RANGE:
                default:
                    break;
            }
            break;
        default:
            break;
        }
}
