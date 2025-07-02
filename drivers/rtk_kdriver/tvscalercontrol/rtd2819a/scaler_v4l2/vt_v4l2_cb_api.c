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
#include <linux/errno.h>
#include <linux/videodev2.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/pageremap.h>
#include <linux/wait.h>
#include <linux/version.h>
#include <asm/unaligned.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-common.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <mach/rtk_log.h>
#include <mach/rtk_platform.h>

#include <ioctrl/scaler/vt_cmd_id.h>
#include <ioctrl/scaler/v4l2_ext_vt.h>
#include <ioctrl/scaler/vsc_cmd_id.h>
#include "vt_v4l2_api.h"


#ifdef CONFIG_COMPAT
#include <linux/compat.h>
#define to_user_ptr(x)          compat_ptr((unsigned long)x)
#else
#define to_user_ptr(x)          ((void* __user)(x)) // convert 32 bit value to user pointer
#endif

//common
#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
#include <scaler/scalerCommon.h>
#else
#include <scalercommon/scalerCommon.h>
#endif


#include <scaler_vtdev.h>
#include <include/rtk_kdriver/rtd_log/rtd_module_log.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <tvscalercontrol/i3ddma/i3ddma.h>


static unsigned char save_hdmi_port[VT_MAX_DEV_NUM] = {0};
static StructSrcRect save_vt_output_size[VT_MAX_DEV_NUM] = {0};
static unsigned char save_dev_buffer_num[VT_MAX_DEV_NUM] = {0};
static unsigned char save_source_type[VT_MAX_DEV_NUM] = {VSC_INPUTSRC_HDMI};
static unsigned char save_memory_type[VT_MAX_DEV_NUM] = {0};
static unsigned char save_pixel_format[VT_MAX_DEV_NUM] = {I3DDMA_COLOR_NV8BITS};
static unsigned int save_pitch[VT_MAX_DEV_NUM] = {0};
static unsigned int save_sizeiamge[VT_MAX_DEV_NUM] = {0};
/*-----------------------------------------------------------*/
extern unsigned int Scaler_DispGetInputInfoByDisp(unsigned char channel, SLR_INPUT_INFO infoList);
extern unsigned char rtk_hal_vsc_GetOutputRegion(KADP_VIDEO_WID_T wid, KADP_VIDEO_RECT_T * poutregion);
extern unsigned char rtk_hal_vsc_GetInputRegion(KADP_VIDEO_WID_T wid, KADP_VIDEO_RECT_T * pinregion);
unsigned char scaler_force_run_idma_impl(unsigned char dev, unsigned char mode, VSC_INPUT_TYPE_T inputSrctype);

/*-----3 Extended control information for implementation--reference spec 3.14-3.15--*/
void set_hdmi_port_by_dev_num(unsigned char dev_num, unsigned char hdmi_port)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return;
    save_hdmi_port[dev_num] = hdmi_port;
}

unsigned char get_hdmi_port_by_dev_num(unsigned char dev_num)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return 0;
    return save_hdmi_port[dev_num];
}

void set_vt_output_size(unsigned char dev_num, unsigned short x, unsigned short y, unsigned short w, unsigned short h)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return;
    save_vt_output_size[dev_num].srcx = x;
    save_vt_output_size[dev_num].srcy = y;
    save_vt_output_size[dev_num].src_wid = w;
    save_vt_output_size[dev_num].src_height = h;
}

StructSrcRect get_vt_output_size(unsigned char dev_num)
{
    StructSrcRect output = {0, 0, 1920, 1080};

    if(dev_num > VT_MAX_DEV_NUM)
        return output;
    return save_vt_output_size[dev_num];
}

void set_vt_dev_buffer_num(unsigned char dev_num, unsigned char buffer_num)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return;
    save_dev_buffer_num[dev_num] = buffer_num;
}

unsigned char get_vt_dev_buffer_num(unsigned char dev_num)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return 0;

    return save_dev_buffer_num[dev_num];
}

void set_vt_capture_source_type(unsigned char dev_num, unsigned char src_type)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return;
    save_source_type[dev_num] = src_type;
}

unsigned char get_vt_capture_source_type(unsigned char dev_num)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return VSC_INPUTSRC_HDMI;
    return save_source_type[dev_num];
}

void set_vt_memory_type(unsigned char dev_num, unsigned char type)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return;
    
    save_memory_type[dev_num] = type;
}

unsigned char get_vt_memory_type(unsigned char dev_num)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return IDDMA_MEMORY_MMAP;
    return save_memory_type[dev_num];
}

void set_vt_pixel_format(unsigned char dev_num, unsigned char format)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return;

    save_pixel_format[dev_num] = format;
}

unsigned char get_vt_pixel_format(unsigned char dev_num)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return I3DDMA_COLOR_NV8BITS;

    return save_pixel_format[dev_num];
}

void set_vt_pitch(unsigned char dev_num, unsigned int pitch)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return;

    save_pitch[dev_num] = pitch;
}

unsigned int get_vt_pitch(unsigned char dev_num)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return 0;

    return save_pitch[dev_num];
}

void set_vt_sizeimage(unsigned char dev_num, unsigned int size)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return;

    save_sizeiamge[dev_num] = size;
}

