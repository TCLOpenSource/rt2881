#ifndef __RTK_VIDEO_COMMON_PRIV_H__
#define __RTK_VIDEO_COMMON_PRIV_H__

#include "rtk_video_common-config.h"
#include "rtk_video_common-osal.h"
#include "rtk_video_common-debug.h"
#include <rtk_kdriver/video_common/rtk_video_common.h>

// UI Channel Define
#define HDMI_DP_CHANNLE_PORT_MAX        4 

typedef struct
{
    VIDEO_COMMON_PORT_IN_SEL   src_in;
    VIDEO_COMMON_SOURCE_TYPE   src_type;
    unsigned char              ch;
    VIDEO_COMMON_DSCD_IN_SEL   dscd_in;

}HDMI_DPRX_PORT_INSTANCE;

extern HDMI_DPRX_PORT_INSTANCE g_vc_port_instance[HDMI_DP_CHANNLE_PORT_MAX];

/*---------VDIEO_COMMON Function--------*/
extern void lib_video_common_crt_all_enable(void);
extern void lib_video_common_crt_enable(unsigned char nport);

extern void lib_video_common_set_hdmi_dp_sel(unsigned char nport, VIDEO_COMMON_PORT_IN_SEL mux);
extern unsigned char lib_video_common_get_hdmi_dp_sel(unsigned char nport);

extern void lib_video_common_set_color_space_input(unsigned char nport, VIDEO_COMMON_COLOR_DEPTH_INPUT color_space);
extern void lib_video_common_set_yuv422_repeat_mode(unsigned char nport, unsigned char enable);
extern void lib_video_common_input_source_sel(unsigned char nport, VIDEO_COMMON_SOURCE_TYPE src);
extern void lib_video_common_dscd_in_sel(unsigned char nport, VIDEO_COMMON_DSCD_IN_SEL dscd_src);
extern void lib_video_common_audio_in_sel(unsigned char nport, VIDEO_COMMON_SOURCE_TYPE src);
extern void lib_video_common_offms_out_sel(unsigned char nport, VIDEO_COMMON_SOURCE_TYPE src);
extern void lib_video_common_set_dprx_pixel_mode_in_sel(unsigned char nport, VIDEO_COMMON_IN_PIXEL_MODE pixel_mode);
extern void lib_video_common_set_output_pixel_mode(unsigned char nport, VIDEO_COMMON_IN_PIXEL_MODE pixel_mode);
extern void lib_video_common_set_dscd_dp_source_clk(unsigned char nport, VIDEO_COMMON_DSCD_IN_SEL dscd_sel, VIDEO_COMMON_IN_PIXEL_MODE  pixel_mode);
extern void lib_video_common_support_clk_gate(unsigned char nport, unsigned char enable);
extern void lib_video_common_set_video_common_clk_sel(unsigned char nport, unsigned char enable);
extern void lib_video_common_set_hs_inverse(unsigned char nport, unsigned char enable) ;
extern void lib_video_common_set_vs_inverse(unsigned char nport, unsigned char enable) ;
extern void lib_video_common_set_line_drop(unsigned char nport, unsigned char enable) ;
/*---------VDIEO_COMMON MISC Function--------*/
extern void lib_video_common_bypass_mode_sel(COMMON_PORT_BYPASS_TO_HDMITX_E bypass_port);
extern void lib_video_common_offms_sel(VIDEO_COMMON_PORT common_port);
extern void lib_video_common_yuv422_raw_data_sel(COMMON_BYPASS_YUV422_RAW_SEL_E yuv422_raw);
extern COMMON_PORT_BYPASS_TO_HDMITX_E lib_video_common_get_bypass_port(void);

#endif // __RTK_VIDEO_COMMON_PRIV_H__
