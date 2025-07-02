#ifndef _AI_CMD_ID_H_
#define _AI_CMD_ID_H_

/************************************************************************
 *  Include files
 ************************************************************************/
#include "ai_types.h"

 /************************************************************************
 *  Definitions
 ************************************************************************/
#define AI_DEVICE_NAME						"rtk-ai"
#define AI_IOC_MAGIC 'a'

enum ai_ioc_cmd
{
	AI_IOC_CMD_INIT = 0,
	AI_IOC_CMD_UNINIT = 1,
	AI_IOC_CMD_OPEN = 2,
	AI_IOC_CMD_CLOSE = 3,
	AI_IOC_CMD_REGIST_MODEL = 4,
	AI_IOC_CMD_UNREGIST_MODEL = 5,
	AI_IOC_CMD_GET_SHARE_MEM = 6,
	AI_IOC_CMD_GET_GRANT = 7,
	AI_IOC_CMD_RELEASE_GRANT = 8,
	AI_IOC_CMD_START_MC_MEASURE = 9,
	AI_IOC_CMD_STOP_MC_MEASURE = 10,
	AI_IOC_CMD_AUDIO_SET_VOCAL_VOL = 11,	
	AI_IOC_CMD_AUDIO_SET_MUSIC_VOL = 12,	
	AI_IOC_CMD_AUDIO_SET_EMOTION = 13,	
	AI_IOC_CMD_SET_ENABLE_STATUS = 14,	
};

#define AI_IOC_INIT                     _IO(AI_IOC_MAGIC, AI_IOC_CMD_INIT)
#define AI_IOC_UNINIT                   _IO(AI_IOC_MAGIC, AI_IOC_CMD_UNINIT)
#define AI_IOC_OPEN                     _IO(AI_IOC_MAGIC, AI_IOC_CMD_OPEN)
#define AI_IOC_CLOSE                    _IO(AI_IOC_MAGIC, AI_IOC_CMD_CLOSE)
#define AI_IOC_REGIST_MODEL             _IOW(AI_IOC_MAGIC, AI_IOC_CMD_REGIST_MODEL, unsigned int)
#define AI_IOC_UNREGIST_MODEL           _IOW(AI_IOC_MAGIC, AI_IOC_CMD_UNREGIST_MODEL, int)
#define AI_IOC_GET_SHARE_MEM            _IOR(AI_IOC_MAGIC, AI_IOC_CMD_GET_SHARE_MEM, unsigned int)
#define AI_IOC_GET_GRANT                _IOW(AI_IOC_MAGIC, AI_IOC_CMD_GET_GRANT, unsigned int)
#define AI_IOC_RELEASE_GRANT            _IOW(AI_IOC_MAGIC, AI_IOC_CMD_RELEASE_GRANT, unsigned int)
#define AI_IOC_START_MC_MEASURE         _IOW(AI_IOC_MAGIC, AI_IOC_CMD_START_MC_MEASURE, unsigned int)
#define AI_IOC_STOP_MC_MEASURE          _IO(AI_IOC_MAGIC, AI_IOC_CMD_STOP_MC_MEASURE)
#define AI_IOC_AUDIO_SET_VOCAL_VOL      _IOW(AI_IOC_MAGIC, AI_IOC_CMD_AUDIO_SET_VOCAL_VOL, unsigned int)
#define AI_IOC_AUDIO_SET_MUSIC_VOL      _IOW(AI_IOC_MAGIC, AI_IOC_CMD_AUDIO_SET_MUSIC_VOL, unsigned int)
#define AI_IOC_AUDIO_SET_EMOTION        _IOW(AI_IOC_MAGIC, AI_IOC_CMD_AUDIO_SET_EMOTION, unsigned int)
#define AI_IOC_SET_ENABLE_STATUS        _IOW(AI_IOC_MAGIC, AI_IOC_CMD_SET_ENABLE_STATUS, unsigned int)

