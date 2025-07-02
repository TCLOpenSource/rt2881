/*=============================================================
 * Copyright (c)      Realtek Semiconductor Corporation, 2016
 *
 * All rights reserved.
 *
 *============================================================*/

/*======================= Description ============================
 * file:    dprx_drvif.c
 * author:  Edwiin_Lai
 * date:    2022/07/04
 * version: 1.0
 *============================================================*/
#include <dprx_vfe.h>
#include "rtk_kdriver/dprx/dprxfun.h"
#include <ioctrl/scaler/vfe_cmd_id.h>
/**********************************************************************************************
*
*   Generic API
*
**********************************************************************************************/
extern dprx_dev_t* _vfe_dprx_drv_get_dprx_adapter(unsigned char ch);

/*------------------------------------------------------
 * Func : drvif_Dprx_GetRawTimingInfo
 *
 * Desc : get current DP raw timing info of current DP port
 *
 * Parm : p_timing_info: pointer of DPRX_TIMING_INFO_T variable from caller
 *
 * Retn : current DP port timing info
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_GetPortRawTimingInfo(unsigned char port, DPRX_TIMING_INFO_T* p_timing_info)
{
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(port);
    if ((p_timing_info==NULL)||(p_adp==NULL))
        return 0;

    memset(p_timing_info, 0, sizeof(DPRX_TIMING_INFO_T));

    if (dprx_adp_get_video_timing(p_adp, p_timing_info)<0)
        return DPRX_DRV_HW_ERR;

    return 1;
}
EXPORT_SYMBOL(drvif_Dprx_GetPortRawTimingInfo);


/*------------------------------------------------------
 * Func : drvif_Dprx_GetPortColorSpace
 *
 * Desc : Get Color space info of selected DPRX port
 *
 * Parm : port : selected port
 *
 * Retn : DP_COLOR_SPACE_RGB
 *        DP_COLOR_SPACE_YUV444
 *        DP_COLOR_SPACE_YUV422
 *        DP_COLOR_SPACE_YUV420
 *------------------------------------------------------*/
DP_COLOR_SPACE_E drvif_Dprx_GetPortColorSpace(unsigned char port)
{
    DPRX_TIMING_INFO_T timing_info;

    if (drvif_Dprx_GetPortRawTimingInfo(port, &timing_info)==1)
        return timing_info.color_space;

    return DP_COLOR_SPACE_RGB;
}
EXPORT_SYMBOL(drvif_Dprx_GetPortColorSpace);

/*------------------------------------------------------
 * Func : drvif_Dprx_GetPortColorDepth
 *
 * Desc : Get Color depth info of selected DPRX port
 *
 * Parm : port : selected port
 *
 * Retn : DP_COLOR_DEPTH_8B
 *        DP_COLOR_DEPTH_10B
 *        DP_COLOR_DEPTH_12B
 *        DP_COLOR_DEPTH_16B
 *------------------------------------------------------*/
DP_COLOR_DEPTH_E drvif_Dprx_GetPortColorDepth(unsigned char port)
{
    DPRX_TIMING_INFO_T timing_info;

    if (drvif_Dprx_GetPortRawTimingInfo(port, &timing_info)==1)
        return timing_info.color_depth;

    return DP_COLOR_DEPTH_8B;
}
EXPORT_SYMBOL(drvif_Dprx_GetPortColorDepth);

/*------------------------------------------------------
 * Func : drvif_Dprx_GetPortInputType
 *
 * Desc : Get Cable Type of selected DPRX port
 *
 * Parm : port : selected port
 *
 * Retn : DP_TYPE_USB_TYPE_C
 *        DP_TYPE_DP
 *------------------------------------------------------*/
DP_TYPE drvif_Dprx_GetPortInputType(unsigned char port)
{
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(port);
    DP_PORT_CAPABILITY cap;

    if (p_adp && dprx_adp_get_capability(p_adp, &cap)==0 && cap.type==DP_TYPE_USB_TYPE_C)
        return DP_TYPE_USB_TYPE_C;

    return DP_TYPE_DP;
}
EXPORT_SYMBOL(drvif_Dprx_GetPortInputType);

