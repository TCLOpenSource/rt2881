#include "arch.h"
#include "mach.h"
#include "usb_mac.h"
#include <rbus/usb3_top_reg.h>
#include <rbus/efuse_reg.h>
#include <rbus/sys_reg_reg.h>
#include "usb_reg.h"
#include <linux/timer.h>
#include "rtk_io.h"
#include "usb_phy.h"
#include "usb_cdc_api.h"
#include "usb_buf_mgr.h"
#include "software_feature.h"
#include "msg_queue_def.h"
#include "msg_q_function_api.h"
#include "msg_queue_ap_def.h"
#include <rbus/mis_gpio_reg.h>
#include "user_data.h"
#include <rtk_kdriver/io.h>
#include <linux/delay.h>
#include "rtk_usb_device_dbg.h"

#define TRUE 1
#define FALSE 0

extern UINT32 reg;
extern UINT32 dwc_temp1;
extern UINT32 u32_uvc_temp_2;
extern UINT8 g_gen2link_cnt;
extern struct dwc3 g_rtk_dwc3;
extern struct usb_uart g_usb_uart;
extern UINT8 rtk_dwc3_usb2_0_only;
extern UINT8 rtk_dwc3_usb3_0_5G_only;
extern struct usb_endpoint_descriptor dwc3_gadget_ep0_desc;
extern UINT8 g_recv_buf[150];
extern UINT8 only_support_cdc;
extern struct usb_ctrlrequest g_ep0_ctrl_req;
extern struct dwc3_trb g_ep0_trbs[2];
extern UINT8 *g_usb_iram;       // usb buffer virtual address
extern UINT32 g_usb_iram_dma;       // usb buffer PHY address


UINT8* rtk_dwc3_usb_sram_alloc(UINT16 len, UINT8 align);
UINT8 rtk_dwc3_gadget_run_stop(UINT8 is_on);
UINT8 dwc3_usb3_load_mac(void) __banked;
extern UINT8 rtk_dwc3_is_usb_control_by_application(void) __banked;
UINT8 usb_dp_switch_status;
extern UINT8 uac_device_to_host_trb_cnt;
extern UINT8 uac_device_to_host_trb_cnt_2;


//#pragma nogcse
//PRAGMA_CODESEG(BANK41);
//PRAGMA_CONSTSEG(BANK41);

void rtk_dwc3_init_uac_show_info(void)
{
        //pm_printk(0, "uac i:%d ", (UINT32)GetUAC_DownStream_Enable());
        //pm_printk(0, "o1:%d o2:%d\n", (UINT32)GetUAC_UpStream_UAC1_Enable(), (UINT32)GetUAC_UpStream_UAC2_Enable());
        //pm_printk(0, "pcm     = %d\n", (UINT32)GetAudio_PCM_Audio_Function());
        //pm_printk(0, "uac d2h trb cnt  = %d\n", (UINT32)uac_device_to_host_trb_cnt);
        //pm_printk(0, "uac d2h trb cnt 2= %d\n", (UINT32)uac_device_to_host_trb_cnt_2);
}

void rtk_usb_driver_check_sw_config_by_sw_config(void)
{
    if(GetTypeC_Fuction() == _TYPEC_USB2_ONLY || GetTypeC_Fuction() == _TYPEC_USB2_DP_1_4) {
            rtk_dwc3_usb2_0_only = 1;
    } else if(GetTypeC_Fuction() == _TYPEC_USB2_USB31 || GetTypeC_Fuction() == _TYPEC_USB2_USB31_DP_1_4) {
            rtk_dwc3_usb3_0_5G_only = 1;
    }
    if( (GetUVC_Enable() == _FUNCTION_DISABLE) && (GetUAC_Enable() == _FUNCTION_DISABLE)) {
            only_support_cdc = 1;
    }
}

void rtk_usb_driver_check_sw_config_by_user_data(void)
{
    if(GetUSBForceConnectSpeed() == 1) {
        pm_printk(LOGGER_FATAL, "[USB] user data force U3 5G\n");
        rtk_dwc3_usb3_0_5G_only = 1;
    }
    if(GetUSBForceConnectSpeed() == 2) {
        pm_printk(LOGGER_FATAL, "[USB] user data force U2\n");
        rtk_dwc3_usb2_0_only = 1;
    }
}

void rtk_usb_driver_check_sw_config_by_pd_controller(void)
{
    if(rtk_dwc3_is_usb_control_by_application() == TRUE) {
        // USB On Off control from the AP
        if(usb_dp_switch_status == USER_SWITCH_DP_MODE)
            rtk_dwc3_usb2_0_only = 1;
    }
}

