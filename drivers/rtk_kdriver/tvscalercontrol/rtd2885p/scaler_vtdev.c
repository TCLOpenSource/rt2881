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
#include <linux/gcd.h>
#include <asm/cacheflush.h>
#include <linux/pageremap.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <uapi/linux/const.h>
#include <linux/mm.h>
#include <rtd_log/rtd_module_log.h>
#include <linux/fs.h>/*for dump buffer to usb file*/
#include <linux/hrtimer.h>/*for wait vsync hr sleep*/
#include <linux/version.h>
#include <rtk_kdriver/RPCDriver.h>
#include <rtk_dc_mt.h>

#include <rbus/dc2h_dma_reg.h>
#include <rbus/M8P_dc2h_dma_reg.h>
#include <rbus/dc2h_vi_reg.h>
#include <rbus/dc2h_scaledown_reg.h>
#include <rbus/dc2h_rgb2yuv_reg.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/scaleup_reg.h>
#include <rbus/scaledown_reg.h>
#include <rbus/scpu_core_reg.h>


#include <rbus/rgb2yuv_reg.h>
#include <rbus/yuv2rgb_reg.h>
#include <rbus/timer_reg.h>
#include <mach/rtk_platform.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <tvscalercontrol/vip/scalerColor.h>

#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
#else
#include <rtk_kdriver/scalercommon/scalerCommon.h>
#endif
#include <tvscalercontrol/scaler/scalerstruct.h>
#include <tvscalercontrol/scalerdrv/pipmp.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <tvscalercontrol/scalerdrv/zoom_smoothtoggle_lib.h>
#include <tvscalercontrol/scalerdrv/scaler_nonlibdma.h>
#include <scaler/vipRPCCommon.h>
#include <tvscalercontrol/vip/ultrazoom.h>
#include <vo/rtk_vo.h>

#include "scaler_vtdev.h"
#include "scaler_vscdev.h"
#include "scaler_vpqmemcdev.h"
#include "rtk_kadp_se.h"

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

VT_CAPTURE_CTRL_T CaptureCtrl_VT;
#ifndef UT_flag
static dev_t vivt_devno = 0;//vt device number
static struct cdev vivt_cdev;
#endif // #ifndef UT_flag

/*vt init work width and length*/
#define VT_CAP_FRAME_WIDTH_2K1k		1920
#define VT_CAP_FRAME_HEIGHT_2K1k	1080

#define VT_CAP_FRAME_WIDTH_4K2K		3840
#define VT_CAP_FRAME_HEIGHT_4K2K	2160

#define VT_FPS_OUTPUT 	(60)
#define MAX_CAP_BUF_NUM 5

#define VT_SEQCAP_BURSTLEN 512 //32*128/8
#define VT_BLKCAP_BURSTLEN 512 //64*8byte

#define ABNORMAL_DVS_FIRST_HAPPEN 1
#define WAIT_DVS_STABLE_COUNT 8

KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_INFO_T VFODState = {FALSE,FALSE,TRUE,1};

static KADP_VT_DUMP_LOCATION_TYPE_T VTDumpLocation = KADP_VT_MAX_LOCATION;
static VT_CUR_CAPTURE_INFO curCapInfo;
extern unsigned char get_vsc_mutestatus(void);
extern unsigned char drvif_scaler_get_abnormal_dvs_long_flag(void);
extern unsigned char drvif_scaler_get_abnormal_dvs_short_flag(void);
extern unsigned char get_vo_camera_flow_flag(unsigned char ch);
static unsigned char VtFunction = FALSE;
static unsigned char VtEnableFRCMode = TRUE;

static unsigned int VtBufferNum = MAX_CAP_BUF_NUM;
static unsigned char VtSwBufferMode = FALSE;
static unsigned char VtCaptureVDC = FALSE;

static unsigned int vt_cap_frame_max_width = VT_CAP_FRAME_WIDTH_2K1k;
static unsigned int vt_cap_frame_max_height = VT_CAP_FRAME_HEIGHT_2K1k;

static unsigned long VtAllocatedBufferStartAdress[VT_MAX_SUPPORT_BUF_NUMS] = {0};
static VT_APP_CASE vt_app_case = VT_APP_CASE_NONE;

static unsigned int abnormal_dvs_cnt = 0;
static unsigned int wait_dvs_stable_cnt = 0;
static unsigned char vt_StreamOn_flag = FALSE;

static unsigned char VdecSecureStatus = FALSE;
static unsigned char DtvSecureStatus = FALSE;

static VT_CAP_FMT VT_Pixel_Format = VT_CAP_RGB888;
static unsigned int uiTargetFps = 0;
static unsigned int DC2H_InputWidthSta;
static unsigned int DC2H_InputLengthSta;
static unsigned int DC2H_InputWidth;
static unsigned int DC2H_InputLength;

unsigned char dqbuf_ioctl_fail_flag = FALSE;
unsigned char IndexOfFreezedVideoFrameBuffer = 0;
volatile unsigned int vfod_capture_out_W = VT_CAP_FRAME_WIDTH_2K1k;
volatile unsigned int vfod_capture_out_H = VT_CAP_FRAME_HEIGHT_2K1k;
volatile unsigned int vfod_capture_location = KADP_VT_DISPLAY_OUTPUT;

unsigned int vtPrintMask = 0;/*mask for vt print log*/
unsigned int vt_dequeue_enqueue_count = 0;
unsigned char vt_dequeue_overflow_flag = FALSE;
static VT_BUF_STATUS_T *pVtBufStatus = NULL;
static DEFINE_SPINLOCK(vt_buf_status_Spinlock);
static unsigned char vt_bufwidth_as_linestep = FALSE;

static unsigned char vt_crop_enable = FALSE;
/*------include extern functions----------*/
extern unsigned char force_enable_two_step_uzu(void);/* get d domain go two pixel mode? */
unsigned char debug_dump_data_to_file(unsigned int idx, unsigned int dump_w,unsigned int dump_h);
extern void MEMC_Set_malloc_address(UINT8 status);
extern void drvif_vt_block_vr_recording(unsigned char vrEnableFlag);
extern unsigned char get_vr_function(void);
extern void drvif_vt_reconfig_vr_recording(void);
extern PANEL_PIXEL_MODE get_panel_pixel_mode(void);
unsigned char Capture_BufferMemInit_VT(unsigned int buffernum);
void Capture_BufferMemDeInit_VT(unsigned int buffernum);
unsigned char HAL_VT_Finalize(void);
void drvif_DC2H_dispD_CaptureConfig(VT_CUR_CAPTURE_INFO capInfo);
void set_cap_buffer_size_by_AP(unsigned int usr_width, unsigned int usr_height);
void reset_dc2h_hw_setting(void);
unsigned char HAL_VT_GetVideoFrameBufferIndex(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, unsigned int *pIndexOfCurrentVideoFrameBuffer);
void drvif_memc_outBg_ctrl(bool enable);
void set_DC2H_cap_boundaryaddr(unsigned int VtBufferNum);
unsigned long vt_bufAddr_align(unsigned int val);

unsigned char do_vt_capture_streamon(void);
unsigned char do_vt_reqbufs(unsigned int buf_cnt);
unsigned char do_vt_streamoff(void);
unsigned char do_vt_dqbuf(unsigned int *pdqbuf_Index);
unsigned short Get_VFOD_FrameRate(void);
void vt_enable_dc2h(unsigned char state);
unsigned char get_dc2h_capture_state(void);
unsigned int sort_boundary_addr_min_index(void);
unsigned int sort_boundary_addr_max_index(void);
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

void handle_vt_test_case(void);
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

void set_dqbuf_ioctl_fail_flag(unsigned char flg)
{
	dqbuf_ioctl_fail_flag = flg;
}
unsigned char get_dqbuf_ioctl_fail_flag(void)
{
	return dqbuf_ioctl_fail_flag;
}

void set_vt_StreamOn_flag(unsigned char flg)
{
	vt_StreamOn_flag = flg;
}
unsigned char get_vt_StreamOn_flag(void)
{
	return vt_StreamOn_flag;
}

unsigned int get_vt_VtBufferNum(void)
{
	return VtBufferNum;
}

void set_vt_VtBufferNum(unsigned int value)
{
	VtBufferNum = value;
}

unsigned char get_vt_VtSwBufferMode(void)
{
	return VtSwBufferMode;
}

void set_vt_VtSwBufferMode(unsigned char value)
{
	VtSwBufferMode = value;
}

unsigned char get_vt_VtCaptureVDC(void)
{
	return VtCaptureVDC;
}

void set_vt_VtCaptureVDC(unsigned char value)
{
	VtCaptureVDC = value;
}

void set_VT_Pixel_Format(VT_CAP_FMT value)
{
	VT_Pixel_Format = value;
}

VT_CAP_FMT get_VT_Pixel_Format(void)
{
	return VT_Pixel_Format;
}

void vtforut_enable_dc2h(unsigned char state)
{
    curCapInfo.enable = state;
}

unsigned int get_framerateDivide(void)
{
	return VFODState.framerateDivide;
}
void set_framerateDivide(unsigned int value)
{
	VFODState.framerateDivide = value;
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
    return CaptureCtrl_VT.cap_buffer[idx].phyaddr;
}

void set_vt_target_fps(unsigned int val)
{
    uiTargetFps = val;
}
unsigned int get_vt_target_fps(void)
{
    return uiTargetFps;
}

void set_vt_app_case(VT_APP_CASE type)
{
    vt_app_case = type;
}

VT_APP_CASE get_vt_app_case(void)
{
    return vt_app_case;
}

void vt_dequeue_enqueue_count_add(void)
{
    vt_dequeue_enqueue_count++;
}

void vt_dequeue_enqueue_count_subtract(void)
{
    if(vt_dequeue_enqueue_count > 0){
		vt_dequeue_enqueue_count--;
	}
}

void set_vt_dequeue_enqueue_count(unsigned int cnt)
{
    vt_dequeue_enqueue_count = cnt;
}

unsigned int get_vt_dequeue_enqueue_count(void)
{
    return vt_dequeue_enqueue_count;
}

unsigned char get_vt_dequeue_overflow_flag(void)
{
    return vt_dequeue_overflow_flag;
}

void set_vt_dequeue_overflow_flag(unsigned char flag)
{
    vt_dequeue_overflow_flag = flag;
}

void set_vt_bufwidth_as_linestep(unsigned char val)
{
    vt_bufwidth_as_linestep = val;
}

unsigned char get_vt_bufwidth_as_linestep(void)
{
    return vt_bufwidth_as_linestep;
}

unsigned char get_vt_crop_enable(void)
{
	return vt_crop_enable;
}
void set_vt_crop_enable(unsigned char en)
{
	vt_crop_enable = en;
}
/*
D-domain Capture to Handler input data select, from :
0 : No input (data all 0)
1 : Select data from main_uzu output
2 .Select data from yuv2rgb output
3 : Select data from Sub-Title Mixer output
4 : Select data from OSD Mixer output
5 : Select data from Dither output
6 : Select data from Sub UZU output
7: Select data from Memc mux output
8. select data from VD output
*/

typedef struct
{
    unsigned int reserved:18 ;
    unsigned int y_pic_no:7;
    unsigned int c_pic_no:7;
} DC2H_YC_PIC_NO ;


typedef struct
{
    SIZE in_size;
    SIZE out_size;
    DC2H_YC_PIC_NO yc_pic_no;
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
    _OSD_MIXER,
    _Dither_Output,
    _Sub_UZU_Output,
    _Memc_mux_Output,
    _Memc_Mux_Input, //8
    _Idomain_Sdnr_input, //main sdnr input
	_Sub_Sdnr_input, //10
    _VD_OUTPUT
}DC2H_IN_SEL;


DC2H_HANDLER *pdc2h_hdl = NULL;
static DC2H_HANDLER dc2h_hdl;

#define CAP_LEN 32
#define CAP_REM 12

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
#define _RGB2YUV_Yo_odd			0x0a
#define _RGB2YUV_Y_gain				0x0b
#define _RGB2YUV_sel_RGB			0x0c
#define _RGB2YUV_set_Yin_offset		0x0d
#define _RGB2YUV_set_UV_out_offset	0x0e
#define _RGB2YUV_sel_UV_off			0x0f
#define _RGB2YUV_Matrix_bypass		0x10
#define _RGB2YUV_Enable_Y_gain		0x11

/*YUV to RBG table for RGB888 and ARGB888 capture format*/
static unsigned short tYUV2RGB_COEF_709_RGB_0_255[] =
{

// CCIR 709 RGB
        0x129f,   // m11
        0x0000,  // m12
        0x1660,  // m13

        0x129f,   // m21 >> 2 0x01D5 up bits at AT Name: cychen2 , Date: 2010/3/25
        0x7a80,   // m22 >> 1
        0x74a0,   // m23 >> 2
        0x129f,   // m31 >> 2
        0x1c50,   // m32 >> 2 0x0195  up bits at AT Name: cychen2 , Date: 2010/3/25
        0x0000,   // m33 >> 1

        0x0000,          // Yo_even
        0x0000,          // Yo_odd
        0x0000,          // Y_gain

        0x0000,  // sel_RGB
        0x0001,  // sel_Yin_offset
        0x0000,  // sel_UV_out_offset
        0x0000,  // sel_UV_off
        0x0000,  // Matrix_bypass
        0x0000,  // Enable_Y_gain

};

/*RGB to YUV table for nv12 nv16 capture format*/
static unsigned short tYUV2RGB_COEF_709_RGB_16_235[] =
{0x02EE, 0x09D3, 0x00FE, 0x7E62, 0x7A98, 0x0706, 0x0706, 0x799E, 0x7F5C, 0x040, 0x040, 0x0100, 0x0001, 0x0000, 0x0001, 0x0000, 0x0000, 0x0000}; //r2y 10bit
//{0x02EE, 0x09D3, 0x00FE, 0x7E62, 0x7A98, 0x0706, 0x0706, 0x799E, 0x7F5C, 0x0100, 0x0100, 0x0100, 0x0001, 0x0000, 0x0001, 0x0000, 0x0000, 0x0000};// r2y 12 bit /*RGB2YUV_COEF_RGB_0_255_TO_YCC709_16_235*/


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
    -2,   0,   1,   3,   6,  10,  15,  22,  32,  43,  58,  75,  95, 119, 145, 174,
    206, 240, 276, 314, 353, 391, 430, 467, 503, 536, 565, 590, 612, 628, 639, 646,
};


static  short *tScaleDown_COEF_TAB[] =
{
    tScale_Down_Coef_Blur,     tScale_Down_Coef_Mid,     tScale_Down_Coef_Sharp,     tScale_Down_Coef_2tap
};

void dc2h_wait_porch(void)
{
	unsigned int uzulinecntA =0;
	unsigned int uzulinecntB =0;

	unsigned int timeoutcount = 0x3ffff;
	ppoverlay_dv_den_start_end_RBUS dv_den_start_end_reg;
	ppoverlay_new_meas1_linecnt_real_RBUS new_meas1_linecnt_real_Reg;

	dv_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_DV_DEN_Start_End_reg);
	do {
		new_meas1_linecnt_real_Reg.regValue = rtd_inl(PPOVERLAY_new_meas1_linecnt_real_reg);
		uzulinecntA = new_meas1_linecnt_real_Reg.uzudtg_line_cnt_rt;
		new_meas1_linecnt_real_Reg.regValue = rtd_inl(PPOVERLAY_new_meas1_linecnt_real_reg);
		uzulinecntB = new_meas1_linecnt_real_Reg.uzudtg_line_cnt_rt;
		if((uzulinecntA == uzulinecntB)&&((uzulinecntA < (dv_den_start_end_reg.dv_den_sta))||(uzulinecntA > (dv_den_start_end_reg.dv_den_end)))) {
			break;
		}
		timeoutcount--;
	} while(timeoutcount);

	if(timeoutcount == 0){
		rtd_pr_vt_debug("[UZU DEN]timeout error!!!\n");
	}
}

#ifndef UT_flag
void vt_enable_dc2h(unsigned char state)
{
	//down(&VT_Semaphore);
	curCapInfo.enable = state;
	drvif_DC2H_dispD_CaptureConfig(curCapInfo);
	//up(&VT_Semaphore);
}
#endif // #ifndef UT_flag

unsigned char get_dc2h_capture_state(void)
{
	if ((get_VT_Pixel_Format() == VT_CAP_NV12) || (get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
	{
		if(DC2H_VI_DC2H_DMACTL_get_dmaen1(rtd_inl(DC2H_VI_DC2H_DMACTL_reg)) == 1)
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if(DC2H_DMA_dc2h_Seq_mode_CTRL1_get_cap_en(rtd_inl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg)) == DC2H_DMA_dc2h_Seq_mode_CTRL1_cap_en_mask)
			return TRUE;
		else
			return FALSE;
	}
}

unsigned char drvif_dc2h_swmode_inforpc(unsigned int onoff,unsigned int buffernumber,unsigned int width,unsigned int length){

	DC2H_SWMODE_STRUCT_T *swmode_infoptr;
	//unsigned int ulCount = 0;
	int ret;
    int i = 0;
    rtd_pr_vt_notice("fun:%s,onoff %d\n",__FUNCTION__, onoff);
	swmode_infoptr = (DC2H_SWMODE_STRUCT_T *)Scaler_GetShareMemVirAddr(SCALERIOC_SET_DC2H_SWMODE_ENABLE);
	//ulCount = sizeof(DC2H_SWMODE_STRUCT_T) / sizeof(unsigned int);

	swmode_infoptr->SwModeEnable = onoff;
	swmode_infoptr->buffernumber = buffernumber;
	swmode_infoptr->cap_format = (UINT32)get_VT_Pixel_Format();
	swmode_infoptr->cap_width = vt_cap_frame_max_width;
	swmode_infoptr->cap_length = vt_cap_frame_max_height;
    swmode_infoptr->user_fps = get_vt_target_fps();

    if(get_VT_Pixel_Format() == VT_CAP_NV12 || get_VT_Pixel_Format() == VT_CAP_NV16 || get_VT_Pixel_Format() == VT_CAP_NV21)
    {
        if(get_vt_bufwidth_as_linestep() == TRUE && (get_VT_Pixel_Format() == VT_CAP_NV12 || get_VT_Pixel_Format() == VT_CAP_NV21))
            swmode_infoptr->YbufferSize = vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height) - vt_cap_frame_max_width*(curCapInfo.y)/2;
        else
            swmode_infoptr->YbufferSize = vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height);
    }
    else if(get_VT_Pixel_Format() == VT_CAP_RGB888 || get_VT_Pixel_Format() == VT_CAP_ARGB8888 || get_VT_Pixel_Format() == VT_CAP_RGBA8888 || get_VT_Pixel_Format() == VT_CAP_ABGR8888)
    {
        swmode_infoptr->YbufferSize = CaptureCtrl_VT.cap_buffer[0].size;
    }
    else
    {
        rtd_pr_vt_emerg("fun:%s,unknow pixel format\n",__FUNCTION__);
        return FALSE;
    }

	if(Get_PANEL_VFLIP_ENABLE())
	{
        if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
        {
            for (i = 0; i < buffernumber; i++)
                swmode_infoptr->cap_buffer[i] = ((UINT32)CaptureCtrl_VT.cap_buffer[i].phyaddr + (vt_cap_frame_max_width*vt_cap_frame_max_height - vt_cap_frame_max_width));
        }
        else if(get_VT_Pixel_Format() == VT_CAP_RGB888)
        {
            for (i = 0; i < buffernumber; i++)
                swmode_infoptr->cap_buffer[i] = (((UINT32)CaptureCtrl_VT.cap_buffer[i].phyaddr) ? ((UINT32)CaptureCtrl_VT.cap_buffer[i].phyaddr + vt_cap_frame_max_width*vt_cap_frame_max_height*3 - vt_cap_frame_max_width*3) : (0));
        }
        else if(get_VT_Pixel_Format() == VT_CAP_ARGB8888 || get_VT_Pixel_Format() == VT_CAP_RGBA8888 || get_VT_Pixel_Format() == VT_CAP_ABGR8888)
        {
            for (i = 0; i < buffernumber; i++)
                swmode_infoptr->cap_buffer[i] = (((UINT32)CaptureCtrl_VT.cap_buffer[i].phyaddr) ? ((UINT32)CaptureCtrl_VT.cap_buffer[i].phyaddr + vt_cap_frame_max_width*vt_cap_frame_max_height*4 - vt_cap_frame_max_width*4) : (0));
        }
        else
        {
            rtd_pr_vt_notice("fun:%s,unknow pixel format\n",__FUNCTION__);
            return FALSE;
        }
	}
	else
	{
        for(i = 0; i < buffernumber; i++)
        {
            if(get_vt_bufwidth_as_linestep() == TRUE && (get_VT_Pixel_Format() == VT_CAP_NV12 || get_VT_Pixel_Format() == VT_CAP_NV21 || get_VT_Pixel_Format() == VT_CAP_NV16))
            {
                swmode_infoptr->cap_buffer[i] = ((UINT32)CaptureCtrl_VT.cap_buffer[i].phyaddr + curCapInfo.y*vt_cap_frame_max_width + curCapInfo.x);
            }
            else{
                swmode_infoptr->cap_buffer[i] = (UINT32)CaptureCtrl_VT.cap_buffer[i].phyaddr;
            }
        }
	}

	rtd_pr_vt_debug("[VT]SwModeEnable = %d\n",swmode_infoptr->SwModeEnable);
	rtd_pr_vt_debug("[VT]buffernumber = %d\n",swmode_infoptr->buffernumber);
	rtd_pr_vt_debug("[VT]cap_format = %d\n",swmode_infoptr->cap_format);
	rtd_pr_vt_debug("[VT]cap_width = %d\n",swmode_infoptr->cap_width);
	rtd_pr_vt_debug("[VT]cap_length = %d\n",swmode_infoptr->cap_length);
    rtd_pr_vt_debug("[VT]user_fps = %d\n",swmode_infoptr->user_fps);
	rtd_pr_vt_debug("[VT]YbufferSize = %d\n",swmode_infoptr->YbufferSize);
	rtd_pr_vt_debug("[VT]cap_buffer[0] = 0x%x\n",swmode_infoptr->cap_buffer[0]);
	rtd_pr_vt_debug("[VT]cap_buffer[1] = 0x%x\n",swmode_infoptr->cap_buffer[1]);
	rtd_pr_vt_debug("[VT]cap_buffer[2] = 0x%x\n",swmode_infoptr->cap_buffer[2]);
	rtd_pr_vt_debug("[VT]cap_buffer[3] = 0x%x\n",swmode_infoptr->cap_buffer[3]);
	rtd_pr_vt_debug("[VT]cap_buffer[4] = 0x%x\n",swmode_infoptr->cap_buffer[4]);
	//change endian
	swmode_infoptr->SwModeEnable = htonl(swmode_infoptr->SwModeEnable);
	swmode_infoptr->buffernumber = htonl(swmode_infoptr->buffernumber);
	swmode_infoptr->cap_format = htonl(swmode_infoptr->cap_format);
	swmode_infoptr->cap_width = htonl(swmode_infoptr->cap_width);
	swmode_infoptr->cap_length = htonl(swmode_infoptr->cap_length);
    swmode_infoptr->user_fps = htonl(swmode_infoptr->user_fps);
	swmode_infoptr->YbufferSize = htonl(swmode_infoptr->YbufferSize);

    for(i = 0; i < buffernumber; i++)
    {
        swmode_infoptr->cap_buffer[i] = htonl(swmode_infoptr->cap_buffer[i]);
    }

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_SET_DC2H_SWMODE_ENABLE,0,0))){
		rtd_pr_vt_emerg("[dc2h]ret=%d, SCALERIOC_SET_DC2H_SWMODE_ENABLE RPC fail !!!\n", ret);
		return FALSE;
	}
	return TRUE;
}

unsigned char drvif_dc2h_se_inforpc(unsigned int capwidth,unsigned int caplength)
{
	VT_SE_CAPTURE_INFO_T *se_capture_infoptr = NULL;
	//unsigned int ulCount = 0;
	int ret;

	rtd_pr_vt_notice("fun:%s=%d\n",__FUNCTION__, __LINE__);

	curCapInfo.capWid = capwidth;
	curCapInfo.capLen = caplength;

	se_capture_infoptr = (VT_SE_CAPTURE_INFO_T *)Scaler_GetShareMemVirAddr(SCALERIOC_SET_VT_SE_CAPTURE_ENABLE);
	//ulCount = sizeof(VT_SE_CAPTURE_INFO_T) / sizeof(unsigned int);
	memset(se_capture_infoptr, 0, sizeof(VT_SE_CAPTURE_INFO_T));

	se_capture_infoptr->capWid = capwidth;

	se_capture_infoptr->capLen = caplength;

	//change endian
	se_capture_infoptr->capWid = htonl(se_capture_infoptr->capWid);

	se_capture_infoptr->capLen = htonl(se_capture_infoptr->capLen);

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_SET_VT_SE_CAPTURE_ENABLE,0,0)))
	{
		rtd_pr_vt_emerg("[dc2h]ret=%d, SCALERIOC_SET_DC2H_SE_CAPTURE_ENABLE RPC fail !!!\n", ret);
		return FALSE;
	}
	return TRUE;
}

#ifndef UT_flag

unsigned char is_scale_down_case(SIZE *in, SIZE *out)
{
	if(in->nWidth > out->nWidth || in->nLength > out->nLength){
		rtd_pr_vt_emerg("vt scale down case\n");
		return TRUE;
	}else{
		rtd_pr_vt_emerg("vt non scale down case\n");
		return FALSE;
	}
}

void get_shm_vt_buf_status(void)
{
    pVtBufStatus = (VT_BUF_STATUS_T *)Scaler_GetShareMemVirAddr(SCLAERIOC_SET_VT_DC2H_BUF_STATUS);
}

VT_BUFFER_STATUS get_vt_buf_status(unsigned int idx)
{
    VT_BUFFER_STATUS vtbuf_status = VT_BUFFER_UNKNOW_STATUS;
    if(pVtBufStatus == NULL)
    {
        rtd_pr_vt_notice("[error0] shm vt_buf_status pointer null\n");
        return VT_BUFFER_UNKNOW_STATUS;
    }
    vtbuf_status = htonl(pVtBufStatus->vtBufStatus[idx]);

    return vtbuf_status;
}

void set_vt_buf_dequeue(unsigned int idx)
{
    unsigned int vt_buf_sta = VT_BUFFER_AP_DEQUEUED;
    /* set share memory which buffer dequeued */

    if(pVtBufStatus == NULL)
    {
        rtd_pr_vt_notice("[error1] shm vt_buf_status pointer null\n");
        return;
    }
    pVtBufStatus->vtBufStatus[idx] = Scaler_ChangeUINT32Endian(vt_buf_sta);
    //rtd_pr_vt_notice("[VT][dequeue] %d %d\n",idx, htonl(vt_buf_status->vtBufStatus[idx]));
}

void set_vt_buf_enqueue(unsigned int idx)
{
    unsigned int vt_buf_sta = VT_BUFFER_AP_ENQUEUED;

    if(pVtBufStatus == NULL)
    {
        rtd_pr_vt_notice("[error2] shm vt_buf_status pointer null\n");
        return;
    }

    if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX)
        rtd_pr_vt_notice("enq 90K %d,idx %d,pre_sta %d\n", IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), idx, get_vt_buf_status(idx));

    pVtBufStatus->vtBufStatus[idx] = Scaler_ChangeUINT32Endian(vt_buf_sta);
}

void reset_vt_buf_status(void)
{
    int i = 0;
    unsigned int vt_buf_sta = VT_BUFFER_AP_ENQUEUED;

    if(pVtBufStatus == NULL)
    {
        rtd_pr_vt_notice("[error3] shm vt_buf_status pointer null\n");
        return;
    }
    else
    {
        pVtBufStatus->printMask = 0;
        for(i = 0; i < VT_CAP_BUF_NUMS; i++)
            pVtBufStatus->vtBufStatus[i] = Scaler_ChangeUINT32Endian(vt_buf_sta);
    }

    pVtBufStatus = NULL;
}

void init_vt_write_read_index(void)
{
    unsigned int init_wr_idx = 0;
	unsigned int init_rd_idx = 0xFF;
    if(pVtBufStatus == NULL)
        rtd_pr_vt_notice("[error] shm vt_buf_status pointer null\n");
    else
    {
        pVtBufStatus->write = Scaler_ChangeUINT32Endian(init_wr_idx);
        pVtBufStatus->read = Scaler_ChangeUINT32Endian(init_rd_idx);
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

void set_dc2h_black_out(bool offon)
{
	if(get_mach_type() == MACH_ARCH_RTK2885P){
		return;
	}else{
		M8P_dc2h_dma_dc2h_black_out_RBUS M8P_dc2h_dma_dc2h_black_out_reg;
		M8P_dc2h_dma_dc2h_black_out_reg.regValue = IoReg_Read32(M8P_DC2H_DMA_DC2H_BLACK_OUT_reg);
		M8P_dc2h_dma_dc2h_black_out_reg.dc2h_black_out_en = offon;
		IoReg_Write32(M8P_DC2H_DMA_DC2H_BLACK_OUT_reg, M8P_dc2h_dma_dc2h_black_out_reg.regValue);
	}
}

void set_dc2h_input_hact(unsigned int in_hact)
{
	if(get_mach_type() == MACH_ARCH_RTK2885P){
		return;
	}else{
		M8P_dc2h_dma_dc2h_in_size_RBUS M8P_dc2h_dma_dc2h_in_size_reg;
		M8P_dc2h_dma_dc2h_in_size_reg.regValue = IoReg_Read32(M8P_DC2H_DMA_DC2H_IN_SIZE_reg);
		M8P_dc2h_dma_dc2h_in_size_reg.dc2h_in_hact = in_hact;
		IoReg_Write32(M8P_DC2H_DMA_DC2H_IN_SIZE_reg, M8P_dc2h_dma_dc2h_in_size_reg.regValue);
	}
}

void set_dc2h_dma_frame_drop_ctrl(unsigned int in_fps, unsigned int out_fps)
{
	if(get_mach_type() == MACH_ARCH_RTK2885P){
		return;
	}else{
		M8P_dc2h_dma_dc2h_fdrop_ctrl_RBUS M8P_dc2h_dma_dc2h_fdrop_ctrl_reg;
		unsigned char fdrop_flag = FALSE;
		bool fdrop_en = FALSE;
		if(out_fps == 0 || in_fps == 0){
			IoReg_ClearBits(M8P_DC2H_DMA_DC2H_FDROP_CTRL_reg, M8P_DC2H_DMA_DC2H_FDROP_CTRL_fdrop_dc2h_en_mask);
			return;
		}

		fdrop_flag = ((in_fps / out_fps) <= 2) ? FALSE : TRUE;
		fdrop_en = (get_vt_VtSwBufferMode() == TRUE && fdrop_flag);
		M8P_dc2h_dma_dc2h_fdrop_ctrl_reg.regValue = IoReg_Read32(M8P_DC2H_DMA_DC2H_FDROP_CTRL_reg);
		if(fdrop_en == TRUE){
			unsigned int gcd_res = gcd(in_fps, out_fps);
			M8P_dc2h_dma_dc2h_fdrop_ctrl_reg.fdrop_dc2h_a = (out_fps/gcd_res) - 1;
			M8P_dc2h_dma_dc2h_fdrop_ctrl_reg.fdrop_dc2h_b = (in_fps/gcd_res) - 1;
			M8P_dc2h_dma_dc2h_fdrop_ctrl_reg.fdrop_dc2h_en = 1;
			IoReg_Write32(M8P_DC2H_DMA_DC2H_FDROP_CTRL_reg, M8P_dc2h_dma_dc2h_fdrop_ctrl_reg.regValue);
		}else{
			M8P_dc2h_dma_dc2h_fdrop_ctrl_reg.fdrop_dc2h_en = 0;
			IoReg_Write32(M8P_DC2H_DMA_DC2H_FDROP_CTRL_reg, M8P_dc2h_dma_dc2h_fdrop_ctrl_reg.regValue);
		}
	}
}

void dc2h_1p_conv_2p_en(DC2H_IN_SEL in_sel)
{
	if(get_mach_type() == MACH_ARCH_RTK2885P){
		return;
	}else{
		M8P_dc2h_dma_dc2h_fdrop_ctrl_RBUS M8P_dc2h_dma_dc2h_fdrop_ctrl_reg;
		M8P_dc2h_dma_dc2h_fdrop_ctrl_reg.regValue = IoReg_Read32(M8P_DC2H_DMA_DC2H_FDROP_CTRL_reg);
		if(in_sel == _Sub_Sdnr_input){ //always 1p
			M8P_dc2h_dma_dc2h_fdrop_ctrl_reg.conv_2p_en = 1;
		}else if(in_sel == _Idomain_Sdnr_input && (is_scaler_input_2p_mode(SLR_MAIN_DISPLAY) == FALSE)){ //input 1p
			M8P_dc2h_dma_dc2h_fdrop_ctrl_reg.conv_2p_en = 1;
		}else{ //aready 2p
			M8P_dc2h_dma_dc2h_fdrop_ctrl_reg.conv_2p_en = 0;
		}
		IoReg_Write32(M8P_DC2H_DMA_DC2H_FDROP_CTRL_reg, M8P_dc2h_dma_dc2h_fdrop_ctrl_reg.regValue);
	}
}

DC2H_Enable_rgb2yuv vt_colorformattransfer_enable(DC2H_IN_SEL dc2h_in_sel)
{
    DC2H_Enable_rgb2yuv dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;

    if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
    {
        if((dc2h_in_sel == _MAIN_UZU) || (dc2h_in_sel == _TWO_SECOND_UZU) || (dc2h_in_sel == _Sub_UZU_Output))  //YUV format
        {
            dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
        }
        else if(dc2h_in_sel == _Idomain_Sdnr_input)
        {
            if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == SCALER_INPUT_COLOR_RGB) && (Get_Val_vsc_run_pc_mode() == TRUE))
            {
                dc2h_enrgb2yuv_state = DC2H_Colconv_RGBtoYUV;
            }
            else
            {
               dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
            }
        }else if(dc2h_in_sel == _Sub_Sdnr_input){
			if((Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_COLOR_SPACE) == SCALER_INPUT_COLOR_RGB) && 
				(RGB2YUV_ICH2_RGB2YUV_CTRL_get_en_rgb2yuv(IoReg_Read32(RGB2YUV_ICH2_RGB2YUV_CTRL_reg)) == 0)){ //rgb input
                dc2h_enrgb2yuv_state = DC2H_Colconv_RGBtoYUV;
            }else if((Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_COLOR_SPACE) != SCALER_INPUT_COLOR_RGB) && 
				(RGB2YUV_ICH2_RGB2YUV_CTRL_get_en_rgb2yuv(IoReg_Read32(RGB2YUV_ICH2_RGB2YUV_CTRL_reg)) == 2)){ //rgb input
				dc2h_enrgb2yuv_state = DC2H_Colconv_RGBtoYUV;
			}else{
				dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
			}
		}
        else{
            dc2h_enrgb2yuv_state = DC2H_Colconv_RGBtoYUV; //Enable RGB to YUV conversion (YUV out)
        }
    }
    else
    {
        if((dc2h_in_sel == _MAIN_UZU) || (dc2h_in_sel == _TWO_SECOND_UZU) || (dc2h_in_sel == _Sub_UZU_Output))
        {
            dc2h_enrgb2yuv_state = DC2H_Colconv_YUVtoRGB; //Enable YUV to RGB conversion (GBR out)
        }
        else if(dc2h_in_sel == _Idomain_Sdnr_input)
        {
            if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == SCALER_INPUT_COLOR_RGB) && (Get_Val_vsc_run_pc_mode() == TRUE))
            {
                dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
            }
            else
            {
                dc2h_enrgb2yuv_state = DC2H_Colconv_YUVtoRGB;
            }
        }else if(dc2h_in_sel == _Sub_Sdnr_input){
			if((Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_COLOR_SPACE) == SCALER_INPUT_COLOR_RGB) && 
				(RGB2YUV_ICH2_RGB2YUV_CTRL_get_en_rgb2yuv(IoReg_Read32(RGB2YUV_ICH2_RGB2YUV_CTRL_reg)) == 0)){ //rgb input
                dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
            }else if((Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_COLOR_SPACE) != SCALER_INPUT_COLOR_RGB) && 
				(RGB2YUV_ICH2_RGB2YUV_CTRL_get_en_rgb2yuv(IoReg_Read32(RGB2YUV_ICH2_RGB2YUV_CTRL_reg)) == 2)){ //rgb input
				dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
			}else{
				dc2h_enrgb2yuv_state = DC2H_Colconv_YUVtoRGB;
			}
		}
        else{
            dc2h_enrgb2yuv_state = DC2H_Colconv_DISABLE;
        }
    }
    rtd_pr_vt_notice("[VT]get_VT_Pixel_Format()=%x, dc2h_in_sel=%x, dc2h_en_rgb2yuv=%d\n",get_VT_Pixel_Format(), dc2h_in_sel, dc2h_enrgb2yuv_state);
    return dc2h_enrgb2yuv_state;
}

