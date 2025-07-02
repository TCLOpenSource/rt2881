#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/delay.h>

#include <rbus/dsce2_misc_reg.h>
#include <rbus/dsce2_reg.h>
#include <rbus/sfg_reg.h>
#include <rbus/sys_reg_reg.h>

#include <tvscalercontrol/dsce/dsce2.h>

#include <tvscalercontrol/io/ioregdrv.h>

void dsce2_ppsConfig(DSCE2_CONFIG_PARAMETER *dsce2_config)
{
    int i =0;
    unsigned char* dsc_pps = NULL;

    if(dsce2_config == NULL) {
        DSCE2_PRINT_DEBUG("dsce2_config NULL\n");
        return;
    }

    if(dsce2_config->dsce2_pps == NULL) {
        DSCE2_PRINT_DEBUG("dsce2_pps NULL\n");
        return;
    }
    
    dsc_pps = dsce2_config->dsce2_pps;
    

    for(i =0; i <94 ;i++)//DSC_PPS00~DSC_PPS93
    {
        IoReg_Write32(DSCE2_DSC_PPS_00_reg + i*4 ,dsc_pps[0]);

        //DebugMessageHDMITx("[HDMITX] %s, 0x%x = %x\n", __FUNCTION__,DSCE2_DSC_PPS_00_reg + i*4,IoReg_Read32(DSCE2_DSC_PPS_00_reg + + i*4));
    }

    return;
}



unsigned int dsc2_clk_ratio(int reg_data)
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

