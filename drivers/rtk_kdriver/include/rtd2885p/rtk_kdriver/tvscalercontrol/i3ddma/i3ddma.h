#ifndef __I3DDMA_H__
#define __I3DDMA_H__

#ifdef __cplusplus
extern "C" {
#endif
//#include <rbus/rbusTypes.h>
//#include <rtd_types.h>
#include <tvscalercontrol/scaler/scalerstruct.h>
//#include <OSAL/PLI.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <rbus/h3ddma_reg.h>
#include <rbus/vgip_reg.h>
#include <vo/rtk_vo.h>

#define I3DDMA_PRINTF printf

#define I3DDMA_HAVE_HDMI_DMA        			1
#define I3DDMA_HAVE_HDMI_DITHER_12X10		1
#define I3DDMA_HAVE_HDMI_3D_TG				1
#define I3DDMA_CAP_BUF_NUM	8
#define I3DDMA_MAIN_CAP_BUF_NUM (I3DDMA_CAP_BUF_NUM - 2)
#define CAP_BUF_NUM_MAX  (8)

#ifndef rtd_maskl
#define rtd_maskl(x, y, z)	IoReg_Mask32(x, y, z)
#endif

#ifndef rtd_outl
//#define rtd_outl(x, y)		IoReg_Write32(x, y)
#endif

#ifndef rtd_inl
#define rtd_inl(x)			IoReg_Read32(x)
#endif

#define _SYNC_HN_VN         _BIT0
#define _SYNC_HP_VN         _BIT1
#define _SYNC_HN_VP         _BIT2
#define _SYNC_HP_VP         _BIT3
#define _1M_SIZE            (1024 * 1024)
#define _4K_WID             4096
#define _4K_WID_3840        3840
#define _3K_1K_WID          2560
#define _3K_1K_LEN          1440
#define _2K_LEN             2160
#define _2K_WID             1920
#define _1K_LEN             1080
#define _48M_SIZE           (48*1024*1024)
#define _24M_SIZE           (24*1024*1024)
#define _24HZ               (24)
#define _48HZ               (48)
#define _30HZ               (30)
#define _50HZ               (50)
#define _60HZ               (60)
#define _2K_LEN_1440P       (1440)
#define _1K_WID_960P        (960)
#define _1K_LEN_576p        (576)
#define _VTOTAL_4K          (2250)
#define VO_FIXED_4K2K_HTOTAL 4400
#define VO_FIXED_4K2K_VTOTAL 2250

#ifdef CONFIG_BW_96B_ALIGNED
    #define _4K_2K_10BIT_SIZE (drvif_memory_get_data_align((_4K_WID * 24 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * _2K_LEN)
#ifndef CONFIG_RTK_KDRV_DV_IDK_DUMP
    #define _2K_1K_10BIT_SIZE (drvif_memory_get_data_align((1920 * 24 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * 1080)
#else
    #define _2K_1K_10BIT_SIZE (drvif_memory_get_data_align((1920 * 30 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * 1080)
#endif
    #define _4K_2K_36BIT_SIZE (((_4K_WID * 36 + 256) / 8) * _2K_LEN)
    #define _4K_2K_12BIT_SIZE (drvif_memory_get_data_align((_4K_WID * 12 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * _2K_LEN)
    #define _4K_2K_20BIT_SIZE (drvif_memory_get_data_align((_4K_WID * 20 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * _2K_LEN)
    #define _2K_1K_12BIT_SIZE (drvif_memory_get_data_align((_2K_WID * 12 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * _1K_LEN)
    #define _2K_1K_36BIT_SIZE (drvif_memory_get_data_align((_2K_WID * 36 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * _1K_LEN)
    #define _2K_1K_20BIT_SIZE (drvif_memory_get_data_align((_2K_WID * 20 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * _1K_LEN)
    #define _2K_1K_24BIT_SIZE (drvif_memory_get_data_align((_2K_WID * 24 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * _1K_LEN)
    #define _3K_1K_12BIT_SIZE (drvif_memory_get_data_align((_3K_1K_WID * 12 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * _3K_1K_LEN)
    #define _3K_1K_20BIT_SIZE (drvif_memory_get_data_align((_3K_1K_WID * 20 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * _3K_1K_LEN)
    #define _4K_2K_MDOMAIN_16BIT_SIZE (drvif_memory_get_data_align((_4K_WID_3840 * 16 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * _2K_LEN)
#else
    #define _4K_2K_36BIT_SIZE (calc_i3ddma_comp_line_sum_bit(_4K_WID, 36, 0) * _2K_LEN * 16)
    #define _4K_2K_10BIT_SIZE (calc_i3ddma_comp_line_sum_bit(_4K_WID, 24, 0) * _2K_LEN * 16)
    #define _4K_2K_12BIT_SIZE (calc_i3ddma_comp_line_sum_bit(_4K_WID, 12, 0) * _2K_LEN * 16)
    #define _4K_2K_20BIT_SIZE (calc_i3ddma_comp_line_sum_bit(_4K_WID, 20, 0) * _2K_LEN * 16)
    #define _2K_1K_36BIT_SIZE (calc_i3ddma_comp_line_sum_bit(_2K_WID, 36, 0) * _1K_LEN * 16)
    #define _2K_1K_10BIT_SIZE (calc_i3ddma_comp_line_sum_bit(_2K_WID, 24, 0) * _1K_LEN * 16)
    #define _2K_1K_12BIT_SIZE (calc_i3ddma_comp_line_sum_bit(_2K_WID, 12, 0) * _1K_LEN * 16)
    #define _2K_1K_20BIT_SIZE (calc_i3ddma_comp_line_sum_bit(_2K_WID, 20, 0) * _1K_LEN * 16)
    #define _2K_1K_24BIT_SIZE (calc_i3ddma_comp_line_sum_bit(_2K_WID, 24, 0) * _1K_LEN * 16)
    #define _3K_1K_12BIT_SIZE (calc_i3ddma_comp_line_sum_bit(_3K_1K_WID, 12, 0) * _3K_1K_LEN * 16)
    #define _3K_1K_20BIT_SIZE (calc_i3ddma_comp_line_sum_bit(_3K_1K_WID, 20, 0) * _3K_1K_LEN * 16)

    #define _4K_2K_MDOMAIN_16BIT_SIZE ((_4K_WID_3840 * 16 + 256)  * _2K_LEN / 8)
#endif

#ifdef CONFIG_BW_96B_ALIGNED
    #define _2K_2K_8BIT_SIZE (drvif_memory_get_data_align((_2K_WID * 8 + 256) / 8, DMA_SPEEDUP_ALIGN_VALUE) * _2K_LEN)
#else
    #define _2K_2K_8BIT_SIZE  ((_2K_WID * 8 + 256)  * _2K_LEN / 8)
#endif

//#include <tvscalercontrol/hdmiRx/hdmiFun.h>
//#include <rbusHDMIReg.h>
//#include <rbusDDCReg.h>

//#include <tvscalercontrol/scaler/scalerStruct.h>
//#include <hdmiPlatform.h>

typedef enum {
	I3DDMA_FRAME_PACKING = 0,
	I3DDMA_FIELD_ALTERNATIVE = 1,
	I3DDMA_LINE_ALTERNATIVE = 2,
	I3DDMA_SIDE_BY_SIDE_FULL = 3,
	I3DDMA_L_DEPTH = 4,
	I3DDMA_L_DEPTH_GPX = 5,
	I3DDMA_TOP_AND_BOTTOM =6,
	I3DDMA_FRAMESEQUENCE=7,
	I3DDMA_SIDE_BY_SIDE_HALF = 8,
	// --- new 3D format ---
	I3DDMA_VERTICAL_STRIPE,
	I3DDMA_CHECKER_BOARD,
	I3DDMA_REALID,
	I3DDMA_SENSIO,
	// -------------------
	I3DDMA_RSV1,
	I3DDMA_2D_ONLY,
	I3DDMA_UNKOWN = 0xFFFFFFFF
} I3DDMA_3D_T;

typedef enum {
	I3DDMA_3D_CHESS_FMT_0 = 0,
	I3DDMA_3D_CHESS_FMT_1
}I3DDMA_3D_CHESS_FMT_T;

typedef enum {
	I3DDMA_3D_OPMODE_DISABLE = 0,
	I3DDMA_3D_OPMODE_HW,
	I3DDMA_3D_OPMODE_HW_2DONLY_L,
	I3DDMA_3D_OPMODE_HW_2DONLY_R,
	I3DDMA_3D_OPMODE_SW,
	I3DDMA_3D_OPMODE_HW_NODMA
} I3DDMA_3D_OPMODE_T;

typedef struct {
	unsigned char *uncache;
	void *cache;
	unsigned long phyaddr;
	unsigned long phyaddr_unalign;
	unsigned long dmabuf_hndl;
	unsigned int size;
	unsigned int getsize;
} I3DDMA_CAPTURE_BUFFER_T;

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
	I3DDMA_COLOR_YUV420,
	I3DDMA_COLOR_UNKNOW
} I3DDMA_COLOR_SPACE_T;

