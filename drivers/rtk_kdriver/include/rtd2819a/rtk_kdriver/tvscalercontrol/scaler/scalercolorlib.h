#ifndef __SCALERCOLOR_LIB_H__
#define __SCALERCOLOR_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Header include
******************************************************************************/

#include <tvscalercontrol/vip/icm.h>
#include <tvscalercontrol/scaler/source.h>
#include <tvscalercontrol/scaler/vipinclude.h>
#include <tvscalercontrol/vip/color.h>
#include <tvscalercontrol/vip/tc_hdr.h>
#include <ioctrl/scaler/uvc_cmd_id.h>
/*******************************************************************************
* Macro
******************************************************************************/
/*#define MID_EXAMPLE(1)*/ /* Debug Message switch */
#define NOT_BY_DISPLAY 							0
#define BY_DISPLAY 								1
//#define VIP_FORCE_DISABLE_PC_RGB_YUV_444 1

/*******************************************************************************
* Constant
******************************************************************************/
/*#define example  100000*/ /* 100Khz */

#define SCALERLIB_PatternColor_OFF		0
#define SCALERLIB_PatternColor_RED		1
#define SCALERLIB_PatternColor_GREEN	2
#define SCALERLIB_PatternColor_BLUE		3
#define SCALERLIB_PatternColor_WHITE	4
#define SCALERLIB_PatternColor_BLACK	5
#define SCALERLIB_PatternColor_COLORBAR	6
#define SCALERLIB_PatternColor_GRAYBAR	7
/*yanger20130114 modify for factory string command>*/
#ifdef CONFIG_ENABLE_COMPAL_LA_FACTORY_STRING_COMMAND
#define SCALERLIB_PatternColor_GRAY_25P	8
#define SCALERLIB_PatternColor_GRAY_50P	9
#endif
/*yanger20130114 modify for factory string command<*/

#define SCALERLIB_PatternColor_DCC_ON	254
#define SCALERLIB_PatternColor_DCC_OFF	255
#ifdef ENABLE_FACTORY_FUNCTIONS_TCL /*hardy20121212*/
#define SCALERLIB_PatternColor_YELLOW	8
#define SCALERLIB_PatternColor_GRAY	9
#endif


/*******************************************************************************
 * Structure
 ******************************************************************************/
/*typedef struct*/
/*{*/
/*} MID_example_Param_t;*/


/*******************************************************************************
* enumeration
******************************************************************************/
/*typedef enum*/
/*{*/
/*    MID_Example_SLEEPING = 0,*/
/*    MID_Example_RUNNING,*/
/*} MID_Example_Status_t;*/

enum _source_TV05_index {
	_SOURCE_INDEX_TV05_VGA,
	_SOURCE_INDEX_TV_NTSC,
	_SOURCE_INDEX_TV_PAL,
	_SOURCE_INDEX_TV_SECAM,
	_SOURCE_INDEX_AVSV_NTSC,
	_SOURCE_INDEX_AVSV_PAL,
	_SOURCE_INDEX_AVSV_SECAM,
	_SOURCE_INDEX_YPBPR_SD,
	_SOURCE_INDEX_YPBPR_HD,
	_SOURCE_INDEX_HDMI_SD,
	_SOURCE_INDEX_HDMI_HD,
	_PCB_SOURCE_TV05_TOTAL_NUM
};

typedef enum _VIP_isPC_RGB444_Check_Mode {
	VIP_Disable_PC_RGB444 = 0,
	VIP_HDMI_PC_RGB444,
	VIP_HDMI_PC_YUV444,
	VIP_DP_PC_RGB444,
	VIP_DP_PC_YUV444,
	
	VIP_RGB444_MAX,
} VIP_isPC_RGB444_Check_Mode;

typedef enum _COLOR_CALLBACK_TYPE {

	COLOR_CALLBACK_GET_QUALITY_COEF = 0,
	COLOR_CALLBACK_GET_QUALITY_EXTEND,
	COLOR_CALLBACK_GET_QUALITY_EXTEND2,
	COLOR_CALLBACK_GET_QUALITY_EXTEND3,
	COLOR_CALLBACK_GET_QUALITY_EXTEND4,
	COLOR_CALLBACK_GET_QUALITY_3Dmode,
	COLOR_CALLBACK_GET_COLOR_DATA,
	COLOR_CALLBACK_GET_COLOR_FAC_MODE,
	COLOR_CALLBACK_GET_COLOR_TEMP_DATA,
	COLOR_CALLBACK_GET_COLOR_TEMP_DATA_TSB,
	COLOR_CALLBACK_GET_COLOR_TEMP_LEVEL,
	COLOR_CALLBACK_GET_PICTURE_MODE_DATA,
	COLOR_CALLBACK_GET_DNR_MODE,

	COLOR_CALLBACK_GET_CURPICMODE,
	COLOR_CALLBACK_GET_ICM_BY_TIMING_PICMODE,	/*Elsie 20131224*/

} COLOR_CALLBACK_TYPE;
#if 0
typedef enum _SLR_SLR_ACTIVE_BACKLIGHT {
	SLR_BlackAPL_a_HSCA	= 0,
	SLR_BlackAPL_b_HSCB,
	SLR_BlackAPL_c_HSCC,
	SLR_BlackAPL_d_HSCD,
	SLR_BlackAPL_e_HSCE,
	SLR_BlackAPL_f_HSCF,
	SLR_BlackAPL_g_HSCG,
	SLR_BlackAPL_h_HSCH,
	SLR_BlackAPL_BL_Th1_SABH,
	SLR_BlackAPL_BL_Th1_SABL,
	SLR_BlackAPL_BL_Th2_EABH,
	SLR_BlackAPL_BL_Th2_EABL,
	SLR_BlackAPL_BL_Th3_MLBL,
	SLR_BlackAPL_M_ATAB,
	SLR_BlackAPL_KU_USAB,
	SLR_BlackAPL_KD_DSAB,
	SLR_APL_BL_MAX,
	SLR_APL_BL_MIN,
	SLR_ACTIVE_BACKLIGHT_ALL,
} SLR_ACTIVE_BACKLIGHT;
#endif

typedef enum _MAGIC_FUNCTION_TYPE {
	MAGIC_IN_FUNCTION = 0,
	MAGIC_EXIT_FUNCTION,
} MAGIC_FUNCTION_TYPE;

typedef enum _SLR_MAGIC_TYPE {
	SLR_MAGIC_OFF,
	SLR_MAGIC_STILLDEMO,
	SLR_MAGIC_STILLDEMO_INVERSE,
	SLR_MAGIC_FULLSCREEN,
	SLR_MAGIC_DYNAMICDEMO,
	SLR_MAGIC_MOVE,
	SLR_MAGIC_MOVE_INVERSE,
	SLR_MAGIC_ZOOM,
	SLR_MAGIC_OPTIMIZE,
	SLR_MAGIC_ENHANCE,
	SLR_MAGIC_FULLGAMUT,					//full gamut
	SLR_MAGIC_LOCALDIMMING_MARQUEE,	//local dimming marquee
	SLR_MAGIC_LOCALDIMMING_TRACK,		//local dimming track
	SLR_MAGIC_MEMC_STILLDEMO_INVERSE,   // MEMC DEMO
	SLR_MAGIC_WCG_STILLDEMO_INVERSE
} SLR_MAGIC_TYPE;

typedef struct StructMagicPicInfo {
	unsigned char	magic_picture;
	unsigned char	magic_direction;
	unsigned char	magic_step;
	unsigned char	magic_parameter;
	short	magic_XStart;
	short	magic_YStart;
	short	magic_XEnd;
	short	magic_YEnd;
	short	magic_XStartForDynamic;
	short	magic_YStartForDynamic;
	short	magic_XEndForDynamic;
	short	magic_YEndForDynamic;
	int	magic_DISP_WID_OSD;
	unsigned char	pre_magic_pic;
} StructMagicPicInfo;

typedef enum {
	HDMI_CSC_DATA_RANGE_MODE_AUTO = 0x0,
	HDMI_CSC_DATA_RANGE_MODE_FULL,
	HDMI_CSC_DATA_RANGE_MODE_LIMITED,
} HDMI_CSC_DATA_RANGE_MODE_T;

typedef enum {
	VPQ_ModuleTestMode_PQByPass = 0x0,
	VPQ_ModuleTestMode_HDRPQByPass = 0x1,
	VPQ_ModuleTestMode_MAX,
} VPQ_ModuleTestMode;

typedef struct S_VPQ_ModuleTestMode {
	unsigned char ModuleTestMode_en;
	VPQ_ModuleTestMode ModuleTestMode_Flag;
} S_VPQ_ModuleTestMode;

typedef enum {
	PQ_EXT_TYPE_Ddomain_SHPTable,
	PQ_EXT_TYPE_D_DLTI_Table,
	PQ_EXT_TYPE_VipNewIDcti_Table,
	PQ_EXT_TYPE_VipNewDDcti_Table,
	PQ_EXT_TYPE_CDS_ini,
	PQ_EXT_TYPE_Idomain_MBPKTable,
	PQ_EXT_TYPE_Ddomain_MBSUTable,

	PQ_EXT_TYPE_Manual_NR_Table,
	PQ_EXT_TYPE_PQA_Table_Write_Mode,
	PQ_EXT_TYPE_PQA_Input_Table,
	PQ_EXT_TYPE_MA_SNR_IESM_Table,
	PQ_EXT_TYPE_I_De_Contour_Table,

	PQ_EXT_TYPE_ICM_Table,
	PQ_EXT_TYPE_CSC_Mapping,

	PQ_EXT_TYPE_DCC_Curve_Control_Coef,
	PQ_EXT_TYPE_DCC_Boundary_check_Table,
	PQ_EXT_TYPE_DCC_Level_and_Blend_Coef_Table,
	PQ_EXT_TYPE_DCCHist_Factor_Table,
	PQ_EXT_TYPE_DCC_AdaptCtrl_Level_Table,
	PQ_EXT_TYPE_USER_DEFINE_CURVE_DCC_CRTL_Table,
	PQ_EXT_TYPE_Database_Curve_CRTL_Table,
	PQ_EXT_TYPE_Color_Independent_Blending_Table,
	PQ_EXT_TYPE_DCC_Chroma_Compensation_Table,
	PQ_EXT_TYPE_DCC_Black_Adjust_Table,
	PQ_EXT_TYPE_Local_Contrast_Table,
	PQ_EXT_TYPE_WBBL_Table,
	PQ_EXT_TYPE_WB_BS_Table,

} PQ_EXT_TYPE;

typedef enum _VIP_CSC_DATA_RANGE_MODE_T{
	CSC_DATA_RANGE_MODE_AUTO = 0x0,
	CSC_DATA_RANGE_MODE_FULL,
	CSC_DATA_RANGE_MODE_LIMITED,

	VIP_CSC_DATA_RANGE_MODE_MAX,
} VIP_CSC_DATA_RANGE_MODE_T;

typedef enum _VIP_APDEM_OD_Channel {
	APDEM_OD_Ch_R = 0,
	APDEM_OD_Ch_G,
	APDEM_OD_Ch_B,
	
	VIP_APDEM_OD_Ch_Max,
} VIP_APDEM_OD_Channel;

#define HDRCID_detect_wid_num 6
#define HDRCID_detect_len_num 4
#define HDRCID_detect_size (HDRCID_detect_wid_num*HDRCID_detect_len_num)

typedef enum _VIP_HDRCID_Pattern_ID {
	CENTER=0,
	HDRCID_Contrast_Step = CENTER,
	
	CORNER,
	HDRCID_Brightness_Step = CORNER,
	
	CHESS5p1,
	HDRCID_BL_Step = CHESS5p1,
	
	CHESS5p2,
	HDRCID_delay = CHESS5p2,
	
	CENTER9p1,
	HDRCID_Target_Th = CENTER9p1,
	
	CENTER9p2,
	CENTER9p3,
	CENTER9p4,
	CENTER9p5,

	reserved_9,
	reserved_10,
	HDRCID_Pattern_MAX,	// ap max is 11
} VIP_HDRCID_Pattern_ID;

typedef struct _VIP_HDRCID_CTRL
{	
	short Pattern_th[HDRCID_Pattern_MAX]; // RADCR_Pattern_APL_th == 0 => enable = 0
	short Pattern_CTRL[HDRCID_Pattern_MAX]; // RADCR_Pattern_APL_th_range

	short Pattern_BL_adj[HDRCID_Pattern_MAX]; // Backlight_Offset_percentage for Pattern, -100~100
	short Pattern_Gain_adj_Offset[HDRCID_Pattern_MAX]; // ColorTemp Gain Offset for Pattern
	short Pattern_Offset_adj_Offset[HDRCID_Pattern_MAX]; // ColorTemp Offset Offset for Pattern	
	short isPQbin_update;
	short showCrossBar_debug;
	short debug_print;
} VIP_HDRCID_CTRL;

typedef struct _VIP_HDRCID_INFO
{	
	unsigned short Detect_info[VIP_CHANNEL_RGB_MAX][HDRCID_detect_size];	
	short Pattern_get_flag[HDRCID_Pattern_MAX]; // which pattern is get
	short Contrast_apply;	// contrast offset for iog con/bri
	short Brightness_apply;	// brightness offset for iog con/bri
	short bl_apply;			// bl offset for pwm or LD
	
	short Contrast_target;	// contrast offset for iog con/bri
	short Brightness_target;	// brightness offset for iog con/bri
	short bl_target;			// bl offset for pwm or LD
	short isRADCR_En;
	short Y_HIST_DIFF;
} VIP_HDRCID_INFO;

#define APDEM_Panal_ini_scale (100000)
typedef struct
{
	unsigned short un16_CIE_x;
	unsigned short	un16_CIE_y;
} VIP_PQ_COLOUR_GAMUT_POINT_T;

