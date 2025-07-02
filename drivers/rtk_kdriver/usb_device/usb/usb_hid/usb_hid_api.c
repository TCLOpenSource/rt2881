#include "arch.h"
#include <rbus/timer_reg.h>
#include "mach.h"
#include "rtk_io.h"

#include "msg_q_function_api.h"
#include "rtk_platform.h"
#include "rtk_io.h"
#include "usb_mac.h"
#include "usb_utility.h"
#include "usb_hid.h"
#include "usb_utility_config_bank37.h"
#include "usb_buf_mgr.h"
#include "msg_queue_ap_def.h"
#include "user_data.h"
#include "rtk_usb_device_dbg.h"

//#pragma nogcse
//PRAGMA_CODESEG(BANK37);
//PRAGMA_CONSTSEG(BANK37);
#define code

static UINT8 dwc_buf[256];

extern struct dwc3 g_rtk_dwc3;
extern UINT8 *p_dwc_temp1_u8_1;
extern unsigned long long usb_phy_vir_offset;
extern UINT16 total_length;
extern UINT16 total_interface_num;
extern UINT8 hid_interface;
extern struct DMEM_COPY_TO_SRAM_STRUCT dwc_info;
extern struct SRAM_COPY_TO_DMEM_STRUCT dwc_copy_to_dmem_info;

code static unsigned char g_hid_descriptor[0x1f] =
{
    0x09, 0x04, 0x08, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00,
    0x09, 0x21, 0x11, 0x01, 0x00, 0x01, 0x22, 0x37, 0x00,
    0x07, 0x05, HID_EP_ADDRESS, 0x03, 0x80, 0x00, 0x01,
    0x06, 0x30 ,0x00 ,0x00, 0x80, 0x00,
};

code static unsigned char g_hid_report_descriptor[55] = {
    0x06, 0x00, 0xFF, 0x09, 0x01, 0xA1, 0x01, 0x75, 0x08, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x85, 0x01,
    0x09, 0x01, 0x95, 0x40, 0x81, 0x02, 0x85, 0x02, 0x09, 0x01, 0x95, 0x40, 0x81, 0x02, 0x85, 0x03,
    0x09, 0x01, 0x95, 0x40, 0x81, 0x02, 0x85, 0x04, 0x09, 0x01, 0x95, 0x40, 0x81, 0x02, 0x85, 0x05,
    0x09, 0x01, 0x95, 0x40, 0x91, 0x02, 0xC0
};

void gen_descriptor_table_HID(void) __banked
{
    hid_interface = total_interface_num;
    dwc_info.len = sizeof(g_hid_descriptor);
    dwc_info.src = g_hid_descriptor;
    dwc_info.dst = p_dwc_temp1_u8_1;
    dwc3_copy_to_memory2_utility();

    dwc_info.src = &hid_interface;
    dwc_info.dst = p_dwc_temp1_u8_1 + 0x02;
    dwc_info.len = 1;
    dwc3_copy_to_memory2_utility();

    p_dwc_temp1_u8_1 = p_dwc_temp1_u8_1 + sizeof(g_hid_descriptor);
    total_length = total_length + sizeof(g_hid_descriptor);
    total_interface_num = total_interface_num + 1;
}

void rtk_dwc3_get_hid_report_descriptor(void) __banked
{
    g_rtk_dwc3.ep0_setup_buf_len = g_rtk_dwc3.ep0_ctrl_req->wLength < sizeof(g_hid_report_descriptor) ? g_rtk_dwc3.ep0_ctrl_req->wLength : sizeof(g_hid_report_descriptor);
    dwc_info.dst = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
    dwc_info.src = &g_hid_report_descriptor;
    dwc_info.len = g_rtk_dwc3.ep0_setup_buf_len;
    dwc3_copy_to_memory2_utility();
}

void rtk_process_hid_set_report(void)
{
    dwc_copy_to_dmem_info.src = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
    dwc_copy_to_dmem_info.dst = dwc_buf;
    dwc_copy_to_dmem_info.len = 4;
    dwc3_copy_from_memory2_utility();

    pm_printk(0, "===================HID SET REPORT==============\n");
    pm_printk(0, " %x", (UINT32)g_rtk_dwc3.ep0_ctrl_req->bRequestType);
    pm_printk(0, " %x", (UINT32)g_rtk_dwc3.ep0_ctrl_req->bRequest);
    pm_printk(0, " %x", (UINT32)g_rtk_dwc3.ep0_ctrl_req->wValue);
    pm_printk(0, " %x", (UINT32)g_rtk_dwc3.ep0_ctrl_req->wIndex);
    pm_printk(0, " %x\n", (UINT32)g_rtk_dwc3.ep0_ctrl_req->wLength);
    pm_printk(0, "Data");
    pm_printk(0, " %x", (UINT32)dwc_buf[0]);
    pm_printk(0, " %x", (UINT32)dwc_buf[1]);
    pm_printk(0, " %x", (UINT32)dwc_buf[2]);
    pm_printk(0, " %x\n", (UINT32)dwc_buf[3]);
    pm_printk(0, "===============================================\n");
}

void rtk_process_hid_get_report(void)
{
    pm_printk(0, "===================HID GET REPORT==============\n");
    dwc_buf[0] = (UINT8)(g_rtk_dwc3.ep0_ctrl_req->wValue & 0xff);
    dwc_buf[1] = dwc_buf[0];
    dwc_buf[2] = dwc_buf[0];
    dwc_buf[3] = dwc_buf[0];
    g_rtk_dwc3.ep0_setup_buf_len = 4;
    g_rtk_dwc3.ep0_setup_buf_len = (g_rtk_dwc3.ep0_setup_buf_len > g_rtk_dwc3.ep0_ctrl_req->wLength) ? g_rtk_dwc3.ep0_ctrl_req->wLength : g_rtk_dwc3.ep0_setup_buf_len;
    dwc_info.src = dwc_buf;
    dwc_info.dst = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
    dwc_info.len = g_rtk_dwc3.ep0_setup_buf_len;
    dwc3_copy_to_memory2_utility();
    pm_printk(0, "===============================================\n");
}

void rtk_hid_set_report_callback(void) __banked
{
    rtk_process_hid_set_report();
}

void rtk_hid_control_interface(void) __banked
{
    switch (g_rtk_dwc3.ep0_ctrl_req->bRequestType) {
        case USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
            switch (g_rtk_dwc3.ep0_ctrl_req->bRequest) {
                case 0x09:
                    pm_printk(0, "Process HID SET REPORT ID %x\n", (UINT32)g_rtk_dwc3.ep0_ctrl_req->wValue);
                    g_rtk_dwc3.ep0_last_callback_id = USB_HID_SET_REPORT;
                    break;
                case 0x0a:
                    pm_printk(0, "Process HID SET IDEL\n");
                    break;
                default:
                    break;
            }
            break;

        case USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
            switch (g_rtk_dwc3.ep0_ctrl_req->bRequest) {
                case 0x01:
                    pm_printk(0, "Process HID SET GET REPORT ID %x\n", (UINT32)g_rtk_dwc3.ep0_ctrl_req->wValue);
                    rtk_process_hid_get_report();
                    break;
                default:
                    break;
                }
            break;
        default:
            break;
    }
}

