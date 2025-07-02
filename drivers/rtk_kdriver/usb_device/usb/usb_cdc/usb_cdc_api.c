#include "arch.h"
#include <rbus/timer_reg.h>
#include "mach.h"
#include "usb_mac.h"
#include "rtk_io.h"
#include "usb_cdc_api.h"

//#pragma nogcse
//PRAGMA_CODESEG(BANK41);
//PRAGMA_CONSTSEG(BANK41);

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
extern UINT32 reg;
extern struct dwc3 g_rtk_dwc3;
extern struct usb_cdc_line_coding g_acm_line_coding;
extern struct DMEM_COPY_TO_SRAM_STRUCT dwc_info;

UINT8 g_usb_uart_ready = false;
#ifdef CONFIG_ARCH_5281
UINT8 g_usb_tx_pending = 0;
#endif

#define dvr_to_phys(a) a

extern unsigned long long usb_phy_vir_offset;

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
struct usb_uart g_usb_uart =
{
        .bulk_in = NULL,
        .bulk_out = NULL,
        .int_in = NULL,
        .recv_buf = NULL,
        .log_buf = {
                .write_pos = 0,
                .read_pos = 0,
        },
};

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
UINT16 get_ymodem_crc(UINT8 * buf, UINT32 len)
{
        UINT16 chsum;
        UINT16 stat;
        UINT16 i;
        chsum = 0;
        for (stat = len ; stat > 0; stat--)
        {
                chsum = chsum^(UINT16)(*buf++) << 8;
                for (i=8; i!=0; i--)
                {
                        if (chsum & 0x8000)
                        {
                                chsum = chsum << 1 ^ 0x1021;
                        }
                        else
                        {
                                chsum = chsum << 1;
                        }
                }
        }
        return chsum;
}



#ifndef CONFIG_ARCH_5281
#include <linux/string.h>

#include <rbus/stb_reg.h>
#include <rbus/wdog_reg.h>

#define STC_LENGHT					(138)	/*8 + 128 + 2*/
#define STC_MAGIC_0					(0x80)
#define STC_MAGIC_1					(0x01)
#define STC_DATA_OFFSET			(0x8)
#define STC_DATA_SIZE				(128)
#define MAGIC_ENTER_RECOVERY_MODE		0x2379abcd


#define STX_MAGIC					(0x81)
#endif

#if 0
int enqueue_uart_rxbuf(UINT8 *pdata, UINT16 len)
{
#if 1
        INT8 blen = UART_BUF_LEN -1 - rxf_len();
        UINT8 woff = rUart_woff;

        if (blen > len)
                blen = len;

	while (blen > 0) { 
                rUartBuf[woff] = *pdata;
                woff = (woff + 1) & (UART_BUF_LEN -1);
                blen -=1;
                pdata ++;
        }
        
        if (rUart_woff != woff) {  /* woff is increasing, something input */
                /* may get a RTICE TPDU, send a RTICE notification. */
                if (rUartBuf[rUart_woff] & 0x80) {
                        mc_rtice_active();
                }
                rUart_woff = woff;
                core_fw_msg_queue_addEvent(MSG_MCOMMAND_ID, MC_SUB_IDX_OF(mc_uart_receiver), woff - rUart_woff);
	}
#endif
	return 0;
}

#if defined(CONFIG_ENABLE_EXT_MCU_UPGRADE_FW)
extern void rtk_ext_mcu_upgrade_firmware_callback(UINT8* buffer) __banked;
extern UINT8 g_ext_mcu_upgrade_status;

UINT8 check_usb_cdc_command_upgrade_stx(void)
{
        if (g_usb_uart.recv_buf[0] != MODEM_STX) {
                return FALSE;
        }
        if (g_usb_uart.recv_buf[1] != STX_MAGIC) {
                return FALSE;
        }

        return TRUE;
}
#endif

