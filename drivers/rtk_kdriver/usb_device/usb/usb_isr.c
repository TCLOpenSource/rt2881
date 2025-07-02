#include "arch.h"
#include <rbus/timer_reg.h>
#include "mach.h"
//#include "rtk_kdriver/isr_sys.h"
#include "rtk_io.h"
#include <rbus/sb2_reg.h>  /* SB2_SYNC */
#include <rbus/usb3_top_reg.h>
#include "usb_mac.h"
#include "usb_reg.h"
#include <linux/interrupt.h>
#include "timer_event.h"
#include "msg_queue_def.h"
#include "msg_q_function_api.h"
#include <linux/timer.h>
#include <rtk_kdriver/io.h>
#include <linux/delay.h>

#include "usb_uvc_api.h"

#include <rbus/uvc_dma_reg.h>
#include <rbus/uvc_vgip_reg.h>
#include "rtk_usb_device_dbg.h"
#include <rbus/mjpeg_enc_reg.h>

//PRAGMA_CODESEG(BANK41);
//PRAGMA_CONSTSEG(BANK41);

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
//DECLARE_TOP_ISR(usb_topisr37);    // for interrupt mode
//DECLARE_ISR(usb_isr37, 3, 7);     // for interrupt mode

struct dwc3 g_rtk_dwc3;
UINT8 Unplug_flag = 0;
UINT32 dwc3_at_cmd_i2c_write_timeout = 0;
UINT32 dwc3_uac_out_enable_timeout = 0;
UINT32 dwc3_uac_out2_enable_timeout = 0;
UINT32 u32_usb_time = 0;
UINT32 u32_usb_time_temp;
UINT32 dwc3_at_cmd_AccelerateMode = 0;

#ifdef DWC3_RTK_ENABLE_USB_BOOT_TIME_DEBUG_LOG
UINT32 u32_usb_time_temp_begin = 0;
UINT32 u32_usb_time_temp_end = 0;
#endif

extern UINT8 wait_do_control_transfer;
UINT32 volatile systick = 0;

extern UINT32 uvc_frame_phy[UVC_FRAME_BUFFER_NUMBER];
extern UINT8 *uvc_frame_vir[UVC_FRAME_BUFFER_NUMBER];

extern UINT32 uvc_header_phy[UVC_FRAME_BUFFER_NUMBER];
extern UINT8 *uvc_header_vir[UVC_FRAME_BUFFER_NUMBER];

extern UINT32 uvc_trb_phy[UVC_FRAME_BUFFER_NUMBER];
extern UINT8 *uvc_trb_vir[UVC_FRAME_BUFFER_NUMBER];

extern UINT32 uvc_trb_size;
extern UINT8 uvc_frame_buffer_status[UVC_FRAME_BUFFER_NUMBER];
extern UINT32 uvc_frame_buffer_real_size[UVC_FRAME_BUFFER_NUMBER];
extern UINT8 uvc_frame_buffer_wp;
extern UINT8 uvc_frame_buffer_wp_old;
extern UINT8 uvc_frame_buffer_rp;

extern UINT8 usb_uvc_format_real;
extern UINT16 usb_uvc_width_real;
extern UINT16 usb_uvc_length_real;
extern UINT16 usb_uvc_freq_real;

extern unsigned char frame_index;

