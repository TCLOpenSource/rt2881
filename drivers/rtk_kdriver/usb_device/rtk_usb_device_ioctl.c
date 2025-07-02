#include <linux/module.h>
#include "mach.h"
#include "rtk_io.h"
#include "rtk_usb_device_dbg.h"
#include "rtk_usb_device_ioctl.h"

UINT32 uac_uid = 0;

int IOCTL_RTK_USB_Device_Get_UAC_SampleRate_Info(RTK_USB_DEVICE_UAC_SAMPLE_RATE_INFO_T *pSampleRateInfo)
{
    RTK_USB_DEVICE_DBG_SIMPLE("IOCTL_RTK_USB_Device_Get_UAC_SampleRate_Info\n");

    pSampleRateInfo->device  = 0;
    pSampleRateInfo->type    = 0;
    pSampleRateInfo->channel = 2;
    pSampleRateInfo->rate    = 48000;
    pSampleRateInfo->bits    = 16;
    pSampleRateInfo->UAC_UID = uac_uid;
    uac_uid++;
    return 0;
}

extern UINT8 usb_uvc_format_real;
extern UINT16 usb_uvc_width_real;
extern UINT16 usb_uvc_length_real;
extern UINT16 usb_uvc_freq_real;
extern UINT32 uvc_uid;

static const UINT8 format_mapping_table[] = {_NONE, _YUYV, _MotionJPEG, _NV12, _I420, _RGB24, _RGB32, _P010};

int IOCTL_RTK_USB_Device_Get_UVC_Timing_Info(RTK_USB_DEVICE_UVC_TIMING_INFO_T *pTimingInfo)
{
    //RTK_USB_DEVICE_DBG_SIMPLE("IOCTL_RTK_USB_Device_Get_UVC_Timing_Info\n");

    pTimingInfo->format  = format_mapping_table[usb_uvc_format_real];
    pTimingInfo->width   = usb_uvc_width_real;
    pTimingInfo->height  = usb_uvc_length_real;
    pTimingInfo->freq    = usb_uvc_freq_real;
    pTimingInfo->UVC_UID = uvc_uid;
    return 0;
}

long rtk_usb_device_ioctl(        struct file*   file, unsigned int cmd, unsigned long arg)
{
    int ret = -ENOTTY;

    if (_IOC_TYPE(cmd) != RTK_USB_DEVICE_IOC_MAGIC || _IOC_NR(cmd) > RTK_USB_DEVICE_IOC_MAXNR)
        return -ENOTTY;

    switch (cmd) {

        case RTK_USB_DEVICE_IOC_DELIVERY_GET_UAC_SAMPLE_RATE_INFO:
        {
            RTK_USB_DEVICE_UAC_SAMPLE_RATE_INFO_T data;

            if(copy_from_user((void *)&data, (const void __user *)arg, sizeof(RTK_USB_DEVICE_UAC_SAMPLE_RATE_INFO_T)))  {
                RTK_USB_DEVICE_DBG_SIMPLE("ioctl RTK_USB_DEVICE_IOC_DELIVERY_GET_UAC_SAMPLE_RATE_INFO failed 1\n");
                ret = -EFAULT;
            } else {
                ret = IOCTL_RTK_USB_Device_Get_UAC_SampleRate_Info(&data);
            }

            if (copy_to_user((void __user *)arg, (void *)&data, sizeof(RTK_USB_DEVICE_UAC_SAMPLE_RATE_INFO_T)))  {
                RTK_USB_DEVICE_DBG_SIMPLE("ioctl RTK_USB_DEVICE_IOC_DELIVERY_GET_UAC_SAMPLE_RATE_INFO failed 2\n");
                ret = -EFAULT;
            }

            break;
        }

        case RTK_USB_DEVICE_IOC_DELIVERY_GET_UVC_TIMING_INFO:
        {
            RTK_USB_DEVICE_UVC_TIMING_INFO_T data;

            if(copy_from_user((void *)&data, (const void __user *)arg, sizeof(RTK_USB_DEVICE_UVC_TIMING_INFO_T)))  {
                RTK_USB_DEVICE_DBG_SIMPLE("ioctl RTK_USB_DEVICE_IOC_DELIVERY_GET_UVC_TIMING_INFO failed 1\n");
                ret = -EFAULT;
            } else {
                ret = IOCTL_RTK_USB_Device_Get_UVC_Timing_Info(&data);
            }

            if (copy_to_user((void __user *)arg, (void *)&data, sizeof(RTK_USB_DEVICE_UVC_TIMING_INFO_T)))  {
                RTK_USB_DEVICE_DBG_SIMPLE("ioctl RTK_USB_DEVICE_IOC_DELIVERY_GET_UVC_TIMING_INFO failed 2\n");
                ret = -EFAULT;
            }

            break;
        }

        default:
            RTK_USB_DEVICE_DBG_SIMPLE("RTK_USB_DEVICE: unknown ioctl(0x%08x)\n", cmd);
            break;
    }
    return ret;
}
