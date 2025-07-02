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
// ID Code      : ScalerHdmi21MacTx0.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>

#include <rbus/dsce_reg.h>
#include "../include/ScalerFunctionInclude.h"
#include "../include/HdmiMacTx/ScalerHdmiMacTx.h"
#include "../include/HdmiPhyTx/ScalerHdmiPhyTxInclude.h"
//#include "../include/HdmiMacTx/ScalerHdmiMacTxInclude.h"
//#include "../include/HdmiMacTx0/ScalerHdmiMacTx0Include.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0.h"
//#include "../include/HdmiMacTx0/ScalerHdmiMacTx0FillPacket.h"
#include "../include/Hdmi21MacTx/ScalerHdmi21MacTxInclude.h"
#include "../include/Hdmi21MacTx0/ScalerHdmi21MacTx0Include.h"
#include "../include/Hdmi21MacTx0/ScalerHdmi21MacTx0.h"
#include "../include/ScalerTx.h"
#include "../include/hdmitx_phy_struct_def.h"
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_driver.h>
#include "../include/hdmitx_dsc_pps_struct_def.h"

#if(_HDMI21_MAC_TX0_SUPPORT == _ON)
//****************************************************************************
// CODE TABLES
//****************************************************************************
#define ScalerHdmiMacTxPxMapping(x) (_P0_OUTPUT_PORT)
#define ScalerHdmi21MacTxPxTxMapping(x) (_HW_P0_HDMI21_MAC_TX_MAPPING)
#define ScalerHdmiMacTxSetStreamSource(x, y) SET_HDMI_MAC_TX0_STREAM_SOURCE(y)
#define ScalerColorStream422to420Support(x) (_SUPPORT_422_TO_420)
#define ScalerColorStream444to420Support(x) (_SUPPORT_444_TO_420)
#define ScalerColorStream422to444Support(x) (_SUPPORT_422_TO_444)
#define ScalerColorStream422To420(x, y)
#define ScalerColorStream444To420(x, y)
#define ScalerColorStreamRgb2Yuv(x, y, z)
#define ScalerHdmiTxSetHDRFlag(x) (ucTxHDRFlag = x)
#define Scaler_HDMITX_HDCP_get_DisableHdcp22Rc() (bDisableHdcp22Rc)
//#define Scaler_HDMITX_DispGetTxInputPathFromScalerPQ() (txDtgInputPath)
#define WAIT_TIMEOUT (90*100)

BOOLEAN g_bHdmi21MacTx0SWPktDFF0Used;
BOOLEAN g_bHdmi21MacTx0SWPktDFF1Used;

static StructDataFlowMeteringInfo stTx0DataFlowMeteringInfo = {0};

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
hdmi_tx_timing_unvalid_st unvalid_480i_576i[4]={
 {2, 3,  5}, //720*480i
  {1, 3,  4}, //720*576i
   {0, 2,  2}, //1440*480i
    {2, 2,  4}, //1440*576i
};
extern UINT8 bHD21_needUnvalid;
extern UINT8 needUnvalid_index;

#define GET_HDMI_HD21_needUnvalid()          (bHD21_needUnvalid)

UINT32 temp_value32;// = 0;
UINT16 temp_value16;// = 0;

extern UINT16 lineCnt, start, end;
//static UINT8 wait_zero;
extern UINT32 lineCntAddr;

#if(_AUDIO_SUPPORT == _ON)

#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
BOOLEAN g_bHdmi21MacTx0CtsCodeCheckEvent;
BOOLEAN g_bHdmi21MacTx0CtsCodeFwMode;
#endif

#endif // #if(_AUDIO_SUPPORT == _ON)
extern unsigned long long HDMI_TX_I2C_BUSID;
extern UINT8 ucTxHDRFlag;
//extern UINT8 ucVTEMpacketEn;
//extern UINT8 ucVRRFlagEn;
extern UINT8 bDisableHdcp22Rc;
extern UINT8 AudioFreqType;
extern StructHdmitxDisplayInfo g_scaler_display_info;
extern UINT8 need_sw_avi_packet;
#ifdef _DEBUG_HALT_TX_WHEN_FIFO_ERROR // HALT TX when TX FIFO error
extern UINT8 TxHaltFlag;
#endif
extern UINT8 bHD_480i_576i;
extern UINT8 bHD21_720_480i;
#define GET_HDMI_HD21_720_480i()                (bHD21_720_480i)
#define GET_HDMI_HD_480i_576i()                   (bHD_480i_576i)
//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
extern UINT8 ScalerStreamGetAudioMuteFlag(EnumOutputPort ucOutputPort);
//extern UINT8 HDMITX_wait_for_tgLineCount(UINT32 start, UINT32 end, UINT8 wait_zero);
extern UINT16 ScalerDpStreamGetElement(EnumOutputPort ucOutputPort, EnumMultiStreamElement enElement);
extern UINT16 Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_INFO infoList);
extern void ScalerHdmiMacTx0RepeaterSetPktHdrBypass(UINT8 enable);
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
extern void drvif_hdmi_pkt_bypass_to_hdmitx_en(unsigned char port, unsigned char enable);
#endif
extern void OutputConfigurationDscSrcSel(UINT8 mode);
extern void ScalerHdmiMacTx0CheckAviUpdate(UINT8 HdmiType);
extern void ScalerHdmiTxNCtsRegenFifoCheckSetting(void);
//--------------------------------------------------
// Description  : Hdmi 2p1 Video or GAP
// Input Value  : port
// Output Value : TRUE => Video ; FALSE => GAP
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTx0VideoMuxStatus(void)
{
    if(rtd_getbits(HDMITX21_MAC_HDMI21_FRL_19_reg, _BIT7) == 0x00)
    {
        return _TRUE;
    }
    else
    {
        return _FALSE;
    }
}


//--------------------------------------------------
// Description  : Hdmi 2p1 check Deep Color SRAM
// Input Value  : port
// Output Value : _STABLE or _UNSTABLE
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTx0CheckDPCSRAM(void)
{
    if(rtd_getbits(HDMITX21_MAC_HDMI21_DPC_1_reg, (_BIT7 | _BIT6)) == 0x00)
    {
        return _STABLE;
    }
    else
    {
        return _UNSTABLE;
    }
}

//--------------------------------------------------
// Description  : HDMI2.1 Data Flow Metering
// Input Value  : None
// Output Value : EnumHdmi21DataFlowMeteringResult
//--------------------------------------------------

#ifdef NOT_USED_NOW
void Cal_ulFreqTriByteAverageRate_ktriByte_ps(void)
{
	// 1.25 FreqTriByteAverageRate(kHz) = FreqPixelClockMax(kHz) * (TBactive + TBblank) / (Hactive + Blank)
	// max = (5940000 / 40) * (13500 * 12 / 8) = 3,007,125,000 < UINT32
	//ulFreqTriByteAverageRate_ktriByte_ps = (ulFreqPixelClockMax_kHz / 40) * (usTriByteActive + usTriBytebBlank) / (usHWidth + usHblank) * 40;
	temp_value32 = ulFreqPixelClockMax_kHz / 40;
	temp1_value32 = usTriByteActive + usTriBytebBlank;
	temp_value32 = temp_value32 * temp1_value32;
	temp1_value32 = usHWidth + usHblank;
	temp_value32 = temp_value32 / temp1_value32;
	ulFreqTriByteAverageRate_ktriByte_ps = temp_value32 * 40;

}

void Cal_ulTimeActiveRef_ns(void)
{
	// 1.26 TimeActiveRef = TimeLinePeriodMin(ns) * Hactive / (Hactive + Hblank)
	// min Line rate = 1s / 15.625kHz / 1.005 = 63681ns
	// max = 63681 * 10240 = 652,093,440 < UINT32
	//ulTimeActiveRef_ns = ulTimeLinePeriodMin_ns * usHWidth / (usHWidth + usHblank);
	ulTimeActiveRef_ns = ulTimeLinePeriodMin_ns;
	ulTimeActiveRef_ns = ulTimeActiveRef_ns * usHWidth;
	temp_value32 = usHWidth + usHblank;
	ulTimeActiveRef_ns = ulTimeActiveRef_ns / temp_value32;

}

void Cal_ulTimeBlankRef_ns(void)
{

	// 1.27 TimeBlankRef = TimeLinePeriodMin(ns) * Hblank / (Hactive + Hblank)
	// max = 63681 * 3320 < UINT32
	//ulTimeBlankRef_ns = ulTimeLinePeriodMin_ns * usHblank / (usHWidth + usHblank);
	ulTimeBlankRef_ns = ulTimeLinePeriodMin_ns;
	ulTimeBlankRef_ns = ulTimeBlankRef_ns * usHblank;
	temp_value32 = usHWidth + usHblank;
	ulTimeBlankRef_ns = ulTimeBlankRef_ns / temp_value32;
}


void Cal_ulTimeActiveMin_ns(void)
{

	// 1.28 TimeActiveMin = (3 * TBactive / 2) / (FrlLanes * RateFrlCharMin(k-char-ps) * (1 - OverheadMax))
	// max = 3 * (10240 * 12/8) /2 * 100000 = 2,304,000,000 < UINT32
	//ulTimeActiveMin_ns = (3UL * usTriByteActive / 2 * 100000) / ucFrlLanes / (ulRateFrlCharMin_kchar_ps / 10) * 100000 / (100000 - usOverheadMax);
	ulTimeActiveMin_ns = usTriByteActive * 3;
	ulTimeActiveMin_ns = ulTimeActiveMin_ns / 2;
	ulTimeActiveMin_ns = ulTimeActiveMin_ns * 100000;
	ulTimeActiveMin_ns = ulTimeActiveMin_ns /ucFrlLanes;
	ulTimeActiveMin_ns = ulTimeActiveMin_ns * 10;
	ulTimeActiveMin_ns = ulTimeActiveMin_ns / ulRateFrlCharMin_kchar_ps;
	ulTimeActiveMin_ns = ulTimeActiveMin_ns * 100000;
	temp_value32 = 100000 - usOverheadMax;
	ulTimeActiveMin_ns = ulTimeActiveMin_ns / temp_value32;
}

