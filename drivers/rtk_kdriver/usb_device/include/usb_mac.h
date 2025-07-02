/**
 * core.h - DesignWare USB3 DRD Core Header
 */

#ifndef __USB_MAC_H
#define __USB_MAC_H

#include "arch.h"
#include <rbus/timer_reg.h>
#include "mach.h"
#include <linux/interrupt.h>
//#include "rtk_kdriver/isr_sys.h"
#include "rtk_io.h"
#include "usb_reg.h"
#include <rbus/sb2_reg.h>  /* SB2_SYNC */
#include <rbus/usb3_top_reg.h>
#include "msg_queue_def.h"

//****************************************************************************
// MARCO DECLARATIONS
//****************************************************************************
#define RTK_DWC3_INFO(fmt, args...)  
#define RTK_DWC3_ERR(fmt, args...)  
//#define RTK_DWC3_DEBUG(fmt, args...)  ROSPrintfInternal(fmt , ## args)
#define RTK_DWC3_DEBUG(fmt, args...)

//#define USB_U2_ONLY_MODE
//#define CONFIG_USB_SUPPORT_U1U2

#define USB_SRAM_DEBUG                  0
//#define DWC3_RTK_ENABLE_USB_BOOT_TIME_DEBUG_LOG 1
//#define EVENT_BUFFER                      0x1a3043f0
#define CRT_INIT_DELAY_US                 100
#define MAC_INIT_DELAY_MS                 1
#ifdef CONFIG_ARCH_RTK6702
// mark2
#define USB_DWC3_DEV_GLOBAL_REG_START     0xB8058100
#endif
#ifdef CONFIG_ARCH_RTK2885P
// mac9p fpga
#define USB_DWC3_DEV_GLOBAL_REG_START     0xB80AC100
#endif
#ifdef CONFIG_ARCH_RTK2819A
// mac9q
#define USB_DWC3_DEV_GLOBAL_REG_START     0xB80AC100
#endif
#define IS_SUPPORT_SELF_POWERED           1
#define USB_PHY_DELAY()                   delayus(50)
#define EACH_EVENT_BUF_SIZE               DWC3_EVENT_BUFFERS_SIZE
#define CHECK_EVENT_BUF_COUNT()           (rtd_inl(DWC3_GEVNTCOUNT(0)) & DWC3_GEVNTCOUNT_MASK)
#define DWC3_DEPCMD_PARAM_SHIFT           16

#define DWC3_TRB_CTRL_HWO                 _BIT0
#define DWC3_TRB_CTRL_LST                 _BIT1
#define DWC3_TRB_CTRL_CHN                 _BIT2
#define DWC3_TRB_CTRL_CSP                 _BIT3
#define DWC3_TRB_CTRL_ISP_IMI             _BIT10
#define DWC3_TRB_CTRL_IOC                 _BIT11

#define DWC3_USB_DESCRIPTOR_BUFFER_SIZE   4096
#define DWC3_USB_DESCRIPTOR_BUFFER_ADDRESS 0x47ff5000

//****************************************************************************
// USB INTERFACE
//****************************************************************************
//#define CONFIG_USB_CDC_ONLY
#ifdef CONFIG_ARCH_5281
#define CONFIG_USB_CDC_ONLY
#endif
#if defined(CONFIG_ARCH_5281) || defined(CONFIG_USB_CDC_ONLY)
#else
#define CDC_CONTROL_INTERFACE_NUM            6
#define CDC_DATA_INTERFACE_NUM               7
#define H5_SERIAL_CONTROL_INTERFACE_NUM      0
#define H5_SERIAL_DATA_INTERFACE_NUM         1
#define UVC_CONTROL_INTERFACE_NUM            0
#define UVC_STREAM_INTERFACE_NUM             1
#define UAC_CONTROL_INTERFACE_NUM            2
#define UAC_STREAM_OUT_INTERFACE_NUM         3
#define UAC_STREAM_IN0_INTERFACE_NUM         4
#define UAC_STREAM_IN1_INTERFACE_NUM         5

