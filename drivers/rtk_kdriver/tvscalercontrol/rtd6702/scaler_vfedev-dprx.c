#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <mach/rtk_platform.h>
#ifdef CONFIG_COMPAT
#include <linux/compat.h>
#define to_user_ptr(x)          compat_ptr((unsigned int) x)
#else
#define to_user_ptr(x)          ((void* __user)(x)) // convert 32 bit value to user pointer
#endif

#include <ioctrl/scaler/vfe_cmd_id.h>
#include <rtk_kdriver/dprx/dprx_vfe_api.h>
#include "scaler_vfedev-dprx.h"


static long __copy_attr_to_user(VFE_DPRX_ATTR* p_usr_attr, VFE_DPRX_ATTR* p_kernel_attr)
{
    if (p_usr_attr==NULL || p_kernel_attr==NULL ||
        copy_to_user((void __user *)p_usr_attr, (void *)p_kernel_attr, sizeof(VFE_DPRX_ATTR)))
    {
        DPRX_VFE_ERR("copy dprx attribute to user space failed\n");
        return -EFAULT;
    }
    return 0;
}

static long __copy_attr_from_user(VFE_DPRX_ATTR* p_kernel_attr, VFE_DPRX_ATTR* p_usr_attr)
{
    if (p_usr_attr==NULL || p_kernel_attr==NULL ||
        copy_from_user((void *)p_kernel_attr, (void __user *)p_usr_attr, sizeof(VFE_DPRX_ATTR)))
    {
        DPRX_VFE_ERR("copy dprx attribute from user space failed\n");
        return -EFAULT;
    }

    return 0;
}


/*---------------------------------------------------
 * Func : _ioctl_ops_dprx_vfe
 *
 * Desc : ioctl function of DPRX VFE
 *
 * Para : cmd   :
 *        arg   :
 *
 * Retn : 0 : success, <0 : failed,
 *--------------------------------------------------*/
