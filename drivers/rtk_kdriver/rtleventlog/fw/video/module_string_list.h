
#ifndef __RTK_MODULE_EVENT_VIDEO_STRING_DEFINE_H__
#define __RTK_MODULE_EVENT_VIDEO_STRING_DEFINE_H__
#include <fw/video/module_list.h>
#include <fw/video/event_string_list-vdec.h>
/*
######################################################################################
# MODULE STRING DEFINE
######################################################################################
*/
struct event_module_s video_module[]={
    {VIDEO_VDEC_MODULE_ID, "vdec", video_vdec_eventtype},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_EVENT_VIDEO_STRING_DEFINE_H__ */
