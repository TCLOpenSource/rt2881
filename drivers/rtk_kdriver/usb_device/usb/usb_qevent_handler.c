#include "arch.h"
#include "mach.h"
#include "timer_event.h"
#include "msg_queue_def.h"
#include "msg_q_function_api.h"
#include <linux/timer.h>
#include "usb_mac.h"
#include "usb_uvc_api.h"
#include <rtk_kdriver/io.h>

//PRAGMA_CODESEG(MSG_EVENT_BANK);
//PRAGMA_CONSTSEG(MSG_EVENT_BANK);
UINT16 uart_start_tx(void) __banked;
void rtk_dwc3_connect_timeout_check(void) __banked;
void dwc3_usb_monitor(void) __banked;
void rtk_dwc3_callback_process_uvc_probe(void) __banked;
void rtk_dwc3_callback_process_uvc_commit(void) __banked;

#define  SYS_REG_SYS_CLKEN2_reg                                                  0xB8000118

extern UINT32 u32_usb_time;
extern UINT32 u32_usb_time_temp;
extern UINT8 usb_uvc_connected;
extern UINT32 u32_uvc_temp;

#ifdef DWC3_RTK_ENABLE_USB_BOOT_TIME_DEBUG_LOG
extern UINT32 u32_usb_time_temp_begin;
extern UINT32 u32_usb_time_temp_end;
#endif

//---------------------------------------------------------------------------
//AP msg event handler
//---------------------------------------------------------------------------
void fw_USB_MsgEventHandler(_MSG_Q_PARM* param)
{
        //pm_printk(LOGGER_ERROR, "fw_USB_MsgEventHandler usb_fw_mq:%x\n", (UINT32)param->sub_id);

        switch(param->sub_id) {
            case MSG_USB_ENUMLATION:
                if((rtd_inl(SYS_REG_SYS_CLKEN2_reg) & _BIT18) == _BIT18) {
                    if (CHECK_EVENT_BUF_COUNT()) {
                        dwc3_gadget_uboot_handle_interrupt();
                    }
                }
                core_fw_msg_queue_addEvent(MSG_USB_ID, MSG_USB_ENUMLATION, 0);
                break;
            case MSG_USB_CDC_START_TX:
                //uart_start_tx();
                break;
            case MSG_USB_INIT:
                rtk_usb_driver_init();
                break;
            case MSG_USB_UVC_PROBE:
                rtk_dwc3_callback_process_uvc_probe();
                break;
            case MSG_USB_UVC_COMMIT:
                rtk_dwc3_callback_process_uvc_commit();
                break;
            default:
                break;
        };


}

void Timer_USB_Handle_uvc_update_transfer(void)
{
    if( rtd_inl(0x18088000) & 0x01000000 ) {
        u32_uvc_temp = rtd_inl(0xb8058800 + 0x10 * UVC_EP_NUMBER + 0xc);
        u32_uvc_temp = u32_uvc_temp & 0xfffffff0;
        u32_uvc_temp = u32_uvc_temp  | 0x00000407;
        rtd_outl(0xb8058800 + 0x10 * UVC_EP_NUMBER + 0xc, u32_uvc_temp);
    }

    if(usb_uvc_connected == 1)
        core_timer_event_addTimerEvent(50, MSG_USB_ID, MSG_USB_UVC_UPDATE_TRANSFER, 0);
}

void Timer_USB_MsgEventHandler(_MSG_Q_PARM* param)
{
        //pm_printk(LOGGER_ERROR, "Timer_USB_MsgEventHandler usb_fw_mq:%x\n", (UINT32)param->sub_id);

        switch(param->sub_id) {
#if 0
            case MSG_USB_ENUMLATION:
                if((rtd_inl(SYS_REG_SYS_CLKEN2_reg) & _BIT18) == _BIT18) {
                    if (CHECK_EVENT_BUF_COUNT()) {
                        dwc3_gadget_uboot_handle_interrupt();
                    }
                }
                core_timer_event_addTimerEvent(100, MSG_USB_ID, MSG_USB_ENUMLATION, 0);
                break;
#endif
            case MSG_USB_INIT:
                rtk_usb_driver_init();
                break;
            case MSG_USB_CONNECT_TIMEOUT:
                rtk_dwc3_connect_timeout_check();
                break;
            case MSG_USB_MONITOR:
                dwc3_usb_monitor();
                core_timer_event_addTimerEvent(1000, MSG_USB_ID, MSG_USB_MONITOR, 0);
                break;
            case MSG_USB_UVC_UPDATE_TRANSFER:
                Timer_USB_Handle_uvc_update_transfer();
                break;
            default:
                break;
        };
}

void Isr_USB_MsgEventHandler(_MSG_Q_PARM* param)
{
        //pm_printk(LOGGER_ERROR, "Isr_USB_MsgEventHandler usb_fw_mq:%x\n", (UINT32)param->sub_id);

        switch(param->sub_id) {
            case MSG_USB_ENUMLATION:
                dwc3_gadget_uboot_handle_interrupt();
                break;
            case MSG_USB_INIT:
                rtk_usb_driver_init();
                break;
            case MSG_USB_UAC_DISCONNECT:
                rtk_uac_disconnect_flow_isr();
                break;
            default:
                break;
        };
}
