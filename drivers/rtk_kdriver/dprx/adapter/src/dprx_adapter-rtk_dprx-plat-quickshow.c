/*=============================================================
 * File:    dprx_adapter-rtk_dprx-platform-quickshow.c
 *
 * Desc:    driver wrapper for quick show
 *
 * AUTHOR:  kevin_wang@realtek.com
 *
 * Vresion: 0.0.1
 *
 *------------------------------------------------------------
 * Copyright (c) Realtek Semiconductor Corporation, 2022
 *
 * All rights reserved.
 *
 *============================================================*/
#include <dprx_adapter.h>
#include <dprx_adapter-rtk_dprx-priv.h>
#include <dprx_adapter-rtk_dprx-plat.h>
#include <dprx_vfe_api.h>
//#include <dprx_vfe.h>
#include <rbus/sys_reg_reg.h>

extern UINT8 newbase_dprx_aux_get_irq_status_exint0(UINT8 port);
extern UINT8 lib_dprx_dpcd_get_link_config_wr_irq_exint0(UINT8 port);
extern UINT8 _check_link_status(UINT8 port);
extern void _aux_hpd_irq_assert_exint0(UINT8 port);
extern dprx_dev_t* _vfe_dprx_drv_get_dprx_adapter(unsigned char ch);
#ifdef ENABLE_DPRX_LT_EVENT_MONITOR

//#define VERBOSE_QS_LT_LOG  // enable it for more detail info

#ifdef VERBOSE_QS_LT_LOG

typedef struct
{
    UINT8 link_lane_count;
    UINT8 link_bw;
    UINT8 link_round;
    UINT8 link_state;
    UINT8 link_sub_state;
    UINT8 sink_status[3];
    UINT32 time_stamp;
}QS_LT_STATUS;

#define QS_EVENT_LOG_SIZE   16
UINT8 lt_status_idx = 0;
QS_LT_STATUS g_qs_lt_status[QS_EVENT_LOG_SIZE];

/*------------------------------------------------------------------
 * Func : rtk_dprx_lt_event_message_enable
 *
 * Desc : link training debug message control
 *
 * Parm : p_dprx : handle of DPRX
 *
 * Retn : 0 : event message disable, 1 : event message enable
 *------------------------------------------------------------------*/
int rtk_dprx_lt_event_message_enable(RTK_DPRX_ADP* p_dprx)
{
    if (p_dprx==NULL)
        return 0;

    switch(p_dprx->link_state)
    {
    case _DP_FAKE_TRAINING_PATTERN_1_RUNNING:
    case _DP_NORMAL_TRAINING_PATTERN_1_RUNNING:
        lt_status_idx = (p_dprx->link_sub_state==0) ? 0 : 1;
        break;
    case _DP_FAKE_TRAINING_PATTERN_1_PASS:
    case _DP_NORMAL_TRAINING_PATTERN_1_PASS:
    case _DP_FAKE_TRAINING_PATTERN_2_RUNNING:
    case _DP_NORMAL_TRAINING_PATTERN_2_RUNNING:
    case _DP_FAKE_TRAINING_PATTERN_2_PASS:
    case _DP_NORMAL_TRAINING_PATTERN_2_PASS:
        if (p_dprx->link_sub_state <= 1)
            lt_status_idx++;
        break;

    default:

        if (lt_status_idx)  // dump enqueued logs...
        {
            int i;

            for (i=0; i<lt_status_idx && (lt_status_idx<QS_EVENT_LOG_SIZE); i++)
            {
                DPRX_ADP_INFO("lt_state (%d-%02x-%d) lane=%d, bw=%02x (sink status : %02x-%02x-%02x)!!!\n",
                    g_qs_lt_status[i].link_round,
                    g_qs_lt_status[i].link_state,
                    g_qs_lt_status[i].link_sub_state,
                    g_qs_lt_status[i].link_lane_count,
                    g_qs_lt_status[i].link_bw,
                    g_qs_lt_status[i].sink_status[0],
                    g_qs_lt_status[i].sink_status[1],
                    g_qs_lt_status[i].sink_status[2]);
            }
            lt_status_idx = 0; // clear detailed log
        }
        return 1;
    }

    // add lt event to event list
    if (lt_status_idx < QS_EVENT_LOG_SIZE)
    {
        g_qs_lt_status[lt_status_idx].link_round      = p_dprx->link_round;
        g_qs_lt_status[lt_status_idx].link_state      = p_dprx->link_state;
        g_qs_lt_status[lt_status_idx].link_sub_state  = p_dprx->link_sub_state;
        g_qs_lt_status[lt_status_idx].link_lane_count = newbase_dprx_dpcd_get_lane_count(p_dprx->mac_idx);
        g_qs_lt_status[lt_status_idx].link_bw         = newbase_dprx_dpcd_get_link_bw_set(p_dprx->mac_idx);
        g_qs_lt_status[lt_status_idx].sink_status[0]  = newbase_dprx_dpcd_get_lane_01_status(p_dprx->mac_idx);
        g_qs_lt_status[lt_status_idx].sink_status[1]  = newbase_dprx_dpcd_get_lane_23_status(p_dprx->mac_idx);
        g_qs_lt_status[lt_status_idx].sink_status[2]  = newbase_dprx_dpcd_get_lane_align_status(p_dprx->mac_idx);
        g_qs_lt_status[lt_status_idx].time_stamp      = dprx_odal_get_system_time_ms();
    }

    return 0;
}

