#ifndef __VSC_H
#define  __VSC_H
#ifdef BUILD_QUICK_SHOW
#include <sysdefs.h>
#endif

#include <ioctrl/scaler/vsc_cmd_id.h>
#include <base_types.h>
#include <tvscalercontrol/panel/panel_dlg.h>
#include "tvscalercontrol/scalerdrv/scalerdrv.h"
#include <measure/rtk_measure.h>
#define ENABLE_SCALER_DUMP_FRAME_INFO //for bring up
//#define CONFIG_SUPPORT_SDR_MAX_RGB
#ifdef ENABLE_SCALER_DUMP_FRAME_INFO
  /* dump VO CRC info to file */
  #ifndef FILE_NAME_SIZE
  #define FILE_NAME_SIZE 50
  #define DUMP_ES_SIZE 2*1024*1024
  #endif
  typedef struct SCALER_MALLOC_STRUCT {
  	unsigned long Memory;
  	unsigned long PhyAddr ;
  	unsigned long VirtAddr ;
  } SCALER_MALLOC_STRUCT;

  typedef struct SCALER_DUMP_STRUCT {
  	unsigned char file_name[FILE_NAME_SIZE];    /* debug log file place & name */
  	unsigned int  mem_size; 	/* debug memory size */
  	unsigned char enable ;
  } SCALER_DUMP_STRUCT;

  typedef struct SCALER_DUMP_BUFFER_HEADER {
  	unsigned int magic;
  	unsigned int size;
  	unsigned int rd;
  	unsigned int wr;
  } SCALER_DUMP_BUFFER_HEADER;
  // -------------------------------
#endif

struct vsc_film_mode_parameter_t {
	int enable;
	int type;
	int v_freq_1000;
};

typedef enum{
    _low_power_tsk_no_pending = 0,      //not low power mode
    _low_power_tsk_request_pending,     //request pending
    _low_power_tsk_pending_finish,      //pending finsiah
} _LOW_POWER_TASK_PENDING_STATUS;

typedef enum{
    TSK_NAME_VSC = 0,                   //vsc_scaler_tsk
    TSK_NAME_GAME_MODE,                 //new_game_mode_tsk
    TSK_NAME_GAME_MODE_UNMUTE,          //game_mode_unmute_tsk
    TSK_NAME_MUTE_OFF,                  //video_muteoff_tsk
    TSK_NAME_FIX_LAST_LINE,             //fixlast_line_tsk
    TSK_NAME_LOCAL_DIMMING,             //localDimmingDemoCtrl_tsk
    TSK_NAME_MAX_DUMMY,
} _SCALER_TSK_LIST;

typedef enum{
    VSC_I_DOMAIN = 0x1,//VSC run i domain 
    VSC_M_DOMAIN = 0x2,//VSC run m domain 
    VSC_D_DOMAIN = 0x4,//VSC run d domain 
    VSC_MAIN_SMOOTHTOGGLE = 0x8,//VSC run main smooth toggle
    VSC_SUB_SMOOTHTOGGLE = 0x10,//VSC run sub smooth toggle
    VSC_FREEZE_DISABLE_MAIN_IP = 0x20, //for fw_scalerip_disable_onlyip
    VSC_FREEZE_DISABLE_SUB_IP = 0x40,//for fw_scalerip_disable_onlyip
    VSC_MAIN_RESET_MODE = 0x80,//reset mode main
    VSC_SUB_RESET_MODE = 0x100,//reset mode sub
    VSC_MAIN_BBD_VGIP = 0x200,//MAIN VGIP BBD
    VSC_SUB_BBD_VGIP = 0x400,//MAIN VGIP BBD
    VSC_MEMC_REALCINEMA = 0x800,//MEMC_REALCINEMA 
    VSC_I3DDMA_DRIVE = 0x1000,//VSC run i3ddma
    VSC_MAIN_CHECK_MODE = 0x2000,//VSC run main check mode
    VSC_SUB_CHECK_MODE = 0x4000,//VSC run sub check mode
    VSC_MESPG_AUTO_TEST = 0x8000,//VSC run MSPG auto test
    VSC_I3DDMA_NN = 0x10000,//VSC run i3ddma NN
    VSC_VPQ_LD = 0x20000,//VSC VPQ LD
    VSC_IV2DV_TUNING = 0x40000,//VSC do iv to dv tuning 
    VSC_DOLBY_LB = 0x80000,//vsc dolby letter box
    VSC_MAIN_PRE_CHECK = 0x100000,//VSC before run scaler 
    VSC_WAIT_GAME_MODE = 0x200000,//VSC wait game mode finish
    VSC_WAIT_VO_INFO = 0x400000,//VSC wait vo info
    VSC_ATV_WAIT_STABLE = 0x800000,//ATV wait stable
    VSC_ATV_SCALER = 0x1000000,//ATV run scaler
    VSC_ATV_SMOOTHTOGGLE = 0x2000000,//ATV run smooth toggle
    VSC_NON_ATV_MAIN_SCALER = 0x4000000,//NONATV run main scaler
    VSC_NON_ATV_SUB_SCALER = 0x8000000,//NONATV run sub scaler
    VSC_SUB_PRE_CHECK = 0x10000000,//VSC before run scaler
    VSC_GET_INFO = 0x20000000,//VSC get_displayinfo
}VSC_TASK_STATUS;

