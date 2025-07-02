#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <rtk_kdriver/rtk_crt.h>
#include <rtk_kdriver/io.h>
#include <asm/cacheflush.h>
#include "demod_ext_common.h"
#include "comm.h"
#include "tv_osal.h"
//#include "ifd_analog.h"


#include "rbus/tv_sb1_ana_reg.h"
#include "rbus/dtv_frontend_reg.h"
//#include "rbus/diseqc_reg.h"
//#include "rbus/img_demod_bus_reg.h"
//#include "rbus/demod_sd_reg.h"
//#include "demodcore/message_func_rtk.h"
#include "rbus/rtk_dtv_demod_8051_reg.h"
#include "rbus/dtv_demod_misc_reg.h"
#include "rbus/rtk_dtv_demod_sys_reg.h"
//#include "rbus/hdic_register_bus_reg.h"
#include "rbus/stb_reg.h"
#include "rbus/topbist_reg.h"
#include "rbus/pll27x_reg_reg.h"
#include <rbus/tvsb5_reg.h>

#include <linux/dma-mapping.h>
#include <linux/pageremap.h>

#include <tp/tp_def.h>
#include <tp/tp_drv_api.h>
#include "rbus/efuse_reg.h"

#include <rtk_kdriver/rtk_otp_region_api.h>


extern int rtk_otp_field_read_int_by_name(const char *name);


#define DYNAMIC_MEM_ALLOCATE 0
#define TPO_DIVIDEND_CONSTANT 250*1000
#define TPO_DVISOR_CONSTANT   2
// Merlin4 Ver.A bring up
//#define MERLIN4_VER_A
//#define REALTEK_R_EXT_QFN


/*------------------------------------------------------------------------------
 * Chip Initialization
 *------------------------------------------------------------------------------*/
//#define rtd_outl(addr, value)   pli_writeReg32(addr, value)
//#define rtd_inl(addr)           pli_readReg32(addr)
//#define rtd_maskl(addr, mask, value)    rtd_outl(addr, ((rtd_inl(addr) & mask) | value))

#define _BIT(x)       (1UL <<x)
#ifndef _BIT0
#define _BIT0          _BIT(0 )
#define _BIT1          _BIT(1 )
#define _BIT2          _BIT(2 )
#define _BIT3          _BIT(3 )
#define _BIT4          _BIT(4 )
#define _BIT5          _BIT(5 )
#define _BIT6          _BIT(6 )
#define _BIT7          _BIT(7 )
#define _BIT8          _BIT(8 )
#define _BIT9          _BIT(9 )
#define _BIT10         _BIT(10)
#define _BIT11         _BIT(11)
#define _BIT12         _BIT(12)
#define _BIT13         _BIT(13)
#define _BIT14         _BIT(14)
#define _BIT15         _BIT(15)
#define _BIT16         _BIT(16)
#define _BIT17         _BIT(17)
#define _BIT18         _BIT(18)
#define _BIT19         _BIT(19)
#define _BIT20         _BIT(20)
#define _BIT21         _BIT(21)
#define _BIT22         _BIT(22)
#define _BIT23         _BIT(23)
#define _BIT24         _BIT(24)
#define _BIT25         _BIT(25)
#define _BIT26         _BIT(26)
#define _BIT27         _BIT(27)
#define _BIT28         _BIT(28)
#define _BIT29         _BIT(29)
#define _BIT30         _BIT(30)
#define _BIT31         _BIT(31)
#endif

//#define REG_RTK_DEMOD_8051_SRAM_WRITE_EN 0xB807223C//Merlin3/Mac5p Demod 8051

//U32BITS RbusAddr = 0;
unsigned char I2c_SpeedByPCB = 0;



/*------------------------------------------------------------------------------
 * FUNC : DTV demod memory mapping
 *------------------------------------------------------------------------------*/
