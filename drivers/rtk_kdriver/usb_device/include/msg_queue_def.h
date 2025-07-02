/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2006
  * All rights reserved.
  * ========================================================================*/

/*================= File Description =======================================*/
/**
 * @file
 * 	This file is for timer related functions.
 *
 * @author 	$Author$
 * @date		$Date$
 * @version	$Revision$
 * @ingroup	timer
 */

/**
 * @addtogroup timer
 * @{
 */

#ifndef _MSG_QUEUE_DEF_H_
#define _MSG_QUEUE_DEF_H_

/*============================ Module dependency  ===========================*/
#include "arch.h"
/*===================================  Types ================================*/
typedef enum MSG_Q_MAIN {
       MSG_OSD_ID = 0,         // 0
       MSG_HDMIRX_ID,          // 1
       MSG_HDMITX_ID,          // 2
       MSG_SCALER_ID,          // 3
       MSG_UVC_ID,             // 4
       MSG_DPRX_ID,            // 5
       MSG_HDCPRX_ID,          // 6
       MSG_HDCPTX_ID,          // 7
       MSG_CEC_ID,             // 8
       MSG_PD_ID,              // 9
       MSG_BB_ID,              // 10
       MSG_UAC_ID,             // 11
       MSG_Audio_ID,           // 12
       MSG_PQ_ID,              // 13
       MSG_HDR_ID,             // 14
       MSG_MJPEG_ID,           // 15
       MSG_POWER_ID,           // 16
       MSG_I2C_ID,             // 17
       MSG_AP_ID,              // 18
       MSG_USB_ID,             // 19
       MSG_GPIO_ID,            // 20
       MSG_HWMONITOR_ID,       // 21
       MSG_UART_ID,            // 22
       MSG_RTICE_ID,           // 23
       MSG_MCOMMAND_ID,        // 24
       MSG_UNKNOW_ID,          // +1
} _MSG_Q_MAIN;

typedef struct MSG_Q_PARM {
	UINT16  sub_id;                 // queue sub id defined in driver
	UINT32  Param;
} _MSG_Q_PARM;

typedef struct MSG_Q_EVENT_FUNCTION_HANDLER {
	void (*MsgEventHandler)(_MSG_Q_PARM*);
} _MSG_Q_EVENT_FUNCTION_HANDLER;

typedef struct MSG_Q_EVENT_ITEM_STRUCT {
#if defined (CONFIG_TED_RECORD_TIMER_Q_DELAY)
	UINT32	expired_tick;
#endif //#if defined (CONFIG_TED_RECORD_TIMER_Q_DELAY)
	UINT8	fActive;
	_MSG_Q_MAIN	msg_no;             // queue id
	_MSG_Q_PARM	PassParam;
} _MSG_Q_EVENT_ITEM_STRUCT;

// Constant definition (2 power units)
#define MAX_FW_EVENT_AMOUNT		16
#define MAX_ISR_EVENT_AMOUNT	16
#define MAX_TIMER_EVENT_AMOUNT	16

#define MSG_EVENT_BANK BANK1


/*  */
typedef enum {
    E_MSG_Q_ISR = 0,
    E_MSG_Q_TMR,
    E_MSG_Q_FW,
    E_MSG_Q_TYPES,
    E_MSG_Q_UNKNOWN = E_MSG_Q_TYPES,
} E_MSG_Q;

typedef enum {
    E_MSG_Q_RESULT_CODE_ALREADY_EXIST = 0xFE,
    E_MSG_Q_RESULT_CODE_IS_FULL = 0xFF,
} E_MSG_Q_RESULT_CODE;

/*  */
typedef struct PROBE_TIME_FROM_ST {
    UINT32  u32_power_on;
    UINT32  u32_rx_plug_in;
    UINT32  u32_tx_plug_in;
    UINT32  u32_rx_timing_chg;
} _PROBE_TIME_FROM_ST;

