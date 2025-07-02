#include "arch.h"
#include <rbus/timer_reg.h>
#include "mach.h"
#include "rtk_io.h"

#include <rbus/sb2_reg.h>  /* SB2_SYNC */
#include <rbus/usb3_top_reg.h>

#include <rbus/sys_reg_reg.h>
#include <rbus/pll_reg_reg.h>
#include <rbus/uvc_vgip_reg.h>
#include <rbus/pll27x_reg_reg.h>
#include <rbus/pinmux_lvdsphy_reg.h>
#include <scaler/scalerCommon.h>

#include "usb_mac.h"
#include "usb_hid.h"
#include "usb_reg.h"
#include "usb_utility_config_bank41.h"
#include <linux/timer.h>
#include "usb_uvc_api.h"
#include "usb_uac_api.h"
#include "usb_cdc_api.h"
#include "timer_event.h"
#include "msg_queue_def.h"
#include "msg_q_function_api.h"
#include "msg_queue_ap_def.h"
#ifdef CONFIG_ENABLE_SW_AUDIO
#include "rtk_kdriver/audio_sw/rtk_audio_sw.h"
#endif
#if defined(SDCC)
#include <linux/string.h>
#endif

#include "usb_buf_mgr.h"
#include <rbus/mis_gpio_reg.h>

#include <rbus/stb_reg.h>
#include <rbus/wdog_reg.h>
#include <rbus/uvc_dma_reg.h>
#include <rbus/uvc_vgip_reg.h>

#include "user_data.h"
#include <rtk_kdriver/io.h>
#include <linux/delay.h>

#include "rtk_ice-at_input.h"

// for thread
#include <linux/kthread.h>

#include <linux/pageremap.h>
#include "rtk_usb_device_dbg.h"

// for cache
#include <asm/cacheflush.h>

#define code
#define dvr_to_phys
//===============================================================================================
//  temp temp temp temp for AT cmd      need to remove
//===============================================================================================
#if 1
UINT8 mc_uart_state;
UINT8 *__pointer_to_rtice_cmd_param;
UINT8 *__at_r_tpdu_send__no_reentrant;

UINT8 rtice_at_input(UINT8 data)
{
    return 0;
}

void __rtice_pure_ack__no_reentrant(void)
{
}
#endif

//===============================================================================================
// temp area need to remove
//===============================================================================================
#define USB_DEVICE_UAC_DATA_GEN


#define DELAY_TIMER_REG        TIMER_TC0CVR
#define DWC3_DEPCMD_CLEARPENDIN        0x800

#define  SYS_REG_SYS_SRST2_reg                                                   0xB8000108
#define  SYS_REG_SYS_SRST3_reg                                                   0xB800010C
#define  SYS_REG_SYS_CLKEN1_reg                                                  0xB8000114
#define  SYS_REG_SYS_CLKEN2_reg                                                  0xB8000118
#define  SYS_REG_SYS_SRST6_reg                                                   0xB8000128
#define  SYS_REG_SYS_CLKEN4_reg                                                  0xB8000130
#define  SYS_REG_SYS_CLKEN6_reg                                                  0xB8000138

//#define mc_rtice_tx_lock()  do { mc_uart_state |= (MC_RTICE_TX_LOCKED); } while(0)
//#define mc_rtice_tx_unlock() do {mc_uart_state &= ~(MC_RTICE_TX_LOCKED); } while(0)

#define STC_LENGHT                (138) /*8 + 128 + 2*/
#define STC_MAGIC_0               (0x80)
#define STC_MAGIC_1               (0x01)
#define STC_DATA_OFFSET           (0x8)
#define STC_DATA_SIZE             (128)
#define MAGIC_ENTER_RECOVERY_MODE 0x2379abcd
#define STX_MAGIC                 (0x81)

#define AT_CMD_I2C_WRITE             0x1c
#define AT_CMD_I2C_WRITE_Return_Size 0x9
#define MS 90
#define USB3_GEN1_FLAG            (0x00000005)

#define UVC_TIMING_LIST_COUNT 14

//#pragma nogcse
//PRAGMA_CODESEG(BANK41);
//PRAGMA_CONSTSEG(BANK41);

UINT8 uvc_is_runing = 0;
UINT8 uvc_timing_change = 0;
UINT8 *p_dwc_temp1_u8_1;

extern UINT8 *usb_addr_vir;
UINT8 run_uac = 0; 
extern UINT32 uvc_uid;
extern UINT32 g_UAC_OUT0_base_mps_size;

unsigned long long usb_phy_vir_offset;

extern UINT32 uvc_frame_phy[UVC_FRAME_BUFFER_NUMBER];
extern UINT8 *uvc_frame_vir[UVC_FRAME_BUFFER_NUMBER];

extern UINT32 uvc_header_phy[UVC_FRAME_BUFFER_NUMBER];
extern UINT8 *uvc_header_vir[UVC_FRAME_BUFFER_NUMBER];

extern UINT32 uvc_trb_phy[UVC_FRAME_BUFFER_NUMBER];
extern UINT8 *uvc_trb_vir[UVC_FRAME_BUFFER_NUMBER];

extern UINT8 uvc_frame_buffer_status[UVC_FRAME_BUFFER_NUMBER];
extern UINT8 uvc_frame_buffer_wp;
extern UINT8 uvc_frame_buffer_rp;

extern UINT32 trb_number;
extern UINT32 last_frame_size;

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
#ifdef CONFIG_USB_SUPPORT_U1U2
UINT8 g_is_uvc_uac_start = 0;
#endif

UINT8 *p_dwc_temp1_u8;
UINT32 dwc_temp1;
UINT32 g_at_cmd_i2c_data_sram_address;
extern UINT8* dwc_u8_temp1;
UINT8 g_usb_link_state = USB_LINK_STATE_RESET;

UINT8 *g_usb_iram = 0;       // usb buffer virtual address
UINT8 *g_usb_cur_alloc_base = 0;
UINT32 g_usb_iram_dma = 0;       // usb buffer PHY address

UINT8 g_usb_handshake = 0xff;

struct DMEM_COPY_TO_SRAM_STRUCT dwc_info;
struct SRAM_COPY_TO_DMEM_STRUCT dwc_copy_to_dmem_info;


struct dwc3_gadget_ep_cmd_params params;
struct dwc3_trb *trb;
struct dwc3_trb cdc_out_trb;
struct dwc3_trb cdc_in_trb;
union dwc3_event event;
UINT32 reg;
UINT8 ret;
UINT8 epnum;
static UINT16 timeout = 500;
UINT16 i;
UINT16 g_dwc3_event_left;
UINT8 g_dwc3_is_reinit = 0;
UINT8 g_direction;
UINT8 g_number;
//USB Gen2 error handle
UINT8 g_polling_cnt = 0;
UINT8 g_ssdis_cnt = 0;
UINT8 g_relink_cnt = 0;
UINT8 g_gen2link_cnt = 0;
UINT8 g_ss_inact_cnt = 0;
UINT8 g_rxdet_cnt = 0;
UINT8 g_dwc3_set_config_done = 0;
//USB Gen2 error handle end
struct usb_ctrlrequest g_ep0_ctrl_req;
struct dwc3_trb g_ep0_trbs[2];
UINT8 uvc_temp_data[32];
UINT8 g_recv_buf[150];

UINT32 usb_addr_phy;
UINT32 u32_rtk_time_stamp;
UINT32 u32_rtk_temp_stamp;
UAC_TYPE_SETTING UAC_IN_freq_type = _48K_16B;
UAC_TYPE_SETTING UAC_OUT0_freq_type = _48K_16B;
UAC_TYPE_SETTING UAC_OUT1_freq_type = _48K_16B;

UINT8 wait_do_control_transfer = 0;
UINT8 only_support_cdc = 0;
UINT8 rtk_dwc3_usb2_0_only = 0;
UINT8 rtk_dwc3_usb3_0_5G_only = 0;
UINT8 enter_u0_count = 0;
UINT8 uac_stream_out_inf_alt = 0;
UINT8 uac_stream_in0_inf_alt  = 0;
UINT8 uac_stream_in1_inf_alt = 0;
UINT8 dwc3_get_config_dt = 0;

UINT8 uac_in_number                                = 3;
UINT8 uac_out1_number                              = 4;
UINT8 uac_out2_number                              = 5;
UINT8 cdc_control_interface                        = 6;
UINT8 cdc_data_interface                           = 7;
UINT8 hid_interface                                = 8;

UINT8 u8_need_calculate_usb_boot_time = 1;

UINT32 uac_out_start_address;
UINT32 uac_out_end_address;
UINT32 uac_out_check_address;

UINT8 usb_link_state_count = 0;
UINT8 isMacOS = 0;
UINT8 *p_usb_dst;
UINT8 *p_usb_src;
UINT32 dwc3_copy_val;
UINT16 dwc3_copy_len;
UINT16 dwc3_copy_cp_len;
UINT32 dwc3_copy_ofst;

UINT32 dwc3_copy_to_dst;
void *dwc3_copy_to_src;

void *dwc3_copy_from_dst;
UINT32 dwc3_copy_from_src;

UINT8 usb_set_address = 0;

extern struct dwc3 g_rtk_dwc3;
extern UINT8 g_usb_uart_ready;
extern struct usb_uart g_usb_uart;
extern struct uvc_extension g_uvc_extension;
extern UINT16 uvc_ex_at_cmd_return_len;
extern UINT32 dwc3_at_cmd_i2c_write_timeout;
extern UINT32 dwc3_uac_out_enable_timeout;
extern UINT32 dwc3_uac_out2_enable_timeout;

extern UINT32 uvc_header_data_check;
extern UINT32 u32_uvc_temp;
extern UINT32 u32_uvc_temp_2;
extern UINT32 u32_uvc_temp_3;
extern UINT32 u32_uvc_temp_4;
extern UINT8  usb_uvc_connected;

extern UINT8  usb_uvc_format;
extern UINT16 usb_uvc_width;
extern UINT16 usb_uvc_length;
extern UINT16 usb_uvc_freq;

extern UINT8 usb_uvc_format_real;
extern UINT16 usb_uvc_width_real;
extern UINT16 usb_uvc_length_real;
extern UINT16 usb_uvc_freq_real;

extern UINT8 is_need_set_internal_pattern_gen;

extern UINT32 uvc_address_header_buffer;                   //PHY
extern UINT8  *uvc_address_header_buffer_vir_non_cache;     // Vir non cache
extern UINT32 uvc_address_data_buffer;                     //PHY
extern UINT8 *uvc_address_data_buffer_vir_non_cache;       // Vir non cache
extern UINT32 uvc_address_trb_buffer;                      // PHY
extern UINT8 *uvc_address_trb_buffer_vir_non_cache;        // Vir non cache

extern UINT32 uac_device_to_host_trb_addr;
extern UINT8  * uac_device_to_host_trb_addr_vir_non_cache;
extern UINT32 uac_host_to_device_trb_addr;
extern UINT8  * uac_host_to_device_trb_addr_vir_non_cache;
extern UINT32 uac_device_to_host_trb_addr_2;
extern UINT8  * uac_device_to_host_trb_addr_2_vir_non_cache;
extern UINT32 uac_host_to_device_trb_addr_2;
extern UINT8  * uac_host_to_device_trb_addr_2_vir_non_cache;

extern UINT8 *uac_device_to_host_buffer_addr_vir_non_cache;

extern UINT32 trb_wr_addr_last;

extern UINT8 mc_uart_state;

extern UINT8 uac_device_to_host_trb_cnt;
extern UINT8 uac_device_to_host_trb_cnt_2;

extern UINT32 u32_usb_time;
extern UINT32 u32_usb_time_temp;
#ifdef DWC3_RTK_ENABLE_USB_BOOT_TIME_DEBUG_LOG
extern UINT32 u32_usb_time_temp_begin;
extern UINT32 u32_usb_time_temp_end;
#endif

extern UINT8 trb_cnt;
extern UINT8 audio_speed;
extern UINT8 uac_timer_run_cnt;
extern UINT8 g_hdmi_audio_stable;

extern UINT32 uac_temp1;
extern UINT32 uac_temp2;

extern UINT32 dwc3_at_cmd_AccelerateMode;

UINT32 u32_sof_last  = 0;
UINT32 u32_sof_total = 0;
UINT32 u32_27M_last  = 0;
UINT32 u32_27M_total = 0;

UINT8 UsbSof_LocalCounter_diff_mode =0xff;
// 0xff: unknow
// 0x00: sof and 90k speed is the same
// 0x01: sof and 90k speed is not the same

void compare_UsbSof_LocalCouner_reset(void);

int enqueue_uart_rxbuf(UINT8 *pdata, UINT16 len);
void rtk_dwc3_usb_load_phy(void) __banked;
UINT8 usb3_phy_dump_dfe(void) __banked;
void rtk_dwc3_get_config_length_by_speed(UINT16 wLength) __banked;
void rtk_dwc3_get_config_data_by_speed(void) __banked;
void rtk_dwc3_get_device(UINT16 wLength) __banked;
void rtk_dwc3_get_string(UINT16 wLength) __banked;
UINT8 _rtice_input_ex(UINT16 data) __banked;
UINT8 rtk_dwc3_is_usb_control_by_application(void) __banked;
#if defined(CONFIG_ENABLE_EXT_MCU_UPGRADE_FW)
extern void rtk_ext_mcu_upgrade_firmware_callback(UINT8* buffer) __banked;
extern UINT8 g_ext_mcu_upgrade_status;
extern UINT8 check_usb_cdc_command_upgrade_stx(void);
#endif
#ifdef CONFIG_ENABLE_SW_AUDIO
extern void audio_uac_in_enable(BOOL isEnable) __banked;
extern void audio_set_uac_in_sampling_rate(UAC_IPT_SPEED ipt_speed, UAC_IPT_SR sr_index) __banked;
extern void audio_set_output_sampling_rate(OUTPUT_SR sampleRate) __banked;
#endif
#ifdef CONFIG_ENABLE_TYPEC_REL
extern UINT8 usb_typecpd_ready_cnt;
#endif
void rtk_usb3_phy_reset(void) __banked;

void show_control_transfer_info(void);
void create_uac_thread(void);
void rtk_usb_device_disable_interrupt(void);
void rtk_set_uvc_path_from_usb(unsigned short width, unsigned short  height);
void rtk_dwc3_init_uac_show_info(void);
void calculs_trb_last_size(void);
void rtk_uvc_dma_setting(void);
UINT8 rtice_at_input(UINT8 data);

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************

/**
 * usb_endpoint_type - get the endpoint's transfer type
 * @epd: endpoint to be checked
 *
 * Returns one of USB_ENDPOINT_XFER_{CONTROL, ISOC, BULK, INT} according
 * to @epd's transfer type.
 */
static inline int usb_endpoint_type(const struct usb_endpoint_descriptor *epd)
{
        return epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK;
}

/**
 * usb_endpoint_xfer_control - check if the endpoint has control transfer type
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint is of type control, otherwise it returns false.
 */
static inline int usb_endpoint_xfer_control(
        const struct usb_endpoint_descriptor *epd)
{
        return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
                USB_ENDPOINT_XFER_CONTROL);
}

/**
 * usb_endpoint_xfer_isoc - check if the endpoint has isochronous transfer type
 * @epd: endpoint to be checked
 *
 * Returns true if the endpoint is of type isochronous, otherwise it returns
 * false.
 */
static inline int usb_endpoint_xfer_isoc(
        const struct usb_endpoint_descriptor *epd)
{
        return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
                USB_ENDPOINT_XFER_ISOC);
}

/**
 * usb_endpoint_maxp - get endpoint's max packet size
 * @epd: endpoint to be checked
 *
 * Returns @epd's max packet
 */
static inline int usb_endpoint_maxp(const struct usb_endpoint_descriptor *epd)
{
        return epd->wMaxPacketSize;
}

struct usb_cdc_line_coding g_acm_line_coding = {
        .dwDTERate = cpu_to_le32(115200),
        .bCharFormat = 0,
        .bParityType = 0,
        .bDataBits = 8,
};

#if 1
struct usb_device_descriptor g_dev_descriptor = {
        .bLength = 0x12,
        .bDescriptorType = USB_DT_DEVICE,
        .bcdUSB = 0x0300,
        .bDeviceClass = 0xEF,
        .bDeviceSubClass = 0x02,
        .bDeviceProtocol = 0x01,
        .idVendor = 0x1D6B,
        .idProduct = 0x0102,
        .bcdDevice = 0x0100,
        .iManufacturer = 0x06,
        .iProduct = 0x07,  // FIXME: Should we provide string desc for iManu, iProduct, iSeral..?
        .iSerialNumber = 0x03,
        .bNumConfigurations = 0x01,
};
#else
code unsigned char g_dev_descriptor[20] = {
        0x12, 0x01, 0x20, 0x03, 0x00, 0x02, 0x01, 0x09, 0x6B, 0x1D, 0x02, 0x01, 0x04, 0x05, 0x01, 0x02, 0x02, 0x01, 0x00, 0x00
};
#endif

/* USB_DT_DEVICE_QUALIFIER: Device Qualifier descriptor */
code struct usb_qualifier_descriptor g_qual_descriptor = {
        .bLength = sizeof g_qual_descriptor,
        .bDescriptorType = USB_DT_DEVICE_QUALIFIER,
        .bcdUSB = 0x0210,
        .bDeviceClass = USB_CLASS_MISC,
        .bDeviceSubClass = 0x02,
        .bDeviceProtocol = 0x01,
        .bMaxPacketSize0 = 0x40,
        .bNumConfigurations = 0x1,
        .bRESERVED = 0,
};

code unsigned char g_bos_descriptor[42] = {
        0x05, 0x0F, 0x2A, 0x00, 0x03, 0x07, 0x10, 0x02, 0x1E, 0xF4, 0x00, 0x00, 0x0A, 0x10, 0x03, 0x00,
        0x0E, 0x00, 0x01, 0x0A, 0xFF, 0x07, 0x14, 0x10, 0x0A, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11,
        0x00, 0x00, 0x30, 0x40, 0x0A, 0x00, 0xB0, 0x40, 0x0A, 0x00
};

code unsigned char g_bos_descriptor_u3_ss_only[22] = {
        0x05, 0x0F, 0x16, 0x00, 0x02, 0x07, 0x10, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x10, 0x03, 0x00,
        0x0E, 0x00, 0x03, 0x00, 0x00, 0x00
};

struct usb_endpoint_descriptor dwc3_gadget_ep0_desc = {
        .bLength         = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bmAttributes    = USB_ENDPOINT_XFER_CONTROL,
        .wMaxPacketSize  = cpu_to_le16(512),
};

// Video control Endpoint
code static struct usb_endpoint_descriptor uvc_control_ep = {
        .bLength          = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType  = USB_DT_ENDPOINT,
        .bEndpointAddress = UVC_CONTROL_INTERFACE_ENDPOINTADDRESS,
        .bmAttributes     = USB_ENDPOINT_XFER_INT,
        .wMaxPacketSize   = 16,
        .bInterval        = 8,
};

// Video IN Endpoint
code static struct usb_endpoint_descriptor uvc_hs_streaming_ep = {
        .bLength          = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType  = USB_DT_ENDPOINT,
        .bEndpointAddress = UVC_STREAM_INTERFACE_ENDPOINTADDRESS,
        .bmAttributes     = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize   = 512,
};

// HID IN Endpoint
code static struct usb_endpoint_descriptor hid_hs_streaming_ep = {
        .bLength          = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType  = USB_DT_ENDPOINT,
        .bEndpointAddress = HID_EP_ADDRESS,
        .bmAttributes     = USB_ENDPOINT_XFER_INT,
        .wMaxPacketSize   = 512,
};

// Audio OUT Endpoint
code static struct usb_endpoint_descriptor uac_hs_epout_desc = {
        .bLength          = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType  = USB_DT_ENDPOINT,
        .bEndpointAddress = UAC_STREAM_OUT_INTERFACE_ENDPOINTADDRESS, //USB_DIR_OUT
        .bmAttributes     = USB_ENDPOINT_XFER_ISOC | USB_ENDPOINT_SYNC_ASYNC,
        .wMaxPacketSize   = UAC_MPS_SIZE,
        .bInterval        = UAC_IN__INTERVAL,
};

// Audio IN0 Endpoint
code static struct usb_endpoint_descriptor uac_hs_epin0_desc = {
        .bLength          = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType  = USB_DT_ENDPOINT,
        .bEndpointAddress = UAC_STREAM_IN0_INTERFACE_ENDPOINTADDRESS, // USB_DIR_IN
        .bmAttributes     = USB_ENDPOINT_XFER_ISOC | USB_ENDPOINT_SYNC_ASYNC,
        .wMaxPacketSize   = UAC_MPS_SIZE,
        .bInterval        = UAC_OUT_INTERVAL,
};

// Audio IN1 Endpoint
code static struct usb_endpoint_descriptor uac_hs_epin1_desc = {
        .bLength          = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType  = USB_DT_ENDPOINT,
        .bEndpointAddress = UAC_STREAM_IN1_INTERFACE_ENDPOINTADDRESS, // USB_DIR_IN
        .bmAttributes     = USB_ENDPOINT_XFER_ISOC | USB_ENDPOINT_SYNC_ASYNC,
        .wMaxPacketSize   = UAC_MPS_SIZE,
        .bInterval        = UAC_OUT_INTERVAL,
};

struct usb_endpoint_descriptor rtk_serial_hs_bulk_in_desc = {
        .bLength          = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType  = USB_DT_ENDPOINT,
        .bEndpointAddress = CDC_EP_BULK_IN,
        .bmAttributes     = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize   = cpu_to_le16(512),
        .bInterval        = 1,
};
struct usb_endpoint_descriptor rtk_serial_hs_bulk_out_desc = {
        .bLength          = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType  = USB_DT_ENDPOINT,
        .bEndpointAddress = CDC_EP_BULK_OUT,
        .bmAttributes     = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize   = cpu_to_le16(512),
        .bInterval        = 1,
};

//****************************************************************************
// CODE TABLES
//****************************************************************************
code static unsigned char g_other_speed_config_descriptor[] = {
        /*struct usb_config_descriptor ofset:0*/
        0x9, //bLength
        USB_DT_OTHER_SPEED_CONFIG,//bDescriptorType other speed
        0x4B, 0x00,  //wTotalLength
        0x2, //bNumInterfaces
        0x1, //bConfigurationValue;
        0x0, //iConfiguration;
        0x80, //__u8  bmAttributes;
        0x26, //__u8  bMaxPower;

        /*struct usb_interface_assoc_descriptor ofset:9*/
        0x8, //bLength
        USB_DT_INTERFACE_ASSOCIATION, //bDescriptorType
        H5_SERIAL_CONTROL_INTERFACE_NUM,//bFirstInterface
        0x2, //bInterfaceCount control + data
        USB_CLASS_COMM, //bFunctionClass
        USB_CDC_SUBCLASS_ACM, //bFunctionSubClass
        USB_CDC_ACM_PROTO_AT_V25TER, //bFunctionProtocol
        0x0, //iFunction

        /*struct usb_interface_descriptor acm_control_interface_desc ofset:17*/
        0x9,//bLength;
        USB_DT_INTERFACE, //bDescriptorType
        H5_SERIAL_CONTROL_INTERFACE_NUM, //bInterfaceNumber
        0x0, //bAlternateSetting
        0x1, //bNumEndpoints
        USB_CLASS_COMM, //bInterfaceClass
        USB_CDC_SUBCLASS_ACM, //bInterfaceSubClass
        USB_CDC_ACM_PROTO_AT_V25TER, //bInterfaceProtocol
        0x0, //iInterface

        /*struct usb_endpoint_descriptor rtk_serial_hs_int_in_des ofset:26*/
        0x7, //bLength
        USB_DT_ENDPOINT, //bDescriptorType
        CDC_EP_INT, //bEndpointAddress
        USB_ENDPOINT_XFER_INT, //.bmAttributes
        0x40, 0x00,//wMaxPacketSize
        4, //bInterval

        /*struct usb_cdc_header_desc   ofset:39*/
        0x5, //bLength
        USB_DT_CS_INTERFACE, // bDescriptorType
        USB_CDC_HEADER_TYPE, // bDescriptorSubType
        0x10, 0x01, //bcdCDC

        /*struct usb_cdc_call_mgmt_descriptor  ofset:44*/
        0x5, //bLength
        USB_DT_CS_INTERFACE, //bDescriptorType
        USB_CDC_CALL_MANAGEMENT_TYPE, //bDescriptorSubType
        0x0, //bmCapabilities
        H5_SERIAL_DATA_INTERFACE_NUM, //bDataInterface

        /*struct usb_cdc_acm_descriptor ofset:49*/
        0x4, //bLength
        USB_DT_CS_INTERFACE, //bDescriptorType
        USB_CDC_ACM_TYPE, //bDescriptorSubType
        USB_CDC_CAP_LINE, //bmCapabilities

        /*struct usb_cdc_union_desc ofset:53*/
        0x5, //bLength
        USB_DT_CS_INTERFACE, //bDescriptorType
        USB_CDC_UNION_TYPE, //.bDescriptorSubType
        H5_SERIAL_CONTROL_INTERFACE_NUM, //bMasterInterface0
        H5_SERIAL_DATA_INTERFACE_NUM, //bSlaveInterface0

        /*struct usb_interface_descriptor acm_data_interface_desc  ofset:58*/
        0x9,//bLength
        USB_DT_INTERFACE,//bDescriptorType
        H5_SERIAL_DATA_INTERFACE_NUM, //bInterfaceNumber
        0x0, //bAlternateSetting
        0x2, //bNumEndpoints
        USB_CLASS_CDC_DATA, //bInterfaceClass
        0x0,  //bInterfaceSubClass
        0x0, //bInterfaceProtocol
        0x0, //iInterface

        /*struct usb_endpoint_descriptor rtk_serial_hs_bulk_in_desc  ofset:67*/
        0x7, //bLength
        USB_DT_ENDPOINT, //bDescriptorType
        CDC_EP_BULK_IN, //bEndpointAddress
        0x02, //bmAttributes
        0x40, 0x00, //wMaxPacketSize ofst:58
        0x01, //bInterval

        /*struct usb_endpoint_descriptor rtk_serial_hs_bulk_out_desc ofset:80*/
        0x7, //bLength
        USB_DT_ENDPOINT, //bDescriptorType
        CDC_EP_BULK_OUT, //bEndpointAddress
        0x02, //bmAttributes
        0x40, 0x00, //wMaxPacketSize ofst:65
        0x01, //bInterval
};

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
void dwc3_enable_uac_timer(void)
{
}

void dwc3_disable_uac_timer(void)
{
        audio_speed = 128;
        uac_timer_run_cnt = 0;
}

void dwc3_copy_from_memory2(void)
{
        dmac_flush_range( dwc_copy_to_dmem_info.src, dwc_copy_to_dmem_info.src + dwc_copy_to_dmem_info.len);
        outer_flush_range( (UINT32)((unsigned long long)dwc_copy_to_dmem_info.src - usb_phy_vir_offset), (UINT32)((unsigned long long)dwc_copy_to_dmem_info.src - usb_phy_vir_offset) + dwc_copy_to_dmem_info.len);
        memcpy(dwc_copy_to_dmem_info.dst, dwc_copy_to_dmem_info.src, dwc_copy_to_dmem_info.len);
}

void dwc3_copy_to_memory2(void)
{
        memcpy(dwc_info.dst, dwc_info.src, dwc_info.len);
        dmac_flush_range( dwc_info.dst, dwc_info.dst + dwc_info.len);
        outer_flush_range( (UINT32)((unsigned long long)dwc_info.dst - usb_phy_vir_offset), (UINT32)((unsigned long long)dwc_info.dst - usb_phy_vir_offset) + dwc_info.len);
}

void dwc3_memcpy(void *dst, void *src, UINT16 len)
{
        p_usb_dst = (UINT8 *)dst;
        p_usb_src = (UINT8 *)src;

        if( (dst == NULL) || (src == NULL)) {
                dwc_printk(0, "[ERROR] %s fail\n", __func__);
                return;
        }
        for (i = 0; i < len; i++) {
                *p_usb_dst++ = *p_usb_src++;
        }
}


void dwc3_memset(UINT8 *buf, UINT8 value, UINT16 len)
{
        if(buf == NULL) {
                dwc_printk(0, "[ERROR] %s fail\n", __func__);
                return;
        }
        memset(buf, value, len);
}

UINT8 rtk_dwc3_gadget_run_stop(UINT8 is_on)
{
        timeout = 9999;
        g_usb_handshake = 0;

        reg = rtd_inl(DWC3_DCTL);
        if (is_on) {
                reg &= ~DWC3_DCTL_KEEP_CONNECT;
                reg |= DWC3_DCTL_RUN_STOP;
        } else {
                reg &= ~DWC3_DCTL_RUN_STOP;
        }

        rtd_outl(DWC3_DCTL, reg);

        do {
                reg = rtd_inl(DWC3_DSTS);
                if (is_on) {
                        if (!(reg & DWC3_DSTS_DEVCTRLHLT)) {
                                break;
                        }
                } else {
                        if (reg & DWC3_DSTS_DEVCTRLHLT)
                                break;
                }
                timeout--;
                if (!timeout) {
                        pm_printk(LOGGER_ERROR, "[ER] run_stop timeout\n");
                        return -ETIMEDOUT;
                }
                delayus(10);
        } while (1);
        return 0;
}

UINT8* rtk_dwc3_usb_sram_alloc(UINT16 len, UINT8 align)
{
        unsigned long long temp1;
        temp1 = (unsigned long long)g_usb_cur_alloc_base;
        if(align && (temp1 & (align - 1 )))
                temp1 += (align - ((dwc_temp1) & (align - 1 )));
        g_usb_cur_alloc_base = (UINT8*)(temp1 + len);
        return (UINT8*)temp1;
}

void AT_System_Send_ExternalMCU_cb(void) __banked;

UINT8 u8_uvc_timing_list;

struct dwc3_uvc_timing_list {
        UINT8       timing_byte0;
        UINT8       timing_byte1;
        UINT16      freq;
};

