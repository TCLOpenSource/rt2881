/*----------------------------------------------------------------------------------
* v4l2 vt(video texture) driver including header files
* creat 2023-7-25 in CS
* Copyright(c) 2023 by Realtek.
-----------------------------------------------------------------------------------*/

/*------------------------- device introduction   ------------------------------------
* total device: 6
* I3ddma: /dev/vtdev10
    #define V4L2_EXT_DEV_NO_CAPTURE1 10
    #define V4L2_EXT_DEV_PATH_CAPTURE1 "/dev/vtdev10"
* I4ddma: /dev/vtdev20
    #define V4L2_EXT_DEV_NO_CAPTURE2 20
    #define V4L2_EXT_DEV_PATH_CAPTURE2 "/dev/vtdev20"
* I5ddma: /dev/vtdev30
    #define V4L2_EXT_DEV_NO_CAPTURE3 30
    #define V4L2_EXT_DEV_PATH_CAPTURE3 "/dev/vtdev30"
* I6ddma: /dev/vtdev40
    #define V4L2_EXT_DEV_NO_CAPTURE4 40
    #define V4L2_EXT_DEV_PATH_CAPTURE4 "/dev/vtdev40"
* DC2H1 : /dev/vtdev50
    #define V4L2_EXT_DEV_NO_CAPTURE5 50
    #define V4L2_EXT_DEV_PATH_CAPTURE5 "/dev/vtdev50"
* DC2H2 : /dev/vtdev60
    #define V4L2_EXT_DEV_NO_CAPTURE6 60
    #define V4L2_EXT_DEV_PATH_CAPTURE6 "/dev/vtdev60"
-----------------------------------------------------------------------------------*/

#ifndef _V4L2_EXT_VT_H
#define _V4L2_EXT_VT_H

#ifndef BUILD_QUICK_SHOW
#include <linux/v4l2-controls.h>
#endif

#define V4L2_EXT_DEV_NO_CAPTURE1 10
#define V4L2_EXT_DEV_NO_CAPTURE2 20
#define V4L2_EXT_DEV_NO_CAPTURE3 30
#define V4L2_EXT_DEV_NO_CAPTURE4 40
#define V4L2_EXT_DEV_NO_CAPTURE5 50
#define V4L2_EXT_DEV_NO_CAPTURE6 60

/*ixddma devide name*/
#define V4L2_EXT_DEV_PATH_CAPTURE1   "/dev/video10"
#define V4L2_EXT_DEV_PATH_CAPTURE2   "/dev/video20"
#define V4L2_EXT_DEV_PATH_CAPTURE3   "/dev/video30"
#define V4L2_EXT_DEV_PATH_CAPTURE4   "/dev/video40"

/*vt module dc2h1 and dc2h2 device name */
#define V4L2_EXT_DEV_PATH_CAPTURE5 "/dev/video50"
#define V4L2_EXT_DEV_PATH_CAPTURE6 "/dev/video60"


#define V4L2_CID_USER_EXT_CAPTURE_BASE (V4L2_CID_USER_BASE + 0x2100)

/* About v4l2 apis returns error should reference below error code
*  Generic error codes included in below file path
*  linux-5.4/Documentation/media/uapi/gen-errors.rst
*/


enum v4l2_ext_capture_video_frame_buffer_pixel_format {
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB = 0,  //RGB 
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ARGB,
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGBA,
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ABGR,
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit,  //4 -> NV12 8bits
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_10bit, //NV12 10bits
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_P010,  //P010 10bit
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV21,
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YV12, //YV12 8bits
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_I420, //I420 8bit
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_MAXN
};

enum v4l2_ext_capture_video_frame_buffer_plane_num {
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED = 1, // INTERLEAVED has one plane.
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR, // SEMI PLANAR has two plane.
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_PLANAR // PLANAR has three plane.
};

struct v4l2_ext_video_rect {
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
};

/* Flags for the 'flags' field in struct v4l2_ext_capture_capability_info. 
* so the flags = below definitions OR operation,
* for example: flags = V4L2_EXT_CAPTURE_CAP_INPUT_VIDEO_DEINTERLACE | V4L2_EXT_CAPTURE_CAP_DISPLAY_VIDEO_DEINTERLACE;(bit0,bit1 set to 1)
* Explanations of the below define
*/