typedef struct PROBE_PREV_COUNT_ST {
    UINT32  u32_power_on;
    UINT32  u32_rx_plug_in;
    UINT32  u32_tx_plug_in;
    UINT32  u32_rx_timing_chg;
} _PROBE_PREV_COUNT_ST;

extern volatile UINT32 g_curr_wdg_free_count;
extern _PROBE_TIME_FROM_ST probe_time_from[1];
extern _PROBE_PREV_COUNT_ST probe_prev_count[1];

#define WDT_27M_HWCNT       0xb8062224

#define AP_PROBE_POWER_ON_ENTRY() \
    do {\
        if (probe_prev_count->u32_power_on == 0) {\
            probe_prev_count->u32_power_on = rtd_inl(WDT_27M_HWCNT);\
        }\
    }while(0)

#define AP_PROBE_HDMIRX_PLUGIN_ENTRY() \
    do {\
        if (probe_prev_count->u32_rx_plug_in == 0) {\
            probe_prev_count->u32_rx_plug_in = rtd_inl(WDT_27M_HWCNT);\
        }\
    }while(0)

#define AP_PROBE_HDMITX_PLUGIN_ENTRY() \
    do {\
        if (probe_prev_count->u32_tx_plug_in == 0) {\
            probe_prev_count->u32_tx_plug_in = rtd_inl(WDT_27M_HWCNT);\
        }\
    }while(0)

#define AP_PROBE_HDMIRX_TIMING_CHG_ENTRY() \
    do {\
        if (probe_prev_count->u32_rx_timing_chg == 0) {\
            probe_prev_count->u32_rx_timing_chg = rtd_inl(WDT_27M_HWCNT);\
        }\
    }while(0)


#ifdef CONFIG_MSG_PROFILE_ENABLE

#define MSG_ANY_MSG_ID                  (0xff)
#define MSG_ANY_SUB_ID                  (0xffff)
#define MSG_IRQ_AMOUNT                  (8)

typedef struct MSG_Q_PROFILE_ST {
    UINT8           u8_on;
    UINT8           u8_print;
    UINT16          u16_trig_event;
    _MSG_Q_MAIN     e_main_id;
    UINT16          u16_sub_id;
    UINT32          u32_time[E_MSG_Q_TYPES];
    UINT16          u16_irq_count[MSG_IRQ_AMOUNT];
    UINT32          u32_banksw_count;
    UINT32          u32_on_off_time;
} _MSG_Q_PROFILE_ST;

extern volatile UINT8              msg_no;
extern volatile UINT16             msg_sub_id;
extern volatile _MSG_Q_PROFILE_ST  msg_q_profile[1];

void msg_profile_on(_MSG_Q_MAIN u8_msg_id,
                    UINT16      u16_sub_id) __banked;
void msg_profile_off(void)                  __banked;
void msg_profile_fn_exit(UINT8 u8_tag);


#define MSG_PROFILE_ON(_e_main_id, _u16_sub_id) \
    do {\
        msg_profile_on(_e_main_id, _u16_sub_id);\
    } while(0)

#define MSG_PROFILE_OFF() \
    do {\
        msg_profile_off();\
    } while(0)

#define MSG_PROFILE_ENTRY(_e_msg_q) \
    do {\
        msg_profile_entry();\
    } while(0)

#define MSG_PROFILE_EXIT(_e_msg_q) \
    do {\
        if (msg_q_profile->u8_on) {\
            event_period = rtd_inl(WDT_27M_HWCNT) - tmp_systick;\
            if (msg_q_profile->e_main_id == msg_no || msg_q_profile->e_main_id == MSG_ANY_MSG_ID) {\
                if (msg_q_profile->u16_sub_id == msg_sub_id || msg_q_profile->u16_sub_id == MSG_ANY_SUB_ID) {\
                    msg_q_profile->u32_time[_e_msg_q] += event_period;\
                    MSG_PROFILE_PRINT(_e_msg_q);\
                }\
            }\
        }\
    } while(0)

