#ifndef _PQLAPI_H
#define _PQLAPI_H

#ifdef _WIN32
#ifdef PQLIBRARY_EXPORTS
#define  _PQLSERVERDLL __declspec(dllexport)
#else
#define  _PQLSERVERDLL __declspec(dllimport)
#endif
#else
#define  _PQLSERVERDLL
#endif

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////
VOID _PQLSERVERDLL PQL_Init(VOID);

VOID _PQLSERVERDLL PQL_PhaseTable_Init(VOID);

VOID _PQLSERVERDLL PQL_oneFifth_OutputInterrupt_Proc(int iSliceIdx);

VOID _PQLSERVERDLL PQL_OnputInterrupt_Hsync_Proc(int  iSliceIdx);

VOID _PQLSERVERDLL PQL_InputInterrupt_Proc(int  iSliceIdx);

VOID _PQLSERVERDLL PQL_OutputInterrupt_Proc(int iSliceIdx);

////////////////////////////////////////////////////////////////////
//Command
enum PQL_COMMAND_
{
	PQL_CMD_VOID,
	PQL_CMD_0,
	PQL_CMD_1,
	
	///
	PQL_CMD_MEMC_MODULE_RATIO,

	PQL_CMD_MEMC_MODE_DEJUDDER,
	PQL_CMD_MEMC_MODE_DEBLUR,
	PQL_CMD_MEMC_MODE_FB,

	PQL_CMD_INPUT_RESOLUTION,
	PQL_CMD_INPUT_FRAME_RATE,
	PQL_CMD_INPUT_FORMAT,

	PQL_CMD_OUTPUT_RESOLUTION,
	PQL_CMD_OUTPUT_FRAME_RATE,
	PQL_CMD_OUTPUT_FORMAT,

	PQL_CMD_OUTPUT_MODE,
	PQL_CMD_LR_SWAP,
	PQL_CMD_INSERT_BLACK,
	PQL_CMD_CRTC_N,
	PQL_CMD_CRTC_M,
	PQL_CMD_OSD_FORCE_MODE,
	PQL_CMD_PULLDOWNMODE,
	PQL_CMD_INPUTVSXRAL_FRAMEREPEAT_MODE,
	PQL_CMD_LOWDELAYMODE,
	PQL_CMD_MEMC_MODE,
	PQL_CMD_POWERSAVING,
};

