#include "arch.h"
#include <rbus/timer_reg.h>
#include "mach.h"
#include "usb_mac.h"
#include <linux/string.h>
#include "msg_q_function_api.h"
#include "usb_uvc_api.h"
#include "usb_uvc_ctrl.h"
#include "usb_uvc_utility.h"
#include "rtk_io.h"
#include "usb_buf_mgr.h"
#include <linux/timer.h>
#include "msg_queue_ap_def.h"

#define code
#ifdef CONFIG_RTK_KDRV_RTICE_AT_COMMAND
#endif

#include "rtk_ice-at_common.h"
#include "rtk_ice-at.h"
#include "rtk_ice-at_customer.h"

#include <rtk_kdriver/io.h>

// for memory alloct
#include <linux/pageremap.h>
// for msleep
#include <linux/delay.h>
// for 90K
#include <rbus/timer_reg.h>
// for uvc
#include <rbus/uvc_dma_reg.h>
#include <rbus/uvc_vgip_reg.h>
#include "rtk_usb_device_dbg.h"
#include "user_data.h"
//#include "../../rtice/services/test/at_test.h"
UINT16 u16_uvc_temp;
UINT16 uvc_ex_at_cmd_return_len = 2;
extern UINT8 g_recv_buf[150];

extern UINT8 ret;
extern UINT8 usb_buffer_status;
UINT8 g_uvc_set_cur_last_control_selector = 0;
UINT8 g_last_uvc_error_val = 0;

//#pragma nogcse
//PRAGMA_CODESEG(BANK41);
//PRAGMA_CONSTSEG(BANK41);

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#define DWC3_DEPCMD(n)                   (0x070c + ((n) * 0x10))+USB_DWC3_DEV_GLOBAL_REG_START
#define USB_UVC_ARBUS_CMD_ADDR           DWC3_DEPCMD(2)

#define BYTE_LENGTH_PER_WACK 16
#define POLLING_PERIOD 100
#define TIMEOUT_PERIOD 100

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
extern struct dwc3 g_rtk_dwc3;
extern struct DMEM_COPY_TO_SRAM_STRUCT dwc_info;
extern struct SRAM_COPY_TO_DMEM_STRUCT dwc_copy_to_dmem_info;
struct uvc_control_selector control_selector;
struct uvc_cur_def uvc_def;
AT_UVC_TIMING_INFO_T *p_uvc_timing_info;

UINT8  usb_uvc_inited    = 0;
UINT8  usb_uvc_connected = 0;
UINT8  usb_uvc_format    = 0;
UINT8  usb_uvc_frame     = 0;
UINT16 usb_uvc_width     = 0;
UINT16 usb_uvc_length    = 0;
UINT16 usb_uvc_freq      = 0;

UINT8 usb_uvc_format_real  = 0;
UINT16 usb_uvc_width_real  = 1920;
UINT16 usb_uvc_length_real = 0;
UINT16 usb_uvc_freq_real   = 0;
UINT32 uvc_uid             = 100;

UINT32 uvc_header_data_check = 0xffffffff;

//UINT8 *uvc_addr_phy;
//UINT8 *uvc_addr_vir;
UINT32 uvc_address_header_buffer;                   //PHY
UINT8  *uvc_address_header_buffer_vir_non_cache;     // Vir non cache
UINT32 uvc_address_data_buffer;                     //PHY
UINT8 *uvc_address_data_buffer_vir_non_cache;       // Vir non cache
UINT32 uvc_address_trb_buffer;                      // PHY
UINT8 *uvc_address_trb_buffer_vir_non_cache;        // Vir non cache
UINT8 *uvc_address_data_buffer_vir;

UINT32 uvc_frame_phy[UVC_FRAME_BUFFER_NUMBER];
UINT8 *uvc_frame_vir[UVC_FRAME_BUFFER_NUMBER];

UINT32 uvc_header_phy[UVC_FRAME_BUFFER_NUMBER];
UINT8 *uvc_header_vir[UVC_FRAME_BUFFER_NUMBER];

UINT32 uvc_trb_phy[UVC_FRAME_BUFFER_NUMBER];
UINT8 *uvc_trb_vir[UVC_FRAME_BUFFER_NUMBER];

extern unsigned long long usb_phy_vir_offset;
UINT8 uvc_frame_buffer_status[UVC_FRAME_BUFFER_NUMBER];
UINT32 uvc_frame_buffer_real_size[UVC_FRAME_BUFFER_NUMBER];
UINT8 uvc_frame_buffer_wp = 0;
UINT8 uvc_frame_buffer_wp_old = 0;
UINT8 uvc_frame_buffer_rp = 0;

UINT8 now_usb_send_frame = 0;
int last_uvcid = 0;
UINT32 trb_number = 0;
UINT32 last_frame_size = 0;
UINT32 frame_rate = 0;

UINT32 frame_count = 0;

UINT32 rtd_mem_inl(UINT8 *addr);
UINT32 rtd_mem_outl(UINT8 *addr, UINT32 value);

UINT32 uvc_trb_size = UVC_TRB_SIZE_UNCOMPRESSION;

UINT8 is_need_set_internal_pattern_gen = 0;

UINT8 u8_uvc_temp;

const UINT8 format_mapping_table[] = {_NONE, _YUYV, _MotionJPEG, _NV12, _I420, _RGB24, _RGB32, _P010};

USB_UVC_TIMING_T uvc_timing_info;

UINT32 u32_uvc_temp;
UINT32 u32_uvc_temp_2;
UINT32 u32_uvc_temp_3;
UINT32 u32_uvc_temp_4;

UINT8 trb_cnt = 12;

extern UINT8 usb_link_state_count;

//#define __ceiling(x, align) (x - x%align)
//#define __floor(x, align) (((x+align -1)/(align))*(align))

struct uvc_extension g_uvc_extension =
{
    .log_buf = {
        .write_pos = 0,
        .read_pos = 0,
    },
};


