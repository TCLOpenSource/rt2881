
//#include <rbus/ppoverlay_outtg_reg.h>
//#include <rbus/scaleup_reg.h>
//#include <rbus/vgip_reg.h>
#include <rbus/dsce_misc_reg.h>
#include <rbus/hdmitx_misc_reg.h>
#include <rbus/timer_reg.h>
#include <rbus/hdmitx_vgip_reg.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/hdmitx_phy_reg.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_ctrl.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_init.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_lib.h>
#include <rtk_kdriver/tvscalercontrol/txpre/txpre.h>
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
//#include <rtk_kdriver/tvscalercontrol/hdmirx/drvif_hdmirx.h>
#include <rtk_kdriver/measure/rtk_measure.h>
#endif
#include <scaler/scalerCommon.h>

//#include <scaler/scalerDrvCommon.h>
//#include <scaler/scalerstruct.h>
#ifdef CONFIG_ENABLE_DPRX
#include <rtk_kdriver/dprx/drvif_dprx.h>
#endif

/************************
// Data Type declaration
//
*************************/
typedef enum
{
    _RX_QUANTIZATION_DEFAULT = 0x00,
    _RX_QUANTIZATION_LIMITED = 0x01,
    _RX_QUANTIZATION_FULL = 0x02,
    _RX_QUANTIZATION_RESERVED = 0x03,
} EnumRXInfoQuantization;

/************************
// Macro/define
//
*************************/
#define WAIT_TIMEOUT (90*100) // timeout=50ms

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
#define vfe_hdmi_drv_get_ALLM() (_ENABLE)
#define get_HDMI_HDR_mode() (HDR_HDR10_HDMI)
#define get_HDMI_Dolby_VSIF_mode() (DOLBY_HDMI_VSIF_LL)
#define vbe_disp_get_VRR_timingMode_flag() (_ENABLE)
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT

/************************
// Global variable declaration
//
*************************/
HDMITX_INIT_SETTING_TYPE hdmitx_init_setting=
    {
        .init_setting.bHdmitxSettingDoneFlag=0,
        .init_setting.bHdmirxSettingDoneFlag=0,
        .init_setting.bPktBypassEnableFlag=0,
        .init_setting.bSkipAudioPktBypassFlag=0,
        .init_setting.bSkipEMPPktBypassFlag=0,
        .init_setting.bDtgFSyncEnable=0,
        .init_setting.bVrrEnable=0,
        .init_setting.bSkipEdidModify=0
    };

I3DDMA_COLOR_SPACE_T ap_hdmitx_color_format = I3DDMA_COLOR_RGB;
I3DDMA_COLOR_DEPTH_T ap_hdmitx_color_depth = I3DDMA_COLOR_DEPTH_8B;
EnumHdmi21FrlType ap_hdmitx_frl_mode = _HDMI21_FRL_NONE;

UINT8 ptg_timing_index = 0;
hdmi_tx_output_timing_type tx_ptg_output_timing_table[PTG_TIMING_NUM] = {
    {3840,  2160,   1, 600,     I3DDMA_COLOR_YUV444,    I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES},// 4K60*RGB444*8bit frl_none
    {3840,  2160,   1, 1440,    I3DDMA_COLOR_YUV444, I3DDMA_COLOR_DEPTH_10B,  _HDMI21_FRL_12G}, // 4K144*YUV444*10bit frl_12G
    {640,  480,   1, 600,     I3DDMA_COLOR_RGB,    I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES},// 640*480 60hz*RGB444*8bit frl_none
};
/************************
// Local variable declaration
//
*************************/
tx_video_path_type txDtgInputPath=TX_VIDEO_PATH_BYPASS;

hdmi_tx_timing_gen_st tx_cea_timing_table={0};
hdmi_tx_output_timing_type tx_output_timing_table={0};

scaler_hdmi_timing_info scalerHdmiTimingInfo={0,0,0,0,0,0,0,0,0,0,0};



extern UINT8 tx_output_colorSpace;
UINT8 tx_output_colordepth = 0;

UINT8 AudioFreqType = _AUDIO_FREQ_NO_AUDIO;
hdmi_timing_check_type  dtd_timing = {0};
/************************
// Local temp variable declaration
//
*************************/

UINT32 lineCntAddr;

UINT8 g_hdmi4k144RBv2 = 0;
UINT16 out_ori_v_total = 0;

// wait den stop variable
extern UINT32 stc;
UINT16 lineCnt, end;
//UINT16 start;
//static UINT8 wait_zero;
UINT8 need_sw_avi_packet = 0;
//static UINT8 timingDiv4;
#ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
UINT8 edid_dtd_flag = 0;
#endif

StructHdmitxDisplayInfo g_scaler_display_info = {0};
/************************
// Extern variable declaration
//
*************************/
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
extern void ScalerHdmiTxRepeaterSetSourceInfo_aviInfoFrame(void);
extern void ScalerHdmiTxRepeaterSetSourceInfo_Allm(UINT8 enable);
extern void ScalerHdmiTxRepeaterSetSourceInfo_hdrMode(HDR_MODE hdr_mode);
extern void ScalerHdmiTxRepeaterSetSourceInfo_DvVsifMode(DOLBY_HDMI_VSIF_T mode);
extern void ScalerHdmiTxRepeaterSetSourceInfo_vtemVrrEn(UINT8 enable);
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
extern UINT8 ScalerHdmiTxGetDSCEn(void);
extern unsigned char txpre_config(StructTxPreInfo * txpre_info);
extern void Txpretg_set_disp_setting(StructTxPreInfo *txpre_info);

extern UINT8 ApSetTxInit;
extern UINT8 scalerSetTxRun;
extern UINT8 HDMITX_TIMING_INDEX;

extern UINT32  hdmiRepeaterBypassPktHdrEnMask;
extern UINT16 adjust_timing_vtotal;
extern UINT8 adjust_vtotal_flag;
extern UINT16 adjust_vrr_vstart;
extern MEASURE_TIMING_T timing_info;
extern BOOLEAN g_bHdmiMacTx0FindSpecialTv6;
extern StructTxPreInfo txpre_info;
extern unsigned int bypass_port;

/************************
// Extern Function declaration
//
*************************/
extern void ScalerHdmiMacTx0PowerProc(UINT8 enumPowerAction);
extern void ScalerHdmiMacTx0AdjustHdmiMode(void);

extern void ScalerHdmiMacTx0EnablePtg(UINT8 ptgmode,UINT8 enable);
extern void Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_INFO infoList, UINT16 value);
extern UINT16 Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_INFO infoList);
extern void ScalerHdmiMacTx0EdidSetFeature(EnumEdidFeatureType enumEdidFeature, UINT8 ucValue);
extern void ScalerHdmiTxSetTargetFrlRate(EnumHdmi21FrlType frlType);
extern INT8 ScalerHdmiTxPhy_GetTxTimingTable(UINT8 index, hdmi_tx_timing_gen_st *time_mode);
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
extern void drvif_hdmi_pkt_bypass_to_hdmitx_en(unsigned char port, unsigned char enable);
extern unsigned char drvif_Hdmi_Multi_port_GetFrlMode(unsigned char port);
#if(_AUDIO_SUPPORT == _ON)
//extern UINT32 drvif_Hdmi_get_audio_sample_freq(void);
#endif
#endif

extern void ScalerHdmiMacTx0PacketBypassConfig(void);
extern UINT8 ScalerHdmiMacTx0EdidGetFeature(EnumEdidFeatureType enumEdidFeature);
extern void hdmitx_ptg_reset_state(void);
extern UINT8 HDMITX_DTG_Wait_Den_Stop_Done(void);;
extern UINT8 ScalerHdmiMacTxEdidGetFeature(EnumOutputPort enumOutputPort, EnumEdidFeatureType enumEdidFeature);
extern void ScalerHdmiTxSetDSCEn(UINT8 enable);
extern void ScalerHdmiTxSetBypassLinkTrainingEn(UINT8 enable);
#if IS_ENABLED(CONFIG_HDCPTX)
extern void SET_HDCP_TX_VFreq(unsigned char port, unsigned int vfreq);
#endif
/************************
// Function implementation
//
*************************/
#ifdef NOT_USED_NOW
void Scaler_HDMITX_DispSetTxInputPath(tx_video_path_type path)
{
    if((UINT8)path > TX_VIDEO_PATH_SCALER_PQ){
        DebugMessageHDMITx("[HDMITX] INVALID PATH[%d]\n", (UINT32)path);
        return;
    }

    if(path != txDtgInputPath){
        DebugMessageHDMITx("[HDMITX] Set Video Path=%d\n", (UINT32)path);
        txDtgInputPath = path;
    }

    return;
}
#endif

void hdmi_set_timingporch(void)
{
    cea_timing_h_param_st *pCeaTimingHParam=NULL;
    cea_timing_v_param_st *pCeaTimingVParam=NULL;
    scaler_hdmi_timing_info *pScalerHdmiTimingInfo=NULL;

    //p_tx_cea_timing_table = &tx_cea_timing_table[0]+value_u8;
    //ScalerHdmiTxPhy_GetTxTimingTable(timeIdx , p_tx_cea_timing_table);

    pCeaTimingHParam = &tx_cea_timing_table.h;
    pCeaTimingVParam = &tx_cea_timing_table.v;
    pScalerHdmiTimingInfo = &scalerHdmiTimingInfo;

#if 1 // [FIX-ME] avoid DSEG usage
    pScalerHdmiTimingInfo->VO_ACT_H_START = pCeaTimingHParam->sync + pCeaTimingHParam->front;
    pScalerHdmiTimingInfo->VO_ACT_V_START = pCeaTimingVParam->sync + pCeaTimingVParam->front ;
    pScalerHdmiTimingInfo->VO_H_BACKPORCH = pCeaTimingHParam->back ;
    pScalerHdmiTimingInfo->VO_V_BACKPORCH = pCeaTimingVParam->back ;
    pScalerHdmiTimingInfo->DTG_H_DEN_STA = pCeaTimingHParam->back + pCeaTimingHParam->sync;
    pScalerHdmiTimingInfo->DTG_V_DEN_STA = pCeaTimingVParam->back + pCeaTimingVParam->sync;
    pScalerHdmiTimingInfo->DTG_H_PORCH = (pScalerHdmiTimingInfo->VO_ACT_H_START + pScalerHdmiTimingInfo->VO_H_BACKPORCH);
    pScalerHdmiTimingInfo->DTG_V_PORCH = (pScalerHdmiTimingInfo->VO_ACT_V_START + pScalerHdmiTimingInfo->VO_V_BACKPORCH);
    pScalerHdmiTimingInfo->DTG_DH_WIDTH = pCeaTimingHParam->sync;
    pScalerHdmiTimingInfo->DTG_DV_LENGTH = pCeaTimingVParam->sync;
#endif
    pScalerHdmiTimingInfo->DTG_IV2DV_HSYNC_WIDTH = 0x320;

    return;
}


void hdmi_dump_timingporch(void)
{
#if 0 // dump timing info
    timeIdx = hdmitx_ctrl_get_timing_type();
    pScalerHdmiTimingInfo = &scalerHdmiTimingInfo;

    DebugMessageHDMITx("[HDMITX] Path[%d] TimeMode[%d] Timing Info\n", (UINT32)Scaler_HDMITX_DispGetTxInputPath(), (UINT32)timeIdx);

    value_u16 = (Scaler_HDMITX_DispGetTxInputPathFromScalerPQ()? PPOVERLAY_DH_DEN_Start_End_get_dh_den_sta(rtd_inl(PPOVERLAY_DH_DEN_Start_End_reg))
                             : PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_get_dh_den_sta(rtd_inl(PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_reg)));
    DebugMessageHDMITx("[HDMITX] DTG H.Start=%d/%d\n", (UINT32)pScalerHdmiTimingInfo->DTG_H_DEN_STA, (UINT32)value_u16);

    value_u16 = (Scaler_HDMITX_DispGetTxInputPathFromScalerPQ()? PPOVERLAY_DV_DEN_Start_End_get_dv_den_sta(rtd_inl(PPOVERLAY_DV_DEN_Start_End_reg))
                             : PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_get_dv_den_sta(rtd_inl(PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_reg)));
    DebugMessageHDMITx("[HDMITX] DTG V.Start=%d/%d\n", (UINT32)pScalerHdmiTimingInfo->DTG_V_DEN_STA, (UINT32)value_u16);

    DebugMessageHDMITx("[HDMITX] DTG H.Blank=%d\n", (UINT32)pScalerHdmiTimingInfo->DTG_H_PORCH);
    DebugMessageHDMITx("[HDMITX] DTG V.Blank=%d\n", (UINT32)pScalerHdmiTimingInfo->DTG_V_PORCH);

    value_u16 = (Scaler_HDMITX_DispGetTxInputPathFromScalerPQ()? PPOVERLAY_DH_Width_get_dh_width(rtd_inl(PPOVERLAY_DH_Width_reg))
                                : PPOVERLAY_OUTTG_OUTTG_DH_Width_get_dh_width(rtd_inl(PPOVERLAY_OUTTG_OUTTG_DH_Width_reg))) +1;
    DebugMessageHDMITx("[HDMITX] DTG H.Sync=%d/%d\n", (UINT32)pScalerHdmiTimingInfo->DTG_DH_WIDTH, (UINT32)value_u16);

    value_u16 = (Scaler_HDMITX_DispGetTxInputPathFromScalerPQ()? PPOVERLAY_DV_Length_get_dv_length(rtd_inl(PPOVERLAY_DV_Length_reg))
                                : PPOVERLAY_OUTTG_OUTTG_DV_Length_get_dv_length(rtd_inl(PPOVERLAY_OUTTG_OUTTG_DV_Length_reg)))+1;
    DebugMessageHDMITx("[HDMITX] DTG V.Sync=%d/%d\n", (UINT32)pScalerHdmiTimingInfo->DTG_DV_LENGTH, (UINT32)value_u16);

    DebugMessageHDMITx("[HDMITX] DTG IV2DV=%d\n", (UINT32)pScalerHdmiTimingInfo->DTG_IV2DV_HSYNC_WIDTH);
#endif

    return;
}

