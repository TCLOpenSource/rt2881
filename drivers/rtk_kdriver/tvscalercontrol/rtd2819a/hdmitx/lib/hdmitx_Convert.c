/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2012>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : hdmitx_Convert.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
//#include <rtk_kdriver/rtk_i2c.h>
//#include <rbus/ppoverlay_reg.h>
//#include <rbus/ppoverlay_outtg_reg.h>
#include <rbus/ppoverlay_reg.h>

#include "../include/ScalerFunctionInclude.h"
#include "../include/ScalerTx.h"
#include "../include/hdmitx_phy_struct_def.h"
#include "../include/HdmiEdid/hdmitx_edid.h"
#include "../include/EdidCommon.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0Include.h"
//#include <rtk_kdriver/pcbMgr.h>
//#include "user_data.h"
#include <rtk_kdriver/measure/rtk_measure.h>

#define ABS(x, y)   ((x > y) ? (x-y) : (y-x))

extern UINT8 ScalerHdmiTxPhy_GetTxTimingTable(UINT8 index, hdmi_tx_timing_gen_st *time_mode);
extern UINT8 ScalerHdmiMacTx0EdidGetFeature(EnumEdidFeatureType enumEdidFeature) ;
extern void getDTDTimingInfo(UINT16 ucDTDAddress);
extern void get20ByteDTDTimingInfo(UINT16 ucDTDAddress);
extern UINT16 calcDTDTimingvFreq(void);
extern UINT8 ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(EDID_TIMING_DB edid_timing_type);
extern TX_TIMING_INDEX ScalerHdmiMacTx0RepeaterPopSinkTxSupportTimingList(EDID_TIMING_DB edid_timing_type, UINT8 index, UINT8 *db_type, BOOLEAN *bSupport420);
#ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
extern UINT8 edid_dtd_flag;
#endif

extern UINT8 tx_output_colorSpace;
extern UINT8 tx_output_colordepth;
extern UINT32 g_TxPixelClock1024;
extern UINT16 pixelClockLimit;
extern UINT32 SourceOutputPixelClk;
extern TX_TIMING_INDEX time_mode;
extern EDID_SUPPORT_TIMING_TYPE repeater_support_timing_table;
extern hdmi_timing_check_type  edid_timing;
extern hdmi_timing_check_type  dtd_timing;
extern UINT8 *pucDdcRamAddr;

extern UINT16 usExtTagBaseAddr;
extern UINT16 pusCtaDataBlockAddr[_CTA_TOTAL_DB + 1];
extern BOOLEAN g_bHdmiMacTx0FindSpecialTv5;
hdmi_tx_timing_gen_st tx_timing_table={0};
extern MEASURE_TIMING_T timing_info;
static UINT8 time_match = 0;
static TX_TIMING_INDEX max_420_resolution_time_mode = 0;
static TX_TIMING_INDEX max_rgb_resolution_time_mode = 0;
static UINT32 rgbMaxPixelClock1024;
static UINT32 y420MaxPixelClock1024;
static UINT16 txVtotal, txHtotal;
static UINT32 u32Y420Bandwidth = 0;
static TX_OUT_TIMING_PRIORITY_TYPE enY420TimingPriority = 0;
static UINT32 u32RgbBandwidth = 0;
static TX_OUT_TIMING_PRIORITY_TYPE enRgbTimingPriority = 0;
static UINT32 u32BandWidth = 0;
#ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
static UINT8 max_push_list_index = 0xff;
static UINT8 dtd_match_flag = FALSE;
#endif
UINT8 adjust_vtotal_flag = FALSE;
UINT16 adjust_timing_vtotal = 0;
UINT16 adjust_rgb_vtotal = 0;
UINT16 adjust_420_vtotal = 0;
UINT16 cur_vratio = 0xffff;
UINT16 min_444_vratio = 0xffff;
UINT16 min_420_vratio = 0xffff;
UINT16 adjust_vrr_vstart = 0;

hdmi_tx_bandwidth_st bandwidth_table[_HDMI21_FRL_MAX_NUM] = {
    {_HDMI21_FRL_NONE, HDMITX_FRL_3G_DATA},
    {_HDMI21_FRL_3G, HDMITX_FRL_3G_DATA},
    {_HDMI21_FRL_6G_3LANES, HDMITX_FRL_6G3L_DATA},
    {_HDMI21_FRL_6G_4LANES, HDMITX_FRL_6G4L_DATA},
    {_HDMI21_FRL_8G, HDMITX_FRL_8G_DATA},
    {_HDMI21_FRL_10G, HDMITX_FRL_10G_DATA},
    {_HDMI21_FRL_12G, HDMITX_FRL_12G_DATA},
};
hdmi_tx_outputDepth_st outputDepth_table[TX_OUT_TIMING_PRIORITY_MAX_NUM+1] = {
    {TX_OUT_RGB_12B, HDMITX_12BIT},
    {TX_OUT_Y444_12B, HDMITX_12BIT},
    {TX_OUT_RGB_10B, HDMITX_10BIT},
    {TX_OUT_Y444_10B, HDMITX_10BIT},
    {TX_OUT_Y422_12B, HDMITX_12BIT},
    {TX_OUT_Y422_10B, HDMITX_10BIT},
    {TX_OUT_Y420_12B, HDMITX_12BIT},
    {TX_OUT_Y420_10B, HDMITX_10BIT},
    {TX_OUT_RGB_8B, HDMITX_8BIT},
    {TX_OUT_Y444_8B, HDMITX_8BIT},
    {TX_OUT_Y422_8B, HDMITX_8BIT},
    {TX_OUT_Y420_8B, HDMITX_8BIT},
    {TX_OUT_TIMING_PRIORITY_MAX_NUM, HDMITX_8BIT},
};
static hdmi_tx_timing_gen_st tx_Max_rgb_timing_table={0};
static hdmi_tx_timing_gen_st tx_Max_420_timing_table={0};
extern StructHDMITxModeInfoType g_stHdmiMacTx0ModeInfo;
#define EDID_GET_HDMI_MAC_TX0_SCRAMBLE_RETRY()                       (g_stHdmiMacTx0ModeInfo.b1HDMITxScrambleRetry)
#define EDID_SET_HDMI_MAC_TX0_SCDC_PROC_STATE(x)                     (g_stHdmiMacTx0ModeInfo.ucHDMITxScrambleStatus = (x))
#define EDID_SET_HDMI_MAC_TX0_SINK_SCRAMBLE_STATUS(x)                (g_stHdmiMacTx0ModeInfo.ucHDMITxSinkScrambleStatus = (x))
#define EDID_CLR_HDMI_MAC_TX0_SCRAMBLE_RETRY()                       (g_stHdmiMacTx0ModeInfo.b1HDMITxScrambleRetry = _FALSE)
#define EDID_GET_HDMI_MAC_TX0_HPD_EVENT()                            (g_stHdmiMacTx0ModeInfo.enumHDMITxHPDEvent)
#define EDID_GET_HDMI_MAC_TX0_SCDC_PROC_STATE()                      (g_stHdmiMacTx0ModeInfo.ucHDMITxScrambleStatus)
#define EDID_GET_HDMI_MAC_TX0_STREAM_SOURCE()                        (g_stHdmiMacTx0ModeInfo.b2StreamSource)
#define WAIT_TIMEOUT (90*100)
#define MAX_OUTPUT_HACTIVE 3840

extern UINT32 stc;
extern UINT16 lineCnt, start;
//static UINT8 wait_zero;
extern UINT32 lineCntAddr;
//extern I2C_XFER_INFO_T xfer_info;
extern I2C_HDMITX_PARA_T i2c_para_info;
//extern PCB_ENUM_VAL HDMI_TX_I2C_BUSID;
void ScalerHdmiMacTx0SCDCScrambleEnable(void);
void ScalerHdmiMacTx0SCDCScrambleCheck(void);
void ScalerHdmiMacTx0SCDCScrambleDisable(void);


#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Scdc Handshake Process
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0SCDCProcHandler(void)
{
    if((EDID_GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_TOGGLE_EVENT) || (EDID_GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_Z0_TOGGLE_EVENT))
    {
        ScalerHdmiMacTx0SCDCSetTMDSConfiguration();
    }

    switch(EDID_GET_HDMI_MAC_TX0_SCDC_PROC_STATE())
    {
        case _HDMI_TX_SCDC_PROC_STATE_INITIAL:
            break;

        case _HDMI_TX_SCDC_PROC_STATE_SCRAMBLE_ENABLE:
            ScalerHdmiMacTx0SCDCScrambleEnable();

            break;

        case _HDMI_TX_SCDC_PROC_STATE_SCRAMBLE_CHECK:
            ScalerHdmiMacTx0SCDCScrambleCheck();

            break;

        case _HDMI_TX_SCDC_PROC_STATE_SCRAMBLE_DISABLE:
            ScalerHdmiMacTx0SCDCScrambleDisable();

            break;

        case _HDMI_TX_SCDC_PROC_STATE_STANDBY:
        default:
            break;
    }
}


//--------------------------------------------------
// Description  : Hdmi Tx SCDC Scramble Enable
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0SCDCScrambleEnable(void)
{
    UINT8 ucScdcSetValue = 0x03;

    DebugMessageHDMITx("[HDMI_TX] SCDC Communication -> Enable Scramble !!!!\n");

    // HF-VSDB SCDC_Present == 1
    if(ScalerHdmiMacTxEdidGetFeature((EnumOutputPort)_TX0, _SCDC_PRESENT) == _TRUE)
    {

        subEdidAddr[0] = 0x20;
        subEdidAddr[1] = ucScdcSetValue;
        if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
        {
            if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
            {
                if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
                {
                    FatalMessageHDMITx("[HDMI_TX] Scramble En W-Fail!@%d\n", (UINT32)ucScdcSetValue);
                }
            }
        }

        ScalerTimerActiveTimerEvent(300, _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_CHECK);

        EDID_SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_STANDBY);

    }

