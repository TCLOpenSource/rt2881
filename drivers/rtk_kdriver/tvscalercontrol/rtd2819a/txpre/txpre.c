/*============================================================================
* Copyright (c)      Realtek Semiconductor Corporation, 2024
* All rights reserved.
* ===========================================================================*/

//----------------------------------------------------------------------------
// ID Code      : txpre.c No.0000
// Update Note  :
//----------------------------------------------------------------------------

#include <scaler/scalerCommon.h>

#include <rbus/hdmitx_uzd_reg.h>
#include <rbus/hdmitx_vgip_reg.h>
#include <rbus/sys_reg_reg.h>

#include <rtd_log/rtd_module_log.h>

#include <tvscalercontrol/hdmirx/hdmifun.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/txpre/txpre.h>
#include <tvscalercontrol/txpre/txpre_mode.h>
#include <tvscalercontrol/vip/ultrazoom.h>
#include <tvscalercontrol/txpre/txpre_display.h>
#include <tvscalercontrol/txpre/txpre_memory.h>


//****************************************************************************
// type define
//
//****************************************************************************


//****************************************************************************
// CODE TABLES
//****************************************************************************


//****************************************************************************
// local variable declaration
//****************************************************************************

//****************************************************************************
// extern variable declaration
//****************************************************************************

extern void HDMITX_ultrazoom_config_scaling_down(SIZE* in_Size, SIZE* out_Size, unsigned char panorama);

//****************************************************************************
// Implementation function define
//****************************************************************************

void txpre_datapath_clock_ctrl(unsigned char bflag)
{
    if(bflag) { // enable
        // crt clock
        IoReg_Write32(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_hdmitx_mask|SYS_REG_SYS_CLKEN6_write_data_mask);
        IoReg_Write32(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_hdmitx_mask|SYS_REG_SYS_SRST6_write_data_mask);

        // src clock
        IoReg_SetBits(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg, HDMITX_VGIP_HDMITX_VGIP_CTRL_hdmitx_in_clk_en_mask);
    } else {    // disable
        // src clock
        IoReg_ClearBits(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg, HDMITX_VGIP_HDMITX_VGIP_CTRL_hdmitx_in_clk_en_mask);

        // crt clock
        IoReg_Write32(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_hdmitx_mask);
        IoReg_Write32(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_hdmitx_mask);
    }
}

void txpre_set_vgip(TXPRE_VGIP_SRC_TYPE src_type, StructTxPreInfo * txpre_info)
{
    hdmitx_vgip_hdmitx_vgip_ctrl_RBUS hdmitx_vgip_ctrl_reg;
    hdmitx_vgip_hdmitx_vgip_act_hsta_width_RBUS hdmitx_vgip_act_hsta_width_reg;
    hdmitx_vgip_hdmitx_vgip_act_vsta_length_RBUS hdmitx_vgip_act_vsta_length_reg;
    hdmitx_vgip_hdmitx_vgip_delay_RBUS hdmitx_vgip_delay_reg;
    hdmitx_vgip_hdmitx_vgip_htotal_RBUS hdmitx_vgip_htotal_reg;

    // disable src clock
    hdmitx_vgip_ctrl_reg.regValue = IoReg_Read32(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg);
    hdmitx_vgip_ctrl_reg.hdmitx_in_clk_en = 0;
    IoReg_Write32(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg, hdmitx_vgip_ctrl_reg.regValue);

    hdmitx_vgip_ctrl_reg.regValue = IoReg_Read32(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg);
    hdmitx_vgip_ctrl_reg.hdmitx_in_sel = src_type;
    hdmitx_vgip_ctrl_reg.hdmitx_vs_inv = txpre_info->Polarity >> 1 ^ 1;
    hdmitx_vgip_ctrl_reg.hdmitx_vs_by_hs_en_n = txpre_info->Interlace ? 0 : 1;
    IoReg_Write32(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg, hdmitx_vgip_ctrl_reg.regValue);

    // enable src clock
    hdmitx_vgip_ctrl_reg.regValue = IoReg_Read32(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg);
    hdmitx_vgip_ctrl_reg.hdmitx_in_clk_en = 1;
    IoReg_Write32(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg, hdmitx_vgip_ctrl_reg.regValue);

    hdmitx_vgip_ctrl_reg.regValue = IoReg_Read32(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg);
    hdmitx_vgip_ctrl_reg.hdmitx_ivrun = 1;
    IoReg_Write32(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg, hdmitx_vgip_ctrl_reg.regValue);

    hdmitx_vgip_act_hsta_width_reg.regValue = IoReg_Read32(HDMITX_VGIP_HDMITX_VGIP_ACT_HSTA_Width_reg);
    hdmitx_vgip_act_hsta_width_reg.hdmitx_ih_act_sta = 0;
    hdmitx_vgip_act_hsta_width_reg.hdmitx_ih_act_wid = txpre_info->IPH_ACT_WID;
    IoReg_Write32(HDMITX_VGIP_HDMITX_VGIP_ACT_HSTA_Width_reg, hdmitx_vgip_act_hsta_width_reg.regValue);

    hdmitx_vgip_act_vsta_length_reg.regValue = IoReg_Read32(HDMITX_VGIP_HDMITX_VGIP_ACT_VSTA_Length_reg);
    hdmitx_vgip_act_vsta_length_reg.hdmitx_iv_act_sta = 0;
    hdmitx_vgip_act_vsta_length_reg.hdmitx_iv_act_len = txpre_info->IPV_ACT_LEN;
    IoReg_Write32(HDMITX_VGIP_HDMITX_VGIP_ACT_VSTA_Length_reg, hdmitx_vgip_act_vsta_length_reg.regValue);

    hdmitx_vgip_delay_reg.regValue = IoReg_Read32(HDMITX_VGIP_HDMITX_VGIP_DELAY_reg);
    hdmitx_vgip_delay_reg.hdmitx_ivs_dly = 0;
    IoReg_Write32(HDMITX_VGIP_HDMITX_VGIP_DELAY_reg, hdmitx_vgip_delay_reg.regValue);

    hdmitx_vgip_htotal_reg.regValue = IoReg_Read32(HDMITX_VGIP_HDMITX_VGIP_HTOTAL_reg);
    hdmitx_vgip_htotal_reg.hdmitx_htotal_num = txpre_info->IHTotal - 1;
    IoReg_Write32(HDMITX_VGIP_HDMITX_VGIP_HTOTAL_reg, hdmitx_vgip_htotal_reg.regValue);
}