void Scaler_TestCase_config(void)
{
    //UINT8 bVgipEn;
    scaler_hdmi_timing_info *pScalerHdmiTimingInfo=NULL;
    UINT8 colorDepth,colorFmt,color_imetry,ucFrlType;
    UINT16 width, height, frame_rate, pixel_clock;
    UINT32  h_freq,v_total;
    TX_TIMING_INDEX timeIdx;
    //I3DDMA_COLOR_DEPTH_T colorDepth;
    //I3DDMA_COLOR_SPACE_T colorFmt;
    UINT16 value_u16;
#ifdef CONFIG_ENABLE_TV_SCALER
    UINT8 value_u8;
    UINT8 color_range = 0;
#endif
    timeIdx = hdmitx_ctrl_get_timing_type();
    if(timeIdx >= TX_TIMING_NUM){
        FatalMessageHDMITx("[HDMITX] INVALID idx[%d]@TestCase\n", (UINT32)timeIdx);
        return;
    }


    pScalerHdmiTimingInfo = &scalerHdmiTimingInfo;
    //p_tx_cea_timing_table = &tx_cea_timing_table[0]+timeIdx;
    //ScalerHdmiTxPhy_GetTxTimingTable(timeIdx, p_tx_cea_timing_table);
    //p_tx_output_timing_table = &tx_output_timing_table[0]+timeIdx;

    width = tx_cea_timing_table.h.active;
    height = tx_cea_timing_table.v.active;
    frame_rate = tx_output_timing_table.frame_rate;

    // [FIX-ME] TX output same color format as RX: SHOULD CHECK SINK SUPPORT 420 to 444 OR NOT
    //bVgipEn = VGIP_VGIP_CHN1_CTRL_get_ch1_ivrun(rtd_inl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg));
#ifdef CONFIG_ENABLE_TV_SCALER
    if((get_hdmitxOutputMode() != TX_OUT_TEST_MODE) && g_scaler_display_info.VSC_isConnect){
        colorFmt = tx_output_timing_table.color_type;//g_scaler_display_info.color_space;
        colorDepth = tx_output_timing_table.color_depth;
        color_imetry = g_scaler_display_info.color_imetry;
    } else 
#else
    {
        colorFmt = tx_output_timing_table.color_type;
        colorDepth = tx_output_timing_table.color_depth;
        color_imetry = _COLORIMETRY_RGB_SRGB;
    }
#endif

    pixel_clock = tx_cea_timing_table.pixelFreq;

    // set target FRL type
    ucFrlType = tx_output_timing_table.frl_type;
    ScalerHdmiTxSetTargetFrlRate(ucFrlType);
InfoMessageHDMITx("pixel clock=%d frlTyp1e=%d\n", (UINT32)pixel_clock, (UINT32)ucFrlType);
    // scaler data format
     Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_COLOR_SPACE, colorFmt);
     Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_COLOR_DEPTH, colorDepth);
     Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_COLOR_IMETRY, color_imetry);
    ScalerHdmiMacTx0EdidSetFeature(_HDMI_MAX_TMDS_CLK, 120); // TMDS clock = 600MHz

#if 1
    InfoMessageHDMITx("TimeIdx=%d\n", (UINT32)timeIdx);
    InfoMessageHDMITx("width=%d height=%d\n", (UINT32)width, (UINT32)height);
    InfoMessageHDMITx("Framerate=%d pixelDepth=%d\n", (UINT32)frame_rate, (UINT32)colorDepth);
    InfoMessageHDMITx("pixel format=%d pixel color_imetry=%d\n", (UINT32)colorFmt, (UINT32)color_imetry);
    InfoMessageHDMITx("pixel clock=%d frlType=%d\n", (UINT32)pixel_clock, (UINT32)ucFrlType);
#endif

    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_TX_OUTPUT_PIXEL_CLOCK, pixel_clock);
    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_TX_CLK_4PIXEL_MODE, 1); // VO will enable 4 pixel mode in default
    //Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_HDMITX_DSC_SRC, DSC_SRC_DISPD); // VO will enable 4 pixel mode in default
    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_TX_OUTPUT_VFREQ, frame_rate);

#ifdef CONFIG_ENABLE_TV_SCALER
     value_u8 = g_scaler_display_info.color_ext_imetry; // _COLORIMETRY_EXT_RGB_SRGB
     Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_COLOR_EXT_IMETRY, value_u8);
     if(g_scaler_display_info.input_src == 0)
         color_range = g_scaler_display_info.color_range;
     else
         color_range = timing_info.colorrange;
     FatalMessageHDMITx("color_range=%d\n", (UINT32)color_range);
     if(colorFmt == I3DDMA_COLOR_RGB){
        if(color_range == _RX_QUANTIZATION_DEFAULT)
            value_u8 = _RGB_QUANTIZATION_DEFAULT_RANGE;
        else if(color_range == _RX_QUANTIZATION_LIMITED)
            value_u8 = _RGB_QUANTIZATION_LIMIT_RANGE;
        else if(color_range == _RX_QUANTIZATION_FULL)
            value_u8 = _RGB_QUANTIZATION_FULL_RANGE;
        else
            value_u8 = _RGB_QUANTIZATION_RESERVED;
     }else{//yuv range
        if(color_range == _RX_QUANTIZATION_LIMITED)
            value_u8 = _YCC_QUANTIZATION_LIMIT_RANGE;
        else if(color_range == _RX_QUANTIZATION_FULL)
            value_u8 = _YCC_QUANTIZATION_FULL_RANGE;
        else
            value_u8 = _YCC_QUANTIZATION_RESERVED;
     }
     Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_COLOR_RANGE, value_u8);

    value_u8 = g_scaler_display_info.Interlace;
    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_INTERLACE, value_u8);
#else
     Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_COLOR_EXT_IMETRY, _COLORIMETRY_EXT_RGB_SRGB);
     Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_COLOR_RANGE, _RGB_QUANTIZATION_FULL_RANGE);
    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_INTERLACE, 0);
#endif // #ifdef CONFIG_ENABLE_TV_SCALER

    // scaler output timing
    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_TX_OUTPUT_H_TOTAL, width + pScalerHdmiTimingInfo->DTG_H_PORCH); // H-total

    // avoid overflow, kernel not support div operation of unsigned long long
    // h_freq = ((width * height /100) * frame_rate /10) /(width + DTG_H_PORCH)/100;
    value_u16 = width + pScalerHdmiTimingInfo->DTG_H_PORCH;
    h_freq = ((UINT32)width * height) /100;
    h_freq = ((UINT32)h_freq * frame_rate)/10;
    h_freq = h_freq / value_u16 / 100;
    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_IHFREQ, h_freq); // h_freq = plane->vsyncgen_pixel_freq / htotal / 100 (0.1KHz)
    //InfoMessageHDMITx("[HDMITX] h_freq=%d\n", (UINT32)h_freq);

    //value_u8 = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_INTERLACE);
    //DebugMessageHDMITx("Interlace=%d\n", (UINT32)value_u8);

    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_TX_OUTPUT_H_ACT_WID, width);
    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_TX_OUTPUT_H_ACT_STA, pScalerHdmiTimingInfo->DTG_H_DEN_STA);
    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_TX_OUTPUT_HSYNC, pScalerHdmiTimingInfo->DTG_DH_WIDTH);
    v_total = height + pScalerHdmiTimingInfo->DTG_V_PORCH;
    SET_TX_ORI_VTOTAL(v_total);
    // VIC 1, 2 is original 59Hz don't need change v-total
    if((frame_rate == 599)|| (frame_rate == 479)|| (frame_rate == 299)|| (frame_rate == 239))
        v_total = (v_total < 1000? v_total+1: ((UINT32)v_total * 1001) / 1000);
    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_TX_OUTPUT_V_TOTAL, v_total); // v-total
    DebugMessageHDMITx("v_total=%d\n", (UINT32)v_total);

    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_TX_OUTPUT_V_ACT_LEN, height);
    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_TX_OUTPUT_V_ACT_STA, pScalerHdmiTimingInfo->DTG_V_DEN_STA);
    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_TX_OUTPUT_VSYNC, pScalerHdmiTimingInfo->DTG_DV_LENGTH);
    InfoMessageHDMITx("hstart=%d vstart=%d\n", (UINT32)pScalerHdmiTimingInfo->DTG_H_DEN_STA, (UINT32)pScalerHdmiTimingInfo->DTG_V_DEN_STA);
    InfoMessageHDMITx("hporch=%d vporch=%d\n", (UINT32)pScalerHdmiTimingInfo->DTG_H_PORCH, (UINT32)pScalerHdmiTimingInfo->DTG_V_PORCH);

    DebugMessageHDMITx("[HDMITX] Scaler_TestCase_config Done!!\n");

    return;
}


#ifdef CONFIG_HDMITX_TX_MENU_TEST_SUPPORT
void HDMITX_drv_menuCmd_showTimingList(void)
{
    int i = 0;

    DebugMessageHDMITx(LOGGER_ERROR, "List for hdmi tx output:\n");

    for(i = 0; i < TX_TIMING_NUM; i++) {
        //p_tx_output_timing_table = &tx_output_timing_table[0]+i;
        if(HDMITX_GetTxOutputTimingTable(i, &tx_output_timing_table) != 0){
            DebugMessageHDMITx(LOGGER_ERROR, "[HDMITX] Fail to Get TxOutputTimingTable[%d]\n", (UINT32)i);
            continue;
        }

        DebugMessageHDMITx(LOGGER_ERROR, "[%d] HDMI 2.%x ", (UINT32)i, (UINT32)(tx_output_timing_table.frl_type? 1: 0));
        DebugMessageHDMITx(LOGGER_ERROR, "%d x %x", (UINT32)tx_output_timing_table.width, (UINT32)tx_output_timing_table.height);
        //DebugMessageHDMITx(LOGGER_ERROR, "%s%d", (tx_output_timing_table.progressive? "p":"i"), (UINT32)tx_output_timing_table.frame_rate);
        if(tx_output_timing_table.progressive)
            DebugMessageHDMITx(LOGGER_ERROR, "p%x", (UINT32)tx_output_timing_table.frame_rate);
        else DebugMessageHDMITx(LOGGER_ERROR, "i%x", (UINT32)tx_output_timing_table.frame_rate);
        DebugMessageHDMITx(LOGGER_ERROR, " Fmt/Depth[%x/%x]\n", (UINT32)tx_output_timing_table.color_type, (UINT32)tx_output_timing_table.color_depth);
        DebugMessageHDMITx(LOGGER_FATAL, " FRL=%x\n",  (UINT32)tx_output_timing_table.frl_type);
    }

    return;
}


void HDMITX_drv_menuCmd_setTxBypassTimingMode(UINT8 outTimeMode)
{
    // set TX output timing mode by force ScalerHdmiMacTx0RepeaterGetTxOutputModeTiming() return value
    DebugMessageHDMITx(LOGGER_ERROR, "[HDMITX] Set Bypass Timing Mode=%d\n", (UINT32)outTimeMode);

    if(outTimeMode >= TX_TIMING_NUM){
        DebugMessageHDMITx(LOGGER_ERROR, "[HDMITX] Time Mode[%d] Over Range(%d)!", (UINT32)outTimeMode, (UINT32)TX_TIMING_NUM);
        return;
    }

    // SCRIPT_TX_ENABLE_PACKET_BYPASS()=1 // RX bypass to TX mode
    // ScalerHdmiMacTx0RepeaterGetTxOutputModeTiming()=X // force output timing mode

    return;
}
#endif
extern UINT8 Hdmitx_PtgMode;

void HDMITX_drv_menuCmd_setTxPtgTimingMode(UINT8 timeMode)
{
    // set TX output timing mode and TX run in PTG mode
    

    if(timeMode >= TX_TIMING_NUM){
        //DebugMessageHDMITx(LOGGER_ERROR, "[HDMITX] Time Mode[%d] Over Range(%d)!", (UINT32)timeMode, (UINT32)TX_TIMING_NUM);
        return;
    }
    ptg_timing_index = timeMode;
    if(Hdmitx_PtgMode)
        rtd_maskl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg, ~HDMITX_VGIP_HDMITX_VGIP_CTRL_hdmitx_ivrun_mask, HDMITX_VGIP_HDMITX_VGIP_CTRL_hdmitx_ivrun(0)); // disable vgip for PTG mode
    else
        rtd_maskl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg, ~HDMITX_VGIP_HDMITX_VGIP_CTRL_hdmitx_ivrun_mask, HDMITX_VGIP_HDMITX_VGIP_CTRL_hdmitx_ivrun(1));
    hdmitx_ptg_reset_state();
#if 0
    if(timeMode == 0)
        HDMITX_TIMING_INDEX = TX_TIMING_DSCE_START; // output timing mode 4k60
    else if(timeMode == 1)
        HDMITX_TIMING_INDEX = TX_TIMING_4K2KP144_CVT_RB; // output timing mode 4k144
    else
        HDMITX_TIMING_INDEX = TX_TIMING_640X480P60; //output timing mode 640*480 for CTS
#else
    HDMITX_TIMING_INDEX = timeMode;
#endif
    ApSetTxInit = 1; // TX init
    scalerSetTxRun = 1; // scaler/TX run
    FatalMessageHDMITx("[HDMITX]PTG Timing Mode=%d %d\n", (UINT32)HDMITX_TIMING_INDEX,Hdmitx_PtgMode);
    return;
}