typedef struct
{
	VIP_PQ_COLOUR_GAMUT_POINT_T 	primary_r;
	VIP_PQ_COLOUR_GAMUT_POINT_T 	primary_g;
	VIP_PQ_COLOUR_GAMUT_POINT_T 	primary_b;
	VIP_PQ_COLOUR_GAMUT_POINT_T 	primary_wihte;
	unsigned short un16_max_luminance;
	unsigned short un16_min_luminance;
} VIP_PANEL_DATA_T; 	// *100000

#define VIP_DEM_10p_Gamma_Offset_idx_102_to_1024 1
#define VIP_DEM_10p_Gamma_Offset_Num 10
#define VIP_DEM_3D_LUT_Offset_Num 18	// 6 axis, 3 channel

// define panel ini address
#define VIP_DEM_PANEL_INI_Gamma_TBL_NUM 10
#define VIP_DEM_PANEL_INI_Gamma_IDX_NUM 1024
typedef struct _VIP_DEM_PANEL_INI_Gamma_TBL {
	unsigned int gamma_r[VIP_DEM_PANEL_INI_Gamma_IDX_NUM];
	unsigned int gamma_g[VIP_DEM_PANEL_INI_Gamma_IDX_NUM];
	unsigned int gamma_b[VIP_DEM_PANEL_INI_Gamma_IDX_NUM];
	
} VIP_DEM_PANEL_INI_Gamma_TBL;

typedef struct _VIP_DEM_PANEL_INI_Color_Chromaticity {
	unsigned int Panel_red_x;
	unsigned int Panel_red_y;
	unsigned int Panel_green_x;
	unsigned int Panel_green_y;
	unsigned int Panel_blue_x;
	unsigned int Panel_blue_y;
	unsigned int Panel_white_x;
	unsigned int Panel_white_y;
	unsigned int Panel_luma_max;
	unsigned int Panel_luma_min;	
} VIP_DEM_PANEL_INI_Color_Chromaticity;

typedef struct _VIP_DEM_PANEL_INI_TBL {	
	unsigned int gamma_bit_num;
	VIP_DEM_PANEL_INI_Gamma_TBL DEM_PANEL_INI_Gamma_TBL[VIP_DEM_PANEL_INI_Gamma_TBL_NUM];
	VIP_DEM_PANEL_INI_Color_Chromaticity DEM_PANEL_INI_Color_Chromaticity;
} VIP_DEM_PANEL_INI_TBL;

typedef struct _VIP_DEM_PANEL_INI_Color_Chromaticity_Facotry {
	unsigned int Panel_red_x;
	unsigned int Panel_red_y;
	unsigned int Panel_green_x;
	unsigned int Panel_green_y;
	unsigned int Panel_blue_x;
	unsigned int Panel_blue_y;
	unsigned int Panel_white_x;
	unsigned int Panel_white_y;
	
} VIP_DEM_PANEL_INI_Color_Chromaticity_Facotry;

typedef struct _VIP_DEM_PANEL_INI_Panel_Luma_Facotry {
	unsigned int Panel_luma_max;
	unsigned int Max_CLL_set;
	unsigned int CalmanCurve;
	unsigned int HDR_Type;
} VIP_DEM_PANEL_INI_Panel_Luma_Facotry;

typedef struct _VIP_INI_Gamma_Curve {	
	unsigned int gamma_bit_num;
	VIP_DEM_PANEL_INI_Gamma_TBL DEM_PANEL_INI_Gamma_TBL;
} VIP_INI_Gamma_Curve;

typedef enum _VIP_DEM_ARG_CTRL_ITEMS {
	DEM_command_Sent_NUM = 0,
	DEM_ARG_ICM_Idx,
	DEM_ARG_DCC_Idx,
	DEM_ARG_LC_Idx,
	DEM_ARG_LC_level,
	DEM_ARG_After_Filter_En,
	DEM_ARG_After_Filter_Val,	
	DEM_ARG_Blue_Stretch_level,
	DEM_ARG_Black_Extension_level,
	DEM_ARG_White_Extension_level,
	DEM_ARG_Data_Range,
	DEM_ARG_Gamma_exp_byOffset,
	DEM_ARG_10p_Gamma_Offset,
	DEM_ARG_Gamma_idx_set,
	DEM_ARG_Color_Space,
	DEM_ARG_D_3DLUT_Offset,
	DEM_ARG_Gamma_CURVE_TBL_PANEL_Chromaticity,
	DEM_ARG_LPF_Gain_ColorDataFac,
	DEM_ARG_OD_Gain,
	DEM_ARG_Dynamic_Black_Equalize_Gain,
	DEM_ARG_Shadow_detail_Gain,
	DEM_ARG_LowB_Gain,
	DEM_ARG_Smooth_Gradation,
	DEM_ARG_OD_TBL_set_by_idx_Sel,
	DEM_ARG_HDR_OETF_Curve_Ctrl,
	DEM_ARG_HDR_OETF_Setting_Ctrl,
	DEM_ARG_Shadow_Boot_Offset,
	DEM_ARG_Data_Range_sub,
	
	DEM_ARG_CTRL_ITEMS_Max,
} VIP_DEM_ARG_CTRL_ITEMS;

typedef enum _VIP_Color_Space_Target {
	Color_Space_Target_OFF = 0,
	Color_Space_Target_Auto,
	Color_Space_D3DLUTbyCS_Last_index = Color_Space_Target_Auto,
	Color_Space_Target_sRGB,
	Color_Space_Target_DCIP3,
	Color_Space_Target_AdobeRGB,
	Color_Space_Target_BT2020,

	// And14 blind mode
	Color_Space_Target_And14_Mono_BLIND_MODE,    // all   
	Color_Space_Target_And14_Deutera_BLIND_MODE, // green
	Color_Space_Target_And14_Prota_BLIND_MODE,   // red
	Color_Space_Target_And14_Trita_BLIND_MODE,   // blue
	
	VIP_DEM_Color_Space_Apply_MAX,
} VIP_Color_Space_Target;

typedef enum _VIP_Gamma_Curve_exp {
	Gamma_exp_1p8 = 0,
	Gamma_exp_1p9,
	Gamma_exp_2p0,
	Gamma_exp_2p1,
	Gamma_exp_2p2,
	Gamma_exp_2p3,
	Gamma_exp_2p4,
	Gamma_exp_2p5,
	Gamma_exp_2p6,
	
	VIP_Gamma_exp_p_max,
} VIP_Gamma_Curve_exp;

typedef enum _VIP_QS_PQ_curMode{
	VIP_QS_PQ_cur_UserMode = 0,
	VIP_QS_PQ_cur_PcMode,
	VIP_QS_PQ_cur_GameMode,
	VIP_QS_PQ_cur__max,
} VIP_QS_PQ_curMode;

typedef struct _VIP_DEM_ColorDataFac {	
	unsigned int LPF_Gain_0;
	unsigned int LPF_Gain_25;
	unsigned int LPF_Gain_50;
	unsigned int LPF_Gain_75;
	unsigned int LPF_Gain_100;
} VIP_DEM_ColorDataFac;

#define VIP_DEM_Dynamic_Black_Equalize_TBL_NUM 11
#define VIP_DEM_Shadow_Detail_TBL_NUM 11
#define VIP_DEM_LowB_Gain_NUM 101
#define VIP_DEM_Shadow_Detail_Boot_TBL_NUM 11

typedef struct _VIP_DEM_TBL {
	signed char LC_Level_Offset_Val[DCC_SELECT_MAX];
    unsigned char DEM_Black_Extent_Gain[DCC_SELECT_MAX];
	unsigned char DEM_White_Extent_Gain[DCC_SELECT_MAX];
	int DEM_10p_offset_TBL_R[VIP_DEM_10p_Gamma_Offset_Num+2];
	int DEM_10p_offset_TBL_G[VIP_DEM_10p_Gamma_Offset_Num+2];
	int DEM_10p_offset_TBL_B[VIP_DEM_10p_Gamma_Offset_Num+2];
	VIP_DEM_PANEL_INI_TBL PANEL_DATA_;
	unsigned int VPQ_COLORSPACE_INDEX[VIP_DEM_3D_LUT_Offset_Num];
	VIP_DEM_ColorDataFac ColorDataFac;
	short Color_Space_Target_Apply[VIP_DEM_Color_Space_Apply_MAX][3][3];
	DRV_OD_Gain OverDriver_Gain;
	unsigned int Dynamic_Black_Equalize_TBL[VIP_DEM_Dynamic_Black_Equalize_TBL_NUM];
	unsigned int Shadow_Detail_TBL[VIP_DEM_Shadow_Detail_TBL_NUM];
	unsigned int LowB_Gain_TBL[VIP_DEM_LowB_Gain_NUM];
	unsigned int Shadow_Detail_Boot_TBL[VIP_DEM_Shadow_Detail_Boot_TBL_NUM];
	
} VIP_DEM_TBL;

typedef enum _VIP_D3DLUTbyCS_Write_Mode {
	VIP_D3DLUTbyCS_setByIni6Axis = 0,
	VIP_D3DLUTbyCS_setByIniColorSpace,
	VIP_D3DLUTbyCS_setByFactory6Axis,
	VIP_D3DLUTbyCS_setByFactoryDecodeTBL,
	
	VIP_D3DLUTbyCS_Write_Mode_MAX,
} VIP_D3DLUTbyCS_Write_Mode;

typedef enum _VIP_D3DLUTbyCS_HEADER_ITEM {
    D3DLUTbyCS_HEADER_SIZE = 0,
    D3DLUTbyCS_HEADER_reserved1,
    D3DLUTbyCS_HEADER_reserved2,
    D3DLUTbyCS_HEADER_reserved3,
    D3DLUTbyCS_HEADER_reserved4,
    D3DLUTbyCS_HEADER_reserved5,
    D3DLUTbyCS_HEADER_reserved6,
    D3DLUTbyCS_HEADER_reserved7,
    D3DLUTbyCS_HEADER_reserved8,
    D3DLUTbyCS_HEADER_reserved9,
    D3DLUTbyCS_HEADER_reserved10,
    D3DLUTbyCS_HEADER_reserved11,
    D3DLUTbyCS_HEADER_reserved12,
	D3DLUTbyCS_HEADER_reserved13,
	D3DLUTbyCS_HEADER_reserved14,
	D3DLUTbyCS_HEADER_CRC16,
	
	D3DLUTbyCS_HEADER_Max,
} VIP_D3DLUTbyCS_HEADER_ITEM;

typedef struct _VIP_D3DLUTbyCS_6axis {
	//unsigned int D3DLUTbyCS_Header[D3DLUTbyCS_HEADER_Max];
	unsigned int index;
	unsigned int offset_6axis[VIP_DEM_3D_LUT_Offset_Num];	
} VIP_D3DLUTbyCS_6axis;

typedef struct _VIP_D3DLUTbyCS_6axis_TBL {
	//unsigned int D3DLUTbyCS_Header[D3DLUTbyCS_HEADER_Max];
	VIP_D3DLUTbyCS_6axis D3DLUTbyCS_6axis[VIP_DEM_Color_Space_Apply_MAX];
} VIP_D3DLUTbyCS_6axis_TBL;

typedef struct _VIP_D3DLUTbyCS_Decode_TBL {
	//unsigned int D3DLUTbyCS_Header[D3DLUTbyCS_HEADER_Max];
	GAMUT_3D_LUT_17x17x17_T D3DLUTbyCS_Decode_TBL[VIP_DEM_Color_Space_Apply_MAX];	
} VIP_D3DLUTbyCS_Decode_TBL;

typedef enum _VIP_APDEM_PTG_Pos_Mode {
	APDEM_PTG_Pos_byXY = 0,
	APDEM_PTG_Pos_Center,		
	APDEM_PTG_Pos_UpLeft,
	APDEM_PTG_Pos_UpRight,
	APDEM_PTG_Pos_BottomLeft,
	APDEM_PTG_Pos_BottomRight,

	VIP_APDEM_PTG_Pos_Info_MAX,
} VIP_APDEM_PTG_Pos_Mode;

typedef struct _VIP_APDEM_PTG_CTRL {
	int PTG_OnOff;
	int PTG_R_val;
	int PTG_G_val;
	int PTG_B_val;
	int PTG_Pos_Mode;
	int PTG_Size;	// 1-100%
	int x_sta;
	int y_sta;
	int x_end;
	int y_end;
	int mute_flag;
	int dis_flag;
} VIP_APDEM_PTG_CTRL;

typedef struct _VIP_VPQ_IOC_CT_DelaySet_CTRL 
{
	unsigned int Enable;

	unsigned int ct_idx;
	unsigned int ct_r_gain;
	unsigned int ct_g_gain;
	unsigned int ct_b_gain;
	unsigned int ct_r_offset;
	unsigned int ct_g_offset;
	unsigned int ct_b_offset;
	unsigned int Shadow_detail;
	unsigned int Gamma_byParameters_LowBlue;
	unsigned int Gamma_10p[VIP_DEM_10p_Gamma_Offset_Num*VIP_CHANNEL_RGB_MAX];
	unsigned int Gamma_byOffset;

	unsigned int ct_idx_active;
	unsigned int ct_rgb_gain_offset_active;
	unsigned int Shadow_detail_active;
	unsigned int Gamma_byParameters_LowBlue_active;
	unsigned int Gamma_10p_active;
	unsigned int Gamma_byOffset_active;

	unsigned int ct_idx_90k;
	unsigned int ct_rgb_gain_offset_90k;
	unsigned int Shadow_detail_90k;
	unsigned int Gamma_byParameters_LowBlue_90k;
	unsigned int Gamma_10p_90k;
	unsigned int Gamma_byOffset_90k;
	
} VIP_VPQ_IOC_CT_DelaySet_CTRL;

//========================= HDR_flag ========================
typedef enum VPQ_HDR_FLAG
{
	HDR_calman = 0,	     //calman
	HDR_normal,			 //normal (have maxcll)
	HDR_dummy,			 //dummy  (have maxcll, user define using case)
	HDR_withoutmetadata, //withoutmetadata (nomaxcll)
} VPQ_HDR_FLAG;

