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
// ID Code      : ScalerHdmiPhyTx.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

//#include <rbus/ppoverlay_outtg_reg.h>

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h> 
//#include <rtk_kdriver/tvscalercontrol/scaler_vscdev.h>

#include "../include/ScalerFunctionInclude.h"
#include "../include/HdmiMacTx/ScalerHdmiMacTx.h"
#include "../include/HdmiPhyTx/ScalerHdmiPhyTxInclude.h"
//#include "../include/HdmiMacTx/ScalerHdmiMacTxInclude.h"
//#include "../include/HdmiMacTx0/ScalerHdmiMacTx0Include.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0.h"
#include "../include/Hdmi21MacTx/ScalerHdmi21MacTxInclude.h"
#include "../include/Hdmi21MacTx0/ScalerHdmi21MacTx0Include.h"
#include "../include/Hdmi21MacTx0/ScalerHdmi21MacTx0.h"
#include "../include/ScalerTx.h"
//#include "../include/hdmitx_phy_struct_def.h"
//#include "msg_q_function_api.h"
#include <rbus/vgip_reg.h>


//****************************************************************************
// Macro/Define
//****************************************************************************

#define ABS1(X)     ((X)<0 ? (-X) : (X))
#define FRL_START_TIMEOUT (90000UL)
#define PRINT_5SEC (450000UL)
#define SCDC_0X40_FRL_READY_MASK 0x40 // B[6]: FTL_ready
#define SCDC_0X40_4LANE_LOCK_MASK 0x1e // B[4:1]: Ch0_Ln3/2/1/0_Locked
#define SCDC_0X40_3LANE_LOCK_MASK 0x0e // B[4:1]: Ch0_Ln2/1/0_Locked
#define SCDC_0x40_LANE_UNLOCK_MAX 3 // re-start link training flow when lane unlock
#define SCDC_0x40_CHECK_FAIL_MAX 10 // check sink status

#define ScalerHdmiMacTxPxMapping(x) (_P0_OUTPUT_PORT)
#define ScalerHdmi21MacTxPxMapping(x) (_P0_OUTPUT_PORT)
#define ScalerHdmi21MacTxPxTxMapping(x) (_HW_P0_HDMI21_MAC_TX_MAPPING)
#define ScalerHdmiMacTxGetStreamSource(x) GET_HDMI_MAC_TX0_STREAM_SOURCE()
#define ScalerHdmiPhyTxPowerOn(x)
#define ScalerHdmiPhyTxCMUPowerOff(x)
//****************************************************************************
// CODE TABLES
//****************************************************************************


//****************************************************************************
// local variable declaration
//****************************************************************************

static UINT8 ucLTFailCnt = 0, ucScdcStatusFailCnt=0;


UINT8 bSkipCtsLtpEnFlag=0; // flag for LTP_3, LTP_F test
UINT8 bCtsRetrainEnFlag=0; // link training retrain control



//****************************************************************************
// extern variable declaration
//****************************************************************************
extern UINT8 pEdidData[16];
extern UINT8 subEdidAddr[6];

extern UINT8 bTxSupportHdmiTxOutput;
extern UINT8 bCheckEdidForHdmitxSupport;
extern UINT32 stc;
//****************************************************************************
// extern function declaration
//****************************************************************************

extern void ScalerHdmiTxBypassAVMuteAndPacketCtrl(UINT8 enable);
extern UINT8 waitOutTgStable(void);

//--------------------------------------------------
// Description  : Hdmi 2p1 linktraing parameter reset
// Input Value  : port
// Output Value : none
//--------------------------------------------------
void ScalerHdmi21MacTx0LinktrainingReset(void)
{
    UINT8 bWaitPorchFlag=0;

    // wait porch when disable FRL signal
    if(rtd_inl(HDMITX21_MAC_HDMI21_TX_CTRL_reg) & _BIT7){
        HDMITX_DTG_Wait_Den_Stop_Done();
        bWaitPorchFlag = 1;
    }

    // Reset FIFO & Disable LT pattern
    rtd_maskl(HDMITX21_MAC_HDMI21_TX_CTRL_reg, ~(_BIT7), 0x00);
    rtd_maskl(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT2 | _BIT1 | _BIT0), (_BIT6 | _BIT5 | _BIT2 | _BIT1 | _BIT0));
    rtd_maskl(HDMITX21_LINK_TRAINING_HDMI21_LT_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), 0x00);
    rtd_maskl(HDMITX21_MAC_HDMI21_RS_0_reg, ~_BIT7, 0x00);
    SET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(_TX0, _LINK_TRAING_NONE);
    CLR_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0);
    CLR_HDMI21_MAC_VIDEO_START(_TX0);
    CLR_HDMI21_MAC_TX_LTS_PASS_POLLING(_TX0);
    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDM21_TX0_FRL_PASS_TIMER);
    CLR_HDMI21_MAC_TX_FRL_SETPHY(_TX0);
    CLR_HDMI21_MAC_FORCE_LEGACY(_TX0);
    CLR_HDMI21_MAC_TX_FRL_NOTIMEOUT(_TX0);
    ScalerDpStreamSetTxCurrentOperatePixelMode(ScalerHdmi21MacTxPxMapping(_TX0), _ONE_PIXEL_MODE);

    ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] LTReset@%d\n", (UINT32) bWaitPorchFlag);
    return;
}


// check sink device link status
void _SCDC_check_RSED_CED_Status(void)
{
    INT8 ret;
    UINT8 ucSCDCWritedata = 0;
    I2C_HDMITX_PARA_T i2c_para_info;
    i2c_para_info.addr = _HDMI21_SCDC_SLAVE_ADDR;
    i2c_para_info.sub_addr_len = 0x01;
    i2c_para_info.read_len = 0x01;
    i2c_para_info.wr_len = 0;

    if((GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT6) == _BIT6)
    {
        DebugMessageHDMI21Tx("[HDMI21_TX] GET RSED_Update=%x\n", (UINT32)pEdidData[0]);

        subEdidAddr[0] = _HDMI21_SCDC_OFFSET_59;
        i2c_para_info.read_len = 0x02;
        if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x02,0x01, _HDMI21_TX0_IIC) == _TRUE)
        {
            // 0x5a:Reed-Solomon Corrections Counter[14:8], 0x59:Reed-Solomon Corrections Counter[7:0],
            NoteMessageHDMI21TxLinkTraining("RSED MSB|LSB=%x|%x\n", (UINT32)pEdidData[1], (UINT32)pEdidData[0]);
        }

        ucSCDCWritedata = _BIT6;
        subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
        subEdidAddr[1] = ucSCDCWritedata;
        ret = ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI21_TX0_IIC);
        if(ret != _SUCCESS)
            ErrorMessageHDMI21Tx("[HDMI21_TX] write RSED_Update fail@LTS[%d]\n", (UINT32)GET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0));

        DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] SET RSED_Update=%x@LTPassCheck\n",(UINT32) subEdidAddr[1]);
    }

    if((GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT1) == _BIT1)
    {
        FatalMessageHDMITx("[HDMI21_TX] GET CED_Update=%x\n", (UINT32)pEdidData[0]);

        subEdidAddr[0] = _HDMI21_SCDC_OFFSET_50;
        i2c_para_info.read_len = 0x09;
        if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x09,0x01, _HDMI21_TX0_IIC) == _TRUE)
        {
            // 0x51:lane0[14:8], 0x50:lane0[7:0],
            FatalMessageHDMITx("[HDMI21_TX] CED MSB|LSB=%x|%x/", (UINT32)pEdidData[1], (UINT32)pEdidData[0]);
            // 0x53:lane1[14:8], 0x52:lane1[7:0],
            FatalMessageHDMITx("%x|%x/", (UINT32)pEdidData[3], (UINT32)pEdidData[2]);
            // 0x55:lane2[14:8], 0x54:lane2[7:0],
            FatalMessageHDMITx("%x|%x/", (UINT32)pEdidData[5], (UINT32)pEdidData[4]);
            // 0x58:lane3[14:8], 0x57:lane3[7:0],
            FatalMessageHDMITx("%x|%x\n", (UINT32)pEdidData[8], (UINT32)pEdidData[7]);
        }

        ucSCDCWritedata = _BIT1;
        subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
        subEdidAddr[1] = ucSCDCWritedata;
        ret = ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI21_TX0_IIC);
    }

    return;
}


void _Linktraining_SCDC35_Config(void)
{
    ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC 0x35=%d\n", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0));

    if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & _BIT5) == _BIT5)
    {
        SET_HDMI21_MAC_TX_FRL_NOTIMEOUT(_TX0);
    }
    else
    {
        CLR_HDMI21_MAC_TX_FRL_NOTIMEOUT(_TX0);
    }

    if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & _BIT6) == _BIT6)
    {
        SET_HDMI21_MAC_TX_FRL_DSC_MAX(_TX0);
    }
    else
    {
        CLR_HDMI21_MAC_TX_FRL_DSC_MAX(_TX0);
    }

    if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & _BIT7) == _BIT7)
    {
        SET_HDMI21_MAC_TX_FRL_MAX(_TX0);
    }
    else
    {
        CLR_HDMI21_MAC_TX_FRL_MAX(_TX0);
    }

    return;
}