void HDMITX_drv_menuCmd_set_colorFormat(I3DDMA_COLOR_SPACE_T colorfmt)
{
    InfoMessageHDMITx("[HDMITX]menuCmd set colorFormat=%d\n", (UINT32)colorfmt);
    SET_HDMITX_AP_COLOR_FORMAT(colorfmt);
    return;
}
void HDMITX_drv_menuCmd_set_colorDepth(I3DDMA_COLOR_DEPTH_T colordepth)
{
    InfoMessageHDMITx("[HDMITX]menuCmd set colorDepth=%d\n", (UINT32)colordepth);
    SET_HDMITX_AP_COLOR_DEPTH(colordepth);
    return;
}

void HDMITX_drv_menuCmd_set_frlMode(EnumHdmi21FrlType frlmode)
{
    InfoMessageHDMITx("[HDMITX]menuCmd set frlmode=%d\n", (UINT32)frlmode);
    SET_HDMITX_AP_FRL_MODE(frlmode);
    return;
}
void HDMITX_drv_menuCmd_set_bypassLT(UINT8 bypass_lt_en)
{
    InfoMessageHDMITx("[HDMITX]menuCmd set bypass_lt_en=%d\n", (UINT32)bypass_lt_en);
    ScalerHdmiTxSetBypassLinkTrainingEn(bypass_lt_en);
    return;
}

UINT8 HDMITX_check_dtg_tx_timing_sync_status(void)
{
#ifdef CONFIG_ENABLE_VBY1RX
    extern vsc_disp_info_t disp_info;
    UINT16 width, height, frame_rate;
    width = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_ACT_LEN);
    height = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_ACT_LEN);
    frame_rate = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_IVFREQ);
    if(width != disp_info.h_act_wid)
        return _FALSE;
    if(height != disp_info.v_act_len)
        return _FALSE;
    if(ABS(frame_rate, (disp_info.v_freq/10)) > 5)
        return _FALSE;
#endif
    return _TRUE;
}

UINT8 HDMITX_check_vby1rx_lock_status(void)
{
#ifdef CONFIG_ENABLE_VBY1RX
    if(vby1rx_drv_get_state() == VFE_VBY1RX_DRV_SERVICE_STATE)
        return 1;
#endif // #ifdef CONFIG_ENABLE_VBY1RX
    return 0;
}

void HDMITX_drv_set_hdmitx_gate_sel(void)
{
    //#define HDMITX_DCLK_GATE_SEL1_TX_MASK ((UINT32)(SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_2p_gate_sel_mask|SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_420_2p_gate_sel_mask))
    UINT32 value_u32;
    if(tx_cea_timing_table.pixelFreq <= 6000){ // [RX 1P] 0x0001101x
        value_u32 = 4;
    }else{ // [RX 2P] TX_OUTPUT_PixelClock > 5940, 0x0000100x
        value_u32 = 0;
    }
    rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_hdmitx_ds444_pre_2p_mask, HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_hdmitx_ds444_pre_2p(value_u32));
    InfoMessageHDMITx("[txpre] gate_sel = %d\n", value_u32);
}

