#include <linux/kernel.h>
#include <linux/usb/ch11.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of_platform.h>
#include <linux/uaccess.h>
#include <rtk_kdriver/io.h>
#include <mach/system.h>
#include <linux/usb/hcd.h>
#include <linux/device.h>
#include <linux/usb/cdc.h>
#include "rtk_usb_hub_defines.h"
#include "rtk_usb_hub_debug.h"
#include "rtk_usb_hub_smbus.h"
#include "rtk_usb_hub.h"
#include "rtk_usb_hub_wrapper.h"
#include "rtk_usb_hub_device_monitor.h"

static bool s_is_in_typec_source = false;
static struct bus_type *s_usb_bus_type = NULL;
extern struct device *g_usb_hub_devices[MAX_USB_HUB_DEVICE_NUM];
static unsigned int g_current_typea_hub_select = 0;


int do_usb_for_each_dev(void *data, int (*fn)(struct usb_device *, void *));

struct usb_hub_port_poll_data
{
	unsigned char bus_num1;   //typea port root_hub_number
	unsigned char bus_num2;   //typea port root_hub_number
	unsigned char port_num; //typea port root_port_number
	unsigned char port_sts_reg_high;  //hub port hub_port_status_reg
	unsigned char port_sts_reg_low;
	bool need_poll;
	bool need_match_bus_num;
	unsigned long check_time;
};

/*WARNING: Need to change according to different Project or IC*/
static struct usb_hub_port_poll_data g_hub_port_poll_data[] =
			CONFIG_HUB_PORT_POLL_DATA;

#if (MAX_USB_HUB_DEVICE_NUM  > 1)
static struct usb_hub_port_poll_data g_hub_port_poll_data_1[] =
			CONFIG_HUB_PORT_POLL_DATA_1;
#endif
int rtk_usb_hub_select_cur_hub(struct device *dev, unsigned int hub_idx)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
	if(hub_idx >= MAX_USB_HUB_DEVICE_NUM)
		return -1;
	if(private_data->id != hub_idx)
		return -1;
	if(private_data->id == g_current_typea_hub_select)
		return 0;
	g_current_typea_hub_select = hub_idx;
	rtk_usb_hub_switch_hub(dev, g_current_typea_hub_select);
	return 0;
}
unsigned int rtk_usb_hub_get_selected_hub(void)
{
	return g_current_typea_hub_select;
}

void rtk_usb_hub_check_and_set_hub_downstream_port_state(struct device *dev)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
	int i = 0;
	for(i = 0; i < MAX_HUB_TYPEA_PORT_NUM; i++) {
		int cur_state = atomic_read(&private_data->dwonstreamport_current_state[i]);
		int should_state =  atomic_read(&private_data->dwonstreamport_should_state[i]);
		if(cur_state != should_state) {
			bool ret;
			unsigned int port_index = 0xFF;
			if(private_data->id == 0) {
				if(i < (sizeof(g_hub_port_poll_data) / sizeof(g_hub_port_poll_data[0])))
					port_index = g_hub_port_poll_data[i].port_sts_reg_high & 0xF;
			}
#if (MAX_USB_HUB_DEVICE_NUM  > 1)			
			else {
				if(i < (sizeof(g_hub_port_poll_data_1) / sizeof(g_hub_port_poll_data_1[0])))
					port_index = g_hub_port_poll_data_1[i].port_sts_reg_high & 0xF;
			}
#endif		
			if(port_index !=  0xFF) {
				ret = rtk_smbus_force_dsp_port_disconnect(dev, (port_index - 1), (should_state == 0) ?  true : false);
				if(ret == true) {
					atomic_set(&private_data->dwonstreamport_current_state[i], should_state);
					rtk_usb_vbus_toggle(i);
				}
			}
		}
		if(should_state)			
			rtk_usb_host_port_onoff(i, false);
	}
}

