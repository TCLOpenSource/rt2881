#ifndef  __DEMOD_CXD2878_S_ISDBT_PRIV_H__
#define  __DEMOD_CXD2878_S_ISDBT_PRIV_H__

#include "CXD2878Family_refcode/sony_demod.h"

//#define GET_SIGNAL_STRENGTH_FROM_SNR

typedef struct {
	sony_demod_t*      pDemod;
	BASE_INTERFACE_MODULE*  pBaseInterface;
	I2C_BRIDGE_MODULE*      pI2CBridge;
	unsigned char           DeviceAddr;
	U32BITS           CrystalFreqHz;
} CXD2878_S_ISDBT_DRIVER_DATA;

#define DECODE_LOCK(x)      ((x==YES) ? DTV_SIGNAL_LOCK : DTV_SIGNAL_NOT_LOCK)

extern CXD2878_S_ISDBT_DRIVER_DATA* AllocRealtekSIsdbt2878Driver(
	COMM*               pComm,
	unsigned char       Addr,
	U32BITS       CrystalFreq
);

extern void ReleaseRealtekSIsdbt2878Driver(CXD2878_S_ISDBT_DRIVER_DATA *pDriver);

//--------------------------------------------------------------------------
// Optimization Setting for Tuner IFAGC (Max/Min)
//--------------------------------------------------------------------------

#endif // __DEMOD_CXD2878_S_ISDBT_PRIV_H__
