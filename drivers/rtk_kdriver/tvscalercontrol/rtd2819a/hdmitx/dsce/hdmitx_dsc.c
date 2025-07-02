/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2012>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : ScalerHdmiPhyTx.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include <rbus/dsce_misc_reg.h>
#include <rbus/dsce_reg.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/dsce_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/hdmitx_phy_reg.h>

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h> // fix by zhaodong
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h> // fix by zhaodong

#include "../include/ScalerFunctionInclude.h"
#include "../include/ScalerTx.h"
#include "../include/hdmitx_dsc_pps_struct_def.h"
#include "../include/hdmitx_phy_struct_def.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0Include.h"
#include "../include/Hdmi21MacTx/ScalerHdmi21MacTxInclude.h"
//#include "../include/HdmiTx_verifier.h"

#ifdef CONFIG_ENABLE_HDMITX
//#include <tvscalercontrol/i3ddma/i3ddma.h>
//#include <tvscalercontrol/scaler/scalerstruct.h>
#endif

//extern StructTimingInfo g_stHdmiTx0InputTimingInfo;
extern dsc_pps_st dsc_pps_8bit_table[HDMI_TX_DSC_PPS_SETTING_NUM] ;
extern dsc_pps_st dsc_pps_10bit_table[HDMI_TX_DSC_PPS_SETTING_NUM] ;
extern dsc_pps_st dsc_pps_12bit_table[HDMI_TX_DSC_PPS_SETTING_NUM] ;
extern UINT32 DTG_DH_WIDTH;
extern UINT32 DTG_DV_LENGTH;

extern dsc_pps_st dsc_pps_8bit_FRL_12G_table[5];
extern UINT8 ScalerHdmiTxGetDscMode(void);
void ScalerHdmi21MacTx0DscTimingGenSetting(void);

// DSC clock info
typedef struct {
    UINT8 dsc_src_div; // 0: div1, 1: div2, 2: div4, 3: div8
    UINT8 pixel_mode; // 0: 1 pixel mode, 1: 2 pixel mode, 2: 4 pixel mode, 3: 8 pixel mode
} dsc_enc_clock_st;

#define DSC_ENC_CLK_INFO_SIZE 3//7
dsc_enc_clock_st dsc_enc_clock[DSC_ENC_CLK_INFO_SIZE] = {
/*
    dsc_src_div   : log2(dsc_enc_clk_p/ dsc_enc_clk_c) (0: div1, 1: div2, 2: div4, 3: div8)
    pixel_mode    : column I (0: 1 pixel mode, 1: 2 pixel mode, 2: 4 pixel mode, 3: 8 pixel mode)
    {dsc_src_div,pixel_mode}
*/
{1, 1}, // HDMI21_TX_DSCE_3840_2160P_444_60Hz_6G4L
{1, 1}, // HDMI21_TX_DSCE_3840_2160P_444_120Hz_6G4L
{1, 1}, // HDMI21_TX_DSCE_3840_2160P_444_144Hz_6G4L

#if 0

{0, 2}, // HDMI21_TX_DSCE_3840_2160P_444_120Hz_6G4L
{0, 1}, // HDMI21_TX_DSCE_5120_2160P_420_60Hz_6G4L
{1, 1}, // HDMI21_TX_DSCE_7680_4320P_420_30Hz_6G4L
{0, 2}, // HDMI21_TX_DSCE_7680_4320P_444_30Hz_6G4L
{1, 2}, // HDMI21_TX_DSCE_7680_4320P_444_60Hz_6G4L
{1, 2}, // HDMI21_TX_DSCE_7680_4320P_444_60Hz_12G
#endif
};

StructDscEncoderParameter dscEncoderParameter={0};


void ScalerDscEncoderSetParameter(EnumOutputPort enumOutputPort, StructDscEncoderParameter *param)
{
    memcpy(&dscEncoderParameter, param, sizeof(StructDscEncoderParameter));
    return;
}


StructDscEncoderParameter *ScalerDscEncoderGetParameter(EnumOutputPort enumOutputPort)
{
    return &dscEncoderParameter;
}



//--------------------------------------------------
// Description  : H5X HDMI Tx DSC pps config
// Input Value  : Hdmitx input source: deep_depth, deep_color, setting_idx
// Output Value :
//--------------------------------------------------
void ScalerHdmiTx0_H5xDsc_PpsConfig(UINT32 setting_idx, UINT8 deep_color, UINT8 deep_depth)
{
    INT32 i =0;
    dsc_pps_st* pps_param;
    UINT8 dscMode;

    if(setting_idx >= HDMI_TX_DSC_PPS_SETTING_NUM){
        ErrorMessageHDMITx("[HDMITX][ERROR] setting_idx[%d] OVER RANGE[%d]@%s\n", setting_idx, HDMI_TX_DSC_PPS_SETTING_NUM, __FUNCTION__);
        return;
    }

    if (deep_color)
    {
        switch (deep_depth) {
            case 10:
                pps_param = &dsc_pps_10bit_table[0] + setting_idx;
                break;
            case 12:
                pps_param = &dsc_pps_12bit_table[0] + setting_idx;
                break;
            default:
                pps_param = &dsc_pps_8bit_table[0] + setting_idx;
                break;
        }
    }
    else
    {
        pps_param = &dsc_pps_8bit_table[0] + setting_idx;
    }

    // 8kp60 444@12G DSC mode: 0: default, 1: bbp=8.125, 2: bbp=9.9375, 3: bbp=12.000, 4: bbp=15.000
    dscMode = ScalerHdmiTxGetDscMode();
    if((setting_idx == HDMI21_TX_7680_4320P_444_60Hz_12G4L) && (deep_depth < 10) && (dscMode > 1) && (dscMode < (DSC_12G_BBP_NUM+1))){
        pps_param = &dsc_pps_8bit_FRL_12G_table[0] + (dscMode-1);
        NoteMessageHDMITx("[HDMITX]\e[1;31m DSC 12G Mode=%d\e[0m@%s\n", dscMode-1, __FUNCTION__);
    }

    memcpy(&g_ppucHdmi21MacTxPPSData[_TX0][0],pps_param,128);

    for(i =0; i <94 ;i++)//DSC_PPS00~DSC_PPS93
    {
        rtd_outl(DSCE_DSC_PPS_00_reg + i*4 ,pps_param->dsc_pps[i]);

        //DebugMessageHDMITx("[HDMITX] %s, 0x%x = %x\n", __FUNCTION__,DSCE_DSC_PPS_00_reg + i*4,rtd_inl(DSCE_DSC_PPS_00_reg + + i*4));
    }

    return;
}


