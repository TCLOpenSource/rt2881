#ifndef __RTK_USB_UVC_API_H__
#define __RTK_USB_UVC_API_H__

#include "usb_cdc.h"

#define USB_UVC_DEBUG_STATUS_MONITOR_TIME_MS 1000

//#define UVC_TRB_SIZE_UNCOMPRESSION 4096
#define UVC_TRB_SIZE_UNCOMPRESSION 16384
#define UVC_TRB_SIZE_MJPEG         16384

#define USB_UVC_TRB_SIZE     16
#define USB_UVC_TRB_LINK_CNT 1

#define USB_UVC_HEADER_REAL_SIZE 12

#define USB_UVC_DATA_BUFFER_GROUP_CNT 3
#define USB_UVC_LINK_TRB_CNT          1
#define USB_UVC_HEADER_SIZE           16
#define USB_UVC_HEADER_CNT            1
#define USB_UVC_HEADER_DMA_BUFFER_SIZE USB_UVC_HEADER_SIZE * USB_UVC_DATA_BUFFER_GROUP_CNT

#define UVC_EXTENSION_BUF_SIZE 150

#define UVC_EXTENSION_CONTROL_SELECTOR_DATA                   0x0100
#define UVC_EXTENSION_CONTROL_SELECTOR_LENGTH                 0x0200
#define UVC_EXTENSION_CONTROL_SELECTOR_DATA_LENGTH_FIXED      0x0300
#define UVC_EXTENSION_CONTROL_SELECTOR_DATA_LENGTH_FIXED_LONG 0x0400

#define UVC_EXTENSION_CONTROL_SELECTOR_LENGTH_SIZE 2
#define UVC_EXTENSION_GET_INFO_SIZE 1
#define UVC_EXTENSION_GET_LENGTH_SIZE 2

#define UVC_EXTENSION_DEVICE_NODE 3

#define UVC_EP_NUMBER 5

#define UVC_FRAME_BUFFER_NUMBER 3
#define UVC_FRAME_BUFFER_SIZE   0xFD2000
#define UVC_HEADER_BUFFER_SIZE  0x10000
#define UVC_TRB_BUFFER_SIZE     0x10000

//#define UVC_HEADER_BUFFER_ADDR         0x1a3043f0+ 0x1850
//#define UVC_DATA_BUFFER_ADDR           UVC_HEADER_BUFFER_ADDR + 0x30
//#define UVC_TRB_BUFFER_ADDR            UVC_DATA_BUFFER_ADDR + 0xc000

// USB UVC Un-Compressed Data Buffer Size
#define USB_UVC_UC_D_SIZE 4096
// USB UVC Un-Compressed Data Buffer Count
#define USB_UVC_UC_D_CNT 4
// USB UVC Un-Compressed TRB Buffer Size
#define USB_UVC_UC_T_SIZE 16
// USB UVC Un-Compressed TRB Buffer Count
#define USB_UVC_UC_T_CNT  (USB_UVC_HEADER_CNT + USB_UVC_UC_D_CNT) * USB_UVC_DATA_BUFFER_GROUP_CNT + USB_UVC_LINK_TRB_CNT

// USB UVC Compressed Data Buffer Size
#define USB_UVC_D_SIZE 16384
// USB UVC Compressed Data Buffer Count
#define USB_UVC_D_CNT 1
// USB UVC Compressed TRB Buffer Size
#define USB_UVC_T_SIZE 16
// USB UVC Compressed TRB Buffer Count
#define USB_UVC_T_CNT  (USB_UVC_HEADER_CNT + USB_UVC_D_CNT) * USB_UVC_DATA_BUFFER_GROUP_CNT + USB_UVC_LINK_TRB_CNT


#if defined(CONFIG_ENABLE_TV_SCALER)
#define CONFIG_SUPPORT_UVC_PROCESSING_UNIT_CONTROL
#endif

//****************************************************************************
// UVC INTERFACE
//****************************************************************************
#define UVC_RC_UNDEFINED                                0x00
#define UVC_SET_CUR                                     0x01
#define UVC_GET_CUR                                     0x81
#define UVC_GET_MIN                                     0x82
#define UVC_GET_MAX                                     0x83
#define UVC_GET_RES                                     0x84
#define UVC_GET_LEN                                     0x85
#define UVC_GET_INFO                                    0x86
#define UVC_GET_DEF                                     0x87


/* A.9.5. Processing Unit Control Selectors */
#define UVC_PU_CONTROL_UNDEFINED			0x00
#define UVC_PU_BACKLIGHT_COMPENSATION_CONTROL		0x01
#define UVC_PU_BRIGHTNESS_CONTROL			0x02
#define UVC_PU_CONTRAST_CONTROL				0x03
#define UVC_PU_GAIN_CONTROL				0x04
#define UVC_PU_POWER_LINE_FREQUENCY_CONTROL		0x05
#define UVC_PU_HUE_CONTROL				0x06
#define UVC_PU_SATURATION_CONTROL			0x07
#define UVC_PU_SHARPNESS_CONTROL			0x08
#define UVC_PU_GAMMA_CONTROL				0x09
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL	0x0a
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL	0x0b
#define UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL		0x0c
#define UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL	0x0d
#define UVC_PU_DIGITAL_MULTIPLIER_CONTROL		0x0e
#define UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL		0x0f
#define UVC_PU_HUE_AUTO_CONTROL				0x10
#define UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL		0x11
#define UVC_PU_ANALOG_LOCK_STATUS_CONTROL		0x12


