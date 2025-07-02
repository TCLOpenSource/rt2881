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
#include <dprx_adapter-rtk_dprx-priv.h>
#include <dprx_adapter-rtk_dprx-plat.h>

extern RTK_DPRX_ADP* g_p_current_dprx_port;
extern const char* _get_dprx_vfsm_str(UINT8 vfsm);
#ifdef CONFIG_DPRX_EANBLE_FREESYNC
extern UINT8 g_force_freesync_off;
#ifdef CONFIG_DPRX_ENABLE_MULTIPORT
#define get_force_freesync_off(p_dprx)                ((p_dprx) ? p_dprx->debug_ctrl.force_freesync_off : 0)
#define set_force_freesync_off(p_dprx, val)           p_dprx->debug_ctrl.force_freesync_off = val
#else
#define get_force_freesync_off(p_dprx)                g_force_freesync_off
#define set_force_freesync_off(p_dprx, val)           g_force_freesync_off = val
#endif

#endif
extern UINT8 g_force_max_lane_count;
extern UINT8 g_force_max_link_rate;
extern UINT8 g_force_fake_tps1_lt;  // disabled
extern UINT8 g_force_fake_tps2_lt;  // disabled
extern UINT8 g_force_timing_ext_flag_en;  // for test
extern UINT8 g_force_timing_ext_flag;     // for test
extern UINT8 g_skip_source_detect;      // disable detect flow to for debug
extern UINT8 g_force_demode;
extern DPRX_SDP_HANDLER g_dprx_sdp_handler;
extern UINT8 g_mute_if_lane_dealign;
#ifdef CONFIG_DPRX_ENABLE_TASK_EVENT_HANDLER
extern UINT16 g_resume_guard_time_interval;
#endif
#ifdef CONFIG_DPRX_ADP_ENABLE_HPD_TOGGLE_TEST
unsigned int hpd_toggle_delay_ms = 0;
char hpd_device_name[20] = {""};
#endif

#ifdef CONFIG_DPRX_ENABLE_MULTIPORT
#define get_force_max_lane_count(p_dprx)                ((p_dprx) ? p_dprx->debug_ctrl.force_max_lane_count : 0)
#define set_force_max_lane_count(p_dprx, val)           p_dprx->debug_ctrl.force_max_lane_count = val
#define get_force_max_link_rate(p_dprx)                 ((p_dprx) ? p_dprx->debug_ctrl.force_max_link_rate : 0)
#define set_force_max_link_rate(p_dprx, val)            p_dprx->debug_ctrl.force_max_link_rate = val
#define get_force_fake_tps1_lt(p_dprx)                  ((p_dprx) ? p_dprx->debug_ctrl.force_fake_tps1_lt : 0)
#define set_force_fake_tps1_lt(p_dprx, val)             p_dprx->debug_ctrl.force_fake_tps1_lt = val
#define get_force_fake_tps2_lt(p_dprx)                  ((p_dprx) ? p_dprx->debug_ctrl.force_fake_tps2_lt : 0)
#define set_force_fake_tps2_lt(p_dprx, val)             p_dprx->debug_ctrl.force_fake_tps2_lt = val
#define get_skip_source_detect(p_dprx)                  ((p_dprx) ? p_dprx->debug_ctrl.skip_source_detect : 0)
#define set_skip_source_detect(p_dprx, val)             p_dprx->debug_ctrl.skip_source_detect = val
#define get_force_demode(p_dprx)                        ((p_dprx) ? p_dprx->debug_ctrl.force_demode : 0)
#define set_force_demode(p_dprx, val)                   p_dprx->debug_ctrl.force_demode = val
#define get_mute_if_lane_dealign(p_dprx)                ((p_dprx) ? p_dprx->debug_ctrl.mute_if_lane_dealign : 0)
#define set_mute_if_lane_dealign(p_dprx, val)           p_dprx->debug_ctrl.mute_if_lane_dealign = val
#else
#define get_force_max_lane_count(p_dprx)                g_force_max_lane_count
#define set_force_max_lane_count(p_dprx, val)           g_force_max_lane_count = val
#define get_force_max_link_rate(p_dprx)                 g_force_max_link_rate
#define set_force_max_link_rate(p_dprx, val)            g_force_max_link_rate = val
#define get_force_fake_tps1_lt(p_dprx)                  g_force_fake_tps1_lt
#define set_force_fake_tps1_lt(p_dprx, val)             g_force_fake_tps1_lt = val
#define get_force_fake_tps2_lt(p_dprx)                  g_force_fake_tps2_lt
#define set_force_fake_tps2_lt(p_dprx, val)             g_force_fake_tps2_lt = val
#define get_skip_source_detect(p_dprx)                  g_skip_source_detect
#define set_skip_source_detect(p_dprx, val)             g_skip_source_detect = val
#define get_force_demode(p_dprx)                        g_force_demode
#define set_force_demode(p_dprx, val)                   g_force_demode = val
#define get_mute_if_lane_dealign(p_dprx)                g_mute_if_lane_dealign
#define set_mute_if_lane_dealign(p_dprx, val)           g_mute_if_lane_dealign = val
#endif