struct dwc3_uvc_timing_list g_uvc_timing_list[UVC_TIMING_LIST_COUNT] = {
        {0x80, 0x1a, 2500},
        {0x62, 0x17, 2997},
        {0x63, 0x17, 2997},
        {0x15, 0x16, 3000},
        {0x90, 0xd0, 4000},
        {0xCD, 0x2D, 4800},
        {0x40, 0x0D, 5000},
        {0xB1, 0x8B, 5994},
        {0x0A, 0x8B, 6000},
        {0xA0, 0x86, 10000},
        {0xD8, 0x45, 11988},
        {0x85, 0x45, 12000},
        {0x44, 0x0f, 14400},
        {0xC2, 0xA2, 24000},
};

void rtk_dwc3_callback_process_uvc_timing(void)
{
#if 0
        240    Hz -> 0xC2, 0xA2, 0x00, 0x00                           C2 A2 00 00
        144    Hz -> 0x44, 0x0F, 0x01, 0x00                           44 0F 01 00
        120    Hz -> 0x85, 0x45, 0x01, 0x00                           85 45 01 00
        119.88 Hz -> 0xD8, 0x45, 0x01, 0x00                           D8 45 01 00
        100    Hz -> 0xA0, 0x86, 0x01, 0x00                           A0 86 01 00
        60     Hz -> 0x0A, 0x8B, 0x02, 0x00                           0A 8B 02 00
        59.94  Hz -> 0xB1, 0x8B, 0x02, 0x00                           B1 8B 02 00
        50     Hz -> 0x40, 0x0D, 0x03, 0x00                           40 0D 03 00
        48     Hz -> 0xCD, 0x2D, 0x03, 0x00                           CD 2D 03 00
        40     Hz -> 0x90, 0xD0, 0x03, 0x00                           90 D0 03 00
        30     Hz -> 0x15, 0x16, 0x05, 0x00                           15 16 05 00
        29.97  Hz -> 0x63, 0x17, 0x05, 0x00                           63 17 05 00
        25     Hz -> 0x80, 0x1A, 0x06, 0x00                           80 1A 06 00
        24     Hz -> 0x9A, 0x5B, 0x06, 0x00                           9A 5B 06 00
#endif
        for(u8_uvc_timing_list = 0; u8_uvc_timing_list < UVC_TIMING_LIST_COUNT; u8_uvc_timing_list++) {
                if(uvc_temp_data[4] == g_uvc_timing_list[u8_uvc_timing_list].timing_byte0 && uvc_temp_data[5] == g_uvc_timing_list[u8_uvc_timing_list].timing_byte1)
                        usb_uvc_set_timing_info(uvc_temp_data[2], uvc_temp_data[3], g_uvc_timing_list[u8_uvc_timing_list].freq);
        }
}

void rtk_dwc3_callback_process_uvc(void)
{
        if(is_need_set_internal_pattern_gen == 0) {
                if(g_rtk_dwc3.ep0_last_callback_id == USB_UVC_SET_COMMIT) {
                        pm_printk(LOGGER_FATAL, "USB MSG_AP_UVC_DISCONNECT\n");
                        usb_uvc_disconnect(g_rtk_dwc3.eps[UVC_EP_NUMBER].number, 0);
                        core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_UVC_DISCONNECT, 0);
                }
        }

        rtk_dwc3_callback_process_uvc_timing();

        if(is_need_set_internal_pattern_gen == 0) {
                if(g_rtk_dwc3.ep0_last_callback_id == USB_UVC_SET_COMMIT) {
                        usb_uvc_format_real  = usb_uvc_format;
                        usb_uvc_width_real   = usb_uvc_width;
                        usb_uvc_length_real  = usb_uvc_length;
                        usb_uvc_freq_real    = usb_uvc_freq;
                        //setting_uvc();
                        usb_uvc_connect(g_rtk_dwc3.eps[UVC_EP_NUMBER].number);
                        pm_printk(LOGGER_FATAL, "USB MSG_AP_UVC_CONNECT\n");
                        core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_UVC_CONNECT, 0);
                }
        }
}

#define BUFFER_INDEX_ID_UVC_PROBE                 64
#define BUFFER_INDEX_ID_UVC_COMMIT                74

void rtk_dwc3_callback_process_uvc_probe(void) __banked
{
    pm_printk(0, "USB_UVC_SET_PROBE\n");
    g_rtk_dwc3.ep0_last_callback_id = USB_UVC_SET_PROBE;
#if 0
    uvc_temp_data[2] = rtd_mem_inl(0x47FF5D40);
    uvc_temp_data[3] = rtd_mem_inl(0x47FF5D44);
    uvc_temp_data[4] = rtd_mem_inl(0x47FF5D48);
    uvc_temp_data[5] = rtd_mem_inl(0x47FF5D4c);
#else
    uvc_temp_data[2] = g_recv_buf[BUFFER_INDEX_ID_UVC_PROBE + 0];
    uvc_temp_data[3] = g_recv_buf[BUFFER_INDEX_ID_UVC_PROBE + 1];
    uvc_temp_data[4] = g_recv_buf[BUFFER_INDEX_ID_UVC_PROBE + 2];
    uvc_temp_data[5] = g_recv_buf[BUFFER_INDEX_ID_UVC_PROBE + 3];
#endif
    rtk_dwc3_callback_process_uvc();
}

void rtk_dwc3_callback_process_uvc_commit(void) __banked
{
    pm_printk(0, "USB_UVC_SET_COMMIT\n");
    g_rtk_dwc3.ep0_last_callback_id = USB_UVC_SET_COMMIT;
#if 0
    uvc_temp_data[2] = rtd_mem_inl(0x47FF5D50);
    uvc_temp_data[3] = rtd_mem_inl(0x47FF5D54);
    uvc_temp_data[4] = rtd_mem_inl(0x47FF5D58);
    uvc_temp_data[5] = rtd_mem_inl(0x47FF5D5c);
#else
    uvc_temp_data[2] = g_recv_buf[BUFFER_INDEX_ID_UVC_COMMIT + 0];
    uvc_temp_data[3] = g_recv_buf[BUFFER_INDEX_ID_UVC_COMMIT + 1];
    uvc_temp_data[4] = g_recv_buf[BUFFER_INDEX_ID_UVC_COMMIT + 2];
    uvc_temp_data[5] = g_recv_buf[BUFFER_INDEX_ID_UVC_COMMIT + 3];
#endif
    rtk_dwc3_callback_process_uvc();
}

void rtk_dwc3_ep0_callback_process_uac(UINT16 actual_len)
{
                        dwc_copy_to_dmem_info.dst = uvc_temp_data;
                        dwc_copy_to_dmem_info.src = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
                        dwc_copy_to_dmem_info.len = actual_len;
                        dwc3_copy_from_memory2();

                        //pm_printk(LOGGER_FATAL, "UAC callback\n");
                        //pm_printk(LOGGER_ERROR,"%x\t",(UINT32)uvc_temp_data[0]);
                        //pm_printk(LOGGER_ERROR,"%x\t",(UINT32)uvc_temp_data[1]);
                        //pm_printk(LOGGER_ERROR,"%x\t",(UINT32)uvc_temp_data[2]);

                        if((rtd_inl(SYS_REG_SYS_CLKEN1_reg) & _BIT16) == 0)
                            return;

                        if((uvc_temp_data[0] == 0x80) && (uvc_temp_data[1] == 0xbb) && (uvc_temp_data[2] == 0x00)) {
                                // 48Khz
                                if(g_rtk_dwc3.ep_out_in_number == 0x83) {
                                        // UAC OUT1 (Device -> Host)
                                        if(usb_sw_cfg_GetUAC_UpStream_UAC2_Enable_BANK41() == _FUNCTION_ENABLE) {
                                                pm_printk(LOGGER_FATAL, "48K uac out\n");
                                                // disable
                                                usb_uac_in_disconnect(_UAC_OUT0);
                                                rtk_dwc3_stop_active_transfer(7);
                                                UAC_OUT0_freq_type = _48K_16B;
                                                usb_uac_alloc(UAC_IN_freq_type, UAC_OUT0_freq_type);
                                                usb_uac_out0_sram_init(UAC_OUT0_freq_type);
                                                // enable
                                                _rtk_dwc3_gadget_ep_enable(7, &uac_hs_epin0_desc, NULL, false);
                                                //usb_uac_in_connect(_UAC_OUT0);
#ifdef CONFIG_ENABLE_SW_AUDIO
                                                //audio_set_output_sampling_rate(OUTPUT_SR_48K);
#endif
                                        }
                                } else if(g_rtk_dwc3.ep_out_in_number == UAC_STREAM_IN1_INTERFACE_ENDPOINTADDRESS) {
                                        // UAC OUT2 (Device -> Host)
                                        if(usb_sw_cfg_GetUAC_UpStream_UAC1_Enable_BANK41() == _FUNCTION_ENABLE) {
                                                pm_printk(LOGGER_FATAL, "48K uac out2\n");
                                                // disable
                                                usb_uac_in_disconnect(_UAC_OUT1);
                                                rtk_dwc3_stop_active_transfer(13);
                                                UAC_OUT1_freq_type = _48K_16B;
                                                usb_uac_alloc(UAC_IN_freq_type, UAC_OUT0_freq_type);
                                                usb_uac_out1_sram_init(UAC_OUT1_freq_type);
                                                // enable
                                                _rtk_dwc3_gadget_ep_enable(13, &uac_hs_epin1_desc, NULL, false);
                                                //usb_uac_in_connect(_UAC_OUT0);
#ifdef CONFIG_ENABLE_SW_AUDIO
                                                //audio_set_output_sampling_rate(OUTPUT_SR_48K);
#endif
                                        }
                                } else if(g_rtk_dwc3.ep_out_in_number == 0x03) {
                                        // UAC IN (Host -> Device)
                                        if(usb_sw_cfg_GetUAC_DownStream_Enable_BANK41() == _FUNCTION_ENABLE) {
                                                pm_printk(LOGGER_FATAL, "48K uac in\n");
                                                // disable
                                                usb_uac_out_disconnect(6);
                                                rtk_dwc3_stop_active_transfer(6);
                                                UAC_IN_freq_type = _48K_16B;
                                                usb_uac_alloc(UAC_IN_freq_type, UAC_OUT0_freq_type);
                                                usb_uac_in_sram_init(UAC_IN_freq_type);
                                                //enable
                                                _rtk_dwc3_gadget_ep_enable(6, &uac_hs_epout_desc, NULL, false);
                                                usb_uac_out_connect(6);
#ifdef CONFIG_ENABLE_SW_AUDIO
                                                //audio_set_uac_in_sampling_rate(UAC_IPT_SPEED_FULL_8K, UAC_IPT_SR_48K);
                                                //audio_uac_in_enable(1);
#endif
                                        }
                                }
                        }
#ifdef SUPPORT_UAC_96K192K
                        else if((uvc_temp_data[0] == 0x00) && (uvc_temp_data[1] == 0x77) && (uvc_temp_data[2] == 0x01)) {
                                // 96Khz
                                if(g_rtk_dwc3.ep_out_in_number == 0x83) {
                                        // UAC OUT1 (Device -> Host)
                                        if(usb_sw_cfg_GetUAC_UpStream_UAC2_Enable_BANK41() == _FUNCTION_ENABLE) {
                                                pm_printk(LOGGER_FATAL, "96K uac out\n");
                                                // disable
                                                usb_uac_in_disconnect(_UAC_OUT0);
                                                rtk_dwc3_stop_active_transfer(7);
                                                UAC_OUT0_freq_type = _96K_16B;
                                                //usb_uac_alloc(UAC_IN_freq_type, UAC_OUT0_freq_type);
                                                usb_uac_out0_sram_init(UAC_OUT0_freq_type);
                                                // enable
                                                _rtk_dwc3_gadget_ep_enable(7, &uac_hs_epin0_desc, NULL, false);
                                                //usb_uac_in_connect(_UAC_OUT0);
#ifdef CONFIG_ENABLE_SW_AUDIO
                                                audio_set_output_sampling_rate(OUTPUT_SR_96K);
#endif
                                        }
                                } else if(g_rtk_dwc3.ep_out_in_number == UAC_STREAM_IN1_INTERFACE_ENDPOINTADDRESS) {
                                        // UAC OUT2 (Device -> Host)
                                        if(usb_sw_cfg_GetUAC_UpStream_UAC1_Enable_BANK41() == _FUNCTION_ENABLE) {
                                                pm_printk(LOGGER_FATAL, "96K uac out2\n");
                                                // disable
                                                usb_uac_in_disconnect(_UAC_OUT1);
                                                rtk_dwc3_stop_active_transfer(13);
                                                UAC_OUT1_freq_type = _96K_16B;
                                                //usb_uac_alloc(UAC_IN_freq_type, UAC_OUT0_freq_type);
                                                usb_uac_out1_sram_init(UAC_OUT1_freq_type);
                                                // enable
                                                _rtk_dwc3_gadget_ep_enable(13, &uac_hs_epin1_desc, NULL, false);
                                                //usb_uac_in_connect(_UAC_OUT0);
#ifdef CONFIG_ENABLE_SW_AUDIO
                                                audio_set_output_sampling_rate(OUTPUT_SR_96K);
#endif
                                        }
                                } else if(g_rtk_dwc3.ep_out_in_number == 0x03) {
                                        // UAC IN (Host -> Device)
                                        if(usb_sw_cfg_GetUAC_DownStream_Enable_BANK41() == _FUNCTION_ENABLE) {
                                                pm_printk(LOGGER_FATAL, "96K uac in\n");
                                                // disable
                                                usb_uac_out_disconnect(6);
                                                rtk_dwc3_stop_active_transfer(6);
                                                UAC_IN_freq_type = _96K_16B;
                                                //usb_uac_alloc(UAC_IN_freq_type, UAC_OUT0_freq_type);
                                                usb_uac_in_sram_init(UAC_IN_freq_type);
                                                //enable
                                                _rtk_dwc3_gadget_ep_enable(6, &uac_hs_epout_desc, NULL, false);
                                                usb_uac_out_connect(6);
#ifdef CONFIG_ENABLE_SW_AUDIO
                                                audio_set_uac_in_sampling_rate(UAC_IPT_SPEED_FULL_8K, UAC_IPT_SR_96K);
                                                audio_uac_in_enable(1);
#endif
                                        }
                                }
                        }
                        else if((uvc_temp_data[0] == 0x00) && (uvc_temp_data[1] == 0xEE) && (uvc_temp_data[2] == 0x02)) {
                                // 192Khz
                                if(g_rtk_dwc3.ep_out_in_number == 0x83) {
                                        // UAC OUT1 (Device -> Host)
                                        if(usb_sw_cfg_GetUAC_UpStream_UAC2_Enable_BANK41() == _FUNCTION_ENABLE) {
                                                pm_printk(LOGGER_FATAL, "192K uac out\n");
                                                // disable
                                                usb_uac_in_disconnect(_UAC_OUT0);
                                                rtk_dwc3_stop_active_transfer(7);
                                                UAC_OUT0_freq_type = _192K_16B;
                                                //usb_uac_alloc(UAC_IN_freq_type, UAC_OUT0_freq_type);
                                                usb_uac_out0_sram_init(UAC_OUT0_freq_type);
                                                // enable
                                                _rtk_dwc3_gadget_ep_enable(7, &uac_hs_epin0_desc, NULL, false);
                                                //usb_uac_in_connect(_UAC_OUT0);
#ifdef CONFIG_ENABLE_SW_AUDIO
                                                audio_set_output_sampling_rate(OUTPUT_SR_192K);
#endif
                                        }
                                } else if(g_rtk_dwc3.ep_out_in_number == UAC_STREAM_IN1_INTERFACE_ENDPOINTADDRESS) {
                                        // UAC OUT2 (Device -> Host)
                                        if(usb_sw_cfg_GetUAC_UpStream_UAC1_Enable_BANK41() == _FUNCTION_ENABLE) {
                                                pm_printk(LOGGER_FATAL, "192K uac out2\n");
                                                // disable
                                                usb_uac_in_disconnect(_UAC_OUT1);
                                                rtk_dwc3_stop_active_transfer(13);
                                                UAC_OUT1_freq_type = _192K_16B;
                                                //usb_uac_alloc(UAC_IN_freq_type, UAC_OUT0_freq_type);
                                                usb_uac_out1_sram_init(UAC_OUT1_freq_type);
                                                // enable
                                                _rtk_dwc3_gadget_ep_enable(13, &uac_hs_epin1_desc, NULL, false);
                                                //usb_uac_in_connect(_UAC_OUT0);
#ifdef CONFIG_ENABLE_SW_AUDIO
                                                audio_set_output_sampling_rate(OUTPUT_SR_192K);
#endif
                                        }
                                } else if(g_rtk_dwc3.ep_out_in_number == 0x03) {
                                        // UAC IN (Host -> Device)
                                        if(usb_sw_cfg_GetUAC_DownStream_Enable_BANK41() == _FUNCTION_ENABLE) {
                                                pm_printk(LOGGER_FATAL, "192K uac in\n");
                                                // disable
                                                usb_uac_out_disconnect(6);
                                                rtk_dwc3_stop_active_transfer(6);
                                                UAC_IN_freq_type = _192K_16B;
                                                //usb_uac_alloc(UAC_IN_freq_type, UAC_OUT0_freq_type);
                                                usb_uac_in_sram_init(UAC_IN_freq_type);
                                                //enable
                                                _rtk_dwc3_gadget_ep_enable(6, &uac_hs_epout_desc, NULL, false);
                                                usb_uac_out_connect(6);
#ifdef CONFIG_ENABLE_SW_AUDIO
                                                audio_set_uac_in_sampling_rate(UAC_IPT_SPEED_FULL_8K, UAC_IPT_SR_192K);
                                                audio_uac_in_enable(1);
#endif
                                        }
                                }
                        }
#endif
}

void rtk_dwc3_ep0_callback_process_uvc_extension_1(UINT16 actual_len)
{
                        //pm_printk(0, "USB_UVC_SET_CUR_ID\n");
                        //pm_printk(0, "actual_len = %x\n", (UINT32)actual_len);

                        if(actual_len > 150)
                                actual_len = 150;

                        dwc_copy_to_dmem_info.dst = g_recv_buf;
                        dwc_copy_to_dmem_info.src = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
                        dwc_copy_to_dmem_info.len = actual_len;
                        dwc3_copy_from_memory2();

                        //for(dwc_temp1=0;dwc_temp1< (g_recv_buf[1]+3); dwc_temp1++) {
                        //    pm_printk(0, "%x ", (UINT32)g_recv_buf[dwc_temp1]);
                        //}
                        //pm_printk(0, "\n");
                        //gCMDParam.connect_type = RTICE_CMD_CONNECT_TYPE_UVC_EX;
                        g_uvc_extension.log_buf.write_pos = 0;
                        g_uvc_extension.log_buf.read_pos = 0;
                        //enqueue_uart_rxbuf(g_recv_buf, actual_len);

                        /*check upgrade cmd, trigger watchdog reboot*/
                        if(actual_len == STC_LENGHT && g_recv_buf[0] == MODEM_STC &&
                            g_recv_buf[1] == STC_MAGIC_0 && g_recv_buf[2] == STC_MAGIC_1 &&
                            get_ymodem_crc(g_recv_buf + STC_DATA_OFFSET, STC_DATA_SIZE) == YMODEM_CRC(g_recv_buf  + STC_DATA_OFFSET + STC_DATA_SIZE)) {
                                if(memcmp(g_recv_buf + STC_DATA_OFFSET, "enter_rescue", 12) == 0) {
                                        //pm_printk(LOGGER_ERROR, "enter_rescue to upgrade\r\n");
                                        /*set dummy register to notify rescue entry recovery mode*/
                                        rtd_outl(STB_WDOG_DATA1, MAGIC_ENTER_RECOVERY_MODE);
                                        /*trigger watchdog reboot*/
                                        rtd_outl(WDOG_TCWCR, WDOG_TCWCR_im_wdog_rst(1));
                                        while(1);
                                }
#if defined(CONFIG_ENABLE_EXT_MCU_UPGRADE_FW_BY_CDC)
                                else if (memcmp(g_recv_buf + STC_DATA_OFFSET, "enter_upgrade_ext_mcu", 21) == 0) {
                                        //pm_printk(LOGGER_ERROR, "enter_upgrade_ext_mcu\n");
                                        g_ext_mcu_upgrade_status = 1;
                                        g_recv_buf[0] = MODEM_ACK;
                                        uvc_extension_tx_directly(g_recv_buf, 1);
                                } else if (memcmp(g_usb_uart.recv_buf + STC_DATA_OFFSET, "exit_upgrade_ext_mcu", 20) == 0) {
                                        //pm_printk(LOGGER_ERROR, "exit_upgrade_ext_mcu\n");
                                        g_ext_mcu_upgrade_status = 0;
                                        g_recv_buf[0] = MODEM_ACK;
                                        uvc_extension_tx_directly(g_recv_buf, 1);
                                }
#endif
                        }
#if defined(CONFIG_ENABLE_EXT_MCU_UPGRADE_FW_BY_CDC)
                        else if(check_usb_cdc_command_upgrade_stx()) {
                                rtk_ext_mcu_upgrade_firmware_callback(g_recv_buf + 2);
                        }
#endif
                        else  {
                                //gCMDParam.connect_type = RTICE_CMD_CONNECT_TYPE_UVC_EX;
                                //EA = 0;
                                mc_rtice_tx_lock();
                                if(g_recv_buf[4] == AT_CMD_I2C_WRITE) {
                                        dwc3_at_cmd_i2c_write_timeout = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
                                        dwc3_at_cmd_i2c_write_timeout = dwc3_at_cmd_i2c_write_timeout + 50 * MS;
                                        if( dwc3_at_cmd_i2c_write_timeout > (0xffffffff - 5 * MS) )
                                                dwc3_at_cmd_i2c_write_timeout = (0xffffffff - 5 * MS);
#ifdef CONFIG_ENABLE_I2C
                                        g_at_cmd_i2c_data_sram_address = g_rtk_dwc3.ep0_setup_buf_addr;
                                        AT_System_Send_ExternalMCU_cb();
#endif
                                        g_uvc_extension.log_buf.write_pos = AT_CMD_I2C_WRITE_Return_Size;
                                } else {
                                        _rtice_input_ex(actual_len);
                                }
                                if( dwc3_at_cmd_AccelerateMode == 1 ) {
                                        dwc3_at_cmd_i2c_write_timeout = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
                                        dwc3_at_cmd_i2c_write_timeout = dwc3_at_cmd_i2c_write_timeout + 100 * MS;
                                }

                                mc_rtice_tx_unlock();
                                //EA = 1;
                        }
}

void rtk_dwc3_ep0_callback(INT8 req_status, UINT16 actual_len)
{
        RTK_USB_DEVICE_INFO("dwc3_ep0_callback %x\n", g_rtk_dwc3.ep0_last_callback_id);
        if(req_status < 0)
                return;
        switch (g_rtk_dwc3.ep0_last_callback_id) {
                case USB_CDC_SET_LINE_CODING_ID:

                        if(actual_len != sizeof(g_acm_line_coding))
                                return;
                        dwc_copy_to_dmem_info.dst = &g_acm_line_coding;
                        dwc_copy_to_dmem_info.src = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
                        dwc_copy_to_dmem_info.len = actual_len;
                        dwc3_copy_from_memory2();
//                        RTK_USB_DEVICE_DBG_SIMPLE(  "SET_LINE_CODING_ID:%u,%u,%u,%u\n", g_acm_line_coding.dwDTERate,
//                                       g_acm_line_coding.bCharFormat, g_acm_line_coding.bParityType, g_acm_line_coding.bDataBits);
                        break;
                case USB_UAC_SET_CUR_ID:
                        rtk_dwc3_ep0_callback_process_uac(actual_len);
                        break;
                case USB_UVC_EX_SET_CUR_ID_CS1:
                case USB_UVC_EX_SET_CUR_ID_CS3:
                case USB_UVC_EX_SET_CUR_ID_CS4:
                        rtk_dwc3_ep0_callback_process_uvc_extension_1(actual_len);
                        break;
                case USB_UVC_EX_SET_CUR_ID_CS2:
                        dwc_copy_to_dmem_info.dst = &uvc_ex_at_cmd_return_len;
                        dwc_copy_to_dmem_info.src = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
                        dwc_copy_to_dmem_info.len = 2;
                        dwc3_copy_from_memory2();
                        break;
                case USB_UVC_PROCESSING_UINT_SET_CUR:
                        dwc_copy_to_dmem_info.dst = g_recv_buf;
                        dwc_copy_to_dmem_info.src = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
                        dwc_copy_to_dmem_info.len = actual_len;
                        dwc3_copy_from_memory2();
                        usb_uvc_set_cur(actual_len);
                        break;
                case USB_UVC_SET_PROBE:
                case USB_UVC_SET_COMMIT:
                        //dma_buf_request(DMA_USB_DESCRIPTOR_DDR, DWC3_USB_DESCRIPTOR_BUFFER_SIZE, &g_rtk_dwc3.ep0_descriptor_table_buf,  &g_rtk_dwc3.ep0_descriptor_table_buf);
                        g_rtk_dwc3.ep0_descriptor_table_buf = DWC3_USB_DESCRIPTOR_BUFFER_ADDRESS;
                        pm_printk(LOGGER_FATAL, "USB VS CS = %x\n", (UINT32)g_rtk_dwc3.ep0_last_callback_id);
                        dwc_copy_to_dmem_info.dst = uvc_temp_data;
                        dwc_copy_to_dmem_info.src = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
                        dwc_copy_to_dmem_info.len = actual_len;
                        dwc3_copy_from_memory2();
                        rtk_dwc3_callback_process_uvc_timing();
                        if(g_rtk_dwc3.ep0_last_callback_id == USB_UVC_SET_PROBE) {
#if 0
                            rtd_mem_outl(0x47FF5D40, (UINT32)uvc_temp_data[2]);
                            rtd_mem_outl(0x47FF5D44, (UINT32)uvc_temp_data[3]);
                            rtd_mem_outl(0x47FF5D48, (UINT32)uvc_temp_data[4]);
                            rtd_mem_outl(0x47FF5D4c, (UINT32)uvc_temp_data[5]);
#else
                            g_recv_buf[BUFFER_INDEX_ID_UVC_PROBE + 0] = uvc_temp_data[2];
                            g_recv_buf[BUFFER_INDEX_ID_UVC_PROBE + 1] = uvc_temp_data[3];
                            g_recv_buf[BUFFER_INDEX_ID_UVC_PROBE + 2] = uvc_temp_data[4];
                            g_recv_buf[BUFFER_INDEX_ID_UVC_PROBE + 3] = uvc_temp_data[5];
#endif
                            rtk_dwc3_callback_process_uvc_probe();
                            core_fw_msg_queue_addEvent(MSG_USB_ID, MSG_USB_UVC_PROBE, 0);
                        }
                        if(g_rtk_dwc3.ep0_last_callback_id == USB_UVC_SET_COMMIT) {
#if 0
                            rtd_mem_outl(0x47FF5D50, (UINT32)uvc_temp_data[2]);
                            rtd_mem_outl(0x47FF5D54, (UINT32)uvc_temp_data[3]);
                            rtd_mem_outl(0x47FF5D58, (UINT32)uvc_temp_data[4]);
                            rtd_mem_outl(0x47FF5D5c, (UINT32)uvc_temp_data[5]);
#else
                            g_recv_buf[BUFFER_INDEX_ID_UVC_COMMIT + 0] = uvc_temp_data[2];
                            g_recv_buf[BUFFER_INDEX_ID_UVC_COMMIT + 1] = uvc_temp_data[3];
                            g_recv_buf[BUFFER_INDEX_ID_UVC_COMMIT + 2] = uvc_temp_data[4];
                            g_recv_buf[BUFFER_INDEX_ID_UVC_COMMIT + 3] = uvc_temp_data[5];
#endif
                            rtk_dwc3_callback_process_uvc_commit();
                            calculs_trb_last_size();
                            //rtk_set_uvc_path_from_usb(usb_uvc_width_real, usb_uvc_length_real);
                            uvc_uid++;
                            rtk_uvc_dma_setting();
                            core_fw_msg_queue_addEvent(MSG_USB_ID, MSG_USB_UVC_COMMIT, 0);
                            uvc_is_runing = 1;
                            uvc_timing_change = 1;
                        }
                        break;
                case USB_HID_SET_REPORT:
                        rtk_hid_set_report_callback();
                        break;
        }
}

bool rtk_dwc3_soft_reset(void)
{
        dwc_temp1 = 0;
        rtd_outl(DWC3_DCTL, DWC3_DCTL_CSFTRST);
        while (dwc_temp1++ < 50000) {
                if (!(rtd_inl(DWC3_DCTL) & DWC3_DCTL_CSFTRST))
                        return true;
        }
        return false;
}

#if 0
static void rtk_dwc3_core_soft_reset(void)
{
        /* Before Resetting PHY, put Core in Reset */
        reg = rtd_inl(DWC3_GCTL);
        reg |= DWC3_GCTL_CORESOFTRESET;
        rtd_outl(DWC3_GCTL, reg);

        /* Assert USB3 PHY reset */
        reg = rtd_inl(DWC3_GUSB3PIPECTL(0));
        reg |= DWC3_GUSB3PIPECTL_PHYSOFTRST;
        rtd_outl(DWC3_GUSB3PIPECTL(0), reg);

        /* Assert USB2 PHY reset */
        reg = rtd_inl(DWC3_GUSB2PHYCFG(0));
        reg |= DWC3_GUSB2PHYCFG_PHYSOFTRST;
        rtd_outl(DWC3_GUSB2PHYCFG(0), reg);

        delayms(100);

        /* Clear USB3 PHY reset */
        reg = rtd_inl(DWC3_GUSB3PIPECTL(0));
        reg &= ~DWC3_GUSB3PIPECTL_PHYSOFTRST;
        rtd_outl( DWC3_GUSB3PIPECTL(0), reg);

        /* Clear USB2 PHY reset */
        reg = rtd_inl(DWC3_GUSB2PHYCFG(0));
        reg &= ~DWC3_GUSB2PHYCFG_PHYSOFTRST;
        rtd_outl(DWC3_GUSB2PHYCFG(0), reg);

        delayms(100);

        /* After PHYs are stable we can take Core out of reset state */
        reg = rtd_inl(DWC3_GCTL);
        reg &= ~DWC3_GCTL_CORESOFTRESET;
        rtd_outl(DWC3_GCTL, reg);
}
#endif