HDMI_TX_DSC_PPS_SETTING_INDEX ScalerHdmiTxGetDscPpsSettingIndex(UINT32 format_idx)
{
    UINT32 width, height;
    UINT8 colorspace;

    if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
        ErrorMessageHDMITx("[HDMITX][ERROR] g_pstHdmi21MacTxInputTimingInfo is NULL@%s\n", __FUNCTION__);
        return NO_DSC_PPS_SETTING_INDEX;
    }

    width = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth;
    height = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight;
    colorspace = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_SPACE);

    //test case
#if (TEST_MODE == DV_SCRIPT)
    width = 7680;
    height = 108;
#endif
    //UINT8 deep_color_space = GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE();

    //need fix when more timing and color space add
    if(height <= 108) // DV test case
    {
        if(width <=3840)
            return HDMI21_TX_DV_3840_2160P_444_60HZ;
        else if(width <=5120)
            return HDMI21_TX_DV_5120_2160P_420_60HZ;
        else // 0: RGB888, 1: YUV422, 2: YUV444, 3: YUV 420
            return (colorspace == 2? HDMI21_TX_DV_7680_4320P_444_60HZ:HDMI21_TX_DV_7680_4320P_420_30HZ);
    }
    else


    {
        // not DSC enabled timing
        if((format_idx < TX_TIMING_DSCE_START) || (format_idx >= TX_TIMING_NUM))
        {
            if(GET_HDMI21_MAC_TX_DSCE_EN(_TX0))
                ErrorMessageHDMITx("[HDMITX][ERROR] NO sutiable DSC_PPS_INDEX %d!!\n", format_idx);
            return NO_DSC_PPS_SETTING_INDEX;
        }

        // check if timing table changed
        if(((TX_TIMING_NUM - TX_TIMING_DSCE_START) != DSC_ENC_CLK_INFO_SIZE)
           || ((format_idx - TX_TIMING_DSCE_START) >= DSC_ENC_CLK_INFO_SIZE)){
                ErrorMessageHDMITx("[HDMITX][ERROR][PPS_CFG] TX_TIMING_INDEX NOT SYNC, VideoIdx start/idx/max=%d/%d/%d, num=%d/%d!!\n",
                TX_TIMING_DSCE_START, format_idx, TX_TIMING_NUM, format_idx - TX_TIMING_DSCE_START, DSC_ENC_CLK_INFO_SIZE);
            return NO_DSC_PPS_SETTING_INDEX;
        }

        //convert video timing index to dsc pps setting index
        format_idx -= TX_TIMING_DSCE_START;
        return format_idx;

    }

    return 0;

}

UINT32 dsc_clk_ratio(INT32 reg_data)
{
    switch (reg_data)
    {
        case 0:
            return 1000;
        case 2:
            return 750;
        case 4:
            return 500;
        case 6:
            return 250;
        case 7:
            return 125;
        default:
            return 1000;
    }

    return 1000;
}

void ScalerHdmiTX0_H5xDsc_DPFConfig(UINT32 setting_idx, UINT8 deep_color, UINT8 deep_depth)
{
#if 0
#ifndef _MARK2_ZEBU_BRING_UP_RUN // [MARK2] FIX-ME -- only for Zebu Verify (zebu not support 3lane mode)
    INT32 i;
    for(i=0; i<10; i++){
        ErrorMessageHDMITx("\n\n[HDMITX]***************************\n");
        ErrorMessageHDMITx("[HDMITX][DSC][ERROR][%d] FIX-ME@%s\n", i, __FUNCTION__);
        ErrorMessageHDMITx("[HDMITX]***************************\n\n\n");
        msleep(1000);
    }
#endif
#endif
//#ifdef _MARK2_FIXME_DSCE_COMPILER_ERROR // [MARK2] FIX-ME -- Compiler error when calculating certain values
    //Scaler_TestCase_config()
    INT32 width, height, ipt_hor_porch;
    INT32 val, msb, lsb;
    INT32 dsc_str_width;
    INT32 dsc_str_htt;
    INT32 dsc_str_hsw;
    INT32 dsc_enc_frame_sync;
    INT32 full_last_line = 1;
    INT32 slice_cnt;
    UINT32 opt_stream_period;
    INT32 dpf_hst;
    INT32 dpf_front_porch;
    UINT32 dsc_enc_clk_s, dsc_enc_clk_x,dsc_enc_clk_p,dsc_enc_clk_c;
    //pps
    INT32 native420 ;
    INT32 native422 ;
    INT32 initialDelay;
    INT32 bitsPerPixel;
   // INT32 dsc_version_minor;
    INT32 bits_per_component;
    INT32 slice_width;
    INT32 initial_xmit_delay;
    //pps
    //ref dvScript_txsoc_config();
    //Ten-Yao suggest use MNT setting (3T) to set DPF hsync width
    INT32   dpf_hsw = _HDMI21_DSC_DPF_SYNCWIDTH*2; // Horizontal Sync Pulse Count (unit: SyncProc Clock)
    INT32   dpf_vtt = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_TOTAL); // Vertical Total length (unit: HSync)
    //  ScalerHdmi21MacTx0InputConverter(); Vertical Sync +  Vertical back porch
    INT32   dpf_vst;// = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVStart;
    INT32   dpf_vsw = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_VSYNC); // Vertical Sync Width    (unit: HSync)


    dsce_dsc_enc_ctrl_RBUS dsce_dsc_enc_ctrl_reg;
    dsce_dsc_output_ctrl_RBUS dsce_dsc_output_ctrl_reg;
    dsce_mn_dpf_htt_m_RBUS dsce_mn_dpf_htt_m_reg;
    dsce_mn_dpf_htt_l_RBUS dsce_mn_dpf_htt_l_reg;
    dsce_mn_dpf_hst_m_RBUS dsce_mn_dpf_hst_m_reg;
    dsce_mn_dpf_hst_l_RBUS dsce_mn_dpf_hst_l_reg;
    dsce_mn_dpf_hwd_m_RBUS dsce_mn_dpf_hwd_m_reg;
    dsce_mn_dpf_hwd_l_RBUS dsce_mn_dpf_hwd_l_reg;
    dsce_mn_dpf_hsw_m_RBUS dsce_mn_dpf_hsw_m_reg;
    dsce_mn_dpf_hsw_l_RBUS dsce_mn_dpf_hsw_l_reg;
    dsce_mn_dpf_vtt_m_RBUS dsce_mn_dpf_vtt_m_reg;
    dsce_mn_dpf_vtt_l_RBUS dsce_mn_dpf_vtt_l_reg;
    dsce_mn_dpf_vst_m_RBUS dsce_mn_dpf_vst_m_reg;
    dsce_mn_dpf_vst_l_RBUS dsce_mn_dpf_vst_l_reg;
    dsce_mn_dpf_vht_m_RBUS dsce_mn_dpf_vht_m_reg;
    dsce_mn_dpf_vht_l_RBUS dsce_mn_dpf_vht_l_reg;
    dsce_mn_dpf_vsw_m_RBUS dsce_mn_dpf_vsw_m_reg;
    dsce_mn_dpf_vsw_l_RBUS dsce_mn_dpf_vsw_l_reg;
    dsce_evblk2vs_h_RBUS dsce_evblk2vs_h_reg;
    dsce_evblk2vs_m_RBUS dsce_evblk2vs_m_reg;
    dsce_evblk2vs_l_RBUS dsce_evblk2vs_l_reg;
    dsce_dp_output_ctrl_RBUS dsce_dp_output_ctrl_reg;
    dsce_dpf_ctrl_0_RBUS dsce_dpf_ctrl_0_reg;
    dsc_pps_st* pps_param;
    UINT8 dscMode;


    if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
        ErrorMessageHDMITx("[HDMITX][ERROR] g_pstHdmi21MacTxInputTimingInfo is NULL@%s\n", __FUNCTION__);
        return;
    }

    width = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth;
    height = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight;
    ipt_hor_porch = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth;
    dpf_vst = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVStart;

