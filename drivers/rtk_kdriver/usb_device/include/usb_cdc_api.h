#ifndef __RTK_USB_CDC_H__
#define __RTK_USB_CDC_H__

#include "arch.h"
#include <rbus/timer_reg.h>
#include "mach.h"
//#include "rtk_kdriver/isr_sys.h"
#include "rtk_io.h"
#include "usb_reg.h"
#include "usb_cdc.h"

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
//#define LOG_BUF_SIZE 1024
#define LOG_BUF_SIZE (1024 + (UINT32)0x600)
#define LOG_RECV_BUF_SIZE 1024

#define MODEM_SOH  0x01
#define MODEM_STX  0x02
#define MODEM_STG  0x03
#define MODEM_STC  0x05
#define MODEM_EOT  0x04
#define MODEM_ACK  0x06
#define MODEM_NAK  0x15
#define MODEM_CAN  0x18
#define MODEM_C    0x43
#define YMODEM_CRC(data)  (((data)[1] << 8) | (data)[0])

//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************
#ifdef CONFIG_RESUCE_MODE

extern static UINT8 g_usb_write_start;

//****************************************************************************
// CODE TABLES
//****************************************************************************
typedef enum
{
        UPGRADE_NO_START,
        UPGRADE_RECV_ENTER_RESUCE_CMD,
        UPGRADE_WAIT_FIRST_SOH,
        UPGRADE_WAIT_FIRST_SOH_SEND_ACK,
        UPGRADE_WAIT_FIRST_SOH_SEND_NACK,
        UPGRADE_WAIT_FIRST_SOH_SEND_C,
        UPGRADE_WAIT_DATA,
        UPGRADE_WAIT_DATA_SENT_ACK,
        UPGRADE_WAIT_DATA_SENT_NACK,
        UPGRADE_RECEIVE_FIRST_EOT,
        UPGRADE_SENT_FIRST_EOT_NACK,
        UPGRADE_RECEIVE_SECOND_EOT,
        UPGRADE_IN_PROCESS,
} UPGRADE_STATE;
static volatile UPGRADE_STATE g_in_upgrade_process = UPGRADE_NO_START;

struct YModem_Struct
{
        UINT32 file_len;
        UINT16 cur_len;
        UINT8 buf[4096];
        UINT8 next_seqnum;
} g_yModem_data;

#endif

struct ring_log_buf
{
        UINT16 write_pos;
        UINT16 read_pos;
#ifdef 	CONFIG_ARCH_5281		
        UINT8 *buf;
#else
	UINT32 buf;
#endif
};

struct usb_uart
{
        struct dwc3_ep *bulk_in;
        struct dwc3_ep *bulk_out;
        struct dwc3_ep *int_in;
        UINT8 *recv_buf;
        UINT32 recv_buf_addr;
        struct ring_log_buf log_buf;
};

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
extern UINT16 get_ymodem_crc(UINT8 * buf, UINT32 len);
extern void uart_recv_callback(INT32 status, UINT32 actual_len);
extern UINT16 uart_start_tx(void);
extern void uart_send_callback(INT32 status, UINT32 actual_len);
void rtk_cdc_control_interface(struct usb_ctrlrequest *ctrl_req);
UINT16 cdc_tx(UINT8 *buf, UINT16 len);
void cdc_tx_flush(void);
#if defined(CONFIG_ENABLE_EXT_MCU_UPGRADE_FW_BY_CDC)
UINT16 cdc_tx_directly(UINT8 *buf, UINT16 len);
#endif
#endif // __RTK_USB_CDC_H__