void rtk_dwc3_event_buffers_setup(void)
{
        g_rtk_dwc3.g_event_buf.buf = (UINT8 *)rtk_dwc3_usb_sram_alloc(EACH_EVENT_BUF_SIZE, 16);
        dwc_printk( 0, "rtk_dwc3_event_buffers_setup => g_rtk_dwc3.g_event_buf.buf=%x\n", (UINT32)g_rtk_dwc3.g_event_buf.buf);
        rtd_outl(DWC3_GEVNTADRLO(0), usb_addr_phy);
        rtd_outl(DWC3_GEVNTADRHI(0), 0);
        rtd_outl(DWC3_GEVNTSIZ(0), DWC3_GEVNTSIZ_SIZE(EACH_EVENT_BUF_SIZE));
        rtd_outl(DWC3_GEVNTCOUNT(0), 0);
}
#ifdef CONFIG_USB_SUPPORT_U1U2
static void dwc3_gadget_set_link_state(enum dwc3_link_state state)
{
        reg = rtd_inl(DWC3_DCTL);
        reg &= ~DWC3_DCTL_ULSTCHNGREQ_MASK;

        /* set requested state */
        reg |= DWC3_DCTL_ULSTCHNGREQ(state);
        rtd_outl(DWC3_DCTL, reg);

        timeout = 20000;

        while (timeout--) {
                reg = rtd_inl(DWC3_DSTS);
                if (DWC3_DSTS_USBLNKST(reg) == DWC3_LINK_STATE_U0)
                        break;
        }
}

static UINT8 dwc3_gadget_get_link_state(void)
{
        reg = rtd_inl(DWC3_DSTS);
        return DWC3_DSTS_USBLNKST(reg);
}
#endif

void rtk_dwc3_wake_up(void) __banked
{
#ifdef CONFIG_USB_SUPPORT_U1U2
        reg = dwc3_gadget_get_link_state();
        if(reg == DWC3_LINK_STATE_U1 ||
            reg == DWC3_LINK_STATE_U2 ||
            reg == DWC3_LINK_STATE_U3)
                dwc3_gadget_set_link_state(DWC3_LINK_STATE_RECOV);
#endif
}


UINT8 rtk_dwc3_send_gadget_ep_cmd(UINT8 ep, UINT32 cmd)
{
        UINT32 reg;
#ifdef CONFIG_USB_SUPPORT_U1U2
        if((cmd & 0xF) == DWC3_DEPCMD_STARTTRANSFER) {
                rtk_dwc3_wake_up();
        }
#endif
        reg = ep;
        timeout = 9999;
#ifdef CONFIG_ARCH_5281
        // for sync dbus/rbus in H5 LX5281
        rtd_inl(SB2_SYNC_reg);
#else
//        rtd_inl(0xb801A020);
#endif
        reg = (reg << 4);

        reg += USB_DWC3_DEV_GLOBAL_REG_START + 0x0708;

        rtd_outl(reg, params.param0);
        reg -= 0x4;
        rtd_outl(reg, params.param1);
        reg -= 0x4;
        rtd_outl(reg, params.param2);

        cmd |= DWC3_DEPCMD_CMDACT;

        reg += 0xc;
        rtd_outl(reg, cmd);

#if 0
        dwc_printk( 0, "rtk_dwc3_send_gadget_ep_cmd (ep_num=%x, DWC3_DEPCMD(ep)=%x, cmd=%x, params.param0=%x, params.param1=%x, params.param2=%x)\n",
                    (UINT32)ep, (UINT32)DWC3_DEPCMD(ep), (UINT32)cmd, (UINT32)params.param0, (UINT32)params.param1, (UINT32)params.param2);
#endif

        do {
                u32_uvc_temp_3 = rtd_inl(reg);
                if (!(u32_uvc_temp_3 & DWC3_DEPCMD_CMDACT)) {
                        if (DWC3_DEPCMD_STATUS(u32_uvc_temp_3)) {
                                pm_printk(LOGGER_ERROR, "USB send ep cmd fail ep=%x val=%x cmd=%x\n", (UINT32)ep, (UINT32)u32_uvc_temp_3, (UINT32)cmd);
                                return -EINVAL;
                        }
                        return 0;
                }
                /*
                * We can't sleep here, because it is also called from
                * interrupt context.
                */
                timeout--;
                if (!timeout) {
                        pm_printk(LOGGER_ERROR, "USB send ep cmd fail 2 ep=%x timeout\n", (UINT32)ep);
                        return -1;
                }
                delayus(1);
        } while (1);
}

static UINT8 rtk_dwc3_gadget_start_config(UINT8 epnum)
{
        UINT32 cmd;

        dwc3_memset((void *)&params, 0x00, sizeof(params));

        if (g_rtk_dwc3.eps[epnum].number != 1) {
                cmd = DWC3_DEPCMD_DEPSTARTCFG;
                //pm_printk(LOGGER_ERROR, "rtk_dwc3_gadget_start_config (DWC3_DEPCMD_DEPSTARTCFG=%x, cmd=%x)\n", (UINT32)DWC3_DEPCMD_DEPSTARTCFG, (UINT32)cmd);
                /* XferRscIdx == 0 for ep0 and 2 for the remaining */
                if (g_rtk_dwc3.eps[epnum].number > 1) {
                        if (g_rtk_dwc3.start_config_issued)
                                return 0;
                        g_rtk_dwc3.start_config_issued = true;
                        cmd |= DWC3_DEPCMD_PARAM(2);
                }
                return rtk_dwc3_send_gadget_ep_cmd(0, cmd);
        }
        return 0;
}

static UINT8 rtk_dwc3_gadget_set_ep_config(UINT8 epnum,
                const struct usb_endpoint_descriptor *desc,
                const struct usb_ss_ep_comp_descriptor *comp_desc,
                UINT8 ignore)
{
        UINT32 max_pkt_size = UAC_MPS_SIZE;

        dwc3_memset((void *)&params, 0x00, sizeof(params));

        params.param0 = DWC3_DEPCFG_EP_TYPE(usb_endpoint_type(desc))
                        | DWC3_DEPCFG_MAX_PACKET_SIZE(usb_endpoint_maxp(desc));

        /* Burst size is only needed in SuperSpeed mode */
        if ( (epnum > 1) && (g_rtk_dwc3.speed == USB_SPEED_SUPER)) {
                // FIXME: Should we mask to prevent 0 minus 1 get 0xffffffff and << 22 ?
                params.param0 |= (DWC3_DEPCFG_BURST_SIZE(g_rtk_dwc3.eps[epnum].maxburst - 1) & ((UINT32)0xf << 22));
        }

        dwc_printk(LOGGER_ERROR, "dwc3 set ep config maxburst= %x\n", (UINT32)g_rtk_dwc3.eps[epnum].maxburst);

        if(epnum == UVC_EP_NUMBER) {
                if (g_rtk_dwc3.speed <= USB_SPEED_HIGH) {
                        params.param0 = DWC3_DEPCFG_EP_TYPE(usb_endpoint_type(desc))
                                        | DWC3_DEPCFG_MAX_PACKET_SIZE(512);
                } else {
                        params.param0 = DWC3_DEPCFG_EP_TYPE(usb_endpoint_type(desc))
                                        | DWC3_DEPCFG_MAX_PACKET_SIZE(1024);
                }
                params.param0 |= (DWC3_DEPCFG_BURST_SIZE(USB_U3_Burst_Size - 1) & ((UINT32)0xf << 22));
        } else if(epnum == UAC1_IN_EP_NUMBER) {
                // UAC IN
                if(UAC_IN_freq_type == _48K_16B) {
                        max_pkt_size = UAC_MPS_SIZE;
                } else if(UAC_IN_freq_type == _96K_16B) {
                        max_pkt_size = UAC_MPS_SIZE_96K16B;
                } else if(UAC_IN_freq_type == _192K_16B) {
                        max_pkt_size = UAC_MPS_SIZE_192K16B;
                }
                params.param0 = DWC3_DEPCFG_EP_TYPE(usb_endpoint_type(desc))
                                | DWC3_DEPCFG_MAX_PACKET_SIZE(max_pkt_size);
        } else if(epnum == UAC1_OUT_EP_NUMBER) {
                // UAC OUT0
                if(UAC_OUT0_freq_type == _48K_16B) {
                        max_pkt_size = UAC_MPS_SIZE;
                } else if(UAC_OUT0_freq_type == _96K_16B) {
                        max_pkt_size = UAC_MPS_SIZE_96K16B;
                } else if(UAC_OUT0_freq_type == _192K_16B) {
                        max_pkt_size = UAC_MPS_SIZE_192K16B;
                }
                params.param0 = DWC3_DEPCFG_EP_TYPE(usb_endpoint_type(desc))
                                | DWC3_DEPCFG_MAX_PACKET_SIZE(max_pkt_size);
        } else if(epnum == UAC2_OUT_EP_NUMBER) {
                // UAC OUT1
                if(UAC_OUT1_freq_type == _48K_16B) {
                        max_pkt_size = UAC_MPS_SIZE;
                } else if(UAC_OUT1_freq_type == _96K_16B) {
                        max_pkt_size = UAC_MPS_SIZE_96K16B;
                } else if(UAC_OUT1_freq_type == _192K_16B) {
                        max_pkt_size = UAC_MPS_SIZE_192K16B;
                }
                params.param0 = DWC3_DEPCFG_EP_TYPE(usb_endpoint_type(desc))
                                | DWC3_DEPCFG_MAX_PACKET_SIZE(max_pkt_size);
        }

        if (ignore)
                params.param0 |= DWC3_DEPCFG_IGN_SEQ_NUM;


        /*
         * We are doing 1:1 mapping for endpoints, meaning
         * Physical Endpoints 2 maps to Logical Endpoint 2 and
         * so on. We consider the direction bit as part of the physical
         * endpoint number. So USB endpoint 0x81 is 0x03.
         */
        params.param1 = DWC3_DEPCFG_EP_NUMBER((UINT32)epnum);

        if(epnum == UVC_EP_NUMBER) {
                params.param1 |= DWC3_DEPCFG_XFER_COMPLETE_EN;
        } else {
                params.param1 |= DWC3_DEPCFG_XFER_COMPLETE_EN
                                 | DWC3_DEPCFG_XFER_NOT_READY_EN;
        }

        //if( epnum!=2 && epnum!=3) {
        //    if (!usb_endpoint_xfer_control(desc))
        //            params.param1 |= DWC3_DEPCFG_XFER_IN_PROGRESS_EN;
        //}

        /*
         * We must use the lower 16 TX FIFOs even though
         * HW might have more
         */
        if (g_rtk_dwc3.eps[epnum].direction)
                params.param0 |= DWC3_DEPCFG_FIFO_NUMBER((UINT32)epnum >> 1);

        if (desc->bInterval) {
                UINT32 bInterval;
                //bInterval = DWC3_DEPCFG_BINTERVAL_M1(desc->bInterval - 1);
                bInterval = DWC3_DEPCFG_BINTERVAL_M1(4 - 1);
                params.param1 |= bInterval;
                //g_rtk_dwc3.eps[epnum].interval = (UINT32)(1 << (UINT32)(desc->bInterval - 1));
        }

        //pm_printk(LOGGER_ERROR, "rtk_dwc3_gadget_set_ep_config epnum=%d\n", (UINT32)epnum);
        //pm_printk(LOGGER_ERROR, "rtk_dwc3_gadget_set_ep_config params.param0=%d\n", (UINT32)params.param0);
        //pm_printk(LOGGER_ERROR, "rtk_dwc3_gadget_set_ep_config params.param1=%d\n", (UINT32)params.param1);
        return rtk_dwc3_send_gadget_ep_cmd(epnum, DWC3_DEPCMD_SETEPCONFIG);
}

static UINT8 rtk_dwc3_gadget_set_xfer_resource(UINT8 epnum)
{
        dwc3_memset((void *)&params, 0x00, sizeof(params));

        params.param0 = DWC3_DEPXFERCFG_NUM_XFER_RES(1);

        return rtk_dwc3_send_gadget_ep_cmd(epnum, DWC3_DEPCMD_SETTRANSFRESOURCE);
}

void rtk_dwc3_stop_active_transfer(UINT8 epnum)
{
        if (!g_rtk_dwc3.eps[epnum].resource_index)
                return;

        u32_uvc_temp = DWC3_DEPCMD_PARAM(g_rtk_dwc3.eps[epnum].resource_index);
        u32_uvc_temp |= DWC3_DEPCMD_ENDTRANSFER;
        u32_uvc_temp |= DWC3_DEPCMD_HIPRI_FORCERM;
        u32_uvc_temp |= DWC3_DEPCMD_CMDIOC;
        dwc3_memset((void *)&params, 0, sizeof(params));
        rtk_dwc3_send_gadget_ep_cmd(epnum, u32_uvc_temp);

        if(epnum ==  UVC_EP_NUMBER || epnum ==  0x6) {
                u32_uvc_temp = DWC3_DEPCMD_CLEARSTALL;
                u32_uvc_temp |= DWC3_DEPCMD_CLEARPENDIN;
                dwc3_memset((void *)&params, 0, sizeof(params));
                rtk_dwc3_send_gadget_ep_cmd(epnum, u32_uvc_temp);
        }

        g_rtk_dwc3.eps[epnum].resource_index = 0;
        g_rtk_dwc3.eps[epnum].flags &= ~DWC3_EP_BUSY;
        delayus(100);
}


UINT8 __rtk_dwc3_gadget_ep_disable(UINT8 epnum)
{
        dwc_printk(LOGGER_ERROR, "dwc3 ep disable ep=%d\n", (UINT32)epnum);

        rtk_dwc3_stop_active_transfer(epnum);

        /* make sure HW endpoint isn't stalled */
        //if (g_rtk_dwc3.eps[epnum].flags & DWC3_EP_STALL)
        //        __rtk_dwc3_gadget_ep_set_halt(epnum, 0, false);

        //reg = DWC3_DALEPENA_EP(g_rtk_dwc3.eps[epnum].number);
        //reg = ~reg;
        //reg = reg & rtd_inl(DWC3_DALEPENA);

        //rtd_outl(DWC3_DALEPENA, reg);

        g_rtk_dwc3.eps[epnum].desc = NULL;
        //g_rtk_dwc3.eps[epnum].comp_desc = NULL;
        g_rtk_dwc3.eps[epnum].type = 0;
        g_rtk_dwc3.eps[epnum].flags = 0;
        return 0;
}

#if 1
UINT8 __rtk_dwc3_gadget_ep_disable_uac(UINT8 epnum)
{
        //pm_printk(LOGGER_ERROR, "uac ep disable ep=%d\n", (UINT32)epnum);

        rtk_dwc3_stop_active_transfer(epnum);

        /* make sure HW endpoint isn't stalled */
        if (g_rtk_dwc3.eps[epnum].flags & DWC3_EP_STALL)
                __rtk_dwc3_gadget_ep_set_halt(epnum, 0, false);

        reg = DWC3_DALEPENA_EP(g_rtk_dwc3.eps[epnum].number);
        reg = ~reg;
        reg = reg & rtd_inl(DWC3_DALEPENA);

        rtd_outl(DWC3_DALEPENA, reg);

        g_rtk_dwc3.eps[epnum].desc = NULL;
        //g_rtk_dwc3.eps[epnum].comp_desc = NULL;
        g_rtk_dwc3.eps[epnum].type = 0;
        g_rtk_dwc3.eps[epnum].flags = 0;
        return 0;
}
#endif

UINT8 _rtk_dwc3_gadget_ep_enable(UINT8 epnum,
                                 const struct usb_endpoint_descriptor *desc,
                                 const struct usb_ss_ep_comp_descriptor *comp_desc,
                                 UINT8 ignore)
{
        dwc_printk(LOGGER_ERROR, "dwc3 ep enable ep=%d\n", (UINT32)epnum);
        if (!(g_rtk_dwc3.eps[epnum].flags & DWC3_EP_ENABLED)) {
                ret = rtk_dwc3_gadget_start_config(epnum);
                if (ret)
                        return ret;
        }

        ret = rtk_dwc3_gadget_set_ep_config(epnum, desc, comp_desc, ignore);
        if (ret)
                return ret;

        if (!(g_rtk_dwc3.eps[epnum].flags & DWC3_EP_ENABLED)) {
                ret = rtk_dwc3_gadget_set_xfer_resource(epnum);
                if (ret)
                        return ret;

                g_rtk_dwc3.eps[epnum].desc = desc;
                //g_rtk_dwc3.eps[epnum].comp_desc = &g_comp_desc;
                //if(epnum>1)
                //    g_rtk_dwc3.eps[epnum].comp_desc = &g_comp_desc;
                g_rtk_dwc3.eps[epnum].type = usb_endpoint_type(desc);
                g_rtk_dwc3.eps[epnum].flags |= DWC3_EP_ENABLED;

                reg = DWC3_DALEPENA_EP(g_rtk_dwc3.eps[epnum].number);
                reg |= rtd_inl(DWC3_DALEPENA);
                rtd_outl(DWC3_DALEPENA, reg);

                if (!usb_endpoint_xfer_isoc(desc))
                        return 0;

        }
        return 0;
}


static void rtk_dwc3_clear_stall_all_ep(void)
{
        for (epnum = 1; epnum < g_rtk_dwc3.num_eps; epnum++) {
                if (!(g_rtk_dwc3.eps[epnum].flags & DWC3_EP_STALL))
                        continue;

                g_rtk_dwc3.eps[epnum].flags &= ~DWC3_EP_STALL;

                dwc3_memset((void *)&params, 0, sizeof(params));
                rtk_dwc3_send_gadget_ep_cmd(epnum, DWC3_DEPCMD_CLEARSTALL);
        }
}

static void rtk_dwc3_stop_active_transfers(void)
{
        for (epnum = 2; epnum < g_rtk_dwc3.num_eps; epnum++) {
                if (!(g_rtk_dwc3.eps[epnum].flags & DWC3_EP_ENABLED))
                        continue;

                if(!(g_rtk_dwc3.eps[epnum].flags & DWC3_EP_BUSY))
                        continue;

                if (!g_rtk_dwc3.eps[epnum].resource_index)
                        continue;

                u32_uvc_temp = DWC3_DEPCMD_ENDTRANSFER;
                u32_uvc_temp |= DWC3_DEPCMD_HIPRI_FORCERM;
                u32_uvc_temp |= DWC3_DEPCMD_CMDIOC;
                u32_uvc_temp |= DWC3_DEPCMD_PARAM(g_rtk_dwc3.eps[epnum].resource_index);
                dwc3_memset((void *)&params, 0, sizeof(params));
                rtk_dwc3_send_gadget_ep_cmd(epnum, u32_uvc_temp);
                g_rtk_dwc3.eps[epnum].resource_index = 0;
                g_rtk_dwc3.eps[epnum].flags &= ~DWC3_EP_BUSY;
                delayus(100);
        }
}

UINT8 __rtk_dwc3_gadget_ep_set_halt(UINT8 epnum, int value, int protocol)
{
        if (usb_endpoint_xfer_isoc(g_rtk_dwc3.eps[epnum].desc)) {
                return -EINVAL;
        }

        dwc3_memset((void *)&params, 0x00, sizeof(params));

        if (value) {
                if (!protocol && (g_rtk_dwc3.eps[epnum].direction && g_rtk_dwc3.eps[epnum].flags & DWC3_EP_BUSY)) {
                        pm_printk(LOGGER_ERROR, "%d: pending request, cannot halt\n",
                                  g_rtk_dwc3.eps[epnum].number);
                        return -EAGAIN;
                }

                ret = rtk_dwc3_send_gadget_ep_cmd(epnum, DWC3_DEPCMD_SETSTALL);
                if (ret)
                        pm_printk(LOGGER_ERROR, "set STALL on %d NG\n",
                                  g_rtk_dwc3.eps[epnum].number);
                else
                        g_rtk_dwc3.eps[epnum].flags |= DWC3_EP_STALL;
        } else {
                if (!(g_rtk_dwc3.eps[epnum].flags & (DWC3_EP_STALL | DWC3_EP_WEDGE)))
                        return 0;

                ret = rtk_dwc3_send_gadget_ep_cmd(epnum, DWC3_DEPCMD_CLEARSTALL);
                if (ret)
                        pm_printk(LOGGER_ERROR, "clear STALL on %d NG\n",
                                  g_rtk_dwc3.eps[epnum].number);
                else
                        g_rtk_dwc3.eps[epnum].flags &= ~(DWC3_EP_STALL | DWC3_EP_WEDGE);
        }
        return ret;
}

void usb3_link_state_cnt_clear(void)
{
        g_dwc3_set_config_done = 0;
        enter_u0_count = 0;
        g_ss_inact_cnt = 0;
        g_ssdis_cnt = 0;
        g_rxdet_cnt = 0;
        g_polling_cnt = 0;
        g_gen2link_cnt = 0;
        g_relink_cnt = 0;
}

void usb3_link_state_monitor(UINT32 * event_ptr) //accumulate link state for error handle
{
        if(*event_ptr == 0x160301) { //recovery fail
                g_ss_inact_cnt++;
        }
        if(*event_ptr == 0x170301) { //polling
                g_polling_cnt++;
        }
        if(*event_ptr == 0x40301) { //link disable
                g_ssdis_cnt++;
        }
        if(*event_ptr == 0x150301) { //rx detect
                g_rxdet_cnt++;
        }
        if(*event_ptr == 0x100301) {
                if(enter_u0_count < DWC3_EVENT_MAX_NUM) {
                        enter_u0_count++;
                }
        }
        if(*event_ptr == 0x130301) { //U3 state clear cnt
                usb3_link_state_cnt_clear();
        }
}

void usb3_recovery_error_handle(void)
{
     if((g_ss_inact_cnt >= 1) && (g_rxdet_cnt >= 1) && (enter_u0_count >= 16)) {
             if(g_dwc3_set_config_done == 0) {
                     pm_printk(LOGGER_FATAL, "ss_inact error handle\n");
                     rtk_dwc3_gadget_run_stop(0);
                     delayms(30);
                     rtk_usb_driver_reinit();
             }
     }
    if((g_ss_inact_cnt >= 2) && (g_rxdet_cnt >= 1) && (g_polling_cnt >= 1)) {
             if(g_dwc3_set_config_done == 0) {
                     pm_printk(LOGGER_FATAL, "ss_inact error handle 2 times\n");
                     rtk_dwc3_gadget_run_stop(0);
                     delayms(30);
                     rtk_usb_driver_reinit();
             }
     }
}

void usb3_phy_dcc_en(UINT32 * event_ptr)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "rtk_usb_device ### usb3_phy_dcc_en need to implement ----------------\n");
}

void usb3_apply_phy_setting(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "rtk_usb_device ### usb3_apply_phy_setting need to implement ----------------\n");
}

UINT8 dwc3_usb3_error_handle(void)
{
        RTK_USB_DEVICE_DBG_SIMPLE( "rtk_usb_device ### dwc3_usb3_error_handle need to implement ----------------\n");
        return 0;
}

void usb3_phy_dump_dfe_ex(void)
{
        if(rtk_dwc3_usb2_0_only == 0)
                usb3_phy_dump_dfe();
}

void rtk_dwc3_process_device_event_link_status_change(void)
{
        if(g_rtk_dwc3.speed != USB_SPEED_UNKNOWN) {
                g_usb_link_state = dwc_temp1;
                core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_USB_POWER_STATUS_UPDATE, dwc_temp1);
        }
        if( (dwc_temp1 == 0) && (usb_uvc_connected == 1) )
            usb_link_state_count++;
}

void rtk_usb_driver_reinit_without_usb(void);

void rtk_check_usb_typecpd_ready(void)
{
    // Check if usb & typec-pd ready
    if((usb_sw_cfg_GetTypeC_Fuction() == _TYPEC_USB2_USB32_DP_1_4) ||
        (usb_sw_cfg_GetTypeC_Fuction() == _TYPEC_USB2_USB32))
    {
#ifdef CONFIG_ENABLE_TYPEC_REL
        usb_typecpd_ready_cnt++;
        if(usb_typecpd_ready_cnt == 2) {
            core_fw_msg_queue_cancelEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT);
            core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT, (UINT32)MSG_USB_CONNECT_SPEED_SSP);
            pm_printk(0, "[USB] USB & PD Ready\n");
            usb_typecpd_ready_cnt = 0;
        }
#endif
    }
    else {
        core_fw_msg_queue_cancelEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT);
        core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT, (UINT32)MSG_USB_CONNECT_SPEED_SSP);
    }
}

void show_dwc3_ltssm(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "rtk_usb_device ### show_dwc3_ltssm need to implement ----------------\n");
#if 0
    if(g_usb_handshake == 0) {
        // can not enter this mode when EP is running
        rtd_outl(0x18069000, 0x0000000c);
        rtd_outl(0x180690b0, 0xd050);
        rtd_outl(0x180690ac, 0x00000001);
        rtd_outl(0x18069000, 0x00000000);
        pm_printk(LOGGER_FATAL, "DWC3 LTSSM= %x", (UINT32)rtd_inl(0x180690b8));
    }
#endif
}

static void rtk_dwc3_gadget_interrupt(const struct dwc3_event_devt *event)
{
        UINT8 ret;
        UINT32 *ptr = (UINT32 *)event;
        switch (((UINT32) * (ptr) >> 8) & 0xF) {
                case DWC3_DEVICE_EVENT_DISCONNECT:
#ifdef CONFIG_USB_SUPPORT_U1U2
                        reg = rtd_inl(DWC3_DCTL);
                        reg &= ~DWC3_DCTL_INITU1ENA;
                        rtd_outl(DWC3_DCTL, reg);

                        reg &= ~DWC3_DCTL_INITU2ENA;
                        rtd_outl(DWC3_DCTL, reg);
#endif
                        g_rtk_dwc3.state = USB_STATE_NOTATTACHED;
                        //rtk_dwc3_gadget_disconnect_interrupt();
                        dwc_printk(0, "DWC3_EVENT_DISCONNECT\n");
                        break;
                case DWC3_DEVICE_EVENT_RESET:
                        //rtk_dwc3_gadget_reset_interrupt();
                        g_rtk_dwc3.state = USB_STATE_ATTACHED;

                        reg = rtd_inl(DWC3_DCTL);
                        reg &= ~DWC3_DCTL_TSTCTRL_MASK;
                        rtd_outl(DWC3_DCTL, reg);

                        rtk_dwc3_stop_active_transfers();
                        rtk_dwc3_clear_stall_all_ep();

                        __rtk_dwc3_gadget_ep_disable(UVC_EP_NUMBER);
                        __rtk_dwc3_gadget_ep_disable(HID_EP_NUMBER);

                        // Reset device address to zero
                        reg = rtd_inl(DWC3_DCFG);
                        reg &= ~(DWC3_DCFG_DEVADDR_MASK);
                        rtd_outl(DWC3_DCFG, reg);
                        dwc_printk(0, "DWC3_EVENT_RESET\n");
                        break;
                case DWC3_DEVICE_EVENT_CONNECT_DONE:
                        //rtk_dwc3_gadget_conndone_interrupt
                        dwc3_disable_uac_timer();
                        reg = rtd_inl(DWC3_DSTS);
                        dwc_printk(0, "DWC3_EVENT_CONNECT_DONE reg=%x\n", reg);
                        if(g_usb_handshake != 0) {
                            core_fw_msg_queue_cancelEvent(MSG_AP_ID, MSG_AP_USB_MAC_HANDSHAKE_STATUS);
                            core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_USB_MAC_HANDSHAKE_STATUS, (UINT32)1);
                            g_usb_handshake = 0;
                        }
                        g_usb_link_state = USB_LINK_STATE_U0;
                        core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_USB_POWER_STATUS_UPDATE, USB_LINK_STATE_U0);

                        compare_UsbSof_LocalCouner_reset();

                        switch ((reg & DWC3_DSTS_CONNECTSPD)) {
                                case DWC3_DSTS_SUPERSPEED_PLUS:
                                        pm_printk(0, "DWC3 SS+_10G\n");
                                        rtk_check_usb_typecpd_ready();
                                        dwc3_gadget_ep0_desc.wMaxPacketSize = cpu_to_le16(512);
                                        g_rtk_dwc3.maxpacket = 512;
                                        g_rtk_dwc3.speed = USB_SPEED_SUPER_PLUS;

                                        g_dev_descriptor.bcdUSB = 0x0320;
                                        //g_dev_descriptor.bDeviceClass = 0x00;
                                        g_dev_descriptor.bDeviceSubClass = 0x02;
                                        g_dev_descriptor.bMaxPacketSize0 = 0x09;

#ifndef CONFIG_USB_CDC_ONLY
                                        //g_config_descriptor[71] = 0x00;
                                        //g_config_descriptor[72] = 0x04;
                                        //g_config_descriptor[84] = 0x00;
                                        //g_config_descriptor[85] = 0x04;
#endif
                                        rtk_serial_hs_bulk_in_desc.wMaxPacketSize = cpu_to_le16(1024);
                                        rtk_serial_hs_bulk_out_desc.wMaxPacketSize = cpu_to_le16(1024);
                                        break;
                                case DWC3_DCFG_SUPERSPEED:
                                        pm_printk(0, "DWC3 SS_5G\n");

#if 0
                                        if(rtd_inl(0x18014604) == USB3_GEN1_FLAG) {
                                                g_gen2link_cnt++;
                                                if(g_gen2link_cnt <= 4) {
                                                        pm_printk(LOGGER_FATAL, "Gen2 error handle, %x time\n", (UINT32)g_gen2link_cnt);
                                                        rtk_dwc3_gadget_run_stop(0);
                                                        delayms(30);
                                                        rtk_usb_driver_reinit();
                                                        break;
                                                }
                                        }
#endif

                                        core_fw_msg_queue_cancelEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT);
                                        core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT, (UINT32)MSG_USB_CONNECT_SPEED_SS);
                                        dwc3_gadget_ep0_desc.wMaxPacketSize = cpu_to_le16(512);
                                        g_rtk_dwc3.maxpacket = 512;
                                        g_rtk_dwc3.speed = USB_SPEED_SUPER;

                                        g_dev_descriptor.bcdUSB = 0x0320;
                                        //g_dev_descriptor.bDeviceClass = 0x00;
                                        g_dev_descriptor.bDeviceSubClass = 0x02;
                                        g_dev_descriptor.bMaxPacketSize0 = 0x09;