#if 0 // MOVE TO VIPCOMMON
typedef struct {
	unsigned int hdr_table_size;
	unsigned char hdr_table[HDR_TABLE_NUM][HDR_ITEM_MAX];
	unsigned int hdr_YUV2RGB[VIP_HDR_DM_CSC1_YUV2RGB_TABLE_Max][12];
	unsigned int hdr_RGB2OPT[HDR_RGB2OPT_TABLE_NUM][9];
	unsigned short HDR10_3DLUT_17x17x17[17*17*17][3];
	unsigned short HLG_3DLUT_17x17x17[17*17*17][3];
	unsigned int EOTF_LUT_R[1025];
	unsigned short OETF_LUT_R[1025];
	unsigned int HLG_EOTF_LUT_R[EOTF_size];
	unsigned short HLG_OETF_LUT_R[OETF_size];
	unsigned int HFC_EOTF_LUT_R[4][1025];
	unsigned short HFC_OETF_LUT_R[5][OETF_size];
	VIP_HDR10_OETF_ByLuminace_TBL HDR10_OETF_ByLuminace_TBL;
	VIP_TCHDR_Mode_TBL TCHDR_Mode_TBL;
	DRV_TCHDR_Table TCHDR_Table[TC_Table_NUM];
	DRV_TCHDR_COEF_I_lut TCHDR_COEF_I_LUT_TBL;
	DRV_TCHDR_COEF_S_lut TCHDR_COEF_S_LUT_TBL;
	DRV_TCHDR_v130_D_Table_settings TCHDR_v130_D_Table_settings[NUM_D_LUT_DEFAULT];
} SLR_HDR_TABLE;
#endif
#ifdef BUILD_QUICK_SHOW /*for quick_show mode */
typedef struct _PANEL_FILE_HEADER_NNSR {
	// table offset of each table
	int	iSize;
	int	iVipTableNNSROffset;
	int	iVersion;
	unsigned int iCRCValue;		// check value
} PANEL_FILE_HEADER_NNSR;

typedef struct _PANEL_TABLE_STRUCT_HEADER_NNSR {
	int				iSize;
	int				iTableArraySize;  // memory size
	int				iIndexTableSize;

	unsigned char	iTableType;		//PANEL_TABLE_TYPE

	unsigned char	iTableArrayLen; // array length
	unsigned char	iIndexTableLen;

	unsigned char	reserved;	// reserved to keep picmode tableLen
} PANEL_TABLE_STRUCT_HEADER_NNSR;


typedef struct _PANEL_TABLE_STRUCT_NNSR {
	PANEL_TABLE_STRUCT_HEADER_NNSR	header;
	void*	pTableArray;
	void*	pIndexTable;	//VIP_TABLE_DATA_STRUCT or VIP_TABLE_DATA_STRUCT_EX (pic mode table)

} PANEL_TABLE_STRUCT_NNSR;

typedef struct _PANEL_FILE_HEADER {
	// table offset of each table
	int	iSize;
	int	iYPPADCTableOffset;
	int	iVGAADCTableOffset;
	int	iColorDataOffset;
	int	iColorFacOffset;
	int	iPicModeOffset;
	int	iColorTempOffset;
	int	iVipTableOffset;
	int	iVipTableCustomOffset;
	int	iVipVerMiscOffset;
	int	iVersion;
	unsigned int iCRCValue;		// check value
} PANEL_FILE_HEADER;

typedef struct _PANEL_TABLE_STRUCT_HEADER {
	int				iSize;
	int				iTableArraySize;  // memory size
	int				iIndexTableSize;
	unsigned char	iTableType;		//PANEL_TABLE_TYPE
	unsigned char	iTableArrayLen; // array length
	unsigned char	iIndexTableLen;
	unsigned char	reserved;	// reserved to keep picmode tableLen
} PANEL_TABLE_STRUCT_HEADER;

typedef struct _PANEL_TABLE_STRUCT {
	PANEL_TABLE_STRUCT_HEADER	header;
	void*	pTableArray;
	void*	pIndexTable;	//VIP_TABLE_DATA_STRUCT or VIP_TABLE_DATA_STRUCT_EX (pic mode table)
} PANEL_TABLE_STRUCT;

typedef struct _VIP_VER_MISC_INFO //512 bytes
{
	char vipver[64];
	char CustomerName[64];
	char ProjectName[64];
	char ModelName[64];
	char PCBVersion[64];
	char reserved1[64];
	char reserved2[64];
	char Security[64];
} VIP_VER_MISC_INFO;

typedef struct _PANEL_FILE_HEADER_OD {
	// table offset of each table
	int	iSize;
	int	iVipTableICMOffset;
	int	iVipICMVerMiscOffset;
	int	iVersion;
	unsigned int iCRCValue;		// check value
} PANEL_FILE_HEADER_OD;

typedef struct _VIP_OD_VER_MISC_INFO //512 bytes
{
	char vipver[64];
	char CustomerName[64];
	char ProjectName[64];
	char ModelName[64];
	char PCBVersion[64];
	char reserved1[64];
	char reserved2[64];
	char Security[64];
} VIP_OD_VER_MISC_INFO;

typedef struct _PANEL_TABLE_STRUCT_HEADER_OD {
	int				iSize;
	int				iTableArraySize;  // memory size
	int				iIndexTableSize;
	unsigned char	iTableType;		//PANEL_TABLE_TYPE
	unsigned char	iTableArrayLen; // array length
	unsigned char	iIndexTableLen;
	unsigned char	reserved;	// reserved to keep picmode tableLen
} PANEL_TABLE_STRUCT_HEADER_OD;


typedef struct _PANEL_TABLE_STRUCT_OD {
	PANEL_TABLE_STRUCT_HEADER_OD	header;
	void*	pTableArray;
	void*	pIndexTable;	//VIP_TABLE_DATA_STRUCT or VIP_TABLE_DATA_STRUCT_EX (pic mode table)
} PANEL_TABLE_STRUCT_OD;

typedef struct _PANEL_FILE_HEADER_ICM {
	// table offset of each table
	int	iSize;
	int	iVipTableICMOffset;
	int	iVipICMVerMiscOffset;
	int	iVersion;
	unsigned int iCRCValue;		// check value
} PANEL_FILE_HEADER_ICM;

typedef struct _VIP_ICM_VER_MISC_INFO //512 bytes
{
	char vipver[64];
	char reserved[448];
} VIP_ICM_VER_MISC_INFO;


typedef struct _PANEL_TABLE_STRUCT_HEADER_ICM {
	int				iSize;
	int				iTableArraySize;  // memory size
	int				iIndexTableSize;
	unsigned char	iTableType;		//PANEL_TABLE_TYPE
	unsigned char	iTableArrayLen; // array length
	unsigned char	iIndexTableLen;
	unsigned char	reserved;	// reserved to keep picmode tableLen
} PANEL_TABLE_STRUCT_HEADER_ICM;


typedef struct _PANEL_TABLE_STRUCT_ICM {
	PANEL_TABLE_STRUCT_HEADER_ICM	header;
	void*	pTableArray;
	void*	pIndexTable;	//VIP_TABLE_DATA_STRUCT or VIP_TABLE_DATA_STRUCT_EX (pic mode table)
} PANEL_TABLE_STRUCT_ICM;

typedef struct _PANEL_FILE_HEADER_HDR{
    // table offset of each table
    int    iSize;
    int    iSLR_HDR_TABLEOffset;
    int    SLR_HDR_TABLE_length;
    int    iVipVerMiscOffset;
    int    VipVerMisc_length;
    int    iVersion;
    unsigned int iCRCValue;        // check value
} PANEL_FILE_HEADER_HDR;

typedef struct _PANEL_TABLE_STRUCT_HEADER_HDR {
    int              iSize;
    int              iTableArraySize;  // memory size
    int              iIndexTableSize;
    unsigned char    iTableType;        //PANEL_TABLE_TYPE
    unsigned char    iTableArrayLen; // array length
    unsigned char    iIndexTableLen;
    unsigned char    reserved;    // reserved to keep picmode tableLen
} PANEL_TABLE_STRUCT_HEADER_HDR;

typedef struct _PANEL_TABLE_STRUCT_HDR {
    PANEL_TABLE_STRUCT_HEADER_HDR    header;
    void*    pTableArray;
} PANEL_TABLE_STRUCT_HDR;

typedef struct _VIP_VER_MISC_INFO_HDR{ //512 bytes
	char vipver[64];
	char reserved[448];
} VIP_HDR_VER_MISC_INFO;
#endif

/*******************************************************************************
* Variable
******************************************************************************/
/*static unsigned char gExample = 100000;*/ /* 100K */



/*******************************************************************************
* Program
******************************************************************************/


/*o---------------------------------------------------------------------------o*/
/*o-------------Scalercolor.cpp------------------------------------o*/
/*o---------------------------------------------------------------------------o*/


/*==================== Definitions ================= */


typedef void *(*COLOR_CALLBACK_FUNC1)(VIP_SOURCE_TIMING source_timing);
typedef void *(*COLOR_CALLBACK_FUNC2)(unsigned int pcbSourceIndex, unsigned int timing, unsigned int param);
typedef int (*COLOR_CALLBACK_FUNC3)(void);
typedef unsigned char (*COLOR_CALLBACK_FUNC4)(unsigned char picMode);	/*Elsie 20131224*/
typedef void *(*COLOR_CALLBACK_FUNC5)(VIP_3DSOURCE_TIMING source_timing);

/*USER: Vincent_Lee  DATE: 2011/10/26  TODO: calcu sat by factory data*/
StructColorDataType *fwif_color_get_color_data(unsigned char src_idx, unsigned char timing);
StructColorDataFacModeType *fwif_color_get_color_fac_mode(unsigned char src_idx, unsigned char timing);
StructColorDataFacModeType *fwif_color_get_color_fac_mode_by_picture(unsigned char src_idx, unsigned char timing, unsigned char pic);

void *fwif_color_get_picture_mode_data(unsigned char src_idx, unsigned char value);
unsigned char fwif_color_get_picture_mode(unsigned char src_idx);


UINT8 fwif_vip_Get_Input_DataArrayIndex(UINT8 display);

unsigned char fwif_color_get_backlight(unsigned char src_idx);
void fwif_color_set_backlight(unsigned char src_idx, unsigned char value);
void fwif_color_set_backlight_TPV(unsigned char src_idx, unsigned char value);

unsigned char scaler_get_color_temp_level_type(unsigned char src_idx);
SLR_COLORTEMP_DATA *scaler_get_color_tempset(unsigned char src_idx, unsigned char timing);

unsigned char fwif_color_get_color_temp_level_type_using_colortmp(unsigned char colortmp);
void fwif_color_set_color_temp_level_type_using_colortmp(unsigned char colortmp, unsigned char value);

unsigned short fwif_color_get_color_temp_r_type_using_colortmp(unsigned char colortmp);
void fwif_color_set_color_temp_r_type_using_colortmp(unsigned char colortmp, unsigned short value);

unsigned short fwif_color_get_color_temp_g_type_using_colortmp(unsigned char colortmp);
void fwif_color_set_color_temp_g_type_using_colortmp(unsigned char colortmp, unsigned short value);

unsigned short fwif_color_get_color_temp_b_type_using_colortmp(unsigned char colortmp);
void fwif_color_set_color_temp_b_type_using_colortmp(unsigned char colortmp, unsigned short value);

unsigned short fwif_color_get_color_temp_r_offset_using_colortmp(unsigned char colortmp);
void fwif_color_set_color_temp_r_offset_using_colortmp(unsigned char colortmp, unsigned short value);
unsigned short fwif_color_get_color_temp_g_offset_using_colortmp(unsigned char colortmp);
void fwif_color_set_color_temp_g_offset_using_colortmp(unsigned char colortmp, unsigned short value);
unsigned short fwif_color_get_color_temp_b_offset_using_colortmp(unsigned char colortmp);
void fwif_color_set_color_temp_b_offset_using_colortmp(unsigned char colortmp, unsigned short value);




#if (defined (PQ_005_STYLE))/*yuan::20130624*/
unsigned char fwif_color_get_autoNR(unsigned char src_idx);/*yuan20130314::add for Auto_NR by picture mode*/
void fwif_color_set_autoNR(unsigned char src_idx, unsigned char value);
void fwif_color_tsb_set_NR_setting(unsigned char src_idx, unsigned char value);

#endif
unsigned char Scaler_get_i_dcti(void);
void Scaler_set_i_dcti(unsigned char src_idx, unsigned char value);

VIP_HDRCID_CTRL* Scaler_Get_VIP_HDRCID_CTRL(void);
VIP_HDRCID_INFO* Scaler_Get_VIP_HDRCID_INFO(void);
char Scaler_VIP_HDRCID_Table_update(unsigned short isPQbin_update, unsigned short* pRADCR_Pattern_BL_adj, unsigned short* pRADCR_Pattern_APL_th, 
	unsigned short* pRADCR_Pattern_APL_th_range, unsigned short* pRADCR_Pattern_Gain_adj_Offset, unsigned short* pRADCR_Pattern_Offset_adj_Offset);

unsigned char fwif_color_get_MpegNR_Level(unsigned char  src_idx);
void fwif_color_set_MpegNR_Level(unsigned char  src_idx, unsigned char  value);

unsigned char fwif_color_get_MpegNR_H(unsigned char  src_idx);
void fwif_color_set_MpegNR_H(unsigned char  src_idx, unsigned char  value);

unsigned char fwif_color_get_MpegNR_V(unsigned char  src_idx);
void fwif_color_set_MpegNR_V(unsigned char  src_idx, unsigned char  value);

unsigned char fwif_color_get_nrspatial(unsigned char src_idx);
void fwif_color_set_nrspatial(unsigned char src_idx, unsigned char value);

void fwif_color_set_SR_edge_gain(unsigned char src_idx, unsigned char value);
void fwif_color_set_SR_texture_gain(unsigned char src_idx, unsigned char value);
void fwif_color_set_SR_driver(unsigned char item_idx, unsigned char value);

void fwif_color_set_Vertical_NR_driver(unsigned char src_idx, unsigned char value);
void fwif_color_set_HMCNR_driver(unsigned char src_idx, unsigned char value);

