#ifndef _VT_DEVICE_ID_H_
#define _VT_DEVICE_ID_H_

#ifdef BUILD_QUICK_SHOW
#include <sysdefs.h>
#endif


#define KADP_VT_VSCSOURCE_DISCONNECT 2
#define VT_NOT_ENOUGH_RESOURCE     0xFF

#define VT_CAP_SCALE_DOWN_LIMIT_WIDTH       128
#define VT_CAP_SCALE_DOWN_LIMIT_HEIGHT      68

#define VT_MAX_SUPPORT_BUF_NUMS      10

#define AP_MAX_HOLD_NUM         2

#define VT_CAP_BUF_NUMS      10

#define HDMI_HDCP_DRM_CASE   0x4

#define DP_HDCP_DRM_CASE     0x10

#define DEFAULT_DC2H_CAP_BURST_LEN  0x10

#define DC2H_NUMBER			2
/*-----------------------------------------------------------------------------
	Definition of struct for IO ctrl cmd(all parameters include in the same struct)
------------------------------------------------------------------------------*/

typedef enum _VT_PRINT_MASK_LEVEL
{
    VT_PRINT_MASK_FRAME_INDEX = 1, // 0x00000001, print vt buffer index
    VT_PRINT_MASK_FRAME_FPS = (1 << 1), // 0x00000002, print fps
    VT_PRINT_MASK_FRAME_ADDRESS = (1 << 2), // 0x00000004, print frame address
    VT_PRINT_MASK_OUT_FPS = (1<<3), // 0x00000008, print wait vsync sleep time
    VT_PRINT_MASK_DUMP_INDEX = (1<<4), // 0x00000010, dump buffer index which send to ap
    VT_PRINT_MASK_DRM_LOG = (1<<5), // 0x00000020, check DRM case
    VT_PRINT_MASK_ISR_LOG = (1<<6), // 0x00000040, print dc2h isr log
    VT_PRINT_MASK_RPC_INFO = (1<<7), // 0x00000080, print rpc info to video
    VT_PRINT_MASK_SHM_VTBUF_STATUS = (1<<8), // 0x00000100, print shm vt buffer status
    VT_PRINT_MASK_MAX_VT_BUF_NUMS = (1<<9), // 0x00000200, enable vt max supported buffer
    VT_PRINT_MASK_DEBUG_PATTERN1 = (1<<10), // 0x00000400, enable vt capture pattern 1
    VT_PRINT_MASK_DEBUG_PATTERN2 = (1<<11), // 0x00000800, enable vt capture pattern 2
    VT_PRINT_MASK_DEQUEUE_TWO_PATTERN = (1<<12), // 0x00001000, enable dequeue two pattern
}VT_PRINT_MASK_LEVEL;

typedef enum _VT_DC2H_IDLE_STATE
{
	DC2H_IDLE = 0,
	DC2H_OCCUPY
}VT_DC2H_IDLE_STATE;

typedef enum _DC2H_MEMORY_TYPE_
{
	DC2H_MEMORY_MMAP = 0,
	DC2H_MEMORY_DMA,
	DC2H_MEMORY_USERPTR
}DC2H_MEMORY_TYPE;

typedef enum _VT_DC2H_NUMBER
{
    DC2H1 = 0,
    DC2H2,
    DC2HX_MAXN
}VT_DC2H_NUMBER;

typedef enum _VT_APP_CASE
{
    VT_APP_CASE_NONE = 0,
    VT_APP_CASE_LIVE_STREAM,
    VT_APP_CASE_SCREEN_CAPTURE,
    VT_APP_CASE_VIDEO_RECORDING,
    VT_APP_CASE_MAXN
}VT_APP_CASE;

typedef enum _VT_CAP_FRAME_LINE_MODE
{
	DC2H_CAP_LINE_MODE = 0, /* line mode */
	DC2H_CAP_FRAME_MODE /* frame mode */
}VT_CAP_FRAME_LINE_MODE;

typedef enum _VT_CAP_SEQ_BLK_SEL
{
	DC2H_SEQ_MODE = 0, /* sequencial mode */
	DC2H_BLK_MODE /* block mode */
}VT_CAP_SEQ_BLK_SEL;