#ifndef CONFIG_USB_CDC_ONLY
                                        //g_config_descriptor[71] = 0x00;
                                        //g_config_descriptor[72] = 0x04;
                                        //g_config_descriptor[84] = 0x00;
                                        //g_config_descriptor[85] = 0x04;
#endif
                                        rtk_serial_hs_bulk_in_desc.wMaxPacketSize = cpu_to_le16(1024);
                                        rtk_serial_hs_bulk_out_desc.wMaxPacketSize = cpu_to_le16(1024);
                                        break;
                                case DWC3_DCFG_HIGHSPEED:
                                        pm_printk(0, "DWC3 HS\n");
                                        //if(GetTypeC_Fuction() < _TYPEC_USB2_USB31_DP_1_4)
                                        //{
                                        //    rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_dp_combo_mask);
                                        //    rtd_outl(0xb806917c, 0x1); // U2 only mode
                                        //}
                                        core_fw_msg_queue_cancelEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT);
                                        core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT, (UINT32)MSG_USB_CONNECT_SPEED_HS);
                                        dwc3_gadget_ep0_desc.wMaxPacketSize = cpu_to_le16(64);
                                        g_rtk_dwc3.maxpacket = 64;
                                        g_rtk_dwc3.speed = USB_SPEED_HIGH;

                                        g_dev_descriptor.bcdUSB = 0x0200;
                                        g_dev_descriptor.bMaxPacketSize0 = 64;

                                        rtk_serial_hs_bulk_in_desc.wMaxPacketSize = cpu_to_le16(512);
                                        rtk_serial_hs_bulk_out_desc.wMaxPacketSize = cpu_to_le16(512);
                                        break;
                        }

                        usb3_phy_dump_dfe_ex();

                        if (g_rtk_dwc3.speed <= USB_SPEED_HIGH) {
                                reg = rtd_inl(DWC3_DCFG);
                                reg |= DWC3_DCFG_LPM_CAP;
                                rtd_outl(DWC3_DCFG, reg);

                                reg = rtd_inl(DWC3_DCTL);
                                reg &= ~(DWC3_DCTL_HIRD_THRES_MASK | DWC3_DCTL_L1_HIBER_EN);
                                reg |= DWC3_DCTL_HIRD_THRES(0xc);
                                reg |= DWC3_DCTL_LPM_ERRATA(0xFF);

                                rtd_outl(DWC3_DCTL, reg);
                        } else {
                                reg = rtd_inl(DWC3_DCTL);
                                reg &= ~DWC3_DCTL_HIRD_THRES_MASK;
                                rtd_outl(DWC3_DCTL, reg);
                        }


                        _rtk_dwc3_gadget_ep_enable((UINT8)0, &dwc3_gadget_ep0_desc, NULL, true);
                        _rtk_dwc3_gadget_ep_enable((UINT8)1, &dwc3_gadget_ep0_desc, NULL, true);
                        //rtk_dwc3_ep0_start_setup_phase
                        g_rtk_dwc3.ep0state = EP0_SETUP_PHASE;
                        rtk_dwc3_ep_start_trans(0, dvr_to_phys(g_rtk_dwc3.ep0_ctrl_req_addr), 8,
                                                DWC3_TRBCTL_CONTROL_SETUP);
                        //#endif

#ifndef CONFIG_ARCH_5281
                        core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_UVC_PLUG_IN, 0);
#endif
                        break;
                case DWC3_DEVICE_EVENT_LINK_STATUS_CHANGE:
                        show_dwc3_ltssm();
                        pm_printk(LOGGER_FATAL, " INK_STATUS_CHANGE=%x\n", (UINT32)*ptr);
                        dwc_temp1 = (UINT32) * ptr;
                        dwc_temp1 = (dwc_temp1 >> 16) & 0x0f;

                        rtk_dwc3_process_device_event_link_status_change();

                        if(dwc_temp1 == DWC3_LINK_STATE_RX_DET) {
                            if(usb_set_address == 0) {
                                rtk_usb_driver_reinit_without_usb();
                            } else {
                                pm_printk(LOGGER_FATAL, "Set add NG\n");
                                rtk_usb_driver_reinit();
                            }
                            usb_set_address = 0;
                            wait_do_control_transfer = 0;
                        }

#ifndef CONFIG_ARCH_5281
                        usb3_link_state_monitor(ptr);
                        usb3_phy_dcc_en(ptr);
                        ret = dwc3_usb3_error_handle();
#endif
                        break;
                case DWC3_DEVICE_EVENT_OVERFLOW:
                        pm_printk(LOGGER_FATAL, "DWC3_EVENT_OVERFLOW\n");
                        if(enter_u0_count > (DWC3_EVENT_MAX_NUM / 2)) {
                                rtk_usb_driver_reinit();
                        }
                        break;
                default:
                        //pm_printk(LOGGER_ERROR, "UNKNOWN IRQ %d\n", event->type);
                        break;
        }
}

void rtk_dwc3_process_bos_descriptor_macos_case(void)
{
#if 1
        if(dwc3_get_config_dt == 0 && (GetUSBForceConnectSpeed()!=3)) {
                pm_printk(0, "USB macos force in SS\n");
                isMacOS = 1;
                if(rtk_dwc3_usb3_0_5G_only != 1) {
                        rtk_dwc3_usb3_0_5G_only = 1;
                        rtk_usb_driver_reinit();
                        if(rtk_dwc3_is_usb_control_by_application() == TRUE) {
                            rtk_dwc3_gadget_run_stop(1);
                        }
                }
        }
#endif
}

void rtk_dwc3_process_bos_descriptor(struct usb_ctrlrequest *ctrl_req)
{
        if(rtk_dwc3_usb3_0_5G_only) {
                g_rtk_dwc3.ep0_setup_buf_len = ctrl_req->wLength < sizeof(g_bos_descriptor_u3_ss_only) ? ctrl_req->wLength : sizeof(g_bos_descriptor_u3_ss_only);
        } else {
                g_rtk_dwc3.ep0_setup_buf_len = ctrl_req->wLength < sizeof(g_bos_descriptor) ? ctrl_req->wLength : sizeof(g_bos_descriptor);
        }

        dwc_info.dst = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
        if(rtk_dwc3_usb3_0_5G_only) {
                dwc_info.src = &g_bos_descriptor_u3_ss_only;
        } else {
                dwc_info.src = &g_bos_descriptor;
        }
        dwc_info.len = g_rtk_dwc3.ep0_setup_buf_len;
        dwc3_copy_to_memory2();

        rtk_dwc3_process_bos_descriptor_macos_case();
}

static void rtk_dwc3_get_description_request(struct usb_ctrlrequest *ctrl_req)
{
        switch ((UINT16)ctrl_req->wValue >> 8) {
                //pm_printk(LOGGER_ERROR, "USB_REQ_GET_DESCRIPTOR (UINT16)ctrl_req->wValue >> 8=%x\n", (UINT16)ctrl_req->wValue >> 8);
                case USB_DT_DEVICE:
                        dwc_printk(0, "USB_DT_DEVICE\n");
                        g_rtk_dwc3.ep0_setup_buf_len = ctrl_req->wLength < 0x12 ? ctrl_req->wLength : 0x12;
                        rtk_dwc3_get_device(ctrl_req->wLength);
                        dwc3_get_config_dt = 0;
                        break;
                case USB_DT_OTHER_SPEED_CONFIG:
                        g_rtk_dwc3.ep0_setup_buf_len = ctrl_req->wLength < sizeof(g_other_speed_config_descriptor) ? ctrl_req->wLength : sizeof(g_other_speed_config_descriptor);
                        dwc_info.dst = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
                        dwc_info.src = &g_other_speed_config_descriptor;
                        dwc_info.len = g_rtk_dwc3.ep0_setup_buf_len;
                        dwc3_copy_to_memory2();
                        break;
                case USB_DT_CONFIG:
                        dwc_printk(0, "USB_DT_CONFIG\n");
                        rtk_dwc3_get_config_length_by_speed(ctrl_req->wLength);
                        rtk_dwc3_get_config_data_by_speed();
                        dwc3_get_config_dt = 1;
                        break;
                case USB_DT_STRING:
                        dwc_printk(0, "USB_DT_STRING\n");
#ifdef CONFIG_ARCH_5281
#else
                        dwc_temp1 = (ctrl_req->wValue & 0xff);
                        if(dwc_temp1 > USB_DT_STRING_INDEX_COUNT)
                                dwc_temp1 = USB_DT_STRING_INDEX_COUNT;
                        rtk_dwc3_get_string(ctrl_req->wLength);
#endif
                        break;
                case USB_DT_BOS:
                        rtk_dwc3_process_bos_descriptor(ctrl_req);
                        break;
                case USB_DT_DEVICE_QUALIFIER:
                        dwc_printk(0, "USB_DT_DEVICE_QUALIFIER\n");
                        g_rtk_dwc3.ep0_setup_buf_len = ctrl_req->wLength < 0xA ? ctrl_req->wLength : 0xA;
#ifdef CONFIG_ARCH_5281
#else
                        dwc_info.dst = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
                        dwc_info.src = &g_qual_descriptor;
                        dwc_info.len = g_rtk_dwc3.ep0_setup_buf_len;
                        dwc3_copy_to_memory2();
#endif
                        break;
                case USB_DT_REPORT_DESCRIPTOR_TYPE:
                        dwc_printk(0, "USB_DT_REPORT_DESCRIPTOR_TYPE\n");
                        rtk_dwc3_get_hid_report_descriptor();
                        break;
                case USB_DT_INTERFACE:
                case USB_DT_ENDPOINT:
                case USB_DT_INTERFACE_POWER:
                default:
                        pm_printk(LOGGER_ERROR, "Unknow descriptor %x\n", (UINT32)ctrl_req->wValue);
                        ret = -EOPNOTSUPP;
                        break;
        }
}


void rtk_dwc3_standard_request(struct usb_ctrlrequest *ctrl_req)
{
        switch (ctrl_req->bRequest) {
                case USB_REQ_GET_DESCRIPTOR:
                        rtk_dwc3_get_description_request(ctrl_req);
                        break;
                case USB_REQ_SET_INTERFACE:
                        ret = rtk_dwc3_set_alt();
                        break;
                case USB_REQ_SET_CONFIGURATION:
                        ret = rtk_dwc3_ep0_set_config();
                        break;
                case USB_REQ_GET_CONFIGURATION:
                        //pm_printk(0, "SLL USB_REQ_GET_CONFIGURATION\n");
                        g_rtk_dwc3.ep0_setup_buf_len = 1;
                        if(g_rtk_dwc3.state != USB_STATE_CONFIGURED)
                                g_recv_buf[0] = 0x0;
                        else
                                g_recv_buf[0] = 0x1;
                        dwc_info.dst = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
                        dwc_info.src = g_recv_buf;
                        dwc_info.len = g_rtk_dwc3.ep0_setup_buf_len;
                        dwc3_copy_to_memory2();
                        ret = 0;
                        break;
                case USB_REQ_GET_INTERFACE:
                case USB_REQ_SET_DESCRIPTOR:
                default:
                        pm_printk(LOGGER_ERROR, "Unknown bRequest: %x\n", (UINT32)ctrl_req->bRequest);
                        ret = -EOPNOTSUPP;
        }
}

UINT8 rtk_dwc3_ep_start_trans_uvc(UINT8 epnum, UINT32 buf_dma              )
{
#ifdef CONFIG_USB_SUPPORT_U1U2
        g_is_uvc_uac_start = 1;
        reg = rtd_inl(DWC3_DCTL);
        reg &= (~(DWC3_DCTL_ACCEPTU1ENA | DWC3_DCTL_ACCEPTU2ENA | DWC3_DCTL_INITU1ENA | DWC3_DCTL_INITU2ENA));
        rtd_outl(DWC3_DCTL, reg);
#endif
        if (g_rtk_dwc3.eps[epnum].flags & DWC3_EP_BUSY) {
                //pm_printk(LOGGER_ERROR, "[ERROR] DWC3_EP_BUSY (epnum=%x)\n", (UINT32)epnum);
                return -2;
        }

        //dwc_printk( 0, "rtk_dwc3_ep_start_trans_uvc %d\n", (UINT32)__LINE__);

        if(epnum <= 1) {
                trb = (struct dwc3_trb *)&g_rtk_dwc3.ep0_trbs[epnum];
                reg = g_rtk_dwc3.ep0_trbs_addr + (UINT32)(sizeof(struct dwc3_trb) * epnum);
        } else {
                trb = (struct dwc3_trb *)&g_rtk_dwc3.eps[epnum].trbs[0];
                reg = uvc_address_trb_buffer;
        }

        //dwc_printk( 0, "rtk_dwc3_ep_start_trans_uvc %d  reg=%x\n", (UINT32)__LINE__, (UINT32)reg);

#if 1
        //trb->bpl = UVC_DATA_BUFFER_ADDR;
        //trb->bph = 0;
        //trb->size = 4096 - 12;
        //trb->ctrl = DWC3_TRBCTL_NORMAL;
        //trb->ctrl = 0;
        //dwc3_copy_to_memory(reg, trb, sizeof(struct dwc3_trb));
        //dwc_printk( 0, "rtk_dwc3_ep_start_trans_uvc %d\n", (UINT32)__LINE__);
        //rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x0, UVC_DATA_BUFFER_ADDR);
        //rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x4, 0x00000000);
        //rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x8, 4096 - 12);
        //rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0xc, 0x00000000);

        for (i = 0; i < (16 * (UINT16)3); i = i + 4) {
                rtd_mem_outl( uvc_header_vir[0] + i, 0x00000000);
        }

        for (i = 0; i < (16 * (UINT16)trb_cnt); i = i + 4) {
                rtd_mem_outl( uvc_trb_vir[0] + i, 0x00000000);
        }

        for (i = 0; i < trb_cnt; i++) {
                rtd_mem_outl( uvc_trb_vir[0] + 0x10 * i + 0x0, uvc_frame_phy[0]);
        }

        rtd_mem_outl( uvc_trb_vir[0] + 0x10 * trb_cnt + 0x0, uvc_trb_phy[0]);
        rtd_mem_outl( uvc_trb_vir[0] + 0x10 * trb_cnt + 0x4, 0x00000000);
        rtd_mem_outl( uvc_trb_vir[0] + 0x10 * trb_cnt + 0x8, 0x00000000);
        rtd_mem_outl( uvc_trb_vir[0] + 0x10 * trb_cnt + 0xc, DWC3_TRBCTL_LINK_TRB | DWC3_TRB_CTRL_HWO);

#else
        rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x0, UVC_DATA_BUFFER_ADDR + 0x1000 * 0);
        rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x4, 0x00000000);
        rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x8, 4096 - 12);
        rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0xc, (DWC3_TRB_CTRL_HWO | DWC3_TRB_CTRL_ISP_IMI | DWC3_TRB_CTRL_IOC));

        rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x10, UVC_DATA_BUFFER_ADDR + 0x1000 * 1);
        rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x14, 0x00000000);
        rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x18, 4096 - 12);
        rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x1c, (DWC3_TRB_CTRL_HWO | DWC3_TRB_CTRL_ISP_IMI | DWC3_TRB_CTRL_IOC));

        rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x20, UVC_DATA_BUFFER_ADDR + 0x1000 * 2);
        rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x24, 0x00000000);
        rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x28, 4096 - 12);
        rtd_mem_outl(UVC_TRB_BUFFER_ADDR + 0x2c, (DWC3_TRB_CTRL_HWO | DWC3_TRB_CTRL_ISP_IMI | DWC3_TRB_CTRL_IOC));
#endif

#if 0
        {
                UINT32 temp_val;
                UINT32 i;
                for (i = 0; i < len; i = i + 0x4) {
                        temp_val = (UINT32)rtd_mem_inl(g_rtk_dwc3.ep0_trbs_addr + (UINT32)(sizeof(struct dwc3_trb) * epnum));
                        dwc_printk( 0, "(epnum=%x) ep0_trbs_addr0=%x, val=%x\n",
                                    (UINT32)epnum, (UINT32)(g_rtk_dwc3.ep0_trbs_addr + (UINT32)(sizeof(struct dwc3_trb)*epnum)), (UINT32)temp_val);
                }
        }
#endif

        dwc3_memset((void *)&params, 0, sizeof(params));

        if(epnum <= 1) {
                params.param1 = dvr_to_phys(g_rtk_dwc3.ep0_trbs_addr + (UINT32)sizeof(struct dwc3_trb) * epnum);
        } else {
                params.param1 = uvc_trb_phy[0];
        }

        ret = rtk_dwc3_send_gadget_ep_cmd(epnum, DWC3_DEPCMD_CMDIOC | DWC3_DEPCMD_STARTTRANSFER);
        if (ret < 0) {
                return ret;
        }

        g_rtk_dwc3.eps[epnum].flags |= DWC3_EP_BUSY;
        g_rtk_dwc3.eps[epnum].resource_index = rtk_dwc3_gadget_ep_get_transfer_index(epnum);
        dwc_printk( 0, "dwc3 ep start trans_uvc %d  resource_index=0x%x\n", (UINT32)__LINE__, (UINT32)g_rtk_dwc3.eps[epnum].resource_index);

        if(epnum <= 1) {
                g_rtk_dwc3.ep0_next_event = DWC3_EP0_COMPLETE;
        } else {
                g_rtk_dwc3.eps[epnum].transfer_size = 4096 - 12;
        }

        //dwc_printk( 0, "rtk_dwc3_ep_start_trans_uvc %d\n", (UINT32)__LINE__);

        return 0;
}

UINT8 uvc_already_run = 0;
void rtk_dwc3_class_request(struct usb_ctrlrequest *ctrl_req)
{
        if(only_support_cdc == 1) {
                ctrl_req->wIndex =  ctrl_req->wIndex + ((UINT16)cdc_control_interface  - H5_SERIAL_CONTROL_INTERFACE_NUM);
        }

        if ((ctrl_req->wIndex & 0xf) == UVC_CONTROL_INTERFACE_NUM) {
                rtk_uvc_control_interface(ctrl_req);
        } else if ((ctrl_req->wIndex & 0xf) == UVC_STREAM_INTERFACE_NUM) {
                if(ctrl_req->wValue == 0x0200) {
                        __rtk_dwc3_gadget_ep_disable(UVC_EP_NUMBER);
                        //_rtk_dwc3_gadget_ep_enable(UVC_EP_NUMBER, &uvc_hs_streaming_ep, NULL, false);
                        rtk_dwc3_ep_start_trans_uvc(UVC_EP_NUMBER, uvc_address_trb_buffer);
                }
                rtk_uvc_stream_interface(ctrl_req);
        } else if ((ctrl_req->wIndex & 0xf) == UAC_CONTROL_INTERFACE_NUM) {
                rtk_uac_control_interface(ctrl_req);
        } else if ((ctrl_req->wIndex) == UAC_STREAM_OUT_INTERFACE_ENDPOINTADDRESS
                   || (ctrl_req->wIndex) == UAC_STREAM_IN0_INTERFACE_ENDPOINTADDRESS
                   || (ctrl_req->wIndex) == UAC_STREAM_IN1_INTERFACE_ENDPOINTADDRESS
                  ) {
                rtk_uac_stream_interface(ctrl_req);
        } else if ((ctrl_req->wIndex & 0xf) == hid_interface) {
                rtk_hid_control_interface();
        } else if ((ctrl_req->wIndex & 0xf) == cdc_control_interface) {
                rtk_cdc_control_interface(ctrl_req);
        }
}

void show_control_transfer_info(void)
{
        pm_printk( LOGGER_FATAL, "USB %02x %02x %04x %04x %04x\n", g_rtk_dwc3.ep0_ctrl_req->bRequestType, g_rtk_dwc3.ep0_ctrl_req->bRequest, g_rtk_dwc3.ep0_ctrl_req->wValue, g_rtk_dwc3.ep0_ctrl_req->wIndex, g_rtk_dwc3.ep0_ctrl_req->wLength);
}

static UINT8 rtk_dwc3_ep0_delegate_req(void)
{
        struct usb_ctrlrequest *ctrl_req = g_rtk_dwc3.ep0_ctrl_req;
        //pm_printk(LOGGER_ERROR, "11 %s: speed(%d), type(%x) req(%x) val(%x) index(%x), length(%x), ret(%x)\n", __func__,
        //               (UINT32)g_rtk_dwc3.speed, (UINT32)ctrl_req->bRequestType, (UINT32)ctrl_req->bRequest,
        //               (UINT32)ctrl_req->wValue, (UINT32)ctrl_req->wIndex,(UINT32)ctrl_req->wLength, ret);
        //show_control_transfer_info();
        if ((ctrl_req->bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
                rtk_dwc3_standard_request(ctrl_req);
        } else if ((ctrl_req->bRequestType & USB_TYPE_MASK) == USB_TYPE_CLASS) {
                rtk_dwc3_class_request(ctrl_req);
        }

        return ret;
}

UINT8 rtk_dwc3_ep_start_trans(UINT8 epnum, UINT32 buf_dma,
                              UINT16 len, UINT8 type)
{
        //int flags;
        UINT8 *pu8;

        if (g_rtk_dwc3.eps[epnum].flags & DWC3_EP_BUSY) {
                //pm_printk(LOGGER_ERROR, "[ERROR] DWC3_EP_BUSY (epnum=%x)\n", (UINT32)epnum);
                return -2;
        }

        if( (epnum == 0x1) && (buf_dma == g_rtk_dwc3.ep0_ctrl_req_addr) ) {
                // check
                //dwc3_memset((UINT8 *)&g_ep0_ctrl_req, 0, sizeof(struct usb_ctrlrequest));
#ifndef CONFIG_ARCH_5281
                dwc_info.dst = (void*)(buf_dma + usb_phy_vir_offset);
                dwc_info.src = &g_ep0_ctrl_req;
                dwc_info.len = sizeof(struct usb_ctrlrequest);
                dwc3_copy_to_memory2();

                pu8 = (UINT8*)(buf_dma + usb_phy_vir_offset);
#endif
#if 0
                {
                        UINT32 temp_val;
                        UINT32 i;
                        for (i = 0; i < len; i = i + 0x4) {
                                temp_val = (UINT32)rtd_mem_inl(buf_dma + i);
                                dwc_printk( 0, "ep0_ctrl_req_addr=%x, val=%x\n",
                                            (UINT32)(buf_dma + i), (UINT32)temp_val);
                        }
                }
#endif
        }

        if(epnum <= 1) {
                trb = (struct dwc3_trb *)&g_rtk_dwc3.ep0_trbs[epnum];
                reg = g_rtk_dwc3.ep0_trbs_addr + (UINT32)(sizeof(struct dwc3_trb) * epnum);
        } else {
                if(epnum == CDC_BULK_OUT) {
                        g_rtk_dwc3.eps[epnum].trbs = (struct dwc3_trb *)&cdc_out_trb;
                        trb = (struct dwc3_trb *)&g_rtk_dwc3.eps[epnum].trbs[0];
                        reg = g_rtk_dwc3.eps_addr + (sizeof(struct dwc3_trb) * epnum);
                } else if(epnum == CDC_BULK_IN) {
                        g_rtk_dwc3.eps[epnum].trbs = (struct dwc3_trb *)&cdc_in_trb;
                        trb = (struct dwc3_trb *)&g_rtk_dwc3.eps[epnum].trbs[0];
                        reg = g_rtk_dwc3.eps_addr + (sizeof(struct dwc3_trb) * epnum);
                }
                g_rtk_dwc3.eps[epnum].trbs_addr = reg;
        }

        trb->bpl = buf_dma;
        trb->bph = 0;
        trb->size = len;
        trb->ctrl = type;
        trb->ctrl |= (DWC3_TRB_CTRL_HWO | DWC3_TRB_CTRL_ISP_IMI | DWC3_TRB_CTRL_IOC | DWC3_TRB_CTRL_LST);
        dwc_info.dst = (void*)(reg + usb_phy_vir_offset);
        dwc_info.src = trb;
        dwc_info.len = sizeof(struct dwc3_trb);
        dwc3_copy_to_memory2();
#if 0
        {
                UINT32 temp_val;
                UINT32 i;
                for (i = 0; i < len; i = i + 0x4) {
                        temp_val = (UINT32)rtd_mem_inl(g_rtk_dwc3.ep0_trbs_addr + (UINT32)(sizeof(struct dwc3_trb) * epnum));
                        dwc_printk( 0, "(epnum=%x) ep0_trbs_addr0=%x, val=%x\n",
                                    (UINT32)epnum, (UINT32)(g_rtk_dwc3.ep0_trbs_addr + (UINT32)(sizeof(struct dwc3_trb)*epnum)), (UINT32)temp_val);
                }
        }
#endif

        dwc3_memset((void *)&params, 0, sizeof(params));

        if(epnum <= 1) {
                params.param1 = dvr_to_phys(g_rtk_dwc3.ep0_trbs_addr + (UINT32)sizeof(struct dwc3_trb) * epnum);
        } else {
                params.param1 = dvr_to_phys(g_rtk_dwc3.eps_addr + (UINT32)sizeof(struct dwc3_trb) * epnum);
        }

        //dwc3_disable_interrupt(flags);

        ret = rtk_dwc3_send_gadget_ep_cmd(epnum, DWC3_DEPCMD_CMDIOC | DWC3_DEPCMD_STARTTRANSFER);
        if (ret < 0) {
                //dwc3_enable_interrupt(flags);
                return ret;
        }
        g_rtk_dwc3.eps[epnum].transfer_size = len;
        g_rtk_dwc3.eps[epnum].flags |= DWC3_EP_BUSY;
        g_rtk_dwc3.eps[epnum].resource_index = rtk_dwc3_gadget_ep_get_transfer_index(epnum);
        if(epnum <= 1)
                g_rtk_dwc3.ep0_next_event = DWC3_EP0_COMPLETE;

        //dwc3_enable_interrupt(flags);
        return 0;
}

void usb_uac_gen_sof_number(void)
{
        u32_rtk_temp_stamp = (UINT32)u32_rtk_time_stamp >> 16;
        u32_rtk_temp_stamp = u32_rtk_temp_stamp & 0x0000C000;

        u32_uvc_temp_3 = rtd_inl(DWC3_DSTS);
        u32_uvc_temp_3 = ((UINT32)u32_uvc_temp_3 >> 3) & 0x3fff;
        u32_uvc_temp_3 = u32_uvc_temp_3 + u32_rtk_temp_stamp;
        u32_uvc_temp_3 = u32_uvc_temp_3 + 23;

        // check wrap around
        dwc_temp1 = (UINT32)u32_rtk_time_stamp >> 16;
        dwc_temp1 = dwc_temp1 & 0x3fff;
        if(dwc_temp1 > (u32_uvc_temp_3 & 0x3fff)) {
                pm_printk(LOGGER_ERROR, "wrap_around\n");
                u32_uvc_temp_3 = u32_uvc_temp_3 + 0x4000;
        }

        pm_printk(LOGGER_ERROR, "start_trans t1=%x t2=%x, t3=%x\n", (UINT32)u32_rtk_time_stamp, (UINT32)u32_rtk_temp_stamp, (UINT32)u32_uvc_temp_3);
}

UINT8 rtk_dwc3_ep_start_trans_uac(UINT8 epnum, UINT32 buf_dma)
{
        UINT8 ret;
        if((rtd_inl(SYS_REG_SYS_CLKEN1_reg) & _BIT16) == 0)
            return 0;

#ifdef CONFIG_USB_SUPPORT_U1U2
        g_is_uvc_uac_start = 1;
        reg = rtd_inl(DWC3_DCTL);
        reg &= (~(DWC3_DCTL_ACCEPTU1ENA | DWC3_DCTL_ACCEPTU2ENA | DWC3_DCTL_INITU1ENA | DWC3_DCTL_INITU2ENA));
        rtd_outl(DWC3_DCTL, reg);
#endif
        if (g_rtk_dwc3.eps[epnum].flags & DWC3_EP_BUSY) {
                //pm_printk(LOGGER_ERROR, "[ERROR] DWC3_EP_BUSY (epnum=%x)\n", (UINT32)epnum);
                return -2;
        }
        dwc3_memset((void *)&params, 0, sizeof(params));

        if(epnum == UAC1_IN_EP_NUMBER)
                params.param1 = uac_host_to_device_trb_addr;
        if(epnum == UAC1_OUT_EP_NUMBER)
                //params.param1 = uac_device_to_host_trb_addr - 0x40;
                params.param1 = uac_device_to_host_trb_addr - ((uac_device_to_host_trb_cnt / 2) * 0x10); //test
        if(epnum == UAC2_OUT_EP_NUMBER)
                //params.param1 = uac_device_to_host_trb_addr - 0x40;
                params.param1 = uac_device_to_host_trb_addr_2 - ((uac_device_to_host_trb_cnt_2 / 2) * 0x10); //test

        usb_uac_gen_sof_number();
        u32_uvc_temp_2 = 0;
        while(1) {
                ret = rtk_dwc3_send_gadget_ep_cmd(epnum, (((UINT32)(u32_uvc_temp_3) << 16) & 0xfff80000) | DWC3_DEPCMD_CMDIOC | DWC3_DEPCMD_STARTTRANSFER);
                if (ret != 0) {
                        u32_uvc_temp_3 = rtd_inl(DWC3_DSTS);
                        u32_uvc_temp_3 = ((UINT32)u32_uvc_temp_3 >> 3) & 0x3fff;
                        u32_uvc_temp_3 = u32_uvc_temp_3 + u32_rtk_temp_stamp;
                        u32_uvc_temp_3 = u32_uvc_temp_3 + 23;
                        if(dwc_temp1 > (u32_uvc_temp_3 & 0x3fff)) {
                                u32_uvc_temp_3 = u32_uvc_temp_3 + 0x4000;
                        }
                        u32_uvc_temp_2++;
                        if(u32_uvc_temp_2 > 10) {
                            return -1;
                        }
                } else {
                        break;
                }
        }
        g_rtk_dwc3.eps[epnum].flags |= DWC3_EP_BUSY;
        g_rtk_dwc3.eps[epnum].resource_index = rtk_dwc3_gadget_ep_get_transfer_index(epnum);

        pm_printk(LOGGER_ERROR, "start_trans send cmd retry %d resource=%x\n", (UINT32)u32_uvc_temp_2, (UINT32)g_rtk_dwc3.eps[epnum].resource_index);
        return 0;
}

static UINT8 rtk_dwc3_set_config(UINT16 cfg)
{
        pm_printk( LOGGER_FATAL, "usb setconfig=%u\n", (UINT32)cfg);
        g_dwc3_set_config_done = 1;
        if(cfg != 0) {
#ifndef CONFIG_ARCH_5281
                //usb_uvc_connect(g_rtk_dwc3.eps[UVC_EP_NUMBER].number);
                //core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_UVC_CONNECT, 0);
                _rtk_dwc3_gadget_ep_enable(UVC_EP_NUMBER, &uvc_hs_streaming_ep, NULL, false);

                // UAC IN (Host -> Device)
                if(usb_sw_cfg_GetUAC_DownStream_Enable_BANK41() == _FUNCTION_ENABLE) {
                        __rtk_dwc3_gadget_ep_disable(UAC1_IN_EP_NUMBER);
                        _rtk_dwc3_gadget_ep_enable(UAC1_IN_EP_NUMBER, &uac_hs_epout_desc, NULL, false);
                }

                // UAC OUT2 (Device -> Host)
                if(usb_sw_cfg_GetUAC_UpStream_UAC2_Enable_BANK41() == _FUNCTION_ENABLE) {
                        __rtk_dwc3_gadget_ep_disable(UAC1_OUT_EP_NUMBER);
                        _rtk_dwc3_gadget_ep_enable(UAC1_OUT_EP_NUMBER, &uac_hs_epin0_desc, NULL, false);
                }

                // UAC OUT1 (Device -> Host)
                if(usb_sw_cfg_GetUAC_UpStream_UAC1_Enable_BANK41() == _FUNCTION_ENABLE) {
                        __rtk_dwc3_gadget_ep_disable(UAC2_OUT_EP_NUMBER);
                        _rtk_dwc3_gadget_ep_enable(UAC2_OUT_EP_NUMBER, &uac_hs_epin1_desc, NULL, false);
                }

                //__rtk_dwc3_gadget_ep_disable(UVC_EP_NUMBER);
                //rtk_dwc3_ep_start_trans_uvc(UVC_EP_NUMBER, UVC_TRB_BUFFER_ADDR);
#endif
                // HID
                _rtk_dwc3_gadget_ep_enable(HID_EP_NUMBER, &hid_hs_streaming_ep, NULL, false);

                g_direction = !!(rtk_serial_hs_bulk_in_desc.bEndpointAddress & 0x80);
                g_number = ((rtk_serial_hs_bulk_in_desc.bEndpointAddress & 0x1f) << 1) | g_direction;

                __rtk_dwc3_gadget_ep_disable(g_number);
                g_usb_uart.bulk_in = &g_rtk_dwc3.eps[g_number];
                g_usb_uart.bulk_in->desc = &rtk_serial_hs_bulk_in_desc;
                _rtk_dwc3_gadget_ep_enable(g_number, g_usb_uart.bulk_in->desc,
                                           NULL, false);

                g_direction = !!(rtk_serial_hs_bulk_out_desc.bEndpointAddress & 0x80);
                g_number = ((rtk_serial_hs_bulk_out_desc.bEndpointAddress & 0x1f) << 1) | g_direction;
                __rtk_dwc3_gadget_ep_disable(g_number);
                g_usb_uart.bulk_out = &g_rtk_dwc3.eps[g_number];
                g_usb_uart.bulk_out->desc = &rtk_serial_hs_bulk_out_desc;
                _rtk_dwc3_gadget_ep_enable(g_number, g_usb_uart.bulk_out->desc,
                                           NULL, false);

#ifdef CONFIG_ARCH_5281
                g_direction = !!(rtk_serial_hs_int_in_desc.bEndpointAddress & 0x80);
                g_number = ((rtk_serial_hs_int_in_desc.bEndpointAddress & 0x1f) << 1) | g_direction;
                __rtk_dwc3_gadget_ep_disable(g_number);
                g_usb_uart.int_in = &g_rtk_dwc3.eps[g_number];
                g_usb_uart.int_in->desc = &rtk_serial_hs_int_in_desc;
                _rtk_dwc3_gadget_ep_enable(g_number, g_usb_uart.int_in->desc,
                                           NULL, false);
#endif
        }

        if(g_usb_handshake != 1) {
            core_fw_msg_queue_cancelEvent(MSG_AP_ID, MSG_AP_USB_MAC_HANDSHAKE_STATUS);
            core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_USB_MAC_HANDSHAKE_STATUS, (UINT32)0);
            g_usb_handshake = 1;
        }
        pm_printk(0, "USB HANDSHAKE OK\n");
        return 0;
}