//--------------------------------------------------
// Description  : Hdmi 2p1 linktraing flow
// Input Value  : port
// Output Value : none
//--------------------------------------------------
void ScalerHdmi21MacTx0Linktraining(void)
{
    I2C_HDMITX_PARA_T i2c_para_info;
    UINT8 ucL0L1Pattern = 0;
    UINT8 ucL2L3Pattern = 0;
    UINT8 newTxFlow;
    UINT8 ucSCDCWritedata = 0;
    UINT8 edidRxMaxFrlRate;
    EnumHdmi21FrlType targetFrlRate;
    I3DDMA_COLOR_DEPTH_T colorBitIndex;
    TX_TIMING_INDEX videoFmtIndex;
    UINT16 dispWidth;
    UINT16 vFreq;
    INT8 ret;
#if(_HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING == _ON)
    newTxFlow = (ScalerHdmiTxGetFrlNewModeEnable()  && ScalerHdmiTxGetFastTimingSwitchEn());
#else
    newTxFlow = ScalerHdmiTxGetFrlNewModeEnable();
#endif
    DebugMessageHDMITx("[HDMITX] ScalerHdmi21MacTx0Linktraining , LINK_TRAINING/STREAM_PROCESS STATE=%d/%d \n", (UINT32)GET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0), GET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(_TX0));

    if(GET_HDMI21_MAC_FORCE_LEGACY(_TX0) == _TRUE)
    {
        SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTSL);

        DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] LT: EDID(%d) support link training, read SCDC version SCDC 0x1\n", (UINT32)pEdidData[0]);
    }
    i2c_para_info.addr = _HDMI21_SCDC_SLAVE_ADDR;
    i2c_para_info.sub_addr_len = 0x01;
    i2c_para_info.read_len = 0x01;
    i2c_para_info.wr_len = 0;
    switch(GET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0))
    {
        case _HDMI21_LT_LTS1:

            ScalerHdmiPhyTxCMUPowerOff(ScalerHdmi21MacTxPxMapping(_TX0));

            NoteMessageHDMI21TxLinkTraining("[HDMI21_TX] LTS1\n");
            SET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(_TX0, _LINK_TRAING_NONE);
            ScalerDpStreamSetTxCurrentOperatePixelMode(ScalerHdmi21MacTxPxMapping(_TX0), _ONE_PIXEL_MODE);
            CLR_HDMI21_MAC_VIDEO_START(_TX0);

            // Check EDID SCDC present & FRL_MAX
            NoteMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC_PRESENT=%d,MAX_FRL_RATE=%d\n",(UINT32)ScalerHdmiMacTxEdidGetFeature(ScalerHdmi21MacTxPxMapping(_TX0), _SCDC_PRESENT),(UINT32)ScalerHdmiMacTxEdidGetFeature(ScalerHdmi21MacTxPxMapping(_TX0), _MAX_FRL_RATE));
            if((ScalerHdmiMacTxEdidGetFeature(ScalerHdmi21MacTxPxMapping(_TX0), _SCDC_PRESENT) == _TRUE) && (ScalerHdmiMacTxEdidGetFeature(ScalerHdmi21MacTxPxMapping(_TX0), _MAX_FRL_RATE) != 0))
            {
                // Read SCDC 0x01 Sink version
                subEdidAddr[0] = _HDMI21_SCDC_OFFSET_1;
                if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _TRUE)
                {
                    DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] EDID (%x) support link training, read SCDC version SCDC 0x1\n", (UINT32)pEdidData[0]);

                    // Check sink EDID/SCDC
                    // if((MAX_FRL_Rate > 0) && (SCDC_Present == 1) && (pData[0] != 0))
                    if(pEdidData[0] != 0)
                    {
                        ScalerHdmi21MacTx0LinktrainingReset();

                        // HDMI2.1 FRL mode
                        SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTS2);

                        #if 0 //  FRL FIFO error when AC ON
                        // Enable HDMI2.1 Module
                        rtd_maskl(HDMITX21_MAC_HDMI21_TX_CTRL_reg, ~_BIT7, _BIT7);
                        #endif
                        ucSCDCWritedata = 0x01;

                        //subEdidAddr[0] = _HDMI21_SCDC_OFFSET_2; HDMI_TX_I2C_BUSID
                        subEdidAddr[0] = _HDMI21_SCDC_OFFSET_2;
                        subEdidAddr[1] = ucSCDCWritedata;
                        if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI21_TX0_IIC) == _TRUE)
                        {
                            InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] Source Version WRITE=%d\n", (UINT32)ucSCDCWritedata);
                        }
                        else
                        {
                            ErrorMessageHDMI21Tx("[HDMI21_TX] Source Version WR(%d) FAIL\n", (UINT32)ucSCDCWritedata);
                        }

                        //make sure 2.0 won't enter link training flow
                        if(ScalerHdmiTxGetTargetFrlRate() == 0) // HDMI 2.0(FRL=0)
                        {
                            SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTSL);
                        }
                    }
                    else
                    {
                        // HDMI2.0 TMDS mode
                        SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTSL);
                    }
                }
                else
                {
                    // HDMI1.4 TMDS mode
                    SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTSL);
                    ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC read fail\n");
                }
            }
            else
            {
                // HDMI2.0 TMDS mode
                SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTSL);
                ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] EDID (%x) not support 2.1 !!!\n", (UINT32)pEdidData[0]);
            }

            break;

        case _HDMI21_LT_LTS2:

            NoteMessageHDMI21TxLinkTraining("[HDMI21_TX] LTS2\n");

            // Read SCDC 0x40 bit6 FLT_Ready
            subEdidAddr[0] = _HDMI21_SCDC_OFFSET_40;
            if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _TRUE)
            {
                SET_HDMI21_MAC_TX_FROM_RX_SCDC40_STATUSFLAG(_TX0, pEdidData[0]);

                // Sink Ready for link training
                if(GET_HDMI21_MAC_TX_FROM_RX_SCDC40_STATUSFLAG(_TX0) & _BIT6)
                {
                    // [CTS][HFR1-10] check Source Test Configuration if Source_Test_Update Flag(_BIT3)=1
                    CLR_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0);
                    subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                    if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _TRUE)
                    {
                        SET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0, pEdidData[0]);
                        NoteMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC 0x10=%d@LTS2\n", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0));
                        if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT3)
                        {
                            subEdidAddr[0] = _HDMI21_SCDC_OFFSET_35;
                            if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _TRUE)
                            {
                                SET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0, pEdidData[0]);
                                _Linktraining_SCDC35_Config(); // update SCDC 0x35 info

                                //DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC 0x35 bit5 GET NO TIME OUT FLag =%d\n", GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0));
                            }
                            else
                            {
                                ErrorMessageHDMI21Tx("[HDMI21_TX] read SCDC 0x35 FAIL@LTS2\n");
                            }

                            subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                            subEdidAddr[1] = _BIT3;
                            if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x02,  0x01,_HDMI21_TX0_IIC) == _SUCCESS)
                            {
                                DebugMessageHDMI21Tx("[HDMI21_TX] write SrcTestUpdate@LTS2\n");
                            }else{
                                ErrorMessageHDMI21Tx("[HDMI21_TX] write SrcTestUpdate FAIL@LTS2\n");
                            }
                        }
                    }

                    //  Use timing target FRL rate
                    edidRxMaxFrlRate = ScalerHdmiMacTxEdidGetFeature(ScalerHdmi21MacTxPxMapping(_TX0), _MAX_FRL_RATE);
                    colorBitIndex = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_DEPTH);
                    videoFmtIndex = ScalerHdmiTxGetScalerVideoFormatIndex();
                    targetFrlRate = ScalerHdmiTxPhy_GetPLLTimingInfo_frlRate(videoFmtIndex, colorBitIndex);
                    ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] FRL MAX/Target=%d/%d@LTS2\n", (UINT32)edidRxMaxFrlRate, (UINT32)targetFrlRate);

                    // [FRL_RATE] set timing mapping FRL mode
                    if((bTxSupportHdmiTxOutput == 1) || (edidRxMaxFrlRate >= targetFrlRate)){
                        SET_HDMI21_MAC_TX_FRL_RATE(_TX0, targetFrlRate);
                    }else{
                        dispWidth = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_ACT_WID);
                        vFreq = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_VFREQ);
                        ErrorMessageHDMI21Tx("[HDMI21_TX] Video %dp%d\n", (UINT32)dispWidth, (UINT32)vFreq);
                        ErrorMessageHDMI21Tx("[HDMI21_TX] FRL MAX(%d) < TARGET(%d)\n",  (UINT32)edidRxMaxFrlRate, (UINT32)targetFrlRate);
                        SET_HDMI21_MAC_TX_FRL_RATE(_TX0, _HW_HDMI21_SET_FRL_RATE);
                    }

                    // Set FFE max level for current FRL Rate (define in project.h)
                    SET_HDMI21_MAC_TX_FFE_LEVEL(_TX0, _HW_HDMI21_SET_FFE_LEVEL);

                    DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] LTS2 FRL rate = %d\n", (UINT32)GET_HDMI21_MAC_TX_FRL_RATE(_TX0));

                    // Reset Tx Lane0 FFE level

                    CLR_HDMI21_MAC_TX_LANE0_FFE(_TX0);
                    // ScalerHdmi21SetTx0FfeLevel(_HDMI21_LANE0, GET_HDMI21_MAC_TX_LANE0_FFE(), _HDMI_2_1_NORMAL);

                    // Reset Tx Lane1 FFE level
                    CLR_HDMI21_MAC_TX_LANE1_FFE(_TX0);
                    // ScalerHdmi21SetTx0FfeLevel(_HDMI21_LANE1, GET_HDMI21_MAC_TX_LANE1_FFE(), _HDMI_2_1_NORMAL);

                    // Reset Tx Lane2 FFE level
                    CLR_HDMI21_MAC_TX_LANE2_FFE(_TX0);
                    // ScalerHdmi21SetTx0FfeLevel(_HDMI21_LANE2, GET_HDMI21_MAC_TX_LANE2_FFE(), _HDMI_2_1_NORMAL);

                    // Reset Tx Lane3 FFE level
                    CLR_HDMI21_MAC_TX_LANE3_FFE(_TX0);
                    // ScalerHdmi21SetTx0FfeLevel(_HDMI21_LANE3, GET_HDMI21_MAC_TX_LANE3_FFE(), _HDMI_2_1_NORMAL);

                    // Set SCDC FRL_Rate and FFE_Level
                    ucSCDCWritedata = (GET_HDMI21_MAC_TX_FFE_LEVEL(_TX0) << 4) | GET_HDMI21_MAC_TX_FRL_RATE(_TX0);

                    DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] write SCDC 0x31(FFE|FRL)=%x@LTS2\n", (UINT32)ucSCDCWritedata);

                    subEdidAddr[0] = _HDMI21_SCDC_OFFSET_31;
                    subEdidAddr[1] = ucSCDCWritedata;
                    if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x02, 0x01, _HDMI21_TX0_IIC) == _TRUE)
                    {
                        CLR_HDMI21_MAC_TX_FRL_SETPHY(_TX0);
                        SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTS3);
                        SET_HDMI21_MAC_TX_LTS3_TIMER_START(_TX0);
                        CLR_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0);
                        NoteMessageHDMI21TxLinkTraining("[HDMI21_TX] LT3 START\n");
                    }
                    else
                    {
                        InfoMessageHDMI21Tx("[HDMI21_TX] write SCDC 0x31=%x FAIL\n", (UINT32)ucSCDCWritedata);
                    }

                }
                else
                {
                    InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] LTS2 read SCDC FLT READY = 0\n");
                }
            }
            else
            {
                ErrorMessageHDMI21Tx("[HDMI21_TX] LTS2 read SCDC FAIL\n");
            }

            break;

        case _HDMI21_LT_LTS3:

            if(GET_HDMI21_MAC_TX_LTS3_TIMER_START(_TX0) == _TRUE) // make sure I2C bus polling interval 10 msec read SCDC
            {
                if((GET_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0) % 10) == 0)
                    InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] _HDMI21_LT_LTS3 =%d\n", (UINT32)GET_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0));
                else
                    DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] _HDMI21_LT_LTS3 =%d\n", (UINT32)GET_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0));

                CLR_HDMI21_MAC_TX_LTS3_TIMER_START(_TX0);
                ADD_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0);
                //10ms to check
                ScalerTimerReactiveTimerEvent(TIMER_HDMITX_LT_CHECK, _SCALER_TIMER_EVENT_HDM21_TX0_LTS3_TIMER);

                // DEBUG_FIX_FRL_RATE // FIX TX OUTPUT FRL RATE
                if((GET_HDMI21_MAC_TX_FRL_NOTIMEOUT(_TX0) == _FALSE)
                    && (GET_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0) > _HDMI21_LINK_TRAINING_TIMEOUT))
                {
                    ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] LTS3 Timeout(%d/%d)\n", (UINT32)GET_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0), (UINT32)_HDMI21_LINK_TRAINING_TIMEOUT);
                    if(!bCheckEdidForHdmitxSupport|| GET_HDMI21_MAC_TX_FRL_NOTIMEOUT(_TX0)){ // Re-start link training flow 
                        #if 0 //extend TX output unlock period
                        if(ScalerHdmiTxGetH5DPlatformType() == HOST_PLATFORM_O20){
                            #define _SLEEP_TIME_MS 100
                            // disable async fifo before change PLL clock
                            ScalerHdmiTx0_Mac_asyncFifoEnable(_DISABLE); // 0x0 : Disable async_fifo
                            msleep(_SLEEP_TIME_MS);
                            InfoMessageHDMITx("[HDMITX] Sleep: %dms@LT3\n", _SLEEP_TIME_MS);
                            // enable async fifo after PLL clock stable
                            ScalerHdmiTx0_Mac_asyncFifoEnable(_ENABLE); // 0x1 : Enable async_fifo
                        }
                        #endif
                        SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTS4);
                    }else
                    { // 200msec timeout jump to legacy
                        SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTSL);
                    }

                    InfoMessageHDMI21TxLinkTraining("[HDMI21_TX]  TV EDID=0/%d, Enter State[%d]\n",  (UINT32)!bCheckEdidForHdmitxSupport, (UINT32)GET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0));
                    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDM21_TX0_LTS3_TIMER);
                }

                // Read SCDC 0x10 bit5 FLT_Update

                subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _TRUE)
                {
                    #if 0 // FRL FIFO error when AC ON
                    // Enable HDMI21 module
                    rtd_maskl(HDMITX21_MAC_HDMI21_TX_CTRL_reg, ~(_BIT7), _BIT7);
                    #endif

                    SET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0, pEdidData[0]);
                    // check [5] FLT_update, [4] FLT_start, [3] Sourece_Test_Update, [2] RR_test
                    if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & (0x3c))
                        ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC 0x10=%x@LTS3\n", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0));

                    // [CTS][HFR1-10] check Source Test Configuration if Source_Test_Update Flag(_BIT3)=1
                    if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT3){
                        subEdidAddr[0] = _HDMI21_SCDC_OFFSET_35;
                        if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _TRUE)
                        {
                            SET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0, pEdidData[0]);
                            _Linktraining_SCDC35_Config(); // update SCDC 0x35 info
                        }
                        else
                        {
                            InfoMessageHDMI21Tx("[HDMI21_TX] read SCDC 0x35 FAIL@LTS3!!\n");
                        }

                        subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                        subEdidAddr[1] = _BIT3;
                        if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x02,  0x01,_HDMI21_TX0_IIC) == _SUCCESS)
                        {
                            DebugMessageHDMI21Tx("[HDMI21_TX] write SrcTestUpdate@LTS3\n");
                        }else{
                            InfoMessageHDMI21Tx("[HDMI21_TX] write SrcTestUpdate FAIL@LTS3\n");
                        }
                    }

                    if((GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT5)|| GET_HDMI21_MAC_TX_CTS_LT_RETRAIN_EN(_TX0))
                    {
                        InfoMessageHDMI21Tx("[HDMI21_TX] SCDC 0x10=%x@LTS3\n", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0));

                        // HDMI2.1 PHY initial
                        if(GET_HDMI21_MAC_TX_FRL_SETPHY(_TX0) == _FALSE)
                        {
                            // set phy function
                            // ScalerHdmiComboDPTxRingCMUPowerOn(GET_HDMI_2_1_TX_FRL_RATE());
                            #if 1 //
                            //UINT8 edidRxMaxFrlRate = ScalerHdmiMacTxEdidGetFeature(ScalerHdmi21MacTxPxMapping(_TX0), _MAX_FRL_RATE);
                            ScalerHdmiPhyTxSet(ScalerHdmiMacTxPxMapping(_TX0));

                            //  HDMITX MAC 2.0 setting
                            //  LANE PN Swap
                            //  LANE src sel
                            ScalerHdmiTx0_MacConfig_LaneSet(TC_HDMI21);

                            #else
                            ScalerGDIPhyHdmiTx0Hdmi21RingCMUPowerOn(ScalerHdmi21MacTxPxMapping(_TX0), GET_HDMI21_MAC_TX_FRL_RATE(_TX0));
                            ScalerGDIPhyHdmiTxHdmi21MLPHYSet(ScalerHdmi21MacTxPxMapping(_TX0));
                            #endif
                            ScalerHdmiPhyTxPowerOn(ScalerHdmi21MacTxPxMapping(_TX0));
                            SET_HDMI21_MAC_TX_FRL_SETPHY(_TX0);
                        }

                        // Read SCDC 0x41 and 0x42 link training pattern
                        subEdidAddr[0] = _HDMI21_SCDC_OFFSET_41;
                        i2c_para_info.read_len = 0x02;
                        if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x02,0x01, _HDMI21_TX0_IIC) == _TRUE)
                        {
                            SET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0, pEdidData[0] & (_BIT3 | _BIT2 | _BIT1 | _BIT0));
                            SET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0, (pEdidData[0] & (_BIT7 | _BIT6 | _BIT5 | _BIT4)) >> 4);
                            SET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0, pEdidData[1] & (_BIT3 | _BIT2 | _BIT1 | _BIT0));
                            SET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0, (pEdidData[1] & (_BIT7 | _BIT6 | _BIT5 | _BIT4)) >> 4);

                            ErrorMessageHDMI21TxLinkTraining("[HDMITX] 0x41=%x/%x\n", (UINT32)pEdidData[0], (UINT32)pEdidData[1]);
                            if(GET_HDMI21_MAC_TX_CTS_LT_RETRAIN_EN(_TX0)){
                                ErrorMessageHDMI21Tx("[HDMI21_TX] Retrain@LTS3\n");
                                CLR_HDMI21_MAC_TX_CTS_LT_RETRAIN_EN(_TX0);
                            }

                            InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC (%x) 0x41 Pattern \n", (UINT32)pEdidData[0]);
                            InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC (%x) 0x42 Pattern \n", (UINT32)pEdidData[1]);

                            // [DANBU-813]: When FLT_no_timeout is cleared (=0), clear (=0) FLT_update and continue sending the previous pattern.
                            CLR_HDMI21_MAC_TX_CTS_IGNORE_LTP_EN(_TX0);

                    #ifdef _DEBUG_HOLD_LT3_MODE // pending in link training LT3 state
                            if(((pEdidData[0] == 0x0) && (pEdidData[1] == 0x0))|| ((pEdidData[0] == 0xff) && (pEdidData[1] == 0xff))){
                                extern void hdmi21_hwsd_script_SET_PLL_12G(void);
                                extern void hdmi21_hwsd_script_DUMP_PLL_12G(void);
                                NoteMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC Pattern req =0x%x 0x%x, HOLD LT state!!\n", (UINT32)pEdidData[0], (UINT32)pEdidData[1]);
                            #if 0 // [TEST] dump register setting
                                    static UINT8 bDumpReg=0;
                                    extern void hdmi21_DumpReg_VIC196_8kp30_12bit_12G4L(void);
                                    extern void hdmi21_DumpReg_LTP_Pattern_5678(void);
                                    ROSTimeDly(100);
                                    if(!bDumpReg){
                                        NoteMessageHDMITx("\n\n\n[TEST] DUMP REG@LinkTraining!!\n");
                                        hdmi21_DumpReg_VIC196_8kp30_12bit_12G4L();
                                        hdmi21_DumpReg_LTP_Pattern_5678();
                                        NoteMessageHDMITx("[TEST] END\n\n\n\n");
                                    }
                                    bDumpReg = 1;
                            #endif
                                while((rtd_inl(0xb8005040) & _BIT16) == 0){
                                    HDMITX_MDELAY(1000);
                                }
                            }
                    #endif
                            // SCDC link training pattern = 0
                            if(((GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0) == 0x00) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0) == 0x00))
                                    && (((GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0) == 0x00)
                                            && ((GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0) == 0x00) || (GET_HDMI21_MAC_TX_FRL_RATE(_TX0) < 2)))))
                            {
                                InfoMessageHDMI21Tx("[HDMI21_TX] SCDC Pattern req =0  LTS P\n");

                                // special case for INTEL, not request LTP pattern , go gap state
                                if(rtd_getbits(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg, _BIT7) == 0x00)
                                    {
                                    // enable 18bit to 20bit fifo setting
                                    rtd_maskl(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT2 | _BIT1 | _BIT0), _BIT7);
                                }


                                SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTSP_GAP);

                                break;
                            }
                            else if(((GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0) == 0xF) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0) == 0xF)) || ((GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0) == 0xF) || (GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0) == 0xF))) // SCDC link training pattern = 0xF
                            {
                                WarnMessageHDMI21Tx("[HDMI21_TX] SCDC Pattern req=0XF, enter LTS4\n");

                                SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTS4);
                                break;
                            }
                            else // SCDC link training pattern = 0x1~8 or 0xE , LTP3 need Check no timeout & LTP4 need check TEST mode
                            {
                                // Lane0
                                if(GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0) == 0x0E)
                                {
                                    NoteMessageHDMI21TxLinkTraining("[HDMI21_TX] L0 FFE REQ=%d/%d\n", (UINT32)GET_HDMI21_MAC_TX_LANE0_FFE(_TX0), (UINT32)GET_HDMI21_MAC_TX_FFE_LEVEL(_TX0));
                                    ADD_HDMI21_MAC_TX_LANE0_FFE(_TX0);

                                    if(GET_HDMI21_MAC_TX_LANE0_FFE(_TX0) > GET_HDMI21_MAC_TX_FFE_LEVEL(_TX0))
                                    {
                                        CLR_HDMI21_MAC_TX_LANE0_FFE(_TX0);
                                    }

                                    // need check if stay in LTP4 , config FFE test mode
                                    if((rtd_inl(HDMITX21_LINK_TRAINING_HDMI21_LT_3_reg) & 0x0F) == _LTP4)
                                    {
                                        if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT3) // NO FFE
                                        {
                                            SET_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0, _HDMI_2_1_NO_FFE);
                                        }
                                        else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT2) // De emphasis
                                        {
                                            SET_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0, _HDMI_2_1_DE_EMPHASIS_ONLY);
                                        }
                                        else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT1) // PRE shoot only
                                        {
                                            SET_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0, _HDMI_2_1_PRE_SHOOT_ONLY);
                                        }
                                        else
                                        {
                                            CLR_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0);
                                        }
                                        DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] LT: SCDC command 0xF AT LTP4 , Get Test config=%x\n", (UINT32)GET_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0));
                                    }
                                    // Set Tx APHY FFE level
                                    ScalerGDIPhyHdmiTxHdmi21SetTxFfeLevel(ScalerHdmi21MacTxPxMapping(_TX0), _HDMI21_LANE0, GET_HDMI21_MAC_TX_LANE0_FFE(_TX0), GET_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0));
                                }
                                else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0) >= _LTP1) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0) <= _LTP8))
                                {
                                    if(GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0) == _LTP3) // Nyquist Clock
                                    {
                                        // [DANBU-813]: When FLT_no_timeout is cleared (=0), clear (=0) FLT_update and continue sending the previous pattern.
                                        if(GET_HDMI21_MAC_TX_FRL_NOTIMEOUT(_TX0) == _TRUE) // NO TIMEOUT FLAG
                                        {
                                            CLR_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0);
                                        }
                                        else
                                        {
                                            SET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0, (rtd_inl(HDMITX21_LINK_TRAINING_HDMI21_LT_3_reg) & 0x0F));
                                            SET_HDMI21_MAC_TX_CTS_IGNORE_LTP_EN();
                                        }
                                    }
                                    else if(GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0) == _LTP4)
                                    {
                                        if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT3) // NO FFE
                                        {
                                            SET_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0, _HDMI_2_1_NO_FFE);
                                        }
                                        else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT2) // De emphasis
                                        {
                                            SET_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0, _HDMI_2_1_DE_EMPHASIS_ONLY);
                                        }
                                        else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT1) // PRE shoot only
                                        {
                                            SET_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0, _HDMI_2_1_PRE_SHOOT_ONLY);
                                        }
                                        else
                                        {
                                            CLR_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0);
                                        }

                                        ScalerGDIPhyHdmiTxHdmi21SetTxFfeLevel(ScalerHdmi21MacTxPxMapping(_TX0), _HDMI21_LANE0, GET_HDMI21_MAC_TX_LANE0_FFE(_TX0), GET_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0));

                                        InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC L0 LTP4 , Get Test config =%x\n", (UINT32)GET_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0));
                                    }
                                    else // LTP other
                                    {
                                        DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC L0 Pattern req =%d\n", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0));

                                        CLR_HDMI21_MAC_TX_LANE0_FFEMODE(_TX0);
                                    }

                                    if((GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0) >= _LTP1) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0) <= _LTP4))
                                    {
                                        // 16b18b encode lane0 disable
                                        rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~_BIT4, 0x00);
                                    }
                                    else
                                    {
                                        // 16b18b encode lane0 enable
                                        rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~_BIT4, _BIT4);
                                    }
                                }

                                // Lane1

                                if(GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0) == 0x0E)
                                {
                                    InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] L1 FFE REQ=%d/%d\n", (UINT32)GET_HDMI21_MAC_TX_LANE1_FFE(_TX0), (UINT32)GET_HDMI21_MAC_TX_FFE_LEVEL(_TX0));
                                    ADD_HDMI21_MAC_TX_LANE1_FFE(_TX0);

                                    if(GET_HDMI21_MAC_TX_LANE1_FFE(_TX0) > GET_HDMI21_MAC_TX_FFE_LEVEL(_TX0))
                                    {
                                        CLR_HDMI21_MAC_TX_LANE1_FFE(_TX0);
                                    }

                                    // need check if stay in LTP4 , config FFE test mode
                                    if(((rtd_inl(HDMITX21_LINK_TRAINING_HDMI21_LT_3_reg) & 0xF0) >> 4) == _LTP4)
                                    {
                                        if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT3) // NO FFE
                                        {
                                            SET_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0, _HDMI_2_1_NO_FFE);
                                        }
                                        else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT2) // De emphasis
                                        {
                                            SET_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0, _HDMI_2_1_DE_EMPHASIS_ONLY);
                                        }
                                        else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT1) // PRE shoot only
                                        {
                                            SET_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0, _HDMI_2_1_PRE_SHOOT_ONLY);
                                        }
                                        else
                                        {
                                            CLR_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0);
                                        }
                                        InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC L1 LTP4, FFE set 0x35 test config=%x\n", (UINT32)GET_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0));
                                    }

                                    // new function for FFE
                                    ScalerGDIPhyHdmiTxHdmi21SetTxFfeLevel(ScalerHdmi21MacTxPxMapping(_TX0), _HDMI21_LANE1, GET_HDMI21_MAC_TX_LANE1_FFE(_TX0), GET_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0));
                                }
                                else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0) >= _LTP1) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0) <= _LTP8))
                                {
                                    if(GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0) == _LTP3) // Nyquist Clock
                                    {
                                        // [DANBU-813]: When FLT_no_timeout is cleared (=0), clear (=0) FLT_update and continue sending the previous pattern.
                                        if(GET_HDMI21_MAC_TX_FRL_NOTIMEOUT(_TX0) == _TRUE) // NO TIMEOUT FLAG
                                        {
                                            CLR_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0);
                                        }
                                        else
                                        {
                                            SET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0, ((rtd_inl(HDMITX21_LINK_TRAINING_HDMI21_LT_3_reg) & 0xF0) >> 4));
                                            SET_HDMI21_MAC_TX_CTS_IGNORE_LTP_EN();
                                        }
                                    }
                                    else if(GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0) == _LTP4)
                                    {
                                        if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT3) // NO FFE
                                        {
                                            SET_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0, _HDMI_2_1_NO_FFE);
                                        }
                                        else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT2) // De emphasis
                                        {
                                            SET_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0, _HDMI_2_1_DE_EMPHASIS_ONLY);
                                        }
                                        else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT1) // PRE shoot only
                                        {
                                            SET_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0, _HDMI_2_1_PRE_SHOOT_ONLY);
                                        }
                                        else
                                        {
                                            CLR_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0);
                                        }

                                        ScalerGDIPhyHdmiTxHdmi21SetTxFfeLevel(ScalerHdmi21MacTxPxMapping(_TX0), _HDMI21_LANE1, GET_HDMI21_MAC_TX_LANE1_FFE(_TX0), GET_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0));

                                        InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC L1 LTP4 , Get Test config=%x\n", (UINT32)GET_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0));
                                    }
                                    else // LTP other
                                    {
                                        DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC L1 Pattern req =%d\n", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0));

                                        // FFE NORMAL
                                        CLR_HDMI21_MAC_TX_LANE1_FFEMODE(_TX0);
                                    }

                                    if((GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0) >= _LTP1) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0) <= _LTP4))
                                    {
                                        // 16b18b encode lane1 disable
                                        rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~_BIT5, 0x00);
                                    }
                                    else
                                    {
                                        // 16b18b encode lane1 enable
                                        rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~_BIT5, _BIT5);
                                    }

                                }

                                // Lane2
                                if(GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0) == 0x0E)
                                {
                                    InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] L2 FFE REQ=%d/%d\n", (UINT32)GET_HDMI21_MAC_TX_LANE2_FFE(_TX0), (UINT32)GET_HDMI21_MAC_TX_FFE_LEVEL(_TX0));
                                    ADD_HDMI21_MAC_TX_LANE2_FFE(_TX0);

                                    if(GET_HDMI21_MAC_TX_LANE2_FFE(_TX0) > GET_HDMI21_MAC_TX_FFE_LEVEL(_TX0))
                                    {
                                        CLR_HDMI21_MAC_TX_LANE2_FFE(_TX0);
                                    }

                                    // need check if stay in LTP4 , config FFE test mode
                                    if((rtd_inl(HDMITX21_LINK_TRAINING_HDMI21_LT_4_reg) & 0x0F) == _LTP4)
                                    {
                                        if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT3) // NO FFE
                                        {
                                            SET_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0, _HDMI_2_1_NO_FFE);
                                        }
                                        else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT2) // De emphasis
                                        {
                                            SET_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0, _HDMI_2_1_DE_EMPHASIS_ONLY);
                                        }
                                        else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT1) // PRE shoot only
                                        {
                                            SET_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0, _HDMI_2_1_PRE_SHOOT_ONLY);
                                        }
                                        else
                                        {
                                            CLR_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0);
                                        }
                                        InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC command 0xE AT LTP4 , Get Test config=%x\n", (UINT32)GET_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0));
                                    }

                                    // Set Tx APHY FFE level
                                    ScalerGDIPhyHdmiTxHdmi21SetTxFfeLevel(ScalerHdmi21MacTxPxMapping(_TX0), _HDMI21_LANE2, GET_HDMI21_MAC_TX_LANE2_FFE(_TX0), GET_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0));
                                }
                                else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0) >= _LTP1) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0) <= _LTP8))
                                {
                                    if(GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0) == _LTP3) // Nyquist Clock
                                    {
                                        // [DANBU-813]: When FLT_no_timeout is cleared (=0), clear (=0) FLT_update and continue sending the previous pattern.
                                        if(GET_HDMI21_MAC_TX_FRL_NOTIMEOUT(_TX0) == _TRUE) // NO TIMEOUT FLAG
                                        {
                                            CLR_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0);
                                        }
                                        else
                                        {
                                            SET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0, (rtd_inl(HDMITX21_LINK_TRAINING_HDMI21_LT_4_reg) & 0x0F));
                                            SET_HDMI21_MAC_TX_CTS_IGNORE_LTP_EN();
                                        }
                                    }
                                    else if(GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0) == _LTP4)
                                    {
                                        if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT3) // NO FFE
                                        {
                                            SET_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0, _HDMI_2_1_NO_FFE);
                                        }
                                        else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT2) // De emphasis
                                        {
                                            SET_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0, _HDMI_2_1_DE_EMPHASIS_ONLY);
                                        }
                                        else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT1) // PRE shoot only
                                        {
                                            SET_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0, _HDMI_2_1_PRE_SHOOT_ONLY);
                                        }
                                        else
                                        {
                                            CLR_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0);
                                        }

                                        ScalerGDIPhyHdmiTxHdmi21SetTxFfeLevel(ScalerHdmi21MacTxPxMapping(_TX0), _HDMI21_LANE2, GET_HDMI21_MAC_TX_LANE2_FFE(_TX0), GET_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0));

                                        InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC L2 LTP4 , Get Test config=%x\n", (UINT32)GET_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0));
                                    }
                                    else // LTP other
                                    {
                                        DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC L2 Pattern req =%d\n", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0));

                                        CLR_HDMI21_MAC_TX_LANE2_FFEMODE(_TX0);
                                    }

                                    if((GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0) >= _LTP1) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0) <= _LTP4))
                                    {
                                        // 16b18b encode lane2 disable
                                        rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~_BIT6, 0x00);
                                    }
                                    else
                                    {
                                        // 16b18b encode lane2 enable
                                        rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~_BIT6, _BIT6);
                                    }

                                }

                                // _HDMI21_FRL_6G_3LANES=2
                                if(GET_HDMI21_MAC_TX_FRL_RATE(_TX0) > 2)
                                {
                                    // Lane3
                                    if(GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0) == 0x0E)
                                    {
                                        InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] L3 FFE REQ=%d/%d\n", (UINT32)GET_HDMI21_MAC_TX_LANE3_FFE(_TX0), (UINT32)GET_HDMI21_MAC_TX_FFE_LEVEL(_TX0));
                                        ADD_HDMI21_MAC_TX_LANE3_FFE(_TX0);

                                        if(GET_HDMI21_MAC_TX_LANE3_FFE(_TX0) > GET_HDMI21_MAC_TX_FFE_LEVEL(_TX0))
                                        {
                                            CLR_HDMI21_MAC_TX_LANE3_FFE(_TX0);
                                        }

                                        // need check if stay in LTP4 , config FFE test mode
                                        if(((rtd_inl(HDMITX21_LINK_TRAINING_HDMI21_LT_4_reg) & 0xF0) >> 4) == _LTP4)
                                        {
                                            if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT3) // NO FFE
                                            {
                                                SET_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0, _HDMI_2_1_NO_FFE);
                                            }
                                            else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT2) // De emphasis
                                            {
                                                SET_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0, _HDMI_2_1_DE_EMPHASIS_ONLY);
                                            }
                                            else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT1) // PRE shoot only
                                            {
                                                SET_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0, _HDMI_2_1_PRE_SHOOT_ONLY);
                                            }
                                            else
                                            {
                                                CLR_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0);
                                            }
                                            DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC L3 LTP4, FFE set 0x35 test config=%x\n", (UINT32)GET_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0));
                                        }

                                        // Set Tx APHY FFE level
                                        ScalerGDIPhyHdmiTxHdmi21SetTxFfeLevel(ScalerHdmi21MacTxPxMapping(_TX0), _HDMI21_LANE3, GET_HDMI21_MAC_TX_LANE3_FFE(_TX0), GET_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0));
                                    }
                                    else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0) >= _LTP1) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0) <= _LTP8))
                                    {
                                        if(GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0) == _LTP3) // Nyquist Clock
                                        {
                                            // [DANBU-813]: When FLT_no_timeout is cleared (=0), clear (=0) FLT_update and continue sending the previous pattern.
                                            if(GET_HDMI21_MAC_TX_FRL_NOTIMEOUT(_TX0) == _TRUE) // NO TIMEOUT FLAG
                                            {
                                                CLR_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0);
                                            }
                                            else
                                            {
                                                SET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0, ((rtd_inl(HDMITX21_LINK_TRAINING_HDMI21_LT_4_reg) & 0xF0) >> 4));
                                                SET_HDMI21_MAC_TX_CTS_IGNORE_LTP_EN();
                                            }
                                        }
                                        else if(GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0) == _LTP4)
                                        {
                                            if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT3) // NO FFE
                                            {
                                                SET_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0, _HDMI_2_1_NO_FFE);
                                            }
                                            else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT2) // De emphasis
                                            {
                                                SET_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0, _HDMI_2_1_DE_EMPHASIS_ONLY);
                                            }
                                            else if((GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & (_BIT3 | _BIT2 | _BIT1)) == _BIT1) // PRE shoot only
                                            {
                                                SET_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0, _HDMI_2_1_PRE_SHOOT_ONLY);
                                            }
                                            else
                                            {
                                                CLR_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0);
                                            }

                                            ScalerGDIPhyHdmiTxHdmi21SetTxFfeLevel(ScalerHdmi21MacTxPxMapping(_TX0), _HDMI21_LANE3, GET_HDMI21_MAC_TX_LANE3_FFE(_TX0), GET_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0));

                                            InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] LTP4: SCDC L3 Pattern req=%x\n", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0));
                                        }
                                        else // LTP other
                                        {

                                            DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC L3 Pattern req =%d\n", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0));

                                            CLR_HDMI21_MAC_TX_LANE3_FFEMODE(_TX0);
                                        }

                                        if((GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0) >= _LTP1) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0) <= _LTP4))
                                        {
                                            // 16b18b encode lane3 disable
                                            rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~_BIT7, 0x00);
                                        }
                                        else
                                        {
                                            // 16b18b encode lane3 enable
                                            rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~_BIT7, _BIT7);
                                        }

                                    }
                                }

                                // Set 4Lane link training pattern
                                // L0 check LTP1~8
                                if((GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0) >= _LTP1) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0) <= _LTP8))
                                {
                                    ucL0L1Pattern = ucL0L1Pattern | GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0);
                                }
                                else
                                {
                                    ucL0L1Pattern = ucL0L1Pattern | (rtd_inl(HDMITX21_LINK_TRAINING_HDMI21_LT_3_reg) & 0x0F);
                                }

                                if((GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0) >= _LTP1) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0) <= _LTP8))
                                {
                                    ucL0L1Pattern = ucL0L1Pattern | (GET_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0) << 4);
                                }
                                else
                                {
                                    ucL0L1Pattern = ucL0L1Pattern | ((rtd_inl(HDMITX21_LINK_TRAINING_HDMI21_LT_3_reg) & 0xF0));
                                }

                                WarnMessageHDMI21Tx("[HDMI21_TX] L0/1 FP=%x\n", (UINT32)ucL0L1Pattern);

                                if((GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0) >= _LTP1) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0) <= _LTP8))
                                {
                                    ucL2L3Pattern = ucL2L3Pattern | GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0);
                                }
                                else
                                {
                                    ucL2L3Pattern = ucL2L3Pattern | (rtd_inl(HDMITX21_LINK_TRAINING_HDMI21_LT_4_reg) & 0x0F);
                                        }

                                if(GET_HDMI21_MAC_TX_FRL_RATE(_TX0) > 2)
                                {
                                    if((GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0) >= _LTP1) && (GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0) <= _LTP8))
                                    {
                                        ucL2L3Pattern = ucL2L3Pattern | (GET_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0) << 4);
                                    }
                                    else
                                    {
                                        ucL2L3Pattern = ucL2L3Pattern | (rtd_inl(HDMITX21_LINK_TRAINING_HDMI21_LT_4_reg) & 0xF0);
                                    }
                                }

                                WarnMessageHDMI21Tx("[HDMI21_TX]  L2/3 FP=%x\n", (UINT32)ucL2L3Pattern);

                                // [DANBU-813]: When FLT_no_timeout is cleared (=0), clear (=0) FLT_update and continue sending the previous pattern.
                                if(GET_HDMI21_MAC_TX_CTS_IGNORE_LTP_EN(_TX0) == 1){
                                    WarnMessageHDMI21Tx("[HDMI21_TX] Ignore LTP3\n");
                                }else
                                {
                                    rtd_outl(HDMITX21_LINK_TRAINING_HDMI21_LT_3_reg, ucL0L1Pattern);
                                    rtd_outl(HDMITX21_LINK_TRAINING_HDMI21_LT_4_reg, ucL2L3Pattern);

                                    // reset LTP change status
                                    rtd_maskl(HDMITX21_LINK_TRAINING_HDMI21_LT_1_reg, ~HDMITX21_LINK_TRAINING_HDMI21_LT_1_ltp5678_chg_done_mask, HDMITX21_LINK_TRAINING_HDMI21_LT_1_ltp5678_chg_done_mask);

                                    // LTP_CHG
                                    rtd_maskl(HDMITX21_LINK_TRAINING_HDMI21_LT_1_reg, ~(_BIT2 | _BIT1), _BIT2);

                                    //InfoMessageHDMI21Tx("[HDMI21_TX] LT: SCDC L2 & L3 Set finial pattern=%x\n", (UINT32)ucL2L3Pattern);
                                    if(GET_HDMI21_MAC_TX_FRL_RATE(_TX0) > 2)
                                    {
                                        // Set Lane0~3 link training enable
                                        rtd_maskl(HDMITX21_LINK_TRAINING_HDMI21_LT_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT7 | _BIT6 | _BIT5 | _BIT4));
                                    }
                                    else
                                    {
                                        // Set Lane0~2 link training enable
                                        rtd_maskl(HDMITX21_LINK_TRAINING_HDMI21_LT_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT6 | _BIT5 | _BIT4));
                                    }

                                    // Tx data from link training module
                                    rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~_BIT3, 0x00);

                                    // enable 18bit to 20bit fifo setting
                                    rtd_maskl(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT2 | _BIT1 | _BIT0), _BIT7);

                                    #if 1 //  FRL FIFO error when AC ON
                                     rtd_maskl(HDMITX21_MAC_HDMI21_TX_CTRL_reg, ~(_BIT7), _BIT7);
                                    #endif

                                    HDMITX_MDELAY(1);
                                }

                                ucSCDCWritedata = _BIT5;

                                subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                                subEdidAddr[1] = ucSCDCWritedata;
                                ret = ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x02,  0x01,_HDMI21_TX0_IIC);
                                if(ret != _SUCCESS)
                                    ErrorMessageHDMI21Tx("[HDMI21_TX] write FLT_Update fail@LTS3\n");

                                CLR_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0);


                                DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] write FLT_Update=%d@LTS3\n", (UINT32)subEdidAddr[1]);
                            }
                        }
                        else
                        {
                            CLR_HDMI21_MAC_TX_FROM_RX_SCDC41_L0_LTP_REQ(_TX0);
                            CLR_HDMI21_MAC_TX_FROM_RX_SCDC41_L1_LTP_REQ(_TX0);
                            CLR_HDMI21_MAC_TX_FROM_RX_SCDC42_L2_LTP_REQ(_TX0);
                            CLR_HDMI21_MAC_TX_FROM_RX_SCDC42_L3_LTP_REQ(_TX0);
                        }
                    }
                    else
                    {
                        //DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] wait SCDC 0x10(%x) _BIT5(%d)@LTS3...\n", GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0), GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT5);
                        // check RSED error status
                        if(0)
                        _SCDC_check_RSED_CED_Status();
                    }
                }
                else
                {
                    ErrorMessageHDMI21Tx("[HDMI21_TX] read SCDC 0x10 FAIL@LTS3\n");
                }
            }
            else{
                static UINT32 dumpCnt=0, lastDumpStc=0, lastDumpCnt=0;
                UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
		stc= (stc - lastDumpStc);
                if((UINT32)ABS1((INT32)stc) > FRL_START_TIMEOUT){
                    ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX][%d]\n", (UINT32)(dumpCnt-lastDumpCnt));
                    //ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX]LTS3_TIMER_START==0@Cnt[%d]\n", (UINT32)ScalerTimerSearchActiveTimerEvent(TIMER_HDMITX_LT_CHECK, _SCALER_TIMER_EVENT_HDM21_TX0_LTS3_TIMER));
                    ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX]Timer[%d]@LTS3\n", (UINT32)GET_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0));
                    lastDumpStc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
                    lastDumpCnt = dumpCnt;
                }
                dumpCnt++;
            }
            break;

        case _HDMI21_LT_LTS4:
            colorBitIndex = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_DEPTH);
            videoFmtIndex = ScalerHdmiTxGetScalerVideoFormatIndex();
            targetFrlRate = ScalerHdmiTxPhy_GetPLLTimingInfo_frlRate(videoFmtIndex, colorBitIndex);

            DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] VideoFmtIdx[%d]\n", (UINT32)videoFmtIndex);
            DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] Bit[%d] \n", (UINT32)colorBitIndex);
            ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] FRL=%d@LTS4\n", (UINT32)targetFrlRate);

            // Stop link training
            rtd_maskl(HDMITX21_LINK_TRAINING_HDMI21_LT_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), 0x00);

            // HDMI2.1 Tx disable
            //rtd_maskl(P55_10_HDMI21_DP14_TX_PHY_CTRL0, ~_BIT6, 0x00); //  NOT H5X HW REGISTER, bypass this command, H5X may just need reconfig TX PHY setting

            // [FRL_RATE] Check FRL_Rate can reduce or not(2 is 3G 4lanes)
            if(/*(GET_HDMI21_MAC_TX_FRL_RATE(_TX0) <= targetFrlRate)
                || */((GET_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0) > _HDMI21_LINK_TRAINING_TIMEOUT) && (GET_HDMI21_MAC_TX_FRL_NOTIMEOUT(_TX0) == _FALSE)))
            {
                static UINT32 lastDumpStc = 0, dumpCnt=0;
                UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
		   stc= (stc - lastDumpStc);
                // re-start link training
                ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_TRAINING);
                ScalerHdmi21MacTxLinktrainingResetToLTS1((EnumOutputPort)_TX0);