unsigned int get_vt_sizeimage(unsigned char dev_num)
{
    if(dev_num > VT_MAX_DEV_NUM)
        return 0;

    return save_sizeiamge[dev_num];
}
/*ictrl api*/
int vt_v4l2_ioctl_s_ext_ctrls(struct file *file, void *fh, struct v4l2_ext_controls *ctrls)
{
    if(!ctrls)
    {
        rtd_pr_vt_err("func:%s [error] ctrls is null\n",__FUNCTION__);
        return -EFAULT;
    }
    if(!ctrls->controls)
    {
        rtd_pr_vt_err("func:%s [error] ctrls->controls is null\n",__FUNCTION__);
        return -EFAULT;
    }

    rtd_pr_vt_notice("fun:%s, id=0x%x\n", __FUNCTION__, ctrls->controls->id);


    if(ctrls->controls->id == V4L2_CID_EXT_CAPTURE_FREEZE_MODE) //stop updating video frame in the capture buffer
    {
        struct v4l2_ext_capture_freeze_mode mode;
        struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
        unsigned int dev_num = 0;

        if(!ctrls->controls->ptr)
        {
            rtd_pr_vt_err("func:%s controls->ptr is null\n",__FUNCTION__);
            return -EFAULT;
        }

        if(copy_from_user((void *)&mode, (const void __user *)ctrls->controls->ptr, sizeof(struct v4l2_ext_capture_freeze_mode)))
        {
            rtd_pr_vt_err("func:%s freeze_mode copy_from_user controls->ptr fail \n",__FUNCTION__);
            return -EFAULT;
        }
        
        if(vt_dev)
        {
            dev_num = vt_dev->vt_dev_num;
            rtd_pr_vt_notice("Freeze dev%d,val [%d,%d]\n", dev_num, mode.plane_index, mode.val);
            switch(dev_num)
            {
                case I3DDMA_DEV:
                case I4DDMA_DEV:
                case I5DDMA_DEV:
                case I6DDMA_DEV:
                    h3ddma_nn_set_cap_enable(dev_num, mode.val);
                break;
                case VT_DC2H1_DEV:
                case VT_DC2H2_DEV:
                {
                    drvif_set_dc2h_capture_freeze(mode.val, dev_num - VT_IDDMA_DEV_NUM);
                }
                break;
            }
        }
    }
    else if(ctrls->controls->id == V4L2_CID_EXT_CAPTURE_PLANE_PROP)
    {
        struct v4l2_ext_capture_plane_prop plane_prop;
        struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
        unsigned int dev_num = 0;

        if(!ctrls->controls->ptr)
        {
            rtd_pr_vt_err("func:%s [error] controls->ptr is null\n",__FUNCTION__);
            return -EFAULT;
        }

        if(copy_from_user((void *)&plane_prop, (const void __user *)ctrls->controls->ptr, sizeof(struct v4l2_ext_capture_plane_prop)))
        {
            rtd_pr_vt_err("func:%s [error] set plane property copy_from_user controls->ptr fail \r\n",__FUNCTION__);
            return -EFAULT;
        }

        rtd_pr_vt_notice("capture location %d, plane size(%d,%d,%d,%d), buf_count %d\n", \
        plane_prop.l, plane_prop.plane.x, plane_prop.plane.y, plane_prop.plane.w, plane_prop.plane.h, plane_prop.buf_count);

        /*= check parameters:if doesn't support a capture location,size,buf count,return EOPNOTSUPP =*/
        if(plane_prop.plane.w > _DISP_WID || plane_prop.plane.h > _DISP_LEN)
        {
            rtd_pr_vt_err("[error]size unsupport over panel size\n");
            return -EOPNOTSUPP;
        }

        if(vt_dev)
        {
            dev_num = vt_dev->vt_dev_num;
            switch(dev_num)
            {
                case I3DDMA_DEV:
                case I4DDMA_DEV:
                case I5DDMA_DEV:
                case I6DDMA_DEV:
                {
                    unsigned char src_type = VSC_INPUTSRC_HDMI;
                    if(plane_prop.l == V4L2_EXT_CAPTURE_HDMI_RX)
                        src_type = VSC_INPUTSRC_HDMI;
                    else if(plane_prop.l == V4L2_EXT_CAPTURE_DP_RX)
                        src_type = VSC_INPUTSRC_DP;
                    else if(plane_prop.l == V4L2_EXT_CAPTURE_VODMA)
                        src_type = VSC_INPUTSRC_VDEC;
                    else
                    {
                        rtd_pr_vt_err("[error] unsupport capture source type %d", plane_prop.l);
                        return -EOPNOTSUPP;
                    }
                    set_vt_capture_source_type(dev_num, src_type);
                    set_vt_dev_buffer_num(dev_num, plane_prop.buf_count);
                }
                break;
                case VT_DC2H1_DEV:
                case VT_DC2H2_DEV:
                {
                    set_vt_capture_source_type(dev_num, plane_prop.l);
                    save_vt_dc2h_capture_location(dev_num - VT_IDDMA_DEV_NUM, plane_prop.l);
                    set_vt_dev_buffer_num(dev_num, plane_prop.buf_count);
                    set_vt_VtBufferNum(plane_prop.buf_count, dev_num - VT_IDDMA_DEV_NUM);
                    save_vt_dc2h_output_size(dev_num - VT_IDDMA_DEV_NUM, plane_prop.plane.x, plane_prop.plane.y, plane_prop.plane.w, plane_prop.plane.h);
                }
                break;
            }
        }
    }
    else if(ctrls->controls->id == V4L2_CID_EXT_CAPTURE_PLANE_INFO)
    {
        struct v4l2_ext_capture_plane_info vt_plane_info;
        struct v4l2_ext_video_rect panel_size;
        struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
        unsigned int dev_num = 0;

        if(!ctrls->controls->ptr)
        {
            rtd_pr_vt_err("func:%s [error] controls->ptr is null\n", __FUNCTION__);
            return -EFAULT;
        }
        if(copy_from_user((void *)&vt_plane_info, (const void __user *)ctrls->controls->ptr, sizeof(struct v4l2_ext_capture_plane_info)))
        {
            rtd_pr_vt_err("func:%s [error] set capture plane info copy_from_user controls->ptr fail\n", __FUNCTION__);
            return -EFAULT;
        }

        rtd_pr_vt_notice("hdmi_port %d,video scan tye %d\n", vt_plane_info.port, vt_plane_info.type); /*0 interlaced, 1 progressive*/
        rtd_pr_vt_notice("set capture input region (%d %d %d %d)\n", vt_plane_info.in.x, vt_plane_info.in.y, vt_plane_info.in.w, vt_plane_info.in.h);
        rtd_pr_vt_notice("set capture output region (%d %d %d %d)\n", vt_plane_info.out.x, vt_plane_info.out.y, vt_plane_info.out.w, vt_plane_info.out.h);

        if(vt_dev != NULL)
        {
            dev_num = vt_dev->vt_dev_num;
            set_hdmi_port_by_dev_num(dev_num, vt_plane_info.port);
            set_vt_output_size(dev_num, vt_plane_info.out.x, vt_plane_info.out.y, vt_plane_info.out.w, vt_plane_info.out.h);
            save_vt_dc2h_output_size(dev_num - VT_IDDMA_DEV_NUM, vt_plane_info.out.x, vt_plane_info.out.y, vt_plane_info.out.w, vt_plane_info.out.h);
        }

        panel_size.x = 0;
        panel_size.y = 0;
        panel_size.w = _DISP_WID;
        panel_size.h = _DISP_LEN;

        if(&(vt_plane_info.panel))
        {
            unsigned int byte_no_copy = 0;

            byte_no_copy = copy_to_user(to_user_ptr(&vt_plane_info.panel), (void *)&panel_size, sizeof(struct v4l2_ext_video_rect));
            if(byte_no_copy)
            {
                rtd_pr_vt_err("func:%s [error] panel_size copy_to_user fail %d\n", __FUNCTION__, byte_no_copy);
            }
        }
        else
        {
            rtd_pr_vt_err("func:%s [error] panel_size copy_to_user fail, due to NULL pointer \n", __FUNCTION__);
        }
    }
    else if(ctrls->controls->id == V4L2_CID_EXT_CAPTURE_VIDEO_FPS)
    {
        struct v4l2_ext_video_fps vt_fps;
        struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
        unsigned int dev_num = 0;
        if(!ctrls->controls->ptr)
        {
            rtd_pr_vt_err("func:%s [error] controls->ptr is null\n", __FUNCTION__);
            return -EFAULT;
        }
        if(copy_from_user((void *)&vt_fps, (const void __user *)ctrls->controls->ptr, sizeof(struct v4l2_ext_video_fps)))
        {
            rtd_pr_vt_err("func:%s [error] set vt_fps copy_from_user controls->ptr fail\n", __FUNCTION__);
            return -EFAULT;
        }

        rtd_pr_vt_notice("set input/output capture framerate %d/%d\n", vt_fps.input_fps, vt_fps.output_fps);
        if(vt_dev != NULL){
            dev_num = vt_dev->vt_dev_num;
            if(dev_num == VT_DC2H1_DEV || dev_num == VT_DC2H2_DEV){
                set_vt_target_fps(vt_fps.output_fps, dev_num - VT_IDDMA_DEV_NUM);
            }
        }
    }
    else if(ctrls->controls->id == V4L2_CID_EXT_CAPTURE_VIDEO_WIN_INFO)
    {
        struct v4l2_ext_capture_video_win_info video_win_info;
        struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
        unsigned int dev_num = 0;
        if(vt_dev == NULL){
            rtd_pr_vt_err("func:%s [error] vt_dev is null\n", __FUNCTION__);
            return -EFAULT;
        }

        dev_num = vt_dev->vt_dev_num;
        if(!ctrls->controls->ptr)
        {
            rtd_pr_vt_err("func:%s [error] controls->ptr is null\n", __FUNCTION__);
            return -EFAULT;
        }

        rtd_pr_vt_notice("func %s,line %d, dev_num %d\n", __FUNCTION__, __LINE__, dev_num);

        if(copy_from_user((void *)&video_win_info, (const void __user *)ctrls->controls->ptr, sizeof(struct v4l2_ext_capture_video_win_info)))
        {
            rtd_pr_vt_err("func:%s [error] video_win_info copy_from_user controls->ptr fail\n", __FUNCTION__);
            return -EFAULT;
        }
        rtd_pr_vt_notice("video_win_info in(%d,%d,%d,%d),out(%d,%d,%d,%d),type %d, panel(%d,%d,%d,%d)\n", video_win_info.in.x, video_win_info.in.y, video_win_info.in.w, video_win_info.in.h,
            video_win_info.out.x, video_win_info.out.y, video_win_info.out.w, video_win_info.out.h, video_win_info.type,
            video_win_info.panel.x, video_win_info.panel.y, video_win_info.panel.w, video_win_info.panel.h);
    }

    return 0;
}

