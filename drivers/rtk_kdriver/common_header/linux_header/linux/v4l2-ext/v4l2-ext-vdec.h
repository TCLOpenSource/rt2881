#ifndef _V4L2_EXT_VDEC_H
#define _V4L2_EXT_VDEC_H

#include <linux/v4l2-controls.h>

#ifndef _UAPI__LINUX_VIDEODEV2_H
#include <linux/videodev2.h>
#endif

#define V4L2_EXT_DEV_NO_VDEC 20
#define V4L2_EXT_DEV_NO_VENC 21
#define V4L2_EXT_DEV_NO_VEA  24
#define V4L2_EXT_DEV_PATH_VDEC "/dev/video20"
#define V4L2_EXT_DEV_PATH_VENC "/dev/video21"
#define V4L2_EXT_DEV_PATH_VEA  "/dev/video24"

/* VDEC class control IDs */
#define V4L2_CID_USER_EXT_VDEC_BASE (V4L2_CID_USER_BASE + 0x8000)
#define V4L2_CID_EXT_VDEC_BASE (V4L2_CID_USER_EXT_VDEC_BASE + 0x0)


/* VDEC class subscription types */
#define V4L2_EVENT_PRIVATE_EXT_VDEC_BASE (V4L2_EVENT_PRIVATE_START + 0x8000)

/**
 * @rst
 * Functional Requirements
 *   V4L2 Subscription Type for VDEC
 * @endrst
 */
#define V4L2_EVENT_PRIVATE_EXT_VDEC_EVENT (V4L2_EVENT_PRIVATE_EXT_VDEC_BASE + 1)




/* VENC class control values for V4L2_CID_RTK_EXT_VENC_PACKET_INFOR */

/**
 * @brief  Struct for V4L2_CID_RTK_EXT_VENC_PACKET_INFOR
 *
 * Sets video packet information.
 * Only valid if the low latency mode is set to 1 and Chip IC is Dora only.
 *
 */
struct v4l2_ext_video_packet_infor {
    /* Vdec port. */
    __s8  port;
    /* Packet entry size, it’s 32B unit. */
    __u8  entry_size;
    /* Video payload offset, it’s 4B unit. */
    __u8  payload_offset;
    /* Packet header length. */
    __u8  header_len;
    /* Length1 offset. */
    __u8  len1_offset;
    /* Length2 offset. */
    __u8  len2_offset;
    /* Sequence number offset. */
    __u8  seq_no_offset;
    /* Timestamp offset. */
    __u8  ts_offset;
    /* Length1 shifting. */
    __u8  len1_shifting;
    /* Length2 shifting. */
    __u8  len2_shifting;
    /* Padding size. */
    __u16 padding_size;
    /* Sequence number. */
    __u16 seq_no;
    /* Reserved for future extensions. */
    __u8  reserved[2];
};


/* VENC class control values for V4L2_CID_RTK_EXT_VENC_INFOR */

/**
 * @brief  Struct for V4L2_CID_RTK_EXT_VENC_INFOR
 *
 * Gets VENC bs information.
 *
 */
struct v4l2_ext_video_encode_infor {
    /* Encode Picture width. */
    __u16   enc_width;
    /* Encode Picture height. */
    __u16   enc_height;
    /* Encode Picture color space. See color_space.
       RTK_VIDEO_FOMAT_420  0   YUV420
       RTK_VIDEO_FOMAT_422  1   YUV422
       RTK_VIDEO_FOMAT_444  2   YUV444
    */
    __u8    enc_color_space;
    /* Encode Picture bit_depth. See bit_depth.
       RTK_VIDEO_BITS_DEPTH_UNKNOWN 0   Bits depth unknown.
       RTK_VIDEO_BITS_DEPTH_8       1   Bits depth 8
       RTK_VIDEO_BITS_DEPTH_10      2   Bits depth 10
    */
    __u8    enc_bit_depth;
    /* Video frame rate.  (value 23970 means 23.97 fps) */
    __s32   framerate;
    /* Reserved for future extensions. */
    __s32   reserved[2];
};


/* VENC class control values for V4L2_CID_RTK_EXT_VENC_BS_INFOR */

/**
 * @brief  Struct for V4L2_CID_RTK_EXT_VENC_BS_INFOR
 *
 * Gets VENC bs information.
 *
 */