long _ioctl_ops_dprx_vfe(unsigned int cmd,  unsigned long arg)
{
    VFE_DPRX_ATTR attr;
    int ret = DPRX_DRV_ARG_ERR;
    unsigned char bval;

    DPRX_VFE_DBG("%s (cmd=%08x, arg=%lx)\n", __FUNCTION__, cmd, arg);

    switch(cmd)
    {
    case VFE_IOC_DPRX_VFE_CTRL:

        switch(GET_VFE_DPRX_CTRL_CMD(arg))
        {
        case DPRX_VFE_CTRL_INIT:       ret = vfe_dprx_drv_init();   break;
        case DPRX_VFE_CTRL_UNINIT:     ret = vfe_dprx_drv_uninit(); break;
        case DPRX_VFE_CTRL_OPEN:       ret = vfe_dprx_drv_open();   break;
        case DPRX_VFE_CTRL_CLOSE:      ret = vfe_dprx_drv_close();  break;
        case DPRX_VFE_CTRL_CONNECT:    ret = vfe_dprx_drv_connect(GET_VFE_DPRX_CTRL_PORT(arg)); break;
        case DPRX_VFE_CTRL_DISCONNECT: ret = vfe_dprx_drv_disconnect(GET_VFE_DPRX_CTRL_PORT(arg)); break;
        default:
            DPRX_VFE_ERR("%s, do vfe_ctrl failed, unknown ctrl command (%lx)\n", __FUNCTION__, arg);
            return -ENOTTY;
        }

        break;

    case VFE_IOC_DPRX_VFE_SET_ATTR:
        // TODO:
        return -ENOTTY;

    case VFE_IOC_DPRX_VFE_GET_ATTR:

        if (__copy_attr_from_user(&attr, (VFE_DPRX_ATTR *)arg)!=0)
        {
            DPRX_VFE_ERR("%s, get vfe attr failed, copy attr from user space failed\n", __FUNCTION__);
            break;
        }

        if (IS_ATTR_READABLE(attr.attr)==0)
        {
            DPRX_VFE_ERR("%s, get vfe attr failed, attr(%x) is not readable\n", __FUNCTION__, attr.attr);
            break;
        }

        switch(GET_ATTR_ID(attr.attr))
        {
        case DPRX_VFE_ATTR_RX_PORT_CNT:

            if (attr.data_array==0 && vfe_dprx_drv_get_rx_port_cnt(&bval)==DPRX_DRV_NO_ERR)
            {
                attr.data = bval;
                ret = DPRX_DRV_NO_ERR;
            }
            break;
        case DPRX_VFE_ATTR_RX_FAST_SWITCH_SUPPORT:
            attr->data = 0;
            ret=DPRX_DRV_NO_ERR;
            break;
        case DPRX_VFE_ATTR_PORT_CAPABILITY:

            if (attr.data_array==1 && attr.data_len==sizeof(KADP_VFE_DPRX_PORT_CAP_T) && attr.p_data)
            {
                DP_PORT_CAPABILITY dp_cap;

                if (vfe_dprx_drv_get_rx_port_capability(attr.port, &dp_cap)==DPRX_DRV_NO_ERR)
                {
                    KADP_VFE_DPRX_PORT_CAP_T cap;
                    memset(&cap, 0, sizeof(KADP_VFE_DPRX_PORT_CAP_T));

                    switch(dp_cap.type)
                    {
                    case DP_TYPE_DP:             cap.type = KADP_VFE_DPRX_PORT_TYPE_DP; break;
                    case DP_TYPE_PSEUDO_HDMI_DP: cap.type = KADP_VFE_DPRX_PORT_TYPE_DP; break;
                    case DP_TYPE_PSEUDO_DP:      cap.type = KADP_VFE_DPRX_PORT_TYPE_DP; break;
                    case DP_TYPE_mDP:            cap.type = KADP_VFE_DPRX_PORT_TYPE_MDP; break;
                    case DP_TYPE_USB_TYPE_C:     cap.type = KADP_VFE_DPRX_PORT_TYPE_TYPEC; break;
                    default:
                    case DP_TYPE_NONE:           cap.type = KADP_VFE_DPRX_PORT_TYPE_NONE;
                    }

                    switch(dp_cap.lane_cnt)
                    {
                    case DP_LANE_MODE_1_LANE: cap.max_lane_cnt = KADP_VFE_DPRX_PORT_LANE_CNT_1; break;
                    case DP_LANE_MODE_2_LANE: cap.max_lane_cnt = KADP_VFE_DPRX_PORT_LANE_CNT_2; break;
                    case DP_LANE_MODE_4_LANE: cap.max_lane_cnt = KADP_VFE_DPRX_PORT_LANE_CNT_4; break;
                    default:                  cap.max_lane_cnt = KADP_VFE_DPRX_PORT_LANE_CNT_NONE; break;
                    }

                    // Link Rate Mask
                    if (dp_cap.link_rate_mask & DP_LINK_RATE_RBR_MASK)
                        cap.link_rate_mask |= KADP_VFE_DPRX_PORT_LINK_RATE_RBR_MASK;

                    if (dp_cap.link_rate_mask & DP_LINK_RATE_HBR1_MASK)
                        cap.link_rate_mask |= KADP_VFE_DPRX_PORT_LINK_RATE_HBR1_MASK;

                    if (dp_cap.link_rate_mask & DP_LINK_RATE_HBR2_MASK)
                        cap.link_rate_mask |= KADP_VFE_DPRX_PORT_LINK_RATE_HBR2_MASK;

                    if (dp_cap.link_rate_mask & DP_LINK_RATE_HBR3_MASK)
                        cap.link_rate_mask |= KADP_VFE_DPRX_PORT_LINK_RATE_HBR3_MASK;

                    if (dp_cap.link_rate_mask & DP_LINK_RATE_UHBR10_MASK)
                        cap.link_rate_mask |= KADP_VFE_DPRX_PORT_LINK_RATE_UHBR10_MASK;

                    if (dp_cap.link_rate_mask & DP_LINK_RATE_UHBR13_MASK)
                        cap.link_rate_mask |= KADP_VFE_DPRX_PORT_LINK_RATE_UHBR13P5_MASK;

                    if (dp_cap.link_rate_mask & DP_LINK_RATE_UHBR20_MASK)
                        cap.link_rate_mask |= KADP_VFE_DPRX_PORT_LINK_RATE_UHBR20_MASK;

                    // Capability
                    if (dp_cap.capability.fec_support)
                        cap.capablity_mask |= KADP_VFE_DPRX_PORT_CAP_FEC_MASK;

                    if (dp_cap.capability.dsc_support)
                        cap.capablity_mask |= KADP_VFE_DPRX_PORT_CAP_DSC_1P3;

                    // copy data
                    if (copy_to_user( (void __user *)to_user_ptr(attr.p_data), (void *)&cap, attr.data_len))
                    {
                        DPRX_VFE_ERR("%s, get port capability failed, copy capability data to user space failed\n", __FUNCTION__);
                        return -EFAULT;
                    }

                    ret = DPRX_DRV_NO_ERR;
                }
            }
            break;

        case DPRX_VFE_ATTR_PORT_LINK_STATUS:
            if (attr.data_array==1 && attr.data_len==sizeof(KADP_VFE_DPRX_PORT_LINK_STATUS_T) && attr.p_data)
            {
                DP_LINK_STATUS_T dp_link_status;

                if (vfe_dprx_drv_get_link_status(attr.port, &dp_link_status)==DPRX_DRV_NO_ERR)
                {
                    KADP_VFE_DPRX_PORT_LINK_STATUS_T link_status;
                    int i;

                    memset(&link_status, 0, sizeof(link_status));

                    // lane mode
                    switch(dp_link_status.lane_mode)
                    {
                    case DP_LANE_MODE_1_LANE: link_status.lane_mode = KADP_VFE_DPRX_PORT_LANE_CNT_1; break;
                    case DP_LANE_MODE_2_LANE: link_status.lane_mode = KADP_VFE_DPRX_PORT_LANE_CNT_2; break;
                    case DP_LANE_MODE_4_LANE: link_status.lane_mode = KADP_VFE_DPRX_PORT_LANE_CNT_4; break;
                    default:                  link_status.lane_mode = KADP_VFE_DPRX_PORT_LANE_CNT_NONE; break;
                    }

                    // link rate
                    switch(dp_link_status.link_rate)
                    {
                    case DP_LINK_RATE_RBR_1p62G:    link_status.link_rate = KADP_VFE_DPRX_PORT_LINK_RATE_RBR; break;
                    case DP_LINK_RATE_HBR1_2p7G:    link_status.link_rate = KADP_VFE_DPRX_PORT_LINK_RATE_HBR1; break;
                    case DP_LINK_RATE_HBR2_5p4G:    link_status.link_rate = KADP_VFE_DPRX_PORT_LINK_RATE_HBR2; break;
                    case DP_LINK_RATE_HBR3_8p1G:    link_status.link_rate = KADP_VFE_DPRX_PORT_LINK_RATE_HBR3; break;
                    case DP_LINK_RATE_UHBR10_10G:   link_status.link_rate = KADP_VFE_DPRX_PORT_LINK_RATE_UHBR10; break;
                    case DP_LINK_RATE_UHBR13_13p5G: link_status.link_rate = KADP_VFE_DPRX_PORT_LINK_RATE_UHBR13P5; break;
                    case DP_LINK_RATE_UHBR20_20G:   link_status.link_rate = KADP_VFE_DPRX_PORT_LINK_RATE_UHBR20; break;
                    default:                        link_status.link_rate = KADP_VFE_DPRX_PORT_LINK_RATE_RBR; break;
                    }

                    // channel align
                    link_status.ch_aligned = dp_link_status.channel_align;

                    // lane status
                    for (i=0; i<4; i++)
                    {
                        link_status.lane_status[i].valid = dp_link_status.channel_status[i].valid;
                        link_status.lane_status[i].lock  = dp_link_status.channel_status[i].lock;
                        link_status.lane_status[i].error_cnt = dp_link_status.channel_status[i].error_cnt;
                    }

                    // copy data
                    if (copy_to_user( (void __user *)to_user_ptr(attr.p_data), (void *) &link_status, attr.data_len))
                    {
                        DPRX_VFE_ERR("%s, get port link status failed, copy link status to user space failed\n", __FUNCTION__);
                        return -EFAULT;
                    }
                    ret = DPRX_DRV_NO_ERR;
                }
            }
            break;

        case DPRX_VFE_ATTR_PORT_STREAM_TYPE:

            if (attr.data_array==0)
            {
                DP_STREAM_TYPE_E stream_type;

                if (vfe_dprx_drv_get_stream_type(attr.port, &stream_type)==DPRX_DRV_NO_ERR)
                {
                    switch(stream_type)
                    {
                    case DPRX_STREAM_TYPE_UNKNOWN:     attr.data = KADP_VFE_DPRX_STREAM_TYPE_UNKNOWN; break;
                    case DPRX_STREAM_TYPE_AUDIO_ONLY:  attr.data = KADP_VFE_DPRX_STREAM_TYPE_AUDIO_ONLY; break;
                    case DPRX_STREAM_TYPE_VIDEO_ONLY:  attr.data = KADP_VFE_DPRX_STREAM_TYPE_VIDEO_ONLY; break;
                    case DPRX_STREAM_TYPE_AUDIO_VIDEO: attr.data = KADP_VFE_DPRX_STREAM_TYPE_AUDIO_VIDEO; break;
                    default:                           attr.data = KADP_VFE_DPRX_STREAM_TYPE_UNKNOWN; break;
                    }
                    ret = DPRX_DRV_NO_ERR;
                }
            }
            break;

        default:
            DPRX_VFE_ERR("%s, get vfe attr failed, unknown attr(%x)\n", __FUNCTION__, attr.attr);
            return -ENOTTY;
        }

        if (attr.data_array==0 && ret == DPRX_DRV_NO_ERR)
            return __copy_attr_to_user((VFE_DPRX_ATTR*)arg, &attr);

        break;

    default:
        DPRX_VFE_ERR("%s, do vfe ioctl failed, unknown command (%x)\n", __FUNCTION__, cmd);
        return -ENOTTY;
    }

    return (ret == DPRX_DRV_NO_ERR) ? 0 : -EFAULT;
}