UINT8 rtk_dwc3_ep0_set_config(void)
{
        g_rtk_dwc3.start_config_issued = false;

        switch (g_rtk_dwc3.state) {
                case USB_STATE_DEFAULT:
                        return -EINVAL;

                case USB_STATE_ADDRESS:
                        if(g_rtk_dwc3.ep0_ctrl_req->wValue > 0x1)
                                return -EINVAL;
                        ret = rtk_dwc3_set_config(g_rtk_dwc3.ep0_ctrl_req->wValue);
                        /* if the cfg matches and the cfg is non zero */
                        if (g_rtk_dwc3.ep0_ctrl_req->wValue && (!ret || (ret == USB_GADGET_DELAYED_STATUS))) {
                                /*
                                 * only change state if set_config has already
                                 * been processed. If gadget driver returns
                                 * USB_GADGET_DELAYED_STATUS, we will wait
                                 * to change the state on the next usb_ep_queue()
                                 */
                                if (ret == 0)
                                        g_rtk_dwc3.state = USB_STATE_CONFIGURED;
                                /*
                                 * Enable transition to U1/U2 state when
                                 * nothing is pending from application.
                                 */
#ifdef CONFIG_USB_SUPPORT_U1U2
                                reg = rtd_inl(DWC3_DCTL);
                                reg |= (DWC3_DCTL_ACCEPTU1ENA | DWC3_DCTL_ACCEPTU2ENA);
                                rtd_outl(DWC3_DCTL, reg);
#endif
#if 0
                                g_rtk_dwc3.resize_fifos = true;
#endif
                        }
                        break;

                case USB_STATE_CONFIGURED:
                        if(g_rtk_dwc3.ep0_ctrl_req->wValue > 0x1)
                                return -EINVAL;
                        ret = rtk_dwc3_set_config(g_rtk_dwc3.ep0_ctrl_req->wValue);
                        if (!g_rtk_dwc3.ep0_ctrl_req->wValue && !ret)
                                g_rtk_dwc3.state = USB_STATE_ADDRESS;
                        break;
                default:
                        ret = -EINVAL;
        }
        return ret;
}

UINT8 rtk_dwc3_get_alt(void)
{
        if(g_rtk_dwc3.ep0_ctrl_req->wIndex == (UINT8)uac_in_number) {
                g_recv_buf[0] = uac_stream_out_inf_alt;
        } else if(g_rtk_dwc3.ep0_ctrl_req->wIndex == (UINT8)uac_out1_number) {
                g_recv_buf[0] = uac_stream_in0_inf_alt;
        } else if(g_rtk_dwc3.ep0_ctrl_req->wIndex == (UINT8)uac_out2_number) {
                g_recv_buf[0] = uac_stream_in1_inf_alt;
        } else {
                g_recv_buf[0] = 0x0;
        }
        g_rtk_dwc3.ep0_setup_buf_len = 1;
        dwc_info.dst = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
        dwc_info.src = g_recv_buf;
        dwc_info.len = g_rtk_dwc3.ep0_setup_buf_len;
        dwc3_copy_to_memory2();
        return 0;
}

void rtk_dwc3_clear_uac_buffer(void)
{
    for(uac_temp1 = uac_temp2; uac_temp1 < (UINT32)(uac_temp2 + 0x1b00); uac_temp1 = uac_temp1 + 4) {
        rtd_mem_outl((UINT8*)(unsigned long long)uac_temp1, 0x00000000);
    }
}

void usb_process_uac_out_alt_0(void)
{
        RTK_USB_DEVICE_DBG_SIMPLE( "rtk_usb_device ### usb_process_uac_out_alt_0 need to implement ----------------\n");
}

void usb_process_uac_out_alt_1(void)
{
        RTK_USB_DEVICE_DBG_SIMPLE( "rtk_usb_device ### usb_process_uac_out_alt_1 need to implement ----------------\n");
}

void usb_process_uac_in0_alt_0(void)
{
        RTK_USB_DEVICE_DBG_SIMPLE(  "%s %d\n", __func__, __LINE__);
        uac_stream_in0_inf_alt = 0;
        run_uac = 0;
        if((rtd_inl(SYS_REG_SYS_CLKEN1_reg) & _BIT16) == _BIT16) {
                //uac_stream_in0_inf_alt  = 0;
                usb_uac_in_disconnect(_UAC_OUT0);
                rtk_dwc3_stop_active_transfer(7);
//                core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_HOST_CDC_POWER_OFF, 0);
                //usb_uac_out0_sram_init(UAC_OUT0_freq_type);
        }
}

void usb_process_uac_in0_alt_1(void)
{
        // Speedup for MACOS
        if( /*g_hdmi_audio_stable == 1 && */ isMacOS == 1) {
            dwc3_uac_out_enable_timeout = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
            dwc3_uac_out_enable_timeout = dwc3_uac_out_enable_timeout + ((UINT32)100 * (UINT32)MS);
            if( dwc3_uac_out_enable_timeout > (0xffffffff - 5 * MS) )
                    dwc3_uac_out_enable_timeout = (0xffffffff - 5 * MS);
        }

        if((rtd_inl(SYS_REG_SYS_CLKEN1_reg) & _BIT16) == _BIT16) {
            uac_stream_in0_inf_alt  = 1;
            //usb_uac_in_connect(_UAC_OUT0);
            //_rtk_dwc3_gadget_ep_enable(7, &uac_hs_epin0_desc, NULL, false);
            //rtk_dwc3_ep_start_trans_uac(UAC1_OUT_EP_NUMBER, 0);
#ifdef CONFIG_ENABLE_SW_AUDIO
            audio_set_output_sampling_rate(OUTPUT_SR_48K);
#endif
//            core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_HOST_CDC_POWER_ON, 0);
        }
}

void usb_process_uac_in1_alt_0(void)
{
        RTK_USB_DEVICE_DBG_SIMPLE(  "%s %d\n", __func__, __LINE__);
        uac_stream_in1_inf_alt = 0;
        if((rtd_inl(SYS_REG_SYS_CLKEN1_reg) & _BIT16) == _BIT16) {
                //uac_stream_in1_inf_alt = 0;
                usb_uac_in_disconnect(_UAC_OUT1);
                rtk_dwc3_stop_active_transfer(13);
//                core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_HOST_CDC_POWER_OFF, 0);
                //usb_uac_out1_sram_init(UAC_OUT1_freq_type);
        }
}

void usb_process_uac_in1_alt_1(void)
{
        // Speedup for MACOS
        if( /*g_hdmi_audio_stable == 1 &&*/ isMacOS == 1) {
            dwc3_uac_out2_enable_timeout = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
            dwc3_uac_out2_enable_timeout = dwc3_uac_out2_enable_timeout + ((UINT32)100 * (UINT32)MS);
            if( dwc3_uac_out2_enable_timeout > (0xffffffff - 5 * MS) )
                    dwc3_uac_out2_enable_timeout = (0xffffffff - 5 * MS);
        }

        if((rtd_inl(SYS_REG_SYS_CLKEN1_reg) & _BIT16) == _BIT16) {
                uac_stream_in1_inf_alt = 1;
                //usb_uac_in_connect(_UAC_OUT1);
                //_rtk_dwc3_gadget_ep_enable(13, &uac_hs_epin1_desc, NULL, false);
                //rtk_dwc3_ep_start_trans_uac(UAC2_OUT_EP_NUMBER, 0);
#ifdef CONFIG_ENABLE_SW_AUDIO
                audio_set_output_sampling_rate(OUTPUT_SR_48K);
#endif
//                core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_HOST_CDC_POWER_ON, 0);
        }
}

UINT8 rtk_dwc3_set_alt(void)
{
        //pm_printk(0, "(interface:%u, alt:%u)\n", (UINT32)g_rtk_dwc3.ep0_ctrl_req->wIndex, (UINT32)g_rtk_dwc3.ep0_ctrl_req->wValue);

        // Interface defined:
        // 0: Video Control
        // 1: Video Stream
        // 2: Audio Control
        // 3: Audio Stream (OUT)
        // 4: Audio Stream (IN)
        // 5: Audio Stream (IN)
        // 6: CDC control
        // 7: CDC flow

        if(g_rtk_dwc3.ep0_ctrl_req->wIndex == UVC_CONTROL_INTERFACE_NUM)
        {
            if (g_rtk_dwc3.ep0_ctrl_req->wValue) {
                    pm_printk(LOGGER_ERROR, "[ER]\n");
            }

            __rtk_dwc3_gadget_ep_disable(3);
            _rtk_dwc3_gadget_ep_enable(3, &uvc_control_ep, NULL, false);
            return 0;

        }
        else if(g_rtk_dwc3.ep0_ctrl_req->wIndex == UVC_STREAM_INTERFACE_NUM)
        {
            //pm_printk(LOGGER_ERROR, "rtk_dwc3_set_alt UVC STREAM alt=%d\n", (UINT32)g_rtk_dwc3.ep0_ctrl_req->wValue);
            switch (g_rtk_dwc3.ep0_ctrl_req->wValue) {
                    case 0:
                            if(is_need_set_internal_pattern_gen == 0) {
                                    usb_uvc_disconnect(g_rtk_dwc3.eps[UVC_EP_NUMBER].number, 1);
                                    core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_UVC_DISCONNECT, 0);
                                    //pm_printk(LOGGER_FATAL, "USB alt 0 MSG_AP_UVC_DISCONNECT\n");
                            }
                            rtk_usb_device_disable_interrupt();
                            return 0;
                    case 1:
                            if(is_need_set_internal_pattern_gen == 0) {
                                    usb_uvc_connect(g_rtk_dwc3.eps[UVC_EP_NUMBER].number);
                                    core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_UVC_CONNECT, 0);
                                    //pm_printk(LOGGER_FATAL, "USB alt 1 MSG_AP_UVC_CONNECT\n");
                            }
                            return 0;
                    default:
                            return -EINVAL;
            }

        }
        else if(g_rtk_dwc3.ep0_ctrl_req->wIndex == uac_in_number)
        {
            switch (g_rtk_dwc3.ep0_ctrl_req->wValue) {
                    case 0:
                            //pm_printk(LOGGER_FATAL, "UACIN alt 0(%x)\n", (UINT32)uac_in_number);
                            //usb_process_uac_out_alt_0();
                            return 0;

                    case 1:
                            //usb_process_uac_out_alt_1();
                            return 0;

                    default:
                            return -EINVAL;
            }

        }
        else if(g_rtk_dwc3.ep0_ctrl_req->wIndex == uac_out1_number)
        {
            switch (g_rtk_dwc3.ep0_ctrl_req->wValue) {
                    case 0:
                            //pm_printk(LOGGER_FATAL, "UACOUT1 alt 0(%x)\n", (UINT32)uac_out1_number);
                            usb_process_uac_in0_alt_0();
                            return 0;

                    case 1:
                            usb_process_uac_in0_alt_1();
                            return 0;

                    default:
                            return -EINVAL;
            }

        }
        else if(g_rtk_dwc3.ep0_ctrl_req->wIndex == uac_out2_number)
        {
            switch (g_rtk_dwc3.ep0_ctrl_req->wValue) {
                    case 0:
                            //pm_printk(LOGGER_FATAL, "UACOUT2 alt 0(%x)\n", (UINT32)uac_out2_number);
                            usb_process_uac_in1_alt_0();
                            return 0;

                    case 1:
                            usb_process_uac_in1_alt_1();
                            return 0;

                    default:
                            return -EINVAL;
            }

        }
        else
        {
            pm_printk(LOGGER_ERROR, "[ER] interface num\n");
        }
        return 0;
}