void drvif_color_colorspaceyuv2rgbtransfer(DC2H_IN_SEL dc2h_in_sel)
{
	unsigned short *table_index = 0;

	dc2h_rgb2yuv_dc2h_rgb2yuv_ctrl_RBUS  dc2h_rgb2yuv_ctrl_reg;
	dc2h_rgb2yuv_dc2h_tab1_m11_m12_RBUS dc2h_tab1_m11_m12_reg;
	dc2h_rgb2yuv_dc2h_tab1_m13_m21_RBUS dc2h_tab1_m13_m21_reg;
	dc2h_rgb2yuv_dc2h_tab1_m22_m23_RBUS dc2h_tab1_m22_m23_reg;
	dc2h_rgb2yuv_dc2h_tab1_m31_m32_RBUS dc2h_tab1_m31_m32_reg;
	dc2h_rgb2yuv_dc2h_tab1_m33_y_gain_RBUS dc2h_tab1_m33_y_gain_reg;
	dc2h_rgb2yuv_dc2h_tab1_yo_RBUS dc2h_tab1_yo_reg;

	if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
		table_index = tYUV2RGB_COEF_709_RGB_16_235;
	else
		table_index = tYUV2RGB_COEF_709_RGB_0_255;

	//main  all tab-1
	dc2h_tab1_m11_m12_reg.m11 = table_index [_RGB2YUV_m11];
	dc2h_tab1_m11_m12_reg.m12 = table_index [_RGB2YUV_m12];
	//rtd_pr_vt_info("dc2h_tab1_m11_m12_reg.regValue = %x\n", dc2h_tab1_m11_m12_reg.regValue);
	rtd_outl(DC2H_RGB2YUV_DC2H_Tab1_M11_M12_reg, dc2h_tab1_m11_m12_reg.regValue);

	dc2h_tab1_m13_m21_reg.m21 = table_index [_RGB2YUV_m21];
	dc2h_tab1_m13_m21_reg.m13 = table_index [_RGB2YUV_m13];
	//rtd_pr_vt_info("dc2h_tab1_m13_m21_reg.regValue = %x\n", dc2h_tab1_m13_m21_reg.regValue);
	rtd_outl(DC2H_RGB2YUV_DC2H_Tab1_M13_M21_reg, dc2h_tab1_m13_m21_reg.regValue);

	dc2h_tab1_m22_m23_reg.m22 = table_index [_RGB2YUV_m22];
	dc2h_tab1_m22_m23_reg.m23 = table_index [_RGB2YUV_m23];
	//rtd_pr_vt_info("dc2h_tab1_m22_m23_reg.regValue = %x\n", dc2h_tab1_m22_m23_reg.regValue);
	rtd_outl(DC2H_RGB2YUV_DC2H_Tab1_M22_M23_reg, dc2h_tab1_m22_m23_reg.regValue);

	dc2h_tab1_m31_m32_reg.m31 = table_index [_RGB2YUV_m31];
	dc2h_tab1_m31_m32_reg.m32 = table_index [_RGB2YUV_m32];
	//rtd_pr_vt_info("dc2h_tab1_m31_m32_reg.regValue = %x\n", dc2h_tab1_m31_m32_reg.regValue);
	rtd_outl(DC2H_RGB2YUV_DC2H_Tab1_M31_M32_reg, dc2h_tab1_m31_m32_reg.regValue);

	dc2h_tab1_m33_y_gain_reg.m33 = table_index [_RGB2YUV_m33];
	dc2h_tab1_m33_y_gain_reg.yo_gain= table_index [_RGB2YUV_Y_gain];
	//rtd_pr_vt_info("dc2h_tab1_m33_y_gain_reg.regValue = %x\n", dc2h_tab1_m33_y_gain_reg.regValue);
	rtd_outl(DC2H_RGB2YUV_DC2H_Tab1_M33_Y_Gain_reg, dc2h_tab1_m33_y_gain_reg.regValue);

	dc2h_tab1_yo_reg.yo_even= table_index [_RGB2YUV_Yo_even];
	dc2h_tab1_yo_reg.yo_odd= table_index [_RGB2YUV_Yo_odd];
	//rtd_pr_vt_info("dc2h_tab1_yo_reg.regValue = %x\n", dc2h_tab1_yo_reg.regValue);
	rtd_outl(DC2H_RGB2YUV_DC2H_Tab1_Yo_reg, dc2h_tab1_yo_reg.regValue);

	dc2h_rgb2yuv_ctrl_reg.regValue = rtd_inl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg);
	dc2h_rgb2yuv_ctrl_reg.sel_rgb= table_index [_RGB2YUV_sel_RGB];
	dc2h_rgb2yuv_ctrl_reg.set_r_in_offset= table_index [_RGB2YUV_set_Yin_offset];
	dc2h_rgb2yuv_ctrl_reg.set_uv_out_offset= table_index [_RGB2YUV_set_UV_out_offset];
	dc2h_rgb2yuv_ctrl_reg.sel_uv_off= table_index [_RGB2YUV_sel_UV_off];
	dc2h_rgb2yuv_ctrl_reg.matrix_bypass= table_index [_RGB2YUV_Matrix_bypass];
	dc2h_rgb2yuv_ctrl_reg.sel_y_gain= table_index [_RGB2YUV_Enable_Y_gain];
	dc2h_rgb2yuv_ctrl_reg.dc2h_in_sel = dc2h_in_sel;

	dc2h_rgb2yuv_ctrl_reg.en_rgb2yuv = vt_colorformattransfer_enable(dc2h_in_sel);

	rtd_outl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg, dc2h_rgb2yuv_ctrl_reg.regValue);
}

void drvif_color_ultrazoom_set_dc2h_scale_down(DC2H_HANDLER *pdc2h_hdl)
{
	unsigned int SDRatio;
	unsigned int SDFilter=0;
	unsigned int tmp_data;
	short *coef_pt;
	unsigned int i;
	unsigned long long tmpLength = 0;
	unsigned int nFactor;
	unsigned int D = 0;
	unsigned char Hini, Vini, a;
	unsigned short S1,S2;

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	dc2h_scaledown_dc2h_hsd_ctrl0_RBUS        dc2h_hsd_ctrl0_reg;
	dc2h_scaledown_dc2h_vsd_ctrl0_RBUS                      dc2h_vsd_ctrl0_reg;
	dc2h_scaledown_dc2h_hsd_scale_hor_factor_RBUS  dc2h_hsd_scale_hor_factor_reg;
	dc2h_scaledown_dc2h_vsd_scale_ver_factor_RBUS  dc2h_vsd_scale_ver_factor_reg;
	dc2h_scaledown_dc2h_hsd_hor_segment_RBUS    dc2h_hsd_hor_segment_reg;
	//dc2h_scaledown_dc2h_hsd_hor_delta1_RBUS      dc2h_hsd_hor_delta1_reg;
	dc2h_scaledown_dc2h_hsd_initial_value_RBUS      dc2h_hsd_initial_value_reg;
	dc2h_scaledown_dc2h_vsd_initial_value_RBUS      dc2h_vsd_initial_value_reg;

	dc2h_hsd_ctrl0_reg.regValue    = rtd_inl(DC2H_SCALEDOWN_DC2H_HSD_Ctrl0_reg);
	dc2h_vsd_ctrl0_reg.regValue    = rtd_inl(DC2H_SCALEDOWN_DC2H_VSD_Ctrl0_reg);
	dc2h_hsd_scale_hor_factor_reg.regValue  = rtd_inl(DC2H_SCALEDOWN_DC2H_HSD_Scale_Hor_Factor_reg);
	dc2h_vsd_scale_ver_factor_reg.regValue  = rtd_inl(DC2H_SCALEDOWN_DC2H_VSD_Scale_Ver_Factor_reg);
	dc2h_hsd_hor_segment_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H_HSD_Hor_Segment_reg);
	//dc2h_hsd_hor_delta1_reg.regValue  = rtd_inl(DC2H_SCALEDOWN_DC2H_HSD_Hor_Delta1_reg);
	dc2h_hsd_initial_value_reg.regValue  = rtd_inl(DC2H_SCALEDOWN_DC2H_HSD_Initial_Value_reg);
	dc2h_vsd_initial_value_reg.regValue  = rtd_inl(DC2H_SCALEDOWN_DC2H_VSD_Initial_Value_reg);

	#define TMPMUL  (16)

	//o============ H scale-down=============o
	if (pdc2h_hdl->in_size.nWidth > pdc2h_hdl->out_size.nWidth)
	{
		// o-------calculate scaledown ratio to select one of different bandwith filters.--------o
		if ( pdc2h_hdl->out_size.nWidth == 0 ) {
		rtd_pr_vt_debug("output width = 0 !!!\n");
		SDRatio = 0;
		} else {
		SDRatio = (pdc2h_hdl->in_size.nWidth*TMPMUL) / pdc2h_hdl->out_size.nWidth;
		}

		//UltraZoom_Printf("CSW SDRatio number=%d\n",SDRatio);

		if(SDRatio <= ((TMPMUL*3)/2))  //<1.5 sharp, wider bw
			SDFilter = 2;
		else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) )  // Mid
			SDFilter = 1;
		else    // blurest, narrow bw
			SDFilter = 0;

		//o---------------- fill coefficients into access port--------------------o
		coef_pt = tScaleDown_COEF_TAB[SDFilter];

		dc2h_hsd_ctrl0_reg.h_y_table_sel = 0;  // TAB1
		dc2h_hsd_ctrl0_reg.h_c_table_sel = 0;  // TAB1

		for (i=0; i<16; i++)
		{
			tmp_data = ((unsigned int)(*coef_pt++)<<16);
			tmp_data += (unsigned int)(*coef_pt++);
			rtd_outl(DC2H_SCALEDOWN_DC2H_UZD_FIR_Coef_Tab1_C0_reg + i*4, tmp_data);
		}
	}

	//o============ V scale-down=============o
	if (pdc2h_hdl->in_size.nLength > pdc2h_hdl->out_size.nLength)
	{
		// o-------calculate scaledown ratio to select one of different bandwith filters.--------o
		//jeffrey 961231
		if ( pdc2h_hdl->out_size.nLength == 0 ) {
		SDRatio = 0;
		} else {
		SDRatio = (pdc2h_hdl->in_size.nLength*TMPMUL) /pdc2h_hdl->out_size.nLength;
		}

		//UltraZoom_Printf("CSW SDRatio number=%d\n",SDRatio);

		if(SDRatio <= ((TMPMUL*3)/2))  //<1.5 sharp, wider bw
			SDFilter = 2;
		else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) )  // Mid
			SDFilter = 1;
		else    // blurest, narrow bw
			SDFilter = 0;

		rtd_pr_vt_debug("filter number=%d\n",SDFilter);

		//o---------------- fill coefficients into access port--------------------o
		 coef_pt = tScaleDown_COEF_TAB[SDFilter];
		//coef_pt = &(ScaleDown_COEF_TAB.FIR_Coef_Table[SD_V_Coeff_Sel[SDFilter]][0]);

		dc2h_vsd_ctrl0_reg.v_y_table_sel = 1;  // TAB2
		dc2h_vsd_ctrl0_reg.v_c_table_sel = 1;  // TAB2

		for (i=0; i<16; i++)
		{
			tmp_data = ((unsigned int)(*coef_pt++)<<16);
			tmp_data += (unsigned int)(*coef_pt++);
			rtd_outl(DC2H_SCALEDOWN_DC2H_UZD_FIR_Coef_Tab2_C0_reg + i*4, tmp_data);
		}
	} else {
		//no need  V scale-down, use bypass filter
	}

	// Decide Output data format for scale down
	//============H Scale-down============
	if (pdc2h_hdl->in_size.nWidth > pdc2h_hdl->out_size.nWidth)
	{    // H scale-down
		Hini = 0x80;//0xff;//0x78;
		dc2h_hsd_initial_value_reg.hor_ini = Hini;
		dc2h_hsd_initial_value_reg.hor_ini_int= 1;
		a = 5;

		if(pdc2h_hdl->panorama)
		{// CSW+ 0960830 Non-linear SD
			S1 = (pdc2h_hdl->out_size.nWidth) >> 2;
			S2 = (pdc2h_hdl->out_size.nWidth) - S1*2;
			//=vv==old from TONY, if no problem, use this=vv==
			nFactor = (unsigned int)(((pdc2h_hdl->in_size.nWidth)<<21) - ((unsigned int)Hini<<13));
			//=vv==Thur debugged=vv==
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
				if(pdc2h_hdl->out_size.nWidth != 0)
					nFactor = (unsigned int)(((pdc2h_hdl->in_size.nWidth)<<21)) / (pdc2h_hdl->out_size.nWidth);
				nFactor = SHR(nFactor + 1, 1); //rounding
				rtd_pr_vt_debug("PANORAMA2 nFactor=%x, input Wid=%d, Out Wid=%d\n",nFactor, pdc2h_hdl->in_size.nWidth, pdc2h_hdl->out_size.nWidth);
				S1 = 0;
				S2 = pdc2h_hdl->out_size.nWidth;
				D = 0;
			}
        	}else{
			if ( pdc2h_hdl->out_size.nWidth == 0 ) {
				rtd_pr_vt_debug("output width = 0 !!!\n");
				nFactor = 0;
			}else if(pdc2h_hdl->in_size.nWidth>4095){
				//nFactor = (unsigned int)((((pdc2h_hdl->in_size.nWidth-1)<<19) / (pdc2h_hdl->out_size.nWidth-1))<<1);
				nFactor = (unsigned int)((((pdc2h_hdl->in_size.nWidth)<<19) / (pdc2h_hdl->out_size.nWidth))<<1);
			}else if(pdc2h_hdl->in_size.nWidth>2047){
				//nFactor = (unsigned int)(((pdc2h_hdl->in_size.nWidth-1)<<20) / (pdc2h_hdl->out_size.nWidth-1));
				nFactor = (unsigned int)(((pdc2h_hdl->in_size.nWidth)<<20) / (pdc2h_hdl->out_size.nWidth));
			}else {
				//nFactor = (unsigned int)(((pdc2h_hdl->in_size.nWidth-1)<<21) / (pdc2h_hdl->out_size.nWidth-1));
				nFactor = (unsigned int)(((pdc2h_hdl->in_size.nWidth)<<21) / (pdc2h_hdl->out_size.nWidth));
				nFactor = SHR(nFactor + 1, 1); //rounding
			}

			rtd_pr_vt_debug("nFactor=%d, input Wid=%d, Out Wid=%d\n",nFactor, pdc2h_hdl->in_size.nWidth, pdc2h_hdl->out_size.nWidth);
			S1 = 0;
			S2 = pdc2h_hdl->out_size.nWidth;
			D = 0;
		}
	}else {
		nFactor = 0x100000;
		S1 = 0;
		S2 = pdc2h_hdl->out_size.nWidth;
		D = 0;
	}

	dc2h_hsd_scale_hor_factor_reg.hor_fac = nFactor;
	//dc2h_hsd_hor_segment_reg.nl_seg1 = S1;
	dc2h_hsd_hor_segment_reg.nl_seg2 = S2;
	//dc2h_hsd_hor_delta1_reg.nl_d1 = D;
	//UltraZoom_Printf("reg_seg1=%x, reg_Seg_all=%x\n",dc2h_hsd_hor_segment_reg.nl_seg1, dc2h_hsd_hor_segment_reg.regValue);
	rtd_pr_vt_debug("nFactor=%x, input_Wid=%d, Output_Wid=%d\n",nFactor, pdc2h_hdl->in_size.nWidth, pdc2h_hdl->out_size.nWidth);

	//================V Scale-down=================
	if (pdc2h_hdl->in_size.nLength > pdc2h_hdl->out_size.nLength) {    // V scale-down
		Vini = 0xff;//0x78;
		dc2h_vsd_initial_value_reg.ver_ini = Vini;
		if ( pdc2h_hdl->out_size.nLength == 0 ) {
			rtd_pr_vt_debug("output length = 0 !!!\n");
			nFactor = 0;
		} else {
			//frank@1018 sync with pacific to solve 3d vide 1920X2205 scaler down fail
			//nFactor = (unsigned int)(((ptInSize->nLength)<<21) / (ptOutSize->nLength));
			//nFactor = (unsigned int)(((unsigned long long)(pdc2h_hdl->in_size.nLength)<<21) / (pdc2h_hdl->out_size.nLength));
			tmpLength = ((unsigned long long)(pdc2h_hdl->in_size.nLength)<<21);
			do_div(tmpLength, (pdc2h_hdl->out_size.nLength));
			nFactor = tmpLength;
		}
		rtd_pr_vt_debug("Ch1 Ver: CAP=%d, Disp=%d, factor=%d\n", pdc2h_hdl->in_size.nLength, pdc2h_hdl->out_size.nLength, nFactor);
		nFactor = SHR(nFactor + 1, 1); //rounding
	} else {
		nFactor = 0x100000;
	}

	dc2h_vsd_scale_ver_factor_reg.ver_fac = nFactor;

	rtd_outl(DC2H_SCALEDOWN_DC2H_HSD_Ctrl0_reg, dc2h_hsd_ctrl0_reg.regValue);
	rtd_outl(DC2H_SCALEDOWN_DC2H_VSD_Ctrl0_reg, dc2h_vsd_ctrl0_reg.regValue);
	rtd_outl(DC2H_SCALEDOWN_DC2H_HSD_Scale_Hor_Factor_reg, dc2h_hsd_scale_hor_factor_reg.regValue);
	rtd_outl(DC2H_SCALEDOWN_DC2H_VSD_Scale_Ver_Factor_reg, dc2h_vsd_scale_ver_factor_reg.regValue);
	rtd_outl(DC2H_SCALEDOWN_DC2H_HSD_Hor_Segment_reg, dc2h_hsd_hor_segment_reg.regValue);
	//rtd_outl(DC2H_SCALEDOWN_DC2H_HSD_Hor_Delta1_reg, dc2h_hsd_hor_delta1_reg.regValue);
	rtd_outl(DC2H_SCALEDOWN_DC2H_HSD_Initial_Value_reg, dc2h_hsd_initial_value_reg.regValue);
	rtd_outl(DC2H_SCALEDOWN_DC2H_VSD_Initial_Value_reg, dc2h_vsd_initial_value_reg.regValue);
}


void drvif_color_ultrazoom_config_dc2h_scaling_down(DC2H_HANDLER *pdc2h_hdl)
{
	// Scale down setup for Channel1
	dc2h_scaledown_dc2h_hsd_ctrl0_RBUS dc2h_hsd_ctrl0_reg;
	dc2h_scaledown_dc2h_vsd_ctrl0_RBUS dc2h_vsd_ctrl0_reg;
	dc2h_scaledown_dc2h_444to422_ctrl_RBUS dc2h_scaledown_dc2h_444to422_ctrl_reg;

	dc2h_scaledown_dc2h_444to422_ctrl_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H_444to422_CTRL_reg);
	//if(pdc2h_hdl->dc2h_in_sel==_MAIN_UZU)/*mux uzu*/
		//dc2h_scaledown_dc2h_444to422_ctrl_reg.cbcr_swap_en =1;
	//else
		dc2h_scaledown_dc2h_444to422_ctrl_reg.cbcr_swap_en =0;
	rtd_outl(DC2H_SCALEDOWN_DC2H_444to422_CTRL_reg, dc2h_scaledown_dc2h_444to422_ctrl_reg.regValue);  //used record freeze buffer


	dc2h_hsd_ctrl0_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H_HSD_Ctrl0_reg);
	dc2h_hsd_ctrl0_reg.h_zoom_en = (pdc2h_hdl->in_size.nWidth > pdc2h_hdl->out_size.nWidth);
	rtd_outl(DC2H_SCALEDOWN_DC2H_HSD_Ctrl0_reg, dc2h_hsd_ctrl0_reg.regValue);

	dc2h_vsd_ctrl0_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H_VSD_Ctrl0_reg);
	if(pdc2h_hdl->in_size.nLength > pdc2h_hdl->out_size.nLength){
		dc2h_vsd_ctrl0_reg.v_zoom_en = 1;
		dc2h_vsd_ctrl0_reg.buffer_mode = 2;
	}else{
		dc2h_vsd_ctrl0_reg.v_zoom_en = 0;
		dc2h_vsd_ctrl0_reg.buffer_mode = 0;
	}
	if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21)){
		dc2h_vsd_ctrl0_reg.sort_fmt = 0;
	} else {
		dc2h_vsd_ctrl0_reg.sort_fmt = 1;
	}
	rtd_outl(DC2H_SCALEDOWN_DC2H_VSD_Ctrl0_reg, dc2h_vsd_ctrl0_reg.regValue);

	drvif_color_ultrazoom_set_dc2h_scale_down(pdc2h_hdl);
}

void drvif_dc2h_input_overscan_config(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_RBUS dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_Reg;
	dc2h_rgb2yuv_dc2h_overscan_ctrl0_RBUS dc2h_rgb2yuv_dc2h_overscan_ctrl0_Reg;
	dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl1_RBUS dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl1_Reg;

	dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_Reg.regValue = rtd_inl(DC2H_RGB2YUV_DC2H_3DMaskTo2D_Ctrl_reg);
	dc2h_rgb2yuv_dc2h_overscan_ctrl0_Reg.regValue = rtd_inl(DC2H_RGB2YUV_DC2H_OVERSCAN_Ctrl0_reg);
	dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl1_Reg.regValue = rtd_inl(DC2H_RGB2YUV_DC2H_3DMaskTo2D_Ctrl1_reg);

	dc2h_rgb2yuv_dc2h_overscan_ctrl0_Reg.dc2h_overscan_left = x+1;
	dc2h_rgb2yuv_dc2h_overscan_ctrl0_Reg.dc2h_overscan_right = x+w;
	rtd_outl(DC2H_RGB2YUV_DC2H_OVERSCAN_Ctrl0_reg, dc2h_rgb2yuv_dc2h_overscan_ctrl0_Reg.regValue);

	dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl1_Reg.dc2h_overscan_top = y;
	dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl1_Reg.dc2h_overscan_bottom = y+h-1;
	rtd_outl(DC2H_RGB2YUV_DC2H_3DMaskTo2D_Ctrl1_reg, dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl1_Reg.regValue);

	dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_Reg.dc2h_3dmaskto2d_3dformat = 4;/*overscan*/
    if( pdc2h_hdl->dc2h_in_sel == _Idomain_Sdnr_input || pdc2h_hdl->dc2h_in_sel == _Sub_Sdnr_input)
    {
        dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_Reg.dc2h_3dmaskto2d_en = 0;
    }
    else
    {
        dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_Reg.dc2h_3dmaskto2d_en = 1;
    }
    rtd_outl(DC2H_RGB2YUV_DC2H_3DMaskTo2D_Ctrl_reg, dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_Reg.regValue);
}

void drvif_vi_config(unsigned int nWidth, unsigned int nLength, unsigned int data_format)
{
#define LINE_STEP_MAX 0x1FFFFFFF
	unsigned int LineSize;
	unsigned int width_16bytes_align = 0;  //128bits align
    unsigned int timeoutcount = 100;
    unsigned int st_time = 0, end_time = 0;
    dc2h_vi_dc2h_v1_output_fmt_RBUS dc2h_v1_output_fmt_reg;//DC2H_RGB2YUV_DC2H_V1_OUTPUT_FMT_VADDR
    dc2h_vi_dc2h_cwvh1_RBUS dc2h_cwvh1_reg;//w,h //DC2H_RGB2YUV_DC2H_CWVH1_VADDR
    dc2h_vi_dc2h_cwvh1_2_RBUS dc2h_cwvh1_2_reg;
	dc2h_vi_dc2h_dmactl_RBUS vi_dmactl_reg;//dmaen	//I3DDMA_VI_IDMA_DMACTL_VADDR
	dc2h_vi_dc2h_vi_addcmd_transform_RBUS dc2h_vi_dc2h_vi_addcmd_transform_reg;
	dc2h_dma_dc2h_seq_interrupt_enable_RBUS dc2h_dma_dc2h_seq_interrupt_enable_Reg;
	dc2h_vi_dc2h_y1buf_RBUS dc2h_vi_dc2h_y1buf_Reg;
	dc2h_vi_dc2h_c1buf_RBUS dc2h_vi_dc2h_c1buf_Reg;
	dc2h_vi_dc2h_vi_c_line_step_RBUS dc2h_vi_dc2h_vi_c_line_step_Reg;
    dc2h_vi_dc2h_vi_doublebuffer_RBUS dc2h_vi_dc2h_vi_doublebuffer_Reg;
    dc2h_vi_dc2h_inten_RBUS dc2h_vi_dc2h_inten_Reg;
    dc2h_vi_dc2h_intst_RBUS dc2h_vi_dc2h_intst_Reg;

    dc2h_v1_output_fmt_reg.regValue = rtd_inl(DC2H_VI_DC2H_V1_OUTPUT_FMT_reg);
	dc2h_v1_output_fmt_reg.data_format = data_format;/*0:420	1:422*/
	if(get_VT_Pixel_Format() == VT_CAP_NV12 || get_VT_Pixel_Format() == VT_CAP_NV16)
	{
		dc2h_v1_output_fmt_reg.swap_1byte = 0;
		dc2h_v1_output_fmt_reg.swap_2byte = 0;
		dc2h_v1_output_fmt_reg.swap_4byte = 0;
		dc2h_v1_output_fmt_reg.swap_8byte = 0;

	}else if(get_VT_Pixel_Format() == VT_CAP_NV21){
		dc2h_v1_output_fmt_reg.swap_1byte = 1;
		dc2h_v1_output_fmt_reg.swap_2byte = 0;
		dc2h_v1_output_fmt_reg.swap_4byte = 0;
		dc2h_v1_output_fmt_reg.swap_8byte = 0;
	}
    rtd_outl(DC2H_VI_DC2H_V1_OUTPUT_FMT_reg, dc2h_v1_output_fmt_reg.regValue);

	width_16bytes_align = drvif_memory_get_data_align(nWidth,16);

    dc2h_cwvh1_reg.regValue = rtd_inl(DC2H_VI_DC2H_CWVH1_reg);
	dc2h_cwvh1_reg.hsize = nWidth;
    dc2h_cwvh1_reg.vsize = nLength;
    rtd_outl(DC2H_VI_DC2H_CWVH1_reg, dc2h_cwvh1_reg.regValue);

   	dc2h_cwvh1_2_reg.regValue = rtd_inl(DC2H_VI_DC2H_CWVH1_2_reg);
    dc2h_cwvh1_2_reg.vsize_c = nLength >> (data_format == 1? 0: 1);
    rtd_outl(DC2H_VI_DC2H_CWVH1_2_reg, dc2h_cwvh1_2_reg.regValue);

    if(get_vt_bufwidth_as_linestep() == TRUE){
        LineSize = vt_cap_frame_max_width * 8 / 64;
    }else{
        LineSize = width_16bytes_align * 8 / 64;
    }

	if(LineSize % 2)
		LineSize += 1;

	if(Get_PANEL_VFLIP_ENABLE())
		LineSize = (LINE_STEP_MAX - LineSize + 1); //2's complement of line step,means nagative line step

	dc2h_vi_dc2h_vi_c_line_step_Reg.regValue = rtd_inl(DC2H_VI_DC2H_vi_c_line_step_reg);
	dc2h_vi_dc2h_vi_c_line_step_Reg.c_line_step = LineSize;
	rtd_outl(DC2H_VI_DC2H_vi_c_line_step_reg, dc2h_vi_dc2h_vi_c_line_step_Reg.regValue);

	dc2h_vi_dc2h_vi_addcmd_transform_reg.regValue = rtd_inl(DC2H_VI_DC2H_vi_addcmd_transform_reg);
	dc2h_vi_dc2h_vi_addcmd_transform_reg.line_step = LineSize;
	dc2h_vi_dc2h_vi_addcmd_transform_reg.vi_addcmd_trans_en = 1;
	rtd_outl(DC2H_VI_DC2H_vi_addcmd_transform_reg, dc2h_vi_dc2h_vi_addcmd_transform_reg.regValue);

    /*write 1 clear interrupt flag */
    dc2h_vi_dc2h_intst_Reg.regValue = IoReg_Read32(DC2H_VI_DC2H_INTST_reg);
    dc2h_vi_dc2h_intst_Reg.video_in_last_wr_y_flag = 1;
    dc2h_vi_dc2h_intst_Reg.video_in_last_wr_c_flag = 1;
    dc2h_vi_dc2h_intst_Reg.vsints1 = 1;
    IoReg_Write32(DC2H_VI_DC2H_INTST_reg, dc2h_vi_dc2h_intst_Reg.regValue);

	if (get_vt_VtSwBufferMode()) {
		dc2h_dma_dc2h_seq_interrupt_enable_Reg.regValue = rtd_inl(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg);
		dc2h_dma_dc2h_seq_interrupt_enable_Reg.vs_rising_ie = 1;
        dc2h_vi_dc2h_inten_Reg.regValue = IoReg_Read32(DC2H_VI_DC2H_INTEN_reg);
        dc2h_vi_dc2h_inten_Reg.video_in_last_wr_c_flag_ie = 1;
	}

    st_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg)/90;
    dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue = IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg);
    dc2h_vi_dc2h_vi_doublebuffer_Reg.vi_db_en = 1;
    IoReg_Write32(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue);
    //dc2h_wait_porch();

	vi_dmactl_reg.regValue = rtd_inl(DC2H_VI_DC2H_DMACTL_reg);
	vi_dmactl_reg.dmaen1 = 1;
	vi_dmactl_reg.seq_blk_sel =1; //0: YV16 or RGB888 1: NV12 or NV16
	vi_dmactl_reg.bstlen = 7;//merlin4 run block mode 96B align
	rtd_outl(DC2H_VI_DC2H_DMACTL_reg, vi_dmactl_reg.regValue);

	dc2h_vi_dc2h_y1buf_Reg.regValue = rtd_inl(DC2H_VI_DC2H_Y1BUF_reg);
	dc2h_vi_dc2h_y1buf_Reg.y_water_lvl = 120; //burst write 120x64bits
	rtd_outl(DC2H_VI_DC2H_Y1BUF_reg, dc2h_vi_dc2h_y1buf_Reg.regValue);

	dc2h_vi_dc2h_c1buf_Reg.regValue = rtd_inl(DC2H_VI_DC2H_C1BUF_reg);
	dc2h_vi_dc2h_c1buf_Reg.c_water_lvl = 120; //burst write 120x64bits
	rtd_outl(DC2H_VI_DC2H_C1BUF_reg, dc2h_vi_dc2h_c1buf_Reg.regValue);

    dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue = IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg);
    dc2h_vi_dc2h_vi_doublebuffer_Reg.vi_db_apply = 1;
    IoReg_Write32(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue);

    //wait db apply clear by the vs_rise
    do{
        if(DC2H_VI_DC2H_vi_doublebuffer_get_vi_db_apply(IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg))){
            timeoutcount--;
            msleep(0);
        }
        else
            break;
    }while(timeoutcount);

    if(timeoutcount == 0)
    {
        rtd_pr_vt_notice("[VT]wait vi double apply clear timeout\n");
    }
    end_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg)/90;
    rtd_pr_vt_notice("[VT]vi db apply clear timecost=%d ms\n", (end_time - st_time));

	if (get_vt_VtSwBufferMode()) {
        IoReg_Write32(DC2H_VI_DC2H_INTEN_reg, dc2h_vi_dc2h_inten_Reg.regValue);
        IoReg_Write32(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg, dc2h_dma_dc2h_seq_interrupt_enable_Reg.regValue);
	}

    dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue = IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg);
    dc2h_vi_dc2h_vi_doublebuffer_Reg.vi_db_en = 0;
    IoReg_Write32(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue);

    rtd_pr_vt_notice("[VT]enable dc2h vi capture,dmaen=%d,db_en=%d,ab_apply=%d\n",\
    DC2H_VI_DC2H_DMACTL_get_dmaen1(IoReg_Read32(DC2H_VI_DC2H_DMACTL_reg)), DC2H_VI_DC2H_vi_doublebuffer_get_vi_db_en(IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg)), DC2H_VI_DC2H_vi_doublebuffer_get_vi_db_apply(IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg)));
}

