/*=============================================================
 * File:    dprx_adapter-rtk_dprx_priv_util.h
 *
 * Desc:    DPRX Adapter util: common struct of v1 & v2
 *
 * AUTHOR:  huang_tzu_yen@realtek.com
 *
 * Vresion: 0.0.1
 *
 *------------------------------------------------------------
 * Copyright (c) Realtek Semiconductor Corporation, 2021
 *
 * All rights reserved.
 *
 *============================================================*/


#ifndef __DPRX_ADAPTER_RTK_DPRX_PRIV_UTIL_H__
#define __DPRX_ADAPTER_RTK_DPRX_PRIV_UTIL_H__
#include <ScalerFunctionInclude.h>
#include <ScalerDpRxInclude.h>
#include <ScalerDpAuxRxInclude.h>
#include <ScalerDpMacRxInclude.h>   // for fec ???
#include <ScalerDpHdcpRxInclude.h>
#include <ScalerDpPhyRxInclude.h>

#include <dprx_edid-rtkddc.h>
#include <dprx_hpd-rtktv.h>
#include <dprx_hpd-typec.h>
#include <dprx_video_common-rtktv.h>

#define DPRX_AUX_DDC                3
#define DPRX_EDID_SIZE              512

extern UINT32 g_dprx_sdp_debug_ctrl;
#define DEBUG_DOLBY_VIOSN_SDP       (1<<0)
#define DEBUG_AMD_FREESYNC_SPD_SDP  (1<<1)
#define DEBUG_VSC_EXT_VESA_SDP      (1<<2)

typedef struct
{
    UINT32       pkt_count;
    UINT32       time_stamp;   // time stamp
    UINT16       expire_time;  // expire time (in ms)
    UINT16       debug_en : 1;
    UINT16       reserved : 15;     // reserved control flag
    DPRX_SDP_PKT pkt;
}DPRX_SDP_PKT_BUFFER;

typedef struct
{
    DPRX_SDP_PKT_BUFFER vsc;
    DPRX_SDP_PKT_BUFFER spd;
    DPRX_SDP_PKT_BUFFER hdr;
    DPRX_SDP_PKT_BUFFER dvs;  // dolby vision vsif
    DPRX_SDP_PKT_BUFFER vsi;  // non DVS VSI
    DPRX_SDP_PKT_BUFFER adp_sync;  // adaptive sync
    DPRX_SDP_PKT_BUFFER vsc_ext_vesa;

    // for extra parser info
    UINT8 freesync_support : 1;
    UINT8 freesync_enable : 1;
    UINT8 freesync_activated : 1;
}DPRX_SDP_HANDLER;

typedef struct
{
    UINT8 force_freesync_off;
    UINT8 force_max_lane_count;
    UINT8 force_max_link_rate;
    UINT8 force_fake_tps1_lt;  // disabled
    UINT8 force_fake_tps2_lt;  // disabled
    UINT8 skip_source_detect;      // disable detect flow to for debug
    UINT8 force_demode;
    UINT8 mute_if_lane_dealign;
} DPRX_DEBUG_CTRL;

extern DPRX_SDP_HANDLER g_dprx_sdp_handler;


