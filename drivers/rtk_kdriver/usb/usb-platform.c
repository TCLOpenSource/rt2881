#include <linux/clk.h>
#include <linux/module.h>
#include <asm/delay.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <rtk_kdriver/io.h>
#include <linux/usb/ch11.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <mach/pcbMgr.h>
#include <mach/platform.h>
#include <mach/pcbMgr.h>
#include <rbus/sb2_reg.h>
#include "rtk_usb_core.h"
#include "usb-platform-def.h"
#if IS_ENABLED(CONFIG_RTK_KDRV_EMCU)
#include <rtk_kdriver/rtk_emcu_export.h>
#endif

extern int rtk_usb_host_wakeup_resume_setting(void);
extern int rtk_usb_host_wakeup_suspend_setting(void);
static bool g_usb_wake_up_flow_en = false;

#define CONFIG_USB_DEVICE_WAKE_UP_SELF_ENABLE
#ifdef CONFIG_USB_DEVICE_WAKE_UP_SELF_ENABLE
static bool g_ever_set_usb_wakeup_enable = false;
#endif

#define DRIVER_DESC "RTK USB generic platform driver"
extern struct atomic_notifier_head panic_notifier_list;
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)
void rtk_ehci_debug_dump_ehci_information(void);
#endif
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)
void rtk_xhci_debug_dump_xhci_information(void);
#endif
static int rtk_usb_monitor_panic(struct notifier_block *np, unsigned long event, void *unused)
{
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)
	rtk_ehci_debug_dump_ehci_information();
#endif
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)
	rtk_xhci_debug_dump_xhci_information();
#endif
	return NOTIFY_DONE;
}

static struct notifier_block  rtk_usb_panic_notifier = {
        .notifier_call = rtk_usb_monitor_panic,
};

#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)
void usb3_crt_on(void);
void usb3_crt_off(void);
#endif
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)
void usb2_crt_on(void);
void usb2_crt_off(void);
#endif
#if IS_ENABLED(CONFIG_RTK_KDRV_USB_OTG_PLATFORM)
bool rtk_otg_is_enable(void);
void rtk_otg_crt_on_off(bool on);
#endif
void reset_usb_wifi_tuning_status(void);

int __init rtk_usb_init(void);
void rtk_usb_exit(void);

static void power_on_all_usb_host(void)
{
    reset_usb_wifi_tuning_status();
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)
    usb3_crt_on();
#endif
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)
    usb2_crt_on();
#endif
#if IS_ENABLED(CONFIG_RTK_KDRV_USB_OTG_PLATFORM)
    if(rtk_otg_is_enable())
        rtk_otg_crt_on_off(true);
#endif
}


void power_off_all_usb_host(void)
{
#if IS_ENABLED(CONFIG_RTK_KDRV_USB_OTG_PLATFORM)
    if(rtk_otg_is_enable())
        rtk_otg_crt_on_off(false);
#endif
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)
    usb3_crt_off();
#endif
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)
    usb2_crt_off();
#endif

}



static int rtk_usb_platform_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct device_node *node = pdev->dev.of_node;
    int error = 0;

    power_on_all_usb_host();

    error = of_platform_populate(node, NULL, NULL, dev); /* populate all children nodes of this device */
    if (error)
        RTK_USB_ERR("failed to create rtk usb platform\n");

#if 0
    if (sysfs_create_group(&dev->kobj, &dev_attr_grp)) {
        RTK_USB_WARN("Create self-defined sysfs attributes fail \n");
    }
#endif
    atomic_notifier_chain_register(&panic_notifier_list,&rtk_usb_panic_notifier);
    return error;
}


static int rtk_usb_platform_remove(struct platform_device *pdev)
{
#if 0
    sysfs_remove_group(&pdev->dev.kobj, &dev_attr_grp);
#endif
    of_platform_depopulate(&pdev->dev);
    platform_set_drvdata(pdev, NULL);
    atomic_notifier_chain_unregister(&panic_notifier_list, &rtk_usb_panic_notifier);
    return 0;
}


static int rtk_usb_platform_resume(struct device *dev)
{
	if(g_usb_wake_up_flow_en) {
		rtk_usb_host_wakeup_resume_setting();
		g_usb_wake_up_flow_en = false;
	}
	power_on_all_usb_host();
    return 0;
}


enum
{
	EHCI_PORT = 0,
	XHCI_PORT = 1,
};


struct USB_PLATFORM_WAKE_UP_PARAMETER
{
	u8 port_type;
	u8 port_num;
	u8 en_bit_offset;
	u8 force_en;
	u32 wake_up_en_reg;
};


static struct USB_PLATFORM_WAKE_UP_PARAMETER usb_platform_wake_up_parameters[] = USB_PLATFORM_WAKE_UP_PARAM_DATA;


#define MAX_WAKE_UP_PARAMETER_CNT (sizeof(usb_platform_wake_up_parameters) / sizeof(usb_platform_wake_up_parameters[0]))