extern UINT8 u8hdmitx_Hdcp_state;
void HDMITX_drv_misc_set_patternGen(void)
{
    TX_TIMING_INDEX timeIdx;
    UINT16 value_u16;
    UINT8 bVby1RxLock, bTimingSync, bVgipEn;
    timeIdx = hdmitx_ctrl_get_timing_type();
    if(timeIdx >= TX_TIMING_NUM){
        //InfoMessageHDMITx("[HDMITX] INVALID TIMING MODE[%d]@HDMITX_drv_misc_set_patternGen\n", (UINT32)timeIdx);
        return;
    }

    bVby1RxLock = HDMITX_check_vby1rx_lock_status();
    bTimingSync = HDMITX_check_dtg_tx_timing_sync_status();
    bVgipEn = HDMITX_VGIP_HDMITX_VGIP_CTRL_get_hdmitx_ivrun(rtd_inl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg));

    //InfoMessageHDMITx("[HDMITX] set_patternGen, Index[%d]:\n", (UINT32)timeIdx);
    //InfoMessageHDMITx("RXLock/DtgSync=[%d/%d]\n", (UINT32)bVby1RxLock, (UINT32)bTimingSync);
    //InfoMessageHDMITx("VgipEn=[%d]\n", (UINT32)bVgipEn);

    //p_tx_cea_timing_table = &tx_cea_timing_table[0] + timeIdx;
    //ScalerHdmiTxPhy_GetTxTimingTable(timeIdx, p_tx_cea_timing_table);

    // DTG PTG
    if(Scaler_HDMITX_DispGetTxInputPathFromScalerPQ()){
        // TXSOC UZU PTG
        #if 0//fix by zhaodong
        rtd_outl(SCALEUP_DM_UZU_PATGEN_COLOR_C0_0_reg, 0xffffffff);
        rtd_outl(SCALEUP_DM_UZU_PATGEN_COLOR_C0_1_reg, 0xffff0000);
        rtd_outl(SCALEUP_DM_UZU_PATGEN_COLOR_C1_0_reg, 0xffff0000);
        rtd_outl(SCALEUP_DM_UZU_PATGEN_COLOR_C1_1_reg, 0x00000000);
        rtd_outl(SCALEUP_DM_UZU_PATGEN_COLOR_C2_0_reg, 0x0000ffff);
        rtd_outl(SCALEUP_DM_UZU_PATGEN_COLOR_C2_1_reg, 0x00000000);
        rtd_outl(SCALEUP_DM_UZU_PATGEN_COLOR_C3_0_reg, 0x00000000);
        rtd_outl(SCALEUP_DM_UZU_PATGEN_COLOR_C3_1_reg, 0xffff0000);

        //rtd_outl(SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE_reg, _BIT29|_BIT28|_BIT27|(DBG_EN_TOGGLE_PTG << 25)|(width > 1920? 0x1df: 0x27)/*((width/4)-1)*/); // PTG width B[21:0], PTG toggle B[26:25]: 0: no toggle, 1: 0/1 toggle, 2: 0/1/2 toggle, 3: 0/1/2/3 toggle
        value_u16 = 7;
        rtd_maskl(SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE_reg, ~SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE_patgen_tog_num_mask, SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE_patgen_tog_num(value_u16));
        rtd_maskl(SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE_reg, ~SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE_patgen_tog_sel_mask, SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE_patgen_tog_sel(1));
        value_u16 = (tx_cea_timing_table.h.active/4) -1; // (width/4) -1;
        rtd_maskl(SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE_reg, ~SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE_patgen_width_mask, SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE_patgen_width(value_u16));

        //rtd_outl(SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE1_reg, (width/8)-1);
        value_u16 = (tx_cea_timing_table.h.active / 8) -1;
        rtd_maskl(SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE1_reg, ~SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE1_patgen_width_1_mask, SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE1_patgen_width_1(value_u16));

        //rtd_outl(SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE2_reg, (width/12)-1);
        value_u16 = (tx_cea_timing_table.h.active / 12) -1;
        rtd_maskl(SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE2_reg, ~SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE2_patgen_width_2_mask, SCALEUP_DM_UZU_PATGEN_FRAME_TOGGLE2_patgen_width_2(value_u16));

    /*
        // RGB2YUV enable control
        if(colorFmt == I3DDMA_COLOR_RGB){ // TX output format is RGB
            drvif_color_hdmitx_rgb2yuv_en(false);
        }else{ // YUV
            drvif_color_hdmitx_rgb2yuv_en(true);
        }
    */

        // enable PTG
        // // PTG B[6:4]: 1: randon, 2: build-in 4 color, 3: build-in 8 color, 4: self 4 color, 5: self 8 color
        value_u16 = 3;
        rtd_maskl(SCALEUP_DM_UZU_Globle_Ctrl_reg, ~SCALEUP_DM_UZU_Globle_Ctrl_patgen_mode_mask, SCALEUP_DM_UZU_Globle_Ctrl_patgen_mode(value_u16));

        // 1: Pattern generator connect to MAIN channel
        value_u16 = ((bVby1RxLock|| bVgipEn) && bTimingSync? 0: 0x1);
        rtd_maskl(SCALEUP_DM_UZU_Globle_Ctrl_reg, ~SCALEUP_DM_UZU_Globle_Ctrl_patgen_sel_mask, SCALEUP_DM_UZU_Globle_Ctrl_patgen_sel(value_u16));
#endif
    } else if((!bVgipEn) && (ScalerHdmiTxGetDSCEn() == TRUE)) {// DSCE HDMITX PTG
        //==================================== Pattern Generator ====================================
        //		0: Random disable, color bar
        //		1: Random pattern (Seed=38'h1)
        //		2: Random pattern (Seed={16'h0,PATGEN_WIDTH})

        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_reg,30,29,0x0);// 0: Random disable, color bar
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_reg, ~DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_patgen_rand_mode_mask,DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_patgen_rand_mode(0x0));// 0: Random disable, color bar

        //HDMITX_IN_PATTERN_GEN_CTRL_0	0x180094D8 21:0 PATGEN_WIDTH => Pattern generator color bar width
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg,21,0,x-size/8-1);// Pattern generator color bar width => (3840)/8 -1 = 0x1df
        if(tx_output_timing_table.frl_type > 0) //hdmi2.1 Pattern generator color bar width => (3840)/8 -1 = 0x1df
            value_u16 = (tx_cea_timing_table.h.active / 8) - 1;
        else //hdmi2.0 Pattern generator color bar width => (3840)/2 -1 for 4pixel mode
            value_u16 = (tx_cea_timing_table.h.active / 4) - 1;
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg,21,0,value_u16);// Pattern generator color bar width =>  (3840)/8 -1 = 0x1df
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_reg, ~DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_patgen_width_mask, DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_patgen_width(value_u16));// Pattern generator color bar width =>  (3840)/8 -1 = 0x1df

        // color 0 G=3FF B=000 R=000
        // color 1 G=000 B=3FF R=000
        // color 2 G=000 B=000 R=3FF
        // color 3 G=3FF B=3FF R=3FF
        // color 4 G=000 B=000 R=000
        // color 5 G=000 B=3FF R=3FF
        // color 6 G=3FF B=3FF R=000
        // color 7 G=3FF B=000 R=3FF


        // color 0 G=3FF B=000 R=000
        //PATGEN_C0_G 0x180094DC 23:12	R/W	0x0F	Pattern generator color 0 G[11:0]
        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_C0_RG_reg,23,12,0xfff);
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C0_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C0_RG_patgen_c0_g_mask, DSCE_MISC_DSC_PATTERN_GEN_C0_RG_patgen_c0_g(0xfff));
        //PATGEN_C0_B	0x180094E0:0	R/W	0x0F	Pattern generator color 0 B[11:0]
        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_C0_B_reg,11,0, 0xfff);
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C0_B_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C0_B_patgen_c0_b_mask, DSCE_MISC_DSC_PATTERN_GEN_C0_B_patgen_c0_b(0xfff));
        //PATGEN_C0_R 0x180094DC 11:0	R/W	0x0F	Pattern generator color 0 R[11:0]
        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_C0_RG_reg,11,0, 0xfff);
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C0_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C0_RG_patgen_c0_r_mask, DSCE_MISC_DSC_PATTERN_GEN_C0_RG_patgen_c0_r(0xfff));

        // color 1 G=000 B=3FF R=000
        //PATGEN_C1_G 0x180094E4 23:12	R/W	0x0F	Pattern generator color 1 G[11:0]
        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_C1_RG_reg,23,12, 0x0);
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C1_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C1_RG_patgen_c1_g_mask, DSCE_MISC_DSC_PATTERN_GEN_C1_RG_patgen_c1_g(0xfff));
        //PATGEN_C1_B	0x180094E8:0	R/W	0x0F	Pattern generator color 1 B[11:0]
        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_C1_B_reg,11,0, 0x0);
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C1_B_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C1_B_patgen_c1_b_mask, DSCE_MISC_DSC_PATTERN_GEN_C1_B_patgen_c1_b(0x0));
        //PATGEN_C1_R 0x180094E4 11:0	R/W	0x0F	Pattern generator color 1 R[11:0]
        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_C1_RG_reg,11,0, 0xfff);
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C1_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C1_RG_patgen_c1_r_mask, DSCE_MISC_DSC_PATTERN_GEN_C1_RG_patgen_c1_r(0xfff));

        // color 2 G=000 B=000 R=3FF
        //PATGEN_C2_G HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_reg 23:12	R/W	0x0F	Pattern generator color 2 G[11:0]
        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_C2_RG_reg,23,12, 0x0);
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C2_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C2_RG_patgen_c2_g_mask, DSCE_MISC_DSC_PATTERN_GEN_C2_RG_patgen_c2_g(0xfff));
        //PATGEN_C2_B	0x180094F0 11:0	R/W	0x0F	Pattern generator color 2 B[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_B_reg,11,0, 0xfff);
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C2_B_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C2_B_patgen_c2_b_mask, DSCE_MISC_DSC_PATTERN_GEN_C2_B_patgen_c2_b(0xfff));
        //PATGEN_C2_R 0x180094EC 11:0	R/W	0x0F	Pattern generator color 2 R[11:0]
        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_C2_RG_reg,11,0, 0x0);
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C2_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C2_RG_patgen_c2_r_mask, DSCE_MISC_DSC_PATTERN_GEN_C2_RG_patgen_c2_r(0x0));

        // color 3 G=3FF B=3FF R=3FF
        //PATGEN_C3_G 0x180094F4 23:12	R/W	0x0F	Pattern generator color 3 G[11:0]
        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_C3_RG_reg,23,12, 0xfff);
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C3_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C3_RG_patgen_c3_g_mask, DSCE_MISC_DSC_PATTERN_GEN_C3_RG_patgen_c3_g(0xfff));
        //PATGEN_C3_B	0x180BB2A0 11:0	R/W	0x0F	Pattern generator color 3 B[11:0]
        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_C3_B_reg,11,0, 0x0);
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C3_B_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C3_B_patgen_c3_b_mask, DSCE_MISC_DSC_PATTERN_GEN_C3_B_patgen_c3_b(0x0));
        //PATGEN_C3_R 0x180094F4 11:0	R/W	0x0F	Pattern generator color 3 R[11:0]
        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_C3_RG_reg,11,0, 0x0);
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C3_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C3_RG_patgen_c3_r_mask, DSCE_MISC_DSC_PATTERN_GEN_C3_RG_patgen_c3_r(0x0));

        // color 4 G=000 B=000 R=000
        //PATGEN_C2_G 0x180194FC 23:12	R/W	0x0F	Pattern generator color 2 G[11:0]
         rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C4_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C4_RG_patgen_c4_g_mask, DSCE_MISC_DSC_PATTERN_GEN_C4_RG_patgen_c4_g(0x0));
        //PATGEN_C2_B	0x1801950011:0	R/W	0x0F	Pattern generator color 2 B[11:0]
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C4_B_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C4_B_patgen_c4_b_mask, DSCE_MISC_DSC_PATTERN_GEN_C4_B_patgen_c4_b(0xfff));
        //PATGEN_C2_R 0x180194FC 11:0	R/W	0x0F	Pattern generator color 2 R[11:0]
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C4_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C4_RG_patgen_c4_r_mask, DSCE_MISC_DSC_PATTERN_GEN_C4_RG_patgen_c4_r(0xfff));

        // color 5 G=000 B=3FF R=3FF
        //PATGEN_C5_G 0x18019504 23:12	R/W	0x0F	Pattern generator color 5 G[11:0]
         rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C5_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C5_RG_patgen_c5_g_mask, DSCE_MISC_DSC_PATTERN_GEN_C5_RG_patgen_c5_g(0x0));
        //PATGEN_C5_B	0x1801950811:0	R/W	0x0F	Pattern generator color 5 B[11:0]
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C5_B_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C5_B_patgen_c5_b_mask, DSCE_MISC_DSC_PATTERN_GEN_C5_B_patgen_c5_b(0x0));
        //PATGEN_C5_R 0x18019504 11:0	R/W	0x0F	Pattern generator color 5 R[11:0]
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C5_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C5_RG_patgen_c5_r_mask, DSCE_MISC_DSC_PATTERN_GEN_C5_RG_patgen_c5_r(0xfff));

        // color 6 G=3FF B=3FF R=000
        //PATGEN_C6_G 0x1801950C 23:12	R/W	0x0F	Pattern generator color 6 G[11:0]
         rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C6_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C6_RG_patgen_c6_g_mask, DSCE_MISC_DSC_PATTERN_GEN_C6_RG_patgen_c6_g(0x0));
        //PATGEN_C6_B	0x1801951011:0	R/W	0x0F	Pattern generator color 6 B[11:0]
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C6_B_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C6_B_patgen_c6_b_mask, DSCE_MISC_DSC_PATTERN_GEN_C6_B_patgen_c6_b(0xfff));
        //PATGEN_C6_R 0x1801950C 11:0	R/W	0x0F	Pattern generator color 6 R[11:0]
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C6_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C6_RG_patgen_c6_r_mask, DSCE_MISC_DSC_PATTERN_GEN_C6_RG_patgen_c6_r(0x0));

        // color 7 G=3FF B=000 R=3FF
        //PATGEN_C7_G 0x18019514 23:12	R/W	0x0F	Pattern generator color 7 G[11:0]
         rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C7_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C7_RG_patgen_c7_g_mask, DSCE_MISC_DSC_PATTERN_GEN_C7_RG_patgen_c7_g(0x0));
        //PATGEN_C7_B	0x1801951811:0	R/W	0x0F	Pattern generator color 7 B[11:0]
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C7_B_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C7_B_patgen_c7_b_mask, DSCE_MISC_DSC_PATTERN_GEN_C7_B_patgen_c7_b(0x0));
        //PATGEN_C7_R 0x18019514 11:0	R/W	0x0F	Pattern generator color 7 R[11:0]
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_C7_RG_reg, ~DSCE_MISC_DSC_PATTERN_GEN_C7_RG_patgen_c7_r_mask, DSCE_MISC_DSC_PATTERN_GEN_C7_RG_patgen_c7_r(0x0));

        if(u8hdmitx_Hdcp_state){
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C0_B_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C0_RG_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C1_B_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C1_RG_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C2_B_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C2_RG_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C3_B_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C3_RG_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C4_B_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C4_RG_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C5_B_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C5_RG_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C6_B_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C6_RG_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C7_B_reg, 0x00000000);
            rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C7_RG_reg, 0x00000000);
        }
        value_u16 = ((bVby1RxLock|| bVgipEn) && bTimingSync? 0: 0x1); // 0: Pattern generator disable for VBY1 37.125M, 1: Pattern generator enable for 27M
        //rtd_part_outl(DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_reg,31,31,value_u16);//PATGEN_EN[31]Pattern generator: 0: Pattern generator disable for VBY1 37.125M, 1: Pattern generator enable for 27M
        rtd_maskl(DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_reg, ~DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_patgen_en_mask, DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_patgen_en(value_u16));//PATGEN_EN[31]Pattern generator: 0: Pattern generator disable for VBY1 37.125M, 1: Pattern generator enable for 27M
    }else{// HDMITX PTG
        //==================================== Pattern Generator ====================================
        //		0: Random disable, color bar
        //		1: Random pattern (Seed=38'h1)
        //		2: Random pattern (Seed={16'h0,PATGEN_WIDTH})

        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg,30,29,0x0);// 0: Random disable, color bar
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_rand_mode_mask,HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_rand_mode(0x0));// 0: Random disable, color bar

        //HDMITX_IN_PATTERN_GEN_CTRL_0	0x180094D8 21:0 PATGEN_WIDTH => Pattern generator color bar width
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg,21,0,x-size/8-1);// Pattern generator color bar width => (3840)/8 -1 = 0x1df
        if(tx_output_timing_table.frl_type > 0) //hdmi2.1 Pattern generator color bar width => (3840)/8 -1 = 0x1df
            value_u16 = (tx_cea_timing_table.h.active / 8) - 1;
        else //hdmi2.0 Pattern generator color bar width => (3840)/2 -1 for 4pixel mode
            value_u16 = (tx_cea_timing_table.h.active / 2) - 1;
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg,21,0,value_u16);// Pattern generator color bar width =>  (3840)/8 -1 = 0x1df
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_width_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_width(value_u16));// Pattern generator color bar width =>  (3840)/8 -1 = 0x1df

        // color 0 G=3FF B=000 R=000
        // color 1 G=000 B=3FF R=000
        // color 2 G=000 B=000 R=3FF
        // color 3 G=3FF B=3FF R=3FF
        // color 4 G=000 B=000 R=000
        // color 5 G=000 B=3FF R=3FF
        // color 6 G=3FF B=3FF R=000
        // color 7 G=3FF B=000 R=3FF


        // color 0 G=3FF B=000 R=000
        //PATGEN_C0_G 0x180094DC 23:12	R/W	0x0F	Pattern generator color 0 G[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_reg,23,12,0xfff);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_patgen_c0_g_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_patgen_c0_g(0xfff));
        //PATGEN_C0_B	0x180094E0:0	R/W	0x0F	Pattern generator color 0 B[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_B_reg,11,0, 0xfff);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_B_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_B_patgen_c0_b_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_B_patgen_c0_b(0xfff));
        //PATGEN_C0_R 0x180094DC 11:0	R/W	0x0F	Pattern generator color 0 R[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_reg,11,0, 0xfff);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_patgen_c0_r_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_patgen_c0_r(0xfff));

        // color 1 G=000 B=3FF R=000
        //PATGEN_C1_G 0x180094E4 23:12	R/W	0x0F	Pattern generator color 1 G[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_reg,23,12, 0x0);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_patgen_c1_g_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_patgen_c1_g(0xfff));
        //PATGEN_C1_B	0x180094E8:0	R/W	0x0F	Pattern generator color 1 B[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_B_reg,11,0, 0x0);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_B_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_B_patgen_c1_b_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_B_patgen_c1_b(0x0));
        //PATGEN_C1_R 0x180094E4 11:0	R/W	0x0F	Pattern generator color 1 R[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_reg,11,0, 0xfff);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_patgen_c1_r_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_patgen_c1_r(0xfff));

        // color 2 G=000 B=000 R=3FF
        //PATGEN_C2_G HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_reg 23:12	R/W	0x0F	Pattern generator color 2 G[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_reg,23,12, 0x0);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_patgen_c2_g_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_patgen_c2_g(0xfff));
        //PATGEN_C2_B	0x180094F0 11:0	R/W	0x0F	Pattern generator color 2 B[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_B_reg,11,0, 0xfff);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_B_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_B_patgen_c2_b_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_B_patgen_c2_b(0xfff));
        //PATGEN_C2_R 0x180094EC 11:0	R/W	0x0F	Pattern generator color 2 R[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_reg,11,0, 0x0);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_patgen_c2_r_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_patgen_c2_r(0x0));

        // color 3 G=3FF B=3FF R=3FF
        //PATGEN_C3_G 0x180094F4 23:12	R/W	0x0F	Pattern generator color 3 G[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_reg,23,12, 0xfff);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_patgen_c3_g_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_patgen_c3_g(0xfff));
        //PATGEN_C3_B	0x180BB2A0 11:0	R/W	0x0F	Pattern generator color 3 B[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_B_reg,11,0, 0x0);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_B_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_B_patgen_c3_b_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_B_patgen_c3_b(0x0));
        //PATGEN_C3_R 0x180094F4 11:0	R/W	0x0F	Pattern generator color 3 R[11:0]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_reg,11,0, 0x0);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_patgen_c3_r_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_patgen_c3_r(0x0));

        // color 4 G=000 B=000 R=000
        //PATGEN_C2_G 0x180194FC 23:12	R/W	0x0F	Pattern generator color 2 G[11:0]
         rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_RG_patgen_c4_g_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_RG_patgen_c4_g(0x0));
        //PATGEN_C2_B	0x1801950011:0	R/W	0x0F	Pattern generator color 2 B[11:0]
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_B_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_B_patgen_c4_b_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_B_patgen_c4_b(0xfff));
        //PATGEN_C2_R 0x180194FC 11:0	R/W	0x0F	Pattern generator color 2 R[11:0]
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_RG_patgen_c4_r_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_RG_patgen_c4_r(0xfff));

        // color 5 G=000 B=3FF R=3FF
        //PATGEN_C5_G 0x18019504 23:12	R/W	0x0F	Pattern generator color 5 G[11:0]
         rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_RG_patgen_c5_g_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_RG_patgen_c5_g(0x0));
        //PATGEN_C5_B	0x1801950811:0	R/W	0x0F	Pattern generator color 5 B[11:0]
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_B_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_B_patgen_c5_b_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_B_patgen_c5_b(0x0));
        //PATGEN_C5_R 0x18019504 11:0	R/W	0x0F	Pattern generator color 5 R[11:0]
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_RG_patgen_c5_r_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_RG_patgen_c5_r(0xfff));

        // color 6 G=3FF B=3FF R=000
        //PATGEN_C6_G 0x1801950C 23:12	R/W	0x0F	Pattern generator color 6 G[11:0]
         rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_RG_patgen_c6_g_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_RG_patgen_c6_g(0x0));
        //PATGEN_C6_B	0x1801951011:0	R/W	0x0F	Pattern generator color 6 B[11:0]
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_B_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_B_patgen_c6_b_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_B_patgen_c6_b(0xfff));
        //PATGEN_C6_R 0x1801950C 11:0	R/W	0x0F	Pattern generator color 6 R[11:0]
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_RG_patgen_c6_r_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_RG_patgen_c6_r(0x0));

        // color 7 G=3FF B=000 R=3FF
        //PATGEN_C7_G 0x18019514 23:12	R/W	0x0F	Pattern generator color 7 G[11:0]
         rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_RG_patgen_c7_g_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_RG_patgen_c7_g(0x0));
        //PATGEN_C7_B	0x1801951811:0	R/W	0x0F	Pattern generator color 7 B[11:0]
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_B_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_B_patgen_c7_b_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_B_patgen_c7_b(0x0));
        //PATGEN_C7_R 0x18019514 11:0	R/W	0x0F	Pattern generator color 7 R[11:0]
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_RG_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_RG_patgen_c7_r_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_RG_patgen_c7_r(0x0));

        if(u8hdmitx_Hdcp_state){
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_B_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_B_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_B_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_B_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_B_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_RG_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_B_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_RG_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_B_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_RG_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_B_reg, 0x00000000);
            rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_RG_reg, 0x00000000);
        }
        value_u16 = ((bVby1RxLock|| bVgipEn) && bTimingSync? 0: 0x1); // 0: Pattern generator disable for VBY1 37.125M, 1: Pattern generator enable for 27M
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg,31,31,value_u16);//PATGEN_EN[31]Pattern generator: 0: Pattern generator disable for VBY1 37.125M, 1: Pattern generator enable for 27M
        rtd_maskl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_en_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_en(value_u16));//PATGEN_EN[31]Pattern generator: 0: Pattern generator disable for VBY1 37.125M, 1: Pattern generator enable for 27M
    }// #else // TX PTG
    //==================================== Pattern Generator done ====================================

    return;
}