#ifdef CONFIG_MACH_RTD2801_ZEBU //#ifdef CONFIG_ENABLE_ZEBU_BRING_UP
    // Don't enable MAC En-Scramble Module in ZEBU platform
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT5), (_BIT6));
    NoteMessageHDMITx("[HDMI_TX] Disable ScrambleEn@SCDCProcHandler\n");
#else // #ifdef CONFIG_ENABLE_ZEBU_BRING_UP
    // Enable MAC En-Scramble Module
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT5), (_BIT6 | _BIT5));
#endif // #else // #ifdef CONFIG_ENABLE_ZEBU_BRING_UP
}

//--------------------------------------------------
// Description  : Hdmi Tx Check Sink Scramble Status
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0SCDCScrambleCheck(void)
{
    UINT8 ucScdcSetValue = 0x00;

#if 0
    if(1){
        static UINT32 dbgCnt=0;
        if((dbgCnt++ % 10) == 0)
            NoteMessageHDMITx("[HDMI_TX][%d] Skip SCDC Scramble Check!\n", dbgCnt);
        return;
    }
#endif

#if 0//#if(_HDMI_MAC_TX0_PURE == _ON)
    ScalerHdmiPhyTxSetZ0Detect(EdidMacPxTxMapping(_TX0), _ENABLE);

    // if Down Stream Sink Z0 = Low -> Return (Do-Not Check SCDC)
    if(ScalerHdmiPhyTxZ0Detect(EdidMacPxTxMapping(_TX0)) == _HDMI_TX_Z0_ALL_LOW)
    {
        ScalerHdmiPhyTxSetZ0Detect(EdidMacPxTxMapping(_TX0), _DISABLE);

        return;
    }

    ScalerHdmiPhyTxSetZ0Detect(EdidMacPxTxMapping(_TX0), _DISABLE);
#endif

    DebugMessageHDMITx("[HDMI_TX] SCDC Communication -> Check Sink Scramble_Status !!!!\n");

    pEdidData[0] = 0x00;
    subEdidAddr[0] = 0x21;
    if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _FALSE)
    {
        if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _FALSE)
        {
            if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _FALSE)
            {
                FatalMessageHDMITx("[HDMI_TX] Scramble Check 0x21 Fail\n");
            }
        }
    }

    if((pEdidData[0] & _BIT0) != _BIT0)
    {
        DebugMessageHDMITx("[HDMI_TX] Scramble Check Sink 0x21 != 0x01\n");

        EDID_SET_HDMI_MAC_TX0_SINK_SCRAMBLE_STATUS(_HDMI_TX_SINK_SCRAMBLE_OFF);

        if(EDID_GET_HDMI_MAC_TX0_SCRAMBLE_RETRY() == _FALSE)
        {
            DebugMessageHDMITx("[HDMI_TX] Scramble Check Reset SCDC and disable Scramble\n");

            ucScdcSetValue = 0x00;
        }
        else
        {
            ucScdcSetValue = 0x03;
        }

        // Delay Between SCDC Read and Write Access
        HDMITX_MDELAY(2);

        subEdidAddr[0] = 0x20;
        subEdidAddr[1] = ucScdcSetValue;
        if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
        {
            if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
            {
                if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
                {
                    FatalMessageHDMITx("[HDMI_TX] Scramble Check w-disable Fail=%d\n", (UINT32)ucScdcSetValue);
                }
            }
        }

        EDID_SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_STANDBY);

        ScalerTimerReactiveTimerEvent(10000, _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_CHECK);
    }
    else
    {
        DebugMessageHDMITx("[HDMI_TX] Scramble Check OK!\n");

        EDID_SET_HDMI_MAC_TX0_SINK_SCRAMBLE_STATUS(_HDMI_TX_SINK_SCRAMBLE_ON);

        EDID_SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_STANDBY);

        ScalerTimerReactiveTimerEvent(10000, _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_CHECK);
    }
}



//--------------------------------------------------
// Description  : Hdmi Tx SCDC Set TMDS Config
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0SCDCSetTMDSConfiguration(void)
{
    // Scramble reset
    EDID_SET_HDMI_MAC_TX0_SINK_SCRAMBLE_STATUS(_HDMI_TX_SINK_NO_READ);
    EDID_SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_INITIAL);

    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_CHECK);
    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_ENABLE);
    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_DISABLE);

    if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() <= 3400)
    {
        EDID_CLR_HDMI_MAC_TX0_SCRAMBLE_RETRY();

        // Disable Scramble Circuit Before MAC Output Content Stream
        if(GET_HDMI_MAC_TX0_SUPPORT() == _TRUE)
        {
            rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT5), _BIT6);
        }
        else
        {
            rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT5), 0x00);
        }

        // Tx Disable Sink Scramble_En BOOLEAN in SCDC
        ScalerHdmiMacTx0SCDCScrambleDisable();

        EDID_SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_STANDBY);
    }
    else
    {
        if(ScalerHdmiMacTxEdidGetFeature((EnumOutputPort)_TX0, _SCDC_PRESENT) == _TRUE)
        {
            if(EDID_GET_HDMI_MAC_TX0_SCRAMBLE_RETRY() == _FALSE)
            {
                #ifdef NOT_USED_NOW//Directly enter mode SCRAMBLE_ENABLE
                ScalerTimerActiveTimerEvent(300, _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_ENABLE);
                #endif
                DebugMessageHDMITx("[HDMI_TX] Set SCDC TMDS Config. : Enable SCRAMBLE ENABLE EVENT\n");
            }
            else
            {
                #ifdef NOT_USED_NOW//Directly enter mode SCRAMBLE_ENABLE
                ScalerTimerActiveTimerEvent(20, _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_ENABLE);
                #endif
                DebugMessageHDMITx("[HDMI_TX] Set SCDC TMDS Config. : Enable SCRAMBLE ENABLE EVENT\n");
            }
            #ifdef NOT_USED_NOW// Directly enter mode SCRAMBLE_ENABLE
            EDID_SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_STANDBY);
            #else
            EDID_SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_SCRAMBLE_ENABLE);
            #endif
        }
        else
        {
            DebugMessageHDMITx("[HDMI_TX] Set SCDC TMDS Config. : Directly En-Sreamble Content Stream\n");

            // [FIXME] wait den_end event
            //ScalerTimerDelayXms(20);
            HDMITX_DTG_Wait_Den_Stop_Done(); // Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,1);

#ifdef CONFIG_MACH_RTD2801_ZEBU //[IC] #ifdef CONFIG_ENABLE_ZEBU_BRING_UP
            // Don't enable MAC En-Scramble Module in ZEBU platform
            rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT5), (_BIT6));
            NoteMessageHDMITx("[HDMI_TX] Disable ScrambleEn@SCDCSetTMDSConfiguration\n");
#else // #ifdef CONFIG_ENABLE_ZEBU_BRING_UP
            // Enable MAC En-Scramble Module in ZEBU platform
            rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT5), (_BIT6 | _BIT5));
#endif // #else // #ifdef CONFIG_ENABLE_ZEBU_BRING_UP
        }
    }
}


//--------------------------------------------------
// Description  : Hdmi Tx Disable Sink Scramble
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0SCDCScrambleDisable(void)
{
    UINT8 ucScdcSetValue = 0x00;

    DebugMessageHDMITx("[HDMI_TX] SCDC Communication -> Disable Scramble !!!!\n");

    if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() <= 3400)
    {
        if(ScalerHdmiMacTxEdidGetFeature((EnumOutputPort)_TX0, _SCDC_PRESENT) == _TRUE)
        {
            if(EDID_GET_HDMI_MAC_TX0_STREAM_SOURCE() == _HDMI_TX_SOURCE_NONE)
            {

                subEdidAddr[0] = 0x20;
                subEdidAddr[1] = ucScdcSetValue;
                if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
                {
                    if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
                    {
                        if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
                        {
                            DebugMessageHDMITx("[HDMI_TX] Disable Scramble write Fail!@%d\n", (UINT32)ucScdcSetValue);
                        }
                    }
                }

                EDID_SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_STANDBY);

                ScalerTimerReactiveTimerEvent(10000, _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_DISABLE);
            }
            else
            {
                pEdidData[0] = 0x00;
                subEdidAddr[0] = 0x21;
                // Read SCDC 0xA8 0x20 0x21 = 0x00
                if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _FALSE)
                {
                    if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _FALSE)
                    {
                        if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _FALSE)
                        {
                            DebugMessageHDMITx("[HDMI_TX] Disable Scramble Read 0x21 Fail\n");
                        }
                    }
                }

                if((pEdidData[0] & _BIT0) != 0x00)
                {
                    // Delay Between SCDC Read and Write Access
                    HDMITX_MDELAY(2);

                    subEdidAddr[0] = 0x20;
                    subEdidAddr[1] = ucScdcSetValue;
                    if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
                    {
                        if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
                        {
                            if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
                            {
                                DebugMessageHDMITx("[HDMI_TX] Disable Scramble write Fail!@%d\n", (UINT32)ucScdcSetValue);
                            }
                        }
                    }

                    DebugMessageHDMITx("[HDMI_TX] Retry Write SCDC 0x20 = 0x00@%x\n", (UINT32)pEdidData[0]);

                    EDID_SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_STANDBY);

                    ScalerTimerReactiveTimerEvent(10000, _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_DISABLE);
                }
                else
                {
                    EDID_SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_STANDBY);

                    DebugMessageHDMITx("[HDMI_TX] Disable Scramble OK\n");
                }
            }
        }
    }
}
#endif // End of #if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)