void Cal_ulTimeBlankMin_ns(void)
{

	// 1.29 TimeBlankMin(ns) = TBblank / (FrlLanes * RateFrlCharMin * (1 - OverheadMax))
	// max = 3320 * 1000000 = 3,320,000,000 < UINT32
	//ulTimeBlankMin_ns = (UINT32)usTriBytebBlank * 1000000 / ucFrlLanes / ulRateFrlCharMin_kchar_ps * 100000 / (100000 - usOverheadMax);
	ulTimeBlankMin_ns = usTriBytebBlank * 1000000;
	ulTimeBlankMin_ns = ulTimeBlankMin_ns / ucFrlLanes;
	ulTimeBlankMin_ns = ulTimeBlankMin_ns / ulRateFrlCharMin_kchar_ps;
	ulTimeBlankMin_ns = ulTimeBlankMin_ns * 100000;
	temp_value32 = 100000 - usOverheadMax;
	ulTimeBlankMin_ns = ulTimeBlankMin_ns / temp_value32;

}
#endif
EnumHdmi21DataFlowMeteringResult ScalerHdmi21MacTx0DataFlowMetering(void)
{
    StructDataFlowMeteringInfo  *pstDataFlowMeteringInfo = &stTx0DataFlowMeteringInfo;

    // Get Input parameter
    UINT32 ulFreqPixelClock_kHz = pstDataFlowMeteringInfo->ulFreqPixelClock_kHz;
    UINT16 usHWidth = pstDataFlowMeteringInfo->usHWidth;
    UINT16 usHblank = pstDataFlowMeteringInfo->usHblank;
    UINT8 ucbpc = pstDataFlowMeteringInfo->ucbpc; // bits
    UINT8 ucCalculatingFrlRate = pstDataFlowMeteringInfo->ucCurrentFrlRate;
    UINT32 ulRateFrlBitRate_kbps = 0;
    UINT8 ucAudioChannels = pstDataFlowMeteringInfo->ucAudioChannels;
    UINT32 ulRateAudio_Hz = pstDataFlowMeteringInfo->ulRateAudio_Hz;
    UINT8 ucAudioCodeType = pstDataFlowMeteringInfo->ucAudioCodeType;
    UINT8 ucFrlLanes = 0;

    // Calculate parameter
    UINT16 usOverheadMax = 0; // 2.136% * 1000
    UINT32 ulFreqPixelClockMax_kHz = 0;
    UINT32 ulTimeLinePeriodMin_ns = 0;
    UINT32 ulRateFrlCharMin_kchar_ps = 0;
    UINT32 ulCharTotalPerLine = 0;
    UINT16 usAP = 0;
    UINT32 ulRateAudioMax_Hz = 0;
    UINT32 ulPacketAudioPerLine = 0;
    UINT16 usHblankRequireMin = 0;
    UINT16 usCharFrlRcSavings = 0;
    UINT16 usTriByteActive = 0;
    UINT16 usTriBytebBlank = 0;
    UINT32 ulFreqTriByteAverageRate_ktriByte_ps = 0;
    UINT32 ulTimeActiveRef_ns = 0;
    UINT32 ulTimeBlankRef_ns = 0;
    UINT32 ulTimeActiveMin_ns = 0;
    UINT32 ulTimeBlankMin_ns = 0;
    UINT16 usTriByteborrow = 0;
    UINT16 usUtilization = 0;
#if(_HDMI_TX_DSC_ENCODER_SUPPORT == _ON)
    // Compressed Data flow Metering
    UINT16 usDscbppTarget_16 = 0;
    UINT8 ucDscSliceNum = 0;
    UINT16 usDscSliceWidth = 0;
    UINT32 ulDscCharAvailablePerLine = 0;
    UINT32 ulDscCharActiveAvailablePerLine = 0;
    UINT32 ulDscCharBlankAvailablePerLine = 0;
    UINT32 ulDscByteTarget = 0;
    UINT32 ulDscTriByteHCactiveTarget = 0;
    UINT32 ulDscTriByteHCblankTargetEst1 = 0;
    UINT32 ulDscTriByteHCblankTargetEst2 = 0;
    UINT32 ulDscTriByteHCblankTarget = 0;
    UINT32 ulDscTimeActiveTarget_ns = 0;
    UINT32 ulDscTimeBlankTarget_ns = 0;
    UINT32 ulDscTimeBorrow_ns = 0;
#endif
    // Set output parameter
    pstDataFlowMeteringInfo->ucCurrentFrlRateMaxAudioPacketNum = 0;
    pstDataFlowMeteringInfo->ucMinFrlRateMaxAudioPacketNum = 0;
    pstDataFlowMeteringInfo->ucCurrentFrlRateResult = 0;
    pstDataFlowMeteringInfo->ucMinFrlRateResult = 0;
    pstDataFlowMeteringInfo->ucMinFrlRate = 0;

	 // 1.01 ~ 1.06 Calculate Overhead
    if(ucFrlLanes == 3) // 3 Lanes mode
    {
        usOverheadMax = 2136; // 2.136% * 1000
    }
    else
    {
        usOverheadMax = 2184; // 2.184% * 1000
    }

    // Check has Audio or not
    switch(ulRateAudio_Hz)
    {
        case _AUDIO_FREQ_32K:

            ulRateAudio_Hz = 32000UL;
            break;

        case _AUDIO_FREQ_44_1K:

            ulRateAudio_Hz = 44100UL;
            break;

        case _AUDIO_FREQ_48K:

            ulRateAudio_Hz = 48000UL;
            break;

        case _AUDIO_FREQ_64K:

            ulRateAudio_Hz = 64000UL;
            break;

        case _AUDIO_FREQ_88_2K:

            ulRateAudio_Hz = 88200UL;
            break;

        case _AUDIO_FREQ_96K:

            ulRateAudio_Hz = 96000UL;
            break;

        case _AUDIO_FREQ_128K:

            ulRateAudio_Hz = 128000UL;
            break;

        case _AUDIO_FREQ_176_4K:

            ulRateAudio_Hz = 176400UL;
            break;

        case _AUDIO_FREQ_192K:

            ulRateAudio_Hz = 192000UL;
            break;

        case _AUDIO_FREQ_256K:

            ulRateAudio_Hz = 256000UL;
            break;

        case _AUDIO_FREQ_352_8K:

            ulRateAudio_Hz = 352800UL;
            break;

        case _AUDIO_FREQ_384K:

            ulRateAudio_Hz = 384000UL;
            break;

        case _AUDIO_FREQ_512K:

            ulRateAudio_Hz = 512000UL;
            break;

        case _AUDIO_FREQ_705_6K:

            ulRateAudio_Hz = 705600UL;
            break;

        case _AUDIO_FREQ_768K:

            ulRateAudio_Hz = 768000UL;
            break;

        default:
        case _AUDIO_FREQ_NO_AUDIO:
            ucAudioChannels = 0;
            ulRateAudio_Hz = 0;
            usAP = 0;

            break;
    }

    if(ulRateAudio_Hz != 0)
    {
        // Get Audio Frequency

        // 1.12(2.15) Number of Audio packet required
        if(ucAudioCodeType == 0)
        {
            if(ucAudioChannels <= 2)
            {
                usAP = 25; // 0.25 * 100
            }
            else
            {
                usAP = 100; // 1 * 100
            }
        }
        else if(ucAudioCodeType == 0x01)
        {
            usAP = 100;
        }
        else if(ucAudioCodeType == 0x02)
        {
            if(ucAudioChannels <= 12)
            {
                usAP = 200; // 2 * 100
            }
            else if(ucAudioChannels <= 24)
            {
                usAP = 300; // 3 * 100
            }
            else
            {
                usAP = 400; // 4 * 100
            }
        }
    }

    // VESA Timing
    if(pstDataFlowMeteringInfo->b1DSCMode == _DOWN_STREAM_VESA)
    {
        // Calculate VESA Data Flow Metering
        while(ucCalculatingFrlRate >= 1)
        {
        	//ulRateFrlBitRate_kbps = (ucCalculatingFrlRate == 6) ? 12000000 : ((ucCalculatingFrlRate == 5) ? 10000000 :
             //                                                                 ((ucCalculatingFrlRate == 4) ? 8000000 : ((ucCalculatingFrlRate == 3) ? 6000000 :
             //                                                                                                          ((ucCalculatingFrlRate == 2) ? 6000000 : ((ucCalculatingFrlRate == 1) ? 3000000 : 0)))));
        	switch(ucCalculatingFrlRate)
        	{
        		case 6:
				ulRateFrlBitRate_kbps = 12000000;
				break;
			case 5:
				ulRateFrlBitRate_kbps = 10000000;
				break;
			case 4:
				ulRateFrlBitRate_kbps = 8000000;
				break;
			case 3:
				ulRateFrlBitRate_kbps = 6000000;
				break;
			case 2:
				ulRateFrlBitRate_kbps = 6000000;
				break;
			case 1:
				ulRateFrlBitRate_kbps = 3000000;
				break;
			default:
				ulRateFrlBitRate_kbps = 0;
				break;
		}



		// 1.07 Pixel clock Rate Max(kHz) = FreqPixelClock(kHz) * 1.005
		// max = (5940000 / 10) * 1005 = 596,970,000 < UINT32
		ulFreqPixelClockMax_kHz = (ulFreqPixelClock_kHz / 10) * 1005 / 100;

		// 1.08 Min One Line Period(ns) = 1000000 * (Hactive + Hblank) / FreqPixelClockMax(kHz)
		// max = 100000 * 13500 = 1,350,000,000 < UINT32
		ulTimeLinePeriodMin_ns = 100000UL * (usHWidth + usHblank) / (ulFreqPixelClockMax_kHz / 10);

		// 1.09 Min FRL Bit Rate and 1.10 Character Rate(k-char-ps) = RateFrlBitRate(kbps) * (1 - 300ppm) / 18
		ulRateFrlCharMin_kchar_ps = (ulRateFrlBitRate_kbps / 1000000 * (1000000 - 300)) / 18;

		// 1.11 Min FRL Chars number per Line(char-per-line) = TimeLinePeriodMin(ns) * (RateFrlCharMin(k-char-ps) * FrlLanes) / 1000000
		// min Line rate = 1s / 15.625kHz / 1.005 = 63681ns
		// max Rchar rate = 12000000 * 999700 / 1000000 / 18 = 666466
		// max = 63681 * (666466 / 50) * 4 = 3,395,216,196 < UINT32
		ucFrlLanes = ((ucCalculatingFrlRate < 3) ? 3 : 4);
		ulCharTotalPerLine = (ulTimeLinePeriodMin_ns * (ulRateFrlCharMin_kchar_ps / 50) * ucFrlLanes / 1000 / 20);


		// 1.12 Audio Calculate 1 time only

		// 1.13 Max Audio Packet Rate(Hz) = RateAudio(Hz) * (1 + 1000ppm) * AP
		// max = 768000 * 1001 = 768,768,000 < UINT32
		ulRateAudioMax_Hz = (ulRateAudio_Hz * (1000 + 1) / 1000) * usAP / 100;
		// 1.14 Audio Packet per Line(packets/1000) = RateAudioMax(Hz) * TimeLinePeriodMin(ns) / 1000000
		// 1.15 Audio Packet per Line(packets) = CEILING(PacketAudioPerLine/1000)
		ulPacketAudioPerLine = ulRateAudioMax_Hz * ulTimeLinePeriodMin_ns / 1000 / 1000;
		ulPacketAudioPerLine = (ulPacketAudioPerLine % 1000 == 0) ? (ulPacketAudioPerLine / 1000) : (ulPacketAudioPerLine / 1000 + 1);


		// 1.16 Min Hblank required(cycles) = 64 + 32 * PacketAudioPerLine
		usHblankRequireMin = 64 + 32 * ulPacketAudioPerLine;

		// 1.17 Blank Free after FRL RC compression(chars) = Blank * KCD / K420 - 32 * (1 + PacketAudioPerLine) - 7
		// 4:4:4 KCD = bpc/8, K420 = 1
		// 4:2:2 KCD = 1, K420 = 1
		// 4:2:0 KCD = bpc/8, K420 = 2
		// 1.18 Char Margin(chars) = 4
		// 1.19 usCharFrlRcSavings(chars) = 7 * Blank Free after FRL RC compression / 8 - Char Margin
		if(pstDataFlowMeteringInfo->ucColorSpace == _COLOR_SPACE_YCBCR420)
		{
			// usCharFrlRcSavings = 7 * (Blank * bpc / 8 / 2 - 32 * (1 + PacketAudioPerLine) - 7) / 8 - 4
			usCharFrlRcSavings = (UINT16)(7UL * ((usHblank * ucbpc / 8 / 2) - 32 * (1 + ulPacketAudioPerLine) - 7) / 8) - 4;
		}
		else if(pstDataFlowMeteringInfo->ucColorSpace == _COLOR_SPACE_YCBCR422)
		{
			// usCharFrlRcSavings = 7 * (Blank * 1 / 1 - 32 * (1 + PacketAudioPerLine) - 7) / 8 - 4
			usCharFrlRcSavings = (UINT16)(7UL * (usHblank - 32 * (1 + ulPacketAudioPerLine) - 7) / 8) - 4;
		}
		else
		{
			// usCharFrlRcSavings = 7 * (Blank * bpc / 8 / 1 - 32 * (1 + PacketAudioPerLine) - 7) / 8 - 4
			usCharFrlRcSavings = (UINT16)(7UL * ((usHblank * ucbpc / 8) - 32 * (1 + ulPacketAudioPerLine) - 7) / 8) - 4;
		}

		// 1.20 bpp = 24 * KCD / K420
		// 1.21 Bytes-per-line = bpp * Hactive / 8
		// 1.22 Tri-Byte = CEILING(Bytes-per-line / 3)
		if(pstDataFlowMeteringInfo->ucColorSpace == _COLOR_SPACE_YCBCR420)
		{
			// Tri-Byte = 24 * bpc / 8 / 2 * Hactive / 8 / 3
			usTriByteActive = (UINT16)((24UL * ucbpc / 8 / 2) * usHWidth / 8) / 3;
		}
		else if(pstDataFlowMeteringInfo->ucColorSpace == _COLOR_SPACE_YCBCR422)
		{
			// Tri-Byte = 24 * 1 / 1 * Hactive / 8 / 3
			usTriByteActive = (UINT16)((24UL * usHWidth / 8) / 3);

		}
		else
		{
			// Tri-Byte = 24 * bpc / 8 / 1 * Hactive / 8 / 3
			usTriByteActive = (UINT16)((24UL * ucbpc / 8) * usHWidth / 8) / 3;
		}

		// 1.23 Tri-Bytes require for Blank = CEILING(Hblank * KCD / K420)
		if(pstDataFlowMeteringInfo->ucColorSpace == _COLOR_SPACE_YCBCR420)
		{
			// Tri-Byte = Hblank * bpc / 8 / 2
			usTriBytebBlank = (UINT16)((UINT32)usHblank * ucbpc / 8 / 2);
		}
		else if(pstDataFlowMeteringInfo->ucColorSpace == _COLOR_SPACE_YCBCR422)
		{
			// Tri-Byte = Hblank * 1 / 1
			usTriBytebBlank = usHblank;
		}
		else
		{
			// Tri-Byte = Hblank * bpc / 8 / 1
			usTriBytebBlank = (UINT16)((UINT32)usHblank * ucbpc / 8);
		}


	     ///////////////////////////////////////////
            // 1.24 Check Audio can support or not   //
            ///////////////////////////////////////////
            if(ucCalculatingFrlRate == pstDataFlowMeteringInfo->ucCurrentFrlRate)
            {
                if(usHblankRequireMin > usTriBytebBlank)
                {
                    DebugMessageHDMI21Tx("[HDMI21_TX] AUDIO_FAIL in Current FRL Rate@%d\n", __LINE__);
                    pstDataFlowMeteringInfo->ucCurrentFrlRateResult = _HDMI21_DATA_FLOW_METERING_AUDIO_FAIL;
                    break;
                }
                else
                {
                    // Calculate max audio packet per line
                    pstDataFlowMeteringInfo->ucCurrentFrlRateMaxAudioPacketNum = usTriBytebBlank / 32;
                }
            }
            else
            {
                if(usHblankRequireMin > usTriBytebBlank)
                {
                    DebugMessageHDMI21Tx("AUDIO_FAIL in Rudece FRL Rate=%d\n", ucCalculatingFrlRate);
                    break;
                }
                else
                {
                    // Calculate max audio packet per line
                    pstDataFlowMeteringInfo->ucMinFrlRateMaxAudioPacketNum = usTriBytebBlank / 32;
                }
            }

		// 1.25 FreqTriByteAverageRate(kHz) = FreqPixelClockMax(kHz) * (TBactive + TBblank) / (Hactive + Blank)
		// max = (5940000 / 40) * (13500 * 12 / 8) = 3,007,125,000 < UINT32
		ulFreqTriByteAverageRate_ktriByte_ps = (ulFreqPixelClockMax_kHz / 40) * (usTriByteActive + usTriBytebBlank) / (usHWidth + usHblank) * 40;
		//Cal_ulFreqTriByteAverageRate_ktriByte_ps();

		// 1.26 TimeActiveRef = TimeLinePeriodMin(ns) * Hactive / (Hactive + Hblank)
		// min Line rate = 1s / 15.625kHz / 1.005 = 63681ns
		// max = 63681 * 10240 = 652,093,440 < UINT32
		ulTimeActiveRef_ns = ulTimeLinePeriodMin_ns * usHWidth / (usHWidth + usHblank);
		//Cal_ulTimeActiveRef_ns();

		// 1.27 TimeBlankRef = TimeLinePeriodMin(ns) * Hblank / (Hactive + Hblank)
		// max = 63681 * 3320 < UINT32
		ulTimeBlankRef_ns = ulTimeLinePeriodMin_ns * usHblank / (usHWidth + usHblank);
		//Cal_ulTimeBlankRef_ns();

		// 1.28 TimeActiveMin = (3 * TBactive / 2) / (FrlLanes * RateFrlCharMin(k-char-ps) * (1 - OverheadMax))
		// max = 3 * (10240 * 12/8) /2 * 100000 = 2,304,000,000 < UINT32
		ulTimeActiveMin_ns = (3UL * usTriByteActive / 2 * 100000) / ucFrlLanes / (ulRateFrlCharMin_kchar_ps / 10) * 100000 / (100000 - usOverheadMax);
		//Cal_ulTimeActiveMin_ns();

		// 1.29 TimeBlankMin(ns) = TBblank / (FrlLanes * RateFrlCharMin * (1 - OverheadMax))
		// max = 3320 * 1000000 = 3,320,000,000 < UINT32
		ulTimeBlankMin_ns = (UINT32)usTriBytebBlank * 1000000 / ucFrlLanes / ulRateFrlCharMin_kchar_ps * 100000 / (100000 - usOverheadMax);
		//Cal_ulTimeBlankMin_ns();
            DebugMessageHDMI21Tx("[HDMI21_TX] ulTimeBlankMin_ns@%d ulTimeBlankRef_ns@%d\n", ulTimeBlankMin_ns,ulTimeBlankRef_ns);
            /////////////////////////////////////
            // 1.30 Check Need Borrow or not   //
            /////////////////////////////////////
            if(ucCalculatingFrlRate == pstDataFlowMeteringInfo->ucCurrentFrlRate)
            {
                if(ulTimeActiveRef_ns < ulTimeActiveMin_ns)
                {
                	// 1.31 Timeborrow = TimeActiveMin - TimeActiveRef
		      // 1.32 TriByteborrow = Timeborrow * FreqTriByteAverageRate
		      usTriByteborrow = (UINT16)((ulTimeActiveMin_ns - ulTimeActiveRef_ns) * (ulFreqTriByteAverageRate_ktriByte_ps / 10) / 100000);


                    ////////////////////////////////////
                    // 1.33 Check Borrow over limit   //
                    ////////////////////////////////////
                    if(usTriByteborrow > _HDMI21_TX_FRL_BORROW_MAX)
                    {
                        DebugMessageHDMI21Tx("[HDMI21_TX] VIDEO_FAIL in current FRL Rate@%d\n", __LINE__);

                        pstDataFlowMeteringInfo->ucCurrentFrlRateResult = _HDMI21_DATA_FLOW_METERING_VIDEO_FAIL;
                        break;
                    }
                }
                else
                {
                    usTriByteborrow = 0;
                }
            }
            else
            {
                if(ulTimeActiveRef_ns < ulTimeActiveMin_ns)
                {
                	// 1.31 Timeborrow = TimeActiveMin - TimeActiveRef
		      // 1.32 TriByteborrow = Timeborrow * FreqTriByteAverageRate
		      usTriByteborrow = (UINT16)((ulTimeActiveMin_ns - ulTimeActiveRef_ns) * (ulFreqTriByteAverageRate_ktriByte_ps / 10) / 100000);


                    ////////////////////////////////////
                    // 1.33 Check Borrow over limit   //
                    ////////////////////////////////////
                    if(usTriByteborrow > _HDMI21_TX_FRL_BORROW_MAX)
                    {
                        DebugMessageHDMI21Tx("[HDMI21_TX] VIDEO_FAIL in reduce FRL Rate@%d\n", __LINE__);
                        break;
                    }
                }
                else
                {
                    usTriByteborrow = 0;
                }
            }

            // 1.34 ~ 1.35
		usUtilization = (UINT16)((UINT32)((3 * usTriByteActive / 2) + usTriBytebBlank - usCharFrlRcSavings) * 10000 / ulCharTotalPerLine);

            DebugMessageHDMI21Tx("usUtilization=%d\n", usUtilization);

            // 1.36 ~ 1.37
            if(ucCalculatingFrlRate == pstDataFlowMeteringInfo->ucCurrentFrlRate)
            {

                if((usUtilization + (usOverheadMax / 10)) <= 10000)
                {
                    if(usTriByteborrow != 0)
                    {
                        DebugMessageHDMI21Tx("[HDMI21_TX] PASS_BORROW in current FRL Rate=%d\n", usTriByteborrow);
                        pstDataFlowMeteringInfo->ucCurrentFrlRateResult = _HDMI21_DATA_FLOW_METERING_PASS_BORROW;
                        break;
                    }
                    else
                    {
                        DebugMessageHDMI21Tx("[HDMI21_TX] PASS_NO_BORROW in current FRL Rate@%d\n", __LINE__);
                        pstDataFlowMeteringInfo->ucCurrentFrlRateResult = _HDMI21_DATA_FLOW_METERING_PASS_NO_BORROW;

                        // Check can reduce FRL Rate or not
                        ucCalculatingFrlRate = ucCalculatingFrlRate - 1;
                    }
                }
                else
                {
                    DebugMessageHDMI21Tx("[HDMI21_TX] VIDEO_FAIL in current FRL Rate@%d\n", __LINE__);
                    pstDataFlowMeteringInfo->ucCurrentFrlRateResult = _HDMI21_DATA_FLOW_METERING_VIDEO_FAIL;
                    break;
                }
            }
            else
            {

                if((usUtilization + (usOverheadMax / 10)) <= 10000)
                {
                    pstDataFlowMeteringInfo->ucMinFrlRate = ucCalculatingFrlRate;

                    if(usTriByteborrow != 0)
                    {
                        DebugMessageHDMI21Tx("[HDMI21_TX] PASS_BORROW in Reduce FRL Rate=%d\n", usTriByteborrow);
                        pstDataFlowMeteringInfo->ucMinFrlRateResult = _HDMI21_DATA_FLOW_METERING_PASS_BORROW;
                        break;
                    }
                    else
                    {
                        DebugMessageHDMI21Tx("[HDMI21_TX] PASS_NO_BORROW in Reduce FRL Rate@%d\n", __LINE__);
                        pstDataFlowMeteringInfo->ucMinFrlRateResult = _HDMI21_DATA_FLOW_METERING_PASS_NO_BORROW;

                        // Check can reduce FRL Rate or not
                        ucCalculatingFrlRate = ucCalculatingFrlRate - 1;
                    }
                }
                else
                {
                    DebugMessageHDMI21Tx("VIDEO_FAIL in Reduce FRL Rate@%d\n", __LINE__);
                    break;
                }
            }
        }
    }
#if(_HDMI_TX_DSC_ENCODER_SUPPORT == _ON)
    // DSC format
    else
    {
        //////////////////
        // Can DO DSC ? //
        //////////////////
        if(ulFreqPixelClock_kHz <= 400000)
        {
            ulRateFrlBitRate_kbps = 3000000;

            ucFrlLanes = 3;

            // 1.07 Pixel clock Rate Max(kHz) = FreqPixelClock(kHz) * 1.005
            ulFreqPixelClockMax_kHz = (ulFreqPixelClock_kHz / 10) * 1005 / 100;

            // 1.08 Min One Line Period(ns) = 1000000 * (Hactive + Hblank) / FreqPixelClockMax(kHz)
            ulTimeLinePeriodMin_ns = 100000UL * (usHWidth + usHblank) / (ulFreqPixelClockMax_kHz / 10);

            // 1.09 Min FRL Bit Rate and 1.10 Character Rate(k-char-ps) = RateFrlBitRate(kbps) * (1 - 300ppm) / 18
            ulRateFrlCharMin_kchar_ps = (ulRateFrlBitRate_kbps / 1000000 * (1000000 - 300)) / 18;

            // 1.11 Min FRL Chars number per Line(char-per-line) = TimeLinePeriodMin(ns) * (RateFrlCharMin(k-char-ps) * FrlLanes) / 1000000
            ulCharTotalPerLine = (ulTimeLinePeriodMin_ns * (ulRateFrlCharMin_kchar_ps / 50) * ucFrlLanes / 1000 / 20);

            // 1.12 Audio Calculate 1 time only

            // 1.13 Max Audio Packet Rate(Hz) = RateAudio(Hz) * (1 + 1000ppm) * AP
            ulRateAudioMax_Hz = (ulRateAudio_Hz * (1000 + 1) / 1000) * usAP / 100;

            // 1.14 Audio Packet per Line(packets/1000) = RateAudioMax(Hz) * TimeLinePeriodMin(ns) / 1000000
            // 1.15 Audio Packet per Line(packets) = CEILING(PacketAudioPerLine/1000)
            ulPacketAudioPerLine = ulRateAudioMax_Hz * ulTimeLinePeriodMin_ns / 1000 / 1000;
            ulPacketAudioPerLine = (ulPacketAudioPerLine % 1000 == 0) ? (ulPacketAudioPerLine / 1000) : (ulPacketAudioPerLine / 1000 + 1);

            // 1.16 Min Hblank required(cycles) = 64 + 32 * PacketAudioPerLine
            usHblankRequireMin = 64 + 32 * ulPacketAudioPerLine;

            // 1.17 Blank Free after FRL RC compression(chars) = Blank * KCD / K420 - 32 * (1 + PacketAudioPerLine) - 7
            // 4:4:4 KCD = bpc/8, K420 = 1
            // 4:2:2 KCD = 1, K420 = 1
            // 4:2:0 KCD = bpc/8, K420 = 2
            // 1.18 Char Margin(chars) = 4
            // 1.19 usCharFrlRcSavings(chars) = 7 * Blank Free after FRL RC compression / 8 - Char Margin
            if(pstDataFlowMeteringInfo->ucColorSpace == _COLOR_SPACE_YCBCR420)
            {
                // usCharFrlRcSavings = 7 * (Blank * bpc / 8 / 2 - 32 * (1 + PacketAudioPerLine) - 7) / 8 - 4
                usCharFrlRcSavings = (UINT16)(7UL * ((usHblank * ucbpc / 8 / 2) - 32 * (1 + ulPacketAudioPerLine) - 7) / 8) - 4;
            }
            else if(pstDataFlowMeteringInfo->ucColorSpace == _COLOR_SPACE_YCBCR422)
            {
                // usCharFrlRcSavings = 7 * (Blank * 1 / 1 - 32 * (1 + PacketAudioPerLine) - 7) / 8 - 4
                usCharFrlRcSavings = (UINT16)(7UL * (usHblank - 32 * (1 + ulPacketAudioPerLine) - 7) / 8) - 4;
            }
            else
            {
                // usCharFrlRcSavings = 7 * (Blank * bpc / 8 / 1 - 32 * (1 + PacketAudioPerLine) - 7) / 8 - 4
                usCharFrlRcSavings = (UINT16)(7UL * ((usHblank * ucbpc / 8) - 32 * (1 + ulPacketAudioPerLine) - 7) / 8) - 4;
            }

            // 1.20 bpp = 24 * KCD / K420
            // 1.21 Bytes-per-line = bpp * Hactive / 8
            // 1.22 Tri-Byte = CEILING(Bytes-per-line / 3)
            if(pstDataFlowMeteringInfo->ucColorSpace == _COLOR_SPACE_YCBCR420)
            {
                // Tri-Byte = 24 * bpc / 8 / 2 * Hactive / 8 / 3
                usTriByteActive = (UINT16)((24UL * ucbpc / 8 / 2) * usHWidth / 8) / 3;
            }
            else if(pstDataFlowMeteringInfo->ucColorSpace == _COLOR_SPACE_YCBCR422)
            {
                // Tri-Byte = 24 * 1 / 1 * Hactive / 8 / 3
                usTriByteActive = (UINT16)((24UL * usHWidth / 8) / 3);
            }
            else
            {
                // Tri-Byte = 24 * bpc / 8 / 1 * Hactive / 8 / 3
                usTriByteActive = (UINT16)((24UL * ucbpc / 8) * usHWidth / 8) / 3;
            }

            // 1.23 Tri-Bytes require for Blank = CEILING(Hblank * KCD / K420)
            if(pstDataFlowMeteringInfo->ucColorSpace == _COLOR_SPACE_YCBCR420)
            {
                // Tri-Byte = Hblank * bpc / 8 / 2
                usTriBytebBlank = (UINT16)((UINT32)usHblank * ucbpc / 8 / 2);
            }
            else if(pstDataFlowMeteringInfo->ucColorSpace == _COLOR_SPACE_YCBCR422)
            {
                // Tri-Byte = Hblank * 1 / 1
                usTriBytebBlank = usHblank;
            }
            else
            {
                // Tri-Byte = Hblank * bpc / 8 / 1
                usTriBytebBlank = (UINT16)((UINT32)usHblank * ucbpc / 8);
            }

            ///////////////////////////////////////////
            // 1.24 Check Audio can support or not   //
            ///////////////////////////////////////////

            // 1.25 FreqTriByteAverageRate(kHz) = FreqPixelClockMax(kHz) * (TBactive + TBblank) / (Hactive + Blank)
            ulFreqTriByteAverageRate_ktriByte_ps = (ulFreqPixelClockMax_kHz / 40) * (usTriByteActive + usTriBytebBlank) / (usHWidth + usHblank) * 40;

            // 1.26 TimeActiveRef = TimeLinePeriodMin(ns) * Hactive / (Hactive + Hblank)
            ulTimeActiveRef_ns = ulTimeLinePeriodMin_ns * usHWidth / (usHWidth + usHblank);

            // 1.27 TimeBlankRef = TimeLinePeriodMin(ns) * Hblank / (Hactive + Hblank)
            ulTimeBlankRef_ns = ulTimeLinePeriodMin_ns * usHblank / (usHWidth + usHblank);

            // 1.28 TimeActiveMin = (3 * TBactive / 2) / (FrlLanes * RateFrlCharMin(k-char-ps) * (1 - OverheadMax))
            ulTimeActiveMin_ns = (3UL * usTriByteActive / 2 * 100000) / ucFrlLanes / (ulRateFrlCharMin_kchar_ps / 10) * 100000 / (100000 - usOverheadMax);

            // 1.29 TimeBlankMin(ns) = TBblank / (FrlLanes * RateFrlCharMin * (1 - OverheadMax))
            ulTimeBlankMin_ns = (UINT32)usTriBytebBlank * 1000000 / ucFrlLanes / ulRateFrlCharMin_kchar_ps * 100000 / (100000 - usOverheadMax);


            /////////////////////////////////////
            // 1.30 Check Need Borrow or not   //
            /////////////////////////////////////

            // 1.34 ~ 1.35
            usUtilization = (UINT16)((UINT32)((3 * usTriByteActive / 2) + usTriBytebBlank - usCharFrlRcSavings) * 10000 / ulCharTotalPerLine);
            DebugMessageHDMI21Tx("[HDMI21_TX] DSC usUtilization=%d\n", usUtilization);

            // 1.36 ~ 1.37
            if((usUtilization + (usOverheadMax / 10)) <= 10000)
            {
                pstDataFlowMeteringInfo->ucCurrentFrlRateResult = _HDMI21_DATA_FLOW_METERING_DSC_SHALL_OFF;
                return _HDMI21_DATA_FLOW_METERING_DSC_SHALL_OFF;
            }
        }


        //////////////////////////////////////
        // Calculate DSC Data Flow Metering //
        //////////////////////////////////////
        ulRateFrlBitRate_kbps = (ucCalculatingFrlRate == 6) ? 12000000 : ((ucCalculatingFrlRate == 5) ? 10000000 :
                                                                          ((ucCalculatingFrlRate == 4) ? 8000000 : ((ucCalculatingFrlRate == 3) ? 6000000 :
                                                                                                                    ((ucCalculatingFrlRate == 2) ? 6000000 : ((ucCalculatingFrlRate == 1) ? 3000000 : 0)))));

        ucFrlLanes = ((ucCalculatingFrlRate < 3) ? 3 : 4);

        usDscbppTarget_16 = pstDataFlowMeteringInfo->usTargetBppx16;
        ucDscSliceNum = pstDataFlowMeteringInfo->b4SliceNum;
        usDscSliceWidth = pstDataFlowMeteringInfo->usSliceWidth;

        // 2.07 Pixel clock Rate Max(kHz) = FreqPixelClock(kHz) * 1.005
        // max = (5940000 / 10) * 1005 = 596,970,000 < DWORD
        ulFreqPixelClockMax_kHz = (ulFreqPixelClock_kHz / 10) * 1005 / 100;

        // 2.08 Min One Line Period(ns) = 1000000 * (Hactive + Hblank) / FreqPixelClockMax(kHz)
        // max = 100000 * 13500 = 1,350,000,000 < DWORD
        ulTimeLinePeriodMin_ns = 100000UL * (usHWidth + usHblank) / (ulFreqPixelClockMax_kHz / 10);

        // 2.09 Min FRL Bit Rate and 1.10 Character Rate(k-char-ps) = RateFrlBitRate(kbps) * (1 - 300ppm) / 18
        ulRateFrlCharMin_kchar_ps = (ulRateFrlBitRate_kbps / 1000000 * (1000000 - 300)) / 18;

        // 2.11 Min FRL Chars number per Line(char-per-line) = TimeLinePeriodMin(ns) * (RateFrlCharMin(k-char-ps) * FrlLanes) / 1000000
        // min Line rate = 1s / 15.625kHz / 1.005 = 63681ns
        // max Rchar rate = 12000000 * 999700 / 1000000 / 18 = 666466
        // max = 63681 * (666466 / 50) * 4 = 3,395,216,196 < DWORD
        ulCharTotalPerLine = (ulTimeLinePeriodMin_ns * (ulRateFrlCharMin_kchar_ps / 50) * ucFrlLanes / 1000 / 20);

        // 2.12 avaiable characters
        // max = 97864 * (169760 / 10) = 1,661,339,264 < DWORD
        ulDscCharAvailablePerLine = (UINT32)(100000 - usOverheadMax) * (ulCharTotalPerLine / 10) / 10000;

        // 2.13 available characters for Active
        // max = 166133 * 10240 = 1,701,201,920 < DWORD
        ulDscCharActiveAvailablePerLine = ulDscCharAvailablePerLine * usHWidth / (usHWidth + usHblank);

        // 2.14 available characters for Blank
        // max = (1701201920 / (10240+80)) * 3320 = 547,285,400 < DWORD
        ulDscCharBlankAvailablePerLine = (UINT16)((UINT32)ulDscCharAvailablePerLine * usHblank / (usHWidth + usHblank));
        DebugMessageHDMI21Tx("[HDMI21_TX] usUtilization=%d %d\n", ulDscCharActiveAvailablePerLine,ulDscCharBlankAvailablePerLine);
        // 2.15 same as 1.12

        // 2.16 Max Audio Packet Rate(Hz) = RateAudio(Hz) * (1 + 1000ppm) * AP
        // max = 768000 * 1001 = 768,768,000 < DWORD
        ulRateAudioMax_Hz = (ulRateAudio_Hz * (1000 + 1) / 1000) * usAP / 100;

        // 2.17 Audio Packet per Line(packets/1000) = RateAudioMax(Hz) * TimeLinePeriodMin(ns) / 1000000
        // 2.18 Audio Packet per Line(packets) = CEILING(PacketAudioPerLine/1000)
        // max = ((768768*4)/1000) * 540000 = 1,660,538,880 < DWORD
        ulPacketAudioPerLine = (ulRateAudioMax_Hz / 1000) * ulTimeLinePeriodMin_ns / 1000;
        ulPacketAudioPerLine = (ulPacketAudioPerLine % 1000 == 0) ? (ulPacketAudioPerLine / 1000) : (ulPacketAudioPerLine / 1000 + 1);

        // 2.19 Min Hblank required(cycles) = 64 + 32 * PacketAudioPerLine
        usHblankRequireMin = 64 + 32 * ulPacketAudioPerLine;

        // 2.20 Set bppTarget
        // 2.21 usByteTarget(Byte) = SliceNum * CEILING(bppTarget * SliceWidth /8)
        // max = 48 * 2560 / 8 = 15360
        ulDscByteTarget = (((UINT32)usDscbppTarget_16 * usDscSliceWidth % (8 * 16)) == 0) ? (UINT16)((UINT32)usDscbppTarget_16 * usDscSliceWidth / (8 * 16)) : ((UINT16)((UINT32)usDscbppTarget_16 * usDscSliceWidth / (8 * 16)) + 1);

        // max = 8 * 15360 =122880
        ulDscByteTarget = (UINT32)ucDscSliceNum * ulDscByteTarget;

        // 2.22 usTriByteHCactiveTarget(Tri-Byte) = CEILING(usByteTarget /3)
        // max = 122880/3=40960
        ulDscTriByteHCactiveTarget = ((ulDscByteTarget % 3) == 0) ? (ulDscByteTarget / 3) : ((ulDscByteTarget / 3) + 1);

        // 2.23 ideal HCBlank Tri-Bytes
        // max = 40960 * 3320 = 135,987,200 < DWORD
        ulDscTriByteHCblankTargetEst1 = ((ulDscTriByteHCactiveTarget * usHblank % usHWidth) == 0) ? (ulDscTriByteHCactiveTarget * usHblank / usHWidth) : ((ulDscTriByteHCactiveTarget * usHblank / usHWidth) + 1);

        // 2.24 HCblank Min. required
        ulDscTriByteHCblankTargetEst2 = (ulDscTriByteHCblankTargetEst1 > (UINT32)usHblankRequireMin) ? ulDscTriByteHCblankTargetEst1 : (UINT32)usHblankRequireMin;

        // 2.25 HCblank Target
        ulDscTriByteHCblankTarget = (ulDscTriByteHCblankTargetEst2 > (ulDscCharAvailablePerLine - 3 * ulDscTriByteHCactiveTarget / 2)) ? (ulDscCharAvailablePerLine - 3 * ulDscTriByteHCactiveTarget / 2) : ulDscTriByteHCblankTargetEst2;

        ulDscTriByteHCblankTarget = 4 * (ulDscTriByteHCblankTarget / 4);


        ///////////////////////////////////////////
        // 2.26 Check Audio can support or not   //
        ///////////////////////////////////////////
        if(usHblankRequireMin > ulDscTriByteHCblankTarget)
        {
            DebugMessageHDMI21Tx("[HDMI21_TX] AUDIO_FAIL in Current DSC FRL Rate@%d\n", __LINE__);
            pstDataFlowMeteringInfo->ucCurrentFrlRateResult = _HDMI21_DATA_FLOW_METERING_AUDIO_FAIL;

            return _HDMI21_DATA_FLOW_METERING_AUDIO_FAIL;
        }

        // 2.27 Average TriBytes Rate(kHz)
        // max = (2970000 / 100) * (40960 + 1000) = 1,246,212,000 < DWORD
        ulFreqTriByteAverageRate_ktriByte_ps = (ulFreqPixelClockMax_kHz / 100) * (ulDscTriByteHCactiveTarget + ulDscTriByteHCblankTarget) / (usHWidth + usHblank) * 100;

        // 2.28 Time Active Ref(ns)
        // max = 63681 * 10200 < DWORD
        ulTimeActiveRef_ns = ulTimeLinePeriodMin_ns * usHWidth / (usHWidth + usHblank);

        // 2.29 Time Blank Ref(ns)
        ulTimeBlankRef_ns = ulTimeLinePeriodMin_ns * usHblank / (usHWidth + usHblank);

        // 2.30 Time Active Target(ns)
        // max = 40960 * 100000 < DWORD
        // max = 4 * (666466/50) * ((100000 - 2136)/3) < DWORD
        ulDscTimeActiveTarget_ns = ((ulDscTriByteHCactiveTarget * 100000 / (ulFreqTriByteAverageRate_ktriByte_ps / 10)) > (ulDscTriByteHCactiveTarget * 100000 / ((UINT32)ucFrlLanes * (ulRateFrlCharMin_kchar_ps / 50) * ((100000 - usOverheadMax) / 3) / 10000))) ?
                                   (ulDscTriByteHCactiveTarget * 100000 / (ulFreqTriByteAverageRate_ktriByte_ps / 10)) : (ulDscTriByteHCactiveTarget * 100000 / ((UINT32)ucFrlLanes * (ulRateFrlCharMin_kchar_ps / 50) * ((100000 - usOverheadMax) / 3) / 10000));

        // 2.31 Time Blank Target(ns)
        ulDscTimeBlankTarget_ns = ulTimeLinePeriodMin_ns - ulDscTimeActiveTarget_ns;

        // 2.32
        if((ulTimeActiveRef_ns == ulDscTimeActiveTarget_ns) && (ulTimeBlankRef_ns == ulDscTimeBlankTarget_ns))
        {
            ulDscTimeBorrow_ns = 0;
        }
        // 2.33
        else if(ulTimeActiveRef_ns < ulDscTimeActiveTarget_ns)
        {
            ulDscTimeBorrow_ns = (ulDscTimeActiveTarget_ns - ulTimeActiveRef_ns);
        }
        // 2.34
        else if(ulTimeBlankRef_ns < ulDscTimeBlankTarget_ns)
        {
            ulDscTimeBorrow_ns = (ulDscTimeBlankTarget_ns - ulTimeBlankRef_ns);
        }

        // 2.35
        usTriByteborrow = (UINT16)((ulDscTimeBorrow_ns / 1000) * (ulFreqTriByteAverageRate_ktriByte_ps / 1000));

        // 2.36
        if(usTriByteborrow <= _HDMI21_TX_FRL_BORROW_MAX)
        {
            // 2.37~2.38
            usUtilization = (UINT16)(((3 * ulDscTriByteHCactiveTarget / 2) + ulDscTriByteHCblankTarget) * 10000 / ulCharTotalPerLine);
            DebugMessageHDMI21Tx("[HDMI21_TX] usUtilization=%d\n", usUtilization);

            // 2.39
            if((usUtilization + (usOverheadMax / 10)) <= 10000)
            {
                if(usTriByteborrow != 0)
                {
                    DebugMessageHDMI21Tx("[HDMI21_TX] PASS_BORROW in current DSC FRL Rate=%d\n", usTriByteborrow);
                    pstDataFlowMeteringInfo->ucCurrentFrlRateResult = _HDMI21_DATA_FLOW_METERING_PASS_BORROW;
                    return _HDMI21_DATA_FLOW_METERING_PASS_BORROW;
                }
                else
                {
                    DebugMessageHDMI21Tx("[HDMI21_TX] PASS_NO_BORROW in current DSC FRL Rate@%d\n", __LINE__);
                    pstDataFlowMeteringInfo->ucCurrentFrlRateResult = _HDMI21_DATA_FLOW_METERING_PASS_NO_BORROW;

                    // Check can reduce FRL Rate or not
                    ucCalculatingFrlRate = ucCalculatingFrlRate - 1;
                }
            }
            else
            {
                DebugMessageHDMI21Tx("[HDMI21_TX] VIDEO_FAIL in current DSC FRL Rate@%d\n", __LINE__);
                pstDataFlowMeteringInfo->ucCurrentFrlRateResult = _HDMI21_DATA_FLOW_METERING_VIDEO_FAIL;
                return _HDMI21_DATA_FLOW_METERING_VIDEO_FAIL;
            }
        }
        else
        {
            DebugMessageHDMI21Tx("[HDMI21_TX] VIDEO_FAIL in current DSC FRL Rate@%d\n", __LINE__);

            pstDataFlowMeteringInfo->ucCurrentFrlRateResult = _HDMI21_DATA_FLOW_METERING_VIDEO_FAIL;
            return _HDMI21_DATA_FLOW_METERING_VIDEO_FAIL;
        }
    }
#endif
    if((pstDataFlowMeteringInfo->b1DSCMode == _DOWN_STREAM_VESA) && (pstDataFlowMeteringInfo->ucMinFrlRate != 0) && (pstDataFlowMeteringInfo->ucCurrentFrlRate > pstDataFlowMeteringInfo->ucMinFrlRate))
    {
        return _HDMI21_DATA_FLOW_METERING_OVER;
    }
    else
    {
        return pstDataFlowMeteringInfo->ucCurrentFrlRateResult;
    }

}

