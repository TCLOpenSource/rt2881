#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/usb/g_hid.h>
#include <linux/usb/composite.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <mach/platform.h>
#include <mach/rtk_platform.h>
#include <linux/usb.h>
#include "rtk_composite_platform.h"
#include "rtk_hidg_dev_info.h"

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
int __init rtk_hidg_register_hid_input_report_callback(void);
void __exit rtk_hidg_unregister_hid_input_report_callback(void);
#endif
int __init rtk_composite_init(void);
void __exit rtk_composite_cleanup(void);
int __init f_hidddc_init(void);
void __exit f_hidddc_uninit(void);
void __exit f_hidg_uninit(void);
int __init f_hidg_init(void);
int __init f_hidg_ex_init(void);
void __exit f_hidg_ex_uninit(void);
#if IS_ENABLED(CONFIG_RTK_KDRV_USB_HUB)
int typec_source_switch_notifier_call_chain(unsigned long val, void *v);
#endif
bool g_enable_rtk_hidg_feature = false;
bool g_hid_via_otg_enable = false;
bool g_hid_via_otg_ex_enable = false;
bool g_hid_via_ddc_enable = false;
bool g_enable_debug = false;
bool g_hidg_driver_registed = false;
bool g_reverse_control_mode = false;
bool g_reverse_control_ex_mode = false;
bool g_hid_fake_standby_usb_filter = false;
bool g_hid_fake_standby_bluetooth_filter = false;
bool g_hid_fake_standby_filter_function_enable = false;
static struct kobject *g_rtk_composite_kobj;

#define composite_attr(_name) \
static struct kobj_attribute _name##_attr = {   \
        .attr   = {                             \
                .name = __stringify(_name),     \
                .mode = 0644,                   \
        },                                      \
        .show   = _name##_show,                 \
        .store  = _name##_store,                \
}

static void judge_global_hid_enable(void)
{
	 if(g_hid_via_otg_enable || g_hid_via_ddc_enable || g_hid_via_otg_ex_enable)
	 	g_enable_rtk_hidg_feature = true;
	 else 
	 	g_enable_rtk_hidg_feature = false;	
}


static ssize_t hidg_enable_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        snprintf(buf, PAGE_SIZE, "hidg_enable:%s\n", g_hid_via_otg_enable ? "ON" : "OFF");
        return strlen(buf);
}

static ssize_t hidg_enable_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
        unsigned int val = 0;

        if (kstrtouint(buf, 10, &val) != 0)
                return -EINVAL;
        g_hid_via_otg_enable = (!!val) ? true : false;
	 judge_global_hid_enable();
#if IS_ENABLED(CONFIG_RTK_KDRV_USB_HUB)	 
	 typec_source_switch_notifier_call_chain(g_hid_via_otg_enable ? 1 : 0, NULL);
#endif	 
        return n;
}
composite_attr(hidg_enable);

static ssize_t hidg_mode_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        snprintf(buf, PAGE_SIZE, "hidg_enable:%s\n", g_hid_via_otg_enable ? "ON" : "OFF");
        return strlen(buf);
}

static ssize_t hidg_mode_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
        unsigned int val = 0;

        if (kstrtouint(buf, 10, &val) != 0)
                return -EINVAL;
        g_hid_via_otg_enable = (!!val) ? true : false;
	 judge_global_hid_enable();
        return n;
}
composite_attr(hidg_mode);


static ssize_t hidg_ex_mode_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        snprintf(buf, PAGE_SIZE, "hidg_ex_enable:%s\n", g_hid_via_otg_ex_enable ? "ON" : "OFF");
        return strlen(buf);
}

static ssize_t hidg_ex_mode_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
        unsigned int val = 0;

        if (kstrtouint(buf, 10, &val) != 0)
                return -EINVAL;
        g_hid_via_otg_ex_enable = (!!val) ? true : false;
	 judge_global_hid_enable();
        return n;
}
composite_attr(hidg_ex_mode);

static ssize_t reverse_control_mode_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        snprintf(buf, PAGE_SIZE, "g_reverse_control_mode:%s\n", g_reverse_control_mode ? "ON" : "OFF");
        return strlen(buf);
}

static ssize_t reverse_control_mode_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
        unsigned int val = 0;

        if (kstrtouint(buf, 10, &val) != 0)
                return -EINVAL;
        g_reverse_control_mode = (!!val) ? true : false;
