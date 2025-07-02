/*
* v4l2 vt(video texture) driver
* creat 2023-7-25 in CS
* Copyright(c) 2023 by Realtek.
*/
//Kernel Header file
#include <linux/atomic.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/videodev2.h>
#include <linux/vmalloc.h>
#include <linux/wait.h>
#include <linux/version.h>
#include <asm/unaligned.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-common.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <rbus/ppoverlay_reg.h>
#include <linux/mm.h>  //remap_pfn_range

#include <ioctrl/scaler/vt_cmd_id.h>
#include <ioctrl/scaler/v4l2_ext_vt.h>
#include "vt_v4l2_api.h"

#include <scaler/scalerCommon.h>
#include <tvscalercontrol/scaler/scalerstruct.h>
#include <include/rtk_kdriver/rtd_log/rtd_module_log.h>
#include <tvscalercontrol/i3ddma/i3ddma.h>

/*ixddma video devide name*/
#define V4L2_VT_CAPTURE1_DEVICE_NAME   "video10"
#define V4L2_VT_CAPTURE2_DEVICE_NAME   "video20"
#define V4L2_VT_CAPTURE3_DEVICE_NAME   "video30"
#define V4L2_VT_CAPTURE4_DEVICE_NAME   "video40"

/* vt module dc2hx video device name */
#define V4L2_VT_CAPTURE5_DEVICE_NAME   "video50"
#define V4L2_VT_CAPTURE6_DEVICE_NAME   "video60"


extern unsigned int get_idle_dc2h(unsigned int nr);
extern void set_idle_dc2h(unsigned int dev_num, unsigned int nr);

static int vt_v4l2_main_open(struct file *file)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned int nr = 0;

    if(vt_dev != NULL)
    {
        nr = vt_dev->nr;
        switch(nr)
        {
            case V4L2_EXT_DEV_NO_CAPTURE1:
            case V4L2_EXT_DEV_NO_CAPTURE2:
            case V4L2_EXT_DEV_NO_CAPTURE3:
            case V4L2_EXT_DEV_NO_CAPTURE4:
            {
                if(vt_dev->vt_dev_num == (unsigned int)VT_MAX_DEV_NUM)
                {
                    vt_dev->vt_dev_num = IDDMA_Find_IdleDev(nr);
                    if(vt_dev->vt_dev_num == (unsigned int)VT_MAX_DEV_NUM)
                        return -EFAULT;
                }
            }
            break;
            case V4L2_EXT_DEV_NO_CAPTURE5:
            case V4L2_EXT_DEV_NO_CAPTURE6:
            { /* for dc2hx */
                if(vt_dev->vt_dev_num == (unsigned int)VT_MAX_DEV_NUM){
                    vt_dev->vt_dev_num = (get_idle_dc2h(nr) + VT_IDDMA_DEV_NUM);
                    rtd_pr_vt_notice("fun:%s, open vt_dev_num %d\n",__FUNCTION__, vt_dev->vt_dev_num);
                    if(vt_dev->vt_dev_num == (unsigned int)VT_MAX_DEV_NUM){
                        rtd_pr_vt_notice("fun:%s, open fail, no idle dc2h\n",__FUNCTION__);
                        return -EFAULT;
                    }
                }
            }
            break;
        }

        return 0;
    }else{
        rtd_pr_vt_err("fun:%s, [error] vt_dev is null when open!\n", __FUNCTION__);
        return -EFAULT;
    }
}