//--------------------------------------------------
// Description  : Hdmi DataFlowMeter cal parameter update
// Input Value  : NONE
// Output Value : NONE , PASS DSC encoder parameter to DSC encoder handler
//--------------------------------------------------
void ScalerHdmi21MacTx0DataFlowMeterParameterSet(void)
{
    StructDataFlowMeteringInfo  *pstDataFlowMeteringInfo = NULL;
    pstDataFlowMeteringInfo = &stTx0DataFlowMeteringInfo;
    if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
        NoteMessageHDMI21Tx("[HDMI21_TX] TimingInfo NULL@DataFlowMeterParameterSet\n");
        return;
    }
    DebugMessageHDMITx("[HDMITX] GET_HDMI21_MAC_TX_FRL_RATE @ DataFlowMeterParameterSet\n");

    pstDataFlowMeteringInfo->ulFreqPixelClock_kHz = (UINT32)GET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(_TX0) * 100;
    pstDataFlowMeteringInfo->usHWidth = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth;
    pstDataFlowMeteringInfo->usHblank = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth;
    pstDataFlowMeteringInfo->ucbpc = GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0);
    pstDataFlowMeteringInfo->ucColorSpace = GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0);
    pstDataFlowMeteringInfo->ucCurrentFrlRate = GET_HDMI21_MAC_TX_FRL_RATE(_TX0);
#if(_AUDIO_SUPPORT == _ON)
    pstDataFlowMeteringInfo->ulRateAudio_Hz = ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_HZ) * 1000;
    pstDataFlowMeteringInfo->ucAudioChannels = ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CHANNEL_COUNT) + 1;
    pstDataFlowMeteringInfo->ucAudioCodeType = ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CODING_TYPE);
#endif
    if(GET_OUTPUT_STREAM_TYPE(ScalerHdmi21MacTxPxMapping(_TX0)) == _STREAM_TYPE_DSC)
    {
        pstDataFlowMeteringInfo->b4SliceNum = (UINT32)ScalerDscDecoderGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _DSC_DECODER_PIC_WIDTH) / ScalerDscDecoderGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _DSC_DECODER_SLICE_WIDTH);
        pstDataFlowMeteringInfo->usSliceWidth = ScalerDscDecoderGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _DSC_DECODER_SLICE_WIDTH);
        pstDataFlowMeteringInfo->usTargetBppx16 = ScalerDscDecoderGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _DSC_DECODER_BPP);

        DebugMessageHDMI21Tx("[HDMI21_TX] ScalerHdmi21MacTx0DataFlowMeterParameterSet :DSC Input mode !!!\n");
    }
    else
    {
        pstDataFlowMeteringInfo->b1DSCMode = 1;

        if(GET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(_TX0) != _DOWN_STREAM_DSC)
        {
            pstDataFlowMeteringInfo->b1DSCMode = 0;
            pstDataFlowMeteringInfo->b4SliceNum = 0;
            pstDataFlowMeteringInfo->usSliceWidth = 0;
            pstDataFlowMeteringInfo->usTargetBppx16 = 0;

            DebugMessageHDMI21Tx("[HDMI21_TX] ScalerHdmi21MacTx0DataFlowMeterParameterSet :VESA mode !!!\n");
        }
        else if(GET_HDMI21_MAC_TX_INPUT_FROM(_TX0) == _INPUT_FROM_ENCODER)
        {
            if(ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->b2SliceCount == 0)
            {
                pstDataFlowMeteringInfo->b4SliceNum = 1;
            }
            else if(ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->b2SliceCount == 1)
            {
                pstDataFlowMeteringInfo->b4SliceNum = 2;
            }
            else if(ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->b2SliceCount == 2)
            {
                pstDataFlowMeteringInfo->b4SliceNum = 4;
            }
            else if(ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->b2SliceCount == 3)
            {
                pstDataFlowMeteringInfo->b4SliceNum = 8;
            }
            pstDataFlowMeteringInfo->usSliceWidth = (g_ppucHdmi21MacTxPPSData[_TX0][0xC] << 8) + g_ppucHdmi21MacTxPPSData[_TX0][0xD];
            pstDataFlowMeteringInfo->usTargetBppx16 = ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->usBitsPerPixel;

            DebugMessageHDMI21Tx("[HDMI21_TX] ScalerHdmi21MacTx0DataFlowMeterParameterSet :DSC encoder mode !!!\n");
        }
    }

/*
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter ulFreqPixelClock_kHz =%d\n", pstDataFlowMeteringInfo.ulFreqPixelClock_kHz);
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter usHWidth =%d\n", pstDataFlowMeteringInfo.usHWidth);
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter usHblank =%d\n", pstDataFlowMeteringInfo.usHblank);
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter ucbpc =%d\n", pstDataFlowMeteringInfo.ucbpc);
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter ucColorSpace =%d\n", pstDataFlowMeteringInfo.ucColorSpace);
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter ucCurrentFrlRate =%d\n", pstDataFlowMeteringInfo.ucCurrentFrlRate);
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter ulRateAudio_Hz =%d\n", pstDataFlowMeteringInfo.ulRateAudio_Hz);
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter ucAudioChannels =%d\n", pstDataFlowMeteringInfo.ucAudioChannels);
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter ucAudioCodeType =%d\n", pstDataFlowMeteringInfo.ucAudioCodeType);
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter b1DSCMode =%d\n", pstDataFlowMeteringInfo.b1DSCMode);
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter b4SliceNum =%d\n", pstDataFlowMeteringInfo.b4SliceNum);
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter usSliceWidth =%d\n", pstDataFlowMeteringInfo.usSliceWidth);
    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter usTargetBppx16 =%d\n", pstDataFlowMeteringInfo.usTargetBppx16);
*/
}

//--------------------------------------------------
// Description  : Hdmi decision BW ok or not
// Input Value  : port
// Output Value : BW status
//--------------------------------------------------
EnumLinkBWDecisionResultType ScalerHdmi21MacTx0BwDecision(void)
{
//    UINT8 ucBpp = 0;
    EnumHdmi21DataFlowMeteringResult enumHdmi21DataFlowMeteringResult = _HDMI21_DATA_FLOW_METERING_NONE;
#if(_DSC_HDMI21_FORCE_ENCODER == _OFF)
    if(GET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(_TX0) != _DOWN_STREAM_DSC) // HDMI INPUT
    {
        // update now Parameter
        ScalerHdmi21MacTx0DataFlowMeterParameterSet();
        enumHdmi21DataFlowMeteringResult = ScalerHdmi21MacTx0DataFlowMetering();

        DebugMessageHDMI21Tx("[HDMI21_TX] _HDMI21_DATA_FLOW_METERING ORG RESULT (not change condition) =%d\n", enumHdmi21DataFlowMeteringResult);
        DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision Parameter ucCurrentFrlRate=%d !!!\n", stTx0DataFlowMeteringInfo.ucCurrentFrlRate);

        switch(enumHdmi21DataFlowMeteringResult)
        {
            case _HDMI21_DATA_FLOW_METERING_PASS_NO_BORROW:
                return _BW_PASS;
                break;

            case _HDMI21_DATA_FLOW_METERING_PASS_BORROW:
                return _BW_PASS;
                break;

            case _HDMI21_DATA_FLOW_METERING_OVER:
                // TO DO
#if(_DSC_HDMI21_LINK_TRAINING_DOWN_SPEED == _ON)
                SET_HDMI21_MAC_TX_RE_LINK_TRAINING(_TX0);
                return _BW_RE_LINKTRAINING_DOWN;
#else
                return _BW_PASS;
#endif
                break;

            case _HDMI21_DATA_FLOW_METERING_VIDEO_FAIL:
                if(ScalerHdmiMacTxEdidGetFeature(ScalerHdmi21MacTxPxMapping(_TX0), _DSC_VERSION_12) == _FALSE) // NOT SUPPORT DSC
                {
                    // do 420 or 8bit truncate
                    ScalerHdmi21MacTx0DataFlowMetering();

                    DebugMessageHDMI21Tx("[HDMI21_TX] BW Decision (%d) Not support DSC mode!!!\n", ScalerHdmiMacTxEdidGetFeature(ScalerHdmi21MacTxPxMapping(_TX0), _DSC_VERSION_12));
                }
                break;

#if(_AUDIO_SUPPORT == _ON)
            case _HDMI21_DATA_FLOW_METERING_AUDIO_FAIL:
                SET_HDMI21_MAC_TX_AUDIO_OFF(_TX0);
                return _BW_PASS;
                break;
#endif
            case _HDMI21_DATA_FLOW_METERING_DSC_SHALL_OFF:
                return _BW_PASS;
                break;

            default:
                break;
        }
    }
    else // DSC
    {
        // TO DO
    }

    return _BW_PASS;
#else
    if((GET_HDMI21_MAC_TX_INPUT_FROM(_TX0) == _INPUT_FROM_DP) || (GET_HDMI21_MAC_TX_INPUT_FROM(_TX0) == _INPUT_FROM_DECODER))
    {
        if(GET_HDMI21_MAC_TX_INFOR_OK(_TX0) == _TRUE) // DP DPF GET timing infor
        {
            {
                return _BW_PASS;
            }
        }
    }
#endif
}

//--------------------------------------------------
// Description  : Hdmi Tx Pixel Repetition Decision
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTx0RepetitionDecision(void)
{
    if((GET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(_TX0) < 250) && (GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) == _DP_STREAM_COLOR_DEPTH_8_BITS))   // (Unit in 0.1MHz)
    {
        SET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0, (250 / GET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(_TX0)));
    }
    else
    {
        SET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0, 0);
    }
}

//--------------------------------------------------
// Description  : Hdmi 2p1 StreamHandler
// Input Value  : NONE
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTx0StreamHandler(void)
{
    UINT8 ucMaxRetryTimes = 3;
    UINT8 ucRetryTimes = 0;
    BOOLEAN bBorrowMode = 0;

    DebugMessageHDMITx("[HDMI21_TX] StreamHandler...\n");

    // bBorrowMode = GET_HDMI21_MAC_TX_BORROW(_TX0);
    bBorrowMode = 0;

    ScalerHdmi21MacTx0RepetitionDecision();

    if(ScalerHdmi21MacTx0OutputConfiguration() == _FALSE)
    {
        return _FALSE;
    }

    // Disable HDCP2 RC
    if(Scaler_HDMITX_HDCP_get_DisableHdcp22Rc() == 1){
        rtd_maskl(HDMITX21_MAC_HDMI21_RC_reg, ~_BIT7, 0);
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_7_reg, ~_BIT1, _BIT1);
    }else{
        rtd_maskl(HDMITX21_MAC_HDMI21_RC_reg, ~_BIT7, _BIT7);
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_7_reg, ~_BIT1, 0);
    }

    //InfoMessageHDMITx("[HDMI21_TX] DisableHdcp22Rc()=%d@ScalerHdmi21MacTx0StreamHandler\n", Scaler_HDMITX_HDCP_get_DisableHdcp22Rc());

    ScalerHdmi21MacTx0FRLReset(bBorrowMode);

    for(ucRetryTimes = 0; ucRetryTimes < ucMaxRetryTimes; ucRetryTimes++)
    {
        if(ScalerHdmi21MacTx0FRLSRAMCheck() == _UNSTABLE)
        {
            static UINT32 lastDumpStc = 0, dumpCnt=0;
            UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);

            ScalerHdmi21MacTx0FRLReset(bBorrowMode);

            // [FIXME] wait den_end event
            //ScalerTimerDelayXms((1000 / GET_HDMI21_MAC_TX_INPUT_FRAME_RATE(_TX0)));
            //Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,1);
    #ifndef CONFIG_MACH_RTD2801_ZEBU
            if(((UINT32)(stc - lastDumpStc)) > 90000)
    #endif
            { // dump period=1sec
                ErrorMessageHDMI21Tx("[HDMI21_TX][%d] [HDMITX] FRL SRAM OVF!!\n", dumpCnt);
                lastDumpStc = stc;
                dumpCnt = 0;

#ifdef _DEBUG_HALT_TX_WHEN_FIFO_ERROR // HALT TX when TX FIFO error
                ErrorMessageHDMI21Tx("[HDMI21_TX] FORCE TX STATE HALT-1!!\n");
                TxHaltFlag=1;
#endif
            }
            dumpCnt++;
        }
        else
        {
            break;
        }
    }

    if(ucRetryTimes < ucMaxRetryTimes)
    {
        if(GET_HDMI21_MAC_VIDEO_START(_TX0) == _TRUE)
        {
            ScalerHdmi21MacTx0SetFRLVideoStartCnt(_FRL_CNT_OFFSET_NONE);

#ifdef FIX_ME_HDMITX_BRING_UP 
            if(HDMITX21_MAC_HDMI21_TX_CTRL_get_hdmi_21_tx_en(rtd_inl(HDMITX21_MAC_HDMI21_TX_CTRL_reg)) == 1)
                FatalMessageHDMITx("[HDMI21_TX] hdmi_21_tx_en=1@StreamHandler\n");

            // set hdmi_21_tx_en=1 in v-porch
            HDMITX_DTG_Wait_Den_Stop_Done();

            // set hdmi_21_tx_en=1 before enable frl_en
            rtd_maskl(HDMITX21_MAC_HDMI21_TX_CTRL_reg, ~_BIT7, _BIT7); // hdmi_21_tx_en=1
#endif

            rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~_BIT7, _BIT7); // frl_en=1

            udelay(50);

            ScalerHdmi21MacTx0FRLVideoStartCntRetry();

#ifdef FIX_ME_HDMITX_BRING_UP 
            if(HDMITX21_MAC_HDMI21_FRL_19_get_gap_en(rtd_inl(HDMITX21_MAC_HDMI21_FRL_19_reg)) == 0){
                DebugMessageHDMITx("[HDMI21_TX] gap_en IS DISABLED@ScalerHdmi21MacTx0StreamHandler\n");
            }else{
                DebugMessageHDMITx("[HDMI21_TX] set gap_en=0 with db_en & in v-porch\n");

                // set gap_en=0 in v-porch
                HDMITX_DTG_Wait_Den_Stop_Done();

                // set gap_gen_db_buf=1 before gap_en=0
                rtd_maskl(HDMITX21_MAC_HDMI21_FRL_19_reg, ~_BIT6, _BIT6); // gap_gen_db_buf=1
            }
#endif
            // switch to out put Video
            rtd_maskl(HDMITX21_MAC_HDMI21_FRL_19_reg, ~_BIT7, 0x00); // gap_en=0
        }

        ScalerHdmiMacTxSetStreamSource(ScalerHdmi21MacTxPxMapping(_TX0), _HDMI_TX_SOURCE_HDMI21);

        FatalMessageHDMITx("[HDMI21_TX] Stream Set Finish2@Cnt(%d/%d)\n", ucRetryTimes, ucMaxRetryTimes);
        //ScalerHdmiTxSetForceLinkTrainingEn(_DISABLE);