void dsce2_dpfConfig(DSCE2_CONFIG_PARAMETER *dsce2_config)
{
//#ifdef _MARK2_FIXME_DSCE_COMPILER_ERROR // [MARK2] FIX-ME -- Compiler error when calculating certain values
    //Scaler_TestCase_config()
    int width, height, ipt_hor_porch;
    int val, msb, lsb;
    int dsc_str_width;
    int dsc_str_htt;
    int dsc_str_hsw;
    int dsc_enc_frame_sync;
    int full_last_line = 1;
    int slice_cnt;
    unsigned int opt_stream_period;
    int dpf_hst;
    int dpf_front_porch;
    unsigned int dsc_enc_clk_s, dsc_enc_clk_x,dsc_enc_clk_p,dsc_enc_clk_c;
    //pps
    int native420 ;
    int native422 ;
    int initialDelay;
    int bitsPerPixel;
    int dsc_version_minor;
    int bits_per_component;
    int slice_width;
    int initial_xmit_delay;
    //pps
    //ref dvScript_txsoc_config();
    //Ten-Yao suggest use MNT setting (3T) to set DPF hsync width
    int   dpf_hsw; // Horizontal Sync Pulse Count (unit: SyncProc Clock)
    int   dpf_vtt;// Vertical Total length (unit: HSync)
    //  ScalerHdmi21MacTx0InputConverter(); Vertical Sync +  Vertical back porch
    int   dpf_vst;// = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVStart;
    int   dpf_vsw;// Vertical Sync Width    (unit: HSync)

    dsce2_dsc_enc_ctrl_RBUS dsce2_dsc_enc_ctrl_reg;
    dsce2_dsc_output_ctrl_RBUS dsce2_dsc_output_ctrl_reg;
    dsce2_mn_dpf_htt_m_RBUS dsce2_mn_dpf_htt_m_reg;
    dsce2_mn_dpf_htt_l_RBUS dsce2_mn_dpf_htt_l_reg;
    dsce2_mn_dpf_hst_m_RBUS dsce2_mn_dpf_hst_m_reg;
    dsce2_mn_dpf_hst_l_RBUS dsce2_mn_dpf_hst_l_reg;
    dsce2_mn_dpf_hwd_m_RBUS dsce2_mn_dpf_hwd_m_reg;
    dsce2_mn_dpf_hwd_l_RBUS dsce2_mn_dpf_hwd_l_reg;
    dsce2_mn_dpf_hsw_m_RBUS dsce2_mn_dpf_hsw_m_reg;
    dsce2_mn_dpf_hsw_l_RBUS dsce2_mn_dpf_hsw_l_reg;
    dsce2_mn_dpf_vtt_m_RBUS dsce2_mn_dpf_vtt_m_reg;
    dsce2_mn_dpf_vtt_l_RBUS dsce2_mn_dpf_vtt_l_reg;
    dsce2_mn_dpf_vst_m_RBUS dsce2_mn_dpf_vst_m_reg;
    dsce2_mn_dpf_vst_l_RBUS dsce2_mn_dpf_vst_l_reg;
    dsce2_mn_dpf_vht_m_RBUS dsce2_mn_dpf_vht_m_reg;
    dsce2_mn_dpf_vht_l_RBUS dsce2_mn_dpf_vht_l_reg;
    dsce2_mn_dpf_vsw_m_RBUS dsce2_mn_dpf_vsw_m_reg;
    dsce2_mn_dpf_vsw_l_RBUS dsce2_mn_dpf_vsw_l_reg;
    dsce2_evblk2vs_h_RBUS dsce2_evblk2vs_h_reg;
    dsce2_evblk2vs_m_RBUS dsce2_evblk2vs_m_reg;
    dsce2_evblk2vs_l_RBUS dsce2_evblk2vs_l_reg;
    dsce2_dp_output_ctrl_RBUS dsce2_dp_output_ctrl_reg;
    dsce2_dpf_ctrl_0_RBUS dsce2_dpf_ctrl_0_reg;
    unsigned char* dsc_pps;

    if(dsce2_config == NULL) {
        DSCE2_PRINT_DEBUG("dsce2_config NULL\n");
        return;
    }

    dpf_hsw = dsce2_config->dsce2_hsync;
    dpf_vtt = dsce2_config->dsce2_vtotal;// Vertical Total length (unit: HSync)
    dpf_vsw = dsce2_config->dsce2_vsync; // Vertical Sync Width    (unit: HSync)
    width = dsce2_config->dsce2_hactive;
    height = dsce2_config->dsce2_vactive;
    ipt_hor_porch = dsce2_config->dsce2_htotal - dsce2_config->dsce2_hactive;
    dpf_vst = dsce2_config->dsce2_vsync + dsce2_config->dsce2_vback;

    if(dsce2_config->dsce2_pps == NULL) {
        DSCE2_PRINT_DEBUG("dsce2_pps NULL\n");
        return;
    }
    
    dsc_pps = dsce2_config->dsce2_pps;

    //pps need to read value
    native420 = DSCE2_DSC_PPS_88_get_native_420(dsc_pps[88]);
    native422 = DSCE2_DSC_PPS_88_get_native_422(dsc_pps[88]);
    bitsPerPixel = (DSCE2_DSC_PPS_04_get_bits_per_pixel_9_8(dsc_pps[4]) <<8)
                        | DSCE2_DSC_PPS_05_bits_per_pixel_7_0(dsc_pps[5]);
    dsc_version_minor = DSCE2_DSC_PPS_00_get_dsc_version_minor(dsc_pps[0]);
    bits_per_component = DSCE2_DSC_PPS_03_get_bits_per_component(dsc_pps[3]);
    slice_width = (DSCE2_DSC_PPS_12_get_slice_width_15_8(dsc_pps[12]) <<8)
                        | DSCE2_DSC_PPS_13_get_slice_width_7_0(dsc_pps[13]);
    initial_xmit_delay = (DSCE2_DSC_PPS_16_get_initial_xmit_delay_9_8(dsc_pps[16]) <<8)
                        | DSCE2_DSC_PPS_17_get_initial_xmit_delay_7_0(dsc_pps[17]);
    //pps

    dsc_enc_clk_s = dsc2_clk_ratio(SFG_SFG_CLKEN_CTRL3_get_clkratio_dsce_s(IoReg_Read32(SFG_SFG_CLKEN_CTRL3_reg)));
    dsc_enc_clk_x = dsc2_clk_ratio(SFG_SFG_CLKEN_CTRL3_get_clkratio_dsce_x(IoReg_Read32(SFG_SFG_CLKEN_CTRL3_reg)));
    dsc_enc_clk_p = dsc2_clk_ratio(SFG_SFG_CLKEN_CTRL3_get_clkratio_dsce_p(IoReg_Read32(SFG_SFG_CLKEN_CTRL3_reg)));
    dsc_enc_clk_c = dsc2_clk_ratio(SFG_SFG_CLKEN_CTRL3_get_clkratio_dsce_c(IoReg_Read32(SFG_SFG_CLKEN_CTRL3_reg)));


    slice_cnt = (width + slice_width-1) / slice_width;

	//slice one line bits count
    dsc_str_width = bitsPerPixel*(slice_width/(1+native422)/(1+native420))/16;
    dsc_str_width = (dsc_str_width + 63)/64;

    DSCE2_PRINT_DEBUG("[HDMITX] dsc_str_width = %d, slice_cnt = %d, slice_width = %d, ipt_hor_porch = %d\n",dsc_str_width,slice_cnt,slice_width, ipt_hor_porch);
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


    DSCE2_PRINT_DEBUG("[HDMITX] dsc_str_htt= %d, dsc_str_hsw = %d, opt_stream_period = %d, dpf_front_porch = %d\n",dsc_str_htt,dsc_str_hsw,opt_stream_period,dpf_front_porch);
    dpf_hst =  dsc_str_htt - 11 - (1 + dpf_front_porch);

    dsc_str_width = (1 + dpf_front_porch);

    if(slice_cnt <= 4){
      dsce2_dsc_enc_ctrl_reg.regValue = IoReg_Read32(DSCE2_DSC_ENC_CTRL_reg);
      dsce2_dsc_enc_ctrl_reg.merge_buf_en = 1; //
      IoReg_Write32(DSCE2_DSC_ENC_CTRL_reg, dsce2_dsc_enc_ctrl_reg.regValue);
    }
    else
    {
        if(slice_width>1280)
        {
            dsce2_dsc_enc_ctrl_reg.regValue = IoReg_Read32(DSCE2_DSC_ENC_CTRL_reg);
            dsce2_dsc_enc_ctrl_reg.merge_buf_en = 1; //
            IoReg_Write32(DSCE2_DSC_ENC_CTRL_reg, dsce2_dsc_enc_ctrl_reg.regValue);
        }
        else
        {
            dsce2_dsc_enc_ctrl_reg.regValue = IoReg_Read32(DSCE2_DSC_ENC_CTRL_reg);
            dsce2_dsc_enc_ctrl_reg.merge_buf_en = 0; //
            IoReg_Write32(DSCE2_DSC_ENC_CTRL_reg, dsce2_dsc_enc_ctrl_reg.regValue);
        }
    }

    val = dsc_str_htt;
    msb = val/256;
    lsb = val%256;
    dsce2_mn_dpf_htt_m_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_HTT_M_reg);
    dsce2_mn_dpf_htt_m_reg.dpf_htt_15_8 = msb;
    IoReg_Write32(DSCE2_MN_DPF_HTT_M_reg, dsce2_mn_dpf_htt_m_reg.regValue); //HTotal [15:8]

    dsce2_mn_dpf_htt_l_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_HTT_L_reg);
    dsce2_mn_dpf_htt_l_reg.dpf_htt_7_0 = lsb;
    IoReg_Write32(DSCE2_MN_DPF_HTT_L_reg, dsce2_mn_dpf_htt_l_reg.regValue); //HTotal [7:0]

    msb = dpf_hst/256;
    lsb = dpf_hst%256;
    dsce2_mn_dpf_hst_m_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_HST_M_reg);
    dsce2_mn_dpf_hst_m_reg.dpf_hst_15_8 = msb;
    IoReg_Write32(DSCE2_MN_DPF_HST_M_reg, dsce2_mn_dpf_hst_m_reg.regValue); //Horizontal Start [15:8]

    dsce2_mn_dpf_hst_l_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_HST_L_reg);
    dsce2_mn_dpf_hst_l_reg.dpf_hst_7_0 = lsb;
    IoReg_Write32(DSCE2_MN_DPF_HST_L_reg, dsce2_mn_dpf_hst_l_reg.regValue); //Horizontal Start [7:0]

    val = dsc_str_width;
    msb = val/256;
    lsb = val%256;
    dsce2_mn_dpf_hwd_m_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_HWD_M_reg);
    dsce2_mn_dpf_hwd_m_reg.dpf_hwd_15_8 = msb;
    IoReg_Write32(DSCE2_MN_DPF_HWD_M_reg, dsce2_mn_dpf_hwd_m_reg.regValue); //Horizontal Active Width [15:8]

    dsce2_mn_dpf_hwd_l_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_HWD_L_reg);
    dsce2_mn_dpf_hwd_l_reg.dpf_hwd_7_0 = lsb;
    IoReg_Write32(DSCE2_MN_DPF_HWD_L_reg, dsce2_mn_dpf_hwd_l_reg.regValue); //Horizontal Active Width [7:0]

    msb = dsc_str_hsw/256;
    lsb = dsc_str_hsw%256;
    dsce2_mn_dpf_hsw_m_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_HSW_M_reg);
    dsce2_mn_dpf_hsw_m_reg.dpf_hsp = 0;
    dsce2_mn_dpf_hsw_m_reg.dpf_hsw_14_8 = msb;
    IoReg_Write32(DSCE2_MN_DPF_HSW_M_reg, dsce2_mn_dpf_hsw_m_reg.regValue); //Horizontal Sync Pulse [14:8]

    dsce2_mn_dpf_hsw_l_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_HSW_L_reg);
    dsce2_mn_dpf_hsw_l_reg.dpf_hsw_7_0 = lsb;
    IoReg_Write32(DSCE2_MN_DPF_HSW_L_reg, lsb); //Horizontal Sync Pulse [7:0]

    // Vtotal
    msb = dpf_vtt/256;
    lsb = dpf_vtt%256;
    dsce2_mn_dpf_vtt_m_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_VTT_M_reg);
    dsce2_mn_dpf_vtt_m_reg.dpf_vtt_15_8 = msb;
    IoReg_Write32(DSCE2_MN_DPF_VTT_M_reg, dsce2_mn_dpf_vtt_m_reg.regValue); //VTotal [15:8]

    dsce2_mn_dpf_vtt_l_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_VTT_L_reg);
    dsce2_mn_dpf_vtt_l_reg.dpf_vtt_7_0 = lsb;
    IoReg_Write32(DSCE2_MN_DPF_VTT_L_reg, dsce2_mn_dpf_vtt_l_reg.regValue); //VTotal [7:0]

    msb = dpf_vst/256;
    lsb = dpf_vst%256;

    dsce2_mn_dpf_vst_m_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_VST_M_reg);
    dsce2_mn_dpf_vst_m_reg.dpf_vst_15_8 = msb;
    IoReg_Write32(DSCE2_MN_DPF_VST_M_reg, dsce2_mn_dpf_vst_m_reg.regValue); //Vertical Start [15:8]

    dsce2_mn_dpf_vst_l_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_VST_L_reg);
    dsce2_mn_dpf_vst_l_reg.dpf_vst_7_0 = lsb;
    IoReg_Write32(DSCE2_MN_DPF_VST_L_reg, dsce2_mn_dpf_vst_l_reg.regValue); //Vertical Start [7:0]

    val = height;
    msb = val/256;
    lsb = val%256;
    dsce2_mn_dpf_vht_m_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_VHT_M_reg);
    dsce2_mn_dpf_vht_m_reg.dpf_vht_15_8 = msb;
    IoReg_Write32(DSCE2_MN_DPF_VHT_M_reg, dsce2_mn_dpf_vht_m_reg.regValue); //Vertical Active Height [15:8]

    dsce2_mn_dpf_vht_l_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_VHT_L_reg);
    dsce2_mn_dpf_vht_l_reg.dpf_vht_7_0 = lsb;
    IoReg_Write32(DSCE2_MN_DPF_VHT_L_reg, dsce2_mn_dpf_vht_l_reg.regValue); //Vertical Active Height [7:0]


    msb = dpf_vsw/256;
    lsb = dpf_vsw%256;
    dsce2_mn_dpf_vsw_m_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_VSW_M_reg);
    dsce2_mn_dpf_vsw_m_reg.dpf_vsp = 0;
    dsce2_mn_dpf_vsw_m_reg.dpf_vsw_14_8 = msb;
    IoReg_Write32(DSCE2_MN_DPF_VSW_M_reg, dsce2_mn_dpf_vsw_m_reg.regValue); //Vertical Sync Pulse [14:8]

    dsce2_mn_dpf_vsw_l_reg.regValue = IoReg_Read32(DSCE2_MN_DPF_VSW_L_reg);
    dsce2_mn_dpf_vsw_l_reg.dpf_vsw_7_0 = lsb;
    IoReg_Write32(DSCE2_MN_DPF_VSW_L_reg, dsce2_mn_dpf_vsw_l_reg.regValue); //Vertical Sync Pulse [7:0]


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

    dsce2_evblk2vs_h_reg.regValue = IoReg_Read32(DSCE2_EVBLK2VS_H_reg);
    dsce2_evblk2vs_h_reg.evblk2vs_23_16 = 0;
    IoReg_Write32(DSCE2_EVBLK2VS_H_reg, dsce2_evblk2vs_h_reg.regValue); //EVBLK2VS [23:16]

    dsce2_evblk2vs_m_reg.regValue = IoReg_Read32(DSCE2_EVBLK2VS_M_reg);
    dsce2_evblk2vs_m_reg.evblk2vs_15_8 = msb;
    IoReg_Write32(DSCE2_EVBLK2VS_M_reg, dsce2_evblk2vs_m_reg.regValue); //EVBLK2VS [15:8]

    dsce2_evblk2vs_l_reg.regValue = IoReg_Read32(DSCE2_EVBLK2VS_L_reg);
    dsce2_evblk2vs_l_reg.evblk2vs_7_0 = lsb;
    IoReg_Write32(DSCE2_EVBLK2VS_L_reg, dsce2_evblk2vs_l_reg.regValue); //EVBLK2VS [7:0]


    IoReg_Write32(DSCE2_OVBLK2VS_H_reg, 0x00); //OVBLK2VS [23:16]
    IoReg_Write32(DSCE2_OVBLK2VS_M_reg, 0x00); //OVBLK2VS [15:8]
    IoReg_Write32(DSCE2_OVBLK2VS_L_reg, 0x00); //OVBLK2VS [7:0]
    IoReg_Write32(DSCE2_VS_FRONT_PORCH_reg, 0x02); //vs front porch

    dsce2_dp_output_ctrl_reg.regValue = IoReg_Read32(DSCE2_DP_OUTPUT_CTRL_reg);
    dsce2_dp_output_ctrl_reg.vblank_start_gen_en = 0;
    dsce2_dp_output_ctrl_reg.freerun_en = 0;
    IoReg_Write32(DSCE2_DP_OUTPUT_CTRL_reg, dsce2_dp_output_ctrl_reg.regValue); //

    if(full_last_line)
    {
        dsce2_dpf_ctrl_0_reg.regValue = IoReg_Read32(DSCE2_DPF_CTRL_0_reg);
        dsce2_dpf_ctrl_0_reg.disp_en = 0;
        dsce2_dpf_ctrl_0_reg.deonly_full_mode = 0;
        dsce2_dpf_ctrl_0_reg.frame_sync_mode = 0;
        dsce2_dpf_ctrl_0_reg.deonly_mode = 0;
        dsce2_dpf_ctrl_0_reg.vs_bs_or_be = 0;
        dsce2_dpf_ctrl_0_reg.lastline_pixel = 0;
        IoReg_Write32(DSCE2_DPF_CTRL_0_reg, dsce2_dpf_ctrl_0_reg.regValue);
    }
    else if(dsc_enc_frame_sync)
    {
        dsce2_dpf_ctrl_0_reg.regValue = IoReg_Read32(DSCE2_DPF_CTRL_0_reg);
        dsce2_dpf_ctrl_0_reg.disp_en = 0;
        dsce2_dpf_ctrl_0_reg.deonly_full_mode = 0;
        dsce2_dpf_ctrl_0_reg.frame_sync_mode = 1;
        dsce2_dpf_ctrl_0_reg.deonly_mode = 0;
        dsce2_dpf_ctrl_0_reg.vs_bs_or_be = 0;
        dsce2_dpf_ctrl_0_reg.lastline_pixel = 0;
        IoReg_Write32(DSCE2_DPF_CTRL_0_reg, dsce2_dpf_ctrl_0_reg.regValue);
    }
    else
    {
        dsce2_dpf_ctrl_0_reg.regValue = IoReg_Read32(DSCE2_DPF_CTRL_0_reg);
        dsce2_dpf_ctrl_0_reg.disp_en = 0;
        dsce2_dpf_ctrl_0_reg.deonly_full_mode = 0;
        dsce2_dpf_ctrl_0_reg.frame_sync_mode = 1;
        dsce2_dpf_ctrl_0_reg.deonly_mode = 1;
        dsce2_dpf_ctrl_0_reg.vs_bs_or_be = 0;
        dsce2_dpf_ctrl_0_reg.lastline_pixel = 0;
        IoReg_Write32(DSCE2_DPF_CTRL_0_reg, dsce2_dpf_ctrl_0_reg.regValue);
    }

    dsce2_dsc_output_ctrl_reg.regValue = IoReg_Read32(DSCE2_DSC_OUTPUT_CTRL_reg);
    dsce2_dsc_output_ctrl_reg.opt_stream_period = opt_stream_period;

    IoReg_Write32(DSCE2_DSC_OUTPUT_CTRL_reg, dsce2_dsc_output_ctrl_reg.regValue);
