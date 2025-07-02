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

#define GET_TYPEC_HPD_DEV(p_ctx)          ((p_ctx) ? ((typec_hpd_dev_t*) p_ctx->p_private) : NULL)


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

    // do nothing
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
    // always report connected
    return 1;
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

    if (p_dev==NULL)
        return -1;
    // always report aux ready
    return 1;
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

    p_dev->pre_hpd_flag = on;
    return 0;
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
    // donothing
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

    return 0;
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

    return p_dev->pre_hpd_flag;
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

    if (p_dev==NULL)
    {
        *p_cable_cfg = 0;
        return 0;
    }

    cfg_id = 0xe;

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

    // do nothing
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

    // do nothing
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

