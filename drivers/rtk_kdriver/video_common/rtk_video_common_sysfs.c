/*=============================================================
 * File:    rtk_video_common_sysfs.c
 *
 * Desc:    VIDEO COMMON SYSFS
 *
 * AUTHOR:  xiao_tim@realtek.com
 *
 * Vresion: 0.0.1
 *
 *------------------------------------------------------------
 * Copyright (c) Realtek Semiconductor Corporation, 2021
 *
 * All rights reserved.
 *
 *============================================================*/
#include <linux/syscalls.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <rtk_kdriver/video_common/rtk_video_common.h>
#include "rtk_video_common-priv.h"


const char* _sysfs_get_video_common_port_str(VIDEO_COMMON_PORT vc_port)
{
    switch(vc_port)
    {
    case VIDEO_COMMON_0:         return "VIDEO_COMMON_0";
    case VIDEO_COMMON_1:         return "VIDEO_COMMON_1";
    case VIDEO_COMMON_2:         return "VIDEO_COMMON_2";
    case VIDEO_COMMON_3:         return "VIDEO_COMMON_3";
    default:                     return "VIDEO_COMMON_NONE";
    }
}


const char* _sysfs_get_input_source_str(VIDEO_COMMON_PORT_IN_SEL src)
{
    switch(src)
    {
    case VIDEO_COMMON_HDMIRX:    return "HDMIRX";
    case VIDEO_COMMON_DPRX:      return "DPRX";
    default:                     return "NONE";
    }
}

const char* _sysfs_get_source_type_str(VIDEO_COMMON_SOURCE_TYPE type)
{
    switch(type)
    {
    case HDMI_2p0:              return "HDMI_2p0";
    case HDMI_2p1:              return "HDMI_2p1";
    case DP_1p4:                return "DP_1p4";
    case DP_2p0:                return "DP_2p0";
    case HDMI_2p0_DSCD:         return "HDMI_2p0_DSCD";
    case HDMI_2p1_DSCD:         return "HDMI_2p1_DSCD";
    case DP_1p4_DSCD:           return "DP_1p4_DSCD";
    case DP_2p0_DSCD:           return "DP_2p0_DSCD";
    default:                    return "NO Source Type";
    }
}


static unsigned int _dump_video_common_status(
    char*           buf,
    int             count
    )
{
    int  n;
    int  i;
    char *ptr = buf;

    n = scnprintf(ptr, count, "---------------- VIDEO COMMON STATUS (ver: %s) --------------\n\n", "V1.0");
    ptr+=n; count-=n;

    for (i=0; i<HDMI_DP_CHANNLE_PORT_MAX; i++)
    {
        n = scnprintf(ptr, count, "====================== VIDEO_COMMON [%2d] ======================\n", i);
        ptr+=n; count-=n;
        n = scnprintf(ptr, count, "Input Source = %2d (%s)\n", g_vc_port_instance[i].src_in, _sysfs_get_input_source_str(g_vc_port_instance[i].src_in));
        ptr+=n; count-=n;
        n = scnprintf(ptr, count, "Input Type = %2d (%s)\n", g_vc_port_instance[i].src_type, _sysfs_get_source_type_str(g_vc_port_instance[i].src_type));
        ptr+=n; count-=n;
        n = scnprintf(ptr, count, "Source Channel : %2d\n", g_vc_port_instance[i].ch);
        ptr+=n; count-=n;
        n = scnprintf(ptr, count, "\n");
        ptr+=n; count-=n;
    }

    return ptr - buf;
}


//===========================================================================
// Func : video_common_show_status
//===========================================================================

static ssize_t video_common_show_status(
    struct device*              dev,
    struct device_attribute*    attr,
    char*                       buf
    )
{
    int  n;
    int  count = PAGE_SIZE;
    char *ptr = buf;

    n = _dump_video_common_status(ptr, count);
    ptr+=n; count-=n;

    return ptr - buf;
}


static DEVICE_ATTR(status, 0444, video_common_show_status, NULL);