static int vt_v4l2_main_release(struct file *file)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned int dev_num = 0;

    if(vt_dev != NULL)
    {
        dev_num = vt_dev->vt_dev_num;
        switch(dev_num)
        {
            case I3DDMA_DEV:
            case I4DDMA_DEV:
            case I5DDMA_DEV:
            case I6DDMA_DEV:
                /*do ixddma*/
            break;
            case VT_DC2H1_DEV:
            case VT_DC2H2_DEV:
            {  /* for dc2hx */
                if(vt_dev->vt_dev_num != (unsigned int)VT_MAX_DEV_NUM){
                    set_idle_dc2h(vt_dev->vt_dev_num - VT_IDDMA_DEV_NUM, vt_dev->nr);
                    vt_dev->vt_dev_num = (unsigned int)VT_MAX_DEV_NUM;
                    rtd_pr_vt_notice("fun:%s, release vt_dev_num %d\n", __FUNCTION__, dev_num);
                }
            }
            break;
        }

        return 0;
    }else{
        rtd_pr_vt_err("fun:%s, [error] vt_dev is null when release!\n", __FUNCTION__);
        return -EFAULT;
    }
}


static ssize_t vt_v4l2_main_read(struct file *file, char __user *data, size_t count, loff_t *ppos)
{
    return 0;
}

static __poll_t vt_v4l2_main_poll(struct file *file, struct poll_table_struct *wait)
{
    return 0;
}

static int vt_v4l2_main_mmap(struct file *file, struct vm_area_struct *vma)
{
    vma->vm_flags |= VM_IO;
    //vma->vm_flags |= VM_RESERVED;
    if(remap_pfn_range(vma,
            vma->vm_start,
            vma->vm_pgoff,
            vma->vm_end - vma->vm_start,
            vma->vm_page_prot))
    {
        return -EAGAIN;
    }
    return 0;
}



static const struct v4l2_ioctl_ops vt_main_ioctl_ops = {

    /*-----3 Extended control information for implementation-----*/
    .vidioc_s_ext_ctrls = vt_v4l2_ioctl_s_ext_ctrls,
    .vidioc_g_ext_ctrls = vt_v4l2_ioctl_g_ext_ctrls,
    .vidioc_s_ctrl = vt_v4l2_ioctl_s_ctrl,
    .vidioc_g_ctrl = vt_v4l2_ioctl_g_ctrl,
    .vidioc_subscribe_event = vt_v4l2_ioctl_subscribe_event,
    .vidioc_unsubscribe_event = vt_v4l2_ioctl_unsubscribe_event,

    /*-----2 standard control information for implementation-----*/
    .vidioc_querycap = vt_v4l2_ioctl_querycap,
    .vidioc_reqbufs = vt_v4l2_ioctl_reqbufs, /* allocate buffers or release buffers*/
    .vidioc_querybuf = vt_v4l2_ioctl_querybuf,
    .vidioc_qbuf = vt_v4l2_ioctl_qbuf,
    .vidioc_dqbuf = vt_v4l2_ioctl_dqbuf,
    .vidioc_streamon = vt_v4l2_ioctl_streamon,
    .vidioc_streamoff = vt_v4l2_ioctl_streamoff,
    .vidioc_g_fmt_vid_cap = vt_v4l2_ioctl_g_fmt_vid_cap,
    .vidioc_s_fmt_vid_cap = vt_v4l2_ioctl_s_fmt_vid_cap,

    .vidioc_g_fmt_vid_cap_mplane = vt_v4l2_ioctl_g_fmt_vid_cap,
    .vidioc_s_fmt_vid_cap_mplane = vt_v4l2_ioctl_s_fmt_vid_cap,

    /* VIDIOC_TRY_FMT handlers */
    .vidioc_try_fmt_vid_cap = vt_v4l2_ioctl_try_fmt_vid_cap,
    .vidioc_try_fmt_vid_cap_mplane = vt_v4l2_ioctl_try_fmt_vid_cap,

    /* VIDIOC_ENUM_FRAMEINTERVALS ioctl */
    .vidioc_enum_frameintervals = vt_v4l2_ioctl_frameintervals,
};


static const struct v4l2_file_operations vt_main_fops = {
    .owner = THIS_MODULE,
    .open = vt_v4l2_main_open,
    .release = vt_v4l2_main_release,
    .read = vt_v4l2_main_read,
    .poll = vt_v4l2_main_poll,
    .unlocked_ioctl = video_ioctl2, /* V4L2 ioctl handler */
    .mmap = vt_v4l2_main_mmap,
};

