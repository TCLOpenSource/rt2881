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
// ID Code      : ScalerTx.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include <rbus/ppoverlay_reg.h>
#include <rbus/hdmitx_phy_reg.h>
#include <rbus/pinmux_reg.h>
#include <rbus/ppoverlay_txpretg_reg.h>

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_ctrl.h>

#include "../include/ScalerFunctionInclude.h"

#include "../include/HdmiMacTx0/ScalerHdmiMacTx0.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0Include.h"
//#include "../include/HdmiMacTx/ScalerHdmiMacTxInterface.h"
#include "../include/Hdmi21MacTx/ScalerHdmi21MacTxInclude.h"
#include "../include/ScalerTx.h"
#include "../include/HdmiMacTx/ScalerHdmiMacTx.h"
#include "../include/hdmitx_phy_struct_def.h"
//#include "../include/HdmiTx_verifier.h"
//#include "../include/hdmitx_dsc_pps_struct_def.h"

//#include <scaler/scalerstruct.h>

//#include <rtk_kdriver/RPCDriver.h>

//#include <rbus/dsce_misc_reg.h>
//#include <rbus/ppoverlay_reg.h>
//#include <rbus/ppoverlay_outtg_reg.h>

//#include <mach/platform.h>

//#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
//#include <scaler/scalerDrvCommon.h>

//#endif
#ifdef CONFIG_ENABLE_DPRX
#include <rtk_kdriver/dprx/drvif_dprx.h>
#endif
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
//#include <rtk_kdriver/tvscalercontrol/hdmirx/drvif_hdmirx.h>
#endif


#if((_DP_TX_SUPPORT == _ON) || (_HDMI_TX_SUPPORT == _ON) || (_VGA_TX_SUPPORT == _ON))
//****************************************************************************
// CODE TABLES
//****************************************************************************
StructTimingInfo g_stHdmiTx0InputTimingInfo = {0};

//#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
StructHdmiRepeaterSourceInfo hdmiRepeaterSrcInfo;
UINT32  hdmiRepeaterBypassPktHdrEnMask=0;

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
UINT32  hdmiRepeaterSwPktEnMask=0;
HDMI_EM_VTEM_T *p_emp_vtem_t=NULL;
#endif

#if 1//#ifndef _MARK2_FIXME_H5X_ROS_TIMER_API // [MARK2] FIX-ME -- replace ROS API into Linux API
#include <linux/timer.h>
#include <linux/version.h>
//static struct timer_list _SCALER_TIMER_EVENT[_SCALER_TIMER_EVENT_NUM];
#define TIMER_TICKS 4 // HZ=250, 1000/250 =4ms
struct timer_data {
    struct timer_list timer;
    EnumScalerTimerEventHdmiTx0Type event_data;
};

static struct timer_data _SCALER_TIMER_EVENT[_SCALER_TIMER_EVENT_NUM];

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
#define setup_timer timer_setup
#endif

#else
    ROS_TIMER _SCALER_TIMER_EVENT[_SCALER_TIMER_EVENT_NUM];
#endif
UINT16	TxsocLinecount[20] = {0};
UINT16	VodmaLinecount[20] = {0};

UINT16	totalcount = 0;
//****************************************************************************
// Local/Static VARIABLE DECLARATIONS
//****************************************************************************
static UINT8 bLastMuteEn=0, bLastClravmute=0;
/*static*/ EnumHdmi21FrlType HDMITX_OUT_FRL_RATE=0;


/*static*/ UINT8 ucTxMuteEnFlag=0;
UINT8 ucTxBypassLinkTrainingEn=0;


#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
static UINT8 bHdmitxUpdateVoInfoFlag=0;
#endif
UINT8 ucTxHDRFlag =0;//for HDR package

StructHdmitxDisplayInfo tx_disp_info[1];

UINT8 bDisableHdcp22Rc=0;

UINT8 bForceLinkTrainingFlag=0;
//static UINT8 gDscEn=0;
UINT8 ucVTEMpacketEn=0;
UINT8 ucVRRFlagEn=0;

//static UINT32 lastDumpStc_ActiveTimerEvent = 0;
//static EnumScalerTimerEventHdmiTx0Type pre_event =_SCALER_TIMER_EVENT_NUM;
//static UINT8 last_event=0;

extern EnumHDMITxGcpToHwState gcpswtohw;
//****************************************************************************
// Global/Extern VARIABLE DECLARATIONS
//****************************************************************************
INT32 stc;
extern UINT8 bCheckEdidForHdmitxSupport;
extern UINT8 HDMI_TIMING_FAST_SWITCH_EN;
extern UINT32 HDMI_TIMING_PIXEL_FMT_CHANGE;

//extern UINT32 HDMITX_Get_HdmiTxRLinkStatus(void);
extern void ScalerHdmiMacTx0SetSwGcpPktMuteEn(void);
extern void ScalerHdmiMacTxHandler(EnumOutputPort enumOutputPort);
//****************************************************************************
// Macro/Definition
//****************************************************************************
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
#define memcpy tx_memcpy
#define memset tx_memset
#endif

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
//EnumHdmi21FrlType ScalerHdmiTxGetTargetFrlRate(void);



#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
void tx_memset(UINT8 *buf, UINT8 value, UINT16 len)
{
    UINT16 i = 0;

    if(buf == NULL){
        //FatalMessageHDMITx("[HDMITX] NULL BUF@tx_memset\n");
        return;
    }

    for (i = 0; i < len; i ++) {
        buf[i] = value;
    }
}


void tx_memcpy(UINT8 *dst, UINT8 *src, UINT16 len)
{
    UINT16 i = 0;

    if((dst == NULL)|| (src == NULL)){
        //FatalMessageHDMITx("[HDMITX] NULL DST|SRC@tx_memcpy\n");
        return;
    }

    for (i = 0; i < len; i ++) {
        dst[i] = src[i];
    }
}
#endif
#ifdef NOT_USED_NOW
void Scaler_HDMITX_HDCP_set_DisableHdcp22Rc(UINT8 disable)
{
    bDisableHdcp22Rc = disable;
    return;
}


UINT8 Scaler_HDMITX_HDCP_get_DisableHdcp22Rc(void)
{
    return bDisableHdcp22Rc;
}
#endif

UINT16 Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_INFO infoList)
{
    StructHdmitxDisplayInfo *p_tx_disp_info;
    //UINT8 channel=0;
    p_tx_disp_info = &tx_disp_info[0];

    switch (infoList)
    {
#ifdef NOT_USED_NOW
        case SCALER_INFO_INPUT_SRC:
        	return p_tx_disp_info->input_src;
        case SCALER_INFO_VSC_ISCONNECT:
        	return p_tx_disp_info->VSC_isConnect;
        case SCALER_INFO_IPH_ACT_STA:
        	return p_tx_disp_info->IPH_ACT_STA;
        case SCALER_INFO_IPV_ACT_STA:
        	return p_tx_disp_info->IPV_ACT_STA;
        case SCALER_INFO_IPH_ACT_WID:
        	return p_tx_disp_info->IPH_ACT_WID;
        case SCALER_INFO_IPV_ACT_LEN:
        	return p_tx_disp_info->IPV_ACT_LEN;

        case SCALER_INFO_IHTOTAL:
        	return p_tx_disp_info->IHTotal;
        case SCALER_INFO_IVTOTAL:
        	return p_tx_disp_info->IVTotal;
        case SCALER_INFO_PIXEL_CLOCK:
        	return p_tx_disp_info->PixelClock;


        case SCALER_INFO_MAIN_UZD_WID:
        	return p_tx_disp_info->Main_Uzd_Wid;
        case SCALER_INFO_MAIN_UZD_LEN:
        	return p_tx_disp_info->Main_Uzd_Len;
        case SCALER_INFO_DC2H_STATE:
        	return p_tx_disp_info->DC2H_state;
        case SCALER_INFO_DC2H_ISCONNECT:
        	return p_tx_disp_info->DC2H_isConnect;
        case SCALER_INFO_DC2H_SHOWTYPE:
        	return p_tx_disp_info->DC2H_showType;
        case SCALER_INFO_DC2H_CAPWID:
        	return p_tx_disp_info->DC2H_CapWid;
        case SCALER_INFO_DC2H_CAPLEN:
        	return p_tx_disp_info->DC2H_CapLen;
        case SCALER_INFO_DC2H_VFREQ:
        	return p_tx_disp_info->DC2H_vFreq;
        case SCALER_INFO_DC2H_DATAFORMAT:
        	return p_tx_disp_info->DC2H_dataFormat;
        case SCALER_INFO_SLR_DISP_10BIT:
        	return p_tx_disp_info->SLR_DISP_10BIT;

        case SCALER_INFO_HDCP:
        	return p_tx_disp_info->HDCP;
        case SCALER_INFO_VRR:
        	return p_tx_disp_info->VRR;
        case SCALER_INFO_HDR:
        	return p_tx_disp_info->HDR;
        case SCALER_INFO_PASSTHROUGH:
        	return p_tx_disp_info->passThrough;
        case SCALER_INFO_TESTMODE_ISCONNECT:
        	return p_tx_disp_info->TestMode_isConnect;
        case SCALER_INFO_TX_OUTPUT_ISCONNECT:
        	return p_tx_disp_info->TX_OUTPUT_isConnect;
        case SCALER_INFO_TX_OUTPUT_MODE:
        	return p_tx_disp_info->TX_OUTPUT_MODE;
        case SCALER_INFO_TX_OUTPUT_TIMING_MODE:
        	return p_tx_disp_info->TX_OUTPUT_Timing_Mode;
   #endif
        case SCALER_INFO_TX_OUTPUT_H_ACT_STA:
        	return p_tx_disp_info->TX_OUTPUT_H_ACT_STA;
        case SCALER_INFO_TX_OUTPUT_V_ACT_STA:
        	return p_tx_disp_info->TX_OUTPUT_V_ACT_STA;
        case SCALER_INFO_TX_OUTPUT_H_ACT_WID:
        	return p_tx_disp_info->TX_OUTPUT_H_ACT_WID;
        case SCALER_INFO_TX_OUTPUT_V_ACT_LEN:
        	return p_tx_disp_info->TX_OUTPUT_V_ACT_LEN;
        case SCALER_INFO_TX_OUTPUT_H_TOTAL:
        	return p_tx_disp_info->TX_OUTPUT_H_Total;
        case SCALER_INFO_TX_OUTPUT_V_TOTAL:
        	return p_tx_disp_info->TX_OUTPUT_V_Total;
        case SCALER_INFO_TX_OUTPUT_VFREQ:
        	return p_tx_disp_info->TX_OUTPUT_VFreq;
        case SCALER_INFO_TX_OUTPUT_PIXEL_CLOCK:
        	return p_tx_disp_info->TX_OUTPUT_PixelClock;
        case SCALER_INFO_TX_OUTPUT_HSYNC:
        	return p_tx_disp_info->TX_OUTPUT_HSYNC;
        case SCALER_INFO_TX_OUTPUT_VSYNC:
        	return p_tx_disp_info->TX_OUTPUT_VSYNC;
        case SCALER_INFO_SLR_DISP_422CAP:
        	return p_tx_disp_info->SLR_DISP_422CAP;
        case SCALER_INFO_TX_CLK_4PIXEL_MODE:
        	return p_tx_disp_info->TX_CLK_4PIXEL_MODE;
        case SCALER_INFO_COLOR_SPACE:
        	return p_tx_disp_info->color_space;
        case SCALER_INFO_COLOR_DEPTH:
        	return p_tx_disp_info->color_depth;
        case SCALER_INFO_COLOR_IMETRY:
        	return p_tx_disp_info->color_imetry;
        case SCALER_INFO_COLOR_EXT_IMETRY:
        	return p_tx_disp_info->color_ext_imetry;
        case SCALER_INFO_COLOR_RANGE:
        	return p_tx_disp_info->color_range;
        case SCALER_INFO_INTERLACE:
        	return p_tx_disp_info->Interlace;
        case SCALER_INFO_IHFREQ:
        	return p_tx_disp_info->IHFreq;
        case SCALER_INFO_IVFREQ:
        	return p_tx_disp_info->IVFreq;
	case SLR_INPUT_HDMITX_DSC_SRC:
             return p_tx_disp_info->hdmitxDscSrc;
            default:
                DebugMessageHDMITx("[disp_info] Don't know how to get disp_info!!!\n");
                return 0;
    }

    //return 0;
}