typedef enum _VT_CAP_PIXEL_ENCODING
{
	PIXEL_ENCODING_444 = 0,
	PIXEL_ENCODING_422,  /* reserved */
	PIXEL_ENCODING_420
}VT_CAP_PIXEL_ENCODING;

typedef enum _VT_CAP_BIT_SEL
{
	PER_CHANNEL_8_BIT = 0,
	PER_CHANNEL_10_BIT
}VT_CAP_BIT_SEL;

typedef enum _VT_CAP_YV12_ENABLE
{
	CAP_YV12_DISABLE = 0,
	CAP_YV12_ENABLE
}VT_CAP_YV12_ENABLE;

typedef enum _VT_CAP_P010_ENABLE
{
	CAP_P010_DISABLE = 0,
	CAP_P010_ENABLE
}VT_CAP_P010_ENABLE;

typedef enum _VT_CAP_CB_CR_SWAP
{
	DC2H_CB_CR_NO_SWAP = 0,
	DC2H_CB_CR_SWAP
}VT_CAP_CB_CR_SWAP;

typedef enum _VT_CAP_RGB_TO_ARGB_EN /* Enable convert RGB888 to ARGB8888 */
{
	RGB_TO_ARGB_DISABLE = 0,
	RGB_TO_ARGB_ENABLE
}VT_CAP_RGB_TO_ARGB_EN;

typedef enum _VT_CAP_CHANNEL_SWAP /* Channel Swap for RGB888/ARGB8888 */
{
	CHANNEL_SWAP_GBR = 0, /* YUV */
	CHANNEL_SWAP_GRB,
	CHANNEL_SWAP_RBG,
	CHANNEL_SWAP_BRG,
	CHANNEL_SWAP_BGR,
	CHANNEL_SWAP_RGB /* =5*/
}VT_CAP_CHANNEL_SWAP;

typedef enum _VT_CAP_ARGB_TYPE /* location of dummy data when ARGB8888 */
{
	CAP_ARGB = 0,
	CAP_RAGB, /* reserved */
	CAP_RGAB, /* reserved */
	CAP_RGBA
}VT_CAP_ARGB_TYPE;

struct planes_pa  //planes physical address
{
	unsigned int pa_y; //one plane start adress(ex:rgb buffer start address)
	unsigned int pa_u;
	unsigned int pa_v;
	unsigned int size_plane_y; //one plane size(ex:rgb buffer size)
	unsigned int size_plane_u;
	unsigned int size_plane_v;
};

typedef struct
{
	DC2H_MEMORY_TYPE mem_type; 
	unsigned int	pixelFormat;
	unsigned int 	plane_number;
	unsigned int    stride;
	unsigned int    width;
	unsigned int    height;
	unsigned int    vtbuffernumber;
	unsigned int    image_size;
	unsigned int    buf_size;
	struct planes_pa vt_pa_buf_info[VT_MAX_SUPPORT_BUF_NUMS];
} VT_VIDEO_FRAME_BUFFER_PROPERTY_INFO_T;

//-------------------------------------new structure end-------------------------------------------------------------

/**
 * @brief This enumeration describes the supported number of plane corresponding to plane type of video frame buffer.
 */
typedef enum
{
	KADP_VT_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED = 1,
	KADP_VT_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR,
	KADP_VT_VIDEO_FRAME_BUFFER_PLANE_PLANAR
} KADP_VT_VIDEO_FRAME_BUFFER_PLANE_NUMBER_T;

/**
 * @brief This enumeration describes the supported pixel format of video frame buffer.
 */
typedef enum
{
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV420_PLANAR = 0,
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV420_SEMI_PLANAR,
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV420_INTERLEAVED,
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV422_PLANAR,
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV422_SEMI_PLANAR,
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV422_INTERLEAVED,
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV444_PLANAR,
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV444_SEMI_PLANAR,
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV444_INTERLEAVED,
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB,
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ARGB
} KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_T;

/**
 * @brief This enumeration describes the supported video window.
 */
typedef enum
{
	KADP_VT_VIDEO_INVALID_WINDOW = -1,
	KADP_VT_VIDEO_WINDOW_0 = 0,
	KADP_VT_VIDEO_WINDOW_1 = 1,
	KADP_VT_VIDEO_WINDOW_2 = 2
} KADP_VT_VIDEO_WINDOW_TYPE_T;

