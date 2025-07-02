#ifndef __RTK_MODULE_ANDROID_ANDROID_DEFINE_H__
#define __RTK_MODULE_ANDROID_ANDROID_DEFINE_H__
#include <fw/android/event_list-karaoke.h>
#include <fw/android/event_list-miracast.h>

#define ANDROID_MODULE_VALUE_MIRACAST 0x20

typedef enum{
    ANDROID_MODULE_ID_MIRACAST,
    ANDROID_KARAOKE_MODULE_ID,
    ANDROID_MODULE_MAX
}ANDROID_MODULE_ID;

#endif /* __RTK_MODULE_ANDROID_ANDROID_DEFINE_H__ */