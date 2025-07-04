/*=============================================================
 * File:    dprx_hpd.c
 *
 * Desc:    DPRX hpd for RTK TV
 *
 * AUTHOR:  kevin_wang@realtek.com
 *
 * Vresion: 0.0.1
 *
 *------------------------------------------------------------
 * Copyright (c) Realtek Semiconductor Corporation, 2021
 *
 * All rights reserved.
 *
 *============================================================*/
#include <dprx_hpd-typec.h>
#include <rtk_kdriver/io.h>
#include <rbus/stb_reg.h>

#define  TYPE_C_DP_HPD_DUMMY_reg0                                                0xb80644c8
#define  TYPE_C_DP_HPD_DUMMY_reg1                                                0xb8064cc8
#define  TYPE_C_DP_HPD_DUMMY_flip_mask                                           0x00000001
#define  TYPE_C_DP_HPD_DUMMY_pn_swap_mask                                        0x00000006
#define  TYPE_C_DP_HPD_DUMMY_hpd_ack_req_mask                                    0x00000010
#define  TYPE_C_DP_HPD_DUMMY_hpd_status_mask                                     0x00000020
#define  TYPE_C_DP_HPD_DUMMY_irq_hpd_status_mask                                 0x00000040
#define  TYPE_C_DP_HPD_DUMMY_alt_mode_status_mask                                0x00000080
#define  TYPE_C_DP_HPD_DUMMY_flip(data)                                          (0x00000001&((data)))
#define  TYPE_C_DP_HPD_DUMMY_cable_type(data)                                    (0x00000006&((data)<<1))
#define  TYPE_C_DP_HPD_DUMMY_hpd_ack_req(data)                                   (0x00000010&((data)<<4))
#define  TYPE_C_DP_HPD_DUMMY_hpd_status(data)                                    (0x00000020&((data)<<5))
#define  TYPE_C_DP_HPD_DUMMY_irq_hpd_status(data)                                (0x00000040&((data)<<6))
#define  TYPE_C_DP_HPD_DUMMY_alt_mode_status(data)                               (0x00000080&((data)<<7))
#define  TYPE_C_DP_HPD_DUMMY_get_flip(data)                                      (0x00000001&((data)))
#define  TYPE_C_DP_HPD_DUMMY_get_cable_type(data)                                ((0x00000006&(data))>>1)
#define  TYPE_C_DP_HPD_DUMMY_get_hpd_ack_req(data)                               ((0x00000010&(data))>>4)
#define  TYPE_C_DP_HPD_DUMMY_get_hpd_status(data)                                ((0x00000020&(data))>>5)
#define  TYPE_C_DP_HPD_DUMMY_get_irq_hpd_status(data)                            ((0x00000040&(data))>>6)
#define  TYPE_C_DP_HPD_DUMMY_get_alt_mode_status(data)                           ((0x00000080&(data))>>7)

#define  TYPE_C_DP_HPD_DUMMY_2_reg0                                              0xb8064430
#define  TYPE_C_DP_HPD_DUMMY_2_reg1                                              0xb8064c30

#define GET_TYPEC_HPD_DEV(p_ctx)          ((p_ctx) ? ((typec_hpd_dev_t*) p_ctx->p_private) : NULL)


#ifdef CONFIG_ARCH_RTK2819A
#define  TYPE_C_DP_HPD_DUMMY_reg    ((p_dev->typec_id) ? TYPE_C_DP_HPD_DUMMY_reg1: TYPE_C_DP_HPD_DUMMY_reg0)
#define  TYPE_C_DP_HPD_DUMMY_2_reg  ((p_dev->typec_id) ? TYPE_C_DP_HPD_DUMMY_2_reg1: TYPE_C_DP_HPD_DUMMY_2_reg0)
#else
#define  TYPE_C_DP_HPD_DUMMY_reg    TYPE_C_DP_HPD_DUMMY_reg0
#define  TYPE_C_DP_HPD_DUMMY_2_reg    TYPE_C_DP_HPD_DUMMY_2_reg0
#endif