void rtk_usb_hub_check_port(struct device *dev)
{
	int i = 0;
	int size = 0;
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
	
	if(dev != g_usb_hub_devices[g_current_typea_hub_select])
		return;
	if(g_current_typea_hub_select == 0) {
		size = sizeof(g_hub_port_poll_data)/ sizeof(g_hub_port_poll_data[0]);
		for(i = 0; i < size; i++) {
			int cur_state = atomic_read(&private_data->dwonstreamport_current_state[i]);
			int should_state =  atomic_read(&private_data->dwonstreamport_should_state[i]);
			if((g_hub_port_poll_data[i].need_poll == true) && time_after(jiffies, g_hub_port_poll_data[i].check_time)  && (cur_state == should_state)) {
				unsigned int reg_val = 0;
				if(rtk_smbus_read_data_transfer(dev, 
	                                g_hub_port_poll_data[i].port_sts_reg_high, 
						g_hub_port_poll_data[i].port_sts_reg_low, &reg_val) == true) {
					if((reg_val & 0x3) == 0x0) {
						g_hub_port_poll_data[i].need_poll = false;
						rtk_usb_hub_specail_dsp_enable(dev, g_hub_port_poll_data[i].port_num - 1, 
							(private_data->typea_port_switch_mode[g_hub_port_poll_data[i].port_num - 1]== 2) ? true : false);
					}
				}
			}
		}
	} else {
#if (MAX_USB_HUB_DEVICE_NUM  > 1)	
		size = sizeof(g_hub_port_poll_data_1)/ sizeof(g_hub_port_poll_data_1[0]);
		for(i = 0; i < size; i++) {
			int cur_state = atomic_read(&private_data->dwonstreamport_current_state[i]);
			int should_state =  atomic_read(&private_data->dwonstreamport_should_state[i]);
			if(g_hub_port_poll_data_1[i].need_poll == true && time_after(jiffies, g_hub_port_poll_data_1[i].check_time)  && (cur_state == should_state)) {
				unsigned int reg_val = 0;
				if(rtk_smbus_read_data_transfer(dev, 
	                                g_hub_port_poll_data_1[i].port_sts_reg_high, 
						g_hub_port_poll_data_1[i].port_sts_reg_low, &reg_val) == true) {
					if((reg_val & 0x3) == 0x0) {
						g_hub_port_poll_data_1[i].need_poll = false;
						rtk_usb_hub_specail_dsp_enable(dev, g_hub_port_poll_data_1[i].port_num - 1, 
							(private_data->typea_port_switch_mode[g_hub_port_poll_data_1[i].port_num - 1]== 2) ? true : false);
					}
				}
			}
		}
#endif		
	}
}


static struct usb_hub_port_poll_data * rtk_usb_hub_find_port_poll_data(unsigned char bus_num, unsigned int port_num)
{
	if(g_current_typea_hub_select == 0) {
		int i = 0;
		int size = sizeof(g_hub_port_poll_data)/ sizeof(g_hub_port_poll_data[0]);
		for(i = 0; i < size; i++) {
			if((!g_hub_port_poll_data[i].need_match_bus_num 
				|| (g_hub_port_poll_data[i].bus_num1 == bus_num || g_hub_port_poll_data[i].bus_num2 == bus_num))
				&& (g_hub_port_poll_data[i].port_num == port_num))
					return &g_hub_port_poll_data[i];
		}
	} else {
#if (MAX_USB_HUB_DEVICE_NUM  > 1)	
		int i = 0;
		int size = sizeof(g_hub_port_poll_data_1)/ sizeof(g_hub_port_poll_data_1[0]);
		for(i = 0; i < size; i++) {
			if((!g_hub_port_poll_data_1[i].need_match_bus_num 
				|| (g_hub_port_poll_data_1[i].bus_num1 == bus_num || g_hub_port_poll_data_1[i].bus_num2 == bus_num))
				&& (g_hub_port_poll_data_1[i].port_num == port_num))
				return &g_hub_port_poll_data_1[i];
		}
#endif		
	}
	return NULL;
}

static unsigned char get_usb_device_root_port_number(struct usb_device *usb_dev)
{
	while(usb_dev->parent && usb_dev->parent->parent) {
		usb_dev = usb_dev->parent;
	}
	return usb_dev->portnum;
}

static bool rtk_usb_is_under_ehci_host(struct usb_device *udev)
{
	struct usb_device *roothub = udev;
	while(roothub && roothub->parent) {
		roothub = roothub->parent;
	}
	if(roothub && roothub->product
		&& (strstr(roothub->product, "EHCI") || strstr(roothub->product, "OHCI")))
		return true;
	return false;
}