/*Explanation for flags (ex: interlace signal input 1280x720i@60hz(fps))
* If the scan type of captured frame on V4L2_EXT_CAPTURE_SCALER_OUTPUT is progressive, 
should set this bit in the variable of flags. The frame size of height is 720.
If the scan type of captured frame on V4L2_EXT_CAPTURE_SCALER_OUTPUT is still interlace 
because the capture position is located before the de-interlace, should unset this bit 
in the variable of flags.The frame size of height is 720/2=360.
*/
#define V4L2_EXT_CAPTURE_CAP_INPUT_VIDEO_DEINTERLACE 0x0001  //bit 0


/* If the scan type of captured frame on V4L2_EXT_CAPTURE_DISPLAY_OUTPUT is progressive,
should set this bit in the variable of flags. The frame size of height is 720.
If the scan type of captured frame on V4L2_EXT_CAPTURE_DISPLAY_OUTPUT is still interlace 
because the capture position is located before the de-interlace, should unset this bit 
in the variable of flags. The frame size of height is 720/2=360.
*/
#define V4L2_EXT_CAPTURE_CAP_DISPLAY_VIDEO_DEINTERLACE 0x0002  //bit 1


/*
When user client request the size of capture frame with 1920x1080, if the vendor can 
capture those size after up-scaling, should set this bit in the variable of flags.
*/
#define V4L2_EXT_CAPTURE_CAP_SCALE_UP 0x0004   //bit 2


/*
When user client request the size of capture frame with 640x480, if the vendor can 
capture those size after down-scaling, you should set this bit in the variable of flags.
*/
#define V4L2_EXT_CAPTURE_CAP_SCALE_DOWN 0x0008  //bit 3


/*
When user client request to divide the fps of capture frame to 60hz or 30hz, if the vendor 
can do dividing the fps, should set this bit in the variable of flags.
*/
#define V4L2_EXT_CAPTURE_CAP_DIVIDE_FRAMERATE 0x0010  //bit 4


/*
If the driver supports the feature of VTV, please set this bit to '1'.
Currently in case of other vendors, please set this bit to '0'.
*/
#define V4L2_EXT_CAPTURE_CAP_VIDEOTOVIDEO 0x0020

#define V4L2_CID_EXT_CAPTURE_CAPABILITY_INFO  (V4L2_CID_USER_EXT_CAPTURE_BASE + 0)


struct v4l2_ext_capture_capability_info {
    unsigned int flags;
    unsigned int scale_up_limit_w;
    unsigned int scale_up_limit_h;
    unsigned int scale_down_limit_w;
    unsigned int scale_down_limit_h;
    unsigned int num_video_frame_buffer;
    struct v4l2_ext_video_rect max_res;
    enum v4l2_ext_capture_video_frame_buffer_plane_num num_plane;
    enum v4l2_ext_capture_video_frame_buffer_pixel_format pixel_format;
};


#define V4L2_CID_EXT_CAPTURE_PLANE_INFO (V4L2_CID_USER_EXT_CAPTURE_BASE + 1)  //controls->id

enum v4l2_ext_capture_video_scan_type {
    V4L2_EXT_CAPTURE_VIDEO_INTERLACED = 0,
    V4L2_EXT_CAPTURE_VIDEO_PROGRESSIVE
};

/* plane information parameter:
@ type: video scan type (interlaced or progressive)
@ in : set capture input region(maybe crop)
@ out: set capture output region(real capture size)
@ panel: panel size
@ port: hdmi_port
*/
struct v4l2_ext_capture_plane_info{
    //unsigned int stride;
    enum v4l2_ext_capture_video_scan_type type;
    struct v4l2_ext_video_rect in;
    struct v4l2_ext_video_rect out;
    struct v4l2_ext_video_rect panel;
    unsigned int port;
    //struct v4l2_ext_video_rect plane_region;
    //struct v4l2_ext_video_rect active_region;
};

struct v4l2_ext_capture_video_win_info {
    enum v4l2_ext_capture_video_scan_type type;
    struct v4l2_ext_video_rect in;
    struct v4l2_ext_video_rect out;
    struct v4l2_ext_video_rect panel;
};