//#endif // [MARK2] FIX-ME -- compiler error on some value calculate
    return;
}

void dsce2_inputSrcConfig(void)
{
    dsce2_misc_dsce_src_sel_RBUS dsce2_misc_dsce_src_sel_reg;

    dsce2_misc_dsce_src_sel_reg.regValue = IoReg_Read32(DSCE2_MISC_DSCE_SRC_SEL_reg);

    dsce2_misc_dsce_src_sel_reg.bypass_dsce = 0;
    IoReg_Write32(DSCE2_MISC_DSCE_SRC_SEL_reg, dsce2_misc_dsce_src_sel_reg.regValue);

    return;
}



void dsce2_encClkConfig(dsce2_enc_clock_st *dsce2_enc_clock)
{
    dsce2_dsc_enc_ctrl_RBUS dsce2_dsc_enc_ctrl_reg;
    dsce2_dsc_clk_ctrl_RBUS dsce2_dsc_clk_ctrl_reg;
    dsce2_dpf_ctrl_0_RBUS dsce2_dpf_ctrl_0_reg;
    dsce2_dsc_enc_ctrl_reg.regValue = IoReg_Read32(DSCE2_DSC_ENC_CTRL_reg);
    dsce2_dpf_ctrl_0_reg.regValue = IoReg_Read32(DSCE2_DPF_CTRL_0_reg);
    if(dsce2_enc_clock == NULL){
        DSCE2_PRINT_DEBUG("[HDMITX] %s, dsc_enc_clock = null\n",__FUNCTION__);
        dsce2_dsc_enc_ctrl_reg.dsc_enc_en = 0; // 0: DSC disabled
    }else{
        // DSC clock control
        dsce2_dsc_clk_ctrl_reg.regValue = IoReg_Read32(DSCE2_DSC_CLK_CTRL_reg);
        dsce2_dsc_clk_ctrl_reg.dsc_src_div = dsce2_enc_clock->dsce2_src_div; // =log2(dsc_enc_clk_p/ dsc_enc_clk_c): 0: div1, 1: div2, 2: div4, 3: div8
        IoReg_Write32(DSCE2_DSC_CLK_CTRL_reg, dsce2_dsc_clk_ctrl_reg.regValue);

        // DSC ENC config
        dsce2_dsc_enc_ctrl_reg.dsc_enc_en = 1; // 1: when DSC is enabled
        dsce2_dsc_enc_ctrl_reg.pixel_mode = dsce2_enc_clock->dsce2_pixel_mode; // 0: 1 pixel mode, 1: 2 pixel mode, 2: 4 pixel mode, 3: 8 pixel mode
        dsce2_dpf_ctrl_0_reg.disp_en = 1;
    }
    IoReg_Write32(DSCE2_DSC_ENC_CTRL_reg, dsce2_dsc_enc_ctrl_reg.regValue);
    IoReg_Write32(DSCE2_DPF_CTRL_0_reg, dsce2_dpf_ctrl_0_reg.regValue);//HY suggest disp_en enable after dsc_enc_en
    //DTG_Wait_Den_Stop_Done((unsigned char*)__FUNCTION__, __LINE__); //Scaler_Wait_for_event(TXSOCTG_TXSOC_pending_status_reg, TXSOCTG_TXSOC_pending_status_mv_den_end_event_mask,1);

    //rtd_maskl(DSCE2_MISC_DSC_TIMING_GEN_FRAME_RATE_reg, ~_BIT31,_BIT31);
    return;
}


