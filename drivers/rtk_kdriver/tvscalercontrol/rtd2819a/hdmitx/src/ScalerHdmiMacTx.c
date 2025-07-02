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
// ID Code      : ScalerHdmiMacTx.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include <rbus/dsce_misc_reg.h>
#include <rbus/dsce_reg.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>

#include "../include/ScalerFunctionInclude.h"
#include "../include/HdmiMacTx/ScalerHdmiMacTx.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0.h"
#include "../include/HdmiMacTx/ScalerHdmiMacTxInclude.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0Include.h"
#include "../include/HdmiMacTx/ScalerHdmiMacTx.h"
#include "../include/ScalerTx.h"
#include "../include/hdmitx_phy_struct_def.h"


#if(_HDMI_TX_SUPPORT == _ON)

//****************************************************************************
// CODE TABLES
//****************************************************************************



//****************************************************************************
// Local/Static VARIABLE DECLARATIONS
//****************************************************************************
#if (_HDMI_HDCP14_TX0_SUPPORT == _ON)
UINT8 g_pucHdmiMacTxHdcpDpk[_HDMI_TX_HDCP14_KEY_SIZE];
UINT8 g_pucHdcp14TxAksvBackup[_HDMI_TX_HDCP14_KEY_SIZE];

UINT16 g_usHdmiMacTxHdcpKsvFifoIndex;
UINT8 g_pucHdmiMacTxHdcpKsvFifo[_HDMI_TX_HDCP_DEVICE_COUNT_MAX * 5];

BOOLEAN g_bHdmiMacTx2UpfSendHdcp14;
#endif // #if(_HDMI_HDCP14_TX0_SUPPORT == _ON)

static UINT8 g_ucHdmitxLaneSrcSel=0xe4; // 0xb1


static UINT8 g_ucHdmitxLanePnSwap=0x0; // 0xf

//static UINT8 value;
//static UINT32 value32;
hdmi_tx_fixed_htotal_retiming_adj_st fixed_htotal_adjust_480i_576i[4]={
 {1, 9,  0, 0, 2}, //720*480i
  {0, 9,  0, 0, 0}, //720*576i
    {0, 0,  0, 0, 0}, //1440*480i
   {0, 10,  0, 0, 0}, //1440*576i
};
//****************************************************************************
// Global VARIABLE DECLARATIONS
//****************************************************************************
extern UINT8 bHD20_needRetiming;
extern UINT8 needRetiming_index;

#define GET_HDMI_HD20_needRetiming()          (bHD20_needRetiming)
//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************

//--------------------------------------------------
// Description  : HDMI Tx Handler
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTxHandler(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiMacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:
            ScalerHdmiMacTx0Handler();
            break;
#endif
        default:
            break;
    }
}

#if 0 //
//--------------------------------------------------
// Description  : HDMI Tx0 Handler
// Input Value  : None
// Output Value : None
//--------------------------------------------------
UINT8 ScalerHdmiMacTxPxTxMapping(EnumOutputPort enumOutputPort)
{

#if 1 // only 1 TX port
    return _HW_P0_HDMI_MAC_TX_MAPPING;
#else
    UINT8 ucTxPortTemp = _TX_MAP_NONE;

    switch(enumOutputPort)
    {
#if(_P1_HDMI_SUPPORT == _ON)
        case _P1_OUTPUT_PORT:
            ucTxPortTemp = _HW_P1_HDMI_MAC_TX_MAPPING;
            break;
#endif

#if(_P2_HDMI_SUPPORT == _ON)
        case _P2_OUTPUT_PORT:
            ucTxPortTemp = _HW_P2_HDMI_MAC_TX_MAPPING;
            break;
#endif

#if(_P3_HDMI_SUPPORT == _ON)
        case _P3_OUTPUT_PORT:
            ucTxPortTemp = _HW_P3_HDMI_MAC_TX_MAPPING;
            break;
#endif

#if(_P4_HDMI_SUPPORT == _ON)
        case _P4_OUTPUT_PORT:
            ucTxPortTemp = _HW_P4_HDMI_MAC_TX_MAPPING;
            break;
#endif
        default:
            break;
    }
    return ucTxPortTemp;
#endif
}
#endif

#endif // #if(_HDMI_TX_SUPPORT == _ON)

//--------------------------------------------------
// Description  : ScalerHdmiMacGetDownStreamStatus
// Input Value  : None
// Output Value : _TRUE / _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTxGetDownStreamStatus(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiMacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:

            return GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS();

#endif

        default:

            return _FALSE;
    }
}

