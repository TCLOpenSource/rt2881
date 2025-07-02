#ifndef _SCALER_VODMADRV_H_
#define _SCALER_VODMADRV_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_ARCH_RTK2875Q
#define VODMA1_2P_MODE
#endif

unsigned char drvif_scaler_vodma_get_2p_gate_sel(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _SCALER_VODMADRV_H_