#if IS_ENABLED(CONFIG_RTK_KDRV_USB_HUB)	 
	 typec_source_switch_notifier_call_chain(g_reverse_control_mode ? 1 : 0, NULL);
#endif
        return n;
}
composite_attr(reverse_control_mode);

static ssize_t reverse_control_ex_mode_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        snprintf(buf, PAGE_SIZE, "g_reverse_control_ex_mode:%s\n", g_reverse_control_ex_mode ? "ON" : "OFF");
        return strlen(buf);
}

static ssize_t reverse_control_ex_mode_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
        unsigned int val = 0;

        if (kstrtouint(buf, 10, &val) != 0)
                return -EINVAL;
        g_reverse_control_ex_mode = (!!val) ? true : false;
#if IS_ENABLED(CONFIG_RTK_KDRV_USB_HUB)	 
	 typec_source_switch_notifier_call_chain(g_reverse_control_ex_mode ? 1 : 0, (void *)1);
#endif
        return n;
}
composite_attr(reverse_control_ex_mode);



static ssize_t hidddc_enable_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        snprintf(buf, PAGE_SIZE, "hidddc_enable:%s\n", g_hid_via_ddc_enable ? "ON" : "OFF");
        return strlen(buf);
}

static ssize_t hidddc_enable_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
        unsigned int val = 0;

        if (kstrtouint(buf, 10, &val) != 0)
                return -EINVAL;
        g_hid_via_ddc_enable = (!!val) ? true : false;
	 judge_global_hid_enable();
        return n;
}
composite_attr(hidddc_enable);


static ssize_t hidg_output_absolute_pos_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        return 0;
}

ssize_t f_hidg_send_data(int id, const unsigned char *buffer, unsigned int count, unsigned int f_flags);
static ssize_t hidg_output_absolute_pos_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
	unsigned int val_x, val_y;
	unsigned int mouse_val = 0;
	unsigned char *ptr = (unsigned char *)&mouse_val;
	unsigned char tmp_buf[16] = {0};
	memset(tmp_buf, 0, 16);
	tmp_buf[0] = 2;

	if(sscanf(buf, "%d,%d", &val_x, &val_y) != 2)
		return -EINVAL;
		
	mouse_val = ((val_y & 0xFFF) << 12) |(val_x & 0xFFF);
	memcpy(tmp_buf + 3, ptr, 3);

        f_hidg_send_data(0, tmp_buf, 8, 0);
        return n;
}
composite_attr(hidg_output_absolute_pos);


static ssize_t composite_debug_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        snprintf(buf, PAGE_SIZE, "debug_en:%s\n", g_enable_debug ? "ON" : "OFF");
        return strlen(buf);
}

static ssize_t composite_debug_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
        unsigned int val = 0;

        if (kstrtouint(buf, 10, &val) != 0)
                return -EINVAL;
        g_enable_debug = (!!val) ? true : false;
        return n;
}
composite_attr(composite_debug);

static ssize_t fake_standby_usb_filter_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        snprintf(buf, PAGE_SIZE, "g_hid_fake_standby_usb_filter:%d\n", g_hid_fake_standby_usb_filter);
        return strlen(buf);
}

static ssize_t fake_standby_usb_filter_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
        unsigned int val = 0;

        if (kstrtouint(buf, 10, &val) != 0)
                return -EINVAL;
        g_hid_fake_standby_usb_filter = (!!val) ? true : false;
        return n;
}
composite_attr(fake_standby_usb_filter);

static ssize_t fake_standby_bluetooth_filter_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        snprintf(buf, PAGE_SIZE, "g_hid_fake_standby_bluetooth_filter:%d\n", g_hid_fake_standby_bluetooth_filter);
        return strlen(buf);
}

static ssize_t fake_standby_bluetooth_filter_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
        unsigned int val = 0;

        if (kstrtouint(buf, 10, &val) != 0)
                return -EINVAL;
        g_hid_fake_standby_bluetooth_filter = (!!val) ? true : false;
        return n;
}
composite_attr(fake_standby_bluetooth_filter);

static ssize_t fake_standby_filter_function_enable_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        snprintf(buf, PAGE_SIZE, "g_hid_fake_standby_filter_function_enable:%d\n", g_hid_fake_standby_filter_function_enable);
        return strlen(buf);
}