#ifdef NOT_USED_NOW
#if(_HDMI_PURE_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Set Clk lane(CH3) transmit by data format
// Input Value  : _ENABLE or _DISABLE
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTxSetClkDataTransmit(EnumOutputPort enumOutputPort, BOOLEAN bEnable)
{
    //bEnable = bEnable;
    switch(ScalerHdmiMacTxPxTxMapping(enumOutputPort))
    {
#if((_HDMI_MAC_TX0_SUPPORT == _ON) && (_HDMI_MAC_TX0_PURE == _ON))
        case _TX0:

            ScalerHdmiMacTx0SetClkDataTransmit(bEnable);

            break;
#endif
        default:
            break;
    }
}
#endif
#endif
#if(_HDMI_PURE_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Set Clk lane div4
// Input Value  : _ENABLE or _DISABLE
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTxSetClkDiv4(EnumOutputPort enumOutputPort, BOOLEAN bEnable)
{
    //bEnable = bEnable;

    switch(ScalerHdmiMacTxPxTxMapping(enumOutputPort))
    {
#if((_HDMI_MAC_TX0_SUPPORT == _ON) && (_HDMI_MAC_TX0_PURE == _ON))
        case _TX0:

            ScalerHdmiMacTx0SetClkDiv4(bEnable);

            break;
#endif
        default:
            break;
    }
}
#endif // End of #if(_HDMI_PURE_SUPPORT == _ON)

#if(_HDMI_TX_SUPPORT == _ON)
#if(_AUDIO_SUPPORT == _ON)

//--------------------------------------------------
// Description  : Get Audio Info From STx Audio
// Input Value  : EnumOutputPort, EnumAudioInfoType
// Output Value : Node
//--------------------------------------------------
UINT8 ScalerHdmiMacTxGetAudioFormat(EnumOutputPort enumOutputPort, EnumAudioInfoType enumAudioInfoType)
{
    switch(enumAudioInfoType)
    {
        case _AUDIO_CHANNEL_COUNT:

            return ScalerStreamAudioGetChannelCount(enumOutputPort);

            break;

        case _AUDIO_CODING_TYPE:

            return ScalerStreamAudioGetCodingType(enumOutputPort);

            break;

        case _AUDIO_SAMPLING_FREQUENCY_TYPE:

            return ScalerStreamAudioGetSamplingFrequencyType(enumOutputPort);

            break;

        default:
            break;
    }

    return 0x00;
}
#ifdef NOT_USED_NOW
//--------------------------------------------------
// Description  : ScalerHdmiMacTxDisableAudioFifo
// Input Value  : enumOutputPort
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTxDisableAudioFifo(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiMacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:

            CLR_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE();

            break;
#endif

        default:
            break;
    }
}
#endif 
#endif // #if(_AUDIO_SUPPORT == _ON)
#ifdef NOT_USED_NOW
#if((_AUDIO_SUPPORT == _ON) || (_HDMI21_TX_SUPPORT == _ON))
//--------------------------------------------------
// Description  : ScalerHdmiMacTxGetStreamSource
// Input Value  : enumOutputPort
// Output Value : EnumHDMITxInputSource
//--------------------------------------------------
EnumHDMITxInputSource ScalerHdmiMacTxGetStreamSource(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiMacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:

            return GET_HDMI_MAC_TX0_STREAM_SOURCE();
#endif

        default:
            break;
    }

    return _HDMI_TX_SOURCE_NONE;
}
#endif // #if((_AUDIO_SUPPORT == _ON) || (_HDMI21_TX_SUPPORT == _ON))


