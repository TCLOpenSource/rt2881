#ifndef _SCALER_DTGDRV_H_
#define _SCALER_DTGDRV_H_

#ifdef __cplusplus
extern "C" {
#endif

void drvif_scaler_dtg_set_dsubreg_db(unsigned char bflag1, unsigned char bflag2);
void drvif_scaler_dtg_set_dsubreg_dbuf_en(unsigned char bflag);
void drvif_scaler_dtg_set_dsubreg_dbuf_set(unsigned char bflag);
unsigned char drvif_scaler_dtg_get_dsubreg_dbuf_set(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _SCALER_DTGDRV_H_
