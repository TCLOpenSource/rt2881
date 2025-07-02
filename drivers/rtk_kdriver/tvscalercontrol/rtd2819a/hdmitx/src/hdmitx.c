#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
#include <rtk_kdriver/measure/rtk_measure.h>
#include <rtk_kdriver/tvscalercontrol/hdmirx/hdmifun.h>
#include <rbus/hdmi_top_common_reg.h>
#include <rbus/hdmi_dp_common_misc_reg.h>
#include <rbus/hdmi_p3_reg.h>
#include <rbus/hdmi21_p3_reg.h>
#endif

#include <rbus/hdmitx_phy_reg.h>
#include <rbus/hdmitx_vgip_reg.h>
#include <rbus/hdmitx20_mac1_reg.h>
#include <rbus/dsce_misc_reg.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/scaleup_reg.h>
//#include <rbus/scaleup3_reg.h>
//#include <rbus/two_step_uzu_reg.h>
#include <rbus/dispd_ds444_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/pll_reg_reg.h>
#include <rbus/vodma_reg.h>
#include <rbus/timer_reg.h>
#include <rbus/pll27x_reg_reg.h>

#include <rbus/dsce_reg.h>
#include <rbus/hdmitx_misc_reg.h>
#include <rbus/hdmitx20_mac0_reg.h>
#include <rbus/hdmitx20_on_region_reg.h>
#include <rbus/hdmitx21_mac_reg.h>
#include <rbus/hdmitx_vgip_reg.h>
#include <rbus/ppoverlay_txpretg_reg.h>

#include <rtk_kdriver/pcbMgr.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h> 
#include <tvscalercontrol/hdmitx/hdmitx_lib.h>
#include <tvscalercontrol/hdmitx/hdmitx.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_share.h>
#include <tvscalercontrol/txpre/txpre_mode.h>
#include <tvscalercontrol/txpre/txpre.h>
#include <rtk_kdriver/tvscalercontrol/hdmirx/hdmifun.h>

//#include <tvscalercontrol/i3ddma/i3ddma.h>
//#include <tvscalercontrol/scaler/scalerstruct.h>
//#include <scaler_vbedev.h>
//#include <scaler/scalerCommon.h>
#include <scaler/scalerCommon.h>

#include <mach/platform.h>
#include <mach/rtk_platform.h>

#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)
#include <rtk_kdriver/RPCDriver.h>
#include <VideoRPC_System.h>
#endif

// [MARK2] module init
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/freezer.h>



static UINT8 bHdmiTxRun_Flag=0;

static UINT8 bHdmiTxAudio_ch=0;
static UINT8 bHdmiTxAudio_type=0;
static UINT8 bHdmiTxAudio_mute=0;
static UINT32 HdmiTxAudio_freq=0;
UINT8 HDMITX_DTG_Wait_Den_Stop_Done(void);
UINT8 HDMITX_DTG_Wait_Den_Start(void);
UINT8 HDMITX_DTG_Wait_vsync_start(void);
void setInputTimingType(int index, UINT32 param2);
#ifdef CONFIG_ENABLE_MARK2_ZEBU_BRING_UP
extern void Scaler_MEMC_outMux(UINT8 dbuf_en,UINT8 outmux_sel);
//extern DEBUG_SCALER_VSC_STRUCT DbgSclrFlgTkr; // debug scaler flag tracker
extern UINT8 vsc_force_rerun_main_scaler;

#ifdef _MARK2_ZEBU_BRING_UP_CHECK_CRC // [MARK2] FIX-ME -- check DSCE/HDMITX CRC
typedef enum{
    HDMITX_CRC_UZU,
    HDMITX_CRC_DSCE,
    HDMITX_CRC_MISC,
    HDMITX_CRC_HDMI20TX,
    HDMITX_CRC_HDMI21TX,
    HDMITX_CRC_LIST_NUM,
} ENUM_HDMITX_CRC_LIST;

#define HDMITX_CRC_CH_NUM 3
typedef struct {
    UINT32 crc_ch[HDMITX_CRC_CH_NUM]; // CRC in 3 channel
} DS_HDMITX_CRC_TYPE;
#endif // #ifdef _MARK2_ZEBU_BRING_UP_CHECK_CRC // [MARK2] FIX-ME -- check DSCE/HDMITX CRC
#endif // #ifdef CONFIG_ENABLE_MARK2_ZEBU_BRING_UP


extern void fw_scaler_dtg_double_buffer_apply(void);
//extern EnumHdmi21FrlType ScalerHdmiTxGetTargetFrlRate(void);
extern void ScalerHdmiTxSetTimingStandardType(EnumHdmitxTimingStdType stdType);
extern EnumHdmitxTimingStdType ScalerHdmiTxGetTimingStandardType(void);
extern void ScalerHdmiMacTx0DataIslandPacking(void);
extern void ScalerHdmiTxSetMute(UINT8 enable);
extern void ScalerHdmiTx0_H5xDsc_inputSrcConfig(EnumHDMITXDscSrc dsc_src);
//extern void ScalerHdmiTxSetVTEMpacektEn(UINT8 enable);
//extern void ScalerHdmiTxSetVRRFlagEn(UINT8 enable);

extern int rtk_otp_field_read_int_by_name(const char *name);

UINT8 hdmitx_get_otp_status(void)
{
    int ret = rtk_otp_field_read_int_by_name("sw_func_hdmi_tx");

    FatalMessageHDMITx("HDMITX_otp_field ret=%d\n", ret);//0:support, -1:func error, other:not support
    if (ret == 0)
        return 0;
    else
        return 1;
}


void ScalerHdmiTxSetTxRunFlag(UINT8 enable)
{
    if(bHdmiTxRun_Flag != enable){
        FatalMessageHDMITx("[HDMITX] bHdmiTxRun_Flag=%d\n", enable);
        bHdmiTxRun_Flag = enable;
    }
    return;
}

void ScalerHdmiTx_reset_FRL(void)
{
    // frl_en=0, frl_reset=1
    HDMITX_DTG_Wait_Den_Stop_Done();
    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg,~(HDMITX21_MAC_HDMI21_FRL_0_frl_en_mask|HDMITX21_MAC_HDMI21_FRL_0_frl_reset_mask), HDMITX21_MAC_HDMI21_FRL_0_frl_reset_mask);//frl_en[7]=0, frl_reset[6]=1 (active mode)

    // frl_reset=0
    HDMITX_DTG_Wait_Den_Start();
    HDMITX_DTG_Wait_Den_Stop_Done();
    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~HDMITX21_MAC_HDMI21_FRL_0_frl_reset_mask, 0);//frl_reset[6]=0 (active mode)

    // frl_en=1
    HDMITX_DTG_Wait_Den_Start();
    HDMITX_DTG_Wait_Den_Stop_Done();
    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~HDMITX21_MAC_HDMI21_FRL_0_frl_en_mask, HDMITX21_MAC_HDMI21_FRL_0_frl_en_mask);//frl_en[7]=1
    return;
}

void ScalerHdmiTxSetTxPkt(UINT32 pktType)
{
    NoteMessageHDMITx("[HDMITX] Set Packet Type[%d]\n", pktType);
    ScalerHdmiMacTx0DataIslandPacking();
    return;
}



UINT8 ScalerHdmiTxGetTxRunFlag(void)
{
    return 1;//bHdmiTxRun_Flag;
}

// [MARK2] FIX-ME -- replace TXSOC setting into main DTG setting flow
UINT8 HDMITX_wait_for_tgLineCount(UINT32 start, UINT32 end, UINT8 wait_zero)
{
	UINT32 timeoutcnt = 0x065000;
	UINT32 lineCnt=0, stc=rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
	do {
		lineCnt = PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_get_line_cnt_rt(rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_reg));
		if(((lineCnt >= start) && (lineCnt < end))|| (wait_zero && (lineCnt == 0))) {
			return TRUE;
		}
	} while(timeoutcnt-- != 0);

	FatalMessageHDMITx("[HDMITX] Wait TG LC timeout, %d->%d(z=%d)@LC/diff=%d/%d\n", start, end, wait_zero, lineCnt, (rtd_inl(TIMER_SCPU_CLK90K_LO_reg)-stc)/90);
	return FALSE;
}


UINT8 HDMITX_DTG_Wait_vsync_start(void) {
    UINT8 ret=0;
    UINT32 dv_len = PPOVERLAY_TXPRETG_TXPRETG_DV_Length_get_dv_length(rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_DV_Length_reg));
    if(dv_len && (dv_len > 1))
        ret = HDMITX_wait_for_tgLineCount(1, dv_len, (PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_get_line_cnt_rt(rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_reg)) ? 1 : 0));
    else
        ret = HDMITX_wait_for_tgLineCount(0, 1, (PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_get_line_cnt_rt(rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_reg)) ? 1 : 0));

    if(ret == 0)
        FatalMessageHDMITx("[HDMITX] Wait v-sync start Fail From\n");

    return ret;
}



UINT8 HDMITX_DTG_Wait_Den_Stop_Done(void) {
    UINT8 ret=0;
    UINT32 vEnd = PPOVERLAY_TXPRETG_TXPRETG_DV_DEN_Start_End_get_dv_den_end(rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_DV_DEN_Start_End_reg));
    if(vEnd)
        ret = HDMITX_wait_for_tgLineCount(vEnd, 0x1fff, (PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_get_line_cnt_rt(rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_reg)) ? 1 : 0));
    else
        NoteMessageHDMITx("[HDMITX] Wait vEnd=0\n");

    if(ret == 0)
        NoteMessageHDMITx("[HDMITX] Wait Den Stop Fail From\n");

    return ret;
}

UINT8 HDMITX_DTG_Wait_Den_Start(void) {
    UINT8 ret=0;
    UINT32 vStart = PPOVERLAY_TXPRETG_TXPRETG_DV_DEN_Start_End_get_dv_den_sta(rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_DV_DEN_Start_End_reg));
    if(vStart)
        ret = HDMITX_wait_for_tgLineCount(vStart, 0x300, (PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_get_line_cnt_rt(rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_reg)) ? 1 : 0));
    else
        NoteMessageHDMITx("[HDMITX] Wait vStart=0\n");

    if(ret == 0)
        NoteMessageHDMITx("[HDMITX] Wait Den Start Fail From\n");

    return ret;
}


void HDMITX_set_IvSrcSelChange(void) {
    #if 0 //fix by zhaodong
    UINT32 *ivSrcSelChange = NULL;
    NoteMessageHDMITx("[txsoc] set iv_src_sel, 90k: %d", rtd_inl(TIMER_SCPU_CLK90K_LO_reg)/90);

    ivSrcSelChange = (UINT32*)Scaler_GetShareMemVirAddr(IV_SRC_SEL_READY);


    *ivSrcSelChange = htonl(TRUE);
    #endif
}

bool HDMITX_get_IvSrcSelChange(void) {
    #if 0 //fix by zhaodong
    UINT32 *ivSrcSelChange = NULL;

    ivSrcSelChange = (UINT32*)Scaler_GetShareMemVirAddr(IV_SRC_SEL_READY);

    *ivSrcSelChange = htonl(*ivSrcSelChange);

    return *ivSrcSelChange;
    #endif
    return TRUE;

}

void rpcVoIvSrcSel(int source) {
#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)
    long ret = 0;
    int timeout = 5;

    HDMITX_set_IvSrcSelChange();

    NoteMessageHDMITx("[HDMITX]line: %d, start send rpc to vcpu, command: %d, source: %d\n", __LINE__, VIDEO_RPC_VOUT_FILTER_ToAgent_VODMA_ConfigVsyncGenIvSrc, source);
    //set_vo_nosignal_flag((port_local & 0xf), 1);
    if (send_rpc_command(RPC_VIDEO, VIDEO_RPC_VOUT_FILTER_ToAgent_VODMA_ConfigVsyncGenIvSrc, 0, source, &ret))
        FatalMessageHDMITx("RPC fail!!\n");

    if( ret == RPC_FAIL)
        FatalMessageHDMITx("[HDMITX]RPC fail!!\n");

    while(HDMITX_get_IvSrcSelChange() && timeout--) {
        msleep(10);
        InfoMessageHDMITx("[HDMITX] check iv_src_sel, 90k: %d", rtd_inl(TIMER_SCPU_CLK90K_LO_reg)/90);
    }
#endif

    return;
}



extern void ScalerHdmiTxSetFastTimingSwitchEn(UINT8 enable);
extern UINT8 ScalerHdmiTxGetFastTimingSwitchEn(void);
extern UINT8 ScalerHdmiTxGetStreamOffWithoutLinkTrainingFlag(void);
//extern HOST_PLAFTORM_TYPE ScalerHdmiTxGetH5DPlatformType(void);
extern void ScalerHdmiTxSetPixelFormatChange(UINT32 status);
extern UINT32 ScalerHdmiTxGetPixelFormatChange(void);
extern void ScalerHdmiTxSetChipVersion(UINT8 value);
extern UINT8 ScalerHdmiTxGetChipVersion(void);
extern UINT8 ScalerHdmiTxGetFrlNewModeEnable(void);
extern void ScalerHdmiTxSetFrlNewModeEnable(UINT8 enable);

//void Scaler_FastTimingSwitch_VRR_4k2k_setting_by_framerate(int frame_rate);

UINT32 VO_ACT_H_START ;
UINT32 VO_ACT_V_START ;
UINT32 VO_H_BACKPORCH ;
UINT32 VO_V_BACKPORCH ;

UINT32 DTG_H_DEN_STA ;
UINT32 DTG_V_DEN_STA ;
UINT32 DTG_H_PORCH ;
UINT32 DTG_V_PORCH ;
UINT32 DTG_IV2DV_HSYNC_WIDTH = 0x320;
UINT32 DTG_DH_WIDTH ;
UINT32 DTG_DV_LENGTH ;

unsigned long long HDMI_TX_I2C_BUSID = 3;

UINT8 HDMI_TIMING_FAST_SWITCH_EN=0;
UINT32 HDMI_TIMING_PIXEL_FMT_CHANGE=0x7;
//static int last_frlType=-1;
//static int last_pixelClk=0;
//static int last_colorFmt=0, last_colorDepth=0;
//static int last_frameRate=0;

static UINT8 vrrLoopMode=0;
static UINT8 vrrLoopDelay=0;
static UINT8 vrr24Hz=0;