struct v4l2_ext_venc_bs_rb_info {
    /* Base address of video bitstream ring buffer. */
    __u32 bsbase_addr;
    /* Limit address of video bitstream ring buffer. */
    __u32 bslimit_addr;
    /* Write pointer of video bitstream ring buffer. */
    __u32 bswrptr_addr;
    /* Read pointer of video bitstream ring buffer. */
    __u32 bsrdptr_addr;
    /* Reserved for future extensions. */
    __u32 reserved;
};


/* VENC class control values for V4L2_CID_RTK_EXT_VENC_FRM_INFOR */

/**
 * @brief  Struct for V4L2_CID_RTK_EXT_VENC_FRM_INFOR
 *
 * Get encoded frame bs information.
 *
 */
struct v4l2_ext_venc_frame_info {
    /* Picture coding type. */
    __u32 pictureType;
    __u32 PTSHigh;
    __u32 PTSLow;
    /* Encoded frame es size. */
    __s32 FrameSize;
    /* Reserved for future extensions. */
    __u32 reserved;
};


/* VENC class control values for V4L2_CID_RTK_EXT_VENC_UPDATE_RDPTR */

/**
 * @brief  Struct for V4L2_CID_RTK_EXT_VENC_UPDATE_RDPTR
 *
 * For AP update encoder bs ringbuffer rdPtr.
 *
 */
struct v4l2_ext_venc_update_rdptr {
    /* Encoding type. H264:0, AV1:1. */
    __u8 encodeType;
    /* Read pointer */
    __u32 bsptr;
    /* Reserved for future extensions. */
    __u32 reserved;
};


/* VENC class control values for V4L2_CID_RTK_EXT_VENC_SET_RAW_BUFFER */

/**
 * @brief  Struct for V4L2_CID_RTK_EXT_VENC_SET_RAW_BUFFER
 *
 * For AP set raw data buffer address to encoder.
 *
 */
struct v4l2_ext_venc_set_raw_buffer {
    /* Encode bs timestamp high */
    __u32 PTSHigh;
    /* Encode bs timestamp low */
    __u32 PTSLow;
    /* buffer start address of raw data plane[] */
    __u32 addr[4];
    /* buffer size of raw data plane[] */
    __u32 size[4];
    /* buffer stride of raw data plane[] */
    __u32 stride[4];
    /* Reserved for future extensions. */
    __u32 reserved;
};


/* VENC class control values for V4L2_CID_RTK_EXT_VENC_COLOR_INFO */

/**
 * @brief  Struct for V4L2_CID_RTK_EXT_VENC_COLOR_INFO
 *
 * For AP set raw data copy mode for encoder.
 *
 */
struct v4l2_ext_venc_color_info {
    /* Reserved0 for future extensions. */
    __u32 reserved0;
    /* Reserved1 for future extensions. */
    __u32 reserved1;
};


/* VENC class control values for V4L2_CID_RTK_EXT_VENC_COPY_MODE */

/**
 * @brief  Struct for V4L2_CID_RTK_EXT_VENC_COPY_MODE
 *
 * For AP set raw data copy mode for encoder.
 *
 */
struct v4l2_ext_venc_copy_mode {
    /* Copy mode.
       0   Raw data copy to ringbuffer
       1   ??
       2   Raw data at dma buffer
       3   Raw data at dma buffer
    */
    __u8 copyMode;
    /* Reserved for future extensions. */
    __u32 reserved;
};


/* VENC class control ID */
#define V4L2_CID_RTK_EXT_VENC_BASE              (V4L2_CID_USER_BASE + 0x8800)
#define V4L2_CID_RTK_EXT_VENC_LOW_LATENCY       (V4L2_CID_RTK_EXT_VENC_BASE + 1)
#define V4L2_CID_RTK_EXT_VENC_PACKET_INFOR      (V4L2_CID_RTK_EXT_VENC_BASE + 2)
#define V4L2_CID_RTK_EXT_VENC_INFOR             (V4L2_CID_RTK_EXT_VENC_BASE + 3)
#define V4L2_CID_RTK_EXT_VENC_BS_INFOR          (V4L2_CID_RTK_EXT_VENC_BASE + 4)
#define V4L2_CID_RTK_EXT_VENC_FRM_INFOR         (V4L2_CID_RTK_EXT_VENC_BASE + 5)