#define AI_MAX_PROCESS		10
#define AI_MAX_MODEL_FILE		10		// max model file per process
#define AI_MAX_MODEL_NAME_LEN 40

#define AI_SEM_HIST_BIN 32

typedef enum{
	AI_ENABLE_STATUS_PQ=0,
	AI_ENABLE_STATUS_AUDIO=1,
	AI_ENABLE_STATUS_CAMERA=2,
	AI_ENABLE_STATUS_AUDIO_DENOISE=3,
	AI_ENABLE_STATUS_MAX
} AI_ENABLE_STATUS_TYPE;

typedef struct _AI_ENABLE_STATUS {
   AI_ENABLE_STATUS_TYPE  type;
   u32 enable;
} AI_ENABLE_STATUS;

typedef enum{
	AI_PQ_AP_OFF=0, // AI_FACE_OFF
	AI_PQ_ALL_ON, // AI_FACE_ON
	AI_PQ_DEMO, // AI_FACE_DEMO
	AI_PQ_SCENE_OFF,
	AI_PQ_FACE_OFF,
	AI_PQ_ALL_OFF, // no pq effect
	AI_PQ_MODE_NUM,
} AI_PQ_MODE;

typedef struct
{
	s32 frameIdx;
	s32 pv8;
	s32 cx12;
	s32 cy12;
	s32 w12;
	s32 h12;
	s32 range12;
	s32 Umax12;
	s32 Vmax12;
	s32 y_med12;
	s32 cb_med12;
	s32 cr_med12;
	s32 y_var12;
	s32 cb_var12;
	s32 cr_var12;
	s32 hue_med12;
	s32 sat_med12;
	s32 val_med12;
} AIInfo;  // from NN IP

enum {
	PQMASK_TUNNEL_K2A = 0,		// kernel to ap signal
	PQMASK_TUNNEL_A2K,			// ap to kernel signal
	PQMASK_TUNNEL_NUM
};

typedef struct{
	u32 PreStart;
	u32 PreEnd;
	u32 GraphStart;
	u32 GraphEnd;
	u32 PostStart;
	u32 PostEnd;
} PQMaskModelPerf;

typedef struct{
	u8 Update;
	u8 TimeStamp;
	u32 Histogram[6*3*AI_SEM_HIST_BIN];
	PQMaskModelPerf Perf;
} PQMaskFlag;

typedef struct {
	u8 *DataPtr;
	u32 *HistPtr;
	u8 TimeStamp;
	PQMaskModelPerf Perf;
} PQMaskInfo; // to gather the infomation and copy to PQMaskFlag for kernel

typedef enum{
	AI_FLAG_INPUT=0,
	AI_FLAG_INPUT_PARAM=1,
	AI_FLAG_OUTPUT=2,
}AI_FLAG_IDX;

typedef enum{
	AI_MC_VPQ_FACE=0,
	AI_MC_VPQ_SCENE,
	AI_MC_VPQ_RESOLUTION,
	AI_MC_VPQ_ANIMATION,
	AI_MC_VPQ_GENRE,
	AI_MC_VPQ_CONTENT,
	AI_MC_VPQ_OBJECT_SEGMENT,
	AI_MC_VPQ_DEPTH_MAP,
	AI_MC_VPQ_FACE_ID,
	AI_MC_VPQ_SEMANTIC,
	AI_MC_VPQ_LOGO,
	AI_MC_VPQ_NOISE,
	AI_MC_VPQ_MAX,
} AI_MC_VPQ_INDEX;

typedef enum{
	AI_SCENE_ANIMATION=0,
    AI_SCENE_NIGHT_BUILDING,
    AI_SCENE_CONCERT,
	AI_SCENE_FIREWORKS,
	AI_SCENE_GAME,
	AI_SCENE_LANDSCAPE,
	AI_SCENE_NEWS,
	AI_SCENE_OTHERs,
	AI_SCENE_SPORTs,
	AI_SCENE_TYPE_NUM,
} AI_SCENE_TYPE;