//   0xb8084808 uvc_ds_ctrl_1
code UINT32 uvc_ds_ctrl_1[4][1] = {
    {0x02800500},   // 1280*720
    {0x02800500},   // 1920*1080
    {0x02800500},   // 2560*1440
    {0x02800500},   // 3840*2160
};

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
UINT8 usb_uvc_init(void)
{
    UINT8 i;
    UINT32 size;

    if(usb_uvc_inited == 1) {
        //pm_printk(LOGGER_ERROR, "[UVC] already init, just return\n");
        return 0;
    }

    if( usb_buffer_status == 0 )
        usbbuf_init();

#if 1
    size = (UVC_FRAME_BUFFER_SIZE + UVC_HEADER_BUFFER_SIZE + UVC_TRB_BUFFER_SIZE) * UVC_FRAME_BUFFER_NUMBER;
    uvc_address_data_buffer_vir = (UINT8*)dvr_malloc_uncached_specific( size, GFP_DCU2, (void*)&uvc_address_data_buffer_vir_non_cache);
    uvc_address_data_buffer = dvr_to_phys( (void*)uvc_address_data_buffer_vir );

    uvc_address_header_buffer = uvc_address_data_buffer + UVC_FRAME_BUFFER_SIZE;
    uvc_address_trb_buffer  = (UINT32)uvc_address_header_buffer + UVC_HEADER_BUFFER_SIZE;

    uvc_address_header_buffer_vir_non_cache = uvc_address_data_buffer_vir_non_cache + 0xFD2000;
    uvc_address_trb_buffer_vir_non_cache  = uvc_address_header_buffer_vir_non_cache   + 0x10000;

    uvc_frame_phy[0]  = uvc_address_data_buffer;
    uvc_frame_vir[0]  = uvc_address_data_buffer_vir_non_cache;
    uvc_header_phy[0] = uvc_frame_phy[0]  + UVC_FRAME_BUFFER_SIZE;
    uvc_header_vir[0] = uvc_frame_vir[0]  + UVC_FRAME_BUFFER_SIZE;
    uvc_trb_phy[0]    = uvc_header_phy[0] + UVC_HEADER_BUFFER_SIZE;
    uvc_trb_vir[0]    = uvc_header_vir[0] + UVC_HEADER_BUFFER_SIZE;

    uvc_frame_phy[1]  = uvc_trb_phy[0]    + UVC_TRB_BUFFER_SIZE;
    uvc_frame_vir[1]  = uvc_trb_vir[0]    + UVC_TRB_BUFFER_SIZE;
    uvc_header_phy[1] = uvc_frame_phy[1]  + UVC_FRAME_BUFFER_SIZE;
    uvc_header_vir[1] = uvc_frame_vir[1]  + UVC_FRAME_BUFFER_SIZE;
    uvc_trb_phy[1]    = uvc_header_phy[1] + UVC_HEADER_BUFFER_SIZE;
    uvc_trb_vir[1]    = uvc_header_vir[1] + UVC_HEADER_BUFFER_SIZE;

    uvc_frame_phy[2]  = uvc_trb_phy[1]    + UVC_TRB_BUFFER_SIZE;
    uvc_frame_vir[2]  = uvc_trb_vir[1]    + UVC_TRB_BUFFER_SIZE;
    uvc_header_phy[2] = uvc_frame_phy[2]  + UVC_FRAME_BUFFER_SIZE;
    uvc_header_vir[2] = uvc_frame_vir[2]  + UVC_FRAME_BUFFER_SIZE;
    uvc_trb_phy[2]    = uvc_header_phy[2] + UVC_HEADER_BUFFER_SIZE;
    uvc_trb_vir[2]    = uvc_header_vir[2] + UVC_HEADER_BUFFER_SIZE;

    for(i=0; i<UVC_FRAME_BUFFER_NUMBER; i++) {
        uvc_frame_buffer_status[i] = 0;
        RTK_USB_DEVICE_DBG_SIMPLE(  "uvc frame[%d]  phy=0x%x vir=0x%px\n", i, uvc_frame_phy[i], uvc_frame_vir[i]);
        RTK_USB_DEVICE_DBG_SIMPLE(  "uvc header[%d] phy=0x%x vir=0x%px\n", i, uvc_header_phy[i], uvc_header_vir[i]);
        RTK_USB_DEVICE_DBG_SIMPLE(  "uvc trb[%d]    phy=0x%x vir=0x%px\n", i, uvc_trb_phy[i], uvc_trb_vir[i]);
    }
#else
    usb_request_usbbuf(UVC_SRAM_BUF, &uvc_address_data_buffer, &uvc_address_data_buffer, &size);
    uvc_address_header_buffer = uvc_address_data_buffer + (UINT32)USB_UVC_D_SIZE * (UINT32)USB_UVC_DATA_BUFFER_GROUP_CNT;
    uvc_address_trb_buffer  = (UINT32)uvc_address_header_buffer   + (UINT32)USB_UVC_HEADER_SIZE      * (UINT32)USB_UVC_DATA_BUFFER_GROUP_CNT;
#endif
    pm_printk(LOGGER_INFO, "[UVC] usb_uvc_init");
    pm_printk(LOGGER_INFO, "PHY\n");
    pm_printk(LOGGER_INFO, " header = %x", uvc_address_header_buffer);
    pm_printk(LOGGER_INFO, " data   = %x", uvc_address_data_buffer);
    pm_printk(LOGGER_INFO, " trb    = %x", uvc_address_trb_buffer);
    pm_printk(LOGGER_INFO, "Virtual\n");
    pm_printk(LOGGER_INFO, " header = %px", uvc_address_header_buffer_vir_non_cache);
    pm_printk(LOGGER_INFO, " data   = %px", uvc_address_data_buffer_vir_non_cache);
    pm_printk(LOGGER_INFO, " trb    = %px", uvc_address_trb_buffer_vir_non_cache);
    pm_printk(LOGGER_INFO, "\n");

    g_uvc_extension.recv_buf = g_recv_buf;


    usb_uvc_inited = 1;
    g_rtk_dwc3.ep0_descriptor_table_buf =  DWC3_USB_DESCRIPTOR_BUFFER_ADDRESS;
    return 0;
}

UINT8 usb_uvc_uninit(void)
{
    //pm_printk(LOGGER_ERROR, "[UVC] usb_uvc_uninit\n");

    if(usb_uvc_inited == 0) {
        //pm_printk(LOGGER_ERROR, "[UVC] already uninint, just return\n");
        return 0;
    }

    dvr_free(uvc_address_data_buffer_vir);
    usb_uvc_inited = 0;
    return 0;
}

UINT8 usb_uvc_connect(UINT8 ep)
{
#if 0
    UINT32 temp;
    UINT32 frame_size = 0;
#endif
    pm_printk(LOGGER_ERROR, "[UVC] usb_uvc_connect=%x\n", (UINT32)ep);

    if(usb_uvc_inited == 0) {
        //pm_printk(LOGGER_ERROR, "[UVC] already init, just return\n");
        return 0;
    }

    if(usb_uvc_connected == 1) {
        //pm_printk(LOGGER_ERROR, "[UVC] already connect, just return\n");
        return 0;
    }
    //usb_uvc_in_enable();
    usb_uvc_connected = 1;
    usb_link_state_count = 0;
    core_timer_event_cancelTimerEvent(MSG_USB_ID, MSG_USB_UVC_UPDATE_TRANSFER);
    core_timer_event_addTimerEvent(50, MSG_USB_ID, MSG_USB_UVC_UPDATE_TRANSFER, 0);
    return 0;
}

#if 0
UINT8 usb_control_uvc_request_process(UINT8 ep)
{
    //pm_printk(LOGGER_ERROR, "[UVC] usb_control_uvc_request_process=%x\n", (UINT32)ep);
    return 0;
}
#endif


#if 0
UINT8 usb_uvc_get_timing_info(StructDisplayInfo *info)
{
    info->DC2H_dataFormat = format_mapping_table[usb_uvc_format];
    info->DC2H_CapWid     = usb_uvc_width;
    info->DC2H_CapLen     = usb_uvc_length;
    info->DC2H_vFreq      = usb_uvc_freq;
    //pm_printk(LOGGER_ERROR, "[UVC] usb_uvc_get_timing_info format=0x%x w=0x%x l=0x%x f=0x%x\n", (UINT32)usb_uvc_format, (UINT32)usb_uvc_width, (UINT32)usb_uvc_length, (UINT32)usb_uvc_freq);
    return 0;
}
#endif

USB_UVC_TIMING_T* RHAL_USB_UVC_GetTimingInfo(void)__banked
{
    uvc_timing_info.format = format_mapping_table[usb_uvc_format_real];
    uvc_timing_info.width  = usb_uvc_width_real;
    uvc_timing_info.height = usb_uvc_length_real;
    uvc_timing_info.freq   = usb_uvc_freq_real;
    return &uvc_timing_info;
}

UINT8 usb_uvc_set_timing_info(UINT8 format_index, UINT8 frame_index, UINT16 frame)
{
    //pm_printk(LOGGER_ERROR, "[UVC] usb_uvc_set_timing_info format_index=0x%x frame_index=0x%x\n", (UINT32)format_index, (UINT32)frame_index);

    usb_uvc_format = format_index;
    usb_uvc_frame  = frame_index;

    usb_uvc_check_format_frame_info();
    usb_uvc_freq   = frame;

    return 0;
}

void usb_uvc_monitor(void) __banked
{
    //pm_printk(LOGGER_ERROR,"usb uvc debug\n");
}

UINT8 usb_uvc_event_state_initial(void) __banked
{
    usb_uvc_init();
    return 0;
}

void rtk_uvc_process_extension_UVC_GET_LEN(struct usb_ctrlrequest *ctrl_req)
{
    if(ctrl_req->wValue == UVC_EXTENSION_CONTROL_SELECTOR_LENGTH) {
        u16_uvc_temp = UVC_EXTENSION_CONTROL_SELECTOR_LENGTH_SIZE;
        dwc_info.src = (void *)&u16_uvc_temp;
        g_rtk_dwc3.ep0_setup_buf_len = UVC_EXTENSION_GET_LENGTH_SIZE;
    } else if(ctrl_req->wValue == UVC_EXTENSION_CONTROL_SELECTOR_DATA) {
        dwc_info.src = (void *)&uvc_ex_at_cmd_return_len;
        g_rtk_dwc3.ep0_setup_buf_len = UVC_EXTENSION_GET_LENGTH_SIZE;
    } else if(ctrl_req->wValue == UVC_EXTENSION_CONTROL_SELECTOR_DATA_LENGTH_FIXED){
        u16_uvc_temp = 150;
        dwc_info.src = (void *)&u16_uvc_temp;
        g_rtk_dwc3.ep0_setup_buf_len = UVC_EXTENSION_GET_LENGTH_SIZE;
    } else if(ctrl_req->wValue == UVC_EXTENSION_CONTROL_SELECTOR_DATA_LENGTH_FIXED_LONG){
        u16_uvc_temp = 525;
        dwc_info.src = (void *)&u16_uvc_temp;
        g_rtk_dwc3.ep0_setup_buf_len = UVC_EXTENSION_GET_LENGTH_SIZE;
    } else {
        u16_uvc_temp = UVC_EXTENSION_CONTROL_SELECTOR_LENGTH_SIZE;
        dwc_info.src = (void *)&u16_uvc_temp;
        g_rtk_dwc3.ep0_setup_buf_len = UVC_EXTENSION_GET_LENGTH_SIZE;
    }
}