typedef enum{
    CHECK_SIGNAL_NO_ERR = 0,
    CHECK_SIGNAL_ERR_HDMI_CHECK_MODE_FAIL,
    CHECK_SIGNAL_ERR_HDMI_VRR_STATUS_CHANGE,
    CHECK_SIGNAL_ERR_HDMI_FREESYNC_STATUS_CHANGE,
    CHECK_SIGNAL_ERR_HDMI_QMS_STATUS_CHANGE,
    CHECK_SIGNAL_ERR_DP_CHECK_MODE_FAIL,
    CHECK_SIGNAL_ERR_DP_VRR_STATUS_CHANGE,
    CHECK_SIGNAL_ERR_DP_FREESYNC_STATUS_CHANGE,
    CHECK_SIGNAL_ERR_HDMI_HDR_CHANGE,
    CHECK_SIGNAL_ERR_DP_HDR_CHANGE,
    CHECK_SIGNAL_ERR_DP_COLOR_SPACE_CHANGE,
    CHECK_SIGNAL_ERR_HDMI_DP_2P_MODE_CHANGE,
} _SCALER_CHECK_SIGNAL_ERROR_STATUS;

//VSC_INPUT_TYPE_T Get_DisplayMode_Src(unsigned char display);
#if 0
unsigned char vo_overscan_disable(VSC_INPUT_TYPE_T srctype);
unsigned char vo_overscan_adjust(VSC_INPUT_TYPE_T srctype);
#endif
unsigned char get_ForceSmoothtoggleGo(void);
unsigned char vsc_get_adaptivestream_flag(unsigned char display);
unsigned char Get_AVD_display(unsigned char display);
void fw_scalerip_set_di_gamemode_flag(unsigned char bflag);//20170524 pinyen create new_game_mode_tsk
bool rtk_hal_vsc_GetRGB444Mode(void);
void Scaler_start_orbit_algo(unsigned char b_enable);
void Scaler_reset_orbit(void);
void ScalerForceUpdateOrbit(unsigned char force_update);
unsigned char Scaler_get_orbit_algo_status(void);
void fw_set_vsc_GameMode(unsigned char b_vscGameMode_OnOff);
unsigned char fw_get_vsc_GameMode(void);