enum v4l2_ext_capture_location {
    V4L2_EXT_CAPTURE_SCALER_INPUT = 0,
    V4L2_EXT_CAPTURE_SCALER_OUTPUT,
    V4L2_EXT_CAPTURE_DISPLAY_OUTPUT,
    V4L2_EXT_CAPTURE_BLENDED_OUTPUT,
    V4L2_EXT_CAPTURE_OSD_OUTPUT,
    V4L2_EXT_CAPTURE_HDMI_RX,
    V4L2_EXT_CAPTURE_DP_RX,
    V4L2_EXT_CAPTURE_VODMA,
};

enum v4l2_ext_capture_buf_location {
    V4L2_EXT_CAPTURE_INPUT_BUF = 0,
    V4L2_EXT_CAPTURE_OUTPUT_BUF
};

struct v4l2_ext_capture_plane_prop {
    enum v4l2_ext_capture_location l;
    struct v4l2_ext_video_rect plane;
    int buf_count;
};

struct v4l2_ext_capture_freeze_mode {
    int plane_index;
    unsigned char val;
};

struct v4l2_ext_capture_physical_memory_info {
    int buf_index;
    union {
        unsigned int *y;
        unsigned int compat_y_data;
        unsigned long long y_sizer;
    };

    union {
        unsigned int *u;
        unsigned int compat_u_data;
        unsigned long long u_sizer;
    };

    union {
        unsigned int *v;
        unsigned int compat_v_data;
        unsigned long long v_sizer;
    };
    enum v4l2_ext_capture_buf_location buf_location;
};

/**
 * @brief Set video window.
 *
 * @rst
 * Functional Requirements
 *   This is a command for obtaining information on the current input, output
 *   video, and panel size. This command can be used at any time in the User
 *   Client without any command or dependency when the VT Video Device is open.
 *   User client can read(VIDIOC_G_EXT_CTLRS) only when user client set the
 *   capture location to V4L2_EXT_CAPTURE_SCALER_OUTPUT or
 *   V4L2_EXT_CAPTURE_DISPLAY_OUTPUT. In this case, the values of the structure
 *   are used as shown below.
 *
 *   1. struct v4l2_ext_video_rect in;
 *    - Current input resolution of scaler.
 *    - Read only
 *
 *   2. struct v4l2_ext_video_rect out;
 *    - Current output resolution of scaler.
 *    - Read only
 *
 *   3. struct v4l2_ext_video_rect panel;
 *    - The size of panel
 *    - Read only
 *
 *   User client can read(VIDIOC_G_EXT_CTLRS) 'in' and 'out', 'panel',
 *   set(VIDIOC_S_EXT_CTLRS) the value of 'out' when user client set the capture
 *   location to V4L2_EXT_CAPTURE_SCALER_INPUT(VTV). In this case, the values of
 *   the structure are used as shown below.
 *
 *   1. struct v4l2_ext_video_rect in;
 *    - Current input resolution of scaler.
 *    - Read only
 *
 *   2. struct v4l2_ext_video_rect out;
 *    - Current output size of vt output buffer.
 *    - Read, Write
 *
 *   3. struct v4l2_ext_video_rect panel;
 *    - The size of panel
 *    - Read only
 *
 * Responses to abnormal situations, including
 *   If abnormal data is set, the driver should return an error.
 *   The generic error codes are described at the :ref:`gen_errors` chapter.
 *
 * Performance Requirements
 *   The response time must respond within 10ms, unless there is a special reason.
 *
 * Constraints
 *   None
 *
 * Functions & Parameters
 *   .. code-block:: cpp
 *
 *     //
 *     // ioctl command
 *     //
 *     VIDIOC_S_EXT_CTRLS
 *     VIDIOC_G_EXT_CTRLS
 *
 *     //
 *     // control id
 *     //
 *     // You have to make the new control id as the below.
 *     V4L2_CID_EXT_CAPTURE_VIDEO_WIN_INFO
 *
 *     //
 *     // parameter
 *     //
 *     struct v4l2_ext_video_rect
 *     {
 *         unsigned short x; // horizontal start position
 *         unsigned short y; // vertical start position
 *         unsigned short w; // horizontal size(width)
 *         unsigned short h; // vertical size(height)
 *     };
 *
 *     enum v4l2_ext_capture_video_scan_type
 *     {
 *         V4L2_EXT_CAPTURE_VIDEO_INTERLACED = 0,
 *         V4L2_EXT_CAPTURE_VIDEO_PROGRESSIVE
 *     };
 *
 *     struct v4l2_ext_capture_video_win_info
 *     {
 *         enum v4l2_ext_capture_video_scan_type type;
 *         struct v4l2_ext_video_rect in;
 *         struct v4l2_ext_video_rect out;
 *         struct v4l2_ext_video_rect panel;
 *     };
 *
 * Return Value
 *   On success 0 is returned. On error -1 and the errno variable is set
 *   appropriately. The generic error codes are described at the
 *   :ref:`gen_errors` chapter.
 *
 * Control Type
 *   Application can get data using VIDIOC_G_EXT_CTRLS and this control id.
 *  
 *
 * Example
 *   .. code-block:: cpp
 *
 *     struct v4l2_ext_capture_plane_info vt_palne_info;
 *     struct v4l2_ext_controls ext_controls;
 *     struct v4l2_ext_control ext_control;
 *     int ret;
 *
 *     memset(&ext_controls, 0, sizeof(struct v4l2_ext_controls));
 *     memset(&ext_control, 0, sizeof(struct v4l2_ext_control));
 *
 *     ext_controls.ctrl_class     = V4L2_CTRL_CLASS_USER;
 *     ext_controls.count          = 1;
 *     ext_controls.controls       = &ext_control;
 *     ext_controls.controls->id   = V4L2_CID_EXT_CAPTURE_PLANE_INFO;
 *     ext_controls.controls->size = sizeof(struct v4l2_ext_capture_plane_info);
 *     ext_controls.controls->ptr  = (void *)&vt_palne_info;
 *
 *     ret = ioctl(fd, VIDIOC_G_EXT_CTRLS, &ext_controls);
 *
 * @endrst
 */
