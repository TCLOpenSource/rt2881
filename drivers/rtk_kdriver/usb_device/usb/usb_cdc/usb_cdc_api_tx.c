#include "arch.h"
#include <rbus/timer_reg.h>
#include "mach.h"
//#include "rtk_kdriver/isr_sys.h"
#include "rtk_io.h"
//#include "rtk_kdriver/rtd_types.h"
#include "usb_mac.h"
#include "usb_cdc_api.h"
#include "msg_q_function_api.h"

//#pragma nogcse
//PRAGMA_CODESEG(BANK41);
//PRAGMA_CONSTSEG(BANK41);

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
extern UINT32 reg;
extern struct dwc3 g_rtk_dwc3;
extern UINT8 g_usb_uart_ready;
#ifdef CONFIG_ARCH_5281
extern UINT8 g_usb_tx_pending;
#endif
extern UINT16 i;
static UINT8 g_last_loop_main_seq = 0;
UINT8 g_main_loop_seq = 0;

extern UINT8 *g_usb_iram;       // usb buffer virtual address
extern UINT32 g_usb_iram_dma;       // usb buffer PHY address

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
extern struct usb_uart g_usb_uart;

#if defined(CONFIG_ENABLE_EXT_MCU_UPGRADE_FW_BY_CDC)
extern UINT8 g_ext_mcu_upgrade_status;
#endif

#ifndef CONFIG_ARCH_5281
static void dwc3_copy_to_memory_cdc(UINT8 *dst, UINT8 ch)
{
    *dst = ch;
}
#endif


static UINT8 cdc_tx_one(UINT8 ch);
UINT16 cdc_tx(UINT8 *buf, UINT16 len);
void cdc_putch(UINT8 ch)
{
        __xdata UINT8 c = ch;
#if defined(CONFIG_ENABLE_EXT_MCU_UPGRADE_FW_BY_CDC)
		if (g_ext_mcu_upgrade_status) {
			return;
		}
#endif
        cdc_tx(&c, 1);
}


static UINT8 cdc_tx_one(UINT8 ch)
{
	 int flags;
        UINT16 cur_write_pos;
	(void)flags;
	 dwc3_disable_interrupt(flags);
        cur_write_pos = g_usb_uart.log_buf.write_pos;
        if(((cur_write_pos + 1) % LOG_BUF_SIZE) == g_usb_uart.log_buf.read_pos) {
		dwc3_enable_interrupt(flags);
		//rtd_outl(0x18062300, '#');
		return 0;
	 }
	 g_usb_uart.log_buf.write_pos = ((cur_write_pos + 1) % LOG_BUF_SIZE);
	 dwc3_enable_interrupt(flags);
	 
        //pm_printk(LOGGER_ERROR, "%s,%d remain:%d %x, %x\n", __FUNCTION__, __LINE__, remain, cur_read_pos, cur_write_pos);
#ifdef 	CONFIG_ARCH_5281
	g_usb_uart.log_buf.buf[cur_write_pos] = ch;
#else
	dwc3_copy_to_memory_cdc( ((unsigned long long)((g_usb_uart.log_buf.buf + cur_write_pos) - g_usb_iram_dma) + g_usb_iram), ch);
#endif	
	 return 1;
}

UINT16 cdc_tx(UINT8 *buf, UINT16 len)
{
        if(!g_usb_uart_ready)
                return 0;
		
	while(len) {
		if(cdc_tx_one(*buf++) == 0)
			break;
		len--;
	}	
	return len;
}

void cdc_tx_flush(void)
{
	if(!g_usb_uart_ready)
                return;
#ifdef CONFIG_ARCH_5281
	g_usb_tx_pending = 1;
#else			
	//TODO:queue tx timer to call return uart_start_tx();
	if(g_last_loop_main_seq != g_main_loop_seq) {
		g_last_loop_main_seq = g_main_loop_seq;
		core_fw_msg_queue_addEvent(MSG_USB_ID, MSG_USB_CDC_START_TX, 0);
	}
#endif		
}