void HDMITX_drv_misc_set_timingGen(void)
{
    UINT8 timingDiv4;
    UINT8 bVby1RxLock, bTimingSync, bVgipEn;
    TX_TIMING_INDEX timeIdx;
    UINT16 value_u16;
    bVby1RxLock = HDMITX_check_vby1rx_lock_status();
    bTimingSync = HDMITX_check_dtg_tx_timing_sync_status();
    bVgipEn = HDMITX_VGIP_HDMITX_VGIP_CTRL_get_hdmitx_ivrun(rtd_inl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg));
#if 1//def CONFIG_MACH_RTD2801_ZEBU
    txInBypassMode = bVgipEn;
#else  // IC platform support DTG timing gen only (curent)
    txInBypassMode = 1;
#endif
    timeIdx = hdmitx_ctrl_get_timing_type();
    if(timeIdx >= TX_TIMING_NUM){
        DebugMessageHDMITx("[HDMITX] INVALID idx[%d]@set_timingGen\n", (UINT32)timeIdx);
        return;
    }

    InfoMessageHDMITx("[HDMITX] TG Index[%d]:\n", (UINT32)timeIdx);
    DebugMessageHDMITx("RXLock/DtgSync=[%d/%d]\n", (UINT32)bVby1RxLock, (UINT32)bTimingSync);
    InfoMessageHDMITx("Bypass/VgipEn=[%d/%d]\n", (UINT32)SCRIPT_TX_ENABLE_PACKET_BYPASS(), (UINT32)bVgipEn);

    //p_tx_cea_timing_table = &tx_cea_timing_table[0] + timeIdx;
    //ScalerHdmiTxPhy_GetTxTimingTable(timeIdx, p_tx_cea_timing_table);

    //p_tx_output_timing_table = &tx_output_timing_table[0]+timeIdx;
    timingDiv4 = (tx_output_timing_table.frl_type == _HDMI21_FRL_NONE? 1: 4); // div 4 for HDMI21
    if((ScalerHdmiTxGetDSCEn() == TRUE))
        timingDiv4 = 2;
    // reset DTG in TX PTG mode
    if(SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE){
        rtd_outl(PPOVERLAY_Display_Timing_CTRL1_reg, 0);
		//fix by zhaodong
        //rtd_outl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg, 0);
    }

    //==================================== Timing Generator ====================================
    // DTG timing gen
    if(SCRIPT_TX_ENABLE_PACKET_BYPASS()){ // enable DTG timimng gen when enable RX bypass to TX mode
    #if 0 //fix by zhaodong
        // rtd_outl(0xb8021858, 0x800014a0);
        // iv2dv_ihs = h-total (4N)
        // h-total (refer to bgic dtg driver)
        //value_u16 = pScalerHdmiTimingInfo->DTG_H_PORCH + tx_cea_timing_table.h.active;
        //value_u16 = pScalerHdmiTimingInfo->DTG_IV2DV_HSYNC_WIDTH;
        //rtd_maskl(PPOVERLAY_OUTTG_OUTTG_dctl_iv2dv_ihs_ctrl_reg, ~PPOVERLAY_OUTTG_OUTTG_dctl_iv2dv_ihs_ctrl_iv2dv_ih_slf_width_mask, PPOVERLAY_OUTTG_OUTTG_dctl_iv2dv_ihs_ctrl_iv2dv_ih_slf_width(value_u16));

        // rtd_outl(0xb8021874, 0x00008000);
        if(Scaler_HDMITX_DispGetTxInputPathFromScalerPQ())
            rtd_maskl(PPOVERLAY_Double_Buffer_CTRL_1_reg, ~PPOVERLAY_Double_Buffer_CTRL_1_dtg_db_read_sel_mask, PPOVERLAY_Double_Buffer_CTRL_1_dtg_db_read_sel(1));
        else
            rtd_maskl(PPOVERLAY_OUTTG_OUTTG_Double_Buffer_CTRL_1_reg, ~PPOVERLAY_OUTTG_OUTTG_Double_Buffer_CTRL_1_outtg_db_read_sel_mask, PPOVERLAY_OUTTG_OUTTG_Double_Buffer_CTRL_1_outtg_db_read_sel(1));

        // rtd_outl(0xb8021804, 0x0000001f);
        // Display h sync width (4N-1)
        value_u16 = pScalerHdmiTimingInfo->DTG_DH_WIDTH -1;
        //rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DH_Width_reg, ~PPOVERLAY_OUTTG_OUTTG_DH_Width_dh_width_padout_mask, PPOVERLAY_OUTTG_OUTTG_DH_Width_dh_width_padout(0xf));
        if(Scaler_HDMITX_DispGetTxInputPathFromScalerPQ())
            rtd_maskl(PPOVERLAY_DH_Width_reg, ~PPOVERLAY_DH_Width_dh_width_mask, PPOVERLAY_DH_Width_dh_width(value_u16));
        else
            rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DH_Width_reg, ~PPOVERLAY_OUTTG_OUTTG_DH_Width_dh_width_mask, PPOVERLAY_OUTTG_OUTTG_DH_Width_dh_width(value_u16));

        // rtd_outl(0xb8021808, 0x149f149f);
        value_u16 = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_TOTAL) -1; // h-total (4N-1  , Min. value 90)
        if(Scaler_HDMITX_DispGetTxInputPathFromScalerPQ())
            rtd_maskl(PPOVERLAY_DH_Total_Length_reg, ~PPOVERLAY_DH_Total_Length_dh_total_mask, PPOVERLAY_DH_Total_Length_dh_total(value_u16));
        else
            rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DH_Total_Length_reg, ~PPOVERLAY_OUTTG_OUTTG_DH_Total_Length_dh_total_mask, PPOVERLAY_OUTTG_OUTTG_DH_Total_Length_dh_total(value_u16));
        //rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DH_Total_Last_Line_Length_reg, ~PPOVERLAY_OUTTG_OUTTG_DH_Total_Last_Line_Length_dh_total_last_line_mask, PPOVERLAY_OUTTG_OUTTG_DH_Total_Last_Line_Length_dh_total_last_line(value_u16));

        // rtd_outl(0xb802180c, 0x00000002);
        value_u16 = pScalerHdmiTimingInfo->DTG_DV_LENGTH - 1; // vsync width (1N-1)//  tx_cea_timing_table.v.sync
        //rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DV_Length_reg, ~PPOVERLAY_OUTTG_OUTTG_DV_Length_dv_length_padout_mask, PPOVERLAY_OUTTG_OUTTG_DV_Length_dv_length_padout(0x2f));
        if(Scaler_HDMITX_DispGetTxInputPathFromScalerPQ())
            rtd_maskl(PPOVERLAY_DV_Length_reg, ~PPOVERLAY_DV_Length_dv_length_mask, PPOVERLAY_DV_Length_dv_length(value_u16));
        else
            rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DV_Length_reg, ~PPOVERLAY_OUTTG_OUTTG_DV_Length_dv_length_mask, PPOVERLAY_OUTTG_OUTTG_DV_Length_dv_length(value_u16));

        // rtd_outl(0xb8021810, 0x00000011);
        value_u16 = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_TOTAL) -1; // v-total (1N-1)
        if(Scaler_HDMITX_DispGetTxInputPathFromScalerPQ())
            rtd_maskl(PPOVERLAY_DV_total_reg, ~PPOVERLAY_DV_total_dv_total_mask, PPOVERLAY_DV_total_dv_total(value_u16));
        else
            rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DV_total_reg, ~PPOVERLAY_OUTTG_OUTTG_DV_total_dv_total_mask, PPOVERLAY_OUTTG_OUTTG_DV_total_dv_total(value_u16));

        // rtd_outl(0xb8021814, 0x01801080);
        value_u16 = pScalerHdmiTimingInfo->DTG_H_DEN_STA;// - 24; // h-start(4N): Setting value = (real value - 24)
        if(Scaler_HDMITX_DispGetTxInputPathFromScalerPQ())
            rtd_maskl(PPOVERLAY_DH_DEN_Start_End_reg, ~PPOVERLAY_DH_DEN_Start_End_dh_den_sta_mask, PPOVERLAY_DH_DEN_Start_End_dh_den_sta(value_u16));
        else
            rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_dh_den_sta_mask, PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_dh_den_sta(value_u16));

        value_u16 = pScalerHdmiTimingInfo->DTG_H_DEN_STA + tx_cea_timing_table.h.active;// - 24; // h-end(4N): Setting value = (real value - 24)
        if(Scaler_HDMITX_DispGetTxInputPathFromScalerPQ())
            rtd_maskl(PPOVERLAY_DH_DEN_Start_End_reg, ~PPOVERLAY_DH_DEN_Start_End_dh_den_end_mask, PPOVERLAY_DH_DEN_Start_End_dh_den_end(value_u16));
        else
            rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_dh_den_end_mask, PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_dh_den_end(value_u16));

        // rtd_outl(0xb8021818, 0x0008000e);
        value_u16 = pScalerHdmiTimingInfo->DTG_V_DEN_STA; // v-start (1N)
        if(Scaler_HDMITX_DispGetTxInputPathFromScalerPQ())
            rtd_maskl(PPOVERLAY_DV_DEN_Start_End_reg, ~PPOVERLAY_DV_DEN_Start_End_dv_den_sta_mask, PPOVERLAY_DV_DEN_Start_End_dv_den_sta(value_u16));
        else
            rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_dv_den_sta_mask, PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_dv_den_sta(value_u16));

        value_u16 = pScalerHdmiTimingInfo->DTG_V_DEN_STA + tx_cea_timing_table.v.active; // v-end (1N)
        if(Scaler_HDMITX_DispGetTxInputPathFromScalerPQ())
            rtd_maskl(PPOVERLAY_DV_DEN_Start_End_reg, ~PPOVERLAY_DV_DEN_Start_End_dv_den_end_mask, PPOVERLAY_DV_DEN_Start_End_dv_den_end(value_u16));
        else
            rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_dv_den_end_mask, PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_dv_den_end(value_u16));


        //rtd_outl(0xb8027840, 0x01801080); // PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_reg
        value_u16 = pScalerHdmiTimingInfo->DTG_H_DEN_STA; // Display h data enable start, set 2N
        rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_dh_act_sta_mask, PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_dh_act_sta(value_u16));
        value_u16 = pScalerHdmiTimingInfo->DTG_H_DEN_STA + tx_cea_timing_table.h.active; // Display h data enable end , set 2N
        rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_dh_act_end_mask, PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_dh_act_end(value_u16));

        //rtd_outl(0xb8027844, 0x001a0022); // PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_reg
        value_u16 = pScalerHdmiTimingInfo->DTG_V_DEN_STA; // Display v data enable start, set 1N
        rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_dv_act_sta_mask, PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_dv_act_sta(value_u16));
        value_u16 = pScalerHdmiTimingInfo->DTG_V_DEN_STA + tx_cea_timing_table.v.active; // Display v data enable end, set 1N
        rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_dv_act_end_mask, PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_dv_act_end(value_u16));

        //rtd_outl(0xb8027800, 0x90000000); // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg
        //value_u32 = PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_frc_fsync_mask;
        //rtd_outl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg, value_u32);

        // rtd_outl(0xb8021800, 0x00410000);
        //value_u16 = 1; // dpll1_clk div: 1: div=2
        //rtd_maskl(BTG_B_Display_Timing_CTRL1_reg, ~BTG_B_Display_Timing_CTRL1_dpll1_div_mask, BTG_B_Display_Timing_CTRL1_dpll1_div(value_u16));

        //value_u16 = 1; // 1: timing from self
        //rtd_maskl(BTG_B_Display_Timing_CTRL1_reg, ~BTG_B_Display_Timing_CTRL1_master_timing_mask, BTG_B_Display_Timing_CTRL1_master_timing(value_u16));

        // timing gen enable when vby1 source unlock or timing not sync mode
        //value_u16 = (bVby1RxLock && bTimingSync? 0: 1);
        //rtd_maskl(PPOVERLAY_OUTTG_OUTTG_dctl_iv2dv_ihs_ctrl_reg, ~PPOVERLAY_OUTTG_OUTTG_dctl_iv2dv_ihs_ctrl_iv2dv_ih_slf_gen_en_mask, PPOVERLAY_OUTTG_OUTTG_dctl_iv2dv_ihs_ctrl_iv2dv_ih_slf_gen_en(value_u16));

        //rtd_outl(0xb8027800, 0x10000000); // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg
        // TX input timing gen (free run)
        value_u16 = ((bVby1RxLock|| bVgipEn) && bTimingSync? 1: 0);
        if(Scaler_HDMITX_DispGetTxInputPathFromScalerPQ()){
            rtd_maskl(PPOVERLAY_Display_Timing_CTRL1_reg,
                                                        ~(PPOVERLAY_Display_Timing_CTRL1_disp_fsync_en_mask| PPOVERLAY_Display_Timing_CTRL1_disp_en_mask),
                                                        (PPOVERLAY_Display_Timing_CTRL1_disp_fsync_en(value_u16)|PPOVERLAY_Display_Timing_CTRL1_disp_en(1)));
        }else{
            value_u8 = (SCRIPT_TX_VRR_SUPPORT()? 1: 0);
            rtd_outl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg, PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_fix_last_line(value_u8)); // free run for ptg
            value_u8 = (SCRIPT_TX_ENABLE_PACKET_BYPASS() && SCRIPT_TX_DTG_FSYNC_EN()? 1: 0); // timing sync mode
            rtd_maskl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg,
                                  ~(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_fsync_en_mask|PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_en_mask),
                                  (PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_fsync_en(value_u8)|PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_en(1)));
        }
		#endif
     }else if((!bVgipEn) && (ScalerHdmiTxGetDSCEn() == TRUE)) {// HDMITX timing gen
        //VIC 97 3840x2160 (4k60)
        // Timing Gen HDMI 2.1  => X/4
        // Timing Gen HDMI 2.0  => X/1
        InfoMessageHDMITx("[HDMITX] timingDiv4[%d]:\n", (UINT32)timingDiv4);
        //HDMITX_IN_TIMING_GEN_STA	HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_reg 28:16 self_tg_x_sta Self Timing Gen x start [12:0] =>Hsync+Hback HDMITX_IN_TIMING_GEN_STA	0x180094C4
        value_u16 = (tx_cea_timing_table.h.sync + tx_cea_timing_table.h.back) / timingDiv4;
        //rtd_part_outl(DSCE_MISC_DSC_TIMING_GEN_STA_reg,28,16,value_u16); //(88+296=0x180 )/4 = 0x60
        rtd_maskl(DSCE_MISC_DSC_TIMING_GEN_STA_reg, ~DSCE_MISC_DSC_TIMING_GEN_STA_self_tg_x_sta_mask, DSCE_MISC_DSC_TIMING_GEN_STA_self_tg_x_sta(value_u16)); //(88+296=0x180 )/4 = 0x60

        //HDMITX_IN__TIMING_GEN_STA	0x180BB264 12:0 self_tg_y_sta Self Timing Gen y start [12:0] = Vsync+Vback
        value_u16 = tx_cea_timing_table.v.sync + tx_cea_timing_table.v.back;
        //rtd_part_outl(DSCE_MISC_DSC_TIMING_GEN_STA_reg,12,0,value_u16);//10+72=0x52
        rtd_maskl(DSCE_MISC_DSC_TIMING_GEN_STA_reg, ~DSCE_MISC_DSC_TIMING_GEN_STA_self_tg_y_sta_mask, DSCE_MISC_DSC_TIMING_GEN_STA_self_tg_y_sta(value_u16));//10+72=0x52

        //HDMITX_IN_TIMING_GEN_SIZE	HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_reg 28:16 self_tg_x_size Self Timing Gen x size [12:0] =Hactive (Max = 7680)=>HDMITX_IN_TIMING_GEN_SIZE	0x180094C8
        value_u16 = tx_cea_timing_table.h.active / timingDiv4;
        //rtd_part_outl(DSCE_MISC_DSC_TIMING_GEN_SIZE_reg,28,16,value_u16);//(3840=0xF00)/4=0x3C0
        rtd_maskl(DSCE_MISC_DSC_TIMING_GEN_SIZE_reg, ~DSCE_MISC_DSC_TIMING_GEN_SIZE_self_tg_x_size_mask, DSCE_MISC_DSC_TIMING_GEN_SIZE_self_tg_x_size(value_u16));//(3840=0xF00)/4=0x3C0

        //HDMITX_IN_TIMING_GEN_SIZE	HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_reg 12:0 self_tg_y_size  Self Timing Gen y size [12:0] =Vactive (Max = 4320)
        value_u16 = tx_cea_timing_table.v.active;
        //rtd_part_outl(DSCE_MISC_DSC_TIMING_GEN_SIZE_reg,12,0,value_u16);//2160=0x870
        rtd_maskl(DSCE_MISC_DSC_TIMING_GEN_SIZE_reg, ~DSCE_MISC_DSC_TIMING_GEN_SIZE_self_tg_y_size_mask, DSCE_MISC_DSC_TIMING_GEN_SIZE_self_tg_y_size(value_u16));//2160=0x870

        //HDMITX_IN_TIMING_GEN_END	0x180BB26C 28:16 self_tg_x_end Self Timing Gen x end [12:0] = Hfront => HDMITX_IN_TIMING_GEN_END	0x180094CC
        value_u16 = tx_cea_timing_table.h.front / timingDiv4;
        //rtd_part_outl(DSCE_MISC_DSC_TIMING_GEN_END_reg,28,16,value_u16);//(176=0xB0 )/4=0x2C
        rtd_maskl(DSCE_MISC_DSC_TIMING_GEN_END_reg, ~DSCE_MISC_DSC_TIMING_GEN_END_self_tg_x_end_mask, DSCE_MISC_DSC_TIMING_GEN_END_self_tg_x_end(value_u16));//(176=0xB0 )/4=0x2C

        //HDMITX_IN__TIMING_GEN_END	HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_reg 12:0 self_tg_7_end Self Timing Gen y end [12:0] = Vfront
        value_u16 = tx_cea_timing_table.v.front;
        //rtd_part_outl(DSCE_MISC_DSC_TIMING_GEN_END_reg,12,0,value_u16); //8=0x8
        rtd_maskl(DSCE_MISC_DSC_TIMING_GEN_END_reg, ~DSCE_MISC_DSC_TIMING_GEN_END_self_tg_y_end_mask, DSCE_MISC_DSC_TIMING_GEN_END_self_tg_y_end(value_u16)); //8=0x8

        value_u16 = tx_cea_timing_table.h.sync / timingDiv4 / 2; // [WHY / 2?]
        //rtd_part_outl(DSCE_MISC_DSC_TIMING_GEN_HSYNC_reg,12,0,0x0); // hs_start
        rtd_maskl(DSCE_MISC_DSC_TIMING_GEN_HSYNC_reg, ~DSCE_MISC_DSC_TIMING_GEN_HSYNC_self_tg_hs_sta_mask, DSCE_MISC_DSC_TIMING_GEN_HSYNC_self_tg_hs_sta(0x0)); // hs_start
        //rtd_part_outl(DSCE_MISC_DSC_TIMING_GEN_HSYNC_reg,28,15,value_u16); //hs_end, hsync width/4= 88/4=22 / 2
        rtd_maskl(DSCE_MISC_DSC_TIMING_GEN_HSYNC_reg, ~DSCE_MISC_DSC_TIMING_GEN_HSYNC_self_tg_hs_end_mask, DSCE_MISC_DSC_TIMING_GEN_HSYNC_self_tg_hs_end(value_u16)); //hs_end, hsync width/4= 88/4=22

        //HDMITX_IN__TIMING_GEN_CTRL	HDMITX_MISC_HDMITX_IN_TIMING_GEN_CTRL_reg [0] self_tg_en => HDMITX_IN_TIMING_GEN_CTRL	0x180094C0
        // 0: HDMITX_IN_TIMING_GEN_CTRL Disable for VBY1 37.125M, 1: HDMITX_IN_TIMING_GEN_CTRL Enable for 27M
        value_u16 = ((bVby1RxLock|| bVgipEn) && bTimingSync? 0: 1);
        //rtd_part_outl(DSCE_MISC_DSC_TIMING_GEN_CTRL_reg,0,0,value_u16);
        rtd_maskl(DSCE_MISC_DSC_TIMING_GEN_CTRL_reg, ~DSCE_MISC_DSC_TIMING_GEN_CTRL_self_tg_en_mask, DSCE_MISC_DSC_TIMING_GEN_CTRL_self_tg_en(value_u16));
    } else{// HDMITX timing gen
        //VIC 97 3840x2160 (4k60)
        // Timing Gen HDMI 2.1  => X/4
        // Timing Gen HDMI 2.0  => X/1

        //HDMITX_IN_TIMING_GEN_STA	HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_reg 28:16 self_tg_x_sta Self Timing Gen x start [12:0] =>Hsync+Hback HDMITX_IN_TIMING_GEN_STA	0x180094C4
        value_u16 = (tx_cea_timing_table.h.sync + tx_cea_timing_table.h.back) / timingDiv4;
        //rtd_part_outl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_reg,28,16,value_u16); //(88+296=0x180 )/4 = 0x60
        rtd_maskl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_self_tg_x_sta_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_self_tg_x_sta(value_u16)); //(88+296=0x180 )/4 = 0x60

        //HDMITX_IN__TIMING_GEN_STA	0x180BB264 12:0 self_tg_y_sta Self Timing Gen y start [12:0] = Vsync+Vback
        value_u16 = tx_cea_timing_table.v.sync + tx_cea_timing_table.v.back;
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_reg,12,0,value_u16);//10+72=0x52
        rtd_maskl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_self_tg_y_sta_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_self_tg_y_sta(value_u16));//10+72=0x52

        //HDMITX_IN_TIMING_GEN_SIZE	HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_reg 28:16 self_tg_x_size Self Timing Gen x size [12:0] =Hactive (Max = 7680)=>HDMITX_IN_TIMING_GEN_SIZE	0x180094C8
        value_u16 = tx_cea_timing_table.h.active / timingDiv4;
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_reg,28,16,value_u16);//(3840=0xF00)/4=0x3C0
        rtd_maskl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_self_tg_x_size_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_self_tg_x_size(value_u16));//(3840=0xF00)/4=0x3C0

        //HDMITX_IN_TIMING_GEN_SIZE	HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_reg 12:0 self_tg_y_size  Self Timing Gen y size [12:0] =Vactive (Max = 4320)
        value_u16 = tx_cea_timing_table.v.active;
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_reg,12,0,value_u16);//2160=0x870
        rtd_maskl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_self_tg_y_size_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_self_tg_y_size(value_u16));//2160=0x870

        //HDMITX_IN_TIMING_GEN_END	0x180BB26C 28:16 self_tg_x_end Self Timing Gen x end [12:0] = Hfront => HDMITX_IN_TIMING_GEN_END	0x180094CC
        value_u16 = tx_cea_timing_table.h.front / timingDiv4;
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_reg,28,16,value_u16);//(176=0xB0 )/4=0x2C
        rtd_maskl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_self_tg_x_end_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_self_tg_x_end(value_u16));//(176=0xB0 )/4=0x2C

        //HDMITX_IN__TIMING_GEN_END	HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_reg 12:0 self_tg_7_end Self Timing Gen y end [12:0] = Vfront
        value_u16 = tx_cea_timing_table.v.front;
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_reg,12,0,value_u16); //8=0x8
        rtd_maskl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_self_tg_y_end_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_self_tg_y_end(value_u16)); //8=0x8

        value_u16 = tx_cea_timing_table.h.sync / timingDiv4 / 2; // [WHY / 2?]
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_reg,12,0,0x0); // hs_start
        rtd_maskl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_self_tg_hs_sta_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_self_tg_hs_sta(0x0)); // hs_start
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_reg,28,15,value_u16); //hs_end, hsync width/4= 88/4=22 / 2
        rtd_maskl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_self_tg_hs_end_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_self_tg_hs_end(value_u16)); //hs_end, hsync width/4= 88/4=22

        //HDMITX_IN__TIMING_GEN_CTRL	HDMITX_MISC_HDMITX_IN_TIMING_GEN_CTRL_reg [0] self_tg_en => HDMITX_IN_TIMING_GEN_CTRL	0x180094C0
        // 0: HDMITX_IN_TIMING_GEN_CTRL Disable for VBY1 37.125M, 1: HDMITX_IN_TIMING_GEN_CTRL Enable for 27M
        value_u16 = ((bVby1RxLock|| bVgipEn) && bTimingSync? 0: 1);
        //rtd_part_outl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_CTRL_reg,0,0,value_u16);
        rtd_maskl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_CTRL_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_CTRL_self_tg_en_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_CTRL_self_tg_en(value_u16));
    }// #else // timing gen from HDMITX
    //==================================== Timing Generator done ====================================

    return;
}