/*------------------------------------------------
 * Func : _ops_init
 *
 * Desc : init DPRX HPD device
 *
 * Para : p_adp : handle of DPRX HPD HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _typec_ops_init(dev_ctx_t* p_ctx)
{
    typec_hpd_dev_t* p_dev = GET_TYPEC_HPD_DEV(p_ctx);

    if (p_dev==NULL)
        return -1;

    p_dev->alt_mode_status = 0xF;
    p_dev->pre_hpd_flag = 0;
    // rtd_maskl(TYPE_C_DP_HPD_DUMMY_reg, ~TYPE_C_DP_HPD_DUMMY_hpd_status_mask, TYPE_C_DP_HPD_DUMMY_hpd_status(0));
    p_dev->hpd_low_timestamp_ms = dprx_odal_get_system_time_ms();  // save hpd low time
    return 0;
}

#ifdef CONFIG_DPRX_VFE_ENABLE_HPD_GUARD_TIME
/*------------------------------------------------
 * Func : _typec_ops_set_hpd_guard_time
 *
 * Desc : set hpd guard time
 *
 * Para : p_dev : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int _typec_ops_set_hpd_guard_time(dev_ctx_t* p_ctx, unsigned short delay_ms)
{
    typec_hpd_dev_t* p_dev = GET_TYPEC_HPD_DEV(p_ctx);

    if(p_dev == NULL)
        return -1;

    p_dev->hpd_low_guard_interval_ms = delay_ms;
    DPRX_HPD_INFO("[TYPEC]HPD new guard time = %d\n", p_dev->hpd_low_guard_interval_ms);

    return 0;
}

/*------------------------------------------------
 * Func : _typec_ops_get_hpd_guard_time
 *
 * Desc : get hpd guard time
 *
 * Para : p_dev : handle of DPRX adapter
 *
 * Retn : hpd_low_guard_interval_ms
 *-----------------------------------------------*/
static int _typec_ops_get_hpd_guard_time(dev_ctx_t* p_ctx)
{
    typec_hpd_dev_t* p_dev = GET_TYPEC_HPD_DEV(p_ctx);

    if(p_dev == NULL)
        return -1;

    return p_dev->hpd_low_guard_interval_ms;
}
#endif

/*------------------------------------------------
 * Func : _ops_uninit
 *
 * Desc : unint DPRX adapter. this function should
 *        be called when adapter to be destroyed
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _typec_ops_uninit(dev_ctx_t* p_ctx)
{
    typec_hpd_dev_t* p_dev = GET_TYPEC_HPD_DEV(p_ctx);

    if (p_dev==NULL)
        return -1;

    // rtd_maskl(TYPE_C_DP_HPD_DUMMY_reg, ~TYPE_C_DP_HPD_DUMMY_hpd_status_mask, TYPE_C_DP_HPD_DUMMY_hpd_status(0));
    return 0;
}


/*------------------------------------------------
 * Func : _ops_get_connect_status
 *
 * Desc : get cable connection status
 *
 * Para : p_dev : handle of DPRX adapter
 *
 * Retn : 0 : disconnect, 1 : connected
 *-----------------------------------------------*/
static int _typec_ops_get_connect_status(dev_ctx_t* p_ctx)
{
    typec_hpd_dev_t* p_dev = GET_TYPEC_HPD_DEV(p_ctx);

    if (p_dev==NULL)
        return -1;

    if ((rtd_inl(TYPE_C_DP_HPD_DUMMY_2_reg) & 0x0C))   // cable present
        return 1;

    rtd_maskl(TYPE_C_DP_HPD_DUMMY_reg, ~TYPE_C_DP_HPD_DUMMY_irq_hpd_status_mask, TYPE_C_DP_HPD_DUMMY_irq_hpd_status(0)); // clear hpd_irq when hpd low

    return 0;
}


/*------------------------------------------------
 * Func : _ops_get_aux_status
 *
 * Desc : get cable connection status
 *
 * Para : p_dev : handle of DPRX adapter
 *        p_aux_status : aux status output (optional)
 *
 * Retn : 0 : disconnect, 1 : connected
 *-----------------------------------------------*/
