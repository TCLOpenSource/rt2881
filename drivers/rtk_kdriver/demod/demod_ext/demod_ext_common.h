#ifndef __DEMOD_EXT_COMMON_H__
#define __DEMOD_EXT_COMMON_H__

#include "demodcomm/foundation.h"
#include "demod_ext_def.h"
#include <mach/rtk_platform.h>

#define DEMOD_MEM_START_PHYS 0x15400000
#define DEMOD_MEM_END_PHYS 0x0153fffff
#define DEMOD_MEM_LENGTH 0x00800000

#ifndef _TRUE
#define _TRUE 1
#endif
#ifndef _FALSE
#define _FALSE 0
#endif

#ifndef ABS
#define ABS(x,y)				((x > y) ? (x-y) : (y-x))
#endif

typedef enum {
	PGAFIX_N7DB = -7,
	PGAFIX_N6DB = -6,
	PGAFIX_N5DB = -5,
	PGAFIX_N4DB = -5,
	PGAFIX_N3DB = -5,
	PGAFIX_N2DB = -5,
	PGAFIX_N1DB = -5,
	PGAFIX_0DB = 0,
	PGAFIX_1DB = 1,
	PGAFIX_2DB = 2,
	PGAFIX_3DB = 3,
	PGAFIX_4DB = 4,
	PGAFIX_5DB = 5,
	PGAFIX_6DB = 6,
	PGAFIX_7DB = 7,
	PGAFIX_8DB = 8,
	PGAFIX_9DB = 9,
	PGAFIX_10DB = 10,
	PGAFIX_11DB = 11,
	PGAFIX_12DB = 12,
} IFADCGAIN;


	typedef struct {
		unsigned int TpOutClk;
		unsigned int pre_div;
		unsigned int post_div;
		unsigned int div16;
	}
	SETTING_ENTRY_TP_OUTPUT_CLK;

/* support diff arch(arm64 or another) */

#ifdef CONFIG_ARM64
typedef unsigned long DUADDRESS;
#define PT_UDEC_DUADDRESS "%lu"
#define PT_DEC_DUADDRESS "%ld"
#define PT_HEX_DUADDRESS "%lx"
#else
typedef unsigned int DUADDRESS;
#define PT_UDEC_DUADDRESS "%u"
#define PT_DEC_DUADDRESS "%d"
#define PT_HEX_DUADDRESS "%x"
#endif


int DtvDemodCheckPll(RTK_DEMOD_MODE mode);
int DtvDemodPllSettingCheck(RTK_DEMOD_MODE mode);
int DtvDemodCRTSettingCheck(void);
int DtvDemodClockEnable(unsigned char On);
int DtvDemodGetPowerStatus(void);
int AtvDemodPowerControl(unsigned char On);

//int ImgIndirectRegAccess(unsigned int RegAddr, unsigned int RegVal);

#ifdef DEMOD_LDR_CAHNGE_BY_FILE
struct device* get_extdemod_device_ptr(void);
#endif

#define RTK_DEMOD_BIF_TX_FIFO_DEPTH       16
#define RTK_DEMOD_BIF_RX_FIFO_DEPTH       16


extern void __realtek_ext_wait_ms(
	BASE_INTERFACE_MODULE*      pBaseInterface,
	unsigned long               ms
);

extern int __realtek_ext_i2c_write(
	BASE_INTERFACE_MODULE*      pBaseInterface,
	unsigned char               DeviceAddr,
	const unsigned char*        pWritingBytes,
	U32BITS               ByteNum
);

extern int __realtek_ext_i2c_read(
	BASE_INTERFACE_MODULE*      pBaseInterface,
	unsigned char               DeviceAddr,
	unsigned char* pRegisterAddr,
	unsigned char RegisterAddrSize,
	unsigned char*              pReadingBytes,
	U32BITS               ByteNum
);

extern int __realtek_ext_i2c_write_speedF(
	BASE_INTERFACE_MODULE*      pBaseInterface,
	unsigned char               DeviceAddr,
	const unsigned char*        pWritingBytes,
	U32BITS               ByteNum
);

extern int __realtek_ext_i2c_read_speedF(
	BASE_INTERFACE_MODULE*      pBaseInterface,
	unsigned char               DeviceAddr,
	unsigned char* pRegisterAddr,
	unsigned char RegisterAddrSize,
	unsigned char*              pReadingBytes,
	U32BITS               ByteNum
);

#define DECODE_RET(x)       ((x==FUNCTION_SUCCESS) ? TUNER_CTRL_OK : TUNER_CTRL_FAIL)
#define ENCODE_RET(x)       ((x==TUNER_CTRL_OK) ?  FUNCTION_SUCCESS : FUNCTION_ERROR)

#define RTK_DEMOD_PRINT_LVL_DBG      0
#define RTK_DEMOD_PRINT_LVL_INFO     1
#define RTK_DEMOD_PRINT_LVL_WARNING  2

#ifndef RTK_DEMOD_PRINT_LVL
#define RTK_DEMOD_PRINT_LVL          RTK_DEMOD_PRINT_LVL_WARNING
#endif


#define RTK_DEMOD_CALLER(fmt, args...)     rtd_extdemod_print(KERN_DEBUG,"CALLER, " fmt, ##args);
#define RTK_DEMOD_DBG(fmt, args...)        rtd_extdemod_print(KERN_DEBUG,"Debug, " fmt, ##args);
#define RTK_DEMOD_INFO(fmt, args...)       rtd_extdemod_print(KERN_INFO,"Info, " fmt, ##args);
#define RTK_DEMOD_WARNING(fmt, args...)    rtd_extdemod_print(KERN_WARNING,"Warning, " fmt, ##args);

#endif // __DEMOD_EXT_COMMON_H__