#ifdef SMOOTH_TOGGLE_DATAFRCFS_TEST
unsigned char vsc_decide_timingframesync(unsigned char display);
#endif
unsigned char Get_AVD_display(unsigned char display);
unsigned char Get_Factory_SelfDiagnosis_Mode(void);
unsigned char Scaler_data_frc_fs_change_adjust_after_scaler(unsigned char display,unsigned short disp_height);
void Scaler_data_frc_fs_change_adjust_outputregion(unsigned char display,unsigned short disp_height);
void Scaler_Dolby_HDR_reset(void);
unsigned char Scaler_check_orbit_store_mode(void);
unsigned char uc4_get_main_dispwin_value(unsigned short *x, unsigned short *y, unsigned short *w, unsigned short *h);//WOSQRTK-7731
void scaler_i2rnd_run_main(void);
void rtk_hal_vsc_i2rnd_enable(unsigned char enable);
unsigned short Scaler_CalAVD27MWidth(SCALER_DISP_CHANNEL display,unsigned short a_usInput);
unsigned char rtk_hal_vsc_i2rnd_b05_enable(unsigned char enable);
KADP_VSC_SUB_PURPOSE_T Get_Magnifier_LiveZoom_Mode(void);
void set_frc_style_input_fast_than_display(bool enable);
unsigned char get_frc_style_input_fast_than_display(void);
void scaler_save_main_info(void);
void scaler_update_struct_info(unsigned char display);
unsigned char rtk_hal_vsc_set_localDimmingCtrlDemoMode(unsigned char bType, unsigned char bCtrl);
unsigned char get_new_game_mode_condition(void);
unsigned char get_new_game_mode_small_window_condition(void);
void scaler_vsc_set_vr360_block_mdomain_doublebuffer(UINT8 bEnable);
UINT8 scaler_vsc_get_vr360_block_mdomain_doublebuffer(void);
void scaler_vsc_vr360_enter_datafs_proc(void);
void scaler_vsc_vr360_enter_datafrc_proc(void);
struct vsc_film_mode_parameter_t vsc_set_film_mode_param(int film_mode_enable, int film_mode_type, unsigned int v_freq);
void scaler_vsc_set_gamemode_force_framerate_lowdelay_mode(UINT8 bOnOff);
UINT8 scaler_vsc_get_gamemode_force_framerate_lowdelay_mode(void);
unsigned int scaler_vsc_get_gamemode_rerun_count(void);
void scaler_vsc_reset_gamemode_rerun_count(void);
void scaler_vsc_increase_gamemode_rerun_count(void);
void scaler_vsc_set_external_src_backporch(unsigned char display,   unsigned int backporch);
unsigned int scaler_vsc_get_external_src_backporch(void);
void scaler_vsc_set_adaptive_pst_lowdelay_mode(UINT8 bOnOff);
UINT8 scaler_vsc_get_adaptive_pst_lowdelay_mode(void);
void scaler_vsc_set_force_pst_lowdelay_mode(UINT8 bOnOff);
UINT8 scaler_vsc_get_force_pst_lowdelay_mode(void);
void scaler_vsc_set_keep_vo_framerate_control(UINT8 bOnOff);
UINT8 scaler_vsc_get_keep_vo_framerate_control(void);
void Scaler_ForceUpdate_Callback_Delay_Info_by_InputOutput_Region(KADP_VIDEO_RECT_T  OutputRegion);
void Scaler_Fulfill_Callback_Delay_Info(KADP_SCALER_WIN_CALLBACK_DELAY_INFO *pCallBack_info);
void Scaler_Reset_ForceUpdate_Callback_Delay_Info(void);
struct vsc_film_mode_parameter_t get_film_mode_parameter(void);
struct semaphore* get_gamemode_check_semaphore(void);
void game_mode_set_ignore_cmd_check(unsigned char enable);
unsigned char Scaler_get_vdec_2k120hz(void);
unsigned char get_new_game_mode_vdec_memc_bypass_condition(void);
void scaler_I2D_pwm_frequency_update(void);
unsigned char scaler_get_I2D_pwm_frequency_update_in_isr(void);
unsigned char scaler_get_I2D_tracking_in_realcinema(void);
void Convert_Timing_To_fs_Depend_datafs(unsigned char display);
void scaler_overscan_vertical_size(unsigned char display,unsigned int progressive,unsigned int source_height,unsigned int *height,unsigned int *v_start);
void scaler_vsc_set_keep_vo_framerate_control(UINT8 bOnOff);
UINT8 scaler_vsc_get_keep_vo_framerate_control(void);
void scaler_set_VoTrackingI3DDMA_frequency_update_in_isr(unsigned char bEnable);
unsigned char scaler_get_VoTrackingI3DDMA_frequency_update_in_isr(void);
void set_force_hdmi_hdr_flow_enable(UINT8 enable);
UINT8 get_force_hdmi_hdr_flow_enable(unsigned char display);
bool is_game_mode_set_line_buffer(void);
void scaler_set_I2D_dly_ratio(void);

