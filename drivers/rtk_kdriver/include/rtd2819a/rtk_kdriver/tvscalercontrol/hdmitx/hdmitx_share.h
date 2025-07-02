#ifndef _HDMITX_SHARE_H_
#define _HDMITX_SHARE_H_

typedef enum _SLR_INPUT_INFO{
    SCALER_INFO_INPUT_SRC = 0x00,
    SCALER_INFO_VSC_ISCONNECT,
    SCALER_INFO_IPH_ACT_STA,
    SCALER_INFO_IPV_ACT_STA,
    SCALER_INFO_IPH_ACT_WID,
    SCALER_INFO_IPV_ACT_LEN,
    SCALER_INFO_IHFREQ,
    SCALER_INFO_IVFREQ,
    SCALER_INFO_IHTOTAL,
    SCALER_INFO_IVTOTAL,
    SCALER_INFO_PIXEL_CLOCK,
    SCALER_INFO_COLOR_SPACE,
    SCALER_INFO_COLOR_DEPTH,
    SCALER_INFO_COLOR_IMETRY,
    SCALER_INFO_COLOR_EXT_IMETRY,
    SCALER_INFO_COLOR_RANGE,
    SCALER_INFO_INTERLACE,
    SCALER_INFO_HDCP,
    SCALER_INFO_VRR,
    SCALER_INFO_HDR,
    SCALER_INFO_PASSTHROUGH,
    SCALER_INFO_TESTMODE_ISCONNECT,
    SCALER_INFO_TX_OUTPUT_ISCONNECT,
    SCALER_INFO_TX_OUTPUT_MODE,
    SCALER_INFO_TX_OUTPUT_TIMING_MODE,
    SCALER_INFO_TX_OUTPUT_H_ACT_STA,
    SCALER_INFO_TX_OUTPUT_V_ACT_STA,
    SCALER_INFO_TX_OUTPUT_H_ACT_WID,
    SCALER_INFO_TX_OUTPUT_V_ACT_LEN,
    SCALER_INFO_TX_OUTPUT_H_TOTAL,
    SCALER_INFO_TX_OUTPUT_V_TOTAL,
    SCALER_INFO_TX_OUTPUT_VFREQ,
    SCALER_INFO_TX_OUTPUT_PIXEL_CLOCK,
    // SCALER_INFO_TX_OUTPUT_COLOR_SPACE,
    // SCALER_INFO_TX_OUTPUT_COLOR_DEPTH,
    SCALER_INFO_Main_Act_Wid,
    SCALER_INFO_Main_Act_Len,
    SCALER_INFO_MAIN_UZD_WID,
    SCALER_INFO_MAIN_UZD_LEN,
    SCALER_INFO_DC2H_STATE,
    SCALER_INFO_DC2H_ISCONNECT,
    SCALER_INFO_DC2H_SHOWTYPE,
    SCALER_INFO_DC2H_CAPWID,
    SCALER_INFO_DC2H_CAPLEN,
    SCALER_INFO_DC2H_VFREQ,
    SCALER_INFO_DC2H_DATAFORMAT,
    SCALER_INFO_SLR_DISP_10BIT,
    SCALER_INFO_SLR_DISP_422CAP,

    /* HDMITX */
    SCALER_INFO_TX_OUTPUT_HSYNC,
    SCALER_INFO_TX_OUTPUT_VSYNC,
    // SCALER_INFO_TX_OUTPUT_RGB_QUANTIZATION_RANGE,
    // SCALER_INFO_TX_OUTPUT_YCC_QUANTIZATION_RANGE,
    // SCALER_INFO_TX_OUTPUT_EXT_COLORIMETRY,
    SCALER_INFO_TX_CLK_4PIXEL_MODE,
    SLR_INPUT_HDMITX_DSC_SRC,
    SCALER_INFO_NUM,
}SLR_INPUT_INFO;