typedef enum{
	AI_RESOLUTION_2K_BAD=0,
	AI_RESOLUTION_2K_NORMAL,
	AI_RESOLUTION_4K_NORMAL,
	AI_RESOLUTION_4K_GOOD,
	AI_RESOLUTION_2KTIMING_2K_WORSE,
	AI_RESOLUTION_2KTIMING_2K_BAD,
	AI_RESOLUTION_2KTIMING_2K_NORMAL,
	AI_RESOLUTION_2KTIMING_2K_GOOD,
	AI_RESOLUTION_TYPE_NUM,
} AI_RESOLUTION_TYPE;

typedef enum{
    AI_ANIM_NON_ANIMA=0,
    AI_ANIM_ANIMATION,
    AI_ANIM_TYPE_NUM,
} AI_ANIM_TYPE;

typedef enum{
	AI_GENRE_MOVIE=0,
	AI_GENRE_ANIMATION,
	AI_GENRE_SPORT,
	AI_GENRE_OTHER,
	AI_GENRE_PATTERN,
	AI_GENRE_TYPE_NUM,
} AI_GENRE_TYPE;

typedef enum{
	AI_CONTENT_BUILDING=0,
	AI_CONTENT_DARK,
	AI_CONTENT_NATURE,
	AI_CONTENT_OTHER,
	AI_CONTENT_TYPE_NUM,
} AI_CONTENT_TYPE;

typedef enum{
	AI_OBJECT_BACKGROUND=0,
	AI_OBJECT_FACE,
	AI_OBJECT_BODY,
	AI_OBJECT_BICYCLE,
	AI_OBJECT_CAR,
	AI_OBJECT_MOTORBIKE,
	AI_OBJECT_AIRPLANE,
	AI_OBJECT_SHIP,
	AI_OBJECT_BIRD,
	AI_OBJECT_CAT,
	AI_OBJECT_DOG,
	AI_OBJECT_HORSE,
	AI_OBJECT_COW,
	AI_OBJECT_TYPE_NUM,
} AI_OBJECT_TYPE;

typedef enum{
	AI_NOISE_CLEAR=0,
	AI_NOISE_NOISY,
	AI_NOISE_TYPE_NUM,
} AI_NOISE_TYPE;


typedef enum{
	AI_EMOTION_SILENCE=0,
	AI_EMOTION_OTHER,
	AI_EMOTION_ANGERY,
	AI_EMOTION_HAPPY,
	AI_EMOTION_NEUTRAL,
	AI_EMOTION_SAD,
	AI_EMOTION_TYPE_NUM,
} AI_EMOTION_TYPE;

typedef struct _AI_PROCESS_INFO {
	s32 activated;
	s32 modelNum;
	char modelFile[AI_MAX_MODEL_FILE][AI_MAX_MODEL_NAME_LEN];		//max 10 model file per process
	s32 inputVideoWidth;
	s32 inputVideoHeight;
	s32	inputAudioFmt;
	AI_MC_VPQ_INDEX outputFunIdx;
	s32 targetFps;
	double  graphTimeMs;  // process time of each graph (ms)
	struct timespec_rtk  lastTime;  // last time to process graph
} AI_PROCESS_INFO;          
 
typedef struct _MODEL_CONTROL {
   s32  aiProcessNum;  					//max 10 ai process
   AI_PROCESS_INFO aiProcessInfo[AI_MAX_PROCESS];   //index corresponding to handle
   s32  curProcessOfUsePPU;    	//current process handle of using PPU
   s32  curProcessOfUseNNU;  		//current process handle of using NNU
   s32  curProcessOfUseOutput;  //current process handle of using output hw
} AI_CONTROL;

typedef enum{
	AI_HW_PPU=0,
	AI_HW_NNU,
	AI_HW_OUTPUT,
	AI_HW_INPUT,
	AI_HW_MAX,
	AI_HW_PNU	  // need PPU & NNU if you do not know exactly
} AI_HW_INDEX;