void scaler_set_VRR_pwm_frequency_update_in_isr(unsigned char bEnable);
unsigned char scaler_get_VRR_pwm_frequency_update_in_isr(void);
void scaler_pwm_frequency_update_by_dvs(void);
unsigned int scaler_get_I2D_tracking_phase_error_threadhold(void);
unsigned int scaler_get_idomain_vfreq_pre(void);
unsigned int rtk_scaler_get_lowdelay_value(void);
unsigned int rtk_scaler_calc_current_video_path_latency(void);
void scaler_Setdualdecoder_notchange(unsigned char flag);
void  rtk_hal_vsc_Setdualdecoder_run(unsigned char flag);
unsigned char rtk_hal_vsc_Getdualdecoder_run(void);
unsigned char rtk_hal_vsc_Getdualdecoder(void);
void drv_set_vrr_mode_dynamic(unsigned char enable);
unsigned char drv_get_vrr_mode_dynamic(void);
void drv_set_freesync_mode_dynamic(unsigned char enable);
unsigned char drv_get_freesync_mode_dynamic(void);
void drv_set_vrr_low_delay_mode_dynamic(unsigned char enable);
unsigned char drv_get_vrr_low_delay_mode_dynamic(void);
unsigned char scaler_get_current_framesync_mode(unsigned char display);
void scaler_set_full_gatting_rpc(unsigned int enable);
void Scaler_Set_Display_Timing_Mode(unsigned char timing_mode);
unsigned char Scaler_Get_Display_Timing_Mode(void);
unsigned int rtk_hal_vsc_scaler_check_video_delay(VIDEO_WID_T wid, unsigned int *current_video_delay);
unsigned int Scaler_Get_HDMI_VFREQ(unsigned int orig_v_freq);
unsigned int Scaler_Get_HDMI_HFREQ(unsigned int orig_h_freq);
void scaler_set_force_update_video_latency(unsigned int bOnOff);
unsigned int scaler_get_force_update_video_latency(void);
UINT8 scaler_vsc_get_gamemode_go_datafrc_mode(void);
void scaler_vsc_set_gamemode_go_datafrc_mode(UINT8 bOnOff);
void scaler_vsc_set_gamemode_go_datafs_mode(bool bOnOff);
bool scaler_vsc_get_gamemode_go_datafs_mode(void);
bool is_game_mode_set_line_buffer(void);
bool get_support_vo_force_v_top(unsigned char display, unsigned int func_flag);
void hdr_crt_ctrl(bool enable);

unsigned char scaler_HDMI_signal_error_handler(unsigned char display);
unsigned char scaler_VDEC_signal_error_handler(unsigned char display);
#ifdef CONFIG_RTK_8KCODEC_INTERFACE
unsigned char send_srnn_real_size_to_slave(unsigned int ulWid, unsigned int ulLen);
#endif
void drv_update_game_mode_frc_fs_flag(void);
void check_set_regen_vsync_at_hdr1(void);
void vo_set_force_framerate_control(unsigned int framerate);
unsigned char rtk_hal_vsc_SetOutputRegion(VIDEO_WID_T wid, KADP_VIDEO_RECT_T outregion, unsigned short Wide, unsigned short High);
unsigned char judge_scaler_run_i3ddma_vo_path(void);
UINT8 get_force_hdmi_hdr_flow_enable(unsigned char display);

#ifdef CONFIG_DYNAMIC_PANEL_SELECT
void dlg_listen_handler(void);
void dlg_callback_handler(DLG_CALLBACK_TYPE eType);
#endif

unsigned char rtk_hal_vsc_SetInputRegion_OutputRegion(KADP_VIDEO_WID_T wid, KADP_VSC_ROTATE_T rotate_type, KADP_VIDEO_RECT_T  inregion,
  KADP_VIDEO_RECT_T originalInput, KADP_VIDEO_RECT_T outregion, unsigned char null_input, unsigned char null_output);

void modestate_decide_rtnr(void);
void modestate_setup_idomain(StructDisplayInfo* info);
void Set_Val_cur_main_vsc_src_num(unsigned char val);
void Set_Val_cur_sub_vsc_src_num(unsigned char val);
void set_current_scaler_source_num(unsigned char display);
void Set_Val_source_connect_verify_num(unsigned char dispaly, unsigned char val);
void Set_vsc_input_src_info(unsigned char display,KADP_VSC_OUTPUT_MODE_T vsc_output_mode, KADP_VSC_INPUT_SRC_INFO_T source_info);
bool Get_Val_VSCModuleInitDone(void);

unsigned char rtk_hal_vsc_initialize(void);
unsigned char rtk_hal_vsc_uninitialize(void);
unsigned char rtk_hal_vsc_open(VIDEO_WID_T wid);
unsigned char rtk_hal_vsc_close(VIDEO_WID_T wid);
unsigned char rtk_hal_vsc_Connect(VIDEO_WID_T wid, KADP_VSC_INPUT_SRC_INFO_T inputSrcInfo, KADP_VSC_OUTPUT_MODE_T outputMode);
unsigned char rtk_hal_vsc_Disconnect(VIDEO_WID_T wid, KADP_VSC_INPUT_SRC_INFO_T inputSrcInfo, KADP_VSC_OUTPUT_MODE_T outputMode);
unsigned char rtk_hal_vsc_SetInputRegion(KADP_VIDEO_WID_T wid, KADP_VIDEO_RECT_T  inregion);
unsigned char rtk_hal_vsc_SetWinBlank(VIDEO_WID_T wid, bool bonoff, KADP_VIDEO_DDI_WIN_COLOR_T color);
unsigned char rtk_output_connect(unsigned char display);
unsigned char vdo_connect(unsigned char display, unsigned char vdec_port);
unsigned char vdo_disconnect(unsigned char display, unsigned char vdec_port);
unsigned char check_hdmi_need_rerun_condition(unsigned char *unstable_hander);//check rerun case
unsigned char check_dp_need_rerun_condition(unsigned char *unstable_hander);//check rerun case
void Trigger_ratio_smooth_toggle(unsigned char display);
void Set_Val_bQsmode(unsigned char val);
void QS_VSCInit_SyncVOInfo(void);