//#ifdef _MARK2_FIXME_H5X_TXSOC_FLOW // [MARK2] FIX-ME -- update mark2 scaler/hdmitx control flow
static UINT8 DBG_EN_TOGGLE_PTG=0;
static UINT8 DBG_EN_PTG=0;
//#endif

//static int HDMITX_TIMING_INDEX = -1;

//static UINT8 _FORCE_DSC_ENABLE=FALSE;

typedef enum
{
    PIXEL_COLOR = _BIT0,
    PIXEL_DEPTH = _BIT1,
    PIXEL_CLK = _BIT2,
}EnumPixelType;



extern void ScalerHdmiMacTx0PowerProc(UINT8 enumPowerAction);
extern void Scaler_TestCase_config(UINT32 width, UINT32 height, UINT32 frame_rate, I3DDMA_COLOR_DEPTH_T colorDepth, I3DDMA_COLOR_SPACE_T colorFmt, UINT8 timeIdx);
extern void ScalerHdmiMacTx0EdidSetFeature(EnumEdidFeatureType enumEdidFeature, BYTE ucValue);
extern void Scaler_TxStateHandler(void);
extern void ScalerHdmiTxSetTargetFrlRate(EnumHdmi21FrlType frlType);
extern void ScalerHdmiTxSetDSCEn(UINT8 enable);
extern void ScalerHdmiTxSetDscMode(UINT8 dscMode);
//extern UINT8 ScalerHdmiTxGetDscMode(void);

extern void drvif_color_hdmitx_rgb2yuv_en(UINT8 enable);
extern void Scaler_color_set_HDMITX_RGB2YUV(void);

//extern void ScalerHdmiTxSetHdmiRxPortLimit(UINT8 portNum);
//extern void ScalerHdmiTxCheckEdidForTxOut(UINT8 enable);
extern UINT16 Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_INFO infoList);
extern void Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_INFO infoList, UINT16 value);
//extern UINT16 Scaler_HDMITX_DispGetStatus(SCALER_DISP_CHANNEL channel, SLR_DISP_STATUS index);
//extern void Scaler_HDMITX_DispSetStatus(SCALER_DISP_CHANNEL channel, SLR_DISP_STATUS index, UINT16 status);
extern UINT8 fwif_scaler_wait_for_event_done(UINT32 addr, UINT32 event);

void ScalerHdmiTxSetBypassLinkTrainingEn(UINT8 enable);
//extern void ScalerHdmiTxSetHDCP22Enable(UINT8 enable);