typedef enum {
	I3DDMA_COLORIMETRY_NOSPECIFIED = 0,
	I3DDMA_COLORIMETRY_601,
	I3DDMA_COLORIMETRY_709,
	I3DDMA_COLORIMETRY_XYYCC601,
	I3DDMA_COLORIMETRY_XYYCC709,
	I3DDMA_COLORIMETRY_SYCC601,
	I3DDMA_COLORIMETRY_ADOBE_YCC601,
	I3DDMA_COLORIMETRY_ADOBE_RGB,
} I3DDMA_COLORIMETRY_T;

typedef enum {
	I3DDMA_ERR_NO = 0,
	I3DDMA_EER_GENERIC,
	I3DDMA_ERR_ONMS_ONESHOT,
	I3DDMA_ERR_ONMS_CONTINUOUS_INIT,
	I3DDMA_ERR_ONMS_CONTINUOUS_CHECK,
	I3DDMA_ERR_ONMS_WRONG_TIMING,
	I3DDMA_ERR_ONMS_WRONG_PARAM,
	I3DDMA_ERR_3D_WRONG_OPMODE,
	I3DDMA_ERR_3D_NO_MEM,
	I3DDMA_ERR_4XXTO4XX_WRONG_PARAM,
	I3DDMA_EER_MEASURE_ACTIVESPACE_FAIL,
	I3DDMA_EER_VODMA_NOT_READY,
} I3DDMA_ERR_T;

