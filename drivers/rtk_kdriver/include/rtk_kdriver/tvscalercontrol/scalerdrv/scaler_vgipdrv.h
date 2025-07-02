#ifndef _SCALER_VGIPDRV_H_
#define _SCALER_VGIPDRV_H_

#ifdef __cplusplus
extern "C" {
#endif

void drvif_scaler_vgip_set_dma_2pxl_en(unsigned char bflag);
void drvif_scaler_vgip_set_uzd2_in_sel(unsigned char bflag);
void drvif_scaler_vgip_set_s2_clk_en(unsigned char bflag);
void drvif_scaler_vgip_set_main_imd_smooth_toggle_apply(unsigned char bflag);
void drvif_scaler_vgip_set_sub_imd_smooth_toggle_apply(unsigned char bflag);
void drvif_scaler_vgip_set_sub_imd_smooth_toggle_style(unsigned char bflag);
void drvif_scaler_vgip_set_sub_imd_smooth_toggle_clk_time2switch_en(unsigned char bflag);
void drvif_scaler_vgip_set_cha_feild_inv_en(unsigned char bflag);


#ifdef __cplusplus
}
#endif

#endif // #ifndef _SCALER_VGIPDRV_H_