//--------------------------------------------------
// Description  : ScalerHdmiMacGetInputPixelClk
// Input Value  :
// Output Value :
//--------------------------------------------------
UINT16 ScalerHdmiMacTxGetInputPixelClk(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiMacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:
            return GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK();
#endif

        default:
            break;
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : ScalerHdmiMacGetInputColorDepth
// Input Value  :
// Output Value :
//--------------------------------------------------
UINT8 ScalerHdmiMacTxGetInputColorDepth(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiMacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:
            return GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH();
#endif

        default:
            break;
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : ScalerHdmiMacGetInputRepeatNum
// Input Value  :
// Output Value :
//--------------------------------------------------
UINT8 ScalerHdmiMacTxGetInputRepeatNum(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiMacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:
            return GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM();
#endif

        default:
            break;
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : ScalerHdmiMacGetInputPixelClk
// Input Value  :
// Output Value :
//--------------------------------------------------
UINT16 ScalerHdmiMacTxGetDeepColorClk(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiMacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _TX0:
            return GET_HDMI_MAC_TX0_DEEP_COLOR_CLK();
#endif

        default:
            break;
    }

    return _FALSE;
}


//--------------------------------------------------
// Description  : ScalerHdmiMacTxGetHdcpBksv
// Input Value  : EnumOutputPort enumOutputPort, UINT8 *pucArray
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTxGetHdcpBksv(EnumOutputPort enumOutputPort, UINT8 *pucArray)
{
    //pucArray = pucArray;

    switch(ScalerHdmiMacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_HDCP14_TX0_SUPPORT == _ON)
        case _TX0:

            ScalerHdmiHdcp14Tx0Auth1CheckBksv();

            memcpy(pucArray, g_pucHdmiHdcp14Tx0HdcpBksv, 5);

            break;
#endif

#if(_HDMI_HDCP14_TX1_SUPPORT == _ON)
        case _TX1:

            ScalerHdmiHdcp14Tx1Auth1CheckBksv();

            memcpy(pucArray, g_pucHdmiHdcp14Tx1HdcpBksv, 5);

            break;
#endif

#if(_HDMI_HDCP14_TX2_SUPPORT == _ON)
        case _TX2:

            ScalerHdmiHdcp14Tx2Auth1CheckBksv();

            memcpy(pucArray, g_pucHdmiHdcp14Tx2HdcpBksv, 5);

            break;
#endif
        default:
            break;
    }
}

#endif 
//--------------------------------------------------
// Description  :  HDMI Tx MAC config -- async FIFO control
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiTx0_Mac_asyncFifoEnable(UINT8 enable)
{
    UINT32 hdmitx_misc_hdmitx_misc_dbg_ctrl_reg_regValue;
    UINT8 value;

    // 1. HDMITX async FIFO control: 0-> mac/phy ready -> 1
    hdmitx_misc_hdmitx_misc_dbg_ctrl_reg_regValue = rtd_inl(HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg);
    value = HDMITX_MISC_HDMITX_MISC_DBG_CTRL_get_async_fifo_en(hdmitx_misc_hdmitx_misc_dbg_ctrl_reg_regValue);
    if(value != enable){
        FatalMessageHDMITx("[HDMITX] async fifo en=%d->%d\n", (UINT32)value, (UINT32)enable);

        //HDMITX_DTG_Wait_Den_Stop_Done();//    Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,1);
        rtd_maskl(HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg,
                        ~HDMITX_MISC_HDMITX_MISC_DBG_CTRL_async_fifo_en_mask, HDMITX_MISC_HDMITX_MISC_DBG_CTRL_async_fifo_en(enable));
    }
    return;
}


//--------------------------------------------------
// Description  :  HDMI Tx MAC config -- interrupt control
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiTx0_Mac_interruptEnable(UINT8 enable)
{
    //#define HDMITX_MISC_INT_MASK (HDMITX_MISC_HDMITX_MISC_INT_irq_mcu_mor_err_mask\
                                                                //|HDMITX_MISC_HDMITX_MISC_INT_irq_channel_mode_chg20_mask\
                                                                //|HDMITX_MISC_HDMITX_MISC_INT_irq_channel_mode_chg21_mask)

    //rtd_maskl(HDMITX_MISC_HDMITX_MISC_INT_reg, ~HDMITX_MISC_HDMITX_MISC_INT_irq_mcu_mor_err_en_mask, HDMITX_MISC_HDMITX_MISC_INT_irq_mcu_mor_err_en(enable));
    rtd_maskl(HDMITX_MISC_HDMITX_MISC_INT_reg, ~HDMITX_MISC_HDMITX_MISC_INT_irq_hdmi_tx_en_mask, HDMITX_MISC_HDMITX_MISC_INT_irq_hdmi_tx_en(enable));
    rtd_maskl(HDMITX_MISC_HDMITX_MISC_INT_reg, ~HDMITX_MISC_HDMITX_MISC_INT_hdmitx_int_en_mask, HDMITX_MISC_HDMITX_MISC_INT_hdmitx_int_en(enable));

    // reset interrupt status
    //rtd_maskl(HDMITX_MISC_HDMITX_MISC_INT_reg, ~(HDMITX_MISC_INT_MASK), HDMITX_MISC_INT_MASK);

    return;
}

//--------------------------------------------------
// Description  :  HDMI Tx MAC config -- pixel shift  control
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiTx0_Mac_pixelShiftConfig(UINT8 mode)
{
    #define HDMITX_SHIFT_P3_P2_P1_P0_MASK (HDMITX_MISC_HDMITX_4PTO1P_hdmitx_shift_p3_mask\
                                                                                     | HDMITX_MISC_HDMITX_4PTO1P_hdmitx_shift_p2_mask\
                                                                                     | HDMITX_MISC_HDMITX_4PTO1P_hdmitx_shift_p1_mask\
                                                                                     | HDMITX_MISC_HDMITX_4PTO1P_hdmitx_shift_p0_mask)
    rtd_maskl(HDMITX_MISC_HDMITX_4PTO1P_reg, ~HDMITX_SHIFT_P3_P2_P1_P0_MASK, 0);
    return;
}


