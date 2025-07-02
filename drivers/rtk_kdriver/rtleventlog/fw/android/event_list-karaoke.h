#ifndef __RTK_MODULE_ANDROID_KARAOKE_H__
#define __RTK_MODULE_ANDROID_KARAOKE_H__
/*
######################################################################################
# KARAOKE EVENT DEFINE
######################################################################################
*/
typedef enum
{
    ANDROID_KARAOKE_FILTER_WPRP_EVENT_ID,
    ANDROID_KARAOKE_FILTER_EVENT_MAX
}ANDROID_KARAOKE_FILTER_EVENT_ID;

typedef enum
{
    ANDROID_KARAOKE_APK_BLE_EVENT_ID,
    ANDROID_KARAOKE_APK_USB_EVENT_ID,
    ANDROID_KARAOKE_APK_EVENT_MAX
}ANDROID_KARAOKE_APK_EVENT_ID;

typedef enum
{
    ANDROID_KARAOKE_USB_TIME_EVENT_ID,
    ANDROID_KARAOKE_USB_EVENT_MAX
}ANDROID_KARAOKE_USB_EVENT_ID;

/*
######################################################################################
# KARAOKE EVENT TYPE DEFINE
######################################################################################
*/
typedef enum
{
    ANDROID_KARAOKE_USB_TYPE_ID,
    ANDROID_KARAOKE_APK_TYPE_ID,
    ANDROID_KARAOKE_FILTER_TYPE_ID,
    ANDROID_KARAOKE_TYPE_MAX
}ANDROID_KARAOKE_TYPE_ID;

/*
######################################################################################
# KARAOKE EVENT MERGER DW1 DEFINE
######################################################################################
*/
#define   ANDROID_KARAOKE_DW1_filter_wprp    MERGER_EVENT_DW1(EVENT_FW_ANDROID, ANDROID_KARAOKE_MODULE_ID, ANDROID_KARAOKE_FILTER_TYPE_ID, ANDROID_KARAOKE_FILTER_WPRP_EVENT_ID)
#define   ANDROID_KARAOKE_DW1_apk_usb    MERGER_EVENT_DW1(EVENT_FW_ANDROID, ANDROID_KARAOKE_MODULE_ID, ANDROID_KARAOKE_APK_TYPE_ID, ANDROID_KARAOKE_APK_USB_EVENT_ID)
#define   ANDROID_KARAOKE_DW1_apk_ble    MERGER_EVENT_DW1(EVENT_FW_ANDROID, ANDROID_KARAOKE_MODULE_ID, ANDROID_KARAOKE_APK_TYPE_ID, ANDROID_KARAOKE_APK_BLE_EVENT_ID)
#define   ANDROID_KARAOKE_DW1_usb_time    MERGER_EVENT_DW1(EVENT_FW_ANDROID, ANDROID_KARAOKE_MODULE_ID, ANDROID_KARAOKE_USB_TYPE_ID, ANDROID_KARAOKE_USB_TIME_EVENT_ID)

#define rtd_android_karaoke_event_log(type, event, event_val, module_reserved)     rtd_android_event_log(type, ANDROID_KARAOKE_MODULE_ID, event, event_val, module_reserved)
#endif /* __RTK_MODULE_ANDROID_KARAOKE_H__ */

