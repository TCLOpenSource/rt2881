#if 0
#include <stdarg.h>
#include "arch.h"
#include "mach.h"
#include "rtk_kdriver/pm_printk.h"
#include "timer_event.h"
#include "msg_queue_def.h"
#include "msg_q_function_api.h"
#include "rtk_kdriver/timer.h"
#include "../usb_mac.h"
#include "usb_uac_api.h"

PRAGMA_CODESEG(MSG_EVENT_BANK);
PRAGMA_CONSTSEG(MSG_EVENT_BANK);

//---------------------------------------------------------------------------
//AP msg event handler
//---------------------------------------------------------------------------
void Timer_USB_UAC_MsgEventHandler(_MSG_Q_PARM* param)
{
    // pass param->sub_id (16bits) & param->Param (32bits)
    switch(param->sub_id) {
        //case MSG_UAC_MONITOR:
            //pm_printk(LOGGER_ERROR, "UAC time:%x\n", (UINT32)param->sub_id);
            //usb_uac_monitor();
            //break;

        default:
            //pm_printk(LOGGER_ERROR, "time:%x\n", (UINT32)param->sub_id);
            //core_timer_event_addTimerEvent(50, MSG_AP_ID, param->sub_id + 1, 0);
            break;
    };
}
#endif