/*---------------------------------------------------
 * Func : _ioctl_ops_dprx_hpd_det
 *
 * Desc : ioctl function of DPRX VFE
 *
 * Para : cmd   :
 *        arg   :
 *
 * Retn : 0 : success, <0 : failed,
 *--------------------------------------------------*/
long _ioctl_ops_dprx_hpd_det(unsigned int cmd,  unsigned long arg)
{
    VFE_DPRX_ATTR attr;
    int ret = DPRX_DRV_ARG_ERR;
    unsigned char bval;

    DPRX_VFE_DBG("%s (cmd=%08x, arg=%lx)\n", __FUNCTION__, cmd, arg);

    switch(cmd)
    {
    case VFE_IOC_DPRX_HPD_DET_CTRL:

        switch(GET_VFE_DPRX_CTRL_CMD(arg))
        {
        case DPRX_HPD_CTRL_ENABLE_HPD:  ret = vfe_dprx_drv_enable_hpd(GET_VFE_DPRX_CTRL_VALUE(arg) ? 1 : 0); break;
        case DPRX_HPD_CTRL_SET_HPD:     ret = vfe_dprx_drv_set_hpd(GET_VFE_DPRX_CTRL_PORT(arg), GET_VFE_DPRX_CTRL_VALUE(arg) ? 1 : 0); break;
        case DPRX_HPD_CTRL_HPD_TOGGLE:  ret = vfe_dprx_drv_hpd_toggle(GET_VFE_DPRX_CTRL_PORT(arg)); break;
        default:
            DPRX_VFE_ERR("%s, do hpd_det_ctrl failed, unknown ctrl command (%lx)\n", __FUNCTION__, arg);
            return -ENOTTY;
        }
        break;

    case VFE_IOC_DPRX_HPD_DET_SET_ATTR:

        if (__copy_attr_from_user(&attr, (VFE_DPRX_ATTR *)arg)!=0)
        {
            DPRX_VFE_ERR("%s, set hpd_det_attr failed, copy attr from user space failed\n", __FUNCTION__);
            break;
        }

        if (IS_ATTR_WRITABLE(attr.attr)==0)
        {
            DPRX_VFE_ERR("%s, set hpd_det_attr failed, attr(%x) is not writable\n", __FUNCTION__, attr.attr);
            break;
        }

        // set attribute
        switch(GET_ATTR_ID(attr.attr))
        {
        case DPRX_HPD_ATTR_HPD_LOW_DELAY:

            if (attr.data_array==0 && vfe_dprx_drv_set_hpd_low_delay(attr.data)==DPRX_DRV_NO_ERR)
                ret = DPRX_DRV_NO_ERR;
            break;

        default:
            DPRX_VFE_ERR("%s, set vfe attr failed, unknown attr(%x)\n", __FUNCTION__, attr.attr);
            return -ENOTTY;
        }
        break;

    case VFE_IOC_DPRX_HPD_DET_GET_ATTR:

        if (__copy_attr_from_user(&attr, (VFE_DPRX_ATTR *)arg)!=0)
        {
            DPRX_VFE_ERR("%s, get hpd_det_attr failed, copy attr from user space failed\n", __FUNCTION__);
            break;
        }

        if (IS_ATTR_READABLE(attr.attr)==0)
        {
            DPRX_VFE_ERR("%s, get hpd_det_attr failed, attr(%x) is not readable\n", __FUNCTION__, attr.attr);
            break;
        }

        // get attribute
        switch(GET_ATTR_ID(attr.attr))
        {
        case DPRX_HPD_ATTR_HPD_LOW_DELAY:

            if (attr.data_array==0)
            {
                attr.data = 2;   // humm... output hpd low delay here
                ret = DPRX_DRV_NO_ERR;
            }
            break;

        case DPRX_HPD_ATTR_DPRX_CONNECT_STATE:

            if (attr.data_array==0 && vfe_dprx_drv_get_connection_state(attr.port, &bval)==DPRX_DRV_NO_ERR)
            {
                attr.data = bval;
                ret = DPRX_DRV_NO_ERR;
            }
            break;

        default:
            DPRX_VFE_ERR("%s, get hpd_det_attr failed, unknown attr(%x)\n", __FUNCTION__, attr.attr);
            return -ENOTTY;
        }

        if (attr.data_array==0 && ret == DPRX_DRV_NO_ERR)
            return __copy_attr_to_user((VFE_DPRX_ATTR*)arg, &attr);

        break;

    default:
        DPRX_VFE_ERR("%s, do ioctl failed, unknown command (%x)\n", __FUNCTION__, cmd);
    }

    return (ret == DPRX_DRV_NO_ERR) ? 0 : -EFAULT;
}