#ifdef NOT_USED_NOW
        //drvif_scaler_set_display_mode(DISPLAY_MODE_FRC, scaler_dtg_get_app_type());//Scaler_TxsocSetDispSync(_FALSE); // TXSOC DTG free run with VODMA
#endif
        if(GET_HDMI_TX0_HPD_FIRST_PLUG())
            CLR_HDMI_TX0_HPD_FIRST_PLUG();
        return _TRUE;
    }
    else
    {
        static UINT32 lastDumpStc = 0, dumpCnt=0;
        UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
    #ifndef CONFIG_MACH_RTD2801_ZEBU 
        if(((UINT32)(stc - lastDumpStc)) > 90000)
    #endif
        { // dump period=1sec
            NoteMessageHDMI21Tx("[HDMI21_TX][%d] 2.1 Stream Set Fail@Cnt(%d/%d), 1st=%d\n", dumpCnt, ucRetryTimes, ucMaxRetryTimes, GET_HDMI_TX0_HPD_FIRST_PLUG());
            lastDumpStc = stc;
            dumpCnt = 0;
        }
        dumpCnt++;

        // [H5X_HDMITX_BYPASS_HPD_EDID]
        if(ScalerHdmiTxGetBypassLinkTrainingEn()){
            if(GET_HDMI_TX0_HPD_FIRST_PLUG())
                CLR_HDMI_TX0_HPD_FIRST_PLUG();
        }
        return _FALSE;
    }

    return _TRUE;
}




//--------------------------------------------------
// Description  : Hdmi 2p1 Mac Set
// Input Value  : NONE
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTx0OutputConfiguration(void)
{
    UINT8 temp_value8;// = 0;
    TX_TIMING_INDEX format_idx;
    DebugMessageHDMITx("[HDMITX] ScalerHdmi21MacTx0OutputConfiguration...\n");

    // DSCE reset
    ScalerHdmiTx0_H5xDsc_encReset();

    // HDMITX DSC source select
    ScalerHdmiTx0_H5xDsc_inputSrcConfig(Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_HDMITX_DSC_SRC));

    // HDMITX MAC 2.0 setting
    // LANE PN Swap
    // LANE src sel
    ScalerHdmiTx0_MacConfig_LaneSet((ScalerHdmiTxGetTargetFrlRate() != 0? TC_HDMI21: TC_HDMI20));

 #if 1 //for debug dsce
    if(GET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(_TX0) == _DOWN_STREAM_DSC)
        ScalerHdmi21MacTx0DscTimingGenSetting();
#endif

    // [H5X_HDMITX_BYPASS_HPD_EDID]
    if(ScalerHdmiTxGetBypassLinkTrainingEn()){
        //txfifo21_en 0x180B9028 7  Enable signal of 18 bits to 20 bits: 1: Enable 0:Disable
        rtd_maskl(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg, ~HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_txfifo21_en_mask, HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_txfifo21_en_mask);//txfifo21_en= 1 =>Enable signal of 18 bits to 20 bits
    }

    // HDCP Bug
    rtd_maskl(HDMITX21_MAC_HDCP22_ENGINE_40_reg, ~_BIT0, 0x00);
    rtd_maskl(HDMITX21_MAC_HDCP22_ENGINE_40_reg, ~_BIT0, _BIT0);
    format_idx = ScalerHdmiTxGetTx0VideoFormatIndex();
    ScalerHdmiTx0DscPPSConfig(format_idx);
    ScalerHdmiTx0_H5xDsc_encClkConfig(format_idx);
    if(GET_HDMI_HD21_720_480i()){
        rtd_maskl(HDMITX21_MAC_HDMI21_MIDDLE_VS_SHIFT_reg, ~HDMITX21_MAC_HDMI21_MIDDLE_VS_SHIFT_middle_vs_shift_mask, HDMITX21_MAC_HDMI21_MIDDLE_VS_SHIFT_middle_vs_shift(0xf));
    }
    ScalerHdmi21MacTx0InputConverter();

    ScalerHdmi21MacTx0VideoSet();

    ScalerHdmi21MacTx0Scheduler();

    ScalerHdmi21MacTx0EmpPacket();

    // TX DSC SRC SEL & FIFO reset/enable
    // mode: 1: bypass mode, 0: 2pto4p mode
    HDMITX_DTG_Wait_Den_Stop_Done();

    // 0 => 2pto4p mode for HDMI21, 1 => bypass mode for HDMI20
    OutputConfigurationDscSrcSel(0);

    ScalerHdmi21MacTx0PeriodicSRAMPacking();

#if 1 // new HW setting
    // check HDMI mode status
    HDMITX_DTG_Wait_Den_Stop_Done();
    HDMITX_DTG_Wait_vsync_start();
    temp_value8 = (get_hdmitxOutputMode() == TX_OUT_TEST_MODE? 0: SCRIPT_TX_ENABLE_PACKET_BYPASS());
    ScalerHdmiMacTx0RepeaterSetPktHdrBypass(temp_value8);
    temp_value8 = ((SCRIPT_TX_ENABLE_PACKET_BYPASS() && !SCRIPT_TX_SKIP_EMP_BYPASS() && ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_EMP))? 0 : 1);
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_hdmi21_emp_src_sel_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_hdmi21_emp_src_sel(temp_value8));
    if(SCRIPT_TX_ENABLE_PACKET_BYPASS()){
        HDMITX_DTG_Wait_Den_Stop_Done();
        #if IS_ENABLED(CONFIG_RTK_HDMI_RX)
        drvif_hdmi_pkt_bypass_to_hdmitx_en(0xF, _ENABLE);
        #endif
        FatalMessageHDMITx("[HDMITX]Hdmi2.1 RX Bypass to TX Done!\n");
    }
#endif // #if 1 //new HW setting

    ScalerHdmi21MacTx0NCtsRegenConfig(); //hdmi21 N/CTS regen setting

#if(_AUDIO_SUPPORT == _ON)
    if(GET_HDMITX_AUDIO_INPUT_TYPE() == _TX_AUDIO_IN_RX_SPDIF){
        if(ScalerHdmi21MacTx0AudioSet() == _FALSE)
        {
            ErrorMessageHDMI21Tx("[HDMI21_TX]  Audio Set Fail\n");
            // return _FALSE;
        }
    }
#endif
    //hdmi21_DumpReg_TxMac_setting();

    //DebugMessageIgnore("[HDMITX] ScalerHdmi21MacTx0OutputConfiguration Done!\n");
    return _TRUE;
}
//--------------------------------------------------
// Description  : Hdmi DSC encoder Parameter Set
// Input Value  : NONE
// Output Value : NONE , PASS DSC encoder parameter to DSC encoder handler
//--------------------------------------------------
void ScalerHdmi21MacTx0SetDscEncoderParameter(void)
{
    StructDscEncoderParameter stHdmiEncoderParameter;

    if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
        NoteMessageHDMI21Tx("[HDMI21_TX] g_pstHdmi21MacTxInputTimingInfo is NULL@%s\n", __FUNCTION__);
        return;
    }

    DebugMessageHDMI21Tx("[HDMI21_TX] _DSC_ENCODER_COMPRESS PARAMETER!!!\n");

    // Update DSC encoder Parameter
    stHdmiEncoderParameter.usPixelClock = GET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(_TX0);
    stHdmiEncoderParameter.usHTotal = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal;
    stHdmiEncoderParameter.usHWidth = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth;
    stHdmiEncoderParameter.usHStart = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart;
    stHdmiEncoderParameter.usHSWidth = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth;
    stHdmiEncoderParameter.usVTotal = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVTotal;
    stHdmiEncoderParameter.usVHeight = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight;
    stHdmiEncoderParameter.usVStart = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVStart;
    stHdmiEncoderParameter.usVSWidth = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVSWidth;

    stHdmiEncoderParameter.b2SliceCount = 0;

#if 1 // [RL6583-3472][FIX-ME][ECO][Ver.C]
    stHdmiEncoderParameter.usDscStrClock = GET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(_TX0);;
    ErrorMessageHDMI21Tx("[HDMI21_TX][FIX-ME] CHECK usDscStrClock VALUE(%d) BEFORE USE!!\n", stHdmiEncoderParameter.usDscStrClock);
#endif

    stHdmiEncoderParameter.usHCactive = ScalerHdmi21MacTxGetHCactive(ScalerHdmi21MacTxPxMapping(_TX0), g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth, g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight, (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVFreq /10));
    stHdmiEncoderParameter.usHCblank = ScalerHdmi21MacTxGetHCblank(ScalerHdmi21MacTxPxMapping(_TX0), g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth, g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight, (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVFreq /10));
    InfoMessageHDMI21Tx("[HDMI21_TX] HDMI21_TX0: 2.1 TABLE HCactive = %d\n", stHdmiEncoderParameter.usHCactive);
    InfoMessageHDMI21Tx("[HDMI21_TX] HDMI21_TX0: 2.1 TABLE HCblank = %d\n", stHdmiEncoderParameter.usHCblank);

    // new add
    stHdmiEncoderParameter.usHCSWidth = _HDMI21_DSC_DPF_SYNCWIDTH;
    stHdmiEncoderParameter.ucSilceCountCap = 4; // read from EDID

    // TO OD need check DP stream
    stHdmiEncoderParameter.ucPixelMode = GET_HDMI21_MAC_TX_INPUT_PIXEL_MODE(_TX0);

    // Find input timing Bpp value by search code Table
    stHdmiEncoderParameter.usBitsPerPixel = ScalerHdmi21MacTxGetLimitBpp(ScalerHdmi21MacTxPxMapping(_TX0), g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth, g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight, (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVFreq /10)); //

    stHdmiEncoderParameter.b4BitsPerComponent = GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0);

    InfoMessageHDMI21Tx("[HDMI21_TX] set dsc encoder pixel mode =%d\n", GET_HDMI21_MAC_TX_INPUT_PIXEL_MODE(_TX0));
    InfoMessageHDMI21Tx("[HDMI21_TX] HDMI21_TX0: 2.1 TABLE BPP = %d\n", stHdmiEncoderParameter.usBitsPerPixel);
    InfoMessageHDMI21Tx("[HDMI21_TX] set dsc encoder bits per component =%d\n", GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0));


    stHdmiEncoderParameter.b4DscVersionMinor = 0x2;

    stHdmiEncoderParameter.b1BlockPredEnable = 1;

    if (GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_RGB)
    {
        stHdmiEncoderParameter.b1UseYuvInput = 0;
    }
    else
    {
        stHdmiEncoderParameter.b1UseYuvInput = 1;
    }

    // DSC encoder do 422 color convert
    stHdmiEncoderParameter.b1Simple422 = 0;
    if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR422)
    {
        stHdmiEncoderParameter.b1Native422 = 1;
    }
    else
    {
        stHdmiEncoderParameter.b1Native422 = 0;
    }

    if (GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR420)
    {
        stHdmiEncoderParameter.b1Native420 = 1;
    }
    else
    {
        stHdmiEncoderParameter.b1Native420 = 0;
    }

    ScalerDscEncoderSetParameter(ScalerHdmi21MacTxPxMapping(_TX0), &stHdmiEncoderParameter);
}

//--------------------------------------------------
// Description  : HDMI2.1 Tx DSC encoder output setting
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmi21MacTx0DscSetting(void)
{
    UINT8 ucChunckSize_L = 0;
    UINT8 ucChunckSize_M = 0;
    UINT8 ucChunckSize_H = 0;
    UINT8 ucChunckNum = 0;
    UINT8 ucDscDropInterval = 0x03;
    UINT16 usDscDropNum = 0x00;
    UINT16 usUnvaild = 0;
    UINT16 usvaild = 0;
    UINT16 usHactive = 0;
    UINT16 usHstart = 0;
    //UINT16 usHblk_drop_total = 0;
    UINT8 ucHbackMask = 4;
    UINT16 usHcBack = 0;
    UINT16 usHCbackTotalDrop = 0;
    UINT8 ucDscDropIntervalHblank = 0x03;
    UINT16 usTableHCblank = 0;
    UINT16 usHCblankORG = 0;
    UINT16 usHCblankDropLimit = 0;
    INT32 slice_width = 0;
    StructTimingInfo stTimingInfo;

    if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
        NoteMessageHDMI21Tx("[HDMI21_TX] g_pstHdmi21MacTxInputTimingInfo is NULL@%s,%d\n", __FUNCTION__, __LINE__);
        return;
    }

#if(_DSC_ENCODER_SUPPORT == _ON)
    // DSC encoder mode
    if(GET_HDMI21_MAC_TX_INPUT_FROM(_TX0) == _INPUT_FROM_ENCODER)
    {
        ucChunckSize_H = 0x00;
        ucChunckSize_M = g_ppucHdmi21MacTxPPSData[_TX0][14];
        ucChunckSize_L = g_ppucHdmi21MacTxPPSData[_TX0][15];

        // need mapping.
         
        g_pstHdmi21MacTxDscTimingInfo[_TX0].usHTotal = (((UINT16)rtd_inl(DSCE_MN_DPF_HTT_M_reg) & 0xFF) << 8) + (rtd_inl(DSCE_MN_DPF_HTT_L_reg) & 0xFF);//get htotal from dpf setting
        //g_pstHdmi21MacTxDscTimingInfo[_TX0].usHTotal = (ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->usHCactive + ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->usHCblank);
        g_pstHdmi21MacTxDscTimingInfo[_TX0].usHSWidth = ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->usHCSWidth;
        //g_pstHdmi21MacTxDscTimingInfo[_TX0].usHTotal =(UINT16)(g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal * g_ppucHdmi21MacTxPPSData[_TX0][5]) / (24 << 4);//ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->usHTotal ;
        //g_pstHdmi21MacTxDscTimingInfo[_TX0].usHSWidth = ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->usHSWidth;
        usTableHCblank = ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->usHCblank;
        //usTableHCblank = (DWORD)(g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth) *  g_ppucHdmi21MacTxPPSData[_TX0][5] / (24 << 4);
    }
    else
#endif
    {
        ucChunckSize_H = 0x00;
        ucChunckSize_M = (UINT16) (ScalerDscDecoderGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _DSC_DECODER_CHUNK_SIZE) & 0xFF00) >> 8;
        ucChunckSize_L = (UINT16) (ScalerDscDecoderGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _DSC_DECODER_CHUNK_SIZE) & 0x00FF);

        // CAL From MSA ORG timing,
        ScalerDpStreamGetMsaTimingInfo(ScalerHdmi21MacTxPxMapping(_TX0), &stTimingInfo);
        usTableHCblank = stTimingInfo.usHTotal - stTimingInfo.usHWidth;

        // need mapping
        g_pstHdmi21MacTxDscTimingInfo[_TX0].usHTotal = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal;
        g_pstHdmi21MacTxDscTimingInfo[_TX0].usHSWidth = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth;
    }

    DebugMessageHDMI21Tx("[HDMI21_TX0] chunck size M =%x\n", ucChunckSize_M);
    DebugMessageHDMI21Tx("[HDMI21_TX0] chunck size L =%x\n", ucChunckSize_L);
    DebugMessageHDMI21Tx("[HDMI21_TX0] DSC DPF usHTotal =%d\n", g_pstHdmi21MacTxDscTimingInfo[_TX0].usHTotal);
    DebugMessageHDMI21Tx("[HDMI21_TX0] DSC DPF usHSWidth =%d\n", g_pstHdmi21MacTxDscTimingInfo[_TX0].usHSWidth);
    DebugMessageHDMI21Tx("[HDMI21_TX0] Target HCBLANK =%d\n", usTableHCblank);

    rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_14_reg, ~_BIT5, 0x00);

    // Chunck size 3 byte => need check
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_10_reg, ucChunckSize_H);
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_11_reg, ucChunckSize_M);
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_12_reg, ucChunckSize_L);
    slice_width = ((UINT16)(g_ppucHdmi21MacTxPPSData[_TX0][12]) <<8)
                        | (g_ppucHdmi21MacTxPPSData[_TX0][13]);

#if(_DSC_ENCODER_SUPPORT == _ON)
    if(GET_HDMI21_MAC_TX_INPUT_FROM(_TX0) == _INPUT_FROM_ENCODER)
    {
        if((ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->b2SliceCount) == 0)
        {
            ucChunckNum = 1;
        }
        else if((ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->b2SliceCount) == 1)
        {
            ucChunckNum = 2;
        }
        else if((ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->b2SliceCount) == 2)
        {
            ucChunckNum = 4;
        }
        else if((ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->b2SliceCount) == 3)
        {
            ucChunckNum = 8;
        }
    }
    else // by pass mode
#endif
    {
        ucChunckNum = (ScalerDscDecoderGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _DSC_DECODER_PIC_WIDTH)) / (ScalerDscDecoderGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _DSC_DECODER_SLICE_WIDTH));
    }
    ucChunckNum = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth / slice_width;
    DebugMessageHDMITx("[HDMI21_TX] hwidth=%d\n",g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth );
    DebugMessageHDMITx("[HDMI21_TX] slice_width=%d\n",slice_width);

    DebugMessageHDMITx("[HDMI21_TX] ucChunckNum=%d\n",ucChunckNum);

    DebugMessageHDMITx("[HDMI21_TX] Read usUnvaild reg time for 2.1 LO=%x\n",rtd_inl(TIMER_SCPU_CLK90K_LO_reg));

    // chunck num
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_13_reg, ucChunckNum);
    //ROSTimeDly(20);
    HDMITX_DTG_Wait_Den_Stop_Done();
    HDMITX_DTG_Wait_Den_Start();
    HDMITX_DTG_Wait_Den_Stop_Done();

    // PART A cal drop inteerval Hactive - DSC_H_act_unvalid_read = valid cyclce
    usHstart = ((rtd_inl(HDMITX21_MAC_HDMI21_IN_CONV_18_reg) & 0x0F) << 8) + (rtd_inl(HDMITX21_MAC_HDMI21_IN_CONV_19_reg) & 0xFF);
    usUnvaild = (((UINT16)rtd_inl(HDMITX21_MAC_HDMI21_IN_CONV_20_reg) & 0x7F) << 8) + (rtd_inl(HDMITX21_MAC_HDMI21_IN_CONV_21_reg) & 0xFF);
    DebugMessageHDMITx("[HDMI21_TX] Read usUnvaild reg time for 2.1 LO=%x\n",rtd_inl(TIMER_SCPU_CLK90K_LO_reg));

    DebugMessageHDMI21Tx("[HDMI21_TX] 5. HDMI21_TX0: ucChunckNum = %d\n", ucChunckNum);
    DebugMessageHDMI21Tx("[HDMI21_TX] !!!!!!HDMI21_TX0: usHstart = %d\n", usHstart);
    DebugMessageHDMI21Tx("[HDMI21_TX] !!!!!!HDMI21_TX0: usUnvaild = %d\n", usUnvaild);

    usHactive = (g_pstHdmi21MacTxDscTimingInfo[_TX0].usHTotal) - usHstart - _HDMI21_DSC_DPF_HFRONT;
    //usHactive = (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth);
    //usHactive = (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal - ScalerDscEncoderGetParameter(ScalerHdmi21MacTxPxMapping(_TX0))->usHCblank);


    // 720 T of 64 bit for DSC ENC DPF
    usvaild = usHactive - usUnvaild;
    ucDscDropInterval = (usHactive / usvaild) + ((((usHactive % usvaild)) > 0) ? 1 : 0);
    usDscDropNum = ucDscDropInterval - 1;

    DebugMessageHDMI21Tx("[HDMI21_TX0] !!!!!!HDMI21_TX0: usvaild=%d\n", usvaild);
    DebugMessageHDMI21Tx("[HDMI21_TX0] usHactive=%d\n", usHactive);
    DebugMessageHDMI21Tx("[HDMI21_TX0] V blanking Drop ucDscDropInterval=%d\n", ucDscDropInterval);
    DebugMessageHDMI21Tx("[HDMI21_TX0] V blanking Drop usDscDropNum=%d\n", usDscDropNum);

    rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_14_reg, ~(_BIT7 | _BIT1 | _BIT0), 0x00);
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_15_reg, ucDscDropInterval);
    rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_16_reg, ~(_BIT1 | _BIT0), 0x00);
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_17_reg, usDscDropNum);
    rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_14_reg, ~_BIT7, _BIT7);


    usHCblankORG = (usHstart + _HDMI21_DSC_DPF_HFRONT) * 4;

    // Drop max value T unit
    usHCblankDropLimit = (usHstart - g_pstHdmi21MacTxDscTimingInfo[_TX0].usHSWidth - 1 - ucHbackMask) * 14 / 15;

    DebugMessageHDMI21Tx("[HDMI21_TX0] HDMI21_TX0:ORG blank value pixel domain=%d\n", usHCblankORG);
    DebugMessageHDMI21Tx("[HDMI21_TX0] HDMI21_TX0:blank value drop limit =%d (T)\n", usHCblankDropLimit);

    // PARB B Set drop mode ( start after HS Falling)

    if(usHCblankORG > usTableHCblank + _HDMI21_DSC_HBALNK_TOLERANCE)
    {
        // Set mask cycle of HCback porch
        rtd_outl(HDMITX21_MAC_HDMI21_FRL_11_reg, ucHbackMask);

        // Calculate number of cycle (4 phase/cycle) need to drop
        usHCbackTotalDrop = (((11 + usHstart) * 4) - usTableHCblank) / 4;
        usHcBack = (usHstart - g_pstHdmi21MacTxDscTimingInfo[_TX0].usHSWidth - 1 - ucHbackMask);

        //ucDscDropIntervalHblank = (usHcBack / (usHcBack - usHCbackTotalDrop)) + (((usHcBack % (usHcBack - usHCbackTotalDrop)) > 0) ? 1 : 0);

        if(usHCbackTotalDrop < usHCblankDropLimit)
        {
            // Cal drop ratio
            ucDscDropIntervalHblank = (usHcBack / (usHcBack - usHCbackTotalDrop)) + (((usHcBack % (usHcBack - usHCbackTotalDrop)) > 0) ? 1 : 0);
        }
        else
        {
            usHCbackTotalDrop = usHCblankDropLimit;
            ucDscDropIntervalHblank = 0xF;
            usTableHCblank = ((usHstart + _HDMI21_DSC_DPF_HFRONT) - usHCbackTotalDrop)*4;

            DebugMessageHDMI21Tx("[HDMI21_TX0] Out of Drop Range=%d\n", usHCblankDropLimit);
            DebugMessageHDMI21Tx("[HDMI21_TX0] Fix HCblank value =%d\n", usTableHCblank);
        }

        // must update HCblank
        g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCblank = usTableHCblank;

        DebugMessageHDMI21Tx("[HDMI21_TX0] usTableHCblank=%d\n", usTableHCblank);
        DebugMessageHDMI21Tx("[HDMI21_TX0] ucDscDropIntervalHblank = %d\n", ucDscDropIntervalHblank);
        DebugMessageHDMI21Tx("[HDMI21_TX0] usHCbackTotalDrop=%d\n", usHCbackTotalDrop);


        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_8_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), ((usHCbackTotalDrop >> 8) & 0x0F));
        rtd_outl(HDMITX21_MAC_HDMI21_FRL_9_reg, (usHCbackTotalDrop) & 0xFF);

        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_10_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (ucDscDropIntervalHblank << 4));
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_10_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (ucDscDropIntervalHblank - 1));

        // Enable Drop part II
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_8_reg, ~_BIT7, _BIT7);
    }
    else
    {
        g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCblank = usHCblankORG;

        DebugMessageHDMI21Tx("[HDMI21_TX0] No Drop Hblank , usHCblankORG = %d\n", usHCblankORG);
    }

    g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCfront = _HDMI21_DSC_DPF_HFRONT * 4;
    g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCactive = usvaild * 96 / (8 * 3); // bit per compont
    g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCStart = g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCblank - g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCfront;
    g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCback = g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCStart - (_HDMI21_DSC_DPF_SYNCWIDTH * 4);

    DebugMessageHDMI21Tx("[HDMI21_TX0] DSC usHCfront = %d\n", g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCfront);
    DebugMessageHDMI21Tx("[HDMI21_TX0] DSC usHCactive = %d\n", g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCactive);
    DebugMessageHDMI21Tx("[HDMI21_TX0] DSC usHCStart = %d\n", g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCStart);
    DebugMessageHDMI21Tx("[HDMI21_TX0] DSC usHCback = %d\n", g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCback);

    // format output path
    rtd_maskl(HDMITX21_MAC_HDMI21_FORMAT_TOP_reg, ~(_BIT7 | _BIT6), _BIT7);

    // scheduler disable HS
    rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_18_reg, ~(_BIT7 | _BIT6), _BIT6);
}
//--------------------------------------------------
// Description  : Hdmi 2p1 Set Input Format Converter
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------
void ScalerHdmi21MacTx0InputConverter(void)
{
    UINT8 ucHSWUnvalid = 0;
    UINT8 ucHFrontUnvalid = 0;
    UINT8 ucHBackUnvalid = 0;
    UINT8 ucHactiveUnvalid = 0;
    UINT16 usVBackPorch = 0;
    UINT16 usVBlankHalf = 0;
    UINT8 inputClk4Pixel = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_CLK_4PIXEL_MODE);
    DebugMessageHDMI21Tx("[HDMI21_TX] ScalerHdmi21MacTx0InputConverter\n");

    if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
        DebugMessageHDMI21Tx("[HDMI21_TX] g_pstHdmi21MacTxInputTimingInfo is NULL@ScalerHdmi21MacTx0InputConverter\n");
        return;
    }