#if 1 // [HFR1-68] update previous FRL rate to re-start link training flow
                SET_HDMI21_MAC_TX_FRL_RATE(_TX0, targetFrlRate);
                SCALER_HDMITX_SET_TARGET_FRLRATE(targetFrlRate);
#endif

                if((UINT32)ABS1((INT32)stc) > 90000){ // dump period=1sec
                    InfoMessageHDMI21TxLinkTraining("[HDMI_TX][%d] Link Training fail\n", (UINT32)dumpCnt);
                    ErrorMessageHDMI21Tx("[HDMI_TX]@FRL Current/Target=%d/%d, Re-start\n", (UINT32)GET_HDMI21_MAC_TX_FRL_RATE(_TX0), (UINT32)targetFrlRate);
                    //InfoMessageHDMI21TxLinkTraining("[HDMI_TX] AMP=%x/%x\n", (UINT32)rtd_inl(HDMITX_PHY_HDMITXPHY_CTRL14_reg), (UINT32)rtd_inl(HDMITX_PHY_HDMITXPHY_CTRL15_reg));
                    //InfoMessageHDMI21TxLinkTraining("[HDMI_TX] AMP=%x\n",  (UINT32)rtd_inl(HDMITX_PHY_HDMITXPHY_CTRL16_reg) & 0xffff0000);
                    lastDumpStc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
                    dumpCnt = 0;
                }
                dumpCnt++;

            #if 0 // dump register setting
                static UINT8 bDumpReg=0;
                extern void hdmi21_DumpReg_VIC196_8kp30_12bit_12G4L(void);
                ROSTimeDly(100);
                if(!bDumpReg){
                    NoteMessageHDMITx("\n\n\n[TEST] DUMP REG!!\n");
                    hdmi21_DumpReg_VIC196_8kp30_12bit_12G4L();
                    NoteMessageHDMITx("[TEST] END\n\n\n\n");
                }
                bDumpReg = 1;
            #endif
            }
            else    // LTP=0xf (change FRL rate)
            {
                // DEBUG_FIX_FRL_RATE // FIX TX OUTPUT FRL RATE
                // Reduce FRL rate and re-link training
                SET_HDMI21_MAC_TX_FRL_RATE(_TX0, GET_HDMI21_MAC_TX_FRL_RATE(_TX0) - 1);
#if 1 // [HFR1-68] change FRL rate (-1) for next link training flow
                SCALER_HDMITX_SET_TARGET_FRLRATE(GET_HDMI21_MAC_TX_FRL_RATE(_TX0));
                CLR_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0);
                ScalerHdmi21MacTxSetLinkTrainingState(ScalerHdmiMacTxPxMapping(_TX0), _HDMI21_LT_LTS3);