/* A.9.7. VideoStreaming Interface Control Selectors */
#define UVC_VS_CONTROL_UNDEFINED                        0x00
#define UVC_VS_PROBE_CONTROL                            0x01
#define UVC_VS_COMMIT_CONTROL                           0x02
#define UVC_VS_STILL_PROBE_CONTROL                      0x03
#define UVC_VS_STILL_COMMIT_CONTROL                     0x04
#define UVC_VS_STILL_IMAGE_TRIGGER_CONTROL              0x05
#define UVC_VS_STREAM_ERROR_CODE_CONTROL                0x06
#define UVC_VS_GENERATE_KEY_FRAME_CONTROL               0x07
#define UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL             0x08
#define UVC_VS_SYNC_DELAY_CONTROL                       0x09

typedef enum _DC2H_CAPTURE_TYPE
{
    _NONE = 0,
    _NV12,
    _M420,
    _I420,
    _YUYV,
    _RGB24,
    _RGB32,
    _P010,
    _MotionJPEG,
#if (defined FPGA_BRING_UP_TEST) || (defined UT_flag)
    _FRC_444_10B,
    _FRC_422_10B,
    _FRC_444_8B,
    _FRC_422_8B,
#endif
} DC2H_CAPTURE_TYPE_Y;

typedef enum _USB_UVC_CAPTURE_TYPE
{
    _USB_UVC_NONE = 0,
    _USB_UVC_YUY2,               // 1
    _USB_UVC_MotionJPEG,         // 2
    _USB_UVC_NV12,               // 3
    _USB_UVC_I420,               // 4
    _USB_UVC_RGB24,              // 5
    _USB_UVC_RGB32,              // 6
    _USB_UVC_P010,               // 7
    _USB_UVC_IYU2,               // 8
    _USB_UVC_AYUV,               // 9
    _USB_UVC_V410,               // 10
    _USB_UVC_Y410,               // 11
    _USB_UVC_UYVY,               // 12
    _USB_UVC_V210,               // 13
    _USB_UVC_Y210,               // 14
    _USB_UVC_YV12,               // 15
    _USB_UVC_RGB565,             // 16
    _USB_UVC_RGB555,             // 17
} USB_UVC_CAPTURE_TYPE_Y;

typedef enum _UVC_PACK_WRAP_MODE
{
    _UVC_PACK_WRAP_MODE_IYU2,         // 0
    _UVC_PACK_WRAP_MODE_AYUV,         // 1
    _UVC_PACK_WRAP_MODE_V410,         // 2
    _UVC_PACK_WRAP_MODE_Y410,         // 3
    _UVC_PACK_WRAP_MODE_YUY2,         // 4
    _UVC_PACK_WRAP_MODE_RESERVED_5,   // 5
    _UVC_PACK_WRAP_MODE_UYVY,         // 6
    _UVC_PACK_WRAP_MODE_V210,         // 7
    _UVC_PACK_WRAP_MODE_Y210,         // 8
    _UVC_PACK_WRAP_MODE_NV12,         // 9
    _UVC_PACK_WRAP_MODE_P010,         // 10
    _UVC_PACK_WRAP_MODE_I420,         // 11
    _UVC_PACK_WRAP_MODE_YV12,         // 12
    _UVC_PACK_WRAP_MODE_RGB565,       // 13
    _UVC_PACK_WRAP_MODE_RGB555,       // 14
    _UVC_PACK_WRAP_MODE_RGB24,        // 15
    _UVC_PACK_WRAP_MODE_RGB32,        // 16
} UVC_PACK_WRAP_MODE_Y;

//****************************************************************************
// CODE TABLES
//****************************************************************************
typedef enum MSG_Q_SUB_UVC {
    MSG_UVC_MONITOR = 0,        // 0
    MAX_MSG_Q_SUB_UVC_COUNT,    // 5
} _MSG_Q_SUB_UVC;

struct uvc_control_selector {
        UINT8  bGetRequest: 1;
        UINT8  bSetRequest: 1;
        UINT8  bDisableAutoMode: 1;
        UINT8  bAutoUpdateControl: 1;
        UINT8  bAsyncControl: 1;
        UINT8  bReserved1: 1;
        UINT8  bReserved2: 1;
        UINT8  bReserved3: 1;
};

#pragma pack(push,1)
struct uvc_cur_def {
        unsigned short bmHint;
        unsigned char  bFormatIndex;
        unsigned char  bFrameIndex;
        unsigned int   bFrameInterval;
        unsigned short wKeyFrameRate;
        unsigned short wPFrameRate;
        unsigned short wCompQuality;
        unsigned short wCompWindowSize;
        unsigned short wDelay;
        unsigned int   dwMaxVideoFrameSize;
        unsigned int   dwMaxPayloadTransferSize;
};
#pragma pack(pop)

struct uvc_ring_log_buf
{
        UINT16 write_pos;
        UINT16 read_pos;
#ifdef CONFIG_ARCH_5281
        UINT8 *buf;
#else
        UINT32 buf;
#endif
};

struct uvc_extension
{
    UINT8 *recv_buf;
    struct uvc_ring_log_buf log_buf;
};

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
UINT8 usb_uvc_init(void);
UINT8 usb_uvc_connect(UINT8 ep);
UINT8 usb_uvc_disconnect(UINT8 ep, UINT8 clean_information);
UINT8 usb_control_uvc_request_process(UINT8 ep);
UINT8 usb_uvc_set_timing_info(UINT8 format_index, UINT8 frame_index, UINT16 frame);
void rtk_uvc_control_interface(struct usb_ctrlrequest *ctrl_req);
void rtk_uvc_stream_interface(struct usb_ctrlrequest *ctrl_req);
void usb_uvc_monitor(void);
UINT8 usb_uvc_event_state_initial(void);
UINT16 uvc_extension_tx_directly(UINT8 *buf, UINT16 len);
void usb_uvc_set_cur(UINT8 actual_len);

void setting_uvc(void);


#endif // __RTK_USB_UVC_API_H__