/**
 * @brief This enumeration describes flags for state of video frame output device setting.
 */
typedef enum
{
	KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_NOFX 			= 0x00000000,
	KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_ENABLED 			= 0x00000001,
	KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FREEZED			= 0x00000002,
	KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_APPLIED_PQ		= 0x00000004,
	KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FRAMERATE_DIVIDE	= 0x00000010
} KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FLAGS_T;

/**
 * @brief This enumeration describes the dump location of video memory.
 */
typedef enum
{
	KADP_VT_SCALER_INPUT = 0,
	/*KADP_VT_SCALER_OUTPUT = 0,*/
	KADP_VT_SCALER_OUTPUT,
	KADP_VT_DISPLAY_OUTPUT,
	KADP_VT_OSDVIDEO_OUTPUT,
	KADP_VT_OSD_OUTPUT,
	KADP_VT_MAX_LOCATION
} KADP_VT_DUMP_LOCATION_TYPE_T;

/**
 * @brief This enumeration describes the type of video.
 */
typedef enum
{
	KADP_VT_VIDEO_INTERLACED	= 0,
	KADP_VT_VIDEO_PROGRESSIVE
} KADP_VT_VIDEO_TYPE_T;


/* VT buffer status is share memory between video dc2h isr and kdriver vt module */
typedef enum _VT_BUFFER_STATUS
{
    VT_BUFFER_AP_ENQUEUED = 0,  //0--dc2h can write this buffer, or AP enqueue buffer to VT driver
    VT_BUFFER_AP_DEQUEUED,       //1--AP dequeued this buffer,dc2h can't write it
    VT_BUFFER_DC2H_CAP_DONE,    //2--dc2h capture done
    VT_BUFFER_DC2H_CAPTURING,    //3--dc2h is capturing
    VT_BUFFER_UNKNOW_STATUS
}VT_BUFFER_STATUS;

typedef struct
{
	VT_BUFFER_STATUS bufSta[VT_CAP_BUF_NUMS];
}VT_BUFFER_STATUS_ST;

/*--------------------------------------------------------------------------------------------
@ dc2h share memory info structure
@ parma1: vt buffer status
@ param2: hw write index
@ param3: AP read index
@ parma4: printMask -> vt print mask for video/kernel vt flow
*--------------------------------------------------------------------------------------------*/
typedef struct
{
	VT_BUFFER_STATUS_ST bufStatus[DC2H_NUMBER];
    unsigned int write[DC2H_NUMBER];
    unsigned int read[DC2H_NUMBER];
    unsigned int printMask;
}VT_BUF_STATUS_T;

/**
 * @brief This structure describes a rectangle specified by a point and a dimension.
 */
typedef struct
{
	unsigned short	x;
	unsigned short	y;
	unsigned short	w;
	unsigned short	h;
} KADP_VT_RECT_T;

/**
 * @brief This structure describes the capability of video device.
 */
typedef struct
{
	unsigned int	numOfVideoWindow;
} KADP_VT_DEVICE_CAPABILITY_INFO_T;

/**
 * @brief This structure describes the capability of video frame buffer.
 */
typedef struct
{
	unsigned int numOfVideoFrameBuffer;
	KADP_VT_VIDEO_FRAME_BUFFER_PLANE_NUMBER_T numOfPlane;
} KADP_VT_VIDEO_FRAME_BUFFER_CAPABILITY_INFO_T;

/**
 * @brief This structure describes the capability of video frame output device.
 */
typedef struct
{
	unsigned char	bSupportedControlEnabled;
	unsigned char	bSupportedControlFreezed;
	unsigned char	bSupportedControlFramerateDivide;
	unsigned char	bSupportedControlPQ;
} KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_CAPABILITY_INFO_T;

/**
 * @brief This structure describes the limitation of video frame output device.
 */
typedef struct {
	KADP_VT_RECT_T 	maxResolution;
	unsigned char	bLeftTopAlign;
	unsigned char	bSupportInputVideoDeInterlacing;
	unsigned char	bSupportDisplayVideoDeInterlacing;
	unsigned char	bSupportScaleUp;
	unsigned int	scaleUpLimitWidth;
	unsigned int	scaleUpLimitHeight;
	unsigned char	bSupportScaleDown;
	unsigned int	scaleDownLimitWidth;
	unsigned int	scaleDownLimitHeight;
} KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_LIMITATION_INFO_T;