void dvScript_txsoc_config(UINT32 width, unsigned height, UINT16 frame_rate)
{
#if 0//#ifdef _MARK2_FIXME_H5X_TXSOC_FLOW // [MARK2] FIX-ME -- replace TXSOC setting into main DTG setting flow
    tx_soc_vgip_tx_soc_vgip_ctrl_RBUS tx_soc_vgip_tx_soc_vgip_ctrl_reg;
    tx_soc_vgip_tx_soc_vgip_act_hsta_width_RBUS tx_soc_vgip_tx_soc_vgip_act_hsta_width_reg;
    tx_soc_vgip_tx_soc_vgip_act_vsta_length_RBUS tx_soc_vgip_tx_soc_vgip_act_vsta_length_reg;
    scaledown_txsoc_ich4_uzd_channel_swap_RBUS scaledown_txsoc_ich4_uzd_channel_swap_reg;
    scaledown_txsoc_ich4_ibuff_ctrli_buff_ctrl_RBUS scaledown_txsoc_ich4_ibuff_ctrli_buff_ctrl_reg;
    scaledown_txsoc_ich4_uzd_ctrl0_RBUS scaledown_txsoc_ich4_uzd_ctrl0_reg;
    scaledown_txsoc_ich4_uzd_initial_value_RBUS scaledown_txsoc_ich4_uzd_initial_value_reg;
    fsyncbuf_fsync_fifo_status_RBUS fsyncbuf_fsync_fifo_status_reg;
    fsyncbuf_pxl2bus_ctrl1_RBUS fsyncbuf_pxl2bus_ctrl1_reg;
    fsyncbuf_pxl2bus_ctrl2_RBUS fsyncbuf_pxl2bus_ctrl2_reg;
    txsoc_scaleup_txsoc_uzu_input_size_RBUS txsoc_scaleup_txsoc_uzu_input_size_reg;
    txsoc_scaleup_txsoc_uzu_ctrl_RBUS txsoc_scaleup_txsoc_uzu_ctrl_reg;
    txsoctg_tx_soc_tg_double_buffer_ctrl_RBUS txsoctg_tx_soc_tg_double_buffer_ctrl_reg;
    txsoctg_txsoc_dh_width_RBUS txsoctg_txsoc_dh_width_reg;
    txsoctg_txsoc_dh_total_last_line_length_RBUS txsoctg_txsoc_dh_total_last_line_length_reg;
    txsoctg_txosc_dv_length_RBUS txsoctg_txosc_dv_length_reg;
    txsoctg_txsoc_dv_total_RBUS txsoctg_txsoc_dv_total_reg;
    txsoctg_txsoc_dh_den_start_end_RBUS txsoctg_txsoc_dh_den_start_end_reg;
    txsoctg_txosc_dv_den_start_end_RBUS txsoctg_txosc_dv_den_start_end_reg;
    txsoctg_txsoc_active_h_start_end_RBUS txsoctg_txsoc_active_h_start_end_reg;
    txsoctg_txsoc_active_v_start_end_RBUS txsoctg_txsoc_active_v_start_end_reg;
    txsoctg_txsoc_fs_iv_dv_fine_tuning5_RBUS txsoctg_txsoc_fs_iv_dv_fine_tuning5_reg;
    txsoctg_txsoc_display_timing_ctrl1_RBUS txsoctg_txsoc_display_timing_ctrl1_reg;
    txsoctg_txsoc_dctl_iv2dv_ihs_ctrl_RBUS txsoctg_txsoc_dctl_iv2dv_ihs_ctrl_reg;
    txsoctg_txsoc_pcr_cnt_RBUS txsoctg_txsoc_pcr_cnt_reg;
    txsoc_ds444_txsoc_ds444_ctrl0_RBUS txsoc_ds444_txsoc_ds444_ctrl0_reg;
    UINT8 ucColorSpace;

    DebugMessageHDMITx("[HDMI_TX] %s...\n", __FUNCTION__);

    //rtd_outl(0xb8021510,0x40000001); // TXSOC in select VO1
    tx_soc_vgip_tx_soc_vgip_ctrl_reg.regValue = 0;
    tx_soc_vgip_tx_soc_vgip_ctrl_reg.tx_soc_ivrun = 0;
    tx_soc_vgip_tx_soc_vgip_ctrl_reg.tx_soc_in_sel = 4; // 4: VODMA1
    tx_soc_vgip_tx_soc_vgip_ctrl_reg.tx_soc_random_en = 0;
    tx_soc_vgip_tx_soc_vgip_ctrl_reg.tx_soc_vact_end_ie = 0;
    tx_soc_vgip_tx_soc_vgip_ctrl_reg.tx_soc_vact_start_ie = 0;
    tx_soc_vgip_tx_soc_vgip_ctrl_reg.tx_soc_field_inv = 0;
    tx_soc_vgip_tx_soc_vgip_ctrl_reg.tx_soc_field_sync_edge = 0;
    tx_soc_vgip_tx_soc_vgip_ctrl_reg.tx_soc_force_tog = 0;
    tx_soc_vgip_tx_soc_vgip_ctrl_reg.tx_soc_vs_inv = 0;
    rtd_outl(TX_SOC_VGIP_TX_SOC_VGIP_CTRL_reg, tx_soc_vgip_tx_soc_vgip_ctrl_reg.regValue);

    //rtd_outl(0xb8021518,0x00000258);
    tx_soc_vgip_tx_soc_vgip_act_hsta_width_reg.regValue = 0;
    tx_soc_vgip_tx_soc_vgip_act_hsta_width_reg.tx_soc_ih_act_sta = 0;
    tx_soc_vgip_tx_soc_vgip_act_hsta_width_reg.tx_soc_bypass_den = 0;
    tx_soc_vgip_tx_soc_vgip_act_hsta_width_reg.tx_soc_ih_act_wid = width;
    rtd_outl(TX_SOC_VGIP_TX_SOC_VGIP_ACT_HSTA_Width_reg, tx_soc_vgip_tx_soc_vgip_act_hsta_width_reg.regValue);

    //rtd_outl(0xb802151c,0x000001e0);
    tx_soc_vgip_tx_soc_vgip_act_vsta_length_reg.regValue = 0;
    tx_soc_vgip_tx_soc_vgip_act_vsta_length_reg.tx_soc_iv_act_sta = 0;
    tx_soc_vgip_tx_soc_vgip_act_vsta_length_reg.tx_soc_iv_act_len = height;
    rtd_outl(TX_SOC_VGIP_TX_SOC_VGIP_ACT_VSTA_Length_reg, tx_soc_vgip_tx_soc_vgip_act_vsta_length_reg.regValue);

    //rtd_outl(0xb8021510,0xc0000001); // TXSOC in select VO1
    tx_soc_vgip_tx_soc_vgip_ctrl_reg.tx_soc_ivrun = 1;
    rtd_outl(TX_SOC_VGIP_TX_SOC_VGIP_CTRL_reg, tx_soc_vgip_tx_soc_vgip_ctrl_reg.regValue);

    //rtd_outl(0xb8021944,0x02580000);
    scaledown_txsoc_ich4_uzd_channel_swap_reg.regValue = 0;
    scaledown_txsoc_ich4_uzd_channel_swap_reg.hsd_input_size = width;
    scaledown_txsoc_ich4_uzd_channel_swap_reg.rgb_ch_swap = 0;
    rtd_outl(SCALEDOWN_TXSOC_ICH4_UZD_channel_swap_reg, scaledown_txsoc_ich4_uzd_channel_swap_reg.regValue);

    //rtd_outl(0xb8021940,0x01e00258);
    scaledown_txsoc_ich4_ibuff_ctrli_buff_ctrl_reg.regValue = 0;
    scaledown_txsoc_ich4_ibuff_ctrli_buff_ctrl_reg.ibuf_v_size = height;
    scaledown_txsoc_ich4_ibuff_ctrli_buff_ctrl_reg.ibuf_h_size = width;
    rtd_outl(SCALEDOWN_TXSOC_ICH4_IBUFF_CTRLI_BUFF_CTRL_reg, scaledown_txsoc_ich4_ibuff_ctrli_buff_ctrl_reg.regValue);

    //rtd_outl(0xb8021904,0x00000300); // UZD output 10bit 444
    scaledown_txsoc_ich4_uzd_ctrl0_reg.regValue = 0;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.en_gate = 0;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.cutout_en = 0;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.sel_3d_en = 0;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.truncationctrl = 0;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.dummy18021904_23_12 = 0;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.video_comp_en = 0;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.odd_inv = 0;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.out_bit = 1; // output 10bit
    scaledown_txsoc_ich4_uzd_ctrl0_reg.sort_fmt = 1; // output 444
    scaledown_txsoc_ich4_uzd_ctrl0_reg.v_y_table_sel =0 ;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.v_c_table_sel = 0;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.h_y_table_sel = 0;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.h_c_table_sel = 0;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.v_zoom_en = 0;
    scaledown_txsoc_ich4_uzd_ctrl0_reg.h_zoom_en = 0;
    rtd_outl(SCALEDOWN_TXSOC_ICH4_UZD_Ctrl0_reg, scaledown_txsoc_ich4_uzd_ctrl0_reg.regValue);

    //rtd_outl(0xb8021914,0x00000000); // H/V init=0
    scaledown_txsoc_ich4_uzd_initial_value_reg.regValue = 0;
    rtd_outl(SCALEDOWN_TXSOC_ICH4_UZD_Initial_Value_reg, scaledown_txsoc_ich4_uzd_initial_value_reg.regValue);

    //rtd_outl(0xb8021b00,0x003f2013); // fifoof_th = 0x3f2;
    fsyncbuf_fsync_fifo_status_reg.regValue = 0;
    fsyncbuf_fsync_fifo_status_reg.fsbuf_fifoerr_wde = 0;
    fsyncbuf_fsync_fifo_status_reg.fsbuf_wd_debug_mode = 0;
    fsyncbuf_fsync_fifo_status_reg.fsbuf_wd_debug_target = 0;
    fsyncbuf_fsync_fifo_status_reg.fifoof_th = 0x3f2;
    fsyncbuf_fsync_fifo_status_reg.wclk_gated = 0;
    fsyncbuf_fsync_fifo_status_reg.fsyncbuf_fifoof = 0;
    fsyncbuf_fsync_fifo_status_reg.fsyncbuf_fifouf = 0;
    fsyncbuf_fsync_fifo_status_reg.fsyncbuf_fifoempty = 1;
    fsyncbuf_fsync_fifo_status_reg.wclk_gated_en = 1;
    fsyncbuf_fsync_fifo_status_reg.fsyncbuf_en = 1;
    rtd_outl(FSYNCBUF_fsync_fifo_status_reg, fsyncbuf_fsync_fifo_status_reg.regValue);

    // rtd_outl(0xb8021b04,0x00002573); // den_width=600, source mode=444 10bit
    fsyncbuf_pxl2bus_ctrl1_reg.regValue = 0;
    fsyncbuf_pxl2bus_ctrl1_reg.den_width = width - 1;
    fsyncbuf_pxl2bus_ctrl1_reg.source_mode_sel = 3; // 3:30bit (444 10bit mode)  &  4pixel_mode:120
    rtd_outl(FSYNCBUF_pxl2bus_ctrl1_reg, fsyncbuf_pxl2bus_ctrl1_reg.regValue);

    //rtd_outl(0xb8021b08,0x000001e0); // den_height=480
    fsyncbuf_pxl2bus_ctrl2_reg.regValue = 0;
    fsyncbuf_pxl2bus_ctrl2_reg.den_height = height;
    rtd_outl(FSYNCBUF_pxl2bus_ctrl2_reg, fsyncbuf_pxl2bus_ctrl2_reg.regValue);

    //rtd_outl(0xb8021c28,0x025801e0); // uzu input = 600x480
    txsoc_scaleup_txsoc_uzu_input_size_reg.regValue = 0;
    txsoc_scaleup_txsoc_uzu_input_size_reg.hor_input_size = width;
    txsoc_scaleup_txsoc_uzu_input_size_reg.ver_input_size = height;
    rtd_outl(TXSOC_SCALEUP_TXSOC_UZU_Input_Size_reg, txsoc_scaleup_txsoc_uzu_input_size_reg.regValue);

    //rtd_outl(0xb8021c04,0x002a8000); // UZU disable
    txsoc_scaleup_txsoc_uzu_ctrl_reg.regValue = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.hor_mode_sel = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.wd_main_buf_udf_en = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.irq_main_buf_udf_en = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.v_y_consttab_sel = 1;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.v_c_consttab_sel = 1;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.h_y_consttab_sel = 1;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.h_c_consttab_sel = 1;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.hor12_table_sel = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.video_comp_en = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.odd_inv = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.in_fmt_conv = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.conv_type = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.v_y_table_sel = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.v_c_table_sel = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.h_y_table_sel = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.h_c_table_sel = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.in_fmt_dup = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.uzu_uzd_linebuf_sel = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.v_zoom_en = 0;
    txsoc_scaleup_txsoc_uzu_ctrl_reg.h_zoom_en = 0;
    rtd_outl(TXSOC_SCALEUP_TXSOC_UZU_Ctrl_reg, txsoc_scaleup_txsoc_uzu_ctrl_reg.regValue);

    //rtd_outl(0xb8021804,0x0000001f); disp hsync width = 0x1f
    txsoctg_txsoc_dh_width_reg.regValue = 0;
    txsoctg_txsoc_dh_width_reg.dh_width = TXSOC_DH_WIDTH -1;
    rtd_outl(TXSOCTG_TXSOC_DH_Width_reg, txsoctg_txsoc_dh_width_reg.regValue);

    txsoctg_tx_soc_tg_double_buffer_ctrl_reg.regValue = rtd_inl(TXSOCTG_TX_SOC_TG_DOUBLE_BUFFER_CTRL_reg);
    txsoctg_tx_soc_tg_double_buffer_ctrl_reg.tx_soc_tg_db_en = 1;
    rtd_outl(TXSOCTG_TX_SOC_TG_DOUBLE_BUFFER_CTRL_reg, txsoctg_tx_soc_tg_double_buffer_ctrl_reg.regValue);
    //rtd_outl(0xb8021808,0x031f031f); // H-total: 4N-1
    txsoctg_txsoc_dh_total_last_line_length_reg.regValue = 0;
    txsoctg_txsoc_dh_total_last_line_length_reg.dh_total = Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_H_LEN)-1;//(width-1) + TXSOC_H_PORCH; // sync to vo h-total
    txsoctg_txsoc_dh_total_last_line_length_reg.dh_total_last_line = Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_H_LEN)-1;//(width-1) + TXSOC_H_PORCH; // sync to vo h-total
    rtd_outl(TXSOCTG_TXSOC_DH_Total_Last_Line_Length_reg, txsoctg_txsoc_dh_total_last_line_length_reg.regValue);

    txsoctg_tx_soc_tg_double_buffer_ctrl_reg.regValue = rtd_inl(TXSOCTG_TX_SOC_TG_DOUBLE_BUFFER_CTRL_reg);
    txsoctg_tx_soc_tg_double_buffer_ctrl_reg.tx_soc_tg_db_rdy = 1;
    rtd_outl(TXSOCTG_TX_SOC_TG_DOUBLE_BUFFER_CTRL_reg, txsoctg_tx_soc_tg_double_buffer_ctrl_reg.regValue);

    Wait_TXSOC_Den_Stop_Done((UINT8*)__FUNCTION__, __LINE__);
    Wait_TXSOC_Den_Start((UINT8*)__FUNCTION__, __LINE__);

    //rtd_outl(0xb802180c,0x00000002); // vsync width =1N-1
    txsoctg_txosc_dv_length_reg.regValue = 0;
    txsoctg_txosc_dv_length_reg.dv_length = TXSOC_DV_LENGTH -1;
    rtd_outl(TXSOCTG_TXOSC_DV_Length_reg, txsoctg_txosc_dv_length_reg.regValue);

    //rtd_outl(0xb8021810,0x00000225); // DV total = 1N-1
    txsoctg_txsoc_dv_total_reg.regValue = 0;
    txsoctg_txsoc_dv_total_reg.dv_total = Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_V_LEN) -1;//0x3fff;//v-sync by v-sync source: vodma
    rtd_outl(TXSOCTG_TXSOC_DV_total_reg, txsoctg_txsoc_dv_total_reg.regValue);

    //rtd_outl(0xb8021814,0x00400298); // h den start = 0x40, den end = 0x298
    txsoctg_txsoc_dh_den_start_end_reg.regValue = 0;
    txsoctg_txsoc_dh_den_start_end_reg.dh_den_sta = TXSOC_TG_H_DEN_STA;
    txsoctg_txsoc_dh_den_start_end_reg.dh_den_end = TXSOC_TG_H_DEN_STA + width;
    rtd_outl(TXSOCTG_TXSOC_DH_DEN_Start_End_reg, txsoctg_txsoc_dh_den_start_end_reg.regValue);

    //rtd_outl(0xb8021818,0x001901f9); // v_den start=0x19, den end = 0x1f9
    txsoctg_txosc_dv_den_start_end_reg.regValue = 0;
    txsoctg_txosc_dv_den_start_end_reg.dv_den_sta = TXSOC_TG_V_DEN_STA;
    txsoctg_txosc_dv_den_start_end_reg.dv_den_end = TXSOC_TG_V_DEN_STA + height;
    rtd_outl(TXSOCTG_TXOSC_DV_DEN_Start_End_reg, txsoctg_txosc_dv_den_start_end_reg.regValue);

    //rtd_outl(0xb8021824,0x00400298); // act h start = 0x40, h end = 0x298
    txsoctg_txsoc_active_h_start_end_reg.regValue = 0;
    txsoctg_txsoc_active_h_start_end_reg.dh_act_sta = TXSOC_TG_H_DEN_STA;
    txsoctg_txsoc_active_h_start_end_reg.dh_act_end = TXSOC_TG_H_DEN_STA + width;
    rtd_outl(TXSOCTG_TXSOC_Active_H_Start_End_reg, txsoctg_txsoc_active_h_start_end_reg.regValue);

    //rtd_outl(0xb8021828,0x001901f9); // act v star=0x19, v-end=0x1f9
    txsoctg_txsoc_active_v_start_end_reg.regValue = 0;
    txsoctg_txsoc_active_v_start_end_reg.dv_act_sta = TXSOC_TG_V_DEN_STA;
    txsoctg_txsoc_active_v_start_end_reg.dv_act_end = TXSOC_TG_V_DEN_STA + height;
    rtd_outl(TXSOCTG_TXSOC_Active_V_Start_End_reg, txsoctg_txsoc_active_v_start_end_reg.regValue);

    //rtd_outl(0xb8021854,0x00000000); // IV2DV line
    txsoctg_txsoc_fs_iv_dv_fine_tuning5_reg.regValue = 0;
    txsoctg_txsoc_fs_iv_dv_fine_tuning5_reg.iv2dv_line = 0;
    txsoctg_txsoc_fs_iv_dv_fine_tuning5_reg.iv2dv_line_2 = 0;
    rtd_outl(TXSOCTG_TXSOC_FS_IV_DV_Fine_Tuning5_reg, txsoctg_txsoc_fs_iv_dv_fine_tuning5_reg.regValue);

    // vsync tracking setting - enable when Fsync_ivs_sel/Fix_last_line_sync_sel = 0:pcr_tracking vsync(A)(B) counter_vs (tg master)
    txsoctg_txsoc_pcr_cnt_reg.regValue = 0;
    txsoctg_txsoc_pcr_cnt_reg.pcr_count_en = 0;
    txsoctg_txsoc_pcr_cnt_reg.pcr_count_vs_clk_sel = 0; // 00: PCR A clock (PLLDDS 27MHz), 11: crystal clock
    txsoctg_txsoc_pcr_cnt_reg.count_vs_period_update = 1;
    txsoctg_txsoc_pcr_cnt_reg.count_vs_period = ((27000000*10)/frame_rate); //VSYNC_PERIOD;//VSYNC_PERIOD_60HZ; // 60hz
    rtd_outl(TXSOCTG_TXSOC_PCR_cnt_reg, txsoctg_txsoc_pcr_cnt_reg.regValue);

    txsoctg_txsoc_display_timing_ctrl1_reg.regValue = rtd_inl(TXSOCTG_TXSOC_Display_Timing_CTRL1_reg);
    //rtd_outl(0xb8021800,0x04000380); // disp_fsync_en=1, fsync_ivs_sel=txsoc_uzd_vs (vodma master)
    txsoctg_txsoc_display_timing_ctrl1_reg.disp_frc_fsync = 0;
    txsoctg_txsoc_display_timing_ctrl1_reg.disp_frc_on_fsync = 0;
    txsoctg_txsoc_display_timing_ctrl1_reg.disp_fix_last_line = 0;
    txsoctg_txsoc_display_timing_ctrl1_reg.disp_fsync_en = 0;
    txsoctg_txsoc_display_timing_ctrl1_reg.wde_to_free_run = 0;
 //   txsoctg_txsoc_display_timing_ctrl1_reg.disp_en = 0;
    txsoctg_txsoc_display_timing_ctrl1_reg.wde_to_bg = 0;
    txsoctg_txsoc_display_timing_ctrl1_reg.fsync_ivs_sel = 3; // 0:pcr_tracking vsync(A)(B) counter_vs (tg master), 3.txsoc_uzd_vs (vodma master)
    txsoctg_txsoc_display_timing_ctrl1_reg.frc2fsync_protection = 1;
    txsoctg_txsoc_display_timing_ctrl1_reg.frc2fsync_speedup_period = 0;
    txsoctg_txsoc_display_timing_ctrl1_reg.frc2fsync_speedup_en = 0;
    txsoctg_txsoc_display_timing_ctrl1_reg.frc2fsync_en = 0;
    txsoctg_txsoc_display_timing_ctrl1_reg.force_bg = 0;
    txsoctg_txsoc_display_timing_ctrl1_reg.fix_last_line_sync_sel = 3; // 0:pcr_tracking vsync(A)(B) counter_vs, 3: txsoc_uzd_vs
    txsoctg_txsoc_display_timing_ctrl1_reg.dv_rst_sscg_alignok_sel = 0;
    rtd_outl(TXSOCTG_TXSOC_Display_Timing_CTRL1_reg, txsoctg_txsoc_display_timing_ctrl1_reg.regValue);

    ucColorSpace = Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_COLOR_SPACE);
    // enable TXSOC TG forceBG
    if((DBG_EN_PTG == 0) && (DBG_EN_TOGGLE_PTG == 0)){
        txsoctg_txsoc_display_background_color_RBUS txsoctg_txsoc_display_background_color_reg;
        txsoctg_txsoc_display_background_color_2_RBUS txsoctg_txsoc_display_background_color_2_reg;
        txsoctg_txsoc_display_background_color_reg.regValue = rtd_inl(TXSOCTG_TXSOC_Display_Background_Color_reg);
        txsoctg_txsoc_display_background_color_2_reg.regValue = rtd_inl(TXSOCTG_TXSOC_Display_Background_Color_2_reg);
        txsoctg_txsoc_display_background_color_reg.d_bg_g = 0;
        txsoctg_txsoc_display_background_color_reg.d_bg_b = (ucColorSpace == I3DDMA_COLOR_RGB? 0: 0x800);
        txsoctg_txsoc_display_background_color_2_reg.d_bg_r = (ucColorSpace == I3DDMA_COLOR_RGB? 0: 0x800);
        rtd_outl(TXSOCTG_TXSOC_Display_Background_Color_reg, txsoctg_txsoc_display_background_color_reg.regValue);
        rtd_outl(TXSOCTG_TXSOC_Display_Background_Color_2_reg, txsoctg_txsoc_display_background_color_2_reg.regValue);

        txsoctg_txsoc_display_timing_ctrl1_reg.force_bg = 1;
        rtd_outl(TXSOCTG_TXSOC_Display_Timing_CTRL1_reg, txsoctg_txsoc_display_timing_ctrl1_reg.regValue);
        IoReg_Mask32(TXSOCTG_TX_SOC_TG_DOUBLE_BUFFER_CTRL_reg, ~TXSOCTG_TX_SOC_TG_DOUBLE_BUFFER_CTRL_tx_soc_tg_db_rdy_mask, TXSOCTG_TX_SOC_TG_DOUBLE_BUFFER_CTRL_tx_soc_tg_db_rdy(1));
    }

    //rtd_outl(0xb8021858,0x80000320); // IV to DV Hsync Width=0x320, IV to DV Hsync Self Generator Enable (always 1)
    txsoctg_txsoc_dctl_iv2dv_ihs_ctrl_reg.regValue = 0;
    txsoctg_txsoc_dctl_iv2dv_ihs_ctrl_reg.iv2dv_ih_slf_gen_en = 1;
    txsoctg_txsoc_dctl_iv2dv_ihs_ctrl_reg.iv2dv_ih_slf_width = TXSOC_IV2DV_HSYNC_WIDTH;
    rtd_outl(TXSOCTG_TXSOC_dctl_iv2dv_ihs_ctrl_reg, txsoctg_txsoc_dctl_iv2dv_ihs_ctrl_reg.regValue);

    // TXSOC DS444 control, ds444_mode_sel = 00: disable, 1 : 444 to 420, 2: 444 to 422 (drop mode), 3: 444 to 422 (average mode)
    txsoc_ds444_txsoc_ds444_ctrl0_reg.regValue = rtd_inl(TXSOC_DS444_txsoc_ds444_ctrl0_reg);
    txsoc_ds444_txsoc_ds444_ctrl0_reg.ds444_mode_sel = (ucColorSpace == I3DDMA_COLOR_YUV411? 1: (ucColorSpace == I3DDMA_COLOR_YUV422? 3: 0));
    rtd_outl(TXSOC_DS444_txsoc_ds444_ctrl0_reg, txsoc_ds444_txsoc_ds444_ctrl0_reg.regValue);

    //rtd_outl(0xb8021800,0x05000380); // disp_en=1
    txsoctg_txsoc_display_timing_ctrl1_reg.disp_en = 1;
    rtd_outl(TXSOCTG_TXSOC_Display_Timing_CTRL1_reg, txsoctg_txsoc_display_timing_ctrl1_reg.regValue);

    DebugMessageHDMITx("[HDMI_TX] %s Done!\n", __FUNCTION__);
