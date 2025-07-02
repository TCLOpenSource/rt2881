#ifndef _SCALER_I3DDMAPDRV_H_
#define _SCALER_I3DDMAPDRV_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CEIL_MODE(x, mode) (((x) + (mode) - 1) & (~(mode - 1)))

void drvif_scaler_i3ddma_set_pqc_frame_limit_bit(unsigned int comp_ratio);
void drvif_scaler_i3ddma_set_pqc_line_limit_bit(unsigned int comp_ratio);
void drvif_scaler_i3ddma_set_nn_inerrupt_en(unsigned char bOnOff);



#ifdef __cplusplus
}
#endif

#endif // #ifndef _SCALER_I3DDMAPDRV_H_