#endif
                ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] down FRL=%d/%d@LTS4\n", (UINT32)GET_HDMI21_MAC_TX_FRL_RATE(_TX0), (UINT32)ScalerHdmiTxGetTargetFrlRate());

                // Set FFE max level for current FRL Rate (define in project.h)
                SET_HDMI21_MAC_TX_FFE_LEVEL(_TX0, _HW_HDMI21_SET_FFE_LEVEL);

                // Reset Tx Lane0 FFE level
                CLR_HDMI21_MAC_TX_LANE0_FFE(_TX0);

                // Reset Tx Lane1 FFE level
                CLR_HDMI21_MAC_TX_LANE1_FFE(_TX0);

                // Reset Tx Lane2 FFE level
                CLR_HDMI21_MAC_TX_LANE2_FFE(_TX0);

                // Reset Tx Lane3 FFE level
                CLR_HDMI21_MAC_TX_LANE3_FFE(_TX0);

                #if 1 //
                ScalerHdmiPhyTxSet(ScalerHdmiMacTxPxMapping(_TX0));
                #else
                // SET PHY
                // ScalerHdmiComboDPTxRingCMUPowerOn(GET_HDMI_2_1_TX_FRL_RATE());
                ScalerGDIPhyHdmiTxHdmi21RingCMUPowerOn(ScalerHdmi21MacTxPxMapping(_TX0), GET_HDMI21_MAC_TX_FRL_RATE(_TX0));
                ScalerGDIPhyHdmiTxHdmi21MLPHYSet(ScalerHdmi21MacTxPxMapping(_TX0));
                ScalerHdmiPhyTxCHPower(ScalerHdmi21MacTxPxMapping(_TX0), _ENABLE);
                #endif
                SET_HDMI21_MAC_TX_FRL_SETPHY(_TX0);
                // HDMI2.1 Tx disable
                // rtd_maskl(P55_10_HDMI21_DP14_TX_PHY_CTRL0, ~_BIT6, _BIT6); //  NOT H5X HW REGISTER, bypass this command, H5X may just need reconfig TX PHY setting

                // Start link training
                if(GET_HDMI21_MAC_TX_FRL_RATE(_TX0) < 3)
                {
                    rtd_maskl(HDMITX21_LINK_TRAINING_HDMI21_LT_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT6 | _BIT5 | _BIT4));
                }
                else
                {
                    rtd_maskl(HDMITX21_LINK_TRAINING_HDMI21_LT_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT7 | _BIT6 | _BIT5 | _BIT4));
                }

                // Set SCDC FRL_Rate and FFE_Level
                ucSCDCWritedata = (GET_HDMI21_MAC_TX_FFE_LEVEL(_TX0) << 4) | GET_HDMI21_MAC_TX_FRL_RATE(_TX0);

                subEdidAddr[0] = _HDMI21_SCDC_OFFSET_31;
                subEdidAddr[1] = ucSCDCWritedata;
                if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI21_TX0_IIC) == _TRUE)
                {
                    SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTS3);

                    //  link training timeout re-start handler
                    if((GET_HDMI21_MAC_TX_LTS3_TIMER_START(_TX0) == _FALSE)
                        && (ScalerTimerSearchActiveTimerEvent(/*TIMER_HDMITX_LT_CHECK, */_SCALER_TIMER_EVENT_HDM21_TX0_LTS3_TIMER) == _FALSE))
                    {
                        SET_HDMI21_MAC_TX_LTS3_TIMER_START(_TX0);
                        CLR_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0);
                        ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] Re-Start LTS3_Timer[%d]@LTS4\n", (UINT32)GET_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0));
                    }
                }

            }

            if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT5){
                //ScalerTimerDelayXms(1);
                ucSCDCWritedata = _BIT5;

                subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                subEdidAddr[1] = ucSCDCWritedata;
                ret = ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI21_TX0_IIC);
                NoteMessageHDMI21TxLinkTraining("[HDMI21_TX] write FLT_Update=1@LTS4\n");
                if(ret != _SUCCESS)
                    ErrorMessageHDMI21Tx("[HDMI21_TX] write FLT_Update fail@LTS4\n");

                CLR_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0);
            }
            break;

        case _HDMI21_LT_LTSP_GAP:

            // Send All Gap pattern
            ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] LTSP_GAP\n");

            // [FRL new mode] save current FRL mode (for debug only)
            rtd_outl(HDMITX21_MAC_HDMI21_FRL_16_reg, GET_HDMI21_MAC_TX_FRL_RATE(_TX0));

            // Enable RS
            rtd_maskl(HDMITX21_MAC_HDMI21_RS_0_reg, ~_BIT7, _BIT7);

            // _HDMI21_FRL_3G_4LANES=2
            if(GET_HDMI21_MAC_TX_FRL_RATE(_TX0) > 2)
            {
                // Set Lane0~3 FRL, Scramble enable, 16b/18b enable
                rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg,  ~((newTxFlow? 0: _BIT7) | _BIT5), ((newTxFlow? 0: _BIT7) | _BIT5));

                rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT7 | _BIT6 | _BIT5 | _BIT4));
            }
            else
            {
                // Set Lane0~2 FRL, Scramble enable, 16b/18b enable
                rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~((newTxFlow? 0: _BIT7) | _BIT5), (newTxFlow? 0: _BIT7));

                rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT6 | _BIT5 | _BIT4));
            }

            // [FRL new mode] Gap Pattern doesn't need VESA clk
            rtd_maskl(HDMITX21_MAC_HDMI21_FRL_12_reg, ~_BIT7, newTxFlow? _BIT7: 0);

            // Enable FRL Gap Pattern and HW auto Gen Gap after Vsync
            if(GET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(_TX0) == _TX_STREAM_READY)
            {
                rtd_maskl(HDMITX21_MAC_HDMI21_FRL_19_reg, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
            }
            else
            {
                // switch to out put GAP immediately
                rtd_maskl(HDMITX21_MAC_HDMI21_FRL_19_reg, ~(_BIT7 | _BIT6), _BIT7);
            }

            // _HDMI21_FRL_3G_4LANES=2 Scramble enable
            if(GET_HDMI21_MAC_TX_FRL_RATE(_TX0) > 2)
            {
                rtd_maskl(HDMITX21_MAC_HDMI21_SCR_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT7 | _BIT6 | _BIT5 | _BIT4));
            }
            else
            {
                rtd_maskl(HDMITX21_MAC_HDMI21_SCR_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT6 | _BIT5 | _BIT4));
            }

            // wait gap_en ready
            udelay(1);

            // Tx data from FRL module
            rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~_BIT3, _BIT3);

            // [FIXME] wait den_end event
            //ScalerTimerDelayXms(1);
            // DSC input from DSC_SRC_DISPD
            // clear status BOOLEAN before wait event
            //rtd_maskl(PPOVERLAY_DTG_pending_status_reg, ~PPOVERLAY_DTG_pending_status_mv_den_end_event_mask, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask);
            HDMITX_DTG_Wait_Den_Stop_Done();//Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,1);

            if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT5){
                ucSCDCWritedata = _BIT5;

                subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                subEdidAddr[1] = ucSCDCWritedata;
                ret = ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x02, 0x01, _HDMI21_TX0_IIC);
                DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] write FLT_Update@LTSP_GAP\n");
                if(ret != _SUCCESS)
                    ErrorMessageHDMI21Tx("[HDMI21_TX] write FLT_Update fail@LTSP_GAP\n");

                //[CTS] wait packet check done
                HDMITX_MDELAY(30);
                CLR_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0);
            }

            SET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(_TX0, _LINK_TRAING_PASS);
            ucLTFailCnt = 0;
            ucScdcStatusFailCnt = 0;
            ScalerDpStreamSetTxCurrentOperatePixelMode(ScalerHdmi21MacTxPxMapping(_TX0), _FOUR_PIXEL_MODE);
            ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDM21_TX0_LTS3_TIMER);
            ScalerTimerActiveTimerEvent(TIMER_HDMITX_LT_CHECK, _SCALER_TIMER_EVENT_HDM21_TX0_FRL_PASS_TIMER);
            break;

        case _HDMI21_LT_LTSL:

            // Disable HDMI2.1 Module
            rtd_maskl(HDMITX21_MAC_HDMI21_TX_CTRL_reg, ~_BIT7, 0x00);
            // NOT H5X Reg. // rtd_maskl(P2A_48_PLL_CONTROL, ~_BIT0, _BIT0); // Power Down hdmi21 pLL
            ScalerHdmiPhyTxCMUPowerOff(ScalerHdmi21MacTxPxMapping(_TX0));

            SET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(_TX0, _LINK_TRAING_LEGACY);
            ScalerDpStreamSetTxCurrentOperatePixelMode(ScalerHdmi21MacTxPxMapping(_TX0), _ONE_PIXEL_MODE);
            CLR_HDMI21_MAC_TX_FRL_RATE(_TX0);
            ucSCDCWritedata = (GET_HDMI21_MAC_TX_FFE_LEVEL(_TX0) << 4) | GET_HDMI21_MAC_TX_FRL_RATE(_TX0);
            DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] LTSL SCDC 0x31 (%x) write FRL RATE = 0\n", (UINT32)ucSCDCWritedata);

            subEdidAddr[0] = _HDMI21_SCDC_OFFSET_31;
            subEdidAddr[1] = ucSCDCWritedata;
            if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI21_TX0_IIC) == _TRUE)
            {
                ErrorMessageHDMI21Tx("[HDMI21_TX] ENTER _LINK_TRAING_LEGACY\n");
            }
            else
            {
                ErrorMessageHDMI21Tx("[HDMI21_TX] Write IIC FAIL\n");
            }