void rtk_uvc_process_extension_UVC_GET_CUR(struct usb_ctrlrequest *ctrl_req)
{
    if(ctrl_req->wValue == UVC_EXTENSION_CONTROL_SELECTOR_LENGTH) {
        uvc_ex_at_cmd_return_len = 0;
        if( g_uvc_extension.log_buf.write_pos >  g_uvc_extension.log_buf.read_pos)
            uvc_ex_at_cmd_return_len = g_uvc_extension.log_buf.write_pos - g_uvc_extension.log_buf.read_pos;
        dwc_info.src = (void *)&uvc_ex_at_cmd_return_len;
        g_rtk_dwc3.ep0_setup_buf_len = UVC_EXTENSION_CONTROL_SELECTOR_LENGTH_SIZE;
    }
    
    if(ctrl_req->wValue == UVC_EXTENSION_CONTROL_SELECTOR_DATA) {
        dwc_info.src = (void *)&g_recv_buf[g_uvc_extension.log_buf.read_pos];
        uvc_ex_at_cmd_return_len = 0;
        if( g_uvc_extension.log_buf.write_pos >  g_uvc_extension.log_buf.read_pos)
            uvc_ex_at_cmd_return_len = g_uvc_extension.log_buf.write_pos - g_uvc_extension.log_buf.read_pos;
        g_rtk_dwc3.ep0_setup_buf_len = uvc_ex_at_cmd_return_len;
        g_uvc_extension.log_buf.read_pos += uvc_ex_at_cmd_return_len;
        if(g_rtk_dwc3.dwc_need_reconnect == 1)
            g_rtk_dwc3.dwc_need_reconnect = 2;
    }
    
    if(ctrl_req->wValue == UVC_EXTENSION_CONTROL_SELECTOR_DATA_LENGTH_FIXED) {
        dwc_info.src = (void *)&g_recv_buf[g_uvc_extension.log_buf.read_pos];
        uvc_ex_at_cmd_return_len = 0;
        if( g_uvc_extension.log_buf.write_pos >  g_uvc_extension.log_buf.read_pos)
            uvc_ex_at_cmd_return_len = g_uvc_extension.log_buf.write_pos - g_uvc_extension.log_buf.read_pos;
        g_rtk_dwc3.ep0_setup_buf_len = uvc_ex_at_cmd_return_len;
        g_uvc_extension.log_buf.read_pos += uvc_ex_at_cmd_return_len;
    }

    if(ctrl_req->wValue == UVC_EXTENSION_CONTROL_SELECTOR_DATA_LENGTH_FIXED_LONG) {
        dwc_info.src = (void *)&g_recv_buf[g_uvc_extension.log_buf.read_pos];
        uvc_ex_at_cmd_return_len = 0;
        if( g_uvc_extension.log_buf.write_pos >  g_uvc_extension.log_buf.read_pos)
            uvc_ex_at_cmd_return_len = g_uvc_extension.log_buf.write_pos - g_uvc_extension.log_buf.read_pos;
        g_rtk_dwc3.ep0_setup_buf_len = uvc_ex_at_cmd_return_len;
        g_uvc_extension.log_buf.read_pos += uvc_ex_at_cmd_return_len;
    }
}

void rtk_uvc_process_extension_in(struct usb_ctrlrequest *ctrl_req)
{
    switch (ctrl_req->bRequest) {
        case UVC_GET_LEN:
            rtk_uvc_process_extension_UVC_GET_LEN(ctrl_req);
            break;
        case UVC_GET_CUR:
            rtk_uvc_process_extension_UVC_GET_CUR(ctrl_req);
             break;
        case UVC_GET_INFO:
            u16_uvc_temp = UVC_EXTENSION_DEVICE_NODE;
            dwc_info.src = (void *)&u16_uvc_temp;
            g_rtk_dwc3.ep0_setup_buf_len = UVC_EXTENSION_GET_INFO_SIZE;
            break;
        case UVC_GET_MIN:
        case UVC_GET_MAX:
        case UVC_GET_RES:
        case UVC_GET_DEF:
        default:
            u16_uvc_temp = UVC_EXTENSION_DEVICE_NODE;
            dwc_info.src = (void *)&u16_uvc_temp;
            g_rtk_dwc3.ep0_setup_buf_len = UVC_EXTENSION_GET_LENGTH_SIZE;
            break;
    }

}
void rtk_uvc_process_input_terminal_in(struct usb_ctrlrequest *ctrl_req)
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



static void usb_uvc_get_request(struct usb_ctrlrequest *ctrl_req)
{
	switch(ctrl_req->wIndex) {
	case 0x000:
		if(ctrl_req->wValue == 0x200) {//Request_Error_Code_Control
			dwc_info.src = g_recv_buf;
			g_recv_buf[0] = g_last_uvc_error_val;
			g_rtk_dwc3.ep0_setup_buf_len = 1;
			g_last_uvc_error_val = 0;
		} else {
			ret = -EOPNOTSUPP;
		}
		break;
	case 0x100:
		rtk_uvc_process_input_terminal_in(ctrl_req);
		break;
	case 0x200: //procssing unit
		rtk_uvc_process_processing_unit_in(ctrl_req);
		break;
	case 0x400: //process_extension
		rtk_uvc_process_extension_in(ctrl_req);
		break;
	default:
		g_last_uvc_error_val = 0x5;
		ret = -EOPNOTSUPP;
		break;
	}
}

static void usb_uvc_processing_unit_set_cur(UINT8 actual_len)
{
	if(actual_len > 2) {
		ret = -EINVAL;
		return;
	}
	if(g_recv_buf[1] != 0) {
		ret = -EINVAL;
		return;
	}
	set_uvc_processing_unit_support_control();
}

void usb_uvc_set_cur(UINT8 actual_len)
{
	switch(g_rtk_dwc3.ep0_last_callback_id) {
	case USB_UVC_PROCESSING_UINT_SET_CUR:
		usb_uvc_processing_unit_set_cur(actual_len);
		break;
	default:
		break;
	}
}

void usb_uvc_set_request_id(struct usb_ctrlrequest *ctrl_req)
{
    switch(ctrl_req->wIndex) {
    case 0x200: //procssing unit
        u8_uvc_temp = (ctrl_req->wValue >> 8) & 0xF;
        if(is_uvc_processing_unit_support_control()) {
            g_rtk_dwc3.ep0_last_callback_id = USB_UVC_PROCESSING_UINT_SET_CUR;
            g_uvc_set_cur_last_control_selector = (ctrl_req->wValue >> 8) & 0xF;
        } else {
            ret = -EOPNOTSUPP;
        }
        break;
    case 0x400: //process_extension
        if(ctrl_req->wValue == UVC_EXTENSION_CONTROL_SELECTOR_DATA) {
            g_rtk_dwc3.ep0_last_callback_id = USB_UVC_EX_SET_CUR_ID_CS1;
        }
        if(ctrl_req->wValue == UVC_EXTENSION_CONTROL_SELECTOR_LENGTH) {
            g_rtk_dwc3.ep0_last_callback_id = USB_UVC_EX_SET_CUR_ID_CS2;
        }
        if(ctrl_req->wValue == UVC_EXTENSION_CONTROL_SELECTOR_DATA_LENGTH_FIXED) {
            g_rtk_dwc3.ep0_last_callback_id = USB_UVC_EX_SET_CUR_ID_CS3;
        }
        if(ctrl_req->wValue == UVC_EXTENSION_CONTROL_SELECTOR_DATA_LENGTH_FIXED_LONG) {
            g_rtk_dwc3.ep0_last_callback_id = USB_UVC_EX_SET_CUR_ID_CS4;
        }
        break;
    default:
        ret = -EOPNOTSUPP;
        break;
    }
}