void txpre_set_sdnr(StructTxPreInfo * txpre_info)
{
    hdmitx_uzd_hdmitx_uzd_cutout_range_hor_RBUS hdmitx_uzd_cutout_range_hor_reg;
    hdmitx_uzd_hdmitx_uzd_cutout_range_ver_RBUS hdmitx_uzd_cutout_range_ver_reg;
    hdmitx_uzd_hdmitx_uzd_ctrl0_RBUS hdmitx_uzd_ctrl0_reg;

    hdmitx_uzd_cutout_range_hor_reg.regValue = IoReg_Read32(HDMITX_UZD_HDMITX_UZD_CUTOUT_RANGE_HOR_reg);
    hdmitx_uzd_cutout_range_hor_reg.hor_front = txpre_info->Crop_H_STA;
    hdmitx_uzd_cutout_range_hor_reg.hor_width = txpre_info->Crop_WID;
    IoReg_Write32(HDMITX_UZD_HDMITX_UZD_CUTOUT_RANGE_HOR_reg, hdmitx_uzd_cutout_range_hor_reg.regValue);

    hdmitx_uzd_cutout_range_ver_reg.regValue = IoReg_Read32(HDMITX_UZD_HDMITX_UZD_CUTOUT_RANGE_VER_reg);
    hdmitx_uzd_cutout_range_ver_reg.ver_front = txpre_info->Crop_V_STA;
    hdmitx_uzd_cutout_range_ver_reg.ver_height = txpre_info->Crop_LEN;
    IoReg_Write32(HDMITX_UZD_HDMITX_UZD_CUTOUT_RANGE_VER_reg, hdmitx_uzd_cutout_range_ver_reg.regValue);

    hdmitx_uzd_ctrl0_reg.regValue = IoReg_Read32(HDMITX_UZD_HDMITX_UZD_CTRL0_reg);
    hdmitx_uzd_ctrl0_reg.cutout_en = 1;
    IoReg_Write32(HDMITX_UZD_HDMITX_UZD_CTRL0_reg, hdmitx_uzd_ctrl0_reg.regValue);
}

void txpre_set_uzd(StructTxPreInfo * txpre_info)
{
    SIZE insize, outsize;

    if(txpre_info->Interlace)
        return; // bypass interlace mode

    insize.nWidth   = txpre_info->IPH_ACT_WID;
    insize.nLength  = txpre_info->IPV_ACT_LEN;

    outsize.nWidth  = txpre_info->Crop_WID;
    outsize.nLength = txpre_info->Crop_LEN;

    rtd_pr_hdmitx_info("[txpre]insize.nWidth=%d, insize.nLength=%d\n", insize.nWidth, insize.nLength);
    rtd_pr_hdmitx_info("[txpre]outsize.nWidth=%d, outsize.nLength=%d\n", outsize.nWidth, outsize.nLength);

    HDMITX_ultrazoom_config_scaling_down(&insize, &outsize, 0);
}

unsigned char txpre_config(StructTxPreInfo * txpre_info)
{
    TXPRE_VGIP_SRC_TYPE vgip_src_type;

    if(txpre_info == NULL) {
        rtd_pr_hdmitx_err("[txpre][%s] return false due to txpre_info is null.\n", __func__);
        return false;
    }

    vgip_src_type = (TXPRE_VGIP_SRC_TYPE) txpre_info->Input_Src;
    txpre_set_vgip(vgip_src_type, txpre_info);

    if(txpre_info->Input_Src == TXPRE_VGIP_SRC_HDMI) {
        txpre_onms5_enable(txpre_info);
        txpre_onms5_setting(txpre_info, false, false);
    } else {
        txpre_onms5_setting(txpre_info, false, false);
        txpre_onms5_disable();
    }

    txpre_set_sdnr(txpre_info);

    txpre_set_uzd(txpre_info);

    if((txpre_info->Input_Src == TXPRE_VGIP_SRC_HDMI) || txpre_info->isVRR){
        txpre_memory_fs_handler(txpre_info);
    } else {
        txpre_memory_frc_handler(txpre_info);
    }

    Txpretg_set_disp_setting(txpre_info);

    if(txpre_info->isVRR)
        Txpretg_set_VRR_timing_framesync(txpre_info);
    else if(txpre_info->Input_Src == TXPRE_VGIP_SRC_HDMI)
        Txpretg_set_line_tracking_timing_freerun(txpre_info);

    return true;
}
