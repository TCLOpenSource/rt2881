#ifndef __RTK_USB_DEVICE_DBG_H__
#define __RTK_USB_DEVICE_DBG_H__

#include <rtd_log/rtd_module_log.h>

// debug log don't have the color by default.  If you want to use color, please add following color tag to the log string.
#define NONE            "\033[m"
#define RED             "\033[0;32;31m"
#define LIGHT_RED       "\033[1;31m"
#define GREEN           "\033[0;32;32m"
#define LIGHT_GREEN     "\033[1;32m"
#define BLUE            "\033[0;32;34m"
#define LIGHT_BLUE      "\033[1;34m"
#define DARY_GRAY       "\033[1;30m"
#define CYAN            "\033[0;36m"
#define LIGHT_CYAN      "\033[1;36m"
#define PURPLE          "\033[0;35m"
#define LIGHT_PURPLE    "\033[1;35m"
#define BROWN           "\033[0;33m"
#define YELLOW          "\033[1;33m"
#define LIGHT_GRAY      "\033[0;37m"
#define WHITE           "\033[1;37m"

#ifdef TAG_NAME
#undef TAG_NAME
#endif

#define TAG_NAME "RTK_USB_DEVICE"

extern unsigned long rtk_usb_device_dbg_log_level_mask;
extern unsigned long rtk_usb_device_dbg_log_block_mask;
extern UINT32 rtk_usb_device_log_onoff;

#define RTK_USB_DEVICE_LOG_MASK_NONE   0
#define RTK_USB_DEVICE_LOG_MASK_BIT_0  1 << 0
#define RTK_USB_DEVICE_LOG_MASK_BIT_1  1 << 1
#define RTK_USB_DEVICE_LOG_MASK_BIT_2  1 << 2
#define RTK_USB_DEVICE_LOG_MASK_BIT_3  1 << 3
#define RTK_USB_DEVICE_LOG_MASK_BIT_4  1 << 4
#define RTK_USB_DEVICE_LOG_MASK_BIT_5  1 << 5
#define RTK_USB_DEVICE_LOG_MASK_BIT_6  1 << 6
#define RTK_USB_DEVICE_LOG_MASK_BIT_7  1 << 7
#define RTK_USB_DEVICE_LOG_MASK_BIT_8  1 << 8
#define RTK_USB_DEVICE_LOG_MASK_BIT_9  1 << 9
#define RTK_USB_DEVICE_LOG_MASK_BIT_10 1 << 10
#define RTK_USB_DEVICE_LOG_MASK_BIT_11 1 << 11
#define RTK_USB_DEVICE_LOG_MASK_BIT_12 1 << 12
#define RTK_USB_DEVICE_LOG_MASK_BIT_13 1 << 13
#define RTK_USB_DEVICE_LOG_MASK_BIT_14 1 << 14



enum RTK_USB_DEVICE_LOG_LEVEL_MASK_BIT {
        RTK_USB_DEVICE_LOG_LEVEL_NONE     = RTK_USB_DEVICE_LOG_MASK_NONE,
        RTK_USB_DEVICE_LOG_LEVEL_ERR      = RTK_USB_DEVICE_LOG_MASK_BIT_0,
        RTK_USB_DEVICE_LOG_LEVEL_WARNING  = RTK_USB_DEVICE_LOG_MASK_BIT_1,
        RTK_USB_DEVICE_LOG_LEVEL_NOTICE   = RTK_USB_DEVICE_LOG_MASK_BIT_2,
        RTK_USB_DEVICE_LOG_LEVEL_INFO     = RTK_USB_DEVICE_LOG_MASK_BIT_3,
        RTK_USB_DEVICE_LOG_LEVEL_DEBUG    = RTK_USB_DEVICE_LOG_MASK_BIT_4,
        RTK_USB_DEVICE_LOG_LEVEL_VERBOSE  = RTK_USB_DEVICE_LOG_MASK_BIT_5,
};

