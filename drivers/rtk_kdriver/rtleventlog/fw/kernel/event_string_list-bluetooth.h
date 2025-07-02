#ifndef __RTK_MODULE_KERNEL_BLUETOOTH_STRING_H__
#define __RTK_MODULE_KERNEL_BLUETOOTH_STRING_H__
/*
######################################################################################
# BLUETOOTH EVENT STRING DEFINE
######################################################################################
*/
static struct event_event_s kernel_bluetooth_debug_event[]={
    {KERNEL_BLUETOOTH_DEBUG_TIME_PROFILE_EVENT_ID, "time_profile"},
    {EVENT_LIMIT_MAX,NULL},
};

/*
######################################################################################
# BLUETOOTH EVENT TYPE STRING DEFINE
######################################################################################
*/
struct event_eventtype_s kernel_bluetooth_eventtype[]={
    {KERNEL_BLUETOOTH_DEBUG_TYPE_ID, "debug", kernel_bluetooth_debug_event},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_KERNEL_BLUETOOTH_STRING_H__ */

