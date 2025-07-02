/*=============================================================
 * File:    dprx_adapter-rtk_dprx-priv.c
 *
 * Desc:    Internal driver for RTK DPRX adapter
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
#include <dprx_platform.h>
#include <dprx_adapter-rtk_dprx.h>
// #include <dprx_adapter-rtk_dprx-priv.h>
#include <dprx_adapter-rtk_dprx-priv_util.h>
#include <dprx_adapter-rtk_dprx-plat.h>
#ifdef CONFIG_DPRX_GET_IC_VERSION
#include <mach/platform.h>
#endif

#define GET_RTK_DPRX_PORT(p_adp)      ((p_adp && p_adp->p_private) ? (RTK_DPRX_ADP*) p_adp->p_private : NULL)

#ifdef CONFIG_RTK_KDRV_EMCU
    extern void emcu_set_dp_aux(unsigned int value);
#else
    #define emcu_set_dp_aux(value)
#endif

//================ EXT CONFIG Related ==================


#ifdef DPRX_EXT_WD_ENABLE
    #define DPRX_DRV_SET_VIDEO_WATCH_DOG(port, val)         dprx_drv_set_ext_config(port, DPRX_EXT_WD_ENABLE, val);
#else
    #define DPRX_DRV_SET_VIDEO_WATCH_DOG(port, val)
#endif

#ifdef DPRX_EXT_HDCP2x_HEADLER
    #define DPRX_DRV_HDCP2x_HEADLER(port, val)              dprx_drv_set_ext_config(port, DPRX_EXT_HDCP2x_HEADLER, val);
#else
    #define DPRX_DRV_HDCP2x_HEADLER(port, val)
#endif

#ifdef DPRX_EXT_HDCP2x_EVENT_HEADLER_ENABLE
    #define DPRX_DRV_HDCP2x_EVENT_HEADLER_ENABLE(port, val) dprx_drv_set_ext_config(port, DPRX_EXT_HDCP2x_EVENT_HEADLER_ENABLE, val);
#else
    #define DPRX_DRV_HDCP2x_EVENT_HEADLER_ENABLE(port, val)
#endif

#ifdef DPRX_EXT_SET_AUX_INT_ROUNTING
    #define DPRX_DRV_SET_AUX_INT_ROUNTING_ENABLE(port, val) dprx_drv_set_ext_config(port, DPRX_EXT_SET_AUX_INT_ROUNTING, val);
#else
    #define DPRX_DRV_SET_AUX_INT_ROUNTING_ENABLE(port, val)
#endif

#ifdef DPRX_EXT_POWER_CTRL_ON_WAKEUP_MODE
    #define DPRX_DRV_SER_POWER_CTRL_ON_WAKEUP_MODE(port, val) dprx_drv_set_ext_config(port, DPRX_EXT_POWER_CTRL_ON_WAKEUP_MODE, val);
#else
    #define DPRX_DRV_SER_POWER_CTRL_ON_WAKEUP_MODE(port, val)
#endif

#ifdef DPRX_EXT_HDCP1x_KEY_INIT
    #define DPRX_DRV_EXT_HDCP1x_KEY_INIT(port)              dprx_drv_set_ext_config(port, DPRX_EXT_HDCP1x_KEY_INIT, 0);
#else
    #define DPRX_DRV_EXT_HDCP1x_KEY_INIT(port)
#endif

#ifdef DPRX_EXT_HDCP2x_KEY_INIT
    #define DPRX_DRV_EXT_HDCP2x_KEY_INIT(port)              dprx_drv_set_ext_config(port, DPRX_EXT_HDCP2x_KEY_INIT, 0);
#else
    #define DPRX_DRV_EXT_HDCP2x_KEY_INIT(port)
#endif

#ifdef DPRX_EXT_AUX_Z0_ENABLE
    #define DPRX_DRV_EXT_AUX_Z0_ENABLE(port, val)           dprx_drv_set_ext_config(port, DPRX_EXT_AUX_Z0_ENABLE, val);
#else
    #define DPRX_DRV_EXT_AUX_Z0_ENABLE(port, val)
#endif

#ifdef DPRX_EXT_DP_IGNORE_DSC_ERR
    #define DPRX_DRV_EXT_IGNORE_DSC_ERR_CHECK(port, val)   dprx_drv_set_ext_config(port, DPRX_EXT_DP_IGNORE_DSC_ERR, val);
#else
    #define DPRX_DRV_EXT_IGNORE_DSC_ERR_CHECK(port, val)
#endif

//================ Definitions of function ==================

// for those legacy code which is still calling port_swap(NULL)+port_swap(p_dprx)
#define rtk_dprx_port_swap(p_dprx) \
  if (p_dprx != NULL) \
  { \
    rtk_dprx_port_reset(p_dprx); \
  }

static int  rtk_dprx_ddc_init(RTK_DPRX_ADP* p_dprx);
static int  rtk_dprx_ddc_uninit(RTK_DPRX_ADP* p_dprx);
//static int  rtk_dprx_port_swap(RTK_DPRX_ADP* p_dprx);
static int rtk_dprx_port_reset(RTK_DPRX_ADP* p_dprx);

static int  rtk_dprx_init(RTK_DPRX_ADP* p_dprx);
static int  rtk_dprx_uninit(RTK_DPRX_ADP* p_dprx);
static int  rtk_dprx_open(RTK_DPRX_ADP* p_dprx);
static int  rtk_dprx_close(RTK_DPRX_ADP* p_dprx);
static int  rtk_dprx_connect(RTK_DPRX_ADP* p_dprx);
static int  rtk_dprx_disconnect(RTK_DPRX_ADP* p_dprx);

static int  rtk_dprx_source_check(RTK_DPRX_ADP* p_dprx);
static int  rtk_dprx_get_capability(RTK_DPRX_ADP* p_dprx, DP_PORT_CAPABILITY* p_cap);
static int  rtk_dprx_get_link_status(RTK_DPRX_ADP* p_dprx, DP_LINK_STATUS_T* p_status);

static int  rtk_dprx_set_hpd(RTK_DPRX_ADP* p_dprx, UINT8 enable);
static int  rtk_dprx_set_hpd_toggle(RTK_DPRX_ADP* p_dprx, UINT16 delay_ms);
static int  rtk_dprx_set_irq_hpd(RTK_DPRX_ADP* p_dprx);
static int  rtk_dprx_get_hpd(RTK_DPRX_ADP* p_dprx);
static int  rtk_dprx_get_typec_pd_ready_status(RTK_DPRX_ADP* p_dprx);
static int  rtk_dprx_get_connect_status(RTK_DPRX_ADP* p_dprx);
static int  rtk_dprx_get_aux_status(RTK_DPRX_ADP* p_dprx);

static int  rtk_dprx_set_edid(RTK_DPRX_ADP* p_dprx, unsigned char* p_edid, unsigned short edid_len);
static int  rtk_dprx_get_edid(RTK_DPRX_ADP* p_dprx, unsigned char* p_edid, unsigned short edid_len);

static int  rtk_dprx_get_dpcd(RTK_DPRX_ADP* p_dprx, unsigned long address, unsigned char* p_dpcd, unsigned short len);
static int  rtk_dprx_set_dpcd(RTK_DPRX_ADP* p_dprx, unsigned long address, unsigned char* p_dpcd, unsigned short len);

static int  rtk_dprx_get_vbid_data(RTK_DPRX_ADP* p_dprx, DPRX_VBID_INFO_T* p_vbid);
static int  rtk_dprx_get_msa_data(RTK_DPRX_ADP* p_dprx, DPRX_MSA_INFO_T* p_msa);
static int  rtk_dprx_get_sdp_data(RTK_DPRX_ADP* p_dprx, DP_SDP_TYPE type, DP_SDP_DATA_T* p_data);
static int  rtk_dprx_get_pps_data(RTK_DPRX_ADP* p_dprx, DP_PPS_DATA_T* p_pps);

static int  rtk_dprx_get_stream_type(RTK_DPRX_ADP* p_dprx, DP_STREAM_TYPE_E* p_type);
static int  rtk_dprx_get_video_timing(RTK_DPRX_ADP* p_dprx, DPRX_TIMING_INFO_T* p_timing);
static int  rtk_dprx_get_stream_bypasss_timing(RTK_DPRX_ADP* p_dprx, DPRX_TIMING_INFO_T* p_timing);

static int  rtk_dprx_get_pixel_encoding_info(RTK_DPRX_ADP* p_dprx,DP_PIXEL_ENCODING_FORMAT_T* p_pixel);
static int  rtk_dprx_get_drm_info(RTK_DPRX_ADP* p_dprx, DPRX_DRM_INFO_T* p_drm);
static int  rtk_dprx_get_avi_info(RTK_DPRX_ADP* p_dprx, DPRX_AVI_INFO_T* p_avi);

static int  rtk_dprx_get_audio_status(RTK_DPRX_ADP* p_dprx, DP_AUDIO_STATUS_T* p_audio_status);
static int  rtk_dprx_get_hdcp_status(RTK_DPRX_ADP* p_dprx, DPRX_HDCP_STATUS_T* p_hdcp_status);

static int  rtk_dprx_suspend(RTK_DPRX_ADP* p_dprx, UINT32 mode);
static int  rtk_dprx_resume(RTK_DPRX_ADP* p_dprx);

static int  rtk_dprx_reset_signal_detect_flow(RTK_DPRX_ADP* p_dprx);
static int  rtk_dprx_handle_online_measure_error(RTK_DPRX_ADP* p_dprx);
static void rtk_dprx_handle_infoframe_sdp_update_event(RTK_DPRX_ADP* p_dprx, SDP_TYPE_E sdp_type);

#ifdef ENABLE_DPRX_LT_EVENT_MONITOR
static void rtk_dprx_handle_lt_state_update_event(RTK_DPRX_ADP* p_dprx, UINT8 state);
#else
#define rtk_dprx_handle_lt_state_update_event(p_dprx, state)
#endif

#ifdef CONFIG_DPRX_ENABLE_TASK_EVENT_HANDLER
static void rtk_dprx_port_event_task_handler(RTK_DPRX_ADP* p_dprx);
#else
#define rtk_dprx_port_event_task_handler(p_dprx)
#define rtk_dprx_event_task_handler()
#endif

static void rtk_dprx_port_set_aux_int_crtl_rounting_enable(RTK_DPRX_ADP* p_dprx, UINT8 enable);
#define rtk_dprx_set_aux_int_crtl_rounting_enable(enable) rtk_dprx_port_set_aux_int_crtl_rounting_enable(NULL, enable)
static int  rtk_dprx_set_ext_ctrl(RTK_DPRX_ADP* p_dprx, ADP_EXCTL_ID ctrl, unsigned int val);
static int  rtk_dprx_get_ext_ctrl(RTK_DPRX_ADP* p_dprx, ADP_EXCTL_ID ctrl);

#ifdef CONFIG_RTK_DPRX_SYSFS
extern int  rtk_dprx_attr_show(RTK_DPRX_ADP* p_dprx, ADP_ATTR attr, UINT8* p_buf, UINT32 cnt);
extern int  rtk_dprx_attr_store(RTK_DPRX_ADP* p_dprx, ADP_ATTR attr, UINT8* p_buf, UINT32 cnt);
#else
#define rtk_dprx_attr_show(p_dprx, attr, p_buf, cnt)         (0)
#define rtk_dprx_attr_store(p_dprx, attr, p_buf, cnt)        (cnt)
#endif

#ifdef CONFIG_SUPPORT_DOLBY_VISION_DP
static int rtk_dprx_set_dolby_vision_check_mode_function(long (*get_dolby_vision_dp_mode)(unsigned char));
#endif




//================ Definitions of TASK HANDLER ==================
#ifdef CONFIG_DPRX_ENABLE_TASK_EVENT_HANDLER
static UINT8 g_resume_system_time_ms = 0;
extern UINT16 g_resume_guard_time_interval;

#define DEFAULT_DP_RECONNECT_TIME_INTERVAL_PERIOD_MS       5000
#define DEFAULT_TYPEC_RECONNECT_TIME_INTERVAL_PERIOD_MS    8000

#endif

//================ Definitions of variable ==================


extern RTK_DPRX_ADP* g_p_current_dprx_port;
extern DPRX_SDP_HANDLER g_dprx_sdp_handler;
extern UINT32 g_dprx_sdp_debug_ctrl;
extern edid_dev_t*   g_p_dprx_edid;

#define DEFAULT_SOURCE_CHECK_NO_LT_INTERVAL_PERIOD_MS       4000   //To recover and inform source device to retrain.

#ifndef CONFIG_DPRX_VFE_ENABLE_HPD_LOW_GUARD_INTERVAL
#define DPRX_PORT_SWAP_DP_HPD_TOGGLE_TIME       900  // MCE1 Slim Dock takes more than 800 ms to detect long hpd toggle
#define DPRX_PORT_SWAP_TYPEC_HPD_TOGGLE_TIME   1300  // Lindy HDMI to typeC converter takes more than 1200 mse to detect hpd
#endif

#ifdef CONFIG_DPRX_EANBLE_FREESYNC
extern UINT8 g_force_freesync_off;
#endif

//legacy code. shall remove in following patch.
extern UINT8 g_force_demode;
extern UINT8 g_force_max_lane_count;  // disabled
extern UINT8 g_force_max_link_rate; // disabled
extern UINT8 g_force_fake_tps1_lt;  // disabled
extern UINT8 g_force_fake_tps2_lt;  // disabled
extern UINT8 g_force_timing_ext_flag_en;  // for test
extern UINT8 g_force_timing_ext_flag;     // for test
extern UINT8 g_skip_source_detect;  // disable detect flow to for debug
extern UINT8 g_mute_if_lane_dealign;

extern const char* sdp_type_str(SDP_TYPE_E sdp_type);

#ifdef CONFIG_SUPPORT_DOLBY_VISION_DP
extern long (*fp_get_dolby_vision_dp_mode)(unsigned char);
#endif

//#define FORCE_DEFAULT_EDID
#ifdef FORCE_DEFAULT_EDID
// ARG Free Sync EDID
extern UINT8 default_edid[];
#endif

#ifdef VFSM_0_WAIT_LINK_READY
extern const char* _get_dprx_vfsm_str(UINT8 vfsm);
#endif


#ifdef CONFIG_ENABLE_TYPEC_DPRX
extern DPRX_PORT_CONFIG g_dprx_typec_cable_cfg[];
#endif

extern void _set_dprx_ic_version(UINT8 port);

extern const char* _get_dprx_hdcp_state_str(DP_HDCP_MODE mode);
extern UINT8 _get_source_check_intenal(
    unsigned long curr_time,
    unsigned long pre_source_check_time_stamp,
    unsigned int  guard_time
    );
extern UINT8 _get_max_lane_count(RTK_DPRX_ADP* p_dprx);

extern void _set_dprx_ic_version(UINT8 port);
/*------------------------------------------------
 * Func : rtk_dprx_ddc_init
 *
 * Desc : init ddc of rtk dprx
 *
 * Para : p_dprx : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_ddc_init(RTK_DPRX_ADP* p_dprx)
{

    int flags = 0;
    flags |= RTK_DDC_FLAG_AUX_ID(p_dprx->port_cfg.aux_sel);
    DPRX_ADP_INFO("rtk_dprx_ddc_init (%d) (reference count=%d)\n", p_dprx->ddc_idx, 0);

    //@ Need Change DPRX_AUX_DDC1 DPRX_AUX_DDC2.
    if (p_dprx->p_edid==NULL)
    {
        p_dprx->ddc_idx +=1 ;
        // create EDID data
#ifdef CONFIG_DPRX_DRV_ENABLE_VIRTUAL_PLATFORM
        p_dprx->p_edid = alloc_dprx_edid_device();
#else
#ifdef CONFIG_DPRX_VFE_ENABLE_QUICK_SHOW
        if(dprx_platform_get_quick_show_enable())
            flags |= RTK_DDC_FLAG_ENABLE_QS_INIT(1);
        p_dprx->p_edid = create_rtk_ddc_edid_device_ex(p_dprx->ddc_idx,flags);
#else
        p_dprx->p_edid = create_rtk_ddc_edid_device(p_dprx->ddc_idx);
#endif
#endif

        if (p_dprx->p_edid==NULL)
        {
            DPRX_ADP_WARN("rtk_dprx_ddc_init failed, create dprx ddc (%d) failed\n", DPRX_AUX_DDC);
            return -1;
        }

        if (dprx_edid_init(p_dprx->p_edid) <0)
        {
            DPRX_ADP_INFO("rtk_dprx_ddc_init failed, init dprx ddc (%d) failed\n", DPRX_AUX_DDC);
            destroy_dprx_edid_device(p_dprx->p_edid);
            p_dprx->p_edid = NULL;
            return -1;
        }

        if (dprx_platform_get_quick_show_enable())
        {
            return 0;
        }
        dprx_edid_enable(p_dprx->p_edid, 0);
        dprx_edid_reset(p_dprx->p_edid);
    }

    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_ddc_uninit
 *
 * Desc : uninit ddc
 *
 * Para : p_dprx : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_ddc_uninit(RTK_DPRX_ADP* p_dprx)
{
    DPRX_ADP_INFO("rtk_dprx_ddc_uninit (%d) (reference count=%d)\n", DPRX_AUX_DDC, 0);

    if (p_dprx->p_edid==NULL)
        return 0;  // already released, do nothing

    DPRX_ADP_INFO("rtk_dprx_ddc_uninit (%d) success (reference count=%d)\n", DPRX_AUX_DDC, 0);

    DPRX_ADP_INFO("destroy release ddc device\n");
    dprx_edid_enable(p_dprx->p_edid, 0);          // disable EDID
    destroy_dprx_edid_device(p_dprx->p_edid);     // free edid device
    p_dprx->p_edid = NULL;

    return 0;
}

#ifdef CONFIG_DPRX_VFE_ENABLE_QUICK_SHOW

/*------------------------------------------------
 * Func : rtk_dprx_port_qs_init
 *
 * Desc : do QS init. When QS enabled, HW has been
 *        Initialized in Kboot, here we have to
 *        resync FW status from HW
 *
 * Para : p_dprx : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_port_qs_init(
    RTK_DPRX_ADP*   p_dprx
    )
{
    if (p_dprx == NULL)
        return 0;  // invalid handle, do nothing

    if (p_dprx->qs_init_ready==0 || p_dprx->qs_connect_ready==0)
    {
        DPRX_ADP_INFO("rtk_dprx_port_qs_init : not current port, skip it\n");
        return 0;  // invalid handle, do nothing
    }

    DPRX_ADP_INFO("rtk_dprx_port_qs_init : current port, do qs init\n");

    // 1. setup low level driver port configuration
// #ifdef CONFIG_ENABLE_TYPEC_DPRX
//     memcpy(&g_dprx_port_config, &p_dprx->port_cfg, sizeof(DPRX_PORT_CONFIG));
// #endif

//     // 2. setup low level driver sink capability
//     memcpy(&g_dprx_sink_capability, &p_dprx->sink_cap, sizeof(DPRX_SINK_CAPABILITY));

    // 3. setup FW EDID state
    if (p_dprx->qs_edid_ready==0)
    {
        dprx_edid_get_edid(p_dprx->p_edid, p_dprx->sink_edid, sizeof(p_dprx->sink_edid));  // read edid to DDC sram to get EDID
        p_dprx->qs_edid_ready = 1;
    }

    dprx_edid_enable(p_dprx->p_edid, 1); // force enable edid access

    // 4. setup FW hpd state
    p_dprx->hpd_enable = 1;  // force hpd enable

    // 5. Set IC version
    _set_dprx_ic_version(p_dprx->mac_idx);

    // 6. record guard time and inital vfsm_status.
    p_dprx->pre_source_check_no_signal_time = dprx_odal_get_system_time_ms();
    p_dprx->vfsm_status = 0xFF;

    // 7. Init HDCP event queue
    DPRX_DRV_HDCP2x_EVENT_HEADLER_ENABLE(p_dprx->mac_idx, 1);
    p_dprx->hdcp2x_event_rp = 0;
    p_dprx->hdcp2x_event_wp = 0;

    // 8. sink_capabliity_update clear
    p_dprx->sink_capabliity_update = 0;

    p_dprx->dprx_port_readly = 1;

    p_dprx->current_port = 1;

    // 9. do driver qs init
    return dprx_drv_qs_init_dprx_port(p_dprx->mac_idx,  &p_dprx->sink_cap, &p_dprx->port_cfg);
}

#endif


#ifdef CONFIG_ENABLE_TYPEC_DPRX
/*------------------------------------------------
 * Func : _typec_hpd_check_status
 *
 * Desc : Update typeC hpd setting
 *
 * Para : p_dprx : handle of DPRX adapter
 *
 * Retn : non
 *-----------------------------------------------*/