#define MSG_PROFILE_PRINT(_e_msg_q) \
    do {\
        if (msg_q_profile->u8_print) {\
            tmp_systick = rtd_inl(EMCU_dummy_3_reg) - msg_q_profile->u32_banksw_count;\
            msg_profile_print();\
        }\
    } while(0)

#define MSG_PROFILE_IRQ(_irq_num) \
    do {\
        if (msg_q_profile->u8_on) {\
            msg_q_profile->u16_irq_count[_irq_num]++;\
        }\
    } while(0)

#define MSG_PROFILE_FN_ENTRY(_u8_tag) \
    do {\
        msg_profile_fn_entry();\
    } while(0)

#define MSG_PROFILE_FN_EXIT(_u8_tag) \
    do {\
        msg_profile_fn_exit(_u8_tag);\
    } while(0)

#define MSG_PROFILE_TRIG_EVENT_EQUAL(_u16_event)    (msg_q_profile->u16_trig_event == _u16_event)
#define MSG_PROFILE_TRIG_EVENT_GET()                (msg_q_profile->u16_trig_event)
#define MSG_PROFILE_TRIG_EVENT_SET(_u16_event)      (msg_q_profile->u16_trig_event = _u16_event)
#define MSG_PROFILE_PRINT_GET()                     (msg_q_profile->u8_print)
#define MSG_PROFILE_PRINT_SET(_u8_print)            (msg_q_profile->u8_print = _u8_print)

#else

#define MSG_PROFILE_ON(_e_main_id, _u16_sub_id)
#define MSG_PROFILE_OFF()
#define MSG_PROFILE_IRQ(_irq_num)
#define MSG_PROFILE_FN_ENTRY(_u8_tag)
#define MSG_PROFILE_FN_EXIT(_u8_tag)
#define MSG_PROFILE_TRIG_EVENT_EQUAL(_u16_event)    (0)
#define MSG_PROFILE_TRIG_EVENT_GET()                (0)
#define MSG_PROFILE_TRIG_EVENT_SET(_u16_event)
#define MSG_PROFILE_PRINT_GET()                     (0)
#define MSG_PROFILE_PRINT_SET(_u8_print)


#endif  //CONFIG_MSG_PROFILE_ENABLE


#ifdef CONFIG_MSG_MONITOR_ENABLE

#define  MSG_Q_RUNTIME_20MS         (540000)

typedef struct MSG_Q_MONITOR_ST {
    UINT8           u8_print;
    UINT8           u8_capture;
    UINT8           u8_msg_q;
    _MSG_Q_MAIN     e_main_id;
    UINT16          u16_sub_id;
    UINT32          u32_take_time;
    UINT32          u32_timer1_isr_take_time;
    UINT32          u32_thr;
} _MSG_Q_MONITOR_ST;

extern UINT32 mnt_prev_wdg_free_count;
extern UINT32 mnt_prev_isr_wdg_free_count;
extern UINT8  mnt_msg_q;
extern UINT8  mnt_msg_no;
extern UINT16 mnt_msg_sub_id;
extern _MSG_Q_MONITOR_ST msg_q_monitor[1];

#define MSG_MNT_INIT() \
    do {\
        mnt_msg_q               = E_MSG_Q_UNKNOWN;\
	    mnt_msg_no              = MSG_UNKNOW_ID;\
	    mnt_msg_sub_id          = 0xffff;\
	    mnt_prev_wdg_free_count = rtd_inl(WDT_27M_HWCNT);\
    } while(0)