static bool usb_dev_is_ethernet(struct usb_device *udev)
{
	unsigned int i = 0;
	bool contain_cdc_net = false;
	
	if(udev->speed < USB_SPEED_HIGH || udev->descriptor.bDeviceClass == USB_CLASS_HUB)
		return false;
	
	if(!udev->config)
		return false;
	
	for(i = 0; i < udev->descriptor.bNumConfigurations; i++) {
		int j = 0;
		struct usb_host_config *config = &udev->config[i];
		if(!config)
			continue;
		for(j = 0; j < config->desc.bNumInterfaces; j++) {
			struct usb_interface_cache *intf = config->intf_cache[j];
			if(!intf || !intf->num_altsetting || intf->altsetting[0].desc.bInterfaceClass != USB_CLASS_COMM)
				continue;
			
			if(intf->altsetting[0].desc.bInterfaceSubClass == USB_CDC_SUBCLASS_EEM
				&& intf->altsetting[0].desc.bInterfaceProtocol == USB_CDC_PROTO_EEM) {//cdc eem
				contain_cdc_net = true;
				break;
			} else if(intf->altsetting[0].desc.bInterfaceSubClass == USB_CDC_SUBCLASS_NCM) {//cdc num
				contain_cdc_net = true;
				break;
			} else if(intf->altsetting[0].desc.bInterfaceSubClass == USB_CDC_SUBCLASS_ETHERNET) {//cdc net
				contain_cdc_net = true;
				break;
			}
				
		}	
	}
	return contain_cdc_net;
}

static bool is_special_usb_combo_device(struct usb_device *udev)
{
	if((udev->descriptor.idVendor == 0x03f0)
		&& (udev->descriptor.idProduct== 0x028c)
		&& udev->product && strstr(udev->product, "HyperX QuadCast")
		&& udev->parent && udev->parent->parent 
		&& !udev->parent->parent->parent) {
		RTK_USB_HUB_WARNING("Get special combo device\n");
		return true;
	}
	return false;
}


struct look_for_audio_data
{
	struct usb_device *parent;
	struct usb_device *cur_dev;
	bool contain_other_devices;
	
};

static int usb_dev_is_video_or_audio_or_storage(struct usb_device *udev)
{
	int i  = 0;
	int intf_num = 0;

	if(!udev->actconfig)
		return 0;
	
	intf_num = udev->actconfig->desc.bNumInterfaces;
	for(i = 0; i < intf_num; i++) {
		struct usb_interface *intf = udev->actconfig->interface[i];
		if(!intf || !intf->cur_altsetting)
			continue;
		
		if(intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_VIDEO) {
			return 1;
		} else if(intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_AUDIO) {
			return 2;
		} else if(intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_MASS_STORAGE) {
			return 3;
		}
	}
	return 0;
}


static int check_is_target_device(struct usb_device *udev, void *arg)
{
	struct look_for_audio_data *pdata = (struct look_for_audio_data *)arg;

	if(pdata->contain_other_devices)
		return 1;
	
	if(udev == pdata->cur_dev || udev->parent != pdata->parent)
		return 0;

	if(usb_dev_is_video_or_audio_or_storage(udev) == 0) {
		pdata->contain_other_devices = true;
		return 1;
	}
	return 0;

}

static bool is_need_switch_hub_device(struct usb_device *udev)
{
	char event[128] = {0};
	struct look_for_audio_data data;
	if(!udev->parent || !udev->parent->parent || udev->parent->parent->parent)
		return false;
	data.contain_other_devices  = false;
	data.cur_dev = udev;
	data.parent = udev->parent;
	
	do_usb_for_each_dev(&data,check_is_target_device);
	
	if(data.contain_other_devices)
		return false;
	snprintf(event, sizeof(event), "USB_REFLECTOR=HUB_SWITCH_BY_SPECIAL_DEVICES");
	send_hub_information_by_uevent(event);
	return true;
	
}

static int check_has_other_device(struct usb_device *udev, void *arg)
{
	struct look_for_audio_data *pdata = (struct look_for_audio_data *)arg;

	if(pdata->contain_other_devices)
		return 1;
	
	if(udev == pdata->cur_dev || udev->parent != pdata->parent)
		return 0;

	pdata->contain_other_devices = true;
	return 0;

}

static void check_is_first_no_redirect_device_under_hub_device(struct usb_device *udev)
{
	char event[128] = {0};
	struct look_for_audio_data data;
	if(!udev->parent || !udev->parent->parent || udev->parent->parent->parent)
		return;
	data.contain_other_devices  = false;
	data.cur_dev = udev;
	data.parent = udev->parent;
	do_usb_for_each_dev(&data,check_has_other_device);
	
	if(!data.contain_other_devices) {
		snprintf(event, sizeof(event), "USB_REFLECTOR=HUB_NOT_SWITCH_BY_NO_REDIRECT_DEVICES");
		send_hub_information_by_uevent(event);
	}
}