void fwif_color_set_HMCNR_blending_driver(unsigned char src_idx, unsigned char value);

unsigned char fwif_color_get_dcc_level(unsigned char src_idx);
void fwif_color_set_dcc_level(unsigned char src_idx, unsigned char value, unsigned char SceneChange);
#if 0
void fwif_color_set_active_backlight_table(unsigned char src_idx, unsigned char item, unsigned char value);
#endif

void fwif_color_set_i_ccoring_driver(unsigned char src_idx, unsigned char value);

void fwif_color_set_d_ccoring_driver(unsigned char src_idx, unsigned char value);

unsigned char fwif_color_get_grid_remove(unsigned char src_idx);
void fwif_color_set_grid_remove(unsigned char src_idx, unsigned char value);

//unsigned char fwif_color_get_conti_still_line(unsigned char src_idx);
//void fwif_color_set_conti_still_line(unsigned char src_idx, unsigned char value);



unsigned char fwif_color_get_senselight(unsigned char src_idx);
void fwif_color_set_senselight(unsigned char src_idx, unsigned char value);

unsigned char fwif_color_get_spread_spectrum(unsigned char src_idx);
void fwif_color_set_spread_spectrum(unsigned char src_idx, unsigned char value);
void fwif_set_dcr_panasonic_apply(unsigned char value);
unsigned char fwif_get_dcr_panasonic(void);
void fwif_set_dcr_panasonic(unsigned char value);
unsigned char fwif_color_get_vip_debug_mode(unsigned char src_idx);
void fwif_color_set_vip_debug_mode(unsigned char src_idx, unsigned char value);

void *fwif_color_get_cur_picture_mode_data(unsigned char src_idx);

void fwif_set_ColorMatrix(bool value);


/*=========================================================*/


/*==================== Functions ================== */
/*void fwif_color_set_bg_color(unsigned char disp, unsigned char r, unsigned char g, unsigned char b);*/
/*void fwif_color_seticmhuesat(unsigned char display);*/

void fwif_color_setcontrastbrightness(unsigned char display);
#if defined(CONFIG_TTX_TOSHIBA_PATCH)
void fwif_color_setcontrastbrightness_half(unsigned char display, BOOL sRGB_User_Sat_En, BOOL sRGB_User_Sat_Mapping);
#endif


void fwif_color_rtnr_adjust(unsigned char channel, unsigned char osd_level);
/*void fwif_color_TSB_rtnr_y_adjust(unsigned char channel, unsigned char level);*/
void fwif_color_rtnr_of_snr_adjust(unsigned char channel, unsigned char level);
/*void fwif_color_TSB_rtnr_c_adjust(unsigned char channel, unsigned char level);*/
void fwif_color_VD_adjust(unsigned char channel, unsigned char level);

/*rock_rau	20130227 mark not use function*/ /**/
/*void fwif_color_noisereduction_spatial(unsigned char channel, unsigned char level, unsigned char bNoiseReductionMode);*/
void fwif_color_video_quality_handler_Sub(void);
void fwif_color_video_quality_handler(void);
void fwif_color_video_3Dmode_quality_handler(void);/*young 3Dtable*/

void fwif_color_sync_rtnr(void);
void fwif_color_sync_dnr(void);
unsigned char fwif_color_reg_callback1(COLOR_CALLBACK_TYPE type, COLOR_CALLBACK_FUNC1 cbFunc);
unsigned char fwif_color_reg_callback2(COLOR_CALLBACK_TYPE type, COLOR_CALLBACK_FUNC2 cbFunc);
unsigned char fwif_color_reg_callback3(COLOR_CALLBACK_TYPE type, COLOR_CALLBACK_FUNC3 cbFunc);
unsigned char fwif_color_reg_callback4(COLOR_CALLBACK_TYPE type, COLOR_CALLBACK_FUNC4 cbFunc);	/*Elsie 20131224*/
unsigned char fwif_color_reg_callback5(COLOR_CALLBACK_TYPE type, COLOR_CALLBACK_FUNC5 cbFunc);/*young 3Dtable*/
/*char fwif_color_isUSBSource(void);*/

unsigned char drvif_color_regRtnrycTable(unsigned int *ptr);    /*20100817 added by Leo Chen*/

void fwif_ByPass_IDomainToMem(void); /* adc dump*/

int fwif_color_get_cur_picMode(void); /* return PICTURE_MODE*/
//unsigned char fwif_vip_HDR10_check(unsigned char display, unsigned char input_src_type);
//unsigned char fwif_vip_DolbyHDR_check(unsigned char display, unsigned char input_src_type);
unsigned char fwif_vip_source_check(unsigned char display, unsigned char callmode);
unsigned char fwif_vip_3Dsource_check(unsigned char display, unsigned char callmode);/*young 3Dtable*/
void *fwif_color_get_VIP_Extend2_CoefArray(unsigned char src_idx);
//VIP_QUALITY_Extend3_Coef *fwif_color_get_VIP_Extend3_CoefArray(unsigned char src_idx);
//VIP_QUALITY_3Dmode_Coef *fwif_color_get_VIP_3Dmode_CoefArray(unsigned char src_idx);/*young 3Dtable*/
//VIP_QUALITY_Extend4_Coef *fwif_color_get_VIP_Extend4_CoefArray(unsigned char src_idx);

UINT16 COsdCtrlGet_R_Drive_RDRV(void);
UINT16 COsdCtrlGet_G_Drive_GDRV(void);
UINT16 COsdCtrlGet_B_Drive_BDRV(void);

void COsdCtrlSet_R_Drive_RDRV(UINT16  Message);
void COsdCtrlSet_G_Drive_GDRV(UINT16  Message);
void COsdCtrlSet_B_Drive_BDRV(UINT16  Message);

/*RGB cutoff base on 512 for cool, RGB offset = RGB cutoff + 512*/
INT8 COsdCtrlGet_R_CUTOFF_RCUT(void);
void COsdCtrlSet_R_CUTOFF_RCUT(UINT16  Message);
INT8 COsdCtrlGet_G_CUTOFF_GCUT(void);
void COsdCtrlSet_G_CUTOFF_GCUT(UINT16  Message);
INT8 COsdCtrlGet_B_CUTOFF_BCUT(void);
void COsdCtrlSet_B_CUTOFF_BCUT(UINT16  Message);

/*RGB drive offset for Natural*/
/*RGB drive for natural = RGB drive for cool + RGB drive offset for Natural*/
INT8 COsdCtrlGet_R_Drive_Offset_Natural_RDON(void);
void COsdCtrlSet_R_Drive_Offset_Natural_RDON(UINT16  Message);
INT8 COsdCtrlGet_G_Drive_Offset_Natural_GDON(void);
void COsdCtrlSet_G_Drive_Offset_Natural_GDON(UINT16  Message);
INT8 COsdCtrlGet_B_Drive_Offset_Natural_BDON(void);
void COsdCtrlSet_B_Drive_Offset_Natural_BDON(UINT16  Message);

/*RGB cutoff for natural = RGB cutoff for cool + RGB cutoff offset for natural*/
INT8 COsdCtrlGet_R_CUTOFF_Offset_Natural_RCON(void);
void COsdCtrlSet_R_CUTOFF_Offset_Natural_RCON(UINT16  Message);
INT8 COsdCtrlGet_G_CUTOFF_Offset_Natural_GCON(void);
void COsdCtrlSet_G_CUTOFF_Offset_Natural_GCON(UINT16  Message);
INT8 COsdCtrlGet_B_CUTOFF_Offset_Natural_BCON(void);
void COsdCtrlSet_B_CUTOFF_Offset_Natural_BCON(UINT16  Message);

/*RGB drive offset for Warm*/
/*RGB drive for Warm = RGB drive for cool + RGB drive offset for Warm*/
INT8 COsdCtrlGet_R_Drive_Offset_Warm_RDOW(void);
void COsdCtrlSet_R_Drive_Offset_Warm_RDOW(UINT16  Message);
INT8 COsdCtrlGet_G_Drive_Offset_Warm_GDOW(void);
void COsdCtrlSet_G_Drive_Offset_Warm_GDOW(UINT16  Message);
INT8 COsdCtrlGet_B_Drive_Offset_Warm_BDOW(void);
void COsdCtrlSet_B_Drive_Offset_Warm_BDOW(UINT16  Message);

/*RGB cutoff for Warm = RGB cutoff for cool + RGB cutoff offset for Warm*/
INT8 COsdCtrlGet_R_CUTOFF_Offset_Warm_RCOW(void);
void COsdCtrlSet_R_CUTOFF_Offset_Warm_RCOW(UINT16  Message);
INT8 COsdCtrlGet_G_CUTOFF_Offset_Warm_GCOW(void);
void COsdCtrlSet_G_CUTOFF_Offset_Warm_GCOW(UINT16  Message);
INT8 COsdCtrlGet_B_CUTOFF_Offset_Warm_BCOW(void);
void COsdCtrlSet_B_CUTOFF_Offset_Warm_BCOW(UINT16  Message);


void drvif_color_auto_timer_BrightnessBoost(void);
void drvif_color_VIP_Hist_Upadte(void);

void  Scaler_icm_enhancement(unsigned char value);
void  Scaler_ChannelColourSet(signed char value);

/*===add for haier 20111210 by lichun zhao======*/
#if defined(TV013UI_1) || defined(BUILD_TV013_1_ATV)
void fwif_set_color_management(unsigned char mode);
#endif


void fwif_color_3DPQ_enhance(unsigned char enable);


void Scaler_set_correcionBit1_duty_Debug(unsigned char value);

/*===================================================================================== */ /*DCC CSFC 20140127*/
/*              DCC*/

/*o---About histogram ----o*/
unsigned int Scaler_VIP_Hist_Upadte(void);

/*o--- source table----o*/
unsigned char Scaler_GetDCC_Table(void);
void Scaler_SetDCC_Table(unsigned char value);

/*o--- picture mode----o*/
unsigned char Scaler_GetDCC_Mode(void);
unsigned char  Scaler_SetDCC_Mode(unsigned char value);
unsigned char  Scaler_update_DCC_Mode_Sel(unsigned char value);	// only set DCC mode to SLR_PICTURE_MODE_DATA *pTable->DccMode
unsigned char Scaler_SetDCC_HAL_FreshContrast(unsigned char display , unsigned char uDCMode, UINT32 pstData);


/*o--- table cync. to video ---oo*/
unsigned char Scaler_Get_DCC_Boundary_Check_Table(void);
void Scaler_Set_DCC_Boundary_Check_Table(unsigned char value);
unsigned char Scaler_Get_DCC_Level_and_Blend_Coef_Table(void);
void Scaler_Set_DCC_Level_and_Blend_Coef_Table(unsigned char value);
void Scaler_Set_DCC_hist_adjust_table(unsigned char value);
unsigned char Scaler_Get_DCC_hist_adjust_table(void);
unsigned char Scaler_Get_DCC_database_Table(void);
void Scaler_Set_DCC_database_Table(unsigned char value);
unsigned char Scaler_Get_DCC_UserCurve_Table(void);
void Scaler_Set_DCC_UserCurve_Table(unsigned char value);
unsigned char Scaler_Get_DCC_AdaptCtrl_Level_Table(void);
void Scaler_Set_DCC_AdaptCtrl_Level_Table(unsigned char value);
unsigned char Scaler_Get_DCC_Picture_Mode_Weight(void);
void Scaler_Set_DCC_Picture_Mode_Weight(int value);

/*o---sync. level and coef base  to video ---o*/
unsigned char Scaler_GetDCL_W(void);
void Scaler_SetDCL_W(int value);
unsigned char Scaler_GetDCL_B(void);
void Scaler_SetDCC_B_extend(int value);
unsigned char Scaler_GetDCC_W_extend(void);
void Scaler_SetDCL_B(int value);
unsigned char Scaler_GetDCC_B_extend(void);
void Scaler_SetDCC_W_extend(int value);
unsigned char Scaler_Get_DCC_DelayTime(void);
void Scaler_Set_DCC_DelayTime(unsigned char flag);
unsigned char Scaler_Get_DCC_Step(void);
void Scaler_Set_DCC_Step(unsigned char flag);

/*o--- sync. table coef. that is non-adaptive table base API to video ---o*/
unsigned char Scaler_Get_Dcc_histogram_gain(unsigned char bin_num);
void Scaler_Set_Dcc_histogram_gain(unsigned char bin_num, unsigned char value, unsigned char SceneChange);
unsigned char Scaler_Get_Dcc_histogram_offset(unsigned char bin_num);
void Scaler_Set_Dcc_histogram_offset(unsigned char bin_num, unsigned char value, unsigned char SceneChange);
unsigned char Scaler_Get_Dcc_histogram_limit(unsigned char bin_num);
void Scaler_Set_Dcc_histogram_limit(unsigned char bin_num, unsigned char value, unsigned char SceneChange);

/*o--- non-adaptive table coef sync. when table change---o*/
void Scaler_set_dcc_sync_histogram_adjust_table(unsigned char src_idx, unsigned char Tabel_sel);/*for histogram adjus table*/

void Scaler_set_dcc_Sync_DCC_AP_Level(unsigned char src_idx, unsigned char Tabel_sel, unsigned char Mode_sel);/* for  DCC_Table change*/
void Scaler_set_dcc_Sync_DCC_Video_Driver_Level(unsigned char src_idx);/*for DCC DCC_Table change*/

UINT32 Scaler_get_DCC_AdapCtrl_Current_Mean(void);
UINT32 Scaler_get_DCC_AdapCtrl_Current_Dev(void);
VIP_DCC_AdaptCtrl_Level_Table *Scaler_get_DCC_AdapCtrl_Table(void);
unsigned char Scaler_get_DCC_AdapCtrl_DCC_Level(void);
BOOL Scaler_set_DCC_AdapCtrl_DCC_Level(UINT8 value);
unsigned char Scaler_get_DCC_AdapCtrl_Cubic_index(void);
BOOL Scaler_set_DCC_AdapCtrl_Cubic_index(UINT8 value);
unsigned char Scaler_get_DCC_AdapCtrl_Cubic_low(void);
BOOL Scaler_set_DCC_AdapCtrl_Cubic_low(UINT8 value);
unsigned char Scaler_get_DCC_AdapCtrl_Cubic_high(void);
BOOL Scaler_set_DCC_AdapCtrl_Cubic_high(UINT8 value);