void ScalerHdmiMacTx0SCDCReset(void)
{
    subEdidAddr[0] = _HDMI21_SCDC_OFFSET_31;
    subEdidAddr[1] = 0;
    if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
    {
        if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
        {
            if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI_TX0_IIC) == _FALSE)
            {
                DebugMessageHDMITx("[HDMI_TX] reset 0x31 Fail!n");
            }
        }
    }
}

UINT8 ScalerHdmiMacTx0SCDCRead(UINT8 scdc_offset)
{
    pEdidData[0] = 0x00;
    subEdidAddr[0] = scdc_offset;
    if(ScalerHdmiMacTxEdidGetFeature((EnumOutputPort)_TX0, _SCDC_PRESENT) == _TRUE) {
        if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _FALSE)
        {
            if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _FALSE)
            {
                if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _FALSE)
                {
                    DebugMessageHDMITx("[HDMI_TX] Disable Scramble Read 0x35 Fail\n");
                }
            }
        }
    }
    return pEdidData[0];
}

UINT8 needNoBlackBorder(UINT16 out_hactive, UINT16 out_vactive)
{
#ifdef HDMITX_SW_CONFIG
    if(GetOutputSafeMode() == TRUE)
        return FALSE;
#endif
    if(((timing_info.h_act_len == 2560) && (timing_info.v_act_len == 1440)) &&
        ((out_hactive == 3840) && (out_vactive == 2160)))
        return TRUE;
    if(((timing_info.h_act_len == 1920) && (timing_info.v_act_len == 1080)) &&
        ((out_hactive == 3840) && (out_vactive == 2160)))
        return TRUE;
    if(((timing_info.h_act_len == 1920) && (timing_info.v_act_len == 1080)) &&
        ((out_hactive == 2560) && (out_vactive == 1440)))
        return TRUE;
    return FALSE;
}

void calcSourceOutputPixelClk(UINT16 txHtotal,UINT16 txVtotal)
{
    UINT32 u32TmpValue = 0;

    u32TmpValue = ((UINT32)(txHtotal) * (txVtotal));
    u32TmpValue = (u32TmpValue/100);
    u32TmpValue = (u32TmpValue*(timing_info.v_freq));
    SourceOutputPixelClk = u32TmpValue;
}
UINT16 hdmitx_cal_ratio(UINT16 dividend, UINT16 dividor)
{
    UINT32 ratio;

    ratio = (UINT32)dividend * 10000;
    ratio /= (UINT32)dividor;

    return (UINT16)ratio;
}

#define V_RATIO_MARGIN 10000U
UINT32 hdmitx_multpy(UINT16 x, UINT16 y)
{
	UINT32 getVal;
	getVal = x;
	getVal = getVal * y;
	return getVal;
}
UINT8 hdmitx_check_vratio_margin(UINT16 vratio_margin, UINT16 tx_vtotal)
{
    if(hdmitx_multpy(vratio_margin, tx_vtotal) <= V_RATIO_MARGIN)
        return 1;
    else
        return 0;
}

UINT8 hdmitxSourceMoeCompareRatio(TX_TIMING_INDEX time_mode, UINT8 dtd_flag)
{
    UINT16 in_ratio;
    UINT16 out_ratio;
    UINT16 out_hactive;
    UINT16 out_vactive;
    UINT16 out_vtotal;
    UINT16 out_vstart;


    if(dtd_flag) {
        out_hactive = edid_timing.h_act;
        out_vactive = edid_timing.v_act;
        out_vtotal = edid_timing.v_act + edid_timing.v_blank;
        out_vstart = edid_timing.v_blank -edid_timing.v_front;
    } else {
        out_hactive = tx_timing_table.h.active;
        out_vactive = tx_timing_table.v.active;
        out_vtotal = tx_timing_table.v.active + tx_timing_table.v.blank;
        out_vstart = tx_timing_table.v.back + tx_timing_table.v.sync;
    }

    in_ratio = hdmitx_cal_ratio(timing_info.v_act_len, timing_info.v_total);
    out_ratio = hdmitx_cal_ratio(out_vactive, out_vtotal);
    if(in_ratio > out_ratio) {
        //check codition2:  v ratio Tx >= Rx
        if(needNoBlackBorder(out_hactive, out_vactive)) {
            DebugMessageHDMITx("[hdmitx]check no black border condition.\n");
            if(hdmitx_check_vratio_margin((in_ratio-out_ratio), out_vtotal)) {
                FatalMessageHDMITx("[hdmitx]vratio margin ok.\n");
                cur_vratio = 0;//in_ratio - out_ratio;
            } else {
                FatalMessageHDMITx("[hdmitx]vratio margin fail.\n");
                //return FALSE;
                cur_vratio = 0xffff;
            }
        } else {
            //check codition2:  v ratio Tx >= Rx
            cur_vratio = 0xffff;
            return FALSE;
        }
    } else {
        cur_vratio = out_ratio - in_ratio;
    }

    in_ratio = hdmitx_cal_ratio(timing_info.h_act_len, timing_info.v_act_len);
    out_ratio = hdmitx_cal_ratio(out_hactive, out_vactive);
    if(in_ratio > out_ratio) {
        FatalMessageHDMITx("[hdmitx]h/v ratio fail[%d,%d].\n",(UINT32)in_ratio,(UINT32)out_ratio);
        return FALSE;
    }

    if(timing_info.run_vrr) {
        in_ratio = hdmitx_cal_ratio(timing_info.v_act_sta, timing_info.v_total);
        out_ratio = hdmitx_cal_ratio(out_vstart, out_vtotal);
        if(out_ratio > in_ratio) {
            FatalMessageHDMITx("[hdmitx]vrr ratio need adjust[%d,%d].\n",(UINT32)in_ratio,(UINT32)out_ratio);
            //return FALSE;
        }
    }

    return TRUE;
}

#ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
UINT16 edid_dtd_vfeq = 0;
UINT8 DTDTimingMatch(TX_TIMING_INDEX time_mode)
{
    UINT32 u32TmpValue = 0;
    BOOLEAN boolTmpValue = 0;

    txHtotal = edid_timing.h_act + edid_timing.h_blank;
    txVtotal = edid_timing.v_act + edid_timing.v_blank;
    calcSourceOutputPixelClk(txHtotal,txVtotal);
    edid_dtd_vfeq = calcDTDTimingvFreq();

    // 1.compare time_mode vfreq == rx  vfreq
    // 2.compare timie mode active >= rx active
    // 3.compare timing mode vfrq == dtd vfreq
    // 4.compare timing mode active == dtd active
    // 5.compare rx and dtd ratio
    // 6.compare SourceOutputPixelClk > rx pixelClk
    u32TmpValue = (UINT32)ABS(tx_timing_table.vFreq, timing_info.v_freq/10) * 1000; // v_freq diff < 0.9%
    boolTmpValue = ((u32TmpValue / tx_timing_table.vFreq) < HDMITX_CLK_DIFF);
    boolTmpValue = boolTmpValue &&(tx_timing_table.h.active >= timing_info.h_act_len);
    boolTmpValue = boolTmpValue && (tx_timing_table.v.active >= timing_info.v_act_len);
    u32TmpValue = (UINT32)ABS(tx_timing_table.vFreq, edid_dtd_vfeq) * 1000; // v_freq diff < 0.9%
    boolTmpValue = boolTmpValue && ((u32TmpValue / tx_timing_table.vFreq) < HDMITX_CLK_DIFF);
    boolTmpValue = boolTmpValue &&(tx_timing_table.h.active == edid_timing.h_act);
    boolTmpValue = boolTmpValue && (tx_timing_table.v.active == edid_timing.v_act);
    boolTmpValue = boolTmpValue && hdmitxSourceMoeCompareRatio(time_mode, 1);
    boolTmpValue = boolTmpValue && (SourceOutputPixelClk >= timing_info.pll_pixelclockx1024);
    if(boolTmpValue) {
        DebugMessageHDMITx("[HDMITX]DTD match [%d/%d] ", (UINT32)edid_timing.h_act, (UINT32)edid_timing.v_act);
        DebugMessageHDMITx("[%d/%d] ", (UINT32)edid_timing.h_blank, (UINT32)edid_timing.v_blank);
        DebugMessageHDMITx("[%d/%d] ", (UINT32)edid_timing.h_front, (UINT32)edid_timing.v_front);
        DebugMessageHDMITx("[%d/%d] ", (UINT32)edid_timing.h_sync, (UINT32)edid_timing.v_sync);
        DebugMessageHDMITx("[%d] \n", (UINT32)edid_timing.pixelClk);
        if(edid_timing.h_act > dtd_timing.h_act)
            memcpy(&dtd_timing, &edid_timing, sizeof(hdmi_timing_check_type));
        dtd_match_flag = TRUE;
        return TRUE;
    }

    return FALSE;
}