#if 1
            if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT5){
                //ScalerTimerDelayXms(1);
                ucSCDCWritedata = _BIT5;

                subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                subEdidAddr[1] = ucSCDCWritedata;
                ret = ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI21_TX0_IIC);
                NoteMessageHDMI21TxLinkTraining("[HDMI21_TX] write SCDC 0x10 _BIT5@LTS:L\n");
                if(ret != _SUCCESS)
                    ErrorMessageHDMI21Tx("[HDMI21_TX] write FLT_Update fail@LTS:L\n");

                CLR_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0);
            }
#endif
#if 1 //  [HFR1-68] reset Link training state before switch to TMDS mode
            SET_HDMI21_MAC_TX_FRL_RATE(_TX0, _HDMI21_FRL_NONE);
            SCALER_HDMITX_SET_TARGET_FRLRATE(_HDMI21_FRL_NONE);
#endif

            break;

        default:
            break;
    }
}

void ScalerHdmi21MacTx0_Get_SourceTestConfiguration(void)
{
    I2C_HDMITX_PARA_T i2c_para_info;

    i2c_para_info.addr = _HDMI21_SCDC_SLAVE_ADDR;
    i2c_para_info.sub_addr_len = 0x01;
    i2c_para_info.read_len = 0x01;
    i2c_para_info.wr_len = 0;
    // [CTS][HFR1-10] check Source Test Configuration if Source_Test_Update Flag(_BIT3)=1
    subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
    if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _TRUE)
    {
        SET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0, pEdidData[0]);
        // check [5] FLT_update, [4] FLT_start, [3] Sourece_Test_Update, [2] RR_test
        if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & (0x3c))
            ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC 0x10=%x@srcTest\n", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0));
        if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT3)
        {
            subEdidAddr[0] = _HDMI21_SCDC_OFFSET_35;
            if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _TRUE)
            {
                SET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0, pEdidData[0]);
                _Linktraining_SCDC35_Config(); // update SCDC 0x35 info
            }
            else
            {
                ErrorMessageHDMI21Tx("[HDMI21_TX] read SCDC 0x35 FAIL@GetSrcTestCfg!!\n");
            }

            subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
            subEdidAddr[1] = _BIT3;
            if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x02,  0x01,_HDMI21_TX0_IIC) == _SUCCESS)
            {
                DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] write SrcTestUpdate@GetSrcTestCfg\n");
            }else{
                ErrorMessageHDMI21Tx("[HDMI21_TX] write SrcTestUpdate FAIL@GetSrcTestCfg\n");
            }
        }

        if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT4)
        {
            static UINT32 lastStc_frlstart = 0;

            UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
            if((((UINT32)(stc - lastStc_frlstart)) > PRINT_5SEC)){
                ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] RCV FRL_START=1@GetSrcTestCfg\n");
                lastStc_frlstart = stc;
            }
            DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] LTPP=%d\n", (UINT32)GET_HDMI21_MAC_TX_LTS_PASS_POLLING(_TX0));
            DebugMessageHDMI21TxLinkTraining("[HDMI21_TX]  SS=%d\n", (UINT32)ScalerHdmiMacTxGetStreamSource(ScalerHdmi21MacTxPxMapping(_TX0)));
            DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] VS=%d\n", (UINT32) (UINT32)GET_HDMI21_MAC_VIDEO_START(_TX0));
            DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] GapEn=%d\n", (UINT32)HDMITX21_MAC_HDMI21_FRL_19_get_gap_en(rtd_inl(HDMITX21_MAC_HDMI21_FRL_19_reg)));
        }
    }
    else
    {
        ErrorMessageHDMI21Tx("[HDMI21_TX] read SCDC 0x10 FAIL@GetSrcTestCfg\n");
    }

    return;
}