/*
int DtvDemodMemRemapping(RTK_DEMOD_MODE mode)
{

	size_t DdrSize = 0;
	unsigned int AllocateSize = 0 ;

#if DYNAMIC_MEM_ALLOCATE
	unsigned char* nonCachedAddr;
#endif

	switch (mode) {
	case RTK_DEMOD_MODE_DVBT:
	case RTK_DEMOD_MODE_DVBT2:
		DdrSize = RTK_A_DVBT2_DDR_SIZE;
		RTK_DEMOD_INFO("DtvDemodMemRemapping (DVBTX) DDR_SIZE= 0x"PT_HEX_DUADDRESS"\n", DdrSize);
		break;
	case RTK_DEMOD_MODE_DTMB:
		DdrSize = RTK_H_DTMB_DDR_SIZE;
		RTK_DEMOD_INFO("DtvDemodMemRemapping (DTMB) DDR_SIZE= 0x"PT_HEX_DUADDRESS"\n", DdrSize);
		break;

	default:
		RTK_DEMOD_WARNING("TV MODE not support at DtvDemodMemRemapping! TV_MODE = %d \n", mode);
		//return FUNCTION_ERROR; for coverity
		break;

	}

	if (DdrSize) {


#if DYNAMIC_MEM_ALLOCATE

		if (pShareMemory)
			pli_freeContinuousMemoryMesg("DTV_DEMOD", pShareMemory);


		pShareMemory = (unsigned char*)pli_allocContinuousMemoryMesg(
						   "DTV_DEMOD",
						   DdrSize,
						   &nonCachedAddr,
						   &ShareMemoryPhysicalAddress);

		if (pShareMemory == NULL) {
			RTK_DEMOD_WARNING("DtvDemodMemRemapping failed, allocate share memory failed\n");
			return FUNCTION_ERROR;
		}


		RTK_DEMOD_INFO("allocate share memory(dynamic) - catch_addr=%p/%08lx, size=%08x\n",
					   pShareMemory,
					   ShareMemoryPhysicalAddress,
					   DdrSize);


#else

		AllocateSize = CravedoutMemoryQuery(&ShareMemoryPhysicalAddress);

		if (AllocateSize < DdrSize)
			RTK_DEMOD_WARNING("allocate share memory(fix) - allocate DDR size is not enough!!!\n");

		RTK_DEMOD_INFO("allocate share memory(fix) - Physical_addr=0x%08lx, size=0x%08x\n", ShareMemoryPhysicalAddress, AllocateSize);
#endif




	}


	switch (mode) {
	case RTK_DEMOD_MODE_DVBT:
	case RTK_DEMOD_MODE_DVBT2:
		rtd_outl(DTV_DEMOD_MISC_atb_wrapper_addr_offset_reg, ShareMemoryPhysicalAddress);
		break;
	case RTK_DEMOD_MODE_DTMB:
		rtd_outl(HDIC_REGISTER_BUS_hdic_mem_bus_ctrl4_reg, ShareMemoryPhysicalAddress);
		break;

	default:
		RTK_DEMOD_WARNING("TV MODE not support at DtvDemodMemRemapping! TV_MODE = %d \n", mode);
		return FUNCTION_ERROR;
		break;

	}


	return FUNCTION_SUCCESS;
}
*/

/*------------------------------------------------------------------------------
 * Base Interface API
 *------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 * FUNC : __realtek_ext_wait_ms
 *
 * DESC : wait function for merlin2 baseinterface
 *
 * PARM : pBIF  : Private Data that wants contain in this base class
 *        ms    : time to wait in ms
 *
 * RET  : N/A
 *------------------------------------------------------------------------------*/
void __realtek_ext_wait_ms(BASE_INTERFACE_MODULE* pBIF, unsigned long ms)
{
	tv_osal_msleep_ext(ms);
}



/*------------------------------------------------------------------------------
 * FUNC : __rtd299s_i2c_write
 *
 * DESC : i2c write function for rtd299s base if
 *
 * PARM : pBaseInterface    : handle of rtd299s baseif
 *        DeviceAddr        : device address
 *        pWritingBytes     : data to be written
 *        ByteNum           : number of data to write
 *
 * RET  : FUNCTION_SUCCESS / FUNCTION_ERROR
 *------------------------------------------------------------------------------*/