#endif // [MARK2] FIX-ME -- replace TXSOC setting
    return;
}


void HDMITX_verifier_init_txsoc_ptg(I3DDMA_COLOR_SPACE_T colorFmt)
{
#if 0//#ifdef _MARK2_FIXME_H5X_TXSOC_FLOW // [MARK2] FIX-ME -- replace TXSOC setting into main DTG setting flow
    UINT32 width_src;

    DebugMessageHDMITx("[HDMI_TX] %s...\n", __FUNCTION__);

    // TXSOC VGIP PTG
    rtd_outl(TX_SOC_VGIP_TX_SOC_VGIP_DOUBLE_BUFFER_CTRL_reg, 0); // disable TXSOC VGIP db_en
    rtd_outl(TX_SOC_VGIP_PTG_TX_SOC_PTG_CTRL_reg, _BIT22|_BIT4|_BIT2); // enable TXSOC VGIP colorbar PTG (vertical 4 color)

    // TXSOC UZU PTG
    width_src = TX_SOC_VGIP_TX_SOC_VGIP_ACT_HSTA_Width_get_tx_soc_ih_act_wid(rtd_inl(TX_SOC_VGIP_TX_SOC_VGIP_ACT_HSTA_Width_reg));

    rtd_outl(TXSOC_SCALEUP_TXSOC_UZU_PATGEN_COLOR0_reg, 0x00ffffff);
    rtd_outl(TXSOC_SCALEUP_TXSOC_UZU_PATGEN_COLOR1_reg, 0x00ff0000);
    rtd_outl(TXSOC_SCALEUP_TXSOC_UZU_PATGEN_COLOR2_reg, 0x0000ff00);
    rtd_outl(TXSOC_SCALEUP_TXSOC_UZU_PATGEN_COLOR3_reg, 0x000000ff);

    rtd_outl(TXSOC_SCALEUP_TXSOC_UZU_PATGEN_FRAME_TOGGLE_reg, _BIT29|_BIT28|_BIT27|(DBG_EN_TOGGLE_PTG << 25)|((width_src/4)-1)); // PTG width B[21:0], PTG toggle B[26:25]: 0: no toggle, 1: 0/1 toggle, 2: 0/1/2 toggle, 3: 0/1/2/3 toggle
    rtd_outl(TXSOC_SCALEUP_TXSOC_UZU_PATGEN_FRAME_TOGGLE1_reg, (width_src/8)-1);
    rtd_outl(TXSOC_SCALEUP_TXSOC_UZU_PATGEN_FRAME_TOGGLE2_reg, (width_src/12)-1);

    // YUV2RGB enable control
    if(colorFmt == I3DDMA_COLOR_RGB){ // RGB
        drvif_color_txsoc_rgb2yuv_en(false);
        drvif_color_txsoc_yuv2rgb_en(true);
    }else{ // YUV
        drvif_color_txsoc_rgb2yuv_en(false);
        drvif_color_txsoc_yuv2rgb_en(false);
    }

    // enable PTG
    rtd_maskl(TXSOC_SCALEUP_TXSOC_UZU_Globle_Ctrl_reg, ~_BIT0, _BIT5|_BIT0); // enable main PTG, // PTG B[6:4]: 1: randon, 2: build-in 4 color, 3: build-in 8 color, 4: self 4 color, 5: self 8 color
#endif // #if 0 // [MARK2] FIX-ME -- replace TXSOC setting
    return;
}



extern hdmi_tx_timing_gen_st tx_cea_timing_table;
extern hdmi_tx_output_timing_type tx_output_timing_table;


void setHdmitxPatternGen(UINT32 width)
{

   NoteMessageHDMITx("[iRealOneTX] %s@Width[%d]\n", __FUNCTION__, width);

    //==================================== Pattern Generator ====================================
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_reg,20,0, (width/8) - 1); // [dsce    ]<-[mcu] // [31]: patgen_en = d'0 ,[30:29] : PATGEN_RAND_MODE, [21:0]: PATGEN_WIDTH = 479+1 (=width/8 -1)

    //1st color -- white
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C0_RG_reg,23,12,3840);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C0_RG_reg,11,0,3840);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C0_B_reg,11,0,3840);
    //2nd color -- red
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C1_RG_reg,23,12, 0);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C1_RG_reg,11,0, 3840);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C1_B_reg,11,0, 0);
    //3rd color -- green
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C2_RG_reg,23,12, 3840);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C2_RG_reg,11,0, 0);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C2_B_reg,11,0, 0);
    //4th color -- blue
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C3_RG_reg,23,12, 0);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C3_RG_reg,11,0, 0);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C3_B_reg,11,0, 3840);
    //5th color -- grey
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C4_RG_reg,23,12,2048);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C4_RG_reg,11,0,2048);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C4_B_reg,11,0,2048);
    //6th color -- orange
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C5_RG_reg,23,12, 1536);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C5_RG_reg,11,0, 3968);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C5_B_reg,11,0, 0);
    //7th color -- yellow
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C6_RG_reg,23,12, 3840);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C6_RG_reg,11,0, 3840);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C6_B_reg,11,0, 0);
    //8th color -- brown
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C7_RG_reg,23,12, 768);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C7_RG_reg,11,0, 1792);
    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_C7_B_reg,11,0, 0);

    rtd_part_outl( DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_reg, 31,31, 1);
    //==================================== Pattern Generator done ====================================

    return;
}