void dsce2_encReset(void)
{
    unsigned int crt_clken6 = IoReg_Read32(SYS_REG_SYS_CLKEN6_reg);
    unsigned char bDscEn = (IoReg_Read32(DSCE2_DSC_ENC_CTRL_reg) & DSCE2_DSC_ENC_CTRL_dsc_enc_en_mask)|| (IoReg_Read32(DSCE2_DPF_CTRL_0_reg) & DSCE2_DPF_CTRL_0_disp_en_mask);

    // DSCE clock enable if need
    if((crt_clken6 & SYS_REG_SYS_CLKEN6_clken_dsce2_mask) == 0){
        IoReg_Write32(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_dsce2_mask|SYS_REG_SYS_CLKEN6_write_data_mask);
        DSCE2_PRINT_DEBUG("[HDMITX][DSCE] Enable DSCE2 CLK %x->%x\n", crt_clken6, IoReg_Read32(SYS_REG_SYS_CLKEN6_reg));
    } // DSCE reset
    else{
        // 1. disable DSC enc & disp enable
        IoReg_Mask32(DSCE2_DSC_ENC_CTRL_reg, ~DSCE2_DSC_ENC_CTRL_dsc_enc_en_mask, 0);
        IoReg_Mask32(DSCE2_DPF_CTRL_0_reg, ~DSCE2_DPF_CTRL_0_disp_en_mask, 0);

        if(bDscEn){ // 2. wait porch if DSC enc is enabled
                //Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,1);
        }

        // 3. RSTN=0
        IoReg_Write32(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_dsce2_mask); // B[15] dsce, B[14]: hdmitx, B[13]: hdmitx_phy, B[12]: hdmitx_off

        // 4. RSTN=1
        IoReg_Write32(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_dsce2_mask|_BIT0); // B[15] dsce, B[14]: hdmitx, B[13]: hdmitx_phy, B[12]: hdmitx_off
    }

    udelay(150);

    return;
}

void dsce2_driver_Config(DSCE2_CONFIG_PARAMETER *dsce2_config, dsce2_enc_clock_st *dsce2_enc_clock)
{
    if(!dsce2_config || !dsce2_enc_clock)
        return;
    // DSCE2 reset
    dsce2_encReset();

    // DSCE2 source select
    dsce2_inputSrcConfig();

    // DSCE2 PPS config
    dsce2_dpfConfig(dsce2_config);

    dsce2_ppsConfig(dsce2_config);

    // DSCE2 clock config
    dsce2_encClkConfig(dsce2_enc_clock);
    
    return;
}