//****************************************************************************
// USB INTERFACE ENDPOINTADDRESS
//****************************************************************************
#define UVC_CONTROL_INTERFACE_ENDPOINTADDRESS                     0x85
#define UVC_STREAM_INTERFACE_ENDPOINTADDRESS                      0x82
#define UAC_STREAM_OUT_INTERFACE_ENDPOINTADDRESS                  0x03
#define UAC_STREAM_IN0_INTERFACE_ENDPOINTADDRESS                  0x83
#define UAC_STREAM_IN1_INTERFACE_ENDPOINTADDRESS                  0x86
#define CDC_CONTROL_INTERFACE_ENDPOINTADDRESS                     0x84
#define CDC_DATA_IN_INTERFACE_ENDPOINTADDRESS                     0x81
#define CDC_DATA_OUT_INTERFACE_ENDPOINTADDRESS                    0x01

#endif

#define CDC_STATUS         0x5
#define CDC_BULK_OUT       0x2
#define CDC_BULK_IN        0x3

#define CDC_EP_INT         0x82
#define CDC_EP_BULK_OUT    0x1
#define CDC_EP_BULK_IN     0x81

#define USB_U3_Burst_Size                    8

#define USB_DT_STRING_INDEX_COUNT              16

//****************************************************************************
// CODE TABLES
//****************************************************************************
typedef enum MSG_Q_USB {
    MSG_USB_ENUMLATION          = 0, // 0
    MSG_USB_CDC_START_TX        = 1,
    MSG_USB_INIT                = 2, // 2
    MSG_USB_CONNECT_TIMEOUT     = 3, // 3
    MSG_USB_UAC_DISCONNECT      = 4, // 4
    MSG_USB_MONITOR             = 5, // 5
    MSG_USB_UVC_UPDATE_TRANSFER = 6, // 6
    MSG_USB_UVC_PROBE           = 7, // 7
    MSG_USB_UVC_COMMIT          = 8, // 8
} MSG_Q_USB;


typedef enum MSG_USB_CONNECTED_SPEED {
    MSG_USB_CONNECT_SPEED_HS        = 0,    // 0
    MSG_USB_CONNECT_SPEED_SS        = 1,    // 1
    MSG_USB_CONNECT_SPEED_SSP       = 2,    // 2
    MSG_USB_CONNECT_SPEED_UNKNOW    = 0xff, // 0xff
} _MSG_USB_CONNECTED_SPEED;

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
extern UINT8 rtk_usb_customized_init(void);
extern void rtk_uac_disconnect_flow_isr(void);
extern void rtk_uac_disconnect_flow(void);
extern void rtk_uvc_disconnect_flow(void);

extern void rtk_dwc3_ep0_start_setup_phase(void);
extern void rtk_dwc3_ep0_stall_and_restart(void);
extern void rtk_dwc3_stop_active_transfer(UINT8 epnum);
extern UINT8 rtk_dwc3_ep0_set_config(void);
UINT8 __rtk_dwc3_gadget_ep_set_halt(UINT8 epnum, int value, int protocol);
extern inline UINT8 rtk_dwc3_gadget_ep_get_transfer_index(UINT8 number);
UINT8 dwc3_usb3_load_phy(void);
UINT8 dwc3_usb2_load_phy(void);
extern UINT8 rtk_dwc3_set_alt(void);
extern UINT8 rtk_dwc3_ep_start_trans(UINT8 epnum, UINT32 buf_dma,
                                   UINT16 len, UINT8 type);
irqreturn_t rtk_dwc3_process_event_buf(void);
UINT8 rtk_usb_driver_init(void);
UINT8 rtk_usb_driver_reinit(void);
UINT8 usb_driver_suspend(void);
UINT8 usb_driver_resume(void);

