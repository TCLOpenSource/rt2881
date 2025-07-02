#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/time.h>

#include <rtk_kdriver/quick_show/source_monitor.h>

static struct sm_source_info curr_sm_info;
static struct sm_source_info prev_sm_info;
static KADP_DISP_PANEL_INFO_T panel_info;
static KADP_DISP_PANEL_SIZE_T panel_size;
static VSC_CONNECT_PARA_T connectPara;
static VSC_SET_FRAME_REGION_T outframeregion;
static VSC_SET_FRAME_REGION_T inframeregion;
static VSC_WINBLANK_PARA_T winblankpara;
static vfe_hdmi_connect_state_t hdmi_con_state;
static KADP_VFE_DPRX_TIMING_INFO_T ret_timing_info;
static unsigned char bval;
static HAL_AUDIO_RHAL_INFO_T rhalinfo;
static DPRX_DRM_INFO_T dp_drm_info;
static vfe_hdmi_drm_t hdmi_drm_info;

bool sm_active = false;
EXPORT_SYMBOL(sm_active);
struct task_struct *sm_thread;
EXPORT_SYMBOL(sm_thread);
unsigned long sm_dp_eEOTFtype;
EXPORT_SYMBOL(sm_dp_eEOTFtype);
unsigned long sm_hdmi_eEOTFtype;
EXPORT_SYMBOL(sm_hdmi_eEOTFtype);

/*
 * sm_state : source monitor state
 * 0 : There is timing from source
 * 1 : Polling source and try to get timing
 */
static atomic_t sm_state = ATOMIC_INIT(0);

static inline HAL_AUDIO_RESOURCE_T SM_AUDIO_DP(int port)
{
    return HAL_AUDIO_RESOURCE_DP;
}

static inline HAL_AUDIO_RESOURCE_T SM_AUDIO_HDMI(int port)
{
    switch (port) {
        case 0:
            return HAL_AUDIO_RESOURCE_HDMI0;
            break;
        case 1:
            return HAL_AUDIO_RESOURCE_HDMI1;
            break;
        case 2:
            return HAL_AUDIO_RESOURCE_HDMI2;
            break;
        case 3:
            return HAL_AUDIO_RESOURCE_HDMI3;
            break;
        default:
            break;
    }
    return HAL_AUDIO_RESOURCE_HDMI0;
}

static int hdmi_disconnect_close(int port)
{
    int ret = 0;

    /* HDMI disconnect */
    ret = sm_vfe_hdmi_disconnect(port);
    if (ret) {
        rtd_pr_sm_err("Fail sm_vfe_hdmi_disconnect, ret = %d\n", ret);
        goto exit;
    }

    /* HDMI close */
    ret = sm_vfe_hdmi_close();
    if (ret) {
        rtd_pr_sm_err("Fail sm_vfe_hdmi_close, ret = %d\n", ret);
        goto exit;
    }

exit:
    return ret;
}

static int dp_disconnect_close(int port)
{
    int ret = 0;

    /* DP discoonect */
    ret = dprx_vfe_ctrl_handler(VFE_DPRX_CTRL_DISCONNECT(port));
    if (ret) {
        rtd_pr_sm_err("Fail VFE_DPRX_CTRL_DISCONNECT, ret = %d\n", ret);
        goto exit;
    }

    /* DP close */
    ret = dprx_vfe_ctrl_handler(DPRX_VFE_CTRL_CLOSE);
    if (ret) {
        rtd_pr_sm_err("Fail DPRX_VFE_CTRL_CLOSE, ret = %d\n", ret);
        goto exit;
    }

exit:
    return ret;
}