void uart_recv_callback(INT32 status, UINT32 actual_len)
{
        dwc_printk(LOGGER_INFO,"~%s: status:%x actul_len :%x\n", __func__, status, actual_len);
        if(status == 0 && actual_len)
        {
#ifndef CONFIG_ARCH_5281
                /*check upgrade cmd, trigger watchdog reboot*/
                if(actual_len == STC_LENGHT && g_usb_uart.recv_buf[0] == MODEM_STC &&
                    g_usb_uart.recv_buf[1] == STC_MAGIC_0 && g_usb_uart.recv_buf[2] == STC_MAGIC_1 && 
                    get_ymodem_crc(g_usb_uart.recv_buf + STC_DATA_OFFSET, STC_DATA_SIZE) == YMODEM_CRC(g_usb_uart.recv_buf  + STC_DATA_OFFSET + STC_DATA_SIZE)){
                        if(memcmp(g_usb_uart.recv_buf + STC_DATA_OFFSET, "enter_rescue", 12) == 0){
                                pm_printk(LOGGER_ERROR, "enter_rescue to upgrade\r\n");
                                /*set dummy register to notify rescue entry recovery mode*/
                                rtd_outl(STB_WDOG_DATA1, MAGIC_ENTER_RECOVERY_MODE);
                                /*trigger watchdog reboot*/
                                rtd_outl(WDOG_TCWCR, WDOG_TCWCR_im_wdog_rst(1));
                                while(1);
                        }
#if defined(CONFIG_ENABLE_EXT_MCU_UPGRADE_FW_BY_CDC)
                        else if (memcmp(g_usb_uart.recv_buf + STC_DATA_OFFSET, "enter_upgrade_ext_mcu", 21) == 0) {
                                pm_printk(LOGGER_ERROR, "enter_upgrade_ext_mcu\n");
                                g_ext_mcu_upgrade_status = 1;
                                g_usb_uart.recv_buf[0] = MODEM_ACK;
                                cdc_tx_directly(g_usb_uart.recv_buf, 1);
                        }
                        else if (memcmp(g_usb_uart.recv_buf + STC_DATA_OFFSET, "exit_upgrade_ext_mcu", 20) == 0) {
                                pm_printk(LOGGER_ERROR, "exit_upgrade_ext_mcu\n");
                                g_ext_mcu_upgrade_status = 0;
                                g_usb_uart.recv_buf[0] = MODEM_ACK;
                                cdc_tx_directly(g_usb_uart.recv_buf, 1);
                        }
#endif
                }
#if defined(CONFIG_ENABLE_EXT_MCU_UPGRADE_FW_BY_CDC)
                else if(check_usb_cdc_command_upgrade_stx()){
                        rtk_ext_mcu_upgrade_firmware_callback(g_usb_uart.recv_buf + 2);
                }
#endif
                else  {
                        // integrated with UART rx buffer
                        gCMDParam.connect_type = RTICE_CMD_CONNECT_TYPE_CDC;
                        enqueue_uart_rxbuf(g_usb_uart.recv_buf, actual_len);
//#define LOOPBACK_TEST
#ifdef LOOPBACK_TEST
                        pm_printk(LOGGER_ERROR, "Got data:%x %x\n", (UINT32)g_usb_uart.recv_buf[0], (UINT32)g_usb_uart.recv_buf[1]);
		  cdc_tx(g_usb_uart.recv_buf, actual_len);
		  cdc_tx_flush();
#endif
                }
#else
                // integrated with UART rx buffer
                gCMDParam.connect_type = RTICE_CMD_CONNECT_TYPE_CDC;
                enqueue_uart_rxbuf(g_usb_uart.recv_buf, actual_len);
#ifdef LOOPBACK_TEST
		  cdc_tx(g_usb_uart.recv_buf, actual_len);
#endif
#endif                
        }
#ifdef CONFIG_ARCH_5281			
                                rtk_dwc3_ep_start_trans(g_usb_uart.bulk_out->number, dvr_to_phys(g_usb_uart.recv_buf), LOG_RECV_BUF_SIZE, DWC3_TRBCTL_NORMAL);
#else
				  rtk_dwc3_ep_start_trans(g_usb_uart.bulk_out->number, dvr_to_phys(g_usb_uart.recv_buf_addr), LOG_RECV_BUF_SIZE, DWC3_TRBCTL_NORMAL);
#endif
}

UINT16 uart_start_tx(void) __banked
{

	 int flags;

        UINT16 remain;
        UINT16 cur_read_pos;
        UINT16 cur_write_pos;
        dwc3_disable_interrupt(flags);
        cur_read_pos = g_usb_uart.log_buf.read_pos;
        cur_write_pos = g_usb_uart.log_buf.write_pos;
	 dwc3_enable_interrupt(flags);
		
        if(cur_read_pos <= cur_write_pos)
                remain = cur_write_pos -cur_read_pos;
        else
                remain = LOG_BUF_SIZE - cur_read_pos;
	//pm_printk(LOGGER_ERROR, "%s,%d remain:%d %x, %x\n", __FUNCTION__, __LINE__, remain, cur_read_pos, cur_write_pos);
        if(remain)
        {
                reg = (UINT32)g_usb_uart.log_buf.buf;
                reg += cur_read_pos;
                rtk_dwc3_ep_start_trans(g_usb_uart.bulk_in->number, dvr_to_phys(reg), remain, DWC3_TRBCTL_NORMAL);
        }
        return remain;
}

