#include "arch.h"
#include <rbus/timer_reg.h>
#include "mach.h"
#include "usb_mac.h"

//****************************************************************************
// MARCO DECLARATIONS
//****************************************************************************
#define UAC_READ_REGISTER_RETRY_CNT 10
#define SIZE_PER_TRB 16
#define TIMER_90K_reg              0x180136B8
#define TIMER_addr                 0x47ff5a40
#define UAC_IN_THRESHOLD_HIGH        30
#define UAC_IN_THRESHOLD_LOW         10

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
UINT32 trb_wr_addr_last = 0;
UINT32 trb_wr_addr_now = 0;
UINT32 temp1;
UINT32 trb_w;
UINT32 trb_r;
UINT32 hw_own_cnt;
UINT8  retry_cnt = 0;
UINT8 UAC_HW_BUG_flag;
UINT8 once_90k_log = 0;
UINT8 UAC_HW_BUG_cnt = 0;
UINT8 u8_timer_uac_isr = 0;
extern UINT8  uac_device_to_host_trb_cnt;

extern UINT32 uac_host_to_device_trb_addr;
extern UINT32 uac_host_to_device_trb_cnt;
extern UINT32 g_UAC_IN_base_mps_size;
//extern void reset_uac_proc(void) __banked;


UINT32 uac_temp1_u32;
UINT16 uac_temp1_u16;
UINT8 audio_speed = 128;

UINT16 audio_sample_number = 192;
UINT8 uac_timer_run_cnt = 0;

#if 0
extern UINT32 uac_out_check_address;
extern UINT32 uac_out_start_address;
extern UINT32 uac_out_end_address;

extern BOOL g_dac_out_hw_mute;

extern char g_isTone;
extern char g_isToneResultUpdate;
extern BOOL g_dac_out_user_enable;
extern BOOL g_forceStopClockPatch;// for debug

void drv_print_isr_str(char __code * ch);
void drv_print_isr_val(UINT32 val, UINT8 len);

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************

void uac_adj_clock_control_core(void)
{
    rtd_isr_outl(0x18000134, 0x00400000);
    rtd_isr_outl(0x1800602c, 0x12000000);
    rtd_isr_outl(0x18000134, 0x00400001);
    rtd_isr_outl(0x1800602c, 0x32000000);
}

void uac_adj_clock_control(void)
{
    if(g_forceStopClockPatch)// debug
        return;

    if(g_dac_out_user_enable == TRUE){
        rtd_isr_outl(0x18006b38, 0x801c0000);
        rtd_isr_outl(0x18006b3c, 0x001c0000);
        uac_adj_clock_control_core();
        rtd_isr_outl(0x18006b38, 0x80180000);
        rtd_isr_outl(0x18006b3c, 0x00180000);
    } else {
        uac_adj_clock_control_core();
    }
}

void uac_adj_clock(void)
{
    //drv_print_isr_val(g_isTone, 1);
    if(g_dac_out_hw_mute == FALSE) {
        // HP plug in
        if( g_isTone == 1 && g_isToneResultUpdate == 1) {
            // is tone
            return;
        }
    }
    //drv_print_isr_str("#\n");

    if(uac_timer_run_cnt & 0x1) {
        if(audio_speed >= 128) {
            uac_adj_clock_control();
        }
    } else {
        if(audio_speed > 128) {
            uac_adj_clock_control();
        }
    }
}