#if 0 // [FIXME][H5X] NOT H5X REGISTER
    // Set Input Pixel Mode
    if(rtd_getbits(PB8_1F_SOURCE_SEL_4, _BIT0) == _BIT0)
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_0_reg, ~(_BIT7 | _BIT6), _BIT6);
    }
    else
#endif
    if(inputClk4Pixel) // [H5X] HDMI21 Input only support 4 pixel mode
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_0_reg, ~(_BIT7 | _BIT6), _BIT7);
    }
    else
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_0_reg, ~(_BIT7 | _BIT6), 0x00);
    }
    if(GET_HDMI_HD_480i_576i() && (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight == 240)){
        rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_H0_reg, ~HDMITX_MISC_HDMITX_RETIMING_H0_hdmi21_480i_vs_center_adj_mask, HDMITX_MISC_HDMITX_RETIMING_H0_hdmi21_480i_vs_center_adj(1));
    }else{
        rtd_maskl(HDMITX_MISC_HDMITX_RETIMING_H0_reg, ~HDMITX_MISC_HDMITX_RETIMING_H0_hdmi21_480i_vs_center_adj_mask, HDMITX_MISC_HDMITX_RETIMING_H0_hdmi21_480i_vs_center_adj(0));
    }
    // [HDMITX 2.1] input FIFO channel connet: color verify on ZEBU platform
#if 0 // [IC] #ifdef FIX_ME_HDMITX_BRING_UP
    if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR444)
    {
    #if 1 // Zebu platform setting -- let HDMI/Video XTOR have the same display color
        // 0xB804C4C0 = 0x90
        hdmitx21_mac_hdmi21_in_fifo_reg.chb_swap = 2; // b
        hdmitx21_mac_hdmi21_in_fifo_reg.chg_swap = 1; // g
        hdmitx21_mac_hdmi21_in_fifo_reg.chr_swap = 0; // r
    #else // Zebu platform setting -- let HDMI XTOR YUV/RGB mode have the same display color
        // 0xB804C4C0 = 0x48
        hdmitx21_mac_hdmi21_in_fifo_reg.chb_swap = 1; // b
        hdmitx21_mac_hdmi21_in_fifo_reg.chg_swap = 0; // g
        hdmitx21_mac_hdmi21_in_fifo_reg.chr_swap = 2; // r
    #endif
    }else

    {
        // 0xB804C4C0 = 0x18
        hdmitx21_mac_hdmi21_in_fifo_reg.chb_swap = 0; // b
        hdmitx21_mac_hdmi21_in_fifo_reg.chg_swap = 1; // g
        hdmitx21_mac_hdmi21_in_fifo_reg.chr_swap = 2; // r
    }
#endif
    rtd_maskl(HDMITX21_MAC_HDMI21_IN_FIFO_reg, ~(_BIT7 | _BIT6), 0x0);
    rtd_maskl(HDMITX21_MAC_HDMI21_IN_FIFO_reg, ~(_BIT5 | _BIT4), _BIT4);
    rtd_maskl(HDMITX21_MAC_HDMI21_IN_FIFO_reg, ~(_BIT3 | _BIT2), _BIT3);

    // Set DSC drop Setting
    if(GET_HDMI21_MAC_TX_DSCE_EN(_TX0))//TC_DSC_ENABLE
    {
        ScalerHdmi21MacTx0SetDscEncoderParameter();
        // Set DSC drop Setting
        ScalerHdmi21MacTx0DscSetting();

        // HSW Unvalid
        ucHSWUnvalid = (_HDMI21_DSC_DPF_SYNCWIDTH * 4) % 4;

        if(ucHSWUnvalid != 0x00)
        {
            ucHSWUnvalid = 4 - ucHSWUnvalid;
        }

        // H front porch Unvalid
        ucHFrontUnvalid = g_pstHdmi21MacTxHCTiminginfo[0].usHCfront % 4;

        if(ucHFrontUnvalid != 0x00)
        {
            ucHFrontUnvalid = 4 - ucHFrontUnvalid;
        }

        // H back porch Unvalid
        ucHBackUnvalid = g_pstHdmi21MacTxHCTiminginfo[0].usHCback % 4;

        if(ucHBackUnvalid != 0x00)
        {
            ucHBackUnvalid = 4 - ucHBackUnvalid;
        }

        // H Avtive Unvalid
        ucHactiveUnvalid = g_pstHdmi21MacTxHCTiminginfo[0].usHCactive % 4;

        if(ucHactiveUnvalid != 0x00)
        {
            ucHactiveUnvalid = 4 - ucHactiveUnvalid;
        }

        DebugMessageHDMI21Tx("HDMI21_TX0: usHCactive = %d ", g_pstHdmi21MacTxHCTiminginfo[0].usHCactive);

        DebugMessageHDMI21Tx("[HDMI21_TX] ucHSWUnvalid= %d,ucHFrontUnvalid=%d,ucHBackUnvalid=%d,ucHactiveUnvalid = %d H5X\n", ucHSWUnvalid,ucHFrontUnvalid,ucHBackUnvalid,ucHactiveUnvalid);
        rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_2_reg, ~(_BIT7 | _BIT6 | _BIT5), ucHSWUnvalid << 5);

        rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_2_reg, ~(_BIT4 | _BIT3 | _BIT2), ucHFrontUnvalid << 2);

        rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_3_reg, ~(_BIT7 | _BIT6 | _BIT5), ucHBackUnvalid << 5);

        rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_3_reg, ~(_BIT4 | _BIT3 | _BIT2), ucHactiveUnvalid << 2);

    }
    else
    {

        if(GET_HDMI_HD21_needUnvalid()){
            ucHSWUnvalid = unvalid_480i_576i[needUnvalid_index].hsw_unvalid;
        }else{
            // HSW Unvalid
            ucHSWUnvalid = (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth) % 4;
            if(ucHSWUnvalid != 0x00)
            {
                ucHSWUnvalid = 4 - ucHSWUnvalid;
            }
        }
        rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_2_reg, ~(_BIT7 | _BIT6 | _BIT5), ucHSWUnvalid << 5);

        // H front porch Unvalid
        if(GET_HDMI_HD21_needUnvalid()){
            ucHFrontUnvalid = unvalid_480i_576i[needUnvalid_index].h_fporch_unvalid;
        }else{
            ucHFrontUnvalid = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal) - (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth) - (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart)) % 4;
            if(ucHFrontUnvalid != 0x00)
            {
                ucHFrontUnvalid = 4 - ucHFrontUnvalid;
            }
        }
        rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_2_reg, ~(_BIT4 | _BIT3 | _BIT2), ucHFrontUnvalid << 2);

        // H back porch Unvalid
        if(GET_HDMI_HD21_needUnvalid()){
            ucHBackUnvalid = unvalid_480i_576i[needUnvalid_index].h_bporch_unvalid;
        }else{
            ucHBackUnvalid = ((g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart) - (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth)) % 4;
            if(ucHBackUnvalid != 0x00)
            {
                ucHBackUnvalid = 4 - ucHBackUnvalid;
            }
        }
        rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_3_reg, ~(_BIT7 | _BIT6 | _BIT5), ucHBackUnvalid << 5);

        // Hactive Unvalid
        ucHactiveUnvalid = (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth) % 4;
        if(ucHactiveUnvalid != 0x00)
        {
            ucHactiveUnvalid = 4 - ucHactiveUnvalid;
        }
        DebugMessageHDMI21Tx("[HDMI21_TX] ucHSWUnvalid= %d,ucHFrontUnvalid=%d,ucHBackUnvalid=%d,ucHactiveUnvalid = %d\n", ucHSWUnvalid,ucHFrontUnvalid,ucHBackUnvalid,ucHactiveUnvalid);

        rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_3_reg, ~(_BIT4 | _BIT3 | _BIT2), ucHactiveUnvalid << 2);
    }

    // Vactive
    rtd_maskl(HDMITX21_MAC_HDMI21_IN_CONV_4_reg, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight) >> 8);
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_5_reg, g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight & 0xff);

    // V back porch
    usVBackPorch = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVStart - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVSWidth;
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_6_reg, usVBackPorch >> 8);
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_7_reg, usVBackPorch);

    // V Blanking / 2
    usVBlankHalf = (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVTotal - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight) / 2;
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_8_reg, usVBlankHalf >> 8);
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_9_reg, usVBlankHalf);

    DebugMessageHDMI21Tx("[HDMI21_TX] usVHeight= %d,usVBackPorch=%d,usVBlankHalf=%d\n", g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVHeight,usVBackPorch,usVBlankHalf);

    return;
}


////////////////////////////////////////////////////////////////////////
//--------------------------------------------------
// Description  : Hdmi 2p1 Set DeepColor/Repetition
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------
void ScalerHdmi21MacTx0VideoSet(void)
{
    // Color Format
    if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_RGB)
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_422_CONV_reg, ~(_BIT7 | _BIT6), 0x00);
    }
    else if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR422)
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_422_CONV_reg, ~(_BIT7 | _BIT6), _BIT6);
        rtd_maskl(HDMITX21_MAC_HDMI21_422_CONV_reg, ~_BIT5, 0x00);
    }
    else if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR444)
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_422_CONV_reg, ~(_BIT7 | _BIT6), _BIT7);
    }
    else if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR420)
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_422_CONV_reg, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
    }

    DebugMessageHDMI21Tx("[HDMI21_TX] Color Depth=%d\n", GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0));

    if(GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) > _DP_STREAM_COLOR_DEPTH_8_BITS)
    {
        ScalerHdmi21MacTx0VideoPllSet();
    }

    // Select repeat mode
    if(GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM((_TX0)) > 0)
    {
        // Set repeat_mode
        rtd_maskl(HDMITX21_MAC_HDMI21_PR_0_reg, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3), (GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) << 3));

        // Set repetition_en = 1
        rtd_maskl(HDMITX21_MAC_HDMI21_PR_0_reg, ~_BIT7, _BIT7);
    }
    else
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_PR_0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), 0x00);
    }

    // DPC disable
    rtd_maskl(HDMITX21_MAC_HDMI21_DPC_0_reg, ~_BIT7, 0x00);

    // set deep color fifo start position and reset
    rtd_maskl(HDMITX21_MAC_HDMI21_DPC_2_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x01);

    // Set Color Depth
    rtd_maskl(HDMITX21_MAC_HDMI21_DPC_0_reg, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3), GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) << 2);

#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
    if((g_scaler_display_info.input_src == 0) && GET_HDMIRX_DPC_DEFAULT_PH()  && (GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) > _DP_STREAM_COLOR_DEPTH_8_BITS))
    {
        //dp_phase_default = 1
        rtd_maskl(HDMITX21_MAC_HDMI21_DPC_4_reg, ~_BIT2, _BIT2);
        //dp_phase_default_en = 1
        rtd_maskl(HDMITX21_MAC_HDMI21_DPC_4_reg, ~_BIT1, _BIT1);
    }
    else
#endif
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_DPC_4_reg, ~(_BIT2 | _BIT1), 0);
    }

    // reset dpc small fifo error status
    rtd_maskl(HDMITX21_MAC_HDMI21_DPC_1_reg, ~(_BIT7 | _BIT6), _BIT7| _BIT6);

    // Set vch_mode_bypass
    rtd_maskl(HDMITX21_MAC_HDMI21_DPC_3_reg, ~_BIT7, _BIT7);


    //00: no wait, same as vch_mode_bypass=1.
    //01: wait 1 Vsync (HW default)
    //10: wait 2 Vsync
    //11: wait 3 Vsync
    rtd_maskl(HDMITX21_MAC_HDMI21_DPC_3_reg, ~HDMITX21_MAC_HDMI21_DPC_3_dp_vch_num_mask, HDMITX21_MAC_HDMI21_DPC_3_dp_vch_num(GET_HDMI21_MAC_TX_DSCE_EN(_TX0)? 1: 0));//[6:5]=00 /00: no wait, same as vch_mode_bypass=1.

    // Data Path Mux(vesa bypass also need enable deep color)
    if(GET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(_TX0) == _DOWN_STREAM_DSC)
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_FORMAT_TOP_reg, ~(_BIT7 | _BIT6), _BIT7); // DSC
    }
    else if(GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM((_TX0)) > 0)
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_FORMAT_TOP_reg, ~(_BIT7 | _BIT6), 0); // VESA
    }
    else
    {
#ifdef CONFIG_ENABLE_ZEBU_BRING_UP // keep clock source in deep color mode (8bit deep color mode clock should same as VESA input source)
        rtd_maskl(HDMITX21_MAC_HDMI21_FORMAT_TOP_reg, ~(_BIT7 | _BIT6), _BIT6); // DEEP COLOR
#else
        if(GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) > _DP_STREAM_COLOR_DEPTH_8_BITS)
        {
            rtd_maskl(HDMITX21_MAC_HDMI21_FORMAT_TOP_reg, ~(_BIT7 | _BIT6), _BIT6); // DEEP COLOR
        }
        else
        {
            rtd_maskl(HDMITX21_MAC_HDMI21_FORMAT_TOP_reg, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6)); // VESA
        }
#endif
    }

    // wait v-sync start
    HDMITX_DTG_Wait_vsync_start();

    // enable deep color in vsync position

    if(GET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(_TX0) == _DOWN_STREAM_DSC)
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_DPC_0_reg, ~_BIT7, 0);
    }
    else
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_DPC_0_reg, ~_BIT7, _BIT7);
    }
#if 0 // TEST
    ErrorMessageHDMI21Tx("[HDMI21_TX] DPC_0=%x!!\n", rtd_inl(HDMITX21_MAC_HDMI21_DPC_0_reg));
#endif

    return;
}

//--------------------------------------------------
// Description  : Hdmi 2p1 Set Video Pll
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------
void ScalerHdmi21MacTx0VideoPllSet(void)
{
#if 0 // NOT H5X PLL Reg.
    UINT16 usPixelClk = GET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(_TX0); // 0.1MHz
    UINT16 usPLLinFreq = 0;
    UINT16 usVCOFreq = 0;
    UINT8 ucPre_Div = 0;
    UINT8 ucOutput_Div = 0;
    UINT8 ucN_code = 0;
    UINT8 ucPixelMode = 0;
    UINT8 ucFactor = 0;

    // PIXEL_MODE 0: 1 pixel mode, 1: 2pixel mode, 2: 4pixel mode
    ucPixelMode = 1 << GET_HDMI21_MAC_TX_INPUT_PIXEL_MODE(_TX0);

    if(GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) == _DP_STREAM_COLOR_DEPTH_10_BITS)
    {
        ucFactor = 5;
    }
    else if(GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) == _DP_STREAM_COLOR_DEPTH_12_BITS)
    {
        ucFactor = 6;
    }

    if(usPixelClk / 4 < 140)
    {
        usPLLinFreq = usPixelClk / ucPixelMode;


        rtd_maskl(HDMITX21_MAC_PLL_IN_CONTROL_reg, ~(_BIT7 | _BIT6), _BIT6);
    }
    else
    {
        usPLLinFreq = usPixelClk / 4;

        rtd_maskl(HDMITX21_MAC_PLL_IN_CONTROL_reg, ~(_BIT7 | _BIT6), _BIT7);
    }

    ucPre_Div = usPLLinFreq / 140;

    if((ucPre_Div > 1) && ((ucPre_Div % 2) != 0))
    {
        ucPre_Div = ucPre_Div - 1;
    }

    for(pData[0] = 0; pData[0] < 255; pData[0] += 2)
    {
        ucOutput_Div = ((pData[0] == 0) ? 1 : pData[0]);

        if(ScalerGetBit(HDMITX21_MAC_PLL_IN_CONTROL_reg, (_BIT7 | _BIT6)) == _BIT6)
        {
            if((ucPre_Div * ucOutput_Div * ucPixelMode) % 16 == 0)
            {
                ucN_code = ucFactor * ucPre_Div * ucOutput_Div * ucPixelMode / 16;

                usVCOFreq = usPLLinFreq * ucN_code / ucPre_Div;

                if((usVCOFreq >= 3000) && (usVCOFreq <= 6300))
                {
                    break;
                }
            }
        }
        else
        {
            if((ucPre_Div * ucOutput_Div) % 4 == 0)
            {
                ucN_code = ucFactor * ucPre_Div * ucOutput_Div / 4;

                usVCOFreq = usPLLinFreq * ucN_code / ucPre_Div;

                if((usVCOFreq >= 3000) && (usVCOFreq <= 6300))
                {
                    break;
                }
            }
        }
    }

    if(ucPre_Div == 0x01)
    {
        rtd_maskl(P2A_45_PLL_DIVIDER1, ~_BIT7, _BIT7);
    }
    else
    {
        // predivider = reg_dpll_sel_prediv + 2
        rtd_outl(P2A_44_PLL_DIVIDER0, ucPre_Div - 2);

        rtd_maskl(P2A_45_PLL_DIVIDER1, ~_BIT7, 0x00);
    }

    // Power Down
    rtd_maskl(P2A_48_PLL_CONTROL, ~_BIT0, _BIT0);

    // LDO_en
    rtd_maskl(P2A_49_PLL_LDO, ~_BIT7, _BIT7);

    rtd_outl(P2A_46_PLL_0, 0x7A);

    if((3200 <= usVCOFreq) && (usVCOFreq < 3800))
    {
        // ipi = 3.33uA
        rtd_maskl(P2A_47_PLL_1, ~(_BIT1 | _BIT0), _BIT1);
    }
    else if((3800 <= usVCOFreq) && (usVCOFreq < 5250))
    {
        // ipi = 5uA
        rtd_maskl(P2A_47_PLL_1, ~(_BIT1 | _BIT0), _BIT0);
    }
    else
    {
        // ipi = 10uA
        rtd_maskl(P2A_47_PLL_1, ~(_BIT1 | _BIT0), 0x00);
    }

    if(ucOutput_Div % 8 == 0)
    {
        rtd_maskl(P2A_45_PLL_DIVIDER1, ~(_BIT1 | _BIT0), (_BIT1 | _BIT0));

        rtd_maskl(HDMITX21_MAC_PLL_OUT_CONTROL_reg, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (ucOutput_Div >> 3) - 1);
    }
    else if(ucOutput_Div % 4 == 0)
    {
        rtd_maskl(P2A_45_PLL_DIVIDER1, ~(_BIT1 | _BIT0), _BIT1);

        rtd_maskl(HDMITX21_MAC_PLL_OUT_CONTROL_reg, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (ucOutput_Div >> 2) - 1);
    }
    else if(ucOutput_Div % 2 == 0)
    {
        rtd_maskl(P2A_45_PLL_DIVIDER1, ~(_BIT1 | _BIT0), _BIT0);

        rtd_maskl(HDMITX21_MAC_PLL_OUT_CONTROL_reg, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (ucOutput_Div >> 1) - 1);
    }
    else
    {
        rtd_maskl(P2A_45_PLL_DIVIDER1, ~(_BIT1 | _BIT0), 0x00);

        rtd_maskl(HDMITX21_MAC_PLL_OUT_CONTROL_reg, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), ucOutput_Div - 1);
    }

    // power on
    rtd_maskl(P2A_48_PLL_CONTROL, ~_BIT0, 0x00);
    rtd_maskl(P2A_48_PLL_CONTROL, ~_BIT7, _BIT7);

    rtd_outl(P2A_4E_N_F_CODE_1, ucN_code - 2);

    rtd_maskl(P2A_4D_N_F_CODE_0, ~_BIT0, _BIT0); // load_nf

    DebugMessageHDMI21Tx("[HDMI21_TX] usVCOFreq = %d\n", usVCOFreq);

    rtd_maskl(HDMITX21_MAC_PLL_OUT_CONTROL_reg, ~_BIT7, _BIT7); // DPLL output Enable

    // wait pll stable
    ScalerTimerDelayXms((1000 / GET_HDMI21_MAC_TX_INPUT_FRAME_RATE(_TX0)) * 10);

#endif
}

