#ifndef __RTK_MODULE_ANDROID_KARAOKE_STRING_H__
#define __RTK_MODULE_ANDROID_KARAOKE_STRING_H__
/*
######################################################################################
# KARAOKE EVENT STRING DEFINE
######################################################################################
*/
static struct event_event_s android_karaoke_filter_event[]={
    {ANDROID_KARAOKE_FILTER_WPRP_EVENT_ID, "wprp"},
    {EVENT_LIMIT_MAX,NULL},
};

static struct event_event_s android_karaoke_apk_event[]={
    {ANDROID_KARAOKE_APK_USB_EVENT_ID, "usb"},
    {ANDROID_KARAOKE_APK_BLE_EVENT_ID, "ble"},
    {EVENT_LIMIT_MAX,NULL},
};

static struct event_event_s android_karaoke_usb_event[]={
    {ANDROID_KARAOKE_USB_TIME_EVENT_ID, "time"},
    {EVENT_LIMIT_MAX,NULL},
};

/*
######################################################################################
# KARAOKE EVENT TYPE STRING DEFINE
######################################################################################
*/
struct event_eventtype_s android_karaoke_eventtype[]={
    {ANDROID_KARAOKE_FILTER_TYPE_ID, "filter", android_karaoke_filter_event},
    {ANDROID_KARAOKE_APK_TYPE_ID, "apk", android_karaoke_apk_event},
    {ANDROID_KARAOKE_USB_TYPE_ID, "usb", android_karaoke_usb_event},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_ANDROID_KARAOKE_STRING_H__ */