extern UINT8 __rtk_dwc3_gadget_ep_disable(UINT8 epnum);
UINT8 rtk_dwc3_gadget_run_stop(UINT8 is_on);
irqreturn_t rtk_dwc3_process_event_buf(void);
void gen_header_trb(void);

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
/*
int usb_topisr37(void)
{
        //rtk_dwc3_check_event_buf
        UINT16 count;
        UINT32 reg;

        count = rtd_isr_inl(DWC3_GEVNTCOUNT(0));
        count &= DWC3_GEVNTCOUNT_MASK;
        if (!count)  // Not our event
                return false;

        g_rtk_dwc3.g_event_buf.count = count;
        g_rtk_dwc3.g_event_buf.flags |= DWC3_EVENT_PENDING;

        // Mask interrupt
        reg = rtd_isr_inl(DWC3_GEVNTSIZ(0));
        reg |= DWC3_GEVNTSIZ_INTMASK;
        rtd_isr_outl(DWC3_GEVNTSIZ(0), reg);

        return true;
}

int usb_isr37(void)
{
        // isr queue
        core_isr_msg_queue_addEvent(MSG_USB_ID, MSG_USB_ENUMLATION, 0);
        return 1;
}

void dwc3_gadget_uboot_handle_interrupt()
{
        #ifdef CONFIG_ARCH_5281
        while (USB3_TOP_WRAP_CTR_get_dbus_wdone_counter(rtd_inl(USB3_TOP_WRAP_CTR)));
        #else
        #define  USB3_TOP_WRAP_CTR_get_dbus_wdone_counter(data)                          ((0x00000F80&(data))>>7) 
        while (USB3_TOP_WRAP_CTR_get_dbus_wdone_counter(rtd_inl(0xb8069000)));
        #endif

        #ifdef CONFIG_ARCH_5281
        rtd_outl(SYS_REG_INT_CTRL_SCPU_2_reg,
                 SYS_REG_INT_CTRL_SCPU_2_usb_int_scpu_routing_en_mask);
        #else
        rtd_outl(0xB80002A0, 
                 0x00080000 |
                 0x0);
        #endif

        rtk_dwc3_process_event_buf();

        #ifdef CONFIG_ARCH_5281
        rtd_outl(SYS_REG_INT_CTRL_SCPU_2_reg,
                 SYS_REG_INT_CTRL_SCPU_2_usb_int_scpu_routing_en_mask |
                 SYS_REG_INT_CTRL_SCPU2_2_write_data(1));
        #else
        rtd_outl(0xB80002A0, 
                 0x00080000 |
                 0x1);
        #endif
}*/

int usb_topisr37(void)
{
        //rtk_dwc3_check_event_buf
        UINT16 count;
        UINT32 reg;

        //pm_printk(LOGGER_ERROR, "usb_topisr37 start\n");
        count = rtd_inl(DWC3_GEVNTCOUNT(0));
        count &= DWC3_GEVNTCOUNT_MASK;
        if (!count)  // Not our event
                return false;

        g_rtk_dwc3.g_event_buf.count = count;
        g_rtk_dwc3.g_event_buf.flags |= DWC3_EVENT_PENDING;

        /* Mask interrupt */
        reg = rtd_inl(DWC3_GEVNTSIZ(0));
        reg |= DWC3_GEVNTSIZ_INTMASK;
        rtd_outl(DWC3_GEVNTSIZ(0), reg);
        return true;
}

int usb_isr37(void)
{
        //pm_printk(LOGGER_ERROR, "usb_isr37 start\n");
        #ifdef CONFIG_ARCH_5281
        while (USB3_TOP_WRAP_CTR_get_dbus_wdone_counter(rtd_inl(USB3_TOP_WRAP_CTR)));
        #endif
        rtk_dwc3_process_event_buf();
        return 1;
}

void dwc3_gadget_handle_reinit(void)
{
        if(g_rtk_dwc3.dwc_need_reconnect == 3) {
                delayms(30);
                rtk_usb_driver_reinit();
                g_rtk_dwc3.dwc_need_reconnect = 0;
        }
}

UINT8 check_typecpd_interrupt(void)
{
        if ((rtd_inl(0xb8060474) & (_BIT30 | _BIT31)) != 0x0) {
            if((rtd_inb(0xb806530C) & (_BIT0 | _BIT2)) != 0x0) {
                return 1;
            }
        }
        return 0;
}

void check_wait_do_control_transfer_systick(void)
{
        if((wait_do_control_transfer == 1) && ((systick & 0xff) == 0x00)) {
            pm_printk(0, "usb wait %d\n", systick);
        }
}