typedef struct _StructHdmitxDisplayInfo{
    UINT8  input_src;          // HDMI or DP
    UINT8  VSC_isConnect;      //VSC is connect or not
    UINT16 IPH_ACT_STA;       //Input act h sta
    UINT16 IPV_ACT_STA;       //Input act v sta
    UINT16 IPH_ACT_WID;      //Input act h wid
    UINT16 IPV_ACT_LEN;       //Input act v length
    UINT16 IPV_VSYNC_WIDTH;   //Input vsync width
    UINT16 IHFreq;        //Input Horizontal Frequency
    UINT16 IVFreq;        //Input Vertical Frequency
    UINT16 IHTotal;           //Input Horizontal Total Length
    UINT16 IVTotal;           //Input Vertical Total Length
    UINT16 PixelClock;        //Input pixel clock

    UINT8 color_space;       // rgb/yuv422/yuv444/yuv420
    UINT8 color_depth;       // 8bits/10bits/12bits/16bits
    UINT8 color_imetry;      // BT709/BT601 ...etc
    UINT8 color_ext_imetry;
    UINT8 color_range;       // default/limit/full
    UINT8 polarity;          //hdmi vsync polarity
    UINT8 Interlace;          //Interlace or Progressive
    UINT8 HDCP;           //is HDCP?
    UINT8 VRR;            //0 : non VRR, 1 : Freesync, 2 : G-sync
    UINT8 HDR;            //HDR forma, 0:SDR, 1:HDR10, 2:HDR10+, 3:Dolby
    UINT8 passThrough;
    UINT8 TestMode_isConnect;     //test mode is connect or not
    UINT8 isSupportSdnr; // sdnr module support or not

    UINT8 TX_OUTPUT_isConnect;     //HDMI TX is connect or not

    UINT8 TX_OUTPUT_MODE;
    UINT8 TX_OUTPUT_Timing_Mode; //Tx timing table index

    UINT16 TX_OUTPUT_H_ACT_STA;     //Tx act h sta
    UINT16 TX_OUTPUT_V_ACT_STA;     //Tx act v sta
    UINT16 TX_OUTPUT_H_ACT_WID; //Tx act h wid
    UINT16 TX_OUTPUT_V_ACT_LEN;     //Tx act v length
    UINT16 TX_OUTPUT_H_Total;          //Tx Horizontal Total Length
    UINT16 TX_OUTPUT_V_Total;          //Tx Vertical Total Length
    UINT16 TX_OUTPUT_VFreq;            //Tx timing output frequency
    UINT16 TX_OUTPUT_PixelClock;        //Tx timing output pixel clock
    UINT8 TX_OUTPUT_ColorSpace; //Tx output color space, rgb/yuv422/yuv444/yuv420

    UINT16 Main_Act_Wid;               // display width after UZU
    UINT16 Main_Act_Len;               // display length after UZU
    UINT16 Main_Uzd_Wid;           // main path width after uzd
    UINT16 Main_Uzd_Len;           // main path length after uzd
    UINT16 DC2H_Uzd_Wid;
    UINT16 DC2H_Uzd_Len;

    UINT16 DC2H_Uzu_Input_Wid;           // DC2H path uzu input width
    UINT16 DC2H_Uzu_Input_Len;           // DC2H path uzu input length

    UINT8 DC2H_state;            //0:DC2H is not active, 1:DC2H is active
    UINT8 DC2H_isConnect;           //DC2H is connect or not

    UINT8 DC2H_showType;            //0: normal, 1: not support, 2: no signal, 3: hdcp on
    UINT16 DC2H_CapWid;              //Dc2h capture width
    UINT16 DC2H_CapLen;              //Dc2h capture length
    UINT16 DC2H_vFreq;               //Dc2h capture v freq
    UINT8 DC2H_dataFormat;          //Dc2h capture data format (NV12,M420,I420,YUYV,RGB24,RGB32,P010,MJ)

    UINT8 SLR_DISP_10BIT;
    UINT8 SLR_DISP_422CAP;

    UINT16 TX_OUTPUT_HSYNC; // TX output hsync width
    UINT16 TX_OUTPUT_VSYNC;
    UINT8 TX_CLK_4PIXEL_MODE;
    UINT8 hdmitxDscSrc;
}StructHdmitxDisplayInfo;


typedef enum _HDR_MODE {
    HDR_MODE_DISABLE 		= 0x0,
    HDR_DOLBY_HDMI			= 0x1,
    HDR_DOLBY_COMPOSER		= 0x2,
    HDR_DOLBY_CERT_DM422		= 0x4,
    HDR_DOLBY_CERT_DM420		= 0x5,
    HDR_DOLBY_CERT_COMPOSER	= 0x6,
    HDR_HDR10_HDMI			= 0x10,
    HDR_HDR10_VDEC	= 0x11
} HDR_MODE;
typedef enum {

	I3DDMA_COLOR_DEPTH_8B =0,
	I3DDMA_COLOR_DEPTH_10B,
	I3DDMA_COLOR_DEPTH_12B,
	I3DDMA_COLOR_DEPTH_16B,

} I3DDMA_COLOR_DEPTH_T;

typedef enum {
	I3DDMA_COLOR_RGB 	= 0x00,
	I3DDMA_COLOR_YUV422,
	I3DDMA_COLOR_YUV444,
	I3DDMA_COLOR_YUV411,
	I3DDMA_COLOR_UNKNOW
} I3DDMA_COLOR_SPACE_T;

#endif//__SCALERSTRUCT_H__

