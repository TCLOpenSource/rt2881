#ifndef _RTK_HDMI_DP_COMMON_H_
#define _RTK_HDMI_DP_COMMON_H_


typedef enum {
    HDMI_2p0 = 0,
    HDMI_2p1,
    DP_1p4,
    DP_2p0,
    HDMI_2p0_DSCD,
    HDMI_2p1_DSCD,
    DP_1p4_DSCD,
    DP_2p0_DSCD,
}VIDEO_COMMON_SOURCE_TYPE;

typedef enum {
    VIDEO_COMMON_HDMIRX = 0,
    VIDEO_COMMON_DPRX,
}VIDEO_COMMON_PORT_IN_SEL;

typedef enum
{
    VIDEO_COMMON_0 = 0,
    VIDEO_COMMON_1,
    VIDEO_COMMON_2,
    VIDEO_COMMON_3,
	VIDEO_COMMON_NONE,
}VIDEO_COMMON_PORT;

typedef enum
{
    COLOR_SPACE_RGB = 0,
    COLOR_SPACE_YUV422,
    COLOR_SPACE_YUV444,
    COLOR_SPACE_YUV420,
    COLOR_SPACE_YONLY,
    COLOR_SPACE_UNKNOWN,
}VIDEO_COMMON_COLOR_DEPTH_INPUT;

typedef enum {
    VIDEO_COMMON_DSCD_0 = 0,
    VIDEO_COMMON_DSCD_1 = 1,
    VIDEO_COMMON_DSCD_NONE,
}VIDEO_COMMON_DSCD_IN_SEL;

typedef enum {
    AUDIO_SOURCE_FROM_HDMIRX = 0,
    AUDIO_SOURCE_FROM_DPRX = 1,
} COMMON_AUDIO_SRC_SEL_E;

typedef enum {
    VIDEO_COMMON_1P_In = 0,
    VIDEO_COMMON_2P_In = 1,
}VIDEO_COMMON_IN_PIXEL_MODE;

typedef enum {
    PORT0_MAC_BYPASS_TO_HDMITX = 0,
    PORT1_MAC_BYPASS_TO_HDMITX = 1,
    PORT2_MAC_BYPASS_TO_HDMITX = 2,
    PORT3_MAC_BYPASS_TO_HDMITX = 3,
    PORT0_COMMON_BYPASS_TO_HDMITX = 4,
    PORT1_COMMON_BYPASS_TO_HDMITX = 5,
    PORT2_COMMON_BYPASS_TO_HDMITX = 6,
    PORT3_COMMON_BYPASS_TO_HDMITX = 7,
} COMMON_PORT_BYPASS_TO_HDMITX_E;

typedef enum {
    BYPASS_ORIGINAL_422_DATA = 0,
    CHANGE_TO_422_RAW_DATA = 1,
} COMMON_BYPASS_YUV422_RAW_SEL_E;




#if  defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
/*********************************************************************************************
*   video and offms path sel
*
*********************************************************************************************/
extern void rtk_video_common_crt_on(void);

extern unsigned char rtk_video_check_common_ownership(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch);

extern void rtk_video_common_set_ownership(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, VIDEO_COMMON_PORT vc_port);

extern void rtk_video_common_set_input_source(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, VIDEO_COMMON_SOURCE_TYPE src_type);

extern void rtk_video_common_set_color_space(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, VIDEO_COMMON_COLOR_DEPTH_INPUT color_space);

extern void rtk_video_common_set_dprx_pixel_mode(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, VIDEO_COMMON_IN_PIXEL_MODE pixel_mode);

extern void rtk_video_common_bypass_mode(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, COMMON_PORT_BYPASS_TO_HDMITX_E bypass_port);

extern void rtk_video_common_offms_sel(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, VIDEO_COMMON_PORT common_port);

extern void rtk_video_common_yuv422_raw_data_mode(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, COMMON_BYPASS_YUV422_RAW_SEL_E yuv422_raw);

extern void rtk_video_common_set_output_pixel_mode(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, VIDEO_COMMON_IN_PIXEL_MODE pixel_mode);

extern COMMON_PORT_BYPASS_TO_HDMITX_E rtk_video_common_get_bypass_port(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch);

extern void rtk_video_common_set_output_clk_div(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, unsigned char enable);

extern void rtk_video_common_set_hs_inverse(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, unsigned char enable);

extern void rtk_video_common_set_vs_inverse(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, unsigned char enable);

extern void rtk_video_common_set_line_drop(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, unsigned char enable);

#else
#define rtk_video_common_crt_on(...)
#define rtk_video_check_common_ownership(...)                       (0)
#define rtk_video_common_set_ownership(...)
#define rtk_video_common_set_input_source(...)
#define rtk_video_common_set_color_space(...)
#define rtk_video_common_set_dprx_pixel_mode(...)
#define rtk_video_common_bypass_mode(...)
#define rtk_video_common_offms_sel(...)
#define rtk_video_common_yuv422_raw_data_mode(...)
#define rtk_video_common_get_bypass_port(...)                       (0xF)
#define rtk_video_common_set_output_pixel_mode(...)
#define rtk_video_common_set_hs_inverse(...)
#define rtk_video_common_set_vs_inverse(...)
#define rtk_video_common_set_line_drop(...);
#endif
#endif
