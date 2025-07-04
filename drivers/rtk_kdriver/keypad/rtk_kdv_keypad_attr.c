/*
 * Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 * Copyright (c) 2014, Sony Mobile Communications Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <mach/platform.h>

#include "rtk_kdv_keypad_attr.h"
#include "rtk_kdv_keypad.h"

extern struct rtk_keypad_host rtk_kp_host;

ssize_t rtk_keypad_show_param(struct device *dev, struct device_attribute *attr, char *buf)
{
    int ret = 0;

    if (strncmp(attr->attr.name, "lockMode", strlen("lockMode")) == 0) {
        ret = snprintf(buf, PAGE_SIZE, "%u\n", rtk_kp_host.key_mode);
    } else  if (strncmp(attr->attr.name, "testMode", strlen("testMode")) == 0) {
        unsigned int i = 0;
        buf[0] = 0;
        snprintf(buf + strlen(buf), PAGE_SIZE - strlen(buf), rtk_kp_host.is_test_mode ? "Current in test mode" : "Current not in test mode");
        if(rtk_kp_host.is_test_mode) {
            snprintf(buf + strlen(buf), PAGE_SIZE - strlen(buf), "receive %u key event\n", rtk_kp_host.key_event_num);
            for(i = 0; i < rtk_kp_host.key_event_num; i++) {
                snprintf(buf + strlen(buf), PAGE_SIZE - strlen(buf), "keycode:%x, val:%x\n", 
                rtk_kp_host.key_event[i].code,rtk_kp_host.key_event[i].value);
            }
        }
        snprintf(buf + strlen(buf), PAGE_SIZE - strlen(buf), "\n");
        ret = strlen(buf);
        rtk_kp_host.key_event_num = 0; 
    } 

    return ret;
}

ssize_t rtk_keypad_set_param(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int val = 0;

    sscanf(buf, "%d\n", &val);
    KP_ERR("rtk_keypad_set_param. val=%d \n", val);

    if (strncmp(attr->attr.name, "lockMode", strlen("lockMode")) == 0) {
        if(1 == val)
            rtk_kp_host.key_mode = LOCK_MODE;
        else
            rtk_kp_host.key_mode = UNLOCK_MODE;
    } else if (strncmp(attr->attr.name, "testMode", strlen("testMode")) == 0) {
        if(1 == val){
            rtk_kp_host.is_test_mode = true;
        }
        else{
            rtk_kp_host.is_test_mode = false;
        }
        rtk_kp_host.key_event_num = 0;
    }

    return count;
}

DEVICE_ATTR(lockMode, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP, rtk_keypad_show_param, rtk_keypad_set_param);
DEVICE_ATTR(testMode, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP, rtk_keypad_show_param, rtk_keypad_set_param);

int  rtk_keypad_create_attr(struct device *device)
{
    int ret;
    ret = device_create_file(device, &dev_attr_lockMode);
    if(ret < 0) {
	return ret;
    }
    ret = device_create_file(device, &dev_attr_testMode);
    if(ret < 0) {
	device_remove_file(device, &dev_attr_lockMode);
    }
    return ret;
}
void rtk_keypad_remove_attr(struct device *device)
{
    device_remove_file(device, &dev_attr_lockMode);
    device_remove_file(device, &dev_attr_testMode);
}


