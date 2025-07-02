#include "arch.h"
#include "mach.h"
#include "timer_event.h"
#include "msg_queue_def.h"
#include "msg_q_function_api.h"
#include <linux/timer.h>
#include "usb_mac.h"

#include "msg_queue_ap_def.h"
#include "usb_uvc_api.h"
#include "rtk_usb_device_dbg.h"

//PRAGMA_CODESEG(MSG_EVENT_BANK);
//PRAGMA_CONSTSEG(MSG_EVENT_BANK);

void usb_uvc_reinit_disconnect(void) __banked;
void usb_uvc_reinit_connect(void) __banked;

//---------------------------------------------------------------------------
//AP msg event handler
//---------------------------------------------------------------------------
void Timer_USB_UVC_MsgEventHandler(_MSG_Q_PARM* param)
{
}

void fw_UvcMsgEventHandler(_MSG_Q_PARM* param)
{
    // pass param->sub_id (16bits) & param->Param (32bits)
    switch(param->sub_id) {
        case MSG_UVC_AP_INITAIL:
            pm_printk(LOGGER_FATAL, "UVC MSG_UVC_AP_INITAIL\n");
            //usb_uvc_monitor();
            usb_uvc_event_state_initial();
            break;
        case MSG_UVC_AP_REINITAIL_DISCONNECT:
            pm_printk(LOGGER_FATAL, "UVC MSG_UVC_AP_REINITAIL_DISCONNECT\n");
            usb_uvc_reinit_disconnect();
            break;

        case MSG_UVC_AP_REINITAIL_CONNECT:
            pm_printk(LOGGER_FATAL, "UVC MSG_UVC_AP_REINITAIL_CONNECT\n");
            usb_uvc_reinit_connect();
            break;

        default:
            //pm_printk(LOGGER_ERROR, "time:%x\n", (UINT32)param->sub_id);
            //core_timer_event_addTimerEvent(50, MSG_AP_ID, param->sub_id + 1, 0);
            break;
    };
}

