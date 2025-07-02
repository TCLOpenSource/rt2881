
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_lib.h>
#ifdef CONFIG_ENABLE_DPRX
#include <rtk_kdriver/dprx/drvif_dprx.h>
#endif
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
//#include <rtk_kdriver/tvscalercontrol/hdmirx/drvif_hdmirx.h>
#endif
static unsigned char hdmitxReNewIndex_in=0;

/************************
// table declaration
//
*************************/
/*
 * Description: HDMITX output timing info
 * Tab le format: {set1}
 * {set1} : {width, height, progress, frame_rate, color_type, color_depth, frl_type}
 * Table index  :
 *      TX_TIMING_NUM : see TX_TIMING_INDEX
 */
 #ifdef HDMITX_TIMING_TABLE_CONFIG
 hdmi_tx_output_timing_type tx_output_timing_table[TX_TIMING_NUM] = {
    {0,  0,   1, 0,   I3DDMA_COLOR_RGB,    I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE},//default
    {640,  480,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_640X480P60_RGB_8BIT
    {720,  480,   1, 599,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_720X480P59_RGB_8BIT
    {720,  480,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_720X480P60_RGB_8BIT
    {720,  576,   1, 500,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_720X576P50_RGB_8BIT
    {1280,  720,   1, 500,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_720P50_RGB_8BIT
    {1280,  720,   1, 599,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_720P59_RGB_8BIT
    {1280,  720,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_720P60_RGB_8BIT
    {1920,  1080,   1, 240,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1080P24_RGB_8BIT
    {1920,  1080,   1, 250,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1080P25_RGB_8BIT
    {1920,  1080,   1, 290,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1080P29_RGB_8BIT
    {1920,  1080,   1, 300,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1080P30_RGB_8BIT
    {1920,  1080,   1, 500,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1080P50_RGB_8BIT
    {1920,  1080,   1, 599,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1080P59_RGB_8BIT
    {1920,  1080,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1080P60_RGB_8BIT
    {1920,  1080,   1, 1200,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1080P120_RGB_8BIT
    //{1920,  1080,   1, 1440,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1080P144_RGB_8BIT
    //{1920,  1080,   1, 2400,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1080P240_RGB_8BIT
    {2560,  1080,   1, 599,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_2560X1080P59_RGB_8BIT
    {2560,  1080,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_2560X1080P60_RGB_8BIT
    {2560,  1080,   1, 1200,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_2560X1080P120_RGB_8BIT
    //{2560,  1080,   1, 1440,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_2560X1080P144_RGB_8BIT
    {2560,  1440,   1, 599,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_2560X1440P59_RGB_8BIT
    {2560,  1440,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_2560X1440P60_RGB_8BIT
    {2560,  1440,   1, 1200,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_2560X1440P120_RGB_8BIT
    //{2560,  1440,   1, 1440,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_2560X1440P144_RGB_8BIT
    {3840,  2160,   1, 240,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_4K2KP24_RGB_8BIT
    {3840,  2160,   1, 250,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_4K2KP25_RGB_8BIT
    {3840,  2160,   1, 300,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_4K2KP30_RGB_8BIT
    {3840,  2160,   1, 500,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_4K2KP50_RGB_8BIT
    {3840,  2160,   1, 599,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_4K2KP59_RGB_8BIT
    {3840,  2160,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_4K2KP60_RGB_8BIT
    {3840,  2160,   1, 500,    I3DDMA_COLOR_YUV411, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_4K2KP50_YUV420_8BIT
    {3840,  2160,   1, 600,    I3DDMA_COLOR_YUV411, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_4K2KP60_YUV420_8BIT
    {3840,  2160,   1, 1200,   I3DDMA_COLOR_YUV411,    I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_4K2KP120_YUV420_8BIT,
    //interlace
    {1440,  240,   0, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_480I60_RGB_8BIT
    {1440,  288,   0, 500,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_576I50_RGB_8BIT
    {1440,  240,   0, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1440X480I60_RGB_8BIT
    {1440,  288,   0, 500,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1440X576I50_RGB_8BIT
    {1920,  540,   0, 500,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1080I50_RGB_8BIT
    {1920,  540,   0, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_NONE}, // TX_TIMING_HDMI20_1080I60_RGB_8BIT

    // HDMI21
    {640,  480,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},       // TX_TIMING_HDMI21_640X480P60_RGB_8BIT_3G,
    {720,  480,   1, 599,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},       // TX_TIMING_HDMI21_720X480P59_RGB_8BIT_3G,
    {720,  480,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},       // TX_TIMING_HDMI21_720X480P60_RGB_8BIT_3G,
    {720,  576,   1, 500,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},       // TX_TIMING_HDMI21_720X576P50_RGB_8BIT_3G,
    {1280,  720,   1, 500,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},      // TX_TIMING_HDMI21_720P50_RGB_8BIT_3G,
    {1280,  720,   1, 599,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},      // TX_TIMING_HDMI21_720P59_RGB_8BIT_3G,
    {1280,  720,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},      // TX_TIMING_HDMI21_720P60_RGB_8BIT_3G,
    {1920,  1080,   1, 240,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},     // TX_TIMING_HDMI21_1080P24_RGB_8BIT_3G,
    {1920,  1080,   1, 250,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},     // TX_TIMING_HDMI21_1080P25_RGB_8BIT_3G,
    {1920,  1080,   1, 290,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},     // TX_TIMING_HDMI21_1080P29_RGB_8BIT_3G,
    {1920,  1080,   1, 300,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},     // TX_TIMING_HDMI21_1080P30_RGB_8BIT_3G,
    {1920,  1080,   1, 500,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},     // TX_TIMING_HDMI21_1080P50_RGB_8BIT_3G,
    {1920,  1080,   1, 599,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},     // TX_TIMING_HDMI21_1080P59_RGB_8BIT_3G,
    {1920,  1080,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_3G},     // TX_TIMING_HDMI21_1080P60_RGB_8BIT_3G,
    {1920,  1080,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES},     // TX_TIMING_HDMI21_1080P60_RGB_8BIT_6G4L,
    {1920,  1080,   1, 1200,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES},    // TX_TIMING_HDMI21_1080P120_RGB_8BIT_6G4L,
    //{1920,  1080,   1, 1440,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES},    // TX_TIMING_HDMI21_1080P144_RGB_8BIT_6G4L,
    //{1920,  1080,   1, 2400,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES},    // TX_TIMING_HDMI21_1080P240_RGB_8BIT_6G4L,
    {2560,  1080,   1, 599,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES},     // TX_TIMING_HDMI21_2560X1080P59_RGB_8BIT_6G4L,
    {2560,  1080,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES},     // TX_TIMING_HDMI21_2560X1080P60_RGB_8BIT_6G4L,
    {2560,  1080,   1, 1200,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES},    // TX_TIMING_HDMI21_2560X1080P120_RGB_8BIT_6G4L
    //{2560,  1080,   1, 1440,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_8G},    // TX_TIMING_HDMI21_2560X1080P144_RGB_8BIT_8G,
    {2560,  1440,   1, 599,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES},     // TX_TIMING_HDMI21_2560X1440P59_RGB_8BIT_6G4L,
    {2560,  1440,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES},     // TX_TIMING_HDMI21_2560X1440P60_RGB_8BIT_6G4L,
    {2560,  1440,   1, 1200,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES},    // TX_TIMING_HDMI21_2560X1440P120_RGB_8BIT_6G4L
    //{2560,  1440,   1, 1440,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_8G},    // TX_TIMING_HDMI21_2560X1440P144_RGB_8BIT_8G4L

    {3840,  2160,   1, 240,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_6G_4LANES},      // TX_TIMING_HDMI21_4K2KP24_RGB_8BIT_6G4L,
    {3840,  2160,   1, 250,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_6G_4LANES},      // TX_TIMING_HDMI21_4K2KP25_RGB_8BIT_6G4L,
    {3840,  2160,   1, 300,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_6G_4LANES},      // TX_TIMING_HDMI21_4K2KP30_RGB_8BIT_6G4L,
    {3840,  2160,   1, 500,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_6G_4LANES},      // TX_TIMING_HDMI21_4K2KP50_RGB_8BIT_6G4L,
    {3840,  2160,   1, 599,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_6G_4LANES},      // TX_TIMING_HDMI21_4K2KP59_RGB_8BIT_6G4L,
    {3840,  2160,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_6G_4LANES},      // TX_TIMING_HDMI21_4K2KP60_RGB_8BIT_6G4L,
    {3840,  2160,   1, 600,    I3DDMA_COLOR_RGB, I3DDMA_COLOR_DEPTH_8B, _HDMI21_FRL_12G},      // TX_TIMING_HDMI21_4K2KP60_RGB_8BIT_12G,
    {3840,  2160,   1, 1200,   I3DDMA_COLOR_RGB,    I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_12G}, // TX_TIMING_HDMI21_4K2KP120_RGB_8BIT,8bit->8G,10bit->10G,12bit->12G
    {3840,  2160,   1, 1440,   I3DDMA_COLOR_RGB,    I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_10G},   // TX_TIMING_HDMI21_4K2KP144_RGB_8BIT_10G,
    {3840,  2160,   1, 1440,   I3DDMA_COLOR_RGB,    I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_12G},   // TX_TIMING_HDMI21_4K2KP144_RGB_8BIT_12G,
    {3840,  2160,   1, 1200,   I3DDMA_COLOR_YUV411,    I3DDMA_COLOR_DEPTH_8B,  _HDMI21_FRL_6G_4LANES}, // TX_TIMING_HDMI21_4K2KP120_YUV420_8BIT_6G4L,

};
#endif

/************************
// Timing table definition
//
*************************/
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
extern UINT8 HDMITX_TIMING_INDEX;

//{set1}  : {blank, active, sync, front, back} for h
//{set2}  : {space, blank, act_video, active, sync, front, back} for v
extern hdmi_tx_timing_gen_st  cea_timing_table_repeater[TX_TIMING_NUM];
#endif // #ifdef CONFIG_HDMITX_REPEATER_SUPPORT
extern hdmi_tx_output_timing_type tx_output_timing_table;

/************************
// extern variable
//
*************************/
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
extern UINT8 current_port;
#endif

/************************
// extern function
//
*************************/
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
extern void ScalerHdmiMacTx0RepeaterEdidSupportDump(void);
extern UINT8 ScalerHdmiMacTx0RepeaterGetEdidSupport(EnumEdidFeatureType enumEdidFeature);
extern INT8 ScalerHdmiMacTx0RepeaterSetEdidSupport(EnumEdidFeatureType enumEdidFeature, UINT8 value);
extern UINT8 ScalerHdmiMacTx0RepeaterSetTxUserTimingMode(TX_TIMING_INDEX time_mode);
extern TX_TIMING_INDEX ScalerHdmiMacTx0RepeaterGetTxOutputModeTiming(TX_OUT_MODE_SELECT_TYPE mode);
extern void ScalerHdmiTxSetMute(UINT8 enable);
extern void HDMITX_drv_set_hdmitx_gate_sel(void);

/* ***************************************
    Description  : AP query the HDMITX support timing in specified output mode
    Input Value  : TX_LIB_OUT_MODE_SELECT_TYPE 
    Return Value : TX output timing index for specified TX output mode
****************************************/
TX_TIMING_INDEX ScalerHdmiRepeaterLibGetTxOutputModeTiming(TX_REPEATER_OUT_MODE_SELECT_TYPE mode)
{
    return ScalerHdmiMacTx0RepeaterGetTxOutputModeTiming((TX_OUT_MODE_SELECT_TYPE)mode);
}

/* ***************************************
    Description  : AP get HDMITX Edid Support Dump
    Input Value  : time_mode (0<= enumEdidFeature < _EDID_FEATURE_TYPE_NUM)
    Return Value : void
****************************************/
void ScalerHdmiRepeaterLibGetTxEdidSupportDump(void)
{
    ScalerHdmiMacTx0RepeaterEdidSupportDump();
}

/* ***************************************
    Description  : AP get HDMITX Edid Support
    Input Value  : time_mode (0<= enumEdidFeature < _EDID_FEATURE_TYPE_NUM)
    Return Value : 0 :false, 1 true
****************************************/
UINT8 ScalerHdmiRepeaterLibGetTxEdidSupport(EnumEdidFeatureType enumEdidFeature)
{
   return  ScalerHdmiMacTx0RepeaterGetEdidSupport(enumEdidFeature);
}

/* ***************************************
    Description  : AP set HDMITX Edid Support
    Input Value  : time_mode (0<= enumEdidFeature < _EDID_FEATURE_TYPE_NUM)
                        value:true/false
    Return Value : 0 :success, -1 fail
****************************************/
INT8 ScalerHdmiRepeaterLibSetTxEdidSupport(EnumEdidFeatureType enumEdidFeature, UINT8 value)
{
   return  ScalerHdmiMacTx0RepeaterSetEdidSupport(enumEdidFeature, value);
}

/* ***************************************
    Description  : AP set HDMITX User timing Mode
    Input Value  : time_mode (0<= time_mode < TX_TIMING_NUM)
    Return Value : 0 :success, TX_TIMING_NUM fail
****************************************/
UINT8 ScalerHdmiRepeaterLibSetTxUserTimingMode(TX_TIMING_INDEX time_mode)
{
   return  ScalerHdmiMacTx0RepeaterSetTxUserTimingMode(time_mode);
}

/* ***************************************
  Description  : Get TX timing table detail info
  Input Value  :
                        TX_TIMING_INDEX: tx timing index
                        hdmi_repeater_timing_gen_st * : tx timing_table
  Return Value :
                        hdmi_repeater_timing_gen_st *: TX timing table info
                        return: 0: SUCCESS
****************************************/
INT8 ScalerHdmiRepeaterLibGetTxTimingTable(TX_TIMING_INDEX index, hdmi_tx_timing_gen_st *time_mode)
{

    if(time_mode == NULL){
        FatalMessageHDMITx("[HDMITX] NULL PTR@ScalerHdmiRepeaterLibGetTxTimingTable\n");
        return -1;
    }

    if(index < TX_TIMING_NUM){
        memcpy(time_mode, &cea_timing_table_repeater[(UINT8)index], sizeof(hdmi_tx_timing_gen_st));
        return 0;
    }else{
        return -1;
    }
}

UINT16 HDMITX_get_output_timing_table(HDMI_OUTPUT_TIMING_TABLE infoList, UINT8 index)
{
    switch (infoList)
    {
        case TX_COLORTYPE_INFO:
        	return tx_output_timing_table.color_type;
        case TX_FRLTYPE_INFO:
            return tx_output_timing_table.frl_type;
        default:
            return 0;

     }
}

 #ifdef HDMITX_TIMING_TABLE_CONFIG
INT8 HDMITX_GetTxOutputTimingTable(TX_TIMING_INDEX index, hdmi_tx_output_timing_type *time_mode)
{

    if(time_mode == NULL){
        FatalMessageHDMITx("[HDMITX] NULL PTR@HDMITX_GetTxOutputTimingTable\n");
        return -1;
    }

    if(index < TX_TIMING_NUM){
        memcpy((UINT8*)time_mode, (UINT8*)&tx_output_timing_table[(UINT8)index], sizeof(hdmi_tx_output_timing_type));
        return 0;
    }else{
        FatalMessageHDMITx("[HDMITX] index over range[%d]@HDMITX_GetTxOutputTimingTable\n", (UINT32)index);
        //time_mode = NULL;
        return -1;
    }
}

UINT8 HDMITX_GetTxOutputColorDepth(UINT8 timing_index)
{
    return tx_output_timing_table[timing_index].color_depth;
}


UINT8 HDMITX_GetTxOutputColorSpace(UINT8 timing_index)
{
    return tx_output_timing_table[timing_index].color_type;
}
#endif

/* ***************************************
  Description  : write modified Sink EDID to RX [and restart RX HPD] when sink EDID is changed
  Input Value  : 1: restart HDMI RX HPD
  Return Value : NA
****************************************/
void ScalerHdmiRepeaterLibSendEdidToRx(BOOLEAN apply)
{
    // AP SHOULD write sink EDID to RX and restart RX HPD when sink EDID is changed
    //ScalerHdmiMacTx0RepeaterEdidSendToRx(apply);

    return;
}


/* ***************************************
  Description  : Set HDMITX output timing mode
  Input Value  : TX_TIMING_INDEX (tx timing index)
  Return Value : NA
****************************************/
void setInputTimingType(TX_TIMING_INDEX index)
{
     if((index >= TX_TIMING_NUM)){
        FatalMessageHDMITx("[HDMITX] INDEX[%d] OVER RANGE\n", (UINT32)index);
        return;
     }
    HDMITX_TIMING_INDEX = index;
    InfoMessageHDMITx("[HDMITX] Scaler set TimingMode INDEX[%d]\n", (UINT32)index);
    return;
}

void ScalerHdmiRepeaterLibSetTxTimingMode(TX_TIMING_INDEX index)
{
    setInputTimingType(index);
    return;
}

void HDMITX_Set_ReNewIndex_in(unsigned int index)
{
    FatalMessageHDMITx("[HDMITX] hdmitxReNewIndex_in=%d->%d\n", hdmitxReNewIndex_in, index);
    hdmitxReNewIndex_in = index;
    return;
}

unsigned char HDMITX_Get_ReNewIndex_in(void)
{
    return hdmitxReNewIndex_in;
}

TX_STATUS ScalerHdmiTx_Get_Timing_Status(void) {

    return tx_status;
}

void ScalerHdmiTx_Set_ClkGateSel(void) {
    HDMITX_drv_set_hdmitx_gate_sel();
    return;
}


/* ***************************************
  Description  : set HDMITX packet avmute enable/disable
  Input Value  : 1: enable, 0: disable
  Return Value : NA
****************************************/
// Scaler MAY set HDMITX packet avmute enable to avoid sink device detect unstable signal from HDMITX to avoid RX abnormal display occurred
void ScalerHdmiRepeaterLibSetTxAvMute(BOOLEAN enable)
{
    ScalerHdmiTxSetMute(enable);
    return;
}

/* ***************************************
  Description  : Get HDMITX GCP packet avmute apply status (next frame)
  Input Value  : NA
  Return Value : TRUE: TX will apply set_avmute=1 in next frame, FALSE: TX will apply set_avmute=0 in next frame
****************************************/
 UINT8 ScalerHdmiRepeaterLibGetTxAvMuteApplyStatus(void)
 {
    // av_mute = sel_mux? (HW GCP packet refer to RX (when enable) | scalerForceBG (when enable) ) : ( HW GCP packet refer to SW setting |scalerForceBG (when enable) )
    // check HDMI mode : HDMI20/21
    // check GCP packet mode: SW/HW packet
    // check HW enable & status: RX/ForceBG (vsync update)


     return 0;
 }
#endif // #ifdef CONFIG_HDMITX_REPEATER_SUPPORT



#ifdef CONFIG_RTK_KDRV_RTICE_AT_COMMAND
extern void ScalerHdmiMacTx0RepeaterOutputTimingInfo(void);
extern UINT8 TxRunDone;

UINT32 AT_HdmiTX_GetRawTimingInfo(AT_HDMI_TX_TIMING_INFO_T* p_timing_info)
{
    if (p_timing_info == NULL)
    {
        return 1;
    }
    if(!TxRunDone){
        FatalMessageHDMITx("[HDMITX] TX not ready\n");
        return 1;
    }
    if(sizeof(AT_HDMI_TX_TIMING_INFO_T) != sizeof(StructHDMITxOutputTimingInfo))
    {
        FatalMessageHDMITx("[HDMITX] structure not match\n");
        return 1;
    }
    set_hdmitxtiminginfo_addr((StructHDMITxOutputTimingInfo *)p_timing_info);
    set_hdmitxOutputMode(repeater_tx_out_mode);
    ScalerHdmiMacTx0RepeaterOutputTimingInfo();

    return 0;

}

void AT_HdmiTX_GetRawTimingInfo_cb(void)
{
        //__xdata UINT8 *const cmd = AT_ACQUIRE_R_TPDU();

       AT_HDMI_TX_TIMING_INFO_T *p_timing_info;
        // 0. the cmd[0] is 0xa0, means the 0xA0 AT command TPDU.
        /* here to handle the input argument from TPDU cmd.
         * a) please keep cmd[0] constant,
         * b) cmd_index is A0_TPDU_CMD_INDEX(cmd), argument data is A0_TPDU_DATA(cmd)
         */
       DebugMessageHDMITx(LOGGER_ERROR, "%s AAA size: %d \n", __func__, (UINT32)sizeof(AT_HDMI_TX_TIMING_INFO_T));

        // 1. take use the container of TPDU as A0 R_TPDU to response the return data.
        p_timing_info = (AT_HDMI_TX_TIMING_INFO_T *) AT_R_TPDU_DATA(AT_ACQUIRE_R_TPDU());
        if (AT_HdmiTX_GetRawTimingInfo(p_timing_info))
        {
            DebugMessageHDMITx(LOGGER_ERROR, "Did not get timing info\n");
        }
        else
        {
            DebugMessageHDMITx(LOGGER_ERROR, "already get timing info");
        }

        // 2. response the CMD with data SIZE
        at_r_tpdu_send(AT_ACQUIRE_R_TPDU(), sizeof(AT_HDMI_TX_TIMING_INFO_T)/*data_size*/);

        DebugMessageHDMITx(LOGGER_ERROR, " CCC \n");

        return;
}


extern UINT8 ScalerHdmiTxGetHPDStatus(void);
void AT_HdmiTX_Get_HPD_Status_cb(void)
{
    UINT32 *value = (UINT32 *)AT_R_TPDU_DATA(AT_ACQUIRE_R_TPDU());
    *value = ScalerHdmiTxGetHPDStatus();
    at_r_tpdu_send(AT_ACQUIRE_R_TPDU(), sizeof(UINT32));
    return;
}
extern HDMI_DP_TIMING_T timing_info;
void AT_HdmiTX_Get_VRR_cb(void)
{
    UINT32 *value = (UINT32 *)AT_R_TPDU_DATA(AT_ACQUIRE_R_TPDU());
    if(ScalerHdmiTx_Get_Timing_Status() == TX_TIMING_SETTING_DONE)
        *value = timing_info.run_vrr;
    else
        *value = 0;
    at_r_tpdu_send(AT_ACQUIRE_R_TPDU(), sizeof(UINT32));
    return;
}
extern UINT8 ApSetTxInit;
extern UINT8 ScalerHdmiTxGetEDIDValid(void);

void AT_HdmiTX_Get_EDID_Valid_cb(void)
{
    UINT32 *value = (UINT32 *)AT_R_TPDU_DATA(AT_ACQUIRE_R_TPDU());
     if(ApSetTxInit)
        *value = ScalerHdmiTxGetEDIDValid();
     else
        *value = 0;
    at_r_tpdu_send(AT_ACQUIRE_R_TPDU(), sizeof(UINT32));
    return;
}

UINT16 at_edid_offset = 0;
__xdata UINT8 *pAtTxCmdData;
__xdata UINT8 *u8_hdmitx_at_response_data;
UINT8 fatReadFinish;
UINT16 atReadSize;
UINT16 atReadTotalSize;
extern UINT8 EDID_HDMI20[MAX_EDID_LEN];
void AT_EDID_Get_TX_table_cb(void)
{
#if 1
    DebugMessageHDMITx(LOGGER_ERROR, "%s AAA size: %d \n", __func__, (UINT32)sizeof(AT_EDID_TABLE_T));

    pAtTxCmdData = AT_TPDU_DATA(AT_ACQUIRE_TPDU());

    // 1. take use the container of TPDU as A0 R_TPDU to response the return data.
    u8_hdmitx_at_response_data = (__xdata UINT8 *)AT_R_TPDU_DATA(AT_ACQUIRE_R_TPDU());
#if 0
    DebugMessageHDMITx(0, "%s \n", __func__);
    DebugMessageHDMITx(0, "%x\t", (UINT32)pAtCmdData[0]);
    DebugMessageHDMITx(0, "%x\t", (UINT32)pAtCmdData[1]);
    DebugMessageHDMITx(0, "%x\t", (UINT32)pAtCmdData[2]);
    DebugMessageHDMITx(0, "%x\t", (UINT32)pAtCmdData[3]);
    DebugMessageHDMITx(0, "%x\t", (UINT32)pAtCmdData[4]);
    DebugMessageHDMITx(0, "%x\t", (UINT32)pAtCmdData[5]);
    DebugMessageHDMITx(0, "\n");
#endif
    fatReadFinish = pAtTxCmdData[0];
    DebugMessageHDMITx(0, "fFinish = %d\n", (UINT32)fatReadFinish);
    *((UINT8 *)&atReadSize + 1) = pAtTxCmdData[3];
    *((UINT8 *)&atReadSize + 0) = pAtTxCmdData[2];
    DebugMessageHDMITx(0, "size = %x\n", (UINT32)atReadSize);
    *((UINT8 *)&atReadTotalSize + 1) = pAtTxCmdData[5];
    *((UINT8 *)&atReadTotalSize + 0) = pAtTxCmdData[4];
    DebugMessageHDMITx(0, "totalSize = %x\n", (UINT32)atReadTotalSize);

    if (fatReadFinish != 1)
    {
        memcpy(u8_hdmitx_at_response_data, EDID_HDMI20 + at_edid_offset, atReadSize);
        at_edid_offset += atReadSize;
    }
    else
    {
        memcpy(u8_hdmitx_at_response_data, EDID_HDMI20 + at_edid_offset, atReadSize);
        at_edid_offset = 0;
    }

#if 0//for debug
    {
        UINT16 i  = 0;
        DebugMessageHDMITx(LOGGER_INFO, "[hdmitx]read edid from tx:\n");
        for (i = 0; i < atReadSize; i++) {
            if (i % 16 == 0) {
                DebugMessageHDMITx(LOGGER_INFO, "\n");
            }
            DebugMessageHDMITx(LOGGER_INFO, "%d,", (UINT32)u8_hdmitx_at_response_data[i]);
        }
        DebugMessageHDMITx(LOGGER_INFO, "\n");
    }
#endif
    // 2. response the CMD with data SIZE
    at_r_tpdu_send(AT_ACQUIRE_R_TPDU(), atReadSize);
    DebugMessageHDMITx(LOGGER_ERROR, " CCC \n");
#endif
    return;
}

void HDMITX_drv_menuCmd_setTxPtgTimingMode(UINT8 timeMode);
void AT_Timing_Switch_Test_cb(void)
{
    UINT8 *index = AT_TPDU_DATA(AT_ACQUIRE_TPDU());
    if (*index < PTG_TIMING_NUM) {
        HDMITX_drv_menuCmd_setTxPtgTimingMode(*index);
    }

    rtice_pure_ack(AT_ACQUIRE_R_TPDU());
    return;
}
#endif // #if CONFIG_RTK_KDRV_RTICE_AT_COMMAND