/*------------------------------------------------------
 * Func : drvif_Dprx_CheckPortMode
 *
 * Desc : Check Dprx status of selected DPRX port. this function is
 *        called by VSC periodically to check DPRX status
 *
 * Parm : port : selected port
 *
 * Retn : 1  : DPRX status check is OK
 *        0 : some error detected
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_CheckPortMode(unsigned char port)
{
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(port);
    if (p_adp==NULL)
        return 0;

    return (dprx_adp_source_check(p_adp)==0) ? 1 : 0;
}
EXPORT_SYMBOL(drvif_Dprx_CheckPortMode);


/*------------------------------------------------------
 * Func : drvif_Dprx_GetDrmInfoFrame
 *
 * Desc : Get DRM Info Frame of currenct DPRX port
 *
 * Parm : pDrmInfo: pointer of DPRX_DRM_INFO_T variable from caller
 *
 * Retn : 1  : successed
 *        0 : failed
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_GetPortDrmInfoFrame(unsigned char port, DPRX_DRM_INFO_T *pDrmInfo)
{
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(port);
    if ((pDrmInfo==NULL)||(p_adp==NULL))
        return 0;

    if(dprx_adp_get_drm_info(p_adp, pDrmInfo)<0)
        return 0;
    else
        return 1;
}

EXPORT_SYMBOL(drvif_Dprx_GetPortDrmInfoFrame);


/*------------------------------------------------------
 * Func : drvif_Dprx_GetRgbYuvRangeMode
 *
 * Desc : Get Range of RGB/YUV of current DPRX port. this function will
 *        be called by
 *
 * Parm : N/A
 *
 * Retn : MODE_RAG_DEFAULT
 *        MODE_RAG_LIMIT
 *        MODE_RAG_LIMIT
 *------------------------------------------------------*/
DPRX_RGB_YUV_RANGE_MODE_T drvif_Dprx_GetRgbYuvRangeMode(void)
{
    //TBD
    return DPRX_MODE_RAG_DEFAULT;
}

EXPORT_SYMBOL(drvif_Dprx_GetRgbYuvRangeMode);


/*------------------------------------------------------
 * Func : drvif_Dprx_GetAviInfoFrame
 *
 * Desc : Get AVI Info Frame of currenct DPRX port
 *
 * Parm : pAviInfo : AVI info frame output (16 bytes)
 *
 * Retn : 1  : successed
 *        0 : failed
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_GetPortAviInfoFrame(unsigned char port, DPRX_AVI_INFO_T *pAviInfo)
{
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(port);
    if ((pAviInfo==NULL)||(p_adp==NULL))
        return 0;

    if(dprx_adp_get_avi_info(p_adp, pAviInfo)<0)
        return 0;
    else
        return 1;
    memset(pAviInfo,0,sizeof(DPRX_AVI_INFO_T));
    //TBD
    return 1;
}

EXPORT_SYMBOL(drvif_Dprx_GetPortAviInfoFrame);


/*------------------------------------------------------
 * Func : drvif_Dprx_GetPortSdpData
 *
 * Desc : get sdp data
 *
 * Parm : p_data : info of dolby vsif data
 *
 * Retn : 0 : fail, 1 : success
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_GetPortSdpData(unsigned char port, DP_SDP_TYPE type, DP_SDP_DATA_T* p_data)
{
    if ((p_data==NULL)||(g_connected_dprx==NULL))
        return 0;

    memset(p_data, 0, sizeof(DP_SDP_DATA_T));

    if (dprx_adp_get_sdp_data(g_connected_dprx, type, p_data)<0)
        return 0;

    return 1;
}

EXPORT_SYMBOL(drvif_Dprx_GetPortSdpData);


/*------------------------------------------------------
 * Func : drvif_Dprx_GetPortPPSData
 *
 * Desc : get pps data
 *
 * Parm : ch :
 *        p_data : info of dsc pps
 *
 * Retn : 1 : PPS present
 *        0 : pps absent
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_GetPortPPSData(unsigned char ch, DP_PPS_DATA_T* p_pps)
{
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(ch);

    if (p_adp && dprx_adp_get_pps_data(p_adp, p_pps)==0)
        return 1;

    return 0;
}

EXPORT_SYMBOL(drvif_Dprx_GetPortPPSData);


/*------------------------------------------------------
 * Func : drvif_Dprx_GetPixelEncodingFormat
 *
 * Desc : Get pixel encoding format of currenct DPRX port
 *
 * Parm : pPixelEnFormat : pointer of pixel encoding format
 *
 * Retn : 1  : successed
 *        0 : failed
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_GetPortPixelEncodingFormat(unsigned char port, DP_PIXEL_ENCODING_FORMAT_T *pPixelEnFormat)
{
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(port);
    if ((pPixelEnFormat==NULL)||(p_adp==NULL))
        return 0;

    if(dprx_adp_get_pixel_encoding_info(p_adp, pPixelEnFormat)<0)
        return 0;
    else
        return 1;
}

EXPORT_SYMBOL(drvif_Dprx_GetPortPixelEncodingFormat);


 /*------------------------------------------------------
 * Func : drvif_Dprx_GetHDR10pVsInfoFrame
 *
 * Desc : Get HDR10+ Vendor Specific Info Frame of currenct DPRX port
 *
 * Parm : info_frame : HDR10+ Vendor specific info frame output
 *
 * Retn : 1  : successed
 *        0 : failed
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_GetHDR10pVsInfoFrame(DP_VSI_T *info_frame)
{
    if ((info_frame==NULL)||(g_connected_dprx==NULL))
        return 0;
    memset(info_frame,0,sizeof(DP_VSI_T));
    //TBD
    return 1;
}

EXPORT_SYMBOL(drvif_Dprx_GetHDR10pVsInfoFrame);


//-------------------------------------------------------------------------------
// DRR Related
//-------------------------------------------------------------------------------

/*------------------------------------------------------
 * Func : drvif_Dprx_GetDrrMode
 *
 * Desc : get drr mode
 *
 * Parm : N/A
 *
 * Retn : DP_DRR_MODE_FRR
 *        DP_DRR_MODE_AMD_FREE_SYNC
 *        DP_DRR_MODE_ADAPTIVE_SYNC
 *------------------------------------------------------*/