/**
 * @brief This structure describes the constant property of video frame buffer.
 * These member values are unique properties of video frame buffer. it is not able to be changed.
 */
typedef struct
{
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_T pixelFormat;
	unsigned int	stride;
	unsigned int	width;
	unsigned int	height;
    unsigned int	ppPhysicalAddress[VT_MAX_SUPPORT_BUF_NUMS];
	unsigned int	vfbbuffernumber;
	unsigned int    vtYbufSize;
} KADP_VT_VIDEO_FRAME_BUFFER_PROPERTY_INFO_T;

/**
 * @brief This structure describes state information of video frame output device.
 */
typedef struct
{
	unsigned char	bEnabled;
	unsigned char	bFreezed;
	unsigned char	bAppliedPQ;
	unsigned int	framerateDivide;
} KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_INFO_T;

/**
 * @brief This structure describes information of input video.
 */

typedef enum
{
    KADP_VT_VIDEO_COLOR_SPACE_UNKNOWN =0,
    KADP_VT_VIDEO_COLOR_SPACE_BT601,
    KADP_VT_VIDEO_COLOR_SPACE_BT709,
    KADP_VT_VIDEO_COLOR_SPACE_BT2020,
    KADP_VT_VIDEO_COLOR_SPACE_BT601_FULL_RANGE,
    KADP_VT_VIDEO_COLOR_SPACE_BT709_FULL_RANGE,
} KADP_VT_VIDEO_COLOR_SPACE_T;

typedef struct {
	KADP_VT_VIDEO_TYPE_T			type;
	KADP_VT_RECT_T				region;
	unsigned char				bIs3DVideo;
    KADP_VT_VIDEO_COLOR_SPACE_T  colorSpace;
} KADP_VT_INPUT_VIDEO_INFO_T;

/**
 * @brief This structure describes information of output video.
 */
typedef struct {
	KADP_VT_VIDEO_TYPE_T			type;
	KADP_VT_RECT_T				maxRegion;
	KADP_VT_RECT_T				activeRegion;
} KADP_VT_OUTPUT_VIDEO_INFO_T;

/**
 * @brief This structure describes output information of video frame output device.
 */
typedef struct {
	unsigned int	stride;
	KADP_VT_RECT_T	maxRegion;
	KADP_VT_RECT_T	activeRegion;
} KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_OUTPUT_INFO_T;

/**
 * @brief This structure describes information of captured video frame
 * 		This legacy structure will be used to support legacy chipset. (M14, H13)
 * 		Then this legacy structure is not used from latest chip (H15, LM15U).
 */
#if defined(CONFIG_COMPAT) && defined(CONFIG_ARM64)
typedef struct
{
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_T pixelFormat;
	unsigned short	stride;
	unsigned short	width;
	unsigned short	height;
	unsigned int	pVideoFrameAddress[3];
} KADP_VT_LEGACY_VIDEO_FRAME_INFO_T;
#else
typedef struct
{
	KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_T pixelFormat;
	unsigned short	stride;
	unsigned short	width;
	unsigned short	height;
	unsigned char	*pVideoFrameAddress[3];
} KADP_VT_LEGACY_VIDEO_FRAME_INFO_T;
#endif

/*-----------------------------------------------------------------------------
	Definition of struct for IO ctrl cmd(all parameters include in the same struct)
------------------------------------------------------------------------------*/
/*
VT_IOC_GET_VFB_CAPABILITY
*/
typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	KADP_VT_VIDEO_FRAME_BUFFER_CAPABILITY_INFO_T vfbCapInfo;
}KADP_VT_VFB_CAPINFO_T;

/*
VT_IOC_GET_VFOD_CAPABILITY
*/
typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_CAPABILITY_INFO_T vfodCapInfo;
}KADP_VT_VFOD_CAPINFO_T;

/*
VT_IOC_GET_VFOD_LIMITATION
*/
typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_LIMITATION_INFO_T vfodlimitationInfo;
}KADP_VT_VFOD_LIMITATIONINFO_T;

/*
VT_IOC_GET_ALLVFB_PROPERTY

*/
typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	KADP_VT_VIDEO_FRAME_BUFFER_PROPERTY_INFO_T vfbProInfo;
}KADP_VT_VFB_PROPERTY_T;