static int _typec_ops_get_aux_status(
    dev_ctx_t*      p_ctx,
    unsigned char*  p_aux_status
    )
{
    typec_hpd_dev_t* p_dev = GET_TYPEC_HPD_DEV(p_ctx);
    unsigned int typec_alt_mode;

    if (p_dev==NULL)
        return -1;

    typec_alt_mode = TYPE_C_DP_HPD_DUMMY_get_alt_mode_status(rtd_inl(TYPE_C_DP_HPD_DUMMY_reg));

    if (typec_alt_mode != p_dev->alt_mode_status)
    {
        if (typec_alt_mode)
        {
            DPRX_HPD_INFO("[TYPEC]Enter Alt mode\n");
            p_dev->hpd_low_timestamp_ms = dprx_odal_get_system_time_ms();
            DPRX_HPD_INFO("[TYPEC]INIT hpd_low_timestamp_ms\n");
        }
        else
            DPRX_HPD_INFO("[TYPEC]Exit Alt mode\n");

        p_dev->alt_mode_status = typec_alt_mode;
    }

    return p_dev->alt_mode_status;
}


/*------------------------------------------------
 * Func : _ops_set_hpd
 *
 * Desc : set/unset HPD signal of the DPRX adapter
 *
 * Para : p_dev : handle of DPRX adapter
 *        on    : 0 : HPD low, others : HPD high
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int _typec_ops_set_hpd(dev_ctx_t* p_ctx, unsigned char on)
{
    typec_hpd_dev_t* p_dev = GET_TYPEC_HPD_DEV(p_ctx);

    if (p_dev==NULL)
        return -1;

    if(_typec_ops_get_connect_status(p_ctx) == 0)
    {
        rtd_maskl(TYPE_C_DP_HPD_DUMMY_reg, ~TYPE_C_DP_HPD_DUMMY_hpd_status_mask, TYPE_C_DP_HPD_DUMMY_hpd_status(0));
        p_dev->pre_hpd_flag = 0;
        return 1;
    }

    if(TYPE_C_DP_HPD_DUMMY_get_hpd_status(rtd_inl(TYPE_C_DP_HPD_DUMMY_reg)) == on)
        return 1;

    if (on)
    {
        unsigned int guard_time = _get_hpd_low_guard_time(dprx_odal_get_system_time_ms(),
                                            p_dev->hpd_low_timestamp_ms,
                                            p_dev->hpd_low_guard_interval_ms);
        if (guard_time)
        {
            DPRX_HPD_INFO("[TYPEC]Set HPD on with in guard interval(%d ms), delay HPD high for %d ms\n",
                    p_dev->hpd_low_guard_interval_ms, guard_time);
            dprx_osal_msleep(guard_time);
        }
    }
    else
    {
        if(p_dev->pre_hpd_flag)     // update hdp low time stamp when hpd from high to low
        {
            p_dev->hpd_low_timestamp_ms = dprx_odal_get_system_time_ms();
            p_dev->pre_hpd_flag = 0;
        }
        rtd_maskl(TYPE_C_DP_HPD_DUMMY_reg, ~TYPE_C_DP_HPD_DUMMY_irq_hpd_status_mask, TYPE_C_DP_HPD_DUMMY_irq_hpd_status(0)); // clear hpd_irq when hpd low
    }

    rtd_maskl(TYPE_C_DP_HPD_DUMMY_reg, ~TYPE_C_DP_HPD_DUMMY_hpd_status_mask, TYPE_C_DP_HPD_DUMMY_hpd_status(on));
    rtd_maskl(TYPE_C_DP_HPD_DUMMY_reg, ~TYPE_C_DP_HPD_DUMMY_hpd_ack_req_mask, TYPE_C_DP_HPD_DUMMY_hpd_ack_req(1));
    DPRX_HPD_INFO("[TYPEC]Set HPD = %d\n", on);
    p_dev->pre_hpd_flag = on;
    return 1;
}
/*------------------------------------------------
 * Func : _typec_ops_set_hpd_toggle
 *
 * Desc : set HPD toggle signal of the DPRX adapter
 *
 * Para : p_adp : handle of DPRX adapter
 *        delay_ms  : the Interval of HPD toggle
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _typec_ops_set_hpd_toggle(dev_ctx_t* p_ctx, unsigned short delay_ms)
{
    _typec_ops_set_hpd(p_ctx, 0);
    dprx_osal_msleep(delay_ms);
    _typec_ops_set_hpd(p_ctx, 1);

    return 1;
}

/*------------------------------------------------
 * Func : _typec_ops_set_irq_hpd
 *
 * Desc : set HPD irq signal of the DPRX adapter
 *
 * Para : p_dev : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int _typec_ops_set_irq_hpd(dev_ctx_t* p_ctx)
{
    typec_hpd_dev_t* p_dev = GET_TYPEC_HPD_DEV(p_ctx);

    if (p_dev==NULL)
        return -1;

    if((rtd_inl(TYPE_C_DP_HPD_DUMMY_2_reg) & 0x0C) == 0)
        return 1;

    if(TYPE_C_DP_HPD_DUMMY_get_hpd_status(rtd_inl(TYPE_C_DP_HPD_DUMMY_reg)) == 0)
        return 1;

    DPRX_HPD_INFO("[TYPEC]HPD IRQ\n");
    rtd_maskl(TYPE_C_DP_HPD_DUMMY_reg, ~TYPE_C_DP_HPD_DUMMY_irq_hpd_status_mask, TYPE_C_DP_HPD_DUMMY_irq_hpd_status(1));
    rtd_maskl(TYPE_C_DP_HPD_DUMMY_reg, ~TYPE_C_DP_HPD_DUMMY_hpd_ack_req_mask, TYPE_C_DP_HPD_DUMMY_hpd_ack_req(1));
    return 1;
}

/*------------------------------------------------
 * Func : _ops_get_hpd
 *
 * Desc : get HPD status of the DPRX adapter
 *
 * Para : p_dev : handle of DPRX adapter
 *
 * Retn : 0 : HPD low, 1 : HPD high
 *-----------------------------------------------*/
