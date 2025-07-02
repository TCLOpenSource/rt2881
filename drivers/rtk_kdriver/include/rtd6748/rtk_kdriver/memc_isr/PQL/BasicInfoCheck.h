#ifndef _BASICINFOCHECK_H
#define _BASICINFOCHECK_H

#ifdef __cplusplus
extern "C" {
#endif
#define TAG_NAME_MEMC "MEMC"
#define __MEMC_SELF_CALIB__				("MEMC_SELF_CALIB")
#define __MEMC_AUTO_REGRESSION__		("MEMC_AUTO_REGRESSION")
#define __MEMC_GET_BASICINFO_DATA__		("MEMC_GET_BASICINFO_DATA")

// file config
#define SAVE_GOLDEN_FILE			(0)
#define SAVE_RESULT_FILE			(0)
#define SAVE_FILE_CONFIG			(SAVE_GOLDEN_FILE||SAVE_RESULT_FILE)

// auto regression data
#define REGRESSION_MAX_DATA			(90)
#define	REGRESSION_YH_MAX_TIME		(REGRESSION_MAX_DATA)
#define	REGRESSION_YH_MAX_DATA		(REGRESSION_YH_MAX_TIME)
#define	REGRESSION_SC_MAX_DATA		(20)
#define	REGRESSION_FB_MAX_TIME		(REGRESSION_MAX_DATA)
#define	REGRESSION_FB_MAX_DATA		(REGRESSION_FB_MAX_TIME) // record data every 1 second
#define	REGRESSION_LOGO_MAX_TIME	(REGRESSION_MAX_DATA)
#define	REGRESSION_LOGO_MAX_DATA	(REGRESSION_LOGO_MAX_TIME) // record data every 1 second
#define RGN21						(21)
#define RGN32						(32)
#if defined (IC_K24) && (IC_K24)
#define IC_SUPPORT_PIXEL_LOGO		(0)
#else
#define IC_SUPPORT_PIXEL_LOGO		(1)
#endif

// auto regression communication between kernel and console
#define COMMUNICATE_REG				(SOFTWARE2_SOFTWARE2_14_reg)
#define COMMUNICATE_STREAMID_START	(0)
#define COMMUNICATE_STREAMID_END	(7)
#define COMMUNICATE_STREAMLEN_START	(8)
#define COMMUNICATE_STREAMLEN_END	(15)
#define COMMUNICATE_STATUS_START	(28)
#define COMMUNICATE_STATUS_END		(31)

typedef struct
{
	unsigned char u8_result; //value defined by SELF_TEST_RESULT
	unsigned short u16_total_num; //max pattern number: 32
	unsigned short u16_correct_num;
	unsigned short u16_time; //unit:sec
	unsigned int u32_error_stream_id; //max record 32 stream
	unsigned int u32_unstable_stream_id; //max record 32 stream
}_Self_Check_Result;

typedef struct
{
	unsigned short u13_bar_width;
	unsigned short u13_bar_height;
	unsigned char u8_speed_h;
	unsigned char u8_speed_v;
	unsigned char u5_mot_len;
	unsigned int u32_mot_seq;
	unsigned short u10_bar_color_r;
	unsigned short u10_bar_color_g;
	unsigned short u10_bar_color_b;
	unsigned char u1_pattern_mode;
	unsigned char u2_bg_type;
	unsigned char u3_bg_type0;
	unsigned short u8_checkerboard_width;
	unsigned short u8_checkerboard_height;
}_PTG_Param;

typedef struct
{
	unsigned char u8_r_id;
	unsigned char u8_g_id;
	unsigned char u8_b_id;
}_APL_RGB;

typedef struct{
	unsigned short u10_r;
	unsigned short u10_g;
	unsigned short u10_b;
}_PTG_RGB;

typedef struct{
	unsigned short u16_x;
	unsigned short u16_y;
}_PTG_POS;

typedef struct{
	_PTG_POS PTG_pos;
	_PTG_RGB PTG_color;
}_PTG_POS_COLOR;

typedef struct
{
	unsigned int u13_square_width;
	unsigned int u13_square_height;
	unsigned int u8_speed;
	unsigned int u8_checkerboard_width;
	unsigned int u8_checkerboard_height;
}_SAD_PTG_Param;

typedef struct
{
	unsigned int u10_color;
	unsigned int u8_checkerboard_width;
	unsigned int u8_checkerboard_height;
}_DTL_PTG_Param;

typedef struct
{
	unsigned int u10_color;
	unsigned int u8_checkerboard_width;
	unsigned int u8_checkerboard_height;
}_PERIODIC_CNT_PTG_Param;

typedef enum{
	USB_START_PLAY,
	USB_FINISH_PLAY,
	USB_PLAYING,
	USB_WITHOUT_PLAYING,
	USB_TYPE_NUM,
}_DTL_USB_STATUS;

typedef enum{
	PTG_DEFAULT = 0,
	PTG_FINAL_CHECK_WITH_SC_1,
	PTG_FINAL_CHECK_WITHOUT_SC_1,
	PTG_FINAL_CHECK_WITH_SC_2,
	PTG_FINAL_CHECK_WITHOUT_SC_2,
	PTG_CHECK_SC_RMVRATIO,
	PTG_CHECK_SC_LV2,
	PTG_CHECK_SC_LV3,
	PTG_GET_APL_DATA,
	PTG_GET_GMVCNT_DATA,
	PTG_GET_DTL_DATA,
	PTG_CHECK_CHECKERBOARD_POS,
	PTG_CHECK_SQUARE_SIZE,
	PTG_CHECK_LBMC_1,
	PTG_CHECK_LBMC_2,
	PTG_CHECK_LBMC_3,
	PTG_CHECK_LBMC_4,
	PTG_MVINFO_STILL,
	PTG_MVINFO_H_MOVE,
	PTG_MVINFO_V_MOVE,
	PTG_LOGO_DETECT,
	PTG_INDEX_NUM,
}PTG_TABLE_INDEX;

typedef enum{
	CHECKERBOARD_SIZE_100 = 0,
	CHECKERBOARD_SIZE_64,
	CHECKERBOARD_SIZE_50,
	CHECKERBOARD_SIZE_32,
	CHECKERBOARD_SIZE_25,
	CHECKERBOARD_SIZE_16,
	CHECKERBOARD_SIZE_10,
	CHECKERBOARD_SIZE_NUM,
}CHECKERBOARD_SIZE_INDEX;

typedef enum{
	CHECK_MVINFO_STILL = 0,
	CHECK_MVINFO_H_MOVE,
	CHECK_MVINFO_V_MOVE,
	CHECK_MVINFO_NUM,
}CHECK_MVINFO_INDEX;

typedef enum{
	PTG_CALIB_APL = 0,
	PTG_CALIB_DTL,
	PTG_CALIB_SAD,
	PTG_CALIB_SC,
	PTG_CALIB_LBMC,
	PTG_CALIB_YUV,
	PTG_CALIB_LOGO_DETECT,
	PTG_CALIB_PERIODIC_CNT,
	PTG_CALIB_NUM,
}INTERNAL_PTG_CALIB_INDEX;

typedef enum{
	REALSTREAM_CALIB_MV_INFO = 0,
	REALSTREAM_CALIB_FB_BADREGION,
	REALSTREAM_CALIB_NUM,
}REALSTREAM_CALIB_INDEX;

typedef enum{
	ENVIRONMENT_CHECK_PTG_APL = 0,
	ENVIRONMENT_CHECK_PTG_POS,
	ENVIRONMENT_CHECK_PTG_SQUARE,
	ENVIRONMENT_CHECK_PTG_MV_INFO,
	ENVIRONMENT_CHECK_DDR_BOUNDARY,
	ENVIRONMENT_CHECK_RIM_BOUNDARY,
	ENVIRONMENT_CHECK_LBMC,
	ENVIRONMENT_CHECK_REGION,
	ENVIRONMENT_CHECK_NUM,
}ENVIRONMENT_CHECK_INDEX;

typedef enum{
	SELF_CHECK_TYPE_ENVIRONMENT = 0,
	SELF_CHECK_TYPE_INTERNAL,
	SELF_CHECK_TYPE_REALSTREAM,
	SELF_CHECK_TYPE_NUM,
}SELF_CHECK_TYPE_INDEX;

typedef enum{
	AUTOREGRESSION_ALL_Cadence_25_4K, //0
	AUTOREGRESSION_ALL_2K_logo_zebra, //1
	AUTOREGRESSION_ALL_RTK_K8HpFHD_Subtitle_03,//2
	AUTOREGRESSION_ALL_002, //3
	AUTOREGRESSION_ALL_059, //4
	AUTOREGRESSION_ALL_129, //5
	AUTOREGRESSION_ALL_134, //6
	AUTOREGRESSION_ALL_135, //7
	AUTOREGRESSION_ALL_171, //8
	AUTOREGRESSION_ALL_204, //9
	AUTOREGRESSION_ALL_205, //10
	AUTOREGRESSION_ALL_206, //11
	AUTOREGRESSION_ALL_243, //12
	AUTOREGRESSION_ALL_315, //13
	AUTOREGRESSION_ALL_321, //14
	AUTOREGRESSION_ALL_324, //15
	AUTOREGRESSION_ALL_389, //16
	AUTOREGRESSION_ALL_403, //17
	AUTOREGRESSION_ALL_408, //18
	AUTOREGRESSION_ALL_431, //19
	AUTOREGRESSION_ALL_NUM,
}AUTOREGRESSION_ALL_INDEX;

typedef enum{
	AUTOREGRESSION_YH = 0,
	AUTOREGRESSION_SC,
	AUTOREGRESSION_GFB,
	AUTOREGRESSION_LFB,
	AUTOREGRESSION_LOGO,
	AUTOREGRESSION_TYPE_NUM,
}AUTOREGRESSION_TYPE_INDEX;

typedef enum{
	SELF_TEST_FAIL = 0, 
	SELF_TEST_PASS,
	SELF_TEST_INIT,
	SELF_TEST_PROCESS,
	SELF_TEST_NUM,
}SELF_TEST_RESULT;

#if SAVE_FILE_CONFIG
typedef enum{
	SAVE_FILE_FAIL = 0, 
	SAVE_FILE_PASS,
	SAVE_FILE_PROCESS,
	SAVE_FILE_INIT = 9,
	SAVE_FILE_NUM,
}SAVE_FILE_STATUS;
#endif
	
typedef enum{
	COM_STATUS_INIT = 0, 
	COM_STATUS_PLAY,
	COM_STATUS_END_PLAY,
	COM_STATUS_FINISH_REGRESSION,
	COM_STATUS_NUM,
}REGRESION_COM_STATUS;

typedef enum{
	POS_CHECK_LEFT_UP = 0,
	POS_CHECK_RIGHT_DOWN,
	POS_CHECK_NUM,
}IPPRE_PTG_POS_CHECK_INDEX;

typedef enum{
	FB_BADREGION_CNT_LOW_THRESH = 0,
	FB_BADREGION_CNT_MID_THRESH,
	FB_BADREGION_CNT_HIGH_THRESH,
	FB_BADREGION_CNT_ALL_THRESH,
	FB_BADREGION_CNT_NUM,
}_FB_BADREGION_CNT;

typedef struct
{
	signed short s11_gmv_x;
	signed short s10_gmv_y;
	unsigned int u17_gmv_cnt;
	unsigned short u12_gmv_unconf;
	signed short s11_rmv_x;
	signed short s10_rmv_y;
}_CHECK_PTG_MV_INFO;

typedef struct
{
	signed short GMV_1st_vx;
	signed short GMV_1st_vy;
}_CHECK_REAL_STREM_MV_INFO;

typedef struct
{
	unsigned short Y[REGRESSION_YH_MAX_TIME][RGN32];
	unsigned short Hue[REGRESSION_YH_MAX_TIME][RGN32];
}_AUTOREGRESSION_SINGE_RESULT_YH;

typedef struct
{
	unsigned int u32_time[REGRESSION_SC_MAX_DATA];
}_AUTOREGRESSION_SINGE_RESULT_SC;

typedef struct
{
	unsigned short u14_LFB_lvl_cnt[REGRESSION_FB_MAX_DATA][RGN21];
}_AUTOREGRESSION_SINGE_RESULT_LFB;

typedef struct
{
	unsigned short u14_GFB_lvl_cnt[REGRESSION_FB_MAX_DATA];
}_AUTOREGRESSION_SINGE_RESULT_GFB;

typedef struct
{
	unsigned short u16_detect_logo_cnt[REGRESSION_LOGO_MAX_DATA][RGN21];
}_AUTOREGRESSION_SINGE_RESULT_LOGO;

typedef struct
{
#if IC_SUPPORT_PIXEL_LOGO
	_AUTOREGRESSION_SINGE_RESULT_LOGO PLOGO;
#endif
	_AUTOREGRESSION_SINGE_RESULT_LOGO BLOGO;
}_AUTOREGRESSION_SINGE_RESULT_LOGO_ALL;

typedef struct
{
	_AUTOREGRESSION_SINGE_RESULT_YH YH;
	_AUTOREGRESSION_SINGE_RESULT_SC SC;
	_AUTOREGRESSION_SINGE_RESULT_GFB GFB;
	_AUTOREGRESSION_SINGE_RESULT_LFB LFB;
	_AUTOREGRESSION_SINGE_RESULT_LOGO_ALL LOGO;
}_AUTOREGRESSION_SINGLE_RESULT;

typedef struct
{
	_AUTOREGRESSION_SINGLE_RESULT Result[AUTOREGRESSION_ALL_NUM];
}_AUTOREGRESSION_ALL_RESULT;

typedef struct
{
	_Self_Check_Result Result[AUTOREGRESSION_TYPE_NUM];
}_CHECK_RESULT_AUTOREGRESSION;

VOID BasicInfoCheck_Init(void);
unsigned char BasicInfoCheck_cal_result(unsigned char AUTOTEST_TYPE, unsigned char AUTOTEST_INDEX, unsigned short u16_correct_num, unsigned short u16_total_num);
VOID BasicInfoCheck_Show_APL_Info(void);
VOID BasicInfoCheck_Show_DTL_Info(void);
VOID BasicInfoCheck_Show_SAD_Info(void);
VOID BasicInfoCheck_Show_Basic_Info(void);
VOID BasicInfoCheck_Environment_check(void);
VOID BasicInfoCheck_IPPRE_PTG_check(void);
VOID BasicInfoCheck_SC_Check(void);
VOID BasicInfoCheck_RealPattern_check(void);
VOID AutoRegression_Update_Info(VOID);
const _CHECK_RESULT_AUTOREGRESSION* AutoRegression_Get_Result(VOID);
VOID AutoRegression_Verify(VOID);
VOID AutoRegression_GetGolden(VOID);
VOID BasicInfoCheck_Proc(VOID);
VOID AutoRegression_Proc(VOID);

#ifdef __cplusplus
}
#endif

#endif

