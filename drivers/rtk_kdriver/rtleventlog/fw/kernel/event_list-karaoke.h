#ifndef __RTK_MODULE_KERNEL_KARAOKE_H__
#define __RTK_MODULE_KERNEL_KARAOKE_H__
/*
######################################################################################
# KARAOKE EVENT DEFINE
######################################################################################
*/
typedef enum
{
    KERNEL_KARAOKE_USB_TIME_EVENT_ID,
    KERNEL_KARAOKE_USB_EVENT_MAX
}KERNEL_KARAOKE_USB_EVENT_ID;

/*
######################################################################################
# KARAOKE EVENT TYPE DEFINE
######################################################################################
*/
typedef enum
{
    KERNEL_KARAOKE_USB_TYPE_ID,
    KERNEL_KARAOKE_TYPE_MAX
}KERNEL_KARAOKE_TYPE_ID;

/*
######################################################################################
# KARAOKE EVENT MERGER DW1 DEFINE
######################################################################################
*/
#define   KERNEL_KARAOKE_DW1_usb_time    MERGER_EVENT_DW1(EVENT_FW_KERNEL, KERNEL_KARAOKE_MODULE_ID, KERNEL_KARAOKE_USB_TYPE_ID, KERNEL_KARAOKE_USB_TIME_EVENT_ID)

#define rtd_kernel_karaoke_event_log(type, event, event_val, module_reserved)     rtd_kernel_event_log(type, KERNEL_KARAOKE_MODULE_ID, event, event_val, module_reserved)
#endif /* __RTK_MODULE_KERNEL_KARAOKE_H__ */