static bool is_supported_usb_hid_device(struct usb_device *udev, u8 *port_type, u8 *portnum)
{
	int i = 0;
	int interface_num = 0;
	if(!udev->parent ||  udev->parent->parent)
		return false;
	
	if(!udev->actconfig)
		return false;
	
	interface_num = udev->actconfig->desc.bNumInterfaces;
	for(i = 0; i < interface_num; i++) {
		struct usb_interface *intf =  udev->actconfig->interface[i];
		if(intf && intf->cur_altsetting && 
					(intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_HID
						&& intf->cur_altsetting->desc.bInterfaceSubClass == 0x01
						&& (intf->cur_altsetting->desc.bInterfaceProtocol == 0x01
							|| intf->cur_altsetting->desc.bInterfaceProtocol == 0x02))) {
					*portnum = udev->portnum - 1;
					if(udev->parent->product && strstr(udev->parent->product, "xHCI Host"))
						*port_type = XHCI_PORT;
					else
						*port_type = EHCI_PORT;
					return true;
						
		}
	}
	return false;
}

static struct USB_PLATFORM_WAKE_UP_PARAMETER  * get_wake_up_parameter(u8 port_type, u8 portnum)
{
	int i = 0;
	for(i = 0; i < MAX_WAKE_UP_PARAMETER_CNT; i++) {
		if(usb_platform_wake_up_parameters[i].port_type == port_type
			&& usb_platform_wake_up_parameters[i].port_num== portnum) {
			return &usb_platform_wake_up_parameters[i];
		}
	}
	return NULL;
}

static void disable_usb_wake_up_reg(void)
{
	int i = 0;
	for(i = 0; i < MAX_WAKE_UP_PARAMETER_CNT; i++) {
		rtd_outl(usb_platform_wake_up_parameters[i].wake_up_en_reg, 
			rtd_inl(usb_platform_wake_up_parameters[i].wake_up_en_reg) 
				& (~(1 << usb_platform_wake_up_parameters[i].en_bit_offset)));
	}	
}

static void check_usb_wake_up_force_en(void)
{
	int i = 0;
	for(i = 0; i < MAX_WAKE_UP_PARAMETER_CNT; i++) {
		if(usb_platform_wake_up_parameters[i].force_en) {
			rtd_outl(usb_platform_wake_up_parameters[i].wake_up_en_reg, 
				rtd_inl(usb_platform_wake_up_parameters[i].wake_up_en_reg) 
				| (1 << usb_platform_wake_up_parameters[i].en_bit_offset));
		}
		
	}	
}

static int check_usb_device_for_u_connect(struct usb_device *udev, void *arg)
{
	u8 port_type = 0;
	u8 port_num = 0;
	
	if(is_supported_usb_hid_device(udev, &port_type, &port_num)) {
		struct USB_PLATFORM_WAKE_UP_PARAMETER  * parameter = get_wake_up_parameter(port_type, port_num);
		if(parameter) {
			*((bool *)arg) = true;
			rtd_outl(parameter->wake_up_en_reg, rtd_inl(parameter->wake_up_en_reg) | (1 << parameter->en_bit_offset));
		}
	}
	return 0;
}

struct each_dev_arg {
	void *data;
	int (*fn)(struct usb_device *, void *);
};

static int __each_dev(struct device *dev, void *data)
{
	struct each_dev_arg *arg = (struct each_dev_arg *)data;

	/* There are struct usb_interface on the same bus, filter them out */
	if (!dev->type || !dev->type->name 
			|| (strcmp(dev->type->name, "usb_device") != 0))
		return 0;

	return arg->fn(to_usb_device(dev), arg->data);
}

static int lookup_usb_for_each_dev(void *data, int (*fn)(struct usb_device *, void *))
{
	struct each_dev_arg arg = {data, fn};

	return bus_for_each_dev(rtk_usb_get_bus_type(), NULL, &arg, __each_dev);
}

static bool do_check_usb_connect_info(void)
{
	bool ret = false;
	unsigned int parameter_cnt = MAX_WAKE_UP_PARAMETER_CNT;
	if(!parameter_cnt)
		return ret;
	lookup_usb_for_each_dev(&ret, check_usb_device_for_u_connect);
	return ret;
}
#ifdef CONFIG_USB_DEVICE_WAKE_UP_SELF_ENABLE

static int check_enable_usb_device_wakeup(struct usb_device *udev, void *arg)
{
	u8 port_type = 0;
	u8 port_num = 0;
	bool enable  = *((bool *)arg);
	if(is_supported_usb_hid_device(udev, &port_type, &port_num)) {
		struct USB_PLATFORM_WAKE_UP_PARAMETER  * parameter = get_wake_up_parameter(port_type, port_num);
		if(parameter) {
			if(device_can_wakeup(&udev->dev))
				device_set_wakeup_enable(&udev->dev, enable ? true : false);
		}
	}
	return 0;
}

