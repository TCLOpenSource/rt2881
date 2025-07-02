#ifndef _SCALER_HDRDRV_H_
#define _SCALER_HDRDRV_H_

#ifdef __cplusplus
extern "C" {
#endif

void hdr_crt_ctrl(bool enable);
void hdr_all_top_top_ctl_set_vs_inv(unsigned int bflag);
void hdr_all_top_top_ctl_set_hdr1_2p(unsigned int bflag);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _SCALER_HDRDRV_H_