void Scaler_HDMITX_DispSetInputInfo(SLR_INPUT_INFO infoList, UINT16 value)
{
    StructHdmitxDisplayInfo *p_tx_disp_info;
    //UINT8 channel=0;
    p_tx_disp_info = &tx_disp_info[0];

    switch (infoList)
    {
#ifdef NOT_USED_NOW
        case SCALER_INFO_INPUT_SRC:
            p_tx_disp_info->input_src = value;
            break;
        case SCALER_INFO_VSC_ISCONNECT:
            p_tx_disp_info->VSC_isConnect = value;
            break;
        case SCALER_INFO_IPH_ACT_STA:
            p_tx_disp_info->IPH_ACT_STA = value;
            break;
        case SCALER_INFO_IPV_ACT_STA:
            p_tx_disp_info->IPV_ACT_STA = value;
            break;
        case SCALER_INFO_IPH_ACT_WID:
            p_tx_disp_info->IPH_ACT_WID = value;
            break;
        case SCALER_INFO_IPV_ACT_LEN:
            p_tx_disp_info->IPV_ACT_LEN = value;
            break;

        case SCALER_INFO_MAIN_UZD_WID:
            p_tx_disp_info->Main_Uzd_Wid = value;
            break;
        case SCALER_INFO_MAIN_UZD_LEN:
            p_tx_disp_info->Main_Uzd_Len = value;
            break;
        case SCALER_INFO_DC2H_STATE:
            p_tx_disp_info->DC2H_state = value;
            break;
        case SCALER_INFO_DC2H_ISCONNECT:
            p_tx_disp_info->DC2H_isConnect = value;
            break;
        case SCALER_INFO_DC2H_SHOWTYPE:
            p_tx_disp_info->DC2H_showType = value;
            break;
        case SCALER_INFO_DC2H_CAPWID:
            p_tx_disp_info->DC2H_CapWid = value;
            break;
        case SCALER_INFO_DC2H_CAPLEN:
            p_tx_disp_info->DC2H_CapLen = value;
            break;
        case SCALER_INFO_DC2H_VFREQ:
            break;
            p_tx_disp_info->DC2H_vFreq = value;
            break;
        case SCALER_INFO_DC2H_DATAFORMAT:
            p_tx_disp_info->DC2H_dataFormat = value;
            break;
        case SCALER_INFO_SLR_DISP_10BIT:
            p_tx_disp_info->SLR_DISP_10BIT = value;
            break;

        case SCALER_INFO_IHTOTAL:
            p_tx_disp_info->IHTotal = value;
            break;
        case SCALER_INFO_IVTOTAL:
            p_tx_disp_info->IVTotal = value;
            break;
        case SCALER_INFO_PIXEL_CLOCK:
            p_tx_disp_info->PixelClock = value;
            break;

        case SCALER_INFO_HDCP:
            p_tx_disp_info->HDCP = value;
            break;
        case SCALER_INFO_VRR:
            p_tx_disp_info->VRR = value;
            break;
        case SCALER_INFO_HDR:
            p_tx_disp_info->HDR = value;
            break;
        case SCALER_INFO_PASSTHROUGH:
            p_tx_disp_info->passThrough = value;
            break;
        case SCALER_INFO_TESTMODE_ISCONNECT:
            p_tx_disp_info->TestMode_isConnect = value;
            break;
        case SCALER_INFO_TX_OUTPUT_ISCONNECT:
            p_tx_disp_info->TX_OUTPUT_isConnect = value;
            break;
        case SCALER_INFO_TX_OUTPUT_MODE:
            p_tx_disp_info->TX_OUTPUT_MODE = value;
            break;
        case SCALER_INFO_TX_OUTPUT_TIMING_MODE:
            p_tx_disp_info->TX_OUTPUT_Timing_Mode = value;
            break;
#endif
        case SCALER_INFO_TX_OUTPUT_H_ACT_STA:
            p_tx_disp_info->TX_OUTPUT_H_ACT_STA = value;
            break;
        case SCALER_INFO_TX_OUTPUT_V_ACT_STA:
            p_tx_disp_info->TX_OUTPUT_V_ACT_STA = value;
            break;
        case SCALER_INFO_TX_OUTPUT_H_ACT_WID:
            p_tx_disp_info->TX_OUTPUT_H_ACT_WID = value;
            break;
        case SCALER_INFO_TX_OUTPUT_V_ACT_LEN:
            p_tx_disp_info->TX_OUTPUT_V_ACT_LEN = value;
            break;
        case SCALER_INFO_TX_OUTPUT_H_TOTAL:
            p_tx_disp_info->TX_OUTPUT_H_Total = value;
            break;
        case SCALER_INFO_TX_OUTPUT_V_TOTAL:
            p_tx_disp_info->TX_OUTPUT_V_Total = value;
            break;
        case SCALER_INFO_TX_OUTPUT_VFREQ:
            p_tx_disp_info->TX_OUTPUT_VFreq = value;
            break;
        case SCALER_INFO_TX_OUTPUT_PIXEL_CLOCK:
            p_tx_disp_info->TX_OUTPUT_PixelClock = value;
            break;

        case SCALER_INFO_SLR_DISP_422CAP:
            p_tx_disp_info->SLR_DISP_422CAP = value;
            break;
        case SCALER_INFO_TX_OUTPUT_HSYNC:
            p_tx_disp_info->TX_OUTPUT_HSYNC = value;
            break;
        case SCALER_INFO_TX_OUTPUT_VSYNC:
            p_tx_disp_info->TX_OUTPUT_VSYNC = value;
            break;

        case SCALER_INFO_TX_CLK_4PIXEL_MODE:
            p_tx_disp_info->TX_CLK_4PIXEL_MODE = value;
            break;
        case SCALER_INFO_IHFREQ:
            p_tx_disp_info->IHFreq = value;
            break;
        case SCALER_INFO_IVFREQ:
            p_tx_disp_info->IVFreq = value;
            break;
         case SCALER_INFO_COLOR_SPACE:
            p_tx_disp_info->color_space = value;
            break;
        case SCALER_INFO_COLOR_DEPTH:
            p_tx_disp_info->color_depth = value;
            break;
        case SCALER_INFO_COLOR_IMETRY:
            p_tx_disp_info->color_imetry = value;
            break;
         case SCALER_INFO_COLOR_EXT_IMETRY:
            p_tx_disp_info->color_ext_imetry = value;
            break;
        case SCALER_INFO_COLOR_RANGE:
            p_tx_disp_info->color_range = value;
            break;
        case SCALER_INFO_INTERLACE:
            p_tx_disp_info->Interlace = value;
            break;
        default:
            DebugMessageHDMITx("[disp_info] Don't know how to set disp_info[%d]=%d!!!\n", (UINT32)infoList, (UINT32)value);
            break;
    }

    //DebugMessageHDMITx("[HDMITX] infoList[%d]=%d\n", infoList, value);

    return;
}


UINT8 ScalerHdmiTxGetStreamStateReady(void)
{
    // check TX MAC state ready
#if(_HDMI21_TX_SUPPORT == _ON)
    if(ScalerHdmiTxGetTargetFrlRate() != 0){ // HDMI2.1
        return ((GET_HDMI_MAC_TX0_MODE_STATE() == _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON) /*&& (rtd_getbits(HDMITX21_MAC_HDMI21_FRL_19_reg, _BIT7) == 0)*/);
    }else
#endif // #if(_HDMI21_TX_SUPPORT == _ON)
    { // HDMI2.0
        return ((GET_HDMI_MAC_TX0_MODE_STATE() == _HDMI_TX_MODE_STATUS_LINK_ON) && (rtd_getbits(HDMITX20_ON_REGION_TXFIFO_CTRL0_reg, _BIT7) == _BIT7));
    }
}


// Get input video info from scaler
UINT16 ScalerDpStreamGetElement(EnumOutputPort ucOutputPort, EnumMultiStreamElement enElement)
{
    UINT16 ret=0;
    UINT8 table_colorDepth[4]={8,10,12,16};

#if 0 // TEST
        DebugMessageHDMITx("[HDMITX] Get Idx[%d]\n", enElement);
#endif

    // Get input video info from scaler
    switch(enElement){
        case _COLOR_SPACE:
            ret = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_SPACE);
            break;
        case _COLOR_DEPTH:
            ret = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_DEPTH);
            if(ret > I3DDMA_COLOR_DEPTH_16B)
                ret = 0;
            else
                ret = table_colorDepth[ret];
            break;
        case _PIXEL_CLOCK:
            ret = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_PIXEL_CLOCK);
            break;
        case _COLOR_RGB_QUANTIZATION_RANGE:
            ret = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_RANGE);
            break;
        case _COLOR_YCC_QUANTIZATION_RANGE:
            ret = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_RANGE);
            break;
        case _EXT_COLORIMETRY:
            ret = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_EXT_IMETRY);
            break;
        case _FRAME_RATE:
            ret = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_VFREQ) / 10;
            break;
        case _COLORIMETRY:
            ret = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_COLOR_IMETRY);
            break;
        case _INTERLACE_INFO:
            ret = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_INTERLACE);
            //InfoMessageHDMITx("[HDMITX] Interlaced ret=%d\n", (UINT32)ret);
            break;
        case _OUTPUT_PIXEL_MODE:
            ret = (Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_CLK_4PIXEL_MODE)? 2: 0); // 0: 1pixel, 2: 4pixel
            break;
        case _DIGITAL_QUANTIZATION_PREVALUE:
        default:
            DebugMessageHDMITx("[HDMITX] NOT SUPPORT ELEMENT[%d]@ScalerDpStreamGetElement()\n", (UINT32)enElement);
            break;
    }

