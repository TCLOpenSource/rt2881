#ifndef _MMC_RTK_DEBUG_H
#define _MMC_RTK_DEBUG_H


#include <rtd_log/rtd_module_log.h>
#define DEV_NAME "EMMC"

/************************************************************************
 *  rtk log define
 ************************************************************************/
#define EM_EMERG(fmt, args...)      \
	rtd_pr_emmc_emerg(fmt , ## args)

#define EM_ALERT(fmt, args...)   \
	rtd_pr_emmc_alert(fmt , ## args)

#define EM_CRIT(fmt, args...)   \
	rtd_pr_emmc_crit(fmt , ## args)

#define EM_ERR(fmt, args...)   \
	rtd_pr_emmc_err(fmt , ## args)

#define EM_WARNING(fmt, args...)   \
	rtd_pr_emmc_warn(fmt , ## args)

#define EM_NOTICE(fmt, args...)   \
	rtd_pr_emmc_notice(fmt , ## args)

#define EM_INFO(fmt, args...)   \
	rtd_pr_emmc_info(fmt , ## args)

#define EM_DEBUG(fmt, args...)   \
	rtd_pr_emmc_debug(fmt , ## args)


/************************************************************************
 *  emmc local dbg define
 ************************************************************************/
/* #define MMC_DEBUG */
 #ifdef MMC_DEBUG
    #define mmcdbg(fmt, args...) \
            EM_ALERT("mmcdbg:%s(%d): " fmt, __func__ ,__LINE__,## args)
#else
    #define mmcdbg(fmt, args...)
#endif

/* #define MMC_DSCP */
#ifdef MMC_DSCP     /* for make descript debug */
    #define mmcdscp(fmt, args...) \
            EM_ALERT("mmcdscp:%s(%d): " fmt, __func__ ,__LINE__,## args)
#else
    #define mmcdscp(fmt, args...)
#endif

/* #define MMC_CARD */
#ifdef MMC_IRQ      /* for irq relative debug */
    #define mmcirq(fmt, args...) \
            EM_ALERT("mmcard:%s(%d): " fmt, __func__ ,__LINE__,## args)
#else
    #define mmcirq(fmt, args...)
#endif

/* #define MMC_TASK */
#ifdef MMC_TASK
    #define mmctask(fmt, args...) \
            EM_ALERT("mmcrtk:%s(%d): " fmt, __func__ ,__LINE__,## args)
#else
    #define mmctask(fmt, args...)
#endif

/* #define MMC_PLL */
#ifdef MMC_PLL
    #define mmcpll(fmt, args...) \
            EM_ALERT("mmcspec:%s(%d): " fmt, __func__ ,__LINE__,## args)
#else
    #define mmcpll(fmt, args...)
#endif

/* #define MMC_MSG1 */
#ifdef MMC_MSG1
    #define mmcmsg1(fmt, args...) \
            EM_ALERT("mmcmsg1:%s(%d): " fmt, __func__ ,__LINE__,## args)
#else
    #define mmcmsg1(fmt, args...)
#endif

/* #define MMC_MSG2 */
#ifdef MMC_MSG2
    #define mmcmsg2(fmt, args...) \
            EM_ALERT("mmcmsg2:%s(%d): " fmt, __func__ ,__LINE__,## args)
#else
    #define mmcmsg2(fmt, args...)
#endif

/* #define MMC_MSG3 */
#ifdef MMC_MSG3
    #define mmcmsg3(fmt, args...) \
            EM_ALERT("mmcmsg3:%s(%d): " fmt, __func__ ,__LINE__,## args)
#else
    #define mmcmsg3(fmt, args...)
#endif

/* #define MMC_MSG4 */
#ifdef MMC_MSG4
    #define mmcmsg4(fmt, args...) \
            EM_ALERT("mmcmsg4:%s(%d): " fmt, __func__ ,__LINE__,## args)
#else
    #define mmcmsg4(fmt, args...)
#endif


/* #define MMC_TRH */
#ifdef MMC_TRH
    #define trhmsg(fmt, args...) \
            EM_ALERT("trhmsg:%s(%d): " fmt, __func__ ,__LINE__,## args)
#else
    #define trhmsg(fmt, args...)
#endif

/* #define MMC_RAW */
#ifdef MMC_RAW
    #define mmcraw(fmt, args...) \
            EM_ALERT("rawmsg:%s(%d): " fmt, __func__ ,__LINE__,## args)
#else
    #define mmcraw(fmt, args...)
#endif


#endif