/*
VT_IOC_GET_VFB_INDEX
*/
typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	unsigned int IndexCurVFB;
}KADP_VT_GET_VFBINDEX_T;

/*
VT_IOC_GET_VFOD_STATE
*/
typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_INFO_T VFODstate;
}KADP_VT_VFOD_GET_STATEINFO_T;

/*
VT_IOC_SET_VFOD_STATE
*/
typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FLAGS_T vfodStateFlag;
	KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_INFO_T VFODstate;
}KADP_VT_VFOD_SET_STATEINFO_T;


/*
VT_IOC_GET_VFOD_FRAMERATE
*/
typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	unsigned int framerate;
}KADP_VT_VFOD_GET_FRAMERATE_T;

typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	KADP_VT_DUMP_LOCATION_TYPE_T DumpLocation;
}KADP_VT_VFOD_GET_DUMPLOCATION_T;

typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	KADP_VT_DUMP_LOCATION_TYPE_T DumpLocation;
}KADP_VT_VFOD_SET_DUMPLOCATION_T;

typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	KADP_VT_DUMP_LOCATION_TYPE_T DumpLocation;
	KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_OUTPUT_INFO_T OutputInfo;
}KADP_VT_VFOD_GET_OUTPUTINFO_T;

typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	KADP_VT_DUMP_LOCATION_TYPE_T DumpLocation;
	KADP_VT_RECT_T OutputRegion;
}KADP_VT_VFOD_SET_OUTPUTREGION_T;

typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	KADP_VT_INPUT_VIDEO_INFO_T InputVideoInfo;
}KADP_VT_GET_INPUTVIDEOINFO_T;

typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	KADP_VT_OUTPUT_VIDEO_INFO_T OutputVideoInfo;
}KADP_VT_GET_OUTPUTVIDEOINFO_T;

typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	unsigned char bOnOff;
}KADP_VT_GET_VIDEOMUTESTATUS_T;

typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	unsigned char IsSecureVideo;
}KADP_VT_GET_VFBSVSTATE_T;

typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	unsigned char bBlockState;
}KADP_VT_GET_VFODBLOCKSTATE_T;

typedef struct
{
	KADP_VT_VIDEO_WINDOW_TYPE_T wid;
	unsigned char bBlockState;
}KADP_VT_SET_VFODBLOCKSTATE_T;

typedef struct
{
	bool enable;
	KADP_VT_RECT_T OutputRegion;
	unsigned long phyaddr;
	unsigned int	width;
	unsigned int	height;	
}KADP_VT_ROWDATA_SHOW_ONSUBDISP_T;

typedef struct {
	unsigned char *uncache;
	void *cache;
	unsigned long phyaddr;
	unsigned int size;
	unsigned int getsize;
} VT_CAPTURE_BUFFER_T;

typedef struct {
	VT_CAPTURE_BUFFER_T cap_buffer[VT_MAX_SUPPORT_BUF_NUMS];
} VT_CAPTURE_CTRL_T;

typedef enum _VT_CAP_FMT{
	VT_CAP_RGB888 = 0,
	VT_CAP_ARGB8888,
	VT_CAP_RGBA8888,
	VT_CAP_ABGR8888,
	VT_CAP_NV12,
	VT_CAP_NV12_10BIT,
	VT_CAP_P010_10BIT,
	VT_CAP_NV21,
	VT_CAP_YV12, /* 3 plane */
	VT_CAP_I420, /* 3 plane */
	VT_CAP_MAX
}VT_CAP_FMT;

typedef struct {
	unsigned char enable;
	VT_DC2H_IDLE_STATE idle_state;
	KADP_VT_RECT_T input_size;
	KADP_VT_RECT_T output_size;
	KADP_VT_RECT_T crop_size;
	unsigned int bufnum;
	unsigned int fmt;
	unsigned int cap_location;
}VT_CUR_CAPTURE_INFO;
/*-----------------------------------------------------------------------------
	rpc struct
------------------------------------------------------------------------------*/
typedef struct{
	unsigned int SwModeEnable;
	unsigned int buffernumber;
	struct planes_pa cap_buffers[VT_CAP_BUF_NUMS];
	unsigned int cap_format;
	unsigned int cap_width;
	unsigned int cap_length;
    unsigned int user_fps;
}DC2H_SWMODE_STRUCT_T;