void drvif_dc2h_seq_config(DC2H_HANDLER *pdc2h_hdl)
{
#define LINE_STEP_MAX 0x1FFFFFFF

	unsigned int num, rem;
	unsigned int lineunits, len;
	unsigned int seq_vflip_lineunits;//Vflip enable for Seq_Mode, use the vi_line_step
	unsigned int width_16bytes_align = 0;  //128bits align
	dc2h_dma_dc2h_seq_mode_ctrl1_RBUS dc2h_seq_mode_ctrl1_reg;
	dc2h_dma_dc2h_seq_mode_ctrl2_RBUS dc2h_seq_mode_ctrl2_reg;
	dc2h_dma_dc2h_seq_interrupt_enable_RBUS dc2h_dma_dc2h_seq_interrupt_enable_Reg;
	dc2h_dma_dc2h_cap_ctl0_RBUS dc2h_cap_ctl0_reg;
	dc2h_dma_dc2h_cap_ctl1_RBUS dc2h_cap_ctl1_reg;
	//dc2h_cap_l1_start_RBUS dc2h_cap_l1_start_reg;
	//dc2h_cap_l2_start_RBUS dc2h_cap_l2_start_reg;
	//dc2h_cap_l3_start_RBUS dc2h_cap_l3_start_reg;
	dc2h_dma_dc2h_seq_byte_channel_swap_RBUS dc2h_seq_byte_channel_swap_reg;
    dc2h_vi_dc2h_vi_addcmd_transform_RBUS dc2h_vi_dc2h_vi_addcmd_transform_reg;

	dc2h_vi_dc2h_dmactl_RBUS vi_dmactl_reg;//dmaen
	vi_dmactl_reg.regValue = rtd_inl(DC2H_VI_DC2H_DMACTL_reg);
	//vi_dmactl_reg.dmaen1 = 1;
	vi_dmactl_reg.seq_blk_sel = 0;
	rtd_outl(DC2H_VI_DC2H_DMACTL_reg, vi_dmactl_reg.regValue);

	dc2h_seq_mode_ctrl1_reg.regValue = rtd_inl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg);
	dc2h_seq_mode_ctrl1_reg.cap_en = 0;
	rtd_outl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg, dc2h_seq_mode_ctrl1_reg.regValue);

	if (pdc2h_hdl->dc2h_en == FALSE)
		return;

    if(get_vt_bufwidth_as_linestep() == TRUE)
        width_16bytes_align = vt_cap_frame_max_width;
    else
	{
		width_16bytes_align = drvif_memory_get_data_align(pdc2h_hdl->out_size.nWidth, 16);
	}

    dc2h_seq_mode_ctrl2_reg.regValue = rtd_inl(DC2H_DMA_dc2h_Seq_mode_CTRL2_reg);
	dc2h_seq_mode_ctrl2_reg.hact = pdc2h_hdl->out_size.nWidth;
	dc2h_seq_mode_ctrl2_reg.vact = pdc2h_hdl->out_size.nLength;
	rtd_outl(DC2H_DMA_dc2h_Seq_mode_CTRL2_reg, dc2h_seq_mode_ctrl2_reg.regValue);

	if(get_VT_Pixel_Format() == VT_CAP_ARGB8888 || get_VT_Pixel_Format() == VT_CAP_RGBA8888 || get_VT_Pixel_Format() == VT_CAP_ABGR8888) {
        lineunits = (pdc2h_hdl->out_size.nWidth * 8 * 4 + 127) / 128;
        seq_vflip_lineunits = (width_16bytes_align * 8 * 4) / 64;
	} else {
        lineunits = (pdc2h_hdl->out_size.nWidth * 8 * 3 + 127) / 128;
        seq_vflip_lineunits = (width_16bytes_align * 8 * 3) / 64;
	}

	len = 32;
	num = (unsigned int)(lineunits / len);
	rem = (unsigned int)(lineunits % len);

	if (rem == 0) { // remainder is not allowed to be zero
		rem = len;
		num -= 1;
	}

	dc2h_cap_ctl0_reg.regValue = rtd_inl(DC2H_DMA_dc2h_Cap_CTL0_reg);
	dc2h_cap_ctl0_reg.bit_sel = 0;
	dc2h_cap_ctl0_reg.pixel_encoding = 0;
	//dc2h_cap_ctl0_reg.triple_buf_en = 1;
	if(get_vt_VtBufferNum() == 1) {
		dc2h_cap_ctl0_reg.triple_buf_en = 0;
		dc2h_cap_ctl0_reg.auto_block_sel_en = 0;
		//dc2h_cap_ctl0_reg.cap_block_fw = 0;
	} else {
		if(get_vt_VtCaptureVDC()) {
			dc2h_cap_ctl0_reg.triple_buf_en = 1;
			dc2h_cap_ctl0_reg.auto_block_sel_en = 1;
		} else {
			if (get_vt_VtSwBufferMode()) {
				dc2h_cap_ctl0_reg.triple_buf_en = 0;
				dc2h_cap_ctl0_reg.auto_block_sel_en = 0;
			} else {
				dc2h_cap_ctl0_reg.triple_buf_en = 0;
				dc2h_cap_ctl0_reg.auto_block_sel_en = 1;
			}
		}
	}
	dc2h_cap_ctl0_reg.cap_write_len = CAP_LEN;
	dc2h_cap_ctl0_reg.cap_write_rem = rem;
	rtd_outl(DC2H_DMA_dc2h_Cap_CTL0_reg, dc2h_cap_ctl0_reg.regValue);

	dc2h_cap_ctl1_reg.regValue = rtd_inl(DC2H_DMA_dc2h_Cap_CTL1_reg);
	dc2h_cap_ctl1_reg.cap_water_lv =CAP_LEN ;
	dc2h_cap_ctl1_reg.cap_write_num = num;
	rtd_outl(DC2H_DMA_dc2h_Cap_CTL1_reg, dc2h_cap_ctl1_reg.regValue);

	//dc2h_seq_mode_ctrl1_reg.regValue = rtd_inl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg);
	//dc2h_seq_mode_ctrl1_reg.cap_en = 1;
	//rtd_outl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg, dc2h_seq_mode_ctrl1_reg.regValue);

	dc2h_seq_byte_channel_swap_reg.regValue = rtd_inl(DC2H_DMA_dc2h_seq_byte_channel_swap_reg);
	dc2h_seq_byte_channel_swap_reg.dma_1byte_swap = 1;
	dc2h_seq_byte_channel_swap_reg.dma_2byte_swap = 1;
	dc2h_seq_byte_channel_swap_reg.dma_4byte_swap = 1;
	dc2h_seq_byte_channel_swap_reg.dma_8byte_swap = 1;
	if(get_VT_Pixel_Format() == VT_CAP_ARGB8888)
		dc2h_seq_byte_channel_swap_reg.dma_channel_swap = 0;
	else
		dc2h_seq_byte_channel_swap_reg.dma_channel_swap = 1;
	rtd_outl(DC2H_DMA_dc2h_seq_byte_channel_swap_reg, dc2h_seq_byte_channel_swap_reg.regValue);

	dc2h_seq_mode_ctrl1_reg.regValue = rtd_inl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg);
	dc2h_seq_mode_ctrl1_reg.cap_en = 1;

	if((get_VT_Pixel_Format() == VT_CAP_ABGR8888) || (get_VT_Pixel_Format() == VT_CAP_ARGB8888) || (get_VT_Pixel_Format() == VT_CAP_RGBA8888)){
		dc2h_seq_mode_ctrl1_reg.argb_dummy_data = 0xFF;
		dc2h_seq_mode_ctrl1_reg.rgb_to_argb_en = 1;
		dc2h_seq_mode_ctrl1_reg.argb_swap = 1;
        if(get_VT_Pixel_Format() == VT_CAP_RGBA8888)
            dc2h_seq_mode_ctrl1_reg.argb_swap = 0;
    }
    else{
        dc2h_seq_mode_ctrl1_reg.rgb_to_argb_en = 0;
    }

    if(Get_PANEL_VFLIP_ENABLE()){
        dc2h_seq_mode_ctrl1_reg.vflip_en = 1;//seq_mode vflip,use vi_line_step @8029D74
        dc2h_vi_dc2h_vi_addcmd_transform_reg.regValue = rtd_inl(DC2H_VI_DC2H_vi_addcmd_transform_reg);
        dc2h_vi_dc2h_vi_addcmd_transform_reg.line_step = (LINE_STEP_MAX - seq_vflip_lineunits + 1); //2's complement of line step,means nagative line step
        rtd_outl(DC2H_VI_DC2H_vi_addcmd_transform_reg, dc2h_vi_dc2h_vi_addcmd_transform_reg.regValue);
    }
    else{
        //dc2h_seq_mode_ctrl1_reg.vflip_en = 0;
        dc2h_seq_mode_ctrl1_reg.vflip_en = 1; //seq_mode enable line step
        dc2h_vi_dc2h_vi_addcmd_transform_reg.regValue = rtd_inl(DC2H_VI_DC2H_vi_addcmd_transform_reg);
        dc2h_vi_dc2h_vi_addcmd_transform_reg.line_step = seq_vflip_lineunits; //positive line step
        rtd_outl(DC2H_VI_DC2H_vi_addcmd_transform_reg, dc2h_vi_dc2h_vi_addcmd_transform_reg.regValue);
    }

	if (get_vt_VtSwBufferMode()) {
		dc2h_dma_dc2h_seq_interrupt_enable_Reg.regValue = rtd_inl(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg);
		dc2h_dma_dc2h_seq_interrupt_enable_Reg.vs_rising_ie = 1;
	}
	dc2h_wait_porch();
	rtd_outl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg, dc2h_seq_mode_ctrl1_reg.regValue);
	if (get_vt_VtSwBufferMode()) {
		rtd_outl(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg, dc2h_dma_dc2h_seq_interrupt_enable_Reg.regValue);
	}
	return;
}

void set_dc2h_capture_sdnr_in_clock(UINT8 enable)
{
    if(enable == TRUE){
		IoReg_SetBits(SCALEDOWN_CTS_FIFO_GAP_DC2H_reg, SCALEDOWN_CTS_FIFO_GAP_DC2H_s1_r_dc2h_clk_en_mask);
    }else{
        IoReg_ClearBits(SCALEDOWN_CTS_FIFO_GAP_DC2H_reg, SCALEDOWN_CTS_FIFO_GAP_DC2H_s1_r_dc2h_clk_en_mask);
    }
}

//according to the dumplocation (in_position)
// calculate the dc2h input size (in_size) and dc2h in sel bit value (dc2h_in_sel)
void set_DC2H_rgb2yuv_ctrl_reg_refer_dump_location(KADP_VT_DUMP_LOCATION_TYPE_T in_position, SIZE *in_size, unsigned char *dc2h_in_sel)
{
	ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;
	dc2h_rgb2yuv_dc2h_rgb2yuv_ctrl_RBUS  dc2h_rgb2yuv_ctrl_reg;

	ppoverlay_main_den_h_start_end_RBUS main_den_h_start_end_Reg;
	ppoverlay_main_den_v_start_end_RBUS main_den_v_start_end_Reg;
	ppoverlay_main_active_h_start_end_RBUS main_active_h_start_end_Reg;
	ppoverlay_main_active_v_start_end_RBUS main_active_v_start_end_Reg;

	main_den_h_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_DEN_H_Start_End_reg);
	main_den_v_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_DEN_V_Start_End_reg);
	main_active_h_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_H_Start_End_reg);
	main_active_v_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_V_Start_End_reg);
	ppoverlay_uzudtg_control1_reg.regValue = rtd_inl(PPOVERLAY_uzudtg_control1_reg);

	dc2h_rgb2yuv_ctrl_reg.regValue = rtd_inl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg); //9c00
	dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en = (get_mach_type() == MACH_ARCH_RTK2885P) ? 0 : 2; //default value

	if(in_position == KADP_VT_SCALER_OUTPUT)
	{
		*dc2h_in_sel = _TWO_SECOND_UZU;
		in_size->nWidth = (main_active_h_start_end_Reg.mh_act_end - main_active_h_start_end_Reg.mh_act_sta);
		in_size->nLength = (main_active_v_start_end_Reg.mv_act_end - main_active_v_start_end_Reg.mv_act_sta);
		set_dc2h_input_hact(in_size->nWidth);

		if((get_panel_pixel_mode() > PANEL_1_PIXEL) && (get_mach_type() == MACH_ARCH_RTK2885P)){	
			if(TRUE == force_enable_two_step_uzu()){ //4k120 source
				if(is_scale_down_case(in_size, &(pdc2h_hdl->out_size)) == FALSE){
					if((get_VT_Pixel_Format() == VT_CAP_NV12) || (get_VT_Pixel_Format() == VT_CAP_NV21))  //4k2k120 NV12
						dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en = 2;
					else
						rtd_pr_vt_emerg("[error] only support 4K2K120 nv12:%d\n",__LINE__);
				}else{
					dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en = 1; /* drop 1 pixel */
					in_size->nWidth = (in_size->nWidth >> 1);
				}
			}else{
				dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en = 0;
			}
		}
		rtd_outl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg, dc2h_rgb2yuv_ctrl_reg.regValue);
		rtd_pr_vt_notice("line:%d,dc2h_drop_en=%d\n",__LINE__, dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en);
	}
	else if(in_position == KADP_VT_DISPLAY_OUTPUT)
	{
		in_size->nWidth = (main_den_h_start_end_Reg.mh_den_end - main_den_h_start_end_Reg.mh_den_sta);
		in_size->nLength = (main_den_v_start_end_Reg.mv_den_end - main_den_v_start_end_Reg.mv_den_sta);
		set_dc2h_input_hact(in_size->nWidth);
		*dc2h_in_sel = _Memc_Mux_Input;

		if((get_panel_pixel_mode() > PANEL_1_PIXEL) && (get_mach_type() == MACH_ARCH_RTK2885P)){
			if(TRUE == force_enable_two_step_uzu()){ //4k120 source
				if(is_scale_down_case(in_size, &(pdc2h_hdl->out_size)) == FALSE){
					if((get_VT_Pixel_Format() == VT_CAP_NV12) || (get_VT_Pixel_Format() == VT_CAP_NV21)){
						dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en = 2; /* only for 4K2K120 nv12 can use */
					}else{
						rtd_pr_vt_emerg("[error] only support 4K2K120 nv12:%d\n",__LINE__);
					}
				}else{
					dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en = 1;
					in_size->nWidth = (in_size->nWidth >> 1);
				}
			}else{ //memc input location on 120 panel,is non 4k2k120 for dc2h
				dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en = 0;
			}
		}
		rtd_outl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg, dc2h_rgb2yuv_ctrl_reg.regValue);
		rtd_pr_vt_notice("line:%d,dc2h_drop_en=%d\n",__LINE__, dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en);
	}
	else if(in_position == KADP_VT_OSDVIDEO_OUTPUT)
	{
		in_size->nWidth = (main_den_h_start_end_Reg.mh_den_end - main_den_h_start_end_Reg.mh_den_sta);
		in_size->nLength = (main_den_v_start_end_Reg.mv_den_end - main_den_v_start_end_Reg.mv_den_sta);
		set_dc2h_input_hact(in_size->nWidth);
		*dc2h_in_sel = _OSD_MIXER;

		if(get_vt_crop_enable() == TRUE){
			in_size->nWidth = curCapInfo.crop_size.w;
			in_size->nLength = curCapInfo.crop_size.h;
			rtd_pr_vt_emerg("change dc2h input size to crop size,input wxh=%dx%d\n", in_size->nWidth, in_size->nLength);
		}
		if((get_panel_pixel_mode() > PANEL_1_PIXEL) && (get_mach_type() == MACH_ARCH_RTK2885P)){
			if(is_scale_down_case(in_size, &(pdc2h_hdl->out_size)) == FALSE){ //4k120 case and not scale down			
				if(get_VT_Pixel_Format() == VT_CAP_NV12 || get_VT_Pixel_Format() == VT_CAP_NV21)  //4k2k120 NV12
					dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en = 2;
				else
					rtd_pr_vt_err("[error] only support 4K2K120 nv12:%d\n",__LINE__);
			}else{
				dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en = 1; /* drop 1 pixel */
				in_size->nWidth = (in_size->nWidth >> 1);
			}
		}
		rtd_outl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg, dc2h_rgb2yuv_ctrl_reg.regValue);
		rtd_pr_vt_notice("line:%d,dc2h_drop_en=%d\n",__LINE__, dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en);
	}
    else if(in_position == KADP_VT_SCALER_INPUT)
    {
        in_size->nWidth = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DI_WID);
        in_size->nLength = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DI_LEN);
		set_dc2h_input_hact(in_size->nWidth);
        *dc2h_in_sel = _Idomain_Sdnr_input;
        set_dc2h_capture_sdnr_in_clock(TRUE);

        if(is_scaler_input_2p_mode(SLR_MAIN_DISPLAY) && (get_mach_type() == MACH_ARCH_RTK2885P)){ //4k120 source
            if(is_scale_down_case(in_size, &(pdc2h_hdl->out_size)) == FALSE){
                if((get_VT_Pixel_Format() == VT_CAP_NV12) || (get_VT_Pixel_Format() == VT_CAP_NV21)){
                    dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en = 2;
                }else{
                    rtd_pr_vt_emerg("[error] only support 4K2K120 nv12:%d\n",__LINE__);
                }
            }else{
                dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en = 1; /* drop 1 pixel */
                in_size->nWidth = (in_size->nWidth >> 1);
            }
        }
        rtd_outl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg, dc2h_rgb2yuv_ctrl_reg.regValue);
        rtd_pr_vt_notice("line:%d,dc2h_drop_en=%d\n",__LINE__, dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en);
    }
	else if(in_position == KADP_VT_SUB_SCALER_INPUT)
	{
		in_size->nWidth = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_IPH_ACT_WID);
        in_size->nLength = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_IPV_ACT_LEN);
		set_dc2h_input_hact(in_size->nWidth);
        *dc2h_in_sel = _Sub_Sdnr_input;
		set_dc2h_capture_sdnr_in_clock(TRUE);
		rtd_outl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg, dc2h_rgb2yuv_ctrl_reg.regValue);
        rtd_pr_vt_notice("line:%d,dc2h_drop_en=%d\n",__LINE__, dc2h_rgb2yuv_ctrl_reg.dc2h_drop_en);
	}
    else
    {
        rtd_pr_vt_emerg("[error]invalid cap position line:%d\n",__LINE__);
    }

	dc2h_1p_conv_2p_en(*dc2h_in_sel);
    // SDNR input W >4096 or V >2160, dc2h output capture max 2k
    if(in_size->nWidth > VT_CAP_FRAME_WIDTH_4K2K || in_size->nLength > VT_CAP_FRAME_HEIGHT_4K2K){
        if(pdc2h_hdl->out_size.nWidth > VT_CAP_FRAME_WIDTH_2K1k || pdc2h_hdl->out_size.nLength > VT_CAP_FRAME_HEIGHT_2K1k)
        {
            pdc2h_hdl->out_size.nWidth = VT_CAP_FRAME_WIDTH_2K1k;
            pdc2h_hdl->out_size.nLength = VT_CAP_FRAME_HEIGHT_2K1k;

            //vt_cap_frame_max_width = VT_CAP_FRAME_WIDTH_2K1k;
            //vt_cap_frame_max_height = VT_CAP_FRAME_HEIGHT_2K1k;

            curCapInfo.capLen = VT_CAP_FRAME_HEIGHT_2K1k;
            curCapInfo.capWid =  VT_CAP_FRAME_WIDTH_2K1k;
            rtd_pr_vt_notice("[warning] Input Source Hact over 3840, output only support 2k1k!!!!\n");
        }
            rtd_pr_vt_notice("[warning] Input Source Hact over 3840 !!!!\n");
     }
}

void drvif_dc2h_seqmode_conifg(unsigned char enable, int capWid, int capLen, int capSrc)
{
	pdc2h_hdl = &dc2h_hdl;
	memset(pdc2h_hdl, 0 , sizeof(DC2H_HANDLER));
	pdc2h_hdl->out_size.nWidth = capWid;
	pdc2h_hdl->out_size.nLength = capLen;
	pdc2h_hdl->dc2h_en = enable;

	if (enable == TRUE)
	{
        set_DC2H_rgb2yuv_ctrl_reg_refer_dump_location(capSrc, &pdc2h_hdl->in_size, &pdc2h_hdl->dc2h_in_sel);
        rtd_pr_vt_emerg("[VT]vt cap src=%d,input w=%d,h=%d\n", pdc2h_hdl->dc2h_in_sel,pdc2h_hdl->in_size.nWidth, pdc2h_hdl->in_size.nLength);

        if(pdc2h_hdl->out_size.nWidth > pdc2h_hdl->in_size.nWidth)
        {
            unsigned int temp = 0;
            rtd_pr_vt_emerg("output w > input w, need set output width max equal input width\n");
            temp = pdc2h_hdl->in_size.nWidth;
            if(temp%16 != 0)
            {
                temp = (temp - temp%16);
            }
            pdc2h_hdl->out_size.nWidth = temp;
            vt_bufwidth_as_linestep = TRUE;
        }
        if(pdc2h_hdl->out_size.nLength > pdc2h_hdl->in_size.nLength)
        {
            pdc2h_hdl->out_size.nLength = pdc2h_hdl->in_size.nLength;
            if(pdc2h_hdl->out_size.nLength%2 != 0)
            {
                pdc2h_hdl->out_size.nLength = pdc2h_hdl->out_size.nLength - 1;
            }
        }

        if(vt_bufwidth_as_linestep == TRUE && (get_VT_Pixel_Format() == VT_CAP_NV12 || get_VT_Pixel_Format() == VT_CAP_NV21 || get_VT_Pixel_Format() == VT_CAP_NV16))
        {
            dc2h_vi_dc2h_vi_ads_start_y_RBUS DC2H_VI_DC2H_vi_ads_start_y_Reg;
            dc2h_vi_dc2h_vi_ads_start_c_RBUS dc2h_vi_dc2h_vi_ads_start_c_Reg;
            if(_DISP_WID == 3440 && _DISP_LEN == 1440) // panel ratio 21:9
            {
                pdc2h_hdl->out_size.nLength = (pdc2h_hdl->out_size.nWidth*9)/21;
                if(pdc2h_hdl->out_size.nLength%2 != 0)
                {
                    pdc2h_hdl->out_size.nLength = pdc2h_hdl->out_size.nLength - 1;
                }
            }
            curCapInfo.x = (vt_cap_frame_max_width - pdc2h_hdl->out_size.nWidth)/2;
            if(curCapInfo.x % 128 != 0){
                curCapInfo.x = curCapInfo.x + (128 - (curCapInfo.x % 128));
            }
            curCapInfo.y = (vt_cap_frame_max_height - pdc2h_hdl->out_size.nLength)/2;

            DC2H_VI_DC2H_vi_ads_start_y_Reg.regValue = ((UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr + (curCapInfo.y*vt_cap_frame_max_width + curCapInfo.x));
            IoReg_Write32(DC2H_VI_DC2H_vi_ads_start_y_reg, DC2H_VI_DC2H_vi_ads_start_y_Reg.regValue);

            dc2h_vi_dc2h_vi_ads_start_c_Reg.regValue = DC2H_VI_DC2H_vi_ads_start_y_Reg.regValue + vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height) - vt_cap_frame_max_width*(curCapInfo.y)/2;
            //dc2h_vi_dc2h_vi_ads_start_c_Reg.regValue = ((UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr + vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height) + (vt_cap_frame_max_width*curCapInfo.y/2 + curCapInfo.x));
            IoReg_Write32(DC2H_VI_DC2H_vi_ads_start_c_reg, dc2h_vi_dc2h_vi_ads_start_c_Reg.regValue);

            rtd_pr_vt_emerg("x,y %d/%d, y/c addr %x/%x\n", curCapInfo.x, curCapInfo.y, IoReg_Read32(DC2H_VI_DC2H_vi_ads_start_y_reg), IoReg_Read32(DC2H_VI_DC2H_vi_ads_start_c_reg));
        }

        rtd_pr_vt_emerg("output wxh=%dx%d\n", pdc2h_hdl->out_size.nWidth, pdc2h_hdl->out_size.nLength);

		if(get_vt_VtSwBufferMode())
		{
			if(FALSE == drvif_dc2h_swmode_inforpc(TRUE,get_vt_VtBufferNum(),capWid,capLen))
			{
				rtd_pr_vt_emerg("%s=%d RPC fail so not setting dc2h and return!!\n", __FUNCTION__, __LINE__);
				return;
			}
		}

		if(get_vt_crop_enable() == TRUE){
			drvif_dc2h_input_overscan_config(curCapInfo.crop_size.x, curCapInfo.crop_size.y, curCapInfo.crop_size.w, curCapInfo.crop_size.h);
			pdc2h_hdl->in_size.nWidth = curCapInfo.crop_size.w;
			pdc2h_hdl->in_size.nLength = curCapInfo.crop_size.h;
			set_vt_crop_enable(FALSE);
		}else{
			drvif_dc2h_input_overscan_config(0, 0, pdc2h_hdl->in_size.nWidth, pdc2h_hdl->in_size.nLength);
		}
		drvif_color_colorspaceyuv2rgbtransfer(pdc2h_hdl->dc2h_in_sel);
		drvif_color_ultrazoom_config_dc2h_scaling_down(pdc2h_hdl);
		if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV21))
			drvif_vi_config(pdc2h_hdl->out_size.nWidth,pdc2h_hdl->out_size.nLength,0);
		else if (get_VT_Pixel_Format() == VT_CAP_NV16)
			drvif_vi_config(pdc2h_hdl->out_size.nWidth,pdc2h_hdl->out_size.nLength,1);
		else
			drvif_dc2h_seq_config(pdc2h_hdl);

		set_dc2h_dma_frame_drop_ctrl(Get_VFOD_FrameRate(), get_vt_target_fps());
	}
	else
	{
		reset_dc2h_hw_setting(); /* disable dc2h and reset dc2h */

		if (get_vt_VtSwBufferMode())
		{
			if(FALSE == drvif_dc2h_swmode_inforpc(FALSE,get_vt_VtBufferNum(),capWid,capLen))
			{
				rtd_pr_vt_emerg("%s=%d RPC fail when close dc2h!!\n", __FUNCTION__, __LINE__);
			}
		}
	}
}

void disable_dc2h_hw_dma(void)
{
    /* when secure source, disable dma if dmaen is enable */
    if(get_dc2h_capture_state() == TRUE)
    {
        dc2h_vi_dc2h_dmactl_RBUS vi_dmactl_reg;
        dc2h_dma_dc2h_seq_mode_ctrl1_RBUS dc2h_seq_mode_ctrl1_reg;
        dc2h_vi_dc2h_vi_doublebuffer_RBUS dc2h_vi_dc2h_vi_doublebuffer_Reg;
        dc2h_dma_dc2h_seq_interrupt_enable_RBUS dc2h_dma_dc2h_seq_interrupt_enable_Reg;
        if (get_vt_VtSwBufferMode())
        {
            dc2h_dma_dc2h_seq_interrupt_enable_Reg.regValue = rtd_inl(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg);
            dc2h_dma_dc2h_seq_interrupt_enable_Reg.vs_rising_ie = 0;
            rtd_outl(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg, dc2h_dma_dc2h_seq_interrupt_enable_Reg.regValue);
            msleep(10);
        }
        if ((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
        {
            vi_dmactl_reg.regValue = IoReg_Read32(DC2H_VI_DC2H_DMACTL_reg);
            vi_dmactl_reg.dmaen1 = 0;
            IoReg_Write32(DC2H_VI_DC2H_DMACTL_reg, vi_dmactl_reg.regValue);
        }
        else
        {
            dc2h_seq_mode_ctrl1_reg.regValue = IoReg_Read32(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg);
            dc2h_seq_mode_ctrl1_reg.cap_en = 0;
            IoReg_Write32(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg, dc2h_seq_mode_ctrl1_reg.regValue);
        }
        if(DC2H_VI_DC2H_vi_doublebuffer_get_vi_db_en(IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg)))
        { //if double buffer enable
            dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue = IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg);
            dc2h_vi_dc2h_vi_doublebuffer_Reg.vi_db_apply = 1;
            IoReg_Write32(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue);
        }
    }
}

void reset_dc2h_hw_setting(void)
{
	dc2h_dma_dc2h_seq_interrupt_enable_RBUS dc2h_dma_dc2h_seq_interrupt_enable_Reg;
	dc2h_vi_dc2h_dmactl_RBUS vi_dmactl_reg;
	dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_RBUS dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_Reg;
	dc2h_rgb2yuv_dc2h_rgb2yuv_ctrl_RBUS	dc2h_rgb2yuv_ctrl_reg;
	dc2h_scaledown_dc2h_hsd_ctrl0_RBUS dc2h_hsd_ctrl0_reg;
	dc2h_scaledown_dc2h_vsd_ctrl0_RBUS dc2h_vsd_ctrl0_reg;
	dc2h_dma_dc2h_seq_mode_ctrl1_RBUS dc2h_seq_mode_ctrl1_reg;
    dc2h_vi_dc2h_vi_doublebuffer_RBUS dc2h_vi_dc2h_vi_doublebuffer_Reg;
	dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_RBUS dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_reg;

	if (get_vt_VtSwBufferMode())
	{
		dc2h_dma_dc2h_seq_interrupt_enable_Reg.regValue = rtd_inl(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg);
		dc2h_dma_dc2h_seq_interrupt_enable_Reg.vs_rising_ie = 0;
		rtd_outl(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg, dc2h_dma_dc2h_seq_interrupt_enable_Reg.regValue);
		msleep(20);
	}

    dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue = IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg);
    dc2h_vi_dc2h_vi_doublebuffer_Reg.vi_db_en = 0;
    IoReg_Write32(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue);

	dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_reg.regValue = IoReg_Read32(DC2H_RGB2YUV_DC2H_3DMaskTo2D_DB_Ctrl_reg);
	dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_reg.color_db_en = 0;
	IoReg_Write32(DC2H_RGB2YUV_DC2H_3DMaskTo2D_DB_Ctrl_reg, dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_reg.regValue);

	if ((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
	{
		vi_dmactl_reg.regValue = rtd_inl(DC2H_VI_DC2H_DMACTL_reg);
		vi_dmactl_reg.dmaen1 = 0;
		rtd_outl(DC2H_VI_DC2H_DMACTL_reg, vi_dmactl_reg.regValue);
	}

	dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_Reg.regValue = rtd_inl(DC2H_RGB2YUV_DC2H_3DMaskTo2D_Ctrl_reg);
	dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_Reg.dc2h_3dmaskto2d_3dformat = 0;
	dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_Reg.dc2h_3dmaskto2d_en = 0;
	rtd_outl(DC2H_RGB2YUV_DC2H_3DMaskTo2D_Ctrl_reg, dc2h_rgb2yuv_dc2h_3dmaskto2d_ctrl_Reg.regValue);

	dc2h_rgb2yuv_ctrl_reg.regValue = rtd_inl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg);
	dc2h_rgb2yuv_ctrl_reg.dc2h_in_sel = 0;
	dc2h_rgb2yuv_ctrl_reg.en_rgb2yuv = 0;
	rtd_outl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg, dc2h_rgb2yuv_ctrl_reg.regValue);

	dc2h_hsd_ctrl0_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H_HSD_Ctrl0_reg);
	dc2h_hsd_ctrl0_reg.h_zoom_en = 0;
	rtd_outl(DC2H_SCALEDOWN_DC2H_HSD_Ctrl0_reg, dc2h_hsd_ctrl0_reg.regValue);

	dc2h_vsd_ctrl0_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H_VSD_Ctrl0_reg);
	dc2h_vsd_ctrl0_reg.v_zoom_en = 0;
	dc2h_vsd_ctrl0_reg.buffer_mode = 0;
	rtd_outl(DC2H_SCALEDOWN_DC2H_VSD_Ctrl0_reg, dc2h_vsd_ctrl0_reg.regValue);

	dc2h_seq_mode_ctrl1_reg.regValue = rtd_inl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg);
	dc2h_seq_mode_ctrl1_reg.rgb_to_argb_en = 0;
	dc2h_seq_mode_ctrl1_reg.cap_en = 0;
	rtd_outl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg, dc2h_seq_mode_ctrl1_reg.regValue);

    set_dc2h_capture_sdnr_in_clock(FALSE);
	set_dc2h_dma_frame_drop_ctrl(0, 0);
}

void set_vfod_freezed(unsigned char boffon)
{
	rtd_pr_vt_debug("fun:%s,boffon=%d\n", __FUNCTION__, boffon);
	if (CaptureCtrl_VT.cap_buffer[0].phyaddr == 0) {
		rtd_pr_vt_emerg("[Vt warning]vt no init, so no memory, return freeze action;%s=%d \n", __FUNCTION__, __LINE__);
		return;
	}
	if(((drvif_scaler_get_abnormal_dvs_long_flag()==TRUE)||(drvif_scaler_get_abnormal_dvs_short_flag()==TRUE))
		&& (get_vt_VtBufferNum() == MAX_CAP_BUF_NUM))
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
			if(TRUE == get_dc2h_capture_state()) /*dc2h already enable*/
			{
				vt_enable_dc2h(FALSE); /*stop dc2h,avoid memory trash,kernel panic KTASKWBS-13022*/
				rtd_pr_vt_emerg("stop dc2h done!\n");
			}
			return;
		}
		if(abnormal_dvs_cnt > MAX_CAP_BUF_NUM)
		{
			set_dqbuf_ioctl_fail_flag(TRUE);
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
				set_dqbuf_ioctl_fail_flag(FALSE);
				if(FALSE == get_dc2h_capture_state())
				{
					vt_enable_dc2h(TRUE); /* KTASKWBS-13909 dvs stable again, enable dc2h */
					rtd_pr_vt_emerg("enable dc2h done!\n");
				}
			}
		}
	}

	if(get_vt_VtBufferNum() == 1)
	{
		unsigned int timeoutcount = 100;
		dc2h_vi_dc2h_vi_doublebuffer_RBUS dc2h_vi_doublebuffer_Reg;
		dc2h_vi_doublebuffer_Reg.regValue = rtd_inl(DC2H_VI_DC2H_vi_doublebuffer_reg);
		dc2h_vi_doublebuffer_Reg.vi_db_en = 1; //enable double buffer
		rtd_outl(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_doublebuffer_Reg.regValue);

		if(boffon == TRUE) //freeze
		{
			dc2h_vi_doublebuffer_Reg.regValue = rtd_inl(DC2H_VI_DC2H_vi_doublebuffer_reg);
			dc2h_vi_doublebuffer_Reg.vi_db_apply = 1;
			rtd_outl(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_doublebuffer_Reg.regValue);

			do{
				if(DC2H_VI_DC2H_vi_doublebuffer_get_vi_db_apply(rtd_inl(DC2H_VI_DC2H_vi_doublebuffer_reg))){
					timeoutcount--;
					msleep(0);
				}
				else{
					break;
				}

			}while(timeoutcount);

			if(timeoutcount == 0)
			{
				rtd_pr_vt_notice("fun:%s, freeze before action timeout\n", __FUNCTION__);
			}

			//dc2h_vi_doublebuffer_Reg.vi_db_en = 1;
			if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
			{
				dc2h_vi_dc2h_dmactl_RBUS dc2h_vi_dc2h_dmactl_Reg;
				dc2h_vi_dc2h_dmactl_Reg.regValue = rtd_inl(DC2H_VI_DC2H_DMACTL_reg);
				dc2h_vi_dc2h_dmactl_Reg.dmaen1 = 0; //double buffer
				rtd_outl(DC2H_VI_DC2H_DMACTL_reg,dc2h_vi_dc2h_dmactl_Reg.regValue);
			}
			else
			{
				dc2h_dma_dc2h_seq_mode_ctrl1_RBUS dc2h_dma_dc2h_seq_mode_ctrl1_Reg;
				dc2h_dma_dc2h_seq_mode_ctrl1_Reg.regValue = rtd_inl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg);
				dc2h_dma_dc2h_seq_mode_ctrl1_Reg.cap_en = 0; //double buffer
				rtd_outl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg,dc2h_dma_dc2h_seq_mode_ctrl1_Reg.regValue);
			}
			dc2h_vi_doublebuffer_Reg.regValue = rtd_inl(DC2H_VI_DC2H_vi_doublebuffer_reg);
			dc2h_vi_doublebuffer_Reg.vi_db_apply = 1;  // double buffer apply
			rtd_outl(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_doublebuffer_Reg.regValue);

			timeoutcount = 100;
			do{
				if(DC2H_VI_DC2H_vi_doublebuffer_get_vi_db_apply(rtd_inl(DC2H_VI_DC2H_vi_doublebuffer_reg))){
					timeoutcount--;
					msleep(0);
					}
				else{
					break;
				}

			}while(timeoutcount);

			if(timeoutcount == 0)
			{
				rtd_pr_vt_notice("fun:%s, freeze action timeout\n", __FUNCTION__);
			}
			IndexOfFreezedVideoFrameBuffer = 0;
		}
		else
		{
			if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
			{
				dc2h_vi_dc2h_dmactl_RBUS dc2h_vi_dc2h_dmactl_Reg;
				dc2h_vi_dc2h_dmactl_Reg.regValue = rtd_inl(DC2H_VI_DC2H_DMACTL_reg);
				dc2h_vi_dc2h_dmactl_Reg.dmaen1 = 1; /* enable dma with double buffer control*/
				rtd_outl(DC2H_VI_DC2H_DMACTL_reg,dc2h_vi_dc2h_dmactl_Reg.regValue);
			}
			else
			{
				dc2h_dma_dc2h_seq_mode_ctrl1_RBUS dc2h_dma_dc2h_seq_mode_ctrl1_Reg;
				dc2h_dma_dc2h_seq_mode_ctrl1_Reg.regValue = rtd_inl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg);
				dc2h_dma_dc2h_seq_mode_ctrl1_Reg.cap_en = 1;
				rtd_outl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg,dc2h_dma_dc2h_seq_mode_ctrl1_Reg.regValue);
			}
			dc2h_vi_doublebuffer_Reg.regValue = rtd_inl(DC2H_VI_DC2H_vi_doublebuffer_reg);
			dc2h_vi_doublebuffer_Reg.vi_db_apply = 1;
			rtd_outl(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_doublebuffer_Reg.regValue);
			do{
				if(DC2H_VI_DC2H_vi_doublebuffer_get_vi_db_apply(rtd_inl(DC2H_VI_DC2H_vi_doublebuffer_reg))){
					timeoutcount--;
					msleep(0);
					}
				else
					break;

			}while(timeoutcount);

			if(timeoutcount == 0)
			{
				rtd_pr_vt_notice("fun:%s, unfreeze action timeout\n", __FUNCTION__);
			}
			/* wait one frame */
			timeoutcount = 100;
			dc2h_vi_doublebuffer_Reg.regValue = rtd_inl(DC2H_VI_DC2H_vi_doublebuffer_reg);
			dc2h_vi_doublebuffer_Reg.vi_db_apply = 1;
			rtd_outl(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_doublebuffer_Reg.regValue);
			do{
				if(DC2H_VI_DC2H_vi_doublebuffer_get_vi_db_apply(rtd_inl(DC2H_VI_DC2H_vi_doublebuffer_reg))){
					timeoutcount--;
					msleep(0);
					}
				else
					break;
			}while(timeoutcount);

			if(timeoutcount == 0)
			{
				rtd_pr_vt_notice("fun:%s, wait one frame timeout\n", __FUNCTION__);
			}
		}
		dc2h_vi_doublebuffer_Reg.regValue = rtd_inl(DC2H_VI_DC2H_vi_doublebuffer_reg);
		dc2h_vi_doublebuffer_Reg.vi_db_en = 0; /* doubel buffer only work when buffer_num=1 */
		rtd_outl(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_doublebuffer_Reg.regValue);

	}
    else
    {
        if (boffon == TRUE) {

            static int last_dequeue_index = -1;
            static unsigned char vt_first_dequeue_flag = TRUE;
            unsigned int bufnum = get_vt_VtBufferNum();

            if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX)
                rtd_pr_vt_notice("deq 90K %d\n", IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg));

            set_vt_dequeue_overflow_flag(FALSE);

            if(vt_first_dequeue_flag == TRUE)
            {
                vt_first_dequeue_flag = FALSE;
                IndexOfFreezedVideoFrameBuffer = (htonl(pVtBufStatus->write) + bufnum - 1)%bufnum;
            }
            else
            {
                if(((htonl(pVtBufStatus->read) + 1)%bufnum) == htonl(pVtBufStatus->write))
                {
                    //set_vt_dequeue_overflow_flag(TRUE);
                    IndexOfFreezedVideoFrameBuffer = htonl(pVtBufStatus->read);
                    set_vt_buf_dequeue(IndexOfFreezedVideoFrameBuffer);
                    rtd_pr_vt_notice("dequeue read=write,retry...\n");
                    return;
                }
                else
                {
                    IndexOfFreezedVideoFrameBuffer = (htonl(pVtBufStatus->read) + 1)%bufnum;
                }
            }

            set_vt_buf_dequeue(IndexOfFreezedVideoFrameBuffer);

            if(last_dequeue_index != -1)
                set_vt_buf_enqueue(last_dequeue_index);

            last_dequeue_index = IndexOfFreezedVideoFrameBuffer;
            pVtBufStatus->read = Scaler_ChangeUINT32Endian(IndexOfFreezedVideoFrameBuffer);

            if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX){
                rtd_pr_vt_notice("sta %d,%d,%d,%d,%d,dq_idx=%d,dmaen %d\n", \
                    htonl(pVtBufStatus->vtBufStatus[0]),\
                    htonl(pVtBufStatus->vtBufStatus[1]),\
                    htonl(pVtBufStatus->vtBufStatus[2]),\
                    htonl(pVtBufStatus->vtBufStatus[3]),\
                    htonl(pVtBufStatus->vtBufStatus[4]),\
                    IndexOfFreezedVideoFrameBuffer, DC2H_VI_DC2H_DMACTL_get_dmaen1(rtd_inl(DC2H_VI_DC2H_DMACTL_reg)));
            }
		}
        else{
			//rtd_outl(DC2H_DMA_dc2h_Cap_L3_Start_reg,0);
		}
	}
}