#define V4L2_CID_EXT_CAPTURE_VIDEO_WIN_INFO (V4L2_CID_USER_EXT_CAPTURE_BASE + 2)

/**
 * @brief Set/get properties.
 *
 * @rst
 * Functional Requirements
 *   This command is used to set the position to be captured. The struct
 *   v4l2_ext_video_rect plane and int buf_count are not referenced as reserved
 *   areas. Uses the value of struct v4l2_ext_video_rect plane
 *   and int buf_count. Capture position uses the following structure defined by
 *   user.
 *
 *   Each position follows the following H / W block.
 *     .. image:: /v4l2/video-texture-V4L2_CID_EXT_CAPTURE_PLANE_PROP_v2.png
 *   1. V4L2_EXT_CAPTURE_SCALER_INPUT
 *     - uses this location only when VTV mode.
 *     - If not supported, the ioctl should be returned -1 and the errno variable should be set to ENOTSUP.
 *   2. V4L2_EXT_CAPTURE_SCALER_OUTPUT
 *     - Recommend location
 *         - Before PQ processing, after de-interlace block
 *         - The frame is a progressive type.
 *     - Alternative location due to chip limitation
 *         - Before PQ processing, before de-interlace block
 *         - The frame is an interlace type.
 *   3. V4L2_EXT_CAPTURE_DISPLAY_OUTPUT
 *     - Capture a video after PQ processing and before OSD blending.
 *     - The frame size calculated with ARC applied.
 *   4. V4L2_EXT_CAPTURE_BLENDED_OUTPUT
 *     - Capture a video after OSD blending.
 *     - If not supported, the ioctl should be returned -1 and the errno variable should be set to ENOTSUP.
 *   5. V4L2_EXT_CAPTURE_OSD_OUTPUT
 *     - Capture a OSD output.
 *     - If not supported, the ioctl should be returned -1 and the errno variable should be set to ENOTSUP.
 *
 *   Struct
 *     .. code-block:: cpp
 *
 *       enum v4l2_ext_capture_location
 *       {
 *           V4L2_EXT_CAPTURE_SCALER_INPUT = 0, // for VTV - before de-interlace
 *           V4L2_EXT_CAPTURE_SCALER_OUTPUT,
 *           V4L2_EXT_CAPTURE_DISPLAY_OUTPUT,
 *           V4L2_EXT_CAPTURE_BLENDED_OUTPUT,
 *           V4L2_EXT_CAPTURE_OSD_OUTPUT
 *       };
 *
 * Responses to abnormal situations, including
 *   If abnormal data is set, the driver should return an error.
 *   The generic error codes are described at the :ref:`gen_errors` chapter.
 *
 * Performance Requirements
 *   The response time must respond within 200ms, unless there is a special reason.
 *
 * Constraints
 *   
 *
 * Functions & Parameters
 *   .. code-block:: cpp
 *
 *     //
 *     // ioctl command
 *     //
 *     VIDIOC_S_EXT_CTLRS
 *     VIDIOC_S_EXT_CTLRS
 *
 *     //
 *     // control id
 *     //
 *     // You have to make the new control id as the below.
 *     V4L2_CID_EXT_CAPTURE_PLANE_PROP
 *
 *     //
 *     // parameter
 *     //
 *     struct v4l2_ext_video_rect
 *     {
 *         unsigned short x; // horizontal start position
 *         unsigned short y; // vertical start position
 *         unsigned short w; // horizontal size(width)
 *         unsigned short h; // vertical size(height)
 *     };
 *
 *     enum v4l2_ext_capture_location
 *     {
 *         V4L2_EXT_CAPTURE_SCALER_INPUT = 0, // for VTV - before de-interlace
 *         V4L2_EXT_CAPTURE_SCALER_OUTPUT,
 *         V4L2_EXT_CAPTURE_DISPLAY_OUTPUT,
 *         V4L2_EXT_CAPTURE_BLENDED_OUTPUT,
 *         V4L2_EXT_CAPTURE_OSD_OUTPUT
 *     };
 *
 *     struct v4l2_ext_capture_plane_prop
 *     {
 *         enum v4l2_ext_capture_location l;
 *         struct v4l2_ext_video_rect plane;
 *         int buf_count;
 *     };
 *
 * Return Value
 *   On success 0 is returned. On error -1 and the errno variable is set
 *   appropriately. The generic error codes are described at the
 *   :ref:`gen_errors` chapter.
 *
 * Control Type
 *   Application can get data using VIDIOC_G_EXT_CTRLS/VIDIOC_S_EXT_CTRLS and
 *   this control id.
 *
 * Example
 *   .. code-block:: cpp
 *
 *     struct v4l2_ext_capture_plane_prop p;
 *     struct v4l2_ext_controls ext_controls;
 *     struct v4l2_ext_control ext_control;
 *     int ret;
 *
 *     memset(&ext_controls, 0, sizeof(struct v4l2_ext_controls));
 *     memset(&ext_control, 0, sizeof(struct v4l2_ext_control));
 *     memset(&p, 0, sizeof(struct v4l2_ext_capture_plane_prop));
 *
 *     p.l = V4L2_EXT_CAPTURE_SCALER_OUTPUT;
 *
 *     ext_controls.ctrl_class = V4L2_CTRL_CLASS_USER;
 *     ext_controls.count = 1;
 *     ext_controls.controls = &ext_control;
 *     ext_controls.controls->id = V4L2_CID_EXT_CAPTURE_PLANE_PROP;
 *     ext_controls.controls->ptr = (void *)&p;
 *
 *     ioctl(fd, VIDIOC_S_EXT_CTRLS, &ext_controls);
 *
 * @endrst
 */