/*o--- sync. table coef. that is adaptive by vide fw ---o*/

#if 0
/* mark those API, because control method change to tool control*/
unsigned char Scaler_GetDccLevel(void);
void Scaler_SetDccLevel(unsigned char value, unsigned char SceneChange);
unsigned char Scaler_GetDCC_Shigh(void);
void Scaler_SetDCC_Shigh(int value);
unsigned char Scaler_GetDCC_Slow(void);
void Scaler_SetDCC_Slow(int value);
unsigned char Scaler_GetDCC_Sindex(void);
void Scaler_SetDCC_Sindex(int value);
#endif

/*===================================================================================== */ /*DCC CSFC 20140128*/

/*===================================================================================== */ /*ConBri CSFC 20140128*/
#ifdef OSD_FOUR_LEVEL_MAPPING_ENABLE
unsigned char Scaler_GetFacBrightness(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100,
	unsigned char *retval_25, unsigned char *retval_75);
unsigned char Scaler_SetFacBrightness(unsigned char val_0, unsigned char val_50, unsigned char val_100,
	unsigned char val_25, unsigned char val_75);
unsigned char Scaler_GetFacContrast(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100,
	unsigned char *retval_25, unsigned char *retval_75);
unsigned char Scaler_SetFacContrast(unsigned char val_0, unsigned char val_50, unsigned char val_100,
	unsigned char val_25, unsigned char val_75);
#else
unsigned char Scaler_GetFacBrightness(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100);
unsigned char Scaler_SetFacBrightness(unsigned char val_0, unsigned char val_50, unsigned char val_100);
unsigned char Scaler_GetFacContrast(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100);
unsigned char Scaler_SetFacContrast(unsigned char val_0, unsigned char val_50, unsigned char val_100);
#endif

/* for tv 020 030*/
unsigned char Scaler_OsdMapToRegValue_Contrast(unsigned char display , unsigned char value);
unsigned char Scaler_OsdMapToRegValue_Brightness(unsigned char display , unsigned char value);

/*===================================================================================== */ /*ConBri CSFC 20140128*/

/*======================================================== hue /sat*/
unsigned char Scaler_GetIntensity(void);
void Scaler_SetIntensity(unsigned char value);

/* for tv 020 030*/
unsigned char Scaler_OsdMapToRegValue_Saturation(unsigned char display , unsigned char value);
unsigned char Scaler_OsdMapToRegValue_Hue(unsigned char display , unsigned char value);

#ifdef OSD_FOUR_LEVEL_MAPPING_ENABLE
unsigned char Scaler_GetFacSaturation(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100,
	unsigned char *retval_25, unsigned char *retval_75);
unsigned char Scaler_SetFacSaturation(unsigned char val_0, unsigned char val_50, unsigned char val_100,
	unsigned char val_25, unsigned char val_75);
unsigned char Scaler_GetFacHue(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100,
	unsigned char *retval_25, unsigned char *retval_75);
unsigned char Scaler_SetFacHue(unsigned char val_0, unsigned char val_50, unsigned char val_100,
	unsigned char val_25, unsigned char val_75);
#else
unsigned char Scaler_GetFacSaturation(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100);
unsigned char Scaler_SetFacSaturation(unsigned char val_0, unsigned char val_50, unsigned char val_100);
unsigned char Scaler_GetFacHue(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100);
unsigned char Scaler_SetFacHue(unsigned char val_0, unsigned char val_50, unsigned char val_100);
#endif

/*===================================================================================== */ /*PictureMode CSFC 20140128*/
/*void Scaler_SetPictureInfo(SCALER_DISP_CHANNEL display, SLR_PICTURE_INFO infoList, unsigned char value);*/
/*unsigned char Scaler_GetPictureInfo(SCALER_DISP_CHANNEL display, SLR_PICTURE_INFO  infoList);*/

void Scaler_ResetPictureQuality(SCALER_DISP_CHANNEL display);
/*void *Scaler_get_cur_picture_mode_data(unsigned char src_idx);*/
/*void *Scaler_get_picture_mode_data(unsigned char src_idx, unsigned char value);*/
unsigned char Scaler_apply_PQ_Linear(unsigned char Enable);	// for AT only. not in normal flow

void Scaler_RefreshPictureMode_sub(void);
void Scaler_RefreshPictureMode(void);
void Scaler_Set_PQ_RGB444MODE(unsigned char path);
unsigned char Scaler_VPQ_check_PC_RGB444(unsigned char display);
/*===================================================================================== */ /*ColorTemp CSFC 20140128*/
unsigned short Scaler_get_color_temp_r_type_using_colortmp(unsigned char colortmp);
void Scaler_set_color_temp_r_type_using_colortmp(unsigned char colortmp, unsigned short value);

unsigned short Scaler_get_color_temp_g_type_using_colortmp(unsigned char colortmp);
void Scaler_set_color_temp_g_type_using_colortmp(unsigned char colortmp, unsigned short value);

unsigned short Scaler_get_color_temp_b_type_using_colortmp(unsigned char colortmp);
void Scaler_set_color_temp_b_type_using_colortmp(unsigned char colortmp, unsigned short value);

unsigned short Scaler_get_color_temp_r_offset_using_colortmp(unsigned char colortmp);
void Scaler_set_color_temp_r_offset_using_colortmp(unsigned char colortmp, unsigned short value);
unsigned short Scaler_get_color_temp_g_offset_using_colortmp(unsigned char colortmp);
void Scaler_set_color_temp_g_offset_using_colortmp(unsigned char colortmp, unsigned short value);
unsigned short Scaler_get_color_temp_b_offset_using_colortmp(unsigned char colortmp);
void Scaler_set_color_temp_b_offset_using_colortmp(unsigned char colortmp, unsigned short value);

unsigned short Scaler_get_color_temp_r_type(unsigned char src_idx);
void Scaler_set_color_temp_r_type(unsigned char src_idx, unsigned short value);

unsigned short Scaler_get_color_temp_g_type(unsigned char src_idx);
void Scaler_set_color_temp_g_type(unsigned char src_idx, unsigned short value);

unsigned short Scaler_get_color_temp_b_type(unsigned char src_idx);
void Scaler_set_color_temp_b_type(unsigned char src_idx, unsigned short value);

unsigned short Scaler_get_color_temp_r_offset(unsigned char src_idx);
void Scaler_set_color_temp_r_offset(unsigned char src_idx, unsigned short value);
unsigned short Scaler_get_color_temp_g_offset(unsigned char src_idx);
void Scaler_set_color_temp_g_offset(unsigned char src_idx, unsigned short value);
unsigned short Scaler_get_color_temp_b_offset(unsigned char src_idx);
void Scaler_set_color_temp_b_offset(unsigned char src_idx, unsigned short value);

/*===================================================================================== */ /*ColorTemp CSFC 20140128*/


void Scaler_set_picmode_init_value(unsigned char item_index, unsigned short value);
int Scaler_set_picmode_init_sendRPC(void);



/*O================================================*/
/*O================================================*/
/*O================================================*/
unsigned short Scaler_GetICMTable(void);/* csfanchiang, 20101101*/

void Scaler_SetICMTable(unsigned char value);/* csfanchiang, 20101101*/

void Scaler_set_ICM_table_by_VipTable(void);


/*rock_rau	20130227 mark not use function*/ /**/
/*void Scaler_SetNRSpatial(unsigned char value);					*/ /*range 0~16*/
/*void Scaler_SetNoiseEstimation(unsigned char mode);			*/ /*range 0~1*/


/*void Scaler_SetDccMode(unsigned char value);*/


void Scaler_SetICMenhancement(unsigned char value);
unsigned char Scaler_GetGammaDebug(void);  /*add by Diane*/
void Scaler_SetGammaDebug(unsigned char value);						/*range 0~10  */ /*add by Diane*/
unsigned char Scaler_GetNormalWarm_80IRE(SLR_COLORTEMP_LEVEL colorTemp, int channel); /*add by Diane*/
unsigned char Scaler_GetNormalWarm_20IRE(SLR_COLORTEMP_LEVEL colorTemp, int channel);/*add by Diane*/
unsigned char Scaler_GetGainRed(void);   /*add by Diane*/
unsigned char Scaler_GetGainGreen(void);   /*add by Diane*/
unsigned char Scaler_GetGainBlue(void);   /*add by Diane*/
unsigned char Scaler_GetCenterRed(void);   /*add by Diane*/
unsigned char Scaler_GetCenterGreen(void);   /*add by Diane*/
unsigned char Scaler_GetCenterBlue(void);   /*add by Diane*/
unsigned char Scaler_GetInternalPattern(void);  /*add by Diane*/
void  Scaler_SetGainRed(unsigned char value);                                              /* range 0~255  */ /*add by Diane*/
void Scaler_SetGainGreen(unsigned char value);                                              /* range 0~255  */ /*add by Diane*/
void Scaler_SetGainBlue(unsigned char value);                                              /* range 0~255  */ /*add by Diane*/
void Scaler_SetCenterRed(unsigned char value);                                              /* range 0~255  */ /*add by Diane*/
void Scaler_SetCenterGreen(unsigned char value);                                              /* range 0~255  */ /*add by Diane*/
void Scaler_SetCenterBlue(unsigned char value);                                              /* range 0~255  */ /*add by Diane*/
void Scaler_SetInternalPattern(unsigned char value);						/*range 0~1  */ /*add by Diane*/

/*ravi_li add*/
#if defined BUILD_TV005_2
void Scaler_Set3DColorMatrix(BASECOLORADJ *colortable);
BASECOLORADJ *Scaler_Get3DColorMatrix(void);
#endif
/*end ravi_li*/
void Scaler_color_HFC_set_TC_FrameSync_Enable(UINT8 enable);

void scaler_set_PQA_Input_table(unsigned char value);
void scaler_set_PQA_table(unsigned char value);
unsigned char scaler_get_PQA_Input_table(void);
unsigned char scaler_get_PQA_table(void);

void Scaler_SetTableNRSpatial(unsigned char value);
unsigned char Scaler_GetTableNRSpatial(void);

unsigned char  Scaler_GetDNR_table(void);
/*void Scaler_SetDNR_table(unsigned char value, unsigned char calledByOSD=CALLED_NOT_BY_OSD);	*/ /* move "DNR table select info" to system_info_struct and replace by PQA table, elieli*/

void Scaler_SetDNR(unsigned char value);
unsigned char Scaler_GetDNR(void);
void Scaler_SetMPEGNR(unsigned char value, unsigned char calledByOSD);
unsigned char Scaler_GetMPEGNR(void);
DRV_RTNR_General_ctrl *Scaler_GetRTNR_General(void);
void Scaler_SetRTNR_Y(unsigned char value);
unsigned char Scaler_GetRTNR_Y(void);
void Scaler_SetRTNR_C(unsigned char value);
unsigned char Scaler_GetRTNR_C(void);
void Scaler_SetNRSpatialY(unsigned char value);
unsigned char Scaler_GetNRSpatialY(void);
void Scaler_SetNRSpatialC(unsigned char value);
unsigned char Scaler_GetNRSpatialC(void);
void Scaler_SetMpegNR_H(unsigned char value);
unsigned char Scaler_GetMpegNR_H(void);
void Scaler_SetMpegNR_V(unsigned char value);
unsigned char Scaler_GetMpegNR_V(void);
void Scaler_SetNRTemporal(unsigned char value);
unsigned char Scaler_GetNRTemporal(void);

#if defined (PQ_005_STYLE)
void Scaler_SetAutoNR(unsigned char value);
unsigned char Scaler_GetAutoNR(void);
#endif

void Scaler_OPC_Auto_Backlight_Adjust(void);

unsigned char drvif_vd_3d_ntsc_table(unsigned char *ptr);
unsigned char drvif_vd_3d_pal_table(unsigned char *ptr);
/*[option]*/
/*rock_rau	20130227 mark not use function*/ /**/
/*unsigned char Scaler_GetNRSpatial(void);*/
void Scaler_SetFacPatternColor(unsigned char val);

#ifdef ENABLE_SPEED_UP_SLR_DISPLAY_ON_FLOW
void Scaler_ResetPictureMode(void);
void Scaler_EnablePictureModeReset(void);
#endif

unsigned char Scaler_GetFacPatternColor(void);

void Scaler_SetNewRTNR_SAD_Mix_Weight(unsigned char value);

void Scaler_SetAutoMAFlag(unsigned char flag);
void Scaler_Set_Open_Cell_Panel_UseOD(unsigned char flag);
void Scaler_Set_YC2D3D(unsigned char flag);
void Scaler_Set_ChannelColourSet(char flag);
void Scaler_Set_ColorMatrix(bool flag);

unsigned char Scaler_Get_NrAvgcount(void);  /*yuan1024*/
void Scaler_SetAutoMA(unsigned char Flag, unsigned char value, unsigned char sendRPC_flag);	/*Elsie 20131210*/

void Scaler_recheck_sharp_nr_table(void);




/*o===========================================================o*/
/*o=================== Enginner Menu Start ===================o*/
/*o===========================================================o*/

unsigned char Scaler_GetScaleDOWNH(void);
void Scaler_SetScaleDOWNH(unsigned char value);					/*range 0~17*/
unsigned char Scaler_GetScaleDOWNV(void);
void Scaler_SetScaleDOWNV(unsigned char value);					/*range 0~17*/

