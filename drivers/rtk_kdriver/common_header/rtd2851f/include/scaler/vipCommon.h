/***************************************************************************
	vipCommon.h  -  description
	-------------------
    begin                : Wed Mar 11 2009
    copyright           : (C) 2008 by hsliao
    email                : hsliao@realtek.com
 ***************************************************************************/

#ifndef _COMMON_VIP_H__
#define _COMMON_VIP_H__
#define yc_tbl

typedef enum _SLR_VIP_CHANNEL {
	VIP_CHANNEL_R = 0,
	VIP_CHANNEL_G,
	VIP_CHANNEL_B,
	VIP_CHANNEL_RGB_MAX,
	VIP_CHANNEL_W = VIP_CHANNEL_RGB_MAX,
	VIP_CHANNEL_RGBW_MAX,
} SLR_VIP_CHANNEL;

typedef enum _SLR_VIP_Freq_TH{
    Freq_TH0 = 0,
    Freq_TH1,
    Freq_TH2,
    Freq_TH3,
	Freq_TH4,
	Freq_TH5,
	Freq_TH6,
	Freq_TH7,
	Freq_TH8,
	Freq_TH9,
	
	Freq_TH_MAX,	
} _SLR_VIP_Freq_TH ;

#ifndef LITTLE_ENDIAN
typedef union
{
    unsigned int regValue;
    struct{
        unsigned int  diffMinThd:10;
        unsigned int  restIdx:6;
        unsigned int  restIdxThd:16;
    };
}vip_di_im_di_hmc_pan_dummy_1_RBUS;

typedef union
{
    unsigned int regValue;
    struct{
        unsigned int  diffMaxMin:16;
        unsigned int  panConti_str:8;
        unsigned int  panConti:8;
    };
}vip_di_im_di_hmc_pan_dummy_2_RBUS;

typedef union
{
    unsigned int regValue;
    struct{
        unsigned int  panDeb_str:8;
        unsigned int  panDeb:8;
        unsigned int  dummy:16;
    };
}vip_di_im_di_hmc_pan_dummy_3_RBUS;

typedef union
{   
    unsigned int regValue;
    struct{     
                unsigned int  DCC_Level:4;// 28 ~ 31
                unsigned int  res1:3; // 25 ~ 27
                unsigned int  H_LPF_Tap:2; // 23 ~ 24
                unsigned int  Panel_compensation:1;//22
                unsigned int  Boundary_Type:4; // 18:21
                unsigned int  C_LPF_Tap:3; // 15~17
                unsigned int  Curve_LPF_Enable:1; // 14
                unsigned int  Hist_LPF_Enable:1; // 13
                unsigned int  Output_LimitRange_Enable:1; // 12
                unsigned int  Limit_range_Hist_Cut_Enable:1;// 11
                unsigned int  Bin_boundary_Enable:1;// 10
                unsigned int  Database_Enable:1; // 9
                unsigned int  UserCurve_Enable:1; // 8
                unsigned int  Expand_Enable:1; // 7
                unsigned int  Scurve_Enable:1; // 6
                unsigned int  Hist_Adjust_Enable:1;// 5
                unsigned int  DCL_Enable:1; // 4
                unsigned int  SkinTone:1; // 3
                unsigned int  SceneChange:1; // 2
                unsigned int  PureColor:1; // 1
                unsigned int  Dummy_debug_enable:1;//0
    };
}vip_color_dcc_d_dcc_fwused_0_RBUS;

typedef union
{
    unsigned int regValue;
    struct{
        unsigned int  VipInfo:1;
        unsigned int  fw_1:7;
        unsigned int  ThreeSEGXIndex1:8;
        unsigned int  ThreeSEGXIndex0:8;
        unsigned int  Res1:3;
        unsigned int  HistogramScanningEnable:1;
        unsigned int  CurveCoefInterpolationEnable:1;
        unsigned int  Res0:1;
        unsigned int  ThreeSegCurveEnable:1;
        unsigned int  HighResCurveEnable:1;
    };
}vip_color_dcc_d_dcc_fwused_1_RBUS;

typedef union
{
    unsigned int regValue;
    struct{
        unsigned int  fw_2:24;
        unsigned int  delay_time:8;
    };
}vip_color_dcc_d_dcc_fwused_2_RBUS;

typedef union
{
    unsigned int regValue;
    struct{
                unsigned int  ThreeSEGYNegOffset1:8;
                unsigned int  ThreeSEGYNegOffset0:8;
                unsigned int  ThreeSEGYPosOffset1:8;
                unsigned int  ThreeSEGYPosOffset0:8;
    };
}vip_color_dcc_d_dcc_fwused_3_RBUS;

#else

typedef union
{
    unsigned int regValue;
    struct{
        unsigned int  restIdxThd:16;
		unsigned int  restIdx:6;
        unsigned int  diffMinThd:10;
    };
}vip_di_im_di_hmc_pan_dummy_1_RBUS;

typedef union
{
    unsigned int regValue;
    struct{
        unsigned int  panConti:8;
        unsigned int  panConti_str:8;
        unsigned int  diffMaxMin:16;
    };
}vip_di_im_di_hmc_pan_dummy_2_RBUS;

typedef union
{
    unsigned int regValue;
    struct{
        unsigned int  dummy:16;
        unsigned int  panDeb:8;
        unsigned int  panDeb_str:8;
    };
}vip_di_im_di_hmc_pan_dummy_3_RBUS;

typedef union
{   
    unsigned int regValue;
    struct{     
                unsigned int  Dummy_debug_enable:1;//0
                unsigned int  PureColor:1; // 1
                unsigned int  SceneChange:1; // 2
                unsigned int  SkinTone:1; // 3
                unsigned int  DCL_Enable:1; // 4
                unsigned int  Hist_Adjust_Enable:1;// 5
                unsigned int  Scurve_Enable:1; // 6
                unsigned int  Expand_Enable:1; // 7
                unsigned int  UserCurve_Enable:1; // 8
                unsigned int  Database_Enable:1; // 9
                unsigned int  Bin_boundary_Enable:1;// 10
                unsigned int  Limit_range_Hist_Cut_Enable:1;// 11
                unsigned int  Output_LimitRange_Enable:1; // 12
                unsigned int  Hist_LPF_Enable:1; // 13
                unsigned int  Curve_LPF_Enable:1; // 14
                unsigned int  C_LPF_Tap:3; // 15~17
                unsigned int  Boundary_Type:4; // 18:21
                unsigned int  Panel_compensation:1;//22
                unsigned int  H_LPF_Tap:2; // 23 ~ 24
                unsigned int  res1:3; // 25 ~ 27
                unsigned int  DCC_Level:4;// 28 ~ 31
    };
}vip_color_dcc_d_dcc_fwused_0_RBUS;

typedef union
{
    unsigned int regValue;
    struct{
        unsigned int  HighResCurveEnable:1;
        unsigned int  ThreeSegCurveEnable:1;
        unsigned int  Res0:1;
        unsigned int  CurveCoefInterpolationEnable:1;
        unsigned int  HistogramScanningEnable:1;
        unsigned int  Res1:3;
        unsigned int  ThreeSEGXIndex0:8;
        unsigned int  ThreeSEGXIndex1:8;
        unsigned int  fw_1:7;
        unsigned int  VipInfo:1;
    };
}vip_color_dcc_d_dcc_fwused_1_RBUS;

typedef union
{
    unsigned int regValue;
    struct{
        unsigned int  delay_time:8;
        unsigned int  fw_2:24;
    };
}vip_color_dcc_d_dcc_fwused_2_RBUS;

typedef union
{
    unsigned int regValue;
    struct{
                unsigned int  ThreeSEGYPosOffset0:8;
                unsigned int  ThreeSEGYPosOffset1:8;
                unsigned int  ThreeSEGYNegOffset0:8;
                unsigned int  ThreeSEGYNegOffset1:8;
    };
}vip_color_dcc_d_dcc_fwused_3_RBUS;
#endif

#define Debug_Buff_Num 50

/*##################################################*/
/*######### Type enum ##########*/
/*##################################################*/
typedef enum _VIP_SOURCE_TIMING {

	VIP_QUALITY_CVBS_NTSC = 0,
	VIP_QUALITY_CVBS_PAL,
	VIP_QUALITY_CVBS_S_NTSC,
	VIP_QUALITY_CVBS_S_PAL,
	VIP_QUALITY_TV_NTSC ,
	VIP_QUALITY_TV_PAL,
	VIP_QUALITY_YPbPr_480I,
	VIP_QUALITY_YPbPr_576I,
	VIP_QUALITY_YPbPr_480P,
	VIP_QUALITY_YPbPr_576P,
	VIP_QUALITY_YPbPr_720P,
	VIP_QUALITY_YPbPr_1080I,
	VIP_QUALITY_YPbPr_1080P,
	VIP_QUALITY_HDMI_480I,
	VIP_QUALITY_HDMI_576I,
	VIP_QUALITY_HDMI_480P,
	VIP_QUALITY_HDMI_576P,
	VIP_QUALITY_HDMI_720P,
	VIP_QUALITY_HDMI_1080I,
	VIP_QUALITY_HDMI_1080P,
	VIP_QUALITY_HDMI_4k2kI_30,
	VIP_QUALITY_HDMI_4k2kP_30,
	VIP_QUALITY_HDMI_4k2kP_60,
	VIP_QUALITY_VGA,

	VIP_QUALITY_DTV_480I,
	VIP_QUALITY_DTV_576I,
	VIP_QUALITY_DTV_480P,
	VIP_QUALITY_DTV_576P,
	VIP_QUALITY_DTV_720P,
	VIP_QUALITY_DTV_1080I,
	VIP_QUALITY_DTV_1080P,
	VIP_QUALITY_DTV_4k2kI_30,
	VIP_QUALITY_DTV_4k2kP_30,
	VIP_QUALITY_DTV_4k2kP_60,
	VIP_QUALITY_PVR_480I,
	VIP_QUALITY_PVR_576I,
	VIP_QUALITY_PVR_480P,
	VIP_QUALITY_PVR_576P,
	VIP_QUALITY_PVR_720P,
	VIP_QUALITY_PVR_1080I,
	VIP_QUALITY_PVR_1080P,
	VIP_QUALITY_PVR_4k2kI_30,
	VIP_QUALITY_PVR_4k2kP_30,
	VIP_QUALITY_PVR_4k2kP_60,

	VIP_QUALITY_DTV_JPEG, /* Erin '100920 , add DTV_JPEG source*/

	VIP_QUALITY_CVBS_SECAN,
	VIP_QUALITY_CVBS_S_SECAN,
	VIP_QUALITY_TV_SECAN,

	VIP_QUALITY_SCART_NTSC,
	VIP_QUALITY_SCART_PAL,
	VIP_QUALITY_SCART_SECAN,

	VIP_QUALITY_SCART_RGB_NTSC,
	VIP_QUALITY_SCART_RGB_PAL,
	VIP_QUALITY_SCART_RGB_SECAN,

	VIP_QUALITY_CVBS_PAL_M,
	VIP_QUALITY_CVBS_S_PAL_M,
	VIP_QUALITY_TV_PAL_M,

	VIP_QUALITY_TV_NTSC443, /*young timing433*/

	/*ATV/AV source Timing update start, 20160930*/
	VIP_QUALITY_CVBS_NTSC443,
	VIP_QUALITY_CVBS_PAL60,
	VIP_QUALITY_CVBS_PALN,
	VIP_QUALITY_CVBS_NTSC50,

	VIP_QUALITY_TV_PAL60,
	VIP_QUALITY_TV_NTSC50,
	VIP_QUALITY_TV_PALN,
	/*ATV/AV source Timing update end, 20160930*/

	/*HDR Timing 20150506 strat*/
	VIP_QUALITY_HDR_HDMI_SD,
	VIP_QUALITY_HDR_HDMI_1080p,
	VIP_QUALITY_HDR_HDMI_4k2kP_30,
	VIP_QUALITY_HDR_HDMI_4k2kP_60,
	VIP_QUALITY_HDR_DTV_SD,
	VIP_QUALITY_HDR_DTV_1080p,
	VIP_QUALITY_HDR_DTV_4k2kP_30,
	VIP_QUALITY_HDR_DTV_4k2kP_60,
	VIP_QUALITY_HDR_PVR_SD,
	VIP_QUALITY_HDR_PVR_1080p,
	VIP_QUALITY_HDR_PVR_4k2kP_30,
	VIP_QUALITY_HDR_PVR_4k2kP_60,

	VIP_QUALITY_DolbyHDR_HDMI_SD,
	VIP_QUALITY_DolbyHDR_HDMI_1080P,
	VIP_QUALITY_DolbyHDR_HDMI_4k2kP,
	VIP_QUALITY_DolbyHDR_DTV_SD,
	VIP_QUALITY_DolbyHDR_DTV_1080P,
	VIP_QUALITY_DolbyHDR_DTV_4k2kP,
	VIP_QUALITY_DolbyHDR_PVR_SD,
	VIP_QUALITY_DolbyHDR_PVR_1080P,
	VIP_QUALITY_DolbyHDR_PVR_4k2kP,
	/*HDR Timing 20150506 end*/

	VIP_QUALITY_ST2094_HDMI_SD,
	VIP_QUALITY_ST2094_HDMI_1080p,
	VIP_QUALITY_ST2094_HDMI_4k2kP_30,
	VIP_QUALITY_ST2094_HDMI_4k2kP_60,
	VIP_QUALITY_ST2094_DTV_SD,
	VIP_QUALITY_ST2094_DTV_1080p,
	VIP_QUALITY_ST2094_DTV_4k2kP_30,
	VIP_QUALITY_ST2094_DTV_4k2kP_60,
	VIP_QUALITY_ST2094_PVR_SD,
	VIP_QUALITY_ST2094_PVR_1080p,
	VIP_QUALITY_ST2094_PVR_4k2kP_30,
	VIP_QUALITY_ST2094_PVR_4k2kP_60,

	VIP_QUALITY_MINI_DP_480I,
	VIP_QUALITY_MINI_DP_576I,
	VIP_QUALITY_MINI_DP_480P,
	VIP_QUALITY_MINI_DP_576P,
	VIP_QUALITY_MINI_DP_720P,
	VIP_QUALITY_MINI_DP_1080I,
	VIP_QUALITY_MINI_DP_1080P,
	VIP_QUALITY_MINI_DP_4k2kI_30,
	VIP_QUALITY_MINI_DP_4k2kP_30,
	VIP_QUALITY_MINI_DP_4k2kP_60,

	VIP_QUALITY_TYPEC_480I,
	VIP_QUALITY_TYPEC_576I,
	VIP_QUALITY_TYPEC_480P,
	VIP_QUALITY_TYPEC_576P,
	VIP_QUALITY_TYPEC_720P,
	VIP_QUALITY_TYPEC_1080I,
	VIP_QUALITY_TYPEC_1080P,
	VIP_QUALITY_TYPEC_4k2kI_30,
	VIP_QUALITY_TYPEC_4k2kP_30,
	VIP_QUALITY_TYPEC_4k2kP_60,

	VIP_QUALITY_HDR_MINI_DP_SD,
	VIP_QUALITY_HDR_MINI_DP_1080p,
	VIP_QUALITY_HDR_MINI_DP_4k2kP_30,
	VIP_QUALITY_HDR_MINI_DP_4k2kP_60,
	VIP_QUALITY_HDR_TYPEC_SD,
	VIP_QUALITY_HDR_TYPEC_1080p,
	VIP_QUALITY_HDR_TYPEC_4k2kP_30,
	VIP_QUALITY_HDR_TYPEC_4k2kP_60,

	VIP_QUALITY_ST2094_MINI_DP_SD,
	VIP_QUALITY_ST2094_MINI_DP_1080p,
	VIP_QUALITY_ST2094_MINI_DP_4k2kP_30,
	VIP_QUALITY_ST2094_MINI_DP_4k2kP_60,
	VIP_QUALITY_ST2094_TYPEC_SD,
	VIP_QUALITY_ST2094_TYPEC_1080p,
	VIP_QUALITY_ST2094_TYPEC_4k2kP_30,
	VIP_QUALITY_ST2094_TYPEC_4k2kP_60,

	VIP_QUALITY_DolbyHDR_MINI_DP_SD,
	VIP_QUALITY_DolbyHDR_MINI_DP_1080P,
	VIP_QUALITY_DolbyHDR_MINI_DP_4k2kP,
	VIP_QUALITY_DolbyHDR_TYPEC_SD,
	VIP_QUALITY_DolbyHDR_TYPEC_1080P,
	VIP_QUALITY_DolbyHDR_TYPEC_4k2kP,

	VIP_QUALITY_MIRACAST_480I,
	VIP_QUALITY_MIRACAST_576I,
	VIP_QUALITY_MIRACAST_480P,
	VIP_QUALITY_MIRACAST_576P,
	VIP_QUALITY_MIRACAST_720P,
	VIP_QUALITY_MIRACAST_1080I,
	VIP_QUALITY_MIRACAST_1080P,
	VIP_QUALITY_MIRACAST_4k2kI_30,
	VIP_QUALITY_MIRACAST_4k2kP_30,
	VIP_QUALITY_MIRACAST_4k2kP_60,

	VIP_QUALITY_AIRPLAY_480I,
	VIP_QUALITY_AIRPLAY_576I,
	VIP_QUALITY_AIRPLAY_480P,
	VIP_QUALITY_AIRPLAY_576P,
	VIP_QUALITY_AIRPLAY_720P,
	VIP_QUALITY_AIRPLAY_1080I,
	VIP_QUALITY_AIRPLAY_1080P,
	VIP_QUALITY_AIRPLAY_4k2kI_30,
	VIP_QUALITY_AIRPLAY_4k2kP_30,
	VIP_QUALITY_AIRPLAY_4k2kP_60,

	VIP_QUALITY_HDR_MIRACAST_SD,
	VIP_QUALITY_HDR_MIRACAST_1080p,
	VIP_QUALITY_HDR_MIRACAST_4k2kP_30,
	VIP_QUALITY_HDR_MIRACAST_4k2kP_60,
	VIP_QUALITY_HDR_AIRPLAY_SD,
	VIP_QUALITY_HDR_AIRPLAY_1080p,
	VIP_QUALITY_HDR_AIRPLAY_4k2kP_30,
	VIP_QUALITY_HDR_AIRPLAY_4k2kP_60,

	VIP_QUALITY_ST2094_MIRACAST_SD,
	VIP_QUALITY_ST2094_MIRACAST_1080p,
	VIP_QUALITY_ST2094_MIRACAST_4k2kP_30,
	VIP_QUALITY_ST2094_MIRACAST_4k2kP_60,
	VIP_QUALITY_ST2094_AIRPLAY_SD,
	VIP_QUALITY_ST2094_AIRPLAY_1080p,
	VIP_QUALITY_ST2094_AIRPLAY_4k2kP_30,
	VIP_QUALITY_ST2094_AIRPLAY_4k2kP_60,

	VIP_QUALITY_DolbyHDR_MIRACAST_SD,
	VIP_QUALITY_DolbyHDR_MIRACAST_1080P,
	VIP_QUALITY_DolbyHDR_MIRACAST_4k2kP,
	VIP_QUALITY_DolbyHDR_AIRPLAY_SD,
	VIP_QUALITY_DolbyHDR_AIRPLAY_1080P,
	VIP_QUALITY_DolbyHDR_AIRPLAY_4k2kP,

	VIP_QUALITY_SOURCE_NUM,

} VIP_SOURCE_TIMING;

typedef enum _VIP_3DSOURCE_TIMING {

	VIP_QUALITY_HDMI_SBS = 0,
	VIP_QUALITY_HDMI_TOP_BOTTOM = 1,
	VIP_QUALITY_HDMI_HD_FP = 2,
	VIP_QUALITY_HDMI_SD_FP = 3,
	VIP_QUALITY_DTV_SBS = 4,
	VIP_QUALITY_DTV_TOP_BOTTOM = 5,
	VIP_QUALITY_DTV_HD_FP = 6,
	VIP_QUALITY_DTV_SD_FP = 7,
	VIP_QUALITY_USB_SBS = 8,
	VIP_QUALITY_USB_TOP_BOTTOM = 9,
	VIP_QUALITY_USB_HD_FP = 10,
	VIP_QUALITY_USB_SD_FP = 11,

	VIP_QUALITY_3DSOURCE_NUM,

} VIP_3DSOURCE_TIMING ;

typedef enum _Histogram_BIN_MODE {
	Mode_Orig_32Bin = 0,
	Mode_128to32Bin,
	Mode_128Bin,
	Mode_HistError,
} Histogram_BIN_MODE ;

/*! MEMC power save status. */
typedef enum
{
	PS_OFF,
	PS_ON,
}MEMC_PS_STATUS; //for prePowerSaveStatus

/*##################################################*/

/*#pragma pack(4)*/     /* set alignment to 4 byte boundary */

typedef struct {
	unsigned char RTNR_y_enable;
	unsigned char RTNR_c_enable;
	// chen 170809
unsigned char RTNR_new_method_en;
// end chen 170809
	unsigned char RTNR_mad_window;
	// chen 170809
unsigned char RTNR_moredetail_cotinue_en;
unsigned char RTNR_k_temporal_compare_snr;
unsigned char RTNR_k_temporal_compare_en;
	// end chen 170809
	unsigned char RTNR_empty_7;
} DRV_RTNR_General_ctrl;

typedef struct {
	unsigned int RTNR_Y_K15_Y_K8;
	unsigned int RTNR_Y_K7_Y_K0;
	unsigned char RTNR_empty_10;
	unsigned char RTNR_empty_11;
	unsigned char RTNR_empty_12;
	unsigned char RTNR_empty_13;
	unsigned char RTNR_empty_14;
	unsigned char RTNR_empty_15;
	unsigned char RTNR_MAD_Y_TH0;
	unsigned char RTNR_MAD_Y_TH1;
	unsigned char RTNR_MAD_Y_TH2;
	unsigned char RTNR_MAD_Y_TH3;
	unsigned char RTNR_MAD_Y_TH4;
	unsigned char RTNR_MAD_Y_TH5;
	unsigned char RTNR_MAD_Y_TH6;
	unsigned char RTNR_MAD_Y_TH7;
	unsigned char RTNR_MAD_Y_TH8;
	unsigned char RTNR_MAD_Y_TH9;
	unsigned char RTNR_MAD_Y_TH10;
	unsigned char RTNR_MAD_Y_TH11;
	unsigned char RTNR_MAD_Y_TH12;
	unsigned char RTNR_MAD_Y_TH13;
	unsigned char RTNR_MAD_Y_TH14;
	unsigned char RTNR_empty_31;
} DRV_RTNR_Old_Y;

typedef struct {
	unsigned char RTNR_Dark_K_en;
	unsigned char RTNR_Dark_TH;
	unsigned char RTNR_Kin;
	unsigned char RTNR_Kout;
	unsigned char RTNR_Kin_2;
	unsigned char RTNR_Kout_2;
	unsigned char RTNR_empty_38;
	unsigned char RTNR_empty_39;
} DRV_RTNR_Dark_K;

	// chen 170809
//	#if 0
typedef struct {
	unsigned int RTNR_sobel_thm;
	unsigned int RTNR_sobel_ths;
	unsigned char RTNR_sobel_Method;
	unsigned char RTNR_SAD_Method;
	unsigned char RTNR_SAD_Mix_cal_weight;
	unsigned char RTNR_SAD_th1;
	unsigned char RTNR_SAD_th2;
	unsigned char RTNR_SAD_th3;
	unsigned int RTNR_K_map1;
	unsigned int RTNR_K_map2;
	unsigned int RTNR_K_map3;
	unsigned int RTNR_K_map4;
	unsigned int RTNR_K_map5;
	unsigned int RTNR_K_map6;
	unsigned int RTNR_K_map7;
	unsigned int RTNR_K_map8;
	unsigned char RTNR_ET_Diff_th1;
	unsigned char RTNR_ET_Diff_th2;
	unsigned char RTNR_ET_Diff_th3;
	unsigned char RTNR_ET_Diff_th4;
	unsigned char RTNR_ET_Diff_th5;
	unsigned char RTNR_ET_Diff_th6;
	unsigned char RTNR_ET_Diff_th7;
	unsigned char RTNR_empty_63;
	unsigned char RTNR_SC_th1;
	unsigned char RTNR_SC_th2;
	unsigned char RTNR_SC_th3;
	unsigned char RTNR_SC_th4;
	unsigned char RTNR_SC_th5;
	unsigned char RTNR_SC_th6;
	unsigned char RTNR_SC_th7;
	unsigned char RTNR_empty_71;
	unsigned char RTNR_SAD_Edge_th1;
	unsigned char RTNR_SAD_Edge_th2;
	unsigned char RTNR_SAD_Edge_th3;
	unsigned char RTNR_SAD_Edge_th4;
	unsigned char RTNR_SAD_Edge_th5;
	unsigned char RTNR_SAD_Edge_th6;
	unsigned char RTNR_SAD_Edge_th7;
	unsigned char RTNR_empty_79;
	unsigned char RTNR_SAD_NonEdge_th1;
	unsigned char RTNR_SAD_NonEdge_th2;
	unsigned char RTNR_SAD_NonEdge_th3;
	unsigned char RTNR_SAD_NonEdge_th4;
	unsigned char RTNR_SAD_NonEdge_th5;
	unsigned char RTNR_SAD_NonEdge_th6;
	unsigned char RTNR_SAD_NonEdge_th7;
	unsigned char RTNR_empty_87;
} DRV_RTNR_New_Method;

typedef struct {
	unsigned char RTNR_Mask1_Enable;
	unsigned char RTNR_Mask1_Dir;
	unsigned char RTNR_Mask1_MapStep;
	unsigned char RTNR_Mask1_MapThd;
	unsigned char RTNR_Mask1_MapLimit;
	unsigned char RTNR_empty_93;
	unsigned char RTNR_empty_94;
	unsigned char RTNR_empty_95;
	unsigned char RTNR_Mask1_Y_Gain;
	unsigned char RTNR_Mask1_U_Gain;
	unsigned char RTNR_Mask1_V_Gain;
	unsigned char RTNR_Mask1_Y_Ref;
	unsigned char RTNR_Mask1_U_Ref;
	unsigned char RTNR_Mask1_V_Ref;
	unsigned char RTNR_empty_102;
	unsigned char RTNR_empty_103;
	unsigned char RTNR_Mask2_Enable;
	unsigned char RTNR_Mask2_Dir;
	unsigned char RTNR_Mask2_MapStep;
	unsigned char RTNR_Mask2_MapThd;
	unsigned char RTNR_Mask2_MapLimit;
	unsigned char RTNR_empty_109;
	unsigned char RTNR_empty_110;
	unsigned char RTNR_empty_111;
	unsigned char RTNR_Mask2_Y_Gain;
	unsigned char RTNR_Mask2_U_Gain;
	unsigned char RTNR_Mask2_V_Gain;
	unsigned char RTNR_Mask2_Y_Ref;
	unsigned char RTNR_Mask2_U_Ref;
	unsigned char RTNR_Mask2_V_Ref;
	unsigned char RTNR_empty_118;
	unsigned char RTNR_empty_119;
} DRV_RTNR_Mask;
//#endif
// end chen 170809


typedef struct {
	unsigned char RTNR_LSB_RTNR_En;
	unsigned char RTNR_LSB_RTNR_channel;
	unsigned char RTNR_bitnum;
	unsigned char RTNR_LSB_RTNR_th;
	unsigned char RTNR_LSB_RTNR_Rounding;
	unsigned char RTNR_Cur_Weight;
	unsigned char RTNR_empty_126;
	unsigned char RTNR_empty_127;
} DRV_RTNR_LSB;

typedef struct {
	unsigned char RTNR_MAD_C_TH0;
	unsigned char RTNR_MAD_C_TH1;
	unsigned char RTNR_MAD_C_TH2;
	unsigned char RTNR_MAD_C_TH3;
	unsigned char RTNR_MAD_C_TH4;
	unsigned char RTNR_MAD_C_TH5;
	unsigned char RTNR_MAD_C_TH6;
	unsigned int RTNR_C_K7_C_K0;
} DRV_RTNR_C;

/* chen 0716*/
typedef struct {
	unsigned char rtnr_ma_snr_en;
	unsigned char rtnr_ma_snr_mask;
	unsigned char rtnr_ma_snr_blending_factor1;
	unsigned char rtnr_ma_snr_blending_factor2;
	unsigned char rtnr_ma_snr_blending_factor3;
	unsigned char rtnr_ma_snr_blending_factor4;
	unsigned char rtnr_ma_snr_blending_factor5;
	unsigned char rtnr_ma_snr_motion_th1_low;
	unsigned char rtnr_ma_snr_motion_th2_low;
	unsigned char rtnr_ma_snr_motion_th3_low;
	unsigned char rtnr_ma_snr_motion_th4_low;
	unsigned char rtnr_ma_snr_motion_th1_high;
	unsigned char rtnr_ma_snr_motion_th2_high;
	unsigned char rtnr_ma_snr_motion_th3_high;
	unsigned char rtnr_ma_snr_motion_th4_high;
	unsigned char rtnr_ma_snr_edge_th_low;
	unsigned char rtnr_ma_snr_edge_th_high;
	unsigned char rtnr_ma_snr_motion_slope1;
	unsigned char rtnr_ma_snr_motion_slope2;
	unsigned char rtnr_ma_snr_motion_slope3;
	unsigned char rtnr_ma_snr_motion_slope4;
	unsigned char rtnr_ma_snr_edge_curve_lo_th;
	unsigned char rtnr_ma_snr_edge_curve_slope;
	unsigned char rtnr_ma_snr_output_clamp;
	unsigned char rtnr_ma_snr_strength;
} DRV_RTNR_MASNR;

typedef struct {
	unsigned char cp_temporal_hmcnr_y_en;
	unsigned char cp_temporal_hmcnr_uv_with_y;
	unsigned char n_mcnr_new_en;
	unsigned char n_y_zero_mv_k_en;
	unsigned char n_c_zero_mv_k_en;
	unsigned char n_mcnr_offset;
	unsigned char cp_temporal_hmcnr_search_range;
	unsigned char hori_boundary_th;
	unsigned char cp_temporal_hmcnr_mad_window;
	unsigned char n_h_pan_zeromv_penalty_en;
	unsigned char h_pan_mv_penalty;
	unsigned char rtnr_output_clamp_en;
	unsigned char rtnr_output_clamp_luma;
	unsigned char rtnr_output_clamp_chroma;
} DRV_MCNR_control;

typedef struct {
	unsigned char n_mv_avg_en;
	unsigned char n_mv_avg_mode_select;
	unsigned char avg_mv_clamp_value;
} DRV_MCNR_Mv_Avg;

typedef struct {
	unsigned char n_zero_mv_gain8_en;
	unsigned char n_mv_diff_distance_select;
	unsigned char n_mcnr_weight_avg_en;
	unsigned char n_mcnr_weight_clamp_en;
	unsigned char mv_diff_th;
	unsigned char mv_diff_slope;
	unsigned char sad_min_th;
	unsigned char sad_min_slope;
	unsigned char mcnr_weight_clamp_loth;
	unsigned char mcnr_weight_clamp_upth;
} DRV_MCNR_Weighting;

typedef struct {
	unsigned char n_rtnr_y_k_select;
	unsigned char n_rtnr_c_k_select;
	unsigned char n_mcnr_y_k_select_mc;
} DRV_MCNR_Output_K_select;

typedef struct {
  DRV_MCNR_control								MCNR_control;
	DRV_MCNR_Mv_Avg									MCNR_Mv_Avg;
	DRV_MCNR_Weighting	 						MCNR_Weighting;
	DRV_MCNR_Output_K_select				MCNR_Output_K_select;
} DRV_MCNR_Table;

typedef struct {
	DRV_RTNR_General_ctrl 	S_RTNR_General_ctrl;
	DRV_RTNR_Old_Y 			S_RTNR_Old_Y;
	DRV_RTNR_Dark_K 		S_RTNR_Dark_K;
	DRV_RTNR_New_Method 	S_RTNR_New_Method;
	DRV_RTNR_Mask 			S_RTNR_Mask;
	DRV_RTNR_LSB 			S_RTNR_LSB;
	DRV_MCNR_Table			S_MCNR_Table;
	DRV_RTNR_MASNR			S_RTNR_MASNR;
	DRV_RTNR_C 				S_RTNR_C;
} DRV_RTNR_Table;

typedef struct {
	unsigned char SNR_spatialenabley;			/*SNR*/
	unsigned char SNR_spatialenablec;
	unsigned char SNR_reserved_2;
	unsigned char SNR_reserved_3;
	unsigned char SNR_mosquito_detect_en;
	unsigned char SNR_prelpf_snr_sel;
	unsigned char SNR_sresultweight;
	unsigned char SNR_reserved_7;
	unsigned char SNR_reserved_8;
	unsigned char SNR_reserved_9;
} DRV_SNR_General_ctrl;

typedef struct {
	unsigned char SNR_fixedweight1y;				/*SNR Blending*/
	unsigned char SNR_fixedweight2y;
	unsigned char SNR_weight1y;
	unsigned char SNR_weight2y;
	unsigned char SNR_reserved_4;
	unsigned char SNR_reserved_5;
	unsigned char SNR_reserved_6;
	unsigned char SNR_reserved_7;
	unsigned char SNR_reserved_8;			
	unsigned char SNR_reserved_9;
	unsigned char SNR_reserved_10;
	unsigned char SNR_zoranweighty_sel;
	unsigned char SNR_spatialthly;
	unsigned char SNR_spatialthlyx2;
	unsigned char SNR_spatialthlyx4;
	unsigned char SNR_reserved_15;
	unsigned char SNR_reserved_16;			
	unsigned char SNR_reserved_17;
	unsigned char SNR_reserved_18;
	unsigned char SNR_reserved_19;
	unsigned char SNR_reserved_20;
	unsigned char SNR_reserved_21;
	unsigned char SNR_reserved_22;
	unsigned char SNR_reserved_23;
} DRV_SNR_Spatial_NR_Y;

typedef struct {
	unsigned char SNR_lpresultweightc;				/*SNR C*/
	unsigned char SNR_reserved_1;
	unsigned char SNR_reserved_2;
	unsigned char SNR_reserved_3;
	unsigned char SNR_zoranweightc_sel;
	unsigned char SNR_spatialthlc;
	unsigned char SNR_spatialthlcx2;
	unsigned char SNR_spatialthlcx4;
} DRV_SNR_Spatial_NR_C;

typedef struct {
	unsigned char SNR_reserved_0;				/*Impulse NR*/
	unsigned char SNR_reserved_1;
	unsigned char SNR_reserved_2;
	unsigned char SNR_reserved_3;
	unsigned char SNR_reserved_4;
	unsigned char SNR_reserved_5;
	unsigned char SNR_reserved_6;
	unsigned char SNR_reserved_7;
	unsigned char SNR_reserved_8;
	unsigned char SNR_reserved_9;
} DRV_SNR_Impulse_NR;

typedef struct {
	unsigned char SNR_curvemappingmode_en;		/*Curve Mapping Mode_Global Control*/
	unsigned char SNR_reserved_1;
	unsigned char SNR_reserved_2;
	unsigned char SNR_reserved_3;
	unsigned char SNR_reserved_4;
	unsigned char SNR_reserved_5;
	unsigned char SNR_reserved_6;
	unsigned char SNR_reserved_7;
	unsigned short SNR_curvemapping_step1;		/*Curve Mapping Mode_step*///jyyang 20150706
	unsigned short SNR_curvemapping_step2;		//jyyang 20150706
	unsigned short SNR_curvemapping_step3;		//jyyang 20150706
	unsigned short SNR_curvemapping_step4;		//jyyang 20150706
	unsigned short SNR_curvemapping_step5;		//jyyang 20150706
	unsigned short SNR_curvemapping_step6;		//jyyang 20150706
	unsigned short SNR_curvemapping_step7;		//jyyang 20150706
	unsigned short SNR_curvemapping_step8;		//jyyang 20150706
	unsigned char SNR_curvemapping_w1_0;		/*Curve Mapping Mode_Weight1*/
	unsigned char SNR_curvemapping_w1_1;
	unsigned char SNR_curvemapping_w1_2;
	unsigned char SNR_curvemapping_w1_3;
	unsigned char SNR_curvemapping_w1_4;
	unsigned char SNR_curvemapping_w1_5;
	unsigned char SNR_curvemapping_w1_6;
	unsigned char SNR_curvemapping_w1_7;
	unsigned char SNR_curvemapping_w1_8;
	unsigned char SNR_reserved_25;
	unsigned char SNR_reserved_26;
	unsigned char SNR_reserved_27;
	unsigned char SNR_reserved_28;
	unsigned char SNR_reserved_29;
	unsigned char SNR_reserved_30;
	unsigned char SNR_reserved_31;
	unsigned char SNR_curvemapping_w2_0;		/*Curve Mapping Mode_Weight2*/
	unsigned char SNR_curvemapping_w2_1;
	unsigned char SNR_curvemapping_w2_2;
	unsigned char SNR_curvemapping_w2_3;
	unsigned char SNR_curvemapping_w2_4;
	unsigned char SNR_curvemapping_w2_5;
	unsigned char SNR_curvemapping_w2_6;
	unsigned char SNR_curvemapping_w2_7;
	unsigned char SNR_curvemapping_w2_8;
	unsigned char SNR_reserved_41;
	unsigned char SNR_reserved_42;
	unsigned char SNR_reserved_43;
	unsigned char SNR_reserved_44;
	unsigned char SNR_reserved_45;
	unsigned char SNR_reserved_46;
	unsigned char SNR_reserved_47;
} DRV_SNR_Curve_Mapping_Mode;

typedef struct {
	unsigned char SNR_Mask1_En;				/*NR_Mask*/
	unsigned char SNR_Mask1_Dir;
	unsigned char SNR_Mask1_Y_Gain;
	unsigned char SNR_Mask1_Cb_Gain;
	unsigned char SNR_Mask1_Cr_Gain;
	unsigned char SNR_Mask1_Step;
	unsigned char SNR_Mask1_Thd;
	unsigned char SNR_Mask1_Limit;
	unsigned char SNR_Mask1_Y_Ref;
	unsigned char SNR_Mask1_Cb_Ref;
	unsigned char SNR_Mask1_Cr_Ref;
	unsigned char SNR_Mask2_En;
	unsigned char SNR_Mask2_Dir;
	unsigned char SNR_Mask2_Y_Gain;
	unsigned char SNR_Mask2_Cb_Gain;
	unsigned char SNR_Mask2_Cr_Gain;
	unsigned char SNR_Mask2_Step;
	unsigned char SNR_Mask2_Thd;
	unsigned char SNR_Mask2_Limit;
	unsigned char SNR_Mask2_Y_Ref;
	unsigned char SNR_Mask2_Cb_Ref;
	unsigned char SNR_Mask2_Cr_Ref;
	unsigned char SNR_Mask3_En;
	unsigned char SNR_Mask3_Dir;
	unsigned char SNR_Mask3_Y_Gain;
	unsigned char SNR_Mask3_Cb_Gain;
	unsigned char SNR_Mask3_Cr_Gain;
	unsigned char SNR_Mask3_Step;
	unsigned char SNR_Mask3_Thd;
	unsigned char SNR_Mask3_Limit;
	unsigned char SNR_Mask3_Y_Ref;
	unsigned char SNR_Mask3_Cb_Ref;
	unsigned char SNR_Mask3_Cr_Ref;
} DRV_SNR_NR_Mask;

typedef struct {	/* NR Mask W1/W2*/
	unsigned char SNR_mask_shift_mode;
	unsigned char SNR_nrm1_filter_select;
	unsigned char SNR_nrm1_w1_step;
	unsigned char SNR_nrm1_w1_limit;
	unsigned char SNR_nrm1_w2_step;
	unsigned char SNR_nrm1_w2_limit;
	unsigned char SNR_nrm2_filter_select;
	unsigned char SNR_nrm2_w1_step;
	unsigned char SNR_nrm2_w1_limit;
	unsigned char SNR_nrm2_w2_step;
	unsigned char SNR_nrm2_w2_limit;
	unsigned char SNR_nrm3_filter_select;
	unsigned char SNR_nrm3_w1_step;
	unsigned char SNR_nrm3_w1_limit;
	unsigned char SNR_nrm3_w2_step;
	unsigned char SNR_nrm3_w2_limit;
} DRV_SNR_NR_Mask_W1W2;

typedef struct {/*Veritcal_NR*/
	//unsigned char SNR_Vertical_fir_En;  //Merlin2 jyyang 20160815
	unsigned char SNR_Veritcal_avlpf_En;
	unsigned char SNR_Veritcal_C_En;
	unsigned char SNR_Veritcal_correct_En;
	unsigned char SNR_Veritcal_BW_En;
	unsigned char SNR_Veritcal_Y_Weight;
	unsigned char SNR_Veritcal_C_Weight;
	unsigned char SNR_Veritcal_Th_start;
	unsigned char SNR_Veritcal_Rate_start;
	unsigned char SNR_Veritcal_Th_finish;
	unsigned char SNR_Veritcal_Rate_finish;
	unsigned char SNR_Veritcal_Max_MinDiff_Weight;
	unsigned char SNR_Veritcal_Min_MinDiff_Weight;
	unsigned char SNR_Veritcal_MaxMinDiff_Th;
	unsigned char SNR_Veritcal_MaxMinDiff_Rate;
	unsigned char SNR_Veritcal_BWPattern_Th;
	unsigned char SNR_Veritcal_BWPattern_Margin;
	unsigned char SNR_Veritcal_BWPattern_Rate;
	unsigned char SNR_Vertical_Clamp_En;
	unsigned char SNR_Vertical_Clamp_Th;
	unsigned char SNR_Vertical_Clamp_Th_TTH; //Mac6
	unsigned char SNR_Vertical_IgnoreHLH; //Mac6
} DRV_SNR_Veritcal_NR;

typedef struct {
	unsigned char SNR_mosquitonr_weight;
	unsigned char SNR_mosquitonr_blending_mode;
	unsigned char SNR_noise_level_mode;
	unsigned char SNR_vertical_en; /*Mac3*/
	unsigned char SNR_edge_lpf_en; /*Mac3*/
	unsigned char SNR_mnr_decay_en; /*Mac3*/
	unsigned char SNR_reserved_6;
	unsigned char SNR_mosquito_edgethd;			/*Mosquito_NR_Edge Condition*/
	unsigned char SNR_mosquito_edgethd_step;
	unsigned char SNR_mosquitonr_edge_strength_gain;
	unsigned char SNR_hdecay_start;/*Mac3*/
	unsigned char SNR_hdecay_step;/*Mac3*/
	unsigned char SNR_vdecay_start;/*Mac3*/
	unsigned char SNR_vdecay_step;/*Mac3*/
	unsigned char SNR_farvar_faredge_refine_en;/*Mac3*/
	unsigned char SNR_far_var_gain;				/*Mosquito_NR_Noise level adjust*/
	unsigned char SNR_far_var_offset;/*Mac3*/
	unsigned char SNR_vardiff_refine_gain;/*Mac3*/
	unsigned char SNR_hdiff_frac_shift;/*Mac3*/
	unsigned char SNR_vdiff_frac_shift;/*Mac3*/
	unsigned char SNR_keep_edge_threshold;/*Mac3*/
	unsigned char SNR_keep_edge_threshold_step;/*Mac3*/
	unsigned char SNR_keep_edge_shift;/*Mac3*/
	short SNR_mosquitonr_nlevel_positive_offset;	/*Mosquito_NR_Final MNR Level Adjust*/
	unsigned char SNR_mosquitonr_nlevel_positive_shift;
	short SNR_mosquitonr_nlevel_negative_offset;
	unsigned char SNR_mosquitonr_nlevel_negative_shift;
	unsigned char MNR_adjustNoiseLevel_byType;
	unsigned char MNR_varDiff_refine_2Dsearch;
	unsigned char MNR_nLevel_trim;
	unsigned char MNR_hdiff_frac_half_range;
	unsigned char SNR_mnr_decay_allcomp_en;
} DRV_SNR_Mosquito_NR;

typedef struct {
	unsigned char SNR_LPF_sel; /*Merlin, jyyang*/
	unsigned char SNR_modified_lpf_edge_gain;  /*Mac3, leonard_wu@20140722*/
	unsigned short SNR_modified_lpf_thd_upbnd;  /*Mac3*/
	unsigned char SNR_modified_lpf_thd_lowbnd; /*Mac3*/
	unsigned char SNR_modified_lpf_thd_step;   /*Mac3*/
	unsigned char MNR_LPF_sel; /*Merlin*/
} DRV_SNR_Modified_LPF;

typedef struct {
	unsigned char SNR_sresultWeight_adjust_shift; /*Mag2, keepLine and typeDetect, Lydia*/
	unsigned short SNR_TypeDetect_MaxStep; // jyyang 20150706
	unsigned short SNR_TypeDetect_MaxThd; // jyyang 20150706
	unsigned short SNR_TypeDetect_MidStep; // jyyang 20150706
	unsigned short SNR_TypeDetect_MidThd; // jyyang 20150706
	unsigned char SNR_empty_5;
	unsigned char SNR_empty_6;
	unsigned char SNR_empty_7;
	unsigned char SNR_empty_8;
	unsigned char SNR_empty_9;
	unsigned char SNR_LineKeep_Shift;
	unsigned char SNR_LineKeep_Mode;
	unsigned char SNR_LineKeep_VerHorCtrl;
	unsigned char SNR_LineKeep_LineStrengthShift;
	unsigned char SNR_LineKeep_FlatFlagThd;
	unsigned char SNR_LineKeep_EdgeFlagThd;
	unsigned char SNR_LineKeep_HorLineJudgeRatio;
	unsigned char SNR_LineKeep_VerLineJudgeRatio;
	unsigned short SNR_LineKeep_YuvDiffStep;	//range 0-1023 // jyyang 20150706
	unsigned short SNR_LineKeep_YuvDiffThd; 	//range 0-1023 // jyyang 20150706
} DRV_SNR_WeightAdjust;


typedef struct {
	unsigned char SNR_uvedge_en; /*Merlin, uvedge, jyyang*/
	unsigned char SNR_uvedge_gain0;
	unsigned char SNR_uvedge_gain1;
	unsigned char SNR_uvedge_step0;
	unsigned char SNR_uvedge_step1;
	unsigned char SNR_uvedge_lowbd0;
	unsigned char SNR_uvedge_lowbd1;
} DRV_SNR_UVEdge;


typedef struct {
	DRV_SNR_General_ctrl 		S_SNR_General_ctrl;
	DRV_SNR_Spatial_NR_Y 		S_SNR_Spatial_NR_Y;
	DRV_SNR_Spatial_NR_C 		S_SNR_Spatial_NR_C;
	DRV_SNR_Modified_LPF		S_SNR_Modified_LPF;
	DRV_SNR_Impulse_NR			S_SNR_Impulse_NR;
	DRV_SNR_Curve_Mapping_Mode 	S_SNR_Curve_Mapping_Mode;
	DRV_SNR_NR_Mask				S_SNR_NR_Mask;
	DRV_SNR_NR_Mask_W1W2		S_SNR_NR_Mask_W1W2;
	DRV_SNR_Veritcal_NR			S_SNR_Veritcal_NR;
	DRV_SNR_Mosquito_NR 		S_SNR_Mosquito_NR;
	DRV_SNR_WeightAdjust        S_SNR_WeightAdjust;/*Mag2*/
	DRV_SNR_Spatial_NR_Y		S_SNR_VD_Spatial_NR_Y; /*Mac3*/
	DRV_SNR_UVEdge        		S_SNR_UVEdge;/*Merlin jyyang 20150615*/
} DRV_SNR_Table;

typedef struct {
	unsigned char MPG_Enable_H;
	unsigned char MPG_DI_Enable_V_Y;
	unsigned char MPG_DI_Enable_V_C;
	unsigned char MPG_SNR_Enabe_V_Y;
	unsigned char MPG_SNR_Enable_V_C;
} DRV_MPG_General_ctrl;

typedef struct {
	unsigned char MPG_ResultWeight_H;
	unsigned char MPG_Hoizontal_index_mode;
	unsigned char MPG_default_mode_filter;
	unsigned char MPG_Upper_Bound_Gain;
	unsigned int MPG_Upper_Bound_X;
	unsigned char MPG_Upper_Bound_Offset;
	unsigned char MPG_DC_mode_filter;
	unsigned char MPG_DC_Gain;
	unsigned char MPG_DC_Qp;
	unsigned char MPG_blend_range;
	unsigned char MPG_blend_xini;
	unsigned int MPG_blend_xstep;

	unsigned char MPG_dec_sel;
	unsigned int MPG_mpgub_delta1;
	unsigned int MPG_mpgub_delta2;
	unsigned int MPG_mpgub_delta3;
	unsigned char MPG_difcmp_en;
	unsigned char MPG_difcen_en;
	unsigned int MPG_diff_th_low;
	unsigned int MPG_diff_th_high;
} DRV_MPG_H;

typedef struct {
	unsigned char MPG_DI_Lower_Bound;
	unsigned char MPG_DI_Upper_Bound;
	unsigned char MPG_DI_Result_Weight_V;
	unsigned char MPG_DI_LP_mode;
	unsigned char MPG_DI_Idx_Mode_Y;
	unsigned char MPG_SNR_Lower_Bound;
	unsigned char MPG_SNR_Upper_Bound;
	unsigned char MPG_SNR_Result_Weight_V;
	unsigned char MPG_SNR_LP_mode;
	unsigned char MPG_SNR_Idx_Mode_Y;
} DRV_MPG_V;

typedef struct {
	short enable;
	short Reserved_01;
	short diff_th_combo;// NxtIC
	short diff_th_hlh;
	short bound_left;
	short bound_right;
	short bound_up;
	short bound_down;
	short diff_th_low;
	short diff_th_high;
	short scaling;
	short offset_pixel;
	short offset_step;
	short offset_forgotten;
	short subsampling_mode;
} DRV_MPEGNR_16_Detector;

typedef struct {
	short enable_y;
	short enable_c;
} DRV_MPEGNR_16_Function_Enable;

typedef struct {
	short scaling;
	short offset_pixel;
	short offset_step;
	short small_block;
	short skip_some_regions;
	short after_interlace;
} DRV_MPEGNR_16_Region_Separation;

typedef struct {
	short _0;
	short _1;
	short _2;
	short _3;
} DRV_MPEGNR_16_Upperbound;

typedef struct {
	short see_range;
	short th_high;
	short gain_high;
	short Reserved_03;
	short dec_sel;
	short delta1;
	short delta2;
	short delta3;
} DRV_MPEGNR_16_Flat_and_Dec;

typedef struct {
	short see_range;
	short th_high;
	short gain_high;
	short th_low;
	short gain_low;
} DRV_MPEGNR_16_Tune;

typedef struct {
	short lpf_sel;
	short result_weight;
} DRV_MPEGNR_16_LPF;
typedef struct {
	int x2l;
	int x2r;
	int x3l;
	int x3r;
	int y3l;
	int y3r;
	int x_ub_ub;
	int fw_ctrl1;
	int fw_ctrl2;
	int fw_ctrl3;
} DRV_MPEGNR_16_SW_CTRL;

typedef struct {
	DRV_MPEGNR_16_Detector S_MPEGNR_16_Detector;
	DRV_MPEGNR_16_Function_Enable S_MPEGNR_16_Function_Enable;
	DRV_MPEGNR_16_Region_Separation S_MPEGNR_16_Region_Separation;
	DRV_MPEGNR_16_Upperbound S_MPEGNR_16_Upperbound;
	DRV_MPEGNR_16_Flat_and_Dec S_MPEGNR_16_Flat_and_Dec;
	DRV_MPEGNR_16_Tune S_MPEGNR_16_Difcmp;
	DRV_MPEGNR_16_Tune S_MPEGNR_16_Difcen;
	DRV_MPEGNR_16_LPF S_MPEGNR_16_LPF;
	DRV_MPEGNR_16_SW_CTRL S_MPEGNR_SW_ctrl;
} DRV_MPEGNR_16_Table;

typedef struct {
	//DRV_MPG_General_ctrl		S_MPEGNR_General_ctrl;
	DRV_MPEGNR_16_Table		S_MPEGNR_H;
	DRV_MPEGNR_16_Table		S_MPEGNR_V;
} DRV_MPG_Table;

typedef struct {
	DRV_RTNR_Table 			S_RTNR_TABLE;
	DRV_SNR_Table 			S_SNR_TABLE;
	DRV_MPG_Table			S_MPG_TABLE;
} DRV_NR_Item;


typedef enum _DRV_NR_Level{
	DRV_NR_OFF = 0,
	DRV_NR_LOW,
	DRV_NR_MID,
	DRV_NR_HIGH,
	DRV_NR_AUTO,
	DRV_NR_Level_MAX,
} DRV_NR_Level;
/*#pragma pack()*/ /* restore original alignment from stack */

typedef struct {
	unsigned int DRV_MAD_counter_y;
	unsigned int DRV_MAD_counter_u;
	unsigned int DRV_MAD_counter_v;
} DRV_MAD_counter;

typedef struct {
	unsigned char UVC_main_en;
	unsigned char UVC_sub_en;
	unsigned char UVC_overlay_sel;
	unsigned char UVC_skin_sel;
	unsigned char UVC_qdrt1_en;
	unsigned char UVC_qdrt2_en;
	unsigned char UVC_qdrt3_en;
	unsigned char UVC_qdrt4_en;
	unsigned int UVC_y0;
	unsigned int UVC_y_slope;
} DRV_NEW_UVC_Ctrl;

typedef struct {
	unsigned char UVC_U_thd1;
	unsigned char UVC_U_slope1;
	unsigned char UVC_U_thd2;
	unsigned char UVC_U_slope2;
	unsigned char UVC_V_thd1;
	unsigned char UVC_V_slope1;
	unsigned char UVC_V_thd2;
	unsigned char UVC_V_slope2;
} DRV_NEW_UVC_set;

typedef struct {
	DRV_NEW_UVC_Ctrl UVC_ctrl;
	DRV_NEW_UVC_set UVC_set_global;
	DRV_NEW_UVC_set UVC_set_skin;
} DRV_NEW_UVC;


typedef struct {
	unsigned char DeCT_en;
	/*unsigned char DeCT_DEBUG;*/
} DRV_De_CT_1D_Ctrl;

typedef struct {
	unsigned char K0;
	unsigned char K1;
	unsigned char K2;
	unsigned char K3;
	unsigned char K4;
	unsigned char K5;
	unsigned char LP_SHIFT;
} DRV_De_CT_1D_Tap;

typedef struct {
	unsigned int W1;
	unsigned int W2;
	unsigned char W1W2;
	unsigned char MIN_GAP_W3;
	unsigned int GAP_W3;
	unsigned char SMOOTH_GAP_W;
	unsigned char ALL_CHANNEL_W;
} DRV_De_CT_1D_Set;

typedef struct {
	DRV_De_CT_1D_Ctrl DeContour_ctrl;
	DRV_De_CT_1D_Tap DeContour_tap;
	DRV_De_CT_1D_Set DeContour_set;
} DRV_De_CT_1D;

typedef struct {
	unsigned char main_en;
	unsigned char sub_en;
	unsigned char sat_thd;
	unsigned char r_gain; //20150709 caroline
	unsigned char g_gain; //20150709 caroline
	unsigned char b_gain; //20150709 caroline
} DRV_Gamma_BS_Ctrl;

typedef struct {
	unsigned char yindx[8];
} DRV_Gamma_BS_Y;

typedef struct {
	unsigned short ofst[8];
	unsigned char step[8];
} DRV_Gamma_BS_OfstStp;

typedef struct {
	DRV_Gamma_BS_Ctrl GammaBS_ctrl;
	DRV_Gamma_BS_Y GammaBS_y;
	DRV_Gamma_BS_OfstStp GammaBS_R;
	DRV_Gamma_BS_OfstStp GammaBS_G;
	DRV_Gamma_BS_OfstStp GammaBS_B;
} DRV_Gamma_BS;

typedef struct {
	unsigned char IESM_En;
	unsigned char IESM_Mixer_Blend;
	unsigned char IESM_Mixer_Weight;
} DRV_IESM_Basic;

typedef struct {
	unsigned char DLTI_2D_Gain;
	unsigned char DLTI_2D_MaxMin_En;
	unsigned char DLTI_2D_Chaos_Gain;
	unsigned char DLTI_2D_VLPF;
	unsigned char DLTI_2D_HLPF;
} DRV_DLTI_2D;

typedef struct {
	unsigned char IESM_skip_hlh;;// IESM_dir3_LPF;
	unsigned char IESM_level;
	unsigned char IESM_smallword_weak;
	unsigned char IESM_delta_weak;
	unsigned char IESM_delta_limit;

	unsigned char IESM_motion_gain;
	unsigned char IESM_3tap_en;// Merlin4
	unsigned char IESM_3tap_gain;// Merlin4
	unsigned char IESM_5tap_gain;// Merlin4
	unsigned char IESM_8diff_coring;// Merlin4

	unsigned char IESM_particle2;// nxtic
	unsigned char IESM_delta_tex_seg;// avoid texture blur
	unsigned char IESM_nondir_tex_seg;// avoid texture blur
	unsigned char IESM_Unstable_area_thd;
} DRV_IESM_Additional_setting;

typedef struct {
	DRV_IESM_Basic				S_IESM_Basic;
	//DRV_DLTI_2D					S_DLTI_2D; //mac6p removed IESM DLTI2D
	DRV_IESM_Additional_setting	S_IESM_Additional_setting;
} DRV_IEdgeSmooth_Coef;

typedef struct {
	unsigned short XOffset;
	unsigned short YOffset;
	unsigned short ZOffset;
} DRV_CM_In_Offset;

typedef struct {
	unsigned short K11;
	unsigned short K12;
	unsigned short K13;
	unsigned short K21;
	unsigned short K22;
	unsigned short K23;
	unsigned short K31;
	unsigned short K32;
	unsigned short K33;
} DRV_CM_Data;

typedef struct {
	unsigned short ROffset;
	unsigned short GOffset;
	unsigned short BOffset;
} DRV_CM_Out_Offset;

#define Color_Map_Table_NUM 10

typedef struct {
	unsigned char Color_Mapping_En;
	DRV_CM_In_Offset CM_InOffset;
	DRV_CM_Data CM_Data;
	DRV_CM_Out_Offset CM_OutOffset;
} DRV_Color_Mapping;

/*===========================================================================*/
/*=== About DI ======*/
/*===========================================================================*/

#define DI_MA_Adjust_Table_MAX 11
#define di_TNR_XC_table_MAX 9
#define gHMETable_MAX  7
#define gHMCTable_MAX  7
#define gPANTable_MAX  7
#define FMV_Hist_Bin_Num 21
#define HMC_Hist_Bin_Num 33
#define new_UVC_MAX 4
#define Blue_Stretch_MAX 4
#define Color_Mapping_MAX 2
/*===========================================================================*/
/*=== About DCC Structure ======*/
/*===========================================================================*/

#define COLOR_D_HISTOGRAM_LEVEL 128      /*add D-histogram from Merlin2, 20160705*/
#define COLOR_HISTOGRAM_LEVEL 128       /* Y histogram level*/ /*add 128 bin from Merlin2, 20160624*/
#define TV006_VPQ_chrm_bin 32
#define COLOR_HISTOGRAM_DATA_BIN_NUM 16       /* Y histogram data bin*/
#define COLOR_HISTOGRAM_SECTION_LEVEL_MAX 8
#define COLOR_HISTOGRAM_MODE1_SECTION_LEVEL_MAX 2
#define COLOR_HUE_HISTOGRAM_LEVEL 24       /* Hue histogram MAX*/
#define COLOR_AutoSat_HISTOGRAM_LEVEL 32       /* Sat histogram MAX*/
#define DCC_Curve_Node_MAX	129
/*o---- Table Max defined ---o*/
#define Histogram_adjust_bin_num	32
#define DCCHist_Factor_Table_MAX	10
#define DCC_skin_tone_TABLE_MAX	7
#define Color_Independent_Blending_Table_MAX	10
#define USER_DEFINE_CURVE_DCC_TBL_NUM	10
#define USER_DEFINE_CURVE_DCC_SEGMENT_NUM	129
#define Database_DCC_Curve_TABLE_MAX	10
#define Database_DCC_Curve_Case_Item_MAX	10
#define DCC_Curve_DataRang_bit 10
#define DCC_Curve_Segment_bit 8
#define DCC_CURVE_Segment_NUM	129
#define DCC_CURVE_User_Gain_Parameter   4
#define DCC_Curve_Adjust_TABLE_MAX	10
#define DCC_Chroma_Compensation_TABLE_MAX	10
#define Chroma_Gain_Seg_MAX	65
#define Chroma_Gain_Limit_Seg_MAX 8
#define DCC_AdaptCtrl_Level_TABLE_MAX 10
#define DCC_coef_Control_Table_MAX	10
#define DCC_Boundary_Check_Table_MAX	10
#define DCC_Level_and_Blend_Coef_Table_MAX	10
#define DCC_Function_Enable_Ctl_Table_MAX 10
/*o--- DCC Curve point num and histogram bin ---o*/
#define	offset_bin_16_235 2
#define HighResOffset_Bin_16_235 8
#define HighRes_bin_num_Max 128
#define	bin_Curve_num_Max (COLOR_HISTOGRAM_LEVEL+1)//33
#define Curve_zero_point 8
#define Curve_max_point 118
#define Curve_num_Max 129
#define TableSeg_num_Max 64
#define DataRange_MAX ((1<<DCC_Curve_DataRang_bit)-1)

#define DeCT_1D_Level_MAX 2

#define DCC_HistShiftBin 3 /*hist 256(8bit), bin number 32, 256/32=2^3*/
#define DCC_OutputShift 2 /*output 8bit to 10 bit*/

typedef struct {
	unsigned int HistCnt_Of_Adj[COLOR_HISTOGRAM_LEVEL];
	int Histogram_Curve[DCC_Curve_Node_MAX];
	int S_Curve[DCC_Curve_Node_MAX];
	int Expand_Curve[DCC_Curve_Node_MAX];
	int User_Curve[DCC_Curve_Node_MAX];
	int Database_Curve[DCC_Curve_Node_MAX];
	int OSD_Weight_Curve[DCC_Curve_Node_MAX];
	int Apply_Curve[DCC_Curve_Node_MAX];
	int ByPass_Curve[DCC_Curve_Node_MAX];
	unsigned int Database_entry_Histogram[Histogram_adjust_bin_num];
} RPC_DCC_LAYER_Array_Struct;

typedef enum _VIP_DCC_Advance_control_info_Item {
	DCC_Tool_mean = 0,
	DCC_Tool_Dev,
	DCC_mean_no_debounce,
	DCC_Dev_no_debounce,
	DCC_Tool_Item_Max,
} VIP_DCC_Advance_control_info_Item;

typedef struct {
	unsigned int database_weight;
	unsigned int database_case_num;
	unsigned int Adapt_Mean_Dev_info[DCC_Tool_Item_Max];
	RPC_DCC_LAYER_Array_Struct S_RPC_DCC_LAYER_Array;
} Adapt_DCC_Info_Item;

typedef enum _DCC_SELECT {
	DCC_SELECT_OFF = 0,
	DCC_SELECT_LOW,
	DCC_SELECT_MIDDLE,
	DCC_SELECT_HIGH,
	DCC_SELECT_Default,
	DCC_SELECT_MAX,
} DCC_SELECT;

typedef enum _DCC_Histogram_section_num {
	Hist_section0 = 0,
	Hist_section1,
} DCC_Histogram_section_num;

typedef enum _DCC_Histogram_color_channel {
	Hist_color_channel_0 = 0,
	Hist_color_channel_1,
} DCC_Histogram_color_channel;

/* ********************ADV API for off low middle high option api **************************/
#define ADV_API_TBL_MAX 10

typedef enum _VIP_ADV_API_LEVEL {
	ADV_API_LEVEL_OFF = 0,
	ADV_API_LEVEL_LOW,
	ADV_API_LEVEL_MIDDLE,
	ADV_API_LEVEL_HIGH,

    VIP_ADV_API_LEVEL_Max,
} VIP_ADV_API_LEVEL, VIP_DCC_ADV_API_LEVEL;

typedef struct _VIP_ADV_API_CE_TBL {
	unsigned char Enable_Flag;
	unsigned char CE_Lv;

} VIP_ADV_API_CE_TBL, VIP_DCC_ADV_API_CE_TBL;

typedef struct _VIP_ADV_API_sGamma_TBL {
	unsigned char Enable_Flag;
	unsigned char sGamma_Lv;

} VIP_ADV_API_sGamma_TBL, VIP_DCC_ADV_API_sGamma_TBL;

typedef struct _VIP_ADV_API_BLE_TBL {
	unsigned char Enable_Flag;
	unsigned char BLE_Lv;
	unsigned char isBLE_Lv_Reverse;
	unsigned char BLE_Range;

} VIP_ADV_API_BLE_TBL, VIP_DCC_ADV_API_BLE_TBL;

typedef struct _VIP_ADV_API_CTRL {
	unsigned char Debug_Log_Flag_0;
	unsigned char Debug_Log_Flag_1;
	unsigned char Debug_Log_Flag_2;
	unsigned char Debug_Log_Flag_3;
	unsigned char Debug_Log_Delay;
	unsigned char TBL_Select_Level_Table;
	unsigned char TBL_Select_CE;
	unsigned char TBL_Select_sGamma;
	unsigned char TBL_Select_BLE;
	unsigned char is_sGamma_Neg;
	unsigned char extend_Mode;
	unsigned char Reserve;

} VIP_ADV_API_CTRL, VIP_DCC_ADV_API_CTRL;

typedef struct _VIP_ADV_API_TBL {
	VIP_ADV_API_CE_TBL ADV_API_CE_TBL;
	VIP_ADV_API_sGamma_TBL ADV_API_sGamma_TBL;
	VIP_ADV_API_BLE_TBL ADV_API_BLE_TBL;

} VIP_ADV_API_TBL, VIP_DCC_ADV_API_TBL;

typedef struct _VIP_ADV_API {
	unsigned char VIP_ADV_API_Level_TBL[ADV_API_TBL_MAX][VIP_ADV_API_LEVEL_Max];
	VIP_ADV_API_CTRL ADV_API_CTRL;
	VIP_ADV_API_TBL	ADV_API_TBL[ADV_API_TBL_MAX];

} VIP_ADV_API, VIP_DCC_ADV_API;

/*======= typedef struct :  VIP_DCC_Skin_Tone_Y_Blending_Item =======*/
typedef struct {
	short y_blending_en;
	short y_blending_mode;
	short colorspace_sel;
} Color_Independent_Settings;	//20160613 chance_qian

typedef struct {
	short region0_en;
	short Y_Center0;
	short U_Center0;
	short V_Center0;
	short Y_Range0;
	short U_Range0;
	short V_Range0;
	short Segment0_0;
	short Segment0_1;
	short Segment0_2;
	short Segment0_3;
	short Segment0_4;
	short Segment0_5;
	short Segment0_6;
	short Segment0_7;
	short Segment0_0_c; //20150708 chance_qian
	short Segment0_1_c; //20150708 chance_qian
	short Segment0_2_c; //20150708 chance_qian
	short Segment0_3_c; //20150708 chance_qian
	short Segment0_4_c; //20150708 chance_qian
	short Segment0_5_c; //20150708 chance_qian
	short Segment0_6_c; //20150708 chance_qian
	short Segment0_7_c; //20150708 chance_qian
} Color_Independent_Region0_Coef;

typedef struct {
	short region1_en;
	short Y_Center1;
	short U_Center1;
	short V_Center1;
	short Y_Range1;
	short U_Range1;
	short V_Range1;
	short Segment1_0;
	short Segment1_1;
	short Segment1_2;
	short Segment1_3;
	short Segment1_4;
	short Segment1_5;
	short Segment1_6;
	short Segment1_7;
	short Segment1_0_c; //20150708 chance_qian
	short Segment1_1_c; //20150708 chance_qian
	short Segment1_2_c; //20150708 chance_qian
	short Segment1_3_c; //20150708 chance_qian
	short Segment1_4_c; //20150708 chance_qian
	short Segment1_5_c; //20150708 chance_qian
	short Segment1_6_c; //20150708 chance_qian
	short Segment1_7_c; //20150708 chance_qian
	short Reverse15;
} Color_Independent_Region1_Coef;

typedef struct {
	short region2_en;
	short Y_Center2;
	short U_Center2;
	short V_Center2;
	short Y_Range2;
	short U_Range2;
	short V_Range2;
	short Segment2_0;
	short Segment2_1;
	short Segment2_2;
	short Segment2_3;
	short Segment2_4;
	short Segment2_5;
	short Segment2_6;
	short Segment2_7;
	short Segment2_0_c; //20150708 chance_qian
	short Segment2_1_c; //20150708 chance_qian
	short Segment2_2_c; //20150708 chance_qian
	short Segment2_3_c; //20150708 chance_qian
	short Segment2_4_c; //20150708 chance_qian
	short Segment2_5_c; //20150708 chance_qian
	short Segment2_6_c; //20150708 chance_qian
	short Segment2_7_c; //20150708 chance_qian
} Color_Independent_Region2_Coef;

typedef struct {
	short region3_en;
	short Y_Center3;
	short U_Center3;
	short V_Center3;
	short Y_Range3;
	short U_Range3;
	short V_Range3;
	short Segment3_0;
	short Segment3_1;
	short Segment3_2;
	short Segment3_3;
	short Segment3_4;
	short Segment3_5;
	short Segment3_6;
	short Segment3_7;
	short Segment3_0_c; //20150708 chance_qian
	short Segment3_1_c; //20150708 chance_qian
	short Segment3_2_c; //20150708 chance_qian
	short Segment3_3_c; //20150708 chance_qian
	short Segment3_4_c; //20150708 chance_qian
	short Segment3_5_c; //20150708 chance_qian
	short Segment3_6_c; //20150708 chance_qian
	short Segment3_7_c; //20150708 chance_qian
} Color_Independent_Region3_Coef;

typedef struct {
	Color_Independent_Settings		S_DCC_Skin_Tone_Settings;
	Color_Independent_Region0_Coef S_DCC_Skin_Tone_Y_Region0_Coef;
	Color_Independent_Region1_Coef S_DCC_Skin_Tone_Y_Region1_Coef;
	Color_Independent_Region2_Coef S_DCC_Skin_Tone_Y_Region2_Coef;
	Color_Independent_Region3_Coef S_DCC_Skin_Tone_Y_Region3_Coef;
} VIP_DCC_Color_Independent_Blending_Table;

typedef enum _VIP_DCC_Curve_Control_Item {
	Boundary_Check_Table,
	Level_and_Blend_Coef_Table,
	hist_adjust_table,
	AdaptCtrl_Level_Table,
	User_Curve_Table,
	database_DCC_Table,
	Picture_Mode_Weight,
	DCC_Curve_Control_ITEM_MAX,
} VIP_DCC_Curve_Control_Item;

typedef struct {
	unsigned char	Boundary_Check_Table;
	unsigned char	Level_and_Blend_Coef_Table;
	unsigned char	hist_adjust_table;
	unsigned char	AdaptCtrl_Level_Table;
	unsigned char	User_Curve_Table;
	unsigned char	database_DCC_Table;
	unsigned char	Picture_Mode_Weight;
	unsigned char	boundary_type;
	unsigned char	S_Curve_mean_mapping_table_select;
	unsigned char	reserved9;
	unsigned char	reserved10;
	unsigned char	reserved11;
	unsigned char	reserved12;
	unsigned char	reserved13;
	unsigned char	reserved14;
	unsigned char	reserved15;
} VIP_DCC_Curve_Control_Coef;

typedef struct {
	unsigned char	Histogram_DCC_Level;
	unsigned char	DCL_level_W;
	unsigned char	DCL_level_B;
	unsigned char	S_Curve_Level_High;
	unsigned char	S_Curve_Level_low;
	unsigned char	S_Curve_Index;
	unsigned char	DCC_Extend_Curve_style;
	unsigned char	Extend_Level_W;
	unsigned char	Extend_Level_B;
	unsigned char	Extend_Index_W;
	unsigned char	Extend_Index_B;
	unsigned char	Blending_Delay_time;
	unsigned char	Blending_Step;
	unsigned char 	Hist_Adjust_table_Select;
	unsigned char	HistMean_Th;
	unsigned char	User_Define_Curve_Table_Select;
} VIP_DCC_Curve_Adjust_Item;

/*=== typedef struct : for histogram adjust ===*/
typedef struct {
	unsigned char	bin0;
	unsigned char	bin1;
	unsigned char	bin2;
	unsigned char	bin3;
	unsigned char	bin4;
	unsigned char	bin5;
	unsigned char	bin6;
	unsigned char	bin7;
	unsigned char	bin8;
	unsigned char	bin9;
	unsigned char	bin10;
	unsigned char	bin11;
	unsigned char	bin12;
	unsigned char	bin13;
	unsigned char	bin14;
	unsigned char	bin15;
	unsigned char	bin16;
	unsigned char	bin17;
	unsigned char	bin18;
	unsigned char	bin19;
	unsigned char	bin20;
	unsigned char	bin21;
	unsigned char	bin22;
	unsigned char	bin23;
	unsigned char	bin24;
	unsigned char	bin25;
	unsigned char	bin26;
	unsigned char	bin27;
	unsigned char	bin28;
	unsigned char	bin29;
	unsigned char	bin30;
	unsigned char	bin31;
} VIP_Histogram_Adjust_Gain;

typedef struct {
	unsigned char	bin0;
	unsigned char	bin1;
	unsigned char	bin2;
	unsigned char	bin3;
	unsigned char	bin4;
	unsigned char	bin5;
	unsigned char	bin6;
	unsigned char	bin7;
	unsigned char	bin8;
	unsigned char	bin9;
	unsigned char	bin10;
	unsigned char	bin11;
	unsigned char	bin12;
	unsigned char	bin13;
	unsigned char	bin14;
	unsigned char	bin15;
	unsigned char	bin16;
	unsigned char	bin17;
	unsigned char	bin18;
	unsigned char	bin19;
	unsigned char	bin20;
	unsigned char	bin21;
	unsigned char	bin22;
	unsigned char	bin23;
	unsigned char	bin24;
	unsigned char	bin25;
	unsigned char	bin26;
	unsigned char	bin27;
	unsigned char	bin28;
	unsigned char	bin29;
	unsigned char	bin30;
	unsigned char	bin31;
} VIP_Histogram_Adjust_Offset;

typedef struct {
	unsigned char	bin0;
	unsigned char	bin1;
	unsigned char	bin2;
	unsigned char	bin3;
	unsigned char	bin4;
	unsigned char	bin5;
	unsigned char	bin6;
	unsigned char	bin7;
	unsigned char	bin8;
	unsigned char	bin9;
	unsigned char	bin10;
	unsigned char	bin11;
	unsigned char	bin12;
	unsigned char	bin13;
	unsigned char	bin14;
	unsigned char	bin15;
	unsigned char	bin16;
	unsigned char	bin17;
	unsigned char	bin18;
	unsigned char	bin19;
	unsigned char	bin20;
	unsigned char	bin21;
	unsigned char	bin22;
	unsigned char	bin23;
	unsigned char	bin24;
	unsigned char	bin25;
	unsigned char	bin26;
	unsigned char	bin27;
	unsigned char	bin28;
	unsigned char	bin29;
	unsigned char	bin30;
	unsigned char	bin31;
} VIP_Histogram_Adjust_Limit;

typedef struct {
	VIP_Histogram_Adjust_Gain	S_Histogram_Adjust_Gain;
	VIP_Histogram_Adjust_Offset S_Histogram_Adjust_Offset;
	VIP_Histogram_Adjust_Limit	S_Histogram_Adjust_Limit;
} VIP_Histogram_Adjust_Item;

/*=== typedef struct :  for user curve adjust mean value th ====*/

typedef struct {
	unsigned char	APL_th0;
	unsigned char	APL_th1;
	unsigned char	APL_th2;
	unsigned char	APL_th3;
	unsigned char	APL_th4;
	unsigned char	APL_th5;
	unsigned char	APL_th6;
	unsigned char	APL_th7;
	unsigned char	APL_th8;
	unsigned char	USER_DEFINE_CURVE_DCC_CURVE_NUM;
} USER_CURVE_DCC_HisMean_th_ITEM;

typedef struct {
	unsigned char	APL_seg0_WEIGHTING;
	unsigned char	APL_seg1_WEIGHTING;
	unsigned char	APL_seg2_WEIGHTING;
	unsigned char	APL_seg3_WEIGHTING;
	unsigned char	APL_seg4_WEIGHTING;
	unsigned char	APL_seg5_WEIGHTING;
	unsigned char	APL_seg6_WEIGHTING;
	unsigned char	APL_seg7_WEIGHTING;
	unsigned char	APL_seg8_WEIGHTING;
	unsigned char	APL_seg9_WEIGHTING;
	unsigned char	APL_skin_WEIGHTING;
} USER_CURVE_DCC_HisSeg_Weighting_ITEM;

typedef struct {

	USER_CURVE_DCC_HisMean_th_ITEM USER_CURVE_DCC_HisMean_th;
	USER_CURVE_DCC_HisSeg_Weighting_ITEM USER_CURVE_DCC_HisSeg_Weighting;

} USER_CURVE_Weight_Mapping_ITEM;


typedef enum _USER_DEFINE_CURVE_DCC_TABLE_ITEM {
	USER_DEFINE_CURVE_DCC_CURVE_LEVEL0 = 0,
	USER_DEFINE_CURVE_DCC_CURVE_LEVEL1,
	USER_DEFINE_CURVE_DCC_CURVE_LEVEL2,
	USER_DEFINE_CURVE_DCC_CURVE_LEVEL3,
	USER_DEFINE_CURVE_DCC_CURVE_LEVEL4,
	USER_DEFINE_CURVE_DCC_CURVE_LEVEL5,
	USER_DEFINE_CURVE_DCC_CURVE_LEVEL6,
	USER_DEFINE_CURVE_DCC_CURVE_LEVEL7,
	USER_DEFINE_CURVE_DCC_CURVE_LEVEL8,
	USER_DEFINE_CURVE_DCC_CURVE_SKIN,
	USER_DEFINE_CURVE_DCC_CURVE_MAX,
} USER_DEFINE_CURVE_DCC_TABLE_ITEM;

typedef enum _USER_DEFINE_CURVE_DCC_CRTL_ITEM {
	USER_DEFINE_CURVE_DCC_TH0 = 0,
	USER_DEFINE_CURVE_DCC_TH1,
	USER_DEFINE_CURVE_DCC_TH2,
	USER_DEFINE_CURVE_DCC_TH3,
	USER_DEFINE_CURVE_DCC_TH4,
	USER_DEFINE_CURVE_DCC_TH5,
	USER_DEFINE_CURVE_DCC_TH6,
	USER_DEFINE_CURVE_DCC_TH7,
	USER_DEFINE_CURVE_DCC_TH8,
	USER_DEFINE_CURVE_DCC_WEIGHTING0,
	USER_DEFINE_CURVE_DCC_CURVE_NUM,
	USER_DEFINE_CURVE_DCC_WEIGHTING_skin,
	USER_DEFINE_CURVE_DCC_Reverse2,
	USER_DEFINE_CURVE_DCC_CRTL_ITEM_MAX,
} USER_DEFINE_CURVE_DCC_CRTL_ITEM;

typedef struct {

	USER_CURVE_Weight_Mapping_ITEM USER_CURVE_Weight_Mapping;
	unsigned int  user_define_curve_dcc_table[USER_DEFINE_CURVE_DCC_CURVE_MAX][USER_DEFINE_CURVE_DCC_SEGMENT_NUM];
} VIP_USER_DEFINE_CURVE_DCC_CRTL_ITEM;

/*=== typedef struct :  for database DCC ====*/

typedef struct {
	unsigned int database_histogram[Histogram_adjust_bin_num];
	unsigned int database_curve[DCC_CURVE_Segment_NUM];
	unsigned int database_adjust[DCC_CURVE_User_Gain_Parameter];  //MHH
} VIP_DCC_Database_Curve_CRTL_Table;

/*=== typedef struct :  for DCC Chroma Compensation ====*/
typedef struct {
	unsigned char	gain_en;
	unsigned char	gain_limit_en;
	unsigned char	gain_mode;
	unsigned char	gain_lookup_mode;
	unsigned char	gain_base;
	unsigned char	enh_mode_above_tab_sel;
	unsigned char	enh_mode_below_tab_sel;
	unsigned char	byY_mode_u_tab_sel;
	unsigned char 	byY_mode_v_tab_sel;
} Chroma_Compensation_Ctrl_Item;

typedef struct {
	Chroma_Compensation_Ctrl_Item Chroma_Compensation_Ctrl;
	int	chroma_gain_t0[Chroma_Gain_Seg_MAX];/*gain_mode=0: Table0 gain, gain_mode=1: Modify gain_t0*/
	int	chroma_gain_t1[Chroma_Gain_Seg_MAX];/*gain_mode=0: Table1 gain, gain_mode=1: Modify offset_t0*/
	unsigned int	chroma_gain_limit[Chroma_Gain_Limit_Seg_MAX];
} VIP_DCC_Chroma_Compensation_TABLE;


/*=== typedef struct :  for advance control DCC ====*/

typedef enum _VIP_DCC_Cubic_Curve_mean_Value_Index {	/* 20140411 CSFC*/
	mean_value_index_0 = 0,
	mean_value_index_1,
	mean_value_index_2,
	mean_value_index_3,
	mean_value_index_4,
	mean_value_index_5,
	mean_value_index_6,
	mean_value_index_7,
	mean_value_index_8,
	mean_value_index_9,
	mean_value_index_10,
	mean_value_index_11,
	mean_value_index_12,
	mean_value_index_13,
	mean_value_index_14,
	mean_value_index_15,
	mean_value_index_16,
	mean_value_index_17,
	mean_value_index_18,
	mean_value_index_19,
	mean_value_index_20,
	mean_value_index_21,
	mean_value_index_22,
	mean_value_index_23,
	mean_value_index_24,
	mean_value_index_25,
	mean_value_index_26,
	mean_value_index_27,
	mean_value_index_28,
	mean_value_index_29,
	mean_value_index_30,
	mean_value_index_31,
	mean_value_index_MAX,
} VIP_DCC_Cubic_Curve_mean_Value_Index;

typedef enum _VIP_DCC_Cubic_Curve_Variance_Index {	/* 20140411 CSFC*/
	Variance_index_0 = 0,
	Variance_index_1,
	Variance_index_2,
	Variance_index_3,
	Variance_index_4,
	Variance_index_5,
	Variance_index_6,
	Variance_index_7,
	Variance_index_8,
	Variance_index_9,
	Variance_index_10,
	Variance_index_11,
	Variance_index_12,
	Variance_index_13,
	Variance_index_MAX,
} VIP_DCC_Cubic_Curve_Variance_Index;

typedef enum _VIP_DCC_table_number_of_S_curve_mean {
	table_number_of_S_curve_mean_0 = 0,
	table_number_of_S_curve_mean_1,
	table_number_of_S_curve_mean_MAX,
} VIP_DCC_table_number_of_S_curve_mean;

typedef enum _VIP_DCC_Boundary_check_condiction {
	Boundary_check_condiction_enable = 0,
	Black_th_ratio_seg0,
	Black_th_ratio_seg1,
	white_th_ratio_seg0,
	white_th_ratio_seg1,
	Black_Bounday_seg1_limit,
	white_Bounday_seg1_limit,
	Black_Bounday_search_range,
	white_Bounday_search_range,
	Bin_debounce_th_Black,
	Bin_debounce_th_white,
	adapt_level_debounce_th,
	Boundary_check_condiction_MAX,
} VIP_DCC_Boundary_check_condiction;

typedef enum _VIP_DCC_LPF_control {
	Hist_LPF_enable = 0,
	Hist_LPF_tap,
	Curve_LPF_enable,
	Curve_LPF_tap,
	EQ_slope_limit_weighting_offset,
	EQ_slope_limit_slope_thershold,
	LPF_control_Item_MAX,
} VIP_DCC_LPF_control;

typedef enum _VIP_DCC_init_boundary_Flag {
	Bin_Boundary_Flag = 0,
	Limit_range_Hist_Cut_Flag,
	Fix_output_range_16_235,
	DCC_init_boundary_Item_MAX,
} VIP_DCC_init_boundary_Flag;


typedef enum _VIP_DCC_Advance_Level_Control_Flag {
	Advance_Level_General_Control_enable = 0,
	Advance_Level_by_mean_dev_enable,
	Advance_Cublc_by_mean_dev_enable,
	Advance_control_only_by_mean_enable,
	Advance_Level_Control_Item_MAX,
} VIP_DCC_Advance_Level_Control_Flag;


#define Advance_control_table_MAX 4
#define Advance_control_table_num ((mean_value_index_MAX*Variance_index_MAX*4)+Boundary_check_condiction_MAX+LPF_control_Item_MAX)


typedef struct {
	unsigned char DCC_Advance_Level_Control_Flag[Advance_Level_Control_Item_MAX];
	unsigned char DCC_LPF_Control[LPF_control_Item_MAX];
	unsigned char DCC_init_boundary_Type;
	unsigned char DCC_Curve_Blending_Step_table[mean_value_index_MAX][Variance_index_MAX];
	unsigned char DCC_Curve_Blending_DT_table[mean_value_index_MAX][Variance_index_MAX];
} RPC_DCC_Advance_control_table;


#define Curve_boundary_table_MAX 13

typedef struct {
	unsigned char Bin_Boundary_Flag; //for DCC his bin boundary
	unsigned char Limit_range_Hist_Cut_Flag; //for DCC his bin boundary
	unsigned char Limit_range_Hist_Cut_start;
	unsigned char Limit_range_Hist_Cut_end;
	unsigned char Fix_output_range_Flag; //for DCC Curve boundary
	unsigned char Fix_output_range_start;
	unsigned char Fix_output_range_end;
	unsigned char Linear_black_Flag; //for DCC Curve linearity
	unsigned char Linear_white_Flag;
	unsigned char panel_compensation_Flag;
} RPC_DCC_Curve_boundary_table;

typedef struct {
	unsigned char DCC_Level_table[mean_value_index_MAX][Variance_index_MAX];
	unsigned char DCC_Cublc_Index_table[mean_value_index_MAX][Variance_index_MAX];
	unsigned char DCC_Cublc_High_table[mean_value_index_MAX][Variance_index_MAX];
	unsigned char DCC_Cublc_Low_table[mean_value_index_MAX][Variance_index_MAX];
	unsigned char DCC_W_Ex_table[mean_value_index_MAX][Variance_index_MAX];
	unsigned char DCC_B_Ex_table[mean_value_index_MAX][Variance_index_MAX];
} VIP_DCC_AdaptCtrl_Level_Table;

typedef enum _VIP_DCC_Advance_blending_control_info_Item {
	DCC_mean_diff = 0,
	DCC_Dev_diff,
	DCC_blending_control_Item_Max,
} VIP_DCC_Advance_blending_control_info_Item;

typedef struct {

	unsigned char Boundary_check_condiction_enable;
	unsigned char Black_th_ratio_seg0;
	unsigned char Black_th_ratio_seg1;
	unsigned char white_th_ratio_seg0;
	unsigned char white_th_ratio_seg1;
	unsigned char Black_Bounday_seg1_limit;
	unsigned char white_Bounday_seg1_limit;
	unsigned char Black_Bounday_search_range;
	unsigned char white_Bounday_search_range;
	unsigned char Bin_debounce_th_Black;
	unsigned char Bin_debounce_th_white;
	unsigned char adapt_level_debounce_th;
} VIP_DCC_Boundary_check_Table;

typedef struct {

	unsigned char	DCL_W_level;
	unsigned char	DCL_B_level;
	unsigned char	Delay_time;
	unsigned char	Step;
	unsigned char 	DCL_Eable;
	unsigned char 	Expand_Eable;
	unsigned char 	S_Curve_Enable;
	unsigned char 	Hist_Adjust_Enable;
	unsigned char 	User_Curve_Eable;
	unsigned char 	Database_Enable;
} VIP_DCC_Level_and_Blend_Coef_Table;

typedef struct {

	unsigned char	Mode;
	unsigned char	bin_th0;
	unsigned char	bin_th1;
} VIP_DCC_Hist_init_coef;

typedef struct {

	VIP_DCC_Hist_init_coef	Hist_init_coef;
	unsigned char	gain[Histogram_adjust_bin_num];
	unsigned char	offset[Histogram_adjust_bin_num];
	unsigned char	limit[Histogram_adjust_bin_num];
} VIP_DCC_Hist_Factor_Table;


typedef struct {
	VIP_DCC_Curve_Control_Coef	DCC_Curve_Control_Coef[DCC_Curve_Adjust_TABLE_MAX][DCC_SELECT_MAX];
	VIP_DCC_Boundary_check_Table DCC_Boundary_check_Table[DCC_Boundary_Check_Table_MAX];
	VIP_DCC_Level_and_Blend_Coef_Table	DCC_Level_and_Blend_Coef_Table[DCC_Level_and_Blend_Coef_Table_MAX];
	VIP_DCC_Hist_Factor_Table	DCCHist_Factor_Table[DCCHist_Factor_Table_MAX];
	VIP_DCC_AdaptCtrl_Level_Table	DCC_AdaptCtrl_Level_Table[DCC_AdaptCtrl_Level_TABLE_MAX];
	VIP_USER_DEFINE_CURVE_DCC_CRTL_ITEM		USER_DEFINE_CURVE_DCC_CRTL_Table[USER_DEFINE_CURVE_DCC_TBL_NUM];
	VIP_DCC_Database_Curve_CRTL_Table	Database_Curve_CRTL_Table[Database_DCC_Curve_TABLE_MAX][Database_DCC_Curve_Case_Item_MAX];
	VIP_DCC_Color_Independent_Blending_Table	Color_Independent_Blending_Table[Color_Independent_Blending_Table_MAX];
	VIP_DCC_Chroma_Compensation_TABLE	DCC_Chroma_Compensation_Table[DCC_Chroma_Compensation_TABLE_MAX];
	VIP_DCC_ADV_API DCC_ADV_API;
} VIP_DCC_Control_Structure;

typedef enum _VIP_DCC_Disable_Mode {
	DCC_HWOFF_Bypass = 0,
	DCC_FWOFF_NZ,	/*normalization*/
	DCC_ON,
	DCC_ON_APPLY_AP_CURVE,
} VIP_DCC_Disable_Mode;

/*======================== FOR DCC ==================================================*/
/*=================================================================================*/

/************************************************************************************************************/
/*********** Local Contrast START ******* Local Contrast START ***** Local Contrast START *******************/
/************************************************************************************************************/
#define LC_BL_Profile_Table_NUM 6
#define LC_BL_Profile_Table_ROW 4
#define LC_BL_Profile_Table_COLUMN 20
#define LC_Table_NUM 10

#define LC_H_and_V 2
#define LC_Backlight_Profile_Interpolation_table_num 125
//juwen, add LC : tone mapping curve
#define LC_Curve_ToneM_PointSlope 256
#define LC_Table_ToneM_NUM 10

#ifdef pqbin_finetune
typedef struct {
	unsigned char lc_local_sharp_en;
	unsigned char lc_tone_mapping_en;
	unsigned char lc_db_en;
	unsigned char lc_db_apply;
	unsigned char lc_db_read_level;
	unsigned char lc_subblk_mode;
	unsigned char lc_gcon_mode;
	unsigned char lc_hist_mode;
	unsigned char lc_tenable;
	unsigned char lc_valid;
} DRV_LC_Global_Ctrl;

#else
typedef struct {
	unsigned char lc_local_sharp_en;
	unsigned char lc_tone_mapping_en;
	unsigned char lc_db_en;
	unsigned char lc_db_apply;
	unsigned char lc_db_read_level;
	unsigned char lc_3d_mode;//mac7p pq compile fix
	//unsigned char lc_blk_hnum;//juwen, 1028, set blk num/size and factor
	//unsigned char lc_blk_vnum;//juwen, 1028, set blk num/size and factor
	unsigned char lc_subblk_mode;
	unsigned char lc_gcon_mode;
	unsigned char lc_hist_mode;
	unsigned char lc_tenable;
	unsigned char lc_valid;
	//unsigned short lc_blk_hsize;//juwen, 1028, set blk num/size and factor
	//unsigned short lc_blk_vsize;//juwen, 1028, set blk num/size and factor

} DRV_LC_Global_Ctrl;
#endif

typedef struct {
	unsigned char lc_maxgain;
	unsigned char lc_avegain;
	unsigned char lc_histshiftbit;
	unsigned char lc_hist0gain;
	unsigned char lc_hist1gain;
	unsigned char lc_hist2gain;
	unsigned char lc_hist3gain;
	unsigned char lc_hist4gain;
	unsigned char lc_hist5gain;
	unsigned char lc_hist6gain;
	unsigned char lc_hist7gain;

} DRV_LC_Backlight_Decision;



typedef struct {
	unsigned char lc_spatialcoef0;
	unsigned char lc_spatialcoef1;
	unsigned char lc_spatialcoef2;
	unsigned char lc_spatialcoef3;
	unsigned char lc_spatialcoef4;
	unsigned char lc_spatialcoef5;
	unsigned char lc_spatialcoef6;
	unsigned char lc_spatialcoef7;
	unsigned char lc_spatialcoef8;
	unsigned char lc_spatialcoef9;
	unsigned char lc_spatialcoef10;
	unsigned char lc_spatialremapen;

} DRV_LC_Spatial_Filter;


typedef struct {

	unsigned char lc_tmp_pos0thd;
	unsigned char lc_tmp_pos1thd;
	unsigned char lc_tmp_posmingain;
	unsigned char lc_tmp_posmaxgain;
	unsigned char lc_tmp_neg0thd;
	unsigned char lc_tmp_neg1thd;
	unsigned char lc_tmp_negmingain;
	unsigned char lc_tmp_negmaxgain;
	unsigned char lc_tmp_maxdiff;
	unsigned char lc_tmp_scenechangegain1;

} DRV_LC_Temporal_Filter;


typedef struct {
	unsigned char lc_tab_hsize;
	//unsigned int lc_hfactor;//juwen, 1028, set blk num/size and factor
	unsigned char lc_hinitphase3_right;
	unsigned char lc_hboundary;
	unsigned char lc_hinitphase1;
	unsigned char lc_hinitphase2;
	unsigned char lc_hinitphase3_left;
	unsigned char lc_tab_vsize;
	//unsigned int lc_vfactor;//juwen, 1028, set blk num/size and factor
	unsigned char lc_vinitphase3_right;
	unsigned char lc_vboundary;
	unsigned char lc_vinitphase1;
	unsigned char lc_vinitphase2;
	unsigned char lc_vinitphase3_left;
	unsigned char lc_blight_update_en;
	unsigned char lc_blight_sw_mode;
	unsigned char lc_table_sw_mode;

} DRV_LC_Backlight_Profile_Interpolation;

typedef struct {
	unsigned char lc_demo_en;
	unsigned char lc_demo_mode;
	unsigned short lc_demo_top;
	unsigned short lc_demo_bottom;
	unsigned short lc_demo_left;
	unsigned short lc_demo_right;
} DRV_LC_Demo_Window;

typedef struct {
	unsigned short lc_tmap_g0_th1;
	unsigned short lc_tmap_g0_th2;
	unsigned short lc_tmap_g0_th3;
	unsigned short lc_tmap_g0_th4;
	unsigned short lc_tmap_g0_th5;
	unsigned short lc_tmap_g0_th6;
	unsigned short lc_tmap_g0_th7;
	unsigned short lc_tmap_g0_th8;
	unsigned short lc_tmap_g0_th9;
	unsigned short lc_tmap_g0_th10;
	unsigned short lc_tmap_g0_th11;
	unsigned short lc_tmap_g0_th12;
	unsigned short lc_tmap_g0_th13;
	unsigned short lc_tmap_g0_th14;
	unsigned short lc_tmap_g0_th15;
	unsigned short lc_tmap_g0_th16;
	unsigned short lc_tmap_g0_th17;
	unsigned short lc_tmap_g0_th18;
	unsigned short lc_tmap_g0_th19;
	unsigned short lc_tmap_g0_th20;
	unsigned short lc_tmap_g0_th21;
	unsigned short lc_tmap_g0_th22;
	unsigned short lc_tmap_g0_th23;
	unsigned short lc_tmap_g0_th24;
	unsigned short lc_tmap_g0_th25;
	unsigned short lc_tmap_g0_th26;
	unsigned short lc_tmap_g0_th27;
	unsigned short lc_tmap_g0_th28;
	unsigned short lc_tmap_g0_th29;
	unsigned short lc_tmap_g0_th30;
	unsigned short lc_tmap_g0_th31;
} DRV_LC_ToneMapping_Grid0;

typedef struct {
	unsigned short lc_tmap_g2_th1;
	unsigned short lc_tmap_g2_th2;
	unsigned short lc_tmap_g2_th3;
	unsigned short lc_tmap_g2_th4;
	unsigned short lc_tmap_g2_th5;
	unsigned short lc_tmap_g2_th6;
	unsigned short lc_tmap_g2_th7;
	unsigned short lc_tmap_g2_th8;
	unsigned short lc_tmap_g2_th9;
	unsigned short lc_tmap_g2_th10;
	unsigned short lc_tmap_g2_th11;
	unsigned short lc_tmap_g2_th12;
	unsigned short lc_tmap_g2_th13;
	unsigned short lc_tmap_g2_th14;
	unsigned short lc_tmap_g2_th15;
	unsigned short lc_tmap_g2_th16;
	unsigned short lc_tmap_g2_th17;
	unsigned short lc_tmap_g2_th18;
	unsigned short lc_tmap_g2_th19;
	unsigned short lc_tmap_g2_th20;
	unsigned short lc_tmap_g2_th21;
	unsigned short lc_tmap_g2_th22;
	unsigned short lc_tmap_g2_th23;
	unsigned short lc_tmap_g2_th24;
	unsigned short lc_tmap_g2_th25;
	unsigned short lc_tmap_g2_th26;
	unsigned short lc_tmap_g2_th27;
	unsigned short lc_tmap_g2_th28;
	unsigned short lc_tmap_g2_th29;
	unsigned short lc_tmap_g2_th30;
	unsigned short lc_tmap_g2_th31;

} DRV_LC_ToneMapping_Grid2;

typedef struct {
	unsigned char lc_tmap_curve0_grid;
	unsigned short lc_tmap_curve0_tone;
	unsigned char lc_tmap_curve1_grid;
	unsigned short lc_tmap_curve1_tone;
	unsigned char lc_tmap_curve2_grid;
	unsigned short lc_tmap_curve2_tone;
	unsigned char lc_tmap_curve3_grid;
	unsigned short lc_tmap_curve3_tone;
	unsigned char lc_tmap_curve4_grid;
	unsigned short lc_tmap_curve4_tone;
	unsigned char lc_tmap_curve5_grid;
	unsigned short lc_tmap_curve5_tone;
	unsigned char lc_tmap_curve6_grid;
	unsigned short lc_tmap_curve6_tone;
	unsigned char lc_tmap_curve7_grid;
	unsigned short lc_tmap_curve7_tone;

} DRV_LC_ToneMapping_CurveSelect;


typedef struct {
	unsigned char lc_tmap_slope_unit;
	unsigned char lc_tmap_blend_factor;
} DRV_LC_ToneMapping_Blend;

typedef struct {
	unsigned char lc_gain_by_yin_0;
	unsigned char lc_gain_by_yin_1;
	unsigned char lc_gain_by_yin_2;
	unsigned char lc_gain_by_yin_3;
	unsigned char lc_gain_by_yin_4;
	unsigned char lc_gain_by_yin_5;
	unsigned char lc_gain_by_yin_6;
	unsigned char lc_gain_by_yin_7;
	unsigned char lc_gain_by_yin_8;
	unsigned char lc_gain_by_yin_9;
	unsigned char lc_gain_by_yin_10;
	unsigned char lc_gain_by_yin_11;
	unsigned char lc_gain_by_yin_12;
	unsigned char lc_gain_by_yin_13;
	unsigned char lc_gain_by_yin_14;
	unsigned char lc_gain_by_yin_15;
	unsigned char lc_gain_by_yin_16;
	unsigned char lc_gain_by_yin_17;
	unsigned char lc_gain_by_yin_18;
	unsigned char lc_gain_by_yin_19;
	unsigned char lc_gain_by_yin_20;
	unsigned char lc_gain_by_yin_21;
	unsigned char lc_gain_by_yin_22;
	unsigned char lc_gain_by_yin_23;
	unsigned char lc_gain_by_yin_24;
	unsigned char lc_gain_by_yin_25;
	unsigned char lc_gain_by_yin_26;
	unsigned char lc_gain_by_yin_27;
	unsigned char lc_gain_by_yin_28;
	unsigned char lc_gain_by_yin_29;
	unsigned char lc_gain_by_yin_30;
	unsigned char lc_gain_by_yin_31;
	unsigned char lc_gain_by_yin_32;
	unsigned char lc_gain_by_yin_divisor;
} DRV_LC_Shpnr_Gain1st;

typedef struct {
	unsigned char lc_gain_by_ydiff_0;
	unsigned char lc_gain_by_ydiff_1;
	unsigned char lc_gain_by_ydiff_2;
	unsigned char lc_gain_by_ydiff_3;
	unsigned char lc_gain_by_ydiff_4;
	unsigned char lc_gain_by_ydiff_5;
	unsigned char lc_gain_by_ydiff_6;
	unsigned char lc_gain_by_ydiff_7;
	unsigned char lc_gain_by_ydiff_8;
	unsigned char lc_gain_by_ydiff_9;
	unsigned char lc_gain_by_ydiff_10;
	unsigned char lc_gain_by_ydiff_11;
	unsigned char lc_gain_by_ydiff_12;
	unsigned char lc_gain_by_ydiff_13;
	unsigned char lc_gain_by_ydiff_14;
	unsigned char lc_gain_by_ydiff_15;
	unsigned char lc_gain_by_ydiff_16;
	unsigned char lc_gain_by_ydiff_17;
	unsigned char lc_gain_by_ydiff_18;
	unsigned char lc_gain_by_ydiff_19;
	unsigned char lc_gain_by_ydiff_20;
	unsigned char lc_gain_by_ydiff_21;
	unsigned char lc_gain_by_ydiff_22;
	unsigned char lc_gain_by_ydiff_23;
	unsigned char lc_gain_by_ydiff_24;
	unsigned char lc_gain_by_ydiff_25;
	unsigned char lc_gain_by_ydiff_26;
	unsigned char lc_gain_by_ydiff_27;
	unsigned char lc_gain_by_ydiff_28;
	unsigned char lc_gain_by_ydiff_29;
	unsigned char lc_gain_by_ydiff_30;
	unsigned char lc_gain_by_ydiff_31;
	unsigned char lc_gain_by_ydiff_32;
	unsigned char lc_gain_by_ydiff_divisor;
} DRV_LC_Shpnr_Gain2nd;

typedef struct {
	unsigned char lc_ydiff_measure_en;
	unsigned short lc_ydiff_abs_th;

} DRV_LC_Diff_Ctrl0;

typedef struct {
	DRV_LC_Global_Ctrl LC_Global_Ctrl;
	DRV_LC_Backlight_Decision LC_Backlight_Decision;
	DRV_LC_Spatial_Filter LC_Spatial_Filter;
	DRV_LC_Temporal_Filter LC_Temporal_Filter;
	DRV_LC_Backlight_Profile_Interpolation LC_Backlight_Profile_Interpolation;
	DRV_LC_Demo_Window LC_Demo_Window;
	DRV_LC_ToneMapping_Grid0 LC_ToneMapping_Grid0;
	DRV_LC_ToneMapping_Grid2 LC_ToneMapping_Grid2;
	DRV_LC_ToneMapping_CurveSelect LC_ToneMapping_CurveSelect;
	DRV_LC_ToneMapping_Blend LC_ToneMapping_Blend;
	DRV_LC_Shpnr_Gain1st LC_Shpnr_Gain1st;
	DRV_LC_Shpnr_Gain2nd LC_Shpnr_Gain2nd;
	DRV_LC_Diff_Ctrl0 LC_Diff_Ctrl0;

} DRV_Local_Contrast_Table;

//juwen, 1028, set blk num/size and factor
typedef struct {
	unsigned char nBlk_Hnum;
	unsigned char nBlk_Vnum;
	unsigned short nBlk_Hsize;
	unsigned short nBlk_Vsize;
	unsigned int lc_hfactor;
	unsigned int lc_vfactor;
} LC_Region_Num_Size;

/************************************************************************************************************/
/*********** YUV2RGB START ******* YUV2RGB START ***** YUV2RGB START ****************************************/
/************************************************************************************************************/
#define YUV2RGB_TBL_Num	10
#define LUT3D_TBL_Num	10
#define LUT3D_TBL_ITEM	17*17*17*2

typedef enum _VIP_YUV2RGB_LEVEL_SELECT {
	VIP_YUV2RGB_LEVEL_OFF = 0,
	VIP_YUV2RGB_LEVEL_LOW,
	VIP_YUV2RGB_LEVEL_MID,
	VIP_YUV2RGB_LEVEL_HIGH,

	VIP_YUV2RGB_LEVEL_SELECT_MAX,
} VIP_YUV2RGB_LEVEL_SELECT;

typedef enum _VIP_CSMatrix_WriteType {
	VIP_CSMatrix_YUV2RGB_Base_Ctrl = 0,
	VIP_CSMatrix_Coef_Y_Only,
	VIP_CSMatrix_Coef_C_Only,
	VIP_CSMatrix_Offset_Only,
	VIP_CSMatrix_Y_Clamp,
	VIP_CSMatrix_CoefByY_ctrl,
	VIP_CSMatrix_UVOffset_ctrl,
	VIP_CSMatrix_UVOffset_Coef,
	VIP_CSMatrix_CoefByY_Index,		/*  add by chris @20140721*/
	VIP_CSMatrix_UVOffset_Index,	/*  add by chris @20140721*/
	VIP_CSMatrix_Coef_BT2020_Only,
	VIP_CSMatrix_MAX,
} VIP_CSMatrix_WriteType;

typedef enum _VIP_YUV2RGB_Y_SEG {
	VIP_YUV2RGB_Y_Seg_0 = 0,
	VIP_YUV2RGB_Y_Seg_1,
	VIP_YUV2RGB_Y_Seg_2,
	VIP_YUV2RGB_Y_Seg_3,
	VIP_YUV2RGB_Y_Seg_4,
	VIP_YUV2RGB_Y_Seg_5,
	VIP_YUV2RGB_Y_Seg_6,
	VIP_YUV2RGB_Y_Seg_7,
	VIP_YUV2RGB_Y_Seg_8,
	VIP_YUV2RGB_Y_Seg_9,
	VIP_YUV2RGB_Y_Seg_10,
	VIP_YUV2RGB_Y_Seg_11,
	VIP_YUV2RGB_Y_Seg_12,
	VIP_YUV2RGB_Y_Seg_13,
	VIP_YUV2RGB_Y_Seg_14,
	VIP_YUV2RGB_Y_Seg_15,
	VIP_YUV2RGB_Y_Seg_16,

	VIP_YUV2RGB_Y_Seg_Max,
} VIP_YUV2RGB_Y_SEG;

typedef enum _VIP_YUV2RGB_CSMatrix_ITEMS {
	VIP_YUV2RGB_CSMatrix_ITEMS_K11 = 0,
	VIP_YUV2RGB_CSMatrix_ITEMS_K12,
	VIP_YUV2RGB_CSMatrix_ITEMS_K13,
	VIP_YUV2RGB_CSMatrix_ITEMS_K22,
	VIP_YUV2RGB_CSMatrix_ITEMS_K23,
	VIP_YUV2RGB_CSMatrix_ITEMS_K32,
	VIP_YUV2RGB_CSMatrix_ITEMS_K33,
	VIP_YUV2RGB_CSMatrix_ITEMS_Y_Index,

	VIP_YUV2RGB_CSMatrix_ITEMS_Max,
} VIP_YUV2RGB_CSMatrix_ITEMS;

typedef enum _VIP_YUV2RGB_UV_OFFSET_ITEMS {
	VIP_YUV2RGB_UV_OFFSET_ITEMS_U_Offset = 0,
	VIP_YUV2RGB_UV_OFFSET_ITEMS_V_Offset,
	VIP_YUV2RGB_UV_OFFSET_ITEMS_U_Step,
	VIP_YUV2RGB_UV_OFFSET_ITEMS_V_Step,
	VIP_YUV2RGB_UV_OFFSET_ITEMS_Y_Index,

	VIP_YUV2RGB_UV_OFFSET_ITEMS_Max,
} VIP_YUV2RGB_UV_OFFSET_ITEMS;

typedef enum _VIP_YUV2RGB_INPUT_DATA_MODE {
	YUV2RGB_INPUT_601_Limted = 0,
	YUV2RGB_INPUT_601_Full,
	YUV2RGB_INPUT_709_Limted,
	YUV2RGB_INPUT_709_Full,
	YUV2RGB_INPUT_2020_NonConstantY,
	YUV2RGB_INPUT_2020_ConstantY,
	YUV2RGB_INPUT_2020_NonConstantY_Full,
	YUV2RGB_INPUT_2020_ConstantY_Full,

	YUV2RGB_INPUT_MODE_UNKNOW,
	YUV2RGB_INPUT_MODE_bypass,
	/* we have 601_limit, 601_full, 709_limit, 709_full, 2020_limit, 2020_full after RGB2YUV . YUV data have no other case. new case is no need*/
	YUV2RGB_INPUT_MODE_MAX,

} VIP_YUV2RGB_INPUT_DATA_MODE;

typedef enum _VIP_tUV2RGB_COEF_ITEMS {
	tUV2RGB_COEF_K11 = 0,
	tUV2RGB_COEF_K12,
	tUV2RGB_COEF_K13,
	tUV2RGB_COEF_K13_2,
	tUV2RGB_COEF_K22,
	tUV2RGB_COEF_K23,
	tUV2RGB_COEF_K32,
	tUV2RGB_COEF_K32_2,
	tUV2RGB_COEF_K33,
	tUV2RGB_COEF_R_Offset,
	tUV2RGB_COEF_G_Offset,
	tUV2RGB_COEF_B_Offset,

	tUV2RGB_COEF_Items_Max,

} VIP_tUV2RGB_COEF_ITEMS;

typedef struct _DRV_VIP_YUV2RGB_CTRL_ITEM {
	unsigned char YUV2RGB_Enable_Main;
	unsigned char YUV2RGB_Enable_Sub;
	unsigned char Overlay;
	unsigned char OutShift_En_Main;
	unsigned char OutShift_En_Sub;
	unsigned char Y_Clamp;
	unsigned char CbCr_Clamp;
	unsigned char Table_Select_Main;
	unsigned char Table_Select_Sub;
	unsigned char CoefByY_En;
	unsigned char UVOffset_En;
	unsigned char UVOffset_Mode_Ctrl;
	unsigned char UVOffset_Mode_byUV_byY;/*juwen, Merlin3, newAlgo : UV offset by UV*/
	unsigned char CoefBySat_mode;
	unsigned char CoefBySat_gain;

	unsigned char Bt2020_En_TBL1;	/* control by flow, tool don't gen this item, define this item at end */
	unsigned char Bt2020_En_TBL2;	/* control by flow, tool don't gen this item, define this item at end */

} DRV_VIP_YUV2RGB_CTRL_ITEM;

typedef struct _DRV_VIP_YUV2RGB_RGB_Offset {
	unsigned int R_offset;
	unsigned int G_offset;
	unsigned int B_offset;
} DRV_VIP_YUV2RGB_RGB_Offset;

typedef struct _DRV_VIP_YUV2RGB_COEF_By_Y {
	unsigned short K11[VIP_YUV2RGB_Y_Seg_Max];
	unsigned short K12[VIP_YUV2RGB_Y_Seg_Max];
	unsigned short K13[VIP_YUV2RGB_Y_Seg_Max];
	unsigned short K22[VIP_YUV2RGB_Y_Seg_Max];
	unsigned short K23[VIP_YUV2RGB_Y_Seg_Max];
	unsigned short K32[VIP_YUV2RGB_Y_Seg_Max];
	unsigned short K33[VIP_YUV2RGB_Y_Seg_Max];
	unsigned char Y_index[VIP_YUV2RGB_Y_Seg_Max-1];		/* Mac3 add by chris @20140721*/
	unsigned short bt2020_K13;
	unsigned short bt2020_K32;

} DRV_VIP_YUV2RGB_COEF_By_Y;

typedef struct _DRV_VIP_YUV2RGB_UV_Offset {
	unsigned char Uoffset[VIP_YUV2RGB_Y_Seg_Max];
	unsigned char Voffset[VIP_YUV2RGB_Y_Seg_Max];
	unsigned char Ustep[VIP_YUV2RGB_Y_Seg_Max];
	unsigned char Vstep[VIP_YUV2RGB_Y_Seg_Max];
	unsigned char UV_index[VIP_YUV2RGB_Y_Seg_Max-1];	/* Mac3 add by chris @20140721*/
} DRV_VIP_YUV2RGB_UV_Offset;

/*juwen, Merlin3, newAlgo : UV offset by UV*/
typedef struct _DRV_VIP_YUV2RGB_UV_Offset_byUV_CURVE {
	unsigned short gainCurve_index[16];
	unsigned short gain[17];
} DRV_VIP_YUV2RGB_UV_Offset_byUV_CURVE;

typedef struct _DRV_VIP_YUV2RGB_CSMatrix {
	DRV_VIP_YUV2RGB_CTRL_ITEM CTRL_ITEM;
	DRV_VIP_YUV2RGB_RGB_Offset RGB_Offset;
	DRV_VIP_YUV2RGB_COEF_By_Y COEF_By_Y;
	DRV_VIP_YUV2RGB_UV_Offset UV_Offset;
	DRV_VIP_YUV2RGB_UV_Offset_byUV_CURVE UV_Offset_byUV_CURVE;/*juwen, Merlin3, newAlgo : UV offset by UV*/
} DRV_VIP_YUV2RGB_CSMatrix;

typedef struct _VIP_YUV2RGB_CSMatrix_Table {
	DRV_VIP_YUV2RGB_CSMatrix YUV2RGB_CSMatrix[VIP_YUV2RGB_LEVEL_SELECT_MAX];
} VIP_YUV2RGB_CSMatrix_Table;

typedef struct _VIP_USING_YUV2RGB_CSMatrix {
	unsigned char YUV2RGB_TBL_Select;
	unsigned char CoefByY_CtrlItem_Level;
	unsigned char UV_Offset_Level;
	unsigned char RGB_Offset_Level;
	unsigned char Input_Data_Mode;
	DRV_VIP_YUV2RGB_CSMatrix YUV2RGB_CSMatrix;
} VIP_USING_YUV2RGB_CSMatrix;

//subHDR h5x, add==========================================================================
typedef enum _subHDR_CSMatrix_WriteType {
        subHDR_CSMatrix_YUV2RGB_Base_Ctrl = 0,
        subHDR_CSMatrix_Coef,
        subHDR_CSMatrix_Offset_Only,
        subHDR_CSMatrix_Y_Clamp,
        subHDR_CSMatrix_Coef_BT2020_Only,
        subHDR_CSMatrix_enable_BT2020,

        subHDR_CSMatrix_MAX,
} subHDR_CSMatrix_WriteType;

typedef struct _DRV_subHDR_YUV2RGB_CTRL_ITEM {
        unsigned char YUV2RGB_Enable;
        unsigned char OutShift_En;
        unsigned char Y_Clamp;
        unsigned char CbCr_Clamp;
        unsigned char Bt2020_En;        /* control by flow, tool don't gen this item, define this item at end */
} DRV_subHDR_YUV2RGB_CTRL_ITEM;

typedef struct _DRV_subHDR_YUV2RGB_COEF {
        unsigned short K11;
        unsigned short K12;
        unsigned short K13;
        unsigned short K22;
        unsigned short K23;
        unsigned short K32;
        unsigned short K33;
        unsigned short bt2020_K13;
        unsigned short bt2020_K32;
} DRV_subHDR_YUV2RGB_COEF;

typedef struct _DRV_subHDR_YUV2RGB_RGB_Offset {
        int R_offset;
        int G_offset;
        int B_offset;
} DRV_subHDR_YUV2RGB_RGB_Offset;

typedef struct _DRV_subHDR_YUV2RGB_CSMatrix {
        DRV_subHDR_YUV2RGB_CTRL_ITEM CTRL_ITEM;
        DRV_subHDR_YUV2RGB_RGB_Offset RGB_Offset;
        DRV_subHDR_YUV2RGB_COEF COEF;
} DRV_subHDR_YUV2RGB_CSMatrix;

//subHDR h5x, add==========================================================================

/************************************************************************************************************/
/*********** YUV2RGB End ******* YUV2RGB End ***** YUV2RGB End ***********************************************/
/************************************************************************************************************/

/************************************************************************************************************/
/********************* RGB2YUV START ******* RGB2YUV START ***** RGB2YUV START ******************************/
/************************************************************************************************************/
typedef struct _VIP_RGB2YUV_COEF_CSMatrix {
	unsigned short m11;
	unsigned short m12;
	unsigned short m13;
	unsigned short m21;
	unsigned short m22;
	unsigned short m23;
	unsigned short m31;
	unsigned short m32;
	unsigned short m33;
	unsigned short Yo_even;
	unsigned short Yo_odd;
	unsigned short Y_gain;
	unsigned short sel_RGB;
	unsigned short sel_Yin_offset;
	unsigned short sel_UV_out_offset;
	unsigned short sel_UV_off;
	unsigned short Matrix_bypass;
	unsigned short Enable_Y_gain;
} VIP_RGB2YUV_COEF_CSMatrix;

/************************************************************************************************************/
/*********** RGB2YUV End ******* RGB2YUV End ***** RGB2YUV End ***********************************************/
/************************************************************************************************************/

/************************************************************************************************************/
/*********** BT2020 CTRL START ******* BT2020 CTRL START ***** BT2020 CTRL START ******************************/
/************************************************************************************************************/
typedef enum _VIP_BT2020_MODE {
	BT2020_MODE_Non_Constant = 0,
	BT2020_MODE_Constant,

	BT2020_MODE_Max,

} VIP_BT2020_MODE;

typedef struct _VIP_BT2020_CTRL {
	unsigned char Enable_Flag;
	unsigned char Mode;
	unsigned char Enable_Flag_Sub;
	unsigned char Mode_Sub;
} VIP_BT2020_CTRL;

/************************************************************************************************************/
/*********** BT2020 CTRL End ******* BT2020 CTRL End ***** BT2020 CTRL End **************************************/
/************************************************************************************************************/

/************************************************************/
/*********** DCTI START ******* DCTI START ***** DCTI START ***/
/************************************************************/
typedef enum _VIP_DCTI_TABLE_LEVEL {
	DCTI_TABLE_LEVEL_0 = 0,
	DCTI_TABLE_LEVEL_1,
	DCTI_TABLE_LEVEL_2,
	DCTI_TABLE_LEVEL_3,
	DCTI_TABLE_LEVEL_4,
	DCTI_TABLE_LEVEL_5,
	DCTI_TABLE_LEVEL_6,
	DCTI_TABLE_LEVEL_7,
	DCTI_TABLE_LEVEL_8,
	DCTI_TABLE_LEVEL_9,
	DCTI_TABLE_LEVEL_10,

	DCTI_TABLE_LEVEL_MAX,
} VIP_DCTI_TABLE_LEVEL;

typedef struct {

	unsigned char data_sel;
	unsigned char maxlen;
	unsigned char psmth;
	unsigned char lp_mode;
	unsigned char engdiv;
	unsigned char offdiv;
	unsigned char uvgain;
	unsigned char dcti_mode;
	unsigned char uvalign_en;
	unsigned char cur_sel;
	unsigned char dcti_en;


} DRV_VipNewDDcti_ctl1;

typedef struct {

	unsigned char uvsync_en;
	unsigned char tran_mode;
	unsigned char rate_steep;
	unsigned char th_steep;
	unsigned char th_coring;
	unsigned char steep_mode;
	unsigned char hp_mode;
	unsigned char vlp_mode;
	unsigned char median_mode;
	unsigned char blending_mode;

} DRV_VipNewDDcti_ctl2;

typedef struct {

	unsigned char maxminlen;
	unsigned char rate_tran;
	unsigned char th_tran;
	unsigned char region_mode;
	unsigned char rate_smooth;
	unsigned char th_smooth;

} DRV_VipNewDDcti_ctl3;


typedef struct {

	unsigned char rate_align_weight;
	unsigned char rate_align;
	unsigned char th_align;
	unsigned char align_mingain;
	unsigned char th_gtran;

} DRV_VipNewDDcti_ctl4;

typedef struct {

	unsigned char th_uvmindiff;
	unsigned char rate_stair2;
	unsigned char rate_stair1;
	unsigned char th_stair;
	unsigned char th_align_weight;

} DRV_VipNewDDcti_ctl5;

typedef struct {

	unsigned char rate_engsync;
	unsigned char rate_uvmindiff;
	unsigned char th_engsync;

} DRV_VipNewDDcti_ctl6;

typedef struct {

	unsigned char debug_shiftbit;
	unsigned char debug_mode;

} DRV_VipNewDDcti_ctl7;

typedef struct {

	unsigned char fhpbound_en;
	unsigned char inc_pix_check_en;
	unsigned char findmid_uxrhor_en;
	unsigned char match_protect_en;
	unsigned char inc_psmth;
	unsigned char match_protect_scale;

} DRV_VipNewDDcti_ctl8;

typedef struct {

	unsigned char halfmid_en;
	unsigned char halfmid_consi_align_en;
	unsigned char halfmid_filter_mode;
	unsigned char halfmid_hp_mode;
	unsigned char halfmid_th_coring;
	unsigned char halfmid_steep_mode;
	unsigned char halfmid_th_steep;
	unsigned char halfmid_rate_steep;

} DRV_VipNewDDcti_uvhalfmid_ctl0;

typedef struct {

	unsigned char halfmid_psmth;
	unsigned char th_slopediff_sum;
	unsigned char rate_slopediff_sum;
	unsigned char slopediff_mode;
	unsigned char slopediff_fixlen;
	unsigned char slopediff_maxmin_en;

} DRV_VipNewDDcti_uvhalfmid_ctl1;

typedef struct {

	unsigned char halfmid_th_eng;
	unsigned char halfmid_rate_eng;
	unsigned char offs_enhance_mode;
	unsigned char offs_enhance_bit;
	unsigned char offs_enhance_val;
	unsigned char halfmid_blending;

} DRV_VipNewDDcti_uvhalfmid_ctl2;

typedef struct  {
    unsigned char  dcti_v_en;
    unsigned char  dcti_mode_ver;
    unsigned char  lp_mode_ver;
    unsigned char  median_mode_ver;
    unsigned char  hp_mode_ver;

}DRV_VipNewVDDcti_ctl0;

typedef struct  {

    unsigned char  uvgain_ver;
    unsigned char  engdiv_ver;
    unsigned char  offdiv_ver;

}DRV_VipNewVDDcti_ctl1;

typedef struct {

	unsigned char table_mode_flag;
	unsigned char low_bound;
	unsigned char up_bound;
} DRV_VipNewDcti_auto_setting;

typedef struct {

	DRV_VipNewDcti_auto_setting VipNewDcti_auto_setting;
	unsigned char th_table[DCTI_TABLE_LEVEL_MAX];
	unsigned char level_table[DCTI_TABLE_LEVEL_MAX];
} DRV_VipNewDcti_auto_adjust;

typedef struct {

	DRV_VipNewDDcti_ctl1 VipNewDDcti_ctl1;
	DRV_VipNewDDcti_ctl2 VipNewDDcti_ctl2;
	DRV_VipNewDDcti_ctl3 VipNewDDcti_ctl3;
	DRV_VipNewDDcti_ctl4 VipNewDDcti_ctl4;
	DRV_VipNewDDcti_ctl5 VipNewDDcti_ctl5;
	DRV_VipNewDDcti_ctl6 VipNewDDcti_ctl6;
	DRV_VipNewDDcti_ctl7 VipNewDDcti_ctl7;
} DRV_VipNewDDcti_Table;

typedef struct {

	unsigned char data_sel;
	unsigned char maxlen;
	unsigned char psmth;
	unsigned char lp_mode;
	unsigned char engdiv;
	unsigned char offdiv;
	unsigned char uvgain;
	unsigned char dcti_mode;
	unsigned char uvalign_en;
	unsigned char cur_sel;
	unsigned char dcti_en;

} DRV_VipNewIDcti_ctl1;

typedef struct {

	unsigned char uvsync_en;
	unsigned char tran_mode;
	unsigned char rate_steep;
	unsigned char th_steep;
	unsigned char th_coring;
	unsigned char steep_mode;
	unsigned char hp_mode;
	unsigned char vlp_mode;
	unsigned char median_mode;
	unsigned char blending_mode;

} DRV_VipNewIDcti_ctl2;

typedef struct {

	unsigned char maxminlen;
	unsigned char rate_tran;
	unsigned char th_tran;
	unsigned char region_mode;
	unsigned char rate_smooth;
	unsigned char th_smooth;

} DRV_VipNewIDcti_ctl3;


typedef struct {

	unsigned char rate_align_weight;
	unsigned char rate_align;
	unsigned char th_align;
	unsigned char align_mingain;
	unsigned char th_gtran;

} DRV_VipNewIDcti_ctl4;

typedef struct {

	unsigned char statistic_en;
	unsigned char th_uvmindiff;
	unsigned char res1;
	unsigned char rate_stair2;
	unsigned char res2;
	unsigned char rate_stair1;
	unsigned char res3;
	unsigned char th_stair;
	unsigned char res4;
	unsigned char th_align_weight;

} DRV_VipNewIDcti_ctl5;

typedef struct {
	unsigned int  statistic_hist;
	unsigned char rate_engsync;
	unsigned char rate_uvmindiff;
	unsigned char th_engsync;

} DRV_VipNewIDcti_ctl6;

typedef struct {
	unsigned char debug_shiftbit;
	unsigned char debug_mode;
	unsigned char high_bound;
	unsigned char low_bound;

} DRV_VipNewIDcti_ctl7;

typedef struct {
	unsigned char uvgain_v;
	unsigned char offdiv_v;
	unsigned char engdiv_v;

} DRV_VipNewIDcti_ctl1_v;

typedef struct {
	unsigned char tran_mode_v;
	unsigned char rate_steep_v;
	unsigned char th_steep_v;
	unsigned char steep_mode_v;

} DRV_VipNewIDcti_ctl2_v;

typedef struct {
	unsigned char rate_tran_v;
	unsigned char th_tran_v;
	unsigned char region_mode_v;
	unsigned char rate_smooth_v;
	unsigned char th_smooth_v;

} DRV_VipNewIDcti_ctl3_v;

typedef struct {
	unsigned char rate_align_weight_v;
	unsigned char th_stair_v;
	unsigned char rate_stair2_v;
	unsigned char rate_stair1_v;
	unsigned char th_align_weight_v;
	unsigned char th_gtran_v;

} DRV_VipNewIDcti_ctl4_v;

typedef struct {
	DRV_VipNewIDcti_ctl1 VipNewIDcti_ctl1;
	DRV_VipNewIDcti_ctl2 VipNewIDcti_ctl2;
	DRV_VipNewIDcti_ctl3 VipNewIDcti_ctl3;
	DRV_VipNewIDcti_ctl4 VipNewIDcti_ctl4;
	DRV_VipNewIDcti_ctl5 VipNewIDcti_ctl5;
	DRV_VipNewIDcti_ctl6 VipNewIDcti_ctl6;
	DRV_VipNewIDcti_ctl7 VipNewIDcti_ctl7;
#if 0 //mac6 no implement
	DRV_VipNewIDcti_ctl1_v VipNewIDcti_ctl1_v;
	DRV_VipNewIDcti_ctl2_v VipNewIDcti_ctl2_v;
	DRV_VipNewIDcti_ctl3_v VipNewIDcti_ctl3_v;
	DRV_VipNewIDcti_ctl4_v VipNewIDcti_ctl4_v;
#endif
} DRV_VipNewIDcti_Table;

typedef struct {
	DRV_VipNewDcti_auto_adjust  auto_adjust_Table[DCTI_TABLE_LEVEL_MAX];
	DRV_VipNewDDcti_Table dDcti_Table[DCTI_TABLE_LEVEL_MAX];
	DRV_VipNewIDcti_Table iDcti_Table[DCTI_TABLE_LEVEL_MAX];
	unsigned char iDcti_table_select;
	unsigned char dDcti_table_select;
} VIP_I_D_Dcti_INFO;

/************************************************************/
/*********** DCTI END **** DCTI END ******* DCTI END **********/
/************************************************************/

/************************************************************/
/*********** UZD  INFO START **** UZD  INFO START *************/
/************************************************************/
#define UZD_Idx_TBL_Max 8
#define SD_FIR_Coef_MAXNUM 32

typedef enum _VIP_ScalingDown_FIR_Level {
	ScalingDown_FIR_Blur = 0,
	ScalingDown_FIR_Mid,
	ScalingDown_FIR_Sharp,
	ScalingDown_FIR_reverse,

	VIP_ScalingDown_FIR_Level_Max

} VIP_ScalingDown_FIR_Level;

typedef enum _VIP_ScalingDown_FIR_Items {
	SDFIR64_90_20 = 0,
	SDFIR64_70_20,
	SDFIR64_40_20,
	SDFIR64_35_20,
	SDFIR64_32_20,
	SDFIR64_50_25Hamm,
	SDFIR64_Blur,
	SDFIR64_Mid,
	SDFIR64_Sharp,
	SDFIR64_45_20Hamm,
	SDFIR64_30_20Hamm,
	SDFIR64_25_20Hamm,
	SDFIR64_25_15Hamm,
	SDFIR64_60_10Hamm,
	SDFIR64_2tap,
	SDFIR64_20_10Hamm,
	SDFIR64_20_10,
	SDFIR64_15_10Hamm,
	SDFIR64_MAXNUM,
} VIP_ScalingDown_FIR_Items;

/************************************************************/
/*********** UZD END **** UZD    END **************************/
/************************************************************/

/************************************************************/
/*********** PQA INFO START **** PQA INFO START ***************/
/************************************************************/
#define PQA_I_TABLE_MAX	20
#define PQA_TABLE_MAX	20
#define PQA_ITEM_MAX	255

typedef enum _VIP_PQA_TBL_CONTENT_CHECK {	/* 20140103	rock add by MA_flow*/
	PQA_CC_RTNR_C_TH = 0,
	PQA_CC_RTNR_C_K,

	PQA_CC_MAX,
} VIP_PQA_TBL_CONTENT_CHECK;

typedef enum _VIP_PQA_LEVEL_INDEX {	/* 20140103	rock add by MA_flow*/
	PQA_Level_Idx_0 = 0,
	PQA_Level_Idx_1,
	PQA_Level_Idx_2,
	PQA_Level_Idx_3,
	PQA_Level_Idx_4,
	PQA_Level_Idx_5,
	PQA_Level_Idx_6,
	PQA_Level_Idx_7,
	PQA_Level_Idx_8,
	PQA_Level_Idx_9,

	PQA_SPM_LvIdx_VD,
	PQA_SPM_LvIdx_MAD_Noise,
	PQA_SPM_LvIdx_MAD_Still,
	PQA_SPM_LvIdx_FMV_Still,
	PQA_SPM_LvIdx_Y,

	PQA_Level_Idx_MAX,
} VIP_PQA_LEVEL_INDEX;

typedef enum _VIP_PQ_ADAPTIVE_INPUT_TABLE {	/* 20140103	rock add by MA_flow*/
	PQA_I_L00 = 0,
	PQA_I_L01,
	PQA_I_L02,
	PQA_I_L03,
	PQA_I_L04,
	PQA_I_L05,
	PQA_I_L06,
	PQA_I_L07,
	PQA_I_L08,
	PQA_I_L09,
	PQA_I_LEVEL_MAX,
} VIP_PQ_ADAPTIVE_INPUT_TABLE;

typedef enum _VIP_PQ_ADAPTIVE_MODE {
	PQA_MODE_WRITE = 0,
	PQA_MODE_OFFSET,
	PQA_MODE_MAX,
} VIP_PQ_ADAPTIVE_MODE;

typedef enum _VIP_PQ_ADAPTIVE_INPUT_ITEM {	/* 20140103	rock add by MA_flow*/
	I_VD_noise_status = 0,
	I_VD_noise_status_offset,
	I_RTNR_MAD,
	I_DCC_Histogram_mean,
	I_DCC_Histogram_mean_dark,
	I_DCC_Histogram_mean_dark_gain,
	I_FMV_Hist_Weighting,
	I_FMV_Hist_stillIdx_th,
	I_FMV_Hist_motionIdx_offset,
	I_HMC_MV_Hist_Weighting,
	I_HMC_MV_Hist_stillIdx_th,
	I_HMC_MV_Hist_motionIdx_offset,
	I_SHP_MAD,
	I_CorrectionBit_Histogram_mean,
	I_CorrectionBit_Flame_th,
	I_MPEG_MAD,
	I_LC_0,
	I_LC_1,
	I_LC_2,
	I_LC_3,
	I_MAD_Hist_Th,

	I_SPM_VD_status,
	I_SPM_MAD_Noise_Index,
	I_SPM_MAD_Still_Index,
	I_SPM_FMV_Still_Index,
	I_SPM_Hist_Mean_Y,

	PQA_I_ITEM_MAX,
} VIP_PQ_ADAPTIVE_INPUT_ITEM;

typedef enum _VIP_PQ_ADAPTIVE_TABLE {	/* 20140103	rock add by MA_flow*/
	PQA_input_type = 0,
	PQA_input_item,
	PQA_L00,
	PQA_L01,
	PQA_L02,
	PQA_L03,
	PQA_L04,
	PQA_L05,
	PQA_L06,
	PQA_L07,
	PQA_L08,
	PQA_L09,
	PQA_reg,
	PQA_bitup,
	PQA_bitlow,
	PQA_LEVEL_MAX,
} VIP_PQ_ADAPTIVE_TABLE;

typedef enum _PQA_INPUT_TYPE {         /* 20140103	rock add by PQA_flow*/
	I_DNR = 0,
	I_MPEGNR,
	I_DNR_OFF,
	I_DNR_LOW,
	I_DNR_MID,
	I_DNR_HIGH,
	I_DNR_AUTO,
	I_INTELL_PIC,	/* for TV002*/
	I_ID,
	I_LC,

    PQA_INPUT_TYPE_MAX,
} PQA_INPUT_TYPE;

typedef enum _PQA_INPUT_ITEM {         /* 20140103	rock add by PQA_flow*/
	I_005_RFCVBS = 0,
	I_005_SDHD,
    I_MOTION_RTNR,
    I_MOTION_SPNR,
    I_MPEG_SHARP,
    I_PATTEN_SHARP1,

	I_002_SDHD_RTNR,
	I_002_SDHD_SNR,
	I_002_RFCVBS_RTNR,
	I_002_RFCVBS_SNR,
	I_002_VD_RFCVBS_SNR,
	I_002_RTNR_0,
	I_002_RTNR_1,
	I_002_RTNR_2,
	I_002_SNR_0,
	I_002_SNR_1,
	I_002_SNR_2,
	I_002_CorrBit_Ctrl,

	I_003_RTNR_TH1,
	I_003_RTNR_TH2,
	I_003_RTNR_TH3,
	I_003_RTNR_TH4,
	I_003_RTNR_TH5,
	I_003_RTNR_TH6,
	I_003_RTNR_TH7,

	I_006_RTNR_Mode0,
	I_006_RTNR_Mode1,
	I_006_RTNR_Mode2,
	I_006_RTNR_Mode3,
	I_006_RTNR_Mode4,
	I_006_SNR_Mode0,
	I_006_SNR_Mode1,
	I_006_SNR_Mode2,
	I_006_SNR_Mode3,
	I_006_SNR_Mode4,
	I_006_SHP_Mode0,
	I_006_SHP_Mode1,
	I_006_SHP_Mode2,
	I_006_SHP_Mode3,
	I_006_SHP_Mode4,
	I_006_RTNR_Mode2_HMCNR,

	I_SPM_LV_CTRL0,

    PQA_INPUT_ITEM_MAX,
} PQA_INPUT_ITEM;

typedef struct _VIP_PQA_System_Setting_INFO {
	unsigned char PQA_table_select;
	unsigned char PQA_Input_table_select;
	unsigned char PQA_select_Mode;		/* no use?*/
	unsigned char PQA_select_Input;		/* no use?*/
	unsigned char Run_Flag[PQA_INPUT_TYPE_MAX];
	unsigned int OFFSET_TEMP[PQA_ITEM_MAX];
	unsigned char PQA_Input_Item_Check_Flag[PQA_INPUT_ITEM_MAX];
	unsigned char Flow_Ctrl_Input_Type[PQA_INPUT_TYPE_MAX];
	unsigned char Flow_Ctrl_Input_Item[PQA_INPUT_ITEM_MAX];
	unsigned char Flow_Ctrl_Input_Type_setFlag[PQA_INPUT_TYPE_MAX];
	unsigned char Flow_Ctrl_Input_Item_setFlag[PQA_INPUT_ITEM_MAX];
} VIP_PQA_System_Setting_INFO;

typedef struct _VIP_PQA_ISR_Calculate_INFO {
	unsigned short Input_Level[PQA_INPUT_ITEM_MAX];
	unsigned short Input_rate[PQA_INPUT_ITEM_MAX];
	unsigned char ContentCheck_Flag[PQA_CC_MAX];
	unsigned int Level_Index[PQA_Level_Idx_MAX];
} VIP_PQA_ISR_Calculate_INFO;

/************************************************************/
/*********** PQA INFO END **** PQA INFO EDN *******************/
/************************************************************/

/************************************************************/
/*********************** MA I SNR and ESGM *******************/
/************************************************************/
#define MA_SNR_IESM_TBL_MAX PQA_TABLE_MAX

typedef struct {
	unsigned int RTNR_isnr_Get_Motion_En; //0
	unsigned int MA_isnr_Get_Motion_EdgeMask;
	unsigned int MA_isnr_Get_Motion_YDiffMask;
	unsigned int MA_isnr_Get_Motion_MotionOffset;
	unsigned int MA_isnr_Get_Motion_EdgeTh_1;
	unsigned int MA_isnr_Get_Motion_EdgeTh_2; // 5
	unsigned int MA_isnr_Get_Motion_EdgeTh_3;
	unsigned int MA_isnr_Get_Motion_EdgeTh_4;
	unsigned int MA_isnr_Get_Motion_MotionTh_L_1;
	unsigned int MA_isnr_Get_Motion_MotionTh_L_2;
	unsigned int MA_isnr_Get_Motion_MotionTh_L_3; // 10
	unsigned int MA_isnr_Get_Motion_MotionTh_M_1;
	unsigned int MA_isnr_Get_Motion_MotionTh_M_2;
	unsigned int MA_isnr_Get_Motion_MotionTh_M_3;
	unsigned int MA_isnr_Get_Motion_MotionTh_N_1;
	unsigned int MA_isnr_Get_Motion_MotionTh_N_2; // 15
	unsigned int MA_isnr_Get_Motion_MotionTh_N_3;
	unsigned int MA_isnr_Get_Motion_MotionTh_H_1;
	unsigned int MA_isnr_Get_Motion_MotionTh_H_2;
	unsigned int MA_isnr_Get_Motion_MotionTh_H_3;
} DRV_MA_ISNR_GetMotion;

typedef struct {
	unsigned int nr_motion_en;
	unsigned int nr_motion_weight1_0;
	unsigned int nr_motion_weight1_1;
	unsigned int nr_motion_weight1_2;
	unsigned int nr_motion_weight1_3;
	unsigned int nr_motion_weight1_4;
	unsigned int nr_motion_weight1_5;
	unsigned int nr_motion_weight1_6;
	unsigned int nr_motion_weight1_7;
	unsigned int nr_motion_weight1_8;
	unsigned int nr_motion_weight1_9;
	unsigned int nr_motion_weight1_10;
	unsigned int nr_motion_weight1_11;
	unsigned int nr_motion_weight1_12;
	unsigned int nr_motion_weight1_13;
	unsigned int nr_motion_weight1_14;
	unsigned int nr_motion_weight1_15;
	unsigned int nr_motion_weight1_16;
	unsigned int nr_motion_weight1_17;
	unsigned int nr_motion_weight1_18;
	unsigned int nr_motion_weight1_19;
	unsigned int nr_motion_weight1_20;
	unsigned int nr_motion_weight1_21;
	unsigned int nr_motion_weight1_22;
	unsigned int nr_motion_weight1_23;
	unsigned int nr_motion_weight1_24;
	unsigned int nr_motion_weight1_25;
	unsigned int nr_motion_weight1_26;
	unsigned int nr_motion_weight1_27;
	unsigned int nr_motion_weight1_28;
	unsigned int nr_motion_weight1_29;
	unsigned int nr_motion_weight1_30;
	unsigned int nr_motion_weight1_31;
	unsigned int nr_motion_signwt1; //Merlin3
	unsigned int nr_motion_signwt2; //Merlin3
}DRV_MA_SNR_Motion_Weight1;
typedef struct {
	unsigned int nr_motion_weight2_0;
	unsigned int nr_motion_weight2_1;
	unsigned int nr_motion_weight2_2;
	unsigned int nr_motion_weight2_3;
	unsigned int nr_motion_weight2_4;
	unsigned int nr_motion_weight2_5;
	unsigned int nr_motion_weight2_6;
	unsigned int nr_motion_weight2_7;
	unsigned int nr_motion_weight2_8;
	unsigned int nr_motion_weight2_9;
	unsigned int nr_motion_weight2_10;
	unsigned int nr_motion_weight2_11;
	unsigned int nr_motion_weight2_12;
	unsigned int nr_motion_weight2_13;
	unsigned int nr_motion_weight2_14;
	unsigned int nr_motion_weight2_15;
	unsigned int nr_motion_weight2_16;
	unsigned int nr_motion_weight2_17;
	unsigned int nr_motion_weight2_18;
	unsigned int nr_motion_weight2_19;
	unsigned int nr_motion_weight2_20;
	unsigned int nr_motion_weight2_21;
	unsigned int nr_motion_weight2_22;
	unsigned int nr_motion_weight2_23;
	unsigned int nr_motion_weight2_24;
	unsigned int nr_motion_weight2_25;
	unsigned int nr_motion_weight2_26;
	unsigned int nr_motion_weight2_27;
	unsigned int nr_motion_weight2_28;
	unsigned int nr_motion_weight2_29;
	unsigned int nr_motion_weight2_30;
	unsigned int nr_motion_weight2_31;
}DRV_MA_SNR_Motion_Weight2;

typedef struct {

	unsigned int IESM_Motion_En; //0
	unsigned int IESM_I2P_Blend_Mode;
	unsigned int IESM_Motion_Teeth_Gain_En;
	unsigned int IESM_Motion_Teeth_Gain;
	unsigned int IESM_Motion_Teeth_Gain_H;
	unsigned int IESM_Motion_Teeth_Gain_V; //5
	unsigned int IESM_Motion_Teeth_Gain_Dia;
	unsigned int IESM_Motion_Teeth_Gain_CDia;
	unsigned int AVLPF_Motion_En;

}DRV_MA_IESM_Motion_Ctrl;


typedef struct {
	unsigned int IESM_Motion_Weight_0;
	unsigned int IESM_Motion_Weight_1;
	unsigned int IESM_Motion_Weight_2;
	unsigned int IESM_Motion_Weight_3;
	unsigned int IESM_Motion_Weight_4;
	unsigned int IESM_Motion_Weight_5;
	unsigned int IESM_Motion_Weight_6;
	unsigned int IESM_Motion_Weight_7;
	unsigned int IESM_Motion_Weight_8;
	unsigned int IESM_Motion_Weight_9;
	unsigned int IESM_Motion_Weight_10;
	unsigned int IESM_Motion_Weight_11;
	unsigned int IESM_Motion_Weight_12;
	unsigned int IESM_Motion_Weight_13;
	unsigned int IESM_Motion_Weight_14;
	unsigned int IESM_Motion_Weight_15;
	unsigned int IESM_Motion_Weight_16;
	unsigned int IESM_Motion_Weight_17;
	unsigned int IESM_Motion_Weight_18;
	unsigned int IESM_Motion_Weight_19;
	unsigned int IESM_Motion_Weight_20;
	unsigned int IESM_Motion_Weight_21;
	unsigned int IESM_Motion_Weight_22;
	unsigned int IESM_Motion_Weight_23;
	unsigned int IESM_Motion_Weight_24;
	unsigned int IESM_Motion_Weight_25;
	unsigned int IESM_Motion_Weight_26;
	unsigned int IESM_Motion_Weight_27;
	unsigned int IESM_Motion_Weight_28;
	unsigned int IESM_Motion_Weight_29;
	unsigned int IESM_Motion_Weight_30;
	unsigned int IESM_Motion_Weight_31;
}DRV_MA_IESM_Motion_Weight;

typedef struct {
	unsigned int AVLPF_Motion_Weight_0;
	unsigned int AVLPF_Motion_Weight_1;
	unsigned int AVLPF_Motion_Weight_2;
	unsigned int AVLPF_Motion_Weight_3;
	unsigned int AVLPF_Motion_Weight_4;
	unsigned int AVLPF_Motion_Weight_5;
	unsigned int AVLPF_Motion_Weight_6;
	unsigned int AVLPF_Motion_Weight_7;
	unsigned int AVLPF_Motion_Weight_8;
	unsigned int AVLPF_Motion_Weight_9;
	unsigned int AVLPF_Motion_Weight_10;
	unsigned int AVLPF_Motion_Weight_11;
	unsigned int AVLPF_Motion_Weight_12;
	unsigned int AVLPF_Motion_Weight_13;
	unsigned int AVLPF_Motion_Weight_14;
	unsigned int AVLPF_Motion_Weight_15;
	unsigned int AVLPF_Motion_Weight_16;
	unsigned int AVLPF_Motion_Weight_17;
	unsigned int AVLPF_Motion_Weight_18;
	unsigned int AVLPF_Motion_Weight_19;
	unsigned int AVLPF_Motion_Weight_20;
	unsigned int AVLPF_Motion_Weight_21;
	unsigned int AVLPF_Motion_Weight_22;
	unsigned int AVLPF_Motion_Weight_23;
	unsigned int AVLPF_Motion_Weight_24;
	unsigned int AVLPF_Motion_Weight_25;
	unsigned int AVLPF_Motion_Weight_26;
	unsigned int AVLPF_Motion_Weight_27;
	unsigned int AVLPF_Motion_Weight_28;
	unsigned int AVLPF_Motion_Weight_29;
	unsigned int AVLPF_Motion_Weight_30;
	unsigned int AVLPF_Motion_Weight_31;
} DRV_MA_VLPF_Motion_Weight;

typedef struct {
	DRV_MA_ISNR_GetMotion	S_MA_ISNR_GetMotion;
	DRV_MA_SNR_Motion_Weight1 	S_MA_SNR_Motion_Weight1;
	DRV_MA_SNR_Motion_Weight2 	S_MA_SNR_Motion_Weight2;
	DRV_MA_IESM_Motion_Ctrl 	S_MA_IESM_Motion_Teeth;
	DRV_MA_IESM_Motion_Weight 	S_MA_IESM_Motion_Weight;
	DRV_MA_VLPF_Motion_Weight	S_MA_VLPF_Motion_Weight;

}DRV_MA_SNR_IESM_Coef;

typedef struct {
	unsigned char TBL_sel;
	DRV_MA_SNR_IESM_Coef MA_SNR_IESM_TBL[MA_SNR_IESM_TBL_MAX];

}DRV_MA_SNR_IESM_Coef_CTRL;
/************************************************************/
/*********************** MA I SNR and ESGM *******************/
/************************************************************/

/************************************************************/
/*********** Profile START **** Profile START ********************/
/************************************************************/

//For store initial value
typedef enum _VIP_PROFILE_TABLE {
	PROFILE_1_TABLE = 0,
	PROFILE_2_TABLE,
	PROFILE_3_TABLE,
	PROFILE_TABLE_TOTAL,
} VIP_PROFILE_TABLE;

typedef struct
{
	unsigned  char en_4k2k_mode_t;
	unsigned  char en_3d_mode_t;
	unsigned  char ch1_pf_yuv_sel_t;
	unsigned  char ch1_pf_field_sel_t;
	unsigned  char ch1_pf_overflow_sel_t;
	unsigned  char ch1_pf_enable_profile_t;
}DRV_Vip_ICH1_Hist_Profile_CTRL_T;

typedef struct
{
	unsigned  short ch1_pf_starth_t;
}DRV_Vip_ICH1_Hist_Profile_StartH_T;

typedef struct
{
	unsigned  short ch1_pf_startv_t;
}DRV_Vip_ICH1_Hist_Profile_StartV_T;

typedef struct
{
	DRV_Vip_ICH1_Hist_Profile_CTRL_T VipICH1_Hist_Profile_CTRL_t;
	DRV_Vip_ICH1_Hist_Profile_StartH_T VipICH1_Hist_Profile_StartH_t;
	DRV_Vip_ICH1_Hist_Profile_StartV_T VipICH1_Hist_Profile_StartV_t;
}DRV_Vip_Profile_Table;

typedef struct
{
	DRV_Vip_Profile_Table  Vip_Profile_Table[PROFILE_TABLE_TOTAL];

}VIP_Profile_Table_Coef;


//For write registers
typedef struct
{
	unsigned  char en_4k2k_mode;
	unsigned  char en_3d_mode;
	unsigned  char ch1_pf_yuv_sel;
	unsigned  char ch1_pf_field_sel;
	unsigned  char ch1_pf_overflow_sel;
	unsigned  char ch1_pf_enable_profile;
}DRV_Vip_ICH1_Hist_Profile_CTRL;

typedef struct
{
	unsigned  short ch1_pf_vsize;
	unsigned  short ch1_pf_hsize;
}DRV_Vip_ICH1_Hist_Profile_Size;

typedef struct
{
	unsigned  short ch1_pf_starth;
}DRV_Vip_ICH1_Hist_Profile_StartH;

typedef struct
{
	unsigned  short ch1_pf_startv;
}DRV_Vip_ICH1_Hist_Profile_StartV;

typedef struct
{
	DRV_Vip_ICH1_Hist_Profile_CTRL VipICH1_Hist_Profile_CTRL;
	DRV_Vip_ICH1_Hist_Profile_Size  VipICH1_Hist_Profile_Size;
	DRV_Vip_ICH1_Hist_Profile_StartH VipICH1_Hist_Profile_StartH;
	DRV_Vip_ICH1_Hist_Profile_StartV VipICH1_Hist_Profile_StartV;
}DRV_Vip_Profile_Info;

typedef struct
{
	DRV_Vip_Profile_Info  Vip_Profile_Info[PROFILE_TABLE_TOTAL];

}DRV_Vip_Profile;

#define Profile_Seg_Num 16
#define Profile_Seg_Num_shiftBit 4

typedef struct _VIP_Profile_Cal_INFO {
	unsigned short H_Block_Y[Profile_Seg_Num];
	unsigned short H_Block_U[Profile_Seg_Num];
	unsigned short H_Block_V[Profile_Seg_Num];
	unsigned short V_Block_Y[Profile_Seg_Num];
	unsigned short V_Block_U[Profile_Seg_Num];
	unsigned short V_Block_V[Profile_Seg_Num];
	unsigned char ProfileFlag_Y;
	unsigned char ProfileFlag_U;
	unsigned char ProfileFlag_V;
	unsigned short Profile2_H_Block_Y[Profile_Seg_Num];
	unsigned short Profile2_H_Block_U[Profile_Seg_Num];
	unsigned short Profile2_H_Block_V[Profile_Seg_Num];
	unsigned short Profile2_V_Block_Y[Profile_Seg_Num];
	unsigned short Profile2_V_Block_U[Profile_Seg_Num];
	unsigned short Profile2_V_Block_V[Profile_Seg_Num];
	unsigned char Profile2_Flag_Y;
	unsigned char Profile2_Flag_U;
	unsigned char Profile2_Flag_V;
	unsigned short Profile3_H_Block_Y[Profile_Seg_Num];
	unsigned short Profile3_H_Block_U[Profile_Seg_Num];
	unsigned short Profile3_H_Block_V[Profile_Seg_Num];
	unsigned short Profile3_V_Block_Y[Profile_Seg_Num];
	unsigned short Profile3_V_Block_U[Profile_Seg_Num];
	unsigned short Profile3_V_Block_V[Profile_Seg_Num];
	unsigned char Profile3_Flag_Y;
	unsigned char Profile3_Flag_U;
	unsigned char Profile3_Flag_V;
	unsigned short ProfileAPL_Y;
} VIP_Profile_Cal_INFO;

/************************************************************/
/*********** Profile END **** Profile END ************************/
/************************************************************/

/*===================================*/
/*======== I de_Contour  =================*/
/*===================================*/
#define I_De_Contour_TBL_Max 10

typedef struct _VIP_I_De_Contour_TBL {
	unsigned short decont_en;
	unsigned short decont_snr_blend_mode;
	unsigned short decont_blend_lowbd;
	unsigned short decont_blend_step;
	unsigned short decont_blend_weight_lpf_en;
	unsigned short decont_neighbor_far;
	//unsigned short decont_filter_mode;
	//unsigned short decont_line_mode;
	unsigned short decont_yc_mode;

	unsigned short decont_contrast_th_y;
	unsigned short decont_contrast_th2_y;
	unsigned short decont_contrast_th3_y;
	unsigned short decont_dark_th;
	unsigned short decont_dark_contrast_th;
	unsigned short decont_dark_contrast_th2;
	unsigned short decont_dark_contrast_th3;
	unsigned short decont_contrast_th_c;
	unsigned short decont_contrast_th2_c;
	unsigned short decont_contrast_th3_c;
	unsigned short decont_neighbor_diff_th_y;
	unsigned short decont_neighbor_diff_th2_y;
	unsigned short decont_neighbor_diff_th_c;
	unsigned short decont_neighbor_diff_th2_c;

	unsigned short decont_gradation_th_y;
	unsigned short decont_gradation_th2_y;
	unsigned short decont_gradation_th_c;
	unsigned short decont_gradation_th2_c;
	unsigned short decont_maxmindiff_th_y;
	unsigned short decont_maxmindiff_th2_y;
	unsigned short decont_maxmindiff_th3_y;
	unsigned short decont_maxmindiff_th_c;
	unsigned short decont_maxmin_lv0;
	unsigned short decont_maxmin_lv1;
	unsigned short decont_maxmin_lv2;
	unsigned short decont_maxmin_lv3;
	unsigned short decont_level_range_hi;
	unsigned short decont_level_range_md;
	unsigned short decont_level_range_lo;
	unsigned short decont_blend_gain_md;
	unsigned short decont_blend_gain_lo;

	unsigned short decont_bottleneck_en;
	unsigned short decont_bottleneck_range0;
	unsigned short decont_bottleneck_range1;
	unsigned short decont_bottleneck_range2;
	unsigned short decont_bottleneck_range3;
	//unsigned short decont_bottleneck_range4;
	unsigned short decont_diff_clamp_y;
	unsigned short decont_diff_clamp_c;

} VIP_I_De_Contour_TBL;

typedef struct _VIP_I_De_Contour_CTRL {
	unsigned char table_select;
	VIP_I_De_Contour_TBL De_Contour_TBL[I_De_Contour_TBL_Max];

} VIP_I_De_Contour_CTRL;

/*===================================*/
/*======== I de_Contour   =================*/
/*===================================*/

/*===================================*/
/*======== SLD =======================*/
/*===================================*/
#define SLD_Table_NUM 4

//juwen, 0603
typedef struct {
	unsigned char sld_en;
	unsigned char sld_blend_en;
	unsigned char sld_global_drop;
	unsigned char sld_hpf_type;
	unsigned char sld_hpf_thl;
	unsigned char sld_scale_mode;

} DRV_SLD_Ctrl;


typedef struct {
	unsigned short sld_height;
	unsigned short sld_width;

} DRV_SLD_Size;

typedef struct {
	unsigned char table_sel;
	DRV_SLD_Size SLD_Size;
} VIP_SLD_INFO;

//juwen, 0603
typedef struct {
	unsigned short sld_curve_seg_0;
	unsigned short sld_curve_seg_1;

	short sld_curve_gain_0;
	short sld_curve_gain_1;
	short sld_curve_gain_2;

	short sld_curve_offset_0;
	short sld_curve_offset_1;
	short sld_curve_offset_2;

} DRV_SLD_CurveMap;

//juwen, 0603
typedef struct {
	unsigned char sld_cnt_thl;
	unsigned char sld_hpf_diff_thl;
	unsigned char sld_pixel_diff_thl;
} DRV_SLD_DROP;


typedef struct {
	DRV_SLD_Ctrl SLD_Ctrl;
	DRV_SLD_CurveMap SLD_CurveMap;
	DRV_SLD_DROP SLD_DROP;

} DRV_Still_Logo_Detection_Table;
/*===================================*/
/*======== SLD =======================*/
/*===================================*/

/*===================================*/
/*============= UZU ===================*/
/*===================================*/
typedef enum _VIP_Two_Step_UZU_MODE
{
	Two_Step_UZU_Disable = 0,
	Two_Step_UZU_Enable,
	Two_Step_UZU_Input_120Hz,

	Two_Step_UZU_MAX,

} VIP_Two_Step_UZU_MODE;

/*===================================*/
/*============= UZU ===================*/
/*===================================*/

/************************************************************/
/*********** For Customer table use*****************************/
/************************************************************/

/************************************************************/
/******************* For TV002 use*****************************/
/************************************************************/
/****************** VIP_QUALITY_Coef_TV002 ************************/
#if 1
#define VIP_QUALITY_FUNCTION_TV002_BP 	0
#define VIP_QUALITY_FUNCTION_TV002_ADV_API 	1

/****************** intelligent pic function ************************/
typedef struct _VIP_INTELLIGENT_PICTURE_Function {
	unsigned char En_Flag;
	unsigned char PICTURE_OPTIMISATION;
	unsigned char INITIAL_FLAG;
	unsigned char Sig_Lv_Indicator_En;
	unsigned char Sig_Lv;

} VIP_INTELLIGENT_PICTURE_Function;

/* *************Magic Gamma*************************/
#define MAGIC_GAMMA_POINT_NUM 11
#define MAGIC_GAMMA_GAMMA_COEF_BIT_NUM 10

typedef enum _VIP_MAGIC_GAMMA_PICTURE_MODE {
	MAGIC_GAMMA_PICTURE_MODE_USER = 0,
	MAGIC_GAMMA_PICTURE_MODE_VIVID,
	MAGIC_GAMMA_PICTURE_MODE_STD,
	MAGIC_GAMMA_PICTURE_MODE_GENTLE,
	MAGIC_GAMMA_PICTURE_MODE_MOVIE,
	MAGIC_GAMMA_PICTURE_MODE_SPORT,
	MAGIC_GAMMA_PICTURE_MODE_GAME,
	MAGIC_GAMMA_PICTURE_MODE_AUTO_VIEW,
	MAGIC_GAMMA_PICTURE_MODE_DYNAMIC,
	MAGIC_GAMMA_PICTURE_MODE_STANDARD,
	MAGIC_GAMMA_PICTURE_MODE_MILD,
	MAGIC_GAMMA_PICTURE_MODE_ECO,
	MAGIC_GAMMA_PICTURE_MODE_PC,
	MAGIC_GAMMA_PICTURE_MODE_CRICKET,
	VIP_MAGIC_GAMMA_PICTURE_MODE_MAX,

} VIP_MAGIC_GAMMA_PICTURE_MODE;	/* PICTURE_MODE is reffer to VIP_MAGIC_GAMMA_PICTURE_MODE in tv002*/

typedef enum _VIP_MAGIC_GAMMA_COLORTEMP_LEVEL {
	MAGIC_GAMMA_COLORTEMP_USER = 0,
	MAGIC_GAMMA_COLORTEMP_NORMAL,
	MAGIC_GAMMA_COLORTEMP_WARMER,
	MAGIC_GAMMA_COLORTEMP_WARM,
	MAGIC_GAMMA_COLORTEMP_COOL,
	MAGIC_GAMMA_COLORTEMP_COOLER,
	VIP_MAGIC_GAMMA_COLORTEMP_MAX_NUM,

} VIP_MAGIC_GAMMA_COLORTEMP_LEVEL;

typedef struct _VIP_MAGIC_GAMMA_CTRL {
	char PanelTypeID[15];	/* no needed?*/
	unsigned char Magic_Number;
	unsigned char shift_Bit;
} VIP_MAGIC_GAMMA_CTRL;

typedef struct _VIP_MAGIC_GAMMA_Curve_Data {
	unsigned short GammaR[MAGIC_GAMMA_POINT_NUM];
	unsigned short GammaG[MAGIC_GAMMA_POINT_NUM];
	unsigned short GammaB[MAGIC_GAMMA_POINT_NUM];
	unsigned short Gamma_Coef[VIP_MAGIC_GAMMA_PICTURE_MODE_MAX][MAGIC_GAMMA_POINT_NUM];
} VIP_MAGIC_GAMMA_Curve_Data;

typedef struct _VIP_MAGIC_GAMMA_COLORTEMP_DATA {
	unsigned short	R_val;
	unsigned short	G_val;
	unsigned short	B_val;
	unsigned short	R_offset_val;
	unsigned short	G_offset_val;
	unsigned short	B_offset_val;
} VIP_MAGIC_GAMMA_COLORTEMP_DATA;

typedef struct _VIP_MAGIC_GAMMA_Function {
	VIP_MAGIC_GAMMA_CTRL MAGIC_GAMMA_CTRL;
	VIP_MAGIC_GAMMA_Curve_Data MAGIC_GAMMA_Curve_Data;
	VIP_MAGIC_GAMMA_COLORTEMP_DATA Fac_ColorTemp[VIP_MAGIC_GAMMA_COLORTEMP_MAX_NUM];
} VIP_MAGIC_GAMMA_Function;

/* ********************BP function **************************/
#define BP_TBL_MAX 10

typedef enum _VIP_BP_SEG {
	BP_SEG_0 = 0,
	BP_SEG_1,
	BP_SEG_2,
	BP_SEG_3,
	BP_SEG_4,

    BP_SEG_MAX,
} VIP_BP_SEG;

typedef struct _VIP_BP_Gain_Point {
	unsigned short Gain[BP_SEG_MAX];
	unsigned short Point[BP_SEG_MAX];

} VIP_BP_Gain_Point;

typedef struct _VIP_BP_Bri_Compensation {
	unsigned short Enable;
	unsigned short Cmps_Gain[BP_SEG_MAX];
	unsigned short Cmps_Point[BP_SEG_MAX];

} VIP_BP_Bri_Compensation;

typedef struct _VIP_BP_Gain_Limit {
	unsigned short Enable;
	unsigned short OverSat_Tol;
	unsigned short OverSat_Th;
	unsigned short OverSat_StepBit;

} VIP_BP_Gain_Limit;

typedef struct _VIP_BP_Smooth_Change {
	unsigned short Enable;
	unsigned short Init_Flag;
	unsigned short Step;
	unsigned short Delay_Flame;
	unsigned short Step_offset;
	unsigned short Delay_Flame_offset;

} VIP_BP_Smooth_Change;

typedef struct _VIP_BP_Debug_Items {
	unsigned short Enable;
	unsigned short APL;
	unsigned short Debug_APL;
	unsigned short Log;
	unsigned short Log_Delay;

} VIP_BP_Debug_Items;

typedef struct _VIP_BP_TBL {
	unsigned short BP_Enable;
	VIP_BP_Gain_Point BP_Gain_Point;
	VIP_BP_Bri_Compensation Bri_Cmps;
	VIP_BP_Gain_Limit Gain_Limit;
	VIP_BP_Smooth_Change Smooth_Change;

} VIP_BP_TBL;

typedef struct _VIP_BP_CTRL {
	unsigned short BP_TBL_Select;
	unsigned short BP_Level_TBL_Select;
	unsigned short BP_Shift_Bit;
	unsigned short HistY_Max;
	unsigned short Debug_Enable;
	unsigned short Debug_APL;
	unsigned short Debug_Log;
	unsigned short Debug_Log_Delay;
	unsigned short BP_Level_TBL[BP_TBL_MAX][VIP_MAGIC_GAMMA_PICTURE_MODE_MAX];	/*by pic mode*/

} VIP_BP_CTRL;

typedef struct _VIP_BP_STATUS {
	unsigned short BP_Gain_TBL;
	unsigned short BP_Gain_Limited;
	unsigned short BP_Gain_Smooth;
	unsigned short BP_Gain;
	unsigned short BP_Gain_Limit_Allowed;
	unsigned short BP_Gain_Limit_Weight;

	short BP_CMPS_TBL;
	short BP_CMPS_Smooth;
	short BP_CMPS;

	unsigned short APL;

} VIP_BP_STATUS;

typedef struct _VIP_BP_Function {
	VIP_BP_STATUS BP_STATUS;
	VIP_BP_CTRL BP_Ctrl;
	VIP_BP_TBL BP_Table[BP_TBL_MAX];

} VIP_BP_Function;

/* for DBC Tv002*/
#define DBC_LUT_APL_MAX mean_value_index_MAX
#define DBC_LUT_DEV_MAX Variance_index_MAX
#define DBC_OSD_BL_MAX 100

typedef enum _VIP_DBC_POWER_SAVING_MODE {
	D_PS_Off = 0,
	D_PS_Low,
	D_PS_High,
	D_PS_Picture_Off,

	D_PS_MODE_MAX,
} VIP_DBC_POWER_SAVING_MODE;

typedef struct _VIP_DBC_SW_REG {
		unsigned char BL_TOP;
		unsigned char BL_MIDDLE;
		unsigned char BL_BOTTOM;
		unsigned char BL_SETUDEN;
		unsigned char BL_PSH;
		unsigned char DBC_APL_LOW_START;
		unsigned char DBC_BL_B_LIMIT;
		unsigned char DBC_APL_LOW_LIMIT;
		unsigned char DBC_PS_ENA;
		unsigned char DBC_PS_TIME;
		unsigned char DBC_PS_LEVEL;
		unsigned char DBC_PS_APL_TH;
		unsigned char DBC_PS_ATN_DN;
		unsigned char DBC_PS_ATN_UP;
		unsigned char DBC_PS_TM_EXIT;
} VIP_DBC_SW_REG;

typedef struct _VIP_DBC_LUT_TBL {
		unsigned char TBL[DBC_LUT_APL_MAX][DBC_LUT_DEV_MAX];

		char DCC_S_High_CMPS_TBL[D_PS_MODE_MAX][DBC_LUT_APL_MAX][DBC_LUT_DEV_MAX];

} VIP_DBC_LUT_TBL;

typedef struct _VIP_DBC_STATUS {
	unsigned char Basic_Backlight;
	unsigned char OSD_PowerSave;
	unsigned char OSD_Backlight;
	unsigned char APL_in_SmartFit;
	unsigned char DBC_DBC;
	unsigned char Y_MAX;
	unsigned char DBC_Backlight;
	unsigned char DBC_Smooth;
	unsigned char APL;
	unsigned char DEV;
	char DBC_DCC_S_High_CMPS;
	char DBC_DCC_S_High_CMPS_LUT;

} VIP_DBC_STATUS;

typedef struct _VIP_DBC_CTRL {
	unsigned char DBC_Enable;
	unsigned char Debug_Log0;
	unsigned char Debug_Log1;
	unsigned char Debug_Delay_Flame;
	unsigned char BL_Debug_ENA;
	unsigned char BL_Debug_APL;

} VIP_DBC_CTRL;

typedef struct _VIP_DBC {
	VIP_DBC_STATUS DBC_STATUS;
	VIP_DBC_CTRL DBC_CTRL;
	VIP_DBC_SW_REG DBC_SW_REG;
        VIP_DBC_LUT_TBL DBC_LUT_TBL;
} VIP_DBC;

// ******************************** for Zero-D Ctrl *******************************************
#define ZERO_D_LOOKUP_TBL_ROW 33
#define ZERO_D_LOOKUP_TBL_COLUMN 33

typedef struct _VIP_ZERO_D_CTRL {
	unsigned char Enable_Flag;
	unsigned char LookupTBL_shiftBit;
	unsigned char HistY_BitNum;
	unsigned char Debug_Log0;
	unsigned char Debug_Log1;
	unsigned char Debug_Delay_Flame;
	unsigned char BL_Debug_ENA;
	unsigned char BL_Debug_APL;

}VIP_ZERO_D_CTRL;

typedef struct _VIP_ZERO_D_LOOKUP_TBL {
	unsigned short HistY_Bin_Ignore_Sum;	// permillage
	unsigned short Gain_Smooth_Step;
	unsigned short Gain_Smooth_DelayFlame;
	unsigned short Backlight_Smooth_Step;
	unsigned short Backlight_Smooth_DelayFlame;

	unsigned short TBL[ZERO_D_LOOKUP_TBL_ROW][ZERO_D_LOOKUP_TBL_COLUMN];	// luminance table
	unsigned short TBL_Idx_PWM[ZERO_D_LOOKUP_TBL_ROW];
	unsigned short TBL_Idx_ContentAPL[ZERO_D_LOOKUP_TBL_COLUMN];

}VIP_ZERO_D_LOOKUP_TBL;

typedef struct _VIP_ZERO_D_STATUS {
	unsigned short Zero_D_PWM_APL_Target;
	unsigned short Zero_D_Target_Idx_PWM;
	unsigned short Zero_D_Target_Idx_APL;

	unsigned short Zero_D_Gained_APL;

	unsigned short Zero_D_Gain;
	unsigned short Zero_D_Offset;
	unsigned short Zero_D_Backlight;
	unsigned short Zero_D_Gain_Smooth;
	unsigned short Zero_D_Backlight_Smooth;
	unsigned short Zero_D_Gain_Hist_Limit;
	unsigned short Zero_D_Backlight_Hist_Limit;

}VIP_ZERO_D_STATUS;

typedef struct _VIP_ZERO_D {
	VIP_ZERO_D_STATUS ZERO_D_STATUS;
	VIP_ZERO_D_CTRL ZERO_D_CTRL;
	VIP_ZERO_D_LOOKUP_TBL ZERO_D_LOOKUP_TBL;

}VIP_ZERO_D;
#endif
/************************************************************/
/******************* For TV002 use*****************************/
/************************************************************/

/************************************************************************************************************/
/*********** HDR CTRL START *******HDR CTRL START ***** HDR CTRL START***************************************/
/************************************************************************************************************/
typedef enum _VIP_HDR_MODE {
	ITU_SMPTE2084 = 0,
	ITU_BT1886,
	ITU_ARIB_STD_B67,
	UnKnown,

	HDR_MODE_Max,
} VIP_HDR_MODE;

typedef struct _VIP_HDR_CTRL {
	unsigned char EOTF_Mode;
	unsigned char EOTF_sub_Mode;
} VIP_HDR_CTRL;

/************************************************************************************************************/
/*********** HDR CTRL END *******HDR CTRL END ***** HDR CTRL END***************************************/
/************************************************************************************************************/
/************************************************************************************************************/
/*********** DITHER CTRL START *******DITHER CTRL START ***** DITHER CTRL START***************************************/
/************************************************************************************************************/
typedef struct _VIP_DITHER_CTRL {
	unsigned char DITHER_ENABLE;
	unsigned char DITHER_MAP_BIT;
	unsigned char DITHER_LUT;
} VIP_DITHER_CTRL;
/************************************************************************************************************/
/*********** DITHER CTRL END *******DITHER CTRL END ***** DITHER CTRL END***************************************/
/************************************************************************************************************/

/************************************************************/
/*********** For Customer table use*****************************/
/************************************************************/
typedef struct _VIP_ISR_Printf_Flag {
	unsigned int Delay_Time;
	unsigned int Flag0;
	unsigned int Flag1;
	unsigned int TV002_Flag;
	unsigned int MAD_Hist_Flag0;
	unsigned int PQA_SPM_Flag0;
	unsigned int FMC_HMC_Flag0;
	unsigned int Y_Hue_Sat_Hist_Flag0;

} VIP_ISR_Printf_Flag;

typedef struct {
	unsigned char	Boundary_Check_Table;	/*0*/
	unsigned char	Level_and_Blend_Coef_Table;
	unsigned char	hist_adjust_table;
	unsigned char	AdaptCtrl_Level_Table;
	unsigned char	User_Curve_Table;
	unsigned char	database_DCC_Table;		/*5*/
	unsigned char	Picture_Mode_Weight;
	unsigned char	DCL_W_level;
	unsigned char	DCL_B_level;
	unsigned char	Expand_W_Level;
	unsigned char	Expand_B_Level;			/*10*/
	unsigned char	Delay_time;
	unsigned char	Step;
	unsigned char	DCC_Table;
	unsigned char	DCC_Level;
	unsigned char	DCC_SCurve_index;		/*15*/
	unsigned char	DCC_SCurve_high;
	unsigned char	DCC_SCurve_low;
	unsigned char	DCC_Chroma_Comp_Table;
	unsigned char	DCC_Color_Dep_Table;
	RPC_DCC_Advance_control_table DCC_Advance_control_table[Advance_control_table_MAX];
	RPC_DCC_Curve_boundary_table DCC_Curve_boundary_table[Curve_boundary_table_MAX];
} VIP_DCC_info_coef;

#define SATSEGMAX	12
#define ITNSEGMAX	12

typedef struct {
	int dHue;
	int dSatBySat[SATSEGMAX];
	int dItnByItn[ITNSEGMAX];
} RPC_ICM_Global_Ctrl;

typedef struct {
	VIP_DCC_info_coef DCC_info_coef;
	VIP_I_D_Dcti_INFO I_D_Dcti_INFO;
	VIP_PQA_System_Setting_INFO PQA_Setting_INFO;
	RPC_ICM_Global_Ctrl S_RPC_ICM_Global_Ctrl;
	VIP_Profile_Table_Coef Profile_Table_Coef;

	VIP_I_De_Contour_CTRL I_De_Contour_CTRL;
	DRV_MA_SNR_IESM_Coef_CTRL sMA_SNR_IESM_Coef_CTRL;
	VIP_SLD_INFO SLD_INFO;

} VIP_PQ_Setting_Info;

typedef struct _VIP_OSD_LAYER_INFO {
	unsigned char Picture_mode;
	unsigned char OSD_Contrast;
	unsigned char OSD_Brightness;
	unsigned char OSD_Saturation;
	unsigned char OSD_Hue;
	unsigned char OSD_Sharpness;
	unsigned char OSD_colorTemp;
	unsigned char OSD_NR_Mode;
	unsigned char OSD_Backlight;
	unsigned char OSD_MpegNR_Mode;
	unsigned char OSD_DCC_Mode;
	unsigned char OSD_DCR_Mode;
	unsigned char OSD_Film_Mode;	/* is not neccessary*/
	unsigned char isIncreaseMode_Flag;
	unsigned int backlightLevel_uiMax;
	unsigned int backlightLevel_uiMin;
	unsigned int backlightLevel_actMax;
	unsigned int backlightLevel_actMin;
	unsigned int backlight_pin_adj;
	unsigned short Contrast_Gain;
	unsigned short Brightness_Gain;
	unsigned short Saturation_Gain;
	unsigned short Hue_Gain;			/* unit:degree*/
	short Brightness_Cmps;
	unsigned short Saturation_Cmps;

	unsigned char OSD_Contrast_Sub;
	unsigned char OSD_Brightness_Sub;
	unsigned char OSD_Saturation_Sub;
	unsigned char OSD_Hue_Sub;
	unsigned short Contrast_Gain_Sub;
	unsigned short Brightness_Gain_Sub;
	unsigned short Saturation_Gain_Sub;
	unsigned short Hue_Gain_Sub;			/* unit:degree*/
	short Brightness_Cmps_Sub;
	unsigned short Saturation_Cmps_Sub;

} VIP_OSD_LAYER_INFO;

typedef enum _VIP_Table_Select {
	VIP_Table_Select_1 = 0,
	VIP_Table_Select_2,

	VIP_Table_Select_Max,
} VIP_Table_Select;

typedef struct {
	unsigned char chromaerror_en;
	unsigned char chromaerror_all;
	unsigned char chromaerror_th;
	unsigned char chromaerror_framemotionc_th;
	unsigned char di444to422Lowpass;
} VIP_MA_ChromaError;

typedef struct _VIP_AutoMA_API_CTRL {
	unsigned char	DI_MiddleWare_Function_En;
	unsigned char	scalerVIP_SMD_weake_move_Detecte_En;
	unsigned char	compute_PQA_level_En;
	unsigned char	scalerVIP_Profile_Block_Motion_En;
	unsigned char	noise_estimation_MAD_et_En;
	unsigned char	Y_U_V_Mad_Calculate_En;
	unsigned char	scalerVIP_ET_Y_MOTION_En;
	unsigned char	scalerVIP_FMV_HMCbin_hist_ratio_Cal_En;
	unsigned char drvif_color_ClueGather_En;
	unsigned char	skin_tone_Level_detect_En;
	unsigned char	pure_color_detect_En;
	unsigned char	scalerVIP_colorHistDetect_En;
	unsigned char	scalerVIP_rtnr_noise_measure_En;
	unsigned char	scalerVIP_color_noise_mpeg_detect_En;
	unsigned char	DI_detect_Champagnet_En;
	unsigned char	motion_concentric_circles_detect_En;
	unsigned char	motion_purecolor_concentric_circles_detect_En;
	unsigned char	motion_move_concentric_circles_detect_En;
	unsigned char	drvif_color_colorbar_dectector_by_SatHueProfile_En;
	unsigned char	black_white_pattern_detect_En;
	unsigned char	scalerVIP_Fade_Detection_En;
	unsigned char	histogram_new_III_dcc_EN;
	unsigned char	scalerVIP_DCR_EN;
	unsigned char	p_film_detection_En;
	unsigned char	scalerVIP_PQ_Adaptive_Table_En;
	unsigned char	scalerVIP_MA_IEdgeSmooth_En;
	unsigned char	scalerVIP_Dynamic_Vertical_NR_En;
	unsigned char	drvif_color_DCTI_for_ColorBar_En;
	unsigned char	scalerVIP_color_noise_mpeg_apply_En;
	unsigned char	scalerVIP_RTNR_correctionbit_period_En;
	unsigned char	scalerVIP_DI_Dynamic_VDNoise_FrameMotion_En;
} VIP_AutoMA_API_CTRL;

typedef enum _NR_DNR_32 {
/*++++++++++++++++++++++++++++++++++++++++++++DNR++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*=========================================== RF_DNR =====================================================*/
/*VD NR*/
	VD_CP_Sw_Edge_Thl = 0,
/*RTNR*/
	CP_Temporal_Y_K0_K7,
	CP_Temporal_Y_K8_K15,
	CP_Temporal_C_K,
	CP_Temporal_Y_K_00_07,
	CP_Temporal_Y_K_10_17,
	CP_Temporal_Y_K_20_27,
	CP_Temporal_Y_K_30_37,
	CP_Temporal_Y_K_40_47,
	CP_Temporal_Y_K_50_57,
	CP_Temporal_Y_K_60_67,
	CP_Temporal_Y_K_70_77,
/*RTNR_Y*/
//	RTNR_Sbl_Thm_Y,
//	RTNR_Sbl_Ths_Y,
/*SPNR*/
	CP_Sw_Edge_Thl,
	Edgedependent_Thrylow,  /*mac2 have no this item*/
	Edgedependent_Thry,
	Modified_Lpf_Thr,

	NR_DNR32_MAX,
} NR_DNR_32;

#define DNR_T0_32    (NR_DNR32_MAX*0)
#define DNR_T1_32    (NR_DNR32_MAX*1)
#define DNR_T2_32    (NR_DNR32_MAX*2)
#define DNR_T3_32    (NR_DNR32_MAX*3)
#define DNR_T4_32    (NR_DNR32_MAX*4)


#define NR_TABLE32_MAX	(NR_DNR32_MAX*5)

typedef enum _NR_INPUT_8 {
/*+++++++++++++++++++++++++++++++++++++++++++DNR+++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*========================================== INPUT ====================================================*/
	VD_noise_status = 0,
	VD_noise_status_offset,
	NR_INPUT8_MAX,

} NR_INPUT_8;
typedef enum _NR_DNR_8 {
/*VD_NR*/
	VD_CP_SResultWeight = 0,
	VD_CP_ZoranFilterSizeY,
	VD_CP_ZoranWeight2Y,
	VD_CP_ZoranWeight3Y,
	VD_CP_ZoranWeight4Y,
	VD_CP_SpatialEnableY,
	VD_CP_SpatialEnableC,
	VD_CP_SpatialThlYx4,
	VD_CP_SpatialThlYx2,
	VD_CP_SpatialThlY,
	VD_CP_FixedWeight1Y,
	VD_CP_FixedWeight2Y,
	VD_CP_Weight1Y,
	VD_CP_Weight2Y,
	VD_CP_RingWeightY,
	VD_CP_RingRange,
	VD_CP_RingGain,
	VD_CP_Edge_Weight,

/*RTNR_Y*/
	CP_RTNR_MAD_Window,
	CP_RTNR_Rounding_Correction,
	CP_RTNR_Y_Enable,
	RTNR_New_Method_En,
	CP_TemporalThlY14,
	CP_TemporalThlY13,
	CP_TemporalThlY12,
	CP_TemporalThlY11,
	CP_TemporalThlY10,
	CP_TemporalThlY9,
	CP_TemporalThlY8,
	CP_TemporalThlY7,
	CP_TemporalThlY6,
	CP_TemporalThlY5,
	CP_TemporalThlY4,
	CP_TemporalThlY3,
	CP_TemporalThlY2,
	CP_TemporalThlY1,
	CP_TemporalThlY0,

	RTNR_Sobel_Method,
	RTNR_Sad_Method,
	RTNR_Sad_Mix_Cal_Weight,
	Rtnr_Moredetail_Cotinue_En,
	Sad_Edge_Thl6,
	Sad_Edge_Thl5,
	Sad_Edge_Thl4,
	Sad_Edge_Thl3,
	Sad_Edge_Thl2,
	Sad_Edge_Thl1,
	Sad_Edge_Thl0,
	Sad_NonEdge_Thl6,
	Sad_NonEdge_Thl5,
	Sad_NonEdge_Thl4,
	Sad_NonEdge_Thl3,
	Sad_NonEdge_Thl2,
	Sad_NonEdge_Thl1,
	Sad_NonEdge_Thl0,
	Staticstic_Count_Thl6,
	Staticstic_Count_Thl5,
	Staticstic_Count_Thl4,
	Staticstic_Count_Thl3,
	Staticstic_Count_Thl2,
	Staticstic_Count_Thl1,
	Staticstic_Count_Thl0,
	RTNR_Statistic_Motion_SadTh3,
	RTNR_Statistic_Motion_SadTh2,
	RTNR_Statistic_Motion_SadTh1,
	Edge_Type_Diff_Thl6,
	Edge_Type_Diff_Thl5,
	Edge_Type_Diff_Thl4,
	Edge_Type_Diff_Thl3,
	Edge_Type_Diff_Thl2,
	Edge_Type_Diff_Thl1,
	Edge_Type_Diff_Thl0,

	RTNR_SNR_En,
	RTNR_SNR_Sobel_1_En,
	RTNR_SNR_Sobel_2_En,
	RTNR_SNR_Sobel_3_En,
	RTNR_SNR_Mask,

	RTNR_SNR_Edgestatus_Invert,
	RTNR_SNR_Motion_Invert,
	RTNR_SNR_Offset_Invert,

	RTNR_SNR_Edgestatus_Weight,
	RTNR_SNR_Motion_Weight,
	RTNR_SNR_Offset,

	RTNR_SNR_Edgestatus_Th6,
	RTNR_SNR_Edgestatus_Th5,
	RTNR_SNR_Edgestatus_Th4,
	RTNR_SNR_Edgestatus_Th3,
	RTNR_SNR_Edgestatus_Th2,
	RTNR_SNR_Edgestatus_Th1,
	RTNR_SNR_Edgestatus_Th0,
/*RTNR_C*/
	CP_RTNR_C_Enable,
	CP_TemporalThlC6,
	CP_TemporalThlC5,
	CP_TemporalThlC4,
	CP_TemporalThlC3,
	CP_TemporalThlC2,
	CP_TemporalThlC1,
	CP_TemporalThlC0,
/*EDGE*/
	CP_Temporal_edge_crct_en,
/*SPNR_Y*/
	CP_SResultWeight,
	CP_ZoranFilterSizeY,
	CP_SpatialEnableY,
	CP_SpatialThlYx4,
	CP_SpatialThlYx2,
	CP_SpatialThlY,
	CP_FixedWeight1Y,
	CP_FixedWeight2Y,
	CP_Weight1Y,
	CP_Weight2Y,
	CP_En_Sw_Edge_Thl,
	CurvemappingMode_Enable,
	CurvemappingMode_Weight10,
	CurvemappingMode_Weight11,
	CurvemappingMode_Weight12,
	CurvemappingMode_Weight13,
	CurvemappingMode_Weight14,
	CurvemappingMode_Weight15,
	CurvemappingMode_Weight16,
	CurvemappingMode_Weight17,
	CurvemappingMode_Weight18,
	CurvemappingMode_Weight20,
	CurvemappingMode_Weight21,
	CurvemappingMode_Weight22,
	CurvemappingMode_Weight23,
	CurvemappingMode_Weight24,
	CurvemappingMode_Weight25,
	CurvemappingMode_Weight26,
	CurvemappingMode_Weight27,
	CurvemappingMode_Weight28,
	CurvemappingMode_Step10,
	CurvemappingMode_Step21,
	CurvemappingMode_Step32,
	CurvemappingMode_Step43,
	CurvemappingMode_Step54,
	CurvemappingMode_Step65,
	CurvemappingMode_Step76,
	CurvemappingMode_Step87,
	/*Smoothedgenearby_Enable,*/
	/*Smoothedgenearby_Range,*/
	/*Smoothedgenearby_Thl,*/
	Modified_Lpf_Enable,
	Prelpf_Snr_Sel,
	Vfir_Weight,

/*SPNR_C*/
	CP_ZoranFilterSizeC,
	CP_SpatialEnableC,
	CP_SpatialThlCx4,
	CP_SpatialThlCx2,
	CP_SpatialThlC,
/*Dark K value*/
	CP_Temporal_Y_Kout_Dark_2,
	CP_Temporal_Y_Kin_Dark_2,
	CP_Temporal_Y_Dark_Th,
	CP_Temporal_Y_Kout_Dark,
	CP_Temporal_Y_Kin_Dark,
	CP_Temporal_Y_Dark_K_en,
/*MCNR*/
	CP_Temporal_HMCNR_weight,
	CP_Temporal_HMCNR_ref_topline_th,
	/*CP_Temporal_HMCNR_debug_progressive,*/
	CP_Temporal_HMCNR_ref_topline_en,
	CP_Temporal_HMCNR_uv_with_y,
	CP_Temporal_HMCNR_search_range,

	NR_DNR8_MAX,
} NR_DNR_8;


typedef enum _NR_MPEG_8 {
/*MPEG_H*/
	CP_MpgUpperBound_X = 0,
	CP_MpgDcGain,
	CP_MpgDcQp,
	CP_MpgFilterSel1,
	/*CP_MpgFilterSel2,*/
	CP_MpgIdxMode_X,
	CP_MpgIdx_X,
	CP_MpegResultWeight_X,
	CP_MpegEnable_X,
/*MPEG_V*/
	CP_MpgLowerBound,
	CP_MpgUpperBound,
	CP_MpgLPmode,
	CP_MpgIdx_Y,
	CP_MpegResultWeight_Y,
	CP_MpgIdxMode_Y,
	CP_MpegEnable_Y_C,
	CP_MpegEnable_Y_Y,

	NR_MPEGNR8_MAX,
} NR_MPEG_8;

/*yuan::add Edge_peaking*/
typedef enum _Edge_peaking {
/*Edge peaking table*/
	CP_HV_neg = 0,
	CP_HV_pos,
	CP_Gain_Pos,
	CP_Gain_Neg,
	CP_Coreing,

	Edge_peaking_MAX,
} Edge_peaking;

typedef enum _DNR_ITEM_TYPE {         /* 20130831	rock_rau sync scalerlib.h and scalervip.h to here*/
	DNR_ITEM_SPATIAL_Y = 0,
	DNR_ITEM_SPATIAL_C,
	DNR_ITEM_TEMPORAL,
	DNR_ITEM_RTNR_Y,
	DNR_ITEM_RTNR_C,
	DNR_ITEM_MPEG_H,
	DNR_ITEM_MPEG_V,
	DNR_ITEM_VD_NR,
	DNR_ITEM_EDGE,
	DNR_ITEM_Dark_K,
	DNR_ITEM_MCNR,
	DNR_ITEM_peaking,
	DNR_ITEM_Reserved2,
	DNR_ITEM_MAX,
} DNR_ITEM_TYPE;

typedef enum _DNR_SELECT {
	DNR_SELECT_OFF = 0,
	DNR_SELECT_LOW,
	DNR_SELECT_MIDDLE,
	DNR_SELECT_HIGH,
	DNR_SELECT_NR_TABLE,
    DNR_SELECT_MAX,
} DNR_SELECT;

typedef enum _DCC_SKIN_TONE_TABLE {
	DCC_SKIN_TONE_TABLE_ENABLE = 0,
	DCC_SKIN_TONE_TABLE_MODE,
	DCC_SKIN_TONE_TABLE_Y_CENTER,
	DCC_SKIN_TONE_TABLE_U_CENTER,
	DCC_SKIN_TONE_TABLE_V_CENTER,
	DCC_SKIN_TONE_TABLE_Y_RANGE,
	DCC_SKIN_TONE_TABLE_U_RANGE,
	DCC_SKIN_TONE_TABLE_V_RANGE,
	DCC_SKIN_TONE_TABLE_SEG0,
	DCC_SKIN_TONE_TABLE_SEG1,
	DCC_SKIN_TONE_TABLE_SEG2,
	DCC_SKIN_TONE_TABLE_SEG3,
	DCC_SKIN_TONE_TABLE_SEG4,
	DCC_SKIN_TONE_TABLE_SEG5,
	DCC_SKIN_TONE_TABLE_SEG6,
	DCC_SKIN_TONE_TABLE_SEG7,
	DCC_SKIN_TONE_TABLE_MAX,
} DCC_SKIN_TONE_TABLE;

typedef enum _INTELLIGENT_PICTURE_2D_PEAKING_TABLE {
	INTELLIGENT_PICTURE_TABLE_TEXTURE_POS_GAIN = 0,
	INTELLIGENT_PICTURE_TABLE_TEXTURE_NEG_GAIN,
	INTELLIGENT_PICTURE_TABLE_TEXTURE_CORING,
	INTELLIGENT_PICTURE_TABLE_EDGE_POS_GAIN,
	INTELLIGENT_PICTURE_TABLE_EDGE_NEG_GAIN,
	INTELLIGENT_PICTURE_TABLE_EDGE_CORING,
	INTELLIGENT_PICTURE_TABLE_VERTICAL_POS_GAIN,
	INTELLIGENT_PICTURE_TABLE_VERTICAL_NEG_GAIN,
	INTELLIGENT_PICTURE_TABLE_VERTICAL_CORING,
	INTELLIGENT_PICTURE_ENABLE,
	INTELLIGENT_PICTURE_OPTIMISATION,
	INTELLIGENT_PICTURE_TABLE_MAX,
} INTELLIGENT_PICTURE_2D_PEAKING_TABLE;

#define AUTOMA_TABLE_MAX	5
#define AUTOMA_ITEM_MAX	255
#define AUTOMA_LEVEL_MAX	20

#define DNR_T0    	(NR_INPUT8_MAX+NR_DNR8_MAX*0)
#define DNR_T1    	(NR_INPUT8_MAX+NR_DNR8_MAX*1)
#define DNR_T2    	(NR_INPUT8_MAX+NR_DNR8_MAX*2)
#define DNR_T3    	(NR_INPUT8_MAX+NR_DNR8_MAX*3)
#define DNR_T4    	(NR_INPUT8_MAX+NR_DNR8_MAX*4)


#define MPEGNR_T0    	(NR_INPUT8_MAX+NR_DNR8_MAX*5+NR_MPEGNR8_MAX*0)
#define MPEGNR_T1    	(NR_INPUT8_MAX+NR_DNR8_MAX*5+NR_MPEGNR8_MAX*1)
#define MPEGNR_T2    	(NR_INPUT8_MAX+NR_DNR8_MAX*5+NR_MPEGNR8_MAX*2)
#define MPEGNR_T3    	(NR_INPUT8_MAX+NR_DNR8_MAX*5+NR_MPEGNR8_MAX*3)
#define MPEGNR_T4    	(NR_INPUT8_MAX+NR_DNR8_MAX*5+NR_MPEGNR8_MAX*4)
#define MPEGNR_T5    	(NR_INPUT8_MAX+NR_DNR8_MAX*5+NR_MPEGNR8_MAX*5)
#define MPEGNR_T6    	(NR_INPUT8_MAX+NR_DNR8_MAX*5+NR_MPEGNR8_MAX*6)
#define MPEGNR_T7    	(NR_INPUT8_MAX+NR_DNR8_MAX*5+NR_MPEGNR8_MAX*7)
#define MPEGNR_T8    	(NR_INPUT8_MAX+NR_DNR8_MAX*5+NR_MPEGNR8_MAX*8)
#define MPEGNR_T9    	(NR_INPUT8_MAX+NR_DNR8_MAX*5+NR_MPEGNR8_MAX*9)
#define NR_TABLE8_MAX	(NR_INPUT8_MAX+NR_DNR8_MAX*5+NR_MPEGNR8_MAX*10)

#define NR_LEVEL_MAX    	15

#define VIP_QUALITY_TOTAL_SOURCE_NUM VIP_QUALITY_SOURCE_NUM
#define VIP_DCL_TBL    	5

#define VIP_QUALITY_FUNCTION_TOTAL_CHECK 	21
#define VIP_QUALITY_3DFUNCTION_TOTAL_CHECK 	21

#define VIP_QUALITY_D_PEAKING_NUM 19
#define VIP_QUALITY_I_EDGE_NUM 6

#define    d_nLV_min              		(17)
#define    d_nLV_max              		(18)

#define SU_PK_COEF_MAX	10

#define t_SR_init_ROW_MAX	5
#define t_SR_init_COL_MAX	10

#define t_SU_PK_Coeff_by_timing_ROW_MAX	11
#define t_SU_PK_Coeff_by_timing_COL_MAX	6

#define t_ODtable_Coeff_ROW_MAX	17
#define t_ODtable_Coeff_COL_MAX	17

#define t_DCR_TABLE_ROW_MAX	5
#define t_DCR_TABLE_COL_MAX	5
#define Tone_Mapping_Table_MAX  5
#define Tone_Mapping_size 	129
#define t_MA_Chroma_Error_MAX	11

#define t_I_EDGE_Smooth_Coef_MAX	10

#define t_xvYcc_sRGB_ROW_MAX	9
#define t_xvYcc_sRGB_COL_MAX	129

#define t_VD_ConBriHueSat_MAX	20
#define t_GAMMA_MAX	10
#define t_outputGAMMA_MAX	3


#define Auto_Function_Array_MAX	20

#define Sharp_table_num 20
#define MBPK_table_num 20
#define MBPK_row_num 20
#define MBSU_table_num 20
#define DI_col_number 21
#define D_DLTI_table_num 20

/*mac3*/
#if 1
#define VIP_ICM_TBL_X 10
#define VIP_ICM_TBL_Y 441 /*12(itn) x 12(sat) x 3(h/s/i) + 2(global settings) = 434+7 (rough)=441*/
#define VIP_ICM_TBL_Z 64
#else
#define VIP_ICM_TBL_X	30
#define VIP_ICM_TBL_Y	92
#define VIP_ICM_TBL_Z	28/*13 */ /*leonard wu 20111216*/
#endif

#define VIP_QUALITY_I_PEAKING_NUM 15
#define VIP_QUALITY_D_EGSM_NUM 17
#define DCR_TABLE_ROW_NUM    6
#define DCR_TABLE_COL_NUM    5
#define New_DCR_TABLE_NUM    3
#define Sharp_row_num 15
#define Sharp_col_num 21



/* VIP_QUALITY_Coef 0~20 (21/21)*/
/*<FUNCTION0_ITEMS_START>*/
#define VIP_QUALITY_FUNCTION_DLTi 					0
#define VIP_QUALITY_FUNCTION_DCTi 					1
#define VIP_QUALITY_FUNCTION_I_DLTi					2
#define VIP_QUALITY_FUNCTION_I_DCTi					3
#define VIP_QUALITY_FUNCTION_V_DCTi					4
#define VIP_QUALITY_FUNCTION_UV_DELAY_ENABLE		5
#define VIP_QUALITY_FUNCTION_UV_DELAY				6
#define VIP_QUALITY_FUNCTION_YUV2RGB 				7
#define VIP_QUALITY_FUNCTION_Film 					8
#define VIP_QUALITY_FUNCTION_Intra 					9
#define VIP_QUALITY_FUNCTION_MA 					10
#define VIP_QUALITY_FUNCTION_tnr_xc 				11
#define VIP_QUALITY_FUNCTION_MA_Chroma_Error		12
// chen 170522
//#define VIP_QUALITY_FUNCTION_RTNR_NM 				13
// end chen 170522
#define VIP_QUALITY_FUNCTION_MADI_HME 				14
#define VIP_QUALITY_FUNCTION_MADI_HMC 				15
#define VIP_QUALITY_FUNCTION_MADI_PAN 				16
#define VIP_QUALITY_DI_SMD_Level					17
#define VIP_QUALITY_COLOR_NEW_UVC					18
#define VIP_QUALITY_FUNCTION_UV_DELAY_TOP				19
#define VIP_QUALITY_FUNCTION_V_DCTi_LPF				20
/*<FUNCTION0_ITEMS_END>*/


/* VIP_QUALITY_Extend_Coef1 0~20 (11/21)*/
/*<FUNCTION1_ITEMS_START>*/
#define VIP_QUALITY_FUNCTION_CDS_Table					0
#define VIP_QUALITY_FUNCTION_EMFMk2						1
#define VIP_QUALITY_FUNCTION_SKIPIR_Dering				2
#define VIP_QUALITY_FUNCTION_SKIPIR_dnoise				3
#define VIP_QUALITY_FUNCTION_SKIPIR_dnoise_table		4
#define VIP_QUALITY_FUNCTION_ADAPTIVE_GAMMA				5
#define VIP_QUALITY_FUNCTION_LD_Enable					6
#define VIP_QUALITY_FUNCTION_LD_Table 					7
#define VIP_QUALITY_FUNCTION_LC_Enable					8
#define VIP_QUALITY_FUNCTION_LC_Table 					9
#define VIP_QUALITY_FUNCTION_3dLUT_Table 				10
#define VIP_QUALITY_FUNCTION_I_De_XC					11
#define VIP_QUALITY_FUNCTION_I_De_Contour					12
#define VIP_QUALITY_FUNCTION_SLD                                       13
#define VIP_QUALITY_FUNCTION_Output_InvOutput_GAMMA					14
#define  VIP_QUALITY_FUNCTION_Tone_Mapping_Table                              15
/*<FUNCTION1_ITEMS_END>*/
/*#define VIP_QUALITY_FUNCTION_dummy 					16*/
/*#define VIP_QUALITY_FUNCTION_dummy					17*/
/*#define VIP_QUALITY_FUNCTION_dummy					18*/
/*#define VIP_QUALITY_FUNCTION_dummy 					19*/
/*#define VIP_QUALITY_FUNCTION_dummy					20*/



/* VIP_QUALITY_Extend2_Coef 0~20 (21/21)*/
/*<FUNCTION2_ITEMS_START>*/
#define VIP_QUALITY_FUNCTION_SU_H 								0
#define VIP_QUALITY_FUNCTION_SU_V 								1
#define VIP_QUALITY_FUNCTION_S_PK 								2
#define VIP_QUALITY_FUNCTION_SUPK_MASK							3
#define VIP_QUALITY_FUNCTION_UNSHARP_MASK						4
#define VIP_QUALITY_FUNCTION_iESM_table		     				6
#define VIP_QUALITY_FUNCTION_TwoStep_SU_Table			7
#define VIP_QUALITY_FUNCTION_SuperResolution_edge_gain			8
#define VIP_QUALITY_FUNCTION_SNR_SuperResolution_texture_gain	9
#define VIP_QUALITY_FUNCTION_SD_H_Table							10
#define VIP_QUALITY_FUNCTION_SD_V_Table							11
#define VIP_QUALITY_FUNCTION_SD_444To422						12
#define VIP_QUALITY_FUNCTION_ColorSpace_Control 				13
#define VIP_QUALITY_FUNCTION_SU_H_12TAP							14
#define VIP_QUALITY_FUNCTION_SU_V_8TAP							15
#define VIP_QUALITY_FUNCTION_OSD_SHP							16
#define VIP_QUALITY_FUNCTION_SU_DIR								17
#define VIP_QUALITY_FUNCTION_SU_DIR_Weighting					18
#define VIP_QUALITY_FUNCTION_SU_C_H 								19
#define VIP_QUALITY_FUNCTION_SU_C_V 								20
/*<FUNCTION2_ITEMS_END>*/



/* VIP_QUALITY_Extend3_Coef 0~20 (16/21)*/
/*<FUNCTION3_ITEMS_START>*/
#define VIP_QUALITY_FUNCTION_VD_ConBriHueSat_Table 		0
#define VIP_QUALITY_FUNCTION_ICM						1
#define VIP_QUALITY_FUNCTION_Gamma 						2
#define VIP_QUALITY_FUNCTION_S_Gamma_Index				3
#define VIP_QUALITY_FUNCTION_S_Gamma_Low				4
#define VIP_QUALITY_FUNCTION_S_Gamma_High				5
#define VIP_QUALITY_FUNCTION_DCC_Table_Select			6
#define VIP_QUALITY_FUNCTION_DCC_Skin_Tone_Table_Select	7
#define VIP_QUALITY_FUNCTION_DCC_Chroma_comp_t_sel		8
#define VIP_QUALITY_FUNCTION_SharpTable 				9
#define VIP_QUALITY_FUNCTION_PQA_Table					10
#define VIP_QUALITY_FUNCTION_PQA_Input_Table			11
#define VIP_QUALITY_FUNCTION_MB_Peaking			12
#define VIP_QUALITY_FUNCTION_blue_stretch		13
#define VIP_QUALITY_FUNCTION_HDR			14
#define VIP_QUALITY_FUNCTION_DCC_ADV_API_tab_sel		15
#define VIP_QUALITY_FUNCTION_ICM_Adjust			16
#define VIP_QUALITY_FUNCTION_RGB_SharpTable				17
/*#define VIP_QUALITY_FUNCTION_dummy 			18*/
/*#define VIP_QUALITY_FUNCTION_dummy			19*/
#define VIP_QUALITY_FUNCTION_MB_SU_Peaking			20
/*<FUNCTION3_ITEMS_END>*/




/*young 3Dtable*/
/*VIP_QUALITY_3Dmode_Coef 0~14(15/15)*/
/*<FUNCTION3D_ITEMS_START>*/
#define VIP_QUALITY_3DFUNCTION_S_Gamma_Index	0
#define VIP_QUALITY_3DFUNCTION_S_Gamma_High		1
#define VIP_QUALITY_3DFUNCTION_S_Gamma_Low		2
#define VIP_QUALITY_3DFUNCTION_S_PK				3
#define VIP_QUALITY_3DFUNCTION_SUPK_MASK		4
#define VIP_QUALITY_3DFUNCTION_UNSHARP_MASK		5
#define VIP_QUALITY_3DFUNCTION_Sharpness_table	6
#define VIP_QUALITY_3DFUNCTION_DCC_Table_Select	7
#define VIP_QUALITY_3DFUNCTION_Dummy1			8
#define VIP_QUALITY_3DFUNCTION_Dummy2			9
#define VIP_QUALITY_3DFUNCTION_NRSPAT			10
#define VIP_QUALITY_3DFUNCTION_rtnr_y			11
#define VIP_QUALITY_3DFUNCTION_rtnr_c			12
#define VIP_QUALITY_3DFUNCTION_ICM				13
#define VIP_QUALITY_3DFUNCTION_iESM_table       14
#define VIP_QUALITY_3DFUNCTION_SHP1D2D_EGSM     15
/*<FUNCTION3D_ITEMS_END>*/


#ifdef BUILD_QUICK_SHOW /*for quick_show mode */

/* QS_QUALITY_Coef_scal 0~20 (16/21)*/
#define QS_SU_H 				0
#define QS_SU_V 				1
#define QS_S_PK 				2
#define QS_SUPK_MASK				3
#define QS_MASK					4
#define QS_iESM_table		     		6
#define QS_TwoStep_SU_Table			7
#define QS_SuperResolution_edge_gain		8
#define QS_SNR_SuperResolution_texture_gain	9
#define QS_SD_H_Table				10
#define QS_SD_V_Table				11
#define QS_SD_444To422				12
#define QS_ColorSpace_Control 			13
#define QS_SU_H_12TAP				14
#define QS_SU_V_8TAP				15
#define QS_OSD_SHP				16
#define QS_SU_DIR				17
#define QS_SU_DIR_Weighting			18
#define QS_SU_C_H 				19
#define QS_SU_C_V 				20


#endif /* BUILD_QUICK_SHOW */




#define YUV2RGB_TABLE_SIZE 68

#define CALLED_BY_OSD 1
#define CALLED_NOT_BY_OSD 0

#define VIP_TABLE_PICTURE_MODE_MAX  15/*For VIP Table Structure : Size defined*/
#define VIP_TABLE_TIMIMG_MAX	   10/*For VIP Table Structure : Size defined*/

typedef enum _VIP_Customer_Project_ID_ENUM {
	VIP_Project_ID_TV001 = 0, /*default*/
	VIP_Project_ID_TV002 ,
	VIP_Project_ID_TV003 ,
	VIP_Project_ID_TV006 ,
	VIP_Project_ID_TV010 ,
	VIP_Project_ID_TV013 ,
	VIP_Project_ID_TV015 ,
	VIP_Project_ID_TV030 ,
	VIP_Project_ID_TV035 ,
	VIP_Project_ID_TV043 ,
	VIP_Project_ID_TV005 ,
	VIP_Project_ID_TV051 ,
	VIP_Project_ID_MAX,
} VIP_Customer_Project_ID_ENUM;



typedef enum _DCC_Histogram_gain_ENUM {

	DCC_Histogram_gain_bin0 = 0,
	DCC_Histogram_gain_bin1,
	DCC_Histogram_gain_bin2,
	DCC_Histogram_gain_bin3,
	DCC_Histogram_gain_bin4,
	DCC_Histogram_gain_bin5,
	DCC_Histogram_gain_bin6,
	DCC_Histogram_gain_bin7,
	DCC_Histogram_gain_bin8,
	DCC_Histogram_gain_bin9,
	DCC_Histogram_gain_bin10,
	DCC_Histogram_gain_bin11,
	DCC_Histogram_gain_bin12,
	DCC_Histogram_gain_bin13,
	DCC_Histogram_gain_bin14,
	DCC_Histogram_gain_bin15,
	DCC_Histogram_gain_bin16,
	DCC_Histogram_gain_bin17,
	DCC_Histogram_gain_bin18,
	DCC_Histogram_gain_bin19,
	DCC_Histogram_gain_bin20,
	DCC_Histogram_gain_bin21,
	DCC_Histogram_gain_bin22,
	DCC_Histogram_gain_bin23,
	DCC_Histogram_gain_bin24,
	DCC_Histogram_gain_bin25,
	DCC_Histogram_gain_bin26,
	DCC_Histogram_gain_bin27,
	DCC_Histogram_gain_bin28,
	DCC_Histogram_gain_bin29,
	DCC_Histogram_gain_bin30,
	DCC_Histogram_gain_bin31,
	DCC_Histogram_gain_bin_max,


} DCC_Histogram_gain_ENUM;

typedef enum _DCC_Histogram_offset_ENUM {

	DCC_Histogram_offset_bin0,
	DCC_Histogram_offset_bin1,
	DCC_Histogram_offset_bin2,
	DCC_Histogram_offset_bin3,
	DCC_Histogram_offset_bin4,
	DCC_Histogram_offset_bin5,
	DCC_Histogram_offset_bin6,
	DCC_Histogram_offset_bin7,
	DCC_Histogram_offset_bin8,
	DCC_Histogram_offset_bin9,
	DCC_Histogram_offset_bin10,
	DCC_Histogram_offset_bin11,
	DCC_Histogram_offset_bin12,
	DCC_Histogram_offset_bin13,
	DCC_Histogram_offset_bin14,
	DCC_Histogram_offset_bin15,
	DCC_Histogram_offset_bin16,
	DCC_Histogram_offset_bin17,
	DCC_Histogram_offset_bin18,
	DCC_Histogram_offset_bin19,
	DCC_Histogram_offset_bin20,
	DCC_Histogram_offset_bin21,
	DCC_Histogram_offset_bin22,
	DCC_Histogram_offset_bin23,
	DCC_Histogram_offset_bin24,
	DCC_Histogram_offset_bin25,
	DCC_Histogram_offset_bin26,
	DCC_Histogram_offset_bin27,
	DCC_Histogram_offset_bin28,
	DCC_Histogram_offset_bin29,
	DCC_Histogram_offset_bin30,
	DCC_Histogram_offset_bin31,
	DCC_Histogram_offset_bin_max,

} DCC_Histogram_offset_ENUM;

typedef enum _DCC_Histogram_limit_ENUM {

	DCC_Histogram_limit_bin0 = 0,
	DCC_Histogram_limit_bin1,
	DCC_Histogram_limit_bin2,
	DCC_Histogram_limit_bin3,
	DCC_Histogram_limit_bin4,
	DCC_Histogram_limit_bin5,
	DCC_Histogram_limit_bin6,
	DCC_Histogram_limit_bin7,
	DCC_Histogram_limit_bin8,
	DCC_Histogram_limit_bin9,
	DCC_Histogram_limit_bin10,
	DCC_Histogram_limit_bin11,
	DCC_Histogram_limit_bin12,
	DCC_Histogram_limit_bin13,
	DCC_Histogram_limit_bin14,
	DCC_Histogram_limit_bin15,
	DCC_Histogram_limit_bin16,
	DCC_Histogram_limit_bin17,
	DCC_Histogram_limit_bin18,
	DCC_Histogram_limit_bin19,
	DCC_Histogram_limit_bin20,
	DCC_Histogram_limit_bin21,
	DCC_Histogram_limit_bin22,
	DCC_Histogram_limit_bin23,
	DCC_Histogram_limit_bin24,
	DCC_Histogram_limit_bin25,
	DCC_Histogram_limit_bin26,
	DCC_Histogram_limit_bin27,
	DCC_Histogram_limit_bin28,
	DCC_Histogram_limit_bin29,
	DCC_Histogram_limit_bin30,
	DCC_Histogram_limit_bin31,
	DCC_Histogram_limit_bin_max,

} DCC_Histogram_limit_ENUM;

typedef enum _Chroma_Enhance_offset_ENUM{

	Chroma_Enhance_offset0=0,
	Chroma_Enhance_offset1,
	Chroma_Enhance_offset2,
	Chroma_Enhance_offset3,
	Chroma_Enhance_offset4,
	Chroma_Enhance_offset5,
	Chroma_Enhance_offset6,
	Chroma_Enhance_offset7,
	Chroma_Enhance_offset8,
	Chroma_Enhance_offset9,
	Chroma_Enhance_offset10,
	Chroma_Enhance_offset11,
	Chroma_Enhance_offset12,
	Chroma_Enhance_offset13,
	Chroma_Enhance_offset14,
	Chroma_Enhance_offset15,
	Chroma_Enhance_offset16,
	Chroma_Enhance_offset17,
	Chroma_Enhance_offset18,
	Chroma_Enhance_offset19,
	Chroma_Enhance_offset20,
	Chroma_Enhance_offset21,
	Chroma_Enhance_offset22,
	Chroma_Enhance_offset23,
	Chroma_Enhance_offset24,
	Chroma_Enhance_offset25,
	Chroma_Enhance_offset26,
	Chroma_Enhance_offset27,
	Chroma_Enhance_offset28,
	Chroma_Enhance_offset29,
	Chroma_Enhance_offset30,
	Chroma_Enhance_offset31,
	Chroma_Enhance_offset_max,

} Chroma_Enhance_offset_ENUM;

typedef enum _Chroma_Enhance_Gain_ENUM{

	Chroma_Enhance_Gain0=0,
	Chroma_Enhance_Gain1,
	Chroma_Enhance_Gain2,
	Chroma_Enhance_Gain3,
	Chroma_Enhance_Gain4,
	Chroma_Enhance_Gain5,
	Chroma_Enhance_Gain6,
	Chroma_Enhance_Gain7,
	Chroma_Enhance_Gain8,
	Chroma_Enhance_Gain9,
	Chroma_Enhance_Gain10,
	Chroma_Enhance_Gain11,
	Chroma_Enhance_Gain12,
	Chroma_Enhance_Gain13,
	Chroma_Enhance_Gain14,
	Chroma_Enhance_Gain15,
	Chroma_Enhance_Gain16,
	Chroma_Enhance_Gain17,
	Chroma_Enhance_Gain18,
	Chroma_Enhance_Gain19,
	Chroma_Enhance_Gain20,
	Chroma_Enhance_Gain21,
	Chroma_Enhance_Gain22,
	Chroma_Enhance_Gain23,
	Chroma_Enhance_Gain24,
	Chroma_Enhance_Gain25,
	Chroma_Enhance_Gain26,
	Chroma_Enhance_Gain27,
	Chroma_Enhance_Gain28,
	Chroma_Enhance_Gain29,
	Chroma_Enhance_Gain30,
	Chroma_Enhance_Gain31,
	Chroma_Enhance_Gain_max,

} Chroma_Enhance_Gain_ENUM;

typedef enum _VIP_TABLE_COLORSTD_ENUM {
	VIP_COLORSTD_DEFAULT = 0,
	VIP_PICTURE_MODE_MAX,
} VIP_TABLE_COLORSTD_ENUM;

typedef enum _VIP_TABLE_TIMIMG_ENUM {/*for VIP table OSD timing defined*/
	VIP_TIMIMG_DEFAULT = 0,
	VIP_TIMIMG_VD_NTSC,
	VIP_TIMIMG_VD_PAL,
	VIP_TIMIMG_VD_SECAM,
    VIP_TIMIMG_SD,
    VIP_TIMIMG_HD,
    VIP_TIMIMG_4K2K,
    VIP_TIMIMG_HDR_4K2K,
    VIP_TIMIMG_DolbyHDR_4K2K,
    VIP_TIMIMG_MAX,
} VIP_TABLE_TIMIMG_ENUM;

/* D_Sharpness start */
typedef struct {
	short Peaking_Enable;
	short Edge_Mode;
	short Sobel_Mode;
	short Edgextrange;
	short Sobel_Upbnd;
	short Sobel_Stepbit;
	short Zdiff_Upbnd;
	short Zdiff_Stepbit;
	short Zigsftbit_Reg;
	short Tex_En;
	short Tex_Upbnd;
	short Tex_Stepbit;
	short Actsftbit_Reg;
	short tex_flat_filter_en;/*Merlin2*/
	short Ring_Tune;/*Removed since Macarthur4, Merlin2*/
	short Ring_Gain;/*Removed since Macarthur4, Merlin2*/
	short Ring_Range;/*Removed since Macarthur4, Merlin2*/
	short Lpf_Mode;/*Removed since Macarthur4, Merlin2*/
	short Lpf_Filter;/*Removed since Macarthur4, Merlin2*/
	short Reserved19;/*magellan2*/ /*short Mkiii_En;*/
	short Tex_Filter_Sel;

	short Hpf_Vpf_Maxmin_En;/*magellan2*/
	short Src_Sel;/*magellan2*/
	short sobelrange_h;/*merlin*/
	short sobelrange_v;/*merlin*/
	short acts_offset;/*merlin*/
	short small_th1;/*merlin*/
	short edgextrange_v;/*merlin*/
	short Reserved28;/**/
	short Reserved29;/**/
	short Reserved30;/**/

} VIP_2D_Peak_AdaptCtrl; /*ROW 000*/

typedef struct {
	short Edge_Filter_Sel;// 0:HPF 1:1-LPF
	short Edg_Tex_Blending_Lpf_En;
	short Edg_Tex_Blending_Lpf_Filter;
	short PkDlti_comsign;
	short edg_dlti;
	short zdiffl_en;
	short zdiffl_lowbnd;
	short zdiffl_stepbit;
	short edg_dlti_tex;// 2020
	short Reserved9;
	short edge_mono_en;
	short edge_mono_stepbit;
	short edge_mono_stepbiu;
	short edge_mono_upbnd;
	short edge_mono_lowbnd;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
	short Reserved31;/* */
} VIP_2D_Peak_AdaptCtrl2; /*ROW 001*/

typedef struct {
	short G_Pos_Min;
	short G_Pos_Max;
	short G_Neg_Min;
	short G_Neg_Max;
	short HV_POS_Min;
	short HV_POS_Max;
	short HV_NEG_Min;
	short HV_NEG_Max;
	short LV_Min;
	short LV_Max;
	short Edg_D2_Shift_Bit;
	short G_Pos2;
	short G_Neg2;
	short LV2;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Edge_Shp_set; /*ROW 002*/

typedef struct {
	short C00;
	short C10;
	short C20;
	short C30;
	short C40;
	short C50;
	short C60;/*magellan2*/
	short C70;/*magellan2*/
	short C80;/*magellan2*/
	short C90;/*magellan2*/
	short C100;/*magellan2*/
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Edge_Shp_coef1; /*ROW 003*/

typedef struct {
	short C01;
	short Reserved1;
	short Reserved2;
	short Reserved3;
	short Reserved4;
	short Reserved5;
	short Reserved6;
	short Reserved7;
	short Reserved8;
	short Reserved9;
	short Reserved10;
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Edge_Shp_coef2; /*ROW 004*/

typedef struct {
	short C02;
	short Reserved1;
	short Reserved2;
	short Reserved3;
	short Reserved4;
	short Reserved5;
	short Reserved6;
	short Reserved7;
	short Reserved8;
	short Reserved9;
	short Reserved10;
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Edge_Shp_coef3; /*ROW 005*/

typedef struct {
	short C03;
	short Reserved1;
	short Reserved2;
	short Reserved3;
	short Reserved4;
	short Reserved5;
	short Reserved6;
	short Reserved7;
	short Reserved8;
	short Reserved9;
	short Reserved10;
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Edge_Shp_coef4; /*ROW 006*/

typedef struct {
	short C04;
	short Reserved1;
	short Reserved2;
	short Reserved3;
	short Reserved4;
	short Reserved5;
	short Reserved6;
	short Reserved7;
	short Reserved8;
	short Reserved9;
	short Reserved10;
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Edge_Shp_coef5; /*ROW 007*/

typedef struct {
	short EMF_En;
	short EMF_Shift;
	short EMF_Range;
	short Seg_0_Gain;
	short Seg_1_Gain;
	short Seg_2_Gain;
	short Seg_0_Offset;
	short Seg_1_Offset;
	short Seg_2_Offset;
	short EMF_fixentent_pos;
	short EMF_fixentent_neg;
	short Seg_0_x;
	short Seg_1_x;
	short Blend_wt;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Edge_EMF_Table; /*ROW 010*/


typedef struct {
	short G_Pos_Min;
	short G_Pos_Max;
	short G_Neg_Min;
	short G_Neg_Max;
	short HV_POS_Min;
	short HV_POS_Max;
	short HV_NEG_Min;
	short HV_NEG_Max;
	short LV_Min;
	short LV_Max;
	short Edg_D2_Shift_Bit;
	short G_Pos2;
	short G_Neg2;
	short LV2;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;
	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Texture_Shp_set; /*ROW 011*/

typedef struct {
	short C00;
	short C10;
	short C20;
	short C30;
	short C40;
	short C50;
	short C60;/*magellan2*/
	short C70;/*magellan2*/
	short C80;/*magellan2*/
	short C90;/*magellan2*/
	short C100;/*magellan2*/
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Texture_Shp_coef1; /*ROW 012*/

typedef struct {
	short C01;
	short Reserved1;
	short Reserved2;
	short Reserved3;
	short Reserved4;
	short Reserved5;
	short Reserved6;
	short Reserved7;
	short Reserved8;
	short Reserved9;
	short Reserved10;
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Texture_Shp_coef2; /*ROW 013*/

typedef struct {
	short C02;
	short Reserved1;
	short Reserved2;
	short Reserved3;
	short Reserved4;
	short Reserved5;
	short Reserved6;
	short Reserved7;
	short Reserved8;
	short Reserved9;
	short Reserved10;
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Texture_Shp_coef3; /*ROW 014*/

typedef struct {
	short C03;
	short Reserved1;
	short Reserved2;
	short Reserved3;
	short Reserved4;
	short Reserved5;
	short Reserved6;
	short Reserved7;
	short Reserved8;
	short Reserved9;
	short Reserved10;
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Texture_Shp_coef4; /*ROW 015*/

typedef struct {
	short C04;
	short Reserved1;
	short Reserved2;
	short Reserved3;
	short Reserved4;
	short Reserved5;
	short Reserved6;
	short Reserved7;
	short Reserved8;
	short Reserved9;
	short Reserved10;
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Texture_Shp_coef5; /*ROW 016*/

typedef struct {
	short EMF_En;
	short EMF_Shift;
	short EMF_Range;
	short Seg_0_Gain;
	short Seg_1_Gain;
	short Seg_2_Gain;
	short Seg_0_Offset;
	short Seg_1_Offset;
	short Seg_2_Offset;
	short EMF_fixentent_pos;
	short EMF_fixentent_neg;
	short Seg_0_x;
	short Seg_1_x;
	short Blend_wt;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Texture_EMF_Table; /*ROW 019*/

typedef struct {
	short C0;
	short C1;
	short C2;
	short C3;/*magellan2*/
	short C4;/*magellan2*/
	short G_Pos_Min;
	short G_Pos_Max;
	short G_Neg_Min;
	short G_Neg_Max;
	short HV_POS_Min;
	short HV_POS_Max;
	short HV_NEG_Min;
	short HV_NEG_Max;
	short LV_Min;
	short LV_Max;
	short Vext_Reg;
	short Vemf_En;
	short V_Tex_en;
	short V_Dering_En;
	short G_Pos2;
	short G_Neg2;
	short LV2;

	short en_edg;/*magellan2*/
	short en_tex;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short vpk_sr_tex;/*magellan2*/
	short vpk_sr_edg;/*magellan2*/
} VIP_Vertical_Shp_t; /*ROW 020*/

typedef struct {
	short C0;
	short C1;
	short C2;
	short C3;
	short C4;
	short C5;
	short Reserved6;
	short Reserved7;
	short Reserved8;
	short Reserved9;
	short Reserved10;
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_DRV_Mk3_Lpf; /*ROW 023*/

typedef struct {
	short Gain_0;
	short Gain_1;
	short Gain_2;
	short Gain_3;
	short Gain_4;
	short Gain_5;
	short Gain_6;
	short Step_1;
	short Step_2;
	short Step_3;
	short Step_4;
	short Step_5;
	short Step_6;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Max_Min_Gain_En;
	short max_min_gain_tex_en;/*merlin2*/
	short max_min_gain_edg_en;/*merlin2*/

	short max_min_gain_v_en;/*merlin2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Max_Min_Gain; /*ROW 024*/

typedef struct {
	short Weit_0;
	short Weit_1;
	short Weit_2;
	short Weit_3;
	short Weit_4;
	short Weit_5;
	short Weit_6;
	short Step_1;
	short Step_2;
	short Step_3;
	short Step_4;
	short Step_5;
	short Step_6;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Weigh_En;
	short lpf_weigh_tex_en;/*merlin2*/
	short lpf_weigh_edg_en;/*merlin2*/

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Max_Min_Lpf; /*ROW 025*/

typedef struct {
	short Gain_0;
	short Gain_1;
	short Gain_2;
	short Gain_3;
	short Gain_4;
	short Gain_5;
	short Gain_6;
	short Step_1;
	short Step_2;
	short Step_3;
	short Step_4;
	short Step_5;
	short Step_6;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Gain_By_Y_En;
	short gain_by_y_tex_en;/*merlin2*/
	short gain_by_y_edg_en;/*merlin2*/

	short gain_by_y_v_en;/*merlin2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short lv_by_y_enable;/*merlin2*/
	short lv_by_y_tex_enable;/*merlin2*/
	short lv_by_y_edg_enable;/*merlin2*/
	short lv_by_y_v_enable;/*merlin2*/
	short lv_by_y_v_edg_enable;/*merlin2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Gain_By_Y; /*ROW 026*/

typedef struct {
	short Edge_Coring_En;
	short Edge_Gain_En;
	short DeltaEdge_Ulevel;
	short Edge_Coring_Clip;
	short Edge_Gain_Bound;
	
	short Edge_Ser_Range;
	short Bigedge_Ratio;
	short Smalledge_Ratio;
	short Deltaedge_Ratio;
	short Smalledge_Ulevel;
	
	short Edge_Delta_Ext_Range;/*magellan2*/
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	
	short Reserved15;
	short lvd_range;
	short lvd_gaim;
	short lvd_gain;
	short Reserved19;
	
	short Reserved20;
	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Edge_Coring;/*ROW 027*/

typedef struct {
	short Slope_gain_en;
	short dir;
	short FH;
	short Gain_boundPos;
	short Gain_boundNeg;
	short Gain_externdPos;
	short Gain_externdNeg;
	short flat_enable;
	short flat_offset;
	short flat_gain;
	short v_slope_gain_en;/*merlin*/
	short dir_v;/*merlin*/
	short gain_boundpos_v;/*merlin*/
	short gain_boundneg_v;/*merlin*/
	short gain_extendpos_v;/*merlin*/
	short gain_extendneg_v;/*merlin*/
	short flat_v_enable;
	short flat_offset_v;
	short flat_gain_v;
	short Reserved19;
	short Reserved20;

	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_Slope_gain_mask;/*ROW 029*/

typedef struct {
	short Period;/*magellan2*/
	short Gain;/*magellan2*/
	short Offset;/*magellan2*/
	short Ring_Gen_Gain;/*magellan2*/
	short Ring_Gen;/*magellan2*/
	short HV_Max;/*magellan2*/
	short Tex_Filter_Sel;/*magellan2*/
	short Edg_Filter_Sel;/*magellan2*/
	short Reserved8;/*magellan2*/
	short Reserved9;/*magellan2*/
	short Reserved10;/*magellan2*/
	short Reserved11;/*magellan2*/
	short Reserved12;/*magellan2*/
	short detail_tex_en;/*magellan2*/
	short Reserved14;/*magellan2*/
	short detail_v_tex_en;/*magellan2*/
	short Reserved16;/*magellan2*/
	short Reserved17;/*magellan2*/
	short Reserved18;/*magellan2*/
	short Reserved19;/*magellan2*/
	short Reserved20;/*magellan2*/
	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_SR_Control; /*ROW 031*/

typedef struct {
	short C0;/*magellan2*/
	short C1;/*magellan2*/
	short C2;/*magellan2*/
	short C3;/*magellan2*/
	short C4;/*magellan2*/
	short C5;/*magellan2*/
	short C6;/*magellan2*/
	short C7;/*magellan2*/
	short C8;/*magellan2*/
	short C9;/*magellan2*/
	short C10;/*magellan2*/
	short Reserved11;/*magellan2*/
	short Reserved12;/*magellan2*/
	short Reserved13;/*magellan2*/
	short en_edg;/*magellan2*/
	short en_tex;/*magellan2*/
	short Reserved16;/*magellan2*/
	short C0H1;/*magellan2*/
	short C0H2;/*magellan2*/
	short C0H3;/*magellan2*/
	short C0H4;/*magellan2*/
	short Reserved21;/*magellan2*/
	short Reserved22;/*magellan2*/
	short Reserved23;/*magellan2*/
	short Reserved24;/*magellan2*/
	short Reserved25;/*magellan2*/
	short Reserved26;/*magellan2*/
	short Reserved27;/*magellan2*/
	short Reserved28;/*magellan2*/
	short Reserved29;/*magellan2*/
	short Reserved30;/*magellan2*/
} VIP_SR_H21; /*ROW 034*/
#define VIP_SR_H9 VIP_SR_H21
#define VIP_SR_H13 VIP_SR_H21
#define VIP_SR_V9_1 VIP_SR_H21
#define VIP_SR_V9_2 VIP_SR_H21
#define VIP_SR_V9_3 VIP_SR_H21
#define VIP_SR_Ring_H11 VIP_SR_H21
#define VIP_SR_Ring_V9 VIP_SR_H21

typedef struct {
	short enable;/*merlin*/
	short G_Pos_Min;/*merlin*/
	short G_Pos_Max;/*merlin*/
	short G_Neg_Min;/*merlin*/
	short G_Neg_Max;/*merlin*/
	short HV_POS_Min;/*merlin*/
	short HV_POS_Max;/*merlin*/
	short HV_NEG_Min;/*merlin*/
	short HV_NEG_Max;/*merlin*/
	short LV_Min;/*merlin*/
	short LV_Max;/*merlin*/
	short G_Pos2;/*merlin*/
	short G_Neg2;/*merlin*/
	short LV2;/*merlin*/
	short Reserved14;/*merlin*/
	short Reserved15;/*merlin*/
	short Reserved16;/*merlin*/
	short Reserved17;/*merlin*/
	short Reserved18;/*merlin*/
	short Reserved19;/*merlin*/
	short Reserved20;/*merlin*/
	short Reserved21;/*merlin*/
	short Reserved22;/*merlin*/
	short Reserved23;/*merlin*/
	short Reserved24;/*merlin*/
	short Reserved25;/*merlin*/
	short Reserved26;/*merlin*/
	short Reserved27;/*merlin*/
	short Reserved28;/*merlin*/
	short Reserved29;/*merlin*/
	short Reserved30;/*merlin*/
} VIP_Vertical_edg_Shp_t; /*ROW 041*/

typedef struct {
	short ise_en;/*Merlin2*/
	short ise_step;/*Merlin2*/
	short ise_scale;/*Merlin2*/
	short ise_ub;/*Merlin2*/
	short ise_h_st;/*Merlin2*/
	short ise_h_end;/*Merlin2*/
	short ise_v_st;/*Merlin2*/
	short ise_v_end;/*Merlin2*/
	short Reserved8;
	short Reserved9;
	short Reserved10;
	short Reserved11;
	short Reserved12;
	short Reserved13;
	short Reserved14;
	short Reserved15;
	short Reserved16;
	short Reserved17;
	short Reserved18;
	short Reserved19;
	short Reserved20;
	short Reserved21;
	short Reserved22;
	short Reserved23;
	short Reserved24;
	short Reserved25;
	short Reserved26;
	short Reserved27;
	short Reserved28;
	short Reserved29;
	short Reserved30;
} VIP_SEGPK_ISE; /*ROW 043*/

typedef struct {
	VIP_2D_Peak_AdaptCtrl TwoD_Peak_AdaptCtrl;/*row000*/
	VIP_2D_Peak_AdaptCtrl2 TwoD_Peak_AdaptCtrl2;/*row001*/
	VIP_Edge_Shp_set Edge_Shp_set;/*row002*/
	VIP_Edge_Shp_coef1 Edge_Shp_coef1;/*row003*/
	VIP_Edge_Shp_coef2 Edge_Shp_coef2;/*row004*/
	VIP_Edge_Shp_coef3 Edge_Shp_coef3;/*row005*/
	VIP_Edge_Shp_coef4 Edge_Shp_coef4;/*row006 magellan2*/
	VIP_Edge_Shp_coef5 Edge_Shp_coef5;/*row007 magellan2*/
	VIP_Edge_EMF_Table Edge_EMF_Table;/*row010*/
	VIP_Texture_Shp_set Texture_Shp_set;/*row011*/
	VIP_Texture_Shp_coef1 Texture_Shp_coef1;/*row012*/
	VIP_Texture_Shp_coef2 Texture_Shp_coef2;/*row013*/
	VIP_Texture_Shp_coef3 Texture_Shp_coef3;/*row014*/
	VIP_Texture_Shp_coef4 Texture_Shp_coef4;/*row015 magellan2*/
	VIP_Texture_Shp_coef5 Texture_Shp_coef5;/*row016 magellan2*/
	VIP_Texture_EMF_Table Texture_EMF_Table;/*row019*/
	VIP_Vertical_Shp_t Vertical;/*row020*/
	VIP_DRV_Mk3_Lpf Mk3_Lpf;/*row023*/
	VIP_Max_Min_Gain Max_Min_Gain;/*row024*/
	VIP_Max_Min_Lpf Max_Min_Lpf;/*row025*/
	VIP_Gain_By_Y Gain_By_Y;/*row026*/
	VIP_Edge_Coring Edge_Coring;/*row027*/
	VIP_Slope_gain_mask Slope_gain_mask;/*row029*/
	VIP_SR_Control SR_Control;/*row031 magellan2*/
	VIP_SR_H21 SR_H9;/*row032 magellan2*/
	VIP_SR_H21 SR_H13;/*row033 magellan2*/
	VIP_SR_H21 SR_H21;/*row034 magellan2*/
	VIP_SR_H21 SR_V9_1;/*row035 magellan2*/
	VIP_SR_H21 SR_V9_2;/*row036 magellan2*/
	VIP_SR_H21 SR_V9_3;/*row037 magellan2*/
	VIP_SR_H21 SR_Ring_H11;/*row038 magellan2*/
	VIP_SR_H21 SR_Ring_V9;/*row039 magellan2*/
	VIP_Vertical_edg_Shp_t Vertical_edg;/*row041 Merlin*/
} VIP_Sharpness_Table;

typedef struct {

	unsigned short cds_enable;
	unsigned short cds_edg_enable;
	unsigned short cds_tex_enable;
	unsigned short cds_v_enable;
	unsigned short cds_v_edg_enable;
	unsigned short reserve5;
	unsigned short reserve6;
	unsigned short reserve7;
	unsigned short reserve8;
	unsigned short reserve9;
	unsigned short reserve10;
	unsigned short reserve11;
	unsigned short reserve12;
	unsigned short reserve13;
	unsigned short reserve14;
	unsigned short reserve15;
	unsigned short reserve16;
	unsigned short reserve17;
	unsigned short reserve18;
	unsigned short reserve19;
	unsigned short reserve20;

} VIP_CDS_Global_Ctrl;

typedef struct {

	short u_lb;
	short u_ub;
	short v_lb;
	short v_ub;
	short cu;
	short cv;
	short uv_rad;
	short cm_conti_area;
	short cm_conti_enable;
	short cds_cm_enable;
	short cm_mode;
	short tex_gain_pos;
	short tex_gain_neg;
	short tex_lv;
	short tex_hv_pos;
	short tex_hv_neg;
	short edge_gain_pos;
	short edge_gain_neg;
	short edge_lv;
	short edge_hv_pos;
	short edge_hv_neg;
	short v_tex_gain_pos;
	short v_tex_gain_neg;
	short v_tex_lv;
	short v_tex_hv_pos;
	short v_tex_hv_neg;
	short v_edge_gain_pos;
	short v_edge_gain_neg;
	short v_edge_lv;
	short v_edge_hv_pos;
	short v_edge_hv_neg;
} VIP_CDS_CM_Ctrl;

typedef struct {

	VIP_CDS_Global_Ctrl CDS_Global_Ctrl;
	VIP_CDS_CM_Ctrl CDS_CM0_Ctrl;
	VIP_CDS_CM_Ctrl CDS_CM1_Ctrl;
	VIP_CDS_CM_Ctrl CDS_CM2_Ctrl;
	VIP_CDS_CM_Ctrl CDS_CM3_Ctrl;
} VIP_CDS_Table;


typedef struct {
	unsigned char VD_hue;
	unsigned char VD_saturation;
	unsigned char VD_contrast;
	unsigned char VD_brightness;
} VIP_VD_ConBriHueSat;

#ifdef yc_tbl

typedef enum _VPQ_INPUT_SRC_TYPE
{
	/* 0 */ INPUT_SRC_AV,
	/* 1 */ INPUT_SRC_TV,
	/* 2 */ INPUT_SRC_EXTV,
	/* 3 */ INPUT_SRC_MAX
}VPQ_INPUT_SRC_TYPE;

typedef enum _VPQ_YCDELAY_TYPE
{
	/* 0 */ YCDELAY_Y,
	/* 1 */ YCDELAY_CB,
	/* 2 */ YCDELAY_CR,
	/* 3 */ YCDELAY_NEW_Y,
	/* 4 */ YCDELAY_NEW_CB,
	/* 5 */ YCDELAY_NEW_CR,
	/* 6 */ YCDELAY_NEW_CKY,
	/* 7 */ YCDELAY_4FSC_Y,
	/* 8 */ YCDELAY_4FSC_CB,
	/* 9 */ YCDELAY_4FSC_CR,
	/* 10*/ YCDLEAY_MAX
}VPQ_YCDELAY_TYPE;

typedef enum _VPQ_YCSEP_STATUS
{
	/* 0 */ YCSEP_STATUS_1D,
	/* 1 */ YCSEP_STATUS_2D3D,
	/* 2 */ YCSEP_STATUS_MAX
}VPQ_YCSEP_STATUS;

typedef enum _VPQ_DCTI_TYPE
{
	/* 0 */ VPQ_DCTI_1D,
	/* 1 */ VPQ_DCTI_2D,
	/* 2 */ VPQ_DCTI_3D,
	/* 3 */ VPQ_DCTI_MAX
}VPQ_DCTI_TYPE;

typedef enum _VPQ_INPUT_FORMAT
{
	/* 0 */ FORMAT_NTSC,
	/* 1 */ FORMAT_PALM,
	/* 2 */ FORMAT_NTSC50,
	/* 3 */ FORMAT_PALN,
	/* 4 */ FORMAT_NTSC443,
	/* 5 */ FORMAT_PAL60,
	/* 6 */ FORMAT_PALI,
	/* 7 */ FORMAT_SECAM,
	/* 8 */ FORMAT_MAX
}VPQ_INPUT_FORMAT;
#endif

typedef struct {
	unsigned char Pk_En;
	unsigned char Pk_Coring;
	unsigned char Pk_X1;
	unsigned char Pk_NdLmtP;
	unsigned char Pk_NdLmtN;
	unsigned char Pk_NdG1;
	unsigned char Pk_NdG2;

} VIP_SU_PK_Coeff;

typedef struct {
	unsigned int R[512];
	unsigned int G[512];
	unsigned int B[512];

} VIP_Gamma;

typedef struct {
	unsigned int R[128];
	unsigned int G[128];
	unsigned int B[128];

} VIP_output_Gamma;

typedef struct {

	short MB_V_En;/*magellan2*/
	short MB_H_En;/*magellan2*/
	short MB_Ver_Vfilter_data;/*magellan2*/
	short mb_peaking_en;/*magellan2*/
	short MB_SNR_flag_en;/*merlin2*/

	short MB_MOS_coring;/*merlin2*/
	short MB_WEI_coring;/*merlin2*/
	short MB_De_Over_HVmin_en;/*Merlin4*/ // 0: no interaction  1: become min  2: become max
	short MB_De_Over_BSmaxmin;/*Merlin4*/
	short mb_de_over_hvmin_ben;/*Mario*/

	short mb_de_over_hvmin_sen;/*Mario*/
	short Reserved11;/*magellan2*/
	short Reserved12;/*magellan2*/
	short Reserved13;/*magellan2*/
	short Reserved14;/*magellan2*/

	short Reserved15;/*magellan2*/
	short Reserved16;/*magellan2*/
	short Reserved17;/*magellan2*/
	short Reserved18;/*magellan2*/
	short Reserved19;/*magellan2*/

} VIP_MBPK_Ctrl;

typedef struct {

	 short MB_Hor_Filter_C0;/*magellan2*/
	 short MB_Hor_Filter_C1;/*magellan2*/
	 short MB_Hor_Filter_C2;/*magellan2*/
	 short MB_Hor_Filter_C3;/*magellan2*/
	 short MB_Hor_Filter_C4;/*magellan2*/
	 short MB_Hor_Filter_C5;/*magellan2*/
	 short MB_Hor_Filter_C6;/*magellan2*/
	 short Reserved7;/*magellan2*/
	 short Reserved8;/*magellan2*/
	 short Reserved9;/*magellan2*/
	 short Reserved10;/*magellan2*/
	 short Reserved11;/*magellan2*/
	 short Reserved12;/*magellan2*/
	 short Reserved13;/*magellan2*/
	 short Reserved14;/*magellan2*/
	 short MB_Filter_en;/*magellan2*/
	 short Reserved16;/*magellan2*/
	 short Reserved17;/*magellan2*/
	 short Reserved18;/*magellan2*/
	 short Reserved19;/*magellan2*/

} VIP_MBPK_H_Coef;

typedef struct {

	 short MB_Gain_Pos;/*magellan2*/
	 short MB_Gain_Neg;/*magellan2*/
	 short MB_HV_Pos;/*magellan2*/
	 short MB_HV_Neg;/*magellan2*/
	 short MB_LV;/*magellan2*/
	 short MB_LV2;/*magellan2*/
	 short MB_Gain_Pos2;/*magellan2*/
	 short MB_Gain_Neg2;/*magellan2*/
	 short MB_D2_shift_bit;/*magellan2*/
	 short Reserved10;/*magellan2*/
	 short Reserved11;/*magellan2*/
	 short Reserved12;/*magellan2*/
	 short Reserved13;/*magellan2*/
	 short Reserved14;/*magellan2*/
	 short Reserved15;/*magellan2*/
	 short Reserved16;/*magellan2*/
	 short Reserved17;/*magellan2*/
	 short Reserved18;/*magellan2*/
	 short Reserved19;/*magellan2*/
	 short Reserved20;/*magellan2*/

} VIP_MBPK_H_Table;

typedef struct {

	 short MB_Ver_Filter_C0;/*magellan2*/
	 short MB_Ver_Filter_C1;/*magellan2*/
	 short MB_Ver_Filter_C2;/*magellan2*/
	 short Reserved3;/*magellan2*/
	 short Reserved4;/*magellan2*/
	 short Reserved5;/*magellan2*/
	 short Reserved6;/*magellan2*/
	 short Reserved7;/*magellan2*/
	 short Reserved8;/*magellan2*/
	 short Reserved9;/*magellan2*/
	 short Reserved10;/*magellan2*/
	 short Reserved11;/*magellan2*/
	 short Reserved12;/*magellan2*/
	 short Reserved13;/*magellan2*/
	 short Reserved14;/*magellan2*/
	 short MB_Filter_en;/*magellan2*/
	 short Reserved16;/*magellan2*/
	 short Reserved17;/*magellan2*/
	 short Reserved18;/*magellan2*/
	 short Reserved19;/*magellan2*/

} VIP_MBPK_V_Coef;

typedef struct {

	 short MB_Gain_Pos_V;/*magellan2*/
	 short MB_Gain_Neg_V;/*magellan2*/
	 short MB_HV_Pos_V;/*magellan2*/
	 short MB_HV_Neg_V;/*magellan2*/
	 short MB_LV_V;/*magellan2*/
	 short MB_LV2_V;/*magellan2*/
	 short MB_Gain_Pos2_V;/*magellan2*/
	 short MB_Gain_Neg2_V;/*magellan2*/
	 short MB_D2_shift_bit_V;/*magellan2*/
	 short Reserved10;/*magellan2*/
	 short Reserved11;/*magellan2*/
	 short Reserved12;/*magellan2*/
	 short Reserved13;/*magellan2*/
	 short Reserved14;/*magellan2*/
	 short Reserved15;/*magellan2*/
	 short Reserved16;/*magellan2*/
	 short Reserved17;/*magellan2*/
	 short Reserved18;/*magellan2*/
	 short Reserved19;/*magellan2*/
	 short Reserved20;/*magellan2*/

} VIP_MBPK_V_Table;

typedef struct {

	 short MB_De_Over_H_en;/*merlin2*/
	 short MB_De_Over_H_S_gain;/*merlin2*/
	 short MB_De_Over_H_B_range;/*merlin2*/
	 short MB_De_Over_H_S_range;/*merlin2*/
	 short mb_de_over_h_t_gain;
	 short Reserved5;
	 short Reserved6;
	 short Reserved7;
	 short Reserved8;
	 short Reserved9;
	 short Reserved10;
	 short Reserved11;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;

} VIP_MBPK_H_Deovershoot0;

typedef struct {

	 short x0;
	 short y0;
	 short a0;
	 short Reserved4;
	 short x1;
	 short y1;
	 short a1;
	 short Reserved8;
	 short x2;
	 short y2;
	 short a2;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_MBPK_H_Deovershoot1;

typedef struct {

	 short x0;
	 short y0;
	 short a0;
	 short Reserved4;
	 short x1;
	 short y1;
	 short a1;
	 short Reserved8;
	 short x2;
	 short y2;
	 short a2;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_MBPK_H_Deovershoot2;

typedef struct {

	 short MB_De_Over_V_en;/*merlin2*/
	 short MB_De_Over_V_S_gain;/*merlin2*/
	 short MB_De_Over_V_B_range;/*merlin2*/
	 short MB_De_Over_V_S_range;/*merlin2*/
	 short mb_de_over_v_t_gain;
	 short Reserved5;
	 short Reserved6;
	 short Reserved7;
	 short Reserved8;
	 short Reserved9;
	 short Reserved10;
	 short Reserved11;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;

} VIP_MBPK_V_Deovershoot0;

typedef struct {

	 short x0;
	 short y0;
	 short a0;
	 short Reserved4;
	 short x1;
	 short y1;
	 short a1;
	 short Reserved8;
	 short x2;
	 short y2;
	 short a2;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_MBPK_V_Deovershoot1;

typedef struct {

	 short x0;
	 short y0;
	 short a0;
	 short Reserved4;
	 short x1;
	 short y1;
	 short a1;
	 short Reserved8;
	 short x2;
	 short y2;
	 short a2;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_MBPK_V_Deovershoot2;

typedef struct {

	 short MB_WEI_coring0;
	 short MB_WEI_coring1;
	 short MB_WEI_coring2;
	 short MB_WEI_coring3;
	 short MB_WEI_coring4;
	 short MB_WEI_coring5;
	 short MB_WEI_coring6;
	 short MB_WEI_coring7;
	 short MB_WEI_coring8;
	 short Reserved10;
	 short Reserved11;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_MBPK_WEI_coring;

typedef struct {

	VIP_MBPK_Ctrl MBPK_Ctrl;
	VIP_MBPK_H_Coef MBPK_H_Coef;
	VIP_MBPK_H_Table MBPK_H_Table;
	VIP_MBPK_V_Coef MBPK_V_Coef;
	VIP_MBPK_V_Table MBPK_V_Table;
	VIP_MBPK_H_Deovershoot0 MBPK_H_Deovershoot0;/*row005 merlin2*/
	VIP_MBPK_H_Deovershoot1 MBPK_H_Deovershoot1;/*row006 merlin4*/
	VIP_MBPK_H_Deovershoot2 MBPK_H_Deovershoot2;/*row007 merlin4*/
	VIP_MBPK_V_Deovershoot0 MBPK_V_Deovershoot0;/*row008 merlin2*/
	VIP_MBPK_V_Deovershoot1 MBPK_V_Deovershoot1;/*row009 merlin4*/
	VIP_MBPK_V_Deovershoot2 MBPK_V_Deovershoot2;/*row010 merlin4*/
	VIP_MBPK_WEI_coring MBPK_WEI_coring;/*row011 merlin4*/
	VIP_MBPK_H_Coef MBPK_H_C1ef;
	VIP_MBPK_V_Coef MBPK_V_C1ef;

} VIP_MBPK_Table;

#define VIP_D_DLTI_table_num D_DLTI_table_num
typedef struct {

	 short Dlti_en;
	 short Reserved2;
	 short Reserved3;
	 short Reserved4;
	 short Reserved5;
	 short Reserved6;
	 short Reserved7;
	 short Reserved8;
	 short Reserved9;
	 short Reserved10;
	 short Reserved11;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_DLTI_Control;

typedef struct {

	 short dlti_h_en;
	 short Ovc_en;
	 short Data_sel;
	 short Can_shift;
	 short pnth;

	 short maxlen;
	 short Ffd1_th;
	 short Ffd1_ratio;
	 short ovc_extend;
	 short ffd0_start;

	 short Reserved11;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_DLTI_H_1;

typedef struct {

	 short tn_blend_en;
	 short tn_blend_mode;
	 short Reserved3;
	 short Reserved4;
	 short D1_CP_shift;
	 short DCP_th;
	 short rn_extend_en;
	 short Reserved8;
	 short ffd2_en;
	 short Ffd2_range2L;
	 short Ffd2_range2R;
	 short Ffd2_ratio;
	 short flat_score_en;
	 short flat_score_thl;
	 short flat_s_range;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_DLTI_H_2;

typedef struct {

	 short Tnoff0;
	 short Tnoff1;
	 short Tnoff2;
	 short Tnoff3;
	 short Tnoff4;
	 short Tnoff5;
	 short Tnoff6;
	 short Tnoff7;
	 short Tnoff8;
	 short Tnoff9;
	 short Tnoff10;
	 short Tnoff11;
	 short Tnoff12;
	 short Tnoff13;
	 short Tnoff14;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_DLTI_H_Tnoff;

typedef struct {

	 short Tngain0;
	 short Tngain1;
	 short Tngain2;
	 short Tngain3;
	 short Tngain4;
	 short Tngain5;
	 short Tngain6;
	 short Tngain7;
	 short Tngain8;
	 short Tngain9;
	 short Tngain10;
	 short Tngain11;
	 short Tngain12;
	 short Tngain13;
	 short Tngain14;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_DLTI_H_Tngain;

typedef struct {

	 short over_enhance;
	 short gain_pos;
	 short gain_neg;
	 short offset_pos;
	 short offset_neg;
	 short Reserved6;
	 short Reserved7;
	 short Reserved8;
	 short Reserved9;
	 short Reserved10;
	 short Reserved11;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_DLTI_H_OverEnhance;

typedef struct {

	 short dlti_v_en;
	 short ovc_v_en;
	 short lpf_sel;
	 short can_shift_v;
	 short pnth_v;

	 short maxlen_v;
	 short ffd1_th;
	 short ffd1_ratio;
	 short ovc_v_extend;
	 short v_ffd0_start;

	 short Reserved11;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_DLTI_V_1;

typedef struct {

	 short Tnoff0;
	 short Tnoff1;
	 short Tnoff2;
	 short Tnoff3;
	 short Tnoff4;
	 short Tnoff5;
	 short Tnoff6;
	 short Reserved8;
	 short Reserved9;
	 short Reserved10;
	 short Reserved11;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_DLTI_V_Tnoff;

typedef struct {

	 short Tngain0;
	 short Tngain1;
	 short Tngain2;
	 short Tngain3;
	 short Tngain4;
	 short Tngain5;
	 short Tngain6;
	 short Reserved8;
	 short Reserved9;
	 short Reserved10;
	 short Reserved11;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_DLTI_V_Tngain;

typedef struct {

	 short over_enhance_v;
	 short gain_pos;
	 short gain_neg;
	 short offset_pos;
	 short offset_neg;
	 short Reserved6;
	 short Reserved7;
	 short Reserved8;
	 short Reserved9;
	 short Reserved10;
	 short Reserved11;
	 short Reserved12;
	 short Reserved13;
	 short Reserved14;
	 short Reserved15;
	 short Reserved16;
	 short Reserved17;
	 short Reserved18;
	 short Reserved19;
	 short Reserved20;

} VIP_DLTI_V_OverEnhance;
typedef struct{
	VIP_DLTI_Control DLTI_Control;
	VIP_DLTI_H_1 DLTI_H_1;
	VIP_DLTI_H_2 DLTI_H_2;
	VIP_DLTI_H_Tnoff DLTI_H_Tnoff;
	VIP_DLTI_H_Tngain DLTI_H_Tngain;
	VIP_DLTI_H_OverEnhance DLTI_H_OverEnhance;
	VIP_DLTI_V_1 DLTI_V_1;
	VIP_DLTI_V_Tnoff DLTI_V_Tnoff;
	VIP_DLTI_V_Tngain DLTI_V_Tngain;
	VIP_DLTI_V_OverEnhance DLTI_V_OverEnhance;
} VIP_D_DLTI_Table;

typedef struct {
	unsigned short vcti_en;
	unsigned short vcti_gain;
	unsigned short vcti_type3_thd;
	unsigned short vcti_select;
} VIP_D_vcti_t;

/*===================================*/
/*======== ICM  =======================*/
/*===================================*/

typedef enum _ICM_H_7axis_table_item {
	Axis_R = 0,
	Axis_Skin,
	Axis_Y,
	Axis_G,
	Axis_C,
	Axis_B,
	Axis_M,
	Axis_Max,
	Axis_tuning_off,
} ICM_H_7axis_table_item;

typedef struct {
	unsigned short R[5];
	unsigned short Skin[5];
	unsigned short Y[5];
	unsigned short G[5];
	unsigned short C[5];
	unsigned short B[5];
	unsigned short M[5];
	unsigned short Total_axis;
} ICM_H_7axis_table;

typedef struct {
	int ICM_OSD_index[12][12][64];
	int ICM_OSD_weight[12][12][64];
	int ICM_SR[9][6];
	int h_Total_axis;
} ICM_OSD_table;

typedef struct {
	unsigned int value[3];
	unsigned int axis_num;
} T_ICM_7Axis_Adjust_table;

/*=========================================*/
/*===========================================================================*/
/*========== About Gamma/inv Gamma/ Output Gamma/Inv output gamma ===========*/
/*===========================================================================*/
#define Bin_Num_Gamma 1024 
#define Vmax_GGamma  16383        /*14,10,10*/
#define Vmax_OutGamma   16383     /*14,10,10*/
#define Vmax_invGamma   16383     /*14, 8, 8*/
#define Vmax_invOutGamma    4095  /*12, 8, 8*/
/*===========================================================================*/
typedef struct _ISR_GAMMA_WRITE_ST {
	unsigned int encoded_GAMMA_R[Bin_Num_Gamma / 2];
	unsigned int encoded_GAMMA_G[Bin_Num_Gamma / 2];
	unsigned int encoded_GAMMA_B[Bin_Num_Gamma / 2];
	unsigned int write_SerialNumber_start;
	unsigned int write_SerialNumber_end;
	unsigned int Reserved3;
	unsigned int Reserved4;
} ISR_GAMMA_WRITE_ST;
/*======================================*/


/*===========================================================================*/
/*======= typedef struct :  RPC struct =======*/
/*===========================================================================*/

typedef struct {
	int nPoint0;
	int nPoint1;
	int nPoint2;
	int nPoint3;
	int nPoint4;
	int nPoint5;
	int nPoint6;
	int nPoint7;
	int nPoint8;
	int nPoint9;
	int nPoint10;
	int nPoint11;
	int nPoint12;
	int nPoint13;
	int nPoint14;
	int nPoint15;
	int nPoint16;
	int nPoint17;
	int nPoint18;
	int nPoint19;
	int nPoint20;
	int nPoint21;
	int nPoint22;
	int nPoint23;
	int nPoint24;
	int nPoint25;
	int nPoint26;
	int nPoint27;
	int nPoint28;
	int nPoint29;
	int nPoint30;
	int nPoint31;
	int nPoint32;
} RPC_DCC_POINTS;

typedef struct {
	unsigned int nLevel0;
	unsigned int nLevel1;
	unsigned int nLevel2;
	unsigned int nLevel3;
	unsigned int nLevel4;
	unsigned int nLevel5;
	unsigned int nLevel6;
	unsigned int nLevel7;
	unsigned int nLevel8;
	unsigned int nLevel9;
	unsigned int nLevel10;
	unsigned int nLevel11;
	unsigned int nLevel12;
	unsigned int nLevel13;
	unsigned int nLevel14;
	unsigned int nLevel15;
	unsigned int nLevel16;
	unsigned int nLevel17;
	unsigned int nLevel18;
	unsigned int nLevel19;
	unsigned int nLevel20;
	unsigned int nLevel21;
	unsigned int nLevel22;
	unsigned int nLevel23;
	unsigned int nLevel24;
	unsigned int nLevel25;
	unsigned int nLevel26;
	unsigned int nLevel27;
	unsigned int nLevel28;
	unsigned int nLevel29;
	unsigned int nLevel30;
	unsigned int nLevel31;
} RPC_HISTOGRAM_LEVEL;

typedef struct {
	unsigned short DCC_ITEM_DCCLevel ;
	unsigned short DCC_ITEM_DCL_W;
	unsigned short DCC_ITEM_DCL_B;
	unsigned short DCC_ITEM_S_high;
	unsigned short DCC_ITEM_S_low;
	unsigned short DCC_ITEM_S_index;
	unsigned short DCC_ITEM_extend_style;
	unsigned short DCC_ITEM_W_extend;
	unsigned short DCC_ITEM_B_extend;
	unsigned short DCC_ITEM_W_index;
	unsigned short DCC_ITEM_B_index;
	unsigned short DCC_ITEM_Blending_Delay_time;
	unsigned short DCC_ITEM_Blending_Step;
	unsigned short DCC_ITEM_hist_table;
	unsigned short DCC_ITEM_DCCByHistMean_Th;
} RPC_DCC_Item;

typedef enum _DCC_APL_default_th_item {
	th_0 = 0,
	th_1,
	th_2,
	th_3,
	th_4,
	th_item_Max,
} DCC_APL_th_item;

typedef enum _DrvSetting_Skip_Flag_Item {
	DrvSetting_Skip_Flag_ICM = 0,
	DrvSetting_Skip_Flag_uvOffset,
	DrvSetting_Skip_Flag_coefByY,
	DrvSetting_Skip_Flag_DCC,
	DrvSetting_Skip_Flag_Gamma,
	DrvSetting_Skip_Flag_item_Max,
} DrvSetting_Skip_Flag_Item;

/*==========================================*/

typedef struct {
	unsigned char VIP_QUALITY_Coef[VIP_QUALITY_SOURCE_NUM][VIP_QUALITY_FUNCTION_TOTAL_CHECK];
	unsigned char VIP_QUALITY_Extend_Coef[VIP_QUALITY_SOURCE_NUM][VIP_QUALITY_FUNCTION_TOTAL_CHECK];
	unsigned char VIP_QUALITY_Extend2_Coef[VIP_QUALITY_SOURCE_NUM][VIP_QUALITY_FUNCTION_TOTAL_CHECK];
	unsigned char VIP_QUALITY_Extend3_Coef[VIP_QUALITY_SOURCE_NUM][VIP_QUALITY_FUNCTION_TOTAL_CHECK];
	unsigned char VIP_QUALITY_3Dmode_Coef[VIP_QUALITY_3DSOURCE_NUM][VIP_QUALITY_3DFUNCTION_TOTAL_CHECK];/*young 3Dtable*/
	unsigned char VIP_QUALITY_Extend4_Coef[VIP_QUALITY_SOURCE_NUM][VIP_QUALITY_FUNCTION_TOTAL_CHECK];

	unsigned char SD_H_table[UZD_Idx_TBL_Max][VIP_ScalingDown_FIR_Level_Max];
	unsigned char SD_V_table[UZD_Idx_TBL_Max][VIP_ScalingDown_FIR_Level_Max];

	unsigned int  ODtable_Coeff[t_ODtable_Coeff_ROW_MAX][t_ODtable_Coeff_COL_MAX];/*young vippanel*/
	unsigned char DCR_TABLE[t_DCR_TABLE_ROW_MAX][t_DCR_TABLE_COL_MAX];/*20101111 added by Leo Chen*/
	unsigned char ICM_by_timing_picmode[VIP_TABLE_PICTURE_MODE_MAX][VIP_TABLE_TIMIMG_MAX];	/*Elsie 20131224*/

	VIP_CDS_Table CDS_ini[Sharp_table_num];
	DRV_IEdgeSmooth_Coef I_EDGE_Smooth_Coef[t_I_EDGE_Smooth_Coef_MAX];

	VIP_YUV2RGB_CSMatrix_Table YUV2RGB_CSMatrix_Table[YUV2RGB_TBL_Num];
	unsigned short Tone_Mapping_LUT_R[Tone_Mapping_Table_MAX][Tone_Mapping_size];

	VIP_VD_ConBriHueSat VD_ConBriHueSat[t_VD_ConBriHueSat_MAX];
    #ifdef yc_tbl
    unsigned char VPQ_YCBCR_DELAY_TABLE[YCSEP_STATUS_MAX][INPUT_SRC_MAX][FORMAT_MAX][YCDLEAY_MAX];
    #endif
	unsigned short tICM_ini[VIP_ICM_TBL_X][VIP_ICM_TBL_Y][VIP_ICM_TBL_Z];
	ICM_H_7axis_table tICM_H_7axis[VIP_ICM_TBL_X];
	VIP_Gamma tGAMMA[t_GAMMA_MAX];
	/*==== DCC Control Table 	====*/
	VIP_DCC_Control_Structure DCC_Control_Structure;
	DRV_Local_Contrast_Table Local_Contrast_Table[LC_Table_NUM];
	unsigned int LC_Backlight_Profile_Interpolation_table[LC_Table_NUM][LC_H_and_V][LC_Backlight_Profile_Interpolation_table_num];
	unsigned int LC_ToneMappingSlopePoint_Table[LC_Table_ToneM_NUM][LC_Curve_ToneM_PointSlope];

	VIP_Sharpness_Table Ddomain_SHPTable[Sharp_table_num];
	VIP_MBPK_Table Idomain_MBPKTable[MBPK_table_num];
	VIP_MBPK_Table Ddomain_MBSUTable[MBSU_table_num];
	DRV_NR_Item Manual_NR_Table[PQA_TABLE_MAX][DRV_NR_Level_MAX];
	unsigned int  PQA_Table[PQA_TABLE_MAX][PQA_MODE_MAX][PQA_ITEM_MAX][PQA_LEVEL_MAX];
	unsigned int  PQA_Input_Table[PQA_I_TABLE_MAX][PQA_I_ITEM_MAX][PQA_I_LEVEL_MAX];
	DRV_Still_Logo_Detection_Table Still_Logo_Detection_Table[SLD_Table_NUM];

	unsigned char Auto_Function_Array1[Auto_Function_Array_MAX];
	unsigned char Auto_Function_Array2[Auto_Function_Array_MAX];
	unsigned char Auto_Function_Array3[Auto_Function_Array_MAX];
	unsigned char Auto_Function_Array4[Auto_Function_Array_MAX];
	unsigned char Auto_Function_Array5[Auto_Function_Array_MAX];
	unsigned char DrvSetting_Skip_Flag[DrvSetting_Skip_Flag_item_Max];
	unsigned int  panel_compensation_curve[Curve_num_Max];
	VIP_D_DLTI_Table vip_D_DLTI_Table[VIP_D_DLTI_table_num];				// DRV_D_DLTI_Table D_DLTI_Table[D_DLTI_table_num]
	DRV_VipNewIDcti_Table VIP_INewDcti_Table[DCTI_TABLE_LEVEL_MAX];		// DRV_VipNewIDcti_Table gVipINewDcti_Table[DCTI_TABLE_LEVEL_MAX];
	DRV_VipNewDDcti_Table VIP_DNewDcti_Table[DCTI_TABLE_LEVEL_MAX];		// DRV_VipNewDDcti_Table gVipDNewDcti_Table[DCTI_TABLE_LEVEL_MAX]
	VIP_D_vcti_t VIP_Vcti_Table[DCTI_TABLE_LEVEL_MAX];					// DRV_D_vcti_t gVipVcti_Table[DCTI_TABLE_LEVEL_MAX]
	short VIP_Color_Mapping_Matrix[Color_Map_Table_NUM][3][3];
	
} SLR_VIP_TABLE;

typedef struct {
	unsigned char	AutoMA_En;
	unsigned char	AutoMA1_En;
	unsigned char	AutoMA2_En;
	unsigned char	AutoMA3_En ;
	unsigned char	AutoMA4_En;
	unsigned char	AutoMA5_En;
	unsigned char	AutoMA6_En;
	VIP_AutoMA_API_CTRL AutoMA_API_CTRL;
} DRV_RPC_AutoMA_Flag;

/************************************************************/
/*********** SmartPIC_Clue START ****  SmartPIC_Clue START *****/
/************************************************************/
typedef struct __VIP_SCALER_DISP_INFO {
	unsigned int 	input_src;
	unsigned int	disp_status;		/*! the status of this display*/
						/* bit0: 1 - through IP, 0 - not through IP*/
						/* bit1: 1 - 422 format, 0 - 444 format*/
						/* bit2: 1 - 10 bits, 0 - 8 bits*/
						/* bit3: 1 - 422 formart caputure memory, 0 - 422*/
						/* bit4: NR_MODE*/
						/* bit5: VUZD_MODE*/
						/* bit6: VIDEO_COMP_MODE*/
						/* bit7: 1 - Freeze, 0 - not Freeze*/
						/* bit8: 1 - Interlace in input, 0 - not Interlace in input*/
						/* bit9: 1 - show, 0 - not show*/
						/* bit10: 1 - Enter RTNR, 0 - Not Enter RTNR*/
						/* bit11: 1 - Motion_Adaptive_3A 0-Motion_Adaptive_5A*/
						/* bit14: 1 - Complete modify size, 0 - not complete*/
						/* bit15: 1 - first, 0 - no first*/
	unsigned int	display;		/* main (channel 1) or sub (channel 2)*/
	unsigned int	ucMode_Curr;	/* 480i, 480p, 576i, 576p, ...etc.*/
	unsigned int	MEM_ACT_WID;	/* the active width of FIFO output in M-domain*/
	unsigned int	MEM_ACT_LEN;	/* the active length of FIFO output in M-domain*/
	unsigned int	MEM_ACT_HSTA;
	unsigned int    MEM_ACT_VSTA;
	unsigned int	DispWid;		/* display window width*/
	unsigned int	DispLen;		/* display window length*/
	unsigned int	CapWid;
	unsigned int	CapLen;
	unsigned int	IpvActSTA_H;
	unsigned int	IpvActSTA_V;
	unsigned int	IphActWid_H;
	unsigned int	IpvActLen_V;
	unsigned int	IphActSTA_Pre;/*BLK*/
	unsigned int	IpvActSTA_Pre;/*BLK*/
	unsigned int	IphActWid_Pre;/*BLK*/
	unsigned int	IpvActLen_Pre;/*BLK*/
	unsigned int	HDelay;
	unsigned int	VDelay;
} VIP_SCALER_DISP_INFO;


#ifndef VIDEO_RECT_T
	typedef struct
	{
		unsigned short		x;	/**< horizontal Start Position in pixel [0,].*/
		unsigned short		y;	/**< vertical	Start Position in pixel [0,].*/
		unsigned short		w;	/**< horizontal Size(Width)    in pixel [0,].*/
		unsigned short		h;	/**< vertical	Size(Height)   in pixel [0,].*/
	} __VIDEO_RECT_T;
#define VIDEO_RECT_T __VIDEO_RECT_T
#endif

typedef struct {
	unsigned char DetectFlag;
	VIDEO_RECT_T OriRegion;
	VIDEO_RECT_T ActRegion;
} VIP_BlackDetectionInfo;



typedef struct _CLUES {
	unsigned int film_motion_next_h_t;
	unsigned int film_motion_next_h_m;
	unsigned int film_motion_next_h_b;
	unsigned int film_motion_pre_h_t;
	unsigned int film_motion_pre_h_m;
	unsigned int film_motion_pre_h_b;

	unsigned int film_motion_next_v_l;
	unsigned int film_motion_next_v_m;
	unsigned int film_motion_next_v_r;
	unsigned int film_motion_pre_v_l;
	unsigned int film_motion_pre_v_m;
	unsigned int film_motion_pre_v_r;

	unsigned int film_motion_next_c;
	unsigned int film_motion_pre_c;

	unsigned int ma_total_fm_sum;
	unsigned int ma_large_fm_pixel;
	unsigned int ma_large_fm_sum;

	unsigned int rtnr_noise;
	unsigned int PureColor;
	unsigned int SceneChange_NR;
	unsigned int SceneChange_by_diff_mean_Dev;
	unsigned int skintoneflag;
	unsigned int colorlessflag;
	unsigned int saturationflag;
	unsigned int di_motion_status;
	unsigned int MAD_motion_status;/*MOTION_NOISE_TYPE*/
	unsigned int block_motion_status;/*MOTION_NOISE_TYPE*/
	unsigned int RTNR_MAD_count_Y_avg_ratio;
	unsigned int RTNR_MAD_count_U_avg_ratio;
	unsigned int RTNR_MAD_count_V_avg_ratio;
	unsigned int VD_Signal_Status_value_avg;
	unsigned int VD_Noise_level_compute;
	unsigned int Y_Main_Hist[COLOR_HISTOGRAM_LEVEL];
	unsigned int Y_Main_Hist_Ratio[TV006_VPQ_chrm_bin];
	unsigned int Hist_Peak_White_Pixel_value;
	unsigned int Hist_Peak_White_IRE_Value;
	unsigned int Hist_Peak_Dark_Pixel_value;
	unsigned int Hist_Peak_Dark_IRE_Value;
	unsigned int Hist_Y_Total_sum;
	unsigned int Hist_Y_Mean_Value;
	unsigned int Hist_Y_MAX_bin_ratio;
	unsigned int Hist_Y_MAX_bin_index;
	unsigned int Hist_Final_Bin_Mode; // for debug and modify setting!
	unsigned int Hue_Main_His[COLOR_HUE_HISTOGRAM_LEVEL];
	unsigned int Hue_Main_His_Ratio[COLOR_HUE_HISTOGRAM_LEVEL];
	unsigned int Sat_Main_His[COLOR_AutoSat_HISTOGRAM_LEVEL];
	unsigned int Sat_Main_His_Ratio[COLOR_AutoSat_HISTOGRAM_LEVEL];
	unsigned int FMV_hist[FMV_Hist_Bin_Num];
	unsigned int HMC_bin_hist[HMC_Hist_Bin_Num];
	unsigned int FMV_hist_Ratio[FMV_Hist_Bin_Num];
	unsigned int HMC_bin_hist_Ratio[HMC_Hist_Bin_Num];
	unsigned int GMV;
	unsigned int sat_ratio_mean_value;
	unsigned int sat_sumcnt_ratio;
	unsigned int hue_sumcnt_ratio;
	/*unsigned int ProfileFlag;		*/ /* move to "VIP_Profile_Cal_INFO".*/
	unsigned int Remmping_Src_Timing_index;
	unsigned int Fade_flag;
	unsigned int neckin_concentric_flag;
	unsigned int PureColor_neckin_concentric_flag;
	unsigned int Motion_concentric_flag;
	unsigned int Colorbar_flag;
	unsigned int colorbar_score;
	unsigned int CVBS_black_white_falg;
	unsigned int Champagne_flag;
	unsigned int film_bad_edit_det;	/*	for HQV BD 22 football court*/
	unsigned int score_NaturalImage;
	Adapt_DCC_Info_Item Adapt_DCC_Info;
	VIP_PQA_ISR_Calculate_INFO PQA_ISR_Cal_Info;
	VIP_Profile_Cal_INFO Profile_Cal_INFO;
	VIP_SCALER_DISP_INFO SCALER_DISP_INFO;
	unsigned int RTNR_MAD_count_Y2_avg_ratio;
	unsigned int RTNR_MAD_count_U2_avg_ratio;
	unsigned int RTNR_MAD_count_V2_avg_ratio;
	unsigned int RTNR_MAD_count_Y3_avg_ratio;
	unsigned int RTNR_MAD_count_U3_avg_ratio;
	unsigned int RTNR_MAD_count_V3_avg_ratio;
	unsigned int RTNR_MAD_count_Y4_avg_ratio;
	unsigned int RTNR_MAD_count_U4_avg_ratio;
	unsigned int RTNR_MAD_count_V4_avg_ratio;
	unsigned int motion_ratio;
	unsigned int noise_ratio;
	unsigned int Hist_Y_APL_Exact;
	unsigned int Hist_Y_Total_sum_less_than_Total_HistCnt_thl;
	unsigned int Total_Hist_Cnt_less_than_one;
	unsigned int DCR_GDBC_mappingValue;
	VIP_BlackDetectionInfo BlackDetectionInfo;

} _clues;
/************************************************************/
/*********** SmartPIC_Clue END *******  SmartPIC_Clue END ******/
/************************************************************/

/************************************************************/
/***** system_setting_info START ** system_setting_info START ******/
/************************************************************/

/************************************************************/
/******************* Paenl-related parameter Tcon****************/
/************************************************************/
#define CONFIG_PANEL_TCON_BIN_COMPILE

#define OD_table_length	289

/* demura DRV*/
#define VIP_Demura_Adaptive_Scale_LV_NUM 10
#define VIP_Demura_Encode_TBL_ROW 271/*136*/
#define VIP_Demura_Encode_TBL_COL 3312/*1680*/

typedef struct {
	unsigned char type;
	unsigned char gain;
} DRV_od_table_mode_t;

typedef struct {
	unsigned int table_deltamode[OD_table_length];
	unsigned int table_targetmode[OD_table_length];
	unsigned int table_inversemode[OD_table_length];
} DRV_od_table_t;

typedef struct  _DRV_DeMura_Adaptive_TBL {
	unsigned char adaptiveScale_En;
	unsigned char adaptiveScale[VIP_Demura_Adaptive_Scale_LV_NUM];
	unsigned char adaptiveScale_APL_th[VIP_Demura_Adaptive_Scale_LV_NUM];

} DRV_DeMura_Adaptive_TBL;

typedef struct  _DRV_DeMura_CTRL_TBL {
	unsigned char demura_table_scale;
	unsigned char demura_block_size;
	unsigned char demura_rgb_table_seperate;
	unsigned char demura_en;

	unsigned short demura_r_upper_cutoff;
	unsigned short demura_r_lower_cutoff;
	unsigned short demura_r_m_level;
	unsigned short demura_r_l_level;
	unsigned short demura_r_h_level;
	unsigned short r_div_factor_l_m;
	unsigned short r_div_factor_l;
	unsigned short r_div_factor_h;
	unsigned short r_div_factor_m_h;

	unsigned short demura_g_upper_cutoff;
	unsigned short demura_g_lower_cutoff;
	unsigned short demura_g_m_level;
	unsigned short demura_g_l_level;
	unsigned short demura_g_h_level;
	unsigned short g_div_factor_l_m;
	unsigned short g_div_factor_l;
	unsigned short g_div_factor_h;
	unsigned short g_div_factor_m_h;

	unsigned short demura_b_upper_cutoff;
	unsigned short demura_b_lower_cutoff;
	unsigned short demura_b_m_level;
	unsigned short demura_b_l_level;
	unsigned short demura_b_h_level;
	unsigned short b_div_factor_l_m;
	unsigned short b_div_factor_l;
	unsigned short b_div_factor_h;
	unsigned short b_div_factor_m_h;

	unsigned char demura_table_scale_H;//juwen, mac6p
	unsigned char demura_table_scale_M;//juwen, mac6p
	unsigned char demura_table_scale_L;//juwen, mac6p
	unsigned short demura_r_h_keep_level;
    unsigned short demura_g_h_keep_level;
    unsigned short demura_b_h_keep_level;
} DRV_DeMura_CTRL_TBL;

typedef struct  _VIP_DeMura_TBL {
	DRV_DeMura_CTRL_TBL DeMura_CTRL_TBL;
	unsigned char TBL[VIP_Demura_Encode_TBL_ROW*VIP_Demura_Encode_TBL_COL];
	unsigned char table_mode;
	DRV_DeMura_Adaptive_TBL DeMura_Adaptive_TBL;

} VIP_DeMura_TBL;

typedef struct  _DRV_DeMura_DMA_TBL {
	unsigned int burst_len;
	unsigned int line_128_num;
	unsigned int line_step;
	unsigned int table_height;
	unsigned int table_width;

} DRV_DeMura_DMA_TBL;

typedef enum _VIP_DeMura_TBL_Mode {
	DeMura_TBL_241x136 = 0,
	DeMura_TBL_481x271,

	VIP_DeMura_TBL_Mode_MAX,
} VIP_DeMura_TBL_Mode;

typedef enum _VIP_DeMura_Channel_Mode {
	DeMura_Ch_Y_Mode = 0,
	DeMura_Ch_RGB_Separate,

	VIP_DeMura_Channel_Mode_MAX,
} VIP_DeMura_Channel_Mode;

typedef enum _SLR_VIP_TBL_HEADER_TBL_DeMura_TBL_ITEM{
	DEMURA_TBL_HEADER_TBL_SIZE = 0,
    DEMURA_TBL_HEADER_TBL_reserved1,
    DEMURA_TBL_HEADER_TBL_reserved2,
    DEMURA_TBL_HEADER_TBL_reserved3,
    DEMURA_TBL_HEADER_TBL_reserved4,
    DEMURA_TBL_HEADER_TBL_reserved5,
    DEMURA_TBL_HEADER_TBL_reserved6,
    DEMURA_TBL_HEADER_TBL_reserved7,
    DEMURA_TBL_HEADER_TBL_reserved8,
    DEMURA_TBL_HEADER_TBL_reserved9,
    DEMURA_TBL_HEADER_TBL_reserved10,
    DEMURA_TBL_HEADER_TBL_reserved11,
    DEMURA_TBL_HEADER_TBL_reserved12,
	DEMURA_TBL_HEADER_TBL_reserved13,
	DEMURA_TBL_HEADER_TBL_reserved14,
	DEMURA_TBL_HEADER_TBL_CRC16,
	
	DEMURA_TBL_HEADER_TBL_Max,

} SLR_VIP_TBL_HEADER_TBL_DeMura_TBL_ITEM;

typedef struct  _SLR_VIP_TBL_HEADER_TBL_DeMura {	
	unsigned int HEADER[DEMURA_TBL_HEADER_TBL_Max];
	VIP_DeMura_TBL vip_DeMura_TBL;

} SLR_VIP_TBL_HEADER_TBL_DeMura;


typedef struct
{
	int valc_en;
	int hpf_clip;
	int hpf_weight[16];
	int sat_weight[16];
	unsigned char RgnWeightEnable;
	unsigned int Sta_x;
	unsigned int Sta_y;
	unsigned char InputSz;
	unsigned char SftSel;
	unsigned char HorWeight[25];
	unsigned char VerWeight[10];
} DRV_pcid_valc_t;

typedef struct _VIP_Output_InvOutput_Gamma_CTRL_ITEM{
	unsigned char output_Gamma_En;
	unsigned char output_Location;
	unsigned char InvOutput_Gamma_En;

} VIP_Output_InvOutput_Gamma_CTRL_ITEM;

typedef struct _VIP_Output_Gamma_TBL{
	unsigned int TBL_R[Bin_Num_Gamma/2];
	unsigned int TBL_G[Bin_Num_Gamma/2];
	unsigned int TBL_B[Bin_Num_Gamma/2];
	unsigned int TBL_W[Bin_Num_Gamma/2];

} VIP_Output_Gamma_TBL;

typedef struct _VIP_InvOutput_Gamma_TBL{
	unsigned int TBL_R[Bin_Num_Gamma/2];
	unsigned int TBL_G[Bin_Num_Gamma/2];
	unsigned int TBL_B[Bin_Num_Gamma/2];

} VIP_InvOutput_Gamma_TBL;


typedef struct _VIP_Output_InvOutput_Gamma{
	VIP_Output_InvOutput_Gamma_CTRL_ITEM Output_InvOutput_Gamma_CTRL_ITEM;
	VIP_Output_Gamma_TBL Output_Gamma_TBL;
	VIP_InvOutput_Gamma_TBL InvOutput_Gamma_TBL;

} VIP_Output_InvOutput_Gamma;
/************************************************************/
/******************* Paenl-related parameter Tcon****************/
/************************************************************/


typedef struct {
	unsigned char Input_src_Type;		/*_SRC_TYPE*/
	unsigned char Input_src_Form;		/*_SRC_FROM_TYPE*/
	unsigned char Input_VO_Form;		/*_VO_SRC_FROM*/
	unsigned char Input_TVD_Form;		/*ENUM_TVD_INPUT_FORMAT*/
	unsigned char vdc_color_standard;	/*VDC_MODE*/
	unsigned char Timing;				/*PresetModeDef*/
	unsigned char HDMI_video_format;	/*HDMI_COLOR_SPACE_T*/
	unsigned char HDMI_data_range;	/*HDMI_COLOR_SPACE_T*/
	unsigned char VIP_3D_source;			/*VIP_SOURCE_TIMING*/
	unsigned char Display_RATIO_TYPE;	/*SLR_RATIO_TYPE*/
	unsigned char DCC_FW_ONOFF;
	unsigned char DCC_hist_shift_bit;
	unsigned char Histogram_Bin_Mode; 	/*add from Merln2, 20160624*/
	unsigned char RTK_DCR_Enable;
	unsigned char NowSourceOption;		/*SourceOption*/
	unsigned char xvYcc_auto_mode_flag;
	unsigned char xvYcc_flag;
	unsigned char HDR_flag;
	unsigned char DolbyHDR_flag;
	unsigned char switch_DVI_HDMI;
	unsigned char VIP_3D_display_flag;
	unsigned char input_display;	/* main or sub channel*/
	unsigned char HDMI_color_space;
	unsigned char color_fac_src_idx;
	unsigned char YC_separation_status;
	unsigned short IV_Start;
	unsigned short IH_Start;
	unsigned short I_Width;
	unsigned short I_Height;
	unsigned short Mem_Width;
	unsigned short Mem_Height;
	unsigned short Cap_Width;
	unsigned short Cap_Height;
	unsigned short DI_Width;
	unsigned short D_Width;
	unsigned short D_Height;
	unsigned int VO_Width;
	unsigned int VO_Height;
	VIP_BT2020_CTRL	BT2020_CTRL;
	VIP_RGB2YUV_COEF_CSMatrix RGB2YUV_COEF_CSMatrix;
	/*DRV_VIP_YUV2RGB_CSMatrix YUV2RGB_COEF_CSMatrix;*/
	VIP_USING_YUV2RGB_CSMatrix using_YUV2RGB_Matrix_Info;
	unsigned char DCC_Status_Change;
	VIP_PQ_Setting_Info	PQ_Setting_Info;
	VIP_OSD_LAYER_INFO OSD_Info;
	DRV_RPC_AutoMA_Flag	S_RPC_AutoMA_Flag;
	VIP_ISR_Printf_Flag	ISR_Printf_Flag;
	unsigned char Debug_Buff_8[Debug_Buff_Num];
	unsigned short Debug_Buff_16[Debug_Buff_Num];
	unsigned int Debug_Buff_32[Debug_Buff_Num];
	VIP_HDR_CTRL HDR_CTRL;
} _system_setting_info;
/************************************************************/
/****** system_setting_info END ****  system_setting_info END *******/
/************************************************************/

/************************************************************/
/***** PQ check check START ** PQ check check START ******/
/************************************************************/
typedef enum
{
	PQ_check_all = 0,
	PQ_check_source_CVBS,
	PQ_check_source_TV,
	PQ_check_source_YPP,
	PQ_check_source_HDMI,
	PQ_check_source_DTV,
	PQ_check_source_PVR,
	PQ_check_source_HDR,
	PQ_check_source_DolbyHDR,

	PQ_check_timming_Interlace,
	PQ_check_timming_Progressive,
	PQ_check_timming_SD,
	PQ_check_timming_HD,
	PQ_check_timming_UHD,

	PQ_check_MAX,
} _PQ_check_source;

typedef struct {
	unsigned int pq_check_register;
	unsigned char pq_check_bitup;
	unsigned char pq_check_bitlow;
} PQ_check_register;

typedef struct {
	char PQ_check_table_item[10];
	unsigned int PQ_check_source[3];
	PQ_check_register  PQ_check_table_register;
	unsigned int PQ_check_table_value;
} PQ_check_struct;

typedef struct {
	char PQ_check_register_diff_table_item[10];
	unsigned int PQ_check_register_diff_table_source[3];
	PQ_check_register  PQ_check_register_diff_table_register_1;
	char PQ_check_register_diff_table_Operand;
	PQ_check_register  PQ_check_register_diff_table_register_2;
	unsigned int PQ_check_register_diff_table_value;
} PQ_check_diff_struct;
/************************************************************/
/******PQ check check END ****  PQ check check END *******/
/************************************************************/

/************************************************************/
/***** PQ device START ** PQ device START ******/
/************************************************************/
typedef struct {
	unsigned char pqautocheck_en;
} PQ_device_struct;
/************************************************************/
/******PQ device END ****  PQ device END *******/
/************************************************************/

/************************************************************/
/****** VIP Custom Table START ****   VIP Custom Table START *****/
/************************************************************/
typedef struct {
	unsigned char InitParam[16];
	unsigned char DCR_TABLE[New_DCR_TABLE_NUM][DCR_TABLE_ROW_NUM][DCR_TABLE_COL_NUM];	/*20121121 and*/
	VIP_DITHER_CTRL DIHER_TABLE_CTRL[7];
} SLR_VIP_TABLE_CUSTOM_TV001;

typedef struct _SLR_VIP_TABLE_CUSTOM_TV002 {
	unsigned char VIP_QUALITY_Coef_TV002[VIP_QUALITY_SOURCE_NUM][VIP_QUALITY_FUNCTION_TOTAL_CHECK];
	VIP_BP_Function BP_Function;
	VIP_MAGIC_GAMMA_Function MAGIC_GAMMA_Function;
	VIP_INTELLIGENT_PICTURE_Function INTELLIGENT_PICTURE_Function;
	VIP_ADV_API ADV_API;
	VIP_DBC DBC;
	VIP_ZERO_D ZERO_D;

} SLR_VIP_TABLE_CUSTOM_TV002;

typedef struct _VIP_MAGIC_GAMMA_Curve_Driver_Data {
	unsigned short GammaR[MAGIC_GAMMA_POINT_NUM];
	unsigned short GammaG[MAGIC_GAMMA_POINT_NUM];
	unsigned short GammaB[MAGIC_GAMMA_POINT_NUM];
	unsigned short Gamma_Coef[MAGIC_GAMMA_POINT_NUM];
	unsigned short shift_bit;
} VIP_MAGIC_GAMMA_Curve_Driver_Data;
/************************************************************/
/****** VIP Custom Table END ********** VIP Custom Table END *****/
/************************************************************/

///////////////////*************************************************////////////////////////////
///////////////////*****************OD bin flow define**************////////////////////////////
///////////////////*************************************************////////////////////////////
typedef enum _SLR_VIP_OD_LEVEL_ITEM{
	VIP_OD_LEVEL_OFF = 0,
	VIP_OD_LEVEL_LOW,
	VIP_OD_LEVEL_MID,
	VIP_OD_LEVEL_HIGH,
	
	VIP_OD_LEVEL_MAX,

} SLR_VIP_OD_LEVEL_ITEM;

typedef struct {
	unsigned char od_en;
	unsigned char Y_mode_en;
	unsigned char VRR_en;
} DRV_OD_Ctrl;

typedef struct {
	unsigned char gain_r;
	unsigned char gain_g;
	unsigned char gain_b;
} DRV_OD_Gain;

typedef struct {
	unsigned char NR_mode_en;
	unsigned char NR_R_en;
	unsigned char NR_G_en;
	unsigned char NR_B_en;
} DRV_OD_NR_Mode_EN_Ctrl;

typedef struct {
	unsigned char NR_R_th;
	unsigned char NR_G_th;
	unsigned char NR_B_th;
} DRV_OD_NR_Mode_Ctrl;

typedef struct {
	DRV_OD_Ctrl OD_Ctrl;
	DRV_OD_Gain OD_Gain;
	DRV_OD_NR_Mode_EN_Ctrl NR_Mode_EN_Ctrl;
	DRV_OD_NR_Mode_Ctrl NR_Mode_Ctrl;
} DRV_OD_Main_Ctrl;

typedef struct {
	unsigned char hhf_en;
	unsigned char scaling_en;
	unsigned char scale_filter;
} DRV_OD_down_sample;

typedef struct {
	unsigned char hhf_max;
	unsigned char hhf_gain;
	unsigned char hhf_w1;
	unsigned char hhf_w2;
	unsigned char hhf_th1;
	unsigned char hhf_th2;
} DRV_OD_down_sample_LUT;

typedef struct {
	DRV_OD_down_sample OD_down_sample;
	DRV_OD_down_sample_LUT OD_down_sample_LUT;
} DRV_OD_down_sample_Ctrl;

typedef struct {
	unsigned char bias_en_Y_sat_en;
	unsigned char gray2rgb_en;
	unsigned char gray2cmy_en;
	unsigned char color2gray_en;
} DRV_OD_Y_mode_Bias_En;

typedef struct {
	unsigned char bias_R_en; 
	unsigned char bias_G_en;
	unsigned char bias_B_en; 
	unsigned char bias_C_en; 
	unsigned char bias_M_en; 
	unsigned char bias_Y_en;
} DRV_OD_Y_mode_Bias_En2;

typedef struct {
	unsigned char Y_diff_neg_en; 
	unsigned char Y_diff_pos_en; 
	unsigned char offset_clamp_en; 
} DRV_OD_Y_mode_Bias_En3;

typedef struct {
	DRV_OD_Y_mode_Bias_En OD_Y_mode_Bias_En;
	DRV_OD_Y_mode_Bias_En2 OD_Y_mode_Bias_En2;
	DRV_OD_Y_mode_Bias_En3 OD_Y_mode_Bias_En3;	
} DRV_OD_Y_mode_Bias_Ctrl;

typedef struct {
	unsigned char R_diff_th;
	unsigned char G_diff_th;
	unsigned char B_diff_th;
	unsigned char RG_diff_th;
	unsigned char GB_diff_th;
	unsigned char BR_diff_th;
} DRV_OD_Y_mode_Bias_Settings_Diff_Th;

typedef struct {
	unsigned char R_offset;
	unsigned char G_offset;
	unsigned char B_offset;
	unsigned char C_offset;
	unsigned char M_offset;
	unsigned char Y_offset;
} DRV_OD_Y_mode_Bias_Settings_Offset;

typedef struct {
	unsigned char sat_diff_factor;
	unsigned char sat_color2gray_th;
	unsigned char sat_color2gray_offset_gain;
} DRV_OD_Y_mode_Bias_Settings_Sat;

typedef struct {
	unsigned char ydiff_low_th;
	unsigned char ydiff_high_th;
	unsigned char offset_low_th;
	unsigned char offset_high_th;
	unsigned char offset_slope;
} DRV_OD_Y_mode_Bias_Settings_Low;

typedef struct {
	unsigned char ydiff_low_th_gray;
	unsigned char ydiff_high_th_gray;
	unsigned char offset_low_th_gray;
	unsigned char offset_high_th_gray;
	unsigned char offset_slop_gray;
} DRV_OD_Y_mode_Bias_Settings_Low_gray;

typedef struct {
	DRV_OD_Y_mode_Bias_Settings_Diff_Th  Y_mode_Bias_Settings_Diff_Th;
	DRV_OD_Y_mode_Bias_Settings_Offset Y_mode_Bias_Settings_Offset;
	DRV_OD_Y_mode_Bias_Settings_Sat Y_mode_Bias_Settings_Sat;
	DRV_OD_Y_mode_Bias_Settings_Low Y_mode_Bias_Settings_Low;
	DRV_OD_Y_mode_Bias_Settings_Low_gray Y_mode_Bias_Settings_Low_gray;
} DRV_OD_Y_mode_Bias_Settings;

typedef struct {
	unsigned char V_en;
	unsigned short V_factor;
	unsigned short V_offset;
} DRV_OD_Vertical_Gain_General;

typedef struct {
	unsigned char V_gain0;
	unsigned char V_gain1;
	unsigned char V_gain2;
	unsigned char V_gain3;
	unsigned char V_gain4;
	unsigned char V_gain5;
	unsigned char V_gain6;
	unsigned char V_gain7;
	unsigned char V_gain8;
	unsigned char V_gain9;
	unsigned char V_gain10;
	unsigned char V_gain11;
	unsigned char V_gain12;
	unsigned char V_gain13;
	unsigned char V_gain14;
	unsigned char V_gain15;
} DRV_OD_Vertical_Gain_V0_15;

typedef struct {
	unsigned char V_gain16;
	unsigned char V_gain17;
	unsigned char V_gain18;
	unsigned char V_gain19;
	unsigned char V_gain20;
	unsigned char V_gain21;
	unsigned char V_gain22;
	unsigned char V_gain23;
	unsigned char V_gain24;
	unsigned char V_gain25;
	unsigned char V_gain26;
	unsigned char V_gain27;
	unsigned char V_gain28;
	unsigned char V_gain29;
	unsigned char V_gain30;
	unsigned char V_gain31;
} DRV_OD_Vertical_Gain_V16_31;

typedef struct {
	DRV_OD_Vertical_Gain_General Vertical_Gain_General;
	DRV_OD_Vertical_Gain_V0_15 Vertical_Gain_V0_15;
	DRV_OD_Vertical_Gain_V16_31 Vertical_Gain_V16_31;
} DRV_OD_Vertical_Gain_ctrl;

typedef struct {
	unsigned char VRR_TH0;
	unsigned char VRR_TH1;
	unsigned char VRR_TH2;
	unsigned char VRR_TH3;
	unsigned char VRR_TH4;
	unsigned char VRR_TH5;
	unsigned char VRR_TH6;
	unsigned char VRR_TH7;
	unsigned char VRR_TH8;
	unsigned char VRR_TH9;
} VIP_OD_VRR_TH_TBL;

#define VIP_OD_TBL_SIZE (17*17)	// 289
#define VIP_OD_TBL_NUM 10

typedef struct {
	unsigned char OD_TBL_R[VIP_OD_TBL_SIZE];
	unsigned char OD_TBL_G[VIP_OD_TBL_SIZE];	
	unsigned char OD_TBL_B[VIP_OD_TBL_SIZE];		
} VIP_OD_TBL;

typedef struct _SLR_VIP_TABLE_OD {
	DRV_OD_Main_Ctrl OD_Main_Ctrl;
	DRV_OD_down_sample_Ctrl OD_down_sample_Ctrl;
	DRV_OD_Y_mode_Bias_Ctrl OD_Y_mode_Bias_Ctrl;
	DRV_OD_Y_mode_Bias_Settings Y_mode_Bias_Settings;
	DRV_OD_Vertical_Gain_ctrl Vertical_Gain_ctrl;
	VIP_OD_VRR_TH_TBL OD_VRR_TH_TBL;
	VIP_OD_TBL OD_TBL[VIP_OD_TBL_NUM];
} SLR_VIP_TABLE_OD;
///////////////////*************************************************////////////////////////////
///////////////////*****************OD bin flow define**************////////////////////////////
///////////////////*************************************************////////////////////////////

///////////////////*************************************************////////////////////////////
///////////////////*****************OUTGAMMA bin flow define**************////////////////////////////
///////////////////*************************************************////////////////////////////
#define SLR_VIP_OUTGAMMA_TBL_Num 10
#define SLR_VIP_OUTGAMMA_Index_Num 1024

typedef struct _SLR_VIP_OutputGamma_Ctrl {
	unsigned char OG_En;
	unsigned char OG_Loc;
	unsigned char reserved_0;
	unsigned char reserved_1;
} SLR_VIP_OutputGamma_Ctrl;

typedef struct _SLR_VIP_OutputGamma_Main_Ctrl {
	SLR_VIP_OutputGamma_Ctrl OutputGamma_Ctrl;
} SLR_VIP_OutputGamma_Main_Ctrl;

typedef struct _SLR_VIP_TABLE_OUTGAMMA {
	SLR_VIP_OutputGamma_Main_Ctrl OutputGamma_Main_Ctrl;
	unsigned short OUTGAMMA_TH[Freq_TH_MAX];
	unsigned short OUTGAMMA_TH_Extend[Freq_TH_MAX];
	unsigned short OUTGAMMA_TBL[SLR_VIP_OUTGAMMA_TBL_Num][VIP_CHANNEL_RGB_MAX][SLR_VIP_OUTGAMMA_Index_Num];
	unsigned short OUTGAMMA_TBL_Extend[SLR_VIP_OUTGAMMA_TBL_Num][VIP_CHANNEL_RGB_MAX][SLR_VIP_OUTGAMMA_Index_Num];
} SLR_VIP_TABLE_OUTGAMMA;

typedef enum _SLR_VIP_AutoACC_CTRL_HEADER{
	AutoACC_CTRL_index_num,
	AutoACC_CTRL_bit_num,	
    AutoACC_CTRL_mode,
    AutoACC_CTRL_Freq_for_Fix_TBL,
    AutoACC_CTRL_OG_En,
    AutoACC_CTRL_OG_Loc,
	AutoACC_CTRL_OG_TBL_Size,
	AutoACC_CTRL_Reserved7,
	AutoACC_CTRL_Reserved8,
	AutoACC_CTRL_Reserved9,
	AutoACC_CTRL_Reserved10,
	AutoACC_CTRL_Reserved11,
	AutoACC_CTRL_Reserved12,
	AutoACC_CTRL_Reserved13,
	AutoACC_CTRL_Reserved14,
	AutoACC_CTRL_CRC,
	
	SLR_VIP_AutoACC_HEADER_MAX,	
} SLR_VIP_AutoACC_HEADER;

typedef struct _SLR_VIP_TABLE_AUTO_ACC_CTRL_CTRL {
	unsigned int Header[SLR_VIP_AutoACC_HEADER_MAX];
	unsigned int OUTGAMMA_TBL[VIP_CHANNEL_RGB_MAX][SLR_VIP_OUTGAMMA_Index_Num];
	
} SLR_VIP_TABLE_AUTO_ACC_CTRL;

///////////////////*************************************************////////////////////////////
///////////////////*****************OUTGAMMA bin flow define**************////////////////////////////
///////////////////*************************************************////////////////////////////

///////////////////*************************************************////////////////////////////
///////////////////*****************VALC bin flow define**************////////////////////////////
///////////////////*************************************************////////////////////////////
#define SLR_VIP_VALC_W_Num 16
#define SLR_VIP_VALC_TBL_SIZE (17*17)

typedef struct _SLR_VIP_VALC_En_Ctrl{
	unsigned char PCID_EN;
	unsigned char PCID2_EN;
	unsigned char reserved_0;
	unsigned char reserved_1;
} SLR_VIP_VALC_En_Ctrl;

typedef struct _SLR_VIP_VALC_pixel_ref_setting_t{
	unsigned char line_sel;
	unsigned char point_sel;
	unsigned char color_sel;
} SLR_VIP_VALC_pixel_ref_setting_t;

typedef struct _SLR_VIP_VALC_ref_setting_t{
	SLR_VIP_VALC_pixel_ref_setting_t line1_R_EVEN;
	SLR_VIP_VALC_pixel_ref_setting_t line1_G_EVEN;
	SLR_VIP_VALC_pixel_ref_setting_t line1_B_EVEN;
	SLR_VIP_VALC_pixel_ref_setting_t line1_R_ODD;
	SLR_VIP_VALC_pixel_ref_setting_t line1_G_ODD;
	SLR_VIP_VALC_pixel_ref_setting_t line1_B_ODD;
	SLR_VIP_VALC_pixel_ref_setting_t line2_R_EVEN;
	SLR_VIP_VALC_pixel_ref_setting_t line2_G_EVEN;
	SLR_VIP_VALC_pixel_ref_setting_t line2_B_EVEN;
	SLR_VIP_VALC_pixel_ref_setting_t line2_R_ODD;
	SLR_VIP_VALC_pixel_ref_setting_t line2_G_ODD;
	SLR_VIP_VALC_pixel_ref_setting_t line2_B_ODD;
	SLR_VIP_VALC_pixel_ref_setting_t line3_R_EVEN;
	SLR_VIP_VALC_pixel_ref_setting_t line3_G_EVEN;
	SLR_VIP_VALC_pixel_ref_setting_t line3_B_EVEN;
	SLR_VIP_VALC_pixel_ref_setting_t line3_R_ODD;
	SLR_VIP_VALC_pixel_ref_setting_t line3_G_ODD;
	SLR_VIP_VALC_pixel_ref_setting_t line3_B_ODD;
	SLR_VIP_VALC_pixel_ref_setting_t line4_R_EVEN;
	SLR_VIP_VALC_pixel_ref_setting_t line4_G_EVEN;
	SLR_VIP_VALC_pixel_ref_setting_t line4_B_EVEN;
	SLR_VIP_VALC_pixel_ref_setting_t line4_R_ODD;
	SLR_VIP_VALC_pixel_ref_setting_t line4_G_ODD;
	SLR_VIP_VALC_pixel_ref_setting_t line4_B_ODD;
} SLR_VIP_VALC_ref_setting_t;

typedef struct _SLR_VIP_VALC_data_t{
	unsigned char Boundary_Mode;
	SLR_VIP_VALC_ref_setting_t VALC_ref_setting_t;
} SLR_VIP_VALC_data_t;

typedef struct _SLR_VIP_VALC_Polarity_Line_CTRL{
	unsigned short Line_1;
	unsigned short Line_2;
	unsigned short Line_3;
	unsigned short Line_4;
	unsigned short Line_5;
	unsigned short Line_6;
	unsigned short Line_7;
	unsigned short Line_8;
} SLR_VIP_VALC_Polarity_Line_CTRL;

typedef struct _SLR_VIP_VALC_Polarity_Table{
	SLR_VIP_VALC_Polarity_Line_CTRL VALC_Polarity_Line_CTRL_R;
	SLR_VIP_VALC_Polarity_Line_CTRL VALC_Polarity_Line_CTRL_G;
	SLR_VIP_VALC_Polarity_Line_CTRL VALC_Polarity_Line_CTRL_B;
} SLR_VIP_VALC_Polarity_Table;

typedef struct _SLR_VIP_VALC_CTRL{
	unsigned char VALC_En;
	unsigned char Val_Clip;
	unsigned char First_Line_Tab;
	unsigned char VALC_W_High_Freq[SLR_VIP_VALC_W_Num];
	unsigned char VALC_W_Sat[SLR_VIP_VALC_W_Num];
	
} SLR_VIP_VALC_CTRL;

typedef struct _SLR_VIP_VALC_1D2D_TBL{
	unsigned short LUT1_Red[SLR_VIP_VALC_TBL_SIZE];
	unsigned short LUT2_Red[SLR_VIP_VALC_TBL_SIZE];
	unsigned short LUT1_Green[SLR_VIP_VALC_TBL_SIZE];
	unsigned short LUT2_Green[SLR_VIP_VALC_TBL_SIZE];
	unsigned short LUT1_Blue[SLR_VIP_VALC_TBL_SIZE];
	unsigned short LUT2_Blue[SLR_VIP_VALC_TBL_SIZE];

} SLR_VIP_VALC_1D2D_TBL;

typedef struct _SLR_VIP_VALC_TBL{
	SLR_VIP_VALC_data_t VALC_data_t;
	SLR_VIP_VALC_Polarity_Table VALC_Polarity_Table;
	SLR_VIP_VALC_CTRL VALC_CTRL;
	SLR_VIP_VALC_1D2D_TBL VALC_1D2D_TBL;
} SLR_VIP_VALC_TBL;

typedef struct _SLR_VIP_TABLE_VALC{
	SLR_VIP_VALC_En_Ctrl VIP_VALC_En_Ctrl;
	SLR_VIP_VALC_TBL VALC_TBL;
} SLR_VIP_TABLE_VALC;

typedef enum _SLR_VIP_VALC_TBL_HEADER_TBL_ITEM{
	VALC_TBL_HEADER_TBL_SIZE = 0,
    VALC_TBL_HEADER_TBL_reserved1,
    VALC_TBL_HEADER_TBL_reserved2,
    VALC_TBL_HEADER_TBL_reserved3,
    VALC_TBL_HEADER_TBL_reserved4,
    VALC_TBL_HEADER_TBL_reserved5,
    VALC_TBL_HEADER_TBL_reserved6,
    VALC_TBL_HEADER_TBL_reserved7,
    VALC_TBL_HEADER_TBL_reserved8,
    VALC_TBL_HEADER_TBL_reserved9,
    VALC_TBL_HEADER_TBL_reserved10,
    VALC_TBL_HEADER_TBL_reserved11,
    VALC_TBL_HEADER_TBL_reserved12,
	VALC_TBL_HEADER_TBL_reserved13,
	VALC_TBL_HEADER_TBL_reserved14,
	VALC_TBL_HEADER_TBL_CRC16,
	
	VALC_TBL_HEADER_TBL_Max,

} SLR_VIP_VALC_TBL_HEADER_TBL_ITEM;

typedef struct _SLR_VIP_TBL_HEADER_TBL_VALC {
	unsigned int HEADER[VALC_TBL_HEADER_TBL_Max];
	SLR_VIP_TABLE_VALC Table_VALC;
} SLR_VIP_TBL_HEADER_TBL_VALC;

///////////////////*************************************************////////////////////////////
///////////////////*****************VALC bin flow define**************////////////////////////////
///////////////////*************************************************////////////////////////////

///////////////////*************************************************////////////////////////////
///////////////////*****************PCID bin flow define**************////////////////////////////
///////////////////*************************************************////////////////////////////
#define SLR_VIP_PCID_X_Num 25
#define SLR_VIP_PCID_Y_Num 10
#define SLR_VIP_PCID_TBL_SIZE (17*17)

typedef struct _SLR_VIP_PCID_En_Ctrl{
	unsigned char LINEOD_EN;
	unsigned char reserved_0;
	unsigned char reserved_1;
	unsigned char reserved_2;
} SLR_VIP_PCID_En_Ctrl;

typedef struct _SLR_VIP_PCID_pixel_ref_setting_t{
	unsigned char line_sel;
	unsigned char point_sel;
	unsigned char color_sel;
} SLR_VIP_PCID_pixel_ref_setting_t;

typedef struct _SLR_VIP_PCID_ref_setting_t{
	SLR_VIP_PCID_pixel_ref_setting_t line1_R_EVEN;
	SLR_VIP_PCID_pixel_ref_setting_t line1_G_EVEN;
	SLR_VIP_PCID_pixel_ref_setting_t line1_B_EVEN;
	SLR_VIP_PCID_pixel_ref_setting_t line1_R_ODD;
	SLR_VIP_PCID_pixel_ref_setting_t line1_G_ODD;
	SLR_VIP_PCID_pixel_ref_setting_t line1_B_ODD;
	SLR_VIP_PCID_pixel_ref_setting_t line2_R_EVEN;
	SLR_VIP_PCID_pixel_ref_setting_t line2_G_EVEN;
	SLR_VIP_PCID_pixel_ref_setting_t line2_B_EVEN;
	SLR_VIP_PCID_pixel_ref_setting_t line2_R_ODD;
	SLR_VIP_PCID_pixel_ref_setting_t line2_G_ODD;
	SLR_VIP_PCID_pixel_ref_setting_t line2_B_ODD;
	SLR_VIP_PCID_pixel_ref_setting_t line3_R_EVEN;
	SLR_VIP_PCID_pixel_ref_setting_t line3_G_EVEN;
	SLR_VIP_PCID_pixel_ref_setting_t line3_B_EVEN;
	SLR_VIP_PCID_pixel_ref_setting_t line3_R_ODD;
	SLR_VIP_PCID_pixel_ref_setting_t line3_G_ODD;
	SLR_VIP_PCID_pixel_ref_setting_t line3_B_ODD;
	SLR_VIP_PCID_pixel_ref_setting_t line4_R_EVEN;
	SLR_VIP_PCID_pixel_ref_setting_t line4_G_EVEN;
	SLR_VIP_PCID_pixel_ref_setting_t line4_B_EVEN;
	SLR_VIP_PCID_pixel_ref_setting_t line4_R_ODD;
	SLR_VIP_PCID_pixel_ref_setting_t line4_G_ODD;
	SLR_VIP_PCID_pixel_ref_setting_t line4_B_ODD;
} SLR_VIP_PCID_ref_setting_t;

typedef struct _SLR_VIP_PCID_data_t{
	unsigned char NR_en;
	unsigned char PCID_thd_mode;
	unsigned char Table1_Red;
	unsigned char Table1_Green;
	unsigned char Table1_Blue;
	unsigned char Table2_Red;
	unsigned char Table2_Green;
	unsigned char Table2_Blue;
	unsigned char Table1_XTR_En;
	unsigned char Table2_XTR_En;
	unsigned char Boundary_Mode;
	SLR_VIP_PCID_ref_setting_t PCID_ref_setting_t;
} SLR_VIP_PCID_data_t;

typedef struct _SLR_VIP_PCID_Polarity_Line_CTRL{
	unsigned short Line_1;
	unsigned short Line_2;
	unsigned short Line_3;
	unsigned short Line_4;
	unsigned short Line_5;
	unsigned short Line_6;
	unsigned short Line_7;
	unsigned short Line_8;
} SLR_VIP_PCID_Polarity_Line_CTRL;

typedef struct _SLR_VIP_PCID_Polarity_Table{
	SLR_VIP_PCID_Polarity_Line_CTRL PCID_Polarity_Line_CTRL_R;
	SLR_VIP_PCID_Polarity_Line_CTRL PCID_Polarity_Line_CTRL_G;
	SLR_VIP_PCID_Polarity_Line_CTRL PCID_Polarity_Line_CTRL_B;
} SLR_VIP_PCID_Polarity_Table;

typedef struct _SLR_VIP_PCID_CTRL{
	unsigned char Regional_w_en;
	unsigned short Start_X;
	unsigned short Start_Y;
	unsigned char Input_Size;
	unsigned char Shift_sel;
	unsigned char Reginal_W_X[SLR_VIP_PCID_X_Num];
	unsigned char Reginal_W_Y[SLR_VIP_PCID_Y_Num];

} SLR_VIP_PCID_CTRL;

typedef struct _SLR_VIP_PCID_1D2D_TBL{
	unsigned short LUT1_Red[SLR_VIP_PCID_TBL_SIZE];
	unsigned short LUT2_Red[SLR_VIP_PCID_TBL_SIZE];
	unsigned short LUT1_Green[SLR_VIP_PCID_TBL_SIZE];
	unsigned short LUT2_Green[SLR_VIP_PCID_TBL_SIZE];
	unsigned short LUT1_Blue[SLR_VIP_PCID_TBL_SIZE];
	unsigned short LUT2_Blue[SLR_VIP_PCID_TBL_SIZE];

} SLR_VIP_PCID_1D2D_TBL;

typedef struct _SLR_VIP_PCID_TBL{
	SLR_VIP_PCID_data_t PCID_data_t;
	SLR_VIP_PCID_Polarity_Table PCID_Polarity_Table;
	SLR_VIP_PCID_CTRL PCID_CTRL;
	SLR_VIP_PCID_1D2D_TBL PCID_1D2D_TBL;
} SLR_VIP_PCID_TBL;

typedef struct _SLR_VIP_TABLE_LINEOD{
	SLR_VIP_PCID_En_Ctrl VIP_PCID_En_Ctrl;
	SLR_VIP_PCID_TBL PCID_TBL;
} SLR_VIP_TABLE_LINEOD;

typedef enum _SLR_VIP_LINEOD_TBL_HEADER_TBL_ITEM{
	LINEOD_TBL_HEADER_TBL_SIZE = 0,
    LINEOD_TBL_HEADER_TBL_reserved1,
    LINEOD_TBL_HEADER_TBL_reserved2,
    LINEOD_TBL_HEADER_TBL_reserved3,
    LINEOD_TBL_HEADER_TBL_reserved4,
    LINEOD_TBL_HEADER_TBL_reserved5,
    LINEOD_TBL_HEADER_TBL_reserved6,
    LINEOD_TBL_HEADER_TBL_reserved7,
    LINEOD_TBL_HEADER_TBL_reserved8,
    LINEOD_TBL_HEADER_TBL_reserved9,
    LINEOD_TBL_HEADER_TBL_reserved10,
    LINEOD_TBL_HEADER_TBL_reserved11,
    LINEOD_TBL_HEADER_TBL_reserved12,
	LINEOD_TBL_HEADER_TBL_reserved13,
	LINEOD_TBL_HEADER_TBL_reserved14,
	LINEOD_TBL_HEADER_TBL_CRC16,
	
	LINEOD_TBL_HEADER_TBL_Max,

} SLR_VIP_LINEOD_TBL_HEADER_TBL_ITEM;

typedef struct _SLR_VIP_TBL_HEADER_TBL_LINEOD {
	unsigned int HEADER[LINEOD_TBL_HEADER_TBL_Max];
	SLR_VIP_TABLE_LINEOD Table_LINEOD;
} SLR_VIP_TBL_HEADER_TBL_LINEOD;

///////////////////*************************************************////////////////////////////
///////////////////*****************PCID bin flow define**************////////////////////////////
///////////////////*************************************************////////////////////////////

/*===========================================================================*/

typedef struct {
        unsigned char need_to_add_extern_PQ_Table;
} SLR_VIP_TABLE_EXT;

#ifndef LD_Common
#define LD_Common

/* ----- for Local Dimming -----*/
#define BL_Profile_Table_NUM 6
#define BL_Profile_Table_ROW 8
#define BL_Profile_Table_COLUMN 40
#define LD_Table_NUM 7


typedef struct {
	unsigned char ld_comp_en;
	unsigned char ld_db_en;
	unsigned char ld_db_apply;
	unsigned char ld_db_read_level;
	unsigned char ld_3d_mode;
	unsigned char ld_blk_hnum;
	unsigned char ld_blk_vnum;
	unsigned char ld_subblk_mode;
	unsigned char ld_gdim_mode;
	unsigned char ld_blk_type;
	unsigned char ld_hist_mode;
	unsigned char ld_tenable;
	unsigned char ld_valid;
	unsigned short ld_blk_hsize;
	unsigned short ld_blk_vsize;

} DRV_LD_Global_Ctrl;


typedef struct {
	unsigned char ld_maxgain;
	unsigned char ld_avegain;
	unsigned char ld_histshiftbit;
	unsigned char ld_hist0gain;
	unsigned char ld_hist1gain;
	unsigned char ld_hist2gain;
	unsigned char ld_hist3gain;
	unsigned char ld_hist4gain;
	unsigned char ld_hist5gain;
	unsigned char ld_hist6gain;
	unsigned char ld_hist7gain;

} DRV_LD_Backlight_Decision;

typedef struct
{
	unsigned char ld_spatialcoef0;
	unsigned char ld_spatialcoef1;
	unsigned char ld_spatialcoef2;
	unsigned char ld_spatialcoef3;
	unsigned char ld_spatialcoef4;
	unsigned char ld_spatialcoef5;
	unsigned char ld_spatialcoef6;
	unsigned char ld_spatialcoef7;
	unsigned char ld_spatialcoef8;
	unsigned char ld_spatialcoef9;
	unsigned char ld_spatialcoef10;
	unsigned char spatial_new_mode;
	unsigned char ld_spatialnewcoef00;
	unsigned char ld_spatialnewcoef01;
	unsigned char ld_spatialnewcoef02;
	unsigned char ld_spatialnewcoef03;
	unsigned char ld_spatialnewcoef04;
	unsigned char ld_spatialnewcoef10;
	unsigned char ld_spatialnewcoef11;
	unsigned char ld_spatialnewcoef12;
	unsigned char ld_spatialnewcoef13;
	unsigned char ld_spatialnewcoef14;
	unsigned char ld_spatialnewcoef20;
	unsigned char ld_spatialnewcoef21;
	unsigned char ld_spatialnewcoef22;
	unsigned char ld_spatialnewcoef23;
	unsigned char ld_spatialnewcoef24;
	unsigned char ld_spatialnewcoef30;
	unsigned char ld_spatialnewcoef31;
	unsigned char ld_spatialnewcoef32;
	unsigned char ld_spatialnewcoef33;
	unsigned char ld_spatialnewcoef34;
	unsigned char ld_spatialnewcoef05;
	unsigned char ld_spatialnewcoef15;
	unsigned char ld_spatialnewcoef25;
	unsigned char ld_spatialnewcoef35;
	unsigned char ld_spatialnewcoef40;
	unsigned char ld_spatialnewcoef41;
	unsigned char ld_spatialnewcoef50;
} DRV_LD_Spatial_Filter;

typedef struct {
	unsigned char ld_spatialremapen;
	unsigned short ld_spatialremaptab[65];
} DRV_LD_Spatial_Remap;

typedef struct {
	unsigned char ld_backlightboosten;
	unsigned short ld_boost_gain_lut[65];
	unsigned short ld_boost_curve_lut[65];
} DRV_LD_Boost;

typedef struct {

	unsigned char ld_tmp_pos0thd;
	unsigned char ld_tmp_pos1thd;
	unsigned char ld_tmp_posmingain;
	unsigned char ld_tmp_posmaxgain;
	unsigned char ld_tmp_neg0thd;
	unsigned char ld_tmp_neg1thd;
	unsigned char ld_tmp_negmingain;
	unsigned char ld_tmp_negmaxgain;
	unsigned char ld_tmp_maxdiff;
	unsigned char ld_tmp_scenechangegain1;

} DRV_LD_Temporal_Filter;

typedef struct {
	unsigned char ld_blu_wr_num;
	unsigned short ld_blu_spitotal;
	unsigned char ld_blu_spien;
	unsigned char ld_blu_nodim;
	unsigned char ld_blu_usergain;
} DRV_LD_Backlight_Final_Decision;

typedef struct {
	unsigned char ld_data_comp_mode;
	unsigned char ld_comp_satrange;
	unsigned char ld_comp_softmax;
	unsigned char ld_comp_minlimit;
	unsigned char ld_softcomp_gain;
	unsigned char ld_comp_maxmode;
	unsigned char ld_comp_inv_shift;
	unsigned char ld_comp_inv_mode;
	unsigned char ld_srcgainsel;
} DRV_LD_Data_Compensation;

typedef struct {
	unsigned char ld_tab_hsize;
	unsigned int ld_hfactor;
	unsigned char ld_hinitphase3_right;
	unsigned char ld_hboundary;
	unsigned char ld_hinitphase1;
	unsigned char ld_hinitphase2;
	unsigned char ld_hinitphase3_left;
	unsigned char ld_tab_vsize;
	unsigned int ld_vfactor;
	unsigned char ld_vinitphase3_right;
	unsigned char ld_vboundary;
	unsigned char ld_vinitphase1;
	unsigned char ld_vinitphase2;
	unsigned char ld_vinitphase3_left;

	unsigned char ld_blight_update_en;
	unsigned char ld_blight_sw_mode;
	unsigned char ld_table_sw_mode;

	unsigned char ld_htabsel_0;
	unsigned char ld_htabsel_1;
	unsigned char ld_htabsel_2;
	unsigned char ld_htabsel_3;
	unsigned char ld_htabsel_4;
	unsigned char ld_htabsel_5;
	unsigned char ld_htabsel_6;
	unsigned char ld_htabsel_7;
	unsigned char ld_htabsel_8;
	unsigned char ld_htabsel_9;
	unsigned char ld_htabsel_10;
	unsigned char ld_htabsel_11;
	unsigned char ld_htabsel_12;
	unsigned char ld_htabsel_13;
	unsigned char ld_htabsel_14;
	unsigned char ld_htabsel_15;
	unsigned char ld_htabsel_16;
	unsigned char ld_htabsel_17;
	unsigned char ld_htabsel_18;
	unsigned char ld_htabsel_19;
	unsigned char ld_htabsel_20;
	unsigned char ld_htabsel_21;
	unsigned char ld_htabsel_22;
	unsigned char ld_htabsel_23;
	unsigned char ld_htabsel_24;
	unsigned char ld_htabsel_25;
	unsigned char ld_htabsel_26;
	unsigned char ld_htabsel_27;
	unsigned char ld_htabsel_28;
	unsigned char ld_htabsel_29;
	unsigned char ld_htabsel_30;
	unsigned char ld_htabsel_31;
	unsigned char ld_vtabsel_0;
	unsigned char ld_vtabsel_1;
	unsigned char ld_vtabsel_2;
	unsigned char ld_vtabsel_3;
	unsigned char ld_vtabsel_4;
	unsigned char ld_vtabsel_5;
	unsigned char ld_vtabsel_6;
	unsigned char ld_vtabsel_7;
	unsigned char ld_vtabsel_8;
	unsigned char ld_vtabsel_9;
	unsigned char ld_vtabsel_10;
	unsigned char ld_vtabsel_11;
	unsigned char ld_vtabsel_12;
	unsigned char ld_vtabsel_13;
	unsigned char ld_vtabsel_14;
	unsigned char ld_vtabsel_15;
	unsigned char ld_vtabsel_16;
	unsigned char ld_vtabsel_17;
	unsigned char ld_vtabsel_18;
	unsigned char ld_vtabsel_19;
	unsigned char ld_vtabsel_20;
	unsigned char ld_vtabsel_21;
	unsigned char ld_vtabsel_22;
	unsigned char ld_vtabsel_23;
	unsigned char ld_vtabsel_24;
	unsigned char ld_vtabsel_25;
	unsigned char ld_vtabsel_26;
	unsigned char ld_vtabsel_27;
	unsigned char ld_vtabsel_28;
	unsigned char ld_vtabsel_29;
	unsigned char ld_vtabsel_30;
	unsigned char ld_vtabsel_31;
} DRV_LD_Backlight_Profile_Interpolation;

typedef struct {
	unsigned char ld_demo_en;
	unsigned char ld_demo_mode;
	unsigned short ld_demo_top;
	unsigned short ld_demo_bottom;
	unsigned short ld_demo_left;
	unsigned short ld_demo_right;
} DRV_LD_Demo_Window;

typedef struct {
	DRV_LD_Global_Ctrl LD_Global_Ctrl;
	DRV_LD_Backlight_Decision LD_Backlight_Decision;
	DRV_LD_Spatial_Filter LD_Spatial_Filter;
	DRV_LD_Spatial_Remap LD_Spatial_Remap;
	DRV_LD_Boost LD_Boost;
	DRV_LD_Temporal_Filter LD_Temporal_Filter;
	DRV_LD_Backlight_Final_Decision LD_Backlight_Final_Decision;
	DRV_LD_Data_Compensation LD_Data_Compensation;
	DRV_LD_Backlight_Profile_Interpolation LD_Backlight_Profile_Interpolation;
	DRV_LD_Demo_Window LD_Demo_Window;

} DRV_Local_Dimming_Table;

typedef struct {
	DRV_Local_Dimming_Table Local_Dimming_Table[LD_Table_NUM];
} SLR_VIP_TABLE_LD;

typedef struct {
	unsigned int ld_comp_2Dtable[17][17];
} DRV_LD_Data_Compensation_NewMode_2DTable;

typedef struct {
	DRV_LD_Data_Compensation_NewMode_2DTable LD_Data_Compensation_NewMode_2DTable[LD_Table_NUM];
} SLR_VIP_TABLE_LDCOMP;

typedef struct {
	unsigned int LD_Backlight_Profile_Interpolation_table_H[BL_Profile_Table_NUM][BL_Profile_Table_COLUMN];
	unsigned int LD_Backlight_Profile_Interpolation_table_V[BL_Profile_Table_NUM][BL_Profile_Table_COLUMN];
	unsigned int LD_Backlight_Profile_Interpolation_table_Edge[BL_Profile_Table_NUM][BL_Profile_Table_ROW][BL_Profile_Table_COLUMN];
} SLR_VIP_TABLE_BLPF;
#endif

typedef struct{
	unsigned short APL_units;	//0: no need APL info   others: 1/2 units
	unsigned short APL_start_index;
	unsigned short UI_en;	
	unsigned short UI_start_index;
	unsigned short BPL_units;
	unsigned short BPL_start_index;
	unsigned short reserved_6;
	unsigned short reserved_7;
	unsigned short reserved_8;
	unsigned short reserved_9;
	unsigned short reserved_10;
	unsigned short reserved_11;
	unsigned short reserved_12;
	unsigned short reserved_13;
	unsigned short reserved_14;
	unsigned short reserved_15;
	unsigned short reserved_16;
	unsigned short reserved_17;
	unsigned short reserved_18;
	unsigned short reserved_19;
	unsigned short reserved_20;
	unsigned short reserved_21;
	unsigned short reserved_22;
	unsigned short reserved_23;
	unsigned short reserved_24;
	unsigned short reserved_25;
	unsigned short reserved_26;
	unsigned short reserved_27;
	unsigned short reserved_28;
	unsigned short reserved_29;
	unsigned short reserved_30;
	unsigned short reserved_31;
	unsigned short reserved_32;
	unsigned short reserved_33;
	unsigned short reserved_34;
	unsigned short reserved_35;
	unsigned short reserved_36;
	unsigned short reserved_37;
	unsigned short reserved_38;
	unsigned short reserved_39;
	unsigned short reserved_40;
	unsigned short reserved_41;
	unsigned short reserved_42;
	unsigned short reserved_43;
	unsigned short reserved_44;
	unsigned short reserved_45;
	unsigned short reserved_46;
	unsigned short reserved_47;
	
}DRV_LD_Extra_Control_Items;

typedef struct {
	DRV_LD_Extra_Control_Items ld_extra_control_items;
	unsigned short ld_datapart_index_map_table[5184];
	unsigned short ld_nondatapart_index_map_table[96];
	unsigned short leddriver_init_tbl[1024];
	unsigned short reserved_curve1[65];
	unsigned short reserved_curve2[65];
} DRV_LD_Extra_Index_Map_Table;

typedef struct {
	DRV_LD_Extra_Index_Map_Table LD_Extra_Index_Map_Table;
} SLR_VIP_TABLE_LDINDEXMAP;

typedef struct _SLR_VIP_TABLE_ICM {
	unsigned short tICM_ini[VIP_ICM_TBL_X][VIP_ICM_TBL_Y][VIP_ICM_TBL_Z];
} SLR_VIP_TABLE_ICM;

typedef struct _VIP_VDBlackLevel{
	int data[8][4];
} VIP_VDBlackLevel;

typedef struct _VIP_VDYcbcrDelay{
	int data[8][10];
} VIP_VDYcbcrDelay;

typedef struct _SLR_VIP_TABLE_VD {
	VIP_VDBlackLevel mBLMTbl[2];
	VIP_VDYcbcrDelay mYcbcrDelayTbl[4];
} SLR_VIP_TABLE_VD;

///////////////////*************************************************////////////////////////////
///////////////////*****************OSDSR ini flow**************////////////////////////////
///////////////////*************************************************////////////////////////////
#define VIP_DRV_OSDSR_TBL_NUM 2

typedef enum _VIP_DRV_OSDSR_ITEMS{
	DRV_OSDSR_sr_lpf_gain = 0,
	// v_3x5; 
	DRV_OSDSR_sr_maxmin_range,
	DRV_OSDSR_sr_lpf_range,
	DRV_OSDSR_sr_edge_range,
	// v_overshoot;
	DRV_OSDSR_sr_maxmin_limit_en,
	DRV_OSDSR_sr_maxmin_shift,
	DRV_OSDSR_sr_var_thd,
	// v_lpf_range;
	DRV_OSDSR_sr_lpf_range_en,
	DRV_OSDSR_sr_lpf_range_thd,
	DRV_OSDSR_sr_lpf_range_gain,
	DRV_OSDSR_sr_lpf_range_step,
	// v_lpf_edge;
	DRV_OSDSR_sr_lpf_edge_en,
	DRV_OSDSR_sr_lpf_edge_thd,
	DRV_OSDSR_sr_lpf_edge_gain,
	DRV_OSDSR_sr_lpf_edge_step,
	DRV_OSDSR_sr_delta_gain,
	// v_lpmm;
	DRV_OSDSR_sr_lpmm_en,
	DRV_OSDSR_sr_lpmm_1px,
	DRV_OSDSR_sr_lpmm_corner,

	VIP_DRV_OSDSR_ITEMS_MAX,
} VIP_DRV_OSDSR_ITEMS;

typedef struct _VIP_DRV_OSDSR {
	unsigned short data[VIP_DRV_OSDSR_TBL_NUM][VIP_DRV_OSDSR_ITEMS_MAX];
} VIP_DRV_OSDSR;

typedef struct _SLR_VIP_TABLE_OSDSR {
	VIP_DRV_OSDSR osdsr_table;
} SLR_VIP_TABLE_OSDSR;
///////////////////*************************************************////////////////////////////
///////////////////*****************OSDSR ini flow**************////////////////////////////
///////////////////*************************************************////////////////////////////

#ifndef LDSPI_Common
#define LDSPI_Common

typedef enum _SEND_MODE{
        VSYNC_MODE = 0,
        FREERUN_MODE_WAIT_LD,
        FREERUN_MODE_AUTO,
        TRIGGER_MODE ,
} _SEND_MODE ;

typedef enum _SHIFT_DIR{
        LEFT = 0,
        RIGHT,
} _SHIFT_DIR ;

#define LDInterface_Table_Num 7
/*General Settings---From Register*/

typedef struct {
        unsigned char  ld_mode;		   /*0:CMO Mode 1:LGD Mode 2:AS Mode 3:AS LED Mode*/
        unsigned char  separate_two_block;
        unsigned char  send_mode;/*change in K8: send mode:0 trigger mode;1:dvs mode;2:free run mode*/
	 unsigned char  send_trigger;
        unsigned char  out_data_align;	
	 unsigned char   in_data_align;
        unsigned char  as_led_with_cs;     /*asled if cs output*/
        unsigned char  as_led_reset;       /*cs high/low*/
        unsigned char  as_force_tx;        /*0:do not care  1:force to tx mode*/
        unsigned char  w_package_repeater;
	 unsigned int vsync_delay;				// add by cathy
	 unsigned int ld_backlight_timing_delay;	//add by cathy, since eva write it in drver function, ask eva/hawaii if it is not neccessary
} DRV_LDINTERFACE_Basic_Ctrl;

typedef struct {
        unsigned char ld_spi_en;       /*ip enable*/
        unsigned char start_enable;    /*disable when change params*/
} DRV_LDINTERFACE_Enable;

typedef struct {
        unsigned short output_units;//real output unit,for register need -1
        unsigned char output_data_format;
        unsigned char tailer_units;
        unsigned char tailer_format;
        unsigned char header_units;
        unsigned char header_format;
        unsigned char delay_format; //for multi unit delay
        unsigned short data_sram_separate; 	// 
        unsigned short ld_total_data_num;	//LD will total send number of data to ld_spi
} DRV_LDINTERFACE_Output;

typedef struct {
        unsigned char rx_units;
        unsigned char rx_data_format;  /*0: 8bit 1:16bit*/
        unsigned char time_out_period;
        unsigned short rx_data_shift;  /*rbus clk period, delay should be smaller than clock period*/
        unsigned char read_after_n_byte;
        unsigned char rw_cmd_byte;
        unsigned char sram_rwcmd_bit;
        unsigned char forceTxorRx_mode_en;
        unsigned char forceTxorRx_mode;
        unsigned char rw_bit_send_sel;
        unsigned char rw_cmd_sel;
        unsigned char rwcmd;

} DRV_LDINTERFACE_Read;

typedef struct {
        unsigned char shift_op;  /*0: only shift data part 1:shift whole sram*/
        unsigned char shift_dir; /*0: LEFT  1:RIGHT*/
        unsigned char shift_num; /*shift number of bit*/
        unsigned char swap_op;   /*0: only swap data part 1:swap whole sram*/
        unsigned char swap_en;   /*0: normal output       1: swap 2 bytes of the 16-bit data*/
} DRV_LDINTERFACE_SHIFT_SWAP;

typedef struct {
        unsigned char tx_sdo_inv;
        unsigned char tx_cs_inv;
        unsigned char tx_clk_inv;
        unsigned char tx_vsync_inv;
//        unsigned char tx_hsync_inv;		
} DRV_LDINTERFACE_INV;

typedef struct {
        unsigned char vrepeat_width;/*equal to H*/
        unsigned char vrepeat;
        unsigned char hrepeat;
} DRV_LDINTERFACE_Repeat;

typedef struct {
        unsigned char dv_reproduce_mode;
        unsigned short ld_spi_dh_total;
        unsigned short ld_spi_dv_total;
        unsigned char dv_total_src_sel;
        unsigned short dv_total_sync_dtg;
} DRV_LDINTERFACE_Reproduce;

typedef struct {
        unsigned char spi_intven;/*For ld_spi_int send to dctl_int1, then send to vcpu*/
        unsigned char spi_intsen;/*For ld_spi_int send to dctl_int2, then send to scpu*/
        unsigned char rx_done_inten;
        unsigned char tx_done_inten;
} DRV_LDINTERFACE_IntEn;

/*Time Info*/
typedef struct {
        unsigned int T1;
        unsigned int T1_star;
        unsigned int T2;
        unsigned int T3;
        unsigned int T4;
        unsigned int T5;
        unsigned int T6;
        unsigned int T7;
        unsigned int cs_end;
        unsigned char  fMultiUnit;//0:Each Unit Delay  1:Multi Unit Delay
        //unsigned char unit;//0:ns 1:us 2:ms  fix ns
} DRV_LDINTERFACE_TimingReal;

typedef struct {
	
	unsigned short info[40];
} DRV_LDSPI_HeaderInfo;

typedef struct {
	unsigned short reserved[24];
} DRV_LDSPI_Reserved;

/*LDSPI Info*/
typedef struct {
	 DRV_LDSPI_HeaderInfo	  LDSPI_HeaderInfo;
	 unsigned short 			LDvalue_init;	//initial backlight value
        DRV_LDSPI_Reserved Reserved;
        
} DRV_LDSPI_Info;

typedef struct {
        DRV_LDINTERFACE_Basic_Ctrl   LDINTERFACE_Basic_Ctrl;
        DRV_LDINTERFACE_Output       LDINTERFACE_Output;
        DRV_LDINTERFACE_SHIFT_SWAP   LDINTERFACE_Shift_Swap;
        DRV_LDINTERFACE_INV          LDINTERFACE_Inv;
        DRV_LDINTERFACE_Repeat       LDINTERFACE_Repeat;
        DRV_LDINTERFACE_Reproduce    LDINTERFACE_Reproduce;
        DRV_LDINTERFACE_TimingReal   LDINTERFACE_TimingReal;
	 DRV_LDSPI_Info 				LDSPI_Info;
} DRV_LDINTERFACE_Table;

typedef struct {
	DRV_LDINTERFACE_Table LDSPI_Table[LDInterface_Table_Num];
} SLR_VIP_TABLE_LDSPI;


#endif  /* end of LDSPI_Common */
#endif