enum RTK_USB_DEVICE_LOG_BLOCK_MASK_BIT {
        RTK_USB_DEVICE_LOG_BLOCK_NONE           = RTK_USB_DEVICE_LOG_MASK_NONE,
        RTK_USB_DEVICE_LOG_BLOCK_ALWAYS         = RTK_USB_DEVICE_LOG_MASK_BIT_0,
        RTK_USB_DEVICE_LOG_BLOCK_FUNCTIN_CALLER = RTK_USB_DEVICE_LOG_MASK_BIT_1,
        RTK_USB_DEVICE_LOG_BLOCK_FRAMER         = RTK_USB_DEVICE_LOG_MASK_BIT_2,
        RTK_USB_DEVICE_LOG_BLOCK_CNT            = RTK_USB_DEVICE_LOG_MASK_BIT_3,
        RTK_USB_DEVICE_LOG_BLOCK_RTK_USB_DEVICEOUT          = RTK_USB_DEVICE_LOG_MASK_BIT_4,
        RTK_USB_DEVICE_LOG_BLOCK_PID            = RTK_USB_DEVICE_LOG_MASK_BIT_5,
        RTK_USB_DEVICE_LOG_BLOCK_PCR            = RTK_USB_DEVICE_LOG_MASK_BIT_6,
        RTK_USB_DEVICE_LOG_BLOCK_SECTION        = RTK_USB_DEVICE_LOG_MASK_BIT_7,
        RTK_USB_DEVICE_LOG_BLOCK_BUFFER         = RTK_USB_DEVICE_LOG_MASK_BIT_8,
        RTK_USB_DEVICE_LOG_BLOCK_MRTK_USB_DEVICE            = RTK_USB_DEVICE_LOG_MASK_BIT_9,
        RTK_USB_DEVICE_LOG_BLOCK_STARTCODE      = RTK_USB_DEVICE_LOG_MASK_BIT_10,
        RTK_USB_DEVICE_LOG_BLOCK_PVR            = RTK_USB_DEVICE_LOG_MASK_BIT_11,
        RTK_USB_DEVICE_LOG_BLOCK_RTK_USB_DEVICEI_RATE_CNT   = RTK_USB_DEVICE_LOG_MASK_BIT_12,
        RTK_USB_DEVICE_LOG_BLOCK_CHANNEL_SCAN   = RTK_USB_DEVICE_LOG_MASK_BIT_13,
        RTK_USB_DEVICE_LOG_BLOCK_RTK_USB_DEVICEO_CLK        = RTK_USB_DEVICE_LOG_MASK_BIT_14,
        RTK_USB_DEVICE_LOG_BLOCK_ALL            = 0xffffffff,
};

enum RTK_USB_DEVICE_STATUS_LOG_FLAG_MASK_BIT {
        RTK_USB_DEVICE_STATUS_LOG_FLAG_NONE          = RTK_USB_DEVICE_LOG_MASK_NONE,
        RTK_USB_DEVICE_STATUS_LOG_FLAG_PKT_CNT       = RTK_USB_DEVICE_LOG_MASK_BIT_0,
        RTK_USB_DEVICE_STATUS_LOG_FLAG_ERR_CNT       = RTK_USB_DEVICE_LOG_MASK_BIT_1,
        RTK_USB_DEVICE_STATUS_LOG_FLAG_REV_CNT       = RTK_USB_DEVICE_LOG_MASK_BIT_2,
        RTK_USB_DEVICE_STATUS_LOG_FLAG_LIVERATE      = RTK_USB_DEVICE_LOG_MASK_BIT_3,
        RTK_USB_DEVICE_STATUS_LOG_FLAG_PIDFILTER     = RTK_USB_DEVICE_LOG_MASK_BIT_4,
        RTK_USB_DEVICE_STATUS_LOG_FLAG_NO_LIVERATE   = RTK_USB_DEVICE_LOG_MASK_BIT_5,
        RTK_USB_DEVICE_STATUS_LOG_FLAG_LOW_LIVERATE  = RTK_USB_DEVICE_LOG_MASK_BIT_6,
        RTK_USB_DEVICE_STATUS_LOG_FLAG_BUFFUSAGE     = RTK_USB_DEVICE_LOG_MASK_BIT_7,
        RTK_USB_DEVICE_STATUS_LOG_FLAG_MASSBUFFFULL  = RTK_USB_DEVICE_LOG_MASK_BIT_8,
        RTK_USB_DEVICE_STATUS_LOG_FLAG_RTK_USB_DEVICEI_RATE_CNT  = RTK_USB_DEVICE_LOG_MASK_BIT_9,
        RTK_USB_DEVICE_STATUS_LOG_FLAG_ALL           = 0xffffffff,
};

#define RTK_USB_DEVICE_HAS_FLAG(value, flag)   (value & flag)
#define RTK_USB_DEVICE_SET_FLAG(value, flag)   (value |= flag)
#define RTK_USB_DEVICE_CLEAR_FLAG(value, flag) (value &= (~flag))

#define CLEAR_LOG_MASK(level_value, block_value) \
rtk_usb_device_dbg_log_level_mask = rtk_usb_device_dbg_log_level_mask & ~level_value; \
rtk_usb_device_dbg_log_block_mask = rtk_usb_device_dbg_log_block_mask & ~block_value; \
 
#define SET_LOG_MASK(level_value, block_value) \
rtk_usb_device_dbg_log_level_mask = rtk_usb_device_dbg_log_level_mask | level_value; \
rtk_usb_device_dbg_log_block_mask = rtk_usb_device_dbg_log_block_mask | block_value; \
 
#define CLEAR_LOG_LEVEL_MASK(value) CLEAR_LOG_MASK(value, RTK_USB_DEVICE_LOG_BLOCK_NONE);
#define SET_LOG_LEVEL_MASK(value)   SET_LOG_MASK(value, RTK_USB_DEVICE_LOG_BLOCK_NONE);