void get_vt_capability_info(unsigned char dev_num, struct v4l2_ext_capture_capability_info *cap)
{
    struct v4l2_ext_video_rect max_reslotion = {0,0,_DISP_WID,_DISP_LEN}; /*max resolution reference panel size*/

    if(dev_num < VT_IDDMA_DEV_NUM)
    {
        max_reslotion.x = 0;
        max_reslotion.y = 0;
        max_reslotion.w = IDDMA_MAX_OUTPUT_WIDTH;
        max_reslotion.h = IDDMA_MAX_OUTPUT_HEIGHT;
        cap->max_res = max_reslotion;
    }else{
        cap->max_res = max_reslotion;
    }

    cap->num_video_frame_buffer = 5;
    cap->scale_up_limit_w = 0;
    cap->scale_up_limit_h = 0;

    cap->scale_down_limit_h = VT_CAP_SCALE_DOWN_LIMIT_HEIGHT;
    cap->scale_down_limit_w = VT_CAP_SCALE_DOWN_LIMIT_WIDTH;

    cap->num_plane = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED | V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR | V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_PLANAR; // supported PLANAR

    cap->pixel_format =  V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit | V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_10bit | 
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB | V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ARGB | V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGBA |
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ABGR | V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV21 | V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YV12 | 
    V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_I420 | V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_P010;

    cap->flags = (V4L2_EXT_CAPTURE_CAP_SCALE_DOWN | V4L2_EXT_CAPTURE_CAP_DIVIDE_FRAMERATE | V4L2_EXT_CAPTURE_CAP_DISPLAY_VIDEO_DEINTERLACE | V4L2_EXT_CAPTURE_CAP_INPUT_VIDEO_DEINTERLACE);
}


void get_capture_video_win_info(unsigned char dev_num, struct v4l2_ext_capture_video_win_info *info)
{
    if(dev_num < VT_IDDMA_DEV_NUM)
    {
        StructSrcRect output = get_vt_output_size(dev_num);

        info->type = IDDMA_GetVFEStatus(dev_num, SLR_DISP_INTERLACE);

        info->in.x = IDDMA_GetVFEInfoByPort(dev_num, SLR_INPUT_IPH_ACT_STA_PRE);
        info->in.y = IDDMA_GetVFEInfoByPort(dev_num, SLR_INPUT_IPV_ACT_STA_PRE);;
        info->in.w = IDDMA_GetVFEInfoByPort(dev_num, SLR_INPUT_IPH_ACT_WID_PRE);;
        info->in.h = IDDMA_GetVFEInfoByPort(dev_num, SLR_INPUT_IPV_ACT_LEN_PRE);;

        info->out.x = output.srcx;
        info->out.y = output.srcy;
        info->out.w = output.src_wid;
        info->out.h = output.src_height;
    }
    else
    {
        if (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_ACTIVE)
        {
            struct v4l2_ext_video_rect OrginalVideo_in, VideoSize_out;
            info->type = V4L2_EXT_CAPTURE_VIDEO_PROGRESSIVE;

            memset(&OrginalVideo_in, 0, sizeof(struct v4l2_ext_video_rect));
            memset(&VideoSize_out, 0, sizeof(struct v4l2_ext_video_rect));

            rtk_hal_vsc_GetInputRegion(KADP_VIDEO_WID_0, (KADP_VIDEO_RECT_T *)(&OrginalVideo_in)); /* Orginal Video Size */
            rtk_hal_vsc_GetOutputRegion(KADP_VIDEO_WID_0, (KADP_VIDEO_RECT_T *)(&VideoSize_out));	/* Output Video Size */

            info->in = OrginalVideo_in;
            info->out = VideoSize_out;
        }
        else
        {
            info->type = V4L2_EXT_CAPTURE_VIDEO_PROGRESSIVE;

            info->in.x = 0; /* if no-signal, it should be 0 */
            info->in.y = 0;
            info->in.w = 0;
            info->in.h = 0;
            //info->in.w = _DISP_WID;
            //info->in.h = _DISP_LEN;

            info->out.x = 0;
            info->out.y = 0;
            info->out.w = _DISP_WID;
            info->out.h = _DISP_LEN;
        }
    }

    info->panel.x = 0;
    info->panel.y = 0;
    info->panel.w = _DISP_WID;
    info->panel.h = _DISP_LEN;
}