void rtk_uvc_control_interface(struct usb_ctrlrequest *ctrl_req)
{
        switch (ctrl_req->bRequestType) {
            case USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
                switch (ctrl_req->bRequest) {
                case UVC_SET_CUR:
                    usb_uvc_set_request_id(ctrl_req);
                    break;
                default:
                    break;
                }
                break;

            case USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
				switch(ctrl_req->bRequest) {
				case UVC_GET_CUR:
				case UVC_GET_RES:
				case UVC_GET_MAX:
				case UVC_GET_MIN:
				case UVC_GET_DEF:
				case UVC_GET_INFO:
				case UVC_GET_LEN:
					usb_uvc_get_request(ctrl_req);
					break;
				default:
					break;
				}
				if(ret == 0) {
					g_rtk_dwc3.ep0_setup_buf_len = (g_rtk_dwc3.ep0_setup_buf_len > ctrl_req->wLength) ? ctrl_req->wLength : g_rtk_dwc3.ep0_setup_buf_len;
					dwc_info.dst = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
					dwc_info.len = g_rtk_dwc3.ep0_setup_buf_len;
					dwc3_copy_to_memory2();
				}

                break;
            default:
                //pm_printk(LOGGER_ERROR, "%s: invalid control ctrl_req->bRequestType:0x%02x, ctrl_req->bRequest:0x%02x, ctrl_req->wValue:0x%04x, ctrl_req->wIndex:0x%04x, ctrl_req->wLength:%d\n",
                //    __func__, ctrl_req->bRequestType, ctrl_req->bRequest, ctrl_req->wValue, ctrl_req->wIndex, ctrl_req->wLength);
                break;
        }
}

void common_setting(void)  __banked;

void yuy2_setting(void)
{
    // 1 plane 422

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(1)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_YUY2));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*usb_uvc_length_real*2/16));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*2)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));
}

void nv12_setting(void)
{
    // 2 plane 420

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(1)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(1));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_NV12));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(1)| 
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(1));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*(usb_uvc_length_real/2)*2/16));

    //Video DMA1
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_cap1_dma_sta_addr(uvc_frame_phy[0]) + usb_uvc_width_real*(usb_uvc_length_real/2)*2);
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_cap1_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_cap1_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_cap1_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_line_num_reg, UVC_DMA_UVC_CAP1_DMA_line_num_cap1_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP1_DMA_WR_num_bl_wrap_word_cap1_line_burst_num(usb_uvc_width_real*(usb_uvc_length_real/2/2)*2/16));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*3/2)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));
}

void i420_setting(void)
{
    // 3 plane 420

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(1)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(1));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_I420));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(1)| 
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(2));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*(usb_uvc_length_real/2)*2/16));

    //Video DMA1
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_cap1_dma_sta_addr(uvc_frame_phy[0]) + usb_uvc_width_real*(usb_uvc_length_real/2)*2);
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_cap1_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_cap1_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_cap1_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_line_num_reg, UVC_DMA_UVC_CAP1_DMA_line_num_cap1_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP1_DMA_WR_num_bl_wrap_word_cap1_line_burst_num(usb_uvc_width_real*(usb_uvc_length_real/2/2/2)*2/16));

    //Video DMA2
    rtd_outl(UVC_DMA_UVC_CAP2_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP2_DMA_WR_Start_addr_cap2_dma_sta_addr(uvc_frame_phy[0]) + usb_uvc_width_real*(usb_uvc_length_real/2)*2 + usb_uvc_width_real*(usb_uvc_length_real/2/2/2)*2);
    rtd_outl(UVC_DMA_UVC_CAP2_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP2_DMA_WR_Ctrl_cap2_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP2_DMA_WR_Ctrl_cap2_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP2_DMA_WR_Ctrl_cap2_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP2_DMA_line_num_reg, UVC_DMA_UVC_CAP2_DMA_line_num_cap2_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP2_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP2_DMA_WR_num_bl_wrap_word_cap2_line_burst_num(usb_uvc_width_real*(usb_uvc_length_real/2/2/2)*2/16));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*3/2)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));
}

void m420_setting(void)
{
}

void rgb24_setting(void)
{
    // 1 plane 444

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_RGB24));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]) + ( usb_uvc_width_real * (usb_uvc_length_real-1) * 3 ));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(usb_uvc_length_real));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*3/16));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_step_reg, UVC_DMA_UVC_CAP0_DMA_line_step_cap0_line_step(-(usb_uvc_width_real*3/16)));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*3)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x00)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x00));
}

void rgb32_setting(void)
{
    // 1 plane 444

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_RGB32));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]) + ( usb_uvc_width_real * (usb_uvc_length_real-1) * 4 ));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(usb_uvc_length_real));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*4/16));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_step_reg, UVC_DMA_UVC_CAP0_DMA_line_step_cap0_line_step(-(usb_uvc_width_real*4/16)));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*4)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x00)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x00));
}


void p010_setting(void)
{
    // 2 plane 420

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(1)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(1));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_P010));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(1)| 
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(1));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*(usb_uvc_length_real)*2/16));

    //Video DMA1
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_cap1_dma_sta_addr(uvc_frame_phy[0]) + usb_uvc_width_real*(usb_uvc_length_real)*2);
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_cap1_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_cap1_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_cap1_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_line_num_reg, UVC_DMA_UVC_CAP1_DMA_line_num_cap1_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP1_DMA_WR_num_bl_wrap_word_cap1_line_burst_num(usb_uvc_width_real*(usb_uvc_length_real/2)*2/16));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*3)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));
}

void mjpeg_setting(void)
{
    // 1 plane

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(1)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(1));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(0));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(3));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(1)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(0x0FFFFFFF));

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));

}

void iyu2_setting(void)
{
    // 1 plane 444

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_IYU2));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*usb_uvc_length_real*3/16));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*3)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));
}

void ayuv_setting(void)
{
    // 1 plane 444

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_AYUV));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*usb_uvc_length_real*4/16));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*4)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));
}

void v410_setting(void)
{
    // 1 plane 444

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_V410));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*usb_uvc_length_real*4/16));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*4)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));
}

void y410_setting(void)
{
    // 1 plane 444

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_Y410));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*usb_uvc_length_real*4/16));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*4)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));
}

void uyvy_setting(void)
{
    // 1 plane 422

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(1)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_UYVY));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*usb_uvc_length_real*2/16));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*2)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));
}

void v210_setting(void)
{
    // 1 plane 422

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(1)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_V210));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*usb_uvc_length_real*16/6/16));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*16/6)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));
}

void y210_setting(void)
{
    // 1 plane 422

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(1)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_Y210));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*usb_uvc_length_real*4/16));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*4)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));
}

void yv12_setting(void)
{
    // 3 plane 420

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(1)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(1));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_YV12));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(1)| 
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(2));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*(usb_uvc_length_real/2)*2/16));

    //Video DMA1
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_cap1_dma_sta_addr(uvc_frame_phy[0]) + usb_uvc_width_real*(usb_uvc_length_real/2)*2);
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_cap1_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_cap1_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP1_DMA_WR_Ctrl_cap1_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_line_num_reg, UVC_DMA_UVC_CAP1_DMA_line_num_cap1_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP1_DMA_WR_num_bl_wrap_word_cap1_line_burst_num(usb_uvc_width_real*(usb_uvc_length_real/2/2/2)*2/16));

    //Video DMA2
    rtd_outl(UVC_DMA_UVC_CAP2_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP2_DMA_WR_Start_addr_cap2_dma_sta_addr(uvc_frame_phy[0]) + usb_uvc_width_real*(usb_uvc_length_real/2)*2 + usb_uvc_width_real*(usb_uvc_length_real/2/2/2)*2);
    rtd_outl(UVC_DMA_UVC_CAP2_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP2_DMA_WR_Ctrl_cap2_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP2_DMA_WR_Ctrl_cap2_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP2_DMA_WR_Ctrl_cap2_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP2_DMA_line_num_reg, UVC_DMA_UVC_CAP2_DMA_line_num_cap2_line_num(1));
    rtd_outl(UVC_DMA_UVC_CAP2_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP2_DMA_WR_num_bl_wrap_word_cap2_line_burst_num(usb_uvc_width_real*(usb_uvc_length_real/2/2/2)*2/16));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*3/2)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x80)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x80));
}