extern void dwc3_copy_from_memory(void *dst, UINT32 src, UINT16 len);
extern void dwc3_copy_to_memory(void *dst, void *src, UINT16 len);
extern void dwc3_memcpy(void *dst, void *src, UINT16 len);
extern void dwc3_memset(UINT8 *buf, UINT8 value, UINT16 len);
extern void dwc3_gadget_uboot_handle_interrupt(void);
extern UINT8 _rtk_dwc3_gadget_ep_enable(UINT8 epnum,
                                      const struct usb_endpoint_descriptor *desc,
                                      const struct usb_ss_ep_comp_descriptor *comp_desc,
                                      UINT8 ignore);

#define dwc3_enable_interrupt(flags)   
#define dwc3_disable_interrupt(flags)  


struct DMEM_COPY_TO_SRAM_STRUCT
{
	void *dst;
	void *src;
	UINT16 len;
};
void dwc3_copy_to_memory2(void);

struct SRAM_COPY_TO_DMEM_STRUCT
{
	void *src;
	void *dst;
	UINT16 len;
};
void dwc3_copy_from_memory2(void);
#endif


typedef enum
{
    _TYPEC_DISABLE = 0,                     // Disable
    _TYPEC_USB2_ONLY,                       // USB2 only
    _TYPEC_USB2_DP_1_4,                     // USB2+DP1.4
    _TYPEC_RESERVED,                        // reserved
    _TYPEC_USB2_USB31,                      // USB2+USB3.1(5G)
    _TYPEC_USB2_USB32,                      // USB2+USB3.2(10G)
    _TYPEC_USB2_USB31_DP_1_4,               // USB2+USB3.1(5G) +DP1.4
    _TYPEC_USB2_USB32_DP_1_4,               // USB2+USB3.2(10G)+DP1.4
} EnumTYPEC_FUNCTION;


#define DMA_UVC 1
#define DMA_UAC 2
#define DMA_UAC_DDR 3
#define DMA_AVSYNC_UAC 4

#define MSG_AP_ID 1
#define MSG_AP_USB_TYPE_DETECT 1

#define USER_SWITCH_DP_MODE 1

#define USB3_TOP_USB3_PHY_CONFIG_reg 1

void rtk_dwc3_event_buffers_setup(void);
bool rtk_dwc3_soft_reset(void);
void rtk_dwc3_init_endpoints(void);
void usb3_apply_phy_setting(void);
void rtk_usb_driver_cdc_function_init(void);
UINT8 GetUSBForceConnectSpeed(void);
UINT8 Get_Package_Type(void);
void core_fw_msg_queue_cancelEvent(UINT32 a, UINT32 b);
void core_fw_msg_queue_addEvent(UINT32 a, UINT32 b, UINT32 c);
void core_timer_event_addTimerEvent(UINT32 a, UINT32 b, UINT32 c, UINT32 d);
void core_timer_event_cancelTimerEvent(UINT32 a, UINT32 b);
void audio_uac_in_enable(UINT8 isEnable);



/*flash layout information*/
#define LAYOUT_SYSTEM_INFO_START						0x000000
#define LAYOUT_SYSTEM_INFO_SIZE						0x010000

#define LAYOUT_RESCUE_FW_START						0x010000
#define LAYOUT_RESCUE_FW_SIZE							0x010000

#define LAYOUT_NORMAL_FW0_START						0x020000
#define LAYOUT_NORMAL_FW0_SIZE						0x230000

#define LAYOUT_NORMAL_FW1_START						0x250000
#define LAYOUT_NORMAL_FW1_SIZE						0x230000

#define LAYOUT_IMP_DATA_START						0x480000
#define LAYOUT_IMP_DATA_SIZE							0x008000

//move to rtk_flash_layout.h
//#define LAYOUT_FILE_AREA_START						0x488000
//#define LAYOUT_FILE_AREA_SIZE							0x360000

#define LAYOUT_RW_AREA_START							0x7E8000
#define LAYOUT_RW_AREA_SIZE							0x018000