//--------------------------------------------------
// Description  : Hdmi 2p1 linktraing flow
// Input Value  : port
// Output Value : TRUE state OK , _FALSE => jump back to link traing state
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTx0LinktrainingPassCheck(void)
{
    UINT8 value_u8;
    static UINT32 lastDumpStc = 0;
    I2C_HDMITX_PARA_T i2c_para_info;
    static UINT32 last_ltResult = 0;
    UINT8 ltResult = GET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(_TX0);
    UINT8 ucSCDCWritedata = 0;
    INT8 ret;
    UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
    if(ltResult != _LINK_TRAING_PASS)
        lastDumpStc = 0;
    else if((last_ltResult != ltResult) && (ltResult == _LINK_TRAING_PASS))
        lastDumpStc = stc;

    last_ltResult = ltResult;
    i2c_para_info.addr = _HDMI21_SCDC_SLAVE_ADDR;
    i2c_para_info.sub_addr_len = 0x01;
    i2c_para_info.read_len = 0x01;
    i2c_para_info.wr_len = 0;

    if(ScalerHdmiTxGetBypassLinkTrainingEn())
        return _TRUE;

    if(GET_HDMI21_MAC_TX_LTS_PASS_POLLING(_TX0) == _TRUE)
    {
        CLR_HDMI21_MAC_TX_LTS_PASS_POLLING(_TX0);

        if(GET_HDMI21_MAC_VIDEO_START(_TX0) != _TRUE)
        {
            ScalerTimerActiveTimerEvent(TIMER_HDMITX_LT_CHECK, _SCALER_TIMER_EVENT_HDM21_TX0_FRL_PASS_TIMER);
        }
        else
        {
            ScalerTimerActiveTimerEvent(TIMER_HDMITX_LT_STABLE_CHECK, _SCALER_TIMER_EVENT_HDM21_TX0_FRL_PASS_TIMER);
        }

        if(GET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(_TX0) == _LINK_TRAING_PASS)
        {
            subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
            if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _TRUE)
            {
                // [CTS][HFR1-10] check Source Test Configuration if Source_Test_Update Flag(_BIT3)=1
                SET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0, pEdidData[0]);
                // check [5] FLT_update, [4] FLT_start, [3] Sourece_Test_Update, [2] RR_test
                if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & (0x3c))
                    ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC 0x10=%x@LTPassCheck\n", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0));
                if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT3){
                    subEdidAddr[0] = _HDMI21_SCDC_OFFSET_35;
                    if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _TRUE)
                    {
                        SET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0, pEdidData[0]);
                        _Linktraining_SCDC35_Config(); // update SCDC 0x35 info

                        // B[7]: FRL_Max: Sources shall enable the link with the lesser of the Max_FRL_Rate from the Sink��s SCDS and the maximum FRL_Rate that they support
                        if(GET_HDMI21_MAC_TX_FROM_RX_SCDC35_TESTCONFIG(_TX0) & _BIT7){
                            ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] SrcTestCfg FRL_Max=1\n");
                        }
                    }
                    else
                    {
                        ErrorMessageHDMI21Tx("[HDMI21_TX] read SCDC 0x35 FAIL@LTPassCheck!!\n");
                    }

                    subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                    subEdidAddr[1] = _BIT3;
                    if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x02,  0x01,_HDMI21_TX0_IIC) == _SUCCESS)
                    {
                        DebugMessageHDMI21Tx("[HDMI21_TX] write SrcTestUpdate@LTPassCheck\n");
                    }else{
                        ErrorMessageHDMI21Tx("[HDMI21_TX] write SrcTestUpdate FAIL@LTPassCheck\n");
                    }
                }

                // SCDC FLT_Update = 1: handle link training retrain
                if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT5) {
                    ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] RCV FLT_Update=1@LTPassCheck\n");