bool IS_CURRENT_PORT(RTK_DPRX_ADP*   p_dprx)
{
#ifndef CONFIG_DPRX_ENABLE_MULTIPORT
    return (p_dprx == g_p_current_dprx_port) ? 1 : 0;
#else
    if(p_dprx)
        return p_dprx->current_port ? 1 : 0;
    else
        return 0;
#endif
}

const char* _rtk_dprx_get_type_str(DP_TYPE type)
{
    switch(type)
    {
    case DP_TYPE_NONE:  return "None";
    case DP_TYPE_DP:    return "DP";
    case DP_TYPE_mDP:   return "mDP";
    case DP_TYPE_USB_TYPE_C:    return "TypeC";
    case DP_TYPE_PSEUDO_HDMI_DP:    return "HDMI";
    case DP_TYPE_PSEUDO_DP:     return "Pseudo";
    default:            return "Unknown";
    }
}

/*------------------------------------------------
 * Func : rtk_dprx_attr_show_status
 *
 * Desc : show adapter status
 *
 * Para : p_dprx : handle of DPRX adapter
 *        p_buf  : data buffer
 *        cnt    : buffer size
 *
 * Retn : number if bytes handled
 *-----------------------------------------------*/
int rtk_dprx_attr_show_status(
    RTK_DPRX_ADP*   p_dprx,
    UINT8*          p_buf,
    UINT32          count
    )
{
    UINT32 n;
    UINT8 *ptr = p_buf;

    //---------------------------------------
    // USB ALT
    //---------------------------------------
    if (p_dprx->type == DP_TYPE_USB_TYPE_C && p_dprx->p_hpd)
    {
        DPRX_CABLE_CFG cable_cfg;
        dprx_hpd_get_cable_config(p_dprx->p_hpd, &cable_cfg);

        n = snprintf(ptr, count, "TYPEC : DP alt mode=%d, cable_flip=%d, config=%x\n",
                dprx_hpd_get_aux_status(p_dprx->p_hpd, NULL),
                (cable_cfg & 0x80) ? 1 : 0,
                (cable_cfg & 0xF));
        ptr+=n; count-=n;
    }

    if (IS_CURRENT_PORT(p_dprx) || p_dprx->connected)
    {
#ifdef VFSM_0_WAIT_LINK_READY
        //---------------------------------------
        // VFSM Status
        //---------------------------------------
        n = snprintf(ptr, count, "VFSM  : fsm=%d(%s)\n", dprx_mac_get_video_fsm(p_dprx->mac_idx), _get_dprx_vfsm_str(dprx_mac_get_video_fsm(p_dprx->mac_idx)));
        ptr+=n; count-=n;
#endif
    }

    return ptr - p_buf;
}



/*------------------------------------------------
 * Func : rtk_dprx_attr_show_dpcd
 *
 * Desc : show adapter dpcd
 *
 * Para : p_dprx : handle of DPRX adapter
 *        p_buf  : data buffer
 *        cnt    : buffer size
 *
 * Retn : number if bytes handled
 *-----------------------------------------------*/