/*data address define*/
#define TBL0_PING_PONG_ADDR							0xD000
#define TBL1_PING_PONG_ADDR							0xE000

#define SPI_TUNING_PATTERN_ADDR						0xF000
#define SPI_TUNING_PATTERN_SIZE						0x10

/*impdata area*/
#define HDCP_KEY_ADDR									LAYOUT_IMP_DATA_START
#define HDCP_KEY_SIZE									0x1000

#define USB_IMPDATA_ADDR							(HDCP_KEY_ADDR + HDCP_KEY_SIZE)
#define USB_IMPDATA_SIZE							0x1000

#define USB_USER_DTAT_MAX_LENGTH_PER_ITEM		0x90
#define USB_USER_DATA_SERIAL_NUMBER_LENGTH     USB_USER_DTAT_MAX_LENGTH_PER_ITEM

#define USB_IMPDATA_SERIAL_NUMBER_OFSET		    	0x00

/*rw area*/
/*rw area: bottom*/
#define SYS_LOG_BUFFER_ADDR							LAYOUT_RW_AREA_START
#define SYS_LOG_BUFFER_SIZE							0x1000

//move to rtk_flash_layout.h
//#define AP_OSD_STATUS_ADDR							(SYS_LOG_BUFFER_ADDR + SYS_LOG_BUFFER_SIZE)
//#define AP_OSD_STATUS_SIZE								0x1000

#define AP_USER_DATA_AREA_A_ADDR						(AP_OSD_STATUS_ADDR + AP_OSD_STATUS_SIZE)
#define AP_USER_DATA_AREA_A_SIZE						0x1000
#define AP_USER_DATA_AREA_B_ADDR						(AP_USER_DATA_AREA_A_ADDR + AP_USER_DATA_AREA_A_SIZE)
#define AP_USER_DATA_AREA_B_SIZE						0x1000

#define SQE_CUSTOMER_SETTING_ADDR					(AP_USER_DATA_AREA_B_ADDR + AP_USER_DATA_AREA_B_SIZE)
#define SQE_CUSTOMER_SETTING_SIZE					0x1000

#define AP_SIGNATURE_AREA_ADDR					(SQE_CUSTOMER_SETTING_ADDR + SQE_CUSTOMER_SETTING_SIZE)					
#define AP_SIGNATURE_AREA_SIZE					0x1000

/*rw area: top*/
#define SPI_TUNING_RESULT_SIZE							0x1000
#define SPI_TUNING_RESULT_ADDR						0x7FF000	/*the last sector*/

#define USB_USER_DATA_VID_LENGTH				0x10
#define USB_USER_DATA_PID_LENGTH				0x10
#define USB_USER_DATA_UVC_NAME_LENGTH			USB_USER_DTAT_MAX_LENGTH_PER_ITEM
#define USB_USER_DATA_UAC_OUT1_LENGTH			USB_USER_DTAT_MAX_LENGTH_PER_ITEM
#define USB_USER_DATA_MANUFACTURER_NAME_LENGTH	USB_USER_DTAT_MAX_LENGTH_PER_ITEM
#define USB_USER_DATA_PRODUCT_NAME_LENGTH		USB_USER_DTAT_MAX_LENGTH_PER_ITEM
#define USB_USER_DATA_UAC_OUT2_LENGTH			USB_USER_DTAT_MAX_LENGTH_PER_ITEM
#define USB_USER_DATA_UAC_IN_LENGTH				USB_USER_DTAT_MAX_LENGTH_PER_ITEM
#define USB_USER_DATA_VID_PID_BY_SPEED_LENGTH	0x10
#define USB_USER_DATA_BCD_DEVICE_LENGTH			0x10
#define USB_USER_DATA_UAC_OUT1_INTERFACE_LENGTH	USB_USER_DTAT_MAX_LENGTH_PER_ITEM
#define USB_USER_DATA_UAC_OUT2_INTERFACE_LENGTH	USB_USER_DTAT_MAX_LENGTH_PER_ITEM
#define USB_USER_DATA_UAC_IN_INTERFACE_LENGTH	USB_USER_DTAT_MAX_LENGTH_PER_ITEM


