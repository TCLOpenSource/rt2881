/*==========================================================================
* Copyright (c)      Realtek Semiconductor Corporation, 2024
* All rights reserved.
* =========================================================================*/

/*================= File Description ======================================*/
/**
 * @file
 *  This file is for uvc related functions.
 *
 * @author  $Author$
 * @date    $Date$
 * @version $Revision$
 * @ingroup scaler/uvc
 */

#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <rbus/uvc_vgip_reg.h>
#include <rbus/uvc_uzd_reg.h>
#include <rbus/vc_uvc_scaleup_reg.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <rtd_log/rtd_module_log.h>
#include <scaler/scalerDrvCommon.h>
#include <scaler/scalerCommon.h>
#include <tvscalercontrol/uvc/uvcdrv.h>
#include <tvscalercontrol/scaler/scalercolorlib.h>
#include <rtd_log/rtd_module_log.h>
#include <ioctrl/scaler/uvc_cmd_id.h>
#include <scaler/scalerCommon.h>
#include <tvscalercontrol/vip/ultrazoom.h>

struct semaphore UVC_Semaphore;
static bool uvc_tsk_running_flag = FALSE;//Record uvc tsk status. True: Task is running
static struct task_struct *p_uvc_tsk = NULL;
static bool uvc_connect_flag = FALSE;
static bool uvc_run_flag = FALSE;
StructUVCInfo uvc_info = {0};
StructUVCInfo uvc_info_ap = {0};

extern void UVC_ultrazoom_set_scale_down(SIZE* in_Size, SIZE* out_Size, unsigned char panorama);

void rtk_uvc_handle_info(void)
{
    unsigned char h_crop = 0, v_crop = 0;

    //crop
    if((uvc_info.IPH_ACT_WID > uvc_info.Crop_WID))
        h_crop = 1;

    if((uvc_info.IPV_ACT_LEN > uvc_info.Crop_LEN))
        v_crop = 1;

    //uzu output must be even
    if(uvc_info.UVC_OutputWid % 2)
        uvc_info.UVC_OutputWid &= _BIT0;

    if(uvc_info.UVC_OutputLen % 2)
        uvc_info.UVC_OutputLen &= _BIT0;

    //uzu input
    uvc_info.UZU_WID = (uvc_info.Crop_WID > uvc_info.UVC_OutputWid) ? uvc_info.UVC_OutputWid : uvc_info.Crop_WID;
    uvc_info.UZU_LEN = (uvc_info.Crop_LEN > uvc_info.UVC_OutputLen) ? uvc_info.UVC_OutputLen : uvc_info.Crop_LEN;

    //uzd
    uvc_info.H_ScalingDown = (uvc_info.Crop_WID > uvc_info.UZU_WID) ? 1 : 0;
    uvc_info.V_ScalingDown = (uvc_info.Crop_LEN > uvc_info.UZU_LEN) ? 1 : 0;

    //uzu
    uvc_info.H_ScalingUp = (uvc_info.UVC_OutputWid > uvc_info.UZU_WID) ? 1 : 0;
    uvc_info.V_ScalingUp = (uvc_info.UVC_OutputLen > uvc_info.UZU_LEN) ? 1 : 0;

    //unsupport crop + uzu case
    if(h_crop && uvc_info.H_ScalingUp){
        //not crop
        uvc_info.Crop_WID = uvc_info.IPH_ACT_WID;

        //uzd
        uvc_info.H_ScalingDown = (uvc_info.Crop_WID > uvc_info.UZU_WID) ? 1 : 0;

        //uzu
        uvc_info.H_ScalingUp = (uvc_info.UVC_OutputWid > uvc_info.UZU_WID) ? 1 : 0;
    }

    if(v_crop && uvc_info.V_ScalingUp){
        //not crop
        uvc_info.Crop_LEN = uvc_info.IPV_ACT_LEN;

        //uzd
        uvc_info.V_ScalingDown = (uvc_info.Crop_LEN > uvc_info.UZU_LEN) ? 1 : 0;

        //uzu
        uvc_info.V_ScalingUp = (uvc_info.UVC_OutputLen > uvc_info.UZU_LEN) ? 1 : 0;
    }

    //mdomain
    uvc_info.Cap_Wid = uvc_info.UVC_OutputWid;
    uvc_info.Cap_Len = uvc_info.UVC_OutputLen;

    rtd_pr_uvc_notice("input: %d,%d,%d,%d\n", uvc_info.IPH_ACT_STA,uvc_info.IPV_ACT_STA,uvc_info.IPH_ACT_WID,uvc_info.IPV_ACT_LEN);
    rtd_pr_uvc_notice("crop: %d,%d,%d,%d\n", uvc_info.Crop_H_STA,uvc_info.Crop_V_STA,uvc_info.Crop_WID,uvc_info.Crop_LEN);
    rtd_pr_uvc_notice("uzuinput: %d,%d\n",  uvc_info.UZU_WID, uvc_info.UZU_LEN);
    rtd_pr_uvc_notice("down:h %d,v %d, up:h %d,v %d\n", uvc_info.H_ScalingDown,uvc_info.V_ScalingDown,uvc_info.H_ScalingUp,uvc_info.V_ScalingUp);
    rtd_pr_uvc_notice("output: %d,%d,framerate %d,format %d\n", uvc_info.UVC_OutputWid,uvc_info.UVC_OutputLen,uvc_info.UVC_OutputFrameRate,uvc_info.UVC_OutputFormat);
    rtd_pr_uvc_notice("vfreq %d, cap:%d, %d\n",uvc_info.IVFreq, uvc_info.Cap_Wid, uvc_info.Cap_Len);
}