typedef struct
{
    DP_TYPE type;                       // dp port type
    UINT8   mac_idx;                    // dp port mac id
    UINT8   ddc_idx;                    // dp using ddc id
    DPRX_SINK_CAPABILITY sink_cap;      // info for low level driver
#ifdef CONFIG_ENABLE_TYPEC_DPRX
    DPRX_PORT_CONFIG port_cfg;          // port config
    DPRX_CABLE_CFG cable_type;          // cable type
#endif
    UINT8 sink_edid[DPRX_EDID_SIZE];    // to store edid assigned from ap
    UINT8 suspend:1;
    UINT8 resume_connect:1;
    UINT8 qs_init_ready:1;
    UINT8 qs_connect_ready:1;
    UINT8 qs_open_ready:1;
    UINT8 qs_edid_ready:1;              // qs edid is ready (FW got real edid)
    UINT8 qs_edid_reload_request:1;     // need to reload edid after qs init
    UINT8 connected:1;
    UINT8 hpd_enable:1;                 // store hpd enable info
    UINT8 pre_cable_state:1;
    UINT8 wake_up_by_aux:1;
    UINT8 lpm_enable:1;

    UINT8 override_eotf_en:1;           // for RTKREQ-577
    UINT8 override_eotf:3;              // for RTKREQ-577

    UINT8 manul_eq_en:1;                // for RTKREQ-577 : when Enabled, DP PHY should use manual EQ setting
    UINT8 hdcp_1x_disable_en:1;         // for RTKREQ-577 : when Enabled, HDCP1x should be disabled
    UINT8 hdcp_2x_disable_en:1;         // for RTKREQ-577 : when Enabled, HDCP2x should be disabled
    UINT8 hdcp_2x_force_reauth:1;       // for RTKREQ-577 : when Enabled, need to reauth HDCP2 (auto clear after reauth)

#ifdef CONFIG_DPRX_FORCE_CHECK_MODE_FAIL_CABLE_OFF_ON
    UINT8 output_enable:1;
    UINT8 fake_cable_off;
#endif
    hpd_dev_t* p_hpd;   // for hpd / detect control
    edid_dev_t* p_edid;
    UINT8 edid_resume;
    UINT8 current_port;
    DPRX_SDP_HANDLER sdp_handler;
    DPRX_DEBUG_CTRL debug_ctrl;

#ifdef CONFIG_DPRX_STATUS_MONITOR
    UINT32 status_monitor_time;
#endif
#ifdef CHECK_SOURCE_IN_GET_CONNECT_STATE
    UINT32 source_check_time;
#endif

    UINT8 link_round;
    UINT8 link_state;
    UINT8 link_sub_state;

    UINT8 vfsm_status;
    UINT32 pre_source_check_no_signal_time;  //for record the time of source_check_start

    UINT8 resume_connect_handler_event;
    UINT8 reconnect_handler_event;

    #define DPRX_MAX_HDCP_EVENT_QUEUE         4
    UINT8 hdcp2x_event_rp;
    UINT8 hdcp2x_event_wp;
    UINT8 hdcp2x_event_msg_id[DPRX_MAX_HDCP_EVENT_QUEUE];        //for hdcp2.x event

    UINT8 sink_capabliity_update;

    UINT8 dprx_port_readly;
    UINT8 dprx_aux_wakeup_mode;
    UINT8 dprx_wakeup_mode_sel;

    DPRX_HDCP_STATUS_T dprx_hdcp_status;

    vc_dev_t* p_vc;   // for video common / detect control

    UINT8 ignore_dsc_check_en;
}RTK_DPRX_ADP;


//================ MACRO ==================
#ifdef UNIT_TEST
#define static    // remove static declariation of local variable
extern RTK_DPRX_ADP* g_p_current_dprx_port;
extern edid_dev_t*   g_p_dprx_edid;
extern unsigned char g_dprx_edid_reference_cnt;
extern UINT8 g_skip_source_detect;
#endif
//================ API ==================

