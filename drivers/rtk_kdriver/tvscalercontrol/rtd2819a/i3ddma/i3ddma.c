/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/
//#include <../../../../../linux-3.10.0/arch/mips/include/asm/mach-generic/spaces.h>
#ifndef BUILD_QUICK_SHOW
#include <generated/autoconf.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/uaccess.h>
#include <asm/cacheflush.h>
#include <linux/pageremap.h>
#include <linux/auth.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <rtk_kdriver/RPCDriver.h>
#include <uapi/linux/const.h>
#include <linux/mm.h>
#include "rtk_kadp_se.h"
#else
#include <mach/rtk_platform.h>
#include <no_os/printk.h>
#include <no_os/semaphore.h>
#include <no_os/spinlock.h>
#include <no_os/spinlock_types.h>
#include <no_os/slab.h>
#include <timer.h>
#include <rtk_kdriver/rmm/rmm.h>
#include <rtd_log/rtd_module_log.h>
#include <no_os/pageremap.h>
#include <div64.h>
#include <malloc.h>
#endif

//#include <mach/video_driver.h>
#include <rbus/mdomain_cap_reg.h>
#include <rbus/dma_vgip_reg.h>
//#include <rbus/scaler/rbusI3DDMAReg.h>
#include <rbus/i4_vgip_reg.h>
#include <rbus/i5_vgip_reg.h>
#include <rbus/i6_vgip_reg.h>
#include <rbus/h3ddma_reg.h>
#include <rbus/h4ddma_reg.h>
#include <rbus/h5ddma_reg.h>
#include <rbus/h6ddma_reg.h>
#include <rbus/vodma_reg.h>
//#include <H3DDMA_fw.h>
//#include <rbusI3DDMA_ScaleDownReg.h>

#include <rbus/h3ddma_0_rgb2yuv_dither_4xxto4xx_reg.h>

#include <rbus/h3ddma_hsd_reg.h>
//#include <H3DDMA_fw.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/pll_reg_reg.h>
#include <rbus/vgip_reg.h>
#ifdef CONFIG_RTK_KDRV_SVP_HDMI_PROTECTION
#include <tvscalercontrol/common/scaler_svp.h>
#include <rtk_vdec_svp.h>
#endif
#include <rbus/hdr_all_top_reg.h>
#include <rbus/dm_reg.h>
#include <rbus/timer_reg.h>

#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <VideoRPC_System.h>
	#include <VideoRPC_System_data.h>
#else
	#include <rtk_kdriver/rpc/VideoRPC_System.h>
	#include <rtk_kdriver/rpc/VideoRPC_System_data.h>


#endif
#endif

#include <rtd_log/rtd_module_log.h>
#include <tvscalercontrol/i3ddma/i3ddma.h>
#include <tvscalercontrol/scaler/modeState.h>
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <tvscalercontrol/i3ddma/i3ddma_drv.h>
#include <tvscalercontrol/scalerdrv/scaler_i2rnd.h>
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <tvscalercontrol/scalerdrv/mode.h>
#include <tvscalercontrol/scalerdrv/scaler_hdr_ctrl.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <scaler_vscdev.h>
#include <tvscalercontrol/hdmirx/hdmifun.h>
#ifdef CONFIG_SUPPORT_SRC_DPRX
#include <rtk_kdriver/dprx/dprxfun.h>
#include <rtk_kdriver/dprx/dprx_types.h>
#endif
#include <tvscalercontrol/i3ddma/i3ddma_reg_array.h>
#include <tvscalercontrol/hdmirx/hdmi_vfe.h>
#include <rtk_kdriver/dprx/dprx_vfe_api.h>
#include <vt_v4l2_export.h>

#define FIFO_WIDTH   128

#ifdef BUILD_QUICK_SHOW
#define  dvr_size_alignment(x)  ((unsigned long)x)
#define  dvr_memory_alignment(x, y)  ((unsigned long)x)
#define  dvr_to_phys(x) ((unsigned int)x)
#endif

#undef rtd_setbits
#define rtd_setbits(offset, Mask)    rtd_outl(offset, (rtd_inl(offset) | Mask))

I3DDMA_3DDMA_CTRL_T i3ddmaCtrl[MAX_IDDMA_DEV];
I3DDMA_3DDMA_CTRL_T metadataCtrl;

/*4k and 96 align*/
#define __4KPAGE  0x1000
#define MD_BUF_SIZE (1024 * 1024)
#define _BIT(x)       (1UL <<x)

#undef  BUG
#define BUG() do { \
	rtd_pr_scaler_memory_info("U-Boot BUG at %s:%d!\n", __FILE__, __LINE__); \
} while (0)

static StructDisplayInfo iddma_info[MAX_IDDMA_DEV] = {0};
IDDMA_DEV_ATTR_T iddma_attr[MAX_IDDMA_DEV] = {0};
unsigned char iddma_work_mode[MAX_IDDMA_DEV] = {0};

#ifdef CONFIG_ENABLE_HDMI_NN
#define NN_CAP_OUTPUT_WID_SIZE 960
#define NN_CAP_OUTPUT_LEN_SIZE 540
#define NN_CAP_BUFFER_MAX_W_SIZE 1024
#define NN_CAP_BUFFER_MAX_H_SIZE 540
I3DDMA_CAPTURE_BUFFER_T g_tNN_Cap_Buffer[NN_CAP_BUFFER_NUM];

unsigned char g_ucNNReadIdx, g_ucNNWriteIdx;
unsigned char g_ucCapEnableMask = 0;

//unsigned long g_cacheaddr, g_uncacheaddr;

void h3ddma_nn_set_cap_enable(unsigned char dev, unsigned int enable);
extern void h3ddma_get_NN_read_idx(void);

typedef struct _SZIE {
	unsigned int	nWidth;
	unsigned int	nLength;
} SIZE;

unsigned char g_chVdecReCfg = 0;


unsigned int g_ulNNInWidth = 0;
unsigned int g_ulNNInLength = 0;
unsigned int g_ulNNOutWidth = 0; //960;
unsigned int g_ulNNOutLength = 0; //540;

I3DDMA_NN_CAP_RATIO_E g_enNNCapRatio = I3DDMA_CAP_1_1;
I3DDMA_NN_CROP_ATTR_T g_stI3ddmaNNCropAttr = {0,0,0,0,0,0,NN_CAP_OUTPUT_WID_SIZE,NN_CAP_OUTPUT_LEN_SIZE};
I3DDMA_NN_SUPPORT_CAP_MODE_E g_enSupCapMode = I3DDMA_CAP_MIX;
extern unsigned char get_MEMC_bypass_status_refer_platform_model(void);

extern void I3DDMA_ultrazoom_config_scaling_down_for_cap(unsigned char dev, SIZE* in_Size, SIZE* out_Size, unsigned char panorama);
extern void h3ddma_nn_set_crop_size(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
extern unsigned int scalerAI_get_AIPQ_mode_enable_flag(void);
extern unsigned char scaler_nn_force_run_idma(unsigned char display, VSC_INPUT_TYPE_T inputSrctype);
#ifdef CONFIG_RTK_KDRV_DV
extern bool check_hdmi_dolby_vision_rgb_and_full_hd(void);
#endif
#endif

extern void i3ddma_cap1_interrupt_ctrl(unsigned char enable);
extern void update_qms_block_info(I3DDMA_CAPTURE_BUFFER_T *capture_info);


/*
 * This gives the physical RAM offset.
 */
#define UNCAC_BASE		_AC(0xa0000000, UL)

#define IDMA_CAP_FRAME_SIZE_SRC_2K2K_3D 	(1920 * 1080 * 4) 	// (2k2k * 1/2) 10bit 444 (4byte/pixel) -- for HDMI 1080p FP source
#define IDMA_CAP_FRAME_SIZE_SRC_2K1K_3D 	(1920 * 540 * 4) 	// (2k1k * 1/2) 10bit 444 (4byte/pixel) -- non-1080pFP 3D source

#define VODMA_IV2PV_DELAY_CAL_MARGIN 1
#define I3DDMA_DRAM_MARGIN 3
// dynamic allocate IDMA capture buffer memory from video FW

void h3ddma_set_capture_enable(unsigned char chanel,unsigned char enable);//decide i3ddma cpture enable or disable

//extern int send_rpc_command(int opt, unsigned long command, unsigned long param1, unsigned long param2, unsigned long *retvalue);
//int send_rpc_command(int opt, unsigned long command, unsigned long param1, unsigned long param2, unsigned long *retvalue);
//[K3LG-282] because can't allocate mem issue
#ifdef CONFIG_VBM_HEAP
int VBM_ReturnMemChunk(unsigned long dmabuf_hndl, unsigned long buffer_addr_in, unsigned long buffer_size_in);
int VBM_BorrowMemChunk(unsigned long *dmabuf_hndl, unsigned long *buffer_addr, unsigned long *buffer_size, UINT32 reqSize, UINT32 type);
#else
extern int VBM_ReturnMemChunk(unsigned long buffer_addr, unsigned long buffer_size);
extern int VBM_BorrowMemChunk(unsigned long *buffer_addr, unsigned long *buffer_size, UINT32 reqSize , UINT32 type);
#endif
extern unsigned int h3ddma_get_comp_burst_num(I3DDMA_TIMING_T *timing);

I3DDMA_TIMING_T hdmi_source_info_backup;//backup hdmi source timing and color format
#ifndef BUILD_QUICK_SHOW
static DEFINE_SPINLOCK(HDR_SEAMLESS_CHANGE_SPINLOCK);/*Spin lock no context switch. When we need to change i3ddma format*/
#endif
unsigned char hdr_seamless_trigger_flag= FALSE;//i3ddma format need to change
unsigned char vtop_input_color_format = I3DDMA_COLOR_UNKNOW;//target i3ddma color format

unsigned char i3ddma_pcmode_alloc_memory_flag = FALSE;

I3DDMA_CAPTURE_BUFFER_T s_i3ddma_sw_cap_buffer[SW_I3DDMA_CAP_NUM];
I3DDMA_CAPTURE_BUFFER_T i3ddma_vdelay_cap_buffer[SW_I3DDMA_CAP_NUM];
static unsigned char s_i3ddma_sw_delay_num = 0;
static unsigned int s_i3ddma_vdelay_numbers = 0;
#define I3DDMA_VIDEO_DELAY_BUFFER_NUMBER 8

extern UINT8 get_nn_force_i3ddma_enable(void);
extern struct semaphore* get_force_i3ddma_semaphore(void);

//#include <tvscalercontrol/scaler/modeState.h>
typedef struct  {

    char *name;
    int progressive;
    unsigned int h_act_len;
    unsigned int v_act_len;
    unsigned int lr_v_act_len;
    unsigned int v_active_space1;
    unsigned int v_active_space2;

}I3DDMA_ACTIVE_SPACE_TABLE_T;

I3DDMA_ACTIVE_SPACE_TABLE_T i3ddma_active_space_table[] = {
	{ "1080P FramePacking",  1, 1920,  1080*2 + 45, 1080, 45, 0 },
	{ "720P FramePacking",  1, 1280,  720*2 + 30, 720, 30, 0 },
	{ "480P FramePacking",  1, 720,  480*2 + 45, 480, 45, 0 },
	{ "576P FramePacking",  1, 720,  576*2 + 49, 576, 49, 0 },
	{ "1080I FramePacking",  0, 1920,  540*4 + 22 + 23*2, 540, 23, 22 },
	{ "576I FramePacking",  0, 1440,  288*4 + 32 + 21*2, 288, 21, 32 },
	{ "480I FramePacking",  0, 1440,  240*4 + 22 + 23*2, 240, 23, 22 },
	{ NULL,  1, 1920,  520, 40, 0 },
};


//#ifdef HDMI_HAVE_HDMI_3D_TG
#if 0
unsigned char g_ucFp1080i_status=FALSE, g_ucFp1080i_fix_vStart_status=FALSE;
void I3DDMA_Set_FP1080i_status(I3DDMA_TIMING_T *gen_timing)
{
	g_ucFp1080i_status = ((gen_timing->i3ddma_3dformat == I3DDMA_FRAME_PACKING) && (gen_timing->progressive == 0));
	return;
}

unsigned char I3DDMA_Get_FP1080i_status(void)
{
	return g_ucFp1080i_status;
}


// [FIX-ME] change v-sync start position for HDMI 1080iFP 3D to 2D mode (3->1)
void I3DDMA_Set_FP1080i_3Dcvt2D_fix_vStart_status(unsigned char mode)
{
	g_ucFp1080i_fix_vStart_status = mode;
}

unsigned char I3DDMA_Get_FP1080i_3Dcvt2D_fix_vStart_status(void)
{
	return g_ucFp1080i_fix_vStart_status;
}
#endif

VO_CHROMA_FMT VOColorMap[I3DDMA_COLOR_UNKNOW] = {
	VO_CHROMA_RGB888,
	VO_CHROMA_YUV422,
	VO_CHROMA_YUV444,
	VO_CHROMA_YUV411,

};

#if 0
char I3DDMA_ResetVODMA4K2K(void)
{

	VIDEO_RPC_VOUT_CONFIG_HDMI_3D config;
	CLNT_STRUCT clnt;

	rtd_pr_i3ddma_info("%s\n", __FUNCTION__);
	clnt = prepareCLNT(BLOCK_MODE | USE_INTR_BUF | SEND_VIDEO_CPU, VIDEO_SYSTEM, VERSION);

	memset(&config, 0, sizeof(VIDEO_RPC_VOUT_CONFIG_HDMI_3D));
	config.width = 0;
	config.height = 0;
	config.videoPlane = VO_VIDEO_PLANE_V1;
	HRESULT* hr = VIDEO_RPC_VOUT_ToAgent_ConfigHDMI_3D_0(&config, &clnt);
	if (hr < 0) {
		rtd_pr_i3ddma_info("CONFIG HDMI 3D RPC fail\n");
		return FALSE;
	}
    free(hr);

	return TRUE;

}

#endif

void IDDMA_SetVFEInfoByPort(unsigned char port, SLR_INPUT_INFO infoList, unsigned int value)
{
    if(port > I6DDMA_DEV) {
		rtd_pr_vsc_err("[Error] Wrong iddma port:%d\n", port);
		return;
	}
	switch (infoList)
	{
		case SLR_INPUT_H_FREQ:
			iddma_info[port].IHFreq = value;
			break;
		case SLR_INPUT_V_FREQ_1000:
			iddma_info[port].IVFreq_1000 = value;
			break;
		case SLR_INPUT_H_LEN:
			iddma_info[port].IHTotal = value;
			break;
		case SLR_INPUT_V_LEN:
			iddma_info[port].IVTotal = value;
			break;
		case SLR_INPUT_H_COUNT:
			iddma_info[port].IHCount_PRE = value;
			break;
		case SLR_INPUT_V_COUNT:
			iddma_info[port].IVCount_PRE = value;
			break;
		case SLR_INPUT_IPH_ACT_STA:
			iddma_info[port].IPH_ACT_STA = value;
			break;
		case SLR_INPUT_IPV_ACT_STA:
			iddma_info[port].IPV_ACT_STA = value;
			break;
		case SLR_INPUT_IPH_ACT_WID:
			iddma_info[port].IPH_ACT_WID = value;
			break;
		case SLR_INPUT_IPV_ACT_LEN:
			iddma_info[port].IPV_ACT_LEN = value;
			break;
		case SLR_INPUT_ADC_CLOCK:
			iddma_info[port].INPUT_ADC_CLOCK = value;
			break;
		case SLR_INPUT_CHANNEL:
			iddma_info[port].channel = value;
			break;
		case SLR_INPUT_DATA_ARRAY_IDX:
			iddma_info[port].data_array_index = value;
			break;
		case SLR_INPUT_MODE_TABLE_INDEX:
			iddma_info[port].ucMode_Table_Index = value;
			break;
		case SLR_INPUT_MODE_CURR:
			iddma_info[port].ucMode_Curr = value;
			break;
		case SLR_INPUT_ASPECT_RATIO:
			iddma_info[port].IAspectRatio = value;
			break;
		case SLR_INPUT_AFD:
			iddma_info[port].afd = (SLR_AFD_TYPE)value;
			break;
		case SLR_INPUT_MEM_ACT_HSTA:
			iddma_info[port].MEM_ACT_HSTA = value;
			break;
		case SLR_INPUT_MEM_ACT_VSTA:
			iddma_info[port].MEM_ACT_VSTA = value;
			break;
		case SLR_INPUT_MEM_ACT_LEN:
			iddma_info[port].MEM_ACT_LEN = value;
			break;
		case SLR_INPUT_MEM_ACT_WID:
			iddma_info[port].MEM_ACT_WID = value;
			break;
		case SLR_INPUT_DISPLAY:
			iddma_info[port].display = value;
			break;
		case SLR_INPUT_FAILURECOUNT:
			iddma_info[port].FailureCount = value;
			break;
		case SLR_INPUT_FAILURECOUNT_NOSUPPORT:
			iddma_info[port].FailureCountNoSupport = value;
			break;
		case SLR_INPUT_FRAMESYNC:
			iddma_info[port].FrameSyncFlag = value;
			break;
		case SLR_INPUT_CAP_LEN:
			iddma_info[port].CapLen = value;
			break;
		case SLR_INPUT_CAP_WID:
			iddma_info[port].CapWid = value;
			break;
		case SLR_INPUT_DISP_LEN:
			iddma_info[port].DispLen = value;
			break;
		case SLR_INPUT_DISP_WID:
			iddma_info[port].DispWid = value;
			break;
		case SLR_INPUT_DISP_LEN_PRE:
			iddma_info[port].DispLen_PRE = value;
			break;
		case SLR_INPUT_DISP_WID_PRE:
			iddma_info[port].DispWid_PRE = value;
			break;
		case SLR_INPUT_POLARITY:
			iddma_info[port].Polarity = value;
			break;
		case SLR_INPUT_IPV_SMARTFIT_START:
			iddma_info[port].IPV_SMARTFIT_START = value;
			break;
		case SLR_INPUT_IPH_SMARTFIT_START:
			iddma_info[port].IPH_SMARTFIT_START = value;
			break;
		case SLR_INPUT_IPV_ACT_LEN_PRE:
			iddma_info[port].IPV_ACT_LEN_PRE = value;
			break;
		case SLR_INPUT_IPV_ACT_STA_PRE:
			iddma_info[port].IPV_ACT_STA_PRE = value;
			break;
		case SLR_INPUT_IPH_ACT_WID_PRE:
			iddma_info[port].IPH_ACT_WID_PRE = value;
			break;
		case SLR_INPUT_IPH_ACT_STA_PRE:
			iddma_info[port].IPH_ACT_STA_PRE = value;
			break;
		case SLR_INPUT_IHSYNCPULSECOUNT:
			iddma_info[port].IHSyncPulseCount = value;
			break;
		case SLR_INPUT_HSYNC:
			iddma_info[port].Hsync = value;
			break;
		case SLR_INPUT_VSYNC:
			iddma_info[port].Vsync = value;
			break;
		case SLR_INPUT_STATE:
			iddma_info[port].state = value;
			break;
		case SLR_INPUT_NSHOWMPTYPE:
			iddma_info[port].nShowMpType = value;
			break;
		case SLR_INPUT_INPUT_SRC:
			iddma_info[port].input_src = (unsigned char)value;
			break;
		case SLR_INPUT_OSD_INPUT_SRC:
			iddma_info[port].osd_input_src = (unsigned char)value;
			break;
		case SLR_INPUT_IPV_DLY_PRE:
			iddma_info[port].IPV_DLY_PRE = value;
			break;
		case SLR_INPUT_BGCOLOR_R:
			iddma_info[port].bg_color_r = value;
			break;
		case SLR_INPUT_BGCOLOR_G:
			iddma_info[port].bg_color_g = value;
			break;
		case SLR_INPUT_BGCOLOR_B:
			iddma_info[port].bg_color_b = value;
			break;
		case SLR_INPUT_DISP_STATUS:
			iddma_info[port].disp_status = value;
			break;
		case SLR_INPUT_WSS:
			iddma_info[port].wss = (SLR_WSS_TYPE)value;
			break;
		case SLR_INPUT_COLOR_SPACE:
			iddma_info[port].color_space= value;
			break;
		case SLR_INPUT_COLOR_DEPTH:
			iddma_info[port].color_depth= value;
			break;
		case SLR_INPUT_COLOR_IMETRY:
			iddma_info[port].color_imetry= value;
			break;
		case SLR_INPUT_THROUGH_I3DDMA:
			iddma_info[port].through_i3ddma= value;
			break;
		case SLR_INPUT_SR_MODE:
			iddma_info[port].SR_MODE= value;
			break;
		case SLR_INPUT_PIXEL_MODE:
			iddma_info[port].PIXEL_MODE= value;
			break;
		case SLR_INPUT_DFRC_ENABLE:
			iddma_info[port].DFRC_ENABLE= value;
			break;
		case SLR_INPUT_DI_WID:
			iddma_info[port].DiWid= value;
			break;
		case SLR_INPUT_DI_LEN:
			iddma_info[port].DiLen= value;
			break;
		case SLR_INPUT_VO_SOURCE_TYPE:
			iddma_info[port].vo_type= value;
			break;
		case SLR_INPUT_FRAC_FS_REMOVE_RATIO:
			iddma_info[port].frac_fs_remove_ratio = value;
			break;
		case SLR_INPUT_FRAC_FS_MULTIPLE_RATIO:
			iddma_info[port].frac_fs_multiple_ratio = value;
			break;
		case SLR_INPUT_DTG_MASTER_V_FREQ:
			iddma_info[port].dtg_m_v_freq = value;
			break;
		case SLR_INPUT_SDNRH_ACT_STA_PRE:
			iddma_info[port].SDNRH_ACT_STA_PRE= value;
			break;
		case SLR_INPUT_SDNRV_ACT_STA_PRE:
			iddma_info[port].SDNRV_ACT_STA_PRE= value;
			break;
		case SLR_INPUT_SDNRH_ACT_WID_PRE:
			iddma_info[port].SDNRH_ACT_WID_PRE= value;
			break;
		case SLR_INPUT_SDNRV_ACT_LEN_PRE:
			iddma_info[port].SDNRV_ACT_LEN_PRE= value;
			break;
		case SLR_INPUT_SDNRH_ACT_STA:
			iddma_info[port].SDNRH_ACT_STA= value;
			break;
		case SLR_INPUT_SDNRV_ACT_STA:
			iddma_info[port].SDNRV_ACT_STA= value;
			break;
		case SLR_INPUT_SDNRH_ACT_WID:
			iddma_info[port].SDNRH_ACT_WID= value;
			break;
		case SLR_INPUT_SDNRV_ACT_LEN:
			iddma_info[port].SDNRV_ACT_LEN= value;
			break;
		case SLR_INPUT_IVSYNCPULSECOUNT:
			iddma_info[port].IVSyncPulseCount= value;
			break;
		case SLR_INPUT_V_SYNC_HIGH_PERIOD:
			iddma_info[port].IVPolarityIndicator= value;
			break;
		case SLR_INPUT_VODMA_PLANE:
			iddma_info[port].vodma_plane = value;
			break;
		case SLR_INPUT_VO_SRC_FRAMERATE_1000:
			iddma_info[port].SrcIVFreq_1000 = value;
			break;

        case SLR_INPUT_SUB_SNRR_ENABLE:
            iddma_info[port].subSrnnEnable = value;
            break;
		case SLR_INPUT_SOURCE_PIXEL_MODE:
			iddma_info[port].input_src_pixel_mode = value;
			break;
		case SLR_INPUT_VSC_SOURCE_TYPE:
			iddma_info[port].vsc_src_type = value;
			break;
		default:
			rtd_pr_vsc_debug("[IDDMA vfe_info] Don't know how to set vfe_info!!!\n");
			break;
	}

	return;
}

unsigned int IDDMA_GetVFEInfoByPort(unsigned char port, SLR_INPUT_INFO infoList)
{
	if ( port > I6DDMA_DEV) {
		rtd_pr_vsc_debug("[IDDMA vfe_info] Invalid port\n");
		return 0;
	}

	switch (infoList)
	{
		case SLR_INPUT_H_FREQ:
			return iddma_info[port].IHFreq;
		case SLR_INPUT_V_FREQ:
			return iddma_info[port].IVFreq_1000 / 100;	// in digit 1 : reserved for dolby common code project
		case SLR_INPUT_V_FREQ_1000:
			return iddma_info[port].IVFreq_1000;
		case SLR_INPUT_H_LEN:
			return iddma_info[port].IHTotal;
		case SLR_INPUT_V_LEN:
			return iddma_info[port].IVTotal;
		case SLR_INPUT_H_COUNT:
			return iddma_info[port].IHCount_PRE;
		case SLR_INPUT_V_COUNT:
			return iddma_info[port].IVCount_PRE;
		case SLR_INPUT_IPH_ACT_STA:
			return iddma_info[port].IPH_ACT_STA;
		case SLR_INPUT_IPV_ACT_STA:
			return iddma_info[port].IPV_ACT_STA;
		case SLR_INPUT_IPH_ACT_WID:
			return iddma_info[port].IPH_ACT_WID;
		case SLR_INPUT_IPV_ACT_LEN:
			return iddma_info[port].IPV_ACT_LEN;
		case SLR_INPUT_ADC_CLOCK:
			return iddma_info[port].INPUT_ADC_CLOCK;
		case SLR_INPUT_CHANNEL:
			return iddma_info[port].channel;
		case SLR_INPUT_DATA_ARRAY_IDX:
			return iddma_info[port].data_array_index;
		case SLR_INPUT_MODE_CURR:
			return iddma_info[port].ucMode_Curr;
		case SLR_INPUT_MODE_TABLE_INDEX:
			return  iddma_info[port].ucMode_Table_Index;
		case SLR_INPUT_ASPECT_RATIO:
			return iddma_info[port].IAspectRatio;
		case SLR_INPUT_AFD:
			return iddma_info[port].afd;
		case SLR_INPUT_MEM_ACT_HSTA:
			return iddma_info[port].MEM_ACT_HSTA;
		case SLR_INPUT_MEM_ACT_VSTA:
			return iddma_info[port].MEM_ACT_VSTA;
		case SLR_INPUT_MEM_ACT_LEN:
			return iddma_info[port].MEM_ACT_LEN;
		case SLR_INPUT_MEM_ACT_WID:
			return iddma_info[port].MEM_ACT_WID;
		case SLR_INPUT_DISPLAY:
			return iddma_info[port].display;
		case SLR_INPUT_FAILURECOUNT:
			return iddma_info[port].FailureCount;
		case SLR_INPUT_FAILURECOUNT_NOSUPPORT:
			return iddma_info[port].FailureCountNoSupport;
		case SLR_INPUT_FRAMESYNC:
			return iddma_info[port].FrameSyncFlag;
		case SLR_INPUT_CAP_LEN:
			return iddma_info[port].CapLen;
		case SLR_INPUT_CAP_WID:
			return iddma_info[port].CapWid;
		case SLR_INPUT_DISP_LEN:
			return iddma_info[port].DispLen;
		case SLR_INPUT_DISP_WID:
			return iddma_info[port].DispWid;
		case SLR_INPUT_DISP_LEN_PRE:
			return iddma_info[port].DispLen_PRE;
		case SLR_INPUT_DISP_WID_PRE:
			return iddma_info[port].DispWid_PRE;
		case SLR_INPUT_POLARITY:
			return iddma_info[port].Polarity;
		case SLR_INPUT_IPV_SMARTFIT_START:
			return iddma_info[port].IPV_SMARTFIT_START;
		case SLR_INPUT_IPH_SMARTFIT_START:
			return iddma_info[port].IPH_SMARTFIT_START;
		case SLR_INPUT_IPV_ACT_LEN_PRE:
			return iddma_info[port].IPV_ACT_LEN_PRE;
		case SLR_INPUT_IPV_ACT_STA_PRE:
			return iddma_info[port].IPV_ACT_STA_PRE;
		case SLR_INPUT_IPH_ACT_WID_PRE:
			return iddma_info[port].IPH_ACT_WID_PRE;
		case SLR_INPUT_IPH_ACT_STA_PRE:
			return iddma_info[port].IPH_ACT_STA_PRE;
		case SLR_INPUT_IHSYNCPULSECOUNT:
			return iddma_info[port].IHSyncPulseCount;
		case SLR_INPUT_HSYNC:
			return iddma_info[port].Hsync;
		case SLR_INPUT_VSYNC:
			return iddma_info[port].Vsync;
		case SLR_INPUT_STATE:
			return iddma_info[port].state;
		case SLR_INPUT_NSHOWMPTYPE:
			return iddma_info[port].nShowMpType;
		case SLR_INPUT_INPUT_SRC:
			return iddma_info[port].input_src;
		case SLR_INPUT_OSD_INPUT_SRC:
			return iddma_info[port].osd_input_src;
		case SLR_INPUT_IPV_DLY_PRE:
			return iddma_info[port].IPV_DLY_PRE;
		case SLR_INPUT_BGCOLOR_R:
			return iddma_info[port].bg_color_r;
		case SLR_INPUT_BGCOLOR_G:
			return iddma_info[port].bg_color_g;
		case SLR_INPUT_BGCOLOR_B:
			return iddma_info[port].bg_color_b;
		case SLR_INPUT_DISP_STATUS:
			return iddma_info[port].disp_status;
		case SLR_INPUT_WSS:
			return iddma_info[port].wss;
		case SLR_INPUT_COLOR_SPACE:
			return iddma_info[port].color_space;
		case SLR_INPUT_COLOR_DEPTH:
			return iddma_info[port].color_depth;
		case SLR_INPUT_COLOR_IMETRY:
			return iddma_info[port].color_imetry;
		case SLR_INPUT_THROUGH_I3DDMA:
			return iddma_info[port].through_i3ddma;
		case SLR_INPUT_SR_MODE:
			return iddma_info[port].SR_MODE;
		case SLR_INPUT_PIXEL_MODE:
			return iddma_info[port].PIXEL_MODE;
		case SLR_INPUT_DFRC_ENABLE:
			return iddma_info[port].DFRC_ENABLE;
		case SLR_INPUT_DI_WID:
			return iddma_info[port].DiWid;
		case SLR_INPUT_DI_LEN:
			return iddma_info[port].DiLen;
		case SLR_INPUT_VO_SOURCE_TYPE:
			return iddma_info[port].vo_type;
		case SLR_INPUT_FRAC_FS_REMOVE_RATIO:
			return iddma_info[port].frac_fs_remove_ratio;
		case SLR_INPUT_FRAC_FS_MULTIPLE_RATIO:
			return iddma_info[port].frac_fs_multiple_ratio;
		case SLR_INPUT_DTG_MASTER_V_FREQ:
			return iddma_info[port].dtg_m_v_freq;
		case SLR_INPUT_SDNRH_ACT_STA_PRE:
			return iddma_info[port].SDNRH_ACT_STA_PRE;
		case SLR_INPUT_SDNRV_ACT_STA_PRE:
			return iddma_info[port].SDNRV_ACT_STA_PRE;
		case SLR_INPUT_SDNRH_ACT_WID_PRE:
			return iddma_info[port].SDNRH_ACT_WID_PRE;
		case SLR_INPUT_SDNRV_ACT_LEN_PRE:
			return iddma_info[port].SDNRV_ACT_LEN_PRE;
		case SLR_INPUT_SDNRH_ACT_STA:
			return iddma_info[port].SDNRH_ACT_STA;
		case SLR_INPUT_SDNRV_ACT_STA:
			return iddma_info[port].SDNRV_ACT_STA;
		case SLR_INPUT_SDNRH_ACT_WID:
			return iddma_info[port].SDNRH_ACT_WID;
		case SLR_INPUT_SDNRV_ACT_LEN:
			return iddma_info[port].SDNRV_ACT_LEN;
		case SLR_INPUT_IVSYNCPULSECOUNT:
			return iddma_info[port].IVSyncPulseCount;
		case SLR_INPUT_V_SYNC_HIGH_PERIOD:
			return iddma_info[port].IVPolarityIndicator;
		case SLR_INPUT_VODMA_PLANE:
			return iddma_info[port].vodma_plane;
		case SLR_INPUT_VO_SRC_FRAMERATE_1000:
			return iddma_info[port].SrcIVFreq_1000;
        case SLR_INPUT_SUB_SNRR_ENABLE:
            return iddma_info[port].subSrnnEnable;
		case SLR_INPUT_SOURCE_PIXEL_MODE:
			return iddma_info[port].input_src_pixel_mode;
		case SLR_INPUT_VSC_SOURCE_TYPE:
			return iddma_info[port].vsc_src_type;
		default:
			rtd_pr_vsc_debug("[IDDMA vfe_info] Don't know how to get vfe_info!!!\n");
			return 0;
	}
}

void IDDMA_SetVFEStatus(unsigned char port, SLR_DISP_STATUS index, unsigned short status)
{
    if(port > I6DDMA_DEV) {
		rtd_pr_vsc_err("[Error] Wrong iddma port:%d, force Main channel\n", port);
		return;
	}
	if (index == SLR_DISP_FSYNC_VUZD) {
		iddma_info[port].disp_status = (iddma_info[port].disp_status & ~(SLR_DISP_FSYNC_VUZD)) | (status<<4);
		return;
	}

	if (status == TRUE)
		iddma_info[port].disp_status |= index;
	else if ( status == FALSE )
		iddma_info[port].disp_status &= ~index;
}

unsigned short IDDMA_GetVFEStatus(unsigned char port, SLR_DISP_STATUS index)
{
    if(port > I6DDMA_DEV) {
		rtd_pr_vsc_err("[Error] Wrong iddma port:%d", port);
		return FALSE;
	}

	if (index == SLR_DISP_FSYNC_VUZD) {
		return ((iddma_info[port].disp_status & index) >> 4);
	}

	if (index == SLR_DISP_COMP) {
		if ((!(iddma_info[port].disp_status & SLR_DISP_THRIP)) && (iddma_info[port].disp_status & SLR_DISP_INTERLACE))
			return TRUE;
		else
			return FALSE;
	}

	if (iddma_info[port].disp_status & index)
		return TRUE;
	else
		return FALSE;
}


void IDDMA_SaveVFEInfoByPort(unsigned char port, StructDisplayInfo *VFEInfo)
{
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_ADC_CLOCK, VFEInfo->INPUT_ADC_CLOCK);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IPV_ACT_LEN_PRE, VFEInfo->IPV_ACT_LEN_PRE);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IPV_ACT_STA_PRE, VFEInfo->IPV_ACT_STA_PRE);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IPH_ACT_WID_PRE, VFEInfo->IPH_ACT_WID_PRE);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IPH_ACT_STA_PRE, VFEInfo->IPH_ACT_STA_PRE);

	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_HSYNC, VFEInfo->Hsync);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_VSYNC, VFEInfo->Vsync);

	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_MODE_CURR, VFEInfo->ucMode_Curr);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_MODE_TABLE_INDEX, VFEInfo->ucMode_Table_Index);

	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_AFD, VFEInfo->afd);

	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_POLARITY, VFEInfo->Polarity);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_H_FREQ, VFEInfo->IHFreq);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_V_FREQ_1000, VFEInfo->IVFreq_1000);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_H_LEN, VFEInfo->IHTotal);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_V_LEN, VFEInfo->IVTotal);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_H_COUNT, VFEInfo->IHCount_PRE);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_V_COUNT, VFEInfo->IVCount_PRE);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IHSYNCPULSECOUNT, VFEInfo->IHSyncPulseCount);

	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_COLOR_SPACE, VFEInfo->color_space);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_COLOR_DEPTH, VFEInfo->color_depth);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_COLOR_IMETRY, VFEInfo->color_imetry);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IVSYNCPULSECOUNT, VFEInfo->IVSyncPulseCount);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_V_SYNC_HIGH_PERIOD, VFEInfo->IVPolarityIndicator);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_VODMA_PLANE, VFEInfo->vodma_plane);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_VO_SRC_FRAMERATE_1000, VFEInfo->SrcIVFreq_1000);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_SOURCE_PIXEL_MODE, 0);  //FIXME later

	IDDMA_SetVFEStatus(port, SLR_DISP_422,((VFEInfo->disp_status&_BIT1)>>1));
	IDDMA_SetVFEStatus(port, SLR_DISP_INTERLACE,((VFEInfo->disp_status&_BIT8)>>8));
}


void IDDMA_SaveHDMIInfoByPort(unsigned char port, MEASURE_TIMING_T *vfehdmitiminginfo)
{
	unsigned char input_color_space = SCALER_INPUT_COLOR_UNKNOW;

	//IDDMA_SetVFEInfoByPort(port, SLR_INPUT_ADC_CLOCK, VFEDispInfo->INPUT_ADC_CLOCK);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IPV_ACT_LEN_PRE, vfehdmitiminginfo->v_act_len);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IPV_ACT_STA_PRE, vfehdmitiminginfo->v_act_sta);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IPH_ACT_WID_PRE, vfehdmitiminginfo->h_act_len);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IPH_ACT_STA_PRE, vfehdmitiminginfo->h_act_sta);

	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_HSYNC, vfehdmitiminginfo->h_total);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_VSYNC, vfehdmitiminginfo->v_total);

	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_MODE_CURR, vfehdmitiminginfo->mode_id);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_MODE_TABLE_INDEX, vfehdmitiminginfo->modetable_index);

	//IDDMA_SetVFEInfoByPort(port, SLR_INPUT_AFD, VFEDispInfo->afd);

	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_POLARITY, vfehdmitiminginfo->polarity);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_H_FREQ, Scaler_Get_HDMI_HFREQ(vfehdmitiminginfo->h_freq));
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_V_FREQ_1000, vfehdmitiminginfo->v_freq);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_H_LEN, vfehdmitiminginfo->h_total);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_V_LEN, vfehdmitiminginfo->v_total);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_H_COUNT, vfehdmitiminginfo->h_total);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_V_COUNT, vfehdmitiminginfo->v_total);
	//IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IHSYNCPULSECOUNT, VFEDispInfo->IHSyncPulseCount);

	switch(vfehdmitiminginfo->colorspace)
	{
		case COLOR_RGB:
			input_color_space = SCALER_INPUT_COLOR_RGB;
			break;
		case COLOR_YUV422:
			input_color_space = SCALER_INPUT_COLOR_YUV422;
			break;
		case COLOR_YUV444:
			input_color_space = SCALER_INPUT_COLOR_YUV444;
			break;
		case COLOR_YUV420:
			input_color_space = SCALER_INPUT_COLOR_YUV420;
			break;
		case COLOR_UNKNOW:
		default:
			input_color_space = SCALER_INPUT_COLOR_UNKNOW;
			break;
	}
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_COLOR_SPACE, input_color_space);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_SOURCE_PIXEL_MODE, vfehdmitiminginfo->pixel_mode == ONE_PIXEL_MODE ? 0 : 1);

	IDDMA_SetVFEStatus(port, SLR_DISP_422, (vfehdmitiminginfo->colorspace==COLOR_YUV422) ? 1 : 0);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_COLOR_DEPTH, vfehdmitiminginfo->colordepth);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_COLOR_IMETRY, vfehdmitiminginfo->colorimetry);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IVSYNCPULSECOUNT, vfehdmitiminginfo->IVSyncPulseCount);
	IDDMA_SetVFEInfoByPort(port, SLR_INPUT_V_SYNC_HIGH_PERIOD, vfehdmitiminginfo->polarity);
    if(vfehdmitiminginfo->is_v_drop == 1)
    {
        IDDMA_SetVFEInfoByPort(port, SLR_INPUT_IPV_ACT_LEN_PRE, vfehdmitiminginfo->v_act_len / 2);
        IDDMA_SetVFEInfoByPort(port, SLR_INPUT_V_COUNT, vfehdmitiminginfo->v_total / 2);
        IDDMA_SetVFEInfoByPort(port, SLR_INPUT_V_LEN, vfehdmitiminginfo->v_total / 2);
        IDDMA_SetVFEInfoByPort(port, SLR_INPUT_VSYNC, vfehdmitiminginfo->v_total / 2);
    }

	IDDMA_SetVFEStatus(port, SLR_DISP_INTERLACE,  vfehdmitiminginfo->is_interlace);
}

#if IS_ENABLED(CONFIG_RTK_DPRX)
void IDDMA_SaveDPInfoByPort(unsigned char port, DPRX_TIMING_INFO_T *dp_timing_info)
{
	MEASURE_TIMING_T dummy_for_dp = {};

	dummy_for_dp.v_act_len = dp_timing_info->vact;
	dummy_for_dp.v_act_sta = dp_timing_info->vstart;
	dummy_for_dp.h_act_len = dp_timing_info->hact;
	dummy_for_dp.h_act_sta = dp_timing_info->hstart;

	dummy_for_dp.h_total = dp_timing_info->hsync;
	dummy_for_dp.v_total = dp_timing_info->vsync;

	dummy_for_dp.mode_id = 5;
	dummy_for_dp.modetable_index = 0;

	dummy_for_dp.polarity = dp_timing_info->hs_polarity|dp_timing_info->vs_polarity<<1;
	dummy_for_dp.h_freq = dp_timing_info->hfreq_hz;
	dummy_for_dp.v_freq = dp_timing_info->vfreq_hz_x100 * 10;
	dummy_for_dp.h_total = dp_timing_info->htotal;
	dummy_for_dp.v_total = dp_timing_info->vtotal;
	dummy_for_dp.colordepth = dp_timing_info->color_depth - 1;
	dummy_for_dp.colorimetry = 0;
	dummy_for_dp.IVSyncPulseCount = 0;

	switch (dp_timing_info->color_space) {
	case DP_COLOR_SPACE_RGB:
		dummy_for_dp.colorspace = (unsigned int)VFE_HDMI_COLOR_RGB;
	break;
	case DP_COLOR_SPACE_YUV444:
		dummy_for_dp.colorspace = (unsigned int)VFE_HDMI_COLOR_YUV444;
	break;
	case DP_COLOR_SPACE_YUV422:
		dummy_for_dp.colorspace = (unsigned int)VFE_HDMI_COLOR_YUV422;
	break;
	case DP_COLOR_SPACE_YUV420:
		dummy_for_dp.colorspace = (unsigned int)VFE_HDMI_COLOR_YUV420;
	break;
	default:
		dummy_for_dp.colorspace = (unsigned int)VFE_HDMI_COLOR_UNKNOW;
		rtd_pr_vsc_err("[%s] %d unknown DP color space: %d\n", __FUNCTION__, __LINE__, dp_timing_info->color_space);
	break;

	}

	dummy_for_dp.is_interlace = dp_timing_info->is_interlace;
	dummy_for_dp.pixel_mode = dp_timing_info->pixel_mode;

	IDDMA_SaveHDMIInfoByPort(port, &dummy_for_dp);
}
#endif

unsigned char IDDMA_Find_IdleDev(unsigned int nr)
{
	unsigned char i = 0;

	for(i = I6DDMA_DEV; (i >= I3DDMA_DEV && i <= I6DDMA_DEV); --i)
	{
		if(iddma_attr[i].occupy == IDDMA_PORT_IDLE)
		{
			iddma_attr[i].occupy = IDDMA_PORT_OCCUPY;
			rtd_pr_i3ddma_info("[IDDMA][%s] i%ddma is idle nr=%d!!!", __FUNCTION__, (i+3), nr);
			return i;
		}
	}

	return MAX_IDDMA_DEV;
}

void IDDMA_Set_HDMIPort(unsigned char dev, unsigned int port)
{
	iddma_attr[dev].hdmi_port = port;
}

unsigned char IDDMA_Get_Port_BySourceAndIdx(unsigned char source_type, unsigned char idx)
{
	unsigned char i = 0;

	for(i = 0; i < MAX_IDDMA_DEV; i++)
	{
		if((iddma_attr[i].source_type == source_type) && (iddma_attr[i].hdmi_port == idx))
		{
			break;
		}
	}

	if(i >= MAX_IDDMA_DEV)
		rtd_pr_i3ddma_err("[IDDMA][%s] get iddma dev fail!!!", __FUNCTION__);

	return i;
}

I3DDMA_3DDMA_CTRL_T *Get_Val_i3ddmaCtrl_addr(void)
{
	return &i3ddmaCtrl[I3DDMA_DEV];
}

void Set_Val_i3ddmaCtrl_addr_init_value(void)
{
    memset(&i3ddmaCtrl[I3DDMA_DEV], 0, sizeof(I3DDMA_3DDMA_CTRL_T));
}

void Set_Val_i3ddmaCtrl_addr_cap_buffer_size(unsigned char ucIndex, unsigned int size)
{
    if(ucIndex < CAP_BUF_NUM_MAX)
    {
        i3ddmaCtrl[I3DDMA_DEV].cap_buffer[ucIndex].getsize = size;
    }
}

void Set_Val_i3ddmaCtrl_addr_cap_buffer_addr(unsigned char ucIndex, unsigned int addr)
{
    if(ucIndex < CAP_BUF_NUM_MAX)
    {
        i3ddmaCtrl[I3DDMA_DEV].cap_buffer[ucIndex].phyaddr= addr;
    }
}

char I3DDMA_ForceVODMA2D(UINT8 enable, UINT8 LR) {
#ifndef BUILD_QUICK_SHOW

	VIDEO_RPC_VOUT_FORCE_HDMI_2D *config;
#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)
	unsigned long ret;
#endif
	unsigned long  vir_addr, vir_addr_noncache;
	unsigned int  phy_addr;

	if(get_nn_force_i3ddma_enable())
		return FALSE;

	vir_addr = (unsigned long)dvr_malloc_uncached_specific(sizeof(VIDEO_RPC_VOUT_FORCE_HDMI_2D), GFP_DCU1_LIMIT, (void*)&vir_addr_noncache);
	//phy_addr = (unsigned int)virt_to_phys((void*)vir_addr);
	phy_addr = (unsigned int)dvr_to_phys((void*)vir_addr);

	config = (VIDEO_RPC_VOUT_FORCE_HDMI_2D *)vir_addr_noncache;
	config->force2d = enable;
	config->l_flag = LR;
	config->videoPlane = VO_VIDEO_PLANE_V1;
	config->videoPlane = htonl(config->videoPlane);
#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)
	if (send_rpc_command(RPC_VIDEO, VIDEO_RPC_VOUT_ToAgent_Force_HDMI_2D, phy_addr, 0, &ret))
		rtd_pr_i3ddma_info("RPC fail!!\n");
#endif
	dvr_free((void *)vir_addr);

#endif

	return TRUE;
}

void I3DDMA_SetupLRSeparator(unsigned char dev, I3DDMA_TIMING_T *tx_timing, I3DDMA_TIMING_T *gen_timing) {

	h3ddma_i3ddma_enable_RBUS 	i3ddma_i3ddma_enable_reg;
	h3ddma_lr_separate_ctrl1_RBUS   i3ddma_lr_separate_ctrl1_reg;
	h3ddma_lr_separate_ctrl2_RBUS   i3ddma_lr_separate_ctrl2_reg;
	h3ddma_i3ddma_ctrl_0_RBUS h3ddma_i3ddma_ctrl_0_reg;
	h3ddma_cap1_cti_dma_wr_ctrl_RBUS i3ddma_byte_channel_swap_reg;

	h3ddma_cap2_cti_dma_wr_ctrl_RBUS i3ddma_byte_channe2_swap_reg;
	rtd_pr_i3ddma_info("!!!!!!!!!!!! %s\n", __FUNCTION__);


#if 0 // [To Be Confirmed] Input HS/VS Signal Polarity Invert setup move to Main VGIP
	//Fix me : benwang 20121128
	//rtd_maskl(GET_VGIP_CHNx_CTRL_VADDR(), ~(VGIP_CHN1_CTRL_ch1_vs_inv_mask | VGIP_CHN1_CTRL_ch1_hs_inv_mask ), 0); // Let HS negtive polarity to positive polarity
	rtd_maskl(VGIP_CHN1_CTRL_reg, ~(VGIP_CHN1_CTRL_ch1_vs_inv_mask | VGIP_CHN1_CTRL_ch1_hs_inv_mask ), 0); // Let HS negtive polarity to positive polarity

	switch(tx_timing->polarity) {

		case _SYNC_HP_VP:
			rtd_maskl(I3DDMA_LR_SEPARATE_CTRL1_VADDR, ~(I3DDMA_LR_SEPARATE_CTRL1_vs_inv_mask | I3DDMA_LR_SEPARATE_CTRL1_hs_inv_mask), 0);
			break;
		case _SYNC_HN_VP:
			rtd_maskl(I3DDMA_LR_SEPARATE_CTRL1_VADDR, ~(I3DDMA_LR_SEPARATE_CTRL1_vs_inv_mask | I3DDMA_LR_SEPARATE_CTRL1_hs_inv_mask),
								I3DDMA_LR_SEPARATE_CTRL1_vs_inv(0) | I3DDMA_LR_SEPARATE_CTRL1_hs_inv(1));
			break;
		case _SYNC_HP_VN:
			rtd_maskl(I3DDMA_LR_SEPARATE_CTRL1_VADDR, ~(I3DDMA_LR_SEPARATE_CTRL1_vs_inv_mask | I3DDMA_LR_SEPARATE_CTRL1_hs_inv_mask),
				I3DDMA_LR_SEPARATE_CTRL1_vs_inv(1) | I3DDMA_LR_SEPARATE_CTRL1_hs_inv(0));

			break;
		case _SYNC_HN_VN:
			rtd_maskl(I3DDMA_LR_SEPARATE_CTRL1_VADDR, ~(I3DDMA_LR_SEPARATE_CTRL1_vs_inv_mask | I3DDMA_LR_SEPARATE_CTRL1_hs_inv_mask),
				I3DDMA_LR_SEPARATE_CTRL1_vs_inv(1) | I3DDMA_LR_SEPARATE_CTRL1_hs_inv(1));
			break;
	}
#endif

	// Setup LR Separate for :Frame packing and TopBottom, Line alternative
	// [HDMI-DMA] HDMI DMA capture control for 4k2k 2D timing
	if(dev == I3DDMA_DEV)
	{
		i3ddma_lr_separate_ctrl1_reg.regValue = IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg);
		i3ddma_lr_separate_ctrl1_reg.progressive = gen_timing->progressive;
		IoReg_Write32(H3DDMA_LR_Separate_CTRL1_reg, i3ddma_lr_separate_ctrl1_reg.regValue);

		i3ddma_lr_separate_ctrl2_reg.regValue = IoReg_Read32(H3DDMA_LR_Separate_CTRL2_reg);
		if ((gen_timing->i3ddma_3dformat == I3DDMA_SIDE_BY_SIDE_HALF) || (gen_timing->i3ddma_3dformat == I3DDMA_SENSIO)|| (gen_timing->i3ddma_3dformat == I3DDMA_REALID)) {
				i3ddma_lr_separate_ctrl2_reg.vact = gen_timing->v_act_len;
				i3ddma_lr_separate_ctrl2_reg.hact = gen_timing->h_act_len*2;
		} else if (gen_timing->i3ddma_3dformat == I3DDMA_TOP_AND_BOTTOM) {
				i3ddma_lr_separate_ctrl2_reg.vact = gen_timing->v_act_len*2;
				i3ddma_lr_separate_ctrl2_reg.hact = gen_timing->h_act_len;
		} else if(gen_timing->i3ddma_3dformat == I3DDMA_VERTICAL_STRIPE) {
				i3ddma_lr_separate_ctrl2_reg.vact = gen_timing->v_act_len;
				i3ddma_lr_separate_ctrl2_reg.hact = gen_timing->h_act_len*2;
		}else if(gen_timing->i3ddma_3dformat == I3DDMA_CHECKER_BOARD) {
				i3ddma_lr_separate_ctrl2_reg.vact = gen_timing->v_act_len;
				i3ddma_lr_separate_ctrl2_reg.hact = gen_timing->h_act_len*2;
		}
		else {
				i3ddma_lr_separate_ctrl2_reg.hact = gen_timing->h_act_len;
				i3ddma_lr_separate_ctrl2_reg.vact = gen_timing->v_act_len;
		}
		IoReg_Write32(H3DDMA_LR_Separate_CTRL2_reg, i3ddma_lr_separate_ctrl2_reg.regValue);

		h3ddma_i3ddma_ctrl_0_reg.regValue= IoReg_Read32(H3DDMA_I3DDMA_ctrl_0_reg);
#ifdef CONFIG_RTK_KDRV_DV
		if (check_hdmi_dolby_vision_rgb_and_full_hd())
			h3ddma_i3ddma_ctrl_0_reg.cap0_channel_swap = 3;
		else
#endif
			h3ddma_i3ddma_ctrl_0_reg.cap0_channel_swap = 0;

		IoReg_Write32(H3DDMA_I3DDMA_ctrl_0_reg, h3ddma_i3ddma_ctrl_0_reg.regValue);
	}

	i3ddma_byte_channel_swap_reg.regValue= IoReg_Read32(iddma_cap1_dma_swap_reg_addr[dev]);
	i3ddma_byte_channel_swap_reg.cap1_dma_1byte_swap=0;
	i3ddma_byte_channel_swap_reg.cap1_dma_2byte_swap=0;
	i3ddma_byte_channel_swap_reg.cap1_dma_4byte_swap=0;
	i3ddma_byte_channel_swap_reg.cap1_dma_8byte_swap=0;
	IoReg_Write32(iddma_cap1_dma_swap_reg_addr[dev], i3ddma_byte_channel_swap_reg.regValue);

	i3ddma_byte_channe2_swap_reg.regValue= IoReg_Read32(iddma_cap2_dma_swap_reg_addr[dev]);
	i3ddma_byte_channe2_swap_reg.cap2_dma_1byte_swap=0;
	i3ddma_byte_channe2_swap_reg.cap2_dma_2byte_swap=0;
	i3ddma_byte_channe2_swap_reg.cap2_dma_4byte_swap=0;
	i3ddma_byte_channe2_swap_reg.cap2_dma_8byte_swap=0;
	IoReg_Write32(iddma_cap2_dma_swap_reg_addr[dev], i3ddma_byte_channe2_swap_reg.regValue);

	i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(iddma_dma_enable_reg_addr[dev]);

	//if dma vgip fake_porch_en=1 && fake_porch_sel=1(gen vsync by den failing), enable vodma gen vsync by first den rising
	if(((IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_VSC_SOURCE_TYPE) == VSC_INPUTSRC_HDMI) || (IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_VSC_SOURCE_TYPE) == VSC_INPUTSRC_DP))
		&& (Get_Val_DP_Vsync_Regen_Pos(dev) == DP_REGEN_VSYNC_AT_DMA_VGIP))
		i3ddma_i3ddma_enable_reg.i3tovo_vs_sel = 1;
	else
		i3ddma_i3ddma_enable_reg.i3tovo_vs_sel = 0;

	IoReg_Write32(iddma_dma_enable_reg_addr[dev], i3ddma_i3ddma_enable_reg.regValue);
}

void I3DDMA_reset_i3tovo_vs_sel(unsigned char dev)
{
	h3ddma_i3ddma_enable_RBUS i3ddma_i3ddma_enable_reg;

	i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(iddma_dma_enable_reg_addr[dev]);
	i3ddma_i3ddma_enable_reg.i3tovo_vs_sel = 0;
	IoReg_Write32(iddma_dma_enable_reg_addr[dev], i3ddma_i3ddma_enable_reg.regValue);
}

I3DDMA_ERR_T I3DDMA_MeasureActiveSpace(I3DDMA_TIMING_T *tx_timing, I3DDMA_TIMING_T *gen_timing) {

	int i = 0;
	int fields;

	//rtd_pr_i3ddma_debug("tx_timing->h_act_len = %d\n", tx_timing->h_act_len);
	//rtd_pr_i3ddma_debug("tx_timing->v_act_len = %d\n", tx_timing->v_act_len);


	while(i3ddma_active_space_table[i].name) {
			//rtd_pr_i3ddma_debug(" hdmi_active_space_table[%d].h_act_len = %d\n", i, i3ddma_active_space_table[i].h_act_len);
			//rtd_pr_i3ddma_debug(" hdmi_active_space_table[%d].v_act_len = %d\n", i, i3ddma_active_space_table[i].v_act_len);

		if (/*tx_timing->h_act_len == hdmi_active_space_table[i].h_act_len &&*/ tx_timing->v_act_len == i3ddma_active_space_table[i].v_act_len) {

			gen_timing->v_act_len = i3ddma_active_space_table[i].lr_v_act_len;
			tx_timing->v_active_space1 = i3ddma_active_space_table[i].v_active_space1;
			tx_timing->v_active_space2 = i3ddma_active_space_table[i].v_active_space2;
			gen_timing->v_active_space1 = i3ddma_active_space_table[i].v_active_space1;
			gen_timing->v_active_space2 = i3ddma_active_space_table[i].v_active_space2;
			gen_timing->progressive = i3ddma_active_space_table[i].progressive;
			fields = gen_timing->progressive ? 2 : 4;
		//	gen_timing->progressive = tx_timing->progressive;
			gen_timing->color = tx_timing->color;
			gen_timing->depth = tx_timing->depth;
#ifndef BUILD_QUICK_SHOW
                        gen_timing->v_freq_1000 = ( gen_timing->progressive ? ( drvif_scaler3d_decide_3d_SG_data_FRC() ? ( tx_timing->v_freq_1000 * 2 ) : 120000 ) : ( tx_timing->v_freq_1000 * fields ) );
#else
                        gen_timing->v_freq_1000 =  ( gen_timing->progressive ? ( tx_timing->v_freq_1000 * 2 )  : ( tx_timing->v_freq_1000 * fields ) );
#endif
			gen_timing->v_total = tx_timing->v_total / fields;
			gen_timing->h_freq = tx_timing->h_freq;
			gen_timing->h_act_len = tx_timing->h_act_len;
			gen_timing->h_total = tx_timing->h_total;
			gen_timing->h_act_sta = tx_timing->h_act_sta;
			gen_timing->v_act_sta = tx_timing->v_act_sta;	 // no used
			rtd_pr_i3ddma_debug("%s \n", i3ddma_active_space_table[i].name);
			return I3DDMA_ERR_NO;
		}
		i++;
	}

	gen_timing->progressive = 1;
	fields = gen_timing->progressive ? 2 : 4;
//	gen_timing->progressive = tx_timing->progressive;
	gen_timing->color = tx_timing->color;
	gen_timing->depth = tx_timing->depth;
	gen_timing->v_freq_1000 = ( gen_timing->progressive ? 120000 : ( tx_timing->v_freq_1000 * fields ) );
	gen_timing->v_total = tx_timing->v_total / fields;
	gen_timing->h_freq = tx_timing->h_freq;
	gen_timing->h_act_len = tx_timing->h_act_len;
	gen_timing->h_total = tx_timing->h_total;
	gen_timing->h_act_sta = tx_timing->h_act_sta;
	gen_timing->v_act_sta = tx_timing->v_act_sta;	 // no used
	gen_timing->v_act_len = (tx_timing->v_act_len - tx_timing->v_act_sta) / 2;
	gen_timing->v_active_space1 = tx_timing->v_act_sta;
	gen_timing->v_active_space2 = 0;
	tx_timing->v_active_space1 = tx_timing->v_act_sta;
	tx_timing->v_active_space2 = 0;

	rtd_pr_i3ddma_debug("Unknow frame packing fomat and force to 2D\n");

	return I3DDMA_EER_MEASURE_ACTIVESPACE_FAIL;

}


void I3DDMA_Setup3Dtg(I3DDMA_TIMING_T *timing,  char frame_rate_x2) {
#if 0
	int fields, i;
	int genvs_start_pixels, genvs_end_pixels;
	int genvs_width, genvs_period;

	rtd_pr_i3ddma_info("!!!!!!!!!!!! %s\n", __FUNCTION__);

	frame_rate_x2 = 1;

	if (frame_rate_x2){
		fields = 8;
		genvs_width = 3;
		genvs_period = timing->v_total / 2;
	}else if(I3DDMA_Get_FP1080i_status() && i3ddmaCtrl.force_3dto2d_enable){
		fields = 2;
		genvs_width = 3;
		rtd_pr_i3ddma_info("[ I3DDMA] FP1080i 3Dto2D mode\n");
	} else {
//		fields = 4;
		fields = 2;
		genvs_width = 3;
		genvs_period = timing->v_total / 2;
	}

	rtd_pr_i3ddma_info("[HDMI-3DTG] FR*2=%d, field/period[%d/%d]\n", frame_rate_x2, fields, genvs_period);

	if(I3DDMA_Get_FP1080i_3Dcvt2D_fix_vStart_status()){
		// Ignore 3DTG v-sync insertion in HDMI 1080i FP video 3D to 2D mode (set vStart/vEnd=0xfff)
		genvs_start_pixels = (timing->h_total << 12)-timing->h_total;
		genvs_end_pixels = genvs_start_pixels;
	}
	// [HDMI-DMA] HDMI DMA capture timing for 4k2k 2D video
	else if(timing->i3ddma_3dformat == I3DDMA_2D_ONLY){
		// Ignore 3DTG v-sync insertion in 4k2k 2D mode (set vStart/vEnd=0xfff)
		genvs_start_pixels = 0; //(timing->h_total << 12)-timing->h_total;
		genvs_end_pixels = 0; //genvs_start_pixels;
		genvs_period = 0;
	}
	else{
		rtd_pr_i3ddma_info("[DBG] FP1080i/3Dto2D=%d/%d\n", I3DDMA_Get_FP1080i_status(), hdmi.force_3dto2d_enable);
		genvs_start_pixels = (timing->v_total * timing->h_total  + (fields/2))/fields;
		genvs_end_pixels = genvs_start_pixels + 3 * timing->h_total;
	}

	rtd_pr_i3ddma_info("[ I3DDMA-3DTG] start pixel/end pixel [%d /%d], vtotal/htotal [%d/%d]\n", genvs_start_pixels, genvs_end_pixels, timing->v_total, timing->h_total);


	// Timing Gen setting
	rtd_maskl(I3DDMA_TG_V1_END_reg, ~(I3DDMA_TG_V1_END_tg_vend1_mask | I3DDMA_TG_V1_END_tg_hend1_mask),
					I3DDMA_TG_V1_END_tg_vend1(3) | I3DDMA_TG_V1_END_tg_hend1(0));

	// FP 1080i 3Dto2D: field == 2
	if((fields == 2) && I3DDMA_Get_FP1080i_status() && i3ddmaCtrl.force_3dto2d_enable){
		rtd_maskl(I3DDMA_TG_V2_START_reg, ~(I3DDMA_TG_V2_START_tg_vstart2_mask | I3DDMA_TG_V2_START_tg_hstart2_mask) ,
						(I3DDMA_TG_V2_START_tg_vstart2(genvs_period) | I3DDMA_TG_V2_START_tg_hstart2(genvs_start_pixels%timing->h_total)));
		rtd_maskl(I3DDMA_TG_V2_END_reg, ~(I3DDMA_TG_V2_END_tg_vend2_mask | I3DDMA_TG_V2_END_tg_hend2_mask) ,
					(I3DDMA_TG_V2_END_tg_vend2(genvs_end_pixels/timing->h_total) | I3DDMA_TG_V2_END_tg_hend2(genvs_end_pixels%timing->h_total)));
	}
	else if(fields == 2){
		rtd_maskl(I3DDMA_TG_V2_START_reg, ~(I3DDMA_TG_V2_START_tg_vstart2_mask | I3DDMA_TG_V2_START_tg_hstart2_mask) ,
						(I3DDMA_TG_V2_START_tg_vstart2(genvs_period) | I3DDMA_TG_V2_START_tg_hstart2(genvs_start_pixels%timing->h_total)));
		rtd_maskl(I3DDMA_TG_V2_END_reg, ~(I3DDMA_TG_V2_END_tg_vend2_mask | I3DDMA_TG_V2_END_tg_hend2_mask) ,
					(I3DDMA_TG_V2_END_tg_vend2(genvs_end_pixels/timing->h_total) | I3DDMA_TG_V2_END_tg_hend2(genvs_end_pixels%timing->h_total)));
	}
	else{
		for (i=0; i<(fields/2-1); i++) {
	//		rtd_maskl(I3DDMA_TG_V2_START_reg + 8 * i, ~(I3DDMA_TG_V2_START_tg_vstart2_mask | I3DDMA_TG_V2_START_tg_hstart2_mask) ,
	//						(I3DDMA_TG_V2_START_tg_vstart2(genvs_start / timing->h_total) | I3DDMA_TG_V2_START_tg_hstart2(genvs_start % timing->h_total)));
	//		rtd_maskl(I3DDMA_TG_V2_END_reg+ 8 * i, ~(I3DDMA_TG_V2_END_tg_vend2_mask | I3DDMA_TG_V2_END_tg_hend2_mask) ,
	//					(I3DDMA_TG_V2_END_tg_vend2(genvs_end / timing->h_total) | I3DDMA_TG_V2_END_tg_hend2(genvs_end % timing->h_total)));
	//#ifdef ENABLE_HDMI_3DDMA_TEST
	#if 0
			rtd_maskl(I3DDMA_TG_V2_START_reg + 8 * i, ~(I3DDMA_TG_V2_START_tg_vstart2_mask | I3DDMA_TG_V2_START_tg_hstart2_mask) ,
							(I3DDMA_TG_V2_START_tg_vstart2(genvs_period) | I3DDMA_TG_V2_START_tg_hstart2(genvs_start_pixels%timing->h_total)));
			rtd_maskl(I3DDMA_TG_V2_END_reg+ 8 * i, ~(I3DDMA_TG_V2_END_tg_vend2_mask | I3DDMA_TG_V2_END_tg_hend2_mask) ,
						(I3DDMA_TG_V2_END_tg_vend2(genvs_period+genvs_width) | I3DDMA_TG_V2_END_tg_hend2(genvs_end_pixels%timing->h_total)));
	#else
			rtd_maskl(I3DDMA_TG_V2_START_reg + 8 * i, ~(I3DDMA_TG_V2_START_tg_vstart2_mask | I3DDMA_TG_V2_START_tg_hstart2_mask) ,
							(I3DDMA_TG_V2_START_tg_vstart2(genvs_start_pixels/timing->h_total) | I3DDMA_TG_V2_START_tg_hstart2(genvs_start_pixels%timing->h_total)));
			rtd_maskl(I3DDMA_TG_V2_END_reg+ 8 * i, ~(I3DDMA_TG_V2_END_tg_vend2_mask | I3DDMA_TG_V2_END_tg_hend2_mask) ,
						(I3DDMA_TG_V2_END_tg_vend2(genvs_end_pixels/timing->h_total) | I3DDMA_TG_V2_END_tg_hend2(genvs_end_pixels%timing->h_total)));
	#endif
		}
	}

	if((timing->i3ddma_3dformat != I3DDMA_2D_ONLY)){
		rtd_maskl(I3DDMA_LR_SEPARATE_CTRL1_VADDR, ~(I3DDMA_LR_SEPARATE_CTRL1_hdmi_3d_sel_mask),
		I3DDMA_LR_SEPARATE_CTRL1_hdmi_3d_sel(1));
	}

	// enable double frame rate
	//if(I3DDMA_Get_FP1080i_status() == TRUE){
		rtd_maskl(I3DDMA_TG_V1_END_reg, ~(I3DDMA_TG_V1_END_pulldown_en_mask),
						I3DDMA_TG_V1_END_pulldown_en(frame_rate_x2!=0));
	//}

	// timing gen enable
	rtd_maskl(I3DDMA_TG_V1_END_reg, ~(I3DDMA_TG_V1_END_tg_en_mask),
					I3DDMA_TG_V1_END_tg_en(1));

#else

	int fields;
	//int i;
	int genvs_start_pixels, genvs_end_pixels;
	//int genvs_width, genvs_period;
	//int start_pixels_tmp;

	h3ddma_tg_v1_end_RBUS i3ddma_tg_v1_end_reg;
	//h3ddma_tg_v2_start_RBUS i3ddma_tg_v2_start_reg;
	//h3ddma_tg_v2_end_RBUS i3ddma_tg_v2_end_reg;
//	h3ddma_lr_separate_ctrl1_RBUS   i3ddma_lr_separate_ctrl1_reg;
	h3ddma_i3ddma_enable_RBUS 	i3ddma_i3ddma_enable_reg;
	vodma_vodma_i2rnd_fifo_th_RBUS vodma_vodma_i2rnd_fifo_th_Reg;
	vodma_vodma_i2rnd_dma_info_RBUS vodma_vodma_i2rnd_dma_info_Reg;

	rtd_pr_i3ddma_debug("!!!!!!!!!!!! 2 %s\n", __FUNCTION__);

	if(frame_rate_x2){
		fields = 8;
		//genvs_width = 3;
		//genvs_period = timing->v_total / 2;
	}else if((timing->progressive == 0) && (timing->i3ddma_3dformat != I3DDMA_2D_ONLY)
			&& (timing->i3ddma_3dformat != I3DDMA_FRAME_PACKING)){
		fields = 4;
		//genvs_width = 3;
	} else{
		fields = 2;
		//genvs_width = 3;
		//genvs_period = timing->v_total / 2;
	}

	if(timing->i3ddma_3dformat == I3DDMA_2D_ONLY){
		genvs_start_pixels = 0; //(timing->h_total << 12)-timing->h_total;
		genvs_end_pixels = 0; //genvs_start_pixels;
		//genvs_period = 0;
	}
	else{
		genvs_start_pixels = (timing->v_total * timing->h_total); //(timing->v_total * timing->h_total  + (fields/2))/fields;
		genvs_end_pixels = genvs_start_pixels + 3 * timing->h_total;
	}

	rtd_pr_i3ddma_debug("[ I3DDMA-3DTG] start pixel/end pixel [%d /%d], vtotal/htotal [%d/%d]\n", genvs_start_pixels, genvs_end_pixels, timing->v_total, timing->h_total);
	rtd_pr_i3ddma_debug("[ I3DDMA-3DTG] progressive / fields / 3d format  [%d/%d/%d]\n", timing->progressive, fields, timing->i3ddma_3dformat);


	// Timing Gen setting
	i3ddma_tg_v1_end_reg.regValue = IoReg_Read32(H3DDMA_TG_v1_end_reg);
	i3ddma_tg_v1_end_reg.tg_vend1 = 3;
	i3ddma_tg_v1_end_reg.tg_hend1 = 0;
	IoReg_Write32(H3DDMA_TG_v1_end_reg, i3ddma_tg_v1_end_reg.regValue);

#if 0 //@jiawei_tang for k6 compile error

	if(fields == 2){
		i3ddma_tg_v2_start_reg.regValue = IoReg_Read32(H3DDMA_TG_v2_start_reg);
		i3ddma_tg_v2_start_reg.tg_vstart2 = genvs_start_pixels/timing->h_total;
		i3ddma_tg_v2_start_reg.tg_hstart2 = genvs_start_pixels%timing->h_total;
		IoReg_Write32(H3DDMA_TG_v2_start_reg, i3ddma_tg_v2_start_reg.regValue);

		i3ddma_tg_v2_end_reg.regValue = IoReg_Read32(H3DDMA_TG_v2_end_reg);
		i3ddma_tg_v2_end_reg.tg_vend2 = genvs_end_pixels/timing->h_total;
		i3ddma_tg_v2_end_reg.tg_hend2 = genvs_end_pixels%timing->h_total;
		IoReg_Write32(H3DDMA_TG_v2_end_reg, i3ddma_tg_v2_end_reg.regValue);
	}
	else if(fields == 4){	//interlace mode

		start_pixels_tmp = genvs_start_pixels;

		for (i=0; i<(fields/2+1); i++) {

			genvs_start_pixels = (i+1) * start_pixels_tmp;
			genvs_end_pixels = genvs_start_pixels + 3 * timing->h_total;


			i3ddma_tg_v2_start_reg.regValue = IoReg_Read32(H3DDMA_TG_v2_start_reg + 8*i);
			i3ddma_tg_v2_start_reg.tg_vstart2 = genvs_start_pixels/timing->h_total;
			i3ddma_tg_v2_start_reg.tg_hstart2 = genvs_start_pixels%timing->h_total;
			IoReg_Write32(H3DDMA_TG_v2_start_reg + 8*i, i3ddma_tg_v2_start_reg.regValue);

			i3ddma_tg_v2_end_reg.regValue = IoReg_Read32(H3DDMA_TG_v2_end_reg + 8*i);
			i3ddma_tg_v2_end_reg.tg_vend2 = genvs_end_pixels/timing->h_total;
			i3ddma_tg_v2_end_reg.tg_hend2 = genvs_end_pixels%timing->h_total;
			IoReg_Write32(H3DDMA_TG_v2_end_reg + 8*i, i3ddma_tg_v2_end_reg.regValue);
		}
	}

	if((timing->i3ddma_3dformat != I3DDMA_2D_ONLY)){
		i3ddma_lr_separate_ctrl1_reg.regValue = IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg);
		i3ddma_lr_separate_ctrl1_reg.hdmi_3d_sel = 1;
		IoReg_Write32(H3DDMA_LR_Separate_CTRL1_reg, i3ddma_lr_separate_ctrl1_reg.regValue);

	}
#endif

	// timing gen enable
//	rtd_maskl(I3DDMA_TG_V1_END_VADDR, ~(I3DDMA_TG_V1_END_tg_en_mask),	I3DDMA_TG_V1_END_tg_en(1));


	vodma_vodma_i2rnd_fifo_th_Reg.regValue = IoReg_Read32(VODMA_vodma_i2rnd_fifo_th_reg);
	vodma_vodma_i2rnd_dma_info_Reg.regValue = 0;

	vodma_vodma_i2rnd_fifo_th_Reg.dma_info_th_dispm = 2;
	vodma_vodma_i2rnd_fifo_th_Reg.dma_info_th_i3ddma = 2;

	vodma_vodma_i2rnd_dma_info_Reg.dma_info_rptr_fw_set_dispm = 1;
	vodma_vodma_i2rnd_dma_info_Reg.dma_info_rptr_fw_set_i3ddma = 1;
	vodma_vodma_i2rnd_dma_info_Reg.dma_info_wptr_fw_set_dispm = 1;
	vodma_vodma_i2rnd_dma_info_Reg.dma_info_wptr_fw_set_i3ddma = 1;

	IoReg_Write32(VODMA_vodma_i2rnd_fifo_th_reg, vodma_vodma_i2rnd_fifo_th_Reg.regValue);
	IoReg_Write32(VODMA_vodma_i2rnd_dma_info_reg, vodma_vodma_i2rnd_dma_info_Reg.regValue);


	i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
	i3ddma_i3ddma_enable_reg.tg_en = 1;
	i3ddma_i3ddma_enable_reg.block_sel_to_flag_fifo_option = 0; //reset the default for rerun scaler
	IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_i3ddma_enable_reg.regValue);


#endif
}

//#endif  //#endif HDMI_HAVE_HDMI_3D_TG

//#ifdef HDMI_HAVE_HDMI_DMA
unsigned int DEPTH2BITS[4] = {
	8,
	10,
	10,
	10,
};

unsigned int components[4] = {
	12,
	8,
	12,
	6,
};

char I3DDMA_SetupCapture0Init(I3DDMA_CAPTURE_BUFFER_T *cap_buffer, I3DDMA_TIMING_T *timing, I3DDMA_3D_OPMODE_T mode) {

	int rem, len, num;
	unsigned int h_act_len;
	int i;

	//h3ddma_cap0_cap_ctl0_RBUS i3ddma_cap_ctl0_reg;
	//h3ddma_cap_ctl1_RBUS i3ddma_cap_ctl1_reg;
//	h3ddma_i3ddma_enable_RBUS i3ddma_i3ddma_enable_reg;
	h3ddma_cap0_cti_dma_wr_ctrl_RBUS h3ddma_cap0_cti_dma_wr_ctrl_reg;

	rtd_pr_i3ddma_debug("!!!!!!!!!!!! %s\n", __FUNCTION__);

	//rtd_maskl(I3DDMA_CAP_CTL0_VADDR, ~(I3DDMA_CAP_CTL0_cap_en_mask),
	//			I3DDMA_CAP_CTL0_cap_en(0));
	h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg);
	h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_enable = 0;
	IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg, h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue);

	for(i=0; i<I3DDMA_CAP_BUF_NUM; i++)
		cap_buffer[i].phyaddr = i3ddmaCtrl[I3DDMA_DEV].cap_buffer[i].phyaddr;

	switch (mode){

		case I3DDMA_3D_OPMODE_HW:
			//size = cap_buffer[0].size / 4;
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg, cap_buffer[0].phyaddr);
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r0_reg, cap_buffer[0].phyaddr);
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg, cap_buffer[1].phyaddr);
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r1_reg, cap_buffer[1].phyaddr);
		break;
		case I3DDMA_3D_OPMODE_HW_2DONLY_L:
			//size = cap_buffer[0].size / 2;
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg, cap_buffer[0].phyaddr);
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg, cap_buffer[1].phyaddr);
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r0_reg, 0);
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r1_reg, 0);

		break;
		case I3DDMA_3D_OPMODE_HW_2DONLY_R:
			//size = cap_buffer[0].size / 2;
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r0_reg, cap_buffer[0].phyaddr);
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r1_reg, cap_buffer[1].phyaddr);
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg, 0);
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg, 0);
		break;

		case I3DDMA_3D_OPMODE_SW:
            IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg, cap_buffer[0].phyaddr);
            IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r0_reg, cap_buffer[1].phyaddr);
            IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg, cap_buffer[2].phyaddr);
            IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r1_reg, cap_buffer[3].phyaddr);

		default:

		break;
	}
	if(drvif_i3ddma_triplebuf_flag()||drvif_i3ddma_triplebuf_by_timing_protect_panel()){
		if(cap_buffer[2].phyaddr){
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_2_reg, cap_buffer[2].phyaddr);
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r2_reg, cap_buffer[2].phyaddr);
		}
		rtd_pr_i3ddma_notice("set capture4 = %x\n", IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_2_reg));
	}
	else {
		IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_2_reg, cap_buffer[0].phyaddr);
		IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r2_reg, cap_buffer[0].phyaddr);
	}

	// 128 bits = 16 bytes
	rtd_pr_i3ddma_debug("DEPTH2BITS[timing->depth] = %d\n", DEPTH2BITS[timing->depth]);
	rtd_pr_i3ddma_debug("components[timing->color] = %d\n", components[timing->color]);
	rtd_pr_i3ddma_debug("timing->h_act_len = %d\n", timing->h_act_len);

	if(dvrif_i3ddma_compression_get_enable() == TRUE){
		h_act_len = timing->h_act_len;
		if((h_act_len % 32) != 0){
			//drop bits
			h_act_len = h_act_len + (32 - (h_act_len % 32));
		}
		h_act_len = h_act_len * dvrif_i3ddma_get_compression_bits() + 256;
		h_act_len = (h_act_len + 127) / 128;
	}else{
		h_act_len  = (DEPTH2BITS[timing->depth] * (timing->h_act_len /4) * components[timing->color] + 127) / 128;
	}
	rtd_pr_i3ddma_debug("h_act_len = %d\n", h_act_len);
	rem = 12;
	len = 32;
	// 96 align  ,unit 128bit=16byte
	len = drvif_memory_get_data_align((unsigned int)len, DMA_SPEEDUP_ALIGN_VALUE/16);
	num = (h_act_len - rem) / len;
	rem = (h_act_len - rem) % len + rem;
	rtd_pr_i3ddma_debug("len = %d\n", len);
	rtd_pr_i3ddma_debug("rem = %d\n", rem);
	rtd_pr_i3ddma_debug("num = %d\n", num);
#if 0
	i3ddma_cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_Cap_CTL0_reg);
	i3ddma_cap_ctl0_reg.cap_write_len = len;
	i3ddma_cap_ctl0_reg.cap_write_rem = rem;
	IoReg_Write32(H3DDMA_Cap_CTL0_reg, i3ddma_cap_ctl0_reg.regValue);

	i3ddma_cap_ctl1_reg.regValue = IoReg_Read32(H3DDMA_Cap_CTL1_reg);
	i3ddma_cap_ctl1_reg.cap_water_lv = len;
	i3ddma_cap_ctl1_reg.cap_write_num = num;
	IoReg_Write32(H3DDMA_Cap_CTL1_reg, i3ddma_cap_ctl1_reg.regValue);
#endif
	drvif_h3ddma_set_sequence_capture0(timing);

//	IoReg_Write32(I3DDMA_CAP_BOUNDARYADDR1_VADDR, cap_buffer[0].phyaddr);
//	IoReg_Write32(I3DDMA_CAP_BOUNDARYADDR2_VADDR, cap_buffer[0].phyaddr + cap_buffer->size);
	IoReg_Write32(H3DDMA_CAP0_Cap_Status_reg, 0x1F);

	return I3DDMA_ERR_NO;
}
#ifndef BUILD_QUICK_SHOW

void drvif_I3DDMA_freeze(unsigned char bFreeze)
{
       h3ddma_cap0_cap_ctl0_RBUS i3ddma_cap_ctl0_reg;
       i3ddma_cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_CAP0_Cap_CTL0_reg);
       if(bFreeze == TRUE)
               i3ddma_cap_ctl0_reg.cap0_freeze_en = 1;
       else
               i3ddma_cap_ctl0_reg.cap0_freeze_en = 0;
       IoReg_Write32(H3DDMA_CAP0_Cap_CTL0_reg, i3ddma_cap_ctl0_reg.regValue);
}
#endif

void I3DDMA_Get3DGenTiming(unsigned char dev, I3DDMA_TIMING_T *tx_timing, I3DDMA_TIMING_T *gen_timing)
{
	if((tx_timing == NULL) || (gen_timing == NULL))
	{
		rtd_pr_i3ddma_emerg("[i3ddma][%s]timing is null,return\n",__FUNCTION__);
		return;
	}

	gen_timing->i3ddma_3dformat = tx_timing->i3ddma_3dformat;
	gen_timing->colorimetry = tx_timing->colorimetry;
	gen_timing->color = i3ddmaCtrl[dev].targetColor; //tx_timing->color;
	gen_timing->depth = i3ddmaCtrl[dev].targetDepth; //tx_timing->depth;
//	gen_timing->quincunx_en = 0;
    gen_timing->src_v_freq_1000 = tx_timing->v_freq_1000;

	switch (gen_timing->i3ddma_3dformat) {
			case I3DDMA_2D_ONLY:
				if (tx_timing->i3ddma_3dformat != I3DDMA_2D_ONLY) {  // 3d convert to 2D , don't do DI
					gen_timing->progressive = 1;
				} else {
					gen_timing->progressive = tx_timing->progressive;
				}
				gen_timing->v_total = tx_timing->v_total;
				gen_timing->v_act_len = tx_timing->v_act_len;
				gen_timing->v_act_sta = tx_timing->v_act_sta;
				gen_timing->h_total = tx_timing->h_total;
				gen_timing->h_act_len = tx_timing->h_act_len;
				gen_timing->h_act_sta = tx_timing->h_act_sta;
				gen_timing->v_freq_1000 = tx_timing->v_freq_1000;
				gen_timing->h_freq = tx_timing->h_freq;
			break;

			case I3DDMA_FRAMESEQUENCE:
				rtd_pr_i3ddma_debug("I3DDMA_FRAMESEQUENCE \n");
				gen_timing->progressive = tx_timing->progressive;
				gen_timing->color = tx_timing->color;
				gen_timing->depth = tx_timing->depth;
				gen_timing->v_total = tx_timing->v_total;
				//gen_timing->v_freq_1000 = tx_timing->v_freq_1000*2;
				gen_timing->v_freq_1000 = V_FREQ_120000;
				gen_timing->h_freq = tx_timing->h_freq;
				gen_timing->h_act_len = tx_timing->h_act_len;
				gen_timing->h_total = tx_timing->h_total;
				gen_timing->h_act_sta = tx_timing->h_act_sta;
				gen_timing->v_act_sta = tx_timing->v_act_sta;
				gen_timing->v_act_len = tx_timing->v_act_len;
				gen_timing->v_active_space1 = tx_timing->v_act_sta;
				gen_timing->v_active_space2 = 0;
				tx_timing->v_active_space1 = tx_timing->v_act_sta;
				tx_timing->v_active_space2 = 0;
			break;

			case I3DDMA_FRAME_PACKING:
				I3DDMA_MeasureActiveSpace(tx_timing, gen_timing);
			break;
			case I3DDMA_LINE_ALTERNATIVE:
				gen_timing->progressive = tx_timing->progressive;
				gen_timing->v_total = tx_timing->v_total/2;
				gen_timing->v_act_len = tx_timing->v_act_len/2;
				gen_timing->v_act_sta = tx_timing->v_act_sta;
				gen_timing->h_total = tx_timing->h_total;
				gen_timing->h_act_len = tx_timing->h_act_len;
				gen_timing->h_act_sta = tx_timing->h_act_sta;
				//gen_timing->v_freq_1000 = tx_timing->v_freq_1000*2;
				gen_timing->v_freq_1000 = V_FREQ_120000;
				gen_timing->h_freq = tx_timing->h_freq;
			break;

			case I3DDMA_SIDE_BY_SIDE_FULL:
				//rtd_pr_i3ddma_debug("I3DDMA_SIDE_BY_SIDE_FULL\n");
				gen_timing->progressive = tx_timing->progressive;
				gen_timing->v_total = tx_timing->v_total/2;
				gen_timing->v_act_len = tx_timing->v_act_len;
				gen_timing->v_act_sta = tx_timing->v_act_sta;
				gen_timing->h_total = tx_timing->h_total/2;
				gen_timing->h_act_len = tx_timing->h_act_len/2;
				gen_timing->h_act_sta = tx_timing->h_act_sta/2;
				//gen_timing->v_freq_1000 = tx_timing->v_freq_1000*2;
				gen_timing->v_freq_1000 = V_FREQ_120000;
				gen_timing->h_freq = tx_timing->h_freq;
			break;
			case I3DDMA_TOP_AND_BOTTOM:
				//rtd_pr_i3ddma_debug("I3DDMA_TOP_AND_BOTTOM\n");
				gen_timing->progressive = tx_timing->progressive;
				gen_timing->v_total = tx_timing->v_total/2;
				gen_timing->v_act_len = tx_timing->v_act_len/2;
				gen_timing->v_act_sta = tx_timing->v_act_sta/2;
				gen_timing->h_total = tx_timing->h_total;
				gen_timing->h_act_len = tx_timing->h_act_len;
				gen_timing->h_act_sta = tx_timing->h_act_sta;
				//gen_timing->v_freq_1000 = tx_timing->v_freq_1000*2;
				gen_timing->v_freq_1000 = V_FREQ_120000;
				gen_timing->h_freq = tx_timing->h_freq;
			break;

			case I3DDMA_REALID:
			case I3DDMA_SENSIO:
			case I3DDMA_SIDE_BY_SIDE_HALF:
				//rtd_pr_i3ddma_debug("%s\n", (gen_timing->i3ddma_3dformat == I3DDMA_SIDE_BY_SIDE_HALF? "I3DDMA_SIDE_BY_SIDE_HALF": "I3DDMA_SENSIO"));
				gen_timing->progressive = tx_timing->progressive;
				gen_timing->v_total = tx_timing->v_total/2;
				gen_timing->v_act_len = tx_timing->v_act_len;
				gen_timing->v_act_sta = tx_timing->v_act_sta;
				gen_timing->h_total = tx_timing->h_total/2;
				gen_timing->h_act_len = tx_timing->h_act_len/2;
				gen_timing->h_act_sta = tx_timing->h_act_sta/2;
				//gen_timing->v_freq_1000 = tx_timing->v_freq_1000*2;
				gen_timing->v_freq_1000 = V_FREQ_120000;
				gen_timing->h_freq = tx_timing->h_freq;

			break;
			case I3DDMA_FIELD_ALTERNATIVE:
				//rtd_pr_i3ddma_debug("I3DDMA_FIELD_ALTERNATIVE\n");
				gen_timing->progressive = tx_timing->progressive;
				gen_timing->v_total = tx_timing->v_total/2;
				gen_timing->v_act_len = tx_timing->v_act_len;
				gen_timing->v_act_sta = tx_timing->v_act_sta;
				gen_timing->h_total = tx_timing->h_total;
				gen_timing->h_act_len = tx_timing->h_act_len;
				gen_timing->h_act_sta = tx_timing->h_act_sta;
				//gen_timing->v_freq_1000 = tx_timing->v_freq_1000*2;
				gen_timing->v_freq_1000 = V_FREQ_120000;
				gen_timing->h_freq = tx_timing->h_freq;
			break;

			case I3DDMA_VERTICAL_STRIPE:
				//rtd_pr_i3ddma_debug("I3DDMA_VERTICAL_STRIPE\n");
				gen_timing->progressive = tx_timing->progressive;
				gen_timing->v_total = tx_timing->v_total/2;
				gen_timing->v_act_len = tx_timing->v_act_len;
				gen_timing->v_act_sta = tx_timing->v_act_sta;
				gen_timing->h_total = tx_timing->h_total/2;
				gen_timing->h_act_len = tx_timing->h_act_len/2;
				gen_timing->h_act_sta = tx_timing->h_act_sta/2;
				//gen_timing->v_freq_1000 = tx_timing->v_freq_1000*2;
				gen_timing->v_freq_1000 = V_FREQ_120000;
				gen_timing->h_freq = tx_timing->h_freq;
			break;

			case I3DDMA_CHECKER_BOARD:
				//rtd_pr_i3ddma_debug("I3DDMA_CHECKER_BOARD\n");
				gen_timing->progressive = tx_timing->progressive;
				gen_timing->v_total = tx_timing->v_total/2;
				gen_timing->v_act_len = tx_timing->v_act_len;
				gen_timing->v_act_sta = tx_timing->v_act_sta;
				gen_timing->h_total = tx_timing->h_total/2;
				gen_timing->h_act_len = tx_timing->h_act_len/2;
				gen_timing->h_act_sta = tx_timing->h_act_sta/2;
				//gen_timing->v_freq_1000 = tx_timing->v_freq_1000*2;
				gen_timing->v_freq_1000 = V_FREQ_120000;
				gen_timing->h_freq = tx_timing->h_freq;
			break;

			default:
			break;
	}
#ifndef BUILD_QUICK_SHOW

	// [IDMA] interlaced video data FRC in M-domain, IDMA VO go auto mode
	if(/*(tx_timing->progressive == 0) ||*/ (drvif_scaler3d_decide_3d_SG_data_FRC() == TRUE) || drvif_scaler3d_decide_3d_PR_enable_IDMA())
	{
		if((gen_timing->i3ddma_3dformat != I3DDMA_FRAME_PACKING) && (get_HDMI_HDR_mode() == HDR_MODE_DISABLE))//dolby and hdr have no 3d till now
			gen_timing->v_freq_1000 = tx_timing->v_freq_1000*2;

		rtd_pr_i3ddma_debug("[IDMA] 3DFmt[%d]@%c%d Hz\n", gen_timing->i3ddma_3dformat, (tx_timing->progressive? 'p': 'i'), gen_timing->v_freq_1000);
	}
#endif
	rtd_pr_i3ddma_debug("[3DDMA] frame_rate_x2 %x, h_freq : %x \n", i3ddmaCtrl[dev].frame_rate_x2, gen_timing->h_freq);

	if(i3ddmaCtrl[dev].frame_rate_x2 == 1){
		gen_timing->v_freq_1000 = gen_timing->v_freq_1000*2;
	}

	if((IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE) && drvif_i3ddma_triplebuf_flag() && !get_scaler_qms_mode_flag())
	{
		// [MAC5P-2291] force VO output frame rate >= 48hz
		if( get_MEMC_bypass_status_refer_platform_model() == TRUE )
		{
			if( ( tx_timing->v_freq_1000 < 31000 ) && get_vsc_data_path_indicator(DRIVER_FREERUN_PATTERN) )
            {
              // 24 -> 60 Hz, 25/30 -> 50/60 Hz
              gen_timing->v_freq_1000 = ( ( tx_timing->v_freq_1000 >= 24100 ) ? ( tx_timing->v_freq_1000 * 2 ) : V_FREQ_60000 );
              rtd_pr_i3ddma_emerg("[IDMA][Force Data FS] Frame rate = %d->%d\n", tx_timing->v_freq_1000, gen_timing->v_freq_1000);
            }
            else
            {
              if( ( tx_timing->v_freq_1000 >= V_FREQ_100000_mOFFSET ) && ( tx_timing->v_freq_1000 < V_FREQ_100000_pOFFSET ) )
                gen_timing->v_freq_1000 = V_FREQ_50000;
              else
                gen_timing->v_freq_1000 = V_FREQ_60000;
            }
		}
		else if( ( tx_timing->v_freq_1000 < 31000 ) && get_vsc_data_path_indicator(DRIVER_FREERUN_PATTERN) )
		{
            //oled panel set 24.005 ~ 24.500 -> 24 Hz, 30.010 ~ 30.500 -> 30 Hz
            if( ( Get_DISPLAY_PANEL_OLED_TYPE() == TRUE ) && ( tx_timing->v_freq_1000 < 31000 ) )
            {
              if( ( tx_timing->v_freq_1000 >= 24005 ) && ( tx_timing->v_freq_1000 < V_FREQ_24500 ) )
                gen_timing->v_freq_1000 = V_FREQ_24000;
              else if( ( tx_timing->v_freq_1000 >= 30010 ) && ( tx_timing->v_freq_1000 < V_FREQ_30000_pOFFSET ) )
                gen_timing->v_freq_1000 = V_FREQ_30000;
              else
                gen_timing->v_freq_1000 = tx_timing->v_freq_1000;
            }
            else
            {
              // 24 -> 60 Hz, 25/30 -> 50/60 Hz
              gen_timing->v_freq_1000 = ( ( tx_timing->v_freq_1000 >= 24100 ) ? ( tx_timing->v_freq_1000 * 2 ) : V_FREQ_60000 );
              pr_debug("[IDMA][Force Data FS] Frame rate = %d->%d\n", tx_timing->v_freq_1000, gen_timing->v_freq_1000);
            }
		}
		else
		{
            // oled panel set 24.005 ~ 24.500 -> 24 Hz, 30.010 ~ 30.500 -> 30 Hz
            if( ( Get_DISPLAY_PANEL_OLED_TYPE() == TRUE ) && ( tx_timing->v_freq_1000 < 31000 ) )
            {
              if( ( tx_timing->v_freq_1000 >= 24005 ) && ( tx_timing->v_freq_1000 < V_FREQ_24500 ) )
                gen_timing->v_freq_1000 = V_FREQ_24000;
              else if( ( tx_timing->v_freq_1000 >= 30010 ) && ( tx_timing->v_freq_1000 < V_FREQ_30000_pOFFSET ) )
                gen_timing->v_freq_1000 = V_FREQ_30000;
              else
                gen_timing->v_freq_1000 = tx_timing->v_freq_1000;
            }
            else if( ( tx_timing->v_freq_1000 >= V_FREQ_100000_mOFFSET ) && ( tx_timing->v_freq_1000 < V_FREQ_100000_pOFFSET ) )
            {
              gen_timing->v_freq_1000 = V_FREQ_50000;
            }
            else if( tx_timing->progressive == 0 )
            {
              gen_timing->v_freq_1000 = tx_timing->v_freq_1000;
            }
            else
            {
              gen_timing->v_freq_1000 = V_FREQ_60000;
            }
		}
	}

        rtd_pr_i3ddma_debug("[3DDMA] hw_i3ddma_dma : %x \n", i3ddmaCtrl[dev].hw_i3ddma_dma);
        rtd_pr_i3ddma_debug("[3DDMA] i3ddma_3dformat : %x \n", gen_timing->i3ddma_3dformat);
        rtd_pr_i3ddma_debug("[3DDMA] progressive : %x \n", gen_timing->progressive);
        rtd_pr_i3ddma_debug("[3DDMA] chroma : %x \n", gen_timing->color);
        rtd_pr_i3ddma_debug("[3DDMA] v_total : %x \n", gen_timing->v_total);
        rtd_pr_i3ddma_debug("[3DDMA] v_act_len : %x \n", gen_timing->v_act_len);
        rtd_pr_i3ddma_debug("[3DDMA] v_act_sta : %x \n", gen_timing->v_act_sta);
        rtd_pr_i3ddma_debug("[3DDMA] h_total : %x \n", gen_timing->h_total);
        rtd_pr_i3ddma_debug("[3DDMA] h_act_len : %x \n", gen_timing->h_act_len);
        rtd_pr_i3ddma_debug("[3DDMA] h_act_sta : %x \n", gen_timing->h_act_sta);
        rtd_pr_i3ddma_debug("[3DDMA] v_freq_1000 : %x \n", gen_timing->v_freq_1000);
        rtd_pr_i3ddma_debug("[3DDMA] h_freq : %x \n", gen_timing->h_freq);
}

unsigned int encodemap[4] = {
	0,
	1,
	0,
	2,
};


char I3DDMA_Setup3DDMA(I3DDMA_3DDMA_CTRL_T* ctrl, I3DDMA_3D_OPMODE_T opmode,unsigned char dev) {
    //i3ddma_enable_RBUS 	i3ddma_i3ddma_enable_reg;
    h3ddma_i3ddma_enable_RBUS  i3ddma_i3ddma_enable_reg;
    //lr_separate_ctrl1_RBUS   i3ddma_lr_separate_ctrl1_reg;
    //	h3ddma_lr_separate_ctrl1_RBUS  i3ddma_lr_separate_ctrl1_reg;
    //cap_ctl0_RBUS i3ddma_cap_ctl0_reg;
    h3ddma_cap0_cap_ctl0_RBUS  i3ddma_cap_ctl0_reg;
    h3ddma_cap3_cap_ctl0_RBUS  i3ddma_cap3_ctl0_reg;
    h3ddma_cap0_wr_dma_pxltobus_RBUS h3ddma_cap0_wr_dma_pxltobus_reg;
    h3ddma_cap0_cti_dma_wr_ctrl_RBUS h3ddma_cap0_cti_dma_wr_ctrl_reg;
    h3ddma_cap1_cti_dma_wr_ctrl_RBUS h3ddma_cap1_cti_dma_wr_ctrl_reg;
    h3ddma_cap2_cti_dma_wr_ctrl_RBUS h3ddma_cap2_cti_dma_wr_ctrl_reg;
    h3ddma_cap3_cti_dma_wr_ctrl_RBUS h3ddma_cap3_cti_dma_wr_ctrl_reg;
    h3ddma_tg_sf_htotal_RBUS h3ddma_tg_sf_htotal_reg;
    dma_vgip_dma_vgip_ctrl_RBUS dma_vgip_dma_vgip_ctrl_reg;
    h3ddma_interrupt_enable_RBUS h3ddma_interrupt_enable_reg;


	switch (opmode){

		case I3DDMA_3D_OPMODE_DISABLE:
			if(dev == I3DDMA_DEV)
			{
				h3ddma_tg_sf_htotal_reg.regValue = IoReg_Read32(H3DDMA_TG_SF_HTOTAL_reg);
				h3ddma_tg_sf_htotal_reg.tg_slf_htotal = 0;
				h3ddma_tg_sf_htotal_reg.tg_slf_htotal_en = 0;
				IoReg_Write32(H3DDMA_TG_SF_HTOTAL_reg, h3ddma_tg_sf_htotal_reg.regValue);
				//rtd_pr_i3ddma_debug("I3DDMA_3D_OPMODE_DISABLE\n");
	            i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
	            i3ddma_i3ddma_enable_reg.block_sel_to_flag_fifo_option = 0;
	            IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_i3ddma_enable_reg.regValue);

	            h3ddma_interrupt_enable_reg.regValue = IoReg_Read32(H3DDMA_Interrupt_Enable_reg);
	            h3ddma_interrupt_enable_reg.cap0_last_wr_ie = 0;
	            IoReg_Write32(H3DDMA_Interrupt_Enable_reg, h3ddma_interrupt_enable_reg.regValue);
			}

//			rtd_maskl(I3DDMA_CAP_CTL0_VADDR, ~(I3DDMA_CAP_CTL0_cap_en_mask), I3DDMA_CAP_CTL0_cap_en(0));
			#if 0
			i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
			i3ddma_lr_separate_ctrl1_reg.regValue = IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg);
			i3ddma_lr_separate_ctrl1_reg.hdmi_3d_sel = 0;
			i3ddma_i3ddma_enable_reg.cap_en = 0;
			IoReg_Write32(H3DDMA_LR_Separate_CTRL1_reg, i3ddma_lr_separate_ctrl1_reg.regValue);
			IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_i3ddma_enable_reg.regValue);
			#endif
			i3ddmaCtrl[dev].enable_3ddma = 0;

		break;
		case I3DDMA_3D_OPMODE_HW:
			if(IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE)
			{
				dma_vgip_dma_vgip_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
				if(dma_vgip_dma_vgip_ctrl_reg.dma_digital_mode)
				{//digital mode
					h3ddma_tg_sf_htotal_reg.regValue = IoReg_Read32(H3DDMA_TG_SF_HTOTAL_reg);
					h3ddma_tg_sf_htotal_reg.tg_slf_htotal = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN);//set h total
					h3ddma_tg_sf_htotal_reg.tg_slf_htotal_en = 1;
					IoReg_Write32(H3DDMA_TG_SF_HTOTAL_reg, h3ddma_tg_sf_htotal_reg.regValue);
				}
				else
				{//analog mode
					h3ddma_tg_sf_htotal_reg.regValue = IoReg_Read32(H3DDMA_TG_SF_HTOTAL_reg);
					h3ddma_tg_sf_htotal_reg.tg_slf_htotal = 0;
					h3ddma_tg_sf_htotal_reg.tg_slf_htotal_en = 0;
					IoReg_Write32(H3DDMA_TG_SF_HTOTAL_reg, h3ddma_tg_sf_htotal_reg.regValue);
				}
				//rtd_pr_i3ddma_debug("I3DDMA_3D_OPMODE_HW\n");
				//rtd_maskl(I3DDMA_CAP_CTL0_VADDR, ~(I3DDMA_CAP_CTL0_cap_en_mask), I3DDMA_CAP_CTL0_cap_en(0));

				h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg);
				h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_enable = 0;
				IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg, h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue);

				h3ddma_cap1_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg);
				h3ddma_cap1_cti_dma_wr_ctrl_reg.cap1_dma_enable = 0;
				IoReg_Write32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg, h3ddma_cap1_cti_dma_wr_ctrl_reg.regValue);

				h3ddma_cap2_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP2_CTI_DMA_WR_Ctrl_reg);
				h3ddma_cap2_cti_dma_wr_ctrl_reg.cap2_dma_enable = 0;
				IoReg_Write32(H3DDMA_CAP2_CTI_DMA_WR_Ctrl_reg, h3ddma_cap2_cti_dma_wr_ctrl_reg.regValue);

				h3ddma_cap3_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP3_CTI_DMA_WR_Ctrl_reg);
				h3ddma_cap3_cti_dma_wr_ctrl_reg.cap3_dma_enable = 0;
				IoReg_Write32(H3DDMA_CAP3_CTI_DMA_WR_Ctrl_reg, h3ddma_cap3_cti_dma_wr_ctrl_reg.regValue);


				h3ddma_cap0_wr_dma_pxltobus_reg.regValue = IoReg_Read32(H3DDMA_CAP0_WR_DMA_pxltobus_reg);
				h3ddma_cap0_wr_dma_pxltobus_reg.cap0_bit_sel = ctrl->pgen_timing->depth != I3DDMA_COLOR_DEPTH_8B;
				h3ddma_cap0_wr_dma_pxltobus_reg.cap0_pixel_encoding = encodemap[ctrl->pgen_timing->color];
				IoReg_Write32(H3DDMA_CAP0_WR_DMA_pxltobus_reg, h3ddma_cap0_wr_dma_pxltobus_reg.regValue);
			}

			I3DDMA_SetupLRSeparator(dev, ctrl->ptx_timing, ctrl->pgen_timing);
			if(IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE)
			{
				I3DDMA_Setup3Dtg(ctrl->pgen_timing, ctrl->frame_rate_x2);
				I3DDMA_SetupCapture0Init(ctrl->cap_buffer, ctrl->pgen_timing, opmode);
			}
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_HDMI_NN
				//aipq enable or hfr HDMI init i3ddma nn cap
			if(scalerAI_get_AIPQ_mode_enable_flag())
			{
				h3ddma_set_cap_enable_mask(I3DDMA_NN_ENABLE_CAP);
			}
			h3ddma_nn_run_capture_config(dev, ctrl->pgen_timing);
#endif
#endif


			if(IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE)
			{
				i3ddma_cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_CAP0_Cap_CTL0_reg);
				i3ddma_cap3_ctl0_reg.regValue = IoReg_Read32(H3DDMA_CAP3_Cap_CTL0_reg);
				i3ddma_cap_ctl0_reg.cap0_auto_block_sel_en = 1;
				//i3ddma_cap_ctl0_reg.cap0_half_wr_sel = 0;
				//i3ddma_cap_ctl0_reg.cap0_half_wr_en = 0;

				if(get_scaler_qms_mode_flag())
				{//sync mode does not need to use single buffer
					if(drvif_i3ddma_triplebuf_flag()||drvif_i3ddma_triplebuf_by_timing_protect_panel())
					{
						if(IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_2_reg)){
							i3ddma_cap3_ctl0_reg.cap3_triple_buf_en = 1;
						}
					}
					i3ddma_cap_ctl0_reg.cap0_triple_buf_en = 1;//qms need to use tripple buffer
					i3ddma_cap_ctl0_reg.cap0_cap_block_fw = 0;//block 0

					//i3ddma_cap1_interrupt_ctrl(TRUE);//enable i3ddma cap1 interrupt
					drvif_scaler_dma_vgip_sta_irq_ctrl(1);//enable dma vgip start isr for qms
				}
				else if(drvif_i3ddma_triplebuf_flag()||drvif_i3ddma_triplebuf_by_timing_protect_panel()){
					if(IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_2_reg)){
						i3ddma_cap_ctl0_reg.cap0_triple_buf_en = 1;
						i3ddma_cap3_ctl0_reg.cap3_triple_buf_en = 1;
					}
				}
				else {
					i3ddma_cap_ctl0_reg.cap0_triple_buf_en = 0;
					i3ddma_cap3_ctl0_reg.cap3_triple_buf_en = 0;
				}
				IoReg_Write32(H3DDMA_CAP0_Cap_CTL0_reg, i3ddma_cap_ctl0_reg.regValue);
	                    IoReg_Write32(H3DDMA_CAP3_Cap_CTL0_reg, i3ddma_cap3_ctl0_reg.regValue);
				//rtd_maskl(I3DDMA_CAP_CTL0_VADDR, ~(I3DDMA_CAP_CTL0_cap_en_mask),
					//I3DDMA_CAP_CTL0_cap_en(1));

				h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg);
				h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_enable = 1;
				IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg, h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue);

				I3DDMA_SetupVODMA(ctrl->pgen_timing,SLR_MAIN_DISPLAY);
			}

			i3ddmaCtrl[dev].enable_3ddma = 1;
		break;

		case I3DDMA_3D_OPMODE_HW_2DONLY_L:
			//rtd_pr_i3ddma_debug("I3DDMA_3D_OPMODE_HW_2DONLY_L\n");
			//rtd_maskl(I3DDMA_CAP_CTL0_VADDR, ~(I3DDMA_CAP_CTL0_cap_en_mask), I3DDMA_CAP_CTL0_cap_en(0));
			i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
			//i3ddma_i3ddma_enable_reg.cap_en = 0;
			IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_i3ddma_enable_reg.regValue);

			h3ddma_cap0_wr_dma_pxltobus_reg.regValue = IoReg_Read32(H3DDMA_CAP0_WR_DMA_pxltobus_reg);
			h3ddma_cap0_wr_dma_pxltobus_reg.cap0_bit_sel = ctrl->pgen_timing->depth != I3DDMA_COLOR_DEPTH_8B;
			h3ddma_cap0_wr_dma_pxltobus_reg.cap0_pixel_encoding = encodemap[ctrl->pgen_timing->color];
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_pxltobus_reg, h3ddma_cap0_wr_dma_pxltobus_reg.regValue);

			I3DDMA_SetupLRSeparator(dev, ctrl->ptx_timing, ctrl->pgen_timing);
			if(IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE)
			{
				I3DDMA_Setup3Dtg(ctrl->pgen_timing, ctrl->frame_rate_x2);
				I3DDMA_SetupCapture0Init(ctrl->cap_buffer, ctrl->pgen_timing, opmode);
			}
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_HDMI_NN
			//aipq enable or hfr HDMI init i3ddma nn cap
			if(scalerAI_get_AIPQ_mode_enable_flag())
			{
				h3ddma_set_cap_enable_mask(I3DDMA_NN_ENABLE_CAP);
			}
			h3ddma_nn_run_capture_config(dev, ctrl->pgen_timing);
#endif
#endif

			i3ddma_cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_CAP0_Cap_CTL0_reg);
			i3ddma_cap_ctl0_reg.cap0_auto_block_sel_en = 1;
			//i3ddma_cap_ctl0_reg.cap0_half_wr_sel = 0;
			//i3ddma_cap_ctl0_reg.cap0_half_wr_en = 1;
			IoReg_Write32(H3DDMA_CAP0_Cap_CTL0_reg, i3ddma_cap_ctl0_reg.regValue);

			//rtd_maskl(I3DDMA_CAP_CTL0_VADDR, ~(I3DDMA_CAP_CTL0_cap_en_mask),I3DDMA_CAP_CTL0_cap_en(1));
			i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
		//	i3ddma_i3ddma_enable_reg.cap_en = 1;
			IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_i3ddma_enable_reg.regValue);
			I3DDMA_SetupVODMA(ctrl->pgen_timing,SLR_MAIN_DISPLAY);
			i3ddmaCtrl[dev].enable_3ddma = 1;

		break;
		case I3DDMA_3D_OPMODE_HW_2DONLY_R:
			//rtd_pr_i3ddma_debug("I3DDMA_3D_OPMODE_HW_2DONLY_R\n");
			//rtd_maskl(I3DDMA_CAP_CTL0_VADDR, ~(I3DDMA_CAP_CTL0_cap_en_mask), I3DDMA_CAP_CTL0_cap_en(0));
			i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
//			i3ddma_i3ddma_enable_reg.cap_en = 0;
			IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_i3ddma_enable_reg.regValue);

			h3ddma_cap0_wr_dma_pxltobus_reg.regValue = IoReg_Read32(H3DDMA_CAP0_WR_DMA_pxltobus_reg);
			h3ddma_cap0_wr_dma_pxltobus_reg.cap0_bit_sel = ctrl->pgen_timing->depth != I3DDMA_COLOR_DEPTH_8B;
			h3ddma_cap0_wr_dma_pxltobus_reg.cap0_pixel_encoding = encodemap[ctrl->pgen_timing->color];
			IoReg_Write32(H3DDMA_CAP0_WR_DMA_pxltobus_reg, h3ddma_cap0_wr_dma_pxltobus_reg.regValue);

			I3DDMA_SetupLRSeparator(dev, ctrl->ptx_timing, ctrl->pgen_timing);
			if(IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE)
			{
				I3DDMA_Setup3Dtg(ctrl->pgen_timing, ctrl->frame_rate_x2);
				I3DDMA_SetupCapture0Init(ctrl->cap_buffer, ctrl->pgen_timing, opmode);
			}
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_HDMI_NN
			//aipq enable or hfr HDMI init i3ddma nn cap
			if(scalerAI_get_AIPQ_mode_enable_flag())
			{
				h3ddma_set_cap_enable_mask(I3DDMA_NN_ENABLE_CAP);
			}
			h3ddma_nn_run_capture_config(dev, ctrl->pgen_timing);
#endif
#endif

			i3ddma_cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_CAP0_Cap_CTL0_reg);
			i3ddma_cap_ctl0_reg.cap0_auto_block_sel_en = 1;
			//i3ddma_cap_ctl0_reg.cap0_half_wr_sel = 1;
			//i3ddma_cap_ctl0_reg.cap0_half_wr_en = 1;
			IoReg_Write32(H3DDMA_CAP0_Cap_CTL0_reg, i3ddma_cap_ctl0_reg.regValue);

			//rtd_maskl(I3DDMA_CAP_CTL0_VADDR, ~(I3DDMA_CAP_CTL0_cap_en_mask),I3DDMA_CAP_CTL0_cap_en(1));
			i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
		//	i3ddma_i3ddma_enable_reg.cap_en = 1;
			IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_i3ddma_enable_reg.regValue);
			I3DDMA_SetupVODMA(ctrl->pgen_timing,SLR_MAIN_DISPLAY);
			i3ddmaCtrl[dev].enable_3ddma = 1;


		break;
		case I3DDMA_3D_OPMODE_HW_NODMA:
			//rtd_pr_i3ddma_debug("I3DDMA_3D_OPMODE_HW_NODMA\n");
			I3DDMA_SetupLRSeparator(dev, ctrl->ptx_timing, ctrl->pgen_timing);
			if(IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE)
			{
				I3DDMA_Setup3Dtg(ctrl->pgen_timing, ctrl->frame_rate_x2);
			}
			rtd_pr_i3ddma_debug("%s %d\n", __FUNCTION__, __LINE__);
			i3ddmaCtrl[dev].enable_3ddma = 0;
		break;
		case I3DDMA_3D_OPMODE_SW:
            rtd_pr_i3ddma_emerg("[I3DDMA][%s %d]I3DDMA_3D_OPMODE_SW\n", __FUNCTION__, __LINE__);
            dma_vgip_dma_vgip_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
            if(dma_vgip_dma_vgip_ctrl_reg.dma_digital_mode)
            {//digital mode
            	h3ddma_tg_sf_htotal_reg.regValue = IoReg_Read32(H3DDMA_TG_SF_HTOTAL_reg);
            	h3ddma_tg_sf_htotal_reg.tg_slf_htotal = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN);//set h total
            	h3ddma_tg_sf_htotal_reg.tg_slf_htotal_en = 1;
            	IoReg_Write32(H3DDMA_TG_SF_HTOTAL_reg, h3ddma_tg_sf_htotal_reg.regValue);
            }
            else
            {//analog mode
            	h3ddma_tg_sf_htotal_reg.regValue = IoReg_Read32(H3DDMA_TG_SF_HTOTAL_reg);
            	h3ddma_tg_sf_htotal_reg.tg_slf_htotal = 0;
            	h3ddma_tg_sf_htotal_reg.tg_slf_htotal_en = 0;
            	IoReg_Write32(H3DDMA_TG_SF_HTOTAL_reg, h3ddma_tg_sf_htotal_reg.regValue);
            }

            h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg);
            h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_enable = 0;
            IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg, h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue);

            h3ddma_cap0_wr_dma_pxltobus_reg.regValue = IoReg_Read32(H3DDMA_CAP0_WR_DMA_pxltobus_reg);
            h3ddma_cap0_wr_dma_pxltobus_reg.cap0_bit_sel = ctrl->pgen_timing->depth != I3DDMA_COLOR_DEPTH_8B;
            h3ddma_cap0_wr_dma_pxltobus_reg.cap0_pixel_encoding = encodemap[ctrl->pgen_timing->color];
            IoReg_Write32(H3DDMA_CAP0_WR_DMA_pxltobus_reg, h3ddma_cap0_wr_dma_pxltobus_reg.regValue);

            I3DDMA_SetupLRSeparator(dev, ctrl->ptx_timing, ctrl->pgen_timing);
            if(IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE)
			{
				I3DDMA_Setup3Dtg(ctrl->pgen_timing, ctrl->frame_rate_x2);
				I3DDMA_SetupCapture0Init(ctrl->cap_buffer, ctrl->pgen_timing, opmode);
			}
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_HDMI_NN
			//aipq enable or hfr HDMI init i3ddma nn cap
			if(scalerAI_get_AIPQ_mode_enable_flag())
			{
				h3ddma_set_cap_enable_mask(I3DDMA_NN_ENABLE_CAP);
			}
			h3ddma_nn_run_capture_config(dev, ctrl->pgen_timing);
#endif
#endif

            i3ddma_cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_CAP0_Cap_CTL0_reg);
            i3ddma_cap_ctl0_reg.cap0_auto_block_sel_en = 0;//1 use  auto mode for interlace input
            //i3ddma_cap_ctl0_reg.cap0_half_wr_sel = 0;
            //i3ddma_cap_ctl0_reg.cap0_half_wr_en = 0;
            i3ddma_cap_ctl0_reg.cap0_triple_buf_en = drvif_i3ddma_triplebuf_flag() ? 1 : 0;
            i3ddma_cap_ctl0_reg.cap0_quad_buf_en = 0;
            IoReg_Write32(H3DDMA_CAP0_Cap_CTL0_reg, i3ddma_cap_ctl0_reg.regValue);

            /*i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
            i3ddma_i3ddma_enable_reg.block_sel_to_flag_fifo_option = 1;
            IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_i3ddma_enable_reg.regValue);*/

            h3ddma_interrupt_enable_reg.regValue = IoReg_Read32(H3DDMA_Interrupt_Enable_reg);
            h3ddma_interrupt_enable_reg.cap0_last_wr_ie = 1;
            IoReg_Write32(H3DDMA_Interrupt_Enable_reg, h3ddma_interrupt_enable_reg.regValue);

            h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg);
            h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_enable = 1;
            IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg, h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue);

            I3DDMA_SetupVODMA(ctrl->pgen_timing,SLR_MAIN_DISPLAY);
            i3ddmaCtrl[dev].enable_3ddma = 1;
		default:
			return I3DDMA_ERR_3D_WRONG_OPMODE;
		break;

	}

	return I3DDMA_ERR_NO;

}
#ifndef BUILD_QUICK_SHOW

#define I3DDMA_FILE_PATH "/tmp/i3ddma_dump.raw"
#define I3DDMA_FILE_PATH_Y "/tmp/i3ddma_y_dump.raw"
#define I3DDMA_FILE_PATH_C "/tmp/i3ddma_c_dump.raw"
#ifndef UT_flag
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

#ifdef CONFIG_ENABLE_HDMI_NN
unsigned int g_ulFileCount = 0;
int i3ddma_dump_data_to_file(unsigned char dev)
{
	struct file* filp = NULL;
	static unsigned char dump_mode = 1;
	unsigned long outfileOffset = 0;
	unsigned int size = (g_tNN_Cap_Buffer[0].size + g_tNN_Cap_Buffer[0].size / 2) * NN_CAP_BUFFER_NUM;
	unsigned char* pVirStartAdrr = NULL;
	unsigned char i = 0;
	unsigned int uzd_y_addr = 0, uzd_c_addr = 0, crop_y_addr = 0, crop_c_addr = 0;

	const char y_path[60] = "/tmp/i3ddma_y_dump";

	char filepath[120];

	static unsigned int bufferindex = 0;

	I3DDMA_NN_BUFFER_ATTR_T i3ddma_nn_buffer_attr_addr[NN_CAP_BUFFER_NUM];
	I3DDMA_NN_BUFFER_ATTR_T* p_i3ddma_nn_buffer_attr_addr = NULL;

	sprintf((unsigned char*)y_path, "/tmp/i%dddma_dump", (dev+3));

	dump_mode = ((IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg)) & _BIT28) ? 1 : 2;

	if(dump_mode == 0)
	{
		if((unsigned char *)Scaler_GetShareMemVirAddr(SCALERIOC_I3_NN_SHARE_BUF_STATUS) == NULL)
		{
			rtd_pr_i3ddma_err("[NN][%s] RPC not init!\n", __FUNCTION__);
			return -1;
		}

		size = drvif_memory_get_data_align(size, __4KPAGE);
		pVirStartAdrr = (unsigned char*)dvr_remap_uncached_memory((unsigned int)g_tNN_Cap_Buffer[0].phyaddr, size, __builtin_return_address(0));
		if(pVirStartAdrr == NULL)
			return -1;

		if(H3DDMA_Interrupt_Enable_get_cap1_last_wr_int_mux(IoReg_Read32(H3DDMA_Interrupt_Enable_reg)))
		{
			for (i = 0; i < NN_CAP_BUFFER_NUM; i++)
			{
				p_i3ddma_nn_buffer_attr_addr = (I3DDMA_NN_BUFFER_ATTR_T *)Scaler_GetShareMemVirAddr(SCALERIOC_I3_NN_SHARE_BUF_STATUS) + i;
				//rtd_pr_i3ddma_info("addr = %x, mode = %d, buffer status=%d", p_i3ddma_nn_buffer_attr_addr->y_buffer_addr, p_i3ddma_nn_buffer_attr_addr->cap_mode, p_i3ddma_nn_buffer_attr_addr->buf_status);
				i3ddma_nn_buffer_attr_addr[i].cap_mode = Scaler_ChangeUINT32Endian(p_i3ddma_nn_buffer_attr_addr->cap_mode);
				i3ddma_nn_buffer_attr_addr[i].buf_status = Scaler_ChangeUINT32Endian(p_i3ddma_nn_buffer_attr_addr->buf_status);
				i3ddma_nn_buffer_attr_addr[i].y_buffer_addr = Scaler_ChangeUINT32Endian(p_i3ddma_nn_buffer_attr_addr->y_buffer_addr);
				i3ddma_nn_buffer_attr_addr[i].c_buffer_addr = Scaler_ChangeUINT32Endian(p_i3ddma_nn_buffer_attr_addr->c_buffer_addr);
				i3ddma_nn_buffer_attr_addr[i].timestamp = Scaler_ChangeUINT64Endian(p_i3ddma_nn_buffer_attr_addr->timestamp);

				//rtd_pr_i3ddma_info("addr = %x, mode = %d, buffer status=%d", i3ddma_nn_buffer_attr_addr[i].y_buffer_addr, i3ddma_nn_buffer_attr_addr[i].cap_mode, i3ddma_nn_buffer_attr_addr[i].buf_status);
			}
		}
		else
		{
			memcpy((unsigned char *)i3ddma_nn_buffer_attr_addr, (unsigned char *)Scaler_GetShareMemVirAddr(SCALERIOC_I3_NN_SHARE_BUF_STATUS), sizeof(I3DDMA_NN_BUFFER_ATTR_T) * NN_CAP_BUFFER_NUM);
		}

		bufferindex = bufferindex % NN_CAP_BUFFER_NUM;

		rtd_pr_i3ddma_info("idx = %d, mode = %d, buffer status=%d", bufferindex, i3ddma_nn_buffer_attr_addr[bufferindex].cap_mode, i3ddma_nn_buffer_attr_addr[bufferindex].buf_status);

		if(!(i3ddma_nn_buffer_attr_addr[bufferindex].y_buffer_addr && i3ddma_nn_buffer_attr_addr[bufferindex].c_buffer_addr))
			return 0;

		g_ulFileCount++;
		sprintf(filepath, "%s_%d_%s_%s_%d.raw", y_path, bufferindex, ((i3ddma_nn_buffer_attr_addr[bufferindex].cap_mode == I3DDMA_UZD_CAP)?"UZD":"CROP"),
			((i3ddma_nn_buffer_attr_addr[bufferindex].buf_status == I3DDMA_NN_BUFF_WRITE_DONE)?"DONE":"USING"), g_ulFileCount);

		filp = file_open(filepath, O_RDWR | O_CREAT | O_APPEND, 755);
		if (filp == NULL) {
			rtd_pr_i3ddma_emerg("file open fail\n");
			return FALSE;
		}
		rtd_pr_i3ddma_emerg("dump y data uncache add=%lx, size=%d\n", (unsigned long)(i3ddma_nn_buffer_attr_addr[bufferindex].y_buffer_addr), g_ulNNOutWidth*g_ulNNOutLength);
		file_write(filp, outfileOffset, (unsigned char*)(pVirStartAdrr+i3ddma_nn_buffer_attr_addr[bufferindex].y_buffer_addr-g_tNN_Cap_Buffer[0].phyaddr), g_ulNNOutWidth*g_ulNNOutLength);

		file_sync(filp);
		file_close(filp);

		filp = file_open(filepath, O_RDWR | O_CREAT | O_APPEND, 755);
		if (filp == NULL) {
			rtd_pr_i3ddma_emerg("file open fail\n");
			return FALSE;
		}
		rtd_pr_i3ddma_emerg("dump c data uncache add=%lx, size=%d\n", (unsigned long)(i3ddma_nn_buffer_attr_addr[bufferindex].c_buffer_addr), g_ulNNOutWidth*g_ulNNOutLength/2);
		file_write(filp, outfileOffset, (unsigned char*)(pVirStartAdrr + i3ddma_nn_buffer_attr_addr[bufferindex].c_buffer_addr-g_tNN_Cap_Buffer[0].phyaddr), g_ulNNOutWidth*g_ulNNOutLength/2);
		file_sync(filp);
		file_close(filp);

		bufferindex++;
	}
	else if(dump_mode == 1) //dump_mode == 1
	{
		if((unsigned char *)Scaler_GetShareMemVirAddr(SCALERIOC_I3_NN_SHARE_BUF_STATUS) == NULL)
		{
			rtd_pr_i3ddma_err("[NN][%s] RPC not init!\n", __FUNCTION__);
			return -1;
		}

		size = drvif_memory_get_data_align(size, __4KPAGE);
		pVirStartAdrr = (unsigned char*)dvr_remap_uncached_memory((unsigned int)g_tNN_Cap_Buffer[0].phyaddr, size, __builtin_return_address(0));
		if(pVirStartAdrr == NULL)
			return -1;

		//msleep(50);
		if(h3ddma_get_NN_read_buffer(&uzd_y_addr, &uzd_c_addr, NULL, &crop_y_addr, &crop_c_addr, NULL) < 0)
			return -1;

		g_ulFileCount++;

		if(uzd_y_addr && uzd_c_addr)
		{
			//dump uzd data
			sprintf(filepath, "%s_y_%s_%d.raw", y_path, "UZD", g_ulFileCount);

			filp = file_open(filepath, O_RDWR | O_CREAT | O_APPEND, 755);
			if (filp == NULL) {
				rtd_pr_i3ddma_emerg("file open fail\n");
				return FALSE;
			}
			rtd_pr_i3ddma_emerg("dump uzd y data uncache add=%lx, size=%d %x\n", (unsigned long)(uzd_y_addr), g_ulNNOutWidth*g_ulNNOutLength, IoReg_Read32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg));
			file_write(filp, outfileOffset, (unsigned char*)(pVirStartAdrr+uzd_y_addr-g_tNN_Cap_Buffer[0].phyaddr), g_ulNNOutWidth*g_ulNNOutLength);

			file_sync(filp);
			file_close(filp);

			sprintf(filepath, "%s_c_%s_%d.raw", y_path, "UZD", g_ulFileCount);
			filp = file_open(filepath, O_RDWR | O_CREAT | O_APPEND, 755);
			if (filp == NULL) {
				rtd_pr_i3ddma_emerg("file open fail\n");
				return FALSE;
			}
			rtd_pr_i3ddma_emerg("dump uzd c data uncache add=%lx, size=%d\n", (unsigned long)(uzd_c_addr), g_ulNNOutWidth*g_ulNNOutLength/2);
			file_write(filp, outfileOffset, (unsigned char*)(pVirStartAdrr + uzd_c_addr-g_tNN_Cap_Buffer[0].phyaddr), g_ulNNOutWidth*g_ulNNOutLength/2);
			file_sync(filp);
			file_close(filp);
		}

		if(crop_y_addr && crop_c_addr)
		{
			//dump crop data
			sprintf(filepath, "%s_y_%s_%d.raw", y_path, "Crop", g_ulFileCount);

			filp = file_open(filepath, O_RDWR | O_CREAT | O_APPEND, 755);
			if (filp == NULL) {
				rtd_pr_i3ddma_emerg("file open fail\n");
				return FALSE;
			}
			rtd_pr_i3ddma_emerg("dump crop y data uncache add=%lx, size=%d\n", (unsigned long)(crop_y_addr), g_ulNNOutWidth*g_ulNNOutLength);
			file_write(filp, outfileOffset, (unsigned char*)(pVirStartAdrr+crop_y_addr-g_tNN_Cap_Buffer[0].phyaddr), g_ulNNOutWidth*g_ulNNOutLength);

			file_sync(filp);
			file_close(filp);

			sprintf(filepath, "%s_c_%s_%d.raw", y_path, "Crop", g_ulFileCount);
			filp = file_open(filepath, O_RDWR | O_CREAT | O_APPEND, 755);
			if (filp == NULL) {
				rtd_pr_i3ddma_emerg("file open fail\n");
				return FALSE;
			}
			rtd_pr_i3ddma_emerg("dump crop c data uncache add=%lx, size=%d\n", (unsigned long)(crop_c_addr), g_ulNNOutWidth*g_ulNNOutLength/2);
			file_write(filp, outfileOffset, (unsigned char*)(pVirStartAdrr + crop_c_addr-g_tNN_Cap_Buffer[0].phyaddr), g_ulNNOutWidth*g_ulNNOutLength/2);
			file_sync(filp);
			file_close(filp);
		}

		//h3ddma_nn_set_cap_enable(1);
	}
	else
	{
		extern unsigned int get_image_y_buffer_size(unsigned char dev);
		static unsigned char index = 0;
		g_ulFileCount++;
		size = drvif_memory_get_data_align(get_image_y_buffer_size(dev) * 3 / 2, __4KPAGE);
		pVirStartAdrr = (unsigned char*)dvr_remap_uncached_memory((unsigned int)iddma_attr[dev].iddma_buffer[index].phyaddr, size, __builtin_return_address(0));
		if(pVirStartAdrr == NULL)
			return -1;

		if(pVirStartAdrr)
		{
			//dump uzd data
			sprintf(filepath, "%s_%s_%d.raw", y_path, "UZD", g_ulFileCount);

			filp = file_open(filepath, O_RDWR | O_CREAT | O_APPEND, 755);
			if (filp == NULL) {
				rtd_pr_i3ddma_emerg("file open fail\n");
				return FALSE;
			}

			rtd_pr_i3ddma_emerg("dump data add=%lx read pnt=%d\n", (unsigned long)(iddma_attr[dev].iddma_buffer[index].phyaddr), index);
			file_write(filp, outfileOffset, (unsigned char*)(pVirStartAdrr), size);

			file_sync(filp);
			file_close(filp);

			index = (index + 1) % iddma_attr[dev].buffer_num;
		}
	}

	dvr_unmap_memory(pVirStartAdrr, size);

	return TRUE;
}




void h3ddma_get_NN_read_buffer_test(unsigned int *a_pulYAddr, unsigned int *a_pulCAddr)
{
	*a_pulYAddr = IoReg_Read32(H3DDMA_CAP1_WR_DMA_num_bl_wrap_addr_0_reg);
	*a_pulCAddr = IoReg_Read32(H3DDMA_CAP2_WR_DMA_num_bl_wrap_addr_0_reg);
}

#endif //end of #ifdef CONFIG_ENABLE_HDMI_NN
#endif //end of #ifndef UT_flag
#endif //end of #ifndef BUILD_QUICK_SHOW

unsigned long I3DDMA_GetMemChunkAddr(unsigned int size)
{
//#define _OFFSET_BYTES    (24*1024*1024)//offset 30M Bytes//32*1024*1024
    unsigned long ulPhyaddr = 0;
	unsigned buffersize;

    if(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr)
	{
		if(drvif_i3ddma_triplebuf_flag())
			buffersize = i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].size *3;
		else
			buffersize = i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].size *2;

		if(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize < buffersize)
		{
			rtd_pr_i3ddma_info("I3DDMA_GetMemChunkAddr get size(%d < %d) fail\n", i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize , buffersize);
			return 0;
		}
		if (size >  i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize - buffersize)
		{
			rtd_pr_i3ddma_info("I3DDMA_GetMemChunkAddr get size(%d) not enough\n", size);
		 	return 0;
		}
	    ulPhyaddr = i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr + buffersize;
	}
	return ulPhyaddr;
}

void disable_i3ddma_cap(void)
{//disable capture 0 for 4k120 borrow i3ddma memory
	unsigned int timeoutcount = 5;
	IoReg_ClearBits(H3DDMA_H3DDMA_PQ_CMP_DOUBLE_BUFFER_CTRL_reg, H3DDMA_H3DDMA_PQ_CMP_DOUBLE_BUFFER_CTRL_db_read_sel_mask);

	/* Disable DMA VGIP online measure before reset VGIP source select */
	drvif_mode_dma_onlinemeasure_setting(FALSE, FALSE);//Disable DMA online measure watchdog and interrupt
	drvif_mode_disable_dma_onlinemeasure();//Disable DMA online measure

	drvif_I3DDMA_Reset_Vgip_Source_Sel(I3DDMA_DEV);
	h3ddma_set_capture_enable(0,0);//disable i3ddma capture 0. becasue need alloc memocy again
	h3ddma_set_capture_enable(1,0);
	h3ddma_set_capture_enable(2,0);
	h3ddma_set_capture_enable(3,0);
	do {
		if((H3DDMA_CAP0_CTI_DMA_WR_Ctrl_get_cap0_dma_enable(IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg)) == 0) &&
			(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_get_cap1_dma_enable(IoReg_Read32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg)) == 0) &&
			(H3DDMA_CAP2_CTI_DMA_WR_Ctrl_get_cap2_dma_enable(IoReg_Read32(H3DDMA_CAP2_CTI_DMA_WR_Ctrl_reg)) == 0) &&
			(H3DDMA_CAP3_CTI_DMA_WR_Ctrl_get_cap3_dma_enable(IoReg_Read32(H3DDMA_CAP3_CTI_DMA_WR_Ctrl_reg)) == 0))
			break;

		msleep(10);
		timeoutcount--;
	}while(timeoutcount);

	rtd_pr_i3ddma_notice("### %s i3ddma capture disable status (%d %d %d %d), timeoutcnt=%d db=%x", __FUNCTION__, (H3DDMA_CAP0_CTI_DMA_WR_Ctrl_get_cap0_dma_enable(IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg))),
		(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_get_cap1_dma_enable(IoReg_Read32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg))),
		(H3DDMA_CAP2_CTI_DMA_WR_Ctrl_get_cap2_dma_enable(IoReg_Read32(H3DDMA_CAP2_CTI_DMA_WR_Ctrl_reg))),
		(H3DDMA_CAP3_CTI_DMA_WR_Ctrl_get_cap3_dma_enable(IoReg_Read32(H3DDMA_CAP3_CTI_DMA_WR_Ctrl_reg))),
				timeoutcount, IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_DOUBLE_BUFFER_CTRL_reg));

	//recovery i3ddma read level
	IoReg_SetBits(H3DDMA_H3DDMA_PQ_CMP_DOUBLE_BUFFER_CTRL_reg, H3DDMA_H3DDMA_PQ_CMP_DOUBLE_BUFFER_CTRL_db_read_sel_mask);
}

unsigned char IDDMA_DisableCap(unsigned char dev)
{
	unsigned int timeoutcount = 5;
	IoReg_ClearBits(iddma_db_reg_addr[dev], H3DDMA_H3DDMA_PQ_CMP_DOUBLE_BUFFER_CTRL_db_read_sel_mask);

	/* Disable DMA VGIP online measure before reset VGIP source select */
	drvif_mode_dma_onlinemeasure_setting(FALSE, FALSE);//Disable DMA online measure watchdog and interrupt
	drvif_mode_disable_dma_onlinemeasure();//Disable DMA online measure

	drvif_I3DDMA_Reset_Vgip_Source_Sel(dev);
	h3ddma_nn_set_cap_enable(dev, 0);
	do {
		if((H3DDMA_CAP1_CTI_DMA_WR_Ctrl_get_cap1_dma_enable(IoReg_Read32(iddma_cap1_dma_swap_reg_addr[dev])) == 0) &&
			(H3DDMA_CAP2_CTI_DMA_WR_Ctrl_get_cap2_dma_enable(IoReg_Read32(iddma_cap2_dma_swap_reg_addr[dev])) == 0) &&
			(H3DDMA_CAP2_1_CTI_DMA_WR_Ctrl_get_cap2_1_dma_enable(IoReg_Read32(iddma_cap2_1_dma_swap_reg_addr[dev])) == 0))
			break;

		msleep(10);
		timeoutcount--;
	}while(timeoutcount);

	rtd_pr_i3ddma_notice("### %s i%dddma capture disable status (%d %d %d), timeoutcnt=%d db=%x", __FUNCTION__, (dev+3),
		(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_get_cap1_dma_enable(IoReg_Read32(iddma_cap1_dma_swap_reg_addr[dev]))),
		(H3DDMA_CAP2_CTI_DMA_WR_Ctrl_get_cap2_dma_enable(IoReg_Read32(iddma_cap2_dma_swap_reg_addr[dev]))),
		(H3DDMA_CAP2_1_CTI_DMA_WR_Ctrl_get_cap2_1_dma_enable(IoReg_Read32(iddma_cap2_1_dma_swap_reg_addr[dev]))),
			timeoutcount, IoReg_Read32(iddma_db_reg_addr[dev]));

	//recovery i3ddma read level
	IoReg_SetBits(iddma_db_reg_addr[dev], H3DDMA_H3DDMA_PQ_CMP_DOUBLE_BUFFER_CTRL_db_read_sel_mask);

	IDDMA_FreeCapBuffer(dev);

	return TRUE;
}


#ifndef BUILD_QUICK_SHOW

unsigned long I3DDMA_DolbyVision_get_md_buf(void)
{
	return i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].phyaddr;
}

unsigned char* I3DDMA_DolbyVision_get_md_uncach_buf(void)
{
	return i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].uncache;
}

unsigned int I3DDMA_DolbyVision_get_md_buf_size(void)
{
	return MD_BUF_SIZE;
}
#endif

void I3DDMA_Set_UHD_PCmode_Mem_Alloc_Fail_flag(unsigned char value)
{
	i3ddma_pcmode_alloc_memory_flag = value;
	rtd_pr_i3ddma_notice("I3DDMA_Set_UHD_PCmode_Mem_Alloc_Fail_flag flag = %d", i3ddma_pcmode_alloc_memory_flag);
}

unsigned char I3DDMA_Get_UHD_PCmode_Mem_Alloc_Fail_flag(void)
{
	return i3ddma_pcmode_alloc_memory_flag;
}

#ifdef CONFIG_RTK_KDRV_SVP_HDMI_PROTECTION
extern bool  Get_HDMI_Port_is_withHDCP(void);
unsigned char i3ddma_is_protected_flag = 0;
static bool  I3DDMA_Hdmi_Is_Need_Protection(void)
{
        if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI ||
                Get_DisplayMode_Src(SLR_SUB_DISPLAY) == VSC_INPUTSRC_HDMI) {

            if(Get_HDMI_Port_is_withHDCP()){
                return true;
            }
            else {
                return false;
            }
        }
        return false;
}
#endif


unsigned char i3ddma_alloc_memory_cma_flag = 0;
unsigned char I3DDMA_DolbyVision_HDMI_Init(void) {
	unsigned char m_domain_borrow_case = FALSE;//m domain borrow from vbm

	unsigned int size_metadata = MD_BUF_SIZE; // 1MB for metadata buffer
	unsigned int size_video;
//	int i;
	int allocSize = 0;
	unsigned long getSize = 0;
	char bAlloc = false;
	unsigned long malloccacheaddr_metadata = 0;
	unsigned long malloccacheaddr_video_phyaddr = 0;//(unsigned int)dvr_malloc_uncached_specific(size_video*2, GFP_DCU2_FIRST, NULL);//(unsigned int)dvr_malloc(size_video*2);//for double buffer use
	int cur_act_w=0, cur_act_h=0;
	#ifdef CONFIG_ARM64
	unsigned long query_addr = 0, align_addr = 0;
	#else
	unsigned int query_addr = 0, align_addr = 0;
	#endif
	unsigned int uhd_flag = 0;
	unsigned char bAllocFourth = FALSE;
	//if(dvrif_i3ddma_compression_get_enable() == TRUE)
		//size_video = (_4K_WID * _2K_LEN * dvrif_i3ddma_get_compression_bits()) / 8;
	//else
#ifndef CONFIG_RTK_KDRV_DV_IDK_DUMP_RGB
		size_video = _4K_2K_10BIT_SIZE ;//4096*2160 RGB444 10bit sequence buffer
#else
		size_video = _4K_2K_12BIT_SIZE;
#endif

	//rtd_pr_i3ddma_emerg("I3DDMA_DolbyVision_HDMI_Init size_video=%x \n",size_video);

     {
		cur_act_w = IDDMA_GetVFEInfoByPort(I3DDMA_DEV,   SLR_INPUT_IPH_ACT_WID_PRE);//Orignal source size
		if(IDDMA_GetVFEStatus(I3DDMA_DEV, SLR_DISP_INTERLACE))
			cur_act_h = IDDMA_GetVFEInfoByPort(I3DDMA_DEV, SLR_INPUT_IPV_ACT_LEN_PRE)*2;//Orignal source size
		else
			cur_act_h = IDDMA_GetVFEInfoByPort(I3DDMA_DEV, SLR_INPUT_IPV_ACT_LEN_PRE);//Orignal source size
		//rtd_pr_i3ddma_emerg("[Minchay] cur_act_w:%d, cur_act_h:%d \n",cur_act_w,cur_act_h);
		if(cur_act_w * cur_act_h <= _2K_WID *_1K_LEN)//20180507 pinyen, fix 1600x1200 timing ,k5ap can just allocate 2k1k size
		{//video under 2k*1k size, set vmb size to 2k*1k from 4k*2k
				size_video = _2K_1K_10BIT_SIZE ;
				uhd_flag = 0;
		}
		else
			uhd_flag = 1;
	}

	disable_i3ddma_cap();

	rtd_pr_i3ddma_debug("%s.%d\n", __FUNCTION__, __LINE__);

	m_domain_borrow_case = (m_domain_4k_memory_from_type() == BORROW_FROM_VBM);

	if(m_domain_borrow_case)
	{
		size_video = MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K;
		size_video = dvr_size_alignment(size_video);
		allocSize = size_video*3;// 1 m domain cave out  2 vbm memory
		disable_i3ddma_cap();//diable i3ddma
	}
	else{
		if(dvrif_i3ddma_compression_get_enable() == TRUE)
		{
			if(uhd_flag)
			{
				//<2560x1440
				if(cur_act_w * cur_act_h <= _3K_1K_WID * _3K_1K_LEN)
				{
					if(dvrif_i3ddma_get_compression_bits() == COMPRESSION_20_BITS) //pc mode
					{
						size_video = _3K_1K_20BIT_SIZE;
					}
					else
					{
						size_video = _3K_1K_12BIT_SIZE;
					}
				}
				else
				{
					//4k
					if(dvrif_i3ddma_get_compression_bits() == COMPRESSION_20_BITS) //pc mode
					{
						size_video = _4K_2K_20BIT_SIZE;
					}
					else
					{
						size_video = _4K_2K_12BIT_SIZE;
					}
				}
			}
			else
			{
				if(dvrif_i3ddma_get_compression_bits() == COMPRESSION_24_BITS)
				{
					size_video = _2K_1K_24BIT_SIZE;
				}
				else
				{
					size_video = _2K_1K_20BIT_SIZE;
				}
			}

#ifdef CONFIG_RTK_KDRV_DV
		} else if (check_hdmi_dolby_vision_rgb_and_full_hd()) {
			size_video = _2K_1K_36BIT_SIZE;
			rtd_pr_i3ddma_info("[DOLBY] hdmi dolby vision RGB888 12 bit mode, video size = %d bytes\n", size_video);
#endif
		}


		size_video = dvr_size_alignment(size_video);
		if(get_scaler_qms_mode_flag())
		{//alloc 3 buffer memory
			if(size_video >= _4K_2K_20BIT_SIZE)
				allocSize = size_video*2;
			else
				allocSize = size_video*3;
		}
		//special panel run 3buffer,but 4k pc mode 20bits alloc idma memory from mdomain
		else if(drvif_i3ddma_triplebuf_flag() || (drvif_i3ddma_triplebuf_by_timing_protect_panel() && !(get_vsc_run_pc_mode() && (dvrif_i3ddma_get_compression_bits() == COMPRESSION_20_BITS))))
		{
///qms 			size_video = dvr_size_alignment(size_video);
			allocSize = size_video*3;
		}
		else
		{
///qms			size_video = dvr_size_alignment(size_video);
		    allocSize = size_video*2;
		}

#ifndef BUILD_QUICK_SHOW //qs hasn't open dexc, no need alloc memory
		if(uhd_flag == 0)
		{
			if(allocSize < _24M_SIZE)
				allocSize += _24M_SIZE;
			else
				allocSize = _48M_SIZE;
		}
#endif
	}
	//rtd_pr_i3ddma_emerg("allocSize=%x\n", allocSize);

	/*
	IoReg_Write32(H3DDMA_Cap_L1_Start_reg, cap_buffer[0].phyaddr);
	IoReg_Write32(H3DDMA_Cap_L2_Start_reg, cap_buffer[2].phyaddr);
	IoReg_Write32(H3DDMA_Cap_R1_Start_reg, cap_buffer[1].phyaddr);
	IoReg_Write32(H3DDMA_Cap_R2_Start_reg, cap_buffer[3].phyaddr);
	*/
	//video capture buffer
	bAlloc = false;
	if(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr == (unsigned long)NULL)
	{
	   bAlloc = true;
	}
	else if(
#ifndef BUILD_QUICK_SHOW
        (get_video_delay_type() == UNKNOW_VIDEO_DELAY_TYPE) &&
#endif
        (i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize < allocSize)){
	// capture mem < allocSize need realloc mem
	       rtd_pr_i3ddma_emerg("[DolbyVision][Minchay][%s] Error: vbm mem has alloced size(getSize:%d ) < allocSize(%d) \n", __FUNCTION__ , i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize, allocSize);
#ifndef BUILD_QUICK_SHOW
           I3DDMA_DolbyVision_HDMI_DeInit();
#endif
	       bAlloc = true;
	}
	//getsize[2] means get memory size from cma.
	//If buf[2] borrow memory from vbm getsize[2] = 0
	//If buf[2] allocate memory from cma getsize[2] = video_size = _4K_2K_20BIT_SIZE
	if((i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].getsize < _4K_2K_20BIT_SIZE) &&
		(drvif_i3ddma_triplebuf_by_timing_protect_panel() || get_scaler_qms_mode_flag()) &&
		(size_video >= _4K_2K_20BIT_SIZE))
	{
		bAllocFourth = TRUE;
		rtd_pr_i3ddma_notice("alloc 4th buffer from cma for 4k pc mode %x,%x", i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].size, size_video);
	}

	if(bAlloc
#ifndef BUILD_QUICK_SHOW
        && (get_video_delay_type() == UNKNOW_VIDEO_DELAY_TYPE)
#endif
        )
    {//alloc need  disable i3ddma capture first
#ifdef CONFIG_VBM_HEAP
		unsigned long dmabuf_hndl = 0;
#endif

#ifdef BUILD_QUICK_SHOW
        unsigned long alloc_size = 0;
#endif

		disable_i3ddma_cap();

#if defined(CONFIG_SCALER_BRING_UP) && !defined(BUILD_QUICK_SHOW)
			malloccacheaddr_video_phyaddr = (unsigned long)dvr_malloc_uncached_specific(allocSize, GFP_DCU1,(void **)&i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].uncache);
			if(malloccacheaddr_video_phyaddr == (unsigned long)NULL)
			{
				rtd_pr_i3ddma_emerg("[DolbyVision] alloc idma sequence metadata memory failed[%lx]\n", malloccacheaddr_video_phyaddr);
#ifdef CONFIG_ENABLE_DOLBY_VISION_HDMI_AUTO_DETECT
				set_metadata_memory_available(false);
#endif
				return 1; //malloc memory fail
			}
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].cache= (void*)malloccacheaddr_video_phyaddr;
			malloccacheaddr_video_phyaddr=(unsigned int)dvr_to_phys((void*)malloccacheaddr_video_phyaddr);
#else
#ifndef BUILD_QUICK_SHOW
        rtd_pr_i3ddma_debug("[DolbyVision][%s] call VBM_BorrowMemChunk\n",__FUNCTION__);
		// buffer[0][2] -- video capture buffer
#ifdef CONFIG_VBM_HEAP
		if (VBM_BorrowMemChunk(&dmabuf_hndl, (unsigned long *)(&malloccacheaddr_video_phyaddr), (unsigned long *)(&getSize), allocSize,0) != 0) {
#else
		if (VBM_BorrowMemChunk((unsigned long *)(&malloccacheaddr_video_phyaddr), (unsigned long *)(&getSize), allocSize,0) != 0) {
#endif

            rtd_pr_i3ddma_emerg("[i3ddma] alloc idma video sequence memory from VBM failed[%lx],alloc from CMA\n",  malloccacheaddr_video_phyaddr);

            malloccacheaddr_video_phyaddr = (unsigned long)pli_malloc(allocSize, GFP_DCU2_FIRST);
            if(malloccacheaddr_video_phyaddr == (unsigned long)NULL){
                rtd_pr_i3ddma_emerg("[i3ddma] alloc idma sequence memory from CMA failed[%lx],return!!!\n", malloccacheaddr_video_phyaddr);
#ifdef CONFIG_ENABLE_DOLBY_VISION_HDMI_AUTO_DETECT
			set_metadata_memory_available(false);
#endif
                return 1; //malloc memory fail
            }
            i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].cache= (void*)malloccacheaddr_video_phyaddr;
            i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].uncache= (void*)malloccacheaddr_video_phyaddr;
            //malloccacheaddr_video_phyaddr=(unsigned int)dvr_to_phys((void*)malloccacheaddr_video_phyaddr);
            getSize = allocSize;
            i3ddma_alloc_memory_cma_flag = 1;
		}
#else
        //malloccacheaddr_video_phyaddr = get_query_start_address(QUERY_IDX_I3DDMA);
        malloccacheaddr_video_phyaddr = qs_get_memory_from_vbm(MDOMAIN_MAIN, allocSize);
        if(malloccacheaddr_video_phyaddr == 0)
        {
            printf2("[%s %d]qs get memory failed\n", __FUNCTION__, __LINE__);
        }
#endif
#endif
		//merlin4 memory do 96 bytes align @Crixus 20180321
		#ifdef CONFIG_ARM64
		query_addr = (unsigned long)malloccacheaddr_video_phyaddr;
		//size_video = dvr_size_alignment(size_video);
		align_addr = dvr_memory_alignment((unsigned long)query_addr, size_video);
		if (align_addr == (unsigned long)NULL){
#ifdef BUILD_QUICK_SHOW
			printf2("[%s %d]align_addr is null\n", __FUNCTION__, __LINE__);
#endif
			BUG();
		}
		#else
		query_addr = (unsigned int)malloccacheaddr_video_phyaddr;
		//size_video = dvr_size_alignment(size_video);
		align_addr = dvr_memory_alignment((unsigned long)query_addr, size_video);
		if (align_addr == (unsigned int)NULL){
#ifdef BUILD_QUICK_SHOW
			printf2("[%s %d]align_addr is null\n", __FUNCTION__, __LINE__);
#endif
			BUG();
		}
		rtd_pr_i3ddma_notice("query_addr=%x, align_addr=%x, size_video=%x, \n", query_addr, align_addr, size_video);
		#endif
		rtd_pr_i3ddma_notice("[VBM] allocSize:%d, getSize:%ld w:%d h:%d\n", allocSize, getSize, cur_act_w, cur_act_h);
		//i3ddmaCtrl.cap_buffer[0].cache= NULL;//(void*)(malloccacheaddr_video);
		i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr =  align_addr;//(unsigned int)dvr_to_phys((void*)i3ddmaCtrl.cap_buffer[0].cache);
		i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr_unalign = (unsigned int)malloccacheaddr_video_phyaddr;//save un align address for free memory
		i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize = getSize;
#ifdef CONFIG_VBM_HEAP
		i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].dmabuf_hndl = dmabuf_hndl;
#endif

#ifdef CONFIG_RTK_KDRV_SVP_HDMI_PROTECTION
		//Enable I3DDMA buffer protect
		if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI  && I3DDMA_Hdmi_Is_Need_Protection()) {
			rtd_pr_i3ddma_warn("@@enable_hdmi_protect,%s,%d",__FUNCTION__, __LINE__);
			scaler_svp_enable_hdmi_protection(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr_unalign ,i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize);
            i3ddma_is_protected_flag = true;
		}
#endif
    }
	if(m_domain_borrow_case)
	{
		if(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr)
		{
    		i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].size = size_video;
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_DOLBY_VISION_HDMI_AUTO_DETECT
			set_metadata_memory_available(true);
#endif
#endif
			return 0;
		}
	}

#ifndef BUILD_QUICK_SHOW
     if (get_video_delay_type() == I3DDMA_VIDEO_DELAY){
        I3DDMA_Send_Vo_SwBufNum_share_memory(0);
        set_i3ddma_video_delay_total_bufnum(get_scaler_video_delay_number());
        rtd_pr_i3ddma_notice("I3DDMA_DolbyVision_HDMI_Init i3 delay buffer=%d, total=%d\n", I3DDMA_SwGetBufNum(), get_i3ddma_video_delay_total_bufnum());
        set_i3ddma_vdelay_cap_buffer();
        I3DDMA_SwSetBufBoundary();
    } else if(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr)
#endif
    {
		// buffer[0][2] -- video capture buffer
		//i3ddmaCtrl.cap_buffer[0].cache= NULL;//(void*)(malloccacheaddr_video);
		//i3ddmaCtrl.cap_buffer[0].phyaddr =  (unsigned int)malloccacheaddr_video_phyaddr;//(unsigned int)dvr_to_phys((void*)i3ddmaCtrl.cap_buffer[0].cache);
		i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].size = size_video;
		//i3ddmaCtrl.cap_buffer[0].getsize = getSize;
		rtd_pr_i3ddma_debug("[DolbyVision][Video] buf[0]=%lx(%d KB), phy(%lx) \n", (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].cache, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].size >> 10, (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr);

		i3ddmaCtrl[I3DDMA_DEV].cap_buffer[1].cache= NULL;//(void*)(malloccacheaddr_video + size_video);
		i3ddmaCtrl[I3DDMA_DEV].cap_buffer[1].phyaddr = drvif_memory_get_data_align((unsigned int)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr + size_video, DMA_SPEEDUP_ALIGN_VALUE); //(unsigned int)dvr_to_phys((void*)i3ddmaCtrl.cap_buffer[2].cache);
		i3ddmaCtrl[I3DDMA_DEV].cap_buffer[1].size = size_video;
		//i3ddmaCtrl.cap_buffer[2].getsize = getSize;
		rtd_pr_i3ddma_debug("[DolbyVision][Video] buf[2]=%lx(%d KB), phy(%lx) \n", (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].cache, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].size >> 10, (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].phyaddr);
#ifndef BUILD_QUICK_SHOW

		if(bAllocFourth)
		{
			rtd_pr_i3ddma_notice("[i3ddma] special panel alloc idma memory from cma COMPRESSION_20_BITS!!!\n");
			//query_addr = get_query_start_address(QUERY_IDX_MDOMAIN_MAIN);
			malloccacheaddr_video_phyaddr = (unsigned long)dvr_malloc_uncached_specific(size_video, GFP_DCU2_FIRST,(void **)&i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].uncache);
			if(malloccacheaddr_video_phyaddr == (unsigned long)NULL)
			{
				I3DDMA_Set_UHD_PCmode_Mem_Alloc_Fail_flag(TRUE);
				rtd_pr_i3ddma_emerg("[i3ddma] alloc idma sequence memory from CMA failed[%lx],set comp bits=12 alloc fail flag=%d\n", malloccacheaddr_video_phyaddr, i3ddma_pcmode_alloc_memory_flag);
#ifdef CONFIG_ENABLE_DOLBY_VISION_HDMI_AUTO_DETECT
				set_metadata_memory_available(false);
#endif
				return 1;
			}

			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].cache= (void*)malloccacheaddr_video_phyaddr;
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].phyaddr = (unsigned int)dvr_to_phys((void*)malloccacheaddr_video_phyaddr);
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].size = size_video;
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].getsize = size_video;
			rtd_pr_i3ddma_notice("[i3ddma][Video] buf[2]=%lx(%d KB), phy(%lx) \n", (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].cache, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].size >> 10, (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].phyaddr);
			//I3DDMA_Set_BorrowMdomainMemory_Flag(TRUE);
		}
		else
#endif
            if(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].phyaddr && (drvif_i3ddma_triplebuf_flag() || drvif_i3ddma_triplebuf_by_timing_protect_panel() || get_scaler_qms_mode_flag()) && (i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].getsize != 0))
		{
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].size = size_video;
			//i3ddmaCtrl.cap_buffer[4].getsize = getSize;
			rtd_pr_i3ddma_notice("[DolbyVision][Video] re-use cma buf[2]=%lx(%d KB), phy(%lx) \n", (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].cache, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].size >> 10, (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].phyaddr);
		}
		else if(drvif_i3ddma_triplebuf_flag() || drvif_i3ddma_triplebuf_by_timing_protect_panel() || get_scaler_qms_mode_flag())
		{
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].cache= NULL;//(void*)(malloccacheaddr_video + size_video);
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].phyaddr = drvif_memory_get_data_align((unsigned int)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr + size_video*2, DMA_SPEEDUP_ALIGN_VALUE); //(unsigned int)dvr_to_phys((void*)i3ddmaCtrl.cap_buffer[2].cache);
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].size = size_video;
			//i3ddmaCtrl.cap_buffer[4].getsize = getSize;
			rtd_pr_i3ddma_notice("[DolbyVision][Video] buf[2]=%lx(%d KB), phy(%lx) \n", (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].cache, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].size >> 10, (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].phyaddr);
		}
		//i3ddma protection

		IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_reg, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr); //cap0 1st low limit
		IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_reg, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr + size_video); //cap0 1st up limit
		IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_1_reg, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[1].phyaddr); //cap0 2nd low limit
		IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_1_reg, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[1].phyaddr + size_video); //cap0 2nd up limit
		IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_3_reg, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr); //cap0 4th low limit
		IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_3_reg, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr + size_video); //cap0 4th up limit
		if(drvif_i3ddma_triplebuf_flag() || drvif_i3ddma_triplebuf_by_timing_protect_panel() || get_scaler_qms_mode_flag())
		{
			IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_2_reg, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].phyaddr); //cap0 3nd low limit
			IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_2_reg, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].phyaddr + size_video);//cap0 3nd high limit
		}
		else
		{
			IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_2_reg, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr); //cap0 3nd low limit
			IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_2_reg, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr + size_video);//cap0 3nd high limit
		}
#ifndef UT_flag
		if(get_scaler_qms_mode_flag())
		{// update buffer address info
			update_qms_block_info(i3ddmaCtrl[I3DDMA_DEV].cap_buffer);
		}
#endif // #ifndef UT_flag
		rtd_pr_i3ddma_debug("i3ddma cap0 boundary low=%x, high=%x\n", IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_reg), IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_reg));
		rtd_pr_i3ddma_debug("i3ddma cap1 boundary low=%x, high=%x\n", IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_1_reg), IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_1_reg));
		rtd_pr_i3ddma_debug("i3ddma cap2 boundary low=%x, high=%x\n", IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_2_reg), IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_2_reg));
    }
#ifndef BUILD_QUICK_SHOW

     if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI) && (get_HDMI_HDR_mode() == HDR_DOLBY_HDMI)) {
		//not force i3ddma case
		if (i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].cache == NULL ){
#ifdef CONFIG_ENABLE_DOLBY_VISION_HDMI_AUTO_DETECT
			reset_dolbyVision_HDMI_video_parameter();
#endif
			malloccacheaddr_metadata = (unsigned long)dvr_malloc_uncached_specific(size_metadata*2, GFP_DCU1, (void **)&i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].uncache);//(unsigned int)dvr_malloc(size_metadata*2);//for double buffer use
			if(malloccacheaddr_metadata == (unsigned long)NULL)
			{
				rtd_pr_i3ddma_emerg("[DolbyVision] alloc idma sequence metadata memory failed[%lx]\n", malloccacheaddr_metadata);
#ifdef CONFIG_ENABLE_DOLBY_VISION_HDMI_AUTO_DETECT
				set_metadata_memory_available(false);
#endif
				return 1; //malloc memory fail
			}

			// buffer[1] -- metadata capture buffer
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].cache= (void*)(malloccacheaddr_metadata);
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].phyaddr =  (unsigned int)dvr_to_phys((void*)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].cache);
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].size = size_metadata;
			rtd_pr_i3ddma_notice("[DolbyVision][Meta] buf[0]=%lx(%d KB), phy(%lx) \n", (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].cache, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].size >> 10, (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].phyaddr);

			// buffer[3] -- not used yet
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM + 1].cache= (void*)(malloccacheaddr_metadata + size_metadata);
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM + 1].phyaddr =  (unsigned int)dvr_to_phys((void*)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM + 1].cache);
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM + 1].size = size_metadata;
			rtd_pr_i3ddma_notice("[DolbyVision][Meta] buf[1]=%lx(%d KB), phy(%lx) \n", (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM + 1].cache, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM + 1].size >> 10, (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM + 1].phyaddr);
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_DOLBY_VISION_HDMI_AUTO_DETECT
			set_metadata_memory_available(true);
#endif
#endif
	    }

		//metadate boundary protection
		IoReg_Write32(H3DDMA_CAP3_CTI_DMA_WR_Rule_check_low_reg, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].phyaddr); //low limit
		IoReg_Write32(H3DDMA_CAP3_CTI_DMA_WR_Rule_check_up_reg, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM + 1].phyaddr + size_metadata);//high limit
	}
#endif
#if 0
	for (i=0; i<4; i++)
	{
		if ((i3ddmaCtrl.cap_buffer[i].cache == NULL)&&(i3ddmaCtrl.cap_buffer[i].phyaddr == NULL)) {
			rtd_pr_i3ddma_emerg("[DolbyVision] malloc HDMI buffer fail .......................................\n");
			i3ddmaCtrl.i3ddma_3d_capability = 0;
			return 1;
		}
	}
#endif

	return 0;
}
#ifndef BUILD_QUICK_SHOW

void I3DDMA_DolbyVision_buf_setting(void)
{
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg, metadataCtrl.cap_buffer[0].phyaddr);
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r0_reg, metadataCtrl.cap_buffer[1].phyaddr);
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg, metadataCtrl.cap_buffer[2].phyaddr);
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r1_reg, metadataCtrl.cap_buffer[3].phyaddr);
}

unsigned char I3DDMA_DolbyVision_MetaData_Init(void) {
	unsigned int size_metadata = 1024 * 1024; // 1MB for metadata buffer
	unsigned int size_video = 3840 * 40 * 3 ;//RGB444 8bit sequence buffer
	int i;
    int getSize = 0;
    unsigned long malloccacheaddr_video_phyaddr = 0;
	unsigned long malloccacheaddr_metadata;
#ifdef CONFIG_VBM_HEAP
	unsigned long dmabuf_hndl = 0;
#endif

	if(metadataCtrl.i3ddma_3d_capability == 1)
	{
		rtd_pr_i3ddma_emerg("metadata memory have exsit, so return!\n");
		return 0;
	}
	malloccacheaddr_metadata = (unsigned long)dvr_malloc_uncached_specific(size_metadata*2, GFP_DCU1, NULL);//(unsigned int)dvr_malloc(size_metadata*2);//for double buffer use
	//unsigned int malloccacheaddr_video = (unsigned int)dvr_malloc_uncached_specific(size_video*2, GFP_DCU2_FIRST, NULL);//(unsigned int)dvr_malloc(size_video*2);//for double buffer use

	rtd_pr_i3ddma_debug("%s.%d\n", __FUNCTION__, __LINE__);
	if(((unsigned long)NULL == malloccacheaddr_metadata)/*|| (malloccacheaddr_video == (unsigned int)NULL)*/)
	{
		rtd_pr_i3ddma_emerg("[DolbyVision] alloc idma sequence metadata memory failed[%lx]\n", malloccacheaddr_metadata);
		return 1; //malloc memory fail
	}
    rtd_pr_i3ddma_debug("[DolbyVision][%s] call VBM_BorrowMemChunk\n",__FUNCTION__);
#ifdef CONFIG_VBM_HEAP
	if (VBM_BorrowMemChunk(&dmabuf_hndl, (unsigned long *)(&malloccacheaddr_video_phyaddr), (unsigned long *)(&getSize), size_video*2, 0) != 0) {
#else
	if (VBM_BorrowMemChunk((unsigned long *)(&malloccacheaddr_video_phyaddr), (unsigned long *)(&getSize), size_video*2, 0) != 0) {
#endif
		rtd_pr_i3ddma_emerg("[DolbyVision] alloc idma video sequence memory failed[%lx]\n",  malloccacheaddr_video_phyaddr);
		return 1; //malloc memory fail
	}

	/*
	IoReg_Write32(H3DDMA_Cap_L1_Start_reg, cap_buffer[0].phyaddr);
	IoReg_Write32(H3DDMA_Cap_L2_Start_reg, cap_buffer[2].phyaddr);
	IoReg_Write32(H3DDMA_Cap_R1_Start_reg, cap_buffer[1].phyaddr);
	IoReg_Write32(H3DDMA_Cap_R2_Start_reg, cap_buffer[3].phyaddr);
	*/

	// buffer[0][2] -- video capture buffer
	metadataCtrl.cap_buffer[0].cache= NULL;//(void*)(malloccacheaddr_video);
	metadataCtrl.cap_buffer[0].phyaddr = (unsigned int)malloccacheaddr_video_phyaddr; //(unsigned int)dvr_to_phys((void*)metadataCtrl.cap_buffer[0].cache);
	metadataCtrl.cap_buffer[0].size = size_video;
	metadataCtrl.cap_buffer[0].getsize = getSize;
#ifdef CONFIG_VBM_HEAP
	metadataCtrl.cap_buffer[0].dmabuf_hndl = dmabuf_hndl;
#endif

	rtd_pr_i3ddma_debug("[DolbyVision][Video] buf[0]=%lx(%d KB), phy(%lx) \n", (unsigned long)metadataCtrl.cap_buffer[0].cache, metadataCtrl.cap_buffer[0].size >> 10, (unsigned long)metadataCtrl.cap_buffer[0].phyaddr);

	metadataCtrl.cap_buffer[2].cache= NULL;//(void*)(malloccacheaddr_video + size_video);
	metadataCtrl.cap_buffer[2].phyaddr = (unsigned int)malloccacheaddr_video_phyaddr + size_video;//(unsigned int)dvr_to_phys((void*)metadataCtrl.cap_buffer[2].cache);
	metadataCtrl.cap_buffer[2].size = size_video;
	metadataCtrl.cap_buffer[2].getsize = getSize;
	rtd_pr_i3ddma_debug("[DolbyVision][Video] buf[2]=%lx(%d KB), phy(%lx) \n", (unsigned long)metadataCtrl.cap_buffer[2].cache, metadataCtrl.cap_buffer[2].size >> 10, (unsigned long)metadataCtrl.cap_buffer[2].phyaddr);

	// buffer[1] -- metadata capture buffer
	metadataCtrl.cap_buffer[1].cache= (void*)(malloccacheaddr_metadata);
	metadataCtrl.cap_buffer[1].phyaddr =  (unsigned int)dvr_to_phys((void*)metadataCtrl.cap_buffer[1].cache);
	metadataCtrl.cap_buffer[1].size = size_metadata;
	rtd_pr_i3ddma_debug("[DolbyVision][Meta] buf[1]=%lx(%d KB), phy(%lx) \n", (unsigned long)metadataCtrl.cap_buffer[1].cache, metadataCtrl.cap_buffer[1].size >> 10, (unsigned long)metadataCtrl.cap_buffer[1].phyaddr);

	// buffer[3] -- not used yet
	metadataCtrl.cap_buffer[3].cache= (void*)(malloccacheaddr_metadata + size_metadata);
	metadataCtrl.cap_buffer[3].phyaddr =  (unsigned int)dvr_to_phys((void*)metadataCtrl.cap_buffer[3].cache);
	metadataCtrl.cap_buffer[3].size = size_metadata;
	rtd_pr_i3ddma_debug("[DolbyVision][Meta] buf[3]=%lx(%d KB), phy(%lx) \n", (unsigned long)metadataCtrl.cap_buffer[3].cache, metadataCtrl.cap_buffer[3].size >> 10, (unsigned long)metadataCtrl.cap_buffer[3].phyaddr);

	metadataCtrl.i3ddma_3d_capability = 1;
	for (i=0; i<I3DDMA_CAP_BUF_NUM; i++)
	{
		if ((metadataCtrl.cap_buffer[i].cache == NULL)&&(metadataCtrl.cap_buffer[i].phyaddr == 0)) {
			rtd_pr_i3ddma_emerg("[DolbyVision] malloc HDMI buffer fail .......................................\n");
			metadataCtrl.i3ddma_3d_capability = 0;
			return 1;
		}
	}


	return 0;
}

unsigned char I3DDMA_DolbyVision_MetaData_DeInit(void){
	int i;
	if ((metadataCtrl.cap_buffer[0].cache)||(metadataCtrl.cap_buffer[0].phyaddr))
	{
		//dvr_free(metadataCtrl.cap_buffer[0].cache);
		if(metadataCtrl.cap_buffer[0].phyaddr)
		{
		    rtd_pr_i3ddma_debug("[DolbyVision][%s] call VBM_ReturnMemChunk\n",__FUNCTION__);
#ifdef CONFIG_VBM_HEAP
			VBM_ReturnMemChunk(metadataCtrl.cap_buffer[0].dmabuf_hndl, metadataCtrl.cap_buffer[0].phyaddr,metadataCtrl.cap_buffer[0].getsize);
#else
			VBM_ReturnMemChunk(metadataCtrl.cap_buffer[0].phyaddr,metadataCtrl.cap_buffer[0].getsize);
#endif
		}
	}

	if (metadataCtrl.cap_buffer[1].cache)
	{
		dvr_free(metadataCtrl.cap_buffer[1].cache);
	}

	for (i=0; i<I3DDMA_CAP_BUF_NUM; i++) {
		if ((metadataCtrl.cap_buffer[i].cache)||(metadataCtrl.cap_buffer[i].phyaddr)) {
			metadataCtrl.cap_buffer[i].cache = NULL;
			metadataCtrl.cap_buffer[i].phyaddr = (unsigned long)NULL;
			metadataCtrl.cap_buffer[i].size = 0;
			metadataCtrl.cap_buffer[i].getsize = 0;
#ifdef CONFIG_VBM_HEAP
			metadataCtrl.cap_buffer[i].dmabuf_hndl = 0;
#endif

		}
	}
	metadataCtrl.i3ddma_3d_capability= 0;
	rtd_pr_i3ddma_debug("[DolbyVision] Deinit done!\n");

	return 0;
}


#endif


#ifndef BUILD_QUICK_SHOW

void I3DDMA_DolbyVision_HDMI_DeInit(void) {
	int i;
#ifdef CONFIG_ENABLE_DOLBY_VISION_HDMI_AUTO_DETECT
	set_metadata_memory_available(false);
	reset_dolbyVision_HDMI_video_parameter();
#endif
	if(i3ddma_alloc_memory_cma_flag && i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].cache)
	{
#ifdef CONFIG_RTK_KDRV_SVP_HDMI_PROTECTION
		//Disable I3DDMA buffer protect
		if(i3ddma_is_protected_flag){
			rtd_pr_i3ddma_warn("@@disable_hdmi_protect,%s,%d",__FUNCTION__, __LINE__);
			scaler_svp_disable_hdmi_protection(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr_unalign,i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize);
            i3ddma_is_protected_flag = false;
		}
#endif
		pli_free((unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].cache);
		i3ddma_alloc_memory_cma_flag = 0;
	}
	else if((i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].cache)|| (i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr))
	{
		//dvr_free(i3ddmaCtrl.cap_buffer[0].cache);
#ifdef CONFIG_RTK_KDRV_SVP_HDMI_PROTECTION
		//Disable I3DDMA buffer protect
		if(i3ddma_is_protected_flag){
			rtd_pr_i3ddma_warn("@@disable_hdmi_protect,%s,%d",__FUNCTION__, __LINE__);
			scaler_svp_disable_hdmi_protection(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr_unalign,i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize);
            i3ddma_is_protected_flag = false;
		}
#endif
		if(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr)
		{
		    rtd_pr_i3ddma_debug("[DolbyVision][%s] call VBM_ReturnMemChunk\n",__FUNCTION__);
#ifdef CONFIG_VBM_HEAP
			VBM_ReturnMemChunk(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].dmabuf_hndl, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr_unalign, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize);
#else
			VBM_ReturnMemChunk(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr_unalign, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize);
#endif
			rtd_pr_i3ddma_info("[%s] call VBM_ReturnMemChunk finish\n",__FUNCTION__);
		}
	}

	if (i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].cache)
	{
		rtd_pr_i3ddma_info("[%s] dvr_free cap_buffer[%d].cache start\n",__FUNCTION__, I3DDMA_MAIN_CAP_BUF_NUM);
		dvr_free(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].cache);
		rtd_pr_i3ddma_info("[%s] dvr_free cap_buffer[%d].cache finish\n",__FUNCTION__, I3DDMA_MAIN_CAP_BUF_NUM);
	}

	if(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].cache)
	{
		dvr_free(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].cache);
		rtd_pr_i3ddma_info("[%s] free cma memory buf3=%lx,%lx\n", __FUNCTION__, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].phyaddr, (unsigned long)i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].cache);
	}

	if(I3DDMA_Get_UHD_PCmode_Mem_Alloc_Fail_flag())
		I3DDMA_Set_UHD_PCmode_Mem_Alloc_Fail_flag(FALSE);


	for (i=0; i<I3DDMA_CAP_BUF_NUM; i++) {
		if ((i3ddmaCtrl[I3DDMA_DEV].cap_buffer[i].cache)||(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[i].phyaddr)) {
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[i].cache = NULL;
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[i].phyaddr = (unsigned long)NULL;
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[i].phyaddr_unalign = (unsigned long)NULL;
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[i].size = 0;
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[i].getsize = 0;//test
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[i].uncache = 0;
#ifdef CONFIG_VBM_HEAP
			i3ddmaCtrl[I3DDMA_DEV].cap_buffer[i].dmabuf_hndl = 0;
#endif

		}
	}

	i3ddmaCtrl[I3DDMA_DEV].i3ddma_3d_capability= 0;
	rtd_pr_i3ddma_info("[DolbyVision] Deinit done!\n");
}

unsigned char video_delay_alloc_memory(unsigned int video_size)
{
    unsigned long  vir_addr = 0;
    unsigned long  vir_addr_noncache = 0;
    unsigned long  malloccacheaddr_video_phyaddr = 0;
    int            i = 0;
    int            j = 0;
    unsigned long  getSize = 0;
    unsigned char  unNum = 0 ;
    unsigned int   unVideoSize = 0;
    unsigned char alloc_num = 0;
#ifdef CONFIG_VBM_HEAP
    unsigned long dmabuf_hndl = 0;
#endif

    unNum = SW_I3DDMA_CAP_NUM;

    alloc_num = unNum / 4 + (unNum % 4 != 0);

    unVideoSize = video_size;
    rtd_pr_i3ddma_info("[%s, %d] unVideoSize=%d KB\n", __FUNCTION__, __LINE__, unVideoSize >> 10);

    if(s_i3ddma_sw_cap_buffer[0].phyaddr && (s_i3ddma_sw_cap_buffer[0].size == video_size))
    {
        rtd_pr_i3ddma_info("[VideoDelay] buffer memory alloc is ready!\n");
        return TRUE;
    }

    if ((i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr) && (s_i3ddma_sw_cap_buffer[0].phyaddr == 0))
    {
        disable_i3ddma_cap();
        drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);
        drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_MAIN_DISPLAY, _DISABLE);
        rtd_pr_i3ddma_info("[VideoDelay] free memory i3ddmaCtrl before alloc!\n");
#ifdef CONFIG_RTK_KDRV_SVP_HDMI_PROTECTION
        //Disable I3DDMA buffer protect
        if(i3ddma_is_protected_flag){
            rtd_pr_i3ddma_warn("@@disable_hdmi_protect,%s,%d",__FUNCTION__, __LINE__);
            scaler_svp_disable_hdmi_protection(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr_unalign,i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize);
            i3ddma_is_protected_flag = false;
        }
#endif

#if 1
#ifdef CONFIG_VBM_HEAP
        VBM_ReturnMemChunk(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].dmabuf_hndl, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr_unalign, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize);
#else
        VBM_ReturnMemChunk(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr_unalign, i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize);
#endif
        memset(&(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0]), 0, sizeof(I3DDMA_CAPTURE_BUFFER_T));
#endif
        //I3DDMA_DolbyVision_HDMI_DeInit();
    }

    rtd_pr_i3ddma_info("[VideoDelay] try to alloc 4-buffer %d times\n", alloc_num);

    for(i = 0; i < alloc_num; i++)
    {
        getSize = 0;
#ifdef CONFIG_VBM_HEAP
        if (VBM_BorrowMemChunk(&dmabuf_hndl, (unsigned long *)(&malloccacheaddr_video_phyaddr), &getSize, unVideoSize * 4, 0) != 0)
#else
        if (VBM_BorrowMemChunk((unsigned long *)(&malloccacheaddr_video_phyaddr), &getSize, unVideoSize * 4, 0) != 0)
#endif
        {
            rtd_pr_i3ddma_info("[VideoDelay][I3DDMA][%s %d] VBM can not alloc memory [unVideoSize:0x%x]\n", __FUNCTION__, __LINE__, unVideoSize);
            break;
        }

#ifdef CONFIG_VBM_HEAP
        s_i3ddma_sw_cap_buffer[i].dmabuf_hndl = dmabuf_hndl;
#endif
        if (malloccacheaddr_video_phyaddr)
        {
            for(j = 0; j < 4; j ++)
            {
                s_i3ddma_sw_cap_buffer[i * 4 + j].cache = NULL;
                s_i3ddma_sw_cap_buffer[i * 4 + j].phyaddr = malloccacheaddr_video_phyaddr + unVideoSize * j;
                s_i3ddma_sw_cap_buffer[i * 4 + j].size = unVideoSize;
                s_i3ddma_sw_cap_buffer[i * 4 + j].getsize = (j == 0) ? getSize : 0;
                s_i3ddma_sw_cap_buffer[i * 4 + j].uncache = (j == 0) ? (unsigned char *)(malloccacheaddr_video_phyaddr) : NULL;
                rtd_pr_i3ddma_info("[VideoDelay][I3DDMA][%s %d][4-buffer]vbm s_i3ddma_sw_cap_buffer[%d].phyaddr:0x%lx, size:%d \n", __FUNCTION__, __LINE__, i * 4 + j, s_i3ddma_sw_cap_buffer[i * 4 + j].phyaddr, unVideoSize);
            }
        }

    }

    if(i != alloc_num){
        rtd_pr_i3ddma_info("[VideoDelay] try to alloc by 1-buffer i = %d\n", i);
        for(i = i * 4; i < unNum; i++)
        {
            malloccacheaddr_video_phyaddr = 0;
#ifdef CONFIG_VBM_HEAP
            if (VBM_BorrowMemChunk(&dmabuf_hndl, (unsigned long *)(&malloccacheaddr_video_phyaddr), &getSize, unVideoSize, 0) != 0)
#else
            if (VBM_BorrowMemChunk((unsigned long *)(&malloccacheaddr_video_phyaddr), &getSize, unVideoSize, 0) != 0)
#endif
            {

            }

#ifdef CONFIG_VBM_HEAP
            s_i3ddma_sw_cap_buffer[i].dmabuf_hndl = dmabuf_hndl;
#endif

            if (malloccacheaddr_video_phyaddr)
            {
                s_i3ddma_sw_cap_buffer[i].cache = NULL;
                s_i3ddma_sw_cap_buffer[i].phyaddr = malloccacheaddr_video_phyaddr;
                s_i3ddma_sw_cap_buffer[i].size = unVideoSize;
                s_i3ddma_sw_cap_buffer[i].getsize = getSize;
                s_i3ddma_sw_cap_buffer[i].uncache = (unsigned char *)(malloccacheaddr_video_phyaddr);

                rtd_pr_i3ddma_info("[VideoDelay][I3DDMA][%s %d][1-buffer]vbm s_i3ddma_sw_cap_buffer[%d].phyaddr:0x%lx, size:%d \n", __FUNCTION__, __LINE__, i, s_i3ddma_sw_cap_buffer[i].phyaddr, unVideoSize);
            } else
            { // allocate cma
                vir_addr = (unsigned long)dvr_malloc_uncached_specific(unVideoSize, GFP_DCU2_FIRST, (void **)&vir_addr_noncache);

                if(vir_addr == 0){
                    rtd_pr_i3ddma_info("[VideoDelay][I3DDMA][%s %d] alloc idma dymanic memory failed from cma\n", __FUNCTION__, __LINE__);
                    break;
                }

                s_i3ddma_sw_cap_buffer[i].cache = (void *) vir_addr;

                malloccacheaddr_video_phyaddr = (unsigned long)dvr_to_phys((void*)vir_addr);
                s_i3ddma_sw_cap_buffer[i].phyaddr = malloccacheaddr_video_phyaddr;
                s_i3ddma_sw_cap_buffer[i].size = unVideoSize;
                s_i3ddma_sw_cap_buffer[i].getsize = unVideoSize;
                s_i3ddma_sw_cap_buffer[i].uncache = NULL;
                rtd_pr_i3ddma_info("[VideoDelay][I3DDMA][%s %d]cma s_i3ddma_sw_cap_buffer[%d].phyaddr: 0x%lx, size:%d \n", __FUNCTION__, __LINE__,i, malloccacheaddr_video_phyaddr,unVideoSize);
            }
        }

#ifdef CONFIG_RTK_KDRV_SVP_HDMI_PROTECTION
        //Enable I3DDMA buffer protect
        if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI  && I3DDMA_Hdmi_Is_Need_Protection()) {
            rtd_pr_i3ddma_warn("@@enable_hdmi_protect,%s,%d",__FUNCTION__, __LINE__);
            scaler_svp_enable_hdmi_protection(s_i3ddma_sw_cap_buffer[0].phyaddr , s_i3ddma_sw_cap_buffer[0].getsize);
            i3ddma_is_protected_flag = true;
        }
#endif

        if(i < unNum)
        {
            rtd_pr_i3ddma_info("[VideoDelay][I3DDMA][%s %d] Error!! alloc sw idma cap buffer(%d) is not not enough, need unNum:%d \n", __FUNCTION__, __LINE__, i, unNum);
            //unNum = i;
            //I3DDMA_SwSetBufNum(unNum);
            return FALSE;
        }
    }

    return TRUE;
}

void video_delay_free_memory(void)
{
    int i = 0;

#ifdef CONFIG_RTK_KDRV_SVP_HDMI_PROTECTION
    //Disable I3DDMA buffer protect
    if(i3ddma_is_protected_flag){
        rtd_pr_i3ddma_warn("@@disable_hdmi_protect,%s,%d",__FUNCTION__, __LINE__);
        scaler_svp_disable_hdmi_protection(s_i3ddma_sw_cap_buffer[0].phyaddr, s_i3ddma_sw_cap_buffer[0].getsize);
        i3ddma_is_protected_flag = false;
    }
#endif

    for(i = 0; i < SW_I3DDMA_CAP_NUM; i++)
    {
        if (s_i3ddma_sw_cap_buffer[i].phyaddr)
        {
            if(s_i3ddma_sw_cap_buffer[i].cache == NULL)
            {//VBM
                if(s_i3ddma_sw_cap_buffer[i].uncache != NULL)
                {
#ifdef CONFIG_VBM_HEAP
                    if(VBM_ReturnMemChunk(s_i3ddma_sw_cap_buffer[i].dmabuf_hndl, s_i3ddma_sw_cap_buffer[i].phyaddr, s_i3ddma_sw_cap_buffer[i].getsize) != 0)
#else
                    if(VBM_ReturnMemChunk(s_i3ddma_sw_cap_buffer[i].phyaddr, s_i3ddma_sw_cap_buffer[i].getsize) != 0)
#endif
                    {
                        rtd_pr_i3ddma_info("[%s %d]free buffer %d VBM_ReturnMemChunk fail fail\n",__FUNCTION__, __LINE__,i);
                    } else {
                        rtd_pr_i3ddma_info("[%s %d]free buffer %d VBM_ReturnMemChunk ok ok\n", __FUNCTION__, __LINE__,i);
                    }
                }
            }
            else
            {//cma
                dvr_free(s_i3ddma_sw_cap_buffer[i].cache);
                rtd_pr_i3ddma_info("[%s %d]call dvr_free, buffer %d\n",__FUNCTION__, __LINE__,i);
            }
            s_i3ddma_sw_cap_buffer[i].cache = NULL;
            s_i3ddma_sw_cap_buffer[i].uncache = NULL;
            s_i3ddma_sw_cap_buffer[i].phyaddr = (unsigned long)NULL;
            s_i3ddma_sw_cap_buffer[i].phyaddr_unalign = (unsigned long)NULL;
            s_i3ddma_sw_cap_buffer[i].size = 0;
            s_i3ddma_sw_cap_buffer[i].getsize = 0;

#ifdef CONFIG_VBM_HEAP
            s_i3ddma_sw_cap_buffer[i].dmabuf_hndl = 0;
#endif
        }
    }

    memset(&(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0]), 0, sizeof(I3DDMA_CAPTURE_BUFFER_T));
    memset(&(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[1]), 0, sizeof(I3DDMA_CAPTURE_BUFFER_T));
    if (drvif_i3ddma_triplebuf_flag() || drvif_i3ddma_triplebuf_by_timing_protect_panel() || get_scaler_qms_mode_flag())
        memset(&(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2]), 0, sizeof(I3DDMA_CAPTURE_BUFFER_T));

}

void set_i3ddma_vdelay_cap_buffer(void)
{
    int i;
    if(I3DDMA_SwGetBufNum() == 0)
        return;

    memcpy(&(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0]), &(s_i3ddma_sw_cap_buffer[0]), sizeof(I3DDMA_CAPTURE_BUFFER_T));
    memcpy(&(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[1]), &(s_i3ddma_sw_cap_buffer[1]), sizeof(I3DDMA_CAPTURE_BUFFER_T));
    if (drvif_i3ddma_triplebuf_flag() || drvif_i3ddma_triplebuf_by_timing_protect_panel() || get_scaler_qms_mode_flag())
    memcpy(&(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2]), &(s_i3ddma_sw_cap_buffer[2]), sizeof(I3DDMA_CAPTURE_BUFFER_T));

    for (i = 0; i < get_i3ddma_video_delay_total_bufnum(); i++){
        i3ddma_vdelay_cap_buffer[i].phyaddr = s_i3ddma_sw_cap_buffer[i].phyaddr;
        rtd_pr_i3ddma_info("[VideoDelay] buffer[%d]=%lx\n", i, i3ddma_vdelay_cap_buffer[i].phyaddr);
    }

}

void disable_i3ddma_video_delay(void)
{
    if(s_i3ddma_sw_cap_buffer[0].phyaddr)
    {
        I3DDMA_Setup3DDMA(&i3ddmaCtrl[I3DDMA_DEV], I3DDMA_3D_OPMODE_DISABLE, I3DDMA_DEV);
        msleep(60);
        video_delay_free_memory();
    }
}

unsigned char I3DDMA_SwGetBufNum(void)
{
    rtd_pr_i3ddma_debug("[I3DDMA][%s, %d]i3ddma sw buffer get s_i3ddma_sw_delay_num : %d\n", __FUNCTION__, __LINE__, s_i3ddma_sw_delay_num);

    return s_i3ddma_sw_delay_num;
}

void I3DDMA_SwSetBufNum(unsigned char ucNum)
{
    if(ucNum > SW_I3DDMA_CAP_NUM)
    {
        s_i3ddma_sw_delay_num = SW_I3DDMA_CAP_NUM;
    }
    else
    {
        s_i3ddma_sw_delay_num = ucNum;
    }

    rtd_pr_i3ddma_info("[I3DDMA][%s, %d]i3ddma sw buffer set s_i3ddma_sw_delay_num : %d\n", __FUNCTION__, __LINE__, s_i3ddma_sw_delay_num);
}

void I3DDMA_SwSetBufBoundary(void)
{
    unsigned long maxAddr = 0;
    unsigned long minAddr = 0;

    unsigned char ucNum = 0;
    unsigned int unLoop = 0;

    ucNum = get_i3ddma_video_delay_total_bufnum();
    minAddr = s_i3ddma_sw_cap_buffer[0].phyaddr;
    maxAddr = s_i3ddma_sw_cap_buffer[0].phyaddr + s_i3ddma_sw_cap_buffer[0].size;

    for(unLoop = 0; unLoop < ucNum; unLoop++)
    {
        if(s_i3ddma_sw_cap_buffer[unLoop].phyaddr < minAddr)
        {
            minAddr = s_i3ddma_sw_cap_buffer[unLoop].phyaddr;
        }
        if(s_i3ddma_sw_cap_buffer[unLoop].phyaddr + s_i3ddma_sw_cap_buffer[unLoop].size > maxAddr)
        {
            maxAddr = s_i3ddma_sw_cap_buffer[unLoop].phyaddr + s_i3ddma_sw_cap_buffer[unLoop].size;
        }
    }

    IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_reg, minAddr); //cap0 low limit
    IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_reg, maxAddr); //cap0 up limit
    IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_1_reg, minAddr); //cap0 low limit
    IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_1_reg, maxAddr); //cap0 up limit

    rtd_pr_i3ddma_info("[I3DDMA][%s, %d]videodelay set boundary minAddr:0x%lx, maxAddr:0x%lx\n", __FUNCTION__, __LINE__, minAddr, maxAddr);

}

void I3DDMA_Send_Vo_SwBufNum_share_memory(unsigned char ucNum)
{
    unsigned int *bufnub = NULL;

    bufnub = (unsigned int *)Scaler_GetShareMemVirAddr(SCALERIOC_SET_I3DDMA_SW_BUF_BUM);
    if(bufnub == NULL)
    {
        rtd_pr_i3ddma_err("[I3DDMA][%s, %d]point is NULL\n",__FUNCTION__,__LINE__);
        return;
    }

    rtd_pr_i3ddma_info("[I3DDMA][%s, %d]share memory video delay num:%d \n", __FUNCTION__, __LINE__, ucNum);

    *bufnub = Scaler_ChangeUINT32Endian(ucNum);
}

void I3DDMA_SetCap0LastWriteFlag(unsigned int flag)
{
	unsigned int *i3ddmaCap0LastWriteFlag = NULL;

	i3ddmaCap0LastWriteFlag = (unsigned int *)Scaler_GetShareMemVirAddr(SCALERIOC_I3DDMA_CAP0_LAST_WRITE_FLAG);
	if(i3ddmaCap0LastWriteFlag == NULL)
	{
		rtd_pr_i3ddma_info("[warning]point is NULL @ func:%s, line:%d\n",__FUNCTION__,__LINE__);
		return;
	}

	*i3ddmaCap0LastWriteFlag = Scaler_ChangeUINT32Endian(flag);
}

unsigned int I3DDMA_Get_VO_SWBufIndex(void)
{
	unsigned int *vo_sw_buf_idx = NULL;
	unsigned int temp = 0;

	vo_sw_buf_idx = (unsigned int *)Scaler_GetShareMemVirAddr(SCALERIOC_I3DDMA_CAP0_LAST_WRITE_FLAG);

	temp = htonl(*vo_sw_buf_idx);

	return temp;
}

void set_i3ddma_video_delay_total_bufnum(unsigned int num)
{
    if(drvif_i3ddma_triplebuf_flag())
        s_i3ddma_vdelay_numbers = (3 + num);
    else
        s_i3ddma_vdelay_numbers =  (2 + num);
}

unsigned int get_i3ddma_video_delay_total_bufnum(void)
{
    return s_i3ddma_vdelay_numbers;
}

#endif

//[ECN][RL6543-17]
#define QMS_IV2PVDELAY_PERCENT (100)
#define VODMA_VSTART_PREREAD_LINE 5

extern unsigned int VODMA_get_datafs_increased_clock(unsigned int orig_ratio_n_off);
unsigned char drv_I3ddmaVodma_GameMode_iv2pv_delay(unsigned char enable){

	UINT32 id2pv_delay_1 = 0, id2pv_delay_2 = 0;
//	UINT32 timeout;
#ifdef BUILD_QUICK_SHOW
	vodma_vodma_pvs0_gen_RBUS vodma_vodma_pvs0_gen_Reg;
#endif
	UINT32 voHtotal, voVactive, voVstart, VoClock, voPreRead;
	unsigned long long VoClock_final;
	UINT32 i3ddmaVtotal, i3ddmaVstart, i3ddmaVactive, i3ddmaVfreq;
	unsigned long long temp1;
	vodma_vodma_read_start_RBUS vodma_vodma_read_start_reg;
	vodma_vodma_v1vgip_vact1_RBUS vodma_vodma_v1vgip_vact1_reg;
	unsigned char ret;
	SLR_VOINFO *pVOInfo = NULL;
	unsigned int sample_rate = 0, divider_num = 0, ratio_n_off_orig = 0, ratio_n_off = 0;
	vodma_vodma_clkgen_RBUS vodma_clkgen_reg;
	UINT32 wait_timeoutcnt = 50; /* timeout 100 ms*/

	//rtd_pr_i3ddma_info("\r\n####func:%s start line:%d####%d\r\n", __FUNCTION__, __LINE__,enable);
	//rtd_pr_i3ddma_info("[qiangzhou]SLR_INPUT_MODE_CURR = %d\n",Get_GamemodeSaveI3ddmaGetcurMode());
	/*all i3ddma source  vodma will sync i3ddma, so all i3ddma source, need setting gamemode iv2pv delay*/
	if(get_force_i3ddma_enable(SLR_MAIN_DISPLAY) == FALSE)
		return FALSE;//avoid crash
	if(get_scaler_qms_mode_flag())
	{//set fixed iv2pv delay for qms.
		voHtotal = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN);
		voVactive = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN);
		id2pv_delay_1 = (((voHtotal * voVactive) / 10) * QMS_IV2PVDELAY_PERCENT) / 10;//one frame active size delay
	}
	else if (enable == _ENABLE){
        vodma_clkgen_reg.regValue=IoReg_Read32(VODMA_VODMA_CLKGEN_reg);
		/*Modify to use formula to calculate the iv2pv delay @Crixus 20161130*/
		voHtotal = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN);
#ifndef BUILD_QUICK_SHOW
		voVactive = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN);
#else
		voVactive = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE);
#endif
		voVstart = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_STA);

		pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
		if (pVOInfo == NULL) {
			rtd_pr_i3ddma_info("[%s][WARNING] VO info is null!!!\n",__FUNCTION__);
			return FALSE;
		}

		//in small window case, vo clock is increased for frc2fs SMT,
		//so we need use vbe_get_HDMI_run_timing_framesync_condition
		//to decide hdmi clock.
#ifndef UT_flag
		if(vbe_get_HDMI_run_timing_framesync_condition())
		{//for all 4k resolution(4096/3840) but not 4k120 but not vrr but not game mdoe we always use line buffer mode vo clock to calculate iv2pv delay.
		 //WOSQRTK-15062 issue hdmi frame buffer to line buffer mode. The buffer mode error finally, because we change iv2pv delay during smooth toggle
			VoClock = VO_Get_vodmapll();
			if(VoClock == 0)
				return 2;
			divider_num = vodma_clkgen_reg.vodma_clk_div_n;
			//ratio_n_off = vodma_clkgen_reg.vodma_clk_ratio_n_off;
			sample_rate = (unsigned int)( ( ( (unsigned long long)Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN) * (unsigned long long)Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN) * (unsigned long long)Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000) ) + 9999999ULL ) / 10000000ULL );
#ifdef VODMA1_2P_MODE
			if( sample_rate > VoClock )
				sample_rate /= 2;
#endif
			ratio_n_off_orig = 64 - ((sample_rate)*64/(VoClock/10000));
			if(((VoClock/64) * (64 - ratio_n_off_orig)) < (sample_rate * 10000))
				ratio_n_off_orig -= 1;
			ratio_n_off = VODMA_get_datafs_increased_clock(ratio_n_off_orig);
			// Merlin8 : ratio_n_off first, then div_n
			VoClock = (VoClock/64) * (64 - ratio_n_off);
			VoClock = VoClock / (divider_num+1);

			if(vodma_clkgen_reg.vodma_clk_div2_en)	// interlace should div2
			{
				VoClock /= 2;
				rtd_pr_vbe_emerg("vo clk div2 for interlace timing\n");
			}
			rtd_pr_vbe_emerg("[VO][%d,%d,%d]\n", ratio_n_off_orig, ratio_n_off, divider_num);
		}
		else
		{
			VoClock = VSYNCGEN_get_vodma_pixel_clk_0(); // this is game mode, or 4k120 or vrr frame buffer mode case
		}
#ifdef VODMA1_2P_MODE
		if( vodma_clkgen_reg.vodma_2p_gate_sel == 0 )
			VoClock_final = (unsigned long long)VoClock * 2ULL;
		else
#endif
			VoClock_final = (unsigned long long)VoClock;
#endif // #ifndef UT_flag
		vodma_vodma_v1vgip_vact1_reg.regValue = IoReg_Read32(VODMA_VODMA_V1VGIP_VACT1_reg);
		voPreRead = vodma_vodma_v1vgip_vact1_reg.v_st - VODMA_IV2PV_DELAY_CAL_MARGIN;

		if(i3ddmaCtrl[I3DDMA_DEV].pgen_timing == NULL)
		{
			rtd_pr_i3ddma_emerg("[i3ddma][%s]timing is null,return\n",__FUNCTION__);
			return FALSE;
		}

		i3ddmaVtotal = i3ddmaCtrl[I3DDMA_DEV].pgen_timing->v_total;
		//i3ddmaHtotal = i3ddmaCtrl[I3DDMA_DEV].pgen_timing->h_total;
		i3ddmaVstart = i3ddmaCtrl[I3DDMA_DEV].pgen_timing->v_act_sta;
		i3ddmaVactive = i3ddmaCtrl[I3DDMA_DEV].pgen_timing->v_act_len;
		i3ddmaVfreq = i3ddmaCtrl[I3DDMA_DEV].pgen_timing->v_freq_1000;

		rtd_pr_i3ddma_info("[iv2dv_dly][%d,%d,%d,%d,%d,%d,%d,%d]",voHtotal, voVactive, voVstart, VoClock_final, i3ddmaVtotal, i3ddmaVstart, i3ddmaVactive, i3ddmaVfreq);

		vodma_vodma_read_start_reg.regValue = IoReg_Read32(VODMA_VODMA_READ_START_reg);
		vodma_vodma_read_start_reg.line_cnt = voPreRead;//VODMA_VSTART_PREREAD_LINE;
		IoReg_Write32(VODMA_VODMA_READ_START_reg, vodma_vodma_read_start_reg.regValue);

		//rtd_pr_i3ddma_emerg("[crixus]VoClock = %d, voHtotal = %d, voVstart = %d, voVactive = %d\n", VoClock, voHtotal, voVstart, voVactive);
		//rtd_pr_i3ddma_emerg("[crixus]i3ddmaVtotal = %d, i3ddmaVstart = %d, i3ddmaVactive = %d, i3ddmaVfreq = %d\n", i3ddmaVtotal, i3ddmaVstart, i3ddmaVactive, i3ddmaVfreq);
		/*
			T1: i3ddma act start
			T2: vodma act start
			N: DDR writing time
			Note: ECN for vodma_pre_read 0xb8005144[11:0]
			(before ecn : vodma pre_read = 0)

			formula:
			         T1 <= T2
			         => 1/i3ddma_clock * i3ddma_Htotal * (i3ddma_Vstart + N) <= 1/vo_clock * vo_Htotal * (iv2pv_delay+vodma pre_read)

			T1 = 1/(i3ddma_Htotal*i3ddma_Vtotal*i3ddma_freq) * i3ddma_Htotal * (i3ddma_Vstart + N)
				   = 1/(i3ddma_Vtotal*i3ddma_freq) * (i3ddma_Vstart + N)
				   = (i3ddma_Vstart + N) / (i3ddma_Vtotal*i3ddma_freq)

			T1 <= T2
			=> T1 <= 1/vo_clock * vo_Htotal * iv2pv_delay
			=> T1 * (vo_clock / vo_Htotal) <= iv2pv_delay
			=> iv2pv_delay >= T1 * (vo_clock / vo_Htotal);
		*/
		temp1 = (unsigned long long)( i3ddmaVstart + I3DDMA_DRAM_MARGIN ) * 1000000000ULL;
		temp1 = temp1 / ( (unsigned long long)i3ddmaVtotal * (unsigned long long)i3ddmaVfreq );
		//rtd_pr_i3ddma_emerg("[crixus]1.temp1 = %lld\n", temp1);

		temp1 = temp1 * VoClock_final / (unsigned long long)voHtotal;
		temp1 = temp1 / 10000ULL;	/*Modify to use VO real clock to calculate @Crixus 20161230*/
		if( ( (unsigned int)temp1 % 100 ) > 0 )
		{
			temp1 = temp1 / 100ULL;
			temp1 = temp1 + 1ULL;
		}
		else
		{
			temp1 = temp1 / 100ULL;
		}
		//rtd_pr_i3ddma_emerg("[crixus]2.temp1 = %lld\n", temp1);

		//Add below code to protect id2pv_delay sets to negative value.
		if( (unsigned int)temp1 < voPreRead )
			temp1 = (unsigned long long)voPreRead;

		id2pv_delay_1 = (unsigned int)temp1 - voPreRead;

		//rtd_pr_i3ddma_emerg("[crixus]id2pv_delay_1 line = %d\n", id2pv_delay_1);

		/*
			T3: i3ddma act end
			T4: vodma act end

			formula:
			         T3 <= T4
			         => 1/i3ddma_clock * i3ddma_Htotal * (i3ddma_Vactive + i3ddma_Vstart + N) <= 1/vo_clock * vo_Htotal * (vo_Vsatrt + vo_Vactive + iv2pv_delay)

			T3 = 1/(i3ddma_Htotal*i3ddma_Vtotal*i3ddma_freq) * i3ddma_Htotal * (i3ddma_Vactive + i3ddma_Vstart + N)
				   = 1/(i3ddma_Vtotal*i3ddma_freq) * (i3ddma_Vactive + i3ddma_Vstart + N)
				   = (i3ddma_Vactive + i3ddma_Vstart + N) / (i3ddma_Vtotal*i3ddma_freq)
			iv2pv_delay >= temp1 * (vo_clock / vo_Htotal) - (vo_Vsatrt + vo_Vactive);
		*/

		temp1 = (unsigned long long)( i3ddmaVactive + i3ddmaVstart + I3DDMA_DRAM_MARGIN ) * 1000000000ULL;
		temp1 = temp1 / ( (unsigned long long)i3ddmaVtotal * (unsigned long long)i3ddmaVfreq );
		//rtd_pr_i3ddma_emerg("[crixus]1.temp1 = %lld\n", temp1);

		temp1 = temp1 * VoClock_final / (unsigned long long)voHtotal;
		temp1 = temp1 / 10000ULL;
		if( ( (unsigned int)temp1 % 100 ) > 0 )
		{
			temp1 = temp1 / 100ULL;
			temp1 = temp1 + 1ULL;
		}
		else
		{
			temp1 = temp1 / 100ULL;
		}

		if( (unsigned int)temp1 < ( voVactive + voPreRead ) )
			temp1 = (unsigned long long)( voVactive + voPreRead );

		id2pv_delay_2 = (unsigned int)temp1 - voVactive - voPreRead;	/*Modify to use VO real clock to calculate @Crixus 20161230*/

		//rtd_pr_i3ddma_emerg("[crixus]id2pv_delay_2 line = %d\n", id2pv_delay_2);

		if(id2pv_delay_1 > id2pv_delay_2)
			id2pv_delay_1 = id2pv_delay_1 * Scaler_DispGetInputInfo(SLR_INPUT_H_LEN);
		else
			id2pv_delay_1 = id2pv_delay_2 * Scaler_DispGetInputInfo(SLR_INPUT_H_LEN);

#ifdef VODMA1_2P_MODE
			id2pv_delay_1 /= 2; // [ML8BU-1186] Only for VO 2P mode
#endif

		if(id2pv_delay_1 == 0)
			id2pv_delay_1 = 2;//default setting

		//rtd_pr_i3ddma_emerg("[crixus]id2pv_delay = 0x%x\n", id2pv_delay_1);
	}
	else{
		id2pv_delay_1 = 2;//default setting
		if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_VDEC) && (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_JPEG) && (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_CAMERA))
		{
			vodma_vodma_v1vgip_vact1_reg.regValue = IoReg_Read32(VODMA_VODMA_V1VGIP_VACT1_reg);
			voPreRead = vodma_vodma_v1vgip_vact1_reg.v_st - VODMA_IV2PV_DELAY_CAL_MARGIN;
			vodma_vodma_read_start_reg.regValue = IoReg_Read32(VODMA_VODMA_READ_START_reg);
			vodma_vodma_read_start_reg.line_cnt = voPreRead;
			IoReg_Write32(VODMA_VODMA_READ_START_reg, vodma_vodma_read_start_reg.regValue);
		}
	}

#if 0
	//AV need fine tune iv2pv delay
	if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_AVD)  && (get_AVD_Input_Source() != _SRC_TV)){
		if((i3ddmaVfreq > 595) && (i3ddmaVfreq < 605)){
			//normal mode 60Hz case, iv2pv = i3_vtotal * i3_htotal
			id2pv_delay_1 = i3ddmaVtotal * i3ddmaHtotal;
		}
		else if((i3ddmaVfreq > 495) && (i3ddmaVfreq < 505)){
			//normal mode 50Hz case, iv2pv = original iv2pv + 200 line (fine tune)
			id2pv_delay_1 = id2pv_delay_1 + (200 * i3ddmaHtotal);
		}
	}
#endif
#ifdef BUILD_QUICK_SHOW
	vodma_vodma_pvs0_gen_Reg.regValue = IoReg_Read32(VODMA_VODMA_PVS0_Gen_reg);
	vodma_vodma_pvs0_gen_Reg.iv2pv_dly = id2pv_delay_1;
	IoReg_Write32(VODMA_VODMA_PVS0_Gen_reg, vodma_vodma_pvs0_gen_Reg.regValue);
#endif
	rtd_pr_i3ddma_notice("[Game Mode] i3ddma vodma iv2pv delay = %x\n", id2pv_delay_1);

	if(VODMA_VODMA_PVS0_Gen_get_iv2pv_dly(IoReg_Read32(VODMA_VODMA_PVS0_Gen_reg))== id2pv_delay_1)
		return FALSE;

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_SET_IV2PV_DELAY, (unsigned long)id2pv_delay_1, 1))){	//info video fw change iv2pv_delay
		rtd_pr_i3ddma_err("ret=%d, SCALERIOC_SET_IV2PV_DELAY RPC fail !!!\n", ret);
	}

	wait_timeoutcnt = 50;
	do{
		if(VODMA_VODMA_PVS0_Gen_get_iv2pv_dly(IoReg_Read32(VODMA_VODMA_PVS0_Gen_reg))== id2pv_delay_1)
			break;
		else{
			usleep_range(1000, 1000); /* delay 10 ms*/
		}
	}while(--wait_timeoutcnt);

	if(0 == wait_timeoutcnt) {
		rtd_pr_i3ddma_emerg("IV2PV delay wait timeout!!!\n");
	}

	return TRUE;
}

#ifndef BUILD_QUICK_SHOW

#if 0
void drv_AV_GameMode_reset_iv2pv_delay(unsigned int iv2dv_delay)
{
	UINT32 i3ddmaHtotal, i3ddmaVtotal;
	UINT32 iv2pv_delay = 0;
	UINT32 dCLK,dHtotal,dVtotal,VO_framerate;
	unsigned char ret;

	i3ddmaVtotal = i3ddmaCtrl.pgen_timing->v_total;
	i3ddmaHtotal = i3ddmaCtrl.pgen_timing->h_total;
	dCLK = Get_DISPLAY_CLOCK_TYPICAL();
	dHtotal = Get_DISP_HORIZONTAL_TOTAL();
	VO_framerate = Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000);

	if(VO_framerate == 0){
		rtd_pr_i3ddma_err("[%s] Invalid VO_framerate %d, set to default 60Hz!!!\n",__FUNCTION__,VO_framerate);
		VO_framerate = 60000;
	}
	dVtotal = dCLK/dHtotal/(VO_framerate/1000);
	//rtd_pr_i3ddma_emerg("[Game Mode][%s][%d] dCLK = %d, dHtotal = %d, VO_framerate = %d\n",__FUNCTION__,__LINE__,dCLK,dHtotal,VO_framerate);
	//rtd_pr_i3ddma_emerg("[Game Mode][%s][%d] iv2dv_delay = %d, i3ddmaHtotal = %d, i3ddmaVtotal = %d, dHtotal = %d, dVtotal = %d\n",__FUNCTION__,__LINE__,iv2dv_delay,i3ddmaHtotal,i3ddmaVtotal,dHtotal,dVtotal);

	if((VO_framerate > 59500) && (VO_framerate < 60500)){
		//60Hz case, iv2pv = (iv2dv /dv_total)*i3_vtotal, margin = 3
		iv2pv_delay = ( iv2dv_delay * i3ddmaVtotal / dVtotal ) + 3;
		//rtd_pr_i3ddma_emerg("[GAME][%s][%d] iv2pv_delay line = %d\n",__FUNCTION__,__LINE__,iv2pv_delay);
		iv2pv_delay = iv2pv_delay * i3ddmaHtotal;
	}
	else if((VO_framerate > 49500) && (VO_framerate < 50500)){
		//50Hz case, iv2pv = i3d_vtotal - 41 line (fine tune)
		iv2pv_delay = ((i3ddmaVtotal - 41) * i3ddmaHtotal);
	}

	//rtd_pr_i3ddma_emerg("[Game Mode][%s][%d] iv2pv_delay = %d\n",__FUNCTION__,__LINE__,iv2pv_delay);

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_SET_IV2PV_DELAY, iv2pv_delay, 1))){	//info video fw change iv2pv_delay
		rtd_pr_i3ddma_err("ret=%d, SCALERIOC_SET_IV2PV_DELAY RPC fail !!!\n", ret);
	}
}
#endif
extern StructSrcRect get_main_input_size(void);
unsigned int drv_I3ddmaVodma_get_vodma_overscan_shift_data(void)
{
	StructSrcRect main_input_size;
	unsigned int data_bit_width;
	unsigned int vpos;
	unsigned int videoWidth;
	unsigned int shift_pixel;
	unsigned int shift_vertical_data;
	unsigned int temp_size = 0;

	main_input_size = get_main_input_size();

	//check i3ddma timing gen is ready
	if(i3ddmaCtrl[I3DDMA_DEV].pgen_timing == NULL){
		rtd_pr_i3ddma_emerg("[%s] i3ddma pgen_timing == NULL \n", __FUNCTION__);
		return 0;
	}

	//VOColorMap size = 4
	if(i3ddmaCtrl[I3DDMA_DEV].pgen_timing->color >= I3DDMA_COLOR_UNKNOW){
		rtd_pr_i3ddma_emerg("[%s] over color size (%d)\n", __FUNCTION__, i3ddmaCtrl[I3DDMA_DEV].pgen_timing->color);
		return 0;
	}

	if(i3ddmaCtrl[I3DDMA_DEV].pgen_timing->progressive == 0){
		vpos = main_input_size.srcy/2;
	}else{
		vpos = main_input_size.srcy;
	}
	videoWidth = i3ddmaCtrl[I3DDMA_DEV].pgen_timing->h_act_len;

	//plane->shift_vertical_data = vpos * ((config_hdmi_info_save.data_bit_width * (plane->displayWin.width/4) * shift_pixel + 127) / 128)*16;
	data_bit_width = (i3ddmaCtrl[I3DDMA_DEV].pgen_timing->depth == I3DDMA_COLOR_DEPTH_8B)? 8: 10;

	if (VOColorMap[i3ddmaCtrl[I3DDMA_DEV].pgen_timing->color] == VO_CHROMA_YUV420) {
		shift_pixel = 6;
	} else if (VOColorMap[i3ddmaCtrl[I3DDMA_DEV].pgen_timing->color] == VO_CHROMA_YUV422) {
		shift_pixel = 8;
	} else if (VOColorMap[i3ddmaCtrl[I3DDMA_DEV].pgen_timing->color] == VO_CHROMA_YUV444) {
		shift_pixel = 12;
	} else if (VOColorMap[i3ddmaCtrl[I3DDMA_DEV].pgen_timing->color] == VO_CHROMA_ARGB8888) {
		shift_pixel = 16;
	} else if (VOColorMap[i3ddmaCtrl[I3DDMA_DEV].pgen_timing->color] == VO_CHROMA_RGB888) {
		shift_pixel = 12;
	} else {
		shift_pixel = 12;
	}
	if(dvrif_i3ddma_compression_get_enable())
	{
		if(videoWidth % 0x20 != 0)
			videoWidth = (videoWidth + 0x20) & ~0x1F;
		data_bit_width = dvrif_i3ddma_get_compression_bits();
		temp_size = videoWidth * data_bit_width + 256;
		temp_size = (temp_size + 127) / 128;
		shift_vertical_data = vpos*temp_size*16;
	}
	else
		shift_vertical_data = vpos * ((data_bit_width * (videoWidth/4) * shift_pixel + 127) / 128) * 16;
	//rtd_pr_i3ddma_emerg("[%s] main_input_size (%d.%d.%d.%d) \n", __FUNCTION__, main_input_size.srcx, main_input_size.srcy, main_input_size.src_wid, main_input_size.src_height);
	//rtd_pr_i3ddma_emerg("[%s] vpos=%d, progressive = %d\n", __FUNCTION__, vpos, i3ddmaCtrl.pgen_timing->progressive);
	//rtd_pr_i3ddma_emerg("[%s] videoWidth=%d\n", __FUNCTION__, videoWidth);
	//rtd_pr_i3ddma_emerg("[%s] data_bit_width=%d, targetDepth=%d\n", __FUNCTION__, data_bit_width, i3ddmaCtrl.targetDepth);
	//rtd_pr_i3ddma_emerg("[%s] shift_pixel=%d, VOColorMap=%d, color=%d\n", __FUNCTION__, shift_pixel, VOColorMap[i3ddmaCtrl.pgen_timing->color], i3ddmaCtrl.pgen_timing->color);
	//rtd_pr_i3ddma_emerg("[%s] shift_vertical_data=%d\n", __FUNCTION__, shift_vertical_data);
	return shift_vertical_data;
}
#endif

void drv_set_I3ddmaVodma_SingleBuffer_rpc(unsigned int enable)
{
	int ret = 0;
	rtd_pr_i3ddma_notice("[%s] enable: %d\n", __FUNCTION__, enable);
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_SET_I3DDMA_VODMA_SINGLE_BUFFER, enable, 1))){
		rtd_pr_i3ddma_err("ret=%d, SCALERIOC_SET_I3DDMA_VODMA_SINGLE_BUFFER RPC fail !!!\n", ret);
	}
}
#ifdef BUILD_QUICK_SHOW
	#define htonl(x)	(x)
#endif
void drv_I3ddmaVodma_Compression_Attr_rpc(void)
{

	I3DDMA_VODMA_COMP_ATTR_T *i3ddma_vodma_comp;
	//unsigned int ulCount = 0;
	int ret;

	i3ddma_vodma_comp = (I3DDMA_VODMA_COMP_ATTR_T *)Scaler_GetShareMemVirAddr(SCALERIOC_SET_I3DDMA_VODMA_COMP_ATTR);
	//ulCount = sizeof(SCALERIOC_SET_I3DDMA_VODMA_COMP_ATTR) / sizeof(unsigned int);

	i3ddma_vodma_comp->bOnOff = dvrif_i3ddma_compression_get_enable();
	i3ddma_vodma_comp->comp_bits = dvrif_i3ddma_get_compression_bits();
	i3ddma_vodma_comp->comp_mode = dvrif_i3ddma_get_compression_mode();

	//change endian
	i3ddma_vodma_comp->bOnOff = htonl(i3ddma_vodma_comp->bOnOff);
	i3ddma_vodma_comp->comp_bits = htonl(i3ddma_vodma_comp->comp_bits);
	i3ddma_vodma_comp->comp_mode = htonl(i3ddma_vodma_comp->comp_mode);

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_SET_I3DDMA_VODMA_COMP_ATTR,0,0))){
		rtd_pr_i3ddma_err("[I3DDMA]ret=%d, SCALERIOC_SET_I3DDMA_VODMA_COMP_ATTR RPC fail !!!\n", ret);
	}
}


void drv_I3ddmaVodma_config_hdmi_ivs_src(void)
{
#ifndef UT_flag
	extern unsigned int Scaler_Get_VO_Ivs_SrcInfo(void);

	if((drvif_i3ddma_triplebuf_by_timing_protect_panel()== FALSE)&&(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI)){
		int ret;
		int count=10;

		if(Scaler_Get_VO_Ivs_SrcInfo() == 3){ //config hdmi set ivs src = IV_SRC_SEL_HDMIDMAVS
			// recover vo ivs as hdmi_ivs
			if (0 != (ret = Scaler_SendRPC(SCALERIOC_SET_VO_IVS_SRC_HDMI,1,1)))//set vo ivs src as hdmiivs
			{
				rtd_pr_i3ddma_err("ret=%d, SCALERIOC_SET_VO_IVS_SRC_HDMI RPC fail !!!\n", ret);
			}
			rtd_pr_i3ddma_notice("[%s] set hdmi ivs.\n", __FUNCTION__);
		}

		// 4k120 don't enable hw mode
		//recover vodma hw mode
		while((0 != (ret = Scaler_SendRPC(SCALERIOC_SET_VO_I3_BUF_SW_MODE,0,1)))&& --count)
		{
			msleep(10);
		}
		rtd_pr_i3ddma_notice("[%s] set vodma hw mode.\n", __FUNCTION__);
	}
#endif // #ifndef UT_flag
}

extern unsigned char get_AVD_Input_Source(void);
void drv_I3ddmaVodma_SingleBuffer_GameMode(unsigned char enable)
{
#if 1
	//set burst num
	h3ddma_cap0_wr_dma_num_bl_wrap_word_RBUS  h3ddma_cap0_wr_dma_num_bl_wrap_word_reg;
	h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.regValue = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_word_reg);
	if(dvrif_i3ddma_get_compression_mode() == I3DDMA_COMP_LINE_MODE)
		h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.cap0_line_burst_num = h3ddma_get_comp_burst_num(i3ddmaCtrl[I3DDMA_DEV].pgen_timing);
	else
		h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.cap0_line_burst_num = 0xFFFFFF;  //frame mode need set max
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_word_reg, h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.regValue);

	//before switching single buffer send comp attr
	drv_I3ddmaVodma_Compression_Attr_rpc();
	//send RPC to enable single buffer at VCPU
	drv_set_I3ddmaVodma_SingleBuffer_rpc(enable);

#else
	StructSrcRect main_input_size;
	unsigned int shift_vertical_data = 0;
	VSC_INPUT_TYPE_T srctype;
	unsigned char ch = Scaler_Get_CurVoInfo_plane();
	unsigned int v_len,interlace;
	extern StructDisplayInfo *Get_VO_Dispinfo(unsigned char ch);
	int block_offset=0;
	vodma_vodma_i2rnd_fifo_th_RBUS vodma_vodma_i2rnd_fifo_th_Reg;
	vodma_vodma_i2rnd_dma_info_RBUS vodma_vodma_i2rnd_dma_info_Reg;
	h3ddma_i3ddma_enable_RBUS  i3ddma_i3ddma_enable_reg;
	h3ddma_cap0_wr_dma_num_bl_wrap_word_RBUS  h3ddma_cap0_wr_dma_num_bl_wrap_word_reg;
	vodma_vodma_i2rnd_fifo_th_Reg.regValue = IoReg_Read32(VODMA_vodma_i2rnd_fifo_th_reg);
	vodma_vodma_i2rnd_dma_info_Reg.regValue = 0;

	rtd_pr_i3ddma_notice("[%s] enable=%d\n", __FUNCTION__, enable);
	//VRR use data FRC, VO do not do overscan

	//set burst num
	h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.regValue = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_word_reg);
	if(dvrif_i3ddma_get_compression_mode() == I3DDMA_COMP_LINE_MODE)
		h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.cap0_line_burst_num = h3ddma_get_comp_burst_num(i3ddmaCtrl.pgen_timing);
	else
		h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.cap0_line_burst_num = 0xFFFFFF;  //frame mode need set max
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_word_reg, h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.regValue);

	if(((Get_DISPLAY_REFRESH_RATE() == 120) && (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI) && (vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag()))
		|| (get_hdmi_4k_hfr_mode() != HDMI_NON_4K120)) // 4k120 case or vrr case do not to do vo shift, because frame buffer mode
	{
		shift_vertical_data = 0;
	}
	else if(get_vsc_data_path_indicator(DRIVER_FREERUN_PATTERN) && (scaler_vsc_get_gamemode_go_datafrc_mode()==FALSE)){
		if ((Get_VO_Dispinfo(ch)->disp_status&_BIT8)>>8) { // interlace
			v_len=(Get_VO_Dispinfo(ch)->IPV_ACT_LEN_PRE<<1);
			interlace=1;
		} else {
			v_len=Get_VO_Dispinfo(ch)->IPV_ACT_LEN_PRE;
			interlace=0;
		}

		main_input_size = get_main_input_size();

		rtd_pr_i3ddma_notice("main_input_size.src_height= %d, v_len = %d \n", main_input_size.src_height, v_len);

		//vo not do overscan
		if((main_input_size.src_height != v_len) && (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_HDMI)){
			return;
		}

		srctype = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);
		if((srctype != VSC_INPUTSRC_VDEC) && (srctype != VSC_INPUTSRC_JPEG) ){
			//vodma do overscan, so the vo display buffer should cut overscan address.
			shift_vertical_data = drv_I3ddmaVodma_get_vodma_overscan_shift_data();
			rtd_pr_i3ddma_notice("[%s] shift_vertical_data=%d\n", __FUNCTION__, shift_vertical_data);
		}
	}

    // vflip buffer start address offset control
    if(Get_PANEL_VFLIP_ENABLE() && i3ddmaCtrl.pgen_timing){
        unsigned int TotalBits;
        unsigned char data_bit_width, chroma_bit_width;
        data_bit_width = i3ddmaCtrl.pgen_timing->depth == I3DDMA_COLOR_DEPTH_8B? 8: 10;
        chroma_bit_width = VOColorMap[i3ddmaCtrl.pgen_timing->color] == VO_CHROMA_YUV422? 2: 3;
        TotalBits = i3ddmaCtrl.pgen_timing->h_act_len * chroma_bit_width * data_bit_width ;
        block_offset = (TotalBits * (i3ddmaCtrl.pgen_timing->v_act_len - 1) ) / 8;

        rtd_pr_i3ddma_debug("[Game Mode][VFlip] H/V=%dx%d, depth/chroma/size=%d/%d/%d\n",
            i3ddmaCtrl.pgen_timing->h_act_len, i3ddmaCtrl.pgen_timing->v_act_len,
            data_bit_width, chroma_bit_width, TotalBits);

        if(!enable){
            rtd_pr_i3ddma_debug("[Game Mode][VFlip] buffer[1]=%x->%x+%d\n", IoReg_Read32(VODMA_VODMA_V1_SEQ_3D_L2_reg), (unsigned int)i3ddmaCtrl.cap_buffer[1].phyaddr, block_offset);
        }
    }
#if 0
    else
        rtd_pr_i3ddma_debug("[Game Mode] VFLip/PGen=%d/%x\n", Get_PANEL_VFLIP_ENABLE(), (unsigned int)i3ddmaCtrl.pgen_timing);
#endif

	if(enable == TRUE){
		IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg, i3ddmaCtrl.cap_buffer[0].phyaddr);
		IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_1_reg, i3ddmaCtrl.cap_buffer[0].phyaddr); //cap1 low limit
		IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_1_reg, i3ddmaCtrl.cap_buffer[0].phyaddr + i3ddmaCtrl.cap_buffer[0].size); //cap1 up limit
		#if 1 //def CONFIG_ALL_SOURCE_DATA_FS
		if(Get_PANEL_VFLIP_ENABLE() && i3ddmaCtrl.pgen_timing){
		    IoReg_Write32(VODMA_VODMA_V1_SEQ_3D_L2_reg, i3ddmaCtrl.cap_buffer[0].phyaddr + block_offset - shift_vertical_data);
			IoReg_Write32(VODMA_VODMA_V1_SEQ_3D_L1_reg, i3ddmaCtrl.cap_buffer[0].phyaddr + block_offset - shift_vertical_data);
		}
		else{
		    IoReg_Write32(VODMA_VODMA_V1_SEQ_3D_L2_reg, i3ddmaCtrl.cap_buffer[0].phyaddr + block_offset + shift_vertical_data);
			IoReg_Write32(VODMA_VODMA_V1_SEQ_3D_L1_reg, i3ddmaCtrl.cap_buffer[0].phyaddr + block_offset + shift_vertical_data);
		}
		#else
			IoReg_Write32(VODMA_VODMA_V1_SEQ_3D_L2_reg, i3ddmaCtrl.cap_buffer[0].phyaddr);
		#endif
		//Eric@20180628 i3ddma->vo input fast case, need use new mode.
		//old mode: send flag before capture
		//new mode: send flag after capture done

		i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
		i3ddma_i3ddma_enable_reg.block_sel_to_flag_fifo_option = 0;
		IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_i3ddma_enable_reg.regValue);
		vodma_vodma_i2rnd_fifo_th_Reg.dram_buf_num_i3ddma = 1;
		vodma_vodma_i2rnd_fifo_th_Reg.dma_info_th_dispm = 1;
		vodma_vodma_i2rnd_fifo_th_Reg.dma_info_th_i3ddma = 1;

		vodma_vodma_i2rnd_dma_info_Reg.dma_info_rptr_fw_set_dispm = 1;
		vodma_vodma_i2rnd_dma_info_Reg.dma_info_rptr_fw_set_i3ddma = 1;
		vodma_vodma_i2rnd_dma_info_Reg.dma_info_wptr_fw_set_dispm = 1;
		vodma_vodma_i2rnd_dma_info_Reg.dma_info_wptr_fw_set_i3ddma = 1;
	}else if(i3ddmaCtrl.cap_buffer[1].phyaddr){
		IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg, i3ddmaCtrl.cap_buffer[1].phyaddr);
		IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_1_reg, i3ddmaCtrl.cap_buffer[1].phyaddr); //cap1 low limit
		IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_1_reg, i3ddmaCtrl.cap_buffer[1].phyaddr + i3ddmaCtrl.cap_buffer[1].size); //cap1 up limit
		#if 1 //def CONFIG_ALL_SOURCE_DATA_FS
		if(Get_PANEL_VFLIP_ENABLE() && i3ddmaCtrl.pgen_timing){
		    IoReg_Write32(VODMA_VODMA_V1_SEQ_3D_L2_reg, i3ddmaCtrl.cap_buffer[1].phyaddr + block_offset - shift_vertical_data);
			IoReg_Write32(VODMA_VODMA_V1_SEQ_3D_L1_reg, i3ddmaCtrl.cap_buffer[0].phyaddr + block_offset - shift_vertical_data);
		}
		else{
		    IoReg_Write32(VODMA_VODMA_V1_SEQ_3D_L2_reg, i3ddmaCtrl.cap_buffer[1].phyaddr + block_offset + shift_vertical_data);
			IoReg_Write32(VODMA_VODMA_V1_SEQ_3D_L1_reg, i3ddmaCtrl.cap_buffer[0].phyaddr + block_offset + shift_vertical_data);
		}
		#else
			IoReg_Write32(VODMA_VODMA_V1_SEQ_3D_L2_reg, i3ddmaCtrl.cap_buffer[1].phyaddr);
		#endif
		//Eric@20180628 i3ddma->vo input fast case, need use new mode.
		//old mode: send flag before capture
		//new mode: send flag after capture done
		i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
		i3ddma_i3ddma_enable_reg.block_sel_to_flag_fifo_option = 1;
		IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_i3ddma_enable_reg.regValue);
		vodma_vodma_i2rnd_fifo_th_Reg.dram_buf_num_i3ddma = 3;
		vodma_vodma_i2rnd_fifo_th_Reg.dma_info_th_dispm = 1;
		vodma_vodma_i2rnd_fifo_th_Reg.dma_info_th_i3ddma = 1;

		vodma_vodma_i2rnd_dma_info_Reg.dma_info_rptr_fw_set_dispm = 1;
		vodma_vodma_i2rnd_dma_info_Reg.dma_info_rptr_fw_set_i3ddma = 1;
		vodma_vodma_i2rnd_dma_info_Reg.dma_info_wptr_fw_set_dispm = 1;
		vodma_vodma_i2rnd_dma_info_Reg.dma_info_wptr_fw_set_i3ddma = 1;
	}
	IoReg_Write32(VODMA_vodma_i2rnd_fifo_th_reg, vodma_vodma_i2rnd_fifo_th_Reg.regValue);
	IoReg_Write32(VODMA_vodma_i2rnd_dma_info_reg, vodma_vodma_i2rnd_dma_info_Reg.regValue);
	rtd_pr_i3ddma_debug("[Game Mode] i3ddma and vodma game mode done enable=%d!!, L2=%x\n",enable, IoReg_Read32(VODMA_VODMA_V1_SEQ_3D_L2_reg));
#endif
}

void set_vtop_input_color_format(I3DDMA_COLOR_SPACE_T color_format)
{//set final target i3ddma output format
	vtop_input_color_format = color_format;
}

I3DDMA_COLOR_SPACE_T get_target_i3ddma_color_format(void)
{//get final target i3ddma output format
	return vtop_input_color_format;
}

void backup_hdmi_src_info(I3DDMA_TIMING_T *src_info)
{//backup hdmi timing and color info
	memcpy(&hdmi_source_info_backup, src_info, sizeof(I3DDMA_TIMING_T));
}

I3DDMA_TIMING_T *get_backup_hdmi_info(void)
{//get hdmi backup data
	return &hdmi_source_info_backup;
}

#if defined CONFIG_SCALER_BRING_UP || defined BUILD_QUICK_SHOW

extern int VODMA_ConfigHDMI(VIDEO_RPC_VOUT_CONFIG_HDMI_3D *HDMI_INFO);
#endif

#ifdef CONFIG_ENABLE_HDMI_NN
void I3DDMA_ResetCap0(void)
{
	h3ddma_cap0_cti_dma_wr_ctrl_RBUS h3ddma_cap0_cti_dma_wr_ctrl_reg;

	h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg);
	h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_force_all_rst = 1;
	IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg, h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue);
	rtd_pr_i3ddma_emerg("0   0xb8025b0c=%x\n", IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg));

	mdelay(2);
	h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg);
	h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_force_all_rst = 0;
	IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg, h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue);
	rtd_pr_i3ddma_emerg("1   0xb8025b0c=%x\n", IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg));
}

void I3DDMA_ResetCap1(void)
{
	h3ddma_cap1_cti_dma_wr_ctrl_RBUS h3ddma_cap1_cti_dma_wr_ctrl_reg;
	vgip_vgip_chn1_ctrl_RBUS vgip_vgip_chn1_ctrl_reg;
	vgip_vgip_chn1_delay_RBUS vgip_vgip_chn1_delay_reg;

	rtd_pr_i3ddma_emerg("I3DDMA_ResetCap1\n");

	h3ddma_cap1_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg);
	h3ddma_cap1_cti_dma_wr_ctrl_reg.cap1_force_all_rst = 1;
	IoReg_Write32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg, h3ddma_cap1_cti_dma_wr_ctrl_reg.regValue);
	rtd_pr_i3ddma_emerg("0   0xb8025b8c=%x\n", IoReg_Read32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg));

	//mdelay(2);
	//h3ddma_cap1_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg);
	//h3ddma_cap1_cti_dma_wr_ctrl_reg.cap1_force_all_rst = 0;
	//IoReg_Write32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg, h3ddma_cap1_cti_dma_wr_ctrl_reg.regValue);
	//rtd_pr_i3ddma_emerg("1   0xb8025b8c=%x\n", IoReg_Read32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg));


	// Main vgip change to HDMI source
	vgip_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_CTRL_reg);
	vgip_vgip_chn1_ctrl_reg.ch1_in_sel = 0;
	IoReg_Write32(VGIP_VGIP_CHN1_CTRL_reg, vgip_vgip_chn1_ctrl_reg.regValue);

	// Set ivs delay = 2
	vgip_vgip_chn1_delay_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_DELAY_reg);
	vgip_vgip_chn1_delay_reg.ch1_ivs_dly = 2;
	IoReg_Write32(VGIP_VGIP_CHN1_DELAY_reg, vgip_vgip_chn1_delay_reg.regValue);

}
#endif

VIDEO_RPC_VOUT_CONFIG_HDMI_3D *Get_Val_GetI3DDMAConfigData_impl(unsigned long vir_addr_noncache)
{
    return (VIDEO_RPC_VOUT_CONFIG_HDMI_3D *)vir_addr_noncache;
}
VIDEO_RPC_VOUT_CONFIG_HDMI_3D *(*GetI3DDMAConfigData)(unsigned long vir_addr_noncache) = Get_Val_GetI3DDMAConfigData_impl;

char I3DDMA_SetupVODMA(I3DDMA_TIMING_T *timing,unsigned char display) {

#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)
	unsigned long result = 0;
#endif

	VIDEO_RPC_VOUT_CONFIG_HDMI_3D *config_data;

	//struct page *page = 0;
	//page = alloc_page(GFP_KERNEL);
	//config_datatmp = (VIDEO_RPC_VOUT_SET_3D_MODE *)page_address(page);
	unsigned long vir_addr, vir_addr_noncache;
	unsigned int phy_addr;
	unsigned int fixed_vfreq = timing->v_freq_1000;
	unsigned int fixed_src_vfreq = timing->src_v_freq_1000;
    unsigned int unLoop = 0;
#ifndef BUILD_QUICK_SHOW

	vir_addr = (unsigned long)dvr_malloc_uncached_specific(sizeof(VIDEO_RPC_VOUT_CONFIG_HDMI_3D), GFP_DCU1_LIMIT, (void **)&vir_addr_noncache);
	phy_addr = (unsigned int)dvr_to_phys((void*)vir_addr);

	config_data = GetI3DDMAConfigData(vir_addr_noncache);
#else
        VIDEO_RPC_VOUT_CONFIG_HDMI_3D localConfigData = {0};
        config_data = &localConfigData;
#endif

	memset(config_data, 0, sizeof(VIDEO_RPC_VOUT_CONFIG_HDMI_3D));

	config_data->width = timing->h_act_len;
	config_data->height = timing->v_act_len;
	config_data->h_start = timing->h_act_sta;
	config_data->v_start = timing->v_act_sta;
	if(timing->color < I3DDMA_COLOR_UNKNOW)	//Klocwork: buffer may overflow.
		config_data->chroma_fmt = VOColorMap[timing->color];
	else
		config_data->chroma_fmt = VOColorMap[I3DDMA_COLOR_RGB];
	if(i3ddmaCtrl[I3DDMA_DEV].ptx_timing && (i3ddmaCtrl[I3DDMA_DEV].ptx_timing->color < I3DDMA_COLOR_UNKNOW))
		config_data->input_fmt = VOColorMap[i3ddmaCtrl[I3DDMA_DEV].ptx_timing->color];
	else
		config_data->input_fmt = VOColorMap[I3DDMA_COLOR_RGB];
	config_data->data_bit_width = (timing->depth == I3DDMA_COLOR_DEPTH_8B)? 8: 10;
	config_data->framerate =  fixed_vfreq;
	config_data->framerate_ori = fixed_src_vfreq;
    config_data->srcframerate =  fixed_src_vfreq;
	config_data->progressive = timing->progressive;
	config_data->l1_st_adr = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg);
	config_data->r1_st_adr = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r0_reg);
	config_data->l2_st_adr = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg);
	config_data->r2_st_adr = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r1_reg);
	config_data->l3_st_adr = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_2_reg);//input fast use 3-buffer @Crixus 20170508
	config_data->r3_st_adr = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_r2_reg);
	//config_data->hw_auto_mode = 1;
	// Dolby Vision force in FW mode

	if(get_scaler_qms_mode_flag())
		config_data->hw_auto_mode = 0;//qms use fw mode to select buffer
	else if(drvif_i3ddma_triplebuf_by_timing_protect_panel()) {
		if(get_HDMI_HDR_mode() == HDR_DOLBY_HDMI)
			config_data->hw_auto_mode = 1;
		else
			config_data->hw_auto_mode = 0;
	} else
		config_data->hw_auto_mode = 1;

	if((Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI) && (Get_DISPLAY_PANEL_CUSTOM_INDEX() == VBY_ONE_PANEL_COMMERCIAL_4K2K_600M_1S_8L_DUPLICATE))
		config_data->hw_auto_mode = 1;
	config_data->quincunx_en = 0;
	config_data->videoPlane = VO_VIDEO_PLANE_V1;
	config_data->i2rnd_sub_src = _DISABLE;
	config_data->i2rnd_display_table = I2RND_TABLE_MAX;
	config_data->htotal = timing->h_total;
	config_data->vtotal = timing->v_total;
	config_data->enable_comps = dvrif_i3ddma_compression_get_enable();
	if(dvrif_i3ddma_compression_get_enable() == TRUE)
	{
		config_data->comps_bits = dvrif_i3ddma_get_compression_bits();//dvrif_i3ddma_get_compression_totalsize();
		config_data->comps_line_mode = dvrif_i3ddma_get_compression_mode();//default use line mode
	}
	else
	{
		config_data->comps_bits =  0;
		config_data->comps_line_mode = 0;
	}
	scaler_overscan_vertical_size(display,config_data->progressive,config_data->height,&config_data->overscan_v_len,&config_data->overscan_v_start);
	if (get_vsc_src_is_hdmi_or_dp(display)) {
		config_data->hdr_source = drvif_Hdmi_IsDRMInfoReady();
		config_data->vrr_en = vbe_disp_get_VRR_timingMode_flag();
		config_data->freesync_en = vbe_disp_get_freesync_mode_flag();
	} else {
		config_data->hdr_source =0;
        config_data->vrr_en = 0;
		config_data->freesync_en = 0;
	}

#ifdef CONFIG_ENABLE_HDMI_NN
	config_data->nn_st_addr = g_tNN_Cap_Buffer[0].phyaddr;
	config_data->nn_buf_size = g_tNN_Cap_Buffer[0].size;
#else
	config_data->nn_st_addr = 0;
	config_data->nn_buf_size = 0;
#endif
	config_data->cap_buffer_size = i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].size;

#ifndef BUILD_QUICK_SHOW
    if(i3ddmaCtrl[I3DDMA_DEV].hw_i3ddma_dma == I3DDMA_3D_OPMODE_SW)
    {
        //I3DDMA_Send_Vo_SwBufNum_share_memory(I3DDMA_SwGetBufNum());
        config_data->l1_st_adr = (unsigned int)s_i3ddma_sw_cap_buffer[0].phyaddr;
        config_data->l2_st_adr = (unsigned int)s_i3ddma_sw_cap_buffer[1].phyaddr;
        config_data->l3_st_adr = (unsigned int)s_i3ddma_sw_cap_buffer[2].phyaddr;
        config_data->hw_auto_mode = 0;// 1 use auto mode for interlace input
        config_data->sw_bufnub = I3DDMA_SwGetBufNum();
        config_data->sw_videosize = s_i3ddma_sw_cap_buffer[0].size;
        for(unLoop = 0; unLoop < SW_I3DDMA_CAP_NUM; unLoop++)
        {
            config_data->sw_cap_buf[unLoop] = s_i3ddma_sw_cap_buffer[unLoop].phyaddr;
        }
    }
#endif

#if 0 //def CONFIG_ENABLE_HDMI_NN
	mdelay(100);
	i3ddma_dump_data_to_file(0);
#endif

#if defined BUILD_QUICK_SHOW
	rtd_pr_i3ddma_info("width = %d\n", config_data->width);
	rtd_pr_i3ddma_info("height = %d\n", config_data->height);
	rtd_pr_i3ddma_info("progressive = %d\n", config_data->progressive);
	rtd_pr_i3ddma_info("framerate = %d\n", config_data->framerate);
	rtd_pr_i3ddma_info("framerate_ori = %d\n", config_data->framerate_ori);
	rtd_pr_i3ddma_info("chroma_fmt = %d\n", config_data->chroma_fmt);
	rtd_pr_i3ddma_info("data_bit_width = %d\n", config_data->data_bit_width);
	rtd_pr_i3ddma_info("hw_auto_mode = %d\n", config_data->hw_auto_mode);
	rtd_pr_i3ddma_info("l1_sta_adr = %x\n", config_data->l1_st_adr);
	rtd_pr_i3ddma_info("r1_sta_adr = %x\n", config_data->r1_st_adr);
	rtd_pr_i3ddma_info("l2_sta_adr = %x\n", config_data->l2_st_adr);
	rtd_pr_i3ddma_info("r2_sta_adr = %x\n", config_data->r2_st_adr);
	rtd_pr_i3ddma_info("quincunx_en = %d\n", config_data->quincunx_en);
	rtd_pr_i3ddma_info("quincunx_mode_fw = %d\n", config_data->quincunx_mode_fw);
	rtd_pr_i3ddma_info("sensio3D_en = %d\n", config_data->sensio3D_en);
	rtd_pr_i3ddma_info("sg3d_dataFrc = %d\n", config_data->sg3d_dataFrc);
	rtd_pr_i3ddma_info("i2rnd_sub_src = %d\n", config_data->i2rnd_sub_src);
	rtd_pr_i3ddma_info("i2rnd_display_table = %d\n", config_data->i2rnd_display_table);
	rtd_pr_i3ddma_info("htotal = %d\n", config_data->htotal);
	rtd_pr_i3ddma_info("vtotal = %d\n", config_data->vtotal);
	rtd_pr_i3ddma_info("hw_auto_mode = %d\n", config_data->hw_auto_mode);
    rtd_pr_i3ddma_info("srcframerate = %d\n", config_data->srcframerate);
	rtd_pr_i3ddma_info("cap_buffer_size = %x\n", config_data->cap_buffer_size);
	VODMA_ConfigHDMI(config_data);
#else
    rtd_pr_i3ddma_info("hw_auto_mode = %d\n", config_data->hw_auto_mode);
    rtd_pr_i3ddma_info("l1_sta_adr = %x\n", config_data->l1_st_adr);
    rtd_pr_i3ddma_info("l2_sta_adr = %x\n", config_data->l2_st_adr);
    rtd_pr_i3ddma_info("l3_st_adr = %x\n", config_data->l3_st_adr);
    rtd_pr_i3ddma_info("sw_bufnub = %d\n", config_data->sw_bufnub);
    rtd_pr_i3ddma_info("sw_videosize = %d\n", config_data->sw_videosize);

	config_data->width = htonl(config_data->width);
	config_data->height = htonl(config_data->height);
	config_data->chroma_fmt = htonl(config_data->chroma_fmt);
	config_data->input_fmt = htonl(config_data->input_fmt);
	config_data->data_bit_width = htonl(config_data->data_bit_width);
	config_data->framerate =  htonl(config_data->framerate);
	config_data->framerate_ori =  htonl(config_data->framerate_ori);
    config_data->srcframerate =  htonl(config_data->srcframerate);
	config_data->progressive = htonl(config_data->progressive);
	config_data->l1_st_adr = htonl(config_data->l1_st_adr);
	config_data->r1_st_adr = htonl(config_data->r1_st_adr);
	config_data->l2_st_adr = htonl(config_data->l2_st_adr);
	config_data->r2_st_adr = htonl(config_data->r2_st_adr);
	config_data->l3_st_adr = htonl(config_data->l3_st_adr);//input fast use 3-buffer @Crixus 20170508
	config_data->r3_st_adr = htonl(config_data->r3_st_adr);
	config_data->hw_auto_mode = htonl(config_data->hw_auto_mode);
	config_data->quincunx_en = htonl(config_data->quincunx_en);
	config_data->videoPlane = htonl(config_data->videoPlane);
	config_data->i2rnd_sub_src = htonl(config_data->i2rnd_sub_src);
	config_data->i2rnd_display_table = htonl(config_data->i2rnd_display_table);
	config_data->htotal = htonl(config_data->htotal);
	config_data->vtotal = htonl(config_data->vtotal);
	config_data->h_start = htonl(config_data->h_start);
	config_data->v_start = htonl(config_data->v_start);
	config_data->enable_comps = htonl(config_data->enable_comps);
	config_data->comps_bits = htonl(config_data->comps_bits);
	config_data->comps_line_mode = htonl(config_data->comps_line_mode);
	config_data->overscan_v_len = htonl(config_data->overscan_v_len);
	config_data->overscan_v_start = htonl(config_data->overscan_v_start);
	config_data->hdr_source = htonl(config_data->hdr_source);
    config_data->vrr_en = htonl(config_data->vrr_en);
    config_data->freesync_en = htonl(config_data->freesync_en);
	config_data->nn_st_addr = htonl(config_data->nn_st_addr);
	config_data->nn_buf_size = htonl(config_data->nn_buf_size);
	config_data->cap_buffer_size = htonl(config_data->cap_buffer_size);
    config_data->sw_bufnub = htonl(config_data->sw_bufnub);
    config_data->sw_videosize = htonl(config_data->sw_videosize);
    for(unLoop = 0; unLoop < SW_I3DDMA_CAP_NUM; unLoop++)
    {
        config_data->sw_cap_buf[unLoop] = htonl(config_data->sw_cap_buf[unLoop]);
    }

#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)
	if (RPC_FAIL==send_rpc_command(RPC_VIDEO, VIDEO_RPC_VOUT_ToAgent_ConfigHDMI_3D,phy_addr, 0, &result))
	{
		rtd_pr_i3ddma_debug("RPC fail!!\n");
		dvr_free((void *)vir_addr);
		return FALSE;
	}
#endif
#endif
#ifndef BUILD_QUICK_SHOW
	dvr_free((void *)vir_addr);
#endif

	return TRUE;

}
#ifndef BUILD_QUICK_SHOW

//I3DDMA_CAPTURE_BUFFER_T idma_cap_buf[3];
I3DDMA_CAPTURE_BUFFER_T idma_cap_buf[4]; //fix coverity issue,the index must >=4
//#define DISP2TVE_IDMA_CAPSIZE  1920 * 1080 * 3// per byte => 3*8 = 24bits
#define SCART_OUT_TVE_IDMA_CAPSIZE  720 * 576 * 2// per byte => 2*8 = 16bits

unsigned char I3DDMA_disp2tve_CapBufInit(void) {

	unsigned int size = SCART_OUT_TVE_IDMA_CAPSIZE;
	unsigned int buffer_size = 0, align_addr = 0, query_addr = 0;

		if(idma_cap_buf[0].phyaddr == 0)
		{
				size = drvif_memory_get_data_align(size, DMA_SPEEDUP_ALIGN_VALUE);

				buffer_size = dvr_size_alignment_cma(size*4 + 512);
				/*Use pli memory allocation @Crixus 20150801*/
				//idma_cap_buf[0].phyaddr=pli_malloc((size*4 + 512), GFP_DCU2_FIRST);

				query_addr = pli_malloc(buffer_size, GFP_DCU2_FIRST);
				align_addr = dvr_memory_alignment((unsigned long)query_addr, buffer_size);

				idma_cap_buf[0].phyaddr = align_addr;
				idma_cap_buf[0].phyaddr_unalign = query_addr;

				if (idma_cap_buf[0].phyaddr == INVALID_VAL) {
					rtd_pr_i3ddma_debug("malloc idma buffer fail .......................................\n");
					return 1;
				}

				idma_cap_buf[0].size = size;

				rtd_pr_i3ddma_debug("[idma-cap-buf] buf[0] phy=%x(%d KB)\n", (UINT32)idma_cap_buf[0].phyaddr, idma_cap_buf[0].size >> 10);

				idma_cap_buf[1].phyaddr = idma_cap_buf[0].phyaddr+ idma_cap_buf[0].size;
				idma_cap_buf[1].size = size;
				rtd_pr_i3ddma_debug("[idma-cap-buf] buf[1] phy=%x(%d KB)\n", (UINT32)idma_cap_buf[1].phyaddr, idma_cap_buf[1].size >> 10);

				idma_cap_buf[2].phyaddr = idma_cap_buf[1].phyaddr+ idma_cap_buf[1].size;
				idma_cap_buf[2].size = size;
				rtd_pr_i3ddma_debug("[idma-cap-buf] buf[2] phy=%x(%d KB)\n", (UINT32)idma_cap_buf[2].phyaddr, idma_cap_buf[2].size >> 10);

				idma_cap_buf[3].phyaddr = idma_cap_buf[2].phyaddr+ idma_cap_buf[2].size;
				idma_cap_buf[3].size = size;
				rtd_pr_i3ddma_debug("[idma-cap-buf] buf[3] phy=%x(%d KB)\n", (UINT32)idma_cap_buf[3].phyaddr, idma_cap_buf[3].size >> 10);

				IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_reg, idma_cap_buf[0].phyaddr );
				IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_reg, idma_cap_buf[3].phyaddr + idma_cap_buf[3].size);

		}
	return 0;
}

//add to disconnect api
void I3DDMA_disp2tve_CapBufDeInit(void) {
	if (idma_cap_buf[0].phyaddr) {
		/*Use pli memory allocation @Crixus 20150801*/
		pli_free(idma_cap_buf[0].phyaddr_unalign);
		idma_cap_buf[0].phyaddr = 0;
		idma_cap_buf[0].phyaddr_unalign = 0;

		rtd_pr_i3ddma_debug("[idma-cap-buf] idma cap buffer is not empty!!free the buffer!\n");
	}

	rtd_pr_i3ddma_debug("[idma-cap-buf] Deinit done!\n");
}

void gamemode_delaytime_print_msg(void){
	UINT32 voHtotal;
	//UINT32 voVactive;
	UINT32 VoClock;
	UINT32 i3ddmaVtotal;
	UINT32 i3ddmaHtotal;
	UINT32 i3ddmaVfreq;
	UINT32 i3ddmaClock;
	UINT32 i3ddmaVsta;
	UINT32 iv2dv_delay;
	UINT32 iv2pv_delay;
	SLR_VOINFO *pVOInfo = NULL;
	vodma_vodma_pvs0_gen_RBUS vodma_vodma_pvs0_gen_Reg;
//	ppoverlay_fs_iv_dv_fine_tuning5_RBUS fs_iv_dv_fine_tuning5_reg;

	if(i3ddmaCtrl[I3DDMA_DEV].pgen_timing == NULL)
	{
		rtd_pr_i3ddma_emerg("[i3ddma][%s]timing is null,return\n",__FUNCTION__);
		return;
	}
	voHtotal = Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_H_LEN);
	//voVactive = Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_IPV_ACT_LEN);
	pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
	VoClock = pVOInfo->pixel_clk;

	i3ddmaHtotal = i3ddmaCtrl[I3DDMA_DEV].pgen_timing->h_total;
	i3ddmaVtotal = i3ddmaCtrl[I3DDMA_DEV].pgen_timing->v_total;
	i3ddmaVfreq = i3ddmaCtrl[I3DDMA_DEV].pgen_timing->v_freq_1000;
	i3ddmaVsta = i3ddmaCtrl[I3DDMA_DEV].pgen_timing->v_act_sta;
	i3ddmaClock = (unsigned int)( (unsigned long long)i3ddmaHtotal * (unsigned long long)i3ddmaVtotal * (unsigned long long)i3ddmaVfreq / 1000ULL );

	vodma_vodma_pvs0_gen_Reg.regValue = IoReg_Read32(VODMA_VODMA_PVS0_Gen_reg);
	iv2pv_delay = (vodma_vodma_pvs0_gen_Reg.iv2pv_dly)/voHtotal;
	//fs_iv_dv_fine_tuning5_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg);
	//iv2dv_delay = fs_iv_dv_fine_tuning5_reg.iv2dv_line;
	if(IoReg_Read32(VGIP_VGIP_VBISLI_reg)&_BIT19){ //for debug only
		iv2dv_delay = drv_GameMode_decided_iv2dv_delay_old_mode();
	}else{
		iv2dv_delay = drv_GameMode_decided_iv2dv_delay_new_mode();
	}

	rtd_pr_i3ddma_emerg(" ===== [GameModeDelayPrint] ======= \n");
	rtd_pr_i3ddma_emerg(" ===== [source] Htotal   : %d ===== \n", i3ddmaHtotal);
	rtd_pr_i3ddma_emerg(" ===== [source] clock    : %d ===== \n", i3ddmaClock);
	rtd_pr_i3ddma_emerg(" ===== [vodma]  voHtotal : %d ===== \n", voHtotal);
	rtd_pr_i3ddma_emerg(" ===== [vodma]  VoClock  : %d ===== \n", VoClock);
	rtd_pr_i3ddma_emerg(" ===== iv2pv_delay       : %d ===== \n", iv2pv_delay);
	rtd_pr_i3ddma_emerg(" ===== iv2dv_delay       : %d ===== \n", iv2dv_delay);
	rtd_pr_i3ddma_emerg(" ===== dma Vgip Vsta     : %d ===== \n", i3ddmaVsta);
	rtd_pr_i3ddma_emerg(" ===== Interlaced        : %d ===== \n", Scaler_DispGetStatus(0, SLR_DISP_INTERLACE));

}

spinlock_t* get_hdr_seamless_change_spinlock(void)
{//this is for  hdr_seamless_trigger_flagprotection
	return &HDR_SEAMLESS_CHANGE_SPINLOCK;
}

void set_seamless_trigger_flag(unsigned char TorF)
{
	hdr_seamless_trigger_flag= TorF;
}

unsigned char get_seamless_trigger_flag(void)
{
	return hdr_seamless_trigger_flag;
}

I3DDMA_COLOR_SPACE_T decide_i3ddma_output_foramt(I3DDMA_TIMING_T *timing)
{//decide i3ddma output color format
	// In Vertical strip & Checker board 3D, I3DDMA only support YUV444 capture
	if(dvrif_i3ddma_compression_get_enable()){
		return I3DDMA_COLOR_YUV444;
	}
	if((timing->i3ddma_3dformat == I3DDMA_VERTICAL_STRIPE)|| (timing->i3ddma_3dformat == I3DDMA_CHECKER_BOARD))
	{
		return I3DDMA_COLOR_YUV444;
	} // [Dolby Vision] IDMA capture in RGB format
	else if((get_HDMI_HDR_mode() == HDR_DOLBY_HDMI) && (timing->color == I3DDMA_COLOR_RGB)){
		return I3DDMA_COLOR_RGB;
	}
	else if((get_HDMI_HDR_mode() == HDR_HDR10_HDMI) /*&& (i3ddma_timing.color != I3DDMA_COLOR_RGB)*/){
		return I3DDMA_COLOR_YUV422;
	}else if(timing->color == I3DDMA_COLOR_RGB) {
		return I3DDMA_COLOR_YUV444;
	}else {
        // [WOSQRTK-10007][BW] Force enable HDMI 444(420) to 422!!
        if((timing->h_act_len >= 3800) && (timing->v_freq_1000 > 48000)
            && (timing->color == I3DDMA_COLOR_YUV444) && (timing->depth > 0))
        {
        	return I3DDMA_COLOR_YUV422;
        }else {
			return timing->color;
    	}
	}
}


void trigger_seamless_flow(VSC_INPUT_TYPE_T src, unsigned char hdr_enable, unsigned char hdr_type)
{//enable seamless flow. vide FW is at video
	unsigned long flags;//for spin_lock_irqsave
    seamless_change_sync_info *seamless_info_sharememory = NULL;
    //rtd_pr_i3ddma_notice("\r\n####func:% src:%d hdr_type:%d#####\r\n",__FUNCTION__,src, hdr_type);
	seamless_info_sharememory = (seamless_change_sync_info *)Scaler_GetShareMemVirAddr(SCALERIOC_SEAMLESS_CHANGE_SHAREMEMORY_INFO_SYNC_FLAG);

	if(!hdr_enable)
	{//sdr case
		if(get_support_vo_force_v_top(SLR_MAIN_DISPLAY, support_sdr_max_rgb))
		{//sdr force vtop maxRGB case
			hdr_enable = 1;
			hdr_type = HDR_DM_SDR_MAX_RGB;
		}
	}

	if(src == VSC_INPUTSRC_HDMI  || src == VSC_INPUTSRC_DP)
    {
    	if(seamless_info_sharememory)
    	{
	    	spin_lock_irqsave(get_hdr_seamless_change_spinlock(), flags);//lock i3ddma change spinlock
	        seamless_info_sharememory->hdmi_target_hdr_info = (hdr_type << 1) | hdr_enable; //bit0 = enable/disable bit1-9 hdr type
			seamless_info_sharememory->hdmi_target_color_format = vtop_input_color_format;
			set_seamless_trigger_flag(TRUE);
			spin_unlock_irqrestore(get_hdr_seamless_change_spinlock(), flags);//unlock i3ddma change spinlock
			rtd_pr_i3ddma_info("####func:%s hdr enable:%d type:%d#####\r\n",__FUNCTION__, hdr_enable, hdr_type);
    	}
	}
/* trigger by video fw
    else if(src == VSC_INPUTSRC_VDEC)
    {
    	if(seamless_info_sharememory)
    	{
	    	spin_lock_irqsave(get_vdec_seamless_change_spinlock(), flags);//lock vdec change spinlock
			set_vdec_seamless_change_flag(TRUE);
			seamless_info_sharememory->vdec_target_hdr_enable = hdr_type;
			spin_unlock_irqrestore(get_vdec_seamless_change_spinlock(), flags);//unlock vdec change spinlock
    	}
	}
*/
}


void I3ddma_format_changed_seamless(I3DDMA_TIMING_T *timing, I3DDMA_COLOR_SPACE_T target)
{//seamless change i3ddma capture setting
	h3ddma_0_rgb2yuv_dither_4xxto4xx_hdmi3_0_ctrl_RBUS i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg;
#ifdef CONFIG_ENABLE_HDMI_NN
    h3ddma_hsd_i3ddma0_uzd_ctrl0_RBUS i3ddma_uzd_ctrl0_reg;
#endif
	/*i3ddma capture setting*/
	int rem, len;
	unsigned int h_act_len;
	//h3ddma_i3ddma_enable_RBUS  i3ddma_i3ddma_enable_reg;
//	h3ddma_cap0_cap_ctl0_RBUS  i3ddma_cap_ctl0_reg;
//	h3ddma_cap_ctl1_RBUS i3ddma_cap_ctl1_reg;
	h3ddma_cap0_wr_dma_pxltobus_RBUS h3ddma_cap0_wr_dma_pxltobus_reg;

	I3DDMA_COLOR_SPACE_T source = timing->color;

#ifdef CONFIG_ENABLE_HDMI_NN
	/*i3ddma 422 444 setting*/
	i3ddma_uzd_ctrl0_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA0_UZD_CTRL0_reg);
	if(source == I3DDMA_COLOR_YUV422)
		i3ddma_uzd_ctrl0_reg.sort_fmt = 1; //yuv422 bypass (1)
	else if(source == I3DDMA_COLOR_RGB) {
		if(target == I3DDMA_COLOR_YUV422)
	 		i3ddma_uzd_ctrl0_reg.sort_fmt = 0; //444 to 422
	 	else
			i3ddma_uzd_ctrl0_reg.sort_fmt = 1;//keep 444
	} else {//input 444
		if(target == I3DDMA_COLOR_YUV422)
			i3ddma_uzd_ctrl0_reg.sort_fmt = 0; //444 to 422
		else
			i3ddma_uzd_ctrl0_reg.sort_fmt = 1;//keep 444
	}
	IoReg_Write32(H3DDMA_HSD_I3DDMA0_UZD_CTRL0_reg, i3ddma_uzd_ctrl0_reg.regValue);
#endif

	/*i3ddma rgb2yuv setting*/
	if (source == I3DDMA_COLOR_RGB) {
		if(DMA_VGIP_DMA_VGIP_CTRL_get_dma_in_sel(IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg)) == VGIP_SRC_ATV) {
			/*av source, idma bypass rgb2yuv*/
			i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.regValue = IoReg_Read32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_CTRL_reg);
			i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.en_rgb2yuv = 0;
			IoReg_Write32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_CTRL_reg, i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.regValue);
		}
		else if(target != I3DDMA_COLOR_RGB) {
			i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.regValue = IoReg_Read32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_CTRL_reg);
			i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.en_rgb2yuv = 1;
			i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.sel_rgb = 1;
			i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.set_uv_out_offset = 1;
			IoReg_Write32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_CTRL_reg, i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.regValue);
		} else {
			i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.regValue = IoReg_Read32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_CTRL_reg);
			i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.en_rgb2yuv = 0;
			i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.sel_rgb = 0;
			i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.set_uv_out_offset = 0;
			IoReg_Write32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_CTRL_reg, i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.regValue);
		}
	} else {
		i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.regValue = IoReg_Read32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_CTRL_reg);
		i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.en_rgb2yuv = 0;
		i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.sel_rgb = 0;
		i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.set_uv_out_offset = 0;
		IoReg_Write32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_CTRL_reg, i3ddma_rgb2yuv_dither_4xxto4xx_hdmi_rgb2yuv_ctrl_reg.regValue);
	}

	/*i3ddma capture setting*/
	h_act_len  = (DEPTH2BITS[timing->depth] * (timing->h_act_len /4) * components[target] + 127) / 128;
	rem = 12;
	len = 32;
	//num = (h_act_len - rem) / len;
	rem = (h_act_len - rem) % len + rem;

	//i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
	//i3ddma_i3ddma_enable_reg.cap_en = 0;
	//IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_i3ddma_enable_reg.regValue);

	h3ddma_cap0_wr_dma_pxltobus_reg.regValue = IoReg_Read32(H3DDMA_CAP0_WR_DMA_pxltobus_reg);
	h3ddma_cap0_wr_dma_pxltobus_reg.cap0_pixel_encoding = encodemap[target];
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_pxltobus_reg, h3ddma_cap0_wr_dma_pxltobus_reg.regValue);
#if 0
	i3ddma_cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_Cap_CTL0_reg);
	i3ddma_cap_ctl0_reg.cap_write_len = len;
	i3ddma_cap_ctl0_reg.cap_write_rem = rem;
	IoReg_Write32(H3DDMA_Cap_CTL0_reg, i3ddma_cap_ctl0_reg.regValue);

	i3ddma_cap_ctl1_reg.regValue = IoReg_Read32(H3DDMA_Cap_CTL1_reg);
	i3ddma_cap_ctl1_reg.cap_water_lv = len;
	i3ddma_cap_ctl1_reg.cap_write_num = num;
	IoReg_Write32(H3DDMA_Cap_CTL1_reg, i3ddma_cap_ctl1_reg.regValue);

	i3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
	i3ddma_i3ddma_enable_reg.cap_en = 1;
	IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_i3ddma_enable_reg.regValue);
#endif
	return;
}
#endif


static unsigned char enable_idma_compression = FALSE;
static unsigned char idma_compression_bits = 0;
static unsigned char idma_compression_mode = I3DDMA_COMP_NONE_MODE;

unsigned char dvrif_i3ddma_compression_get_enable(void)
{
	return enable_idma_compression;
}
void dvrif_i3ddma_compression_set_enable(unsigned char enable)
{
	enable_idma_compression = enable;
}

unsigned char dvrif_i3ddma_get_compression_bits(void)
{
	return idma_compression_bits;
}
void dvrif_i3ddma_set_compression_bits(unsigned char comp_bits)
{
	idma_compression_bits = comp_bits;
}

unsigned char dvrif_i3ddma_get_compression_mode(void)
{
	return idma_compression_mode;
}

void dvrif_i3ddma_set_compression_mode(unsigned char comp_mode)
{
	idma_compression_mode = comp_mode;
}

#define I3DDMA_PQC_LINE_MARGIN 6
extern unsigned char pc_mode_run_444(void);
extern unsigned char pc_mode_run_422(void);
/***
force 444 :
cmp_data_format = 0 & do_422_mode = 2
dynamic 444 :
cmp_data_format = 0 & do_422_mode = 0
force 422 :
cmp_data_format = 1 & do_422_mode = 0

***/
void dvrif_i3ddma_comp_setting(UINT8 enable_compression, UINT16 comp_wid, UINT16 comp_len, UINT8 comp_ratio)
{
	//i3ddma->vodma use PQC and PQDC from merlin3
	h3ddma_h3ddma_pq_cmp_RBUS h3ddma_h3ddma_pq_cmp_reg;
	h3ddma_h3ddma_pq_cmp_pair_RBUS h3ddma_h3ddma_pq_cmp_pair_reg;
	h3ddma_h3ddma_pq_cmp_bit_RBUS h3ddma_h3ddma_pq_cmp_bit_reg;
        //>>>> [start]K5LG-1492:modify for i3ddma and mdomain compression
	h3ddma_h3ddma_pq_cmp_enable_RBUS h3ddma_h3ddma_pq_cmp_enable_reg;
	h3ddma_h3ddma_pq_cmp_allocate_RBUS h3ddma_h3ddma_pq_cmp_allocate_reg;
	h3ddma_h3ddma_pq_cmp_poor_RBUS h3ddma_h3ddma_pq_cmp_poor_Reg;

	h3ddma_h3ddma_pq_cmp_guarantee_RBUS h3ddma_h3ddma_pq_cmp_guarantee_reg;
	h3ddma_h3ddma_pq_cmp_blk0_add0_RBUS h3ddma_h3ddma_pq_cmp_blk0_add0_reg;
	h3ddma_h3ddma_pq_cmp_blk0_add1_RBUS h3ddma_h3ddma_pq_cmp_blk0_add1_reg;

	h3ddma_h3ddma_pq_cmp_balance_RBUS h3ddma_h3ddma_pq_cmp_balance_reg;
	h3ddma_h3ddma_pq_cmp_smooth_RBUS h3ddma_h3ddma_pq_cmp_smooth_reg;
	h3ddma_h3ddma_pq_cmp_qp_st_RBUS h3ddma_pq_cmp_qp_st_reg;
	h3ddma_h3ddma_pq_cmp_fifo_st1_RBUS h3ddma_pq_cmp_fifo_st1_reg;

    vodma_vodma_rtl_improve_RBUS vodma_vodma_rtl_improve_reg;
	vodma_vodma_pq_decmp_RBUS vodma_vodma_pq_decmp_reg;
    vodma_vodma_pq_decmp_pair_RBUS vodma_vodma_pq_decmp_pair_reg;
	vodma_vodma_pq_decmp_sat_en_RBUS vodma_vodma_pq_decmp_sat_en_reg;

    unsigned int cmp_line_sum_bit = 0;


	rtd_pr_i3ddma_info("[zhaodong]enable_compression = %d, comp_wid = %d, comp_len = %d, comp_mode=%d, comp_ratio = %d\n",enable_compression, comp_wid, comp_len, dvrif_i3ddma_get_compression_mode(), comp_ratio);

	if(enable_compression == TRUE){
		/*
			width need to align 32
		*/

		if((comp_wid % 32) != 0){
			//drop bits
			//IoReg_Write32(MDOMAIN_DISP_DDR_MainAddrDropBits_reg, (32 - (comp_wid % 32)));
			comp_wid = comp_wid + (32 - (comp_wid % 32));
		}

        cmp_line_sum_bit = calc_i3ddma_comp_line_sum_bit(comp_wid, comp_ratio);


		//i3ddma capture compression setting
		h3ddma_h3ddma_pq_cmp_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_reg);
		h3ddma_h3ddma_pq_cmp_reg.cmp_height = comp_len;//set length
		h3ddma_h3ddma_pq_cmp_reg.cmp_width_div32 = comp_wid / 32;//set width
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_reg, h3ddma_h3ddma_pq_cmp_reg.regValue);

		//compression bits setting
		h3ddma_h3ddma_pq_cmp_bit_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_BIT_reg);
		if(dvrif_i3ddma_get_compression_mode() == I3DDMA_COMP_LINE_MODE)
		{
			h3ddma_h3ddma_pq_cmp_bit_reg.frame_limit_bit = comp_ratio*4;
			//h3ddma_h3ddma_pq_cmp_bit_reg.line_limit_bit = comp_ratio; // Merlin8 remove
		}
		else{//frame mode, line limit bit need to add margin to avoid the peak bandwidth
			h3ddma_h3ddma_pq_cmp_bit_reg.frame_limit_bit = comp_ratio*4;
			//h3ddma_h3ddma_pq_cmp_bit_reg.line_limit_bit = comp_ratio + I3DDMA_PQC_LINE_MARGIN; // Merlin8 remove
		}
		h3ddma_h3ddma_pq_cmp_bit_reg.block_limit_bit = 0x3f;//PQC fine tune setting
		h3ddma_h3ddma_pq_cmp_bit_reg.first_line_more_bit = 0x20;
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_BIT_reg, h3ddma_h3ddma_pq_cmp_bit_reg.regValue);

		//compression other setting
		h3ddma_h3ddma_pq_cmp_pair_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_PAIR_reg);

#if 0
		if(pc_mode_run_444())
		{//hdmi input yuv444 pc mode
			h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_bit_width = 1; //10 bits
		} else {
			if(i3ddmaCtrl.pgen_timing && (i3ddmaCtrl.pgen_timing->depth == I3DDMA_COLOR_DEPTH_10B))
				h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_bit_width = 1; //10 bits
			else
				h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_bit_width = 0;//8 bits
		}
#endif
		h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_bit_width = 1; //10 bits


		#if 0
		if(i3ddmaCtrl.pgen_timing->color == I3DDMA_COLOR_YUV422){
			//h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_format = 1;//422
			h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_color = 1;
		}else{
			//h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_format = 0;//444
			h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_color = 1;//currently no case rgb channel
		}
		#endif
		#if 0
		if(get_vsc_run_pc_mode()){
			if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI) && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == SCALER_INPUT_COLOR_YUV422))
			{//hdmi input yuv422 pc mode
				h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_format = 1; //pc422
			}
			else
			{
				h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_format = 0;//pc444
			}
		}
		else{
			h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_format = 0;
		}
		#endif

		if(get_vpq_check_pc_rgb444_mode_flag() == _ENABLE)
		{
			h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_color = 0; // RGB , pure RGB case
			h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_format = 0; // 444 , pure RGB case
		}
		else
		{
			h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_color = 1; // YUV
			if(get_vsc_run_pc_mode())
			{
				if(pc_mode_run_444())
				{
					h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_format = 0; // 444
				}
				else
				{
					h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_format = 1; // 422
				}
			}
			else
			{
				if(i3ddmaCtrl[I3DDMA_DEV].ptx_timing && i3ddmaCtrl[I3DDMA_DEV].ptx_timing->color == I3DDMA_COLOR_YUV422)
				{
					h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_format = 1; // 422
				}
				else
				{
					h3ddma_h3ddma_pq_cmp_pair_reg.cmp_data_format = 0; // 444
				}
			}
		}
		h3ddma_h3ddma_pq_cmp_pair_reg.cmp_pair_para = 1;

		if(dvrif_i3ddma_get_compression_mode() == I3DDMA_COMP_LINE_MODE)
		{
			// use line mode
			h3ddma_h3ddma_pq_cmp_pair_reg.cmp_line_mode = 1;
            h3ddma_h3ddma_pq_cmp_pair_reg.cmp_line_sum_bit = cmp_line_sum_bit;
			//h3ddma_h3ddma_pq_cmp_pair_reg.two_line_prediction_en = 0;
		}
		else
		{
			//use frame mode
			h3ddma_h3ddma_pq_cmp_pair_reg.cmp_line_mode = 0;
            h3ddma_h3ddma_pq_cmp_pair_reg.cmp_line_sum_bit = 0;
			//h3ddma_h3ddma_pq_cmp_pair_reg.two_line_prediction_en = 1;
		}
#ifndef BUILD_QUICK_SHOW
        if(get_vo_vflip_flag(SLR_MAIN_DISPLAY)){
            h3ddma_h3ddma_pq_cmp_pair_reg.two_line_prediction_en = 0;
        }
		else
#endif
        {
			if ((dvrif_i3ddma_get_compression_mode() == I3DDMA_COMP_LINE_MODE) && (get_vsc_data_path_indicator(DRIVER_FREERUN_PATTERN) == 1)) {
				h3ddma_h3ddma_pq_cmp_pair_reg.two_line_prediction_en = 0;
			} else {
				h3ddma_h3ddma_pq_cmp_pair_reg.two_line_prediction_en = 0;
			}
		}

		h3ddma_h3ddma_pq_cmp_pair_reg.cmp_qp_mode = 0;
		//h3ddma_h3ddma_pq_cmp_pair_reg.cmp_dic_mode_en = 0; // cmp_dic_mode_en no effect
		h3ddma_h3ddma_pq_cmp_pair_reg.cmp_jump4_en = 1;
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_PAIR_reg, h3ddma_h3ddma_pq_cmp_pair_reg.regValue);

                //>>>>[start]K5LG-1492:modify for i3ddma and mdomain compression
		h3ddma_h3ddma_pq_cmp_enable_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_ENABLE_reg);
                //RGB444 PQC mode do_422_mode=2,comment from DIC Tien-Hung
		#if 0
        if(get_vsc_run_pc_mode()){
			if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI) && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == SCALER_INPUT_COLOR_YUV422))
			{//hdmi input yuv422 pc mode
				h3ddma_h3ddma_pq_cmp_enable_reg.do_422_mode = 1;//422 format
				h3ddma_h3ddma_pq_cmp_enable_reg.g_ratio = 16;
			}
			else
			{
				h3ddma_h3ddma_pq_cmp_enable_reg.do_422_mode = 2;//444 format
				h3ddma_h3ddma_pq_cmp_enable_reg.g_ratio = 12;
			}
		}
		else{
			h3ddma_h3ddma_pq_cmp_enable_reg.do_422_mode = 0;//dynamic mode 444/422 dynamic
			h3ddma_h3ddma_pq_cmp_enable_reg.g_ratio = 14;
		}
		#endif

		if(get_vsc_run_pc_mode()) {
			if(pc_mode_run_444()) {
				h3ddma_h3ddma_pq_cmp_enable_reg.do_422_mode = 2;//444 format
				h3ddma_h3ddma_pq_cmp_enable_reg.g_ratio = 12;
			} else {
				h3ddma_h3ddma_pq_cmp_enable_reg.do_422_mode = 0;//422 format
				h3ddma_h3ddma_pq_cmp_enable_reg.g_ratio = 16;
			}
		} else {
			if(i3ddmaCtrl[I3DDMA_DEV].ptx_timing && i3ddmaCtrl[I3DDMA_DEV].ptx_timing->color == I3DDMA_COLOR_YUV422){
				h3ddma_h3ddma_pq_cmp_enable_reg.do_422_mode = 0;//422 format
			}else {
				if((comp_wid <= 1920) && (comp_len <= 1080))//<=2k normal mode 444
					h3ddma_h3ddma_pq_cmp_enable_reg.do_422_mode = 2;//444 format
				else
					h3ddma_h3ddma_pq_cmp_enable_reg.do_422_mode = 0;//dynamic mode 444/422 dynamic
			}
			h3ddma_h3ddma_pq_cmp_enable_reg.g_ratio = 14;
		}

		h3ddma_h3ddma_pq_cmp_enable_reg.first_predict_nc_en = 1;
		h3ddma_h3ddma_pq_cmp_enable_reg.first_predict_nc_mode = 1;
		h3ddma_h3ddma_pq_cmp_enable_reg.guarantee_max_qp_en = 0;
		h3ddma_h3ddma_pq_cmp_enable_reg.first_line_more_en  = 1;
		h3ddma_h3ddma_pq_cmp_enable_reg.blk0_add_en = 1;
		h3ddma_h3ddma_pq_cmp_enable_reg.blk0_add_half_en = 1;
		h3ddma_h3ddma_pq_cmp_enable_reg.add_last2blk_over_curve_bit = 1;
		h3ddma_h3ddma_pq_cmp_enable_reg.balance_en = 1;
		h3ddma_h3ddma_pq_cmp_enable_reg.variation_maxmin_en = 0;
		h3ddma_h3ddma_pq_cmp_enable_reg.dynamic_allocate_ratio_en = 1;
		h3ddma_h3ddma_pq_cmp_enable_reg.dynamic_sum_line_rst = 1;
		h3ddma_h3ddma_pq_cmp_enable_reg.not_enough_bit_do_422_en = 1;
		h3ddma_h3ddma_pq_cmp_enable_reg.not_rich_do_422_en = 1;
		h3ddma_h3ddma_pq_cmp_enable_reg.rb_lossy_do_422_en = 1;
		h3ddma_h3ddma_pq_cmp_enable_reg.poor_limit_min_qp_en = 1;
		//h3ddma_h3ddma_pq_cmp_enable_reg.cmp_ctrl_para0 = 0x10;
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_ENABLE_reg,h3ddma_h3ddma_pq_cmp_enable_reg.regValue);

		h3ddma_h3ddma_pq_cmp_allocate_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_ALLOCATE_reg);
		//RGB444 setting for PQC ratio_max and ratio_min, comment from DIC Tien-Hung
		if (get_vsc_run_pc_mode()){
			if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI) && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_COLOR_SPACE) == SCALER_INPUT_COLOR_YUV422))
			{//hdmi input yuv422 pc mode
				h3ddma_h3ddma_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
				h3ddma_h3ddma_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
			}
			else
			{
				h3ddma_h3ddma_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 13;
				h3ddma_h3ddma_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 9;
			}
		}
		else{
			h3ddma_h3ddma_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
			h3ddma_h3ddma_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
		}

		h3ddma_h3ddma_pq_cmp_allocate_reg.dynamic_allocate_less = 2;
		h3ddma_h3ddma_pq_cmp_allocate_reg.dynamic_allocate_more = 2;
		h3ddma_h3ddma_pq_cmp_allocate_reg.dynamic_allocate_line = 4;
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_ALLOCATE_reg, h3ddma_h3ddma_pq_cmp_allocate_reg.regValue);

		h3ddma_h3ddma_pq_cmp_poor_Reg.regValue=IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_POOR_reg);

		#if 0
		if(pc_mode_run_444())
		{//hdmi input yuv444 pc mode
			h3ddma_h3ddma_pq_cmp_poor_Reg.poor_limit_th_qp4 = 0x14;
			h3ddma_h3ddma_pq_cmp_poor_Reg.poor_limit_th_qp3 = 0x06;
			h3ddma_h3ddma_pq_cmp_poor_Reg.poor_limit_th_qp2 = 0x03;
			h3ddma_h3ddma_pq_cmp_poor_Reg.poor_limit_th_qp1 = 0x03;
		} else {
			h3ddma_h3ddma_pq_cmp_poor_Reg.poor_limit_th_qp4 = 0x14;
			h3ddma_h3ddma_pq_cmp_poor_Reg.poor_limit_th_qp3 = 0x0c;
			h3ddma_h3ddma_pq_cmp_poor_Reg.poor_limit_th_qp2 = 0x06;
			h3ddma_h3ddma_pq_cmp_poor_Reg.poor_limit_th_qp1 = 0x03;
		}
		#endif

		h3ddma_h3ddma_pq_cmp_poor_Reg.poor_limit_th_qp1 = 3;
		h3ddma_h3ddma_pq_cmp_poor_Reg.poor_limit_th_qp2 = 6;
		h3ddma_h3ddma_pq_cmp_poor_Reg.poor_limit_th_qp3 = 12;
		h3ddma_h3ddma_pq_cmp_poor_Reg.poor_limit_th_qp4 = 20;
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_POOR_reg, h3ddma_h3ddma_pq_cmp_poor_Reg.regValue);

		// Guarantee
		h3ddma_h3ddma_pq_cmp_guarantee_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_Guarantee_reg);
		h3ddma_h3ddma_pq_cmp_guarantee_reg.guarantee_max_g_qp = 0;
		h3ddma_h3ddma_pq_cmp_guarantee_reg.guarantee_max_rb_qp = 0;
		// Dictionary
		//h3ddma_h3ddma_pq_cmp_guarantee_reg.dic_mode_qp_th_g = 0;
		//h3ddma_h3ddma_pq_cmp_guarantee_reg.dic_mode_qp_th_rb = 0;
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_Guarantee_reg, h3ddma_h3ddma_pq_cmp_guarantee_reg.regValue);

		// Quota distribution
		h3ddma_h3ddma_pq_cmp_blk0_add0_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_BLK0_ADD0_reg);
		h3ddma_h3ddma_pq_cmp_blk0_add0_reg.blk0_add_value0 = 1;
		h3ddma_h3ddma_pq_cmp_blk0_add0_reg.blk0_add_value16 = 8;
		h3ddma_h3ddma_pq_cmp_blk0_add0_reg.blk0_add_value32 = 16;

		if(pc_mode_run_444())
		{//hdmi input yuv444 pc mode
			// Dictionary
			//h3ddma_h3ddma_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 1; //2p remove it
		}
		else
		{
			// Dictionary
			//h3ddma_h3ddma_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 0;
		}
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_BLK0_ADD0_reg, h3ddma_h3ddma_pq_cmp_blk0_add0_reg.regValue);


		h3ddma_h3ddma_pq_cmp_blk0_add1_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_BLK0_ADD1_reg);
		h3ddma_h3ddma_pq_cmp_blk0_add1_reg.blk0_add_value1 = 1;
		h3ddma_h3ddma_pq_cmp_blk0_add1_reg.blk0_add_value2 = 2;
		h3ddma_h3ddma_pq_cmp_blk0_add1_reg.blk0_add_value4 = 4;
		h3ddma_h3ddma_pq_cmp_blk0_add1_reg.blk0_add_value8 = 6;
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_BLK0_ADD1_reg, h3ddma_h3ddma_pq_cmp_blk0_add1_reg.regValue);

		// Balance
		h3ddma_h3ddma_pq_cmp_balance_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_BALANCE_reg);
		h3ddma_h3ddma_pq_cmp_balance_reg.balance_g_give = 3;
		h3ddma_h3ddma_pq_cmp_balance_reg.balance_g_ud_th = 3;
		h3ddma_h3ddma_pq_cmp_balance_reg.balance_g_ov_th = 0;
		h3ddma_h3ddma_pq_cmp_balance_reg.balance_rb_give = 3;
		h3ddma_h3ddma_pq_cmp_balance_reg.balance_rb_ud_th = 3;
		h3ddma_h3ddma_pq_cmp_balance_reg.balance_rb_ov_th = 0;

		// smooth
		h3ddma_h3ddma_pq_cmp_balance_reg.variation_maxmin_th = 30;
		h3ddma_h3ddma_pq_cmp_balance_reg.variation_maxmin_th2 = 3;
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_BALANCE_reg, h3ddma_h3ddma_pq_cmp_balance_reg.regValue);

		// Smooth
		h3ddma_h3ddma_pq_cmp_smooth_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_SMOOTH_reg);
		h3ddma_h3ddma_pq_cmp_smooth_reg.variation_near_num_th = 4;
		h3ddma_h3ddma_pq_cmp_smooth_reg.variation_value_th = 3;
		h3ddma_h3ddma_pq_cmp_smooth_reg.variation_num_th = 3;

		// Dynamically 422 decision
		h3ddma_h3ddma_pq_cmp_smooth_reg.rb_lossy_do_422_qp_level = 0;
		h3ddma_h3ddma_pq_cmp_smooth_reg.not_rich_do_422_th = 4;
		h3ddma_h3ddma_pq_cmp_smooth_reg.not_enough_bit_do_422_qp = 5;

		// DIctionary
		//h3ddma_h3ddma_pq_cmp_smooth_reg.dic_mode_entry_th = 15;
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_SMOOTH_reg, h3ddma_h3ddma_pq_cmp_smooth_reg.regValue);

		h3ddma_pq_cmp_qp_st_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_QP_ST_reg);
		//h3ddma_pq_cmp_qp_st_reg.cmp_ctrl_para1 = 0x44;
		//h3ddma_pq_cmp_qp_st_reg.cmp_ctrl_para2 = 0xD0;
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_QP_ST_reg, h3ddma_pq_cmp_qp_st_reg.regValue);

		h3ddma_pq_cmp_fifo_st1_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_FIFO_ST1_reg);
		// h3ddma_pq_cmp_fifo_st1_reg.cmp_ctrl_para3 = 0x0; // Merlin8 reserved
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_FIFO_ST1_reg, h3ddma_pq_cmp_fifo_st1_reg.regValue);

		//vodma de-compression setting
		vodma_vodma_pq_decmp_reg.regValue = IoReg_Read32(VODMA_VODMA_PQ_DECMP_reg);
		vodma_vodma_pq_decmp_reg.decmp_height = comp_len;
		vodma_vodma_pq_decmp_reg.decmp_width_div32 = comp_wid / 32;
		IoReg_Write32(VODMA_VODMA_PQ_DECMP_reg, vodma_vodma_pq_decmp_reg.regValue);

		//de-compression bits setting
        vodma_vodma_pq_decmp_pair_reg.regValue = IoReg_Read32(VODMA_VODMA_PQ_DECMP_PAIR_reg);

		if(dvrif_i3ddma_get_compression_mode() == I3DDMA_COMP_LINE_MODE)
		{
        		vodma_vodma_pq_decmp_pair_reg.decmp_line_mode = 1;
                vodma_vodma_pq_decmp_pair_reg.decmp_line_sum_bit = cmp_line_sum_bit;//HDMI_INFO->comps_bits; // Constrain: decmp_line_sum_bit = cmp_line_sum_bit
				//vodma_vodma_pq_decmp_pair_reg.decmp_two_line_prediction_en = 0;
		}
		else
		{
				vodma_vodma_pq_decmp_pair_reg.decmp_line_mode = 0;
                vodma_vodma_pq_decmp_pair_reg.decmp_line_sum_bit = 0;
				//vodma_vodma_pq_decmp_pair_reg.decmp_two_line_prediction_en = 1;
		}
#ifndef BUILD_QUICK_SHOW
		if(get_vo_vflip_flag(SLR_MAIN_DISPLAY))
		{
			vodma_vodma_pq_decmp_pair_reg.decmp_two_line_prediction_en = 0;
		}
		else
#endif
        {
			if ((dvrif_i3ddma_get_compression_mode()) && (get_vsc_data_path_indicator(DRIVER_FREERUN_PATTERN) == 1 )) {
				vodma_vodma_pq_decmp_pair_reg.decmp_two_line_prediction_en = 0;
			} else {
				vodma_vodma_pq_decmp_pair_reg.decmp_two_line_prediction_en = 0;
			}
		}

#if 0
		if (pc_mode_run_444()) {
			vodma_vodma_pq_decmp_pair_reg.decmp_data_bit_width = 1;
	 	} else {
        	vodma_vodma_pq_decmp_pair_reg.decmp_data_bit_width = (HDMI_INFO->data_bit_width == 8)? 0: 1 ; // 8bits, 10bits
        }
#endif
		vodma_vodma_pq_decmp_pair_reg.decmp_data_bit_width = 1;

			//decompression YUV444
		#if 0
	        if (HDMI_INFO->chroma_fmt == VO_CHROMA_YUV422) {
	            vodma_vodma_pq_decmp_pair_reg.decmp_data_format = 1;//422
	            vodma_vodma_pq_decmp_pair_reg.decmp_data_color = 1;
	        } else {
	            vodma_vodma_pq_decmp_pair_reg.decmp_data_format = 0;//444
	            vodma_vodma_pq_decmp_pair_reg.decmp_data_color = 1; //currently no case rgb channel
	        }
		#endif

		if(get_vpq_check_pc_rgb444_mode_flag() == _ENABLE)
		{
			vodma_vodma_pq_decmp_pair_reg.decmp_data_color = 0; // RGB , pure RGB case
			vodma_vodma_pq_decmp_pair_reg.decmp_data_format = 0; // 444 , pure RGB case
		}
		else
		{
			vodma_vodma_pq_decmp_pair_reg.decmp_data_color = 1; // YUV
			if(get_vsc_run_pc_mode())
			{
				if(pc_mode_run_444())
				{
					vodma_vodma_pq_decmp_pair_reg.decmp_data_format = 0; // 444
				}
				else
				{
					vodma_vodma_pq_decmp_pair_reg.decmp_data_format = 1; // 422
				}
			}
			else
			{
				if(i3ddmaCtrl[I3DDMA_DEV].ptx_timing && i3ddmaCtrl[I3DDMA_DEV].ptx_timing->color == I3DDMA_COLOR_YUV422)
				{
					vodma_vodma_pq_decmp_pair_reg.decmp_data_format = 1; // 422
				}
				else
				{
					vodma_vodma_pq_decmp_pair_reg.decmp_data_format = 0; // 444
				}
			}
		}

		#if 0
		if (pc_mode_run_444())
		{
			vodma_vodma_pq_decmp_pair_reg.decmp_data_format = 0;//444
		}
		else
		{
			vodma_vodma_pq_decmp_pair_reg.decmp_data_format = 1;//422
		}
		#endif

		vodma_vodma_pq_decmp_pair_reg.decmp_qp_mode = 0;
		//FIXME: disable dic mode
		//vodma_vodma_pq_decmp_pair_reg.decmp_dic_mode_en = 0;
		vodma_vodma_pq_decmp_pair_reg.decmp_jump4_en = 1;
		vodma_vodma_pq_decmp_pair_reg.decmp_pair_para = 1;
	    IoReg_Write32(VODMA_VODMA_PQ_DECMP_PAIR_reg, vodma_vodma_pq_decmp_pair_reg.regValue);

		vodma_vodma_pq_decmp_sat_en_reg.regValue = IoReg_Read32(VODMA_VODMA_PQ_DECMP_SAT_EN_reg);
		//vodma_vodma_pq_decmp_sat_en_reg.decmp_ctrl_para = 1;
		vodma_vodma_pq_decmp_sat_en_reg.saturation_en = 1;
		IoReg_Write32(VODMA_VODMA_PQ_DECMP_SAT_EN_reg, vodma_vodma_pq_decmp_sat_en_reg.regValue);

        //enable de-compression
        vodma_vodma_pq_decmp_reg.regValue = IoReg_Read32(VODMA_VODMA_PQ_DECMP_reg);
        vodma_vodma_pq_decmp_reg.decmp_en = 1;
        IoReg_Write32(VODMA_VODMA_PQ_DECMP_reg, vodma_vodma_pq_decmp_reg.regValue);

		//enable compression
		h3ddma_h3ddma_pq_cmp_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_reg);
		h3ddma_h3ddma_pq_cmp_reg.cmp_en = 1;
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_reg, h3ddma_h3ddma_pq_cmp_reg.regValue);

        vodma_vodma_rtl_improve_reg.regValue = IoReg_Read32(VODMA_VODMA_RTL_IMPROVE_reg);
        vodma_vodma_rtl_improve_reg.pqdc_pair_sel = 1; // sync pq decmp settings
        IoReg_Write32(VODMA_VODMA_RTL_IMPROVE_reg, vodma_vodma_rtl_improve_reg.regValue);
	}
	else{
		//disable compression
		h3ddma_h3ddma_pq_cmp_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_reg);
		h3ddma_h3ddma_pq_cmp_reg.cmp_en = 0;
		IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_reg, h3ddma_h3ddma_pq_cmp_reg.regValue);

		//disable de-compression
		vodma_vodma_pq_decmp_reg.regValue = IoReg_Read32(VODMA_VODMA_PQ_DECMP_reg);
		vodma_vodma_pq_decmp_reg.decmp_en = 0;
		IoReg_Write32(VODMA_VODMA_PQ_DECMP_reg, vodma_vodma_pq_decmp_reg.regValue);
	}
}

void h3ddma_set_capture_enable(unsigned char chanel,unsigned char enable)
{
	//decide i3ddma cpture enable or disable
	h3ddma_cap0_cti_dma_wr_ctrl_RBUS h3ddma_cap0_cti_dma_wr_ctrl_reg;
	h3ddma_cap1_cti_dma_wr_ctrl_RBUS h3ddma_cap1_cti_dma_wr_ctrl_reg;
	h3ddma_cap2_cti_dma_wr_ctrl_RBUS h3ddma_cap2_cti_dma_wr_ctrl_reg;
	h3ddma_cap3_cti_dma_wr_ctrl_RBUS h3ddma_cap3_cti_dma_wr_ctrl_reg;

	if(chanel==0)
	{
		h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg);
		h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_enable= enable;
		IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg, h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue);
	}
	else if(chanel==1)
	{
		h3ddma_cap1_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg);
		h3ddma_cap1_cti_dma_wr_ctrl_reg.cap1_dma_enable= enable;
		IoReg_Write32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg, h3ddma_cap1_cti_dma_wr_ctrl_reg.regValue);
	}
	else if(chanel==2)
	{
		h3ddma_cap2_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP2_CTI_DMA_WR_Ctrl_reg);
		h3ddma_cap2_cti_dma_wr_ctrl_reg.cap2_dma_enable= enable;
		IoReg_Write32(H3DDMA_CAP2_CTI_DMA_WR_Ctrl_reg, h3ddma_cap2_cti_dma_wr_ctrl_reg.regValue);
	}
	else if(chanel==3)
	{
		h3ddma_cap3_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP3_CTI_DMA_WR_Ctrl_reg);
		h3ddma_cap3_cti_dma_wr_ctrl_reg.cap3_dma_enable= enable;
		IoReg_Write32(H3DDMA_CAP3_CTI_DMA_WR_Ctrl_reg, h3ddma_cap3_cti_dma_wr_ctrl_reg.regValue);
	}
}

#ifndef BUILD_QUICK_SHOW

#ifdef CONFIG_ENABLE_HDMI_NN
/*
Total 6 buffers,i3ddma and sub each have 3 buffer

			*************
			*			*
			*	i3ddma	*
			*  y buffer *
			*			*
			*************
			*************
			*	i3ddma	*
			*  c buffer *
			*************
			*************
			*			*
			*	 sub    *
			*  y buffer *
			*			*
			*************
			*************
			*	 sub 	*
			*  c buffer *
			*************

*/
unsigned char h3ddma_allocate_nv12_mermory(UINT32 width,UINT32 height,unsigned char buffernub)
{
    unsigned int size_video;
	//unsigned long  vir_addr, vir_addr_noncache;
	unsigned long phyaddr;
	int i;
	unsigned int allocSize = 0;

	// Reset buffer index
	g_ucNNReadIdx = 0;
	g_ucNNWriteIdx = 0;

	if(g_tNN_Cap_Buffer[0].phyaddr != 0)
	{
		rtd_pr_i3ddma_info("HDMI not disconnect and memory not free,so reuse buffer");
		return TRUE;
	}

	size_video = ALIGN((width*height),__4KPAGE);

	allocSize = (size_video + size_video / 2) * buffernub;

	rtd_pr_i3ddma_info("h3ddma_allocate_nv12_mermory, widh=%d, height=%d, buffernub=%d, size_video=%d, allocSize=%d\n", width, height, buffernub, size_video, allocSize);

	//rtd_pr_i3ddma_info("origianl  [h3ddma nv12] buf[0]=%lx(%d KB), phy(%lx) \n", (unsigned long)i3ddmaCtrl.cap_buffer[5].cache, i3ddmaCtrl.cap_buffer[5].size >> 10, phyaddr);
	//g_cacheaddr=(unsigned long)dvr_malloc_uncached_specific(allocSize, GFP_DCU1_LIMIT,(void **)&g_uncacheaddr);
	//phyaddr = (unsigned long)dvr_to_phys((void*)g_cacheaddr);
	phyaddr = pli_malloc(allocSize, GFP_DCU2_FIRST);

	//i3ddmaCtrl.cap_buffer[5].cache = (void *)g_cacheaddr;
	//i3ddmaCtrl.cap_buffer[5].uncache = (unsigned char *)g_uncacheaddr;

	//rtd_pr_i3ddma_info("cacheaddr=%lx, uncacheaddr=%lx\n", g_cacheaddr, g_uncacheaddr);

	if(phyaddr==INVALID_VAL)
	{
		rtd_pr_i3ddma_emerg("h3ddma_allocate_nv12_mermory faile %lx\n", phyaddr);
		return FALSE;
	}

	//i3ddma cap buffer address
	for(i=0;i<NN_CAP_BUFFER_NUM;i++)
	{
		//i3ddmaCtrl.cap_buffer[i].phyaddr =  (phyaddr + i*size_video);
		//i3ddmaCtrl.cap_buffer[i].size=size_video;
		g_tNN_Cap_Buffer[i].phyaddr = (phyaddr + i*size_video);
		g_tNN_Cap_Buffer[i].size = size_video;
		rtd_pr_i3ddma_info("face detec start address=0x%lx\n", g_tNN_Cap_Buffer[i].phyaddr);
	}

	//rtd_pr_i3ddma_info("new [h3ddma nv12] buf[%d]=%lx(%d KB), phy(%lx) \n", i, (unsigned long)i3ddmaCtrl.cap_buffer[5].cache, i3ddmaCtrl.cap_buffer[5].size >> 10, (unsigned long)i3ddmaCtrl.cap_buffer[5].phyaddr);
	rtd_pr_i3ddma_info("new [h3ddma nv12] buf[%d]=%lx(%d KB), phy(%lx) \n", i, (unsigned long)g_tNN_Cap_Buffer[0].cache, g_tNN_Cap_Buffer[0].size >> 10, (unsigned long)g_tNN_Cap_Buffer[0].phyaddr);

	IoReg_Write32(H3DDMA_CAP1_CTI_DMA_WR_Rule_check_low_reg, phyaddr);
	IoReg_Write32(H3DDMA_CAP1_CTI_DMA_WR_Rule_check_up_reg, phyaddr + size_video * NN_CAP_BUFFER_NUM);

	IoReg_Write32(H3DDMA_CAP2_CTI_DMA_WR_Rule_check_low_reg, phyaddr + size_video * NN_CAP_BUFFER_NUM);
	IoReg_Write32(H3DDMA_CAP2_CTI_DMA_WR_Rule_check_up_reg, phyaddr + (size_video + size_video/2) * NN_CAP_BUFFER_NUM);

	//dcmt_clear_monitor(8);
	//h3mdda_set_dcmt(3);

	return TRUE;

}

void h3ddma_free_nv12_mermory(void)
{
	int i;

	rtd_pr_i3ddma_info("!!!h3ddma_free_nv12_mermory free nn memory %lx\n", g_tNN_Cap_Buffer[0].phyaddr);

	if(g_tNN_Cap_Buffer[0].phyaddr !=0 )
	{
		pli_free(g_tNN_Cap_Buffer[0].phyaddr);
	}

	for(i=0;i<NN_CAP_BUFFER_NUM;i++)
	{
		g_tNN_Cap_Buffer[i].phyaddr =  0;
		g_tNN_Cap_Buffer[i].size=0;
	}
	g_tNN_Cap_Buffer[0].cache=NULL;
	g_tNN_Cap_Buffer[0].uncache=NULL;
	g_ucCapEnableMask = 0;
	h3ddma_nn_set_recfg_flag(FALSE);

}
void h3ddma_set_cap_enable_mask(unsigned char mask)
{
	if(mask > I3DDMA_CAP_EANBLE_MAX_MASK)
		return;

	g_ucCapEnableMask |= _BIT(mask);
}

void h3ddma_clear_cap_enable_mask(unsigned char mask)
{
	if(mask > I3DDMA_CAP_EANBLE_MAX_MASK)
		return;

	g_ucCapEnableMask &= ~(_BIT(mask));
}

unsigned char h3ddma_get_cap_enable_mask(void)
{
	return g_ucCapEnableMask;
}

//open i3ddma nn cap
void h3ddma_open_i3ddma_capture(unsigned char mask)
{
#ifndef UT_flag
	if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_SEARCH) || (scalerAI_get_AIPQ_mode_enable_flag() == 0) ||
		(mask > I3DDMA_CAP_EANBLE_MAX_MASK))
		return;
#endif // #ifndef UT_flag
	down(get_force_i3ddma_semaphore());
	IDDMA_Set_DMAWorkMode(I3DDMA_DEV, IDDMA_NN_MODE);
	if(h3ddma_get_cap_enable_mask() == 0)
	{
		rtd_pr_i3ddma_info("[NN] I3ddma nn capture not init,init i3ddma capture\n");
		h3ddma_set_cap_enable_mask(mask);
		//VDEC source maybe resolution change need re-config
		if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_JPEG) || (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC))
		{
			scaler_nn_force_run_idma(SLR_MAIN_DISPLAY, Get_DisplayMode_Src(SLR_MAIN_DISPLAY));
		}
		else
		{
			rtd_pr_i3ddma_info("[NN] I3ddma nn capture had init with scaler, just enable i3ddma capture %d\n", h3ddma_get_cap_enable_mask());
			h3ddma_nn_set_cap_enable(I3DDMA_DEV, 1);
		}
	}
	else
	{
		rtd_pr_i3ddma_info("[NN] I3ddma nn capture had init,enable i3ddma capture %d\n", h3ddma_get_cap_enable_mask());
		h3ddma_nn_set_cap_enable(I3DDMA_DEV, 1);
		h3ddma_set_cap_enable_mask(mask);
	}
	up(get_force_i3ddma_semaphore());
}

//close i3ddma nn cap
void h3ddma_close_i3ddma_capture(unsigned char mask)
{
	unsigned char mask_en = 0;
	down(get_force_i3ddma_semaphore());
	h3ddma_clear_cap_enable_mask(mask);
	up(get_force_i3ddma_semaphore());
	mask_en = h3ddma_get_cap_enable_mask();

	if((g_tNN_Cap_Buffer[0].phyaddr != 0) && (mask_en == 0))
	{
		rtd_pr_i3ddma_info("[NN] All apps off,disable i3ddma capture %d\n", mask_en);
		h3ddma_nn_set_cap_enable(I3DDMA_DEV, 0);
	}
	else
	{
		rtd_pr_i3ddma_info("[NN] Other apps are still working,do nothing %d\n", mask_en);
	}
}


#endif

void h3ddma_set_nv12_buffer_addr(UINT32 width,UINT32 height,unsigned long addr)
{
	unsigned long y_addr,c_addr;

	y_addr=addr;
	c_addr=addr+ALIGN((width*height),__4KPAGE);

	IoReg_Write32( H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg, y_addr);
	IoReg_Write32( H3DDMA_CAP2_WR_DMA_num_bl_wrap_addr_0_reg, c_addr);
}

void h3ddma_set_MDP_enable(unsigned char enable){

	h3ddma_cap3_cti_dma_wr_ctrl_RBUS h3ddma_cap3_cti_dma_wr_ctrl_reg;
	h3ddma_cap3_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP3_CTI_DMA_WR_Ctrl_reg);
    h3ddma_cap3_cti_dma_wr_ctrl_reg.cap3_dma_enable = enable;
    IoReg_Write32(H3DDMA_CAP3_CTI_DMA_WR_Ctrl_reg, h3ddma_cap3_cti_dma_wr_ctrl_reg.regValue);
}

void h3ddma_set_mdp_buffer_addr(void)
{
	unsigned int mdCapStartAddr = 0;
	unsigned int mdCapM1, mdCapM2, mdCapM3, mdCapM4;

	if(get_hdr_semaless_active())//(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_ACTIVE)
	{
		mdCapStartAddr = (unsigned int)I3DDMA_DolbyVision_get_md_buf();
		// IDMA md_parser capture size = 128 * 100 * 3= 0x9600

		mdCapM1 = mdCapStartAddr + 0x0;
		mdCapM2 = mdCapStartAddr + 0x10000;
		mdCapM3 = mdCapStartAddr + 0x20000;
		mdCapM4 = mdCapStartAddr + 0x30000;
	}
	else{


		rtd_pr_i3ddma_debug("\n*** [DV][ERROR] IDMA NOT ENABLED YET!! ***\n\n");
		return;
	}

	IoReg_Write32(H3DDMA_CAP3_WR_DMA_num_bl_wrap_addr_0_reg, mdCapM1);
	IoReg_Write32(H3DDMA_CAP3_WR_DMA_num_bl_wrap_addr_1_reg, mdCapM2);
	IoReg_Write32(H3DDMA_CAP3_WR_DMA_num_bl_wrap_addr_2_reg, mdCapM3);
	IoReg_Write32(H3DDMA_CAP3_WR_DMA_num_bl_wrap_addr_3_reg, mdCapM4);

	rtd_pr_i3ddma_debug("[DV] MD_Cap[1]=%x\n", mdCapM1);
	rtd_pr_i3ddma_debug("[DV] MD_Cap[2]=%x\n", mdCapM2);
	rtd_pr_i3ddma_debug("[DV] MD_Cap[3]=%x\n", mdCapM3);
	rtd_pr_i3ddma_debug("[DV] MD_Cap[4]=%x\n", mdCapM4);
}

void  drvif_h3ddma_set_MDP_capture(int pktNum, int burstPktNum)
{
        //#define FIFO_WIDTH   128 //128 bits

		int bitsPerFrame; //in bits
        int fifoElementsPerFrame; //# of 128 bits
        int burstLen;

		h3ddma_cap3_wr_dma_num_bl_wrap_word_RBUS  h3ddma_cap3_wr_dma_num_bl_wrap_word_reg;
		h3ddma_cap3_wr_dma_num_bl_wrap_ctl_RBUS  h3ddma_cap3_wr_dma_num_bl_wrap_ctl_reg;
		h3ddma_cap3_wr_dma_num_bl_wrap_line_step_RBUS  h3ddma_cap3_wr_dma_num_bl_wrap_line_step_reg;
		h3ddma_cap3_cti_dma_wr_ctrl_RBUS h3ddma_cap3_cti_dma_wr_ctrl_reg;
		h3ddma_cap3_wr_dma_pxltobus_RBUS h3ddma_cap3_wr_dma_pxltobus_reg;
		h3ddma_hsd_i3ddma0_uzd_ctrl0_RBUS h3ddma_hsd_i3ddma_uzd_ctrl0_reg;
		h3ddma_i3ddma_mdp_cr_RBUS h3ddma_i3ddma_mdp_cr_reg;
		h3ddma_interrupt_enable_RBUS h3ddma_interrupt_enable_reg;

		h3ddma_cap3_wr_dma_num_bl_wrap_word_reg.regValue = IoReg_Read32(H3DDMA_CAP3_WR_DMA_num_bl_wrap_word_reg);
		h3ddma_cap3_wr_dma_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(H3DDMA_CAP3_WR_DMA_num_bl_wrap_ctl_reg);
		h3ddma_cap3_wr_dma_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(H3DDMA_CAP3_WR_DMA_num_bl_wrap_line_step_reg);
		h3ddma_cap3_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP3_CTI_DMA_WR_Ctrl_reg);
		h3ddma_hsd_i3ddma_uzd_ctrl0_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA0_UZD_CTRL0_reg);

		bitsPerFrame =  pktNum * 128 * 8; // each pkt is 128*8 bits
        if(bitsPerFrame%FIFO_WIDTH)
            fifoElementsPerFrame = (bitsPerFrame/FIFO_WIDTH)+1;
        else
            fifoElementsPerFrame = (bitsPerFrame/FIFO_WIDTH);

		burstLen =  0x10; //burstPktNum*24;


		h3ddma_cap3_wr_dma_num_bl_wrap_word_reg.cap3_line_burst_num = fifoElementsPerFrame;
		IoReg_Write32(H3DDMA_CAP3_WR_DMA_num_bl_wrap_word_reg, h3ddma_cap3_wr_dma_num_bl_wrap_word_reg.regValue);

		h3ddma_cap3_wr_dma_num_bl_wrap_ctl_reg.cap3_burst_len = burstLen;
		h3ddma_cap3_wr_dma_num_bl_wrap_ctl_reg.cap3_line_num = 1;
		IoReg_Write32(H3DDMA_CAP3_WR_DMA_num_bl_wrap_ctl_reg, h3ddma_cap3_wr_dma_num_bl_wrap_ctl_reg.regValue);

		h3ddma_cap3_wr_dma_num_bl_wrap_line_step_reg.cap3_line_step = fifoElementsPerFrame; // in 64bits
		IoReg_Write32(H3DDMA_CAP3_WR_DMA_num_bl_wrap_line_step_reg, h3ddma_cap3_wr_dma_num_bl_wrap_line_step_reg.regValue);

		h3ddma_cap3_cti_dma_wr_ctrl_reg.cap3_dma_8byte_swap = 1;
		h3ddma_cap3_cti_dma_wr_ctrl_reg.cap3_dma_4byte_swap = 1;
		h3ddma_cap3_cti_dma_wr_ctrl_reg.cap3_dma_2byte_swap = 1;
		h3ddma_cap3_cti_dma_wr_ctrl_reg.cap3_dma_1byte_swap = 1;
		//h3ddma_cap3_cti_dma_wr_ctrl_reg.cap3_dma_enable=1;
		IoReg_Write32(H3DDMA_CAP3_CTI_DMA_WR_Ctrl_reg, h3ddma_cap3_cti_dma_wr_ctrl_reg.regValue);

		h3ddma_hsd_i3ddma_uzd_ctrl0_reg.out_bit = 0;
	//	h3ddma_hsd_i3ddma_vsd_ctrl0_reg.sort_fmt = 0;
       	IoReg_Write32(H3DDMA_HSD_I3DDMA0_UZD_CTRL0_reg, h3ddma_hsd_i3ddma_uzd_ctrl0_reg.regValue);

        h3ddma_i3ddma_mdp_cr_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_MDP_CR_reg);
        h3ddma_i3ddma_mdp_cr_reg.mdp_max_packet_num = 3;
        h3ddma_i3ddma_mdp_cr_reg.mdp_mode = 1;
        h3ddma_i3ddma_mdp_cr_reg.mdp_en = 1;
       	IoReg_Write32(H3DDMA_I3DDMA_MDP_CR_reg, h3ddma_i3ddma_mdp_cr_reg.regValue);

		//DIC: in cap3 case, Cap3_Pixel_encoding should set 1
    	h3ddma_cap3_wr_dma_pxltobus_reg.regValue = IoReg_Read32(H3DDMA_CAP3_WR_DMA_pxltobus_reg);
    	h3ddma_cap3_wr_dma_pxltobus_reg.cap3_pixel_encoding =1;
    	IoReg_Write32(H3DDMA_CAP3_WR_DMA_pxltobus_reg, h3ddma_cap3_wr_dma_pxltobus_reg.regValue);

#ifdef CONFIG_HDR_SDR_SEAMLESS
    	//down(&i3ddma_reg_Semaphore);  //todo: enable semaphore
    	h3ddma_interrupt_enable_reg.regValue = IoReg_Read32(H3DDMA_Interrupt_Enable_reg);
    	h3ddma_interrupt_enable_reg.cap3_last_wr_ie =1;
    	IoReg_Write32(H3DDMA_Interrupt_Enable_reg, h3ddma_interrupt_enable_reg.regValue);
    	//up(&i3ddma_reg_Semaphore);  //todo: enable semaphore
#else
    	h3ddma_interrupt_enable_reg.regValue = IoReg_Read32(H3DDMA_Interrupt_Enable_reg);
    	h3ddma_interrupt_enable_reg.cap3_last_wr_ie =1;
    	IoReg_Write32(H3DDMA_Interrupt_Enable_reg, h3ddma_interrupt_enable_reg.regValue);
#endif

	   	h3ddma_set_mdp_buffer_addr();
	  	h3ddma_set_MDP_enable(1);
}

#ifdef CONFIG_ENABLE_HDMI_NN
unsigned char get_cap_depth(unsigned dev)
{
	unsigned char depth = 0;

	switch(iddma_attr[dev].cap_depth)
	{
		case I3DDMA_COLOR_DEPTH_8B:
			depth = 8;
			break;
		case I3DDMA_COLOR_DEPTH_10B:
			depth = 10;
			break;
		case I3DDMA_COLOR_DEPTH_12B:
			depth = 12;
			break;
		case I3DDMA_COLOR_DEPTH_16B:
			depth = 16;
			break;
		default:
			depth = 8;
			break;
	}

	return depth;
}

void h3ddma_nn_set_buffer_addr(unsigned char mode)
{
	unsigned int ucBufAddr = 0;
	unsigned int ucSize = 0;

	if (g_tNN_Cap_Buffer[g_ucNNWriteIdx].phyaddr == 0)
	{
		rtd_pr_i3ddma_info("h3ddma_set_film_buffer_addr error, Idx=%d\n", g_ucNNWriteIdx);
		return;
	}

	if (mode == 0)
	{
		ucBufAddr = g_tNN_Cap_Buffer[g_ucNNWriteIdx].phyaddr;
		ucSize = g_tNN_Cap_Buffer[g_ucNNWriteIdx].size;

		IoReg_Write32( H3DDMA_CAP1_WR_DMA_num_bl_wrap_addr_0_reg , ucBufAddr);
		ucBufAddr = g_tNN_Cap_Buffer[0].phyaddr + ucSize * NN_CAP_BUFFER_NUM + (ucSize/2) * g_ucNNWriteIdx;
	    IoReg_Write32( H3DDMA_CAP2_WR_DMA_num_bl_wrap_addr_0_reg, ucBufAddr);
		//IoReg_SetBits(H3DDMA_H3DDMA_PQ_CMP_DOUBLE_BUFFER_CTRL_reg, _BIT1);

		// set getsize and this buffer is valid
		g_tNN_Cap_Buffer[g_ucNNWriteIdx].getsize = g_tNN_Cap_Buffer[g_ucNNWriteIdx].size;
		//rtd_pr_i3ddma_info("h3ddma_set_film_buffer_addr idx=%d, phyaddr=%x\n", g_ucNNWriteIdx, ucBufAddr);

		g_ucNNWriteIdx = (g_ucNNWriteIdx + 1) % NN_CAP_BUFFER_NUM;
	}
}

void drvif_h3ddma_nn_set_capture(UINT8 dev, UINT32 width, UINT32 height, UINT32 pitch)
{
	unsigned char depth = get_cap_depth(dev);
	h3ddma_i3ddma_enable_RBUS                      h3ddma_i3ddma_enable_reg;
	h3ddma_cap1_cap_ctl0_RBUS                      h3ddma_cap1_cap_ctl0_reg;
	h3ddma_cap1_wr_dma_num_bl_wrap_word_RBUS       h3ddma_cap1_wr_dma_num_bl_wrap_word_reg;
	h3ddma_cap1_wr_dma_num_bl_wrap_ctl_RBUS		   h3ddma_cap1_wr_dma_num_bl_wrap_ctl_reg;
	h3ddma_cap1_wr_dma_num_bl_wrap_line_step_RBUS  h3ddma_cap1_wr_dma_num_bl_wrap_line_step_reg;
	h3ddma_cap1_wr_dma_pxltobus_RBUS	           h3ddma_cap1_wr_dma_pxltobus_reg;
	h3ddma_cap2_cap_ctl0_RBUS                      h3ddma_cap2_cap_ctl0_reg;
	h3ddma_cap2_wr_dma_num_bl_wrap_word_RBUS 	   h3ddma_cap2_wr_dma_num_bl_wrap_word_reg;
	h3ddma_cap2_wr_dma_pxltobus_RBUS	           h3ddma_cap2_wr_dma_pxltobus_reg;
	h3ddma_cap2_wr_dma_num_bl_wrap_ctl_RBUS		   h3ddma_cap2_wr_dma_num_bl_wrap_ctl_reg;
	h3ddma_cap2_wr_dma_num_bl_wrap_line_step_RBUS  h3ddma_cap2_wr_dma_num_bl_wrap_line_step_reg;
//	h3ddma_lr_separate_ctrl2_RBUS                  h3ddma_lr_separate_ctrl2_reg;
	h3ddma_lr_separate_ctrl3_RBUS                  h3ddma_lr_separate_ctrl3_reg;
//	h3ddma_lr_separate_ctrl1_RBUS                  h3ddma_lr_separate_ctrl1_reg;
	h3ddma_interrupt_enable_RBUS				   h3ddma_interrupt_enable_reg;
	h3ddma_cap1_cti_dma_wr_ctrl_RBUS 			   i3ddma_byte_channel_swap_reg;
	h3ddma_cap2_cti_dma_wr_ctrl_RBUS 			   i3ddma_byte_channe2_swap_reg;


	UINT32 bitsPerLine = width * depth;
	UINT32 bitsPerLine_align; //for gpu align
	UINT32 fifoWidth = 128;
	UINT32 fifoElementsPerLine;
	UINT32 fifoElementsPerLine_align;
	SIZE insize,outsize;

	h3ddma_nn_set_cap_enable(dev, 0);

	fifoElementsPerLine = (bitsPerLine + fifoWidth - 1) / fifoWidth;
	rtd_pr_i3ddma_info("i%ddma pitch=%d\n", (dev + 3), pitch);
	if((pitch != 0) && (pitch > width))
	{
		bitsPerLine_align = pitch * depth;
		fifoElementsPerLine_align = (bitsPerLine_align + fifoWidth - 1) / fifoWidth;
	}
	else
	{
		fifoElementsPerLine_align = fifoElementsPerLine;
	}


	// Disable double bfffer
	IoReg_ClearBits(iddma_db_reg_addr[dev], _BIT0);

	//cap0 block mode
	h3ddma_i3ddma_enable_reg.regValue = IoReg_Read32(iddma_dma_enable_reg_addr[dev]);
	h3ddma_i3ddma_enable_reg.cap1_seq_blk_sel = 1;
	h3ddma_i3ddma_enable_reg.cap2_seq_blk_sel = 1;
	//h3ddma_i3ddma_enable_reg.uzd_mux_sel = 1;
	IoReg_Write32(iddma_dma_enable_reg_addr[dev], h3ddma_i3ddma_enable_reg.regValue);

	h3ddma_cap1_cap_ctl0_reg.regValue = IoReg_Read32(iddma_cap1_ctrl_reg_addr[dev]);
	h3ddma_cap1_cap_ctl0_reg.cap1_auto_block_sel_en = 0;
	IoReg_Write32(iddma_cap1_ctrl_reg_addr[dev], h3ddma_cap1_cap_ctl0_reg.regValue);
	h3ddma_cap2_cap_ctl0_reg.regValue = IoReg_Read32(iddma_cap2_ctrl_reg_addr[dev]);
	h3ddma_cap2_cap_ctl0_reg.cap2_auto_block_sel_en = 0;
	IoReg_Write32(iddma_cap2_ctrl_reg_addr[dev], h3ddma_cap2_cap_ctl0_reg.regValue);
	IoReg_Write32(iddma_cap2_1_ctrl_reg_addr[dev], h3ddma_cap2_cap_ctl0_reg.regValue);

	//data_y
	h3ddma_cap1_wr_dma_num_bl_wrap_word_reg.regValue      = IoReg_Read32(iddma_cap1_line_burst_unm_reg_addr[dev]);
	h3ddma_cap1_wr_dma_num_bl_wrap_word_reg.cap1_line_burst_num = fifoElementsPerLine;
	IoReg_Write32(iddma_cap1_line_burst_unm_reg_addr[dev], h3ddma_cap1_wr_dma_num_bl_wrap_word_reg.regValue);

	h3ddma_cap1_wr_dma_pxltobus_reg.regValue = IoReg_Read32(iddma_cap1_pixel_encode_reg_addr[dev]);
	if(depth == 8)
		h3ddma_cap1_wr_dma_pxltobus_reg.cap1_bit_sel = 0;
	else if(depth == 10)
		h3ddma_cap1_wr_dma_pxltobus_reg.cap1_bit_sel = 1;
	h3ddma_cap1_wr_dma_pxltobus_reg.cap1_pixel_encoding = 0;
	IoReg_Write32(iddma_cap1_pixel_encode_reg_addr[dev], h3ddma_cap1_wr_dma_pxltobus_reg.regValue);

	h3ddma_cap1_wr_dma_num_bl_wrap_ctl_reg.regValue	   = IoReg_Read32(iddma_cap1_line_len_reg_addr[dev]);
	h3ddma_cap1_wr_dma_num_bl_wrap_ctl_reg.cap1_burst_len = 32;
	h3ddma_cap1_wr_dma_num_bl_wrap_ctl_reg.cap1_line_num = height;
	IoReg_Write32( iddma_cap1_line_len_reg_addr[dev], h3ddma_cap1_wr_dma_num_bl_wrap_ctl_reg.regValue);

	h3ddma_cap1_wr_dma_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(iddma_cap1_line_step_reg_addr[dev]);
	h3ddma_cap1_wr_dma_num_bl_wrap_line_step_reg.cap1_line_step = fifoElementsPerLine_align;
	IoReg_Write32(iddma_cap1_line_step_reg_addr[dev], h3ddma_cap1_wr_dma_num_bl_wrap_line_step_reg.regValue);

	//data_c
	h3ddma_cap2_wr_dma_num_bl_wrap_word_reg.regValue	   = IoReg_Read32(iddma_cap2_line_burst_unm_reg_addr[dev]);
	h3ddma_cap2_wr_dma_num_bl_wrap_word_reg.cap2_line_burst_num = fifoElementsPerLine;
	IoReg_Write32(iddma_cap2_line_burst_unm_reg_addr[dev], h3ddma_cap2_wr_dma_num_bl_wrap_word_reg.regValue);
	IoReg_Write32(iddma_cap2_1_line_burst_unm_reg_addr[dev], h3ddma_cap2_wr_dma_num_bl_wrap_word_reg.regValue);

	h3ddma_cap2_wr_dma_pxltobus_reg.regValue = IoReg_Read32(iddma_cap2_pixel_encode_reg_addr[dev]);
	if(depth == 8)
		h3ddma_cap2_wr_dma_pxltobus_reg.cap2_bit_sel = 0;
	else if(depth == 10)
		h3ddma_cap2_wr_dma_pxltobus_reg.cap2_bit_sel = 1;

	if((iddma_attr[dev].cap_format == I3DDMA_COLOR_YV12) || (iddma_attr[dev].cap_format == I3DDMA_COLOR_I420))
		h3ddma_cap2_wr_dma_pxltobus_reg.cap2_yv12_en = 1;
	else
		h3ddma_cap2_wr_dma_pxltobus_reg.cap2_yv12_en = 0;

	if(iddma_attr[dev].cap_format == I3DDMA_COLOR_P010)
		h3ddma_cap2_wr_dma_pxltobus_reg.cap2_p010_en = 1;
	else
		h3ddma_cap2_wr_dma_pxltobus_reg.cap2_p010_en = 0;

	if(iddma_attr[dev].cap_format == I3DDMA_COLOR_I420)
		h3ddma_cap2_wr_dma_pxltobus_reg.cap2_420_cb_cr_swap = 1;
	else
		h3ddma_cap2_wr_dma_pxltobus_reg.cap2_420_cb_cr_swap = 0;
	h3ddma_cap2_wr_dma_pxltobus_reg.cap2_pixel_encoding = 2;
	IoReg_Write32(iddma_cap2_pixel_encode_reg_addr[dev], h3ddma_cap2_wr_dma_pxltobus_reg.regValue);

	h3ddma_cap2_wr_dma_num_bl_wrap_ctl_reg.regValue	   = IoReg_Read32(iddma_cap2_line_len_reg_addr[dev]);
	if(iddma_attr[dev].format_is_3planes)
		h3ddma_cap2_wr_dma_num_bl_wrap_ctl_reg.cap2_line_num = height/4;
	else
		h3ddma_cap2_wr_dma_num_bl_wrap_ctl_reg.cap2_line_num = height/2;
	h3ddma_cap2_wr_dma_num_bl_wrap_ctl_reg.cap2_burst_len = 32;
	IoReg_Write32(iddma_cap2_line_len_reg_addr[dev], h3ddma_cap2_wr_dma_num_bl_wrap_ctl_reg.regValue);
	IoReg_Write32(iddma_cap2_1_line_len_reg_addr[dev], h3ddma_cap2_wr_dma_num_bl_wrap_ctl_reg.regValue);

	h3ddma_cap2_wr_dma_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(iddma_cap2_line_step_reg_addr[dev]);
	h3ddma_cap2_wr_dma_num_bl_wrap_line_step_reg.cap2_line_step = fifoElementsPerLine_align;
	IoReg_Write32(iddma_cap2_line_step_reg_addr[dev], h3ddma_cap2_wr_dma_num_bl_wrap_line_step_reg.regValue);
	IoReg_Write32(iddma_cap2_1_line_step_reg_addr[dev], h3ddma_cap2_wr_dma_num_bl_wrap_line_step_reg.regValue);

	h3ddma_lr_separate_ctrl3_reg.regValue = IoReg_Read32(iddma_lr_separate_ctrl3_reg_addr[dev]);
	h3ddma_lr_separate_ctrl3_reg.hact2 = width;
	h3ddma_lr_separate_ctrl3_reg.vact2 = height;
	IoReg_Write32(iddma_lr_separate_ctrl3_reg_addr[dev],h3ddma_lr_separate_ctrl3_reg.regValue);

	h3ddma_interrupt_enable_reg.regValue = IoReg_Read32(iddma_interrupt_enable_reg_addr[dev]);
	//if(IDDMA_Get_DMAWorkMode(dev) == IDDMA_NN_MODE)
	//{
	//	h3ddma_interrupt_enable_reg.cap1_last_wr_ie = 1;
	//	h3ddma_interrupt_enable_reg.cap1_last_wr_int_mux = 1; //only i3 have it
	//}
	//else 
	//if(dev != I3DDMA_DEV)
	{
		h3ddma_interrupt_enable_reg.cap1_last_wr_ie = 1;
		if(IDDMA_Get_DMAWorkMode(dev) == IDDMA_CAP_MODE)
		{
			//h3ddma_interrupt_enable_reg.vs_rising_ie = 1;
		}
	}
	IoReg_Write32(iddma_interrupt_enable_reg_addr[dev],h3ddma_interrupt_enable_reg.regValue);


	i3ddma_byte_channel_swap_reg.regValue = IoReg_Read32(iddma_cap1_dma_swap_reg_addr[dev]);
	i3ddma_byte_channel_swap_reg.cap1_dma_1byte_swap = 1;
	i3ddma_byte_channel_swap_reg.cap1_dma_2byte_swap = 1;
	i3ddma_byte_channel_swap_reg.cap1_dma_4byte_swap = 1;
	i3ddma_byte_channel_swap_reg.cap1_dma_8byte_swap = 1;
	IoReg_Write32(iddma_cap1_dma_swap_reg_addr[dev], i3ddma_byte_channel_swap_reg.regValue);

	i3ddma_byte_channe2_swap_reg.regValue = IoReg_Read32(iddma_cap2_dma_swap_reg_addr[dev]);
	i3ddma_byte_channe2_swap_reg.cap2_dma_1byte_swap = 1;
	i3ddma_byte_channe2_swap_reg.cap2_dma_2byte_swap = 1;
	i3ddma_byte_channe2_swap_reg.cap2_dma_4byte_swap = 1;
	i3ddma_byte_channe2_swap_reg.cap2_dma_8byte_swap = 1;
	IoReg_Write32(iddma_cap2_dma_swap_reg_addr[dev], i3ddma_byte_channe2_swap_reg.regValue);
	IoReg_Write32(iddma_cap2_1_dma_swap_reg_addr[dev], i3ddma_byte_channe2_swap_reg.regValue);

	if(IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_SOURCE_PIXEL_MODE))
	{
		if(dev != I3DDMA_DEV)
			insize.nWidth = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_WID_PRE)/2;
		else
			insize.nWidth = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_WID_PRE);
	}
	else
		insize.nWidth = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_WID_PRE);
	insize.nLength = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPV_ACT_LEN_PRE);
	outsize.nWidth = width;
	outsize.nLength = height;

	I3DDMA_ultrazoom_config_scaling_down_for_cap(dev, &insize, &outsize, 1);

	//always config i3ddma path, but only AIPQ enable set capture on
#if IS_ENABLED(CONFIG_RTK_AI_DRV)
	if(scalerAI_get_AIPQ_mode_enable_flag() && (IDDMA_Get_DMAWorkMode(dev) == IDDMA_NN_MODE))
		h3ddma_nn_set_cap_enable(dev, 1);
	else
#endif
		h3ddma_nn_set_cap_enable(dev, 1);
	rtd_pr_i3ddma_info("i%ddma cap_en=%x\n", (dev + 3), IoReg_Read32(iddma_cap1_dma_swap_reg_addr[dev]));
}
#endif

void h3ddma_set_sequence_cap0_buffer_addr(void)
{
		IoReg_Write32( H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg , 0);
        IoReg_Write32( H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg, 0);
}
#endif

/*
width need to align 32
line sum bit = (width * compression bit + 256) / 128
for new PQC and PQDC @Crixus 20170615
*/
unsigned int calc_i3ddma_comp_line_sum_bit(unsigned int comp_wid, unsigned int comp_ratio)
{
	unsigned char cmp_alpha_en = 0;
	unsigned short cmp_line_sum_bit, sub_pixel_num;
	unsigned int cmp_line_sum_bit_pure_rgb, cmp_line_sum_bit_pure_a, cmp_line_sum_bit_32_dummy, cmp_line_sum_bit_128_dummy, cmp_line_sum_bit_real;
	unsigned int cmp_width_div32 = 0, frame_limit_bit = comp_ratio;


	if((comp_wid % 32) != 0){
		//drop bits
		comp_wid = comp_wid + (32 - (comp_wid % 32));
	}

	frame_limit_bit = frame_limit_bit << 2;
	cmp_width_div32 = comp_wid / 32;


	sub_pixel_num = (cmp_alpha_en)? 4 : 3;
		cmp_line_sum_bit_pure_rgb = (cmp_width_div32 * 32) * (frame_limit_bit>>2);
	cmp_line_sum_bit_pure_a = (cmp_alpha_en)?((cmp_width_div32 * 32)* (frame_limit_bit>>2)):(0);
	cmp_line_sum_bit_32_dummy = (6+1) * 32 * sub_pixel_num;
	cmp_line_sum_bit_128_dummy = 0;
	cmp_line_sum_bit_real = cmp_line_sum_bit_pure_rgb + cmp_line_sum_bit_pure_a + cmp_line_sum_bit_32_dummy + cmp_line_sum_bit_128_dummy;
	cmp_line_sum_bit = ((cmp_line_sum_bit_real+128)>>8)*2;//ceil((cmp_line_sum_bit_real)/128)*2;

	rtd_pr_i3ddma_notice("function=%s, line=%d\n", __FUNCTION__, __LINE__);
	rtd_pr_i3ddma_notice("sub_pixel_num=%x, cmp_line_sum_bit_pure_rgb=%x, cmp_line_sum_bit_pure_a=%x, cmp_line_sum_bit_32_dummy=%x, cmp_line_sum_bit_128_dummy=%x\n", sub_pixel_num, cmp_line_sum_bit_pure_rgb, cmp_line_sum_bit_pure_a, cmp_line_sum_bit_32_dummy, cmp_line_sum_bit_128_dummy);
	rtd_pr_i3ddma_notice("cmp_line_sum_bit_real=0x%x\n", cmp_line_sum_bit_real);
	rtd_pr_i3ddma_notice("cmp_line_sum_bit=0x%x, frame_limit_bit=%d\n", cmp_line_sum_bit, frame_limit_bit);

	return cmp_line_sum_bit;
}


unsigned int h3ddma_get_comp_burst_num(I3DDMA_TIMING_T *timing)
{
	int bitsPerLine;
    int fifoElementsPerFrame;

	if(timing == NULL)
	{
		rtd_pr_i3ddma_emerg("function=%s, ERROR timing\n", __FUNCTION__);
		return 0x0;
	}
	if(dvrif_i3ddma_compression_get_enable() == TRUE){
		// cmp_line_sum_bit unit:128bits
		bitsPerLine = calc_i3ddma_comp_line_sum_bit(timing->h_act_len, dvrif_i3ddma_get_compression_bits());
		bitsPerLine = bitsPerLine * 128;
		//bitsPerLine = h_act_len * dvrif_i3ddma_get_compression_bits() + 256;
	}
	else
		bitsPerLine =(DEPTH2BITS[timing->depth] * (timing->h_act_len /4) * components[timing->color]);
	if(bitsPerLine%FIFO_WIDTH)
		fifoElementsPerFrame = (bitsPerLine/FIFO_WIDTH)+1;
	else
		fifoElementsPerFrame = (bitsPerLine/FIFO_WIDTH);

	return fifoElementsPerFrame;
}

void  drvif_h3ddma_set_sequence_capture0(I3DDMA_TIMING_T *timing)
{
    int fifoElementsPerFrame = h3ddma_get_comp_burst_num(timing);
    int burstLen;

	h3ddma_cap0_cap_ctl0_RBUS h3ddma_cap0_cap_ctl0_reg;
	h3ddma_lr_separate_ctrl1_RBUS h3ddma_lr_separate_ctrl1_reg;
	h3ddma_lr_separate_ctrl2_RBUS h3ddma_lr_separate_ctrl2_reg;
	h3ddma_cap0_cti_dma_wr_ctrl_RBUS h3ddma_cap0_cti_dma_wr_ctrl_reg;
//	h3ddma_i3ddma_enable_RBUS i3ddma_enable_reg;
	h3ddma_cap0_wr_dma_pxltobus_RBUS h3ddma_cap0_wr_dma_pxltobus_reg;
	h3ddma_cap0_wr_dma_num_bl_wrap_word_RBUS  h3ddma_cap0_wr_dma_num_bl_wrap_word_reg;
	h3ddma_cap0_wr_dma_num_bl_wrap_ctl_RBUS  h3ddma_cap0_wr_dma_num_bl_wrap_ctl_reg;
	h3ddma_cap0_wr_dma_num_bl_wrap_line_step_RBUS  h3ddma_cap0_wr_dma_num_bl_wrap_line_step_reg;

	if(timing  == NULL)
		return;
	// IDMA LR separate control
	h3ddma_lr_separate_ctrl1_reg.regValue = IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg);
	h3ddma_lr_separate_ctrl1_reg.progressive = (timing->progressive)? 1 : 0;
	IoReg_Write32(H3DDMA_LR_Separate_CTRL1_reg, h3ddma_lr_separate_ctrl1_reg.regValue);

	h3ddma_lr_separate_ctrl2_reg.regValue = IoReg_Read32(H3DDMA_LR_Separate_CTRL2_reg);
	h3ddma_lr_separate_ctrl2_reg.hact = timing->h_act_len;
	h3ddma_lr_separate_ctrl2_reg.vact = timing->v_act_len;
	IoReg_Write32(H3DDMA_LR_Separate_CTRL2_reg,h3ddma_lr_separate_ctrl2_reg.regValue);

	burstLen = 0x50;

	h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.regValue = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_word_reg);
	if(dvrif_i3ddma_compression_get_enable()) {
		if(dvrif_i3ddma_get_compression_mode() == I3DDMA_COMP_LINE_MODE)
			h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.cap0_line_burst_num = fifoElementsPerFrame;
		else
			h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.cap0_line_burst_num = 0xFFFFFF;  //frame mode need set max
	} else {
		h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.cap0_line_burst_num = fifoElementsPerFrame; //disable pqc default run line mode
	}	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_word_reg, h3ddma_cap0_wr_dma_num_bl_wrap_word_reg.regValue);

	h3ddma_cap0_wr_dma_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_ctl_reg);
	h3ddma_cap0_wr_dma_num_bl_wrap_ctl_reg.cap0_burst_len = burstLen;
	h3ddma_cap0_wr_dma_num_bl_wrap_ctl_reg.cap0_line_num = timing->v_act_len;
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_ctl_reg, h3ddma_cap0_wr_dma_num_bl_wrap_ctl_reg.regValue);

	h3ddma_cap0_wr_dma_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_line_step_reg);
	h3ddma_cap0_wr_dma_num_bl_wrap_line_step_reg.cap0_line_step = fifoElementsPerFrame;
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_line_step_reg, h3ddma_cap0_wr_dma_num_bl_wrap_line_step_reg.regValue);

	h3ddma_cap0_cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_CAP0_Cap_CTL0_reg);
	h3ddma_cap0_cap_ctl0_reg.cap0_triple_buf_en = 0;
	h3ddma_cap0_cap_ctl0_reg.cap0_quad_buf_en = 0;
	h3ddma_cap0_cap_ctl0_reg.cap0_freeze_en = 0;
	IoReg_Write32(H3DDMA_CAP0_Cap_CTL0_reg, h3ddma_cap0_cap_ctl0_reg.regValue);

	h3ddma_cap0_wr_dma_pxltobus_reg.regValue=IoReg_Read32(H3DDMA_CAP0_WR_DMA_pxltobus_reg);
//#ifndef CONFIG_RTK_KDRV_DV_IDK_DUMP
#if 0
	h3ddma_cap0_wr_dma_pxltobus_reg.cap0_bit_sel=0;
	h3ddma_cap0_wr_dma_pxltobus_reg.cap0_pixel_encoding=0;
#else
	h3ddma_cap0_wr_dma_pxltobus_reg.cap0_bit_sel = timing->depth != I3DDMA_COLOR_DEPTH_8B;
	h3ddma_cap0_wr_dma_pxltobus_reg.cap0_pixel_encoding = encodemap[timing->color];
#endif
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_pxltobus_reg, h3ddma_cap0_wr_dma_pxltobus_reg.regValue);

	if(I3DDMA_3D_OPMODE_HW==drvif_I3DDMA_Get_Input_Info_Vaule(I3DDMA_DEV, I3DDMA_INFO_HW_I3DDMA_DMA))
	{
		//for normal i3ddma vodma
		h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg);
		h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_1byte_swap = 0;
		h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_2byte_swap = 0;
		h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_4byte_swap = 0;
		h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_8byte_swap = 0;
		h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_max_outstanding = 3;
		IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg, h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue);
	}
	else
	{
#ifndef BUILD_QUICK_SHOW
        if(I3DDMA_SwGetBufNum() > 0)
        {
            //for video delay
            h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg);
            h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_1byte_swap = 0;
            h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_2byte_swap = 0;
            h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_4byte_swap = 0;
            h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_8byte_swap = 0;
            IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg, h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue);
        } else
#endif
        {
            //for capture
            h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg);
            h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_1byte_swap = 0;
            h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_2byte_swap = 1;
            h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_4byte_swap = 1;
            h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_8byte_swap = 1;
            //h3ddma_cap0_cti_dma_wr_ctrl_reg.cap0_dma_enable=1; //enable IDMA capture3
            IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg, h3ddma_cap0_cti_dma_wr_ctrl_reg.regValue);
        }
	}

}
#ifndef BUILD_QUICK_SHOW

#ifdef CONFIG_ENABLE_HDMI_NN
void h3ddma_nn_send_cap_info(void)
{
	int ret = 0;
    I3DDMA_NN_CAP_INFO_T sendNNCapInfo = {0};

	sendNNCapInfo.start_buffer_addr = g_tNN_Cap_Buffer[0].phyaddr;
	sendNNCapInfo.buffer_size = g_tNN_Cap_Buffer[0].size;
	sendNNCapInfo.buffer_num = NN_CAP_BUFFER_NUM;
	sendNNCapInfo.cap_ratio = h3ddma_NN_get_cap_ratio();
	sendNNCapInfo.support_cap_mode = g_enSupCapMode;

	sendNNCapInfo.start_buffer_addr = htonl(sendNNCapInfo.start_buffer_addr);
	sendNNCapInfo.buffer_size = htonl(sendNNCapInfo.buffer_size);
	//sendNNCapInfo.buffer_num = htonl(sendNNCapInfo.buffer_num);
	sendNNCapInfo.cap_ratio = htonl(sendNNCapInfo.cap_ratio);
	sendNNCapInfo.support_cap_mode = htonl(sendNNCapInfo.support_cap_mode);

	// copy from RPC share memory
	memcpy((unsigned char*)Scaler_GetShareMemVirAddr(SCALERIOC_I3_NN_SEND_CAP_INFO), (unsigned char *)&sendNNCapInfo, sizeof(I3DDMA_NN_CAP_INFO_T));

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_I3_NN_SEND_CAP_INFO,0,0)))
	{
		rtd_pr_i3ddma_err("[%s] ret=%d, SCALERIOC_I3_NN_SEND_CAP_INFO RPC fail !!!\n", __FUNCTION__, ret);
	}
}

void h3ddma_nn_set_cap_enable(unsigned char dev, unsigned int enable)
{
//	h3ddma_cap0_cti_dma_wr_ctrl_RBUS h3ddma_cap0_cti_dma_wr_ctrl_reg;
	h3ddma_cap1_cti_dma_wr_ctrl_RBUS h3ddma_cap1_cti_dma_wr_ctrl_reg;
	h3ddma_cap2_cti_dma_wr_ctrl_RBUS h3ddma_cap2_cti_dma_wr_ctrl_reg;
//	h3ddma_cap3_cti_dma_wr_ctrl_RBUS h3ddma_cap3_cti_dma_wr_ctrl_reg;

	//IoReg_SetBits(H3DDMA_H3DDMA_PQ_CMP_DOUBLE_BUFFER_CTRL_reg, _BIT0);

	h3ddma_cap1_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(iddma_cap1_dma_swap_reg_addr[dev]);
	h3ddma_cap1_cti_dma_wr_ctrl_reg.cap1_dma_enable=enable;
	IoReg_Write32(iddma_cap1_dma_swap_reg_addr[dev], h3ddma_cap1_cti_dma_wr_ctrl_reg.regValue);

	h3ddma_cap2_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(iddma_cap2_dma_swap_reg_addr[dev]);
	h3ddma_cap2_cti_dma_wr_ctrl_reg.cap2_dma_enable=enable;
	IoReg_Write32(iddma_cap2_dma_swap_reg_addr[dev], h3ddma_cap2_cti_dma_wr_ctrl_reg.regValue);
	if(iddma_attr[dev].format_is_3planes && enable)
		IoReg_SetBits(iddma_cap2_1_dma_swap_reg_addr[dev], H3DDMA_CAP2_1_CTI_DMA_WR_Ctrl_cap2_1_dma_enable_mask);
	else
		IoReg_ClearBits(iddma_cap2_1_dma_swap_reg_addr[dev], H3DDMA_CAP2_1_CTI_DMA_WR_Ctrl_cap2_1_dma_enable_mask);

	//IoReg_SetBits(H3DDMA_H3DDMA_PQ_CMP_DOUBLE_BUFFER_CTRL_reg, _BIT1);

	//rtd_pr_i3ddma_emerg("0xB8025B8C=%x 0xB8025D0C=%x\n", IoReg_Read32(H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg), IoReg_Read32(H3DDMA_CAP2_CTI_DMA_WR_Ctrl_reg));
}

unsigned char h3ddma_nn_get_cap_enable(unsigned char dev)
{
	h3ddma_cap1_cti_dma_wr_ctrl_RBUS h3ddma_cap1_cti_dma_wr_ctrl_reg;
	h3ddma_cap2_cti_dma_wr_ctrl_RBUS h3ddma_cap2_cti_dma_wr_ctrl_reg;

	h3ddma_cap1_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(iddma_cap1_dma_swap_reg_addr[dev]);
	h3ddma_cap2_cti_dma_wr_ctrl_reg.regValue = IoReg_Read32(iddma_cap2_dma_swap_reg_addr[dev]);

	if(h3ddma_cap1_cti_dma_wr_ctrl_reg.cap1_dma_enable || h3ddma_cap2_cti_dma_wr_ctrl_reg.cap2_dma_enable)
		return TRUE;
	else
		return FALSE;
}

void h3ddma_nn_decide_cap_mode(void)
{
	if(get_hdmi_4k_hfr_mode() == HDMI_4K120_2_1)
		g_enSupCapMode = I3DDMA_UZD_CAP_ONLY;
	else if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC)
		g_enSupCapMode = I3DDMA_CAP_MIX;
	else
		g_enSupCapMode = I3DDMA_CAP_MIX;

	rtd_pr_i3ddma_notice("[I3DDMA][NN][%s] support cap mode = %d\n", __FUNCTION__, g_enSupCapMode);
}


void init_buffer(unsigned char dev)
{
	unsigned char i = 0;
	unsigned char* pVirStartAdrr = NULL;

	if((iddma_attr[dev].iddma_buffer[0].uncache == NULL) && (IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg) & _BIT29))
	{
		pVirStartAdrr = (unsigned char*)dvr_remap_uncached_memory(iddma_attr[dev].start_buffer_addr, iddma_attr[dev].total_buffer_size, __builtin_return_address(0));
	}

	for(i = 0; i < iddma_attr[dev].buffer_num; ++i)
	{
		if(IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg) & _BIT29)
			iddma_attr[dev].iddma_buffer[0].uncache = pVirStartAdrr + iddma_attr[dev].image_size * i;
		else
			iddma_attr[dev].iddma_buffer[0].uncache = NULL;
		iddma_attr[dev].iddma_buffer[i].phyaddr = iddma_attr[dev].start_buffer_addr + iddma_attr[dev].image_size * i;
		iddma_attr[dev].iddma_buffer[i].size = iddma_attr[dev].image_size;
		iddma_attr[dev].iddma_buffer[i].flag = 0;
		iddma_attr[dev].iddma_buffer[i].flag |= _BIT(IDDMA_CAPTURE_BUFFER_FLAG_RDY);
		iddma_attr[dev].iddma_buffer[i].flag |= _BIT(IDDMA_CAPTURE_BUFFER_FLAG_IDLE);
		iddma_attr[dev].iddma_buffer[i].weight = 0;
		iddma_attr[dev].iddma_buffer[i].fd = 0;
		iddma_attr[dev].iddma_buffer[i].timestamp = 0;
		iddma_attr[dev].iddma_buffer[i].bufferId = 0;
		if(i == 0)
			iddma_attr[dev].iddma_buffer[i].getsize = iddma_attr[dev].total_buffer_size;
		rtd_pr_i3ddma_notice("i%ddma addr%d=%x \n", (dev + 3), i, iddma_attr[dev].iddma_buffer[i].phyaddr);
	}
}

void  IDDMA_FreeCapBuffer(unsigned dev)
{
	unsigned int i;
	
	if(iddma_attr[dev].memory_type == IDDMA_MEMORY_MMAP)
	{
		if(iddma_attr[dev].iddma_buffer[0].uncache)
			dvr_unmap_memory(iddma_attr[dev].iddma_buffer[0].uncache, iddma_attr[dev].total_buffer_size);
		if(iddma_attr[dev].start_buffer_addr != 0)
			pli_free(iddma_attr[dev].start_buffer_addr);
	}

	iddma_attr[dev].start_buffer_addr = 0;
	iddma_attr[dev].image_size = 0;
	iddma_attr[dev].total_buffer_size = 0;
	
	for(i = 0; i < IDDMA_MAX_BUFFER_NUM; i++)
	{
		iddma_attr[dev].iddma_buffer[i].cache = NULL;
		iddma_attr[dev].iddma_buffer[i].phyaddr = (unsigned long)NULL;
		iddma_attr[dev].iddma_buffer[i].phyaddr_unalign = (unsigned long)NULL;
		if((iddma_attr[dev].iddma_buffer[i].uncache) && (iddma_attr[dev].memory_type == IDDMA_MEMORY_DMA))
			dvr_unmap_memory(iddma_attr[dev].iddma_buffer[i].uncache, drvif_memory_get_data_align(iddma_attr[dev].iddma_buffer[i].size, __4KPAGE));
		iddma_attr[dev].iddma_buffer[i].uncache = NULL;
		iddma_attr[dev].iddma_buffer[i].size = 0;
		iddma_attr[dev].iddma_buffer[i].getsize = 0;//test
#ifdef CONFIG_VBM_HEAP
		iddma_attr[dev].iddma_buffer[i].dmabuf_hndl = 0;
#endif
		iddma_attr[dev].iddma_buffer[i].flag = 0;
		iddma_attr[dev].iddma_buffer[i].fd = 0;
#ifndef BUILD_QUICK_SHOW
		//list_del(&iddma_attr[dev].iddma_buffer[i].list);
#endif
	}
}

unsigned int get_image_y_buffer_size(unsigned char dev)
{
	unsigned char depth = get_cap_depth(dev);
	unsigned int width_align = iddma_attr[dev].pitch;
	if((iddma_attr[dev].output_region.src_wid == 0) || (iddma_attr[dev].output_region.src_height == 0))
	{
		rtd_pr_i3ddma_notice("%s i%dddma size no init \n", __FUNCTION__, (dev + 3));
		return 0;
	}
	if(width_align < iddma_attr[dev].output_region.src_wid)
	{
		//rtd_pr_i3ddma_err("pitch(%d) < output size(%d)\n", width_align, iddma_attr[dev].output_region.src_wid);
		width_align = iddma_attr[dev].output_region.src_wid;
	}
	return ((width_align * depth) / 8 * iddma_attr[dev].output_region.src_height);
}

void IDDMA_Set_memoryType(unsigned char dev, unsigned char type)
{
	iddma_attr[dev].memory_type = type;
}

unsigned char IDDMA_Get_memoryType(unsigned char dev)
{
	return iddma_attr[dev].memory_type;
}

void IDDMA_Set_memoryBufNum(unsigned char dev, unsigned char num)
{
	if(num > IDDMA_MAX_BUFFER_NUM)
		num = IDDMA_MAX_BUFFER_NUM;

	iddma_attr[dev].buffer_num = num;
}

unsigned char IDDMA_Get_memoryBufNum(unsigned char dev)
{
	return iddma_attr[dev].buffer_num;
}

unsigned int IDDMA_Allocat_memoryBuf(unsigned char dev)
{
	if(IDDMA_Get_memoryType(dev) == IDDMA_MEMORY_MMAP)
	{
		iddma_attr[dev].image_size = get_image_y_buffer_size(dev) * 3 / 2; 
		iddma_attr[dev].total_buffer_size = ALIGN(iddma_attr[dev].image_size * iddma_attr[dev].buffer_num, __4KPAGE);
		iddma_attr[dev].start_buffer_addr = pli_malloc(iddma_attr[dev].total_buffer_size, GFP_DCU2_FIRST);
		if(iddma_attr[dev].start_buffer_addr == INVALID_VAL)
		{
			rtd_pr_i3ddma_err("[%s] i%ddma memory allocate fail!!!!!", __FUNCTION__, dev);
			return FALSE;
		}

		IoReg_Write32(iddma_cap1_boundary_low_limit_reg_addr[dev], iddma_attr[dev].start_buffer_addr);
		IoReg_Write32(iddma_cap1_boundary_up_limit_reg_addr[dev], iddma_attr[dev].start_buffer_addr + iddma_attr[dev].total_buffer_size);

		IoReg_Write32(iddma_cap2_boundary_low_limit_reg_addr[dev], iddma_attr[dev].start_buffer_addr);
		IoReg_Write32(iddma_cap2_boundary_up_limit_reg_addr[dev], iddma_attr[dev].start_buffer_addr + iddma_attr[dev].total_buffer_size);

		IoReg_Write32(iddma_cap2_1_boundary_low_limit_reg_addr[dev], iddma_attr[dev].start_buffer_addr);
		IoReg_Write32(iddma_cap2_1_boundary_up_limit_reg_addr[dev], iddma_attr[dev].start_buffer_addr + iddma_attr[dev].total_buffer_size);

		init_buffer(dev);
	}
	else
	{
		unsigned char i = 0;
		iddma_attr[dev].image_size = get_image_y_buffer_size(dev) * 3 / 2; 

		if(iddma_attr[dev].image_size > iddma_attr[dev].iddma_buffer[0].getsize)
		{
			rtd_pr_i3ddma_info("[IDDMA] memory size too small (%x %x)!!!\n", iddma_attr[dev].image_size, iddma_attr[dev].iddma_buffer[0].getsize);
			return FALSE;
		}

		for(i = 0; i < IDDMA_MAX_BUFFER_NUM; i++)
		{
			if(i < iddma_attr[dev].buffer_num)
			{
				if((iddma_attr[dev].iddma_buffer[i].phyaddr == INVALID_VAL) || (iddma_attr[dev].iddma_buffer[i].phyaddr == 0))
				{
					rtd_pr_ioreg_emerg("[IDDMA][%s] buffer address is zero!!!\n", __FUNCTION__);
					return FALSE;
				}
				iddma_attr[dev].iddma_buffer[i].flag = 0;
				iddma_attr[dev].iddma_buffer[i].flag |= _BIT(IDDMA_CAPTURE_BUFFER_FLAG_RDY);
				iddma_attr[dev].iddma_buffer[i].flag |= _BIT(IDDMA_CAPTURE_BUFFER_FLAG_IDLE);
				iddma_attr[dev].iddma_buffer[i].weight = 0;
				iddma_attr[dev].iddma_buffer[i].timestamp = 0;
				rtd_pr_ioreg_emerg("[IDDMA][%s] %d i%ddma dma buffer %x flag %d\n",__FUNCTION__, i, (dev+3), iddma_attr[dev].iddma_buffer[i].phyaddr, iddma_attr[dev].iddma_buffer[i].flag);
			}
		}
	}

	return TRUE;
}

unsigned int IDDMA_Get_YbufSize(unsigned char dev)
{
	return get_image_y_buffer_size(dev);
}

unsigned int IDDMA_Get_UVbufSize(unsigned char dev)
{
	if(iddma_attr[dev].format_is_3planes)
		return (get_image_y_buffer_size(dev)/4);
	else
		return (get_image_y_buffer_size(dev)/2);
}

unsigned int IDDMA_Get_YbufAddr_byIdx(unsigned dev, unsigned char index)
{
	if(index > iddma_attr[dev].buffer_num)
		return 0;
	return iddma_attr[dev].iddma_buffer[index].phyaddr;
}

unsigned char find_available_buffer_index(unsigned char dev)
{
	unsigned char i;
	unsigned char idle_buf_idx = IDDMA_MAX_BUFFER_NUM;
	unsigned char oldest_buf_idx = IDDMA_MAX_BUFFER_NUM;
	unsigned char mini_wwight_idx = IDDMA_MAX_BUFFER_NUM;
	unsigned long long tmp_stamp = 0;
	unsigned long long weigth = ~1;

	for (i = 0; i < iddma_attr[dev].buffer_num; ++i)
	{
		//1. find idle buffer index
		if(iddma_attr[dev].iddma_buffer[i].flag & _BIT(IDDMA_CAPTURE_BUFFER_FLAG_IDLE))
		{
			idle_buf_idx = i;
			if(iddma_attr[dev].iddma_buffer[i].weight <= weigth)
			{
				weigth = iddma_attr[dev].iddma_buffer[i].weight;
				mini_wwight_idx = i;
			}
		}

		//2. find oldest buffer index
		if(iddma_attr[dev].iddma_buffer[i].flag & _BIT(IDDMA_CAPTURE_BUFFER_FLAG_WD))
		{
			if(tmp_stamp == 0)
			{
				tmp_stamp = iddma_attr[dev].iddma_buffer[i].timestamp;
				oldest_buf_idx = i;
			}
			else
			{
				if(iddma_attr[dev].iddma_buffer[i].timestamp < tmp_stamp)
				{
					tmp_stamp = iddma_attr[dev].iddma_buffer[i].timestamp;
					oldest_buf_idx = i;
				}
			}
		}
	}

	//rtd_pr_i3ddma_err("[IDDMA]i%ddma find available buffer (%d %d %d) w:%lld!!!!!\n", (dev + 3), mini_wwight_idx, idle_buf_idx, oldest_buf_idx, weigth);
	if(mini_wwight_idx < IDDMA_MAX_BUFFER_NUM)
	{
		return mini_wwight_idx;
	}

	if(idle_buf_idx < IDDMA_MAX_BUFFER_NUM)
	{
		return idle_buf_idx;
	}

	if(oldest_buf_idx < IDDMA_MAX_BUFFER_NUM)
	{
		return oldest_buf_idx;
	}

	rtd_pr_i3ddma_err("[IDDMA]i%ddma can't find available buffer!!!!!\n", (dev + 3));
	return 0;
}

unsigned int calc_raw_data_crc_for_debug(unsigned char dev, unsigned char* pVirStartAdrr, unsigned int size)
{
	const unsigned int chk_cnt = 20;
	unsigned int i;
	unsigned int crc_value = 100;
	unsigned int shift_size1 = size/2 - 10;
	unsigned int shift_size2 = size - 100;

	if((shift_size1 > size) || (shift_size2) > size)
		return 0;

	if(pVirStartAdrr != NULL)
	{
		crc_value = 0;
		for(i = 0; i < chk_cnt; ++i)
		{
			crc_value += pVirStartAdrr[i];
			crc_value += pVirStartAdrr[shift_size1 + i];
			crc_value += pVirStartAdrr[shift_size2 + i];
		}
	}

	return crc_value;
}


unsigned char IDDMA_Set_CapStartAddr(unsigned char dev)
{
	unsigned int ucBufAddr = 0;
	unsigned int ucSize = 0;
	unsigned int write_idx = 0;
	unsigned long flags;
	int ptrlo = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);
	int ptrhi = IoReg_Read32(TIMER_SCPU_CLK90K_HI_reg);
	long long stamp = ptrlo | (((int64_t)ptrhi) << 32);
	static long long pre_stamp[MAX_IDDMA_DEV] = {0, 0, 0, 0};

	spin_lock_irqsave(&iddma_attr[dev].iddma_spinlock, flags);
	write_idx = iddma_attr[dev].write_point;

	//set previous write done flag
	if(write_idx < iddma_attr[dev].buffer_num)
	{
		iddma_attr[dev].iddma_buffer[write_idx].crc = calc_raw_data_crc_for_debug(dev, iddma_attr[dev].iddma_buffer[write_idx].uncache, iddma_attr[dev].iddma_buffer[write_idx].size);
		if(iddma_attr[dev].iddma_buffer[write_idx].crc <= 10)
		{
			rtd_pr_i3ddma_err("[IDDMA]i%ddma may output green data!!!!!\n", (dev + 3));
		}

		//delay one frame output buffer to ap
		//if(iddma_attr[dev].forcebg_en != TRUE)
		//{
			iddma_attr[dev].iddma_buffer[write_idx].flag &= ~_BIT(IDDMA_CAPTURE_BUFFER_FLAG_IDLE);
			iddma_attr[dev].iddma_buffer[write_idx].flag |= _BIT(IDDMA_CAPTURE_BUFFER_FLAG_WD);
			iddma_attr[dev].iddma_buffer[write_idx].weight += 1;
			iddma_attr[dev].iddma_buffer[write_idx].timestamp = stamp;
		//}
		//else
		//{
			iddma_attr[dev].forcebg_en = FALSE;
		//}
	}

	if((pre_stamp[dev] != 0) && ((pre_stamp[dev] - stamp) > 9000))
	{
		rtd_pr_i3ddma_err("[IDDMA]i%ddma over 100ms no update buffer!!!!!\n", (dev + 3));
	}
	pre_stamp[dev] = stamp;
	
	write_idx = find_available_buffer_index(dev);//(write_idx + 1) % iddma_attr[dev].buffer_num;
	if ((iddma_attr[dev].iddma_buffer[write_idx].phyaddr == 0) || !(iddma_attr[dev].iddma_buffer[write_idx].flag & _BIT(IDDMA_CAPTURE_BUFFER_FLAG_RDY)))
	{
		rtd_pr_i3ddma_err("[IDDMA]i%ddma memory no init idx=%d phy=%lld flag=%d!!!!!\n", (dev + 3), write_idx, iddma_attr[dev].iddma_buffer[write_idx].phyaddr, iddma_attr[dev].iddma_buffer[write_idx].flag);
		spin_unlock_irqrestore(&iddma_attr[dev].iddma_spinlock, flags);
		return FALSE;
	}
	iddma_attr[dev].write_point = write_idx;
	iddma_attr[dev].iddma_buffer[write_idx].flag &= ~_BIT(IDDMA_CAPTURE_BUFFER_FLAG_IDLE);
	iddma_attr[dev].iddma_buffer[write_idx].flag &= ~_BIT(IDDMA_CAPTURE_BUFFER_FLAG_WD);

	if(IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg) & _BIT29)
	{
		unsigned char i;
		for (i = 0; i < iddma_attr[dev].buffer_num; ++i)
		{
			rtd_pr_i3ddma_info("i%dddma-w-%d-----flag=%d--weight=%d--crc=%d---\n", (dev + 3), i, iddma_attr[dev].iddma_buffer[i].flag, iddma_attr[dev].iddma_buffer[i].weight, iddma_attr[dev].iddma_buffer[i].crc);
		}
	}

	ucBufAddr = iddma_attr[dev].iddma_buffer[write_idx].phyaddr;
	//rtd_pr_i3ddma_info("[IDDMA]i%ddma y statr addr=%x pnt=%d stamp=%lld!!!!!\n", (dev + 3), ucBufAddr, iddma_attr[dev].write_point, stamp);
	ucSize = get_image_y_buffer_size(dev);
	IoReg_Write32( iddma_cap1_start_reg_addr[dev] , ucBufAddr);
	ucBufAddr = ucBufAddr + ucSize;
	//drvif_memory_get_data_align(ucBufAddr, 4*1024);
	//rtd_pr_i3ddma_info("[IDDMA]i%ddma c statr addr=%x!!!!!\n", (dev + 3), ucBufAddr);
	IoReg_Write32( iddma_cap2_start_reg_addr[dev], ucBufAddr);
	if(iddma_attr[dev].format_is_3planes)
	{
		ucBufAddr = ucBufAddr + ucSize/4;
		IoReg_Write32(iddma_cap2_1_start_reg_addr[dev], ucBufAddr);
	}

	if(iddma_attr[dev].memory_type == IDDMA_MEMORY_DMA)
	{
		IoReg_Write32(iddma_cap1_boundary_low_limit_reg_addr[dev], iddma_attr[dev].iddma_buffer[write_idx].phyaddr);
		IoReg_Write32(iddma_cap1_boundary_up_limit_reg_addr[dev], iddma_attr[dev].iddma_buffer[write_idx].phyaddr + iddma_attr[dev].iddma_buffer[write_idx].getsize);

		IoReg_Write32(iddma_cap2_boundary_low_limit_reg_addr[dev], iddma_attr[dev].iddma_buffer[write_idx].phyaddr);
		IoReg_Write32(iddma_cap2_boundary_up_limit_reg_addr[dev], iddma_attr[dev].iddma_buffer[write_idx].phyaddr + iddma_attr[dev].iddma_buffer[write_idx].getsize);

		IoReg_Write32(iddma_cap2_1_boundary_low_limit_reg_addr[dev], iddma_attr[dev].iddma_buffer[write_idx].phyaddr);
		IoReg_Write32(iddma_cap2_1_boundary_up_limit_reg_addr[dev], iddma_attr[dev].iddma_buffer[write_idx].phyaddr + iddma_attr[dev].iddma_buffer[write_idx].getsize);
	}

	spin_unlock_irqrestore(&iddma_attr[dev].iddma_spinlock, flags);

	return TRUE;
}

unsigned char IDDMA_Check_IsI3Occpuy(void)
{
	if(iddma_attr[I3DDMA_DEV].start_buffer_addr || iddma_attr[I3DDMA_DEV].memory_type)
		return TRUE;
	else
		return FALSE;
}

extern phys_addr_t dmaheap_to_phys(int fd);
unsigned char IDDMA_QBuf(unsigned char dev, unsigned char index, int fd, unsigned int size, unsigned int bufferId)
{
	unsigned long flags;
	unsigned int after_crc = 0;

	//rtd_pr_i3ddma_info("[IDDMA] memory type:%d addr:%x\n", iddma_attr[dev].memory_type);
	spin_lock_irqsave(&iddma_attr[dev].iddma_spinlock, flags);
	if(iddma_attr[dev].memory_type == IDDMA_MEMORY_MMAP)
	{
		iddma_attr[dev].iddma_buffer[index].flag &= ~_BIT(IDDMA_CAPTURE_BUFFER_FLAG_QUEUD);
		iddma_attr[dev].iddma_buffer[index].flag |= _BIT(IDDMA_CAPTURE_BUFFER_FLAG_IDLE);
	}
	else if(iddma_attr[dev].memory_type == IDDMA_MEMORY_DMA)
	{
		iddma_attr[dev].iddma_buffer[index].phyaddr = dmaheap_to_phys(fd);
		if((iddma_attr[dev].iddma_buffer[index].phyaddr == INVALID_VAL) || (iddma_attr[dev].iddma_buffer[index].phyaddr == 0))
		{
			iddma_attr[dev].iddma_buffer[index].phyaddr = 0;
			spin_unlock_irqrestore(&iddma_attr[dev].iddma_spinlock, flags);
			rtd_pr_i3ddma_err("[IDDMA] converter fd=%d to phy address fail!!!\n", fd);
			return FALSE;
		}

		if(index == iddma_attr[dev].buffer_num)
		{
			iddma_attr[dev].iddma_buffer[index].flag |= _BIT(IDDMA_CAPTURE_BUFFER_FLAG_RDY);
			iddma_attr[dev].iddma_buffer[index].weight = iddma_attr[dev].iddma_buffer[0].weight;
			if((iddma_attr[dev].iddma_buffer[index].uncache == NULL) && (IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg) & _BIT29))
			{
				iddma_attr[dev].iddma_buffer[index].uncache = (unsigned char*)dvr_remap_uncached_memory(iddma_attr[dev].iddma_buffer[index].phyaddr, drvif_memory_get_data_align(iddma_attr[dev].iddma_buffer[index].size, __4KPAGE), __builtin_return_address(0));
			}
			rtd_pr_i3ddma_info("[IDDMA] i%dddma increase buffer num %d -> %d flag=%d\n", (dev+3), iddma_attr[dev].buffer_num, (index + 1), iddma_attr[dev].iddma_buffer[index].flag);
			iddma_attr[dev].buffer_num = index + 1;
		}
		else if(index > iddma_attr[dev].buffer_num)
		{
			rtd_pr_i3ddma_err("[IDDMA] i%dddma increase buffer num error %d -> %d flag=%d\n", (dev+3), iddma_attr[dev].buffer_num, index, iddma_attr[dev].iddma_buffer[index].flag);
			spin_unlock_irqrestore(&iddma_attr[dev].iddma_spinlock, flags);
			return FALSE;
		}
		
		iddma_attr[dev].iddma_buffer[index].size = size;
		iddma_attr[dev].iddma_buffer[index].getsize = size;

		if((iddma_attr[dev].iddma_buffer[index].flag & _BIT(IDDMA_CAPTURE_BUFFER_FLAG_QUEUD)) && iddma_attr[dev].iddma_buffer[index].uncache)
		{
			after_crc = calc_raw_data_crc_for_debug(dev, iddma_attr[dev].iddma_buffer[index].uncache, iddma_attr[dev].iddma_buffer[index].size);
			if(after_crc != iddma_attr[dev].iddma_buffer[index].crc)
			{
				rtd_pr_i3ddma_info("after dq/enq raw data crc change (%d %d),need check memory corruption\n", iddma_attr[dev].iddma_buffer[index].crc, after_crc);
			}
		}
		iddma_attr[dev].iddma_buffer[index].flag &= ~_BIT(IDDMA_CAPTURE_BUFFER_FLAG_QUEUD);
		iddma_attr[dev].iddma_buffer[index].flag &= ~_BIT(IDDMA_CAPTURE_BUFFER_FLAG_WD);
		iddma_attr[dev].iddma_buffer[index].flag |= _BIT(IDDMA_CAPTURE_BUFFER_FLAG_IDLE);

		if(IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg) & _BIT29)
		{
			unsigned char i;
			for (i = 0; i < iddma_attr[dev].buffer_num; ++i)
			{
				rtd_pr_i3ddma_info("i%dddma-q-%d-----flag=%d------\n", (dev+3), i, iddma_attr[dev].iddma_buffer[i].flag);
			}
		}

		if((iddma_attr[dev].iddma_buffer[index].fd != fd) && (iddma_attr[dev].iddma_buffer[index].fd != 0))
		{
			rtd_pr_i3ddma_info("[IDDMA] fd change maybe memory leak (%d %d) !!!\n", iddma_attr[dev].iddma_buffer[index].fd, fd);
		}
		iddma_attr[dev].iddma_buffer[index].fd = fd;
		iddma_attr[dev].iddma_buffer[index].bufferId = bufferId;
	}
	else
	{
		spin_unlock_irqrestore(&iddma_attr[dev].iddma_spinlock, flags);
		return FALSE;
	}

	spin_unlock_irqrestore(&iddma_attr[dev].iddma_spinlock, flags);

	//rtd_pr_i3ddma_info("[IDDMA] qbuf index:%d addr:%x\n", index, iddma_attr[dev].iddma_buffer[index].phyaddr);

	return TRUE;
}

unsigned char find_closest_buffer_index(unsigned char dev, unsigned long long stamp)
{
	unsigned char i;
	unsigned cloest_buf_idx = IDDMA_MAX_BUFFER_NUM;
	unsigned long long deita = 0;
	int ptrlo = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);
	int ptrhi = IoReg_Read32(TIMER_SCPU_CLK90K_HI_reg);
	unsigned long long curren_stamp = ptrlo | (((int64_t)ptrhi) << 32);

	for(i = 0; i < iddma_attr[dev].buffer_num; ++i)
	{
		if((iddma_attr[dev].iddma_buffer[i].timestamp > stamp) && (iddma_attr[dev].iddma_buffer[i].flag & _BIT(IDDMA_CAPTURE_BUFFER_FLAG_WD)))
		{
			if(deita == 0)
			{
				deita = iddma_attr[dev].iddma_buffer[i].timestamp - stamp;
				cloest_buf_idx = i;
			}
			else if((iddma_attr[dev].iddma_buffer[i].timestamp - stamp) > deita)
			{
				deita = iddma_attr[dev].iddma_buffer[i].timestamp - stamp;
				cloest_buf_idx = i;
			}
		}
	}

	if(IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg) & _BIT30)
	{
		for(i = 0; i < iddma_attr[dev].buffer_num; ++i)
		{
			if(!(iddma_attr[dev].iddma_buffer[i].flag & _BIT(IDDMA_CAPTURE_BUFFER_FLAG_QUEUD)))
			{
				cloest_buf_idx = i;
			}
		}
		rtd_pr_i3ddma_info("i%dddma force dq idx=%d %d\n", (dev+3), cloest_buf_idx, iddma_attr[dev].buffer_num);
	}

	if(cloest_buf_idx < IDDMA_MAX_BUFFER_NUM)
	{
		return cloest_buf_idx;
	}

	if(IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg) & _BIT29)
	{
		unsigned char i;
		for (i = 0; i < iddma_attr[dev].buffer_num; ++i)
		{
			rtd_pr_i3ddma_info("i%dddma dq-%d-----flag=%d------\n", (dev+3), i, iddma_attr[dev].iddma_buffer[i].flag);
		}
		rtd_pr_i3ddma_info("i%dddma dqBuf fail port=%d source=%d!! lincnt(%x %x %x %x) (%lld %lld)\n", (dev+3), iddma_attr[dev].hdmi_port, iddma_attr[dev].source_type, 
							IoReg_Read32(DMA_VGIP_DMA_VGIP_LC_reg), IoReg_Read32(I4_VGIP_I4_VGIP_LC_reg), IoReg_Read32(I5_VGIP_I5_VGIP_LC_reg), IoReg_Read32(I6_VGIP_I6_VGIP_LC_reg), curren_stamp, stamp);
	} 
	else if((curren_stamp - stamp) > 5400) //60ms
	{
		unsigned char i;
		for (i = 0; i < iddma_attr[dev].buffer_num; ++i)
		{
			rtd_pr_i3ddma_info("i%dddma dq-%d-----flag=%d------\n", (dev+3), i, iddma_attr[dev].iddma_buffer[i].flag);
		}
		rtd_pr_i3ddma_info("i%dddma dqBuf fail port=%d source=%d!! lincnt(%x %x %x %x) (%lld %lld)\n", (dev+3), iddma_attr[dev].hdmi_port, iddma_attr[dev].source_type, 
							IoReg_Read32(DMA_VGIP_DMA_VGIP_LC_reg), IoReg_Read32(I4_VGIP_I4_VGIP_LC_reg), IoReg_Read32(I5_VGIP_I5_VGIP_LC_reg), IoReg_Read32(I6_VGIP_I6_VGIP_LC_reg), curren_stamp, stamp);
	}

	return IDDMA_MAX_BUFFER_NUM;
}

void vfe_disconnect_force_set_buffer_write_done(unsigned char dev)
{
	unsigned char i = 0;
	int ptrlo = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);
	int ptrhi = IoReg_Read32(TIMER_SCPU_CLK90K_HI_reg);
	unsigned long long stamp = ptrlo | (((int64_t)ptrhi) << 32);

	for(i = 0; i < iddma_attr[dev].buffer_num; ++i)
	{
		if(iddma_attr[dev].iddma_buffer[i].flag & _BIT(IDDMA_CAPTURE_BUFFER_FLAG_IDLE))
		{
			iddma_attr[dev].iddma_buffer[i].flag &= ~_BIT(IDDMA_CAPTURE_BUFFER_FLAG_IDLE);
			iddma_attr[dev].iddma_buffer[i].flag |= _BIT(IDDMA_CAPTURE_BUFFER_FLAG_WD);
			iddma_attr[dev].iddma_buffer[i].weight += 1;
			iddma_attr[dev].iddma_buffer[i].timestamp = stamp + 1;
		}
	}
}

int IDDMA_DQBuf(unsigned char dev, unsigned int* pIdx, int* fd, unsigned int* bufferId)
{
	unsigned int ret = 0;
	unsigned int read_idx = 0;
	unsigned long flags;
	unsigned long long stamp = 0;
	unsigned char port = iddma_attr[dev].hdmi_port;
	unsigned char disconnect_state = 0;
	unsigned char check_mode = TRUE;

	port = iddma_attr[dev].hdmi_port;
	if(iddma_attr[dev].source_type == VSC_INPUTSRC_HDMI)
	{
		vfe_hdmi_connect_state_t hdmi_state = {0, 0};
		hdmi_state.port = port;
		vfe_hdmi_drv_get_connection_state(&hdmi_state);
		disconnect_state = hdmi_state.state;

		check_mode = drvif_Hdmi_Multi_port_CheckMode(port);
	}
	else if(iddma_attr[dev].source_type == VSC_INPUTSRC_DP)
	{
		vfe_dprx_drv_get_connection_state(port, &disconnect_state);

		check_mode = drvif_Dprx_CheckPortMode(port);
	}

	spin_lock_irqsave(&iddma_attr[dev].iddma_spinlock, flags);

	if(iddma_attr[dev].read_point == IDDMA_MAX_BUFFER_NUM)
	{
		if(iddma_attr[dev].write_point > 0)
		{
			iddma_attr[dev].read_point = iddma_attr[dev].write_point - 1;
			read_idx = iddma_attr[dev].read_point;
		}
		else
		{
			iddma_attr[dev].read_point = iddma_attr[dev].buffer_num - 1;
			read_idx = iddma_attr[dev].read_point;
		}
		if(iddma_attr[dev].iddma_buffer[read_idx].timestamp > 0)
			stamp = iddma_attr[dev].iddma_buffer[read_idx].timestamp - 1;
		else
			stamp = 0;
	}
	else
	{
		read_idx = iddma_attr[dev].read_point;
		stamp = iddma_attr[dev].iddma_buffer[read_idx].timestamp;
	}

	if((disconnect_state == 0) || (check_mode == FALSE))
	{
		iddma_attr[dev].forcebg_en = TRUE;
		vfe_disconnect_force_set_buffer_write_done(dev);
		ret = -ENOLINK;
	}

	if((iddma_attr[dev].forcebg_en == TRUE) && ((disconnect_state == 0) || (check_mode == FALSE)))
	{
		vfe_disconnect_force_set_buffer_write_done(dev);
		ret = -ENOLINK;
	}
	
	if((read_idx = find_closest_buffer_index(dev, stamp)) < IDDMA_MAX_BUFFER_NUM)
	{
		if(read_idx >= iddma_attr[dev].buffer_num)
			rtd_pr_i3ddma_info("i%dddma-dq index error-%d-----flag=%d------\n", (dev + 3), read_idx, iddma_attr[dev].iddma_buffer[read_idx].flag);
		iddma_attr[dev].iddma_buffer[read_idx].flag &= ~_BIT(IDDMA_CAPTURE_BUFFER_FLAG_WD);
		iddma_attr[dev].iddma_buffer[read_idx].flag |= _BIT(IDDMA_CAPTURE_BUFFER_FLAG_QUEUD);

		if(IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg) & _BIT29)
		{
			unsigned char i;
			for (i = 0; i < iddma_attr[dev].buffer_num; ++i)
			{
				rtd_pr_i3ddma_info("i%dddma-dq-%d-----flag=%d------\n", (dev + 3), i, iddma_attr[dev].iddma_buffer[i].flag);
			}
		}
		*pIdx = read_idx;
		*fd = iddma_attr[dev].iddma_buffer[read_idx].fd;
		*bufferId = iddma_attr[dev].iddma_buffer[read_idx].bufferId;
		iddma_attr[dev].read_point = read_idx;//(read_idx + 1) % iddma_attr[dev].buffer_num;
		//rtd_pr_i3ddma_info("[IDDMA] dqBuf i%dddma write:%d read:%d fd:%d ret:%d !!\n", (dev + 3), iddma_attr[dev].write_point, read_idx, *fd, ret);
	}
	else
	{
		ret = -EAGAIN;
	}

	spin_unlock_irqrestore(&iddma_attr[dev].iddma_spinlock, flags);

	return ret;
}

spinlock_t* IDDMA_GetSpinLock(unsigned dev)
{
	return &iddma_attr[dev].iddma_spinlock;
}

void h3ddma_nn_run_capture_config(unsigned char dev, I3DDMA_TIMING_T *timing)
{
	if((IDDMA_Get_DMAWorkMode(dev) == IDDMA_NN_MODE) || (IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE))
	{
		if(timing == NULL)
		{
			g_ulNNInWidth = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_WID_PRE);
			g_ulNNInLength = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPV_ACT_LEN_PRE);
		}
		else
		{
			g_ulNNInWidth = timing->h_act_len;
			g_ulNNInLength = timing->v_act_len;
		}

		if(IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_SOURCE_PIXEL_MODE))
		{
			if(DMA_VGIP_DMA_VGIP_CTRL_get_i3_2p_gate_sel(IoReg_Read32(vgip_ctrl_reg_addr[dev])))
				g_ulNNOutWidth = (g_ulNNInWidth) / 2;
			else
				g_ulNNOutWidth = (g_ulNNInWidth) / 4;
			g_ulNNOutLength = (g_ulNNInLength) / 4;
		}
		else if((g_ulNNInWidth * g_ulNNInLength) > 1920*1080)
		{
			g_ulNNOutWidth = (g_ulNNInWidth) / 4;
			g_ulNNOutLength = (g_ulNNInLength) / 4;
		}
		// 2k > input size > 1k
		else if ((g_ulNNInWidth * g_ulNNInLength) > NN_CAP_BUFFER_MAX_W_SIZE * NN_CAP_BUFFER_MAX_H_SIZE)
		{
			g_ulNNOutWidth = (g_ulNNInWidth) / 2;
			g_ulNNOutLength = (g_ulNNInLength) / 2;
		}
		else
		{
			g_ulNNOutWidth = g_ulNNInWidth;
			g_ulNNOutLength = g_ulNNInLength;
		}

		if((g_ulNNOutWidth * g_ulNNOutLength) > NN_CAP_BUFFER_MAX_W_SIZE * NN_CAP_BUFFER_MAX_H_SIZE)
		{
			g_ulNNOutWidth = NN_CAP_BUFFER_MAX_W_SIZE;
			g_ulNNOutLength = NN_CAP_BUFFER_MAX_H_SIZE;
		}

		//SE can't support 8x uzd
		if(g_ulNNOutWidth > NN_CAP_BUFFER_MAX_W_SIZE)
		{
			g_ulNNOutWidth = NN_CAP_BUFFER_MAX_W_SIZE;
		}

		if(g_ulNNOutWidth % 16 != 0)
		{
			g_ulNNOutWidth = (g_ulNNOutWidth & ~0xF);
		}

		g_stI3ddmaNNCropAttr.x = 0;
		g_stI3ddmaNNCropAttr.y = 0;
		g_stI3ddmaNNCropAttr.w = g_ulNNOutWidth;
		g_stI3ddmaNNCropAttr.h = g_ulNNOutLength;
		g_stI3ddmaNNCropAttr.h_num = 0;
		g_stI3ddmaNNCropAttr.h_dis = 0;
		g_stI3ddmaNNCropAttr.v_num = 0;
		g_stI3ddmaNNCropAttr.v_dis = 0;

		h3ddma_nn_set_recfg_flag(FALSE);

		//i3ddma capture and sub path each have 3buffers
		if(h3ddma_allocate_nv12_mermory(NN_CAP_BUFFER_MAX_W_SIZE, NN_CAP_BUFFER_MAX_H_SIZE, (NN_CAP_BUFFER_NUM)) == FALSE)
			return;

		//drvif_h3ddma_set_sequence_capture0(timing);

		rtd_pr_i3ddma_notice("iddma cap mode input and output size, in_width=%d, in_height=%d, out_width=%d, out_height=%d\n", g_ulNNInWidth, g_ulNNInLength, g_ulNNOutWidth, g_ulNNOutLength);
		h3ddma_nn_decide_cap_mode();

		h3ddma_nn_set_buffer_addr(0);
		//h3ddma_set_film_buffer_addr(1);

		//reset multi crop
		h3ddma_nn_config_multi_crop(g_stI3ddmaNNCropAttr);

		//send nn cap info to vcpu for cap switching
		h3ddma_nn_send_cap_info();

		//enable i3ddma capture
		drvif_h3ddma_nn_set_capture(dev, g_ulNNOutWidth, g_ulNNOutLength, 0);
	}
	else
	{
		StructSrcRect output = get_vt_output_size(dev);
		if(iddma_attr[dev].buffer_num > IDDMA_MAX_BUFFER_NUM)
			return;

		if(iddma_attr[dev].buffer_num == 0)
			iddma_attr[dev].buffer_num = IDDMA_DEFAULT_BUFFER_NUM;

		if((IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_WID_PRE) == 0) || (IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPV_ACT_LEN_PRE) == 0))
			return;

		iddma_attr[dev].input_region.srcx = 0;
		iddma_attr[dev].input_region.srcy = 0;
		if(IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_SOURCE_PIXEL_MODE))
		{
			if(dev != I3DDMA_DEV)
				iddma_attr[dev].input_region.src_wid  = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_WID_PRE)/2;
			else
				iddma_attr[dev].input_region.src_wid  = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_WID_PRE);
		}
		else
			iddma_attr[dev].input_region.src_wid  = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_WID_PRE);
		iddma_attr[dev].input_region.src_height  = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPV_ACT_LEN_PRE);

		iddma_attr[dev].pitch = get_vt_pitch(dev);

		iddma_attr[dev].output_region.srcx = 0;
		iddma_attr[dev].output_region.srcy = 0;
		if(output.src_wid != 0)
		{
			if(iddma_attr[dev].input_region.src_wid > output.src_wid)
				iddma_attr[dev].output_region.src_wid = output.src_wid;
			else
				iddma_attr[dev].output_region.src_wid = iddma_attr[dev].input_region.src_wid;
		}
		else
		{
			if(iddma_attr[dev].input_region.src_wid > IDDMA_DEFAULT_OUTPUT_WIDTH)
				iddma_attr[dev].output_region.src_wid = IDDMA_DEFAULT_OUTPUT_WIDTH;
			else
				iddma_attr[dev].output_region.src_wid = iddma_attr[dev].input_region.src_wid;
		}

		if(output.src_height != 0)
		{
			if(iddma_attr[dev].input_region.src_height > output.src_height)
				iddma_attr[dev].output_region.src_height = output.src_height;
			else
				iddma_attr[dev].output_region.src_height = iddma_attr[dev].input_region.src_height;
		}
		else
		{
			if(iddma_attr[dev].input_region.src_height > IDDMA_DEFAULT_OUTPUT_HEIGHT)
				iddma_attr[dev].output_region.src_height = IDDMA_DEFAULT_OUTPUT_HEIGHT;
			else
				iddma_attr[dev].output_region.src_height = iddma_attr[dev].input_region.src_height;
		}

		rtd_pr_i3ddma_notice("[IDDMA] i%ddma input size(%d %d)\n", (dev+3), iddma_attr[dev].input_region.src_wid, iddma_attr[dev].input_region.src_height);
		rtd_pr_i3ddma_notice("[IDDMA] i%ddma output size(%d %d) pitch:%d\n", (dev+3), iddma_attr[dev].output_region.src_wid, iddma_attr[dev].output_region.src_height, iddma_attr[dev].pitch);

		if(IDDMA_Allocat_memoryBuf(dev) != TRUE)
			return;

		if(IDDMA_Set_CapStartAddr(dev) != TRUE)
			return;

		drvif_h3ddma_nn_set_capture(dev, iddma_attr[dev].output_region.src_wid, iddma_attr[dev].output_region.src_height, iddma_attr[dev].pitch);
	}
}


unsigned int h3ddma_get_NN_start_buffer_addr(void)
{
	return g_tNN_Cap_Buffer[0].phyaddr;
}

void h3ddma_get_NN_read_idx(void)
{
	unsigned int BufAddr1 = 0;
	unsigned char i = 0;
	BufAddr1 = IoReg_Read32(H3DDMA_CAP1_WR_DMA_num_bl_wrap_addr_0_reg);

	g_ucNNReadIdx = 0xFF;

	for(i = 0; i < NN_CAP_BUFFER_NUM; i++)
	{
		if(BufAddr1 == g_tNN_Cap_Buffer[i].phyaddr)
		{
			g_ucNNReadIdx = (i + NN_CAP_BUFFER_NUM - 1) % NN_CAP_BUFFER_NUM;
		}

		if(g_ucNNReadIdx != 0xFF)
			break;
	}

	if(i == NN_CAP_BUFFER_NUM)
	{
		g_ucNNReadIdx = 0;
		rtd_pr_i3ddma_err("h3ddma_get_NN_read_idx error, BufAddr=%x\n", BufAddr1);
	}
}


int h3ddma_get_NN_read_buffer(unsigned int *a_pulYAddr, unsigned int *a_pulCAddr, unsigned long long *uzd_timestamp, unsigned int *a_pulCropYAddr, unsigned int *a_pulCropCAddr, unsigned long long *crop_timestamp)
{
	unsigned char i = 0;
	unsigned char uzd_index = 0xFF;
	unsigned char crop_index = 0xFF;
	unsigned long long uzd_max_stamp = 0;
	unsigned long long crop_max_stamp = 0;
	//unsigned int ulBufAddr = 0;
	//unsigned int ulBufSize = 0;
	I3DDMA_NN_BUFFER_ATTR_T i3ddma_nn_buffer_attr_addr[NN_CAP_BUFFER_NUM];
	I3DDMA_NN_BUFFER_ATTR_T* p_i3ddma_nn_buffer_attr_addr = NULL;

	if((unsigned char *)Scaler_GetShareMemVirAddr(SCALERIOC_I3_NN_SHARE_BUF_STATUS) == NULL)
	{
		rtd_pr_i3ddma_err("[NN][%s] RPC not init!\n", __FUNCTION__);
		return -1;
	}

	if(H3DDMA_Interrupt_Enable_get_cap1_last_wr_int_mux(IoReg_Read32(H3DDMA_Interrupt_Enable_reg)))
	{
		for (i = 0; i < NN_CAP_BUFFER_NUM; i++)
		{
			p_i3ddma_nn_buffer_attr_addr = (I3DDMA_NN_BUFFER_ATTR_T *)Scaler_GetShareMemVirAddr(SCALERIOC_I3_NN_SHARE_BUF_STATUS) + i;

			//rtd_pr_i3ddma_info("addr = %x, mode = %d, buffer status=%d", p_i3ddma_nn_buffer_attr_addr->y_buffer_addr, p_i3ddma_nn_buffer_attr_addr->cap_mode, p_i3ddma_nn_buffer_attr_addr->buf_status);
			i3ddma_nn_buffer_attr_addr[i].cap_mode = Scaler_ChangeUINT32Endian(p_i3ddma_nn_buffer_attr_addr->cap_mode);
			i3ddma_nn_buffer_attr_addr[i].buf_status = Scaler_ChangeUINT32Endian(p_i3ddma_nn_buffer_attr_addr->buf_status);
			i3ddma_nn_buffer_attr_addr[i].y_buffer_addr = Scaler_ChangeUINT32Endian(p_i3ddma_nn_buffer_attr_addr->y_buffer_addr);
			i3ddma_nn_buffer_attr_addr[i].c_buffer_addr = Scaler_ChangeUINT32Endian(p_i3ddma_nn_buffer_attr_addr->c_buffer_addr);
			i3ddma_nn_buffer_attr_addr[i].timestamp = Scaler_ChangeUINT64Endian(p_i3ddma_nn_buffer_attr_addr->timestamp);

			//rtd_pr_i3ddma_info("addr = %x, mode = %d, buffer status=%d addr  %x", i3ddma_nn_buffer_attr_addr[i].y_buffer_addr, i3ddma_nn_buffer_attr_addr[i].cap_mode, i3ddma_nn_buffer_attr_addr[i].buf_status,i3ddma_nn_buffer_attr_addr[i].y_buffer_addr);
		}
	}
	else
	{
		memcpy((unsigned char *)i3ddma_nn_buffer_attr_addr, (unsigned char *)Scaler_GetShareMemVirAddr(SCALERIOC_I3_NN_SHARE_BUF_STATUS), sizeof(I3DDMA_NN_BUFFER_ATTR_T) * NN_CAP_BUFFER_NUM);
	}

	if(h3ddma_get_cap_enable_mask() == 0)
	{
		//rtd_pr_i3ddma_info("[NNIP][Error] i3ddma NN disable\n");
		return -1;
	}
#if 0
	h3ddma_get_NN_read_idx();

	ulBufAddr = g_tNN_Cap_Buffer[g_ucNNReadIdx].phyaddr;
	ulBufSize = g_tNN_Cap_Buffer[g_ucNNReadIdx].size;

	if (0 == ulBufAddr)
	{
		rtd_pr_i3ddma_info("h3ddma_get_NN_read_buffer error (phyaddr = 0), Idx=%d\n", g_ucNNReadIdx);
		return -1;
	}

	if((a_pulYAddr != NULL) && (a_pulCAddr != NULL))
	{
		*a_pulYAddr = ulBufAddr;
		*a_pulCAddr = g_tNN_Cap_Buffer[0].phyaddr + ulBufSize * NN_CAP_BUFFER_NUM + (ulBufSize/2)*g_ucNNReadIdx;
	}

	if((a_pulCropYAddr != NULL) && (a_pulCropCAddr != NULL))
	{
		*a_pulCropYAddr = g_tNN_Cap_Buffer[g_ucResDetRdIdx + NN_CAP_BUFFER_NUM].phyaddr;
		*a_pulCropCAddr = g_tNN_Cap_Buffer[NN_CAP_BUFFER_NUM].phyaddr + ulBufSize * NN_CAP_BUFFER_NUM + (ulBufSize/2)*g_ucResDetRdIdx;
	}

	//rtd_pr_i3ddma_debug("face idx=%d, resolution idx=%d, phyaddr=%x, Y_Addr=%x, C_Addr=%x\n", g_ucNNReadIdx, g_ucResDetRdIdx, ulBufAddr, *a_pulYAddr, *a_pulCAddr);
#endif
	for(i = 0; i < NN_CAP_BUFFER_NUM; i++)
	{
		I3DDMA_NN_BUFFER_ATTR_T* pCurrFrame = &i3ddma_nn_buffer_attr_addr[i];
		if((pCurrFrame->cap_mode == I3DDMA_UZD_CAP) && (pCurrFrame->buf_status == I3DDMA_NN_BUFF_WRITE_DONE) && (pCurrFrame->timestamp > uzd_max_stamp))
		{
			uzd_index = i;
			uzd_max_stamp = pCurrFrame->timestamp;
		}

		if((pCurrFrame->cap_mode == I3DDMA_CROP_CAP) && (pCurrFrame->buf_status == I3DDMA_NN_BUFF_WRITE_DONE) && (pCurrFrame->timestamp > crop_max_stamp))
		{
			crop_index = i;
			crop_max_stamp = pCurrFrame->timestamp;
		}

		//rtd_pr_i3ddma_info("mode=%d, buf status=%d y_addr=%x, c_addr=%x, timestamp=%lld", pCurrFrame->cap_mode, pCurrFrame->buf_status,
		//			pCurrFrame->y_buffer_addr, pCurrFrame->c_buffer_addr, pCurrFrame->timestamp);
	}

	if((uzd_index > NN_CAP_BUFFER_NUM) && (crop_index > NN_CAP_BUFFER_NUM))
	{
		rtd_pr_i3ddma_err("h3ddma_get_NN_read_buffer get buffer fail uzd_index= %d crop_index= %d \n",uzd_index,crop_index);
		return -1;
	}

	if(uzd_index < NN_CAP_BUFFER_NUM)
	{
		if((a_pulYAddr != NULL) && (a_pulCAddr != NULL))
		{
			*a_pulYAddr = i3ddma_nn_buffer_attr_addr[uzd_index].y_buffer_addr;
			*a_pulCAddr = i3ddma_nn_buffer_attr_addr[uzd_index].c_buffer_addr;
			if(uzd_timestamp != NULL)
				*uzd_timestamp = i3ddma_nn_buffer_attr_addr[uzd_index].timestamp;
		}
	}

	if(crop_index < NN_CAP_BUFFER_NUM)
	{
		if((a_pulCropYAddr != NULL) && (a_pulCropCAddr != NULL))
		{
			*a_pulCropYAddr = i3ddma_nn_buffer_attr_addr[crop_index].y_buffer_addr;
			*a_pulCropCAddr = i3ddma_nn_buffer_attr_addr[crop_index].c_buffer_addr;
			if(crop_timestamp != NULL)
				*crop_timestamp = i3ddma_nn_buffer_attr_addr[crop_index].timestamp;
		}
	}

	return i;
}


void h3ddma_get_NN_output_size(unsigned int *outputWidth, unsigned int *outputLength)
{
   *outputWidth = g_ulNNOutWidth;
   *outputLength = g_ulNNOutLength;
}
void h3ddma_NN_set_cap_ratio(unsigned char cap_ratio)
{
	g_enNNCapRatio = cap_ratio;
}

unsigned char h3ddma_NN_get_cap_ratio(void)
{
	return g_enNNCapRatio;
}

void h3ddma_nn_config_multi_crop(I3DDMA_NN_CROP_ATTR_T i3ddma_nn_atrr)
{
	h3ddma_hsd_i3ddma1_uzd_cutout_range_hor_RBUS h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg;
	h3ddma_hsd_i3ddma1_uzd_cutout_range_ver_RBUS h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg;
	h3ddma_hsd_i3ddma1_uzd_cutout_dis_num_hor_RBUS h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg;
	h3ddma_hsd_i3ddma1_uzd_cutout_dis_num_ver_RBUS h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg;
	h3ddma_hsd_i3ddma1_uzd_v_db_ctrl_RBUS h3ddma_hsd_i3ddma_v_db_ctrl_reg;
	h3ddma_hsd_i3ddma1_uzd_h_db_ctrl_RBUS h3ddma_hsd_i3ddma_h_db_ctrl_reg;

	if((i3ddma_nn_atrr.w == 0) || (i3ddma_nn_atrr.h == 0) ||
		(i3ddma_nn_atrr.w > g_ulNNInWidth) || (i3ddma_nn_atrr.h > g_ulNNInWidth))
	{
		//printk("[i3ddma][NN]crop size is wrong(%d,%d).\n", i3ddma_nn_atrr.w, i3ddma_nn_atrr.h);
		return;
	}

	//hw constraint
	if((i3ddma_nn_atrr.h_num != 0) && ((i3ddma_nn_atrr.x == 0) || (i3ddma_nn_atrr.x == 1)))
	{
		return;
	}

	if((i3ddma_nn_atrr.x + i3ddma_nn_atrr.w) > g_ulNNInWidth)
	{
		//printk("[i3ddma][NN]crop width size is wrong(%d,%d).\n", i3ddma_nn_atrr.x, i3ddma_nn_atrr.w);
		return;
	}

	if((i3ddma_nn_atrr.y + i3ddma_nn_atrr.h) > g_ulNNInLength)
	{
		//printk("[i3ddma][NN]crop length size is wrong(%d,%d).\n", i3ddma_nn_atrr.y, i3ddma_nn_atrr.h);
		return;
	}

	if(i3ddma_nn_atrr.h_num > 15)
	{
		//printk("[i3ddma][NN] h_num over size= %d is wrong.\n", i3ddma_nn_atrr.h_num);
		return;
	}

	if((i3ddma_nn_atrr.h_dis & _BIT0) != 0)
	{
		//printk("[i3ddma][NN] h_dis is odd= %d is wrong.\n", i3ddma_nn_atrr.h_dis);
		return;
	}

	if(i3ddma_nn_atrr.v_num > 7)
	{
		//printk("[i3ddma][NN] v_num  over size= %d is wrong.\n", i3ddma_nn_atrr.v_num);
		return;
	}

	if((i3ddma_nn_atrr.v_dis & _BIT0) != 0)
	{
		//printk("[i3ddma][NN] v_dis is odd = %d is wrong.\n", i3ddma_nn_atrr.v_dis);
		return;
	}

	if((i3ddma_nn_atrr.x + i3ddma_nn_atrr.w * (i3ddma_nn_atrr.h_num + 1) + i3ddma_nn_atrr.h_num * i3ddma_nn_atrr.h_dis) > g_ulNNInWidth)
	{
		//printk("[i3ddma][NN] h_num=%d h_dis=%d is wrong \n", i3ddma_nn_atrr.h_num, i3ddma_nn_atrr.h_dis);
		return;
	}

	if(i3ddma_nn_atrr.w * (i3ddma_nn_atrr.h_num + 1) != g_ulNNOutWidth)
	{
		return;
	}

	if((i3ddma_nn_atrr.y + i3ddma_nn_atrr.h * (i3ddma_nn_atrr.v_num + 1) + i3ddma_nn_atrr.v_num * i3ddma_nn_atrr.v_dis) > g_ulNNInLength)
	{
		//printk("[i3ddma][NN] v_num=%d v_dis=%d is wrong \n", i3ddma_nn_atrr.v_num, i3ddma_nn_atrr.v_dis);
		return;
	}

	if(i3ddma_nn_atrr.h * (i3ddma_nn_atrr.v_num + 1) != g_ulNNOutLength)
	{
		return;
	}

	g_stI3ddmaNNCropAttr = i3ddma_nn_atrr;

	h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg);
	h3ddma_hsd_i3ddma_v_db_ctrl_reg.v_db_en = 1;
	IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg, h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue);
	h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg);
	h3ddma_hsd_i3ddma_h_db_ctrl_reg.h_db_en = 1;
	IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg, h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue);

	h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_RANGE_HOR_reg);
	h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg.hor_front = i3ddma_nn_atrr.x;
	h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg.hor_width = i3ddma_nn_atrr.w;
	IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_RANGE_HOR_reg, h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg.regValue);

	h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_RANGE_VER_reg);
	h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg.ver_front = i3ddma_nn_atrr.y;
	h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg.ver_height = i3ddma_nn_atrr.h;
	IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_RANGE_VER_reg, h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg.regValue);

	if(i3ddma_nn_atrr.h_num != 0)
	{
		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_HOR_reg);
		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg.hor_num = i3ddma_nn_atrr.h_num;
		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg.hor_dis = i3ddma_nn_atrr.h_dis;
		IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_HOR_reg, h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg.regValue);

		if(i3ddma_nn_atrr.v_num != 0)
		{
			h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_VER_reg);
			h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.ver_num = i3ddma_nn_atrr.v_num;
			h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.ver_dis = i3ddma_nn_atrr.v_dis;
			IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_VER_reg, h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.regValue);
		}
		else
		{
			h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_VER_reg);
			h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.ver_num = 0;
			h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.ver_dis = 0;
			IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_VER_reg, h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.regValue);
		}
	}
	else
	{
		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_HOR_reg);
		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg.hor_num = 0;
		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg.hor_dis = 0;
		IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_HOR_reg, h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg.regValue);

		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_VER_reg);
		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.ver_num = 0;
		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.ver_dis = 0;
		IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_VER_reg, h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.regValue);
	}

	h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg);
	h3ddma_hsd_i3ddma_v_db_ctrl_reg.v_db_apply = 1;
	IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg, h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue);
	h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg);
	h3ddma_hsd_i3ddma_h_db_ctrl_reg.h_db_apply = 1;
	IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg, h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue);
}



void h3ddma_nn_set_recfg_flag(unsigned char bVal)
{
	g_chVdecReCfg = bVal;
}

unsigned char h3ddma_nn_get_recfg_flag(void)
{
	return g_chVdecReCfg;
}

#endif //end of #ifdef CONFIG_ENABLE_HDMI_NN
#endif //end of #ifndef BUILD_QUICK_SHOW


#ifndef BUILD_QUICK_SHOW
void h3ddma_capture_dma_freeze(unsigned char dma_num, unsigned char enable)
{
	h3ddma_cap0_cap_ctl0_RBUS	h3ddma_cap0_cap_ctl0_reg;
	h3ddma_cap1_cap_ctl0_RBUS	h3ddma_cap1_cap_ctl0_reg;
	h3ddma_cap2_cap_ctl0_RBUS	h3ddma_cap2_cap_ctl0_reg;
	h3ddma_cap3_cap_ctl0_RBUS	h3ddma_cap3_cap_ctl0_reg;

	switch(dma_num)
	{
		case 0:
			h3ddma_cap0_cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_CAP0_Cap_CTL0_reg);
			h3ddma_cap0_cap_ctl0_reg.cap0_freeze_en = enable;
			IoReg_Write32(H3DDMA_CAP0_Cap_CTL0_reg, h3ddma_cap0_cap_ctl0_reg.regValue);
			break;
		case 1:
			h3ddma_cap1_cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_CAP1_Cap_CTL0_reg);
			h3ddma_cap1_cap_ctl0_reg.cap1_freeze_en = enable;
			IoReg_Write32(H3DDMA_CAP1_Cap_CTL0_reg, h3ddma_cap1_cap_ctl0_reg.regValue);
			break;
		case 2:
			h3ddma_cap2_cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_CAP2_Cap_CTL0_reg);
			h3ddma_cap2_cap_ctl0_reg.cap2_freeze_en = enable;
			IoReg_Write32(H3DDMA_CAP2_Cap_CTL0_reg, h3ddma_cap2_cap_ctl0_reg.regValue);
			break;
		case 3:
			h3ddma_cap3_cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_CAP3_Cap_CTL0_reg);
			h3ddma_cap3_cap_ctl0_reg.cap3_freeze_en = enable;
			IoReg_Write32(H3DDMA_CAP3_Cap_CTL0_reg, h3ddma_cap3_cap_ctl0_reg.regValue);
			break;
		default:
			rtd_pr_i3ddma_emerg("h3ddma_capture_dma_freeze dma_num[%d] error\n", dma_num);
			break;
	}


}
#endif

//VO_DMEM *VO_Dmem=NULL;
//VO_DATA *vo=NULL;

#define VO_MAIN_DCU_INDEX_Y     0
#define VO_MAIN_DCU_INDEX_C1    1
#define VO_MAIN_DCU_INDEX_C2    2
#define VO_MAIN_SEQ_ADDR        ((unsigned int)(0x08200000))


extern VODMA_MODE Mode_Decision(unsigned int width, unsigned int height, bool isProg);
extern HDR_MODE HDR_DolbyVision_Get_CurDolbyMode(void);
extern void Scaler_I2rnd_set_display(unsigned char display);

#if defined(CONFIG_ENABLE_HDR10_HDMI_AUTO_DETECT)|| defined(CONFIG_ENABLE_DOLBY_VISION_HDMI_AUTO_DETECT)
extern DOLBY_HDMI_MODE HDR_DolbyVision_Get_CurHdmiMode(void);
#endif

#ifndef BUILD_QUICK_SHOW

//config sub vi path start
#ifdef CONFIG_ENABLE_HDMI_NN
void h3ddma_nn_set_crop_size(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	h3ddma_hsd_i3ddma1_uzd_cutout_range_hor_RBUS h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg;
	h3ddma_hsd_i3ddma1_uzd_cutout_range_ver_RBUS h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg;
	h3ddma_hsd_i3ddma1_uzd_v_db_ctrl_RBUS h3ddma_hsd_i3ddma_v_db_ctrl_reg;
	h3ddma_hsd_i3ddma1_uzd_h_db_ctrl_RBUS h3ddma_hsd_i3ddma_h_db_ctrl_reg;

	if((w == 0) || (h == 0) || (w > g_ulNNInWidth) || (h > g_ulNNInLength))
	{
		//printk("[i3ddma][NN][h3ddma_nn_set_crop_size]crop size is wrong(%d,%d).\n", w, h);
		return;
	}

	if((x + w) > g_ulNNInWidth)
	{
		//printk("[i3ddma][NN][h3ddma_nn_set_crop_size]crop width size is wrong(%d,%d).\n", x, w);
		return;
	}

	if((y + h) > g_ulNNInLength)
	{
		//printk("[i3ddma][NN][h3ddma_nn_set_crop_size]crop length size is wrong(%d,%d).\n", y, h);
		return;
	}

	if(w != g_ulNNOutWidth)
	{
		return;
	}

	if(h != g_ulNNOutLength)
	{
		return;
	}

	g_stI3ddmaNNCropAttr.x = x;
	g_stI3ddmaNNCropAttr.y = y;
	g_stI3ddmaNNCropAttr.w = w;
	g_stI3ddmaNNCropAttr.h = h;

	h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg);
	h3ddma_hsd_i3ddma_v_db_ctrl_reg.v_db_en = 1;
	IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg, h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue);
	h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg);
	h3ddma_hsd_i3ddma_h_db_ctrl_reg.h_db_en = 1;
	IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg, h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue);

	h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_RANGE_HOR_reg);
	h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg.hor_front = x;
	h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg.hor_width = w;
	IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_RANGE_HOR_reg, h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg.regValue);

	h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_RANGE_VER_reg);
	h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg.ver_front = y;
	h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg.ver_height = h;
	IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_RANGE_VER_reg, h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg.regValue);

	h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg);
	h3ddma_hsd_i3ddma_v_db_ctrl_reg.v_db_apply = 1;
	IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg, h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue);
	h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg);
	h3ddma_hsd_i3ddma_h_db_ctrl_reg.h_db_apply = 1;
	IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg, h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue);

	//h3ddma_nn_set_sub_sdnr();
}
#endif


void h3mdda_set_dcmt(unsigned int mode)
{
	//#define MIN_3(a,b,c)                (((a) > (b)) ? (((b) > (c)) ? (c) : (b)) : (((a) > (c)) ? (c) : (a)))
	#define I3DDMA_INVAL_ADDR (0x7FFFFFF0)

	unsigned int cap0_vbm_addr = 0;		//borrow from vbm
	unsigned int cap0_vbm_up_limit = 0;
	unsigned int cap0_mdm_addr = 0;		//borrow from vbm or m-domian
	unsigned int cap0_mdm_up_limit = 0;
	unsigned int cap0_addr = 0;
	unsigned int cap0_up_limit = 0;
	unsigned int cap1_addr = 0;				//NN start address
	unsigned int cap1_up_limit = 0;
	unsigned int cap3_addr = 0;			//dolby metadata start address
	unsigned int cap3_up_limit = 0;
	//unsigned int start_addr = 0;
	//unsigned int mem_size = 0;
	//unsigned int cap_up_limit = 0;

	if(mode == 3)
	{
		cap0_vbm_addr = i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr;           //borrow from vbm
		cap0_vbm_up_limit = i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr + i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].getsize;
		cap0_mdm_addr = i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].phyaddr;									//borrow from vbm or m-domian
		cap0_mdm_up_limit = i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].phyaddr + i3ddmaCtrl[I3DDMA_DEV].cap_buffer[2].size;
		cap1_addr = IoReg_Read32(H3DDMA_CAP1_CTI_DMA_WR_Rule_check_low_reg);                 //NN start address
		cap1_up_limit = IoReg_Read32(H3DDMA_CAP2_CTI_DMA_WR_Rule_check_up_reg);
		cap3_addr = i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].phyaddr;		//dolby metadata start address
		cap3_up_limit = i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].phyaddr + i3ddmaCtrl[I3DDMA_DEV].cap_buffer[I3DDMA_MAIN_CAP_BUF_NUM].size * 2;
	}
	else //mode = 1
	{
		cap0_vbm_addr = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg);		//borrow from vbm
		cap0_vbm_up_limit = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_1_reg);
		cap0_mdm_addr = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_2_reg);		//borrow from vbm or m-domian
		cap0_mdm_up_limit = IoReg_Read32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_2_reg);
		cap1_addr = IoReg_Read32(H3DDMA_CAP1_CTI_DMA_WR_Rule_check_low_reg);				//NN start address
		cap1_up_limit = IoReg_Read32(H3DDMA_CAP2_CTI_DMA_WR_Rule_check_up_reg);
		cap3_addr = IoReg_Read32(H3DDMA_CAP3_WR_DMA_num_bl_wrap_addr_0_reg);			//dolby metadata start address
		cap3_up_limit = IoReg_Read32(H3DDMA_CAP3_CTI_DMA_WR_Rule_check_up_reg);
	}

	//rtd_pr_i3ddma_info("cap0_vbm_low=%x,cap0_mdm_low=%x,cap1_low=%x,cap3_low=%x\n", cap0_vbm_addr, cap0_mdm_addr, cap1_addr, cap3_addr);
	//rtd_pr_i3ddma_info("cap0_vbm_up=%x,cap0_mdm_up=%x,cap1_up=%x,cap3_up=%x\n", cap0_vbm_up_limit, cap0_mdm_up_limit, cap1_up_limit, cap3_up_limit);

	if(H3DDMA_CAP0_Cap_CTL0_get_cap0_triple_buf_en(IoReg_Read32(H3DDMA_CAP0_Cap_CTL0_reg)))
	{
		cap0_addr = MIN(cap0_vbm_addr, cap0_mdm_addr);
		cap0_up_limit = MAX(cap0_vbm_up_limit, cap0_mdm_up_limit);
	}
	else
	{
		cap0_addr = cap0_vbm_addr;
		cap0_up_limit = cap0_vbm_up_limit;
	}

	if(cap0_addr == 0)
		cap0_addr = I3DDMA_INVAL_ADDR;

	if(cap1_addr == 0)
		cap1_addr = I3DDMA_INVAL_ADDR;

	if(cap3_addr == 0)
		cap3_addr = I3DDMA_INVAL_ADDR;

	//start_addr = MIN(MIN(cap0_addr, cap1_addr), cap3_addr);
#if 0
	if((cap1_addr != 0) && (cap3_addr != 0))				//dolby + nn case
	{
		start_addr = MIN(MIN(cap0_addr, cap1_addr), cap3_addr);
	}
	else if((cap1_addr != 0) && (cap0_addr != 0))			//non-dolby + nn case
	{
		start_addr = MIN(cap0_addr, cap1_addr);
	}
	else if(cap1_addr != 0)			                        //non-dolby + nn case
	{
		start_addr = cap1_addr;
	}
	else if(cap3_addr != 0)									//nn disable + dolby case
	{
		start_addr = MIN(cap0_addr, cap3_addr);
	}
	else													//
	{
		start_addr = cap0_addr;
	}
#endif

	if(cap0_up_limit == I3DDMA_INVAL_ADDR)
		cap0_up_limit = 0;

	if(cap1_up_limit == I3DDMA_INVAL_ADDR)
		cap1_up_limit = 0;

	if(cap3_up_limit == I3DDMA_INVAL_ADDR)
		cap3_up_limit = 0;

	//cap_up_limit = MAX(MAX(cap0_up_limit, cap1_up_limit), cap3_up_limit);
#if 0
	if((cap1_addr != 0) && (cap3_addr != 0))
	{
		cap_up_limit = MAX(MAX(cap0_up_limit, cap1_up_limit), cap3_up_limit);
	}
	else if(cap1_addr != 0)
	{
		cap_up_limit = MAX(cap0_up_limit, cap1_up_limit);
	}
	else if(cap3_addr != 0)
	{
		cap_up_limit = MAX(cap0_up_limit, cap3_up_limit);
	}
	else
	{
		cap_up_limit = cap0_up_limit;
	}
#endif
	//mem_size = cap_up_limit - start_addr;

#if IS_ENABLED(CONFIG_RTK_KDRV_DC_MEMORY_TRASH_DETCTER)
	//h3ddma_dcmt_monitor(start_addr, mem_size, 8, mode);
#endif
}

unsigned int scaler_I3ddma_ISR(void)
{
	static unsigned char frame_cnt = 0;
	unsigned char mode_switching_bit = 0;
	//UINT8 handle_i3_cap1_last_wr_done = 0;
	I3DDMA_NN_BUFFER_ATTR_T* pi3ddma_nn_buffer_attr;

	h3ddma_cap1_cap_status_RBUS h3ddma_cap1_cap_status_reg;
	//h3ddma_interrupt_enable_RBUS h3ddma_interrupt_enable_reg;

	h3ddma_hsd_i3ddma1_uzd_ctrl0_RBUS i3ddma_uzd_ctrl0_reg;
	h3ddma_hsd_i3ddma1_uzd_v_db_ctrl_RBUS h3ddma_hsd_i3ddma_v_db_ctrl_reg;
	h3ddma_hsd_i3ddma1_uzd_h_db_ctrl_RBUS h3ddma_hsd_i3ddma_h_db_ctrl_reg;

	static unsigned int pre_update_time = 0;
	unsigned int update_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);
	I3DDMA_NN_BUFFER_ATTR_T* i3ddma_nn_buffer_attr_addr = (I3DDMA_NN_BUFFER_ATTR_T*)Scaler_GetShareMemVirAddr(SCALERIOC_I3_NN_SHARE_BUF_STATUS);

	//h3ddma_interrupt_enable_reg.regValue = IoReg_Read32(H3DDMA_Interrupt_Enable_reg);
	h3ddma_cap1_cap_status_reg.regValue = IoReg_Read32(H3DDMA_CAP1_Cap_Status_reg);

	//rtd_pr_i3ddma_info("I3ddma_ISR wr = %x\n", h3ddma_cap1_cap_status_reg.cap1_cap_last_wr_flag);

	//if(h3ddma_interrupt_enable_reg.cap1_last_wr_ie == 1)
	//	handle_i3_cap1_last_wr_done = 1;

	if((update_time - pre_update_time) > 60*90)
		rtd_pr_i3ddma_info("unexpected isr delay time %dms\n", (update_time - pre_update_time)/90);

	pre_update_time = update_time;

	//if((DMA_VGIP_DMA_VGIP_LC_get_dma_line_cnt(IoReg_Read32(DMA_VGIP_DMA_VGIP_LC_reg)) > 5) &&
	//	(DMA_VGIP_DMA_VGIP_LC_get_dma_line_cnt(IoReg_Read32(DMA_VGIP_DMA_VGIP_LC_reg)) < Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN)))
	//{
	//	rtd_pr_i3ddma_info("unexpected isr time %d\n", DMA_VGIP_DMA_VGIP_LC_get_dma_line_cnt(IoReg_Read32(DMA_VGIP_DMA_VGIP_LC_reg)));
	//	return 0;
	//}

	//if ((h3ddma_cap1_cap_status_reg.cap1_cap_last_wr_flag) /*&& (handle_i3_cap1_last_wr_done == 1)*/)
	{
		unsigned int ucBufAddr = 0;
		unsigned int ucSize = 0;
		int ptrlo = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);
		int ptrhi = IoReg_Read32(TIMER_SCPU_CLK90K_HI_reg);
		long long stamp = ptrlo | (((int64_t)ptrhi) << 32);

		if((g_tNN_Cap_Buffer[0].phyaddr == 0) || (i3ddma_nn_buffer_attr_addr == NULL))
		{
			IoReg_Write32(H3DDMA_CAP1_Cap_Status_reg, h3ddma_cap1_cap_status_reg.regValue);
			return 1;
		}

		i3ddma_uzd_ctrl0_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CTRL0_reg);

		pi3ddma_nn_buffer_attr = ((I3DDMA_NN_BUFFER_ATTR_T *) i3ddma_nn_buffer_attr_addr) + g_ucNNWriteIdx;
		pi3ddma_nn_buffer_attr->buf_status = I3DDMA_NN_BUFF_WRITE_DONE;
		pi3ddma_nn_buffer_attr->cap_mode = (i3ddma_uzd_ctrl0_reg.cutout_en == 1)?(I3DDMA_CROP_CAP):(I3DDMA_UZD_CAP);
		pi3ddma_nn_buffer_attr->y_buffer_addr = IoReg_Read32(H3DDMA_CAP1_WR_DMA_num_bl_wrap_addr_0_reg);
		pi3ddma_nn_buffer_attr->c_buffer_addr = IoReg_Read32(H3DDMA_CAP2_WR_DMA_num_bl_wrap_addr_0_reg);
		pi3ddma_nn_buffer_attr->timestamp = stamp;
		//rtd_pr_i3ddma_err("kernel cap_mode = %d, addr %x %d %llu\n", pi3ddma_nn_buffer_attr->cap_mode,pi3ddma_nn_buffer_attr->y_buffer_addr,g_ucNNWriteIdx,stamp);

		//update buffer address
		g_ucNNWriteIdx = (g_ucNNWriteIdx + 1) % NN_CAP_BUFFER_NUM;

		ucBufAddr = g_tNN_Cap_Buffer[g_ucNNWriteIdx].phyaddr;
		ucSize = g_tNN_Cap_Buffer[g_ucNNWriteIdx].size;
		IoReg_Write32( H3DDMA_CAP1_WR_DMA_num_bl_wrap_addr_0_reg , ucBufAddr);
		ucBufAddr = g_tNN_Cap_Buffer[0].phyaddr + ucSize * NN_CAP_BUFFER_NUM + (ucSize/2) * g_ucNNWriteIdx;
	    IoReg_Write32( H3DDMA_CAP2_WR_DMA_num_bl_wrap_addr_0_reg, ucBufAddr);

		//mark next buffer in using
		pi3ddma_nn_buffer_attr = ((I3DDMA_NN_BUFFER_ATTR_T *) i3ddma_nn_buffer_attr_addr) + g_ucNNWriteIdx;
		pi3ddma_nn_buffer_attr->buf_status = I3DDMA_NN_BUFF_IN_USE;


		//Generate bit switch
		/*
			1 means uzd mode
			0 means crop mode
			mode_switching_bit = 0 | 0 | 0 | 0 | 0 | 0 | 1 | 1
														 ^
												set change mode,next vsync apply
		*/
		if(g_enNNCapRatio == I3DDMA_CAP_1_1)
		{
			mode_switching_bit = _BIT0 | _BIT1;
		}
		else if(g_enNNCapRatio == I3DDMA_CAP_3_1)
		{
			mode_switching_bit = _BIT0 | _BIT1 | _BIT2;
		}
		else if(g_enNNCapRatio == I3DDMA_CAP_1_3)
		{
			mode_switching_bit = _BIT0;
		}
		else
		{
			mode_switching_bit = _BIT0 | _BIT1;
		}

		//switching mode
		if(g_enSupCapMode == I3DDMA_CAP_MIX)
		{
			//enable vsd double buffer
			h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg);
			h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg);
			if(DMA_VGIP_DMA_VGIP_LC_get_dma_line_cnt(IoReg_Read32(DMA_VGIP_DMA_VGIP_LC_reg)) <= 5)
			{
				//disable vsd double buffer
				h3ddma_hsd_i3ddma_v_db_ctrl_reg.v_db_en = 0;

				//disable vsd double buffer
				h3ddma_hsd_i3ddma_h_db_ctrl_reg.h_db_en = 0;
			}
			else
			{
				//enable vsd double buffer
				h3ddma_hsd_i3ddma_v_db_ctrl_reg.v_db_en = 1;

				//enable vsd double buffer
				h3ddma_hsd_i3ddma_h_db_ctrl_reg.h_db_en = 1;
			}
			IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg, h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue);
			IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg, h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue);

			i3ddma_uzd_ctrl0_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CTRL0_reg);

			frame_cnt++;
			//rtd_pr_i3ddma_info("bit = %x, cnt=%d\n", mode_switching_bit, frame_cnt);
			if(((mode_switching_bit & _BIT(frame_cnt)) == 0) && (i3ddma_uzd_ctrl0_reg.cutout_en == 0))
			{
				i3ddma_uzd_ctrl0_reg.h_zoom_en = 0;
				i3ddma_uzd_ctrl0_reg.v_zoom_en = 0;
				i3ddma_uzd_ctrl0_reg.cutout_en = 1;
				IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CTRL0_reg, i3ddma_uzd_ctrl0_reg.regValue);

				//rtd_pr_i3ddma_info("switch to crop mode\n");
			}

			//reset back uzd mode
			if((frame_cnt >= NN_CAP_BUFFER_NUM) && ((mode_switching_bit & _BIT(0)) == 1))
			{
				frame_cnt = 0;
				i3ddma_uzd_ctrl0_reg.h_zoom_en = 1;
				i3ddma_uzd_ctrl0_reg.v_zoom_en = 1;
				i3ddma_uzd_ctrl0_reg.cutout_en = 0;
				IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CTRL0_reg, i3ddma_uzd_ctrl0_reg.regValue);

				//rtd_pr_i3ddma_info("switch to uzd mode\n");
			}

			h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg);
			h3ddma_hsd_i3ddma_v_db_ctrl_reg.v_db_apply = 1;
			IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg, h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue);

			h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg);
			h3ddma_hsd_i3ddma_h_db_ctrl_reg.h_db_apply = 1;
			IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg, h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue);
		}

		IoReg_Write32(H3DDMA_CAP1_Cap_Status_reg, h3ddma_cap1_cap_status_reg.regValue);

		return 1;
	}

	return 0;
}


static I3DDMA_3D_T i3ddma3Dformat = I3DDMA_2D_ONLY;
static I3DDMA_3D_OPMODE_T i3ddmaOpMode = I3DDMA_3D_OPMODE_HW;

void Set_Val_I3ddma3Dformat(I3DDMA_3D_T i3ddma3DType)
{
    i3ddma3Dformat = i3ddma3DType;
}

I3DDMA_3D_T Get_Val_I3ddma3Dformat(void)
{
    return i3ddma3Dformat;
}

void Set_Val_I3ddma3DMode(I3DDMA_3D_OPMODE_T emode)
{
    i3ddmaOpMode = emode;
}

I3DDMA_3D_OPMODE_T Get_Val_I3ddma3DMode(void)
{
    return i3ddmaOpMode;
}
#endif // #ifndef BUILD_QUICK_SHOW

void IDDMA_Set_DMAWorkMode(unsigned char dev, unsigned char mode)
{
	iddma_work_mode[dev] = mode;
}

unsigned char IDDMA_Get_DMAWorkMode(unsigned char dev)
{
	return iddma_work_mode[dev];
}

