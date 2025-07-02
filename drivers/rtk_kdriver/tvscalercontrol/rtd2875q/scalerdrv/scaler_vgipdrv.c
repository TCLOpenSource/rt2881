#include <rbus/vgip_reg.h>
// #include <rbus/sub_vgip_reg.h>
#include <rbus/dma_vgip_reg.h>
#include <tvscalercontrol/io/ioregdrv.h>

void drvif_scaler_vgip_set_dma_2pxl_en(unsigned char bflag)
{
    // dma_vgip_dma_data_path_select_RBUS dma_vgip_dma_data_path_select_reg;

    // dma_vgip_dma_data_path_select_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_Data_Path_Select_reg);
    // dma_vgip_dma_data_path_select_reg.dma_2pxl_en = bflag;
    // IoReg_Write32(DMA_VGIP_DMA_Data_Path_Select_reg, dma_vgip_dma_data_path_select_reg.regValue);
}

void drvif_scaler_vgip_set_uzd2_in_sel(unsigned char bflag)
{
    vgip_data_path_select_sub_RBUS data_path_select_sub_reg;

    data_path_select_sub_reg.regValue = IoReg_Read32(VGIP_Data_Path_Select_sub_reg);
    data_path_select_sub_reg.uzd2_in_sel = bflag;
    IoReg_Write32(VGIP_Data_Path_Select_sub_reg, data_path_select_sub_reg.regValue);
}

void drvif_scaler_vgip_set_s2_clk_en(unsigned char bflag)
{
    vgip_data_path_select_sub_RBUS data_path_select_sub_reg;

    data_path_select_sub_reg.regValue = IoReg_Read32(VGIP_Data_Path_Select_sub_reg);
    data_path_select_sub_reg.s2_clk_en = bflag;
    IoReg_Write32(VGIP_Data_Path_Select_sub_reg, data_path_select_sub_reg.regValue);
}

void drvif_scaler_vgip_set_main_imd_smooth_toggle_apply(unsigned char bflag)
{
    vgip_smooth_tog_ctrl_RBUS vgip_smooth_tog_ctrl_reg;

    vgip_smooth_tog_ctrl_reg.regValue = IoReg_Read32(VGIP_Smooth_tog_ctrl_reg);
    vgip_smooth_tog_ctrl_reg.main_imd_smooth_toggle_apply = bflag; // global main smooth toggle
    IoReg_Write32(VGIP_Smooth_tog_ctrl_reg,vgip_smooth_tog_ctrl_reg.regValue);
}

void drvif_scaler_vgip_set_sub_imd_smooth_toggle_apply(unsigned char bflag)
{
    vgip_smooth_tog_ctrl_sub_RBUS vgip_smooth_tog_ctrl_sub_reg;

    vgip_smooth_tog_ctrl_sub_reg.regValue = IoReg_Read32(VGIP_Smooth_tog_ctrl_sub_reg);
    vgip_smooth_tog_ctrl_sub_reg.sub_imd_smooth_toggle_apply = bflag; // global sub smooth toggle
    IoReg_Write32(VGIP_Smooth_tog_ctrl_sub_reg,vgip_smooth_tog_ctrl_sub_reg.regValue);
}

void drvif_scaler_vgip_set_sub_imd_smooth_toggle_style(unsigned char bflag)
{
    vgip_smooth_tog_ctrl_sub_RBUS vgip_smooth_tog_ctrl_sub_reg;

    vgip_smooth_tog_ctrl_sub_reg.regValue = IoReg_Read32(VGIP_Smooth_tog_ctrl_sub_reg);
    vgip_smooth_tog_ctrl_sub_reg.sub_imd_smooth_toggle_style = bflag; // global sub smooth toggle
    IoReg_Write32(VGIP_Smooth_tog_ctrl_sub_reg,vgip_smooth_tog_ctrl_sub_reg.regValue);
}


void drvif_scaler_vgip_set_sub_imd_smooth_toggle_clk_time2switch_en(unsigned char bflag)
{
    vgip_smooth_tog_ctrl_sub_RBUS vgip_smooth_tog_ctrl_sub_reg;

    vgip_smooth_tog_ctrl_sub_reg.regValue = IoReg_Read32(VGIP_Smooth_tog_ctrl_sub_reg);
    vgip_smooth_tog_ctrl_sub_reg.sub_imd_smooth_toggle_clk_time2switch_en = bflag; // global sub smooth toggle
    IoReg_Write32(VGIP_Smooth_tog_ctrl_sub_reg,vgip_smooth_tog_ctrl_sub_reg.regValue);
}

void drvif_scaler_vgip_set_cha_feild_inv_en(unsigned char bflag)
{
	vgip_vgip_chn1_ctrl_RBUS vgip_chn1_ctrl_reg;
	
	vgip_chn1_ctrl_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_CTRL_reg);
	vgip_chn1_ctrl_reg.ch1_field_inv = bflag;
	IoReg_Write32(VGIP_VGIP_CHN1_CTRL_reg, vgip_chn1_ctrl_reg.regValue);
}

