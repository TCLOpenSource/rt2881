#ifndef __RTK_USB_DEVICE_IOCTL_H__
#define __RTK_USB_DEVICE_IOCTL_H__

#include <ioctrl/scaler/uvc_cmd_id.h>

typedef struct {
    UINT8  device;
    UINT8  type;
    UINT8  channel;
    UINT32 rate;
    UINT8  bits;
    UINT32 UAC_UID;
} RTK_USB_DEVICE_UAC_SAMPLE_RATE_INFO_T;

typedef struct {
    UINT8                   format;
    UINT16                  width;
    UINT16                  height;
    UINT16                  freq;
    UINT32                  UVC_UID;
} RTK_USB_DEVICE_UVC_TIMING_INFO_T;

#define RTK_USB_DEVICE_IOC_MAGIC  'u'

#define RTK_USB_DEVICE_IOC_DELIVERY_GET_UAC_SAMPLE_RATE_INFO (_IOR (RTK_USB_DEVICE_IOC_MAGIC, 1, RTK_USB_DEVICE_UAC_SAMPLE_RATE_INFO_T))
#define RTK_USB_DEVICE_IOC_DELIVERY_GET_UVC_TIMING_INFO      (_IOR (RTK_USB_DEVICE_IOC_MAGIC, 2, RTK_USB_DEVICE_UVC_TIMING_INFO_T))

#define RTK_USB_DEVICE_IOC_MAXNR           10

long rtk_usb_device_ioctl(struct file* file, unsigned int cmd, unsigned long arg);
#endif      /* __RTK_USB_DEVICE_IOCTL_H__ */
