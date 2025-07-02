#ifndef __RTK_HDMI_DP_COMMON_DEBUG_H__
#define __RTK_HDMI_DP_COMMON_DEBUG_H__

#include <rtd_log/rtd_module_log.h>

#define RTK_VIDEO_COMMON_INFO(fmt, args...)      rtd_pr_video_common_info("[Info] " fmt, ## args)
#define RTK_VIDEO_COMMON_WARNING(fmt, args...)   rtd_pr_video_common_warn("[Warn] " fmt, ## args)
#define RTK_VIDEO_COMMON_ERR(fmt, args...)       rtd_pr_video_common_err("[Err] " fmt, ## args)
#define RTK_VIDEO_COMMON_ALERT(fmt, args...)     rtd_pr_video_common_alert("[Alert] " fmt, ## args)

#endif // __RTK_HDMI_DP_COMMON_DEBUG_H__

#include <rtd_log/rtd_module_log.h>