#define V4L2_CID_EXT_CAPTURE_PLANE_PROP (V4L2_CID_USER_EXT_CAPTURE_BASE + 3)

/**
 * @brief Set/get freeze.
 *
 * @rst
 * Functional Requirements
 *   It is a freeze command to temporarily stop updating video frames in the
 *   capture buffer.
 *
 * Responses to abnormal situations, including
 *   If abnormal data is set, the driver should return an error.
 *   The generic error codes are described at the :ref:`gen_errors` chapter.
 *
 * Performance Requirements
 *   The response time must respond within 10ms, unless there is a special reason.
 *
 * Constraints
 *   None
 *
 * Functions & Parameters
 *   .. code-block:: cpp
 *
 *     //
 *     // ioctl command
 *     //
 *     VIDIOC_S_EXT_CTLRS
 *     VIDIOC_G_EXT_CTLRS
 *
 *     //
 *     // control id
 *     //
 *     // You have to make the new control id as the below.
 *     V4L2_CID_EXT_CAPTURE_FREEZE_MODE
 *
 *     //
 *     // parameter
 *     //
 *     struct v4l2_ext_capture_freeze_mode
 *     {
 *         int plane_index; //buffer index
 *         unsigned char val;
 *     }
 *
 * Return Value
 *   On success 0 is returned. On error -1 and the errno variable is
 *   set appropriately. The generic error codes are described at the
 *   :ref:`gen_errors` chapter.
 *
 * Control Type
 *   Application can get data using VIDIOC_S_EXT_CTRLS/VIDIOC_G_EXT_CTRLS and
 *   this control id.
 *
 * Example
 *   .. code-block:: cpp
 *
 *     struct v4l2_ext_controls ext_controls;
 *     struct v4l2_ext_control ext_control;
 *     struct v4l2_ext_capture_freeze_mode m;
 *
 *     memset(&ext_controls, 0, sizeof(v4l2_ext_controls));
 *     memset(&ext_control, 0, sizeof(v4l2_ext_control));
 *     memset(&m, 0, sizeof(struct v4l2_ext_capture_freeze_mode));
 *
 *     m.plane_index = index; m.val = 1; //on
 *
 *     ext_controls.ctrl_class = V4L2_CTRL_CLASS_USER;
 *     ext_controls.count = 1;
 *     ext_controls.controls = &ext_control;
 *     ext_controls.controls->id = V4L2_CID_EXT_CAPTURE_FREEZE_MODE;
 *     ext_controls.controls->ptr = (void *)&m;
 *
 *     ioctl(fd, VIDIOC_S_EXT_CTRLS, &ext_controls);
 *
 * @endrst
 */