//--------------------------------------------------
// Description  : ScalerHdmi21MacTx0HVPol
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------
#if 1
void ScalerHdmi21MacTx0HVPol(void)
{
    UINT16 usHFrontPorch = 0;

    // Calculate HfrontPorch = Hblank - Hstart = usHtotal - usHactive - Hstart
    usHFrontPorch = (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth - g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart);

    // HfrontPorch >= Hsync
    if(usHFrontPorch >= g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth)
    {
        // Hpol/Vpol : P/P, H:bit5, V:bit6, 0 is positive, 1: negtive
        rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~(_BIT6|_BIT5), 0);
    }
    else
    {
        // VIC = 39
        if((usHFrontPorch == 32) && (g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth == 168))
        {
            // Hpol/Vpol : P/N
            rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~(_BIT6|_BIT5), _BIT6);
        }
        else
        {
            // Hpol/Vpol : N/N
            rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~(_BIT6|_BIT5), _BIT6|_BIT5);
        }
    }

    // dump H/V polarity status
    DebugMessageHDMITx("[HDMI21_TX] POL H=%d\n", (UINT32)HDMITX21_MAC_HDMI21_SCHEDULER_0_get_hs_out_inv(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg));
    DebugMessageHDMITx("[HDMI21_TX] POL V=%d\n", (UINT32)HDMITX21_MAC_HDMI21_SCHEDULER_0_get_vs_out_inv(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg));
    DebugMessageHDMITx("[HDMI21_TX] hs/start=%d/%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHSWidth, (UINT32)g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart);
    DebugMessageHDMITx("[HDMI21_TX] Width/front=%d/%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth, (UINT32)usHFrontPorch);
    DebugMessageHDMITx("[HDMI21_TX] H-Total=%d\n", (UINT32)g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal);

    return;
}
#endif
#if 0
void ScalerHdmi21MacTx0HVPol(void)
{
    UINT8 hdmirx_pol = 0x3;
#ifdef CONFIG_RTK_HDMI_RX
    if(g_scaler_display_info.input_src == 0)
        hdmirx_pol = GET_HDMIRX_RAW_TIMING_POLARITY();
#endif
    FatalMessageHDMITx("[HDMI21_TX]polarity=%d\n", (UINT32)hdmirx_pol);
    // 0 : hpol=N,vpol=N, 1 : hpol=P,vpol=N, 2 : hpol=N,vpol=P, 3 : hpol=P,vpol=P
    if(hdmirx_pol == 0)
        rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~(_BIT6|_BIT5), _BIT6|_BIT5);
    else if(hdmirx_pol == 1)
        rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~(_BIT6|_BIT5), _BIT6);
    else if(hdmirx_pol == 2)
        rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~(_BIT6|_BIT5), _BIT5);
    else
        rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~(_BIT6|_BIT5), 0);
}
#endif
//--------------------------------------------------
// Description  : Hdmi 2p1 Set Scheduler
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------

void ScalerHdmi21MacTx0Scheduler(void)
{
    UINT16 usKeepoutStart1st = 0;
    UINT16 usKeepoutStart = 0;
    UINT16 usMaxPktCanTransmitPerLine = 0;
    UINT16 usHStart = 0;
    UINT16 usHTotal = 0;
    UINT8 ucReKeyWinSize = rtd_inl(HDMITX21_MAC_HDMI21_SCHEDULER_7_reg) + 1;
    UINT16 usHWidth = 0;
    if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
        DebugMessageHDMI21Tx("[HDMI21_TX] g_pstHdmi21MacTxInputTimingInfo is NULL@ScalerHdmi21MacTx0Scheduler\n");
        return;
    }
    // HW GAP
    // cp_clravmute = 1
    rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg, ~(_BIT7 | _BIT6 | _BIT5), _BIT5);

    // Delay 1 Frame + 2ms
    HDMITX_MDELAY((1000 / GET_HDMI21_MAC_TX_INPUT_FRAME_RATE(_TX0) + 2));

    rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg, ~_BIT7, _BIT7); // HW GAP
    //rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg, ~(_BIT7 | _BIT5), (_BIT7 | _BIT5)); // HW GAP
    // Delay 1 Frame + 2ms
    HDMITX_MDELAY((GET_HDMI_MAC_TX0_INPUT_FRAME_RATE()? (1000 / GET_HDMI_MAC_TX0_INPUT_FRAME_RATE()): 16) + 2);

    if(GET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(_TX0) != _DOWN_STREAM_DSC)
    {
        if(g_pstHdmi21MacTxDPFCompensateInfo[_TX0] == NULL){
            DebugMessageHDMI21Tx("[HDMI21_TX][ERROR] g_pstHdmi21MacTxDPFCompensateInfo is NULL!!@ScalerHdmi21MacTx0Scheduler\n");
            return;
        }

        if(g_pstHdmi21MacTxDPFCompensateInfo[_TX0]->b1TimingCompensationFlag == _TRUE)
        {
            usHStart = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart - g_pstHdmi21MacTxDPFCompensateInfo[_TX0]->b2HSyncCompensate - g_pstHdmi21MacTxDPFCompensateInfo[_TX0]->b2HBackPorchCompensate;
            usHTotal = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal - g_pstHdmi21MacTxDPFCompensateInfo[_TX0]->b4HDirectionCompensate;
            usHWidth = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth - g_pstHdmi21MacTxDPFCompensateInfo[_TX0]->b2HWidthCompensate;
        }
        else
        {
            usHStart = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHStart;
            usHTotal = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal;
            usHWidth = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth;
        }

#ifdef SCALER_SUPPORT_420_CONFIG
        if((GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR420) && (ScalerDpStreamGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _COLOR_SPACE) != _COLOR_SPACE_YCBCR420))
#else
        if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR420)
#endif
        {
            usHStart = usHStart / 2;
            usHTotal = usHTotal / 2;
            usHWidth = usHWidth / 2;
        }

        usHStart = (UINT16)usHStart * GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) / 8 * (GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) + 1);
        usHTotal = (UINT16)usHTotal * GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) / 8 * (GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) + 1);
        usHWidth = (UINT16)usHWidth * GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) / 8 * (GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) + 1);

        DebugMessageHDMI21Tx("[HDMI21_TX] HDMI21_TX0: VESA mode\n");
    }
    else
    {
#if(_DSC_ENCODER_SUPPORT == _ON)
        if(GET_HDMI21_MAC_TX_INPUT_FROM(_TX0) == _INPUT_FROM_ENCODER)
        {
            usHStart = g_pstHdmi21MacTxHCTiminginfo[0].usHCblank - g_pstHdmi21MacTxHCTiminginfo[0].usHCfront;
            usHTotal = g_pstHdmi21MacTxHCTiminginfo[0].usHCactive + g_pstHdmi21MacTxHCTiminginfo[0].usHCblank;
            usHWidth = g_pstHdmi21MacTxHCTiminginfo[0].usHCactive;
        }
        else
#endif
        {
            usHStart = (((rtd_inl(HDMITX21_MAC_HDMI21_IN_CONV_18_reg) & 0x0F) << 8) + (rtd_inl(HDMITX21_MAC_HDMI21_IN_CONV_19_reg) & 0xFF)) * 4; // 86
            usHWidth = g_pstHdmi21MacTxInputTimingInfo[0]->usHWidth * 2 / 3 * 4; // 1920
            usHTotal = usHWidth + (_HDMI21_DSC_DPF_HFRONT * 4) + usHStart; // 2048
        }
    }

    DebugMessageHDMI21Tx("[HDMI21_TX] HDMI21_TX0: ScalerHdmi21MacTx0Scheduler usHStart=%d\n", (UINT32)usHStart);
    DebugMessageHDMI21Tx("[HDMI21_TX] HDMI21_TX0: ScalerHdmi21MacTx0Scheduler usHTotal=%d\n", (UINT32)usHTotal);
    DebugMessageHDMI21Tx("[HDMI21_TX] HDMI21_TX0: ScalerHdmi21MacTx0Scheduler usHWidth=%d\n", (UINT32)usHWidth);

    if(usHStart > _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE)
    {
        usKeepoutStart1st = usHStart - _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE;
    }
    else
    {
        usKeepoutStart1st = 1;
    }

    if((usHTotal - usHWidth) > _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE)
    {
        usKeepoutStart = usHTotal - usHWidth - _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE;
    }
    else
    {
        usKeepoutStart = 1;

        SET_HDMI21_MAC_TX_BW_AUDIO_MUTE(_TX0);
    }

    DebugMessageHDMI21Tx("[HDMI21_TX] color=%d/%d\n", (UINT32)GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0), (UINT32)ScalerDpStreamGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _COLOR_SPACE));
    DebugMessageHDMI21Tx("[HDMI21_TX] usHStart=%d\n", (UINT32)usHStart);
    DebugMessageHDMI21Tx("[HDMI21_TX] total=%d\n", (UINT32)usHTotal);
    DebugMessageHDMI21Tx("[HDMI21_TX] cwidth=%d\n", (UINT32)usHWidth);

    // pre_keepout_start_1st
    rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_3_reg, ~(HDMITX21_MAC_HDMI21_SCHEDULER_3_video_pre_keepout_start_1st_14_8_mask), (usKeepoutStart1st >> 8) & HDMITX21_MAC_HDMI21_SCHEDULER_3_video_pre_keepout_start_1st_14_8_mask);
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_4_reg, usKeepoutStart1st);

    // pre_keepout_start
    rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_5_reg, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), usKeepoutStart >> 8);
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_6_reg, usKeepoutStart);

    usMaxPktCanTransmitPerLine = ((((usHTotal - usHWidth) * (GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) + 1)) > (ucReKeyWinSize + 14 + 17)) ?
                                  (((usHTotal - usHWidth) * (GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) + 1) - ucReKeyWinSize - 14 - 17) / 32) : 0);

    if(usMaxPktCanTransmitPerLine > 18)
    {
        usMaxPktCanTransmitPerLine = 18;
    }

    // Set Max Continue Pkt Size by input timing
    rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~HDMITX21_MAC_HDMI21_SCHEDULER_0_vblank_island_max_pkt_num_mask, (BYTE)usMaxPktCanTransmitPerLine);
    rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_1_reg, ~HDMITX21_MAC_HDMI21_SCHEDULER_1_vactive_island_max_pkt_num_mask, ((BYTE)usMaxPktCanTransmitPerLine) << 3);

    DebugMessageHDMI21Tx("[HDMI21_TX] DI max countinue PKTs:%d\n", usMaxPktCanTransmitPerLine);

    ScalerHdmi21MacTx0HVPol();

    // Scheduler enable
    rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~_BIT7, _BIT7);

#ifdef CONFIG_MACH_RTD2801_ZEBU 
    // [ZEBU] hs/vs out inv
    rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~(_BIT6|_BIT5), _BIT6|_BIT5);
#endif // #ifdef CONFIG_MACH_RTD2801_ZEBU

    return;
}


//--------------------------------------------------
// Description  : Hdmi 2p1 Set FRL Video Start Count
// Input Value  : Fine tune FRL offset
// Output Value : NONE
//--------------------------------------------------
void ScalerHdmi21MacTx0SetFRLVideoStartCnt(UINT8 ucFrlFineTune)
{
    UINT16 usFRLInFreq = 0; // 0.1MHz
    UINT16 usFRLClkFreq = 0; // 0.1MHz
    UINT16 usVideoStartCnt = 0;

    if(!ScalerHdmiTxGetFrlNewModeEnable())
        return;

    // VESA
    if(GET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(_TX0) != _DOWN_STREAM_DSC)
    {
        usFRLInFreq = ((UINT32)GET_HDMI21_MAC_TX_INPUT_PIXEL_CLK(_TX0)) * GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) / 8 * (GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) + 1) / 4;

        if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR420)
        {
            usFRLInFreq = usFRLInFreq / 2;
        }
    }

    if(GET_HDMI21_MAC_TX_FRL_RATE(_TX0) <= 2)
    {
        usFRLClkFreq = GET_HDMI21_MAC_TX_FRL_RATE(_TX0) * 3 * 10000 / 18;

        usVideoStartCnt = 680 - (6 * usFRLClkFreq / usFRLInFreq + 1) - 4;
    }
    else
    {
        usFRLClkFreq = ((UINT32)GET_HDMI21_MAC_TX_FRL_RATE(_TX0)) * 2 * 10000 / 18;

        usVideoStartCnt = 510 - (6 * usFRLClkFreq / usFRLInFreq + 1) - 4;
    }

    usVideoStartCnt = usVideoStartCnt - ucFrlFineTune;
    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_12_reg, ~(_BIT1 | _BIT0), usVideoStartCnt >> 8);
    rtd_outl(HDMITX21_MAC_HDMI21_FRL_13_reg, usVideoStartCnt);

    DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] FRL StartCnt=%d@FRL=%d\n", usVideoStartCnt, GET_HDMI21_MAC_TX_FRL_RATE(_TX0));
    return;
}

//--------------------------------------------------
// Description  : Hdmi 2p1 Check FRL count Stable or not
// Input Value  : NONE
// Output Value : _TRUE : stable , _FALSE : unstable
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTx0FRLVideoStartCntCheck(void)
{
    UINT16 usLimit = 0;

    if(!ScalerHdmiTxGetFrlNewModeEnable())
        return _TRUE;

    // Read FRL reference Count
    usLimit = ((rtd_inl(HDMITX21_MAC_HDMI21_FRL_14_reg) & 0x3) << 8) + rtd_inl(HDMITX21_MAC_HDMI21_FRL_15_reg);

    // Check FRL count unstable case
    if((usLimit > 0x00) && (usLimit < 0x10))
    {
        return _FALSE;
    }
    else
    {
        return _TRUE;
    }
}

//--------------------------------------------------
// Description  : Hdmi 2p1 Check FRL count Retry
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------
void ScalerHdmi21MacTx0FRLVideoStartCntRetry(void)
{
    #define START_CNT_RETRY_NUM 10
    UINT8 ucRetry = 0;

    if(!ScalerHdmiTxGetFrlNewModeEnable())
        return;

    if(ScalerHdmi21MacTx0FRLVideoStartCntCheck() == _FALSE)
    {
        for(ucRetry = 0; ucRetry < START_CNT_RETRY_NUM; ucRetry++)
        {
            rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~_BIT7, 0x00);
            ScalerHdmi21MacTx0FRLReset(GET_HDMI21_MAC_TX_BORROW(_TX0));
            ScalerHdmi21MacTx0SetFRLVideoStartCnt(_FRL_CNT_OFFSET_2 + ucRetry);
            rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~_BIT7, _BIT7);
            udelay(50);

            if(ScalerHdmi21MacTx0FRLVideoStartCntCheck() == _TRUE)
            {
                break;
            }

            DebugMessageHDMITx("FRL StartCnt Check=%d\n", ucRetry);
        }

        if(ucRetry == START_CNT_RETRY_NUM)
            ErrorMessageHDMI21Tx("[HDMI21_TX] FRL StartCnt Check Fail@%d!!\n", START_CNT_RETRY_NUM);
    }

    return;
}


//--------------------------------------------------
// Description  : Hdmi 2p1 Check FRL SRAM Flag
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTx0FRLSRAMCheck(void)
{
    //rtd_maskl(HDMITX21_MAC_HDMI21_FRL_23_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), _BIT7);
    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_23_reg, ~HDMITX21_MAC_HDMI21_FRL_ERR, HDMITX21_MAC_HDMI21_FRL_ERR);

    HDMITX_MDELAY(2);

    if(rtd_getbits(HDMITX21_MAC_HDMI21_FRL_23_reg, ~(_BIT7|_BIT2|_BIT1|_BIT0)) != 0x00)
    {
        UINT32 errCode = rtd_inl(HDMITX21_MAC_HDMI21_FRL_23_reg) & HDMITX21_MAC_HDMI21_FRL_ERR;
        ErrorMessageHDMI21Tx("[HDMI21_TX] FRL SRAM Err[%x]!!\n", errCode);

        #if 0 // [FOR TEST] FRL SRAM error debuging
        {
            // bypass FRL SRAM error check
            if(rtd_inl(0xB8028304) & _BIT19){ // 0x00080000
                ErrorMessageHDMI21Tx("[HDMI21_TX] BYPASS FRL SRAM Error\n");
                rtd_maskl(HDMITX21_MAC_HDMI21_FRL_23_reg, ~HDMITX21_MAC_HDMI21_FRL_ERR, errCode);
                return _STABLE;
            }

            while((rtd_inl(0xB8028304) & _BIT21) == 0){ // 0x00200000
                ErrorMessageHDMI21Tx("[HDMI21_TX] FRL SRAM Error[%x] Pending!\n", rtd_inl(HDMITX21_MAC_HDMI21_FRL_23_reg));
                msleep(3000);
            }
        }
        #endif

        //rtd_maskl(HDMITX21_MAC_HDMI21_FRL_23_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), _BIT7);
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_23_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), errCode);

        return _UNSTABLE;
    }
    else
    {
        return _STABLE;
    }
}

//--------------------------------------------------
// Description  : Hdmi Tx Color Space Convert to 420
// Input Value  :
// Output Value :
//--------------------------------------------------
#ifdef NOT_USED_NOW
void ScalerHdmi21MacTx0ConvertTo420(void)
{
    if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR422)
    {
        if(ScalerColorStream422to420Support(ScalerHdmi21MacTxPxMapping(_TX0)) == _TRUE)
        {
            ScalerColorStream422To420(ScalerHdmi21MacTxPxMapping(_TX0), _FUNCTION_ON);

            SET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0, _COLOR_SPACE_YCBCR420);
        }
        else if((ScalerColorStream444to420Support(ScalerHdmi21MacTxPxMapping(_TX0)) == _TRUE) && (ScalerColorStream422to444Support(ScalerHdmi21MacTxPxMapping(_TX0)) == _TRUE))
        {
            ScalerColorStream422To444(ScalerHdmi21MacTxPxMapping(_TX0), _FUNCTION_ON);

            ScalerColorStream444To420(ScalerHdmi21MacTxPxMapping(_TX0), _FUNCTION_ON);

            SET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0, _COLOR_SPACE_YCBCR420);
        }
        else
        {
            DebugMessageHDMI21Tx("[HDMI21_TX]  HW can't support 420 convert\n");

            return;
        }
    }
    else if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_RGB)
    {
        if(ScalerColorStream444to420Support(ScalerHdmi21MacTxPxMapping(_TX0)) == _TRUE)
        {
            if(GET_HDMI21_MAC_TX_INPUT_COLOR_RGB_QUANTIZATION_RANGE(_TX0) == _RGB_QUANTIZATION_LIMIT_RANGE)
            {
                ScalerColorStreamRgb2Yuv(ScalerHdmi21MacTxPxMapping(_TX0), _FUNCTION_ON, _RGB2YUV_ITU709_LIMIT);
            }
            else
            {
                ScalerColorStreamRgb2Yuv(ScalerHdmi21MacTxPxMapping(_TX0), _FUNCTION_ON, _RGB2YUV_ITU709_FULL);
            }

            ScalerColorStream444To420(ScalerHdmi21MacTxPxMapping(_TX0), _FUNCTION_ON);

            SET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0, _COLOR_SPACE_YCBCR420);
        }
        else
        {
            DebugMessageHDMI21Tx("[HDMI21_TX]  HW can't support 420 convert\n");

            return;
        }
    }
    else if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR444)
    {
        if(ScalerColorStream444to420Support(ScalerHdmi21MacTxPxMapping(_TX0)) == _TRUE)
        {
            ScalerColorStream444To420(ScalerHdmi21MacTxPxMapping(_TX0), _FUNCTION_ON);

            SET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0, _COLOR_SPACE_YCBCR420);
        }
        else
        {
            DebugMessageHDMI21Tx("[HDMI21_TX]  HW can't support 420 convert\n");

            return;
        }
    }
}
#endif
//--------------------------------------------------
// Description  : HDMI Tx Detect Hdcp rekey window change
// Input Value  : None
// Output Value : TRUE: Change; FALSE: No Change
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTx0DetectHdcpRekeyWindowChg(void)
{
    UINT8 ucReKeyWinSize = rtd_inl(HDMITX21_MAC_HDMI21_SCHEDULER_7_reg) + 1;

    if(GET_HDMI21_MAC_TX_HDCP_REKEY_WIN_SIZE(_TX0) != ucReKeyWinSize)
    {
        SET_HDMI21_MAC_TX_HDCP_REKEY_WIN_SIZE(_TX0, ucReKeyWinSize);

        return _TRUE;
    }

    return _FALSE;
}

//UINT32 ulHDMIRefCTScode = 0;
void ScalerHdmi21MacTx0NCtsRegenConfig(void)
{
    UINT32 ulHDMITxACRNcode = 0;
    //UINT32 ulHDMIRefCTScode = 0;
    UINT16 usAudioSampleFreq = 480; // Unit:0.1KHz
    UINT16 usRbit = 0;
    UINT8 ucFreqIndex = 0;
    UINT8 ucLinkRateIndex = 0;
    UINT8 temp_value8 = 0;
    SET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0, AudioFreqType);

    switch(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0))
    {
        case _AUDIO_FREQ_32K:
        case _AUDIO_FREQ_44_1K:
        case _AUDIO_FREQ_48K:

            ucFreqIndex = 0;
            break;

        case _AUDIO_FREQ_64K:
        case _AUDIO_FREQ_88_2K:
        case _AUDIO_FREQ_96K:

            ucFreqIndex = 1;
            break;

        case _AUDIO_FREQ_128K:
        case _AUDIO_FREQ_176_4K:
        case _AUDIO_FREQ_192K:

            ucFreqIndex = 2;
            break;

        case _AUDIO_FREQ_256K:
        case _AUDIO_FREQ_352_8K:
        case _AUDIO_FREQ_384K:

            ucFreqIndex = 3;
            break;

        case _AUDIO_FREQ_512K:
        case _AUDIO_FREQ_705_6K:
        case _AUDIO_FREQ_768K:

            ucFreqIndex = 4;
            break;

        default:
            break;
    }

    switch(GET_HDMI21_MAC_TX_FRL_RATE(_TX0))
    {
        case _HDMI21_FRL_3G:

            ucLinkRateIndex = 0;
            usRbit = (UINT16)(30000 / 18);
            break;

        case _HDMI21_FRL_6G_3LANES:
        case _HDMI21_FRL_6G_4LANES:

            ucLinkRateIndex = 1;
            usRbit = (UINT16)(60000 / 18);
            break;

        case _HDMI21_FRL_8G:

            ucLinkRateIndex = 2;
            usRbit = (UINT16)(80000 / 18);
            break;

        case _HDMI21_FRL_10G:

            ucLinkRateIndex = 3;
            usRbit = (UINT16)(100000 / 18);
            break;

        case _HDMI21_FRL_12G:

            ucLinkRateIndex = 4;
            usRbit = (UINT16)(120000 / 18);
            break;

        default:
            break;
    }

    switch(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0))
    {
        case _AUDIO_FREQ_32K:
        case _AUDIO_FREQ_64K:
        case _AUDIO_FREQ_128K:

            ulHDMITxACRNcode = tHDMI21_TX_AUDIO_32K_MULTI_N_CTS[ucLinkRateIndex * 7 + ucFreqIndex];
            //ulHDMIRefCTScode = tHDMI21_TX_AUDIO_32K_MULTI_N_CTS[ucLinkRateIndex * 7 + 6];

            if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) == _AUDIO_FREQ_128K)
            {
                usAudioSampleFreq = 1280;
            }
            else if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) == _AUDIO_FREQ_64K)
            {
                usAudioSampleFreq = 640;
            }
            else
            {
                usAudioSampleFreq = 320;
            }

            break;

        case _AUDIO_FREQ_44_1K:
        case _AUDIO_FREQ_88_2K:
        case _AUDIO_FREQ_176_4K:

            ulHDMITxACRNcode = tHDMI21_TX_AUDIO_44_1K_MULTI_N_CTS[ucLinkRateIndex * 7 + ucFreqIndex];
            //ulHDMIRefCTScode = tHDMI21_TX_AUDIO_44_1K_MULTI_N_CTS[ucLinkRateIndex * 7 + 6];

            if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) == _AUDIO_FREQ_176_4K)
            {
                usAudioSampleFreq = 1764;
            }
            else if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) == _AUDIO_FREQ_88_2K)
            {
                usAudioSampleFreq = 882;
            }
            else
            {
                usAudioSampleFreq = 441;
            }
            break;

        case _AUDIO_FREQ_48K:
        case _AUDIO_FREQ_96K:
        case _AUDIO_FREQ_192K:

            ulHDMITxACRNcode = tHDMI21_TX_AUDIO_48K_MULTI_N_CTS[ucLinkRateIndex * 7 + ucFreqIndex];
            //ulHDMIRefCTScode = tHDMI21_TX_AUDIO_48K_MULTI_N_CTS[ucLinkRateIndex * 7 + 6];

            if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) == _AUDIO_FREQ_192K)
            {
                usAudioSampleFreq = 1920;
            }
            else if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) == _AUDIO_FREQ_96K)
            {
                usAudioSampleFreq = 960;
            }
            else
            {
                usAudioSampleFreq = 480;
            }
            break;

        default:
            break;
    }
    FatalMessageHDMITx("[HDMITX21] AudioBypassEn=%d,usRbit %d", (UINT32)!SCRIPT_TX_SKIP_AUDIO_BYPASS(),usRbit);
    FatalMessageHDMITx("Type[%d]/,", (UINT32)AudioFreqType);
    FatalMessageHDMITx("Sample=%d,", (UINT32)usAudioSampleFreq);
    FatalMessageHDMITx("Ncode=%d\n", (UINT32)ulHDMITxACRNcode);
        //rtd_outl(0xb801952c, 0x05000001); // HDMITX_MISC_HDMITX_NCTS_REGEN_3_reg
    rtd_outl(HDMITX_MISC_HDMITX_NCTS_REGEN_3_reg, HDMITX_MISC_HDMITX_NCTS_REGEN_3_tx_n_value(ulHDMITxACRNcode));
    temp_value8 = (SCRIPT_TX_SKIP_AUDIO_BYPASS()? 0: 1);
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_3_reg, ~ HDMITX_MISC_HDMITX_NCTS_REGEN_3_ncts_dbg_en_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_3_ncts_dbg_en(temp_value8));

        // [V] HDMITX MISC NCTS Regen
    //rtd_outl(0xb8019524, 0x00ff0820); // HDMITX_MISC_HDMITX_NCTS_REGEN_1_reg
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_1_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_1_fifo_threshold_hi_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_1_fifo_threshold_hi(0x820));

    //rtd_outl(0xb8019528, 0x080007e0); // HDMITX_MISC_HDMITX_NCTS_REGEN_2_reg
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_2_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_2_fifo_threshold_mid_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_2_fifo_threshold_mid(0x800));
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_2_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_2_fifo_threshold_low_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_2_fifo_threshold_low(0x7e0));

    //rtd_outl(0xb8019520, 0x71000fa0); // HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg
    temp_value8 = (SCRIPT_TX_SKIP_AUDIO_BYPASS()? 0: 1);
    rtd_outl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_mux(temp_value8)); // N/CTS mux: 0x0 : N/CTS from spdif. Original path, 0x1 : N/CTS from regen. The new method.
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_0_rbus_period_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_0_rbus_period(0xfa));
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_0_measure_period_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_0_measure_period(0x7));
#ifdef CONFIG_FORCE_AUDIO_PACKET_BYPASS
    if(lib_hdmitx_is_hdmi_bypass_support() == _TRUE)
        temp_value8 = 0;//always ncts bypass rx
    else