extern scaler_hdmi_timing_info scalerHdmiTimingInfo;
void dvScript_mainDtg_config(UINT32 width, unsigned height, UINT16 frame_rate)
{
    scaler_hdmi_timing_info *pScalerHdmiTimingInfo=NULL;

//#ifdef _MARK2_FIXME_H5X_TXSOC_FLOW // [MARK2] FIX-ME -- replace TXSOC setting into main DTG setting flow
    ppoverlay_display_timing_ctrl2_RBUS ppoverlay_display_timing_ctrl2_reg;
    ppoverlay_dispd_smooth_toggle1_RBUS ppoverlay_dispd_smooth_toggle1_reg;
    //ppoverlay_uzudtg_4k8k_ctrl0_RBUS ppoverlay_uzudtg_4k8k_ctrl0_reg;
    //dispd_ds444_dispd_ds444_crc_ctrl_RBUS dispd_ds444_dispd_ds444_crc_ctrl_reg;//spec
    scaleup_dm_uzu_input_size_RBUS scaleup_dm_uzu_input_size_reg;
    //scaleup3_dm_uzu3_input_size_RBUS reg_scaleup3_dm_uzu3_input_size_reg;//spec
    scaleup_d_uzu_patgen_frame_toggle_RBUS scaleup_d_uzu_patgen_frame_toggle_reg;
    scaleup_d_uzu_globle_ctrl_RBUS scaleup_d_uzu_globle_ctrl_reg;
    scaleup_dm_uzu_ctrl_RBUS scaleup_dm_uzu_ctrl_reg;
    ppoverlay_main_den_h_start_end_RBUS ppoverlay_main_den_h_start_end_reg;
    ppoverlay_main_den_v_start_end_RBUS ppoverlay_main_den_v_start_end_reg;
    ppoverlay_main_background_h_start_end_RBUS ppoverlay_main_background_h_start_end_reg;
    ppoverlay_main_background_v_start_end_RBUS ppoverlay_main_background_v_start_end_reg;
    ppoverlay_main_active_h_start_end_RBUS ppoverlay_main_active_h_start_end_reg;
    ppoverlay_main_active_v_start_end_RBUS ppoverlay_main_active_v_start_end_reg;
    //ppoverlay_main_active_h_start_end_new_RBUS ppoverlay_main_active_h_start_end_new_reg;//spec
    ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;
    ppoverlay_dh_width_RBUS ppoverlay_dh_width_reg;
    ppoverlay_dh_den_start_end_RBUS ppoverlay_dh_den_start_end_reg;
    ppoverlay_dh_total_last_line_length_RBUS ppoverlay_dh_total_last_line_length_reg;
    ppoverlay_dv_length_RBUS ppoverlay_dv_length_reg;
    ppoverlay_dv_den_start_end_RBUS ppoverlay_dv_den_start_end_reg;
    ppoverlay_dv_total_RBUS ppoverlay_dv_total_reg;
    ppoverlay_uzudtg_dv_total_RBUS ppoverlay_uzudtg_dv_total_reg;
    ppoverlay_uzudtg_dh_total_RBUS ppoverlay_uzudtg_dh_total_reg;
    ppoverlay_uzudtg_countrol_RBUS ppoverlay_uzudtg_countrol_reg;
    ppoverlay_memcdtg_dv_total_RBUS ppoverlay_memcdtg_dv_total_reg;
    ppoverlay_memcdtg_dh_total_RBUS ppoverlay_memcdtg_dh_total_reg;
    ppoverlay_memcdtg_dv_den_start_end_RBUS ppoverlay_memcdtg_dv_den_start_end_reg;
    ppoverlay_memcdtg_dh_den_start_end_RBUS ppoverlay_memcdtg_dh_den_start_end_reg;
    ppoverlay_memcdtg_control_RBUS ppoverlay_memcdtg_control_reg;
    ppoverlay_display_timing_ctrl1_RBUS ppoverlay_display_timing_ctrl1_reg;
    ppoverlay_double_buffer_ctrl_RBUS ppoverlay_double_buffer_ctrl_reg;
    ppoverlay_fs_iv_dv_fine_tuning2_RBUS ppoverlay_fs_iv_dv_fine_tuning2_reg;
    ppoverlay_fs_iv_dv_fine_tuning5_RBUS ppoverlay_fs_iv_dv_fine_tuning5_reg;
    ppoverlay_dctl_iv2dv_ihs_ctrl_RBUS ppoverlay_dctl_iv2dv_ihs_ctrl_reg;
    //ppoverlay_fix_last_line_pcr_ctrl_RBUS ppoverlay_fix_last_line_pcr_ctrl_reg;//spec
    pll27x_reg_sys_dclkss_RBUS pll27x_reg_sys_dclkss_reg;
    //UINT8 ucColorSpace;//spec jing zhou
    //two_step_uzu_dm_two_step_sr_input_size_RBUS two_step_uzu_dm_two_step_sr_input_size_reg;//spec
    ppoverlay_osddtg_dv_total_RBUS ppoverlay_osddtg_dv_total_reg;
    ppoverlay_osddtg_dh_total_RBUS ppoverlay_osddtg_dh_total_reg;
    ppoverlay_osddtg_dv_den_start_end_RBUS ppoverlay_osddtg_dv_den_start_end_reg;
    ppoverlay_osddtg_dh_den_start_end_RBUS ppoverlay_osddtg_dh_den_start_end_reg;
    ppoverlay_osddtg_control_RBUS ppoverlay_osddtg_control_reg;

    NoteMessageHDMITx("[HDMITX] DTG Setting: %dx%dp%d\n", width, height, frame_rate);
	pScalerHdmiTimingInfo = &scalerHdmiTimingInfo;

#if 1 //#ifndef _MARK2_ZEBU_BRING_UP_RUN // [MARK2] CHECK-ME -- only for Zebu Verify (wait HDMI XTOR ready)
    HDMITX_DTG_Wait_Den_Stop_Done();
#endif

    // disable DTG clock
    rtd_clearbits(PPOVERLAY_Display_Timing_CTRL1_reg, PPOVERLAY_Display_Timing_CTRL1_disp_en_mask);

    // disable UZU
    rtd_clearbits(SCALEUP_DM_UZU_Ctrl_reg, SCALEUP_DM_UZU_Ctrl_v_zoom_en_mask|SCALEUP_DM_UZU_Ctrl_h_zoom_en_mask);
    //rtd_clearbits(TWO_STEP_UZU_DM_TWO_STEP_SR_Ctrl_reg, TWO_STEP_UZU_DM_TWO_STEP_SR_Ctrl_v_zoom_en_mask|TWO_STEP_UZU_DM_TWO_STEP_SR_Ctrl_h_zoom_en_mask);//spec
    //rtd_clearbits(PPOVERLAY_uzudtg_control1_reg, PPOVERLAY_uzudtg_control1_third_uzu_mode_mask);//spec

    ppoverlay_display_timing_ctrl2_reg.regValue = rtd_inl(PPOVERLAY_Display_Timing_CTRL2_reg);
    //rtd_outl(0xB8028004, 0x80060008);
    // [MARK2] CHECK
    //ppoverlay_display_timing_ctrl2_reg.dispd2hdmitx_clk_en = 1;
    ppoverlay_display_timing_ctrl2_reg.shpnr_line_mode_sel = 0;
    ppoverlay_display_timing_ctrl2_reg.d2i3ddma_src_sel = 6;
    ppoverlay_display_timing_ctrl2_reg.orbit_timing_en = 0;
    ppoverlay_display_timing_ctrl2_reg.frc2fsync_protection = 1;
    ppoverlay_display_timing_ctrl2_reg.frc2fsync_speedup_period = 0;
    ppoverlay_display_timing_ctrl2_reg.frc2fsync_speedup_en = 0;
    ppoverlay_display_timing_ctrl2_reg.frc2fsync_en = 0;

    // TXSOC DS444 control, ds444_mode_sel = 0: 444to444 (disable, willbypass from input to output),
    // 1: 444 to 422 (drop mode), 2: 444 to 422 (average_h mode), 3: 444 to 420 (drop mode), 4: 444 to 420 (average_h_mode), 5: 444 to 420 (average_v mode)
    //ucColorSpace = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_SPACE);//spec
    //ppoverlay_display_timing_ctrl2_reg.ds444_mode_sel = (ucColorSpace == I3DDMA_COLOR_YUV411? 4: (ucColorSpace == I3DDMA_COLOR_YUV422? 2: 0));//spec
    rtd_outl(PPOVERLAY_Display_Timing_CTRL2_reg, ppoverlay_display_timing_ctrl2_reg.regValue);

    udelay(150);

    // disable DCLK Spread Spectrum
    pll27x_reg_sys_dclkss_reg.regValue = rtd_inl(PLL27X_REG_SYS_DCLKSS_reg);
    pll27x_reg_sys_dclkss_reg.dclk_new_en = 0;
    pll27x_reg_sys_dclkss_reg.dclk_ss_load = 0;
    pll27x_reg_sys_dclkss_reg.dclk_ss_en = 0;
    rtd_outl(PLL27X_REG_SYS_DCLKSS_reg, pll27x_reg_sys_dclkss_reg.regValue);

    udelay(150);

    //rtd_outl(0xb802811c, 0x00000005);
    ppoverlay_dispd_smooth_toggle1_reg.regValue = rtd_inl(PPOVERLAY_dispd_smooth_toggle1_reg);
    ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_clk_mode_revised = 1; // stage1 frac_a/b modify reg will latch by uzudtg delay done vsync.: 0:by vsync latch, 1:use old mode
    ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_src_sel = 0;
    ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_apply = 1;
    rtd_outl(PPOVERLAY_dispd_smooth_toggle1_reg, ppoverlay_dispd_smooth_toggle1_reg.regValue);

    #if 0 // [MARK2] removed from mark2
    //rtd_outl(0xb8028134, 0x00000000);
    ppoverlay_uzudtg_4k8k_ctrl0_reg.regValue = rtd_inl(PPOVERLAY_UZUDTG_4K8K_CTRL0_reg);
    ppoverlay_uzudtg_4k8k_ctrl0_reg.uzudtg_sr_timing_sel = 0;
    ppoverlay_uzudtg_4k8k_ctrl0_reg.ip_uzu_sel = (width > 4096? 1: 0);
    ppoverlay_uzudtg_4k8k_ctrl0_reg.uzudtg_4k8k_timing_sel = 0;
    rtd_outl(PPOVERLAY_UZUDTG_4K8K_CTRL0_reg, ppoverlay_uzudtg_4k8k_ctrl0_reg.regValue);
    #endif

    //rtd_outl(0xb8029028, 0x0f00006c);
    scaleup_dm_uzu_input_size_reg.regValue = rtd_inl(SCALEUP_DM_UZU_Input_Size_reg);
    scaleup_dm_uzu_input_size_reg.hor_input_size = width;
    scaleup_dm_uzu_input_size_reg.ver_input_size = height;
    rtd_outl(SCALEUP_DM_UZU_Input_Size_reg, scaleup_dm_uzu_input_size_reg.regValue);
#if 0
    two_step_uzu_dm_two_step_sr_input_size_reg.regValue = rtd_inl(TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_reg);//spec
    two_step_uzu_dm_two_step_sr_input_size_reg.hor_input_size = width;//spec
    two_step_uzu_dm_two_step_sr_input_size_reg.ver_input_size = height;//spec
    rtd_outl(TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_reg, two_step_uzu_dm_two_step_sr_input_size_reg.regValue);//spec

    // [Mark2][IC] New
    reg_scaleup3_dm_uzu3_input_size_reg.regValue = rtd_inl(SCALEUP3_DM_UZU3_Input_Size_reg);//spec
    reg_scaleup3_dm_uzu3_input_size_reg.hor_input_size = width;//spec
    reg_scaleup3_dm_uzu3_input_size_reg.ver_input_size = height;//spec
    rtd_outl(SCALEUP3_DM_UZU3_Input_Size_reg, reg_scaleup3_dm_uzu3_input_size_reg.regValue);//spec
#endif
    scaleup_dm_uzu_ctrl_reg.regValue = rtd_inl(SCALEUP_DM_UZU_Ctrl_reg);
    scaleup_dm_uzu_ctrl_reg.h_zoom_en = 0;
    scaleup_dm_uzu_ctrl_reg.v_zoom_en = 0;
    rtd_outl(SCALEUP_DM_UZU_Ctrl_reg, scaleup_dm_uzu_ctrl_reg.regValue);

    //rtd_outl(0xb8029074, 0x00000005);
    scaleup_d_uzu_patgen_frame_toggle_reg.regValue = rtd_inl(SCALEUP_D_UZU_PATGEN_FRAME_TOGGLE_reg);
    scaleup_d_uzu_patgen_frame_toggle_reg.patgen_tog_num = 0;
    scaleup_d_uzu_patgen_frame_toggle_reg.patgen_tog_sel = 0; // 0: no toggle
    scaleup_d_uzu_patgen_frame_toggle_reg.patgen_width = (width > 1920? 0x1df: 0x27);//(width/16)-1;
    rtd_outl(SCALEUP_D_UZU_PATGEN_FRAME_TOGGLE_reg, scaleup_d_uzu_patgen_frame_toggle_reg.regValue);

    //rtd_outl(0xb8029000, 0x00000020);
    scaleup_d_uzu_globle_ctrl_reg.regValue = rtd_inl(SCALEUP_D_UZU_Globle_Ctrl_reg);
    scaleup_d_uzu_globle_ctrl_reg.uzu_bypass_no_pwrsave = 0;
    scaleup_d_uzu_globle_ctrl_reg.uzu_bypass_force = 0;
    scaleup_d_uzu_globle_ctrl_reg.in_black_en = 0;
    scaleup_d_uzu_globle_ctrl_reg.in_even_black = 0;
    scaleup_d_uzu_globle_ctrl_reg.out_black_en = 0;
    scaleup_d_uzu_globle_ctrl_reg.out_even_black = 0;
    scaleup_d_uzu_globle_ctrl_reg.patgen_sync = 0;
    scaleup_d_uzu_globle_ctrl_reg.patgen_mode = 3; // 2: build-in 4-color, 3: build-in 8-color
    scaleup_d_uzu_globle_ctrl_reg.patgen_field = 0;
    scaleup_d_uzu_globle_ctrl_reg.patgen_sel = 0; // 1: Pattern generator connect to MAIN
    rtd_outl(SCALEUP_D_UZU_Globle_Ctrl_reg, scaleup_d_uzu_globle_ctrl_reg.regValue);

    //rtd_outl(0xb8028540, 0x00000f00);
    ppoverlay_main_den_h_start_end_reg.regValue = rtd_inl(PPOVERLAY_MAIN_DEN_H_Start_End_reg);
    ppoverlay_main_den_h_start_end_reg.mh_den_sta = 0;
    ppoverlay_main_den_h_start_end_reg.mh_den_end = width;
    rtd_outl(PPOVERLAY_MAIN_DEN_H_Start_End_reg, ppoverlay_main_den_h_start_end_reg.regValue);

    //rtd_outl(0xb8028544, 0x0000006c);
    ppoverlay_main_den_v_start_end_reg.regValue = rtd_inl(PPOVERLAY_MAIN_DEN_V_Start_End_reg);
    ppoverlay_main_den_v_start_end_reg.mv_den_sta = 0;
    ppoverlay_main_den_v_start_end_reg.mv_den_end = height;
    rtd_outl(PPOVERLAY_MAIN_DEN_V_Start_End_reg, ppoverlay_main_den_v_start_end_reg.regValue);

    //rtd_outl(0xb8028548, 0x00000f00);
    ppoverlay_main_background_h_start_end_reg.regValue = rtd_inl(PPOVERLAY_MAIN_Background_H_Start_End_reg);
    ppoverlay_main_background_h_start_end_reg.mh_bg_sta = 0;
    ppoverlay_main_background_h_start_end_reg.mh_bg_end = width;
    rtd_outl(PPOVERLAY_MAIN_Background_H_Start_End_reg, ppoverlay_main_background_h_start_end_reg.regValue);

    //rtd_outl(0xb802854c, 0x0000006c);
    ppoverlay_main_background_v_start_end_reg.regValue = rtd_inl(PPOVERLAY_MAIN_Background_V_Start_End_reg);
    ppoverlay_main_background_v_start_end_reg.mv_bg_sta = 0;
    ppoverlay_main_background_v_start_end_reg.mv_bg_end = height;
    rtd_outl(PPOVERLAY_MAIN_Background_V_Start_End_reg, ppoverlay_main_background_v_start_end_reg.regValue);

    //rtd_outl(0xb8028550, 0x00000f00);
    ppoverlay_main_active_h_start_end_reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_H_Start_End_reg);
    ppoverlay_main_active_h_start_end_reg.mh_act_sta = 0;
    ppoverlay_main_active_h_start_end_reg.mh_act_end = width;
    rtd_outl(PPOVERLAY_MAIN_Active_H_Start_End_reg, ppoverlay_main_active_h_start_end_reg.regValue);

    //rtd_outl(0xb8028554, 0x0000006c);
    ppoverlay_main_active_v_start_end_reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_V_Start_End_reg);
    ppoverlay_main_active_v_start_end_reg.mv_act_sta = 0;
    ppoverlay_main_active_v_start_end_reg.mv_act_end = height;
    rtd_outl(PPOVERLAY_MAIN_Active_V_Start_End_reg, ppoverlay_main_active_v_start_end_reg.regValue);

    // [Mark2][IC] New
    //rtd_outl(0xb8028558, 0x00000f00);
    //ppoverlay_main_active_h_start_end_new_reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_H_Start_End_New_reg);//spec
    //ppoverlay_main_active_h_start_end_new_reg.mh_act_sta_new = 0;//spec
    //ppoverlay_main_active_h_start_end_new_reg.mh_act_end_new = width;//spec
    //rtd_outl(PPOVERLAY_MAIN_Active_H_Start_End_New_reg, ppoverlay_main_active_h_start_end_new_reg.regValue);//spec

    //rtd_outl(0xb8028300, 0x00000001);
    ppoverlay_main_display_control_rsv_reg.regValue = rtd_inl(PPOVERLAY_Main_Display_Control_RSV_reg);
    ppoverlay_main_display_control_rsv_reg.main_wd_to_background = 0;
    ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = 0;
    ppoverlay_main_display_control_rsv_reg.m_hbd_en = 0;
    ppoverlay_main_display_control_rsv_reg.m_bd_trans = 0;
    ppoverlay_main_display_control_rsv_reg.m_force_bg = 0;
    ppoverlay_main_display_control_rsv_reg.m_disp_en = 1;
    rtd_outl(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);

    //rtd_outl(0xb8028008, 0x00000057);
    ppoverlay_dh_width_reg.regValue = rtd_inl(PPOVERLAY_DH_Width_reg);
    ppoverlay_dh_width_reg.dh_width = pScalerHdmiTimingInfo->DTG_DH_WIDTH -1;
    rtd_outl(PPOVERLAY_DH_Width_reg, ppoverlay_dh_width_reg.regValue);

    //rtd_outl(0xb8028518, 0x01801080);
    ppoverlay_dh_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_DH_DEN_Start_End_reg);
    ppoverlay_dh_den_start_end_reg.dh_den_sta = pScalerHdmiTimingInfo->DTG_H_DEN_STA  - 24;
    ppoverlay_dh_den_start_end_reg.dh_den_end = pScalerHdmiTimingInfo->DTG_H_DEN_STA  + width - 24 ;
    rtd_outl(PPOVERLAY_DH_DEN_Start_End_reg, ppoverlay_dh_den_start_end_reg.regValue);

    //rtd_outl(0xb802800c, 0x112f112f);
    ppoverlay_dh_total_last_line_length_reg.regValue = rtd_inl(PPOVERLAY_DH_Total_Last_Line_Length_reg);
    ppoverlay_dh_total_last_line_length_reg.dh_total = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_TOTAL)-1;
    ppoverlay_dh_total_last_line_length_reg.dh_total_last_line = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_TOTAL)-1;
    rtd_outl(PPOVERLAY_DH_Total_Last_Line_Length_reg, ppoverlay_dh_total_last_line_length_reg.regValue);

    //rtd_outl(0xb8028010, 0x00000001);
    ppoverlay_dv_length_reg.regValue = rtd_inl(PPOVERLAY_DV_Length_reg);
    ppoverlay_dv_length_reg.dv_length = pScalerHdmiTimingInfo->DTG_DV_LENGTH -1;
    rtd_outl(PPOVERLAY_DV_Length_reg, ppoverlay_dv_length_reg.regValue);

    //rtd_outl(0xb802851c, 0x00080074);
    ppoverlay_dv_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_DV_DEN_Start_End_reg);
    ppoverlay_dv_den_start_end_reg.dv_den_sta = pScalerHdmiTimingInfo->DTG_V_DEN_STA;
    ppoverlay_dv_den_start_end_reg.dv_den_end = pScalerHdmiTimingInfo->DTG_V_DEN_STA + height;
    rtd_outl(PPOVERLAY_DV_DEN_Start_End_reg, ppoverlay_dv_den_start_end_reg.regValue);

    //rtd_outl(0xb8028014, 0x00000077);
    ppoverlay_dv_total_reg.regValue = rtd_inl(PPOVERLAY_DV_total_reg);
    ppoverlay_dv_total_reg.dv_total = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_TOTAL) -1;
    rtd_outl(PPOVERLAY_DV_total_reg, ppoverlay_dv_total_reg.regValue);

    //rtd_outl(0xb8028504, 0x00000077);
    ppoverlay_uzudtg_dv_total_reg.regValue = rtd_inl(PPOVERLAY_uzudtg_DV_TOTAL_reg);
    ppoverlay_uzudtg_dv_total_reg.uzudtg_dv_total = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_TOTAL) -1;
    rtd_outl(PPOVERLAY_uzudtg_DV_TOTAL_reg, ppoverlay_uzudtg_dv_total_reg.regValue);

    // rtd_outl(0xb8028508, 0x112f112f);
    ppoverlay_uzudtg_dh_total_reg.regValue = rtd_inl(PPOVERLAY_uzudtg_DH_TOTAL_reg);
    ppoverlay_uzudtg_dh_total_reg.uzudtg_dh_total_last_line = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_TOTAL)-1;
    ppoverlay_uzudtg_dh_total_reg.uzudtg_dh_total =Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_TOTAL)-1;
    rtd_outl(PPOVERLAY_uzudtg_DH_TOTAL_reg, ppoverlay_uzudtg_dh_total_reg.regValue);

    //rtd_outl(0xb8028500, 0x00000051);
    ppoverlay_uzudtg_countrol_reg.regValue = rtd_inl(PPOVERLAY_UZUDTG_COUNTROL_reg);
    ppoverlay_uzudtg_countrol_reg.uzudtg_mode_revised = 1;
    ppoverlay_uzudtg_countrol_reg.uzudtg_line_cnt_sync = 1;
    ppoverlay_uzudtg_countrol_reg.uzudtg_frc_fsync_status = 1; // 0 : free run, 1 : frame sync
    ppoverlay_uzudtg_countrol_reg.uzudtg_frc2fsync_by_hw = 0;
    ppoverlay_uzudtg_countrol_reg.uzudtg_fsync_en = 1; // 0: FRC mode, 1: Frame sync mode.
    ppoverlay_uzudtg_countrol_reg.uzudtg_en = 1;
    rtd_outl(PPOVERLAY_UZUDTG_COUNTROL_reg, ppoverlay_uzudtg_countrol_reg.regValue);

    //rtd_outl(0xb8028604, 0x00000077);
    ppoverlay_memcdtg_dv_total_reg.regValue = rtd_inl(PPOVERLAY_memcdtg_DV_TOTAL_reg);
    ppoverlay_memcdtg_dv_total_reg.memcdtg_dv_total = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_TOTAL) -1;
    rtd_outl(PPOVERLAY_memcdtg_DV_TOTAL_reg, ppoverlay_memcdtg_dv_total_reg.regValue);

    //rtd_outl(0xb8028608, 0x112f112f);
    ppoverlay_memcdtg_dh_total_reg.regValue = rtd_inl(PPOVERLAY_memcdtg_DH_TOTAL_reg);
    ppoverlay_memcdtg_dh_total_reg.memcdtg_dh_total_last_line = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_TOTAL)-1;
    ppoverlay_memcdtg_dh_total_reg.memcdtg_dh_total = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_TOTAL)-1;
    rtd_outl(PPOVERLAY_memcdtg_DH_TOTAL_reg, ppoverlay_memcdtg_dh_total_reg.regValue);

    //rtd_outl(0xb8028618, 0x00080074);
    ppoverlay_memcdtg_dv_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg);
    ppoverlay_memcdtg_dv_den_start_end_reg.memcdtg_dv_den_sta = pScalerHdmiTimingInfo->DTG_V_DEN_STA;
    ppoverlay_memcdtg_dv_den_start_end_reg.memcdtg_dv_den_end = pScalerHdmiTimingInfo->DTG_V_DEN_STA + height;
    rtd_outl(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg, ppoverlay_memcdtg_dv_den_start_end_reg.regValue);

    //rtd_outl(0xb802861c, 0x01801080);
    ppoverlay_memcdtg_dh_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_memcdtg_DH_DEN_Start_End_reg);
    ppoverlay_memcdtg_dh_den_start_end_reg.memcdtg_dh_den_sta = pScalerHdmiTimingInfo->DTG_H_DEN_STA;
    ppoverlay_memcdtg_dh_den_start_end_reg.memcdtg_dh_den_end = pScalerHdmiTimingInfo->DTG_H_DEN_STA + width;
    rtd_outl(PPOVERLAY_memcdtg_DH_DEN_Start_End_reg, ppoverlay_memcdtg_dh_den_start_end_reg.regValue);