static int sm_vfe_dp(int port, bool connect)
{
    int ret = 0, timeout = 0, start = 0;

    /* DP open and connect */
    if (!connect) {
        ret = dprx_vfe_ctrl_handler(VFE_DPRX_CTRL_OPEN);
        if (ret) {
            rtd_pr_sm_err("Fail VFE_DPRX_CTRL_OPEN, ret = %d\n", ret);
            goto exit;
        }

        ret = dprx_vfe_ctrl_handler(VFE_DPRX_CTRL_CONNECT(port));
        if (ret) {
            rtd_pr_sm_err("Fail VFE_DPRX_CTRL_CONNECT, port = %d, ret = %d\n", port, ret);
            goto exit;
        }
    }

    /* Try to get DP timing */
    memset(&outframeregion, 0, sizeof(VSC_SET_FRAME_REGION_T));
    start = ktime_get_ns();
    do {
        msleep(100);
        timeout += 1;

        memset(&ret_timing_info, 0, sizeof(KADP_VFE_DPRX_TIMING_INFO_T));
        sm_vfe_dprx_get_timing_info(&ret_timing_info, port);
        if (ret_timing_info.vact == 0 || ret_timing_info.hact == 0) {
            continue;
        } else {
            curr_sm_info.source = DP_SOURCE;
            curr_sm_info.port = port;
            inframeregion.inregion.w = ret_timing_info.hact;
            inframeregion.inregion.h = ret_timing_info.is_interlace ?
                (ret_timing_info.vact * 2) : ret_timing_info.vact;
            rtd_pr_sm_notice("Success getting timing inforation, %d [ms]\n", ktime_get_ns() - start);
            goto exit;
        }
    } while (timeout < 100);

    if (!connect) {
        /* No timing, DP disconnect and close */
        ret = dp_disconnect_close(port);
        if (ret) {
            rtd_pr_sm_err("Fail dp_disconnect_close, ret = %d\n", ret);
        }
    }

    rtd_pr_sm_err("Can't get timing, DP \n", port);
    ret = -1;

exit:
    return ret;
}

static int sm_vfe_hdmi(int port, bool connect)
{
    int ret = 0, timeout = 0, start = 0;

    vfe_hdmi_timing_info_t hdmitiminginfo;
    memset(&hdmitiminginfo, 0, sizeof(vfe_hdmi_timing_info_t));
    hdmitiminginfo.port = port;

    /* HDMI open and connect */
    if (!connect) {
        ret = sm_vfe_hdmi_open();
        if (ret) {
            rtd_pr_sm_err("Fail sm_vfe_hdmi_open\n");
            goto exit;
        }

        ret = sm_vfe_hdmi_connect(hdmitiminginfo.port);
        if (ret) {
            rtd_pr_sm_err("Fail sm_vfe_hdmi_connect, ret = %d\n", ret);
            goto exit;
        }
    }

    /* Try to get HDMI Timing */
    memset(&outframeregion, 0, sizeof(VSC_SET_FRAME_REGION_T));
    start = ktime_get_ns();
    do {
        msleep(100);
        timeout += 1;

        ret = sm_vfe_hdmi_get_timing_info(&hdmitiminginfo);
        if (ret) {
            rtd_pr_sm_err("Fail sm_vfe_hdmi_get_timing_info, ret = %d\n", ret);
        } else if ((0 == hdmitiminginfo.active.w) || (0 == hdmitiminginfo.active.h)) {
            memset(&hdmitiminginfo, 0, sizeof(vfe_hdmi_timing_info_t));
            hdmitiminginfo.port = port;
        } else {
            curr_sm_info.source = HDMI_SOURCE;
            curr_sm_info.port = hdmitiminginfo.port;
            inframeregion.inregion.w = hdmitiminginfo.active.w;
            /* interlace would be 0 */
            inframeregion.inregion.h = hdmitiminginfo.scan_type ?
                                        hdmitiminginfo.active.h : hdmitiminginfo.active.h * 2;
            rtd_pr_sm_notice("Success getting timing inforation, %d [ns]\n", ktime_get_ns() - start);
            goto exit;
        }
    } while (timeout < 100);

    if (!connect) {
        /* No timing, HDMI disconnect and close */
        ret = hdmi_disconnect_close(port);
        if (ret) {
            rtd_pr_sm_err("Fail hdmi_disconnect_close, ret = %d\n", ret);
        }
    }

    rtd_pr_sm_err("Can't get timing, HDMI \n", hdmitiminginfo.port);
    ret = -1;

exit:
    return ret;
}