#define USB_USER_DATA_SIZE							(0x1000)
#define USB_USER_DATA_ADDR							(SPI_TUNING_RESULT_ADDR - USB_USER_DATA_SIZE)
#define USB_USER_DATA_VID_OFSET						(0x00)
#define USB_USER_DATA_PID_OFSET						(USB_USER_DATA_VID_OFSET               + USB_USER_DATA_VID_LENGTH)
#define USB_USER_DATA_UVC_NAME_OFSET				(USB_USER_DATA_PID_OFSET               + USB_USER_DATA_PID_LENGTH)
#define USB_USER_DATA_UAC_OUT1_NAME_OFSET			(USB_USER_DATA_UVC_NAME_OFSET          + USB_USER_DATA_UVC_NAME_LENGTH)
#define USB_USER_DATA_MANUFACTURER_NAME_OFSET		(USB_USER_DATA_UAC_OUT1_NAME_OFSET     + USB_USER_DATA_UAC_OUT1_LENGTH)
#define USB_USER_DATA_PRODUCT_NAME_OFSET			(USB_USER_DATA_MANUFACTURER_NAME_OFSET + USB_USER_DATA_MANUFACTURER_NAME_LENGTH)
#define USB_USER_DATA_UAC_OUT2_NAME_OFSET			(USB_USER_DATA_PRODUCT_NAME_OFSET      + USB_USER_DATA_PRODUCT_NAME_LENGTH)
#define USB_USER_DATA_UAC_IN_NAME_OFSET				(USB_USER_DATA_UAC_OUT2_NAME_OFSET     + USB_USER_DATA_UAC_OUT2_LENGTH)
#define USB_USER_DATA_VID_PID_BY_SPEED_OFFSET		(USB_USER_DATA_UAC_IN_NAME_OFSET       + USB_USER_DATA_UAC_IN_LENGTH)
#define USB_USER_DATA_BCD_DEVICE_OFFSET				(USB_USER_DATA_VID_PID_BY_SPEED_OFFSET + USB_USER_DATA_VID_PID_BY_SPEED_LENGTH)
#define USB_USER_DATA_UAC_OUT1_INTERFACE_NAME_OFSET	(USB_USER_DATA_BCD_DEVICE_OFFSET             + USB_USER_DATA_BCD_DEVICE_LENGTH)
#define USB_USER_DATA_UAC_OUT2_INTERFACE_NAME_OFSET	(USB_USER_DATA_UAC_OUT1_INTERFACE_NAME_OFSET + USB_USER_DATA_UAC_OUT1_INTERFACE_LENGTH)
#define USB_USER_DATA_UAC_IN_INTERFACE_NAME_OFSET	(USB_USER_DATA_UAC_OUT2_INTERFACE_NAME_OFSET + USB_USER_DATA_UAC_OUT2_INTERFACE_LENGTH)

#define USB_USER_DATA_ACCESS_SIZE				2048

//move to rtk_flash_layout.h
//#define CUSTOMER_PROPRIETARY_SIZE					0x1000
//#define CUSTOMER_PROPRIETARY_ADDR					(USB_USER_DATA_ADDR - CUSTOMER_PROPRIETARY_SIZE)


typedef enum MSG_Q_EVENT_UVC {
    MSG_UVC_AP_INITAIL              = 1,     
    MSG_UVC_AP_REINITAIL_DISCONNECT = 2,     
    MSG_UVC_AP_REINITAIL_CONNECT    = 3,     
    MSG_UVC_AP_UNKNOW_ID            = 0xffff,
} _MSG_Q_EVENT_UVC;


typedef enum
{
	OK					= 0,
	NOT_OK				= -1,
	INVALID_PARAMS		= -2,
	TIMEOUT 			= -3,
} DRV_STATUS_T;



