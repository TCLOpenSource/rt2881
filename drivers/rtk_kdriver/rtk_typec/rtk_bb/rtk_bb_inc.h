#ifndef __RTK_BB_INC_H__
#define __RTK_BB_INC_H__
//----------------------------------------------------------------------------------------------------
// ID Code      : BillboardInclude.h
// Update Note  :
//----------------------------------------------------------------------------------------------------

#if(_USB_TYPE_C_BB_PROGRAM_SUPPORT == _ON)
//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#define GET_USB_BB_FLASH_WRITE_FLAG(private_data)                     ((private_data)->bUsbBillboardFlashWriteFlag)
#define SET_USB_BB_FLASH_WRITE_FLAG(private_data)                     ((private_data)->bUsbBillboardFlashWriteFlag = _TRUE)
#define CLR_USB_BB_FLASH_WRITE_FLAG(private_data)                     ((private_data)->bUsbBillboardFlashWriteFlag = _FALSE)

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************


//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
#endif

#endif