unsigned int sort_boundary_addr_max_index(void)
{
    unsigned int i,index = 0;
    unsigned int bufnum = get_vt_VtBufferNum();
    unsigned int max = CaptureCtrl_VT.cap_buffer[0].phyaddr;

    if(CaptureCtrl_VT.cap_buffer[0].phyaddr == 0 || bufnum == 1)
    {
        index = 0;
    }
    else
    {
        for( i = 1; i < bufnum; i++)
        {
            if(CaptureCtrl_VT.cap_buffer[i].phyaddr > max)
            {
                max = CaptureCtrl_VT.cap_buffer[i].phyaddr;
                index = i;
            }
        }
    }

    return index;
}

unsigned int sort_boundary_addr_min_index(void)
{
    unsigned int i,index = 0;
    unsigned int bufnum = get_vt_VtBufferNum();
    unsigned int min = CaptureCtrl_VT.cap_buffer[0].phyaddr;

    if(CaptureCtrl_VT.cap_buffer[0].phyaddr == 0 || bufnum == 1)
    {
        index = 0;
    }
    else
    {
        for( i = 1; i < bufnum; i++)
        {
            if(CaptureCtrl_VT.cap_buffer[i].phyaddr < min)
            {
                min = CaptureCtrl_VT.cap_buffer[i].phyaddr;
                index = i;
            }
        }
    }

    return index;
}

void blacken_nv12_buffer(unsigned int nv12_pa, unsigned int width, unsigned int height)
{
    unsigned char *nv12_va = NULL;
    nv12_va = (unsigned char *)phys_to_virt(nv12_pa);
    memset(nv12_va, 0, width * height);
    memset(nv12_va + vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height), 128, width * height / 2);
}

#endif

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

unsigned int get_vt_capBuf_Size(void)
{
    if((get_VT_Pixel_Format() == VT_CAP_ABGR8888) || (get_VT_Pixel_Format() == VT_CAP_ARGB8888) || (get_VT_Pixel_Format() == VT_CAP_RGBA8888))
        VT_CAPTURE_BUFFER_UNITSIZE = vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height*4);
    else if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV21))
        VT_CAPTURE_BUFFER_UNITSIZE = vt_bufAddr_align((vt_cap_frame_max_width*vt_cap_frame_max_height))+vt_bufAddr_align((vt_cap_frame_max_width*vt_cap_frame_max_height/2));
    else if(get_VT_Pixel_Format() == VT_CAP_NV16)
        VT_CAPTURE_BUFFER_UNITSIZE = vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height)+vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height);
    else
        VT_CAPTURE_BUFFER_UNITSIZE = vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height*3);

    return VT_CAPTURE_BUFFER_UNITSIZE;
}

unsigned char Capture_BufferMemInit_VT(unsigned int buffernum)
{
    unsigned int size = 0;
    int i;
    unsigned long allocatetotalsize = 0, carvedout_size = 0;
    unsigned long physicalstartaddree = 0;
    unsigned int vt_real_buf_nums = 0;
    unsigned long DMA_release_start = 0;
    unsigned long DMA_release_size = 0;

    size = get_vt_capBuf_Size();

    rtd_pr_vt_debug("[VT]buf size=0x%x\n",size);

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
            if(CaptureCtrl_VT.cap_buffer[i].phyaddr == 0)
            {
                vt_real_buf_nums++;
                CaptureCtrl_VT.cap_buffer[i].cache = NULL;
                CaptureCtrl_VT.cap_buffer[i].phyaddr = vt_bufAddr_align((i == 0) ? (physicalstartaddree) : (CaptureCtrl_VT.cap_buffer[i-1].phyaddr + size));
                CaptureCtrl_VT.cap_buffer[i].size = size;
                rtd_pr_vt_notice("[memory allocate1] buf[%d]=(%d KB), phy(%lx) \n", i, CaptureCtrl_VT.cap_buffer[i].size >> 10, (unsigned long)CaptureCtrl_VT.cap_buffer[i].phyaddr);
            }
        }
    }
    else
    {
        allocatetotalsize = size * buffernum;
        carvedout_size = 0;//(unsigned int)carvedout_buf_query(CARVEDOUT_VT, (void *)&physicalstartaddree);

        if ((carvedout_size == 0) || (physicalstartaddree == 0) || (allocatetotalsize > carvedout_size))
        {
            //rtd_pr_vt_notice("VT pli(%lx/%lx)\n", physicalstartaddree, carvedout_size);

            for(i = 0; i < buffernum; i++)
            {
                physicalstartaddree = pli_malloc(size, GFP_DCU2_FIRST);
                if(physicalstartaddree == INVALID_VAL)
                {
                    rtd_pr_vt_emerg("[error]retry pli malloc fail(%lx)\n", physicalstartaddree);
#ifdef UT_flag
                    return FALSE;
#endif
                }
                else
                {
                    VtAllocatedBufferStartAdress[i] = physicalstartaddree;
                    if(CaptureCtrl_VT.cap_buffer[i].phyaddr == 0)
                    {
                        vt_real_buf_nums++;
                        CaptureCtrl_VT.cap_buffer[i].cache = NULL;
                        CaptureCtrl_VT.cap_buffer[i].phyaddr = vt_bufAddr_align(physicalstartaddree);
                        CaptureCtrl_VT.cap_buffer[i].size = size;
                        rtd_pr_vt_notice("[memory allocate2] buf[%d]=(%d KB), phy(%lx) \n", i, CaptureCtrl_VT.cap_buffer[i].size >> 10, (unsigned long)CaptureCtrl_VT.cap_buffer[i].phyaddr);
                    }
                }
            }
        }
        else
        {
            for (i = 0; i < buffernum; i++)
            {
                if(CaptureCtrl_VT.cap_buffer[i].phyaddr == 0)
                {
                    vt_real_buf_nums++;
                    CaptureCtrl_VT.cap_buffer[i].cache = NULL;
                    CaptureCtrl_VT.cap_buffer[i].phyaddr = vt_bufAddr_align((i == 0) ? (physicalstartaddree) : (CaptureCtrl_VT.cap_buffer[i-1].phyaddr + size));
                    CaptureCtrl_VT.cap_buffer[i].size = size;
                    rtd_pr_vt_notice("[memory allocate3] buf[%d]=(%d KB), phy(%lx) \n", i, CaptureCtrl_VT.cap_buffer[i].size >> 10, (unsigned long)CaptureCtrl_VT.cap_buffer[i].phyaddr);
                }
            }
        }
    }

    set_vt_VtBufferNum(vt_real_buf_nums);

#ifndef UT_flag
    get_shm_vt_buf_status();
    init_vt_write_read_index();

    if((get_VT_Pixel_Format() == VT_CAP_NV12) || (get_VT_Pixel_Format() == VT_CAP_NV21))
    {
        for(i = 0; i < get_vt_VtBufferNum(); i++)
            blacken_nv12_buffer((UINT32)CaptureCtrl_VT.cap_buffer[i].phyaddr, vt_cap_frame_max_width, vt_cap_frame_max_height);
    }
#endif

    if(vt_real_buf_nums == 1)
        set_vt_VtSwBufferMode(FALSE);
    else
        set_vt_VtSwBufferMode(TRUE);

    return TRUE;
}

void Capture_BufferMemDeInit_VT(unsigned int buffernum)
{
	int i;

	for (i = 0; i < buffernum; i++)
	{
		if(VtAllocatedBufferStartAdress[i])
		{
			pli_free(VtAllocatedBufferStartAdress[i]);
			VtAllocatedBufferStartAdress[i] = 0;
		}
	}

	for (i = 0; i < buffernum; i++)
	{
		if (CaptureCtrl_VT.cap_buffer[i].phyaddr)
		{
			CaptureCtrl_VT.cap_buffer[i].cache = NULL;
			CaptureCtrl_VT.cap_buffer[i].phyaddr = (unsigned long)NULL;
			CaptureCtrl_VT.cap_buffer[i].size = 0;
		}
		rtd_pr_vt_debug("[MemDeInit_VT]VtAllocatedBufferStartAdress[%d]= %ld\n", i, VtAllocatedBufferStartAdress[i]);
	}

	rtd_pr_vt_debug("[VT memory free] Deinit done!\n");
}

#ifndef UT_flag
void set_DC2H_cap_boundaryaddr(unsigned int VtBufferNum)
{
	dc2h_dma_dc2h_cap_boundaryaddr1_RBUS dc2h_cap_boundaryaddr1_reg;  //up limit
	dc2h_dma_dc2h_cap_boundaryaddr2_RBUS dc2h_cap_boundaryaddr2_reg;  //low limit

	dc2h_dma_dc2h_cap_boundaryaddr3_RBUS dc2h_cap_boundaryaddr3_reg;
	dc2h_dma_dc2h_cap_boundaryaddr4_RBUS dc2h_cap_boundaryaddr4_reg;

	dc2h_dma_dc2h_cap_boundaryaddr5_RBUS dc2h_cap_boundaryaddr5_reg;
	dc2h_dma_dc2h_cap_boundaryaddr6_RBUS dc2h_cap_boundaryaddr6_reg;

	dc2h_dma_dc2h_cap_boundaryaddr7_RBUS dc2h_cap_boundaryaddr7_reg;
	dc2h_dma_dc2h_cap_boundaryaddr8_RBUS dc2h_cap_boundaryaddr8_reg;

	dc2h_dma_dc2h_cap_boundaryaddr9_RBUS dc2h_cap_boundaryaddr9_reg;
	dc2h_dma_dc2h_cap_boundaryaddr10_RBUS dc2h_cap_boundaryaddr10_reg;


	if(CaptureCtrl_VT.cap_buffer[0].phyaddr == 0)
	{
		rtd_pr_vt_emerg("[error]vt buffer addr invalid,%s=%d \n", __FUNCTION__, __LINE__);
	}
	else
	{
		if((get_VT_Pixel_Format() == VT_CAP_NV12) || (get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21)){
            if(vtPrintMask & VT_PRINT_MASK_MAX_VT_BUF_NUMS)
            {
                unsigned int vt_min_buf_addr = (UINT32)CaptureCtrl_VT.cap_buffer[sort_boundary_addr_min_index()].phyaddr;
                unsigned int vt_max_buf_add = ((UINT32)CaptureCtrl_VT.cap_buffer[sort_boundary_addr_max_index()].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[0].size - VT_BLKCAP_BURSTLEN);
                dc2h_cap_boundaryaddr1_reg.regValue = vt_max_buf_add;
                rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr1_reg, dc2h_cap_boundaryaddr1_reg.regValue);
                dc2h_cap_boundaryaddr2_reg.regValue = vt_min_buf_addr;
                rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr2_reg, dc2h_cap_boundaryaddr2_reg.regValue);

                dc2h_cap_boundaryaddr3_reg.regValue = vt_max_buf_add;
                rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr3_reg, dc2h_cap_boundaryaddr3_reg.regValue);
                dc2h_cap_boundaryaddr4_reg.regValue = vt_min_buf_addr;
                rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr4_reg, dc2h_cap_boundaryaddr4_reg.regValue);

                dc2h_cap_boundaryaddr5_reg.regValue = vt_max_buf_add;
                rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr5_reg, dc2h_cap_boundaryaddr5_reg.regValue);
                dc2h_cap_boundaryaddr6_reg.regValue = vt_min_buf_addr;
                rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr6_reg, dc2h_cap_boundaryaddr6_reg.regValue);

                dc2h_cap_boundaryaddr7_reg.regValue = vt_max_buf_add;
                rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr7_reg, dc2h_cap_boundaryaddr7_reg.regValue);
                dc2h_cap_boundaryaddr8_reg.regValue = vt_min_buf_addr;
                rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr8_reg, dc2h_cap_boundaryaddr8_reg.regValue);

                dc2h_cap_boundaryaddr9_reg.regValue = vt_max_buf_add;
                rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr9_reg, dc2h_cap_boundaryaddr9_reg.regValue);
                dc2h_cap_boundaryaddr10_reg.regValue = vt_min_buf_addr;
                rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr10_reg, dc2h_cap_boundaryaddr10_reg.regValue);
            }
            else if(5 == VtBufferNum)
			{
				dc2h_cap_boundaryaddr1_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[0].size - VT_BLKCAP_BURSTLEN ;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr1_reg, dc2h_cap_boundaryaddr1_reg.regValue);
				dc2h_cap_boundaryaddr2_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr2_reg, dc2h_cap_boundaryaddr2_reg.regValue);

				dc2h_cap_boundaryaddr3_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[1].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr3_reg, dc2h_cap_boundaryaddr3_reg.regValue);
				dc2h_cap_boundaryaddr4_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr4_reg, dc2h_cap_boundaryaddr4_reg.regValue);

				dc2h_cap_boundaryaddr5_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[2].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[2].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr5_reg, dc2h_cap_boundaryaddr5_reg.regValue);
				dc2h_cap_boundaryaddr6_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[2].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr6_reg, dc2h_cap_boundaryaddr6_reg.regValue);

				dc2h_cap_boundaryaddr7_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[3].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[3].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr7_reg, dc2h_cap_boundaryaddr7_reg.regValue);
				dc2h_cap_boundaryaddr8_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[3].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr8_reg, dc2h_cap_boundaryaddr8_reg.regValue);

				dc2h_cap_boundaryaddr9_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[4].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[4].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr9_reg, dc2h_cap_boundaryaddr9_reg.regValue);
				dc2h_cap_boundaryaddr10_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[4].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr10_reg, dc2h_cap_boundaryaddr10_reg.regValue);
			}
			else if(4 == VtBufferNum)
			{
				dc2h_cap_boundaryaddr1_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[0].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr1_reg, dc2h_cap_boundaryaddr1_reg.regValue);
				dc2h_cap_boundaryaddr2_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr2_reg, dc2h_cap_boundaryaddr2_reg.regValue);

				dc2h_cap_boundaryaddr3_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[1].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr3_reg, dc2h_cap_boundaryaddr3_reg.regValue);
				dc2h_cap_boundaryaddr4_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr4_reg, dc2h_cap_boundaryaddr4_reg.regValue);

				dc2h_cap_boundaryaddr5_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[2].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[2].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr5_reg, dc2h_cap_boundaryaddr5_reg.regValue);
				dc2h_cap_boundaryaddr6_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[2].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr6_reg, dc2h_cap_boundaryaddr6_reg.regValue);

				dc2h_cap_boundaryaddr7_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[3].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[3].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr7_reg, dc2h_cap_boundaryaddr7_reg.regValue);
				dc2h_cap_boundaryaddr8_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[3].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr8_reg, dc2h_cap_boundaryaddr8_reg.regValue);

				dc2h_cap_boundaryaddr9_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[3].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[3].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr9_reg, dc2h_cap_boundaryaddr9_reg.regValue);
				dc2h_cap_boundaryaddr10_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[3].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr10_reg, dc2h_cap_boundaryaddr10_reg.regValue);
			}
			else if(3 == VtBufferNum)
			{
				dc2h_cap_boundaryaddr1_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[0].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr1_reg, dc2h_cap_boundaryaddr1_reg.regValue);
				dc2h_cap_boundaryaddr2_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr2_reg, dc2h_cap_boundaryaddr2_reg.regValue);

				dc2h_cap_boundaryaddr3_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[1].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr3_reg, dc2h_cap_boundaryaddr3_reg.regValue);
				dc2h_cap_boundaryaddr4_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr4_reg, dc2h_cap_boundaryaddr4_reg.regValue);

				dc2h_cap_boundaryaddr5_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[2].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[2].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr5_reg, dc2h_cap_boundaryaddr5_reg.regValue);
				dc2h_cap_boundaryaddr6_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[2].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr6_reg, dc2h_cap_boundaryaddr6_reg.regValue);

				dc2h_cap_boundaryaddr7_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[2].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[2].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr7_reg, dc2h_cap_boundaryaddr7_reg.regValue);
				dc2h_cap_boundaryaddr8_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[2].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr8_reg, dc2h_cap_boundaryaddr8_reg.regValue);

				dc2h_cap_boundaryaddr9_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[2].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[2].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr9_reg, dc2h_cap_boundaryaddr9_reg.regValue);
				dc2h_cap_boundaryaddr10_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[2].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr10_reg, dc2h_cap_boundaryaddr10_reg.regValue);

			}
			else if(2 == VtBufferNum)
			{
				dc2h_cap_boundaryaddr1_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[0].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr1_reg, dc2h_cap_boundaryaddr1_reg.regValue);
				dc2h_cap_boundaryaddr2_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr2_reg, dc2h_cap_boundaryaddr2_reg.regValue);

				dc2h_cap_boundaryaddr3_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[1].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr3_reg, dc2h_cap_boundaryaddr3_reg.regValue);
				dc2h_cap_boundaryaddr4_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr4_reg, dc2h_cap_boundaryaddr4_reg.regValue);

				dc2h_cap_boundaryaddr5_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[1].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr5_reg, dc2h_cap_boundaryaddr5_reg.regValue);
				dc2h_cap_boundaryaddr6_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr6_reg, dc2h_cap_boundaryaddr6_reg.regValue);

				dc2h_cap_boundaryaddr7_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[1].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr7_reg, dc2h_cap_boundaryaddr7_reg.regValue);
				dc2h_cap_boundaryaddr8_reg.regValue = (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr8_reg, dc2h_cap_boundaryaddr8_reg.regValue);

				dc2h_cap_boundaryaddr9_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[1].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr9_reg, dc2h_cap_boundaryaddr9_reg.regValue);
				dc2h_cap_boundaryaddr10_reg.regValue = (UINT32)CaptureCtrl_VT.cap_buffer[1].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr10_reg, dc2h_cap_boundaryaddr10_reg.regValue);

			}
			else //1 == VtBufferNum
			{
				dc2h_cap_boundaryaddr1_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[0].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr1_reg, dc2h_cap_boundaryaddr1_reg.regValue);
				dc2h_cap_boundaryaddr2_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr2_reg, dc2h_cap_boundaryaddr2_reg.regValue);

				dc2h_cap_boundaryaddr3_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[0].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr3_reg, dc2h_cap_boundaryaddr3_reg.regValue);
				dc2h_cap_boundaryaddr4_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr4_reg, dc2h_cap_boundaryaddr4_reg.regValue);

				dc2h_cap_boundaryaddr5_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[0].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr5_reg, dc2h_cap_boundaryaddr5_reg.regValue);
				dc2h_cap_boundaryaddr6_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr6_reg, dc2h_cap_boundaryaddr6_reg.regValue);

				dc2h_cap_boundaryaddr7_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[0].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr7_reg, dc2h_cap_boundaryaddr7_reg.regValue);
				dc2h_cap_boundaryaddr8_reg.regValue = (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr8_reg, dc2h_cap_boundaryaddr8_reg.regValue);

				dc2h_cap_boundaryaddr9_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[0].size - VT_BLKCAP_BURSTLEN;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr9_reg, dc2h_cap_boundaryaddr9_reg.regValue);
				dc2h_cap_boundaryaddr10_reg.regValue = (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr;
				rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr10_reg, dc2h_cap_boundaryaddr10_reg.regValue);
			}
		}
		else
		{
            //Sequential capture just use 1st boundary address register
            dc2h_cap_boundaryaddr1_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[sort_boundary_addr_max_index()].phyaddr +  (UINT32)CaptureCtrl_VT.cap_buffer[0].size;
            rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr1_reg, dc2h_cap_boundaryaddr1_reg.regValue);
            dc2h_cap_boundaryaddr2_reg.regValue =  (UINT32)CaptureCtrl_VT.cap_buffer[sort_boundary_addr_min_index()].phyaddr;
            rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr2_reg, dc2h_cap_boundaryaddr2_reg.regValue);
		}
	}
}

void drvif_DC2H_dispD_CaptureConfig(VT_CUR_CAPTURE_INFO capInfo)
{
    if(capInfo.enable == TRUE)
    {
		if (CaptureCtrl_VT.cap_buffer[0].phyaddr == 0) {
			rtd_pr_vt_emerg("[Vt warning]vt no init, so no memory, return setting;%s=%d \n", __FUNCTION__, __LINE__);
			return;
		}

		if ((capInfo.capWid > vt_cap_frame_max_width)||(capInfo.capLen > vt_cap_frame_max_height)) {
			rtd_pr_vt_emerg("[VT warning]drvif_DC2H_dispD_CaptureConfig:could not support max than 2k1k output w=%d,output h=%d! \n", capInfo.capWid,capInfo.capLen);
			return;
		}

		if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
		{
			dc2h_vi_dc2h_vi_ads_start_y_RBUS DC2H_VI_DC2H_vi_ads_start_y_Reg;
			dc2h_vi_dc2h_vi_ads_start_c_RBUS dc2h_vi_dc2h_vi_ads_start_c_Reg;
			dc2h_dma_dc2h_cap_l3_start_RBUS dc2h_cap_l3_start_reg;

			dc2h_cap_l3_start_reg.regValue= 0;
			rtd_outl(DC2H_DMA_dc2h_Cap_L3_Start_reg, dc2h_cap_l3_start_reg.regValue);  //used record freeze buffer

			if(Get_PANEL_VFLIP_ENABLE())
			{
				DC2H_VI_DC2H_vi_ads_start_y_Reg.regValue = ((UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr + (vt_cap_frame_max_width*vt_cap_frame_max_height - vt_cap_frame_max_width));
				IoReg_Write32(DC2H_VI_DC2H_vi_ads_start_y_reg, DC2H_VI_DC2H_vi_ads_start_y_Reg.regValue);

				if((get_VT_Pixel_Format() == VT_CAP_NV12) || (get_VT_Pixel_Format() == VT_CAP_NV21))
                    dc2h_vi_dc2h_vi_ads_start_c_Reg.regValue = ((UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr + vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height) + (vt_cap_frame_max_width*vt_cap_frame_max_height/2 - vt_cap_frame_max_width));
				else //NV16
                    dc2h_vi_dc2h_vi_ads_start_c_Reg.regValue = ((UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr + vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height) + (vt_cap_frame_max_width*vt_cap_frame_max_height - vt_cap_frame_max_width));
				IoReg_Write32(DC2H_VI_DC2H_vi_ads_start_c_reg, dc2h_vi_dc2h_vi_ads_start_c_Reg.regValue);
			}
			else
			{
				DC2H_VI_DC2H_vi_ads_start_y_Reg.regValue = (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr;
				IoReg_Write32(DC2H_VI_DC2H_vi_ads_start_y_reg, DC2H_VI_DC2H_vi_ads_start_y_Reg.regValue);

                dc2h_vi_dc2h_vi_ads_start_c_Reg.regValue = (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr + vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height);
				IoReg_Write32(DC2H_VI_DC2H_vi_ads_start_c_reg, dc2h_vi_dc2h_vi_ads_start_c_Reg.regValue);
			}

			//set boundary address
			set_DC2H_cap_boundaryaddr(get_vt_VtBufferNum());

		}
		else
		{
            dc2h_dma_dc2h_cap_l1_start_RBUS dc2h_cap_l1_start_reg;
            dc2h_dma_dc2h_cap_l2_start_RBUS dc2h_cap_l2_start_reg;
            dc2h_dma_dc2h_cap_l3_start_RBUS dc2h_cap_l3_start_reg;

            if(Get_PANEL_VFLIP_ENABLE())
            {
                if(get_VT_Pixel_Format() == VT_CAP_RGB888)
                    dc2h_cap_l1_start_reg.regValue = ((UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr + vt_cap_frame_max_width*vt_cap_frame_max_height*3 - vt_cap_frame_max_width*3);
                else if(get_VT_Pixel_Format() == VT_CAP_ARGB8888 || get_VT_Pixel_Format() == VT_CAP_RGBA8888 || get_VT_Pixel_Format() == VT_CAP_ABGR8888)
                    dc2h_cap_l1_start_reg.regValue = ((UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr + vt_cap_frame_max_width*vt_cap_frame_max_height*4 - vt_cap_frame_max_width*4);
                else
                   rtd_pr_vt_emerg("[error]unknow pixel format\n");
            }
            else
            {
                dc2h_cap_l1_start_reg.regValue = (UINT32)CaptureCtrl_VT.cap_buffer[0].phyaddr;
            }

            rtd_outl(DC2H_DMA_dc2h_Cap_L1_Start_reg, dc2h_cap_l1_start_reg.regValue);

            dc2h_cap_l2_start_reg.regValue = 0;
            rtd_outl(DC2H_DMA_dc2h_Cap_L2_Start_reg, dc2h_cap_l2_start_reg.regValue);

            dc2h_cap_l3_start_reg.regValue = 0;
            rtd_outl(DC2H_DMA_dc2h_Cap_L3_Start_reg, dc2h_cap_l3_start_reg.regValue);

            set_DC2H_cap_boundaryaddr(get_vt_VtBufferNum());
		}

        drvif_dc2h_seqmode_conifg(TRUE, capInfo.capWid, capInfo.capLen, capInfo.capSrc);
	} else {
		if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21)){
			dc2h_scaledown_dc2h_444to422_ctrl_RBUS dc2h_scaledown_dc2h_444to422_ctrl_reg;

			dc2h_scaledown_dc2h_444to422_ctrl_reg.regValue = rtd_inl(DC2H_SCALEDOWN_DC2H_444to422_CTRL_reg);
			if(VTDumpLocation == KADP_VT_SCALER_OUTPUT)
				dc2h_scaledown_dc2h_444to422_ctrl_reg.cbcr_swap_en = 0;
			rtd_outl(DC2H_SCALEDOWN_DC2H_444to422_CTRL_reg, dc2h_scaledown_dc2h_444to422_ctrl_reg.regValue);
		}
		drvif_dc2h_seqmode_conifg(FALSE, capInfo.capWid, capInfo.capLen, capInfo.capSrc);
	}
}

unsigned char do_vt_reqbufs(unsigned int buf_cnt)
{
	if(0 == buf_cnt) /* Release buffers by setting buf_cnt = 0*/
	{
		if((vfod_capture_location == KADP_VT_DISPLAY_OUTPUT) || (vfod_capture_location == KADP_VT_SCALER_OUTPUT)
			|| (vfod_capture_location == KADP_VT_OSDVIDEO_OUTPUT) || (vfod_capture_location == KADP_VT_SCALER_INPUT))
		{
			if(HAL_VT_Finalize() == FALSE)
			{
				rtd_pr_vt_notice("%s,release buffers fail\n", __FUNCTION__);
				return FALSE;
			}
			rtd_pr_vt_notice("reqbufs(0)for dc2h capture\n");
		}
		else
		{
			rtd_pr_vt_emerg("func:%s,unsupport capture_location\n", __FUNCTION__);
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		down(&VT_Semaphore);

		if (Capture_BufferMemInit_VT(buf_cnt) == FALSE)
		{
			rtd_pr_vt_emerg("VT init allocate memory fail;%s=%d \n", __FUNCTION__, __LINE__);
			up(&VT_Semaphore);
			return FALSE;
		}

		set_vt_VtBufferNum(buf_cnt);
		if(1 == buf_cnt)
			set_vt_VtSwBufferMode(FALSE);
		else
			set_vt_VtSwBufferMode(TRUE);
		set_vt_VtCaptureVDC(FALSE);

		rtd_pr_vt_debug("reqbufs(%d) success\n", buf_cnt);
		up(&VT_Semaphore);
		return TRUE;

	}
}

unsigned char do_vt_streamoff(void)
{
	if(get_vt_function() == TRUE)
	{
		if((vfod_capture_location == KADP_VT_DISPLAY_OUTPUT) || (vfod_capture_location == KADP_VT_SCALER_OUTPUT)
			|| (vfod_capture_location == KADP_VT_OSDVIDEO_OUTPUT) || (vfod_capture_location == KADP_VT_SCALER_INPUT))
		{
			if(get_dc2h_capture_state() == TRUE)
			{
				vt_enable_dc2h(FALSE);
				rtd_pr_vt_notice("%s,dc2h capture", __FUNCTION__);
			}
			return TRUE;
		}
		else
		{
			rtd_pr_vt_emerg("func:%s,unsupport capture_location\n", __FUNCTION__);
			return FALSE;
		}
	}
	else
	{
		rtd_pr_vt_notice("can't do streamoff,%s=%d \n", __FUNCTION__, __LINE__);
		return FALSE;
	}
}

unsigned char do_vt_capture_streamon(void)
{
	ppoverlay_display_timing_ctrl1_RBUS display_timing_ctrl1_reg;
	down(&VT_Semaphore);

	display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);

	if (display_timing_ctrl1_reg.disp_en == 0)/*means d domain clock off, stop capture*/
	{
		rtd_pr_vt_emerg("[Warning Vt]%s=%d D domain clock is off status,could not do vt capture!\n", __FUNCTION__, __LINE__);
		set_vt_function(TRUE);/* KTASKWBS-14031 if VT returns fail in stream on,AP will release memory,But VT finalize run condition is get_vt_function() == TRUE,so set_vt_function(TRUE)*/
		up(&VT_Semaphore);
		return FALSE;
	}

	if(FALSE == get_vt_StreamOn_flag())
	{
		rtd_pr_vt_notice("vt_StreamOn_flag FALSE\n");
		set_vt_function(TRUE);
		up(&VT_Semaphore);
		return FALSE;
	}

	if(get_vt_function() == FALSE)
	{
		set_vt_function(TRUE);

		if((vfod_capture_location == KADP_VT_DISPLAY_OUTPUT) || (vfod_capture_location == KADP_VT_SCALER_OUTPUT)
			|| (vfod_capture_location == KADP_VT_OSDVIDEO_OUTPUT) || (vfod_capture_location == KADP_VT_SCALER_INPUT))
		{

			curCapInfo.enable =  1; //iMode;
			curCapInfo.capMode = VT_CAP_MODE_SEQ;

			curCapInfo.capSrc =  vfod_capture_location;  //0:scaler output 1:display output
			curCapInfo.capWid = vfod_capture_out_W; //iWid;
			curCapInfo.capLen = vfod_capture_out_H;

			drvif_DC2H_dispD_CaptureConfig(curCapInfo);

			VFODState.bEnabled = TRUE;
			VFODState.bAppliedPQ=TRUE;
			VFODState.bFreezed =FALSE;
			VFODState.framerateDivide = get_framerateDivide();
			VTDumpLocation = KADP_VT_DISPLAY_OUTPUT;
			rtd_pr_vt_debug("%s Success\n", __FUNCTION__);

			up(&VT_Semaphore);
			return TRUE;
		}
		else
		{
			rtd_pr_vt_emerg("func:%s,unsupport capture_location\n", __FUNCTION__);
			up(&VT_Semaphore);
			return FALSE;
		}
	}
	else
	{
		rtd_pr_vt_notice("VT aready streamon,do nothing;%s=%d \n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return TRUE;
	}
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
    if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_STATE) ==  _MODE_STATE_ACTIVE){
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

	if(((drvif_scaler_get_abnormal_dvs_long_flag()==TRUE)||(drvif_scaler_get_abnormal_dvs_short_flag()==TRUE))
		&& (get_vt_VtBufferNum() == MAX_CAP_BUF_NUM)){ /* add vr360 condition(5 buffers),avoid capture out of range */
		rtd_pr_vt_emerg("[Warning VBE][VT] Vbe at abnormal dvs status,could not do vt capture!\n");
		up(&VT_Semaphore);
		return FALSE;
	}

	if(get_vt_function() == FALSE) {
		curCapInfo.enable = 0; //iMode;
		curCapInfo.capMode = VT_CAP_MODE_SEQ;

		curCapInfo.capSrc =  vfod_capture_location;
		curCapInfo.capWid = vfod_capture_out_W;
		curCapInfo.capLen = vfod_capture_out_H;

		set_vt_function(TRUE);
		set_vt_VtBufferNum(buffernum);
		set_vt_VtCaptureVDC(FALSE);

        //drvif_DC2H_dispD_CaptureConfig(curCapInfo);

		VFODState.bEnabled = TRUE;
		VFODState.bAppliedPQ = TRUE;
		VFODState.bFreezed = FALSE;
		VFODState.framerateDivide = get_framerateDivide();
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
		if(get_dc2h_capture_state() == TRUE || get_vt_VtSwBufferMode() == TRUE)
		{
			curCapInfo.enable = 0; //iMode;
			drvif_DC2H_dispD_CaptureConfig(curCapInfo);
		}

		Capture_BufferMemDeInit_VT(get_vt_VtBufferNum());
		set_vt_VtBufferNum(5);
		IndexOfFreezedVideoFrameBuffer = 0;
		//set_VT_Pixel_Format(VT_CAP_RGB888);
		set_vt_VtSwBufferMode(FALSE);
		VFODState.bEnabled = FALSE;
		VFODState.bAppliedPQ = TRUE;
		VFODState.bFreezed = FALSE;
		VFODState.framerateDivide = 1;
		VTDumpLocation = KADP_VT_MAX_LOCATION;
		set_vt_function(FALSE);
		set_dqbuf_ioctl_fail_flag(FALSE);
		abnormal_dvs_cnt = 0;
		wait_dvs_stable_cnt = 0;
        set_vt_target_fps(0);
        reset_vt_buf_status();
        set_vt_dequeue_enqueue_count(0);
        vtPrintMask = 0;
        set_vt_bufwidth_as_linestep(FALSE);
		rtd_pr_vt_emerg("%s done\n", __FUNCTION__);
		if ((get_vt_EnableFRCMode()==FALSE))
		{
			//360vr close FRC, so end 360vr need reopen frc
#ifndef CONFIG_MEMC_BYPASS
			//please close memc here
			MEMC_Set_malloc_address(TRUE);
#endif
			set_vt_EnableFRCMode(TRUE);

			rtd_pr_vt_emerg("VT finalize EnableFRCMode;%s \n", __FUNCTION__);
			if(get_film_mode_parameter().enable == _DISABLE){
				rtd_pr_vt_notice("[VR360] exit vt_frc_mode, set mc on!!\n");
				if(0 /*(Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) > 2560) &&
					(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE) > 1440) &&
					(Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) > 49000) &&
					(Get_DISPLAY_REFRESH_RATE() < 120)*/){
					Scaler_MEMC_MEMC_CLK_OnOff(0, 1, 1);	//4k 50/60Hz, enable MC clock with mute on
				}
				else{
					if(FALSE == force_enable_two_step_uzu()) /* input 120 memc always bypass */
						Scaler_MEMC_MEMC_CLK_OnOff(1, 1, 1);	//not 4k 50/60Hz, enable MEMC clock with mute on
				}
				if(FALSE == force_enable_two_step_uzu()){
					Scaler_MEMC_outMux(_ENABLE,_ENABLE);	//db on, mux on
				}

				drvif_memc_outBg_ctrl(_DISABLE);
			}

		}

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
	pVideoFrameBufferCapabilityInfo ->numOfVideoFrameBuffer = get_vt_VtBufferNum();

	if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21)){
		pVideoFrameBufferCapabilityInfo ->numOfPlane = KADP_VT_VIDEO_FRAME_BUFFER_PLANE_SEMI_PLANAR;
	} else {
		pVideoFrameBufferCapabilityInfo ->numOfPlane = KADP_VT_VIDEO_FRAME_BUFFER_PLANE_INTERLEAVED;//sequence buffer
	}
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
	pVideoFrameOutputDeviceCapabilityInfo ->bSupportedControlEnabled = TRUE;
	pVideoFrameOutputDeviceCapabilityInfo ->bSupportedControlFreezed = TRUE;
	pVideoFrameOutputDeviceCapabilityInfo ->bSupportedControlFramerateDivide = FALSE;
	pVideoFrameOutputDeviceCapabilityInfo ->bSupportedControlPQ = FALSE;
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
	pVideoFrameOutputDeviceLimitationInfo->bSupportInputVideoDeInterlacing =FALSE;
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
		if(get_VT_Pixel_Format() == VT_CAP_NV16){
			pVideoFrameBufferProperty->pixelFormat = KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV422_SEMI_PLANAR;
			pVideoFrameBufferProperty->stride = vt_cap_frame_max_width; /* capture buffer width */
			pVideoFrameBufferProperty->width = curCapInfo.capWid;
		}else if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV21)){
			pVideoFrameBufferProperty->pixelFormat = KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_YUV420_SEMI_PLANAR;
			pVideoFrameBufferProperty->stride = vt_cap_frame_max_width;
			pVideoFrameBufferProperty->width = curCapInfo.capWid;
		}else if((get_VT_Pixel_Format() == VT_CAP_ABGR8888)||(get_VT_Pixel_Format() == VT_CAP_ARGB8888)||(get_VT_Pixel_Format() == VT_CAP_RGBA8888)) {
			pVideoFrameBufferProperty->pixelFormat = KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_ARGB;
			pVideoFrameBufferProperty->stride = (vt_cap_frame_max_width * 4);
			pVideoFrameBufferProperty->width = curCapInfo.capWid*4;
		} else {
			pVideoFrameBufferProperty->pixelFormat = KADP_VT_VIDEO_FRAME_BUFFER_PIXEL_FORMAT_RGB;
			pVideoFrameBufferProperty->stride = (vt_cap_frame_max_width * 3);
			pVideoFrameBufferProperty->width = curCapInfo.capWid*3;
		}

		pVideoFrameBufferProperty->height = curCapInfo.capLen;
        for(i = 0; i < get_vt_VtBufferNum(); i++)
        {
            pVideoFrameBufferProperty->ppPhysicalAddress[i] = (unsigned int)CaptureCtrl_VT.cap_buffer[i].phyaddr;
        }

		pVideoFrameBufferProperty->vfbbuffernumber = get_vt_VtBufferNum();
        pVideoFrameBufferProperty->vtYbufSize = vt_bufAddr_align((vt_cap_frame_max_width*vt_cap_frame_max_height));
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
    dc2h_dma_dc2h_seq_interrupt_enable_RBUS dc2h_dma_dc2h_seq_interrupt_enable_Reg;
    dc2h_vi_dc2h_dmactl_RBUS vi_dmactl_reg;
    dc2h_dma_dc2h_seq_mode_ctrl1_RBUS dc2h_seq_mode_ctrl1_reg;
    dc2h_vi_dc2h_vi_doublebuffer_RBUS dc2h_vi_dc2h_vi_doublebuffer_Reg;
    static unsigned char ucOnce_Drm_case_flag = FALSE;

    if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
        rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    down(&VT_Semaphore);

    if(get_vt_function() == TRUE)
    {
        if(get_drm_case_securestatus()== TRUE){
            disable_dc2h_hw_dma();
            if(vtPrintMask & VT_PRINT_MASK_DRM_LOG)
                rtd_pr_vt_notice("[VT]get_drm_case_securestatus is true;%s=%d\n", __FUNCTION__, __LINE__);
            *pIndexOfCurrentVideoFrameBuffer = IndexOfFreezedVideoFrameBuffer;
            ucOnce_Drm_case_flag = TRUE;
            up(&VT_Semaphore);
            return TRUE;
        }
        else{
            if((get_vt_VtSwBufferMode()) && (DC2H_DMA_dc2h_seq_Interrupt_Enable_vs_rising_ie(IoReg_Read32(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg) == 0)))
            {
               dc2h_dma_dc2h_seq_interrupt_enable_Reg.regValue = rtd_inl(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg);
               dc2h_dma_dc2h_seq_interrupt_enable_Reg.vs_rising_ie = 1;
               rtd_outl(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg, dc2h_dma_dc2h_seq_interrupt_enable_Reg.regValue);
               msleep(20);
            }

            if(ucOnce_Drm_case_flag == TRUE){
                ucOnce_Drm_case_flag = FALSE;
               if((DC2H_VI_DC2H_DMACTL_dmaen1(IoReg_Read32(DC2H_VI_DC2H_DMACTL_reg)) == 0) || (DC2H_DMA_dc2h_Seq_mode_CTRL1_cap_en(IoReg_Read32(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg)) == 0))
               {
                    if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
                    {
                        vi_dmactl_reg.regValue = rtd_inl(DC2H_VI_DC2H_DMACTL_reg);
                        vi_dmactl_reg.dmaen1 = 1;
                        rtd_outl(DC2H_VI_DC2H_DMACTL_reg, vi_dmactl_reg.regValue);
                    }
                    else
                    {
                        dc2h_seq_mode_ctrl1_reg.regValue = rtd_inl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg);
                        dc2h_seq_mode_ctrl1_reg.cap_en = 1;
                        rtd_outl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg, dc2h_seq_mode_ctrl1_reg.regValue);
                    }

                    if(DC2H_VI_DC2H_vi_doublebuffer_get_vi_db_en(IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg)))
                    { //if double buffer enable
                        dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue = IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg);
                        dc2h_vi_dc2h_vi_doublebuffer_Reg.vi_db_apply = 1;
                        IoReg_Write32(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue);
                    }
               }
            }
        }

		if(VFODState.bFreezed == TRUE) {
			set_vfod_freezed(FALSE);
			VFODState.bFreezed = FALSE;
		}

		if(VFODState.bFreezed == FALSE) {
			set_vfod_freezed(TRUE);
			VFODState.bFreezed = TRUE;
		}

		if(VFODState.bFreezed == TRUE) 
		{
			if(TRUE == get_dqbuf_ioctl_fail_flag())
			{
				up(&VT_Semaphore);
				return FALSE;
			}

            if(get_vt_dequeue_overflow_flag() == TRUE)
            {
                *pIndexOfCurrentVideoFrameBuffer = VT_NOT_ENOUGH_RESOURCE;
                rtd_pr_vt_notice("[VT]dq_idx=0xFF,will retrun VT_NOT_ENOUGH_RESOURCE in rhal api\n");
            }
            else{
                *pIndexOfCurrentVideoFrameBuffer = IndexOfFreezedVideoFrameBuffer;
            }
		}

		up(&VT_Semaphore);
		return TRUE;
	}
	else
	{
		rtd_pr_vt_notice("VT is not Inited So return False;%s=%d\n", __FUNCTION__, __LINE__);
		up(&VT_Semaphore);
		return FALSE;
	}
}

