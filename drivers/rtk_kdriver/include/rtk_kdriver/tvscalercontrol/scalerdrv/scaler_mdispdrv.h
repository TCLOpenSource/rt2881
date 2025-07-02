#ifndef _SCALER_MDISPDRV_H_
#define _SCALER_MDISPDRV_H_

#ifdef __cplusplus
extern "C" {
#endif

void drvif_scaler_mdisp_set_disp1_db(unsigned char bflag1, unsigned char bflag2);
void drvif_scaler_mdisp_set_disp1_double_enable(unsigned char bflag);
unsigned char drvif_scaler_mdisp_get_disp1_double_enable(void);
void drvif_scaler_mdisp_set_disp1_double_apply(unsigned char bflag);
unsigned char drvif_scaler_mdisp_get_disp1_double_apply(void);
void drvif_scaler_mdisp_set_disp2_db(unsigned char bflag1, unsigned char bflag2);
void drvif_scaler_mdisp_set_disp2_double_enable(unsigned char bflag);
void drvif_scaler_mdisp_set_disp2_double_apply(unsigned char bflag);
unsigned char drvif_scaler_mdisp_get_disp2_double_apply(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _SCALER_MDISPDRV_H_