#if(TEST_MODE == DV_SCRIPT)
    width = 7680;
    height = 108;
    ipt_hor_porch = 10800-7680;

    dpf_hsw = 176;
    dpf_vtt = 108 + 4 + 2 + 6; //vfront 4, vsync 2, vback porch 6
    dpf_vst = 6 + 2;
    dpf_vsw = 2;
#endif
    if (deep_color)
    {
        switch (deep_depth) {
            case 10:
                pps_param = &dsc_pps_10bit_table[0] + setting_idx;
                break;
            case 12:
                pps_param = &dsc_pps_12bit_table[0] + setting_idx;
                break;
            default:
                pps_param = &dsc_pps_8bit_table[0] + setting_idx;
                break;
        }
    }
    else
    {
        pps_param = &dsc_pps_8bit_table[0] + setting_idx;
    }

    // 8kp60 444@12G DSC mode: 0: default, 1: bbp=8.125, 2: bbp=9.9375, 3: bbp=12.000, 4: bbp=15.000
    dscMode = ScalerHdmiTxGetDscMode();
    if((setting_idx == HDMI21_TX_7680_4320P_444_60Hz_12G4L) && (deep_depth < 10) && (dscMode > 1) && (dscMode < (DSC_12G_BBP_NUM+1))){
        pps_param = &dsc_pps_8bit_FRL_12G_table[0] + (dscMode-1);
        NoteMessageHDMITx("[HDMITX] DSC 12G Mode=%d@%s\n", dscMode-1, __FUNCTION__);
    }

    //pps need to read value
    native420 = DSCE_DSC_PPS_88_get_native_420(pps_param->dsc_pps[88]);
    native422 = DSCE_DSC_PPS_88_get_native_422(pps_param->dsc_pps[88]);
    bitsPerPixel = (DSCE_DSC_PPS_04_get_bits_per_pixel_9_8(pps_param->dsc_pps[4]) <<8)
                        | DSCE_DSC_PPS_05_bits_per_pixel_7_0(pps_param->dsc_pps[5]);
    //dsc_version_minor = DSCE_DSC_PPS_00_get_dsc_version_minor(pps_param->dsc_pps[0]);
    bits_per_component = DSCE_DSC_PPS_03_get_bits_per_component(pps_param->dsc_pps[3]);
    slice_width = (DSCE_DSC_PPS_12_get_slice_width_15_8(pps_param->dsc_pps[12]) <<8)
                        | DSCE_DSC_PPS_13_get_slice_width_7_0(pps_param->dsc_pps[13]);
    initial_xmit_delay = (DSCE_DSC_PPS_16_get_initial_xmit_delay_9_8(pps_param->dsc_pps[16]) <<8)
                        | DSCE_DSC_PPS_17_get_initial_xmit_delay_7_0(pps_param->dsc_pps[17]);
    //pps


    dsc_enc_clk_s = dsc_clk_ratio(HDMITX_PHY_HDMITX_CLK_DIV_get_reg_clkratio_dsce_s(rtd_inl(HDMITX_PHY_HDMITX_CLK_DIV_reg)));
    dsc_enc_clk_x = dsc_clk_ratio(HDMITX_PHY_HDMITX_CLK_DIV_get_reg_clkratio_hdmitx_ds444_2p(rtd_inl(HDMITX_PHY_HDMITX_CLK_DIV_reg)));
    dsc_enc_clk_p = dsc_clk_ratio(HDMITX_PHY_HDMITX_CLK_DIV_get_reg_clkratio_dsce_p(rtd_inl(HDMITX_PHY_HDMITX_CLK_DIV_reg)));
    dsc_enc_clk_c = dsc_clk_ratio(HDMITX_PHY_HDMITX_CLK_DIV_get_reg_clkratio_dsce_c(rtd_inl(HDMITX_PHY_HDMITX_CLK_DIV_reg)));
    NoteMessageHDMITx("[HDMITX] dsc_enc_clk_s = %d, x = %d, p = %d, c = %d\n",dsc_enc_clk_s,dsc_enc_clk_x,dsc_enc_clk_p, dsc_enc_clk_c);




    slice_cnt = (width + slice_width-1) / slice_width;

	//slice one line bits count
    dsc_str_width = bitsPerPixel*(slice_width/(1+native422)/(1+native420))/16;
    dsc_str_width = (dsc_str_width + 63)/64;

    NoteMessageHDMITx("[HDMITX] dsc_str_width = %d, slice_cnt = %d, slice_width = %d, ipt_hor_porch = %d\n",dsc_str_width,slice_cnt,slice_width, ipt_hor_porch);
    if(dsc_enc_clk_s/dsc_enc_clk_x==8)
    {
        opt_stream_period = (((1+native420)*8*dsc_enc_clk_x/dsc_enc_clk_s*dsc_enc_clk_c*128/dsc_enc_clk_s)*(slice_cnt*dsc_str_width)/(slice_width/(1+native422)));  // BPP setting is consider line porch time percentage
    }
    else if(dsc_enc_clk_s/dsc_enc_clk_x==4)
    {
        opt_stream_period = (((1+native420)*4*dsc_enc_clk_x/dsc_enc_clk_s*dsc_enc_clk_c*128/dsc_enc_clk_s)*(slice_cnt*dsc_str_width)/(slice_width/(1+native422)));
    }
    else if(dsc_enc_clk_s/dsc_enc_clk_x==2)
    {
        opt_stream_period = (((1+native420)*2*dsc_enc_clk_x/dsc_enc_clk_s*dsc_enc_clk_c*128/dsc_enc_clk_s)*(slice_cnt*dsc_str_width)/(slice_width/(1+native422)));
    }
    else
    {
        opt_stream_period = (((1+native420)*(dsc_enc_clk_s/dsc_enc_clk_x)*dsc_enc_clk_x/dsc_enc_clk_s*dsc_enc_clk_c*128/dsc_enc_clk_s)*(slice_cnt*dsc_str_width)/(slice_width/(1+native422)));
    }


    if (opt_stream_period==0) {
        opt_stream_period = 67 * (slice_cnt*dsc_str_width) / (slice_width/(1+native422)/(1+native420));
    }

    if(opt_stream_period>128)
	    opt_stream_period = 128;

    dsc_str_htt = (width + ipt_hor_porch)/2/(1+native420) *(dsc_enc_clk_s/dsc_enc_clk_x);

    dsc_str_hsw = dpf_hsw/2/(1+native420);


    if((((slice_cnt*dsc_str_width-1)*128)%opt_stream_period)!=0){
      dpf_front_porch = (slice_cnt*dsc_str_width-1)*128/opt_stream_period +1;
    }else{
      dpf_front_porch = (slice_cnt*dsc_str_width-1)*128/opt_stream_period ;
    }


    NoteMessageHDMITx("[HDMITX] dsc_str_htt= %d, dsc_str_hsw = %d, opt_stream_period = %d, dpf_front_porch = %d\n",dsc_str_htt,dsc_str_hsw,opt_stream_period,dpf_front_porch);
    dpf_hst =  dsc_str_htt - 11 - (1 + dpf_front_porch);

    dsc_str_width = (1 + dpf_front_porch);

    if(slice_cnt <= 4){
      dsce_dsc_enc_ctrl_reg.regValue = rtd_inl(DSCE_DSC_ENC_CTRL_reg);
      dsce_dsc_enc_ctrl_reg.merge_buf_en = 1; //
      rtd_outl(DSCE_DSC_ENC_CTRL_reg, dsce_dsc_enc_ctrl_reg.regValue);
    }
    else
    {
        if(slice_width>1280)
        {
            dsce_dsc_enc_ctrl_reg.regValue = rtd_inl(DSCE_DSC_ENC_CTRL_reg);
            dsce_dsc_enc_ctrl_reg.merge_buf_en = 1; //
            rtd_outl(DSCE_DSC_ENC_CTRL_reg, dsce_dsc_enc_ctrl_reg.regValue);
        }
        else
        {
            dsce_dsc_enc_ctrl_reg.regValue = rtd_inl(DSCE_DSC_ENC_CTRL_reg);
            dsce_dsc_enc_ctrl_reg.merge_buf_en = 0; //
            rtd_outl(DSCE_DSC_ENC_CTRL_reg, dsce_dsc_enc_ctrl_reg.regValue);
        }
    }

    val = dsc_str_htt;
    msb = val/256;
    lsb = val%256;
    dsce_mn_dpf_htt_m_reg.regValue = rtd_inl(DSCE_MN_DPF_HTT_M_reg);
    dsce_mn_dpf_htt_m_reg.dpf_htt_15_8 = msb;
    rtd_outl(DSCE_MN_DPF_HTT_M_reg, dsce_mn_dpf_htt_m_reg.regValue); //HTotal [15:8]

    dsce_mn_dpf_htt_l_reg.regValue = rtd_inl(DSCE_MN_DPF_HTT_L_reg);
    dsce_mn_dpf_htt_l_reg.dpf_htt_7_0 = lsb;
    rtd_outl(DSCE_MN_DPF_HTT_L_reg, dsce_mn_dpf_htt_l_reg.regValue); //HTotal [7:0]

    msb = dpf_hst/256;
    lsb = dpf_hst%256;
    dsce_mn_dpf_hst_m_reg.regValue = rtd_inl(DSCE_MN_DPF_HST_M_reg);
    dsce_mn_dpf_hst_m_reg.dpf_hst_15_8 = msb;
    rtd_outl(DSCE_MN_DPF_HST_M_reg, dsce_mn_dpf_hst_m_reg.regValue); //Horizontal Start [15:8]

    dsce_mn_dpf_hst_l_reg.regValue = rtd_inl(DSCE_MN_DPF_HST_L_reg);
    dsce_mn_dpf_hst_l_reg.dpf_hst_7_0 = lsb;
    rtd_outl(DSCE_MN_DPF_HST_L_reg, dsce_mn_dpf_hst_l_reg.regValue); //Horizontal Start [7:0]

    val = dsc_str_width;
    msb = val/256;
    lsb = val%256;
    dsce_mn_dpf_hwd_m_reg.regValue = rtd_inl(DSCE_MN_DPF_HWD_M_reg);
    dsce_mn_dpf_hwd_m_reg.dpf_hwd_15_8 = msb;
    rtd_outl(DSCE_MN_DPF_HWD_M_reg, dsce_mn_dpf_hwd_m_reg.regValue); //Horizontal Active Width [15:8]

    dsce_mn_dpf_hwd_l_reg.regValue = rtd_inl(DSCE_MN_DPF_HWD_L_reg);
    dsce_mn_dpf_hwd_l_reg.dpf_hwd_7_0 = lsb;
    rtd_outl(DSCE_MN_DPF_HWD_L_reg, dsce_mn_dpf_hwd_l_reg.regValue); //Horizontal Active Width [7:0]

    msb = dsc_str_hsw/256;
    lsb = dsc_str_hsw%256;
    dsce_mn_dpf_hsw_m_reg.regValue = rtd_inl(DSCE_MN_DPF_HSW_M_reg);
    dsce_mn_dpf_hsw_m_reg.dpf_hsp = 0;
    dsce_mn_dpf_hsw_m_reg.dpf_hsw_14_8 = msb;
    rtd_outl(DSCE_MN_DPF_HSW_M_reg, dsce_mn_dpf_hsw_m_reg.regValue); //Horizontal Sync Pulse [14:8]

    dsce_mn_dpf_hsw_l_reg.regValue = rtd_inl(DSCE_MN_DPF_HSW_L_reg);
    dsce_mn_dpf_hsw_l_reg.dpf_hsw_7_0 = lsb;
    rtd_outl(DSCE_MN_DPF_HSW_L_reg, lsb); //Horizontal Sync Pulse [7:0]

    // Vtotal
    msb = dpf_vtt/256;
    lsb = dpf_vtt%256;
    dsce_mn_dpf_vtt_m_reg.regValue = rtd_inl(DSCE_MN_DPF_VTT_M_reg);
    dsce_mn_dpf_vtt_m_reg.dpf_vtt_15_8 = msb;
    rtd_outl(DSCE_MN_DPF_VTT_M_reg, dsce_mn_dpf_vtt_m_reg.regValue); //VTotal [15:8]

    dsce_mn_dpf_vtt_l_reg.regValue = rtd_inl(DSCE_MN_DPF_VTT_L_reg);
    dsce_mn_dpf_vtt_l_reg.dpf_vtt_7_0 = lsb;
    rtd_outl(DSCE_MN_DPF_VTT_L_reg, dsce_mn_dpf_vtt_l_reg.regValue); //VTotal [7:0]

    msb = dpf_vst/256;
    lsb = dpf_vst%256;

    dsce_mn_dpf_vst_m_reg.regValue = rtd_inl(DSCE_MN_DPF_VST_M_reg);
    dsce_mn_dpf_vst_m_reg.dpf_vst_15_8 = msb;
    rtd_outl(DSCE_MN_DPF_VST_M_reg, dsce_mn_dpf_vst_m_reg.regValue); //Vertical Start [15:8]

    dsce_mn_dpf_vst_l_reg.regValue = rtd_inl(DSCE_MN_DPF_VST_L_reg);
    dsce_mn_dpf_vst_l_reg.dpf_vst_7_0 = lsb;
    rtd_outl(DSCE_MN_DPF_VST_L_reg, dsce_mn_dpf_vst_l_reg.regValue); //Vertical Start [7:0]

    val = height;
    msb = val/256;
    lsb = val%256;
    dsce_mn_dpf_vht_m_reg.regValue = rtd_inl(DSCE_MN_DPF_VHT_M_reg);
    dsce_mn_dpf_vht_m_reg.dpf_vht_15_8 = msb;
    rtd_outl(DSCE_MN_DPF_VHT_M_reg, dsce_mn_dpf_vht_m_reg.regValue); //Vertical Active Height [15:8]

    dsce_mn_dpf_vht_l_reg.regValue = rtd_inl(DSCE_MN_DPF_VHT_L_reg);
    dsce_mn_dpf_vht_l_reg.dpf_vht_7_0 = lsb;
    rtd_outl(DSCE_MN_DPF_VHT_L_reg, dsce_mn_dpf_vht_l_reg.regValue); //Vertical Active Height [7:0]


    msb = dpf_vsw/256;
    lsb = dpf_vsw%256;
    dsce_mn_dpf_vsw_m_reg.regValue = rtd_inl(DSCE_MN_DPF_VSW_M_reg);
    dsce_mn_dpf_vsw_m_reg.dpf_vsp = 0;
    dsce_mn_dpf_vsw_m_reg.dpf_vsw_14_8 = msb;
    rtd_outl(DSCE_MN_DPF_VSW_M_reg, dsce_mn_dpf_vsw_m_reg.regValue); //Vertical Sync Pulse [14:8]

    dsce_mn_dpf_vsw_l_reg.regValue = rtd_inl(DSCE_MN_DPF_VSW_L_reg);
    dsce_mn_dpf_vsw_l_reg.dpf_vsw_7_0 = lsb;
    rtd_outl(DSCE_MN_DPF_VSW_L_reg, dsce_mn_dpf_vsw_l_reg.regValue); //Vertical Sync Pulse [7:0]


    initialDelay= initial_xmit_delay;
    if (bits_per_component==12)
    {
        val = (1+native422)*(1+native420)*(initialDelay+3*64+3*52-3*3)/slice_width; // delay line count
        val = (slice_cnt-1)*dsc_str_width/slice_cnt + (dsc_str_htt-dsc_str_width) +
                  val*dsc_str_htt +
                  ((1+native422)*(1+native420)*(initialDelay+3*64+3*52-3*3)-(val*slice_width))*dsc_str_width/slice_width;
    }
    else{
        val = (1+native422)*(1+native420)*(initialDelay+3*48+3*44-3*3)/slice_width; // delay line count
        val = (slice_cnt-1)*dsc_str_width/slice_cnt + (dsc_str_htt-dsc_str_width) +
                  val*dsc_str_htt +
                  ((1+native422)*(1+native420)*(initialDelay+3*48+3*44-3*3)-(val*slice_width))*dsc_str_width/slice_width;
    }

    dsc_enc_frame_sync = (val>=(2.8*dsc_str_htt)) ? 1 : 0;

    // evblk2vs
    msb = val/256;
    lsb = val%256;

    dsce_evblk2vs_h_reg.regValue = rtd_inl(DSCE_EVBLK2VS_H_reg);
    dsce_evblk2vs_h_reg.evblk2vs_23_16 = 0;
    rtd_outl(DSCE_EVBLK2VS_H_reg, dsce_evblk2vs_h_reg.regValue); //EVBLK2VS [23:16]

    dsce_evblk2vs_m_reg.regValue = rtd_inl(DSCE_EVBLK2VS_M_reg);
    dsce_evblk2vs_m_reg.evblk2vs_15_8 = msb;
    rtd_outl(DSCE_EVBLK2VS_M_reg, dsce_evblk2vs_m_reg.regValue); //EVBLK2VS [15:8]

    dsce_evblk2vs_l_reg.regValue = rtd_inl(DSCE_EVBLK2VS_L_reg);
    dsce_evblk2vs_l_reg.evblk2vs_7_0 = lsb;
    rtd_outl(DSCE_EVBLK2VS_L_reg, dsce_evblk2vs_l_reg.regValue); //EVBLK2VS [7:0]


    rtd_outl(DSCE_OVBLK2VS_H_reg, 0x00); //OVBLK2VS [23:16]
    rtd_outl(DSCE_OVBLK2VS_M_reg, 0x00); //OVBLK2VS [15:8]
    rtd_outl(DSCE_OVBLK2VS_L_reg, 0x00); //OVBLK2VS [7:0]
    rtd_outl(DSCE_VS_FRONT_PORCH_reg, 0x02); //vs front porch

    dsce_dp_output_ctrl_reg.regValue = rtd_inl(DSCE_DP_OUTPUT_CTRL_reg);
    dsce_dp_output_ctrl_reg.vblank_start_gen_en = 0;
    dsce_dp_output_ctrl_reg.freerun_en = 0;
    rtd_outl(DSCE_DP_OUTPUT_CTRL_reg, dsce_dp_output_ctrl_reg.regValue); //

    if(full_last_line)
    {
        dsce_dpf_ctrl_0_reg.regValue = rtd_inl(DSCE_DPF_CTRL_0_reg);
        dsce_dpf_ctrl_0_reg.disp_en = 0;
        dsce_dpf_ctrl_0_reg.deonly_full_mode = 0;
        dsce_dpf_ctrl_0_reg.frame_sync_mode = 0;
        dsce_dpf_ctrl_0_reg.deonly_mode = 0;
        dsce_dpf_ctrl_0_reg.vs_bs_or_be = 0;
        dsce_dpf_ctrl_0_reg.lastline_pixel = 0;
        rtd_outl(DSCE_DPF_CTRL_0_reg, dsce_dpf_ctrl_0_reg.regValue);
    }
    else if(dsc_enc_frame_sync)
    {
        dsce_dpf_ctrl_0_reg.regValue = rtd_inl(DSCE_DPF_CTRL_0_reg);
        dsce_dpf_ctrl_0_reg.disp_en = 0;
        dsce_dpf_ctrl_0_reg.deonly_full_mode = 0;
        dsce_dpf_ctrl_0_reg.frame_sync_mode = 1;
        dsce_dpf_ctrl_0_reg.deonly_mode = 0;
        dsce_dpf_ctrl_0_reg.vs_bs_or_be = 0;
        dsce_dpf_ctrl_0_reg.lastline_pixel = 0;
        rtd_outl(DSCE_DPF_CTRL_0_reg, dsce_dpf_ctrl_0_reg.regValue);
    }
    else
    {
        dsce_dpf_ctrl_0_reg.regValue = rtd_inl(DSCE_DPF_CTRL_0_reg);
        dsce_dpf_ctrl_0_reg.disp_en = 0;
        dsce_dpf_ctrl_0_reg.deonly_full_mode = 0;
        dsce_dpf_ctrl_0_reg.frame_sync_mode = 1;
        dsce_dpf_ctrl_0_reg.deonly_mode = 1;
        dsce_dpf_ctrl_0_reg.vs_bs_or_be = 0;
        dsce_dpf_ctrl_0_reg.lastline_pixel = 0;
        rtd_outl(DSCE_DPF_CTRL_0_reg, dsce_dpf_ctrl_0_reg.regValue);
    }

    dsce_dsc_output_ctrl_reg.regValue = rtd_inl(DSCE_DSC_OUTPUT_CTRL_reg);
    dsce_dsc_output_ctrl_reg.opt_stream_period = opt_stream_period;

    rtd_outl(DSCE_DSC_OUTPUT_CTRL_reg, dsce_dsc_output_ctrl_reg.regValue);