UINT8 ScalerHdmiMacTx0RepeaterEdidMatchDTDTiming(EDID_BLOCK_INDEX edidBlock, TX_TIMING_INDEX time_mode)
{
    UINT16 u16TmpValue = 0;
    UINT16 usDetailTimeOffset= 0x00;
    UINT16 usExtDetailTimeBase;
    UINT16 ucDTDAddress;
    UINT8 ucExtDtdNum;

    usExtDetailTimeBase = 0x82;
    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(_TX0);

    if(edidBlock == EDID_BLOCK_0){
        ucDTDAddress = 0x36;
        ucExtDtdNum = 4;
    }else if(edidBlock == EDID_BLOCK_1){
        usExtDetailTimeBase = usExtTagBaseAddr + 0x2;//modify ExtensionTag Block DTD
        usDetailTimeOffset = usExtTagBaseAddr + pucDdcRamAddr[usExtDetailTimeBase];
        ucDTDAddress = usDetailTimeOffset;
        ucExtDtdNum = (usExtTagBaseAddr + 0x7f - ucDTDAddress) / 18;
    }else if(edidBlock == EDID_EXT_DID_BLOCK_1){
        usExtDetailTimeBase = pusCtaDataBlockAddr[_DID_DTDDB];
        ucDTDAddress = usExtDetailTimeBase + 3;
        ucExtDtdNum = pucDdcRamAddr[usExtDetailTimeBase + 2] / 20;
    }else{
        DebugMessageHDMITx("[HDMI_TX][Modify] DTD[%d] Not Support Yet!!\n", (UINT32)edidBlock);
        return FALSE;
    }

    DebugMessageHDMITx("[HDMI_TX][Modify] DTD[%d] Num=%d",(UINT32)edidBlock, (UINT32)ucExtDtdNum);
    DebugMessageHDMITx("@ADDR[%x(/%x)]\n",(UINT32)ucDTDAddress, (UINT32)GET_HDMI_MAC_TX0_EDID_EXT_DTD_START_ADDR());
    for(u16TmpValue = 0; u16TmpValue < ucExtDtdNum; u16TmpValue++)
    {
        if(edidBlock == EDID_EXT_DID_BLOCK_1){
            get20ByteDTDTimingInfo(ucDTDAddress);
            edid_timing.h_front = ((pucDdcRamAddr[ucDTDAddress + 9] & 0x7f) << 8);
            edid_timing.h_front = edid_timing.h_front | (pucDdcRamAddr[ucDTDAddress + 8]);
            edid_timing.h_front += 1;

            edid_timing.h_sync = ((pucDdcRamAddr[ucDTDAddress + 11]) << 8);
            edid_timing.h_sync = edid_timing.h_sync | (pucDdcRamAddr[ucDTDAddress + 10]);
            edid_timing.h_sync += 1;

            edid_timing.v_front = ((pucDdcRamAddr[ucDTDAddress + 17] & 0x7f) << 8);
            edid_timing.v_front = edid_timing.v_front | (pucDdcRamAddr[ucDTDAddress + 16]);
            edid_timing.v_front += 1;

            edid_timing.v_sync = ((pucDdcRamAddr[ucDTDAddress + 19]) << 8);
            edid_timing.v_sync = edid_timing.v_sync | (pucDdcRamAddr[ucDTDAddress + 18]);
            edid_timing.v_sync += 1;            
            ucDTDAddress = ucDTDAddress + 20;
        }else{
            getDTDTimingInfo(ucDTDAddress);

            edid_timing.h_front = ((pucDdcRamAddr[ucDTDAddress + 11] & 0xC0) << 2) | (pucDdcRamAddr[ucDTDAddress + 8]);
            edid_timing.h_sync = ((pucDdcRamAddr[ucDTDAddress + 11] & 0x30) << 4) | (pucDdcRamAddr[ucDTDAddress + 9]);
            edid_timing.v_front = ((pucDdcRamAddr[ucDTDAddress + 11] & 0x0C) << 2) | ((pucDdcRamAddr[ucDTDAddress + 10] & 0xF0) >> 4);
            edid_timing.v_sync = ((pucDdcRamAddr[ucDTDAddress + 11] & 0x03) << 4) | (pucDdcRamAddr[ucDTDAddress + 10] & 0x0F);
            ucDTDAddress = ucDTDAddress + 18;

        }
        if(edid_timing.pixelClk && edid_timing.h_act && edid_timing.v_act){
            if(!needNoBlackBorder(edid_timing.h_act, edid_timing.v_act) && DTDTimingMatch(time_mode))
                return TRUE;
        }

    }
    return FALSE;
}
#endif


UINT8 judgeNoCTATimingIndex(TX_TIMING_INDEX time_mode)
{
    // 4k60 noCTA861
    if(time_mode >= TX_TIMING_4K2KP60_CVT && time_mode <= TX_TIMING_4K2KP60_CVT_RBv2)
        return TRUE;
    // 4k120 noCTA861
    if(time_mode >= TX_TIMING_4K2KP120_CVT && time_mode <= TX_TIMING_4K2KP120_CVT_RBv2)
        return TRUE;
    return FALSE;
}

UINT8 ScalerHdmiMacTx0RepeaterGetTimingMatchSourceMode(TX_OUT_MODE_SELECT_TYPE mode,EDID_TIMING_DB edid_timing_type,TX_TIMING_INDEX time_mode)
{
    UINT32 u32TmpValue = 0;
    BOOLEAN boolTmpValue = 0;
    if((time_mode >= TX_TIMING_NUM))
        return _FALSE;
    if(ScalerHdmiTxPhy_GetTxTimingTable(time_mode, &tx_timing_table) !=  0){
        FatalMessageHDMITx("[HDMITX] Get tx_cea_timing_table Fail\n");
        return _FALSE;
    }
    if(mode == TX_OUT_TEST_MODE){//test mode only support 2k60 or 4k60
        if((tx_timing_table.h.active != 1920 || tx_timing_table.v.active != 1080)
            && (tx_timing_table.h.active != 3840 || tx_timing_table.v.active != 2160))
            return _FALSE;
    }
    if(ScalerHdmiMacTxEdidGetFeature((EnumOutputPort)_TX0,_SCDC_PRESENT) == _FALSE)
        pixelClockLimit = TX_OUT_PIXEL_CLOCK_MAX_WO_SCDC;
    else if(ScalerHdmiMacTxEdidGetFeature((EnumOutputPort)_TX0,_MAX_FRL_RATE) == 0)
        pixelClockLimit = ((UINT16)ScalerHdmiMacTxEdidGetFeature((EnumOutputPort)_TX0,_HDMI_MAX_TMDS_CLK) * 50);
    else
        pixelClockLimit = getLimitPixelClkWithDDR();

#ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
    if(edid_timing_type == EDID_TIMING_DTD) {
        if(ScalerHdmiMacTx0RepeaterEdidMatchDTDTiming(EDID_BLOCK_0, time_mode))
            return TRUE;
        else if(
            #ifdef HDMITX_SW_CONFIG
            (GetOutputSafeMode() == TRUE) || 
            #endif
            !needNoBlackBorder(tx_timing_table.h.active, tx_timing_table.v.active))
            return FALSE;
    } else if(edid_timing_type == EDID_TIMING_EXT_DTD) {
        if(ScalerHdmiMacTx0RepeaterEdidMatchDTDTiming(EDID_BLOCK_1, time_mode))
            return TRUE;
        else if(
            #ifdef HDMITX_SW_CONFIG
            (GetOutputSafeMode() == TRUE) || 
            #endif
            !needNoBlackBorder(tx_timing_table.h.active, tx_timing_table.v.active))
            return FALSE;
    }else if(edid_timing_type == EDID_TIMING_DID_DTD) {
        if(ScalerHdmiMacTx0RepeaterEdidMatchDTDTiming(EDID_EXT_DID_BLOCK_1, time_mode))
            return TRUE;
        else if(
            #ifdef HDMITX_SW_CONFIG
            (GetOutputSafeMode() == TRUE) || 
            #endif
            !needNoBlackBorder(tx_timing_table.h.active, tx_timing_table.v.active))
            return FALSE;
    } else {
        // non DTD timing, normal case, non CTA timing 
        if(!needNoBlackBorder(tx_timing_table.h.active, tx_timing_table.v.active) && judgeNoCTATimingIndex(time_mode))
            return FALSE;
    }
#endif

    //SourceOutputPixelClk = ((txoutput.h_total * txoutput.v_total /1000)*(timing_info.v_freq))/1000;
    txHtotal = tx_timing_table.h.active+tx_timing_table.h.blank;
    txVtotal = tx_timing_table.v.active+tx_timing_table.v.blank;
    calcSourceOutputPixelClk(txHtotal,txVtotal);

    // sourcce mode support conditoin:
    // 1. TX timing mode H/V active size >= input active size
    // 2. TX output frame rate == input frame rate
    // 3. inratio > outratio
    // 4. TX output pixel clock >= input pixel clock


    if((timing_info.v_freq < 2300)|| (timing_info.v_freq > 14500))
        DebugMessageHDMITx("[HDMITX][HACK] ABNORMAL VFREQ[%x]\n", (UINT32)timing_info.v_freq);

    u32TmpValue = (UINT32)ABS(tx_timing_table.vFreq, timing_info.v_freq/10) * 1000; // v_freq diff < 0.9%
    boolTmpValue = ((u32TmpValue / tx_timing_table.vFreq) < HDMITX_CLK_DIFF);
    boolTmpValue = boolTmpValue &&(tx_timing_table.h.active >= timing_info.h_act_len);
    boolTmpValue = boolTmpValue && (tx_timing_table.v.active >= timing_info.v_act_len);
    boolTmpValue = boolTmpValue && hdmitxSourceMoeCompareRatio(time_mode, 0);
    boolTmpValue = boolTmpValue && (SourceOutputPixelClk >= timing_info.pll_pixelclockx1024);
    return boolTmpValue;
}