static int audio_disconnect(void)
{
    int ret = 0;

    /* Audio disconnect */
    if (curr_sm_info.source != NONE) {
        memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
        rhalinfo.adecindex = HAL_AUDIO_ADEC0;
        rhalinfo.type = SET_STOP_DECODING;
        ret = rhal_set_ioctl(rhalinfo);
        if (ret) {
            rtd_pr_sm_err("Fail rhal_set_ioctl SET_STOP_DECODING, ret = %d\n", ret);
            goto exit;
        }

        memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
        rhalinfo.adecindex = HAL_AUDIO_ADEC0;
        rhalinfo.enable = TRUE;
        rhalinfo.type = SET_DECODER_INPUT_MUTE;
        ret = rhal_set_ioctl(rhalinfo);
        if (ret) {
            rtd_pr_sm_err("Fail rhal_set_ioctl SET_DECODER_INPUT_MUTE, ret = %d\n", ret);
            goto exit;
        }

        memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
        rhalinfo.inputConnect = HAL_AUDIO_RESOURCE_ADEC0;
        rhalinfo.type = SET_SE_DISCONNECT;
        ret = rhal_set_ioctl(rhalinfo);
        if (ret) {
            rtd_pr_sm_err("Fail rhal_set_ioctl SET_SE_DISCONNECT, ret = %d\n", ret);
            goto exit;
        }

        memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
        rhalinfo.adecinfo.prevTPInputPort = HAL_AUDIO_RESOURCE_ADEC0;
        rhalinfo.adecinfo.curTPInputPort = curr_sm_info.source == HDMI_SOURCE ?
            SM_AUDIO_HDMI(curr_sm_info.port) : SM_AUDIO_DP(curr_sm_info.port);
        rhalinfo.type = SET_ADEC_DISCONNECT;
        ret = rhal_set_ioctl(rhalinfo);
        if (ret) {
            rtd_pr_sm_err("Fail rhal_set_ioctl SET_ADEC_DISCONNECT, ret = %d\n", ret);
            goto exit;
        }

        memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
        rhalinfo.type = curr_sm_info.source == HDMI_SOURCE ?
             SET_HDMI_DISCONNECT_PORT : SET_DP_DISCONNECT;
        rhalinfo.hdmiIndex = curr_sm_info.source == HDMI_SOURCE ?
             curr_sm_info.port : 0;
        ret = rhal_set_ioctl(rhalinfo);
        if (ret) {
            rtd_pr_sm_err("Fail rhal_set_ioctl Source Disconnect, ret = %d\n", ret);
            goto exit;
        }

        memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
        rhalinfo.type = curr_sm_info.source == HDMI_SOURCE ? SET_HDMI_CLOSE_PORT : SET_DP_CLOSE;
        rhalinfo.hdmiIndex = curr_sm_info.source == HDMI_SOURCE ?
             curr_sm_info.port : 0;
        ret = rhal_set_ioctl(rhalinfo);
        if (ret) {
            rtd_pr_sm_err("Fail rhal_set_ioctl Source Open, ret = %d\n", ret);
            goto exit;
        }
    }

exit:
    return ret;
}

static int sm_disconnect(void)
{
    int ret = 0;

    if (curr_sm_info.source == NONE)
        goto exit;

    ret = audio_disconnect();
    if (ret) {
        rtd_pr_sm_err("Fail audio_disconnect, ret = %d\n", ret);
        goto exit;
    }

    if (curr_sm_info.source == HDMI_SOURCE) {
        ret = hdmi_disconnect_close(curr_sm_info.port);
        if (ret) {
            rtd_pr_sm_err("Fail hdmi_disconnect_close, ret = %d\n", ret);
            goto exit;
        }
    } else {
        ret = dp_disconnect_close(curr_sm_info.port);
        if (ret) {
            rtd_pr_sm_err("Fail dp_disconnect_close, ret = %d\n", ret);
            goto exit;
        }
    }

exit:
    curr_sm_info.source = NONE;
    return ret;
}