static void do_enable_usb_device_wakeup(bool enable)
{
	unsigned int parameter_cnt = MAX_WAKE_UP_PARAMETER_CNT;
	if(!parameter_cnt)
		return ;
	lookup_usb_for_each_dev(&enable, check_enable_usb_device_wakeup);
}

#endif

static bool is_usb_wakeup_enable(void)
{
#if IS_ENABLED(CONFIG_RTK_KDRV_EMCU)
	if(emcu_USB_WAKE_event())
		return true;
#endif		
	return false;
}

int rtk_usb_platform_wake_up_setting(void)
{
	if(!is_usb_wakeup_enable())
		return -1;
	disable_usb_wake_up_reg();
	if(!rtk_usb_get_bus_type())
		return -1;
	if(!do_check_usb_connect_info())
		return -1;
	check_usb_wake_up_force_en();
	RTK_USB_INFO("usb wake up enable\n");
	return 0;
}

#ifdef CONFIG_USB_DEVICE_WAKE_UP_SELF_ENABLE
int rtk_usb_platform_prepare(struct device *dev)
{
	//RTK_USB_INFO("%s %d\n", __FUNCTION__ ,  __LINE__);
	if(!rtk_usb_get_bus_type())
		return 0;	
	if(is_usb_wakeup_enable()) {
		do_enable_usb_device_wakeup(true);
		g_ever_set_usb_wakeup_enable = true;
	} else {
		if(g_ever_set_usb_wakeup_enable)
			do_enable_usb_device_wakeup(false);
		g_ever_set_usb_wakeup_enable = false;
	}

	return 0;
}

void rtk_usb_platform_complete(struct device *dev)
{
}
#endif

static int rtk_usb_platform_suspend(struct device *dev)
{
    if(rtk_usb_platform_wake_up_setting() != 0) {
#ifdef CONFIG_USB_PLATFORM_ENABLE_SUSPEND_POWER_DOWN
	    RTK_USB_INFO("%s start\n", __FUNCTION__);
	    power_off_all_usb_host();
	    RTK_USB_INFO("%s done\n", __FUNCTION__);
#endif
    } else {
		rtk_usb_host_wakeup_suspend_setting();
		g_usb_wake_up_flow_en = true;
	}
    return 0;
}


static int rtk_usb_platform_restore(struct device *dev)
{
    power_on_all_usb_host();

    return 0;
}

static void rtk_usb_platform_shutdown(struct platform_device *dev)
{
#ifdef CONFIG_USB_PLATFORM_ENABLE_SHUTDOWN_POWER_DOWN
	RTK_USB_INFO("%s start\n", __FUNCTION__);
	power_off_all_usb_host();
	RTK_USB_INFO("%s done\n", __FUNCTION__);
#endif
}


static const struct dev_pm_ops rtk_usb_platform_pm_ops = {
#ifdef CONFIG_USB_DEVICE_WAKE_UP_SELF_ENABLE	
    .prepare = rtk_usb_platform_prepare,
    .complete = rtk_usb_platform_complete,
#endif    
    .resume = rtk_usb_platform_resume,
    .suspend = rtk_usb_platform_suspend,
    .restore = rtk_usb_platform_restore,
};


static const struct of_device_id rtk_usb_of_match[] = {
    { .compatible = "rtk,usb-platform", },
    {},
};
MODULE_DEVICE_TABLE(of, rtk_usb_of_match);


static struct platform_driver rtk_usb_platform_driver = {
    .probe      = rtk_usb_platform_probe,
    .remove     = rtk_usb_platform_remove,
    .shutdown	= rtk_usb_platform_shutdown,
    .driver     = {
        .name = "usb-platform",
        .pm = &rtk_usb_platform_pm_ops,
        .of_match_table = rtk_usb_of_match,
    }
};


static int __init rtk_usb_platform_init(void)
{
    int ret = 0;
    ret = rtk_usb_init();
    if(ret != 0) {
        	RTK_USB_ERR("Realtek USB Core driver init failed\n");
		goto FAL_INIT_USB_CORE;
    }	
	ret = platform_driver_register(&rtk_usb_platform_driver);
    if(ret != 0) {
		RTK_USB_ERR("Realtek USB Platorm driver init failed\n");
		goto FAIL_INIT_USB_PLATFORM;
    }
    return ret;
FAIL_INIT_USB_PLATFORM:
    rtk_usb_exit();
FAL_INIT_USB_CORE:
    return ret;
}
module_init(rtk_usb_platform_init);


static void __exit rtk_usb_platform_cleanup(void)
{
    platform_driver_unregister(&rtk_usb_platform_driver);
    rtk_usb_exit();
}
module_exit(rtk_usb_platform_cleanup);


MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR("Jason Chiu");
MODULE_LICENSE("GPL");
