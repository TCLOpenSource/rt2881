#ifndef __RTK_MODULE_KERNEL_BLUETOOTH_H__
#define __RTK_MODULE_KERNEL_BLUETOOTH_H__
/*
######################################################################################
# BLUETOOTH EVENT DEFINE
######################################################################################
*/
typedef enum
{
    KERNEL_BLUETOOTH_DEBUG_TIME_PROFILE_EVENT_ID,
    KERNEL_BLUETOOTH_DEBUG_EVENT_MAX
}KERNEL_BLUETOOTH_DEBUG_EVENT_ID;

/*
######################################################################################
# BLUETOOTH EVENT TYPE DEFINE
######################################################################################
*/
typedef enum
{
    KERNEL_BLUETOOTH_DEBUG_TYPE_ID,
    KERNEL_BLUETOOTH_TYPE_MAX
}KERNEL_BLUETOOTH_TYPE_ID;

/*
######################################################################################
# BLUETOOTH EVENT MERGER DW1 DEFINE
######################################################################################
*/
#define   KERNEL_BLUETOOTH_DW1_debug_time_profile    MERGER_EVENT_DW1(EVENT_FW_KERNEL, KERNEL_BLUETOOTH_MODULE_ID, KERNEL_BLUETOOTH_DEBUG_TYPE_ID, KERNEL_BLUETOOTH_DEBUG_TIME_PROFILE_EVENT_ID)

#define rtd_kernel_bluetooth_event_log(type, event, event_val, module_reserved)     rtd_kernel_event_log(type, KERNEL_BLUETOOTH_MODULE_ID, event, event_val, module_reserved)
#endif /* __RTK_MODULE_KERNEL_BLUETOOTH_H__ */