//#endif // [MARK2] FIX-ME -- compiler error on some value calculate
    return;
}


void ScalerHdmiTx0DscPPSConfig(UINT32 format_idx)
{
    HDMI_TX_DSC_PPS_SETTING_INDEX setting_idx;
    UINT8 deep_color, deep_depth;

    deep_depth = GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0);
    deep_color = (deep_depth > 8? 1: 0);
    setting_idx= ScalerHdmiTxGetDscPpsSettingIndex(format_idx);

#if (TEST_MODE == DV_SCRIPT)

    deep_depth = 12;
    deep_color = (deep_depth > 8? 1: 0);

#endif
    InfoMessageHDMITx("[HDMITX][DSC] Input/Pps Idx=%d(%d)/%d(%d)@depth=%d\n", format_idx, TX_TIMING_NUM, setting_idx, HDMI_TX_DSC_PPS_SETTING_NUM, deep_depth);
    if(setting_idx == NO_DSC_PPS_SETTING_INDEX)
    {
        return;
    }
    ScalerHdmiTX0_H5xDsc_DPFConfig(setting_idx, deep_color, deep_depth);

    ScalerHdmiTx0_H5xDsc_PpsConfig(setting_idx, deep_color, deep_depth);
    //ScalerHdmi21MacTx0DscTimingGenSetting();

    return;

}