static int _typec_ops_get_hpd(dev_ctx_t* p_ctx)
{
    typec_hpd_dev_t* p_dev = GET_TYPEC_HPD_DEV(p_ctx);

    if (p_dev==NULL)
        return 0;

    return TYPE_C_DP_HPD_DUMMY_get_hpd_status(rtd_inl(TYPE_C_DP_HPD_DUMMY_reg));
}

/*------------------------------------------------
 * Func : _ops_get_cable_config
 *
 * Desc : get cable_config of the DPRX adapter
 *
 * Para : p_dev : handle of DPRX adapter
 *
 * Retn : 1 : successed, 0 : failed
 *-----------------------------------------------*/
static int _typec_ops_get_cable_config(dev_ctx_t* p_ctx, DPRX_CABLE_CFG* p_cable_cfg)
{
    typec_hpd_dev_t* p_dev = GET_TYPEC_HPD_DEV(p_ctx);
    unsigned char cfg_id;
    unsigned char flip;

    if (p_dev==NULL)
    {
        *p_cable_cfg = 0;
        return 0;
    }

    if((rtd_inl(TYPE_C_DP_HPD_DUMMY_2_reg) & 0x0C) == 0)
    {
        *p_cable_cfg = 0;
        return 1;
    }

    if(TYPE_C_DP_HPD_DUMMY_get_alt_mode_status(rtd_inl(TYPE_C_DP_HPD_DUMMY_reg)) == 0)
        return 1;

    cfg_id = TYPE_C_DP_HPD_DUMMY_get_cable_type(rtd_inl(TYPE_C_DP_HPD_DUMMY_reg));
    flip = TYPE_C_DP_HPD_DUMMY_get_flip(rtd_inl(TYPE_C_DP_HPD_DUMMY_reg));

    switch(cfg_id)
    {
        case 0: cfg_id = 0xc; break;
        case 1: cfg_id = 0xd; break;
        case 2: cfg_id = 0xe; break;
        default :
            cfg_id = 0;
            break;
    }

    if(flip == 1)
        cfg_id = cfg_id + 0x80;

    *p_cable_cfg = cfg_id;
    return 1;
}

