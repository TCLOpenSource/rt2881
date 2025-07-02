#ifndef __RTK_USB_CORE_H
#define __RTK_USB_CORE_H
#include <rtd_log/rtd_module_log.h>

#define RTK_USB_ERR(fmt, args...)      rtd_pr_rtk_usb_err(fmt, ## args)
#define RTK_USB_WARN(fmt, args...)     rtd_pr_rtk_usb_warn(fmt, ## args)
#define RTK_USB_INFO(fmt, args...)     rtd_pr_rtk_usb_info(fmt, ## args)
#define RTK_USB_DBG(fmt, args...)      rtd_pr_rtk_usb_debug(fmt, ## args)

/* IOCTL */
#define RTK_USB_IOC_MAGIC                        't'
#define RTK_USB_IOC_GET_TOTAL_CONN_NUM           _IOR(RTK_USB_IOC_MAGIC, 1, int)
#define RTK_USB_IOC_GET_OCD_STATUS               _IOR(RTK_USB_IOC_MAGIC, 2, int)
#define RTK_USB_IOC_SET_SS_SUPPORT               _IOW(RTK_USB_IOC_MAGIC, 3, int)
#define RTK_USB_IOC_SET_VBUS                     _IOW(RTK_USB_IOC_MAGIC, 4, int)
#define RTK_USB_IOC_GET_MAPPED_PORT             _IOWR(RTK_USB_IOC_MAGIC, 5, int)
#define RTK_USB_IOC_GET_U_MIC_INFO             _IOWR(RTK_USB_IOC_MAGIC, 6, U_MIC_INFO *)

/*COMPAT IOCTL*/
#define COMPAT_RTK_USB_IOC_GET_U_MIC_INFO             _IOWR(RTK_USB_IOC_MAGIC, 6, int)


typedef struct {
    int connector1;
    int status;
} OC_INFO;

typedef struct {
    int connector1;
    int status;
    bool on;
} VBUS_INFO;

typedef union {
    VBUS_INFO vbus_info;
    OC_INFO oc_info;
} PORT_INFO;

typedef struct {
    int connector1;
    int hc_num;
    int u2_port1;
    int u3_port1;
} CONN_MAP;

typedef struct {
    int total_conn_num;
    PORT_INFO port_info;
} RTK_USB_INFO;

typedef struct {
	uint8_t serial[32];
	uint8_t product_name[256];
	uint32_t get_result;
	uint16_t vid;
	uint16_t pid;
}U_MIC_INFO;

// [1:0]
#define GET_HC_NUM(map, connector_idx) \
    (((map >> (connector_idx * 8)) & 0x3))

// [4:2]
#define GET_U2_PORT1(map, connector_idx) \
    (((map >> ((connector_idx * 8) + 2)) & 0x7))

// [7:5]
#define GET_U3_PORT1(map, connector_idx) \
    (((map >> ((connector_idx * 8) + 5)) & 0x7))


struct bus_type * rtk_usb_get_bus_type(void);

struct wifi_port_info
{
	bool is_ehci_or_xhci_port; //ehci:true, xhci:false;
	struct usb_hcd * hcd_priv;
	unsigned int port_num;
};

bool get_usb_device_controller(struct usb_device *udev, struct wifi_port_info *port_info);
void rtk_usb_disconnect_usb_port(struct wifi_port_info *port_info);
bool rtk_is_usb_device_under_root_port(struct device *dev);


#endif /* __RTK_USB_CORE_H */