static UINT8 rtk_dwc3_wIndex_to_dep(__le16 wIndex_le)
{
        epnum = (UINT8)((UINT8)(wIndex_le & USB_ENDPOINT_NUMBER_MASK) << 1);
        if ((wIndex_le & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN)
                epnum |= 1;

        return (g_rtk_dwc3.eps[epnum].flags & DWC3_EP_ENABLED) ? epnum : -1;
}

static UINT8 rtk_dwc3_wIndex_to_dep_no_status(__le16 wIndex_le)
{
        epnum = (UINT8)((UINT8)(wIndex_le & USB_ENDPOINT_NUMBER_MASK) << 1);
        if ((wIndex_le & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN)
                epnum |= 1;

        return epnum;
}

INT8 rtk_dwc3_ep0_std_request_set_feature(void)
{
                        pm_printk( LOGGER_FATAL, "USB_SET_FEATURE\n");
                        switch (g_rtk_dwc3.ep0_ctrl_req->bRequestType & USB_RECIP_MASK) {
                                case USB_RECIP_ENDPOINT:
                                        dwc_printk( 0, "USB_ENDPOINT\n");
                                        epnum = rtk_dwc3_wIndex_to_dep_no_status(g_rtk_dwc3.ep0_ctrl_req->wIndex);
                                        if (epnum == -1)
                                                return -EINVAL;
                                        if(g_rtk_dwc3.ep0_ctrl_req->wValue == USB_ENDPOINT_HALT)
                                                g_rtk_dwc3.eps[epnum].flags |=  DWC3_EP_STALL;
                                        break;
                                case USB_RECIP_INTERFACE:
                                        if(g_rtk_dwc3.ep0_ctrl_req->wValue == USB_INTRF_FUNC_SUSPEND) {
                                                if( g_rtk_dwc3.ep0_ctrl_req->wIndex == (UVC_CONTROL_INTERFACE_NUM | USB_INTRF_FUNC_SUSPEND_LOW_POWER_SUSPEND_STATE) ) {
                                                        //pm_printk(LOGGER_FATAL, "dwc3 SET FEATURE UVC LOW POWER MSG_AP_UVC_DISCONNECT\n");
                                                        //usb_uvc_disconnect(g_rtk_dwc3.eps[UVC_EP_NUMBER].number, 1);
                                                        //core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_UVC_DISCONNECT, 0);
                                                }
                                        }
                                        break;
                                case USB_RECIP_DEVICE:
                                        if(g_rtk_dwc3.ep0_ctrl_req->wValue > 6 && g_rtk_dwc3.ep0_ctrl_req->wValue < 48) {
                                                ret = -EOPNOTSUPP;
                                        } else {
                                                switch(g_rtk_dwc3.ep0_ctrl_req->wValue) {
#ifdef CONFIG_USB_SUPPORT_U1U2
                                                        case USB_DEVICE_U1_ENABLE:
                                                                if(g_rtk_dwc3.state != USB_STATE_CONFIGURED) {
                                                                        ret = -EOPNOTSUPP;
                                                                        break;
                                                                }
                                                                if (g_rtk_dwc3.speed <= DWC3_DSTS_HIGHSPEED)
                                                                        return -EINVAL;
                                                                if(!g_is_uvc_uac_start) {
                                                                        reg = rtd_inl(DWC3_DCTL);
                                                                        reg |= DWC3_DCTL_INITU1ENA;
                                                                        rtd_outl(DWC3_DCTL, reg);
                                                                }
                                                                break;
                                                        case USB_DEVICE_U2_ENABLE:
                                                                if(g_rtk_dwc3.state != USB_STATE_CONFIGURED) {
                                                                        ret = -EOPNOTSUPP;
                                                                        break;
                                                                }
                                                                if (g_rtk_dwc3.speed <= DWC3_DSTS_HIGHSPEED)
                                                                        return -EINVAL;
                                                                if(!g_is_uvc_uac_start) {
                                                                        reg = rtd_inl(DWC3_DCTL);
                                                                        reg |= DWC3_DCTL_INITU2ENA;
                                                                        rtd_outl(DWC3_DCTL, reg);
                                                                }
                                                                break;
#endif
                                                        default:
                                                                break;
                                                }
                                        }
                                        break;
                                default:
                                        ret = -EOPNOTSUPP;
                                        break;
                        }
                        return 0;
}

INT8 rtk_dwc3_ep0_std_request_get_status(void)
{
                        dwc_printk( 0, "USB_GET_STATUS\n");
                        //ret = rtk_dwc3_ep0_handle_status(g_rtk_dwc3.ep0_ctrl_req);
                        switch (g_rtk_dwc3.ep0_ctrl_req->bRequestType & USB_RECIP_MASK) {
                                case USB_RECIP_DEVICE:
                                        dwc_printk( 0, "USB_DEVICE\n");
                                        g_rtk_dwc3.is_selfpowered = IS_SUPPORT_SELF_POWERED;
                                        if (g_rtk_dwc3.speed >= DWC3_DSTS_SUPERSPEED) {
                                                reg = rtd_inl(DWC3_DCTL);
                                                if (reg & DWC3_DCTL_INITU1ENA) {
                                                        g_rtk_dwc3.is_selfpowered |= (UINT16)((UINT16)1 << USB_DEV_STAT_U1_ENABLED);
                                                }
                                                if (reg & DWC3_DCTL_INITU2ENA) {
                                                        g_rtk_dwc3.is_selfpowered |= (UINT16)((UINT16)1 << USB_DEV_STAT_U2_ENABLED);
                                                }
                                        }
                                        break;
                                case USB_RECIP_INTERFACE:
                                        g_rtk_dwc3.is_selfpowered = 0;
                                        dwc_printk( 0, "USB_INTERFACE\n");
                                        break;
                                case USB_RECIP_ENDPOINT:
                                        dwc_printk( 0, "USB_ENDPOINT\n");
                                        epnum = rtk_dwc3_wIndex_to_dep_no_status(g_rtk_dwc3.ep0_ctrl_req->wIndex);
                                        if (epnum == -1)
                                                return -EINVAL;
                                        if (g_rtk_dwc3.eps[epnum].flags & DWC3_EP_STALL)
                                                g_rtk_dwc3.is_selfpowered = (UINT8)((UINT8)1 << USB_ENDPOINT_HALT);
                                        else
                                                g_rtk_dwc3.is_selfpowered &= (~((UINT8)((UINT8)1 << USB_ENDPOINT_HALT)));
                                        break;
                                default:
                                        dwc_printk( 0, "UNKNOWN USB_RECIP_MASK ERROR\n");
                                        return -EINVAL;
                        }
                        g_rtk_dwc3.ep0_setup_buf_len = (sizeof(g_rtk_dwc3.is_selfpowered) > g_rtk_dwc3.ep0_ctrl_req->wLength) ? g_rtk_dwc3.ep0_ctrl_req->wLength : sizeof(g_rtk_dwc3.is_selfpowered);
                        //dwc3_memset((UINT8 *)&g_ep0_ctrl_req, 0, sizeof(struct usb_ctrlrequest));
                        //dwc3_memset((UINT8 *)&g_ep0_ctrl_req, 0, sizeof(struct usb_ctrlrequest));
#ifndef CONFIG_ARCH_5281
                        dwc_info.dst = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
                        dwc_info.src = &g_rtk_dwc3.is_selfpowered;
                        dwc_info.len = g_rtk_dwc3.ep0_setup_buf_len;
                        dwc3_copy_to_memory2();
#endif
#if 0
                        {
                                UINT32 temp_val;
                                temp_val = (UINT32)rtd_mem_inl(g_rtk_dwc3.ep0_setup_buf_addr);
                                dwc_printk( 0, "g_rtk_dwc3.ep0_setup_buf_addr=%x, val=%x\n", (UINT32)g_rtk_dwc3.ep0_setup_buf_addr, (UINT32)temp_val);
                        }
#endif
                        return 0;
}

INT8 rtk_dwc3_ep0_std_request_clear_feature(void)
{
                        pm_printk( LOGGER_FATAL, "USB_CLEAR_FEATURE\n");
                        //ret = rtk_dwc3_ep0_handle_feature(g_rtk_dwc3.ep0_ctrl_req, 0);
#ifndef CONFIG_ARCH_5281

                        switch (g_rtk_dwc3.ep0_ctrl_req->bRequestType & USB_RECIP_MASK) {
                                case USB_RECIP_ENDPOINT:
                                        if( g_rtk_dwc3.ep0_ctrl_req->wValue == USB_ENDPOINT_HALT) {
                                                rtk_dwc3_wIndex_to_dep(g_rtk_dwc3.ep0_ctrl_req->wIndex);
                                                dwc3_memset((void *)&params, 0, sizeof(params));
                                                rtk_dwc3_send_gadget_ep_cmd(epnum, DWC3_DEPCMD_CLEARSTALL);
                                        }
                                        break;
                                default:
                                        break;
                        }
#endif
                        switch (g_rtk_dwc3.ep0_ctrl_req->bRequestType & USB_RECIP_MASK) {
                                case USB_RECIP_ENDPOINT:
                                        dwc_printk( 0, "USB_ENDPOINT\n");
                                        epnum = rtk_dwc3_wIndex_to_dep_no_status(g_rtk_dwc3.ep0_ctrl_req->wIndex);
                                        if (epnum == -1)
                                                return -EINVAL;
                                        if(g_rtk_dwc3.ep0_ctrl_req->wValue == USB_ENDPOINT_HALT)
                                                g_rtk_dwc3.eps[epnum].flags &=  (~DWC3_EP_STALL);
                                        break;
                                case USB_RECIP_INTERFACE:
                                        break;
                                case USB_RECIP_DEVICE:
                                        if(g_rtk_dwc3.ep0_ctrl_req->wValue > 6 && g_rtk_dwc3.ep0_ctrl_req->wValue < 48) {
                                                ret = -EOPNOTSUPP;
                                        } else {
                                                switch(g_rtk_dwc3.ep0_ctrl_req->wValue) {
#ifdef CONFIG_USB_SUPPORT_U1U2
                                                        case USB_DEVICE_U1_ENABLE:
                                                                if(g_rtk_dwc3.state != USB_STATE_CONFIGURED) {
                                                                        ret = -EOPNOTSUPP;
                                                                        break;
                                                                }
                                                                if (g_rtk_dwc3.speed <= DWC3_DSTS_HIGHSPEED)
                                                                        return -EINVAL;

                                                                reg = rtd_inl(DWC3_DCTL);
                                                                reg &= ~DWC3_DCTL_INITU1ENA;
                                                                rtd_outl(DWC3_DCTL, reg);
                                                                break;
                                                        case USB_DEVICE_U2_ENABLE:
                                                                if(g_rtk_dwc3.state != USB_STATE_CONFIGURED) {
                                                                        ret = -EOPNOTSUPP;
                                                                        break;
                                                                }
                                                                if (g_rtk_dwc3.speed <= DWC3_DSTS_HIGHSPEED)
                                                                        return -EINVAL;

                                                                reg = rtd_inl(DWC3_DCTL);
                                                                reg &= ~DWC3_DCTL_INITU2ENA;
                                                                rtd_outl(DWC3_DCTL, reg);
                                                                break;
#endif
                                                        default:
                                                                break;
                                                }
                                        }
                                        break;
                                default:
                                        ret = -EOPNOTSUPP;
                                        break;
                        }
                        return ret;
}

INT8 rtk_dwc3_ep0_std_request_set_address(void)
{
                        dwc_printk( 0, "USB_SET_ADDRESS\n");
                        usb_set_address = 1;
                        //rtk_dwc3_ep0_set_address
                        //addr = g_rtk_dwc3.ep0_ctrl_req->wValue;
                        if (g_rtk_dwc3.state == USB_STATE_CONFIGURED) {
                                pm_printk(LOGGER_ERROR, "trying to set address when configured");
                                return -EINVAL;
                        }
                        reg = rtd_inl(DWC3_DCFG);
                        reg &= ~(DWC3_DCFG_DEVADDR_MASK);
                        reg |= DWC3_DCFG_DEVADDR(g_rtk_dwc3.ep0_ctrl_req->wValue);
                        rtd_outl(DWC3_DCFG, reg);

                        g_rtk_dwc3.state = (g_rtk_dwc3.ep0_ctrl_req->wValue) ? USB_STATE_ADDRESS : USB_STATE_DEFAULT;
                        return ret;
}

static INT8 rtk_dwc3_ep0_std_request(void)
{
        switch (g_rtk_dwc3.ep0_ctrl_req->bRequest) {
                case USB_REQ_SET_FEATURE:
                        ret = rtk_dwc3_ep0_std_request_set_feature();
                        break;
                case USB_REQ_GET_STATUS:
                        rtk_dwc3_ep0_std_request_get_status();
                        return 0;
                        break;
                case USB_REQ_SET_SEL:
                        RTK_USB_DEVICE_DBG_SIMPLE( "USB_REQ_SET_SEL\n");
                        break;
                case USB_REQ_SET_ISOCH_DELAY:
                        RTK_USB_DEVICE_DBG_SIMPLE( "USB_SET_ISOCH_DELAY\n");
                        break;
                case USB_REQ_CLEAR_FEATURE:
                        ret = rtk_dwc3_ep0_std_request_clear_feature();
                        break;
                case USB_REQ_SET_ADDRESS:
                        ret = rtk_dwc3_ep0_std_request_set_address();
                        break;
                case USB_REQ_SET_CONFIGURATION:
                        dwc_printk( 0, "USB_SET_CONFIGURATION\n");
                        usb_set_address = 0;
                        ret = rtk_dwc3_ep0_delegate_req();
                        break;
                case USB_REQ_SET_INTERFACE:
                        dwc_printk( 0, "USB_REQ_SET_INTERFACE\n");
                        ret = rtk_dwc3_set_alt();
                        break;
                case USB_REQ_GET_INTERFACE:
                        dwc_printk( 0, "USB_REQ_GET_INTERFACE\n");
                        ret = rtk_dwc3_get_alt();
                        break;
                default:
                        dwc_printk( 0, "Forwarding to gadget driver\n");
                        // FIXME: let composite driver handle get descriptor.
                        ret = rtk_dwc3_ep0_delegate_req();
                        break;
        }
        //pm_printk(LOGGER_ERROR, "%s: speed(%d), type(%x) req(%x) val(%x) index(%x), length(%x), ret(%x)\n", __func__,
        //               g_rtk_dwc3.speed, g_rtk_dwc3.ep0_ctrl_req->bRequestType, g_rtk_dwc3.ep0_ctrl_req->bRequest,
        //              g_rtk_dwc3.ep0_ctrl_req->wValue, g_rtk_dwc3.ep0_ctrl_req->wIndex,g_rtk_dwc3.ep0_ctrl_req->wLength, ret);
        return ret;
}

void rtk_dwc3_ep0_stall_and_restart(void)
{
        pm_printk(0, "[ER] ep0 stall & restart\n");
        /* reinitialize physical ep1 */
        g_rtk_dwc3.eps[1].flags = DWC3_EP_ENABLED;

        /* stall is always issued on EP0 */
        __rtk_dwc3_gadget_ep_set_halt(0, 1, false);
        g_rtk_dwc3.eps[0].flags = DWC3_EP_ENABLED;
        //g_rtk_dwc3.delayed_status = false;

        //rtk_dwc3_ep0_start_setup_phase
        g_rtk_dwc3.ep0state = EP0_SETUP_PHASE;
        rtk_dwc3_ep_start_trans(0, dvr_to_phys(g_rtk_dwc3.ep0_ctrl_req_addr), 8,
                                DWC3_TRBCTL_CONTROL_SETUP);

        wait_do_control_transfer = 0;
}

void rtk_dwc3_endpoint_interrupt_check_not_ready(UINT8 ep_number)
{
        if(ep_number == UAC1_IN_EP_NUMBER) {
                // Host -> device
                rtk_dwc3_ep_start_trans_uac(ep_number, 0);
#if 0
                trb_wr_addr_last = rtd_inl(0xB80064A4);
                dwc3_enable_uac_timer();
#endif
        }

        if(ep_number == UAC1_OUT_EP_NUMBER) {
                // Device -> Host
                while(rtk_dwc3_ep_start_trans_uac(ep_number, 0) != 0) {
                    pm_printk(LOGGER_ERROR, "uac ep disable ep=%d\n", (UINT32)epnum);
                    __rtk_dwc3_gadget_ep_disable_uac(ep_number);
                }
            RTK_USB_DEVICE_DBG_SIMPLE( "rtk_dwc3_endpoint_interrupt_check_not_ready %d\n", ep_number);
            run_uac = 1;
        }

        if(ep_number == UAC2_OUT_EP_NUMBER) {
        }

        dwc_printk(LOGGER_WARN, "ep=%x XFERNOTREADY\n", (UINT32)ep_number);
}

static void rtk_dwc3_endpoint_interrupt_EP0(const struct dwc3_event_depevt *event)
{
    UINT32 *ptr = (UINT32 *)event;
    UINT8 *u8p;
                //rtk_dwc3_ep0_interrupt(event);
                switch (((UINT32) * (ptr) >> 6) & 0xF) {
                        case DWC3_DEPEVT_XFERCOMPLETE:
                                //rtk_dwc3_ep0_xfer_complete
                                g_rtk_dwc3.eps[event->endpoint_number].flags &= ~DWC3_EP_BUSY;
                                g_rtk_dwc3.eps[event->endpoint_number].resource_index = 0;
                                g_rtk_dwc3.setup_packet_pending = false;

                                switch (g_rtk_dwc3.ep0state) {
                                        case EP0_SETUP_PHASE:
                                                wait_do_control_transfer = 1;
                                                dwc_printk(0, "(event->endpoint_number=%x)   DWC3_DEPEVT_XFERCOMPLETE => EP0_SETUP_PHASE \n", (UINT32)event->endpoint_number);
                                                //rtk_dwc3_ep0_inspect_setup
                                                g_rtk_dwc3.ep0_setup_buf_len = 0;
                                                g_rtk_dwc3.ep0_actual_buf_len = 0;
                                                g_rtk_dwc3.ep0_last_callback_id = 0;

#ifndef CONFIG_ARCH_5281
                                                dwc_copy_to_dmem_info.dst = g_rtk_dwc3.ep0_ctrl_req;
                                                dwc_copy_to_dmem_info.src = (void*)(g_rtk_dwc3.ep0_ctrl_req_addr + usb_phy_vir_offset);
                                                dwc_copy_to_dmem_info.len = sizeof(struct usb_ctrlrequest);
                                                dwc3_copy_from_memory2();
                                                u8p = (UINT8*)(g_rtk_dwc3.ep0_ctrl_req_addr + usb_phy_vir_offset);
                                                g_rtk_dwc3.ep0_ctrl_req->bRequestType = u8p[0];
                                                g_rtk_dwc3.ep0_ctrl_req->bRequest     = u8p[1];
                                                g_rtk_dwc3.ep0_ctrl_req->wValue       = (u8p[3] << 8) + u8p[2];
                                                g_rtk_dwc3.ep0_ctrl_req->wIndex       = (u8p[5] << 8) + u8p[4];
                                                g_rtk_dwc3.ep0_ctrl_req->wLength      = (u8p[7] << 8) + u8p[6];
#endif
#if 0
                                                pm_printk(LOGGER_ERROR, "11 %s: speed(%d), type(%x) req(%x) val(%x) index(%x), length(%x), ret(%x)\n", __func__,
                                                          (UINT32)g_rtk_dwc3.speed, (UINT32)g_rtk_dwc3.ep0_ctrl_req->bRequestType, (UINT32)g_rtk_dwc3.ep0_ctrl_req->bRequest,
                                                          (UINT32)g_rtk_dwc3.ep0_ctrl_req->wValue, (UINT32)g_rtk_dwc3.ep0_ctrl_req->wIndex, (UINT32)g_rtk_dwc3.ep0_ctrl_req->wLength, ret);
#endif
                                                if (!g_rtk_dwc3.ep0_ctrl_req->wLength) {
                                                        g_rtk_dwc3.three_stage_setup = false;
                                                        g_rtk_dwc3.ep0_expect_in = false;
                                                        g_rtk_dwc3.ep0_next_event = DWC3_EP0_NRDY_STATUS;
                                                } else {
                                                        g_rtk_dwc3.three_stage_setup = true;
                                                        g_rtk_dwc3.ep0_expect_in = !!(g_rtk_dwc3.ep0_ctrl_req->bRequestType & USB_DIR_IN);
                                                        g_rtk_dwc3.ep0_next_event = DWC3_EP0_NRDY_DATA;
                                                }

                                                //dwc_printk(0, "%x %x %x %x %x\n", (UINT32)g_rtk_dwc3.ep0_ctrl_req->bRequestType, (UINT32)g_rtk_dwc3.ep0_ctrl_req->bRequest, (UINT32)g_rtk_dwc3.ep0_ctrl_req->wValue, (UINT32)g_rtk_dwc3.ep0_ctrl_req->wIndex, (UINT32)g_rtk_dwc3.ep0_ctrl_req->wLength );
                                                //dwc_printk(0, "%x\n", (UINT32)g_rtk_dwc3.ep0_ctrl_req->bRequestType);
                                                //dwc_printk(0, "%x\n", (UINT32)g_rtk_dwc3.ep0_ctrl_req->bRequest);
                                                //dwc_printk(0, "%x\n", (UINT32)g_rtk_dwc3.ep0_ctrl_req->wValue);
                                                //dwc_printk(0, "%x\n", (UINT32)g_rtk_dwc3.ep0_ctrl_req->wIndex);
                                                //dwc_printk(0, "%x\n", (UINT32)g_rtk_dwc3.ep0_ctrl_req->wLength );
                                                show_control_transfer_info();

                                                ret = 0;
                                                if ((g_rtk_dwc3.ep0_ctrl_req->bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD)
                                                        ret = rtk_dwc3_ep0_std_request();
                                                else
                                                        ret = rtk_dwc3_ep0_delegate_req();

                                                if (ret != 0) {
                                                        rtk_dwc3_ep0_stall_and_restart();
                                                } else {
                                                        if(g_rtk_dwc3.three_stage_setup) {
                                                                g_rtk_dwc3.ep0_setup_buf_len = g_rtk_dwc3.ep0_expect_in ? g_rtk_dwc3.ep0_setup_buf_len : 1024;
                                                                g_rtk_dwc3.ep0state = EP0_DATA_PHASE;
                                                                if(g_rtk_dwc3.ep0_expect_in)
                                                                        ret =  rtk_dwc3_ep_start_trans(1, dvr_to_phys(g_rtk_dwc3.ep0_setup_buf_addr), g_rtk_dwc3.ep0_setup_buf_len, DWC3_TRBCTL_CONTROL_DATA);
                                                                else
                                                                        ret =  rtk_dwc3_ep_start_trans(0, dvr_to_phys(g_rtk_dwc3.ep0_setup_buf_addr), g_rtk_dwc3.ep0_setup_buf_len, DWC3_TRBCTL_CONTROL_DATA);
                                                                if(ret < 0) {
                                                                        pm_printk(0, "SLL 2\n");
                                                                        rtk_dwc3_ep0_stall_and_restart();
                                                                }
                                                        } else {
                                                                g_rtk_dwc3.ep0state = EP0_STATUS_PHASE;
                                                        }
                                                }
                                                if(g_rtk_dwc3.dwc_need_reconnect == 2) {
                                                    g_rtk_dwc3.dwc_need_reconnect = 3;
                                                }
                                                break;

                                        case EP0_DATA_PHASE:
                                                dwc_printk(0, "(event->endpoint_number=%x)     DWC3_DEPEVT_XFERCOMPLETE => EP0_DATA_PHASE\n", (UINT32)event->endpoint_number);
                                                g_rtk_dwc3.ep0_next_event = DWC3_EP0_NRDY_STATUS;
                                                //rtk_dwc3_ep0_complete_data
#ifndef CONFIG_ARCH_5281
                                                dwc_copy_to_dmem_info.dst = g_rtk_dwc3.ep0_trbs;
                                                dwc_copy_to_dmem_info.src = (void*)(g_rtk_dwc3.ep0_trbs_addr + usb_phy_vir_offset);
                                                dwc_copy_to_dmem_info.len = sizeof(struct dwc3_trb);
                                                dwc3_copy_from_memory2();
#endif

                                                if (DWC3_TRB_SIZE_TRBSTS(g_rtk_dwc3.ep0_trbs->size) == DWC3_TRBSTS_SETUP_PENDING) {
                                                        pm_printk(LOGGER_ERROR, "[USB_TRB] g_rtk_dwc3.ep0_trbs->size!\n");
                                                        rtk_dwc3_ep0_callback(-ECONNRESET, 0);
                                                        break;
                                                }
                                                g_rtk_dwc3.ep0_actual_buf_len += (g_rtk_dwc3.ep0_setup_buf_len - (g_rtk_dwc3.ep0_trbs->size & DWC3_TRB_SIZE_MASK));

                                                //pm_printk(LOGGER_ERROR, "(event->endpoint_number=%x)     g_rtk_dwc3.ep0_actual_buf_len=%x, g_rtk_dwc3.ep0_setup_buf_len=%x, g_rtk_dwc3.ep0_trbs->size=%x\n",
                                                //                        (UINT32)event->endpoint_number, (UINT32)g_rtk_dwc3.ep0_actual_buf_len, (UINT32)g_rtk_dwc3.ep0_setup_buf_len, (UINT32)g_rtk_dwc3.ep0_trbs->size);
                                                if ((event->endpoint_number & 1) && g_rtk_dwc3.ep0_actual_buf_len < g_rtk_dwc3.ep0_setup_buf_len) {
                                                        /* for some reason we did not get everything out */
                                                        pm_printk(LOGGER_ERROR, "trb size is not zero !!! (Please check SRAM sidde)\n");
                                                        rtk_dwc3_ep0_stall_and_restart();
                                                } else {
                                                        ret = 0;
                                                        rtk_dwc3_ep0_callback(0, g_rtk_dwc3.ep0_actual_buf_len);
                                                        if(ret != 0)
                                                                rtk_dwc3_ep0_stall_and_restart();
                                                }
                                                break;
                                        case EP0_STATUS_PHASE:
                                                dwc_printk(0, "(event->endpoint_number=%x)     DWC3_DEPEVT_XFERCOMPLETE => EP0_STATUS_PHASE\n", (UINT32)event->endpoint_number);
                                                //rtk_dwc3_ep0_start_setup_phase();
                                                g_rtk_dwc3.ep0state = EP0_SETUP_PHASE;
                                                rtk_dwc3_ep_start_trans(0, dvr_to_phys(g_rtk_dwc3.ep0_ctrl_req_addr), 8,
                                                                        DWC3_TRBCTL_CONTROL_SETUP);
                                                wait_do_control_transfer = 0;
                                                break;
                                        default:
                                                pm_printk(LOGGER_ERROR, "UNKNOWN ep0state %d\n", g_rtk_dwc3.ep0state);
                                                break;
                                }
                                break;

                        case DWC3_DEPEVT_XFERNOTREADY:
                                //pm_printk(LOGGER_ERROR, "(event->endpoint_number=%x)     DWC3_DEPEVT_XFERNOTREADY\n", (UINT32)event->endpoint_number);
                                if(g_rtk_dwc3.ep0state == EP0_SETUP_PHASE) {
                                        //rtk_dwc3_ep0_stall_and_restart();
                                        break;
                                }
                                //rtk_dwc3_ep0_xfernotready
                                g_rtk_dwc3.setup_packet_pending = true;
                                switch (((UINT32) * (ptr) >> 12) & 0xF) {
                                        case DEPEVT_STATUS_CONTROL_DATA:
                                                dwc_printk(0, "(event->endpoint_number=%x)     DWC3_DEPEVT_XFERNOTREADY => Control Data\n", (UINT32)event->endpoint_number);
                                                /*
                                                 * We already have a DATA transfer in the controller's cache,
                                                 * if we receive a XferNotReady(DATA) we will ignore it, unless
                                                 * it's for the wrong direction.
                                                 *
                                                 * In that case, we must issue END_TRANSFER command to the Data
                                                 * Phase we already have started and issue SetStall on the
                                                 * control endpoint.
                                                 */
                                                if (g_rtk_dwc3.ep0_expect_in != event->endpoint_number) {
                                                        //g_rtk_dwc3.eps[event->endpoint_number] = g_rtk_dwc3.eps[g_rtk_dwc3.ep0_expect_in];
                                                        pm_printk(LOGGER_ERROR, "Wrong direction for Data phase\n");

                                                        //rtk_dwc3_ep0_end_control_data
                                                        if (g_rtk_dwc3.eps[g_rtk_dwc3.ep0_expect_in].resource_index) {
                                                                u32_uvc_temp_4 = DWC3_DEPCMD_ENDTRANSFER;
                                                                u32_uvc_temp_4 |= DWC3_DEPCMD_CMDIOC;
                                                                u32_uvc_temp_4 |= DWC3_DEPCMD_PARAM(g_rtk_dwc3.eps[g_rtk_dwc3.ep0_expect_in].resource_index);
                                                                dwc3_memset((void *)&params, 0, sizeof(params));
                                                                ret = rtk_dwc3_send_gadget_ep_cmd(g_rtk_dwc3.ep0_expect_in, u32_uvc_temp_4);
                                                                g_rtk_dwc3.eps[g_rtk_dwc3.ep0_expect_in].resource_index = 0;
                                                        }
                                                        rtk_dwc3_ep0_stall_and_restart();
                                                        return;
                                                }
                                                break;

                                        case DEPEVT_STATUS_CONTROL_STATUS:
                                                if (g_rtk_dwc3.ep0_next_event != DWC3_EP0_NRDY_STATUS)
                                                        return;

                                                g_rtk_dwc3.ep0state = EP0_STATUS_PHASE;

                                                //if (g_rtk_dwc3.delayed_status)
                                                //{
                                                //       pm_printk(LOGGER_ERROR, "Delayed Status");
                                                //       return;
                                                //}

                                                //rtk_dwc3_ep0_start_control_status
                                                dwc_printk(0, "(event->endpoint_number=%x)     DWC3_DEPEVT_XFERNOTREADY => Control Status\n", (UINT32)event->endpoint_number);
                                                if(g_rtk_dwc3.three_stage_setup)
                                                        rtk_dwc3_ep_start_trans(event->endpoint_number, dvr_to_phys(g_rtk_dwc3.ep0_ctrl_req_addr), 0, DWC3_TRBCTL_CONTROL_STATUS3);
                                                else
                                                        rtk_dwc3_ep_start_trans(event->endpoint_number, dvr_to_phys(g_rtk_dwc3.ep0_ctrl_req_addr), 0, DWC3_TRBCTL_CONTROL_STATUS2);
                                }
                                break;

                        case DWC3_DEPEVT_XFERINPROGRESS:
                        case DWC3_DEPEVT_RXTXFIFOEVT:
                        case DWC3_DEPEVT_STREAMEVT:
                        case DWC3_DEPEVT_EPCMDCMPLT:
                                break;
                }
}

void rtk_dwc3_endpoint_interrupt_EP(const struct dwc3_event_depevt *event)
{
    UINT32  cmd;
    UINT32 *ptr = (UINT32 *)event;

        switch (((UINT32) * (ptr) >> 6) & 0xF) {
                case DWC3_DEPEVT_XFERNOTREADY:
                        u32_rtk_time_stamp = (UINT32) * (ptr);
                        rtk_dwc3_endpoint_interrupt_check_not_ready(event->endpoint_number);
                        break;
                case DWC3_DEPEVT_XFERCOMPLETE:
                        g_rtk_dwc3.eps[event->endpoint_number].resource_index = 0;

                        if (usb_endpoint_xfer_isoc(g_rtk_dwc3.eps[event->endpoint_number].desc)) {
                                dwc_printk(0, "%d is an ISO EP\n",
                                           g_rtk_dwc3.eps[event->endpoint_number].number);
                                return;
                        }
                        g_rtk_dwc3.eps[event->endpoint_number].flags &= ~DWC3_EP_BUSY;

                        dwc_copy_to_dmem_info.dst = &g_rtk_dwc3.eps[event->endpoint_number].trbs[0];
                        dwc_copy_to_dmem_info.src = (void*)(g_rtk_dwc3.eps[event->endpoint_number].trbs_addr + usb_phy_vir_offset);
                        dwc_copy_to_dmem_info.len = sizeof(struct dwc3_trb);
                        dwc3_copy_from_memory2();
                        if(event->endpoint_number == CDC_BULK_IN) {
                                //dwc_printk(0, "(event->endpoint_number=%x)     uart_send_callback (Transfer size=%x, TRB size=%x)\n", (UINT32)event->endpoint_number, (UINT32)g_rtk_dwc3.eps[event->endpoint_number].transfer_size, (UINT32)DWC3_TRB_SIZE_LENGTH(g_rtk_dwc3.eps[event->endpoint_number].trbs[0].size));
                                cmd = (UINT32)g_rtk_dwc3.eps[event->endpoint_number].transfer_size;
                                cmd = cmd - DWC3_TRB_SIZE_LENGTH(g_rtk_dwc3.eps[event->endpoint_number].trbs[0].size);
                                //uart_send_callback((((UINT32) * (ptr) >> 12) & 0xF & DEPEVT_STATUS_BUSERR) ? (-ECONNRESET) : 0, cmd);
                        } else if(event->endpoint_number == CDC_BULK_OUT) {
                                //dwc_printk(0, "(event->endpoint_number=%x)     uart_recv_callbackm. (Transfer size=%x, TRB size=%x)\n", (UINT32)event->endpoint_number, (UINT32)g_rtk_dwc3.eps[event->endpoint_number].transfer_size, (UINT32)DWC3_TRB_SIZE_LENGTH(g_rtk_dwc3.eps[event->endpoint_number].trbs[0].size));
                                cmd = (UINT32)g_rtk_dwc3.eps[event->endpoint_number].transfer_size;
                                cmd = cmd - DWC3_TRB_SIZE_LENGTH(g_rtk_dwc3.eps[event->endpoint_number].trbs[0].size);
                                dwc_copy_to_dmem_info.dst = g_usb_uart.recv_buf;
                                dwc_copy_to_dmem_info.src = (void*)(g_usb_uart.recv_buf_addr + usb_phy_vir_offset);
                                while(cmd > 0) {
                                        if(cmd >= sizeof(g_recv_buf)) {
                                                dwc_copy_to_dmem_info.len = sizeof(g_recv_buf);
                                                cmd = cmd - sizeof(g_recv_buf);
                                                dwc3_copy_from_memory2();
                                                dwc_copy_to_dmem_info.src += sizeof(g_recv_buf);
                                                //uart_recv_callback((((UINT32) * (ptr) >> 12) & 0xF & DEPEVT_STATUS_BUSERR) ? (-ECONNRESET) : 0, sizeof(g_recv_buf));
                                        } else {
                                                dwc_copy_to_dmem_info.len = cmd;
                                                dwc3_copy_from_memory2();
                                                //uart_recv_callback((((UINT32) * (ptr) >> 12) & 0xF & DEPEVT_STATUS_BUSERR) ? (-ECONNRESET) : 0, cmd);
                                                cmd = 0;
                                        }
                                }
                        } else if(event->endpoint_number == UVC_STREAM_INTERFACE_ENDPOINTADDRESS) {
                            uvc_frame_buffer_status[uvc_frame_buffer_rp] = 0;
                        }
                        break;
                default:
                        break;
        }
}

static void rtk_dwc3_endpoint_interrupt(const struct dwc3_event_depevt *event)
{
        UINT32 *ptr = (UINT32 *)event;

        if (!(g_rtk_dwc3.eps[event->endpoint_number].flags & DWC3_EP_ENABLED)) {
                if((((UINT32) * (ptr) >> 6) & 0xF) != DWC3_DEPEVT_EPCMDCMPLT)
                    RTK_USB_DEVICE_DBG_SIMPLE(  "usb_endpoint_interrupt fail event = %x\n", (UINT32) * (ptr));
                return;
        }

        if (event->endpoint_number <= 1) {
                rtk_dwc3_endpoint_interrupt_EP0(event);
                return;
        }

        rtk_dwc3_endpoint_interrupt_EP(event);
}

irqreturn_t rtk_dwc3_process_event_buf(void)
{
        g_dwc3_event_left = g_rtk_dwc3.g_event_buf.count;
        if (!(g_rtk_dwc3.g_event_buf.flags & DWC3_EVENT_PENDING)) {
                pm_printk(LOGGER_ERROR, "[ER] usb event buf\n");
                return IRQ_NONE;
        }

        while (g_dwc3_event_left >= 4) {
                event.raw = (UINT32)rtd_mem_inl(g_rtk_dwc3.g_event_buf.buf + g_rtk_dwc3.g_event_buf.lpos);
                dwc_printk(0, "[EVENT] =================== %x ======================\n", (UINT32)event.raw);

                //rtk_dwc3_process_event_entry
                (event.type.is_devspec == 0) ? rtk_dwc3_endpoint_interrupt(&event.depevt) : rtk_dwc3_gadget_interrupt(&event.devt);
                if(g_dwc3_is_reinit == 1) {
                        g_dwc3_is_reinit = 0;
                        break;
                }
                g_rtk_dwc3.g_event_buf.lpos = (g_rtk_dwc3.g_event_buf.lpos + 4) % DWC3_EVENT_BUFFERS_SIZE;
                g_dwc3_event_left -= 4;
                rtd_outl(DWC3_GEVNTCOUNT(0), 4);
        }

        g_rtk_dwc3.g_event_buf.count = 0;
        g_rtk_dwc3.g_event_buf.flags &= ~DWC3_EVENT_PENDING;

        /* Unmask interrupt */
        reg = rtd_inl(DWC3_GEVNTSIZ(0));
        reg &= ~DWC3_GEVNTSIZ_INTMASK;
        rtd_outl(DWC3_GEVNTSIZ(0), reg);

#if 0
        imod_i = DWC3_DEV_IMOD_INTERVAL(rtd_inl(DWC3_DEV_IMOD(0)));
        if (imod_i) {
                busy = !!(rtd_inl(DWC3_GEVNTCOUNT(0)) & DWC3_GEVNTCOUNT_BUSY_MASK);
                if (busy) {
                        /* clear EHB */
                        rtd_outl(DWC3_GEVNTCOUNT(0), DWC3_GEVNTCOUNT_BUSY_MASK);
                }
        }
#endif
        return IRQ_HANDLED;
}

void rtk_dwc3_init_endpoints(void)
{
        UINT8 i;
        UINT8 out_ep_num;

        dwc3_memset((UINT8 *)g_rtk_dwc3.eps, 0, sizeof(g_rtk_dwc3.eps));
        reg = rtd_inl(DWC3_GHWPARAMS3);
        //pm_printk(LOGGER_ERROR, "DWC3_GHWPARAMS3 = %x, reg=%x\n", (UINT32)DWC3_GHWPARAMS3, (UINT32)reg);

        g_rtk_dwc3.num_eps = (UINT8)USB_EP_COUNT;
        g_rtk_dwc3.num_in_eps = (UINT8)(USB_EP_COUNT / 2);

        out_ep_num = g_rtk_dwc3.num_eps - g_rtk_dwc3.num_in_eps;
        for( i = 0; i < out_ep_num; i++) {
                epnum = (UINT8)((UINT8)i << 1);
                g_rtk_dwc3.eps[epnum].number = epnum;
                g_rtk_dwc3.eps[epnum].direction = 0;

        }

        for( i = 0; i <  g_rtk_dwc3.num_in_eps; i++) {
                epnum = (UINT8)((UINT8)i << 1) | 1;
                g_rtk_dwc3.eps[epnum].number = epnum;
                g_rtk_dwc3.eps[epnum].direction = 1;
        }
        //pm_printk(LOGGER_ERROR, "g_rtk_dwc3.num_eps = %x, g_rtk_dwc3.num_in_eps = %x\n", (UINT32)g_rtk_dwc3.num_eps, (UINT32)g_rtk_dwc3.num_in_eps);
}

UINT8 rtk_dwc3_gadget_ep_get_transfer_index(UINT8 number)
{
        u32_uvc_temp = rtd_inl(DWC3_DEPCMD(number));
        return DWC3_DEPCMD_GET_RSC_IDX(u32_uvc_temp);
}

void rtk_dwc3_connect_timeout_check(void) __banked
{
        pm_printk(LOGGER_FATAL, "USB timeout check msg\n");
        if (g_rtk_dwc3.speed == USB_SPEED_UNKNOWN)
        {
                pm_printk(LOGGER_FATAL, "USB send SPEED_UNKNOW\n");
                core_fw_msg_queue_cancelEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT);
                core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT, (UINT32)MSG_USB_CONNECT_SPEED_UNKNOW);
        }
}

void rtk_dwc3_usb_phy_compliance_monitor(void)
{
        //polling LTSSM for DCC on off
        u32_uvc_temp = (UINT32)rtd_inl(0x1805870c);
        if((u32_uvc_temp & 0x003c0007) == 0x00280005) {
                if((((UINT32)rtd_inl(0x18030ef4)) & 0x3) == 0x0) {
                        rtd_outl(0x18030ef4, 0x0ff00003); //TX DCC on
                        pm_printk(0, "Compliance Gen2 DCC on %x\n", (UINT32)rtd_inl(0x18030ef4));
                }
        }
}

UINT8 usb_uvc_check_trb_hwo_full(void) __banked;
void usb_uvc_setting_framerate_xtal_cnt(void)  __banked;

UINT8 usb_uvc_check_trb_hwo_full_ex(void)
{
        return usb_uvc_check_trb_hwo_full();
}

void compare_UsbSof_LocalCouner_reset(void)
{
    u32_sof_last  = 0;
    u32_sof_total = 0;
    u32_27M_last  = 0;
    u32_27M_total = 0;
    UsbSof_LocalCounter_diff_mode = 0xff;
}

void compare_UsbSof_LocalCouner_diff_1(void)
{
    u32_uvc_temp_2 = rtd_inl(0xb805870c);
    u32_uvc_temp_2 = (u32_uvc_temp_2 >> 3) & 0x3fff;
    u32_uvc_temp_4 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
}

void compare_UsbSof_LocalCouner_diff_2(void)
{
    if(u32_27M_last != 0) {
        if(u32_uvc_temp_2 < u32_sof_last)
            u32_uvc_temp_3 = u32_uvc_temp_2 + 0x4000;
        else
            u32_uvc_temp_3 = u32_uvc_temp_2;
        u32_sof_total = u32_sof_total + u32_uvc_temp_3 - u32_sof_last;
        u32_27M_total = u32_27M_total + u32_uvc_temp_4 - u32_27M_last;
    }
}

void compare_UsbSof_LocalCouner_diff_3(void)
{
    u32_sof_last = u32_uvc_temp_2;
    u32_27M_last = u32_uvc_temp_4;
}

void compare_UsbSof_LocalCouner_diff(void)
{
    if((rtd_inl(0x18088000) & 0x01000000)) {
        if(UsbSof_LocalCounter_diff_mode == 0xff) {
            compare_UsbSof_LocalCouner_diff_1();
            compare_UsbSof_LocalCouner_diff_2();
            compare_UsbSof_LocalCouner_diff_3();
            pm_printk(0, "SOF measure %d %d ", u32_uvc_temp_2, u32_uvc_temp_4);
            pm_printk(0, "%d %d\n", u32_sof_total, u32_27M_total);
            u32_uvc_temp_2 = u32_sof_total / 8;
            u32_uvc_temp_4 = u32_27M_total / 90;
            if(u32_uvc_temp_4 > 180000) {
                if(u32_uvc_temp_4 > u32_uvc_temp_2) {
                    u32_uvc_temp_4 = u32_uvc_temp_4 - u32_uvc_temp_2;
                } else {
                    u32_uvc_temp_4 = u32_uvc_temp_2 - u32_uvc_temp_4;
                }

                if(u32_uvc_temp_4 >= 3) {
                    UsbSof_LocalCounter_diff_mode = 1;
                } else {
                    UsbSof_LocalCounter_diff_mode = 0;
                }
                usb_uvc_setting_framerate_xtal_cnt();
                pm_printk(0, "diff mode %d\n", (UINT32)UsbSof_LocalCounter_diff_mode);
                compare_UsbSof_LocalCouner_reset();
            }
        }
    }
}

void dwc3_usb_monitor(void) __banked
{
        RTK_USB_DEVICE_DBG_SIMPLE(  "rtk_usb_device ### dwc3_usb_monitor need to implement ----------------\n");
        compare_UsbSof_LocalCouner_diff();
        //polling LTSSM for DCC on off
        rtk_dwc3_usb_phy_compliance_monitor();
}

void rtk_usb_driver_cdc_function_init(void)
{
        if( GetFlashCdcOnOff() == 1 ) {
            g_rtk_dwc3.dwc_use_cdc = 1;
            mc_console_cdc_on();
        } else {
            g_rtk_dwc3.dwc_use_cdc = 0;
            mc_console_cdc_off();
        }
        g_rtk_dwc3.dwc_need_reconnect = 0;
}

void rtk_usb_driver_init_parameter(void)
{
    isMacOS = 0;
}

UINT8 rtk_usb_driver_init(void) __banked
{
#ifdef USB_U2_ONLY_MODE
        rtk_dwc3_usb2_0_only = 1;
#endif

        rtk_usb_driver_init_parameter();
        if(u8_need_calculate_usb_boot_time == 1)
        {
#ifdef DWC3_RTK_ENABLE_USB_BOOT_TIME_DEBUG_LOG
                u32_usb_time_temp_begin = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
                pm_printk(0, "## A_I %d\n", u32_usb_time_temp_begin);
#endif
                u32_usb_time_temp = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
        }

        if(rtk_usb_customized_init() == 0)
                return 0;

        pm_printk(0, "usb init\n");

        dwc3_memset((void *)&g_rtk_dwc3, 0, sizeof(g_rtk_dwc3));
        enter_u0_count = 0;
        g_ss_inact_cnt = 0;
        g_rxdet_cnt = 0;
        g_dwc3_set_config_done = 0;

        rtk_usb_driver_check_sw_config();

#ifdef CONFIG_ARCH_5281
        rtd_outl(SYS_REG_INT_CTRL_SCPU2_2_reg,
                 SYS_REG_INT_CTRL_SCPU2_2_usb_int_scpu2_routing_en_mask);
#endif
        //rtk_dwc3_crt_init();
        //usb_mac_init();
        rtk_dwc3_usb_load_phy();

        g_usb_iram = usb_addr_vir;
        g_usb_iram_dma = usb_addr_phy;

        if(!g_usb_iram)
        {
                //pm_printk(LOGGER_ERROR, " %s %dmalloc fail\n", __FUNCTION__, __LINE__);
                return -1;
        }
        g_usb_cur_alloc_base = g_usb_iram;

        rtk_dwc3_probe();
        // fw queue
        //core_fw_msg_queue_addEvent(MSG_UVC_ID, 0, 0);
        core_fw_msg_queue_addEvent(MSG_USB_ID, MSG_USB_ENUMLATION, 0);
        core_timer_event_addTimerEvent(100, MSG_USB_ID, MSG_USB_CONNECT_TIMEOUT, 0);
        core_timer_event_addTimerEvent(1000, MSG_USB_ID, MSG_USB_MONITOR, 0);

        rtk_uac_disconnect_flow_pd();
#ifndef CONFIG_ARCH_5281
        // timer queue
        //core_timer_event_addTimerEvent(100, MSG_USB_ID, MSG_USB_ENUMLATION, 0);
#endif
        if(u8_need_calculate_usb_boot_time == 1)
        {
                u32_usb_time_temp = rtd_inl(TIMER_SCPU_CLK90K_LO_reg) - u32_usb_time_temp;
                u32_usb_time =  u32_usb_time + u32_usb_time_temp;
#ifdef DWC3_RTK_ENABLE_USB_BOOT_TIME_DEBUG_LOG
                u32_usb_time_temp_end = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
                pm_printk(0, "## A_O %d %d", u32_usb_time_temp_end, (u32_usb_time_temp_end - u32_usb_time_temp_begin) / 90);
                pm_printk(0, "     %d\n", u32_usb_time / 90);
#endif
        }

        return 0;
}

UINT8 usb_driver_init(void) __banked
{
        return rtk_usb_driver_init();
}

UINT8 rtk_usb_driver_connect(void) __banked
{
        pm_printk(0, "#connect\n");
        if(rtk_dwc3_is_usb_control_by_application() == TRUE) {
            // The PHY setting maybe changed by DP
            // So need to run USB init flow again
            rtk_usb_driver_reinit();
        } else {
            rtk_dwc3_gadget_run_stop(1);
        }
        return 0;
}

UINT8 rtk_usb_driver_disconnect(void) __banked
{
        pm_printk(0, "#disconnect\n");
        rtk_dwc3_gadget_run_stop(0);
        rtk_usb3_phy_reset();
        wait_do_control_transfer = 0;
        return 0;
}

UINT8 rtk_usb_driver_suspend(void)
{
        /*
            UINT8 ret;

            pm_printk(LOGGER_ERROR, "rtk_usb_driver_suspend\n");

            ret = rtk_dwc3_gadget_run_stop(false);
            if (ret < 0) {
                    pm_printk(LOGGER_ERROR, "failed to stop\n");
                    return -1;
            }


            //disable dwc3 irqs
            rtd_outl(DWC3_DEVTEN, 0);

            if (__rtk_dwc3_gadget_ep_disable(0) )
            {
                    pm_printk(LOGGER_ERROR, "failed to disable ep %d\n", g_rtk_dwc3.eps[0].number);
                    return false;
            }

            if (__rtk_dwc3_gadget_ep_disable(1))
            {
                    pm_printk(LOGGER_ERROR, "failed to disable ep %d\n", g_rtk_dwc3.eps[0].number);
                    return false;
            }
        */
        return 0;
}

UINT8 usb_driver_suspend(void) __banked
{
        return rtk_usb_driver_suspend();
}

UINT8 rtk_usb_driver_resume(void)
{
        return 0;
}

UINT8 usb_driver_resume(void) __banked
{
        return rtk_usb_driver_resume();
}

UINT8 rtk_usb_customized_init(void)
{
#if 0
//****************************************************************************
// for customized flow
//****************************************************************************
        // 1. PD disable mode
        if(usb_sw_cfg_GetPD_Mode_BANK41() == _PD_FUNCTION_DISABLE) {
                // Check GPIO06 detect pin
                if(Get_PIN_GPIO06_Mode() == _PIN_GPIO06_TYPEC_DRP_DET) {
                    if((rtd_inl(MIS_GPIO_GP0DATI_reg) & _BIT7) == 0x0) {
                            // Sent USB_CONNECT event
                            core_fw_msg_queue_cancelEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT);
                            core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT, (UINT32)MSG_USB_CONNECT_SPEED_UNKNOW);
                            return 0;
                    }
                }
                // Check GPIO13 detect pin
                if(Get_PIN_GPIO13_Mode() == _PIN_GPIO13_TYPEC_DRP_DET) {
                    return 1;
                }
                return 1;
        }
