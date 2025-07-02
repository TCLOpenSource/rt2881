#ifndef __RTK_MODULE_EVENT_VDEC_STRING_H__
#define __RTK_MODULE_EVENT_VDEC_STRING_H__
/*
######################################################################################
# VDEC EVENT STRING DEFINE
######################################################################################
*/
static struct event_event_s video_vdec_event_event[]={
    {VIDEO_VDEC_EVENT_TEST_EVENT_ID, "test"},
    {EVENT_LIMIT_MAX,NULL},
};

/*
######################################################################################
# VDEC EVENT TYPE STRING DEFINE
######################################################################################
*/
struct event_eventtype_s video_vdec_eventtype[]={
    {VIDEO_VDEC_EVENT_TYPE_ID, "event", video_vdec_event_event},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_EVENT_VDEC_STRING_H__ */