void dwc3_gadget_uboot_handle_interrupt(void) __banked
{
#ifdef DWC3_RTK_ENABLE_USB_BOOT_TIME_DEBUG_LOG
        u32_usb_time_temp_begin = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
        pm_printk(0, "## B_I %d\n", u32_usb_time_temp_begin);
#endif
        u32_usb_time_temp = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);

        do {
            while(1)
            {
#ifdef CONFIG_ENABLE_TYPEC_REL
                // Check if TYPEC INT
                if (check_typecpd_interrupt() == 1) {
                    break;
                }
#endif
                if (usb_topisr37()) {
                    usb_isr37();
                }

                if ( (rtd_inl(TIMER_SCPU_CLK90K_LO_reg)>dwc3_uac_out_enable_timeout)
                    && (rtd_inl(TIMER_SCPU_CLK90K_LO_reg)>dwc3_uac_out2_enable_timeout)) {
                    break;
                }
            }

            check_wait_do_control_transfer_systick();

        } while( (wait_do_control_transfer == 1) || ( rtd_inl(TIMER_SCPU_CLK90K_LO_reg) < dwc3_at_cmd_i2c_write_timeout));

        dwc3_at_cmd_i2c_write_timeout = 0;
        dwc3_uac_out_enable_timeout   = 0;
        dwc3_uac_out2_enable_timeout  = 0;
        dwc3_at_cmd_AccelerateMode    = 0;

        dwc3_gadget_handle_reinit();

        u32_usb_time_temp = rtd_inl(TIMER_SCPU_CLK90K_LO_reg) - u32_usb_time_temp;
        u32_usb_time =  u32_usb_time + u32_usb_time_temp;

#ifdef DWC3_RTK_ENABLE_USB_BOOT_TIME_DEBUG_LOG
        u32_usb_time_temp_end = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
        pm_printk(0, "## B_O %d %d", u32_usb_time_temp_end, (u32_usb_time_temp_end-u32_usb_time_temp_begin)/90);
        pm_printk(0, "     %d\n", u32_usb_time/90);
#endif

/*#ifndef CONFIG_ARCH_5281
        UINT32 temp, temp1;

        temp  = rtd_inl(0xb8084aa0);
        temp  = temp & 0x00000010;
        temp1 = rtd_inl(0xb8084868);
        temp1 = temp1 & 0x00000001; 

        
        //pm_printk(LOGGER_ERROR, "dwc3_gadget_uboot_handle_interrupt start\n");
        if(temp!=0 && temp1==0x00000001) {
            rtd_outl(0xb8084aa0, 0x00020016);  // WTCTL FIFO STATUS
            pm_printk(LOGGER_ERROR, "uvc gating\n");
        }
#endif		*/
/*#ifndef CONFIG_ARCH_5281		
        if(temp!=0 && temp1==0x00000001) {
            rtd_outl(0xb8084aa0, 0x0002001C);  // WTCTL FIFO STATUS
        }
#endif		*/
}

void dwc3_gadget_uboot_handle_interrupt_ex(void) __banked
{
    dwc3_gadget_uboot_handle_interrupt();
}

irqreturn_t rtk_usb_device_interrupt(int irq, void *_evt)
{
    UINT16 count;
    UINT32 reg;

    if( (rtd_inl(SYS_REG_SYS_SRST6_reg) & SYS_REG_SYS_SRST6_rstn_usb3_dev_dwc_mask) == 0)
        return IRQ_NONE;

    count = rtd_inl(DWC3_GEVNTCOUNT(0));
    count &= DWC3_GEVNTCOUNT_MASK;
    if (!count)  // Not our event
            return IRQ_NONE;
    
    g_rtk_dwc3.g_event_buf.count = count;
    g_rtk_dwc3.g_event_buf.flags |= DWC3_EVENT_PENDING;
    
    /* Mask interrupt */
    reg = rtd_inl(DWC3_GEVNTSIZ(0));
    reg |= DWC3_GEVNTSIZ_INTMASK;
    rtd_outl(DWC3_GEVNTSIZ(0), reg);
    return IRQ_WAKE_THREAD;
}

irqreturn_t rtk_usb_device_thread_interrupt(int irq, void *_evt)
{
	unsigned long flags;
	irqreturn_t ret = IRQ_NONE;

	local_bh_disable();
	spin_lock_irqsave(&g_rtk_dwc3.usb_device_mac_lock, flags);
    ret = rtk_dwc3_process_event_buf();
	spin_unlock_irqrestore(&g_rtk_dwc3.usb_device_mac_lock, flags);
	local_bh_enable();

	return ret;
}