void rgb565_setting(void)
{
    // 1 plane 444

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_RGB565));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]) + ( usb_uvc_width_real * (usb_uvc_length_real-1) * 2 ));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(usb_uvc_length_real));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*2/16));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_step_reg, UVC_DMA_UVC_CAP0_DMA_line_step_cap0_line_step(-(usb_uvc_width_real*2/16)));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*2)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x00)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x00));
}

void rgb555_setting(void)
{
    // 1 plane 444

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_RGB555));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    // 1 plane 444

    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_0_reg      , UVC_DMA_UVC_DS_Ctrl_0_ds_hor_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_mode(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_hor_en(0)|
                                              UVC_DMA_UVC_DS_Ctrl_0_ds_ver_en(0));
    rtd_outl(UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_reg, UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_data_order(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_rgb32_alpha(0)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_en(1)|
                                              UVC_DMA_UVC_Pxl_Wrap_Ctrl_0_pack_wrap_mode(_UVC_PACK_WRAP_MODE_RGB565));

    //Video DMA common
    rtd_outl(UVC_DMA_UVC_DMA_CTRL_reg, UVC_DMA_UVC_DMA_CTRL_uvc_cap1_arbitor_en(0)|
                                       UVC_DMA_UVC_DMA_CTRL_uvc_dma_wdone_sel(0));

    //Video DMA0
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]) + ( usb_uvc_width_real * (usb_uvc_length_real-1) * 2 ));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_reg, UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_data_sel(0)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_max_outstanding(0x24)|
                                               UVC_DMA_UVC_CAP0_DMA_WR_Ctrl_cap0_dma_enable(1));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_num_reg, UVC_DMA_UVC_CAP0_DMA_line_num_cap0_line_num(usb_uvc_length_real));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_reg, UVC_DMA_UVC_CAP0_DMA_WR_num_bl_wrap_word_cap0_line_burst_num(usb_uvc_width_real*2/16));
    rtd_outl(UVC_DMA_UVC_CAP0_DMA_line_step_reg, UVC_DMA_UVC_CAP0_DMA_line_step_cap0_line_step(-(usb_uvc_width_real*2/16)));

    // Header Gen
    rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, ((usb_uvc_width_real*usb_uvc_length_real*2)+16371)/16372);

    //Aspect Ratio Control
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG0_reg, UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_ub(0x00)|
                                           UVC_DMA_UVC_UGLY_DEN_BG0_ugly_bg_yg(0x00));
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_BG1_reg, UVC_DMA_UVC_UGLY_DEN_BG1_ugly_bg_vr(0x00));
}

void setting_uvc(void)
{
        //rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x10 * 15 + 0x0, UVC_TRB_BUFFER_ADDR);
        //rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x10 * 15 + 0x4, 0x00000000);
        //rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x10 * 15 + 0x8, 0x00000000);
        //rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x10 * 15 + 0xc, DWC3_TRBCTL_LINK_TRB | DWC3_TRB_CTRL_HWO);

        pm_printk(0, "setting_uvc %d\n", (UINT32)usb_uvc_format_real);

        //dwc_printk( 0, "rtk_dwc3_ep_start_trans_uvc %d\n", (UINT32)__LINE__);
#if 0
        if(rtd_inl(0xb8084AEC) != 0)
            uvc_trb_size = rtd_inl(0xb8084AEC);
        else
            uvc_trb_size = UVC_TRB_SIZE_UNCOMPRESSION;
#endif
    switch(usb_uvc_format_real) {
        case _USB_UVC_YUY2:
            u32_uvc_temp_3 = 2;
            u32_uvc_temp_4 = 1;
            common_setting();
            yuy2_setting();
            break;

        case _USB_UVC_MotionJPEG:
#if 0
            if(rtd_inl(0xb8084AEC) != 0)
                uvc_trb_size = rtd_inl(0xb8084AEC);
            else
                uvc_trb_size = UVC_TRB_SIZE_MJPEG;
#endif
            common_setting();
            mjpeg_setting();
            break;
        case _USB_UVC_NV12:
            u32_uvc_temp_3 = 3;
            u32_uvc_temp_4 = 2;
            common_setting();
            nv12_setting();
            break;
        case _USB_UVC_I420:
            u32_uvc_temp_3 = 3;
            u32_uvc_temp_4 = 2;
            common_setting();
            i420_setting();
            break;
        case _USB_UVC_RGB24:
            u32_uvc_temp_3 = 3;
            u32_uvc_temp_4 = 1;
            common_setting();
            rgb24_setting();
            break;
        case _USB_UVC_RGB32:
            u32_uvc_temp_3 = 4;
            u32_uvc_temp_4 = 1;
            common_setting();
            rgb32_setting();
            break;
        case _USB_UVC_P010:
            u32_uvc_temp_3 = 3;
            u32_uvc_temp_4 = 1;
            common_setting();
            p010_setting();
            break;
        case _USB_UVC_IYU2:
            common_setting();
            iyu2_setting();
            break;
        case _USB_UVC_AYUV:
            common_setting();
            ayuv_setting();
            break;
        case _USB_UVC_V410:
            common_setting();
            v410_setting();
            break;
        case _USB_UVC_Y410:
            common_setting();
            y410_setting();
            break;
        case _USB_UVC_UYVY:
            common_setting();
            uyvy_setting();
            break;
        case _USB_UVC_V210:
            common_setting();
            v210_setting();
            break;
        case _USB_UVC_Y210:
            common_setting();
            y210_setting();
            break;
        case _USB_UVC_YV12:
            common_setting();
            yv12_setting();
            break;
        case _USB_UVC_RGB565:
            common_setting();
            rgb565_setting();
            break;
        case _USB_UVC_RGB555:
            common_setting();
            rgb555_setting();
            break;
        default:
            RTK_USB_DEVICE_DBG_SIMPLE("ERROR uvc unknow format %d\n", (UINT32)usb_uvc_format_real);
            break;
        }
        uvc_header_data_check  = 0xffffffff;
}