unsigned char do_vt_dqbuf(unsigned int *pdqbuf_Index)
{
	if(get_vt_function() == FALSE)
	{
		rtd_pr_vt_notice("[fail]func:%s,vt not stream on!!!\n", __FUNCTION__);
		*pdqbuf_Index = VT_NOT_ENOUGH_RESOURCE;
		return FALSE;
	}

	if(FALSE == HAL_VT_GetVideoFrameBufferIndex(KADP_VT_VIDEO_WINDOW_0, pdqbuf_Index))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
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

	if(videoFrameOutputDeviceStateFlags==KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_NOFX) {
		rtd_pr_vt_debug("KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_NOFX is do nothing\n");
		return TRUE;
	}

	down(&VT_Semaphore);
	if(get_vt_function() == TRUE) {

		if (videoFrameOutputDeviceStateFlags&KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_ENABLED) {
			if (VFODState.bEnabled != pVideoFrameOutputDeviceState->bEnabled) {
				curCapInfo.enable = pVideoFrameOutputDeviceState->bEnabled;
				VFODState.bEnabled = pVideoFrameOutputDeviceState->bEnabled;
				drvif_DC2H_dispD_CaptureConfig(curCapInfo);
			}
		}

		if(videoFrameOutputDeviceStateFlags&KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FRAMERATE_DIVIDE) {
			rtd_pr_vt_debug("KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FRAMERATE_DIVIDE is %d\n",pVideoFrameOutputDeviceState->framerateDivide);
			VFODState.framerateDivide = pVideoFrameOutputDeviceState->framerateDivide;

		}

		/*freeze do last*/
		if(videoFrameOutputDeviceStateFlags&KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FREEZED) {
			if(get_vt_function() == TRUE) {
				if(pVideoFrameOutputDeviceState->bFreezed == VFODState.bFreezed) {
					rtd_pr_vt_debug("KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FREEZED is the same as you setting\n");
				} else {
					set_vfod_freezed(pVideoFrameOutputDeviceState->bFreezed);
					VFODState.bFreezed = pVideoFrameOutputDeviceState->bFreezed;
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

unsigned short Get_VFOD_FrameRate(void)
{
#define XTAL_CLK (27000000)
	unsigned int framerate = 0;

	ppoverlay_dvs_cnt_RBUS ppoverlay_dvs_cnt_Reg;
	ppoverlay_uzudtg_dvs_cnt_RBUS ppoverlay_uzudtg_dvs_cnt_Reg;
	if(VTDumpLocation == KADP_VT_SCALER_INPUT){
		framerate = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000)/1000;
		framerate = (framerate+5)/10;
	}else if(VTDumpLocation == KADP_VT_SUB_SCALER_INPUT){
		framerate = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_V_FREQ_1000)/1000;
		framerate = (framerate+5)/10;
	}else if(VTDumpLocation == KADP_VT_DISPLAY_OUTPUT || VTDumpLocation == KADP_VT_SCALER_OUTPUT){
		ppoverlay_uzudtg_dvs_cnt_Reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DVS_cnt_reg);
		framerate = (ppoverlay_uzudtg_dvs_cnt_Reg.uzudtg_dvs_cnt == 0) ? VT_FPS_OUTPUT : ((XTAL_CLK - 1)/ppoverlay_uzudtg_dvs_cnt_Reg.uzudtg_dvs_cnt + 1);
	}else{
		ppoverlay_dvs_cnt_Reg.regValue = IoReg_Read32(PPOVERLAY_DVS_cnt_reg);
		framerate = (ppoverlay_dvs_cnt_Reg.dvs_cnt == 0) ? VT_FPS_OUTPUT : ((XTAL_CLK - 1)/ppoverlay_dvs_cnt_Reg.dvs_cnt + 1);
	}

	if(get_vo_camera_flow_flag(Get_DisplayMode_Port(SLR_MAIN_DISPLAY))==1){
		if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_ACTIVE)
		{
			if(VTDumpLocation == KADP_VT_DISPLAY_OUTPUT || VTDumpLocation == KADP_VT_SCALER_OUTPUT)
			{
				if(get_vt_EnableFRCMode() == TRUE)
				{
					framerate = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000);
					framerate = ( framerate + 500 ) / 1000;
				}
				else
					framerate = VT_FPS_OUTPUT; /*fixed vt vfod 60 fps for VR case */
			}
			else{
				framerate = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DTG_MASTER_V_FREQ);
			}
		}
		else
		{
			framerate = Get_DISPLAY_REFRESH_RATE();
		}
	}

	return (unsigned short)framerate;
}

unsigned char HAL_VT_GetVideoFrameOutputDeviceFramerate(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, unsigned int *pVideoFrameOutputDeviceFramerate)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	if(get_vt_function() == TRUE) {
		*pVideoFrameOutputDeviceFramerate = Get_VFOD_FrameRate();
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
		} else if(get_mach_type() == MACH_ARCH_RTK2885P && dumpLocation == KADP_VT_SUB_SCALER_INPUT){
			rtd_pr_vt_notice("KADP_VT_SUB_SCALER_INPUT unsupport!\n");
			up(&VT_Semaphore);
			return FALSE;
		}else{
			curCapInfo.enable = 0; //close vfod first then rerun
			drvif_DC2H_dispD_CaptureConfig(curCapInfo);
			curCapInfo.enable = 1;

			curCapInfo.capSrc = (VT_CAP_SRC)dumpLocation;
			drvif_DC2H_dispD_CaptureConfig(curCapInfo);
			if(KADP_VT_SCALER_OUTPUT == dumpLocation || KADP_VT_SCALER_INPUT == dumpLocation)
				VFODState.bAppliedPQ = FALSE;
			else
				VFODState.bAppliedPQ = TRUE;

			VTDumpLocation = dumpLocation;

			if(VFODState.bFreezed == TRUE) {
				set_vfod_freezed(TRUE);//revert freezed status
			}
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
	if(get_mach_type() == MACH_ARCH_RTK2885P && dumpLocation == KADP_VT_SUB_SCALER_INPUT){
		rtd_pr_vt_notice("KADP_VT_SUB_SCALER_INPUT unsupport\n");
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
			pOutputInfo->maxRegion.y =0;
			pOutputInfo->maxRegion.w = curCapInfo.capWid;
			pOutputInfo->maxRegion.h = curCapInfo.capLen;

			x = main_active_h_start_end_reg.mh_act_sta;
			y =main_active_v_start_end_reg.mv_act_sta;
			w =main_active_h_start_end_reg.mh_act_end - main_active_h_start_end_reg.mh_act_sta;
			h = main_active_v_start_end_reg.mv_act_end - main_active_v_start_end_reg.mv_act_sta;

			//scaledown:
			x = x*curCapInfo.capWid;
			x = x/(main_den_h_start_end_Reg.mh_den_end -main_den_h_start_end_Reg.mh_den_sta);

			y = y*curCapInfo.capLen;
			y = y/(main_den_v_start_end_Reg.mv_den_end - main_den_v_start_end_Reg.mv_den_sta);

			w = w*curCapInfo.capWid;
			w = w/(main_den_h_start_end_Reg.mh_den_end -main_den_h_start_end_Reg.mh_den_sta);

			h = h*curCapInfo.capLen;
			h = h/(main_den_v_start_end_Reg.mv_den_end - main_den_v_start_end_Reg.mv_den_sta);

			pOutputInfo->activeRegion.x = x;
			pOutputInfo->activeRegion.y =y;
			pOutputInfo->activeRegion.w = w;
			pOutputInfo->activeRegion.h = h;
			pOutputInfo->stride = curCapInfo.capWid;
		} else if(KADP_VT_SCALER_OUTPUT==dumpLocation || KADP_VT_SCALER_INPUT == dumpLocation || KADP_VT_SUB_SCALER_INPUT == dumpLocation) {
			pOutputInfo->maxRegion.x = 0;
			pOutputInfo->maxRegion.y =0;
			pOutputInfo->maxRegion.w = curCapInfo.capWid;
			pOutputInfo->maxRegion.h = curCapInfo.capLen;

			pOutputInfo->activeRegion.x = 0;
			pOutputInfo->activeRegion.y =0;
			pOutputInfo->activeRegion.w = curCapInfo.capWid;
			pOutputInfo->activeRegion.h = curCapInfo.capLen;

			pOutputInfo->stride = curCapInfo.capWid;
		}
		up(&VT_Semaphore);
		return TRUE;
	} else {
		pOutputInfo->maxRegion.x = 0;
		pOutputInfo->maxRegion.y =0;
		pOutputInfo->maxRegion.w = 0;
		pOutputInfo->maxRegion.h = 0;

		pOutputInfo->activeRegion.x = 0;
		pOutputInfo->activeRegion.y =0;
		pOutputInfo->activeRegion.w =0;
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
	if(get_mach_type() == MACH_ARCH_RTK2885P && dumpLocation == KADP_VT_SUB_SCALER_INPUT){
		rtd_pr_vt_notice("KADP_VT_SUB_SCALER_INPUT unsupport\n");
		return FALSE;
	}

	down(&VT_Semaphore);
	rtd_pr_vt_notice("HAL_VT_SetVideoFrameOutputDeviceOutputRegion:output w=%d,output h=%d,dumplocation=%d\n", pOutputRegion->w,pOutputRegion->h,dumpLocation);

    if((pOutputRegion->w> _DISP_WID)||(pOutputRegion->w<VT_CAP_SCALE_DOWN_LIMIT_WIDTH)||(pOutputRegion->h> _DISP_LEN)||(pOutputRegion->h<VT_CAP_SCALE_DOWN_LIMIT_HEIGHT)) {
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
	if((drvif_scaler_get_abnormal_dvs_long_flag()==TRUE)||(drvif_scaler_get_abnormal_dvs_short_flag()==TRUE)){
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
		if((dumpLocation==VTDumpLocation)&&(pOutputRegion->w==curCapInfo.capWid)&&(pOutputRegion->h==curCapInfo.capLen)) {
			rtd_pr_vt_notice("all SetVideoFrameOutputDeviceOutputRegion is the same with now\n");
		} else if((pOutputRegion->x!=0)||(pOutputRegion->y!=0)) {
			rtd_pr_vt_notice("[warning]only support output region x y is 0\n");
			up(&VT_Semaphore);
			return FALSE;
		} else {
			curCapInfo.enable = 0; //close vfod first
			drvif_DC2H_dispD_CaptureConfig(curCapInfo);

            set_cap_buffer_size_by_AP(pOutputRegion->w, pOutputRegion->h);
            if(vtPrintMask & VT_PRINT_MASK_MAX_VT_BUF_NUMS)
                vt_cap_buf_nums = VT_CAP_BUF_NUMS;
            else
                vt_cap_buf_nums = get_vt_VtBufferNum();
			if(Capture_BufferMemInit_VT(vt_cap_buf_nums) == FALSE)
			{
				rtd_pr_vt_notice("VT init allocate memory fail,%s=%d \n", __FUNCTION__, __LINE__);
				up(&VT_Semaphore);
				return FALSE;
			}

			curCapInfo.capSrc = (VT_CAP_SRC)dumpLocation;
			curCapInfo.capWid = pOutputRegion->w;
			curCapInfo.capLen = pOutputRegion->h;

			curCapInfo.enable = 1;
			drvif_DC2H_dispD_CaptureConfig((VT_CUR_CAPTURE_INFO)curCapInfo);
			if(KADP_VT_SCALER_OUTPUT == dumpLocation || KADP_VT_SCALER_INPUT == dumpLocation)
				VFODState.bAppliedPQ = FALSE;
			else
				VFODState.bAppliedPQ = TRUE;

			VTDumpLocation = dumpLocation;
			vfod_capture_location = VTDumpLocation;

			if(VFODState.bFreezed == TRUE)
			{
				set_vfod_freezed(TRUE);//revert freezed status
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

        vtframerate = Get_VFOD_FrameRate();/*Get_VFOD_FrameRate not return 0*/

        if(get_vt_target_fps() == 0)
        {
            if(VFODState.framerateDivide != 0)
                waitvsyncframerate = vtframerate/VFODState.framerateDivide;
            else
                waitvsyncframerate = vtframerate;
        }
        else
        {
            waitvsyncframerate = get_vt_target_fps();
            if(waitvsyncframerate > vtframerate)
                waitvsyncframerate = vtframerate;
        }

        st_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg)/90;
        sleeptime = (1000/waitvsyncframerate);
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
    dc2h_rgb2yuv_dc2h_rgb2yuv_ctrl_RBUS  dc2h_rgb2yuv_ctrl_reg;
    dc2h_rgb2yuv_ctrl_reg.regValue = rtd_inl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg);
    if(curCapInfo.enable == TRUE){
        if(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_get_en_rgb2yuv(rtd_inl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg))==0)
        {
            if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
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
        else if((DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_get_en_rgb2yuv(rtd_inl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg))==1) || (DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_get_en_rgb2yuv(rtd_inl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg))==2))
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
	y =main_active_v_start_end_reg.mv_act_sta;
	w =main_active_h_start_end_reg.mh_act_end - main_active_h_start_end_reg.mh_act_sta;
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
		pOutputVideoInfo->maxRegion.y =0;
		pOutputVideoInfo->maxRegion.w = 0;
		pOutputVideoInfo->maxRegion.h = 0;

		pOutputVideoInfo->activeRegion.x = 0;
		pOutputVideoInfo->activeRegion.y =0;
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
        disable_dc2h_hw_dma();
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

unsigned char get_drm_case_securestatus(void)
{
    scpu_core_sc_dummy15_RBUS scpu_core_sc_dummy15_RBUS_reg;
    scpu_core_sc_dummy15_RBUS_reg.regValue = rtd_inl(SCPU_CORE_SC_DUMMY15_reg);

    if(vtPrintMask & VT_PRINT_MASK_DRM_LOG)
        rtd_pr_vt_notice("[VT]DRM 0xB805B0E4=%x \n",scpu_core_sc_dummy15_RBUS_reg.regValue);

    if(SCPU_CORE_SC_DUMMY15_scpu_dummy15(IoReg_Read32(SCPU_CORE_SC_DUMMY15_reg)) != 0){
        if(SCPU_CORE_SC_DUMMY15_scpu_dummy15(IoReg_Read32(SCPU_CORE_SC_DUMMY15_reg) == HDMI_HDCP_DRM_CASE) && Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_HDMI) //hdcp ta not disconnect,BIT2=1,already hdmi disconnect
            return FALSE;
        else if(SCPU_CORE_SC_DUMMY15_scpu_dummy15(IoReg_Read32(SCPU_CORE_SC_DUMMY15_reg) == DP_HDCP_DRM_CASE) && Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_DP)
            return FALSE;
        else
            return TRUE;
    }
    else{
        return FALSE;
    }
}

void set_dtv_securestatus(unsigned char status)
{
	if(TRUE == status)  //dtv security status
	{
        disable_dc2h_hw_dma();
	}

    DtvSecureStatus = status;
}

unsigned char get_svp_protect_status(void)
{
	if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC)
	{
		return (get_vdec_securestatus());
	}
	else
		return FALSE;
}
unsigned char HAL_VT_GetVideoFrameBufferSecureVideoState(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, unsigned char *pIsSecureVideo)
{
	if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
		rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	*pIsSecureVideo = get_drm_case_securestatus();
	// if (get_drm_case_securestatus() == TRUE) {
	// 	* pIsSecureVideo = get_drm_case_securestatus();
	// }
    // else {
	// 	* pIsSecureVideo = FALSE;
	// }
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
		if (value != get_VT_Pixel_Format()) {
			rtd_pr_vt_notice("%s=%d\n", __FUNCTION__, value);
			set_VT_Pixel_Format(value);

			curCapInfo.enable = 0; //iMode;
			drvif_DC2H_dispD_CaptureConfig(curCapInfo);
			Capture_BufferMemDeInit_VT(get_vt_VtBufferNum());
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
    uiTargetFps = outfps;
    rtd_pr_vt_notice("ap set outfps=%d\n", uiTargetFps);
    up(&VT_Semaphore);
    return TRUE;
}

unsigned char HAL_VT_Set_Enqueue_index(unsigned int idx)
{
    down(&VT_Semaphore_enq);
    spin_lock(&vt_buf_status_Spinlock);
    set_vt_buf_enqueue(idx);
    vt_dequeue_enqueue_count_subtract();
    spin_unlock(&vt_buf_status_Spinlock);
    if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX)
        rtd_pr_vt_notice("enqEnd 90K %d, %d\n", IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), vt_dequeue_enqueue_count);

    up(&VT_Semaphore_enq);
    return TRUE;
}

unsigned char HAL_VT_Get_Dequeue_index(KADP_VT_VIDEO_WINDOW_TYPE_T videoWindowID, unsigned int *pIndex)
{
    dc2h_dma_dc2h_seq_interrupt_enable_RBUS dc2h_dma_dc2h_seq_interrupt_enable_Reg;
    dc2h_vi_dc2h_dmactl_RBUS vi_dmactl_reg;
    dc2h_dma_dc2h_seq_mode_ctrl1_RBUS dc2h_seq_mode_ctrl1_reg;
    dc2h_vi_dc2h_vi_doublebuffer_RBUS dc2h_vi_dc2h_vi_doublebuffer_Reg;
    static unsigned char ucOnce_Drm_case_flag = FALSE;

    if(videoWindowID != KADP_VT_VIDEO_WINDOW_0) {
        rtd_pr_vt_notice("videoWindowID id is not ok;%s=%d\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    down(&VT_Semaphore);

    vi_dmactl_reg.regValue = IoReg_Read32(DC2H_VI_DC2H_DMACTL_reg);
#if 1
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
#else
// method 2:
// - 0xb8029d0c[9] = 1, enable color bar
// - 0xb8029d0c[10]= 1, enable auto switch

    if(DC2H_VI_DC2H_DMACTL_get_dummy_30_9(vi_dmactl_reg.regValue) == 3) {
        static unsigned int auto_switch_flag = 0;

        if(auto_switch_flag == 300)
            auto_switch_flag = 0;

        auto_switch_flag += 1;

        if(auto_switch_flag < 150) {
            static unsigned char idx_1 = 0;
            if(idx_1 > 2) idx_1 = 0;
            *pIndex = idx_1;
            // rtd_pr_vt_notice("###[%s(%d)] VT force return %d\n", __FUNCTION__, __LINE__,idx_1);
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
            disable_dc2h_hw_dma();
            *pIndex = IndexOfFreezedVideoFrameBuffer;
            spin_lock(&vt_buf_status_Spinlock);
            set_vt_buf_dequeue(IndexOfFreezedVideoFrameBuffer);
            //vt_dequeue_enqueue_count_add();
            spin_unlock(&vt_buf_status_Spinlock);
            if(vtPrintMask & VT_PRINT_MASK_DRM_LOG){
                rtd_pr_vt_notice("[VT]get_drm_case_securestatus is true;%s=%d\n", __FUNCTION__, vt_dequeue_enqueue_count);
            }
            ucOnce_Drm_case_flag = TRUE;
            up(&VT_Semaphore);
            return TRUE;
        }
        else{
            if((get_vt_VtSwBufferMode()) && (DC2H_DMA_dc2h_seq_Interrupt_Enable_vs_rising_ie(IoReg_Read32(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg) == 0)))
            {
               dc2h_dma_dc2h_seq_interrupt_enable_Reg.regValue = rtd_inl(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg);
               dc2h_dma_dc2h_seq_interrupt_enable_Reg.vs_rising_ie = 1;
               rtd_outl(DC2H_DMA_dc2h_seq_Interrupt_Enable_reg, dc2h_dma_dc2h_seq_interrupt_enable_Reg.regValue);
               msleep(20);
            }

            if(ucOnce_Drm_case_flag == TRUE){
                ucOnce_Drm_case_flag = FALSE;
               if((DC2H_VI_DC2H_DMACTL_dmaen1(IoReg_Read32(DC2H_VI_DC2H_DMACTL_reg)) == 0) || (DC2H_DMA_dc2h_Seq_mode_CTRL1_cap_en(IoReg_Read32(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg)) == 0))
               {
                    if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
                    {
                        vi_dmactl_reg.regValue = rtd_inl(DC2H_VI_DC2H_DMACTL_reg);
                        vi_dmactl_reg.dmaen1 = 1;
                        rtd_outl(DC2H_VI_DC2H_DMACTL_reg, vi_dmactl_reg.regValue);
                    }
                    else
                    {
                        dc2h_seq_mode_ctrl1_reg.regValue = rtd_inl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg);
                        dc2h_seq_mode_ctrl1_reg.cap_en = 1;
                        rtd_outl(DC2H_DMA_dc2h_Seq_mode_CTRL1_reg, dc2h_seq_mode_ctrl1_reg.regValue);
                    }

                    if(DC2H_VI_DC2H_vi_doublebuffer_get_vi_db_en(IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg)))
                    { //if double buffer enable
                        dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue = IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg);
                        dc2h_vi_dc2h_vi_doublebuffer_Reg.vi_db_apply = 1;
                        IoReg_Write32(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_dc2h_vi_doublebuffer_Reg.regValue);
                    }
               }
            }
        }

        if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
        {
            static unsigned char vt_first_dequeue_flag = TRUE;
            unsigned int bufnum = get_vt_VtBufferNum();

            if(get_vt_dequeue_enqueue_count() >= AP_MAX_HOLD_NUM)
            {
                *pIndex = VT_NOT_ENOUGH_RESOURCE;
                rtd_pr_vt_notice("dq at least %d (%d) buffers already,no buffer more\n", AP_MAX_HOLD_NUM, vt_dequeue_enqueue_count);
            }
            else
            {
                if(vt_first_dequeue_flag == TRUE)
                {
                    vt_first_dequeue_flag = FALSE;
                    IndexOfFreezedVideoFrameBuffer = (htonl(pVtBufStatus->write) + bufnum - 1)%bufnum;
                }
                else
                {
                    if(((htonl(pVtBufStatus->read) + 1)%bufnum) == htonl(pVtBufStatus->write))
                    {
                        *pIndex = VT_NOT_ENOUGH_RESOURCE;
                        //spin_lock(&vt_buf_status_Spinlock);
                        //set_vt_buf_dequeue(IndexOfFreezedVideoFrameBuffer);
                        //spin_unlock(&vt_buf_status_Spinlock);
                        rtd_pr_vt_notice("dequeue repeat index %d\n", IndexOfFreezedVideoFrameBuffer);
                        up(&VT_Semaphore);
                        return TRUE;
                    }
                    else
                    {
                        IndexOfFreezedVideoFrameBuffer = (htonl(pVtBufStatus->read) + 1)%bufnum;
                    }
                }

                *pIndex = IndexOfFreezedVideoFrameBuffer;
                spin_lock(&vt_buf_status_Spinlock);
                set_vt_buf_dequeue(IndexOfFreezedVideoFrameBuffer);
                vt_dequeue_enqueue_count_add();
                spin_unlock(&vt_buf_status_Spinlock);
                if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX){
                    rtd_pr_vt_notice("deq 90K %d, %d\n", IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), vt_dequeue_enqueue_count);
                }
                pVtBufStatus->read = Scaler_ChangeUINT32Endian(IndexOfFreezedVideoFrameBuffer);

                if(vtPrintMask & VT_PRINT_MASK_FRAME_INDEX){
                    rtd_pr_vt_notice("sta %d,%d,%d,%d,%d,dq_idx=%d,dmaen %d\n", \
                        htonl(pVtBufStatus->vtBufStatus[0]),\
                        htonl(pVtBufStatus->vtBufStatus[1]),\
                        htonl(pVtBufStatus->vtBufStatus[2]),\
                        htonl(pVtBufStatus->vtBufStatus[3]),\
                        htonl(pVtBufStatus->vtBufStatus[4]),\
                        IndexOfFreezedVideoFrameBuffer, DC2H_VI_DC2H_DMACTL_get_dmaen1(rtd_inl(DC2H_VI_DC2H_DMACTL_reg)));
                }
            }
        }
        else
        {
            *pIndex = VT_NOT_ENOUGH_RESOURCE;
            rtd_pr_vt_notice("[VT][pixelfmt]not enough resour\n");
        }

        up(&VT_Semaphore);
        return TRUE;
    }
    else
    {
        rtd_pr_vt_notice("VT is not Inited So return False;%s=%d\n", __FUNCTION__, __LINE__);
        up(&VT_Semaphore);
        return FALSE;
    }
}

unsigned char HAL_VT_Set_App_Case(VT_APP_CASE type)
{
    down(&VT_Semaphore);
    /*if(type == VT_APP_CASE_LIVE_STREAM)
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
    }*/
    up(&VT_Semaphore);
    return TRUE;
}