#else
        RTK_USB_DEVICE_DBG_SIMPLE(  "rtk_usb_device ###### rtk_usb_customized_init need to implement\n");
#endif
        return 1;
}

void rtk_uac_disconnect_flow_isr(void) __banked
{
        // Check GP_06 H/L
        if(usb_sw_cfg_GetPD_Mode_BANK41() == _PD_FUNCTION_DISABLE)
        {
                if((rtd_inl(MIS_GPIO_GP0DATI_reg) & _BIT7) == 0x0) {
                        if((rtd_inl(SYS_REG_SYS_CLKEN2_reg) & _BIT18) == _BIT18) {
                                if((rtd_inl(SYS_REG_SYS_CLKEN1_reg) & _BIT16) == _BIT16) {
                                        dwc3_disable_uac_timer();
                                        usb_uac_out_disconnect(6);
                                        usb_uac_in_disconnect(_UAC_OUT0);
                                        usb_uac_in_disconnect(_UAC_OUT1);
                                        audio_uac_in_enable(0);
                                        __rtk_dwc3_gadget_ep_disable(6);
                                        ///////////USB CRT setting /////
                                        rtd_outl(SYS_REG_SYS_CLKEN2_reg, 0x00040000);   //bit[18]=0, MAC CLKEN_U3
                                        delayms(1);
                                        rtd_outl(SYS_REG_SYS_SRST2_reg, 0x00040000);   //bit[18]=0, MAC RSTN_Reset
                                        delayms(1);
                                        rtd_outl(SYS_REG_SYS_SRST6_reg, 0x00000018);   //bit[4,3]=0,PHY RSTN_combo/rbus
                                        delayms(1);
                                        rtd_outl(SYS_REG_SYS_SRST3_reg, 0x00000034);   //bit[5,4,2]=0,Reset_U2 PHY0/1,U3 PHY => old U3 PHY no use
                                        delayms(1);
                                        rtd_outl(SYS_REG_SYS_CLKEN6_reg, 0x00000010);   //bit[4]=0, DP combo U3 phy CLKEN
                                        delayms(1);
                                        core_timer_event_cancelTimerEvent(MSG_USB_ID, MSG_USB_MONITOR);
                                }
                        }
                }
        }
}

void rtk_uac_disconnect_flow(void)
{
        // for UAC dissconnect flow
        if(rtd_inl(SYS_REG_SYS_CLKEN1_reg) & _BIT16) {
                rtd_outl(0xb8069174, 0x00000010); // usb sent mframe
                UAC_IN_freq_type = _48K_16B;
                UAC_OUT0_freq_type = _48K_16B;
                UAC_OUT1_freq_type = _48K_16B;

                if(usb_sw_cfg_GetUAC_Enable_BANK41() == _FUNCTION_ENABLE) {
                        if(usb_sw_cfg_GetAudio_PCM_Audio_Function_BANK41() == _AUDIO_48K_ONLY)
                                usb_uac_alloc(_48K_16B, _48K_16B);  // Max. alloc
                        else if(usb_sw_cfg_GetAudio_PCM_Audio_Function_BANK41() == _AUDIO_WITH_96K)
                                usb_uac_alloc(_96K_16B, _96K_16B);  // Max. alloc
                        else if(usb_sw_cfg_GetAudio_PCM_Audio_Function_BANK41() == _AUDIO_WITH_192K)
                                usb_uac_alloc(_192K_16B, _192K_16B);  // Max. alloc
                        usb_uac_in_sram_init(UAC_IN_freq_type);
                        usb_uac_out0_sram_init(UAC_OUT0_freq_type);
                        usb_uac_out1_sram_init(UAC_OUT1_freq_type);
                }
        }
}

void rtk_uac_disconnect_flow_pd(void)
{
#if 0
        if(usb_sw_cfg_GetPD_Mode_BANK41() == _PD_FUNCTION_DISABLE) {
                rtk_uac_disconnect_flow();
        }
#else
        RTK_USB_DEVICE_DBG_SIMPLE(  "rtk_usb_device ###### rtk_uac_disconnect_flow_pd need to implement\n");
#endif
}

void rtk_uvc_disconnect_flow(void)
{
        // for UVC dissconnect flow
        if(rtd_inl(SYS_REG_SYS_CLKEN4_reg) & _BIT2) {
                core_timer_event_cancelTimerEvent(MSG_USB_ID, MSG_USB_UVC_UPDATE_TRANSFER);
                if(usb_sw_cfg_GetUVC_Enable_BANK41() == _FUNCTION_ENABLE) {
                        usb_uvc_disconnect(g_rtk_dwc3.eps[UVC_EP_NUMBER].number, 1);
                        core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_UVC_DISCONNECT, 0);
                }
        }
}

void reset_uac_proc(void) __banked
{
        if(usb_sw_cfg_GetPD_Mode_BANK41() == _PD_FUNCTION_DISABLE)
        {
                if((rtd_inl(MIS_GPIO_GP0DATI_reg) & _BIT7) == 0x0)
                        return;
        }

        pm_printk(LOGGER_ERROR, "re uac ep\n");
        usb_uac_out_disconnect(6);
        audio_uac_in_enable(0);
        //__rtk_dwc3_gadget_ep_disable_uac(6);
        rtk_dwc3_stop_active_transfer(6);
        usb_uac_in_sram_init(UAC_IN_freq_type);
        delayms(50);
        //_rtk_dwc3_gadget_ep_enable(6, &uac_hs_epout_desc, NULL, false);
        //rtk_dwc3_ep_start_trans_uac(UAC1_IN_EP_NUMBER, 0);
        usb_uac_out_connect(6);
        audio_uac_in_enable(1);
}

void rtk_dwc3_init_uac_check_trb_cnt(void)
{
        if( usb_sw_cfg_Get_Chip_version_BANK41() <= Version_B ) {
                uac_device_to_host_trb_cnt   = 32;
                uac_device_to_host_trb_cnt_2 = 32;
        }
}

void rtk_dwc3_init_uac(void)
{
        rtk_dwc3_init_uac_check_trb_cnt();
        usb_uac_init();
}

void usb_dwc3_init_uac(void) __banked
{
        rtk_dwc3_init_uac();
        rtk_dwc3_init_uac_show_info();
}

UINT8 rtk_usb_driver_reinit(void)
{
        pm_printk(0, "rtk_usb_driver_reinit\n");
        dwc3_disable_uac_timer();
        rtk_uac_disconnect_flow();
        rtk_uvc_disconnect_flow();
        u8_need_calculate_usb_boot_time = 0;
        core_timer_event_cancelTimerEvent(MSG_USB_ID, MSG_USB_MONITOR);
        rtk_usb_driver_init();
        u8_need_calculate_usb_boot_time = 1;
        usb_dwc3_init_uac();
        g_rtk_dwc3.speed = USB_SPEED_UNKNOWN;
        g_dwc3_is_reinit = 1;
        compare_UsbSof_LocalCouner_reset();
        if(rtk_dwc3_is_usb_control_by_application() == TRUE) {
            rtk_dwc3_gadget_run_stop(1);
        }
        return 0;
}

void rtk_usb_driver_reinit_without_usb(void)
{
    g_rtk_dwc3.speed = USB_SPEED_UNKNOWN;
    core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT, (UINT32)MSG_USB_CONNECT_SPEED_UNKNOW);
#if 0
    if((rtd_inl(SYS_REG_SYS_CLKEN2_reg) & _BIT18) == _BIT18)
    {
            if((rtd_inl(SYS_REG_SYS_CLKEN1_reg) & _BIT16) == _BIT16) {
                dwc3_disable_uac_timer();
                usb_uac_out_disconnect(6);
                usb_uac_in_disconnect(_UAC_OUT0);
                usb_uac_in_disconnect(_UAC_OUT1);
                rtk_uac_disconnect_flow();
                rtk_uvc_disconnect_flow();
                usb_dwc3_init_uac();
            }
    }
#endif
    compare_UsbSof_LocalCouner_reset();
}

UINT8 GetUSBForceConnectSpeed(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE(  "rtk_usb_device ### GetUSBForceConnectSpeed need to implement ----------------\n");
    return 0;
}




UINT8 Get_Package_Type(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE(  "rtk_usb_device ### Get_Package_Type need to implement ----------------\n");
    return 0;
}


void core_fw_msg_queue_cancelEvent(UINT32 a, UINT32 b)
{
    RTK_USB_DEVICE_DBG_SIMPLE(  "rtk_usb_device ### core_fw_msg_queue_cancelEvent need to implement ----------------\n");
}

void core_fw_msg_queue_addEvent(UINT32 a, UINT32 b, UINT32 c)
{
    RTK_USB_DEVICE_DBG_SIMPLE(  "rtk_usb_device ### core_fw_msg_queue_addEvent need to implement ----------------\n");
    // send uvcid and timing infor to AP
}

void core_timer_event_addTimerEvent(UINT32 a, UINT32 b, UINT32 c, UINT32 d)
{
    RTK_USB_DEVICE_DBG_SIMPLE(  "rtk_usb_device ### core_timer_event_AddTimerEvent need to implement ----------------\n");
}

void core_timer_event_cancelTimerEvent(UINT32 a, UINT32 b)
{
    RTK_USB_DEVICE_DBG_SIMPLE(  "rtk_usb_device ### core_timer_event_cancelTimerEvent need to implement ----------------\n");
}

void audio_uac_in_enable(UINT8 isEnable)
{
    RTK_USB_DEVICE_DBG_SIMPLE(  "rtk_usb_device ### audio_uac_in_enable need to implement ----------------\n");
}


UINT8 GetPD_Mode(void)
{
    return 0;
}

UINT8 Get_Chip_version(void)
{
    return Version_B;
}

UINT8 Get_UVC_Format_MJPEG_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 Get_UVC_Format_YUV2_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 GetUVC_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 Get_UVC_Format_RGB32_Enable(void)
{
    return 0;
}

UINT8 Get_UVC_Format_RGB24_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 Get_UVC_Format_P010_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 Get_UVC_Format_NV12_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 Get_UVC_Format_I420_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 Get_UVC_Format_M420_Enable(void)
{
    return 0;
}

UINT8 GetAudio_PCM_Audio_Function(void)
{
    return _AUDIO_48K_ONLY;
}

UINT8 GetUAC_UpStream_UAC2_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 GetUAC_UpStream_UAC1_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 GetUAC_DownStream_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 GetUAC_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 Get_PID(void)
{
    return 0;
}

UINT8 Get_CID(void)
{
    return 0;
}

UINT8 Get_UVC_Control_Processing_Unit_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 Get_USB_HID_Enable(void)
{
    return _FUNCTION_ENABLE;
}

UINT8 Get_Ext_DP_MUX_Select(void)
{
    return _Ext_DP_MUX_Select_RTS5450M;
}

UINT8 GetTypeC_Fuction(void)
{
    return _TYPEC_USB2_USB32_DP_1_4;
}

UINT8 _rtice_input_ex(UINT16 data)
{
    int i;
    RTK_USB_DEVICE_DBG_SIMPLE(  "_rtice_input_ex\n");
#if 1
    for(i=0;i<data; i++) {
        rtice_at_input(g_recv_buf[i]);
    }
    return 0;
#else
    RTK_USB_DEVICE_DBG_SIMPLE(  "_rtice_input_ex lenth =%d\n", data);
    RTK_USB_DEVICE_DBG_SIMPLE(  "data %02x %02x %02x %02x %02x %02x %02x %02x\n", g_recv_buf[0], g_recv_buf[1], g_recv_buf[2], g_recv_buf[3], g_recv_buf[4], g_recv_buf[5], g_recv_buf[6], g_recv_buf[7]);
    if(g_recv_buf[0] == 0xa0) {

        for(int i=0;i<137;i ++)
            g_recv_buf[i] = 0;

        g_recv_buf[0] = 0xa0;
        g_recv_buf[1] = 0x06;
        g_recv_buf[2] = 0x00;
        g_recv_buf[3] = 0x00;
        g_recv_buf[4] = 0x01;
        g_recv_buf[5] = 0x00;
        g_recv_buf[6] = 0x00;
        g_recv_buf[7] = 0x00;
        g_recv_buf[8] = 0x59;
        g_uvc_extension.log_buf.write_pos = 9;

    } else {
        // AT cmd 121
        for(int i=0;i<137;i ++)
            g_recv_buf[i] = 0;

        g_recv_buf[0] = 0xa1;
        g_recv_buf[1] = 0x80;
        g_recv_buf[2] = 0x85;
        g_recv_buf[3] = 0x00;

        g_recv_buf[4] = 0x01;
        g_recv_buf[5] = 0x00;
        g_recv_buf[6] = 0x00;
        g_recv_buf[7] = 0x00;

        g_recv_buf[8]  = 0x01;
        g_recv_buf[9]  = 0x00;
        g_recv_buf[10] = 0x00;
        g_recv_buf[11] = 0x00;

        g_recv_buf[136] = 0x58;
        g_uvc_extension.log_buf.write_pos = 137;
    }
    return 0;
#endif
}


#ifdef USB_DEVICE_UAC_DATA_GEN
UINT8 audio_1khz[192] = {
0x4f, 0xf8, 0x4e, 0xf8, 0x29, 0xf7, 0x28, 0xf7, 0x2a, 0xf6, 0x2a, 0xf6, 0x57, 0xf5, 0x57, 0xf5,
0xb2, 0xf4, 0xb2, 0xf4, 0x3f, 0xf4, 0x3e, 0xf4, 0xff, 0xf3, 0xfe, 0xf3, 0xf4, 0xf3, 0xf4, 0xf3,
0x1d, 0xf4, 0x1d, 0xf4, 0x7a, 0xf4, 0x7a, 0xf4, 0x0a, 0xf5, 0x0a, 0xf5, 0xcb, 0xf5, 0xca, 0xf5,
0xb8, 0xf6, 0xb7, 0xf6, 0xcc, 0xf7, 0xcd, 0xf7, 0x05, 0xf9, 0x06, 0xf9, 0x5e, 0xfa, 0x5e, 0xfa,
0xce, 0xfb, 0xcf, 0xfb, 0x52, 0xfd, 0x51, 0xfd, 0xe0, 0xfe, 0xe0, 0xfe, 0x73, 0x00, 0x73, 0x00,
0x05, 0x02, 0x05, 0x02, 0x8d, 0x03, 0x8e, 0x03, 0x07, 0x05, 0x06, 0x05, 0x6a, 0x06, 0x6a, 0x06,
0xb1, 0x07, 0xb1, 0x07, 0xd6, 0x08, 0xd6, 0x08, 0xd5, 0x09, 0xd5, 0x09, 0xa9, 0x0a, 0xa8, 0x0a,
0x4e, 0x0b, 0x4d, 0x0b, 0xc1, 0x0b, 0xc1, 0x0b, 0x01, 0x0c, 0x01, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
0xe3, 0x0b, 0xe2, 0x0b, 0x85, 0x0b, 0x84, 0x0b, 0xf5, 0x0a, 0xf5, 0x0a, 0x35, 0x0a, 0x34, 0x0a,
0x48, 0x09, 0x47, 0x09, 0x32, 0x08, 0x33, 0x08, 0xf9, 0x06, 0xf8, 0x06, 0xa1, 0x05, 0xa0, 0x05,
0x31, 0x04, 0x31, 0x04, 0xae, 0x02, 0xae, 0x02, 0x1f, 0x01, 0x1f, 0x01, 0x8c, 0xff, 0x8b, 0xff,
0xfb, 0xfd, 0xfa, 0xfd, 0x72, 0xfc, 0x71, 0xfc, 0xf7, 0xfa, 0xf9, 0xfa, 0x95, 0xf9, 0x96, 0xf9,
};

typedef struct _tagUacRingBufferHeader {
	void*   beginAddr;
    unsigned long   beginAddr_phy; // phy is for audio FW , if needs
	unsigned long   size;
	unsigned long   writePtr_offset;// 0 means wp = beginAddr+0
	unsigned long   readPtr_offset; // 4 means rp = beginAddr+4
} UAC_RINGBUFFER_HEADER ;


// input device type
typedef enum {
    UAC_DEVICE_IN = 0,
    UAC_DEVICE_OUT = 0,
} UAC_DEVICE_LIST;

void rtkaudio_uac_init(UAC_DEVICE_LIST device, UAC_RINGBUFFER_HEADER* uac_ring_header, UAC_RINGBUFFER_HEADER* uac_ring_header_phy);
void rtkaudio_uac_reset(UAC_DEVICE_LIST device );
void rtkaudio_uac_enable(UAC_DEVICE_LIST device, int isEnable);

#define MARK2_DUMMY_01 0xb805b1fc
#define MARK2_DUMMY_02 0xb805b1f8


UINT8 uac_thread_runing = 0;
void rtkaudio_uac_set_drift(UAC_DEVICE_LIST device, int step);

typedef struct
{
    unsigned long pHaddr[2];
    unsigned long vHaddr[2];
    unsigned long pBaddr[2];
    unsigned long vBaddr[2];
    unsigned long outPinID;
    unsigned long ao_focus;
    unsigned long channel;
    unsigned long samplerate;
    unsigned long interleave;
} HAL_AUDIO_UACOUT_INFO_T;

int RHAL_AUDIO_UACOUT_INIT_RINGBUFFER(HAL_AUDIO_UACOUT_INFO_T *uacinfo);
UINT32 RHAL_AUDIO_UACOUT_Run(HAL_AUDIO_UACOUT_INFO_T *uacinfo);
UINT32 RHAL_AUDIO_SetASRCDrift(float speed, float count_duration, float stc_duration, int pinId);


u32 USB_Device_reverseInteger(u32 value)
{
	unsigned long b0 =  value & 0x000000ff;
	unsigned long b1 = (value & 0x0000ff00) >> 8;
	unsigned long b2 = (value & 0x00ff0000) >> 16;
	unsigned long b3 = (value & 0xff000000) >> 24;

	return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3 ;
}
unsigned long long USB_Device_reverseLongInteger(unsigned long long value)
{
	unsigned long long ret;
	unsigned char *des, *src;
	src = (unsigned char *)&value;
	des = (unsigned char *)&ret;
	des[0] = src[7];
	des[1] = src[6];
	des[2] = src[5];
	des[3] = src[4];
	des[4] = src[3];
	des[5] = src[2];
	des[6] = src[1];
	des[7] = src[0];
	return ret;
}

float test_data[][2] = { { 501.0 , 501.0},
                         { 10001.0 , 10001.0},
                         { 20001.0 , 20001.0},
                         { 30001.0 , 30001.0},
                         { 40001.0 , 40001.0},
                         { 50001.0 , 50001.0},
                         { 60001.0 , 60001.0},
                         { 70001.0 , 70001.0},
                         { 80001.0 , 80001.0},
                         { 930001.0 , 90001.0}};