static int sm_audio(void)
{
    int ret = 0;

    /* Audio open another source */
    memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
    rhalinfo.type = curr_sm_info.source == HDMI_SOURCE ? SET_HDMI_OPEN_PORT : SET_DP_OPEN;
    rhalinfo.hdmiIndex = curr_sm_info.source == HDMI_SOURCE ?
         curr_sm_info.port : 0;
    ret = rhal_set_ioctl(rhalinfo);
    if (ret) {
        rtd_pr_sm_err("Fail rhal_set_ioctl Source Open, ret = %d\n", ret);
        goto exit;
    }

    /* Source connect, HDMI or DP */
    memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
    rhalinfo.type = curr_sm_info.source == HDMI_SOURCE ?
         SET_HDMI_CONNECT_PORT : SET_DP_CONNECT;
    rhalinfo.hdmiIndex = curr_sm_info.source == HDMI_SOURCE ?
         curr_sm_info.port : 0;
    ret = rhal_set_ioctl(rhalinfo);
    if (ret) {
        rtd_pr_sm_err("Fail rhal_set_ioctl Source Connect, ret = %d\n", ret);
        goto exit;
    }

    /* ADEC connect */
    memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
    rhalinfo.adecinfo.curTPInputPort = curr_sm_info.source == HDMI_SOURCE ?
            SM_AUDIO_HDMI(curr_sm_info.port) : SM_AUDIO_DP(curr_sm_info.port);
    rhalinfo.adecinfo.prevTPInputPort = HAL_AUDIO_RESOURCE_ADEC0;
    rhalinfo.type = SET_ADEC_CONNECT;
    ret = rhal_set_ioctl(rhalinfo);
    if (ret) {
        rtd_pr_sm_err("Fail rhal_set_ioctl SET_ADEC_CONNECT, ret = %d\n", ret);
        goto exit;
    }

    /* SE connect */
    memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
    rhalinfo.inputConnect = HAL_AUDIO_RESOURCE_ADEC0;
    rhalinfo.type = SET_SE_CONNECT;
    ret = rhal_set_ioctl(rhalinfo);
    if (ret) {
        rtd_pr_sm_err("Fail rhal_set_ioctl SET_SE_CONNECT, ret = %d\n", ret);
        goto exit;
    }

    memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
    rhalinfo.adecindex = HAL_AUDIO_ADEC0;
    rhalinfo.enable = TRUE;
    rhalinfo.type = SET_DECODER_INPUT_MUTE;
    ret = rhal_set_ioctl(rhalinfo);
    if (ret) {
        rtd_pr_sm_err("Fail rhal_set_ioctl SET_DECODER_INPUT_MUTE, ret = %d\n", ret);
        goto exit;
    }

    memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
    rhalinfo.adecindex = HAL_AUDIO_ADEC0;
    rhalinfo.type = SET_MAIN_DECODER_OUTPUT;
    ret = rhal_set_ioctl(rhalinfo);
    if (ret) {
        rtd_pr_sm_err("Fail rhal_set_ioctl SET_MAIN_DECODER_OUTPUT, ret = %d\n", ret);
        goto exit;
    }

    memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
    rhalinfo.adecindex = HAL_AUDIO_ADEC0;
    rhalinfo.audiotype = HAL_AUDIO_SRC_TYPE_PCM;
    rhalinfo.type = SET_START_DECODING;
    ret = rhal_set_ioctl(rhalinfo);
    if (ret) {
        rtd_pr_sm_err("Fail rhal_set_ioctl SET_START_DECODING, ret = %d\n", ret);
        goto exit;
    }

    memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
    rhalinfo.adecindex = HAL_AUDIO_ADEC0;
    rhalinfo.enable = FALSE;
    rhalinfo.type = SET_DECODER_INPUT_MUTE;
    ret = rhal_set_ioctl(rhalinfo);
    if (ret) {
        rtd_pr_sm_err("Fail rhal_set_ioctl SET_DECODER_INPUT_MUTE, ret = %d\n", ret);
        goto exit;
    }

exit:
    return ret;
}

static inline void change_qs_state_reg(void)
{
    unsigned int val, ori_val;
    ori_val = val = rtd_inl(QS_STATUS_reg);
    if (curr_sm_info.source == DP_SOURCE) {
        val |= QS_STATUS_dp_mask | QS_STATUS_typec_mask;
        val &= ~QS_STATUS_hdmi_mask;
        val &= ~QS_STATUS_portnum_mask;
        val |= curr_sm_info.port << QS_STATUS_portnum_shift;
    } else if (curr_sm_info.source == HDMI_SOURCE) {
        val |= QS_STATUS_hdmi_mask;
        val &= ~(QS_STATUS_typec_mask | QS_STATUS_dp_mask);
        val &= ~QS_STATUS_portnum_mask;
        val |= curr_sm_info.port << QS_STATUS_portnum_shift;
    } else {
        val &= ~(QS_STATUS_typec_mask | QS_STATUS_dp_mask);
        val &= ~QS_STATUS_hdmi_mask;
        val &= ~QS_STATUS_portnum_mask;
    }

    rtd_outl(QS_STATUS_reg, val);
    rtd_pr_sm_notice("Change Quick Show state register : %08x --> %08x\n", ori_val, val);
}