#else  // VERBOSE_QS_LT_LOG

/*------------------------------------------------------------------
 * Func : rtk_dprx_lt_event_message_enable
 *
 * Desc : link training debug message control
 *
 * Parm : p_dprx : handle of DPRX
 *
 * Retn : 0 : event message disable, 1 : event message enable
 *------------------------------------------------------------------*/
int rtk_dprx_lt_event_message_enable(RTK_DPRX_ADP* p_dprx)
{
    if (p_dprx==NULL)
        return 0;

    switch(p_dprx->link_state)
    {
    case _DP_FAKE_TRAINING_PATTERN_1_RUNNING:
    case _DP_NORMAL_TRAINING_PATTERN_1_RUNNING:
    case _DP_FAKE_TRAINING_PATTERN_1_PASS:
    case _DP_NORMAL_TRAINING_PATTERN_1_PASS:
    case _DP_FAKE_TRAINING_PATTERN_2_RUNNING:
    case _DP_NORMAL_TRAINING_PATTERN_2_RUNNING:
    case _DP_FAKE_TRAINING_PATTERN_2_PASS:
    case _DP_NORMAL_TRAINING_PATTERN_2_PASS:
        return 0;  // do not print message at lt1 state

    default:
        return 1;
    }
}

#endif // VERBOSE_QS_LT_LOG

#endif // ENABLE_DPRX_LT_EVENT_MONITOR

#ifdef CONFIG_DPRX_ENABLE_MULTIPORT
int rtk_dprx_channel_do_link_training(unsigned char ch)
{
    int ret = 0;
    int mac_idx = 0;
    dprx_dev_t* p_dev = _vfe_dprx_drv_get_dprx_adapter(ch);
    if(p_dev && p_dev->adp.p_private)
        mac_idx = ((RTK_DPRX_ADP*) p_dev->adp.p_private)->mac_idx;
    else
        return 0;

#ifdef DPRX_SUPPORT_AUX_MAC_INTERRUPT_HANDLER
    dprx_drv_mac_interrupt_handler_exint0();

    if (newbase_dprx_aux_get_irq_status_exint0(mac_idx))
    {
        dprx_drv_aux_interrupt_handler_exint0();
        dprx_drv_exit_interrupt_handler_exint0();
    }    
    
#else
    if (newbase_dprx_aux_get_irq_status_exint0(mac_idx))
    {
        dprx_drv_interrupt_handler_exint0();
        dprx_drv_exit_interrupt_handler_exint0();
    }
#endif

    if (newbase_dprx_aux_get_link_train_status(mac_idx)==_DP_FAKE_LINK_TRAINING_PASS ||
        newbase_dprx_aux_get_link_train_status(mac_idx)==_DP_NORMAL_LINK_TRAINING_PASS ||
        newbase_dprx_aux_get_link_train_status(mac_idx)==_DP_FAKE_LINK_TRAINING_PASS_VBIOS)
    {
        if(newbase_dprx_dpcd_get_device_service_cp_irq(mac_idx))
        {
            _aux_hpd_irq_assert_exint0(mac_idx);
        }

        return 1;
    }

    return 0;
}
#endif
/*------------------------------------------------------------------
 * Func : rtk_dprx_do_link_training
 *
 * Desc : link training handler for quick show
 *
 * Parm : N/A
 *
 * Retn : 0 : link train fail, 1 : link train pass
 *------------------------------------------------------------------*/