unsigned char HAL_VT_Set_Crop(KADP_VT_RECT_T cropSize)
{
	if((cropSize.x + cropSize.w) > _DISP_WID || (cropSize.y + cropSize.h) > _DISP_LEN){
		rtd_pr_vt_notice("cropSize out range(%d,%d,%d,%d),%s=%d \n",cropSize.x, cropSize.y,cropSize.w,cropSize.h, __FUNCTION__, __LINE__);
		return FALSE;
	}
	down(&VT_Semaphore);
	if(DC2H_RGB2YUV_DC2H_3DMaskTo2D_Ctrl_get_dc2h_3dmaskto2d_en(IoReg_Read32(DC2H_RGB2YUV_DC2H_3DMaskTo2D_Ctrl_reg)) == 0){
		curCapInfo.crop_size = cropSize;
		set_vt_crop_enable(TRUE);
		rtd_pr_vt_notice("cropSize(%d,%d,%d,%d)\n",curCapInfo.crop_size.x, curCapInfo.crop_size.y,curCapInfo.crop_size.w,curCapInfo.crop_size.h);
		if(curCapInfo.crop_size.x > 0){
			curCapInfo.crop_size.x = curCapInfo.crop_size.x - 1; //crop left right register algo: pix_cnt start from 1
		}
	}else{
		dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_RBUS dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_reg;
		if(cropSize.x == curCapInfo.crop_size.x && cropSize.y == curCapInfo.crop_size.y && cropSize.w == curCapInfo.crop_size.w && cropSize.h == curCapInfo.crop_size.h){
			rtd_pr_vt_notice("cropSize not changed(%d,%d,%d,%d)\n",cropSize.x, cropSize.y,cropSize.w,cropSize.h);
			up(&VT_Semaphore);
			return TRUE;
		}
		if(cropSize.w != curCapInfo.capWid || cropSize.h != curCapInfo.capLen){
			rtd_pr_vt_notice("cropSize wid or len changed(%d,%d,%d,%d)\n",cropSize.x, cropSize.y,cropSize.w,cropSize.h);
			up(&VT_Semaphore);
			return FALSE;
		}
		dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_reg.regValue = IoReg_Read32(DC2H_RGB2YUV_DC2H_3DMaskTo2D_DB_Ctrl_reg);
		if(dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_reg.color_db_en == 0){
			dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_reg.color_db_en = 1;
			IoReg_Write32(DC2H_RGB2YUV_DC2H_3DMaskTo2D_DB_Ctrl_reg, dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_reg.regValue);
		}
		drvif_dc2h_input_overscan_config(cropSize.x, cropSize.y, cropSize.w, cropSize.h);
		dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_reg.regValue = IoReg_Read32(DC2H_RGB2YUV_DC2H_3DMaskTo2D_DB_Ctrl_reg);
		dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_reg.color_db_apply = 1;
		IoReg_Write32(DC2H_RGB2YUV_DC2H_3DMaskTo2D_DB_Ctrl_reg, dc2h_rgb2yuv_dc2h_3dmaskto2d_db_ctrl_reg.regValue);
		curCapInfo.crop_size = cropSize;
	}
	up(&VT_Semaphore);
	return TRUE;
}

/************************************start of DC2H capture VDC(ATV or CVBS) data function********************************/
void rtk_dc2h_capturevdc_config(unsigned char connect, VIDEO_RECT_T inregion, VIDEO_RECT_T outregion)
{
	if (connect == TRUE) {
		curCapInfo.enable = 1; //iMode;
		curCapInfo.capSrc = 2;  //0:scaler output 1:display output 2:VD output
		curCapInfo.capMode = VT_CAP_MODE_SEQ;
		curCapInfo.capWid = outregion.w; //iWid;
		curCapInfo.capLen = outregion.h;

		DC2H_InputWidthSta = inregion.x;
		DC2H_InputLengthSta = inregion.y;
		DC2H_InputWidth = inregion.w;
		DC2H_InputLength = inregion.h;

		if (Capture_BufferMemInit_VT(3) == FALSE) {
			rtd_pr_vt_notice("VT init allocate memory fail;%s=%d \n", __FUNCTION__, __LINE__);
			up(&VT_Semaphore);
			return ;
		}
		set_vt_VtBufferNum(3);
		set_vt_VtSwBufferMode(FALSE);
		set_vt_VtCaptureVDC(TRUE);
		//freezebufferaddr = CaptureCtrl_VT.cap_buffer[2].phyaddr;
		drvif_DC2H_dispD_CaptureConfig(curCapInfo);
	} else {
		curCapInfo.enable = 0; //iMode;
		drvif_DC2H_dispD_CaptureConfig(curCapInfo);
		Capture_BufferMemDeInit_VT(get_vt_VtBufferNum());
		set_vt_VtBufferNum(0);
		set_vt_VtSwBufferMode(FALSE);
		set_vt_VtCaptureVDC(FALSE);
	}
}
/************************************end of DC2H capture VDC(ATV or CVBS) data function********************************/


/************************************start of VT buffer show by sub display function********************************/
//#define _BURSTLENGTH2 	(0x78) // 0x78 (120) for sub-channel
//#define _FIFOLENGTH2 	(0x80) //sub path FIFO
#if 0 //nonlibdma mdomain de-access
void memory_set_sub_displaywindow_DispVT(VIDEO_RECT_T s_dispwin)
{
	mdomain_disp_ddr_sublinestep_RBUS mdomain_disp_ddr_sublinestep_reg;
	mdomain_disp_ddr_sublinenum_RBUS mdomain_disp_ddr_sublinenum_reg;
	mdomain_disp_ddr_subrdnumlenrem_RBUS mdomain_disp_ddr_subrdnumlenrem_reg;
	mdomain_disp_ddr_subpixnumwtl_RBUS mdomain_disp_ddr_subpixnumwtl_reg;
	SCALER_DISP_CHANNEL display = SLR_SUB_DISPLAY;
	unsigned int TotalSize;
	unsigned int Quotient;
	unsigned char Remainder;
	unsigned int fifoLen = _FIFOLENGTH2;
	unsigned int burstLen = _BURSTLENGTH2;
	unsigned int MemShiftAddr = 0;
	unsigned short droppixel = 0;
	unsigned int subctrltemp = 0;
	mdomain_disp_ddr_subprevstart_RBUS mdomain_disp_ddr_subprevstart_Reg;
	mdomain_disp_display_sub_byte_channel_swap_RBUS display_sub_byte_channel_swap_reg;

	mdomain_disp_ddr_subprevstart_Reg.regValue = rtd_inl(MDOMAIN_DISP_DDR_SubPreVStart_reg);
	if(s_dispwin.y <= 100) {
		mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = 7;
	} else {
		mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = (s_dispwin.y-4);
	}
	rtd_outl(MDOMAIN_DISP_DDR_SubPreVStart_reg, mdomain_disp_ddr_subprevstart_Reg.regValue);


	MemGetBlockShiftAddr(display, Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_HSTA),Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_VSTA), &MemShiftAddr, &droppixel);
	TotalSize = memory_get_disp_line_size1(display, Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID) + droppixel);	// calculate the memory size of capture
	mdomain_disp_ddr_subpixnumwtl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubPixNumWTL_reg);
	mdomain_disp_ddr_subpixnumwtl_reg.sub_pixel_num = (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID) +droppixel) & 0x3FFF;
	mdomain_disp_ddr_subpixnumwtl_reg.sub_wtlvl = ((fifoLen - (burstLen>>1))>>1);
	IoReg_Write32(MDOMAIN_DISP_DDR_SubPixNumWTL_reg, mdomain_disp_ddr_subpixnumwtl_reg.regValue);

	memory_division(TotalSize, burstLen, &Quotient, &Remainder);	// Do a division

	// remainder is not allowed to be zero
	if (Remainder == 0) {
		Remainder = burstLen;
		Quotient -= 1;
	}


	if((dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE) && (dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_FRAME_MODE))
	{
		mdomain_disp_ddr_subrdnumlenrem_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg);
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_num1 = 0xffff;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_len = burstLen;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_remain = burstLen;
		IoReg_Write32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg, mdomain_disp_ddr_subrdnumlenrem_reg.regValue);
	}
	else
	{
		mdomain_disp_ddr_subrdnumlenrem_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg);
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_num1 = Quotient;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_len = burstLen;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_remain = Remainder;
		IoReg_Write32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg, mdomain_disp_ddr_subrdnumlenrem_reg.regValue);
	}

	TotalSize = memory_get_capture_size(display, MEMCAPTYPE_LINE);
	/*TotalSize = Quotient * burstLen + Remainder; 	// count one line, 64bits unit
	TotalSize = drvif_memory_get_data_align(TotalSize, 4);	// times of 4*/

	mdomain_disp_ddr_sublinestep_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubLineStep_reg);
	mdomain_disp_ddr_sublinestep_reg.sub_line_step = (TotalSize + TotalSize%2);//bit 3 need to be 0. rbus rule
	IoReg_Write32(MDOMAIN_DISP_DDR_SubLineStep_reg, mdomain_disp_ddr_sublinestep_reg.regValue);

	TotalSize = SHL(TotalSize, 3); // unit conversion from 64bits to 8bits

	mdomain_disp_ddr_sublinenum_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubLineNum_reg);
	mdomain_disp_ddr_sublinenum_reg.sub_line_number = (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN) & 0x0FFF);
	IoReg_Write32(MDOMAIN_DISP_DDR_SubLineNum_reg, mdomain_disp_ddr_sublinenum_reg.regValue);

	rtd_outl(MDOMAIN_DISP_DDR_SubAddr_reg, (CaptureCtrl_VT.cap_buffer[0].phyaddr + MemShiftAddr) &0xfffffff0);
	rtd_outl(MDOMAIN_DISP_DDR_Sub2ndAddr_reg, (CaptureCtrl_VT.cap_buffer[1].phyaddr + MemShiftAddr) &0xfffffff0);
	rtd_outl(MDOMAIN_DISP_DDR_Sub3rdAddr_reg, (CaptureCtrl_VT.cap_buffer[2].phyaddr + MemShiftAddr) &0xfffffff0);

	rtd_outl(MDOMAIN_DISP_DDR_SubAddrDropBits_reg,droppixel);

	subctrltemp = _BIT6;
	subctrltemp |=_BIT7;//set Mdomain display triple buffer
	//subctrltemp |= (_BIT1);
	//disp ctrl default setting
	subctrltemp |= (_BIT25 | _BIT28);
	subctrltemp |= Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) ? 0 : _BIT17;
	//subctrltemp |= (_BIT19);/*for VT data is RGB*/
	rtd_outl(MDOMAIN_DISP_DDR_SubCtrl_reg, subctrltemp);

	display_sub_byte_channel_swap_reg.regValue = rtd_inl(MDOMAIN_DISP_Display_sub_byte_channel_swap_reg);
	display_sub_byte_channel_swap_reg.sub_1byte_swap = 1;
	display_sub_byte_channel_swap_reg.sub_2byte_swap = 1;
	display_sub_byte_channel_swap_reg.sub_4byte_swap = 1;
	display_sub_byte_channel_swap_reg.sub_8byte_swap = 0;
	display_sub_byte_channel_swap_reg.sub_channel_swap = 4;
	rtd_outl(MDOMAIN_DISP_Display_sub_byte_channel_swap_reg, display_sub_byte_channel_swap_reg.regValue);
	rtd_outl(MDOMAIN_DISP_Disp_sub_enable_reg, _BIT0);
}
#endif

extern void magnifier_color_ultrazoom_config_scaling_up(unsigned char display);
extern void Scaler_disp_setting(unsigned char display);
unsigned char Scaler_SubMDispWindow_VFB(bool enable)
{
	rtd_pr_vt_debug("Scaler_SubMDispWindow_VFB curCapInfo.enable==%d\n",curCapInfo.enable);
	if (curCapInfo.enable){
		SCALER_DISP_CHANNEL display = SLR_SUB_DISPLAY;
		//mdomain_disp_ddr_mainsubctrl_RBUS mdomain_disp_ddr_mainsubctrl_Reg;
		ppoverlay_double_buffer_ctrl_RBUS ppoverlay_double_buffer_ctrl_Reg;
		scaleup_ds_uzu_db_ctrl_RBUS scaleup_ds_uzu_db_ctrl_Reg;
		//IoReg_SetBits(MDOMAIN_DISP_DDR_MainSubCtrl_reg, _BIT17);
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//mdomain_disp_ddr_mainsubctrl_Reg.regValue = rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
			//mdomain_disp_ddr_mainsubctrl_Reg.disp2_double_enable = 0;
			//rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg, mdomain_disp_ddr_mainsubctrl_Reg.regValue);
			nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_enable(1, 0);
		}

		//IoReg_SetBits(PPOVERLAY_Double_Buffer_CTRL_reg, _BIT6);//Enable dtg double buffer register
		ppoverlay_double_buffer_ctrl_Reg.regValue = rtd_inl(PPOVERLAY_Double_Buffer_CTRL_reg);
		ppoverlay_double_buffer_ctrl_Reg.dsubreg_dbuf_en = 0;
		rtd_outl(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_Reg.regValue);

        m8p_dtg_set_sub_double_buffer_disable(DOUBLE_BUFFER_D3);

		//IoReg_SetBits(SCALEUP_DS_UZU_DB_CTRL_reg, _BIT2);//Enable uzu double buffer register
		scaleup_ds_uzu_db_ctrl_Reg.regValue = rtd_inl(SCALEUP_DS_UZU_DB_CTRL_reg);
		scaleup_ds_uzu_db_ctrl_Reg.db_en = 0;
		rtd_outl(SCALEUP_DS_UZU_DB_CTRL_reg, scaleup_ds_uzu_db_ctrl_Reg.regValue);
		if(enable == TRUE) {
			unsigned short VTCapLen = curCapInfo.capLen;
			unsigned short VTCapWid = curCapInfo.capWid;
			VIDEO_RECT_T outputwin;
			yuv2rgb_d_yuv2rgb_control_RBUS yuv2rgb_d_yuv2rgb_control_Reg;

			outputwin.x = 0;
			outputwin.y = 0;
			outputwin.w = VTCapWid;
			outputwin.h = VTCapLen;



			Scaler_DispSetStatus(display, SLR_DISP_422CAP, FALSE);/*VT buffer is RGB888,so go 444*/
			Scaler_DispSetStatus(display, SLR_DISP_10BIT, FALSE);/*VT buffer is RGB888,so go 8bit*/
			Scaler_DispSetStatus(display, SLR_DISP_INTERLACE, FALSE);

			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_FRAMESYNC, FALSE);

			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_CAP_LEN, VTCapLen);
			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_CAP_WID, VTCapWid);

			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN, VTCapLen);
			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_VSTA, 0);

			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID, VTCapWid);
			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_HSTA, 0);

			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_DISP_WID, VTCapWid);
			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_DISP_LEN, VTCapLen);

			if(Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID) <= Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DISP_WID)){
				Scaler_DispSetScaleStatus(display, SLR_SCALE_H_UP, TRUE);
			}else{
				Scaler_DispSetScaleStatus(display, SLR_SCALE_H_UP, FALSE);
			}

			if(Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN) <= Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DISP_LEN))
				Scaler_DispSetScaleStatus(display, SLR_SCALE_V_UP, TRUE);
			else
				Scaler_DispSetScaleStatus(display, SLR_SCALE_V_UP, FALSE);

			//merlin8/8p mdomain one-bin
			if(get_mach_type() == MACH_ARCH_RTK2885P) {
				//memory_set_sub_displaywindow_DispVT(outputwin);
				nonlibdma_memory_set_sub_displaywindow_DispVT(outputwin);
			}

			/*sub uzu*/
			magnifier_color_ultrazoom_config_scaling_up(display);

			/*sub yuv2rgb*/
			yuv2rgb_d_yuv2rgb_control_Reg.regValue = rtd_inl(YUV2RGB_D_YUV2RGB_Control_reg);
			if(curCapInfo.capSrc == 1) {//memc
				yuv2rgb_d_yuv2rgb_control_Reg.yuv2rgb_overlay = 1;
				yuv2rgb_d_yuv2rgb_control_Reg.yuv2rgb_sub_en = 0;
			} else{//uzu
				yuv2rgb_d_yuv2rgb_control_Reg.yuv2rgb_overlay = 0;
				yuv2rgb_d_yuv2rgb_control_Reg.yuv2rgb_sub_en = 1;
			}
			rtd_outl(YUV2RGB_D_YUV2RGB_Control_reg, yuv2rgb_d_yuv2rgb_control_Reg.regValue);

			PipmpSetSubDisplayWindow(
				outputwin.x, outputwin.x + Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DISP_WID) - 1,
				outputwin.y, outputwin.y + Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DISP_LEN) - 1,
				0);

			PipmpSetSubActiveWindow(
				0, Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY,SLR_INPUT_DISP_WID)  - 1,
				0, Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY,SLR_INPUT_DISP_LEN) - 1);

			Scaler_disp_setting(display);

			set_display_forcebg_mask(SLR_SUB_DISPLAY, SLR_FORCE_BG_TYPE_VSC, FALSE);
			mute_control(SLR_SUB_DISPLAY, FALSE);
		} else {
			//merlin8/8p mdomain one-bin
			if(get_mach_type() == MACH_ARCH_RTK2885P) {
				//rtd_outl(MDOMAIN_DISP_Disp_sub_enable_reg, 0x00000000);
				nonlibdma_set_disp_enable_reg_disp_en(1, 0);
			}

			down(get_forcebg_semaphore());
			drvif_scalerdisplay_enable_display(SLR_SUB_DISPLAY, _DISABLE);
			up(get_forcebg_semaphore());
		}
	} else {
		return FALSE;
	}
	return TRUE;
}

#if 0 //nonlibdma mdomain de-access
void memory_set_displaysubwindow_DispVT(VIDEO_RECT_T s_dispwin, unsigned long phyaddr)
{
	mdomain_disp_ddr_sublinestep_RBUS mdomain_disp_ddr_sublinestep_reg;
	mdomain_disp_ddr_sublinenum_RBUS mdomain_disp_ddr_sublinenum_reg;
	mdomain_disp_ddr_subrdnumlenrem_RBUS mdomain_disp_ddr_subrdnumlenrem_reg;
	mdomain_disp_ddr_subpixnumwtl_RBUS mdomain_disp_ddr_subpixnumwtl_reg;
	SCALER_DISP_CHANNEL display = SLR_SUB_DISPLAY;
	unsigned int TotalSize;
	unsigned int Quotient;
	unsigned char Remainder;
	unsigned int fifoLen = _FIFOLENGTH2;
	unsigned int burstLen = _BURSTLENGTH2;
	unsigned int MemShiftAddr = 0;
	unsigned short droppixel = 0;
	unsigned int subctrltemp = 0;
	mdomain_disp_ddr_subprevstart_RBUS mdomain_disp_ddr_subprevstart_Reg;
	mdomain_disp_display_sub_byte_channel_swap_RBUS display_sub_byte_channel_swap_reg;

	mdomain_disp_ddr_subprevstart_Reg.regValue = rtd_inl(MDOMAIN_DISP_DDR_SubPreVStart_reg);
	if(s_dispwin.y <= 100) {
		mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = 7;
	} else {
		mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = (s_dispwin.y-4);
	}
	rtd_outl(MDOMAIN_DISP_DDR_SubPreVStart_reg, mdomain_disp_ddr_subprevstart_Reg.regValue);


	MemGetBlockShiftAddr(display, Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_HSTA),Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_VSTA), &MemShiftAddr, &droppixel);
	TotalSize = memory_get_disp_line_size1(display, Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID) + droppixel);	// calculate the memory size of capture

	mdomain_disp_ddr_subpixnumwtl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubPixNumWTL_reg);
	mdomain_disp_ddr_subpixnumwtl_reg.sub_pixel_num = (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID) +droppixel) & 0x3FFF;
	mdomain_disp_ddr_subpixnumwtl_reg.sub_wtlvl = ((fifoLen - (burstLen>>1))>>1);
	IoReg_Write32(MDOMAIN_DISP_DDR_SubPixNumWTL_reg, mdomain_disp_ddr_subpixnumwtl_reg.regValue);

	memory_division(TotalSize, burstLen, &Quotient, &Remainder);	// Do a division

	// remainder is not allowed to be zero
	if (Remainder == 0) {
		Remainder = burstLen;
		Quotient -= 1;
	}

	if((dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE) && (dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_FRAME_MODE))
	{
		mdomain_disp_ddr_subrdnumlenrem_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg);
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_num1 = 0xffff;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_len = burstLen;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_remain = burstLen;
		IoReg_Write32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg, mdomain_disp_ddr_subrdnumlenrem_reg.regValue);
	}
	else
	{
		mdomain_disp_ddr_subrdnumlenrem_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg);
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_num1 = Quotient;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_len = burstLen;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_remain = Remainder;
		IoReg_Write32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg, mdomain_disp_ddr_subrdnumlenrem_reg.regValue);
	}


	TotalSize = memory_get_capture_size(display, MEMCAPTYPE_LINE);
	/*TotalSize = Quotient * burstLen + Remainder; 	// count one line, 64bits unit
	TotalSize = drvif_memory_get_data_align(TotalSize, 4);	// times of 4*/


	mdomain_disp_ddr_sublinestep_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubLineStep_reg);
	mdomain_disp_ddr_sublinestep_reg.sub_line_step = (TotalSize + TotalSize%2);//bit 3 need to be 0. rbus rule
	IoReg_Write32(MDOMAIN_DISP_DDR_SubLineStep_reg, mdomain_disp_ddr_sublinestep_reg.regValue);

	TotalSize = SHL(TotalSize, 3); // unit conversion from 64bits to 8bits

	mdomain_disp_ddr_sublinenum_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubLineNum_reg);
	mdomain_disp_ddr_sublinenum_reg.sub_line_number = (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN) & 0x0FFF);
	IoReg_Write32(MDOMAIN_DISP_DDR_SubLineNum_reg, mdomain_disp_ddr_sublinenum_reg.regValue);

#if 1
	rtd_outl(MDOMAIN_DISP_DDR_SubAddr_reg, (phyaddr + MemShiftAddr) &0xfffffff0);
#else
	rtd_outl(MDOMAIN_DISP_DDR_SubAddr_reg, (CaptureCtrl_VT.cap_buffer[0].phyaddr + MemShiftAddr) & 0xfffffff0);
	rtd_outl(MDOMAIN_DISP_DDR_Sub2ndAddr_reg, (CaptureCtrl_VT.cap_buffer[1].phyaddr + MemShiftAddr) & 0xfffffff0);
	rtd_outl(MDOMAIN_DISP_DDR_Sub3rdAddr_reg, (CaptureCtrl_VT.cap_buffer[2].phyaddr + MemShiftAddr) & 0xfffffff0);
#endif

	rtd_outl(MDOMAIN_DISP_DDR_SubAddrDropBits_reg,droppixel);

//	subctrltemp = _BIT6;
//	subctrltemp |=_BIT7;//set Mdomain display triple buffer
	//subctrltemp |= (_BIT1);
	//disp ctrl default setting
	subctrltemp |= (_BIT25 | _BIT28);
	subctrltemp |= Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) ? 0 : _BIT17;
	//subctrltemp |= (_BIT19);/*for VT data is RGB*/
	rtd_outl(MDOMAIN_DISP_DDR_SubCtrl_reg, subctrltemp);

	display_sub_byte_channel_swap_reg.regValue = rtd_inl(MDOMAIN_DISP_Display_sub_byte_channel_swap_reg);
	display_sub_byte_channel_swap_reg.sub_1byte_swap = 1;
	display_sub_byte_channel_swap_reg.sub_2byte_swap = 1;
	display_sub_byte_channel_swap_reg.sub_4byte_swap = 1;
	display_sub_byte_channel_swap_reg.sub_8byte_swap = 0;
	display_sub_byte_channel_swap_reg.sub_channel_swap = 4;
	rtd_outl(MDOMAIN_DISP_Display_sub_byte_channel_swap_reg, display_sub_byte_channel_swap_reg.regValue);
	rtd_outl(MDOMAIN_DISP_Disp_sub_enable_reg, _BIT0);
}
#endif

unsigned char Scaler_Rowdata_SubMDispWindow(KADP_VT_ROWDATA_SHOW_ONSUBDISP_T *VT_ROWDATA_SHOW_ONSUBDISP)
{
		ppoverlay_memc_mux_ctrl_bg_RBUS memc_mux_ctrl_bg_reg;
		SCALER_DISP_CHANNEL display = SLR_SUB_DISPLAY;
		//mdomain_disp_ddr_mainsubctrl_RBUS mdomain_disp_ddr_mainsubctrl_Reg;
		ppoverlay_double_buffer_ctrl_RBUS ppoverlay_double_buffer_ctrl_Reg;
		scaleup_ds_uzu_db_ctrl_RBUS scaleup_ds_uzu_db_ctrl_Reg;

		memc_mux_ctrl_bg_reg.regValue = rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_BG_reg);
		memc_mux_ctrl_bg_reg.memc_out_bg_en = 0;
		rtd_outl(PPOVERLAY_MEMC_MUX_CTRL_BG_reg, memc_mux_ctrl_bg_reg.regValue);
		//IoReg_SetBits(MDOMAIN_DISP_DDR_MainSubCtrl_reg, _BIT17);
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//mdomain_disp_ddr_mainsubctrl_Reg.regValue = rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
			//mdomain_disp_ddr_mainsubctrl_Reg.disp2_double_enable = 0;
			//rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg, mdomain_disp_ddr_mainsubctrl_Reg.regValue);
			nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_enable(1, 0);
		}

		//IoReg_SetBits(PPOVERLAY_Double_Buffer_CTRL_reg, _BIT6);//Enable dtg double buffer register
		ppoverlay_double_buffer_ctrl_Reg.regValue = rtd_inl(PPOVERLAY_Double_Buffer_CTRL_reg);
		ppoverlay_double_buffer_ctrl_Reg.dsubreg_dbuf_en = 0;
		rtd_outl(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_Reg.regValue);

        m8p_dtg_set_sub_double_buffer_disable(DOUBLE_BUFFER_D3);

		//IoReg_SetBits(SCALEUP_DS_UZU_DB_CTRL_reg, _BIT2);//Enable uzu double buffer register
		scaleup_ds_uzu_db_ctrl_Reg.regValue = rtd_inl(SCALEUP_DS_UZU_DB_CTRL_reg);
		scaleup_ds_uzu_db_ctrl_Reg.db_en = 0;
		rtd_outl(SCALEUP_DS_UZU_DB_CTRL_reg, scaleup_ds_uzu_db_ctrl_Reg.regValue);
		if(VT_ROWDATA_SHOW_ONSUBDISP->enable == TRUE) {
			unsigned short VTCapLen = VT_ROWDATA_SHOW_ONSUBDISP->height;
			unsigned short VTCapWid = VT_ROWDATA_SHOW_ONSUBDISP->width;
			VIDEO_RECT_T outputwin;
			yuv2rgb_d_yuv2rgb_control_RBUS yuv2rgb_d_yuv2rgb_control_Reg;

			outputwin.x = VT_ROWDATA_SHOW_ONSUBDISP->OutputRegion.x;
			outputwin.y = VT_ROWDATA_SHOW_ONSUBDISP->OutputRegion.y;
			outputwin.w = VT_ROWDATA_SHOW_ONSUBDISP->OutputRegion.w;
			outputwin.h = VT_ROWDATA_SHOW_ONSUBDISP->OutputRegion.h;

			Scaler_DispSetStatus(display, SLR_DISP_422CAP, FALSE);/*VT buffer is RGB888,so go 444*/
			Scaler_DispSetStatus(display, SLR_DISP_10BIT, FALSE);/*VT buffer is RGB888,so go 8bit*/
			Scaler_DispSetStatus(display, SLR_DISP_INTERLACE, FALSE);

			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_FRAMESYNC, FALSE);

			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_CAP_LEN, VTCapLen);
			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_CAP_WID, VTCapWid);

			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN, VTCapLen);
			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_VSTA, 0);

			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID, VTCapWid);
			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_HSTA, 0);

			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_DISP_WID, outputwin.w);
			Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_DISP_LEN, outputwin.h);

			if(Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID) <= Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DISP_WID)){
				Scaler_DispSetScaleStatus(display, SLR_SCALE_H_UP, TRUE);
			}else{
				Scaler_DispSetScaleStatus(display, SLR_SCALE_H_UP, FALSE);
			}

			if(Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN) <= Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DISP_LEN))
				Scaler_DispSetScaleStatus(display, SLR_SCALE_V_UP, TRUE);
			else
				Scaler_DispSetScaleStatus(display, SLR_SCALE_V_UP, FALSE);

			//merlin8/8p mdomain one-bin
			if(get_mach_type() == MACH_ARCH_RTK2885P) {
				//memory_set_displaysubwindow_DispVT(outputwin, VT_ROWDATA_SHOW_ONSUBDISP->phyaddr);
				nonlibdma_memory_set_displaysubwindow_DispVT(outputwin, VT_ROWDATA_SHOW_ONSUBDISP->phyaddr);
			}

			/*sub uzu*/
			magnifier_color_ultrazoom_config_scaling_up(display);

			/*sub yuv2rgb*/
			yuv2rgb_d_yuv2rgb_control_Reg.regValue = rtd_inl(YUV2RGB_D_YUV2RGB_Control_reg);
			yuv2rgb_d_yuv2rgb_control_Reg.yuv2rgb_overlay = 1;
			yuv2rgb_d_yuv2rgb_control_Reg.yuv2rgb_sub_en = 0;
			rtd_outl(YUV2RGB_D_YUV2RGB_Control_reg, yuv2rgb_d_yuv2rgb_control_Reg.regValue);

			PipmpSetSubDisplayWindow(
				outputwin.x, outputwin.x + Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DISP_WID) - 1,
				outputwin.y, outputwin.y + Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DISP_LEN) - 1,
				0);

			PipmpSetSubActiveWindow(
				0, Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY,SLR_INPUT_DISP_WID)  - 1,
				0, Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY,SLR_INPUT_DISP_LEN) - 1);

			Scaler_disp_setting(display);

			set_display_forcebg_mask(SLR_SUB_DISPLAY, SLR_FORCE_BG_TYPE_VSC, FALSE);
			mute_control(SLR_SUB_DISPLAY, FALSE);
		} else {
			//merlin8/8p mdomain one-bin
			if(get_mach_type() == MACH_ARCH_RTK2885P) {
				//rtd_outl(MDOMAIN_DISP_Disp_sub_enable_reg, 0x00000000);
				nonlibdma_set_disp_enable_reg_disp_en(1, 0);
			}

			down(get_forcebg_semaphore());
			drvif_scalerdisplay_enable_display(SLR_SUB_DISPLAY, _DISABLE);
			up(get_forcebg_semaphore());
		}
	return TRUE;
}

/************************************end of VT buffer show by sub display function********************************/



unsigned char ColorBlock_Compare(unsigned long bufferaddr, unsigned int R, unsigned int G, unsigned int B, VIDEO_RECT_T block)
{

	unsigned int sumR, sumG, sumB;
	unsigned int i,j;
	unsigned char* bufferpoint;

	if((block.w==0)&&(block.h==0))
		return TRUE;

	sumR = 0;
	sumG = 0;
	sumB = 0;
	bufferpoint = (unsigned char*)bufferaddr;

	for(i=0;i<block.h;i++)
	{
		for(j=0;j<block.w;j++)
		{
			sumR += bufferpoint[1920*3*(i+block.y)+(block.x+j)*3];
			sumG += bufferpoint[1920*3*(i+block.y)+(block.x+j)*3+1];
			sumB += bufferpoint[1920*3*(i+block.y)+(block.x+j)*3+2];
		}
	}
	sumR = sumR/(block.w*block.h);
	sumG = sumG/(block.w*block.h);
	sumB = sumB/(block.w*block.h);
	rtd_pr_vt_emerg("@@@@@@@");
	rtd_pr_vt_emerg("R=%d;sumR=%d\n",R,sumR);
	rtd_pr_vt_emerg("G=%d;sumG=%d\n",G,sumG);
	rtd_pr_vt_emerg("B=%d;sumB=%d\n",B,sumB);

	if((abs(R-sumR)<80)&&(abs(G-sumG)<80)&&(abs(B-sumB)<80))
		return TRUE;
	else{
		if(abs(R-sumR)>=80)
			rtd_pr_vt_emerg("!!!!!!@@@R_Diff_value =%d\n",(int)abs(R-sumR));
		if(abs(G-sumR)>=80)
			rtd_pr_vt_emerg("!!!!!!@@@G_Diff_value =%d\n",(int)abs(G-sumR));
		if(abs(B-sumR)>=80)
			rtd_pr_vt_emerg("!!!!!!@@@B_Diff_value =%d\n",(int)abs(B-sumR));
		return FALSE;
	}
}