#if 0 // re-start link training from LTS2 -> LTS3
                    SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTS2);
                    ScalerHdmi21MacTx0LinktrainingReset();
#else // re-start link training from LTS3

                    // 1. use AV mute if video is active
                    if((GET_HDMI_MAC_TX0_MODE_STATE() == _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON) && (rtd_getbits(HDMITX21_MAC_HDMI21_FRL_19_reg, _BIT7) == 0)){
                        ScalerHdmiTxMuteEnable(_ENABLE);
                        ScalerHdmiTxBypassAVMuteAndPacketCtrl(_DISABLE);
                        CLR_HDMI21_MAC_VIDEO_START(_TX0);
                    }

                    // 2. stop FRL transmission including Gap-character-only transmission initiated in STEP 1 within 200 ms of the Stop condition where FLT_update=1 was read
                    // FRL disable & reset
                    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~(_BIT7|_BIT6), _BIT6);
                    udelay(50);

                    // disable link training module
                    ScalerHdmi21MacTx0LinktrainingReset();

                    HDMITX_DTG_Wait_Den_Stop_Done();
#endif
                    // 3. clear FLT_update by writing ��1��
                    ucSCDCWritedata = _BIT5;
                    subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                    subEdidAddr[1] = ucSCDCWritedata;
                    ret = ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI21_TX0_IIC);
                    if(ret != _SUCCESS)
                        ErrorMessageHDMI21Tx("[HDMI21_TX] write FLT_Update fail@LTPassCheck\n");
                    else
                        DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] SET FLT_Update=%x@LTPassCheck\n", (UINT32)subEdidAddr[1]);

                    // 4. EXIT to LTS:3 or retrain
                    SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTS3);
                    CLR_HDMI21_MAC_TX_LTS3_TIMER_COUNTER(_TX0);
                    SET_HDMI21_MAC_TX_CTS_LT_RETRAIN_EN(_TX0);

                    return _FALSE;
                }
                // SCDC FRL_Start = 1
                else if(GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0) & _BIT4){
                    // switch video here
                    if((ScalerHdmiMacTxGetStreamSource(ScalerHdmi21MacTxPxMapping(_TX0)) == _HDMI_TX_SOURCE_HDMI21)
                        &&(waitOutTgStable()))
                    {
                        if((HDMITX21_MAC_HDMI21_FRL_19_get_gap_en(rtd_inl(HDMITX21_MAC_HDMI21_FRL_19_reg)))){
                            ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] FRL_START=1@LTPassCheck\n");
                            ScalerHdmi21MacTx0FRLReset(GET_HDMI21_MAC_TX_BORROW(_TX0));

                            ScalerHdmi21MacTx0SetFRLVideoStartCnt(_FRL_CNT_OFFSET_NONE);

                            rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~_BIT7, _BIT7);

                            udelay(50);
                            ScalerHdmi21MacTx0FRLVideoStartCntRetry();

                            // stream ready FRL path, set Mux here to out put video
                            rtd_maskl(HDMITX21_MAC_HDMI21_FRL_19_reg, ~_BIT7, 0x00);
                            SET_HDMI21_MAC_VIDEO_START(_TX0);

                        }

                        // Write 1 to clear SCDC FRL_Start
                        ucSCDCWritedata = _BIT4;

                        subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                        subEdidAddr[1] = ucSCDCWritedata;
                        ret = ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI21_TX0_IIC);
                        if(ret != _SUCCESS)
                            ErrorMessageHDMI21Tx("[HDMI21_TX] write FRL_START fail@LTPassCheck\n");

                        DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] SET FRL_START=%x@LTPassCheck!\n", (UINT32)subEdidAddr[1]);
                        // [H5X_HDMITX_BYPASS_HPD_EDID]
                        if(ScalerHdmiTxGetBypassLinkTrainingEn() == _DISABLE){
                            #if 0 // dump register setting
                            static UINT8 bDumpReg=0;
                            extern void hdmi21_DumpReg_VIC196_8kp30_12bit_12G4L(void);
                            ROSTimeDly(100);
                            if(!bDumpReg){
                                NoteMessageHDMITx("[TEST] DUMP REG!!\n");
                                hdmi21_DumpReg_VIC196_8kp30_12bit_12G4L();
                            }
                            bDumpReg = 1;
                            #endif
                        }
                    }else{

#ifdef _FIXME_H5X_SCALER_FLOW
                        if((INT32)ABS(stc - lastDumpStc) > FRL_START_TIMEOUT)
                            ErrorMessageHDMI21Tx("[HDMI21_TX][SCDC 0x10=%x] Wait StreamSrc(%d)@%d ms\n", (UINT32)pEdidData[0], (UINT32)ScalerHdmiMacTxGetStreamSource(_P0_OUTPUT_PORT), (UINT32)(stc - ltPassStc)/90);
                        else
                            InfoMessageHDMITx("[HDMI21_TX][SCDC 0x10=%x] Wait StreamSrc(%d)@%d ms\n", (UINT32)pEdidData[0], (UINT32)ScalerHdmiMacTxGetStreamSource(_P0_OUTPUT_PORT), (UINT32)(stc - ltPassStc)/90);
#endif
                    }

                    return _TRUE;
                }

                // check Sink device link status
                _SCDC_check_RSED_CED_Status();

            }

