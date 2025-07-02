#ifndef __RTK_EVENT_VIDEO_H__
#define __RTK_EVENT_VIDEO_H__

#include "VP_Utilities.h"
#include "rtio.h"

/*
######################################################################################
# MAX DEFINE
######################################################################################
*/
#define FW_MAX                      16
#define MODULE_MAX                 256
#define EVENT_MAX                  256
#define EVENT_TYPE_MAX              16
#define EVENT_LIMIT_MAX            256

#define EVENT_SYNCBYTE            0x47
#define EVENT_FW_VIDEO            0x05
#define EVENT_ERROR                  1
#define EVENT_OK                     0

#define LOG_SIZE                0x2000
#define INT_LOG_SIZE        LOG_SIZE/4 //(4M size)

/*
######################################################################################
# STRUCT DEFINE
######################################################################################
*/
struct event_format {
    /* DW1 */
    unsigned char syncbyte;
    unsigned char fw_type : 4;
    unsigned char event_type : 4;
    unsigned char module;
    unsigned char event;
    /* DW2 */
    unsigned int event_value;
    /* DW3 */
    unsigned int module_reserved;
    /* DW4 */
    unsigned int timestamp;
};

/*
######################################################################################
# STATIC FUNCTION
######################################################################################
*/
unsigned int rtk_timer_misc_90k_us (void);
unsigned int rtk_timer_misc_90k_ms (void);
unsigned int event_log_save        (unsigned int fw_type, unsigned int event_type, unsigned int module, unsigned int event, unsigned int event_val, unsigned int module_reserved);
void         eventlog_printf       (unsigned int DW1,unsigned int DW2, unsigned int DW3 ,unsigned int DW4);
void         eventlog_printf_all   (void);

/*
######################################################################################
# Define
######################################################################################
*/
#define rtd_video_event_log(event_type, module, event, event_val, module_reserved)    event_log_save(EVENT_FW_VIDEO, event_type, module, event, event_val, module_reserved)

#endif /* __RTK_EVENT_UBOOT_H__ */