#ifndef __RTK_LOG_API__
#define __RTK_LOG_API__

#ifndef BUILD_QUICK_SHOW
#include <linux/printk.h>
#include <linux/dynamic_debug.h>
#else
#include <no_os/printk.h>
#endif

#if !defined(BUILD_QUICK_SHOW) && (IS_ENABLED(CONFIG_REALTEK_LOGBUF))
extern  bool check_log_level_and_module(char *level_buf,unsigned int module_id);
#else
#define check_log_level_and_module(a,b)  1
#endif

#define rtd_fmt(tag, fmt) "["tag"] "fmt

#define KERN_DEBUG_C '7'
#define rtd_printk(level, tag, fmt, ...)                    \
    do {                                    \
        if(check_log_level_and_module(level,0) == true)                    \
        {   \
            printk(level rtd_fmt(tag, fmt), ##__VA_ARGS__);     \
        }  \
    }while(0)
#define _rtd_printk(level, module_id, tag, fmt, ...)                    \
    do {                                    \
        if(check_log_level_and_module(level,module_id) == true)                    \
        {   \
            printk(level rtd_fmt(tag, fmt), ##__VA_ARGS__);     \
        }  \
    }while(0)

#endif // __RTK_LOG_API__