void rtk_uvc_set_vgip(void)
{
    uvc_vgip_uvc_vgip_ctrl_RBUS uvc_vgip_uvc_vgip_ctrl_reg;
    uvc_vgip_uvc_vgip_act_hsta_width_RBUS uvc_vgip_uvc_vgip_act_hsta_width_reg;
    uvc_vgip_uvc_vgip_act_vsta_length_RBUS uvc_vgip_uvc_vgip_act_vsta_length_reg;
    uvc_vgip_uvc_vgip_delay_RBUS uvc_vgip_uvc_vgip_delay_reg;

    uvc_vgip_uvc_vgip_ctrl_reg.regValue = IoReg_Read32(UVC_VGIP_UVC_VGIP_CTRL_reg);
    uvc_vgip_uvc_vgip_ctrl_reg.uvc_in_sel = uvc_info.Input_Position;
    uvc_vgip_uvc_vgip_ctrl_reg.uvc_vs_by_hs_en_n = 1;
    uvc_vgip_uvc_vgip_ctrl_reg.uvc_digital_mode = 1;
    uvc_vgip_uvc_vgip_ctrl_reg.uvc_in_clk_en = 1;
    uvc_vgip_uvc_vgip_ctrl_reg.uvc_ivrun = 1;
    IoReg_Write32(UVC_VGIP_UVC_VGIP_CTRL_reg, uvc_vgip_uvc_vgip_ctrl_reg.regValue);

    uvc_vgip_uvc_vgip_act_hsta_width_reg.regValue = IoReg_Read32(UVC_VGIP_UVC_VGIP_ACT_HSTA_Width_reg);
    uvc_vgip_uvc_vgip_act_hsta_width_reg.uvc_bypass_den = 1;
    uvc_vgip_uvc_vgip_act_hsta_width_reg.uvc_ih_act_sta = uvc_info.IPH_ACT_STA;
    uvc_vgip_uvc_vgip_act_hsta_width_reg.uvc_ih_act_wid = uvc_info.IPH_ACT_WID;
    IoReg_Write32(UVC_VGIP_UVC_VGIP_ACT_HSTA_Width_reg, uvc_vgip_uvc_vgip_act_hsta_width_reg.regValue);

    uvc_vgip_uvc_vgip_act_vsta_length_reg.regValue = IoReg_Read32(UVC_VGIP_UVC_VGIP_ACT_VSTA_Length_reg);
    uvc_vgip_uvc_vgip_act_vsta_length_reg.uvc_iv_act_sta = uvc_info.IPV_ACT_STA;
    uvc_vgip_uvc_vgip_act_vsta_length_reg.uvc_iv_act_len = uvc_info.IPV_ACT_LEN;
    IoReg_Write32(UVC_VGIP_UVC_VGIP_ACT_VSTA_Length_reg, uvc_vgip_uvc_vgip_act_vsta_length_reg.regValue);

    uvc_vgip_uvc_vgip_delay_reg.regValue = IoReg_Read32(UVC_VGIP_UVC_VGIP_DELAY_reg);
    uvc_vgip_uvc_vgip_delay_reg.uvc_ivs_dly = 0;
    IoReg_Write32(UVC_VGIP_UVC_VGIP_DELAY_reg, uvc_vgip_uvc_vgip_delay_reg.regValue);
}