#endif
        temp_value8 = (SCRIPT_TX_SKIP_AUDIO_BYPASS()? 0: 1);
    if(temp_value8){
        ScalerHdmiTxNCtsRegenFifoCheckSetting();
    }
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_regen_en_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_regen_en(temp_value8));
    return;
}

#if(_AUDIO_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Get Hdcp Audio Mute Flag
// Input Value  :
// Output Value :
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTxGetHdcpAudioMute(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmiMacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)

#endif

        default:
            break;
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : HDMI2.1 Tx Audio Set
// Input Value  : None
// Output Value : None
//--------------------------------------------------
BOOLEAN ScalerHdmi21MacTx0AudioSet(void)
{
    UINT8 audFmtChange = 0;

    UINT8 ucAudioSampleNuminOnePacket = 0;
    UINT8 temp_value8;// = 0;
    // Audio Exist(Audio Frequency Range Detect Result and VB-ID amute flag = 0)
    if((ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_TYPE) != _AUDIO_FREQ_NO_AUDIO)
            && (ScalerHdmiMacTxGetHdcpAudioMute(ScalerHdmi21MacTxPxMapping(_TX0)) == _FALSE)
            && (ScalerStreamGetAudioMuteFlag(ScalerHdmi21MacTxPxMapping(_TX0)) == _FALSE))
    {

        //DebugMessageHDMI21Tx("[HDMI21_TX]  Audio Exist\n");
        // Audio Firstly deal or Audio Format/ Frequency/ Audio CH Num change
        if((GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) != ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_TYPE)) ||
           (GET_HDMI21_MAC_TX_AUDIO_CH_NUM(_TX0) != (ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CHANNEL_COUNT) + 1)) ||
           (GET_HDMI21_MAC_TX_AUDIO_FORMAT(_TX0) != (ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CODING_TYPE))) ||
           (ScalerHdmi21MacTx0DetectHdcpRekeyWindowChg() == _TRUE))
        {
            // Save Audio Ch Num and Frequency infomation
            // check audio format change
            audFmtChange = 0;
            temp_value8 = ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_TYPE);
            if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) != temp_value8){
                FatalMessageHDMITx("[HDMI21_TX]  Audio Freq=%d->", (UINT32)temp_value8);
                FatalMessageHDMITx("%d\n", (UINT32)GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0));
                SET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0, temp_value8);
                audFmtChange =1;
            }
            temp_value8 = ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CHANNEL_COUNT);
            if(GET_HDMI21_MAC_TX_AUDIO_CH_NUM(_TX0) != (temp_value8 + 1)){
                FatalMessageHDMITx("[HDMI21_TX]  Audio CH=%d->", (UINT32)temp_value8+1);
                FatalMessageHDMITx("%d\n", (UINT32)GET_HDMI21_MAC_TX_AUDIO_CH_NUM(_TX0));
                SET_HDMI21_MAC_TX_AUDIO_CH_NUM(_TX0, temp_value8 + 1);
                audFmtChange =1;
            }
            temp_value8 = ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CODING_TYPE);
            if(GET_HDMI21_MAC_TX_AUDIO_FORMAT(_TX0) != temp_value8){
                FatalMessageHDMITx("[HDMI21_TX]  Audio Format=%d->", (UINT32)temp_value8);
                FatalMessageHDMITx("%d\n", (UINT32)GET_HDMI21_MAC_TX_AUDIO_FORMAT(_TX0));
                SET_HDMI21_MAC_TX_AUDIO_FORMAT(_TX0, temp_value8);
                audFmtChange =1;
            }

#if 0
            if(audFmtChange|| (GET_HDMI_MAC_TX0_STREAM_SOURCE() == _HDMI_TX_SOURCE_NONE)){
                WarnMessageHDMITx("[HDMI21_TX]  Audio Freq/CH/Fmt=%d/", (UINT32)GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0));
                WarnMessageHDMITx("%d/", (UINT32)GET_HDMI21_MAC_TX_AUDIO_CH_NUM(_TX0));
                WarnMessageHDMITx("%d\n", (UINT32)GET_HDMI21_MAC_TX_AUDIO_FORMAT(_TX0));
            }
#endif
#if(AUDIO_BYPASS)
            rtd_maskl(HDMITX_MISC_HDMITX_AUDIO_BYPASS_reg,~(_BIT25 | _BIT24| _BIT4| _BIT0),(_BIT25 | _BIT24| _BIT4| _BIT0));
#endif

#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
            /*if(ScalerDpStreamGetAudioFwTrackingStableStatus(ScalerHdmi21MacTxPxMapping(_TX0)) == _FALSE)
            {
                // Set afifo_en = 0(Disable Audio Data into HDMI Module)
                CLR_HDMI21_MAC_TX0_AUDIO_FIFO_ENABLE();
            }
            else*/
            {
                // Set afifo_en = 1(Enable Audio Data into HDMI Module)
                SET_HDMI21_MAC_TX0_AUDIO_FIFO_ENABLE();
            }
#else
            SET_HDMI21_MAC_TX0_AUDIO_FIFO_ENABLE();
#endif

            // -----------------------------------------------------------
            // FW Precheck Audio BW Avaliable
            // -----------------------------------------------------------
            ucAudioSampleNuminOnePacket = ScalerHdmi21MacTx0CalAudioSampleNuminOnePacket();

            // -----------------------------------------------------------
            // Set Audio Sample Packet Relatived Setting
            // -----------------------------------------------------------
            ScalerHdmi21MacTx0AudioSamplePktProc(ucAudioSampleNuminOnePacket);

            // -----------------------------------------------------------
            // Set Audio Auxiliary Info Pkt
            // -----------------------------------------------------------
#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
            SET_HDMI21_MAC_TX0_CTS_CODE_FW_MODE();
#endif

#if(AUDIO_BYPASS)
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_reg, ~_BIT2, _BIT2);

            ScalerHdmi21MacTx0ACRPacket(audFmtChange);

            // ACR Packet will be sent
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_reg, ~_BIT4, 0x00);
#else

            ScalerHdmi21MacTx0ACRPacket(audFmtChange);

            // ACR Packet will be sent
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_reg, ~_BIT4, 0x00);
            // Packet1: Audio infoframe
            // disable pkt transmission
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT1, 0x00);
            ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT1_ADDRESS, _HDMI_TX_AUDIO_INFOFRAME, _HDMI_TX_SW_PKT_DFF0);

            // enable pkt transmission
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT1, _BIT1);
#endif

#if(_HDMI21_3D_AUDIO_TX0_SUPPORT == _ON)
            if(ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CODING_TYPE) == _3D_LPCM_ASP)
            {
                // Packet7: Audio Metadata Packet
                // disable pkt transmission
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT7, 0x00);
                ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT7_ADDRESS, _HDMI_TX_AUDIO_METADATA, _HDMI_TX_SW_PKT_DFF1);

                // enable pkt transmission
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT7, _BIT7);
            }
#endif

            // -----------------------------------------------------------
            // Set hdmi_tx_spdif_en = 1, SPDIF Audio Format to HDMI Audio Packet
            // -----------------------------------------------------------
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_0_reg, ~_BIT4, _BIT4);

            HDMITX_MDELAY(10);
        }

        // VG870 audio 2ch >> 8ch 0x137A[4]=1(audio SRAM full)
        // no audio output in this case, FW need to reset audio FIFO(0x126C[0])
        // Chech audio sram status
        if(rtd_getbits(HDMITX21_PACKET_CONTROL_HDMI21_AUD_SRAM_TOP_1_reg, _BIT4) == _BIT4)
        {
            // Reset Audio FIFO
            CLR_HDMI21_MAC_TX0_AUDIO_FIFO_ENABLE();
            SET_HDMI21_MAC_TX0_AUDIO_FIFO_ENABLE();

            HDMITX_MDELAY(10);

            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_SRAM_TOP_1_reg, ~_BIT4, _BIT4);
        }
    }
    else
    {
        if(((ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_TYPE) == _AUDIO_FREQ_NO_AUDIO) && (ScalerStreamGetAudioMuteFlag(ScalerHdmi21MacTxPxMapping(_TX0)) == _TRUE))
                || (ScalerHdmiMacTxGetHdcpAudioMute(ScalerHdmi21MacTxPxMapping(_TX0)) == _TRUE))
        {

#if(AUDIO_BYPASS)
            rtd_maskl(HDMITX_MISC_HDMITX_AUDIO_BYPASS_reg,~(_BIT25 | _BIT24| _BIT4| _BIT0),0x00);
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_reg, ~_BIT2, 0x00);

#endif
            if(rtd_getbits(HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_0_reg, _BIT4) == _BIT4)
            {
                // ACR Packet will be not sent
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_reg, ~_BIT4, _BIT4);

                // Set hdmi_tx_spdif_en = 0, No SPDIF Audio Format to HDMI Audio Packet
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_0_reg, ~_BIT4, 0x00);

                // Set afifo_en = 0 Disable Audio FIFO
                CLR_HDMI21_MAC_TX0_AUDIO_FIFO_ENABLE();

                // Packet1: Set Null Packet
                // disable pkt transmission
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT1, 0x00);
                ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT1_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0);

                // enable pkt transmission
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT1, _BIT1);

#if(_HDMI21_3D_AUDIO_TX0_SUPPORT == _ON)
                // Packet7: Set Null Packet
                // disable pkt transmission
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT7, 0x00);
                ScalerHdmi21MacTx0FillPktData(_HDMI_TX_SW_PKT7_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF1);

                // enable pkt transmission
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, ~_BIT7, _BIT7);
#endif
                WarnMessageHDMITx("[HDMI21_TX]  Audio OFF/Mute\n");
            }
        }
        SET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0, 0);
        SET_HDMI21_MAC_TX_AUDIO_CH_NUM(_TX0, 0);
    }
    return _TRUE;
}

//--------------------------------------------------
// Description  : HDMI Tx Calculate Audio Sample Number in One HDMI Audio Sample Packet
// Input Value  : None
// Output Value : None
//--------------------------------------------------
UINT8 ScalerHdmi21MacTx0CalAudioSampleNuminOnePacket(void)
{
    UINT16 usHtotal  = 0x00;
    UINT16 usHStart = 0;
    UINT8 ucReKeyWinSize;
    UINT16 usAudioSampleFreq = 480; // Unit:0.1KHz
    UINT16 usMaxPktCanTransmitPerLine = 0;
    UINT16 usAP = 0;
    UINT8 bLpcmAudio;
    UINT32 ulAudioSampleNumPerLine = 0;
    UINT32 ulAudioPktNeedTransmitPerLine = 0;

    UINT8 ucAudioCHnum = GET_HDMI21_MAC_TX_AUDIO_CH_NUM(_TX0);
    UINT8 usACRFreq = 10; // Unit:0.1KHz

    UINT16 usHactive = 0x00;

    UINT16 usVtotal = 0x00;
    UINT8 ucAudioSampleNuminOnePacket = 4;


    // Rekey reg + HW dummy cycle
    ucReKeyWinSize = rtd_inl(HDMITX21_MAC_HDMI21_SCHEDULER_7_reg) + 1;
    bLpcmAudio = ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CHANNEL_COUNT); // ch num = ucAudioChfromDpRx + 1

    if(!g_pstHdmi21MacTxInputTimingInfo[_TX0]){
        DebugMessageHDMI21Tx("[HDMI21_TX] g_pstHdmi21MacTxInputTimingInfo is NULL@CalAudioSampleNuminOnePacket\n");
        return 0;
    }


    // Set AP factor
    if(ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CODING_TYPE) == _LPCM_ASP)
    {
        if(ucAudioCHnum <= 2)
        {
            usAP = 25; // 0.25 * 100
        }
        else
        {
            usAP = 100; // 1 * 100
        }
    }
    else if(ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CODING_TYPE) == _HBR_ASP)
    {
        usAP = 100;
    }
    else if(ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CODING_TYPE) == _3D_LPCM_ASP)
    {
        if(ucAudioCHnum <= 12)
        {
            usAP = 200; // 2 * 100
        }
        else if(ucAudioCHnum <= 24)
        {
            usAP = 300; // 3 * 100
        }
        else
        {
            usAP = 400; // 4 * 100
        }
    }


    // Set Audio sample rate
    switch(ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_TYPE))
    {
        case _AUDIO_FREQ_32K:

            usAudioSampleFreq = 320;
            break;

        case _AUDIO_FREQ_44_1K:

            usAudioSampleFreq = 441;
            break;

        case _AUDIO_FREQ_48K:

            usAudioSampleFreq = 480;
            break;

        case _AUDIO_FREQ_64K:

            usAudioSampleFreq = 640;
            break;

        case _AUDIO_FREQ_88_2K:

            usAudioSampleFreq = 882;
            break;

        case _AUDIO_FREQ_96K:

            usAudioSampleFreq = 960;
            break;

        case _AUDIO_FREQ_128K:

            usAudioSampleFreq = 1280;
            break;

        case _AUDIO_FREQ_176_4K:

            usAudioSampleFreq = 1764;
            break;

        case _AUDIO_FREQ_192K:

            usAudioSampleFreq = 1920;
            break;

        case  _AUDIO_FREQ_256K:

            usAudioSampleFreq = 2560;
            break;

        case  _AUDIO_FREQ_352_8K:

            usAudioSampleFreq = 3528;
            break;

        case  _AUDIO_FREQ_384K:

            usAudioSampleFreq = 3840;
            break;

        case  _AUDIO_FREQ_512K:

            usAudioSampleFreq = 5120;
            break;

        case  _AUDIO_FREQ_705_6K:

            usAudioSampleFreq = 7056;
            break;

        case  _AUDIO_FREQ_768K:

            usAudioSampleFreq = 7680;
            break;

        default:
            usAudioSampleFreq = 0;

            break;
    }

    // ---------------------------------------------------
    // Get Real Output Timing (unit: Tri-Byte)
    // ---------------------------------------------------
    usVtotal = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usVTotal;

    if(GET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(_TX0) != _DOWN_STREAM_DSC)
    {
#if 0 // new from MNT driver
        if(g_pstHdmi21MacTxDPFCompensateInfo[_TX0]->b1TimingCompensationFlag == _TRUE)
        {
            usHtotal = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal - g_pstHdmi21MacTxDPFCompensateInfo[_TX0]->b6HDirectionCompensate;
            usHactive = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth;
        }
        else
#endif
        {
            usHtotal = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHTotal;
            usHactive = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth;
        }

        // Deep color factor @ VESA Timing
        usHtotal = (UINT16)usHtotal * GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) / 8;
        usHactive = (UINT16)usHactive * GET_HDMI21_MAC_TX_INPUT_COLOR_DEPTH(_TX0) / 8;

        // 420 Factor @ VESA Timing
#ifdef SCALER_SUPPORT_420_CONFIG
        if((ScalerDpStreamGetElement(ScalerHdmi21MacTxPxMapping(_TX0), _COLOR_SPACE) != _COLOR_SPACE_YCBCR420) && (GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR420))
#else
        if(GET_HDMI21_MAC_TX_INPUT_COLOR_SPACE(_TX0) == _COLOR_SPACE_YCBCR420)
#endif
        {
            usHtotal = (usHtotal / 2);
            usHactive = (usHactive / 2);
        }

        if(GET_HDMI21_MAC_TX_INPUT_INTERLACE(_TX0) == _TRUE)
        {
            if(ScalerDpStreamGetInterlaceVttEven(ScalerHdmi21MacTxPxMapping(_TX0)) == _TRUE) // Even
            {
                usVtotal = (usVtotal * 2);
            }
            else
            {
                usVtotal = (usVtotal * 2 + 1);
            }
        }
    }
    else
    {
#if(_HDMI_TX_DSC_ENCODER_SUPPORT == _ON)
        if(GET_HDMI21_MAC_TX_INPUT_FROM(_TX0) == _INPUT_FROM_ENCODER)
        {
            usHtotal = g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCactive + g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCblank;
            usHactive = g_pstHdmi21MacTxHCTiminginfo[_TX0].usHCactive;
        }
        else
#endif
        {
            usHStart = (((rtd_inl(HDMITX21_MAC_HDMI21_IN_CONV_18_reg) & 0x0F) << 8) + (rtd_inl(HDMITX21_MAC_HDMI21_IN_CONV_19_reg) & 0xFF)) * 4;
            usHactive = g_pstHdmi21MacTxInputTimingInfo[_TX0]->usHWidth * 2 / 3 * 4;
            usHtotal = usHactive + (_HDMI21_DSC_DPF_HFRONT * 4) + usHStart;
        }
    }


    // ---------------------------------------------------
    // Calculate sub-packet # per packet
    // ---------------------------------------------------
    // Cal Audio BW with continues Pkts in Spec view
    ulAudioPktNeedTransmitPerLine = (((((UINT16)usAudioSampleFreq * 100 * usAP) / 100 + (2 * usACRFreq * 100)) / GET_HDMI21_MAC_TX_INPUT_FRAME_RATE(_TX0)) / usVtotal + 1);

    // VGB + Preamble + CTRL = 14; DI GB + Preamble + CTRL + HW dummy cycle (incoming CTRL after DI) = 17
    usMaxPktCanTransmitPerLine = ((((usHtotal - usHactive) * (GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) + 1)) > (ucReKeyWinSize + 14 + 17)) ?
                                  (((usHtotal - usHactive) * (GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) + 1) - ucReKeyWinSize - 14 - 17) / 32) : 0);

    ulAudioSampleNumPerLine = ((((UINT16)usAudioSampleFreq * 100) / GET_HDMI21_MAC_TX_INPUT_FRAME_RATE(_TX0)) / usVtotal + 1); // Unit: sample

    // Set sp# & Audio BW re-check
    if(ulAudioPktNeedTransmitPerLine > usMaxPktCanTransmitPerLine)
        {
            // Audio Error
            SET_HDMI21_MAC_TX_AUDIO_BW_ERROR(_TX0);
        DebugMessageHDMI21Tx("[HDMI21_TX] Audio BW ERROR by online check !!\n");
        }
        else
        {
        DebugMessageHDMI21Tx("[HDMI21_TX] Audio BW Pass by online check\n");
            CLR_HDMI21_MAC_TX_AUDIO_BW_ERROR(_TX0);
        }

    // sp# = 4 when ch>2's LPCM format
    if((bLpcmAudio == _TRUE) && ((ucAudioCHnum > 2) && (ucAudioCHnum <= 16)))
    {
        // In 8ch layout & 3D audio case & "HBR DP RX return 8ch case"
        // HW may auto sel the sp#, FW fill the max Value
        ucAudioSampleNuminOnePacket = 4;
    }
    else // N-PCM or 2ch LPCM
    {
        if(usMaxPktCanTransmitPerLine != 0)
        {
            if(ulAudioSampleNumPerLine % usMaxPktCanTransmitPerLine != 0)
            {
                ucAudioSampleNuminOnePacket = (ulAudioSampleNumPerLine / usMaxPktCanTransmitPerLine) + 1;
        }
        else
        {
                ucAudioSampleNuminOnePacket = ulAudioSampleNumPerLine / usMaxPktCanTransmitPerLine;
        }
        }
        else
        {
            // if no Packet can be transmit --> sp_pst of an ASP is Unlimited!!(set 4 sp in an ASP)
            ucAudioSampleNuminOnePacket = 4;
        }
    }

    FatalMessageHDMITx("[HDMITX]AudSp NumPerLine=%d\n", (UINT32)ulAudioSampleNumPerLine);
    FatalMessageHDMITx("[HDMITX]AudSp ForActual=%d\n", (UINT32)usMaxPktCanTransmitPerLine);
    FatalMessageHDMITx("[HDMITX]AudSp OnePacket=%d\n", (UINT32)ucAudioSampleNuminOnePacket);

    return ucAudioSampleNuminOnePacket;
}

//--------------------------------------------------
// Description  : Hdmi Tx Audio Sample Packet Proc
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTx0AudioSamplePktProc(UINT8 ucAudioSampleNuminOnePacket)
{
#if(_HW_HDMI_AUDIO_STRUCT == _HDMI_AUDIO_GEN_2)
    // Audio SRAM Input Fix to 1 sp & Clr sp_pst_fw_md
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_4_reg, ~(_BIT5 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
#endif

    switch(ScalerHdmi21MacTxGetAudioFormat(ScalerHdmi21MacTxPxMapping(_TX0), _AUDIO_CODING_TYPE))
    {
        case _LPCM_ASP:

            // Audio Packet Header to ASP
            rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_1_reg, 0x02);

            // Set Max Continue Pkt Size to 1
            rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
            rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), _BIT3);

            // 8 ch
            if(GET_HDMI21_MAC_TX_AUDIO_CH_NUM(_TX0) > 2)
            {
                // Layout = 1
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_0_reg, ~_BIT3, _BIT3);

                // Audio Pkt: Fix mode
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_3_reg, ~_BIT2, 0x00);

                // DP Audio Data Format: 8CH
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_1_reg, ~(_BIT2 | _BIT1 | _BIT0), _BIT0);
            }
            else // 2 ch
            {
                // Layout = 0
                // Set sp & b frame HW mode
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_0_reg, ~(_BIT5 | _BIT4 | _BIT3), 0x00);
                rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_2_reg,  0x00);