int rtk_dprx_attr_show_dpcd(
    RTK_DPRX_ADP*   p_dprx,
    UINT8*          p_buf,
    UINT32          count
    )
{
    UINT32 n;
    UINT8 *ptr = p_buf;
    UINT8 buff[16];

    if (IS_CURRENT_PORT(p_dprx))
    {
        //---------------------------------------
        // Receiver Capability
        //---------------------------------------
        memset(buff, 0, sizeof(buff));
        lib_dprx_dpcd_read_bytes(p_dprx->mac_idx, DPCD_00000_DPCD_REV, buff, 12);
        n = scnprintf(ptr, count, "\n- Receiver Capability\n00000h-0000Bh : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\n",
            buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7],buff[8],buff[9],buff[10],buff[11]);
        ptr+=n; count-=n;

        //---------------------------------------
        // Link Config
        //---------------------------------------
        memset(buff, 0, sizeof(buff));
        lib_dprx_dpcd_read_bytes(p_dprx->mac_idx, 0x00100, buff, 8);
        n = scnprintf(ptr, count, "- Link Config \n00100h-00108h : %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\n",
            buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7],buff[8]);
        ptr+=n; count-=n;

        //---------------------------------------
        // Device Status
        //---------------------------------------
        memset(buff, 0, sizeof(buff));
        lib_dprx_dpcd_read_bytes(p_dprx->mac_idx, 0x00200, buff, 12);
        n = scnprintf(ptr, count, "- Device Status\n00200h-0020Bh : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7],buff[8],buff[9],buff[10],buff[11]);
        ptr+=n; count-=n;

        lib_dprx_dpcd_read_bytes(p_dprx->mac_idx, 0x00270, buff, 1);
        n = scnprintf(ptr, count, "00270h (TEST_SINK) : %02x\n\n",buff[0]);
        ptr+=n; count-=n;

        //---------------------------------------
        // Source Device Specific
        //---------------------------------------
        memset(buff, 0, sizeof(buff));
        lib_dprx_dpcd_read_bytes(p_dprx->mac_idx, DPCD_00300_IEEE_OUI_1, buff, 12);
        n = scnprintf(ptr, count, "- Source Device Specific\n00400h-0040Bh : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\n",
            buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7],buff[8],buff[9],buff[10],buff[11]);
        ptr+=n; count-=n;

        //---------------------------------------
        // Sink Device Specific
        //---------------------------------------
        memset(buff, 0, sizeof(buff));
        lib_dprx_dpcd_read_bytes(p_dprx->mac_idx, DPCD_00400_IEEE_OUI_1, buff, 12);
        n = scnprintf(ptr, count, "- Sink Device Specific\n00400h-0040Bh : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\n",
            buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7],buff[8],buff[9],buff[10],buff[11]);
        ptr+=n; count-=n;

        //---------------------------------------
        // Power
        //---------------------------------------
        memset(buff, 0, sizeof(buff));
        lib_dprx_dpcd_read_bytes(p_dprx->mac_idx, 0x00600, buff, 1);
        n = scnprintf(ptr, count, "- Device Power Control\n00600h-00600h : %02x \n\n", buff[0]);
        ptr+=n; count-=n;

        //---------------------------------------
        // EXT Receiver Capability
        //---------------------------------------
        memset(buff, 0, sizeof(buff));
        lib_dprx_dpcd_read_bytes(p_dprx->mac_idx, 0x002200, buff, 12);
        n = scnprintf(ptr, count, "- EXT Receiver Capability\n02200h-0220Bh : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\n",
            buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7],buff[8],buff[9],buff[10],buff[11]);
        ptr+=n; count-=n;
    }

    return ptr - p_buf;
}

#ifdef CONFIG_DPRX_ADP_ENABLE_HPD_TOGGLE_TEST
/*------------------------------------------------
 * Func : _do_hpd_toggle_test
 *
 * Desc : Check the HPD has been recognized by the source device.
 *
 * Para : p_dprx                 : handle of DPRX adapter
 *        hpd_delay_ms           : hpd delay times
 *        hpd_timeout_ms         : hpd timout times
 *        hpd_stable_count       : >0, retry hpd toggle is stable
 *
 * Retn : 1 : hpd toggle successed, 0 : hpd toggle timeout
 *-----------------------------------------------*/
int _do_hpd_toggle_test(
    RTK_DPRX_ADP* p_dprx,
    unsigned int  hpd_delay_ms,
    unsigned int  hpd_timeout_ms,
    UINT8 hpd_stable_count
    )
{
    UINT8 i, pre_link_round;
    UINT8 try_count;

    for (i = 0; i < hpd_stable_count; i++)
    {
        pre_link_round = p_dprx->link_round;
        try_count = hpd_timeout_ms / 100;

        rtk_dprx_set_hpd(p_dprx, 0);
        dprx_osal_msleep(hpd_delay_ms);
        rtk_dprx_set_hpd(p_dprx, 1);

        while(try_count && (pre_link_round == p_dprx->link_round))
        {
            try_count--;
            dprx_osal_msleep(100);
        }

        if (try_count == 0) // no link train update
            return 0;
    }
    return 1;   // hpd is stable
}

/*------------------------------------------------
 * Func : _hpd_toggle_test_main
 *
 * Desc : auto tune the shortest HPD time
 *
 * Para : p_dprx                : handle of DPRX adapter
 *        hpd_delay_ms_result   : shortest hpd times
 *
 * Retn : 1 : func successed, others : failed
 *-----------------------------------------------*/