void printTimingMatchLog(UINT8 ret, TX_TIMING_INDEX time_mode)
{//for reduce dseg
#if 0
        InfoMessageHDMITx("[%d]", (UINT32)ret );
        InfoMessageHDMITx("[%d",  (UINT32)time_mode);
        InfoMessageHDMITx("=%d/%d]",(UINT32)tx_timing_table.video_code1, (UINT32)tx_timing_table.video_code2);
        InfoMessageHDMITx(", %d", (UINT32)timing_info.h_act_len);
        InfoMessageHDMITx("x%d", (UINT32)timing_info.v_act_len);
        InfoMessageHDMITx("@%d(/10),",  (UINT32)timing_info.v_freq/10);
        InfoMessageHDMITx("PClk=%x",  (UINT32)timing_info.pll_pixelclockx1024);
        InfoMessageHDMITx(" -> %d", (UINT32)tx_timing_table.h.active);
        InfoMessageHDMITx("x%d", (UINT32)tx_timing_table.v.active);
        InfoMessageHDMITx("@%d,",  (UINT32)tx_timing_table.vFreq);
        InfoMessageHDMITx("PClk=%x",  (UINT32)SourceOutputPixelClk);
        InfoMessageHDMITx("(/%x)\n",  (UINT32)pixelClockLimit);
#else
    InfoMessageHDMITx("[%d][%d=%d/%d], %dx%d@%d(/10)\n", (UINT32)ret,  (UINT32)time_mode,(UINT32)tx_timing_table.video_code1, (UINT32)tx_timing_table.video_code2,
(UINT32)timing_info.h_act_len, (UINT32)timing_info.v_act_len,  (UINT32)timing_info.v_freq/10);
    InfoMessageHDMITx("PClk=%x-> %dx%d@%d,PClk=%x(/%x)\n",  (UINT32)timing_info.pll_pixelclockx1024, (UINT32)tx_timing_table.h.active, 
    (UINT32)tx_timing_table.v.active,  (UINT32)tx_timing_table.vFreq,  (UINT32)SourceOutputPixelClk,  (UINT32)pixelClockLimit);

#endif
}

UINT8 isSpecialTV5And2p5K(void)
{
    if(g_bHdmiMacTx0FindSpecialTv5 && ((timing_info.h_act_len == 2560) && (timing_info.v_act_len == 1440)))
        return TRUE;
    else
        return FALSE;
}

void calcRgbTimingPriority(void)
{
        UINT16 u16TmpValue2 = 0;
        UINT16 u16TmpValue = 0;
        if(ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE)){
            u16TmpValue = tx_Max_rgb_timing_table.h.active + tx_Max_rgb_timing_table.h.blank;
            u16TmpValue2 = tx_Max_rgb_timing_table.v.active + tx_Max_rgb_timing_table.v.blank;
            u32RgbBandwidth =  (UINT32)u16TmpValue * (UINT32)u16TmpValue2 / 100;
            u32RgbBandwidth = u32RgbBandwidth * (UINT32)timing_info.v_freq / 1000;
            u32RgbBandwidth = u32RgbBandwidth * 24;
            u32RgbBandwidth = u32RgbBandwidth / 8;
            u32RgbBandwidth = u32RgbBandwidth * 9;
            u32BandWidth = bandwidth_table[ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE)].u32BandWidth;
        }else{
            u32RgbBandwidth = rgbMaxPixelClock1024 /100000;
            u32BandWidth = pixelClockLimit;
        }
        if((u32RgbBandwidth * 3 / 2 <= u32BandWidth) && (ScalerHdmiMacTx0EdidGetFeature(_DC36_SUPPORT) == _TRUE)){
            enRgbTimingPriority =  TX_OUT_RGB_12B;
        }else if( (u32RgbBandwidth * 5 / 4 <= u32BandWidth) && (ScalerHdmiMacTx0EdidGetFeature(_DC30_SUPPORT) == _TRUE)){
            enRgbTimingPriority =  TX_OUT_RGB_10B;
        }else if(u32RgbBandwidth   <= u32BandWidth){
            enRgbTimingPriority = TX_OUT_RGB_8B;
        }else{
            enRgbTimingPriority = TX_OUT_TIMING_PRIORITY_MAX_NUM;
        }
        FatalMessageHDMITx("[HDMITX]rgb match %d\n",(UINT32)enRgbTimingPriority);
}

void calc420TimingPriority(void)
{
        UINT16 u16TmpValue2 = 0;
        UINT16 u16TmpValue = 0;
        if(ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE)){
            u16TmpValue = tx_Max_420_timing_table.h.active + tx_Max_420_timing_table.h.blank;
            u16TmpValue2 = tx_Max_420_timing_table.v.active + tx_Max_420_timing_table.v.blank;
            u32Y420Bandwidth =  (UINT32)u16TmpValue * (UINT32)u16TmpValue2 / 100;
            u32Y420Bandwidth = u32Y420Bandwidth * (UINT32)timing_info.v_freq / 1000;
            u32Y420Bandwidth = u32Y420Bandwidth / 2;
            u32Y420Bandwidth = u32Y420Bandwidth * 24;
            u32Y420Bandwidth = u32Y420Bandwidth / 8;
            u32Y420Bandwidth = u32Y420Bandwidth * 9;
            u32BandWidth = bandwidth_table[ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE)].u32BandWidth;
        }else{
            u32Y420Bandwidth = y420MaxPixelClock1024 /10;
            u32Y420Bandwidth = u32Y420Bandwidth / 20000;
            u32BandWidth = pixelClockLimit;
        }
        if((u32Y420Bandwidth * 3 / 2 <= u32BandWidth) && (ScalerHdmiMacTx0EdidGetFeature(_DC36_420_SUPPORT) == _TRUE)){
            enY420TimingPriority= TX_OUT_Y420_12B;
        }else if((u32Y420Bandwidth * 5 / 4 <= u32BandWidth) && (ScalerHdmiMacTx0EdidGetFeature(_DC30_420_SUPPORT) == _TRUE)){
            enY420TimingPriority= TX_OUT_Y420_10B;
        }else if(u32Y420Bandwidth  <= u32BandWidth){
            enY420TimingPriority= TX_OUT_Y420_8B;
        }else{
            enY420TimingPriority= TX_OUT_TIMING_PRIORITY_MAX_NUM;
        }
        FatalMessageHDMITx("[HDMITX]420 match %d\n",(UINT32)enY420TimingPriority);
}

void calcDefault444TimingPriority(void)
{ 
        if(ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE)){
            u32RgbBandwidth = timing_info.pll_pixelclockx1024 / 1000;
            u32RgbBandwidth = u32RgbBandwidth * 24;
            u32RgbBandwidth = u32RgbBandwidth / 8;
            u32RgbBandwidth = u32RgbBandwidth * 9;
            u32BandWidth = bandwidth_table[ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE)].u32BandWidth;
        }else{
            u32RgbBandwidth = timing_info.pll_pixelclockx1024 /100000;
            u32BandWidth = pixelClockLimit;
        }
        if((u32RgbBandwidth * 3 / 2 <= u32BandWidth) && (ScalerHdmiMacTx0EdidGetFeature(_DC36_SUPPORT) == _TRUE)){
            enRgbTimingPriority =  TX_OUT_RGB_12B;
        }else if( (u32RgbBandwidth * 5 / 4 <= u32BandWidth) && (ScalerHdmiMacTx0EdidGetFeature(_DC30_SUPPORT) == _TRUE)){
            enRgbTimingPriority =  TX_OUT_RGB_10B;
        }else if(u32RgbBandwidth   <= u32BandWidth){
            enRgbTimingPriority = TX_OUT_RGB_8B;
        }else{
            enRgbTimingPriority = TX_OUT_TIMING_PRIORITY_MAX_NUM;
        }
        FatalMessageHDMITx("[HDMITX]default 444 match %d\n",(UINT32)enRgbTimingPriority);
}

void calcDefault420TimingPriority(void)
{
        if(ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE)){
            u32Y420Bandwidth = timing_info.pll_pixelclockx1024 / 1000;
            u32Y420Bandwidth = u32Y420Bandwidth * 24;
            u32Y420Bandwidth = u32Y420Bandwidth / 2;
            u32Y420Bandwidth = u32Y420Bandwidth / 8;
            u32Y420Bandwidth = u32Y420Bandwidth * 9;
            u32BandWidth = bandwidth_table[ScalerHdmiMacTx0EdidGetFeature(_MAX_FRL_RATE)].u32BandWidth;
        }else{
            u32Y420Bandwidth = timing_info.pll_pixelclockx1024 /10;
            u32Y420Bandwidth = u32Y420Bandwidth / 20000;
            u32BandWidth = pixelClockLimit;
        }
        if((u32Y420Bandwidth * 3 / 2 <= u32BandWidth) && (ScalerHdmiMacTx0EdidGetFeature(_DC36_420_SUPPORT) == _TRUE)){
            enY420TimingPriority= TX_OUT_Y420_12B;
        }else if((u32Y420Bandwidth * 5 / 4 <= u32BandWidth) && (ScalerHdmiMacTx0EdidGetFeature(_DC30_420_SUPPORT) == _TRUE)){
            enY420TimingPriority= TX_OUT_Y420_10B;
        }else if(u32Y420Bandwidth  <= u32BandWidth){
            enY420TimingPriority= TX_OUT_Y420_8B;
        }else{
            enY420TimingPriority= TX_OUT_TIMING_PRIORITY_MAX_NUM;
        }
        FatalMessageHDMITx("[HDMITX]default 420 match %d\n",(UINT32)enY420TimingPriority);
}

