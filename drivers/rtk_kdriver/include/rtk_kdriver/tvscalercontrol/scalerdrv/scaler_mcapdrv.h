#ifndef _SCALER_MCAPDRV_H_
#define _SCALER_MCAPDRV_H_

#ifdef __cplusplus
extern "C" {
#endif

void drvif_scaler_mcap_set_cap1_db(unsigned char bflag1, unsigned char bflag2);
void drvif_scaler_mcap_set_cap1_db_en(unsigned char bflag);
unsigned char drvif_scaler_mcap_get_cap1_db_en(void);
void drvif_scaler_mcap_set_cap1_db_apply(unsigned char bflag);
unsigned char drvif_scaler_mcap_get_cap1_db_apply(void);
void drvif_scaler_mcap_set_cap2_db(unsigned char bflag1, unsigned char bflag2);
void drvif_scaler_mcap_set_cap2_db_en(unsigned char bflag);
unsigned char drvif_scaler_mcap_get_cap2_db_en(void);
void drvif_scaler_mcap_set_cap2_db_apply(unsigned char bflag);
unsigned char drvif_scaler_mcap_get_cap2_db_apply(void);
void drvif_scaler_mcap_set_in2_pqc_clken(unsigned char bflag);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _SCALER_MCAPDRV_H_