#define V4L2_CID_EXT_CAPTURE_FREEZE_MODE (V4L2_CID_USER_EXT_CAPTURE_BASE + 4)

/**
 * @brief Notify done processing to driver.
 *
 * @rst
 * Functional Requirements
 *   It is a command used in VTV to process the dumped frame at the user level on
 *   the platfom side and inform the completion of the processing.
 *
 * Responses to abnormal situations, including
 *   If abnormal data is set, the driver should return an error.
 *   The generic error codes are described at the :ref:`gen_errors` chapter.
 *
 * Performance Requirements
 *   The response time must respond within 10ms, unless there is a special reason.
 *
 * Constraints
 *   This is only available when V4L2_EXT_CAPTURE_CAP_VIDEOTOVIDEO is supported.
 *   This is only available when the capture location of V4L2_CID_EXT_CAPTURE_PLANE_PROP is set V4L2_EXT_CAPTURE_SCALER_INPUT.
 *
 * Functions & Parameters
 *   .. code-block:: cpp
 *
 *     //
 *     // ioctl command
 *     //
 *     VIDIOC_S_CTRL
 *
 *     //
 *     // control id
 *     //
 *     // You have to make the new control id as the below.
 *     V4L2_CID_EXT_CAPTURE_DONE_USER_PROCESSING 
 *
 *     //
 *     // parameter
 *     //
 *     plane_index //This index means vt output buffer index.
 *
 * Return Value
 *   On success 0 is returned. On error -1 and the errno variable is
 *   set appropriately. The generic error codes are described at the
 *   :ref:`gen_errors` chapter.
 *
 * Control Type
 *   Application can get data using VIDIOC_S_CTRL and this control id.
 *
 * Example
 *   .. code-block:: cpp
 *
 *       struct  v4l2_control control;
 *       control.id = V4L2_CID_EXT_CAPTURE_DONE_USER_PROCESSING;
 *       control.value = plane_index;
 *       ioctl(fd, VIDIOC_S_CTRL, &control);
 *
 * @endrst
 */
#define V4L2_CID_EXT_CAPTURE_DONE_USER_PROCESSING  (V4L2_CID_USER_EXT_CAPTURE_BASE + 5)