DP_DRR_MODE_E drvif_Dprx_GetPortDrrMode(unsigned char port)
{
    DPRX_TIMING_INFO_T timing_info;
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(port);
    if (p_adp==NULL)
        return DP_DRR_MODE_FRR;

    memset(&timing_info, 0, sizeof(DPRX_TIMING_INFO_T));

    if (dprx_adp_get_video_timing(p_adp, &timing_info)<0)
        return DP_DRR_MODE_FRR;

    return (DP_DRR_MODE_E) timing_info.drr_mode;
}
EXPORT_SYMBOL(drvif_Dprx_GetPortDrrMode);


//-------------------------------------------------------------------------------
// AMD Free Sync Related
//-------------------------------------------------------------------------------


/*------------------------------------------------------
 * Func : drvif_Dprx_GetAMDFreeSyncEnable
 *
 * Desc : get AMD free sync status
 *
 * Parm : N/A
 *
 * Retn : 1 : DPRX status check is OK
 *        0 : some error detected
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_GetPortAMDFreeSyncEnable(unsigned char port)
{
    return (drvif_Dprx_GetPortDrrMode(port)==DP_DRR_MODE_AMD_FREE_SYNC) ? 1 : 0;
}
EXPORT_SYMBOL(drvif_Dprx_GetPortAMDFreeSyncEnable);


/*------------------------------------------------------
 * Func : drvif_Dprx_GetAMDFreeSyncStatus
 *
 * Desc : get AMD Free Sync Status
 *
 * Parm : p_info : info of AMD free sync status
 *
 * Retn : 0 : fail, 1 : success
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_GetPortAMDFreeSyncStatus(unsigned char port, DP_AMD_FREE_SYNC_INFO_T* p_info)
{
    DP_SDP_DATA_T sdp;
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(port);

    if (p_info==NULL || p_adp==NULL)
        return 0;

    memset(p_info, 0, sizeof(DP_AMD_FREE_SYNC_INFO_T));

    if (dprx_adp_get_sdp_data(p_adp, DP_SDP_TYPE_SPD, &sdp)<0) {
        DP_VFE_INFO("drvif_Dprx_GetAMDFreeSyncStatus failed \n");
        return 0;
    }

    // PB0 : Check Sum
    // PB1-3 : AMD OUI
    // PB4-5 : reserved
    // PB6 : b
    // PB7 : Min Refresh Rate (Hz)
    // PB8 : Max Refresh Rate (Hz)
    // c7 1a 00 00 _ 00 00 01 3c _ 3c 00 00 00 _ 00 00 00 00

    DP_VFE_INFO("SPD : hb={%02x %02x %02x %02x} pb={%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x}\n",
        sdp.hb[0], sdp.hb[1], sdp.hb[2], sdp.hb[3],
        sdp.pb[0], sdp.pb[1], sdp.pb[2], sdp.pb[3],
        sdp.pb[4], sdp.pb[5], sdp.pb[6], sdp.pb[7],
        sdp.pb[8], sdp.pb[9]);

    if (sdp.hb[0]== 0x00 && sdp.hb[1]== 0x83 &&
        sdp.hb[2]== 0x1B && sdp.hb[3]== 0x08 &&
        sdp.pb[1]==0x1A && sdp.pb[2]==0x00 && sdp.pb[3]==0x00)
    {
        p_info->version = 2;
        p_info->freesync_supported = ((sdp.pb[6] >>0) & 0x1);
        p_info->freesync_enabled   = ((sdp.pb[6] >>1) & 0x1);
        p_info->freesync_activate  = ((sdp.pb[6] >>2) & 0x1);
        p_info->native_color_space_active  = ((sdp.pb[6] >>3) & 0x1);
        p_info->brightness_control_active  = ((sdp.pb[6] >>4) & 0x1);
        p_info->local_diming_disalbe  = ((sdp.pb[6] >>5) & 0x1);
        p_info->min_refresh_rate   = sdp.pb[7];
        p_info->max_refresh_rate   = sdp.pb[8];

        p_info->srgb_eotf_active   = ((sdp.pb[9] >>0) & 0x1);
        p_info->bt709_eotf_active  = ((sdp.pb[9] >>1) & 0x1);
        p_info->gamma_2p2_eotf_active  = ((sdp.pb[9] >>2) & 0x1);
        p_info->gamma_2p6_eotf_active  = ((sdp.pb[9] >>3) & 0x1);
        p_info->gamma_2p6_eotf_active  = ((sdp.pb[9] >>5) & 0x1);
        p_info->brightness_control  = sdp.pb[10];
        return 1;
    }

    return 0;
}

EXPORT_SYMBOL(drvif_Dprx_GetPortAMDFreeSyncStatus);

/*------------------------------------------------------
 * Func : drvif_Dprx_SetVideoWatchDagEnable
 *
 * Desc : Disable/Enable Video Watch Dog Enable
 *
 * Parm : enable: Enable/Disable Video WatchDog
 *
 * Retn : 0 : fail, 1 : success
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_SetPortVideoWatchDagEnable(unsigned char port, unsigned char enable)
{
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(port);
    if (p_adp==NULL)
        return 0;

    if (dprx_adp_set_ext_ctrl(p_adp, (ADP_EXCTL_ID)ADP_EXCTL_WATCH_DOG_ENABLE, enable)<0)
        return 0;
    return 1;
}
EXPORT_SYMBOL(drvif_Dprx_SetPortVideoWatchDagEnable);


/*------------------------------------------------------
 * Func : drvif_Dprx_SetVideoCommonSel
 *
 * Desc : Set video common of the connected DPRX port
 *        (some soc supports multiple video common)
 *
 * Parm : sel : video common sel
 *         DP_VIDEO_COMMON_SEL_NONE : none (off, not output)
 *         DP_VIDEO_COMMON_SEL_1 : video common 1
 *         DP_VIDEO_COMMON_SEL_2 : video common 2
 *         DP_VIDEO_COMMON_SEL_3 : video common 3
 *         DP_VIDEO_COMMON_SEL_4 : video common 4
 *
 * Retn : TRUE  : DPRX status check is OK
 *        FALSE : some error detected
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_SetPortVideoCommonSel(unsigned char port, DP_VIDEO_COMMON_SEL sel)
{
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(port);
    if (p_adp==NULL)
        return 0;

    if (dprx_adp_set_ext_ctrl(p_adp, (ADP_EXCTL_ID)ADP_EXCTL_VIDEO_COMMON_SEL, (unsigned int) sel)<0)
        return 0;

    return 1;
}
EXPORT_SYMBOL(drvif_Dprx_SetPortVideoCommonSel);


/*------------------------------------------------------
 * Func : drvif_Dprx_SetPortHdmitxBypassEn
 *
 * Desc : Enable DPRX bypass to HDMI TX enable
 *
 * Parm : ch:
 *        enable : 0 : disable hdmi tx bypass
 *                 1 : enable hdmi tx bypass
 *
 * Retn : 0 : fail, 1 : success
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_SetPortHdmitxBypassEn(
    unsigned char           ch,
    unsigned char           enable
    )
{
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(ch);
    if (p_adp==NULL)
        return 0;

    if (enable)
    {
        if (dprx_adp_set_ext_ctrl(p_adp, (ADP_EXCTL_ID)ADP_EXCTL_STREAM_BYPASS_MODE, DPRX_STREAM_BYPASS_MODE_TO_HDMITX)<0 ||
            dprx_adp_set_ext_ctrl(p_adp, (ADP_EXCTL_ID)ADP_EXCTL_STREAM_BYPASS_ENABLE, 1)<0)
            return 0;
    }
    else
    {
        if (dprx_adp_set_ext_ctrl(p_adp, (ADP_EXCTL_ID)ADP_EXCTL_STREAM_BYPASS_MODE, DPRX_STREAM_BYPASS_MODE_OFF)<0 ||
            dprx_adp_set_ext_ctrl(p_adp, (ADP_EXCTL_ID)ADP_EXCTL_STREAM_BYPASS_ENABLE, 0)<0)
            return 0;
    }

    return 1;
}

EXPORT_SYMBOL(drvif_Dprx_SetPortHdmitxBypassEn);

HDMI_COLOR_DEPTH_T _to_hdmi_color_depth(DP_COLOR_DEPTH_E depth)
{
    switch(depth)
    {
    case DP_COLOR_DEPTH_8B:  return HDMI_COLOR_DEPTH_8B;
    case DP_COLOR_DEPTH_10B: return HDMI_COLOR_DEPTH_10B;
    case DP_COLOR_DEPTH_12B: return HDMI_COLOR_DEPTH_12B;
    case DP_COLOR_DEPTH_16B: return HDMI_COLOR_DEPTH_16B;
    default:                 return HDMI_COLOR_DEPTH_8B;
    }
}

HDMI_COLOR_SPACE_T _to_hdmi_color_space(DP_COLOR_SPACE_E color_space)
{
    switch(color_space)
    {
    case DP_COLOR_SPACE_RGB:    return COLOR_RGB;
    case DP_COLOR_SPACE_YUV444: return COLOR_YUV422;
    case DP_COLOR_SPACE_YUV422: return COLOR_YUV444;
    case DP_COLOR_SPACE_YUV420: return COLOR_YUV420;
    default:                    return COLOR_UNKNOW;
    }
}

HDMI_COLORIMETRY_T _to_hdmi_colorimetry(DP_COLORIMETRY_E colorimetry)
{
    switch(colorimetry)
    {
    case DP_COLORIMETRY_ADOBE_RGB:     return HDMI_COLORIMETRY_ADOBE_RGB;
    case DP_COLORIMETRY_BT_601:        return HDMI_COLORIMETRY_601;
    case DP_COLORIMETRY_BT_709:        return HDMI_COLORIMETRY_709;
    case DP_COLORIMETRY_XV_YCC_601:    return HDMI_COLORIMETRY_XYYCC601;
    case DP_COLORIMETRY_XV_YCC_709:    return HDMI_COLORIMETRY_XYYCC709;
    case DP_COLORIMETRY_S_YCC_601:     return HDMI_COLORIMETRY_SYCC601;
    case DP_COLORIMETRY_BT_2020_RGB:   return HDMI_COMORIMETRY_BT2020_RGB_OR_YCC;
    case DP_COLORIMETRY_BT_2020_YCBCR: return HDMI_COLORIMETRY_BT2020_YCC;
    case DP_COLORIMETRY_DCI_P3:        return HDMI_COMORIMETRY_ST_2113_P3DCI;
    default:                           return HDMI_COLORIMETRY_NOSPECIFIED;
    }
}

PIXEL_MODE_E _to_hdmi_pixel_mode(unsigned char pixe_mode)
{
    switch(pixe_mode)
    {
    case DPRX_OUT_PIXEL_MODE_1P:       return ONE_PIXEL_MODE;
    case DPRX_OUT_PIXEL_MODE_2P:       return TWO_PIXEL_MODE;
    case DPRX_OUT_PIXEL_MODE_4P:       return FOUR_PIXEL_MODE;
    default:                           return PIXEL_MODE_RESERVED;
    }
}

unsigned short _compute_hc_active_bytes(
    DP_PPS_DATA_T*          p_pps_data
    )
{
    // hcacitve bytes - slice * celling (slice width * bpp / 8)
    unsigned short pic_width   = (p_pps_data->pps[8] <<8) + (p_pps_data->pps[9]);
    unsigned short slice_width = (p_pps_data->pps[12]<<8) + (p_pps_data->pps[13]);
    unsigned short slice_num   = ((pic_width + slice_width -1) / slice_width);
    unsigned short bpp         = ((p_pps_data->pps[4] & 0x3) <<8) + (p_pps_data->pps[5]);
    unsigned short hcacitve_bytes =  slice_num * (((slice_width * bpp) + 127) /128);
    DP_VFE_INFO("_compute_hc_active_bytes : pic_width = %d, slice_width=%d, slice_num=%d, bpp=%d, hcacitve_bytes=%d\n", pic_width,  slice_width, slice_num, bpp, hcacitve_bytes);
    return hcacitve_bytes;
}

void _dprx_timing_to_measure_timing(
    DPRX_TIMING_INFO_T*     p_dprx_timing,
    MEASURE_TIMING_T*       p_measure_timing,
    DP_PPS_DATA_T*          p_pps_data
    )
{
    if (p_measure_timing==NULL)
        return ;

    memset(p_measure_timing, 0, sizeof(MEASURE_TIMING_T));

    if (p_dprx_timing)
    {
        p_measure_timing->h_total          = p_dprx_timing->htotal;
        p_measure_timing->h_act_len        = p_dprx_timing->hact;
        p_measure_timing->h_act_sta        = p_dprx_timing->hstart;
        p_measure_timing->h_freq           = p_dprx_timing->hfreq_hz;
        p_measure_timing->IHSyncPulseCount = p_dprx_timing->hsync;

        p_measure_timing->v_total          = p_dprx_timing->vtotal;
        p_measure_timing->v_act_len        = p_dprx_timing->vact;
        p_measure_timing->v_act_sta        = p_dprx_timing->vstart;
        p_measure_timing->v_freq           = p_dprx_timing->vfreq_hz_x100 * 10;
        p_measure_timing->IVSyncPulseCount = p_dprx_timing->vsync;

        p_measure_timing->polarity         = 0;
        //p_measure_timing->mode_id        = ???;
        //p_measure_timing->modetable_index= ???;
        p_measure_timing->is_interlace     = p_dprx_timing->is_interlace;

        //p_measure_timing->mode           = ???;
        p_measure_timing->colordepth       = _to_hdmi_color_depth(p_dprx_timing->color_depth);
        p_measure_timing->colorspace       = _to_hdmi_color_space(p_dprx_timing->color_space);
        p_measure_timing->colorimetry      = HDMI_COLORIMETRY_NOSPECIFIED; //_to_hdmi_colorimetry(p_dprx_timing->color_space);
        p_measure_timing->pixel_repeat     = 0;  // DSC dies not support
        //p_measure_timing->hvf            = ???;
        //p_measure_timing->h3dformat      = 0;
        //p_measure_timing->tmds_clk_b     = 0;
        p_measure_timing->pll_pixelclockx1024 = p_dprx_timing->htotal * p_dprx_timing->vtotal * p_dprx_timing->vfreq_hz_x100 / 100;   // ptiming->h_total * ptiming->v_total* ptiming->v_freq / 1024;

        // only for frame packing
        //p_measure_timing->v_active_space1   = ????;
        //p_measure_timing->v_active_space2   = ????;
        p_measure_timing->info_vic            = 0; // fix to VIC 0
        p_measure_timing->run_vrr             = (p_dprx_timing->drr_mode!=DP_DRR_MODE_FRR) ? 1 : 0;
        p_measure_timing->run_dsc             = p_dprx_timing->is_dsc;;
        p_measure_timing->fva_factor          = 0;   // DP does not support FVA
        p_measure_timing->pixel_mode          = _to_hdmi_pixel_mode(p_dprx_timing->pixel_mode);
        //p_measure_timing->single_p_mode_thd = ????;
        p_measure_timing->is_v_drop           = 0;

        // for DSC
        if (p_dprx_timing->is_dsc && p_pps_data)
        {
            // update color info
            unsigned short bpp = ((p_pps_data->pps[4] & 0x3) <<8) + (p_pps_data->pps[5]);
            unsigned char bpc = (p_pps_data->pps[3]>>4) & 0xF;               // PPS3[7:4]
            p_measure_timing->dsc_version_minor = p_pps_data->pps[0] & 0xF;  // PPS0[3:0]
            p_measure_timing->convert_rgb = (p_pps_data->pps[4]>>4) & 0x1;   // PPS4[4]
            p_measure_timing->simple_422  = (p_pps_data->pps[4]>>3) & 0x1;   // PPS4[3]
            p_measure_timing->native_420  = (p_pps_data->pps[88]) & 0x1;     // PPS88[0]
            p_measure_timing->native_422  = (p_pps_data->pps[88]>>1) & 0x1;  // PPS88[1]

            // update color space by pps
            if (p_measure_timing->convert_rgb)
                p_measure_timing->colorspace = COLOR_RGB;
            else if (p_measure_timing->simple_422 || p_measure_timing->native_422)
                p_measure_timing->colorspace = COLOR_YUV422;
            else if (p_measure_timing->native_420)
                p_measure_timing->colorspace = COLOR_YUV420;
            else
                p_measure_timing->colorspace = COLOR_YUV444;

            // update color depth by pps
            switch(bpc)
            {
            case 0x0: p_measure_timing->colordepth  = HDMI_COLOR_DEPTH_16B; bpc = 16; break;
            case 0x8: p_measure_timing->colordepth  = HDMI_COLOR_DEPTH_8B;  break;
            case 0xA: p_measure_timing->colordepth  = HDMI_COLOR_DEPTH_10B; break;
            case 0xC: p_measure_timing->colordepth  = HDMI_COLOR_DEPTH_12B; break;
            default:  p_measure_timing->colordepth  = HDMI_COLOR_DEPTH_8B;  bpc = 8; break;
            }

            // update CVTEM info
            p_measure_timing->cvtem_h_front = p_measure_timing->h_total - p_measure_timing->h_act_len - p_measure_timing->h_act_sta;
            p_measure_timing->cvtem_h_sync  = p_measure_timing->IHSyncPulseCount;
            p_measure_timing->cvtem_h_back  = p_measure_timing->h_act_sta - p_measure_timing->IHSyncPulseCount;

            DP_VFE_INFO("cvtem info : h_front = %d, h_sync=%d, h_back=%d, bpp=%d, bpc=%d\n",
                 p_measure_timing->cvtem_h_front,  p_measure_timing->cvtem_h_sync, p_measure_timing->cvtem_h_back, bpp, bpc);

            p_measure_timing->cvtem_hcative_bytes  = _compute_hc_active_bytes(p_pps_data);

            p_measure_timing->is_dsc_to_tx  = 1; // notify HDMI TX it is compressed video stream
        }
    }
}

/*------------------------------------------------------
 * Func : drvif_Dprx_GetPortRawTimingInfoForHdmiTX
 *
 * Desc : get DPRX Raw Timing for HDMI TX
 *
 * Parm : ch:
 *        p_timing_info : raw timing
 *
 * Retn : 0 : fail, 1 : success
 *------------------------------------------------------*/
DPRX_bool drvif_Dprx_GetPortRawTimingInfoForHdmiTX(
    unsigned char           ch,
    MEASURE_TIMING_T*       p_timing_info
    )
{
    dprx_dev_t* p_adp = _vfe_dprx_drv_get_dprx_adapter(ch);
    DPRX_TIMING_INFO_T dprx_timing;
    DP_PPS_DATA_T pps;

    if ((p_timing_info==NULL)||(p_adp==NULL))
        return 0;

    memset(p_timing_info, 0, sizeof(MEASURE_TIMING_T));
    memset(&dprx_timing, 0, sizeof(dprx_timing));
    memset(&pps, 0, sizeof(pps));

    if (dprx_adp_get_stream_bypass_timing(p_adp, &dprx_timing)<0)
        return 0;

    if (dprx_timing.is_dsc)
        dprx_adp_get_pps_data(p_adp, &pps);

    _dprx_timing_to_measure_timing(&dprx_timing, p_timing_info, &pps);

    return 1;
}

EXPORT_SYMBOL(drvif_Dprx_GetPortRawTimingInfoForHdmiTX);