static int rtk_usb_bus_notify(struct notifier_block *nb, unsigned long action,
		void *data)
{
	struct device *dev = data;
	bool is_special_combo = false;

	switch (action) {
	case BUS_NOTIFY_ADD_DEVICE:
		if (dev->type && dev->type->name 
			&& (strcmp(dev->type->name, "usb_device") == 0))
			rtk_usb_hub_usb_dev_judge_flow(to_usb_device(dev));
		break;
	default:
		break;
	}

	if(!s_is_in_typec_source)
		return 0;

	switch (action) {
	case BUS_NOTIFY_ADD_DEVICE:
		if (dev->type && dev->type->name 
			&& (strcmp(dev->type->name, "usb_interface") == 0)) {
			struct usb_interface *intf = to_usb_interface(dev);
			struct usb_device *udev = interface_to_usbdev(intf);

			if(!rtk_usb_is_under_ehci_host(udev))
				return 0;
			
			//only for device under root hub
			if(!udev->parent || udev->parent->parent) {
				is_special_combo = is_special_usb_combo_device(udev);
				if(!is_special_combo) {
					if(intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_VIDEO
						|| intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_AUDIO
						|| intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_MASS_STORAGE)
						is_special_combo = is_need_switch_hub_device(udev);
				}
				if(!is_special_combo) {
					check_is_first_no_redirect_device_under_hub_device(udev);
					return 0;
				}
			}
			
			if((intf->cur_altsetting && 
				(intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_VIDEO 
					|| intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_MASS_STORAGE
					|| intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_AUDIO))
					|| usb_dev_is_ethernet(udev)
					|| is_special_combo) {
				int portnum = get_usb_device_root_port_number(udev);
				struct usb_hub_port_poll_data *poll_data = rtk_usb_hub_find_port_poll_data(udev->bus->busnum, portnum);
				if(poll_data != NULL) {
					struct device * dev = g_usb_hub_devices[g_current_typea_hub_select];
					if(dev) {
						struct platform_device *pdev = container_of(dev, struct platform_device, dev);
						struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
						rtk_usb_hub_specail_dsp_enable(dev, 
							poll_data->port_num - 1, 
							(private_data->typea_port_switch_mode[poll_data->port_num - 1]== 1) ? false : true);
						if(private_data->typea_port_switch_mode[poll_data->port_num - 1] == 0) {
							poll_data->check_time = jiffies + msecs_to_jiffies(3000);
							poll_data->need_poll = true;
							
						}
					}
				}	
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

static int check_usb_device_for_switch(struct usb_device *udev, void *unused)
{
	int i  = 0;
	int intf_num = 0;
	bool is_special_combo = false;

	if(!rtk_usb_is_under_ehci_host(udev))
		return 0;	

	//only for device under root hub
	if(!udev->parent || udev->parent->parent) {
		is_special_combo = is_special_usb_combo_device(udev);
		if(!is_special_combo) {
			int devcie_type = usb_dev_is_video_or_audio_or_storage(udev);
			if(devcie_type != 0)
				is_special_combo = is_need_switch_hub_device(udev);
		}
		if(!is_special_combo) {
			check_is_first_no_redirect_device_under_hub_device(udev);
			return 0;
		}
	}

	if(!udev->actconfig)
		return 0;

	intf_num = udev->actconfig->desc.bNumInterfaces;
	for(i = 0; i < intf_num; i++) {
		struct usb_interface *intf = udev->actconfig->interface[i];
		if((intf->cur_altsetting && 
				(intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_VIDEO 
					|| intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_MASS_STORAGE
					|| intf->cur_altsetting->desc.bInterfaceClass == USB_CLASS_AUDIO))
					|| usb_dev_is_ethernet(udev)
					|| is_special_combo) {
				struct device * dev = g_usb_hub_devices[g_current_typea_hub_select];
				if(dev) {
					struct platform_device *pdev = container_of(dev, struct platform_device, dev);
					struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
					int portnum = get_usb_device_root_port_number(udev);
					struct usb_hub_port_poll_data *poll_data = rtk_usb_hub_find_port_poll_data(udev->bus->busnum, portnum);
					if(poll_data != NULL) {
						rtk_usb_hub_specail_dsp_enable(dev, 
							poll_data->port_num - 1, (private_data->typea_port_switch_mode[poll_data->port_num - 1] == 1) ? false : true);
						if (private_data->typea_port_switch_mode[poll_data->port_num - 1] == 0) {
							poll_data->check_time = jiffies + msecs_to_jiffies(3000);
							poll_data->need_poll = true;
							
						}
						break;
					}
				}
					
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

int do_usb_for_each_dev(void *data, int (*fn)(struct usb_device *, void *))
{
	struct each_dev_arg arg = {data, fn};

	return bus_for_each_dev(s_usb_bus_type, NULL, &arg, __each_dev);
}

void do_check_usb_camera_on_enter_typec(struct device *dev)
{
	if(dev == g_usb_hub_devices[g_current_typea_hub_select]) {
		if(s_usb_bus_type) {
			do_usb_for_each_dev(NULL, check_usb_device_for_switch);
		}
		s_is_in_typec_source = true;
	}
}

void do_check_usb_camera_on_exit_typec(struct device *dev)
{
       if(dev == g_usb_hub_devices[g_current_typea_hub_select]) {
        	int i = 0;
        	int size = sizeof(g_hub_port_poll_data)/ sizeof(g_hub_port_poll_data[0]);
        	for(i = 0; i < size; i++) {
			struct platform_device *pdev = container_of(dev, struct platform_device, dev);
			struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
        		rtk_usb_hub_specail_dsp_enable(dev, 
					g_hub_port_poll_data[i].port_num - 1, (private_data->typea_port_switch_mode[g_hub_port_poll_data[i].port_num - 1] == 2) ? true : false);
        		g_hub_port_poll_data[i].need_poll = false;
        	}
        	s_is_in_typec_source = false;
        }
}

static struct notifier_block rtk_usb_bus_nb = {
	.notifier_call = rtk_usb_bus_notify,
};


static struct usb_device_id dummy_usb_ids[] = {
	{ }
};

static int dummy_usb_probe(struct usb_interface *intf,
			 const struct usb_device_id *id)
{
	return -1;
}

static void dummy_usb_disconnect(struct usb_interface *intf)
{
	
}

static struct usb_driver dummy_usb_driver = {
	.name =		"dummy_usb_hub_driver",
	.probe =	dummy_usb_probe,
	.disconnect =	dummy_usb_disconnect,
	.id_table =	dummy_usb_ids,
	.supports_autosuspend = 0,
	.soft_unbind =	0,
};


int register_typec_source_switch_notifier(struct notifier_block *nb);
int unregister_typec_source_switch_notifier(struct notifier_block *nb);

static void set_pop_bb_flag(unsigned int id, unsigned char en)
{
	struct device *dev = NULL;
	struct platform_device *pdev;
	struct RTK_USB_HUB_PRIVATE *private_data;	
	if(id >= MAX_USB_HUB_DEVICE_NUM || !g_usb_hub_devices[id])
		return;
	dev = g_usb_hub_devices[id];
	pdev = container_of(dev, struct platform_device, dev);
	private_data = platform_get_drvdata(pdev);
	private_data->is_need_pop_bb = en;
}

static int typec_source_switch_notifier_fn(struct notifier_block *nb, unsigned long action, void *data)
{
	switch(action) {
	case 1:
		do_check_usb_camera_on_enter_typec(g_usb_hub_devices[g_current_typea_hub_select]);
		break;
	case 0:
		do_check_usb_camera_on_exit_typec(g_usb_hub_devices[g_current_typea_hub_select]);
		break;
	case 0x20:
		set_pop_bb_flag(0, 1);
		break;
	case 0x21:
		set_pop_bb_flag(0, 0);
		break;
	case 0x40:
		set_pop_bb_flag(1, 1);
		break;
	case 0x41:
		set_pop_bb_flag(1, 0);
		break;
	default:
		break;
	}
	return 0;
}

static struct notifier_block typec_source_switch_notifier = {
	.notifier_call = typec_source_switch_notifier_fn,
};


int rtk_usb_hub_register_notifier(void)
{
	int ret;
	ret = usb_register(&dummy_usb_driver);
	if(ret)
		goto EXIT;
	s_usb_bus_type = dummy_usb_driver.drvwrap.driver.bus;
	usb_deregister(&dummy_usb_driver);
	if(!s_usb_bus_type)
		goto EXIT;
	ret = bus_register_notifier(s_usb_bus_type, &rtk_usb_bus_nb);
	if(ret) {
		s_usb_bus_type = NULL;
		goto EXIT;
	}
	ret = register_typec_source_switch_notifier(&typec_source_switch_notifier);
	if(ret) {
		bus_unregister_notifier(s_usb_bus_type, &rtk_usb_bus_nb);
		s_usb_bus_type = NULL;
		goto EXIT;
	}
EXIT:
	return 0;
}

void rtk_usb_hub_unregister_notifier(void)
{
	if(s_usb_bus_type) {
		unregister_typec_source_switch_notifier(&typec_source_switch_notifier);	
		bus_unregister_notifier(s_usb_bus_type, &rtk_usb_bus_nb);
		s_usb_bus_type = NULL;
	}
}