#if 0//spec jing zhou
    ppoverlay_fix_last_line_pcr_ctrl_reg.regValue = rtd_inl(PPOVERLAY_fix_last_line_pcr_ctrl_reg);
    ppoverlay_fix_last_line_pcr_ctrl_reg.pcr_vs_count_en = 1;
    ppoverlay_fix_last_line_pcr_ctrl_reg.pcr_clk_sel = 0; // 0: pcr_a_clk, 1: pcr_b_clk
    ppoverlay_fix_last_line_pcr_ctrl_reg.pcr_vs_free_period_update = 1;
    ppoverlay_fix_last_line_pcr_ctrl_reg.pcr_vs_free_period = ((27000000*10)/frame_rate); ;
    rtd_outl(PPOVERLAY_fix_last_line_pcr_ctrl_reg, ppoverlay_fix_last_line_pcr_ctrl_reg.regValue);
#endif
    //rtd_outl(0xb8028600, 0x00000051);
    ppoverlay_memcdtg_control_reg.regValue = rtd_inl(PPOVERLAY_MEMCDTG_CONTROL_reg);
    ppoverlay_memcdtg_control_reg.memcdtg_mode_revised = 1;
    ppoverlay_memcdtg_control_reg.memcdtg_line_cnt_sync = 1;
    ppoverlay_memcdtg_control_reg.memcdtg_frc_fsync_status = 1; // 0 : free run, 1 : frame sync
    ppoverlay_memcdtg_control_reg.memcdtg_frc2fsync_by_hw = 0;
    ppoverlay_memcdtg_control_reg.memcdtg_fsync_en = 1; // 0: FRC mode, 1: Frame sync mode.
    ppoverlay_memcdtg_control_reg.memcdtg_en = 1;
    rtd_outl(PPOVERLAY_MEMCDTG_CONTROL_reg, ppoverlay_memcdtg_control_reg.regValue);

    ppoverlay_osddtg_dv_total_reg.regValue = rtd_inl(PPOVERLAY_osddtg_DV_TOTAL_reg);
    ppoverlay_osddtg_dv_total_reg.osddtg_dv_total = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_TOTAL)-1;
    rtd_outl(PPOVERLAY_osddtg_DV_TOTAL_reg, ppoverlay_osddtg_dv_total_reg.regValue);

    ppoverlay_osddtg_dh_total_reg.regValue = rtd_inl(PPOVERLAY_osddtg_DH_TOTAL_reg);
    ppoverlay_osddtg_dh_total_reg.osddtg_dh_total_last_line = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_TOTAL)-1;
    ppoverlay_osddtg_dh_total_reg.osddtg_dh_total = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_TOTAL)-1;
    rtd_outl(PPOVERLAY_osddtg_DH_TOTAL_reg, ppoverlay_osddtg_dh_total_reg.regValue);

    ppoverlay_osddtg_dv_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_osddtg_DV_DEN_Start_End_reg);
    ppoverlay_osddtg_dv_den_start_end_reg.osddtg_dv_den_sta = pScalerHdmiTimingInfo->DTG_V_DEN_STA;
    ppoverlay_osddtg_dv_den_start_end_reg.osddtg_dv_den_end = pScalerHdmiTimingInfo->DTG_V_DEN_STA + height;
    rtd_outl(PPOVERLAY_osddtg_DV_DEN_Start_End_reg, ppoverlay_osddtg_dv_den_start_end_reg.regValue);

    ppoverlay_osddtg_dh_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_osddtg_DH_DEN_Start_End_reg);
    ppoverlay_osddtg_dh_den_start_end_reg.osddtg_dh_den_sta = pScalerHdmiTimingInfo->DTG_H_DEN_STA;
    ppoverlay_osddtg_dh_den_start_end_reg.osddtg_dh_den_end = pScalerHdmiTimingInfo->DTG_H_DEN_STA + width;
    rtd_outl(PPOVERLAY_osddtg_DH_DEN_Start_End_reg, ppoverlay_osddtg_dh_den_start_end_reg.regValue);

    ppoverlay_osddtg_control_reg.regValue = rtd_inl(PPOVERLAY_OSDDTG_CONTROL_reg);
    ppoverlay_osddtg_control_reg.osddtg_mode_revised = 1;
    ppoverlay_osddtg_control_reg.osddtg_line_cnt_sync = 1;
    ppoverlay_osddtg_control_reg.osddtg_frc_fsync_status = 1;
    ppoverlay_osddtg_control_reg.osddtg_fsync_en = 1;
    ppoverlay_osddtg_control_reg.osddtg_en = 1;
    rtd_outl(PPOVERLAY_OSDDTG_CONTROL_reg, ppoverlay_osddtg_control_reg.regValue);

    //rtd_outl(0xb8028000, 0x20000008);
    ppoverlay_display_timing_ctrl1_reg.regValue = rtd_inl(PPOVERLAY_Display_Timing_CTRL1_reg);
    ppoverlay_display_timing_ctrl1_reg.disp_frc_fsync = 0;
    ppoverlay_display_timing_ctrl1_reg.disp_frc_on_fsync = 0;
    ppoverlay_display_timing_ctrl1_reg.disp_line_4x = 1;
    ppoverlay_display_timing_ctrl1_reg.disp_fix_last_line_new = 0;
    ppoverlay_display_timing_ctrl1_reg.disp_fsync_en = 0;
    ppoverlay_display_timing_ctrl1_reg.mdtg_line_cnt_sync = 1;
    ppoverlay_display_timing_ctrl1_reg.disp_en = 0; // 1: Display timing generator runs normally
    ppoverlay_display_timing_ctrl1_reg.wde_to_free_run = 0;
    ppoverlay_display_timing_ctrl1_reg.dout_force_bg = 0;
    ppoverlay_display_timing_ctrl1_reg.fsync_fll_mode = 0;
    //ppoverlay_display_timing_ctrl1_reg.stage1_dly_auto_en = 1;
    //ppoverlay_display_timing_ctrl1_reg.fix_last_line_vsync_sel = 0;//spec jing zhou
    ppoverlay_display_timing_ctrl1_reg.dv_rst_sscg_alignok_sel = 0;
    rtd_outl(PPOVERLAY_Display_Timing_CTRL1_reg, ppoverlay_display_timing_ctrl1_reg.regValue);

    ppoverlay_fs_iv_dv_fine_tuning2_reg.regValue = rtd_inl(PPOVERLAY_FS_IV_DV_Fine_Tuning2_reg);
    ppoverlay_fs_iv_dv_fine_tuning2_reg.iv2dv_pixel = 0;
    ppoverlay_fs_iv_dv_fine_tuning2_reg.iv2dv_pixel2 = 0;
    rtd_outl(PPOVERLAY_FS_IV_DV_Fine_Tuning2_reg, ppoverlay_fs_iv_dv_fine_tuning2_reg.regValue);

    ppoverlay_fs_iv_dv_fine_tuning5_reg.regValue = rtd_inl(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg);
    ppoverlay_fs_iv_dv_fine_tuning5_reg.iv2dv_line = 0;
    ppoverlay_fs_iv_dv_fine_tuning5_reg.iv2dv_line_2 = 0;
    rtd_outl(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg, ppoverlay_fs_iv_dv_fine_tuning5_reg.regValue);

    ppoverlay_dctl_iv2dv_ihs_ctrl_reg.regValue = rtd_inl(PPOVERLAY_dctl_iv2dv_ihs_ctrl_reg);
    ppoverlay_dctl_iv2dv_ihs_ctrl_reg.iv2dv_ih_slf_gen_en = 1;
    ppoverlay_dctl_iv2dv_ihs_ctrl_reg.iv2dv_ih_slf_width = pScalerHdmiTimingInfo->DTG_IV2DV_HSYNC_WIDTH;
    rtd_outl(PPOVERLAY_dctl_iv2dv_ihs_ctrl_reg, ppoverlay_dctl_iv2dv_ihs_ctrl_reg.regValue);

    //rtd_outl(0xb8028028, 0x00002222);
    ppoverlay_double_buffer_ctrl_reg.regValue = rtd_inl(PPOVERLAY_Double_Buffer_CTRL_reg);
    ppoverlay_double_buffer_ctrl_reg.orbit_dbuf_en = 0;
    ppoverlay_double_buffer_ctrl_reg.dmainreg_dbuf_en = 0;
    //ppoverlay_double_buffer_ctrl_reg.dsubreg_dbuf_en = 0;//spec jing zhou
    ppoverlay_double_buffer_ctrl_reg.dreg_dbuf_en = 0;
    ppoverlay_double_buffer_ctrl_reg.drm_multireg_dbuf_en = 0;
    rtd_outl(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
#if 0//spec jing zhou
    //rtd_outl(0xb802e730, 0x00000003);
    dispd_ds444_dispd_ds444_crc_ctrl_reg.regValue = rtd_inl(DISPD_DS444_dispd_ds444_CRC_Ctrl_reg);
    dispd_ds444_dispd_ds444_crc_ctrl_reg.crc_conti = 1;
    dispd_ds444_dispd_ds444_crc_ctrl_reg.crc_start = 1;
    rtd_outl(DISPD_DS444_dispd_ds444_CRC_Ctrl_reg, dispd_ds444_dispd_ds444_crc_ctrl_reg.regValue);
#endif
    //rtd_outl(0xb8028000, 0x21000008);
    ppoverlay_display_timing_ctrl1_reg.disp_en = 1; // 1: Display timing generator runs normally
    rtd_outl(PPOVERLAY_Display_Timing_CTRL1_reg, ppoverlay_display_timing_ctrl1_reg.regValue);

#if 1 //#ifndef _MARK2_ZEBU_BRING_UP_RUN // [MARK2] CHECK-ME -- only for Zebu Verify (wait HDMI XTOR ready)
    HDMITX_DTG_Wait_Den_Stop_Done();
    NoteMessageHDMITx("[HDMITX] DTG %dx%d Setting Done\n",
            PPOVERLAY_MAIN_DEN_H_Start_End_get_mh_den_end(rtd_inl(PPOVERLAY_MAIN_DEN_H_Start_End_reg)),
            PPOVERLAY_MAIN_DEN_V_Start_End_get_mv_den_end(rtd_inl(PPOVERLAY_MAIN_DEN_V_Start_End_reg)));
#endif
//#endif // #if 0 // [MARK2] FIX-ME -- replace TXSOC setting
    return;
}


