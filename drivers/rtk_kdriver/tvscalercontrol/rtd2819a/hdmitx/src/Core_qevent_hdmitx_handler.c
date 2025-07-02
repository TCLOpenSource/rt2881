/*===========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2013
  * All rights reserved.
  * =========================================================================*/

/*================= File Description ========================================*/
/**														   x`
 * @file
 *  This file is a main fuction and entrance for firmwae.
 *
 * @author  $Author$
 * @date    $Date$
 * @version $Revision$
 * @ingroup System
 */

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h> 
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h> 
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_init.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_driver.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_lib.h>



/************************
// Static/Local variable declaration
//
*************************/
//static UINT8 value_u8;
//static UINT32 value_u32;


/************************
// Extern Function declaration
//
*************************/
void hdmitx_state_event_handler(UINT32 param); // [BANK1][TIMER] don't need declare __bank due to timer event handler call by bank1 function (same bank function)
void hdmitx_check_event_handler(UINT32 param); // [BANK0][TX_INIT]
void hdmitx_deinit_event_handler(UINT32 param);
void hdmitx_AP_Init_event_handler(UINT32 param);
void hdmitx_AP_NoSignal_event_handler(UINT32 param) ;
void hdmitx_AP_ReInit_event_handler(UINT32 param);
void hdmitx_Scaler_Txrun_event_handler(UINT32 param);
void hdmitx_AP_CTS_Mode_event_handler(UINT32 param);
extern void ScalerSetTimerEvent(UINT32 param);
/************************
// Function implementation
//
*************************/
#if 0
void fw_HdmitxMsgEventHandler(_MSG_Q_PARM* param)
{
    if(param == 0){
        NoteMessageHDMITx(LOGGER_ERROR, "NULL@fw txMsgEventHandler\n");
        return;
    }

    value_u8 = param->sub_id;
    value_u32 = param->Param;

    NoteMessageHDMITx(LOGGER_ERROR, "hdmitx_fw_mq:%x\n", (UINT32)value_u8);

    switch(value_u8){
      case TX_EVENT_CHECK_EVENT:
        hdmitx_check_event_handler(value_u32);
        break;
      case TX_EVENT_STATE_EVENT:
        hdmitx_state_event_handler(value_u32);
        break;
      case TX_EVENT_DEINIT_EVENT:
        hdmitx_deinit_event_handler(value_u32);
        break;
      case MSG_HDMITX_AP_TX_STATE_INITIAL:
        hdmitx_AP_Init_event_handler(value_u32);
        break;
      case MSG_HDMITX_AP_TX_STATE_REINITIAL:
        hdmitx_AP_ReInit_event_handler(value_u32);
        break;
      case MSG_HDMITX_AP_RX_NO_SIGNAL:
        hdmitx_AP_NoSignal_event_handler(value_u32);
        break;
      case MSG_HDMITX_SCALER_TXRUN:
        hdmitx_Scaler_Txrun_event_handler(value_u32);
        break;
      case MSG_HDMITX_AP_TX_CTS_MODE:
        hdmitx_AP_CTS_Mode_event_handler(value_u32);
        break;
      default:
	NoteMessageHDMITx(LOGGER_ERROR, "[HDMITX] UNKNOWN MSG EVENT:%x\n", (UINT32)value_u8);
        break;
    }

    return;
}


void timer_HDMITXMsgEventHandler(_MSG_Q_PARM* param)
{

    if(param == 0){
        NoteMessageHDMITx(LOGGER_ERROR, "NULL@timer txMsgEventHandler\n");
        return;
    }

    value_u8 = param->sub_id;
    value_u32 = param->Param;
    //if(ScalerHdmiTx_Get_Timing_Status() != TX_TIMING_SETTING_DONE)
        //NoteMessageHDMITx(LOGGER_ERROR, "hdmitx_timer_msg:%x\n", (UINT32)value_u8);
    //delayms(1000);
    // pass param->sub_id (16bits) & param->Param (32bits)

    switch(value_u8){
      case TX_EVENT_CHECK_EVENT:
        hdmitx_check_event_handler(value_u32);
        break;
      case TX_EVENT_STATE_EVENT:
        hdmitx_state_event_handler(value_u32);
        break;
      case TX_EVENT_DEINIT_EVENT:
        hdmitx_deinit_event_handler(value_u32);
        break;

      default:
        ScalerSetTimerEvent(value_u8);
	//NoteMessageHDMITx(LOGGER_ERROR, "[HDMITX] UNKNOWN TIMER EVENT:%x\n", (UINT32)value_u8);
        break;
    }

    return;
}


void isr_HDMITXMsgEventHandler(_MSG_Q_PARM* param)
{
    if(param == 0){
        NoteMessageHDMITx(LOGGER_ERROR, "NULL@Isr txMsgEventHandler\n");
        return;
    }

    value_u8 = param->sub_id;
    value_u32 = param->Param;

    NoteMessageHDMITx(LOGGER_ERROR, "hdmitx_isr_msg:%x\n", (UINT32)value_u8);

    NoteMessageHDMITx(LOGGER_ERROR, "ISR MSG Event IS NOT SUPPORT YET!!\n");

    // pass param->sub_id (16bits) & param->Param (32bits)

    return;
}
#endif