BOOLEAN HDMITX_drv_get_Pllready(void)
{
   UINT8 value_u8;
   value_u8 =   HDMITX_PHY_HDMITXPHY_CTRL06_get_reg_plldisp_pow(rtd_inl(HDMITX_PHY_HDMITXPHY_CTRL06_reg));
   if(value_u8)
    return TRUE;

   return FALSE;
}



void hdmi_set_audio_sample_freq(void)
{
#if(_AUDIO_SUPPORT == _ON)

    UINT32 hdmirx_sapmle_freq = 48000;
    if(g_scaler_display_info.input_src == 0){
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
    hdmirx_sapmle_freq = drvif_Hdmi_get_audio_sample_freq();
#endif
     }else{
#ifdef CONFIG_ENABLE_DPRX
        hdmirx_sapmle_freq = DRVIF_DPRX_GET_AUDIO_SAMPLE_FREQUENCY();
#endif
     }

    switch(hdmirx_sapmle_freq)
    {
        case 32000:
            AudioFreqType = _AUDIO_FREQ_32K;
            break;
          case 64000:
            AudioFreqType = _AUDIO_FREQ_64K;
            break;
        case 128000:
            AudioFreqType = _AUDIO_FREQ_128K;
            break;
        case 44100:
            AudioFreqType = _AUDIO_FREQ_44_1K;
            break;
        case 88200:
            AudioFreqType = _AUDIO_FREQ_88_2K;
            break;
        case 176400:
            AudioFreqType = _AUDIO_FREQ_176_4K;
            break;
        case 48000:
            AudioFreqType = _AUDIO_FREQ_48K;
            break;
        case 96000:
            AudioFreqType = _AUDIO_FREQ_96K;
            break;
        case 192000:
            AudioFreqType = _AUDIO_FREQ_192K;
            break;
        case 256000:
            AudioFreqType = _AUDIO_FREQ_256K;
            break;
        case 352800:
            AudioFreqType = _AUDIO_FREQ_352_8K;
            break;
        case 384000:
            AudioFreqType = _AUDIO_FREQ_384K;
            break;
        case 512000:
            AudioFreqType = _AUDIO_FREQ_512K;
            break;
        case 705600:
            AudioFreqType = _AUDIO_FREQ_705_6K;
            break;
        case 768000:
            AudioFreqType = _AUDIO_FREQ_768K;
            break;
        default:
            AudioFreqType = _AUDIO_FREQ_NO_AUDIO;
            break;
    }
#endif

    return;
}