#define CLEAR_LOG_BLOCK_MASK(value) CLEAR_LOG_MASK(RTK_USB_DEVICE_LOG_LEVEL_NONE, value);
#define SET_LOG_BLOCK_MASK(value)   SET_LOG_MASK(RTK_USB_DEVICE_LOG_LEVEL_NONE, value);

#define CHECK_LOG_BLOCK_MASK(value) (rtk_usb_device_dbg_log_block_mask&value)

#define rtk_usb_device_debug_emerg_log(fmt, args...)        {rtd_pr_usb_gadget_emerg(fmt, ## args);}
#define rtk_usb_device_mask_print_emerg(BIT_BLOCK, BIT_LEVEL, fmt, args...) {if (((BIT_LEVEL) & rtk_usb_device_dbg_log_level_mask) && ((BIT_BLOCK) & rtk_usb_device_dbg_log_block_mask)){rtk_usb_device_debug_emerg_log(fmt, ## args);}}

#define rtk_usb_device_debug_dbg_log(fmt, args...)  {rtd_pr_usb_gadget_info(fmt, ## args);}
#define rtk_usb_device_mask_print_dbg(BIT_BLOCK, BIT_LEVEL, fmt, args...)   {if (((BIT_LEVEL) & rtk_usb_device_dbg_log_level_mask) && ((BIT_BLOCK) & rtk_usb_device_dbg_log_block_mask)){rtk_usb_device_debug_dbg_log(fmt, ## args);}}

#define RTK_USB_DEVICE_TRACE(fmt, args...)  \
{ \
        if (rtk_usb_device_log_onoff)  \
        {   \
                rtk_usb_device_mask_print_emerg(RTK_USB_DEVICE_LOG_BLOCK_FUNCTIN_CALLER, RTK_USB_DEVICE_LOG_LEVEL_DEBUG, fmt, ## args) \
        }   \
}

#define RTK_USB_DEVICE_VERBOSE(fmt, args...)  \
{ \
        if (rtk_usb_device_log_onoff)  \
        {   \
                rtk_usb_device_mask_print_emerg( RTK_USB_DEVICE_LOG_BLOCK_ALWAYS, RTK_USB_DEVICE_LOG_LEVEL_VERBOSE, fmt ,  ## args)  \
        }       \
}

#define RTK_USB_DEVICE_INFO(fmt, args...)      \
{  \
        if (rtk_usb_device_log_onoff)     \
        {   \
                rtk_usb_device_mask_print_emerg( RTK_USB_DEVICE_LOG_BLOCK_ALWAYS, RTK_USB_DEVICE_LOG_LEVEL_INFO, fmt,  ## args)      \
        }   \
}

#define RTK_USB_DEVICE_WARNING(fmt, args...)  \
{   \
        if (rtk_usb_device_log_onoff)  \
        {  \
                rtk_usb_device_mask_print_emerg( RTK_USB_DEVICE_LOG_BLOCK_ALWAYS, RTK_USB_DEVICE_LOG_LEVEL_WARNING, "WARN, "fmt, ## args)  \
        }   \
}

#define RTK_USB_DEVICE_WARNING_DEBUG_LEVEL(fmt, args...)  \
{   \
        if (rtk_usb_device_log_onoff)  \
        {  \
                rtk_usb_device_mask_print_dbg( RTK_USB_DEVICE_LOG_BLOCK_ALWAYS, RTK_USB_DEVICE_LOG_LEVEL_WARNING, "WARN, "fmt, ## args)  \
        }   \
}

#define RTK_USB_DEVICE_ERROR(fmt, args...)  \
{   \
        rtk_usb_device_mask_print_emerg( RTK_USB_DEVICE_LOG_BLOCK_ALWAYS, RTK_USB_DEVICE_LOG_LEVEL_ERR, "ERROR, "fmt, ## args)  \
}

#define RTK_USB_DEVICE_DBG(fmt, args...)        {rtk_usb_device_mask_print_dbg( RTK_USB_DEVICE_LOG_BLOCK_ALL, RTK_USB_DEVICE_LOG_LEVEL_DEBUG, fmt, ## args)}

#define RTK_USB_DEVICE_DBG_SIMPLE(fmt, args...) do{rtk_usb_device_mask_print_emerg( RTK_USB_DEVICE_LOG_BLOCK_ALL, RTK_USB_DEVICE_LOG_LEVEL_DEBUG, fmt , ## args)}while(0)


#define pm_printk(LEVEL, fmt, args...)   RTK_USB_DEVICE_DBG_SIMPLE(fmt, ## args)
#define dwc_printk(LEVEL, fmt, args...)   

#endif          /* __RTK_USB_DEVICE_DBG_H__ */

