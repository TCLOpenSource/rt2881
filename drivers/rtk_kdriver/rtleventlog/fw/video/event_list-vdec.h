#ifndef __RTK_MODULE_EVENT_VDEC_H__
#define __RTK_MODULE_EVENT_VDEC_H__
/*
######################################################################################
# VDEC EVENT DEFINE
######################################################################################
*/
typedef enum
{
    VIDEO_VDEC_EVENT_TEST_EVENT_ID,
    VIDEO_VDEC_EVENT_EVENT_MAX
}VIDEO_VDEC_EVENT_EVENT_ID;

/*
######################################################################################
# VDEC EVENT TYPE DEFINE
######################################################################################
*/
typedef enum
{
    VIDEO_VDEC_EVENT_TYPE_ID,
    VIDEO_VDEC_TYPE_MAX
}VIDEO_VDEC_TYPE_ID;

/*
######################################################################################
# VDEC EVENT MERGER DW1 DEFINE
######################################################################################
*/
#define   VDEC_DW1_event_test    MERGER_EVENT_DW1(EVENT_FW_VIDEO, VIDEO_VDEC_MODULE_ID, VIDEO_VDEC_EVENT_type_ID, VIDEO_VDEC_EVENT_TEST_EVENT_ID)

#define rtd_vdec_event_log(type, event, event_val, module_reserved)     rtd_video_event_log(type, VIDEO_VDEC_MODULE_ID, event, event_val, module_reserved)
#endif /* __RTK_MODULE_EVENT_VDEC_H__ */