/*---------------------------------------------------
 * Func : _ioctl_ops_dprx_edid
 *
 * Desc : ioctl function of DPRX VFE
 *
 * Para : cmd   :
 *        arg   :
 *
 * Retn : 0 : success, <0 : failed,
 *--------------------------------------------------*/
long _ioctl_ops_dprx_edid(unsigned int cmd,  unsigned long arg)
{
    VFE_DPRX_ATTR attr;
    int ret = DPRX_DRV_ARG_ERR;
    unsigned char* p_edid;

    DPRX_VFE_DBG("%s (cmd=%08x, arg=%lx)\n", __FUNCTION__, cmd, arg);

    switch(cmd)
    {
    case VFE_IOC_DPRX_EDID_CTRL:
        // TODO:
        DPRX_VFE_ERR("%s, do edid_ioctl failed, unknown command (%lx)\n", __FUNCTION__, arg);
        return -ENOTTY;

    case VFE_IOC_DPRX_EDID_SET_ATTR:

        if (__copy_attr_from_user(&attr, (VFE_DPRX_ATTR *)arg)!=0)
        {
            DPRX_VFE_ERR("%s, set edid_attr failed, copy attr from user space failed\n", __FUNCTION__);
            break;
        }

        if (IS_ATTR_WRITABLE(attr.attr)==0)
        {
            DPRX_VFE_ERR("%s, set edid_attr failed, attr(%x) is not writable\n", __FUNCTION__, attr.attr);
            break;
        }

        // set attribute
        switch(GET_ATTR_ID(attr.attr))
        {
        case DPRX_EDID_ATTR_DPRX_EDID:

            if (IS_VALID_DPRX_EDID_ATTR_DATA(attr))
            {
                p_edid = kmalloc(attr.data_len, GFP_KERNEL);

                if (p_edid==NULL)
                {
                    DPRX_VFE_ERR("%s, set dprx edid failed, alloc edid mem failed\n", __FUNCTION__);
                    return -EFAULT;
                }

                if (copy_from_user((void *)p_edid, (const void __user *)to_user_ptr(attr.p_data), attr.data_len))
                {
                    DPRX_VFE_ERR("%s, set dprx edid failed, copy edid from user space failed\n", __FUNCTION__);
                    return -EFAULT;
                }

                ret = vfe_dprx_drv_write_edid(attr.port, p_edid, attr.data_len);
                kfree(p_edid);
            }
            break;

        default:
            DPRX_VFE_ERR("%s, set edid_attr failed, unknown attr(%x)\n", __FUNCTION__, attr.attr);
            return -ENOTTY;
        }

        break;

    case VFE_IOC_DPRX_EDID_GET_ATTR:

        if (__copy_attr_from_user(&attr, (VFE_DPRX_ATTR *)arg)!=0)
        {
            DPRX_VFE_ERR("%s, get edid_attr failed, copy attr from user space failed\n", __FUNCTION__);
            break;
        }

        if (IS_ATTR_READABLE(attr.attr)==0)
        {
            DPRX_VFE_ERR("%s, get edid_attr failed, attr(%x) is not readable\n", __FUNCTION__, attr.attr);
            break;
        }

        // get attribute
        switch(GET_ATTR_ID(attr.attr))
        {
        case DPRX_EDID_ATTR_DPRX_EDID:

            if (IS_VALID_DPRX_EDID_ATTR_DATA(attr))
            {
                p_edid = kmalloc(attr.data_len, GFP_KERNEL);

                if (p_edid==NULL)
                {
                    DPRX_VFE_ERR("%s, get dprx edid failed, alloc edid mem failed\n", __FUNCTION__);
                    return -EFAULT;
                }

                ret = vfe_dprx_drv_read_edid(attr.port, p_edid, attr.data_len);

                if (ret==DPRX_DRV_NO_ERR && copy_to_user(to_user_ptr(attr.p_data), p_edid, attr.data_len)!=0)
                {
                    DPRX_VFE_ERR("%s, get dprx edid failed, copy edid to user space failed\n", __FUNCTION__);
                    ret = DPRX_DRV_HW_ERR;
                }

                kfree(p_edid);
            }
            break;

        default:
            DPRX_VFE_ERR("%s, get edid_attr failed, unknown attr(%x)\n", __FUNCTION__, attr.attr);
            return -ENOTTY;
        }

        if (attr.data_array==0 && ret == DPRX_DRV_NO_ERR)
            return __copy_attr_to_user((VFE_DPRX_ATTR*)arg, &attr);
        break;

    default:
        DPRX_VFE_ERR("%s, do ioctl failed, unknown command (%x)\n", __FUNCTION__, cmd);
        return -ENOTTY;
    }

    return (ret == DPRX_DRV_NO_ERR) ? 0 : -EFAULT;
}



/*---------------------------------------------------
 * Func : _ioctl_ops_dprx_hdcp
 *
 * Desc : ioctl ops of DPRX HDCP
 *
 * Para : cmd   :
 *        arg   :
 *
 * Retn : 0 : success, <0 : failed,
 *--------------------------------------------------*/