int vt_v4l2_ioctl_g_ext_ctrls(struct file *file, void *fh, struct v4l2_ext_controls *ctrls)
{
    struct v4l2_ext_control controls;
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned char dev_num = VT_IDDMA_DEV_NUM;

    if(vt_dev)
        dev_num = vt_dev->vt_dev_num;

    if(!ctrls)
    {
        rtd_pr_vt_err("func:%s [error] ctrls is null\n",__FUNCTION__);
        return -EFAULT;
    }
    if(!ctrls->controls)
    {
        rtd_pr_vt_err("func:%s [error] ctrls->controls is null\n",__FUNCTION__);
        return -EFAULT;
    }

    memcpy(&controls, ctrls->controls, sizeof(struct v4l2_ext_control));

    rtd_pr_vt_notice("fun:%s,id=0x%x\n", __FUNCTION__,controls.id);

    if(controls.id == V4L2_CID_EXT_CAPTURE_CAPABILITY_INFO) /* id 0*/
    {
        struct v4l2_ext_capture_capability_info vt_capability_info;
        memset(&vt_capability_info, 0, sizeof(struct v4l2_ext_capture_capability_info));

        if(!controls.ptr)
        {
            rtd_pr_vt_err("func:%s [error] controls->ptr is null\n",__FUNCTION__);
            return -EFAULT;
        }

        get_vt_capability_info(dev_num, &vt_capability_info);

        if(copy_to_user(to_user_ptr((controls.ptr)), &vt_capability_info, sizeof(struct v4l2_ext_capture_capability_info)))
        {
            rtd_pr_vt_err("func:%s [error] get vt_capability_info copy_to_user fail\n",__FUNCTION__);
            return -EFAULT;
        }
    }
    else if(controls.id == V4L2_CID_EXT_CAPTURE_PLANE_INFO) /* id=1*/
    {
        /*struct v4l2_ext_capture_plane_info vt_palne_info;
        memset(&vt_palne_info, 0, sizeof(struct v4l2_ext_capture_plane_info));
        if(!controls.ptr)
        {
            rtd_pr_vt_err("func:%s [error] controls->ptr is null\n",__FUNCTION__);
            return -EFAULT;  
        }

        vt_palne_info.stride = get_cap_buffer_Width();
        vt_palne_info.plane_region.x = 0;
        vt_palne_info.plane_region.y = 0;
        vt_palne_info.plane_region.w = vfod_capture_out_W;
        vt_palne_info.plane_region.h = vfod_capture_out_H;

        //same with plane region
        vt_palne_info.active_region = vt_palne_info.plane_region;

        rtd_pr_vt_notice("[VT] plane info1:(%d,%d,%d,%d,%d)",vt_palne_info.stride,vt_palne_info.plane_region.x,vt_palne_info.plane_region.y
            ,vt_palne_info.plane_region.w,vt_palne_info.plane_region.h);

        rtd_pr_vt_notice("[VT] plane info2:(%d,%d,%d,%d)",vt_palne_info.active_region.x,vt_palne_info.active_region.y,vt_palne_info.active_region.w
            ,vt_palne_info.active_region.h);

        if(copy_to_user(to_user_ptr((controls.ptr)), &vt_palne_info, sizeof(struct v4l2_ext_capture_plane_info)))
        {
            rtd_pr_vt_err("func:%s [error] get vt_palne_info copy_to_user fail \n",__FUNCTION__);
            return -EFAULT;
        }*/
    }
    else if(controls.id == V4L2_CID_EXT_CAPTURE_VIDEO_WIN_INFO) /* id=2*/
    {
        struct v4l2_ext_capture_video_win_info video_info;
        memset(&video_info, 0, sizeof(struct v4l2_ext_capture_video_win_info));

        if(!controls.ptr)
        {
            rtd_pr_vt_err("func:%s [error] controls->ptr is null\n",__FUNCTION__);
            return -EFAULT;
        }
        get_capture_video_win_info(dev_num, &video_info);

        if(copy_to_user(to_user_ptr((controls.ptr)), &video_info, sizeof(struct v4l2_ext_capture_video_win_info)))
        {
            rtd_pr_vt_err("func:%s [error] get video_win_info copy_to_user fail \n",__FUNCTION__);
            return -EFAULT;
        }
    }
    else if(controls.id == V4L2_CID_EXT_CAPTURE_FREEZE_MODE)/*id=4 */ //stop updating video frame in the capture buffer
    {
        struct v4l2_ext_capture_freeze_mode mode;
  
        if(!controls.ptr)
        {
            rtd_pr_vt_err("func:%s [error] controls->ptr is null\n",__FUNCTION__);
            return -EFAULT;
        }
        memset(&mode, 0, sizeof(struct v4l2_ext_capture_freeze_mode));

        if(dev_num < VT_IDDMA_DEV_NUM)
        {
            mode.plane_index = 1;
            mode.val = h3ddma_nn_get_cap_enable(dev_num);
        }
        else
        {
            mode.plane_index = get_index_of_freezed(dev_num - VT_IDDMA_DEV_NUM);
            mode.val = get_dc2h_dmaen_state(dev_num - VT_IDDMA_DEV_NUM);
        }

        if(copy_to_user(to_user_ptr((controls.ptr)), &mode, sizeof(struct v4l2_ext_capture_freeze_mode)))
        {
            rtd_pr_vt_err("func:%s [error] get freeze_mode copy_to_user fail \n",__FUNCTION__);
            return -EFAULT;
        }
    }
    else if(controls.id == V4L2_CID_EXT_CAPTURE_PHYSICAL_MEMORY_INFO) /*id=6*/
    {
        unsigned int y_addr = 0;
        unsigned int u_addr = 0;
        unsigned int v_addr = 0;
        struct v4l2_ext_capture_physical_memory_info info;
        memset(&info, 0, sizeof(struct v4l2_ext_capture_physical_memory_info));

        if(!controls.ptr)
        {
            rtd_pr_vt_err("func:%s [error] controls->ptr is null\n",__FUNCTION__);
            return -EFAULT;
        }

        if(copy_from_user((void *)&info, (const void __user *)controls.ptr, sizeof(struct v4l2_ext_capture_physical_memory_info)))
        {
            rtd_pr_vt_err("func:%s [error] get physical_memory copy_from_user controls->ptr fail \n",__FUNCTION__);
            return -EFAULT;
        }

        rtd_pr_vt_notice("fun:%s=%d,index:%d\n", __FUNCTION__, __LINE__, info.buf_index);

        if(dev_num == VT_DC2H1_DEV || dev_num == VT_DC2H2_DEV){
            int plane_num = get_dc2h_cap_fmt_plane_number(get_vt_pixel_format(dev_num));
            if(plane_num != 0 && plane_num <= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_PLANAR){
                if(info.y == NULL){
                    rtd_pr_vt_err("fun:%s=%d, y is null\n", __FUNCTION__, __LINE__);
                    return -EFAULT;
                }else{
                    y_addr = get_vt_dc2h_plane_buf_addr(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED, info.buf_index);
                    rtd_pr_vt_notice("index %d,addr y %x\n", info.buf_index, y_addr);
                    if(copy_to_user(to_user_ptr((info.y)), (void *)&y_addr, sizeof(unsigned int)))
                    {
                        rtd_pr_vt_err("func:%s [error] y addr copy_to_user fail \n",__FUNCTION__);
                        return -EFAULT;
                    }
                }

                if(plane_num > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){
                    if(info.u == NULL){
                        rtd_pr_vt_err("fun:%s=%d, u is null\n", __FUNCTION__, __LINE__);
                        return -EFAULT;
                    }else{
                        u_addr = get_vt_dc2h_plane_buf_addr(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR, info.buf_index);
                        rtd_pr_vt_notice("index %d,addr u %x\n", info.buf_index, u_addr);
                        if(copy_to_user(to_user_ptr((info.u)), (void *)&u_addr, sizeof(unsigned int)))
                        {
                            rtd_pr_vt_err("func:%s [error] u addr copy_to_user fail \n",__FUNCTION__);
                            return -EFAULT;
                        }
                    }
                }

                if(plane_num > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){
                    if(info.v == NULL){
                        rtd_pr_vt_err("fun:%s=%d, v is null\n", __FUNCTION__, __LINE__);
                        return -EFAULT;
                    }else{
                        v_addr = get_vt_dc2h_plane_buf_addr(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_PLANAR, info.buf_index);
                        rtd_pr_vt_notice("index %d,addr v %x\n", info.buf_index, v_addr);
                        if(copy_to_user(to_user_ptr((info.v)), (void *)&v_addr, sizeof(unsigned int)))
                        {
                            rtd_pr_vt_err("func:%s [error] v addr copy_to_user fail \n",__FUNCTION__);
                            return -EFAULT;
                        }
                    }
                }
            }
        }else{
            /* ixddma dev*/
        }
    }
    else if(controls.id == V4L2_CID_EXT_CAPTURE_PLANE_PROP) /*id=3*/
    {
        struct v4l2_ext_capture_plane_prop plane_prop;
        memset(&plane_prop, 0, sizeof(struct v4l2_ext_capture_plane_prop));

        if(!controls.ptr)
        {
            rtd_pr_vt_err("func:%s [error] controls->ptr is null \n",__FUNCTION__);
            return -EFAULT;
        }
        rtd_pr_vt_notice("func %s,line %d\n", __FUNCTION__, __LINE__);
        if(dev_num < VT_IDDMA_DEV_NUM)
        {
            if(get_vt_capture_source_type(dev_num) == VSC_INPUTSRC_HDMI)
                plane_prop.l = V4L2_EXT_CAPTURE_HDMI_RX;
            else if(get_vt_capture_source_type(dev_num) == VSC_INPUTSRC_DP)
                plane_prop.l = V4L2_EXT_CAPTURE_DP_RX;
            else if(get_vt_capture_source_type(dev_num) == VSC_INPUTSRC_VDEC)
                plane_prop.l = V4L2_EXT_CAPTURE_VODMA;
            
            plane_prop.buf_count = get_vt_dev_buffer_num(dev_num);
            plane_prop.plane.x = 0;
            plane_prop.plane.y = 0;
            plane_prop.plane.w = _DISP_WID;
            plane_prop.plane.h = _DISP_LEN;
        }
        else
        {
            plane_prop.l = get_vt_capture_source_type(dev_num);
            plane_prop.buf_count = get_vt_dev_buffer_num(dev_num);
            plane_prop.plane.x = 0;
            plane_prop.plane.y = 0;
            plane_prop.plane.w = _DISP_WID;
            plane_prop.plane.h = _DISP_LEN;
        }

        if(copy_to_user(to_user_ptr((controls.ptr)), &plane_prop, sizeof(struct v4l2_ext_capture_plane_prop)))
        {
            rtd_pr_vt_err("func:%s [error] get freeze_mode copy_to_user fail \n",__FUNCTION__);
            return -EFAULT;
        }
    }

    return 0;
}