typedef struct {

    I3DDMA_COLOR_DEPTH_T depth;
    I3DDMA_COLOR_SPACE_T color;

    I3DDMA_COLORIMETRY_T  colorimetry;
    char progressive;          // 0 --> interlaced ; 1 --> progressive
    I3DDMA_3D_T i3ddma_3dformat;
	VO_FORCE2D_MODE i3ddma_force2d; // 0: disable, 1: L-frame, 2: R-frame

    unsigned int v_total;
    unsigned int h_total;
    unsigned int v_act_len;
    unsigned int h_act_len;
    unsigned int v_act_sta;
    unsigned int h_act_sta;
    //unsigned int h_sync_high;
    unsigned int v_freq_1000; // in digit 3
    unsigned int h_freq;
    unsigned int polarity;
    //unsigned int mode_id;

    // only for fram packing
    unsigned int v_active_space1;
    unsigned int v_active_space2;

    // quincunx
//    unsigned quincunx_en;
//    unsigned quincunx_mode;

    unsigned int src_v_freq_1000; // in digit 3

}I3DDMA_TIMING_T;


#define CAP_BUF_NUM_MAX  (8)

// share with VO DMA
typedef struct {

    int 	   i3ddma_3d_capability;
    int      fast_boot_source;
    int 	   force_all_3d_disable;
    int 	   force_2dto3d_enable;
    I3DDMA_3D_T force_2dto3d_mode;
    int 	   force_3dto2d_enable;
    int 	  force_3dto2d_lr_sel;
    int      enable_3ddma;
    int 	   resume;

	I3DDMA_3D_OPMODE_T   hw_i3ddma_dma;
	int      hw_dither12x10_enable;
	int      hw_dither10x8_enable;
	int      hw_4xxto4xx_enable;
	int      hw_rgbtoyuv_enable;

	I3DDMA_COLOR_DEPTH_T targetDepth;
	I3DDMA_COLOR_SPACE_T targetColor;

	int      hw_hsd_enable;
	unsigned int input_src_idx;
	unsigned int input_src_type;
	unsigned int input_src_from;
	unsigned int input_src_mode;
	unsigned int input_h_Count;
	unsigned int input_h_size;
	unsigned int output_h_size;

	I3DDMA_TIMING_T *ptx_timing;
	I3DDMA_TIMING_T *pgen_timing;


    // for 3d
    unsigned int frame_rate_x2;
    unsigned int enable_4xxto4xx;
    unsigned int pixel_bits;





     int down_sample_number;

	int enable; 					// 3d global enable
	// video ring buffer
	unsigned int cap_rp;			// W: Video R:System
	unsigned int cap_wp;			// W: System R:Video
	int cap_size;
	I3DDMA_CAPTURE_BUFFER_T cap_buffer_pool;
	I3DDMA_CAPTURE_BUFFER_T cap_buffer[I3DDMA_CAP_BUF_NUM];
	// freeze
	unsigned int vodma_freeze;	// W: System R:Video
	unsigned int hdmi_freeze;
	unsigned int force_2d;			// 0: 3d  1:L only 2:R only
	// pull down 22
	unsigned int pulldown22;
	I3DDMA_COLOR_SPACE_T  cap_color_fmt;
	int cap_pixel_bits;
	I3DDMA_COLOR_DEPTH_T cap_colordepth;
	I3DDMA_COLOR_SPACE_T cap_colorspace;
	I3DDMA_TIMING_T vodma_timing;
	I3DDMA_3D_OPMODE_T opmode;
	I3DDMA_CAPTURE_BUFFER_T cap_total_buffer;
} I3DDMA_3DDMA_CTRL_T;