/*public function*/
static struct vt_v4l2_device* vt_init_device(int* ret, char* dev_path, char* dev_name, unsigned int nr)
{
    unsigned char i;
    struct vt_v4l2_device *vt_v4l2_dev = NULL;
    struct video_device *vt_vdev = NULL;

    vt_v4l2_dev = kzalloc(sizeof(struct vt_v4l2_device), GFP_KERNEL);
    if (!vt_v4l2_dev){
        rtd_pr_vt_err("%s(%d)alloc memory for vt_v4l2_device fail\n", __func__, __LINE__);
        *ret = -ENOMEM;
    }
    
    snprintf(vt_v4l2_dev->v4l2_dev.name, sizeof(vt_v4l2_dev->v4l2_dev.name), "%s", dev_path);

    *ret = v4l2_device_register(NULL, &vt_v4l2_dev->v4l2_dev);
    if(*ret){
        rtd_pr_vt_err("%s(%d)v4l2_device_register fail\n", __func__, __LINE__);
        goto free_dev;
    }

    vt_vdev = video_device_alloc();
    if(!vt_vdev){
        v4l2_err(&vt_v4l2_dev->v4l2_dev, "Failed to allocate video device\n");
        *ret = -ENOMEM;
        goto unreg_dev;
    }

    mutex_init(&vt_v4l2_dev->lock);
    vt_vdev->lock = &vt_v4l2_dev->lock;

    snprintf(vt_vdev->name, sizeof(vt_vdev->name), "%s", dev_name);
    vt_vdev->fops = &vt_main_fops;
    vt_vdev->ioctl_ops = &vt_main_ioctl_ops;
    vt_vdev->minor = -1;
    vt_vdev->release = video_device_release;
    vt_vdev->vfl_dir = VFL_DIR_RX;
    vt_vdev->v4l2_dev = &vt_v4l2_dev->v4l2_dev;
    vt_vdev->device_caps = (V4L2_CAP_VIDEO_CAPTURE_MPLANE | V4L2_CAP_STREAMING);

    *ret = video_register_device(vt_vdev, VFL_TYPE_VIDEO, nr);
    if(*ret){
        v4l2_err(&vt_v4l2_dev->v4l2_dev, "Failed to register video device\n");
        goto rel_vdev;
    }

    vt_v4l2_dev->nr = nr;
    vt_v4l2_dev->vt_dev_num = VT_MAX_DEV_NUM;
    for(i = 0; i < MAX_VT_BUFFERS_NUM; i++)
    {
        vt_v4l2_dev->pdmabuf[i] = NULL;
    }

    video_set_drvdata(vt_vdev, vt_v4l2_dev);
    vt_v4l2_dev->vfd = vt_vdev;
    v4l2_info(&vt_v4l2_dev->v4l2_dev, "V4L2 device registered as %s\n", video_device_node_name(vt_vdev));
    spin_lock_init(&vt_vdev->fh_lock);


    return vt_v4l2_dev;

rel_vdev:
    video_device_release(vt_vdev);
unreg_dev:
    v4l2_device_unregister(&vt_v4l2_dev->v4l2_dev);
free_dev:
    kfree(vt_v4l2_dev);

    return NULL;
}

/*I3DDMA device init*/
static void vt_deinit_device(struct vt_v4l2_device* vt_dev)
{
    if(vt_dev == NULL)
    {
        return;
    }
        
	video_device_release(vt_dev->vfd);
    v4l2_device_unregister(&vt_dev->v4l2_dev);
    kfree(vt_dev);
}


struct vt_v4l2_device *g_vt_v4l2_dev1 = NULL; 
int __init v4l2_vtdev_capture1_init(void)
{
    int ret;

    g_vt_v4l2_dev1 = vt_init_device(&ret, V4L2_EXT_DEV_PATH_CAPTURE1, V4L2_VT_CAPTURE1_DEVICE_NAME, V4L2_EXT_DEV_NO_CAPTURE1);

    return ret;
}

