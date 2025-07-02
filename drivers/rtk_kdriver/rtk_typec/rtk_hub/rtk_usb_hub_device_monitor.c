#include <linux/kernel.h>
#include <linux/usb/ch11.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/uaccess.h>
#include <rtk_kdriver/io.h>
#include <mach/system.h>
#include <linux/usb/hcd.h>
#include <linux/device.h>
#include <rtk_kdriver/pcbMgr.h>
#include "rtk_usb_hub_debug.h"
#include "rtk_usb_hub_smbus.h"
#include "rtk_usb_hub.h"
#include "rtk_usb_hub_wrapper.h"
#include "rtk_usb_hub_device_monitor.h"

extern struct device *g_usb_hub_devices[MAX_USB_HUB_DEVICE_NUM];

#define MAX_MONITOR_USB_DEV_PATH_NUM 8
#define MAX_MONITOR_USB_DEV_PATH_LEN 24
static unsigned char g_monitor_usb_dev_path[MAX_MONITOR_USB_DEV_PATH_NUM][MAX_MONITOR_USB_DEV_PATH_LEN];
static unsigned int g_monitor_usb_dev_path_num = 0;
static DEFINE_MUTEX(g_monitor_usb_dev_path_lock);

bool rtk_usb_hub_add_monitor_usb_dev_path(const char *usb_dev_path)
{
	bool ret = false;
	int i = 0;
	char dev_path[MAX_MONITOR_USB_DEV_PATH_LEN];
	if(!usb_dev_path || strlen(usb_dev_path) == 0 
		|| strlen(usb_dev_path) >= MAX_MONITOR_USB_DEV_PATH_LEN)
		return ret;
	strncpy(dev_path, usb_dev_path, MAX_MONITOR_USB_DEV_PATH_LEN);
	if(dev_path[strlen(dev_path) - 1] == '\n')
		dev_path[strlen(dev_path) - 1] = 0;
	
	mutex_lock(&g_monitor_usb_dev_path_lock);
	for(i = 0; i < g_monitor_usb_dev_path_num; i++) {
		if(strcmp(g_monitor_usb_dev_path[i], dev_path) == 0) {
			ret =true;
			goto EXIT;
		}
	}
	if(g_monitor_usb_dev_path_num == MAX_MONITOR_USB_DEV_PATH_NUM)
		goto EXIT;
	snprintf(g_monitor_usb_dev_path[g_monitor_usb_dev_path_num], MAX_MONITOR_USB_DEV_PATH_LEN, "%s", dev_path);
	g_monitor_usb_dev_path_num++;
	ret = true;
EXIT:			
	mutex_unlock(&g_monitor_usb_dev_path_lock);
	return ret;
}

bool rtk_usb_hub_remove_monitor_usb_dev_path(const char *usb_dev_path)
{
	bool ret = false;
	int i = 0;
	char dev_path[MAX_MONITOR_USB_DEV_PATH_LEN];
	if(!usb_dev_path || strlen(usb_dev_path) == 0 
		|| strlen(usb_dev_path) >= MAX_MONITOR_USB_DEV_PATH_LEN)
		return ret;
	strncpy(dev_path, usb_dev_path, MAX_MONITOR_USB_DEV_PATH_LEN);
	if(dev_path[strlen(dev_path) - 1] == '\n')
		dev_path[strlen(dev_path) - 1] = 0;
	
	mutex_lock(&g_monitor_usb_dev_path_lock);
	for(i = 0; i < g_monitor_usb_dev_path_num; i++) {
		if(strcmp(g_monitor_usb_dev_path[i], dev_path) == 0) {
			break;
		}
	}
	if(i >= g_monitor_usb_dev_path_num) {
		ret = true;
		goto EXIT;
	}
	if((i + 1)  == g_monitor_usb_dev_path_num) {
		g_monitor_usb_dev_path[i][0] = 0;
	} else {
		snprintf(g_monitor_usb_dev_path[i], MAX_MONITOR_USB_DEV_PATH_LEN, "%s", g_monitor_usb_dev_path[g_monitor_usb_dev_path_num -1]);
		g_monitor_usb_dev_path[g_monitor_usb_dev_path_num - 1][0] = 0;
	}
	g_monitor_usb_dev_path_num--;
	ret = true;
EXIT:			
	mutex_unlock(&g_monitor_usb_dev_path_lock);
	return ret;
}

void rtk_usb_hub_dump_monitor_usb_dev_path(char *buf, unsigned int buf_len)
{
	int i  =0;
	if(!buf || !buf_len)
		return;
	buf[0] = 0;
	mutex_lock(&g_monitor_usb_dev_path_lock);
	for(i = 0; i < g_monitor_usb_dev_path_num; i++) {
		snprintf(buf + strlen(buf), buf_len - strlen(buf), "%s\n", g_monitor_usb_dev_path[i]);
	}
	mutex_unlock(&g_monitor_usb_dev_path_lock);
}