//--------------------------------------------------
// Description  :  HDMI Tx MAC config -- pixel shift  control
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiTx0_Mac_retimingMode(UINT8 mode, UINT8 enable)
{
    #define HDMITX_RETIMING_MASK (HDMITX_MISC_HDMITX_RETIMING_MODE_hdmitx_retiming_mode_mask\
                                                                | HDMITX_MISC_HDMITX_RETIMING_MODE_htotal_fix_hs_st_adj_mask\
                                                                | HDMITX_MISC_HDMITX_RETIMING_MODE_htotal_fix_hs_end_adj_mask\
                                                                | HDMITX_MISC_HDMITX_RETIMING_MODE_htotal_fix_de_st_adj_mask\
                                                                | HDMITX_MISC_HDMITX_RETIMING_MODE_htotal_fix_de_end_adj_mask)
    rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_MODE_reg, ~HDMITX_RETIMING_MASK, HDMITX_MISC_HDMITX_RETIMING_MODE_hdmitx_retiming_mode(mode));

    if(mode == 1){
        rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_MODE_reg, ~HDMITX_MISC_HDMITX_RETIMING_MODE_htotal_fix_hs_st_adj_mask, HDMITX_MISC_HDMITX_RETIMING_MODE_htotal_fix_hs_st_adj(fixed_htotal_adjust_480i_576i[needRetiming_index].hs_st_adj));
        rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_MODE_reg, ~HDMITX_MISC_HDMITX_RETIMING_MODE_htotal_fix_hs_end_adj_mask, HDMITX_MISC_HDMITX_RETIMING_MODE_htotal_fix_hs_end_adj(fixed_htotal_adjust_480i_576i[needRetiming_index].hs_end_adj));
        rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_MODE_reg, ~HDMITX_MISC_HDMITX_RETIMING_MODE_htotal_fix_de_st_adj_mask, HDMITX_MISC_HDMITX_RETIMING_MODE_htotal_fix_de_st_adj(fixed_htotal_adjust_480i_576i[needRetiming_index].de_st_adj));
        rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_MODE_reg, ~HDMITX_MISC_HDMITX_RETIMING_MODE_htotal_fix_de_end_adj_mask, HDMITX_MISC_HDMITX_RETIMING_MODE_htotal_fix_de_end_adj(fixed_htotal_adjust_480i_576i[needRetiming_index].de_end_adj));
        rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_H0_reg, ~HDMITX_MISC_HDMITX_RETIMING_H0_htotal_fix_vs_center_adj_mask, HDMITX_MISC_HDMITX_RETIMING_H0_htotal_fix_vs_center_adj(fixed_htotal_adjust_480i_576i[needRetiming_index].fix_vs_center_adj));
    }else{
        rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_H0_reg, ~HDMITX_MISC_HDMITX_RETIMING_H0_htotal_fix_vs_center_adj_mask, HDMITX_MISC_HDMITX_RETIMING_H0_htotal_fix_vs_center_adj(0));
    }
    #define HDMITX_RETIMING_H0_MASK (HDMITX_MISC_HDMITX_RETIMING_H0_retiming_en_mask\
                                                                        | HDMITX_MISC_HDMITX_RETIMING_H0_vsync_inv_mask\
                                                                        | HDMITX_MISC_HDMITX_RETIMING_H0_hsync_inv_mask\
                                                                        | HDMITX_MISC_HDMITX_RETIMING_H0_retiming_hsync_mask\
                                                                        | HDMITX_MISC_HDMITX_RETIMING_H0_retiming_hback_mask)
    rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_H0_reg, ~HDMITX_RETIMING_H0_MASK, HDMITX_MISC_HDMITX_RETIMING_H0_retiming_en(enable));

#ifdef CHIP_PLATFORM_BRIDGE
    rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_H0_reg, ~HDMITX_MISC_HDMITX_RETIMING_H0_hsync_inv_mask, HDMITX_MISC_HDMITX_RETIMING_H0_hsync_inv(1));