unsigned char Scaler_GetScaleDOWNVTable(void);
void Scaler_SetScaleDOWNVTable(unsigned char value);				/*range 0~7*/
unsigned short Scaler_GetH8Scaling(void);
void Scaler_SetH8Scaling(unsigned char value);
unsigned char Scaler_GetPacificNewIntra(void);
void Scaler_SetPacificNewIntra(unsigned char value);
unsigned short Scaler_GetICcoring(void);
void Scaler_SetICcoring(unsigned char value);
unsigned short Scaler_GetDCcoring(void);
void Scaler_SetDCcoring(unsigned char value);
unsigned char Scaler_GetVertical_NR(void);
void Scaler_SetVertical_NR(unsigned char value);
unsigned char Scaler_GetHMCNR(void);
void Scaler_SetHMCNR(unsigned char value);
unsigned char Scaler_GetHMCNR_blending(void);
void Scaler_SetHMCNR_blending(unsigned char value);
unsigned char Scaler_GetMBPeaking(void);
void Scaler_SetMBPeaking(unsigned char value);					/*range 0~40*/
unsigned char Scaler_GetMBSUPeaking(void);
void Scaler_SetMBSUPeaking(unsigned char value);					/*range 0~40*/
unsigned short Scaler_GetSharpnessVerticalGain(void);
void Scaler_SetSharpnessVerticalGain(unsigned char value);
unsigned short  Scaler_Getharpness2Dpk_Seg(void);
void Scaler_SetSharpness2Dpk_Seg(unsigned char value);
unsigned short  Scaler_GetSharpness2Dpk_Tex_Detect(void);
void Scaler_SetSharpness2Dpk_Tex_Detect(unsigned char value);
unsigned char Scaler_GetMADI_BTR(void);
void Scaler_SetMADI_BTR(unsigned char value);
unsigned char Scaler_GetMADI_BER(void);
void Scaler_SetMADI_BER(unsigned char value);
unsigned char Scaler_GetGridRemove(void);
void Scaler_SetGridRemove(unsigned char value);
unsigned char Scaler_GetContiStillLine(void);
void Scaler_SetContiStillLine(unsigned char value);

void Scaler_Set_TVAutoNR_Mode(unsigned char flag);
unsigned char Scaler_GetAutoMA1Flag(void);
void Scaler_SetAutoMA1Flag(unsigned char flag);
unsigned char Scaler_GetAutoMA2Flag(void);
void Scaler_SetAutoMA2Flag(unsigned char flag);
unsigned char Scaler_GetAutoMA3Flag(void);
void Scaler_SetAutoMA3Flag(unsigned char flag);
unsigned char Scaler_GetAutoMA4Flag(void);
void Scaler_SetAutoMA4Flag(unsigned char flag);
unsigned char Scaler_GetAutoMA5Flag(void);
void Scaler_SetAutoMA5Flag(unsigned char flag);
unsigned char Scaler_GetAutoMA6Flag(void);
void Scaler_SetAutoMA6Flag(unsigned char flag);
unsigned char Scaler_GetAutoMAFlag(void);
void Scaler_SetAutoMA_Flag(void);
unsigned char Scaler_GetAutoMA_VD_3D(void);
void Scaler_SetAutoMA_VD_3D(unsigned char flag);
/*unsigned char Scaler_Get_NR_style(void);*/
void Scaler_Set_NR_style(unsigned char flag, unsigned char calledByOSD);
unsigned char Scaler_GetDNR_log(void);
void Scaler_SetDNR_log(unsigned char flag);
unsigned char Scaler_GetRTNR_log(void);
void Scaler_SetRTNR_log(unsigned char flag);
unsigned char Scaler_GetSNR_log(void);
void Scaler_SetSNR_log(unsigned char flag);
unsigned char Scaler_GetINR_log(void);
void Scaler_SetINR_log(unsigned char flag);
unsigned char Scaler_Get_DEBUG_AUTO_NR(void);
void Scaler_Set_DEBUG_AUTO_NR(unsigned char flag);
unsigned char Scaler_Get_DEBUG_VD_NOISE(void);
void Scaler_Set_DEBUG_VD_NOISE(unsigned char flag);
unsigned char Scaler_Get_DEBUG_RTNR_MAD(void);
void Scaler_Set_DEBUG_RTNR_MAD(unsigned char flag);
unsigned char Scaler_Get_DEBUG_HISTOGRAM_MEAN(void);
void Scaler_Set_DEBUG_HISTOGRAM_MEAN(unsigned char flag);

void Scaler_SetVIP_Dbg_Msg(unsigned char flag);
unsigned char Scaler_Get_TVAutoNR_Mode(void);
unsigned char Scaler_GetVIP_Dbg_Msg(void);

/*NR_log*/
INT8 COsdCtrlGet_DNR_log(void);
void COsdCtrlSet_DNR_log(UINT16  Message);
INT8 COsdCtrlGet_RTNR_log(void);
void COsdCtrlSet_RTNR_log(UINT16  Message);
INT8 COsdCtrlGet_SNR_log(void);
void COsdCtrlSet_SNR_log(UINT16  Message);
INT8 COsdCtrlGet_INR_log(void);
void COsdCtrlSet_INR_log(UINT16  Message);
INT8 COsdCtrlGet_MPEGNR_log(void);
void COsdCtrlSet_MPEGNR_log(UINT16  Message);


/*AUTO_NR_DEBUG*/
INT8 COsdCtrlGet_DEBUG_AUTO_NR(void);
void COsdCtrlSet_DEBUG_AUTO_NR(UINT16  Message);
INT8 COsdCtrlGet_DEBUG_VD_NOISE(void);
void COsdCtrlSet_DEBUG_VD_NOISE(UINT16  Message);
INT8 COsdCtrlGet_DEBUG_RTNR_MAD(void);
void COsdCtrlSet_DEBUG_RTNR_MAD(UINT16  Message);
INT8 COsdCtrlGet_DEBUG_HISTOGRAM_MEAN(void);
void COsdCtrlSet_DEBUG_HISTOGRAM_MEAN(UINT16  Message);

/*o===========================================================o*/
/*o=================== Enginner Menu End =====================o*/
/*o===========================================================o*/


/*o===========================================================o*/
/*o==================== OSD MENU Start =======================o*/
/*o===========================================================o*/




/*
void Scaler_GetFacBrightness(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100);
void Scaler_GetFacContrast(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100);
void Scaler_GetFacSaturation(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100);
void Scaler_SetFacSaturation(unsigned char val_0, unsigned char val_50, unsigned char val_100);
void Scaler_GetFacHue(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100);
void Scaler_SetFacHue(unsigned char val_0, unsigned char val_50, unsigned char val_100);
*/

#ifdef OSD_FOUR_LEVEL_MAPPING_ENABLE
unsigned char Scaler_GetFacSharpness(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100,
	unsigned char *retval_25, unsigned char *retval_75);
unsigned char Scaler_SetFacSharpness(unsigned char val_0, unsigned char val_50, unsigned char val_100,
	unsigned char val_25, unsigned char val_75);
unsigned char Scaler_GetFacBacklight(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100,
	unsigned char *retval_25, unsigned char *retval_75);
unsigned char Scaler_SetFacBacklight(unsigned char val_0, unsigned char val_50, unsigned char val_100,
	unsigned char val_25, unsigned char val_75);
#else
unsigned char Scaler_GetFacSharpness(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100);
unsigned char Scaler_SetFacSharpness(unsigned char val_0, unsigned char val_50, unsigned char val_100);
unsigned char Scaler_GetFacBacklight(unsigned char *retval_0, unsigned char *retval_50, unsigned char *retval_100);
unsigned char Scaler_SetFacBacklight(unsigned char val_0, unsigned char val_50, unsigned char val_100);
#endif
/*o===========================================================o*/
/*o==================== OSD MENU End = =======================o*/
/*o===========================================================o*/
unsigned char Scaler_APDEM_En_Access(unsigned char en, unsigned char isSetFlag);
unsigned char Scaler_APDEM_Arg_Access(VIP_DEM_ARG_CTRL_ITEMS ID, unsigned char val, unsigned char isSetFlag);
int Scaler_set_APDEM_CM_flow_Polling_TASK(void);
int Scaler_set_APDEM(VIP_DEM_ARG_CTRL_ITEMS ID, void *DEM_arg);
unsigned char Scaler_APDEM_OsdMapToRegValue(VIP_DEM_ARG_CTRL_ITEMS ID, unsigned char value);
char Scaler_APDEM_PTG_set(VIP_APDEM_PTG_CTRL *PTG_CTRL);
char Scaler_Set_ColorSpace_D_3dlut_APDEM(unsigned char writeMode);

#if 0  /* sirius not used*/

void Scaler_SetSceneMode(unsigned char level);				/*range 0~4*/

unsigned short  Scaler_color_GetTwoD_peaking_AdaptiveCtrl(void);

void Scaler_color_SetTwoD_peaking_AdaptiveCtrl(unsigned char value);

unsigned char Scaler_color_GetDCDI(void);

void Scaler_color_SetDCDI(unsigned char mode);

unsigned char Scaler_GetChrommaErr(void);
void Scaler_SetChrommaErr(unsigned char value);					/*range 0~8*/

unsigned char Scaler_color_GetVIP_Debug_Mode(void);
void Scaler_color_SetVIP_Debug_Mode(unsigned char value);

unsigned char Scaler_GetHist(void);
void Scaler_SetHist(unsigned char mode);

unsigned char Scaler_Get_NrAvgcount(void);
void Scaler_Set_NrAvgcount(unsigned char flag);/*yuan1024*/

#if defined (PQ_005_STYLE)
/*20130706 added by Diane for Dynamic Peaking Display*/
unsigned char Scaler_Get_RTNR_MAD_count_Y_sum_avg(void);
unsigned char Scaler_Get_DCC_Histogram_Mean_value(void);
void Scaler_Get_Dynamic_Peaking_Value_Display(unsigned char value[3]);
#endif

/*PQ Enable_DEBUG*/
INT8 COsdCtrlGet_DEBUG_DCTI_EN(void);
void COsdCtrlSet_DEBUG_DCTI_EN(UINT16  Message);
INT8 COsdCtrlGet_DEBUG_SHARP_EN(void);
void COsdCtrlSet_DEBUG_SHARP_EN(UINT16  Message);
INT8 COsdCtrlGet_DEBUG_EDGE_SMOOTH_EN(void);
void COsdCtrlSet_DEBUG_EDGE_SMOOTH_EN(UINT16  Message);
INT8 COsdCtrlGet_DEBUG_DCC_EN(void);
void COsdCtrlSet_DEBUG_DCC_EN(UINT16  Message);
INT8 COsdCtrlGet_DEBUG_ICM_EN(void);
void COsdCtrlSet_DEBUG_ICM_EN(UINT16  Message);
INT8 COsdCtrlGet_DEBUG_SRGB_EN(void);
void COsdCtrlSet_DEBUG_SRGB_EN(UINT16  Message);
INT8 COsdCtrlGet_DEBUG_GAMMA_EN(void);
void COsdCtrlSet_DEBUG_GAMMA_EN(UINT16  Message);
INT8 COsdCtrlGet_DEBUG_DITHER_EN(void);
void COsdCtrlSet_DEBUG_DITHER_EN(UINT16  Message);
INT8 COsdCtrlGet_DEBUG_DCR_EN(void);
void COsdCtrlSet_DEBUG_DCR_EN(UINT16  Message);
#if defined(IS_TV05_STYLE_PICTUREMODE)
#if defined (BUILD_TV005_1_ISDB)
INT8 COsdCtrlGet_DEBUG_DCR_OFFSET(void);
void COsdCtrlSet_DEBUG_DCR_OFFSET(UINT16  Message);
#endif
INT8 COsdCtrlGet_DEBUG_MEGA_DCR_EN(void);
void COsdCtrlSet_DEBUG_MEGA_DCR_EN(UINT16  Message);
INT16 COsdCtrlGet_DEBUG_MEGA_DCR_COUNT(void);
void COsdCtrlSet_DEBUG_MEGA_DCR_COUNT(UINT16  Message);
INT8 COsdCtrlGet_DEBUG_MEGA_DCR_WEIGHT(void);
void COsdCtrlSet_DEBUG_MEGA_DCR_WEIGHT(UINT16  Message);
#endif


#endif


/* for load quality handle*/

void Scaler_Get_Display_info(unsigned char *display, unsigned char *src_idx);

void Scaler_VIP_IP_init(unsigned char Vip_source);

void Scaler_VIP_MEMC_init(unsigned char Vip_source);

void SE_FilmDtect_Init(void);

void Scaler_VIP_Project_ID_init(void);

//unsigned char Scaler_GetMA_Chroma_Error(void);

//void Scaler_SetMA_Chroma_Error(unsigned char value);

//unsigned char Scaler_GetMA_Chroma_Error_En_From_vipTable(void);

unsigned char Scaler_GetMADI_HMC(void);

void Scaler_SetMADI_HMC(unsigned char value);

unsigned char Scaler_GetMADI_HME(void);

void Scaler_SetMADI_HME(unsigned char value);

unsigned char Scaler_GetMADI_PAN(void);

void Scaler_SetMADI_PAN(unsigned char value);

unsigned char Scaler_GetNew_UVC(void);

void Scaler_SetNew_UVC(unsigned char value);

unsigned char Scaler_GetDLti(void);

void Scaler_SetDLti(unsigned char value);						/*range 0~10*/

unsigned char Scaler_GetDCti(void);

void Scaler_SetDCti(unsigned char value);						/*range 0~10*/

unsigned char Scaler_GetIDLti(void);

void Scaler_SetIDLti(unsigned char value);

unsigned char Scaler_GetIDCti(void);

void Scaler_SetIDCti(unsigned char value);

unsigned char Scaler_GetVDCti(void);

void Scaler_SetVDCti(unsigned char value);

void Scaler_SetVDCti_LPF(unsigned char value);
unsigned char Scaler_GetVDCti_LPF(void);

unsigned char Scaler_GetColor_UV_Delay_Enable(void);

void Scaler_SetColor_UV_Delay_Enable(unsigned char on_off);

unsigned char Scaler_GetColor_UV_Delay(void);

void Scaler_SetColor_UV_Delay(unsigned char value);
void Scaler_SetColor_UV_Delay_TOP(unsigned char value);