//compression bits
typedef enum {
	COMPS_10_BITS = 10,
	COMPS_11_BITS = 11,
	COMPS_12_BITS = 12,
	COMPS_13_BITS = 13,
	COMPS_14_BITS = 14,
	COMPS_15_BITS = 15,
	COMPS_16_BITS = 16,
	COMPS_17_BITS = 17,
	COMPS_18_BITS = 18,
	COMPS_19_BITS = 19,
	COMPS_20_BITS = 20,
	COMPS_21_BITS = 21,
	COMPS_22_BITS = 22,
	COMPS_23_BITS = 23,
	COMPS_24_BITS = 24,
	COMPS_25_BITS = 25,
	COMPS_26_BITS = 26,
	COMPS_27_BITS = 27,
	COMPS_28_BITS = 28,
	COMPS_29_BITS = 29,
	COMPS_NONE_BITS = 30,
}I3ddmaCompression_Bits;

//compression mode
typedef enum {
	I3DDMA_COMP_FRAME_MODE = 0,
	I3DDMA_COMP_LINE_MODE = 1,
	I3DDMA_COMP_NONE_MODE = 2,
}I3ddmaCompression_Mode;

extern VIDEO_RPC_VOUT_CONFIG_HDMI_3D *(*GetI3DDMAConfigData)(unsigned long);
char I3DDMA_SetupVODMA(I3DDMA_TIMING_T *timing,unsigned char display);
char I3DDMA_Setup3DDMA(I3DDMA_3DDMA_CTRL_T* ctrl, I3DDMA_3D_OPMODE_T opmode,unsigned char display);
char I3DDMA_ForceVODMA2D(UINT8 enable, UINT8 LR);
unsigned char I3DDMA_Get3DAllocReleaseMemoryDynamic(void);
char I3DDMA_ResetVODMA4K2K(void);
char drvif_I3DDMA_3D_FeatureEnable(char enable);
unsigned char I3DDMA_Get_FP1080i_status(void);