//===========================================================================
// Func : video_common_attr_show_debug_ctrl
//===========================================================================
static int g_vc_src_in = 0;
static int g_vc_ch = 0;
static int g_vc_vc = 0;

static ssize_t video_common_attr_show_debug_ctrl(
    struct device*              dev,
    struct device_attribute*    attr,
    char*                       buf
    )
{
    int  n;
    int  count = PAGE_SIZE;
    char *ptr = buf;

    //---------------------------------------------------
    // Set Video Common Port
    //---------------------------------------------------
    n = scnprintf(ptr, count, "\n\n-----------------Set Video Common Port-----------------\n");
    ptr+=n; count-=n;
    n = scnprintf(ptr, count, "src_in=%d, ch=%d, vc=%d\n", g_vc_src_in, g_vc_ch, g_vc_vc);
    ptr+=n; count-=n;

    n = scnprintf(ptr, count, "\n-----------------Parameter-----------------\n");
    ptr+=n; count-=n;
    n = scnprintf(ptr, count, "src_in : 0 (VIDEO_COMMON_HDMIRX), 1 (VIDEO_COMMON_DPRX)\n");
    ptr+=n; count-=n;
    n = scnprintf(ptr, count, "ch : channel port 0 ~ 3\n");
    ptr+=n; count-=n;
    n = scnprintf(ptr, count, "vc : VIDEO COMMON Port 0 ~ 3\n");
    ptr+=n; count-=n;
    n = scnprintf(ptr, count, "----------------------------------------\n\n");
    ptr+=n; count-=n;
    return ptr - buf;
}


static ssize_t video_common_attr_store_debug_ctrl(struct device *dev,
                   struct device_attribute *attr,
                   const char *buf, size_t count)
{
    int val_1;
    if (sscanf(buf, "src_in=%5d", &val_1)==1)
    {
        g_vc_src_in = val_1;
    }
    else if (sscanf(buf, "ch=%5d", &val_1)==1)
    {
        g_vc_ch = val_1;
    }
    else if (sscanf(buf, "vc=%5d", &val_1)==1)
    {
        g_vc_vc = val_1;
    }
    else if (sscanf(buf, "run=%5d", &val_1)==1)
    {
        if(val_1)
        {
            RTK_VIDEO_COMMON_WARNING("[Debug] rtk_video_common_set_ownership %d, %d, %d\n", g_vc_src_in, g_vc_ch, g_vc_vc);
            rtk_video_common_set_ownership(g_vc_src_in, g_vc_ch, g_vc_vc); 
        }
    }

    return count;
}

static DEVICE_ATTR(debug_ctrl, 0644, video_common_attr_show_debug_ctrl, video_common_attr_store_debug_ctrl);


//////////////////////////////////////////////////////////////////////////////
// File Operations
//////////////////////////////////////////////////////////////////////////////

int video_common_open(struct inode *inode, struct file *file)
{
    return 0;
}

int video_common_release(struct inode *inode, struct file *file)
{
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Module Init / Exit
//////////////////////////////////////////////////////////////////////////////

static struct file_operations video_common_fops =
{
    .owner   = THIS_MODULE,
    .open    = video_common_open,
    .release = video_common_release,
};

static struct miscdevice video_common_miscdev =
{
    MISC_DYNAMIC_MINOR, "rtk_video_common", &video_common_fops
};

int video_common_sysfs_init(void)
{
    RTK_VIDEO_COMMON_INFO("video_common_sysfs_init\n");
    if (misc_register(&video_common_miscdev))
    {
        RTK_VIDEO_COMMON_WARNING("video_common_init failed - register misc device failed\n");
        return -ENODEV;
    }

    device_create_file(video_common_miscdev.this_device, &dev_attr_status);
    device_create_file(video_common_miscdev.this_device, &dev_attr_debug_ctrl);
    return 0;
}

void video_common_syfss_exit(void)
{
    RTK_VIDEO_COMMON_INFO("video_common_syfss_exit\n");
    device_remove_file(video_common_miscdev.this_device, &dev_attr_status);
    device_remove_file(video_common_miscdev.this_device, &dev_attr_debug_ctrl);
    misc_deregister(&video_common_miscdev);
}