void rtk_uvc_set_uzd(void)
{
    uvc_uzd_uvc_uzd_ctrl0_RBUS uvc_uzd_uvc_uzd_ctrl0_reg;
    uvc_uzd_uvc_uzd_cutout_range_hor_RBUS uvc_uzd_uvc_uzd_cutout_range_hor_reg;
    uvc_uzd_uvc_uzd_cutout_range_ver_RBUS uvc_uzd_uvc_uzd_cutout_range_ver_reg;
    SIZE inSize = {0}, outSize = {0};

    //crop
    uvc_uzd_uvc_uzd_cutout_range_hor_reg.regValue = IoReg_Read32(UVC_UZD_UVC_UZD_CUTOUT_RANGE_HOR_reg);
    uvc_uzd_uvc_uzd_cutout_range_hor_reg.hor_front = uvc_info.Crop_H_STA;
    uvc_uzd_uvc_uzd_cutout_range_hor_reg.hor_width = uvc_info.Crop_WID;
    IoReg_Write32(UVC_UZD_UVC_UZD_CUTOUT_RANGE_HOR_reg, uvc_uzd_uvc_uzd_cutout_range_hor_reg.regValue);

    uvc_uzd_uvc_uzd_cutout_range_ver_reg.regValue = IoReg_Read32(UVC_UZD_UVC_UZD_CUTOUT_RANGE_VER_reg);
    uvc_uzd_uvc_uzd_cutout_range_ver_reg.ver_front = uvc_info.Crop_V_STA;
    uvc_uzd_uvc_uzd_cutout_range_ver_reg.ver_height = uvc_info.Crop_LEN;
    IoReg_Write32(UVC_UZD_UVC_UZD_CUTOUT_RANGE_VER_reg, uvc_uzd_uvc_uzd_cutout_range_ver_reg.regValue);

    uvc_uzd_uvc_uzd_ctrl0_reg.regValue = IoReg_Read32(UVC_UZD_UVC_UZD_CTRL0_reg);
    uvc_uzd_uvc_uzd_ctrl0_reg.cutout_en = 1;
    IoReg_Write32(UVC_UZD_UVC_UZD_CTRL0_reg, uvc_uzd_uvc_uzd_ctrl0_reg.regValue);

    //uzd
    uvc_uzd_uvc_uzd_ctrl0_reg.regValue = IoReg_Read32(UVC_UZD_UVC_UZD_CTRL0_reg);
    uvc_uzd_uvc_uzd_ctrl0_reg.h_zoom_en = uvc_info.H_ScalingDown;
    uvc_uzd_uvc_uzd_ctrl0_reg.v_zoom_en = uvc_info.V_ScalingDown;
    IoReg_Write32(UVC_UZD_UVC_UZD_CTRL0_reg, uvc_uzd_uvc_uzd_ctrl0_reg.regValue);

    inSize.nWidth   = uvc_info.Crop_WID;
    inSize.nLength  = uvc_info.Crop_LEN;
    outSize.nWidth  = uvc_info.UZU_WID;
    outSize.nLength = uvc_info.UZU_LEN;

    UVC_ultrazoom_set_scale_down(&inSize, &outSize, 0);
}