#endif // #ifdef CHIP_PLATFORM_BRIDGE

    rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_H0_reg, ~HDMITX_MISC_HDMITX_RETIMING_H0_hdmi21_480i_vs_center_adj_mask, HDMITX_MISC_HDMITX_RETIMING_H0_hdmi21_480i_vs_center_adj(0));

    #define HDMITX_RETIMING_H1_MASK (HDMITX_MISC_HDMITX_RETIMING_H1_retiming_hactive_mask\
                                                                        | HDMITX_MISC_HDMITX_RETIMING_H1_dummy_15_12_mask\
                                                                        | HDMITX_MISC_HDMITX_RETIMING_H1_retiming_hfront_mask)
    rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_H1_reg, ~HDMITX_RETIMING_H1_MASK, 0);

    #define HDMITX_RETIMING_V0_MASK (HDMITX_MISC_HDMITX_RETIMING_V0_retiming_vsync_mask| HDMITX_MISC_HDMITX_RETIMING_V0_retiming_vback_mask)
    rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_V0_reg, ~HDMITX_RETIMING_V0_MASK, 0);

    #define HDMITX_RETIMING_V1_MASK (HDMITX_MISC_HDMITX_RETIMING_V1_retiming_vactive_mask| HDMITX_MISC_HDMITX_RETIMING_V1_retiming_vfront_mask)
    rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_V1_reg, ~HDMITX_RETIMING_V1_MASK, 0);

    return;
}

//--------------------------------------------------
// Description  :  HDMI Tx MAC config -- check RX TMDS status
// Input Value  :
// Output Value :
//--------------------------------------------------
UINT8 ScalerHdmiTx0_Mac_checkRxTmdsStatus(void)
{
    //B[3] HDMITX TMDS_RXONA from HDMITXPHY
    //B[2] HDMITX TMDS_RXONB from HDMITXPHY
    //B[1] HDMITX TMDS_RXONC from HDMITXPHY
    //B[0] HDMITX TMDS_RXONCK from HDMITXPHY
    return (rtd_inl(HDMITX_MISC_HDMITX_TMDS_RXON_reg) & 0xf);
}