#if(_DEFAULT_AUDIO_PKT_MODE == _AUD_FIX_MODE)
                // Set Aud Pkt Mode: Fix mode
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_3_reg, ~_BIT2, 0x00);
#endif
                // DP Audio Data Format: 2ch
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_1_reg, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
            }

            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_4_reg, ~(_BIT5), HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_4_sp_pst_fw_md(0x1));
            // Set sp# for an ASP

#if(_HW_HDMI_AUDIO_STRUCT == _HDMI_AUDIO_GEN_1)
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_4_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (((ucAudioSampleNuminOnePacket - 1) << 2) | (ucAudioSampleNuminOnePacket - 1)));
#else
            if(rtd_getbits(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_3_reg, _BIT2) == 0x00)
            {
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_3_reg, ~(_BIT1 | _BIT0), (ucAudioSampleNuminOnePacket - 1));
            }
#endif
            break;

#if(_HDMI21_3D_AUDIO_TX0_SUPPORT == _ON)
        case _3D_LPCM_ASP:

            // Audio Packet Header to 3D ASP
            rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_1_reg, 0x0B);

            // Audio Pkt: Fix mode
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_3_reg, ~_BIT2, 0x00);

            if(GET_HDMI21_MAC_TX_AUDIO_CH_NUM(_TX0) <= 12)
            {
                // Set Max Continue Pkt Size to 2
                rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
                rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), _BIT3);

                // DP Audio Data Format: 12 ch
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_1_reg, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);
            }
            else if(GET_HDMI21_MAC_TX_AUDIO_CH_NUM(_TX0) <= 16)
            {
                // Set Max Continue Pkt Size to 2
                rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
                rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), _BIT3);

                // DP Audio Data Format: 16 ch
                rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_1_reg, ~(_BIT2 | _BIT1 | _BIT0), (_BIT1 | _BIT0));
            }

            break;
#endif

#if(_HDMI21_HBR_AUDIO_TX0_SUPPORT == _ON)
        case _HBR_ASP:

            // Audio Packet Header to HBR ASP
            rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_1_reg, 0x09);

            // Set sp & b frame HW mode //according spec HBR mode should set b frame 0
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_0_reg, ~(_BIT5 | _BIT4 | _BIT3), 0x00);
            rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_2_reg, 0x00);

            // Set Max Continue Pkt Size to 1
            rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
            rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), 0x00);

            // DP Audio Data Format: 8 ch (Current DP Rx use 4 spdif to TX Mac for HBR Audio)
            //  HBR in SPDIF 2ch mode
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_1_reg, ~(_BIT2 | _BIT1 | _BIT0), 0);
            DebugMessageHDMI21Tx("[HBR] 2CH!\n");

            // Audio Pkt: Fix mode & 4 sp# out of Aud SRAM
            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_3_reg, ~(_BIT2 | _BIT1 | _BIT0), (_BIT1 | _BIT0));

            break;
#endif

        default:
            break;
    }
}

//--------------------------------------------------
// Description  : Hdmi Tx ACR Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTx0ACRPacket(UINT8 audFmtChange)
{
    UINT16 usAudioSampleFreq = 480; // Unit:0.1KHz
    UINT32 ulHDMITxACRNcode = 0;
    UINT16 usRbit = 0;
    UINT8 ucFreqIndex = 0;
    UINT8 ucLinkRateIndex = 0;

    UINT32 ulHDMIRefCTScode = 0;
#if(_HDMI_TX_ACR_CLAMP_SUPPORT == _ON)
    UINT32 ulHDMIHiBondCTScode = 0;
    UINT32 ulHDMILowBondCTScode = 0;
#endif

    switch(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0))
    {
        case _AUDIO_FREQ_32K:
        case _AUDIO_FREQ_44_1K:
        case _AUDIO_FREQ_48K:

            ucFreqIndex = 0;
            break;

        case _AUDIO_FREQ_64K:
        case _AUDIO_FREQ_88_2K:
        case _AUDIO_FREQ_96K:

            ucFreqIndex = 1;
            break;

        case _AUDIO_FREQ_128K:
        case _AUDIO_FREQ_176_4K:
        case _AUDIO_FREQ_192K:

            ucFreqIndex = 2;
            break;

        case _AUDIO_FREQ_256K:
        case _AUDIO_FREQ_352_8K:
        case _AUDIO_FREQ_384K:

            ucFreqIndex = 3;
            break;

        case _AUDIO_FREQ_512K:
        case _AUDIO_FREQ_705_6K:
        case _AUDIO_FREQ_768K:

            ucFreqIndex = 4;
            break;

        default:
            break;
    }

    switch(GET_HDMI21_MAC_TX_FRL_RATE(_TX0))
    {
        case _HDMI21_FRL_3G:

            ucLinkRateIndex = 0;
            usRbit = (UINT16)(30000 / 18);
            break;

        case _HDMI21_FRL_6G_3LANES:
        case _HDMI21_FRL_6G_4LANES:

            ucLinkRateIndex = 1;
            usRbit = (UINT16)(60000 / 18);
            break;

        case _HDMI21_FRL_8G:

            ucLinkRateIndex = 2;
            usRbit = (UINT16)(80000 / 18);
            break;

        case _HDMI21_FRL_10G:

            ucLinkRateIndex = 3;
            usRbit = (UINT16)(100000 / 18);
            break;

        case _HDMI21_FRL_12G:

            ucLinkRateIndex = 4;
            usRbit = (UINT16)(120000 / 18);
            break;

        default:
            break;
    }

    switch(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0))
    {
        case _AUDIO_FREQ_32K:
        case _AUDIO_FREQ_64K:
        case _AUDIO_FREQ_128K:

            ulHDMITxACRNcode = tHDMI21_TX_AUDIO_32K_MULTI_N_CTS[ucLinkRateIndex * 7 + ucFreqIndex];
            ulHDMIRefCTScode = tHDMI21_TX_AUDIO_32K_MULTI_N_CTS[ucLinkRateIndex * 7 + 6];

            if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) == _AUDIO_FREQ_128K)
            {
                usAudioSampleFreq = 1280;
            }
            else if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) == _AUDIO_FREQ_64K)
            {
                usAudioSampleFreq = 640;
            }
            else
            {
                usAudioSampleFreq = 320;
            }

            break;

        case _AUDIO_FREQ_44_1K:
        case _AUDIO_FREQ_88_2K:
        case _AUDIO_FREQ_176_4K:

            ulHDMITxACRNcode = tHDMI21_TX_AUDIO_44_1K_MULTI_N_CTS[ucLinkRateIndex * 7 + ucFreqIndex];
            ulHDMIRefCTScode = tHDMI21_TX_AUDIO_44_1K_MULTI_N_CTS[ucLinkRateIndex * 7 + 6];

            if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) == _AUDIO_FREQ_176_4K)
            {
                usAudioSampleFreq = 1764;
            }
            else if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) == _AUDIO_FREQ_88_2K)
            {
                usAudioSampleFreq = 882;
            }
            else
            {
                usAudioSampleFreq = 441;
            }
            break;

        case _AUDIO_FREQ_48K:
        case _AUDIO_FREQ_96K:
        case _AUDIO_FREQ_192K:

            ulHDMITxACRNcode = tHDMI21_TX_AUDIO_48K_MULTI_N_CTS[ucLinkRateIndex * 7 + ucFreqIndex];
            ulHDMIRefCTScode = tHDMI21_TX_AUDIO_48K_MULTI_N_CTS[ucLinkRateIndex * 7 + 6];

            if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) == _AUDIO_FREQ_192K)
            {
                usAudioSampleFreq = 1920;
            }
            else if(GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0) == _AUDIO_FREQ_96K)
            {
                usAudioSampleFreq = 960;
            }
            else
            {
                usAudioSampleFreq = 480;
            }
            break;

        default:
            break;
    }

    // when N is ODD, need correct to EVEN
    if(ulHDMITxACRNcode % 2) {
        ulHDMITxACRNcode = ulHDMITxACRNcode << 1;
    }

    if(audFmtChange|| (GET_HDMI_MAC_TX0_STREAM_SOURCE() == _HDMI_TX_SOURCE_NONE))
    {
        FatalMessageHDMITx("[HDMI21_TX] Audio Freq Type=%d/", (UINT32)AudioFreqType);
        FatalMessageHDMITx("%d/,", (UINT32)GET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0));
        FatalMessageHDMITx("Sample/Ncode/CTS/usRbit=%d/", (UINT32)usAudioSampleFreq);
        FatalMessageHDMITx("%d/", (UINT32)ulHDMITxACRNcode);
        FatalMessageHDMITx("%d/%d\n", (UINT32)ulHDMIRefCTScode,usRbit);
    }

    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_7_reg, (UINT8)(ulHDMITxACRNcode)); // [7:0]
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_8_reg, (UINT8)(ulHDMITxACRNcode >> 8)); // [15:8] -> [7:0]
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_9_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), ((UINT8)(ulHDMITxACRNcode >> 12) & 0xF0)); // [19:16] -> [7:4]

#if(_HDMI_TX_ACR_CLAMP_SUPPORT == _ON)
    // Set the Upper/ Lower CTS Value of ACR
    ulHDMIHiBondCTScode = ulHDMIRefCTScode + (ulHDMIRefCTScode / 10);
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_16_reg, LOBYTE(LOWORD(ulHDMIHiBondCTScode)));
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_17_reg, HIBYTE(LOWORD(ulHDMIHiBondCTScode)));
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_18_reg, (LOBYTE(HIWORD(ulHDMIHiBondCTScode)) & 0x0F) << 4);

    // Cal Hi/ Low boundary value ( 10%)
    ulHDMILowBondCTScode = ulHDMIRefCTScode - (ulHDMIRefCTScode / 10);
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_19_reg, LOBYTE(LOWORD(ulHDMILowBondCTScode)));
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_20_reg, HIBYTE(LOWORD(ulHDMILowBondCTScode)));
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_21_reg, (LOBYTE(HIWORD(ulHDMILowBondCTScode)) & 0x0F) << 4);

    // Enable Clamp Function
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_15_reg, ~_BIT7, _BIT7);
#endif

#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
    UINT32 g_ulHdmi21MacTx0FwCTSVaule = 0;

    if(GET_HDMI21_MAC_TX0_CTS_CODE_FW_MODE() == _TRUE)
    {

        // FW calucate CTS value
        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_reg, ~_BIT3, _BIT3);
        // CTS = TMDS_Clk * N / (128 * fs)
        g_ulHdmi21MacTx0FwCTSVaule = (UINT32)(ulHDMITxACRNcode * usRbit);

        // / 128 * 1000 = / 16 * 125
        g_ulHdmi21MacTx0FwCTSVaule = (UINT32)(g_ulHdmi21MacTx0FwCTSVaule / 16);
        g_ulHdmi21MacTx0FwCTSVaule = (UINT32)(g_ulHdmi21MacTx0FwCTSVaule  * 125);
        g_ulHdmi21MacTx0FwCTSVaule = (UINT32)(g_ulHdmi21MacTx0FwCTSVaule / usAudioSampleFreq);

        rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_11_reg, (UINT8)(g_ulHdmi21MacTx0FwCTSVaule >> 12)); // [19:12] -> [7:0]
        rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_10_reg, (UINT8)(g_ulHdmi21MacTx0FwCTSVaule >> 4)); // [11:4] -> [7:0]
        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_9_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), ((UINT8)(g_ulHdmi21MacTx0FwCTSVaule) & 0x0F)); // [3:0]

        DebugMessageHDMI21Tx("[HDMI21_TX] CTScode=%d\n", (UINT32)g_ulHdmi21MacTx0FwCTSVaule);

    }
    else
    {
        // acrp_cts_source = 0  HW auto calucate CTS value
        rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_reg, ~_BIT3, 0x00);
    }
#else
    // acrp_cts_source = 0  HW auto calucate CTS value
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_reg, ~_BIT3, 0x00);
#endif
}

//--------------------------------------------------
// Description  : HDMI2.1 Tx Audio Set
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmi21MacTx0AudioPowerOff(void)
{
    // Set hdmi_tx_spdif_en = 0, No SPDIF Audio Format to HDMI Audio Packet
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_0_reg, ~_BIT4, 0x00);

    // Diable Audio FIFO
    CLR_HDMI21_MAC_TX0_AUDIO_FIFO_ENABLE();

    // Set Global to Initial
    SET_HDMI21_MAC_TX_AUDIO_FREQ(_TX0, 0);
    SET_HDMI21_MAC_TX_AUDIO_CH_NUM(_TX0, 0);
    CLR_HDMI21_MAC_TX_AUDIO_BW_ERROR(_TX0);

    DebugMessageHDMITx("[HDMI21_TX]  Audio Reset@%d\n", __LINE__);
}

#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
//--------------------------------------------------
// Description  : HDMI2.1 Tx FW CTS Mode Handler
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmi21MacTx0CtsCodeHandler(void)
{
}
#endif
#endif

//--------------------------------------------------
// Description  : HDMI Tx Fine Tune
// Input Value  : OutputPort
// Output Value :
//--------------------------------------------------
void ScalerHdmi21MacTx0PktFineTune(void)
{
#if(_AUDIO_SUPPORT == _ON)
    if(GET_HDMITX_AUDIO_INPUT_TYPE() == _TX_AUDIO_IN_RX_SPDIF)
        ScalerHdmi21MacTx0AudioSet();

  #if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
    ScalerHdmi21MacTx0CtsCodeHandler();
  #endif
#endif // #if(_AUDIO_SUPPORT == _ON)
#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
    ScalerHdmi21MacTx0HDRSet();
#endif
    ScalerHdmi21MacTx0EmpPacket();
    if(need_sw_avi_packet){
        ScalerHdmiMacTx0CheckAviUpdate(TC_HDMI21);
    }
#ifdef CONFIG_ENABLE_ZEBU_BRING_UP 
    if(rtd_inl(0xB8028304) & _BIT16){
        // 0: RGB, 2: YUV444
        EnumColorSpace colorFmt = rtd_inl(0xB8028304) & 0x3;

        msleep(3000);

        NoteMessageHDMI21Tx("[HDMI21_TX] Set AVI COLOR SPACE=%d\n", colorFmt);
        ScalerHdmi21MacTx0SetAviInfoFramePkt(colorFmt);
        rtd_clearbits(0xB8028304, _BIT16);
    }
#endif

}

//--------------------------------------------------
// Description  : HDMI2.1 Tx Reset
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmi21MacTx0Reset(void)
{
    CLR_HDMI21_MAC_TX0_SW_PKT_DFF0_USED();
    CLR_HDMI21_MAC_TX0_SW_PKT_DFF1_USED();

    // switch to out put GAP
    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_19_reg, ~(_BIT7 | _BIT6), _BIT7);

#if(_HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING == _ON)
    if(ScalerHdmiTxGetFrlNewModeEnable()  && ScalerHdmiTxGetFastTimingSwitchEn())
#else
     if(ScalerHdmiTxGetFrlNewModeEnable())
#endif
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~_BIT7, 0);

    // Disable the transmission of Periodic RAM0-7
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, 0x00);

    // Disable the transmission of Periodic RAM8-9
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_41_reg, 0x00);

    // Disable EMP PPS ,must set bypass mode
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PPS_CTRL_reg, ~(_BIT7 | _BIT6 | _BIT5), _BIT5);

    // RC disable
    rtd_maskl(HDMITX21_MAC_HDMI21_RC_reg, ~_BIT7, 0x00);

    // reset Disable HDCP2 RC
    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_7_reg, ~_BIT1, 0);

    // Disable HDCP2 RC
    if(Scaler_HDMITX_HDCP_get_DisableHdcp22Rc() == 1)
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_7_reg, ~_BIT1, _BIT1);
    InfoMessageHDMITx("[HDMI21_TX] DisableHdcp22Rc()=%d\n", (UINT32)Scaler_HDMITX_HDCP_get_DisableHdcp22Rc());

    // Scheduler disable
    rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~_BIT7, 0x00);

    InfoMessageHDMITx("[HDMI21_TX] Mac Reset\n");
    //set HDR flag to false
    ScalerHdmiTxSetHDRFlag(FALSE);

#if(_AUDIO_SUPPORT == _ON)
    // Hdmi 2.1 Audio Moudle Top Reset
    ScalerHdmi21MacTx0AudioPowerOff();

    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_3_reg, ~_BIT3, _BIT3);
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_3_reg, ~_BIT3, 0x00);

#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)  // TODO: Add H2.1 Mac FW Tracking !!!
    CLR_HDMI21_MAC_TX0_CTS_CODE_CHECK_EVENT();

    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_AUDIO_CTS_CODE_CHECK);

    CLR_HDMI21_MAC_TX0_CTS_CODE_FW_MODE();
#endif
#endif // #if(_AUDIO_SUPPORT == _ON)
}

//--------------------------------------------------
// Description  : HDMI2.1 Tx Reset
// Input Value  : bBorrowEnable
// Output Value : None
//--------------------------------------------------
void ScalerHdmi21MacTx0FRLReset(BOOLEAN bBorrowEnable)
{
    // reset FRL
    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~_BIT6, _BIT6);

    if(bBorrowEnable == _TRUE)
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_20_reg, ~(_BIT7 | _BIT5), (_BIT7 | _BIT5));
    }
    else
    {
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_20_reg, ~(_BIT7 | _BIT5), _BIT5);
    }

    HDMITX_MDELAY(2);

    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~_BIT6, 0x00);
}

//--------------------------------------------------
// Description  : HDMI2.1 Tx Initial
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmi21MacTx0Initial(void)
{

    SET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0, 0);
//#if(_HDMI21_HDCP2_TX_SUPPORT == _ON)
    // Set Rekey window size = 12,but 4k144 rbv2 set 0
    if(GET_HDMI_4K144_RBV2_FLAG()) {
        rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_7_reg, 0x00);
        SET_HDMI21_MAC_TX_HDCP_REKEY_WIN_SIZE(_TX0, 0x00 + 0x01);
    } else {
        rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_7_reg, 0x0B);
        SET_HDMI21_MAC_TX_HDCP_REKEY_WIN_SIZE(_TX0, 0x0B + 0x01);
    }

    // HDCP compatibility, (default 0xCA) - 0x20
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_9_reg, 0xAA);

    // HDCP Bug
    rtd_maskl(HDMITX21_MAC_HDCP22_ENGINE_40_reg, ~_BIT0, _BIT0);
    rtd_maskl(HDMITX21_MAC_HDCP22_ENGINE_40_reg, ~_BIT1, _BIT1);

    // Set Initial of HDCP opportunity window (FW Sol for mismatch default value between Hdmi2.0 & 2.1 Mac)
    // Align Hdmi2.0 Default Value
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_14_reg, 0x01);
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_15_reg, 0xFE);
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_17_reg, 0x0E);
//#endif

    // Set Sp_pst based on sp_pst_blank and sp_pst_active Reg
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_5_reg, ~_BIT5, 0x00);

    // Disable ACR Packet
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_reg, ~_BIT4, _BIT4);

    return;
}

#ifdef NOT_USED_NOW
//--------------------------------------------------
// Description  : H5X HDMI Tx DSC source config
// Input Value  : Hdmitx dsc input source: 0x0: TXSOC, 0x1: DISPD
// Output Value :
//--------------------------------------------------
void ScalerHdmiTx0_Dsc_inputSrcConfig(EnumHDMITXDscSrc dsc_src)
{
    hdmitx_phy_dsc_src_sel_RBUS dsce_misc_dsc_src_sel_reg;
    hdmitx_phy_dsc_insel_fifo_status_RBUS dsce_misc_dsc_insel_fifo_status_reg;
    UINT32 timeout = 50;

    dsce_misc_dsc_src_sel_reg.regValue = rtd_inl(HDMITX_PHY_DSC_SRC_SEL_reg);
    if(dsc_src == DSC_SRC_DISPD){
        dsce_misc_dsc_src_sel_reg.hdmitx_dsc_source_sel = 0; // switch to TXSOC before reset DISPD FIFO
        dsce_misc_dsc_src_sel_reg.dispd_fifo_en = 0;
        rtd_outl(HDMITX_PHY_DSC_SRC_SEL_reg, dsce_misc_dsc_src_sel_reg.regValue);

        dsce_misc_dsc_src_sel_reg.dispd_fifo_reset = 1; // reset DISPD FIFO
        rtd_outl(HDMITX_PHY_DSC_SRC_SEL_reg, dsce_misc_dsc_src_sel_reg.regValue);

        while(rtd_inl(HDMITX_PHY_DSC_SRC_SEL_reg) & HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_reset_mask){
            HDMITX_MDELAY(1);
            if(--timeout == 0)
                break;
        }
        DebugMessageHDMITx("[HDMITX] DISP FIFO RESET CNT[%d]\n", timeout);

        // clear DISP FIFO error status
        dsce_misc_dsc_insel_fifo_status_reg.regValue = 0;
        dsce_misc_dsc_insel_fifo_status_reg.dispd_fifo_uf = 1; // wclr_out
        dsce_misc_dsc_insel_fifo_status_reg.dispd_fifo_of = 1; // wclr_out
        rtd_outl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg, dsce_misc_dsc_insel_fifo_status_reg.regValue);
    }
    dsce_misc_dsc_src_sel_reg.dispd_fifo_en = dsc_src; // Function enable for DISPD to HDMITX FIFO
    dsce_misc_dsc_src_sel_reg.hdmitx_dsc_source_sel = dsc_src; // Decide Hdmitx dsc input source: 0x0: TXSOC, 0x1: DISPD
    rtd_outl(HDMITX_PHY_DSC_SRC_SEL_reg, dsce_misc_dsc_src_sel_reg.regValue);

    //NoteMessageHDMITx("[HDMITX] hdmitx_dsc_source_sel=%d@ScalerHdmiTx0_Dsc_inputSrcConfig\n", DSCE_MISC_DSC_SRC_SEL_get_hdmitx_dsc_source_sel(rtd_inl(HDMITX_PHY_DSC_SRC_SEL_reg)));
    return;
}
#endif // #ifdef NOT_USED_NOW

//--------------------------------------------------
// Description  : Hdmi decision BW ok or not
// Input Value  : port
// Output Value : BW status
//--------------------------------------------------
UINT8 ScalerHdmi21MacTxBwDecision(EnumOutputPort enumOutputPort)
{
    switch(ScalerHdmi21MacTxPxTxMapping(enumOutputPort))
    {
#if(_HDMI21_MAC_TX0_SUPPORT == _ON)

        case _TX0:

            return ScalerHdmi21MacTx0BwDecision();
#endif

#if(_HDMI21_MAC_TX1_SUPPORT == _ON)

        case _TX1:

            return ScalerHdmi21MacTx1BwDecision();
#endif

#if(_HDMI21_MAC_TX2_SUPPORT == _ON)

        case _TX2:

            return ScalerHdmi21MacTx2BwDecision();
#endif

        default:
            break;
    }
    return _FALSE;
}


#endif