void rtk_uvc_set_uzu(void)
{
    vc_uvc_scaleup_uvc_uzu_ctrl_RBUS vc_uvc_scaleup_uvc_uzu_ctrl_reg;
    vc_uvc_scaleup_uvc_uzu_input_size_RBUS vc_uvc_scaleup_uvc_uzu_input_size_reg;
    vc_uvc_scaleup_uvc_uzu_output_size_RBUS vc_uvc_scaleup_uvc_uzu_output_size_reg;
    
    vc_uvc_scaleup_uvc_uzu_input_size_reg.regValue = IoReg_Read32(VC_UVC_SCALEUP_UVC_UZU_Input_Size_reg);
    vc_uvc_scaleup_uvc_uzu_input_size_reg.hor_input_size = uvc_info.UZU_WID;
    vc_uvc_scaleup_uvc_uzu_input_size_reg.ver_input_size = uvc_info.UZU_LEN;
    IoReg_Write32(VC_UVC_SCALEUP_UVC_UZU_Input_Size_reg, vc_uvc_scaleup_uvc_uzu_input_size_reg.regValue);

    vc_uvc_scaleup_uvc_uzu_output_size_reg.regValue = IoReg_Read32(VC_UVC_SCALEUP_UVC_UZU_Output_Size_reg);
    vc_uvc_scaleup_uvc_uzu_output_size_reg.hor_output_size = uvc_info.UVC_OutputWid;
    vc_uvc_scaleup_uvc_uzu_output_size_reg.ver_output_size = uvc_info.UVC_OutputLen;
    IoReg_Write32(VC_UVC_SCALEUP_UVC_UZU_Output_Size_reg, vc_uvc_scaleup_uvc_uzu_output_size_reg.regValue);

    vc_uvc_scaleup_uvc_uzu_ctrl_reg.regValue = IoReg_Read32(VC_UVC_SCALEUP_UVC_UZU_Ctrl_reg);
    vc_uvc_scaleup_uvc_uzu_ctrl_reg.h_zoom_en = uvc_info.H_ScalingUp;
    vc_uvc_scaleup_uvc_uzu_ctrl_reg.v_zoom_en = uvc_info.V_ScalingUp;
    IoReg_Write32(VC_UVC_SCALEUP_UVC_UZU_Ctrl_reg, vc_uvc_scaleup_uvc_uzu_ctrl_reg.regValue);

    Scaler_Set_PQ_UVC_DataPath(uvc_info_ap.UVC_OutputFormat, 1, uvc_info.Input_Color_Space, uvc_info.UZU_LEN, uvc_info.UZU_WID, uvc_info.UVC_OutputLen ,uvc_info.UVC_OutputWid);
}

void rtk_set_uvc_path(void)
{
    rtk_uvc_handle_info();
    rtk_uvc_set_vgip();
    rtk_uvc_set_uzd();
    rtk_uvc_set_uzu();
}

void uvc_in_sel_ctrl(unsigned char in_sel_type)
{
    uvc_vgip_uvc_vgip_ctrl_RBUS uvc_vgip_uvc_vgip_ctrl_reg;

    uvc_vgip_uvc_vgip_ctrl_reg.regValue = IoReg_Read32(UVC_VGIP_UVC_VGIP_CTRL_reg);
    uvc_vgip_uvc_vgip_ctrl_reg.uvc_in_sel = in_sel_type;
    IoReg_Write32(UVC_VGIP_UVC_VGIP_CTRL_reg, uvc_vgip_uvc_vgip_ctrl_reg.regValue);
}

void uvc_src_clock_ctrl(unsigned char bflag)
{
    uvc_vgip_uvc_vgip_ctrl_RBUS uvc_vgip_uvc_vgip_ctrl_reg;

    uvc_vgip_uvc_vgip_ctrl_reg.regValue = IoReg_Read32(UVC_VGIP_UVC_VGIP_CTRL_reg);
    if(bflag) { // enable
        uvc_vgip_uvc_vgip_ctrl_reg.uvc_in_clk_en = 1;
    } else {    // disable
        uvc_vgip_uvc_vgip_ctrl_reg.uvc_in_clk_en = 0;
    }
    IoReg_Write32(UVC_VGIP_UVC_VGIP_CTRL_reg, uvc_vgip_uvc_vgip_ctrl_reg.regValue);
}

unsigned char rtk_uvc_mute(bool enable)
{
    uvc_vgip_uvc_hdcp_secure_black_ctrl2_RBUS uvc_vgip_uvc_hdcp_secure_black_ctrl2_reg;
    rtd_pr_uvc_notice("[%s %d]enable: %d\n",__func__,  __LINE__, enable);

    uvc_vgip_uvc_hdcp_secure_black_ctrl2_reg.regValue = IoReg_Read32(UVC_VGIP_UVC_HDCP_SECURE_BLACK_CTRL2_reg);
    if(enable){
        uvc_vgip_uvc_hdcp_secure_black_ctrl2_reg.black_en_sw_mode = 1;
        uvc_vgip_uvc_hdcp_secure_black_ctrl2_reg.force_data_yg = 0;
        uvc_vgip_uvc_hdcp_secure_black_ctrl2_reg.force_data_ub = 0;
        uvc_vgip_uvc_hdcp_secure_black_ctrl2_reg.force_data_vr = 0;
    }else{
        uvc_vgip_uvc_hdcp_secure_black_ctrl2_reg.black_en_sw_mode = 0;
    }
    IoReg_Write32(UVC_VGIP_UVC_HDCP_SECURE_BLACK_CTRL2_reg, uvc_vgip_uvc_hdcp_secure_black_ctrl2_reg.regValue);
    return TRUE;
}