long _ioctl_ops_dprx_hdcp(unsigned int cmd,  unsigned long arg)
{
    VFE_DPRX_ATTR attr;
    int ret = DPRX_DRV_ARG_ERR;
    DPRX_HDCP_KEY_T* p_hdcp_key = NULL;

    DPRX_VFE_ERR("%s (cmd=%08x, arg=%lx)\n", __FUNCTION__, cmd, arg);

    switch(cmd)
    {
    case VFE_IOC_DPRX_HDCP_CTRL:

        switch(GET_VFE_DPRX_CTRL_CMD(arg))
        {
        case DPRX_HDCP_CTRL_FORCE_REAUTH:
            // FIXME : trigger hdcp reauth here
            DPRX_VFE_WARN("%s, force HDCP reauth is not implemented yet, do nothing\n", __FUNCTION__);
            return 0;

        default:
            DPRX_VFE_ERR("%s, do hdcp_ctrl failed, unknown command (%lx)\n", __FUNCTION__, arg);
        }

        return -ENOTTY;

    case VFE_IOC_DPRX_HDCP_SET_ATTR:

        if (__copy_attr_from_user(&attr, (VFE_DPRX_ATTR *)arg)!=0)
        {
            DPRX_VFE_ERR("%s, set hdcp_attr failed, copy attr from user space failed\n", __FUNCTION__);
            break;
        }

        if (IS_ATTR_WRITABLE(attr.attr)==0)
        {
            DPRX_VFE_ERR("%s, set hdcp_attr failed, attr(%x) is not writable\n", __FUNCTION__, attr.attr);
            break;
        }

        // set attribute
        switch(GET_ATTR_ID(attr.attr))
        {
        case DPRX_HDCP_ATTR_HDCP2_ENABLE:

            if (attr.data_array)
            {
                DPRX_VFE_ERR("%s, set HDCP2 ENABLE attr failed, attr data sould not be an array\n", __FUNCTION__);
                return -EFAULT;
            }

            // DOTO: enable hdcp2 over here
            DPRX_VFE_WARN("%s, HDCP2 ENABLE is not implemented yet, do nothing....\n", __FUNCTION__);
            return 0;

        case DPRX_HDCP_ATTR_HDCP1x_KEY:

            if (attr.data_array==1 && attr.data_len==HDCP1X_KEY_SIZE && attr.p_data)
            {
                p_hdcp_key = kmalloc(sizeof(DPRX_HDCP_KEY_T), GFP_KERNEL);

                if (p_hdcp_key==NULL)
                {
                    DPRX_VFE_ERR("%s, set HDCP1x key failed, alloc mem failed\n", __FUNCTION__);
                    return -EFAULT;
                }

                p_hdcp_key->version = DP_HDCP_VER_HDCP13;

                if (copy_from_user((void *)p_hdcp_key->hdcp1x_key, (const void __user *)to_user_ptr(attr.p_data), attr.data_len))
                {
                    DPRX_VFE_ERR("%s, set HDCP1x key failed, copy key data from user space failed\n", __FUNCTION__);
                    kfree(p_hdcp_key);
                    return -EFAULT;
                }

                ret = vfe_dprx_drv_write_hdcp_key(p_hdcp_key, 1);
                kfree(p_hdcp_key);
            }
            else
            {
                DPRX_VFE_ERR("%s, set HDCP1x key attr failed, incorrect attr data\n", __FUNCTION__);
            }
            break;

        case DPRX_HDCP_ATTR_HDCP2x_KEY:

            if (attr.data_array==1 && attr.data_len==HDCP2X_KEY_SIZE && attr.p_data)
            {
                p_hdcp_key = kmalloc(sizeof(DPRX_HDCP_KEY_T), GFP_KERNEL);

                if (p_hdcp_key==NULL)
                {
                    DPRX_VFE_ERR("%s, set HDCP2x key failed, alloc mem failed\n", __FUNCTION__);
                    return -EFAULT;
                }

                p_hdcp_key->version = DP_HDCP_VER_HDCP22;

                if (copy_from_user((void *)p_hdcp_key->hdcp2x_key, (const void __user *)to_user_ptr(attr.p_data), attr.data_len))
                {
                    DPRX_VFE_ERR("%s, set HDCP2x key failed, copy key data from user space failed\n", __FUNCTION__);
                    kfree(p_hdcp_key);
                    return -EFAULT;
                }

                ret = vfe_dprx_drv_write_hdcp_key(p_hdcp_key, 1);
                kfree(p_hdcp_key);
            }
            else
            {
                DPRX_VFE_ERR("%s, set HDCP2x key failed, incorrect attr data\n", __FUNCTION__);
            }
            break;

        default:
            DPRX_VFE_ERR("%s, set hdcp_attr failed, unknown attr(%x)\n", __FUNCTION__, attr.attr);
            return -EFAULT;
        }

        break;

    case VFE_IOC_DPRX_HDCP_GET_ATTR:

        if (__copy_attr_from_user(&attr, (VFE_DPRX_ATTR *)arg)!=0)
        {
            DPRX_VFE_ERR("%s, get hdcp_attr failed, copy attr from user space failed\n", __FUNCTION__);
            break;
        }

        if (IS_ATTR_READABLE(attr.attr)==0)
        {
            DPRX_VFE_ERR("%s, get hdcp_attr failed, attr(%x) is not readable\n", __FUNCTION__, attr.attr);
            break;
        }

        switch(GET_ATTR_ID(attr.attr))
        {
        case DPRX_HDCP_ATTR_CAPABILITY:

            if (attr.data_array==0)
            {
                unsigned int get_hdcp_cap=0;
                unsigned int ret_hdcp_cap=0;

                if (vfe_dprx_drv_get_hdcp_capability(&get_hdcp_cap)==DPRX_DRV_NO_ERR)
                {
                    if(get_hdcp_cap & DP_HDCP_CAPBILITY_HDCP13)
                       ret_hdcp_cap |= VFE_DP_HDCP_CAPBILITY_HDCP13;

                    if(get_hdcp_cap & DP_HDCP_CAPBILITY_HDCP22)
                       ret_hdcp_cap |= VFE_DP_HDCP_CAPBILITY_HDCP22;

                    if(get_hdcp_cap & DP_HDCP_CAPBILITY_HDCP23)
                       ret_hdcp_cap |= VFE_DP_HDCP_CAPBILITY_HDCP23;

                    if(get_hdcp_cap & DP_HDCP_CAPBILITY_HDCP_REPEATER)
                       ret_hdcp_cap |= VFE_DP_HDCP_CAPBILITY_HDCP_REPEATER;

                    attr.data = ret_hdcp_cap;
                    ret = DPRX_DRV_NO_ERR;
                }
            }
            break;

        case DPRX_HDCP_ATTR_DPRX_HDCP_STATUS:

            if (attr.data_array==1 && attr.data_len==sizeof(VFE_DPRX_HDCP_STATUS_T) && attr.p_data)
            {
                DPRX_HDCP_STATUS_T get_hdcp_status;
                VFE_DPRX_HDCP_STATUS_T ret_hdcp_status;
                if (vfe_dprx_drv_get_hdcp_status(attr.port, &get_hdcp_status)==DPRX_DRV_NO_ERR)
                {
                    memset(&ret_hdcp_status, 0, sizeof(VFE_DPRX_HDCP_STATUS_T));
                    ret_hdcp_status.mode = (VFE_DP_HDCP_MODE)get_hdcp_status.mode;
                    ret_hdcp_status.state = (VFE_DP_HDCP_STATE)get_hdcp_status.state;
                    memcpy(&(ret_hdcp_status.hdcp2x.receiverid),&(get_hdcp_status.hdcp2x.receiverid),VFE_HDCP2X_RECEIVER_ID_LEN);
                    if (copy_to_user( (void __user *)to_user_ptr(attr.p_data), (void *) &ret_hdcp_status, attr.data_len))
                    {
                        DPRX_VFE_ERR("%s, get hdcp status failed, copy hdcp status to user space failed\n", __FUNCTION__);
                        return -EFAULT;
                    }
                    ret = DPRX_DRV_NO_ERR;
                }
            }
            break;

        case DPRX_HDCP_ATTR_HDCP2_ENABLE:

            if (attr.data_array==0)
            {
                attr.data = 1;
                ret = DPRX_DRV_NO_ERR;
            }
            else
            {
                DPRX_VFE_ERR("%s, get hdcp2 enable attr failed, invalid attr data\n", __FUNCTION__);
            }
            break;

        default:
            DPRX_VFE_ERR("%s, get hdcp_attr failed, unknown attr(%x)\n", __FUNCTION__, attr.attr);
            return -ENOTTY;
        }

        if (attr.data_array==0 && ret == DPRX_DRV_NO_ERR)
            return __copy_attr_to_user((VFE_DPRX_ATTR*)arg, &attr);

        break;

    default:
        DPRX_VFE_ERR("%s, do ioctl failed, unknown command (%x)\n", __FUNCTION__, cmd);
        return -ENOTTY;
    }

    return (ret==DPRX_DRV_NO_ERR) ? 0 : -EFAULT;
}