void setTxOutPutTiming(TX_OUT_TIMING_TYPE emOutType){
    if(TX_OUT_RGB_TIMING == emOutType){
            time_mode = max_rgb_resolution_time_mode;
            tx_output_colorSpace = HDMITX_COLOR_RGB;
            tx_output_colordepth = outputDepth_table[enRgbTimingPriority].u8OutputDepth;
            g_TxPixelClock1024 = rgbMaxPixelClock1024;
            if(GET_ADJUST_RGB_VTOTAL()) {
                SET_ADJUST_VTOTAL_FLAG(TRUE);
                SET_ADJUST_OUTPUT_VTOTAL(GET_ADJUST_RGB_VTOTAL());
            }
    }else if(TX_OUT_Y420_TIMING == emOutType){
        time_mode = max_420_resolution_time_mode;
        tx_output_colorSpace = HDMITX_COLOR_YUV420;
        tx_output_colordepth = outputDepth_table[enY420TimingPriority].u8OutputDepth;
        g_TxPixelClock1024 = y420MaxPixelClock1024;
        if(GET_ADJUST_420_VTOTAL()) {
            SET_ADJUST_VTOTAL_FLAG(TRUE);
            SET_ADJUST_OUTPUT_VTOTAL(GET_ADJUST_420_VTOTAL());
        }
    }
}

void decideTxOutputTimingPriority(void)
{
    enY420TimingPriority = TX_OUT_TIMING_PRIORITY_MAX_NUM;
    enRgbTimingPriority = TX_OUT_TIMING_PRIORITY_MAX_NUM;

    if(max_rgb_resolution_time_mode != 0)
    {
        calcRgbTimingPriority();
    }
    if(max_420_resolution_time_mode != 0){
        calc420TimingPriority();
    }
    if(tx_Max_420_timing_table.h.active > tx_Max_rgb_timing_table.h.active)
    {//resolution :420  > rgb
        setTxOutPutTiming(TX_OUT_Y420_TIMING);
    }else if(tx_Max_420_timing_table.h.active < tx_Max_rgb_timing_table.h.active)
    {//resolution :rgb  > 420
        if(enRgbTimingPriority != TX_OUT_TIMING_PRIORITY_MAX_NUM){
            setTxOutPutTiming(TX_OUT_RGB_TIMING);
        }else{//mean rgb 8 bit > max frl,need turn to yuv420
            FatalMessageHDMITx("[HDMITX] rgb not match %d %d\n", (UINT32)max_rgb_resolution_time_mode,(UINT32)enRgbTimingPriority);
            time_mode = TX_TIMING_DEFAULT;
            tx_output_colorSpace = timing_info.colorspace;
            tx_output_colordepth = timing_info.colordepth;
            g_TxPixelClock1024 = timing_info.pll_pixelclockx1024;
        }
    }
    else{//resolution :rgb = yuv420 && not zero
        if(enRgbTimingPriority < enY420TimingPriority){//yuv420 PRIORITY low rgb,out rgb
            setTxOutPutTiming(TX_OUT_RGB_TIMING);
        }else{//yuv420 PRIORITY high rgb or rgb is max,out yuv420
            setTxOutPutTiming(TX_OUT_Y420_TIMING);
        }
    }
}

void decideTxOutputTimingAndPixelClk(void)
{
        FatalMessageHDMITx("[HDMITX]match idx rgb/420[%d,%d]\n", (UINT32)max_rgb_resolution_time_mode, (UINT32)max_420_resolution_time_mode);
        decideTxOutputTimingPriority();
        FatalMessageHDMITx("[HDMITX]match idx [%d,%d]\n", (UINT32)tx_output_colorSpace, (UINT32)tx_output_colordepth);
}

void decideDefaultOutputTimingPriority(void)
{
    enY420TimingPriority = TX_OUT_TIMING_PRIORITY_MAX_NUM;
    enRgbTimingPriority = TX_OUT_TIMING_PRIORITY_MAX_NUM;

    FatalMessageHDMITx("[HDMITX][defult max]444:%d,420:%d\n", (UINT32)tx_Max_rgb_timing_table.h.active, (UINT32)tx_Max_420_timing_table.h.active);

    if(tx_Max_rgb_timing_table.h.active > tx_Max_420_timing_table.h.active)
        calcDefault444TimingPriority();
    else if(tx_Max_rgb_timing_table.h.active < tx_Max_420_timing_table.h.active)
        calcDefault420TimingPriority();
    else {
        calcDefault444TimingPriority();
        calcDefault420TimingPriority();
    }

    if(enRgbTimingPriority < enY420TimingPriority){//yuv420 PRIORITY low rgb,out rgb
        tx_output_colorSpace = HDMITX_COLOR_RGB;
        tx_output_colordepth = outputDepth_table[enRgbTimingPriority].u8OutputDepth;
        g_TxPixelClock1024 = timing_info.pll_pixelclockx1024;
    }else{//yuv420 PRIORITY high rgb or rgb is max,out yuv420
        tx_output_colorSpace = HDMITX_COLOR_YUV420;
        tx_output_colordepth = outputDepth_table[enY420TimingPriority].u8OutputDepth;
        g_TxPixelClock1024 = timing_info.pll_pixelclockx1024;
    }

}

void updataMaxResolution(TX_OUT_TIMING_TYPE emOutType)
{
    UINT16 tmpPixelClk;

    if(TX_OUT_RGB_TIMING == emOutType){
        tmpPixelClk = SourceOutputPixelClk/100000;
        if(tmpPixelClk <= pixelClockLimit){
           if((tx_timing_table.h.active > tx_Max_rgb_timing_table.h.active) ||
            ((tx_timing_table.h.active == tx_Max_rgb_timing_table.h.active) && needNoBlackBorder(tx_timing_table.h.active, tx_timing_table.v.active) && (min_444_vratio > cur_vratio))){
                max_rgb_resolution_time_mode = time_mode;
                rgbMaxPixelClock1024 = SourceOutputPixelClk;
                min_444_vratio = cur_vratio;
                FatalMessageHDMITx("[HDMITX][444]h:%d,vratio:%d\n", (UINT32)tx_timing_table.h.active, (UINT32)min_444_vratio);
            }
        }
    }else{
        tmpPixelClk = SourceOutputPixelClk/200000;
        if(tmpPixelClk <= pixelClockLimit){
               if((tx_timing_table.h.active > tx_Max_420_timing_table.h.active) ||
            ((tx_timing_table.h.active == tx_Max_420_timing_table.h.active) && needNoBlackBorder(tx_timing_table.h.active, tx_timing_table.v.active) && (min_420_vratio > cur_vratio))){
                    max_420_resolution_time_mode = time_mode;
                    y420MaxPixelClock1024 = SourceOutputPixelClk;
                    min_420_vratio = cur_vratio;
                    FatalMessageHDMITx("[HDMITX][420]h:%d,vratio:%d\n", (UINT32)tx_timing_table.h.active, (UINT32)min_420_vratio);
                }
        }
    }
}

#ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
void decideTxDTDTiming(void)
{
    if(GET_ADJUST_VTOTAL_FLAG())
        edid_dtd_flag = 0;
    else if(tx_output_colorSpace == HDMITX_COLOR_YUV420)
        edid_dtd_flag = 0;
    else if((max_push_list_index != 0xff) && (repeater_support_timing_table.tx_index[max_push_list_index] == time_mode)&&((repeater_support_timing_table.db_type[max_push_list_index].db_type== EDID_TIMING_DTD) || (repeater_support_timing_table.db_type[max_push_list_index].db_type== EDID_TIMING_EXT_DTD) || (repeater_support_timing_table.db_type[max_push_list_index].db_type== EDID_TIMING_DID_DTD)))
        edid_dtd_flag = 1;
    else
        edid_dtd_flag = 0;
    DebugMessageHDMITx("[hdmitx]tx_index/db_type[%d/%d] ", (UINT32)repeater_support_timing_table.tx_index[max_push_list_index], (UINT32)repeater_support_timing_table.db_type[max_push_list_index].db_type);
    FatalMessageHDMITx("[HDMITX]dtd_flag[%d]\n", (UINT32)edid_dtd_flag);
}
#endif

// 1.ajust the max out vtotal, in_ratio(in_vact/in_vtotal) <= out_ratio(out_vact/out_vtotal)
// 2. if below one line, and in ratio < out_ratio, need do out_vtotal+1 to let in_ratio > out_ratio to void black border
UINT16 adjustSourceModeVtotal(UINT16 vactive)
{
    UINT32 u32TmpValue = 0;
    u32TmpValue = (UINT32)vactive * timing_info.v_total;
    u32TmpValue += timing_info.v_act_len;
    u32TmpValue -= 1;
    u32TmpValue /= (UINT32)timing_info.v_act_len;
    return (UINT16)u32TmpValue;
}

void updateRGBSourceModeClock(void)
{
    SET_ADJUST_RGB_VTOTAL(adjustSourceModeVtotal(tx_Max_rgb_timing_table.v.active));
    calcSourceOutputPixelClk(tx_Max_rgb_timing_table.h.active+tx_Max_rgb_timing_table.h.blank, GET_ADJUST_RGB_VTOTAL());
}