#define UART_BUF_LEN 64

typedef struct {
    UINT8  format;
    UINT16 width;
    UINT16 height;
    UINT16 freq;
} USB_UVC_TIMING_T;


#define MSG_AP_UVC_CONNECT 103
#define MSG_AP_UVC_DISCONNECT 104
#define MSG_AP_USB_MAC_HANDSHAKE_STATUS 138
#define MSG_AP_USB_POWER_STATUS_UPDATE 134
#define MSG_AP_UVC_PLUG_OUT 107
#define MSG_AP_UVC_PLUG_IN 108




enum usb_link_state {
    /* In SuperSpeed */
    USB_LINK_STATE_U0       = 0x00,
    USB_LINK_STATE_U1       = 0x01,
    USB_LINK_STATE_U2       = 0x02,
    USB_LINK_STATE_U3       = 0x03,
    USB_LINK_STATE_SS_DIS   = 0x04,
    USB_LINK_STATE_RX_DET   = 0x05,
    USB_LINK_STATE_SS_INACT = 0x06,
    USB_LINK_STATE_POLL     = 0x07,
    USB_LINK_STATE_RECOV    = 0x08,
    USB_LINK_STATE_HRESET   = 0x09,
    USB_LINK_STATE_CMPLY    = 0x0a,
    USB_LINK_STATE_LPBK     = 0x0b,
    USB_LINK_STATE_RESET    = 0x0e,
    USB_LINK_STATE_RESUME   = 0x0f,
    USB_LINK_STATE_MASK     = 0x0f,
};


UINT32 rtd_mem_inl(UINT8 *addr);
UINT32 rtd_mem_outl(UINT8 *addr, UINT32 value);


//------------------------------------------
// Definitions of Bits
//------------------------------------------

#ifndef _BIT0
#define _ZERO   						0x00
#define _BIT0   						0x01UL
#define _BIT1   						0x02UL
#define _BIT2   						0x04UL
#define _BIT3   						0x08UL
#define _BIT4   						0x10UL
#define _BIT5   						0x20UL
#define _BIT6   						0x40UL
#define _BIT7   						0x80UL
#define _BIT8   						0x0100UL
#define _BIT9   						0x0200UL
#define _BIT10  						0x0400UL
#define _BIT11  						0x0800UL
#define _BIT12  						0x1000UL
#define _BIT13  						0x2000UL
#define _BIT14  						0x4000UL
#define _BIT15  						0x8000UL
#define _BIT16   						0x00010000UL
#define _BIT17  						0x00020000UL
#define _BIT18   						0x00040000UL
#define _BIT19   						0x00080000UL
#define _BIT20   						0x00100000UL
#define _BIT21   						0x00200000UL
#define _BIT22   						0x00400000UL
#define _BIT23   						0x00800000UL
#define _BIT24   						0x01000000UL
#define _BIT25   						0x02000000UL
#define _BIT26  						0x04000000UL
#define _BIT27  						0x08000000UL
#define _BIT28  						0x10000000UL
#define _BIT29  						0x20000000UL
#define _BIT30  						0x40000000UL
#define _BIT31  						0x80000000UL
#endif

#define GetFlashCdcOnOff()          0

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

void rtk_uac_disconnect_flow_pd(void);
bool rtk_dwc3_probe(void);
UINT8 rtk_usb_driver_check_sw_config(void);


#define mc_console_cdc_on() do {} while(0)
#define mc_console_cdc_off() do {} while(0)

typedef enum
{
    _Ext_DP_MUX_Select_RTS5450M = 0,
    _Ext_DP_MUX_Select_EJ899M,
    _Ext_DP_MUX_Select_EJ899I,
    _Ext_DP_MUX_Select_CYPD3176,
} EnumExt_DP_MUX_Select;

typedef enum 
{
    _FUNCTION_DISABLE = 0,    // Disable
    _FUNCTION_ENABLE = 1,     // Enable
} FUNCTION_ENABLE;


