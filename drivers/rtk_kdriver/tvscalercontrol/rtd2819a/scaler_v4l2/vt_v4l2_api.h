/*
* VT v4l2 related api header file
* creat 2023-7-25 in CS
* Copyright(c) 2023 by Realtek.
*/
#ifndef _VT_V4L2_API_H
#define _VT_V4L2_API_H

//#include <include/media/v4l2-device.h>
//#include <v4l2-dev.h>
#define VT_IDDMA_DEV_NUM (4)
#define VT_MAX_DEV_NUM (VT_IDDMA_DEV_NUM + 2) //4 ixddma and 2 dc2h

#define MAX_VT_BUFFERS_NUM    (32)

struct vt_v4l2_device {
    unsigned char source_type;
    unsigned int nr;
    unsigned int vt_dev_num; //ixddma:0,1,2,3 dc2h:4,5
    unsigned int hdmi_port; //hdmi or dp port
    struct dma_buf* pdmabuf[MAX_VT_BUFFERS_NUM]; //memory type is DMABUF will use it
    struct v4l2_device v4l2_dev;
    struct video_device *vfd;
    struct mutex lock;
};

typedef enum _SCALER_DC2H_DEV_{
	VT_DC2H1_DEV = 4,
	VT_DC2H2_DEV,
    VT_MAX_DC2H_DEV = VT_MAX_DEV_NUM,
}SCALER_DC2H_DEV;

//v4l2 ioctrl callback api
int vt_v4l2_ioctl_s_ext_ctrls(struct file *file, void *fh, struct v4l2_ext_controls *ctrls);
int vt_v4l2_ioctl_g_ext_ctrls(struct file *file, void *fh, struct v4l2_ext_controls *ctrls);
int vt_v4l2_ioctl_s_ctrl(struct file *file, void *fh, struct v4l2_control *ctrl);
int vt_v4l2_ioctl_g_ctrl(struct file *file, void *fh, struct v4l2_control *ctrl);
int vt_v4l2_ioctl_subscribe_event(struct v4l2_fh *fh, const struct v4l2_event_subscription *sub);
int vt_v4l2_ioctl_unsubscribe_event(struct v4l2_fh *fh, const struct v4l2_event_subscription *sub);

//vt internal api

int vt_v4l2_ioctl_querycap(struct file *file, void *fh, struct v4l2_capability *cap);
int vt_v4l2_ioctl_reqbufs(struct file *file, void *fh, struct v4l2_requestbuffers *b);
int vt_v4l2_ioctl_querybuf(struct file *file, void *fh, struct v4l2_buffer *b);
int vt_v4l2_ioctl_qbuf(struct file *file, void *fh, struct v4l2_buffer *b);
int vt_v4l2_ioctl_dqbuf(struct file *file, void *fh, struct v4l2_buffer *b);
int vt_v4l2_ioctl_streamon(struct file *file, void *fh, enum v4l2_buf_type i);
int vt_v4l2_ioctl_streamoff(struct file *file, void *fh, enum v4l2_buf_type i);
int vt_v4l2_ioctl_s_fmt_vid_cap(struct file *file, void *fh, struct v4l2_format *f);
int vt_v4l2_ioctl_g_fmt_vid_cap(struct file *file, void *fh, struct v4l2_format *f);
int vt_v4l2_ioctl_try_fmt_vid_cap(struct file *file, void *fh, struct v4l2_format *f);
int vt_v4l2_ioctl_frameintervals(struct file *file, void *fh, struct v4l2_frmivalenum *fival);


#endif