static UINT8 _typec_hpd_check_status(
    RTK_DPRX_ADP*           p_dprx
    )
{
    if (p_dprx == NULL ||
        p_dprx->cable_type == 0 ||
        p_dprx->type != DP_TYPE_USB_TYPE_C ||
        p_dprx->p_hpd==NULL ||
        dprx_hpd_get_connect_status(p_dprx->p_hpd)==0)
        return 1;

    return 0;
}

/*------------------------------------------------
 * Func : _update_typec_hpd_setting
 *
 * Desc : Update typeC hpd setting
 *
 * Para : p_dprx : handle of DPRX adapter
 *
 * Retn : non
 *-----------------------------------------------*/
static void _update_typec_hpd_setting(
    RTK_DPRX_ADP*           p_dprx
    )
{
    unsigned char hpd_enable;

    // check is typeC
    if (_typec_hpd_check_status(p_dprx))
        return ;

    // check hpd value
    hpd_enable = p_dprx->hpd_enable;
    if (hpd_enable)
    {
        if ( !p_dprx->current_port ||
            dprx_hpd_get_aux_status(p_dprx->p_hpd , NULL)==0)
            hpd_enable = 0;   // force hpd enable = 0
    }

    // apply setting
    if (dprx_hpd_get_hpd(p_dprx->p_hpd) != hpd_enable)
    {
        DPRX_ADP_INFO("[TYPEC]Set current hpd(%d) to hpd_enable(%d)\n", dprx_hpd_get_hpd(p_dprx->p_hpd), hpd_enable);
        dprx_hpd_set_hpd(p_dprx->p_hpd, hpd_enable);
    }
}
#else
    #define _update_typec_hpd_setting(p_dprx)       dprx_hpd_set_hpd(p_dprx->p_hpd, p_dprx->hpd_enable);
#endif


/*------------------------------------------------
 * Func : rtk_dprx_port_reset
 *
 * Desc :   Replace port_swap(p_dprx) & port_swap(NULL)+port_swap(p_dprx);
 *          After port_reset, p_dprx will become current port.
 *          and start low level driver.
 *          hpd will set to low or do toggle.
 *
 * Para : p_dprx : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_port_reset(
    RTK_DPRX_ADP*   p_dprx
    )
{
    // B. start new current port
    if (p_dprx)
    {
        DPRX_ADP_INFO("rtk_dprx_port_swap : start new DPRX port\n");

        // b.1 setup new dprx
        if(p_dprx->debug_ctrl.force_max_link_rate && p_dprx->sink_cap.max_link_rate != p_dprx->debug_ctrl.force_max_link_rate)
            p_dprx->sink_cap.max_link_rate = p_dprx->debug_ctrl.force_max_link_rate;

        p_dprx->sink_cap.max_lane_count = _get_max_lane_count(p_dprx);

        if(p_dprx->debug_ctrl.force_max_lane_count && p_dprx->sink_cap.max_lane_count != p_dprx->debug_ctrl.force_max_lane_count)
            p_dprx->sink_cap.max_lane_count = p_dprx->debug_ctrl.force_max_lane_count;

#ifdef DPRX_SINK_CAP_LINK_TRAIN_CTRL_FAKE_TP1_LT
        if(p_dprx->debug_ctrl.force_fake_tps1_lt)
            p_dprx->sink_cap.link_train_ctrl |= DPRX_SINK_CAP_LINK_TRAIN_CTRL_FAKE_TP1_LT;
        else
            p_dprx->sink_cap.link_train_ctrl = 0;

#endif
#ifdef DPRX_SINK_CAP_LINK_TRAIN_CTRL_FAKE_TP2_LT
        if(p_dprx->debug_ctrl.force_fake_tps2_lt)
            p_dprx->sink_cap.link_train_ctrl |= DPRX_SINK_CAP_LINK_TRAIN_CTRL_FAKE_TP2_LT;
        else
            p_dprx->sink_cap.link_train_ctrl = 0;
#endif

        DPRX_ADP_INFO("set dprx port (max_lane_count=%d, max_link_rate=%02x)\n",
            p_dprx->sink_cap.max_lane_count,
            p_dprx->sink_cap.max_link_rate);

#ifdef CONFIG_DPRX_EANBLE_FREESYNC
        if (p_dprx->debug_ctrl.force_freesync_off && p_dprx->sink_cap.freesync_support)
        {
            p_dprx->sink_cap.freesync_support = 0;
        }

#endif

        dprx_drv_init_dprx_port(p_dprx->mac_idx, &p_dprx->sink_cap, &p_dprx->port_cfg);

        DPRX_DRV_EXT_IGNORE_DSC_ERR_CHECK(p_dprx->mac_idx, p_dprx->ignore_dsc_check_en);

        switch(p_dprx->type)
        {
            case DP_TYPE_USB_TYPE_C:
                DPRX_DRV_EXT_AUX_Z0_ENABLE(p_dprx->mac_idx, (p_dprx->cable_type)? 1:0);           //enable aux z0 if usbc enter alt mode
                break;
            default :
                DPRX_DRV_EXT_AUX_Z0_ENABLE(p_dprx->mac_idx, 1);                                   //force enable aux z0 on DP port
        }

        // b.2 update edid
        dprx_edid_set_edid(p_dprx->p_edid, p_dprx->sink_edid, sizeof(p_dprx->sink_edid));  // write edid to sram

        // 3. ddc/edid on
        dprx_edid_enable(p_dprx->p_edid, 1);

        // 4. setup hpd
        if (p_dprx->p_hpd)
        {
#ifdef CONFIG_ENABLE_TYPEC_DPRX
            if (p_dprx->hpd_enable)
            {
                dprx_hpd_set_hpd(p_dprx->p_hpd, 0); // hpd should be reset 0 no matter the hpd value
#ifdef CONFIG_DPRX_VFE_ENABLE_HPD_LOW_GUARD_INTERVAL
                // no delay, let HPD engine to guarantee the guard interval
#else
                switch(p_dprx->type)
                {
                    case DP_TYPE_USB_TYPE_C:
                        DPRX_ADP_INFO("rtk_dprx_port_swaped, do hpd toggle (%dms)\n", DPRX_PORT_SWAP_TYPEC_HPD_TOGGLE_TIME);
                        dprx_osal_msleep(DPRX_PORT_SWAP_TYPEC_HPD_TOGGLE_TIME);
                        break;
                    default :
                        DPRX_ADP_INFO("rtk_dprx_port_swaped, do hpd toggle (%dms)\n", DPRX_PORT_SWAP_DP_HPD_TOGGLE_TIME);
                        dprx_osal_msleep(DPRX_PORT_SWAP_DP_HPD_TOGGLE_TIME);
                }
#endif
            }
#endif

#ifdef CONFIG_DPRX_VFE_ENABLE_HPD_LOW_GUARD_INTERVAL
            switch(p_dprx->type)
            {
            case DP_TYPE_USB_TYPE_C:
                if(dprx_hpd_get_aux_status(p_dprx->p_hpd , NULL) && p_dprx->cable_type != 0)
                    dprx_hpd_set_hpd(p_dprx->p_hpd, p_dprx->hpd_enable);
                break;
            default :
                dprx_hpd_set_hpd(p_dprx->p_hpd, p_dprx->hpd_enable);
            }
#else
            dprx_hpd_set_hpd(p_dprx->p_hpd, p_dprx->hpd_enable);
#endif
        }

        // 5. clear sdp info
        memset(&p_dprx->sdp_handler, 0, sizeof(p_dprx->sdp_handler));

#ifdef DPRX_SUPPORT_SET_LOW_POWER_MODE
        // 6. set lpm state
        if (p_dprx->p_hpd ==NULL || dprx_hpd_get_connect_status(p_dprx->p_hpd)==0)
        {
            DPRX_ADP_INFO("rtk_dprx_port_swaped without cable, enter low power mode\n");
            dprx_drv_set_low_power_mode_enable(p_dprx->mac_idx, 1);
            p_dprx->lpm_enable = 1;
        }
        else if (p_dprx->lpm_enable)
        {
            DPRX_ADP_INFO("rtk_dprx_port_swaped with cable under low power mode, exit low power mode\n");
            p_dprx->lpm_enable = 0;
        }
        else
        {
            DPRX_ADP_INFO("rtk_dprx_port_swaped with cable under normal mode\n");
        }
#endif

        // 6. Set IC version
        _set_dprx_ic_version(p_dprx->mac_idx);

        // 7. record guard time and inital vfsm_status.
        p_dprx->pre_source_check_no_signal_time = dprx_odal_get_system_time_ms();
        p_dprx->vfsm_status = 0xFF;

        // 8. Init HDCP event queue
        DPRX_DRV_HDCP2x_EVENT_HEADLER_ENABLE(p_dprx->mac_idx, 1);
        p_dprx->hdcp2x_event_rp = 0;
        p_dprx->hdcp2x_event_wp = 0;

        // 9. sink_capabliity_update clear
        p_dprx->sink_capabliity_update = 0;

        p_dprx->dprx_port_readly = 1;

        p_dprx->current_port = 1;
        p_dprx->resume_connect = 0;
    }



    return 0;

}

// @
/*------------------------------------------------
 * Func : rtk_dprx_port_stop
 *
 * Desc :   replace port_swap(NULL). exit current
 *          port, enable lpm mode, force hpd low.
 *
 * Para : p_dprx : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_port_stop(
    RTK_DPRX_ADP*   p_dprx
)
{
    if (p_dprx)
    {
        DPRX_ADP_INFO("rtk_dprx_port_swap : stop current port\n");

        p_dprx->dprx_port_readly = 0;
        p_dprx->current_port = 0;
        p_dprx->connected = 0;   // disconnect current port

        // 1. stop dprx port output

        // 2. current port hpd off
        if (p_dprx->p_hpd)
            dprx_hpd_set_hpd(p_dprx->p_hpd, 0);   // force set current port hpd to low

        // 3. ddc/edid off
        dprx_edid_enable(p_dprx->p_edid, 0);


        // 5. clear sdp info
        memset(&p_dprx->sdp_handler, 0, sizeof(p_dprx->sdp_handler));

#ifdef DPRX_SUPPORT_SET_LOW_POWER_MODE
        // 6. set lpm state
        DPRX_ADP_INFO("rtk_dprx_port_swap : set current port to low power mode\n");
        dprx_drv_set_low_power_mode_enable(p_dprx->mac_idx, 1);
        p_dprx->lpm_enable = 1;
#endif
        p_dprx->link_round = 0;
        p_dprx->link_state = 0;
        p_dprx->link_sub_state = 0;
    }


    return 0;
}
#ifdef CONFIG_ENABLE_TYPEC_DPRX
extern int rtk_dprx_set_typec_port_config(RTK_DPRX_ADP* p_dprx, DPRX_CABLE_CFG* p_type_c_dp_cfg);
#endif


/*------------------------------------------------
 * Func : rtk_dprx_init
 *
 * Desc : init DPRX adapter
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_init(
    RTK_DPRX_ADP*   p_dprx
    )
{
    DPRX_ADP_INFO("rtk_dprx_init (port=%d)\n", p_dprx->mac_idx);

    if (p_dprx==NULL)
    {
        DPRX_ADP_WARN("init rtk dprx port failed, p_dprx==NULL!!!! \n");
        return -1;
    }

    // init global ddc
    if (rtk_dprx_ddc_init(p_dprx)<0)
    {
        DPRX_ADP_WARN("init rtk dprx port failed, init global ddc failed!!!! \n");
        return -1;
    }

    dprx_video_common_init(p_dprx->p_vc);  // init dprx video common

    rtk_dprx_plat_init();  // do platform initialization

#ifdef CONFIG_DPRX_VFE_ENABLE_QUICK_SHOW
    if (p_dprx->qs_init_ready)
    {
        DPRX_ADP_INFO("init rtk dprx port in QS mode (conneced=%d)\n", p_dprx->qs_connect_ready);

        // reset parameter
        p_dprx->connected = p_dprx->qs_connect_ready;
        p_dprx->hpd_enable = 1;  // QS is alway hpd enable

        // init port
        if (p_dprx->connected)
        {
            // QS port. dp QS init
            if (rtk_dprx_port_qs_init(p_dprx)<0)
            {
#ifdef CONFIG_ENABLE_TYPEC_DPRX
#ifdef FORCE_RE_ALT_MODE
                if (p_dprx->p_hpd && p_dprx->type == DP_TYPE_USB_TYPE_C)
                {
                    DPRX_ADP_INFO("TypeC : force do re-alt\n");
                    dprx_hpd_set_typec_re_alt_mode(p_dprx->p_hpd);
                }
#endif
#endif

                rtk_dprx_port_reset(p_dprx);   // QS init fail, do port swap to instead
            }
            else
            {
                // QS init pass
                dprx_hpd_init(p_dprx->p_hpd);

#ifdef CONFIG_ENABLE_TYPEC_DPRX
                if (p_dprx->type == DP_TYPE_USB_TYPE_C)
                {
                    dprx_hpd_get_cable_config(p_dprx->p_hpd, &p_dprx->cable_type);   // read back current cable status
                    rtk_dprx_set_typec_port_config(p_dprx, &p_dprx->cable_type);
                }
#endif
                p_dprx->sink_capabliity_update=1;
            }

            p_dprx->current_port = 1;   // set DP port to current port
        }
        else
        {
            dprx_hpd_init(p_dprx->p_hpd);  // QS port. dp QS init
        }
        p_dprx->qs_init_ready = 0; // clear qs init ready flag
        p_dprx->qs_open_ready = 1; // force open ready
    }
    else
#endif
    {
        DPRX_ADP_INFO("init rtk dprx port %d in normal mode\n", p_dprx->mac_idx);

        // reset parameter
        p_dprx->connected = 0;
        p_dprx->hpd_enable = 0;

        // init port
        if(p_dprx->p_hpd)
        {
            dprx_hpd_init(p_dprx->p_hpd);
#ifdef CONFIG_ENABLE_TYPEC_DPRX
#ifdef FORCE_RE_ALT_MODE
            if (p_dprx->type == DP_TYPE_USB_TYPE_C)
            {
                DPRX_ADP_INFO("TypeC : force do re-alt\n");
                dprx_hpd_set_typec_re_alt_mode(p_dprx->p_hpd);
            }
#endif
#endif
        }
        rtk_dprx_port_reset(p_dprx);
    }
    rtk_dprx_plat_interrupt_enable();

    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_open
 *
 * Desc : open DPRX adapter. after open, the DPRX
 *        adapter will become operational. caller
 *        should call open before any other operations
 *        excepts init/uninit
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_open(
    RTK_DPRX_ADP*   p_dprx
    )
{
    if (p_dprx==NULL)
    {
        DPRX_ADP_WARN("open rtk dprx port failed, p_dprx==NULL!!!! \n");
        return -1;
    }

    // set DPRX port to current port (if no current port present)
    if(!p_dprx->current_port)
    {
        rtk_dprx_port_reset(p_dprx);
    }

    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_close
 *
 * Desc : close DPRX adapter. after close, the DPRX
 *        adapter will stop operation.
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_close(
    RTK_DPRX_ADP*   p_dprx
    )
{
    if (p_dprx==NULL)
    {
        DPRX_ADP_WARN("close rtk dprx port failed, p_dprx==NULL!!!! \n");
        return -1;
    }

    rtk_dprx_port_stop(p_dprx);  // set current port to null. it will stop current port immedately
    p_dprx->connected = 0;
    p_dprx->hpd_enable = 0;    // reset hpd status

    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_uninit
 *
 * Desc : unint DPRX adapter. this function should
 *        be called when adapter to be destroyed
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_uninit(
    RTK_DPRX_ADP*   p_dprx
    )
{
    if (p_dprx==NULL)
    {
        DPRX_ADP_WARN("uninit rtk dprx port failed, p_dprx==NULL!!!! \n");
        return -1;
    }

    rtk_dprx_plat_interrupt_disable();

    rtk_dprx_close(p_dprx);    // force clese dprx port

    if (p_dprx->p_hpd)
        destroy_dprx_hpd_device(p_dprx->p_hpd);   // destroy HPD

    if(p_dprx->p_vc)
        destroy_dprx_video_common_device(p_dprx->p_vc);
    rtk_dprx_ddc_uninit(p_dprx);     // release ddc
    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_connect
 *
 * Desc : connect DPRX adapter. after connect, the
 *        DPRX adapter will begin audio/video format
 *        detection and able to output audio/video
 *        after detect.
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_connect(
    RTK_DPRX_ADP*   p_dprx
    )
{
    if (p_dprx==NULL)
    {
        DPRX_ADP_WARN("connect rtk dprx port failed, p_dprx==NULL!!!! \n");
        return -1;
    }

    if (!p_dprx->current_port)
    {
        rtk_dprx_port_reset(p_dprx);     // change to new port
    }

    p_dprx->connected = 1;          // setup connect flag

    // check input source
    if (dprx_drv_source_check(p_dprx->mac_idx)==_TRUE)
        return 0;

    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_disconnect
 *
 * Desc : disable DPRX adapter. after detect, the
 *        DPRX adapter detect/output audio/video data
 *
 * Para : p_dprx : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_disconnect(
    RTK_DPRX_ADP*   p_dprx
    )
{
    if (p_dprx==NULL)
    {
        DPRX_ADP_WARN("disconnect rtk dprx port failed, p_dprx==NULL!!!! \n");
        return -1;
    }

    // stop dprx A/V output (current port only)
    if (p_dprx->current_port)
    {
        DPRX_ADP_INFO("rtk_dprx_disconnect : stop av output\n");
        // setup av mute state
    }

    // clear connect flag
    p_dprx->connected = 0;

    rtk_dprx_set_ext_ctrl(p_dprx, ADP_EXCTL_WATCH_DOG_ENABLE, 0); //Disable DPRX WatchDog
    DPRX_ADP_INFO("disconnect rtk dprx port successed \n");
    return 0;
}


#ifdef CONFIG_ENABLE_TYPEC_DPRX

/*------------------------------------------------
 * Func : _check_typec_config
 *
 * Desc : check typeC config status.
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : type c config not changed
 *        1 : type c config changed
 *-----------------------------------------------*/