/*---------------------------------------------------
 * Func : _ioctl_ops_dprx_video
 *
 * Desc : ioctl function of DPRX VFE
 *
 * Para : cmd   :
 *        arg   :
 *
 * Retn : 0 : success, <0 : failed,
 *--------------------------------------------------*/
long _ioctl_ops_dprx_video(unsigned int cmd,  unsigned long arg)
{
    VFE_DPRX_ATTR attr;
    int ret = DPRX_DRV_ARG_ERR;
    DPRX_VFE_ERR("%s (cmd=%08x, arg=%lx)\n", __FUNCTION__, cmd, arg);

    switch(cmd)
    {
    case VFE_IOC_DPRX_VIDEO_CTRL:
        DPRX_VFE_WARN("%s, do video ctrl failed, ctrl (%lx) is not implemented yet\n", __FUNCTION__, arg);
        break;

    case VFE_IOC_DPRX_VIDEO_GET_ATTR:

        if (__copy_attr_from_user(&attr, (VFE_DPRX_ATTR *)arg)!=0)
        {
            DPRX_VFE_ERR("%s, get video_attr failed, copy attr from user space failed\n", __FUNCTION__);
            break;
        }

        if (IS_ATTR_READABLE(attr.attr)==0)
        {
            DPRX_VFE_ERR("%s, get video_attr failed, attr(%x) is not readable\n", __FUNCTION__, attr.attr);
            break;
        }

        switch(GET_ATTR_ID(attr.attr))
        {
        case DPRX_VIDEO_ATTR_VIDEO_TIMING:

            if (attr.data_array==1 && attr.data_len==sizeof(KADP_VFE_DPRX_TIMING_INFO_T) && attr.p_data)
            {
                DPRX_TIMING_INFO_T get_timing_info;
                KADP_VFE_DPRX_TIMING_INFO_T ret_timing_info;

                if (vfe_dprx_drv_get_port_timing_info(attr.port, &get_timing_info)==DPRX_DRV_NO_ERR)
                {
                    memset(&ret_timing_info, 0, sizeof(KADP_VFE_DPRX_TIMING_INFO_T));
                    ret_timing_info.color_space = (KADP_VFE_DPRX_COLOR_SPACE_E)get_timing_info.color_space;
                    ret_timing_info.color_depth = (KADP_VFE_DPRX_COLOR_DEPTH_E)get_timing_info.color_depth;
                    ret_timing_info.ptg_mode = (KADP_VFE_DPRX_PTG_REGEN_E)get_timing_info.ptg_mode;
                    ret_timing_info.htotal = get_timing_info.htotal;
                    ret_timing_info.hporch = get_timing_info.hporch;
                    ret_timing_info.hstart = get_timing_info.hstart;
                    ret_timing_info.hsync = get_timing_info.hsync;
                    ret_timing_info.hact = get_timing_info.hact;
                    ret_timing_info.vtotal = get_timing_info.vtotal;
                    ret_timing_info.vporch = get_timing_info.vporch;
                    ret_timing_info.vstart = get_timing_info.vstart;
                    ret_timing_info.vsync = get_timing_info.vsync;
                    ret_timing_info.vact = get_timing_info.vact;
                    ret_timing_info.hfreq_hz = get_timing_info.hfreq_hz;
                    ret_timing_info.vfreq_hz_x100 = get_timing_info.vfreq_hz_x100;
                    ret_timing_info.is_interlace = get_timing_info.is_interlace;
                    ret_timing_info.is_dsc = get_timing_info.is_dsc;
                    ret_timing_info.drr_mode = get_timing_info.drr_mode;
                    ret_timing_info.reserved = get_timing_info.reserved;
                    ret_timing_info.curr_vfreq_hz_x100 = get_timing_info.curr_vfreq_hz_x100;
                    ret_timing_info.pixel_mode  = get_timing_info.pixel_mode;

                    // extra flags
#ifdef KADP_VFE_DPRX_TIMING_OVER_DISP_LIMIT
                    if (get_timing_info.ext_flags & DPRX_TIMING_OVER_DISP_LIMIT)
                        ret_timing_info.ext_flags |= KADP_VFE_DPRX_TIMING_OVER_DISP_LIMIT;
#endif

#ifdef KADP_VFE_DPRX_TIMING_BYPASSABLE
                    if (get_timing_info.ext_flags & DPRX_TIMING_BYPASSABLE)
                        ret_timing_info.ext_flags |= KADP_VFE_DPRX_TIMING_BYPASSABLE;
#endif

                    if (copy_to_user( (void __user *)to_user_ptr(attr.p_data), (void *) &ret_timing_info, attr.data_len))
                    {
                        DPRX_VFE_ERR("%s, get timing info failed, copy timing info to user space failed\n", __FUNCTION__);
                        return -EFAULT;
                    }
                    ret = DPRX_DRV_NO_ERR;
                }
            }
            break;

        default:
            DPRX_VFE_ERR("%s, get video_attr failed, unknown attr(%x)\n", __FUNCTION__, attr.attr);
            return -ENOTTY;
        }

        if (attr.data_array==1 && ret == DPRX_DRV_NO_ERR)
            return __copy_attr_to_user((VFE_DPRX_ATTR*)arg, &attr);

        break;

    case VFE_IOC_DPRX_VIDEO_SET_ATTR:
        DPRX_VFE_WARN("%s, set video attr failed, get attr is not implemented yet\n", __FUNCTION__);
        break;

    default:
        DPRX_VFE_ERR("%s, do ioctl failed, unknown command (%x)\n", __FUNCTION__, cmd);
        return -ENOTTY;
    }

    return (ret==DPRX_DRV_NO_ERR) ? 0 : -EFAULT;
}



