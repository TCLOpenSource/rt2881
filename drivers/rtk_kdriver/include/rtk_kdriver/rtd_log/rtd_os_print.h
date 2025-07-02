/*
    this file is used for rtd logbuf manager
*/
#ifndef __RTD_OS_PRINT_H__
#define __RTD_OS_PRINT_H__

#define MSG_LVL_TAG_EMERG      "M]["
#define MSG_LVL_TAG_ALERT      "A]["
#define MSG_LVL_TAG_CRIT       "C]["
#define MSG_LVL_TAG_ERR        "E]["
#define MSG_LVL_TAG_WARN       "W]["
#define MSG_LVL_TAG_NOTICE     "N]["
#define MSG_LVL_TAG_INFO       "I]["
#define MSG_LVL_TAG_DEBUG      "D]["
#define MSG_LVL_TAG_PRINT      "P]["

/*----------------------------------------
 * low level print API, this might be modified
 * by in the future, please don't use them
 * directly.
 *---------------------------------------*/
#if 0 //def BUILD_QUICK_SHOW

#include <printf.h>   // running under uboot

#define _rtd_pr_emerg(tag, fmt, ...)      printf2(tag fmt, ##__VA_ARGS__)
#define _rtd_pr_alert(tag, fmt, ...)      printf2(tag fmt, ##__VA_ARGS__)
#define _rtd_pr_crit(tag, fmt, ...)       printf2(tag fmt, ##__VA_ARGS__)
#define _rtd_pr_err(tag, fmt, ...)        printf2(tag fmt, ##__VA_ARGS__)
#define _rtd_pr_warn(tag, fmt, ...)       printf2(tag fmt, ##__VA_ARGS__)
#define _rtd_pr_notice(tag, fmt, ...)     printf2(tag fmt, ##__VA_ARGS__)
#define _rtd_pr_info(tag, fmt, ...)       printf2(tag fmt, ##__VA_ARGS__)
#define _rtd_pr_debug(tag, fmt, ...)      printf2(tag fmt, ##__VA_ARGS__)
#define _rtd_pr_print(level, tag, fmt, ...)      printf2(tag, fmt, ##__VA_ARGS__)

#else

#include <mach/rtk_log.h>

#define _rtd_pr_emerg(module_id, tag, fmt, ...)      _rtd_printk(KERN_EMERG,   module_id, MSG_LVL_TAG_EMERG  tag, fmt, ##__VA_ARGS__)
#define _rtd_pr_alert(module_id, tag, fmt, ...)      _rtd_printk(KERN_ALERT,   module_id, MSG_LVL_TAG_ALERT  tag, fmt, ##__VA_ARGS__)
#define _rtd_pr_crit(module_id, tag, fmt, ...)       _rtd_printk(KERN_CRIT,    module_id, MSG_LVL_TAG_CRIT   tag, fmt, ##__VA_ARGS__)
#define _rtd_pr_err(module_id, tag, fmt, ...)        _rtd_printk(KERN_ERR,     module_id, MSG_LVL_TAG_ERR    tag, fmt, ##__VA_ARGS__)
#define _rtd_pr_warn(module_id, tag, fmt, ...)       _rtd_printk(KERN_WARNING, module_id, MSG_LVL_TAG_WARN   tag, fmt, ##__VA_ARGS__)
#define _rtd_pr_notice(module_id, tag, fmt, ...)     _rtd_printk(KERN_NOTICE,  module_id, MSG_LVL_TAG_NOTICE tag, fmt, ##__VA_ARGS__)
#define _rtd_pr_info(module_id, tag, fmt, ...)       _rtd_printk(KERN_INFO,    module_id, MSG_LVL_TAG_INFO   tag, fmt, ##__VA_ARGS__)
#define _rtd_pr_debug(module_id, tag, fmt, ...)      _rtd_printk(KERN_DEBUG,   module_id, MSG_LVL_TAG_DEBUG  tag, fmt, ##__VA_ARGS__)
#define _rtd_pr_print(level, module_id, tag, fmt, ...)  _rtd_printk(level,     module_id, MSG_LVL_TAG_PRINT  tag, fmt, ##__VA_ARGS__)

#endif

#endif /*__RTD_OS_PRINT_H__*/