static ssize_t fake_standby_filter_function_enable_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
        unsigned int val = 0;

        if (kstrtouint(buf, 10, &val) != 0)
                return -EINVAL;
        g_hid_fake_standby_filter_function_enable = (!!val) ? true : false;
        return n;
}
composite_attr(fake_standby_filter_function_enable);

static ssize_t reprobe_all_usb_mouses_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return -1;
}

struct usbhid_device_info
{
	int intf_num;
	struct usb_interface *intf[16];
};

static int __hid_bus_reprobe_drivers(struct device *dev, void *data)
{
	int i  = 0;
	struct usbhid_device_info *info = (struct usbhid_device_info *)data;
	struct hid_device *hdev = to_hid_device(dev);
	struct usb_interface *intf = NULL;
	struct usb_device *cur_udev = NULL;

	if(hdev->bus != BUS_USB)
		return 0;
	if(hdev->collection == NULL || hdev->collection->usage != HID_GD_MOUSE)
		return 0;
	if(info->intf_num >= 16)
		return 1;

	intf = to_usb_interface(hdev->dev.parent);
	cur_udev = interface_to_usbdev (intf);
	
	for(i = 0; i < info->intf_num; i++) {
		struct usb_device *tmp_udev = interface_to_usbdev (info->intf[i]);
		if(cur_udev == tmp_udev) {
			intf->needs_binding = true;
			return 0;
		}
	}
	intf->needs_binding = true;
	usb_get_intf(intf);
	info->intf[info->intf_num++] = intf;
	return 0;
}

static void check_registered_hid_device(void)
{
	int i = 0;
	struct usbhid_device_info device_info;
	static struct bus_type *s_hid_bus_type = NULL;
	if(!s_hid_bus_type) {
		struct hid_device *hid_dev = hid_allocate_device();
		if(!hid_dev)
			return;
		s_hid_bus_type = hid_dev->dev.bus;
		hid_destroy_device(hid_dev);
	}
	if(!s_hid_bus_type)
		return;
	
	device_info.intf_num = 0;
	bus_for_each_dev(s_hid_bus_type, NULL, &device_info,
				 __hid_bus_reprobe_drivers);
	for(i = 0; i < device_info.intf_num; i++) {
		struct usb_device *udev;
		struct usb_interface *intf = device_info.intf[i];
		int ret;
		udev = interface_to_usbdev (intf);
		ret = usb_lock_device_for_reset(udev, intf);
		if (ret >= 0) {
			usb_reset_device(udev);
			usb_unlock_device(udev);
		}
		usb_put_intf(intf);
	}
}

static ssize_t reprobe_all_usb_mouses_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
	unsigned int val = 0;
	if (kstrtouint(buf, 10, &val) != 0)
		return -EINVAL;
	if(val) {
		check_registered_hid_device();
	}
	return n;
}
composite_attr(reprobe_all_usb_mouses);

static struct attribute * rtk_composite_attrs[] =
{
        &hidg_enable_attr.attr,
		&hidddc_enable_attr.attr,
        &composite_debug_attr.attr,
        &hidg_mode_attr.attr,
        &hidg_ex_mode_attr.attr,
        &reverse_control_mode_attr.attr,
        &reverse_control_ex_mode_attr.attr,
        &hidg_output_absolute_pos_attr.attr,
        &fake_standby_usb_filter_attr.attr,
        &fake_standby_bluetooth_filter_attr.attr,
        &fake_standby_filter_function_enable_attr.attr,
        &reprobe_all_usb_mouses_attr.attr,
        NULL,
};

static struct attribute_group rtk_composite_attr_group =
{
        .attrs = rtk_composite_attrs,
};

void RTK_USB_Typec_Hidg_enable(bool enable)
{
	g_hid_via_otg_enable = enable;
	judge_global_hid_enable();
}
EXPORT_SYMBOL(RTK_USB_Typec_Hidg_enable);

void RTK_USB_Typec_Hidg_ex_enable(bool enable)
{
	g_hid_via_otg_ex_enable = enable;
	judge_global_hid_enable();
}
EXPORT_SYMBOL(RTK_USB_Typec_Hidg_ex_enable);

void RTK_USB_Typec_Hidddc_enable(bool enable)
{
	g_hid_via_ddc_enable = enable;
	judge_global_hid_enable();
}
EXPORT_SYMBOL(RTK_USB_Typec_Hidddc_enable);