//--------------------------------------------------
// Description  : H5X HDMI Tx DSC source config
// Input Value  : Hdmitx dsc input source: 0x0: TXSOC, 0x1: DISPD
// Output Value :
//--------------------------------------------------
void ScalerHdmiTx0_H5xDsc_inputSrcConfig(EnumHDMITXDscSrc dsc_src)
{
    dsce_misc_dsce_src_sel_RBUS dsce_misc_dsce_src_sel_reg;

    dsce_misc_dsce_src_sel_reg.regValue = rtd_inl(DSCE_MISC_DSCE_SRC_SEL_reg);

    dsce_misc_dsce_src_sel_reg.bypass_dsce = 0;
    dsce_misc_dsce_src_sel_reg.hdmitx_dsc_source_sel = 0;
    rtd_outl(DSCE_MISC_DSCE_SRC_SEL_reg, dsce_misc_dsce_src_sel_reg.regValue);

    NoteMessageHDMITx("[HDMITX] hdmitx_dsc_source_sel=%d@%s\n", DSCE_MISC_DSCE_SRC_SEL_get_hdmitx_dsc_source_sel(rtd_inl(DSCE_MISC_DSCE_SRC_SEL_reg)), __FUNCTION__);

    return;
}


dsc_enc_clock_st *ScalerHdmiTx0_H5xDsc_getDscTimingInfo(UINT32 format_idx)
{
    INT32 dsc_clock_idx = format_idx - TX_TIMING_DSCE_START;
    // not DSC enabled timing
    if((format_idx < TX_TIMING_DSCE_START) || (format_idx >= TX_TIMING_NUM))
        return NULL;

    // check if timing table changed
    if(((TX_TIMING_NUM - TX_TIMING_DSCE_START) != DSC_ENC_CLK_INFO_SIZE)
        || ((format_idx - TX_TIMING_DSCE_START) >= DSC_ENC_CLK_INFO_SIZE)){
        ErrorMessageHDMITx("[HDMITX][ERROR][ENC_CLK] TX_TIMING_INDEX NOT SYNC, VideoIdx start/idx/max=%d/%d/%d, num=%d/%d!!\n",
            TX_TIMING_DSCE_START, format_idx, TX_TIMING_NUM, format_idx - TX_TIMING_DSCE_START, DSC_ENC_CLK_INFO_SIZE);
        return NULL;
    }

#if 0 // TEST
    NoteMessageHDMITx("[HDMITX] TX_TIMING_INDEX INFO, VideoIdx start/idx/max=%d/%d/%d, num=%d/%d!!\n",
        TX_TIMING_DSCE_START, format_idx, TX_TIMING_NUM,
        format_idx - TX_TIMING_DSCE_START, DSC_ENC_CLK_INFO_SIZE);
#endif


    return &dsc_enc_clock[dsc_clock_idx];
}