/**
 * @rst
 * Functional Requirements
 *   It is a control ID for AP process BS done. Then can use ext ioctrl to update
 *   rdPtr to video encoder. Driver will get rdPtr value, then set to ringbuffer.
 *
 * Functions & Parameters
 *   .. code-block:: cpp
 *
 *     //
 *     // ioctl command
 *     //
 *     VIDIOC_S_EXT_CTRLS       // Set
 *
 *     //
 *     // control id
 *     //
 *     // You have to make the new control id as the below.
 *     V4L2_CID_RTK_EXT_VENC_UPDATE_RDPTR
 *
 *     //
 *     // control value
 *     //
 *     -
 *
 * Responses to abnormal situations, including
 *   None
 *
 * Performance Requirements
 *   Function elapsed time should be less than 100ms.
 *
 * Constraints
 *   None
 *
 * Return Value
 *   On success 0 is returned.
 *
 *   On error -1 and the errno variable is set appropriately.
 *   The generic error codes are described at the :ref:`gen_errors` chapter.
 *
 * Control Type
 *   Application can get data using :c:func:`v4l-dvb-apis:VIDIOC_S_EXT_CTRLS`
 *   and this control id.
 *
 * Example
 *   .. code-block:: cpp
 *
 *      struct v4l2_ext_controls ext_controls;
 *      struct v4l2_ext_control ext_control;
 *      struct v4l2_ext_venc_update_rdptr upRdPtr;

 *      memset(&ext_controls, 0, sizeof(struct v4l2_ext_controls));
 *      memset(&ext_control, 0, sizeof(struct v4l2_ext_control));
 *      memset(&upRdPtr, 0, sizeof(struct v4l2_ext_venc_update_rdptr));

 *      upRdPtr.encodeType = 0;
 *      upRdPtr.bsptr = bsRead;

 *      ext_controls.ctrl_class     = V4L2_CTRL_CLASS_USER;
 *      ext_controls.count          = 1;
 *      ext_controls.controls       = &ext_control;
 *      ext_controls.controls->id   = V4L2_CID_RTK_EXT_VENC_UPDATE_RDPTR;
 *      ext_controls.controls->size = sizeof(struct v4l2_ext_venc_update_rdptr);
 *      ext_controls.controls->ptr  = (void *)&upRdPtr;

 *      ret = ioctl(fd_enc, VIDIOC_S_EXT_CTRLS, &ext_controls);
 *      if (ret) goto err;
 *
 * @endrst
 */
#define V4L2_CID_RTK_EXT_VENC_UPDATE_RDPTR                                     \
    (V4L2_CID_RTK_EXT_VENC_BASE + 6) // For AP update encoder ringbuffer bs
                                     // rdPtr.

/**
 * @rst
 * Functional Requirements
 *   It is a control ID for AP process BS done. Then can use ext ioctrl to update
 *   rdPtr to video encoder. Driver will get rdPtr value, then set to ringbuffer.
 *
 * Functions & Parameters
 *   .. code-block:: cpp
 *
 *     //
 *     // ioctl command
 *     //
 *     VIDIOC_S_EXT_CTRLS       // Set
 *
 *     //
 *     // control id
 *     //
 *     // You have to make the new control id as the below.
 *     V4L2_CID_RTK_EXT_VENC_SET_RAW_BUFFER
 *
 *     //
 *     // control value
 *     //
 *     -
 *
 * Responses to abnormal situations, including
 *   None
 *
 * Performance Requirements
 *   Function elapsed time should be less than 100ms.
 *
 * Constraints
 *   None
 *
 * Return Value
 *   On success 0 is returned.
 *
 *   On error -1 and the errno variable is set appropriately.
 *   The generic error codes are described at the :ref:`gen_errors` chapter.
 *
 * Control Type
 *   Application can get data using :c:func:`v4l-dvb-apis:VIDIOC_S_EXT_CTRLS`
 *   and this control id.
 *
 * Example
 *   .. code-block:: cpp
 *
 *      struct v4l2_ext_controls ext_controls;
 *      struct v4l2_ext_control ext_control;
 *      struct v4l2_ext_venc_set_raw_buffer setRawBuffer;

 *      memset(&ext_controls, 0, sizeof(struct v4l2_ext_controls));
 *      memset(&ext_control, 0, sizeof(struct v4l2_ext_control));
 *      memset(&setRawBuffer, 0, sizeof(struct v4l2_ext_venc_set_raw_buffer));

 *      setRawBuffer.PTSHigh = 0;
 *      setRawBuffer.PTSLow = 0;

 *      for ( i = 0; i < NUM_PLANE; i++ )
 *      {
 *          setRawBuffer.addr[i] = addr;
 *          setRawBuffer.size[i] = size;
 *          setRawBuffer.stride[i] = stride;
 *      }

 *      ext_controls.ctrl_class     = V4L2_CTRL_CLASS_USER;
 *      ext_controls.count          = 1;
 *      ext_controls.controls       = &ext_control;
 *      ext_controls.controls->id   = V4L2_CID_RTK_EXT_VENC_SET_RAW_BUFFER;
 *      ext_controls.controls->size = sizeof(struct v4l2_ext_venc_set_raw_buffer);
 *      ext_controls.controls->ptr  = (void *)&setRawBuffer;

 *      ret = ioctl(fd_enc, VIDIOC_S_EXT_CTRLS, &ext_controls);
 *      if (ret) goto err;
 *
 * @endrst
 */