void hdmi_get_tx_timing_info(void)
{
    tx_cea_timing_table.h.active = timing_info.h_act_len;
    tx_cea_timing_table.h.sync = timing_info.IHSyncPulseCount;
    tx_cea_timing_table.h.blank = timing_info.h_total - timing_info.h_act_len;
    tx_cea_timing_table.h.back = timing_info.h_act_sta - timing_info.IHSyncPulseCount;
    tx_cea_timing_table.h.front = tx_cea_timing_table.h.blank - timing_info.h_act_sta;

    tx_cea_timing_table.v.active = timing_info.v_act_len;
    tx_cea_timing_table.v.sync = timing_info.IVSyncPulseCount;
    tx_cea_timing_table.v.blank = timing_info.v_total - timing_info.v_act_len;
    tx_cea_timing_table.v.back = timing_info.v_act_sta - timing_info.IVSyncPulseCount;
    tx_cea_timing_table.v.front = tx_cea_timing_table.v.blank - timing_info.v_act_sta;
    tx_cea_timing_table.vFreq = timing_info.v_freq;
    tx_cea_timing_table.pixelFreq = timing_info.pll_pixelclockx1024 / 100000;
    if(timing_info.run_vrr  && (g_scaler_display_info.input_src == 1)) {
        if(tx_cea_timing_table.v.sync == 1)
            tx_cea_timing_table.v.sync = 2;
        tx_cea_timing_table.v.back = g_scaler_display_info.TX_OUTPUT_V_ACT_STA - tx_cea_timing_table.v.sync;
        tx_cea_timing_table.v.front = tx_cea_timing_table.v.blank - g_scaler_display_info.TX_OUTPUT_V_ACT_STA;
    }
    InfoMessageHDMITx("get_tx_timing pixel clock=%d\n", (UINT32)tx_cea_timing_table.pixelFreq);
}

#ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
extern hdmi_timing_check_type  dtd_timing;
void hdmi_get_dtd_timing_info(void)
{
    tx_cea_timing_table.h.active = dtd_timing.h_act;
    tx_cea_timing_table.h.sync = dtd_timing.h_sync;
    tx_cea_timing_table.h.blank = dtd_timing.h_blank;
    tx_cea_timing_table.h.back = dtd_timing.h_blank - dtd_timing.h_sync - dtd_timing.h_front;
    tx_cea_timing_table.h.front = dtd_timing.h_front;

    tx_cea_timing_table.v.active = dtd_timing.v_act;
    tx_cea_timing_table.v.sync = dtd_timing.v_sync;
    tx_cea_timing_table.v.blank = dtd_timing.v_blank;
    tx_cea_timing_table.v.back = dtd_timing.v_blank - dtd_timing.v_sync - dtd_timing.v_front;
    tx_cea_timing_table.v.front = dtd_timing.v_front;

    tx_cea_timing_table.pixelFreq = dtd_timing.pixelClk / 10;
}
#endif

void update_adjust_vototal(void)
{
    UINT16 value_u16;
    if((tx_cea_timing_table.v.blank + tx_cea_timing_table.v.active) >= adjust_timing_vtotal) {
        value_u16 = tx_cea_timing_table.v.blank + tx_cea_timing_table.v.active - adjust_timing_vtotal;
        tx_cea_timing_table.v.blank = tx_cea_timing_table.v.blank - value_u16;
        if(tx_cea_timing_table.v.back > value_u16)
            tx_cea_timing_table.v.back -= value_u16;
    } else {
        value_u16 = adjust_timing_vtotal - tx_cea_timing_table.v.blank -tx_cea_timing_table.v.active;
        tx_cea_timing_table.v.blank = tx_cea_timing_table.v.blank + value_u16;
        tx_cea_timing_table.v.back += value_u16;
    }
}

#ifndef UT_flag

extern UINT8 bHD21_480P_576P;
UINT8 decide_hdmitx_frl_type(UINT8 color_type,UINT16 htotal,UINT16 vtotal)
{
    UINT8 value_u8;
    UINT32 value_u32;
    // dummy register for force tx output frl mode
    if(HDMITX_MISC_HDMITX_RETIMING_V0_get_dummy_31_24(rtd_inl(HDMITX_MISC_HDMITX_RETIMING_V0_reg)))
        return HDMITX_MISC_HDMITX_RETIMING_V0_get_dummy_31_24(rtd_inl(HDMITX_MISC_HDMITX_RETIMING_V0_reg));
    // 4k240 dsc bypass workround
    if(timing_info.is_dsc_to_tx 
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
       && drvif_Hdmi_Multi_port_GetFrlMode(bypass_port) <= _HDMI21_FRL_8G
#endif
      )
        return _HDMI21_FRL_10G;

    // bypass mode tx frl mode=rx frl mode(only for hdmirx->hdmitx)
    if(get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE /*&& (g_scaler_display_info.input_src == 0)*/) { //fix by zhaodong
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
        if((drvif_Hdmi_Multi_port_GetFrlMode(bypass_port) != 0) && bHD21_480P_576P && tx_output_timing_table.color_depth > 0)
            return _HDMI21_FRL_3G;
        else
            return drvif_Hdmi_Multi_port_GetFrlMode(bypass_port);
#endif
    } else {
        // if rx is hdmi20, and tx pixel clk<=600M, tx run 2.0
        if(
        #if IS_ENABLED(CONFIG_RTK_HDMI_RX)
		(drvif_Hdmi_Multi_port_GetFrlMode(bypass_port) == 0) ||
		#endif
		(ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE)==0)
		|| (g_scaler_display_info.input_src != DRIVER_HDMITX_INPUT_SRC_HDMIRX)
		) {
            value_u32 = (UINT32)hdmitxGetTxOutputPixelClock() / 10;
            if(color_type == I3DDMA_COLOR_YUV422) { //422 10/12bit pll equel 444 8bit
                ;//value_u32 = value_u32;
            } else {
                switch(tx_output_timing_table.color_depth) {
                    case 0:
                         ;//value_u32 = value_u32;
                         break;
                     case 1:
                         value_u32 = value_u32 * 5 /4;
                         break;
                     case 2:
                         value_u32 = value_u32 * 3 / 2;
                         break;
                     default:
                        ;//value_u32 = value_u32;
                        break;
                }
            }
            if(color_type == I3DDMA_COLOR_YUV411)
                value_u32 = value_u32 / 2;
            if(value_u32 <= 60000000)
                return _HDMI21_FRL_NONE;
        }

        value_u32 = (UINT32)htotal * (UINT32)vtotal/ 100;
        //value_u32 = value_u32 * (UINT32)timing_info.v_freq / 1000;
        value_u32 = value_u32 * (UINT32)tx_output_timing_table.frame_rate / 1000;
         if(color_type == I3DDMA_COLOR_YUV422) { //422 10/12bit pll equel 444 8bit
                value_u32 = value_u32 * 24;
         } else {
            switch(tx_output_timing_table.color_depth) {
                case 0:
                     value_u32 = value_u32 * 24;
                     break;
                 case 1:
                     value_u32 = value_u32 * 30;
                     break;
                 case 2:
                     value_u32 = value_u32 * 36;
                     break;
                 default:
                    value_u32 = value_u32 * 24;
                    break;
            }
        }

        if(color_type == I3DDMA_COLOR_YUV411)
            value_u32 = value_u32 / 2;

        value_u32 = value_u32 / 8;
        value_u32 = value_u32 * 9; 
        DebugMessageHDMITx("[HDMITX]frl bandwith/lane=%d\n", (UINT32)value_u32);

        if(value_u32 > HDMITX_FRL_12G_DATA) {
            DebugMessageHDMITx("[HDMITX]frl over range\n");
            value_u8 = _HDMI21_FRL_12G;
        } else if(value_u32 > HDMITX_FRL_10G_DATA && value_u32 <= HDMITX_FRL_12G_DATA) {
            value_u8 = _HDMI21_FRL_12G;
        } else if(value_u32 > HDMITX_FRL_8G_DATA&& value_u32 <= HDMITX_FRL_10G_DATA) {
            value_u8 = _HDMI21_FRL_10G;
        } else if(value_u32 > HDMITX_FRL_6G4L_DATA && value_u32 <= HDMITX_FRL_8G_DATA) {
            value_u8 = _HDMI21_FRL_8G;
        } else if(value_u32 > HDMITX_FRL_6G3L_DATA && value_u32 <= HDMITX_FRL_6G4L_DATA) {
            value_u8 = _HDMI21_FRL_6G_4LANES;
        } else if(value_u32 > HDMITX_FRL_3G_DATA && value_u32 <= HDMITX_FRL_6G3L_DATA) {
            value_u8 = _HDMI21_FRL_6G_3LANES;
        } else {
            value_u8 = _HDMI21_FRL_3G;
        }
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
                if((drvif_Hdmi_Multi_port_GetFrlMode(bypass_port) > value_u8) && (g_scaler_display_info.input_src == 0))
                    value_u8 = (UINT8)(drvif_Hdmi_Multi_port_GetFrlMode(bypass_port));
#endif
        if(value_u8 > ScalerHdmiMacTxEdidGetFeature(_P0_OUTPUT_PORT,_MAX_FRL_RATE))
            value_u8 = ScalerHdmiMacTxEdidGetFeature(_P0_OUTPUT_PORT,_MAX_FRL_RATE);
        return value_u8;
    }
	return 0;
}
#endif // UT_flag
UINT8 decide_hdmitx_force_disable_deepcolor(void)
{
    UINT16 value_u16;
    if(!lib_hdmitx_is_hdmi_21_support() && (g_scaler_display_info.input_src == 1)) {
        value_u16 = (UINT32)hdmitxGetTxOutputPixelClock() / 100000;
        if(tx_output_timing_table.color_type == I3DDMA_COLOR_YUV422) { //422 10/12bit pll equel 444 8bit
            //value_u16 = value_u16;
        } else {
            switch(tx_output_timing_table.color_depth) {
                 case HDMITX_10BIT:
                     value_u16 = value_u16 /4 * 5;
                     break;
                 case HDMITX_12BIT:
                     value_u16 = value_u16 / 2 * 3;
                     break;
                 default:
                    break;
            }
        }
        if(tx_output_timing_table.color_type == I3DDMA_COLOR_YUV411)
            value_u16 = value_u16 / 2;
        if(value_u16 > HDMITX_HDMI20_MAX_PLL)
            return 1;
        else
            return 0;
        }
    return 0;
}
void hdmi_get_tx_output_info(void)
{
    UINT16 htotal = 0;
    UINT16 vtotal = 0;
    UINT8 bVgipEn;
    if(get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE) {
        tx_output_timing_table.width = timing_info.h_act_len;
        tx_output_timing_table.height = timing_info.v_act_len;
        htotal = timing_info.h_total;
        vtotal = timing_info.v_total;
        tx_output_timing_table.frame_rate = timing_info.v_freq / 100;
        g_TxPixelClock1024 = timing_info.pll_pixelclockx1024;
        tx_output_timing_table.color_type =timing_info.colorspace;//tx_output_colorSpace;
        tx_output_timing_table.color_depth = timing_info.colordepth;//tx_output_colordepth;

    } else {
        tx_output_timing_table.width = tx_cea_timing_table.h.active;
        tx_output_timing_table.height = tx_cea_timing_table.v.active;
        htotal = tx_cea_timing_table.h.active + tx_cea_timing_table.h.blank;
        vtotal = tx_cea_timing_table.v.active + tx_cea_timing_table.v.blank;
        g_TxPixelClock1024 = tx_cea_timing_table.pixelFreq * 100000;
        tx_output_timing_table.frame_rate = tx_cea_timing_table.vFreq;//timing_info.v_freq / 10;
        tx_output_timing_table.color_type = GET_HDMITX_AP_COLOR_FORMAT();//tx_output_colorSpace;
        tx_output_timing_table.color_depth = GET_HDMITX_AP_COLOR_DEPTH();//tx_output_colordepth;

    }
    //tx_output_timing_table.frame_rate = tx_cea_timing_table.vFreq;//timing_info.v_freq / 10;
    tx_output_timing_table.progressive = !timing_info.is_interlace;
    //tx_output_timing_table.color_type = GET_HDMITX_AP_COLOR_FORMAT();//tx_output_colorSpace;
    //tx_output_timing_table.color_depth = GET_HDMITX_AP_COLOR_DEPTH();//tx_output_colordepth;
    if(decide_hdmitx_force_disable_deepcolor())
        tx_output_timing_table.color_depth = HDMITX_8BIT;
    tx_output_timing_table.frl_type = decide_hdmitx_frl_type(tx_output_timing_table.color_type, htotal, vtotal);
    //tx_output_timing_table.frl_type = GET_HDMITX_AP_FRL_MODE();
    //g_TxPixelClock1024 = tx_cea_timing_table.pixelFreq * 100000;
    bVgipEn = HDMITX_VGIP_HDMITX_VGIP_CTRL_get_hdmitx_ivrun(rtd_inl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg));
    if(bVgipEn == 0) {
        g_TxPixelClock1024 = tx_cea_timing_table.pixelFreq * 100000;
        tx_output_timing_table.width = tx_cea_timing_table.h.active;
        tx_output_timing_table.height = tx_cea_timing_table.v.active;
        tx_output_timing_table.frame_rate = tx_cea_timing_table.vFreq;
        tx_output_timing_table.progressive = 1;//tx_ptg_output_timing_table[ptg_timing_index].progressive;
        tx_output_timing_table.color_type = GET_HDMITX_AP_COLOR_FORMAT();
        tx_output_timing_table.color_depth = GET_HDMITX_AP_COLOR_DEPTH();
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
        if(ptg_timing_index == CTS_TIMING_INDEX)
            tx_output_timing_table.frl_type = (UINT8)(drvif_Hdmi_Multi_port_GetFrlMode(bypass_port));
        else
#endif
            //tx_output_timing_table.frl_type = decide_hdmitx_frl_type(tx_output_timing_table.color_type, htotal, vtotal);
            tx_output_timing_table.frl_type = GET_HDMITX_AP_FRL_MODE();
    }
	InfoMessageHDMITx("zd pll_pixelclockx1024=%lu, frame_rate=%d,clk=%d\n", timing_info.pll_pixelclockx1024,tx_output_timing_table.frame_rate,g_TxPixelClock1024);
}