extern unsigned char rtk_hal_vsc_GetInputRegion(KADP_VIDEO_WID_T wid, KADP_VIDEO_RECT_T * pinregion);
unsigned char Check_colorbar_window_cal(VIDEO_RECT_T *GrayBlock, VIDEO_RECT_T *YellowBlock, VIDEO_RECT_T *LittleBlueBlock, VIDEO_RECT_T *GreenBlock, VIDEO_RECT_T *PinkBlock, VIDEO_RECT_T *RedBlock, VIDEO_RECT_T *BlueBlock, VIDEO_RECT_T *BlackBlock)
{
	unsigned int SourceWidth;
	unsigned int OverscanWidth;
	KADP_VIDEO_RECT_T inregion;
	unsigned int input_w;
	unsigned int nooverscancolorbarWidth;
	unsigned int overscancolorbarWidth;
	unsigned int colorbarnum;
	unsigned int sidecolorwidth;
	VSC_INPUT_TYPE_T srctype;
	srctype = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);

	SourceWidth = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPH_ACT_WID_PRE);
	rtk_hal_vsc_GetInputRegion(KADP_VIDEO_WID_0, &inregion);
	input_w = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IHWID);
	if(input_w > 3840){
		inregion.x = 3840*inregion.x/input_w;
		inregion.w = 3840*inregion.w/input_w;
	}

	if ((VSC_INPUTSRC_AVD == srctype))
	{
		OverscanWidth = Scaler_CalAVD27MWidth(SLR_MAIN_DISPLAY,inregion.w);
	} else {
		OverscanWidth = inregion.w;
	}

	nooverscancolorbarWidth = SourceWidth/8;
	overscancolorbarWidth = 1920*nooverscancolorbarWidth/OverscanWidth;
	sidecolorwidth = (960%overscancolorbarWidth)?(960%overscancolorbarWidth):overscancolorbarWidth;
	colorbarnum =( 8 - 2*((SourceWidth - OverscanWidth) / (nooverscancolorbarWidth*2)));

	rtd_pr_vt_emerg("SourceWidth=%d;OverscanWidth=%d\n",SourceWidth,OverscanWidth);
	rtd_pr_vt_emerg("overscancolorbarWidth=%d;sidecolorwidth=%d\n",overscancolorbarWidth,sidecolorwidth);
	rtd_pr_vt_emerg("colorbarnum=%d\n",colorbarnum);

	if(8 == colorbarnum ){
		/*line one*/
		GreenBlock[0].x = 960 - overscancolorbarWidth/2 -5;
		GreenBlock[0].y = 175;
		GreenBlock[0].w = 10;
		GreenBlock[0].h = 10;

		PinkBlock[0].x = 960 + overscancolorbarWidth/2 -5;
		PinkBlock[0].y = 175;
		PinkBlock[0].w = 10;
		PinkBlock[0].h = 10;

		LittleBlueBlock[0].x = 960 - 3*overscancolorbarWidth/2 -5;
		LittleBlueBlock[0].y = 175;
		LittleBlueBlock[0].w = 10;
		LittleBlueBlock[0].h = 10;

		RedBlock[0].x = 960 + 3*overscancolorbarWidth/2 -5;
		RedBlock[0].y = 175;
		RedBlock[0].w = 10;
		RedBlock[0].h = 10;

		YellowBlock[0].x = 960 - 5*overscancolorbarWidth/2 -5;
		YellowBlock[0].y = 175;
		YellowBlock[0].w = 10;
		YellowBlock[0].h = 10;

		BlueBlock[0].x = 960 + 5*overscancolorbarWidth/2 -5;
		BlueBlock[0].y = 175;
		BlueBlock[0].w = 10;
		BlueBlock[0].h = 10;

		if(sidecolorwidth >= 30){
			GrayBlock[0].x = 960 - 3*overscancolorbarWidth - sidecolorwidth/2 -5;
			GrayBlock[0].y = 175;
			GrayBlock[0].w = 10;
			GrayBlock[0].h = 10;

			BlackBlock[0].x = 960 + 3*overscancolorbarWidth + sidecolorwidth/2 -5;
			BlackBlock[0].y = 175;
			BlackBlock[0].w = 10;
			BlackBlock[0].h = 10;
		} else {
			GrayBlock[0].x = 0;
			GrayBlock[0].y = 0;
			GrayBlock[0].w = 0;
			GrayBlock[0].h = 0;

			BlackBlock[0].x = 0;
			BlackBlock[0].y = 0;
			BlackBlock[0].w = 0;
			BlackBlock[0].h = 0;
		}

		/*line two*/
		GreenBlock[1].x = 960 - overscancolorbarWidth/2 -5;
		GreenBlock[1].y = 535;
		GreenBlock[1].w = 10;
		GreenBlock[1].h = 10;

		PinkBlock[1].x = 960 + overscancolorbarWidth/2 -5;
		PinkBlock[1].y = 535;
		PinkBlock[1].w = 10;
		PinkBlock[1].h = 10;

		LittleBlueBlock[1].x = 960 - 3*overscancolorbarWidth/2 -5;
		LittleBlueBlock[1].y = 535;
		LittleBlueBlock[1].w = 10;
		LittleBlueBlock[1].h = 10;

		RedBlock[1].x = 960 + 3*overscancolorbarWidth/2 -5;
		RedBlock[1].y = 535;
		RedBlock[1].w = 10;
		RedBlock[1].h = 10;

		YellowBlock[1].x = 960 - 5*overscancolorbarWidth/2 -5;
		YellowBlock[1].y = 535;
		YellowBlock[1].w = 10;
		YellowBlock[1].h = 10;

		BlueBlock[1].x = 960 + 5*overscancolorbarWidth/2 -5;
		BlueBlock[1].y = 535;
		BlueBlock[1].w = 10;
		BlueBlock[1].h = 10;

		if(sidecolorwidth >= 30){
			GrayBlock[1].x = 960 - 3*overscancolorbarWidth - sidecolorwidth/2 -5;
			GrayBlock[1].y = 535;
			GrayBlock[1].w = 10;
			GrayBlock[1].h = 10;

			BlackBlock[1].x = 960 + 3*overscancolorbarWidth + sidecolorwidth/2 -5;
			BlackBlock[1].y = 535;
			BlackBlock[1].w = 10;
			BlackBlock[1].h = 10;
		} else {
			GrayBlock[1].x = 0;
			GrayBlock[1].y = 0;
			GrayBlock[1].w = 0;
			GrayBlock[1].h = 0;

			BlackBlock[1].x = 0;
			BlackBlock[1].y = 0;
			BlackBlock[1].w = 0;
			BlackBlock[1].h = 0;
		}

		/*line three*/
		GreenBlock[2].x = 960 - overscancolorbarWidth/2 -5;
		GreenBlock[2].y = 895;
		GreenBlock[2].w = 10;
		GreenBlock[2].h = 10;

		PinkBlock[2].x = 960 + overscancolorbarWidth/2 -5;
		PinkBlock[2].y = 895;
		PinkBlock[2].w = 10;
		PinkBlock[2].h = 10;

		LittleBlueBlock[2].x = 960 - 3*overscancolorbarWidth/2 -5;
		LittleBlueBlock[2].y = 895;
		LittleBlueBlock[2].w = 10;
		LittleBlueBlock[2].h = 10;

		RedBlock[2].x = 960 + 3*overscancolorbarWidth/2 -5;
		RedBlock[2].y = 895;
		RedBlock[2].w = 10;
		RedBlock[2].h = 10;

		YellowBlock[2].x = 960 - 5*overscancolorbarWidth/2 -5;
		YellowBlock[2].y = 895;
		YellowBlock[2].w = 10;
		YellowBlock[2].h = 10;

		BlueBlock[2].x = 960 + 5*overscancolorbarWidth/2 -5;
		BlueBlock[2].y = 895;
		BlueBlock[2].w = 10;
		BlueBlock[2].h = 10;

		if(sidecolorwidth >= 30){
			GrayBlock[2].x = 960 - 3*overscancolorbarWidth - sidecolorwidth/2 -5;
			GrayBlock[2].y = 895;
			GrayBlock[2].w = 10;
			GrayBlock[2].h = 10;

			BlackBlock[2].x = 960 + 3*overscancolorbarWidth + sidecolorwidth/2 -5;
			BlackBlock[2].y = 895;
			BlackBlock[2].w = 10;
			BlackBlock[2].h = 10;
		} else {
			GrayBlock[2].x = 0;
			GrayBlock[2].y = 0;
			GrayBlock[2].w = 0;
			GrayBlock[2].h = 0;

			BlackBlock[2].x = 0;
			BlackBlock[2].y = 0;
			BlackBlock[2].w = 0;
			BlackBlock[2].h = 0;
		}
	} else if (6 == colorbarnum ){
			/*line one*/
			GreenBlock[0].x = 960 - overscancolorbarWidth/2 -5;
			GreenBlock[0].y = 175;
			GreenBlock[0].w = 10;
			GreenBlock[0].h = 10;

			PinkBlock[0].x = 960 + overscancolorbarWidth/2 -5;
			PinkBlock[0].y = 175;
			PinkBlock[0].w = 10;
			PinkBlock[0].h = 10;

			LittleBlueBlock[0].x = 960 - 3*overscancolorbarWidth/2 -5;
			LittleBlueBlock[0].y = 175;
			LittleBlueBlock[0].w = 10;
			LittleBlueBlock[0].h = 10;

			RedBlock[0].x = 960 + 3*overscancolorbarWidth/2 -5;
			RedBlock[0].y = 175;
			RedBlock[0].w = 10;
			RedBlock[0].h = 10;

			if(sidecolorwidth >= 30){
				YellowBlock[0].x = 960 - 2*overscancolorbarWidth-sidecolorwidth/2 -5;
				YellowBlock[0].y = 175;
				YellowBlock[0].w = 10;
				YellowBlock[0].h = 10;

				BlueBlock[0].x = 960 + 2*overscancolorbarWidth+sidecolorwidth/2 -5;
				BlueBlock[0].y = 175;
				BlueBlock[0].w = 10;
				BlueBlock[0].h = 10;

			} else {
				YellowBlock[0].x = 0;
				YellowBlock[0].y = 0;
				YellowBlock[0].w = 0;
				YellowBlock[0].h = 0;

				BlueBlock[0].x = 0;
				BlueBlock[0].y = 0;
				BlueBlock[0].w = 0;
				BlueBlock[0].h = 0;

			}

			GrayBlock[0].x = 0;
			GrayBlock[0].y = 0;
			GrayBlock[0].w = 0;
			GrayBlock[0].h = 0;

			BlackBlock[0].x = 0;
			BlackBlock[0].y = 0;
			BlackBlock[0].w = 0;
			BlackBlock[0].h = 0;
			/*line two*/
			GreenBlock[1].x = 960 - overscancolorbarWidth/2 -5;
			GreenBlock[1].y = 535;
			GreenBlock[1].w = 10;
			GreenBlock[1].h = 10;

			PinkBlock[1].x = 960 + overscancolorbarWidth/2 -5;
			PinkBlock[1].y = 535;
			PinkBlock[1].w = 10;
			PinkBlock[1].h = 10;

			LittleBlueBlock[1].x = 960 - 3*overscancolorbarWidth/2 -5;
			LittleBlueBlock[1].y = 535;
			LittleBlueBlock[1].w = 10;
			LittleBlueBlock[1].h = 10;

			RedBlock[1].x = 960 + 3*overscancolorbarWidth/2 -5;
			RedBlock[1].y = 535;
			RedBlock[1].w = 10;
			RedBlock[1].h = 10;

			if(sidecolorwidth >= 30){
				YellowBlock[1].x = 960 - 2*overscancolorbarWidth-sidecolorwidth/2 -5;
				YellowBlock[1].y = 535;
				YellowBlock[1].w = 10;
				YellowBlock[1].h = 10;

				BlueBlock[1].x = 960 + 2*overscancolorbarWidth+sidecolorwidth/2 -5;
				BlueBlock[1].y = 535;
				BlueBlock[1].w = 10;
				BlueBlock[1].h = 10;
			} else {
				YellowBlock[1].x = 0;
				YellowBlock[1].y = 0;
				YellowBlock[1].w = 0;
				YellowBlock[1].h = 0;

				BlueBlock[1].x = 0;
				BlueBlock[1].y = 0;
				BlueBlock[1].w = 0;
				BlueBlock[1].h = 0;
			}
			GrayBlock[1].x = 0;
			GrayBlock[1].y = 0;
			GrayBlock[1].w = 0;
			GrayBlock[1].h = 0;

			BlackBlock[1].x = 0;
			BlackBlock[1].y = 0;
			BlackBlock[1].w = 0;
			BlackBlock[1].h = 0;

			/*line three*/
			GreenBlock[2].x = 960 - overscancolorbarWidth/2 -5;
			GreenBlock[2].y = 895;
			GreenBlock[2].w = 10;
			GreenBlock[2].h = 10;

			PinkBlock[2].x = 960 + overscancolorbarWidth/2 -5;
			PinkBlock[2].y = 895;
			PinkBlock[2].w = 10;
			PinkBlock[2].h = 10;

			LittleBlueBlock[2].x = 960 - 3*overscancolorbarWidth/2 -5;
			LittleBlueBlock[2].y = 895;
			LittleBlueBlock[2].w = 10;
			LittleBlueBlock[2].h = 10;

			RedBlock[2].x = 960 + 3*overscancolorbarWidth/2 -5;
			RedBlock[2].y = 895;
			RedBlock[2].w = 10;
			RedBlock[2].h = 10;

			if(sidecolorwidth >= 30){
				YellowBlock[2].x = 960 - 2*overscancolorbarWidth-sidecolorwidth/2 -5;
				YellowBlock[2].y = 895;
				YellowBlock[2].w = 10;
				YellowBlock[2].h = 10;

				BlueBlock[2].x = 960 + 2*overscancolorbarWidth+sidecolorwidth/2 -5;
				BlueBlock[2].y = 895;
				BlueBlock[2].w = 10;
				BlueBlock[2].h = 10;

			} else {
				YellowBlock[2].x = 0;
				YellowBlock[2].y = 0;
				YellowBlock[2].w = 0;
				YellowBlock[2].h = 0;

				BlueBlock[2].x = 0;
				BlueBlock[2].y = 0;
				BlueBlock[2].w = 0;
				BlueBlock[2].h = 0;
			}
			GrayBlock[2].x = 0;
			GrayBlock[2].y = 0;
			GrayBlock[2].w = 0;
			GrayBlock[2].h = 0;

			BlackBlock[2].x = 0;
			BlackBlock[2].y = 0;
			BlackBlock[2].w = 0;
			BlackBlock[2].h = 0;
	} else if (4 == colorbarnum){
		/*line one*/
		GreenBlock[0].x = 960 - overscancolorbarWidth/2 -5;
		GreenBlock[0].y = 175;
		GreenBlock[0].w = 10;
		GreenBlock[0].h = 10;

		PinkBlock[0].x = 960 + overscancolorbarWidth/2 -5;
		PinkBlock[0].y = 175;
		PinkBlock[0].w = 10;
		PinkBlock[0].h = 10;

		if(sidecolorwidth >= 30){
			LittleBlueBlock[0].x = 960 - overscancolorbarWidth-sidecolorwidth/2 -5;
			LittleBlueBlock[0].y = 175;
			LittleBlueBlock[0].w = 10;
			LittleBlueBlock[0].h = 10;

			RedBlock[0].x = 960 + overscancolorbarWidth+sidecolorwidth/2 -5;
			RedBlock[0].y = 175;
			RedBlock[0].w = 10;
			RedBlock[0].h = 10;
		} else {
			LittleBlueBlock[0].x = 0;
			LittleBlueBlock[0].y = 0;
			LittleBlueBlock[0].w = 0;
			LittleBlueBlock[0].h = 0;

			RedBlock[0].x = 0;
			RedBlock[0].y = 0;
			RedBlock[0].w = 0;
			RedBlock[0].h = 0;
		}
		YellowBlock[0].x = 0;
		YellowBlock[0].y = 0;
		YellowBlock[0].w = 0;
		YellowBlock[0].h = 0;

		BlueBlock[0].x = 0;
		BlueBlock[0].y = 0;
		BlueBlock[0].w = 0;
		BlueBlock[0].h = 0;

		GrayBlock[0].x = 0;
		GrayBlock[0].y = 0;
		GrayBlock[0].w = 0;
		GrayBlock[0].h = 0;

		BlackBlock[0].x = 0;
		BlackBlock[0].y = 0;
		BlackBlock[0].w = 0;
		BlackBlock[0].h = 0;

		/*line two*/
		GreenBlock[1].x = 960 - overscancolorbarWidth/2 -5;
		GreenBlock[1].y = 535;
		GreenBlock[1].w = 10;
		GreenBlock[1].h = 10;

		PinkBlock[1].x = 960 + overscancolorbarWidth/2 -5;
		PinkBlock[1].y = 535;
		PinkBlock[1].w = 10;
		PinkBlock[1].h = 10;

		if(sidecolorwidth >= 30){
			LittleBlueBlock[1].x = 960 - overscancolorbarWidth-sidecolorwidth/2 -5;
			LittleBlueBlock[1].y = 535;
			LittleBlueBlock[1].w = 10;
			LittleBlueBlock[1].h = 10;

			RedBlock[1].x = 960 + overscancolorbarWidth+sidecolorwidth/2 -5;
			RedBlock[1].y = 535;
			RedBlock[1].w = 10;
			RedBlock[1].h = 10;
		} else {
			LittleBlueBlock[1].x = 0;
			LittleBlueBlock[1].y = 0;
			LittleBlueBlock[1].w = 0;
			LittleBlueBlock[1].h = 0;

			RedBlock[1].x = 0;
			RedBlock[1].y = 0;
			RedBlock[1].w = 0;
			RedBlock[1].h = 0;
		}
		YellowBlock[1].x = 0;
		YellowBlock[1].y = 0;
		YellowBlock[1].w = 0;
		YellowBlock[1].h = 0;

		BlueBlock[1].x = 0;
		BlueBlock[1].y = 0;
		BlueBlock[1].w = 0;
		BlueBlock[1].h = 0;

		GrayBlock[1].x = 0;
		GrayBlock[1].y = 0;
		GrayBlock[1].w = 0;
		GrayBlock[1].h = 0;

		BlackBlock[1].x = 0;
		BlackBlock[1].y = 0;
		BlackBlock[1].w = 0;
		BlackBlock[1].h = 0;

		/*line three*/
		GreenBlock[2].x = 960 - overscancolorbarWidth/2 -5;
		GreenBlock[2].y = 895;
		GreenBlock[2].w = 10;
		GreenBlock[2].h = 10;

		PinkBlock[2].x = 960 + overscancolorbarWidth/2 -5;
		PinkBlock[2].y = 895;
		PinkBlock[2].w = 10;
		PinkBlock[2].h = 10;

		if(sidecolorwidth >= 30){
			LittleBlueBlock[2].x = 960 - overscancolorbarWidth-sidecolorwidth/2 -5;
			LittleBlueBlock[2].y = 895;
			LittleBlueBlock[2].w = 10;
			LittleBlueBlock[2].h = 10;

			RedBlock[2].x = 960 + overscancolorbarWidth+sidecolorwidth/2 -5;
			RedBlock[2].y = 895;
			RedBlock[2].w = 10;
			RedBlock[2].h = 10;
		} else {
			LittleBlueBlock[2].x = 0;
			LittleBlueBlock[2].y = 0;
			LittleBlueBlock[2].w = 0;
			LittleBlueBlock[2].h = 0;

			RedBlock[2].x = 0;
			RedBlock[2].y = 0;
			RedBlock[2].w = 0;
			RedBlock[2].h = 0;
		}
		YellowBlock[2].x = 0;
		YellowBlock[2].y = 0;
		YellowBlock[2].w = 0;
		YellowBlock[2].h = 0;

		BlueBlock[2].x = 0;
		BlueBlock[2].y = 0;
		BlueBlock[2].w = 0;
		BlueBlock[2].h = 0;

		GrayBlock[2].x = 0;
		GrayBlock[2].y = 0;
		GrayBlock[2].w = 0;
		GrayBlock[2].h = 0;

		BlackBlock[2].x = 0;
		BlackBlock[2].y = 0;
		BlackBlock[2].w = 0;
		BlackBlock[2].h = 0;

	}else if ((2 == colorbarnum) || (0 == colorbarnum)) {
		/*line one*/
		GreenBlock[0].x = 475;
		GreenBlock[0].y = 175;
		GreenBlock[0].w = 10;
		GreenBlock[0].h = 10;

		PinkBlock[0].x = 475;
		PinkBlock[0].y = 175;
		PinkBlock[0].w = 10;
		PinkBlock[0].h = 10;

		LittleBlueBlock[0].x = 0;
		LittleBlueBlock[0].y = 0;
		LittleBlueBlock[0].w = 0;
		LittleBlueBlock[0].h = 0;

		RedBlock[0].x = 0;
		RedBlock[0].y = 0;
		RedBlock[0].w = 0;
		RedBlock[0].h = 0;

		YellowBlock[0].x = 0;
		YellowBlock[0].y = 0;
		YellowBlock[0].w = 0;
		YellowBlock[0].h = 0;

		BlueBlock[0].x = 0;
		BlueBlock[0].y = 0;
		BlueBlock[0].w = 0;
		BlueBlock[0].h = 0;

		GrayBlock[0].x = 0;
		GrayBlock[0].y = 0;
		GrayBlock[0].w = 0;
		GrayBlock[0].h = 0;

		BlackBlock[0].x = 0;
		BlackBlock[0].y = 0;
		BlackBlock[0].w = 0;
		BlackBlock[0].h = 0;

		/*line two*/
		GreenBlock[1].x = 475;
		GreenBlock[1].y = 535;
		GreenBlock[1].w = 10;
		GreenBlock[1].h = 10;

		PinkBlock[1].x = 475;
		PinkBlock[1].y = 535;
		PinkBlock[1].w = 10;
		PinkBlock[1].h = 10;

		LittleBlueBlock[1].x = 0;
		LittleBlueBlock[1].y = 0;
		LittleBlueBlock[1].w = 0;
		LittleBlueBlock[1].h = 0;

		RedBlock[1].x = 0;
		RedBlock[1].y = 0;
		RedBlock[1].w = 0;
		RedBlock[1].h = 0;

		YellowBlock[1].x = 0;
		YellowBlock[1].y = 0;
		YellowBlock[1].w = 0;
		YellowBlock[1].h = 0;

		BlueBlock[1].x = 0;
		BlueBlock[1].y = 0;
		BlueBlock[1].w = 0;
		BlueBlock[1].h = 0;

		GrayBlock[1].x = 0;
		GrayBlock[1].y = 0;
		GrayBlock[1].w = 0;
		GrayBlock[1].h = 0;

		BlackBlock[1].x = 0;
		BlackBlock[1].y = 0;
		BlackBlock[1].w = 0;
		BlackBlock[1].h = 0;

		/*line three*/
		GreenBlock[2].x = 475;
		GreenBlock[2].y = 895;
		GreenBlock[2].w = 10;
		GreenBlock[2].h = 10;

		PinkBlock[2].x = 475;
		PinkBlock[2].y = 895;
		PinkBlock[2].w = 10;
		PinkBlock[2].h = 10;

		LittleBlueBlock[2].x = 0;
		LittleBlueBlock[2].y = 0;
		LittleBlueBlock[2].w = 0;
		LittleBlueBlock[2].h = 0;

		RedBlock[2].x = 0;
		RedBlock[2].y = 0;
		RedBlock[2].w = 0;
		RedBlock[2].h = 0;

		YellowBlock[2].x = 0;
		YellowBlock[2].y = 0;
		YellowBlock[2].w = 0;
		YellowBlock[2].h = 0;

		BlueBlock[2].x = 0;
		BlueBlock[2].y = 0;
		BlueBlock[2].w = 0;
		BlueBlock[2].h = 0;

		GrayBlock[2].x = 0;
		GrayBlock[2].y = 0;
		GrayBlock[2].w = 0;
		GrayBlock[2].h = 0;

		BlackBlock[2].x = 0;
		BlackBlock[2].y = 0;
		BlackBlock[2].w = 0;
		BlackBlock[2].h = 0;
	}
	return TRUE;
}

/*colorbar type is master7800 PTG pattern 32 colorbar 75%*/
unsigned char Check_DisplayFrame_isRightColorbar(void)
{
	KADP_VT_RECT_T OutputRegion;
	KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_INFO_T VideoFrameOutputDeviceState;
	unsigned int IndexOfCurrentVideoFrameBuffer;
	unsigned long colorbarbufferaddr;

	VIDEO_RECT_T GrayBlock[3];
	VIDEO_RECT_T YellowBlock[3];
	VIDEO_RECT_T LittleBlueBlock[3];
	VIDEO_RECT_T GreenBlock[3];
	VIDEO_RECT_T PinkBlock[3];
	VIDEO_RECT_T RedBlock[3];
	VIDEO_RECT_T BlueBlock[3];
	VIDEO_RECT_T BlackBlock[3];

	if (HAL_VT_InitEx(5) == FALSE){
		rtd_pr_vt_emerg("KADP_VT_Init fail!%s=%d \n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	OutputRegion.x = 0;
	OutputRegion.y = 0;
	OutputRegion.w = 1920;
	OutputRegion.h = 1080;
	if (HAL_VT_SetVideoFrameOutputDeviceOutputRegion(KADP_VT_VIDEO_WINDOW_0, KADP_VT_DISPLAY_OUTPUT, &OutputRegion) == FALSE){
		rtd_pr_vt_emerg("KADP_VT_SetVideoFrameOutputDeviceOutputRegion fail!%s=%d \n", __FUNCTION__, __LINE__);
		HAL_VT_Finalize();
		return FALSE;
	}

	VideoFrameOutputDeviceState.bAppliedPQ = 0;
	VideoFrameOutputDeviceState.bEnabled = 1;
	VideoFrameOutputDeviceState.framerateDivide = 0;
	VideoFrameOutputDeviceState.bFreezed = 1;
	if (HAL_VT_SetVideoFrameOutputDeviceState(KADP_VT_VIDEO_WINDOW_0, KADP_VT_VIDEO_FRAME_OUTPUT_DEVICE_STATE_FREEZED, &VideoFrameOutputDeviceState) ==FALSE){
		rtd_pr_vt_emerg("KADP_VT_SetVideoFrameOutputDeviceState fail!%s=%d \n", __FUNCTION__, __LINE__);
		HAL_VT_Finalize();
		return FALSE;
	}

	if (HAL_VT_GetVideoFrameBufferIndex(KADP_VT_VIDEO_WINDOW_0, &IndexOfCurrentVideoFrameBuffer) == FALSE){
		rtd_pr_vt_emerg("KADP_VT_SetVideoFrameOutputDeviceState fail!%s=%d \n", __FUNCTION__, __LINE__);
		HAL_VT_Finalize();
		return FALSE;
	}

	colorbarbufferaddr =(unsigned long) CaptureCtrl_VT.cap_buffer[IndexOfCurrentVideoFrameBuffer].cache;

	Check_colorbar_window_cal(GrayBlock,YellowBlock,LittleBlueBlock,GreenBlock,PinkBlock,RedBlock,BlueBlock,BlackBlock);

	rtd_pr_vt_emerg("GrayBlock[0] x=%d;y=%d\n",GrayBlock[0].x,GrayBlock[0].y);
	rtd_pr_vt_emerg("GrayBlock[1] x=%d;y=%d\n",GrayBlock[1].x,GrayBlock[1].y);
	rtd_pr_vt_emerg("GrayBlock[2] x=%d;y=%d\n",GrayBlock[2].x,GrayBlock[2].y);

	rtd_pr_vt_emerg("YellowBlock[0] x=%d;y=%d\n",YellowBlock[0].x,YellowBlock[0].y);
	rtd_pr_vt_emerg("YellowBlock[1] x=%d;y=%d\n",YellowBlock[1].x,YellowBlock[1].y);
	rtd_pr_vt_emerg("YellowBlock[2] x=%d;y=%d\n",YellowBlock[2].x,YellowBlock[2].y);

	rtd_pr_vt_emerg("LittleBlueBlock[0] x=%d;y=%d\n",LittleBlueBlock[0].x,LittleBlueBlock[0].y);
	rtd_pr_vt_emerg("LittleBlueBlock[1] x=%d;y=%d\n",LittleBlueBlock[1].x,LittleBlueBlock[1].y);
	rtd_pr_vt_emerg("LittleBlueBlock[2] x=%d;y=%d\n",LittleBlueBlock[2].x,LittleBlueBlock[2].y);

	rtd_pr_vt_emerg("GreenBlock[0] x=%d;y=%d\n",GreenBlock[0].x,GreenBlock[0].y);
	rtd_pr_vt_emerg("GreenBlock[1] x=%d;y=%d\n",GreenBlock[1].x,GreenBlock[1].y);
	rtd_pr_vt_emerg("GreenBlock[2] x=%d;y=%d\n",GreenBlock[2].x,GreenBlock[2].y);

	rtd_pr_vt_emerg("PinkBlock[0] x=%d;y=%d\n",PinkBlock[0].x,PinkBlock[0].y);
	rtd_pr_vt_emerg("PinkBlock[1] x=%d;y=%d\n",PinkBlock[1].x,PinkBlock[1].y);
	rtd_pr_vt_emerg("PinkBlock[2] x=%d;y=%d\n",PinkBlock[2].x,PinkBlock[2].y);

	rtd_pr_vt_emerg("RedBlock[0] x=%d;y=%d\n",RedBlock[0].x,RedBlock[0].y);
	rtd_pr_vt_emerg("RedBlock[1] x=%d;y=%d\n",RedBlock[1].x,RedBlock[1].y);
	rtd_pr_vt_emerg("RedBlock[2] x=%d;y=%d\n",RedBlock[2].x,RedBlock[2].y);

	rtd_pr_vt_emerg("BlueBlock[0] x=%d;y=%d\n",BlueBlock[0].x,BlueBlock[0].y);
	rtd_pr_vt_emerg("BlueBlock[1] x=%d;y=%d\n",BlueBlock[1].x,BlueBlock[1].y);
	rtd_pr_vt_emerg("BlueBlock[2] x=%d;y=%d\n",BlueBlock[2].x,BlueBlock[2].y);

	rtd_pr_vt_emerg("BlackBlock[0] x=%d;y=%d\n",BlackBlock[0].x,BlackBlock[0].y);
	rtd_pr_vt_emerg("BlackBlock[1] x=%d;y=%d\n",BlackBlock[1].x,BlackBlock[1].y);
	rtd_pr_vt_emerg("BlackBlock[2] x=%d;y=%d\n",BlackBlock[2].x,BlackBlock[2].y);

	rtd_pr_vt_emerg("Check_DisplayFrame_isRightColorbar start!%s=%d \n", __FUNCTION__, __LINE__);
	if(ColorBlock_Compare(colorbarbufferaddr,0xBE,0xBE,0xC2,GrayBlock[0]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0xBE,0xBE,0xC2,GrayBlock[1]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0xBE,0xBE,0xC2,GrayBlock[2]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0xBC,0xBB,0,YellowBlock[0]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0xBC,0xBB,0,YellowBlock[1]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0xBC,0xBB,0,YellowBlock[2]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0,0xB7,0xB5,LittleBlueBlock[0]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0,0xB7,0xB5,LittleBlueBlock[1]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0,0xB7,0xB5,LittleBlueBlock[2]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0,0xB5,0,GreenBlock[0]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0,0xB5,0,GreenBlock[1]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0,0xB5,0,GreenBlock[2]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0xB6,0,0xB9,PinkBlock[0]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0xB6,0,0xB9,PinkBlock[1]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0xB6,0,0xB9,PinkBlock[2]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0xB8,0,0,RedBlock[0]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0xB8,0,0,RedBlock[1]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0xB8,0,0,RedBlock[2]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0,0,0xC0,BlueBlock[0]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0,0,0xC0,BlueBlock[1]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0,0,0xC0,BlueBlock[2]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0,0,0,BlackBlock[0]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0,0,0,BlackBlock[1]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	if(ColorBlock_Compare(colorbarbufferaddr,0,0,0,BlackBlock[2]) == FALSE)
	{
		HAL_VT_Finalize();
		return FALSE;
	}
	HAL_VT_Finalize();
	rtd_pr_vt_emerg("Check_DisplayFrame_isRightColorbar success!%s=%d \n", __FUNCTION__, __LINE__);
	return TRUE;

}
unsigned int VT_get_input_wid(VT_CAP_SRC capSrc)
{
	unsigned int wid=_DISP_WID;
	ppoverlay_main_den_h_start_end_RBUS main_den_h_start_end_Reg;
	ppoverlay_main_active_h_start_end_RBUS main_active_h_start_end_Reg;
	ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;

	main_den_h_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_DEN_H_Start_End_reg);
	main_active_h_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_H_Start_End_reg);
	ppoverlay_uzudtg_control1_reg.regValue = rtd_inl(PPOVERLAY_uzudtg_control1_reg);

	if((get_vt_VtCaptureVDC())){
			wid=DC2H_InputWidth;
	} else {
		if(capSrc == VT_CAP_SRC_VIDEO_NOPQ) {
//for uzu position could not support 2k1k(when 2step enable,uzu mux size will be less than 2k1k)
			if(ppoverlay_uzudtg_control1_reg.two_step_uzu_en ==1) {
				wid = (main_active_h_start_end_Reg.mh_act_end - main_active_h_start_end_Reg.mh_act_sta)>>1;
			}else {
				wid = main_active_h_start_end_Reg.mh_act_end - main_active_h_start_end_Reg.mh_act_sta;
			}
		} else {
			if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL)
				wid = (main_den_h_start_end_Reg.mh_den_end - main_den_h_start_end_Reg.mh_den_sta)/2;
			else
				wid = main_den_h_start_end_Reg.mh_den_end - main_den_h_start_end_Reg.mh_den_sta;
		}
	}
	return wid;
}
unsigned int VT_get_input_height(VT_CAP_SRC capSrc)
{
	unsigned int len=_DISP_LEN;
	ppoverlay_main_den_v_start_end_RBUS main_den_v_start_end_Reg;
	ppoverlay_main_active_v_start_end_RBUS main_active_v_start_end_Reg;
	ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;

	main_den_v_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_DEN_V_Start_End_reg);
	main_active_v_start_end_Reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_V_Start_End_reg);
	ppoverlay_uzudtg_control1_reg.regValue = rtd_inl(PPOVERLAY_uzudtg_control1_reg);

	if((get_vt_VtCaptureVDC())){
			len = DC2H_InputLength;
	} else {
		if(capSrc == VT_CAP_SRC_VIDEO_NOPQ) {
//for uzu position could not support 2k1k(when 2step enable,uzu mux size will be less than 2k1k)
			if(ppoverlay_uzudtg_control1_reg.two_step_uzu_en ==1) {
				len = (main_active_v_start_end_Reg.mv_act_end - main_active_v_start_end_Reg.mv_act_sta)>>1;
			}else {
				len = main_active_v_start_end_Reg.mv_act_end - main_active_v_start_end_Reg.mv_act_sta;
			}
		} else {
			len = main_den_v_start_end_Reg.mv_den_end - main_den_v_start_end_Reg.mv_den_sta;
		}
	}
	return len;
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

unsigned char debug_dump_data_to_file(unsigned int idx, unsigned int dump_w,unsigned int dump_h)
{
	struct file* filp = NULL;
	unsigned long outfileOffset = 0;
	unsigned int size = VT_CAPTURE_BUFFER_UNITSIZE;
	unsigned char *pVirStartAdrr = NULL;
	static unsigned int g_ulFileCount = 0;
	unsigned int src_phy = CaptureCtrl_VT.cap_buffer[idx].phyaddr;
	char raw_dat_path[30];
	sprintf(raw_dat_path, "/tmp/vt_dump_%d_%d.raw", idx, g_ulFileCount);
	pVirStartAdrr = (unsigned char *)dvr_remap_uncached_memory(src_phy, size, __builtin_return_address(0));

	if((get_VT_Pixel_Format() == VT_CAP_NV12) || (get_VT_Pixel_Format() == VT_CAP_NV16)||(get_VT_Pixel_Format() == VT_CAP_NV21))
	{
        rtd_pr_vt_notice("[VT]y buf offset=%lx\n", vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height));
		if (pVirStartAdrr != 0)
		{
			filp = file_open(raw_dat_path, O_RDWR | O_CREAT | O_APPEND, 0);
			if (filp == NULL)
			{
				rtd_pr_vt_emerg("[VT]file open Y fail\n");
				dvr_unmap_memory(pVirStartAdrr, size);
				return FALSE;
			}

            rtd_pr_vt_notice("[VT]dump y buf addr=%lx\n", (unsigned long)pVirStartAdrr);
			file_write(filp, outfileOffset, pVirStartAdrr, (dump_w * dump_h));//Y plane
			file_sync(filp);
			file_close(filp);

			filp = file_open(raw_dat_path, O_RDWR | O_CREAT | O_APPEND, 0);
			if (filp == NULL)
			{
				rtd_pr_vt_emerg("[VT]file open fail\n");
				dvr_unmap_memory(pVirStartAdrr, size);
				return FALSE;
			}

            rtd_pr_vt_notice("[VT]dump c buf addr=%lx\n", (unsigned long)(pVirStartAdrr + vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height)));
			if((get_VT_Pixel_Format() == VT_CAP_NV12)||(get_VT_Pixel_Format() == VT_CAP_NV21))
                file_write(filp, outfileOffset, (unsigned char *)(pVirStartAdrr+vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height)), ((dump_w * dump_h)/2));//nv12 UV plane
			else
                file_write(filp, outfileOffset, (unsigned char *)(pVirStartAdrr+vt_bufAddr_align(vt_cap_frame_max_width*vt_cap_frame_max_height)), (dump_w * dump_h));//nv16 UV plane
			file_sync(filp);
			file_close(filp);

			rtd_pr_vt_emerg("[VT]dump_finish\n");
			g_ulFileCount++;
			dvr_unmap_memory(pVirStartAdrr, size);
			return TRUE;
		}
		else {
			rtd_pr_vt_emerg("\n\n\n\n ***************** [VT]dump_to_file NG for null buffer address	*********************\n\n\n\n");
			return FALSE;
		}
	}
	else if(get_VT_Pixel_Format() == VT_CAP_ARGB8888 || get_VT_Pixel_Format() == VT_CAP_ABGR8888 || get_VT_Pixel_Format() == VT_CAP_RGBA8888 || get_VT_Pixel_Format() == VT_CAP_RGB888)
	{
		if (pVirStartAdrr != 0)
		{
			filp = file_open(raw_dat_path, O_RDWR | O_CREAT, 0);
			if (filp == NULL)
			{
				rtd_pr_vt_emerg("[VT]file open fail\n");
				dvr_unmap_memory(pVirStartAdrr, size);
				return FALSE;
			}

			if(get_VT_Pixel_Format() == VT_CAP_RGB888){
				file_write(filp, outfileOffset, pVirStartAdrr, (dump_w * dump_h*3));
			}
			else
				file_write(filp, outfileOffset, pVirStartAdrr, (dump_w * dump_h*4));
			file_sync(filp);
			file_close(filp);

			rtd_pr_vt_emerg("[VT]dump_finish\n");
			g_ulFileCount++;
			dvr_unmap_memory(pVirStartAdrr, size);
			return TRUE;
		}
		else {
			rtd_pr_vt_emerg("\n\n\n\n ***************** [VT]dump_to_file NG for null buffer address	*********************\n\n\n\n");
			return FALSE;
		}
	}
	else
	{
		rtd_pr_vt_emerg("[VT]invalid pixelfmt\n");
		dvr_unmap_memory(pVirStartAdrr, size);
		return TRUE;
	}

}