static int sm_scaler_disconnect(void)
{
    static bool reclaim = false;
    int ret = 0;

    /* Scaler disconnect */
    memset(&connectPara, 0, sizeof(VSC_CONNECT_PARA_T));
    connectPara.vsc_input.type = prev_sm_info.source == HDMI_SOURCE ?
        KADP_VSC_INPUTSRC_HDMI : KADP_VSC_INPUTSRC_DP;
    ret = sm_vsc_disconnect(connectPara);
    if (ret) {
        rtd_pr_sm_err("Fail sm_svc_disconnect, ret = %d\n", ret);
        return ret;
    } else if (!reclaim) {
        rtd_pr_sm_notice("Reclaim vbm using by QS\n");
        do_dvr_reclaim(4);
        reclaim = true;
    }
    return ret;
}

static int sm_scaler_show_source(void)
{
    int ret = 0;

    ret = sm_scaler_disconnect();
    if (ret) {
        rtd_pr_sm_err("Fail sm_scaler_disconnect, ret = %d\n", ret);
        goto exit;
    }

    memset(&winblankpara, 0, sizeof(VSC_WINBLANK_PARA_T));
    memset(&outframeregion, 0, sizeof(VSC_SET_FRAME_REGION_T));
    memset(&panel_size, 0, sizeof(KADP_DISP_PANEL_SIZE_T));

    /* Screen block */
    winblankpara.winblankbonoff = 1;
    ret = sm_vsc_set_win_blank(winblankpara);
    if (ret) {
        rtd_pr_sm_err("Fail enable sm_vsc_set_win_blank, ret = %d\n", ret);
        goto exit;
    }

    /* Scaler connect */
    memset(&connectPara, 0, sizeof(VSC_CONNECT_PARA_T));
    connectPara.vsc_input.type = curr_sm_info.source == HDMI_SOURCE ?
        KADP_VSC_INPUTSRC_HDMI : KADP_VSC_INPUTSRC_DP;
    ret = sm_vsc_connect(connectPara);
    if (ret) {
        rtd_pr_sm_err("Fail sm_vsc_connect, ret = %d\n", ret);
        goto exit;
    }

    /* Set input source region */
    ret = sm_vsc_set_input_region(inframeregion);
    if (ret) {
        rtd_pr_sm_err("Fail sm_vsc_set_input_region, ret = %d\n", ret);
        goto exit;
    }

    /* Get panel information Full HD, 2K, 4K */
    HAL_VBE_DISP_GetPanelSize(&panel_size);
    rtd_pr_sm_info("width = %d, height = %d\n", panel_size.DISP_WIDTH, panel_size.DISP_HEIGHT);
    outframeregion.inregion.w = panel_size.DISP_WIDTH;
    outframeregion.inregion.h = panel_size.DISP_HEIGHT;

    /* Set output source region */
    ret = sm_vsc_set_output_region(outframeregion);
    if (ret) {
        rtd_pr_sm_err("Fail sm_vsc_set_output_region, ret = %d\n", ret);
        goto exit;
    }

    /* Screen unblock */
    winblankpara.winblankbonoff = 0;
    ret = sm_vsc_set_win_blank(winblankpara);
    if (ret) {
        rtd_pr_sm_err("Fail disable sm_vsc_set_win_blank, ret = %d\n", ret);
        goto exit;
    }

exit:
    return ret;
}