UINT8 rtk_usb_driver_check_sw_config(void)
{
#ifndef USB_U2_ONLY_MODE
        //pm_printk(LOGGER_FATAL, "[USB] TypeC=%d ", (UINT32)GetTypeC_Fuction());
        //pm_printk(LOGGER_FATAL, "UVC=%d ", (UINT32)GetUVC_Enable());
        //pm_printk(LOGGER_FATAL, "UAC=%d ", (UINT32)GetUAC_Enable());
        //pm_printk(LOGGER_FATAL, "CDC=%d\n", (UINT32)GetFlashCdcOnOff());

        rtk_usb_driver_cdc_function_init();

        if(GetTypeC_Fuction() == _TYPEC_DISABLE)
                return 0;

        rtk_usb_driver_check_sw_config_by_sw_config();
        rtk_usb_driver_check_sw_config_by_user_data();
        rtk_usb_driver_check_sw_config_by_pd_controller();
#endif
    return 0;
}

void rtk_dwc3_probe_set_to_device(void)
{
        //set mode to device mode
        reg = rtd_inl(DWC3_GCTL);
        reg &= ~(DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG));
        reg |= DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_DEVICE);
        rtd_outl(DWC3_GCTL, reg);
}

extern unsigned long long usb_phy_vir_offset;
extern UINT8 *uvc_sram_addr_vir_cache;
extern UINT8 *uvc_sram_addr_vir_non_cache;

void rtk_dwc3_probe_prepare_memory(void)
{
        g_rtk_dwc3.ep0_ctrl_req = &g_ep0_ctrl_req;
        g_rtk_dwc3.ep0_ctrl_req_addr =  (UINT32)(rtk_dwc3_usb_sram_alloc(256, 16) - g_usb_iram) + g_usb_iram_dma;

        g_rtk_dwc3.ep0_trbs = &g_ep0_trbs[0];
        g_rtk_dwc3.ep0_trbs_addr = (UINT32)( rtk_dwc3_usb_sram_alloc(sizeof(struct dwc3_trb) * 2, 16) - g_usb_iram) + g_usb_iram_dma;

        g_rtk_dwc3.eps_addr = (UINT32)(rtk_dwc3_usb_sram_alloc(sizeof(struct dwc3_trb) * 16, 16) - g_usb_iram) + g_usb_iram_dma;

        g_usb_uart.recv_buf = g_recv_buf;
        g_usb_uart.recv_buf_addr =  (UINT32)(rtk_dwc3_usb_sram_alloc(LOG_RECV_BUF_SIZE, 16) - g_usb_iram) + g_usb_iram_dma;

        g_usb_uart.log_buf.buf = (UINT32)(rtk_dwc3_usb_sram_alloc(LOG_BUF_SIZE, 16) - g_usb_iram) + g_usb_iram_dma;
        g_usb_uart.log_buf.read_pos = g_usb_uart.log_buf.write_pos = 0;

        g_rtk_dwc3.ep0_setup_buf_addr = (UINT32)(rtk_dwc3_usb_sram_alloc(3072, 16) - g_usb_iram) + g_usb_iram_dma;
        g_rtk_dwc3.ep0_descriptor_table_buf =  (UINT32)(rtk_dwc3_usb_sram_alloc(DWC3_USB_DESCRIPTOR_BUFFER_SIZE, 16) - g_usb_iram) + g_usb_iram_dma;

        pm_printk(LOGGER_ERROR, "g_event   =0x%px\n", g_rtk_dwc3.g_event_buf.buf);
        pm_printk(LOGGER_ERROR, "ep0_ctrl=%x\n", (UINT32)g_rtk_dwc3.ep0_ctrl_req_addr);
        pm_printk(LOGGER_ERROR, "ep0_trbs=%x\n", (UINT32)g_rtk_dwc3.ep0_trbs_addr);
        pm_printk(LOGGER_ERROR, "eps=%x\n", (UINT32)g_rtk_dwc3.eps_addr);
        pm_printk(LOGGER_ERROR, "recv=%x\n", (UINT32)g_usb_uart.recv_buf_addr);
        pm_printk(LOGGER_ERROR, "log_buf=%x\n", (UINT32)g_usb_uart.log_buf.buf);
        pm_printk(LOGGER_ERROR, "ep0_setup=%x\n", (UINT32)g_rtk_dwc3.ep0_setup_buf_addr);
        pm_printk(LOGGER_ERROR, "descr=%x\n", (UINT32)g_rtk_dwc3.ep0_descriptor_table_buf);
}