void update420SourceModeClock(void)
{
    SET_ADJUST_420_VTOTAL(adjustSourceModeVtotal(tx_Max_420_timing_table.v.active));
    calcSourceOutputPixelClk(tx_Max_420_timing_table.h.active+tx_Max_420_timing_table.h.blank, GET_ADJUST_420_VTOTAL());

}

void  checkRGBSourceModeVtotalVaild(void)
{
    UINT16 u16TmpValue = 0;
    if((tx_Max_rgb_timing_table.v.blank + tx_Max_rgb_timing_table.v.active) > GET_ADJUST_RGB_VTOTAL()) {
        u16TmpValue = tx_Max_rgb_timing_table.v.blank + tx_Max_rgb_timing_table.v.active - GET_ADJUST_RGB_VTOTAL();
        if(tx_Max_rgb_timing_table.v.back <= u16TmpValue) {
            FatalMessageHDMITx("[HDMITX] rgb vtotal[%d] unvaild\n", (UINT32)GET_ADJUST_RGB_VTOTAL());
        }

    }
    return;
}

void check420SourceModeVtotalVaild(void)
{
    UINT16 u16TmpValue = 0;
    if(tx_Max_420_timing_table.v.blank + tx_Max_420_timing_table.v.active > GET_ADJUST_420_VTOTAL()) {
        u16TmpValue = tx_Max_420_timing_table.v.blank + tx_Max_420_timing_table.v.active - GET_ADJUST_420_VTOTAL();
        if(tx_Max_420_timing_table.v.back <= u16TmpValue) {
            FatalMessageHDMITx("[HDMITX] 420 vtotal[%d] unvaild\n", (UINT32)GET_ADJUST_420_VTOTAL());
        }

    }
    return;
}

void adjustVtotalCase(void)
{
    FatalMessageHDMITx("[HDMITX][adj444]h:%d,vratio:%d\n", (UINT32)tx_Max_rgb_timing_table.h.active, (UINT32)min_444_vratio);
    FatalMessageHDMITx("[HDMITX][adj420]h:%d,vratio:%d\n", (UINT32)tx_Max_420_timing_table.h.active, (UINT32)min_420_vratio);

    if(!needNoBlackBorder(tx_Max_rgb_timing_table.h.active, tx_Max_rgb_timing_table.v.active) 
        && !needNoBlackBorder(tx_Max_420_timing_table.h.active, tx_Max_420_timing_table.v.active))
        return;

    if((min_444_vratio == 0) && (min_420_vratio == 0))
        return;

    if(min_444_vratio && (max_rgb_resolution_time_mode != 0) && needNoBlackBorder(tx_Max_rgb_timing_table.h.active, tx_Max_rgb_timing_table.v.active)) {
        updateRGBSourceModeClock();
        checkRGBSourceModeVtotalVaild();
        rgbMaxPixelClock1024 = SourceOutputPixelClk;
        FatalMessageHDMITx("[HDMITX][adj444]vtotal:%d,pll:%d\n", (UINT32)GET_ADJUST_RGB_VTOTAL(), (UINT32)SourceOutputPixelClk);
    }
    if(min_420_vratio && (max_420_resolution_time_mode != 0) && needNoBlackBorder(tx_Max_420_timing_table.h.active, tx_Max_420_timing_table.v.active)) {
        update420SourceModeClock();
        check420SourceModeVtotalVaild();
        y420MaxPixelClock1024 = SourceOutputPixelClk;
        FatalMessageHDMITx("[HDMITX][adj420]vtotal:%d,pll:%d\n", (UINT32)GET_ADJUST_420_VTOTAL(), (UINT32)SourceOutputPixelClk);
    }

    return;
}

UINT16 adjustVrrVstart(UINT16 in_vstart, UINT16 in_vtotal, UINT16 out_vtotal)
{
    UINT32 u32TmpValue = 0;
    u32TmpValue = (UINT32)in_vstart * out_vtotal;
    u32TmpValue /= (UINT32)in_vtotal;
    return (UINT16)u32TmpValue;
}

extern StructHDMITxOutputTimingInfo* p_getTxOutputTimingInfo;
void checkVrrVstart(void)
{
    UINT16 in_ratio;
    UINT16 out_ratio;
    UINT16 in_start;
    UINT16 out_start;

    if(!p_getTxOutputTimingInfo)
        return;
    in_start = timing_info.v_act_sta;
    out_start = p_getTxOutputTimingInfo->TxOutputVsta;
    in_ratio = hdmitx_cal_ratio(in_start, timing_info.v_total);
    out_ratio = hdmitx_cal_ratio(out_start, p_getTxOutputTimingInfo->TxOutputVTotal);
    if(out_ratio > in_ratio) {
        FatalMessageHDMITx("[hdmitx]adjust vrr vbp\n");
        out_start = adjustVrrVstart(in_start, timing_info.v_total, p_getTxOutputTimingInfo->TxOutputVTotal);
        p_getTxOutputTimingInfo->TxOutputVsta =out_start;
        SET_ADJUST_VRR_VSTART(out_start);
    }
}

void matchMaxResolutionTiming(TX_OUT_MODE_SELECT_TYPE mode,EDID_TIMING_DB edid_timing_type)
{
        UINT8 i,timing_count;
        UINT8 ret = _FALSE;
        UINT8 match = _FALSE;
        UINT8 bsupport420 = _FALSE;
        EDID_TIMING_DB db_type = EDID_TIMING_MODE_NUM;
        min_444_vratio = 0xffff;
        min_420_vratio = 0xffff;
        timing_count = ScalerHdmiMacTx0RepeaterGetSinkTxSupportTimingCount(edid_timing_type);
        //InfoMessageHDMITx("[HDMITX]type %d timing_cnt=%d\n", (UINT32)edid_timing_type,(UINT32)timing_count);
        if(timing_count > 0){
            for(i = 0;i<timing_count; i++){
                time_mode = ScalerHdmiMacTx0RepeaterPopSinkTxSupportTimingList(edid_timing_type, i, (UINT8 *)&db_type, (BOOLEAN *)&bsupport420);
                if(time_mode < TX_TIMING_NUM){
                    match = ScalerHdmiMacTx0RepeaterGetTimingMatchSourceMode(mode,db_type,time_mode);
                    if(match){
                        #ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
                            if((db_type == EDID_TIMING_DTD || db_type == EDID_TIMING_EXT_DTD  || db_type == EDID_TIMING_DID_DTD) && dtd_match_flag) {
                                if(tx_timing_table.h.active > tx_Max_rgb_timing_table.h.active)
                                    max_push_list_index = i;
                            }
                        
                            if(edid_timing_type == EDID_TIMING_DTD){
                                updataMaxResolution(TX_OUT_RGB_TIMING);
                            }
                             if(edid_timing_type == EDID_TIMING_VDB){
                                    updataMaxResolution(TX_OUT_RGB_TIMING);
                                    if(bsupport420){
                                        updataMaxResolution(TX_OUT_Y420_TIMING);
                                    }
                             }
                         #endif

                             if(edid_timing_type == EDID_TIMING_ANY){
                                if(db_type == EDID_TIMING_420VDB){
                                        updataMaxResolution(TX_OUT_Y420_TIMING);
                                }else{
                                        updataMaxResolution(TX_OUT_RGB_TIMING);
                                        if(db_type == EDID_TIMING_VDB){
                                              if(bsupport420){
                                                   updataMaxResolution(TX_OUT_Y420_TIMING);
                                              }
                                        }
                                }
                            }
                             //update tx_Max_rgb_timing_table tx_Max_420_timing_table
                            if(ScalerHdmiTxPhy_GetTxTimingTable(max_rgb_resolution_time_mode, &tx_Max_rgb_timing_table) !=  0){
                                FatalMessageHDMITx("[HDMITX] Get tx_cea_timing_table Fail\n");
                                return;
                            }
                             if(ScalerHdmiTxPhy_GetTxTimingTable(max_420_resolution_time_mode, &tx_Max_420_timing_table) !=  0){
                                FatalMessageHDMITx("[HDMITX] Get tx_cea_timing_table Fail\n");
                                return;
                            }
                            if((tx_Max_rgb_timing_table.h.active == MAX_OUTPUT_HACTIVE)
                                && (tx_Max_420_timing_table.h.active == MAX_OUTPUT_HACTIVE)
                                && (min_444_vratio == 0)
                                && (min_420_vratio == 0)){//max support 3840*2160, and vratio=0 no black border
                                ret = _TRUE;
                            }
                        }
                     //InfoMessageHDMITx("max %d %d:\n", (UINT32)max_420_resolution_time_mode,(UINT32)max_rgb_resolution_time_mode);
                     //InfoMessageHDMITx("[HDMITX]type %d Match %d", (UINT32)edid_timing_type,(UINT32)i);
                     //InfoMessageHDMITx("|%d:", (UINT32)db_type);
                     printTimingMatchLog(match,time_mode);
                    if(ret == _TRUE){
                        time_match = 1;
                        break;
                    }
                }
            }
        }
}