int vt_v4l2_ioctl_s_ctrl(struct file *file, void *fh, struct v4l2_control *ctrl)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned char dev_num = VT_IDDMA_DEV_NUM;

    if(vt_dev)
        dev_num = vt_dev->vt_dev_num;

    if(!ctrl)
    {
        rtd_pr_vt_err("func:%s [error] ctrl is null\n",__FUNCTION__);
        return -EFAULT;
    }

    rtd_pr_vt_notice("fun:%s=%d,id=0x%x\n", __FUNCTION__, __LINE__, ctrl->id);

    if(ctrl->id == V4L2_CID_EXT_CAPTURE_DONE_USER_PROCESSING)
    {
        rtd_pr_vt_err("fun:%s=%d,unsupport this cmd\n", __FUNCTION__, __LINE__);
        return -EFAULT;
    }
    else if(ctrl->id == V4L2_CID_EXT_CAPTURE_DIVIDE_FRAMERATE)
    {
        rtd_pr_vt_notice("set_framerateDivide=%d\n", ctrl->value);
        if(ctrl->value <= 0)
        {
            rtd_pr_vt_err("invalid value\n");
            return -EFAULT;
        }
        if(dev_num < VT_IDDMA_DEV_NUM)
        {
            return -EFAULT;
        }else{
            if(ctrl->value != get_framerate_divide(dev_num - VT_IDDMA_DEV_NUM)){
                set_framerate_divide(ctrl->value, dev_num - VT_IDDMA_DEV_NUM);
            }
        }
    }

    return 0;
}

int vt_v4l2_ioctl_g_ctrl(struct file *file, void *fh, struct v4l2_control *ctrl)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned char dev_num = VT_IDDMA_DEV_NUM;

    if(vt_dev)
        dev_num = vt_dev->vt_dev_num;

    if(!ctrl)
    {
        rtd_pr_vt_err("#####[%s(%d)][error] ctrl is null\n",__FUNCTION__,__LINE__);
        return -EFAULT;
    }

    rtd_pr_vt_notice("fun:%s=%d,id=0x%x\n", __FUNCTION__, __LINE__,ctrl->id);

    if(ctrl->id == V4L2_CID_EXT_CAPTURE_OUTPUT_FRAMERATE)
    {
        if(dev_num < VT_IDDMA_DEV_NUM)
            ctrl->value = IDDMA_GetVFEInfoByPort(dev_num, SLR_INPUT_V_FREQ_1000/1000);
        else
            ctrl->value = get_vt_capture_framerate(get_vt_capture_source_type(dev_num - VT_IDDMA_DEV_NUM));

        rtd_pr_vt_notice("get vfod_framerate=%d\n",ctrl->value);
    }
    else if(ctrl->id == V4L2_CID_EXT_CAPTURE_DIVIDE_FRAMERATE)
    {
        if(dev_num < VT_IDDMA_DEV_NUM)
            ctrl->value = 1;
        else{
            ctrl->value = get_framerate_divide(dev_num - VT_IDDMA_DEV_NUM);
            rtd_pr_vt_notice("get_framerateDivide=%d\n",ctrl->value);
        }
    }

    return 0;
}

int vt_v4l2_ioctl_subscribe_event(struct v4l2_fh *fh, const struct v4l2_event_subscription *sub)
{
    return 0;
}

int vt_v4l2_ioctl_unsubscribe_event(struct v4l2_fh *fh, const struct v4l2_event_subscription *sub)
{
    return 0;
}

/*-----2 standard control information for implementation-----*/
int vt_v4l2_ioctl_querycap(struct file *file, void *fh, struct v4l2_capability *cap)
{
    char *chipname = "rtd2819a";
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);

    rtd_pr_vt_notice("func:%s,chipname:%s\n", __FUNCTION__,chipname);
    if(!cap)
    {
        rtd_pr_vt_err("func:%s [error] ctrl is null\n",__FUNCTION__);
        return -EFAULT;
    }

    if(vt_dev){
        memcpy(cap->driver, vt_dev->v4l2_dev.name, strlen(vt_dev->v4l2_dev.name));
    }
    cap->version = KERNEL_VERSION(0, 0, 1);
    cap->capabilities = (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_VIDEO_CAPTURE_MPLANE | V4L2_CAP_STREAMING | V4L2_CAP_DEVICE_CAPS);
    //cap->device_caps = V4L2_CAP_VIDEO_CAPTURE_MPLANE;  /* must set device_caps field */

    return 0;
}


int vt_v4l2_ioctl_reqbufs(struct file *file, void *fh, struct v4l2_requestbuffers *b)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned char dev_num = VT_IDDMA_DEV_NUM;

    if(!b)
    {
        rtd_pr_vt_err("func:%s [error] pointer is null\n",__FUNCTION__);
        return -EFAULT;
    }

    rtd_pr_vt_notice("reqbufs: count %d, type %d, memory %d,capabilities %d\n", __FUNCTION__, b->count, b->type, b->memory, b->capabilities);

/* Once request buffer number over the capability, return fail and update the max capability number */
    if(b->count > MAX_VT_BUFFERS_NUM && b->memory == V4L2_MEMORY_MMAP)
    {
        b->count = MAX_VT_BUFFERS_NUM;
        rtd_pr_vt_err("fun: %s [warning] max support %d buffers\n", __FUNCTION__, b->count);
        return -EFAULT;
    }
    else if( b->memory == V4L2_MEMORY_DMABUF)
    {
        rtd_pr_vt_err("fun: %s  V4L2_MEMORY_DMABUF\n", __FUNCTION__);
    }
    else if(b->memory == V4L2_MEMORY_USERPTR)
    {
        rtd_pr_vt_err("fun: %s [error] not support\n", __FUNCTION__);
        return -EFAULT;
    }

    if(vt_dev)
    {
        dev_num = vt_dev->vt_dev_num;
        
        if(dev_num < VT_IDDMA_DEV_NUM)
        {
            unsigned char type = IDDMA_MEMORY_MMAP;
            switch(b->memory)
            {
                case V4L2_MEMORY_MMAP:
                    type = IDDMA_MEMORY_MMAP;
                break;
                case V4L2_MEMORY_USERPTR:
                    type = IDDMA_MEMORY_USERPTR;
                break;
                case V4L2_MEMORY_DMABUF:
                    type = IDDMA_MEMORY_DMA;
                break;
            }
            set_vt_memory_type(dev_num, type);
            set_vt_dev_buffer_num(dev_num, b->count);
            if(b->memory == V4L2_MEMORY_MMAP)
            {
                if(IDDMA_Allocat_memoryBuf(dev_num) != TRUE)
                    return -ENOMEM;
            }
            
        }
        else
        {
            unsigned char type = DC2H_MEMORY_MMAP;
            switch(b->memory){
                case V4L2_MEMORY_MMAP:
                    type = DC2H_MEMORY_MMAP;
                break;
                case V4L2_MEMORY_DMABUF:
                    type = DC2H_MEMORY_DMA;
                break;
                case V4L2_MEMORY_USERPTR:
                    type = DC2H_MEMORY_USERPTR;
                break;
                default:
                break;
            }
            set_vt_memory_type(dev_num, b->memory);
            set_vt_dev_buffer_num(dev_num, b->count);
            set_vt_VtBufferNum(b->count, dev_num - VT_IDDMA_DEV_NUM);
            set_vt_dc2h_cap_mem_type(dev_num - VT_IDDMA_DEV_NUM, type);
            if(b->memory == V4L2_MEMORY_MMAP)
            {
                if(do_vt_reqbufs(b->count, dev_num - VT_IDDMA_DEV_NUM) == FALSE){
                    return -ENOMEM;
                }
            }
        }
    }

    return  0;
}


