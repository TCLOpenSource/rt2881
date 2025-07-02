#ifndef _AICAM_CMD_ID_H_
#define _AICAM_CMD_ID_H_

/************************************************************************
 *  Include files
 ************************************************************************/
#include "ai_types.h"

/************************************************************************
 *  Definitions
 ************************************************************************/
#define AICAM_DEVICE_NAME						"rtk-aicam"
#define AICAM_IOC_MAGIC 'a'

enum aicam_ioc_cmd
{
	AICAM_IOC_CMD_INIT =				0,
	AICAM_IOC_CMD_UNINIT = 				1,
	AICAM_IOC_CMD_OPEN = 				2,
	AICAM_IOC_CMD_CLOSE = 				3,
	AICAM_IOC_CMD_GET_SHARE_MEM = 		4,
	AICAM_IOC_CMD_SET_SCALER 	= 		5,
	AICAM_IOC_CMD_SET_AUDIO		=		6,
	AICAM_IOC_CMD_GET_AUDIO		=		7,
};

#define AICAM_IOC_INIT                     _IO(AICAM_IOC_MAGIC, AICAM_IOC_CMD_INIT)
#define AICAM_IOC_UNINIT                   _IO(AICAM_IOC_MAGIC, AICAM_IOC_CMD_UNINIT)
#define AICAM_IOC_OPEN                     _IO(AICAM_IOC_MAGIC, AICAM_IOC_CMD_OPEN)
#define AICAM_IOC_CLOSE                    _IO(AICAM_IOC_MAGIC, AICAM_IOC_CMD_CLOSE)
#define AICAM_IOC_GET_SHARE_MEM            _IOR(AICAM_IOC_MAGIC, AICAM_IOC_CMD_GET_SHARE_MEM, unsigned int)
#define AICAM_IOC_SET_SCALER               _IOW(AICAM_IOC_MAGIC, AICAM_IOC_CMD_SET_SCALER, unsigned int)
#define AICAM_IOC_SET_AUDIO                _IOW(AICAM_IOC_MAGIC, AICAM_IOC_CMD_SET_AUDIO, unsigned int)
#define AICAM_IOC_GET_AUDIO                _IOR(AICAM_IOC_MAGIC, AICAM_IOC_CMD_GET_AUDIO, unsigned int)

#define AI_CAM_MAX_FRAME_NUM			3
#define AI_CAM_MAX_W					1920
#define AI_CAM_MAX_H					1080
#define AI_CAM_MODEL_MAX_OUTPUT_W		960
#define AI_CAM_MODEL_MAX_OUTPUT_H		1080//540
#define ALIGN_4K(s) (((s) < 0x1000) ? 0x1000 : (((s) & 0xfff) == 0 ? (s) : (((s) + 0xfff)&(~0xfff))))
#define ALIGN_96(s) ((s/96)*96)
#define ALIGN_12K(s) (((s) < 0x3000) ? 0x3000 : (((s) & 0x2fff) == 0 ? (s) : (((s) + 0x2fff)&(~0x2fff))))


typedef enum{
	AICAM_SCALER_START=1,
	AICAM_SCALER_STOP=2,
	AICAM_SCALER_FLUSH=3,
} AICAM_SCALER_ACT;

typedef struct _AI_RECT {
     u32 x;
     u32 y;
     u32 w;
     u32 h;
}AI_RECT;

typedef struct _AI_AUDIO {
     u32 isVoiced;
     s32 micAzimuth;
}AI_AUDIO;

#if ( defined(CONFIG_RTK_AI_DRV) || defined(CONFIG_RTK_AI_DRV_MODULE) )
typedef struct _AI_CAM_FRAME {
     u32 width;
     u32 height;
     u32 phy_addr_align;
     u32 result_phy_addr_align;
     u32 *pVir_addr_align;
     u32 *result_pVir_addr_align;
     AI_RECT inregion;
     AI_RECT outregion;
     struct timespec_rtk ts_valid;
     struct timespec_rtk ts_detect;
     struct timespec_rtk ts_scale;
     u32 isValid;
     u32 detectDone;
     u32 scaleDone;
     u32 ai_input_vir;
} AI_CAM_FRAME;

typedef struct _AI_CAM_INFO {
	u32 modelId;
	u32 frame_max;
	u32 rtkcam_cur_wd;
	u32 rtkcam_pre_wd;
	u32 rtkcam_ai_rd;
	u32 rtkcam_scale_rd;
	u32 scaler_rd;
	u32 cam_w;
	u32 cam_h;
	u32 cam_fmt;
	u32 iv_src_sel;
    AI_RECT outregion;
	u32 *latest_buffer;
	u32 *latest_result;
	AI_RECT result_rect;
	struct timespec_rtk latest_timestamp;
	//osal_sem_t bufferLock;
	//osal_sem_t resultLock;
	AI_CAM_FRAME frame[AI_CAM_MAX_FRAME_NUM];
} AI_CAM_INFO;
#else
typedef struct _AI_CAM_FRAME {
	u32 width;
	u32 height;
	u32 phy_addr_align;
	u32 result_phy_addr_align;
	u32 *pVir_addr_align;	//NV12 format
	u32 *reserve1;	// reserve another 4 bytes to align with driver
	u32 *result_pVir_addr_align;	//NV12 format
	u32 *reserve2;	// reserve another 4 bytes to align with driver
    AI_RECT inregion;
    AI_RECT outregion;
	struct timespec_rtk ts_valid;
	struct timespec_rtk ts_detect;
	struct timespec_rtk ts_scale;
	u32 isValid;
	u32 detectDone;
	u32 scaleDone;
    u32 ai_input_vir;
} AI_CAM_FRAME;

typedef struct _AI_CAM_INFO {
	u32 modelId;
	u32 frame_max;
	u32 rtkcam_cur_wd;
	u32 rtkcam_pre_wd;
	u32 rtkcam_ai_rd;
	u32 rtkcam_scale_rd;
	u32 scaler_rd;
	u32 cam_w;
	u32 cam_h;
	u32 cam_fmt;
	u32 iv_src_sel;
    AI_RECT outregion;
	u32 *latest_buffer;		//YU12 format
	u32 *reserve1;	// reserve another 4 bytes to align with driver
	u32 *latest_result;		//YU12 format
	u32 *reserve2;	// reserve another 4 bytes to align with driver
	AI_RECT result_rect;
	struct timespec_rtk latest_timestamp;
	//osal_sem_t bufferLock;
	//osal_sem_t resultLock;
	AI_CAM_FRAME frame[AI_CAM_MAX_FRAME_NUM];
} AI_CAM_INFO;
#endif

#endif	/* _AICAM_CMD_ID_H_ */