#if(_HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING == _ON) // [TEST]support fast timing change flow
            if(ScalerHdmiTxGetFastTimingSwitchEn() && (GET_HDMI_MAC_TX0_MODE_STATE() != _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON)){
                ;
            }else if(ScalerHdmiTxGetFastTimingSwitchEn() && HDMITX21_MAC_HDMI21_FRL_19_get_gap_en(rtd_inl(HDMITX21_MAC_HDMI21_FRL_19_reg)) && (GET_HDMI_MAC_TX0_MODE_STATE() == _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON)){
                    NoteMessageHDMI21TxLinkTraining("[HDMI21_TX] Wait Stream Ready...\n");
            }else
#endif
            if(HDMITX21_MAC_HDMI21_FRL_19_get_gap_en(rtd_inl(HDMITX21_MAC_HDMI21_FRL_19_reg)))
            {
                // re-start link training when wait FRL_start timeout
                stc = (stc - lastDumpStc);
                if((UINT32)ABS1(stc) > FRL_START_TIMEOUT){
                    ErrorMessageHDMI21Tx("[HDMI21_TX] Wait FRL_START TIMEOUT %x(%d ms)!!@LTPassCheck\n", (UINT32)lastDumpStc, (UINT32)(stc/90));
                    ScalerHdmi21MacTxLinktrainingResetToLTS1((EnumOutputPort)_TX0);
                    return _FALSE;
                }else{
                    DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] Wait FRL_START(%d ms)...\n", (stc-lastDumpStc)/90);
                }
            }

            //check Sink device Link Status after Link Training Pass
            if((HDMITX21_MAC_HDMI21_FRL_19_get_gap_en(rtd_inl(HDMITX21_MAC_HDMI21_FRL_19_reg)) == 0)
                /*&& ((rtd_inl(PPOVERLAY_OUTTG_OUTTG_watchdog_CTRL_reg) & PPOVERLAY_OUTTG_OUTTG_watchdog_CTRL_dout_force_bg_mask) == 0)*/ //fix by zhaodong
                && (ucScdcStatusFailCnt < SCDC_0x40_CHECK_FAIL_MAX))
            {
                subEdidAddr[0] = _HDMI21_SCDC_OFFSET_40;
                if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _TRUE)
                {
                    ucScdcStatusFailCnt = 0;
                    SET_HDMI21_MAC_TX_FROM_RX_SCDC40_STATUSFLAG(_TX0, pEdidData[0]);
                    // check sink device link status
                    // Status Flags: 0x40: B[0]: Clock_Detected, B[6]: FTL_ready, B[4:1]: Ch0_Ln3/2/1/0_Locked,B[7]: DSC_DecodeFail
                    value_u8 = GET_HDMI21_MAC_TX_FROM_RX_SCDC40_STATUSFLAG(_TX0) & SCDC_0X40_4LANE_LOCK_MASK;
                    if((value_u8 == SCDC_0X40_4LANE_LOCK_MASK)||((value_u8 == SCDC_0X40_3LANE_LOCK_MASK) && (GET_HDMI21_MAC_TX_FRL_RATE(_TX0) <= 2)))
                        ucLTFailCnt = 0;
                    else
                        ucLTFailCnt++;

                    if(ucLTFailCnt){
                        ErrorMessageHDMI21TxLinkTraining("[SCDC] %x/", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC10_UPDATEFLAG(_TX0));
                        ErrorMessageHDMI21TxLinkTraining("%x@FailCnt=%x\n", (UINT32)GET_HDMI21_MAC_TX_FROM_RX_SCDC40_STATUSFLAG(_TX0), (UINT32) ucLTFailCnt);
                        if(ucLTFailCnt > SCDC_0x40_LANE_UNLOCK_MAX){
                            ScalerHdmi21MacTxLinktrainingReset(ScalerHdmiMacTxPxMapping(_TX0));
                            return _FALSE;
                        }
                    }
                }else{
                    ucScdcStatusFailCnt++;
                    ErrorMessageHDMI21TxLinkTraining("[HDMI21_TX] SCDC 0x40 READ FAIL(%d)@PassCheck\n", (UINT32)ucScdcStatusFailCnt);
                }
            }

            // Judge source &  sink all support hdcp22 => can support hdmi21, if not change to legacy mode
            return _TRUE;
        }else

        if(GET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(_TX0) == _LINK_TRAING_LEGACY)
        {
            if(ScalerHdmiMacTxEdidGetFeature(ScalerHdmi21MacTxPxMapping(_TX0), _SCDC_PRESENT) == _TRUE) // Check EDID SCDC present
            {
#if 1 // HFR1-68
                // CLR SCDC FRL_Rate
                ucSCDCWritedata = 0x00;
                subEdidAddr[0] = _HDMI21_SCDC_OFFSET_31;
                subEdidAddr[1] = ucSCDCWritedata;
                ErrorMessageHDMI21Tx("[HDMI21_TX] Set SCDC0x31=0@Legacy");
                if(ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI21_TX0_IIC) == _FALSE)
                {
                    ErrorMessageHDMI21Tx("FAIL\n");
                }else
                     ErrorMessageHDMI21Tx("Pass\n");

#endif

                subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                i2c_para_info.sub_addr_len = 0x01;
                if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x01,0x01, _HDMI21_TX0_IIC) == _TRUE)
                {
                    NoteMessageHDMI21Tx("[HDMI21_TX] GET FLT_Update=%x@LTLegacy\n", pEdidData[0]);
                    // SCDC FLT_Update = 1
                    if((pEdidData[0] & _BIT5) == _BIT5)
                    {
                        // Write 1 to clear SCDC FLT_Update
                        pEdidData[0] = _BIT5;
                        ucSCDCWritedata = _BIT5;
                        subEdidAddr[0] = _HDMI21_SCDC_OFFSET_10;
                        subEdidAddr[1] = ucSCDCWritedata;
                        ret = ScalerMcuHwIICWrite(HDMI_TX_I2C_BUSID, _HDMI21_SCDC_SLAVE_ADDR, subEdidAddr,0x02,0x01, _HDMI21_TX0_IIC);
                        InfoMessageHDMI21TxLinkTraining("[HDMI21_TX] write FLT_Update=%x@LTLegacy\n", subEdidAddr[1]);
                        return _TRUE;
                    }
                }
            }

            return _TRUE;
        }
        else if(GET_HDMI21_MAC_TX_RE_LINK_TRAINING(_TX0) == _TRUE)
        {
            CLR_HDMI21_MAC_TX_RE_LINK_TRAINING(_TX0);
            ScalerHdmi21MacTxLinktrainingReset(ScalerHdmiMacTxPxMapping(_TX0));
            return _FALSE;
        }

        return _TRUE;
    }
    else
    {
        if((GET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(_TX0) == _LINK_TRAING_PASS) && (GET_HDMI_MAC_TX0_HPD_STATUS_CHANGE() == FALSE))
           ScalerHdmi21MacTx0_Get_SourceTestConfiguration();
        return _TRUE;
    }
}

//--------------------------------------------------
// Description  : Hdmi 2.1 port SET LT STATE initial
// Input Value  : port & LTS STATE
// Output Value : NONE
//--------------------------------------------------
void ScalerHdmi21MacTxSetLinkTrainingState(EnumOutputPort enumOutputPort, EnumLinkTraingState enumState)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, enumState);

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX1, enumState);

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX2, enumState);

            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }
}

//--------------------------------------------------
// Description  : Hdmi 2.1 port SET Re link traing flag
// Input Value  : port & LTS STATE
// Output Value : NONE
//--------------------------------------------------
void ScalerHdmi21MacTxSetReLinkTraining(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            SET_HDMI21_MAC_TX_RE_LINK_TRAINING(_TX0);

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            SET_HDMI21_MAC_TX_RE_LINK_TRAINING(_TX1);

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            SET_HDMI21_MAC_TX_RE_LINK_TRAINING(_TX2);

            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }
}

//--------------------------------------------------
// Description  : Hdmi 2p1 linktraing flow
// Input Value  : port
// Output Value : none
//--------------------------------------------------
void ScalerHdmi21MacTxLinktraining(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            ScalerHdmi21MacTx0Linktraining();

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            ScalerHdmi21MacTx1Linktraining();

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            ScalerHdmi21MacTx2Linktraining();

            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }
}

//--------------------------------------------------
// Description  : Hdmi 2p1 linktraing parameter reset
// Input Value  : port
// Output Value : none
//--------------------------------------------------
void ScalerHdmi21MacTxLinktrainingReset(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            ScalerHdmi21MacTx0LinktrainingReset();

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            ScalerHdmi21MacTx1LinktrainingReset();

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            ScalerHdmi21MacTx2LinktrainingReset();

            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }
}

//--------------------------------------------------
// Description  : Hdmi 2p1 linktraing PASS or Legacy check
// Input Value  : port
// Output Value : none
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTxLinktrainingPassCheck(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            return ScalerHdmi21MacTx0LinktrainingPassCheck();

            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            return ScalerHdmi21MacTx1LinktrainingPassCheck();

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            return ScalerHdmi21MacTx2LinktrainingPassCheck();

            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : Hdmi 2p1 linktraing flow
// Input Value  : port
// Output Value : none
//--------------------------------------------------
void ScalerHdmi21MacTxLinktrainingResetToLTS1(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:
            SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, _HDMI21_LT_LTS1);
            ScalerHdmi21MacTx0LinktrainingReset();
            break;
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            break;
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            break;
#endif

        case _TX_MAP_NONE:
        default:

            break;
    }
}