unsigned char rtk_uvc_connect(bool enable, unsigned int position, unsigned int format)
{
    rtd_pr_uvc_notice("[%s %d]enable: %d, position:%d, format:%d\n",__func__,  __LINE__, enable, position, format);
    down(&UVC_Semaphore);
    uvc_info_ap.Input_Position = position;
    uvc_info_ap.UVC_OutputFormat = format;

    if(enable){
        uvc_connect_flag = true;
        up(&UVC_Semaphore);
    }else{
        uvc_connect_flag = false;
        up(&UVC_Semaphore);
        uvc_info.state = _MODE_STATE_SEARCH;
        uvc_in_sel_ctrl(0x7);//reserved value
        uvc_src_clock_ctrl(0);

        //To do: disable dma
    }
    return TRUE;
}

unsigned char rtk_uvc_region_ctrl(unsigned int UID, VIDEO_RECT_T crop_window, VIDEO_RECT_T ori_window, VIDEO_RECT_T output_window, VIDEO_RECT_T zoom_window, UVC_WINDOW_ZOOM_TYPE zoom_type, unsigned int outputFrameRate)
{
    rtd_pr_uvc_notice("[%s %d]UID: %d, framerate: %d\n",__func__,  __LINE__, UID, outputFrameRate);
    rtd_pr_uvc_notice("input(%d,%d,%d,%d)\n", ori_window.x, ori_window.y, ori_window.w, ori_window.h);
    rtd_pr_uvc_notice("crop(%d,%d,%d,%d)\n", crop_window.x, crop_window.y, crop_window.w, crop_window.h);
    rtd_pr_uvc_notice("output(%d,%d,%d,%d)\n", output_window.x, output_window.y, output_window.w, output_window.h);
    rtd_pr_uvc_notice("zoom(%d,%d,%d,%d), zoom_type:%d\n", zoom_window.x, zoom_window.y, zoom_window.w, zoom_window.h, zoom_type);

    down(&UVC_Semaphore);
    uvc_info_ap.IPH_ACT_STA = ori_window.x;
    uvc_info_ap.IPV_ACT_STA = ori_window.y;
    uvc_info_ap.IPH_ACT_WID = ori_window.w;
    uvc_info_ap.IPV_ACT_LEN = ori_window.h;
    uvc_info_ap.Crop_H_STA = crop_window.x;
    uvc_info_ap.Crop_V_STA = crop_window.y;
    uvc_info_ap.Crop_WID = crop_window.w;
    uvc_info_ap.Crop_LEN = crop_window.h;
    uvc_info_ap.UVC_OutputWid = output_window.w;
    uvc_info_ap.UVC_OutputLen = output_window.h;
    uvc_info_ap.UVC_OutputFrameRate = outputFrameRate;

    if(uvc_connect_flag){
        uvc_run_flag = true;
    }
    up(&UVC_Semaphore);

    return TRUE;
}

static int uvc_run_tsk(void *p)
{
    while (1)
    {
        msleep(10);//need to switch
        down(&UVC_Semaphore);
        if(uvc_run_flag){
            uvc_run_flag = false;
            uvc_info_ap.state = uvc_info.state;
            uvc_info = uvc_info_ap;
            up(&UVC_Semaphore);
            rtk_set_uvc_path();
            uvc_info.state = _MODE_STATE_ACTIVE;
        }else{
            up(&UVC_Semaphore);
        }
		if (freezing(current))
       	{//for some continue case
			try_to_freeze();
        }
		if (kthread_should_stop()) {
         	break;
      	}
    }
    do_exit(0);
    return 0;
}