void rtk_uvc_stream_interface(struct usb_ctrlrequest *ctrl_req)
{
    //struct DMEM_COPY_TO_SRAM_STRUCT info;
    switch (ctrl_req->bRequestType) {
        case USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
            switch (ctrl_req->bRequest) {
                case UVC_SET_CUR:
                    if(usb_uvc_inited == 0) {
                        //pm_printk(LOGGER_ERROR, "[UVC] already init, just return\n");
                        return ;
                    }
                    //usb_uvc_set_timing_info(uvc_def.bFormatIndex, uvc_def.bFrameIndex);
                    if(ctrl_req->wValue == 0x0100) {
                        g_rtk_dwc3.ep0_last_callback_id = USB_UVC_SET_PROBE;
                    }
                    if(ctrl_req->wValue == 0x0200) {
                        g_rtk_dwc3.ep0_last_callback_id = USB_UVC_SET_COMMIT;
                    }
                    break;
                default:
                    break;
            }
            break;

        case USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
            switch (ctrl_req->bRequest) {
                case UVC_GET_CUR:
                case UVC_GET_MIN:
                case UVC_GET_MAX:
                case UVC_GET_RES:
                case UVC_GET_DEF:
                default:
                    uvc_def.bmHint = 0;
                    uvc_def.bFormatIndex = usb_uvc_format;
                    uvc_def.bFrameIndex = usb_uvc_frame;

#if 0
                    240    Hz -> , 0xC2, 0xA2, 0x00, 0x00    = 0x0000A2C2
                    144    Hz -> , 0x44, 0x0F, 0x01, 0x00    = 0x00010f44
                    120    Hz -> , 0x85, 0x45, 0x01, 0x00    = 0x00014585
                    119.88 Hz -> , 0xD8, 0x45, 0x01, 0x00    = 0x000145D8
                    60     Hh -> , 0x0A, 0x8B, 0x02, 0x00    = 0x00028B0A
                    59.94  Hz -> , 0xB1, 0x8B, 0x02, 0x00    = 0x00028BB1
                    50     Hz -> , 0x40, 0x0D, 0x03, 0x00    = 0x00030D40
                    48     Hz -> , 0xCD, 0x2D, 0x03, 0x00    = 0x00032DCD
                    40     Hz -> , 0x90, 0xD0, 0x03, 0x00    = 0x0003D090
                    30     Hz -> , 0x15, 0x16, 0x05, 0x00    = 0x00051615
                    29.97  Hz -> , 0x63, 0x17, 0x05, 0x00    = 0x00051763
                    25     Hz -> , 0x80, 0x1A, 0x06, 0x00    = 0x00061A80
#endif

                    switch(usb_uvc_freq) {
                        case 24000:
                            uvc_def.bFrameInterval = 0x0000A2C2;
                            break;
                        case 14400:
                            uvc_def.bFrameInterval = 0x00010f44;
                            break;
                        case 12000:
                            uvc_def.bFrameInterval = 0x00014585;
                            break;
                        case 11988:
                            uvc_def.bFrameInterval = 0x000145D8;
                            break;
                        case 6000:
                            uvc_def.bFrameInterval = 0x00028B0A;
                            break;
                        case 5994:
                            uvc_def.bFrameInterval = 0x00028BB1;
                            break;
                        case 5000:
                            uvc_def.bFrameInterval = 0x00030D40;
                            break;
                        case 4800:
                            uvc_def.bFrameInterval = 0x00032DCD;
                            break;
                        case 4000:
                            uvc_def.bFrameInterval = 0x0003D090;
                            break;
                        case 3000:
                            uvc_def.bFrameInterval = 0x00051615;
                            break;
                        case 2997:
                            uvc_def.bFrameInterval = 0x00051763;
                            break;
                        case 2500:
                            uvc_def.bFrameInterval = 0x00061A80;
                            break;
                        default:
                            uvc_def.bFrameInterval = 0x00051615;
                    }

                    uvc_def.wKeyFrameRate = 0;
                    uvc_def.wPFrameRate = 0;
                    uvc_def.wCompQuality = 0;
                    uvc_def.wCompWindowSize = 0;
                    uvc_def.wDelay = 0;

                    if(usb_uvc_format==1) {
                        // for YUY2
                        uvc_def.dwMaxVideoFrameSize = (UINT32)usb_uvc_width * (UINT32)usb_uvc_length * 2;
                    }

                    if(usb_uvc_format==2) {
                        // for MJPEG
                        uvc_def.wCompQuality = 0x2f;
                        uvc_def.dwMaxVideoFrameSize = 0x003f4800;
                        //mjpeg_setting();
                    }

                    if(usb_uvc_format==3) {
                        // for NV12
                        uvc_def.dwMaxVideoFrameSize = (UINT32)usb_uvc_width * (UINT32)usb_uvc_length * 3 / 2;
                    }

                    if(usb_uvc_format==4) {
                        // for I420
                        uvc_def.dwMaxVideoFrameSize = (UINT32)usb_uvc_width * (UINT32)usb_uvc_length * 3 / 2;
                    }

                    if(usb_uvc_format==5) {
                        // for RGB24
                        uvc_def.dwMaxVideoFrameSize = (UINT32)usb_uvc_width * (UINT32)usb_uvc_length * 3;
                    }

                    if(usb_uvc_format==6) {
                        // for RGB32
                        uvc_def.dwMaxVideoFrameSize = (UINT32)usb_uvc_width * (UINT32)usb_uvc_length * 4;
                    }

                    if(usb_uvc_format==7) {
                        // for P010
                        uvc_def.dwMaxVideoFrameSize = (UINT32)usb_uvc_width * (UINT32)usb_uvc_length * 3;
                    }

                    // for m420
                    //uvc_def.dwMaxVideoFrameSize = 0x002f7600;

                    uvc_def.dwMaxPayloadTransferSize = 0x4000;
                    g_rtk_dwc3.ep0_setup_buf_len = (sizeof(uvc_def) < ctrl_req->wLength)? sizeof(uvc_def): ctrl_req->wLength;
                    dwc_info.dst = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
                    dwc_info.src = &uvc_def;
                    dwc_info.len = g_rtk_dwc3.ep0_setup_buf_len;
                    dwc3_copy_to_memory2();

                    break;
                }
            break;
        default:
            //pm_printk(LOGGER_ERROR, "%s: invalid control ctrl_req->bRequestType:0x%02x, ctrl_req->bRequest:0x%02x, ctrl_req->wValue:0x%04x, ctrl_req->wIndex:0x%04x, ctrl_req->wLength:%d\n",
            //    __func__, ctrl_req->bRequestType, ctrl_req->bRequest, ctrl_req->wValue, ctrl_req->wIndex, ctrl_req->wLength);
            break;
    }
}

UINT8 __rtk_dwc3_gadget_ep_disable(UINT8 epnum);
UINT8 rtk_dwc3_ep_start_trans_uvc(UINT8 epnum, UINT32 buf_dma              );

void usb_uvc_reinit_disconnect(void) __banked;

void usb_uvc_reinit_connect(void) __banked
{
}

static UINT8 uvc_extension_tx_one(UINT8 ch)
{
    u16_uvc_temp = g_uvc_extension.log_buf.write_pos;

    if(((u16_uvc_temp + 1) % UVC_EXTENSION_BUF_SIZE) == g_uvc_extension.log_buf.read_pos) {
        return 0;
    }
    g_uvc_extension.log_buf.write_pos = ((u16_uvc_temp + 1) % UVC_EXTENSION_BUF_SIZE);
#ifdef 	CONFIG_ARCH_5281
    g_uvc_extension.log_buf.buf[u16_uvc_temp] = ch;
#else
    g_recv_buf[u16_uvc_temp] = ch;
#endif
    return 1;
}

UINT16 uvc_extension_tx(UINT8 *buf, UINT16 len)
{
    while(len) {
        if(uvc_extension_tx_one(*buf++) == 0)
            break;
        len--;
    }
    return len;
}

void uvc_extension_putch(UINT8 ch)
{
    __xdata UINT8 c = ch;
    uvc_extension_tx(&c, 1);
}

void uvc_extension_output(UINT16 len) __banked
{
    __xdata UINT8 *cmd = RTICE_ACQUIRE_R_TPDU();
    UINT8 checksum = 0; 

    while (--len) {
        checksum += *cmd;
        uvc_extension_putch(*cmd);
        cmd++;
    }
    uvc_extension_putch((UINT8) -checksum );
}

UINT16 uvc_extension_tx_directly(UINT8 *buf, UINT16 len)
{
    uvc_extension_tx(buf, len);
    return 0;
}

unsigned frame_y_offset = 0;
unsigned char frame_index = 0;
extern UINT8 uvc_is_runing;

UINT32 uvc_output_time = 0;
UINT32 usb_output_time = 0;

UINT32 write_frame_start = 0;
UINT32 write_frame_stop = 0;