typedef struct
{

    // Basic control
    int (*reset)(RTK_DPRX_ADP* p_dprx);
    int (*port_swap)(RTK_DPRX_ADP* p_dprx);
    int (*init)(RTK_DPRX_ADP* p_dprx);
    int (*uninit)(RTK_DPRX_ADP* p_dprx);
    int (*open)(RTK_DPRX_ADP* p_dprx);
    int (*close)(RTK_DPRX_ADP* p_dprx);
    int (*connect)(RTK_DPRX_ADP* p_dprx);
    int (*disconnect)(RTK_DPRX_ADP* p_dprx);

    int (*source_check)(RTK_DPRX_ADP* p_dprx);
    int (*get_capability)(RTK_DPRX_ADP* p_dprx, DP_PORT_CAPABILITY* p_cap);
    int (*get_link_status)(RTK_DPRX_ADP* p_dprx, DP_LINK_STATUS_T* p_status);

    int (*set_hpd)(RTK_DPRX_ADP* p_dprx, UINT8 enable);
    int (*set_hpd_toggle)(RTK_DPRX_ADP* p_dprx, UINT16 delay_ms);
    int (*set_irq_hpd)(RTK_DPRX_ADP* p_dprx);
    int (*get_hpd)(RTK_DPRX_ADP* p_dprx);
    int (*get_typec_pd_ready_status)(RTK_DPRX_ADP* p_dprx);
    int (*get_connect_status)(RTK_DPRX_ADP* p_dprx);
    int (*get_aux_status)(RTK_DPRX_ADP* p_dprx);

    int (*set_edid)(RTK_DPRX_ADP* p_dprx, unsigned char* p_edid, unsigned short edid_len);
    int (*get_edid)(RTK_DPRX_ADP* p_dprx, unsigned char* p_edid, unsigned short edid_len);

    int (*set_dpcd)(RTK_DPRX_ADP* p_dprx, unsigned long address, unsigned char* p_dpcd, unsigned short len);
    int (*get_dpcd)(RTK_DPRX_ADP* p_dprx, unsigned long address, unsigned char* p_dpcd, unsigned short len);


    int (*get_vbid_data)(RTK_DPRX_ADP* p_dprx, DPRX_VBID_INFO_T* p_vbid);
    int (*get_msa_data)(RTK_DPRX_ADP* p_dprx, DPRX_MSA_INFO_T* p_msa);
    int (*get_sdp_data)(RTK_DPRX_ADP* p_dprx, DP_SDP_TYPE type, DP_SDP_DATA_T* p_data);
    int (*get_pps_data)(RTK_DPRX_ADP* p_dprx,DP_PPS_DATA_T* p_pps_sdp);

    int (*get_stream_type)(RTK_DPRX_ADP* p_dprx, DP_STREAM_TYPE_E* p_type);
    int (*get_video_timing)(RTK_DPRX_ADP* p_dprx, DPRX_TIMING_INFO_T* p_timing);
    int (*get_pixel_encoding_info)(RTK_DPRX_ADP* p_dprx,DP_PIXEL_ENCODING_FORMAT_T* p_pixel);
    int (*get_stream_bypasss_timing)(RTK_DPRX_ADP* p_dprx,DPRX_TIMING_INFO_T* p_timing);
    int (*get_drm_info)(RTK_DPRX_ADP* p_dprx, DPRX_DRM_INFO_T* p_drm);
    int (*get_avi_info)(RTK_DPRX_ADP* p_dprx, DPRX_AVI_INFO_T* p_avi);

    int (*get_audio_status)(RTK_DPRX_ADP* p_dprx, DP_AUDIO_STATUS_T* p_audio_status);
    int (*get_hdcp_status)(RTK_DPRX_ADP* p_dprx, DPRX_HDCP_STATUS_T* p_hdcp_status);

    int (*suspend)(RTK_DPRX_ADP* p_dprx, UINT32 mode);
    int (*resume)(RTK_DPRX_ADP* p_dprx);

    int (*reset_signal_detect_flow)(RTK_DPRX_ADP* p_dprx);
    int (*handle_online_measure_error)(RTK_DPRX_ADP* p_dprx);
    void(*handle_infoframe_sdp_update_event)(RTK_DPRX_ADP* p_dprx, SDP_TYPE_E sdp_type);

    int (*set_ext_ctrl)(RTK_DPRX_ADP* p_dprx, ADP_EXCTL_ID ctrl, unsigned int val);
    int (*get_ext_ctrl)(RTK_DPRX_ADP* p_dprx, ADP_EXCTL_ID ctrl);
    
    void (*port_set_aux_int_crtl_rounting_enable)(RTK_DPRX_ADP* p_dprx, UINT8 enable);
    void (*set_aux_int_crtl_rounting_enable)(UINT8 enable);

#ifdef  ENABLE_DPRX_LT_EVENT_MONITOR
    void (*handle_lt_state_update_event)(RTK_DPRX_ADP* p_dprx, UINT8 states);
#else
    #define rtk_dprx_handle_lt_state_update_event(p_dprx, state)
#endif

#ifdef CONFIG_DPRX_ENABLE_TASK_EVENT_HANDLER
    void (*port_event_task_handler)(RTK_DPRX_ADP* p_dprx);  // use for v2
    void (*event_task_handler)(void); // use for v1
#else
    #define rtk_dprx_port_event_task_handler(p_dprx)
    #define rtk_dprx_event_task_handler()
#endif

#ifdef CONFIG_SUPPORT_DOLBY_VISION_DP
    int (*set_dolby_vision_check_mode_function)(long (*get_dolby_vision_dp_mode)(unsigned char));
#endif


}dprx_priv_ops_t;


#endif // __DPRX_ADAPTER_RTK_DPRX_PRIV_V1_H__