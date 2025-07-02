#include <linux/types.h>
#include <rtd_types.h>
#include <rtd_log/rtd_module_log.h>
#include <ioctrl/scaler/vfe_cmd_id.h>
#include <ioctrl/scaler/vsc_cmd_id.h>
#include <ioctrl/scaler/vbe_cmd_id.h>
#include <ioctrl/scaler/vfe_cmd_id-dprx.h>
#include <ioctrl/audio/audio_cmd_id.h>
#include <ioctrl/quick_show/quick_show_cmd_id.h>
#include <rtk_kdriver/dprx/dprx_vfe_api.h>
#include <rtk_kdriver/dprx/dprxfun.h>
#include <rtk_kdriver/quick_show/quick_show.h>

#include "./../../../audio/hal_audio.h"

#define HDMI_MAX_PORT (4)
#define DP_MAX_PORT (2)

/* Scaler API */
extern int sm_vsc_init(void);
extern int sm_vsc_connect(VSC_CONNECT_PARA_T connectPara);
extern int sm_vsc_disconnect(VSC_CONNECT_PARA_T disconnectPara);
extern int sm_vsc_set_output_region(VSC_SET_FRAME_REGION_T frameregion);
extern int sm_vsc_set_input_region(VSC_SET_FRAME_REGION_T frameregion);
extern int sm_vsc_set_win_blank(VSC_WINBLANK_PARA_T winblankpara);
extern int sm_vsc_open(VIDEO_WID_T wid);
extern atomic_t *sm_state_vfe;
extern atomic_t *sm_state_vsc;

/* Panel API */
extern unsigned char HAL_VBE_DISP_Initialize(KADP_DISP_PANEL_INFO_T panelInfo);
extern void HAL_VBE_DISP_GetPanelSize(KADP_DISP_PANEL_SIZE_T *panel_size);

/* HDMI API */
extern int sm_vfe_qs_hdmi_init(void);
extern int sm_vfe_hdmi_init(void);
extern int sm_vfe_hdmi_open(void);
extern int sm_vfe_hdmi_close(void);
extern int sm_vfe_hdmi_connect(unsigned char port);
extern int sm_vfe_hdmi_get_timing_info(vfe_hdmi_timing_info_t *hdmitiminginfo);
extern int sm_vfe_hdmi_disconnect(int port);
extern int vfe_hdmi_drv_get_connection_state(vfe_hdmi_connect_state_t *con_state);
extern int vfe_hdmi_drv_get_drm_info(vfe_hdmi_drm_t *info_frame);

/* DP API */
extern void sm_vfe_dprx_get_timing_info(KADP_VFE_DPRX_TIMING_INFO_T *ret_timing_info, int port);
extern int dprx_vfe_ctrl_handler(unsigned long arg);
extern int vfe_dprx_drv_get_connection_state(unsigned char ch, unsigned char *p_connect_state);
extern int vfe_dprx_drv_enable_hpd(unsigned char on);

/* PQ API */
extern void sm_vpq_extern_init(void);

/* Audio API */
extern DTV_STATUS_T rhal_set_ioctl(HAL_AUDIO_RHAL_INFO_T rhalinfo);

/* RMM API */
extern void do_dvr_reclaim(int cmd);

/* IR API */
extern void set_QS_handle_key(bool enable);

enum SM_SOURCE {
    NONE,
    HDMI_SOURCE,
    DP_SOURCE
};

struct sm_source_info {
    unsigned int port;
    enum SM_SOURCE source;
};