void HDMITX_verifier_init_mainDtg_ptg(UINT32 width, I3DDMA_COLOR_SPACE_T colorFmt)
{
    scaleup_d_uzu_globle_ctrl_RBUS scaleup_d_uzu_globle_ctrl_reg;
    UINT8 vgip_source_ready=0;

    InfoMessageHDMITx("[HDMI_TX] D-domain Ptg width=%d@%s...\n", width, __FUNCTION__);

    // TXSOC UZU PTG
    rtd_outl(SCALEUP_D_UZU_PATGEN_COLOR0_reg, 0x00ffffff);
    rtd_outl(SCALEUP_D_UZU_PATGEN_COLOR1_reg, 0x00ff0000);
    rtd_outl(SCALEUP_D_UZU_PATGEN_COLOR2_reg, 0x0000ff00);
    rtd_outl(SCALEUP_D_UZU_PATGEN_COLOR3_reg, 0x000000ff);

    rtd_outl(SCALEUP_D_UZU_PATGEN_FRAME_TOGGLE_reg, _BIT29|_BIT28|_BIT27|(DBG_EN_TOGGLE_PTG << 25)|(width > 1920? 0x1df: 0x27)/*((width/4)-1)*/); // PTG width B[21:0], PTG toggle B[26:25]: 0: no toggle, 1: 0/1 toggle, 2: 0/1/2 toggle, 3: 0/1/2/3 toggle
    rtd_outl(SCALEUP_D_UZU_PATGEN_FRAME_TOGGLE1_reg, (width/8)-1);
    rtd_outl(SCALEUP_D_UZU_PATGEN_FRAME_TOGGLE2_reg, (width/12)-1);
#if 0//spec jing zhou
    // RGB2YUV enable control
    if(colorFmt == I3DDMA_COLOR_RGB){ // TX output format is RGB
        drvif_color_hdmitx_rgb2yuv_en(false);
    }else{ // YUV
        drvif_color_hdmitx_rgb2yuv_en(true);
    }
#endif
    // enable PTG
#if 1 // short pattern width for ZEBU XTOR windows view
    rtd_maskl(SCALEUP_D_UZU_Globle_Ctrl_reg, ~(_BIT6|_BIT5|_BIT4|_BIT0), _BIT5|_BIT4|_BIT0); // enable main PTG, // PTG B[6:4]: 1: randon, 2: build-in 4 color, 3: build-in 8 color, 4: self 4 color, 5: self 8 color
#else
    rtd_maskl(SCALEUP_D_UZU_Globle_Ctrl_reg, ~(_BIT6|_BIT5|_BIT4|_BIT0), _BIT5|_BIT0); // enable main PTG, // PTG B[6:4]: 1: randon, 2: build-in 4 color, 3: build-in 8 color, 4: self 4 color, 5: self 8 color
#endif

    vgip_source_ready = HDMITX_VGIP_HDMITX_VGIP_CTRL_get_hdmitx_ivrun(rtd_inl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg));

    //rtd_outl(0xb8029000, 0x00000021);
    scaleup_d_uzu_globle_ctrl_reg.regValue = rtd_inl(SCALEUP_D_UZU_Globle_Ctrl_reg);
    scaleup_d_uzu_globle_ctrl_reg.patgen_sel = (vgip_source_ready? 0: 1);//1; // 1: Pattern generator connect to MAIN
    rtd_outl(SCALEUP_D_UZU_Globle_Ctrl_reg, scaleup_d_uzu_globle_ctrl_reg.regValue);
    return;
}


UINT32 ScalerHdmiTx_get_bush(void)
{
	uint freq, tmp, value_f, value_n, pdiv, div,n_state;
	n_state = rtd_inl(PLL_REG_SYS_PLL_CPU_reg)&0x1; // = 0
	tmp = rtd_inl(PLL_REG_SYS_PLL_BUSH2_reg);
	value_f = ((tmp & PLL_REG_SYS_PLL_BUSH2_pllbush_fcode_mask) >> 16); // = 0x556
	value_n = ((tmp & PLL_REG_SYS_PLL_BUSH2_pllbush_ncode_mask) >> 8);  // = 0x3e
	tmp = rtd_inl(PLL_REG_SYS_PLL_BUSH1_reg);
	div = ((tmp & PLL_REG_SYS_PLL_BUSH1_pllbush_o_mask) >> 28); // = 0
	pdiv = ((tmp & PLL_REG_SYS_PLL_BUSH1_pllbush_prediv_mask)>>4);  // = 1
	if(n_state)
	{
		freq = ( 27 * (value_n + 3) + ((27 * value_f) >> 11)) /(pdiv + 1) / (1 << div);
	}
	else
	{
		freq = ( 27 * (value_n + 4) + ((27 * value_f) >> 11)) /(pdiv + 1) / (1 << div); // (27*(66) + (27*1366)/2048) / (1+1) /1 = (1782 + 18) / 2 = 900
	}
	return freq;
}



void ScalerHdmiTx_Set_DtgClkSource(int index, DCLK_SRC_TYPE type)
{
    return;
}


void ScalerHdmiTxSetDtgClkSource(UINT32 timing_type)
{
//#ifndef CONFIG_ENABLE_MARK2_ZEBU_BRING_UP
//#ifdef _MARK2_ZEBU_BRING_UP_NOT_RUN_FOR_DTG_FREE_RUN // [MARK2] FIX-ME -- let DTG free run for verify
    UINT32 width = tx_output_timing_table.width;
    UINT32 frame_rate = tx_output_timing_table.frame_rate;

    // switch Dclk source selection
    ScalerHdmiTx_Set_DtgClkSource(timing_type, DCLK_SRC_HDMITX_DPLL); // 0: from dpll, 1: from hdmitxpll
    udelay(150);

    InfoMessageHDMITx("[HDMITX] DTG width/FR=%d/%d\n", width, frame_rate);
    InfoMessageHDMITx("0230/0234/0208=%x/%x/%x\n", rtd_inl(0xb8000230), rtd_inl(0xb8000234), rtd_inl(0xb8000208));

//#endif // #ifdef _MARK2_ZEBU_BRING_UP_NOT_RUN_FOR_DTG_FREE_RUN // [MARK2] FIX-ME -- let DTG free run for verify
//#endif // #ifndef CONFIG_ENABLE_MARK2_ZEBU_BRING_UP
}

//extern void ScalerHdmiTx0_H5xLaneSrcSel_config(BYTE value);
//extern void ScalerHdmiTx0_H5xLanePnSwap_config(BYTE value);
//extern void ScalerHdmiTxSetH5DPlatformType(UINT8 type);
void setPtgStatus(UINT8 ptgEn, UINT8 ptgtoggle)
{
    DBG_EN_PTG = ptgEn;
    DBG_EN_TOGGLE_PTG = ptgtoggle;

#if 1 //#ifdef _MARK2_FIXME_H5X_TXSOC_FLOW // [MARK2] FIX-ME -- replace TXSOC setting into main DTG setting flow
    // main DTG forcebg disable
    rtd_maskl(PPOVERLAY_Main_Display_Control_RSV_reg, ~PPOVERLAY_Main_Display_Control_RSV_m_force_bg_mask, PPOVERLAY_Main_Display_Control_RSV_m_force_bg(0));
    rtd_maskl(PPOVERLAY_Double_Buffer_CTRL_reg, ~PPOVERLAY_Double_Buffer_CTRL_dmainreg_dbuf_set_mask, PPOVERLAY_Double_Buffer_CTRL_dmainreg_dbuf_set(1));
    // main UZU ptg enable
    rtd_maskl(SCALEUP_D_UZU_PATGEN_FRAME_TOGGLE_reg, ~SCALEUP_D_UZU_PATGEN_FRAME_TOGGLE_patgen_tog_sel_mask,
                                    SCALEUP_D_UZU_PATGEN_FRAME_TOGGLE_patgen_tog_sel(ptgtoggle)); // PTG toggle B[26:25]: 0: no toggle, 1: 0/1 toggle, 2: 0/1/2 toggle, 3: 0/1/2/3 toggle
#else
    // set PTG enable
    rtd_maskl(TXSOCTG_TXSOC_Display_Timing_CTRL1_reg, ~TXSOCTG_TXSOC_Display_Timing_CTRL1_force_bg_mask,
                                    TXSOCTG_TXSOC_Display_Timing_CTRL1_force_bg(!ptgEn && !ptgtoggle));

    IoReg_Mask32(TXSOCTG_TX_SOC_TG_DOUBLE_BUFFER_CTRL_reg, ~TXSOCTG_TX_SOC_TG_DOUBLE_BUFFER_CTRL_tx_soc_tg_db_rdy_mask, TXSOCTG_TX_SOC_TG_DOUBLE_BUFFER_CTRL_tx_soc_tg_db_rdy(1));
    // set PTG toggle enable
    rtd_maskl(TXSOC_SCALEUP_TXSOC_UZU_PATGEN_FRAME_TOGGLE_reg, ~TXSOC_SCALEUP_TXSOC_UZU_PATGEN_FRAME_TOGGLE_patgen_tog_sel_mask,
                                    TXSOC_SCALEUP_TXSOC_UZU_PATGEN_FRAME_TOGGLE_patgen_tog_sel(ptgtoggle)); // PTG toggle B[26:25]: 0: no toggle, 1: 0/1 toggle, 2: 0/1/2 toggle, 3: 0/1/2/3 toggle
#endif // #if 0 // [MARK2] FIX-ME -- replace TXSOC setting

    return;
}


void setHdmitxConfig(UINT32 param1, UINT32 param2)
{
    HdmiTxConfigPara1 *para1 = (HdmiTxConfigPara1 *)&param1;
    HdmiTxConfigPara2 *para2 = (HdmiTxConfigPara2 *)&param2;

    NoteMessageHDMITx("[HDMITX] Platform[%d], PortNum=%d, LaneSrc/Swap=%x/%x, VRR FR/Mode/Delay/24=%d/%d/%d/%d\n",
        para2->ucPlatformType, para1->uclimitPortNum, para2->ucLaneSrcCfg, para2->ucTxPnSwapCfg,
        para2->vrr_frame_rate, para2->vrr_mode, para2->vrr_loop_delay, para2->vrr_24hz);

    NoteMessageHDMITx("[HDMITX] CheckEdie/BypassLT=%d/%d, ptg En/Toggle=%d/%d, hdcp22 En=%d, hdcp2 RC Off=%d, DscMode=%d\n",
        para1->bCheckEdidInfo, para1->bBypassLinkTraining, para1->bEnablePtg, para1->bTogglePtg, para1->bEnableHdcp22, para1->bDisableHdcp22Rc, para1->ucDscMode);

    // config H5D platform type: 0: General, 1: O18, 2: O20
    //ScalerHdmiTxSetH5DPlatformType(para2->ucPlatformType); //fix by zhaodong

    // Config HDMITX output connect to HDMIRX port number constrain (0~4): 0: no limit, x: port[x]
    //ScalerHdmiTxSetHdmiRxPortLimit(para1->uclimitPortNum);//fix by zhaodong

    // check EDID info for TX output enable
    //ScalerHdmiTxCheckEdidForTxOut(para1->bCheckEdidInfo); //fix by zhaodong

    // force bypass HPD/EDID/LinkTraining control flow
    ScalerHdmiTxSetBypassLinkTrainingEn(para1->bBypassLinkTraining);

    // config lane src sel
    //ScalerHdmiTx0_H5xLaneSrcSel_config(para2->ucLaneSrcCfg);//fix by zhaodong

    // config lane PN swap
    //ScalerHdmiTx0_H5xLanePnSwap_config(para2->ucTxPnSwapCfg);//fix by zhaodong

    // set ptg
    setPtgStatus(para1->bEnablePtg, para1->bTogglePtg);

    //set hdcp22 enable
    //ScalerHdmiTxSetHDCP22Enable(para1->bEnableHdcp22);//fix by zhaodong

    //set DSC mode
    // DSC mode: 0: disable, 1: default, 2: 12G (bbp=8.125), 3: 12G (bbp=9.9375), 4: 12G (bbp=12.000), 5: 12G (bbp=15.000)
    #if 0 //fix by zhaodong
    if((para1->ucDscMode <= 5) && (para1->ucDscMode != ScalerHdmiTxGetDscMode())){
        UINT8 bDscEn = (para1->ucDscMode? _ENABLE: _DISABLE);
        ScalerHdmiTxSetDscMode(para1->ucDscMode);
        if(_FORCE_DSC_ENABLE != bDscEn){
            _FORCE_DSC_ENABLE = bDscEn;
            NoteMessageHDMITx("[HDMITX] _FORCE_DSC_ENABLE=%d\n", _FORCE_DSC_ENABLE);
        }
    }
    #endif

    // VRR: set output frame rate
    #if 0 //fix by zhaodong
    if(para2->vrr_frame_rate)
        Scaler_FastTimingSwitch_VRR_4k2k_setting_by_framerate(para2->vrr_frame_rate * 10);
    #endif

    // VRR loop mode & delay
    vrrLoopMode = para2->vrr_mode;
    vrrLoopDelay = para2->vrr_loop_delay;
    vrr24Hz = para2->vrr_24hz;

    return;
}


#if 0
void setInputTimingType(int index, UINT32 param2) {
    //UINT32 waitCnt = 100;
    if((index < 0)||(index >= TX_TIMING_NUM)){
        FatalMessageHDMITx("[HDMITX] INDEX[%d/%d] OVER RANGE\n", index, TX_TIMING_NUM);
        return;
    }

    NoteMessageHDMITx("[HDMITX] Input Timing %d->%d\n", HDMITX_TIMING_INDEX, index);
    
    if(!tx_output_timing_table[index].enable) {
        shell_printf (1, "select the wrong timing, please select again\n");
        return ;
    }
    else {
        if(_FORCE_DSC_ENABLE == 1){
            switch(index){
              case TX_TIMING_HDMI21_8K4KP60_YUV420_8BIT_8G:
                index = TX_TIMING_HDMI21_DSCE_7680_4320P60Hz_YUV444_8BIT_6G4L;
                break;
              case TX_TIMING_HDMI21_8K4KP60_YUV420_10BIT_10G:
                index = TX_TIMING_HDMI21_DSCE_7680_4320P60Hz_YUV444_10BIT_6G4L;
                break;
              case TX_TIMING_HDMI21_8K4KP59_YUV420_8BIT_8G:
                index = TX_TIMING_HDMI21_DSCE_7680_4320P59Hz_YUV444_8BIT_6G4L;
                break;
              case TX_TIMING_HDMI21_8K4KP59_YUV420_10BIT_10G:
                index = TX_TIMING_HDMI21_DSCE_7680_4320P59Hz_YUV444_10BIT_6G4L;
                break;
              default:
                break;
            }
        }

        HDMITX_TIMING_INDEX = index;

#if 1 //#if(_HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING == _ON) // [TEST][KTASKWBS-13388] support fast timing change flow
    if(ScalerHdmiTxGetStreamOffWithoutLinkTrainingFlag() && ScalerHdmiTxGetFrlNewModeEnable()){
        UINT32 bPixelFmtChange=(PIXEL_CLK|PIXEL_DEPTH|PIXEL_COLOR);
        UINT8 bSupportFastTimingSwitch = ((index >= TX_TIMING_HDMI21_4K2KP120_YUV444_8BIT_8G) && (index <= TX_TIMING_HDMI21_4K2KP60_YUV420_10BIT_10G)? 1: 0);
        if(last_frlType && bSupportFastTimingSwitch  && (last_frlType == tx_output_timing_table[index].frl_type)){
            ScalerHdmiTxSetFastTimingSwitchEn(_ENABLE);
            if(last_pixelClk == tx_cea_timing_table[index].pixelFreq)
                bPixelFmtChange &= ~PIXEL_CLK;
            if(last_colorFmt == tx_output_timing_table[index].color_type)
                bPixelFmtChange &= ~PIXEL_COLOR;
            if(last_colorDepth == tx_output_timing_table[index].color_depth)
                bPixelFmtChange &= ~PIXEL_DEPTH;
        }else{
            ScalerHdmiTxSetFastTimingSwitchEn(_DISABLE);
        }
        ScalerHdmiTxSetPixelFormatChange(bPixelFmtChange);
    }
    last_frlType = tx_output_timing_table[index].frl_type;
    last_pixelClk = tx_cea_timing_table[index].pixelFreq;
    last_colorFmt = tx_output_timing_table[index].color_type;
    last_colorDepth = tx_output_timing_table[index].color_depth;
#endif

    }
    return;
}