bool rtk_usb_hub_find_monitor_usb_dev_path(char *dev_path)
{
	int i = 0;
	bool ret = false;
	if(!dev_path)
		return ret;
	mutex_lock(&g_monitor_usb_dev_path_lock);
	for(i = 0; i < g_monitor_usb_dev_path_num; i++) {
		if(strstr(dev_path, g_monitor_usb_dev_path[i]))
			ret = true;
	}
	mutex_unlock(&g_monitor_usb_dev_path_lock);
	return ret;
}


static bool rtk_usb_hub_get_usb_dev_path(struct usb_device *udev, char *buf, unsigned int buf_len)
{
	bool ret = false;
	int i = 0;
	unsigned char port_array[8] = {0};
	int buf_port_size = 0;
	int bus_num = 0;
	
	if(!udev || !udev->parent || !buf || !buf_len)
		return ret;

	while(udev) {
		if(udev->parent) {
			port_array[buf_port_size] = udev->portnum;
			buf_port_size++;
			if(buf_port_size >= 8)
				goto EXIT;
			udev = udev->parent;
		} else {
			bus_num = udev->bus->busnum;
			break;
		}
	}
	if(!buf_port_size)
		goto EXIT;
	
	buf[0] = 0;
	snprintf(buf + strlen(buf), buf_len - strlen(buf), "%d-", bus_num);
	for(i = (buf_port_size - 1); i >= 0; i--)
		snprintf(buf + strlen(buf), buf_len - strlen(buf), "%d.", port_array[i]);
	buf[strlen(buf) - 1] = 0;
	ret = true;	
EXIT:
	return ret;
}

void send_hub_information_by_uevent(char *message)
{
	char event[64] = {0};
	char *envp[2] = {NULL, NULL};
    
	if(!message || g_usb_hub_devices[0] == NULL)
		return;
        
	snprintf(event, sizeof(event), "%s", message);
	envp[0]= event;
	envp[1] = NULL;

	kobject_uevent_env(&g_usb_hub_devices[0]->kobj, KOBJ_CHANGE, envp);
}



static void _send_intercept_uevent(char *dev_path)
{
	char event[64] = {0};
	snprintf(event, sizeof(event), "USB_REFLECTOR=LIMITED_DEVICE_INSERTED_%s", dev_path);
	send_hub_information_by_uevent(event);
}

void rtk_usb_hub_usb_dev_judge_flow(struct usb_device *udev)
{
	char dev_path[MAX_MONITOR_USB_DEV_PATH_LEN];
	if(udev->speed < USB_SPEED_HIGH
		||udev->descriptor.bDeviceClass == USB_CLASS_HUB)
		return;
	if(!g_monitor_usb_dev_path_num)
		return ;
	if(rtk_usb_hub_get_usb_dev_path(udev, dev_path, sizeof(dev_path)) != true)
		return;
	if(rtk_usb_hub_find_monitor_usb_dev_path(dev_path) != true)
		return ;
	
	_send_intercept_uevent(dev_path);
}

//PCB_USB_PORT_MONITOR0=(2 << 0) | (1 << 4) | (1 << 8)
void rtk_usb_hub_get_monitor_usb_dev_info(void)
{
	
	int i = 0;
	for(i = 0; i < 8; i++) {
		char pin_name[32];
		u64 pin = 0;
		int ret = 0;
		int count = 0;
		unsigned char buf[MAX_MONITOR_USB_DEV_PATH_LEN];
		snprintf(pin_name, sizeof(pin_name), "%s%d", "PCB_USB_PORT_MONITOR", i);
		ret = pcb_mgr_get_enum_info_byname(pin_name, &pin);
		if(ret)
			break;
		if(!pin)
			break;
		buf[0] = 0;
		while(pin) {
			unsigned char val = pin & 0xF;
			if(!val)
				break;
			pin = pin >> 4;
			if(count == 0)
				snprintf(buf + strlen(buf), MAX_MONITOR_USB_DEV_PATH_LEN - strlen(buf), "%u-", val);
			else
				snprintf(buf + strlen(buf), MAX_MONITOR_USB_DEV_PATH_LEN - strlen(buf), "%u.", val);
			count++;
		}
		if(count < 2)
			continue;
		buf[strlen(buf) - 1] = 0;
		RTK_USB_HUB_INFO("%s%d=%s\n", "PCB_USB_PORT_MONITOR", i, buf);
		rtk_usb_hub_add_monitor_usb_dev_path(buf);	
	}
}