#if 0 // TEST
    DebugMessageHDMITx("[HDMITX] Get Idx[%d]=%d\n", enElement, ret);
#endif

    return ret;
}


void CLR_EDID_READ_FLG(void)
{
    g_stHdmiOutputStatus.b1DfpDeviceEditReadFlag = _FALSE;
}

void ScalerHdmiTxMuteEnable(UINT8 enable)
{
UINT32 hdmitx20_mac0_scheduler_1_reg;
    if(bLastMuteEn != enable)
        FatalMessageHDMITx("[HDMITX] HW MuteEn=%d, HDMI2.%d\n", (UINT32)enable, (UINT32)(ScalerHdmiTxGetTargetFrlRate() != 0? 1: 0));

#if(_HDMI21_TX_SUPPORT == _ON)
    if(ScalerHdmiTxGetTargetFrlRate() != 0){ // HDMI2.1
	rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg, ~HDMITX21_MAC_HDMI21_SCHEDULER_2_gcp_setavmute_mask, HDMITX21_MAC_HDMI21_SCHEDULER_2_get_gcp_setavmute(enable));
        if(1)
        {
            // [CTS] disable clear_avmute flag after clear_avmute is done
            if((bLastMuteEn != enable) && (enable == 0)){ // set clear_avmute=1
                // Double Buffer by vsync rising, Read value is effective after double buffer triggered
                rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg, ~HDMITX21_MAC_HDMI21_SCHEDULER_2_gcp_clravmute_mask, HDMITX21_MAC_HDMI21_SCHEDULER_2_gcp_clravmute(1));

                //  wait clear_avmute apply done
                //rtd_maskl(TXSOCTG_TXSOC_pending_status_reg, ~TXSOCTG_TXSOC_pending_status_mv_den_end_event_mask, TXSOCTG_TXSOC_pending_status_mv_den_end_event_mask);
                HDMITX_DTG_Wait_vsync_start(); // Scaler_Wait_for_event(TXSOCTG_TXSOC_pending_status_reg, TXSOCTG_TXSOC_pending_status_mv_den_end_event_mask,1);
                HDMITX_DTG_Wait_Den_Stop_Done(); // Scaler_Wait_for_event(TXSOCTG_TXSOC_pending_status_reg, TXSOCTG_TXSOC_pending_status_mv_den_end_event_mask,1);
                DebugMessageHDMITx("[HDMITX] Clear Mute Step1@%2x!\n", (UINT32)rtd_inl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg) & 0xff);
            }
            else{ // reset clear_avmute
                if(bLastClravmute != HDMITX21_MAC_HDMI21_SCHEDULER_2_get_gcp_clravmute(rtd_inl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg)))
                    InfoMessageHDMITx("[HDMITX] Clear Mute Step2@%2x!\n", (UINT32)rtd_inl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg) & 0xff);

                // save last gcp_clravmute register read status
                bLastClravmute = HDMITX21_MAC_HDMI21_SCHEDULER_2_get_gcp_clravmute(rtd_inl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg));

                // Double Buffer by vsync rising, Read value is effective after double buffer triggered
                rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg, ~HDMITX21_MAC_HDMI21_SCHEDULER_2_gcp_clravmute_mask, HDMITX21_MAC_HDMI21_SCHEDULER_2_gcp_clravmute(0));
            }
        }else{
            // Double Buffer by vsync rising, Read value is effective after double buffer triggered
            rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg, ~HDMITX21_MAC_HDMI21_SCHEDULER_2_gcp_clravmute_mask, HDMITX21_MAC_HDMI21_SCHEDULER_2_gcp_clravmute(!enable));
        }
    }else
#endif // #if(_HDMI21_TX_SUPPORT == _ON)
    { // HDMI2.0
        // GCP packet in HW mode
        // Double Buffer by vsync rising, Read value is effective after double buffer triggered
        hdmitx20_mac0_scheduler_1_reg = rtd_inl(HDMITX20_MAC0_SCHEDULER_1_reg);
        if(HDMITX20_MAC0_SCHEDULER_1_get_gcp_en(hdmitx20_mac0_scheduler_1_reg)){

            if (enable){
                hdmitx20_mac0_scheduler_1_reg = hdmitx20_mac0_scheduler_1_reg | HDMITX20_MAC0_SCHEDULER_1_gcp_setavmute_mask;
                hdmitx20_mac0_scheduler_1_reg = hdmitx20_mac0_scheduler_1_reg & (~HDMITX20_MAC0_SCHEDULER_1_gcp_clravmute_mask);
            }else
                {
                hdmitx20_mac0_scheduler_1_reg = hdmitx20_mac0_scheduler_1_reg & (~HDMITX20_MAC0_SCHEDULER_1_gcp_setavmute_mask);
                hdmitx20_mac0_scheduler_1_reg = hdmitx20_mac0_scheduler_1_reg | HDMITX20_MAC0_SCHEDULER_1_gcp_clravmute_mask;
            }
             rtd_outl(HDMITX20_MAC0_SCHEDULER_1_reg, hdmitx20_mac0_scheduler_1_reg);
             //rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~HDMITX20_MAC0_SCHEDULER_1_gcp_setavmute_mask, HDMITX20_MAC0_SCHEDULER_1_gcp_setavmute(enable));
            //rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~HDMITX20_MAC0_SCHEDULER_1_gcp_clravmute_mask, HDMITX20_MAC0_SCHEDULER_1_gcp_clravmute(!enable));

        }
        #if(_HDMI_TX_CTS_TEST == _ON) // HDMI2.0 mute when GCP in SW Packet mode
        else{ // GCP packet in SW mode
            if(SCRIPT_TX_SKIP_AVMUTE_BYPASS() && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 5000) && (GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_8_BITS))
            ScalerHdmiMacTx0SetSwGcpPktMuteEn();
            }
        #endif
//#ifdef FIX_ME_HDMITX_BRING_UP
            if(HDMITX20_MAC0_SCHEDULER_1_get_gcp_en(rtd_inl(HDMITX20_MAC0_SCHEDULER_1_reg)) == 0)
                DebugMessageHDMITx("[HDMITX] GCP Reg=%x\n", (UINT32)rtd_inl(HDMITX20_MAC0_SCHEDULER_1_reg));
//#endif // #ifdef FIX_ME_HDMITX_BRING_UP
    }

    bLastMuteEn = enable;

    return;
}


void ScalerHdmiTxSetMute(UINT8 enable)
{
    if(ucTxMuteEnFlag != enable){
        NoteMessageHDMITx("[HDMITX] Set Mute=%d\n", (UINT32)enable);
        ucTxMuteEnFlag = enable;
    }
    return;
}
#ifdef NOT_USED_NOW
static UINT8 ucTxDscMuteEn =0;
void ScalerHdmiTxSetDSCMuteEn(UINT8 enable)
{
    // [FIX-ME]
    if(ucTxDscMuteEn != enable){
        DebugMessageHDMITx("[HDMITX][NOT SUPPORT] Set DscMuteEn=%d\n", (UINT32)enable);
        ucTxDscMuteEn = enable;
    }

    return;
}

UINT8 ScalerHdmiTxGetDSCMuteEn(void)
{
    return ucTxDscMuteEn;
}
void ScalerHdmiTxCheckEdidForTxOut(UINT8 enable)
{
    if(bCheckEdidForHdmitxSupport != enable)
        NoteMessageHDMITx("[HDMITX] Check EDID For TX Out=%d\n", (UINT32)enable);
    bCheckEdidForHdmitxSupport = enable;
    return;
}
// wait for line count reset
UINT8 Scaler_Wait_for_LineCount(void)
{
    UINT16 timecount1, linecount, linecount1;
    UINT32 regAddr;

    timecount1 = 0x032500;
    linecount = 0;
    // outMode : 1: bypass, 0: PQ IP
    regAddr = (PPOVERLAY_DTG_d1_output_mux_get_d1_output_mux_sel(rtd_inl(PPOVERLAY_DTG_d1_output_mux_reg)) ? PPOVERLAY_OUTTG_OUTTG_new_meas0_linecnt_real_reg: PPOVERLAY_new_meas0_linecnt_real_reg);
    do{
        linecount1 = PPOVERLAY_new_meas0_linecnt_real_get_line_cnt_rt(rtd_inl(regAddr));
        if(linecount > linecount1)
        {
            return TRUE;
        }
        else
        {
            linecount = linecount1;
        }
        timecount1--;
    }while(timecount1);
    FatalMessageHDMITx("[HDMITX][WARN] Timeout!\n");
    return FALSE;
}
#endif
UINT8 ScalerHdmiTxGetHPDStatus(void)
{
    return ScalerHdmiMacTx0GetHpdStatus();
}
UINT8 ScalerHdmiTxGetEDIDValid(void)
{
    return GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS() && GET_OUTPUT_DFP_DEVICE_EDID_READ_FLG();
}



#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
void ScalerHdmiTxSetUpdateVoInfo(UINT8 enable)
{
    if(bHdmitxUpdateVoInfoFlag != enable){
        DebugMessageHDMITx("[HDMI_TX] bHdmitxUpdateVoInfoFlag=%d\n", (UINT32)enable);
        bHdmitxUpdateVoInfoFlag = enable;
    }
    return;
}


UINT8 ScalerHdmiTxGetUpdateVoInfoFlag(void)
{
    return bHdmitxUpdateVoInfoFlag;
}
#endif // #if(_HDMI_HDR10_TX0_SUPPORT == _ON)



UINT8 ScalerHdmiPhyTx0Initial(void)
{

    // disable async fifo before change PLL clock
    ScalerHdmiTx0_Mac_asyncFifoEnable(_DISABLE); // 0x0 : Disable async_fifo

    ScalerHdmiTxPhy_PowerOnDefaultSetting();

    // enable async fifo after PLL clock stable
    udelay(150);
    ScalerHdmiTx0_Mac_asyncFifoEnable(_ENABLE); // 0x1 : Enable async_fifo

    return 0;
}


UINT8 ScalerHdmiPhyTx0Set(void)
{

    ScalerTxHdmiTx0PhyConfig();
    return _TRUE;
}
static UINT8 last_Z0Detect=0;