int _hpd_toggle_test_main(
    RTK_DPRX_ADP* p_dprx,
    unsigned int* p_hpd_delay_ms_result
    )
{
    unsigned int hpd_delay_ms = 2;
    unsigned int hpd_timeout_ms = 2000;
    unsigned int func_timeout_ms = 5000;
    UINT8 hpd_stable_count = 3;
    unsigned int pre_hpd_guard_time = 0;

    if (dprx_hpd_get_guard_time(p_dprx->p_hpd) > 0) // clean hpd_guard_time to 0
    {
        pre_hpd_guard_time = dprx_hpd_get_guard_time(p_dprx->p_hpd);
        DPRX_ADP_INFO("[HPD_TEST] HPD guard_time change: %d->%d ms\n", pre_hpd_guard_time, 0);
        dprx_hpd_set_guard_time(p_dprx->p_hpd, 0);
    }

    DPRX_ADP_INFO("[HPD_TEST] Start to hpd toggle test...\n");
    while (hpd_delay_ms < func_timeout_ms)
    {
        if(_do_hpd_toggle_test(p_dprx, hpd_delay_ms, hpd_timeout_ms, 1) == 0)
        {   // When hpd toggle fail -> increase hpd_delay_ms
            DPRX_ADP_INFO("[HPD_TEST] HPD toggle delay time change: %d->%d ms\n", hpd_delay_ms, hpd_delay_ms + 20);
            hpd_delay_ms += 20;
        }
        else
        {   // When hpd toggle pass -> do hpd_stable_count
            DPRX_ADP_INFO("[HPD_TEST] Double check HPD delay time is shortest time...\n");
            if (_do_hpd_toggle_test(p_dprx, hpd_delay_ms, hpd_timeout_ms, hpd_stable_count) == 0)
                continue;
            else
            {
                *p_hpd_delay_ms_result = hpd_delay_ms;
                DPRX_ADP_INFO("[HPD_TEST] Found the hpd shortest time=%d ms\n", *p_hpd_delay_ms_result);

                break;  // continue to resume hpd_guard_time to before value
            }
        }
    }
    if (hpd_delay_ms >= func_timeout_ms)
    {
        *p_hpd_delay_ms_result = func_timeout_ms;
        DPRX_ADP_INFO("[HPD_TEST] Not found the hpd shortest time, p_hpd_delay_ms_result=%d ms\n", *p_hpd_delay_ms_result);
    }

    if (pre_hpd_guard_time > 0) // resume hpd_guard_time to before value
    {
        DPRX_ADP_INFO("[HPD_TEST] HPD guard_time change: %d->%d ms\n", 0, pre_hpd_guard_time);
        dprx_hpd_set_guard_time(p_dprx->p_hpd, pre_hpd_guard_time);
    }

    return 1;
}
#endif

/*------------------------------------------------
 * Func : rtk_dprx_attr_show_debug_ctrl
 *
 * Desc : show debug control attr
 *
 * Para : p_dprx : handle of DPRX adapter
 *        p_buf  : data buffer
 *        cnt    : buffer size
 *
 * Retn : number if bytes handled
 *-----------------------------------------------*/