/*------------------------------------------------
 * Func : _typec_ops_set_typec_re_alt_mode
 *
 * Desc : re_alt mode
 *
 * Para : p_dev : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int _typec_ops_set_typec_re_alt_mode(dev_ctx_t* p_ctx)
{
    typec_hpd_dev_t* p_dev = GET_TYPEC_HPD_DEV(p_ctx);

    if (p_dev==NULL)
        return -1;

    rtd_maskl(TYPE_C_DP_HPD_DUMMY_reg, ~TYPE_C_DP_HPD_DUMMY_pn_swap_mask, TYPE_C_DP_HPD_DUMMY_cable_type(0xf));
    return 0;
}


/*------------------------------------------------
 * Func : _typec_ops_get_typec_pd_ready_status
 *
 * Desc : get typec pd ready status
 *
 * Para : p_dev : handle of DPRX adapter
 *
 * Retn : 0 : typec pd is ready, 1 : typec pd is busy
 *-----------------------------------------------*/
static int _typec_ops_get_typec_pd_ready_status(dev_ctx_t* p_ctx)
{
    typec_hpd_dev_t* p_dev = GET_TYPEC_HPD_DEV(p_ctx);

    if (p_dev==NULL)
        return -1;

    if((TYPE_C_DP_HPD_DUMMY_get_hpd_ack_req(rtd_inl(TYPE_C_DP_HPD_DUMMY_reg))) == 1)
        return 1;

    return 0;
}


/*------------------------------------------------
 * Func : create_rtk_tv_dprx_hpd_device
 *
 * Desc : alocate a dprx_edid device
 *
 * Para : N/A
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
hpd_dev_t* create_rtk_tv_typec_hpd_device(unsigned char idx)
{
    typec_hpd_dev_t* p_dev = NULL;

    if(STB_ST_CLKEN1_get_clken_typecpd(rtd_inl(STB_ST_CLKEN1_reg)) == 0)
    {
        DPRX_HPD_ERR("[TYPEC]create_rtk_tv_dprx_hpd_device failed, invalid configuration, please check your configiration\n");
        return NULL;
    }

    p_dev = (typec_hpd_dev_t*) dprx_osal_malloc(sizeof(typec_hpd_dev_t));

    if (p_dev)
    {
        memset(p_dev, 0, sizeof(typec_hpd_dev_t));

        p_dev->hpd_dev.ctx.name = "TYPEC HPD";
        p_dev->hpd_dev.ctx.p_private = (void*) p_dev;

        p_dev->hpd_dev.ops.init   = _typec_ops_init;
        p_dev->hpd_dev.ops.uninit = _typec_ops_uninit;
        p_dev->hpd_dev.ops.get_aux_status = _typec_ops_get_aux_status;
        p_dev->hpd_dev.ops.get_connect_status = _typec_ops_get_connect_status;
        p_dev->hpd_dev.ops.set_hpd = _typec_ops_set_hpd;
        p_dev->hpd_dev.ops.set_hpd_toggle = _typec_ops_set_hpd_toggle;
        p_dev->hpd_dev.ops.get_hpd = _typec_ops_get_hpd;
        p_dev->hpd_dev.ops.get_cable_config = _typec_ops_get_cable_config;
#ifdef CONFIG_DPRX_VFE_ENABLE_HPD_GUARD_TIME
        p_dev->hpd_dev.ops.set_hpd_guard_time = _typec_ops_set_hpd_guard_time;
        p_dev->hpd_dev.ops.get_hpd_guard_time = _typec_ops_get_hpd_guard_time;
#endif
        p_dev->hpd_dev.ops.set_typec_re_alt_mode = _typec_ops_set_typec_re_alt_mode;
        p_dev->hpd_dev.ops.get_typec_pd_ready_status = _typec_ops_get_typec_pd_ready_status;
        p_dev->hpd_dev.ops.set_irq_hpd = _typec_ops_set_irq_hpd;

        p_dev->typec_id = idx;
        p_dev->hpd_low_guard_interval_ms = DEFAULT_TYPEC_HPD_LOW_PERIOD_MS;
    }

    return &p_dev->hpd_dev;
}