static void sm_start(void)
{
    int ret = 0, port = 0;
    memset(&hdmi_con_state, 0, sizeof(vfe_hdmi_connect_state_t));

    rtd_pr_sm_notice("Polling Source\n");

/* Keep polling until kthread stop or get source timing */
polling:
    /* Polling HDMI Port 0 ~ Port 3 */
    for (hdmi_con_state.port = 0; hdmi_con_state.port < HDMI_MAX_PORT; hdmi_con_state.port++) {
        if (kthread_should_stop())
            goto exit;

        /* Check HDMI plugin state */
        vfe_hdmi_drv_get_connection_state(&hdmi_con_state);
        if (!hdmi_con_state.state || hdmi_con_state.port >= HDMI_MAX_PORT) {
            /*
             * TODO : Sometime the port is bigger than Max port. I don't know why
             *        Therefore, I add the condition to avoid this sitution.
             */
            continue;
        } else {
            rtd_pr_sm_notice("Connect HDMI Port %d\n", hdmi_con_state.port);
        }

        /* HDMI connect and get timing */
        ret = sm_vfe_hdmi(hdmi_con_state.port, false);
        if (ret) {
            rtd_pr_sm_err("Fail sm_vfe_hdmi, ret = %d\n", ret);
        } else {
            rtd_pr_sm_notice("Success sm_vfe_hdmi, Source, port : HDMI, %d\n",
                     hdmi_con_state.port);
            goto show_source;
        }
    }

    /* Polling DP Port 0 ~ Port 1 */
    for (port = 0, bval = 0; port < DP_MAX_PORT; port++) {
        if (kthread_should_stop())
            goto exit;

        /* Check DP plugin state */
        if (vfe_dprx_drv_get_connection_state(port, &bval) == DPRX_DRV_NO_ERR) {
            if (!bval) {
                continue;
            } else {
                rtd_pr_sm_notice("Connect DP Port %d\n", port);
            }
        } else {
            rtd_pr_sm_err("Fail vfe_dprx_drv_get_connection_state\n");
            continue;
        }

        /* DP connect and get timing */
        ret = sm_vfe_dp(port, false);
        if (ret) {
            rtd_pr_sm_err("Fail sm_vfe_dp, ret = %d\n", ret);
        } else {
            rtd_pr_sm_notice("Success sm_vfe_dp, Source, port : DP, %d\n",
                     port);
            goto show_source;
        }
    }

    if (curr_sm_info.source == NONE) {
        // rtd_pr_sm_notice("[SM] No detect any source, keep polling.\n");
        msleep(100);
        goto polling;
    }

show_source:
    /* Show source */
    ret = sm_scaler_show_source();
    if (ret) {
        rtd_pr_sm_err("Fail sm_scaler_show_source, ret = %d\n", ret);
        curr_sm_info.source = NONE;
        goto exit;
    }

    /* Make some noise */
    ret = sm_audio();
    if (ret) {
        rtd_pr_sm_err("Fail sm_audio\n");
    }

    // Get DRM info which include HDR info
    if (curr_sm_info.source == DP_SOURCE && dp_drm_info.nLength == 0) {
        ret = drvif_Dprx_GetDrmInfoFrame(&dp_drm_info);
        if (!ret) {
            rtd_pr_sm_err("Fail drvif_Dprx_GetDrmInfoFrame\n");
        } else {
            sm_dp_eEOTFtype = dp_drm_info.eEOTFtype;
            rtd_pr_sm_notice("sm_dp_eEOTFtype : %lu\n", sm_dp_eEOTFtype);
        }
    }

    if (curr_sm_info.source == HDMI_SOURCE && hdmi_drm_info.len == 0) {
        ret = vfe_hdmi_drv_get_drm_info(&hdmi_drm_info);
        if (ret) {
            rtd_pr_sm_err("Fail vfe_hdmi_drv_get_drm_info, ret = %d\n", ret);
        } else {
            sm_hdmi_eEOTFtype = hdmi_drm_info.eEOTFtype;
            rtd_pr_sm_notice("sm_hdmi_eEOTFtype : %lu\n", sm_hdmi_eEOTFtype);
        }
    }

    rtd_pr_sm_info("Source Monitor State = Sleep\n");
    atomic_set(&sm_state, 0);
    prev_sm_info.source =  curr_sm_info.source;
    prev_sm_info.port =  curr_sm_info.port;
    change_qs_state_reg();

exit:
    return;
}

/*
 * sm_source_remove - check if the source is still connecting or not
 * return : true (remove)
 *          false (still connecting and getting timing)
 */