void timer_uac_isr(void)
{
    u8_timer_uac_isr = 0;
    while(1) {
        uac_temp1_u32 = rtd_isr_mem_inl(uac_out_check_address + 0xc);
        if( uac_temp1_u32 & 0x1) {
            uac_temp1_u16 = rtd_isr_mem_inl(uac_out_check_address + 0x8) & 0xffff;
            if( uac_temp1_u16 > audio_sample_number)
                audio_speed++;
            if(uac_temp1_u16 < audio_sample_number)
                audio_speed--;
            uac_out_check_address = uac_out_check_address + 0x10;
            if(uac_out_check_address >= uac_out_end_address) {
                uac_out_check_address = uac_out_start_address;
                //pm_printk(0, "wrap\n");
            }
            u8_timer_uac_isr++;
        } else {
            //pm_printk(0, "%d %d\n", uac_out1_check_address, (UINT32)audio_speed);
            break;
        }
        if(u8_timer_uac_isr > uac_device_to_host_trb_cnt)
            break;
    }

    uac_adj_clock();

    uac_timer_run_cnt++;

#if 0
    //if((rtd_inl(0xB8006400) & _BIT0) == _BIT0)
    if(trb_wr_addr_last != 0)
    {
        //rtd_mem_outl(TIMER_addr + once_90k_log, rtd_inl(TIMER_90K_reg));

        retry_cnt = 0;

        //0xb8006470[5:0] HWO_SET_NUM > (TRB_NUM + THRESHOLD )/ 2 RESET FLOW
        //if((rtd_inl(0xB8006470) & 0x3F) > 0x23) {
        //    reset_uac_proc();
        //}

        while( trb_wr_addr_now != (rtd_inl(0xB800649C) & 0xfffffff0))
        {
            trb_wr_addr_now = (rtd_inl(0xB800649C) & 0xfffffff0);
            if(trb_wr_addr_now < uac_host_to_device_trb_addr)
                trb_wr_addr_now = 555;
            retry_cnt++;
            if(retry_cnt >= UAC_READ_REGISTER_RETRY_CNT)
                break;
            rtd_mem_outl(0x1a3035b0, trb_wr_addr_now);
        }

        if(retry_cnt < UAC_READ_REGISTER_RETRY_CNT)
        {
            while(trb_wr_addr_last != trb_wr_addr_now)
            {
                if(trb_wr_addr_now == 0)
                    break;
                temp1 = trb_wr_addr_last & 0xfffffff8;

                UAC_HW_BUG_flag = ((rtd_mem_inl(temp1 + 0x0c) & _BIT0) == 0) ? 1 : 0;

                rtd_mem_outl(0x1a3035b4, trb_wr_addr_last);
                rtd_mem_outl(0x1a3035b8, trb_wr_addr_now);

                if(UAC_HW_BUG_flag)
                {
                    // sram before
                    rtd_mem_outl(0x1a3035c0, rtd_mem_inl(temp1 + 0x08));
                    rtd_mem_outl(0x1a3035c4, rtd_mem_inl(temp1 + 0x0c));
                    rtd_mem_outl(temp1 + 0x08, (UINT32)g_UAC_IN_base_mps_size);
                    rtd_mem_outl(temp1 + 0x0c, 0x00000069);
                    // sram after
                    rtd_mem_outl(0x1a3035c8, rtd_mem_inl(temp1 + 0x08));
                    rtd_mem_outl(0x1a3035cc, rtd_mem_inl(temp1 + 0x0c));
                    UAC_HW_BUG_cnt++;
                }
                trb_wr_addr_last = trb_wr_addr_last + SIZE_PER_TRB;
                if(trb_wr_addr_last >= (uac_host_to_device_trb_addr + (uac_host_to_device_trb_cnt * SIZE_PER_TRB))) {
                    trb_wr_addr_last = uac_host_to_device_trb_addr;
                }
            }
        }
        //rtd_mem_outl(TIMER_addr + once_90k_log + 0x4, rtd_inl(TIMER_90K_reg));
        //once_90k_log = once_90k_log + 0x10;

#if 0
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Patch for SW Tracking
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get the trb read write address
        while( trb_w != (rtd_inl(0xb800649c) & 0xfffffff0)) {
            trb_w = rtd_inl(0xb800649c) & 0xfffffff0;
        }
        while( trb_r != (rtd_inl(0xb800646c) & 0xfffffff0)) {
            trb_r = rtd_inl(0xb800646c) & 0xfffffff0;
        }

        // Calcus the HWO cnt
        if(trb_w > trb_r) {
            hw_own_cnt = (trb_w - trb_r) / SIZE_PER_TRB + 1;
        } else {
            hw_own_cnt = (trb_w + uac_host_to_device_trb_cnt * SIZE_PER_TRB - trb_r) / SIZE_PER_TRB + 1;
        }

        // Decide the div1 or div2
        temp1 = rtd_inl(0x18006b00);
        temp1 = temp1 & 0x1FFFFFFF;
        if(hw_own_cnt > UAC_IN_THRESHOLD_HIGH) {
            temp1 = temp1 + ((UINT32)1 << 29); // div2
            rtd_outl(0x18006b00, temp1);
        }
        else if(hw_own_cnt < UAC_IN_THRESHOLD_LOW) {
            temp1 = temp1 + ((UINT32)0x11 << 29); // mul2
            rtd_outl(0x18006b00, temp1);
        }
        else {
            temp1 = temp1 + ((UINT32)0 << 29); // div1
            rtd_outl(0x18006b00, temp1);
        }
        rtd_mem_outl(0x1a3035d0, trb_w);
        rtd_mem_outl(0x1a3035d4, trb_r);
        rtd_mem_outl(0x1a3035d8, hw_own_cnt);
#endif

    }
    rtd_mem_outl(0x1a3035bc, UAC_HW_BUG_cnt);
#endif
}
#endif