void __exit v4l2_vtdev_capture1_exit(void)
{
    vt_deinit_device(g_vt_v4l2_dev1);
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(v4l2_vtdev_capture1_init);
module_exit(v4l2_vtdev_capture1_exit);
#endif


/*I4DDMA device init*/
struct vt_v4l2_device *g_vt_v4l2_dev2 = NULL; 
int __init v4l2_vtdev_capture2_init(void)
{
    int ret;

    g_vt_v4l2_dev2 = vt_init_device(&ret, V4L2_EXT_DEV_PATH_CAPTURE2, V4L2_VT_CAPTURE2_DEVICE_NAME, V4L2_EXT_DEV_NO_CAPTURE2);

    return ret;
}

void __exit v4l2_vtdev_capture2_exit(void)
{
    vt_deinit_device(g_vt_v4l2_dev2);
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(v4l2_vtdev_capture2_init);
module_exit(v4l2_vtdev_capture2_exit);
#endif

/*I5DDMA device init*/
struct vt_v4l2_device *g_vt_v4l2_dev3 = NULL; 
int __init v4l2_vtdev_capture3_init(void)
{
    int ret;

    g_vt_v4l2_dev3 = vt_init_device(&ret, V4L2_EXT_DEV_PATH_CAPTURE3, V4L2_VT_CAPTURE3_DEVICE_NAME, V4L2_EXT_DEV_NO_CAPTURE3);

    return ret;
}

void __exit v4l2_vtdev_capture3_exit(void)
{
    vt_deinit_device(g_vt_v4l2_dev3);
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(v4l2_vtdev_capture3_init);
module_exit(v4l2_vtdev_capture3_exit);
#endif

/*I6DDMA device init*/
struct vt_v4l2_device *g_vt_v4l2_dev4 = NULL; 
int __init v4l2_vtdev_capture4_init(void)
{
    int ret;

    g_vt_v4l2_dev4 = vt_init_device(&ret, V4L2_EXT_DEV_PATH_CAPTURE4, V4L2_VT_CAPTURE4_DEVICE_NAME, V4L2_EXT_DEV_NO_CAPTURE4);

    return ret;
}

void __exit v4l2_vtdev_capture4_exit(void)
{
    vt_deinit_device(g_vt_v4l2_dev4);
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(v4l2_vtdev_capture4_init);
module_exit(v4l2_vtdev_capture4_exit);
#endif


/* DC2H1 & DC2H2 device init & exit */
struct vt_v4l2_device *g_vt_v4l2_dev5 = NULL;
int __init v4l2_vtdev_capture5_init(void)
{
    int ret;
    g_vt_v4l2_dev5 = vt_init_device(&ret, V4L2_EXT_DEV_PATH_CAPTURE5, V4L2_VT_CAPTURE5_DEVICE_NAME, V4L2_EXT_DEV_NO_CAPTURE5);

    return ret;
}

void __exit v4l2_vtdev_capture5_exit(void)
{
    vt_deinit_device(g_vt_v4l2_dev5);
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(v4l2_vtdev_capture5_init);
module_exit(v4l2_vtdev_capture5_exit);
#endif


struct vt_v4l2_device *g_vt_v4l2_dev6 = NULL;
int __init v4l2_vtdev_capture6_init(void)
{
    int ret;
    g_vt_v4l2_dev6 = vt_init_device(&ret, V4L2_EXT_DEV_PATH_CAPTURE6, V4L2_VT_CAPTURE6_DEVICE_NAME, V4L2_EXT_DEV_NO_CAPTURE6);

    return ret;
}

void __exit v4l2_vtdev_capture6_exit(void)
{
    vt_deinit_device(g_vt_v4l2_dev6);
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(v4l2_vtdev_capture6_init);
module_exit(v4l2_vtdev_capture6_exit);
#endif