EnumHDMITxZ0Detect ScalerHdmiPhyTx0Z0Detect(void)
{
    static UINT16 zeDetectLoopCnt=0;
#if 0//[IC] #ifdef CONFIG_ENABLE_ZEBU_BRING_UP // force return ZO value
    static UINT8 bFirstRunFlag=1;
    if(bFirstRunFlag){
        NoteMessageHDMITx("[HDMI_TX] 1st Z0 Detect!!\n");
        bFirstRunFlag = 0;
    }
    return _HDMI_TX_Z0_NOT_ALL_LOW;

#else // #ifdef CONFIG_ENABLE_ZEBU_BRING_UP // force return ZO value

    UINT32 hdmitx_misc_hdmitx_tmds_rxon_reg_regValue;
    hdmitx_misc_hdmitx_tmds_rxon_reg_regValue = rtd_inl(HDMITX_MISC_HDMITX_TMDS_RXON_reg);
    if(last_Z0Detect != hdmitx_misc_hdmitx_tmds_rxon_reg_regValue){
        // send unplug status message to host
        #ifdef NOT_USED_NOW
        if(hdmitx_misc_hdmitx_tmds_rxon_reg_regValue == 0)
            ScalerHdmiTxSendTxConnectStatus(_TRUE);
        #endif
        last_Z0Detect = hdmitx_misc_hdmitx_tmds_rxon_reg_regValue;
        zeDetectLoopCnt = 0;
    }
    if(++zeDetectLoopCnt % 100 == 0){
        DebugMessageHDMITx("[HDMI_TX][%d] z0Detect_state=%d \n", (UINT32)zeDetectLoopCnt, (UINT32)hdmitx_misc_hdmitx_tmds_rxon_reg_regValue);
    }
    if(hdmitx_misc_hdmitx_tmds_rxon_reg_regValue == 0x0)
        return _HDMI_TX_Z0_ALL_LOW;
    else if(HDMITX_MISC_HDMITX_TMDS_RXON_get_tmds_rxonck(hdmitx_misc_hdmitx_tmds_rxon_reg_regValue) == 0)
        return _HDMI_TX_Z0_CLK_LOW;
    else
        return _HDMI_TX_Z0_NOT_ALL_LOW;

#endif // #else //force return ZO value

    return 0;
}

UINT8 ScalerHdmiPhyTx0SetZ0Detect(BOOLEAN enable)
{
    UINT32 hdmitx_phy_hdmitxphy_ctrl17_reg;
    hdmitx_phy_hdmitxphy_ctrl17_reg = rtd_inl(HDMITX_PHY_HDMITXPHY_CTRL17_reg);
    if(enable && HDMITX_PHY_HDMITXPHY_CTRL17_get_reg_tmds_pow(hdmitx_phy_hdmitxphy_ctrl17_reg) == 0)
    {
        rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL17_reg, ~HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_pow_mask, HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_pow(0x1));
    }
    else ///close will cause fifo unstable
    {
        //rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL17_reg, ~HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_pow_mask, HDMITX_PHY_HDMITXPHY_CTRL17_reg_tmds_pow(0x0));
    }

    return 0;
}

UINT8 ScalerHdmiPhyTx0PowerOff(void)
{

    ScalerHdmiTxPhy_PowerOff();
    return _TRUE;
}

StructTimingInfo *ScalerDpStreamGetDisplayTimingInfo(UINT8 ucOutputPort)
{
    g_stHdmiTx0InputTimingInfo.b1HSP = 0; // HSP
    g_stHdmiTx0InputTimingInfo.b1VSP = 0; // VSP
    g_stHdmiTx0InputTimingInfo.b1Interlace = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_INTERLACE); // Interlace
    g_stHdmiTx0InputTimingInfo.b1VideoField = 0; // Field for video compensation
    g_stHdmiTx0InputTimingInfo.usHFreq = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_IHFREQ); // Horizontal Freq. (unit: 0.1kHz)
    g_stHdmiTx0InputTimingInfo.usHTotal = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_TOTAL); // Horizontal Total length (unit: Pixel)
    g_stHdmiTx0InputTimingInfo.usHWidth = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_ACT_WID); // Horizontal Active Width (unit: Pixel)
    g_stHdmiTx0InputTimingInfo.usHStart = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_ACT_STA); // Horizontal Start (unit: Pixel)
//#ifdef H5X_HDMITX_FIXME
    g_stHdmiTx0InputTimingInfo.usHSWidth = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_HSYNC); // Horizontal Sync Pulse Count (unit: SyncProc Clock)
//#endif
    g_stHdmiTx0InputTimingInfo.usVFreq = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_VFREQ); // Vertical Freq. (unit: 0.1Hz)
    g_stHdmiTx0InputTimingInfo.usVTotal = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_TOTAL); // Vertical Total length (unit: HSync)
    g_stHdmiTx0InputTimingInfo.usVHeight = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_ACT_LEN); // Vertical Active Height (unit: HSync)
    g_stHdmiTx0InputTimingInfo.usVStart = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_ACT_STA); // Vertical Start (unit: HSync)
//#ifdef H5X_HDMITX_FIXME
    g_stHdmiTx0InputTimingInfo.usVSWidth = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_VSYNC); // Vertical Sync Width    (unit: HSync)
//#endif

    return &g_stHdmiTx0InputTimingInfo;
}


//#ifdef CONFIG_ENABLE_HDMITX
UINT8 Scaler_TxStateHandler(void)
{
    UINT8 value_u8;

    SET_OUTPUT_POWER_STATUS(_POWER_STATUS_NORMAL); //power status will turn off in 2.0 reset flow of MAC power off
    SET_OUTPUT_STREAM_TYPE(_STREAM_TYPE_SST);//where to set stream type?
#if(_HDMI21_TX_SUPPORT == _ON)
    if(GET_OUTPUT_DFP_DEVICE_EDID_READ_FLG(_P0_OUTPUT_PORT) == _TRUE) //should set in ScalerHdmi21MacTxStreamManagement(), where to call function?
        SET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(_TX0, _TX_STREAM_READY);
    if(ScalerHdmiTxGetDSCEn() == 1)
    {
        SET_HDMI21_MAC_TX_DSCE_EN(_TX0, ScalerHdmiTxGetDSCEn());
        SET_HDMI21_MAC_TX_INPUT_FROM(_TX0, _INPUT_FROM_ENCODER);
        SET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(_TX0, _DOWN_STREAM_DSC);
    }
    // [H5X_HDMITX_BYPASS_HPD_EDID]
    if(ScalerHdmiTxGetBypassLinkTrainingEn()){
        SET_HDMI21_MAC_TX_FRL_RATE(_TX0, ScalerHdmiTxGetTargetFrlRate());
    }
#endif // #if(_HDMI21_TX_SUPPORT == _ON)

    // check HPD status
    ScalerHdmiMacTxHandler(_P0_OUTPUT_PORT);

    value_u8 = ScalerHdmiTxGetStreamStateReady();

    return value_u8;
}
//#endif


void ScalerColorStream422To444(UINT8 ucOutputPort, UINT8 enable)
{
    // for 444->420
    Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_SLR_DISP_422CAP, !enable);
    return;
}

UINT8 ScalerColorStream422to444Support(UINT8 ucOutputPort)
{
    return _SUPPORT_422_TO_444;
}

void ScalerColorStreamYuv2Rgb(UINT8 ucOutputPort, UINT8 enable)
{
    // TBD -- PQ setting
//#ifndef H5X_HDMITX_FIXME
//      #error "FIX-ME@ScalerColorStreamYuv2Rgb()"
//#endif
    return;
}
#if(_DSC_SUPPORT == _ON)
static UINT8 gDscEn=0;
static UINT8 ucDscMode =0;
void ScalerHdmiTxSetDSCEn(UINT8 enable)
{
    if(gDscEn != enable){
        NoteMessageHDMITx("[HDMITX] Set gDscEn=%d->%d\n", gDscEn, enable);
        gDscEn = enable;
    }
}

UINT8 ScalerHdmiTxGetDSCEn(void)
{
    return gDscEn;
}

// DSC mode: 0: disable, 1: default, 2: 12G (bbp=8.125), 3: 12G (bbp=9.9375), 4: 12G (bbp=12.000), 5: 12G (bbp=15.000)
void ScalerHdmiTxSetDscMode(UINT8 dscMode)
{

    if(ucDscMode != dscMode){
        NoteMessageHDMITx("[HDMITX] DscMode=%d->%d\n", ucDscMode, dscMode);
        ucDscMode = dscMode;
    }
    return;
}


// DSC mode: 0: disable, 1: default, 2: 12G (bbp=8.125), 3: 12G (bbp=9.9375), 4: 12G (bbp=12.000), 5: 12G (bbp=15.000)
unsigned char ScalerHdmiTxGetDscMode(void)
{
    return ucDscMode;
}

void ScalerDpStreamGetMsaTimingInfo(EnumOutputPort ucOutputPort, StructTimingInfo *stTimingInfo)
{

    stTimingInfo = g_pstHdmi21MacTxInputTimingInfo[_TX0];
    return;
}

UINT16 ScalerDscDecoderGetElement(EnumOutputPort ucOutputPort, EnumDscDecoderElement enElement)
{
    unsigned short ret=0;

#if 1 // H5X_HDMITX_FIXME -- need only when dsc decoder is enabled?
    ErrorMessageHDMITx("[HDMITX][WARN] NOT IMPLEMENT [%d]@%s", enElement, __FUNCTION__);
    return 0;
#endif

    switch(enElement){
        case _DSC_DECODER_PIXEL_CLK:
            break;
        case _DSC_DECODER_COLOR_SPACE:
            break;
        case _DSC_DECODER_COLOR_DEPTH:
            break;
        case _DSC_DECODER_NATIVE_420:
            break;
        case _DSC_DECODER_PIC_WIDTH:
            break;
         case _DSC_DECODER_SLICE_WIDTH:
            break;
        case _DSC_DECODER_BPP:
            break;
        case _DSC_DECODER_CHUNK_SIZE:
            break;
        default:
            break;
   }

    return ret;
}
#endif
#ifdef NOT_USED_NOW
INT8 ScalerHdmiPhyTx0SetIBHNTrim(UINT8 ucIBHNTRIMCurrent)
{

    return 0;
}


UINT8 ScalerHdmiPhyTx0GetIBHNTrim(void)
{

    return 0;
}

INT8 ScalerHdmiPhyTx0PowerOn(void)
{

    return 0;
}

INT8 ScalerHdmiPhyTx0CMUPowerOff(void)
{

    return 0;
}