static void create_uvc_run_tsk(void)
{
	int err;
	if (uvc_tsk_running_flag == FALSE) {
		p_uvc_tsk = kthread_create(uvc_run_tsk, NULL, "uvc_run_tsk\n");

	    if (p_uvc_tsk) {
			wake_up_process(p_uvc_tsk);
			uvc_tsk_running_flag = TRUE;
	    } else {
	    	err = PTR_ERR(p_uvc_tsk);
	    	rtd_pr_uvc_notice("Unable to start uvc_run_tsk (err_id = %d)\n", err);
	    }
	}
}

static void delete_uvc_run_tsk(void)
{
	int ret;
	if (uvc_tsk_running_flag) {
		if(p_uvc_tsk){
	 		ret = kthread_stop(p_uvc_tsk);
	 		if (!ret) {
	 			p_uvc_tsk = NULL;
	 			uvc_tsk_running_flag = FALSE;
				rtd_pr_uvc_notice("uvc run thread stopped\n");
	 		}
		}else{
			rtd_pr_uvc_notice("uvc run thread stopped already\n");
		}
	}
}

static int uvc_suspend (struct device *p_dev)
{
	return 0;
}

static int uvc_resume (struct device *p_dev)
{
	return 0;
}

static int uvc_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int uvc_release(struct inode *inode, struct file *filep)
{
	return 0;
}

static ssize_t uvc_read(struct file *filep, char *buffer, size_t count, loff_t *offp)
{
	return 0;
}

static ssize_t uvc_write(struct file *filep, const char *buffer, size_t count, loff_t *offp)
{
	return 0;
}

