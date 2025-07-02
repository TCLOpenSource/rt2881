#ifndef  __DEMOD_CXD2878_S_ISDBS3_PRIV_H__
#define  __DEMOD_CXD2878_S_ISDBS3_PRIV_H__


#include "CXD2878Family_refcode/sony_demod.h"

//#define GET_SIGNAL_STRENGTH_FROM_SNR

typedef struct {
	sony_demod_t*      pDemod;
	BASE_INTERFACE_MODULE*  pBaseInterface;
	I2C_BRIDGE_MODULE*      pI2CBridge;
	unsigned char           DeviceAddr;
	U32BITS           CrystalFreqHz;
} CXD2878_S_ISDBS3_DRIVER_DATA;

#define DECODE_LOCK(x)      ((x==YES) ? DTV_SIGNAL_LOCK : DTV_SIGNAL_NOT_LOCK)

extern CXD2878_S_ISDBS3_DRIVER_DATA* AllocRealtekSIsdbs3Driver(
	COMM*               pComm,
	unsigned char       Addr,
	U32BITS       CrystalFreq
);

extern void ReleaseRealtekSIsdbs3Driver(CXD2878_S_ISDBS3_DRIVER_DATA *pDriver);//isdbs3 CXD2878
//--------------------------------------------------------------------------
// Optimization Setting for Tuner IFAGC (Max/Min)
//--------------------------------------------------------------------------

#endif // __DEMOD_CXD2878_S_ISDBS3_PRIV_H__