static bool sm_source_remove(void)
{
    bool remove = false;
    int ret = 0;

    /* Check if the source is still connecting */
    if (curr_sm_info.source == HDMI_SOURCE) {
        memset(&hdmi_con_state, 0, sizeof(vfe_hdmi_connect_state_t));
        hdmi_con_state.port = curr_sm_info.port;
        vfe_hdmi_drv_get_connection_state(&hdmi_con_state);
        if (!hdmi_con_state.state || hdmi_con_state.port >= HDMI_MAX_PORT) {
            remove = true;
            goto exit;
        } else {
            rtd_pr_sm_notice("Still connection, try to get timing, HDMI %d\n", hdmi_con_state.port);
        }

        /* HDMI connect and get timing */
        ret = sm_vfe_hdmi(hdmi_con_state.port, true);
        if (ret) {
            rtd_pr_sm_err("Fail get HDMI timing\n");
            remove = true;
            goto exit;
        } else {
            rtd_pr_sm_notice("Success sm_vfe_hdmi, Source, port : HDMI, %d\n",
                     hdmi_con_state.port);
            goto show_source;
        }
    }

    /* Check if the source is still connecting */
    if (prev_sm_info.source == DP_SOURCE) {
        if (vfe_dprx_drv_get_connection_state(curr_sm_info.port, &bval) == DPRX_DRV_NO_ERR) {
            if (!bval) {
                remove = true;
                goto exit;
            } else {
                rtd_pr_sm_notice("Still connection, try to get timing, DP %d\n", curr_sm_info.port);
            }
        } else {
            rtd_pr_sm_err("Fail vfe_dprx_drv_get_connection_state\n");
            remove = true;
            goto exit;
        }

        /* DP connect and get timing */
        ret = sm_vfe_dp(curr_sm_info.port, true);
        if (ret) {
            rtd_pr_sm_err("Fail sm_vfe_dp, ret = %d\n");
            remove = true;
            goto exit;
        } else {
            rtd_pr_sm_notice("Success sm_vfe_dp, Source, port : DP, %d\n",
                     curr_sm_info.port);
            goto show_source;
        }
    }

show_source:
    ret = sm_scaler_show_source();
    if (ret) {
        rtd_pr_sm_err("Fail sm_scaler_show_source\n");
        remove = true;
        goto exit;
    }

    atomic_set(&sm_state, 0);
    rtd_pr_sm_notice("Keep original source\n");

exit:
    return remove;
}

static int sm_standby(void *arg)
{
    int ret = 0;
    rtd_pr_sm_notice("Ready to source monitor\n");
    sm_active = true;

    while (!kthread_should_stop()) {
        /* Wait for Online Measurement ISR */
        switch (atomic_read(&sm_state)) {
            case 0:
                msleep(500);
                break;
            case 1:
                rtd_pr_sm_notice("Start to source monitor\n");

                if (!sm_source_remove()) {
                    break;
                }

                if (sm_disconnect()) {
                    rtd_pr_sm_err("Fail sm_disconnect\n");
                    break;
                }

                change_qs_state_reg();
                sm_start();
                break;
        }
    }

    if (curr_sm_info.source == NONE) {
        set_QS_handle_key(false);
        ret = sm_scaler_disconnect();
    }

    rtd_pr_sm_notice("End to source monitor\n");
    sm_active = false;
    return ret;
}