void uart_send_callback(INT32 status, UINT32 actual_len)
{

	 int flags;

        UINT16 remain;
        UINT16 cur_read_pos;
        UINT16 cur_write_pos;
        //pm_printk(LOGGER_INFO, "%s,%d, status:%x,%x\n", __func__, __LINE__, status , actual_len);
	 dwc3_disable_interrupt(flags);
        if(status == 0 && actual_len)	
                g_usb_uart.log_buf.read_pos = (g_usb_uart.log_buf.read_pos + actual_len) % LOG_BUF_SIZE;
       
        cur_read_pos = g_usb_uart.log_buf.read_pos;
        cur_write_pos = g_usb_uart.log_buf.write_pos;
		
	dwc3_enable_interrupt(flags);
		   
        if(cur_read_pos < cur_write_pos)
                remain = cur_write_pos -cur_read_pos;
        else
                remain = LOG_BUF_SIZE - cur_read_pos;
	 
	//pm_printk(LOGGER_ERROR, "%s,%d remain:%d %x, %x\n", __FUNCTION__, __LINE__, remain, cur_read_pos, cur_write_pos);
        
	   
       if(remain) {
          //pm_printk(LOGGER_ERROR, "%s,%d, remain=%u\n", __FUNCTION__, __LINE__,remain);
#ifdef CONFIG_ARCH_5281
		  g_usb_tx_pending = 1;
#else			
		  core_fw_msg_queue_addEvent(MSG_USB_ID, MSG_USB_CDC_START_TX, 0);
#endif
	}
}
#endif

void rtk_cdc_control_interface(struct usb_ctrlrequest *ctrl_req)
{
        UINT32 ret;
        switch ((UINT16)(ctrl_req->bRequestType << 8) | ctrl_req->bRequest)
        {
                case 0x2120:
                        g_rtk_dwc3.ep0_setup_buf_len = ctrl_req->wLength;
                        g_rtk_dwc3.ep0_last_callback_id = USB_CDC_SET_LINE_CODING_ID;
                        break;
                case 0xa121:
#ifdef CONFIG_ARCH_5281					
                        dwc3_memcpy((void *)g_rtk_dwc3.ep0_setup_buf, (void *)&g_acm_line_coding, sizeof(g_acm_line_coding));
#else
                        dwc_info.dst = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
                        dwc_info.src = &g_acm_line_coding;
                        dwc_info.len = sizeof(g_acm_line_coding);
                        dwc3_copy_to_memory2();
#endif
                        g_rtk_dwc3.ep0_setup_buf_len = sizeof(g_acm_line_coding);
                        break;
                case 0x2122:
                        if(ctrl_req->wValue  == 0x3 || ctrl_req->wValue  == 0) {
                                g_usb_uart_ready = 1;
#ifdef CONFIG_ARCH_5281			
                                rtk_dwc3_ep_start_trans(g_usb_uart.bulk_out->number, dvr_to_phys(g_usb_uart.recv_buf), LOG_RECV_BUF_SIZE, DWC3_TRBCTL_NORMAL);
#else
				  rtk_dwc3_ep_start_trans(g_usb_uart.bulk_out->number, dvr_to_phys(g_usb_uart.recv_buf_addr), LOG_RECV_BUF_SIZE, DWC3_TRBCTL_NORMAL);
#endif
                        }
                        else {
                                g_usb_uart_ready = 0;
                        }
                        break;
                default:
                        RTK_DWC3_ERR("invalid control req%02x.%02x v%04x i%04x l%d\n",
                                     ctrl_req->bRequestType, ctrl_req->bRequest,
                                     ctrl_req->wValue, ctrl_req->wIndex, ctrl_req->wLength);
                        ret = -EOPNOTSUPP;
                        break;
        }
}

#if defined(CONFIG_ENABLE_EXT_MCU_UPGRADE_FW_BY_CDC)
UINT16 cdc_tx_directly(UINT8 *buf, UINT16 len)
{
#if 1
	g_usb_uart_ready = 1;
	g_usb_uart.log_buf.write_pos = 0;
	g_usb_uart.log_buf.read_pos = 0;
	cdc_tx(buf, len);
	//uart_start_tx();
        return 0;
#else
	g_usb_uart.log_buf.write_pos = 0;
	g_usb_uart.log_buf.read_pos = 0;
	while(len) {
		if(cdc_tx_one(*buf++) == 0)
			break;
		len--;
	}
	core_fw_msg_queue_addEvent(MSG_USB_ID, MSG_USB_CDC_START_TX, 0);
	return len;
#endif
}
#endif