int vt_v4l2_ioctl_querybuf(struct file *file, void *fh, struct v4l2_buffer *b)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned char dev_num = VT_IDDMA_DEV_NUM;

    if(!b)
    {
        rtd_pr_vt_err("func:%s [error] pointer is null\n",__FUNCTION__);
        return -EFAULT;
    }

    rtd_pr_vt_notice("querybuf:index %d, memory %d\n", b->index, b->memory);

    if(vt_dev)
        dev_num = vt_dev -> vt_dev_num;

    if(V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR == b->length) // SEMI PLANAR has two plane.
    {
        if(dev_num < VT_IDDMA_DEV_NUM)
        {
            b->m.planes[0].data_offset = 0;
            b->m.planes[0].m.mem_offset = IDDMA_Get_YbufAddr_byIdx(dev_num, b->index);
            b->m.planes[0].length = IDDMA_Get_YbufSize(dev_num);

            b->m.planes[1].data_offset = 0;
            b->m.planes[1].length = IDDMA_Get_UVbufSize(dev_num);
            b->m.planes[1].m.mem_offset = b->m.planes[0].m.mem_offset + b->m.planes[0].length;
        }
        else
        {
            b->m.planes[0].data_offset = 0;
            b->m.planes[0].m.mem_offset = get_vt_dc2h_plane_buf_addr(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED, b->index);
            b->m.planes[0].length = get_vt_dc2h_plane_buf_size(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED);

            b->m.planes[1].data_offset = 0;
            b->m.planes[1].m.mem_offset = get_vt_dc2h_plane_buf_addr(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR, b->index);
            b->m.planes[1].length = get_vt_dc2h_plane_buf_size(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR);
        }
    }
    else if(V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED == b->length) // INTERLEAVED has one plane
    {
        if(dev_num < VT_IDDMA_DEV_NUM)
        {
            rtd_pr_vt_err("line:%d [error] unsupport format\n",__LINE__);
            return -EOPNOTSUPP ;
        }else{
            b->m.planes[0].data_offset = 0;
            b->m.planes[0].m.mem_offset = get_vt_dc2h_plane_buf_addr(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED, b->index);
            b->m.planes[0].length = get_vt_dc2h_plane_buf_size(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED);
        }
    }
    else if(V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_PLANAR == b->length) // PLANAR has three plane
    {
        if(dev_num < VT_IDDMA_DEV_NUM)
        {
            b->m.planes[0].data_offset = 0;
            b->m.planes[0].m.mem_offset = IDDMA_Get_YbufAddr_byIdx(dev_num, b->index);
            b->m.planes[0].length = IDDMA_Get_YbufSize(dev_num);

            b->m.planes[1].data_offset = 0;
            b->m.planes[1].length = IDDMA_Get_UVbufSize(dev_num);
            b->m.planes[1].m.mem_offset = b->m.planes[0].m.mem_offset + b->m.planes[0].length;

            b->m.planes[2].data_offset = 0;
            b->m.planes[2].length = IDDMA_Get_UVbufSize(dev_num);
            b->m.planes[2].m.mem_offset = b->m.planes[0].m.mem_offset + b->m.planes[0].length + b->m.planes[1].length;
        }
        else
        {
            b->m.planes[0].data_offset = 0;
            b->m.planes[0].m.mem_offset = get_vt_dc2h_plane_buf_addr(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED, b->index);
            b->m.planes[0].length = get_vt_dc2h_plane_buf_size(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED);

            b->m.planes[1].data_offset = 0;
            b->m.planes[1].m.mem_offset = get_vt_dc2h_plane_buf_addr(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR, b->index);
            b->m.planes[1].length = get_vt_dc2h_plane_buf_size(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR);

            b->m.planes[2].data_offset = 0;
            b->m.planes[2].m.mem_offset = get_vt_dc2h_plane_buf_addr(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_PLANAR, b->index);
            b->m.planes[2].length = get_vt_dc2h_plane_buf_size(dev_num - VT_IDDMA_DEV_NUM, V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_PLANAR);
        }
    }
    else
    {
        rtd_pr_vt_err("func:%s [error] unsupport plane number %d\n",__FUNCTION__, b->length);
        return -EFAULT;
    }

    return  0;
}

int vt_v4l2_ioctl_qbuf(struct file *file, void *fh, struct v4l2_buffer *b)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned char dev_num = VT_IDDMA_DEV_NUM;

    if(vt_dev)
        dev_num = vt_dev->vt_dev_num;

    if(!b)
    {
        rtd_pr_vt_err("func:%s [error] pointer is null\n",__FUNCTION__);
        return -EFAULT;
    }

    //rtd_pr_vt_notice("func:%s,index %d,type %d\n", __FUNCTION__, b->index, b->type);
    
    if(b->memory == V4L2_MEMORY_MMAP)
    {
        rtd_pr_vt_notice("func:%s  v4l2 mmap %d\n", __FUNCTION__, b->m.offset);
        if(dev_num < VT_IDDMA_DEV_NUM)
        {
            unsigned char type = V4L2_MEMORY_MMAP;
            switch(IDDMA_Get_memoryType(dev_num))
            {
                case IDDMA_MEMORY_MMAP:
                    type = V4L2_MEMORY_MMAP;
                break;
                case IDDMA_MEMORY_USERPTR:
                    type = V4L2_MEMORY_USERPTR;
                break;
                case IDDMA_MEMORY_DMA:
                    type = V4L2_MEMORY_DMABUF;
                break;
            }
            if(b->memory != type)
            {
                rtd_pr_vt_err("func:%s  v4l2 buffer memory type error %d %d\n", __FUNCTION__, b->memory, type);
                return -EFAULT;
            }

            if(b->index > get_vt_dev_buffer_num(dev_num))
            {
                rtd_pr_vt_notice("func:%s  increase buffer num %d %d\n", __FUNCTION__, b->index, get_vt_dev_buffer_num(dev_num));
                return -EFAULT;
            }

            if(IDDMA_QBuf(dev_num, b->index, 0, 0, 0) == FALSE)
            {
                rtd_pr_vt_err("func:%s  v4l2 buffer memory error %d %d\n", __FUNCTION__, b->memory, type);
                return -EFAULT;
            }
        }
        else
        {
            if(get_vt_dc2h_cap_mem_type(dev_num - VT_IDDMA_DEV_NUM) != DC2H_MEMORY_MMAP){
                rtd_pr_vt_err("func:%s buffer memory type error %d\n", __FUNCTION__, get_vt_dc2h_cap_mem_type(dev_num - VT_IDDMA_DEV_NUM));
                return -EFAULT;
            }

            if(do_vt_dc2h_qbuf(b->index, dev_num - VT_IDDMA_DEV_NUM, 0, 0) == FALSE){
                rtd_pr_vt_err("func:%s,v4l2 buffer memory error %d\n", __FUNCTION__, b->memory);
                return -EFAULT;
            }
        }
    }
    else if(b->memory == V4L2_MEMORY_USERPTR)
    {
        rtd_pr_vt_notice("func:%s  v4l2 userptr 0x%x\n", __FUNCTION__, (unsigned int)b->m.userptr);
        rtd_pr_vt_err("not support V4L2_MEMORY_USERPTR\n");
        return -EOPNOTSUPP;
    }
    else if(b->memory == V4L2_MEMORY_DMABUF)
    {
        struct dma_buf *p_dmabuf = NULL;

        if(IDDMA_Get_memoryType(dev_num) != IDDMA_MEMORY_DMA)
        {
            set_vt_memory_type(dev_num, IDDMA_MEMORY_DMA);
            IDDMA_Set_memoryType(dev_num, IDDMA_MEMORY_DMA);
        }

        if(b->m.fd < 0)
        {
            rtd_pr_vt_err("fd of dma_buf invalid!\n");
            return -EFAULT;
        }
        if(vt_dev->pdmabuf[b->index] == NULL)
        {
            p_dmabuf = dma_buf_get(b->m.fd);
            if(IS_ERR_OR_NULL(p_dmabuf))
            {
                rtd_pr_vt_err("get dma_buf by fd failed\n");
                return -EFAULT;
            }

            vt_dev->pdmabuf[b->index] = p_dmabuf;
        }

        if(IS_ERR_OR_NULL(vt_dev->pdmabuf[b->index]))
        {
            rtd_pr_vt_err("p_dmabuf is NULL\n");
            return -EFAULT;
        }

        if(dev_num < VT_IDDMA_DEV_NUM)
        {
            if(IDDMA_QBuf(dev_num, b->index, b->m.fd, vt_dev->pdmabuf[b->index]->size, b->sequence) == FALSE)
            {
                rtd_pr_vt_err("IDDMA_QBuf failed!\n");
                return -EFAULT;
            }  
        }
        else
        {
            if(do_vt_dc2h_qbuf(b->index, dev_num - VT_IDDMA_DEV_NUM, b->m.fd, p_dmabuf->size) == FALSE){
                rtd_pr_vt_err("func:%s, dma buffer qbuf fail\n");
                return -EFAULT;
            }
        }
    }

    return 0;
}