//--------------------------------------------------
// Description  :  HDMI Tx MAC config
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiTx0_MacConfig(UINT8 hdmiMode)
{
    hdmitx_misc_hdmitx_misc_dbg_ctrl_RBUS hdmitx_misc_hdmitx_misc_dbg_ctrl_reg;
    hdmitx_misc_hdmitx_4pto1p_RBUS hdmitx_misc_hdmitx_4pto1p_reg;
    dsce_misc_dsce_src_sel_RBUS dsce_misc_dsc_src_sel_reg;//spec
    //unsigned char inputClk4Pixel = Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_CLK_4PIXEL_MODE);

    DebugMessageHDMITx("[HDMITX] %s, hdmiMode=%d\n", __FUNCTION__, hdmiMode);

    // 1. HDMITX async FIFO control: 0-> mac/phy ready -> 1
    // 2. HDMITX MAC data source select
    hdmitx_misc_hdmitx_misc_dbg_ctrl_reg.regValue = rtd_inl(HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg);
    hdmitx_misc_hdmitx_misc_dbg_ctrl_reg.phy_data_src_sel = hdmiMode; // HDMITXPHY data source selector: 0 : From HDMITX 2.0 MAC, From HDMITX 2.1 MAC.
    // Lane power on control: always=1
    hdmitx_misc_hdmitx_misc_dbg_ctrl_reg.lane3_force = 1;
    hdmitx_misc_hdmitx_misc_dbg_ctrl_reg.lane2_force = 1;
    hdmitx_misc_hdmitx_misc_dbg_ctrl_reg.lane1_force = 1;
    hdmitx_misc_hdmitx_misc_dbg_ctrl_reg.lane0_force = 1;
    rtd_outl(HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg, hdmitx_misc_hdmitx_misc_dbg_ctrl_reg.regValue);

    // 3. HDMITX interrupt control enable & reset
    //ScalerHdmiTx0_H5xMac_interruptEnable(_ENABLE); // move to HDMITX init

    hdmitx_misc_hdmitx_4pto1p_reg.regValue = rtd_inl(HDMITX_MISC_HDMITX_4PTO1P_reg);
    dsce_misc_dsc_src_sel_reg.regValue = rtd_inl(DSCE_MISC_DSCE_SRC_SEL_reg);//spec

    // 4. HDMITX 4P to 1P config
    if(hdmiMode == TC_HDMI20)
    {
        hdmitx_misc_hdmitx_4pto1p_reg.hdmitx_4pto1p_en = 0; // 4 pixel to 1 pixel function enable
        // [Mark2] -- change to 2pto1p mode in HDMI2.0
        hdmitx_misc_hdmitx_4pto1p_reg.hdmitx_2pto1p_en = 1; // 2 pixel to 1 pixel function enable (Mark2 input from DispD)

        // [Mark2] enable bypass mode in HDMI20 mode
       // dsce_misc_dsc_src_sel_reg.insel_mode = 1; // [Mark2] bypass 2p to 4p in HDMI20 mode (1 => bypass mode (2p valid, ch0/ch1))//spec
    }else{
        hdmitx_misc_hdmitx_4pto1p_reg.hdmitx_4pto1p_en = 0; // 4 pixel to 1 pixel function enable
        // [Mark2] -- HDMI2.1 restore setting
        hdmitx_misc_hdmitx_4pto1p_reg.hdmitx_2pto1p_en = 0; // 2 pixel to 1 pixel function enable (Mark2 input from DispD)
        //dsce_misc_dsc_src_sel_reg.insel_mode = 0; // [Mark2] enable 2p to 4p in HDMI21 mode (0 => 2pto4p mode (4p valid, ch0/ch1/ch2/ch3))//spec
    }
    dsce_misc_dsc_src_sel_reg.hdmitx_dsc_source_sel = 0;//spec
    rtd_outl(HDMITX_MISC_HDMITX_4PTO1P_reg, hdmitx_misc_hdmitx_4pto1p_reg.regValue);
    rtd_outl(DSCE_MISC_DSCE_SRC_SEL_reg, dsce_misc_dsc_src_sel_reg.regValue);//spec

    // 5. HDMIRX TMDS status check
    DebugMessageInfo("[HDMITX] RX TMDS CH status=%d\n", (UINT32)ScalerHdmiTx0_Mac_checkRxTmdsStatus());

    // 6. HDMITX 2.0 4X width handler: retiming_mode. pixel_shift
    ScalerHdmiTx0_Mac_pixelShiftConfig(0);
    ScalerHdmiTx0_Mac_retimingMode(GET_HDMI_HD20_needRetiming(), _DISABLE);

    // 7. HDMITX EMP packet setting: N/A

    // 8. HDMITX Audio bypass setting: N/A

    return;
}
#if 0
{
    //UINT8 inputClk4Pixel = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_CLK_4PIXEL_MODE);

    DebugMessageHDMITx("[HDMITX] MacConfig(), hdmiMode=%d\n",  (UINT32)hdmiMode);

    // 1. HDMITX async FIFO control: 0-> mac/phy ready -> 1
    // 2. HDMITX MAC data source select
    // Lane power on control: always=1
#ifdef FIX_ME_HDMITX_BRING_UP
    //0x0 : HDMITX 2.0. From HDMITX 2.0 MAC.
    //0x1 : HDMITX 2.1. From HDMITX 2.1 MAC.
    rtd_maskl( HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg, ~HDMITX_MISC_HDMITX_MISC_DBG_CTRL_phy_data_src_sel_mask, HDMITX_MISC_HDMITX_MISC_DBG_CTRL_phy_data_src_sel(hdmiMode));//phy_data_src_sel[28]=0  (from 0: HDMI2.0, 1: HDMI2.1 MAC) @@@@@
//fix by zhaodong
#if 0
    rtd_maskl( HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg, ~HDMITX_MISC_HDMITX_MISC_DBG_CTRL_lane3_force_mask, HDMITX_MISC_HDMITX_MISC_DBG_CTRL_lane3_force(1));//Enable lan3[27] =1
    rtd_maskl( HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg, ~HDMITX_MISC_HDMITX_MISC_DBG_CTRL_lane2_force_mask, HDMITX_MISC_HDMITX_MISC_DBG_CTRL_lane2_force(1));//Enable lan2[26] =1
    rtd_maskl( HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg, ~HDMITX_MISC_HDMITX_MISC_DBG_CTRL_lane1_force_mask, HDMITX_MISC_HDMITX_MISC_DBG_CTRL_lane1_force(1));//Enable lan1[25] =1
    rtd_maskl( HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg, ~HDMITX_MISC_HDMITX_MISC_DBG_CTRL_lane0_force_mask, HDMITX_MISC_HDMITX_MISC_DBG_CTRL_lane0_force(1));//Enable lan0[24] =1
#endif
#endif // #ifdef FIX_ME_HDMITX_BRING_UP

    // 3. HDMITX interrupt control enable & reset
    //ScalerHdmiTx0_Mac_interruptEnable(_ENABLE); // move to HDMITX init

    // 4. HDMITX 4P to 1P config
    if(hdmiMode == TC_HDMI20)
    {
        // 4 pixel to 1 pixel function enable
        value = 0;
        rtd_maskl(HDMITX_MISC_HDMITX_4PTO1P_reg, ~HDMITX_MISC_HDMITX_4PTO1P_hdmitx_4pto1p_en_mask, HDMITX_MISC_HDMITX_4PTO1P_hdmitx_4pto1p_en(value));

        //change to 2pto1p mode in HDMI2.0: 2 pixel to 1 pixel function enable (input from DispD)
        value = 1;
        rtd_maskl(HDMITX_MISC_HDMITX_4PTO1P_reg, ~HDMITX_MISC_HDMITX_4PTO1P_hdmitx_2pto1p_en_mask, HDMITX_MISC_HDMITX_4PTO1P_hdmitx_2pto1p_en(value));

        //enable bypass mode in HDMI20 mode
        //dsce_misc_dsc_src_sel_reg.insel_mode = 1; // bypass 2p to 4p in HDMI20 mode (1 => bypass mode (2p valid, ch0/ch1))
    }else{
        // 4 pixel to 1 pixel function enable
        rtd_maskl(HDMITX_MISC_HDMITX_4PTO1P_reg, ~HDMITX_MISC_HDMITX_4PTO1P_hdmitx_4pto1p_en_mask, HDMITX_MISC_HDMITX_4PTO1P_hdmitx_4pto1p_en(0));
        // -- HDMI2.1 restore setting
        // 2 pixel to 1 pixel function enable (input from DispD)
        rtd_maskl(HDMITX_MISC_HDMITX_4PTO1P_reg, ~HDMITX_MISC_HDMITX_4PTO1P_hdmitx_2pto1p_en_mask, HDMITX_MISC_HDMITX_4PTO1P_hdmitx_2pto1p_en(0));

        //dsce_misc_dsc_src_sel_reg.insel_mode = 0; //enable 2p to 4p in HDMI21 mode (0 => 2pto4p mode (4p valid, ch0/ch1/ch2/ch3))
    }

    // 5. HDMIRX TMDS status check
    DebugMessageHDMITx("[HDMITX] RX TMDS CH status=%d\n", (UINT32)ScalerHdmiTx0_Mac_checkRxTmdsStatus());

    // 6. HDMITX 2.0 4X width handler: retiming_mode. pixel_shift
    ScalerHdmiTx0_Mac_pixelShiftConfig(0);
    ScalerHdmiTx0_Mac_retimingMode(0, _DISABLE);

    // 7. HDMITX EMP packet setting: N/A

    // 8. HDMITX Audio bypass setting: N/A

    return;
}
#endif