#define MSG_MNT_PRINT() \
    do {\
        extern volatile UINT32 g_curr_wdg_free_count;\
	    g_curr_wdg_free_count = rtd_inl(WDT_27M_HWCNT) - mnt_prev_wdg_free_count;\
	    if (g_curr_wdg_free_count > msg_q_monitor->u32_thr) {\
            if (msg_q_monitor->u8_capture) {\
                if (g_curr_wdg_free_count > msg_q_monitor->u32_take_time) {\
                    msg_q_monitor->u8_msg_q      = mnt_msg_q;\
                    msg_q_monitor->e_main_id     = mnt_msg_no;\
                    msg_q_monitor->u16_sub_id    = mnt_msg_sub_id;\
                    msg_q_monitor->u32_take_time = g_curr_wdg_free_count;\
                }\
            }\
            if (msg_q_monitor->u8_print) {\
	        }\
	    }\
    } while(0)

#define MSG_MNT_RUN() \
    do {\
        mnt_prev_wdg_free_count = rtd_inl(WDT_27M_HWCNT);\
    } while(0)

#define MSG_MNT_TIMER_1_ISR_ENTRY() \
    do {\
        mnt_prev_isr_wdg_free_count = rtd_isr_inl(WDT_27M_HWCNT);\
    } while(0)

#define MSG_MNT_TIMER_1_ISR_EXIT() \
    do {\
        extern volatile UINT32 g_curr_isr_wdg_free_count;\
        g_curr_isr_wdg_free_count = rtd_isr_inl(WDT_27M_HWCNT) - mnt_prev_isr_wdg_free_count;\
        if (msg_q_monitor->u8_capture) {\
            if (g_curr_isr_wdg_free_count > msg_q_monitor->u32_timer1_isr_take_time) {\
                msg_q_monitor->u32_timer1_isr_take_time = g_curr_isr_wdg_free_count;\
            }\
        }\
    } while(0)

#define MSG_MNT_THR_GET()                   (msg_q_monitor->u32_thr)
#define MSG_MNT_THR_SET(_u32_val)           (msg_q_monitor->u32_thr = _u32_val)
#define MSG_MNT_PRINT_GET()                 (msg_q_monitor->u8_print)
#define MSG_MNT_PRINT_SET(_u8_on)           (msg_q_monitor->u8_print = _u8_on)
#define MSG_MNT_CAPTURE_GET()               (msg_q_monitor->u8_capture)
#define MSG_MNT_CAPTURE_SET(_u8_on)         (msg_q_monitor->u8_capture = _u8_on)
#define MSG_MNT_Q_GET()                     (msg_q_monitor->u8_msg_q)
#define MSG_MNT_Q_CLR()                     (msg_q_monitor->u8_msg_q = 0)
#define MSG_MNT_MSG_MAIN_ID_GET()           (msg_q_monitor->e_main_id)
#define MSG_MNT_MSG_MAIN_ID_CLR()           (msg_q_monitor->e_main_id = 0)
#define MSG_MNT_SUB_ID_GET()                (msg_q_monitor->u16_sub_id)
#define MSG_MNT_SUB_ID_CLR()                (msg_q_monitor->u16_sub_id = 0)
#define MSG_MNT_TAKE_TIME_GET()             (msg_q_monitor->u32_take_time)
#define MSG_MNT_TAKE_TIME_CLR()             (msg_q_monitor->u32_take_time = 0)
#define MSG_MNT_TIMER1_ISR_TAKE_TIME_GET()  (msg_q_monitor->u32_timer1_isr_take_time)
#define MSG_MNT_TIMER1_ISR_TAKE_TIME_CLR()  (msg_q_monitor->u32_timer1_isr_take_time = 0)


#else

#define MSG_MNT_INIT()
#define MSG_MNT_PRINT()
#define MSG_MNT_RUN()
#define MSG_MNT_TIMER_1_ISR_ENTRY()
#define MSG_MNT_TIMER_1_ISR_EXIT()

#endif  //CONFIG_MSG_MONITOR_ENABLE


#endif // #ifndef _MSG_QUEUE_DEF_H_
/*======================== End of File =======================================*/
/**
*
* @}
*/