void ScalerHdmiTx0_H5xDsc_encClkConfig(UINT32 format_idx)
{
    dsce_dsc_enc_ctrl_RBUS dsce_dsc_enc_ctrl_reg;
    dsce_dsc_clk_ctrl_RBUS dsce_dsc_clk_ctrl_reg;
    dsce_dpf_ctrl_0_RBUS dsce_dpf_ctrl_0_reg;
    dsc_enc_clock_st *dsc_enc_clock = ScalerHdmiTx0_H5xDsc_getDscTimingInfo(format_idx);
    dsce_dsc_enc_ctrl_reg.regValue = rtd_inl(DSCE_DSC_ENC_CTRL_reg);
    dsce_dpf_ctrl_0_reg.regValue = rtd_inl(DSCE_DPF_CTRL_0_reg);
    if(dsc_enc_clock == NULL){
        DebugMessageHDMITx("[HDMITX] %s, dsc_enc_clock = null\n",__FUNCTION__);
        dsce_dsc_enc_ctrl_reg.dsc_enc_en = 0; // 0: DSC disabled
    }else{
        // DSC clock control
        dsce_dsc_clk_ctrl_reg.regValue = rtd_inl(DSCE_DSC_CLK_CTRL_reg);
        dsce_dsc_clk_ctrl_reg.dsc_src_div = dsc_enc_clock->dsc_src_div; // =log2(dsc_enc_clk_p/ dsc_enc_clk_c): 0: div1, 1: div2, 2: div4, 3: div8
        rtd_outl(DSCE_DSC_CLK_CTRL_reg, dsce_dsc_clk_ctrl_reg.regValue);

        // DSC ENC config
        dsce_dsc_enc_ctrl_reg.dsc_enc_en = 1; // 1: when DSC is enabled
        dsce_dsc_enc_ctrl_reg.pixel_mode = dsc_enc_clock->pixel_mode; // 0: 1 pixel mode, 1: 2 pixel mode, 2: 4 pixel mode, 3: 8 pixel mode
        dsce_dpf_ctrl_0_reg.disp_en = 1;
    }
    rtd_outl(DSCE_DSC_ENC_CTRL_reg, dsce_dsc_enc_ctrl_reg.regValue);
    rtd_outl(DSCE_DPF_CTRL_0_reg, dsce_dpf_ctrl_0_reg.regValue);//HY suggest disp_en enable after dsc_enc_en
    HDMITX_DTG_Wait_Den_Stop_Done(); //Scaler_Wait_for_event(TXSOCTG_TXSOC_pending_status_reg, TXSOCTG_TXSOC_pending_status_mv_den_end_event_mask,1);

    //rtd_maskl(DSCE_MISC_DSC_TIMING_GEN_FRAME_RATE_reg, ~_BIT31,_BIT31);
    return;
}


