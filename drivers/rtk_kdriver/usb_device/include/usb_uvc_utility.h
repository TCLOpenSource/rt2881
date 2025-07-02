#ifndef __RTK_USB_UVC_UTILITY_H__
#define __RTK_USB_UVC_UTILITY_H__

void rtk_uvc_process_processing_unit_in(struct usb_ctrlrequest *ctrl_req);
void set_uvc_processing_unit_support_control(void)  ;
UINT8 is_uvc_processing_unit_support_control(void);
UINT8 usb_uvc_check_format_frame_info(void);

//////////////////////////////
// UVC format
//////////////////////////////
typedef enum
{
    UVC_NONE,
    UVC_NV12,
    UVC_M420,// not support
    UVC_I420,
    UVC_YUYV,
    //UVC_UYVY,// not support
    UVC_RGB24,
    UVC_RGB32,
    UVC_P010,  // P010
    UVC_MJPEG, // MJPEG
} UVC_FORMAT;

typedef struct {
    UINT32     weight;     // weight
    UINT32     height;     // height
    UINT32     Freq;       // Freq
    UVC_FORMAT format;     // format
} AT_UVC_TIMING_INFO_T;


#endif // __RTK_USB_UVC_UTILITY_H__