typedef struct 
{ /* se input: src_info, se output: dst_info */
	unsigned int dst_x;
	unsigned int dst_y;
	unsigned int dst_w;
	unsigned int dst_h;
	unsigned int dst_phyaddr;
	unsigned int dst_phyaddr_uv;
	unsigned int dst_fmt;
	unsigned int src_x;
	unsigned int src_y;
	unsigned int src_w;
	unsigned int src_h;
	unsigned int src_phyaddr;
	unsigned int src_phyaddr_uv;
	unsigned int src_fmt;
	unsigned int src_pitch_y;
	//unsigned int src_pitch_c;
} SE_SRC_DST_info;

/*-----------------------------------------------------------------------------
	System call
------------------------------------------------------------------------------*/
#define VT_DEVICE_NUM								   1
#define VT_DEVICE_NAME								"vivtdev"
#define VT_IOC_MAGIC								  't'

#define VT_MAJOR 0   /* dynamic major by default */
#define VT_NR_DEVS 1    /* se0 through se3 */

//index range
//0 ~ 255
#define VT_IOC_START_INDEX 0
enum vt_ioc_idx
{
	VT_IOC_IDX_0 = VT_IOC_START_INDEX,//0
	VT_IOC_IDX_1,
	VT_IOC_IDX_2,
	VT_IOC_IDX_3,
	VT_IOC_IDX_4,
	VT_IOC_IDX_5,
	VT_IOC_IDX_6,
	VT_IOC_IDX_7,
	VT_IOC_IDX_8,
	VT_IOC_IDX_9,
	VT_IOC_IDX_10,
	VT_IOC_IDX_11,
	VT_IOC_IDX_12,
	VT_IOC_IDX_13,
	VT_IOC_IDX_14,
	VT_IOC_IDX_15,
	VT_IOC_IDX_16,
	VT_IOC_IDX_17,
	VT_IOC_IDX_18,
	VT_IOC_IDX_19,
	VT_IOC_IDX_20,
	VT_IOC_IDX_21,
	VT_IOC_IDX_22,
	VT_IOC_IDX_23,
	VT_IOC_IDX_24,
	VT_IOC_IDX_25,
	VT_IOC_IDX_26,
	VT_IOC_IDX_27,
	VT_IOC_IDX_28,
	VT_IOC_IDX_29,
	VT_IOC_IDX_30,
	VT_IOC_IDX_31,
	VT_IOC_IDX_32,
	VT_IOC_MAXNR,
};