//  LANE PN Swap
void ScalerHdmiTx0_LanePNSwap(void)
{

    // HDMI20 PN swap
    rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL0_reg, ~HDMITX20_ON_REGION_TXFIFO_CTRL0_tx_pn_swap_mask, HDMITX20_ON_REGION_TXFIFO_CTRL0_tx_pn_swap(g_ucHdmitxLanePnSwap));


    // HDMI21 PN swap
#ifdef CHIP_PLATFORM_BRIDGE
    //HDMI 2.1 TX0_On_Region
    rtd_part_outl( HDMITX21_ON_REGION_HDMI21_DPHY_0_reg,7,7,0x1); //l3_pn_swap[7] Enable Lane3 PN swap
    rtd_part_outl( HDMITX21_ON_REGION_HDMI21_DPHY_0_reg,6,6,0x1); //l2_pn_swap[6] Enable Lane2 PN swap
    rtd_part_outl( HDMITX21_ON_REGION_HDMI21_DPHY_0_reg,5,5,0x1); //l1_pn_swap[5] Enable Lane1 PN swap
    rtd_part_outl( HDMITX21_ON_REGION_HDMI21_DPHY_0_reg,4,4,0x1); //l0_pn_swap[5] Enable Lane0 PN swap

    rtd_part_outl( HDMITX21_ON_REGION_HDMI21_DPHY_1_reg,7,6,0x3); //l3_mux_sel[7:6] 11:from lane3
    rtd_part_outl( HDMITX21_ON_REGION_HDMI21_DPHY_1_reg,5,4,0x2); //l2_mux_sel[5:4] 10:from lane2
    rtd_part_outl( HDMITX21_ON_REGION_HDMI21_DPHY_1_reg,3,2,0x1); //l1_mux_sel[3:2] 01:from lane1
    rtd_part_outl( HDMITX21_ON_REGION_HDMI21_DPHY_1_reg,1,0,0x0); //l0_mux_sel[1:0] 00:from lane0
