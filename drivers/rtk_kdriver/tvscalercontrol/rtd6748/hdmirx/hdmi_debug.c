
#include "hdmi_debug.h"

//Global variable for debugging/sysfs

//------------------------------------------------------------------
// HDMI FLOW PRINT Setting
//------------------------------------------------------------------

unsigned int debugging_flow_print_flag = HDMI_FLOW_PRPINT_AUDIO | HDMI_FLOW_PRPINT_CEDERR | HDMI_FLOW_PRPINT_SCDC | HDMI_FLOW_PRPINT_FRL | HDMI_FLOW_PRPINT_DSCD | HDMI_FLOW_PRPINT_AMDFREESYNC;    // Default: enable audio
unsigned short g_audio_print_cycle_thd = 600;
unsigned short g_char_bit_print_cycle_cnt_thd = CHAR_BIT_PRINT_CYCLE_CNT_THD;
unsigned short g_scdc_print_cycle_cnt_thd = 2000;
unsigned short g_pwr_print_cycle_cnt_thd = 2000; // real loop thd =  g_pwr_print_cycle_cnt_thd/Max_port_num
unsigned short g_dscd_print_cycle_cnt_thd = 1000;

#define AMD_FREESYNC_DEBUG_MESSAGE_TIMEOUT   2000
unsigned short  g_amd_vsdb_debug_thd = AMD_FREESYNC_DEBUG_MESSAGE_TIMEOUT;

unsigned char g_infoframe_drm_debug_print = 0;
unsigned char g_infoframe_avi_debug_print = 0;
unsigned char g_infoframe_spd_debug_print = 0;
unsigned char g_infoframe_audio_debug_print = 0;
unsigned char g_infoframe_vsi_debug_print = 0;
unsigned char g_infoframe_dvs_debug_print = 0;
unsigned char g_infoframe_hdr10pvsi_debug_print = 0;
unsigned char g_infoframe_xperi_vsi_debug_print = 0;
unsigned char g_infoframe_rsv3_debug_print = 0;
unsigned char g_infoframe_cvtem_debug_print = 0;
unsigned char g_infoframe_vtem_debug_print = 0;
unsigned char g_infoframe_hdremp_debug_print = 0;
unsigned char g_infoframe_vsem_debug_print = 0;
unsigned char g_qms_debug_print = 0;

//------------------------------------------------------------------
// HDMI sysfs debugging initial setting
//------------------------------------------------------------------
//for lib_hdmi_get_avmute
unsigned char g_hdmi_debug_force_avmute[4] = {0, 0, 0, 0};

//for newbase_hdmi_video_monitor
unsigned char g_hdmi_pixel_monitor_en = 0;
unsigned int  g_hdmi_pixel_monitor_x = 1024;
unsigned int  g_hdmi_pixel_monitor_y = 1024;

//for newbase_hdmi_error_handler
unsigned int g_hdmi_err_det_message_interval = 5000;

unsigned char g_hdmi_debug_offms_continuous = 0;
unsigned int g_hdmi_debug_add_onms_mask = 0;

//for bypassing connect & disconnect
unsigned char g_hdmi_bypass_connect_disconnect = 0;

//for check COMMCRC
unsigned int g_hdmi_check_commcrc = 0;

//for crc continue mode
unsigned char g_is_hdmi_crc_continue_mode = 1;

unsigned char g_check_commcrc_num = 30;

unsigned char g_picture_non_static_thd = 10;

//for check COMMCRC time
unsigned int g_hdmi_check_commcrc_over_ms = 20000;

//for check COMMCRC max fail times
unsigned int g_hdmi_check_commcrc_max_fail_times = 1;

// for YUV422_CD_pixel_cnt_thd
unsigned char g_yuv422_cd_pixel_cnt_thd = 0; // 0 ~ 127

// forYUV422_CD_frame_cnt_thr
unsigned char g_yun422_cd_frame_cnt_thd = 0; // 0 ~ 127

unsigned char g_hdmi_dsc_fw_mode_en = 0;

unsigned char g_hdmi_force_5v_debug = 0;

unsigned char g_hdmi_vrr_debug_disable = HDMI_DEBUG_PACKET_USE_ORIGINAL;  // default use vtem info vrr

