/*=============================================================
 * File:    dprx_adapter-rtk_dprx_priv.h
 *
 * Desc:    DPRX Adapter
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
#ifndef __DPRX_ADAPTER_RTK_DPRX_PRIV_H__
#define __DPRX_ADAPTER_RTK_DPRX_PRIV_H__

#include <dprx_edid-rtkddc.h>
#include <dprx_hpd-rtktv.h>
#include <dprx_hpd-typec.h>
#include <dprx_video_common-rtktv.h>
#include <dprx_adapter-rtk_dprx-priv_util.h>
#define DPRX_AUX_DDC                3
#define DPRX_EDID_SIZE              512

extern UINT32 g_dprx_sdp_debug_ctrl;
#define DEBUG_DOLBY_VIOSN_SDP       (1<<0)
#define DEBUG_AMD_FREESYNC_SPD_SDP  (1<<1)
#define DEBUG_VSC_EXT_VESA_SDP      (1<<2)


//================ API ==================

extern int  rtk_dprx_ddc_init(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_ddc_uninit(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_port_swap(RTK_DPRX_ADP* p_dprx);
extern int rtk_dprx_port_reset(RTK_DPRX_ADP* p_dprx);

extern int  rtk_dprx_init(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_uninit(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_open(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_close(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_connect(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_disconnect(RTK_DPRX_ADP* p_dprx);

extern int  rtk_dprx_source_check(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_get_capability(RTK_DPRX_ADP* p_dprx, DP_PORT_CAPABILITY* p_cap);
extern int  rtk_dprx_get_link_status(RTK_DPRX_ADP* p_dprx, DP_LINK_STATUS_T* p_status);

extern int  rtk_dprx_set_hpd(RTK_DPRX_ADP* p_dprx, UINT8 enable);
extern int  rtk_dprx_set_hpd_toggle(RTK_DPRX_ADP* p_dprx, UINT16 delay_ms);
extern int  rtk_dprx_set_irq_hpd(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_get_hpd(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_get_typec_pd_ready_status(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_get_connect_status(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_get_aux_status(RTK_DPRX_ADP* p_dprx);

extern int  rtk_dprx_set_edid(RTK_DPRX_ADP* p_dprx, unsigned char* p_edid, unsigned short edid_len);
extern int  rtk_dprx_get_edid(RTK_DPRX_ADP* p_dprx, unsigned char* p_edid, unsigned short edid_len);

extern int  rtk_dprx_get_dpcd(RTK_DPRX_ADP* p_dprx, unsigned long address, unsigned char* p_dpcd, unsigned short len);
extern int  rtk_dprx_set_dpcd(RTK_DPRX_ADP* p_dprx, unsigned long address, unsigned char* p_dpcd, unsigned short len);

extern int  rtk_dprx_get_vbid_data(RTK_DPRX_ADP* p_dprx, DPRX_VBID_INFO_T* p_vbid);
extern int  rtk_dprx_get_msa_data(RTK_DPRX_ADP* p_dprx, DPRX_MSA_INFO_T* p_msa);
extern int  rtk_dprx_get_sdp_data(RTK_DPRX_ADP* p_dprx, DP_SDP_TYPE type, DP_SDP_DATA_T* p_data);
extern int  rtk_dprx_get_pps_data(RTK_DPRX_ADP* p_dprx, DP_PPS_DATA_T* p_pps);

extern int  rtk_dprx_get_stream_type(RTK_DPRX_ADP* p_dprx, DP_STREAM_TYPE_E* p_type);
extern int  rtk_dprx_get_video_timing(RTK_DPRX_ADP* p_dprx, DPRX_TIMING_INFO_T* p_timing);
extern int  rtk_dprx_get_pixel_encoding_info(RTK_DPRX_ADP* p_dprx,DP_PIXEL_ENCODING_FORMAT_T* p_pixel);
extern int  rtk_dprx_get_stream_bypasss_timing(RTK_DPRX_ADP* p_dprx, DPRX_TIMING_INFO_T* p_timing);
extern int  rtk_dprx_get_drm_info(RTK_DPRX_ADP* p_dprx, DPRX_DRM_INFO_T* p_drm);
extern int  rtk_dprx_get_avi_info(RTK_DPRX_ADP* p_dprx, DPRX_AVI_INFO_T* p_avi);

extern int  rtk_dprx_get_audio_status(RTK_DPRX_ADP* p_dprx, DP_AUDIO_STATUS_T* p_audio_status);
extern int  rtk_dprx_get_hdcp_status(RTK_DPRX_ADP* p_dprx, DPRX_HDCP_STATUS_T* p_hdcp_status);

extern int  rtk_dprx_suspend(RTK_DPRX_ADP* p_dprx, UINT32 mode);
extern int  rtk_dprx_resume(RTK_DPRX_ADP* p_dprx);

extern int  rtk_dprx_reset_signal_detect_flow(RTK_DPRX_ADP* p_dprx);
extern int  rtk_dprx_handle_online_measure_error(RTK_DPRX_ADP* p_dprx);
extern void rtk_dprx_handle_infoframe_sdp_update_event(RTK_DPRX_ADP* p_dprx, SDP_TYPE_E sdp_type);

#ifdef ENABLE_DPRX_LT_EVENT_MONITOR
extern void rtk_dprx_handle_lt_state_update_event(RTK_DPRX_ADP* p_dprx, UINT8 state);
#else
#define rtk_dprx_handle_lt_state_update_event(p_dprx, state)
#endif

#ifdef CONFIG_DPRX_ENABLE_TASK_EVENT_HANDLER
extern void rtk_dprx_port_event_task_handler(RTK_DPRX_ADP* p_dprx);
extern void rtk_dprx_event_task_handler(void);
#else
#define rtk_dprx_port_event_task_handler(p_dprx)
#define rtk_dprx_event_task_handler()
#endif

extern void rtk_dprx_port_set_aux_int_crtl_rounting_enable(RTK_DPRX_ADP* p_dprx, UINT8 enable);
extern void rtk_dprx_set_aux_int_crtl_rounting_enable(UINT8 enable);
extern int  rtk_dprx_set_ext_ctrl(RTK_DPRX_ADP* p_dprx, ADP_EXCTL_ID ctrl, unsigned int val);
extern int  rtk_dprx_get_ext_ctrl(RTK_DPRX_ADP* p_dprx, ADP_EXCTL_ID ctrl);

#ifdef CONFIG_RTK_DPRX_SYSFS
extern int  rtk_dprx_attr_show(RTK_DPRX_ADP* p_dprx, ADP_ATTR attr, UINT8* p_buf, UINT32 cnt);
extern int  rtk_dprx_attr_store(RTK_DPRX_ADP* p_dprx, ADP_ATTR attr, UINT8* p_buf, UINT32 cnt);
#else
#define rtk_dprx_attr_show(p_dprx, attr, p_buf, cnt)         (0)
#define rtk_dprx_attr_store(p_dprx, attr, p_buf, cnt)        (cnt)
#endif

#ifdef CONFIG_SUPPORT_DOLBY_VISION_DP
extern int rtk_dprx_set_dolby_vision_check_mode_function(long (*get_dolby_vision_dp_mode)(unsigned char));
#endif
#endif // __DPRX_ADAPTER_RTK_DPRX_PRIV_H__