void ScalerHdmiTx0_H5xDsc_encReset(void)
{
    UINT32 crt_clken6 = rtd_inl(SYS_REG_SYS_CLKEN6_reg);
    UINT8 bDscEn = (rtd_inl(DSCE_DSC_ENC_CTRL_reg) & DSCE_DSC_ENC_CTRL_dsc_enc_en_mask)|| (rtd_inl(DSCE_DPF_CTRL_0_reg) & DSCE_DPF_CTRL_0_disp_en_mask);

    // DSCE clock enable if need
    if((crt_clken6 & SYS_REG_SYS_CLKEN6_clken_dsce_mask) == 0){
        rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_dsce_mask|SYS_REG_SYS_CLKEN6_write_data_mask);
        NoteMessageHDMITx("[HDMITX][DSCE] Enable DSCE CLK %x->%x\n", crt_clken6, rtd_inl(SYS_REG_SYS_CLKEN6_reg));
    } // DSCE reset
    else{
        // 1. disable DSC enc & disp enable
        rtd_maskl(DSCE_DSC_ENC_CTRL_reg, ~DSCE_DSC_ENC_CTRL_dsc_enc_en_mask, 0);
        rtd_maskl(DSCE_DPF_CTRL_0_reg, ~DSCE_DPF_CTRL_0_disp_en_mask, 0);

        if(bDscEn){ // 2. wait porch if DSC enc is enabled
            if(Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_HDMITX_DSC_SRC) == DSC_SRC_TXSOC)
                HDMITX_DTG_Wait_Den_Stop_Done(); // Scaler_Wait_for_event(TXSOCTG_TXSOC_pending_status_reg, TXSOCTG_TXSOC_pending_status_mv_den_end_event_mask,1);
            else // DSC input from DSC_SRC_DISPD
                HDMITX_DTG_Wait_Den_Stop_Done();//Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,1);
        }

        // 3. RSTN=0
        rtd_outl(SYS_REG_SYS_SRST4_reg, SYS_REG_SYS_SRST6_rstn_dsce_mask); // B[15] dsce, B[14]: hdmitx, B[13]: hdmitx_phy, B[12]: hdmitx_off

        // 4. RSTN=1
        rtd_outl(SYS_REG_SYS_SRST4_reg, SYS_REG_SYS_SRST6_rstn_dsce_mask|_BIT0); // B[15] dsce, B[14]: hdmitx, B[13]: hdmitx_phy, B[12]: hdmitx_off
    }

    udelay(150);

    return;
}