int rtk_dprx_attr_show_debug_ctrl(
    RTK_DPRX_ADP*   p_dprx,
    UINT8*          p_buf,
    UINT32          count
    )
{
    UINT32 n;
    UINT8 *ptr = p_buf;

    n = snprintf(ptr, count, "force_max_lane_count=0x%02x (Off: 0, 1/2/4. others: reserved)\n",
        get_force_max_lane_count(p_dprx));
    ptr+=n; count-=n;

    n = snprintf(ptr, count, "force_max_link_rate=0x%02x (Off: 0, RBR: %x, HBR: %x, HBR2:%x, HBR3:%x, UHBR10: %x, UHBR13.5: %x)\n",
        get_force_max_link_rate(p_dprx), _DP_LINK_RBR, _DP_LINK_HBR, _DP_LINK_HBR2, _DP_LINK_HBR3, _DP_LINK_UHBR10G, _DP_LINK_UHBR13p5G);
    ptr+=n; count-=n;

#ifdef DPRX_SINK_CAP_LINK_TRAIN_CTRL_FAKE_TP1_LT
    n = snprintf(ptr, count, "force_fake_tps1_lt=%d\n", get_force_fake_tps1_lt(p_dprx));
    ptr+=n; count-=n;
#endif
#ifdef DPRX_SINK_CAP_LINK_TRAIN_CTRL_FAKE_TP2_LT
    n = snprintf(ptr, count, "force_fake_tps2_lt=%d\n", get_force_fake_tps2_lt(p_dprx));
    ptr+=n; count-=n;
#endif
    n = snprintf(ptr, count, "fw_tps1_tuning=0x0  (0x6 : RBR, 0xA: HBR, 0x14 : HBR2, 0x1E : HBR3\n");
    ptr+=n; count-=n;

    n = snprintf(ptr, count, "fw_tps2_tuning=0x0  (0x1 : TPS1, 0x2: TPS2, 0x3 : TPS3, 0x7 : TPS4\n");
    ptr+=n; count-=n;

    n = snprintf(ptr, count, "skip_source_detect=%d (0: off, 1 : disable detect flow)\n", get_skip_source_detect(p_dprx));
    ptr+=n; count-=n;

#ifdef DPRX_FAKE_PHY_SINK_TEST_OFF
    n = snprintf(ptr, count, "fake_phy_sink_test=%d (off:%x, lane0~3: %x/%x/%x/%x)\n",
            g_fake_phy_sink_test,
            DPRX_FAKE_PHY_SINK_TEST_OFF,
            DPRX_FAKE_PHY_SINK_TEST_LANE0,
            DPRX_FAKE_PHY_SINK_TEST_LANE1,
            DPRX_FAKE_PHY_SINK_TEST_LANE2,
            DPRX_FAKE_PHY_SINK_TEST_LANE3);
    ptr+=n; count-=n;
#endif

    n = snprintf(ptr, count, "force_lt_retrain=0\n");
    ptr+=n; count-=n;

    n = snprintf(ptr, count, "reset_vfsm=0\n");
    ptr+=n; count-=n;

    n = snprintf(ptr, count, "force_demode=%d\n", get_force_demode(p_dprx));
    ptr+=n; count-=n;

    n = snprintf(ptr, count, "force_timing_ext_flag_en=%d\n", g_force_timing_ext_flag_en);
    ptr+=n; count-=n;

    n = snprintf(ptr, count, "force_timing_ext_flag=%x (b7: v_drop, b6: h_drop, b1: bypassable, b0: over_disp_limit)\n",
		    g_force_timing_ext_flag);
    ptr+=n; count-=n;

    n = snprintf(ptr, count, "stream_bypass_mode=%d (%d : OFF, %d : TO_HDMITX)\n",
            rtk_dprx_get_ext_ctrl(p_dprx, ADP_EXCTL_STREAM_BYPASS_MODE),
            DPRX_STREAM_BYPASS_MODE_OFF, DPRX_STREAM_BYPASS_MODE_TO_HDMITX);
    ptr+=n; count-=n;

#ifdef CONFIG_DPRX_EANBLE_FREESYNC
    n = snprintf(ptr, count, "force_freesync_off=%d\n", get_force_freesync_off(p_dprx));
    ptr+=n; count-=n;
#endif

    n = snprintf(ptr, count, "dump_dolby_vision_sdp=%d\n", (g_dprx_sdp_debug_ctrl & DEBUG_DOLBY_VIOSN_SDP) ? 1 : 0);
    ptr+=n; count-=n;

    n = snprintf(ptr, count, "dump_amd_freesync_sdp=%d\n", (g_dprx_sdp_debug_ctrl & DEBUG_AMD_FREESYNC_SPD_SDP) ? 1 : 0);
    ptr+=n; count-=n;

    n = snprintf(ptr, count, "dump_vsc_ext_vesa_sdp=%d\n", (g_dprx_sdp_debug_ctrl & DEBUG_VSC_EXT_VESA_SDP) ? 1 : 0);
    ptr+=n; count-=n;

    n = snprintf(ptr, count, "mute_if_lane_dealign=%d\n", get_mute_if_lane_dealign(p_dprx) ? 1 : 0);
    ptr+=n; count-=n;
#ifdef CONFIG_DPRX_VFE_ENABLE_HPD_GUARD_TIME
    n = snprintf(ptr, count, "hpd_guard_time=%d\n", dprx_hpd_get_guard_time(p_dprx->p_hpd));
    ptr+=n; count-=n;
#endif
#ifdef CONFIG_DPRX_ENABLE_TASK_EVENT_HANDLER
    n = snprintf(ptr, count, "resume_guard_time=%d\n", g_resume_guard_time_interval);
    ptr+=n; count-=n;
#endif
#ifdef CONFIG_DPRX_ADP_ENABLE_HPD_TOGGLE_TEST
    if (hpd_toggle_delay_ms == 0)    // first hpd_toggle_test status
    {
        n = snprintf(ptr, count, "hpd_toggle_test=device_name\n");  ptr+=n; count-=n;
    }
    else
    {
        /*------------------------------------------------
         * Print: hpd_toggle_test=NV3050_DP->DP,hpd_toggle=60,SourceDevice=4e 56 49 44 49 41->NVIDIA
         * Desc : NV3050_DP       -> Source device name provided by user.
         *        DP              -> Sink device port.
         *        hpd_toggle=60   -> Shortest time of HPD toggle.
         *        SourceDevice=4e 56 49 44 49 41 -> HEX  : DPCD (Device Identification String)
         *        NVIDIA                         -> ASCII: DPCD (Device Identification String)
        *-----------------------------------------------*/
        UINT8 buff[6];
        DP_PORT_CAPABILITY cap;

        rtk_dprx_get_capability(p_dprx, &cap);
        n = scnprintf(ptr, count, "hpd_toggle_test=%s->%s,hpd_toggle=%d,",
            hpd_device_name, _rtk_dprx_get_type_str(cap.type), hpd_toggle_delay_ms); ptr+=n; count-=n;

        rtk_dprx_get_dpcd(p_dprx, 0x00303, buff, 6);   // Read value of the register for DPCD (Device Identification String)
        n = scnprintf(ptr, count, "SourceDevice=%02x %02x %02x %02x %02x %02x->%s\n",
            buff[0],buff[1],buff[2],buff[3],buff[4],buff[5], buff); ptr+=n; count-=n;
    }
#endif


#ifdef DPRX_EXT_DO_FW_LINK_TRAINING
    n = snprintf(ptr, count, "fw_tps1_tuning=0x0  (0x6 : RBR, 0xA: HBR, 0x14 : HBR2, 0x1E : HBR3\n");
    ptr+=n; count-=n;

    n = snprintf(ptr, count, "fw_tps2_tuning=0x0  (0x1 : TPS1, 0x2: TPS2, 0x3 : TPS3, 0x7 : TPS4\n");
    ptr+=n; count-=n;
#endif

    n = snprintf(ptr, count, "\n");  ptr+=n; count-=n;
    return ptr - p_buf;
}


