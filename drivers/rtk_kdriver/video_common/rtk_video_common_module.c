/*=============================================================
 * File:    dprx_vfe_sysfs.c
 *
 * Desc:    DPRX VFE API
 *
 * AUTHOR:  kevin_wang@realtek.com
 *
 * Vresion: 0.0.1
 *
 *------------------------------------------------------------
 * Copyright (c) Realtek Semiconductor Corporation, 2021
 *
 * All rights reserved.
 *
 *============================================================*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/kthread.h>
#include <linux/syscalls.h>
#include <linux/miscdevice.h>
#include <rtk_kdriver/video_common/rtk_video_common.h>
#include "rtk_video_common-priv.h"


/////////////////////////////////////////////////////////////////////
// Platform device Interface
/////////////////////////////////////////////////////////////////////

extern int  video_common_sysfs_init(void);
extern void video_common_syfss_exit(void);

#ifdef CONFIG_PM

static int rtk_video_common_pm_suspend(struct device *dev)
{
    RTK_VIDEO_COMMON_INFO("rtk_video_common_pm_suspend\n");
    return 0;
}

static int rtk_video_common_pm_resume(struct device *dev)
{
    RTK_VIDEO_COMMON_INFO("rtk_video_common_pm_resume\n");
    return 0;
}

#ifdef CONFIG_HIBERNATION

static int rtk_video_common_pm_suspend_std(struct device *dev)
{
    RTK_VIDEO_COMMON_INFO("rtk_video_common_pm_suspend_std\n");
    return 0;
}
static int rtk_video_common_pm_resume_std(struct device *dev)
{
    RTK_VIDEO_COMMON_INFO("rtk_video_common_pm_resume_std\n");
    return 0;
}

#endif // CONFIG_HIBERNATION

static const struct dev_pm_ops rtk_video_common_pm_ops = {

    .suspend = rtk_video_common_pm_suspend,
    .resume  = rtk_video_common_pm_resume,

#ifdef CONFIG_HIBERNATION
    .freeze     = rtk_video_common_pm_suspend_std,
    .thaw       = rtk_video_common_pm_resume_std,
#endif // CONFIG_HIBERNATION
};


#endif // CONFIG_PM

/*------------------------------------------------------------------
 * Func : rtk_video_common_drv_probe
 *
 * Desc : probe video common device
 *
 * Parm : pdev : handle of dprx device
 *
 * Retn : success / fail
 *------------------------------------------------------------------*/
static int rtk_video_common_drv_probe(
    struct platform_device*     pdev
    )
{
    return 0;
}


/*------------------------------------------------------------------
 * Func : rtk_video_common_drv_remove
 *
 * Desc : remove video common driver
 *
 * Parm : pdev : handle of dprx device
 *
 * Retn : 0
 *------------------------------------------------------------------*/
static int rtk_video_common_drv_remove(
    struct platform_device*     pdev
    )
{
    return 0;
}


static const struct of_device_id rtk_video_common_of_match[] =
{
    {
        .compatible = "realtek,video_common",
    },
    {
    },
};

MODULE_DEVICE_TABLE(of, rtk_video_common_of_match);

static struct platform_driver rtk_video_common_platform_driver =
{
    .probe      = rtk_video_common_drv_probe,
    .remove     = rtk_video_common_drv_remove,
    .driver     =
    {
        .name = "rtk_video_common",
        .of_match_table = rtk_video_common_of_match,
#ifdef CONFIG_PM
        .pm    = &rtk_video_common_pm_ops,
#endif
    },
};

/////////////////////////////////////////////////////////////////////
// Module Interface
/////////////////////////////////////////////////////////////////////


/*------------------------------------------------------------------
 * Func : rtk_video_common_module_init
 *
 * Desc : init function of video common module
 *
 * Parm : N/A
 *
 * Retn : success / fail
 *------------------------------------------------------------------*/
int rtk_video_common_module_init(void)
{
    return platform_driver_register(&rtk_video_common_platform_driver);
}


/*------------------------------------------------------------------
 * Func : rtk_video_common_module_exit
 *
 * Desc : exif function of video common module
 *
 * Parm : N/A
 *
 * Retn : N/A
 *------------------------------------------------------------------*/
void rtk_video_common_module_exit(void)
{
    platform_driver_unregister(&rtk_video_common_platform_driver);
}

 /*------------------------------------------------
 * Func : video_common_module_init
 *
 * Desc : VC module init function
 *
 * Para : N/A
 *
 * Retn : N/A
 *-----------------------------------------------*/
int __init video_common_module_init(void)
{
    rtk_video_common_module_init();
    video_common_sysfs_init();
    return 0;
}

/*------------------------------------------------
 * Func : video_common_module_exit
 *
 * Desc : VC module exit function
 *
 * Para : N/A
 *
 * Retn : N/A
 *-----------------------------------------------*/
static void __exit video_common_module_exit(void)
{
    video_common_syfss_exit();
    rtk_video_common_module_exit();
}

module_init(video_common_module_init);
module_exit(video_common_module_exit);
MODULE_LICENSE("GPL");