INT8 ScalerHdmiPhyTx0SetCMUSignal(BOOLEAN enable)
{

    return 0;
}
void ScalerColorStreamRgb2Yuv(UINT8 ucOutputPort, UINT8 enable, EnumColorSpace type)
{
    // TBD -- PQ setting
//#ifndef H5X_HDMITX_FIXME
//      #error "FIX-ME@ScalerColorStreamRgb2Yuv()"
//#endif
    return;
}

void ScalerColorStreamYuv2RgbClamp(UINT8 ucOutputPort, UINT8 enable)
{
        // TBD -- PQ setting
//#ifndef H5X_HDMITX_FIXME
//      #error "FIX-ME@ScalerColorStreamYuv2Rgb()"
//#endif
        return;
}

void ScalerColorStream422To420(UINT8 ucOutputPort, UINT8 enable)
{
    // TBD
//#ifndef H5X_HDMITX_FIXME
//      #error "FIX-ME@ScalerColorStream422To420()"
//#endif
    return;
}


void ScalerColorStream444To420(UINT8 ucOutputPort, UINT8 enable)
{
    // TBD
//#ifndef H5X_HDMITX_FIXME
//      #error "FIX-ME@ScalerColorStream444To420()"
//#endif
    return;
}
#endif
UINT8 ScalerColorStream444to420Support(UINT8 ucOutputPort)
{
    return _SUPPORT_444_TO_420;
}



UINT8 ScalerColorStream422to420Support(UINT8 ucOutputPort)
{
    return _SUPPORT_422_TO_420;
}


void ScalerHdmiTxSetTargetFrlRate(EnumHdmi21FrlType frlType)
{

    HDMITX_OUT_FRL_RATE = frlType;
    return;
}

#if 0
EnumHdmi21FrlType ScalerHdmiTxGetTargetFrlRate(void)
{
    return HDMITX_OUT_FRL_RATE;
}
#endif

UINT8 Scaler_Wait_for_event(UINT32 addr, UINT32 event, UINT32 timecount)
{
//#ifdef _MARK2_FIXME_H5X_TXSOC_FLOW // [MARK2] FIX-ME -- replace TXSOC setting into main DTG setting flow
        UINT32 timeout = 0x1493E0;
        UINT16 count = 0;
        UINT16 tmpTxsocLinecount = 0;
        UINT16 tmpVodmaLinecount = 0;
        static UINT16	maxTxsocLinecount = 0;
        static UINT16	maxVodmaLinecount = 0;
        static UINT16 	curcount = 0;
	totalcount += timecount+1;

	do{
		tmpTxsocLinecount = rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_reg);
#ifndef CONFIG_ENABLE_MARK2_ZEBU_BRING_UP // [MARK2] FIX-ME -- remove me after VODMA clock is enabled
		tmpVodmaLinecount = rtd_inl(VODMA_VODMA_LINE_ST_reg);
#endif // #ifndef CONFIG_ENABLE_MARK2_ZEBU_BRING_UP // [MARK2] FIX-ME -- remove me after VODMA clock is enabled
		if(tmpTxsocLinecount > maxTxsocLinecount)
		{
			maxTxsocLinecount = tmpTxsocLinecount;
		}

		if(tmpVodmaLinecount > maxVodmaLinecount)
		{
			maxVodmaLinecount = tmpVodmaLinecount;
		}

		if(rtd_inl(addr) & event)
		{
			if(curcount <=19)
			{
				TxsocLinecount[curcount] = rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_new_meas0_linecnt_real_reg);
#ifndef CONFIG_ENABLE_MARK2_ZEBU_BRING_UP // [MARK2] FIX-ME -- remove me after VODMA clock is enabled
				VodmaLinecount[curcount] = rtd_inl(VODMA_VODMA_LINE_ST_reg);
#endif // #ifndef CONFIG_ENABLE_MARK2_ZEBU_BRING_UP // [MARK2] FIX-ME -- remove me after VODMA clock is enabled
				curcount++;
			}
			count++;
			rtd_outl(addr, event);//clear

           //DebugMessageHDMITx("Wait %x Cnt=%d/%d@LC=%d/%d\n", addr, count, timecount, VodmaLinecount[curcount], TxsocLinecount[curcount]);

		}
		if(count >= timecount)
		{
			//DebugMessageHDMITx("[HDMITX] Write(%d) sucess  !\n", timecount);
			return TRUE;
		}
		timeout--;
  	}while(timeout);
	FatalMessageHDMITx("[HDMITX][WARN] Pending_status timeout!@Addr=%x, Event=%x\n", addr, event);
//#endif
	return FALSE;
}

INT32 ScalerTimerActiveTimerEvent(UINT32 time_ms, EnumScalerTimerEventHdmiTx0Type event)
{

    static UINT32 lastDumpStc = 0;
    static EnumScalerTimerEventHdmiTx0Type pre_event =-1;
    UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
    struct timer_data *tmd = &_SCALER_TIMER_EVENT[event];

#if 1//#ifndef _MARK2_FIXME_H5X_ROS_TIMER_API // [MARK2] FIX-ME -- replace ROS API into Linux API
    if(event >= _SCALER_TIMER_EVENT_NUM){
        FatalMessageHDMITx("[HDMI_TX] Event[%d] OVER RANGE!!@%s\n", event, __FUNCTION__);
        return 0;
    }

    if(_SCALER_TIMER_EVENT[event].timer.function != NULL){
        if(((UINT32)(stc - lastDumpStc)) > 90000 || (pre_event != event)){ // dump period=1sec
            NoteMessageHDMITx("[HDMI_TX] Event[%d] is Runing@...\n", event);
            lastDumpStc = stc;
            pre_event = event;
        }
        return 0;
    }
    time_ms = (time_ms + TIMER_TICKS/2)/TIMER_TICKS;
    pre_event = event;
    tmd->event_data = event;
    setup_timer(&tmd->timer, (void *)ScalerSetTimerEvent, 0);
    tmd->timer.expires = jiffies + time_ms*HZ/250;// currently HZ is 250, 1000/250 = 4 ms every ticks
    add_timer(&tmd->timer);
    //NoteMessageHDMITx("[HDMI_TX]add timer success event = %d\n",event);
#else // #ifndef _MARK2_FIXME_H5X_ROS_TIMER_API // [MARK2] FIX-ME -- replace ROS API into Linux API
    PTIMER_DPC_ROUTINE pFunc;

    if(ROSTimerGet(&_SCALER_TIMER_EVENT[event]) > 0)//ROS timer will count down to zero. when timer count to zero, timer will remove from active list
    {
        if(((UINT32)(stc - lastDumpStc)) > 90000 || pre_event != event){ // dump period=1sec
            NoteMessageHDMITx("[HDMI_TX] %s,exist event=%d, time count =%d\n",__FUNCTION__, event,ROSTimerGet(&_SCALER_TIMER_EVENT[event]));
            lastDumpStc = stc;
            pre_event = event;
        }
        return 0;
    }

    pre_event = event;
    pFunc = (PTIMER_DPC_ROUTINE)ScalerSetTimerEvent;
    ROSTimerCreate(&_SCALER_TIMER_EVENT[event], HDMITX_DETEC_TASKID, (ROS_SIG)NULL, pFunc, ((void *) event));
    ROSTimerSet(&_SCALER_TIMER_EVENT[event], time_ms);// 1tick 1ms

//#ifndef H5X_HDMITX_FIXME
//      #error "FIX-ME@ScalerTimerActiveTimerEvent()"
//#endif
#endif// #else // #ifndef _MARK2_FIXME_H5X_ROS_TIMER_API // [MARK2] FIX-ME -- replace ROS API into Linux API
    return 0;
}


INT32 ScalerTimerCancelTimerEvent(EnumScalerTimerEventHdmiTx0Type event)
{

#if 1//#ifndef _MARK2_FIXME_H5X_ROS_TIMER_API // [MARK2] FIX-ME -- replace ROS API into Linux API
    if(event >= _SCALER_TIMER_EVENT_NUM){
        FatalMessageHDMITx("[HDMI_TX] Event[%d] OVER RANGE!!@%s\n", event, __FUNCTION__);
        return 0;
    }

    if(_SCALER_TIMER_EVENT[event].timer.function != NULL){
        del_timer(&_SCALER_TIMER_EVENT[event].timer);
        _SCALER_TIMER_EVENT[event].timer.function = NULL;
    }else{
        //FatalMessageHDMITx("[HDMI_TX] Event[%d] Timer is Not Exist!!@%s\n", event,__FUNCTION__);
    }

#else // #ifndef _MARK2_FIXME_H5X_ROS_TIMER_API // [MARK2] FIX-ME -- replace ROS API into Linux API
    ROSTimerClear(&_SCALER_TIMER_EVENT[event]);// clear timer
//#ifndef H5X_HDMITX_FIXME
//      #error "FIX-ME@ScalerTimerCancelTimerEvent()"
//#endif
#endif // #else // #ifndef _MARK2_FIXME_H5X_ROS_TIMER_API // [MARK2] FIX-ME -- replace ROS API into Linux API
    return 0;
}


INT32 ScalerTimerSearchActiveTimerEvent(EnumScalerTimerEventHdmiTx0Type event)
 {

#if 1//#ifndef _MARK2_FIXME_H5X_ROS_TIMER_API // [MARK2] FIX-ME -- replace ROS API into Linux API
    if(event >= _SCALER_TIMER_EVENT_NUM){
        FatalMessageHDMITx("[HDMI_TX] Event[%d] OVER RANGE!!@%s\n", event, __FUNCTION__);
        return 0;
    }

    if(_SCALER_TIMER_EVENT[event].timer.function)
        return 1;

#else
    ROSTimerGet(&_SCALER_TIMER_EVENT[event]);
//#ifndef H5X_HDMITX_FIXME
//      #error "FIX-ME@ScalerTimerSearchActiveTimerEvent()"
//#endif
#endif // #if 0 // [MARK2] FIX-ME -- NOTEXIST In Linux

    return 0;
}

// 5, HDMITX20_MAC0_HDCP14_ENGINE_10_reg, _BIT4, 1
INT32 HDMITX_ScalerTimerPollingFlagProc(UINT32 pts_90k, UINT32 regAddr, UINT32 mask, UINT8 status)
{

    Scaler_Wait_for_event(regAddr, mask, 1);

//#ifndef H5X_HDMITX_FIXME
//    #error "FIX-ME@ScalerTimerPollingFlagProc()"
//#endif
    return _TRUE;
}