extern unsigned char (*scaler_force_run_idma)(unsigned char, VSC_INPUT_TYPE_T);
extern unsigned char (*get_force_i3ddma_enable)(unsigned char);
extern unsigned char (*get_displayinfo_proc)(unsigned char, VSC_INPUT_TYPE_T);
extern unsigned char (*init_source_proc)(unsigned char display, VSC_INPUT_TYPE_T sourcetype, unsigned char sourceport);
extern unsigned char (*rtk_run_scaler)(unsigned char display, KADP_VSC_INPUT_TYPE_T inputType, KADP_VSC_OUTPUT_MODE_T outputMode);
extern unsigned char (*Scaler_get_data_framesync)(unsigned char display);
extern unsigned char (*Setup_IMD)(StructDisplayInfo* info);
extern unsigned char (*modestate_set_framesync)(StructDisplayInfo* info);
extern unsigned char (*judge_scaler_break_case)(unsigned char display);
extern void (*reset_rotate_mode)(unsigned char display);
extern void (*reset_hdr_mode)(unsigned char display);
extern long (*Scaler_Send_VoDisconnect_VOMDA)(KADP_VO_VSC_SRC_TYPE_T type, unsigned char wid_port);
extern void (*Scaler_SetFreeze)(unsigned char channel, unsigned char mode);
extern unsigned int (*Get_Force_run_fake_scaler_State)(void);
extern unsigned int (*Get_fake_scaler_connect)(void);
extern unsigned int (*Get_fake_scaler_on_going)(void);

void Set_Force_run_fake_scaler_State(unsigned int state, unsigned int lowdelay, unsigned int dolby);
void Set_fake_scaler_lowdelay(unsigned int enable);
unsigned int Get_fake_scaler_lowdely_impl(void);
void Set_fake_scaler_connect(unsigned int enable);
unsigned int Get_fake_scaler_connect_impl(void);
unsigned char rtk_hal_vsc_Fake_run_scaler(VSC_FAKE_SCALER_PARA_T FakePara);
unsigned int Get_Force_run_fake_scaler_State_impl(void);
unsigned int Get_fake_scaler_on_going_impl(void);
void Set_fake_scaler_on_going(unsigned int enable);
unsigned char get_multiview_enable(void);
void set_multiview_enable(unsigned char enable);
unsigned char vbe_disp_get_multiview_flag(void);
void vbe_disp_set_multiview_flag(unsigned char enable);
unsigned char get_new_game_mode_small_window_condition(void);
void drv_update_game_mode_frc_fs_flag(void);
unsigned char ScalerCheckOrbitFinish(void);

void set_video_delay_type(VIDEO_DELAY_TYPE res);
VIDEO_DELAY_TYPE get_video_delay_type(void);
unsigned char decide_video_delay_type(void);
void set_ap_video_delay_number(VIDEO_WID_T wId, UINT8 buffer);
unsigned char get_ap_video_delay_number(void);
unsigned char get_scaler_video_delay_number_pre(void);
void set_scaler_video_delay_number_pre(unsigned char val);
unsigned char get_scaler_video_delay_number(void);
void set_scaler_video_delay_number(unsigned char val);
void disable_videodelay(void);
unsigned char judge_source_support_video_delay(VSC_INPUT_TYPE_T srctype);
void Scaler_QsSetHdrType(void);
void quickshow_set_pc_mode(VSC_INPUT_TYPE_T inputType);
void quickshow_set_low_latency_mode(void);
void Scaler_SET_VSCDispinfo_With_NEW_HDMI_TIMING_INFO(unsigned char display, MEASURE_TIMING_T vfehdmitiminginfo);
void set_force_dolby_vision_mode(bool enable);
bool is_force_dolby_vision(void);
int vsc_ioctrl_operate_extend_handle(unsigned long arg);
void reset_hdrtype_change_counter(void);
unsigned char vbe_disp_get_VRR_timingMode_flag(void);
unsigned char vbe_disp_get_freesync_mode_flag(void);
unsigned char get_sub_OutputVencMode(void);
unsigned char Get_vscFilmMode(void);
void filmmode_videofw_config(unsigned int filmmode, unsigned int buf4addr);
#endif