void hdmi_set_need_sw_avi_packet(void)
{
    need_sw_avi_packet = 0;
    if((get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE) && (g_scaler_display_info.input_src == 0))
        need_sw_avi_packet = 0;
#if 0
    if((get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE) && (g_scaler_display_info.input_src == 0) && (tx_output_timing_table.frl_type == 0)){
        if((timing_info.h_total <= 4000) && (timing_info.v_total == 2222) && (timing_info.h_act_len == 3840)
                && (timing_info.v_act_len == 2160) && (59 < (timing_info.v_freq / 10) < 61)){
            need_sw_avi_packet = 1;
            DebugMessageHDMITx("[HDMITX] need sw avi\n");
        }
    }
#endif
    return;
}

UINT8 hdmi_check_4k144_RBv2(TX_TIMING_INDEX timeIdx)
{
    if((timeIdx >= TX_TIMING_4K2KP144_CVT_RB) && (timeIdx <= TX_TIMING_4K2KP144_CVT)) {
            return 1;
    }
    return 0;
}
void setting4K144Rbv2Flag(TX_TIMING_INDEX timeIdx)
{//for UT cover
    if(hdmi_check_4k144_RBv2(timeIdx))
        SET_HDMI_4K144_RBV2_FLAG();
    else
        CLR_HDMI_4K144_RBV2_FLAG();
}

void hdmitx_set_ptg(void)
{
    UINT8 bVgipEn;
    bVgipEn = HDMITX_VGIP_HDMITX_VGIP_CTRL_get_hdmitx_ivrun(rtd_inl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg));
    if(bVgipEn)
        ScalerHdmiMacTx0EnablePtg(0,0);
    else {
        ScalerHdmiMacTx0EnablePtg(0,1);
        set_hdmitxOutputMode(0);
    }
}

void hdmitx_interlace_force_bypass(void)
{
    UINT8 bVgipEn;
    bVgipEn = HDMITX_VGIP_HDMITX_VGIP_CTRL_get_hdmitx_ivrun(rtd_inl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg));
    if((get_hdmitxOutputMode() == TX_OUT_SOURCE_MODE) && timing_info.is_interlace && bVgipEn)
    {
        FatalMessageHDMITx("[HDMITX] interlace case change to bypass\n");
        set_hdmitxOutputMode(TX_OUT_THROUGH_MODE);
    }
}
void HDMITX_InitTxPreInfo(StructTxPreInfo *txpre_info)
{
    if(g_scaler_display_info.input_src == DRIVER_HDMITX_INPUT_SRC_HDMIRX){
        txpre_info->Input_Src = TXPRE_VGIP_SRC_HDMI;
        txpre_info->Polarity = timing_info.polarity;
    }
    else if(g_scaler_display_info.input_src == DRIVER_HDMITX_INPUT_SRC_DTG){
        txpre_info->Input_Src = TXPRE_VGIP_SRC_OSD123;
        txpre_info->Polarity = 3; // positive
    }
    else if(g_scaler_display_info.input_src == DRIVER_HDMITX_INPUT_SRC_DPRX){
        txpre_info->Input_Src = TXPRE_VGIP_SRC_HDMI;
        txpre_info->Polarity = 3; // positive
    }
    txpre_info->pixelclock = tx_cea_timing_table.pixelFreq;
    FatalMessageHDMITx("[HDMITX] Input_Src txpre %d\n",txpre_info->Input_Src);

    txpre_info->IPH_ACT_STA = tx_cea_timing_table.h.back + tx_cea_timing_table.h.sync;
    txpre_info->IPV_ACT_STA = tx_cea_timing_table.v.back + tx_cea_timing_table.v.sync;
    txpre_info->IPH_ACT_WID = tx_cea_timing_table.h.active;
    txpre_info->IPV_ACT_LEN = tx_cea_timing_table.v.active;
    //txpre_info->IHFreq = tx_cea_timing_table.v.back + tx_cea_timing_table.v.sync;
    txpre_info->IVFreq = tx_cea_timing_table.vFreq;
    txpre_info->IHTotal = tx_cea_timing_table.h.active + tx_cea_timing_table.h.blank;
    txpre_info->IVTotal = tx_cea_timing_table.v.active + tx_cea_timing_table.v.blank;
    txpre_info->IHsync = tx_cea_timing_table.h.sync;
    txpre_info->IVsync = tx_cea_timing_table.v.sync;
    txpre_info->Input_Color_Space = timing_info.colorspace;//GET_HDMITX_AP_COLOR_FORMAT();//HDMITX_COLOR_RGB;
    txpre_info->Input_Color_Depth = timing_info.colordepth;//GET_HDMITX_AP_COLOR_DEPTH();//HDMITX_8BIT;
    txpre_info->Cap_Wid = tx_cea_timing_table.h.active;
    txpre_info->Cap_Len = tx_cea_timing_table.v.active;

    txpre_info->TXPRE_OUTPUT_ACT_HSTA = tx_cea_timing_table.h.back + tx_cea_timing_table.h.sync;
    txpre_info->TXPRE_OUTPUT_ACT_VSTA = tx_cea_timing_table.v.back + tx_cea_timing_table.v.sync;
    txpre_info->TXPRE_OUTPUT_ACT_WID = tx_output_timing_table.width;
    txpre_info->TXPRE_OUTPUT_ACT_LEN = tx_output_timing_table.height;
    txpre_info->TXPRE_OUTPUT_HTOTAL = tx_cea_timing_table.h.active + tx_cea_timing_table.h.blank;
    txpre_info->TXPRE_OUTPUT_VTOTAL = tx_cea_timing_table.v.active + tx_cea_timing_table.v.blank;
    txpre_info->TXPRE_OUTPUT_HSYNC = tx_cea_timing_table.h.sync;
    txpre_info->TXPRE_OUTPUT_VSYNC = tx_cea_timing_table.v.sync;
    txpre_info->TXPRE_OUTPUT_FRAMERATE = tx_output_timing_table.frame_rate;
    txpre_info->TXPRE_OUTPUT_FORMAT = tx_output_timing_table.color_type;
    txpre_info->Output_Color_Space = tx_output_timing_table.color_type;
    txpre_info->Output_Color_Depth = tx_output_timing_table.color_depth; 
    txpre_info->Crop_H_STA = 0;
    txpre_info->Crop_V_STA = 0;
    txpre_info->Crop_WID = tx_output_timing_table.width;
    txpre_info->Crop_LEN = tx_output_timing_table.height;
    txpre_info->TXPRE_OUTPUT_DEN_HSTA = tx_cea_timing_table.h.back + tx_cea_timing_table.h.sync;
    txpre_info->TXPRE_OUTPUT_DEN_HEND = txpre_info->TXPRE_OUTPUT_DEN_HSTA + txpre_info->TXPRE_OUTPUT_ACT_WID;
    txpre_info->TXPRE_OUTPUT_DEN_VSTA = tx_cea_timing_table.v.back + tx_cea_timing_table.v.sync;
    txpre_info->TXPRE_OUTPUT_DEN_VEND = txpre_info->TXPRE_OUTPUT_DEN_VSTA + txpre_info->TXPRE_OUTPUT_ACT_LEN;

    txpre_info->Interlace = !tx_output_timing_table.progressive;
    txpre_info->isVRR = timing_info.run_vrr;
    txpre_info->isDsc = timing_info.is_dsc_to_tx;
}

extern void dvScript_mainDtg_config(UINT32 width, unsigned height, UINT16 frame_rate);
extern void HDMITX_verifier_init_mainDtg_ptg(UINT32 width, I3DDMA_COLOR_SPACE_T colorFmt);
void HDMITX_Input_timing_config(void)
{
    
    UINT8 bVgipEn; 
    TX_TIMING_INDEX timeIdx;
    UINT8 dscEn = 0;
    FatalMessageHDMITx("[HDMITX]timing_config %d\n",get_hdmitxOutputMode());
    hdmitx_set_ptg();
    bVgipEn= HDMITX_VGIP_HDMITX_VGIP_CTRL_get_hdmitx_ivrun(rtd_inl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg));

    timeIdx = hdmitx_ctrl_get_timing_type();
    if(timeIdx >= TX_TIMING_NUM){
        FatalMessageHDMITx("[HDMITX] INVALID idx[%d]@timingConfig\n", (UINT32)timeIdx);
        return;
    }
    dscEn = (timeIdx >= TX_TIMING_DSCE_START? 1: 0);
    ScalerHdmiTxSetDSCEn(dscEn);
    hdmitx_interlace_force_bypass();

    // update tx output timing table by timeIdx
    if((timing_info.run_vrr &&  (g_scaler_display_info.input_src == 1)) || (lib_hdmitx_industrial_ctrl_enable())||(get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE) || (timeIdx == TX_TIMING_DEFAULT))
        hdmi_get_tx_timing_info();
#ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
    else if((get_hdmitxOutputMode() == TX_OUT_SOURCE_MODE) && (edid_dtd_flag == 1))
        hdmi_get_dtd_timing_info();
#endif
    else if(ScalerHdmiTxPhy_GetTxTimingTable(timeIdx, &tx_cea_timing_table) !=  0){
        DebugMessageHDMITx("[HDMITX] Get timing_table Fail\n");
        return;
    }

    if((get_hdmitxOutputMode() == TX_OUT_SOURCE_MODE) && adjust_vtotal_flag) {
        update_adjust_vototal();
    }
    if((get_hdmitxOutputMode() == TX_OUT_SOURCE_MODE) && timing_info.run_vrr && adjust_vrr_vstart) {
        tx_cea_timing_table.v.back = adjust_vrr_vstart - tx_cea_timing_table.v.sync;
        tx_cea_timing_table.v.front = tx_cea_timing_table.v.blank - tx_cea_timing_table.v.back - tx_cea_timing_table.v.sync;
    }
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
    drvif_hdmi_pkt_bypass_to_hdmitx_en(0xF, _DISABLE);
#endif
    // get output info from Rx
    hdmi_get_tx_output_info();
#if IS_ENABLED(CONFIG_HDCPTX)
    SET_HDCP_TX_VFreq(0, tx_output_timing_table.frame_rate);
#endif
    memset(&txpre_info, 0x0, sizeof(StructTxPreInfo));

    HDMITX_InitTxPreInfo(&txpre_info);
    if(!bVgipEn)
        Txpretg_set_disp_setting(&txpre_info);

    if((!lib_hdmitx_is_hdmi_21_support()) && (tx_output_timing_table.frl_type >0))
    {//when hdmi only support 2.0 but rx send 2.1timing,need show no signal
        return;
    }
    // for 4k144 rbv2 cts verify
    setting4K144Rbv2Flag(timeIdx);
    // check need avi packet
    hdmi_set_need_sw_avi_packet();

    //adjust tx output dvi timing,when input is dvi timing
    ScalerHdmiMacTx0AdjustHdmiMode();

    hdmi_set_audio_sample_freq();

    // 2. config HDMITX input timing table
    hdmi_set_timingporch();
    hdmi_dump_timingporch();

    // 3* HDMITX HW init: TX PHY power on, TX Mac reset, HPD enable, TX interrupt enable(on SOC)
    ScalerHdmiMacTx0PowerProc(_POWER_ACTION_AC_ON_TO_NORMAL);

    // 4. HDMITX input timing config
    Scaler_TestCase_config();

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    // Set source AVI info frame
    ScalerHdmiTxRepeaterSetSourceInfo_aviInfoFrame();
    // Set source ALLM mode
    ScalerHdmiTxRepeaterSetSourceInfo_Allm(vfe_hdmi_drv_get_ALLM());
    // Set HDR mode
    ScalerHdmiTxRepeaterSetSourceInfo_hdrMode(get_HDMI_HDR_mode());
    // Set source Dolby Vision VSIF mode
    ScalerHdmiTxRepeaterSetSourceInfo_DvVsifMode(get_HDMI_Dolby_VSIF_mode());
    // Set source VRR_EN
    ScalerHdmiTxRepeaterSetSourceInfo_vtemVrrEn(vbe_disp_get_VRR_timingMode_flag());
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT

    // HDMITX Pattern Gen setting
    HDMITX_drv_misc_set_patternGen();

    // HDMITX Timing Gen setting
    HDMITX_drv_misc_set_timingGen();

	//dvScript_mainDtg_config(tx_output_timing_table.width, tx_output_timing_table.height, tx_output_timing_table.frame_rate);

        // 6-1. Main PTG
        //HDMITX_verifier_init_mainDtg_ptg(tx_output_timing_table.width, tx_output_timing_table.color_type);

    // wait timing stable
    HDMITX_DTG_Wait_Den_Stop_Done();

    return;
}


void HDMITX_phy_init(void)
{
    DebugMessageHDMITx("[HDMITX] HDMITX_phy_init...\n");
    // 3* HDMITX HW init: TX PHY power on, TX Mac reset, HPD enable, TX interrupt enable(on SOC)
    ScalerHdmiMacTx0PowerProc(_POWER_ACTION_AC_ON_TO_NORMAL);

    return;
}