enum PQL_INFO_
{
	PQL_INFO_VOID,
	PQL_INFO_0,
	PQL_INFO_1,
	PQL_INFO_SRC_VO,
	PQL_INFO_HDR_CHG,
	PQL_INFO_BRIGHTNESS,
	PQL_INFO_CONTRAST,
	PQL_INFO_SPEED,
	PQL_INFO_ZOOM,
	PQL_INFO_SHIFT,
	PQL_INFO_SOURCE_TYPE,
	PQL_INFO_VOSEEK,
	PQL_INFO_MEMC_MODE,
	PQL_INFO_DARKLEVEL,
	PQL_INFO_HIST_SC,
	PQL_INFO_SOCCER_Patch,
	PQL_INFO_PICTUREMODE_CHG,
	PQL_INFO_VIP_STILL,
	PQL_INFO_MIRACAST,
	PQL_INFO_FORBES,
	PQL_INFO_STEP_HIST_SUM_RATIO,
	PQL_INFO_VIP_PROJECT_ID,
	PQL_INFO_TRIBALL_Patch
};
typedef enum
{
	_PQL_IN_2D,
	_PQL_IN_SBS,
	_PQL_IN_TB,
	_PQL_IN_LR,
	_PQL_IN_LBL,
	_PQL_IN_FP,
}PQL_INPUT_3D_FORMAT;
typedef enum
{
	_PQL_OUT_2D,
	_PQL_OUT_SG,
	_PQL_OUT_PR,
	_PQL_OUT_SG_LLRR,
}PQL_OUTPUT_3D_FORMAT;
typedef enum
{
	_PQL_IN_24HZ,
	_PQL_IN_25HZ,
	_PQL_IN_30HZ,
	_PQL_IN_48HZ,
	_PQL_IN_50HZ,
	_PQL_IN_60HZ,
	_PQL_IN_96HZ,
	_PQL_IN_100HZ,
	_PQL_IN_120HZ,
}PQL_INPUT_FRAME_RATE;
typedef enum
{
	_PQL_OUT_48HZ,
	_PQL_OUT_50HZ,
	_PQL_OUT_60HZ,
	_PQL_OUT_96HZ,
	_PQL_OUT_100HZ,
	_PQL_OUT_120HZ,
	_PQL_OUT_200HZ,
	_PQL_OUT_240HZ,
}PQL_OUTPUT_FRAME_RATE;
typedef enum
{
	_PQL_IN_1920x1080,
	_PQL_IN_1920x540,
	_PQL_IN_3840x2160,	
	_PQL_IN_3840x1080,
	_PQL_IN_3840x540,
	_PQL_IN_1920x2160,
	_PQL_IN_768X480,
	_PQL_IN_1280X720,
	_PQL_IN_7680X4320,
	_PQL_IN_2560x1440,
	_PQL_IN_RESOLUTION_MAX,
}PQL_INPUT_RESOLUTION;
typedef enum
{
	_PQL_OUT_1920x1080,
	_PQL_OUT_1920x540,
	_PQL_OUT_3840x2160,
	_PQL_OUT_3840x1080,
	_PQL_OUT_3840x540,
	_PQL_OUT_768X480,
	_PQL_OUT_1280X720,
	_PQL_OUT_7680X4320,
	_PQL_OUT_2560x1440,
	_PQL_OUT_RESOLUTION_MAX,
}PQL_OUTPUT_RESOLUTION;

typedef enum
{
	_PQL_OUT_VIDEO_MODE,
	_PQL_OUT_PC_MODE,
}PQL_OUT_MODE_;

typedef enum
{
	_PQL_IN24HZ_TRUMOTOFF_CINEMAON = 0,
	_PQL_IN24HZ_TRUMOTOFF_CINEMAOFF,
	_PQL_IN25HZ_TRUMOTOFF_CINEMAOFF,
	_PQL_IN30HZ_TRUMOTOFF_CINEMAOFF,
	_PQL_IN50HZ_TRUMOTOFF_CINEMAOFF,
	_PQL_IN60HZ_TRUMOTOFF_CINEMAON,
	_PQL_IN60HZ_TRUMOTOFF_CINEMAOFF,
	_PQL_INXHZ_TRUMOTOFF,
	_PQL_PULLDOWN_MODE_ELSE,
}PQL_OUTPUT_PULLDOWN_MODE;

typedef struct  
{
	unsigned int uiData;
	char  cData;
}_PQL_Command0;

typedef struct  
{
	int			 iData;
	unsigned char  ucData;
}_PQL_Command1;

typedef struct  
{
	unsigned char  u8_in_me1_pix_hsft;
	unsigned char  u8_in_me1_pix_vsft;
	unsigned char  u8_in_me1_blk_hsft;
	unsigned char  u8_in_me1_blk_vsft;

	unsigned char  u8_in_me2_pix_hsft;
	unsigned char  u8_in_me2_pix_vsft;	
	unsigned char  u8_in_me2_blk_hsft;
	unsigned char  u8_in_me2_blk_vsft;

	unsigned char  u8_in_dh_blk_hsft;
	unsigned char  u8_in_dh_blk_vsft;

	unsigned char  u8_in_logo_pix_hsft;
	unsigned char  u8_in_logo_pix_vsft;	
	unsigned char  u8_in_logo_blk_hsft;
	unsigned char  u8_in_logo_blk_vsft;	

//	unsigned char  u8_in_me1_pix_sft;
//	unsigned char  u8_in_me1_blk_sft;

//	unsigned char  u8_in_me2_pix_sft;
//	unsigned char  u8_in_me2_blk_sft;
}_PQL_MEMC_MODULE_RATIO;