int __realtek_ext_i2c_write(
	BASE_INTERFACE_MODULE*      pBaseInterface,
	unsigned char               DeviceAddr,
	const unsigned char*        pWritingBytes,
	U32BITS               ByteNum
)
{
	COMM* pComm;
	pBaseInterface->GetUserDefinedDataPointer(pBaseInterface, (void**)&pComm);
	//RTK_DEMOD_INFO("[ext_i2c_write] i2c speed = %x\n", I2c_SpeedByPCB);
	if(I2c_SpeedByPCB == 0) {
		return ENCODE_RET(pComm->SendWriteCommand(pComm, DeviceAddr, (unsigned char*) pWritingBytes, ByteNum, I2C_M_FAST_SPEED));
	}
	else {
		return ENCODE_RET(pComm->SendWriteCommand(pComm, DeviceAddr, (unsigned char*) pWritingBytes, ByteNum, I2c_SpeedByPCB));
	}
}



/*------------------------------------------------------------------------------
 * FUNC : __rtd299s_i2c_read
 *
 * DESC : i2c read function for rtd299s base if
 *
 * PARM : pBaseInterface    : handle of rtd299s baseif
 *        DeviceAddr        : device address
 *        pReadingBytes     : read data buffer
 *        ByteNum           : number of data to read
 *
 * RET  : FUNCTION_SUCCESS / FUNCTION_ERROR
 *------------------------------------------------------------------------------*/
int __realtek_ext_i2c_read(
	BASE_INTERFACE_MODULE*      pBaseInterface,
	unsigned char               DeviceAddr,
	unsigned char* pRegisterAddr,
	unsigned char RegisterAddrSize,
	unsigned char*              pReadingBytes,
	U32BITS               ByteNum
)
{

	COMM* pComm ;
	pBaseInterface->GetUserDefinedDataPointer(pBaseInterface, (void**)&pComm);
	//RTK_DEMOD_INFO("[ext_i2c_read] i2c speed = %x\n", I2c_SpeedByPCB);
	if(I2c_SpeedByPCB == 0) {
		return ENCODE_RET(pComm->SendReadCommand(pComm, DeviceAddr, pRegisterAddr, RegisterAddrSize, pReadingBytes, ByteNum, I2C_M_FAST_SPEED));
	}
	else {
		return ENCODE_RET(pComm->SendReadCommand(pComm, DeviceAddr, pRegisterAddr, RegisterAddrSize, pReadingBytes, ByteNum, I2c_SpeedByPCB));
	}
}


/*------------------------------------------------------------------------------
 * FUNC : __rtd299s_i2c_write
 *
 * DESC : i2c write function for rtd299s base if
 *
 * PARM : pBaseInterface    : handle of rtd299s baseif
 *        DeviceAddr        : device address
 *        pWritingBytes     : data to be written
 *        ByteNum           : number of data to write
 *
 * RET  : FUNCTION_SUCCESS / FUNCTION_ERROR
 *------------------------------------------------------------------------------*/
int __realtek_ext_i2c_write_speedF(
	BASE_INTERFACE_MODULE*      pBaseInterface,
	unsigned char               DeviceAddr,
	const unsigned char*        pWritingBytes,
	U32BITS               ByteNum
)
{
	COMM* pComm;
	pBaseInterface->GetUserDefinedDataPointer(pBaseInterface, (void**)&pComm);
	return ENCODE_RET(pComm->SendWriteCommand(pComm, DeviceAddr, (unsigned char*) pWritingBytes, ByteNum, I2C_M_FAST_SPEED));
}



/*------------------------------------------------------------------------------
 * FUNC : __rtd299s_i2c_read
 *
 * DESC : i2c read function for rtd299s base if
 *
 * PARM : pBaseInterface    : handle of rtd299s baseif
 *        DeviceAddr        : device address
 *        pReadingBytes     : read data buffer
 *        ByteNum           : number of data to read
 *
 * RET  : FUNCTION_SUCCESS / FUNCTION_ERROR
 *------------------------------------------------------------------------------*/
int __realtek_ext_i2c_read_speedF(
	BASE_INTERFACE_MODULE*      pBaseInterface,
	unsigned char               DeviceAddr,
	unsigned char* pRegisterAddr,
	unsigned char RegisterAddrSize,
	unsigned char*              pReadingBytes,
	U32BITS               ByteNum
)
{

	COMM* pComm ;
	pBaseInterface->GetUserDefinedDataPointer(pBaseInterface, (void**)&pComm);
	return ENCODE_RET(pComm->SendReadCommand(pComm, DeviceAddr, pRegisterAddr, RegisterAddrSize, pReadingBytes, ByteNum, I2C_M_FAST_SPEED));
}



