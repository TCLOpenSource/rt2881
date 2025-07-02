#include <linux/delay.h>
#include <generated/autoconf.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/uaccess.h>
#include <asm/cacheflush.h>
#include <linux/pageremap.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <uapi/linux/const.h>
#include <linux/videodev2.h>
#include <linux/mm.h>
#include <rtd_log/rtd_module_log.h>
#include <linux/fs.h>
#include <linux/hrtimer.h>
#include <linux/version.h>
#include <rtk_kdriver/RPCDriver.h>
#include <rtk_dc_mt.h>

#include <rbus/dc2h_cap_reg.h>
#include <rbus/dc2h_scaledown_reg.h>
#include <rbus/dc2h_r2y_dither_4xxto4xx_reg.h>
#include <rbus/dc2h2_cap_reg.h>
#include <rbus/dc2h2_scaledown_reg.h>
#include <rbus/dc2h2_r2y_dither_4xxto4xx_reg.h>
#include <rbus/vgip_reg.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/mdomain_disp_reg.h>
#include <rbus/scaleup_reg.h>
#include <rbus/scaledown_reg.h>
#include <rbus/scpu_core_reg.h>

#include <rbus/rgb2yuv_reg.h>
#include <rbus/yuv2rgb_reg.h>
#include <rbus/timer_reg.h>
#include <rbus/dma_vgip_reg.h>
#include <rbus/sub_vgip_reg.h>
#include <rbus/hdr_all_top_reg.h>
#include <rbus/hdr_sub_reg.h>

#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <tvscalercontrol/vip/scalerColor.h>

#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
#else
#include <rtk_kdriver/scalercommon/scalerCommon.h>
#endif
#include <tvscalercontrol/scaler/scalerstruct.h>
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <tvscalercontrol/scalerdrv/pipmp.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <tvscalercontrol/scalerdrv/scaler_dtg.h>
#include <tvscalercontrol/scalerdrv/zoom_smoothtoggle_lib.h>
#include <scaler/vipRPCCommon.h>
#include <tvscalercontrol/vip/ultrazoom.h>
#include <vo/rtk_vo.h>

#include "scaler_vtdev.h"
#include "scaler_vscdev.h"
#include "scaler_vpqmemcdev.h"

#undef rtd_outl
#define rtd_outl(x, y)              IoReg_Write32(x,y)

struct semaphore VT_Semaphore;
struct semaphore VT_Semaphore_enq;
static struct semaphore VTBlock_Semaphore;

#define _ALIGN(val, align) (((val) + ((align) - 1)) & ~((align) - 1))
#define __4KPAGE  0x1000
/*4k and 96 align*/
#define __12KPAGE  0x3000

unsigned int VT_CAPTURE_BUFFER_UNITSIZE;

VT_VIDEO_FRAME_BUFFER_PROPERTY_INFO_T CapBuffer_VT[DC2H_NUMBER]; //DC2H1 and DC2H2

#ifndef UT_flag
static dev_t vivt_devno = 0;//vt device number
static struct cdev vivt_cdev;
#endif // #ifndef UT_flag

#define XTAL_CLK (27000000)
/*vt init work width and length*/
#define VT_CAP_FRAME_WIDTH_2K1k		1920
#define VT_CAP_FRAME_HEIGHT_2K1k	1080

#define VT_CAP_FRAME_WIDTH_4K2K		3840
#define VT_CAP_FRAME_HEIGHT_4K2K	2160

#define VT_FPS_OUTPUT 	(60)
#define TMPMUL  		(16)
#define MAX_CAP_BUF_NUM 5

#define ABNORMAL_DVS_FIRST_HAPPEN 1
#define WAIT_DVS_STABLE_COUNT 8

#define DC2H2_PQC_FRAME_LIMIT_BIT 10  //8~16
KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_INFO_T VFODState = {FALSE,FALSE,TRUE,1};

static KADP_VT_DUMP_LOCATION_TYPE_T VTDumpLocation = KADP_VT_MAX_LOCATION;
static VT_CUR_CAPTURE_INFO curCapInfo[DC2H_NUMBER];  //DC2H1 and DC2H2
extern unsigned char get_vsc_mutestatus(void);
static unsigned char VtFunction = FALSE;
static unsigned char VtEnableFRCMode = TRUE;
static unsigned int VtBufferNum[DC2H_NUMBER] = {0, 0};
static unsigned char VtSwBufferMode[DC2H_NUMBER] = {FALSE, FALSE};
static unsigned int dc2h_dma_cap_mode[DC2H_NUMBER] = {0, 0}; //default line mode
static unsigned int vt_cap_frame_max_width = VT_CAP_FRAME_WIDTH_2K1k;
static unsigned int vt_cap_frame_max_height = VT_CAP_FRAME_HEIGHT_2K1k;

static unsigned long VtAllocatedBufferStartAdress[DC2H_NUMBER][VT_MAX_SUPPORT_BUF_NUMS] = {0};
static VT_APP_CASE vt_app_case = VT_APP_CASE_NONE;
static unsigned char pre_rtk_qos_mode = 0;

static unsigned int abnormal_dvs_cnt = 0;
static unsigned int wait_dvs_stable_cnt = 0;
static unsigned char vt_StreamOn_flag = FALSE;

static unsigned char VdecSecureStatus = FALSE;
static unsigned char DtvSecureStatus = FALSE;
static unsigned char dc2h2_pqc_path_enable = FALSE;
static unsigned int uiTargetFps[DC2H_NUMBER] = {0, 0};
static unsigned int framerate_divide[DC2H_NUMBER]= {1, 1};
static unsigned char dc2h_crop_enable[DC2H_NUMBER] = {FALSE, FALSE};
unsigned char IndexOfFreezedVideoFrameBuffer[DC2H_NUMBER] = {0, 0};
volatile unsigned int vfod_capture_out_W = VT_CAP_FRAME_WIDTH_2K1k;
volatile unsigned int vfod_capture_out_H = VT_CAP_FRAME_HEIGHT_2K1k;
volatile unsigned int vfod_capture_location = KADP_VT_DISPLAY_OUTPUT;

unsigned int vtPrintMask = 0;/*mask for vt print log*/
unsigned int vt_dequeue_enqueue_count[DC2H_NUMBER] = {0, 0};
unsigned char vt_dequeue_overflow_flag[DC2H_NUMBER] = {FALSE, FALSE};
static VT_BUF_STATUS_T *pDc2h_BufStatus = NULL;
static DEFINE_SPINLOCK(vt_buf_status_Spinlock);

static unsigned int dc2h2_pqc_cmp_ratio = DC2H2_PQC_FRAME_LIMIT_BIT;
/*------include extern functions----------*/
extern phys_addr_t dmaheap_to_phys(int fd);
extern unsigned char force_enable_two_step_uzu(void);/* get d domain go two pixel mode? */
extern void MEMC_Set_malloc_address(UINT8 status);
extern void drvif_vt_block_vr_recording(unsigned char vrEnableFlag);
extern unsigned char get_vr_function(void);
extern void drvif_vt_reconfig_vr_recording(void);

unsigned char HAL_VT_Finalize(void);
void drvif_dc2h_capture_config(VT_CUR_CAPTURE_INFO *pdc2h_ctrl_info);
void set_cap_buffer_size_by_AP(unsigned int usr_width, unsigned int usr_height);
void reset_dc2h_hw_setting(void);
unsigned char HAL_VT_GetVideoFrameBufferIndex(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, unsigned int *pIndexOfCurrentVideoFrameBuffer);
void drvif_memc_outBg_ctrl(bool enable);
unsigned char drvif_vt_get_dequeue_index(unsigned int *pIndex, unsigned int dc2h_num);
unsigned long vt_bufAddr_align(unsigned int val);
void vt_enable_dc2h(unsigned char state);
//unsigned char get_dc2h_dmaen_state(unsigned int dc2h_num);
extern unsigned char get_MEMC_bypass_status_refer_platform_model(void);

#define ENABLE_VT_TEST_CASE_CONFIG
#ifdef ENABLE_VT_TEST_CASE_CONFIG
typedef enum _VT_TEST_CASE{
	VT_TEST_CASE_DISABLE = 0,
	VT_TEST_SINGLE_CAPTURE_CASE,
	VT_TEST_ACR_CASE,
	VT_TEST_VR360_CASE,
}VT_TEST_CASE_T;

VT_TEST_CASE_T vt_test_state = VT_TEST_CASE_DISABLE;
void set_vt_test_case(VT_TEST_CASE_T enable)
{
	vt_test_state = enable;
}
VT_TEST_CASE_T get_vt_test_case(void)
{
	return vt_test_state;
}

void handle_vt_test_case(unsigned int dc2h_num);
#endif

unsigned char get_vt_function(void)
{
	return VtFunction;
}

void set_vt_function(unsigned char value)
{
	VtFunction = value;
}

unsigned char get_vt_EnableFRCMode(void)
{
	return VtEnableFRCMode;
}
EXPORT_SYMBOL(get_vt_EnableFRCMode);

void set_vt_EnableFRCMode(unsigned char value)
{
	VtEnableFRCMode = value;
}

void set_vt_StreamOn_flag(unsigned char flg)
{
	vt_StreamOn_flag = flg;
}
unsigned char get_vt_StreamOn_flag(void)
{
	return vt_StreamOn_flag;
}

unsigned int get_vt_VtBufferNum(unsigned int dc2h_num)
{
	return VtBufferNum[dc2h_num];
}

void set_vt_VtBufferNum(unsigned int value, unsigned int dc2h_num)
{
	VtBufferNum[dc2h_num] = value;
}

unsigned char get_vt_VtSwBufferMode(unsigned int dc2h_num)
{
	return VtSwBufferMode[dc2h_num];
}

void set_vt_VtSwBufferMode(unsigned char value, unsigned int dc2h_num)
{
	VtSwBufferMode[dc2h_num] = value;
}

void set_VT_Pixel_Format(VT_CAP_FMT value, unsigned int dc2h_num)
{
	curCapInfo[dc2h_num].fmt = value;
}

unsigned int get_VT_Pixel_Format(unsigned int dc2h_num)
{
	return curCapInfo[dc2h_num].fmt;
}

void vtforut_enable_dc2h(unsigned char state)
{
    curCapInfo[DC2H1].enable = state;
}

unsigned int get_framerate_divide(unsigned int dc2h_num)
{
	return framerate_divide[dc2h_num];
}

void set_framerate_divide(unsigned int value, unsigned int dc2h_num)
{
	framerate_divide[dc2h_num] = value;
}

unsigned int get_cap_buffer_Width(void)
{
	return vt_cap_frame_max_width;
}
unsigned int get_cap_buffer_Height(void)
{
	return vt_cap_frame_max_height;
}

unsigned int get_vt_capBuf_Addr(unsigned int idx)
{
    return CapBuffer_VT[DC2H1].vt_pa_buf_info[idx].pa_y;
}

void set_vt_target_fps(unsigned int val, unsigned int dc2h_num)
{
    uiTargetFps[dc2h_num] = val;
}
unsigned int get_vt_target_fps(unsigned int dc2h_num)
{
    return uiTargetFps[dc2h_num];
}

void set_vt_app_case(VT_APP_CASE type)
{
    vt_app_case = type;
}

VT_APP_CASE get_vt_app_case(void)
{
    return vt_app_case;
}

unsigned char get_vt_dc2h_crop_enable(unsigned int dc2h_num)
{
	return dc2h_crop_enable[dc2h_num];
}

void set_vt_dc2h_crop_enable(unsigned char en, unsigned int dc2h_num)
{
	dc2h_crop_enable[dc2h_num] = en;
}

void vt_back_qos_current_mode(unsigned char val)
{
    pre_rtk_qos_mode = val;
}

void vt_dequeue_enqueue_count_add(unsigned int dc2h_num)
{
    vt_dequeue_enqueue_count[dc2h_num]++;
}

void vt_dequeue_enqueue_count_subtract(unsigned int dc2h_num)
{
	if(vt_dequeue_enqueue_count[dc2h_num] > 0){
		vt_dequeue_enqueue_count[dc2h_num]--;
	}
}

void set_vt_dequeue_enqueue_count(unsigned int cnt, unsigned int dc2h_num)
{
    vt_dequeue_enqueue_count[dc2h_num] = cnt;
}

unsigned int get_vt_dequeue_enqueue_count(unsigned int dc2h_num)
{
    return vt_dequeue_enqueue_count[dc2h_num];
}

unsigned char get_vt_dequeue_overflow_flag(unsigned int dc2h_num)
{
    return vt_dequeue_overflow_flag[dc2h_num];
}

void set_vt_dequeue_overflow_flag(unsigned char flag, unsigned int dc2h_num)
{
    vt_dequeue_overflow_flag[dc2h_num] = flag;
}

unsigned int get_dc2h_dma_cap_mode(unsigned int dc2h_num)
{
	return dc2h_dma_cap_mode[dc2h_num];
}

void set_dc2h_dma_cap_mode(unsigned int mode, unsigned int dc2h_num)
{
	dc2h_dma_cap_mode[dc2h_num] = mode;
}

unsigned int get_index_of_freezed(unsigned int dc2h_num)
{
	if(dc2h_num < DC2HX_MAXN){
		return IndexOfFreezedVideoFrameBuffer[dc2h_num];
	}else{
		rtd_pr_vt_err("%s,dc2h_num %d\n", __FUNCTION__, dc2h_num);
		return 0;
	}
}

unsigned int get_idle_dc2h(unsigned int nr)
{
	unsigned int i = 0;
	for(i = DC2H1; i < DC2HX_MAXN; i++){
		if(curCapInfo[i].idle_state == DC2H_IDLE){
			curCapInfo[i].idle_state = DC2H_OCCUPY;
			rtd_pr_vt_notice("%s,dc2h%d set to occupy,nr=%d\n", __FUNCTION__, (i+1), nr);
			return i;
		}
	}
	return DC2HX_MAXN;
}

void set_idle_dc2h(unsigned int dc2h_num, unsigned int nr)
{
	if(dc2h_num < DC2HX_MAXN){
		curCapInfo[dc2h_num].idle_state = DC2H_IDLE;
		rtd_pr_vt_notice("%s,dc2h%d set to idle,nr=%d\n", __FUNCTION__, (dc2h_num+1), nr);
	}
}

typedef struct
{
    SIZE in_size;
    SIZE out_size;
    //unsigned char current_idx;
    unsigned char dc2h_init;
    unsigned char dc2h_en;
    unsigned char dc2h_in_sel;
    unsigned char panorama;
    unsigned char set_bit;
    unsigned char done_bit;
} DC2H_HANDLER ;

typedef enum _DC2H_IN_SEL
{
    _NO_INPUT= 0,
    _MAIN_UZU,
    _MAIN_YUV2RGB,
    _TWO_SECOND_UZU,  //2nd_uzu = 3
    _OSD123_MIXER_Output, /* osd123 output*/
    _Dither_Output, 
    _Sub_UZU_Output,
    _Memc_mux_Output,
    _Memc_Mux_Input, //8
    _Idomain_Sdnr_input,
	_OSD4_Output,
    _VD_OUTPUT
}DC2H_IN_SEL;


DC2H_HANDLER *pdc2h_hdl = NULL;
// static DC2H_HANDLER dc2h_hdl;

// #define CAP_LEN 32
// #define CAP_REM 12

#define _RGB2YUV_m11				0x00
#define _RGB2YUV_m12				0x01
#define _RGB2YUV_m13				0x02
#define _RGB2YUV_m21				0x03
#define _RGB2YUV_m22				0x04
#define _RGB2YUV_m23				0x05
#define _RGB2YUV_m31				0x06
#define _RGB2YUV_m32				0x07
#define _RGB2YUV_m33				0x08
#define _RGB2YUV_Yo_even			0x09
#define _RGB2YUV_Yo_odd				0x0a
#define _RGB2YUV_Y_gain				0x0b
#define _RGB2YUV_sel_RGB			0x0c
#define _RGB2YUV_set_R_in_offset	0x0d
#define _RGB2YUV_set_UV_out_offset	0x0e
#define _RGB2YUV_sel_UV_off			0x0f
#define _RGB2YUV_Matrix_bypass		0x10
#define _RGB2YUV_Enable_Y_gain		0x11

/* YUV to RGB */
static unsigned short tYUV2RGB_COEF_709_RGB_0_255[] =
{
	0xc99,   // m11
	0x800,  // m12
	0x0000,  // m13

	0x3c41,   // m21 >> 2 0x01D5 up bits at AT Name: cychen2 , Date: 2010/3/25
	0x800,   // m22 >> 1
	0x3e80,   // m23 >> 2
	0x0000,   // m31 >> 2
	0x800,   // m32 >> 2 0x0195  up bits at AT Name: cychen2 , Date: 2010/3/25
	0xed8,   // m33 >> 1

	0x0000,          // Yo_even
	0x0000,          // Yo_odd
	0x12a,          // Y_gain

	0x0000,  // sel_RGB
	0x0000,  // sel_R_in_offset
	0x0001,  // sel_UV_out_offset
	0x0000,  // sel_UV_off
	0x0000,  // Matrix_bypass
	0x0000,  // sel_y_gain
};

/*RGB to YUV table for nv12 nv16 capture format*/
static unsigned short tYUV2RGB_COEF_709_RGB_16_235[] =
{
	0x02EE,
	0x09D3,
	0x00FE,
	0x7E62,
	0x7A98,
	0x0706,
	0x0706,
	0x799E,
	0x7F5C,
	0x040,
	0x040,
	0x0100,
	0x0001,
	0x0000,
	0x0001,
	0x0000,
	0x0000,
	0x0000
}; //r2y 10bit(m7 and m9)
//{0x02EE, 0x09D3, 0x00FE, 0x7E62, 0x7A98, 0x0706, 0x0706, 0x799E, 0x7F5C, 0x0100, 0x0100, 0x0100, 0x0001, 0x0000, 0x0001, 0x0000, 0x0000, 0x0000};// r2y 12 bit(merlin8) /*RGB2YUV_COEF_RGB_0_255_TO_YCC709_16_235*/


static short tScale_Down_Coef_2tap[] = {
0,   0,   0,   0,   0,   0,   0,   0,
0,   0,   0,   0,   0,   0,   0,   0,
31,  95, 159, 223, 287, 351, 415, 479,
543, 607, 671, 735, 799, 863, 927, 991,
};

static short tScale_Down_Coef_Blur[] = {
//2,   3,   5,   7,  10,  15,  20,  28,  38,  49,  64,  81, 101, 124, 150, 178,
//209, 242, 277, 314, 351, 389, 426, 462, 496, 529, 556, 582, 602, 618, 629, 635,
213,  217,  221,  222,  224,  229,  232,  234,
239,  242,  245,  249,  252,  255,  259,  262,
265,  266,  267,  269,  271,  272,  273,  275,
276,  277,  278,  280,  281,  281,  282,  284,
};

static short tScale_Down_Coef_Mid[] = {
//-2,  0,   1,   2,   5,   9,  15,  22,  32,  45,  60,  77,  98, 122, 149, 179,
//211, 245, 281, 318, 356, 394, 431, 468, 502, 533, 561, 586, 606, 620, 630, 636,
131,  125,  124,  126,  130,  135,  142,  151,
161,  172,  183,  195,  208,  221,  234,  245,
260,  275,  290,  304,  318,  330,  341,  351,
361,  369,  376,  381,  386,  389,  390,  388,
};


static short tScale_Down_Coef_Sharp[] = {
/*-2,   0,   1,   3,   6,  10,  15,  22,  32,  43,  58,  75,  95, 119, 145, 174,
206, 240, 276, 314, 353, 391, 430, 467, 503, 536, 565, 590, 612, 628, 639, 646,*/
	-6,  -5,	-5,  -5,  -3,  -5,	-7,  -9,
	-16, -19, -22, -23, -21, -15,	-4,  13,
	40,  76, 122, 178, 242, 316, 396, 481,
	568, 654, 735, 808, 872, 922, 957, 977,
};

static  short *tScaleDown_COEF_TAB[] =
{
	tScale_Down_Coef_Blur,     tScale_Down_Coef_Mid,     tScale_Down_Coef_Sharp,     tScale_Down_Coef_2tap
};

#ifndef UT_flag

void set_vt_dc2h_cap_mem_type(unsigned int dc2h_num, unsigned int type)
{
	CapBuffer_VT[dc2h_num].mem_type = type;
}

DC2H_MEMORY_TYPE get_vt_dc2h_cap_mem_type(unsigned int dc2h_num)
{
	return CapBuffer_VT[dc2h_num].mem_type;
}

unsigned int get_vt_dc2h_plane_buf_addr(unsigned int dc2h_num, int plane_num, unsigned int idx)
{
	if(idx >= get_vt_VtBufferNum(dc2h_num)){
		rtd_pr_vt_emerg("[error] idx %d bigger than buffer number\n", idx);
		return 0;
	}

	if(plane_num == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){
		return CapBuffer_VT[dc2h_num].vt_pa_buf_info[idx].pa_y;
	}else if(plane_num == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){
		return CapBuffer_VT[dc2h_num].vt_pa_buf_info[idx].pa_u;
	}else{
		return CapBuffer_VT[dc2h_num].vt_pa_buf_info[idx].pa_v;
	}
}

unsigned int get_vt_dc2h_plane_buf_size(unsigned int dc2h_num, int plane_num)
{
	if(plane_num == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){
		return CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y;
	}else if(plane_num == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){
		return CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_u;
	}else{
		return CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_v;
	}
}

unsigned int get_dc2h_in_sel_by_location(unsigned int loaction)
{
	unsigned int res = _OSD123_MIXER_Output;
	if(loaction == KADP_VT_SCALER_INPUT){
		res = _Idomain_Sdnr_input;
	}else if(loaction == KADP_VT_SCALER_OUTPUT){
		res = _MAIN_UZU;
	}else if(loaction == KADP_VT_DISPLAY_OUTPUT){
		res = _Memc_Mux_Input;
	}else if(loaction == KADP_VT_OSDVIDEO_OUTPUT){
		res = _OSD123_MIXER_Output;
	}else if(loaction == KADP_VT_OSD_OUTPUT){
		res = _OSD4_Output;
	}

	return res;
}

void vt_enable_dc2h(unsigned char state)
{
	//down(&VT_Semaphore);
	curCapInfo[DC2H1].enable = state;
	drvif_dc2h_capture_config(&curCapInfo[DC2H1]);
	//up(&VT_Semaphore);
}

void get_shm_vt_buf_status(void)
{
    pDc2h_BufStatus = (VT_BUF_STATUS_T *)Scaler_GetShareMemVirAddr(SCLAERIOC_SET_VT_DC2H_BUF_STATUS);
}

VT_BUFFER_STATUS get_vt_buf_status(unsigned int num, unsigned int idx)
{
    VT_BUFFER_STATUS vtbuf_status = VT_BUFFER_UNKNOW_STATUS;
    if(pDc2h_BufStatus == NULL){
        rtd_pr_vt_notice("[error0] shm vt_buf_status pointer null\n");
        return VT_BUFFER_UNKNOW_STATUS;
    }
    vtbuf_status = htonl(pDc2h_BufStatus->bufStatus[num].bufSta[idx]);

    return vtbuf_status;
}

void set_vt_buf_dequeue(unsigned int num, unsigned int idx)
{
    unsigned int vt_buf_sta = VT_BUFFER_AP_DEQUEUED;
    /* set share memory which buffer dequeued */

    if(pDc2h_BufStatus == NULL){
        rtd_pr_vt_notice("[error1] shm vt_buf_status pointer null\n");
        return;
    }
    pDc2h_BufStatus->bufStatus[num].bufSta[idx] = Scaler_ChangeUINT32Endian(vt_buf_sta);
    //rtd_pr_vt_notice("[VT][dequeue] %d %d\n",idx, htonl(vt_buf_status->vtBufStatus[idx]));
}

void set_vt_buf_enqueue(unsigned int num, unsigned int idx)
{
    unsigned int vt_buf_sta = VT_BUFFER_AP_ENQUEUED;

    if(pDc2h_BufStatus == NULL){
        rtd_pr_vt_notice("[error2] shm vt_buf_status pointer null\n");
        return;
    }

    if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX)
        rtd_pr_vt_notice("enq 90K %d,idx %d,pre_sta %d\n", IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), idx, get_vt_buf_status(num, idx));

    pDc2h_BufStatus->bufStatus[num].bufSta[idx] = Scaler_ChangeUINT32Endian(vt_buf_sta);
}

void reset_vt_buf_status(void)
{
    int i = 0, j = 0;
    unsigned int vt_buf_sta = VT_BUFFER_AP_ENQUEUED;

    if(pDc2h_BufStatus == NULL){
        rtd_pr_vt_notice("[error3] shm vt_buf_status pointer null\n");
        return;
    }else{
		for(i = 0; i < DC2H_NUMBER; i++){
			for(j = 0; j < VT_CAP_BUF_NUMS; j++){
				pDc2h_BufStatus->bufStatus[i].bufSta[j] = Scaler_ChangeUINT32Endian(vt_buf_sta);
			}
		}    
    }
    pDc2h_BufStatus = NULL;
}

void init_vt_write_read_index(void)
{	int i = 0;
    unsigned int init_wr_idx = 0;
	unsigned int init_rd_idx = 0xFF;
    if(pDc2h_BufStatus == NULL){
        rtd_pr_vt_notice("[error] shm vt_buf_status pointer null\n");
		return;
	}else{
		for(i = 0; i < DC2H_NUMBER; i++){
			pDc2h_BufStatus->write[i] = Scaler_ChangeUINT32Endian(init_wr_idx);
			pDc2h_BufStatus->read[i] = Scaler_ChangeUINT32Endian(init_rd_idx);
		}
    }
}

void char_to_vtPrintMask(unsigned int *res, char value, unsigned int shift)
{
    unsigned int MASK = 0xffffffff ;
    MASK <<= 28 - shift ;
    MASK >>= 28 ;
    MASK <<= shift ;
    *res &= ~MASK ;
    if     (value == '0') *res |= (0x0 << shift) & MASK ;
    else if(value == '1') *res |= (0x1 << shift) & MASK ;
    else if(value == '2') *res |= (0x2 << shift) & MASK ;
    else if(value == '3') *res |= (0x3 << shift) & MASK ;
    else if(value == '4') *res |= (0x4 << shift) & MASK ;
    else if(value == '5') *res |= (0x5 << shift) & MASK ;
    else if(value == '6') *res |= (0x6 << shift) & MASK ;
    else if(value == '7') *res |= (0x7 << shift) & MASK ;
    else if(value == '8') *res |= (0x8 << shift) & MASK ;
    else if(value == '9') *res |= (0x9 << shift) & MASK ;
    else if(value == 'a') *res |= (0xa << shift) & MASK ;
    else if(value == 'b') *res |= (0xb << shift) & MASK ;
    else if(value == 'c') *res |= (0xc << shift) & MASK ;
    else if(value == 'd') *res |= (0xd << shift) & MASK ;
    else if(value == 'e') *res |= (0xe << shift) & MASK ;
    else if(value == 'f') *res |= (0xf << shift) & MASK ;
    else                  *res |= (0x0 << shift) & MASK ;
}

#endif // #ifndef UT_flag

void save_vt_dc2h_capture_location(unsigned char dc2h_num, unsigned int location)
{
	if(dc2h_num < DC2HX_MAXN){
		curCapInfo[dc2h_num].cap_location = get_dc2h_in_sel_by_location(location);
	}
}

void save_vt_dc2h_output_size(unsigned char dc2h_num, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	if(dc2h_num < DC2HX_MAXN){
		curCapInfo[dc2h_num].output_size.x = x;
		curCapInfo[dc2h_num].output_size.y = y;
		curCapInfo[dc2h_num].output_size.w = w;
		curCapInfo[dc2h_num].output_size.h = h;
	}
}

unsigned char get_dc2h_dmaen_state(unsigned int dc2h_num)
{
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	if(dc2h_num >= DC2HX_MAXN){
		return FALSE;
	}
	
	if(dc2h_num == DC2H1){
		dc2h_cap_color_format_reg.regValue = IoReg_Read32(DC2H_CAP_DC2H_CAP_Color_format_reg);
		if(dc2h_cap_color_format_reg.dc2h_cap0_en || dc2h_cap_color_format_reg.dc2h_cap1_en || dc2h_cap_color_format_reg.dc2h_cap2_en){
			return TRUE;
		}else{
			return FALSE;
		}
	}else{
		dc2h2_cap_color_format_reg.regValue = IoReg_Read32(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
		if(dc2h2_cap_color_format_reg.dc2h2_cap0_en || dc2h2_cap_color_format_reg.dc2h2_cap1_en || dc2h2_cap_color_format_reg.dc2h2_cap2_en){
			return TRUE;
		}else{
			return FALSE;
		}
	}
}

unsigned int sort_boundary_addr_max_index(unsigned int dc2h_num)
{
    unsigned int i,index = 0;
    unsigned int bufnum = get_vt_VtBufferNum(dc2h_num);
    unsigned int max = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_y;

   if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_y == 0 || bufnum == 1)
    {
        index = 0;
    }
    else
    {
        for( i = 1; i < bufnum; i++)
        {
            if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y > max)
            {
                max = CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y;
                index = i;
            }
        }
    }
    return index;
}

unsigned int sort_boundary_addr_min_index(unsigned int dc2h_num)
{
    unsigned int i,index = 0;
    unsigned int bufnum = get_vt_VtBufferNum(dc2h_num);
    unsigned int min = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_y;

    if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_y == 0 || bufnum == 1)
    {
        index = 0;
    }
    else
    {
        for( i = 1; i < bufnum; i++)
        {
            if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y < min)
            {
                min = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_y;
                index = i;
            }
        }
    }
    return index;
}

unsigned int sort_boundary_addr_max_index_2nd_plane(unsigned int dc2h_num)
{
    unsigned int i,index = 0;
    unsigned int bufnum = get_vt_VtBufferNum(dc2h_num);
    unsigned int max = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_u;

   if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_u == 0 || bufnum == 1)
    {
        index = 0;
    }
    else
    {
        for( i = 1; i < bufnum; i++)
        {
            if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_u > max)
            {
                max = CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_u;
                index = i;
            }
        }
    }
    return index;
}

unsigned int sort_boundary_addr_min_index_2nd_plane(unsigned int dc2h_num)
{
    unsigned int i,index = 0;
    unsigned int bufnum = get_vt_VtBufferNum(dc2h_num);
    unsigned int min = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_u;

    if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_u == 0 || bufnum == 1)
    {
        index = 0;
    }
    else
    {
        for( i = 1; i < bufnum; i++)
        {
            if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_u < min)
            {
                min = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_u;
                index = i;
            }
        }
    }
    return index;
}

unsigned int sort_boundary_addr_max_index_3rd_plane(unsigned int dc2h_num)
{
    unsigned int i,index = 0;
    unsigned int bufnum = get_vt_VtBufferNum(dc2h_num);
    unsigned int max = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_v;

   if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_v == 0 || bufnum == 1)
    {
        index = 0;
    }
    else
    {
        for( i = 1; i < bufnum; i++)
        {
            if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_v > max)
            {
                max = CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_v;
                index = i;
            }
        }
    }
    return index;
}

unsigned int sort_boundary_addr_min_index_3rd_plane(unsigned int dc2h_num)
{
    unsigned int i,index = 0;
    unsigned int bufnum = get_vt_VtBufferNum(dc2h_num);
    unsigned int min = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_y;

    if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_y == 0 || bufnum == 1)
    {
        index = 0;
    }
    else
    {
        for( i = 1; i < bufnum; i++)
        {
            if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y < min)
            {
                min = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].pa_y;
                index = i;
            }
        }
    }
    return index;
}

void set_dc2h_cap0_line_burst_num(int burst_num, unsigned int capLen, VT_CAP_FRAME_LINE_MODE cap_mode)
{
	dc2h_cap_dc2h_cap0_wr_dma_num_bl_wrap_addr_RBUS dc2h_cap0_wr_dma_num_bl_wrap_addr_reg;
	dc2h_cap_dc2h_cap0_wr_dma_num_bl_wrap_line_step_RBUS dc2h_cap0_wr_dma_num_bl_wrap_line_step_reg;
	dc2h_cap_dc2h_cap0_wr_dma_num_bl_wrap_ctl_RBUS dc2h_cap0_wr_dma_num_bl_wrap_ctl_reg;
	dc2h_cap_dc2h_cap0_wr_dma_num_bl_wrap_word_RBUS dc2h_cap0_wr_dma_num_bl_wrap_word_reg;

	dc2h_cap_dc2h_cap0_dma_wr_rule_check_up_RBUS dc2h_cap0_dma_wr_rule_check_up_reg;
	dc2h_cap_dc2h_cap0_dma_wr_rule_check_low_RBUS dc2h_cap0_dma_wr_rule_check_low_reg;

	dc2h_cap0_wr_dma_num_bl_wrap_addr_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP0_WR_DMA_num_bl_wrap_addr_reg);
	dc2h_cap0_wr_dma_num_bl_wrap_addr_reg.dc2h_cap0_sta_addr = CapBuffer_VT[DC2H1].vt_pa_buf_info[0].pa_y >> 4;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h_cap0_wr_dma_num_bl_wrap_addr_reg.dc2h_cap0_sta_addr += (capLen - 1) * burst_num;
	}
	IoReg_Write32(DC2H_CAP_DC2H_CAP0_WR_DMA_num_bl_wrap_addr_reg, dc2h_cap0_wr_dma_num_bl_wrap_addr_reg.regValue);

	dc2h_cap0_wr_dma_num_bl_wrap_line_step_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP0_WR_DMA_num_bl_wrap_line_step_reg);
	dc2h_cap0_wr_dma_num_bl_wrap_line_step_reg.dc2h_cap0_line_step = burst_num;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h_cap0_wr_dma_num_bl_wrap_line_step_reg.dc2h_cap0_line_step = -burst_num;
	}
	IoReg_Write32(DC2H_CAP_DC2H_CAP0_WR_DMA_num_bl_wrap_line_step_reg, dc2h_cap0_wr_dma_num_bl_wrap_line_step_reg.regValue);

	dc2h_cap0_wr_dma_num_bl_wrap_ctl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP0_WR_DMA_num_bl_wrap_ctl_reg);
	if(cap_mode == DC2H_CAP_LINE_MODE){
		dc2h_cap0_wr_dma_num_bl_wrap_ctl_reg.dc2h_cap0_line_num = capLen;
	}else{
		dc2h_cap0_wr_dma_num_bl_wrap_ctl_reg.dc2h_cap0_line_num = 1; /* frame mode */
	}
	dc2h_cap0_wr_dma_num_bl_wrap_ctl_reg.dc2h_cap0_burst_len = DEFAULT_DC2H_CAP_BURST_LEN; /* dc2h_cap0_burst_len*128bit */
	IoReg_Write32(DC2H_CAP_DC2H_CAP0_WR_DMA_num_bl_wrap_ctl_reg, dc2h_cap0_wr_dma_num_bl_wrap_ctl_reg.regValue);

	dc2h_cap0_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP0_WR_DMA_num_bl_wrap_word_reg);
	dc2h_cap0_wr_dma_num_bl_wrap_word_reg.dc2h_cap0_line_burst_num = burst_num;
	IoReg_Write32(DC2H_CAP_DC2H_CAP0_WR_DMA_num_bl_wrap_word_reg, dc2h_cap0_wr_dma_num_bl_wrap_word_reg.regValue);

	// dc2h_cap0_dma_wr_rule_check_low_reg.regValue = CapBuffer_VT[0].vt_pa_buf_info[0].pa_y;
	// IoReg_Write32(DC2H_CAP_DC2H_CAP0_DMA_WR_Rule_check_low_reg, dc2h_cap0_dma_wr_rule_check_low_reg.regValue);
	// dc2h_cap0_dma_wr_rule_check_up_reg.regValue = CapBuffer_VT[0].vt_pa_buf_info[0].pa_y + CapBuffer_VT[0].vt_pa_buf_info[0].size_plane_y - DEFAULT_DC2H_CAP_BURST_LEN;
	// IoReg_Write32(DC2H_CAP_DC2H_CAP0_DMA_WR_Rule_check_up_reg, dc2h_cap0_dma_wr_rule_check_up_reg.regValue);
	dc2h_cap0_dma_wr_rule_check_low_reg.regValue = CapBuffer_VT[DC2H1].vt_pa_buf_info[sort_boundary_addr_min_index(DC2H1)].pa_y;
	IoReg_Write32(DC2H_CAP_DC2H_CAP0_DMA_WR_Rule_check_low_reg, dc2h_cap0_dma_wr_rule_check_low_reg.regValue);
	dc2h_cap0_dma_wr_rule_check_up_reg.regValue = CapBuffer_VT[DC2H1].vt_pa_buf_info[sort_boundary_addr_max_index(DC2H1)].pa_y + CapBuffer_VT[DC2H1].vt_pa_buf_info[0].size_plane_y - DEFAULT_DC2H_CAP_BURST_LEN;
	IoReg_Write32(DC2H_CAP_DC2H_CAP0_DMA_WR_Rule_check_up_reg, dc2h_cap0_dma_wr_rule_check_up_reg.regValue);
}

void set_dc2h_cap1_line_burst_num(int burst_num, unsigned int capLen, VT_CAP_FRAME_LINE_MODE cap_mode)
{
	dc2h_cap_dc2h_cap1_wr_dma_num_bl_wrap_addr_RBUS dc2h_cap1_wr_dma_num_bl_wrap_addr_reg;
	dc2h_cap_dc2h_cap1_wr_dma_num_bl_wrap_line_step_RBUS dc2h_cap1_wr_dma_num_bl_wrap_line_step_reg;
	dc2h_cap_dc2h_cap1_wr_dma_num_bl_wrap_ctl_RBUS dc2h_cap1_wr_dma_num_bl_wrap_ctl_reg;
	dc2h_cap_dc2h_cap1_wr_dma_num_bl_wrap_word_RBUS dc2h_cap1_wr_dma_num_bl_wrap_word_reg;

	dc2h_cap_dc2h_cap1_dma_wr_rule_check_up_RBUS dc2h_cap1_dma_wr_rule_check_up_reg;
	dc2h_cap_dc2h_cap1_dma_wr_rule_check_low_RBUS dc2h_cap1_dma_wr_rule_check_low_reg;

	dc2h_cap1_wr_dma_num_bl_wrap_addr_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP1_WR_DMA_num_bl_wrap_addr_reg);
	dc2h_cap1_wr_dma_num_bl_wrap_addr_reg.dc2h_cap1_sta_addr = CapBuffer_VT[DC2H1].vt_pa_buf_info[0].pa_u >> 4;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h_cap1_wr_dma_num_bl_wrap_addr_reg.dc2h_cap1_sta_addr += (capLen - 1) * burst_num;
	}
	IoReg_Write32(DC2H_CAP_DC2H_CAP1_WR_DMA_num_bl_wrap_addr_reg, dc2h_cap1_wr_dma_num_bl_wrap_addr_reg.regValue);

	dc2h_cap1_wr_dma_num_bl_wrap_line_step_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP1_WR_DMA_num_bl_wrap_line_step_reg);
	dc2h_cap1_wr_dma_num_bl_wrap_line_step_reg.dc2h_cap1_line_step = burst_num;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h_cap1_wr_dma_num_bl_wrap_line_step_reg.dc2h_cap1_line_step = -burst_num;
	}
	IoReg_Write32(DC2H_CAP_DC2H_CAP1_WR_DMA_num_bl_wrap_line_step_reg, dc2h_cap1_wr_dma_num_bl_wrap_line_step_reg.regValue);

	dc2h_cap1_wr_dma_num_bl_wrap_ctl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP1_WR_DMA_num_bl_wrap_ctl_reg);
	if(cap_mode == DC2H_CAP_LINE_MODE){
		dc2h_cap1_wr_dma_num_bl_wrap_ctl_reg.dc2h_cap1_line_num = capLen;
	}else{
		dc2h_cap1_wr_dma_num_bl_wrap_ctl_reg.dc2h_cap1_line_num = 1; /* frame mode */
	}
	dc2h_cap1_wr_dma_num_bl_wrap_ctl_reg.dc2h_cap1_burst_len = DEFAULT_DC2H_CAP_BURST_LEN; /* dc2h_cap1_burst_len*128bit */
	IoReg_Write32(DC2H_CAP_DC2H_CAP1_WR_DMA_num_bl_wrap_ctl_reg, dc2h_cap1_wr_dma_num_bl_wrap_ctl_reg.regValue);

	dc2h_cap1_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP1_WR_DMA_num_bl_wrap_word_reg);
	dc2h_cap1_wr_dma_num_bl_wrap_word_reg.dc2h_cap1_line_burst_num = burst_num;
	IoReg_Write32(DC2H_CAP_DC2H_CAP1_WR_DMA_num_bl_wrap_word_reg, dc2h_cap1_wr_dma_num_bl_wrap_word_reg.regValue);

	dc2h_cap1_dma_wr_rule_check_low_reg.regValue = CapBuffer_VT[DC2H1].vt_pa_buf_info[sort_boundary_addr_min_index_2nd_plane(DC2H1)].pa_u;
	IoReg_Write32(DC2H_CAP_DC2H_CAP1_DMA_WR_Rule_check_low_reg, dc2h_cap1_dma_wr_rule_check_low_reg.regValue);

	dc2h_cap1_dma_wr_rule_check_up_reg.regValue = CapBuffer_VT[DC2H1].vt_pa_buf_info[sort_boundary_addr_max_index_2nd_plane(DC2H1)].pa_u + CapBuffer_VT[DC2H1].vt_pa_buf_info[0].size_plane_u - DEFAULT_DC2H_CAP_BURST_LEN;
	IoReg_Write32(DC2H_CAP_DC2H_CAP1_DMA_WR_Rule_check_up_reg, dc2h_cap1_dma_wr_rule_check_up_reg.regValue);
}

void set_dc2h_cap2_line_burst_num(int burst_num, unsigned int capLen, VT_CAP_FRAME_LINE_MODE cap_mode)
{
	dc2h_cap_dc2h_cap2_wr_dma_num_bl_wrap_addr_RBUS dc2h_cap2_wr_dma_num_bl_wrap_addr_reg;
	dc2h_cap_dc2h_cap2_wr_dma_num_bl_wrap_line_step_RBUS dc2h_cap2_wr_dma_num_bl_wrap_line_step_reg;
	dc2h_cap_dc2h_cap2_wr_dma_num_bl_wrap_ctl_RBUS dc2h_cap2_wr_dma_num_bl_wrap_ctl_reg;
	dc2h_cap_dc2h_cap2_wr_dma_num_bl_wrap_word_RBUS dc2h_cap2_wr_dma_num_bl_wrap_word_reg;

	dc2h_cap_dc2h_cap2_dma_wr_rule_check_up_RBUS dc2h_cap2_dma_wr_rule_check_up_reg;
	dc2h_cap_dc2h_cap2_dma_wr_rule_check_low_RBUS dc2h_cap2_dma_wr_rule_check_low_reg;

	dc2h_cap2_wr_dma_num_bl_wrap_addr_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP2_WR_DMA_num_bl_wrap_addr_reg);
	dc2h_cap2_wr_dma_num_bl_wrap_addr_reg.dc2h_cap2_sta_addr = CapBuffer_VT[DC2H1].vt_pa_buf_info[0].pa_v >> 4;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h_cap2_wr_dma_num_bl_wrap_addr_reg.dc2h_cap2_sta_addr += (capLen - 1) * burst_num;
	}
	IoReg_Write32(DC2H_CAP_DC2H_CAP2_WR_DMA_num_bl_wrap_addr_reg, dc2h_cap2_wr_dma_num_bl_wrap_addr_reg.regValue);

	dc2h_cap2_wr_dma_num_bl_wrap_line_step_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP2_WR_DMA_num_bl_wrap_line_step_reg);
	dc2h_cap2_wr_dma_num_bl_wrap_line_step_reg.dc2h_cap2_line_step = burst_num;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h_cap2_wr_dma_num_bl_wrap_line_step_reg.dc2h_cap2_line_step = -burst_num;
	}
	IoReg_Write32(DC2H_CAP_DC2H_CAP2_WR_DMA_num_bl_wrap_line_step_reg, dc2h_cap2_wr_dma_num_bl_wrap_line_step_reg.regValue);

	dc2h_cap2_wr_dma_num_bl_wrap_ctl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP2_WR_DMA_num_bl_wrap_ctl_reg);
	if(cap_mode == DC2H_CAP_LINE_MODE){
		dc2h_cap2_wr_dma_num_bl_wrap_ctl_reg.dc2h_cap2_line_num = capLen;
	}else{
		dc2h_cap2_wr_dma_num_bl_wrap_ctl_reg.dc2h_cap2_line_num = 1; /* frame mode */
	}
	dc2h_cap2_wr_dma_num_bl_wrap_ctl_reg.dc2h_cap2_burst_len = DEFAULT_DC2H_CAP_BURST_LEN; /* dc2h_cap2_burst_len*128bit */
	IoReg_Write32(DC2H_CAP_DC2H_CAP2_WR_DMA_num_bl_wrap_ctl_reg, dc2h_cap2_wr_dma_num_bl_wrap_ctl_reg.regValue);

	dc2h_cap2_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP2_WR_DMA_num_bl_wrap_word_reg);
	dc2h_cap2_wr_dma_num_bl_wrap_word_reg.dc2h_cap2_line_burst_num = burst_num;
	IoReg_Write32(DC2H_CAP_DC2H_CAP2_WR_DMA_num_bl_wrap_word_reg, dc2h_cap2_wr_dma_num_bl_wrap_word_reg.regValue);

	dc2h_cap2_dma_wr_rule_check_low_reg.regValue = CapBuffer_VT[DC2H1].vt_pa_buf_info[sort_boundary_addr_min_index_3rd_plane(DC2H1)].pa_v;
	IoReg_Write32(DC2H_CAP_DC2H_CAP2_DMA_WR_Rule_check_low_reg, dc2h_cap2_dma_wr_rule_check_low_reg.regValue);

	dc2h_cap2_dma_wr_rule_check_up_reg.regValue = CapBuffer_VT[DC2H1].vt_pa_buf_info[sort_boundary_addr_max_index_3rd_plane(DC2H1)].pa_v + CapBuffer_VT[DC2H1].vt_pa_buf_info[0].size_plane_v - DEFAULT_DC2H_CAP_BURST_LEN;
	IoReg_Write32(DC2H_CAP_DC2H_CAP2_DMA_WR_Rule_check_up_reg, dc2h_cap2_dma_wr_rule_check_up_reg.regValue);
}

void set_dc2h_capture_sdnr_in_clock(UINT8 enable)
{
    scaledown_cts_fifo_gap_RBUS scaledown_cts_fifo_gap_reg;
    scaledown_cts_fifo_gap_reg.regValue = rtd_inl(SCALEDOWN_CTS_FIFO_GAP_reg);
    if(enable == TRUE)
    {
        scaledown_cts_fifo_gap_reg.s1_r_dc2h_clk_en = 1;
    }
    else
    {
        scaledown_cts_fifo_gap_reg.s1_r_dc2h_clk_en = 0;
    }
    rtd_outl(SCALEDOWN_CTS_FIFO_GAP_reg, scaledown_cts_fifo_gap_reg.regValue);
}

void reset_dc2h_hw_setting(void)
{
	dc2h_cap_dc2h_ctrl_RBUS dc2h_cap_ctrl_reg;
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	dc2h_cap_dc2h_interrupt_enable_RBUS dc2h_interrupt_enable_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_rgb2yuv_ctrl_RBUS dc2h_rgb2yuv_ctrl_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_pq_db_ctrl_RBUS dc2h_r2y_dither_4xxto4xx_dc2h_pq_db_ctrl_reg;
	dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_RBUS dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_reg;
	dc2h_scaledown_dc2h1_uzd_h_db_ctrl_RBUS dc2h_scaledown_dc2h1_uzd_h_db_ctrl_reg;
	dc2h_scaledown_dc2h1_uzd_v_db_ctrl_RBUS dc2h_scaledown_dc2h1_uzd_v_db_ctrl_reg;
	dc2h_scaledown_dc2h1_uzd_ctrl0_RBUS dc2h_scaledown_dc2h1_uzd_ctrl0_reg;

	if (get_vt_VtSwBufferMode(DC2H1))
	{
		dc2h_interrupt_enable_reg.regValue = rtd_inl(DC2H_CAP_DC2H_Interrupt_Enable_reg);
		dc2h_interrupt_enable_reg.dc2h_vs_rising_int_en = 0;
		rtd_outl(DC2H_CAP_DC2H_Interrupt_Enable_reg, dc2h_interrupt_enable_reg.regValue);
		msleep(20);
	}

	dc2h_r2y_dither_4xxto4xx_dc2h_pq_db_ctrl_reg.regValue = rtd_inl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_PQ_DB_CTRL_reg);
	dc2h_r2y_dither_4xxto4xx_dc2h_pq_db_ctrl_reg.db_en = 0;
	rtd_outl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_PQ_DB_CTRL_reg, dc2h_r2y_dither_4xxto4xx_dc2h_pq_db_ctrl_reg.regValue);

	dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_COEF_DB_CTRL_reg);
	dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_reg.coef_db_en = 0;
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_COEF_DB_CTRL_reg, dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_reg.regValue);

	dc2h_scaledown_dc2h1_uzd_h_db_ctrl_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_H_DB_CTRL_reg);
	dc2h_scaledown_dc2h1_uzd_h_db_ctrl_reg.h_db_en = 0;
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_H_DB_CTRL_reg, dc2h_scaledown_dc2h1_uzd_h_db_ctrl_reg.regValue);

	dc2h_scaledown_dc2h1_uzd_v_db_ctrl_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_V_DB_CTRL_reg);
	dc2h_scaledown_dc2h1_uzd_v_db_ctrl_reg.v_db_en = 0;
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_V_DB_CTRL_reg, dc2h_scaledown_dc2h1_uzd_v_db_ctrl_reg.regValue);

	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_cap0_en = 0;
	dc2h_cap_color_format_reg.dc2h_cap1_en = 0;
	dc2h_cap_color_format_reg.dc2h_cap2_en = 0;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);

	dc2h_cap_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CTRL_reg);
	dc2h_cap_ctrl_reg.dc2h_in_sel = _NO_INPUT;
	rtd_outl(DC2H_CAP_DC2H_CTRL_reg, dc2h_cap_ctrl_reg.regValue);

	dc2h_rgb2yuv_ctrl_reg.regValue = rtd_inl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg);
	dc2h_rgb2yuv_ctrl_reg.en_rgb2yuv = 0;
	rtd_outl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg, dc2h_rgb2yuv_ctrl_reg.regValue);

	dc2h_scaledown_dc2h1_uzd_ctrl0_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_CTRL0_reg);
	dc2h_scaledown_dc2h1_uzd_ctrl0_reg.h_zoom_en = 0;
	dc2h_scaledown_dc2h1_uzd_ctrl0_reg.v_zoom_en = 0;
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_CTRL0_reg, dc2h_scaledown_dc2h1_uzd_ctrl0_reg.regValue);

    set_dc2h_capture_sdnr_in_clock(FALSE);
}

#ifndef UT_flag
// ----------------------------------------------------dc2h---------------------------
void set_dc2h_black_out_en(unsigned char enable, unsigned int dc2h_num)
{
	unsigned int dc2h_black_out_en_reg[DC2H_NUMBER] = {DC2H_CAP_DC2H_BLACK_OUT_reg, DC2H2_CAP_DC2H2_BLACK_OUT_reg};
	unsigned int dc2h_black_out_mask[DC2H_NUMBER] = {DC2H_CAP_DC2H_BLACK_OUT_dc2h_black_out_en_mask, DC2H2_CAP_DC2H2_BLACK_OUT_dc2h2_black_out_en_mask};
	unsigned int dc2h_cap_db_reg[DC2H_NUMBER] = {DC2H_CAP_DC2H_CAP_doublebuffer_reg, DC2H2_CAP_DC2H2_CAP_doublebuffer_reg};
	unsigned int dc2h_cap_db_en_mask[DC2H_NUMBER] = {DC2H_CAP_DC2H_CAP_doublebuffer_dc2h_db_en_mask, DC2H2_CAP_DC2H2_CAP_doublebuffer_dc2h2_db_en_mask};
	unsigned int dc2h_cap_db_apply_mask[DC2H_NUMBER] = {DC2H_CAP_DC2H_CAP_doublebuffer_dc2h_db_apply_mask, DC2H2_CAP_DC2H2_CAP_doublebuffer_dc2h2_db_apply_mask};

	if(enable == TRUE){
		IoReg_SetBits(dc2h_black_out_en_reg[dc2h_num], dc2h_black_out_mask[dc2h_num]);
	}else{
		IoReg_ClearBits(dc2h_black_out_en_reg[dc2h_num], dc2h_black_out_mask[dc2h_num]);
	}

	if(IoReg_Read32(dc2h_cap_db_reg[dc2h_num]) & dc2h_cap_db_en_mask[dc2h_num]){
		IoReg_SetBits(dc2h_cap_db_reg[dc2h_num], dc2h_cap_db_apply_mask[dc2h_num]);
	}
}

void set_dc2h_cap_color_fmt_rgb(VT_CAP_FRAME_LINE_MODE dc2h_cap_mode)
{
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_frame_access_mode = dc2h_cap_mode;
	dc2h_cap_color_format_reg.dc2h_seq_blk_sel = DC2H_SEQ_MODE;
	dc2h_cap_color_format_reg.dc2h_pixel_encoding = PIXEL_ENCODING_444;
	dc2h_cap_color_format_reg.dc2h_bit_sel = PER_CHANNEL_8_BIT;
	dc2h_cap_color_format_reg.dc2h_yv12_en = CAP_YV12_DISABLE;
	dc2h_cap_color_format_reg.dc2h_p010_en = CAP_P010_DISABLE;
	dc2h_cap_color_format_reg.dc2h_cb_cr_swap = DC2H_CB_CR_NO_SWAP;
	dc2h_cap_color_format_reg.dc2h_rgb_to_argb_en = RGB_TO_ARGB_DISABLE;
	dc2h_cap_color_format_reg.dc2h_channel_swap = CHANNEL_SWAP_BGR;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);
}

void set_dc2h_cap_color_fmt_argb(VT_CAP_ARGB_TYPE alpha_location, VT_CAP_FRAME_LINE_MODE dc2h_cap_mode)
{
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_frame_access_mode = dc2h_cap_mode;
	dc2h_cap_color_format_reg.dc2h_seq_blk_sel = DC2H_SEQ_MODE;
	dc2h_cap_color_format_reg.dc2h_pixel_encoding = PIXEL_ENCODING_444;
	dc2h_cap_color_format_reg.dc2h_bit_sel = PER_CHANNEL_8_BIT;
	dc2h_cap_color_format_reg.dc2h_yv12_en = CAP_YV12_DISABLE;
	dc2h_cap_color_format_reg.dc2h_p010_en = CAP_P010_DISABLE;
	dc2h_cap_color_format_reg.dc2h_cb_cr_swap = DC2H_CB_CR_NO_SWAP;
	dc2h_cap_color_format_reg.dc2h_rgb_to_argb_en = RGB_TO_ARGB_ENABLE;
	dc2h_cap_color_format_reg.dc2h_channel_swap = CHANNEL_SWAP_RGB;
	dc2h_cap_color_format_reg.dc2h_argb_dummy_data = 0xFF;
	dc2h_cap_color_format_reg.dc2h_argb_dummy_loc = alpha_location;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);
}

void set_dc2h_cap_color_fmt_nv12(VT_CAP_BIT_SEL bit_sel, VT_CAP_FRAME_LINE_MODE dc2h_cap_mode, VT_CAP_CB_CR_SWAP cbcr_swap)
{
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_frame_access_mode = dc2h_cap_mode;
	dc2h_cap_color_format_reg.dc2h_seq_blk_sel = DC2H_BLK_MODE;
	dc2h_cap_color_format_reg.dc2h_pixel_encoding = PIXEL_ENCODING_420;
	dc2h_cap_color_format_reg.dc2h_bit_sel = bit_sel; /* NV12 8bit or 10bit */
	dc2h_cap_color_format_reg.dc2h_yv12_en = CAP_YV12_DISABLE;
	dc2h_cap_color_format_reg.dc2h_p010_en = CAP_P010_DISABLE;
	dc2h_cap_color_format_reg.dc2h_cb_cr_swap = cbcr_swap;
	dc2h_cap_color_format_reg.dc2h_rgb_to_argb_en = RGB_TO_ARGB_DISABLE;
	dc2h_cap_color_format_reg.dc2h_channel_swap = CHANNEL_SWAP_GBR;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);
}

void set_dc2h_cap_color_fmt_yv12(VT_CAP_FRAME_LINE_MODE dc2h_cap_mode)
{
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_frame_access_mode = dc2h_cap_mode;
	dc2h_cap_color_format_reg.dc2h_seq_blk_sel = DC2H_BLK_MODE;
	dc2h_cap_color_format_reg.dc2h_pixel_encoding = PIXEL_ENCODING_420;
	dc2h_cap_color_format_reg.dc2h_bit_sel = PER_CHANNEL_8_BIT;
	dc2h_cap_color_format_reg.dc2h_yv12_en = CAP_YV12_ENABLE;
	dc2h_cap_color_format_reg.dc2h_p010_en = CAP_P010_DISABLE;
	dc2h_cap_color_format_reg.dc2h_cb_cr_swap = DC2H_CB_CR_SWAP;
	dc2h_cap_color_format_reg.dc2h_rgb_to_argb_en = RGB_TO_ARGB_DISABLE;
	dc2h_cap_color_format_reg.dc2h_channel_swap = CHANNEL_SWAP_GBR;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);
}

void set_dc2h_cap_color_fmt_i420(VT_CAP_FRAME_LINE_MODE dc2h_cap_mode)
{
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_frame_access_mode = dc2h_cap_mode;
	dc2h_cap_color_format_reg.dc2h_seq_blk_sel = DC2H_BLK_MODE;
	dc2h_cap_color_format_reg.dc2h_pixel_encoding = PIXEL_ENCODING_420;
	dc2h_cap_color_format_reg.dc2h_bit_sel = PER_CHANNEL_8_BIT;
	dc2h_cap_color_format_reg.dc2h_yv12_en = CAP_YV12_ENABLE;
	dc2h_cap_color_format_reg.dc2h_p010_en = CAP_P010_DISABLE;
	dc2h_cap_color_format_reg.dc2h_cb_cr_swap = DC2H_CB_CR_NO_SWAP;
	dc2h_cap_color_format_reg.dc2h_rgb_to_argb_en = RGB_TO_ARGB_DISABLE;
	dc2h_cap_color_format_reg.dc2h_channel_swap = CHANNEL_SWAP_GBR;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);
}

void set_dc2h_cap_color_fmt_p010(VT_CAP_FRAME_LINE_MODE dc2h_cap_mode)
{
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_frame_access_mode = dc2h_cap_mode;
	dc2h_cap_color_format_reg.dc2h_seq_blk_sel = DC2H_BLK_MODE;
	dc2h_cap_color_format_reg.dc2h_pixel_encoding = PIXEL_ENCODING_420;
	dc2h_cap_color_format_reg.dc2h_bit_sel = PER_CHANNEL_10_BIT;
	dc2h_cap_color_format_reg.dc2h_yv12_en = CAP_YV12_DISABLE;
	dc2h_cap_color_format_reg.dc2h_p010_en = CAP_P010_ENABLE;
	dc2h_cap_color_format_reg.dc2h_cb_cr_swap = DC2H_CB_CR_SWAP;
	dc2h_cap_color_format_reg.dc2h_rgb_to_argb_en = RGB_TO_ARGB_DISABLE;
	dc2h_cap_color_format_reg.dc2h_channel_swap = CHANNEL_SWAP_GBR;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);
}

void set_dc2h_in_size_and_in_sel(unsigned int location)
{
	unsigned int input_w = 0, input_h = 0;
	dc2h_cap_dc2h_ctrl_RBUS dc2h_cap_dc2h_ctrl_reg;
	ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;

	ppoverlay_main_den_h_start_end_RBUS main_den_h_start_end_Reg;
	ppoverlay_main_den_v_start_end_RBUS main_den_v_start_end_Reg;
	ppoverlay_main_active_h_start_end_RBUS main_active_h_start_end_Reg;
	ppoverlay_main_active_v_start_end_RBUS main_active_v_start_end_Reg;
	ppoverlay_sub_active_h_start_end_RBUS sub_active_h_start_end_reg;
	ppoverlay_sub_active_v_start_end_RBUS sub_active_v_start_end_reg;
	ppoverlay_osd4dtg_dv_den_start_end_RBUS ppoverlay_osd4dtg_dv_den_start_end_reg;
	ppoverlay_osd4dtg_dh_den_start_end_RBUS ppoverlay_osd4dtg_dh_den_start_end_reg;

	dc2h_cap_dc2h_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CTRL_reg);
	dc2h_cap_dc2h_ctrl_reg.clken_disp_dc2h = 1; 
	//rtd_outl(DC2H_CAP_DC2H_CTRL_reg, dc2h_cap_dc2h_ctrl_reg.regValue);

	if(location == _MAIN_UZU || location == _TWO_SECOND_UZU){
		main_active_h_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_H_Start_End_reg);
		main_active_v_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_V_Start_End_reg);
		ppoverlay_uzudtg_control1_reg.regValue = rtd_inl(PPOVERLAY_uzudtg_control1_reg);
		input_w = (main_active_h_start_end_Reg.mh_act_end - main_active_h_start_end_Reg.mh_act_sta);
		input_h = (main_active_v_start_end_Reg.mv_act_end - main_active_v_start_end_Reg.mv_act_sta);
		if(ppoverlay_uzudtg_control1_reg.two_step_uzu_en && location == _MAIN_UZU){
			input_w = input_w >> 1;
			input_h = input_h >> 1;
		}
	}else if(location == _Sub_UZU_Output){
		sub_active_h_start_end_reg.regValue = rtd_inl(PPOVERLAY_SUB_Active_H_Start_End_reg);
		sub_active_v_start_end_reg.regValue = rtd_inl(PPOVERLAY_SUB_Active_V_Start_End_reg);
		input_w = (sub_active_h_start_end_reg.sh_act_end - sub_active_h_start_end_reg.sh_act_sta);
		input_h = (sub_active_v_start_end_reg.sv_act_end - sub_active_v_start_end_reg.sv_act_sta);
	}else if(location == _MAIN_YUV2RGB || location == _OSD123_MIXER_Output || location == _Dither_Output || location == _Memc_mux_Output ||
		location == _Memc_Mux_Input){
		main_den_h_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_DEN_H_Start_End_reg);
		main_den_v_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_DEN_V_Start_End_reg);
		input_w = (main_den_h_start_end_Reg.mh_den_end - main_den_h_start_end_Reg.mh_den_sta);
		input_h = (main_den_v_start_end_Reg.mv_den_end - main_den_v_start_end_Reg.mv_den_sta);
	}else if(location == _Idomain_Sdnr_input){
        input_w = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DI_WID);
        input_h = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DI_LEN);
        set_dc2h_capture_sdnr_in_clock(TRUE);
    }else if(location == _OSD4_Output){
		ppoverlay_osd4dtg_dh_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_osd4dtg_DH_DEN_Start_End_reg);
		ppoverlay_osd4dtg_dv_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_osd4dtg_DV_DEN_Start_End_reg);
		input_w = (ppoverlay_osd4dtg_dh_den_start_end_reg.osd4dtg_dh_den_end - ppoverlay_osd4dtg_dh_den_start_end_reg.osd4dtg_dh_den_sta);
		input_h = (ppoverlay_osd4dtg_dv_den_start_end_reg.osd4dtg_dv_den_end - ppoverlay_osd4dtg_dv_den_start_end_reg.osd4dtg_dv_den_sta);
	}else{
        rtd_pr_vt_emerg("[error]invalid cap position %d, line:%d\n",__LINE__, location);
		dc2h_cap_dc2h_ctrl_reg.clken_disp_dc2h = 0;
		rtd_outl(DC2H_CAP_DC2H_CTRL_reg, dc2h_cap_dc2h_ctrl_reg.regValue);
		location = _NO_INPUT;
    }

	dc2h_cap_dc2h_ctrl_reg.dc2h_in_sel = location;
	dc2h_cap_dc2h_ctrl_reg.dc2h_in_vact = input_h;
	rtd_outl(DC2H_CAP_DC2H_CTRL_reg, dc2h_cap_dc2h_ctrl_reg.regValue);

	/* save dc2h original input size */
	curCapInfo[DC2H1].input_size.x = 0;
	curCapInfo[DC2H1].input_size.y = 0;
	curCapInfo[DC2H1].input_size.w = input_w;
	curCapInfo[DC2H1].input_size.h = input_h;

	rtd_pr_vt_notice("[VT]vt cap src=%d,input w=%d,h=%d\n", location, input_w, input_h);

    // SDNR input W >4096 or V >2160, dc2h output capture max 2k
    if(input_w > VT_CAP_FRAME_WIDTH_4K2K || input_h > VT_CAP_FRAME_HEIGHT_4K2K){
        if(curCapInfo[DC2H1].output_size.w > VT_CAP_FRAME_WIDTH_2K1k && curCapInfo[DC2H1].output_size.w < input_w){
			curCapInfo[DC2H1].output_size.w = VT_CAP_FRAME_WIDTH_2K1k;
		}
		if(curCapInfo[DC2H1].output_size.h > VT_CAP_FRAME_HEIGHT_2K1k && curCapInfo[DC2H1].output_size.h < input_h){
			curCapInfo[DC2H1].output_size.h = VT_CAP_FRAME_HEIGHT_2K1k;
		}
		rtd_pr_vt_notice("Input hact over 3840,vact over 2160 , output max support 2k1k %dx%d\n", curCapInfo[DC2H1].output_size.w, curCapInfo[DC2H1].output_size.h);
     }
}

unsigned char drvif_set_dc2h_swmode_inforpc(unsigned int onoff, VT_VIDEO_FRAME_BUFFER_PROPERTY_INFO_T *pdc2h_buf_info)
{
	DC2H_SWMODE_STRUCT_T *swmode_infoptr;

	int ret;
    int i;
	int buf_num = 0;
	rtd_pr_vt_notice("fun:%s, onoff %d\n", __FUNCTION__, onoff);
	swmode_infoptr = (DC2H_SWMODE_STRUCT_T *)Scaler_GetShareMemVirAddr(SCALERIOC_SET_DC2H_SWMODE_ENABLE);

	buf_num = get_vt_VtBufferNum(DC2H1);
	swmode_infoptr->SwModeEnable = onoff;
	swmode_infoptr->buffernumber = buf_num;
	swmode_infoptr->cap_format = pdc2h_buf_info->pixelFormat;
	swmode_infoptr->cap_width = pdc2h_buf_info->width;
	swmode_infoptr->cap_length = pdc2h_buf_info->height;
	swmode_infoptr->user_fps = get_vt_target_fps(DC2H1);
	//swmode_infoptr->YbufferSize = vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height);
	// swmode_infoptr->YbufferSize = width * length;

	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h_cap_dc2h_cap0_wr_dma_num_bl_wrap_word_RBUS dc2h_cap0_wr_dma_num_bl_wrap_word_reg;
		dc2h_cap_dc2h_cap1_wr_dma_num_bl_wrap_word_RBUS dc2h_cap1_wr_dma_num_bl_wrap_word_reg;
		dc2h_cap_dc2h_cap2_wr_dma_num_bl_wrap_word_RBUS dc2h_cap2_wr_dma_num_bl_wrap_word_reg;
		dc2h_cap0_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP0_WR_DMA_num_bl_wrap_word_reg);
		dc2h_cap1_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP1_WR_DMA_num_bl_wrap_word_reg);
		dc2h_cap2_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP2_WR_DMA_num_bl_wrap_word_reg);

		for (i = 0; i < buf_num; i++){
			swmode_infoptr->cap_buffers[i].pa_y = ((pdc2h_buf_info->vt_pa_buf_info[i].pa_y) ? (pdc2h_buf_info->vt_pa_buf_info[i].pa_y + (pdc2h_buf_info->height - 1)*dc2h_cap0_wr_dma_num_bl_wrap_word_reg.dc2h_cap0_line_burst_num) : (0));

			if(pdc2h_buf_info->plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){ //two plane
				swmode_infoptr->cap_buffers[i].pa_u = ((pdc2h_buf_info->vt_pa_buf_info[i].pa_u) ? (pdc2h_buf_info->vt_pa_buf_info[i].pa_u + (pdc2h_buf_info->height/2 - 1)*dc2h_cap1_wr_dma_num_bl_wrap_word_reg.dc2h_cap1_line_burst_num) : (0));
			}

			if(pdc2h_buf_info->plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){ //three plane
				swmode_infoptr->cap_buffers[i].pa_v = ((pdc2h_buf_info->vt_pa_buf_info[i].pa_v) ? (pdc2h_buf_info->vt_pa_buf_info[i].pa_v + (pdc2h_buf_info->height/2 - 1)*dc2h_cap2_wr_dma_num_bl_wrap_word_reg.dc2h_cap2_line_burst_num) : (0));
			}
		}	
    }else{
		for (i = 0; i < buf_num; i++){
			swmode_infoptr->cap_buffers[i].pa_y = ((pdc2h_buf_info->vt_pa_buf_info[i].pa_y) ? (pdc2h_buf_info->vt_pa_buf_info[i].pa_y) : (0));
			
			if(pdc2h_buf_info->plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){ //two plane
				swmode_infoptr->cap_buffers[i].pa_u = ((pdc2h_buf_info->vt_pa_buf_info[i].pa_u) ? (pdc2h_buf_info->vt_pa_buf_info[i].pa_u) : (0));
			}

			if(pdc2h_buf_info->plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){ //three plane
				swmode_infoptr->cap_buffers[i].pa_v = ((pdc2h_buf_info->vt_pa_buf_info[i].pa_v) ? (pdc2h_buf_info->vt_pa_buf_info[i].pa_v) : (0));
			}
		}
	}

	//add debu log to print rpc info
	//change endian
	swmode_infoptr->SwModeEnable = htonl(swmode_infoptr->SwModeEnable);
	swmode_infoptr->buffernumber = htonl(swmode_infoptr->buffernumber);
	swmode_infoptr->cap_format = htonl(swmode_infoptr->cap_format);
	swmode_infoptr->cap_width = htonl(swmode_infoptr->cap_width);
	swmode_infoptr->cap_length = htonl(swmode_infoptr->cap_length);
	swmode_infoptr->user_fps = htonl(swmode_infoptr->user_fps);
	//swmode_infoptr->YbufferSize = htonl(swmode_infoptr->YbufferSize);

	for (i = 0; i < buf_num; i++){
		swmode_infoptr->cap_buffers[i].pa_y = htonl(swmode_infoptr->cap_buffers[i].pa_y);
		swmode_infoptr->cap_buffers[i].size_plane_y = htonl(swmode_infoptr->cap_buffers[i].size_plane_y);

		if(pdc2h_buf_info->plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){ //two plane
			swmode_infoptr->cap_buffers[i].pa_u = htonl(swmode_infoptr->cap_buffers[i].pa_u);
			swmode_infoptr->cap_buffers[i].size_plane_u = htonl(swmode_infoptr->cap_buffers[i].size_plane_u);
		}

		if(pdc2h_buf_info->plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){ //three plane
			swmode_infoptr->cap_buffers[i].pa_v = htonl(swmode_infoptr->cap_buffers[i].pa_v);
			swmode_infoptr->cap_buffers[i].size_plane_v = htonl(swmode_infoptr->cap_buffers[i].size_plane_v);
		}
	}

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_SET_DC2H_SWMODE_ENABLE,0,0))){
		rtd_pr_vt_emerg("[dc2h]ret=%d, SCALERIOC_SET_DC2H_SWMODE_ENABLE RPC fail !!!\n", ret);
		return FALSE;
	}
	return TRUE;
}

void drvif_set_dc2h_crop_config(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char en)
{
	dc2h_cap_dc2h_3dmaskto2d_ctrl_RBUS dc2h_cap_3dmaskto2d_ctrl_reg;
	dc2h_cap_dc2h_overscan_ctrl0_RBUS dc2h_cap_overscan_ctrl0_reg;
	dc2h_cap_dc2h_3dmaskto2d_ctrl1_RBUS dc2h_cap_3dmaskto2d_ctrl1_reg;

	dc2h_cap_3dmaskto2d_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_3DMaskTo2D_Ctrl_reg);
	dc2h_cap_overscan_ctrl0_reg.regValue = rtd_inl(DC2H_CAP_DC2H_OVERSCAN_Ctrl0_reg);
	dc2h_cap_3dmaskto2d_ctrl1_reg.regValue = rtd_inl(DC2H_CAP_DC2H_3DMaskTo2D_Ctrl1_reg);

	if(x % 2){
		x = x - (x % 2);
	}
	if((x + w) % 2){
		w = w - (w % 2);
	}

	dc2h_cap_overscan_ctrl0_reg.dc2h_overscan_left = x;
	dc2h_cap_overscan_ctrl0_reg.dc2h_overscan_right = x+w-2;
	rtd_outl(DC2H_CAP_DC2H_OVERSCAN_Ctrl0_reg, dc2h_cap_overscan_ctrl0_reg.regValue);

	dc2h_cap_3dmaskto2d_ctrl1_reg.dc2h_overscan_top = y;
	dc2h_cap_3dmaskto2d_ctrl1_reg.dc2h_overscan_bottom = y+h-1;
	rtd_outl(DC2H_CAP_DC2H_3DMaskTo2D_Ctrl1_reg, dc2h_cap_3dmaskto2d_ctrl1_reg.regValue);

	dc2h_cap_3dmaskto2d_ctrl_reg.dc2h_3dmaskto2d_3dformat = 4;/*overscan*/
	dc2h_cap_3dmaskto2d_ctrl_reg.dc2h_3dmaskto2d_h_size = curCapInfo[DC2H1].input_size.w;
	dc2h_cap_3dmaskto2d_ctrl_reg.dc2h_3dmaskto2d_mode = 0; /* L data */
	dc2h_cap_3dmaskto2d_ctrl_reg.dc2h_3dmaskto2d_en = (curCapInfo[DC2H1].cap_location == _Idomain_Sdnr_input) ? 0 : en;
    rtd_outl(DC2H_CAP_DC2H_3DMaskTo2D_Ctrl_reg, dc2h_cap_3dmaskto2d_ctrl_reg.regValue);

	/* save crop region */
	curCapInfo[DC2H1].crop_size.x = x;
	curCapInfo[DC2H1].crop_size.y = y;
	curCapInfo[DC2H1].crop_size.w = w;
	curCapInfo[DC2H1].crop_size.h = h;
	rtd_pr_vt_notice("saved crop size (%d,%d,%d,%d)\n", curCapInfo[DC2H1].crop_size.x, curCapInfo[DC2H1].crop_size.y, curCapInfo[DC2H1].crop_size.w, curCapInfo[DC2H1].crop_size.h);
}

unsigned char dc2h_cap_fmt_is_rgb_space(unsigned int cap_fmt)
{
	if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB && cap_fmt <= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ABGR){
		return TRUE;
	}else if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit && cap_fmt < V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_MAXN){
		return FALSE; //capture format is yuv space
	}
	else{
		rtd_pr_vt_notice("%s, error,unexpected capture format\n", __FUNCTION__);
		return TRUE;
	}
}

unsigned char dc2h_cap_fmt_is_10bit(unsigned int cap_fmt)
{
	if(cap_fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_10bit || cap_fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_P010){
		return TRUE;
	}else{
		return FALSE;
	}
}

unsigned char dc2h_cap_pixel_encoding_is_444(unsigned int cap_fmt)
{
	if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB && cap_fmt <= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ABGR){
		return TRUE;
	}else if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit && cap_fmt < V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_MAXN){
		return FALSE;
	}
	else{
		rtd_pr_vt_notice("%s, error, unexpected capture format\n", __FUNCTION__);
		return TRUE;
	}
}

DC2H_Enable_rgb2yuv set_dc2h_colconv_enable(unsigned int cap_fmt)
{
	unsigned int dc2h_in_sel = DC2H_CAP_DC2H_CTRL_get_dc2h_in_sel(IoReg_Read32(DC2H_CAP_DC2H_CTRL_reg));
	DC2H_Enable_rgb2yuv dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;

	if(dc2h_in_sel == _NO_INPUT || dc2h_in_sel >= _VD_OUTPUT){
		return dc2h_enrgb2yuv_state;
	}

	if(dc2h_cap_fmt_is_rgb_space(cap_fmt) == TRUE) // capture format is rgb space
	{
		if(dc2h_in_sel == _MAIN_UZU || dc2h_in_sel == _TWO_SECOND_UZU || dc2h_in_sel == _Sub_UZU_Output){
			dc2h_enrgb2yuv_state = DC2H_Colconv_YUVtoRGB; 
		}
		else if(dc2h_in_sel == _Idomain_Sdnr_input){
			if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == VODMA_COLOR_RGB) && (RGB2YUV_ICH1_RGB2YUV_CTRL_get_en_rgb2yuv(IoReg_Read32(RGB2YUV_ICH1_RGB2YUV_CTRL_reg)) == 0)){
                dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
            }else{
                dc2h_enrgb2yuv_state = DC2H_Colconv_YUVtoRGB;
            }
		}else{
			dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
		}
	}else{ // capture format is yuv space
		if(dc2h_in_sel == _MAIN_UZU || dc2h_in_sel == _TWO_SECOND_UZU || dc2h_in_sel == _Sub_UZU_Output){
			dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
		}else if(dc2h_in_sel == _Idomain_Sdnr_input){
			if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == VODMA_COLOR_RGB) && (RGB2YUV_ICH1_RGB2YUV_CTRL_get_en_rgb2yuv(IoReg_Read32(RGB2YUV_ICH1_RGB2YUV_CTRL_reg)) == 0)){
                dc2h_enrgb2yuv_state = DC2H_Colconv_RGBtoYUV;
            }else{
                dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
            }
		}else{
			dc2h_enrgb2yuv_state = DC2H_Colconv_RGBtoYUV;
		}
	}
    rtd_pr_vt_notice("cap_fmt %d, dc2h_in_sel=%d, dc2h_en_rgb2yuv=%d\n", cap_fmt, dc2h_in_sel, dc2h_enrgb2yuv_state);
    return dc2h_enrgb2yuv_state;
}

void set_dc2h_r2y_422to444_ctrl(unsigned int pxlfmt, unsigned int location)
{
	dc2h_r2y_dither_4xxto4xx_dc2h_422to444_ctrl_RBUS dc2h_r2y_dither_4xxto4xx_dc2h_422to444_ctrl_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_rgb2yuv_ctrl_RBUS dc2h_rgb2yuv_ctrl_reg;

	dc2h_r2y_dither_4xxto4xx_dc2h_422to444_ctrl_reg.regValue = IoReg_Read32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_422to444_CTRL_reg);
	// if dc2h input is yuv space && 422, capture format is rgb, set en_422to444
	if(location == _Idomain_Sdnr_input && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == VODMA_COLOR_YUV420 ||
	  Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == VODMA_COLOR_YUV422) && 
	  (RGB2YUV_ICH1_RGB2YUV_CTRL_get_en_rgb2yuv(IoReg_Read32(RGB2YUV_ICH1_RGB2YUV_CTRL_reg)) == 0) && 
	  dc2h_cap_fmt_is_rgb_space(pxlfmt) == TRUE){
		dc2h_r2y_dither_4xxto4xx_dc2h_422to444_ctrl_reg.en_422to444 = 1;
		if(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_get_sel_rgb(IoReg_Read32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg))){
			//Use limitation: when 422to444 enable, sel_RGB should disable in RGB2YUV(422to444 ONLY In yuv color_space)
			dc2h_rgb2yuv_ctrl_reg.regValue = IoReg_Read32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg);
			dc2h_rgb2yuv_ctrl_reg.sel_rgb = 0;
			IoReg_Write32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg, dc2h_rgb2yuv_ctrl_reg.regValue);
		}
	}else{
		dc2h_r2y_dither_4xxto4xx_dc2h_422to444_ctrl_reg.en_422to444 = 0;
	}

	IoReg_Write32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_422to444_CTRL_reg, dc2h_r2y_dither_4xxto4xx_dc2h_422to444_ctrl_reg.regValue);
}

void set_dc2h_y2r_dither_output_bit(unsigned int pxlfmt)
{
	dc2h_r2y_dither_4xxto4xx_dc2h_422to444_ctrl_RBUS dc2h_r2y_dither_4xxto4xx_dc2h_422to444_ctrl_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_dither_ctrl1_RBUS dc2h_r2y_dither_4xxto4xx_dc2h_dither_ctrl1_reg;

	dc2h_r2y_dither_4xxto4xx_dc2h_422to444_ctrl_reg.regValue = IoReg_Read32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_422to444_CTRL_reg);
	dc2h_r2y_dither_4xxto4xx_dc2h_dither_ctrl1_reg.regValue = IoReg_Read32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_DITHER_CTRL1_reg);
	
	if(dc2h_cap_fmt_is_10bit(pxlfmt) == TRUE){
		dc2h_r2y_dither_4xxto4xx_dc2h_422to444_ctrl_reg.dither_uzd_swap_en = 0; // dither->uzd
		dc2h_r2y_dither_4xxto4xx_dc2h_dither_ctrl1_reg.dither_bit_sel = 1; // dither output 10bit
	}else{
		dc2h_r2y_dither_4xxto4xx_dc2h_422to444_ctrl_reg.dither_uzd_swap_en = 1; // uzd->dither
		dc2h_r2y_dither_4xxto4xx_dc2h_dither_ctrl1_reg.dither_bit_sel = 0; // dither output 8bit
	}
	dc2h_r2y_dither_4xxto4xx_dc2h_dither_ctrl1_reg.dither_en = 1;
	dc2h_r2y_dither_4xxto4xx_dc2h_dither_ctrl1_reg.temporal_enable = 0;

	IoReg_Write32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_422to444_CTRL_reg, dc2h_r2y_dither_4xxto4xx_dc2h_422to444_ctrl_reg.regValue);
	IoReg_Write32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_DITHER_CTRL1_reg, dc2h_r2y_dither_4xxto4xx_dc2h_dither_ctrl1_reg.regValue);
}

void set_dc2h_r2y_dither_444to422_ctrl(unsigned int pxlfmt)
{
	dc2h_r2y_dither_4xxto4xx_dc2h_444to422_ctrl_RBUS dc2h_r2y_dither_4xxto4xx_dc2h_444to422_ctrl_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_444to422_ctrl_reg.regValue = IoReg_Read32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_444to422_CTRL_reg);

	if(dc2h_cap_pixel_encoding_is_444(pxlfmt) == TRUE){
		dc2h_r2y_dither_4xxto4xx_dc2h_444to422_ctrl_reg.en_444to422 = 0;
	}else{
		dc2h_r2y_dither_4xxto4xx_dc2h_444to422_ctrl_reg.en_444to422 = 1;
	}
	IoReg_Write32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_444to422_CTRL_reg, dc2h_r2y_dither_4xxto4xx_dc2h_444to422_ctrl_reg.regValue);
}

void drvif_set_dc2h_r2y_config(unsigned int pxlfmt, unsigned int location)
{
	unsigned short *table_index = 0;
	dc2h_r2y_dither_4xxto4xx_dc2h_rgb2yuv_ctrl_RBUS dc2h_rgb2yuv_ctrl_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_rgb2yuv_tab_m11_m12_RBUS dc2h_rgb2yuv_tab_m11_m12_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_rgb2yuv_tab_m13_m21_RBUS dc2h_rgb2yuv_tab_m13_m21_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_rgb2yuv_tab_m22_m23_RBUS dc2h_rgb2yuv_tab_m22_m23_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_rgb2yuv_tab_m31_m32_RBUS dc2h_rgb2yuv_tab_m31_m32_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_rgb2yuv_tab_m33_ygain_RBUS dc2h_rgb2yuv_tab_m33_ygain_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_rgb2yuv_tab_yo_RBUS dc2h_rgb2yuv_tab_yo_reg;

	if(dc2h_cap_fmt_is_rgb_space(pxlfmt)){ //y2r
		table_index = tYUV2RGB_COEF_709_RGB_0_255;
	}else{
		table_index = tYUV2RGB_COEF_709_RGB_16_235; //r2y
	}

	//main  all tab-1
	dc2h_rgb2yuv_tab_m11_m12_reg.m11 = table_index [_RGB2YUV_m11];
	dc2h_rgb2yuv_tab_m11_m12_reg.m12 = table_index [_RGB2YUV_m12];
	rtd_outl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_TAB_M11_M12_reg, dc2h_rgb2yuv_tab_m11_m12_reg.regValue);

	dc2h_rgb2yuv_tab_m13_m21_reg.m21 = table_index [_RGB2YUV_m21];
	dc2h_rgb2yuv_tab_m13_m21_reg.m13 = table_index [_RGB2YUV_m13];
	rtd_outl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_TAB_M13_M21_reg, dc2h_rgb2yuv_tab_m13_m21_reg.regValue);

	dc2h_rgb2yuv_tab_m22_m23_reg.m22 = table_index [_RGB2YUV_m22];
	dc2h_rgb2yuv_tab_m22_m23_reg.m23 = table_index [_RGB2YUV_m23];
	rtd_outl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_TAB_M22_M23_reg, dc2h_rgb2yuv_tab_m22_m23_reg.regValue);

	dc2h_rgb2yuv_tab_m31_m32_reg.m31 = table_index [_RGB2YUV_m31];
	dc2h_rgb2yuv_tab_m31_m32_reg.m32 = table_index [_RGB2YUV_m32];
	rtd_outl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_TAB_M31_M32_reg, dc2h_rgb2yuv_tab_m31_m32_reg.regValue);

	dc2h_rgb2yuv_tab_m33_ygain_reg.m33 = table_index [_RGB2YUV_m33];
	dc2h_rgb2yuv_tab_m33_ygain_reg.y_gain = table_index [_RGB2YUV_Y_gain];
	rtd_outl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_TAB_M33_YGain_reg, dc2h_rgb2yuv_tab_m33_ygain_reg.regValue);

	dc2h_rgb2yuv_tab_yo_reg.yo_even = table_index [_RGB2YUV_Yo_even];
	dc2h_rgb2yuv_tab_yo_reg.yo_odd = table_index [_RGB2YUV_Yo_odd];
	rtd_outl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_TAB_Yo_reg, dc2h_rgb2yuv_tab_yo_reg.regValue);

	dc2h_rgb2yuv_ctrl_reg.regValue = IoReg_Read32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg);
	dc2h_rgb2yuv_ctrl_reg.sel_rgb = table_index [_RGB2YUV_sel_RGB];
	dc2h_rgb2yuv_ctrl_reg.set_r_in_offset = table_index [_RGB2YUV_set_R_in_offset];
	dc2h_rgb2yuv_ctrl_reg.set_uv_out_offset = table_index [_RGB2YUV_set_UV_out_offset];
	dc2h_rgb2yuv_ctrl_reg.sel_uv_off = table_index [_RGB2YUV_sel_UV_off];
	dc2h_rgb2yuv_ctrl_reg.matrix_bypass = table_index [_RGB2YUV_Matrix_bypass];
	dc2h_rgb2yuv_ctrl_reg.sel_y_gain = table_index [_RGB2YUV_Enable_Y_gain];
	dc2h_rgb2yuv_ctrl_reg.en_rgb2yuv = set_dc2h_colconv_enable(pxlfmt);

	IoReg_Write32(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg, dc2h_rgb2yuv_ctrl_reg.regValue);

	set_dc2h_r2y_422to444_ctrl(pxlfmt, location);
	set_dc2h_y2r_dither_output_bit(pxlfmt);
	set_dc2h_r2y_dither_444to422_ctrl(pxlfmt);
}

void set_dc2h_uzd_coef_table_db_apply(unsigned int dc2h_num)
{
	if(dc2h_num == DC2H1){
		dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_RBUS dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_reg;
		dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_reg.regValue = IoReg_Read32(DC2H_SCALEDOWN_DC2H1_UZD_COEF_DB_CTRL_reg);
		dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_reg.coef_db_apply = 1;
		IoReg_Write32(DC2H_SCALEDOWN_DC2H1_UZD_COEF_DB_CTRL_reg, dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_reg.regValue);
	}else{
		dc2h2_scaledown_dc2h2_uzd_coef_db_ctrl_RBUS dc2h2_scaledown_dc2h2_uzd_coef_db_ctrl_reg;
		dc2h2_scaledown_dc2h2_uzd_coef_db_ctrl_reg.regValue = IoReg_Read32(DC2H2_SCALEDOWN_DC2H2_UZD_COEF_DB_CTRL_reg);
		dc2h2_scaledown_dc2h2_uzd_coef_db_ctrl_reg.coef_db_apply = 1;
		IoReg_Write32(DC2H2_SCALEDOWN_DC2H2_UZD_COEF_DB_CTRL_reg, dc2h2_scaledown_dc2h2_uzd_coef_db_ctrl_reg.regValue);
	}
}

unsigned int get_dc2h_uzd_htotal_by_in_sel(unsigned int location)
{
	unsigned int uzd_htotal = 0;
	switch(location)
	{
		case _MAIN_UZU:
		case _TWO_SECOND_UZU:
		case _Sub_UZU_Output:
			uzd_htotal = PPOVERLAY_uzudtg_DH_TOTAL_get_uzudtg_dh_total(IoReg_Read32(PPOVERLAY_uzudtg_DH_TOTAL_reg)) + 1;
			break;
		case _MAIN_YUV2RGB:
		case _Memc_Mux_Input:
		case _Memc_mux_Output:
		case _Dither_Output:
			uzd_htotal = PPOVERLAY_DH_Total_Last_Line_Length_get_dh_total(IoReg_Read32(PPOVERLAY_DH_Total_Last_Line_Length_reg)) + 1;
			break;
		case _OSD123_MIXER_Output:
			uzd_htotal = PPOVERLAY_osddtg_DH_TOTAL_get_osddtg_dh_total(IoReg_Read32(PPOVERLAY_osddtg_DH_TOTAL_reg)) + 1;
			break;
		case _OSD4_Output:
			uzd_htotal = PPOVERLAY_osd4dtg_DH_TOTAL_get_osd4dtg_dh_total(IoReg_Read32(PPOVERLAY_osd4dtg_DH_TOTAL_reg)) + 1;
			break;
		case _Idomain_Sdnr_input:
			uzd_htotal = VGIP_ICH1_VGIP_HTOTAL_get_ch1_htotal_num(IoReg_Read32(VGIP_ICH1_VGIP_HTOTAL_reg)) + 1;
			break;
		default:
			rtd_pr_vt_notice("func %s, invalid location %d\n", __FUNCTION__, location);
	}
	rtd_pr_vt_notice("dc2h uzd htotal %d\n", uzd_htotal);
	return uzd_htotal;
}

void set_dc2h_uzd_scale_down_coef_tab(KADP_VT_RECT_T in_size, KADP_VT_RECT_T out_size)
{
	unsigned int SDRatio;
	unsigned int SDFilter=0;
	unsigned int tmp_data;
	short *coef_pt;
	unsigned int i;
	//unsigned long long tmp = 0;
	unsigned int nFactor;
	unsigned int D = 0;
	unsigned char Hini, Vini, a;
	unsigned short S1,S2;

	dc2h_scaledown_dc2h1_uzd_ctrl0_RBUS dc2h_uzd_ctrl0_reg;
	dc2h_scaledown_dc2h1_uzd_ctrl1_RBUS dc2h_uzd_ctrl1_reg;

	dc2h_scaledown_dc2h1_uzd_scale_hor_factor_RBUS dc2h_uzd_hor_factor_reg;
	dc2h_scaledown_dc2h1_uzd_scale_ver_factor_RBUS dc2h_uzd_ver_factor_reg;

	dc2h_scaledown_dc2h1_uzd_initial_value_RBUS dc2h_uzd_initial_value_reg;
	dc2h_scaledown_dc2h1_uzd_initial_int_value_RBUS dc2h_uzd_initial_int_value_reg;

	dc2h_uzd_ctrl0_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_CTRL0_reg);
	dc2h_uzd_ctrl1_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_CTRL1_reg);

	dc2h_uzd_hor_factor_reg.regValue  = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_SCALE_HOR_FACTOR_reg);
	dc2h_uzd_ver_factor_reg.regValue  = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_SCALE_VER_FACTOR_reg);

	dc2h_uzd_initial_value_reg.regValue  = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_INITIAL_VALUE_reg);
	dc2h_uzd_initial_int_value_reg.regValue  = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_INITIAL_INT_VALUE_reg);

	//o============ H scale-down=============o
	if (in_size.w > out_size.w)
	{
		// o-------calculate scaledown ratio to select one of different bandwith filters.--------o
		if ( out_size.w == 0) {
			rtd_pr_vt_debug("output width = 0 !!!\n");
			SDRatio = 0;
		} else {
			SDRatio = (in_size.w * TMPMUL) / out_size.w;
		}

		if(SDRatio <= ((TMPMUL*3)/2))  //<1.5 sharp, wider bw
			SDFilter = 2;
		else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) )  // Mid
			SDFilter = 1;
		else    // blurest, narrow bw
			SDFilter = 0;

		//o---------------- fill coefficients into access port--------------------o
		coef_pt = tScaleDown_COEF_TAB[SDFilter];

		dc2h_uzd_ctrl0_reg.h_y_table_sel = 0;  // TAB1
		dc2h_uzd_ctrl0_reg.h_c_table_sel = 0;  // TAB1

		for (i=0; i<16; i++)
		{
			tmp_data = ((unsigned int)(*coef_pt++)<<16);
			tmp_data += (unsigned int)(*coef_pt++);
			rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_FIR_COEF_TAB1_C0_reg + i*4, tmp_data);
		}
	}

	//o============ V scale-down=============o
	if (in_size.h > out_size.h)
	{
		// o-------calculate scaledown ratio to select one of different bandwith filters.--------o
		if ( out_size.h == 0 ) {
			SDRatio = 0;
		} else {
			SDRatio = (in_size.h * TMPMUL) /out_size.h;
		}	

		if(SDRatio <= ((TMPMUL*3)/2))  //<1.5 sharp, wider bw
			SDFilter = 2;
		else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) )  // Mid
			SDFilter = 1;
		else    // blurest, narrow bw
			SDFilter = 0;

		rtd_pr_vt_debug("filter number=%d\n",SDFilter);

		//o---------------- fill coefficients into access port--------------------o
		 coef_pt = tScaleDown_COEF_TAB[SDFilter];

		dc2h_uzd_ctrl0_reg.v_y_table_sel = 1;  // TAB2
		dc2h_uzd_ctrl0_reg.v_c_table_sel = 1;  // TAB2

		for (i=0; i<16; i++)
		{
			tmp_data = ((unsigned int)(*coef_pt++)<<16);
			tmp_data += (unsigned int)(*coef_pt++);
			rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_FIR_COEF_TAB2_C0_reg + i*4, tmp_data);
		}
	} else {
		//no need  V scale-down, use bypass filter
	}

	set_dc2h_uzd_coef_table_db_apply(DC2H1);
	// Decide Output data format for scale down
	//============H Scale-down============
	if (in_size.w > out_size.w) // H scale-down
	{
		Hini = 0x80;
		dc2h_uzd_initial_value_reg.hor_ini = Hini;
		dc2h_uzd_initial_int_value_reg.hor_ini_int = 1;
		a = 5;

		if(0/*panorama*/)
		{// CSW+ 0960830 Non-linear SD
			S1 = (out_size.w) >> 2;
			S2 = (out_size.w) - S1*2;
			//=vv==old from TONY, if no problem, use this=vv==
			nFactor = (unsigned int)(((in_size.w)<<21) - ((unsigned int)Hini<<13));
			//nFactor = ((((unsigned long long)(ptInSize->nWidth))*(ptOutSize->nWidth-1))<<21) / ptOutSize->nWidth;
			//===================
			nFactor = nFactor /((2*a+1)*S1 + (a+1)*S2 - a) * a;
			if(S1 != 0)
				D = nFactor / a / S1;
			nFactor = SHR((nFactor + 1), 1); //rounding
			D = SHR(D + 1, 1); //rounding
			rtd_pr_vt_debug("\nPANORAMA2 TEST 111\n");
			rtd_pr_vt_debug("nPANORAMA2 Factor = %d\n",nFactor);
			rtd_pr_vt_debug("PANORAMA2 S1=%d, S2=%d, D = %d\n",S1, S2, D);

			if(nFactor < 1048576) {
				rtd_pr_vt_debug("PANORAMA2 Can't do nonlinear SD \n");
				//nFactor = (unsigned int)(((ptInSize->nWidth)<<21) - ((unsigned int)Hini<<13)) / (ptOutSize->nWidth-1);
				if(out_size.w != 0)
					nFactor = (unsigned int)(((in_size.w)<<21)) / (out_size.w);
				nFactor = SHR(nFactor + 1, 1); //rounding
				rtd_pr_vt_debug("PANORAMA2 nFactor=%x, input Wid=%d, Out Wid=%d\n",nFactor, in_size.w, out_size.w);
				S1 = 0;
				S2 = out_size.w;
				D = 0;
			}
        }else{
			if(out_size.w == 0 ){
				rtd_pr_vt_debug("output width = 0 !!!\n");
				nFactor = 0;
			}else if(in_size.w > 4095){
				//nFactor = (unsigned int)((((in_size.w)<<19) / (out_size.w))<<1);
				nFactor = (unsigned int)(in_size.w * 524288) / (out_size.w) * 2;
			}else if(in_size.w > 2047){
				nFactor = (unsigned int)(in_size.w * 1048576) / (out_size.w); // 2^20 = 1048576
			}else{
				//nFactor = (unsigned int)((in_size.w<<21)) / (out_size.w);
				nFactor = (unsigned int)(in_size.w * 2097152) / (out_size.w);
				nFactor = SHR(nFactor + 1, 1); //rounding
			}
			
			/*else{
				tmp = ((unsigned long long)(in_size.w) << 20);
				do_div(tmp, (out_size.w));
				//nFactor = SHR(nFactor + 1, 1); //rounding
				nFactor = tmp;
			}*/
		}
	}else {
		nFactor = 0x100000;
	}

	dc2h_uzd_hor_factor_reg.hor_fac = nFactor;
	rtd_pr_vt_notice("hfactor=%d, in_wid=%d, out_wid=%d\n",nFactor, in_size.w, out_size.w);

	//================V Scale-down=================
	if (in_size.h > out_size.h) {    // V scale-down
		Vini = 0x80;//0xff;
		dc2h_uzd_initial_value_reg.ver_ini = Vini;
		dc2h_uzd_initial_int_value_reg.ver_ini_int = 1;//0;

		if ( out_size.h == 0 ) {
			rtd_pr_vt_debug("output length = 0 !!!\n");
			nFactor = 0;
		} else {
			nFactor = (unsigned int)((in_size.h * 1024 * 1024)) / (out_size.h);
			// tmp = ((unsigned long long)(in_size.h) << 20);
			// do_div(tmp, (out_size.h));
			// nFactor = tmp;
		}
	} else {
		nFactor = 0x100000;
	}

	dc2h_uzd_ver_factor_reg.ver_fac = nFactor;
	rtd_pr_vt_notice("vfactor=%d,in_h=%d, out_h=%d\n", nFactor, in_size.h, out_size.h);

	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_CTRL0_reg, dc2h_uzd_ctrl0_reg.regValue);
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_CTRL1_reg, dc2h_uzd_ctrl1_reg.regValue);

	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_SCALE_HOR_FACTOR_reg, dc2h_uzd_hor_factor_reg.regValue);
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_SCALE_VER_FACTOR_reg, dc2h_uzd_ver_factor_reg.regValue);

	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_INITIAL_VALUE_reg, dc2h_uzd_initial_value_reg.regValue);
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_INITIAL_INT_VALUE_reg, dc2h_uzd_initial_int_value_reg.regValue);
}

void drvif_set_dc2h_scale_down_config(KADP_VT_RECT_T in_size, KADP_VT_RECT_T out_size)
{
	dc2h_scaledown_dc2h1_uzd_ctrl0_RBUS dc2h_uzd_ctrl0_reg;
	dc2h_scaledown_dc2h1_uzd_ctrl1_RBUS dc2h_uzd_ctrl1_reg;
	dc2h_scaledown_dc2h1_uzd_444to422_ctrl_RBUS dc2h_uzd_444to422_ctrl_reg;
	dc2h_scaledown_dc2h1_uzd_ibuff_ctrl_RBUS dc2h_uzd_ibuff_ctrl_reg;

	dc2h_uzd_444to422_ctrl_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_444to422_CTRL_reg);
	dc2h_uzd_444to422_ctrl_reg.cbcr_swap_en = 0;
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_444to422_CTRL_reg, dc2h_uzd_444to422_ctrl_reg.regValue);

	dc2h_uzd_ctrl1_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_CTRL1_reg);
	dc2h_uzd_ctrl1_reg.uzd_htotal = get_dc2h_uzd_htotal_by_in_sel(curCapInfo[DC2H1].cap_location);
	dc2h_uzd_ctrl1_reg.auto_four_tap_en = 1;
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_CTRL1_reg, dc2h_uzd_ctrl1_reg.regValue);

	dc2h_uzd_ibuff_ctrl_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_IBUFF_CTRL_reg);
	dc2h_uzd_ibuff_ctrl_reg.ibuf_h_size = out_size.w;
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_IBUFF_CTRL_reg, dc2h_uzd_ibuff_ctrl_reg.regValue);

	dc2h_uzd_ctrl0_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_CTRL0_reg);
	dc2h_uzd_ctrl0_reg.h_zoom_en = (in_size.w > out_size.w);
	dc2h_uzd_ctrl0_reg.v_zoom_en = (in_size.h > out_size.h);

	dc2h_uzd_ctrl0_reg.out_bit = 1; //uzd output always 10bit
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_CTRL0_reg, dc2h_uzd_ctrl0_reg.regValue);

	set_dc2h_uzd_scale_down_coef_tab(in_size, out_size);
}

void set_dc2h_sequence_cap(VT_CUR_CAPTURE_INFO *dc2h_ctrl_info)
{
	int cap0_total_bit = 0;
	int cap0_burst_num = 0;
	int bitperpixel = 0;

	unsigned int dc2h_cap_mode = get_dc2h_dma_cap_mode(DC2H1);
	dc2h_cap_dc2h_cap0_dma_wr_ctrl_RBUS dc2h_cap0_dma_wr_ctrl_reg;
    dc2h_cap_dc2h_cap_size_RBUS dc2h_cap_size_reg;//w,h
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	// dc2h_cap_dc2h_interrupt_enable_RBUS dc2h_interrupt_enable_reg;

	if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB) {
		bitperpixel = 24;
	}else{
		bitperpixel = 32;
	}

	dc2h_ctrl_info->output_size.w  = (dc2h_ctrl_info->output_size.w+1)/2*2;
	rtd_pr_vt_notice("cap width align to even %d\n", dc2h_ctrl_info->output_size.w);

	if(dc2h_cap_mode == DC2H_CAP_FRAME_MODE){
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * dc2h_ctrl_info->output_size.h;
	}else{
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
	}

	cap0_burst_num = ((cap0_total_bit + 127) / 128);
	
	dc2h_cap_size_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Size_reg);
	dc2h_cap_size_reg.dc2h_dma_hact = dc2h_ctrl_info->output_size.w; //should be even
    dc2h_cap_size_reg.dc2h_dma_vact = dc2h_ctrl_info->output_size.h;
    rtd_outl(DC2H_CAP_DC2H_CAP_Size_reg, dc2h_cap_size_reg.regValue);
	
    dc2h_cap0_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP0_DMA_WR_Ctrl_reg);
	dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_1byte_swap = 0;
	dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_2byte_swap = 0;
	dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_4byte_swap = 0;
	dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_8byte_swap = 0;

	if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB){
		set_dc2h_cap_color_fmt_rgb(dc2h_cap_mode);
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_1byte_swap = 1;
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_2byte_swap = 1;
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_4byte_swap = 1;
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_8byte_swap = 1;
	}else if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ARGB){
		set_dc2h_cap_color_fmt_argb(CAP_ARGB, dc2h_cap_mode);
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_8byte_swap = 1;
	}else if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGBA){
		set_dc2h_cap_color_fmt_argb(CAP_RGBA, dc2h_cap_mode);
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_8byte_swap = 1;
	}else{
		rtd_pr_vt_notice("[VT]unspport capture RAGB,RGAB\n");
		set_dc2h_cap_color_fmt_argb(CAP_ARGB, dc2h_cap_mode);
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_8byte_swap = 1;
	}

	rtd_outl(DC2H_CAP_DC2H_CAP0_DMA_WR_Ctrl_reg, dc2h_cap0_dma_wr_ctrl_reg.regValue);

	set_dc2h_cap0_line_burst_num(cap0_burst_num, dc2h_ctrl_info->output_size.h, dc2h_cap_mode);

	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_cap0_en = 1;
	dc2h_cap_color_format_reg.dc2h_cap1_en = 0;
	dc2h_cap_color_format_reg.dc2h_cap2_en = 0;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);

	/*if (get_vt_VtSwBufferMode()) {
		rtd_outl(DC2H_CAP_DC2H_Interrupt_Enable_reg, dc2h_interrupt_enable_reg.regValue);
	}*/
}

void set_dc2h_nv12_cap(VT_CUR_CAPTURE_INFO *dc2h_ctrl_info)
{
	int cap0_total_bit = 0;
	int cap1_total_bit = 0;
	int cap0_burst_num = 0;
	int cap1_burst_num = 0;

	int bitperpixel = 0;
	VT_CAP_BIT_SEL bit_sel = PER_CHANNEL_8_BIT;

	int odd_line = dc2h_ctrl_info->output_size.h % 2;
	unsigned int dc2h_cap_mode = get_dc2h_dma_cap_mode(DC2H1);

    dc2h_cap_dc2h_cap0_dma_wr_ctrl_RBUS dc2h_cap0_dma_wr_ctrl_reg;
	dc2h_cap_dc2h_cap1_dma_wr_ctrl_RBUS dc2h_cap1_dma_wr_ctrl_reg;

    dc2h_cap_dc2h_cap_size_RBUS dc2h_cap_size_reg;//w,h
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	// dc2h_cap_dc2h_interrupt_enable_RBUS dc2h_interrupt_enable_reg;

	dc2h_ctrl_info->output_size.w  = (dc2h_ctrl_info->output_size.w+1)/2*2;
	rtd_pr_vt_notice("[VT]cap width align to even %d\n", dc2h_ctrl_info->output_size.w);

	if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit || dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV21){
		bitperpixel = 8;
		bit_sel = PER_CHANNEL_8_BIT;
	}else{
		bitperpixel = 10;
		bit_sel = PER_CHANNEL_10_BIT;
	}

	if(dc2h_cap_mode == DC2H_CAP_FRAME_MODE){
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * dc2h_ctrl_info->output_size.h;
		cap1_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * (dc2h_ctrl_info->output_size.h / 2 + odd_line);
	}else{
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
		cap1_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
	}

	cap0_burst_num = ((cap0_total_bit + 127) / 128);
	cap1_burst_num = ((cap1_total_bit + 127) / 128);
	
	dc2h_cap_size_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Size_reg);
	dc2h_cap_size_reg.dc2h_dma_hact = dc2h_ctrl_info->output_size.w;
    dc2h_cap_size_reg.dc2h_dma_vact = dc2h_ctrl_info->output_size.h;
    rtd_outl(DC2H_CAP_DC2H_CAP_Size_reg, dc2h_cap_size_reg.regValue);

    dc2h_cap0_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP0_DMA_WR_Ctrl_reg);
	dc2h_cap1_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP1_DMA_WR_Ctrl_reg);

	if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit || dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_10bit){
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_1byte_swap = 1;
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_2byte_swap = 1;
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_4byte_swap = 1;
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_8byte_swap = 1;

		dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_1byte_swap = 1; //UV
		dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_2byte_swap = 1;
		dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_4byte_swap = 1;
		dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_8byte_swap = 1;
		set_dc2h_cap_color_fmt_nv12(bit_sel, dc2h_cap_mode, DC2H_CB_CR_NO_SWAP);
	}else if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV21){
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_1byte_swap = 1;
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_2byte_swap = 1;
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_4byte_swap = 1;
		dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_8byte_swap = 1;

		dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_1byte_swap = 1;//1;  //vU
		dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_2byte_swap = 1;
		dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_4byte_swap = 1;
		dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_8byte_swap = 1;
		set_dc2h_cap_color_fmt_nv12(bit_sel, dc2h_cap_mode, DC2H_CB_CR_SWAP);
	}
    rtd_outl(DC2H_CAP_DC2H_CAP0_DMA_WR_Ctrl_reg, dc2h_cap0_dma_wr_ctrl_reg.regValue);
	rtd_outl(DC2H_CAP_DC2H_CAP1_DMA_WR_Ctrl_reg, dc2h_cap1_dma_wr_ctrl_reg.regValue);

	set_dc2h_cap0_line_burst_num(cap0_burst_num, dc2h_ctrl_info->output_size.h, dc2h_cap_mode);
	set_dc2h_cap1_line_burst_num(cap1_burst_num, dc2h_ctrl_info->output_size.h / 2, dc2h_cap_mode);

	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_cap0_en = 1; //y
	dc2h_cap_color_format_reg.dc2h_cap1_en = 1; //UV
	dc2h_cap_color_format_reg.dc2h_cap2_en = 0;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);
}

void set_dc2h_p010_cap(VT_CUR_CAPTURE_INFO *dc2h_ctrl_info)
{
	int cap0_total_bit = 0;
	int cap1_total_bit = 0;
	int cap0_burst_num = 0;
	int cap1_burst_num = 0;

	int bitperpixel = 16;

	int odd_line = dc2h_ctrl_info->output_size.h % 2;
	unsigned int dc2h_cap_mode = get_dc2h_dma_cap_mode(DC2H1);

    dc2h_cap_dc2h_cap0_dma_wr_ctrl_RBUS dc2h_cap0_dma_wr_ctrl_reg;
	dc2h_cap_dc2h_cap1_dma_wr_ctrl_RBUS dc2h_cap1_dma_wr_ctrl_reg;

    dc2h_cap_dc2h_cap_size_RBUS dc2h_cap_size_reg;//w,h
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	// dc2h_cap_dc2h_interrupt_enable_RBUS dc2h_interrupt_enable_reg;

	dc2h_ctrl_info->output_size.w  = (dc2h_ctrl_info->output_size.w+1)/2*2;
	rtd_pr_vt_notice("[VT]cap width align to even %d\n", dc2h_ctrl_info->output_size.w);

	if(dc2h_cap_mode == DC2H_CAP_FRAME_MODE){
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * dc2h_ctrl_info->output_size.h;
		cap1_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * (dc2h_ctrl_info->output_size.h / 2 + odd_line);
	}else{
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
		cap1_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
	}

	cap0_burst_num = ((cap0_total_bit + 127) / 128);
	cap1_burst_num = ((cap1_total_bit + 127) / 128);
	
	dc2h_cap_size_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Size_reg);
	dc2h_cap_size_reg.dc2h_dma_hact = dc2h_ctrl_info->output_size.w;
    dc2h_cap_size_reg.dc2h_dma_vact = dc2h_ctrl_info->output_size.h;
    rtd_outl(DC2H_CAP_DC2H_CAP_Size_reg, dc2h_cap_size_reg.regValue);

    dc2h_cap0_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP0_DMA_WR_Ctrl_reg);
	dc2h_cap1_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP1_DMA_WR_Ctrl_reg);

	dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_1byte_swap = 1;
	dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_2byte_swap = 1;
	dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_4byte_swap = 1;
	dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_8byte_swap = 1;

	dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_1byte_swap = 1; //VU
	dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_2byte_swap = 1;
	dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_4byte_swap = 1;
	dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_8byte_swap = 1;
	
    rtd_outl(DC2H_CAP_DC2H_CAP0_DMA_WR_Ctrl_reg, dc2h_cap0_dma_wr_ctrl_reg.regValue);
	rtd_outl(DC2H_CAP_DC2H_CAP1_DMA_WR_Ctrl_reg, dc2h_cap1_dma_wr_ctrl_reg.regValue);

	set_dc2h_cap_color_fmt_p010(dc2h_cap_mode);

	set_dc2h_cap0_line_burst_num(cap0_burst_num, dc2h_ctrl_info->output_size.h, dc2h_cap_mode);
	set_dc2h_cap1_line_burst_num(cap1_burst_num, dc2h_ctrl_info->output_size.h / 2, dc2h_cap_mode);

	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_cap0_en = 1;
	dc2h_cap_color_format_reg.dc2h_cap1_en = 1;
	dc2h_cap_color_format_reg.dc2h_cap2_en = 0;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);
}

void set_dc2h_yv12_i420_cap(VT_CUR_CAPTURE_INFO *dc2h_ctrl_info)
{
	int cap0_total_bit = 0;
	int cap1_total_bit = 0;
	int cap2_total_bit = 0;
	int cap0_burst_num = 0;
	int cap1_burst_num = 0;
	int cap2_burst_num = 0;

	int bitperpixel = 8;
	int odd_line = dc2h_ctrl_info->output_size.h % 2;

	unsigned int dc2h_cap_mode = get_dc2h_dma_cap_mode(DC2H1);

    dc2h_cap_dc2h_cap0_dma_wr_ctrl_RBUS dc2h_cap0_dma_wr_ctrl_reg;
	dc2h_cap_dc2h_cap1_dma_wr_ctrl_RBUS dc2h_cap1_dma_wr_ctrl_reg;
	dc2h_cap_dc2h_cap2_dma_wr_ctrl_RBUS dc2h_cap2_dma_wr_ctrl_reg;

    dc2h_cap_dc2h_cap_size_RBUS dc2h_cap_size_reg;//w,h
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	// dc2h_cap_dc2h_interrupt_enable_RBUS dc2h_interrupt_enable_reg;

	dc2h_ctrl_info->output_size.w  = (dc2h_ctrl_info->output_size.w+1)/2*2;
	rtd_pr_vt_notice("[VT]cap width align to even %d\n", dc2h_ctrl_info->output_size.w);

	if(dc2h_cap_mode == DC2H_CAP_FRAME_MODE){
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * dc2h_ctrl_info->output_size.h;
		cap1_total_bit = bitperpixel * (dc2h_ctrl_info->output_size.w >> 1) * (dc2h_ctrl_info->output_size.h / 2 + odd_line);
		cap2_total_bit = bitperpixel * (dc2h_ctrl_info->output_size.w >> 1) * (dc2h_ctrl_info->output_size.h / 2 + odd_line);
	}else{
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
		cap1_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w / 2;
		cap2_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w / 2;
	}

	cap0_burst_num = ((cap0_total_bit + 127) / 128);
	cap1_burst_num = ((cap1_total_bit + 127) / 128);
	cap2_burst_num = ((cap1_total_bit + 127) / 128);
	
	dc2h_cap_size_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Size_reg);
	dc2h_cap_size_reg.dc2h_dma_hact = dc2h_ctrl_info->output_size.w;
    dc2h_cap_size_reg.dc2h_dma_vact = dc2h_ctrl_info->output_size.h;
    rtd_outl(DC2H_CAP_DC2H_CAP_Size_reg, dc2h_cap_size_reg.regValue);

    dc2h_cap0_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP0_DMA_WR_Ctrl_reg);
	dc2h_cap1_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP1_DMA_WR_Ctrl_reg);
	dc2h_cap2_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP2_DMA_WR_Ctrl_reg);

	dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_1byte_swap = 1; //Y
	dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_2byte_swap = 1;
	dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_4byte_swap = 1;
	dc2h_cap0_dma_wr_ctrl_reg.dc2h_cap0_dma_8byte_swap = 1;

	dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_1byte_swap = 1; //u
	dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_2byte_swap = 1;
	dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_4byte_swap = 1;
	dc2h_cap1_dma_wr_ctrl_reg.dc2h_cap1_dma_8byte_swap = 1;

	dc2h_cap2_dma_wr_ctrl_reg.dc2h_cap2_dma_1byte_swap = 1; //v
	dc2h_cap2_dma_wr_ctrl_reg.dc2h_cap2_dma_2byte_swap = 1;
	dc2h_cap2_dma_wr_ctrl_reg.dc2h_cap2_dma_4byte_swap = 1;
	dc2h_cap2_dma_wr_ctrl_reg.dc2h_cap2_dma_8byte_swap = 1;
	
    rtd_outl(DC2H_CAP_DC2H_CAP0_DMA_WR_Ctrl_reg, dc2h_cap0_dma_wr_ctrl_reg.regValue);
	rtd_outl(DC2H_CAP_DC2H_CAP1_DMA_WR_Ctrl_reg, dc2h_cap1_dma_wr_ctrl_reg.regValue);
	rtd_outl(DC2H_CAP_DC2H_CAP2_DMA_WR_Ctrl_reg, dc2h_cap2_dma_wr_ctrl_reg.regValue);
	
	if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YV12){
		set_dc2h_cap_color_fmt_yv12(dc2h_cap_mode);
	}else{
		set_dc2h_cap_color_fmt_i420(dc2h_cap_mode);
	}

	set_dc2h_cap0_line_burst_num(cap0_burst_num, dc2h_ctrl_info->output_size.h, dc2h_cap_mode);
	set_dc2h_cap1_line_burst_num(cap1_burst_num, dc2h_ctrl_info->output_size.h / 2, dc2h_cap_mode);
	set_dc2h_cap2_line_burst_num(cap2_burst_num, dc2h_ctrl_info->output_size.h / 2, dc2h_cap_mode);

	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_cap0_en = 1;
	dc2h_cap_color_format_reg.dc2h_cap1_en = 1;
	dc2h_cap_color_format_reg.dc2h_cap2_en = 1;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);
}

int get_dc2h_cap_fmt_plane_number(unsigned int cap_fmt)
{
	int plane_number = 0;
	if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB && cap_fmt <= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ABGR){
		plane_number = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED;
	}else if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit && cap_fmt <= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV21){
		plane_number = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR;
	}else if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YV12 && cap_fmt < V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_MAXN){
		plane_number = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_PLANAR;
	}else{
		plane_number = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED;
	}

	return plane_number;
}

void set_dc2h_cap_buf_property(VT_CUR_CAPTURE_INFO *dc2h_ctrl_info)
{
	CapBuffer_VT[DC2H1].pixelFormat = dc2h_ctrl_info->fmt;
	CapBuffer_VT[DC2H1].plane_number = get_dc2h_cap_fmt_plane_number(dc2h_ctrl_info->fmt);
	CapBuffer_VT[DC2H1].vtbuffernumber = get_vt_VtBufferNum(DC2H1);
	CapBuffer_VT[DC2H1].stride = drvif_memory_get_data_align(dc2h_ctrl_info->output_size.w, 16);
	CapBuffer_VT[DC2H1].width = dc2h_ctrl_info->output_size.w;
	CapBuffer_VT[DC2H1].height = dc2h_ctrl_info->output_size.h;
}

void disable_dc2h_inten_and_dmaen(void)
{
	dc2h_cap_dc2h_interrupt_enable_RBUS dc2h_interrupt_enable_reg;
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	if (get_vt_VtBufferNum(DC2H1) > 1){
		dc2h_interrupt_enable_reg.regValue = rtd_inl(DC2H_CAP_DC2H_Interrupt_Enable_reg);
		dc2h_interrupt_enable_reg.dc2h_vs_rising_int_en = 0;
		dc2h_interrupt_enable_reg.dc2h_cap0_done_int_en = 0;
		dc2h_interrupt_enable_reg.dc2h_cap1_done_int_en = 0;
		dc2h_interrupt_enable_reg.dc2h_cap2_done_int_en = 0;
		rtd_outl(DC2H_CAP_DC2H_Interrupt_Enable_reg, dc2h_interrupt_enable_reg.regValue);
		msleep(20);
	}
	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_cap0_en = 0;
	dc2h_cap_color_format_reg.dc2h_cap1_en = 0;
	dc2h_cap_color_format_reg.dc2h_cap2_en = 0;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);
}

void disable_dc2h_capture(void)
{
	dc2h_cap_dc2h_ctrl_RBUS dc2h_cap_ctrl_reg;
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
	dc2h_cap_dc2h_interrupt_enable_RBUS dc2h_interrupt_enable_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_rgb2yuv_ctrl_RBUS dc2h_rgb2yuv_ctrl_reg;
	dc2h_r2y_dither_4xxto4xx_dc2h_pq_db_ctrl_RBUS dc2h_r2y_dither_4xxto4xx_dc2h_pq_db_ctrl_reg;
	dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_RBUS dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_reg;
	dc2h_scaledown_dc2h1_uzd_h_db_ctrl_RBUS dc2h_scaledown_dc2h1_uzd_h_db_ctrl_reg;
	dc2h_scaledown_dc2h1_uzd_v_db_ctrl_RBUS dc2h_scaledown_dc2h1_uzd_v_db_ctrl_reg;
	dc2h_scaledown_dc2h1_uzd_ctrl0_RBUS dc2h_scaledown_dc2h1_uzd_ctrl0_reg;
	dc2h_cap_dc2h_3dmaskto2d_ctrl_RBUS dc2h_cap_3dmaskto2d_ctrl_reg;

	if (get_vt_VtBufferNum(DC2H1) > 1){
		dc2h_interrupt_enable_reg.regValue = rtd_inl(DC2H_CAP_DC2H_Interrupt_Enable_reg);
		dc2h_interrupt_enable_reg.dc2h_vs_rising_int_en = 0;
		dc2h_interrupt_enable_reg.dc2h_cap0_done_int_en = 0;
		dc2h_interrupt_enable_reg.dc2h_cap1_done_int_en = 0;
		dc2h_interrupt_enable_reg.dc2h_cap2_done_int_en = 0;
		rtd_outl(DC2H_CAP_DC2H_Interrupt_Enable_reg, dc2h_interrupt_enable_reg.regValue);
		msleep(20);
	}

	dc2h_r2y_dither_4xxto4xx_dc2h_pq_db_ctrl_reg.regValue = rtd_inl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_PQ_DB_CTRL_reg);
	dc2h_r2y_dither_4xxto4xx_dc2h_pq_db_ctrl_reg.db_en = 0;
	rtd_outl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_PQ_DB_CTRL_reg, dc2h_r2y_dither_4xxto4xx_dc2h_pq_db_ctrl_reg.regValue);

	dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_COEF_DB_CTRL_reg);
	dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_reg.coef_db_en = 0;
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_COEF_DB_CTRL_reg, dc2h_scaledown_dc2h1_uzd_coef_db_ctrl_reg.regValue);

	dc2h_scaledown_dc2h1_uzd_h_db_ctrl_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_H_DB_CTRL_reg);
	dc2h_scaledown_dc2h1_uzd_h_db_ctrl_reg.h_db_en = 0;
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_H_DB_CTRL_reg, dc2h_scaledown_dc2h1_uzd_h_db_ctrl_reg.regValue);

	dc2h_scaledown_dc2h1_uzd_v_db_ctrl_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_V_DB_CTRL_reg);
	dc2h_scaledown_dc2h1_uzd_v_db_ctrl_reg.v_db_en = 0;
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_V_DB_CTRL_reg, dc2h_scaledown_dc2h1_uzd_v_db_ctrl_reg.regValue);

	IoReg_ClearBits(DC2H_CAP_DC2H_CAP_doublebuffer_reg, DC2H_CAP_DC2H_CAP_doublebuffer_dc2h_db_en_mask);
	IoReg_ClearBits(DC2H_CAP_DC2H_3DMaskTo2D_doublebuffer_reg, DC2H_CAP_DC2H_3DMaskTo2D_doublebuffer_dc2h_3dmaskto2d_db_en_mask);

	dc2h_cap_3dmaskto2d_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_3DMaskTo2D_Ctrl_reg);
	dc2h_cap_3dmaskto2d_ctrl_reg.dc2h_3dmaskto2d_en = 0;
	rtd_outl(DC2H_CAP_DC2H_3DMaskTo2D_Ctrl_reg, dc2h_cap_3dmaskto2d_ctrl_reg.regValue);

	dc2h_cap_color_format_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CAP_Color_format_reg);
	dc2h_cap_color_format_reg.dc2h_cap0_en = 0;
	dc2h_cap_color_format_reg.dc2h_cap1_en = 0;
	dc2h_cap_color_format_reg.dc2h_cap2_en = 0;
	rtd_outl(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);

	dc2h_cap_ctrl_reg.regValue = rtd_inl(DC2H_CAP_DC2H_CTRL_reg);
	dc2h_cap_ctrl_reg.dc2h_in_sel = _NO_INPUT;
	rtd_outl(DC2H_CAP_DC2H_CTRL_reg, dc2h_cap_ctrl_reg.regValue);

	dc2h_rgb2yuv_ctrl_reg.regValue = rtd_inl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg);
	dc2h_rgb2yuv_ctrl_reg.en_rgb2yuv = 0;
	rtd_outl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg, dc2h_rgb2yuv_ctrl_reg.regValue);

	dc2h_scaledown_dc2h1_uzd_ctrl0_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H1_UZD_CTRL0_reg);
	dc2h_scaledown_dc2h1_uzd_ctrl0_reg.h_zoom_en = 0;
	dc2h_scaledown_dc2h1_uzd_ctrl0_reg.v_zoom_en = 0;
	rtd_outl(DC2H_SCALEDOWN_DC2H1_UZD_CTRL0_reg, dc2h_scaledown_dc2h1_uzd_ctrl0_reg.regValue);

    set_dc2h_capture_sdnr_in_clock(FALSE);
}

void resize_dc2h_capture_size(VT_CUR_CAPTURE_INFO *pdc2h_ctrl_info)
{
	if(pdc2h_ctrl_info->output_size.w > pdc2h_ctrl_info->input_size.w){
		unsigned int temp = 0;
		rtd_pr_vt_emerg("output w > input w, need set output width max equal input width\n");
		temp = pdc2h_ctrl_info->input_size.w;
		if(temp%16 != 0){
			temp = (temp - temp%16);
		}
		pdc2h_ctrl_info->output_size.w = temp;
	}
	if(pdc2h_ctrl_info->output_size.h > pdc2h_ctrl_info->input_size.h){
		pdc2h_ctrl_info->output_size.h = pdc2h_ctrl_info->input_size.h;
		if(pdc2h_ctrl_info->output_size.h%2 != 0)
			pdc2h_ctrl_info->output_size.h = pdc2h_ctrl_info->output_size.h - 1;
	}
}

void set_dc2h_isr_enable(VT_VIDEO_FRAME_BUFFER_PROPERTY_INFO_T *pdc2h_buf_info)
{
	if(get_vt_VtSwBufferMode(DC2H1) == TRUE){
		dc2h_cap_dc2h_interrupt_enable_RBUS dc2h_interrupt_enable_reg;
		dc2h_interrupt_enable_reg.regValue = rtd_inl(DC2H_CAP_DC2H_Interrupt_Enable_reg);

		if(pdc2h_buf_info->plane_number == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_PLANAR){
			dc2h_interrupt_enable_reg.dc2h_vs_rising_int_en = 1;
			dc2h_interrupt_enable_reg.dc2h_cap2_done_int_en = 0;
			dc2h_interrupt_enable_reg.dc2h_cap1_done_int_en = 0;
			dc2h_interrupt_enable_reg.dc2h_cap0_done_int_en = 1;
		}else if(pdc2h_buf_info->plane_number == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){
			dc2h_interrupt_enable_reg.dc2h_vs_rising_int_en = 1;
			dc2h_interrupt_enable_reg.dc2h_cap2_done_int_en = 0;
			dc2h_interrupt_enable_reg.dc2h_cap1_done_int_en = 0;
			dc2h_interrupt_enable_reg.dc2h_cap0_done_int_en = 1;
		}else{
			dc2h_interrupt_enable_reg.dc2h_vs_rising_int_en = 1; // one plane
			dc2h_interrupt_enable_reg.dc2h_cap2_done_int_en = 0;
			dc2h_interrupt_enable_reg.dc2h_cap1_done_int_en = 0;
			dc2h_interrupt_enable_reg.dc2h_cap0_done_int_en = 1;
		}
		rtd_outl(DC2H_CAP_DC2H_Interrupt_Enable_reg, dc2h_interrupt_enable_reg.regValue);
	}
}

void drvif_dc2h_capture_config(VT_CUR_CAPTURE_INFO *pdc2h_ctrl_info)
{
	if(pdc2h_ctrl_info == NULL){
		rtd_pr_vt_emerg("%s=%d pdc2h_ctrl_info is NULL!\n", __FUNCTION__, __LINE__);
		return;
	}
	if(pdc2h_ctrl_info->enable == TRUE)
	{
		set_dc2h_in_size_and_in_sel(pdc2h_ctrl_info->cap_location);
		/* crop region initial to input size */
		//pdc2h_ctrl_info->crop_size = pdc2h_ctrl_info->input_size;
		//rtd_pr_vt_emerg("dc2h crop size (%d,%d,%d,%d)\n", pdc2h_ctrl_info->crop_size.x, pdc2h_ctrl_info->crop_size.y, pdc2h_ctrl_info->crop_size.w, pdc2h_ctrl_info->crop_size.h);
		resize_dc2h_capture_size(pdc2h_ctrl_info);
		set_dc2h_cap_buf_property(pdc2h_ctrl_info);
		rtd_pr_vt_emerg("output wxh=%dx%d\n", pdc2h_ctrl_info->output_size.w, pdc2h_ctrl_info->output_size.h);	

		drvif_set_dc2h_crop_config(0, 0, pdc2h_ctrl_info->input_size.w, pdc2h_ctrl_info->input_size.h, FALSE);	
		drvif_set_dc2h_r2y_config(pdc2h_ctrl_info->fmt, pdc2h_ctrl_info->cap_location);
		//drvif_set_dc2h_scale_down_config(pdc2h_ctrl_info->input_size, pdc2h_ctrl_info->output_size);
		drvif_set_dc2h_scale_down_config(pdc2h_ctrl_info->crop_size, pdc2h_ctrl_info->output_size);
		if(dc2h_cap_fmt_is_rgb_space(pdc2h_ctrl_info->fmt) == TRUE){
			set_dc2h_sequence_cap(pdc2h_ctrl_info);
		}else if(pdc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit || pdc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV21 ||
			pdc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_10bit){
			set_dc2h_nv12_cap(pdc2h_ctrl_info);
		}else if(pdc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_P010){
			set_dc2h_p010_cap(pdc2h_ctrl_info);
		}else if(pdc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YV12 || pdc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_I420){
			set_dc2h_yv12_i420_cap(pdc2h_ctrl_info);
		}else{
			rtd_pr_vt_emerg("%s=%d invalid pixel format!\n", __FUNCTION__, __LINE__);
			return;
		}

		if(get_vt_VtSwBufferMode(DC2H1) == TRUE){
			if(FALSE == drvif_set_dc2h_swmode_inforpc(TRUE, &CapBuffer_VT[0])){
				rtd_pr_vt_emerg("%s=%d RPC fail so not setting dc2h and return!\n", __FUNCTION__, __LINE__);
				return;
			}
		}
	
		set_dc2h_isr_enable(&CapBuffer_VT[DC2H1]); //isr enable
	}else{
		/* stop dc2h hw capture */
		disable_dc2h_capture();
		if(get_vt_VtSwBufferMode(DC2H1) == TRUE){
			if(FALSE == drvif_set_dc2h_swmode_inforpc(FALSE, &CapBuffer_VT[0])){
				rtd_pr_vt_emerg("%s=%d RPC fail so not setting dc2h and return!\n", __FUNCTION__, __LINE__);
				return;
			}
		}
	}
}
// ------------------------------dc2h2------------------------------------

void set_dc2h2_cap_color_fmt_rgb(VT_CAP_FRAME_LINE_MODE dc2h_cap_mode)
{
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	dc2h2_cap_color_format_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
	dc2h2_cap_color_format_reg.dc2h2_frame_access_mode = dc2h_cap_mode;
	dc2h2_cap_color_format_reg.dc2h2_seq_blk_sel = DC2H_SEQ_MODE;
	dc2h2_cap_color_format_reg.dc2h2_pixel_encoding = PIXEL_ENCODING_444;
	dc2h2_cap_color_format_reg.dc2h2_bit_sel = PER_CHANNEL_8_BIT;
	dc2h2_cap_color_format_reg.dc2h2_yv12_en = CAP_YV12_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_p010_en = CAP_P010_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_cb_cr_swap = DC2H_CB_CR_NO_SWAP;
	dc2h2_cap_color_format_reg.dc2h2_rgb_to_argb_en = RGB_TO_ARGB_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_channel_swap = CHANNEL_SWAP_BGR;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, dc2h2_cap_color_format_reg.regValue);
}

void set_dc2h2_cap_color_fmt_argb(VT_CAP_ARGB_TYPE alpha_location, VT_CAP_FRAME_LINE_MODE dc2h_cap_mode)
{
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	dc2h2_cap_color_format_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
	dc2h2_cap_color_format_reg.dc2h2_frame_access_mode = dc2h_cap_mode;
	dc2h2_cap_color_format_reg.dc2h2_seq_blk_sel = DC2H_SEQ_MODE;
	dc2h2_cap_color_format_reg.dc2h2_pixel_encoding = PIXEL_ENCODING_444;
	dc2h2_cap_color_format_reg.dc2h2_bit_sel = PER_CHANNEL_8_BIT;
	dc2h2_cap_color_format_reg.dc2h2_yv12_en = CAP_YV12_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_p010_en = CAP_P010_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_cb_cr_swap = DC2H_CB_CR_NO_SWAP;
	dc2h2_cap_color_format_reg.dc2h2_rgb_to_argb_en = RGB_TO_ARGB_ENABLE;
	dc2h2_cap_color_format_reg.dc2h2_channel_swap = CHANNEL_SWAP_RGB;
	dc2h2_cap_color_format_reg.dc2h2_argb_dummy_data = 0xFF;
	dc2h2_cap_color_format_reg.dc2h2_argb_dummy_loc = alpha_location;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, dc2h2_cap_color_format_reg.regValue);
}

void set_dc2h2_cap_color_fmt_nv12(VT_CAP_BIT_SEL bit_sel, VT_CAP_FRAME_LINE_MODE dc2h_cap_mode, VT_CAP_CB_CR_SWAP cbcr_swap)
{
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	dc2h2_cap_color_format_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
	dc2h2_cap_color_format_reg.dc2h2_frame_access_mode = dc2h_cap_mode;
	dc2h2_cap_color_format_reg.dc2h2_seq_blk_sel = DC2H_BLK_MODE;
	dc2h2_cap_color_format_reg.dc2h2_pixel_encoding = PIXEL_ENCODING_420;
	dc2h2_cap_color_format_reg.dc2h2_bit_sel = bit_sel; /* NV12 8bit or 10bit */
	dc2h2_cap_color_format_reg.dc2h2_yv12_en = CAP_YV12_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_p010_en = CAP_P010_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_cb_cr_swap = cbcr_swap;
	dc2h2_cap_color_format_reg.dc2h2_rgb_to_argb_en = RGB_TO_ARGB_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_channel_swap = CHANNEL_SWAP_GBR;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, dc2h2_cap_color_format_reg.regValue);
}

void set_dc2h2_cap_color_fmt_yv12(VT_CAP_FRAME_LINE_MODE dc2h_cap_mode)
{
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	dc2h2_cap_color_format_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
	dc2h2_cap_color_format_reg.dc2h2_frame_access_mode = dc2h_cap_mode;
	dc2h2_cap_color_format_reg.dc2h2_seq_blk_sel = DC2H_BLK_MODE;
	dc2h2_cap_color_format_reg.dc2h2_pixel_encoding = PIXEL_ENCODING_420;
	dc2h2_cap_color_format_reg.dc2h2_bit_sel = PER_CHANNEL_8_BIT;
	dc2h2_cap_color_format_reg.dc2h2_yv12_en = CAP_YV12_ENABLE;
	dc2h2_cap_color_format_reg.dc2h2_p010_en = CAP_P010_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_cb_cr_swap = DC2H_CB_CR_SWAP;
	dc2h2_cap_color_format_reg.dc2h2_rgb_to_argb_en = RGB_TO_ARGB_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_channel_swap = CHANNEL_SWAP_GBR;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, dc2h2_cap_color_format_reg.regValue);
}

void set_dc2h2_cap_color_fmt_i420(VT_CAP_FRAME_LINE_MODE dc2h_cap_mode)
{
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	dc2h2_cap_color_format_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
	dc2h2_cap_color_format_reg.dc2h2_frame_access_mode = dc2h_cap_mode;
	dc2h2_cap_color_format_reg.dc2h2_seq_blk_sel = DC2H_BLK_MODE;
	dc2h2_cap_color_format_reg.dc2h2_pixel_encoding = PIXEL_ENCODING_420;
	dc2h2_cap_color_format_reg.dc2h2_bit_sel = PER_CHANNEL_8_BIT;
	dc2h2_cap_color_format_reg.dc2h2_yv12_en = CAP_YV12_ENABLE;
	dc2h2_cap_color_format_reg.dc2h2_p010_en = CAP_P010_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_cb_cr_swap = DC2H_CB_CR_NO_SWAP;
	dc2h2_cap_color_format_reg.dc2h2_rgb_to_argb_en = RGB_TO_ARGB_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_channel_swap = CHANNEL_SWAP_GBR;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, dc2h2_cap_color_format_reg.regValue);
}

void set_dc2h2_cap_color_fmt_p010(VT_CAP_FRAME_LINE_MODE dc2h_cap_mode)
{
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	dc2h2_cap_color_format_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
	dc2h2_cap_color_format_reg.dc2h2_frame_access_mode = dc2h_cap_mode;
	dc2h2_cap_color_format_reg.dc2h2_seq_blk_sel = DC2H_BLK_MODE;
	dc2h2_cap_color_format_reg.dc2h2_pixel_encoding = PIXEL_ENCODING_420;
	dc2h2_cap_color_format_reg.dc2h2_bit_sel = PER_CHANNEL_10_BIT;
	dc2h2_cap_color_format_reg.dc2h2_yv12_en = CAP_YV12_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_p010_en = CAP_P010_ENABLE;
	dc2h2_cap_color_format_reg.dc2h2_cb_cr_swap = DC2H_CB_CR_SWAP;
	dc2h2_cap_color_format_reg.dc2h2_rgb_to_argb_en = RGB_TO_ARGB_DISABLE;
	dc2h2_cap_color_format_reg.dc2h2_channel_swap = CHANNEL_SWAP_GBR;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, dc2h2_cap_color_format_reg.regValue);
}

void set_dc2h2_cap0_line_burst_num(int burst_num, unsigned int capLen, VT_CAP_FRAME_LINE_MODE cap_mode)
{
	dc2h2_cap_dc2h2_cap0_cap_ctl0_RBUS dc2h2_cap_dc2h2_cap0_cap_ctl0_reg;
	dc2h2_cap_dc2h2_cap0_blk0_ctrl0_RBUS dc2h2_cap_dc2h2_cap0_blk0_ctrl0_reg; //dc2h2_cap0_sta_addr_blk0
	dc2h2_cap_dc2h2_cap0_blk0_ctrl1_RBUS dc2h2_cap_dc2h2_cap0_blk0_ctrl1_reg; //dc2h2_cap0_DMA_up_limit_blk0
	dc2h2_cap_dc2h2_cap0_blk0_ctrl2_RBUS dc2h2_cap_dc2h2_cap0_blk0_ctrl2_reg; //dc2h2_cap0_DMA_low_limit_blk0
	dc2h2_cap_dc2h2_cap0_wr_dma_num_bl_wrap_line_step_RBUS dc2h2_cap0_wr_dma_num_bl_wrap_line_step_reg;
	dc2h2_cap_dc2h2_cap0_wr_dma_num_bl_wrap_ctl_RBUS dc2h2_cap0_wr_dma_num_bl_wrap_ctl_reg;
	dc2h2_cap_dc2h2_cap0_wr_dma_num_bl_wrap_word_RBUS dc2h2_cap0_wr_dma_num_bl_wrap_word_reg;

	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_Cap_CTL0_reg);
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_buf_index_mode = 0; //FW set memory start address
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_frc_style = 0;
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_buf_cnt_en = 0;
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_buf_index_fw = 0; //0x0: 1th frame buffer
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_smooth_toggle_en = 0;
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_Cap_CTL0_reg, dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.regValue);

	dc2h2_cap_dc2h2_cap0_blk0_ctrl0_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL0_reg);
	dc2h2_cap_dc2h2_cap0_blk0_ctrl0_reg.dc2h2_cap0_sta_addr_blk0 = CapBuffer_VT[DC2H2].vt_pa_buf_info[0].pa_y >> 4;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h2_cap_dc2h2_cap0_blk0_ctrl0_reg.dc2h2_cap0_sta_addr_blk0 += (capLen - 1) * burst_num;
	}
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL0_reg, dc2h2_cap_dc2h2_cap0_blk0_ctrl0_reg.regValue);

	dc2h2_cap0_wr_dma_num_bl_wrap_line_step_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_line_step_reg);
	dc2h2_cap0_wr_dma_num_bl_wrap_line_step_reg.dc2h2_cap0_line_step = burst_num;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h2_cap0_wr_dma_num_bl_wrap_line_step_reg.dc2h2_cap0_line_step = -burst_num;
	}
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_line_step_reg, dc2h2_cap0_wr_dma_num_bl_wrap_line_step_reg.regValue);

	dc2h2_cap0_wr_dma_num_bl_wrap_ctl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_ctl_reg);
	if(cap_mode == DC2H_CAP_LINE_MODE){
		dc2h2_cap0_wr_dma_num_bl_wrap_ctl_reg.dc2h2_cap0_line_num = capLen;
	}else{
		dc2h2_cap0_wr_dma_num_bl_wrap_ctl_reg.dc2h2_cap0_line_num = 1; /* frame mode */
	}
	dc2h2_cap0_wr_dma_num_bl_wrap_ctl_reg.dc2h2_cap0_burst_len = DEFAULT_DC2H_CAP_BURST_LEN; /* dc2h2_cap0_burst_len*128bit */
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_ctl_reg, dc2h2_cap0_wr_dma_num_bl_wrap_ctl_reg.regValue);

	dc2h2_cap0_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_word_reg);
	dc2h2_cap0_wr_dma_num_bl_wrap_word_reg.dc2h2_cap0_line_burst_num = burst_num;
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_word_reg, dc2h2_cap0_wr_dma_num_bl_wrap_word_reg.regValue);

	dc2h2_cap_dc2h2_cap0_blk0_ctrl2_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[sort_boundary_addr_min_index(DC2H2)].pa_y;
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL2_reg, dc2h2_cap_dc2h2_cap0_blk0_ctrl2_reg.regValue);
	dc2h2_cap_dc2h2_cap0_blk0_ctrl1_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[sort_boundary_addr_max_index(DC2H2)].pa_y + CapBuffer_VT[DC2H2].vt_pa_buf_info[0].size_plane_y - DEFAULT_DC2H_CAP_BURST_LEN;
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL1_reg, dc2h2_cap_dc2h2_cap0_blk0_ctrl1_reg.regValue);
}

void set_dc2h2_cap1_line_burst_num(int burst_num, unsigned int capLen, VT_CAP_FRAME_LINE_MODE cap_mode)
{
	dc2h2_cap_dc2h2_cap1_wr_dma_num_bl_wrap_addr_RBUS dc2h2_cap1_wr_dma_num_bl_wrap_addr_reg;
	dc2h2_cap_dc2h2_cap1_wr_dma_num_bl_wrap_line_step_RBUS dc2h2_cap1_wr_dma_num_bl_wrap_line_step_reg;
	dc2h2_cap_dc2h2_cap1_wr_dma_num_bl_wrap_ctl_RBUS dc2h2_cap1_wr_dma_num_bl_wrap_ctl_reg;
	dc2h2_cap_dc2h2_cap1_wr_dma_num_bl_wrap_word_RBUS dc2h2_cap1_wr_dma_num_bl_wrap_word_reg;

	dc2h2_cap_dc2h2_cap1_dma_wr_rule_check_up_RBUS dc2h2_cap1_dma_wr_rule_check_up_reg;
	dc2h2_cap_dc2h2_cap1_dma_wr_rule_check_low_RBUS dc2h2_cap1_dma_wr_rule_check_low_reg;

	dc2h2_cap1_wr_dma_num_bl_wrap_addr_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP1_WR_DMA_num_bl_wrap_addr_reg);
	dc2h2_cap1_wr_dma_num_bl_wrap_addr_reg.dc2h2_cap1_sta_addr = CapBuffer_VT[DC2H2].vt_pa_buf_info[0].pa_u >> 4;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h2_cap1_wr_dma_num_bl_wrap_addr_reg.dc2h2_cap1_sta_addr += (capLen - 1) * burst_num;
	}
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP1_WR_DMA_num_bl_wrap_addr_reg, dc2h2_cap1_wr_dma_num_bl_wrap_addr_reg.regValue);

	dc2h2_cap1_wr_dma_num_bl_wrap_line_step_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP1_WR_DMA_num_bl_wrap_line_step_reg);
	dc2h2_cap1_wr_dma_num_bl_wrap_line_step_reg.dc2h2_cap1_line_step = burst_num;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h2_cap1_wr_dma_num_bl_wrap_line_step_reg.dc2h2_cap1_line_step = -burst_num;
	}
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP1_WR_DMA_num_bl_wrap_line_step_reg, dc2h2_cap1_wr_dma_num_bl_wrap_line_step_reg.regValue);

	dc2h2_cap1_wr_dma_num_bl_wrap_ctl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP1_WR_DMA_num_bl_wrap_ctl_reg);
	if(cap_mode == DC2H_CAP_LINE_MODE){
		dc2h2_cap1_wr_dma_num_bl_wrap_ctl_reg.dc2h2_cap1_line_num = capLen;
	}else{
		dc2h2_cap1_wr_dma_num_bl_wrap_ctl_reg.dc2h2_cap1_line_num = 1; /* frame mode */
	}
	dc2h2_cap1_wr_dma_num_bl_wrap_ctl_reg.dc2h2_cap1_burst_len = DEFAULT_DC2H_CAP_BURST_LEN; /* dc2h_cap1_burst_len*128bit */
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP1_WR_DMA_num_bl_wrap_ctl_reg, dc2h2_cap1_wr_dma_num_bl_wrap_ctl_reg.regValue);

	dc2h2_cap1_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP1_WR_DMA_num_bl_wrap_word_reg);
	dc2h2_cap1_wr_dma_num_bl_wrap_word_reg.dc2h2_cap1_line_burst_num = burst_num;
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP1_WR_DMA_num_bl_wrap_word_reg, dc2h2_cap1_wr_dma_num_bl_wrap_word_reg.regValue);

	dc2h2_cap1_dma_wr_rule_check_low_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[sort_boundary_addr_min_index_2nd_plane(DC2H2)].pa_u;
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP1_DMA_WR_Rule_check_low_reg, dc2h2_cap1_dma_wr_rule_check_low_reg.regValue);

	dc2h2_cap1_dma_wr_rule_check_up_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[sort_boundary_addr_max_index_2nd_plane(DC2H2)].pa_u + CapBuffer_VT[DC2H2].vt_pa_buf_info[0].size_plane_u - DEFAULT_DC2H_CAP_BURST_LEN;
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP1_DMA_WR_Rule_check_up_reg, dc2h2_cap1_dma_wr_rule_check_up_reg.regValue);
}

void set_dc2h2_cap2_line_burst_num(int burst_num, unsigned int capLen, VT_CAP_FRAME_LINE_MODE cap_mode)
{
	dc2h2_cap_dc2h2_cap2_wr_dma_num_bl_wrap_addr_RBUS dc2h2_cap2_wr_dma_num_bl_wrap_addr_reg;
	dc2h2_cap_dc2h2_cap2_wr_dma_num_bl_wrap_line_step_RBUS dc2h2_cap2_wr_dma_num_bl_wrap_line_step_reg;
	dc2h2_cap_dc2h2_cap2_wr_dma_num_bl_wrap_ctl_RBUS dc2h2_cap2_wr_dma_num_bl_wrap_ctl_reg;
	dc2h2_cap_dc2h2_cap2_wr_dma_num_bl_wrap_word_RBUS dc2h2_cap2_wr_dma_num_bl_wrap_word_reg;

	dc2h2_cap_dc2h2_cap2_dma_wr_rule_check_up_RBUS dc2h2_cap2_dma_wr_rule_check_up_reg;
	dc2h2_cap_dc2h2_cap2_dma_wr_rule_check_low_RBUS dc2h2_cap2_dma_wr_rule_check_low_reg;

	dc2h2_cap2_wr_dma_num_bl_wrap_addr_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP2_WR_DMA_num_bl_wrap_addr_reg);
	dc2h2_cap2_wr_dma_num_bl_wrap_addr_reg.dc2h2_cap2_sta_addr = CapBuffer_VT[DC2H2].vt_pa_buf_info[0].pa_v >> 4;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h2_cap2_wr_dma_num_bl_wrap_addr_reg.dc2h2_cap2_sta_addr += (capLen - 1) * burst_num;
	}
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP2_WR_DMA_num_bl_wrap_addr_reg, dc2h2_cap2_wr_dma_num_bl_wrap_addr_reg.regValue);

	dc2h2_cap2_wr_dma_num_bl_wrap_line_step_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP2_WR_DMA_num_bl_wrap_line_step_reg);
	dc2h2_cap2_wr_dma_num_bl_wrap_line_step_reg.dc2h2_cap2_line_step = burst_num;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h2_cap2_wr_dma_num_bl_wrap_line_step_reg.dc2h2_cap2_line_step = -burst_num;
	}
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP2_WR_DMA_num_bl_wrap_line_step_reg, dc2h2_cap2_wr_dma_num_bl_wrap_line_step_reg.regValue);

	dc2h2_cap2_wr_dma_num_bl_wrap_ctl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP2_WR_DMA_num_bl_wrap_ctl_reg);
	if(cap_mode == DC2H_CAP_LINE_MODE){
		dc2h2_cap2_wr_dma_num_bl_wrap_ctl_reg.dc2h2_cap2_line_num = capLen;
	}else{
		dc2h2_cap2_wr_dma_num_bl_wrap_ctl_reg.dc2h2_cap2_line_num = 1; /* frame mode */
	}
	dc2h2_cap2_wr_dma_num_bl_wrap_ctl_reg.dc2h2_cap2_burst_len = DEFAULT_DC2H_CAP_BURST_LEN; /* dc2h_cap2_burst_len*128bit */
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP2_WR_DMA_num_bl_wrap_ctl_reg, dc2h2_cap2_wr_dma_num_bl_wrap_ctl_reg.regValue);

	dc2h2_cap2_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP2_WR_DMA_num_bl_wrap_word_reg);
	dc2h2_cap2_wr_dma_num_bl_wrap_word_reg.dc2h2_cap2_line_burst_num = burst_num;
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP2_WR_DMA_num_bl_wrap_word_reg, dc2h2_cap2_wr_dma_num_bl_wrap_word_reg.regValue);

	dc2h2_cap2_dma_wr_rule_check_low_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[sort_boundary_addr_min_index_3rd_plane(DC2H2)].pa_v;
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP2_DMA_WR_Rule_check_low_reg, dc2h2_cap2_dma_wr_rule_check_low_reg.regValue);

	dc2h2_cap2_dma_wr_rule_check_up_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[sort_boundary_addr_max_index_3rd_plane(DC2H2)].pa_v + CapBuffer_VT[DC2H2].vt_pa_buf_info[0].size_plane_v - DEFAULT_DC2H_CAP_BURST_LEN;
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP2_DMA_WR_Rule_check_up_reg, dc2h2_cap2_dma_wr_rule_check_up_reg.regValue);
}

void set_dc2h2_in_size_and_in_sel(unsigned int location)
{
	unsigned int input_w = 0, input_h = 0;
	dc2h2_cap_dc2h2_ctrl_RBUS dc2h2_cap_dc2h2_ctrl_reg;
	ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;

	ppoverlay_main_den_h_start_end_RBUS main_den_h_start_end_Reg;
	ppoverlay_main_den_v_start_end_RBUS main_den_v_start_end_Reg;
	ppoverlay_main_active_h_start_end_RBUS main_active_h_start_end_Reg;
	ppoverlay_main_active_v_start_end_RBUS main_active_v_start_end_Reg;
	ppoverlay_sub_active_h_start_end_RBUS sub_active_h_start_end_reg;
	ppoverlay_sub_active_v_start_end_RBUS sub_active_v_start_end_reg;
	ppoverlay_osd4dtg_dv_den_start_end_RBUS ppoverlay_osd4dtg_dv_den_start_end_reg;
	ppoverlay_osd4dtg_dh_den_start_end_RBUS ppoverlay_osd4dtg_dh_den_start_end_reg;

	dc2h2_cap_dc2h2_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CTRL_reg);
	dc2h2_cap_dc2h2_ctrl_reg.clken_disp_dc2h2 = 1; 
	//rtd_outl(DC2H2_CAP_DC2H2_CTRL_reg, dc2h2_cap_dc2h2_ctrl_reg.regValue);

	if(location == _MAIN_UZU || location == _TWO_SECOND_UZU){
		main_active_h_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_H_Start_End_reg);
		main_active_v_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_V_Start_End_reg);
		ppoverlay_uzudtg_control1_reg.regValue = rtd_inl(PPOVERLAY_uzudtg_control1_reg);
		input_w = (main_active_h_start_end_Reg.mh_act_end - main_active_h_start_end_Reg.mh_act_sta);
		input_h = (main_active_v_start_end_Reg.mv_act_end - main_active_v_start_end_Reg.mv_act_sta);
		if(ppoverlay_uzudtg_control1_reg.two_step_uzu_en && location == _MAIN_UZU){
			input_w = input_w >> 1;
			input_h = input_h >> 1;
		}
	}else if(location == _Sub_UZU_Output){
		sub_active_h_start_end_reg.regValue = rtd_inl(PPOVERLAY_SUB_Active_H_Start_End_reg);
		sub_active_v_start_end_reg.regValue = rtd_inl(PPOVERLAY_SUB_Active_V_Start_End_reg);
		input_w = (sub_active_h_start_end_reg.sh_act_end - sub_active_h_start_end_reg.sh_act_sta);
		input_h = (sub_active_v_start_end_reg.sv_act_end - sub_active_v_start_end_reg.sv_act_sta);
	}else if(location == _MAIN_YUV2RGB || location == _OSD123_MIXER_Output || location == _Dither_Output || location == _Memc_mux_Output ||
		location == _Memc_Mux_Input){
		main_den_h_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_DEN_H_Start_End_reg);
		main_den_v_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_DEN_V_Start_End_reg);
		input_w = (main_den_h_start_end_Reg.mh_den_end - main_den_h_start_end_Reg.mh_den_sta);
		input_h = (main_den_v_start_end_Reg.mv_den_end - main_den_v_start_end_Reg.mv_den_sta);
	}else if(location == _Idomain_Sdnr_input){
        input_w = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DI_WID);
        input_h = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DI_LEN);
        set_dc2h_capture_sdnr_in_clock(TRUE);
    }else if(location == _OSD4_Output){
		ppoverlay_osd4dtg_dh_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_osd4dtg_DH_DEN_Start_End_reg);
		ppoverlay_osd4dtg_dv_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_osd4dtg_DV_DEN_Start_End_reg);
		input_w = (ppoverlay_osd4dtg_dh_den_start_end_reg.osd4dtg_dh_den_end - ppoverlay_osd4dtg_dh_den_start_end_reg.osd4dtg_dh_den_sta);
		input_h = (ppoverlay_osd4dtg_dv_den_start_end_reg.osd4dtg_dv_den_end - ppoverlay_osd4dtg_dv_den_start_end_reg.osd4dtg_dv_den_sta);
	}else{
        rtd_pr_vt_emerg("[error]invalid cap position %d, line:%d\n",__LINE__, location);
		dc2h2_cap_dc2h2_ctrl_reg.clken_disp_dc2h2 = 0;
		rtd_outl(DC2H2_CAP_DC2H2_CTRL_reg, dc2h2_cap_dc2h2_ctrl_reg.regValue);
		location = _NO_INPUT;
    }

	dc2h2_cap_dc2h2_ctrl_reg.dc2h2_in_sel = location;
	dc2h2_cap_dc2h2_ctrl_reg.dc2h2_in_vact = input_h;
	rtd_outl(DC2H2_CAP_DC2H2_CTRL_reg, dc2h2_cap_dc2h2_ctrl_reg.regValue);

	/* save dc2h2 original input size */
	curCapInfo[DC2H2].input_size.x = 0;
	curCapInfo[DC2H2].input_size.y = 0;
	curCapInfo[DC2H2].input_size.w = input_w;
	curCapInfo[DC2H2].input_size.h = input_h;

	rtd_pr_vt_notice("[VT]vt cap src=%d,input w=%d,h=%d\n", location, input_w, input_h);

    // SDNR input W >4096 or V >2160, dc2h output capture max 2k
    if(input_w > VT_CAP_FRAME_WIDTH_4K2K || input_h > VT_CAP_FRAME_HEIGHT_4K2K){
        if(curCapInfo[DC2H2].output_size.w > VT_CAP_FRAME_WIDTH_2K1k && curCapInfo[DC2H2].output_size.w < input_w){
			curCapInfo[DC2H2].output_size.w = VT_CAP_FRAME_WIDTH_2K1k;
		}
		if(curCapInfo[DC2H2].output_size.h > VT_CAP_FRAME_HEIGHT_2K1k && curCapInfo[DC2H2].output_size.h < input_h){
			curCapInfo[DC2H2].output_size.h = VT_CAP_FRAME_HEIGHT_2K1k;
		}
		rtd_pr_vt_notice("Input hact over 3840,vact over 2160 , output max support 2k1k %dx%d\n", curCapInfo[DC2H2].output_size.w, curCapInfo[DC2H2].output_size.h);
     }
}

unsigned char drvif_set_dc2h2_swmode_inforpc(unsigned int onoff, VT_VIDEO_FRAME_BUFFER_PROPERTY_INFO_T *pdc2h_buf_info)
{
	DC2H_SWMODE_STRUCT_T *swmode_infoptr;

	int ret;
    int i;
	int buf_num = 0;
	rtd_pr_vt_notice("fun:%s, onoff %d\n", __FUNCTION__, onoff);
	swmode_infoptr = (DC2H_SWMODE_STRUCT_T *)Scaler_GetShareMemVirAddr(SCALERIOC_SET_DC2H2_SWMODE_ENABLE);

	buf_num = pdc2h_buf_info->vtbuffernumber;
	swmode_infoptr->SwModeEnable = onoff;
	swmode_infoptr->buffernumber = buf_num;
	swmode_infoptr->cap_format = pdc2h_buf_info->pixelFormat;
	swmode_infoptr->cap_width = pdc2h_buf_info->width;
	swmode_infoptr->cap_length = pdc2h_buf_info->height;
	swmode_infoptr->user_fps = get_vt_target_fps(DC2H2);
	//swmode_infoptr->YbufferSize = vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height);
	// swmode_infoptr->YbufferSize = width * length;

	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h2_cap_dc2h2_cap0_wr_dma_num_bl_wrap_word_RBUS dc2h2_cap0_wr_dma_num_bl_wrap_word_reg;
		dc2h2_cap_dc2h2_cap1_wr_dma_num_bl_wrap_word_RBUS dc2h2_cap1_wr_dma_num_bl_wrap_word_reg;
		dc2h2_cap_dc2h2_cap2_wr_dma_num_bl_wrap_word_RBUS dc2h2_cap2_wr_dma_num_bl_wrap_word_reg;
		dc2h2_cap0_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_word_reg);
		dc2h2_cap1_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP1_WR_DMA_num_bl_wrap_word_reg);
		dc2h2_cap2_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP2_WR_DMA_num_bl_wrap_word_reg);

		for (i = 0; i < buf_num; i++){
			swmode_infoptr->cap_buffers[i].pa_y = ((pdc2h_buf_info->vt_pa_buf_info[i].pa_y) ? (pdc2h_buf_info->vt_pa_buf_info[i].pa_y + (pdc2h_buf_info->height - 1)*dc2h2_cap0_wr_dma_num_bl_wrap_word_reg.dc2h2_cap0_line_burst_num) : (0));

			if(pdc2h_buf_info->plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){ //two plane
				swmode_infoptr->cap_buffers[i].pa_u = ((pdc2h_buf_info->vt_pa_buf_info[i].pa_u) ? (pdc2h_buf_info->vt_pa_buf_info[i].pa_u + (pdc2h_buf_info->height/2 - 1)*dc2h2_cap1_wr_dma_num_bl_wrap_word_reg.dc2h2_cap1_line_burst_num) : (0));
			}

			if(pdc2h_buf_info->plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){ //three plane
				swmode_infoptr->cap_buffers[i].pa_v = ((pdc2h_buf_info->vt_pa_buf_info[i].pa_v) ? (pdc2h_buf_info->vt_pa_buf_info[i].pa_v + (pdc2h_buf_info->height/2 - 1)*dc2h2_cap2_wr_dma_num_bl_wrap_word_reg.dc2h2_cap2_line_burst_num) : (0));
			}
		}
    }else{
		for (i = 0; i < buf_num; i++){
			swmode_infoptr->cap_buffers[i].pa_y = ((pdc2h_buf_info->vt_pa_buf_info[i].pa_y) ? (pdc2h_buf_info->vt_pa_buf_info[i].pa_y) : (0));

			if(pdc2h_buf_info->plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){ //two plane
				swmode_infoptr->cap_buffers[i].pa_u = ((pdc2h_buf_info->vt_pa_buf_info[i].pa_u) ? (pdc2h_buf_info->vt_pa_buf_info[i].pa_u) : (0));
			}

			if(pdc2h_buf_info->plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){ //three plane
				swmode_infoptr->cap_buffers[i].pa_v = ((pdc2h_buf_info->vt_pa_buf_info[i].pa_v) ? (pdc2h_buf_info->vt_pa_buf_info[i].pa_v) : (0));
			}
		}
	}

	//add debu log to print rpc info
	//change endian
	swmode_infoptr->SwModeEnable = htonl(swmode_infoptr->SwModeEnable);
	swmode_infoptr->buffernumber = htonl(swmode_infoptr->buffernumber);
	swmode_infoptr->cap_format = htonl(swmode_infoptr->cap_format);
	swmode_infoptr->cap_width = htonl(swmode_infoptr->cap_width);
	swmode_infoptr->cap_length = htonl(swmode_infoptr->cap_length);
	swmode_infoptr->user_fps = htonl(swmode_infoptr->user_fps);
	//swmode_infoptr->YbufferSize = htonl(swmode_infoptr->YbufferSize);

	for (i = 0; i < buf_num; i++){
		swmode_infoptr->cap_buffers[i].pa_y = htonl(swmode_infoptr->cap_buffers[i].pa_y);
		swmode_infoptr->cap_buffers[i].size_plane_y = htonl(swmode_infoptr->cap_buffers[i].size_plane_y);
		
		if(pdc2h_buf_info->plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){ //two plane
			swmode_infoptr->cap_buffers[i].pa_u = htonl(swmode_infoptr->cap_buffers[i].pa_u);
			swmode_infoptr->cap_buffers[i].size_plane_u = htonl(swmode_infoptr->cap_buffers[i].size_plane_u);
		}

		if(pdc2h_buf_info->plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){ //three plane
			swmode_infoptr->cap_buffers[i].pa_v = htonl(swmode_infoptr->cap_buffers[i].pa_v);
			swmode_infoptr->cap_buffers[i].size_plane_v = htonl(swmode_infoptr->cap_buffers[i].size_plane_v);
		}
	}

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_SET_DC2H2_SWMODE_ENABLE,0,0))){
		rtd_pr_vt_emerg("[dc2h2]ret=%d, SCALERIOC_SET_DC2H2_SWMODE_ENABLE RPC fail !!!\n", ret);
		return FALSE;
	}
	return TRUE;
}

void drvif_set_dc2h2_crop_config(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char en)
{
	dc2h2_cap_dc2h2_3dmaskto2d_ctrl_RBUS dc2h2_cap_3dmaskto2d_ctrl_reg;
	dc2h2_cap_dc2h2_overscan_ctrl0_RBUS dc2h2_cap_overscan_ctrl0_reg;
	dc2h2_cap_dc2h2_3dmaskto2d_ctrl1_RBUS dc2h2_cap_3dmaskto2d_ctrl1_reg;

	dc2h2_cap_3dmaskto2d_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_3DMaskTo2D_Ctrl_reg);
	dc2h2_cap_overscan_ctrl0_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_OVERSCAN_Ctrl0_reg);
	dc2h2_cap_3dmaskto2d_ctrl1_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_3DMaskTo2D_Ctrl1_reg);

	if(x % 2){
		x = x - (x % 2);
	}
	if((x + w) % 2){
		w = w - (w % 2);
	}

	dc2h2_cap_overscan_ctrl0_reg.dc2h2_overscan_left = x;
	dc2h2_cap_overscan_ctrl0_reg.dc2h2_overscan_right = x+w-2;
	rtd_outl(DC2H2_CAP_DC2H2_OVERSCAN_Ctrl0_reg, dc2h2_cap_overscan_ctrl0_reg.regValue);

	dc2h2_cap_3dmaskto2d_ctrl1_reg.dc2h2_overscan_top = y;
	dc2h2_cap_3dmaskto2d_ctrl1_reg.dc2h2_overscan_bottom = y+h-1;
	rtd_outl(DC2H2_CAP_DC2H2_3DMaskTo2D_Ctrl1_reg, dc2h2_cap_3dmaskto2d_ctrl1_reg.regValue);

	dc2h2_cap_3dmaskto2d_ctrl_reg.dc2h2_3dmaskto2d_3dformat = 4;/*overscan*/
	dc2h2_cap_3dmaskto2d_ctrl_reg.dc2h2_3dmaskto2d_h_size = curCapInfo[DC2H2].input_size.w;
	dc2h2_cap_3dmaskto2d_ctrl_reg.dc2h2_3dmaskto2d_mode = 0; /* L data */
	dc2h2_cap_3dmaskto2d_ctrl_reg.dc2h2_3dmaskto2d_en = (curCapInfo[DC2H2].cap_location == _Idomain_Sdnr_input) ? 0 : en;
    rtd_outl(DC2H2_CAP_DC2H2_3DMaskTo2D_Ctrl_reg, dc2h2_cap_3dmaskto2d_ctrl_reg.regValue);

	/* save crop region */
	curCapInfo[DC2H2].crop_size.x = x;
	curCapInfo[DC2H2].crop_size.y = y;
	curCapInfo[DC2H2].crop_size.w = w;
	curCapInfo[DC2H2].crop_size.h = h;
	rtd_pr_vt_notice("saved crop size (%d,%d,%d,%d)\n", curCapInfo[DC2H2].crop_size.x, curCapInfo[DC2H2].crop_size.y, curCapInfo[DC2H2].crop_size.w, curCapInfo[DC2H2].crop_size.h);
}

unsigned char dc2h2_cap_fmt_is_rgb_space(unsigned int cap_fmt)
{
	if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB && cap_fmt <= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ABGR){
		return TRUE;
	}else if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit && cap_fmt < V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_MAXN){
		return FALSE; //capture format is yuv space
	}
	else{
		rtd_pr_vt_notice("%s, error,unexpected capture format\n", __FUNCTION__);
		return TRUE;
	}
}

unsigned char dc2h2_cap_fmt_is_10bit(unsigned int cap_fmt)
{
	if(cap_fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_10bit || cap_fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_P010){
		return TRUE;
	}else{
		return FALSE;
	}
}

unsigned char dc2h2_cap_pixel_encoding_is_444(unsigned int cap_fmt)
{
	if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB && cap_fmt <= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ABGR){
		return TRUE;
	}else if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit && cap_fmt < V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_MAXN){
		return FALSE;
	}
	else{
		rtd_pr_vt_notice("%s, error, unexpected capture format\n", __FUNCTION__);
		return TRUE;
	}
}

DC2H_Enable_rgb2yuv set_dc2h2_colconv_enable(unsigned int cap_fmt)
{
	unsigned int dc2h_in_sel = DC2H2_CAP_DC2H2_CTRL_get_dc2h2_in_sel(IoReg_Read32(DC2H2_CAP_DC2H2_CTRL_reg));
	DC2H_Enable_rgb2yuv dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;

	if(dc2h_in_sel == _NO_INPUT || dc2h_in_sel >= _VD_OUTPUT){
		return dc2h_enrgb2yuv_state;
	}

	if(dc2h2_cap_fmt_is_rgb_space(cap_fmt) == TRUE) // capture format is rgb space
	{
		if(dc2h_in_sel == _MAIN_UZU || dc2h_in_sel == _TWO_SECOND_UZU || dc2h_in_sel == _Sub_UZU_Output){
			dc2h_enrgb2yuv_state = DC2H_Colconv_YUVtoRGB; 
		}
		else if(dc2h_in_sel == _Idomain_Sdnr_input){
			if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == VODMA_COLOR_RGB) && (RGB2YUV_ICH1_RGB2YUV_CTRL_get_en_rgb2yuv(IoReg_Read32(RGB2YUV_ICH1_RGB2YUV_CTRL_reg)) == 0)){
                dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
            }else{
                dc2h_enrgb2yuv_state = DC2H_Colconv_YUVtoRGB;
            }
		}else{
			dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
		}
	}else{ // capture format is yuv space
		if(dc2h_in_sel == _MAIN_UZU || dc2h_in_sel == _TWO_SECOND_UZU || dc2h_in_sel == _Sub_UZU_Output){
			dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
		}else if(dc2h_in_sel == _Idomain_Sdnr_input){
			if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == VODMA_COLOR_RGB) && (RGB2YUV_ICH1_RGB2YUV_CTRL_get_en_rgb2yuv(IoReg_Read32(RGB2YUV_ICH1_RGB2YUV_CTRL_reg)) == 0)){
                dc2h_enrgb2yuv_state = DC2H_Colconv_RGBtoYUV;
            }else{
                dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
            }
		}else{
			dc2h_enrgb2yuv_state = DC2H_Colconv_RGBtoYUV;
		}
	}
    rtd_pr_vt_notice("cap_fmt %d, dc2h_in_sel=%d, dc2h_en_rgb2yuv=%d\n", cap_fmt, dc2h_in_sel, dc2h_enrgb2yuv_state);
    return dc2h_enrgb2yuv_state;
}

void set_dc2h2_r2y_422to444_ctrl(unsigned int pxlfmt, unsigned int location)
{
	dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_RBUS dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_ctrl_RBUS dc2h2_rgb2yuv_ctrl_reg;

	dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.regValue = IoReg_Read32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_422to444_CTRL_reg);
	// if dc2h input is yuv space && 422, capture format is rgb, set en_422to444
	if(location == _Idomain_Sdnr_input && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == VODMA_COLOR_YUV420 ||
	  Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == VODMA_COLOR_YUV422) && 
	  (RGB2YUV_ICH1_RGB2YUV_CTRL_get_en_rgb2yuv(IoReg_Read32(RGB2YUV_ICH1_RGB2YUV_CTRL_reg)) == 0) && 
	  dc2h2_cap_fmt_is_rgb_space(pxlfmt) == TRUE){
		dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.en_422to444 = 1;
		if(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_CTRL_get_sel_rgb(IoReg_Read32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_CTRL_reg))){
			//Use limitation: when 422to444 enable, sel_RGB should disable in RGB2YUV(422to444 ONLY In yuv color_space)
			dc2h2_rgb2yuv_ctrl_reg.regValue = IoReg_Read32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_CTRL_reg);
			dc2h2_rgb2yuv_ctrl_reg.sel_rgb = 0;
			IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_CTRL_reg, dc2h2_rgb2yuv_ctrl_reg.regValue);
		}
	}else{
		dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.en_422to444 = 0;
	}

	IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_422to444_CTRL_reg, dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.regValue);
}

void set_dc2h2_y2r_dither_output_bit(unsigned int pxlfmt)
{
	dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_RBUS dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_RBUS dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg;

	dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.regValue = IoReg_Read32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_422to444_CTRL_reg);
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.regValue = IoReg_Read32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_DITHER_CTRL1_reg);
	
	if(dc2h2_cap_fmt_is_10bit(pxlfmt) == TRUE){
		dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.dither_uzd_swap_en = 0; // dither->uzd
		dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.dither_bit_sel = 1; // dither output 10bit
	}else{
		dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.dither_uzd_swap_en = 1; // uzd->dither
		dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.dither_bit_sel = 0; // dither output 8bit
	}
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.dither_en = 1;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.temporal_enable = 0;

	IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_422to444_CTRL_reg, dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.regValue);
	IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_DITHER_CTRL1_reg, dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.regValue);
}

void set_dc2h2_r2y_dither_444to422_ctrl(unsigned int pxlfmt)
{
	dc2h2_r2y_dither_4xxto4xx_dc2h2_444to422_ctrl_RBUS dc2h2_r2y_dither_4xxto4xx_dc2h2_444to422_ctrl_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_444to422_ctrl_reg.regValue = IoReg_Read32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_444to422_CTRL_reg);

	if(dc2h2_cap_pixel_encoding_is_444(pxlfmt) == TRUE){
		dc2h2_r2y_dither_4xxto4xx_dc2h2_444to422_ctrl_reg.en_444to422 = 0;
	}else{
		dc2h2_r2y_dither_4xxto4xx_dc2h2_444to422_ctrl_reg.en_444to422 = 1;
	}
	IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_444to422_CTRL_reg, dc2h2_r2y_dither_4xxto4xx_dc2h2_444to422_ctrl_reg.regValue);
}

void drvif_set_dc2h2_r2y_config(unsigned int pxlfmt, unsigned int location)
{
	unsigned short *table_index = 0;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_ctrl_RBUS dc2h2_rgb2yuv_ctrl_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_tab_m11_m12_RBUS dc2h2_rgb2yuv_tab_m11_m12_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_tab_m13_m21_RBUS dc2h2_rgb2yuv_tab_m13_m21_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_tab_m22_m23_RBUS dc2h2_rgb2yuv_tab_m22_m23_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_tab_m31_m32_RBUS dc2h2_rgb2yuv_tab_m31_m32_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_tab_m33_ygain_RBUS dc2h2_rgb2yuv_tab_m33_ygain_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_tab_yo_RBUS dc2h2_rgb2yuv_tab_yo_reg;

	if(dc2h2_cap_fmt_is_rgb_space(pxlfmt)){ //y2r
		table_index = tYUV2RGB_COEF_709_RGB_0_255;
	}else{
		table_index = tYUV2RGB_COEF_709_RGB_16_235; //r2y
	}

	//main  all tab-1
	dc2h2_rgb2yuv_tab_m11_m12_reg.m11 = table_index [_RGB2YUV_m11];
	dc2h2_rgb2yuv_tab_m11_m12_reg.m12 = table_index [_RGB2YUV_m12];
	rtd_outl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_M11_M12_reg, dc2h2_rgb2yuv_tab_m11_m12_reg.regValue);

	dc2h2_rgb2yuv_tab_m13_m21_reg.m21 = table_index [_RGB2YUV_m21];
	dc2h2_rgb2yuv_tab_m13_m21_reg.m13 = table_index [_RGB2YUV_m13];
	rtd_outl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_M13_M21_reg, dc2h2_rgb2yuv_tab_m13_m21_reg.regValue);

	dc2h2_rgb2yuv_tab_m22_m23_reg.m22 = table_index [_RGB2YUV_m22];
	dc2h2_rgb2yuv_tab_m22_m23_reg.m23 = table_index [_RGB2YUV_m23];
	rtd_outl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_M22_M23_reg, dc2h2_rgb2yuv_tab_m22_m23_reg.regValue);

	dc2h2_rgb2yuv_tab_m31_m32_reg.m31 = table_index [_RGB2YUV_m31];
	dc2h2_rgb2yuv_tab_m31_m32_reg.m32 = table_index [_RGB2YUV_m32];
	rtd_outl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_M31_M32_reg, dc2h2_rgb2yuv_tab_m31_m32_reg.regValue);

	dc2h2_rgb2yuv_tab_m33_ygain_reg.m33 = table_index [_RGB2YUV_m33];
	dc2h2_rgb2yuv_tab_m33_ygain_reg.y_gain = table_index [_RGB2YUV_Y_gain];
	rtd_outl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_M33_YGain_reg, dc2h2_rgb2yuv_tab_m33_ygain_reg.regValue);

	dc2h2_rgb2yuv_tab_yo_reg.yo_even = table_index [_RGB2YUV_Yo_even];
	dc2h2_rgb2yuv_tab_yo_reg.yo_odd = table_index [_RGB2YUV_Yo_odd];
	rtd_outl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_Yo_reg, dc2h2_rgb2yuv_tab_yo_reg.regValue);

	dc2h2_rgb2yuv_ctrl_reg.regValue = IoReg_Read32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_CTRL_reg);
	dc2h2_rgb2yuv_ctrl_reg.sel_rgb = table_index [_RGB2YUV_sel_RGB];
	dc2h2_rgb2yuv_ctrl_reg.set_r_in_offset = table_index [_RGB2YUV_set_R_in_offset];
	dc2h2_rgb2yuv_ctrl_reg.set_uv_out_offset = table_index [_RGB2YUV_set_UV_out_offset];
	dc2h2_rgb2yuv_ctrl_reg.sel_uv_off = table_index [_RGB2YUV_sel_UV_off];
	dc2h2_rgb2yuv_ctrl_reg.matrix_bypass = table_index [_RGB2YUV_Matrix_bypass];
	dc2h2_rgb2yuv_ctrl_reg.sel_y_gain = table_index [_RGB2YUV_Enable_Y_gain];
	dc2h2_rgb2yuv_ctrl_reg.en_rgb2yuv = set_dc2h2_colconv_enable(pxlfmt);

	IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_CTRL_reg, dc2h2_rgb2yuv_ctrl_reg.regValue);

	set_dc2h2_r2y_422to444_ctrl(pxlfmt, location);
	set_dc2h2_y2r_dither_output_bit(pxlfmt);
	set_dc2h2_r2y_dither_444to422_ctrl(pxlfmt);
}

void set_dc2h2_uzd_scale_down_coef_tab(KADP_VT_RECT_T in_size, KADP_VT_RECT_T out_size)
{
	unsigned int SDRatio;
	unsigned int SDFilter=0;
	unsigned int tmp_data;
	short *coef_pt;
	unsigned int i;
	unsigned int nFactor;
	unsigned int D = 0;
	unsigned char Hini, Vini, a;
	unsigned short S1,S2;

	dc2h2_scaledown_dc2h2_uzd_ctrl0_RBUS dc2h2_uzd_ctrl0_reg;
	dc2h2_scaledown_dc2h2_uzd_ctrl1_RBUS dc2h2_uzd_ctrl1_reg;

	dc2h2_scaledown_dc2h2_uzd_scale_hor_factor_RBUS dc2h2_uzd_hor_factor_reg;
	dc2h2_scaledown_dc2h2_uzd_scale_ver_factor_RBUS dc2h2_uzd_ver_factor_reg;

	dc2h2_scaledown_dc2h2_uzd_initial_value_RBUS dc2h2_uzd_initial_value_reg;
	dc2h2_scaledown_dc2h2_uzd_initial_int_value_RBUS dc2h2_uzd_initial_int_value_reg;

	dc2h2_uzd_ctrl0_reg.regValue = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL0_reg);
	dc2h2_uzd_ctrl1_reg.regValue = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL1_reg);

	dc2h2_uzd_hor_factor_reg.regValue  = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_SCALE_HOR_FACTOR_reg);
	dc2h2_uzd_ver_factor_reg.regValue  = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_SCALE_VER_FACTOR_reg);

	dc2h2_uzd_initial_value_reg.regValue  = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_INITIAL_VALUE_reg);
	dc2h2_uzd_initial_int_value_reg.regValue  = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_INITIAL_INT_VALUE_reg);

	//o============ H scale-down=============o
	if (in_size.w > out_size.w)
	{
		// o-------calculate scaledown ratio to select one of different bandwith filters.--------o
		if ( out_size.w == 0) {
			rtd_pr_vt_debug("output width = 0 !!!\n");
			SDRatio = 0;
		} else {
			SDRatio = (in_size.w * TMPMUL) / out_size.w;
		}

		if(SDRatio <= ((TMPMUL*3)/2))  //<1.5 sharp, wider bw
			SDFilter = 2;
		else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) )  // Mid
			SDFilter = 1;
		else    // blurest, narrow bw
			SDFilter = 0;

		//o---------------- fill coefficients into access port--------------------o
		coef_pt = tScaleDown_COEF_TAB[SDFilter];

		dc2h2_uzd_ctrl0_reg.h_y_table_sel = 0;  // TAB1
		dc2h2_uzd_ctrl0_reg.h_c_table_sel = 0;  // TAB1

		for (i=0; i<16; i++)
		{
			tmp_data = ((unsigned int)(*coef_pt++)<<16);
			tmp_data += (unsigned int)(*coef_pt++);
			rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_FIR_COEF_TAB1_C0_reg + i*4, tmp_data);
		}
	}

	//o============ V scale-down=============o
	if (in_size.h > out_size.h)
	{
		// o-------calculate scaledown ratio to select one of different bandwith filters.--------o
		if ( out_size.h == 0 ) {
			SDRatio = 0;
		} else {
			SDRatio = (in_size.h * TMPMUL) /out_size.h;
		}	

		if(SDRatio <= ((TMPMUL*3)/2))  //<1.5 sharp, wider bw
			SDFilter = 2;
		else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) )  // Mid
			SDFilter = 1;
		else    // blurest, narrow bw
			SDFilter = 0;

		rtd_pr_vt_debug("filter number=%d\n",SDFilter);

		//o---------------- fill coefficients into access port--------------------o
		 coef_pt = tScaleDown_COEF_TAB[SDFilter];

		dc2h2_uzd_ctrl0_reg.v_y_table_sel = 1;  // TAB2
		dc2h2_uzd_ctrl0_reg.v_c_table_sel = 1;  // TAB2

		for (i=0; i<16; i++)
		{
			tmp_data = ((unsigned int)(*coef_pt++)<<16);
			tmp_data += (unsigned int)(*coef_pt++);
			rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_FIR_COEF_TAB2_C0_reg + i*4, tmp_data);
		}
	} else {
		//no need  V scale-down, use bypass filter
	}

	set_dc2h_uzd_coef_table_db_apply(DC2H2);
	// Decide Output data format for scale down
	//============H Scale-down============
	if (in_size.w > out_size.w)
	{
		Hini = 0x80;
		dc2h2_uzd_initial_value_reg.hor_ini = Hini;
		dc2h2_uzd_initial_int_value_reg.hor_ini_int = 1;
		a = 5;

		if(0/*panorama*/)
		{// CSW+ 0960830 Non-linear SD
			S1 = (out_size.w) >> 2;
			S2 = (out_size.w) - S1*2;
			//=vv==old from TONY, if no problem, use this=vv==
			nFactor = (unsigned int)(((in_size.w)<<21) - ((unsigned int)Hini<<13));

			//nFactor = ((((unsigned long long)(ptInSize->nWidth))*(ptOutSize->nWidth-1))<<21) / ptOutSize->nWidth;
			//===================
			nFactor = nFactor /((2*a+1)*S1 + (a+1)*S2 - a) * a;
			if(S1 != 0)
				D = nFactor / a / S1;
			nFactor = SHR((nFactor + 1), 1); //rounding
			D = SHR(D + 1, 1); //rounding
			rtd_pr_vt_debug("\nPANORAMA2 TEST 111\n");
			rtd_pr_vt_debug("nPANORAMA2 Factor = %d\n",nFactor);
			rtd_pr_vt_debug("PANORAMA2 S1=%d, S2=%d, D = %d\n",S1, S2, D);

			if(nFactor < 1048576) {
				rtd_pr_vt_debug("PANORAMA2 Can't do nonlinear SD \n");
				//nFactor = (unsigned int)(((ptInSize->nWidth)<<21) - ((unsigned int)Hini<<13)) / (ptOutSize->nWidth-1);
				if(out_size.w != 0)
					nFactor = (unsigned int)(((in_size.w)<<21)) / (out_size.w);
				nFactor = SHR(nFactor + 1, 1); //rounding
				rtd_pr_vt_debug("PANORAMA2 nFactor=%x, input Wid=%d, Out Wid=%d\n",nFactor, in_size.w, out_size.w);
				S1 = 0;
				S2 = out_size.w;
				D = 0;
			}
        }else{
			if (out_size.w == 0 ) {
				rtd_pr_vt_debug("output width = 0 !!!\n");
				nFactor = 0;
			}else if(in_size.w > 4095){
				//nFactor = (unsigned int)((((in_size.w)<<19) / (out_size.w))<<1);
				nFactor = (unsigned int)(in_size.w * 524288) / (out_size.w) * 2;
			}else if(in_size.w > 2047){
				nFactor = (unsigned int)(in_size.w * 1048576) / (out_size.w); // 2^20 = 1048576
			}else{
				//nFactor = (unsigned int)((in_size.w<<21)) / (out_size.w);
				nFactor = (unsigned int)(in_size.w * 2097152) / (out_size.w);
				nFactor = SHR(nFactor + 1, 1); //rounding
			}

			/*else{
				tmp = ((unsigned long long)(in_size.w) << 20);
				do_div(tmp, (out_size.w));
				//nFactor = SHR(nFactor + 1, 1); //rounding
				nFactor = tmp;
			}*/
		}
	}else {
		nFactor = 0x100000;
	}

	dc2h2_uzd_hor_factor_reg.hor_fac = nFactor;
	rtd_pr_vt_notice("hfactor=%d, in_wid=%d, out_wid=%d\n",nFactor, in_size.w, out_size.w);

	//================V Scale-down=================
	if (in_size.h > out_size.h) {    // V scale-down
		Vini = 0x80;//0xff;
		dc2h2_uzd_initial_value_reg.ver_ini = Vini;
		dc2h2_uzd_initial_int_value_reg.ver_ini_int = 1;//0;

		if ( out_size.h == 0 ) {
			rtd_pr_vt_debug("output length = 0 !!!\n");
			nFactor = 0;
		} else {
			nFactor = (unsigned int)((in_size.h * 1024 * 1024)) / (out_size.h);
			// tmp = ((unsigned long long)(in_size.h) << 20);
			// do_div(tmp, (out_size.h));
			// nFactor = tmp;
		}
	} else {
		nFactor = 0x100000;
	}

	dc2h2_uzd_ver_factor_reg.ver_fac = nFactor;
	rtd_pr_vt_notice("vfactor=%d,in_h=%d, out_h=%d\n", nFactor, in_size.h, out_size.h);

	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL0_reg, dc2h2_uzd_ctrl0_reg.regValue);
	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL1_reg, dc2h2_uzd_ctrl1_reg.regValue);

	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_SCALE_HOR_FACTOR_reg, dc2h2_uzd_hor_factor_reg.regValue);
	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_SCALE_VER_FACTOR_reg, dc2h2_uzd_ver_factor_reg.regValue);

	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_INITIAL_VALUE_reg, dc2h2_uzd_initial_value_reg.regValue);
	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_INITIAL_INT_VALUE_reg, dc2h2_uzd_initial_int_value_reg.regValue);
}

void drvif_set_dc2h2_scale_down_config(KADP_VT_RECT_T in_size, KADP_VT_RECT_T out_size)
{
	dc2h2_scaledown_dc2h2_uzd_ctrl0_RBUS dc2h2_uzd_ctrl0_reg;
	dc2h2_scaledown_dc2h2_uzd_ctrl1_RBUS dc2h2_uzd_ctrl1_reg;
	dc2h2_scaledown_dc2h2_uzd_444to422_ctrl_RBUS dc2h2_uzd_444to422_ctrl_reg;
	dc2h2_scaledown_dc2h2_uzd_ibuff_ctrl_RBUS dc2h2_uzd_ibuff_ctrl_reg;

	dc2h2_uzd_444to422_ctrl_reg.regValue = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_444to422_CTRL_reg);
	dc2h2_uzd_444to422_ctrl_reg.cbcr_swap_en = 0;
	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_444to422_CTRL_reg, dc2h2_uzd_444to422_ctrl_reg.regValue);

	dc2h2_uzd_ctrl1_reg.regValue = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL1_reg);
	dc2h2_uzd_ctrl1_reg.uzd_htotal = get_dc2h_uzd_htotal_by_in_sel(curCapInfo[DC2H2].cap_location);
	dc2h2_uzd_ctrl1_reg.auto_four_tap_en = 1;
	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL1_reg, dc2h2_uzd_ctrl1_reg.regValue);

	dc2h2_uzd_ibuff_ctrl_reg.regValue = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_IBUFF_CTRL_reg);
	dc2h2_uzd_ibuff_ctrl_reg.ibuf_h_size = out_size.w;
	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_IBUFF_CTRL_reg, dc2h2_uzd_ibuff_ctrl_reg.regValue);

	dc2h2_uzd_ctrl0_reg.regValue = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL0_reg);
	dc2h2_uzd_ctrl0_reg.h_zoom_en = (in_size.w > out_size.w);
	dc2h2_uzd_ctrl0_reg.v_zoom_en = (in_size.h > out_size.h);

	dc2h2_uzd_ctrl0_reg.out_bit = 1; //uzd output always 10bit
	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL0_reg, dc2h2_uzd_ctrl0_reg.regValue);

	set_dc2h2_uzd_scale_down_coef_tab(in_size, out_size);
}

void set_dc2h2_sequence_cap(VT_CUR_CAPTURE_INFO *dc2h_ctrl_info)
{
	int cap0_total_bit = 0;
	int cap0_burst_num = 0;
	int bitperpixel = 0;

	unsigned int dc2h_cap_mode = get_dc2h_dma_cap_mode(DC2H2);
	dc2h2_cap_dc2h2_cap0_dma_wr_ctrl_RBUS dc2h2_cap0_dma_wr_ctrl_reg;
    dc2h2_cap_dc2h2_cap_size_RBUS dc2h2_cap_size_reg;//w,h
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	// dc2h2_cap_dc2h2_interrupt_enable_RBUS dc2h2_interrupt_enable_reg;

	if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB) {
		bitperpixel = 24;
	}else{
		bitperpixel = 32;
	}

	dc2h_ctrl_info->output_size.w  = (dc2h_ctrl_info->output_size.w+1)/2*2;
	rtd_pr_vt_notice("cap width align to even %d\n", dc2h_ctrl_info->output_size.w);

	if(dc2h_cap_mode == DC2H_CAP_FRAME_MODE){
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * dc2h_ctrl_info->output_size.h;
	}else{
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
	}

	cap0_burst_num = ((cap0_total_bit + 127) / 128);
	
	dc2h2_cap_size_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Size_reg);
	dc2h2_cap_size_reg.dc2h2_dma_hact = dc2h_ctrl_info->output_size.w;//should be even
    dc2h2_cap_size_reg.dc2h2_dma_vact = dc2h_ctrl_info->output_size.h;
    rtd_outl(DC2H2_CAP_DC2H2_CAP_Size_reg, dc2h2_cap_size_reg.regValue);
	
    dc2h2_cap0_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg);
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_1byte_swap = 0;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_2byte_swap = 0;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_4byte_swap = 0;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 0;

	if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB){
		set_dc2h2_cap_color_fmt_rgb(dc2h_cap_mode);
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_1byte_swap = 1;
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_2byte_swap = 1;
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_4byte_swap = 1;
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;
	}else if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ARGB){
		set_dc2h2_cap_color_fmt_argb(CAP_ARGB, dc2h_cap_mode);
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;
	}else if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGBA){
		set_dc2h2_cap_color_fmt_argb(CAP_RGBA, dc2h_cap_mode);
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;
	}else{
		rtd_pr_vt_notice("[VT]unspport capture RAGB,RGAB\n");
		set_dc2h2_cap_color_fmt_argb(CAP_ARGB, dc2h_cap_mode);
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;
	}
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg, dc2h2_cap0_dma_wr_ctrl_reg.regValue);
	set_dc2h2_cap0_line_burst_num(cap0_burst_num, dc2h_ctrl_info->output_size.h, dc2h_cap_mode);

	dc2h2_cap_color_format_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
	dc2h2_cap_color_format_reg.dc2h2_cap0_en = 1;
	dc2h2_cap_color_format_reg.dc2h2_cap1_en = 0;
	dc2h2_cap_color_format_reg.dc2h2_cap2_en = 0;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, dc2h2_cap_color_format_reg.regValue);
}

void set_dc2h2_nv12_cap(VT_CUR_CAPTURE_INFO *dc2h_ctrl_info)
{
	int cap0_total_bit = 0;
	int cap1_total_bit = 0;
	int cap0_burst_num = 0;
	int cap1_burst_num = 0;

	int bitperpixel = 0;
	VT_CAP_BIT_SEL bit_sel = PER_CHANNEL_8_BIT;

	int odd_line = dc2h_ctrl_info->output_size.h % 2;
	unsigned int dc2h_cap_mode = get_dc2h_dma_cap_mode(DC2H2);

    dc2h2_cap_dc2h2_cap0_dma_wr_ctrl_RBUS dc2h2_cap0_dma_wr_ctrl_reg;
	dc2h2_cap_dc2h2_cap1_dma_wr_ctrl_RBUS dc2h2_cap1_dma_wr_ctrl_reg;

    dc2h2_cap_dc2h2_cap_size_RBUS dc2h2_cap_size_reg;//w,h
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	// dc2h2_cap_dc2h2_interrupt_enable_RBUS dc2h2_interrupt_enable_reg;

	dc2h_ctrl_info->output_size.w  = (dc2h_ctrl_info->output_size.w+1)/2*2;
	rtd_pr_vt_notice("[VT]cap width align to even %d\n", dc2h_ctrl_info->output_size.w);

	if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit || dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV21){
		bitperpixel = 8;
		bit_sel = PER_CHANNEL_8_BIT;
	}else{
		bitperpixel = 10;
		bit_sel = PER_CHANNEL_10_BIT;
	}

	if(dc2h_cap_mode == DC2H_CAP_FRAME_MODE){
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * dc2h_ctrl_info->output_size.h;
		cap1_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * (dc2h_ctrl_info->output_size.h / 2 + odd_line);
	}else{
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
		cap1_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
	}

	cap0_burst_num = ((cap0_total_bit + 127) / 128);
	cap1_burst_num = ((cap1_total_bit + 127) / 128);
	
	dc2h2_cap_size_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Size_reg);
	dc2h2_cap_size_reg.dc2h2_dma_hact = dc2h_ctrl_info->output_size.w;
    dc2h2_cap_size_reg.dc2h2_dma_vact = dc2h_ctrl_info->output_size.h;
    rtd_outl(DC2H2_CAP_DC2H2_CAP_Size_reg, dc2h2_cap_size_reg.regValue);

    dc2h2_cap0_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg);
	dc2h2_cap1_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP1_DMA_WR_Ctrl_reg);

	if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit || dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_10bit){
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_1byte_swap = 1;
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_2byte_swap = 1;
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_4byte_swap = 1;
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;

		dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_1byte_swap = 1; //UV
		dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_2byte_swap = 1;
		dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_4byte_swap = 1;
		dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_8byte_swap = 1;
		set_dc2h2_cap_color_fmt_nv12(bit_sel, dc2h_cap_mode, DC2H_CB_CR_NO_SWAP);
	}else if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV21){
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_1byte_swap = 1;
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_2byte_swap = 1;
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_4byte_swap = 1;
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;

		dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_1byte_swap = 1;//vU
		dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_2byte_swap = 1;
		dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_4byte_swap = 1;
		dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_8byte_swap = 1;
		set_dc2h2_cap_color_fmt_nv12(bit_sel, dc2h_cap_mode, DC2H_CB_CR_SWAP);
	}
    rtd_outl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg, dc2h2_cap0_dma_wr_ctrl_reg.regValue);
	rtd_outl(DC2H2_CAP_DC2H2_CAP1_DMA_WR_Ctrl_reg, dc2h2_cap1_dma_wr_ctrl_reg.regValue);

	set_dc2h2_cap0_line_burst_num(cap0_burst_num, dc2h_ctrl_info->output_size.h, dc2h_cap_mode);
	set_dc2h2_cap1_line_burst_num(cap1_burst_num, dc2h_ctrl_info->output_size.h / 2, dc2h_cap_mode);

	dc2h2_cap_color_format_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
	dc2h2_cap_color_format_reg.dc2h2_cap0_en = 1; //y
	dc2h2_cap_color_format_reg.dc2h2_cap1_en = 1; //UV
	dc2h2_cap_color_format_reg.dc2h2_cap2_en = 0;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, dc2h2_cap_color_format_reg.regValue);
}

void set_dc2h2_p010_cap(VT_CUR_CAPTURE_INFO *dc2h_ctrl_info)
{
	int cap0_total_bit = 0;
	int cap1_total_bit = 0;
	int cap0_burst_num = 0;
	int cap1_burst_num = 0;

	int bitperpixel = 16;

	int odd_line = dc2h_ctrl_info->output_size.h % 2;
	unsigned int dc2h_cap_mode = get_dc2h_dma_cap_mode(DC2H2);

    dc2h2_cap_dc2h2_cap0_dma_wr_ctrl_RBUS dc2h2_cap0_dma_wr_ctrl_reg;
	dc2h2_cap_dc2h2_cap1_dma_wr_ctrl_RBUS dc2h2_cap1_dma_wr_ctrl_reg;

    dc2h2_cap_dc2h2_cap_size_RBUS dc2h2_cap_size_reg;//w,h
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	// dc2h2_cap_dc2h2_interrupt_enable_RBUS dc2h2_interrupt_enable_reg;

	dc2h_ctrl_info->output_size.w  = (dc2h_ctrl_info->output_size.w+1)/2*2;
	rtd_pr_vt_notice("[VT]cap width align to even %d\n", dc2h_ctrl_info->output_size.w);

	if(dc2h_cap_mode == DC2H_CAP_FRAME_MODE){
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * dc2h_ctrl_info->output_size.h;
		cap1_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * (dc2h_ctrl_info->output_size.h / 2 + odd_line);
	}else{
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
		cap1_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
	}

	cap0_burst_num = ((cap0_total_bit + 127) / 128);
	cap1_burst_num = ((cap1_total_bit + 127) / 128);
	
	dc2h2_cap_size_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Size_reg);
	dc2h2_cap_size_reg.dc2h2_dma_hact = dc2h_ctrl_info->output_size.w;
    dc2h2_cap_size_reg.dc2h2_dma_vact = dc2h_ctrl_info->output_size.h;
    rtd_outl(DC2H2_CAP_DC2H2_CAP_Size_reg, dc2h2_cap_size_reg.regValue);

    dc2h2_cap0_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg);
	dc2h2_cap1_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP1_DMA_WR_Ctrl_reg);

	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_1byte_swap = 1;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_2byte_swap = 1;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_4byte_swap = 1;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;

	dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_1byte_swap = 1; //VU
	dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_2byte_swap = 1;
	dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_4byte_swap = 1;
	dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_8byte_swap = 1;
	
    rtd_outl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg, dc2h2_cap0_dma_wr_ctrl_reg.regValue);
	rtd_outl(DC2H2_CAP_DC2H2_CAP1_DMA_WR_Ctrl_reg, dc2h2_cap1_dma_wr_ctrl_reg.regValue);

	set_dc2h2_cap_color_fmt_p010(dc2h_cap_mode);

	set_dc2h2_cap0_line_burst_num(cap0_burst_num, dc2h_ctrl_info->output_size.h, dc2h_cap_mode);
	set_dc2h2_cap1_line_burst_num(cap1_burst_num, dc2h_ctrl_info->output_size.h / 2, dc2h_cap_mode);

	dc2h2_cap_color_format_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
	dc2h2_cap_color_format_reg.dc2h2_cap0_en = 1;
	dc2h2_cap_color_format_reg.dc2h2_cap1_en = 1;
	dc2h2_cap_color_format_reg.dc2h2_cap2_en = 0;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, dc2h2_cap_color_format_reg.regValue);
}

void set_dc2h2_yv12_i420_cap(VT_CUR_CAPTURE_INFO *dc2h_ctrl_info)
{
	int cap0_total_bit = 0;
	int cap1_total_bit = 0;
	int cap2_total_bit = 0;
	int cap0_burst_num = 0;
	int cap1_burst_num = 0;
	int cap2_burst_num = 0;

	int bitperpixel = 8;
	int odd_line = dc2h_ctrl_info->output_size.h % 2;

	unsigned int dc2h_cap_mode = get_dc2h_dma_cap_mode(DC2H2);

    dc2h2_cap_dc2h2_cap0_dma_wr_ctrl_RBUS dc2h2_cap0_dma_wr_ctrl_reg;
	dc2h2_cap_dc2h2_cap1_dma_wr_ctrl_RBUS dc2h2_cap1_dma_wr_ctrl_reg;
	dc2h2_cap_dc2h2_cap2_dma_wr_ctrl_RBUS dc2h2_cap2_dma_wr_ctrl_reg;

    dc2h2_cap_dc2h2_cap_size_RBUS dc2h2_cap_size_reg;//w,h
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	// dc2h2_cap_dc2h2_interrupt_enable_RBUS dc2h2_interrupt_enable_reg;

	dc2h_ctrl_info->output_size.w  = (dc2h_ctrl_info->output_size.w+1)/2*2;
	rtd_pr_vt_notice("[VT]cap width align to even %d\n", dc2h_ctrl_info->output_size.w);

	if(dc2h_cap_mode == DC2H_CAP_FRAME_MODE){
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * dc2h_ctrl_info->output_size.h;
		cap1_total_bit = bitperpixel * (dc2h_ctrl_info->output_size.w >> 1) * (dc2h_ctrl_info->output_size.h / 2 + odd_line);
		cap2_total_bit = bitperpixel * (dc2h_ctrl_info->output_size.w >> 1) * (dc2h_ctrl_info->output_size.h / 2 + odd_line);
	}else{
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
		cap1_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w / 2;
		cap2_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w / 2;
	}

	cap0_burst_num = ((cap0_total_bit + 127) / 128);
	cap1_burst_num = ((cap1_total_bit + 127) / 128);
	cap2_burst_num = ((cap1_total_bit + 127) / 128);
	
	dc2h2_cap_size_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Size_reg);
	dc2h2_cap_size_reg.dc2h2_dma_hact = dc2h_ctrl_info->output_size.w;
    dc2h2_cap_size_reg.dc2h2_dma_vact = dc2h_ctrl_info->output_size.h;
    rtd_outl(DC2H2_CAP_DC2H2_CAP_Size_reg, dc2h2_cap_size_reg.regValue);

    dc2h2_cap0_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg);
	dc2h2_cap1_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP1_DMA_WR_Ctrl_reg);
	dc2h2_cap2_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP2_DMA_WR_Ctrl_reg);

	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_1byte_swap = 1; //Y
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_2byte_swap = 1;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_4byte_swap = 1;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;

	dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_1byte_swap = 1; //V
	dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_2byte_swap = 1;
	dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_4byte_swap = 1;
	dc2h2_cap1_dma_wr_ctrl_reg.dc2h2_cap1_dma_8byte_swap = 1;

	dc2h2_cap2_dma_wr_ctrl_reg.dc2h2_cap2_dma_1byte_swap = 1; //U
	dc2h2_cap2_dma_wr_ctrl_reg.dc2h2_cap2_dma_2byte_swap = 1;
	dc2h2_cap2_dma_wr_ctrl_reg.dc2h2_cap2_dma_4byte_swap = 1;
	dc2h2_cap2_dma_wr_ctrl_reg.dc2h2_cap2_dma_8byte_swap = 1;
	
    rtd_outl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg, dc2h2_cap0_dma_wr_ctrl_reg.regValue);
	rtd_outl(DC2H2_CAP_DC2H2_CAP1_DMA_WR_Ctrl_reg, dc2h2_cap1_dma_wr_ctrl_reg.regValue);
	rtd_outl(DC2H2_CAP_DC2H2_CAP2_DMA_WR_Ctrl_reg, dc2h2_cap2_dma_wr_ctrl_reg.regValue);
	
	if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YV12){
		set_dc2h2_cap_color_fmt_yv12(dc2h_cap_mode);
	}else{
		set_dc2h2_cap_color_fmt_i420(dc2h_cap_mode);
	}

	set_dc2h2_cap0_line_burst_num(cap0_burst_num, dc2h_ctrl_info->output_size.h, dc2h_cap_mode);
	set_dc2h2_cap1_line_burst_num(cap1_burst_num, dc2h_ctrl_info->output_size.h / 2, dc2h_cap_mode);
	set_dc2h2_cap2_line_burst_num(cap2_burst_num, dc2h_ctrl_info->output_size.h / 2, dc2h_cap_mode);

	dc2h2_cap_color_format_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
	dc2h2_cap_color_format_reg.dc2h2_cap0_en = 1;
	dc2h2_cap_color_format_reg.dc2h2_cap1_en = 1;
	dc2h2_cap_color_format_reg.dc2h2_cap2_en = 1;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, dc2h2_cap_color_format_reg.regValue);
}

int get_dc2h2_cap_fmt_plane_number(unsigned int cap_fmt)
{
	int plane_number = 0;
	if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB && cap_fmt <= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ABGR){
		plane_number = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED;
	}else if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit && cap_fmt <= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV21){
		plane_number = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR;
	}else if(cap_fmt >= V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YV12 && cap_fmt < V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_MAXN){
		plane_number = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_PLANAR;
	}else{
		plane_number = V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED;
	}

	return plane_number;
}

void set_dc2h2_cap_buf_property(VT_CUR_CAPTURE_INFO *dc2h_ctrl_info)
{
	CapBuffer_VT[DC2H2].pixelFormat = dc2h_ctrl_info->fmt;
	CapBuffer_VT[DC2H2].plane_number = get_dc2h2_cap_fmt_plane_number(dc2h_ctrl_info->fmt);
	CapBuffer_VT[DC2H2].vtbuffernumber = get_vt_VtBufferNum(DC2H2);
	CapBuffer_VT[DC2H2].stride = drvif_memory_get_data_align(dc2h_ctrl_info->output_size.w, 16);
	CapBuffer_VT[DC2H2].width = dc2h_ctrl_info->output_size.w;
	CapBuffer_VT[DC2H2].height = dc2h_ctrl_info->output_size.h;
}

void disable_dc2h2_capture(void)
{
	dc2h2_cap_dc2h2_ctrl_RBUS dc2h2_cap_ctrl_reg;
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	dc2h2_cap_dc2h2_interrupt_enable_RBUS dc2h2_interrupt_enable_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_ctrl_RBUS dc2h2_rgb2yuv_ctrl_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_pq_db_ctrl_RBUS dc2h2_r2y_dither_4xxto4xx_dc2h2_pq_db_ctrl_reg;
	dc2h2_scaledown_dc2h2_uzd_coef_db_ctrl_RBUS dc2h2_scaledown_dc2h2_uzd_coef_db_ctrl_reg;
	dc2h2_scaledown_dc2h2_uzd_h_db_ctrl_RBUS dc2h2_scaledown_dc2h2_uzd_h_db_ctrl_reg;
	dc2h2_scaledown_dc2h2_uzd_v_db_ctrl_RBUS dc2h2_scaledown_dc2h2_uzd_v_db_ctrl_reg;
	dc2h2_scaledown_dc2h2_uzd_ctrl0_RBUS dc2h2_scaledown_dc2h2_uzd_ctrl0_reg;
	dc2h2_cap_dc2h2_pq_cmp_RBUS dc2h2_cap_dc2h2_pq_cmp_reg;
	dc2h2_cap_dc2h2_cap_doublebuffer_RBUS dc2h2_cap_dc2h2_cap_doublebuffer_reg;
	dc2h2_cap_dc2h2_3dmaskto2d_ctrl_RBUS dc2h2_cap_3dmaskto2d_ctrl_reg;

	if (get_vt_VtBufferNum(DC2H2) > 1){
		dc2h2_interrupt_enable_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_Interrupt_Enable_reg);
		dc2h2_interrupt_enable_reg.dc2h2_vs_rising_int_en = 0;
		dc2h2_interrupt_enable_reg.dc2h2_cap0_done_int_en = 0;
		dc2h2_interrupt_enable_reg.dc2h2_cap1_done_int_en = 0;
		dc2h2_interrupt_enable_reg.dc2h2_cap2_done_int_en = 0;
		rtd_outl(DC2H2_CAP_DC2H2_Interrupt_Enable_reg, dc2h2_interrupt_enable_reg.regValue);
		msleep(20);
	}

	dc2h2_r2y_dither_4xxto4xx_dc2h2_pq_db_ctrl_reg.regValue = rtd_inl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_PQ_DB_CTRL_reg);
	dc2h2_r2y_dither_4xxto4xx_dc2h2_pq_db_ctrl_reg.db_en = 0;
	rtd_outl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_PQ_DB_CTRL_reg, dc2h2_r2y_dither_4xxto4xx_dc2h2_pq_db_ctrl_reg.regValue);

	dc2h2_scaledown_dc2h2_uzd_coef_db_ctrl_reg.regValue = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_COEF_DB_CTRL_reg);
	dc2h2_scaledown_dc2h2_uzd_coef_db_ctrl_reg.coef_db_en = 0;
	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_COEF_DB_CTRL_reg, dc2h2_scaledown_dc2h2_uzd_coef_db_ctrl_reg.regValue);

	dc2h2_scaledown_dc2h2_uzd_h_db_ctrl_reg.regValue = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_H_DB_CTRL_reg);
	dc2h2_scaledown_dc2h2_uzd_h_db_ctrl_reg.h_db_en = 0;
	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_H_DB_CTRL_reg, dc2h2_scaledown_dc2h2_uzd_h_db_ctrl_reg.regValue);

	dc2h2_scaledown_dc2h2_uzd_v_db_ctrl_reg.regValue = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_V_DB_CTRL_reg);
	dc2h2_scaledown_dc2h2_uzd_v_db_ctrl_reg.v_db_en = 0;
	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_V_DB_CTRL_reg, dc2h2_scaledown_dc2h2_uzd_v_db_ctrl_reg.regValue);
	
	dc2h2_cap_dc2h2_cap_doublebuffer_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_doublebuffer_reg);
	dc2h2_cap_dc2h2_cap_doublebuffer_reg.dc2h2_db_en = 0;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_doublebuffer_reg, dc2h2_cap_dc2h2_cap_doublebuffer_reg.regValue);

	IoReg_ClearBits(DC2H2_CAP_DC2H2_3DMaskTo2D_doublebuffer_reg, DC2H2_CAP_DC2H2_3DMaskTo2D_doublebuffer_dc2h2_3dmaskto2d_db_en_mask);

	dc2h2_cap_3dmaskto2d_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_3DMaskTo2D_Ctrl_reg);
	dc2h2_cap_3dmaskto2d_ctrl_reg.dc2h2_3dmaskto2d_en = 0;
	rtd_outl(DC2H2_CAP_DC2H2_3DMaskTo2D_Ctrl_reg, dc2h2_cap_3dmaskto2d_ctrl_reg.regValue);

	dc2h2_cap_color_format_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
	dc2h2_cap_color_format_reg.dc2h2_cap0_en = 0;
	dc2h2_cap_color_format_reg.dc2h2_cap1_en = 0;
	dc2h2_cap_color_format_reg.dc2h2_cap2_en = 0;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, dc2h2_cap_color_format_reg.regValue);

	dc2h2_cap_dc2h2_pq_cmp_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_PQ_CMP_reg);
	dc2h2_cap_dc2h2_pq_cmp_reg.cmp_en = 0;
	rtd_outl(DC2H2_CAP_DC2H2_PQ_CMP_reg, dc2h2_cap_dc2h2_pq_cmp_reg.regValue);

	dc2h2_cap_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CTRL_reg);
	dc2h2_cap_ctrl_reg.dc2h2_in_sel = _NO_INPUT;
	rtd_outl(DC2H2_CAP_DC2H2_CTRL_reg, dc2h2_cap_ctrl_reg.regValue);

	dc2h2_rgb2yuv_ctrl_reg.regValue = rtd_inl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_CTRL_reg);
	dc2h2_rgb2yuv_ctrl_reg.en_rgb2yuv = 0;
	rtd_outl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_CTRL_reg, dc2h2_rgb2yuv_ctrl_reg.regValue);

	dc2h2_scaledown_dc2h2_uzd_ctrl0_reg.regValue = rtd_inl(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL0_reg);
	dc2h2_scaledown_dc2h2_uzd_ctrl0_reg.h_zoom_en = 0;
	dc2h2_scaledown_dc2h2_uzd_ctrl0_reg.v_zoom_en = 0;
	rtd_outl(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL0_reg, dc2h2_scaledown_dc2h2_uzd_ctrl0_reg.regValue);

    set_dc2h_capture_sdnr_in_clock(FALSE);
}

void resize_dc2h2_capture_size(VT_CUR_CAPTURE_INFO *pdc2h_ctrl_info)
{
	if(pdc2h_ctrl_info->output_size.w > pdc2h_ctrl_info->input_size.w){
		unsigned int temp = 0;
		rtd_pr_vt_emerg("output w > input w, need set output width max equal input width\n");
		temp = pdc2h_ctrl_info->input_size.w;
		if(temp%16 != 0){
			temp = (temp - temp%16);
		}
		pdc2h_ctrl_info->output_size.w = temp;
	}
	if(pdc2h_ctrl_info->output_size.h > pdc2h_ctrl_info->input_size.h){
		pdc2h_ctrl_info->output_size.h = pdc2h_ctrl_info->input_size.h;
		if(pdc2h_ctrl_info->output_size.h%2 != 0)
			pdc2h_ctrl_info->output_size.h = pdc2h_ctrl_info->output_size.h - 1;
	}
}

void set_dc2h2_isr_enable(VT_VIDEO_FRAME_BUFFER_PROPERTY_INFO_T *pdc2h_buf_info)
{
	if(get_vt_VtSwBufferMode(DC2H2) == TRUE){
		dc2h2_cap_dc2h2_interrupt_enable_RBUS dc2h2_interrupt_enable_reg;
		dc2h2_interrupt_enable_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_Interrupt_Enable_reg);

		if(pdc2h_buf_info->plane_number == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_PLANAR){
			dc2h2_interrupt_enable_reg.dc2h2_vs_rising_int_en = 1;
			dc2h2_interrupt_enable_reg.dc2h2_cap2_done_int_en = 0;
			dc2h2_interrupt_enable_reg.dc2h2_cap1_done_int_en = 0;
			dc2h2_interrupt_enable_reg.dc2h2_cap0_done_int_en = 1;
		}else if(pdc2h_buf_info->plane_number == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){
			dc2h2_interrupt_enable_reg.dc2h2_vs_rising_int_en = 1;
			dc2h2_interrupt_enable_reg.dc2h2_cap2_done_int_en = 0;
			dc2h2_interrupt_enable_reg.dc2h2_cap1_done_int_en = 0;
			dc2h2_interrupt_enable_reg.dc2h2_cap0_done_int_en = 1;
		}else{
			dc2h2_interrupt_enable_reg.dc2h2_vs_rising_int_en = 1; // one plane
			dc2h2_interrupt_enable_reg.dc2h2_cap2_done_int_en = 0;
			dc2h2_interrupt_enable_reg.dc2h2_cap1_done_int_en = 0;
			dc2h2_interrupt_enable_reg.dc2h2_cap0_done_int_en = 1;
		}
		rtd_outl(DC2H2_CAP_DC2H2_Interrupt_Enable_reg, dc2h2_interrupt_enable_reg.regValue);
	}
}

void drvif_dc2h2_capture_config(VT_CUR_CAPTURE_INFO *pdc2h_ctrl_info)
{
	if(pdc2h_ctrl_info == NULL){
		rtd_pr_vt_emerg("%s=%d pdc2h_ctrl_info is NULL!\n", __FUNCTION__, __LINE__);
		return;
	}
	if(pdc2h_ctrl_info->enable == TRUE)
	{
		set_dc2h2_in_size_and_in_sel(pdc2h_ctrl_info->cap_location);
		/* crop region initial to input size */
		//pdc2h_ctrl_info->crop_size = pdc2h_ctrl_info->input_size;
		//rtd_pr_vt_emerg("dc2h crop size (%d,%d,%d,%d)\n", pdc2h_ctrl_info->crop_size.x, pdc2h_ctrl_info->crop_size.y, pdc2h_ctrl_info->crop_size.w, pdc2h_ctrl_info->crop_size.h);
		resize_dc2h2_capture_size(pdc2h_ctrl_info);
		set_dc2h2_cap_buf_property(pdc2h_ctrl_info);
		rtd_pr_vt_emerg("output wxh=%dx%d\n", pdc2h_ctrl_info->output_size.w, pdc2h_ctrl_info->output_size.h);

		drvif_set_dc2h2_crop_config(0, 0, pdc2h_ctrl_info->input_size.w, pdc2h_ctrl_info->input_size.h, FALSE);
		drvif_set_dc2h2_r2y_config(pdc2h_ctrl_info->fmt, pdc2h_ctrl_info->cap_location);
		drvif_set_dc2h2_scale_down_config(pdc2h_ctrl_info->crop_size, pdc2h_ctrl_info->output_size);
		if(dc2h2_cap_fmt_is_rgb_space(pdc2h_ctrl_info->fmt) == TRUE){
			set_dc2h2_sequence_cap(pdc2h_ctrl_info);
		}else if(pdc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_8bit || pdc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV21 ||
			pdc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_NV12_10bit){
			set_dc2h2_nv12_cap(pdc2h_ctrl_info);
		}else if(pdc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_P010){
			set_dc2h2_p010_cap(pdc2h_ctrl_info);
		}else if(pdc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YV12 || pdc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_I420){
			set_dc2h2_yv12_i420_cap(pdc2h_ctrl_info);
		}else{
			rtd_pr_vt_emerg("%s=%d invalid pixel format!\n", __FUNCTION__, __LINE__);
			return;
		}

		if(get_vt_VtSwBufferMode(DC2H2) == TRUE){
			if(FALSE == drvif_set_dc2h2_swmode_inforpc(TRUE, &CapBuffer_VT[DC2H2])){
				rtd_pr_vt_emerg("%s=%d RPC fail so not setting dc2h2 and return!\n", __FUNCTION__, __LINE__);
				return;
			}
		}
	
		set_dc2h2_isr_enable(&CapBuffer_VT[DC2H2]);//isr enable
	}else{
		/* stop dc2h hw capture */
		disable_dc2h2_capture();
		if(get_vt_VtSwBufferMode(DC2H2) == TRUE){
			if(FALSE == drvif_set_dc2h2_swmode_inforpc(FALSE, &CapBuffer_VT[DC2H2])){
				rtd_pr_vt_emerg("%s=%d RPC fail so not setting dc2h2 and return!\n", __FUNCTION__, __LINE__);
				return;
			}
		}
	}
}
//-------------------------------dc2h2 pqc---------------------------------------------------------------------------------
static unsigned int calc_cmp_line_sum_bit(unsigned int comp_wid, unsigned int comp_ratio)
{
	unsigned short cmp_line_sum_bit = 0, sub_pixel_num = 0;
	unsigned int cmp_line_sum_bit_pure_rgb, cmp_line_sum_bit_pure_a, cmp_line_sum_bit_32_dummy, cmp_line_sum_bit_128_dummy, cmp_line_sum_bit_real;
	unsigned int cmp_width_div32 = 0, frame_limit_bit = 0;

	if((comp_wid % 32) != 0){
		//drop bits
		comp_wid = comp_wid + (32 - (comp_wid % 32));
	}

	frame_limit_bit = comp_ratio << 2;
	cmp_width_div32 = comp_wid / 32;

	sub_pixel_num = 3;
	cmp_line_sum_bit_pure_rgb = (cmp_width_div32 * 32) * (frame_limit_bit>>2);
	cmp_line_sum_bit_pure_a = 0;
	cmp_line_sum_bit_32_dummy = (6+1) * 32 * sub_pixel_num;
	cmp_line_sum_bit_128_dummy = 0;
	cmp_line_sum_bit_real = cmp_line_sum_bit_pure_rgb + cmp_line_sum_bit_pure_a + cmp_line_sum_bit_32_dummy + cmp_line_sum_bit_128_dummy;
	cmp_line_sum_bit = ((cmp_line_sum_bit_real+128)>>8)*2;

	rtd_pr_vt_notice("%s,cmp_line_sum_bit=0x%x\n", __FUNCTION__, cmp_line_sum_bit);
	return cmp_line_sum_bit;
}

void dc2h2_pqc_cmp_line_setting(UINT8 enable_compression, unsigned int comp_wid, unsigned int comp_len, UINT8 comp_ratio)
{
	dc2h2_cap_dc2h2_pq_cmp_RBUS dc2h2_cap_dc2h2_pq_cmp_reg;
	dc2h2_cap_dc2h2_pq_cmp_pair_RBUS dc2h2_cap_dc2h2_pq_cmp_pair_reg;
	dc2h2_cap_dc2h2_pq_cmp_bit_RBUS dc2h2_cap_dc2h2_pq_cmp_bit_reg;
	dc2h2_cap_dc2h2_pq_cmp_bit_llb_RBUS dc2h2_cap_dc2h2_pq_cmp_bit_llb_reg;
	unsigned int cmp_line_sum_bit = 0;
	unsigned int cmp_width_div32 = 0, frame_limit_bit = 0;
	unsigned int llb_tolerance = 0, llb_x_blk_sta = 0, Llb_init = 0, Llb_step = 0;

	if(enable_compression == TRUE){
		dc2h2_cap_dc2h2_pq_cmp_reg.regValue = IoReg_Read32(DC2H2_CAP_DC2H2_PQ_CMP_reg);
		dc2h2_cap_dc2h2_pq_cmp_reg.cmp_en = 0;
		IoReg_Write32(DC2H2_CAP_DC2H2_PQ_CMP_reg, dc2h2_cap_dc2h2_pq_cmp_reg.regValue);

		if(comp_ratio < 8 || comp_ratio > 16){
			rtd_pr_vt_emerg("frame_limit_bit out range (8-16), will set default 10\n");
			comp_ratio = 10;
		}
		rtd_pr_vt_notice("%s,enable_compression=%d,comp_wid=%d,comp_len=%d,comp_ratio=%d\n", __FUNCTION__, enable_compression, comp_wid, comp_len, comp_ratio);

		if((comp_wid % 32) != 0){
			comp_wid = comp_wid + (32 - (comp_wid % 32));
		}
		cmp_width_div32 = comp_wid / 32;
		frame_limit_bit = comp_ratio << 2;

		dc2h2_cap_dc2h2_pq_cmp_reg.regValue = IoReg_Read32(DC2H2_CAP_DC2H2_PQ_CMP_reg);
		dc2h2_cap_dc2h2_pq_cmp_reg.cmp_width_div32 = cmp_width_div32;
		dc2h2_cap_dc2h2_pq_cmp_reg.cmp_height = comp_len;
		IoReg_Write32(DC2H2_CAP_DC2H2_PQ_CMP_reg, dc2h2_cap_dc2h2_pq_cmp_reg.regValue);

		dc2h2_cap_dc2h2_pq_cmp_bit_reg.regValue = IoReg_Read32(DC2H2_CAP_DC2H2_PQ_CMP_BIT_reg);
		dc2h2_cap_dc2h2_pq_cmp_bit_reg.frame_limit_bit = (comp_ratio << 2);
		dc2h2_cap_dc2h2_pq_cmp_bit_reg.block_limit_bit = 0x3F;
		dc2h2_cap_dc2h2_pq_cmp_bit_reg.first_line_more_bit = 0x20;
		IoReg_Write32(DC2H2_CAP_DC2H2_PQ_CMP_BIT_reg, dc2h2_cap_dc2h2_pq_cmp_bit_reg.regValue);

		cmp_line_sum_bit = calc_cmp_line_sum_bit(comp_wid, comp_ratio);

		dc2h2_cap_dc2h2_pq_cmp_pair_reg.regValue = IoReg_Read32(DC2H2_CAP_DC2H2_PQ_CMP_PAIR_reg);
		dc2h2_cap_dc2h2_pq_cmp_pair_reg.cmp_line_sum_bit = cmp_line_sum_bit;
		dc2h2_cap_dc2h2_pq_cmp_pair_reg.cmp_jump4_en = 1;
		dc2h2_cap_dc2h2_pq_cmp_pair_reg.cmp_jump6_en = 1;
		dc2h2_cap_dc2h2_pq_cmp_pair_reg.two_line_prediction_en = 1;
		dc2h2_cap_dc2h2_pq_cmp_pair_reg.cmp_line_mode = 1; //line mode
		dc2h2_cap_dc2h2_pq_cmp_pair_reg.cmp_data_color = 0; //RGB
		dc2h2_cap_dc2h2_pq_cmp_pair_reg.cmp_data_bit_width = (frame_limit_bit == 16) ? 1 : 0; //1=10bit,16bit for 3U10 pqc for video data qaulity
		dc2h2_cap_dc2h2_pq_cmp_pair_reg.cmp_data_format = 0; //444
		dc2h2_cap_dc2h2_pq_cmp_pair_reg.cmp_pair_para = 1;
		dc2h2_cap_dc2h2_pq_cmp_pair_reg.cmp_qp_mode = 0;
		IoReg_Write32(DC2H2_CAP_DC2H2_PQ_CMP_PAIR_reg, dc2h2_cap_dc2h2_pq_cmp_pair_reg.regValue);

		//compression llb setting
		llb_tolerance = 6;
		llb_x_blk_sta = 0;
		Llb_init = frame_limit_bit * 4 + llb_tolerance * 32;
		Llb_step = (Llb_init *16 - frame_limit_bit * 64) / cmp_width_div32;
		dc2h2_cap_dc2h2_pq_cmp_bit_llb_reg.regValue = IoReg_Read32(DC2H2_CAP_DC2H2_PQ_CMP_BIT_LLB_reg);
		dc2h2_cap_dc2h2_pq_cmp_bit_llb_reg.llb_x_blk_sta = llb_x_blk_sta;
		dc2h2_cap_dc2h2_pq_cmp_bit_llb_reg.llb_init = Llb_init;
		dc2h2_cap_dc2h2_pq_cmp_bit_llb_reg.llb_step = Llb_step;
		IoReg_Write32(DC2H2_CAP_DC2H2_PQ_CMP_BIT_LLB_reg, dc2h2_cap_dc2h2_pq_cmp_bit_llb_reg.regValue);

		dc2h2_cap_dc2h2_pq_cmp_reg.regValue = IoReg_Read32(DC2H2_CAP_DC2H2_PQ_CMP_reg);
		dc2h2_cap_dc2h2_pq_cmp_reg.cmp_en = 1;
		IoReg_Write32(DC2H2_CAP_DC2H2_PQ_CMP_reg, dc2h2_cap_dc2h2_pq_cmp_reg.regValue);
	}else{
		dc2h2_cap_dc2h2_pq_cmp_reg.regValue = IoReg_Read32(DC2H2_CAP_DC2H2_PQ_CMP_reg);
		dc2h2_cap_dc2h2_pq_cmp_reg.cmp_en = 0;
		IoReg_Write32(DC2H2_CAP_DC2H2_PQ_CMP_reg, dc2h2_cap_dc2h2_pq_cmp_reg.regValue);
	}
}

void set_dc2h2_cap0_blkx_sta_addr_and_boundary(void)
{
	dc2h2_cap_dc2h2_cap0_blk0_ctrl0_RBUS dc2h2_cap_dc2h2_cap0_blk0_ctrl0_reg;
	dc2h2_cap_dc2h2_cap0_blk0_ctrl1_RBUS dc2h2_cap_dc2h2_cap0_blk0_ctrl1_reg;
	dc2h2_cap_dc2h2_cap0_blk0_ctrl2_RBUS dc2h2_cap_dc2h2_cap0_blk0_ctrl2_reg;

	dc2h2_cap_dc2h2_cap0_blk1_ctrl0_RBUS dc2h2_cap_dc2h2_cap0_blk1_ctrl0_reg;
	dc2h2_cap_dc2h2_cap0_blk1_ctrl1_RBUS dc2h2_cap_dc2h2_cap0_blk1_ctrl1_reg;
	dc2h2_cap_dc2h2_cap0_blk1_ctrl2_RBUS dc2h2_cap_dc2h2_cap0_blk1_ctrl2_reg;

	dc2h2_cap_dc2h2_cap0_blk2_ctrl0_RBUS dc2h2_cap_dc2h2_cap0_blk2_ctrl0_reg;
	dc2h2_cap_dc2h2_cap0_blk2_ctrl1_RBUS dc2h2_cap_dc2h2_cap0_blk2_ctrl1_reg;
	dc2h2_cap_dc2h2_cap0_blk2_ctrl2_RBUS dc2h2_cap_dc2h2_cap0_blk2_ctrl2_reg;

	dc2h2_cap_dc2h2_cap0_blk3_ctrl0_RBUS dc2h2_cap_dc2h2_cap0_blk3_ctrl0_reg;
	dc2h2_cap_dc2h2_cap0_blk3_ctrl1_RBUS dc2h2_cap_dc2h2_cap0_blk3_ctrl1_reg;
	dc2h2_cap_dc2h2_cap0_blk3_ctrl2_RBUS dc2h2_cap_dc2h2_cap0_blk3_ctrl2_reg;

	dc2h2_cap_dc2h2_cap0_blk0_ctrl0_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[0].pa_y;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL0_reg, dc2h2_cap_dc2h2_cap0_blk0_ctrl0_reg.regValue);
	//upper boundary 
	dc2h2_cap_dc2h2_cap0_blk0_ctrl1_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[0].pa_y + CapBuffer_VT[DC2H2].vt_pa_buf_info[0].size_plane_y - DEFAULT_DC2H_CAP_BURST_LEN;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL1_reg, dc2h2_cap_dc2h2_cap0_blk0_ctrl1_reg.regValue);
	// low boundary
	dc2h2_cap_dc2h2_cap0_blk0_ctrl2_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[0].pa_y;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL2_reg, dc2h2_cap_dc2h2_cap0_blk0_ctrl2_reg.regValue);

	dc2h2_cap_dc2h2_cap0_blk1_ctrl0_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[1].pa_y;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL0_reg, dc2h2_cap_dc2h2_cap0_blk1_ctrl0_reg.regValue);
	//upper boundary 
	dc2h2_cap_dc2h2_cap0_blk1_ctrl1_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[1].pa_y + CapBuffer_VT[DC2H2].vt_pa_buf_info[1].size_plane_y - DEFAULT_DC2H_CAP_BURST_LEN;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL1_reg, dc2h2_cap_dc2h2_cap0_blk1_ctrl1_reg.regValue);
	// low boundary
	dc2h2_cap_dc2h2_cap0_blk1_ctrl2_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[1].pa_y;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL2_reg, dc2h2_cap_dc2h2_cap0_blk1_ctrl2_reg.regValue);

	dc2h2_cap_dc2h2_cap0_blk2_ctrl0_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[2].pa_y;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL0_reg, dc2h2_cap_dc2h2_cap0_blk2_ctrl0_reg.regValue);
	//upper boundary 
	dc2h2_cap_dc2h2_cap0_blk2_ctrl1_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[2].pa_y + CapBuffer_VT[DC2H2].vt_pa_buf_info[2].size_plane_y - DEFAULT_DC2H_CAP_BURST_LEN;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL1_reg, dc2h2_cap_dc2h2_cap0_blk2_ctrl1_reg.regValue);
	// low boundary
	dc2h2_cap_dc2h2_cap0_blk2_ctrl2_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[2].pa_y;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL2_reg, dc2h2_cap_dc2h2_cap0_blk2_ctrl2_reg.regValue);


	dc2h2_cap_dc2h2_cap0_blk3_ctrl0_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[3].pa_y;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL0_reg, dc2h2_cap_dc2h2_cap0_blk3_ctrl0_reg.regValue);
	//upper boundary 
	dc2h2_cap_dc2h2_cap0_blk3_ctrl1_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[3].pa_y + CapBuffer_VT[DC2H2].vt_pa_buf_info[3].size_plane_y - DEFAULT_DC2H_CAP_BURST_LEN;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL1_reg, dc2h2_cap_dc2h2_cap0_blk3_ctrl1_reg.regValue);
	// low boundary
	dc2h2_cap_dc2h2_cap0_blk3_ctrl2_reg.regValue = CapBuffer_VT[DC2H2].vt_pa_buf_info[3].pa_y;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL2_reg, dc2h2_cap_dc2h2_cap0_blk3_ctrl2_reg.regValue);
}

unsigned int get_osd5_dispaly_framerate(void)
{
	return 90; //fake: OSD5 fix framerate 90 hz
}

unsigned char get_dc2h2_cap_d2_to_osd5_frc_input_slow(void)
{
	//d2 osd123 blended output, osd4 blended output
	if(Get_DISPLAY_REFRESH_RATE() <= get_osd5_dispaly_framerate()){
		return TRUE;
	}else{
		return FALSE;
	}
}

void set_dc2h2_pqc_cap0_line_burst_num(int burst_num, unsigned int capLen, VT_CAP_FRAME_LINE_MODE cap_mode)
{
	dc2h2_cap_dc2h2_cap0_wr_dma_num_bl_wrap_line_step_RBUS dc2h2_cap0_wr_dma_num_bl_wrap_line_step_reg;
	dc2h2_cap_dc2h2_cap0_wr_dma_num_bl_wrap_ctl_RBUS dc2h2_cap0_wr_dma_num_bl_wrap_ctl_reg;
	dc2h2_cap_dc2h2_cap0_wr_dma_num_bl_wrap_word_RBUS dc2h2_cap0_wr_dma_num_bl_wrap_word_reg;

	dc2h2_cap0_wr_dma_num_bl_wrap_line_step_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_line_step_reg);
	dc2h2_cap0_wr_dma_num_bl_wrap_line_step_reg.dc2h2_cap0_line_step = burst_num;
	if(Get_PANEL_VFLIP_ENABLE()){
		dc2h2_cap0_wr_dma_num_bl_wrap_line_step_reg.dc2h2_cap0_line_step = -burst_num;
	}
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_line_step_reg, dc2h2_cap0_wr_dma_num_bl_wrap_line_step_reg.regValue);

	dc2h2_cap0_wr_dma_num_bl_wrap_ctl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_ctl_reg);
	if(cap_mode == DC2H_CAP_LINE_MODE){
		dc2h2_cap0_wr_dma_num_bl_wrap_ctl_reg.dc2h2_cap0_line_num = capLen;
	}else{
		dc2h2_cap0_wr_dma_num_bl_wrap_ctl_reg.dc2h2_cap0_line_num = 1; /* frame mode */
	}
	dc2h2_cap0_wr_dma_num_bl_wrap_ctl_reg.dc2h2_cap0_burst_len = DEFAULT_DC2H_CAP_BURST_LEN; /* dc2h2_cap0_burst_len*128bit */
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_ctl_reg, dc2h2_cap0_wr_dma_num_bl_wrap_ctl_reg.regValue);

	dc2h2_cap0_wr_dma_num_bl_wrap_word_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_word_reg);
	dc2h2_cap0_wr_dma_num_bl_wrap_word_reg.dc2h2_cap0_line_burst_num = burst_num;
	IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_word_reg, dc2h2_cap0_wr_dma_num_bl_wrap_word_reg.regValue);
}

void drvif_set_dc2h2_pqc_cap0_ctl(VT_CUR_CAPTURE_INFO *dc2h_ctrl_info)
{
	int cap0_total_bit = 0;
	int cap0_burst_num = 0;

	VT_CAP_FRAME_LINE_MODE dc2h_cap_mode = DC2H_CAP_LINE_MODE;
    dc2h2_cap_dc2h2_cap_size_RBUS dc2h2_cap_size_reg;//w,h
	dc2h2_cap_dc2h2_pq_cmp_pair_RBUS dc2h2_cap_dc2h2_pq_cmp_pair_reg;
	dc2h2_cap_dc2h2_cap0_cap_ctl0_RBUS dc2h2_cap_dc2h2_cap0_cap_ctl0_reg;
	dc2h2_cap_dc2h2_cap0_dma_wr_ctrl_RBUS dc2h2_cap0_dma_wr_ctrl_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_RBUS dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_RBUS dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg;

	if(dc2h_ctrl_info == NULL){
		rtd_pr_vt_emerg("%s=%d pdc2h_ctrl_info is null, return\n", __FUNCTION__, __LINE__);
		return;
	}

	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, 0x00000000); //clear format setting & line mode, capx disable
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg, 0x00000000); //clear byte swap

	dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.regValue = IoReg_Read32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_422to444_CTRL_reg);
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.regValue = IoReg_Read32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_DITHER_CTRL1_reg);
	//make data path 10bit
	dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.dither_uzd_swap_en = 0; // dither->uzd
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.dither_bit_sel = 1; // dither output 10bit
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.dither_en = 1;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.temporal_enable = 0;

	rtd_outl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_422to444_CTRL_reg, dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.regValue);
	rtd_outl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_DITHER_CTRL1_reg, dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.regValue);

	//dc2h2 capture size
	dc2h2_cap_size_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Size_reg);
	dc2h2_cap_size_reg.dc2h2_dma_hact = dc2h_ctrl_info->output_size.w;
    dc2h2_cap_size_reg.dc2h2_dma_vact = dc2h_ctrl_info->output_size.h;
    rtd_outl(DC2H2_CAP_DC2H2_CAP_Size_reg, dc2h2_cap_size_reg.regValue);

	dc2h2_pqc_cmp_line_setting(TRUE, dc2h_ctrl_info->output_size.w, dc2h_ctrl_info->output_size.h, dc2h2_pqc_cmp_ratio);

	dc2h2_cap_dc2h2_pq_cmp_pair_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_PQ_CMP_PAIR_reg);
	cap0_total_bit = dc2h2_cap_dc2h2_pq_cmp_pair_reg.cmp_line_sum_bit * 128;
	cap0_burst_num = ((cap0_total_bit + 127) / 128);

	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_Cap_CTL0_reg);
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_buf_index_mode = 1; //HW auto select memory block
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_buf_total_num = (dc2h_ctrl_info->bufnum - 1);
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_frc_style = 1;//get_dc2h2_cap_d2_to_osd5_frc_input_slow() ? 0 : 1;
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_buf_cnt_en = 1;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_Cap_CTL0_reg, dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.regValue);

	dc2h2_cap0_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg);
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_1byte_swap = 1;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_2byte_swap = 1;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_4byte_swap = 1;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg, dc2h2_cap0_dma_wr_ctrl_reg.regValue);

	set_dc2h2_cap_color_fmt_rgb(DC2H_CAP_LINE_MODE);
	set_dc2h2_cap0_blkx_sta_addr_and_boundary();
	set_dc2h2_pqc_cap0_line_burst_num(cap0_burst_num, dc2h_ctrl_info->output_size.h, dc2h_cap_mode);
	IoReg_SetBits(DC2H2_CAP_DC2H2_CAP_Color_format_reg, DC2H2_CAP_DC2H2_CAP_Color_format_dc2h2_cap0_en_mask);//cap0 enable
}

void set_dc2h2_cap0_hw_mode(VT_CUR_CAPTURE_INFO *dc2h_ctrl_info)
{
	int cap0_total_bit = 0;
	int cap0_burst_num = 0;
	int bitperpixel = 0;

	unsigned int dc2h_cap_mode = get_dc2h_dma_cap_mode(DC2H2);
	dc2h2_cap_dc2h2_cap0_dma_wr_ctrl_RBUS dc2h2_cap0_dma_wr_ctrl_reg;
    dc2h2_cap_dc2h2_cap_size_RBUS dc2h2_cap_size_reg;//w,h
	dc2h2_cap_dc2h2_cap_color_format_RBUS dc2h2_cap_color_format_reg;
	dc2h2_cap_dc2h2_pq_cmp_RBUS dc2h2_cap_dc2h2_pq_cmp_reg;
	dc2h2_cap_dc2h2_cap0_cap_ctl0_RBUS dc2h2_cap_dc2h2_cap0_cap_ctl0_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_RBUS dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_RBUS dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg;

	if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB) {
		bitperpixel = 24;
	}else{
		bitperpixel = 32;
	}

	dc2h_ctrl_info->output_size.w  = (dc2h_ctrl_info->output_size.w+1)/2*2;
	rtd_pr_vt_notice("cap width align to even %d\n", dc2h_ctrl_info->output_size.w);

	if(dc2h_cap_mode == DC2H_CAP_FRAME_MODE){
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w * dc2h_ctrl_info->output_size.h;
	}else{
		cap0_total_bit = bitperpixel * dc2h_ctrl_info->output_size.w;
	}

	cap0_burst_num = ((cap0_total_bit + 127) / 128);

	dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.regValue = IoReg_Read32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_422to444_CTRL_reg);
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.regValue = IoReg_Read32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_DITHER_CTRL1_reg);
	//make data path 10bit
	dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.dither_uzd_swap_en = 0; // dither->uzd
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.dither_bit_sel = 1; // dither output 10bit
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.dither_en = 1;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.temporal_enable = 0;
	rtd_outl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_422to444_CTRL_reg, dc2h2_r2y_dither_4xxto4xx_dc2h2_422to444_ctrl_reg.regValue);
	rtd_outl(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_DITHER_CTRL1_reg, dc2h2_r2y_dither_4xxto4xx_dc2h2_dither_ctrl1_reg.regValue);

	dc2h2_cap_dc2h2_pq_cmp_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_PQ_CMP_reg);
	dc2h2_cap_dc2h2_pq_cmp_reg.cmp_en = 0;
	rtd_outl(DC2H2_CAP_DC2H2_PQ_CMP_reg, dc2h2_cap_dc2h2_pq_cmp_reg.regValue);
	
	dc2h2_cap_size_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Size_reg);
	dc2h2_cap_size_reg.dc2h2_dma_hact = dc2h_ctrl_info->output_size.w;//should be even
    dc2h2_cap_size_reg.dc2h2_dma_vact = dc2h_ctrl_info->output_size.h;
    rtd_outl(DC2H2_CAP_DC2H2_CAP_Size_reg, dc2h2_cap_size_reg.regValue);

    dc2h2_cap0_dma_wr_ctrl_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg);
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_1byte_swap = 0;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_2byte_swap = 0;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_4byte_swap = 0;
	dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 0;

	if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB){
		set_dc2h2_cap_color_fmt_rgb(dc2h_cap_mode);
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_1byte_swap = 1;
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_2byte_swap = 1;
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_4byte_swap = 1;
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;
	}else if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ARGB){
		set_dc2h2_cap_color_fmt_argb(CAP_ARGB, dc2h_cap_mode);
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;
	}else if(dc2h_ctrl_info->fmt == V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGBA){
		set_dc2h2_cap_color_fmt_argb(CAP_RGBA, dc2h_cap_mode);
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;
	}else{
		rtd_pr_vt_notice("[VT]unspport capture RAGB,RGAB\n");
		set_dc2h2_cap_color_fmt_argb(CAP_ARGB, dc2h_cap_mode);
		dc2h2_cap0_dma_wr_ctrl_reg.dc2h2_cap0_dma_8byte_swap = 1;
	}
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_DMA_WR_Ctrl_reg, dc2h2_cap0_dma_wr_ctrl_reg.regValue);
	set_dc2h2_cap0_line_burst_num(cap0_burst_num, dc2h_ctrl_info->output_size.h, dc2h_cap_mode);

	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP0_Cap_CTL0_reg);
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_buf_index_mode = 1; //HW auto select memory block
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_buf_total_num = 2; //3 frame buffers,blk 0,1,2
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_frc_style = 0; //input slow
	dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_buf_cnt_en = 1;
	rtd_outl(DC2H2_CAP_DC2H2_CAP0_Cap_CTL0_reg, dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.regValue);

	set_dc2h2_cap0_blkx_sta_addr_and_boundary();

	dc2h2_cap_color_format_reg.regValue = rtd_inl(DC2H2_CAP_DC2H2_CAP_Color_format_reg);
	dc2h2_cap_color_format_reg.dc2h2_cap0_en = 1;
	dc2h2_cap_color_format_reg.dc2h2_cap1_en = 0;
	dc2h2_cap_color_format_reg.dc2h2_cap2_en = 0;
	rtd_outl(DC2H2_CAP_DC2H2_CAP_Color_format_reg, dc2h2_cap_color_format_reg.regValue);
}

void drvif_dc2h2_pqc_capture_config(VT_CUR_CAPTURE_INFO *pdc2h_ctrl_info)
{
	if(pdc2h_ctrl_info == NULL){
		rtd_pr_vt_emerg("%s=%d pdc2h_ctrl_info is null, return\n", __FUNCTION__, __LINE__);
		return;
	}
	if(pdc2h_ctrl_info->enable == TRUE){
		set_dc2h2_in_size_and_in_sel(pdc2h_ctrl_info->cap_location);
		/* crop region initial to input size */
		pdc2h_ctrl_info->crop_size = pdc2h_ctrl_info->input_size;
		rtd_pr_vt_emerg("dc2h crop size (%d,%d,%d,%d)\n", pdc2h_ctrl_info->crop_size.x, pdc2h_ctrl_info->crop_size.y, pdc2h_ctrl_info->crop_size.w, pdc2h_ctrl_info->crop_size.h);
		resize_dc2h2_capture_size(pdc2h_ctrl_info);
		set_dc2h2_cap_buf_property(pdc2h_ctrl_info);
		rtd_pr_vt_emerg("output wxh=%dx%d\n", pdc2h_ctrl_info->output_size.w, pdc2h_ctrl_info->output_size.h);

		drvif_set_dc2h2_crop_config(0, 0, pdc2h_ctrl_info->crop_size.w, pdc2h_ctrl_info->crop_size.h, FALSE);
		drvif_set_dc2h2_r2y_config(pdc2h_ctrl_info->fmt, pdc2h_ctrl_info->cap_location);
		drvif_set_dc2h2_scale_down_config(pdc2h_ctrl_info->crop_size, pdc2h_ctrl_info->output_size);
		drvif_set_dc2h2_pqc_cap0_ctl(pdc2h_ctrl_info);
	}else{
		/* stop dc2h hw capture */
		disable_dc2h2_capture();
	}
}

void set_dc2h2_cap0_seq_capture_hw_mode(VT_CUR_CAPTURE_INFO *pdc2h_ctrl_info)
{
	if(pdc2h_ctrl_info == NULL){
		rtd_pr_vt_emerg("%s=%d pdc2h_ctrl_info is null, return\n", __FUNCTION__, __LINE__);
		return;
	}
	if(pdc2h_ctrl_info->enable == TRUE){
		set_dc2h2_in_size_and_in_sel(pdc2h_ctrl_info->cap_location);
		/* crop region initial to input size */
		pdc2h_ctrl_info->crop_size = pdc2h_ctrl_info->input_size;
		rtd_pr_vt_emerg("dc2h crop size (%d,%d,%d,%d)\n", pdc2h_ctrl_info->crop_size.x, pdc2h_ctrl_info->crop_size.y, pdc2h_ctrl_info->crop_size.w, pdc2h_ctrl_info->crop_size.h);
		resize_dc2h2_capture_size(pdc2h_ctrl_info);
		set_dc2h2_cap_buf_property(pdc2h_ctrl_info);
		rtd_pr_vt_emerg("output wxh=%dx%d\n", pdc2h_ctrl_info->output_size.w, pdc2h_ctrl_info->output_size.h);

		drvif_set_dc2h2_crop_config(0, 0, pdc2h_ctrl_info->crop_size.w, pdc2h_ctrl_info->crop_size.h, FALSE);
		drvif_set_dc2h2_r2y_config(pdc2h_ctrl_info->fmt, pdc2h_ctrl_info->cap_location);
		drvif_set_dc2h2_scale_down_config(pdc2h_ctrl_info->crop_size, pdc2h_ctrl_info->output_size);
		set_dc2h2_cap0_hw_mode(pdc2h_ctrl_info);
	}else{
		/* stop dc2h hw capture */
		disable_dc2h2_capture();
	}
}

void set_vfod_freezed(unsigned int *pIndex)
{
	unsigned int bufnum = 0;

	if (CapBuffer_VT[DC2H1].vt_pa_buf_info[0].pa_y == 0) {
		rtd_pr_vt_emerg("[Vt warning]vt no init, so no memory, return freeze action;%s=%d \n", __FUNCTION__, __LINE__);
		return;
	}

	if((scaler_dtg_get_abnormal_dvs_long_flag(DTG_MASTER)==TRUE) || (scaler_dtg_get_abnormal_dvs_short_flag(DTG_MASTER)==TRUE))
	{
		if(wait_dvs_stable_cnt != 0) /* dvs stable once */
		{
			abnormal_dvs_cnt = 0;
			wait_dvs_stable_cnt = 0;
			rtd_pr_vt_emerg("reset abnormal_dvs_cnt,wait_dvs_stable_cnt,because not wait dvs stable\n");
		}
		abnormal_dvs_cnt++;

		if(ABNORMAL_DVS_FIRST_HAPPEN == abnormal_dvs_cnt) /*happen abnormal_dvs stop dc2h */
		{
			rtd_pr_vt_emerg("[Warning VBE]%d Vbe at abnormal dvs status,could not do vt capture!\n", __LINE__);
			if(TRUE == get_dc2h_dmaen_state(DC2H1)) /*dc2h already enable*/
			{
				vt_enable_dc2h(FALSE); /*stop dc2h,avoid memory trash,kernel panic KTASKWBS-13022*/
				rtd_pr_vt_emerg("stop dc2h done!\n");
			}
			return;
		}
		if(abnormal_dvs_cnt > MAX_CAP_BUF_NUM)
		{
			rtd_pr_vt_emerg("[Warning]dc2h has been disable because abnormal dvs\n");
			return;
		}
	}
	else
	{
		if(abnormal_dvs_cnt != 0) /* happen abnormal dvs before*/
		{
			wait_dvs_stable_cnt++;
			if(wait_dvs_stable_cnt == WAIT_DVS_STABLE_COUNT) /* dvs wait stable,this function called twice in every dqubf,consecutive 4 times stable dvs, enable VT capture again */
			{
				abnormal_dvs_cnt = 0;
				wait_dvs_stable_cnt = 0;
				if(FALSE == get_dc2h_dmaen_state(DC2H1))
				{
					vt_enable_dc2h(TRUE); /* KTASKWBS-13909 dvs stable again, enable dc2h */
					rtd_pr_vt_emerg("enable dc2h done!\n");
				}
			}
		}
	}

	bufnum = get_vt_VtBufferNum(DC2H1);
	if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX){
		rtd_pr_vt_notice("deq 90K %d\n", IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg));
	}

	if(bufnum == 1){
		if(VFODState.bFreezed == TRUE) {
			drvif_set_dc2h_capture_freeze(FALSE, DC2H1);
			VFODState.bFreezed = FALSE;
		}

		if(VFODState.bFreezed == FALSE){
			drvif_set_dc2h_capture_freeze(TRUE, DC2H1);
			VFODState.bFreezed = TRUE;
		}

		*pIndex = IndexOfFreezedVideoFrameBuffer[DC2H1];
	}else{
		static unsigned char vt_first_dequeue_flag = TRUE;
		static int last_dequeue_index = -1;
		set_vt_dequeue_overflow_flag(FALSE, DC2H1);
		if(vt_first_dequeue_flag == TRUE)
		{
			vt_first_dequeue_flag = FALSE;
			IndexOfFreezedVideoFrameBuffer[DC2H1] = (htonl(pDc2h_BufStatus->write[DC2H1]) + bufnum - 1)%bufnum;
		}else{
			if(((htonl(pDc2h_BufStatus->read[DC2H1]) + 1)%bufnum) == htonl(pDc2h_BufStatus->write[DC2H1])){
				//*pIndex = VT_NOT_ENOUGH_RESOURCE;
				IndexOfFreezedVideoFrameBuffer[DC2H1] = htonl(pDc2h_BufStatus->read[DC2H1]);
                set_vt_buf_dequeue(DC2H1, IndexOfFreezedVideoFrameBuffer[DC2H1]);
				*pIndex = IndexOfFreezedVideoFrameBuffer[DC2H1];
				rtd_pr_vt_notice("dequeue repeat last index %d\n", IndexOfFreezedVideoFrameBuffer[DC2H1]);
				//up(&VT_Semaphore);
				return;
			}else{
				IndexOfFreezedVideoFrameBuffer[DC2H1] = (htonl(pDc2h_BufStatus->read[DC2H1]) + 1)%bufnum;
			}
		}

		*pIndex = IndexOfFreezedVideoFrameBuffer[DC2H1];
		set_vt_buf_dequeue(DC2H1, IndexOfFreezedVideoFrameBuffer[DC2H1]);
		if(last_dequeue_index != -1){
			set_vt_buf_enqueue(DC2H1, last_dequeue_index);
		}
		last_dequeue_index = IndexOfFreezedVideoFrameBuffer[DC2H1];
		pDc2h_BufStatus->read[DC2H1] = Scaler_ChangeUINT32Endian(IndexOfFreezedVideoFrameBuffer[DC2H1]);

		if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX){
			rtd_pr_vt_notice("sta %d,%d,%d,%d,%d,dq_idx=%d\n", \
				htonl(pDc2h_BufStatus->bufStatus[DC2H1].bufSta[0]),\
				htonl(pDc2h_BufStatus->bufStatus[DC2H1].bufSta[1]),\
				htonl(pDc2h_BufStatus->bufStatus[DC2H1].bufSta[2]),\
				htonl(pDc2h_BufStatus->bufStatus[DC2H1].bufSta[3]),\
				htonl(pDc2h_BufStatus->bufStatus[DC2H1].bufSta[4]),\
				IndexOfFreezedVideoFrameBuffer[DC2H1]);
		}
	}
}
// ------------------------------end-------------------------------------
#endif // #ifndef UT_flag

void set_cap_buffer_size_by_AP(unsigned int usr_width, unsigned int usr_height)
{
	unsigned int width_16bytes_align = 0;
	width_16bytes_align = drvif_memory_get_data_align(usr_width,16);
	vt_cap_frame_max_width = width_16bytes_align;
	vt_cap_frame_max_height = usr_height;
	rtd_pr_vt_notice("cap buf size:(%d,%d)\n",vt_cap_frame_max_width, vt_cap_frame_max_height);
}

unsigned long vt_bufAddr_align(unsigned int val)
{
    unsigned long ret = 0;

#ifdef CONFIG_BW_96B_ALIGNED
    ret = drvif_memory_get_data_align(val, __12KPAGE); // LCM(96, 64, 4K),96b align for 3ddr
#else
    ret = drvif_memory_get_data_align(val, __4KPAGE); //64b align for gpu,4k align for mmap
    //ret = PAGE_ALIGN(ret); // 4k align for mmap
#endif

    return ret;
}

unsigned int get_dc2h_capbuf_size(unsigned int dc2h_num)
{
	unsigned int size = 0;
	unsigned int fmt = curCapInfo[dc2h_num].fmt;
	unsigned int bufWid = drvif_memory_get_data_align(curCapInfo[dc2h_num].output_size.w, 16);
	unsigned int bufHei = curCapInfo[dc2h_num].output_size.h;
	if(dc2h2_pqc_path_enable == TRUE){
		unsigned int cmp_line_sum_bit = 0;
		cmp_line_sum_bit = calc_cmp_line_sum_bit(curCapInfo[dc2h_num].output_size.w, dc2h2_pqc_cmp_ratio);
		size = (bufHei * cmp_line_sum_bit * 16);//128bit unit
		CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y = size;
		CapBuffer_VT[dc2h_num].image_size = size;
		rtd_pr_vt_notice("[VT]buf dc2h2 pqc case\n");
	}else{
		if(fmt == VT_CAP_ABGR8888 || fmt == VT_CAP_ARGB8888 || fmt == VT_CAP_RGBA8888 || fmt == VT_CAP_RGB888){
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y = (fmt == VT_CAP_RGB888) ? (bufWid*bufHei*3):(bufWid*bufHei*4);
			CapBuffer_VT[dc2h_num].image_size = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y;
		}else if(fmt == VT_CAP_NV12 || fmt == VT_CAP_NV21){
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y = bufWid*bufHei;
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_u = bufWid*bufHei/2;
			CapBuffer_VT[dc2h_num].image_size = (CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y + CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_u);
		}else if(fmt == VT_CAP_NV12_10BIT){
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y = bufWid*10*bufHei/8;
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_u = bufWid*10*bufHei/8/2;
			CapBuffer_VT[dc2h_num].image_size = (CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y + CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_u);
		}else if(fmt == VT_CAP_P010_10BIT){
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y = bufWid*2*bufHei; //(bufWid*16*bufHei/8 + bufWid*16*bufHei/8/2)
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_u = bufWid*bufHei;
			CapBuffer_VT[dc2h_num].image_size = (CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y + CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_u);
		}else if(fmt == VT_CAP_YV12 || fmt == VT_CAP_I420){
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y = bufWid*bufHei;
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_u = bufWid*bufHei/4;
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_v = bufWid*bufHei/4;
			CapBuffer_VT[dc2h_num].image_size = (CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y+CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_u+CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_v);
		}
	}

	size = vt_bufAddr_align(CapBuffer_VT[dc2h_num].image_size);
	CapBuffer_VT[dc2h_num].buf_size = size;
	rtd_pr_vt_notice("[VT]buf size=0x%x\n",size);
	return size;
}

unsigned char Capture_BufferMemInit_VT(unsigned int buffernum, unsigned int dc2h_num)
{
    unsigned int size = 0;
    int i;
    unsigned long allocatetotalsize = 0, carvedout_size = 0;
    unsigned long physicalstartaddree = 0;
    unsigned int vt_real_buf_nums = 0;
    unsigned long DMA_release_start = 0;
    unsigned long DMA_release_size = 0;
	unsigned int plane_num = get_dc2h_cap_fmt_plane_number(curCapInfo[dc2h_num].fmt);

    size = get_dc2h_capbuf_size(dc2h_num);
	if(size == 0 || get_vt_dc2h_cap_mem_type(dc2h_num) != DC2H_MEMORY_MMAP)
	{
		rtd_pr_vt_emerg("func %s,[error] size 0x%x, mem_type %d\n", __FUNCTION__, size, get_vt_dc2h_cap_mem_type(dc2h_num));
		return FALSE;
	}

    if(get_vt_EnableFRCMode() == TRUE)
    {
        DMA_release_start = 0;
    }
    else
    {// co-buffer with memc

        allocatetotalsize = size * buffernum;
        if(get_MEMC_bypass_status_refer_platform_model() == FALSE)
        {//non-bypass memc
#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
            DMA_release_size = (unsigned int)carvedout_buf_query_secure(CARVEDOUT_SCALER_MEMC, (void *)&DMA_release_start);
#else
            DMA_release_size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_MEMC, (void *)&DMA_release_start);
#endif
            if((DMA_release_start == 0) || (allocatetotalsize > DMA_release_size))
            {
                rtd_pr_vt_emerg("[error]:query carvedout memc memory failed (%lx/%lx)\n", DMA_release_start, DMA_release_size);
                return FALSE;
            }
        }
        else
        {//if memc bypass
#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
            carvedout_size = (unsigned int)carvedout_buf_query_secure(CARVEDOUT_SCALER_MEMC, (void *)&DMA_release_start);
#else
            carvedout_size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_MEMC, (void *)&DMA_release_start);
#endif
            if((carvedout_size == 0) || (allocatetotalsize > carvedout_size))
            {
                rtd_pr_vt_emerg("[warn]query carvedout memc memory failed (%lx/%lx)\n", DMA_release_start, carvedout_size);
                DMA_release_start = 0; //allocate memory from cma
            }
        }
    }

    if(DMA_release_start != 0)
    { /* all capture buffer form memc */
        physicalstartaddree = DMA_release_start;

        for (i = 0; i < buffernum; i++)
        {
            if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y == 0)
            {
				vt_real_buf_nums++;
				CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y = vt_bufAddr_align((i == 0) ? (physicalstartaddree) : (CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y + size));
				CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_y = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y;
				rtd_pr_vt_notice("[memory allocate1] y plane buf[%d]=(%d KB), phy(%x) \n", i, CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_y >> 10, CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y);
				
				if(plane_num > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){
					CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_u = CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y + CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_y; 
					CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_u = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_u;
					rtd_pr_vt_notice("[memory allocate1] u buf[%d]=(%d KB), phy(%x) \n", i, CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_u >> 10, CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_u);
				}
				if(plane_num > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){
					CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_v = CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_u + CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_u; 
					CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_v = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_v;
					rtd_pr_vt_notice("[memory allocate1] v buf[%d]=(%d KB), phy(%x) \n", i, CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_v >> 10, CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_v);
				}
            }
        }
    }
    else
    {
		for(i = 0; i < buffernum; i++)
		{
			physicalstartaddree = pli_malloc(size, GFP_DCU2_FIRST);
			if(physicalstartaddree == INVALID_VAL)
			{
				rtd_pr_vt_emerg("[error]retry pli malloc fail(%lx)\n", physicalstartaddree);
#ifdef UT_flag
				return FALSE;
#else
				break;
#endif
			}
			else
			{
				VtAllocatedBufferStartAdress[dc2h_num][i] = physicalstartaddree;
				if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y == 0)
				{
					vt_real_buf_nums++;
					CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y = vt_bufAddr_align(physicalstartaddree);
					CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_y = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_y;
					
					rtd_pr_vt_notice("[memory allocate2] y plane buf[%d]=(%d KB), phy(%x) \n", i, CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_y >> 10, CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y);

					if(plane_num > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){
						CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_u = CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y + CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_y; 
						CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_u = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_u;
						rtd_pr_vt_notice("[memory allocate2] u buf[%d]=(%d KB), phy(%x) \n", i, CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_u >> 10, CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_u);
					}
					if(plane_num > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){							
						CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_v = CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_u + CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_u; 
						CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_v = CapBuffer_VT[dc2h_num].vt_pa_buf_info[0].size_plane_v;
						rtd_pr_vt_notice("[memory allocate2] v buf[%d]=(%d KB), phy(%x) \n", i, CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_v >> 10, CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_v);
					}
				}
			}
		}
    }

#ifndef UT_flag
    get_shm_vt_buf_status();
	init_vt_write_read_index();
#endif

	set_vt_VtBufferNum(vt_real_buf_nums, dc2h_num);
	curCapInfo[dc2h_num].bufnum = vt_real_buf_nums;
    if(vt_real_buf_nums == 1){
        set_vt_VtSwBufferMode(FALSE, dc2h_num);
	}else{
        set_vt_VtSwBufferMode(TRUE, dc2h_num);
	}

    return TRUE;
}

void Capture_BufferMemDeInit_VT(unsigned int dc2h_num)
{
	int i;
	unsigned int buffernum = get_vt_VtBufferNum(dc2h_num);
	for (i = 0; i < buffernum; i++)
	{
		if(VtAllocatedBufferStartAdress[dc2h_num][i])
		{
			pli_free(VtAllocatedBufferStartAdress[dc2h_num][i]);
			VtAllocatedBufferStartAdress[dc2h_num][i] = 0;
		}
	}

	for (i = 0; i < buffernum; i++)
	{
		if (CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y)
		{
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y = 0;
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_u = 0;
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_v = 0;
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_y = 0;
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_u = 0;
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].size_plane_v = 0;
		}
	}
	CapBuffer_VT[dc2h_num].image_size = 0;
	CapBuffer_VT[dc2h_num].buf_size = 0;
	CapBuffer_VT[dc2h_num].mem_type = DC2H_MEMORY_MMAP;
	rtd_pr_vt_notice("[VT memory free] Deinit done!\n");
}

#ifndef UT_flag

unsigned char do_vt_dc2h_qbuf(unsigned int index, unsigned int dc2h_num, int fd, unsigned int size)
{
	DC2H_MEMORY_TYPE mem_type = get_vt_dc2h_cap_mem_type(dc2h_num);
	rtd_pr_vt_notice("dc2h%d, qbuf %d\n", dc2h_num+1, index);
	if(index > get_vt_VtBufferNum(dc2h_num)){
		rtd_pr_vt_err("index %d bigger than buffer number\n", index);
		return FALSE;
	}

	if(mem_type == DC2H_MEMORY_MMAP){
		drvif_vt_set_enqueue_index(index, dc2h_num);
	}else if(mem_type == DC2H_MEMORY_DMA){
		if(CapBuffer_VT[dc2h_num].image_size == 0 || CapBuffer_VT[dc2h_num].buf_size == 0){
			get_dc2h_capbuf_size(dc2h_num); //update image size
		}

		if(CapBuffer_VT[dc2h_num].image_size > size){
			rtd_pr_vt_err("error, qbuf dma buf size too small,0x%x,0x%x\n", CapBuffer_VT[dc2h_num].image_size, size);
			return FALSE;
		}

		CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].pa_y = dmaheap_to_phys(fd);
		if(CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].pa_y == INVALID_VAL){
			rtd_pr_vt_err("get dma pa address failed\n");
			return FALSE;
		}
		CapBuffer_VT[dc2h_num].buf_size = size;
		CapBuffer_VT[dc2h_num].image_size = size;
		rtd_pr_vt_notice("[dma buffer] y buf[%d]=(%d KB), phy(%x) \n", index, CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].size_plane_y >> 10, CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].pa_y);

		if(CapBuffer_VT[dc2h_num].plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].pa_u = CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].pa_y + CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].size_plane_y;
			rtd_pr_vt_notice("[dma buffer] u buf[%d]=(%d KB), phy(%x) \n", index, CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].size_plane_u >> 10, CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].pa_u);
		}

		if(CapBuffer_VT[dc2h_num].plane_number > V4L2_EXT_CAPTURE_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].pa_v = CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].pa_u + CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].size_plane_u;
			rtd_pr_vt_notice("[dma buffer] v buf[%d]=(%d KB), phy(%x) \n", index, CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].size_plane_v >> 10, CapBuffer_VT[dc2h_num].vt_pa_buf_info[index].pa_v);
		}

		drvif_vt_set_enqueue_index(index, dc2h_num);

	}else{
		return FALSE;
	}
	return TRUE;
}

unsigned char do_vt_dc2h_dqbuf(unsigned int *getidx, unsigned int dc2h_num)
{
	if(drvif_vt_get_dequeue_index(getidx, dc2h_num) == FALSE){
		return FALSE;
	}
	return TRUE;
}

unsigned char do_vt_reqbufs(unsigned int buf_cnt, unsigned int dc2h_num)
{
	if(0 == buf_cnt) /* Release buffers by setting buf_cnt = 0*/
	{
		Capture_BufferMemDeInit_VT(dc2h_num);
		return TRUE;
	}
	else
	{
		if(Capture_BufferMemInit_VT(buf_cnt, dc2h_num) == FALSE){
			rtd_pr_vt_emerg("func:%s, failed!!!\n", __FUNCTION__);
			return FALSE;
		}
		return TRUE;
	}
}

void do_vt_dc2h_streamoff(unsigned int dc2h_num)
{
	rtd_pr_vt_emerg("func:%s,dc2h%d\n", __FUNCTION__, dc2h_num+1);
	curCapInfo[dc2h_num].enable = 0;
	if(dc2h_num == DC2H1){
		drvif_dc2h_capture_config(&curCapInfo[DC2H1]);
	}else{
		drvif_dc2h2_capture_config(&curCapInfo[DC2H2]);
	}
	drvif_vt_finalize(dc2h_num);
}

unsigned char do_vt_dc2h_streamon(unsigned int dc2h_num)
{
	ppoverlay_display_timing_ctrl1_RBUS display_timing_ctrl1_reg;
	down(&VT_Semaphore);

	display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);

	if (display_timing_ctrl1_reg.disp_en == 0)/*means d domain clock off, stop capture*/
	{
		rtd_pr_vt_emerg("[Warning Vt]%s=%d D domain clock is off status,could not do vt capture!\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return FALSE;
	}

    if(get_vt_VtBufferNum(dc2h_num) == 1){
        set_vt_VtSwBufferMode(FALSE, dc2h_num);
	}else{
        set_vt_VtSwBufferMode(TRUE, dc2h_num);
	}
	set_vt_dequeue_enqueue_count(0, dc2h_num);

	curCapInfo[dc2h_num].enable = 1;
	if(dc2h_num == DC2H1){
		drvif_dc2h_capture_config(&curCapInfo[DC2H1]);
	}else{
		drvif_dc2h2_capture_config(&curCapInfo[DC2H2]);
	}

	return TRUE;
}

unsigned char HAL_VT_InitEx(unsigned int buffernum)
{
	ppoverlay_display_timing_ctrl1_RBUS display_timing_ctrl1_reg;
	down(&VT_Semaphore);

    drvif_vt_block_vr_recording(get_vr_function());

	if((buffernum == 0) || (buffernum > VT_MAX_SUPPORT_BUF_NUMS)) { /*max support buffer number */
            buffernum = VT_MAX_SUPPORT_BUF_NUMS;
    }

	rtd_pr_vt_emerg("%s=%d buffernum=%d\n", __FUNCTION__, __LINE__,buffernum);

    if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) ==  _MODE_STATE_ACTIVE){
        if(get_drm_case_securestatus()==TRUE){
            rtd_pr_vt_emerg("[Warning vdecsvp]%s=%d DRM CASE source security is TRUE,could not do vt capture!\n", __FUNCTION__, __LINE__);
            up(&VT_Semaphore);
            return FALSE;
        }
    }

	display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
	if (display_timing_ctrl1_reg.disp_en == 0)/*means d domain clock off, stop capture*/
	{
		rtd_pr_vt_emerg("[Warning VT]D domain clock is off status,could not do vt capture!\n");
		up(&VT_Semaphore);
		return FALSE;
	}

	if((scaler_dtg_get_abnormal_dvs_long_flag(DTG_MASTER)==TRUE) || (scaler_dtg_get_abnormal_dvs_short_flag(DTG_MASTER)==TRUE)){ 
		rtd_pr_vt_emerg("[Warning VBE][VT] Vbe at abnormal dvs status,could not do vt capture!\n");
		up(&VT_Semaphore);
		return FALSE;
	}

	if(get_vt_function() == FALSE) {
		curCapInfo[DC2H1].enable = 0;
		curCapInfo[DC2H1].output_size.w = vfod_capture_out_W;
		curCapInfo[DC2H1].output_size.h = vfod_capture_out_H;
		set_vt_dc2h_cap_mem_type(DC2H1, DC2H_MEMORY_MMAP);
		VFODState.bEnabled = TRUE;
		VFODState.bAppliedPQ = TRUE;
		VFODState.bFreezed = FALSE;
		VFODState.framerateDivide = get_framerate_divide(DC2H1);
		set_vt_VtBufferNum(buffernum, DC2H1);
		set_vt_function(TRUE);
		rtd_pr_vt_notice("VT init Success;%s=%d \n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return TRUE;
	} else {
		rtd_pr_vt_notice("VT have inited,do nothing;%s=%d \n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return TRUE;
	}
}

unsigned char HAL_VT_Init(void)
{
	rtd_pr_vt_emerg("%s=%d \n", __FUNCTION__, __LINE__);

	if(HAL_VT_InitEx(5)==FALSE)
		return FALSE;
	else
		return TRUE;
}

unsigned char HAL_VT_Finalize(void)
{
	down(&VT_Semaphore);
	if(get_vt_function()==TRUE) {

		rtd_pr_vt_emerg("%s start\n", __FUNCTION__);
		if(get_dc2h_dmaen_state(DC2H1) == TRUE || get_vt_VtSwBufferMode(DC2H1) == TRUE)
		{
			curCapInfo[DC2H1].enable = 0; //iMode;
			drvif_dc2h_capture_config(&curCapInfo[DC2H1]);
		}

		Capture_BufferMemDeInit_VT(DC2H1);
		drvif_vt_finalize(DC2H1);
		VFODState.bEnabled = FALSE;
		VFODState.bAppliedPQ = TRUE;
		VFODState.bFreezed = FALSE;
		VFODState.framerateDivide = 1;
		VTDumpLocation = KADP_VT_MAX_LOCATION;
		set_vt_function(FALSE);
		rtd_pr_vt_emerg("%s done\n", __FUNCTION__);

        drvif_vt_reconfig_vr_recording();
		up(&VT_Semaphore);
		return TRUE;
	} else {
		rtd_pr_vt_emerg("VT have finalized,do nothing;%s=%d \n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return TRUE;
	}

}
#endif // #ifndef UT_flag

unsigned char HAL_VT_GetDeviceCapability(KADP_VT_DEVICE_CAPABILITY_INFO_T *pDeviceCapabilityInfo)
{
	down(&VT_Semaphore);
	pDeviceCapabilityInfo->numOfVideoWindow = 1;
	up(&VT_Semaphore);
	return TRUE;
}


unsigned char HAL_VT_GetVideoFrameBufferCapability(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, KADP_VT_VIDEO_FRAME_BUFFER_CAPABILITY_INFO_T *pVideoFrameBufferCapabilityInfo)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	pVideoFrameBufferCapabilityInfo ->numOfVideoFrameBuffer = get_vt_VtBufferNum(DC2H1);
	pVideoFrameBufferCapabilityInfo ->numOfPlane = get_dc2h_cap_fmt_plane_number(get_VT_Pixel_Format(DC2H1));
	up(&VT_Semaphore);
	return TRUE;
}

unsigned char HAL_VT_GetVideoFrameOutputDeviceCapability(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_CAPABILITY_INFO_T *pVideoFrameOutputDeviceCapabilityInfo)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	pVideoFrameOutputDeviceCapabilityInfo->bSupportedControlEnabled = TRUE;
	pVideoFrameOutputDeviceCapabilityInfo->bSupportedControlFreezed = TRUE;
	pVideoFrameOutputDeviceCapabilityInfo->bSupportedControlFramerateDivide = FALSE;
	pVideoFrameOutputDeviceCapabilityInfo->bSupportedControlPQ = FALSE;
	up(&VT_Semaphore);
	return TRUE;
}

unsigned char HAL_VT_GetVideoFrameOutputDeviceLimitation(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_LIMITATION_INFO_T *pVideoFrameOutputDeviceLimitationInfo)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	pVideoFrameOutputDeviceLimitationInfo->maxResolution = (KADP_VT_RECT_T){0,0,_DISP_WID,_DISP_LEN};
	pVideoFrameOutputDeviceLimitationInfo->bLeftTopAlign = FALSE;
	pVideoFrameOutputDeviceLimitationInfo->bSupportInputVideoDeInterlacing = FALSE;
	pVideoFrameOutputDeviceLimitationInfo->bSupportDisplayVideoDeInterlacing = FALSE;
	pVideoFrameOutputDeviceLimitationInfo->bSupportScaleUp = FALSE;
	pVideoFrameOutputDeviceLimitationInfo->scaleUpLimitWidth = 0;
	pVideoFrameOutputDeviceLimitationInfo->scaleUpLimitHeight = 0;
	pVideoFrameOutputDeviceLimitationInfo->bSupportScaleDown = TRUE;
	pVideoFrameOutputDeviceLimitationInfo->scaleDownLimitWidth = VT_CAP_SCALE_DOWN_LIMIT_WIDTH;
	pVideoFrameOutputDeviceLimitationInfo->scaleDownLimitHeight = VT_CAP_SCALE_DOWN_LIMIT_HEIGHT;
	up(&VT_Semaphore);
	return TRUE;
}

unsigned char HAL_VT_GetAllVideoFrameBufferProperty(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, KADP_VT_VIDEO_FRAME_BUFFER_PROPERTY_INFO_T *pVideoFrameBufferProperty)
{
    int i = 0;
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	if(get_vt_function() == TRUE) {
		if((get_VT_Pixel_Format(DC2H1) == VT_CAP_NV12) || (get_VT_Pixel_Format(DC2H1) == VT_CAP_NV21) || get_VT_Pixel_Format(DC2H1) == VT_CAP_NV12_10BIT || get_VT_Pixel_Format(DC2H1) == VT_CAP_P010_10BIT){
			pVideoFrameBufferProperty->pixelFormat = KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV420_SEMI_PLANAR;
			pVideoFrameBufferProperty->stride = vt_cap_frame_max_width;
			pVideoFrameBufferProperty->width = curCapInfo[DC2H1].output_size.w;
		}else if((get_VT_Pixel_Format(DC2H1) == VT_CAP_ABGR8888)||(get_VT_Pixel_Format(DC2H1) == VT_CAP_ARGB8888)||(get_VT_Pixel_Format(DC2H1) == VT_CAP_RGBA8888)) {
			pVideoFrameBufferProperty->pixelFormat = KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ARGB;
			pVideoFrameBufferProperty->stride = (vt_cap_frame_max_width * 4);
			pVideoFrameBufferProperty->width = curCapInfo[DC2H1].output_size.w * 4;
		}else if(get_VT_Pixel_Format(DC2H1) == VT_CAP_RGB888){
			pVideoFrameBufferProperty->pixelFormat = KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB;
			pVideoFrameBufferProperty->stride = (vt_cap_frame_max_width * 3);
			pVideoFrameBufferProperty->width = curCapInfo[DC2H1].output_size.w * 3;
		}else if(get_VT_Pixel_Format(DC2H1) == VT_CAP_YV12 || get_VT_Pixel_Format(DC2H1) == VT_CAP_I420){
			pVideoFrameBufferProperty->pixelFormat = KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV420_PLANAR;
			pVideoFrameBufferProperty->stride = vt_cap_frame_max_width;
			pVideoFrameBufferProperty->width = curCapInfo[DC2H1].output_size.w;
		}else{
			rtd_pr_vt_notice("func %s, unsupport format %d\n", __FUNCTION__, get_VT_Pixel_Format(DC2H1));
			up(&VT_Semaphore);
			return FALSE;
		}

		pVideoFrameBufferProperty->height = curCapInfo[DC2H1].output_size.h;
        for(i = 0; i < get_vt_VtBufferNum(DC2H1); i++)
        {
            pVideoFrameBufferProperty->ppPhysicalAddress[i] = CapBuffer_VT[DC2H1].vt_pa_buf_info[i].pa_y;
        }

		pVideoFrameBufferProperty->vfbbuffernumber = get_vt_VtBufferNum(DC2H1);
        pVideoFrameBufferProperty->vtYbufSize = CapBuffer_VT[DC2H1].vt_pa_buf_info[0].size_plane_y;
		up(&VT_Semaphore);
		return TRUE;
	} else {
		rtd_pr_vt_notice("VT is not Inited So return False;%s=%d\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return FALSE;
	}
}

#ifndef UT_flag
unsigned char HAL_VT_GetVideoFrameBufferIndex(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, unsigned int *pIndexOfCurrentVideoFrameBuffer)
{
	static unsigned char ucOnce_Drm_case_flag = FALSE;
	dc2h_cap_dc2h_interrupt_enable_RBUS dc2h_interrupt_enable_reg;
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
    if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
        rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    down(&VT_Semaphore);

    if(get_vt_function() == TRUE)
    {
        if(get_drm_case_securestatus() == TRUE){
            disable_dc2h_inten_and_dmaen();
            if(vtPrintMask & VT_PRINT_MASK_DRM_LOG)
                rtd_pr_vt_notice("[VT]get_drm_case_securestatus is true;%s=%d\n", __FUNCTION__, __LINE__);
            *pIndexOfCurrentVideoFrameBuffer = IndexOfFreezedVideoFrameBuffer[DC2H1];
            ucOnce_Drm_case_flag = TRUE;
            up(&VT_Semaphore);
            return TRUE;
        }
        else{
            if((get_vt_VtSwBufferMode(DC2H1)) && (DC2H_CAP_DC2H_Interrupt_Enable_get_dc2h_vs_rising_int_en(IoReg_Read32(DC2H_CAP_DC2H_Interrupt_Enable_reg) == 0)))
            {
               dc2h_interrupt_enable_reg.regValue = IoReg_Read32(DC2H_CAP_DC2H_Interrupt_Enable_reg);
               dc2h_interrupt_enable_reg.dc2h_vs_rising_int_en = 1;
			   dc2h_interrupt_enable_reg.dc2h_cap0_done_int_en = 1;
               IoReg_Write32(DC2H_CAP_DC2H_Interrupt_Enable_reg, dc2h_interrupt_enable_reg.regValue);
               msleep(20);
            }

            if(ucOnce_Drm_case_flag == TRUE){
                ucOnce_Drm_case_flag = FALSE;
				dc2h_cap_color_format_reg.regValue = IoReg_Read32(DC2H_CAP_DC2H_CAP_Color_format_reg);
				dc2h_cap_color_format_reg.dc2h_cap0_en = 1;
				if(CapBuffer_VT[DC2H1].plane_number > KADP_VT_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){
					dc2h_cap_color_format_reg.dc2h_cap1_en = 1;
				}
				if(CapBuffer_VT[DC2H1].plane_number > KADP_VT_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){
					dc2h_cap_color_format_reg.dc2h_cap2_en = 1;
				}
				IoReg_Write32(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);
            }
        }

		set_vfod_freezed(pIndexOfCurrentVideoFrameBuffer);

		up(&VT_Semaphore);
		return TRUE;
	}else{
		up(&VT_Semaphore);
		rtd_pr_vt_notice("VT is not Inited So return False;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
}
#endif // #ifndef UT_flag

unsigned char HAL_VT_GetVideoFrameOutputDeviceState(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_INFO_T *pVideoFrameOutputDeviceState)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	*pVideoFrameOutputDeviceState = VFODState;
	up(&VT_Semaphore);
	if(get_vt_function() == TRUE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

#ifndef UT_flag
unsigned char HAL_VT_SetVideoFrameOutputDeviceState(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FLAGS_T videoFrameOutputDeviceStateFlags, KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_INFO_T *pVideoFrameOutputDeviceState)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	if(videoFrameOutputDeviceStateFlags == KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_NOFX) {
		rtd_pr_vt_debug("KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_NOFX is do nothing\n");
		return TRUE;
	}

	down(&VT_Semaphore);
	if(get_vt_function() == TRUE) {

		if (videoFrameOutputDeviceStateFlags & KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_ENABLED) {
			if (VFODState.bEnabled != pVideoFrameOutputDeviceState->bEnabled) {
				curCapInfo[DC2H1].enable = pVideoFrameOutputDeviceState->bEnabled;
				VFODState.bEnabled = pVideoFrameOutputDeviceState->bEnabled;
				drvif_dc2h_capture_config(&curCapInfo[DC2H1]);
			}
		}

		if(videoFrameOutputDeviceStateFlags & KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FRAMERATE_DIVIDE) {
			rtd_pr_vt_debug("KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FRAMERATE_DIVIDE is %d\n",pVideoFrameOutputDeviceState->framerateDivide);
			VFODState.framerateDivide = pVideoFrameOutputDeviceState->framerateDivide;
		}

		/*freeze do last*/
		if(videoFrameOutputDeviceStateFlags & KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FREEZED) {
			if(get_vt_function() == TRUE) {
				if(pVideoFrameOutputDeviceState->bFreezed == VFODState.bFreezed) {
					rtd_pr_vt_debug("KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FREEZED is the same as you setting\n");
				} else {
					VFODState.bFreezed = pVideoFrameOutputDeviceState->bFreezed;
					rtd_pr_vt_notice("func %s, freeze no action\n", __FUNCTION__);
				}
			} else {
				rtd_pr_vt_notice("VFOD is not inited, so no freeze cmd!\n");
			}
		}
		up(&VT_Semaphore);
		return TRUE;
	} else {
		rtd_pr_vt_notice("VT is not Inited So return False;%s=%d\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return FALSE;
	}
}
#endif // #ifndef UT_flag

unsigned int get_vt_capture_framerate(unsigned int caploc)
{
	unsigned int framerate = 0;
	ppoverlay_dvs_cnt_RBUS ppoverlay_dvs_cnt_Reg;
	ppoverlay_uzudtg_dvs_cnt_RBUS ppoverlay_uzudtg_dvs_cnt_Reg;
	rtd_pr_vt_notice("func=%s,caploc %d\n", __FUNCTION__, caploc);
	if(caploc == KADP_VT_SCALER_INPUT){
		framerate = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000)/100;
		framerate = (framerate+5)/10;
	}else if(caploc == KADP_VT_DISPLAY_OUTPUT || caploc == KADP_VT_SCALER_OUTPUT){
		ppoverlay_uzudtg_dvs_cnt_Reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DVS_cnt_reg);
		framerate = (ppoverlay_uzudtg_dvs_cnt_Reg.uzudtg_dvs_cnt == 0) ? 0 : ((XTAL_CLK-1)/ppoverlay_uzudtg_dvs_cnt_Reg.uzudtg_dvs_cnt + 1);
	}else{
		ppoverlay_dvs_cnt_Reg.regValue = IoReg_Read32(PPOVERLAY_DVS_cnt_reg);
		framerate = (ppoverlay_dvs_cnt_Reg.dvs_cnt == 0) ? 0 : ((XTAL_CLK-1)/ppoverlay_dvs_cnt_Reg.dvs_cnt + 1);
	}

	return framerate;
}

unsigned char HAL_VT_GetVideoFrameOutputDeviceFramerate(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, unsigned int *pVideoFrameOutputDeviceFramerate)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	if(get_vt_function() == TRUE) {
		*pVideoFrameOutputDeviceFramerate = get_vt_capture_framerate(VTDumpLocation);
		up(&VT_Semaphore);
		return TRUE;
	} else {
		*pVideoFrameOutputDeviceFramerate = 0;
		rtd_pr_vt_notice("VT is not Inited So return pVideoFrameOutputDeviceFramerate 0;%s=%d\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return FALSE;
	}
}

unsigned char HAL_VT_GetVideoFrameOutputDeviceDumpLocation(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, KADP_VT_DUMP_LOCATION_TYPE_T *pDumpLocation)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	if(get_vt_function() == TRUE) {
		*pDumpLocation = VTDumpLocation;
		up(&VT_Semaphore);
		return TRUE;
	} else {
		*pDumpLocation = VTDumpLocation;
		rtd_pr_vt_notice("[Warning]VT is not Inited when get dump location;%s=%d\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return FALSE;
	}
}

#ifndef UT_flag
unsigned char HAL_VT_SetVideoFrameOutputDeviceDumpLocation(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, KADP_VT_DUMP_LOCATION_TYPE_T dumpLocation)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	if(get_vt_function() == TRUE)
	{
		if(VTDumpLocation == dumpLocation) {
			rtd_pr_vt_notice("VTDumpLocation is the same as you setting\n");
		} else {
			curCapInfo[DC2H1].enable = 0; //close vfod first
			drvif_dc2h_capture_config(&curCapInfo[DC2H1]);
			curCapInfo[DC2H1].cap_location = get_dc2h_in_sel_by_location(dumpLocation);
			VTDumpLocation = dumpLocation;
		}
		up(&VT_Semaphore);
		return TRUE;
	} else {
		rtd_pr_vt_notice("[Warning]VT is not Inited when set dump location;%s=%d\n", __FUNCTION__, __LINE__);
		VTDumpLocation = dumpLocation;
		up(&VT_Semaphore);
		return FALSE;
	}
}
#endif // #ifndef UT_flag

unsigned char HAL_VT_GetVideoFrameOutputDeviceOutputInfo(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, KADP_VT_DUMP_LOCATION_TYPE_T dumpLocation, KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_OUTPUT_INFO_T *pOutputInfo)
{

	ppoverlay_main_active_h_start_end_RBUS main_active_h_start_end_reg;
	ppoverlay_main_active_v_start_end_RBUS main_active_v_start_end_reg;
	ppoverlay_main_den_h_start_end_RBUS main_den_h_start_end_Reg;
	ppoverlay_main_den_v_start_end_RBUS main_den_v_start_end_Reg;
	unsigned int x,y,w,h;

	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	//Main Active H pos
	main_active_h_start_end_reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_H_Start_End_reg);
	//Main Active V pos
	main_active_v_start_end_reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_V_Start_End_reg);

	main_den_h_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_DEN_H_Start_End_reg);
	main_den_v_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_DEN_V_Start_End_reg);

	if((get_vt_function() == TRUE)&&(dumpLocation ==VTDumpLocation )) {
		if((KADP_VT_DISPLAY_OUTPUT==dumpLocation)||(KADP_VT_OSDVIDEO_OUTPUT==dumpLocation)) {
			pOutputInfo->maxRegion.x = 0;
			pOutputInfo->maxRegion.y = 0;
			pOutputInfo->maxRegion.w = curCapInfo[DC2H1].output_size.w;
			pOutputInfo->maxRegion.h = curCapInfo[DC2H1].output_size.h;

			x = main_active_h_start_end_reg.mh_act_sta;
			y = main_active_v_start_end_reg.mv_act_sta;
			w = main_active_h_start_end_reg.mh_act_end - main_active_h_start_end_reg.mh_act_sta;
			h = main_active_v_start_end_reg.mv_act_end - main_active_v_start_end_reg.mv_act_sta;

			//scaledown:
			x = x*curCapInfo[DC2H1].output_size.w;
			x = x/(main_den_h_start_end_Reg.mh_den_end -main_den_h_start_end_Reg.mh_den_sta);

			y = y*curCapInfo[DC2H1].output_size.h;
			y = y/(main_den_v_start_end_Reg.mv_den_end - main_den_v_start_end_Reg.mv_den_sta);

			w = w*curCapInfo[DC2H1].output_size.w;
			w = w/(main_den_h_start_end_Reg.mh_den_end -main_den_h_start_end_Reg.mh_den_sta);

			h = h*curCapInfo[DC2H1].output_size.h;
			h = h/(main_den_v_start_end_Reg.mv_den_end - main_den_v_start_end_Reg.mv_den_sta);

			pOutputInfo->activeRegion.x = x;
			pOutputInfo->activeRegion.y = y;
			pOutputInfo->activeRegion.w = w;
			pOutputInfo->activeRegion.h = h;
			pOutputInfo->stride = curCapInfo[DC2H1].output_size.w;
		} else if(KADP_VT_SCALER_OUTPUT==dumpLocation) {
			pOutputInfo->maxRegion.x = 0;
			pOutputInfo->maxRegion.y = 0;
			pOutputInfo->maxRegion.w = curCapInfo[DC2H1].output_size.w;
			pOutputInfo->maxRegion.h = curCapInfo[DC2H1].output_size.h;

			pOutputInfo->activeRegion.x = 0;
			pOutputInfo->activeRegion.y = 0;
			pOutputInfo->activeRegion.w = curCapInfo[DC2H1].output_size.w;
			pOutputInfo->activeRegion.h = curCapInfo[DC2H1].output_size.h;

			pOutputInfo->stride = curCapInfo[DC2H1].output_size.w;
		}
		up(&VT_Semaphore);
		return TRUE;
	} else {
		pOutputInfo->maxRegion.x = 0;
		pOutputInfo->maxRegion.y = 0;
		pOutputInfo->maxRegion.w = 0;
		pOutputInfo->maxRegion.h = 0;

		pOutputInfo->activeRegion.x = 0;
		pOutputInfo->activeRegion.y = 0;
		pOutputInfo->activeRegion.w = 0;
		pOutputInfo->activeRegion.h = 0;

		pOutputInfo->stride = 0;

		rtd_pr_vt_notice("VFOD is not inited or vfod dump location not matches you want;%s=%d\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return FALSE;
	}
}

#ifndef UT_flag
unsigned char HAL_VT_SetVideoFrameOutputDeviceOutputRegion(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, KADP_VT_DUMP_LOCATION_TYPE_T dumpLocation, KADP_VT_RECT_T *pOutputRegion)
{
	unsigned int vt_cap_buf_nums = 0;
	if (videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	rtd_pr_vt_notice("func:%s,output w=%d,output h=%d,dumplocation=%d\n", __FUNCTION__, pOutputRegion->w,pOutputRegion->h,dumpLocation);

    if((pOutputRegion->w > _DISP_WID)||(pOutputRegion->w < VT_CAP_SCALE_DOWN_LIMIT_WIDTH)||(pOutputRegion->h > _DISP_LEN)||(pOutputRegion->h<VT_CAP_SCALE_DOWN_LIMIT_HEIGHT)) {
        rtd_pr_vt_notice("[warning]OutputRegion over limitation!!!!\n");
        up(&VT_Semaphore);
        return FALSE;
    }

	if(dumpLocation >= KADP_VT_MAX_LOCATION)
	{
		rtd_pr_vt_emerg("[error]%s=%d invalid capture location!\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return FALSE;
	}
	if((scaler_dtg_get_abnormal_dvs_long_flag(DTG_MASTER)==TRUE)||(scaler_dtg_get_abnormal_dvs_short_flag(DTG_MASTER)==TRUE)){
		rtd_pr_vt_emerg("[Warning VBE]%s=%d Vbe at abnormal dvs status,could not do vt capture!\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return FALSE;
	}

    if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_STATE) ==  _MODE_STATE_ACTIVE){
        if(get_drm_case_securestatus()==TRUE){
            rtd_pr_vt_emerg("[Warning vdecsvp]%s=%d DRM CASE source security is TRUE,could not do vt capture!\n", __FUNCTION__, __LINE__);
            up(&VT_Semaphore);
            return FALSE;
        }
    }

	if(get_vt_function() == TRUE) {
		if((dumpLocation==VTDumpLocation)&&(pOutputRegion->w==curCapInfo[DC2H1].output_size.w)&&(pOutputRegion->h==curCapInfo[DC2H1].output_size.h)) {
			rtd_pr_vt_notice("all SetVideoFrameOutputDeviceOutputRegion is the same with now\n");
		} else if((pOutputRegion->x!=0)||(pOutputRegion->y!=0)) {
			rtd_pr_vt_notice("[warning]only support output region x y is 0\n");
			up(&VT_Semaphore);
			return FALSE;
		} else {
			curCapInfo[DC2H1].enable = 0; //close vfod first
			drvif_dc2h_capture_config(&curCapInfo[DC2H1]);
			set_cap_buffer_size_by_AP(pOutputRegion->w, pOutputRegion->h);
			curCapInfo[DC2H1].output_size.x = 0;
			curCapInfo[DC2H1].output_size.y = 0;
			curCapInfo[DC2H1].output_size.w = pOutputRegion->w;
			curCapInfo[DC2H1].output_size.h = pOutputRegion->h;
			curCapInfo[DC2H1].cap_location = get_dc2h_in_sel_by_location(dumpLocation);
            if(vtPrintMask & VT_PRINT_MASK_MAX_VT_BUF_NUMS)
                vt_cap_buf_nums = VT_CAP_BUF_NUMS;
            else
                vt_cap_buf_nums = get_vt_VtBufferNum(DC2H1);
			if(Capture_BufferMemInit_VT(vt_cap_buf_nums, DC2H1) == FALSE)
			{
				rtd_pr_vt_notice("VT init allocate memory fail,%s=%d \n", __FUNCTION__, __LINE__);
				up(&VT_Semaphore);
				return FALSE;
			}

			curCapInfo[DC2H1].enable = 1;
			drvif_dc2h_capture_config(&curCapInfo[DC2H1]);
			if(KADP_VT_SCALER_OUTPUT == dumpLocation || KADP_VT_SCALER_INPUT == dumpLocation)
				VFODState.bAppliedPQ = FALSE;
			else
				VFODState.bAppliedPQ = TRUE;

			VTDumpLocation = dumpLocation;
			vfod_capture_location = VTDumpLocation;
		}
		up(&VT_Semaphore);
		return TRUE;
	} else {
		rtd_pr_vt_notice("VT is not Inited So return False;%s=%d\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return FALSE;
	}
}

unsigned char drvif_set_dc2h_wait_vsync(unsigned int in_sel, unsigned int dc2h_num)
{
	unsigned int sleeptime;
	unsigned int waitvsyncframerate;
	unsigned int vtframerate = 0;
	unsigned int st_time = 0, end_time = 0;

	ppoverlay_dvs_cnt_RBUS ppoverlay_dvs_cnt_Reg;
	ppoverlay_uzudtg_dvs_cnt_RBUS ppoverlay_uzudtg_dvs_cnt_Reg;

	if(in_sel == _OSD123_MIXER_Output || in_sel == _Dither_Output || in_sel == _Memc_mux_Output || in_sel == _OSD4_Output){
		ppoverlay_dvs_cnt_Reg.regValue = IoReg_Read32(PPOVERLAY_DVS_cnt_reg);
		vtframerate = (XTAL_CLK/ppoverlay_dvs_cnt_Reg.dvs_cnt);
	}else{
		ppoverlay_uzudtg_dvs_cnt_Reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DVS_cnt_reg);
		vtframerate = (XTAL_CLK/ppoverlay_uzudtg_dvs_cnt_Reg.uzudtg_dvs_cnt);
	}

	if(get_vt_target_fps(dc2h_num) == 0){
		if(VFODState.framerateDivide != 0)
			waitvsyncframerate = vtframerate/VFODState.framerateDivide;
		else
			waitvsyncframerate = vtframerate;
	}else{
		waitvsyncframerate = get_vt_target_fps(dc2h_num);
		if(waitvsyncframerate > vtframerate)
			waitvsyncframerate = vtframerate;
	}
	st_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg)/90;
	sleeptime = (1000/waitvsyncframerate);
	usleep_range(sleeptime*1000, sleeptime*1000);
	end_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg)/90;
	if(vtPrintMask & VT_PRINT_MASK_OUT_FPS)
		rtd_pr_vt_notice("[VT]waitVsync=%d ms\n", (end_time - st_time));

	// if(((KADP_VT_SCALER_INPUT == VTDumpLocation) || (KADP_VT_SCALER_OUTPUT == VTDumpLocation))
	// 	&& (FALSE== get_vsc_connect_ready(SLR_MAIN_DISPLAY))){
	// 	return KADP_VT_VSCSOURCE_DISCONNECT;
	// }

	return TRUE;
}

unsigned char HAL_VT_WaitVsync(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	if(get_vt_function() == TRUE) {
        unsigned int sleeptime;
        unsigned int waitvsyncframerate;
        unsigned int vtframerate;
        unsigned int st_time = 0, end_time = 0;

        vtframerate = get_vt_capture_framerate(VTDumpLocation);

        if(get_vt_target_fps(DC2H1) == 0)
        {
            if(VFODState.framerateDivide != 0)
                waitvsyncframerate = vtframerate/VFODState.framerateDivide;
            else
                waitvsyncframerate = vtframerate;
        }
        else
        {
            waitvsyncframerate = get_vt_target_fps(DC2H1);
            if(waitvsyncframerate > vtframerate)
                waitvsyncframerate = vtframerate;
        }

        st_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg)/90;
        sleeptime = (waitvsyncframerate == 0) ? (1000/VT_FPS_OUTPUT) : (1000/waitvsyncframerate);
        usleep_range(sleeptime*1000, sleeptime*1000);
        end_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg)/90;
        if(vtPrintMask & VT_PRINT_MASK_OUT_FPS)
            rtd_pr_vt_notice("[VT]waitVsync=%d ms\n", (end_time - st_time));

        if(((KADP_VT_SCALER_INPUT == VTDumpLocation) || (KADP_VT_SCALER_OUTPUT == VTDumpLocation))
            && (FALSE== get_vsc_connect_ready(SLR_MAIN_DISPLAY))){
            return KADP_VT_VSCSOURCE_DISCONNECT;
        }

		return TRUE;
	} else {
		//rtd_pr_vt_notice("VT is not Inited So return False;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
}

#endif // #ifndef UT_flag

unsigned int VT_Output_Colorspace(void)
{
    KADP_VT_VIDEO_COLOR_SPACE_T vtoutput_clorspace_state = KADP_VT_VIDEO_COLOR_SPACE_UNKNOWN;
    unsigned char pq_yuv2rgb_inputdata = 0 ;
    // dc2h_r2y_dither_4xxto4xx_dc2h_rgb2yuv_ctrl_RBUS  dc2h_rgb2yuv_ctrl_reg;
    // dc2h_rgb2yuv_ctrl_reg.regValue = rtd_inl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg);
    if(curCapInfo[DC2H1].enable == TRUE){
        if(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_get_en_rgb2yuv(rtd_inl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg)) == 0)
        {
            if((get_VT_Pixel_Format(DC2H1) == VT_CAP_NV12) || (get_VT_Pixel_Format(DC2H1) == VT_CAP_NV21))
            {
                _system_setting_info *VIP_system_info_structure_table = (_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_system_info_structure);
                if(VIP_system_info_structure_table){
                    pq_yuv2rgb_inputdata = VIP_system_info_structure_table->using_YUV2RGB_Matrix_Info.Input_Data_Mode;
                    if(pq_yuv2rgb_inputdata == 0){
                        vtoutput_clorspace_state = KADP_VT_VIDEO_COLOR_SPACE_BT601;
                    }else if(pq_yuv2rgb_inputdata == 1){
                        vtoutput_clorspace_state = KADP_VT_VIDEO_COLOR_SPACE_BT601_FULL_RANGE;
                    }else if(pq_yuv2rgb_inputdata == 2){
                        vtoutput_clorspace_state = KADP_VT_VIDEO_COLOR_SPACE_BT709;
                    }else if(pq_yuv2rgb_inputdata == 3){
                        vtoutput_clorspace_state = KADP_VT_VIDEO_COLOR_SPACE_BT709_FULL_RANGE;
                    }else if((pq_yuv2rgb_inputdata == 4) || (pq_yuv2rgb_inputdata == 5)|| (pq_yuv2rgb_inputdata == 6)|| (pq_yuv2rgb_inputdata == 7)){
                        vtoutput_clorspace_state = KADP_VT_VIDEO_COLOR_SPACE_BT2020;
                    }else {
                        vtoutput_clorspace_state = KADP_VT_VIDEO_COLOR_SPACE_UNKNOWN;
                    }
                }else{
                    vtoutput_clorspace_state = KADP_VT_VIDEO_COLOR_SPACE_UNKNOWN;
                }
            }
            else
            {
                vtoutput_clorspace_state = KADP_VT_VIDEO_COLOR_SPACE_UNKNOWN;
            }
        }
        else if(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_get_en_rgb2yuv(rtd_inl(DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg)) == 1)
        {
            vtoutput_clorspace_state = KADP_VT_VIDEO_COLOR_SPACE_BT709;
        }
        else
        {
            vtoutput_clorspace_state = KADP_VT_VIDEO_COLOR_SPACE_UNKNOWN;
        }
    }else{
        vtoutput_clorspace_state = KADP_VT_VIDEO_COLOR_SPACE_UNKNOWN;
    }

    rtd_pr_vt_notice("[VT]gvtoutput_clorspace_state =%d \n",vtoutput_clorspace_state);
    return vtoutput_clorspace_state;
}

#ifndef UT_flag
unsigned char HAL_VT_GetInputVideoInfo(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, KADP_VT_INPUT_VIDEO_INFO_T *pInputVideoInfo)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0)
	{
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	if(get_vt_function() == TRUE) {
		pInputVideoInfo->region.x = 0;
		pInputVideoInfo->region.y = 0;
		pInputVideoInfo->region.w =Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPH_ACT_WID_PRE);
		pInputVideoInfo->region.h = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE);
		if (Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_INTERLACE) == TRUE)
		{
			pInputVideoInfo->type = KADP_VT_VIDEO_INTERLACED;
			pInputVideoInfo->region.h = pInputVideoInfo->region.h *2;
		} else {
			pInputVideoInfo->type = KADP_VT_VIDEO_PROGRESSIVE;
		}
		pInputVideoInfo->bIs3DVideo = FALSE;
        pInputVideoInfo->colorSpace = VT_Output_Colorspace();
        if(((KADP_VT_SCALER_INPUT == VTDumpLocation) || (KADP_VT_SCALER_OUTPUT == VTDumpLocation))
            && (FALSE== get_vsc_connect_ready(SLR_MAIN_DISPLAY))){
            up(&VT_Semaphore);
            return KADP_VT_VSCSOURCE_DISCONNECT;
        }
		up(&VT_Semaphore);
		return TRUE;
	} else {
		pInputVideoInfo->region.x = 0;
		pInputVideoInfo->region.y =0;
		pInputVideoInfo->region.w =0;
		pInputVideoInfo->region.h = 0;
		pInputVideoInfo->type = KADP_VT_VIDEO_PROGRESSIVE;
		pInputVideoInfo->bIs3DVideo = FALSE;
        pInputVideoInfo->colorSpace = VT_Output_Colorspace();
		rtd_pr_vt_notice("VT is not Inited So return InputVideoregion 0;%s=%d\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return FALSE;
	}
}

unsigned char HAL_VT_GetOutputVideoInfo(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, KADP_VT_OUTPUT_VIDEO_INFO_T *pOutputVideoInfo)
{
	ppoverlay_main_active_h_start_end_RBUS main_active_h_start_end_reg;
	ppoverlay_main_active_v_start_end_RBUS main_active_v_start_end_reg;
	unsigned int x,y,w,h;
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	main_active_h_start_end_reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_H_Start_End_reg);
	main_active_v_start_end_reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_V_Start_End_reg);
	x = main_active_h_start_end_reg.mh_act_sta;
	y = main_active_v_start_end_reg.mv_act_sta;
	w = main_active_h_start_end_reg.mh_act_end - main_active_h_start_end_reg.mh_act_sta;
	h = main_active_v_start_end_reg.mv_act_end - main_active_v_start_end_reg.mv_act_sta;

	if(get_vt_function() == TRUE) {
		if((KADP_VT_DISPLAY_OUTPUT==VTDumpLocation)||(KADP_VT_OSDVIDEO_OUTPUT==VTDumpLocation)) {
			pOutputVideoInfo->maxRegion.x = Get_DISP_ACT_STA_HPOS();
			pOutputVideoInfo->maxRegion.y = Get_DISP_ACT_STA_VPOS();
			pOutputVideoInfo->maxRegion.w = Get_DISP_ACT_END_HPOS() - Get_DISP_ACT_STA_HPOS();
			pOutputVideoInfo->maxRegion.h = Get_DISP_ACT_END_VPOS() - Get_DISP_ACT_STA_VPOS();

			pOutputVideoInfo->activeRegion.x = x;
			pOutputVideoInfo->activeRegion.y = y;
			pOutputVideoInfo->activeRegion.w = w;
			pOutputVideoInfo->activeRegion.h = h;
			pOutputVideoInfo->type = KADP_VT_VIDEO_PROGRESSIVE;
		} else if(KADP_VT_SCALER_OUTPUT==VTDumpLocation) {
			pOutputVideoInfo->maxRegion.x = x;
			pOutputVideoInfo->maxRegion.y = y;
			pOutputVideoInfo->maxRegion.w = w;
			pOutputVideoInfo->maxRegion.h = h;

			pOutputVideoInfo->activeRegion.x = x;
			pOutputVideoInfo->activeRegion.y = y;
			pOutputVideoInfo->activeRegion.w = w;
			pOutputVideoInfo->activeRegion.h = h;

			pOutputVideoInfo->type = KADP_VT_VIDEO_PROGRESSIVE;
		}
		up(&VT_Semaphore);
		return TRUE;
	} else {
		pOutputVideoInfo->maxRegion.x = 0;
		pOutputVideoInfo->maxRegion.y = 0;
		pOutputVideoInfo->maxRegion.w = 0;
		pOutputVideoInfo->maxRegion.h = 0;

		pOutputVideoInfo->activeRegion.x = 0;
		pOutputVideoInfo->activeRegion.y = 0;
		pOutputVideoInfo->activeRegion.w = 0;
		pOutputVideoInfo->activeRegion.h = 0;

		pOutputVideoInfo->type = KADP_VT_VIDEO_PROGRESSIVE;
		up(&VT_Semaphore);
		rtd_pr_vt_notice("VT is not Inited So return OutputVideoregion 0;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
}

unsigned char HAL_VT_GetVideoMuteStatus(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, unsigned char *pbOnOff)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	if(get_vt_function() == TRUE) {
		*pbOnOff = get_vsc_mutestatus();
		up(&VT_Semaphore);
		return TRUE;
	} else {
		*pbOnOff = TRUE;
		rtd_pr_vt_notice("VT is not Inited So return MuteStatus true;%s=%d\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return FALSE;
	}
}

void set_vdec_securestatus(unsigned char value)
{
	if(TRUE == value)  // security status
	{
		if(get_dc2h_dmaen_state(DC2H1) == TRUE)
		{
			reset_dc2h_hw_setting(); /*stop and reset dc2h */
		}
	}

	VdecSecureStatus = value;
}

unsigned char get_vdec_securestatus(void)
{

	if((VdecSecureStatus == TRUE) || ((DtvSecureStatus == TRUE) && (is_DTV_flag_get() == TRUE)))
		return TRUE;  //security status
	else
		return FALSE;
}

static unsigned char check_i3ddma_input_port(unsigned int *port)
{
	unsigned int dmavgip_in_sel = 0;
	dma_vgip_dma_vgip_ctrl_RBUS dma_vgip_dma_vgip_ctrl_reg;
	dma_vgip_dma_vgip_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	if(0 == dma_vgip_dma_vgip_ctrl_reg.dma_in_clk_en){
		rtd_pr_vt_notice("dma vgip_in_clk_en disable\n", __FUNCTION__);
		return FALSE;
	}

	dmavgip_in_sel = dma_vgip_dma_vgip_ctrl_reg.dma_in_sel;
	switch(dmavgip_in_sel){
		case DMA_VGIP_SEL_HDMI1: 
			*port = 0;
		break;
		case DMA_VGIP_SEL_HDMI2:
			*port = 1;
		break;
		case DMA_VGIP_SEL_HDMI3:
			*port = 2;
		break;
		case DMA_VGIP_SEL_HDMI4:
			*port = 3;
		break;
		default:
			rtd_pr_vt_notice("%s,invalid i3ddma HDMI port %d\n", __FUNCTION__, dmavgip_in_sel);
		break;
	}
	return TRUE;
}

unsigned char get_hdcp_status_for_scaler(unsigned char display)
{
	unsigned int hdmi_port = 0xFF;
	unsigned int vgip_in_clk_en = 0, vgip_in_sel= 0, vgip_in_sel_vo = 0;
	vgip_vgip_chn1_ctrl_RBUS vgip_vgip_chn1_ctrl_reg;
	sub_vgip_vgip_chn2_ctrl_RBUS sub_vgip_vgip_chn2_ctrl_reg;
	hdr_all_top_top_ctl_RBUS hdr_all_top_top_ctl_reg;
	hdr_sub_hdr2_top_ctl_RBUS hdr_sub_hdr2_top_ctl_reg;
	hdr_all_top_top_ctl_reg.regValue = IoReg_Read32(HDR_ALL_TOP_TOP_CTL_reg);
	hdr_sub_hdr2_top_ctl_reg.regValue = IoReg_Read32(HDR_SUB_HDR2_TOP_CTL_reg);

	if(SLR_MAIN_DISPLAY == display){
        vgip_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_CTRL_reg);
        vgip_in_clk_en = vgip_vgip_chn1_ctrl_reg.ch1_in_clk_en;
        vgip_in_sel = vgip_vgip_chn1_ctrl_reg.ch1_in_sel;
        vgip_in_sel_vo = vgip_vgip_chn1_ctrl_reg.ch1_in_sel_vo;
    }else{
        sub_vgip_vgip_chn2_ctrl_reg.regValue = IoReg_Read32(SUB_VGIP_VGIP_CHN2_CTRL_reg);
        vgip_in_clk_en = sub_vgip_vgip_chn2_ctrl_reg.ch2_in_clk_en;
        vgip_in_sel = sub_vgip_vgip_chn2_ctrl_reg.ch2_in_sel;
        vgip_in_sel_vo = sub_vgip_vgip_chn2_ctrl_reg.ch2_in_sel_vo;
    }

	if(0 == vgip_in_clk_en)
    {
		rtd_pr_vt_notice("vgip_in_clk_en disable\n",__FUNCTION__);
        return FALSE;
    }
	//dc2h_cap_dc2h_cap0_wr_dma_pxltobus_reg.regValue = IoReg_Read32(DC2H_CAP_DC2H_CAP0_WR_DMA_pxltobus_reg);

	switch(vgip_in_sel){
		case VGIP_SEL_HDMI1:
			hdmi_port = 0;
		break;
		case VGIP_SEL_HDMI2:
			hdmi_port = 1;
		break;
		case VGIP_SEL_HDMI3:
			hdmi_port = 2;
		break;
		case VGIP_SEL_HDMI4:
			hdmi_port = 3;
		break;
		case VGIP_SEL_HDR1:
		{
			switch(hdr_all_top_top_ctl_reg.hdr1_in_sel){
				case HDR_SEL_HDMI0:
					hdmi_port = 0;
				break;
				case HDR_SEL_HDMI1:
					hdmi_port = 1;
				break;
				case HDR_SEL_HDMI2:
					hdmi_port = 2;
				break;
				case HDR_SEL_HDMI3:
					hdmi_port = 3;
				break;
				case HDR_SEL_VO:
				{
					if(IV_SRC_SEL_HDMIDMAVS == VODMA_VODMA_PVS0_Gen_iv_src_sel(IoReg_Read32(VODMA_VODMA_PVS0_Gen_reg))){
						if(check_i3ddma_input_port(&hdmi_port) == FALSE){
							return FALSE;
						}
					}
				}
				break;
				default:
					rtd_pr_vt_notice("invalid HDR1 hdmi port %d\n",__FUNCTION__, hdr_all_top_top_ctl_reg.hdr1_in_sel);
				break;
			}
		}
		break;
		case VGIP_SEL_HDR2:
		{
			switch(hdr_sub_hdr2_top_ctl_reg.hdr2_in_sel){
				case HDR_SEL_HDMI0:
					hdmi_port = 0;
				break;
				case HDR_SEL_HDMI1:
					hdmi_port = 1;
				break;
				case HDR_SEL_HDMI2:
					hdmi_port = 2;
				break;
				case HDR_SEL_HDMI3:
					hdmi_port = 3;
				break;
				case HDR_SEL_VO:
				{
					if(IV_SRC_SEL_HDMIDMAVS == VODMA2_VODMA2_PVS0_Gen_get_iv_src_sel(IoReg_Read32(VODMA2_VODMA2_PVS0_Gen_reg))){
                        if(check_i3ddma_input_port(&hdmi_port) == FALSE){
							return FALSE;
						}
					}
				}
				break;
				default:
					rtd_pr_vt_notice("invalid HDR2 hdmi port %d\n",__FUNCTION__, hdr_sub_hdr2_top_ctl_reg.hdr2_in_sel);
				break;
			}
		}
		break;
		case VGIP_SEL_VODMA:
		{
			if(((VGIP_SEL_VODMA2 == vgip_in_sel_vo) && (IV_SRC_SEL_HDMIDMAVS == VODMA2_VODMA2_PVS0_Gen_get_iv_src_sel(IoReg_Read32(VODMA2_VODMA2_PVS0_Gen_reg))))
                || ((VGIP_SEL_VODMA1 == vgip_in_sel_vo) && (IV_SRC_SEL_HDMIDMAVS == VODMA_VODMA_PVS0_Gen_get_iv_src_sel(IoReg_Read32(VODMA_VODMA_PVS0_Gen_reg))))){
                        if(check_i3ddma_input_port(&hdmi_port) == FALSE){
							return FALSE;
						}
			}
		}
		break;
		default:
			rtd_pr_vt_notice("%s, invalid vgip in sel %d\n",__FUNCTION__, vgip_in_sel);
		break;
	}

	if(hdmi_port == 0){
		return DC2H_CAP_DC2H_CAP0_WR_DMA_pxltobus_get_hdcp0_enc_en(IoReg_Read32(DC2H_CAP_DC2H_CAP0_WR_DMA_pxltobus_reg));
	}else if(hdmi_port == 1){
		return DC2H_CAP_DC2H_CAP0_WR_DMA_pxltobus_get_hdcp1_enc_en(IoReg_Read32(DC2H_CAP_DC2H_CAP0_WR_DMA_pxltobus_reg));
	}else if(hdmi_port == 2){
		return DC2H_CAP_DC2H_CAP0_WR_DMA_pxltobus_get_hdcp2_enc_en(IoReg_Read32(DC2H_CAP_DC2H_CAP0_WR_DMA_pxltobus_reg));
	}else if(hdmi_port == 3){
		return DC2H_CAP_DC2H_CAP0_WR_DMA_pxltobus_get_hdcp3_enc_en(IoReg_Read32(DC2H_CAP_DC2H_CAP0_WR_DMA_pxltobus_reg));
	}else{
		rtd_pr_vt_notice("%s, invalid port %d\n",__FUNCTION__, hdmi_port);
		return FALSE;
	}
}

unsigned char get_drm_case_securestatus(void)
{
    if(get_hdcp_status_for_scaler(SLR_MAIN_DISPLAY)){
		return TRUE;
	}else if(get_hdcp_status_for_scaler(SLR_SUB_DISPLAY)){
		return TRUE;
	}else if(get_vdec_securestatus()){
		return TRUE;
	}else{
		return FALSE;
	}
}

void set_dtv_securestatus(unsigned char status)
{
	if(TRUE == status)  //dtv security status
	{
		if(get_dc2h_dmaen_state(DC2H1) == TRUE)
		{
			reset_dc2h_hw_setting(); /*stop and reset dc2h */
		}
	}

    DtvSecureStatus = status;
}

unsigned char HAL_VT_GetVideoFrameBufferSecureVideoState(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, unsigned char *pIsSecureVideo)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	*pIsSecureVideo = get_drm_case_securestatus();

	return TRUE;
}


static unsigned char VTBlockStatus = FALSE;
unsigned char HAL_VT_GetVideoFrameOutputDeviceBlockState(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, unsigned char *pbBlockState)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	down(&VTBlock_Semaphore);
	*pbBlockState = VTBlockStatus;
	up(&VTBlock_Semaphore);
	return TRUE;
}

unsigned char HAL_VT_SetVideoFrameOutputDeviceBlockState(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, unsigned char bBlockState)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	down(&VTBlock_Semaphore);
	VTBlockStatus = bBlockState;
	up(&VTBlock_Semaphore);
	return TRUE;
}

unsigned char HAL_VT_set_Pixel_Format(VT_CAP_FMT value)
{
	down(&VT_Semaphore);
	if((value >= VT_CAP_RGB888)&&(value < VT_CAP_MAX)) {
		if (value != get_VT_Pixel_Format(DC2H1)) {
			rtd_pr_vt_notice("%s=%d\n", __FUNCTION__, value);
			set_VT_Pixel_Format(value, DC2H1);
			curCapInfo[DC2H1].fmt = value;
			curCapInfo[DC2H1].enable = 0;
			drvif_dc2h_capture_config(&curCapInfo[DC2H1]);
			Capture_BufferMemDeInit_VT(DC2H1);
		}
		up(&VT_Semaphore);
		return TRUE;
	} else {
		up(&VT_Semaphore);
		return FALSE;
	}
}

void drvif_memc_outBg_ctrl(bool enable)
{
	unsigned int timeoutcount = 100;
	ppoverlay_double_buffer_ctrl3_RBUS ppoverlay_double_buffer_ctrl3_reg;
	ppoverlay_memc_mux_ctrl_bg_RBUS memc_mux_ctrl_bg_reg;

	ppoverlay_double_buffer_ctrl3_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
	ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_en = 1;
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);

	/* MEMC_Out_Bg_en  ctrl */
	memc_mux_ctrl_bg_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg);
	memc_mux_ctrl_bg_reg.memc_out_bg_en = enable;
	IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg, memc_mux_ctrl_bg_reg.regValue);

	/* set apply */
	ppoverlay_double_buffer_ctrl3_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
	ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_set = 1;
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);

   /* wait apply */
	do{
		if(PPOVERLAY_Double_Buffer_CTRL3_get_d2bg_dtgreg_dbuf_set(rtd_inl(PPOVERLAY_Double_Buffer_CTRL3_reg))){
			timeoutcount--;
			msleep(0);
		}
		else
			break;

	}while(timeoutcount);

	if(timeoutcount == 0)
	{
		rtd_pr_vt_notice("memc_out_bg_en apply timeout\n");
	}

}


unsigned char HAL_VT_EnableFRCMode(unsigned char bEnableFRC)
{
	down(&VT_Semaphore);
	rtd_pr_vt_notice("+++ %s=%d bEnableFRC=%d!\n", __FUNCTION__, __LINE__,bEnableFRC);

	if(get_vt_EnableFRCMode()==bEnableFRC){
		rtd_pr_vt_notice("%s=%d bEnableFRC same,no need set again!\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return TRUE;
	}

	set_vt_EnableFRCMode(bEnableFRC);
	if(bEnableFRC == FALSE){
		if((get_film_mode_parameter().enable == _DISABLE) /*&& (Get_DISPLAY_REFRESH_RATE() != 120)*/){
			if(1 /*(Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) > 2560) &&
				(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE) > 1440)*/){
				if(1 /*Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) > 49000*/){
					Check_smooth_toggle_update_flag(SLR_MAIN_DISPLAY);
					drvif_memc_outBg_ctrl(_ENABLE); /* memc mute black */
					//Scaler_MEMC_outMux(_ENABLE,_DISABLE); //db on, mux off
					//Scaler_MEMC_Bypass_On(_ENABLE);
					//rtd_pr_vt_notice("[VR360] memc bypass done!!\n");
					Scaler_MEMC_MEMC_CLK_OnOff(0, 0, 1);	//bypass MEMC with mute on
					rtd_pr_vt_notice("[VR360]memc mute and disable clock done!\n");
				}
				else{
					rtd_pr_vt_notice("[VR360] 4k timing enter vt_frc_mode, set mc on!!\n");
					Check_smooth_toggle_update_flag(SLR_MAIN_DISPLAY);
					Scaler_MEMC_MEMC_CLK_OnOff(0, 1, 1);	//Set MC on with mute on
					Scaler_MEMC_outMux(_ENABLE,_ENABLE);	//db on, mux on
					rtd_pr_vt_notice("[VR360] memc enable done!!\n");
				}
			}
			else if(Scaler_get_vdec_2k120hz()){
				rtd_pr_vt_notice("[VR360] 2k120Hz enter vt_frc_mode, set memc bypass!!\n");
				Scaler_MEMC_outMux(_ENABLE,_DISABLE);	//db on, mux off
				Scaler_MEMC_MEMC_CLK_OnOff(0, 0, 1);	// bypass MEMC with mute on
			}
			else{
				rtd_pr_vt_notice("[VR360] not 4k timing enter vt_frc_mode, set memc on!!\n");
				Scaler_MEMC_MEMC_CLK_OnOff(1, 1, 1);	//Set MEMC on with mute on
				Scaler_MEMC_outMux(_ENABLE,_ENABLE);	//db on, mux off
			}
		}
#ifndef CONFIG_MEMC_BYPASS
		//EnableFRCMode is FALSE,pls close FRC
		MEMC_Set_malloc_address(bEnableFRC);
#endif
	}
	else{
		//EnableFRCMode is TRUE,pls reopen FRC,however,webos never set HAL_VT_EnableFRCMode(TRUE), so need reopen at HAL_VT_Finalize
#ifndef CONFIG_MEMC_BYPASS
		MEMC_Set_malloc_address(bEnableFRC);
#endif
	}

	up(&VT_Semaphore);
	return TRUE;

}

unsigned char HAL_VT_Set_OutFps(unsigned int outfps)
{
    down(&VT_Semaphore);
    uiTargetFps[DC2H1] = outfps;
    rtd_pr_vt_notice("ap set outfps=%d\n", uiTargetFps[DC2H1]);
    up(&VT_Semaphore);
    return TRUE;
}

unsigned char HAL_VT_Set_Enqueue_index(unsigned int idx)
{
    down(&VT_Semaphore_enq);
	spin_lock(&vt_buf_status_Spinlock);
    set_vt_buf_enqueue(DC2H1, idx);
	vt_dequeue_enqueue_count_subtract(DC2H1);
	spin_unlock(&vt_buf_status_Spinlock);
    if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX)
        rtd_pr_vt_notice("enqEnd 90K %d\n", IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg));

    up(&VT_Semaphore_enq);
    return TRUE;
}

void drvif_vt_set_stream_off(unsigned int dc2h_num)
{
	curCapInfo[dc2h_num].enable = 0;
	if(dc2h2_pqc_path_enable == TRUE){
		drvif_dc2h2_pqc_capture_config(&curCapInfo[dc2h_num]);
		dc2h2_pqc_path_enable = FALSE;
	}else{
		if(dc2h_num == DC2H1){
			drvif_dc2h_capture_config(&curCapInfo[DC2H1]);
		}else{
			drvif_dc2h2_capture_config(&curCapInfo[DC2H2]);
		}
	}
}

void drvif_vt_release_capture_cma_buf(unsigned int dc2h_num)
{
	int i;

	for (i = 0; i < get_vt_VtBufferNum(dc2h_num); i++)
	{
		if(VtAllocatedBufferStartAdress[dc2h_num][i])
		{
			pli_free(VtAllocatedBufferStartAdress[dc2h_num][i]);
			VtAllocatedBufferStartAdress[dc2h_num][i] = 0;
		}
	}

	for (i = 0; i < get_vt_VtBufferNum(dc2h_num); i++)
	{
		if (CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y)
		{
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_y = 0;
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_u = 0;
			CapBuffer_VT[dc2h_num].vt_pa_buf_info[i].pa_v = 0;
		}
	}

	CapBuffer_VT[dc2h_num].image_size = 0;
	CapBuffer_VT[dc2h_num].buf_size = 0;
	CapBuffer_VT[dc2h_num].mem_type = DC2H_MEMORY_MMAP;
	rtd_pr_vt_notice("[VT memory free] DC2H %d Deinit done!\n", dc2h_num);
}

void drvif_vt_finalize(unsigned int dc2h_num)
{
	set_vt_VtSwBufferMode(FALSE, dc2h_num);
	set_vt_VtBufferNum(0, dc2h_num);
	IndexOfFreezedVideoFrameBuffer[dc2h_num] = 0;
	abnormal_dvs_cnt = 0;
	wait_dvs_stable_cnt = 0;
	reset_vt_buf_status();
	set_vt_target_fps(0, dc2h_num);
    set_vt_dequeue_enqueue_count(0, dc2h_num);
	dc2h2_pqc_path_enable = FALSE;
	set_vt_dc2h_crop_enable(FALSE, dc2h_num);
}

unsigned char drvif_vt_set_enqueue_index(unsigned int idx, unsigned int dc2h_num)
{
	down(&VT_Semaphore_enq);
	spin_lock(&vt_buf_status_Spinlock);
	set_vt_buf_enqueue(dc2h_num, idx);
	vt_dequeue_enqueue_count_subtract(dc2h_num);
	spin_unlock(&vt_buf_status_Spinlock);
	if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX){
        rtd_pr_vt_notice("enqEnd 90K %d\n", IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg));
	}
	up(&VT_Semaphore_enq);
	return TRUE;
}

void drvif_set_dc2h_capture_freeze(unsigned char onoff, unsigned int dc2h_num)
{
	unsigned int timeoutcount = 100;
	unsigned int dc2h_cap_color_format_reg[DC2H_NUMBER] = {DC2H_CAP_DC2H_CAP_Color_format_reg, DC2H2_CAP_DC2H2_CAP_Color_format_reg};
	unsigned int dc2h_cap_double_buffer_reg[DC2H_NUMBER] = {DC2H_CAP_DC2H_CAP_doublebuffer_reg, DC2H2_CAP_DC2H2_CAP_doublebuffer_reg};
	unsigned int dc2h_db_en_mask[DC2H_NUMBER] = {DC2H_CAP_DC2H_CAP_doublebuffer_dc2h_db_en_mask, DC2H2_CAP_DC2H2_CAP_doublebuffer_dc2h2_db_en_mask};
	unsigned int dc2h_db_apply_mask[DC2H_NUMBER] = {DC2H_CAP_DC2H_CAP_doublebuffer_dc2h_db_apply_mask, DC2H2_CAP_DC2H2_CAP_doublebuffer_dc2h2_db_apply_mask};
	unsigned int dc2h_cap_en_mask[DC2H_NUMBER] = {DC2H_CAP_DC2H_CAP_Color_format_dc2h_cap0_en_mask|DC2H_CAP_DC2H_CAP_Color_format_dc2h_cap1_en_mask|DC2H_CAP_DC2H_CAP_Color_format_dc2h_cap2_en_mask,
	DC2H2_CAP_DC2H2_CAP_Color_format_dc2h2_cap0_en_mask|DC2H2_CAP_DC2H2_CAP_Color_format_dc2h2_cap1_en_mask|DC2H2_CAP_DC2H2_CAP_Color_format_dc2h2_cap2_en_mask};

	rtd_pr_vt_notice("fun:%s, onff %d\n", __FUNCTION__, onoff);

	if(onoff == TRUE){//freeze
		IoReg_SetBits(dc2h_cap_double_buffer_reg[dc2h_num], dc2h_db_en_mask[dc2h_num]);
		IoReg_SetBits(dc2h_cap_double_buffer_reg[dc2h_num], dc2h_db_apply_mask[dc2h_num]);
		do{		
			if(IoReg_Read32(dc2h_cap_double_buffer_reg[dc2h_num]) & dc2h_db_apply_mask[dc2h_num]){
				timeoutcount--;
				msleep(0);
			}
			else{
				break;
			}

		}while(timeoutcount);
		if(timeoutcount == 0)
		{
			rtd_pr_vt_notice("fun:%s,%d, freeze before action timeout\n", __FUNCTION__, __LINE__);
		}

		IoReg_ClearBits(dc2h_cap_color_format_reg[dc2h_num], dc2h_cap_en_mask[dc2h_num]); //disable, will be apllied immediately
		IoReg_ClearBits(dc2h_cap_double_buffer_reg[dc2h_num], dc2h_db_en_mask[dc2h_num]);
		IndexOfFreezedVideoFrameBuffer[dc2h_num] = 0;
	}else{
		IoReg_SetBits(dc2h_cap_double_buffer_reg[dc2h_num], dc2h_db_en_mask[dc2h_num]);
		if(CapBuffer_VT[dc2h_num].plane_number == 1){
			IoReg_SetBits(dc2h_cap_color_format_reg[dc2h_num], dc2h_num == DC2H1 ? DC2H_CAP_DC2H_CAP_Color_format_dc2h_cap0_en_mask : DC2H2_CAP_DC2H2_CAP_Color_format_dc2h2_cap0_en_mask);
		}else if(CapBuffer_VT[dc2h_num].plane_number == 2){
			IoReg_SetBits(dc2h_cap_color_format_reg[dc2h_num], dc2h_num == DC2H1 ? (DC2H_CAP_DC2H_CAP_Color_format_dc2h_cap0_en_mask|DC2H_CAP_DC2H_CAP_Color_format_dc2h_cap1_en_mask) : (DC2H2_CAP_DC2H2_CAP_Color_format_dc2h2_cap0_en_mask|DC2H2_CAP_DC2H2_CAP_Color_format_dc2h2_cap1_en_mask));
		}else if(CapBuffer_VT[dc2h_num].plane_number == 3){
			IoReg_SetBits(dc2h_cap_color_format_reg[dc2h_num], dc2h_cap_en_mask[dc2h_num]);
		}

		IoReg_SetBits(dc2h_cap_double_buffer_reg[dc2h_num], dc2h_db_apply_mask[dc2h_num]);
		do{		
			if(IoReg_Read32(dc2h_cap_double_buffer_reg[dc2h_num]) & dc2h_db_apply_mask[dc2h_num]){
				timeoutcount--;
				msleep(0);
			}
			else{
				break;
			}

		}while(timeoutcount);
		if(timeoutcount == 0)
		{
			rtd_pr_vt_notice("fun:%s,%d, unfreeze action timeout\n", __FUNCTION__, __LINE__);
		}

		IoReg_ClearBits(dc2h_cap_double_buffer_reg[dc2h_num], dc2h_db_en_mask[dc2h_num]);
	}
}

unsigned char drvif_vt_get_dequeue_index(unsigned int *pIndex, unsigned int dc2h_num)
{
	//down(&VT_Semaphore);
	static unsigned char vt_first_dequeue_flag = TRUE;
	unsigned int bufnum = get_vt_VtBufferNum(dc2h_num);

	if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX){
		rtd_pr_vt_notice("deq 90K %d\n", IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg));
	}

	if(bufnum == 1){
		if(VFODState.bFreezed == TRUE) {
			drvif_set_dc2h_capture_freeze(FALSE, dc2h_num);
			VFODState.bFreezed = FALSE;
		}

		if(VFODState.bFreezed == FALSE){
			drvif_set_dc2h_capture_freeze(TRUE, dc2h_num);
			VFODState.bFreezed = TRUE;
		}

		*pIndex = IndexOfFreezedVideoFrameBuffer[dc2h_num];
	}else{

		if(get_vt_dequeue_enqueue_count(dc2h_num) >= AP_MAX_HOLD_NUM)
		{
			*pIndex = VT_NOT_ENOUGH_RESOURCE;
			rtd_pr_vt_notice("dq at least %d buffers already,no buffer more\n", AP_MAX_HOLD_NUM);
			return TRUE;
		}

		if(vt_first_dequeue_flag == TRUE)
		{
			vt_first_dequeue_flag = FALSE;
			IndexOfFreezedVideoFrameBuffer[dc2h_num] = (htonl(pDc2h_BufStatus->write[dc2h_num]) + bufnum - 1)%bufnum;
		}
		else
		{
			if(((htonl(pDc2h_BufStatus->read[dc2h_num]) + 1)%bufnum) == htonl(pDc2h_BufStatus->write[dc2h_num])){
				//*pIndex = VT_NOT_ENOUGH_RESOURCE;
				IndexOfFreezedVideoFrameBuffer[dc2h_num] = htonl(pDc2h_BufStatus->read[dc2h_num]);
                set_vt_buf_dequeue(dc2h_num, IndexOfFreezedVideoFrameBuffer[dc2h_num]);
				rtd_pr_vt_notice("dequeue repeat last index %d\n", IndexOfFreezedVideoFrameBuffer[dc2h_num]);
				//up(&VT_Semaphore);
				return TRUE;
			}else{
				IndexOfFreezedVideoFrameBuffer[dc2h_num] = (htonl(pDc2h_BufStatus->read[dc2h_num]) + 1)%bufnum;
			}
		}

		*pIndex = IndexOfFreezedVideoFrameBuffer[dc2h_num];
		spin_lock(&vt_buf_status_Spinlock);
		set_vt_buf_dequeue(dc2h_num, IndexOfFreezedVideoFrameBuffer[dc2h_num]);
		vt_dequeue_enqueue_count_add(dc2h_num);
		spin_unlock(&vt_buf_status_Spinlock);
		pDc2h_BufStatus->read[dc2h_num] = Scaler_ChangeUINT32Endian(IndexOfFreezedVideoFrameBuffer[dc2h_num]);

		if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX){
			rtd_pr_vt_notice("sta %d,%d,%d,%d,%d,dq_idx=%d\n", \
				htonl(pDc2h_BufStatus->bufStatus[dc2h_num].bufSta[0]),\
				htonl(pDc2h_BufStatus->bufStatus[dc2h_num].bufSta[1]),\
				htonl(pDc2h_BufStatus->bufStatus[dc2h_num].bufSta[2]),\
				htonl(pDc2h_BufStatus->bufStatus[dc2h_num].bufSta[3]),\
				htonl(pDc2h_BufStatus->bufStatus[dc2h_num].bufSta[4]),\
				IndexOfFreezedVideoFrameBuffer[dc2h_num]);
		}
	}
	return TRUE;
}

unsigned char HAL_VT_Get_Dequeue_index(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, unsigned int *pIndex)
{
	static unsigned char ucOnce_Drm_case_flag = FALSE;
	dc2h_cap_dc2h_interrupt_enable_RBUS dc2h_interrupt_enable_reg;
	dc2h_cap_dc2h_cap_color_format_RBUS dc2h_cap_color_format_reg;
    if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
        rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    down(&VT_Semaphore);
#if 0
// method 1: echo vtmask_00001200 > /dev/vivtdev
    if(vtPrintMask & VT_PRINT_MASK_DEQUEUE_TWO_PATTERN) {
        static unsigned int auto_switch_flag = 0;

        if(auto_switch_flag == 300)
            auto_switch_flag = 0;

        auto_switch_flag += 1;

        if(auto_switch_flag < 150) {
            static unsigned char idx_1 = 0;
            if(idx_1 > 2) idx_1 = 0;
            *pIndex = idx_1;
            //rtd_pr_vt_notice("###[%s(%d)] VT force return %d\n", __FUNCTION__, __LINE__,idx_1);
            idx_1+=1;
            up(&VT_Semaphore);
            return TRUE;
        } else {
            static unsigned char idx_2 = 3;
            if(idx_2 > 5) idx_2 = 3;
            *pIndex = idx_2;
            // rtd_pr_vt_notice("###[%s(%d)] VT force return %d\n", __FUNCTION__, __LINE__,idx_2);
            idx_2+=1;
            up(&VT_Semaphore);
            return TRUE;
        }
    }
#endif

    if(get_vt_function() == TRUE)
	{
        if(get_drm_case_securestatus() == TRUE){
            disable_dc2h_inten_and_dmaen();
            if(vtPrintMask & VT_PRINT_MASK_DRM_LOG)
                rtd_pr_vt_notice("[VT]get_drm_case_securestatus is true;%s=%d\n", __FUNCTION__, __LINE__);
            *pIndex = IndexOfFreezedVideoFrameBuffer[DC2H1];
            ucOnce_Drm_case_flag = TRUE;
            up(&VT_Semaphore);
            return TRUE;
        }
        else{
            if((get_vt_VtSwBufferMode(DC2H1)) && (DC2H_CAP_DC2H_Interrupt_Enable_get_dc2h_vs_rising_int_en(IoReg_Read32(DC2H_CAP_DC2H_Interrupt_Enable_reg) == 0)))
            {
               dc2h_interrupt_enable_reg.regValue = IoReg_Read32(DC2H_CAP_DC2H_Interrupt_Enable_reg);
               dc2h_interrupt_enable_reg.dc2h_vs_rising_int_en = 1;
			   dc2h_interrupt_enable_reg.dc2h_cap0_done_int_en = 1;
               IoReg_Write32(DC2H_CAP_DC2H_Interrupt_Enable_reg, dc2h_interrupt_enable_reg.regValue);
               msleep(20);
            }

            if(ucOnce_Drm_case_flag == TRUE){
                ucOnce_Drm_case_flag = FALSE;
				dc2h_cap_color_format_reg.regValue = IoReg_Read32(DC2H_CAP_DC2H_CAP_Color_format_reg);
				dc2h_cap_color_format_reg.dc2h_cap0_en = 1;
				if(CapBuffer_VT[DC2H1].plane_number > KADP_VT_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED){
					dc2h_cap_color_format_reg.dc2h_cap1_en = 1;
				}
				if(CapBuffer_VT[DC2H1].plane_number > KADP_VT_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR){
					dc2h_cap_color_format_reg.dc2h_cap2_en = 1;
				}
				IoReg_Write32(DC2H_CAP_DC2H_CAP_Color_format_reg, dc2h_cap_color_format_reg.regValue);
            }
        }

		drvif_vt_get_dequeue_index(pIndex, DC2H1);
        up(&VT_Semaphore);
        return TRUE;
    }else{
        rtd_pr_vt_notice("VT is not Inited So return False;%s=%d\n", __FUNCTION__, __LINE__);
        up(&VT_Semaphore);
        return FALSE;
    }
}

unsigned char HAL_VT_Set_App_Case(VT_APP_CASE type)
{
    /*down(&VT_Semaphore);
    if(type == VT_APP_CASE_LIVE_STREAM)
    {
        if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI && Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPH_ACT_WID_PRE) < 3800 &&
            get_rtk_qos_current_mode() != RTK_QOS_STREAMING_HDMI_2K_MODE)//2k hdmi
        {
            vt_back_qos_current_mode(get_rtk_qos_current_mode());
            rtk_qos_set_mode(RTK_QOS_STREAMING_HDMI_2K_MODE);
        }
        else if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC && Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPH_ACT_WID_PRE) >= 3800 &&
            get_rtk_qos_current_mode() != RTK_QOS_STREAMING_DECODER_4K_MODE)//4k vdec
        {
            vt_back_qos_current_mode(get_rtk_qos_current_mode());
            rtk_qos_set_mode(RTK_QOS_STREAMING_DECODER_4K_MODE);
        }
    }
    else if(type == VT_APP_CASE_NONE)
    {
        rtk_qos_set_mode(pre_rtk_qos_mode);
    }
    up(&VT_Semaphore);*/
    return TRUE;
}

unsigned char drv_set_vt_dc2h_crop(unsigned int dc2h_num, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	unsigned int timeoutcount = 200;
	if((x + w) > _DISP_WID || (y + h) > _DISP_LEN){
		rtd_pr_vt_notice("cropSize out range(%d,%d,%d,%d),%s=%d \n",x, y, w, h, __FUNCTION__, __LINE__);
		return FALSE;
	}
	if(dc2h_num >= DC2HX_MAXN){
		return FALSE;
	}
	if(w < curCapInfo[dc2h_num].output_size.w || h < curCapInfo[dc2h_num].output_size.h){
		rtd_pr_vt_notice("dc2h path no scale up, but crop w %d or h %d over output w %d or h %d\n", w, h, curCapInfo[dc2h_num].output_size.w, curCapInfo[dc2h_num].output_size.h);
		return FALSE;
	}

	rtd_pr_vt_notice("[DC2H%d]crop (%d,%d,%d,%d)\n", dc2h_num, x, y, w, h);
	if(dc2h_num == DC2H1){
		dc2h_cap_dc2h_3dmaskto2d_ctrl_RBUS dc2h_cap_3dmaskto2d_ctrl_reg;
		//if crop en ,keep crop size = output size
		if(x == curCapInfo[dc2h_num].crop_size.x && y == curCapInfo[dc2h_num].crop_size.y && w == curCapInfo[dc2h_num].crop_size.w && h == curCapInfo[dc2h_num].crop_size.h){
			rtd_pr_vt_notice("cropSize not changed(%d,%d,%d,%d)\n",x, y, w, h);
			return TRUE;
		}
		if(w != curCapInfo[dc2h_num].output_size.w || h != curCapInfo[dc2h_num].output_size.h){
			rtd_pr_vt_notice("cropSize wid and hei should equal output size\n");
			return FALSE;
		}
		IoReg_SetBits(DC2H_CAP_DC2H_3DMaskTo2D_doublebuffer_reg, DC2H_CAP_DC2H_3DMaskTo2D_doublebuffer_dc2h_3dmaskto2d_db_en_mask);
		IoReg_SetBits(DC2H_SCALEDOWN_DC2H1_UZD_H_DB_CTRL_reg, DC2H_SCALEDOWN_DC2H1_UZD_H_DB_CTRL_h_db_en_mask);
		IoReg_SetBits(DC2H_SCALEDOWN_DC2H1_UZD_V_DB_CTRL_reg, DC2H_SCALEDOWN_DC2H1_UZD_V_DB_CTRL_v_db_en_mask);
		drvif_set_dc2h_crop_config(x, y, w, h, TRUE);
		drvif_set_dc2h_scale_down_config(curCapInfo[dc2h_num].crop_size, curCapInfo[dc2h_num].output_size);
		IoReg_ClearBits(DC2H_CAP_DC2H_3DMaskTo2D_Ctrl_reg, DC2H_CAP_DC2H_3DMaskTo2D_Ctrl_dc2h_3dmaskto2d_h_size_mask);
		IoReg_SetBits(DC2H_SCALEDOWN_DC2H1_UZD_H_DB_CTRL_reg, DC2H_SCALEDOWN_DC2H1_UZD_H_DB_CTRL_h_db_apply_mask);
		IoReg_SetBits(DC2H_SCALEDOWN_DC2H1_UZD_V_DB_CTRL_reg, DC2H_SCALEDOWN_DC2H1_UZD_V_DB_CTRL_v_db_apply_mask);
		IoReg_SetBits(DC2H_CAP_DC2H_3DMaskTo2D_doublebuffer_reg, DC2H_CAP_DC2H_3DMaskTo2D_doublebuffer_dc2h_3dmaskto2d_db_apply_mask);
		do{		
			if(DC2H_CAP_DC2H_3DMaskTo2D_doublebuffer_get_dc2h_3dmaskto2d_db_apply(IoReg_Read32(DC2H_CAP_DC2H_3DMaskTo2D_doublebuffer_reg))){
				timeoutcount--;
				msleep(0);
			}
			else{
				break;
			}

		}while(timeoutcount);
		if(timeoutcount == 0)
		{
			rtd_pr_vt_notice("fun:%s,%d, wait db apply clear timeout\n", __FUNCTION__, __LINE__);
		}
		dc2h_cap_3dmaskto2d_ctrl_reg.regValue = IoReg_Read32(DC2H_CAP_DC2H_3DMaskTo2D_Ctrl_reg);
		dc2h_cap_3dmaskto2d_ctrl_reg.dc2h_3dmaskto2d_h_size = curCapInfo[dc2h_num].input_size.w;
		IoReg_Write32(DC2H_CAP_DC2H_3DMaskTo2D_Ctrl_reg, dc2h_cap_3dmaskto2d_ctrl_reg.regValue);
		IoReg_SetBits(DC2H_CAP_DC2H_3DMaskTo2D_doublebuffer_reg, DC2H_CAP_DC2H_3DMaskTo2D_doublebuffer_dc2h_3dmaskto2d_db_apply_mask);
	}else{
		dc2h2_cap_dc2h2_3dmaskto2d_ctrl_RBUS dc2h2_cap_3dmaskto2d_ctrl_reg;
		//if crop en already,keep crop size = output size
		if(x == curCapInfo[dc2h_num].crop_size.x && y == curCapInfo[dc2h_num].crop_size.y && w == curCapInfo[dc2h_num].crop_size.w && h == curCapInfo[dc2h_num].crop_size.h){
			rtd_pr_vt_notice("cropSize not changed(%d,%d,%d,%d)\n",x, y, w, h);
			return TRUE;
		}
		if(w != curCapInfo[dc2h_num].output_size.w || h != curCapInfo[dc2h_num].output_size.h){
			rtd_pr_vt_notice("cropSize wid and hei should equal output size\n");
			return FALSE;
		}
		IoReg_SetBits(DC2H2_CAP_DC2H2_3DMaskTo2D_doublebuffer_reg, DC2H2_CAP_DC2H2_3DMaskTo2D_doublebuffer_dc2h2_3dmaskto2d_db_en_mask);
		IoReg_SetBits(DC2H2_SCALEDOWN_DC2H2_UZD_H_DB_CTRL_reg, DC2H2_SCALEDOWN_DC2H2_UZD_H_DB_CTRL_h_db_en_mask);
		IoReg_SetBits(DC2H2_SCALEDOWN_DC2H2_UZD_V_DB_CTRL_reg, DC2H2_SCALEDOWN_DC2H2_UZD_V_DB_CTRL_v_db_en_mask);
		drvif_set_dc2h2_crop_config(x, y, w, h, TRUE);
		drvif_set_dc2h2_scale_down_config(curCapInfo[dc2h_num].crop_size, curCapInfo[dc2h_num].output_size);
		IoReg_ClearBits(DC2H2_CAP_DC2H2_3DMaskTo2D_Ctrl_reg, DC2H2_CAP_DC2H2_3DMaskTo2D_Ctrl_dc2h2_3dmaskto2d_h_size_mask);
		IoReg_SetBits(DC2H2_SCALEDOWN_DC2H2_UZD_H_DB_CTRL_reg, DC2H2_SCALEDOWN_DC2H2_UZD_H_DB_CTRL_h_db_apply_mask);
		IoReg_SetBits(DC2H2_SCALEDOWN_DC2H2_UZD_V_DB_CTRL_reg, DC2H2_SCALEDOWN_DC2H2_UZD_V_DB_CTRL_v_db_apply_mask);
		IoReg_SetBits(DC2H2_CAP_DC2H2_3DMaskTo2D_doublebuffer_reg, DC2H2_CAP_DC2H2_3DMaskTo2D_doublebuffer_dc2h2_3dmaskto2d_db_apply_mask);
		do{		
			if(DC2H2_CAP_DC2H2_3DMaskTo2D_doublebuffer_get_dc2h2_3dmaskto2d_db_apply(IoReg_Read32(DC2H2_CAP_DC2H2_3DMaskTo2D_doublebuffer_reg))){
				timeoutcount--;
				msleep(0);
			}
			else{
				break;
			}

		}while(timeoutcount);
		if(timeoutcount == 0)
		{
			rtd_pr_vt_notice("fun:%s,%d, wait db apply clear timeout\n", __FUNCTION__, __LINE__);
		}
		dc2h2_cap_3dmaskto2d_ctrl_reg.regValue = IoReg_Read32(DC2H2_CAP_DC2H2_3DMaskTo2D_Ctrl_reg);
		dc2h2_cap_3dmaskto2d_ctrl_reg.dc2h2_3dmaskto2d_h_size = curCapInfo[dc2h_num].input_size.w;
		IoReg_Write32(DC2H2_CAP_DC2H2_3DMaskTo2D_Ctrl_reg, dc2h2_cap_3dmaskto2d_ctrl_reg.regValue);
		IoReg_SetBits(DC2H2_CAP_DC2H2_3DMaskTo2D_doublebuffer_reg, DC2H2_CAP_DC2H2_3DMaskTo2D_doublebuffer_dc2h2_3dmaskto2d_db_apply_mask);
	}
	return TRUE;
}

unsigned char HAL_VT_Set_Crop(KADP_VT_RECT_T cropSize){
	down(&VT_Semaphore);
	set_vt_dc2h_crop_enable(TRUE, DC2H1);
	curCapInfo[DC2H1].crop_size = cropSize;

	if(drv_set_vt_dc2h_crop(DC2H1, cropSize.x, cropSize.y, cropSize.w, cropSize.h) == FALSE){
		up(&VT_Semaphore);
		return FALSE;
	}

	up(&VT_Semaphore);
	return TRUE;
}
/* =======================================VT_BUFFER_DUMP_DEBUG======================================================*/
#ifdef CONFIG_RTK_FEATURE_FOR_GKI
static struct file* file_open(const char* path, int flags, int rights) {
	struct file* filp = NULL;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 11, 0))
	mm_segment_t oldfs;
	oldfs = get_fs();
	set_fs(KERNEL_DS);
#endif
	filp = filp_open(path, flags, rights);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 11, 0))
	set_fs(oldfs);
#endif
	if(IS_ERR(filp)) {
		return NULL;
	}
	return filp;
}

static void file_close(struct file* file) {
	filp_close(file, NULL);
}

/*static int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
	mm_segment_t oldfs;
	int ret;

	oldfs = get_fs();
	set_fs(KERNEL_DS);

#ifdef CONFIG_SUPPORT_SCALER_MODULE
	ret = kernel_read(file, data, size, &offset);
#else
	ret = vfs_read(file, data, size, &offset);
#endif

	set_fs(oldfs);
	return ret;
}*/

static int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
	int ret = 0;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 11, 0))
	mm_segment_t oldfs;
	oldfs = get_fs();
	set_fs(KERNEL_DS);
#endif
#ifdef CONFIG_SUPPORT_SCALER_MODULE
    ret = kernel_write(file, data, size, &offset);
#else
	ret = vfs_write(file, data, size, &offset);
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 11, 0))
	set_fs(oldfs);
#endif
	return ret;
}

static int file_sync(struct file* file) {
	vfs_fsync(file, 0);
	return 0;
}
#else
static struct file* file_open(const char* path, int flags, int rights) {
	return NULL;
}

static void file_close(struct file* file) {
}

static int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
	return 0;
}

static int file_sync(struct file* file) {
	return 0;
}
#endif

unsigned char dump_data_to_file(unsigned char dc2h_num, unsigned int idx, unsigned int dump_w,unsigned int dump_h)
{
	struct file* filp = NULL;
	unsigned long outfileOffset = 0;
	unsigned int size = CapBuffer_VT[dc2h_num].buf_size;
	unsigned char *pVirStartAdrr = NULL;
	static unsigned int g_ulFileCount = 0;
	unsigned int src_phy = CapBuffer_VT[dc2h_num].vt_pa_buf_info[idx].pa_y;
	char raw_dat_path[30];
	sprintf(raw_dat_path, "/data/vt_dump_%d_%d.raw", idx, g_ulFileCount);
	pVirStartAdrr = (unsigned char *)dvr_remap_uncached_memory(src_phy, size, __builtin_return_address(0));

	if(pVirStartAdrr != 0){
		filp = file_open(raw_dat_path, O_RDWR | O_CREAT, 0);
		if (filp == NULL)
		{
			rtd_pr_vt_emerg("file open 1st fail\n");
			dvr_unmap_memory(pVirStartAdrr, size);
			return FALSE;
		}

		file_write(filp, outfileOffset, pVirStartAdrr, CapBuffer_VT[dc2h_num].vt_pa_buf_info[idx].size_plane_y);
		file_sync(filp);
		file_close(filp);

		if(CapBuffer_VT[dc2h_num].plane_number > 1){
			filp = file_open(raw_dat_path, O_RDWR | O_CREAT | O_APPEND, 0);
			if (filp == NULL)
			{
				rtd_pr_vt_emerg("file open 2nd fail\n");
				dvr_unmap_memory(pVirStartAdrr, size);
				return FALSE;
			}
			file_write(filp, outfileOffset, (unsigned char *)(pVirStartAdrr + CapBuffer_VT[dc2h_num].vt_pa_buf_info[idx].size_plane_y), CapBuffer_VT[dc2h_num].vt_pa_buf_info[idx].size_plane_u);
			file_sync(filp);
			file_close(filp);
		}

		if(CapBuffer_VT[dc2h_num].plane_number > 2){
			filp = file_open(raw_dat_path, O_RDWR | O_CREAT | O_APPEND, 0);
			if (filp == NULL)
			{
				rtd_pr_vt_emerg("file open 3rd fail\n");
				dvr_unmap_memory(pVirStartAdrr, size);
				return FALSE;
			}
			file_write(filp, outfileOffset, (unsigned char *)(pVirStartAdrr + CapBuffer_VT[dc2h_num].vt_pa_buf_info[idx].size_plane_y + CapBuffer_VT[dc2h_num].vt_pa_buf_info[idx].size_plane_u), CapBuffer_VT[dc2h_num].vt_pa_buf_info[idx].size_plane_v);
			file_sync(filp);
			file_close(filp);
		}

		rtd_pr_vt_emerg("[VT]dump_finish\n");
		g_ulFileCount++;
		dvr_unmap_memory(pVirStartAdrr, size);
		return TRUE;
	}
	else {
		rtd_pr_vt_emerg("line %d,[VT]dump_to_file NG for null buffer address\n", __LINE__);
		return FALSE;
	}
}

#endif // #ifndef UT_flag

#ifdef CONFIG_PM
static int vivt_suspend (struct device *p_dev)
{
	//int i,size;

	if(get_vt_function() == TRUE)  /*WOSQRTK-13830 tvpowerd suspend call, stop dc2h capture */
	{
		vt_enable_dc2h(FALSE);
		rtd_pr_vt_emerg("%s=%d\n", __FUNCTION__, __LINE__);
	}
	else
		rtd_pr_vt_emerg("%s=%d\n", __FUNCTION__, __LINE__);

	return 0;
}

static int vivt_resume (struct device *p_dev)
{
	return 0;
}
#endif

#ifndef UT_flag
int vivt_open(struct inode *inode, struct file *filp) {
	return 0;
}

ssize_t  vivt_read(struct file *filep, char *buffer, size_t count, loff_t *offp)
{
	return 0;

}

ssize_t vivt_write(struct file *filep, const char *buffer, size_t count, loff_t *offp)
{
	long ret = count;
	char cmd_buf[100] = {0};

#ifdef ENABLE_VT_TEST_CASE_CONFIG
	static int dequeue_idx[VT_CAP_BUF_NUMS] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	static int dc2h_num = 0; //dc2h & dc2h2
#endif

#ifndef CONFIG_ARM64
	rtd_pr_vt_notice("%s(): count=%d, buf=0x%08lx\n", __func__, count, (long)buffer);
#endif

	if (count >= 100)
		return -EFAULT;

	if (copy_from_user(cmd_buf, buffer, count)) {
		ret = -EFAULT;
	}

	if(count > 0) {
	   cmd_buf[count] = '\0';
	}

	if ((cmd_buf[0] == 'v') && (cmd_buf[1] == 't') && (cmd_buf[2] == '-') && (cmd_buf[3] == '-') && (cmd_buf[4] == 'h')
			&& (cmd_buf[5] == 'e') && (cmd_buf[6] == 'l') && (cmd_buf[7] == 'p'))
	{	 /* echo vt--help > /dev/vivtdev */
		rtd_pr_vt_notice("[VT] test dc2h1: echo dc2h 0 > /dev/vivtdev\n");
		rtd_pr_vt_notice("[VT] or test dc2h2: echo dc2h 1 > /dev/vivtdev\n");
		rtd_pr_vt_notice("[VT] capture szie:echo vtsize wid len > /dev/vivtdev\n");
		rtd_pr_vt_notice("[VT] vttest fmt location buffernum, ex:echo vttest 1 1 1 > /dev/vivtdev\n");
		rtd_pr_vt_notice("[VT] fmt: refer VT_CAP_FMT(0~9), location: DC2H_IN_SEL(1~10), buffernum: (1~10)\n");
		rtd_pr_vt_notice("[VT] dump raw:echo dequeue > /dev/vivtdev\n");
		rtd_pr_vt_notice("[VT] dump raw:echo enqueue > /dev/vivtdev\n");
		rtd_pr_vt_notice("[VT] exit: echo exit > /dev/vivtdev \n");
	}
#ifdef ENABLE_VT_TEST_CASE_CONFIG
	else if((cmd_buf[0] == 'd') && (cmd_buf[1] == 'c') && (cmd_buf[2] == '2') && (cmd_buf[3] == 'h') && (cmd_buf[4] == ' ')){
		dc2h_num = (cmd_buf[5] - 0x30); //echo dc2h 1 > /dev/vivtdev
		if(dc2h_num >= DC2HX_MAXN){
			rtd_pr_vt_notice("[VT] out range, only support 1 and 2!\n");
			dc2h_num = DC2H1;
		}
	}
	else if((cmd_buf[0] == 'v') && (cmd_buf[1] == 't') && (cmd_buf[2] == 's') && (cmd_buf[3] == 'i') && (cmd_buf[4] == 'z')
			&& (cmd_buf[5] == 'e'))
	{
		 /* echo vtsize wid len > /dev/vivtdev, ex: echo vtsize 1920 1080 > /dev/vivtdev */
		if((cmd_buf[10] <= '9') && (cmd_buf[10] >= '0'))
		{
			vfod_capture_out_W = ((cmd_buf[7]-0x30)*1000 + (cmd_buf[8]-0x30)*100 + (cmd_buf[9]-0x30)*10 + (cmd_buf[10]-0x30)*1);
			if((cmd_buf[15] <= '9') && (cmd_buf[15] >= '0'))
				vfod_capture_out_H = ((cmd_buf[12]-0x30)*1000 + (cmd_buf[13]-0x30)*100 + (cmd_buf[14]-0x30)*10 + (cmd_buf[15]-0x30)*1);
			else
				vfod_capture_out_H = ((cmd_buf[12]-0x30)*100 + (cmd_buf[13]-0x30)*10 + (cmd_buf[14]-0x30)*1);
		}
		else
		{
			vfod_capture_out_W = ((cmd_buf[7]-0x30)*100 + (cmd_buf[8]-0x30)*10 + (cmd_buf[9]-0x30)*1);
			if((cmd_buf[14] <= '9') && (cmd_buf[14] >= '0'))
				vfod_capture_out_H = ((cmd_buf[11]-0x30)*1000 + (cmd_buf[12]-0x30)*100 + (cmd_buf[13]-0x30)*10 + (cmd_buf[14]-0x30)*1);
			else
				vfod_capture_out_H = ((cmd_buf[11]-0x30)*100 + (cmd_buf[12]-0x30)*10 + (cmd_buf[13]-0x30)*1);
		}

		set_cap_buffer_size_by_AP(vfod_capture_out_W, vfod_capture_out_H);

		curCapInfo[dc2h_num].output_size.x = 0;
		curCapInfo[dc2h_num].output_size.y = 0;
		curCapInfo[dc2h_num].output_size.w = vfod_capture_out_W;
		curCapInfo[dc2h_num].output_size.h = vfod_capture_out_H;
		set_vt_dc2h_cap_mem_type(dc2h_num, DC2H_MEMORY_MMAP);
	}
	else if(((cmd_buf[0] == 'v') && (cmd_buf[1] == 't') && (cmd_buf[2] == 't') && (cmd_buf[3] == 'e') && (cmd_buf[4] == 's')
			&& (cmd_buf[5] == 't')))   /* echo vttest fmt location buffernum > /dev/vivtdev */
	{
		if(((cmd_buf[7]-0x30) >= VT_CAP_RGB888) && ((cmd_buf[7]-0x30) < VT_CAP_MAX)){
			curCapInfo[dc2h_num].fmt = (cmd_buf[7]-0x30);
			rtd_pr_vt_notice("[VT]set pixelfmt=%d\n", curCapInfo[dc2h_num].fmt);
		}else{
			rtd_pr_vt_notice("[VT]set pixelfmt error\n");
			ret = -EFAULT;
			return ret;
		}

		if(((cmd_buf[9]-0x30) >= _MAIN_UZU) && ((cmd_buf[9]-0x30) < _VD_OUTPUT)){
			vfod_capture_location = (cmd_buf[9]-0x30);
			curCapInfo[dc2h_num].cap_location = vfod_capture_location;
			rtd_pr_vt_notice("[VT]set location=%d\n", curCapInfo[dc2h_num].cap_location);
			//VTDumpLocation = (KADP_VT_DUMP_LOCATION_TYPE_T)vfod_capture_location;
		}else if(cmd_buf[9] == 'A'){
			vfod_capture_location = _OSD4_Output;
			curCapInfo[dc2h_num].cap_location = vfod_capture_location;
			rtd_pr_vt_notice("[VT]set location=%d\n", curCapInfo[dc2h_num].cap_location);
		}else{
			rtd_pr_vt_notice("[VT] set test case location error\n");
			ret = -EFAULT;
			return ret;
		}

		if(((cmd_buf[11]-0x30) > 0) && ((cmd_buf[11]-0x30) <= VT_MAX_SUPPORT_BUF_NUMS)){
			//set_vt_VtBufferNum(cmd_buf[11]-0x30, dc2h_num);
			curCapInfo[dc2h_num].bufnum = (cmd_buf[11]-0x30);
			rtd_pr_vt_notice("[VT]set bufnum=%d\n", curCapInfo[dc2h_num].bufnum);
		}else{
			rtd_pr_vt_notice("[VT] set test case bufnum error\n");
			ret = -EFAULT;
			return ret;
		}

		rtd_pr_vt_notice("[VT]%s\n", cmd_buf);
		set_vt_dequeue_enqueue_count(0, dc2h_num);
		//start test case
		handle_vt_test_case(dc2h_num);
	}
	else if(((cmd_buf[0] == 'v') && (cmd_buf[1] == 't') && (cmd_buf[2] == 'c') && (cmd_buf[3] == 'r') && (cmd_buf[4] == 'o')
		&& (cmd_buf[5] == 'p'))){ /* echo vtcrop xxxx xxxx xxxx xxxx > /dev/vivtdev  (echo x,y,w,h)*/
		unsigned int x = 0, y = 0;
		//unsigned int timeoutcount = 200;
		x = ((cmd_buf[7]-0x30)*1000 + (cmd_buf[8]-0x30)*100 + (cmd_buf[9]-0x30)*10 + (cmd_buf[10]-0x30));
		y = ((cmd_buf[12]-0x30)*1000 + (cmd_buf[13]-0x30)*100 + (cmd_buf[14]-0x30)*10 + (cmd_buf[15]-0x30));
		curCapInfo[dc2h_num].crop_size.w = ((cmd_buf[17]-0x30)*1000 + (cmd_buf[18]-0x30)*100 + (cmd_buf[19]-0x30)*10 + (cmd_buf[20]-0x30));
		curCapInfo[dc2h_num].crop_size.h = ((cmd_buf[22]-0x30)*1000 + (cmd_buf[23]-0x30)*100 + (cmd_buf[24]-0x30)*10 + (cmd_buf[25]-0x30));
		rtd_pr_vt_notice("[VT]crop test (%d,%d,%d,%d)\n", x, y, curCapInfo[dc2h_num].crop_size.w, curCapInfo[dc2h_num].crop_size.h);

		drv_set_vt_dc2h_crop(dc2h_num, x, y, curCapInfo[dc2h_num].crop_size.w, curCapInfo[dc2h_num].crop_size.h);
	}
	else if(strcmp(cmd_buf, "debug\n") == 0)
	{
		IoReg_Write32(0xb8029dbc, 0x00000040);
		IoReg_Write32(0xb8029dc8, 0x00000007);
		IoReg_Write32(0xb8029dbc, 0x0f000041);
		IoReg_Write32(0xb8029dc8, 0x00000007);
	}
	else if(strcmp(cmd_buf, "exit\n") == 0) /* echo exit > /dev/vivtdev */
	{
		drvif_vt_set_stream_off(dc2h_num);

		drvif_vt_release_capture_cma_buf(dc2h_num);
		drvif_vt_finalize(dc2h_num);
	}
	else if(((cmd_buf[0] == 'v') && (cmd_buf[1] == 't') && (cmd_buf[2] == 'm') && (cmd_buf[3] == 'a') && (cmd_buf[4] == 's')
            && (cmd_buf[5] == 'k') && (cmd_buf[6] == '_'))) /* echo vtmask_xxxxxxxx > /dev/vivtdev*/
    {
        int i;
        unsigned int vtmask_print = 0;
        VT_BUF_STATUS_T *vt_buf_status = NULL;
        for (i = 7; i < 15; i++)
            char_to_vtPrintMask(&vtmask_print, cmd_buf[i], 28-((i-7) << 2));

        vtPrintMask = vtmask_print;
        rtd_pr_vt_notice("[VT]vtPrintMask=0x%x\n", vtPrintMask);

        /*set share memory print mask*/
        vt_buf_status = (VT_BUF_STATUS_T *)Scaler_GetShareMemVirAddr(SCLAERIOC_SET_VT_DC2H_BUF_STATUS);
        if(vt_buf_status == NULL)
        {
            rtd_pr_vt_notice("[VT][error] shm vt_buf_status pointer null\n");
            return 0;
        }
        vt_buf_status->printMask = Scaler_ChangeUINT32Endian(vtPrintMask);
    }
    else if((cmd_buf[0] == 'v') && (cmd_buf[1] == 't') && (cmd_buf[2] == 'f') && (cmd_buf[3] == 'p') && (cmd_buf[4] == 's')
            && (cmd_buf[5] == ' ')) /*echo vtfps xx > /dev/vivtdev*/
    {
        unsigned int user_fps = 0;
        user_fps = (cmd_buf[6] - 0x30)*10 + (cmd_buf[7] - 0x30);
        set_vt_target_fps(user_fps, dc2h_num);
        rtd_pr_vt_notice("[VT]user_fps %d\n", user_fps);
    }
    else if(strcmp(cmd_buf, "vtcase\n") == 0) /* echo vtcase > /dev/vivtdev */
    {
        int i = 0;
        set_vt_app_case(VT_APP_CASE_LIVE_STREAM);
        HAL_VT_Set_App_Case(get_vt_app_case());
        rtd_pr_vt_notice("[VT]vt_app_case %d\n", vt_app_case);

        for(i = 0; i < get_vt_VtBufferNum(dc2h_num); i++)
        {
            dequeue_idx[i] = -1;
        }
    }
    else if(strcmp(cmd_buf, "dequeue\n") == 0) /* echo dequeue > /dev/vivtdev */
    {
        unsigned int getidx = 0;
        int i = 0;
        drvif_set_dc2h_wait_vsync(curCapInfo[dc2h_num].cap_location, dc2h_num);
		drvif_vt_get_dequeue_index(&getidx, dc2h_num);
        rtd_pr_vt_notice("[VT]dequeue %d\n", getidx);

        for (i = 0; i < get_vt_VtBufferNum(dc2h_num); i++)
        {
            if(dequeue_idx[i] == -1 && getidx < get_vt_VtBufferNum(dc2h_num))
            {
                dequeue_idx[i] = getidx;
				dump_data_to_file(dc2h_num, getidx, vt_cap_frame_max_width, vt_cap_frame_max_height);
                break;
            }
        }
    }
    else if(strcmp(cmd_buf, "enqueue\n") == 0) /* echo enqueue > /dev/vivtdev */
    {
        int i = 0;
        for (i = 0; i < get_vt_VtBufferNum(dc2h_num); i++)
        {
            if(dequeue_idx[i] != -1)
            {
				drvif_vt_set_enqueue_index(dequeue_idx[i], dc2h_num);
                rtd_pr_vt_notice("[VT]enqueue %d\n", dequeue_idx[i]);
                dequeue_idx[i] = -1;
                break;
            }
        }
    }
	else if((cmd_buf[0] == 'p') && (cmd_buf[1] == 'q') && (cmd_buf[2] == 'c')){ //echo pqc 12 > /dev/vivtdev
		dc2h2_pqc_cmp_ratio = ((cmd_buf[4]-0x30)*10 + (cmd_buf[5]-0x30)*1);
		if(dc2h2_pqc_cmp_ratio < 8 || dc2h2_pqc_cmp_ratio > 16){
			rtd_pr_vt_notice("[VT] dc2h2 pqc output bits out range, please retry, type value 08~16\n");
		}
		rtd_pr_vt_notice("[VT] dc2h2 pqc output bits %d\n", dc2h2_pqc_cmp_ratio);
	}
	else if(strcmp(cmd_buf, "vtptg\n") == 0)
    {
        IoReg_ClearBits(0xB802B0A0, _BIT1);
        IoReg_ClearBits(0xB802B000, _BIT0);
        IoReg_ClearBits(0xB8028478, _BIT10);
        IoReg_ClearBits(0xB8028128, _BIT1 | _BIT0);
        IoReg_ClearBits(0xB8028478, _BIT2);
        IoReg_ClearBits(0xB8028300, _BIT1);
        IoReg_Write32(0xB8029000, 0x00000031); // D-domain Pattern Gen Setting
        IoReg_Write32(0xB8029074, 0x000001DF); // D-domain Pattern Gen Color-Bar Width
        IoReg_Write32(0xB8029028, 0x0F000870); // UZU Input Size = 3840x2160
    }
	else if((cmd_buf[0] == 'm') && (cmd_buf[1] == 'o') && (cmd_buf[2] == 'd') && (cmd_buf[3] == 'e')){
		if((cmd_buf[5]-0x30) == 0 || (cmd_buf[5]-0x30) == 1){ /* echo mode 0 > /dev/vivtdev */
			set_dc2h_dma_cap_mode(cmd_buf[5]-0x30, dc2h_num);
		}else{
			rtd_pr_vt_notice("[VT]retry, only support line mode(0), frame mode(1)\n");
		}
	}
	else if(strcmp(cmd_buf, "dc2h2_pqc\n") == 0) //echo dc2h2_pqc > /dev/vivtdev
	{
		dc2h2_pqc_path_enable = TRUE;
		dc2h_num = DC2H2;
		vfod_capture_out_W = 1920;
		vfod_capture_out_H = 2160;
		set_cap_buffer_size_by_AP(vfod_capture_out_W, vfod_capture_out_H);
		curCapInfo[dc2h_num].output_size.x = 0;
		curCapInfo[dc2h_num].output_size.y = 0;
		curCapInfo[dc2h_num].output_size.w = vfod_capture_out_W;
		curCapInfo[dc2h_num].output_size.h = vfod_capture_out_H;
		curCapInfo[dc2h_num].fmt = VT_CAP_RGB888;
		curCapInfo[dc2h_num].cap_location = _OSD4_Output;
		curCapInfo[dc2h_num].bufnum = 4; //pqc max support 4 buffers
		if (Capture_BufferMemInit_VT(curCapInfo[dc2h_num].bufnum, dc2h_num) == FALSE)
		{
			rtd_pr_vt_emerg("VT init allocate memory fail;%s=%d \n", __FUNCTION__, __LINE__);
			return 0;
		}
		curCapInfo[dc2h_num].enable = 1;
		drvif_dc2h2_pqc_capture_config(&curCapInfo[dc2h_num]);
	}
	else if(strcmp(cmd_buf, "dc2h2_cap0\n") == 0)
	{
		//dc2h2 cap0 use hw mode, frc style: input slow, echo dc2h2_cap0 > /dev/vivtdev
		dc2h2_pqc_path_enable = FALSE;
		dc2h_num = DC2H2;
		vfod_capture_out_W = 1920;
		vfod_capture_out_H = 1080;
		set_cap_buffer_size_by_AP(vfod_capture_out_W, vfod_capture_out_H);
		curCapInfo[dc2h_num].output_size.x = 0;
		curCapInfo[dc2h_num].output_size.y = 0;
		curCapInfo[dc2h_num].output_size.w = vfod_capture_out_W;
		curCapInfo[dc2h_num].output_size.h = vfod_capture_out_H;
		curCapInfo[dc2h_num].fmt = VT_CAP_RGB888;
		curCapInfo[dc2h_num].cap_location = _Memc_Mux_Input;
		curCapInfo[dc2h_num].bufnum = 4; //pqc max support 4 buffers
		if (Capture_BufferMemInit_VT(curCapInfo[dc2h_num].bufnum, dc2h_num) == FALSE)
		{
			rtd_pr_vt_emerg("VT init allocate memory fail;%s=%d \n", __FUNCTION__, __LINE__);
			return 0;
		}
		curCapInfo[dc2h_num].enable = 1;
		set_dc2h2_cap0_seq_capture_hw_mode(&curCapInfo[dc2h_num]);
	}
	else if(strcmp(cmd_buf, "dump_cap0\n") == 0) /* echo dump_cap0 > /dev/vivtdev, hw mode 3-buffer*/
    {
		dc2h2_cap_dc2h2_cap0_cap_ctl0_RBUS dc2h2_cap_dc2h2_cap0_cap_ctl0_reg;
		unsigned int dequeue_addr = 0;
		int dequeue_index = -1;
        if(!DC2H2_CAP_DC2H2_CAP_Color_format_get_dc2h2_cap0_en(IoReg_Read32(DC2H2_CAP_DC2H2_CAP_Color_format_reg))){
			rtd_pr_vt_notice("dump_cap0 fail, due to cap0 disable\n");
			return 0;
		}
		if(CapBuffer_VT[DC2H2].vt_pa_buf_info[0].pa_y == 0 || CapBuffer_VT[DC2H2].vt_pa_buf_info[1].pa_y == 0 || CapBuffer_VT[DC2H2].vt_pa_buf_info[2].pa_y == 0 ||
			CapBuffer_VT[DC2H2].vt_pa_buf_info[3].pa_y == 0){
			rtd_pr_vt_notice("dump_cap0 fail, due to pa buffer address zero\n");
			return 0;
		}

		dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.regValue = IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_Cap_CTL0_reg);
		if(dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_buf_index_dbg == 0){ //hw is writing 1th frame buffer, blk0
			dequeue_addr = IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL0_reg);
			IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL0_reg, IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL0_reg));
			IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL0_reg, dequeue_addr);
			IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL2_reg, IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL0_reg)); //low boundary
			IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL1_reg, IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL0_reg)+CapBuffer_VT[DC2H2].vt_pa_buf_info[0].size_plane_y - DEFAULT_DC2H_CAP_BURST_LEN);
		}else if(dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_buf_index_dbg == 1){ //hw is writing 2nd frame buffer, blk1
			dequeue_addr = IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL0_reg);
			IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL0_reg, IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL0_reg));
			IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL0_reg, dequeue_addr);
			IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL2_reg, IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL0_reg)); //low boundary
			IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL1_reg, IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL0_reg)+CapBuffer_VT[DC2H2].vt_pa_buf_info[0].size_plane_y - DEFAULT_DC2H_CAP_BURST_LEN);
		}else if(dc2h2_cap_dc2h2_cap0_cap_ctl0_reg.dc2h2_cap0_buf_index_dbg == 2){ //hw is writing 3rd frame buffer, blk2
			dequeue_addr = IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL0_reg);
			IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL0_reg, IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL0_reg));
			IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL0_reg, dequeue_addr);
			IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL2_reg, IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL0_reg)); //low boundary
			IoReg_Write32(DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL1_reg, IoReg_Read32(DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL0_reg)+CapBuffer_VT[DC2H2].vt_pa_buf_info[0].size_plane_y - DEFAULT_DC2H_CAP_BURST_LEN);
		}

		if(dequeue_addr == CapBuffer_VT[DC2H2].vt_pa_buf_info[0].pa_y){
			dequeue_index = 0;
		}else if(dequeue_addr == CapBuffer_VT[DC2H2].vt_pa_buf_info[1].pa_y){
			dequeue_index = 1;
		}else if(dequeue_addr == CapBuffer_VT[DC2H2].vt_pa_buf_info[2].pa_y){
			dequeue_index = 2;
		}else if(dequeue_addr == CapBuffer_VT[DC2H2].vt_pa_buf_info[3].pa_y){
			dequeue_index = 3;
		}

		rtd_pr_vt_notice("dump_cap0 index %d\n", dequeue_index);
		dump_data_to_file(dc2h_num, dequeue_index, vt_cap_frame_max_width, vt_cap_frame_max_height);
    }
	else
	{
		rtd_pr_vt_notice("[VT]%s\n", cmd_buf);
	}

#endif

	return ret;
}

#ifdef ENABLE_VT_TEST_CASE_CONFIG

void handle_vt_test_case(unsigned int dc2h_num)
{
	//do_vt_reqbufs(curCapInfo[dc2h_num].bufnum);
	if (Capture_BufferMemInit_VT(curCapInfo[dc2h_num].bufnum, dc2h_num) == FALSE)
	{
		rtd_pr_vt_emerg("VT init allocate memory fail;%s=%d \n", __FUNCTION__, __LINE__);
		return;
	}
	curCapInfo[dc2h_num].enable = 1;
	if(dc2h_num == DC2H1){
		drvif_dc2h_capture_config(&curCapInfo[DC2H1]);
	}else{
		drvif_dc2h2_capture_config(&curCapInfo[DC2H2]);
	}
}
#endif


int vivt_release(struct inode *inode, struct file *filep)
{
	return 0;
}

long vivt_ioctl(struct file *file, unsigned int cmd,  unsigned long arg)
{
	int retval = 0;
	if (_IOC_TYPE(cmd) != VT_IOC_MAGIC || _IOC_NR(cmd) > VT_IOC_MAXNR) return -ENOTTY ;

	switch (cmd)
	{
		case VT_IOC_INIT:
		{
			if(HAL_VT_Init()==FALSE)
				retval = -1;
			break;
		}
		case VT_IOC_INITEX:
		{
			unsigned int buffernum;
			if(copy_from_user((void *)&buffernum, (const void __user *)arg, sizeof(unsigned int)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_INITEX failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_InitEx(buffernum)==FALSE)
					retval =  -1;
			}
			break;
		}
		case VT_IOC_FINALIZE:
		{
			//addr = *((unsigned int *)arg);
			if(HAL_VT_Finalize()==FALSE)
				retval = -1;
			break;
		}
		case VT_IOC_GET_DEVICE_CAPABILITY:
		{
			KADP_VT_DEVICE_CAPABILITY_INFO_T DeviceCapabilityInfo;
			if(HAL_VT_GetDeviceCapability(&DeviceCapabilityInfo)==FALSE) {
				retval = -1;
			} else {
				if(copy_to_user((void __user *)arg, (void *)&DeviceCapabilityInfo, sizeof(KADP_VT_DEVICE_CAPABILITY_INFO_T)))
				{
					retval = -EFAULT;
					rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_DEVICE_CAPABILITY failed!!!!!!!!!!!!!!!\n");
				}
			}

			break;
		}
		case VT_IOC_GET_VFB_CAPABILITY:
		{
			KADP_VT_VFB_CAPINFO_T vfb_capinfo;
			if(copy_from_user((void *)&vfb_capinfo, (const void __user *)arg, sizeof(KADP_VT_VFB_CAPINFO_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_GET_VFB_CAPABILITY failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetVideoFrameBufferCapability(vfb_capinfo.wid,&vfb_capinfo.vfbCapInfo)==FALSE) {
					retval = -1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&vfb_capinfo, sizeof(KADP_VT_VFB_CAPINFO_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_VFB_CAPABILITY failed!!!!!!!!!!!!!!!\n");
					}
				}
			}
			break;
		}
		case VT_IOC_GET_VFOD_CAPABILITY:
		{
			KADP_VT_VFOD_CAPINFO_T vfod_capinfo;
			if(copy_from_user((void *)&vfod_capinfo, (const void __user *)arg, sizeof(KADP_VT_VFOD_CAPINFO_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_GET_VFOD_CAPABILITY failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetVideoFrameOutputDeviceCapability(vfod_capinfo.wid,&vfod_capinfo.vfodCapInfo)==FALSE) {
					retval = -1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&vfod_capinfo, sizeof(KADP_VT_VFOD_CAPINFO_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_VFOD_CAPABILITY failed!!!!!!!!!!!!!!!\n");
					}
				}
			}
			break;
		}
		case VT_IOC_GET_VFOD_LIMITATION:
		{
			KADP_VT_VFOD_LIMITATIONINFO_T vfod_limitationinfo;
			if(copy_from_user((void *)&vfod_limitationinfo, (const void __user *)arg, sizeof(KADP_VT_VFOD_LIMITATIONINFO_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_GET_VFOD_LIMITATION failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetVideoFrameOutputDeviceLimitation(vfod_limitationinfo.wid,&vfod_limitationinfo.vfodlimitationInfo)==FALSE) {
					retval = -1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&vfod_limitationinfo, sizeof(KADP_VT_VFOD_LIMITATIONINFO_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_VFOD_LIMITATION failed!!!!!!!!!!!!!!!\n");
					}
				}
			}
			break;
		}
		case VT_IOC_GET_ALLVFB_PROPERTY:
		{
			KADP_VT_VFB_PROPERTY_T vfb_property;
			if(copy_from_user((void *)&vfb_property, (const void __user *)arg, sizeof(KADP_VT_VFB_PROPERTY_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_GET_ALLVFB_PROPERTY failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetAllVideoFrameBufferProperty(vfb_property.wid,&vfb_property.vfbProInfo)==FALSE) {
					retval = -1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&vfb_property, sizeof(KADP_VT_VFB_PROPERTY_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_ALLVFB_PROPERTY failed!!!!!!!!!!!!!!!\n");
					}
				}
			}
			break;
		}
		case VT_IOC_GET_VFB_INDEX:
		{
			KADP_VT_GET_VFBINDEX_T vfb_index;
			if(copy_from_user((void *)&vfb_index, (const void __user *)arg, sizeof(KADP_VT_GET_VFBINDEX_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_GET_VFB_INDEX failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetVideoFrameBufferIndex(vfb_index.wid,&vfb_index.IndexCurVFB)==FALSE) {
					retval = -1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&vfb_index, sizeof(KADP_VT_GET_VFBINDEX_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_VFB_INDEX failed!!!!!!!!!!!!!!!\n");
					}
				}
			}
			break;
		}
		case VT_IOC_GET_VFOD_STATE:
		{
			KADP_VT_VFOD_GET_STATEINFO_T vfodgetstateinfo;
			if(copy_from_user((void *)&vfodgetstateinfo, (const void __user *)arg, sizeof(KADP_VT_VFOD_GET_STATEINFO_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_GET_VFOD_STATE failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetVideoFrameOutputDeviceState(vfodgetstateinfo.wid,&vfodgetstateinfo.VFODstate)==FALSE) {
					retval =-1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&vfodgetstateinfo, sizeof(KADP_VT_VFOD_GET_STATEINFO_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_VFOD_STATE failed!!!!!!!!!!!!!!!\n");
					}
				}
			}
			break;
		}
		case VT_IOC_SET_VFOD_STATE:
		{
			KADP_VT_VFOD_SET_STATEINFO_T vfodsetstateinfo;
			if(copy_from_user((void *)&vfodsetstateinfo, (const void __user *)arg, sizeof(KADP_VT_VFOD_SET_STATEINFO_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_SET_VFOD_STATE failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_SetVideoFrameOutputDeviceState(vfodsetstateinfo.wid,vfodsetstateinfo.vfodStateFlag,&vfodsetstateinfo.VFODstate)==FALSE)
					retval =-1;
			}

			break;
		}
		case VT_IOC_GET_VFOD_FRAMERATE:
		{
			KADP_VT_VFOD_GET_FRAMERATE_T vfodgetfr;
			if(copy_from_user((void *)&vfodgetfr, (const void __user *)arg, sizeof(KADP_VT_VFOD_GET_FRAMERATE_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_GET_VFOD_FRAMERATE failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetVideoFrameOutputDeviceFramerate(vfodgetfr.wid,&vfodgetfr.framerate)==FALSE) {
					retval = -1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&vfodgetfr, sizeof(KADP_VT_VFOD_GET_FRAMERATE_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_VFOD_FRAMERATE failed!!!!!!!!!!!!!!!\n");
					}
				}
			}

			break;
		}
		case VT_IOC_GET_VFOD_DUMPLOCATION:
		{
			KADP_VT_VFOD_GET_DUMPLOCATION_T vfodgetdumplocation;
			if(copy_from_user((void *)&vfodgetdumplocation, (const void __user *)arg, sizeof(KADP_VT_VFOD_GET_DUMPLOCATION_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_GET_VFOD_DUMPLOCATION failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetVideoFrameOutputDeviceDumpLocation(vfodgetdumplocation.wid,&vfodgetdumplocation.DumpLocation)==FALSE) {
					retval =-1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&vfodgetdumplocation, sizeof(KADP_VT_VFOD_GET_DUMPLOCATION_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_VFOD_DUMPLOCATION failed!!!!!!!!!!!!!!!\n");
					}
				}
			}

			break;
		}
		case VT_IOC_SET_VFOD_DUMPLOCATION:
		{
			KADP_VT_VFOD_SET_DUMPLOCATION_T vfodsetdumplocation;
			if(copy_from_user((void *)&vfodsetdumplocation, (const void __user *)arg, sizeof(KADP_VT_VFOD_SET_DUMPLOCATION_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_SET_VFOD_DUMPLOCATION failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_SetVideoFrameOutputDeviceDumpLocation(vfodsetdumplocation.wid,vfodsetdumplocation.DumpLocation)==FALSE)
					retval =-1;
			}

			break;
		}
		case VT_IOC_GET_VFOD_OUTPUTINFO:
		{
			KADP_VT_VFOD_GET_OUTPUTINFO_T vfodgetoututinfo;
			if(copy_from_user((void *)&vfodgetoututinfo, (const void __user *)arg, sizeof(KADP_VT_VFOD_GET_OUTPUTINFO_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_GET_VFOD_OUTPUTINFO failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetVideoFrameOutputDeviceOutputInfo(vfodgetoututinfo.wid,vfodgetoututinfo.DumpLocation,&vfodgetoututinfo.OutputInfo)==FALSE) {
					retval =-1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&vfodgetoututinfo, sizeof(KADP_VT_VFOD_GET_OUTPUTINFO_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_VFOD_OUTPUTINFO failed!!!!!!!!!!!!!!!\n");
					}
				}
			}

			break;
		}
		case VT_IOC_SET_VFOD_OUTPUTREGION:
		{
			KADP_VT_VFOD_SET_OUTPUTREGION_T vfodsetoututinfo;
			if(copy_from_user((void *)&vfodsetoututinfo, (const void __user *)arg, sizeof(KADP_VT_VFOD_SET_OUTPUTREGION_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_SET_VFOD_OUTPUTREGION failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_SetVideoFrameOutputDeviceOutputRegion(vfodsetoututinfo.wid,vfodsetoututinfo.DumpLocation,&vfodsetoututinfo.OutputRegion)==FALSE)
					retval =-1;
			}

			break;
		}
		case VT_IOC_WAIT_VSYNC:
		{
			KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID;
			if(copy_from_user((void *)&videoWindowID, (const void __user *)arg, sizeof(KADP_VT_VIDEO_WINDOW_TYPE_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("scaler vt ioctl code=VT_IOC_WAIT_VSYNC failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_WaitVsync(videoWindowID)==FALSE)
					retval =-1;
			}
			break;
		}
		case VT_IOC_GET_INPUTVIDEOINFO:
		{
			KADP_VT_GET_INPUTVIDEOINFO_T inputvideoinfo;
			if(copy_from_user((void *)&inputvideoinfo, (const void __user *)arg, sizeof(KADP_VT_GET_INPUTVIDEOINFO_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_GET_INPUTVIDEOINFO failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetInputVideoInfo(inputvideoinfo.wid,&inputvideoinfo.InputVideoInfo)==FALSE) {
					retval =-1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&inputvideoinfo, sizeof(KADP_VT_GET_INPUTVIDEOINFO_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_INPUTVIDEOINFO failed!!!!!!!!!!!!!!!\n");
					}
				}
			}

			break;
		}
		case VT_IOC_GET_OUTPUTVIDEOINFO:
		{
			KADP_VT_GET_OUTPUTVIDEOINFO_T onputvideoinfo;
			if(copy_from_user((void *)&onputvideoinfo, (const void __user *)arg, sizeof(KADP_VT_GET_OUTPUTVIDEOINFO_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("scaler vt ioctl code=VT_IOC_GET_OUTPUTVIDEOINFO failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetOutputVideoInfo(onputvideoinfo.wid,&onputvideoinfo.OutputVideoInfo)==FALSE) {
					retval =-1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&onputvideoinfo, sizeof(KADP_VT_GET_OUTPUTVIDEOINFO_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_OUTPUTVIDEOINFO failed!!!!!!!!!!!!!!!\n");
					}
				}
			}

			break;
		}
		case VT_IOC_GET_VIDEOMUTESTATUS:
		{
			KADP_VT_GET_VIDEOMUTESTATUS_T videomutestatus;
			if(copy_from_user((void *)&videomutestatus, (const void __user *)arg, sizeof(KADP_VT_GET_VIDEOMUTESTATUS_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_GET_VIDEOMUTESTATUS failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetVideoMuteStatus(videomutestatus.wid,&videomutestatus.bOnOff)==FALSE) {
					retval =-1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&videomutestatus, sizeof(KADP_VT_GET_VIDEOMUTESTATUS_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_VIDEOMUTESTATUS failed!!!!!!!!!!!!!!!\n");
					}
				}
			}

			break;
		}
		case VT_IOC_GET_VFB_SVSTATE:
		{
			KADP_VT_GET_VFBSVSTATE_T vfbsvstatus;
			if(copy_from_user((void *)&vfbsvstatus, (const void __user *)arg, sizeof(KADP_VT_GET_VFBSVSTATE_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_from_user:scaler vt ioctl code=VT_IOC_GET_VFB_SVSTATE failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetVideoFrameBufferSecureVideoState(vfbsvstatus.wid,&vfbsvstatus.IsSecureVideo)==FALSE) {
					retval =-1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&vfbsvstatus, sizeof(KADP_VT_GET_VFBSVSTATE_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_VFB_SVSTATE failed!!!!!!!!!!!!!!!\n");
					}
				}
			}
			break;
		}
		case VT_IOC_GET_VFOD_BLOCKSTATE:
		{
			KADP_VT_GET_VFODBLOCKSTATE_T vfodblockstatus;
			if(copy_from_user((void *)&vfodblockstatus, (const void __user *)arg, sizeof(KADP_VT_GET_VFODBLOCKSTATE_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("scaler vt ioctl code=VT_IOC_GET_VFOD_BLOCKSTATE failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_GetVideoFrameOutputDeviceBlockState(vfodblockstatus.wid,&vfodblockstatus.bBlockState)==FALSE) {
					retval =-1;
				} else {
					if(copy_to_user((void __user *)arg, (void *)&vfodblockstatus, sizeof(KADP_VT_GET_VFODBLOCKSTATE_T)))
					{
						retval = -EFAULT;
						rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_GET_VFOD_BLOCKSTATE failed!!!!!!!!!!!!!!!\n");
					}

				}
			}
			break;
		}
		case VT_IOC_SET_VFOD_BLOCKSTATE:
		{
			KADP_VT_SET_VFODBLOCKSTATE_T setvfodblockstatus;
			if(copy_from_user((void *)&setvfodblockstatus, (const void __user *)arg, sizeof(KADP_VT_SET_VFODBLOCKSTATE_T)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("scaler vt ioctl code=VT_IOC_SET_VFOD_BLOCKSTATE failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_SetVideoFrameOutputDeviceBlockState(setvfodblockstatus.wid,setvfodblockstatus.bBlockState)==FALSE)
					retval =-1;
			}
			break;
		}
		case VT_IOC_SET_PIXEL_FORMAT:
		{
			VT_CAP_FMT format;
			if(copy_from_user((void *)&format, (const void __user *)arg, sizeof(VT_CAP_FMT)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("scaler vt ioctl code=VT_IOC_SET_PIXEL_FORMAT failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_set_Pixel_Format(format)==FALSE)
					retval =-1;
			}
			break;
		}
		case VT_IOC_VFB_DATASHOW_ONSUBDISP:
		{
			retval = -EFAULT;
			rtd_pr_vt_notice("scaler vt ioctl code=VT_IOC_VFB_DATASHOW_ONSUBDISP failed!!!!!!!!!!!!!!!\n");
			break;
		}
		case VT_IOC_EnableFRCMode:
		{
			unsigned int bEnableFrcMode;
			if(copy_from_user((void *)&bEnableFrcMode, (const void __user *)arg, sizeof(unsigned int)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_INITEX failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				unsigned char bEnableFrc;
				bEnableFrc = (unsigned char)bEnableFrcMode;
				if(HAL_VT_EnableFRCMode(bEnableFrc)==FALSE)
					retval =  -1;
			}
			break;
		}
        case VT_IOC_SET_OUTPUT_FPS:
		{
			unsigned int tmpFps;
			if(copy_from_user((void *)&tmpFps, (const void __user *)arg, sizeof(unsigned int)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("copy_to_user:scaler vt ioctl code=VT_IOC_SET_OUTPUT_FPS failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(HAL_VT_Set_OutFps(tmpFps) == FALSE)
					retval = -1;
			}
			break;
		}
		case VT_IOC_SET_APP_CASE:
        {
            unsigned int tmp;
            if(copy_from_user((void *)&tmp, (const void __user *)arg, sizeof(unsigned int)))
            {
                retval = -EFAULT;
                rtd_pr_vt_notice("copy_from_user:scaler vt ioctl code=VT_IOC_SET_APP_CASE failed!\n");
            }
            else
            {
                set_vt_app_case((VT_APP_CASE)tmp);
                HAL_VT_Set_App_Case(get_vt_app_case());
                rtd_pr_vt_notice("[VT]vt_app_case %d\n", vt_app_case);
            }
            break;
        }
        case VT_IOC_AP_ENQUEUE_INDEX:
        {
            unsigned int idx;
            if(copy_from_user((void *)&idx, (const void __user *)arg, sizeof(unsigned int)))
            {
                retval = -EFAULT;
                rtd_pr_vt_notice("copy_from_user:scaler vt ioctl code=VT_IOC_AP_ENQUEUE_INDEX failed!\n");
            }
            else
            {
                if(HAL_VT_Set_Enqueue_index(idx) == FALSE)
                    retval = -1;
            }
            break;
        }
        case VT_IOC_AP_DEQUEUE_INDEX:
        {
            KADP_VT_GET_VFBINDEX_T vfb_index;
            if(copy_from_user((void *)&vfb_index, (const void __user *)arg, sizeof(KADP_VT_GET_VFBINDEX_T)))
            {
                retval = -EFAULT;
                rtd_pr_vt_notice("copy_from_user:scaler vt ioctl code=VT_IOC_AP_DEQUEUE_INDEX failed!\n");
            }
            else
            {
                if(HAL_VT_Get_Dequeue_index(vfb_index.wid,&vfb_index.IndexCurVFB) == FALSE) {
                    retval = -1;
                } else {
                    if(copy_to_user((void __user *)arg, (void *)&vfb_index, sizeof(KADP_VT_GET_VFBINDEX_T)))
                    {
                        retval = -EFAULT;
                        rtd_pr_vt_notice("copy_to_user:scaler vt ioctl code=VT_IOC_AP_DEQUEUE_INDEX failed!\n");
                    }
                }
            }
            break;
        }
		case VT_IOC_AP_SET_CROP:
		{
			KADP_VT_RECT_T crop_region;
			if(copy_from_user((void *)&crop_region, (const void __user *)arg, sizeof(KADP_VT_RECT_T)))
            {
                retval = -EFAULT;
                rtd_pr_vt_notice("copy_from_user:scaler vt ioctl code=VT_IOC_AP_SET_CROP failed!\n");
            }else{
				if(HAL_VT_Set_Crop(crop_region) == FALSE){
					rtd_pr_vt_notice("HAL_VT_Set_Crop failed\n");
					retval = -1;
				}
			}
			break;
		}
		default:
			rtd_pr_vt_notice("Scaler vt disp: ioctl code = %d is invalid!\n", cmd);
			break ;
		}
	return retval;
}

#if defined(CONFIG_COMPAT) && defined(CONFIG_ARM64)
long vivt_compat_ioctl(struct file *file, unsigned int cmd,  unsigned long arg)
{
	return vivt_ioctl(file,cmd,arg);
}
#endif

int vivt_major   = VT_MAJOR;
int vivt_minor   = 0 ;
int vivt_nr_devs = VT_NR_DEVS;

module_param(vivt_major, int, S_IRUGO);
module_param(vivt_minor, int, S_IRUGO);
module_param(vivt_nr_devs, int, S_IRUGO);

static struct class *vivt_class = NULL;
static struct platform_device *vivt_platform_devs = NULL;

struct file_operations vivt_fops= {
	.owner =    THIS_MODULE,
	.open  =    vivt_open,
	.release =  vivt_release,
	.read  =    vivt_read,
	.write = 	vivt_write,
	.unlocked_ioctl = 	vivt_ioctl,
#if defined(CONFIG_COMPAT) && defined(CONFIG_ARM64)
  	.compat_ioctl = vivt_compat_ioctl,
#endif
};

#ifdef CONFIG_PM
static const struct dev_pm_ops vivt_pm_ops =
{
	.suspend    = vivt_suspend,
	.resume     = vivt_resume,
#ifdef CONFIG_HIBERNATION
	.freeze 	= vivt_suspend,
	.thaw		= vivt_resume,
	.poweroff	= vivt_suspend,
	.restore	= vivt_resume,
#endif

};
#endif

static struct platform_driver vivt_device_driver = {
    .driver = {
        .name       = VT_DEVICE_NAME,
        .bus        = &platform_bus_type,
#ifdef CONFIG_PM
	.pm 		= &vivt_pm_ops,
#endif

    },
} ;


static char *vivt_devnode(struct device *dev, umode_t *mode)
{
	*mode =0666;
	return NULL;
}

#if IS_ENABLED(CONFIG_RTK_KDRV_DC_MEMORY_TRASH_DETCTER)
static void sb4_dc2h_debug_info(void)
{
	dcmt_trap_info trap_info;
	unsigned int i = 0;
	//check trash module
	int dc2h_dcmt_trap = isDcmtTrap("TVSB4_DC2H");
	int dc2h2_dcmt_trap = isDcmtTrap("TVSB4_DC2H2");
	if(!dc2h_dcmt_trap || !dc2h2_dcmt_trap){
		rtd_pr_vt_err("not TVSB4_DC2H module!\n");
		return;
	}

	get_dcmt_trap_info(&trap_info);  //get dcmt trap information (include trash_addr,module_id,rw_type)
	rtd_pr_vt_err("TVSB4_DC2H module trashed somewhere!\n");

	if(dc2h_dcmt_trap){
		rtd_pr_vt_err("TVSB4_DC2H module!\n");

		for(i = DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_CTRL_reg; i <= DC2H_R2Y_DITHER_4XXTO4XX_DC2H_RGB2YUV_TAB_Yo_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		for(i = DC2H_SCALEDOWN_DC2H1_UZD_CTRL0_reg; i <= DC2H_SCALEDOWN_DC2H1_UZD_444to422_CTRL_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		rtd_pr_vt_err("0xB80299A0=%x\n", IoReg_Read32(DC2H_SCALEDOWN_DC2H1_UZD_H_DB_CTRL_reg));
		rtd_pr_vt_err("0xB80299A4=%x\n", IoReg_Read32(DC2H_SCALEDOWN_DC2H1_UZD_V_DB_CTRL_reg));

		for(i = DC2H_CAP_DC2H_CTRL_reg; i <= DC2H_CAP_DC2H_BLACK_OUT_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		rtd_pr_vt_err("0xB8029D10=%x\n", IoReg_Read32(DC2H_CAP_DC2H_CAP_doublebuffer_reg));

		for(i = DC2H_CAP_DC2H_CAP0_WR_DMA_num_bl_wrap_addr_reg; i <= DC2H_CAP_DC2H_CAP0_DMA_WR_status_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		for(i = DC2H_CAP_DC2H_CAP1_WR_DMA_num_bl_wrap_addr_reg; i <= DC2H_CAP_DC2H_CAP1_DMA_WR_Rule_check_low_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		for(i = DC2H_CAP_DC2H_CAP2_WR_DMA_num_bl_wrap_addr_reg; i<= DC2H_CAP_DC2H_CAP2_DMA_WR_Rule_check_low_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		for(i = DC2H_CAP_DC2H_3DMaskTo2D_Ctrl_reg; i<= DC2H_CAP_DC2H_3DMaskTo2D_doublebuffer_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}
		rtd_pr_vt_err("0xB8029DE4=%x\n", IoReg_Read32(DC2H_CAP_DC2H_Interrupt_Enable_reg));
	}

	if(dc2h2_dcmt_trap){
		rtd_pr_vt_err("TVSB4_DC2H2 module!\n");

		for(i = DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_CTRL_reg; i <= DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_Yo_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		for(i = DC2H2_SCALEDOWN_DC2H2_UZD_CTRL0_reg; i <= DC2H2_SCALEDOWN_DC2H2_UZD_INITIAL_INT_VALUE_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		rtd_pr_vt_err("0xB80664A0=%x\n", IoReg_Read32(DC2H2_SCALEDOWN_DC2H2_UZD_H_DB_CTRL_reg));
		rtd_pr_vt_err("0xB80664A4=%x\n", IoReg_Read32(DC2H2_SCALEDOWN_DC2H2_UZD_V_DB_CTRL_reg));
		for(i = DC2H2_CAP_DC2H2_PQ_CMP_reg; i <= DC2H2_CAP_DC2H2_PQ_CMP_BIT_LLB_A_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		for(i = DC2H2_CAP_DC2H2_CTRL_reg; i <= DC2H2_CAP_DC2H2_BLACK_OUT_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		rtd_pr_vt_err("0xB8066710=%x\n", IoReg_Read32(DC2H2_CAP_DC2H2_CAP_doublebuffer_reg));

		for(i = DC2H2_CAP_DC2H2_CAP0_Cap_CTL0_reg; i <= DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL2_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		for(i = DC2H2_CAP_DC2H2_CAP1_WR_DMA_num_bl_wrap_addr_reg; i <= DC2H2_CAP_DC2H2_CAP1_DMA_WR_Rule_check_low_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		for(i = DC2H2_CAP_DC2H2_CAP2_WR_DMA_num_bl_wrap_addr_reg; i<= DC2H2_CAP_DC2H2_CAP2_DMA_WR_Rule_check_low_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}

		for(i = DC2H2_CAP_DC2H2_3DMaskTo2D_Ctrl_reg; i<= DC2H2_CAP_DC2H2_3DMaskTo2D_doublebuffer_reg; i=i+4){
			rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
		}
		rtd_pr_vt_err("0xB8066894=%x\n", IoReg_Read32(DC2H2_CAP_DC2H2_Interrupt_Enable_reg));
	}
}
DCMT_DEBUG_INFO_DECLARE(sb4_dc2h_mdscpu, sb4_dc2h_debug_info);
#endif


int vivt_init_module(void)
{
	int result;
  	int devno;
	dev_t dev = 0;

#if IS_ENABLED(CONFIG_RTK_KDRV_DC_MEMORY_TRASH_DETCTER)
	/******************DCMT register callback func in init flow******************/
	DCMT_DEBUG_INFO_REGISTER(sb4_dc2h_mdscpu, sb4_dc2h_debug_info);
	rtd_pr_vt_debug("DCMT_DEBUG_INFO_REGISTER(sb4_dc2h_mdscpu, sb4_dc2h_debug_info)\n");
#endif

	rtd_pr_vt_debug("\n\n\n\n *****************  vt init module  *********************\n\n\n\n");
	if (vivt_major) {
		dev = MKDEV(vivt_major, vivt_minor);
		result = register_chrdev_region(dev, vivt_nr_devs, VT_DEVICE_NAME);
	} else {
		result = alloc_chrdev_region(&dev, vivt_minor, vivt_nr_devs,VT_DEVICE_NAME);
		vivt_major = MAJOR(dev);
	}
	if (result < 0) {
		rtd_pr_vt_debug("vt: can't get major %d\n", vivt_major);
		return result;
	}

	rtd_pr_vt_debug("vt init module major number = %d\n", vivt_major);

	vivt_class = class_create(THIS_MODULE,VT_DEVICE_NAME);

	if (IS_ERR(vivt_class))
	{
		rtd_pr_vt_debug("scalevt: can not create class...\n");
		result = PTR_ERR(vivt_class);
		goto fail_class_create;
	}

	vivt_class->devnode = vivt_devnode;

	vivt_platform_devs = platform_device_register_simple(VT_DEVICE_NAME, -1, NULL, 0);

    	if((result=platform_driver_register(&vivt_device_driver)) != 0){
		rtd_pr_vt_debug("scalevt: can not register platform driver...\n");
		result = -ENOMEM;
		goto fail_platform_driver_register;
    	}

    	devno = MKDEV(vivt_major, vivt_minor);
    	cdev_init(&vivt_cdev, &vivt_fops);
    	vivt_cdev.owner = THIS_MODULE;
   	vivt_cdev.ops = &vivt_fops;
	result = cdev_add (&vivt_cdev, devno, 1);
	if (result<0)
	{
		rtd_pr_vt_debug("scalevt: can not add character device...\n");
		goto fail_cdev_init;
	}
    	device_create(vivt_class, NULL, MKDEV(vivt_major, 0), NULL, VT_DEVICE_NAME);
	sema_init(&VT_Semaphore, 1);
	sema_init(&VTBlock_Semaphore, 1);
	sema_init(&VT_Semaphore_enq, 1);

    return 0;	//success

fail_cdev_init:
	platform_driver_unregister(&vivt_device_driver);
fail_platform_driver_register:
	platform_device_unregister(vivt_platform_devs);
	vivt_platform_devs = NULL;
	class_destroy(vivt_class);
fail_class_create:
	vivt_class = NULL;
	unregister_chrdev_region(vivt_devno, 1);
	return result;
}



void __exit vivt_cleanup_module(void)
{
	dev_t devno = MKDEV(vivt_major, vivt_minor);
	rtd_pr_vt_debug("rtice clean module vt_major = %d\n", vivt_major);
  	device_destroy(vivt_class, MKDEV(vivt_major, 0));
  	class_destroy(vivt_class);
	vivt_class = NULL;
	cdev_del(&vivt_cdev);
   	/* device driver removal */
	if(vivt_platform_devs) {
		platform_device_unregister(vivt_platform_devs);
		vivt_platform_devs = NULL;
	}
  	platform_driver_unregister(&vivt_device_driver);
	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, vivt_nr_devs);
}


#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(vivt_init_module);
module_exit(vivt_cleanup_module);
#endif
#endif // #ifndef UT_flag