int uac_thread(void *data)
{
#define UAC_BUFFER (30*1024)
    UINT8 *wp;
    UINT8 i;
	unsigned long flags;

    void *vir_addr = NULL;
    void *unvir_addr = NULL;
    unsigned long phy_addr = 0;
    UINT32 *ptemp_u32;
    unsigned long temp_long;
    unsigned long audio_ring_buffer_size;
    unsigned char show_uac_buffer_log_cnt = 0;
    int uac_step = 0;
    UINT8 temp;
    HAL_AUDIO_UACOUT_INFO_T uac_info;
    RINGBUFFER_HEADER   *m_pBufferHeader;
    UINT32 temp1;
    UINT32 temp2;
    UINT32 audio_buffer_size;
    UINT8 *pwrite;
    UINT32 temp3;
    UINT8 *p_non_cache_Virtul_B_write;
    UINT8 already_set_audio = 0;
    struct file *pFile;
    loff_t file_offset = 0;

    UINT32 max_size = 0;
    UINT32 min_size = 0;
    UINT16 show_log_cnt = 0;

    UINT8 speed = 5;

    if(uac_thread_runing == 1)
        return 0;

    uac_thread_runing = 1;

    (void)uac_step;
    (void)temp_long;
    (void)audio_ring_buffer_size;

start_run_uac:
    RTK_USB_DEVICE_DBG_SIMPLE( "uac_thread %d\n", __LINE__);
    while(1) {
        if(run_uac ==1 )
            break;
        usleep_range(1000, 1000);
    }

    RTK_USB_DEVICE_DBG_SIMPLE( "uac_thread %d\n", __LINE__);

    if(already_set_audio == 0 ) {
        uac_info.channel  = 2;
        uac_info.samplerate = 48000;
        uac_info.interleave = 1;

        RHAL_AUDIO_UACOUT_INIT_RINGBUFFER(&uac_info);

        //RHAL_AUDIO_SetASRCDrift(0, 90000, 48000, uac_info.outPinID);

        RHAL_AUDIO_UACOUT_Run(&uac_info);

        RTK_USB_DEVICE_DBG_SIMPLE("[UAC] uac_info.pHaddr %lx\n", uac_info.pHaddr[0]);
        RTK_USB_DEVICE_DBG_SIMPLE("[UAC] uac_info.vHaddr %lx\n", uac_info.vHaddr[0]);

        RTK_USB_DEVICE_DBG_SIMPLE("[UAC] uac_info.pBaddr %lx\n", uac_info.pBaddr[0]);
        RTK_USB_DEVICE_DBG_SIMPLE("[UAC] uac_info.vBaddr %lx\n", uac_info.vBaddr[0]);

        // for 1k tone buffer
        vir_addr = dvr_malloc_uncached_specific(UAC_BUFFER, GFP_DCU2, &unvir_addr);
        if (!vir_addr) {
            RTK_USB_DEVICE_DBG_SIMPLE("[UAC] %d alloc memory fail\n", __LINE__);
            return 0;
        }
        phy_addr = (unsigned long) dvr_to_phys(vir_addr);

        already_set_audio = 1;
    }

    m_pBufferHeader = (RINGBUFFER_HEADER*)(uac_info.vHaddr[0]);

    RTK_USB_DEVICE_DBG_SIMPLE("[UAC] m_pBufferHeader.magic %x\n",        USB_Device_reverseInteger(m_pBufferHeader->magic));
    RTK_USB_DEVICE_DBG_SIMPLE("[UAC] m_pBufferHeader.beginAddr %x\n",    USB_Device_reverseInteger(m_pBufferHeader->beginAddr));
    RTK_USB_DEVICE_DBG_SIMPLE("[UAC] m_pBufferHeader.size %x\n",         USB_Device_reverseInteger(m_pBufferHeader->size));
    RTK_USB_DEVICE_DBG_SIMPLE("[UAC] m_pBufferHeader.bufferID %x\n",     USB_Device_reverseInteger(m_pBufferHeader->bufferID));
    RTK_USB_DEVICE_DBG_SIMPLE("[UAC] m_pBufferHeader.writePtr %x\n",     USB_Device_reverseInteger(m_pBufferHeader->writePtr));
    RTK_USB_DEVICE_DBG_SIMPLE("[UAC] m_pBufferHeader.numOfReadPtr %x\n", USB_Device_reverseInteger(m_pBufferHeader->numOfReadPtr));
    RTK_USB_DEVICE_DBG_SIMPLE("[UAC] m_pBufferHeader.readPtr[0] %x\n",   USB_Device_reverseInteger(m_pBufferHeader->readPtr[0]));
    RTK_USB_DEVICE_DBG_SIMPLE("[UAC] m_pBufferHeader.readPtr[1] %x\n",   USB_Device_reverseInteger(m_pBufferHeader->readPtr[1]));
    RTK_USB_DEVICE_DBG_SIMPLE("[UAC] m_pBufferHeader.readPtr[2] %x\n",   USB_Device_reverseInteger(m_pBufferHeader->readPtr[2]));
    RTK_USB_DEVICE_DBG_SIMPLE("[UAC] m_pBufferHeader.readPtr[3] %x\n",   USB_Device_reverseInteger(m_pBufferHeader->readPtr[3]));

    temp1 = USB_Device_reverseInteger(m_pBufferHeader->beginAddr);
    temp2 = USB_Device_reverseInteger(m_pBufferHeader->writePtr);

    RTK_USB_DEVICE_DBG_SIMPLE( "uac_thread %d m_pBufferHeader->beginAddr = 0x%x\n", __LINE__, temp1);
    RTK_USB_DEVICE_DBG_SIMPLE( "uac_thread %d m_pBufferHeader->writePtr  = 0x%x\n", __LINE__, temp2);

    temp2 = temp2 - temp1;
    temp2 = temp2 / 192;
    temp2 = temp2 * 192;
    temp2 =  temp1 + temp2;
    m_pBufferHeader->readPtr[0] = USB_Device_reverseInteger(temp2);
    RTK_USB_DEVICE_DBG_SIMPLE( "uac_thread %d m_pBufferHeader->readPtr[0]  = 0x%x\n", __LINE__, temp2);

    for(i=0;i<uac_device_to_host_trb_cnt;i++ ) {
        memcpy(unvir_addr + i*g_UAC_OUT0_base_mps_size, audio_1khz, 192);
    }
    wp = uac_device_to_host_trb_addr_vir_non_cache;

    (void)pwrite;
    (void)temp3;
    (void)pFile;
    (void)file_offset;

    p_non_cache_Virtul_B_write =  (UINT8*)uac_info.vBaddr[0];
    while(1) {

        while(1) {
            //RTK_USB_DEVICE_DBG_SIMPLE("uac %px %x\n", wp, temp);
            temp = *(wp+12);

            if(rtd_inl(0xb8004ae8) == 0x00000001 ) {
                UINT32 smaple_value;
                smaple_value = rtd_inl(0xb8004aec);
                if(smaple_value == 0) {
                    RHAL_AUDIO_SetASRCDrift(89995.0, 89995.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 89995.0 89995.0 90000\n");
                }
                if(smaple_value == 1) {
                    RHAL_AUDIO_SetASRCDrift(89996.0, 89996.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 89996.0 89996.0 90000\n");
                }
                if(smaple_value == 2) {
                    RHAL_AUDIO_SetASRCDrift(89997.0, 89997.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 89997.0 89997.0 90000\n");
                }
                if(smaple_value == 3) {
                    RHAL_AUDIO_SetASRCDrift(89998.0, 89998.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 89998.0 89998.0 90000\n");
                }
                if(smaple_value == 4) {
                    RHAL_AUDIO_SetASRCDrift(89999.0, 89999.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 89999.0 89999.0 90000\n");
                }
                if(smaple_value == 5) {
                    RHAL_AUDIO_SetASRCDrift(90000.0, 90000.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90000.0 90000.0 90000\n");
                }
                if(smaple_value == 6) {
                    RHAL_AUDIO_SetASRCDrift(90001.0, 90001.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90001.0 90001.0 90000\n");
                }
                if(smaple_value == 7) {
                    RHAL_AUDIO_SetASRCDrift(90002.0, 90002.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90002.0 90002.0 90000\n");
                }
                if(smaple_value == 8) {
                    RHAL_AUDIO_SetASRCDrift(90003.0, 90003.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90003.0 90003.0 90000\n");
                }
                if(smaple_value == 9) {
                    RHAL_AUDIO_SetASRCDrift(90004.0, 90004.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90004.0 90004.0 90000\n");
                }
                rtd_outl(0xb8004ae8, 0);
            }

            if(rtd_inl(0xb8004ae8) == 0x00000002 ) {
                UINT32 smaple_value;
                smaple_value = rtd_inl(0xb8004aec);
                if(smaple_value == 0) {
                    RHAL_AUDIO_SetASRCDrift(90000.0, 89995.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90000.0 89995.0 90000\n");
                }
                if(smaple_value == 1) {
                    RHAL_AUDIO_SetASRCDrift(90000.0, 89996.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90000.0 89996.0 90000\n");
                }
                if(smaple_value == 2) {
                    RHAL_AUDIO_SetASRCDrift(90000.0, 89997.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90000.0 89997.0 90000\n");
                }
                if(smaple_value == 3) {
                    RHAL_AUDIO_SetASRCDrift(90000.0, 89998.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90000.0 89998.0 90000\n");
                }
                if(smaple_value == 4) {
                    RHAL_AUDIO_SetASRCDrift(90000.0, 89999.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90000.0 89999.0 90000\n");
                }
                if(smaple_value == 5) {
                    RHAL_AUDIO_SetASRCDrift(90000.0, 90000.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90000.0 90000.0 90000\n");
                }
                if(smaple_value == 6) {
                    RHAL_AUDIO_SetASRCDrift(90000.0, 90001.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90000.0 90001.0 90000\n");
                }
                if(smaple_value == 7) {
                    RHAL_AUDIO_SetASRCDrift(90000.0, 90002.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90000.0 90002.0 90000\n");
                }
                if(smaple_value == 8) {
                    RHAL_AUDIO_SetASRCDrift(90000.0, 90003.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90000.0 90003.0 90000\n");
                }
                if(smaple_value == 9) {
                    RHAL_AUDIO_SetASRCDrift(90000.0, 90004.0 * 3,90000.0 * 3, uac_info.outPinID);
                    RTK_USB_DEVICE_DBG_SIMPLE("=======  use 90000.0 90004.0 90000\n");
                }
                rtd_outl(0xb8004ae8, 0);
            }

            if( (temp & 0x1)==0x0 ) {

                if(show_uac_buffer_log_cnt > 100) {
                    show_uac_buffer_log_cnt = 0;
                }
                show_uac_buffer_log_cnt++;

                temp1 = USB_Device_reverseInteger(m_pBufferHeader->writePtr);
                temp2 = USB_Device_reverseInteger(m_pBufferHeader->readPtr[0]);
                temp3 = temp2 - USB_Device_reverseInteger(m_pBufferHeader->beginAddr);

                if(temp1 >= temp2)
                    audio_buffer_size = temp1 - temp2;
                else
                    audio_buffer_size = (temp1 + USB_Device_reverseInteger(m_pBufferHeader->size)) - temp2;

                if(audio_buffer_size > max_size)
                    max_size = audio_buffer_size;
                if(audio_buffer_size < min_size)
                    min_size = audio_buffer_size;
                show_log_cnt++;
                if(show_log_cnt > 1000) {
                    RTK_USB_DEVICE_DBG_SIMPLE("UAC max=%d min=%d\n", max_size, min_size);

                    if(min_size < 1000) {
                        // audio become fast
                        speed = 4;
                    }

                    if(min_size > 2000) {
                        // audio become slow
                        speed = 5;
                    }

                    if(speed == 4) {
                        // audio become fast
                        RHAL_AUDIO_SetASRCDrift(90000.0, 89999.0 * 3,90000.0 * 3, uac_info.outPinID);
                        RTK_USB_DEVICE_DBG_SIMPLE("ASRC use 90000.0 89999.0 90000\n");
                    }

                    if(speed == 5) {
                        // audio become slow
                        RHAL_AUDIO_SetASRCDrift(90000.0, 89995.0 * 3,90000.0 * 3, uac_info.outPinID);
                        RTK_USB_DEVICE_DBG_SIMPLE("ASRC  use 90000.0 89995.0 90000\n");
                    }

                    show_log_cnt = 0;
                    max_size     = 0;
                    min_size     = 30*1024;

                }

                //RTK_USB_DEVICE_DBG_SIMPLE("w=0x%x r=0x%x audio_buffer_size=%d\n", temp1, temp2, audio_buffer_size);
                //RTK_USB_DEVICE_DBG_SIMPLE("%d\n", audio_buffer_size);

                if(audio_buffer_size >= 192) {
                    UINT8 *temp_u8;
                    UINT8  temp_swap1;
                    temp_u8 = p_non_cache_Virtul_B_write + temp3;

                    for(i=0;i< 192; i=i+2) {
                        temp_swap1 = temp_u8[i+0];
                        temp_u8[i+0] = temp_u8[i+1];
                        temp_u8[i+1] = temp_swap1;
                    }
                    ptemp_u32 = (UINT32*)wp;
                    *(ptemp_u32+0)  = temp2;
                    *(wp+8)  = 0xc0;
                    *(wp+12) = 0x69;
                    wp = wp + 0x10;

                    temp2 = temp2 + 192;
                    if(temp2 >= (USB_Device_reverseInteger(m_pBufferHeader->beginAddr) + USB_Device_reverseInteger(m_pBufferHeader->size))) {
                        temp2 = USB_Device_reverseInteger(m_pBufferHeader->beginAddr);
                        //RTK_USB_DEVICE_DBG_SIMPLE("wrap\n");
                    }
                    m_pBufferHeader->readPtr[0] = USB_Device_reverseInteger(temp2);
                } else {
                    ptemp_u32 = (UINT32*)wp;
                    *(ptemp_u32+0)  = phy_addr;
                    *(wp+8)  = 0xc0;
                    *(wp+12) = 0x69;
                    wp = wp + 0x10;
                    RTK_USB_DEVICE_DBG_SIMPLE("underflow\n");
                }
                if(wp >= (uac_device_to_host_trb_addr_vir_non_cache + uac_device_to_host_trb_cnt*0x10)) {
                    wp = uac_device_to_host_trb_addr_vir_non_cache;
                    //RTK_USB_DEVICE_DBG_SIMPLE("uac wrap %px m_pBufferHeader=%x\n", uac_device_to_host_trb_addr_vir_non_cache, USB_Device_reverseInteger(m_pBufferHeader->writePtr));
                }

                spin_lock_irqsave(&g_rtk_dwc3.usb_device_mac_lock, flags);
                rtd_outl(0xb80ac87c, 0x00040507);
                spin_unlock_irqrestore(&g_rtk_dwc3.usb_device_mac_lock, flags);

            }

            if(uac_stream_in0_inf_alt == 0 ) {
                usleep_range(100, 100);
                RTK_USB_DEVICE_DBG_SIMPLE( "uac_thread %d\n", __LINE__);
                goto start_run_uac;
            }
            
            if(run_uac == 0) {
                usleep_range(100, 100);
                RTK_USB_DEVICE_DBG_SIMPLE( "uac_thread %d\n", __LINE__);
                goto start_run_uac;
            }

            usleep_range(100, 100);

        }

    }
    return 0;
}
#else
int uac_thread(void *data)
{
    while(1) {
        usleep_range(1000, 1000);
    }
    return 0;
}
#endif

void create_uac_thread(void)
{
    struct task_struct *uac_thread_handle = NULL;
    uac_thread_handle = kthread_run(uac_thread, NULL, "uac_thread");
}

#ifndef VIDEO_RECT_T
typedef struct
{
    unsigned short		x;	/**< horizontal Start Position in pixel [0,].*/
    unsigned short		y;	/**< vertical	Start Position in pixel [0,].*/
    unsigned short		w;	/**< horizontal Size(Width)    in pixel [0,].*/
    unsigned short		h;	/**< vertical	Size(Height)   in pixel [0,].*/
} __VIDEO_RECT_T;
#define VIDEO_RECT_T __VIDEO_RECT_T
#endif

#ifndef VSC_OUTPUT_POSITION_T
typedef enum
{
    VSC_OUTPUT_POSITION_MAIN = 0,
    VSC_OUTPUT_POSITION_SUB,
    VSC_OUTPUT_POSITION_MAIN_SUB,
    VSC_OUTPUT_POSITION_OSD1,
    VSC_OUTPUT_POSITION_OSD2,
    VSC_OUTPUT_POSITION_OSD3,
    VSC_OUTPUT_POSITION_OSD4,
    VSC_OUTPUT_POSITION_OSD5,
} _VSC_OUTPUT_POSITION_T;
#define VSC_OUTPUT_POSITION_T _VSC_OUTPUT_POSITION_T
#endif

#ifndef UVC_WINDOW_ZOOM_TYPE
typedef enum
{
    FIT_WINDOW = 0,
    KEEP_ASPECT_RATIO
} _UVC_WINDOW_ZOOM_TYPE;
#define UVC_WINDOW_ZOOM_TYPE _UVC_WINDOW_ZOOM_TYPE
#endif

unsigned char rtk_uvc_mute(bool enable);
unsigned char rtk_uvc_connect(bool enable, unsigned int position, unsigned int format);
unsigned char rtk_uvc_region_ctrl(unsigned int UID, VIDEO_RECT_T crop_window, VIDEO_RECT_T ori_window, VIDEO_RECT_T output_window, VIDEO_RECT_T zoom_window, UVC_WINDOW_ZOOM_TYPE zoom_type, unsigned int outputFrameRate);

void rtk_set_uvc_path_from_usb(unsigned short width, unsigned short height)
{
    VIDEO_RECT_T crop_window = {0,0,3840,2160};
    VIDEO_RECT_T ori_window = {0,0,3840,2160};
    VIDEO_RECT_T output_window = {0,0,1920,1080};
    VIDEO_RECT_T zoom_window = {0,0,0,0};
    UINT32 outputFrameRate = 0;

#if 0
    RTK_USB_DEVICE_DBG("rtk_set_uvc_path_from_usb 0xb8004aec=0x%x\n", rtd_inl(0xb8004aec));
    if(rtd_inl(0xb8004aec) != 0) {
        width  = (rtd_inl(0xb8004aec) >>16) & 0xffff;
        height = (rtd_inl(0xb8004aec)) & 0xffff;
        RTK_USB_DEVICE_DBG("rtk_set_uvc_path_from_usb width=%d height=%d\n", width, height);
    }
#endif
    output_window.w = width;
    output_window.h = height;
    
    rtk_uvc_mute(0);
    rtk_uvc_connect(1, 1, 0);//osd123
    rtk_uvc_region_ctrl(0, crop_window, ori_window, output_window, zoom_window, FIT_WINDOW, outputFrameRate);
}


void rtk_uvc_dma_setting_timer(void)
{
#if 0
        240    Hz -> 0xC2, 0xA2, 0x00, 0x00                           C2 A2 00 00
        144    Hz -> 0x44, 0x0F, 0x01, 0x00                           44 0F 01 00
        120    Hz -> 0x85, 0x45, 0x01, 0x00                           85 45 01 00
        119.88 Hz -> 0xD8, 0x45, 0x01, 0x00                           D8 45 01 00
        100    Hz -> 0xA0, 0x86, 0x01, 0x00                           A0 86 01 00
        60     Hz -> 0x0A, 0x8B, 0x02, 0x00                           0A 8B 02 00
        59.94  Hz -> 0xB1, 0x8B, 0x02, 0x00                           B1 8B 02 00
        50     Hz -> 0x40, 0x0D, 0x03, 0x00                           40 0D 03 00
        48     Hz -> 0xCD, 0x2D, 0x03, 0x00                           CD 2D 03 00
        40     Hz -> 0x90, 0xD0, 0x03, 0x00                           90 D0 03 00
        30     Hz -> 0x15, 0x16, 0x05, 0x00                           15 16 05 00
        29.97  Hz -> 0x63, 0x17, 0x05, 0x00                           63 17 05 00
        25     Hz -> 0x80, 0x1A, 0x06, 0x00                           80 1A 06 00
        24     Hz -> 0x9A, 0x5B, 0x06, 0x00                           9A 5B 06 00
#endif
    
        // enable timer
        if(usb_uvc_freq_real == 24000) {
            rtd_outl(0xB8004a80, ((27000000/240)<<8) + 1);   // 240Hz
        } else if(usb_uvc_freq_real == 14400) {
            rtd_outl(0xB8004a80, ((27000000/144)<<8) + 1);   // 144Hz
        } else if(usb_uvc_freq_real == 12000) {
            rtd_outl(0xB8004a80, ((27000000/120)<<8) + 1);
        } else if(usb_uvc_freq_real == 11988) {
            rtd_outl(0xB8004a80, ((27000000/119)<<8) + 1);
        } else if(usb_uvc_freq_real == 10000) {
            rtd_outl(0xB8004a80, ((27000000/100)<<8) + 1);
        } else if(usb_uvc_freq_real == 6000) {
            rtd_outl(0xB8004a80, ((27000000/60)<<8) + 1);
        } else if(usb_uvc_freq_real == 5994) {
            rtd_outl(0xB8004a80, ((27000000/59)<<8) + 1);
        } else if(usb_uvc_freq_real == 5000) {
            rtd_outl(0xB8004a80, ((27000000/50)<<8) + 1);
        } else if(usb_uvc_freq_real == 4800) {
            rtd_outl(0xB8004a80, ((27000000/48)<<8) + 1);
        } else if(usb_uvc_freq_real == 4000) {
            rtd_outl(0xB8004a80, ((27000000/40)<<8) + 1);
        } else if(usb_uvc_freq_real == 3000) {
            rtd_outl(0xB8004a80, ((27000000/30)<<8) + 1);
        } else if(usb_uvc_freq_real == 2997) {
            rtd_outl(0xB8004a80, ((27000000/30)<<8) + 1);
        } else if(usb_uvc_freq_real == 2500) {
            rtd_outl(0xB8004a80, ((27000000/25)<<8) + 1);
        } else if(usb_uvc_freq_real == 2400) {
            rtd_outl(0xB8004a80, ((27000000/24)<<8) + 1);
        } else {
            rtd_outl(0xB8004a80, ((27000000/60)<<8) + 1);
        }
}


void mjpeg_enc_qmem_table(void);
void mjpeg_enc_quant_table(void);
extern StructUVCInfo uvc_info;
void set_MJPEG_ENC(void);

void rtk_uvc_dma_setting_venc(void)
{
        //rtd_outl( UVC_VGIP_UVC_VGIP_CTRL_reg ,0x00000001);// [0]: reg_uvc_in_clk_en
        //udelay(1000);
#if 0
        // 1920 * 1080
        rtd_outl(0xb8004e00, 0xb9438780); // load qmem,height, width
        mjpeg_enc_qmem_table();
        rtd_outl(0xb8004e00, 0xd9438780); // load quant,height, width
        mjpeg_enc_quant_table();

        rtd_outl(0xb8004e14, 0x000000ff); // q table
        rtd_outl(0xb8004e00, 0x99438780); // height, width
        rtd_outl(0xb8004e68, 0x00094000);
        rtd_outl(0xb8004e70, 0x000016b6);
        rtd_outl(0xb8004e44, 0x00110804);
        rtd_outl(0xb8004e48, 0x38078003);
        rtd_outl(0xb8004e4c, 0x01220002);
        rtd_outl(0xb8004e50, 0x11010311);
        rtd_outl(0xb8004e54, 0x00000001);
        rtd_outl(0xb8004e04, 0x00000003);
#endif
#if 0
        // 3840 * 2160
        rtd_outl(0xb8004e00, 0xb9870F00); // load qmem,height, width
        mjpeg_enc_qmem_table();
        rtd_outl(0xb8004e00, 0xd9870F00); // load quant,height, width
        mjpeg_enc_quant_table();
        rtd_outl(0xb8004e14, 0x000000ff); // q table
        rtd_outl(0xb8004e00, 0x99870F00); // height, width
        rtd_outl(0xb8004e68, 0x001fffff);
        rtd_outl(0xb8004e70, 0x00000400);
        rtd_outl(0xb8004e44, 0x00110808);
        rtd_outl(0xb8004e48, 0x700F0003);
        rtd_outl(0xb8004e4c, 0x01220002);
        rtd_outl(0xb8004e50, 0x11010311);
        rtd_outl(0xb8004e54, 0x00000001);
        rtd_outl(0xb8004e04, 0x00000003);
#endif
#if 1
        uvc_info.Cap_Wid = usb_uvc_width_real;
        uvc_info.Cap_Len = usb_uvc_length_real;
        set_MJPEG_ENC();
#endif
}

extern code UINT8 format_mapping_table[];

void rtk_uvc_dma_setting(void)
{
    UINT32 temp;
    INT32  s_temp;

    RTK_USB_DEVICE_DBG("rtk_uvc_dma_force_yuy2 usb_uvc_width_real=%d usb_uvc_length_real=%d\n", usb_uvc_width_real, usb_uvc_length_real);
    RTK_USB_DEVICE_DBG_SIMPLE("uvc_timing_info format=%d width=%d height=%d freq=%d UVC_UID=%d\n", format_mapping_table[usb_uvc_format_real], usb_uvc_width_real, usb_uvc_length_real, usb_uvc_freq_real, uvc_uid);

    rtd_outl(0xb8004ae0, 0x00000004);
    rtd_outl(0xb80002a0, 0x00800000);

    rtd_outl(0xb8004808, (usb_uvc_width_real<<16) + usb_uvc_length_real); // uvc DS_image_witdth[28:16] uvc DS_image_height[12:0]

    rtd_outl(0xb800484c, 0x00000240); // cap0_max_outstanding[9:4] uvc dma0 en[0]
    rtd_outl(0xb800488c, 0x00000150); // cap1_max_outstanding[9:4] uvc dma1 en[0]
    rtd_outl(0xb80048cc, 0x00000070); // cap2_max_outstanding[9:4] uvc dma1 en[0]
    rtd_outl(0xb80048F0, 0x00000000); // uvc dma wdone sel[1:0]
    rtd_outl(0xb800483C, 0x00000010);
    rtd_outl(0xb8004838, 0x00000000);

    switch(usb_uvc_format_real) {
        case _USB_UVC_YUY2:
            rtd_outl(0xb8004848, uvc_frame_phy[0]);
            rtd_outl(0xb8004864, usb_uvc_width_real*(usb_uvc_length_real)*2/16);
            rtd_outl(0xb8004804, 0x00000001);
            rtd_outl(0xb8004810, 0x00000104);
            rtd_outl(0xb800484c, 0x00000240|0x00000001);
            break;
        case _USB_UVC_NV12:
            rtd_outl(0xb8004848, uvc_frame_phy[0] + 0); // uvc dma0 start address
            rtd_outl(0xb8004864, usb_uvc_width_real*usb_uvc_length_real/16); // uvc dma0 busrt number(unit: 16byte)
            rtd_outl(0xb8004888, uvc_frame_phy[0] + (usb_uvc_width_real*usb_uvc_length_real)); // uvc dma1 start address
            rtd_outl(0xb80048A4, usb_uvc_width_real*usb_uvc_length_real/2/16); // uvc dma1 busrt number(unit: 16byte)

            rtd_outl(0xb8004804, 0x00000003); // uvc ds hor ver on
            rtd_outl(0xb8004808, (usb_uvc_width_real<<16) + usb_uvc_length_real); // uvc DS_image_witdth[28:16] uvc DS_image_height[12:0]
            rtd_outl(0xb8004810, 0x00000109); // uvc pack wrap en[8], pack wrap mode[4:0]

            rtd_outl(0xb80048F0, 0x00000001); // uvc dma wdone sel[1:0]

            rtd_outl(0xb800484c, 0x00000240|0x00000001); // cap0_max_outstanding[9:4] uvc dma0 en[0]
            rtd_outl(0xb800488c, 0x00000150|0x00000001); // cap1_max_outstanding[9:4] uvc dma1 en[0]
            break;
        case _USB_UVC_I420:
            rtd_outl(0xb8004848, uvc_frame_phy[0] + 0); // uvc dma0 start address
            rtd_outl(0xb8004864, usb_uvc_width_real*usb_uvc_length_real/16); // uvc dma0 busrt number(unit: 16byte)
            rtd_outl(0xb8004888, uvc_frame_phy[0] + (usb_uvc_width_real*usb_uvc_length_real)); // uvc dma1 start address
            rtd_outl(0xb80048A4, usb_uvc_width_real*usb_uvc_length_real/2/2/16); // uvc dma1 busrt number(unit: 16byte)
            rtd_outl(0xb80048c8, uvc_frame_phy[0] + (usb_uvc_width_real*usb_uvc_length_real) + usb_uvc_width_real*(usb_uvc_length_real)/2/2); // uvc dma1 start address
            rtd_outl(0xb80048e4, usb_uvc_width_real*usb_uvc_length_real/2/2/16); // uvc dma1 busrt number(unit: 16byte)


            rtd_outl(0xb8004804, 0x00000003); // uvc ds hor ver on
            rtd_outl(0xb8004808, (usb_uvc_width_real<<16) + usb_uvc_length_real); // uvc DS_image_witdth[28:16] uvc DS_image_height[12:0]
            rtd_outl(0xb8004810, 0x0000010B); // uvc pack wrap en[8], pack wrap mode[4:0]

            rtd_outl(0xb80048F0, 0x00000002); // uvc dma wdone sel[1:0]

            rtd_outl(0xb800484c, 0x00000240|0x00000001); // cap0_max_outstanding[9:4] uvc dma0 en[0]
            rtd_outl(0xb800488c, 0x00000150|0x00000001); // cap1_max_outstanding[9:4] uvc dma1 en[0]
            rtd_outl(0xb80048cc, 0x00000070|0x00000001); // cap2_max_outstanding[9:4] uvc dma2 en[0]
            break;
        case _USB_UVC_RGB24:
            rtd_outl(0xb8004848, uvc_frame_phy[0] + (usb_uvc_width_real*(usb_uvc_length_real-1)*3)); //uvc dma start address
            rtd_outl(0xb8004864, usb_uvc_width_real * 3 /16); // busrt number(unit: 16byte)
            rtd_outl(0xb8004804, 0x00000000); //uvc ds hor ver off
            rtd_outl(0xb8004810, 0x0000010F); //uvc pack wrap en[8], pack wrap mode[4:0]

            rtd_outl(0xb800483C, usb_uvc_length_real<<4); //  [17:4] 540
            s_temp = -(usb_uvc_width_real*3/16);
            rtd_outl(0xb8004838, s_temp<<4); //  [31:4] -(960*3/16 = 180)
            rtd_outl(0xb800484c, 0x00000240|0x00000001); // uvc dma en[0]
            break;
        case _USB_UVC_MotionJPEG:
            rtd_outl(0xb8004848, uvc_frame_phy[0] + 0); // uvc dma0 start address
            rtd_outl(0xb8004864, 0x0FFFFFFF); // busrt number(unit: 16byte)
            rtd_outl(0xb8004804, 0x00000003); //uvc ds hor ver on
            rtd_outl(0xb8004810, 0x00000000); //uvc pack wrap en[8], pack wrap mode[4:0]
            rtd_outl(0xb800484c, 0x00000240|0x00010000); //[16] cap0_dma_data_sel
            // MJPEG enc
            // run mj_fpga_test1.c
            rtk_uvc_dma_setting_venc();
            rtd_outl(0xb80048F0, 0x00000003);  // uvc dma wdone sel[1:0]
            rtd_outl(0xb800484c, 0x00000240|0x00010001); // uvc dma en[0]
            break;
        case _USB_UVC_P010:
            rtd_outl(0xb8004848, uvc_frame_phy[0] + 0); // uvc dma0 start address
            rtd_outl(0xb8004864, usb_uvc_width_real* usb_uvc_length_real *2 /16); // uvc dma0 busrt number(unit: 16byte)
            rtd_outl(0xb8004888, uvc_frame_phy[0] + (usb_uvc_width_real* usb_uvc_length_real *2)); // uvc dma1 start address
            rtd_outl(0xb80048A4, usb_uvc_width_real*usb_uvc_length_real/16); // uvc dma1 busrt number(unit: 16byte)

            rtd_outl(0xb8004804, 0x00000003); // uvc ds hor ver on
            rtd_outl(0xb8004808, (usb_uvc_width_real<<16) + usb_uvc_length_real); // uvc DS_image_witdth[28:16] uvc DS_image_height[12:0]
            rtd_outl(0xb8004810, 0x0000010A); // uvc pack wrap en[8], pack wrap mode[4:0]

            rtd_outl(0xb80048F0, 0x00000001); // uvc dma wdone sel[1:0]

            rtd_outl(0xb800484c, 0x00000240|0x00000001); // cap0_max_outstanding[9:4] uvc dma0 en[0]
            rtd_outl(0xb800488c, 0x00000150|0x00000001); // cap1_max_outstanding[9:4] uvc dma1 en[0]
            break;
        default:
            break;
    }

    // enable uvc_vgip LC
    temp = rtd_inl(0xB8004010);
    rtd_outl(0xB8004010, temp|0x80000000);
    rtd_outl(0xb8004044, 0x00000f00);         // 3840
    rtd_outl(0xb8004034, 0x80008438);         // 2160/2 = 1080
    temp = rtd_inl(0xb8004040);
    rtd_outl(0xb8004040, temp|0x00000020);
    temp = rtd_inl(0xb800408C);
    rtd_outl(0xb800408C, temp|0x00000010);

    // enable uvc_dma wdone interrupt
    rtd_outl(0xb80002a0, 0x00800001);
    rtd_outl(0xB8004800, 0x00000010);
    rtd_outl(0xB8004820, 0x00000101);

    rtk_uvc_dma_setting_timer();

}

void calculs_trb_last_size(void)
{
    UINT32 total_size;

    switch(usb_uvc_format_real) {
        case _USB_UVC_YUY2:
            total_size = usb_uvc_width_real*usb_uvc_length_real*2;
            break;
        case _USB_UVC_NV12:
        case _USB_UVC_I420:
        case _USB_UVC_MotionJPEG:
            total_size = usb_uvc_width_real*usb_uvc_length_real*3/2;
            break;
        case _USB_UVC_P010:
            total_size = usb_uvc_width_real*usb_uvc_length_real*3;
            break;
        case _USB_UVC_RGB24:
            total_size = usb_uvc_width_real*usb_uvc_length_real*3;
            break;
        default:
            total_size = usb_uvc_width_real*usb_uvc_length_real*3;
            break;
    }

    trb_number      = total_size / 16372;
    last_frame_size = total_size % 16372;

}