void rtk_composite_dump_hex(const char *func, unsigned int line, unsigned char *data, unsigned int len)
{
        char buf[128] = {0};
        unsigned int i = 0;
        if(!func || !line || !data || !len)
                return;
        RTK_COMPOSITE_INFO("%s %u[%u]:\n", func, line, len);
        for(i = 0; i < len; i++) {
             snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "0x%02x,", data[i]);   
             if((i + 1) % 16 == 0) {
                RTK_COMPOSITE_INFO("%s\n", buf);
                buf[0] = 0;
             }
        }
        RTK_COMPOSITE_INFO("%s\n", buf);
}

static int __init rtk_composite_misc_init(void)
{
        int result = 0;
        
        g_rtk_composite_kobj = kobject_create_and_add("rtk_composite", NULL);
        if (!g_rtk_composite_kobj) {
                result = -ENOMEM;
                goto FAIL_CREATE_KOBJ;
        }
        result = sysfs_create_group(g_rtk_composite_kobj, &rtk_composite_attr_group);
        if (result != 0) {
                goto FAIL_CREATE_SYSFS_GROUP;
        }
        return result;
FAIL_CREATE_SYSFS_GROUP:
    kobject_put(g_rtk_composite_kobj);
    g_rtk_composite_kobj = NULL;
FAIL_CREATE_KOBJ:
    return result;

}

static void rtk_composite_misc_uninit(void)
{
        sysfs_remove_group(g_rtk_composite_kobj, &rtk_composite_attr_group);
        kobject_put(g_rtk_composite_kobj);
        g_rtk_composite_kobj = NULL;

}

bool rtk_hidg_driver_is_init(void)
{
    return g_hidg_driver_registed;
}

static int __init rtk_hidg_plat_init(void)
{
        int ret = 0;

	//if(get_product_type() != PRODUCT_TYPE_DIAS)
	//	return -1;

        if(rtk_hidg_plat_devices_add() != true) {
                ret = -1;
                RTK_COMPOSITE_ERR(" rtk_hidg_plat_devices_add fail\n");
                goto FAIL_ADD_HIDG_PLAT_DEVICES;
        }

        ret= rtk_composite_misc_init();
        if (ret != 0) {
                RTK_COMPOSITE_ERR(" rtk_hidg_chardev_init fail %d\n", ret);
                goto FAIL_CHARDEV_INIT;
        }

	f_hidddc_init();

	ret = f_hidg_init();
	if (ret != 0) {
                RTK_COMPOSITE_ERR(" f_hidg_init fail %d\n", ret);
                goto FAIL_HIDG_INIT_F;
        }

	ret = f_hidg_ex_init();
	if (ret != 0) {
                RTK_COMPOSITE_ERR(" f_hidg_ex_init fail %d\n", ret);
                goto FAIL_HIDG_EX_INIT_F;
        }

        ret = rtk_composite_init();
        if(ret < 0) {
                RTK_COMPOSITE_ERR(" rtk_composite_init fail %d\n", ret);
                goto FAIL_INIT_HIDG;
        }
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
	ret = rtk_hidg_register_hid_input_report_callback();
	if(ret != 0) {
                RTK_COMPOSITE_ERR(" rtk_hidg_register_hid_input_report_callback fail %d\n", ret);
                goto FAIL_REGISTER_HID_CALLBACK;
	}
#endif
	g_hidg_driver_registed = true;
	return ret;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
FAIL_REGISTER_HID_CALLBACK:
	rtk_composite_cleanup();
#endif
FAIL_INIT_HIDG:
	f_hidg_ex_uninit();
FAIL_HIDG_EX_INIT_F:
	f_hidg_uninit();
FAIL_HIDG_INIT_F:
        f_hidddc_uninit();
        rtk_composite_misc_uninit();
FAIL_CHARDEV_INIT:        
        rtk_hidg_plat_devices_remove();
FAIL_ADD_HIDG_PLAT_DEVICES:        
        return ret;
}
module_init(rtk_hidg_plat_init);

static void __exit rtk_composite_plat_cleanup(void)
{
        g_hidg_driver_registed = false;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
        rtk_hidg_unregister_hid_input_report_callback();
#endif
        rtk_hidg_plat_devices_remove();
        rtk_composite_misc_uninit();
        rtk_composite_cleanup();
        f_hidddc_uninit();
        f_hidg_uninit();	 
        f_hidg_ex_uninit();
}

module_exit(rtk_composite_plat_cleanup);

MODULE_LICENSE("GPL");