#ifndef __EDID_COMMON_H__
#define __EDID_COMMON_H__

//****************************************************************************
// INCLUDE HEADER FILES
//****************************************************************************
//#include <arch.h>

#define CONFIG_DUMP_EDIDINFO
//edid

#define ScalerOutputGetPxPortDdcRamAddr(x)    (EDID_HDMI20)
extern UINT8 EDID_HDMI20[EDID_BUFFER_SIZE];
extern INT32 ScalerMcuHwIICWrite(UINT8 id, UINT8 addr,
    UINT8 *write_buff, UINT32 write_len,
    UINT8 flag, UINT8 port);
extern INT32 ScalerMcuHwIICRead(UINT8 id, UINT8 addr,
    UINT8 *p_sub_addr, UINT8 sub_addr_len,
    UINT8 *p_read_buff, UINT32 read_len, UINT16 flags,UINT8 port);
BOOLEAN ScalerHdmiMacTx0GetDSEDID(void);
BOOLEAN ScalerHdmiMacTx0CheckEdidDifference(void);
void ModifyEdidProcess(void);
void ScalerHdmiMacTx0SCDCSetTMDSConfiguration(void);
void ScalerEdidGetCtaExtDbAddress(UINT16 *pusCtaDataBlockAddr);
#endif // #ifndef __SCALER_FUNCTION_INCLUDE_H__