/**
 * @brief Get physical memory
 *
 * @rst
 * Functional Requirements
 *   It is a command to read the physical address of the buffer through the buffer
 *   index. If the user client sets the buffer index to the driver, the driver
 *   sets the physical address to each of  ``y`` and ``c``. User client uses
 *   V4L2_EXT_CAPTURE_INPUT_BUF to get the physical memory of input buffer when
 *   VTG and VTV. In case of V4L2_EXT_CAPTURE_OUTPUT_BUF, user client uses that to
 *   get the physical memory of input buffer when VTV. So if the vendor does not
 *   support the feature of VTV, the vendor does not need to implement about
 *   V4L2_EXT_CAPTURE_OUTPUT_BUF.
 *
 * Responses to abnormal situations, including
 *   If abnormal data is set, the driver should return an error.
 *   The generic error codes are described at the :ref:`gen_errors` chapter.
 *
 * Performance Requirements
 *   The response time must respond within 10ms, unless there is a special reason.
 *
 * Constraints
 *   None
 *
 * Functions & Parameters
 *   .. code-block:: cpp
 *
 *     //
 *     // ioctl command
 *     //
 *     VIDIOC_G_EXT_CTLRS
 *
 *     //
 *     // control id
 *     //
 *     // You have to make the new control id as the below.
 *     V4L2_CID_EXT_CAPTURE_PHYSICAL_MEMORY_INFO
 *
 *     //
 *     // parameter
 *     //
 *     enum v4l2_ext_capture_buf_location
 *     {
 *         V4L2_EXT_CAPTURE_INPUT_BUF = 0,
 *         V4L2_EXT_CAPTURE_OUTPUT_BUF
 *     };
 *
 *     struct v4l2_ext_capture_physical_memory_info
 *     {
 *         int buf_index;
 *         union {
 *             unsigned int *y;
 *             unsigned int compat_y_data;
 *             unsigned long long y_sizer;
 *         };
 *
 *         union {
 *             unsigned int *c;
 *             unsigned int compat_c_data;
 *             unsigned long long c_sizer;
 *         };
 *         enum v4l2_ext_capture_buf_location buf_location;
 *     };
 *
 * Return Value
 *   On success 0 is returned. On error -1 and the errno variable is set
 *   appropriately. The generic error codes are described at the
 *   :ref:`gen_errors` chapter.
 *
 * Control Type
 *   Application can get data using VIDIOC_G_EXT_CTRLS and this control id.
 *
 * Example
 *   .. code-block:: cpp
 *
 *       struct v4l2_ext_controls ext_controls;
 *       struct v4l2_ext_control ext_control;
 *       struct v4l2_ext_capture_physical_memory_info info;
 *
 *       memset(&ext_controls, 0, sizeof(v4l2_ext_controls));
 *       memset(&ext_control, 0, sizeof(v4l2_ext_control));
 *       memset(&info, 0, sizeof(struct v4l2_ext_capture_physical_memory_info));
 *
 *       info.buf_index = index;
 *       info.y = malloc(sizeof(unsigned int));
 *       info.c = malloc(sizeof(unsigned int));
 *
 *       ext_controls.ctrl_class = V4L2_CTRL_CLASS_USER;
 *       ext_controls.count = 1;
 *       ext_controls.controls = &ext_control;
 *       ext_controls.controls->id = V4L2_CID_EXT_CAPTURE_PHYSICAL_MEMORY_INFO;
 *       ext_controls.controls->ptr = (void *)&info;
 *
 *       ioctl(fd, VIDIOC_G_EXT_CTRLS, &ext_controls);
 *
 * @endrst
 */
#define V4L2_CID_EXT_CAPTURE_PHYSICAL_MEMORY_INFO    (V4L2_CID_USER_EXT_CAPTURE_BASE + 6)