/*-----------------------------------------------------------------------------------
*input:   capW, capH-> buffer size
*output:  buffer phy addr from cma
*note:
*-----------------------------------------------------------------------------------*/
unsigned long Camera_DC2H_alloc_mem(unsigned int capW, unsigned int capH)
{
	unsigned long  ulRetAddr = 0;
	unsigned int   uiBufsize = 0;
	uiBufsize = _ALIGN((capW*capH),__12KPAGE)+_ALIGN((capW*capH/2),__12KPAGE);

	ulRetAddr = pli_malloc(uiBufsize, GFP_DCU1_FIRST);

	if(ulRetAddr == INVALID_VAL)
	{
		rtd_pr_vt_emerg("[error]VT pli_malloc memory fail from cma1\n");
		return INVALID_VAL;
	}
	else
	{
		rtd_pr_vt_notice("[VT memory allocate] buf=(%d KB),phy(%lx)n", uiBufsize>>10, ulRetAddr);
		return ulRetAddr;
	}
}

void Camera_set_DC2H_boundary(unsigned long phyYaddr, unsigned int capW, unsigned int capH)
{
	dc2h_dma_dc2h_cap_boundaryaddr1_RBUS dc2h_cap_boundaryaddr1_reg;  //up limit
	dc2h_dma_dc2h_cap_boundaryaddr2_RBUS dc2h_cap_boundaryaddr2_reg;  //low limit

	dc2h_dma_dc2h_cap_boundaryaddr3_RBUS dc2h_cap_boundaryaddr3_reg;
	dc2h_dma_dc2h_cap_boundaryaddr4_RBUS dc2h_cap_boundaryaddr4_reg;

	dc2h_dma_dc2h_cap_boundaryaddr5_RBUS dc2h_cap_boundaryaddr5_reg;
	dc2h_dma_dc2h_cap_boundaryaddr6_RBUS dc2h_cap_boundaryaddr6_reg;

	dc2h_dma_dc2h_cap_boundaryaddr7_RBUS dc2h_cap_boundaryaddr7_reg;
	dc2h_dma_dc2h_cap_boundaryaddr8_RBUS dc2h_cap_boundaryaddr8_reg;

	dc2h_dma_dc2h_cap_boundaryaddr9_RBUS dc2h_cap_boundaryaddr9_reg;
	dc2h_dma_dc2h_cap_boundaryaddr10_RBUS dc2h_cap_boundaryaddr10_reg;

	dc2h_cap_boundaryaddr1_reg.regValue =  (UINT32)phyYaddr + _ALIGN((capW*capH),__12KPAGE)+_ALIGN((capW*capH/2),__12KPAGE);
	rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr1_reg, dc2h_cap_boundaryaddr1_reg.regValue);
	dc2h_cap_boundaryaddr2_reg.regValue =  (UINT32)phyYaddr;
	rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr2_reg, dc2h_cap_boundaryaddr2_reg.regValue);

	dc2h_cap_boundaryaddr3_reg.regValue =  (UINT32)phyYaddr + _ALIGN((capW*capH),__12KPAGE)+_ALIGN((capW*capH/2),__12KPAGE);
	rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr3_reg, dc2h_cap_boundaryaddr3_reg.regValue);
	dc2h_cap_boundaryaddr4_reg.regValue =  (UINT32)phyYaddr;
	rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr4_reg, dc2h_cap_boundaryaddr4_reg.regValue);

	dc2h_cap_boundaryaddr5_reg.regValue =  (UINT32)phyYaddr + _ALIGN((capW*capH),__12KPAGE)+_ALIGN((capW*capH/2),__12KPAGE);
	rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr5_reg, dc2h_cap_boundaryaddr5_reg.regValue);
	dc2h_cap_boundaryaddr6_reg.regValue =  (UINT32)phyYaddr;
	rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr6_reg, dc2h_cap_boundaryaddr6_reg.regValue);

	dc2h_cap_boundaryaddr7_reg.regValue =  (UINT32)phyYaddr + _ALIGN((capW*capH),__12KPAGE)+_ALIGN((capW*capH/2),__12KPAGE);
	rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr7_reg, dc2h_cap_boundaryaddr7_reg.regValue);
	dc2h_cap_boundaryaddr8_reg.regValue = (UINT32)phyYaddr;
	rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr8_reg, dc2h_cap_boundaryaddr8_reg.regValue);

	dc2h_cap_boundaryaddr9_reg.regValue =  (UINT32)phyYaddr + _ALIGN((capW*capH),__12KPAGE)+_ALIGN((capW*capH/2),__12KPAGE);
	rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr9_reg, dc2h_cap_boundaryaddr9_reg.regValue);
	dc2h_cap_boundaryaddr10_reg.regValue = (UINT32)phyYaddr;
	rtd_outl(DC2H_DMA_dc2h_Cap_BoundaryAddr10_reg, dc2h_cap_boundaryaddr10_reg.regValue);
}

unsigned char Upadte_DC2H_NV12_CapAddr(void)
{
	unsigned long  ulRetAddr = 0;
	DC2H_SWMODE_STRUCT_T *swmode_infoptr;
	unsigned int ulCount = 0;
	int ret;
	rtd_pr_vt_notice("fun:%s\n",__FUNCTION__);

	ulRetAddr = Camera_DC2H_alloc_mem(vt_cap_frame_max_width, vt_cap_frame_max_height);

	swmode_infoptr = (DC2H_SWMODE_STRUCT_T *)Scaler_GetShareMemVirAddr(SCALERIOC_SET_DC2H_SWMODE_ENABLE);
	ulCount = sizeof(DC2H_SWMODE_STRUCT_T) / sizeof(unsigned int);

	swmode_infoptr->SwModeEnable = 1;
	swmode_infoptr->buffernumber = 1;
	swmode_infoptr->cap_format = (UINT32)get_VT_Pixel_Format();
	swmode_infoptr->cap_width = vt_cap_frame_max_width;
	swmode_infoptr->cap_length = vt_cap_frame_max_height;
	swmode_infoptr->YbufferSize = _ALIGN(vt_cap_frame_max_width*vt_cap_frame_max_height,__12KPAGE);
	swmode_infoptr->cap_buffer[0] = (UINT32)ulRetAddr;
	swmode_infoptr->cap_buffer[1] = (UINT32)ulRetAddr + _ALIGN(vt_cap_frame_max_width*vt_cap_frame_max_height,__12KPAGE);
	swmode_infoptr->cap_buffer[2] = 0;
	swmode_infoptr->cap_buffer[3] = 0;
	swmode_infoptr->cap_buffer[4] = 0;

	pr_notice("[VT]SwModeEnable = %d\n",swmode_infoptr->SwModeEnable);
	pr_notice("[VT]buffernumber = %d\n",swmode_infoptr->buffernumber);
	pr_notice("[VT]cap_format = %d\n",swmode_infoptr->cap_format);
	pr_notice("[VT]cap_width = %d\n",swmode_infoptr->cap_width);
	pr_notice("[VT]cap_length = %d\n",swmode_infoptr->cap_length);
	pr_notice("[VT]YbufferSize = %d\n",swmode_infoptr->YbufferSize);
	pr_notice("[VT]cap_buffer[0] = 0x%x\n",swmode_infoptr->cap_buffer[0]);
	pr_notice("[VT]cap_buffer[1] = 0x%x\n",swmode_infoptr->cap_buffer[1]);
	pr_notice("[VT]cap_buffer[2] = 0x%x\n",swmode_infoptr->cap_buffer[2]);
	pr_notice("[VT]cap_buffer[3] = 0x%x\n",swmode_infoptr->cap_buffer[3]);
	pr_notice("[VT]cap_buffer[4] = 0x%x\n",swmode_infoptr->cap_buffer[4]);

	//change endian
	swmode_infoptr->SwModeEnable = htonl(swmode_infoptr->SwModeEnable);
	swmode_infoptr->buffernumber = htonl(swmode_infoptr->buffernumber);
	swmode_infoptr->cap_format = htonl(swmode_infoptr->cap_format);
	swmode_infoptr->cap_width = htonl(swmode_infoptr->cap_width);
	swmode_infoptr->cap_length = htonl(swmode_infoptr->cap_length);
	swmode_infoptr->YbufferSize = htonl(swmode_infoptr->YbufferSize);
	swmode_infoptr->cap_buffer[0] = htonl(swmode_infoptr->cap_buffer[0]);
	swmode_infoptr->cap_buffer[1] = htonl(swmode_infoptr->cap_buffer[1]);
	swmode_infoptr->cap_buffer[2] = htonl(swmode_infoptr->cap_buffer[2]);
	swmode_infoptr->cap_buffer[3] = htonl(swmode_infoptr->cap_buffer[3]);
	swmode_infoptr->cap_buffer[4] = htonl(swmode_infoptr->cap_buffer[4]);

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_SET_DC2H_SWMODE_ENABLE,0,0)))
	{
		pr_emerg("[VT]ret=%d, SCALERIOC_SET_DC2H_SE_CAPTURE_ENABLE RPC fail !!!\n", ret);
		return FALSE;
	}
	return TRUE;

}

unsigned char Camera_Dump_DC2H_buffer(void)
{
	unsigned int PhyAddr = 0;
	struct file* filp = NULL;
	unsigned long outfileOffset = 0;
	dc2h_vi_dc2h_vi_doublebuffer_RBUS dc2h_vi_doublebuffer_Reg;
	//dc2h_dma_dc2h_cap_l3_start_RBUS dc2h_cap_l3_start_Reg;

	unsigned int size = _ALIGN((vt_cap_frame_max_width*vt_cap_frame_max_height),__12KPAGE)+_ALIGN((vt_cap_frame_max_width*vt_cap_frame_max_height/2),__12KPAGE);
	unsigned char *pVirStartAdrr = NULL;
	static unsigned int g_ulFileCount = 0;
	char raw_dat_path[30];
	sprintf(raw_dat_path, "/tmp/vt_dump_%d.raw", g_ulFileCount);

	dc2h_vi_doublebuffer_Reg.regValue = rtd_inl(DC2H_VI_DC2H_vi_doublebuffer_reg);
	dc2h_vi_doublebuffer_Reg.vi_db_en = 0; //enable double buffer
	rtd_outl(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_doublebuffer_Reg.regValue);
	msleep(40);
	PhyAddr = rtd_inl(DC2H_VI_DC2H_vi_ads_start_y_reg);
	pr_emerg("[VT]dump phy=%x\n", PhyAddr);

	pVirStartAdrr = (unsigned char *)dvr_remap_uncached_memory(PhyAddr, size, __builtin_return_address(0));

	if (pVirStartAdrr != 0)
	{
		filp = file_open(raw_dat_path, O_RDWR | O_CREAT | O_APPEND, 0);
		if (filp == NULL)
		{
			pr_emerg("[VT]file open Y fail\n");
			dvr_unmap_memory(pVirStartAdrr, size);
			return FALSE;
		}

		file_write(filp, outfileOffset, pVirStartAdrr, (vt_cap_frame_max_width * vt_cap_frame_max_height));//Y plane
		file_sync(filp);
		file_close(filp);

		filp = file_open(raw_dat_path, O_RDWR | O_CREAT | O_APPEND, 0);
		if (filp == NULL)
		{
			pr_emerg("[VT]file open fail\n");
			dvr_unmap_memory(pVirStartAdrr, size);
			return FALSE;
		}

		//if(get_VT_Pixel_Format() == VT_CAP_NV12)
		file_write(filp, outfileOffset, (pVirStartAdrr+_ALIGN(vt_cap_frame_max_width*vt_cap_frame_max_height,__12KPAGE)), ((vt_cap_frame_max_width * vt_cap_frame_max_height)/2));//nv12 UV plane
		//else
			//file_write(filp, outfileOffset, (pVirStartAdrr+_ALIGN(vt_cap_frame_max_width*vt_cap_frame_max_height,__12KPAGE)), (vt_cap_frame_max_width * vt_cap_frame_max_height));//nv16 UV plane
		file_sync(filp);
		file_close(filp);

		pr_emerg("[VT]dump_finish\n");
		g_ulFileCount++;
		dvr_unmap_memory(pVirStartAdrr, size);
		return TRUE;
	}
	else {
			pr_emerg( "\n\n\n\n ***************** [VT]dump_to_file NG for null buffer address	*********************\n\n\n\n");
			return FALSE;
		}

}
/*-----------------------------------------------------------------------------------
*input:   x ,y, w, h
*output:  none
*note:    x,y should be 0 because no overscan function in dc2h, w,h is capture size of dc2h
*		  default format NV12, capture location memc input
*aim:     enable DC2H to capture
*-----------------------------------------------------------------------------------*/
void Camera_DC2H_init(unsigned int startAddr,unsigned int capW, unsigned int capH)
{
	KADP_VT_DUMP_LOCATION_TYPE_T capLocation = KADP_VT_DISPLAY_OUTPUT;
	//KADP_VT_VIDEO_WINDOW_TYPE_T wid = KADP_VT_VIDEO_WINDOW_0;
	//SIZE dc2hInSize = {0,0,0,0};
	//DC2H_IN_SEL dc2hCapSrc = _NO_INPUT;

	dc2h_vi_dc2h_vi_ads_start_y_RBUS DC2H_VI_DC2H_vi_ads_start_y_Reg;
	dc2h_vi_dc2h_vi_ads_start_c_RBUS dc2h_vi_dc2h_vi_ads_start_c_Reg;
	dc2h_dma_dc2h_cap_l3_start_RBUS dc2h_cap_l3_start_Reg;

	dc2h_cap_l3_start_Reg.regValue = rtd_inl(DC2H_DMA_dc2h_Cap_L3_Start_reg);
	dc2h_cap_l3_start_Reg.regValue = 0;
	rtd_outl(DC2H_DMA_dc2h_Cap_L3_Start_reg, dc2h_cap_l3_start_Reg.regValue);  //used record freeze buffer

	set_VT_Pixel_Format(VT_CAP_NV12);

	if(capW == 0 || capH == 0)
	{
		rtd_pr_vt_notice("[error]capW or capH is zero\n");
		return;
	}
	if(capW > VT_CAP_FRAME_WIDTH_4K2K || capH > VT_CAP_FRAME_HEIGHT_4K2K)
	{
		rtd_pr_vt_notice("[error]capW or capH is larger than 4k\n");
		return;
	}

	set_cap_buffer_size_by_AP(capW, capH);
	//capSize.w = capW;
	//capSize.h = capH;

	//HAL_VT_InitEx(3);


	pdc2h_hdl = &dc2h_hdl;

    memset(pdc2h_hdl, 0 , sizeof(DC2H_HANDLER));
    pdc2h_hdl->out_size.nWidth = capW;
    pdc2h_hdl->out_size.nLength = capH;
    pdc2h_hdl->dc2h_en = 1;

#if 0
    if(uiDc2hCapAddr==0)
		uiDc2hCapAddr = Camera_DC2H_alloc_mem(capW, capH);

	if(uiDc2hCapAddr == INVALID_VAL)
		return;
	else
#endif
	{
		DC2H_VI_DC2H_vi_ads_start_y_Reg.regValue = startAddr;
		IoReg_Write32(DC2H_VI_DC2H_vi_ads_start_y_reg, DC2H_VI_DC2H_vi_ads_start_y_Reg.regValue);

		dc2h_vi_dc2h_vi_ads_start_c_Reg.regValue = startAddr + _ALIGN(capW*capH,__12KPAGE);
		IoReg_Write32(DC2H_VI_DC2H_vi_ads_start_c_reg, dc2h_vi_dc2h_vi_ads_start_c_Reg.regValue);

		Camera_set_DC2H_boundary(startAddr, capW, capH);
	}
	//set boundary address

	set_DC2H_rgb2yuv_ctrl_reg_refer_dump_location(capLocation, &pdc2h_hdl->in_size, &pdc2h_hdl->dc2h_in_sel);

    drvif_dc2h_input_overscan_config(0, 0, capW*2, capH*2);

	//drvif_dc2h_input_overscan_config(0, 0, pdc2h_hdl->in_size.nWidth, pdc2h_hdl->in_size.nLength);

	drvif_color_colorspaceyuv2rgbtransfer(pdc2h_hdl->dc2h_in_sel);
 	pdc2h_hdl->in_size.nWidth = capW*2;
 	pdc2h_hdl->in_size.nLength= capH*2;
	drvif_color_ultrazoom_config_dc2h_scaling_down(pdc2h_hdl);

	drvif_vi_config(pdc2h_hdl->out_size.nWidth, pdc2h_hdl->out_size.nLength, 0);

    rtd_pr_vt_notice("[%s]done\n", __FUNCTION__);
}

/*-----------------------------------------------------------------------------------
*input:   none
*output:  none
*note:    release memory
*aim:     release DC2H
*-----------------------------------------------------------------------------------*/
void Camera_DC2H_uninit(void)
{
    dc2h_rgb2yuv_dc2h_rgb2yuv_ctrl_RBUS  dc2h_rgb2yuv_ctrl_reg;
    dc2h_vi_dc2h_vi_doublebuffer_RBUS dc2h_vi_doublebuffer_Reg;

    dc2h_vi_doublebuffer_Reg.regValue = rtd_inl(DC2H_VI_DC2H_vi_doublebuffer_reg);
    dc2h_vi_doublebuffer_Reg.vi_db_en = 0;
    rtd_outl(DC2H_VI_DC2H_vi_doublebuffer_reg, dc2h_vi_doublebuffer_Reg.regValue);

    dc2h_rgb2yuv_ctrl_reg.en_rgb2yuv = DC2H_Colconv_DISABLE;
    //reset sts fifo state because it was reset in camera pq setting
    dc2h_rgb2yuv_ctrl_reg.cts_fifo_sync_rst_n = 1;
    dc2h_rgb2yuv_ctrl_reg.cts_fifo_en_vs_rst = 1;
    dc2h_rgb2yuv_ctrl_reg.set_uv_out_offset = 1;
    rtd_outl(DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg, dc2h_rgb2yuv_ctrl_reg.regValue);

	//disable dc2h
	reset_dc2h_hw_setting();

#if 0
	//release memory
	if(uiDc2hCapAddr){
	    rtd_pr_vt_notice("[%s]release addr 0x%ld\n", __FUNCTION__,uiDc2hCapAddr);
	    pli_free(uiDc2hCapAddr);
	    uiDc2hCapAddr=0;
	}
#endif

	//set as default
	set_cap_buffer_size_by_AP(VT_CAP_FRAME_WIDTH_2K1k,VT_CAP_FRAME_HEIGHT_2K1k);

    rtd_pr_vt_notice("[%s]done\n", __FUNCTION__);
}

/*-----------------------------------------------------------------------------------
*input:   none
*output:  buffer address whcih write done by DC2H
*note:    Camera_DC2H_init must be called fistly
*aim:     return buffer address whcih write done by DC2H
*-----------------------------------------------------------------------------------*/

void Camera_DC2H_output_buffer_addr(unsigned int *pYaddr, unsigned int *pCaddr)
{
	unsigned int idx = 0;

	if(get_vt_function() == TRUE)
	{
		if(pYaddr == NULL && pCaddr == NULL)
		{
			rtd_pr_vt_notice("release VT driver\n");
		}
		else
		{
			HAL_VT_WaitVsync(KADP_VT_VIDEO_WINDOW_0);

			HAL_VT_GetVideoFrameBufferIndex(KADP_VT_VIDEO_WINDOW_0, &idx);

			*pYaddr = (UINT32)CaptureCtrl_VT.cap_buffer[idx].phyaddr;
			*pCaddr = (UINT32)CaptureCtrl_VT.cap_buffer[idx].phyaddr + _ALIGN(vt_cap_frame_max_width*vt_cap_frame_max_height,__12KPAGE);

			debug_dump_data_to_file(idx, vt_cap_frame_max_width, vt_cap_frame_max_height);
		}
	}
	else
	{
		rtd_pr_vt_notice("[error]VT driver not init\n");
	}
}

/* =======================================VT_BUFFER_DUMP_DEBUG======================================================*/


unsigned int VIVT_PM_REGISTER_DATA[][2]
=
{
	{DC2H_DMA_dc2h_Cap_L1_Start_reg, 0},
	{DC2H_DMA_dc2h_Cap_L2_Start_reg, 0},
	{DC2H_DMA_dc2h_Cap_L3_Start_reg, 0},

	{DC2H_RGB2YUV_DC2H_Tab1_M11_M12_reg, 0},
	{DC2H_RGB2YUV_DC2H_Tab1_M13_M21_reg, 0},
	{DC2H_RGB2YUV_DC2H_Tab1_M22_M23_reg, 0},
	{DC2H_RGB2YUV_DC2H_Tab1_M31_M32_reg, 0},
	{DC2H_RGB2YUV_DC2H_Tab1_M33_Y_Gain_reg, 0},
	{DC2H_RGB2YUV_DC2H_Tab1_Yo_reg, 0},
	{DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg, 0},

	{DC2H_SCALEDOWN_DC2H_HSD_Scale_Hor_Factor_reg, 0},
	{DC2H_SCALEDOWN_DC2H_VSD_Scale_Ver_Factor_reg, 0},
	{DC2H_SCALEDOWN_DC2H_HSD_Hor_Segment_reg, 0},
	/*{DC2H_SCALEDOWN_DC2H_HSD_Hor_Delta1_reg, 0},*/
	{DC2H_SCALEDOWN_DC2H_HSD_Initial_Value_reg, 0},
	{DC2H_SCALEDOWN_DC2H_VSD_Initial_Value_reg, 0},
	{DC2H_SCALEDOWN_DC2H_HSD_Ctrl0_reg, 0},
	{DC2H_SCALEDOWN_DC2H_VSD_Ctrl0_reg, 0},

	{DC2H_VI_DC2H_DMACTL_reg, 0},
	{DC2H_DMA_dc2h_Seq_mode_CTRL2_reg, 0},
	{DC2H_DMA_dc2h_Cap_CTL0_reg, 0},
	{DC2H_DMA_dc2h_Cap_CTL1_reg, 0},
	{DC2H_DMA_dc2h_seq_byte_channel_swap_reg, 0},
	{DC2H_DMA_dc2h_Seq_mode_CTRL1_reg, 0},
};
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
#if 0
	size = (sizeof(VIVT_PM_REGISTER_DATA)/4)/2;
	for(i=0; i<size; i++)
	{
		VIVT_PM_REGISTER_DATA[i][1]=rtd_inl(VIVT_PM_REGISTER_DATA[i][0]);
	}
#endif
	return 0;
}

static int vivt_resume (struct device *p_dev)
{
	//int i, size;
#if 0
	size = (sizeof(VIVT_PM_REGISTER_DATA)/4)/2;
	for(i=0; i<size; i++)
	{
		rtd_outl(VIVT_PM_REGISTER_DATA[i][0],VIVT_PM_REGISTER_DATA[i][1]);
	}
#endif
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
	unsigned int cap_idx = 0;
    static int dequeue_idx[VT_CAP_BUF_NUMS] = {0};
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
		rtd_pr_vt_notice("[VT] capture szie:echo vtsize wid len > /dev/vivtdev\n");
		rtd_pr_vt_notice("[VT] set pixelfmt:echo vtfmt=0 > /dev/vivtdev\n");
		rtd_pr_vt_notice("[VT] vtfmt:0(RGB888),1(ARGB8888),2(RGBA8888),3(ABGR8888),4(NV12),5(NV16),6(NV21)\n");
		rtd_pr_vt_notice("[VT] vttest case location buffernum, ex:echo vttest 1 1 1 > /dev/vivtdev\n");
		rtd_pr_vt_notice("[VT] case:0(DISABLE),1(single capture),2(ACR),3(VR360)\n");
		rtd_pr_vt_notice("[VT] location:0(Scaler_Input),1(Scaler_Output),2(Display_Output),3(Blended_Output),4(Osd_Output)\n");
		rtd_pr_vt_notice("[VT] buffernum:1(single capture),3(ACR),5(VR360)\n");
		rtd_pr_vt_notice("[VT] dump raw:echo vtdump=1 > /dev/vivtdev\n");
		rtd_pr_vt_notice("[VT] exit VT testcase: echo vttest 0 > /dev/vivtdev\n");
	}
#ifdef ENABLE_VT_TEST_CASE_CONFIG
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

		set_cap_buffer_size_by_AP(vfod_capture_out_W,vfod_capture_out_H);
	}
	else if((cmd_buf[0] == 'v') && (cmd_buf[1] == 't') && (cmd_buf[2] == 'f') && (cmd_buf[3] == 'm') && (cmd_buf[4] == 't')
			&& (cmd_buf[5] == '='))
	{
		rtd_pr_vt_notice("[VT]current pixelfmt=%d\n", get_VT_Pixel_Format());

		if(((cmd_buf[6]-0x30) >= VT_CAP_RGB888) && ((cmd_buf[6]-0x30) < VT_CAP_MAX))
		{
			if((cmd_buf[6]-0x30) != get_VT_Pixel_Format())
			{
				set_VT_Pixel_Format((VT_CAP_FMT)(cmd_buf[6]-0x30));
				rtd_pr_vt_notice("[VT]set pixelfmt=%d\n", get_VT_Pixel_Format());
			}
		}
		else
		{
			rtd_pr_vt_notice("[VT]set pixelfmt error\n");
			ret = -EFAULT;
			return ret;
		}
	}
	else if(((cmd_buf[0] == 'v') && (cmd_buf[1] == 't') && (cmd_buf[2] == 't') && (cmd_buf[3] == 'e') && (cmd_buf[4] == 's')
			&& (cmd_buf[5] == 't')))   /* echo vttest case location buffernum > /dev/vivtdev */
	{
		if(((cmd_buf[7]-0x30) >= VT_TEST_CASE_DISABLE) && ((cmd_buf[7]-0x30) <= VT_TEST_VR360_CASE)){
			set_vt_test_case((VT_TEST_CASE_T)(cmd_buf[7]-0x30));
		}else{
			rtd_pr_vt_notice("[VT] set test case error\n");
			ret = -EFAULT;
			return ret;
		}

		if(get_vt_test_case() != VT_TEST_CASE_DISABLE)
		{
			if(((cmd_buf[9]-0x30) >= KADP_VT_SCALER_INPUT) && ((cmd_buf[9]-0x30) <= KADP_VT_OSD_OUTPUT)){
				vfod_capture_location = (cmd_buf[9]-0x30);
				VTDumpLocation = (KADP_VT_DUMP_LOCATION_TYPE_T)vfod_capture_location;
			}else{
				rtd_pr_vt_notice("[VT] set test case location error\n");
				ret = -EFAULT;
				return ret;
			}

			if(((cmd_buf[11]-0x30) >= 1) && ((cmd_buf[11]-0x30) <= 5)){
				set_vt_VtBufferNum(cmd_buf[11]-0x30);
			}else{
				rtd_pr_vt_notice("[VT] set test case bufnum error\n");
				ret = -EFAULT;
				return ret;
			}
		}

		rtd_pr_vt_notice("[VT]%s\n", cmd_buf);

		//start test case
		handle_vt_test_case();
	}
	else if(strcmp(cmd_buf, "vtdump=1\n") == 0)
	{
		rtd_pr_vt_notice("[VT]enable vt dump buffer\n");

		HAL_VT_WaitVsync(KADP_VT_VIDEO_WINDOW_0);
		if(do_vt_dqbuf(&cap_idx) == TRUE){
            if(cap_idx < get_vt_VtBufferNum()){
                debug_dump_data_to_file(cap_idx, vt_cap_frame_max_width, vt_cap_frame_max_height);
            }
		}
		else
			rtd_pr_vt_notice("[VT]dqubf fail,please retry\n");
	}
	else if(strcmp(cmd_buf, "vtcam_on\n") == 0)
	{
		unsigned long uiDc2hCapAddr;

		uiDc2hCapAddr = Camera_DC2H_alloc_mem(960, 540);
		Camera_DC2H_init(uiDc2hCapAddr, 960, 540);
		//Camera_DC2H_output_buffer_addr(unsigned int * pYaddr, unsigned int * pCaddr)
	}
	else if(strcmp(cmd_buf, "vtupdate_buf\n") == 0)
	{
		Upadte_DC2H_NV12_CapAddr();
	}
	else if(strcmp(cmd_buf, "vtcam_dump\n") == 0)
	{
		Camera_Dump_DC2H_buffer();
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
        set_vt_target_fps(user_fps);
        rtd_pr_vt_notice("[VT]user_fps %d\n", user_fps);
    }
    else if(strcmp(cmd_buf, "vtdump\n") == 0) /* echo vtdump > /dev/vivtdev */
    {
        if(do_vt_dqbuf(&cap_idx) == TRUE){
            debug_dump_data_to_file(cap_idx, vfod_capture_out_W, vfod_capture_out_H);
            HAL_VT_Set_Enqueue_index(cap_idx);
        }
    }
    else if(strcmp(cmd_buf, "vtcase\n") == 0) /* echo vtcase > /dev/vivtdev */
    {
        int i = 0;
        set_vt_app_case(VT_APP_CASE_LIVE_STREAM);
        HAL_VT_Set_App_Case(get_vt_app_case());
        rtd_pr_vt_notice("[VT]vt_app_case %d\n", vt_app_case);

        for(i = 0; i < get_vt_VtBufferNum(); i++)
        {
            dequeue_idx[i] = -1;
        }
    }
    else if(strcmp(cmd_buf, "getidx\n") == 0) /* echo getidx > /dev/vivtdev */
    {
        unsigned int getidx = 0;
        HAL_VT_WaitVsync(KADP_VT_VIDEO_WINDOW_0);
        HAL_VT_GetVideoFrameBufferIndex(KADP_VT_VIDEO_WINDOW_0, &getidx);
        rtd_pr_vt_notice("[VT]getidx %d\n", getidx);
    }
    else if(strcmp(cmd_buf, "dequeue\n") == 0) /* echo dequeue > /dev/vivtdev */
    {
        unsigned int getidx = 0;
        int i = 0;
        HAL_VT_WaitVsync(KADP_VT_VIDEO_WINDOW_0);
        HAL_VT_Get_Dequeue_index(KADP_VT_VIDEO_WINDOW_0, &getidx);
        rtd_pr_vt_notice("[VT]dequeue %d\n", getidx);

        for (i = 0; i < get_vt_VtBufferNum(); i++)
        {
            if(dequeue_idx[i] == -1 && getidx < get_vt_VtBufferNum())
            {
                dequeue_idx[i] = getidx;
                debug_dump_data_to_file(getidx, vt_cap_frame_max_width, vt_cap_frame_max_height);
                break;
            }
        }
    }
    else if(strcmp(cmd_buf, "enqueue\n") == 0) /* echo enqueue > /dev/vivtdev */
    {
        int i = 0;
        for (i = 0; i < get_vt_VtBufferNum(); i++)
        {
            if(dequeue_idx[i] != -1)
            {
                HAL_VT_Set_Enqueue_index(dequeue_idx[i]);
                rtd_pr_vt_notice("[VT]enqueue %d\n", dequeue_idx[i]);
                dequeue_idx[i] = -1;
                break;
            }
        }
    }
	else
	{
		rtd_pr_vt_notice("[VT]%s\n", cmd_buf);
	}

#endif

	return ret;
}


#ifdef ENABLE_VT_TEST_CASE_CONFIG

	void handle_vt_test_case(void)
	{
		vt_StreamOn_flag = TRUE;
		if(get_vt_test_case() == VT_TEST_CASE_DISABLE)
		{
			//exit vt capture
			do_vt_streamoff();
			do_vt_reqbufs(0);
		}
		else
		{
			if(get_vt_test_case() == VT_TEST_VR360_CASE)
			{
				HAL_VT_EnableFRCMode(FALSE);
			}
			do_vt_reqbufs(get_vt_VtBufferNum());
			do_vt_capture_streamon();
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
			unsigned char vfbdatashowonsub;
			if(copy_from_user((void *)&vfbdatashowonsub, (const void __user *)arg, sizeof(unsigned char)))
			{
				retval = -EFAULT;
				rtd_pr_vt_debug("scaler vt ioctl code=VT_IOC_VFB_DATASHOW_ONSUBDISP failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				if(Scaler_SubMDispWindow_VFB(vfbdatashowonsub)==FALSE)
					retval =-1;
			}
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
			rtd_pr_vt_notice("Scaler vt disp: ioctl code = %d is invalid!!!!!!!!!!!!!!!\n", cmd);
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
	if(!isDcmtTrap("TVSB4_DC2H"))  //"SB3_MD_SCPU"?trap ip,ip?????rtk_dc_mt.c??module_info[]
	{
		rtd_pr_vt_err("not TVSB4_DC2H module!\n");
		return;
	}

	get_dcmt_trap_info(&trap_info);  //get dcmt trap information (include trash_addr,module_id,rw_type)
	rtd_pr_vt_err("TVSB4_DC2H module trashed somewhere!\n");

	//dc2h address and control
	for (i = DC2H_SCALEDOWN_DC2H_HSD_Ctrl0_reg; i <=DC2H_SCALEDOWN_DC2H_444to422_CTRL_reg; i=i+4)
	{
		rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
	}

	for (i = DC2H_RGB2YUV_DC2H_RGB2YUV_CTRL_reg; i <=DC2H_RGB2YUV_DC2H_Tab1_Yo_reg; i=i+4)
	{
		rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
	}

	rtd_pr_vt_err("0xb8029c20=%x\n", IoReg_Read32(DC2H_RGB2YUV_DC2H_clken_reg));

	for (i = DC2H_RGB2YUV_DC2H_3DMaskTo2D_Ctrl_reg; i <=DC2H_RGB2YUV_DC2H_3DMaskTo2D_Ctrl1_reg; i=i+4)
	{
		rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
	}

	for (i = DC2H_VI_DC2H_V1_OUTPUT_FMT_reg; i <=DC2H_VI_DC2H_VI_READ_MARGIN_reg; i=i+4)
	{
		rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
	}

	rtd_pr_vt_err("0xb8029d68=%x\n", IoReg_Read32(DC2H_VI_DC2H_DBG_reg));
	rtd_pr_vt_err("0xb8029d6c=%x\n", IoReg_Read32(DC2H_VI_DC2H_vi_doublebuffer_reg));

	for (i = DC2H_VI_DC2H_vi_c_line_step_reg; i <=DC2H_DMA_dc2h_seq_byte_channel_swap_reg; i=i+4)
	{
		rtd_pr_vt_err("%x=%x\n", i, IoReg_Read32(i));
	}
	return;
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