typedef union
{
	_PQL_MEMC_MODULE_RATIO    _stc_memc_module_ratio;
	_PQL_Command0       _Command0;//PQL_CMD_0
	_PQL_Command1       _Command1;//PQL_CMD_1

	////
	unsigned char                     u4_memc_mode_dejudder_lvl;
	unsigned char                     u4_memc_mode_deblur_lvl;
	unsigned char                     u4_memc_mode_fb_lvl;

	PQL_INPUT_RESOLUTION      _input_resolution;
	PQL_INPUT_3D_FORMAT       _input_3dFormat;
	PQL_INPUT_FRAME_RATE      _input_frameRate;
	  
	PQL_OUTPUT_RESOLUTION     _output_resolution;
	PQL_OUTPUT_3D_FORMAT      _output_3dFormat;
	PQL_OUTPUT_FRAME_RATE     _output_frameRate;

	PQL_OUT_MODE_             _output_mode;
	unsigned char                     u1_LR_swap;
	unsigned char                     u1_InsertBlack;

	unsigned char                     u8_crtc_n;
	unsigned char                     u8_crtc_m;

	BOOL		u1_osd_force_mode;
	unsigned char		u8_pulldown_mode;
	unsigned char		u8_InputVSXral_FrameRepeatMode;
	unsigned char		u1_lowDelayMode;
	unsigned char		u3_memcMode;
	BOOL		u1_powerSaving;
}_PQL_Command;

typedef union
{
	_PQL_Command0       _Info0;//PQL_INFO_0
	_PQL_Command1       _Info1;//PQL_INFO_1
	BOOL		u1_SRC_VO;
	BOOL		u1_HDR_Chg;
	unsigned short		u16_mBrightness;
	unsigned short		u16_mContrast;
	signed short 		s16_mSpeed;
	BOOL		u1_ZoomAction;
	BOOL      	u1_ShiftAction;	
	unsigned short		u16_source_type;
	unsigned char		u1_mVOSeek;
	unsigned short		u16_MEMCMode;
	unsigned char		u8_mDarkLevel;
	BOOL		u1_hist_sc_flg;
	unsigned char		u8_mSoccer_holdfrm;
	BOOL		u1_PictureMode_Chg;
	BOOL		u1_mVIP_Still;
	unsigned char		u1_mMiraCast;
	unsigned short		u1_Forbes_flg;
	unsigned int DTL_Step_Hist_sum_ratio[5];
	unsigned char u8_VIP_Project_ID;
	unsigned char		u8_mTriBall_holdfrm;
}_PQL_Info;

VOID _PQLSERVERDLL PQL_Command(const unsigned char CmdID,const _PQL_Command *pCommand);
VOID _PQLSERVERDLL PQL_Info(const unsigned char InfoID,const _PQL_Info *pInfo);

///////////////////////////////////////////////////////////////////////
//Request
enum PQL_REQUEST_
{
	PQL_REQ_CADENCE_ID,
	PQL_REQ_0,
	PQL_REQ_1,
	PQL_REQ_2,
	PQL_REQ_3,
	PQL_VER,
};

typedef struct  
{
	unsigned int uiData;
	unsigned char  ucData;
}_PQL_Request0;

typedef struct  
{
	unsigned int uiData;
	unsigned char  ucData;
}_PQL_Request1;

typedef union
{
	unsigned char           u1_cadence_isVideo;
	_PQL_Request0	_Request0;//PQL_REQ_0
	_PQL_Request1	_Request1;//PQL_REQ_1
	char			_Version[128];//PQL_VERSION
}_PQL_Request;
VOID _PQLSERVERDLL PQL_Request(const unsigned char ReqID,_PQL_Request *pRequest);

#ifdef __cplusplus
};
#endif


#endif