typedef struct _IOCTL_REGIST_MODEL_PARAM{
	s32 handle;
	AI_PROCESS_INFO info;
}IOCTL_REGIST_MODEL_PARAM;

typedef struct _IOCTL_GRANT_PARAM{
	s32 handle;
	AI_HW_INDEX index;
}IOCTL_GRANT_PARAM;

enum {
	AI_FACE_DETECT  = (1 << 0),
	AI_SCENE_DETECT = (1 << 1),
	AI_SQM_DETECT   = (1 << 2),
	AI_DEPTH_DETECT = (1 << 3),
	AI_OBJ_DETECT   = (1 << 4),
	AI_SEMATIC_DETECT   = (1 << 5),
	AI_NOISE_DETECT   = (1 << 6),
};
#if ( defined(CONFIG_RTK_AI_DRV) || defined(CONFIG_RTK_AI_DRV_MODULE) )
typedef struct _VIP_NN_MEMORY_CTRL {
	u32 size;
	u32 phy_addr_align;
	u32 *pVir_addr_align;

} VIP_NN_MEMORY_CTRL;
#else
// for case of 64bit kernel with 32bit AP
typedef struct _VIP_NN_MEMORY_CTRL {
	u32 size;
	u32 phy_addr_align;
	u32 *pVir_addr_align;
	u32 *reserve;	// reserve another 4 bytes to align with driver
} VIP_NN_MEMORY_CTRL;
#endif


typedef enum _VIP_NN_BUFFER_INDEX{
	VIP_NN_BUFFER_224FULL=0,		//scene,content
	VIP_NN_BUFFER_320FULL=1,		//ultraface
	VIP_NN_BUFFER_224GENRE=2,		//gerne
	VIP_NN_BUFFER_240X135CROP=3,	//SQM
	VIP_NN_BUFFER_192FULL=4,		//depth map
	VIP_NN_BUFFER_320X256=5,		//logo detect
	VIP_NN_BUFFER_256FULL=6,		//object segment
	VIP_NN_BUFFER_480FULL=7,		// semantic
	VIP_NN_BUFFER_240X136CROP=8,	//noise
	VIP_NN_BUFFER_MAX,
} VIP_NN_BUFFER_INDEX;

#define VIP_NN_BUFFER_NUM VIP_NN_BUFFER_MAX

typedef struct _VIP_NN_CTRL {
        //unsigned char PA_IDX; // SE write buffer idx
        VIP_NN_MEMORY_CTRL NN_flag_Addr[VIP_NN_BUFFER_NUM];
        VIP_NN_MEMORY_CTRL NN_data_Addr[VIP_NN_BUFFER_NUM];
        VIP_NN_MEMORY_CTRL NN_info_Addr[VIP_NN_BUFFER_NUM];
        //VIP_NN_MEMORY_CTRL NN_indx_Addr;
} VIP_NN_CTRL;

typedef struct {
	s32 bufferIndex;
	VIP_NN_CTRL nnCtrl;
} __attribute__((packed)) AI_NN_BUF_INFO;

#define VIP_NN_FLAG_BUFFER_DEFAULT_SIZE 64
#define VIP_NN_INFO_BUFFER_DEFAULT_SIZE 64

#define AI_MAX_FACE_NUM			6
#define AI_NOISE_BLOCK_NUM_PER_SECOND	8

#define AI_DEPTHMAP_WIDTH			288
#define AI_DEPTHMAP_HEIGHT			224
#define AI_OBJECT_WIDTH				480
#define AI_OBJECT_HEIGHT			256
#define AI_OBJECT_OUTPUT_WIDTH		480
#define AI_OBJECT_OUTPUT_HEIGHT		256
#define AI_SEMANTIC_WIDTH			480
#define AI_SEMANTIC_HEIGHT			256
#define AI_SEMANTIC_OUTPUT_WIDTH	480
#define AI_SEMANTIC_OUTPUT_HEIGHT	256

#endif	/* _AI_CMD_ID_H_ */