/*------------------------------------------------
 * Func : rtk_dprx_attr_store_debug_ctrl
 *
 * Desc : store debug control attr
 *
 * Para : p_dprx : handle of DPRX adapter
 *        p_buf  : data buffer
 *        cnt    : buffer size
 *
 * Retn : number if bytes handled
 *-----------------------------------------------*/
int rtk_dprx_attr_store_debug_ctrl(
    RTK_DPRX_ADP*   p_dprx,
    UINT8*          p_buf,
    UINT32          count
    )
{
    int val;

    if (p_buf ==NULL || count ==0)
        return -1;

    if (sscanf(p_buf, "force_max_link_rate=0x%x", &val)==1)
    {
        if (val != get_force_max_link_rate(p_dprx))
        {
            switch(val)
            {
            case 0:
            case _DP_LINK_RBR:
            case _DP_LINK_HBR:
            case _DP_LINK_HBR2:
            case _DP_LINK_HBR3:
            case _DP_LINK_UHBR10G:
            case _DP_LINK_UHBR13p5G:
                break;
            default:
                DPRX_ADP_WARN("unknown max link rate - %x, skip it!!!\n", val);
                return -1;
            }

            set_force_max_link_rate(p_dprx,val);

            // restart current port
            if (p_dprx)
            {
                UINT8 connected = p_dprx->connected;
                rtk_dprx_set_hpd(p_dprx, 0);
                dprx_osal_msleep(100);
                rtk_dprx_port_swap(NULL);
                rtk_dprx_port_swap(p_dprx);
                dprx_osal_msleep(100);
                rtk_dprx_set_hpd(p_dprx, 1);
                p_dprx->connected = connected;
            }
        }
    }
    else if (sscanf(p_buf, "force_max_lane_count=0x%x", &val)==1)
    {
        if (val != get_force_max_lane_count(p_dprx))
        {
            switch(val)
            {
            case 0:
            case 1:
            case 2:
            case 4:
                break;
            default:
                DPRX_ADP_WARN("unknown max lane count - %x, skip it!!!\n", val);
                return -1;
            }

            set_force_max_lane_count(p_dprx,val);

            // restart current port
            if (get_force_max_lane_count(p_dprx) && p_dprx)
            {
                UINT8 connected = p_dprx->connected;
                rtk_dprx_set_hpd(p_dprx, 0);
                dprx_osal_msleep(100);
                rtk_dprx_port_swap(NULL);
                rtk_dprx_port_swap(p_dprx);
                dprx_osal_msleep(100);
                rtk_dprx_set_hpd(p_dprx, 1);
                p_dprx->connected = connected;
            }
        }
    }
#ifdef DPRX_SINK_CAP_LINK_TRAIN_CTRL_FAKE_TP1_LT
    else if (sscanf(p_buf, "force_fake_tps1_lt=%5d", &val)==1)
    {
        if(val)
            set_force_fake_tps1_lt(p_dprx, 1);
        else
            set_force_fake_tps1_lt(p_dprx, 0);
    }
#endif
#ifdef DPRX_SINK_CAP_LINK_TRAIN_CTRL_FAKE_TP2_LT
    else if (sscanf(p_buf, "force_fake_tps2_lt=%5d", &val)==1)
    {
        if(val)
            set_force_fake_tps2_lt(p_dprx, 1);
        else
            set_force_fake_tps2_lt(p_dprx, 0);
    }
#endif
    else if (sscanf(p_buf, "skip_source_detect=%5d", &val)==1)
    {
        if(val)
            set_skip_source_detect(p_dprx, 1);
        else
            set_skip_source_detect(p_dprx, 0);
    }
#ifdef DPRX_FAKE_PHY_CTS_SUPPORT
    else if (sscanf(p_buf, "fake_phy_sink_test=%x", &val)==1)
    {
        switch(val)
        {
        case DPRX_FAKE_PHY_SINK_TEST_OFF:
        case DPRX_FAKE_PHY_SINK_TEST_LANE0:
        case DPRX_FAKE_PHY_SINK_TEST_LANE1:
        case DPRX_FAKE_PHY_SINK_TEST_LANE2:
        case DPRX_FAKE_PHY_SINK_TEST_LANE3:
            // restart current port
            if (IS_CURRENT_PORT(p_dprx))
                lib_dprx_dpcd_write_manual_mode(p_dprx->mac_idx, DPCD_00270_TEST_SINK, 0);
            g_fake_phy_sink_test = val;
            break;
        default:
            break;
        }
    }
#endif
    else if (sscanf(p_buf, "force_lt_retrain=%5d", &val)==1)
    {
        if (IS_CURRENT_PORT(p_dprx))
        {
            lib_dprx_dpcd_write_manual_mode(p_dprx->mac_idx, DPCD_00202_LANE_0_1_STATUS, 0x00);
            lib_dprx_dpcd_write_manual_mode(p_dprx->mac_idx, DPCD_00203_LANE_2_3_STATUS, 0x00);
            lib_dprx_dpcd_write_manual_mode(p_dprx->mac_idx, DPCD_00204_LANE_ALIGN_STATUS_UPDATED, 0x80);
            //rtk_dprx_set_irq_hpd(g_p_current_dprx_port);
            rtk_dprx_set_hpd(p_dprx, 0);
            dprx_osal_msleep(500);
            rtk_dprx_set_hpd(p_dprx, 1);
        }
    }
    else if (sscanf(p_buf, "reset_vfsm=%5d", &val)==1)
    {
        dprx_drv_do_avmute(p_dprx->mac_idx);  // do avmute
        val = dprx_drv_source_check(p_dprx->mac_idx);  // redo source check
        DPRX_ADP_WARN("source check ret = %d\n", val);
        rtk_dprx_reset_signal_detect_flow(p_dprx);
    }
    else if (sscanf(p_buf, "force_demode=%5d", &val)==1)
    {
        if(val)
            set_force_demode(p_dprx, 1);
        else
            set_force_demode(p_dprx, 0);
        rtk_dprx_set_hpd(p_dprx, 0);
        dprx_osal_msleep(500);
        rtk_dprx_set_hpd(p_dprx, 1);
    }
    else if (sscanf(p_buf, "force_timing_ext_flag_en=%x", &val)==1)
    {
        g_force_timing_ext_flag_en = (val) ? 1 : 0;
    }
    else if (sscanf(p_buf, "force_timing_ext_flag=%x", &val)==1)
    {
        g_force_timing_ext_flag = val;
    }
    else if (sscanf(p_buf, "stream_bypass_mode=%2d", &val)==1)
    {
        if (val < DPRX_STREAM_BYPASS_MODE_MAX)
            rtk_dprx_set_ext_ctrl(p_dprx, ADP_EXCTL_STREAM_BYPASS_MODE, val);
        else
            rtk_dprx_set_ext_ctrl(p_dprx, ADP_EXCTL_STREAM_BYPASS_MODE, DPRX_STREAM_BYPASS_MODE_OFF);
    }
#ifdef CONFIG_DPRX_EANBLE_FREESYNC
    else if (sscanf(p_buf, "force_freesync_off=%5d", &val)==1)
    {
        if(val)
            set_force_freesync_off(p_dprx,1);
        else
            set_force_freesync_off(p_dprx,0);
        rtk_dprx_set_hpd(p_dprx, 0);
        dprx_osal_msleep(500);
        rtk_dprx_set_hpd(p_dprx, 1);
    }
#endif
    else if (sscanf(p_buf, "dump_dolby_vision_sdp=%5d", &val)==1)
    {
        if (val)
            g_dprx_sdp_debug_ctrl |= DEBUG_DOLBY_VIOSN_SDP;
        else
            g_dprx_sdp_debug_ctrl &= ~DEBUG_DOLBY_VIOSN_SDP;
    }
    else if (sscanf(p_buf, "dump_amd_freesync_sdp=%5d", &val)==1)
    {
        if (val)
            g_dprx_sdp_debug_ctrl |= DEBUG_AMD_FREESYNC_SPD_SDP;
        else
            g_dprx_sdp_debug_ctrl &= ~DEBUG_AMD_FREESYNC_SPD_SDP;
    }
    else if (sscanf(p_buf, "dump_vsc_ext_vesa_sdp=%5d", &val)==1)
    {
        if (val)
            g_dprx_sdp_debug_ctrl |= DEBUG_VSC_EXT_VESA_SDP;
        else
            g_dprx_sdp_debug_ctrl &= ~DEBUG_VSC_EXT_VESA_SDP;
    }
    else if (sscanf(p_buf, "mute_if_lane_dealign=%5d", &val)==1)
    {
        if(val)
            set_mute_if_lane_dealign(p_dprx,1);
        else
            set_mute_if_lane_dealign(p_dprx,0);
    }
#ifdef CONFIG_DPRX_VFE_ENABLE_HPD_GUARD_TIME
    else if (sscanf(p_buf, "hpd_guard_time=%5d", &val)==1)
    {
        dprx_hpd_set_guard_time(p_dprx->p_hpd, val);
    }
#endif
#ifdef CONFIG_DPRX_ENABLE_TASK_EVENT_HANDLER
    else if (sscanf(p_buf, "resume_guard_time=%5d", &val)==1)
    {
        g_resume_guard_time_interval = val;
    }
#endif
#ifdef CONFIG_DPRX_ADP_ENABLE_HPD_TOGGLE_TEST
    else if (sscanf(p_buf, "hpd_toggle_test=%s", hpd_device_name)==1)
    {
        DP_PORT_CAPABILITY cap;
        rtk_dprx_get_capability(p_dprx, &cap);
        DPRX_ADP_INFO("[HPD_TEST] ====== Start hpd_toggle_test, SourceDevice=%s->%s ======\n", hpd_device_name, _rtk_dprx_get_type_str(cap.type));
        _hpd_toggle_test_main(p_dprx, &hpd_toggle_delay_ms);
        DPRX_ADP_INFO("[HPD_TEST] ====== End hpd_toggle_test, SourceDevice=%s->%s ======\n", hpd_device_name, _rtk_dprx_get_type_str(cap.type));
    }
#endif
#ifdef DPRX_EXT_DO_FW_LINK_TRAINING
    else if (sscanf(p_buf, "fw_tps1_tuning=0x%x", &val)==1)
    {
        newbase_dprx_dpcd_set_link_bw_set(p_dprx->mac_idx, val);
        newbase_dprx_dpcd_set_lane_count_set(p_dprx->mac_idx, ((g_force_max_lane_count) ? g_force_max_lane_count : 4));
        dprx_drv_set_ext_config(p_dprx->mac_idx, DPRX_EXT_DO_FW_LINK_TRAINING, 1);
    }
    else if (sscanf(p_buf, "fw_tps2_tuning=0x%x", &val)==1)
    {
        dprx_drv_set_ext_config(p_dprx->mac_idx, DPRX_EXT_DO_FW_LINK_TRAINING, val);
    }
#endif

    return count;
}