int vt_v4l2_ioctl_dqbuf(struct file *file, void *fh, struct v4l2_buffer *b)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned char dev_num = VT_IDDMA_DEV_NUM;

    if(vt_dev)
        dev_num = vt_dev->vt_dev_num;

    if(!b)
    {
        rtd_pr_vt_err("func:%s [error] pointer is null\n", __FUNCTION__);
        return -EFAULT;
    }

    //rtd_pr_vt_notice("%s, mem_tye %d\n", __FUNCTION__, b->memory); /* enum v4l2_memory , V4L2_MEMORY_DMABUF = 4 */

    if(dev_num < VT_IDDMA_DEV_NUM)
    {
        unsigned int ret = 0;

        b->flags = 0;

        if(b->memory == V4L2_MEMORY_MMAP)
        {
            ret = IDDMA_DQBuf(dev_num, &b->index, &b->m.fd, &b->sequence);

            //rtd_pr_vt_notice("func:%s index:%d\n", __FUNCTION__, b->index);
        }
        else if(b->memory == V4L2_MEMORY_DMABUF)
        {
            ret = IDDMA_DQBuf(dev_num, &b->index, &b->m.fd, &b->sequence);
            if(ret == -ENOLINK)
            {
                b->flags |= V4L2_BUF_FLAG_ERROR;
                ret = 0;
            }
            //rtd_pr_vt_notice("func:%s index:%d, fd:%d\n", __FUNCTION__, b->index, b->m.fd);

            //if (!IS_ERR_OR_NULL(vt_dev->pdmabuf[b->index]))
            //{
            //    dma_buf_put(vt_dev->pdmabuf[b->index]);
            //    vt_dev->pdmabuf[b->index] = NULL;
            //}
        }
        else
        {
            rtd_pr_vt_notice("func:%s  v4l2 userptr 0x%x\n", __FUNCTION__, (unsigned int)b->m.userptr);
            rtd_pr_vt_err("not support V4L2_MEMORY_USERPTR\n");
            ret = -EOPNOTSUPP;
        }

        return ret;
    }
    else
    {
        if(get_drm_case_securestatus() == TRUE)
        {
            rtd_pr_vt_err("fun:%s, unsupport VT capture in SVP video\n", __FUNCTION__);
            b->index = 0xFF;
            return -EACCES;
        }

        drvif_set_dc2h_wait_vsync(get_vt_capture_source_type(dev_num - VT_IDDMA_DEV_NUM), dev_num - VT_IDDMA_DEV_NUM);

        if(b->memory == V4L2_MEMORY_MMAP)
        {
            if(do_vt_dc2h_dqbuf(&b->index, dev_num - VT_IDDMA_DEV_NUM) == FALSE){
                rtd_pr_vt_err("EAGAIN: v4l2_ioctl dqbuf error,%d\n", __LINE__);
                return -EAGAIN;
            }
        }else if(b->memory == V4L2_MEMORY_DMABUF){
            if(do_vt_dc2h_dqbuf(&b->index, dev_num - VT_IDDMA_DEV_NUM) == FALSE){
                rtd_pr_vt_err("EAGAIN: v4l2_ioctl dqbuf error,%d\n", __LINE__);
                return -EAGAIN;
            }

            dma_buf_put(vt_dev->pdmabuf[b->index]);
        }else{
            rtd_pr_vt_notice("func:%s  v4l2 userptr 0x%x\n", __FUNCTION__, (unsigned int)b->m.userptr);
            rtd_pr_vt_err("not support V4L2_MEMORY_USERPTR\n");
            return -EOPNOTSUPP;         
        }

        return 0;
    }
}

int vt_v4l2_ioctl_streamon(struct file *file, void *fh, enum v4l2_buf_type i)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned char dev_num = VT_IDDMA_DEV_NUM;

    if(vt_dev)
        dev_num = vt_dev->vt_dev_num;

    rtd_pr_vt_notice("%s, v4l2 buf type %d\n", __FUNCTION__, i);

    if(dev_num < VT_IDDMA_DEV_NUM)
    {
        if(scaler_force_run_idma_impl(dev_num, IDDMA_CAP_MODE, (VSC_INPUT_TYPE_T)get_vt_capture_source_type(dev_num)) == FALSE)
        {
            rtd_pr_vt_err("%s, i%dddma init fail !!!!!\n", __FUNCTION__, (dev_num + 3));
            return -EFAULT;
        }
    }
    else
    {
        if(do_vt_dc2h_streamon(dev_num - VT_IDDMA_DEV_NUM) == FALSE){
            return -EFAULT;
        }
    }
    return 0;
}