#else
    rtd_maskl(HDMITX21_ON_REGION_HDMI21_DPHY_0_reg, ~HDMITX21_ON_REGION_HDMI21_DPHY_0_l3_pn_swap_mask, HDMITX21_ON_REGION_HDMI21_DPHY_0_l3_pn_swap((g_ucHdmitxLanePnSwap >> 3)&0x1));
    rtd_maskl(HDMITX21_ON_REGION_HDMI21_DPHY_0_reg, ~HDMITX21_ON_REGION_HDMI21_DPHY_0_l2_pn_swap_mask, HDMITX21_ON_REGION_HDMI21_DPHY_0_l2_pn_swap((g_ucHdmitxLanePnSwap >> 2)&0x1));
    rtd_maskl(HDMITX21_ON_REGION_HDMI21_DPHY_0_reg, ~HDMITX21_ON_REGION_HDMI21_DPHY_0_l1_pn_swap_mask, HDMITX21_ON_REGION_HDMI21_DPHY_0_l1_pn_swap((g_ucHdmitxLanePnSwap >> 1)&0x1));
    rtd_maskl(HDMITX21_ON_REGION_HDMI21_DPHY_0_reg, ~HDMITX21_ON_REGION_HDMI21_DPHY_0_l0_pn_swap_mask, HDMITX21_ON_REGION_HDMI21_DPHY_0_l0_pn_swap((g_ucHdmitxLanePnSwap >> 0)&0x1));
#endif // #ifdef CHIP_PLATFORM_BRIDGE
    return;
}

//  LANE src sel
void ScalerHdmiTx0_LaneSrcSel(void)
{
    rtd_maskl(HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg, ~HDMITX_MISC_HDMITX_MISC_DBG_CTRL_laneck_src_sel_mask, HDMITX_MISC_HDMITX_MISC_DBG_CTRL_laneck_src_sel((g_ucHdmitxLaneSrcSel >> 6) & 0x3));
    rtd_maskl(HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg, ~HDMITX_MISC_HDMITX_MISC_DBG_CTRL_lanec_src_sel_mask, HDMITX_MISC_HDMITX_MISC_DBG_CTRL_lanec_src_sel((g_ucHdmitxLaneSrcSel >> 4) & 0x3));
    rtd_maskl(HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg, ~HDMITX_MISC_HDMITX_MISC_DBG_CTRL_laneb_src_sel_mask, HDMITX_MISC_HDMITX_MISC_DBG_CTRL_laneb_src_sel((g_ucHdmitxLaneSrcSel >> 2) & 0x3));
    rtd_maskl(HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg, ~HDMITX_MISC_HDMITX_MISC_DBG_CTRL_lanea_src_sel_mask, HDMITX_MISC_HDMITX_MISC_DBG_CTRL_lanea_src_sel((g_ucHdmitxLaneSrcSel >> 0) & 0x3));
    return;
}
#ifdef NOT_USED_NOW

//  LANE PN swap
void ScalerHdmiTx0_LanePnSwap_config(UINT8 value)
{
    if(g_ucHdmitxLanePnSwap != value){
        NoteMessageHDMITx("[HDMITX] LanePnSwap =%x\n", (UINT32)value);
        g_ucHdmitxLanePnSwap = value;
    }

    // sync lane PN swap setting
    ScalerHdmiTx0_LanePNSwap();

    return;
}
//  LANE src sel
void ScalerHdmiTx0_LaneSrcSel_config(UINT8 value)
{

    if(g_ucHdmitxLaneSrcSel != value){
        value32 = (((value >> 6) & 0x3)<< 12)|(((value >> 4) & 0x3)<<8)| (((value >> 2) & 0x3)<< 4)| ((value & 0x3));
        NoteMessageHDMITx("[HDMITX] LaneSrcSel=%x\n", (UINT32)value32);
        g_ucHdmitxLaneSrcSel = value;
    }

    // sync lane src sel setting
    ScalerHdmiTx0_LaneSrcSel();

    return;
}
#endif
void ScalerHdmiTx0_MacConfig_LaneSet(UINT8 hdmiMode)
{
	//  HDMITX MAC 2.0 setting
      ScalerHdmiTx0_MacConfig(hdmiMode);

	//  LANE PN Swap
	ScalerHdmiTx0_LanePNSwap();

	//  LANE src sel
	ScalerHdmiTx0_LaneSrcSel();
}
#endif // End of #if(_HDMI_TX_SUPPORT == _ON)