long uvc_ioctl(struct file *file, unsigned int cmd,  unsigned long arg)
{
    int retVal = 0;

    rtd_pr_uvc_info("UVC ioctl: %d\n", _IOC_NR(cmd));

    if (_IOC_TYPE(cmd) != UVC_IOC_MAGIC || _IOC_NR(cmd) > UVC_IOC_MAXNR)
        return -ENOTTY ;

    switch (cmd)
    {
        case UVC_IOC_CONNECT:
        {
            UVC_CONNECT_PARA_T connectPara;
            if(copy_from_user((void *)&connectPara, (const void __user *)arg, sizeof(UVC_CONNECT_PARA_T)))
            {
                retVal = -EFAULT;
                rtd_pr_uvc_debug("scaler uvc ioctl code=UVC_IOC_CONNECT copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                retVal = rtk_uvc_connect(connectPara.bConnect, connectPara.inputPosition, connectPara.captureFormat);
            }
            break;
        }

        case UVC_IOC_SET_WINBLANK:
        {
            UVC_WINBLANK_PARA_T winBlankPara;
            if(copy_from_user((void *)&winBlankPara, (const void __user *)arg, sizeof(UVC_WINBLANK_PARA_T)))
            {
                retVal = -EFAULT;
                rtd_pr_uvc_debug("scaler uvc ioctl code=UVC_IOC_SET_WINBLANK copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                retVal =  rtk_uvc_mute(winBlankPara.winBlankBEnable);
            }
            break;
        }

        case UVC_IOC_SET_REGION_CTRL:
        {
            UVC_REGION_CTRL_PARA_T regionCtrlPara;
            if(copy_from_user((void *)&regionCtrlPara, (const void __user *)arg, sizeof(UVC_REGION_CTRL_PARA_T)))
            {
                retVal = -EFAULT;
                rtd_pr_uvc_debug("scaler uvc ioctl code=UVC_IOC_SET_REGION_CTRL copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                retVal = rtk_uvc_region_ctrl(regionCtrlPara.UID,
                    regionCtrlPara.cropWin,
                    regionCtrlPara.oriWin,
                    regionCtrlPara.outputWin,
                    regionCtrlPara.zoomWin,
                    regionCtrlPara.zoomType,
                    regionCtrlPara.outputFrameRate
                    );
            }
            break;
        }

        default:
            rtd_pr_uvc_debug("UVC ioctl code = %d is invalid!!!!!!!!!!!!!!!\n", _IOC_NR(cmd));
            break;
    }

    return retVal;
}

#if defined(CONFIG_COMPAT) && defined(CONFIG_ARM64)
static long uvc_compat_ioctl(struct file *file, unsigned int cmd,  unsigned long arg)
{
    return uvc_ioctl(file, cmd, (unsigned long)compat_ptr(arg));
}
#endif

static struct cdev uvc_cdev;
static int uvc_major   = UVC_MAJOR;
static int uvc_minor   = 0 ;
static int uvc_nr_devs = UVC_NR_DEVS;

module_param(uvc_major, int, S_IRUGO);
module_param(uvc_minor, int, S_IRUGO);
module_param(uvc_nr_devs, int, S_IRUGO);

static struct class *uvc_class = NULL;

static struct file_operations uvcdev_fops= {
	.owner          = THIS_MODULE,
	.open           = uvc_open,
	.release        = uvc_release,
	.read           = uvc_read,
	.write          = uvc_write,
	.unlocked_ioctl = uvc_ioctl,
#if defined(CONFIG_COMPAT) && defined(CONFIG_ARM64)
	.compat_ioctl   = uvc_compat_ioctl,
#endif
};

#ifdef CONFIG_PM
static const struct dev_pm_ops uvc_pm_ops =
{
	.suspend    = uvc_suspend,
	.resume     = uvc_resume,
#ifdef CONFIG_HIBERNATION
	.freeze 	= uvc_suspend,
	.thaw		= uvc_resume,
	.poweroff	= uvc_suspend,
	.restore	= uvc_resume,
#endif

};
#endif

static struct platform_driver uvc_device_driver = {
	.driver = {
		.name       = UVC_DEVICE_NAME,
		.bus        = &platform_bus_type,
#ifdef CONFIG_PM
		.pm         = &uvc_pm_ops,
#endif
	},
};

static char *uvc_devnode(struct device *dev, umode_t *mode)
{
	return NULL;
}

int uvc_module_init(void)
{
	int result;
	int uvc_devno;
	//char cmd_buf[25];
	dev_t devno = 0;//uvc device number

	result = alloc_chrdev_region(&devno, uvc_minor, uvc_nr_devs, UVC_DEVICE_NAME);
	uvc_major = MAJOR(devno);
	if (result < 0) {
		rtd_pr_uvc_warn("UVC_DEVICE: can't get major %d\n", uvc_major);
		return result;
	}

	rtd_pr_uvc_debug("UVC_DEVICE init module major number = %d\n", uvc_major);
	uvc_devno = MKDEV(uvc_major, uvc_minor);

	uvc_class = class_create(THIS_MODULE, UVC_DEVICE_NAME);

	if (IS_ERR(uvc_class))
	{
		rtd_pr_uvc_debug("scaler uvc: can not create class...\n");
		result=PTR_ERR(uvc_class);
		goto fail_class_create;
	}
	uvc_class->devnode = uvc_devnode;

	result = platform_driver_register(&uvc_device_driver);
	if (result) {
		rtd_pr_uvc_emerg("%s: can not register platform driver, ret=%d\n", __func__, result);
		goto fail_platform_driver_register;
	}

	cdev_init(&uvc_cdev, &uvcdev_fops);
	uvc_cdev.owner = THIS_MODULE;
	uvc_cdev.ops = &uvcdev_fops;
	result = cdev_add (&uvc_cdev, uvc_devno, 1);

	if (result)
	{
		rtd_pr_uvc_notice("Error %d adding UVC_DEVICE!\n", result);
		goto fail_cdev_init;
	}

	device_create(uvc_class, NULL, MKDEV(uvc_major, 0), NULL, UVC_DEVICE_NAME);
	sema_init(&UVC_Semaphore, 1);

    create_uvc_run_tsk();

	return 0;//Success

fail_cdev_init:
	platform_driver_unregister(&uvc_device_driver);
fail_platform_driver_register:
	class_destroy(uvc_class);
fail_class_create:
	uvc_class = NULL;
	unregister_chrdev_region(uvc_devno, uvc_nr_devs);
	return result;
}

void __exit uvc_module_exit(void)
{
	dev_t devno = MKDEV(uvc_major, uvc_minor);
	rtd_pr_uvc_info("uvc clean module uvc_major = %d\n", uvc_major);
    delete_uvc_run_tsk();

  	device_destroy(uvc_class, MKDEV(uvc_major, 0));
  	class_destroy(uvc_class);
	uvc_class=NULL;
	cdev_del(&uvc_cdev);

	platform_driver_unregister(&uvc_device_driver);

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, uvc_nr_devs);
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(uvc_module_init);
module_exit(uvc_module_exit);
#endif