/*------------------------------------------------
 * Func : rtk_dprx_attr_show_ver_info
 *
 * Desc : show adapter version
 *
 * Para : p_dprx : handle of DPRX adapter
 *        p_buf  : data buffer
 *        cnt    : buffer size
 *
 * Retn : number if bytes handled
 *-----------------------------------------------*/
int rtk_dprx_attr_show_ver_info(
    UINT8*          p_buf,
    UINT32          count
    )
{
    UINT32 n;
    UINT8 *ptr = p_buf;

    n = snprintf(ptr, count, "--- DP low level driver\n");                             ptr+=n; count-=n;
#if defined(DP_LOW_LEVEL_DRIVER_BUILD_DATE) && defined(DP_LOW_LEVEL_DRIVER_VERSION)
    n = snprintf(ptr, count, "Build date:\t%s\n", DP_LOW_LEVEL_DRIVER_BUILD_DATE);     ptr+=n; count-=n;
    n = snprintf(ptr, count, "Version:\t%s\n\n", DP_LOW_LEVEL_DRIVER_VERSION);         ptr+=n; count-=n;
#else
    n = snprintf(ptr, count, "Current patch doesn't enable finding the version number.\n\n");  ptr+=n; count-=n;
#endif
    return ptr - p_buf;
}

//===============================================================================
// Attribute Interface
//===============================================================================