static int __init rtk_sm_init(void)
{
    int ret = 0;

    sm_state_vfe = &sm_state;
    sm_state_vsc = &sm_state;

    memset(&dp_drm_info, 0, sizeof(DPRX_DRM_INFO_T));
    memset(&hdmi_drm_info, 0, sizeof(vfe_hdmi_drm_t));

    /* Check QS active or not */
    if(!is_QS_active()) {
        rtd_pr_sm_notice("Quick Show it not active, exit source monitor\n");
        goto exit;
    } else {
        if (is_QS_hdmi_enable()) {
            /* Init QS HDMI, DP init, and enable HPD */
            curr_sm_info.source = prev_sm_info.source = HDMI_SOURCE;
            curr_sm_info.port = prev_sm_info.port = get_QS_portnum();
            ret = sm_vfe_qs_hdmi_init();
            if (ret) {
                rtd_pr_sm_err("Fail sm_vfe_qs_hdmi_init, ret = %d\n", ret);
                goto exit;
            }

            ret = dprx_vfe_ctrl_handler(VFE_DPRX_CTRL_INIT);
            if (ret) {
                rtd_pr_sm_err("Fail VFE_DPRX_CTRL_INIT, ret = %d\n", ret);
                goto exit;
            }

            ret = dprx_vfe_ctrl_handler(VFE_DPRX_CTRL_OPEN);
            if (ret) {
                rtd_pr_sm_err("Fail VFE_DPRX_CTRL_OPEN, ret = %d\n", ret);
                goto exit;
            }

            ret = vfe_dprx_drv_enable_hpd(true);
            if (ret) {
                rtd_pr_sm_err("Fail vfe_dprx_drv_enable_hpd, ret = %d\n", ret);
                goto exit;
            }

            // Get DRM info which include HDR info
            ret = vfe_hdmi_drv_get_drm_info(&hdmi_drm_info);
            if (ret) {
                rtd_pr_sm_err("Fail vfe_hdmi_drv_get_drm_info, ret = %d\n", ret);
            } else {
                sm_hdmi_eEOTFtype = hdmi_drm_info.eEOTFtype;
                rtd_pr_sm_notice("sm_hdmi_eEOTFtype : %lu\n", sm_hdmi_eEOTFtype);
            }

            rtd_pr_sm_notice("start source monitor, current source, port : HDMI, %d\n",
                     curr_sm_info.port);
        } else if (is_QS_dp_enable()) {
            /* Init QS DP, HDMI init and open */
            curr_sm_info.source = prev_sm_info.source = DP_SOURCE;
            curr_sm_info.port = prev_sm_info.port = get_QS_portnum();
            ret = dprx_vfe_ctrl_handler(VFE_DPRX_CTRL_QS_INIT);
            if (ret) {
                rtd_pr_sm_err("Fail VFE_DPRX_CTRL_QS_INIT, ret = %d\n", ret);
                goto exit;
            }

            // Initialize and connect to HDMI1
            ret = sm_vfe_qs_hdmi_init();
            if (ret) {
                rtd_pr_sm_err("Fail sm_vfe_hdmi_init\n");
                goto exit;
            }

            // disconnect hdmi1 and keep open done status
            ret = sm_vfe_hdmi_disconnect(0);
            if (ret) {
                rtd_pr_sm_err("Fail sm_vfe_hdmi_disconnect\n");
                goto exit;
            }

            // Get DRM info which include HDR info
            ret = drvif_Dprx_GetDrmInfoFrame(&dp_drm_info);
            if (!ret) {
                rtd_pr_sm_err("Fail drvif_Dprx_GetDrmInfoFrame\n");
            } else {
                sm_dp_eEOTFtype = dp_drm_info.eEOTFtype;
                rtd_pr_sm_notice("sm_dp_eEOTFtype : %lu\n", sm_dp_eEOTFtype);
            }

            rtd_pr_sm_notice("start source monitor, current source, port : DP, %d\n",
                     curr_sm_info.port);
        } else {
            rtd_pr_sm_err("Fail to get QS source info.\n");
            goto exit;
        }

        /* Audio QS Init */
        memset(&rhalinfo, 0, sizeof(HAL_AUDIO_RHAL_INFO_T));
        rhalinfo.eSifType = HAL_AUDIO_SIF_BTSC_US_SELECT;
        rhalinfo.setSoundSystem = (HAL_AUDIO_SIF_SOUNDSYSTEM_T)0xFF;
        rhalinfo.type = SET_SIF_BAND_SETUP;
        ret = rhal_set_ioctl(rhalinfo);
        if (ret) {
            rtd_pr_sm_err("Fail rhal_set_ioctl QS Init, ret = %d\n", ret);
            goto exit;
        }

        /* VPQ init */
        sm_vpq_extern_init();

        /* Scaler init and open, actully it's Scaler QS init */
        ret = sm_vsc_init();
        if (ret) {
            rtd_pr_sm_err("Fail sm_svc_init\n");
            goto exit;
        }

        ret = sm_vsc_open(VIDEO_WID_0);
        if (ret) {
            rtd_pr_sm_err("Fail sm_vsc_open, ret = %d\n", ret);
            goto exit;
        }

        memset(&panel_info, 0, sizeof(KADP_DISP_PANEL_INFO_T));
        ret = HAL_VBE_DISP_Initialize(panel_info) == 0 ? -1 : 0;
        if (ret) {
            rtd_pr_sm_err("Fail sm_vbe_init, ret = %d\n", ret);
            goto exit;
        }
    }

    sm_thread = kthread_create(sm_standby, NULL, "source_monitor_thread");
    if (IS_ERR(sm_thread)) {
        rtd_pr_sm_err("Fail to create source monitor thread\n");
        return -1;
    }
    wake_up_process(sm_thread);

exit:
    return ret;
}

/* Make sure DP, HDMI, Scaler modules have initialized */
late_initcall(rtk_sm_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Realtek Semiconductor Corp., Taiwan");
MODULE_DESCRIPTION("Source Monitor Driver");