void gen_frame(void)
{
    UINT8 temp, temp1;

    if(g_rtk_dwc3.uvc_show_log) {
        RTK_USB_DEVICE_WARNING_DEBUG_LEVEL("gen_frame wp=%d [%d %d %d]\n", uvc_frame_buffer_wp, uvc_frame_buffer_status[0], uvc_frame_buffer_status[1], uvc_frame_buffer_status[2]);
        RTK_USB_DEVICE_WARNING_DEBUG_LEVEL("usb_uvc_width_real=%d\n", usb_uvc_width_real);
    }

    uvc_frame_buffer_wp_old = uvc_frame_buffer_wp;
    temp = uvc_frame_buffer_wp + 1;
    temp = temp % UVC_FRAME_BUFFER_NUMBER;

    if(uvc_frame_buffer_status[temp] == 0) {
        if(now_usb_send_frame == temp) {
            temp1 = *(uvc_trb_vir[0] + (trb_number * 32) + 28);
            if(g_rtk_dwc3.uvc_show_log) {
                RTK_USB_DEVICE_DBG("gen_frame %d %d 0x%x\n", now_usb_send_frame, temp, temp1);
            }
            if( (temp1 & 0x01)==0x0 ) {
                uvc_frame_buffer_wp = temp;
            }
        } else {
            uvc_frame_buffer_wp = temp;
        }
    }

    uvc_frame_buffer_status[uvc_frame_buffer_wp] = 0;

    rtd_outl(UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_reg, UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_db_read_sel(1)|
                                                 UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_db_apply(0)|
                                                 UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_db_en(1));

    switch(usb_uvc_format_real) {
        case _USB_UVC_YUY2:
        case _USB_UVC_MotionJPEG:
            rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, uvc_frame_phy[uvc_frame_buffer_wp]);
            break;
        case _USB_UVC_NV12:
            rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, uvc_frame_phy[uvc_frame_buffer_wp] + 0); // uvc dma0 start address
            rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_reg, uvc_frame_phy[uvc_frame_buffer_wp] + (usb_uvc_width_real*usb_uvc_length_real)); // uvc dma1 start address
            break;
        case _USB_UVC_RGB24:
            rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, uvc_frame_phy[uvc_frame_buffer_wp] + (usb_uvc_width_real*(usb_uvc_length_real-1)*3)); //uvc dma start address
            break;
        case _USB_UVC_I420:
            rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, uvc_frame_phy[uvc_frame_buffer_wp] + 0); // uvc dma0 start address
            rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_reg, uvc_frame_phy[uvc_frame_buffer_wp] + (usb_uvc_width_real*usb_uvc_length_real)); // uvc dma1 start address
            rtd_outl(UVC_DMA_UVC_CAP2_DMA_WR_Start_addr_reg, uvc_frame_phy[uvc_frame_buffer_wp] + (usb_uvc_width_real*usb_uvc_length_real) + usb_uvc_width_real*(usb_uvc_length_real)/2/2); // uvc dma1 start address
            break;
        case _USB_UVC_P010:
            rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, uvc_frame_phy[uvc_frame_buffer_wp] + 0); // uvc dma0 start address
            rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_reg, uvc_frame_phy[uvc_frame_buffer_wp] + (usb_uvc_width_real*usb_uvc_length_real * 2)); // uvc dma1 start address
            break;
        default:
            break;
    }

    rtd_outl(UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_reg, UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_db_read_sel(1)|
                                                 UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_db_apply(1)|
                                                 UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_db_en(1));

    //memset(uvc_frame_vir[uvc_frame_buffer_wp], 0x00, 0x300000);
    //memset(uvc_frame_vir[uvc_frame_buffer_wp] + frame_y_offset*1280, 0x55, 1280*10);
    //memset(uvc_frame_vir[uvc_frame_buffer_wp], 0x00, 0x1000);

    if(g_rtk_dwc3.uvc_show_log) {
        RTK_USB_DEVICE_DBG("setting_frame wp=%d %x [%d %d %d]\n", uvc_frame_buffer_wp, uvc_frame_phy[uvc_frame_buffer_wp], uvc_frame_buffer_status[0], uvc_frame_buffer_status[1], uvc_frame_buffer_status[2]);
    }
}

void sw_gen_header(void)
{
    int i=0;
    unsigned long temp;

    temp = 0x100 * frame_index;
    // fill header data
    for(i=0;i< trb_number; i++) {
        memset(uvc_header_vir[0] + i * 16, 0x00, 0x10);
        rtd_mem_outl(uvc_header_vir[0] + i * 16, 0x0000800c + temp);
        rtd_mem_outl(uvc_header_vir[0] + i * 16 + 4, frame_count++);
    }
    memset(uvc_header_vir[0] + i * 16, 0x00, 0x10);
    rtd_mem_outl(uvc_header_vir[0] + i * 16, 0x0000820c + temp);
    rtd_mem_outl(uvc_header_vir[0] + i * 16 + 4, frame_count++);
}

extern UINT8 uvc_timing_change;
void sw_gen_trb(void)
{
    int i=0;
    UINT32 temp;

// use SW
    temp = uvc_frame_buffer_rp + 1;
    temp = temp % UVC_FRAME_BUFFER_NUMBER;

    if(uvc_frame_buffer_status[temp] == 1) {
        uvc_frame_buffer_rp = temp;
    }

    uvc_frame_buffer_status[uvc_frame_buffer_rp] = 1;

    temp = *(uvc_trb_vir[0] + (trb_number * 32) + 28);

    if(uvc_timing_change == 1) {
        temp = 0x00;
        uvc_timing_change = 0;
    }

    if( (temp & 0x01)==0x0 ) {
        // fill trb data
        for(i=0;i< trb_number; i++)
        {
            // header trb
            rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 0, uvc_header_phy[0] + (i*16));
            rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 4, 0x00000000);
            rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 8, 0x0000000c);
            rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 12, 0x00000015);

            // data trb
            rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 16, uvc_frame_phy[uvc_frame_buffer_rp] + (i*16372));
            rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 20, 0x00000000);
            rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 24, 0x00003ff4);
            rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 28, 0x00000011);
        }
        // header trb
        rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 0, uvc_header_phy[0] + (i*16));
        rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 4, 0x00000000);
        rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 8, 0x0000000c);
        rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 12, 0x000000815);

        // data trb
        rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 16, uvc_frame_phy[uvc_frame_buffer_rp] + (i*16372));
        rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 20, 0x00000000);
        rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 24, last_frame_size);
        rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 28, 0x00000811);

        // link trb
        rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 32, uvc_trb_phy[0]);
        rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 36, 0x00000000);
        rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 40, 0x00000000);
        rtd_mem_outl(uvc_trb_vir[0] + (i * 32) + 44, DWC3_TRBCTL_LINK_TRB | DWC3_TRB_CTRL_HWO);

        rtd_outl(DWC3_DEPCMD(5), 0x00020407);
    }else {
        if(g_rtk_dwc3.uvc_show_log) {
            RTK_USB_DEVICE_DBG("sw_gen_trb skip frame\n");
        }
    }

    uvc_frame_buffer_status[uvc_frame_buffer_rp] = 0;
    if(g_rtk_dwc3.uvc_show_log) {
        RTK_USB_DEVICE_DBG("send frame %d done [%d %d %d]\n", uvc_frame_buffer_rp, uvc_frame_buffer_status[0], uvc_frame_buffer_status[1], uvc_frame_buffer_status[2]);
    }
}

void pattern_gen_one_frame(void)
{
    if(uvc_is_runing == 0)
        return;

    if(rtd_inl(TIMER_SCPU_CLK90K_LO_reg) < uvc_output_time)
        return;

    //RTK_USB_DEVICE_DBG("pattern_gen_one_frame %d\n", rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
    uvc_output_time = rtd_inl(TIMER_SCPU_CLK90K_LO_reg) + 90*frame_rate;

    gen_frame();

    frame_y_offset = frame_y_offset + 10;
    if(frame_y_offset >= 700)
        frame_y_offset = 0;
}

void usb_output_one_frame(void)
{
    if(uvc_is_runing == 0)
        return;

    if(rtd_inl(TIMER_SCPU_CLK90K_LO_reg) < usb_output_time)
        return;

    usb_output_time = rtd_inl(TIMER_SCPU_CLK90K_LO_reg) + 90*frame_rate;

    sw_gen_header();
    sw_gen_trb();

    frame_index++;
    frame_index = frame_index & 0x1;
}