/*---------------------------------------------------
 * Func : _ioctl_ops_dprx_audio
 *
 * Desc : ioctl operation of DPRX AUDIO
 *
 * Para : cmd   :
 *        arg   :
 *
 * Retn : 0 : success, <0 : failed,
 *--------------------------------------------------*/
long _ioctl_ops_dprx_audio(unsigned int cmd,  unsigned long arg)
{
    VFE_DPRX_ATTR attr;
    int ret = DPRX_DRV_ARG_ERR;
    DPRX_VFE_ERR("%s (cmd=%08x, arg=%lx)\n", __FUNCTION__, cmd, arg);

    switch(cmd)
    {
    case VFE_IOC_DPRX_AUDIO_CTRL:
        DPRX_VFE_WARN("%s, do audio ctrl failed, ctrl (%lx) is not implemented yet\n", __FUNCTION__, arg);
        break;

    case VFE_IOC_DPRX_AUDIO_GET_ATTR:

        if (__copy_attr_from_user(&attr, (VFE_DPRX_ATTR *)arg)!=0)
        {
            DPRX_VFE_ERR("%s, get audio_attr failed, copy attr from user space failed\n", __FUNCTION__);
            break;
        }

        if (IS_ATTR_READABLE(attr.attr)==0)
        {
            DPRX_VFE_ERR("%s, get audio_attr failed, attr(%x) is not readable\n", __FUNCTION__, attr.attr);
            break;
        }

        switch(GET_ATTR_ID(attr.attr))
        {
        case DPRX_AUDIO_ATTR_AUDIO_STATUS:

            if (attr.data_array==1 && attr.data_len==sizeof(KADP_VFE_DPRX_AUDIO_STATUS_T) && attr.p_data)
            {
                DP_AUDIO_STATUS_T get_audio_status;
                KADP_VFE_DPRX_AUDIO_STATUS_T ret_audio_status;

                if (vfe_dprx_drv_get_audio_status(&get_audio_status)==DPRX_DRV_NO_ERR)
                {
                    memset(&ret_audio_status, 0, sizeof(KADP_VFE_DPRX_AUDIO_STATUS_T));
                    ret_audio_status.coding_type = (KADP_VFE_DPRX_AUDIO_CODING_TYPE)get_audio_status.coding_type;
                    ret_audio_status.ch_num = get_audio_status.ch_num;
                    ret_audio_status.sampling_frequency_khz_x100 = get_audio_status.sampling_frequency_khz_x100;
                    if (copy_to_user( (void __user *)to_user_ptr(attr.p_data), (void *) &ret_audio_status, attr.data_len))
                    {
                        DPRX_VFE_ERR("%s, get audio status failed, copy audio status to user space failed\n", __FUNCTION__);
                        return -EFAULT;
                    }
                    ret = DPRX_DRV_NO_ERR;
                }
            }
            break;

        default:
            DPRX_VFE_ERR("%s, get audio_attr failed, unknown attr(%x)\n", __FUNCTION__, attr.attr);
            return -ENOTTY;
        }

        if (attr.data_array==0 && ret == DPRX_DRV_NO_ERR)
            return __copy_attr_to_user((VFE_DPRX_ATTR*)arg, &attr);

        break;

    case VFE_IOC_DPRX_AUDIO_SET_ATTR:
        DPRX_VFE_WARN("%s, set audio attr failed, set attr is not implemented yet\n", __FUNCTION__);
        break;

    default:
        DPRX_VFE_ERR("%s, do ioctl failed, unknown command (%x)\n", __FUNCTION__, cmd);
        return -ENOTTY;
    }

    return (ret==DPRX_DRV_NO_ERR) ? 0 : -EFAULT;
}

/*---------------------------------------------------
 * Func : _ioctl_ops_dprx_sdp
 *
 * Desc : ioctl operation of DPRX sdp data get
 *
 * Para : cmd   :
 *        arg   :
 *
 * Retn : 0 : success, <0 : failed,
 *--------------------------------------------------*/