// [VRR] Just adjust v-total & vsync period to avoid RX measure error during VRR_EN mode
void Scaler_FastTimingSwitch_VRR_4k2k_setting_by_framerate(int frame_rate)
{
//#ifdef _MARK2_FIXME_H5X_TXSOC_FLOW // [MARK2] FIX-ME -- replace TXSOC setting into main DTG setting flow
    #define TXSOC_PIXEL_CLK_4K2K 594000000
    #define TXSOC_DH_TOTAL_4K2K 4400
    ppoverlay_fix_last_line_pcr_ctrl_RBUS ppoverlay_fix_last_line_pcr_ctrl_reg;
    ppoverlay_dv_total_RBUS ppoverlay_dv_total_reg;
    ppoverlay_uzudtg_dv_total_RBUS ppoverlay_uzudtg_dv_total_reg;
    // pixel clock = H-total * V-total * frame rate
    // V-total = 594000000 / H-total(4400) / frame rate
    UINT32 dv_total;
    UINT32 usVBlankHalf;
    UINT32 curDvTotal, curVsyncPeriod;
    int index = HDMITX_TIMING_INDEX;
    UINT32 vEnd = PPOVERLAY_DV_DEN_Start_End_get_dv_den_end(rtd_inl(PPOVERLAY_DV_DEN_Start_End_reg));
    UINT32 curLineCnt;
    ppoverlay_double_buffer_ctrl_RBUS ppoverlay_double_buffer_ctrl_reg;
    static UINT32 last_applyStc=0;
    UINT32 stc, stcDiff, count;

    if((index < TX_TIMING_HDMI21_4K2KP120_YUV444_8BIT_8G) || (index > TX_TIMING_HDMI21_4K2KP60_YUV420_10BIT_10G)){
        FatalMessageHDMITx("[HDMITX] INVALID Current Video Index[%d]\n", index);
        return;
    }

    if((vrr24Hz? (frame_rate < 239): ((frame_rate < 479)))|| (frame_rate > 600)){
        FatalMessageHDMITx("[HDMITX] frame rate %d over range\n", frame_rate);
        return;
    }

    // wait front-porch
    HDMITX_DTG_Wait_Den_Stop_Done((UINT8*)__FUNCTION__, __LINE__);


    dv_total = (TXSOC_PIXEL_CLK_4K2K/ TXSOC_DH_TOTAL_4K2K) * 10 / frame_rate;
    usVBlankHalf = (dv_total - tx_output_timing_table[index].height) / 2;;

    ppoverlay_dv_total_reg.regValue = rtd_inl(PPOVERLAY_DV_total_reg);
    ppoverlay_uzudtg_dv_total_reg.regValue = rtd_inl(PPOVERLAY_uzudtg_DV_TOTAL_reg);
    ppoverlay_fix_last_line_pcr_ctrl_reg.regValue = rtd_inl(PPOVERLAY_fix_last_line_pcr_ctrl_reg);

    curDvTotal = ppoverlay_uzudtg_dv_total_reg.uzudtg_dv_total+1;
    curVsyncPeriod = ppoverlay_fix_last_line_pcr_ctrl_reg.pcr_vs_free_period;

    // target dv total
    ppoverlay_dv_total_reg.dv_total = dv_total -1;
    rtd_outl(PPOVERLAY_DV_total_reg, ppoverlay_dv_total_reg.regValue);

    ppoverlay_uzudtg_dv_total_reg.uzudtg_dv_total = dv_total -1;
    rtd_outl(PPOVERLAY_uzudtg_DV_TOTAL_reg, ppoverlay_uzudtg_dv_total_reg.regValue);

    // vsync tracking setting
    ppoverlay_fix_last_line_pcr_ctrl_reg.pcr_vs_free_period_update = 1;
    ppoverlay_fix_last_line_pcr_ctrl_reg.pcr_vs_free_period = ((27000000*10)/frame_rate);
    rtd_outl(PPOVERLAY_fix_last_line_pcr_ctrl_reg, ppoverlay_fix_last_line_pcr_ctrl_reg.regValue);

    // V Blanking
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_8_reg, usVBlankHalf >> 8);
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_9_reg, usVBlankHalf);

    fw_scaler_dtg_double_buffer_apply();

    count = 0x3fffff;
    ppoverlay_double_buffer_ctrl_reg.regValue = rtd_inl(PPOVERLAY_Double_Buffer_CTRL_reg);
    while((ppoverlay_double_buffer_ctrl_reg.dreg_dbuf_set) && --count){
        ppoverlay_double_buffer_ctrl_reg.regValue = rtd_inl(PPOVERLAY_Double_Buffer_CTRL_reg);
    }
    if(count==0)
        FatalMessageHDMITx("[HDMI_TX] double buffer timeout !!!@%s\n", __FUNCTION__);

    curLineCnt = PPOVERLAY_new_meas0_linecnt_real_get_line_cnt_rt(rtd_inl(PPOVERLAY_new_meas0_linecnt_real_reg));
    stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
    stcDiff = stc - last_applyStc;
    if((curLineCnt < vEnd)|| ((vrrLoopDelay == 0) && (stcDiff > ((curVsyncPeriod/300)*3/2))))
        FatalMessageHDMITx("[HDMITX] Index[%d] Setting Done@V-end/LC=%d/%d, StcDIff=%d/%d@DB=%d!!\n",
            index, vEnd, curLineCnt, stcDiff, curVsyncPeriod/300, ppoverlay_double_buffer_ctrl_reg.dmainreg_dbuf_en);

    last_applyStc = stc;
    InfoMessageHDMITx("[HDMITX][2] Index[%d] vsPeriod=%d->%d, DvTotal=%d->%d, VBlankHalf=%d->%d\n", index,
        curVsyncPeriod, ((27000000*10)/frame_rate), curDvTotal, dv_total,
        ((rtd_inl(HDMITX21_MAC_HDMI21_IN_CONV_8_reg) << 8)| rtd_inl(HDMITX21_MAC_HDMI21_IN_CONV_9_reg)), usVBlankHalf);

//#endif // #if 0 // [MARK2] FIX-ME -- replace TXSOC setting
    return;
}
#endif

#ifdef ENABLE_FIX_FRL_RATE_FOR_VRR_FAST_TIMING_SWAITCH // for fast timing switch & VRR function verify
// loopMoe 0: jump mode, 1: sequence mode
#define LOOP_MODE_NUM 2
#define LOOP_INDEX_NUM 40
int vrrLoopTable[LOOP_MODE_NUM][LOOP_INDEX_NUM]=
    {{600,500,480,479,600,500,480,479,600,479,500,480,600,479,480,500,600,479,500,480,600,479,600,480,500,600,500,480,479,600,500,480,479,480,500,600,479,500,480,600},
      {600,590,580,570,560,550,540,530,520,510,500,490,480,490,500,510,520,530,540,550,560,570,580,590,600,590,580,570,560,550,540,530,520,510,500,490,480,490,500,600}};

int vrrLoopTable_24hz[LOOP_MODE_NUM][LOOP_INDEX_NUM]=
     {{600,500,480,300,250,240,600,500,480,300,250,240,250,300,480,500,600,240,250,300,480,500,600,500,480,300,250,240,250,300,480,500,600,240,500,300,600,250,480,240},
      {600,590,580,560,540,520,500,480,460,440,420,400,380,360,340,320,300,280,260,250,240,250,260,280,300,320,340,360,380,400,420,440,460,480,500,520,540,560,580,600}};

void Scaler_FastTimingSwitch_VRR_4k2k_loopMode_handler(void)
{
    static UINT8 last_loopMode=0;
    static UINT8 loopDelayCnt=0;
    static UINT8 loopIndex=0;

    // VRR loop mode OFF
    if(vrrLoopMode|| last_loopMode){
        if(vrrLoopMode && (vrrLoopMode <= LOOP_MODE_NUM)){ // VRR jump mode
            if(loopDelayCnt >= vrrLoopDelay){
               int frame_rate;
               if(vrr24Hz)
                frame_rate = vrrLoopTable_24hz[vrrLoopMode-1][loopIndex % LOOP_INDEX_NUM];
               else
                frame_rate = vrrLoopTable[vrrLoopMode-1][loopIndex % LOOP_INDEX_NUM];

               //Scaler_FastTimingSwitch_VRR_4k2k_setting_by_framerate(frame_rate); //fix by zhaodong
               InfoMessageHDMITx("[HDMITX] VRR LOOP Mode/24/FR[%d/%d/%d], Index/DelayCnt=%d/%d(/%d)\n", vrrLoopMode, vrr24Hz, frame_rate, loopIndex, loopDelayCnt, vrrLoopDelay);
               loopDelayCnt = 0;
               loopIndex++;
            }else{
               loopDelayCnt++;
            }
        }else{ // VRR loop mode disable
            InfoMessageHDMITx("[HDMITX] VRR LOOP OFF(%d), Index/DelayCnt=%d/%d(/%d)\n", vrrLoopMode, loopIndex, loopDelayCnt, vrrLoopDelay);
            loopIndex = 0;
            loopDelayCnt = 0;
        }
        last_loopMode = vrrLoopMode;
    }

    return;
}
#endif // #ifdef ENABLE_FIX_FRL_RATE_FOR_VRR_FAST_TIMING_SWAITCH // for fast timing switch & VRR function verify


// [VRR] Just adjust v-total & vsync period to avoid RX measure error during VRR_EN mode
void Scaler_FastTimingSwitch_VRR_4k2k_setting(int index)
{
//#ifdef _MARK2_FIXME_H5X_TXSOC_FLOW // [MARK2] FIX-ME -- replace TXSOC setting into main DTG setting flow
    #define TXSOC_PIXEL_CLK_4K2K 594000000
    #define TXSOC_DH_TOTAL_4K2K 4400
    //ppoverlay_fix_last_line_pcr_ctrl_RBUS ppoverlay_fix_last_line_pcr_ctrl_reg;//spec jing zhou
    ppoverlay_dv_total_RBUS ppoverlay_dv_total_reg;
    ppoverlay_uzudtg_dv_total_RBUS ppoverlay_uzudtg_dv_total_reg;
    // pixel clock = H-total * V-total * frame rate
    // V-total = 594000000 / H-total(4400) / frame rate
    UINT32 frame_rate = tx_output_timing_table.frame_rate;
    UINT32 dv_total = (TXSOC_PIXEL_CLK_4K2K/ TXSOC_DH_TOTAL_4K2K) * 10 / frame_rate;
    UINT32 usVBlankHalf = (dv_total - tx_output_timing_table.height) / 2;;
    ppoverlay_dv_total_reg.regValue = rtd_inl(PPOVERLAY_DV_total_reg);
    ppoverlay_uzudtg_dv_total_reg.regValue = rtd_inl(PPOVERLAY_uzudtg_DV_TOTAL_reg);
    //ppoverlay_fix_last_line_pcr_ctrl_reg.regValue = rtd_inl(PPOVERLAY_fix_last_line_pcr_ctrl_reg);//spec jing zhou
#if 0//spec jing zhou
    InfoMessageHDMITx("[HDMITX] Index[%d] vsPeriod=%d->%d, DvTotal=%d->%d, VBlankHalf=%d->%d\n", index,
        ppoverlay_fix_last_line_pcr_ctrl_reg.pcr_vs_free_period, ((27000000*10)/frame_rate), ppoverlay_uzudtg_dv_total_reg.uzudtg_dv_total+1, dv_total,
        ((rtd_inl(HDMITX21_MAC_HDMI21_IN_CONV_8_reg) << 8)| rtd_inl(HDMITX21_MAC_HDMI21_IN_CONV_9_reg)), usVBlankHalf);
#endif
    // target dv total
    ppoverlay_dv_total_reg.dv_total = dv_total -1;
    rtd_outl(PPOVERLAY_DV_total_reg, ppoverlay_dv_total_reg.regValue);

    ppoverlay_uzudtg_dv_total_reg.uzudtg_dv_total = dv_total -1;
    rtd_outl(PPOVERLAY_uzudtg_DV_TOTAL_reg, ppoverlay_uzudtg_dv_total_reg.regValue);
#if 0//spec jing zhou
    // vsync tracking setting
    ppoverlay_fix_last_line_pcr_ctrl_reg.pcr_vs_free_period_update = 1;
    ppoverlay_fix_last_line_pcr_ctrl_reg.pcr_vs_free_period = ((27000000*10)/frame_rate);
    rtd_outl(PPOVERLAY_fix_last_line_pcr_ctrl_reg, ppoverlay_fix_last_line_pcr_ctrl_reg.regValue);
#endif
    // V Blanking
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_8_reg, usVBlankHalf >> 8);
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_9_reg, usVBlankHalf);
//#endif // #if 0 // [MARK2] FIX-ME -- replace TXSOC setting

    return;
}


void setHdmitxAudioChanel(UINT8 ch_num)
{
	bHdmiTxAudio_ch = ch_num;
	return;
}
UINT8 getHdmitxAudioChanel(void)
{
	return bHdmiTxAudio_ch;
}
void setHdmitxAudioType(UINT8 audio_type)
{
	bHdmiTxAudio_type = audio_type;
	return;
}
UINT8 getHdmitxAudioType(void)
{
	return bHdmiTxAudio_type;
}
void setHdmitxAudioMute(UINT8 audio_mute)
{
	bHdmiTxAudio_mute = audio_mute;
	return;
}
UINT8 getHdmitxAudioMute(void)
{
	return bHdmiTxAudio_mute;
}
void setHdmitxAudioFreq(UINT32 audio_freq)
{
	HdmiTxAudio_freq = audio_freq;
	return;
}
UINT32 getHdmitxAudioFreq(void)
{
	return HdmiTxAudio_freq;
}