unsigned char Scaler_GetFilm_Mode(void);

void Scaler_SetFilm_Mode(unsigned char value);

void Scaler_SetFilmonoff(unsigned char value);

void Scaler_SetFilm_Setting(void);


unsigned char Scaler_GetIntra(void);

void Scaler_SetIntra(unsigned char value);	/*range 0~11*/

unsigned char Scaler_GetMADI(void);

void Scaler_SetMADI(unsigned char value);
void Scaler_SetMADI_INIT(void);

#if 0
unsigned char Scaler_GetTNRXC(void);
void Scaler_SetTNRXC(unsigned char value);
unsigned char Scaler_GetTNRXC_Mk2(void);
void Scaler_SetTNRXC_Mk2(unsigned char value);
unsigned char fwif_color_get_tnrxc_mk2(unsigned char src_idx);
void fwif_color_set_tnrxc_mk2(unsigned char src_idx, unsigned char value);

#else
unsigned char Scaler_GetTNRXC_Ctrl(void);
void Scaler_SetTNRXC_Ctrl(unsigned char value);

unsigned char Scaler_GetTNRXC_MK2(void);
void Scaler_SetTNRXC_MK2(unsigned char value);

void Scaler_SetTNRXC(unsigned char value);
#endif

void Scaler_SetDataFormatHandler(unsigned char display);

unsigned char Scaler_get_YUV2RGB(void);

void Scaler_set_YUV2RGB(unsigned char which_table, unsigned char drvSkip_Flag_coefByY, unsigned char drvSkip_Flag_uvOffset);
#if 0
unsigned char Scaler_get_YUV2RGB_COEF_BY_Y(void);

void Scaler_set_YUV2RGB_COEF_BY_Y(unsigned char which_table, unsigned char drvSkip_Flag);

unsigned char Scaler_get_YUV2RGB_UV_OFFSET_BY_Y(void);

void Scaler_set_YUV2RGB_UV_OFFSET_BY_Y(unsigned char which_table, unsigned char drvSkip_Flag);
#endif
/*signed char Scaler_GetCrossColor(void);*/

/*void Scaler_SetCrossColor(unsigned char value);*/ /*range 0~2*/



void Scaler_module_vdc_SetConBriHueSat_Table(unsigned char value);
unsigned char Scaler_module_vdc_GetConBriHueSat_Table(void);



unsigned char Scaler_get_ICM_table(void);

void Scaler_set_ICM_table(unsigned char value, unsigned char drvSkip_Flag);

unsigned char Scaler_GetGamma(void);

void Scaler_SetGamma(unsigned char value);/*range 0~3*/

unsigned char Scaler_GetSindex(void);

unsigned char Scaler_SetSindex(unsigned short value);/*range 0~32*/

unsigned char Scaler_GetSlow(void);

unsigned char Scaler_SetSlow(unsigned short value);	/*range 0~20*/

unsigned char Scaler_GetShigh(void);

unsigned char Scaler_SetShigh(unsigned short value);/*range 0~20*/

void Scaler_SetGamma_for_Handler(unsigned char gamma,
															unsigned char s_gamma_index,
															unsigned char s_gamma_low,
															unsigned char s_gamma_high, unsigned char drvSkip_Flag);
void Scaler_SetGamma_for_ddomainISR_Handler(unsigned char gamma,
															unsigned char s_gamma_index,
															unsigned char s_gamma_low,
															unsigned char s_gamma_high, unsigned char drvSkip_Flag);

//unsigned char Scaler_Get_blue_stretch(void);//k5l hw remove

//void Scaler_set_blue_stretch(unsigned char table);//k5l hw remove

unsigned short Scaler_GetSharpnessTable(unsigned char display);

void Scaler_SetSharpnessTable(unsigned char display, unsigned char value);

unsigned short Scaler_GetCDSTable(void);

void Scaler_SetCDSTable(unsigned char value);

unsigned short Scaler_GetEGSM_postshp_level(void);

void Scaler_SetEGSM_postshp_level(unsigned char value);

unsigned char Scaler_GetEMF_Mk2(void);

void Scaler_SetEMF_Mk2(unsigned char value);

unsigned char Scaler_Get_DCC_Color_Independent_Table(void);

void Scaler_Set_DCC_Color_Independent_Table(unsigned char value);

unsigned char Scaler_Get_DCC_chroma_compensation_Table(void);

void Scaler_Set_DCC_chroma_compensation_Table(unsigned char value);

unsigned char Scaler_get_Adaptive_Gamma(void);

//void Scaler_set_Adaptive_Gamma(unsigned char value);

/*unsigned char Scaler_Get_DCC_Style(void);*/

/*void Scaler_Set_DCC_Style(unsigned char value, unsigned char calledByOSD);*/

unsigned char Scaler_GetRTNR_Noise_Measure(void);

void Scaler_SetRTNR_Noise_Measure(unsigned char value);

unsigned char Scaler_GetScaleUPH(void);

void Scaler_SetScaleUPH(unsigned char value);

unsigned char Scaler_GetScaleUPH_8tap(void);

void Scaler_SetScaleUPH_8tap(unsigned char value);

unsigned char Scaler_GetScaleUPV(void);

void Scaler_SetScaleUPV(unsigned char value);
void Scaler_SetScaleUP_YCHV(unsigned char HY, unsigned char VY, unsigned char HC, unsigned char VC);
void Scaler_GetScaleUP_YCHV(unsigned char *pHY,unsigned char *pVY,unsigned char *pHC,unsigned char *pVC);

unsigned char Scaler_GetScaleUPDir(void);

void Scaler_SetScaleUPDir(unsigned char value);/*range 0~19*/

unsigned char Scaler_GetScaleUPDir_Weighting(void);

void Scaler_SetScaleUPDir_Weighting(unsigned char value);/*range 0~9*/

unsigned char Scaler_GetScalePK(void);

void Scaler_SetScalePK(unsigned char value);					/*range 0~10*/

unsigned short Scaler_GetSUPKType(void);

void Scaler_SetSUPKType(unsigned char value);

unsigned short Scaler_GetUnsharpMask(void);

void Scaler_SetUnsharpMask(unsigned char value);



unsigned char Scaler_get_egsm_postshp_table(void);

void Scaler_set_egsm_postshp_table(unsigned char value);

unsigned char Scaler_getdESM(void);

void Scaler_setdESM(unsigned char value);

unsigned char Scaler_getiESM(void);

void Scaler_setiESM(unsigned char value);

void Scaler_set_SuperResolution_init_table(unsigned char value);

unsigned char Scaler_GetSR_edge_gain(void);

void Scaler_SetSR_edge_gain(unsigned char value);

unsigned char Scaler_GetSR_texture_gain(void);

void Scaler_SetSR_texture_gain(unsigned char value);

unsigned char Scaler_GetScaleDOWNHTable(void);

void Scaler_SetScaleDOWNHTable(unsigned char value);/*range 0~7*/

unsigned char Scaler_GetScaleDOWN444To422(void);

void Scaler_SetScaleDOWN444To422(unsigned char value);/*range 0~17*/

void Scaler_SetScaleUP_TwoStep(unsigned char value);
unsigned char Scaler_GetScaleUP_TwoStep(void);

unsigned char Scaler_GetScaleUPV_6tap(void);
void Scaler_SetScaleUPV_6tap(unsigned char value);

unsigned char Scaler_GetDiSmd(void);
void Scaler_SetDiSmd(unsigned char value);

void Scaler_SetScaleOSD_Sharpness(unsigned char value);
unsigned char Scaler_GetScaleOSD_Sharpness(void);

unsigned char Scaler_Debug_Get_UserCurveDCC_Curve_Select(void);
unsigned char Scaler_Debug_Get_HIST_MEAN_VALUE(void);
unsigned char Scaler_Debug_Get_Pure_Color_Flag(void);
unsigned char Scaler_Debug_Get_Skin_Tone_Found_Flag(void);
unsigned char Scaler_Debug_Get_UserCurveDCC_Curve_Weighting(void);
unsigned char Scaler_Debug_Get_UserCurveDCC_Curve_Weighting_Skin(void);

void Scaler_Debug_Set_UserCurveDCC_Curve_Select(unsigned char value);
void Scaler_Debug_Set_UserCurveDCC_Curve_Apply(unsigned char value);
void Scaler_Debug_Set_UserCurveDCC_Curve_Weighting(unsigned char value);
void Scaler_Debug_Set_UserCurveDCC_Curve_Weighting_Skin(unsigned char value);

void Scaler_set_pq_dir_path(char *path, char *patch_path);
unsigned char Scaler_get_average_luma(unsigned char *val);
unsigned char Scaler_get_luma_level_distribution(unsigned short *luma_distribution, unsigned char size);
unsigned char Scaler_get_Y_Total_Hist_Cnt_distribution(UINT8 MODE, UINT8 size, unsigned int *Y_Hist_distr_cnt);

void Scaler_SetDcrMode(unsigned char value);
unsigned char Scaler_GetDcrMode(void);
unsigned char Scaler_SetOSDBacklight2VIP(unsigned char value);
void Scaler_set_DCR_Enable(UINT8 value);
UINT8 Scaler_get_DCR_Enable(void);
void Scaler_Get_color_access_TVStatus_DisplayInfo(unsigned int *VIP_Access_Memory_DisplayInfo_Table);

#if 1//def LocalDimming
void Scaler_SetLocalDimmingEnable(unsigned char enable);
void Scaler_SetLocalDimmingTable(unsigned char TableIdx);
void Scaler_fwif_color_set_LocalDimming_table(unsigned char src_idx, unsigned char TableIdx);
#endif

void Scaler_SetLocalContrastEnable(unsigned char enable);
unsigned char Scaler_GetLocalContrastEnable(void);
void Scaler_SetLocalContrastTable(unsigned char TableIdx);
unsigned char Scaler_GetLocalContrastTable(void);

void Scaler_fwif_color_set_LocalContrast_table(unsigned char src_idx, unsigned char TableIdx);

void Scaler_Set_Dynamic_SLC_Table(unsigned char table_idx);
unsigned char Scaler_Get_Dynamic_SLC_Table(void);
void Scaler_Set_Dynamic_SLC_Enable(unsigned char enable);
unsigned char Scaler_Get_Dynamic_SLC_Enable(void);

#if 0/*def RTICE2AP_DEBUG_TOOL_ENABLE, */ /*change flow to scalerThread_rtice2AP.h for PQ tool share memory access*/
int rtice_get_vip_table(unsigned int id, unsigned short size, unsigned char *num_type, unsigned char *buf);
int rtice_set_vip_table(unsigned int id, unsigned short size, unsigned char *buf);
int rtice_set_IdxTable2Buff(unsigned short mode, unsigned short size, unsigned char *num_type, unsigned char *buf);	/*return idx table size*/
#endif

int Scaler_GetRGBPixelInfo(RGB_pixel_info *data);

void Scaler_Init_ColorLib_customer_share_memory_struct(void);

void Scaler_color_handler(void);

void fwif_color_video_quality_handler_customization(void);

void Scaler_SetPictureMode(unsigned char level);

unsigned char Scaler_GetPictureMode(void);

unsigned char Scaler_GetContrast(unsigned char dis);

void Scaler_SetContrast(unsigned char dis, unsigned char value);				/*range 0~100*/

unsigned char Scaler_GetBrightness(unsigned char dis);

void Scaler_SetBrightness(unsigned char dis, unsigned char value);				/*range 0~100*/

unsigned char Scaler_GetSaturation(unsigned char dis);

void Scaler_SetSaturation(unsigned char dis, unsigned char value);    /*range 0~100*/

unsigned char Scaler_GetHue(unsigned char dis);

void Scaler_SetHue(unsigned char dis, unsigned char value);        /*range 0~100*/

unsigned char Scaler_GetSharpness(void);

void Scaler_SetSharpness(unsigned char value);

unsigned char Scaler_SetBackLight(unsigned char value);

unsigned char Scaler_GetBacklight(void);

SLR_COLORTEMP_LEVEL Scaler_GetColorTemp(void);

void Scaler_SetColorTemp(SLR_COLORTEMP_LEVEL level);

unsigned char Scaler_PipGetSubBrightness(void);

/**
 * Scaler_PipSetSubBrightness
 * set the brightness level of sub channel
 *
 * @param { value:0~100}
 * @return { }
 *
 */
void Scaler_PipSetSubBrightness(unsigned char value);

/**
 * Scaler_PipGetSubContrast
 * set the contrast level of sub channel
 *
 * @param { }
 * @return { TRUE/FALSE }
 *
 */
unsigned char Scaler_PipGetSubContrast(void);

/**
 * Scaler_PipSetSubContrast
 * set the contrast level of sub channel
 *
 * @param { value:0~100  }
 * @return { }
 *
 */
void Scaler_PipSetSubContrast(unsigned char value);
void Scaler_Load_VipTable_Custom(void *ptr);

UINT8 Scaler_Get_DI_Force_2D_Enable(void);
void Scaler_Set_DI_Force_2D_Enable(UINT8 ucEnable);
UINT8 Scaler_Enter_DI_Force_2D_Condition(UINT8 ucDisplay);

unsigned short Scaler_GetMAG_H_Region_Start(void);
unsigned short Scaler_GetMAG_H_Region_End(void);
unsigned short Scaler_GetMAG_H_Region_Width(void);
unsigned short Scaler_GetMAG_V_Region_Start(void);
unsigned short Scaler_GetMAG_V_Region_End(void);
unsigned short Scaler_GetMAG_V_Region_Len(void);

/*=======================for magic picture======================*/
void Scaler_SetMagicPicture(SLR_MAGIC_TYPE level);
void mScalerSetMAG(void);
//void Scaler_MagicSaturation(unsigned char display, unsigned char MagicMode);
void mScalerMagicPictureCtrlForDynamic(void);
void Scaler_MoveMagicPictureForStill(unsigned int timerId, void *msg);
/*=======================for magic picture end======================*/