unsigned char I3DDMA_check_idma_shareMemWithVideoFW(void);

extern unsigned char I3DDMA_3DDMAInit(void);
extern void I3DDMA_3DDMADeInit(void);
unsigned char I3DDMA_DolbyVision_HDMI_Init(void);
unsigned long I3DDMA_DolbyVision_get_md_buf(void);
unsigned char* I3DDMA_DolbyVision_get_md_uncach_buf(void);
unsigned int I3DDMA_DolbyVision_get_md_buf_size(void);
void I3DDMA_DolbyVision_HDMI_DeInit(void);
unsigned char I3DDMA_DolbyVision_MetaData_Init(void);
unsigned char I3DDMA_DolbyVision_MetaData_DeInit(void);
void I3DDMA_DolbyVision_buf_setting(void);
//unsigned long I3DDMA_GetMemChunkAddr(void);
unsigned long I3DDMA_GetMemChunkAddr(unsigned int size);

extern void I3DDMA_Get3DGenTiming(I3DDMA_TIMING_T *tx_timing, I3DDMA_TIMING_T *gen_timing);
extern void I3DDMA_BandWidth_Control(I3DDMA_TIMING_T* tx_timing, I3DDMA_TIMING_T*gen_timing);
extern char I3DDMA_Dithering12to10(I3DDMA_COLOR_DEPTH_T source, I3DDMA_COLOR_DEPTH_T target);
extern char I3DDMA_4xxto4xx_Downsample(I3DDMA_COLOR_SPACE_T source, I3DDMA_COLOR_SPACE_T target);
extern void I3DDMA_ultrazoom_config_h_scaling_down(unsigned int in_h_Size, unsigned int out_h_Size, unsigned char panorama);
extern  void I3DDMA_ColorSpacergb2yuvtransfer(unsigned char nSrcType, unsigned char nSD_HD, unsigned char VO_Jpeg_Back_YUV);
extern  unsigned char I3DDMA_disp2tve_CapBufInit(void);
extern void I3DDMA_disp2tve_CapBufDeInit(void) ;
extern I3DDMA_3DDMA_CTRL_T i3ddmaCtrl;

extern I3DDMA_CAPTURE_BUFFER_T idma_cap_buf[4];
unsigned char drv_I3ddmaVodma_GameMode_iv2pv_delay(unsigned char enable);
//void drv_AV_GameMode_reset_iv2pv_delay(unsigned int iv2dv_delay);
void drv_I3ddmaVodma_SingleBuffer_GameMode(unsigned char enable);
void backup_hdmi_src_info(I3DDMA_TIMING_T *src_info);
I3DDMA_TIMING_T *get_backup_hdmi_info(void);
void set_vtop_input_color_format(I3DDMA_COLOR_SPACE_T color_format);
I3DDMA_COLOR_SPACE_T get_target_i3ddma_color_format(void);
I3DDMA_COLOR_SPACE_T decide_i3ddma_output_foramt(I3DDMA_TIMING_T *timing);//decide i3ddma output color format
void I3ddma_format_changed_seamless(I3DDMA_TIMING_T *timing, I3DDMA_COLOR_SPACE_T target);//seamless change i3ddma capture setting
#ifndef BUILD_QUICK_SHOW
spinlock_t* get_hdr_seamless_change_spinlock(void);
#endif
void set_seamless_trigger_flag(unsigned char TorF);
unsigned char get_seamless_trigger_flag(void);
void trigger_seamless_flow(VSC_INPUT_TYPE_T src, unsigned char hdr_enable, unsigned char hdr_type);
//unsigned char dvrif_i3ddma_compression_get_enable(void);
void dvrif_i3ddma_compression_set_enable(unsigned char enable);
//unsigned char dvrif_i3ddma_get_compression_bits(void);
void dvrif_i3ddma_set_compression_bits(unsigned char comp_bits);
void I3DDMA_Set_UHD_PCmode_Mem_Alloc_Fail_flag(unsigned char value);
unsigned char I3DDMA_Get_UHD_PCmode_Mem_Alloc_Fail_flag(void);
unsigned char dvrif_i3ddma_compression_get_enable(void);
unsigned char dvrif_i3ddma_get_compression_bits(void);
unsigned char dvrif_i3ddma_get_compression_mode(void);
void dvrif_i3ddma_set_compression_mode(unsigned char comp_mode);
void dvrif_i3ddma_comp_setting(UINT8 enable_compression, UINT16 comp_wid, UINT16 comp_len, UINT8 comp_ratio);
void drvif_I3DDMA_freeze(unsigned char bFreeze);
void  drvif_h3ddma_set_sequence_capture0(I3DDMA_TIMING_T *timing);
void  drvif_h3ddma_set_MDP_capture(int , int );	
void h3ddma_nn_run_capture_config(I3DDMA_TIMING_T *timing);