// SEC(10), _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_CHECK
INT32 ScalerTimerReactiveTimerEvent(UINT32 time_ms, EnumScalerTimerEventHdmiTx0Type event)
{
#if 1//#ifndef _MARK2_FIXME_H5X_ROS_TIMER_API // [MARK2] FIX-ME -- replace ROS API into Linux API
    if(event >= _SCALER_TIMER_EVENT_NUM){
        FatalMessageHDMITx("[HDMI_TX] Event[%d] OVER RANGE!!@%s\n", event, __FUNCTION__);
        return 0;
    }

    if(_SCALER_TIMER_EVENT[event].timer.function != NULL)
        del_timer(&_SCALER_TIMER_EVENT[event].timer);
    else
        DebugMessageHDMITx("[HDMI_TX][%s] Event[%d] Timer is Not Exist!!\n",__FUNCTION__ ,event);

    time_ms = (time_ms + TIMER_TICKS/2)/TIMER_TICKS;
    _SCALER_TIMER_EVENT[event].event_data =event;
    setup_timer(&_SCALER_TIMER_EVENT[event].timer, (void *)ScalerSetTimerEvent, 0);
    _SCALER_TIMER_EVENT[event].timer.expires = jiffies + time_ms*HZ/250;
    add_timer(&_SCALER_TIMER_EVENT[event].timer);

#else // #ifndef _MARK2_FIXME_H5X_ROS_TIMER_API // [MARK2] FIX-ME -- replace ROS API into Linux API
    PTIMER_DPC_ROUTINE pFunc;

    ROSTimerClear(&_SCALER_TIMER_EVENT[event]);// clear timer first

    pFunc = (PTIMER_DPC_ROUTINE)ScalerSetTimerEvent;
    ROSTimerCreate(&_SCALER_TIMER_EVENT[event], HDMITX_DETEC_TASKID, (ROS_SIG)NULL, pFunc, ((void *) event));
    ROSTimerSet(&_SCALER_TIMER_EVENT[event], time_ms);// 1tick 1ms

//#ifndef H5X_HDMITX_FIXME
//      #error "FIX-ME@ScalerTimerReactiveTimerEvent()"
//#endif
#endif// #else // #ifndef _MARK2_FIXME_H5X_ROS_TIMER_API // [MARK2] FIX-ME -- replace ROS API into Linux API

        return 0;

}


INT32 ScalerTimerWDActivateTimerEvent_EXINT0(UINT32 pts_90k, EnumScalerWdTimerEventHdmiTx0Type event)
{
//#ifndef H5X_HDMITX_FIXME
//      #error "FIX-ME@ScalerTimerWDActivateTimerEvent_EXINT0()"
//#endif
    return 0;
}



void ScalerSetTimerEvent(struct timer_list *timer)
{
    static UINT8 last_event=0;
#if 1 // [MARK2][FIX-ME]
    EnumScalerTimerEventHdmiTx0Type event=0;
    struct timer_data *tmd = from_timer(tmd, timer,timer);
    event =tmd->event_data;
    tmd->timer.function =NULL;
    //NoteMessageHDMITx("[HDMITX][FIX-ME] NOT SUPPORT TIMER API YET!! %d",event);
    //return;
#endif



    if(last_event != event){
        DebugMessageHDMITx("[HDMI Tx] ScalerSetTimerEvent() event = %d\n", (UINT32)event);
        last_event = event;
    }
    DebugMessageHDMITx("[HDMI Tx] ScalerSetTimerEvent() event = %d\n", (UINT32)event);
    switch (event)
    {
        case _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_CHECK:

            SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_SCRAMBLE_CHECK);
            break;

        case _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_ENABLE:

            SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_SCRAMBLE_ENABLE);
            break;

        case _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_DISABLE:

            SET_HDMI_MAC_TX0_SCDC_PROC_STATE(_HDMI_TX_SCDC_PROC_STATE_SCRAMBLE_DISABLE);
            break;

        case _SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT:
            SET_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS();
            break;

        case _SCALER_TIMER_EVENT_HDMI_TX0_HPD_TOGGLE_EVENT:
            SET_HDMI_MAC_TX0_HPD_TOGGLE_TIMEOUT();
            break;

        case _SCALER_TIMER_EVENT_HDMI_TX0_Z0_TOGGLE_EVENT:
            SET_HDMI_MAC_TX0_Z0_TOGGLE_TIMEOUT();
            break;

        case _SCALER_TIMER_EVENT_HDMI_TX0_AUDIO_CTS_CODE_CHECK:

#if(_AUDIO_SUPPORT == _ON)
#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
            ScalerHdmiMacTx0CtsCodeCheckEventSet();
#endif
#endif
            break;

#if(_HDMI21_TX_SUPPORT == _ON)
        case _SCALER_TIMER_EVENT_HDM21_TX0_FRL_PASS_TIMER:
            SET_HDMI21_MAC_TX_LTS_PASS_POLLING(_TX0);
            break;

        case _SCALER_TIMER_EVENT_HDM21_TX0_LTS3_TIMER:
            SET_HDMI21_MAC_TX_LTS3_TIMER_START(_TX0);

            break;
#endif // #if(_HDMI21_TX_SUPPORT == _ON)
        case _SCALER_TIMER_EVENT_HDMI_TX0_GCPTOHW_TIMER:
            gcpswtohw = _HDMI_TX_GCP_TOHW_JUMP;
            break;
        default:
            //NoteMessageHDMITx("[HDMITX] UNKNOWN TIMER EVENT:%x\n", (UINT32)event);
            break;
    }

#if 1//#ifndef _MARK2_FIXME_H5X_ROS_TIMER_API // [MARK2] FIX-ME -- replace ROS API into Linux API
    if((event < _SCALER_TIMER_EVENT_NUM) && _SCALER_TIMER_EVENT[event].timer.function)
        _SCALER_TIMER_EVENT[event].timer.function = NULL;
#endif
}


void ScalerHdmiTxSetBypassLinkTrainingEn(UINT8 enable)
{

    if(ucTxBypassLinkTrainingEn != enable)
        NoteMessageHDMITx("[HDMITX] Bypass LinkTraining=%d->%d\n", (UINT32)ucTxBypassLinkTrainingEn, (UINT32)enable);

    ucTxBypassLinkTrainingEn = enable;
}

#ifdef NOT_USED_NOW
UINT8 ScalerHdmiTxGetStreamOffWithoutLinkTrainingFlag(void)
{
    return _HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING;
}
#endif

void ScalerTxHdmiTx0PhyConfig(void)
{
  #if(_HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING == _ON) // [TEST]support fast timing change flow
    if(ScalerHdmiTxGetFastTimingSwitchEn() && (GET_HDMI_MAC_TX0_MODE_STATE() == _HDMI_TX_MODE_STATUS_INITIAL)){
    }else
  #endif
    // disable async fifo before change PLL clock
    ScalerHdmiTx0_Mac_asyncFifoEnable(_DISABLE); // 0x0 : Disable async_fifo

    // HDMITX PHY PLL setting & swing setting
    ScalerHdmiTxSetPhyAndSwing();
    // enable async fifo after PLL clock stable
    udelay(150);
#if(_HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING == _ON) // [TEST]support fast timing change flow
    if(ScalerHdmiTxGetFastTimingSwitchEn() && (GET_HDMI_MAC_TX0_MODE_STATE() == _HDMI_TX_MODE_STATUS_INITIAL)){
    }else
#endif
    ScalerHdmiTx0_Mac_asyncFifoEnable(_ENABLE); // 0x1 : Enable async_fifo

    return;
}

void ScalerHdmiTxHPDInitial(void)
{
    //pinmux driver will handle TX HPD pin mux setting
    hdmitx_ctrl_set_tx_hpd_init();
    
    rtd_maskl(PINMUX_GPIO_LTOP_CFG_2_reg,~(_BIT23|_BIT22|_BIT21|_BIT20),0x0);//GPIO23: Hdmitx_hpd src1
    rtd_maskl(PINMUX_GPIO_LTOP_CFG_2_reg,~(_BIT18),_BIT18);//GPIO23 pull low on, 0x18000838[18]=1
    rtd_maskl(PINMUX_Pin_Mux_Ctrl2_reg,~(_BIT23|_BIT22|_BIT21),_BIT21);//0x180008cc[23:21]=1: hpd=src1
    return;
}


#ifdef NOT_USED_NOW
// Send HDMITX connect status to SCPU2 (target is send to TVSOC)
INT8 ScalerHdmiTxSendTxConnectStatus(UINT8 bUnPlugFlag)
{
    return _TRUE;
}
#endif

void ScalerHdmiTxSetHDRFlag(UINT8 bHDRFlag)
{
    DebugMessageHDMITx("[HDMITX] Set HDR flag %d\n", (UINT32)bHDRFlag);
    ucTxHDRFlag = bHDRFlag;
}

UINT8 ScalerHdmiTxGetHDRFlag(void)
{
    return ucTxHDRFlag;
}
#ifdef NOT_USED_NOW
void ScalerHdmiTxSetVTEMpacektEn(UINT8 enable)
{
    if(ucVTEMpacketEn != enable)
        DebugMessageHDMITx("[HDMITX] VTEM packet Enable=%d->%d\n", (UINT32)ucVTEMpacketEn, (UINT32)enable);

    ucVTEMpacketEn = enable;
}

void ScalerHdmiTxSetVRRFlagEn(UINT8 enable)
{
    if(ucVRRFlagEn != enable)
        DebugMessageHDMITx("[HDMITX] VRR flag Enable=%d->%d\n", (UINT32)ucVRRFlagEn, (UINT32)enable);

    ucVRRFlagEn = enable;

}
#endif


#if 1//#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
void ScalerHdmiTxRepeaterClrSourceInfo(void)
{
    hdmiRepeaterSrcInfo.hdmiVsifAllmMode = 0;
    hdmiRepeaterSrcInfo.dvVsifMode = DOLBY_HDMI_VSIF_DISABLE;
    hdmiRepeaterSrcInfo.empDvMode = 0;
    hdmiRepeaterSrcInfo.empVtemVrrMode = 0;
    hdmiRepeaterSrcInfo.sdpAmdFreeSyncMode = 0;
}



UINT8 ScalerHdmiTxRepeaterSetBypassPktHdrEn(EnumHdmiRepeaterBypassPktHdrType bypassPktHdrType, UINT8 enable)
{
    if(enable){
        hdmiRepeaterBypassPktHdrEnMask |= bypassPktHdrType;
    }else{
        hdmiRepeaterBypassPktHdrEnMask &= (~bypassPktHdrType);
    }
    InfoMessageHDMITx("[HDMITX] Bypass PKT Hdr[%x]=>%x\n", (UINT32)bypassPktHdrType, (UINT32)hdmiRepeaterBypassPktHdrEnMask);
    return 0;
}



#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
UINT8 ScalerHdmiTxRepeaterSetSwPktEn(EnumHdmiRepeaterSwPktType swPktType, UINT8 enable)
{
    if(enable){
        hdmiRepeaterSwPktEnMask |= swPktType;
    }else{
        hdmiRepeaterSwPktEnMask &= (~swPktType);
    }
    InfoMessageHDMITx("[HDMITX] SW PKT[%x]=>%x\n", (UINT32)swPktType, (UINT32)hdmiRepeaterSwPktEnMask);
    return 0;
}
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT


void ScalerHdmiTxRepeaterSetSourceInfo_Allm(UINT8 enable)
{
    if(hdmiRepeaterSrcInfo.hdmiVsifAllmMode != enable){
        DebugMessageHDMITx("[HDMITX] Set ALLM=%d\n", (UINT32)enable);
        hdmiRepeaterSrcInfo.hdmiVsifAllmMode = enable;
    }
}



void ScalerHdmiTxRepeaterSetSourceInfo_hdrMode(HDR_MODE hdr_mode)
{
    //UINT8 ret=0;

    if(hdmiRepeaterSrcInfo.hdr_mode != hdr_mode){
        DebugMessageHDMITx("[HDMITX] Set HDR mode=%d\n", (UINT32)hdr_mode);
        hdmiRepeaterSrcInfo.hdr_mode = hdr_mode;
    }
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    if(hdr_mode == HDR_HDR10_HDMI){
#ifdef CONFIG_MACH_RTD2801_ZEBU
        memset((UINT8*)&hdmiRepeaterSrcInfo.hdmi_drmInfo, 0, sizeof(HDMI_DRM_T));
        hdmiRepeaterSrcInfo.hdmi_drmInfo.type_code = 0x87;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.ver = 0x1;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.len = 0x1a;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.eEOTFtype = 0x2;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.eMeta_Desc = 0x0;
  #if 1 // for test only
        hdmiRepeaterSrcInfo.hdmi_drmInfo.display_primaries_x0 = 0x12;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.display_primaries_y0 = 0x34;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.display_primaries_x1 = 0x56;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.display_primaries_y1 = 0x78;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.display_primaries_x2 = 0x9a;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.display_primaries_y2 = 0xbc2;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.white_point_x = 0xde;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.white_point_y = 0xf0;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.max_display_mastering_luminance = 0x12;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.min_display_mastering_luminance = 0x34;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.maximum_content_light_level = 0x56;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.maximum_frame_average_light_level = 0x78;
        hdmiRepeaterSrcInfo.hdmi_drmInfo.reserved = 0xff;
  #endif
#else // #ifdef CONFIG_MACH_RTD2801_ZEBU
        ret = vfe_hdmi_drv_get_drm_info(&hdmiRepeaterSrcInfo.hdmi_drmInfo);
#endif // #else // #ifdef CONFIG_MACH_RTD2801_ZEBU
        if(ret != HDMI_DRV_NO_ERR){
            NoteMessageHDMITx("[HDMITX] Set HDRMD FAIL!!\n");
        }else{
            NoteMessageHDMITx("type/ver/len/eof=%x/%x/", (UINT32)hdmiRepeaterSrcInfo.hdmi_drmInfo.type_code, (UINT32)hdmiRepeaterSrcInfo.hdmi_drmInfo.ver);
            NoteMessageHDMITx("%x/%x\n", (UINT32)hdmiRepeaterSrcInfo.hdmi_drmInfo.len, (UINT32)hdmiRepeaterSrcInfo.hdmi_drmInfo.eEOTFtype);
            NoteMessageHDMITx("MD=%x\n", (UINT32)hdmiRepeaterSrcInfo.hdmi_drmInfo.eMeta_Desc);
        }
    }
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    return;
}

void ScalerHdmiTxRepeaterSetSourceInfo_DvVsifMode(DOLBY_HDMI_VSIF_T mode)
{
    //UINT8 ret=true;

    if(hdmiRepeaterSrcInfo.dvVsifMode != mode){
        DebugMessageHDMITx("[HDMITX] Set DV VSIF mode=%d\n", (UINT32)mode);
        hdmiRepeaterSrcInfo.dvVsifMode = mode;
    }
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    if(mode != DOLBY_HDMI_VSIF_DISABLE){
#ifdef CONFIG_MACH_RTD2801_ZEBU
        memset((UINT8*)&hdmiRepeaterSrcInfo.hdmi_dvVsiInfo, 0, sizeof(HDMI_VSI_T));
        hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.VSIF_TypeCode = 0x81;
        hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.VSIF_Version = 0x1;
        hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Length = 0x1b;
        hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Checksum = 0x4a;
        hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Reg_ID[0]= 0x46;
        hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Reg_ID[1]= 0xd0;
        hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Reg_ID[2]= 0x0;
        hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Payload[0] = 0x3;
        hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Payload[1] = 0x0;
#else
        ret = drvif_Hdmi_GetDolbyVsInfo(&hdmiRepeaterSrcInfo.hdmi_dvVsiInfo);
#endif
        if(ret != true){
            NoteMessageHDMITx("[HDMITX] Set DV VSIF FAIL!!\n");
        }else{
            NoteMessageHDMITx("Type/Ver/Len=%x/%x/", (UINT32)hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.VSIF_TypeCode, (UINT32)hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.VSIF_Version);
            NoteMessageHDMITx("%x\n", (UINT32)hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Length);
            NoteMessageHDMITx("CheckSum=%x\n", (UINT32)hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Checksum);
            NoteMessageHDMITx("ID=%x %x ", (UINT32)hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Reg_ID[2], (UINT32)hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Reg_ID[1]);
            NoteMessageHDMITx("%x\n", (UINT32)hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Reg_ID[0]);
            NoteMessageHDMITx("PB=%x %x\n", (UINT32)hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Payload[0], (UINT32)hdmiRepeaterSrcInfo.hdmi_dvVsiInfo.Payload[1]);
        }
    }
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    return;
}

void ScalerHdmiTxRepeaterSetSourceInfo_aviInfoFrame(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    UINT8 ret=true;
#ifdef CONFIG_MACH_RTD2801_ZEBU
    memset((UINT8*)&hdmiRepeaterSrcInfo.hdmi_aviInfo, 0, sizeof(HDMI_AVI_T));
    *((UINT8*)(&hdmiRepeaterSrcInfo.hdmi_aviInfo)+0) = 0x82;
    *((UINT8*)(&hdmiRepeaterSrcInfo.hdmi_aviInfo)+1) = 0x2;
    *((UINT8*)(&hdmiRepeaterSrcInfo.hdmi_aviInfo)+2) = 0xd;
    *((UINT8*)(&hdmiRepeaterSrcInfo.hdmi_aviInfo)+3)= 0x89;
    *((UINT8*)(&hdmiRepeaterSrcInfo.hdmi_aviInfo)+4) = 0x88;
    *((UINT8*)(&hdmiRepeaterSrcInfo.hdmi_aviInfo)+5) = 0x87;
    *((UINT8*)(&hdmiRepeaterSrcInfo.hdmi_aviInfo)+6) = 0x86;
    *((UINT8*)(&hdmiRepeaterSrcInfo.hdmi_aviInfo)+7) = 0x85;
#else
    ret = drvif_Hdmi_GetAviInfoFrame(&hdmiRepeaterSrcInfo.hdmi_aviInfo);
#endif

    NoteMessageHDMITx("[HDMITX] Set AVI InfoFrame %s\n", (ret==true? "SUCESS": "FAIL!"));
    if(ret == true){
        NoteMessageHDMITx("Type/Ver/Len=%x %x ", (UINT32)hdmiRepeaterSrcInfo.hdmi_aviInfo.type_code, (UINT32)hdmiRepeaterSrcInfo.hdmi_aviInfo.ver);
        NoteMessageHDMITx("%x\n", (UINT32)hdmiRepeaterSrcInfo.hdmi_aviInfo.len);
        NoteMessageHDMITx("PB[5:1]=%x %x", (UINT32)*((UINT8*)&hdmiRepeaterSrcInfo.hdmi_aviInfo)+7, (UINT32)*((UINT8*)&hdmiRepeaterSrcInfo.hdmi_aviInfo)+6);
        NoteMessageHDMITx("%x %x ", (UINT32)*((UINT8*)&hdmiRepeaterSrcInfo.hdmi_aviInfo)+5, (UINT32)*((UINT8*)&hdmiRepeaterSrcInfo.hdmi_aviInfo)+4);
        NoteMessageHDMITx("%x\n", (UINT32)*((UINT8*)&hdmiRepeaterSrcInfo.hdmi_aviInfo)+3);
    }
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    return;
}

UINT8 ScalerHdmiTxRepeaterGetSourceInfo_aviInfoFrame(UINT8 *aviInfoFrame)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    if(aviInfoFrame != NULL){
        memcpy(aviInfoFrame, (UINT8*)&hdmiRepeaterSrcInfo.hdmi_aviInfo, sizeof(HDMI_AVI_T));
        return 0;
    }

    return 1;
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    return 0;
}