/**
 * @brief Get output frame rate.
 *
 * @rst
 * Functional Requirements
 *   It is a command for obtaining the output frame rate data of the buffer from
 *   the user client.
 *
 * Responses to abnormal situations, including
 *   If abnormal data is set, the driver should return an error.
 *   The generic error codes are described at the :ref:`gen_errors` chapter.
 *
 * Performance Requirements
 *   The response time must respond within 10ms, unless there is a special reason.
 *
 * Constraints
 *   None
 *
 * Functions & Parameters
 *   .. code-block:: cpp
 *
 *     //
 *     // ioctl command
 *     //
 *     VIDIOC_G_CTRL
 *
 *     //
 *     // control id
 *     //
 *     // You have to make the new control id as the below.
 *     V4L2_CID_EXT_CAPTURE_OUTPUT_FRAMERATE
 *
 *     //
 *     // parameter
 *     //
 *     frame rate (30 or 50 or 60 or 120 or etc..)
 *
 * Return Value
 *   On success 0 is returned. On error -1 and the errno variable is set
 *   appropriately. The generic error codes are described at the
 *   :ref:`gen_errors` chapter.
 *
 * Control Type
 *   Application can get data using VIDIOC_G_CTRL and this control id.
 *
 * Example
 *   .. code-block:: cpp
 *
 *       struct v4l2_control control;
 *       control.id = V4L2_CID_EXT_CAPTURE_OUTPUT_FRAMERATE;
 *       ioctl(fd, VIDIOC_G_CTRL, &control);
 *
 * @endrst
 */
#define V4L2_CID_EXT_CAPTURE_OUTPUT_FRAMERATE          (V4L2_CID_USER_EXT_CAPTURE_BASE + 7)

/**
 * @brief Divide output frame rate.
 *
 * @rst
 * Functional Requirements
 *   This function adjusts the output frame rate of the buffer. The default value
 *   is 1; if the function is not supported, an error is returned. Below is an
 *   example of using this command.
 *     - When the output buffer frame rate of the current buffer is 60Hz and the user
 *       client sets the DIVIDE_FRAMERATE value to 2, the final buffer output frame
 *       rate is 30Hz (60Hz / 2).
 *     - When the output buffer frame rate of current buffer is 120Hz and the user
 *       client sets DIVIDE_FRAMERATE value to 4, the final buffer output frame
 *       rate is 30Hz (120Hz / 4).
 *
 * Responses to abnormal situations, including
 *   If abnormal data is set, the driver should return an error.
 *   The generic error codes are described at the :ref:`gen_errors` chapter.
 *
 * Performance Requirements
 *   The response time must respond within 10ms, unless there is a special reason.
 *
 * Constraints
 *   None
 *
 * Functions & Parameters
 *   .. code-block:: cpp
 *
 *     //
 *     // ioctl command
 *     //
 *     VIDIOC_G_CTRL
 *     VIDIOC_S_CTRL
 *
 *     //
 *     // control id
 *     //
 *     // You have to make the new control id as the below.
 *     V4L2_CID_EXT_CAPTURE_DIVIDE_FRAMERATE
 *
 *     //
 *     // parameter
 *     //
 *     divide value
 *
 * Return Value
 *   On success 0 is returned. On error -1 and the errno variable is set
 *   appropriately. The generic error codes are described at the
 *   :ref:`gen_errors` chapter.
 *
 * Control Type
 *   Application can get data using VIDIOC_S_CTRL/VIDIOC_G_CTRL and this control
 *   id.
 *
 * Example
 *   .. code-block:: cpp
 *
 *       struct v4l2_control control;
 *       control.id = V4L2_CID_EXT_CAPTURE_DIVIDE_FRAMERATE;
 *       control.value = 2;
 *       ioctl(fd, VIDIOC_S_CTRL, &control);
 *
 * @endrst
 */
#define V4L2_CID_EXT_CAPTURE_DIVIDE_FRAMERATE         (V4L2_CID_USER_EXT_CAPTURE_BASE + 8)


#define V4L2_CID_EXT_CAPTURE_VIDEO_FPS                (V4L2_CID_USER_EXT_CAPTURE_BASE + 9)

/* v4l2_ext_video_fps structure
@ input_fps: set Input Capture Framerate
@ output_fps: set Output Capture Framerate
*/
struct v4l2_ext_video_fps{
    unsigned int input_fps;
    unsigned int output_fps;
};


/* Capture class subscription IDs */
#define V4L2_CID_EXT_CAPTURE_SUBSCRIBE_FRAME_READY    (V4L2_CID_USER_EXT_CAPTURE_BASE + 30)

/* Capture class subscription types */
//#define V4L2_EVENT_PRIVATE_EXT_CAPTURE_BASE (V4L2_EVENT_PRIVATE_START + 0x2100)
//#define V4L2_EVENT_PRIVATE_EXT_CAPTURE_EVENT  (V4L2_EVENT_PRIVATE_EXT_CAPTURE_BASE + 1)

#endif
