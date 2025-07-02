#ifndef __RTK_BB_INTERFACE_H__
#define __RTK_BB_INTERFACE_H__
//----------------------------------------------------------------------------------------------------
// ID Code      : BillboardInterface.h
// Update Note  :
//----------------------------------------------------------------------------------------------------

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************

#if(_TYPE_C_PORT_CTRL_SUPPORT == _ON)
#define GET_USB_HUB_MUX_SEL_STATUS(private_data)                    ((private_data)->ucUsbBillboardMuxSelPort)
#define SET_USB_HUB_MUX_SEL_STATUS(private_data, x)                   ((private_data)->ucUsbBillboardMuxSelPort = (x))
#endif

#if(_USB_TYPE_C_BB_PROGRAM_SUPPORT == _ON)

#define _USB_VENDOR_DUAL_BANK_PROCESS                   0x45

#define GET_USB_BB_ISP_READY_FLAG(private_data)                     ((private_data)->bUsbBillboardIspInfoReadyFlag)
#define SET_USB_BB_ISP_READY_FLAG(private_data, x)                    ((private_data)->bUsbBillboardIspInfoReadyFlag = (x))
#define CLR_USB_BB_ISP_READY_FLAG(private_data)                     ((private_data)->bUsbBillboardIspInfoReadyFlag = _FALSE)

#define GET_USB_BB_ISP_SUB_OPCODE(private_data)                     ((private_data)->stUsbBillboardIspCommandInfo.ucSubOpCode)
#define SET_USB_BB_ISP_SUB_OPCODE(private_data, x)                    ((private_data)->stUsbBillboardIspCommandInfo.ucSubOpCode = (x))
#define CLR_USB_BB_ISP_SUB_OPCODE(private_data)                     ((private_data)->stUsbBillboardIspCommandInfo.ucSubOpCode = _FALSE)

#define GET_USB_BB_ISP_OPCODE(private_data)                         ((private_data)->stUsbBillboardIspCommandInfo.ucOpCode)
#define SET_USB_BB_ISP_OPCODE(private_data, x)                        ((private_data)->stUsbBillboardIspCommandInfo.ucOpCode = (x))
#define CLR_USB_BB_ISP_OPCODE(private_data)                         ((private_data)->stUsbBillboardIspCommandInfo.ucOpCode = _FALSE)


#define GET_USB_BB_ISP_REV_CMD_STATUS(private_data)                 ((private_data)->stUsbBillboardIspCommandInfo.ucRevCmd)
#define SET_USB_BB_ISP_REV_CMD_STATUS(private_data)                 ((private_data)->stUsbBillboardIspCommandInfo.ucRevCmd = _TRUE)
#define CLR_USB_BB_ISP_REV_CMD_STATUS(private_data)                 ((private_data)->stUsbBillboardIspCommandInfo.ucRevCmd = _FALSE)

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************

//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************

extern void UsbBillboardIspFlagInitial(struct USB_BB_PRIVATE_DATA *private_data);
#endif

#endif