#define V4L2_CID_RTK_EXT_VENC_SET_RAW_BUFFER                                    \
    (V4L2_CID_RTK_EXT_VENC_BASE + 7) // For AP set raw data buffer address to
                                     // encoder.

/**
 * @rst
 * Functional Requirements
 *   It is a control ID for AP process BS done. Then can use ext ioctrl to update
 *   rdPtr to video encoder. Driver will get rdPtr value, then set to ringbuffer.
 *
 * Functions & Parameters
 *   .. code-block:: cpp
 *
 *     //
 *     // ioctl command
 *     //
 *     VIDIOC_S_EXT_CTRLS       // Set
 *
 *     //
 *     // control id
 *     //
 *     // You have to make the new control id as the below.
 *     V4L2_CID_RTK_EXT_VENC_COLOR_INFO
 *
 *     //
 *     // control value
 *     //
 *     -
 *
 * Responses to abnormal situations, including
 *   None
 *
 * Performance Requirements
 *   Function elapsed time should be less than 100ms.
 *
 * Constraints
 *   None
 *
 * Return Value
 *   On success 0 is returned.
 *
 *   On error -1 and the errno variable is set appropriately.
 *   The generic error codes are described at the :ref:`gen_errors` chapter.
 *
 * Control Type
 *   Application can get data using :c:func:`v4l-dvb-apis:VIDIOC_S_EXT_CTRLS`
 *   and this control id.
 *
 * Example
 *   .. code-block:: cpp
 *
 *      struct v4l2_ext_controls ext_controls;
 *      struct v4l2_ext_control ext_control;
 *      struct v4l2_ext_venc_color_info colorInfo;

 *      memset(&ext_controls, 0, sizeof(struct v4l2_ext_controls));
 *      memset(&ext_control, 0, sizeof(struct v4l2_ext_control));
 *      memset(&rpc_sendTVD, 0, sizeof(VIDEO_RPC_ENC_SEND_TVD_INFO));

 *      colorInfo.reserved0 =
                ((video_signal_type_present_flag & 0x000000FF) << 24 |
                (colorAspects.bVideoFullRangeFlag & 0x000000FF) << 16);
 *      colorInfo.reserved1 =
                ((colour_description_present_flag & 0x000000FF) << 24 |
                (colorAspects.nColorPrimaries & 0x000000FF) << 16 |
                (colorAspects.nTransferCharacteristics & 0x000000FF) << 8 |
                (colorAspects.nMatrixCoefficiets & 0x000000FF));

 *      ext_controls.ctrl_class     = V4L2_CTRL_CLASS_USER;
 *      ext_controls.count          = 1;
 *      ext_controls.controls       = &ext_control;
 *      ext_controls.controls->id   = V4L2_CID_RTK_EXT_VENC_COLOR_INFO;
 *      ext_controls.controls->size = sizeof(struct v4l2_ext_venc_color_info);
 *      ext_controls.controls->ptr  = (void *)&colorInfo;

 *      ret = ioctl(fd_enc, VIDIOC_S_EXT_CTRLS, &ext_controls);
 *      if (ret) goto err;
 *
 * @endrst
 */