void ScalerHdmi21MacTx0DscTimingGenSetting(void)
{
    UINT8 timingDiv4 = 2;
    dsce_misc_dsc_timing_gen_ctrl_RBUS dsce_misc_dsc_timing_gen_ctrl_reg;
    dsce_misc_dsc_timing_gen_sta_RBUS dsce_misc_dsc_timing_gen_sta_reg;
    dsce_misc_dsc_timing_gen_size_RBUS dsce_misc_dsc_timing_gen_size_reg;
    dsce_misc_dsc_timing_gen_end_RBUS dsce_misc_dsc_timing_gen_end_reg;
    dsce_misc_dsc_timing_gen_hsync_RBUS dsce_misc_dsc_timing_gen_hsync_reg;
    dsce_misc_dsc_pattern_gen_ctrl_0_RBUS dsce_misc_dsc_pattern_gen_ctrl_0_reg;

    dsce_misc_dsc_pattern_gen_c0_rg_RBUS dsce_misc_dsc_pattern_gen_c0_rg_reg;
    dsce_misc_dsc_pattern_gen_c0_b_RBUS dsce_misc_dsc_pattern_gen_c0_b_reg;
    dsce_misc_dsc_pattern_gen_c1_rg_RBUS dsce_misc_dsc_pattern_gen_c1_rg_reg;
    dsce_misc_dsc_pattern_gen_c1_b_RBUS dsce_misc_dsc_pattern_gen_c1_b_reg;
    dsce_misc_dsc_pattern_gen_c2_rg_RBUS dsce_misc_dsc_pattern_gen_c2_rg_reg;
    dsce_misc_dsc_pattern_gen_c2_b_RBUS dsce_misc_dsc_pattern_gen_c2_b_reg;
    dsce_misc_dsc_pattern_gen_c3_rg_RBUS dsce_misc_dsc_pattern_gen_c3_rg_reg;
    dsce_misc_dsc_pattern_gen_c3_b_RBUS dsce_misc_dsc_pattern_gen_c3_b_reg;

    dsce_misc_dsc_timing_gen_ctrl_reg.regValue = rtd_inl(DSCE_MISC_DSC_TIMING_GEN_CTRL_reg);
    dsce_misc_dsc_timing_gen_sta_reg.regValue = rtd_inl(DSCE_MISC_DSC_TIMING_GEN_STA_reg);
    dsce_misc_dsc_timing_gen_sta_reg.self_tg_x_sta = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart/timingDiv4;
    dsce_misc_dsc_timing_gen_sta_reg.self_tg_y_sta = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVStart;
    rtd_outl(DSCE_MISC_DSC_TIMING_GEN_STA_reg, dsce_misc_dsc_timing_gen_sta_reg.regValue);
    dsce_misc_dsc_timing_gen_size_reg.regValue = rtd_inl(DSCE_MISC_DSC_TIMING_GEN_SIZE_reg);
    dsce_misc_dsc_timing_gen_size_reg.self_tg_x_size = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth/timingDiv4;
    dsce_misc_dsc_timing_gen_size_reg.self_tg_y_size = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight;
    rtd_outl(DSCE_MISC_DSC_TIMING_GEN_SIZE_reg, dsce_misc_dsc_timing_gen_size_reg.regValue);
    dsce_misc_dsc_timing_gen_end_reg.regValue = rtd_inl(DSCE_MISC_DSC_TIMING_GEN_END_reg);
    dsce_misc_dsc_timing_gen_end_reg.self_tg_x_end = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth))/timingDiv4;
    dsce_misc_dsc_timing_gen_end_reg.self_tg_y_end = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVTotal - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVStart - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight));
    rtd_outl(DSCE_MISC_DSC_TIMING_GEN_END_reg, dsce_misc_dsc_timing_gen_end_reg.regValue);
    dsce_misc_dsc_timing_gen_hsync_reg.regValue = rtd_inl(DSCE_MISC_DSC_TIMING_GEN_HSYNC_reg);
    dsce_misc_dsc_timing_gen_hsync_reg.self_tg_hs_end = (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth)/timingDiv4;
    dsce_misc_dsc_timing_gen_hsync_reg.self_tg_hs_sta = 0;
    rtd_outl(DSCE_MISC_DSC_TIMING_GEN_HSYNC_reg, dsce_misc_dsc_timing_gen_hsync_reg.regValue);

    dsce_misc_dsc_pattern_gen_c0_rg_reg.regValue = 0;
    dsce_misc_dsc_pattern_gen_c0_rg_reg.patgen_c0_r = 0xfff;
    rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C0_RG_reg, dsce_misc_dsc_pattern_gen_c0_rg_reg.regValue);
    dsce_misc_dsc_pattern_gen_c0_b_reg.regValue = 0;
    rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C0_B_reg, dsce_misc_dsc_pattern_gen_c0_b_reg.regValue);

    dsce_misc_dsc_pattern_gen_c1_rg_reg.regValue = 0;
    dsce_misc_dsc_pattern_gen_c1_rg_reg.patgen_c1_g = 0xfff;
    rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C1_RG_reg, dsce_misc_dsc_pattern_gen_c1_rg_reg.regValue);
    dsce_misc_dsc_pattern_gen_c1_b_reg.regValue = 0;
    rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C1_B_reg, dsce_misc_dsc_pattern_gen_c1_b_reg.regValue);

    dsce_misc_dsc_pattern_gen_c2_rg_reg.regValue = 0;
    rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C2_RG_reg, dsce_misc_dsc_pattern_gen_c2_rg_reg.regValue);
    dsce_misc_dsc_pattern_gen_c2_b_reg.regValue = 0xfff;
    rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C2_B_reg, dsce_misc_dsc_pattern_gen_c2_b_reg.regValue);

    dsce_misc_dsc_pattern_gen_c3_rg_reg.regValue = 0x00ffffff;
    rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C3_RG_reg, dsce_misc_dsc_pattern_gen_c3_rg_reg.regValue);
    dsce_misc_dsc_pattern_gen_c3_b_reg.regValue = 0xfff;
    rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_C3_B_reg, dsce_misc_dsc_pattern_gen_c3_b_reg.regValue);

    dsce_misc_dsc_pattern_gen_ctrl_0_reg.regValue = rtd_inl(DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_reg);
    dsce_misc_dsc_pattern_gen_ctrl_0_reg.patgen_width = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth/8 -1;
    dsce_misc_dsc_pattern_gen_ctrl_0_reg.patgen_en = 1;
    rtd_outl(DSCE_MISC_DSC_PATTERN_GEN_CTRL_0_reg, dsce_misc_dsc_pattern_gen_ctrl_0_reg.regValue);

    dsce_misc_dsc_timing_gen_ctrl_reg.self_tg_en = 1;
    rtd_outl(DSCE_MISC_DSC_TIMING_GEN_CTRL_reg, dsce_misc_dsc_timing_gen_ctrl_reg.regValue);

}