static long _ioctl_ops_dprx_sdp(unsigned int cmd,  unsigned long arg)
{
    VFE_DPRX_ATTR attr;
    DP_SDP_DATA_T get_sdp_data;
    KADP_VFE_DPRX_PORT_SDP_DATA_T ret_sdp_data;
    KADP_VFE_DPRX_PORT_SDP_TYPE_E kadp_sdp_type = 0;
    DP_SDP_TYPE vfe_sdp_type;
    int ret = DPRX_DRV_ARG_ERR;
    int i=0;
    DPRX_VFE_DBG("%s TESTO!!!!!!!!!!!!!!!!!!!!!!!!! (cmd=%08x, arg=%lx)\n", __FUNCTION__, cmd, arg);
    if (__copy_attr_from_user(&attr, (VFE_DPRX_ATTR *)arg)!=0)
    {
        DPRX_VFE_ERR("%s, get sdp_attr failed, copy attr from user space failed\n", __FUNCTION__);
        return -EFAULT;
    }
    kadp_sdp_type = GET_ATTR_ID(attr.attr);
    if (IS_ATTR_READABLE(attr.attr)==0)
    {
        DPRX_VFE_ERR("%s, get sdp_attr failed, attr(%x) is not readable\n", __FUNCTION__, attr.attr);
        return -EFAULT;
    }
    switch(kadp_sdp_type)
    {
        case KADP_VFE_DPRX_PORT_SDP_INFOFRAME_VSC:
            vfe_sdp_type = DP_SDP_TYPE_VSC;
            break;
        case KADP_VFE_DPRX_PORT_SDP_INFOFRAME_HDR:
            vfe_sdp_type = DP_SDP_TYPE_HDR;
            break;
        case KADP_VFE_DPRX_PORT_SDP_INFOFRAME_SPD:
            vfe_sdp_type = DP_SDP_TYPE_SPD;
            break;
        case KADP_VFE_DPRX_PORT_SDP_INFOFRAME_DVS:
            vfe_sdp_type = DP_SDP_TYPE_DVS;
            break;
        case KADP_VFE_DPRX_PORT_SDP_INFOFRAME_ADP_SYNC:
            vfe_sdp_type = DP_SDP_TYPE_ADP_SYNC;
            break;
        case KADP_VFE_DPRX_PORT_SDP_INFOFRAME_VSC_EXT_VESA:
            vfe_sdp_type = DP_SDP_TYPE_VSC_EXT_VESA;
            break;
        case KADP_VFE_DPRX_PORT_SDP_INFOFRAME_AVI:
            vfe_sdp_type = DP_SDP_TYPE_AVI;
            break;
        default:
            DPRX_VFE_ERR("%s, get sdp_attr failed, Unknown sdp type\n", __FUNCTION__);
            return -EFAULT;

    }

    if (attr.data_array==1 && attr.data_len==sizeof(KADP_VFE_DPRX_PORT_SDP_DATA_T) && attr.p_data)
    {
        if (vfe_dprx_drv_get_port_sdp_data(attr.port, vfe_sdp_type, &get_sdp_data)==DPRX_DRV_NO_ERR)
        {
            memset(&ret_sdp_data, 0, sizeof(KADP_VFE_DPRX_PORT_SDP_DATA_T));
            for(i=0; i<4;i++)
            {
                ret_sdp_data.hb[i] = get_sdp_data.hb[i];
            }
            for(i=0; i<32; i++)
            {
                ret_sdp_data.pb[i] = get_sdp_data.pb[i];
            }
            ret_sdp_data.seq_num = get_sdp_data.seq_num;
            ret_sdp_data.time_stamp = get_sdp_data.time_stamp;

            if (copy_to_user( (void __user *)to_user_ptr(attr.p_data), (void *) &ret_sdp_data, attr.data_len))
            {
                DPRX_VFE_ERR("%s, get audio status failed, copy audio status to user space failed\n", __FUNCTION__);
                return -EFAULT;
            }
            ret = DPRX_DRV_NO_ERR;
        }
    }

    return (ret==DPRX_DRV_NO_ERR) ? 0 : -EFAULT;
}

/*---------------------------------------------------
 * Func : vfe_dprx_ioctl
 *
 * Desc : ioctl function of DPRX VFE
 *
 * Para : file  :
 *        cmd   :
 *        arg   :
 *
 * Retn : 0 : success, <0 : failed,
 *--------------------------------------------------*/
long vfe_dprx_ioctl(struct file *file, unsigned int cmd,  unsigned long arg)
{
    long ret = -ENOTTY;

    DPRX_VFE_DBG("%s (cmd=%08x, arg=%lx)\n", __FUNCTION__, cmd, arg);

    if (!IS_VFE_DPRX_IOC(cmd))
    {
        DPRX_VFE_ERR("%s, do ioctl failed, cmd = %x is not belongs to DPRX VFE\n", __FUNCTION__, cmd);
        return -EFAULT;
    }

    switch(GET_DPRX_VFE_IOC_IDX(cmd))
    {
    case VFE_IOC_DPRX_VFE_CTRL_IDX:     ret = _ioctl_ops_dprx_vfe(cmd, arg);     break;
    case VFE_IOC_DPRX_HPD_DET_CTRL_IDX: ret = _ioctl_ops_dprx_hpd_det(cmd, arg); break;
    case VFE_IOC_DPRX_EDID_CTRL_IDX:    ret = _ioctl_ops_dprx_edid(cmd, arg);    break;
    case VFE_IOC_DPRX_HDCP_CTRL_IDX:    ret = _ioctl_ops_dprx_hdcp(cmd, arg);    break;
    case VFE_IOC_DPRX_VIDEO_CTRL_IDX:   ret = _ioctl_ops_dprx_video(cmd, arg);   break;
    case VFE_IOC_DPRX_AUDIO_CTRL_IDX:   ret = _ioctl_ops_dprx_audio(cmd, arg);   break;
    case VFE_IOC_DPRX_SDP_CTRL_IDX:     ret = _ioctl_ops_dprx_sdp(cmd, arg);   break;
    default:
        DPRX_VFE_ERR("%s, do ioctl failed, unkonwn command (%x)\n", __FUNCTION__, cmd);
        return -ENOTTY;
    }

    return ret;
}