int vt_v4l2_ioctl_streamoff(struct file *file, void *fh, enum v4l2_buf_type i)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned char dev_num = VT_IDDMA_DEV_NUM;

    if(vt_dev)
        dev_num = vt_dev->vt_dev_num;

    rtd_pr_vt_notice("%s\n", __FUNCTION__);

    if(dev_num < VT_IDDMA_DEV_NUM)
    {
        unsigned char i;
        IDDMA_DisableCap(dev_num);

        if(get_vt_memory_type(dev_num) == IDDMA_MEMORY_DMA)
        {
            for(i = 0; i < get_vt_dev_buffer_num(dev_num); ++i)
            {
                rtd_pr_vt_notice("%s dma put idx:%d dmabuf:%x", __FUNCTION__, i, vt_dev->pdmabuf[i]);
                if(vt_dev->pdmabuf[i])
                {
                    dma_buf_put(vt_dev->pdmabuf[i]);
                    vt_dev->pdmabuf[i] = NULL;
                }
            }
        }
    }
    else
    {
        do_vt_dc2h_streamoff(dev_num - VT_IDDMA_DEV_NUM);
        if(get_vt_memory_type(dev_num) == V4L2_MEMORY_DMABUF){
            for(i = 0; i < get_vt_dev_buffer_num(dev_num); ++i)
            {
                dma_buf_put(vt_dev->pdmabuf[i]);
            }
        }
    }

    return 0;
}

int vt_v4l2_ioctl_s_fmt_vid_cap(struct file *file, void *fh, struct v4l2_format *f)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned char dev_num = VT_IDDMA_DEV_NUM;

    if(vt_dev)
        dev_num = vt_dev->vt_dev_num;

    if(!f)
    {
        rtd_pr_vt_err("func:%s [error] pointer is null\r\n", __FUNCTION__);
        return -EFAULT;
    }

    rtd_pr_vt_notice("%s,fmt.type:%d,fmt %d\n", __FUNCTION__, f->type, f->fmt.pix.pixelformat);
    rtd_pr_vt_notice("%s,w:%d h:%d pitch:%d sizeimage:%d\n", __FUNCTION__, f->fmt.pix.width, f->fmt.pix.height, f->fmt.pix.bytesperline, f->fmt.pix.sizeimage);

    if(dev_num < VT_IDDMA_DEV_NUM)
    {
        unsigned char format = I3DDMA_COLOR_NV8BITS;
        set_vt_output_size(dev_num, 0, 0, f->fmt.pix.width, f->fmt.pix.height);
        set_vt_pitch(dev_num, f->fmt.pix.bytesperline);
        set_vt_sizeimage(dev_num, f->fmt.pix.sizeimage);
        switch(f->fmt.pix.pixelformat)
        {
            case V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit:
                format = I3DDMA_COLOR_NV8BITS;
            break;
            case V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YV12:
                format = I3DDMA_COLOR_YV12;
            break;
            case V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_I420:
                format = I3DDMA_COLOR_I420;
            break;
            case V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_10bit:
                format = I3DDMA_COLOR_NV10BITS;
            break;
            case V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_P010:
                format = I3DDMA_COLOR_P010;
            break;
        }
        set_vt_pixel_format(dev_num, format);
    }
    else
    {
        set_vt_pixel_format(dev_num, f->fmt.pix.pixelformat);
        set_vt_output_size(dev_num, 0, 0, f->fmt.pix.width, f->fmt.pix.height);
        set_VT_Pixel_Format(f->fmt.pix.pixelformat, dev_num - VT_IDDMA_DEV_NUM);
        save_vt_dc2h_output_size(dev_num - VT_IDDMA_DEV_NUM, 0, 0, f->fmt.pix.width, f->fmt.pix.height);
    }

    return 0;
}

int vt_v4l2_ioctl_g_fmt_vid_cap(struct file *file, void *fh, struct v4l2_format *f)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned char dev_num = VT_IDDMA_DEV_NUM;

    if(vt_dev)
        dev_num = vt_dev->vt_dev_num;

    rtd_pr_vt_notice("%s,fmt.type:%d\n", __FUNCTION__, f->type);

    if(!f)
    {
        rtd_pr_vt_err("func:%s [error] pointer is null\r\n",__FUNCTION__);
        return -EFAULT;
    }

    if(dev_num < VT_IDDMA_DEV_NUM)
    {
        switch(get_vt_pixel_format(dev_num))
        {
            case I3DDMA_COLOR_NV8BITS:
                f->fmt.pix.pixelformat = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit;
            break;
            case I3DDMA_COLOR_YV12:
                f->fmt.pix.pixelformat = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YV12;
            break;
            case I3DDMA_COLOR_I420:
                f->fmt.pix.pixelformat = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_I420;
            break;
            case I3DDMA_COLOR_NV10BITS:
                f->fmt.pix.pixelformat = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_10bit;
            break;
            case I3DDMA_COLOR_P010:
                f->fmt.pix.pixelformat = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_P010;
            break;

            f->fmt.pix.width = IDDMA_GetVFEInfoByPort(dev_num, SLR_INPUT_IPH_ACT_WID_PRE);
            f->fmt.pix.height = IDDMA_GetVFEInfoByPort(dev_num, SLR_INPUT_IPV_ACT_LEN_PRE);
            f->fmt.pix.bytesperline = get_vt_pitch(dev_num);
            f->fmt.pix.sizeimage = get_vt_sizeimage(dev_num);
        }
    }
    else
    {
        StructSrcRect output = get_vt_output_size(dev_num);
        f->fmt.pix.pixelformat = get_vt_pixel_format(dev_num);
        f->fmt.pix.width = output.src_wid;
        f->fmt.pix.height = output.src_height;
    }

	return 0;
}

int vt_v4l2_ioctl_try_fmt_vid_cap(struct file *file, void *fh, struct v4l2_format *f)
{
    if(!f)
    {
        rtd_pr_vt_err("func:%s [error] pointer is null\r\n",__FUNCTION__);
        return -EFAULT;
    }


    return 0;
}

/* This api need frame_interval to calculate framerate with formula: framerate=1/frame_interval
 * enumerate the given pixelformat and frame width and height, driver returns frame interval
 * Actual vt real fps range 30-120, so frame_interval range like below
 * Minimum frame interal: 1/120
 * Maximum frame interal: 1/30
 * Frame interval step size: assume stepwise 10fps
*/
int vt_v4l2_ioctl_frameintervals(struct file *file, void *fh, struct v4l2_frmivalenum *fival)
{
    struct vt_v4l2_device* vt_dev = (struct vt_v4l2_device*)video_drvdata(file);
    unsigned char dev_num = VT_IDDMA_DEV_NUM;

    if(vt_dev)
        dev_num = vt_dev->vt_dev_num;
    
    if(!fival)
    {
        rtd_pr_vt_err("func:%s [error] pointer is null\n",__FUNCTION__);
        return -EINVAL;
    }

    rtd_pr_vt_notice("func:%s,index %d, pixel fmt %d, frame widthxheight %dx%d\n",\
        __FUNCTION__, fival->index, fival->pixel_format, fival->width, fival->height);

    if(dev_num < VT_IDDMA_DEV_NUM)
    {
        return -EOPNOTSUPP;
    }

    fival->type = V4L2_FRMIVAL_TYPE_STEPWISE; /* Frame interval type the device supports. STEPWISE */

    if(fival->type == V4L2_FRMIVAL_TYPE_STEPWISE)
    {
        fival->stepwise.min.numerator = 1;
        fival->stepwise.min.denominator = 120;

        fival->stepwise.max.numerator = 1;
        fival->stepwise.max.denominator = 30;

        fival->stepwise.step.numerator = 1;
        fival->stepwise.step.denominator = 10;
    }else if(fival->type == V4L2_FRMIVAL_TYPE_DISCRETE){
        fival->discrete.numerator = 1;
        fival->discrete.denominator = get_vt_capture_framerate(get_vt_capture_source_type(dev_num));
    }else if(fival->type == V4L2_FRMIVAL_TYPE_CONTINUOUS){
        fival->stepwise.min.numerator = 1;
        fival->stepwise.min.denominator = 120;

        fival->stepwise.max.numerator = 1;
        fival->stepwise.max.denominator = 30;
    }

    return 0;
}