int rtk_dprx_do_link_training(void)
{
    int ret = 0;

#ifdef DPRX_SUPPORT_AUX_MAC_INTERRUPT_HANDLER
    dprx_drv_mac_interrupt_handler_exint0();

    if (newbase_dprx_aux_get_irq_status_exint0(0))
    {
        dprx_drv_aux_interrupt_handler_exint0();
        dprx_drv_exit_interrupt_handler_exint0();
    }
    
#else
    if (newbase_dprx_aux_get_irq_status_exint0(0))
    {
        dprx_drv_interrupt_handler_exint0();
        dprx_drv_exit_interrupt_handler_exint0();
    }
#endif

    if (newbase_dprx_aux_get_link_train_status(0)==_DP_FAKE_LINK_TRAINING_PASS ||
        newbase_dprx_aux_get_link_train_status(0)==_DP_NORMAL_LINK_TRAINING_PASS ||
        newbase_dprx_aux_get_link_train_status(0)==_DP_FAKE_LINK_TRAINING_PASS_VBIOS)
    {
        if(newbase_dprx_dpcd_get_device_service_cp_irq(0))
        {
            _aux_hpd_irq_assert_exint0(0);
        }

        return 1;
    }

    return 0;
}


/*------------------------------------------------------------------
 * Func : rtk_dprx_plat_init
 *
 * Desc : dprx platform init
 *
 * Parm : N/A
 *
 * Retn : 0
 *------------------------------------------------------------------*/
int rtk_dprx_plat_init(void)
{
    return 0; // do nothing
}


/*------------------------------------------------------------------
 * Func : rtk_dprx_plat_interrupt_enable
 *
 * Desc : enable dprx interrupt
 *
 * Parm : N/A
 *
 * Retn : 0
 *------------------------------------------------------------------*/
int rtk_dprx_plat_interrupt_enable(void)
{
    // quick show doesn't use HW interrupt, force disable HW interrupt
    #ifdef DPRX_EXT_SET_AUX_INT_ROUNTING
    rtk_dprx_set_aux_int_crtl_rounting_enable(0);
    #else
    rtd_outl(SYS_REG_INT_CTRL_SCPU_2_reg, SYS_REG_INT_CTRL_SCPU_2_dp_auxrx_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_2_write_data(0));
    #endif
    return 0;
}


/*------------------------------------------------------------------
 * Func : rtk_dprx_plat_interrupt_disable
 *
 * Desc : disable dprx interrupt
 *
 * Parm : N/A
 *
 * Retn : 0
 *------------------------------------------------------------------*/
int rtk_dprx_plat_interrupt_disable(void)
{
    #ifdef DPRX_EXT_SET_AUX_INT_ROUNTING
    rtk_dprx_set_aux_int_crtl_rounting_enable(0);
    #else
    rtd_outl(SYS_REG_INT_CTRL_SCPU_2_reg, SYS_REG_INT_CTRL_SCPU_2_dp_auxrx_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_2_write_data(0));
    #endif
    return 0;
}