void timingMatchForSourceMode(TX_OUT_MODE_SELECT_TYPE mode)
{
    time_match = 0;
    max_rgb_resolution_time_mode = 0;
    max_420_resolution_time_mode = 0;

#ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
    edid_dtd_flag = 0;
    max_push_list_index = 0xff;
    dtd_match_flag = FALSE;
#endif
    SET_ADJUST_OUTPUT_VTOTAL(0);
    SET_ADJUST_RGB_VTOTAL(0);
    SET_ADJUST_420_VTOTAL(0);
    SET_ADJUST_VTOTAL_FLAG(FALSE);
    SET_ADJUST_VRR_VSTART(0);
    memset(&tx_Max_rgb_timing_table, 0x0, sizeof(hdmi_tx_timing_gen_st));
    memset(&tx_Max_420_timing_table, 0x0, sizeof(hdmi_tx_timing_gen_st));
    /*ver1:before 2022/12/6
     TX output timing mode select:
     1. 1st DTD timing mode && (the same frame rate ) && (input pixel clock <= TX timing pixel clock)
     2. 1st SVD timing mode && (the same frame rate ) && (input pixel clock <= TX timing pixel clock)
     3. any timing mode && (the same frame rate ) && (input pixel clock <= TX timing pixel clock)*/
    /*ver2:after 2022/12/6
        TX output sink support maximal resolution from DTD/SVD/420SVD
    */
#ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
    matchMaxResolutionTiming(mode, EDID_TIMING_DTD);
    if(!time_match)
        matchMaxResolutionTiming(mode, EDID_TIMING_VDB);
    if(!time_match)
        matchMaxResolutionTiming(mode, EDID_TIMING_ANY);
#else
    matchMaxResolutionTiming(mode, EDID_TIMING_ANY);
#endif
    if((!time_match && (max_420_resolution_time_mode == 0)&& (max_rgb_resolution_time_mode == 0))
        || (timing_info.is_interlace) || isSpecialTV5And2p5K()) {
            if(mode == TX_OUT_TEST_MODE)
                time_mode = TX_TIMING_NUM;
            else
                time_mode = TX_TIMING_DEFAULT;
            tx_output_colorSpace = timing_info.colorspace;
            tx_output_colordepth = timing_info.colordepth;
            g_TxPixelClock1024 = timing_info.pll_pixelclockx1024;
    } else if(timing_info.h_act_len == MAX_OUTPUT_HACTIVE) {
            if(mode == TX_OUT_TEST_MODE)
                time_mode = TX_TIMING_NUM;
            else
                time_mode = TX_TIMING_DEFAULT;
            decideDefaultOutputTimingPriority();
    } else{//resolution :rgb or yuv420  not zero
            adjustVtotalCase();
            decideTxOutputTimingAndPixelClk();
            #ifdef HDMITX_OUTPUT_MATCH_DTD_FIRST
            decideTxDTDTiming();
            #endif
            SourceOutputPixelClk = hdmitxGetTxOutputPixelClock();
            printTimingMatchLog(1,time_mode);
    }
}
void cmpBypassModeMatchTiming(UINT8 index)
{
    UINT16 u16TmpValue = 0;
    if(ScalerHdmiTxPhy_GetTxTimingTable(index, &tx_timing_table) !=  0){
        FatalMessageHDMITx("[HDMITX] Get tx_cea_timing_table Fail\n");
        return;
    }
    if(timing_info.IHSyncPulseCount != tx_timing_table.h.sync)
        InfoMessageHDMITx("[HDMITX]hsync no match,rx/tx=[%d/%d]\n", (UINT32)timing_info.IHSyncPulseCount, (UINT32)tx_timing_table.h.sync);

    if(timing_info.IVSyncPulseCount != tx_timing_table.v.sync)
        InfoMessageHDMITx("[HDMITX]vsync no match,rx/tx=[%d/%d]\n", (UINT32)timing_info.IVSyncPulseCount, (UINT32)tx_timing_table.v.sync);

    u16TmpValue = tx_timing_table.h.sync + tx_timing_table.h.back;
    if(timing_info.h_act_sta != u16TmpValue)
        InfoMessageHDMITx("[HDMITX]hstart no match,rx/tx=[%d/%d]\n", (UINT32)timing_info.h_act_sta, (UINT32)u16TmpValue);

    u16TmpValue = tx_timing_table.v.sync + tx_timing_table.v.back;
    if(timing_info.v_act_sta != u16TmpValue)
        InfoMessageHDMITx("[HDMITX]vstart no match,rx/tx=[%d/%d]\n", (UINT32)timing_info.v_act_sta, (UINT32)u16TmpValue);

    u16TmpValue = tx_timing_table.h.active + tx_timing_table.h.sync;
    u16TmpValue = u16TmpValue + tx_timing_table.h.front;
    u16TmpValue = u16TmpValue + tx_timing_table.h.back;
    if(timing_info.h_total != u16TmpValue)
        InfoMessageHDMITx("[HDMITX]htotal no match,rx/tx=[%d/%d]\n", (UINT32)timing_info.h_total, (UINT32)u16TmpValue);

    u16TmpValue = tx_timing_table.v.active + tx_timing_table.v.sync;
    u16TmpValue = u16TmpValue + tx_timing_table.v.front;
    u16TmpValue = u16TmpValue + tx_timing_table.v.back;
    if(timing_info.v_total != u16TmpValue)
        InfoMessageHDMITx("[HDMITX]vtotal no match,rx/tx=[%d/%d]\n", (UINT32)timing_info.v_total, (UINT32)u16TmpValue);

    return;
}

extern UINT8 pucEdidVDBVIC[31];
UINT8 pucExtVicLen = 0;
static UINT16 u16BlockBaseAddr = 0;

void parseOversizeBlockVIC(INT8 blockNum)
{
    UINT8 i = 0;
    UINT16 usDetailTimeOffset;
    UINT8 ucOffset;
    UINT16 usNewDataAdd;
    UINT16 u16TmpValue = 0;
    UINT16 u8TmpValue = 0;

    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(_TX0);
    u16BlockBaseAddr = ((0x80*blockNum) % 0x200);
    usNewDataAdd = 0x4 + u16BlockBaseAddr;
    usDetailTimeOffset = u16BlockBaseAddr + pucDdcRamAddr[u16BlockBaseAddr + 0x2];
    NoteMessageHDMITx("[HDMITX] parseExtVIC DT %d \n",(UINT32)usDetailTimeOffset);
    while(usNewDataAdd < usDetailTimeOffset){
            u8TmpValue = *(pucDdcRamAddr + usNewDataAdd);
            u8TmpValue = u8TmpValue & 0xE0;
            NoteMessageHDMITx("[HDMI_TX] parseExtVIC ND %d %d \n",(UINT32)usNewDataAdd,(UINT32)u8TmpValue);
            if((u8TmpValue) == 0x40 ) //_CTA_VDB
            {
                u16TmpValue = pucDdcRamAddr[usNewDataAdd] & 0x1F;
                for(i = 0; i < u16TmpValue; i++){
                    pucEdidVDBVIC[pucExtVicLen+i] = pucDdcRamAddr[usNewDataAdd+1+i];
                }
                pucExtVicLen += u16TmpValue;
            }
            ucOffset = (pucDdcRamAddr[usNewDataAdd] & 0x1F);  // Length of data block
            usNewDataAdd = (usNewDataAdd + ucOffset + 1);
    }
    
    NoteMessageHDMITx("[HDMI_TX] parseExtVIC %d \n",(UINT32)pucExtVicLen);
}

void moveDataBackward(UINT16 start)
{
    UINT16 i = 0;
    for(i = 0x1fe; i > start; i--){
        pucDdcRamAddr[i] = pucDdcRamAddr[i-pucExtVicLen];
    }

}

void insertExtVIC(void)
{
    UINT8 i = 0;
    UINT16 newVdbAddr = 0;
    BOOLEAN bExitVdb = 0;
    UINT16 usDetailTimeOffset;
    UINT8 ucOffset;
    UINT16 usNewDataAdd;
    UINT16 u8TmpValue = 0;
    pucDdcRamAddr = ScalerOutputGetPxPortDdcRamAddr(_TX0);
    usNewDataAdd = 0x184;
    usDetailTimeOffset = 0x180 + pucDdcRamAddr[0x182];
    while(usNewDataAdd < usDetailTimeOffset){
            u8TmpValue = *(pucDdcRamAddr + usNewDataAdd);
            u8TmpValue = u8TmpValue & 0xE0;
            
            if((u8TmpValue) == 0x40 ) //_CTA_VDB
            {
                bExitVdb = 1;
                newVdbAddr = usNewDataAdd;
                break;
            }

            ucOffset = (pucDdcRamAddr[usNewDataAdd] & 0x1F);  // Length of data block
            usNewDataAdd = (usNewDataAdd + ucOffset + 1);
    }
    if(bExitVdb){
        pucDdcRamAddr[newVdbAddr] +=pucExtVicLen;
    }else{
        u8TmpValue = 0x2 << 5;
        u8TmpValue |= pucExtVicLen;
        newVdbAddr = usDetailTimeOffset;
        pucDdcRamAddr[newVdbAddr]=u8TmpValue;
        pucDdcRamAddr[0x182] = pucDdcRamAddr[0x182]  + 1;
    }
    pucDdcRamAddr[0x182] = pucDdcRamAddr[0x182]  +pucExtVicLen;
    NoteMessageHDMITx("[HDMI_TX] inset vic %d %d\n",(UINT32)newVdbAddr,(UINT32)pucDdcRamAddr[newVdbAddr]);
        moveDataBackward(newVdbAddr+pucExtVicLen);
        for(i = 0; i < pucExtVicLen; i++){
            pucDdcRamAddr[newVdbAddr+1+i]=pucEdidVDBVIC[i];
        }
}

