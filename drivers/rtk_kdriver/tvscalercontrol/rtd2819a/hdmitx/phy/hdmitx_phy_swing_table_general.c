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

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include "../include/hdmitx_phy_struct_def.h"

#ifdef HDMITX_TIMING_TABLE_CONFIG
hdmi_tx_swing_st hdmi_tx_swing_dpc_8bit_table_tv001[TX_TIMING_NUM+1] = {
	/*
	 * Discription : hdmi tx swing for 8bit deep color
	 * Table format : {{set1}, {set2}, {set3}, {set4}, ffeTableIndex}
	 *  	{set1}	: r {em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
	 *  	{set2}	: g {em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
	 *  	{set3}	: b {em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
	 *  	{set4}	: ck{em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
        *   ffeTableIndex
	 * Table index  :
	 *      TX_TIMING_NUM : see TX_TIMING_INDEX
	 */
#if 1 //#ifdef FIX_ME_HDMITX_BRING_UP
       {							// default
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
    // HDMI20 ----------------------------------------------------------------
      {							// 640X480p60, 27M  HDMI_TX_640_480P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X480p59, 27M  HDMI_TX_720_480P_59HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X480p60, 27M  HDMI_TX_720_480P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X576p50, 27M  HDMI_TX_720_576P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p50, 74.5M  HDMI_TX_1280_720P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p59, 74.5M  HDMI_TX_1280_720P_59HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p60, 74.5M  HDMI_TX_1280_720P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p24, 74.5M  HDMI_TX_1920_1080P_24HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p25, 74.5M  HDMI_TX_1920_1080P_25HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p29, 74.5M  HDMI_TX_1920_1080P_29HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p30, 74.2M  HDMI_TX_1920_1080P_30HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p50, 148.5M  HDMI_TX_1920_1080P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p59, 148.5M  HDMI_TX_1920_1080P_59HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p60, 148.5M  HDMI_TX_1920_1080P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1920x1080@120, 297M   HDMI_TX_1920_1080P_444_120HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// 1920x1080@120, 297M   HDMI_TX_1920_1080P_444_120HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1920x1080@240, 594M   HDMI_TX_1920_1080P_444_240HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// 2560x1080@59, 198M   HDMI_TX_2560_1080P_444_59HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 2560x1080@60, 198M   HDMI_TX_2560_1080P_444_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 2560x1080@120, 495M   HDMI_TX_2560_1080P_444_120HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// 2560x1080@144, 594M   HDMI_TX_2560_1080P_444_144HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// 2560x1440@59, 321.25M   HDMI_TX_2560_1440P_444_59HZ,
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 2560x1440@60, 321.25M   HDMI_TX_2560_1440P_444_60HZ,
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 2560x1440@120, 497M   HDMI_TX_2560_1440P_444_120HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// 2560x1440@144, 594M   HDMI_TX_2560_1440P_444_144HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// 3840x2160@24, 297M   HDMI_TX_3840_2160P_444_24HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@25, 297M   HDMI_TX_3840_2160P_444_25HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@30, 297M   HDMI_TX_3840_2160P_444_30HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@50, 594M   HDMI_TX_3840_2160P_444_50HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@59, 594M   HDMI_TX_3840_2160P_444_59HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@60, 594M   HDMI_TX_3840_2160P_444_60HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@50, 297M   HDMI_TX_3840_2160P_420_50HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@60, 297M   HDMI_TX_3840_2160P_420_60HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@120, 594M   HDMI_TX_3840_2160P_420_120HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X480i60, 27M  HDMI_TX_720_480I_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X576i50, 27M  HDMI_TX_720_576I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1440X480i60, 27M  HDMI_TX_1440_480I_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1440X576i50, 27M  HDMI_TX_1440_576I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080i50, 74.5M  HDMI_TX_1920_1080I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080i60, 74.5M  HDMI_TX_1920_1080I_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	// HDMI21 ----------------------------------------------------------------
	{							// HDMI21_TX_640_480P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_720_480P_59HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_720_480P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_720_576P_50HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1280_720P_50HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1280_720P_59HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1280_720P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_24HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_25HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_29HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_30HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_50HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_59HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_60HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_120HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
    /*
	{							// HDMI21_TX_1920_1080P_144HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_240HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// HDMI21_TX_2560_1080P_59HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_2560_1080P_60HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_2560_1080P_120HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// HDMI21_TX_2560_1080P_144HZ_8G
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_8G, 							// AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// HDMI21_TX_2560_1440P_59HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_2560_1440P_60HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_2560_1440P_120HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// HDMI21_TX_2560_1440P_144HZ_8G4L
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_8G, 							// AMP fine tune table index (0xff: not exist)
	},
    */
	{							// HDMI21_TX_3840_2160P_444_24HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_25HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_30HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_50HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_59HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_12G
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_12G, 							// AMP fine tune table index (0: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_120HZ_8bit_8G
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_8G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_144HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G, 							// AMP fine tune table index (0xff: not exist)
	},
//	 *  	{set1}	: r {em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
	{							// HDMI21_TX_3840_2160P_444_144HZ_12G
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_12G, 							// AMP fine tune table index (0: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_120HZ_6G
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},

#else // #ifdef FIX_ME_HDMITX_BRING_UP
        // HDMI20 ----------------------------------------------------------------
	{							// 640x480p60, 25.175M  HDMI_TX_640_480P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 480i60, 13.5M    HDMI_TX_720_480I_60HZ_13p5M,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 800x600@72, 50M  HDMI_TX_800_600P_72HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 480i60, 27M  HDMI_TX_720_1440_480I_60HZ
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 480p60, 27M  HDMI_TX_720_480P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 576i50, 27M  HDMI_TX_720_1440_576I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 576p50, 27M  HDMI_TX_720_576P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p60, 74.25M   HDMI_TX_1280_720P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080i50, 74.25M  HDMI_TX_1920_1080I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080i60, 74.25M  HDMI_TX_1920_1080I_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p24, 74.25M  HDMI_TX_1920_1080P_24HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p50, 74.25M   HDMI_TX_1280_720P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p48, 148.5M  HDMI_TX_1920_1080P_48HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p50, 148.5M  HDMI_TX_1920_1080P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p60, 148.5M  HDMI_TX_1920_1080P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@30, 297M   HDMI_TX_3840_2160P_444_30HZ,
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@60, 594M   HDMI_TX_3840_2160P_420_60HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@48, 594M   HDMI_TX_3840_2160P_444_48HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@50, 594M   HDMI_TX_3840_2160P_444_50HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@60, 594M   HDMI_TX_3840_2160P_444_60HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},

        // HDMI21 ----------------------------------------------------------------
	{							// HDMI21_TX_640_480P_60HZ
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_3G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_720_480P_60HZ
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_3G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1280_720P_60HZ
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_3G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_3G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_60HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_47HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_47HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_48HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_48HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_50HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_50HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_60HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_60HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_47HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_47HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_48HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_48HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_50HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_50HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_120HZ_8G
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_8G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_12G
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_12G,  // AMP fine tune table index (0: not exist)
	},
#endif // #ifdef FIX_ME_HDMITX_BRING_UP
	{							// SWING TABLE CHECK CODE
			{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},     // R channel
			{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},     // G channel
			{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},     //B channel
			{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},     // CLK channel
			0xff,  // AMP fine tune table index (0xff: not exist)
	},
};

hdmi_tx_swing_st hdmi_tx_swing_dpc_10bit_table_tv001[TX_TIMING_NUM+1] = {
	/*
	 * Discription : hdmi tx swing for 10bit deep color
	 * Table format : {{set1}, {set2}, {set3}, {set4}, ffeTableIndex}
	 *  	{set1}	: r {em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
	 *  	{set2}	: g {em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
	 *  	{set3}	: b {em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
	 *  	{set4}	: ck{em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
        *   ffeTableIndex
	 * Table index  :
	 *      TX_TIMING_NUM : see TX_TIMING_INDEX
	 */
#if 1 //#ifdef FIX_ME_HDMITX_BRING_UP
       {							// default
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	// HDMI20 ----------------------------------------------------------------
      {							// 640X480p60, 27M  HDMI_TX_640_480P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X480p59, 27M  HDMI_TX_720_480P_59HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X480p60, 27M  HDMI_TX_720_480P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X576p50, 27M  HDMI_TX_720_576P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p50, 74.5M  HDMI_TX_1280_720P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p59, 74.5M  HDMI_TX_1280_720P_59HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p60, 74.5M  HDMI_TX_1280_720P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p24, 74.5M  HDMI_TX_1920_1080P_24HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p25, 74.5M  HDMI_TX_1920_1080P_25HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p29, 74.5M  HDMI_TX_1920_1080P_29HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p30, 74.2M  HDMI_TX_1920_1080P_30HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p50, 148.5M  HDMI_TX_1920_1080P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p59, 148.5M  HDMI_TX_1920_1080P_59HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p60, 148.5M  HDMI_TX_1920_1080P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1920x1080@120, 297M   HDMI_TX_1920_1080P_444_120HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// 1920x1080@144, 297M   HDMI_TX_1920_1080P_444_144HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1920x1080@240, 594M   HDMI_TX_1920_1080P_444_240HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// 2560x1080@59, 198M   HDMI_TX_2560_1080P_444_59HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 2560x1080@60, 198M   HDMI_TX_2560_1080P_444_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 2560x1080@120, 495M   HDMI_TX_2560_1080P_444_120HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// 2560x1080@144, 594M   HDMI_TX_2560_1080P_444_144HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// 2560x1440@59, 321.25M   HDMI_TX_2560_1440P_444_59HZ,
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 2560x1440@60, 321.25M   HDMI_TX_2560_1440P_444_60HZ,
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 2560x1440@120, 497M   HDMI_TX_2560_1440P_444_120HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// 2560x1440@144, 594M   HDMI_TX_2560_1440P_444_144HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// 3840x2160@24, 297M   HDMI_TX_3840_2160P_444_24HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@25, 297M   HDMI_TX_3840_2160P_444_25HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@30, 297M   HDMI_TX_3840_2160P_444_30HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@50, 594M   HDMI_TX_3840_2160P_444_50HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@59, 594M   HDMI_TX_3840_2160P_444_59HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@60, 594M   HDMI_TX_3840_2160P_444_60HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@50, 297M   HDMI_TX_3840_2160P_420_50HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@60, 297M   HDMI_TX_3840_2160P_420_60HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@120, 594M   HDMI_TX_3840_2160P_420_120HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X480i60, 27M  HDMI_TX_720_480I_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X576i50, 27M  HDMI_TX_720_576I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1440X480i60, 27M  HDMI_TX_1440_480I_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1440X576i50, 27M  HDMI_TX_1440_576I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080i50, 74.5M  HDMI_TX_1920_1080I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080i60, 74.5M  HDMI_TX_1920_1080I_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},

	// HDMI21 ----------------------------------------------------------------
	{							// HDMI21_TX_640_480P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_720_480P_59HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_720_480P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_720_576P_50HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1280_720P_50HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1280_720P_59HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1280_720P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_24HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_25HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_29HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_30HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_50HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_59HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_60HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_120HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
    /*
	{							// HDMI21_TX_1920_1080P_144HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_240HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// HDMI21_TX_2560_1080P_59HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_2560_1080P_60HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_2560_1080P_120HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// HDMI21_TX_2560_1080P_144HZ_8G
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_8G, 							// AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// HDMI21_TX_2560_1440P_59HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_2560_1440P_60HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_2560_1440P_120HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// HDMI21_TX_2560_1440P_144HZ_8G4L
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_8G, 							// AMP fine tune table index (0xff: not exist)
	},
    */
	{							// HDMI21_TX_3840_2160P_444_24HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_25HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_30HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_50HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_59HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_12G
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_12G, 							// AMP fine tune table index (0: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_120HZ_10bit_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G, 	 							// AMP fine tune table index (0: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_144HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_144HZ_12G
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_12G, 							// AMP fine tune table index (0: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_120HZ_6G
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},

#else // #ifdef FIX_ME_HDMITX_BRING_UP
        // HDMI20 ---------------------------------------
	{							// 640x480p60, 25.175M  HDMI_TX_640_480P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 480i60, 13.5M    HDMI_TX_720_480I_60HZ_13p5M,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 800x600@72, 50M  HDMI_TX_800_600P_72HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 480i60, 27M  HDMI_TX_720_1440_480I_60HZ
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 480p60, 27M  HDMI_TX_720_480P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 576i50, 27M  HDMI_TX_720_1440_576I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 576p50, 27M  HDMI_TX_720_576P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p60, 74.25M   HDMI_TX_1280_720P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080i50, 74.25M  HDMI_TX_1920_1080I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080i60, 74.25M  HDMI_TX_1920_1080I_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p24, 74.25M  HDMI_TX_1920_1080P_24HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p50, 74.25M   HDMI_TX_1280_720P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p48, 148.5M  HDMI_TX_1920_1080P_48HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p50, 148.5M  HDMI_TX_1920_1080P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p60, 148.5M  HDMI_TX_1920_1080P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@30, 297M   HDMI_TX_3840_2160P_444_30HZ,
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@60, 594M   HDMI_TX_3840_2160P_420_60HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@48, 594M   HDMI_TX_3840_2160P_444_48HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@50, 594M   HDMI_TX_3840_2160P_444_50HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@60, 594M   HDMI_TX_3840_2160P_444_60HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},

        // HDMI21 ---------------------------------------
	{                           // HDMI21_TX_640_480P_60HZ ,
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_3G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_720_480P_60HZ,
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_3G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_1280_720P_60HZ
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_3G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_1920_1080P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_3G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_1920_1080P_60HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_3840_2160P_420_47HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_3840_2160P_420_47HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_3840_2160P_420_48HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_3840_2160P_420_48HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_3840_2160P_420_50HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_3840_2160P_420_50HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_3840_2160P_420_60HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_3840_2160P_420_60HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_3840_2160P_444_47HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_47HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{                           // HDMI21_TX_3840_2160P_444_48HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_48HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{                                           // HDMI21_TX_3840_2160P_444_50HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_50HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{                                           // HDMI21_TX_3840_2160P_444_60HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_120HZ_8G
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_8G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_12G
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_12G,  // AMP fine tune table index (0: not exist)
	},

#endif // #ifdef FIX_ME_HDMITX_BRING_UP
	{							// SWING TABLE CHECK CODE
			{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},     // R channel
			{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},     // G channel
			{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},     //B channel
			{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},     // CLK channel
			0xff,  // AMP fine tune table index (0xff: not exist)
	},
};

hdmi_tx_swing_st hdmi_tx_swing_dpc_12bit_table_tv001[TX_TIMING_NUM+1] = {
	/*
	 * Discription : hdmi tx swing for 12bit deep color
	 * Table format : {{set1}, {set2}, {set3}, {set4}, ffeTableIndex}
	 *  	{set1}	: r {em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
	 *  	{set2}	: g {em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
	 *  	{set3}	: b {em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
	 *  	{set4}	: ck{em_en, empre_en, rsel, idrv, iem, iempre, csel, slew, ipdrv, ipdrvem, ipdrvempre, tmds_csel, tmds_srem, tmds_srempre}
       *   ffeTableIndex
	 * Table index  :
	 *      TX_TIMING_NUM : see TX_TIMING_INDEX
	 */
#if 1 //#ifdef FIX_ME_HDMITX_BRING_UP
       {							// default
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	// HDMI20 ----------------------------------------------------------------
      {							// 640X480p60, 27M  HDMI_TX_640_480P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X480p59, 27M  HDMI_TX_720_480P_59HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X480p60, 27M  HDMI_TX_720_480P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X576p50, 27M  HDMI_TX_720_576P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p50, 74.5M  HDMI_TX_1280_720P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p59, 74.5M  HDMI_TX_1280_720P_59HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p60, 74.5M  HDMI_TX_1280_720P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p24, 74.5M  HDMI_TX_1920_1080P_24HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p25, 74.5M  HDMI_TX_1920_1080P_25HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p29, 74.5M  HDMI_TX_1920_1080P_29HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p30, 74.2M  HDMI_TX_1920_1080P_30HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p50, 148.5M  HDMI_TX_1920_1080P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p59, 148.5M  HDMI_TX_1920_1080P_59HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p60, 148.5M  HDMI_TX_1920_1080P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1920x1080@120, 297M   HDMI_TX_1920_1080P_444_120HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// 1920x1080@144, 297M   HDMI_TX_1920_1080P_444_144HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1920x1080@240, 594M   HDMI_TX_1920_1080P_444_240HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// 2560x1080@59, 198M   HDMI_TX_2560_1080P_444_59HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 2560x1080@60, 198M   HDMI_TX_2560_1080P_444_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 2560x1080@120, 495M   HDMI_TX_2560_1080P_444_120HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// 2560x1080@144, 594M   HDMI_TX_2560_1080P_444_144HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// 2560x1440@59, 321.25M   HDMI_TX_2560_1440P_444_59HZ,
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 2560x1440@60, 321.25M   HDMI_TX_2560_1440P_444_60HZ,
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 2560x1440@120, 497M   HDMI_TX_2560_1440P_444_120HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// 2560x1440@144, 594M   HDMI_TX_2560_1440P_444_144HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// 3840x2160@24, 297M   HDMI_TX_3840_2160P_444_24HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@25, 297M   HDMI_TX_3840_2160P_444_25HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@30, 297M   HDMI_TX_3840_2160P_444_30HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@50, 594M   HDMI_TX_3840_2160P_444_50HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@59, 594M   HDMI_TX_3840_2160P_444_59HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@60, 594M   HDMI_TX_3840_2160P_444_60HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@50, 297M   HDMI_TX_3840_2160P_420_50HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@60, 297M   HDMI_TX_3840_2160P_420_60HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@120, 594M   HDMI_TX_3840_2160P_420_120HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 1, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X480i60, 27M  HDMI_TX_720_480I_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720X576i50, 27M  HDMI_TX_720_576I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1440X480i60, 27M  HDMI_TX_1440_480I_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1440X576i50, 27M  HDMI_TX_1440_576I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080i50, 74.5M  HDMI_TX_1920_1080I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080i60, 74.5M  HDMI_TX_1920_1080I_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 1, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},

	// HDMI21 ----------------------------------------------------------------
	{							// HDMI21_TX_640_480P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_720_480P_59HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_720_480P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_720_576P_50HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1280_720P_50HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1280_720P_59HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1280_720P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_24HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_25HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_29HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_30HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_50HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_59HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_60HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_120HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
    /*
	{							// HDMI21_TX_1920_1080P_144HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_240HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// HDMI21_TX_2560_1080P_59HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_2560_1080P_60HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_2560_1080P_120HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// HDMI21_TX_2560_1080P_144HZ_8G
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_8G, 							// AMP fine tune table index (0xff: not exist)
	},
	*/
	{							// HDMI21_TX_2560_1440P_59HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_2560_1440P_60HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_2560_1440P_120HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	/*
	{							// HDMI21_TX_2560_1440P_144HZ_8G4L
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_8G, 							// AMP fine tune table index (0xff: not exist)
	},
    */
	{							// HDMI21_TX_3840_2160P_444_24HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_25HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_30HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_50HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_59HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_6G4L
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_12G
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_12G, 							// AMP fine tune table index (0: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_120HZ_12bit_12G
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_12G, 							// AMP fine tune table index (0: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_144HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_144HZ_12G
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_12G, 							// AMP fine tune table index (0: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_120HZ_6G
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{2, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)						// AMP fine tune table index (0xff: not exist)
	},

#else
        // HDMI20 ---------------------------------------
	{							// 640x480p60, 25.175M  HDMI_TX_640_480P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 480i60, 13.5M    HDMI_TX_720_480I_60HZ_13p5M,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 800x600@72, 50M  HDMI_TX_800_600P_72HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 480i60, 27M  HDMI_TX_720_1440_480I_60HZ
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 480p60, 27M  HDMI_TX_720_480P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 576i50, 27M  HDMI_TX_720_1440_576I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 576p50, 27M  HDMI_TX_720_576P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p60, 74.25M   HDMI_TX_1280_720P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080i50, 74.25M  HDMI_TX_1920_1080I_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080i60, 74.25M  HDMI_TX_1920_1080I_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p24, 74.25M  HDMI_TX_1920_1080P_24HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 720p50, 74.25M   HDMI_TX_1280_720P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p48, 148.5M  HDMI_TX_1920_1080P_48HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p50, 148.5M  HDMI_TX_1920_1080P_50HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 1080p60, 148.5M  HDMI_TX_1920_1080P_60HZ,
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@30, 297M   HDMI_TX_3840_2160P_444_30HZ,
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// R channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		// G channel
			{0, 0, 12, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},		//B channel
			{0, 0, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@60, 594M   HDMI_TX_3840_2160P_420_60HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@48, 594M   HDMI_TX_3840_2160P_444_48HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@50, 594M   HDMI_TX_3840_2160P_444_50HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							// 3840x2160@60, 594M   HDMI_TX_3840_2160P_444_60HZ,
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 3, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 3, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 2, 0, 0, 7, 7, 7, 3, 0, 0},		//B channel
			{1, 1, 12, 0x17, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},

        // HDMI21 ---------------------------------------
	{							// HDMI21_TX_640_480P_60HZ
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_3G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_720_480P_60HZ
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_3G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1280_720P_60HZ
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_3G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_60HZ_3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_3G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_1920_1080P_60HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_47HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_47HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_48HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_48HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_50HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_50HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_60HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_420_60HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_47HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_47HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_48HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_48HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_50HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_50HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_6G4L
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // R channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // G channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     //B channel
			{1, 1, 12, 0xa, 0, 0, 0, 0, 7, 7, 7, 3, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 1, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_120HZ_8G
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{4, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_8G,  // AMP fine tune table index (0xff: not exist)
	},
	{							// HDMI21_TX_3840_2160P_444_60HZ_12G
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 0xe, 0x12, 3, 3, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_12G,  // AMP fine tune table index (0: not exist)
	},
#endif // #ifdef FIX_ME_HDMITX_BRING_UP
	{							// SWING TABLE CHECK CODE
			{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},     // R channel
			{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},     // G channel
			{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},     //B channel
			{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},     // CLK channel
			0xff,  // AMP fine tune table index (0xff: not exist)
	},
};

// For TV001 H5D board
// [TV001] H5D board TX AMP fine tune table
hdmi_tx_amp_fine_tune_st hdmi_tx_FFE_table_TV001[HDMI_TX_AMP_FINE_TUNE_INDEX_NUM+1] = {
/*
 * Discription : hdmi tx amp fine tune table
 * Table format : {{r level0}}  , {r level1}    , {r level2}    , {r level3}}
 *                   : {{g level0}} , {g level1}    , {g level2}   , {g level3}}
 *                   : {{b level0}} , {b level1}    , {b level2}   , {b level3}}
 *                   : {{clk level0}}, {clk level1} , {clk level2} , {clk level3}}
 *  	{r}	: {idrv, iem, iempre}
 *  	{g}	: {idrv, iem, iempre}
 *  	{b}	: {idrv, iem, iempre}
 *  	{clk}: {idrv, iem, iempre}
 * Table index  :
 *      HDMI_TX_AMP_FINE_TUNE_INDEX_NUM : see HDMI_TX_AMP_FINE_TUNE_INDEX
 */
    {                                               // HDMI21_TX_FFE_3G
        {{0x12, 2, 2},{0x0e, 3, 3},{0x0a, 4, 4},{0x06, 5, 0x5}}, // R channel: level {0},{1},{2},{3}
        {{0x12, 2, 2},{0x0e, 3, 3},{0x0a, 4, 4},{0x06, 5, 0x5}}, // G channel: level {0},{1},{2},{3}
        {{0x12, 2, 2},{0x0e, 3, 3},{0x0a, 4, 4},{0x06, 5, 0x5}}, // B channel: level {0},{1},{2},{3}
        {{0x12, 3, 2},{0x0e, 3, 3},{0x0a, 4, 4},{0x06, 5, 0x5}}, // CLK channel: level {0},{1},{2},{3}
    },
    {                                               // HDMI21_TX_FFE_6G
        {{0x11, 2, 1},{0xd, 3, 2},{0x9, 4, 3},{0x5, 5, 0x4}}, // R channel: level {0},{1},{2},{3}
        {{0x11, 2, 1},{0xd, 3, 2},{0x9, 4, 3},{0x5, 5, 0x4}}, // G channel: level {0},{1},{2},{3}
        {{0x11, 2, 1},{0xd, 3, 2},{0x9, 4, 3},{0x5, 5, 0x4}}, // B channel: level {0},{1},{2},{3}
        {{0x11, 2, 1},{0xd, 3, 2},{0x9, 4, 3},{0x5, 5, 0x4}}, // CLK channel: level {0},{1},{2},{3}
    },
    {                                               // HDMI21_TX_FFE_8G
        {{0x11, 4, 2},{0xd, 5, 3},{0x9, 6, 4},{0x5, 7, 0x5}}, // R channel: level {0},{1},{2},{3}
        {{0x11, 4, 2},{0xd, 5, 3},{0x9, 6, 4},{0x5, 7, 0x5}}, // G channel: level {0},{1},{2},{3}
        {{0x11, 4, 2},{0xd, 5, 3},{0x9, 6, 4},{0x5, 7, 0x5}}, // B channel: level {0},{1},{2},{3}
        {{0x11, 4, 2},{0xd, 5, 3},{0x9, 6, 4},{0x5, 7, 0x5}}, // CLK channel: level {0},{1},{2},{3}
    },
    {                                               // HDMI21_TX_FFE_10G
        {{0x14, 4, 2},{0x10, 5, 3},{0x0c, 6, 4},{0x08, 7, 0x5}}, // R channel: level {0},{1},{2},{3}
        {{0x14, 4, 2},{0x10, 5, 3},{0x0c, 6, 4},{0x08, 7, 0x5}}, // G channel: level {0},{1},{2},{3}
        {{0x14, 4, 2},{0x10, 5, 3},{0x0c, 6, 4},{0x08, 7, 0x5}}, // B channel: level {0},{1},{2},{3}
        {{0x14, 4, 2},{0x10, 5, 3},{0x0c, 6, 4},{0x08, 7, 0x5}}, // CLK channel: level {0},{1},{2},{3}
    },
    {                                               // HDMI21_TX_FFE_12G
        {{0x12, 3, 3},{0x0e, 4, 4},{0x0a, 5, 5},{0x06, 6, 0x6}}, // R channel: level {0},{1},{2},{3}
        {{0x12, 3, 3},{0x0e, 4, 4},{0x0a, 5, 5},{0x06, 6, 0x6}}, // G channel: level {0},{1},{2},{3}
        {{0x12, 3, 3},{0x0e, 4, 4},{0x0a, 5, 5},{0x06, 6, 0x6}}, // B channel: level {0},{1},{2},{3}
        {{0x12, 3, 3},{0x0e, 4, 4},{0x0a, 5, 5},{0x06, 6, 0x6}}, // CLK channel: level {0},{1},{2},{3}
    },
    {                                               // FFE TABLE CHECK CODE
        {{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff}}, // R channel: level {0},{1},{2},{3}
        {{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff}}, // G channel: level {0},{1},{2},{3}
        {{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff}}, // B channel: level {0},{1},{2},{3}
        {{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff},{0xff, 0xff, 0xff}}, // CLK channel: level {0},{1},{2},{3}
    },
};
#endif

hdmi_tx_swing_st hdmi_tx_swing_hdmitx20_table[SWING_HDMI20_TABLE_NUM] = {
    {							//  74.5M
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 3, 0, 0},		// R channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 3, 0, 0},		// G channel
			{1, 1, 12, 0x10, 0, 0, 0, 0, 5, 5, 5, 3, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 5, 5, 5, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							//  148.5M
			{1, 1, 12, 0x0D, 1, 0, 0, 0, 1, 1, 1, 3, 0, 0},		// R channel
			{1, 1, 12, 0x0D, 1, 0, 0, 0, 1, 1, 1, 3, 0, 0},		// G channel
			{1, 1, 12, 0x0D, 1, 0, 0, 0, 1, 1, 1, 3, 0, 0},		//B channel
			{1, 1, 12, 0x0C, 0, 0, 0, 0, 1, 1, 1, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							//  297M
			{1, 1, 12, 0x17, 2, 0, 0, 0, 1, 1, 1, 3, 0, 0},		// R channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 1, 1, 1, 3, 0, 0},		// G channel
			{1, 1, 12, 0x17, 2, 0, 0, 0, 1, 1, 1, 3, 0, 0},		//B channel
			{1, 1, 12, 0x09, 0, 0, 0, 0, 1, 1, 1, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
	{							//  594M
			{1, 1, 12, 0x19, 0x0c, 0x0a, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 12, 0x19, 0x0c, 0x0a, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 12, 0x19, 0x0c, 0x0a, 0, 0, 0x1b, 0x1b, 0x1b, 0, 0, 0},		//B channel
			{1, 1, 12, 0x16, 0x0c, 0x0a, 0, 0, 0x1b, 0x1b, 0x1b, 3, 0, 0},		// CLK channel
			HDMI_TX_AMP_FINE_TUNE_INDEX_NONE,  // AMP fine tune table index (0xff: not exist)
	},
};

hdmi_tx_swing_st hdmi_tx_swing_hdmitx21_table[SWING_HDMI21_TABLE_NUM] = {
    {							// 3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// 6G3L
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// 6G4L
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// 8G
			{1, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 2, 2},		// R channel
			{1, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 2, 2},		// G channel
			{1, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 2, 2},		//B channel
			{1, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 2, 2},		// CLK channel
			HDMI21_TX_FFE_8G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// 10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G, 	 							// AMP fine tune table index (0: not exist)
	},
	{							// 12G
			{1, 1, 0xe, 0x13, 3, 5, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 0xe, 0x11, 3, 5, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 0xe, 0x12, 3, 5, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 0xe, 0x10, 3, 5, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_12G, 							// AMP fine tune table index (0: not exist)
	},
};

hdmi_tx_swing_st hdmi_tx_swing_hdmitx21_pcb002_table[SWING_HDMI21_TABLE_NUM] = { //for tvs pcb002
    {							// 3G
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x12, 2, 2, 0, 0, 7, 7, 7, 0, 2, 2},    							// CLK channel
			HDMI21_TX_FFE_3G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// 6G3L
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// 6G4L
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// R channel
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},    							// G channel
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     //B channel
			{1, 1, 12, 0x11, 2, 1, 0, 0, 7, 7, 7, 0, 2, 2},     // CLK channel
			HDMI21_TX_FFE_6G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// 8G
			{1, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 2, 2},		// R channel
			{1, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 2, 2},		// G channel
			{1, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 2, 2},		//B channel
			{1, 1, 0xc, 0x11, 4, 2, 0, 0, 7, 7, 7, 0, 2, 2},		// CLK channel
			HDMI21_TX_FFE_8G, 							// AMP fine tune table index (0xff: not exist)
	},
	{							// 10G
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 13, 0x14, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		//B channel
			{1, 1, 13, 0x16, 4, 2, 0, 0, 7, 7, 7, 0, 0, 0},		// CLK channel
			HDMI21_TX_FFE_10G, 	 							// AMP fine tune table index (0: not exist)
	},
	{							// 12G
			{1, 1, 0xe, 0x11, 3, 5, 0, 0, 7, 7, 7, 0, 0, 0},		// R channel
			{1, 1, 0xe, 0x11, 3, 5, 0, 0, 7, 7, 7, 0, 0, 0},		// G channel
			{1, 1, 0xe, 0x10, 3, 3, 0, 0, 7, 7, 7, 0, 3, 3},		//B channel
			{1, 1, 0xe, 0x10, 3, 3, 0, 0, 7, 7, 7, 0, 3, 3},		// CLK channel
			HDMI21_TX_FFE_12G, 							// AMP fine tune table index (0: not exist)
	},
};