static int _check_typec_config(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret = 0;
    if (p_dprx && p_dprx->type == DP_TYPE_USB_TYPE_C)
    {
        DPRX_CABLE_CFG p_cable_cfg = 0;
        if(p_dprx->resume_connect==1) return 0;
        ret = dprx_hpd_get_cable_config(p_dprx->p_hpd, &p_cable_cfg);

        if(ret && p_dprx->cable_type != p_cable_cfg && p_dprx->current_port)
        {
            DPRX_ADP_INFO("dprx_hpd_get_cable_config : cable_status %x -> %x\n", p_dprx->cable_type, p_cable_cfg);
            rtk_dprx_set_typec_port_config(p_dprx, &p_cable_cfg);
            rtk_dprx_port_reset(p_dprx);     // change to type c port
            p_dprx->connected = 1;
            return 1;
        }

        if(p_dprx->dprx_port_readly)
            _update_typec_hpd_setting(p_dprx);  // typeC restore hpd_enable of AP
    }
    return 0;
}

#endif



/*------------------------------------------------
 * Func : rtk_dprx_get_connect_status
 *
 * Desc : get cable connection status
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : disconnect, 1 : connected
 *-----------------------------------------------*/
static int rtk_dprx_get_connect_status(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret = 0;

    if (p_dprx==NULL)
    {
        DPRX_ADP_WARN("get connect status rtk dprx port failed, p_dprx==NULL!!!! \n");
        return 0;
    }

#ifdef CONFIG_ENABLE_TYPEC_DPRX
    if (p_dprx->type == DP_TYPE_USB_TYPE_C)
        _check_typec_config(p_dprx);
#endif

#ifdef CONFIG_DPRX_FORCE_CHECK_MODE_FAIL_CABLE_OFF_ON
    if (p_dprx->fake_cable_off)
    {
        p_dprx->fake_cable_off--;
        DPRX_ADP_WARN("get connect status rtk dprx port failed, fake cable_off==%d!!!! \n", p_dprx->fake_cable_off);
        if (p_dprx->fake_cable_off==0)
            p_dprx->output_enable = 0;
        return 0;
    }
#endif

    if (p_dprx && p_dprx->p_hpd)
    {
        ret = dprx_hpd_get_connect_status(p_dprx->p_hpd);

        if (p_dprx->pre_cable_state != ret)
        {
            DPRX_ADP_INFO("cable state changed !!!! %d -> %d \n", p_dprx->pre_cable_state, ret);

            p_dprx->pre_cable_state = ret;

#ifdef DPRX_SUPPORT_SET_LOW_POWER_MODE
            if (p_dprx->current_port && (ret==0 || p_dprx->lpm_enable)&& p_dprx->resume_connect==0)
            {
                unsigned char connected = p_dprx->connected;

                if (ret==0) {
                    DPRX_ADP_INFO("cable disconnected !!!! do port reset\n");
                }
                else {
                    DPRX_ADP_INFO("cable connected with lpm_enable !!!! do port reset to exit low power mode\n");
                }

                rtk_dprx_port_reset(p_dprx);
                p_dprx->connected = connected;
            }
#else
            if (p_dprx->current_port && ret==0 && p_dprx->resume_connect==0)
            {
                unsigned char connected = p_dprx->connected;
                DPRX_ADP_INFO("cable disconnected !!!! do port reset\n");
                rtk_dprx_port_reset(p_dprx);
                p_dprx->connected = connected;
            }
#endif
        }

#ifdef CHECK_SOURCE_IN_GET_CONNECT_STATE
        if (ret)
        {
            if ((dprx_odal_get_system_time_ms() < p_dprx->source_check_time) &&
                (dprx_odal_get_system_time_ms() > p_dprx->source_check_time + 30))
            {
                rtk_dprx_source_check(p_dprx);
            }
        }
#endif
    }

    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_aux_status
 *
 * Desc : get aux status
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : aux disconnect, 1 : aux connected
 *-----------------------------------------------*/
static int rtk_dprx_get_aux_status(
    RTK_DPRX_ADP*   p_dprx
    )
{
    if (p_dprx==NULL)
    {
        DPRX_ADP_WARN("get connect status rtk dprx port failed, p_dprx==NULL!!!! \n");
        return -1;
    }

    return (p_dprx && p_dprx->p_hpd) ? dprx_hpd_get_aux_status(p_dprx->p_hpd, NULL) : 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_set_hpd
 *
 * Desc : set HPD status of the DPRX adapter
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_set_hpd(
    RTK_DPRX_ADP*   p_dprx,
    UINT8           enable
    )
{
    if (p_dprx==NULL)
    {
        DPRX_ADP_WARN("get hpd failed, invalid argments!!!!\n");
        return -1;
    }

    p_dprx->hpd_enable = (enable) ? 1 : 0;  // store hpd info

    if (p_dprx->p_hpd)
    {
        if (p_dprx->current_port)
        {
            if(p_dprx->sink_capabliity_update && p_dprx->hpd_enable)
            {
                // sink capabliity updata
                rtk_dprx_port_reset(p_dprx);     // change to type c port
                p_dprx->connected = 1;
            }
            else
            {
                // enable/disable hpd
                switch(p_dprx->type)
                {
                case DP_TYPE_USB_TYPE_C:
                    _update_typec_hpd_setting(p_dprx);
                    break;
                default :
                    dprx_hpd_set_hpd(p_dprx->p_hpd, p_dprx->hpd_enable);
                }
            }
        }
        else
            dprx_hpd_set_hpd(p_dprx->p_hpd, 0);   // always force hpd low if not current port
    }

    return 0;
}

/*------------------------------------------------
 * Func : rtk_dprx_set_hpd_toggle
 *
 * Desc : set HPD toggle signal of the DPRX adapter
 *
 * Para : p_adp : handle of DPRX adapter
 *        delay_ms  : the Interval of HPD toggle
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_set_hpd_toggle(
    RTK_DPRX_ADP*   p_dprx,
    UINT16           delay_ms
    )
{
    if (p_dprx==NULL)
    {
        DPRX_ADP_WARN("set hpd toggle failed, invalid argments!!!!\n");
        return -1;
    }

    p_dprx->hpd_enable = 0;

    if (p_dprx->p_hpd)
    {
        if (p_dprx->current_port)
            dprx_hpd_set_hpd_toggle(p_dprx->p_hpd, delay_ms);   // hpd_toggle
    }

    p_dprx->hpd_enable = 1;

    return 0;
}

/*------------------------------------------------
 * Func : rtk_dprx_set_irq_hpd
 *
 * Desc : set IRQ HPD of the DPRX adapter
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_set_irq_hpd(
    RTK_DPRX_ADP*   p_dprx
    )
{
    if (p_dprx==NULL)
        return 0;

    if (p_dprx->current_port)
        dprx_hpd_set_irq_hpd(p_dprx->p_hpd);   // set irq hpd

    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_hpd
 *
 * Desc : get HPD status of the DPRX adapter
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : HPD low, 1 : HPD high
 *-----------------------------------------------*/
static int rtk_dprx_get_hpd(
    RTK_DPRX_ADP*   p_dprx
    )
{
    if (p_dprx==NULL)
        return 0;

    if (p_dprx->current_port && p_dprx->p_hpd)
        return dprx_hpd_get_hpd(p_dprx->p_hpd);

    return p_dprx->hpd_enable;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_typec_pd_ready_status
 *
 * Desc : get typec pd ready status
 *
 * Para : p_dev : handle of DPRX adapter
 *
 * Retn : 0 : typec pd is ready, 1 : typec pd is busy
 *-----------------------------------------------*/
static int rtk_dprx_get_typec_pd_ready_status(
    RTK_DPRX_ADP*   p_dprx
    )
{
    if (p_dprx==NULL)
        return 0;

    if (p_dprx->current_port && p_dprx->p_hpd)
        return dprx_hpd_get_typec_pd_ready_status(p_dprx->p_hpd);

    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_set_edid
 *
 * Desc : set EDID of the DPRX adapter
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_edid : edid data
 *        edid_len : size of edid data
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_set_edid(
    RTK_DPRX_ADP*   p_dprx,
    unsigned char*  p_edid,
    unsigned short  edid_len
    )
{
    if (p_dprx==NULL || p_dprx->p_edid==NULL ||
        p_edid==NULL || edid_len > sizeof(p_dprx->sink_edid))
    {
        DPRX_ADP_WARN("set edid failed, invalid argments!!!!\n");
        return -1;
    }

    // save new edid
    memset(p_dprx->sink_edid, 0, sizeof(p_dprx->sink_edid)); // clear all edid setting
#ifdef FORCE_DEFAULT_EDID
    memcpy(p_dprx->sink_edid, default_edid, sizeof(default_edid));             // store edid to memory
#else
    memcpy(p_dprx->sink_edid, p_edid, edid_len);             // store edid to memory
#endif

    // apply new edid
    if (p_dprx->current_port)
    {
        return dprx_edid_set_edid(p_dprx->p_edid, p_dprx->sink_edid, sizeof(p_dprx->sink_edid));  // write edid to sram
    }

    return 0;
}


/*------------------------------------------------
 * Func : _ops_dprx_get_edid
 *
 * Desc : get EDID of the DPRX adapter
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_edid : edid data
 *        edid_len : size of edid data
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_edid(
    RTK_DPRX_ADP*   p_dprx,
    unsigned char*  p_edid,
    unsigned short  edid_len
    )
{
    if (p_dprx->p_edid==NULL || p_dprx==NULL ||
        p_edid==NULL || edid_len > sizeof(p_dprx->sink_edid))
    {
        DPRX_ADP_WARN("get edid failed, invalid argments!!!!\n");
        return -1;
    }

    if (p_dprx->current_port)
    {
        return dprx_edid_get_edid(p_dprx->p_edid, p_edid, edid_len);  // current port, read edid to sram
    }
    else
    {
        memcpy(p_edid, p_dprx->sink_edid, edid_len);   // store edid to memory
    }

    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_stream_type
 *
 * Desc : get dprx port stream type
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_stream_type(
    RTK_DPRX_ADP*       p_dprx,
    DP_STREAM_TYPE_E*   p_type
    )
{
    DPRX_VIDEO_STREAM_STATUS video_stream_status;

    if (p_dprx==NULL || p_type==NULL)
        return -1;

    // check dprx port status
    if (!p_dprx->current_port || p_dprx->connected==0)
    {
        *p_type = DPRX_STREAM_TYPE_UNKNOWN;  // not current port, or not connected force return unknown
        return 0;
    }

    if (dprx_drv_get_stream_type(p_dprx->mac_idx, &video_stream_status))
    {
        if (video_stream_status.video_mute)
        {
            if (video_stream_status.audio_mute)
                *p_type = DPRX_STREAM_TYPE_UNKNOWN;
            else
                *p_type = DPRX_STREAM_TYPE_AUDIO_ONLY;
        }
        else
        {
            if (video_stream_status.audio_mute)
                *p_type = DPRX_STREAM_TYPE_VIDEO_ONLY;
            else
                *p_type = DPRX_STREAM_TYPE_AUDIO_VIDEO;
        }
    }

    return 0;
}


/*------------------------------------------------
 * Func : _ops_dprx_get_capability
 *
 * Desc : get dprx port hardware capability
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_capability(
    RTK_DPRX_ADP*       p_dprx,
    DP_PORT_CAPABILITY* p_cap
    )
{
    DPRX_SINK_CAPABILITY dprx_sink_capability;

    if (p_dprx==NULL || p_cap==NULL)
        return -1;

    memset(p_cap, 0, sizeof(DP_PORT_CAPABILITY));

    if (p_dprx->current_port)  // current port, get from DPCD
    {
        if (dprx_drv_get_rx_port_capability(p_dprx->mac_idx, &dprx_sink_capability) !=_TRUE)
            return -1;

        p_cap->type     = p_dprx->type;
        p_cap->lane_cnt = dprx_sink_capability.max_lane_count; // g_dprx_sink_capability.max_lane_count;//GET_DPRX_DRV_SINK_LANE_COUNT();

        switch(dprx_sink_capability.max_link_rate)
        {
        case _DP_LINK_NONE:
            p_cap->link_rate_mask = 0;
            break;
        case _DP_LINK_RBR:
            p_cap->link_rate_mask = DP_LINK_RATE_RBR_MASK;
            break;
        case _DP_LINK_HBR:
            p_cap->link_rate_mask = DP_LINK_RATE_RBR_MASK | DP_LINK_RATE_HBR1_MASK;
            break;
        case _DP_LINK_HBR2:
            p_cap->link_rate_mask = DP_LINK_RATE_RBR_MASK | DP_LINK_RATE_HBR1_MASK | DP_LINK_RATE_HBR2_MASK;
            break;
        case _DP_LINK_HBR3:
            p_cap->link_rate_mask = DP_LINK_RATE_RBR_MASK | DP_LINK_RATE_HBR1_MASK | DP_LINK_RATE_HBR2_MASK | DP_LINK_RATE_HBR3_MASK;
            break;
        case _DP_LINK_UHBR10G:
            p_cap->link_rate_mask = DP_LINK_RATE_RBR_MASK | DP_LINK_RATE_HBR1_MASK | DP_LINK_RATE_HBR2_MASK | DP_LINK_RATE_HBR3_MASK | DP_LINK_RATE_UHBR10_MASK;
            break;
        case _DP_LINK_UHBR13p5G:
            p_cap->link_rate_mask = DP_LINK_RATE_RBR_MASK | DP_LINK_RATE_HBR1_MASK | DP_LINK_RATE_HBR2_MASK | DP_LINK_RATE_HBR3_MASK | DP_LINK_RATE_UHBR10_MASK | DP_LINK_RATE_UHBR13_13p5G;
            break;
        default:
            p_cap->link_rate_mask = 0;
            DPRX_ADP_WARN("_ops_dprx_get_capability failed, !!!!, unknow max link bw - %02x\n", dprx_sink_capability.max_link_rate);
            break;
        }

        p_cap->capability.fec_support = dprx_sink_capability.fec_support;
        p_cap->capability.dsc_support = dprx_sink_capability.dsc_support;
    }
    else       // non current port, get from config
    {
        memset(p_cap, 0, sizeof(DP_PORT_CAPABILITY));
        p_cap->type     = p_dprx->type;
        p_cap->lane_cnt = p_dprx->sink_cap.max_lane_count;

        switch(p_dprx->sink_cap.max_link_rate)
        {
        case _DP_LINK_NONE:
            p_cap->link_rate_mask = 0;
            break;
        case _DP_LINK_RBR:
            p_cap->link_rate_mask = DP_LINK_RATE_RBR_MASK;
            break;
        case _DP_LINK_HBR:
            p_cap->link_rate_mask = DP_LINK_RATE_RBR_MASK | DP_LINK_RATE_HBR1_MASK;
            break;
        case _DP_LINK_HBR2:
            p_cap->link_rate_mask = DP_LINK_RATE_RBR_MASK | DP_LINK_RATE_HBR1_MASK | DP_LINK_RATE_HBR2_MASK;
            break;
        case _DP_LINK_HBR3:
            p_cap->link_rate_mask = DP_LINK_RATE_RBR_MASK | DP_LINK_RATE_HBR1_MASK | DP_LINK_RATE_HBR2_MASK | DP_LINK_RATE_HBR3_MASK;
            break;
        case _DP_LINK_HBR10G:
            p_cap->link_rate_mask = DP_LINK_RATE_RBR_MASK | DP_LINK_RATE_HBR1_MASK | DP_LINK_RATE_HBR2_MASK | DP_LINK_RATE_HBR3_MASK | DP_LINK_RATE_UHBR10_MASK;
            break;
        default:
            p_cap->link_rate_mask = 0;
            DPRX_ADP_WARN("_ops_dprx_get_capability failed, !!!!, unknow max link bw - %02x\n", p_dprx->sink_cap.max_link_rate);
            break;
        }

        p_cap->capability.fec_support = p_dprx->sink_cap.fec_support;
        p_cap->capability.dsc_support = p_dprx->sink_cap.dsc_support;
    }

    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_link_status
 *
 * Desc : get dprx port link status
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_link_status(
    RTK_DPRX_ADP*       p_dprx,
    DP_LINK_STATUS_T*   p_status
    )
{
    if (p_dprx==NULL || p_status==NULL)
        return -1;

    memset(p_status, 0, sizeof(DP_LINK_STATUS_T));

    if (p_dprx->current_port)
    {
#ifdef DPRX_LINK_RATE_NONE

        DPRX_LINK_STATUS link_status;

        dprx_drv_get_link_status(p_dprx->mac_idx, &link_status);

        p_status->lane_mode         = link_status.lane_mode;
        p_status->channel_coding    = link_status.channel_coding;
        p_status->fec_enable        = link_status.fec_enable;
        p_status->scarmbling_enable = link_status.scarmbling_enable;
        p_status->channel_align     = link_status.channel_align;

        switch(p_status.link_rate)
        {
        case DPRX_LINK_RATE_RBR_1p62G:  p_status->link_rate = DP_LINK_RATE_RBR_1p62G; break;
        case DPRX_LINK_RATE_HBR1_2p7G:  p_status->link_rate = DP_LINK_RATE_HBR1_2p7G; break;
        case DPRX_LINK_RATE_HBR2_5p4G:  p_status->link_rate = DP_LINK_RATE_HBR2_5p4G; break;
        case DPRX_LINK_RATE_HBR3_8p1G:  p_status->link_rate = DP_LINK_RATE_HBR3_8p1G; break;
        case DPRX_LINK_RATE_UHBR10_10G: p_status->link_rate = DP_LINK_RATE_UHBR10_10G; break;
        case DPRX_LINK_RATE_UHBR20_20G: p_status->link_rate = DP_LINK_RATE_UHBR20_20G; break;
        case DPRX_LINK_RATE_UHBR13_13p5G: p_status->link_rate = DP_LINK_RATE_UHBR13_13p5G; break;
        default:                        p_status->link_rate = DP_LINK_RATE_MAX; break;  // should not drop into it
        }
#else
        /* FIX ME : get real value from DPCD */
        p_status->lane_mode = newbase_dprx_dpcd_get_lane_count(p_dprx->mac_idx);  // get from dpcd ?
        p_status->channel_coding = newbase_dprx_dpcd_get_main_link_channel_coding_set(p_dprx->mac_idx);  // get from dpcd ?
        p_status->fec_enable = newbase_dprx_dpcd_get_fec_ready(p_dprx->mac_idx);  // get from dpcd ?
        p_status->scarmbling_enable = newbase_dprx_dpcd_get_tps_scramble_disable(p_dprx->mac_idx) ?  0 : 1 ;
        p_status->channel_align = newbase_dprx_dpcd_get_lane_align_status(p_dprx->mac_idx);  // get from dpcd ?
        switch(newbase_dprx_dpcd_get_link_bw_set(p_dprx->mac_idx))
        {
            case 0x6:
                p_status->link_rate = DP_LINK_RATE_RBR_1p62G;
                break;

            case 0xA:
                p_status->link_rate = DP_LINK_RATE_HBR1_2p7G;
                break;

            case 0x14:
                p_status->link_rate = DP_LINK_RATE_HBR2_5p4G;
                break;

            case 0x1E:
                p_status->link_rate = DP_LINK_RATE_HBR3_8p1G;
                break;

            case 0x1:
                p_status->link_rate = DP_LINK_RATE_UHBR10_10G;
                break;

            case 0x2:
                p_status->link_rate = DP_LINK_RATE_UHBR20_20G;
                break;

            case 0x4:
                p_status->link_rate = DP_LINK_RATE_UHBR13_13p5G;
                break;

            default :
                p_status->link_rate = DP_LINK_RATE_MAX;
                break;
        }
        /*
        p_status->channel_status[0] = ;  // get from dpcd ?
        p_status->channel_status[1] = ;  // get from dpcd ?
        p_status->channel_status[2] = ;  // get from dpcd ?
        p_status->channel_status[3] = ;  // get from dpcd ?
        */
#endif
    }

    return 0;
}


extern DP_COLOR_SPACE_E _to_dp_color_space(
    UINT8           color_space
    );


extern DP_COLORIMETRY_E _to_dp_colorimetry(
    UINT8           colorimetry
    );


extern DP_COLORIMETRY_E _to_dp_colorimetry_ext(
    UINT8           colorimetry_ext
    );

extern DP_CONTENT_TYPE_E _to_dp_content_type(
    UINT8           content_type
    );

extern DP_DYNAMIC_RANGE_E _to_dp_dynamic_range(
    UINT8           dynamic_range
    );
extern DP_DRR_MODE_E _to_dp_drr_mode(DPRX_DRR_MODE_E mode);
extern const char* _to_dp_drr_mode_str(DPRX_DRR_MODE_E mode);

extern int _check_sdp_buff_expiration(
    DPRX_SDP_PKT_BUFFER* p_sdp,
    UINT32               expire_time
    );

extern unsigned char _to_dp_pixel_mode(DPRX_DRV_PIXEL_MODE_E pixel_mode);

static void _check_info_frame_expiration(RTK_DPRX_ADP* p_dprx, UINT32 frame_rate_hz)
{
    UINT32 frame_preoid;

    // get frame peroid
    if (frame_rate_hz < 20)
        frame_rate_hz = 20;    // Min frame rate
    else if (frame_rate_hz > 300)
        frame_rate_hz = 300;   // Max frame rate

    frame_preoid = ((1000 / frame_rate_hz) +1);  // frame perid in ms

    if (_check_sdp_buff_expiration(&p_dprx->sdp_handler.dvs, frame_preoid))    // for DolbyVision, the DVS should be updated every frame
    {
        DPRX_ADP_WARN("dolby vsion VSIF expired, force clear dolby vsif\n");
    }

    if (_check_sdp_buff_expiration(&p_dprx->sdp_handler.vsi, frame_preoid * 30))    // for non Dolby VSI
    {
        DPRX_ADP_WARN("VSI expired, force clear vsif\n");
    }

    if (_check_sdp_buff_expiration(&p_dprx->sdp_handler.adp_sync, frame_preoid))    // for Adaptive Sync, the DVS should be updated every frame
    {
        DPRX_ADP_WARN("Adaptive Sync SDP expired, force clear adaptive Sync SDP\n");
    }

    if (_check_sdp_buff_expiration(&p_dprx->sdp_handler.hdr, frame_preoid * 30))
    {
        DPRX_ADP_WARN("HDR INFO expired, force clear HDR SDP\n");
    }

    if (_check_sdp_buff_expiration(&p_dprx->sdp_handler.vsc, frame_preoid * 30))
    {
        DPRX_ADP_WARN("VSC INFO expired, force clear VSC\n");
    }

    if (_check_sdp_buff_expiration(&p_dprx->sdp_handler.spd, frame_preoid * 30))
    {
        DPRX_ADP_WARN("SPD INFO expired, force clear SPD, force disable freesync\n");
        p_dprx->sdp_handler.freesync_support = 0;
        p_dprx->sdp_handler.freesync_enable = 0;
        p_dprx->sdp_handler.freesync_activated = 0;
    }
}

#ifndef DPRX_ASYNCHRONOUS_DETECT_TASK
extern UINT32 g_check_source_detect_period;
extern void _check_source_detect_period(RTK_DPRX_ADP* p_dprx, UINT32 frame_rate_hz);
#endif

#ifdef CONFIG_SUPPORT_DOLBY_VISION_DP
static long (get_dolby_vision_dp_mode)(unsigned char port)
{
	return fp_get_dolby_vision_dp_mode ? fp_get_dolby_vision_dp_mode(port) : 0;
}

static int rtk_dprx_set_dolby_vision_check_mode_function(
    long (*get_dolby_vision_dp_mode)(unsigned char))
{
	fp_get_dolby_vision_dp_mode = get_dolby_vision_dp_mode;

	return 0;
}
#endif
/*------------------------------------------------
 * Func : rtk_dprx_get_video_timing
 *
 * Desc : get dprx port video timing
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_timing : pointer of timing info
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_video_timing(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_TIMING_INFO_T* p_timing
    )
{
    DPRX_TIMING_INFO dprx_timing_info;

    if (p_dprx==NULL || p_timing==NULL || g_skip_source_detect)
        return -1;

    memset(p_timing, 0, sizeof(DPRX_TIMING_INFO_T));

    // check dprx port status
    if (!p_dprx->current_port || p_dprx->connected==0)
        return -1;

#ifdef CONFIG_DPRX_FORCE_CHECK_MODE_FAIL_CABLE_OFF_ON
    if (p_dprx->fake_cable_off)
        return -1;
#endif

#ifdef CHECK_SOURCE_IN_GET_CONNECT_STATE
    rtk_dprx_source_check(p_dprx);
#endif

    // get timing info from low level driver
    if (dprx_drv_get_timing_info(p_dprx->mac_idx, &dprx_timing_info)==_TRUE)
    {
        // check
        _check_info_frame_expiration(p_dprx,dprx_timing_info.vfreq_hz_x100/100);

#ifndef DPRX_ASYNCHRONOUS_DETECT_TASK
        _check_source_detect_period(p_dprx,dprx_timing_info.vfreq_hz_x100/100);
#endif

        p_timing->htotal        = dprx_timing_info.htotal;
        p_timing->hstart        = dprx_timing_info.hstart;
        p_timing->hact          = dprx_timing_info.hact;
        p_timing->hsync         = dprx_timing_info.hsync;
        p_timing->hporch        = p_timing->htotal - p_timing->hact;
        p_timing->vtotal        = dprx_timing_info.vtotal;
        p_timing->vstart        = dprx_timing_info.vstart;
        p_timing->vact          = dprx_timing_info.vact;
        p_timing->vsync         = dprx_timing_info.vsync;
        p_timing->vporch        = p_timing->vtotal - p_timing->vact;
        p_timing->hfreq_hz      = dprx_timing_info.hfreq_hz;
        p_timing->vfreq_hz_x100 = dprx_timing_info.vfreq_hz_x100;
        p_timing->is_interlace  = dprx_timing_info.is_interlace;
        p_timing->is_dsc        = dprx_timing_info.is_dsc;
        p_timing->drr_mode      = _to_dp_drr_mode(dprx_timing_info.drr_mode);

        if (p_timing->drr_mode==DP_DRR_MODE_AMD_FREE_SYNC)
        {
            if ((p_dprx->sdp_handler.freesync_support==0) ||
                (p_dprx->sdp_handler.freesync_enable == 0 && p_dprx->sdp_handler.freesync_activated==0))
                p_timing->drr_mode = DP_DRR_MODE_FRR;
        }

        p_timing->curr_vfreq_hz_x100 = dprx_timing_info.curr_vfreq_hz_x100;
        p_timing->isALLM        = 0;       // not implemented yet

        p_timing->pixel_mode    = _to_dp_pixel_mode(dprx_timing_info.pixel_mode);
#ifdef DPRX_DRV_OFFMS_PIXEL_MODE_SUPPORT
        p_timing->pixel_mode_offms = _to_dp_pixel_mode(dprx_timing_info.pixel_mode_offms);
#else
        p_timing->pixel_mode_offms = DPRX_OUT_PIXEL_MODE_2P;  // for backward compatible (should be removed later)
#endif

#ifdef CONFIG_SUPPORT_DOLBY_VISION_DP
        p_timing->is_dolby_hdr  = get_dolby_vision_dp_mode(p_dprx->mac_idx) ? 1 : 0;
#else
        p_timing->is_dolby_hdr  = 0;
#endif
        p_timing->color_space   = _to_dp_color_space(dprx_timing_info.color_space);
        p_timing->color_depth   = (DP_COLOR_DEPTH_E)dprx_timing_info.color_depth;
        p_timing->hs_polarity   = HS_POL_POSITIVE;  // dprx drv always uses positive polarity
        p_timing->vs_polarity   = VS_POL_POSITIVE;  // dprx drv always uses positive polarity

        if (dprx_timing_info.drr_mode || g_force_demode || dprx_timing_info.is_dsc)
            p_timing->ptg_mode = DP_PTG_REGEN_NO_VSYNC_MODE;
        else
            p_timing->ptg_mode = DP_PTG_REGEN_HV_MODE;

#ifdef CONFIG_DPRX_FORCE_CHECK_MODE_FAIL_CABLE_OFF_ON
        if (p_timing->htotal && p_timing->vtotal)
            p_dprx->output_enable = 1;
#endif

#ifdef DPRX_DRV_TIMING_OVER_DISP_LIMIT
        if ((dprx_timing_info.ext_flags & DPRX_DRV_TIMING_OVER_DISP_LIMIT))
            p_timing->ext_flags |= DPRX_TIMING_OVER_DISP_LIMIT;
#endif

#ifdef DPRX_DRV_TIMING_BYPASSABLE
        if ((dprx_timing_info.ext_flags & DPRX_DRV_TIMING_BYPASSABLE))
            p_timing->ext_flags |= DPRX_TIMING_BYPASSABLE;
#endif

#ifdef DPRX_DRV_TIMING_V_DROP_ENABLE
        if ((dprx_timing_info.ext_flags & DPRX_DRV_TIMING_V_DROP_ENABLE))
            p_timing->ext_flags |= DPRX_TIMING_V_DROP_EN;
#endif

#ifdef DPRX_DRV_TIMING_H_DROP_ENABLE
        if ((dprx_timing_info.ext_flags & DPRX_DRV_TIMING_H_DROP_ENABLE))
            p_timing->ext_flags |= DPRX_TIMING_H_DROP_EN;
#endif

        if (g_force_timing_ext_flag_en)
        {
            // this flas is reverved for force
            DPRX_ADP_WARN("g_force_timing_ext_flag_en is enabled, force timing ext flags %02x to %02x\n",
                p_timing->ext_flags, g_force_timing_ext_flag);
            p_timing->ext_flags = g_force_timing_ext_flag;   //
        }

        return 0;
    }
    else
    {
        _check_info_frame_expiration(p_dprx,0);  // use default frame rate for infor frame check
    }

    return -1;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_stream_bypasss_timing
 *
 * Desc : get dprx port stream bypass timing
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_timing : pointer of timing info
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_stream_bypasss_timing(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_TIMING_INFO_T* p_timing
    )
{
    if (rtk_dprx_get_video_timing(p_dprx, p_timing)<0)
        return -1;

    if (p_timing->is_dsc)
    {
        UINT8   pps[128];
        UINT16  bpp;
        UINT8   bpc;

        if (dprx_drv_read_current_pps(p_dprx->mac_idx, pps, sizeof(pps))!=_TRUE)
            return -1;

        bpp = ((pps[4] & 0x3)<<8) + pps[5]; // PPS4[1:0] | PPS5[7:0]
        bpc = (pps[3]>>4) & 0xF;            // PPS3[7:4]

        // update htt / hact / hblank to hctt hcact hcblank
        // bpp_ratio = compressed_bits_per_pixe/ / uncompressed bits per pixel
        //           = (bpp / 16) / (bpc * 3) = bpp / (bpc * 3 * 16)
        //
        DPRX_ADP_INFO("_dprx_timing_to_measure_timing, bpc=%d, bpp=%d/16, htt=%d, hact=%d, h_act_sta=%d, hsync=%d\n",
            bpc, bpp,
            p_timing->htotal,
            p_timing->hact,
            p_timing->hstart,
            p_timing->hsync);

        p_timing->htotal = p_timing->htotal  * bpp / (bpc * 48);
        p_timing->hact   = p_timing->hact * bpp / (bpc * 48);
        p_timing->hstart = p_timing->hstart * bpp / (bpc * 48);
        p_timing->hsync  = p_timing->hsync * bpp / (bpc * 48);

        DPRX_ADP_INFO("_dprx_timing_to_measure_timing, HCtt=%d, HCact=%d, HCsta=%d, HCsync=%d\n",
            p_timing->htotal,
            p_timing->hact,
            p_timing->hstart,
            p_timing->hsync);
    }

    return 0;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_pixel_encoding_info
 *
 * Desc : get dprx port pixel format info
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_pixel : pointer of pixel encoding format
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_pixel_encoding_info(
    RTK_DPRX_ADP*       p_dprx,
    DP_PIXEL_ENCODING_FORMAT_T*    p_pixel
    )
{
    DPRX_PIXEL_ENCODING_FORMAT dprx_pixel_format;

    if (p_dprx==NULL || p_pixel==NULL)
        return -1;

    // check dprx port status
    if (!p_dprx->current_port || p_dprx->connected==0)
        return -1;

    memset(p_pixel, 0, sizeof(DP_PIXEL_ENCODING_FORMAT_T));

    if (dprx_drv_get_pixel_encoding_info(p_dprx->mac_idx, &dprx_pixel_format)!=_TRUE)
        return -1;

    // convert data types
    p_pixel->color_depth = dprx_pixel_format.color_depth_enum;

    p_pixel->color_space = _to_dp_color_space(dprx_pixel_format.color_space_enum);

    if (dprx_pixel_format.colorimetry_enum != _COLORIMETRY_EXT)
        p_pixel->colorimetry = _to_dp_colorimetry(dprx_pixel_format.colorimetry_enum);
    else
        p_pixel->colorimetry = _to_dp_colorimetry_ext(dprx_pixel_format.colorimetry_ext_enum);

    p_pixel->dynamic_range = _to_dp_dynamic_range(dprx_pixel_format.dynamic_range_enum);

    p_pixel->content_type = _to_dp_content_type(dprx_pixel_format.content_type_enum);

    return 0;
}


#ifdef CONFIG_SUPPORT_GET_AVI_INFO


/*------------------------------------------------
 * Func : _gen_avi_by_avi_sdp
 *
 * Desc : generate avi info from avi sdp
 *
 * Para : p_avi : pointer of avi info
 *        p_avi_sdp : avi sdp
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int _gen_avi_by_avi_sdp(
    DPRX_AVI_INFO_T*    p_avi,
    DP_SDP_DATA_T*      p_avi_sdp
    )
{
    // Referenct : DP2.1 5.3.3.5.8.1.3 AVI info frame generation
    // PON should retransmit the received infoframe if DP source transmit it

    // TODO: FIXME

    return -1;
}


/*------------------------------------------------
 * Func : _update_avi_color_space
 *
 * Desc : update Color space code for AVI
 *        Y2:Y1:Y0 : RGB or YCbCr,
 *
 * Para : color_space
 *
 * Retn : AVI color space code
 *-----------------------------------------------*/
static void _update_avi_color_space(
    DPRX_AVI_INFO_T*            p_avi,
    DPRX_PIXEL_ENCODING_FORMAT* p_pixel_format
    )
{
    switch (p_pixel_format->color_space_enum)
    {
    case _COLOR_SPACE_Y_ONLY:
    case _COLOR_SPACE_YCBCR444:
        p_avi->ePixelEncoding =  DP_AVI_CSC_YCBCR444;
        break;

    case _COLOR_SPACE_YCBCR422:
        p_avi->ePixelEncoding = DP_AVI_CSC_YCBCR422;
        break;

    case _COLOR_SPACE_YCBCR420:
        p_avi->ePixelEncoding = DP_AVI_CSC_YCBCR420;
        break;

    default:
    case _COLOR_SPACE_RGB:
    case _COLOR_SPACE_RAW:
        p_avi->ePixelEncoding = DP_AVI_CSC_RGB;
        break;
    }
}

/*------------------------------------------------
 * Func : _update_avi_colorimetry
 *
 * Desc : update colorimetry info of AVI
 *        C1:C0 : Colorimetry
 *        EC2~EC0 : Extended Colorimetry
 *        ACE3~ACE0 : Additional Colorimetry Extension
 *
 * Para : p_avi
 *        p_pixel_format
 *
 * Retn : N/A
 *-----------------------------------------------*/
static void _update_avi_colorimetry(
    DPRX_AVI_INFO_T*            p_avi,
    DPRX_PIXEL_ENCODING_FORMAT* p_pixel_format
    )
{
    // set initial value to no data
    p_avi->eColorimetry = DP_AVI_COLORIMETRY_NODATA;
    p_avi->eExtendedColorimetry = 0;
    p_avi->eAdditionalColorimetry = 0;

    if (p_pixel_format->colorimetry_enum != _COLORIMETRY_EXT)
    {
        switch(p_pixel_format->colorimetry_enum)
        {
        case _COLORIMETRY_RGB_ADOBERGB:
            p_avi->eColorimetry = DP_AVI_COLORIMETRY_FUTURE;
            p_avi->eExtendedColorimetry = DP_AVI_EXT_COLORIMETRY_ADOBERGB;
            break;

        case _COLORIMETRY_YCC_SMPTE_170M:
            p_avi->eColorimetry = DP_AVI_COLORIMETRY_SMPTE170;
            break;

        case _COLORIMETRY_YCC_ITUR_BT601:
            p_avi->eColorimetry = DP_AVI_COLORIMETRY_FUTURE;
            p_avi->eExtendedColorimetry = DP_AVI_EXT_COLORIMETRY_XVYCC601;
            break;

        case _COLORIMETRY_YCC_ITUR_BT709:
            p_avi->eColorimetry = DP_AVI_COLORIMETRY_ITU709;
            break;

        case _COLORIMETRY_YCC_XVYCC601:
            p_avi->eColorimetry = DP_AVI_COLORIMETRY_FUTURE;
            p_avi->eExtendedColorimetry = DP_AVI_EXT_COLORIMETRY_XVYCC601;
            break;

        case _COLORIMETRY_YCC_XVYCC709:
            p_avi->eColorimetry = DP_AVI_COLORIMETRY_FUTURE;
            p_avi->eExtendedColorimetry = DP_AVI_EXT_COLORIMETRY_XVYCC709;
            break;

        default:
        //case _COLORIMETRY_RGB_SRGB:
        //case _COLORIMETRY_RGB_XRRGB:
        //case _COLORIMETRY_RGB_SCRGB:
        //case _COLORIMETRY_RGB_DCI_P3:
        //case _COLORIMETRY_RGB_COLOR_PROFILE:
        //case _COLORIMETRY_Y_ONLY:
        //case _COLORIMETRY_RAW:
            break;
        }
    }
    else
    {
        switch (p_pixel_format->colorimetry_ext_enum)
        {
        case _COLORIMETRY_EXT_RGB_ADOBERGB :
            p_avi->eColorimetry = DP_AVI_COLORIMETRY_FUTURE;
            p_avi->eExtendedColorimetry = DP_AVI_EXT_COLORIMETRY_ADOBERGB;
            break;

        case _COLORIMETRY_EXT_YCC_ITUR_BT601 :
        case _COLORIMETRY_EXT_YCC_XVYCC601 :
            p_avi->eColorimetry = DP_AVI_COLORIMETRY_FUTURE;
            p_avi->eExtendedColorimetry = DP_AVI_EXT_COLORIMETRY_XVYCC601;
            break;

        case _COLORIMETRY_EXT_YCC_ITUR_BT709 :
        case _COLORIMETRY_EXT_YCC_XVYCC709 :
            p_avi->eColorimetry = DP_AVI_COLORIMETRY_FUTURE;
            p_avi->eExtendedColorimetry = DP_AVI_EXT_COLORIMETRY_XVYCC709;
            break;

        case _COLORIMETRY_EXT_YCC_ITUR_BT2020_CL :
        case _COLORIMETRY_EXT_YCC_ITUR_BT2020_NCL :
        case _COLORIMETRY_EXT_RGB_ITUR_BT2020 :
            p_avi->eColorimetry = DP_AVI_COLORIMETRY_FUTURE;
            p_avi->eExtendedColorimetry = DP_AVI_EXT_COLORIMETRY_BT2020_RGBORYCBCR;
            break;

        case _COLORIMETRY_EXT_YCC_SYCC601 :
            p_avi->eColorimetry = DP_AVI_COLORIMETRY_FUTURE;
            p_avi->eExtendedColorimetry = DP_AVI_EXT_COLORIMETRY_SYCC601;
            break;

        case _COLORIMETRY_EXT_YCC_ADOBEYCC601 :
            p_avi->eColorimetry = DP_AVI_COLORIMETRY_FUTURE;
            p_avi->eExtendedColorimetry = DP_AVI_EXT_COLORIMETRY_ADOBEYCC601;
            break;

        default :
        //case _COLORIMETRY_EXT_RGB_SRGB :
        //case _COLORIMETRY_EXT_RGB_XRRGB :
        //case _COLORIMETRY_EXT_RGB_SCRGB :
        //case _COLORIMETRY_EXT_RGB_DCI_P3 :
        //case _COLORIMETRY_EXT_RGB_CUSTOM_COLOR_PROFILE :
        //case _COLORIMETRY_EXT_RAW_CUSTOM_COLOR_PROFILE :
        //case _COLORIMETRY_EXT_Y_ONLY_DICOM_PART14 :
            break;
        }
    }
}

/*------------------------------------------------
 * Func : _update_avi_quantization_range
 *
 * Desc : update quantization range fields of AVI
 *        YQ1/YQ0 : YCC_QUANTIZATION_RANG
 *        Q1/Q0 : RGB_QUANTIZATION_RANGE
 *
 * Para : p_avi
 *        p_pixel_format
 *
 * Retn : N/A
 *-----------------------------------------------*/
static void _update_avi_quantization_range(
    DPRX_AVI_INFO_T*            p_avi,
    DPRX_PIXEL_ENCODING_FORMAT* p_pixel_format
    )
{
    if (p_pixel_format->dynamic_range_enum == _DP_COLOR_QUANTIZATION_FULL)
    {
        p_avi->eYCCQuantizationRange = DP_AVI_YCC_QUANTIZATION_RANGE_FULLRANGE;
        p_avi->eRGBQuantizationRange = DP_AVI_RGB_QUANTIZATION_RANGE_FULLRANGE;
    }
    else
    {
        p_avi->eYCCQuantizationRange = DP_AVI_YCC_QUANTIZATION_RANGE_LIMITEDRANGE;
        p_avi->eRGBQuantizationRange = DP_AVI_RGB_QUANTIZATION_RANGE_LIMITEDRANGE;
    }
}


/*------------------------------------------------
 * Func : _update_avi_content_type
 *
 * Desc : update content type for AVI
 *        ITC : IT_CONTENT
 *        CN1/CN0 CONTENT_TYPE
 *
 * Para : content_type
 *
 * Retn : N/A
 *
 *-----------------------------------------------*/
static void _update_avi_content_type(
    DPRX_AVI_INFO_T*                p_avi,
    DPRX_PIXEL_ENCODING_FORMAT*     p_pixel_format
    )
{
    switch(p_pixel_format->content_type_enum)
    {
    case 1 :
        p_avi->eITContent    = DP_AVI_IT_CONTENT_ITCONTENT;
        p_avi->eContentType  = DP_AVI_CONTENT_TYPE_GRAPHICS;
        //DPRX_ADP_WARN("AVI content = Graphic (%d)\n", p_pixel_format->content_type_enum);
        break;

    case 2 :
        p_avi->eITContent    = DP_AVI_IT_CONTENT_ITCONTENT;
        p_avi->eContentType  = DP_AVI_CONTENT_TYPE_PHOTO;
        //DPRX_ADP_WARN("AVI content = Photo (%d)\n", p_pixel_format->content_type_enum);
        break;

    case 3 :
        p_avi->eITContent    = DP_AVI_IT_CONTENT_ITCONTENT;
        p_avi->eContentType  = DP_AVI_CONTENT_TYPE_CINEMA;
        //DPRX_ADP_WARN("AVI content = Cinema (%d)\n", p_pixel_format->content_type_enum);
        break;

    case 4 :
        p_avi->eITContent    = DP_AVI_IT_CONTENT_ITCONTENT;
        p_avi->eContentType  = DP_AVI_CONTENT_TYPE_GAME;
        //DPRX_ADP_WARN("AVI content = Game (%d)\n", p_pixel_format->content_type_enum);
        break;

    default :
        p_avi->eITContent    = DP_AVI_IT_CONTENT_NODATA;     // force ITC = 0
        p_avi->eContentType  = DP_AVI_CONTENT_TYPE_GRAPHICS; // force content type to graphic
        //DPRX_ADP_WARN("AVI content = unkown - graphic (%d)\n", p_pixel_format->content_type_enum);
        break;
    }
}

static void _update_avi_picture_aspect_ratio_by_msa(
    DPRX_AVI_INFO_T*    p_avi,
    DPRX_MSA_INFO_T*    p_msa
    )
{
    unsigned short hv_ratio;

    if ((p_msa->hwidth *3)==(p_msa->vwidth*4))
    {
        p_avi->ePictureAspectRatio = DP_AVI_PICTURE_ARC_4_3;
    }
    else if ((p_msa->hwidth *9)==(p_msa->vwidth*16))
    {
        p_avi->ePictureAspectRatio = DP_AVI_PICTURE_ARC_16_9;
    }
    else
    {
        // check hvratio
        // 4:3   = 1.33 : 1
        // 14:9  = 1.55 : 1
        // 16:9  = 1.7 : 1
        // 21:9  = 2.3 : 1
        // 64:27 = 2.37 : 1
        hv_ratio = p_msa->hwidth * 10 / p_msa->vwidth;
        if (hv_ratio > 15)
            p_avi->ePictureAspectRatio = DP_AVI_PICTURE_ARC_16_9;
        else
            p_avi->ePictureAspectRatio = DP_AVI_PICTURE_ARC_4_3;
    }
}

static void _update_avi_bar_info(
    DPRX_AVI_INFO_T*    p_avi,
    DPRX_MSA_INFO_T*    p_msa
    )
{
    p_avi->eBarInfo = DP_AVI_BAR_INFO_INVALID;
    p_avi->TopBarEndLineNumber = 0;      // no Top Bar
    p_avi->BottomBarStartLineNumber = p_msa->vwidth+1; // no Bottom Bar
    p_avi->LeftBarEndPixelNumber = 0;    // no left Bar
    p_avi->RightBarEndPixelNumber = p_msa->hwidth+1;   // no Top Bar
}

static void _update_avi_vic_info(
    DPRX_AVI_INFO_T*    p_avi,
    DPRX_MSA_INFO_T*    p_msa
    )
{
    // FIXME : search vic code ?
    p_avi->VideoIdCode = 0;   // force not CTA timing
}


/*------------------------------------------------
 * Func : _gen_avi_by_msa_vsc
 *
 * Desc : generate avi info from msa and vsc
 *
 * Para : p_avi : pointer of avi info
 *        p_vsc : vsc sdp
 *
 * Reference : DP 2.1 spec  Table 5-11: Pixel Encoding/Colorimetry Indication of AVI INFOFRAME
 *             Using MSA Packet and VSC SDP
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int _gen_avi_by_msa_vsc(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_AVI_INFO_T*    p_avi,
    DPRX_MSA_INFO_T*    p_msa,
    DP_SDP_DATA_T*      p_vsc
    )
{
    // To simpilty the implementation, we use low level driver pixel encoding info to
    // get color space, colorimertry, content type ,to generate AVI info

    DPRX_PIXEL_ENCODING_FORMAT dprx_pixel_format;

    if (p_avi==NULL || p_msa ==NULL || dprx_drv_get_pixel_encoding_info(p_dprx->mac_idx, &dprx_pixel_format)!=_TRUE)
        return -1;

    // Fix value field
    p_avi->eActiveInfo              = DP_AVI_ACTIVE_INFO_VALID;         // A0 : Active Format Information Presetn, (Default 1)
    p_avi->eScanInfo                = DP_AVI_SCAN_INFO_UNDERSCANNED;    // S1:S0 : Bar Info, (Default 2) (Under Scan)
    p_avi->ePictureAspectRatio      = DP_AVI_PICTURE_ARC_4_3;           // M1:M0 : Colored Frame Aspect Ratio, (Default: 1)
    p_avi->eActiveFormatAspectRatio = DP_AVI_ACTIVE_FORMAT_ARC_8_FULL_FRAME_IMAGE;     // R3-R0 Active Portion Aspect Ratio (Default: 0x8)
    p_avi->PixelRepeat              = 0;                                // PR3:PR2:PR1:PR0 : Pixel Repetition Factor (Defailt 0: Graphic)

    // values from MSA/VSC
    _update_avi_color_space(p_avi, &dprx_pixel_format);                 // Y2:Y1:Y0 : RGB or YCbCr, Derived from VSC packet

    _update_avi_colorimetry(p_avi, &dprx_pixel_format);                 // C1:C0 : Colorimetry, Derived from MSA packet
                                                                        // EC2~EC0 : Derived from MSA for xvYCC, opRGB, DCI-P3

    _update_avi_quantization_range(p_avi, &dprx_pixel_format);          // YQ1/YQ0 : YCC_QUANTIZATION_RANG, Derived from MSA MISC0[3]
                                                                        // Q1/Q0 : RGB_QUANTIZATION_RANGE, Derived from MSA MISC0[3]
                                                                        // MISC0 bit3 :  0 : Full, 1 : limited

    _update_avi_content_type(p_avi, &dprx_pixel_format);                // ITC : IT_CONTENT (Default: 0x0)
                                                                        // CN1/CN0 CONTENT_TYPE (Defailt 0: Graphic) when ITC==0

    _update_avi_vic_info(p_avi, p_msa);                                 // VIC : Derived from video timing parameters in MSA packet

    _update_avi_bar_info(p_avi, p_msa);                                 // B1:B0 : Bar Info, Default 0
                                                                        // ETB / SBB / ELB / SRB

    // update avi packet status
    p_avi->packetStatus = DP_PACKET_STATUS_UPDATED;
    p_avi->packet.type = 0x2;  // AVI
    p_avi->packet.version = 0x3;
    p_avi->packet.length = 13;
    return 0;
}


/*------------------------------------------------
 * Func : _gen_avi_by_msa_alone
 *
 * Desc : generate avi info from msa data
 *
 * Para : p_avi : pointer of avi info
 *
 * Retn : 0 : successed, 0< : failed
 *
 * Reference : DP 2.1 Table 5-12: Pixel Encoding/Colorimetry Indication Using MSA Packet Alone
 *
 *-----------------------------------------------*/
static int _gen_avi_by_msa_alone(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_AVI_INFO_T*    p_avi,
    DPRX_MSA_INFO_T*    p_msa
    )
{
    // To simpilty the implementation, we use low level driver pixel encoding info to
    // get color space, colorimertry, content type ,to generate AVI info

    DPRX_PIXEL_ENCODING_FORMAT dprx_pixel_format;

    if (p_avi==NULL || p_msa==NULL || dprx_drv_get_pixel_encoding_info(p_dprx->mac_idx, &dprx_pixel_format)!=_TRUE)
        return -1;

    // Fix value field
    p_avi->eActiveInfo              = DP_AVI_ACTIVE_INFO_VALID;         // A0 : Active Format Information Presetn, Default 1
    p_avi->eScanInfo                = DP_AVI_SCAN_INFO_OVERSCANNED;     // S1:S0 : Scan Info, Default 1 (Over Scan)
    p_avi->eActiveFormatAspectRatio = DP_AVI_ACTIVE_FORMAT_ARC_8_FULL_FRAME_IMAGE; // R3-R0 Active Portion Aspect Ratio (Default: 0x8)
    p_avi->eContentType             = DP_AVI_CONTENT_TYPE_GRAPHICS;     // CN1/CN0 CONTENT_TYPE (Defailt 0: Graphic)
    p_avi->PixelRepeat              = 0;                                // PR3:PR2:PR1:PR0 : Pixel Repetition Factor (Defailt 0: Graphic)
    p_avi->eITContent               = DP_AVI_IT_CONTENT_NODATA;         // ITC : IT_CONTENT (Default: 0x0)

    // values from MSA packet
    _update_avi_color_space(p_avi, &dprx_pixel_format);                 // Y2:Y1:Y0 : RGB or YCbCr, Derived from MSA packet

    _update_avi_colorimetry(p_avi, &dprx_pixel_format);                 // C1:C0 : Colorimetry, Derived from MSA packet
                                                                        // EC2~EC0 : Derived from MSA for xvYCC, opRGB, DCI-P3

    _update_avi_quantization_range(p_avi, &dprx_pixel_format);          // YQ1/YQ0 : YCC_QUANTIZATION_RANG, Derived from MSA MISC0[3]
                                                                        // Q1/Q0 : RGB_QUANTIZATION_RANGE, Derived from MSA MISC0[3]
                                                                        // MISC0 bit3 :  0 : Full, 1 : limited

    _update_avi_vic_info(p_avi, p_msa);                                 // VIC : Derived from video timing parameters in MSA packet

    _update_avi_picture_aspect_ratio_by_msa(p_avi, p_msa);              // M1:M0 : Colored Frame Aspect Ratio, derivered from msa

    _update_avi_bar_info(p_avi, p_msa);                                 // B1:B0 : Bar Info, Default 0
                                                                        // ETB / SBB / ELB / SRB

    // update avi packet status
    p_avi->packetStatus = DP_PACKET_STATUS_UPDATED;
    p_avi->packet.type = 0x2;  // AVI
    p_avi->packet.version = 0x3;
    p_avi->packet.length = 13;
    return 0;
}


#endif // #ifdef CONFIG_SUPPORT_GET_AVI_INFO


/*------------------------------------------------
 * Func : rtk_dprx_get_avi_info
 *
 * Desc : get dprx port avi drm
 *
 * Para : p_adp : handle of DPRX adapter
 *        p_avi : pointer of avi info
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_avi_info(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_AVI_INFO_T*    p_avi
    )
{
#ifdef CONFIG_SUPPORT_GET_AVI_INFO

    DP_SDP_DATA_T sdp;
    DPRX_MSA_INFO_T msa;

    if (p_avi==NULL)
        return -1;

    memset(p_avi, 0, sizeof(DPRX_AVI_INFO_T));
    p_avi->packetStatus = DP_PACKET_STATUS_NOT_RECEIVED;

    if (p_dprx==NULL || !p_dprx->current_port ||
        p_dprx->connected==0)
        return -1;

    // If a DP Source device is transmitting an AVI INFOFRAME encapsulated in an SDP with the
    // INFOFRAME mapping version 1.3, a PCON shall retransmit the received AVI INFOFRAME
    // as is.
    if (rtk_dprx_get_sdp_data(p_dprx, DP_SDP_TYPE_AVI, &sdp)==0)  // if AVI info present
        return _gen_avi_by_avi_sdp(p_avi, &sdp);

    if (rtk_dprx_get_msa_data(p_dprx, &msa)==0)
    {
        // if a DP Source device is transmitting a VSC SDP, a PCON shall generate an
        // AVI INFOFRAME from the parameters of an MSA packet and a VSC SDP
        // Else, a PCON shall generate an AVI INFOFRAME from an MSA packet alone
        if (rtk_dprx_get_sdp_data(p_dprx, DP_SDP_TYPE_VSC, &sdp)==0)
            return _gen_avi_by_msa_vsc(p_dprx, p_avi, &msa, &sdp);
        else
            return _gen_avi_by_msa_alone(p_dprx, p_avi, &msa);
    }
#endif

    return -1;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_drm_info
 *
 * Desc : get dprx port drm drm
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_drm : pointer of drm info
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_drm_info(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_DRM_INFO_T*    p_drm
    )
{
#ifdef CONFIG_SUPPORT_GET_DRM_INFO

    DP_SDP_DATA_T hdr_sdp;

    if (p_drm==NULL)
        return -1;

    memset(p_drm, 0, sizeof(DPRX_DRM_INFO_T));

    if (p_dprx==NULL || !p_dprx->current_port ||
        p_dprx->connected==0 || rtk_dprx_get_sdp_data(p_dprx, DP_SDP_TYPE_HDR, &hdr_sdp)!=0)
        return -1;

    memcpy(p_drm, hdr_sdp.pb, sizeof(DPRX_DRM_INFO_T));
    return 0;

#else
    return -1;
#endif
}


/*------------------------------------------------
 * Func : rtk_dprx_get_vbid_data
 *
 * Desc : get vbid data from a given DPRX Port
 *
 * Para : p_dprx : handle of DPRX adapter
*         p_msa  : SDP type
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_vbid_data(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_VBID_INFO_T*   p_vbid
    )
{
    UINT8 vbid;

    if (p_dprx==NULL || p_vbid==NULL)
        return -1;

    memset(p_vbid, 0, sizeof(DPRX_VBID_INFO_T));

    if (!p_dprx->current_port || p_dprx->connected==0)
        return 0;  // not current port, report zero mas

    vbid = dprx_drv_read_vbid(p_dprx->mac_idx);
    memcpy(p_vbid, &vbid, 1);
    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_msa_data
 *
 * Desc : get msa data from a given DPRX Port
 *
 * Para : p_dprx : handle of DPRX adapter
*         p_msa  : SDP type
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_msa_data(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_MSA_INFO_T*    p_msa
    )
{
    DPRX_MSA msa;

    if (p_dprx==NULL || p_msa==NULL)
        return -1;

    memset(p_msa, 0, sizeof(DPRX_MSA_INFO_T));
    memset(&msa, 0, sizeof(msa));

    if (!p_dprx->current_port || p_dprx->connected==0)
        return 0;  // not current port, report zero mas

    dprx_drv_read_msa(p_dprx->mac_idx, &msa);

    p_msa->mvid         = msa.mvid;
    p_msa->nvid         = msa.nvid;
    p_msa->htotal       = msa.htotal;
    p_msa->vtotal       = msa.vtotal;
    p_msa->hstart       = msa.hstart;
    p_msa->vstart       = msa.vstart;
    p_msa->hsync_polarity = msa.hsync_polarity;
    p_msa->vsync_polarity = msa.vsync_polarity;
    p_msa->hsync_width  = msa.hsync_width;
    p_msa->vsync_width  = msa.vsync_width;
    p_msa->hwidth       = msa.hwidth;
    p_msa->vwidth       = msa.vwidth;
    p_msa->misc0        = msa.misc0;
    p_msa->misc1        = msa.misc1;
    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_sdp_data
 *
 * Desc : get sdp data from a given DPRX Port
 *
 * Para : p_adp : handle of DPRX adapter
 *        type   : SDP type
 *        p_data : spd data output
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_sdp_data(
    RTK_DPRX_ADP*       p_dprx,
    DP_SDP_TYPE         type,
    DP_SDP_DATA_T*      p_sdp
    )
{
    DPRX_SDP_PKT_BUFFER* p_sdp_buff = NULL;

    if (p_sdp==NULL || !p_dprx->current_port || p_dprx->connected==0)
        return -1;

    memset(p_sdp, 0, sizeof(DP_SDP_DATA_T));

    switch(type)
    {
    case DP_SDP_TYPE_VSC: p_sdp_buff = &p_dprx->sdp_handler.vsc; break;
    case DP_SDP_TYPE_SPD: p_sdp_buff = &p_dprx->sdp_handler.spd; break;
    case DP_SDP_TYPE_DVS: p_sdp_buff = &p_dprx->sdp_handler.dvs; break;
    case DP_SDP_TYPE_VSC_EXT_VESA: p_sdp_buff = &p_dprx->sdp_handler.vsc_ext_vesa; break;
    case DP_SDP_TYPE_HDR:
        p_sdp_buff = &p_dprx->sdp_handler.hdr;

        if (p_dprx->override_eotf_en) // to support override eotf, override eotf
        {
            p_sdp_buff->pkt_count++;
            p_sdp_buff->time_stamp = dprx_odal_get_system_time_ms();

            // fake hb
            p_sdp_buff->pkt.hb[0] = 0x00;
            p_sdp_buff->pkt.hb[1] = 0x87;   // pkt header
            p_sdp_buff->pkt.hb[2] = 0x1D;
            p_sdp_buff->pkt.hb[3] = 0x00;

            // fake db
            p_sdp_buff->pkt.pb[0] = 0x01;   // version
            p_sdp_buff->pkt.pb[1] = 0x1A;   // info frame size
            p_sdp_buff->pkt.pb[2] = p_dprx->override_eotf;  // return fake eotf
        }
        break;

    default:
        return -1;
    }

    if (p_sdp_buff->pkt_count)
    {
        p_sdp->seq_num = p_sdp_buff->pkt_count;
        p_sdp->time_stamp = p_sdp_buff->time_stamp;
        memcpy(p_sdp->hb, p_sdp_buff->pkt.hb, 4);  // copy header bytes
        memcpy(p_sdp->pb, p_sdp_buff->pkt.pb, 32); // copy data bytes
        return 0;
    }

    return -1;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_pps_data
 *
 * Desc : get sdp data from a given DPRX Port
 *
 * Para : p_adp : handle of DPRX adapter
 *        p_data : spd data output
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_pps_data(
    RTK_DPRX_ADP*       p_dprx,
    DP_PPS_DATA_T*      p_pps_sdp
    )
{
    if (p_pps_sdp==NULL || !p_dprx->current_port || p_dprx->connected==0)
        return -1;

    memset(p_pps_sdp, 0, sizeof(DP_SDP_DATA_T));

#ifdef DPRX_DRV_SUPPORT_PPS_READ
    if (dprx_drv_read_current_pps(p_dprx->mac_idx, p_pps_sdp->pps, sizeof(p_pps_sdp->pps))==_TRUE)
        return 0;
#endif

    return -1;
}

#ifdef CONFIG_DPRX_STATUS_MONITOR

#define POLLING_INTERVAL        3000

extern UINT16 dprx_mac_get_video_crc(UINT8 port, UINT8 idx);

/*------------------------------------------------
 * Func : _dprx_status_monitor
 *
 * Desc : monitor dprx status
 *
 * Para : port : dprx port
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static void _dprx_status_monitor(RTK_DPRX_ADP* p_dprx)
{
    UINT8 vfsm;
    DPRX_HDCP_STATUS_T hdcp_status;

    if (p_dprx==NULL || !p_dprx->current_port ||
        p_dprx->connected==0)
        return ;

    if ((dprx_odal_get_system_time_ms() > p_dprx->status_monitor_time) &&
        (dprx_odal_get_system_time_ms() < p_dprx->status_monitor_time + POLLING_INTERVAL))
        return ;

    vfsm = dprx_mac_get_video_fsm(p_dprx->mac_idx);
    rtk_dprx_get_hdcp_status(p_dprx, &hdcp_status);

    //updata hdcp status
    if(hdcp_status.state != p_dprx->dprx_hdcp_status.state)
        p_dprx->dprx_hdcp_status = hdcp_status;

     // update vfsm status
    if(p_dprx->vfsm_status != vfsm)
    {
        p_dprx->pre_source_check_no_signal_time = dprx_odal_get_system_time_ms();
        p_dprx->vfsm_status = vfsm;
    }

    if (vfsm==VFSM_9_VIDEO_OUT_ENABLE || vfsm==VFSM_16_DSC_OUT_ENABLE)
    {
        DPRX_TIMING_INFO dprx_timing_info;

        memset(&dprx_timing_info, 0, sizeof(dprx_timing_info));

        dprx_drv_get_timing_info(p_dprx->mac_idx, &dprx_timing_info);

        if (dprx_timing_info.drr_mode==DP_DRR_MODE_AMD_FREE_SYNC)
        {
            if ((p_dprx->sdp_handler.freesync_support==0) ||
                (p_dprx->sdp_handler.freesync_enable == 0 && p_dprx->sdp_handler.freesync_activated==0))
                dprx_timing_info.drr_mode = DPRX_DRR_MODE_FRR;
        }

        DPRX_ADP_WARN("Status : Port=%d, vfsm=%d(%s), crc=(%04x/%04x/%04x), vfreq=%d.%d (Hz), DRR mode=%s, HDCP=%s, pixel_mode=%d\n",
            p_dprx->mac_idx, vfsm, _get_dprx_vfsm_str(vfsm),
            dprx_mac_get_video_crc(p_dprx->mac_idx, 0),
            dprx_mac_get_video_crc(p_dprx->mac_idx, 1),
            dprx_mac_get_video_crc(p_dprx->mac_idx, 2),
            dprx_timing_info.curr_vfreq_hz_x100/100,
            (dprx_timing_info.curr_vfreq_hz_x100 % 100),
            _to_dp_drr_mode_str(dprx_timing_info.drr_mode),
            _get_dprx_hdcp_state_str(p_dprx->dprx_hdcp_status.mode),
            _to_dp_pixel_mode(dprx_timing_info.pixel_mode));
    }
    else
    {
        DPRX_ADP_WARN("Status : vfsm=%d(%s)\n", vfsm, _get_dprx_vfsm_str(vfsm));
    }
    // update polling time
    p_dprx->status_monitor_time = dprx_odal_get_system_time_ms();
}

#endif


#ifdef CONFIG_DPRX_EMERGENCY_LIFECYCLE

/*------------------------------------------------
 * Func : _dprx_emergency_lifecycle_monitor
 *
 * Desc : monitor dprx emergency lifecycle
 *
 * Para : port : dprx port
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static void _dprx_emergency_lifecycle_monitor(RTK_DPRX_ADP* p_dprx)
{
#ifdef DPRX_EXT_HDCP_EMERGENCY_HANDLER
    #define DEFAULT_HDCP_EMERGENCY_TIMEOUT_MS                       1000
    if(dprx_drv_get_hdcp_emergency_flag(p_dprx->mac_idx))
    {
        UINT8 res = 0;
        res = _get_source_check_intenal(dprx_odal_get_system_time_ms(),
                                        dprx_drv_get_hdcp_emergency_start_time(p_dprx->mac_idx),
                                        DEFAULT_HDCP_EMERGENCY_TIMEOUT_MS);

        if(res == 0)
        {
            DPRX_ADP_WARN("DPRX_EMERGENCY_MONITOR: Clear HDCP Emergency Flag\n");
            dprx_drv_set_hdcp_emergency_flag(p_dprx->mac_idx, 0);
        }
    }
#endif

}

#endif


/*------------------------------------------------
 * Func : rtk_dprx_source_check
 *
 * Desc : Check input source
 *
 * Para : port : dprx port
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_source_check(
    RTK_DPRX_ADP*       p_dprx
    )
{
    if (p_dprx==NULL)
        return -1;

#ifdef DPRX_SINK_CAP_LINK_TRAIN_CTRL_FAKE_TP2_LT
    if (g_skip_source_detect)
        return -1;
#endif

    // check dprx port status
    if (!p_dprx->current_port || p_dprx->connected==0)
        return -1;

#ifdef CONFIG_DPRX_STATUS_MONITOR
    _dprx_status_monitor(p_dprx);
#endif

#ifdef CONFIG_DPRX_EMERGENCY_LIFECYCLE
    _dprx_emergency_lifecycle_monitor(p_dprx);
#endif

#ifdef CHECK_SOURCE_IN_GET_CONNECT_STATE
    p_dprx->source_check_time = dprx_odal_get_system_time_ms();
#endif

    // check input source
    if (dprx_drv_source_check(p_dprx->mac_idx)==_TRUE)
        return 0;

#ifdef CONFIG_DPRX_FORCE_CHECK_MODE_FAIL_CABLE_OFF_ON
    if (p_dprx->output_enable)
    {
        DPRX_ADP_WARN("check mode failed, !!!, stop video and restart fake cable off\n");
        p_dprx->fake_cable_off = 20;
    }
#endif

    return -1;
}

/*------------------------------------------------
 * Func : rtk_dprx_reset_signal_detect_flow
 *
 * Desc : reset signal detect flow of the given dprx port
 *
 * Para : p_dprx  : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_reset_signal_detect_flow(
    RTK_DPRX_ADP*       p_dprx
    )
{
#ifndef DPRX_EXT_RESET_SIG_DETECT_FLOW
    extern UINT8 g_dprx_detected_status;
#endif

    // check dprx port status
    if (p_dprx==NULL || !p_dprx->current_port || p_dprx->connected==0)
    {
        DPRX_ADP_WARN("rtk_dprx_reset_signal_detect_flow failed, not current port\n");
        return -1;
    }

#ifdef DPRX_EXT_RESET_SIG_DETECT_FLOW
    dprx_drv_set_ext_config(p_dprx->mac_idx, DPRX_EXT_RESET_SIG_DETECT_FLOW, 0);
#else
    g_dprx_detected_status = _FALSE;
#endif

    return 0;
}

/*------------------------------------------------
 * Func : rtk_dprx_handle_online_measure_error
 *
 * Desc : handle on line measure error
 *
 * Para : p_dprx  : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_handle_online_measure_error(
    RTK_DPRX_ADP*       p_dprx
    )
{
    // check dprx port status
    if (p_dprx==NULL || !p_dprx->current_port || p_dprx->connected==0)
    {
        DPRX_ADP_WARN("rtk_dprx_handle_online_measure_error failed, not current port\n");
        return -1;
    }

#ifdef CONFIG_DPRX_VFE_ENABLE_ONLINE_MEASURE_ERROR
    // stop video and restart timing check
    DPRX_ADP_WARN("rtk_dprx_handle_online_measure_error!!!, stop video and reset detect flow\n");

    if ((dprx_drv_read_vbid(p_dprx->mac_idx) & VBID_6_COMPRESSED_STREAM_FLAG))
    {    // force reset video state if DSC
        dprx_drv_do_avmute(p_dprx->mac_idx);
        rtk_dprx_reset_signal_detect_flow(p_dprx);
    }
#endif

    return 0;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_audio_status
 *
 * Desc : get dprx port audio status
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_audio_status : pointer of hdcp status
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_audio_status(
    RTK_DPRX_ADP*       p_dprx,
    DP_AUDIO_STATUS_T*  p_audio_status
    )
{
    if (p_dprx==NULL || p_audio_status==NULL)
        return -1;

    memset(p_audio_status, 0, sizeof(DP_AUDIO_STATUS_T));

    // check dprx port status
    if (!p_dprx->current_port || p_dprx->connected==0)
    {
        // force report no audio
        p_audio_status->coding_type = DP_AUDIO_CODING_NO_AUDIO;
        p_audio_status->ch_num = 0;
        p_audio_status->sampling_frequency_khz_x100 = 0;
        return 0;
    }

    // FIX ME: get audio status from Low level driver
    p_audio_status->coding_type = DP_AUDIO_CODING_LPCM;
    p_audio_status->ch_num = 5;
    p_audio_status->sampling_frequency_khz_x100 = 45000;
    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_hdcp_status
 *
 * Desc : get dprx port hdcp status
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_hdcp_status : pointer of hdcp status
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_hdcp_status(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_HDCP_STATUS_T* p_hdcp_status
    )
{
#ifdef DPRX_SUPPORT_READ_HDCP_STATUS
    unsigned char hdcp_mode;
#endif
    unsigned char rtx[8];
    unsigned char eks[16];

    if (p_dprx==NULL || p_hdcp_status==NULL)
        return -1;

    memset(p_hdcp_status, 0, sizeof(DPRX_HDCP_STATUS_T));

    // check dprx port status
    if (!p_dprx->current_port || p_dprx->connected==0)
    {
        p_hdcp_status->mode  = DP_HDCP_MODE_OFF;
        p_hdcp_status->state = DP_HDCP_STATE_IDEL;  // UNAUTH
        return 0;
    }

    // FIX ME: get hdcp status from Low level driver
    p_hdcp_status->mode  = DP_HDCP_MODE_OFF;
    p_hdcp_status->state = DP_HDCP_STATE_IDEL;  // UNAUTH

    rtk_dprx_get_dpcd(p_dprx, DPCD_68000_BKSV, p_hdcp_status->hdcp1x.bksv, HDCP1X_KSV_INFO_LEN);
    rtk_dprx_get_dpcd(p_dprx, DPCD_68007_AKSV_0, p_hdcp_status->hdcp1x.aksv, HDCP1X_KSV_INFO_LEN);
    rtk_dprx_get_dpcd(p_dprx, DPCD_6800C_AN_0, p_hdcp_status->hdcp1x.an, 8);
    rtk_dprx_get_dpcd(p_dprx, DPCD_68005_R0_PRIME, p_hdcp_status->hdcp1x.ri, 2);
    rtk_dprx_get_dpcd(p_dprx, DPCD_68028_BCAPS, &p_hdcp_status->hdcp1x.bcaps, 1);
    rtk_dprx_get_dpcd(p_dprx, DPCD_68029_BSTATUS, p_hdcp_status->hdcp1x.bstatus, 2);

    rtk_dprx_get_dpcd(p_dprx, DPCD_69000_RTX, rtx, 8);
    rtk_dprx_get_dpcd(p_dprx, DPCD_69318_E_DKEY_KS, eks, sizeof(eks));

#ifdef DPRX_SUPPORT_READ_HDCP_STATUS
    dprx_drv_read_hdcp_status(0, &hdcp_mode);
    if(hdcp_mode > 0)
    {
        if (p_hdcp_status->hdcp1x.aksv[0]!=0 ||
            p_hdcp_status->hdcp1x.aksv[1]!=0 ||
            p_hdcp_status->hdcp1x.aksv[2]!=0 ||
            p_hdcp_status->hdcp1x.aksv[3]!=0 ||
            p_hdcp_status->hdcp1x.aksv[4]!=0)
        {
            p_hdcp_status->mode  = DP_HDCP_MODE_HDCP_1P3;

            if (p_hdcp_status->hdcp1x.ri[0]==0 && p_hdcp_status->hdcp1x.ri[1]==0)
                p_hdcp_status->state  = DP_HDCP_STATE_AUTH_START;
            else
                p_hdcp_status->state  = DP_HDCP_STATE_AUTH_DONE;

            return 0;
        }
        else if (rtx[0]!=0 || rtx[1]!=0 ||rtx[2]!=0 ||rtx[3]!=0 || rtx[4]!=0 || rtx[5]!=0 ||rtx[6]!=0 ||rtx[7]!=0)
        {
            p_hdcp_status->mode  = DP_HDCP_MODE_HDCP_2P3;

            if (eks[0]!=0 || eks[1]!=0 ||eks[2]!=0 ||eks[3]!=0 || eks[4]!=0 || eks[5]!=0 ||eks[6]!=0 ||eks[7]!=0 ||
                eks[8]!=0 || eks[9]!=0 || eks[10]!=0 ||eks[11]!=0 ||eks[12]!=0 || eks[13]!=0 || eks[14]!=0 ||eks[15]!=0 )
                p_hdcp_status->state  = DP_HDCP_STATE_AUTH_DONE;
            else
                p_hdcp_status->state  = DP_HDCP_STATE_AUTH_START;
        }
    }
    else
    {
        p_hdcp_status->mode  = DP_HDCP_MODE_OFF;
        p_hdcp_status->state = DP_HDCP_STATE_IDEL;  // UNAUTH
    }
#else
    if (p_hdcp_status->hdcp1x.aksv[0]!=0 ||
        p_hdcp_status->hdcp1x.aksv[1]!=0 ||
        p_hdcp_status->hdcp1x.aksv[2]!=0 ||
        p_hdcp_status->hdcp1x.aksv[3]!=0 ||
        p_hdcp_status->hdcp1x.aksv[4]!=0)
    {
        p_hdcp_status->mode  = DP_HDCP_MODE_HDCP_1P3;
        if (p_hdcp_status->hdcp1x.ri[0]==0 && p_hdcp_status->hdcp1x.ri[1]==0)
            p_hdcp_status->state  = DP_HDCP_STATE_AUTH_START;
        else
            p_hdcp_status->state  = DP_HDCP_STATE_AUTH_DONE;
        return 0;
    }
    else if (rtx[0]!=0 || rtx[1]!=0 ||rtx[2]!=0 ||rtx[3]!=0 || rtx[4]!=0 || rtx[5]!=0 ||rtx[6]!=0 ||rtx[7]!=0)
    {
        p_hdcp_status->mode  = DP_HDCP_MODE_HDCP_2P3;
        if (eks[0]!=0 || eks[1]!=0 ||eks[2]!=0 ||eks[3]!=0 || eks[4]!=0 || eks[5]!=0 ||eks[6]!=0 ||eks[7]!=0 ||
            eks[8]!=0 || eks[9]!=0 || eks[10]!=0 ||eks[11]!=0 ||eks[12]!=0 || eks[13]!=0 || eks[14]!=0 ||eks[15]!=0 )
            p_hdcp_status->state  = DP_HDCP_STATE_AUTH_DONE;
        else
            p_hdcp_status->state  = DP_HDCP_STATE_AUTH_START;
    }
#endif

    return 0;
}


//===============================================================================
// DPCD related
//===============================================================================

/*------------------------------------------------
 * Func : rtk_dprx_set_dpcd
 *
 * Desc : set DPCD of the DPRX adapter
 *
 * Para : p_dprx  : handle of DPRX adapter
 *        address : dpcd address
 *        p_dpcd  : dpcd data
 *        len     : size of dpcd data
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_set_dpcd(
    RTK_DPRX_ADP*       p_dprx,
    unsigned long       address,
    unsigned char*      p_dpcd,
    unsigned short      len
    )
{
    if (p_dprx==NULL || p_dpcd==NULL || len==0 || address > 0xFFFFF)
        return -1;

    if (!p_dprx->current_port || p_dprx->connected==0)
    {
        DPRX_ADP_WARN("rtk_dprx_set_dpcd to a not current port, just ignore it\n");
        return 0;
    }

    while (len--)
    {
        lib_dprx_dpcd_write_manual_mode(p_dprx->mac_idx, address++, *p_dpcd++);
    }

    return 0;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_dpcd
 *
 * Desc : get DPCD of the DPRX adapter
 *
 * Para : p_dprx  : handle of DPRX adapter
 *        address : dpcd address
 *        p_dpcd  : dpcd data
 *        len     : size of dpcd data
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_dpcd(
    RTK_DPRX_ADP*       p_dprx,
    unsigned long       address,
    unsigned char*      p_dpcd,
    unsigned short      len
    )
{
    if (p_dprx==NULL || p_dpcd==NULL || len==0 || address > 0xFFFFF)
        return -1;

    memset(p_dpcd, 0, len);

    if (!p_dprx->current_port || p_dprx->connected==0)
    {
        DPRX_ADP_WARN("rtk_dprx_get_dpcd from a not current port, return zero to instead\n");
        return 0;
    }

    lib_dprx_dpcd_read_bytes(p_dprx->mac_idx, address, p_dpcd, len);

    return 0;
}

//===============================================================================
// Extension Control
//===============================================================================

/*------------------------------------------------
 * Func : rtk_dprx_set_ext_ctrl
 *
 * Desc : set ext control of a DPRX Port
 *
 * Para : p_dprx : handle of DPRX adapter
 *        ctrl : ext control command
 *
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_set_ext_ctrl(
    RTK_DPRX_ADP*   p_dprx,
    ADP_EXCTL_ID    ctrl,
    unsigned int    val
    )
{
    if (p_dprx==NULL)
        return -1;

    switch(ctrl)
    {
    case ADP_EXCTL_OVERRIDE_EOTF_EN:
        p_dprx->override_eotf_en = (val) ? 1 : 0;
        break;
    case ADP_EXCTL_OVERRIDE_EOTF:
        if (val > 0x7)
            return -1;
        p_dprx->override_eotf = val;
        break;
    // PHY
    case ADP_EXCTL_MANUAL_EQ_EN:
        p_dprx->manul_eq_en = (val) ? 1 : 0;
        break;
    // HDCP
    case ADP_EXCTL_DISABLE_HDCP14:
        p_dprx->hdcp_1x_disable_en = (val) ? 1 : 0;
        break;
    case ADP_EXCTL_DISABLE_HDCP22:
        p_dprx->hdcp_2x_disable_en = (val) ? 1 : 0;
        break;
    case ADP_EXCTL_FRORCE_REAUTH_HDCP22:
        p_dprx->hdcp_2x_force_reauth = (val) ? 1 : 0;
        break;
    case ADP_EXCTL_WATCH_DOG_ENABLE:
        DPRX_DRV_SET_VIDEO_WATCH_DOG(p_dprx->mac_idx, (val) ? 1:0);
        break;
    case ADP_EXCTL_VIDEO_COMMON_SEL:
#ifdef DPRX_EXT_SET_VIDEO_COMMON
        switch (val)
        {
        case DP_VIDEO_COMMON_SEL_NONE:
            dprx_drv_set_ext_config(p_dprx->mac_idx, DPRX_EXT_SET_VIDEO_COMMON, DPRX_EXT_VIDEO_COMMON_NONE);
            break;
        case DP_VIDEO_COMMON_SEL_1:
            dprx_drv_set_ext_config(p_dprx->mac_idx, DPRX_EXT_SET_VIDEO_COMMON, DPRX_EXT_VIDEO_COMMON_1);
            break;
        case DP_VIDEO_COMMON_SEL_2:
            dprx_drv_set_ext_config(p_dprx->mac_idx, DPRX_EXT_SET_VIDEO_COMMON, DPRX_EXT_VIDEO_COMMON_2);
            break;
        case DP_VIDEO_COMMON_SEL_3:
            dprx_drv_set_ext_config(p_dprx->mac_idx, DPRX_EXT_SET_VIDEO_COMMON, DPRX_EXT_VIDEO_COMMON_3);
            break;
        case DP_VIDEO_COMMON_SEL_4:
            dprx_drv_set_ext_config(p_dprx->mac_idx, DPRX_EXT_SET_VIDEO_COMMON, DPRX_EXT_VIDEO_COMMON_4);
            break;
        default:
            break;
        }
#else
        DPRX_ADP_WARN("setup video common is not supported by low level driver, do nothing\n");
#endif // DPRX_EXT_SET_VIDEO_COMMON
        break;
    case ADP_EXCTL_VRR_EN:
        p_dprx->sink_cap.freesync_support = (val) ? 1 : 0;
        p_dprx->sink_cap.adaptive_sync_support = (val) ? 1 : 0;
        p_dprx->sink_capabliity_update = 1;
        DPRX_ADP_WARN("Set ADP_EXCTL_VRR_EN = %d\n", val);
        break;
    case ADP_EXCTL_WAKEUP_MODE:
        p_dprx->dprx_aux_wakeup_mode = val;
        if (p_dprx->type == DP_TYPE_USB_TYPE_C)
        {
            switch(val)
            {
                case 0:
                    DPRX_DRV_SER_POWER_CTRL_ON_WAKEUP_MODE(0, DPRX_DRV_TYPEC_DPMS_OFF);
                    break;
                case 1:
                    DPRX_DRV_SER_POWER_CTRL_ON_WAKEUP_MODE(0, DPRX_DRV_TYPEC_Signal);
                    break;
                case 2:
                    DPRX_DRV_SER_POWER_CTRL_ON_WAKEUP_MODE(0, DPRX_DRV_TYPEC_AUX);
                    break;
                case 3:
                    DPRX_DRV_SER_POWER_CTRL_ON_WAKEUP_MODE(0, DPRX_DRV_TYPEC_Singal_AUX);
                    break;
            }
        }
        else if (p_dprx->type == DP_TYPE_DP)
        {
            switch(val)
            {
                case 0:
                    DPRX_DRV_SER_POWER_CTRL_ON_WAKEUP_MODE(0, DPRX_DRV_DP_DPMS_OFF);
                    break;
                case 1:
                    DPRX_DRV_SER_POWER_CTRL_ON_WAKEUP_MODE(0, DPRX_DRV_DP_Signal);
                    break;
                case 2:
                    DPRX_DRV_SER_POWER_CTRL_ON_WAKEUP_MODE(0, DPRX_DRV_DP_AUX);
                    break;
                case 3:
                    DPRX_DRV_SER_POWER_CTRL_ON_WAKEUP_MODE(0, DPRX_DRV_DP_Singal_AUX);
                    break;
            }
        }
        break;
    case ADP_EXCTL_HDCP1x_KEY_INIT:
        DPRX_DRV_EXT_HDCP1x_KEY_INIT(p_dprx->mac_idx);
        break;
    case ADP_EXCTL_HDCP2x_KEY_INIT:
        DPRX_DRV_EXT_HDCP2x_KEY_INIT(p_dprx->mac_idx);
        break;
    // Stream Bypass
    case ADP_EXCTL_STREAM_BYPASS_ENABLE:
        dprx_video_common_set_bypass_port(p_dprx->p_vc, (val) ? 1 : 0);  // enable/disable stream bypass
        break;
    case ADP_EXCTL_STREAM_BYPASS_MODE:

#ifdef DPRX_EXT_STREAM_BYPASS_MODE
        switch(val)
        {
        case DPRX_STREAM_BYPASS_MODE_TO_HDMITX:
            dprx_drv_set_ext_config(p_dprx->mac_idx, DPRX_EXT_STREAM_BYPASS_MODE, DPRX_EXT_STREAM_BYPASS_TO_HDMITX);
            break;
        default:
        case DPRX_STREAM_BYPASS_MODE_OFF:
            dprx_drv_set_ext_config(p_dprx->mac_idx, DPRX_EXT_STREAM_BYPASS_MODE, DPRX_EXT_STREAM_BYPASS_OFF);
            break;
        }
#endif // DPRX_EXT_STREAM_BYPASS_MODE
        break;
    default:
        return -1;
    }
    return 0;
}

/*------------------------------------------------
 * Func : rtk_dprx_set_ext_ctrl
 *
 * Desc : set ext control of a DPRX Port
 *
 * Para : p_dprx : handle of DPRX adapter
 *        ctrl : ext control command
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_get_ext_ctrl(
    RTK_DPRX_ADP*   p_dprx,
    ADP_EXCTL_ID    ctrl
    )
{
    if (p_dprx==NULL)
        return -1;

    switch(ctrl)
    {
    // HDR
    case ADP_EXCTL_OVERRIDE_EOTF_EN:     return p_dprx->override_eotf_en;        break;
    case ADP_EXCTL_OVERRIDE_EOTF:        return p_dprx->override_eotf;           break;
    // PHY
    case ADP_EXCTL_MANUAL_EQ_EN:         return p_dprx->manul_eq_en;             break;
    // HDCP
    case ADP_EXCTL_DISABLE_HDCP14:       return p_dprx->hdcp_1x_disable_en;      break;
    case ADP_EXCTL_DISABLE_HDCP22:       return p_dprx->hdcp_2x_disable_en;      break;
    case ADP_EXCTL_FRORCE_REAUTH_HDCP22: return p_dprx->hdcp_2x_force_reauth;    break;
    // Stream Bypass
    case ADP_EXCTL_STREAM_BYPASS_MODE:
        {
#ifdef DPRX_EXT_STREAM_BYPASS_MODE

            UINT8 mode = DPRX_EXT_STREAM_BYPASS_OFF;

            dprx_drv_get_ext_config(p_dprx->mac_idx, DPRX_EXT_STREAM_BYPASS_MODE, &mode);

            switch(mode)
            {
            case DPRX_EXT_STREAM_BYPASS_TO_HDMITX:
                return DPRX_STREAM_BYPASS_MODE_TO_HDMITX;
                break;

            default:
                break;
            }
#endif
            return DPRX_STREAM_BYPASS_MODE_OFF;
        }
    default:
        return -1;
    }

    return 0;
}

//----------------------------------------------------------------------------------------
// Power Management API
//----------------------------------------------------------------------------------------

/*------------------------------------------------
 * Func : rtk_dprx_suspend
 *
 * Desc : suspend a rtk dprx adapter
 *
 * Para : p_dprx  : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_suspend(
    RTK_DPRX_ADP*       p_dprx,
    UINT32              mode
    )
{
    if (p_dprx==NULL)
    {
        DPRX_ADP_WARN("suspend failed, invalid argments!!!!\n");
        return -1;
    }

    DPRX_ADP_INFO("suspend, p_dprx->suspend=%d, mode=%d!!!!\n", p_dprx->suspend, mode);

   #ifdef DPRX_EXT_WAKEUP_BY_DATA_ENABLE
    dprx_drv_set_ext_config(p_dprx->mac_idx, DPRX_EXT_WAKEUP_BY_DATA_ENABLE, 1); // enable wakeup by data
    #endif
    if (p_dprx->suspend==0)
    {
        p_dprx->suspend = 1; // setup suspend flag
        p_dprx->resume_connect = 0;

        // 2. power off DPRX if it is current port
        if (p_dprx->current_port)
        {
            if ((mode & WAKEUP_BY_AUX))
                newbase_dprx_dpcd_set_set_power_state(p_dprx->mac_idx, 0x5);  // change set the power state to D3
            else
                rtk_dprx_port_stop(p_dprx);     // deactiavte currect port

            p_dprx->resume_connect = 1;   // auto connect after resume
        }

        if ((mode & WAKEUP_BY_AUX))
            emcu_set_dp_aux(1);
        else
            emcu_set_dp_aux(0);

        // 2. put hpd to low (optional)
        if ((mode & WAKEUP_BY_AUX)==0)
            dprx_hpd_set_hpd(p_dprx->p_hpd, 0);   // power down hpd
    }

    p_dprx->edid_resume = 0;

    return 0;
}


/*------------------------------------------------
 * Func : rtk_dprx_resume
 *
 * Desc : resume a rtk dprx adapter
 *
 * Para : p_dprx  : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static int rtk_dprx_resume(
    RTK_DPRX_ADP*       p_dprx
    )
{
    if (p_dprx==NULL)
    {
        DPRX_ADP_WARN("resume failed, invalid argments!!!!\n");
        return -1;
    }

    DPRX_ADP_INFO("resume, p_dprx->suspend=%d, resume_connect=%d\n", p_dprx->suspend, p_dprx->resume_connect);

    // resume edid
    if (p_dprx->p_edid && p_dprx->edid_resume==0)
    {
        if (dprx_edid_init(g_p_dprx_edid) <0)
        {
            DPRX_ADP_INFO("rtk_dprx_ddc_init failed, init dprx ddc (%d) failed\n", DPRX_AUX_DDC);
        }
        dprx_edid_reset(p_dprx->p_edid);  // force reset edid
        p_dprx->edid_resume = 1;
    }

    if (p_dprx->suspend)
    {
        // clear suspend flag
        p_dprx->suspend = 0;

#ifdef FORCE_RE_ALT_MODE
        if(p_dprx->type == DP_TYPE_USB_TYPE_C)
            dprx_hpd_set_typec_re_alt_mode(p_dprx->p_hpd);
#endif

#ifdef CONFIG_DPRX_ENABLE_TASK_EVENT_HANDLER
        // reinit dprx mac
        if (p_dprx->resume_connect)
        {
            dprx_drv_init_dprx_port(p_dprx->mac_idx, &p_dprx->sink_cap, &p_dprx->port_cfg);
            p_dprx->resume_connect_handler_event = 1;
            p_dprx->current_port = 1;
            g_resume_system_time_ms = dprx_odal_get_system_time_ms();
            p_dprx->connected = 1;
        }
#else
        // reinit dprx mac
        if (p_dprx->resume_connect)
        {
            rtk_dprx_port_reset(p_dprx);
        }
#endif
        else
        {
            DPRX_ADP_INFO("not current port, do nothing!!!!\n");
        }

        rtk_dprx_plat_interrupt_enable();  // re-enable interrupt
    }

    return 0;
}


//----------------------------------------------------------------------------------------
// Event Handler API
//----------------------------------------------------------------------------------------



/*------------------------------------------------
 * Func : _update_sdp_debug_flag
 *
 * Desc : update sdp debug flag.
 *
 * Para :
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static void _update_sdp_debug_flag(RTK_DPRX_ADP* p_dprx)
{
    p_dprx->sdp_handler.dvs.debug_en = (g_dprx_sdp_debug_ctrl & DEBUG_DOLBY_VIOSN_SDP) ? 1 : 0;
    p_dprx->sdp_handler.spd.debug_en = (g_dprx_sdp_debug_ctrl & DEBUG_AMD_FREESYNC_SPD_SDP) ? 1 : 0;
    p_dprx->sdp_handler.vsc_ext_vesa.debug_en = (g_dprx_sdp_debug_ctrl & DEBUG_VSC_EXT_VESA_SDP) ? 1 : 0;
}

/*------------------------------------------------
 * Func : rtk_dprx_handle_infoframe_sdp_update_event
 *
 * Desc : get sdp data from a given DPRX Port
 *
 * Para : p_adp : handle of DPRX adapter
 *        sdp_type : sdp type
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static void rtk_dprx_handle_infoframe_sdp_update_event(
    RTK_DPRX_ADP*       p_dprx,
    SDP_TYPE_E          sdp_type
    )
{
    DPRX_SDP_PKT_BUFFER* p_sdp_buff = NULL;
    DPRX_SDP_PKT sdp_pkt;

    if (p_dprx==NULL || !p_dprx->current_port || p_dprx->connected==0)
        return ;

    dprx_drv_read_sdp_packet(p_dprx->mac_idx, sdp_type, &sdp_pkt);

    switch(sdp_type)
    {
    case SDP_TYPE_INFOFRAME_VENDOR_SPEC:
        if (sdp_pkt.pb[0]==0x1 && sdp_pkt.pb[1]==0x1B && sdp_pkt.pb[2]==0x46 && sdp_pkt.pb[3]==0xD0 && sdp_pkt.pb[4]==0x00)
            p_sdp_buff = &p_dprx->sdp_handler.dvs;
        else
            p_sdp_buff = &p_dprx->sdp_handler.vsi;
        break;

    case SDP_TYPE_INFOFRAME_HDR: p_sdp_buff = &p_dprx->sdp_handler.hdr; break;
    case SDP_TYPE_VSC:           p_sdp_buff = &p_dprx->sdp_handler.vsc; break;
    case SDP_TYPE_INFOFRAME_SPD: p_sdp_buff = &p_dprx->sdp_handler.spd; break;
    case SDP_TYPE_ADAPTIVE_SYNC_SDP: p_sdp_buff = &p_dprx->sdp_handler.adp_sync; break;
    case SDP_TYPE_VSC_EXT_VESA:  p_sdp_buff = &p_dprx->sdp_handler.vsc_ext_vesa; break;
    default:
        break;
    }

    if (p_sdp_buff==NULL)
        return ;  // ignrore

    p_sdp_buff->time_stamp = dprx_odal_get_system_time_ms();
    _update_sdp_debug_flag(p_dprx);

    if (p_sdp_buff->pkt_count<0xFFFFFFFF)
        p_sdp_buff->pkt_count++;          // increase packet count

    // restore pkt buffer
    memcpy(&p_sdp_buff->pkt, &sdp_pkt, sizeof(sdp_pkt));

    if (p_sdp_buff->debug_en)
    {
        DPRX_ADP_INFO("sdp_update (type=%02x, %s, %d) HB=%02x %02x %02x %02x, PB=%02x %02x %02x %02x, %02x %02x %02x %02x, %02x %02x %02x %02x, %02x %02x %02x %02x, %02x %02x %02x %02x, %02x %02x %02x %02x, %02x %02x %02x %02x, %02x %02x %02x %02x\n",
                sdp_type,
                sdp_type_str(sdp_type),
                p_sdp_buff->pkt_count,
                sdp_pkt.hb[0],  sdp_pkt.hb[1], sdp_pkt.hb[2], sdp_pkt.hb[3],
                sdp_pkt.pb[0],  sdp_pkt.pb[1], sdp_pkt.pb[2], sdp_pkt.pb[3],
                sdp_pkt.pb[4],  sdp_pkt.pb[5], sdp_pkt.pb[6], sdp_pkt.pb[7],
                sdp_pkt.pb[8],  sdp_pkt.pb[9], sdp_pkt.pb[10], sdp_pkt.pb[11],
                sdp_pkt.pb[12], sdp_pkt.pb[13], sdp_pkt.pb[14], sdp_pkt.pb[15],
                sdp_pkt.pb[16], sdp_pkt.pb[17], sdp_pkt.pb[18], sdp_pkt.pb[19],
                sdp_pkt.pb[20], sdp_pkt.pb[21], sdp_pkt.pb[22], sdp_pkt.pb[23],
                sdp_pkt.pb[24], sdp_pkt.pb[25], sdp_pkt.pb[26], sdp_pkt.pb[27],
                sdp_pkt.pb[28], sdp_pkt.pb[29], sdp_pkt.pb[30], sdp_pkt.pb[31]);
    }

#ifdef CONFIG_DPRX_EANBLE_FREESYNC
    if (sdp_type==SDP_TYPE_INFOFRAME_SPD)
    {
  #ifdef DPRX_AMD_FREESYNC_STATUS
        // New API
        DPRX_AMD_FREESYNC_STATUS status;

        dprx_drv_get_amd_freesync_status(p_dprx->mac_idx, &status);

        if (p_dprx->sdp_handler.freesync_enable != status.freesync_en||
            p_dprx->sdp_handler.freesync_activated != status.freesync_act ||
            p_dprx->sdp_handler.freesync_support != status.freesync_support)
        {
            p_dprx->sdp_handler.freesync_enable    = status.freesync_en;
            p_dprx->sdp_handler.freesync_activated = status.freesync_act;
            p_dprx->sdp_handler.freesync_support   = status.freesync_support;

            DPRX_ADP_INFO("AMD SDP updated (fs_en=%d, fs_act=%d, fs_support=%d, pkt_count=%d)\n",
                status.freesync_en, status.freesync_act, status.freesync_support, p_sdp_buff->pkt_count);
        }
  #else
        // Leagacy API
        UINT8 freesync_en = newbase_dprx_sdp_get_amd_spd_info(p_dprx->mac_idx, _SPD_INFO_FREESYNC_ENABLE) ? 1 : 0;
        UINT8 freesync_act = newbase_dprx_sdp_get_amd_spd_info(p_dprx->mac_idx, _SPD_INFO_FREESYNC_ENABLE) ? 1 : 0;
        UINT8 freesync_support = newbase_dprx_sdp_get_amd_spd_info(p_dprx->mac_idx, _SPD_INFO_FREESYNC_ENABLE) ? 1 : 0;

        if (p_dprx->sdp_handler.freesync_enable != freesync_en||
            p_dprx->sdp_handler.freesync_activated != freesync_act ||
            p_dprx->sdp_handler.freesync_support != freesync_support)
        {
            p_dprx->sdp_handler.freesync_enable    = freesync_en;
            p_dprx->sdp_handler.freesync_activated = freesync_act;
            p_dprx->sdp_handler.freesync_support   = freesync_support;

            DPRX_ADP_INFO("AMD SDP updated (fs_en=%d, fs_act=%d, fs_support=%d, pkt_count=%d)\n",
                freesync_en, freesync_act, freesync_support, p_sdp_buff->pkt_count);
        }
  #endif
    }
#endif
}

#ifdef ENABLE_DPRX_LT_EVENT_MONITOR

/*------------------------------------------------
 * Func : rtk_dprx_handle_lt_state_update_event
 *
 * Desc : handle lt state update event
 *
 * Para : p_adp : handle of DPRX adapter
 *        state : lt state type
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static void rtk_dprx_handle_lt_state_update_event(
    RTK_DPRX_ADP*       p_dprx,
    UINT8               state
    )
{
    if (p_dprx==NULL || !p_dprx->current_port || p_dprx->connected==0)
        return ;

    if (p_dprx->link_state != state)
    {
        if (state==_DP_NORMAL_TRAINING_PATTERN_1_RUNNING ||
            state==_DP_FAKE_TRAINING_PATTERN_1_RUNNING)
            p_dprx->link_round++;

        p_dprx->link_state = state;
        p_dprx->link_sub_state = 0;
    }
    else
    {
        p_dprx->link_sub_state++;
    }

    if (rtk_dprx_lt_event_message_enable(p_dprx))
    {
        DPRX_ADP_INFO("lt_state (%d-%02x-%d) lane=%d, bw=%02x (sink status : %02x-%02x-%02x)!!!\n",
            p_dprx->link_round,
            p_dprx->link_state,
            p_dprx->link_sub_state,
            newbase_dprx_dpcd_get_lane_count(p_dprx->mac_idx),
            newbase_dprx_dpcd_get_link_bw_set(p_dprx->mac_idx),
            newbase_dprx_dpcd_get_lane_01_status(p_dprx->mac_idx),
            newbase_dprx_dpcd_get_lane_23_status(p_dprx->mac_idx),
            newbase_dprx_dpcd_get_lane_align_status(p_dprx->mac_idx));
    }
}

#endif

//----------------------------------------------------------------------------------------
// DPRX TASK HANDLER
//----------------------------------------------------------------------------------------
#ifdef CONFIG_DPRX_ENABLE_TASK_EVENT_HANDLER
/*------------------------------------------------
 * Func : _dprx_hdcp2x_handler
 *
 * Desc : when hdcp2.x handshack is failed, source switch hdcp
 *  version to v1.x. But dp driver has been disconnect, driver
 *  don't detect hdcp version which it's changed.
 *  For this case, we create _dprx_hdcp2x_handler to process
 *  hdcp handshake
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static void _dprx_hdcp2x_handler(RTK_DPRX_ADP* p_dprx)
{
    // HDCP detect
    if(p_dprx->hdcp2x_event_rp != p_dprx->hdcp2x_event_wp)
    {
        DPRX_ADP_INFO("[_dprx_hdcp2x_handler] = %d\n", p_dprx->mac_idx);
        DPRX_DRV_HDCP2x_HEADLER(p_dprx->mac_idx, p_dprx->hdcp2x_event_msg_id[p_dprx->hdcp2x_event_rp++]);
        if(p_dprx->hdcp2x_event_rp >= DPRX_MAX_HDCP_EVENT_QUEUE)
            p_dprx->hdcp2x_event_rp = 0;
    }
}

/*------------------------------------------------
 * Func : _dprx_resume_handler
 *
 * Desc : Since NV4060 Link Training time is more stringent.
 * In the status of resume flow. it's a lot of task running,
 * so DPRX can't reply lt_status to DPTX.
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static void _dprx_resume_handler(RTK_DPRX_ADP* p_dprx)
{
    UINT8 res = 0;

    if (p_dprx->connected==0)
        return;

    if(p_dprx->resume_connect_handler_event)
    {
        res = _get_source_check_intenal(dprx_odal_get_system_time_ms(),
                                        g_resume_system_time_ms,
                                        g_resume_guard_time_interval);
        if(res == 0)
        {
            DPRX_ADP_INFO("[resume hander]current port, do port swap to reconnect!!!!\n");
            rtk_dprx_port_reset(p_dprx);   // change dprx port
            p_dprx->connected = 1;
            p_dprx->resume_connect_handler_event = 0;
        }
    }
}
/*------------------------------------------------
 * Func : rtk_dprx_event_task_handler
 *
 * Desc : handle dprx event task
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
static void rtk_dprx_port_event_task_handler(RTK_DPRX_ADP* p_dprx)
{
    //Need CHANGE without port @
    if(p_dprx == NULL)
        return;

    _dprx_resume_handler(p_dprx);
    _dprx_hdcp2x_handler(p_dprx);
}
#endif

/*------------------------------------------------
 * Func : rtk_dprx_set_aux_int_crtl_rounting_enable
 *
 * Desc : AUX INT CTRL
 *
 * Para : enable : 0/1
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_dprx_port_set_aux_int_crtl_rounting_enable(
    RTK_DPRX_ADP*       p_dprx,
    UINT8               enable
    )
{
    if ((!p_dprx))
    {
        DPRX_DRV_SET_AUX_INT_ROUNTING_ENABLE(0, enable);
    }else
    {
        DPRX_DRV_SET_AUX_INT_ROUNTING_ENABLE(p_dprx->mac_idx, enable);
    }
}


dprx_priv_ops_t g_rtk_priv2_ops =
{
    .reset                              = rtk_dprx_port_reset,
    // .port_swap                          = rtk_dprx_port_swap, v2 will not use
    .init                               = rtk_dprx_init,
    .uninit                             = rtk_dprx_uninit,
    .open                               = rtk_dprx_open,
    .close                              = rtk_dprx_close,
    .connect                            = rtk_dprx_connect,
    .disconnect                         = rtk_dprx_disconnect,


    .source_check                       = rtk_dprx_source_check,
    .get_capability                     = rtk_dprx_get_capability,
    .get_link_status                    = rtk_dprx_get_link_status,


    .set_hpd                            = rtk_dprx_set_hpd,
    .set_hpd_toggle                     = rtk_dprx_set_hpd_toggle,
    .set_irq_hpd                        = rtk_dprx_set_irq_hpd,
    .get_hpd                            = rtk_dprx_get_hpd,
    .get_typec_pd_ready_status          = rtk_dprx_get_typec_pd_ready_status,
    .get_connect_status                 = rtk_dprx_get_connect_status,
    .get_aux_status                     = rtk_dprx_get_aux_status,

    .set_edid                           = rtk_dprx_set_edid,
    .get_edid                           = rtk_dprx_get_edid,

    .set_dpcd                           = rtk_dprx_set_dpcd,
    .get_dpcd                           = rtk_dprx_get_dpcd,

    .get_vbid_data                      = rtk_dprx_get_vbid_data,
    .get_msa_data                       = rtk_dprx_get_msa_data,
    .get_sdp_data                       = rtk_dprx_get_sdp_data,
    .get_pps_data                       = rtk_dprx_get_pps_data,

    .get_stream_type                    = rtk_dprx_get_stream_type,
    .get_video_timing                   = rtk_dprx_get_video_timing,
    .get_pixel_encoding_info            = rtk_dprx_get_pixel_encoding_info,
    .get_stream_bypasss_timing          = rtk_dprx_get_stream_bypasss_timing,
    .get_drm_info                       = rtk_dprx_get_drm_info,
    .get_avi_info                       = rtk_dprx_get_avi_info,

    .get_audio_status                   = rtk_dprx_get_audio_status,
    .get_hdcp_status                    = rtk_dprx_get_hdcp_status,

    .suspend                            = rtk_dprx_suspend,
    .resume                             = rtk_dprx_resume,

    .reset_signal_detect_flow           = rtk_dprx_reset_signal_detect_flow,
    .handle_online_measure_error        = rtk_dprx_handle_online_measure_error,
    .handle_infoframe_sdp_update_event  = rtk_dprx_handle_infoframe_sdp_update_event,


    .set_ext_ctrl                       = rtk_dprx_set_ext_ctrl,
    .get_ext_ctrl                       = rtk_dprx_get_ext_ctrl,
    .port_set_aux_int_crtl_rounting_enable  = rtk_dprx_port_set_aux_int_crtl_rounting_enable,
    //.set_aux_int_crtl_rounting_enable  = rtk_dprx_port_set_aux_int_crtl_rounting_enable, //v2 will not use

#ifdef ENABLE_DPRX_LT_EVENT_MONITOR
    .handle_lt_state_update_event  = rtk_dprx_handle_lt_state_update_event,
#endif

#ifdef CONFIG_DPRX_ENABLE_TASK_EVENT_HANDLER
    .port_event_task_handler            = rtk_dprx_port_event_task_handler,
    //.event_task_handler                 = null, // v2 will not use
#endif
#ifdef CONFIG_SUPPORT_DOLBY_VISION_DP
    .set_dolby_vision_check_mode_function = rtk_dprx_set_dolby_vision_check_mode_function,
#endif
};