void rtk_dwc3_probe_set_speed(void)
{
        reg = rtd_inl(DWC3_DCFG);
        reg &= ~(DWC3_DCFG_SPEED_MASK);
#ifdef USB_U2_ONLY_MODE
        reg |= DWC3_DSTS_HIGHSPEED;
#else
        if(rtk_dwc3_usb2_0_only == 1)
                reg |= DWC3_DSTS_HIGHSPEED;
        else {
                if(rtk_dwc3_usb3_0_5G_only)
                        reg |= DWC3_DSTS_SUPERSPEED;
                else
                        reg |= DWC3_DSTS_SUPERSPEED_PLUS;
        }
#endif
        rtd_outl(DWC3_DCFG, reg);
}

void rtk_dwc3_probe_set_ep0(void)
{
        if (_rtk_dwc3_gadget_ep_enable(0, &dwc3_gadget_ep0_desc, NULL, false)) {
                pm_printk(LOGGER_ERROR, "On EP%d NG\n", g_rtk_dwc3.eps[0].number);
                return;
        }

        if (_rtk_dwc3_gadget_ep_enable(1, &dwc3_gadget_ep0_desc, NULL, false)) {
                pm_printk(LOGGER_ERROR, "ON EP%d NG\n", g_rtk_dwc3.eps[1].number);
                return;
        }

        /* begin to receive SETUP packets */
        //rtk_dwc3_ep0_start_setup_phase
        g_rtk_dwc3.ep0state = EP0_SETUP_PHASE;
        rtk_dwc3_ep_start_trans(0, g_rtk_dwc3.ep0_ctrl_req_addr, 8,
                                DWC3_TRBCTL_CONTROL_SETUP);
}

void rtk_dwc3_probe_enable_irq(void)
{
        rtd_outl(DWC3_DEVTEN, (DWC3_DEVTEN_ULSTCNGEN |
                               DWC3_DEVTEN_CONNECTDONEEN |
                               DWC3_DEVTEN_USBRSTEN |
                               DWC3_DEVTEN_DISCONNEVTEN));
}

void rtk_dwc3_probe_polling_LTSSM(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "rtk_usb_device ### rtk_dwc3_probe_polling_LTSSM need to implement ----------------\n");
}

UINT8 rtk_dwc3_is_usb_control_by_application(void) __banked
{
    if(Get_Ext_DP_MUX_Select() == _Ext_DP_MUX_Select_EJ899M) {
        // DK520
        // USB On Off control from the AP
        return TRUE;
    }
#if 0
    if(Get_Ext_DP_MUX_Select() == _Ext_DP_MUX_Select_EJ899I) {
        // DK525
        // USB On Off control from the AP
        return TRUE;
    }
#endif
    return FALSE;
}

void USB_init(void);

extern UINT8 *EVENT_BUFFER;

bool rtk_dwc3_probe(void)
{
        int i;

        if(rtk_dwc3_usb2_0_only == 1)
                rtd_outl(0xb806917c, 0x1); // U2 only mode

        rtk_dwc3_init_endpoints();

        //if(!rtk_dwc3_soft_reset()) {   //thomas
        //        return false;
        //}

        dwc3_usb3_load_mac();
        //rtk_dwc3_core_soft_reset();
        rtk_dwc3_event_buffers_setup();
        rtk_dwc3_probe_set_to_device();
        rtk_dwc3_probe_prepare_memory();
        rtk_dwc3_probe_set_speed();

        g_rtk_dwc3.start_config_issued = false;
        rtk_dwc3_probe_set_ep0();
        rtk_dwc3_probe_enable_irq(); //enable dwc3 irqs

        for(i = 0;i< 100;i = i+4) {
            rtd_mem_outl(g_rtk_dwc3.g_event_buf.buf + i, 0);
        }

        if(rtk_dwc3_is_usb_control_by_application() == TRUE) {
            // USB On Off control from the AP
            core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_USB_TYPE_DETECT, (UINT32)MSG_USB_CONNECT_SPEED_UNKNOW);
        } else {
            rtk_dwc3_gadget_run_stop(1);
        }

        dwc_temp1 = (UINT32)rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
        //polling LTSSM main&sub state
        rtk_dwc3_probe_polling_LTSSM();
#ifdef CONFIG_ARCH_5281
        // for interrupt mode
        rtd_outl(SYS_REG_INT_CTRL_SCPU_2_reg,
                 SYS_REG_INT_CTRL_SCPU_2_usb_int_scpu_routing_en_mask |
                 SYS_REG_INT_CTRL_SCPU2_2_write_data(1));
#else
        // for interrupt mode
        //rtd_outl(0xB80002A4,
        //         0x00008000 |
        //         0x1);
        //rtd_outl(0xB80002A0,
        //         0x00080000 |
        //         0x1);
#endif

#ifdef CONFIG_ENABLE_GPIO
        //rtk_gpio_usb_detect(); // Add gpio detect
#endif
        return true;
}