#define VT_IOC_INIT										_IO(VT_IOC_MAGIC,  VT_IOC_IDX_1)
#define VT_IOC_FINALIZE									_IO(VT_IOC_MAGIC,  VT_IOC_IDX_2)
#define VT_IOC_GET_DEVICE_CAPABILITY					_IOR(VT_IOC_MAGIC, VT_IOC_IDX_3, unsigned int)
#define VT_IOC_GET_VFB_CAPABILITY						_IOR(VT_IOC_MAGIC, VT_IOC_IDX_4, KADP_VT_VFB_CAPINFO_T)
#define VT_IOC_GET_VFOD_CAPABILITY						_IOR(VT_IOC_MAGIC, VT_IOC_IDX_5, KADP_VT_VFOD_CAPINFO_T)
#define VT_IOC_GET_VFOD_LIMITATION						_IOR(VT_IOC_MAGIC, VT_IOC_IDX_6, KADP_VT_VFOD_CAPINFO_T)
#define VT_IOC_GET_ALLVFB_PROPERTY						_IOR(VT_IOC_MAGIC, VT_IOC_IDX_7, KADP_VT_VFB_PROPERTY_T)
#define VT_IOC_GET_VFB_INDEX							_IOR(VT_IOC_MAGIC, VT_IOC_IDX_8, KADP_VT_GET_VFBINDEX_T)
#define VT_IOC_GET_VFOD_STATE							_IOR(VT_IOC_MAGIC, VT_IOC_IDX_9, KADP_VT_VFOD_GET_STATEINFO_T)
#define VT_IOC_SET_VFOD_STATE							_IOW(VT_IOC_MAGIC, VT_IOC_IDX_10, KADP_VT_VFOD_SET_STATEINFO_T)
#define VT_IOC_GET_VFOD_FRAMERATE						_IOR(VT_IOC_MAGIC, VT_IOC_IDX_11, KADP_VT_VFOD_GET_FRAMERATE_T)
#define VT_IOC_GET_VFOD_DUMPLOCATION					_IOR(VT_IOC_MAGIC, VT_IOC_IDX_12, KADP_VT_VFOD_GET_DUMPLOCATION_T)
#define VT_IOC_SET_VFOD_DUMPLOCATION					_IOW(VT_IOC_MAGIC, VT_IOC_IDX_13, KADP_VT_VFOD_SET_DUMPLOCATION_T)
#define VT_IOC_GET_VFOD_OUTPUTINFO						_IOR(VT_IOC_MAGIC, VT_IOC_IDX_14, KADP_VT_VFOD_GET_OUTPUTINFO_T)
#define VT_IOC_SET_VFOD_OUTPUTREGION					_IOW(VT_IOC_MAGIC, VT_IOC_IDX_15, KADP_VT_VFOD_SET_OUTPUTREGION_T)
#define VT_IOC_WAIT_VSYNC								_IOW(VT_IOC_MAGIC, VT_IOC_IDX_16, KADP_VT_VIDEO_WINDOW_TYPE_T)
#define VT_IOC_GET_INPUTVIDEOINFO						_IOR(VT_IOC_MAGIC, VT_IOC_IDX_17, KADP_VT_GET_INPUTVIDEOINFO_T)
#define VT_IOC_GET_OUTPUTVIDEOINFO						_IOR(VT_IOC_MAGIC, VT_IOC_IDX_18, KADP_VT_GET_OUTPUTVIDEOINFO_T)
#define VT_IOC_GET_VIDEOMUTESTATUS						_IOR(VT_IOC_MAGIC, VT_IOC_IDX_19, KADP_VT_GET_VIDEOMUTESTATUS_T)
#define VT_IOC_GET_VFB_SVSTATE							_IOR(VT_IOC_MAGIC, VT_IOC_IDX_20, KADP_VT_GET_VFBSVSTATE_T)
#define VT_IOC_GET_VFOD_BLOCKSTATE						_IOR(VT_IOC_MAGIC, VT_IOC_IDX_21, KADP_VT_GET_VFODBLOCKSTATE_T)
#define VT_IOC_SET_VFOD_BLOCKSTATE						_IOR(VT_IOC_MAGIC, VT_IOC_IDX_22, KADP_VT_SET_VFODBLOCKSTATE_T)
#define VT_IOC_VFB_DATASHOW_ONSUBDISP					_IOR(VT_IOC_MAGIC, VT_IOC_IDX_23, unsigned char)
#define VT_IOC_INITEX									_IOR(VT_IOC_MAGIC, VT_IOC_IDX_24, unsigned int)
#define VT_IOC_SET_PIXEL_FORMAT							_IOW(VT_IOC_MAGIC, VT_IOC_IDX_25, unsigned int)
#define VT_IOC_ROWDATA_SHOW_ONSUBDISP					_IOW(VT_IOC_MAGIC, VT_IOC_IDX_26, KADP_VT_ROWDATA_SHOW_ONSUBDISP_T)
#define VT_IOC_EnableFRCMode							_IOR(VT_IOC_MAGIC, VT_IOC_IDX_27, unsigned int)
#define VT_IOC_SET_OUTPUT_FPS                           _IOW(VT_IOC_MAGIC, VT_IOC_IDX_28, unsigned int)
#define VT_IOC_SET_APP_CASE                             _IOW(VT_IOC_MAGIC, VT_IOC_IDX_29, unsigned int)
#define VT_IOC_AP_ENQUEUE_INDEX                         _IOW(VT_IOC_MAGIC, VT_IOC_IDX_30, unsigned int)
#define VT_IOC_AP_DEQUEUE_INDEX                         _IOW(VT_IOC_MAGIC, VT_IOC_IDX_31, unsigned int)
#define VT_IOC_AP_SET_CROP                              _IOW(VT_IOC_MAGIC, VT_IOC_IDX_32, unsigned int)


#endif