void h3ddma_free_nv12_mermory(void);
void h3ddma_nn_disable_sub_path(void);
void I3DDMA_set_dither_bit(unsigned char dither_bit);
unsigned char I3DDMA_get_dither_bit(void);
void h3ddma_set_cap_enable_mask(unsigned char mask);
void h3ddma_clear_cap_enable_mask(unsigned char mask);
void h3mdda_set_dcmt(unsigned int mode);
unsigned char h3ddma_get_cap_enable_mask(void);
void h3ddma_open_i3ddma_capture(unsigned char mask);
void h3ddma_close_i3ddma_capture(unsigned char mask);
void h3ddma_NN_set_cap_ratio(unsigned char cap_ratio);
unsigned char h3ddma_NN_get_cap_ratio(void);
void h3ddma_nn_config_multi_crop(I3DDMA_NN_CROP_ATTR_T i3ddma_nn_atrr);
void h3ddma_nn_set_recfg_flag(unsigned char bVal);
unsigned char h3ddma_nn_get_recfg_flag(void);
void h3ddma_set_capture_enable(unsigned char chanel,unsigned char enable);//decide i3ddma cpture enable or disable
int h3ddma_get_NN_read_buffer(unsigned int *a_pulYAddr, unsigned int *a_pulCAddr, unsigned long long *uzd_timestamp, unsigned int *a_pulCropYAddr, unsigned int *a_pulCropCAddr, unsigned long long *crop_timestamp);
void I3DDMA_reset_i3tovo_vs_sel(void);
unsigned int calc_i3ddma_comp_line_sum_bit(unsigned int comp_wid, unsigned int frame_limit_bit, unsigned char cmp_alpha_en);

void Set_Val_i3ddmaCtrl_addr_init_value(void);
void Set_Val_i3ddmaCtrl_addr_cap_buffer_size(unsigned char ucIndex, unsigned int size);
void Set_Val_i3ddmaCtrl_addr_cap_buffer_addr(unsigned char ucIndex, unsigned int addr);
void Set_Val_I3ddma3DMode(I3DDMA_3D_OPMODE_T emode);
void Set_Val_I3ddma3Dformat(I3DDMA_3D_T i3ddma3DType);
I3DDMA_3D_T Get_Val_I3ddma3Dformat(void);
I3DDMA_3DDMA_CTRL_T *Get_Val_i3ddmaCtrl_addr(void);
void disable_i3ddma_cap(void);
void I3DDMA_Send_Vo_SwBufNum_share_memory(unsigned char ucNum);
void I3DDMA_SetCap0LastWriteFlag(unsigned int flag);
void I3DDMA_SwSetBufNum(unsigned char ucNum);
unsigned char I3DDMA_SwGetBufNum(void);
void video_delay_free_memory(void);
unsigned char video_delay_alloc_memory(unsigned int video_size);
extern I3DDMA_CAPTURE_BUFFER_T s_i3ddma_sw_cap_buffer[SW_I3DDMA_CAP_NUM];
void set_i3ddma_video_delay_total_bufnum(unsigned int num);
unsigned int get_i3ddma_video_delay_total_bufnum(void);
void I3DDMA_SwSetBufBoundary(void);
void set_i3ddma_vdelay_cap_buffer(void);

#ifdef __cplusplus
}
#endif


#endif