irqreturn_t rtk_usb_uvc_interrupt(int irq, void *_evt)
{
     UINT32 temp;

     temp = rtd_inl(UVC_VGIP_UVC_VGIP_STATUS_reg);
     if(UVC_VGIP_UVC_VGIP_STATUS_get_uvc_vlc_status(temp) == 1) {
         if(g_rtk_dwc3.uvc_show_log) {
             RTK_USB_DEVICE_DBG( "LC [%d %d %d]\n", uvc_frame_buffer_status[0], uvc_frame_buffer_status[1], uvc_frame_buffer_status[2]);
         }
         rtd_outl(UVC_VGIP_UVC_VGIP_STATUS_reg, temp & ~(UVC_VGIP_UVC_VGIP_STATUS_uvc_vlc_status(0)));
         g_rtk_dwc3.frame_lc_done = 1;
         return IRQ_WAKE_THREAD;
     }

     temp = rtd_inl(UVC_DMA_UVC_top_int_data_reg);
     if(UVC_DMA_UVC_top_int_data_get_uvc_cap_dma_wdone(temp) == 1) {
         if(g_rtk_dwc3.uvc_show_log) {
             RTK_USB_DEVICE_DBG( "Done [%d %d %d]\n", uvc_frame_buffer_status[0], uvc_frame_buffer_status[1], uvc_frame_buffer_status[2]);
         }
         rtd_outl(UVC_DMA_UVC_top_int_data_reg, UVC_DMA_UVC_top_int_data_uvc_cap_dma_wdone(1));
         g_rtk_dwc3.wdma_wdone = 1;
         return IRQ_WAKE_THREAD;
     }

     if(UVC_DMA_UVC_top_int_data_get_uvc_timer_done(temp) == 1) {
         if(g_rtk_dwc3.uvc_show_log) {
             RTK_USB_DEVICE_DBG( "Timer [%d %d %d]\n", uvc_frame_buffer_status[0], uvc_frame_buffer_status[1], uvc_frame_buffer_status[2]);
         }
         rtd_outl(UVC_DMA_UVC_top_int_data_reg, UVC_DMA_UVC_top_int_data_uvc_timer_done(1));
         if( g_rtk_dwc3.timer_done == 1) {
             return IRQ_HANDLED;
         } else {
             g_rtk_dwc3.timer_done = 1;
             return IRQ_WAKE_THREAD;
         }
     }

     temp = rtd_inl(MJPEG_ENC_mjpg_enc_int_status_reg);
     if(MJPEG_ENC_mjpg_enc_int_status_get_enc_done(temp) == 1) {
         UINT32 bitcount=0,crc=0,qid=0;
         rtd_outl(MJPEG_ENC_mjpg_enc_int_status_reg,MJPEG_ENC_mjpg_enc_int_status_enc_done(1)|MJPEG_ENC_mjpg_enc_int_status_write_data(0));
         bitcount=rtd_inl(MJPEG_ENC_vlc_bitcnt_reg)>>3;
         crc=rtd_inl(MJPEG_ENC_enc_crc_reg);
         qid=rtd_inl(MJPEG_ENC_mjpg_enc_quant_reg);
         if(g_rtk_dwc3.uvc_show_log) {
             RTK_USB_DEVICE_DBG_SIMPLE("MJENC=[%d %d %x]\n",bitcount,qid,crc);
         }
         return IRQ_WAKE_THREAD;
     }

     return IRQ_HANDLED;
}

void sw_gen_header(void);
void sw_gen_trb(void);

extern UINT32 trb_number;
extern UINT32 last_frame_size;
extern UINT32 frame_rate;
extern UINT16 usb_uvc_width_real;
void gen_frame(void);

UINT8 wait_wdone = 0;