void gen_header_trb(void)
{
    UINT32 temp, temp1, temp2;

    temp2 = *(uvc_trb_vir[0] + (trb_number * 32) + 28);

    if( (temp2 & 0x01)==0x0 ) {
        temp = uvc_frame_buffer_rp + 1;
        temp = temp % UVC_FRAME_BUFFER_NUMBER;
        
        if(uvc_frame_buffer_status[temp] == 1) {
            uvc_frame_buffer_status[uvc_frame_buffer_rp] = 0;
            uvc_frame_buffer_rp = temp;
        }
        uvc_frame_buffer_status[uvc_frame_buffer_rp] = 1;
    }

    if(uvc_timing_change == 1) {
        temp2 = 0x00;
        uvc_timing_change = 0;
    }

    if(usb_uvc_format_real == _USB_UVC_MotionJPEG) {
        trb_number      = uvc_frame_buffer_real_size[uvc_frame_buffer_rp] / 16372;
        last_frame_size = uvc_frame_buffer_real_size[uvc_frame_buffer_rp] % 16372;
        if(g_rtk_dwc3.uvc_show_log) {
            RTK_USB_DEVICE_DBG( "trb_number=%d last_frame_size=%d\n", trb_number, last_frame_size);
        }
    }

    if(trb_number == 0)
        temp2 = 0x01;

    if(last_frame_size == 0)
        last_frame_size = 16;

    if( (temp2 & 0x01)==0x0 ) {

        now_usb_send_frame = uvc_frame_buffer_rp;

        rtd_outl(UVC_DMA_UVC_top_int_data_reg, UVC_DMA_UVC_top_int_data_get_trbdma_wdone(1)|UVC_DMA_UVC_top_int_data_get_headerdma_wdone(1));
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //       Setting Header
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //rtd_outl(0x18004970, 0x00000000); // HEADER_DMA_CTRL1  H01
        rtd_outl(UVC_DMA_HEADER_DMA_CTRL1_reg, UVC_DMA_HEADER_DMA_CTRL1_headerdma_enable(0));
        //rtd_outl(0x18004940, 0x00000000); // UVC_HEADER_Ctrl   H02
        rtd_outl(UVC_DMA_UVC_HEADER_Ctrl_reg, UVC_DMA_UVC_HEADER_Ctrl_header_start_flag(0));
        //rtd_outl(0x18004944, 0x7cfb6000); // UVC_HEAD_ADDR     H03
        rtd_outl(UVC_DMA_UVC_HEAD_ADDR_reg, uvc_header_phy[0]);
        //rtd_outl(0x18004948, 0x000000FD); // UVC_HEAD_NUM      H04
        rtd_outl(UVC_DMA_UVC_HEAD_NUM_reg, trb_number);
        //rtd_outl(0x18004950, 0x55555555); // UVC_PTS_VALUE     H05
        rtd_outl(UVC_DMA_UVC_PTS_VALUE_reg, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
        //rtd_outl(0x18004954, 0xAAAA0000); // UVC_SCR_VALUE     H06
        rtd_outl(UVC_DMA_UVC_SCR_VALUE_reg, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
        //rtd_outl(0x18004958, 0x00000001); // UVC_SCR_STEP      H07
        temp = rtd_inl(UVC_DMA_UVC_HEAD_NUM_reg);
        rtd_outl(UVC_DMA_UVC_SCR_STEP_reg, 27000*usb_uvc_freq_real/100 / temp);

        //rtd_outl(0x180049A8, 0x100000FD); // [30:24] : burst length, [22:0] burst_num     UVC_HEAD_ADDR* 16/ 16   H08
        temp = rtd_inl(UVC_DMA_UVC_HEAD_NUM_reg);
        rtd_outl(UVC_DMA_HEADER_WR_DMA_num_bl_wrap_reg, UVC_DMA_HEADER_WR_DMA_num_bl_wrap_header_burst_len(0x10)|UVC_DMA_HEADER_WR_DMA_num_bl_wrap_header_line_burst_num(temp));

        //rtd_outl(0x18004970, 0x00000001); // HEADER_DMA_CTRL1  H09
        rtd_outl(UVC_DMA_HEADER_DMA_CTRL1_reg, UVC_DMA_HEADER_DMA_CTRL1_headerdma_enable(1));
        //rtd_outl(0x18004940, 0x00100100); // UVC_HEADER_Ctrl   H10
        rtd_outl(UVC_DMA_UVC_HEADER_Ctrl_reg, UVC_DMA_UVC_HEADER_Ctrl_header_start_flag(1)|
                                               UVC_DMA_UVC_HEADER_Ctrl_header_frame_id(frame_index)|
                                               UVC_DMA_UVC_HEADER_Ctrl_pts_scr_en(1)|
                                               UVC_DMA_UVC_HEADER_Ctrl_header_gen_swmode(0)|
                                               UVC_DMA_UVC_HEADER_Ctrl_scr_mode(1));

        temp = 0x100 * frame_index;
        memset(uvc_header_vir[0] + trb_number * 16, 0x00, 0x10);
        rtd_mem_outl(uvc_header_vir[0] + trb_number * 16, 0x0000820c + temp);
        rtd_mem_outl(uvc_header_vir[0] + trb_number * 16 + 4, frame_count++);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //       Setting TRB
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        temp = rtd_inl(UVC_DMA_UVC_top_int_data_reg);
        rtd_outl(UVC_DMA_UVC_TRB_GEN_Ctrl_reg, UVC_DMA_UVC_TRB_GEN_Ctrl_trb_gen_en(1));
        rtd_outl(UVC_DMA_UVC_TRB_ADDR_reg, uvc_trb_phy[0]);
        temp = rtd_inl(UVC_DMA_UVC_HEAD_NUM_reg);
        rtd_outl(UVC_DMA_UVC_TRB_NUM_reg, temp);
        rtd_outl(UVC_DMA_UVC_TRB_DBUFF_ADDR_0_reg, uvc_frame_phy[uvc_frame_buffer_rp]);
        rtd_outl(UVC_DMA_UVC_TRB_HBUFF_ADDR_0_reg, uvc_header_phy[0]);
        rtd_outl(UVC_DMA_TRB_WR_DMA_num_bl_wrap_reg, UVC_DMA_TRB_WR_DMA_num_bl_wrap_trb_burst_len(0x10) | UVC_DMA_TRB_WR_DMA_num_bl_wrap_trb_line_burst_num(temp*2));
        rtd_outl(UVC_DMA_TRBDMA_DMA_CTRL1_reg, UVC_DMA_TRBDMA_DMA_CTRL1_trbdma_enable(1));
        rtd_outl(UVC_DMA_UVC_TRB_GEN_Ctrl_reg, UVC_DMA_UVC_TRB_GEN_Ctrl_trb_start_flag(1) | UVC_DMA_UVC_TRB_GEN_Ctrl_trb_gen_en(1));

        udelay(100);

        // header trb
        rtd_mem_outl(uvc_trb_vir[0] + (trb_number * 32) + 0, uvc_header_phy[0] + (trb_number*16));
        rtd_mem_outl(uvc_trb_vir[0] + (trb_number * 32) + 4, 0x00000000);
        rtd_mem_outl(uvc_trb_vir[0] + (trb_number * 32) + 8, 0x0000000c);
        rtd_mem_outl(uvc_trb_vir[0] + (trb_number * 32) + 12, 0x000000815);

        // data trb
        rtd_mem_outl(uvc_trb_vir[0] + (trb_number * 32) + 16, uvc_frame_phy[uvc_frame_buffer_rp] + (trb_number*16372));
        rtd_mem_outl(uvc_trb_vir[0] + (trb_number * 32) + 20, 0x00000000);
        rtd_mem_outl(uvc_trb_vir[0] + (trb_number * 32) + 24, last_frame_size);
        rtd_mem_outl(uvc_trb_vir[0] + (trb_number * 32) + 28, 0x00000811);

        // link trb
        rtd_mem_outl(uvc_trb_vir[0] + (trb_number * 32) + 32, uvc_trb_phy[0]);
        rtd_mem_outl(uvc_trb_vir[0] + (trb_number * 32) + 36, 0x00000000);
        rtd_mem_outl(uvc_trb_vir[0] + (trb_number * 32) + 40, 0x00000000);
        rtd_mem_outl(uvc_trb_vir[0] + (trb_number * 32) + 44, DWC3_TRBCTL_LINK_TRB | DWC3_TRB_CTRL_HWO);

        temp  = rtd_mem_inl(uvc_trb_vir[0] + 12);
        temp1 = rtd_inl(UVC_DMA_TRBDMA_DMA_status2_reg); 
        rtd_outl(DWC3_DEPCMD(5), 0x00020407);
        if(g_rtk_dwc3.uvc_show_log) {
            RTK_USB_DEVICE_DBG( "temp %x %x\n", temp, temp1);
        }
    } else {
        if(g_rtk_dwc3.uvc_show_log) {
            RTK_USB_DEVICE_DBG("send frame skip frame %d\n", uvc_frame_buffer_rp);
        }
    }
    uvc_frame_buffer_status[uvc_frame_buffer_rp] = 0;
    if(g_rtk_dwc3.uvc_show_log) {
        RTK_USB_DEVICE_DBG("send frame %d done [%d %d %d]\n", uvc_frame_buffer_rp, uvc_frame_buffer_status[0], uvc_frame_buffer_status[1], uvc_frame_buffer_status[2]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// RHAL Part
////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 RHAL_Set_UVC_Ready_uvcid(int uvcid)
{
    if( uvcid == last_uvcid ) {
        setting_uvc();
    }
    return 0;
}