unsigned char Scaler_getDI_SMD_en(unsigned char source);
void Scale_SetHDR3DLUT_Offset(int offset);
void Scaler_color_copy_scalerinfo_to_vip(unsigned char display);
void Scaler_color_print_scalerinfo(void);

void Scaler_SetFacPatternColor(unsigned char val);

void Scaler_SetMagicPicture_demo_reset(unsigned char level);
unsigned char Scaler_GetMagicPicture_demo_reset(void);

//for VIP table check
void Scaler_checkVIPTable(void);

void Scaler_Set3dLUT(UINT8 TableIdx);
unsigned char Scaler_Get3dLUT(void);
unsigned char fwif_vip_increase_DI_water_lv(void);
//void Scaler_LGE_HDR10_Enable(unsigned char enable);
void Scaler_Set_HDR_YUV2RGB(unsigned char display, unsigned char HDR_mode, unsigned char color_format);
//void Scaler_SetHDR_BlackLevel(unsigned char blk_lv);

//void drvif_TV006_HDR10_init(void);
void Scaler_set_HDR10_Enable(unsigned char* enable);
void Scaler_LGE_ONEKEY_HDR10_Enable(unsigned char enable);
void Scaler_LGE_ONEKEY_BBC_Enable(unsigned char enable);
void drvif_TV006_SET_HDR_mode(UINT32* modeinfo);
#ifdef CONFIG_HDR_SDR_SEAMLESS
void drvif_TV006_SET_HDR_mode_Seamless(UINT32* modeinfo);
void drvif_TV006_SET_HDR_mode_Seamless_hdmi_position(UINT32* modeinfo);
void drvif_TV006_SET_HDR_mode_Seamless_hdmi_finish(UINT32* modeinfo);
void Scaler_LGE_HDR10_Enable_Seamless(unsigned char enable);
void Scaler_LGE_HDR10_Enable_Seamless_hdmi_position(unsigned char enable);
void Scaler_LGE_HDR10_Enable_Seamless_hdmi_finish(unsigned char enable);
void Scaler_SetDataFormatHandle_Seamless(void);
#endif
void Scaler_LGE_ONEKEY_SDR2HDR_Enable(unsigned char enable);

void Scaler_color_set_m_nowSource(unsigned char value);
unsigned char Scaler_color_get_m_nowSource(void);

void vpq_PQ_setting_check(char *item_name, unsigned int *PQ_check_error);
void vpq_PQ_check_timming(char *item_name, unsigned int *PQ_check_error);

unsigned char  Scaler_Set_I_De_XC(unsigned char value);
unsigned char Scaler_Get_I_De_XC(void);
unsigned char  Scaler_Set_I_De_Contour(unsigned char value);
unsigned char Scaler_Get_I_De_Contour(void);
unsigned char Scaler_Set_Block_DeContour(unsigned char value);
void  Scaler_Set_BOE_RGBW(int w_rate);


#if 0/*SLD, hack, elieli*/ //k5l remove
unsigned char  Scaler_Set_SLD(unsigned char value);
unsigned char Scaler_Get_SLD(void);
#endif
void Scaler_color_colorwrite_Output_InvOutput_gamma(unsigned char value);
unsigned char Scaler_Get_Output_InvOutput_gamma(void);

void Scaler_Share_Memory_Access_VIP_TABLE_Custom_Struct(unsigned long arg);

void Scaler_color_set_LC_DebugMode(unsigned char DebugMode);
char Scaler_color_get_LC_DebugMode(void);

unsigned char Scaler_color_LC_DemoSplit_Screen(unsigned char mode);
unsigned char Scaler_color_LC_DemoStep(unsigned char mode);
unsigned char Scaler_color_I_De_Contour_Demo(unsigned char mode);

#ifdef CONFIG_HDR_SDR_SEAMLESS
void Scaler_hdr_setting_SEAMLESS(unsigned char hdr_enable);
void Scaler_hdr_setting_SEAMLESS_hdmi_position(unsigned char hdr_enable, unsigned char hdr_type);
void Scaler_hdr_setting_SEAMLESS_hdmi_finish(unsigned char hdr_enable, unsigned char hdr_type);
#endif
unsigned char*  Scaler_color_Access_HDR_AutoRun_TBL_Flag(unsigned char isSetFlag, unsigned char *HDR_AutoRun_TBL_Flag);
void Scaler_color_set_HDR_AutoRun_TBL(void);
void fwif_color_vsc_to_scaler_src_mapping(unsigned char vsc_src, unsigned char *p_scaler_src_type, unsigned char *p_scaler_src_form);
unsigned char fwif_color_get_cur_input_timing_mode(unsigned char display);

//char Scaler_scalerVIP_PQ_IP_Enable_Monitor(SLR_VIP_TABLE* VIP_TBL, _RPC_clues* RPC_SmartPic_clue, _clues* SmartPic_clue, _RPC_system_setting_info* RPC_sysInfoTBL, _system_setting_info* sysInfoTBL, unsigned int MA_print_count);
//char Scaler_scalerVIP_PQ_IP_Enable_Monitor_Resort_check(unsigned char item);
//void Scaler_PQ_IP_Enable_Monitor_clear_Buff(void);
char Scaler_color_Set_PQ_ByPass_Lv(unsigned char table_idx);
char Scaler_color_Get_PQ_ByPass_Lv(void);
void drvif_TV006_SET_HDR_mode_off(void);
void Scaler_LGE_HDR10_Enable_newflow(unsigned char enable);

void Scaler_color_set_HDRver(unsigned int ver);
unsigned int Scaler_color_get_HDRver(void);
void Scaler_color_Set_EOTF_OETF_Table(void);
void EOTF_1024point(unsigned short maxlevel, VIP_HDR10_EOTF_ByLuminace_TBL_v4* EOTF_table, VIP_HDR10_OETF_ByLuminace_TBL_v4* OETF_table, unsigned int* EOTFyi_curve);
char Scaler_color_Get_HDR10_EOTF_ByMaxCLL_ByPanelLuminance(unsigned short forceIndex);
void OETF_inner_64point(int row, int col, unsigned short maxlevel, unsigned int OETFyi_curve[][1028], unsigned short OETF_Lmix[][16], unsigned int* OETFyi_inner, unsigned int* shift_bit, unsigned short* _3dlut);
char Scaler_color_Get_HDR10_OETF_ByMaxCLL_ByPanelLuminance(unsigned short forceIndex);
void Scaler_color_Get_HDR10_EOTF_OETF_ByMaxCLL_ByPanelLuminance_for_task(void);
char Scaler_color_Get_HLG_EOTF_ByMaxCLL_ByPanelLuminance(void);
char Scaler_color_Get_HLG_OETF_ByMaxCLL_ByPanelLuminance(void);
unsigned char Scaler_color_HAL_VPQ_HDR_Type_to_HDR_DM_Type(unsigned char HAL_VPQ_HDR);
void Scaler_color_set_HDR_Type(unsigned char HDR_Type);
void Scaler_color_set_HDR_sub_Type(unsigned char HDR_Type);
void Scaler_color_Set_HDR_AutoRun(unsigned char nEnable);
unsigned char Scaler_color_Get_HDR_AutoRun(void);
void Scaler_color_HDRIP_AutoRun(unsigned char display, unsigned char HDR_Type);
//void Scaler_color_HDRInfo_Update(UINT32* modeinfo);

void Scaler_color_HDRInfo_Handler(unsigned char display, UINT32* modeinfo);
//void Scaler_Set_HDR_YUV2RGB_by_HFC(UINT8 index, unsigned char color_format, unsigned char HDT_type);

void Scaler_color_Set_DCCProcess(unsigned char nEnable);
unsigned char Scaler_color_Get_DCCProcess(void);

void Scaler_color_copy_HDR_table_from_AP(SLR_HDR_TABLE* ptr_HDR_bin);
void Scaler_color_copy_HDR_table_from_AP_v4(SLR_HDR_TABLE_v4* ptr_HDR_bin);
void Scaler_color_copy_HDR_table_panel_luminance_from_AP(void);
void Scaler_color_copy_HDR_table_oetf_setting_ctrl_from_AP(unsigned short oetf_setting_ctrl_from_ini);
void Scaler_color_copy_HDR_table_oetf_curve_ctrl_from_AP(unsigned short oetf_curve_index_from_ini);

void Scaler_color_set_pq_src_idx(unsigned char src_idx);
unsigned char Scaler_color_get_pq_src_idx(void);

void scaler_pq_sub_mode_set(unsigned char sub_mode);

HDMI_CSC_DATA_RANGE_MODE_T Scaler_GetHDMI_CSC_DataRange_Mode(void);
void Scaler_SetHDMI_CSC_DataRange_Mode(HDMI_CSC_DATA_RANGE_MODE_T value);
void Scaler_recheck_sharpness_table(void);
unsigned char Scaler_Get_CinemaMode_PQ(void);
void Scaler_Set_CinemaMode_PQ(unsigned char mode);
bool Scaler_Init_OD(void);
unsigned char Scaler_Get_OD_Bits(void);
unsigned char Scaler_Set_OD_Bits(unsigned char bits);
unsigned char Scaler_Get_OD_Enable(void);
unsigned char Scaler_OD_Test(unsigned char en);
char Scaler_Set_DeMura_En(unsigned char en);
unsigned char Scaler_Get_DeMura_En(void);
char Scaler_Set_DeMura_Table(DRV_DeMura_CTRL_TBL * pCtrl_tbl, unsigned char *pDeMura_tbl);
void Scaler_Set_PicMode_VIP_src(unsigned char src);
void Scaler_Check_VIP_src_resync_flag(void);
unsigned char Scaler_Get_VIP_src_resync_flag(void);
unsigned char Scaler_SetBackLightMappingRange(unsigned int uiMax,unsigned int uiMin,unsigned int actMax,unsigned int actMin,unsigned char increaseMode);
unsigned char Scaler_GetBackLightMappingRange(unsigned int *uiMax,unsigned int *uiMin,unsigned int *actMax,unsigned int *actMin, unsigned char *increaseMode);
unsigned char Scaler_Get_DCR_Backlight(void);
unsigned char  Scaler_Set_VIP_HANDLER_RESET(void);
void Scaler_Set_PQ_table_bin_En(unsigned char en);
unsigned char Scaler_Get_PQ_table_bin_En(void);

unsigned char Scaler_VPQ_Set_ModuleTestMode(unsigned char);


unsigned char  Scaler_Set_VIP_Disable_PQ(VPQ_ModuleTestMode ModuleTestMode, unsigned char enable);

unsigned char Scaler_Set_OD_Bit_By_XML(void);

char Scaler_Set_DI_SNR_Borrow_Sram_Mode(VIP_SNR_DI_Sram_Borrow_Mode mode);
char Scaler_Get_DI_SNR_Borrow_Sram_Mode(void);

void Scaler_color_ModuleTest_HDR_FixTable(void);
unsigned char Scaler_color_access_RTNR_H_Rotate_Flag(unsigned char Val, unsigned char isSetFlag);
void Scaler_color_set_RTNR_H_Rotate(unsigned char display, unsigned char En);
void Scaler_color_decide_RTNR_H_Rotate(unsigned char display);

unsigned char  Scaler_Set_PQ_ModuleTest(VPQ_ModuleTestMode ModuleTestMode);
S_VPQ_ModuleTestMode *Scaler_Get_PQ_ModuleTest(void);
void Scaler_color_Dither_Table_Ctrl(void);
void Scaler_SetDataFormatHandle_Seamless(void);
unsigned char Scaler_get_dcc_table_by_source(unsigned char table_mode);
unsigned char Scaler_Set_Gamma_level(unsigned char value);

#if IS_ENABLED(CONFIG_RTK_AI_DRV)
void* scaler_VIP_PQMask_GetSemanticTable(unsigned char bLogEnable);
void* scaler_VIP_PQMask_GetDepthTable(unsigned char bLogEnable);
void Scaler_VIP_LC_reset(void);
void Scaler_VIP_scene_sharpness_reset(void);
void Scaler_VIP_DCC_Color_Independent_reset(void);
#endif
void Scaler_color_get_LC_His_dat(unsigned int *plc_out);
int Scaler_color_set_AP_PQ_extend_data(unsigned char type, unsigned long pUserSpaceData);

#if IS_ENABLED(CONFIG_RTK_AI_DRV)
void Scaler_VIP_LC_reset(void);
void Scaler_VIP_scene_sharpness_reset(void);
void Scaler_VIP_DCC_Color_Independent_reset(void);
#endif
void Scaler_color_write_data_to_bin_file(unsigned char *data, unsigned int size, unsigned char* ouputfilename);


void Scaler_SetNNSR_Ctrl_Table(unsigned char TableIdx);
unsigned char Scaler_GetNNSR_Ctrl_Table(void);

void Scaler_SetNNSR_Blending_Weight_Table(unsigned char TableIdx);
unsigned char Scaler_GetNNSR_Blending_Weight_Table(void);

void Scaler_Set_ColorMapping_3x3_Table(unsigned char table_idx);
unsigned char Scaler_Get_ColorMapping_3x3_Table(void);

void Scaler_Set_ColorMapping_By_ColorFormat(unsigned char channel, unsigned char nSD_HD);
void Scaler_Set_WCG_mode(unsigned int wcgmode);
void Scaler_Set_Camera_PQsetting(unsigned char get_vo_camera_flow_flag);
void Scaler_Copy_PQbin(void);
void Scaler_fwif_color_set_pip_colorkey(CONBRI_VIDEO_COLORKEY_PARA video_color);
void Scaler_Set_PQ_UVC_DataPath(UVC_CAPTURE_FORMAT_TYPE  UVC_OutputFormat,unsigned char enable,unsigned char colorspce,unsigned int inSize_nLength , unsigned int inSize_nWidth,unsigned int outSize_nLength , unsigned int outSize_nWidth);
void Scaler_SetPQrBringUp(void);
void Scaler_resolution_change_check(void);
void Scaler_Reset_Sharpness_Table(void);
#ifdef __cplusplus
}
#endif

#endif /* __SCALER_LIB_H__*/