irqreturn_t rtk_usb_uvc_thread_interrupt(int irq, void *_evt)
{
    if(g_rtk_dwc3.timer_done == 1) {
#if 1
            gen_header_trb();
#else
            sw_gen_header();
            sw_gen_trb();
#endif
            frame_index++;
            frame_index = frame_index & 0x1;

            g_rtk_dwc3.timer_done = 0;
            //return IRQ_HANDLED;
    }

    if(g_rtk_dwc3.frame_lc_done == 1) {

            if(wait_wdone == 0 ) {
                wait_wdone = 1;
                gen_frame();
            }
            g_rtk_dwc3.frame_lc_done = 0;
            //return IRQ_HANDLED;
    }

    if(g_rtk_dwc3.wdma_wdone == 1) {
#if 1
            if(uvc_frame_buffer_wp_old != uvc_frame_buffer_wp)
                uvc_frame_buffer_status[uvc_frame_buffer_wp_old] = 1;
#else
            uvc_frame_buffer_status[uvc_frame_buffer_wp] = 1;
#endif
            if(usb_uvc_format_real == _USB_UVC_MotionJPEG) {
                UINT32 temp1, temp2;
                UINT8 *vir;
                temp1 = rtd_inl(UVC_DMA_UVC_MJPEG_SIZE_0_reg);
                temp2 = rtd_inl(UVC_DMA_UVC_MJPEG_SIZE_1_reg);
                vir = uvc_frame_vir[uvc_frame_buffer_wp_old];
                uvc_frame_buffer_real_size[uvc_frame_buffer_wp_old] = temp1*16+temp2;
                vir = vir + temp1*16+temp2;
                vir = vir - 4;
                if(g_rtk_dwc3.uvc_show_log) {
                    RTK_USB_DEVICE_DBG("%d %d %d %02x %02x %02x %02x\n", temp1, temp2, temp1*16+temp2, *(vir+0), *(vir+1), *(vir+2), *(vir+3));
                }
            }
            if(g_rtk_dwc3.uvc_show_log) {
                RTK_USB_DEVICE_DBG("complet_frame wp=%d [%d %d %d]\n", uvc_frame_buffer_wp, uvc_frame_buffer_status[0], uvc_frame_buffer_status[1], uvc_frame_buffer_status[2]);
            }

            wait_wdone = 0;
            g_rtk_dwc3.wdma_wdone = 0;
            //return IRQ_HANDLED;
    }
    
    return IRQ_HANDLED;
}

irqreturn_t rtk_usb_uvc_vgip_interrupt(int irq, void *_evt)
{
    UINT32 temp;
    RTK_USB_DEVICE_DBG_SIMPLE( "+v\n");
    temp = rtd_inl(UVC_VGIP_UVC_VGIP_STATUS_reg);
    if(UVC_VGIP_UVC_VGIP_STATUS_get_uvc_vlc_status(temp) == 0) {
        return IRQ_NONE;
    }
   rtd_outl(UVC_VGIP_UVC_VGIP_STATUS_reg, UVC_VGIP_UVC_VGIP_STATUS_uvc_vlc_status(1));
    return IRQ_WAKE_THREAD;
}

irqreturn_t rtk_usb_uvc_vgip_thread_interrupt(int irq, void *_evt)
{
    UINT32 temp;

    RTK_USB_DEVICE_DBG_SIMPLE( "-v\n");
    temp = uvc_frame_buffer_wp + 1;
    temp = temp % UVC_FRAME_BUFFER_NUMBER;

    if(uvc_frame_buffer_status[temp] == 0) {
        uvc_frame_buffer_wp = temp;
    }

    switch(g_rtk_dwc3.uvc_plant_cnt) {
    case 1:
        rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
        break;
    case 2:
        rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
        rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_cap1_dma_sta_addr(uvc_frame_phy[0]) + usb_uvc_width_real*(usb_uvc_length_real/2)*2);
        break;
    case 3:
        rtd_outl(UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP0_DMA_WR_Start_addr_cap0_dma_sta_addr(uvc_frame_phy[0]));
        rtd_outl(UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP1_DMA_WR_Start_addr_cap1_dma_sta_addr(uvc_frame_phy[0]) + usb_uvc_width_real*(usb_uvc_length_real/2)*2);
        rtd_outl(UVC_DMA_UVC_CAP2_DMA_WR_Start_addr_reg, UVC_DMA_UVC_CAP2_DMA_WR_Start_addr_cap2_dma_sta_addr(uvc_frame_phy[0]) + usb_uvc_width_real*(usb_uvc_length_real/2)*2 + usb_uvc_width_real*(usb_uvc_length_real/2/2/2)*2);
        break;
    default:
        break;
    }

    rtd_outl(UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_reg, UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_db_read_sel(1)|UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_db_apply(1)|UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_db_en(1));

    return IRQ_HANDLED;
}

