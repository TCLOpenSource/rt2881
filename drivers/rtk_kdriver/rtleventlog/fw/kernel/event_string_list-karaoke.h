#ifndef __RTK_MODULE_KERNEL_KARAOKE_STRING_H__
#define __RTK_MODULE_KERNEL_KARAOKE_STRING_H__
/*
######################################################################################
# KARAOKE EVENT STRING DEFINE
######################################################################################
*/
static struct event_event_s kernel_karaoke_usb_event[]={
    {KERNEL_KARAOKE_USB_TIME_EVENT_ID, "time"},
    {EVENT_LIMIT_MAX,NULL},
};

/*
######################################################################################
# KARAOKE EVENT TYPE STRING DEFINE
######################################################################################
*/
struct event_eventtype_s kernel_karaoke_eventtype[]={
    {KERNEL_KARAOKE_USB_TYPE_ID, "usb", kernel_karaoke_usb_event},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_KERNEL_KARAOKE_STRING_H__ */