void ScalerHdmiTxRepeaterSetSourceInfo_vtemVrrEn(UINT8 enable)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    UINT8 ret=EM_VTEM_INFO_LEN;

    if(hdmiRepeaterSrcInfo.empVtemVrrMode != enable){
        NoteMessageHDMITx("[HDMITX] Set VRR_EN=%d\n", (UINT32)enable);
        hdmiRepeaterSrcInfo.empVtemVrrMode = enable;
    }

    if(enable){
#ifdef CONFIG_MACH_RTD2801_ZEBU
        memset((UINT8*)&hdmiRepeaterSrcInfo.hdmi_empVtemBuf[0], 0, EM_VTEM_INFO_LEN);
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[ 0]= 0x7f;     // HB0, 0x7F (EMP)
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[ 1]= 0xc0;     // HB1, B[7]: First, B[6]: Last, B[5:0]: Rsvd
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[ 2]= 0x0;      // HB2, Sequence Index, First=0
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[ 3] = 0x84;    // PB0: B[7]: New=1, B[6]: End=0, B[5:4]: DS_Type=0, B[3]: AFR=0, B[2]: VFR=1, B[1] Sync=0, B[0] Rsvd(0)
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[ 4] = 0x0;     // PB1: Rsvd(0)
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[ 5] = 0x1;     // PB2: Organization_ID=1
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[ 6] = 0x0;     // PB3: Data_Set_Tag=1 (MSB)
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[ 7] = 0x1;     // PB4: Data_Set_Tag=1 (LSB)
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[ 8] = 0x0;     // PB5: Data_Set_Length=4(MSB)
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[ 9] = 0x4;     // PB6: Data_Set_Length=4(LSB)
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[10] = 0x91;     // PB7: MD0=1 (B[1]: M_CONST=0, B[0]: VRR_EN=1)   //B[7:4]: FVA_Factor_M1, B[1]: M_CONST, B[0]: VRR_EN
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[11] = 0x3f;     // PB8: MD1                                                               // B[7:0]: base VFront[7:0]
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[12] = 0x00;     // PB9: MD2                                                               // B[2] RB, B[1:0]: base frame rate[9:8]
        hdmiRepeaterSrcInfo.hdmi_empVtemBuf[13] = 0x3c;     // PB10: MD3                                                              // B[7:0]: base frame rate[7:0]
#else
        ret = drvif_Hdmi_GetEmpPacket(KADP_VFE_HDMI_EMP_VTEM, &hdmiRepeaterSrcInfo.hdmi_empVtemBuf[0], EM_VTEM_INFO_LEN);
#endif
        if(ret != EM_VTEM_INFO_LEN){
            NoteMessageHDMITx("[HDMITX] Set EMP VTEM FAIL!\n");
        }else{
            NoteMessageHDMITx("HB=%x %x ", (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[0], (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[1]);
            NoteMessageHDMITx("%x\n", (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[2]);
            NoteMessageHDMITx("PB=%x %x ", (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[3], (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[4]);
            NoteMessageHDMITx("%x %x ", (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[5], (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[6]);
            NoteMessageHDMITx("%x %x ", (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[7], (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[8]);
            NoteMessageHDMITx("%x %x ", (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[9], (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[10]);
            NoteMessageHDMITx("%x %x ", (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[11], (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[12]);
            NoteMessageHDMITx("%x\n", (UINT32)hdmiRepeaterSrcInfo.hdmi_empVtemBuf[13]);
        }
    }
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
}


UINT8 ScalerHdmiTxRepeaterGetSourceInfo_empVtemInfo(void)
{
#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    if(p_emp_vtem_t == NULL){
        NoteMessageHDMITx("[HDMITX] NULL PTR@ScalerHdmiTxRepeaterGetSourceInfo_empVtemInfo()\n");
        return 0;
    }

    p_emp_vtem_t->FVA_Factor_M1 = (hdmiRepeaterSrcInfo.hdmi_empVtemBuf[10] & (_BIT7 | _BIT6 | _BIT5 | _BIT4))>>4;
    //p_emp_vtem_t->QMS_EN= (hdmiRepeaterSrcInfo.hdmi_empVtemBuf[10] & (_BIT2))>>2;
    p_emp_vtem_t->M_CONST = (hdmiRepeaterSrcInfo.hdmi_empVtemBuf[10] & (_BIT1))>>1;
    p_emp_vtem_t->VRR_EN = (hdmiRepeaterSrcInfo.hdmi_empVtemBuf[10] & (_BIT0));
    p_emp_vtem_t->Base_Vfront = hdmiRepeaterSrcInfo.hdmi_empVtemBuf[11];
    //p_emp_vtem_t->Next_TFR = (hdmiRepeaterSrcInfo.hdmi_empVtemBuf[12] & (_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3))>>3;
    p_emp_vtem_t->RB = (hdmiRepeaterSrcInfo.hdmi_empVtemBuf[12]& (_BIT2))>>2;
    p_emp_vtem_t->Base_Refresh_Rate = ((hdmiRepeaterSrcInfo.hdmi_empVtemBuf[12]& (_BIT1 | _BIT0))<<8) | hdmiRepeaterSrcInfo.hdmi_empVtemBuf[13];

    return p_emp_vtem_t->VRR_EN;
#else // #ifndef CONFIG_HDMITX_REPEATER_SUPPORT
    return 0;
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
}
//#endif // #if 1//#ifdef CONFIG_HDMITX_REPEATER_SUPPORT


#endif
#if(_AUDIO_SUPPORT == _ON)
extern StructHdmitxDisplayInfo g_scaler_display_info;

UINT8 ScalerStreamAudioGetChannelCount(EnumOutputPort ucOutputPort) 
{
if(g_scaler_display_info.input_src == 0){
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
    if(GET_H_AUDIO_FSM(0) == AUDIO_FSM_AUDIO_CHECK){
        DebugMessageHDMITx("[HDMITX] AUD_CH_NUM=%d\n", (UINT32)GET_HDMIRX_AUDIO_ST_CHANNEL_NUM());
        return (GET_HDMIRX_AUDIO_ST_CHANNEL_NUM()? GET_HDMIRX_AUDIO_ST_CHANNEL_NUM() -1: 0);
    }else{
        DebugMessageHDMITx("[HDMITX] Audio FSM=[%d]@GetChannelCount\n", (UINT32)GET_H_AUDIO_FSM(0));
    }
#endif
    }else{
#if defined(CONFIG_ENABLE_DPRX)
            if(DRVIF_DPRX_IS_AUDIO_MUTE() == 0){
                DebugMessageHDMITx("[HDMITX] DPRX AUD_CH_NUM=%d\n", (UINT32)DRVIF_DPRX_GET_AUDIO_CHANNEL_NUM());
                return (DRVIF_DPRX_GET_AUDIO_CHANNEL_NUM()? DRVIF_DPRX_GET_AUDIO_CHANNEL_NUM() -1: 0);
            }else{
                DebugMessageHDMITx("[HDMITX] DPRX Audio mute=[%d]@GetChannelCount\n", (UINT32)DRVIF_DPRX_IS_AUDIO_MUTE());
            }
#endif
        }
    return 0;//
}


UINT8 ScalerStreamAudioGetCodingType(EnumOutputPort ucOutputPort)
{
if(g_scaler_display_info.input_src == 0){
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
    if(GET_H_AUDIO_FSM(0) == AUDIO_FSM_AUDIO_CHECK){
        DebugMessageHDMITx("[HDMITX] AUD_HBRSampleCnt=%d\n", (UINT32)GET_HDMIRX_AUDIO_ST_HBR_SAMPLE_COUNT());
        if(GET_HDMIRX_AUDIO_ST_HBR_SAMPLE_COUNT())
            return _HBR_ASP;
    }else{
        DebugMessageHDMITx("[HDMITX] Audio FSM=[%d]@GetCodingType\n", (UINT32)GET_H_AUDIO_FSM(0));
    }
#endif
    }else{
#if defined(CONFIG_ENABLE_DPRX)
            if(DRVIF_DPRX_IS_AUDIO_MUTE() == 0){
                DebugMessageHDMITx("[HDMITX] AUD_HBRSampleCnt=%d\n", (UINT32)DRVIF_DPRX_GET_AUDIO_CODE_TYPE());
                return DRVIF_DPRX_GET_AUDIO_CODE_TYPE();
            }else{
                DebugMessageHDMITx("[HDMITX] DPRX Audio mute=[%d]@GetCodingType\n", (UINT32)DRVIF_DPRX_IS_AUDIO_MUTE());
            }
#endif
        }
    return _LPCM_ASP;//

}

//static UINT32 value_u32;
UINT8 ScalerStreamAudioGetSamplingFrequencyType(EnumOutputPort ucOutputPort)
{
#if IS_ENABLED(CONFIG_RTK_HDMI_RX) || IS_ENABLED(CONFIG_ENABLE_DPRX)
    UINT32 value_u32 = 0;
    if(g_scaler_display_info.input_src == 0){
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
        if(GET_H_AUDIO_FSM(0) == AUDIO_FSM_AUDIO_CHECK)
        {
            DebugMessageHDMITx("[HDMITX] audio_sample_freq=%d\n", (UINT32)newbase_hdmi_get_audio_sample_freq(0));
            value_u32 = newbase_hdmi_get_audio_sample_freq(0);

        }else{
            DebugMessageHDMITx("[HDMITX] Audio FSM=[%d]@GetSamplingFrequency\n", (UINT32)GET_H_AUDIO_FSM(0));
            return _AUDIO_FREQ_NO_AUDIO;//
        }
#endif
    }else{
#if defined(CONFIG_ENABLE_DPRX)
            if(DRVIF_DPRX_IS_AUDIO_MUTE() == 0){
                DebugMessageHDMITx("[HDMITX] DPRX audio_sample_freq=%d\n", (UINT32)DRVIF_DPRX_GET_AUDIO_SAMPLE_FREQUENCY());
                value_u32 = DRVIF_DPRX_GET_AUDIO_SAMPLE_FREQUENCY();
            }else{
                DebugMessageHDMITx("[HDMITX] DPRX Audio mute=[%d]@GetSamplingFrequency\n", (UINT32)DRVIF_DPRX_IS_AUDIO_MUTE());
                return _AUDIO_FREQ_NO_AUDIO;//
            }
#endif
   }
    switch(value_u32)
    {
        case 32000:
            return _AUDIO_FREQ_32K;
        case 64000:
            return _AUDIO_FREQ_64K;
        case 128000:
            return _AUDIO_FREQ_128K;
        case 44100:
            return _AUDIO_FREQ_44_1K;
        case 88200:
            return _AUDIO_FREQ_88_2K;
        case 176400:
            return _AUDIO_FREQ_176_4K;
        case 48000:
            return _AUDIO_FREQ_48K;
        case 96000:
            return _AUDIO_FREQ_96K;
        case 192000:
            return _AUDIO_FREQ_192K;
        case 256000:
            return _AUDIO_FREQ_256K;
        case 352800:
            return _AUDIO_FREQ_352_8K;
        case 384000:
            return _AUDIO_FREQ_384K;
        case 512000:
            return _AUDIO_FREQ_512K;
        case 705600:
            return _AUDIO_FREQ_705_6K;
        case 768000:
            return _AUDIO_FREQ_768K;
        default:
            DebugMessageHDMITx("[HDMITX] get audio_sample_freq=[%d] fail\n", (UINT32)value_u32);
            break;
    }
#endif
    return _AUDIO_FREQ_NO_AUDIO;//

}


UINT8 ScalerStreamGetAudioMuteFlag(EnumOutputPort ucOutputPort)
{
if(g_scaler_display_info.input_src == 0){
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
    if(GET_H_AUDIO_FSM(0) == AUDIO_FSM_AUDIO_CHECK){
        DebugMessageHDMITx("[HDMITX] AUDIO_MUTE=%d\n", (UINT32)IS_HDMIR_AUDIO_MUTE());
        return IS_HDMIR_AUDIO_MUTE();
    }else{
        DebugMessageHDMITx("[HDMITX] Audio FSM=[%d]@GetAudioMuteFlag\n", (UINT32)GET_H_AUDIO_FSM(0));
    }
#endif
    }else{
#if defined(CONFIG_ENABLE_DPRX)
        DebugMessageHDMITx("[HDMITX] DPRX AUDIO_MUTE=%d\n", (UINT32)DRVIF_DPRX_IS_AUDIO_MUTE());
        return DRVIF_DPRX_IS_AUDIO_MUTE();
#endif
    }
    return _TRUE;//
}
#endif
//#endif // #ifdef H5X_HDMITX_FIXME // [ZEBU] DTG force freerun in ZEBU platform
#endif // End of #if((_DP_TX_SUPPORT == _ON) || (_HDMI_TX_SUPPORT == _ON) || (_VGA_TX_SUPPORT == _ON))