/*------------------------------------------------
 * Func : rtk_dprx_attr_store
 *
 * Desc : store adapter attr
 *
 * Para : p_dprx : handle of DPRX adapter
 *        attr   : attribute name
 *        p_buf  : data buffer
 *        cnt    : buffer size
 *
 * Retn : number if bytes handled
 *-----------------------------------------------*/
int rtk_dprx_attr_show(
    RTK_DPRX_ADP*   p_dprx,
    ADP_ATTR        attr,
    UINT8*          p_buf,
    UINT32          count
    )
{
    UINT32 n;
    UINT8 *ptr = p_buf;

    switch(attr)
    {
    case ADP_ATTR_STATUS:
        n = rtk_dprx_attr_show_status(p_dprx, ptr, count);
        ptr+=n; count-=n;
        break;

    case ADP_ATTR_DPCD:
        n = rtk_dprx_attr_show_dpcd(p_dprx, ptr, count);
        ptr+=n; count-=n;
        break;

    case ADP_ATTR_SDP:
        break;

    case ADP_ATTR_DEBUG_CTRL:
        n = rtk_dprx_attr_show_debug_ctrl(p_dprx, ptr, count);
        ptr+=n; count-=n;
        break;

    case ADP_ATTR_VER_INFO:
        n = rtk_dprx_attr_show_ver_info(ptr, count);
        ptr+=n; count-=n;
        break;

    default:
        break;
    }

    return ptr - p_buf;
}


/*------------------------------------------------
 * Func : rtk_dprx_attr_store
 *
 * Desc : store adapter attr
 *
 * Para : p_dprx : handle of DPRX adapter
 *        attr   : attribute name
 *        p_buf  : data buffer
 *        cnt    : buffer size
 *
 * Retn : number if bytes handled
 *-----------------------------------------------*/
int rtk_dprx_attr_store(
    RTK_DPRX_ADP*   p_dprx,
    ADP_ATTR        attr,
    UINT8*          p_buf,
    UINT32          count
    )
{
    switch(attr)
    {
    case ADP_ATTR_STATUS:
        break;

    case ADP_ATTR_DPCD:
        break;

    case ADP_ATTR_SDP:
        break;

    case ADP_ATTR_DEBUG_CTRL:
        return rtk_dprx_attr_store_debug_ctrl(p_dprx, p_buf, count);
        break;

    case ADP_ATTR_VER_INFO:
        break;

    default:
        break;
    }

    return count;
}