#define V4L2_CID_RTK_EXT_VENC_COLOR_INFO                                         \
    (V4L2_CID_RTK_EXT_VENC_BASE + 8) // For AP set colorAspect info

/**
 * @rst
 * Functional Requirements
 *   It is a control ID for AP process BS done. Then can use ext ioctrl to update
 *   rdPtr to video encoder. Driver will get rdPtr value, then set to ringbuffer.
 *
 * Functions & Parameters
 *   .. code-block:: cpp
 *
 *     //
 *     // ioctl command
 *     //
 *     VIDIOC_S_EXT_CTRLS       // Set
 *
 *     //
 *     // control id
 *     //
 *     // You have to make the new control id as the below.
 *     V4L2_CID_RTK_EXT_VENC_COPY_MODE
 *
 *     //
 *     // control value
 *     //
 *     -
 *
 * Responses to abnormal situations, including
 *   None
 *
 * Performance Requirements
 *   Function elapsed time should be less than 100ms.
 *
 * Constraints
 *   None
 *
 * Return Value
 *   On success 0 is returned.
 *
 *   On error -1 and the errno variable is set appropriately.
 *   The generic error codes are described at the :ref:`gen_errors` chapter.
 *
 * Control Type
 *   Application can get data using :c:func:`v4l-dvb-apis:VIDIOC_S_EXT_CTRLS`
 *   and this control id.
 *
 * Example
 *   .. code-block:: cpp
 *
 *      struct v4l2_ext_controls ext_controls;
 *      struct v4l2_ext_control ext_control;
 *      struct v4l2_ext_venc_copy_mode setCopyMode;

 *      memset(&ext_controls, 0, sizeof(struct v4l2_ext_controls));
 *      memset(&ext_control, 0, sizeof(struct v4l2_ext_control));
 *      memset(&setCopyMode, 0, sizeof(struct v4l2_ext_venc_copy_mode));

 *      setCopyMode.copyMode = 0;

 *      ext_controls.ctrl_class     = V4L2_CTRL_CLASS_USER;
 *      ext_controls.count          = 1;
 *      ext_controls.controls       = &ext_control;
 *      ext_controls.controls->id   = V4L2_CID_RTK_EXT_VENC_COPY_MODE;
 *      ext_controls.controls->size = sizeof(struct v4l2_ext_venc_copy_mode);
 *      ext_controls.controls->ptr  = (void *)&setCopyMode;

 *      ret = ioctl(fd_enc, VIDIOC_S_EXT_CTRLS, &ext_controls);
 *      if (ret) goto err;
 *
 * @endrst
 */
#define V4L2_CID_RTK_EXT_VENC_COPY_MODE                                         \
    (V4L2_CID_RTK_EXT_VENC_BASE + 9) // For AP set encoder copy mode


/* VENC class control values for V4L2_EVENT_PRIVATE_EXT_VENC_NEWES */

/**
 * @brief  Struct for V4L2_EVENT_PRIVATE_EXT_VENC_NEWES
 *
 * Inform AP there have new es in ringbuffer.
 * With PTS and bs info.
 *
 */
struct v4l2_ext_event_venc_new_es {
    /* Picture coding type. */
    __u32 pictureType;
    /* Encode bs timestamp high */
    __u32 PTSHigh;
    /* Encode bs timestamp low */
    __u32 PTSLow;
    /* Encode bs start address */
    __u32 bsStart;
    /* Encode bs size */
    __u32 bsSize;
};

/* VENC class subscription types */
#define V4L2_EVENT_PRIVATE_EXT_VENC_BASE  (V4L2_EVENT_PRIVATE_START + 0xA000)

/**
 * @rst
 * Functional Requirements
 *   V4L2 Subscription Type for VENC Send Event to AP.
 *   AP Can Get Encode Bs Address Range.
 * @endrst
 */
#define V4L2_EVENT_PRIVATE_EXT_VENC_NEWES (V4L2_EVENT_PRIVATE_EXT_VENC_BASE + 1)


#endif // #ifndef _V4L2_EXT_VDEC_H
