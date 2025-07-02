#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif
#include <rbus/dma_vgip_reg.h>
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //file not found
#include <rbus/h3ddma_rgb2yuv_dither_4xxto4xx_reg.h>
#endif // SCALER_ERROR
#include <rbus/h3ddma_reg.h>
#include <rbus/h3ddma_hsd_reg.h>
#include <rbus/scaleup_reg.h>
//#include <rbus/scaler/rbusPpOverlayReg.h>
#include <rbus/mdomain_cap_reg.h>
#include <rbus/mdomain_disp_reg.h>
//#include <rbus/scaler/rbusPpOverlayReg.h>
#include <rbus/ppoverlay_reg.h>


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
#include <uapi/linux/const.h>
#include <linux/mm.h>
#include <rtk_kdriver/RPCDriver.h>
#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <VideoRPC_System.h>
	#include <VideoRPC_System_data.h>
#else
	#include <rtk_kdriver/rpc/VideoRPC_System.h>
	#include <rtk_kdriver/rpc/VideoRPC_System_data.h>
#endif

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
#include <asm/io.h>
#endif

//#include <mach/video_driver.h>



#include <tvscalercontrol/i3ddma/i3ddma.h>
#include <tvscalercontrol/i3ddma/i3ddma_drv.h>
#include <tvscalercontrol/i3ddma/i3ddma_uzd.h>
#include <tvscalercontrol/scaler/modeState.h>
//#include <tvscalercontrol/scaler/scalerlib.h>
#include <tvscalercontrol/vip/ultrazoom.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <tvscalercontrol/adcsource/vga.h>
#include <tvscalercontrol/adcsource/adcctrl.h>
#include <scaler_vfedev.h>
//#ifdef DTV_SCART_OUT_ENABLE
//#include "tvscalercontrol/tve/tve.h"
#include <rbus/dma_vgip_reg.h>
#include <rbus/i4_vgip_reg.h>
#include <rbus/i5_vgip_reg.h>
#include <rbus/i6_vgip_reg.h>
#include <mach/platform.h>

#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerDrvCommon.h>
#else
#include <scalercommon/scalerDrvCommon.h>
#endif


#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
#else
#include <scalercommon/scalerCommon.h>
#endif

//#endif
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //file not found
#include <rbus/dc2h_dma_reg.h>
#endif // SCALER_ERROR
#include <tvscalercontrol/scalerdrv/scaler_i2rnd.h>
#include <tvscalercontrol/i3ddma/i3ddma_reg_array.h>
#include <vt_v4l2_export.h>


//#include <tvscalercontrol/hdmirx/hdmiInternal.h>
void drvif_I3DDMA_set_reorder_mode(unsigned char enable);

extern UINT8 get_nn_force_i3ddma_enable(void);

extern I3DDMA_3DDMA_CTRL_T i3ddmaCtrl[];
I3DDMA_TIMING_T       inputTiming[MAX_IDDMA_DEV];
I3DDMA_TIMING_T       genTiming[MAX_IDDMA_DEV];
extern IDDMA_DEV_ATTR_T iddma_attr[];

#define UNCAC_BASE		_AC(0xa0000000, UL)

#include <rtd_log/rtd_module_log.h>


void drvif_I3DDMA_Init(unsigned char dev)
{
	h3ddma_interrupt_enable_RBUS	h3ddma_interrupt_enable_reg;

	i3ddmaCtrl[dev].hw_dither12x10_enable = 0;
	i3ddmaCtrl[dev].hw_i3ddma_dma = I3DDMA_3D_OPMODE_HW;

// [HDMI-DMA] HDMI-DMA capture enable for 4k2k video timing
#ifdef ENABLE_HDMI_DMA_FOR_4K2K
	i3ddmaCtrl[dev].force_all_3d_disable = 0;
	i3ddmaCtrl[dev].force_2dto3d_enable = 1;
/* not defined
#elif ENABLE_DRIVER_I3DDMA
	i3ddmaCtrl[dev].force_all_3d_disable = 0;
	i3ddmaCtrl[dev].force_2dto3d_enable = 1;
*/
#else
	i3ddmaCtrl[dev].force_all_3d_disable = 1;
	i3ddmaCtrl[dev].force_2dto3d_enable = 0;
#endif

	i3ddmaCtrl[dev].force_3dto2d_enable = 0;
	i3ddmaCtrl[dev].force_2dto3d_mode = I3DDMA_2D_ONLY;
	i3ddmaCtrl[dev].i3ddma_3d_capability = 0;
	i3ddmaCtrl[dev].enable_3ddma = 1;

	memset((void*) &inputTiming[dev], 0, sizeof(I3DDMA_TIMING_T));
	memset((void*) &genTiming[dev], 0, sizeof(I3DDMA_TIMING_T));
	i3ddmaCtrl[dev].ptx_timing = &(inputTiming[dev]);
	i3ddmaCtrl[dev].pgen_timing = &(genTiming[dev]);

	//disable cap1 interrupt
	h3ddma_interrupt_enable_reg.regValue = IoReg_Read32(iddma_interrupt_enable_reg_addr[dev]);
	h3ddma_interrupt_enable_reg.cap1_last_wr_ie = 0;
	IoReg_Write32(iddma_interrupt_enable_reg_addr[dev],h3ddma_interrupt_enable_reg.regValue);

#ifndef BUILD_QUICK_SHOW
	iddma_attr[dev].forcebg_en = TRUE;
	iddma_attr[dev].cap_format = get_vt_pixel_format(dev);//default I3DDMA_COLOR_NV8BITS;
	if(iddma_attr[dev].cap_format == I3DDMA_COLOR_NV10BITS)
		iddma_attr[dev].cap_depth = I3DDMA_COLOR_DEPTH_10B;
	else if(iddma_attr[dev].cap_format == I3DDMA_COLOR_P010)
		iddma_attr[dev].cap_depth = I3DDMA_COLOR_DEPTH_16B;
	else
		iddma_attr[dev].cap_depth = I3DDMA_COLOR_DEPTH_8B;
	if((iddma_attr[dev].cap_format == I3DDMA_COLOR_YV12) ||(iddma_attr[dev].cap_format == I3DDMA_COLOR_I420))
		iddma_attr[dev].format_is_3planes = TRUE;
	else
		iddma_attr[dev].format_is_3planes = FALSE;
	IDDMA_Set_memoryType(dev, get_vt_memory_type(dev));
	if(get_vt_memory_type(dev) == IDDMA_MEMORY_MMAP)
		IDDMA_Set_memoryBufNum(dev, IDDMA_DEFAULT_BUFFER_NUM);
	else
		IDDMA_Set_memoryBufNum(dev, get_vt_dev_buffer_num(dev));
	iddma_attr[dev].total_buffer_size = 0;
	if(iddma_attr[dev].start_buffer_addr != 0)
	{
		IDDMA_DisableCap(dev);
		rtd_pr_ioreg_emerg("[IDDMA][%s] i%ddma memory not free\n",__FUNCTION__, (dev+3));
	}
	iddma_attr[dev].start_buffer_addr = 0;
	iddma_attr[dev].image_size = 0;
	iddma_attr[dev].source_type = get_vt_capture_source_type(dev);
	iddma_attr[dev].hdmi_port = get_hdmi_port_by_dev_num(dev);
	iddma_attr[dev].pitch = 0;
	iddma_attr[dev].input_region.srcx = 0;
	iddma_attr[dev].input_region.srcy = 0;
	iddma_attr[dev].input_region.src_wid = 0;
	iddma_attr[dev].input_region.src_height = 0;
	iddma_attr[dev].output_region.srcx = 0;
	iddma_attr[dev].output_region.srcy = 0;
	iddma_attr[dev].output_region.src_wid = IDDMA_DEFAULT_OUTPUT_WIDTH;
	iddma_attr[dev].output_region.src_height = IDDMA_DEFAULT_OUTPUT_HEIGHT;
	iddma_attr[dev].read_point = IDDMA_MAX_BUFFER_NUM;
	iddma_attr[dev].write_point = IDDMA_MAX_BUFFER_NUM;

	spin_lock_init(&iddma_attr[dev].iddma_spinlock);
#endif

	return;
}


extern void rtk_i3ddma_disable(void);
void drvif_I3DDMA_DeInit(void)
{
	rtk_i3ddma_disable();
}



void drvif_I3DDMA_Set_Input_Info_Vaule(unsigned char dev, I3DDMA_INFO_LIST input_type, unsigned long value)
{
	switch(input_type)
	{
		case I3DDMA_INFO_HW_I3DDMA_DMA:
			i3ddmaCtrl[dev].hw_i3ddma_dma = (I3DDMA_3D_OPMODE_T) value;
		break;

		case I3DDMA_INFO_HW_DITHER12X10_ENABLE:
			i3ddmaCtrl[dev].hw_dither12x10_enable= value;
		break;

		case I3DDMA_INFO_HW_DITHER10x8_ENABLE:
			i3ddmaCtrl[dev].hw_dither10x8_enable= value;
		break;

		case I3DDMA_INFO_HW_4XXTO4XX_ENABLE:
			i3ddmaCtrl[dev].hw_4xxto4xx_enable= value;
		break;

		case I3DDMA_INFO_HW_RGBTOYUV_ENABLE:
			i3ddmaCtrl[dev].hw_rgbtoyuv_enable= value;
		break;

		case I3DDMA_INFO_INPUT_TIMING:
		#ifdef CONFIG_ARM64
			memcpy_toio((void*) i3ddmaCtrl[dev].ptx_timing, (void*) value, sizeof(I3DDMA_TIMING_T));
		#else
			memcpy((void*) i3ddmaCtrl[dev].ptx_timing, (void*) value, sizeof(I3DDMA_TIMING_T));
		#endif
		break;

		case I3DDMA_INFO_GEN_TIMING:
		#ifdef CONFIG_ARM64
			memcpy_toio((void*) i3ddmaCtrl[dev].pgen_timing, (void*) value, sizeof(I3DDMA_TIMING_T));
		#else
			memcpy((void*) i3ddmaCtrl[dev].pgen_timing, (void*) value, sizeof(I3DDMA_TIMING_T));
		#endif
		break;

		case I3DDMA_INFO_FRAMERATE_X2:
			i3ddmaCtrl[dev].frame_rate_x2= value;
		break;

		case I3DDMA_INFO_HW_HSD_ENABLE:
			i3ddmaCtrl[dev].hw_hsd_enable = value;
		break;

		case I3DDMA_INFO_INPUT_SRC_IDX:
			i3ddmaCtrl[dev].input_src_idx= value;
		break;

		case I3DDMA_INFO_INPUT_SRC_TYPE:
			i3ddmaCtrl[dev].input_src_type= value;
		break;

		case I3DDMA_INFO_INPUT_SRC_FROM:
			i3ddmaCtrl[dev].input_src_from= value;
		break;

		case I3DDMA_INFO_INPUT_MODE_CURR:
			i3ddmaCtrl[dev].input_src_mode= value;
		break;

		case I3DDMA_INFO_INPUT_H_COUNT:
			i3ddmaCtrl[dev].input_h_Count= value;
		break;

		case I3DDMA_INFO_INPUT_H_SIZE:
			i3ddmaCtrl[dev].input_h_size= value;
		break;

		case I3DDMA_INFO_OUTPUT_H_SIZE:
			i3ddmaCtrl[dev].output_h_size= value;
		break;

		case I3DDMA_INFO_TARGET_DEPTH:
			i3ddmaCtrl[dev].targetDepth= (I3DDMA_COLOR_DEPTH_T) value;
		break;

		case I3DDMA_INFO_TARGET_COLOR:
			i3ddmaCtrl[dev].targetColor= (I3DDMA_COLOR_SPACE_T) value;
		break;


		default:

			break;

	};

}


unsigned long drvif_I3DDMA_Get_Input_Info_Vaule(unsigned char dev, I3DDMA_INFO_LIST input_type)
{
	switch(input_type)
	{
		case I3DDMA_INFO_HW_I3DDMA_DMA:
			return (UINT32) i3ddmaCtrl[dev].hw_i3ddma_dma;
		break;

		case I3DDMA_INFO_HW_DITHER12X10_ENABLE:
			return i3ddmaCtrl[dev].hw_dither12x10_enable;
		break;

		case I3DDMA_INFO_HW_DITHER10x8_ENABLE:
			return i3ddmaCtrl[dev].hw_dither10x8_enable;
		break;

		case I3DDMA_INFO_HW_4XXTO4XX_ENABLE:
			return i3ddmaCtrl[dev].hw_4xxto4xx_enable;
		break;

		case I3DDMA_INFO_HW_RGBTOYUV_ENABLE:
			return i3ddmaCtrl[dev].hw_rgbtoyuv_enable;
		break;

		case I3DDMA_INFO_INPUT_TIMING:
			return (unsigned long) i3ddmaCtrl[dev].ptx_timing;
		break;

		case I3DDMA_INFO_GEN_TIMING:
			return (unsigned long) i3ddmaCtrl[dev].pgen_timing;
		break;

		case I3DDMA_INFO_FRAMERATE_X2:
			return i3ddmaCtrl[dev].frame_rate_x2;
		break;

		case I3DDMA_INFO_ENABLE_3DDMA:
			return i3ddmaCtrl[dev].enable_3ddma;
		break;

		case I3DDMA_INFO_HW_HSD_ENABLE:
			return i3ddmaCtrl[dev].hw_hsd_enable;
		break;

		case I3DDMA_INFO_INPUT_SRC_IDX:
			return i3ddmaCtrl[dev].input_src_idx;
		break;

		case I3DDMA_INFO_INPUT_SRC_TYPE:
			return i3ddmaCtrl[dev].input_src_type;
		break;

		case I3DDMA_INFO_INPUT_SRC_FROM:
			return i3ddmaCtrl[dev].input_src_from;
		break;

		case I3DDMA_INFO_INPUT_MODE_CURR:
			return i3ddmaCtrl[dev].input_src_mode;
		break;

		case I3DDMA_INFO_INPUT_H_COUNT:
			return i3ddmaCtrl[dev].input_h_Count;
		break;

		case I3DDMA_INFO_INPUT_H_SIZE:
			return i3ddmaCtrl[dev].input_h_size;
		break;

		case I3DDMA_INFO_OUTPUT_H_SIZE:
			return i3ddmaCtrl[dev].output_h_size;
		break;

		case I3DDMA_INFO_TARGET_DEPTH:
			return i3ddmaCtrl[dev].targetDepth;
		break;

		case I3DDMA_INFO_TARGET_COLOR:
			return i3ddmaCtrl[dev].targetColor;
		break;

		default:
			break;

	};

	return 0;
}


extern unsigned char Get_Factory_SelfDiagnosis_Mode(void);
void drv_I3DDMA_ApplySetting(unsigned char dev)
{
	SIZE insize,outsize;

#if 0
	if(i3ddmaCtrl.hw_hsd_enable == 1){
		I3DDMA_ultrazoom_config_h_scaling_down(i3ddmaCtrl.input_h_size, i3ddmaCtrl.output_h_size, 0);
		i3ddmaCtrl.ptx_timing->h_act_len = i3ddmaCtrl.output_h_size;
	}
#endif
	if((i3ddmaCtrl[dev].ptx_timing == NULL) || (i3ddmaCtrl[dev].pgen_timing == NULL))
	{
		rtd_pr_ioreg_emerg("[i3ddma][%s]timing is null,return\n",__FUNCTION__);
		return;
	}

	I3DDMA_Get3DGenTiming(dev, i3ddmaCtrl[dev].ptx_timing, i3ddmaCtrl[dev].pgen_timing);

	//HDMI 4096x2160 case uzd to 3840x2160 @Crixus 20170620
	if(IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE)
	{
		if((i3ddmaCtrl[dev].ptx_timing->h_act_len > 5120) /*&& (get_HDMI_HDR_mode() == HDR_HDR10_HDMI)*/)
		{
			insize.nLength = i3ddmaCtrl[dev].ptx_timing->v_act_len;
			insize.nWidth = i3ddmaCtrl[dev].ptx_timing->h_act_len;
			i3ddmaCtrl[dev].pgen_timing->h_act_len = 5120;
			outsize.nWidth = 5120;
			I3DDMA_ultrazoom_config_scaling_down(&insize, &outsize, 1);

		}
		else if((i3ddmaCtrl[I3DDMA_DEV].ptx_timing->h_act_len ==1440) && (DMA_VGIP_DMA_VGIP_CTRL_get_dma_in_sel(IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg)) == VGIP_SRC_ATV))
		{

			rtd_pr_i3ddma_debug("av source,1440->960\n");
			insize.nLength = i3ddmaCtrl[I3DDMA_DEV].ptx_timing->v_act_len;
			insize.nWidth = i3ddmaCtrl[I3DDMA_DEV].ptx_timing->h_act_len;
			outsize.nLength = i3ddmaCtrl[I3DDMA_DEV].ptx_timing->v_act_len;

#ifdef CONFIG_ENABLE_VD_27MHZ_MODE
			//self-dianosis AV source force to 704, rzhen@2016-09-01
			if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_AVD)
				&& Get_Factory_SelfDiagnosis_Mode()){
				outsize.nWidth = 704;
				i3ddmaCtrl[I3DDMA_DEV].pgen_timing->h_act_len = 704;
			}
			else
#endif
			{
				outsize.nWidth = 960;
				i3ddmaCtrl[I3DDMA_DEV].pgen_timing->h_act_len = 960;
			}

			I3DDMA_ultrazoom_config_scaling_down(&insize, &outsize, 1);
		}
		else if((i3ddmaCtrl[I3DDMA_DEV].ptx_timing->h_act_len > 1920) && (i3ddmaCtrl[I3DDMA_DEV].ptx_timing->v_act_len > 240)/*2880x576i case*/
			&& (i3ddmaCtrl[I3DDMA_DEV].ptx_timing->progressive == 0)/*only interlace*/
			&& (DMA_VGIP_DMA_VGIP_CTRL_get_dma_in_sel(IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg)) == get_vgip_in_sel_by_display(SLR_MAIN_DISPLAY)))
		{
			// 2880x576i case need to scale down to 1920x576i @Crixus 20160725
			//rtd_pr_i3ddma_info("HDMI 2880x576i => 1920x576i\n");
			insize.nLength = i3ddmaCtrl[I3DDMA_DEV].ptx_timing->v_act_len;
			insize.nWidth = i3ddmaCtrl[I3DDMA_DEV].ptx_timing->h_act_len;
			outsize.nLength = i3ddmaCtrl[I3DDMA_DEV].ptx_timing->v_act_len;
			outsize.nWidth = 1920;
			i3ddmaCtrl[I3DDMA_DEV].pgen_timing->h_act_len = 1920;

			I3DDMA_ultrazoom_config_scaling_down(&insize, &outsize, 1);
		}
	}

	if(IDDMA_Get_DMAWorkMode(dev) == IDDMA_NN_MODE)
	{
		//reset uzd and crop setting
		h3ddma_hsd_i3ddma1_uzd_ctrl0_RBUS i3ddma_uzd_ctrl0_reg;
		h3ddma_hsd_i3ddma1_uzd_cutout_range_hor_RBUS h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg;
		h3ddma_hsd_i3ddma1_uzd_cutout_range_ver_RBUS h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg;
		h3ddma_hsd_i3ddma1_uzd_cutout_dis_num_hor_RBUS h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg;
		h3ddma_hsd_i3ddma1_uzd_cutout_dis_num_ver_RBUS h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg;
		h3ddma_hsd_i3ddma1_uzd_v_db_ctrl_RBUS h3ddma_hsd_i3ddma_v_db_ctrl_reg;
		h3ddma_hsd_i3ddma0_uzd_h_db_ctrl_RBUS h3ddma_hsd_i3ddma_h_db_ctrl_reg;

		//run scaler clear double buffer
		h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg);
		h3ddma_hsd_i3ddma_v_db_ctrl_reg.v_db_en = 0;
		IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg, h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue);
		h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg);
		h3ddma_hsd_i3ddma_h_db_ctrl_reg.h_db_en = 0;
		IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg, h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue);

		i3ddma_uzd_ctrl0_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CTRL0_reg);
		i3ddma_uzd_ctrl0_reg.h_zoom_en = 0;

		//reset v-scale down buffer mode @Crixus 20160804
		i3ddma_uzd_ctrl0_reg.v_zoom_en = 0;
		//i3ddma_uzd_ctrl0_reg.buffer_mode = 0;
		i3ddma_uzd_ctrl0_reg.cutout_en = 0;
		IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CTRL0_reg, i3ddma_uzd_ctrl0_reg.regValue);

		h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_RANGE_HOR_reg);
		h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg.hor_front = 0;
		h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg.hor_width = 0;
		IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_RANGE_HOR_reg, h3ddma_hsd_i3ddma_sdnr_cutout_range_hor_reg.regValue);

		h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_RANGE_VER_reg);
		h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg.ver_front = 0;
		h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg.ver_height = 0;
		IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_RANGE_VER_reg, h3ddma_hsd_i3ddma_sdnr_cutout_range_ver_reg.regValue);

		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_HOR_reg);
		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg.hor_num = 0;
		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg.hor_dis = 0;
		IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_HOR_reg, h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_hor_reg.regValue);

		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_VER_reg);
		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.ver_num = 0;
		h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.ver_dis = 0;
		IoReg_Write32(H3DDMA_HSD_I3DDMA1_UZD_CUTOUT_DIS_NUM_VER_reg, h3ddma_hsd_i3ddma_sdnr_cutout_dis_num_ver_reg.regValue);

	}

	I3DDMA_BandWidth_Control(dev, i3ddmaCtrl[dev].ptx_timing, i3ddmaCtrl[dev].pgen_timing);

	if((dev == I3DDMA_DEV) && (IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE))
	{
		I3DDMA_Dithering12to10(i3ddmaCtrl[dev].ptx_timing->depth, i3ddmaCtrl[dev].pgen_timing->depth);
		I3DDMA_4xxto4xx_Downsample(i3ddmaCtrl[I3DDMA_DEV].ptx_timing->color, i3ddmaCtrl[I3DDMA_DEV].pgen_timing->color);
	}
	else
	{
		I3DDMA_Dithering12to10_for_cap(dev, i3ddmaCtrl[dev].ptx_timing->depth, I3DDMA_COLOR_DEPTH_8B);
		I3DDMA_4xxto4xx_Downsample_for_cap(dev, i3ddmaCtrl[dev].ptx_timing->color, I3DDMA_COLOR_YUV444);
	}

	switch (i3ddmaCtrl[dev].hw_i3ddma_dma) {

		case I3DDMA_3D_OPMODE_HW_2DONLY_L:
			  I3DDMA_Setup3DDMA(&i3ddmaCtrl[dev], I3DDMA_3D_OPMODE_HW_2DONLY_L,dev);
			  I3DDMA_ForceVODMA2D(1, 1);
		break;
		case I3DDMA_3D_OPMODE_HW_2DONLY_R:
			  I3DDMA_Setup3DDMA(&i3ddmaCtrl[dev], I3DDMA_3D_OPMODE_HW_2DONLY_R,dev);
			  I3DDMA_ForceVODMA2D(1, 0);
		break;
		case I3DDMA_3D_OPMODE_HW:
			  I3DDMA_Setup3DDMA(&i3ddmaCtrl[dev], I3DDMA_3D_OPMODE_HW,dev);
			  I3DDMA_ForceVODMA2D(0, 0);
		break;

		case I3DDMA_3D_OPMODE_DISABLE:
			I3DDMA_Setup3DDMA(&i3ddmaCtrl[dev], I3DDMA_3D_OPMODE_DISABLE,dev);
		break;
		case I3DDMA_3D_OPMODE_HW_NODMA:
			I3DDMA_Setup3DDMA(&i3ddmaCtrl[dev], I3DDMA_3D_OPMODE_HW_NODMA,dev);
		break;
        case I3DDMA_3D_OPMODE_SW:
            I3DDMA_Setup3DDMA(&i3ddmaCtrl[dev], I3DDMA_3D_OPMODE_SW,dev);
            I3DDMA_ForceVODMA2D(0, 0);
        break;
		default:
				rtd_pr_i3ddma_debug("unknow HDMI 3D DMA mode\n");
		break;

	}

	if(IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE)
	{
		if(dvrif_i3ddma_compression_get_enable()){
			//Re-order control for HDMI Dolby @Crixus 20180322
			drvif_I3DDMA_set_reorder_mode(get_i3ddma_dolby_reorder_mode());
			dvrif_i3ddma_comp_setting(TRUE, i3ddmaCtrl[I3DDMA_DEV].pgen_timing->h_act_len, i3ddmaCtrl[I3DDMA_DEV].pgen_timing->v_act_len, dvrif_i3ddma_get_compression_bits());
		}
		else{
			//Re-order control for HDMI Dolby @Crixus 20180322
			drvif_I3DDMA_set_reorder_mode(_DISABLE);
			dvrif_i3ddma_comp_setting(FALSE, i3ddmaCtrl[I3DDMA_DEV].pgen_timing->h_act_len, i3ddmaCtrl[I3DDMA_DEV].pgen_timing->v_act_len, 0);
		}
	}
}


void drvif_I3DDMA_Enable_Vgip(void)
{
	//dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;
	dma_vgip_dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_ivrun = 1;
	dma_vgip_chn1_ctrl_reg.dma_in_clk_en = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

	return;
}



void drvif_I3DDMA_Disable_Vgip(void)
{
	//dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;
	dma_vgip_dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_ivrun = 0;
	dma_vgip_chn1_ctrl_reg.dma_in_clk_en = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

	return;
}

void drvif_I3DDMA_Reset_Vgip_Source_Sel(unsigned char dev)
{
	dma_vgip_dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(vgip_ctrl_reg_addr[dev]);
	dma_vgip_chn1_ctrl_reg.dma_in_sel = VGIP_SRC_VODMA;
	IoReg_Write32(vgip_ctrl_reg_addr[dev], dma_vgip_chn1_ctrl_reg.regValue);

	return;
}

void drvif_I3DDMA_freeze_enable(unsigned char enable)
{
	h3ddma_cap0_cap_ctl0_RBUS cap_ctl0_reg;

	cap_ctl0_reg.regValue = IoReg_Read32(H3DDMA_CAP0_Cap_CTL0_reg);
	if(enable)
		cap_ctl0_reg.cap0_freeze_en = 1;
	else
		cap_ctl0_reg.cap0_freeze_en = 0;
	IoReg_Write32(H3DDMA_CAP0_Cap_CTL0_reg, cap_ctl0_reg.regValue);

	return;
}

void drvif_I3DDMA_Disable_uzd(void)
{
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	h3ddma_hsd_i3ddma_v_db_ctrl_RBUS h3ddma_hsd_i3ddma_v_db_ctrl_reg;
	h3ddma_hsd_i3ddma_h_db_ctrl_RBUS h3ddma_hsd_i3ddma_h_db_ctrl_reg;
	h3ddma_hsd_i3ddma_hsd_ctrl0_RBUS i3ddma_hsd_ctrl0_reg;
	h3ddma_hsd_i3ddma_vsd_ctrl0_RBUS i3ddma_vsd_ctrl0_reg;

	//run scaler clear double buffer
	h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA_V_DB_CTRL_reg);
	h3ddma_hsd_i3ddma_v_db_ctrl_reg.v_db_en = 0;
	IoReg_Write32(H3DDMA_HSD_I3DDMA_V_DB_CTRL_reg, h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue);
	h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA_H_DB_CTRL_reg);
	h3ddma_hsd_i3ddma_h_db_ctrl_reg.h_db_en = 0;
	IoReg_Write32(H3DDMA_HSD_I3DDMA_H_DB_CTRL_reg, h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue);

	i3ddma_hsd_ctrl0_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA_HSD_Ctrl0_reg);
	i3ddma_hsd_ctrl0_reg.h_zoom_en = 0;
	IoReg_Write32(H3DDMA_HSD_I3DDMA_HSD_Ctrl0_reg, i3ddma_hsd_ctrl0_reg.regValue);

	//reset v-scale down buffer mode
	i3ddma_vsd_ctrl0_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA_VSD_Ctrl0_reg);
	i3ddma_vsd_ctrl0_reg.v_zoom_en = 0;
	i3ddma_vsd_ctrl0_reg.cutout_en = 0;
	i3ddma_vsd_ctrl0_reg.buffer_mode = 0;
	IoReg_Write32(H3DDMA_HSD_I3DDMA_VSD_Ctrl0_reg, i3ddma_vsd_ctrl0_reg.regValue);

	return;
#endif // SCALER_ERROR
}


//extern "C"
//{
extern unsigned int Get_DISP_DEN_STA_HPOS(void);
extern unsigned int Get_DISP_DEN_STA_VPOS(void);
//}
static void I3DDMA_Set_Vgip(unsigned char src, DISPD_CAP_SRC dispD_sel, unsigned char mode)
{
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	//dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;
	dma_vgip_dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;
	//dma_vgip_act_hsta_width_RBUS    dma_vgip_chn1_act_hsta_width_reg;
	dma_vgip_dma_vgip_act_hsta_width_RBUS dma_vgip_chn1_act_hsta_width_reg;
	//dma_vgip_act_vsta_length_RBUS   dma_vgip_chn1_act_vsta_length_reg;
	dma_vgip_dma_vgip_act_vsta_length_RBUS dma_vgip_chn1_act_vsta_length_reg;
	//dma_vgip_delay_RBUS dma_vgip_chn1_delay_reg;
	dma_vgip_dma_vgip_delay_RBUS  dma_vgip_chn1_delay_reg;
	//dma_vgip_cts_fifo_ctl_RBUS 			 	 dma_cts_fifo_ctl_reg;
	dma_vgip_dma_vgip_cts_fifo_ctl_RBUS  dma_cts_fifo_ctl_reg;
	//int dispD_hstaOffset=0;

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_in_clk_en = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_in_sel = src;
	dma_vgip_chn1_ctrl_reg.dma_digital_mode = mode;

	if(dispD_sel == SCART_OUT_DTV)
		dma_vgip_chn1_ctrl_reg.dma_field_det_en = 0;
	else
		dma_vgip_chn1_ctrl_reg.dma_field_det_en = 1;

	dma_vgip_chn1_ctrl_reg.dma_field_inv = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_in_clk_en = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);


	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_ivrun = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);


	dma_vgip_chn1_act_hsta_width_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_ACT_HSTA_Width_reg);
	dma_vgip_chn1_act_vsta_length_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_ACT_VSTA_Length_reg);

#if 0
	// [FIX-ME] unknown DispD horizontal offset
	switch(dispD_sel){
	  case CAP_SRC_SUBTITLE:
		dispD_hstaOffset = -2;
		break;
	  case CAP_SRC_OSD:
		dispD_hstaOffset = 11;
		break;
	  default:
		break;
	}
#endif

	#ifdef HDMI_4K2K_I3DDMA
	if(Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) == HDMI_4K2K_TIMING_WIDTH && Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE) == HDMI_4K2K_TIMING_LENGTH)//for 4k2k
	{
		dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_STA_PRE);
		dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
		dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_sta = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_STA_PRE);
		dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_len= Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);
	}
	#endif

	//change this item
	if(dispD_sel == SCART_OUT_DTV){
		dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_STA_PRE);
		dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
		dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_sta = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_STA_PRE);
		if((Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE) < 288 ) && Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_THRIP))//SE do scaling up to 720x288 interlace
			dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_len= 288;
		else if((Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE) < 576)  && !Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_THRIP))//SE do scaling up to 720x576 prograssive
			dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_len= 576;
		else
			dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_len= Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);
		if(Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) < 720)
			dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = 720;
		else
			dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
	}
	else{
		if(dispD_sel == CAP_SRC_SUBTITLE || dispD_sel == CAP_SRC_OSD ||dispD_sel == CAP_SRC_DITHER)
		{
			rtd_pr_i3ddma_debug("[dsip2tve]CAP_SRC_SUBTITLE||CAP_SRC_OSD||CAP_SRC_DITHER..._DISP_WID=%x,_DISP_LEN=%x\n",_DISP_WID,_DISP_LEN);
			dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta = Get_DISP_DEN_STA_HPOS();// + dispD_hstaOffset;
			dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = Get_DISP_ACT_END_HPOS();
			dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_sta = Get_DISP_DEN_STA_VPOS();
			dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_len= Get_DISP_ACT_END_VPOS();
		}
		else if(dispD_sel == CAP_SRC_UZU)
		{
			dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta = Get_DISP_DEN_STA_HPOS();
			dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = Scaler_DispGetInputInfo(SLR_INPUT_DISP_WID);
			dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_sta = Get_DISP_DEN_STA_VPOS();
			dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_len= Scaler_DispGetInputInfo(SLR_INPUT_DISP_LEN);
		}
	}

	IoReg_Write32(DMA_VGIP_DMA_VGIP_ACT_HSTA_Width_reg, dma_vgip_chn1_act_hsta_width_reg.regValue);
	IoReg_Write32(DMA_VGIP_DMA_VGIP_ACT_VSTA_Length_reg, dma_vgip_chn1_act_vsta_length_reg.regValue);

	// IHS/IVS delay
	dma_vgip_chn1_delay_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_DELAY_reg);
	dma_vgip_chn1_delay_reg.dma_ihs_dly = 0;
	dma_vgip_chn1_delay_reg.dma_ivs_dly = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_DELAY_reg, dma_vgip_chn1_delay_reg.regValue);

	dma_cts_fifo_ctl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTS_FIFO_CTL_reg);
	dma_cts_fifo_ctl_reg.dma_en_ctsfifo_vsrst_vgip = 1;
	dma_cts_fifo_ctl_reg.dma_force_ctsfifo_rstn_vgip = 1;
	dma_cts_fifo_ctl_reg.dma_force_ctsfifo_rstn_sdnr = 1;
	dma_cts_fifo_ctl_reg.dma_en_ctsfifo_vsrst_sdnr = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTS_FIFO_CTL_reg, dma_cts_fifo_ctl_reg.regValue);

	return;
#endif // SCALER_ERROR
}



#if 1

unsigned char drvif_I3DDMA_StartCaptureRpc(int width /* = 720 */, int height /* = 480*/, DISPD_CAP_SRC mode /* = SUB_MIXER */)
{
#ifndef BUILD_QUICK_SHOW
#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)
	unsigned long ret;
#endif

	VIDEO_RPC_CAP_START_CAPTURE *start;
//		unsigned long ret;
	unsigned long  vir_addr, vir_addr_noncache;
	unsigned int  phy_addr;
	vir_addr = (unsigned long)dvr_malloc_uncached_specific(sizeof(VIDEO_RPC_CAP_START_CAPTURE), GFP_DCU1_LIMIT, (void*)&vir_addr_noncache);
	//phy_addr = (unsigned int)virt_to_phys((void*)vir_addr);
	phy_addr = (unsigned int)dvr_to_phys((void*)vir_addr);


	start = (VIDEO_RPC_CAP_START_CAPTURE *)vir_addr_noncache;
	start->instanceID = 0;
	start->instanceID = htonl(start->instanceID);
	start->startMode = mode;
	start->startMode = htonl(start->startMode);
	start->cap_width = width;
	start->cap_width = htonl(start->cap_width);
	start->cap_height = height;
	start->cap_height = htonl(start->cap_height);
#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)

	if (send_rpc_command(RPC_VIDEO, VIDEO_RPC_CAP_ToAgent_StartCapture, phy_addr, 0, &ret))
	{
		rtd_pr_i3ddma_debug("RPC fail!!\n");
		dvr_free((void*)vir_addr);
		return FALSE;
	}
#endif
	dvr_free((void*)vir_addr);
#endif
	return TRUE;
}

unsigned char drvif_I3DDMA_StopCaptureRpc(void)
{
#ifndef BUILD_QUICK_SHOW
#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)
	unsigned long ret;
#endif

	VIDEO_RPC_CAP_STOP_CAPTURE *stop;
//		unsigned long ret;
	unsigned long  vir_addr, vir_addr_noncache;
	unsigned int  phy_addr;
	vir_addr = (unsigned long)dvr_malloc_uncached_specific(sizeof(VIDEO_RPC_CAP_STOP_CAPTURE), GFP_DCU1_LIMIT, (void*)&vir_addr_noncache);
	//phy_addr = (unsigned int)virt_to_phys((void*)vir_addr);
	phy_addr = (unsigned int)dvr_to_phys((void*)vir_addr);

	stop = (VIDEO_RPC_CAP_STOP_CAPTURE *)vir_addr_noncache;
	stop->instanceID = 0;
	stop->instanceID = htonl(stop->instanceID);
#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)

	if (send_rpc_command(RPC_VIDEO, VIDEO_RPC_CAP_ToAgent_StopCapture, phy_addr, 0, &ret))
	{
		rtd_pr_i3ddma_debug("RPC fail!!\n");
		dvr_free((void*)vir_addr);
		return 1;
	}
#endif
	dvr_free((void*)vir_addr);
#endif
    return TRUE;
}

#endif

void  drvif_I3DDMA_set_capture_for2D(SIZE *outsize)
{
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	//unsigned int num;
	//unsigned int len;
	//unsigned int rem;
	//unsigned int lineunits;

	 //lr_separate_ctrl1_RBUS lr_separate_ctrl1;
	 h3ddma_lr_separate_ctrl1_RBUS lr_separate_ctrl1;
	 //lr_separate_ctrl2_RBUS lr_separate_ctrl2;
	 h3ddma_lr_separate_ctrl2_RBUS lr_separate_ctrl2;
	 //cap_ctl1_RBUS cap_ctl1;
	// h3ddma_cap_ctl1_RBUS cap_ctl1;
	 //cap_ctl0_RBUS cap_ctl0;
	 h3ddma_cap0_cap_ctl0_RBUS cap_ctl0;
	 //tg_v1_end_RBUS tg_v1_end;
	 h3ddma_tg_v1_end_RBUS tg_v1_end;
	 //i3ddma_enable_RBUS i3ddma_enable;
	 h3ddma_i3ddma_enable_RBUS i3ddma_enable;
//	 i3ddma_i3ddma_byte_channel_swap_RBUS  i3ddma_byte_channel_swap;
	 h3ddma_cap0_wr_dma_pxltobus_RBUS h3ddma_cap0_wr_dma_pxltobus_reg;


	I3DDMA_disp2tve_CapBufInit();

	// dispi hdmi_3ddma
	 lr_separate_ctrl1.regValue = IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg);
	 lr_separate_ctrl1.hdmi_3d_structure = 0x0a;
 	 lr_separate_ctrl1.progressive = 1;
	// lr_separate_ctrl1.hdmi_3d_sel = 0;
 	 IoReg_Write32(H3DDMA_LR_Separate_CTRL1_reg,lr_separate_ctrl1.regValue);

	lr_separate_ctrl2.regValue = IoReg_Read32(H3DDMA_LR_Separate_CTRL2_reg);
	lr_separate_ctrl2.hact = outsize->nWidth;
	lr_separate_ctrl2.vact = outsize->nLength;
       IoReg_Write32(H3DDMA_LR_Separate_CTRL2_reg,lr_separate_ctrl2.regValue);

	//Capture
	//check this item
	rtd_pr_i3ddma_debug("[scart out]I3DDMA: 422cap = %d, 8bit=%d\n",Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_422CAP), !Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_10BIT));
#if 0
    if(Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_422CAP)) //422
		lineunits = (outsize->nWidth * 8 * 2) /128;
	else   //444
		lineunits = (outsize->nWidth * 8 * 3) /128;
#endif
	//len = 32;
	//num = (unsigned int)(lineunits / len);
	//rem = (unsigned int)(lineunits % len);
#if 0
	cap_ctl1.regValue = IoReg_Read32(H3DDMA_Cap_CTL1_reg);
	cap_ctl1.cap_write_num = num;
	cap_ctl1.cap_water_lv = len;
	IoReg_Write32(H3DDMA_Cap_CTL1_reg,cap_ctl1.regValue);
#endif
	cap_ctl0.regValue = IoReg_Read32(H3DDMA_CAP0_Cap_CTL0_reg);
	h3ddma_cap0_wr_dma_pxltobus_reg.regValue=IoReg_Read32(H3DDMA_CAP0_WR_DMA_pxltobus_reg);
//	cap_ctl0.cap_write_rem = rem;
//	cap_ctl0.cap_write_len = len;
	cap_ctl0.cap0_auto_block_sel_en = 1;
//	cap_ctl0.vs_fall_rst_en = 1;
	cap_ctl0.cap0_quad_buf_en = 0;

	#if 0
	if (scaler_dispDtoI3ddma_get_enable() == TRUE) {
		cap_ctl0.triple_buf_en= 1;//tve use 3-buffer //scart out use 3-buffer
	} else {
	if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_THRIP))
		cap_ctl0.quad_buf_en= 1;//interlaced use 4-buffer to jump even and odd field
	else
		cap_ctl0.triple_buf_en= 1;//tve use 3-buffer //scart out use 3-buffer
	}
	#endif
	cap_ctl0.cap0_quad_buf_en= 1;
	if(Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_422CAP)) //422
 	{
 		h3ddma_cap0_wr_dma_pxltobus_reg.cap0_pixel_encoding = 1;
		h3ddma_cap0_wr_dma_pxltobus_reg.cap0_bit_sel = 0;

	 }
	 else   //444
 	{
		 h3ddma_cap0_wr_dma_pxltobus_reg.cap0_bit_sel=0;
		 h3ddma_cap0_wr_dma_pxltobus_reg.cap0_pixel_encoding=0;

 	}

 	IoReg_Write32(H3DDMA_CAP0_Cap_CTL0_reg,cap_ctl0.regValue);
	 IoReg_Write32(H3DDMA_CAP0_WR_DMA_pxltobus_reg, h3ddma_cap0_wr_dma_pxltobus_reg.regValue);
	// why enable tripple buffer?, reset FIFO address at VS fall, HW auto set memory address, pixel encode 422,
	// length of data to fill in DDR LEN= 32, remaining part REM= 26

	// 2:2 pull up disable, first VS falling edge pixel num = 2, first VS falling edge line num = 2
	 tg_v1_end.regValue = IoReg_Read32(H3DDMA_TG_v1_end_reg);
	 tg_v1_end.tg_vend1= 2;
 	 tg_v1_end.tg_hend1= 2;
	 tg_v1_end.pullup_en = 0;
	 IoReg_Write32(H3DDMA_TG_v1_end_reg,tg_v1_end.regValue);
	// because input is a 2D signal, the value here is important here, just don't be equal to 0?
	  i3ddma_enable.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
//	 i3ddma_enable.cap_en = 1;
	 i3ddma_enable.lr_separate_en =1;
	 i3ddma_enable.tg_en =1;
	 if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_THRIP))
	 	i3ddma_enable.frc_en = 1;
	 else
	 	 i3ddma_enable.frc_en = 0;
 	IoReg_Write32(H3DDMA_I3DDMA_enable_reg,i3ddma_enable.regValue);// FRC, timing gen, 3D LR separate, Capture enable

	// 8-byte swap
	IoReg_Mask32(H3DDMA_CAP0_CTI_DMA_WR_Ctrl_reg, ~H3DDMA_CAP0_CTI_DMA_WR_Ctrl_cap0_dma_8byte_swap_mask, H3DDMA_CAP0_CTI_DMA_WR_Ctrl_cap0_dma_8byte_swap(0x1));

	//scart out use 3-buffer
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg, idma_cap_buf[0].phyaddr);  // block0, DDR buffer L1 start address
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg,  idma_cap_buf[1].phyaddr);  // block1, DDR buffer L2 start address
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_2_reg,  idma_cap_buf[2].phyaddr);  // block2, DDR buffer L3 start address
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_3_reg,  idma_cap_buf[3].phyaddr);  // block3, DDR buffer L4 start address

#endif // SCALER_ERROR
}

extern void  drvif_I3DDMA_set_capture(SIZE *outsize,unsigned char capformat422);
void scaler_vodmatoI3ddma_vgip2_config(SIZE outsize)
{
	unsigned char nMode;
//	int srcIdx;
	unsigned char source;
	unsigned char mode;
	SIZE insize;

	StructDisplayInfo *info;
	rtd_pr_i3ddma_debug("[scart out]###############%s~~1\n",__FUNCTION__);

	// keep the current VGIP1 input source information
	//scaler_vodmatoI3ddma_save_vgip1Info();//this can delete function

	// VGIP2 parameters setup sync from VGIP1
	//scaler_MaincvtSub_config_sync_vgip();
#if 0
	if(Scaler_InputSrcGetMainChType() == _SRC_HDMI)
		source = VGIP_SRC_TMDS;
	else if(Scaler_InputSrcGetMainChType() == _SRC_VO)
		source = VGIP_SRC_VODMA1;
	else
		source = VGIP_SRC_ADC;
#endif

	//LGE WebOS Project - only DTV using
	source = VGIP_SRC_HDR1;

	mode = VGIP_MODE_DIGITAL;

	rtd_pr_i3ddma_debug("[scart out]input source = %d\n",source);

	//VodmatoI3ddma_Set_Vgip(source, mode);
	I3DDMA_Set_Vgip(source,SCART_OUT_DTV, mode);

	// VGIP2 color format configure
	info = Scaler_DispGetInfoPtr();
	nMode = (info->IPV_ACT_LEN > 480? 1: 0);

	I3DDMA_ColorSpacergb2yuvtransfer(Scaler_InputSrcGetType(SLR_MAIN_DISPLAY), nMode, 0);

	insize.nWidth = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
	insize.nLength = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);

	//check this item
	if(insize.nWidth < 720){//if width < 720, must be the same with input, else always output 720x576
		outsize.nWidth = 720;//insize.nWidth;
	}

	rtd_pr_i3ddma_debug("[scart out]insize.nWidth = %d\n",insize.nWidth);
	rtd_pr_i3ddma_debug("[scart out]insize.nLength = %d\n",insize.nLength);
	rtd_pr_i3ddma_debug("[scart out]outsize.nWidth = %d\n",outsize.nWidth);
	rtd_pr_i3ddma_debug("[scart out]outsize.nLength = %d\n",outsize.nLength);

	if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_THRIP))
	{

		outsize.nLength /=2;
		rtd_pr_i3ddma_debug("[scart out]enter DI, nLength = %d\n",outsize.nLength);
		//if(TRUE ==  fwif_scaler_decide_display_cut4line(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)))// for delete 4 lines (Up 2 line and down 2 line)
		//	insize.nLength = insize.nLength -4;
	}

	Scaler_DispSetScaleStatus(SLR_SUB_DISPLAY, SLR_SCALE_H_DOWN, (insize.nWidth > outsize.nWidth)?true:false);
	Scaler_DispSetScaleStatus(SLR_SUB_DISPLAY, SLR_SCALE_V_DOWN, (insize.nLength > outsize.nLength)?true:false);

	Scaler_DispSetStatus(SLR_SUB_DISPLAY, SLR_DISP_10BIT, false);
	Scaler_DispSetStatus(SLR_SUB_DISPLAY, SLR_DISP_FSYNC_VUZD, 0x2);
	Scaler_DispSetStatus(SLR_SUB_DISPLAY, SLR_DISP_COMP, false);
	Scaler_DispSetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP, true);
	I3DDMA_ultrazoom_config_scaling_down(&insize, &outsize, 1);

	drvif_I3DDMA_set_capture_for2D(&outsize);
}

void drvif_i3ddma_vactive_end_irq(int enable)
{
	if(enable)
		IoReg_Mask32(DMA_VGIP_DMA_VGIP_CTRL_reg , ~_BIT25, _BIT25);	//enable VSync end interrupt
	else
		IoReg_Mask32(DMA_VGIP_DMA_VGIP_CTRL_reg , ~_BIT25, 0);	//disable VSync end interrupt
}

void disable_I3DDMA_dispDtoSub(void)
{
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	// IDMA timeing gen disable
       IoReg_Mask32(H3DDMA_I3DDMA_enable_reg, ~(/*H3DDMA_I3DDMA_enable_cap_en_mask|*/H3DDMA_I3DDMA_enable_lr_separate_en_mask
                               |H3DDMA_I3DDMA_enable_tg_en_mask|H3DDMA_I3DDMA_enable_frc_en_mask),/* H3DDMA_I3DDMA_enable_cap_en(0)
                               |*/H3DDMA_I3DDMA_enable_lr_separate_en(0)|H3DDMA_I3DDMA_enable_tg_en(0)
                               |H3DDMA_I3DDMA_enable_frc_en(0));// FRC, timing gen, 3D LR separate, Capture disable

	// release capture buffer
	I3DDMA_disp2tve_CapBufDeInit();

	// disable DMA VGIP when disable IDMA capture
	drvif_I3DDMA_Disable_Vgip();

	//Not to disable DAC power in based driver - LGE Project @ Crixus 20141110
	//drvif_module_tve_AVout_Enable(TVE_AVOUT_DISABLE);

	 drvif_i3ddma_vactive_end_irq(false);

	 //drvif_module_tve_interrupt(false);

	 //bVodma2I3Dma = false;
#endif // SCALER_ERROR
}

UINT8 tve_vflip_en_g = false;
static UINT8 fenable_dispDtoI3ddma;
static UINT8 fScartOut_isr_Enable;
//static UINT8 fScartOut_VO_clock_check;
//static UINT8 fScartOut_VO_scaling_enable;



unsigned char scaler_dispDtoI3ddma_get_enable(void)
{
	return fenable_dispDtoI3ddma;
}
void scaler_dispDtoI3ddma_set_enable(unsigned char enable)
{
	fenable_dispDtoI3ddma=enable;
}
unsigned char scaler_get_tve_vflip_enable(void)
{
	return tve_vflip_en_g;
}
void scaler_set_tve_vflip_enable(unsigned char enable)
{
	tve_vflip_en_g = enable;
}
unsigned char scaler_scart_out_isr_get_enable(void)
{
	return fScartOut_isr_Enable;
}
void scaler_scart_out_isr_set_enable(unsigned char enable)
{
	fScartOut_isr_Enable=enable;
}

#if 0	//remove tve
void scaler_vodmatoI3ddma_prog(TVE_VIDEO_MODE_SEL tve_mode, UINT8 tve_vflip_en)
{

	SIZE tve_outsize;
//	UINT32 i3ddmaCapAddr;
	//UINT8 tve_vflip_en = 1;

    	//if (bVodma2I3Dma == true)
    	//{
  	//	return;
    	//}

	if (tve_mode == TVE_NTSC) {
		tve_outsize.nWidth = 720;
		tve_outsize.nLength = 480;
	}
	else if (tve_mode == TVE_PAL_I) {
		tve_outsize.nWidth = 720;
		tve_outsize.nLength = 576;
	} else {
            rtd_pr_i3ddma_debug("wrong params  tve_mode=%d\n",(int)tve_mode);
            return ;
       }
	/*not to run dispD to i3ddma path*/
       	scaler_dispDtoI3ddma_set_enable(FALSE);

	disable_I3DDMA_dispDtoSub();
	//msleep(20);//why add this delay?
	scaler_vodmatoI3ddma_vgip2_config(tve_outsize);
	//Not to do initial in based driver - LGE Project @ Crixus 20141110
	//drvif_module_tve_init();
	drvif_module_tve_set_mode(tve_mode);
	drvif_module_tve_video_memory_setting(tve_vflip_en);
	drvif_module_tve_change_memory_setting(Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE),tve_vflip_en);

	//small than 576p should send RPC
	#if 0
	if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_THRIP)){
		scaler_scart_out_isr_set_enable(TRUE);
	}else{
		scaler_scart_out_isr_set_enable(FALSE);
	}
	#endif
	scaler_scart_out_isr_set_enable(TRUE);
	rtd_pr_i3ddma_debug("[scart out]fScartOut_isr_Enable = %x\n",fScartOut_isr_Enable);

	//VODMA clock fine tune
	if((Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) <= 720) && (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE) <= 576)){
		fScartOut_VO_clock_check = TRUE;
		//SE scaling up
		if((Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) < 720) || (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE) < 576))
			fScartOut_VO_scaling_enable = TRUE;
		else
			fScartOut_VO_scaling_enable = FALSE;
	}else{
		fScartOut_VO_clock_check = FALSE;
	}

	scaler_set_tve_vflip_enable(tve_vflip_en);

   	 //bVodma2I3Dma = true;
#if 0//M-disp debug using
	//M-domain Disp verify
	IoReg_ClearBits(MDOMAIN_DISP_DDR_MainSubCtrl_reg,_BIT1); //clear double buffer
	IoReg_ClearBits(MDOMAIN_DISP_DDR_MainCtrl_reg,MDOMAIN_DISP_DDR_MainCtrl_main_double_en_mask); //use single buffer. [9:8] = 0 use buffer 0,
									   // [9:8] = 1 use buffer 1
	i3ddmaCapAddr = IoReg_Read32(I3DDMA_CAP_L1_START_VADDR); //capture Buf L1
	//UINT32 i3ddmaCapAddr = IoReg_Read32(I3DDMA_CAP_L2_START_VADDR); //capture Buf L2
	rtd_pr_i3ddma_debug("##### i3ddmaCapAddr : %x ##### \n", i3ddmaCapAddr);  //capture Buf L
	IoReg_Write32(MDOMAIN_DISP_DDR_MainAddr_reg, i3ddmaCapAddr); //M-Disp Buffer0 use capture Buf
#endif
}
#endif

#if 0	//remove tve
void scaler_scart_out_tve_prog(TVE_VIDEO_MODE_SEL mode, UINT32 width){

	rtd_pr_i3ddma_debug("[scart out]@@@@@@%s\n",__FUNCTION__);

	//@ Crixus 20141202
	drvif_module_tve_init();
	drvif_module_tve_set_mode(mode);
	drvif_module_tve_video_memory_setting(scaler_get_tve_vflip_enable());
	drvif_module_tve_change_memory_setting(width, scaler_get_tve_vflip_enable());
}
#endif

//#ifdef DTV_SCART_OUT_ENABLE
static unsigned char vo_se_stretch_scaling_enable=FALSE;
extern unsigned int tve_new_linestep;

unsigned char vo_se_scaling_get_enable(void)
{
	return vo_se_stretch_scaling_enable;
}
void vo_se_scaling_set_enable(unsigned char enable)
{
	vo_se_stretch_scaling_enable=enable;
}

void VO_SE_Scaling(bool enable)
{
#if 0
	SCALER_DISPLAY_WINDOW *display_window;
	//unsigned int *pulTemp = NULL;
	unsigned int ulCount = 0;//, i = 0;
	int ret;

	rtd_pr_i3ddma_debug("[scart out]###############%s~~1 \n",__FUNCTION__);

	vo_se_scaling_set_enable(enable);

	display_window = (SCALER_DISPLAY_WINDOW *)Scaler_GetShareMemVirAddr(SCALERIOC_VO_SE_SCALING_ENABLE);
	ulCount = sizeof(SCALER_DISPLAY_WINDOW) / sizeof(unsigned int);

	display_window->outputWidth=720;
    	display_window->outputHeight=576;
	display_window->tve_linestep = tve_new_linestep;
	display_window->vo_scaling_enable = 0;//fScartOut_VO_scaling_enable;
	display_window->vo_scaling_alloc_mem_enable= enable;
	display_window->tve_vflip_en = scaler_get_tve_vflip_enable();
        	display_window->vo_clock_modify = fScartOut_VO_clock_check;
	display_window->scart_isr_enable = scaler_scart_out_isr_get_enable();

	//change endian
	display_window->outputWidth = htonl(display_window->outputWidth);
	display_window->outputHeight = htonl(display_window->outputHeight);
	display_window->tve_linestep = htonl(display_window->tve_linestep);
	display_window->vo_scaling_enable = htonl(display_window->vo_scaling_enable);
	display_window->vo_scaling_alloc_mem_enable = htonl(display_window->vo_scaling_alloc_mem_enable);
	display_window->tve_vflip_en = htonl(display_window->tve_vflip_en);
	display_window->vo_clock_modify = htonl(display_window->vo_clock_modify);
	display_window->scart_isr_enable = htonl(display_window->scart_isr_enable);

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_VO_SE_SCALING_ENABLE,0,0))){
		rtd_pr_i3ddma_debug("[scart out]ret=%d, Deinit VO ROTATE fail !!!\n", ret);
	}
#endif
	//tve isr is not ready @ 20140930
	//if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_THRIP))
		{
		//drvif_i3ddma_vactive_end_irq(true);
		drvif_scaler_vactive_end_irq(_ENABLE, SLR_SUB_DISPLAY);
		drvif_scaler_vactive_sta_irq(_ENABLE, SLR_SUB_DISPLAY);
		//drvif_module_tve_interrupt(true);
	}
	rtd_pr_i3ddma_debug("[scart out]###############%s~~3\n",__FUNCTION__);
}

void vo_se_scaling_ctrl(int enable)
{
    	VO_SE_Scaling(enable);
}

#ifdef CONFIG_DTV_SCART_OUT_ENABLE_444_FORMAT
long Scaler_VO_Set_VoForce422_enable(unsigned char bEnable)
{
	long ret = 0;

	rtd_pr_i3ddma_debug("[scart out]###############%s\n",__FUNCTION__);
#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)
	if (send_rpc_command(RPC_VIDEO, VIDEO_RPC_VOUT_ToAgent_Force_VO_Chroma_422, bEnable, 0, &ret))
		rtd_pr_i3ddma_debug("RPC fail!!\n");
#endif
	return ret;
}
#endif



//#endif

static void I3DDMA_Set_MHL_Vgip(unsigned char src,  unsigned char mode)
{
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	//dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;
	dma_vgip_dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;
	//dma_vgip_act_hsta_width_RBUS    dma_vgip_chn1_act_hsta_width_reg;
	dma_vgip_dma_vgip_act_hsta_width_RBUS dma_vgip_chn1_act_hsta_width_reg;
	//dma_vgip_act_vsta_length_RBUS   dma_vgip_chn1_act_vsta_length_reg;
	dma_vgip_dma_vgip_act_vsta_length_RBUS dma_vgip_chn1_act_vsta_length_reg;
	//dma_vgip_delay_RBUS dma_vgip_chn1_delay_reg;
	dma_vgip_dma_vgip_delay_RBUS  dma_vgip_chn1_delay_reg;
	//dma_vgip_cts_fifo_ctl_RBUS 			 	 dma_cts_fifo_ctl_reg;
	dma_vgip_dma_vgip_cts_fifo_ctl_RBUS  dma_cts_fifo_ctl_reg;
	//int dispD_hstaOffset=0;

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_in_clk_en = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_in_sel = src;
	dma_vgip_chn1_ctrl_reg.dma_digital_mode = mode;

	dma_vgip_chn1_ctrl_reg.dma_field_det_en = 0;

	dma_vgip_chn1_ctrl_reg.dma_field_inv = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_in_clk_en = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);


	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_ivrun = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);


	dma_vgip_chn1_act_hsta_width_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_ACT_HSTA_Width_reg);
	dma_vgip_chn1_act_vsta_length_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_ACT_VSTA_Length_reg);

	dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_STA_PRE);
	dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
	dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_sta = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_STA_PRE);
	dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_len= Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);

	IoReg_Write32(DMA_VGIP_DMA_VGIP_ACT_HSTA_Width_reg, dma_vgip_chn1_act_hsta_width_reg.regValue);
	IoReg_Write32(DMA_VGIP_DMA_VGIP_ACT_VSTA_Length_reg, dma_vgip_chn1_act_vsta_length_reg.regValue);

	// IHS/IVS delay
	dma_vgip_chn1_delay_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_DELAY_reg);
	dma_vgip_chn1_delay_reg.dma_ihs_dly = 0;
	dma_vgip_chn1_delay_reg.dma_ivs_dly = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_DELAY_reg, dma_vgip_chn1_delay_reg.regValue);

	dma_cts_fifo_ctl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTS_FIFO_CTL_reg);
	dma_cts_fifo_ctl_reg.dma_en_ctsfifo_vsrst_vgip = 1;
	dma_cts_fifo_ctl_reg.dma_force_ctsfifo_rstn_vgip = 1;
	dma_cts_fifo_ctl_reg.dma_force_ctsfifo_rstn_sdnr = 1;
	dma_cts_fifo_ctl_reg.dma_en_ctsfifo_vsrst_sdnr = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTS_FIFO_CTL_reg, dma_cts_fifo_ctl_reg.regValue);

	return;
#endif // SCALER_ERROR
}

unsigned char I3DDMA_MHL_CapBufInit(void) {
#ifndef BUILD_QUICK_SHOW
	unsigned int size = 1920*1080*4;

	if(idma_cap_buf[0].phyaddr == 0)
	{
			/*Use pli memory allocation @Crixus 20150801*/
			unsigned long malloccacheaddr  = (unsigned long)dvr_malloc_uncached_specific(size*2, GFP_DCU2_FIRST, NULL);

			#ifdef CONFIG_ARM64
			if (malloccacheaddr == (unsigned long)NULL) {
				rtd_pr_i3ddma_debug("malloc idma buffer fail .......................................\n");
				return 1;
			}
			#else
			if (malloccacheaddr == (unsigned int)NULL) {
				rtd_pr_i3ddma_debug("malloc idma buffer fail .......................................\n");
				return 1;
			}
			#endif
			idma_cap_buf[0].size = size;
			idma_cap_buf[0].cache = (void*)(malloccacheaddr);
			idma_cap_buf[0].phyaddr = (unsigned long)dvr_to_phys((void*)idma_cap_buf[0].cache);

			rtd_pr_i3ddma_debug("[idma-cap-buf] buf[0]=%lx(%d KB), phy(%lx) \n", (unsigned long)idma_cap_buf[0].cache, idma_cap_buf[0].size >> 10, (unsigned long)idma_cap_buf[0].phyaddr);

			idma_cap_buf[1].size = size;
			idma_cap_buf[1].cache = (void*)(malloccacheaddr + size);
			idma_cap_buf[1].phyaddr = (unsigned long)dvr_to_phys((void*)idma_cap_buf[1].cache);
			rtd_pr_i3ddma_debug("[idma-cap-buf] buf[1]=%lx(%d KB), phy(%lx) \n", (unsigned long)idma_cap_buf[1].cache, idma_cap_buf[1].size >> 10, (unsigned long)idma_cap_buf[1].phyaddr);

			IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_low_reg, idma_cap_buf[0].phyaddr );
			IoReg_Write32(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_reg, idma_cap_buf[1].phyaddr + idma_cap_buf[1].size);
	}
#endif
    return 0;
}

void  drvif_I3DDMA_set_capture_Mhl(SIZE *outsize)
{
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	//unsigned int num;
	unsigned int len;
	unsigned int rem;
	unsigned int lineunits;

	 //lr_separate_ctrl1_RBUS lr_separate_ctrl1;
	 h3ddma_lr_separate_ctrl1_RBUS lr_separate_ctrl1;
	 //lr_separate_ctrl2_RBUS lr_separate_ctrl2;
	 h3ddma_lr_separate_ctrl2_RBUS lr_separate_ctrl2;
	 //cap_ctl1_RBUS cap_ctl1;
	// h3ddma_cap_ctl1_RBUS cap_ctl1;
	 //cap_ctl0_RBUS cap_ctl0;
	 h3ddma_cap0_cap_ctl0_RBUS cap_ctl0;
	 //tg_v1_end_RBUS tg_v1_end;
	 h3ddma_tg_v1_end_RBUS tg_v1_end;
	 //i3ddma_enable_RBUS i3ddma_enable;
	 h3ddma_i3ddma_enable_RBUS i3ddma_enable;
//	 i3ddma_i3ddma_byte_channel_swap_RBUS  i3ddma_byte_channel_swap;
	 h3ddma_cap0_wr_dma_pxltobus_RBUS h3ddma_cap0_wr_dma_pxltobus_reg;

	I3DDMA_MHL_CapBufInit();

	// dispi hdmi_3ddma
	 lr_separate_ctrl1.regValue = IoReg_Read32(H3DDMA_LR_Separate_CTRL1_reg);
	 lr_separate_ctrl1.hdmi_3d_structure = 0x0a;
 	 lr_separate_ctrl1.progressive = 1;
	// lr_separate_ctrl1.hdmi_3d_sel = 0;
 	 IoReg_Write32(H3DDMA_LR_Separate_CTRL1_reg,lr_separate_ctrl1.regValue);

	lr_separate_ctrl2.regValue = IoReg_Read32(H3DDMA_LR_Separate_CTRL2_reg);
	lr_separate_ctrl2.hact = outsize->nWidth;
	lr_separate_ctrl2.vact = outsize->nLength;
       IoReg_Write32(H3DDMA_LR_Separate_CTRL2_reg,lr_separate_ctrl2.regValue);

	//Capture
	//check this item
	//rtd_pr_i3ddma_info("[scart out]I3DDMA: 422cap = %d, 8bit=%d\n",Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_422CAP), !Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_10BIT));
	if(Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_422CAP)) //422
		lineunits = (outsize->nWidth * 8 * 2) /128;
	else   //444
		lineunits = (outsize->nWidth * 8 * 3) /128;
	len = 32;
	//num = (unsigned int)(lineunits / len);
	rem = (unsigned int)(lineunits % len);
	if(rem == 0)
	{
		//num -= 1;
		rem = 32;
	}
#if 0
	cap_ctl1.regValue = IoReg_Read32(H3DDMA_Cap_CTL1_reg);
	cap_ctl1.cap_write_num = num;
	cap_ctl1.cap_water_lv = len;
	IoReg_Write32(H3DDMA_Cap_CTL1_reg,cap_ctl1.regValue);
#endif
	cap_ctl0.regValue = IoReg_Read32(H3DDMA_CAP0_Cap_CTL0_reg);
	h3ddma_cap0_wr_dma_pxltobus_reg.regValue = IoReg_Read32(H3DDMA_CAP0_WR_DMA_pxltobus_reg);
//	cap_ctl0.cap_write_rem = rem;
//	cap_ctl0.cap_write_len = len;
	cap_ctl0.cap0_auto_block_sel_en = 1;
//	cap_ctl0.vs_fall_rst_en = 1;
	cap_ctl0.cap0_quad_buf_en = 0;

	cap_ctl0.cap0_triple_buf_en= 0;

	if(Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_422CAP)) //422
 	{
 		h3ddma_cap0_wr_dma_pxltobus_reg.cap0_pixel_encoding = 1;
		h3ddma_cap0_wr_dma_pxltobus_reg.cap0_bit_sel= 0;
	 }
	 else   //444
 	{
  		h3ddma_cap0_wr_dma_pxltobus_reg.cap0_pixel_encoding = 0;
		h3ddma_cap0_wr_dma_pxltobus_reg.cap0_bit_sel = 0;
 	}

 	IoReg_Write32(H3DDMA_CAP0_Cap_CTL0_reg,cap_ctl0.regValue);
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_pxltobus_reg,h3ddma_cap0_wr_dma_pxltobus_reg.regValue);
	// why enable tripple buffer?, reset FIFO address at VS fall, HW auto set memory address, pixel encode 422,
	// length of data to fill in DDR LEN= 32, remaining part REM= 26

	// 2:2 pull up disable, first VS falling edge pixel num = 2, first VS falling edge line num = 2
	 tg_v1_end.regValue = IoReg_Read32(H3DDMA_TG_v1_end_reg);
	 tg_v1_end.tg_vend1= 2;
 	 tg_v1_end.tg_hend1= 2;
	 tg_v1_end.pullup_en = 0;
	 IoReg_Write32(H3DDMA_TG_v1_end_reg,tg_v1_end.regValue);
	// because input is a 2D signal, the value here is important here, just don't be equal to 0?
	  i3ddma_enable.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
	// i3ddma_enable.cap_en = 1;
	 i3ddma_enable.lr_separate_en =1;
	 i3ddma_enable.tg_en =1;
	 i3ddma_enable.frc_en = 1;
 	IoReg_Write32(H3DDMA_I3DDMA_enable_reg,i3ddma_enable.regValue);// FRC, timing gen, 3D LR separate, Capture enable

	// 8-byte swap
	//IoReg_Mask32(H3DDMA_I3DDMA_byte_channel_swap_reg, ~H3DDMA_I3DDMA_byte_channel_swap_dma_8byte_swap_mask, H3DDMA_I3DDMA_byte_channel_swap_dma_8byte_swap(0x1));

	//scart out use 3-buffer
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg, idma_cap_buf[0].phyaddr);  // block0, DDR buffer L1 start address
	IoReg_Write32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg,  idma_cap_buf[1].phyaddr);  // block1, DDR buffer L2 start address
	//IoReg_Write32(H3DDMA_Cap_L3_Start_reg,  idma_cap_buf[2].phyaddr);  // block2, DDR buffer L3 start address
	//IoReg_Write32(H3DDMA_Cap_L4_Start_reg,  idma_cap_buf[3].phyaddr);  // block3, DDR buffer L4 start address

#endif // SCALER_ERROR
}

unsigned char drvif_rpc_confighdmi(void)
{
#ifndef BUILD_QUICK_SHOW
#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)
	unsigned long result = 0;
#endif
	VIDEO_RPC_VOUT_CONFIG_HDMI_3D *config_data;

	//struct page *page = 0;
	//page = alloc_page(GFP_KERNEL);
	//config_datatmp = (VIDEO_RPC_VOUT_SET_3D_MODE *)page_address(page);
	unsigned long vir_addr, vir_addr_noncache;
	unsigned int phy_addr;

	vir_addr = (unsigned long)dvr_malloc_uncached_specific(sizeof(VIDEO_RPC_VOUT_CONFIG_HDMI_3D), GFP_DCU1_LIMIT, (void **)&vir_addr_noncache);
	//phy_addr = (unsigned int)virt_to_phys((void*)vir_addr);
	phy_addr = (unsigned int)dvr_to_phys((void*)vir_addr);

	config_data = (VIDEO_RPC_VOUT_CONFIG_HDMI_3D *)vir_addr_noncache;

	memset(config_data, 0, sizeof(VIDEO_RPC_VOUT_CONFIG_HDMI_3D));
	config_data->width = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
	config_data->height = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);
	config_data->chroma_fmt = VO_CHROMA_YUV422;//(vgip2CtrlInfo.srcInfo.chroma_422Cap? VO_CHROMA_YUV422: VO_CHROMA_YUV444);
	config_data->data_bit_width = 8;
	config_data->framerate =  Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000);
	config_data->progressive = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_INTERLACE)?0:1;
	config_data->l1_st_adr = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg);
	config_data->r1_st_adr = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_0_reg);
	config_data->l2_st_adr = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg);
	config_data->r2_st_adr = IoReg_Read32(H3DDMA_CAP0_WR_DMA_num_bl_wrap_addr_1_reg);
	config_data->hw_auto_mode = 1;
	config_data->quincunx_en = 0;
	config_data->videoPlane = VO_VIDEO_PLANE_V1;

	config_data->width = htonl(config_data->width);
	config_data->height = htonl(config_data->height);
	config_data->chroma_fmt = htonl(config_data->chroma_fmt);
	config_data->data_bit_width = htonl(config_data->data_bit_width);
	config_data->framerate =  htonl(config_data->framerate);
	config_data->progressive = htonl(config_data->progressive);
	config_data->l1_st_adr = htonl(config_data->l1_st_adr);
	config_data->r1_st_adr = htonl(config_data->r1_st_adr);
	config_data->l2_st_adr = htonl(config_data->l2_st_adr);
	config_data->r2_st_adr = htonl(config_data->r2_st_adr);
	config_data->hw_auto_mode = htonl(config_data->hw_auto_mode);
	config_data->quincunx_en = htonl(config_data->quincunx_en);
	config_data->videoPlane = htonl(config_data->videoPlane);

#if IS_ENABLED(CONFIG_RTK_KDRV_RPC)
	if (RPC_FAIL==send_rpc_command(RPC_VIDEO, VIDEO_RPC_VOUT_ToAgent_ConfigHDMI_3D,phy_addr, 0, &result))
	{
		rtd_pr_i3ddma_debug("RPC fail!!\n");
		dvr_free((void *)vir_addr);
		return FALSE;
	}
#endif
	dvr_free((void *)vir_addr);
	return TRUE;
#else
    return 0;
#endif
}

void scaler_mhl_i3ddma_config(void)
{
	//unsigned char nMode;
	unsigned char source;
	unsigned char mode;
	SIZE insize;
	SIZE outsize;
	//StructDisplayInfo *info;

	source = get_vgip_in_sel_by_display(SLR_SUB_DISPLAY);
	mode = 0;

	I3DDMA_Set_MHL_Vgip(source, mode);

	// VGIP2 color format configure
	//info = Scaler_DispGetInfoPtr();
	//nMode = (info->IPV_ACT_LEN > 480? 1: 0);

	insize.nWidth = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
	insize.nLength = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);
	outsize.nWidth = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
	outsize.nLength = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);

	Scaler_DispSetScaleStatus(SLR_SUB_DISPLAY, SLR_SCALE_H_DOWN, (insize.nWidth > outsize.nWidth)?true:false);
	Scaler_DispSetScaleStatus(SLR_SUB_DISPLAY, SLR_SCALE_V_DOWN, (insize.nLength > outsize.nLength)?true:false);

	Scaler_DispSetStatus(SLR_SUB_DISPLAY, SLR_DISP_10BIT, false);
	Scaler_DispSetStatus(SLR_SUB_DISPLAY, SLR_DISP_FSYNC_VUZD, 0x2);
	Scaler_DispSetStatus(SLR_SUB_DISPLAY, SLR_DISP_COMP, false);
	Scaler_DispSetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP, true);

//@@@@@@@@@@@@@@@@@@@@@ MUST FIX ME @@@@@@@
/*	if(GET_HDMI_COLOR_SPACE() == COLOR_RGB)
		I3DDMA_ColorSpacergb2yuvtransfer(_SRC_HDMI, nMode, 0);
*/
//@@@@@@@@@@@@@@@@@@@@@ MUST FIX ME @@@@@@@
	I3DDMA_ultrazoom_config_scaling_down(&insize, &outsize, 1);

	drvif_I3DDMA_set_capture_Mhl(&outsize);

	drvif_rpc_confighdmi();
	return;
}

void I3DDMA_Set_HDMI3D_Vgip(void)
{
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	//dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;
	dma_vgip_dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;
	//dma_vgip_act_hsta_width_RBUS    dma_vgip_chn1_act_hsta_width_reg;
	dma_vgip_dma_vgip_act_hsta_width_RBUS dma_vgip_chn1_act_hsta_width_reg;
	//dma_vgip_act_vsta_length_RBUS   dma_vgip_chn1_act_vsta_length_reg;
	dma_vgip_dma_vgip_act_vsta_length_RBUS dma_vgip_chn1_act_vsta_length_reg;
	//dma_vgip_delay_RBUS dma_vgip_chn1_delay_reg;
	dma_vgip_dma_vgip_delay_RBUS  dma_vgip_chn1_delay_reg;
	//dma_vgip_cts_fifo_ctl_RBUS 			 	 dma_cts_fifo_ctl_reg;
	dma_vgip_dma_vgip_cts_fifo_ctl_RBUS  dma_cts_fifo_ctl_reg;
	//int dispD_hstaOffset=0;

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_in_clk_en = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_in_sel = get_vgip_in_sel_by_display(SLR_SUB_DISPLAY);
	dma_vgip_chn1_ctrl_reg.dma_digital_mode = VGIP_MODE_DIGITAL;

	dma_vgip_chn1_ctrl_reg.dma_field_det_en = 0;

	dma_vgip_chn1_ctrl_reg.dma_field_inv = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_in_clk_en = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);


	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_ivrun = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

	// enable IDMA VGIP data end interrupt
	if(get_HDMI_HDR_mode() == HDR_DOLBY_HDMI){
		dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
		dma_vgip_chn1_ctrl_reg.dma_vact_end_ie= 1;
		IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);
	}

	dma_vgip_chn1_act_hsta_width_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_ACT_HSTA_Width_reg);
	dma_vgip_chn1_act_vsta_length_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_ACT_VSTA_Length_reg);

	dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_STA_PRE);
	dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
	dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_sta = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_STA_PRE);
	dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_len= Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);

	IoReg_Write32(DMA_VGIP_DMA_VGIP_ACT_HSTA_Width_reg, dma_vgip_chn1_act_hsta_width_reg.regValue);
	IoReg_Write32(DMA_VGIP_DMA_VGIP_ACT_VSTA_Length_reg, dma_vgip_chn1_act_vsta_length_reg.regValue);

	// IHS/IVS delay
	dma_vgip_chn1_delay_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_DELAY_reg);
	dma_vgip_chn1_delay_reg.dma_ihs_dly = 0;
	dma_vgip_chn1_delay_reg.dma_ivs_dly = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_DELAY_reg, dma_vgip_chn1_delay_reg.regValue);

	dma_cts_fifo_ctl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTS_FIFO_CTL_reg);
	dma_cts_fifo_ctl_reg.dma_en_ctsfifo_vsrst_vgip = 1;
	dma_cts_fifo_ctl_reg.dma_force_ctsfifo_rstn_vgip = 1;
	dma_cts_fifo_ctl_reg.dma_force_ctsfifo_rstn_sdnr = 1;
	dma_cts_fifo_ctl_reg.dma_en_ctsfifo_vsrst_sdnr = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTS_FIFO_CTL_reg, dma_cts_fifo_ctl_reg.regValue);

	return;
#endif // SCALER_ERROR
}


void I3DDMA_Set_Metadata_Vgip(unsigned char enable)
{
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	//dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;
	dma_vgip_dma_vgip_ctrl_RBUS     dma_vgip_chn1_ctrl_reg;
	//dma_vgip_act_hsta_width_RBUS    dma_vgip_chn1_act_hsta_width_reg;
	dma_vgip_dma_vgip_act_hsta_width_RBUS dma_vgip_chn1_act_hsta_width_reg;
	//dma_vgip_act_vsta_length_RBUS   dma_vgip_chn1_act_vsta_length_reg;
	dma_vgip_dma_vgip_act_vsta_length_RBUS dma_vgip_chn1_act_vsta_length_reg;
	//dma_vgip_delay_RBUS dma_vgip_chn1_delay_reg;
	dma_vgip_dma_vgip_delay_RBUS  dma_vgip_chn1_delay_reg;
	//dma_vgip_cts_fifo_ctl_RBUS 			 	 dma_cts_fifo_ctl_reg;
	dma_vgip_dma_vgip_cts_fifo_ctl_RBUS  dma_cts_fifo_ctl_reg;
	//int dispD_hstaOffset=0;

	unsigned char mode =VGIP_MODE_ANALOG;
	unsigned char src =get_vgip_in_sel_by_display(SLR_SUB_DISPLAY);

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_in_clk_en = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_in_sel = src;
	dma_vgip_chn1_ctrl_reg.dma_digital_mode = mode;

	dma_vgip_chn1_ctrl_reg.dma_field_det_en = 0;

	dma_vgip_chn1_ctrl_reg.dma_field_inv = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_in_clk_en = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);


	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	dma_vgip_chn1_ctrl_reg.dma_ivrun = 1;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);

	// enable IDMA VGIP data end interrupt
	if(get_HDMI_HDR_mode() == HDR_DOLBY_HDMI){
		dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
		dma_vgip_chn1_ctrl_reg.dma_vact_end_ie= 1;
		IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);
	}

	dma_vgip_chn1_act_hsta_width_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_ACT_HSTA_Width_reg);
	dma_vgip_chn1_act_vsta_length_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_ACT_VSTA_Length_reg);
//@@@@@@@MUST FIX ME@@@@@@@@@@@@@@@@@@@
	//dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta = GET_MODE_IHSTA();//Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_STA_PRE);
	//dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = GET_MODE_IHWIDTH();//Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
	//dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_sta = GET_MODE_IVSTA();//Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_STA_PRE);
	//dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_len= GET_MODE_IVHEIGHT();//Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);

	//IoReg_Write32(DMA_VGIP_DMA_VGIP_ACT_HSTA_Width_reg, dma_vgip_chn1_act_hsta_width_reg.regValue);
	//IoReg_Write32(DMA_VGIP_DMA_VGIP_ACT_VSTA_Length_reg, dma_vgip_chn1_act_vsta_length_reg.regValue);
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// IHS/IVS delay
	dma_vgip_chn1_delay_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_DELAY_reg);
	dma_vgip_chn1_delay_reg.dma_ihs_dly = 0;
	dma_vgip_chn1_delay_reg.dma_ivs_dly = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_DELAY_reg, dma_vgip_chn1_delay_reg.regValue);

	dma_cts_fifo_ctl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTS_FIFO_CTL_reg);
	dma_cts_fifo_ctl_reg.dma_en_ctsfifo_vsrst_vgip = 1;
	dma_cts_fifo_ctl_reg.dma_force_ctsfifo_rstn_vgip = 1;
	dma_cts_fifo_ctl_reg.dma_force_ctsfifo_rstn_sdnr = 1;
	dma_cts_fifo_ctl_reg.dma_en_ctsfifo_vsrst_sdnr = 0;
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTS_FIFO_CTL_reg, dma_cts_fifo_ctl_reg.regValue);

	return;
#endif // SCALER_ERROR
}

void I3DDMA_Parse_HDMI3D_pattern(void)
{
       //unsigned char nMode;
       SIZE insize;
       SIZE outsize;
       //StructDisplayInfo *info;

       I3DDMA_Set_HDMI3D_Vgip();

       // VGIP2 color format configure
       //info = Scaler_DispGetInfoPtr();
       //nMode = (info->IPV_ACT_LEN > 480? 1: 0);

       insize.nWidth = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
       insize.nLength = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);
       outsize.nWidth = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
       outsize.nLength = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);

       Scaler_DispSetScaleStatus(SLR_SUB_DISPLAY, SLR_SCALE_H_DOWN, (insize.nWidth > outsize.nWidth)?true:false);
       Scaler_DispSetScaleStatus(SLR_SUB_DISPLAY, SLR_SCALE_V_DOWN, (insize.nLength > outsize.nLength)?true:false);

       Scaler_DispSetStatus(SLR_SUB_DISPLAY, SLR_DISP_10BIT, false);
       Scaler_DispSetStatus(SLR_SUB_DISPLAY, SLR_DISP_FSYNC_VUZD, 0x2);
       Scaler_DispSetStatus(SLR_SUB_DISPLAY, SLR_DISP_COMP, false);
       Scaler_DispSetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP, true);

//@@@@@@@@@@@@@@@@@@@@@ MUST FIX ME @@@@@@@
/*       if(GET_HDMI_COLOR_SPACE() == COLOR_RGB)
               I3DDMA_ColorSpacergb2yuvtransfer(_SRC_HDMI, nMode, 0);
   */
//@@@@@@@@@@@@@@@@@@@@@ MUST FIX ME @@@@@@@

       I3DDMA_ultrazoom_config_scaling_down(&insize, &outsize, 1);

       drvif_I3DDMA_set_capture_Mhl(&outsize);

       return;
}


void fw_i3ddma_set_vgip(unsigned char dev, unsigned char src, unsigned char mode)
{
	unsigned char isHDMIOrDP = FALSE;
	VGIP_PATH vgip_path;
	dma_vgip_dma_vgip_htotal_RBUS dma_vgip_dma_vgip_htotal_reg;
	dma_vgip_dma_vgip_ctrl_RBUS dma_vgip_chn1_ctrl_reg;
	dma_vgip_dma_vgip_act_hsta_width_RBUS dma_vgip_chn1_act_hsta_width_reg;
	dma_vgip_dma_vgip_act_vsta_length_RBUS dma_vgip_chn1_act_vsta_length_reg;
	dma_vgip_dma_vgip_delay_RBUS dma_vgip_chn1_delay_reg;
	dma_vgip_dma_vgip_cts_fifo_ctl_RBUS dma_cts_fifo_ctl_reg;
	dma_vgip_dma_vgip_misc_RBUS dma_vgip_dma_vgip_misc_reg;
	// dma_vgip_dma_data_path_select_RBUS dma_vgip_dma_data_path_select_reg; // fixme 6982
	//int dispD_hstaOffset=0;
	RBus_UInt32 u32_cal_ivs_dly= 0;	//default
	unsigned int polarity=0;
	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(vgip_ctrl_reg_addr[dev]);
	dma_vgip_chn1_ctrl_reg.dma_in_clk_en = 0;
	IoReg_Write32(vgip_ctrl_reg_addr[dev], dma_vgip_chn1_ctrl_reg.regValue);

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(vgip_ctrl_reg_addr[dev]);
	dma_vgip_chn1_ctrl_reg.dma_in_sel = src;
	dma_vgip_chn1_ctrl_reg.dma_digital_mode = mode;

	//modify the field setting by liyu chen suggestion
	dma_vgip_chn1_ctrl_reg.dma_field_det_en = 0;	//bit16
	dma_vgip_chn1_ctrl_reg.dma_field_sync_edge = 1;	//bit17
	dma_vgip_chn1_ctrl_reg.dma_field_inv = 0;		//bit18

	isHDMIOrDP = ((src == VGIP_SRC_HDMI1) || (src == VGIP_SRC_HDMI2) ||
				(src == VGIP_SRC_HDMI3) || (src == VGIP_SRC_HDMI4)) ? TRUE : FALSE;

	if(isHDMIOrDP)
	{
		polarity=IDDMA_GetVFEInfoByPort(dev,SLR_INPUT_V_SYNC_HIGH_PERIOD);//fix K5LG-1397

		if(Get_Val_DP_Vsync_Regen_Pos(dev) == DP_NO_REGEN_VSYNC)
		{
			if ((polarity>>1)&0x01)
				dma_vgip_chn1_ctrl_reg.dma_vs_inv = 0;
			else
				dma_vgip_chn1_ctrl_reg.dma_vs_inv = 1;
		}

		if (polarity&0x01)
			dma_vgip_chn1_ctrl_reg.dma_hs_inv = 0;
		else
			dma_vgip_chn1_ctrl_reg.dma_hs_inv = 1;
#if 0 // fixme 6982
		dma_vgip_dma_data_path_select_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_Data_Path_Select_reg);
		if(get_i3ddma_2p_mode_flag())
		{
			dma_vgip_dma_data_path_select_reg.dma_2pxl_en = 1;
		}
		else
		{
			dma_vgip_dma_data_path_select_reg.dma_2pxl_en = 0;
		}
		IoReg_Write32(DMA_VGIP_DMA_Data_Path_Select_reg, dma_vgip_dma_data_path_select_reg.regValue);
#endif
	}

	IoReg_Write32(vgip_ctrl_reg_addr[dev], dma_vgip_chn1_ctrl_reg.regValue);

	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(vgip_ctrl_reg_addr[dev]);
	dma_vgip_chn1_ctrl_reg.dma_in_clk_en = 1;
	IoReg_Write32(vgip_ctrl_reg_addr[dev], dma_vgip_chn1_ctrl_reg.regValue);


	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(vgip_ctrl_reg_addr[dev]);
	dma_vgip_chn1_ctrl_reg.dma_ivrun = 1;
	IoReg_Write32(vgip_ctrl_reg_addr[dev], dma_vgip_chn1_ctrl_reg.regValue);

	switch(dev)
	{
		case I3DDMA_DEV:
			vgip_path = VGIP_PATH_I3;
		break;
		case I4DDMA_DEV:
			vgip_path = VGIP_PATH_I4;
		break;
		case I5DDMA_DEV:
			vgip_path = VGIP_PATH_I5;
		break;
		case I6DDMA_DEV:
			vgip_path = VGIP_PATH_I6;
		break;
		default:
			vgip_path = VGIP_PATH_I3;
		break;
	}
	two_pixel_mode_ctrl(vgip_path, IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_SOURCE_PIXEL_MODE));


	// enable IDMA VGIP data end interrupt,only i3 support dolby
	if((get_HDMI_HDR_mode() == HDR_DOLBY_HDMI) && (IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE)){
		dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
		dma_vgip_chn1_ctrl_reg.dma_vact_end_ie= 1;
		IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);
	}

#if 0//no need
	dma_vgip_chn1_ctrl_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_CTRL_reg);
	if(dma_vgip_chn1_ctrl_reg.dma_digital_mode)
	{//digital mode
		dma_vgip_chn1_ctrl_reg.dma_vs_by_hs_en_n = 1;
	}
	else
	{
		dma_vgip_chn1_ctrl_reg.dma_vs_by_hs_en_n = 0;
	}
	IoReg_Write32(DMA_VGIP_DMA_VGIP_CTRL_reg, dma_vgip_chn1_ctrl_reg.regValue);
#endif

	if((mode == VGIP_MODE_ANALOG) && (IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE))
	{
		if(IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_VSC_SOURCE_TYPE) == VSC_INPUTSRC_AVD)	//av
		{
			if(IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_V_SYNC_HIGH_PERIOD)==0)	//0 => inverse
			{
				u32_cal_ivs_dly = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_V_COUNT) - IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IVSYNCPULSECOUNT) + 1;
			}
			else
			{
				u32_cal_ivs_dly = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IVSYNCPULSECOUNT) + 1;
			}
		}
#ifdef CONFIG_SUPPORT_SRC_ADC
		else if(IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_VSC_SOURCE_TYPE) == VSC_INPUTSRC_ADC)	//ypo
		{
			if(IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_V_SYNC_HIGH_PERIOD)==0)	//0 => inverse
			{
				u32_cal_ivs_dly = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_V_COUNT) - IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IVSYNCPULSECOUNT) + 1;
			}
			else
			{
				u32_cal_ivs_dly = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IVSYNCPULSECOUNT) + 1;
			}
		}
#endif // #ifdef CONFIG_SUPPORT_SRC_ADC
	}
	else if(isHDMIOrDP)	//hdmi
	{
		if(IDDMA_GetVFEStatus(dev, SLR_DISP_INTERLACE))
		{//only interlace need delay
			if((IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_V_SYNC_HIGH_PERIOD)&_BIT1)==0)	//b'00 => inverse
			{
				u32_cal_ivs_dly = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_V_COUNT) - IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IVSYNCPULSECOUNT) + 1;
			}
			else	// b'10
			{
				u32_cal_ivs_dly = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IVSYNCPULSECOUNT);	//already add in offline measure
			}
			rtd_pr_i3ddma_info("[%s] %d dma_delay=%d, interlace=%d, input_vsync_pol=0x%x, input_vtotal=%d, input_vsync_width=%d\n", __FUNCTION__, __LINE__, u32_cal_ivs_dly, IDDMA_GetVFEStatus(dev, SLR_DISP_INTERLACE), IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_V_SYNC_HIGH_PERIOD), IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_V_COUNT), IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IVSYNCPULSECOUNT));
		}
	}

	if(u32_cal_ivs_dly>8)	//protection
	{
		rtd_pr_ioreg_emerg("%s(%d)-%s: Warning:dma delay incorrect=%d \n",__FILE__,__LINE__,__FUNCTION__,u32_cal_ivs_dly);
		u32_cal_ivs_dly = 8;
	}

	if(IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_V_COUNT)==0 && IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IVSYNCPULSECOUNT)==0)
	{	//fail to get offline measure data protection
		u32_cal_ivs_dly =0;
	}

	dma_vgip_chn1_act_hsta_width_reg.regValue = IoReg_Read32(vgip_hsta_width_reg_addr[dev]);
	dma_vgip_chn1_act_vsta_length_reg.regValue = IoReg_Read32(vgip_vsta_length_reg_addr[dev]);
	if (IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_VSC_SOURCE_TYPE) == VSC_INPUTSRC_ADC) {
		/*ADC Analog mode: Real start = X+2*/
		dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta = (mode == VGIP_MODE_ANALOG) ? (IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_STA_PRE)+2) : 0;
	} else {
		dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta = (mode == VGIP_MODE_ANALOG) ? IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_STA_PRE) : 0;
	}
	if(IDDMA_Get_DMAWorkMode(dev) != IDDMA_DISPLAY_MODE)
	{
		if(IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_SOURCE_PIXEL_MODE))
		{
			if(dev != I3DDMA_DEV)
				dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_WID_PRE)/2;
			else
				dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_WID_PRE);
		}
		else
			dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_WID_PRE);
	}
	else
	{
		dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_wid = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPH_ACT_WID_PRE);
	}

	dma_vgip_dma_vgip_misc_reg.regValue = IoReg_Read32(vgip_misc_reg_addr[dev]);
	dma_vgip_dma_vgip_htotal_reg.regValue = IoReg_Read32(vgip_htotal_reg_addr[dev]);
	if(dma_vgip_chn1_ctrl_reg.dma_digital_mode == VGIP_MODE_ANALOG)
	{
		if(IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPV_ACT_STA_PRE)>=u32_cal_ivs_dly)	// according liyu chen suggestion
		{
			dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_sta= IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPV_ACT_STA_PRE)-u32_cal_ivs_dly;
		}
		else
		{
			dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_sta= IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPV_ACT_STA_PRE);
			rtd_pr_i3ddma_warn("Warning: i3ddma active start(%d) is smaller than %d\n", dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_sta, u32_cal_ivs_dly);
		}
		//dma_vgip_dma_vgip_misc_reg.dma_hporch_num = 0;
		dma_vgip_dma_vgip_misc_reg.dma_hs_mode = 1;//hs mode
		dma_vgip_dma_vgip_misc_reg.dma_v1_v3_swap = 0;
		dma_vgip_dma_vgip_misc_reg.dma_v2_v3_swap = 0;
		dma_vgip_dma_vgip_misc_reg.dma_v1_v2_swap = 0;
		dma_vgip_dma_vgip_htotal_reg.dma_htotal_num = 0;//set h toal
	}else {//digital mode vstart need to be 0
		dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_sta = 0;
		//dma_vgip_dma_vgip_misc_reg.dma_hporch_num = 0;	//no need to set h porch number for no hs mode
		dma_vgip_dma_vgip_misc_reg.dma_hs_mode = 0;//no hs mode
		dma_vgip_dma_vgip_htotal_reg.dma_htotal_num = IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_H_LEN) - 1;//set h toal

		if((src == VGIP_SRC_HDR1) && (IDDMA_Get_DMAWorkMode(dev) != IDDMA_DISPLAY_MODE))
		{
			if((IDDMA_Get_DMAWorkMode(dev) == IDDMA_NN_MODE) && ((IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_VSC_SOURCE_TYPE) == VSC_INPUTSRC_JPEG) ||
				((IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_VSC_SOURCE_TYPE) == VSC_INPUTSRC_VDEC) && (get_OTT_HDR_mode() == HDR_DOLBY_COMPOSER))))
			{
				dma_vgip_dma_vgip_misc_reg.dma_v1_v2_swap = 0;
				dma_vgip_dma_vgip_misc_reg.dma_v2_v3_swap = 0;
				dma_vgip_dma_vgip_misc_reg.dma_v1_v3_swap = 0;
			}
			else
			{
				dma_vgip_dma_vgip_misc_reg.dma_v1_v2_swap = 1;
				dma_vgip_dma_vgip_misc_reg.dma_v2_v3_swap = 1;
				dma_vgip_dma_vgip_misc_reg.dma_v1_v3_swap = 0;
			}
		}
		else
		{
			dma_vgip_dma_vgip_misc_reg.dma_v1_v3_swap = 0;
			dma_vgip_dma_vgip_misc_reg.dma_v2_v3_swap = 0;
			dma_vgip_dma_vgip_misc_reg.dma_v1_v2_swap = 0;
		}
    }
	dma_vgip_chn1_act_vsta_length_reg.dma_iv_act_len= IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_IPV_ACT_LEN_PRE);

	//v start lenght keeps no change, if digital mode.

	IoReg_Write32(vgip_hsta_width_reg_addr[dev], dma_vgip_chn1_act_hsta_width_reg.regValue);
	IoReg_Write32(vgip_vsta_length_reg_addr[dev], dma_vgip_chn1_act_vsta_length_reg.regValue);
	IoReg_Write32(vgip_misc_reg_addr[dev], dma_vgip_dma_vgip_misc_reg.regValue);
	IoReg_Write32(vgip_htotal_reg_addr[dev], dma_vgip_dma_vgip_htotal_reg.regValue);

	// IHS/IVS delay
	dma_vgip_chn1_delay_reg.regValue = IoReg_Read32(vgip_delay_reg_addr[dev]);
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_SUPPORT_SRC_ADC
	if((IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE) && (IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_VSC_SOURCE_TYPE) == VSC_INPUTSRC_ADC) && (get_ADC_Input_Source() == _SRC_VGA))
		dma_vgip_chn1_delay_reg.dma_ihs_dly = FIX_VGIP_H_DELAY;
	else
#endif//#ifdef CONFIG_SUPPORT_SRC_ADC
#endif
		dma_vgip_chn1_delay_reg.dma_ihs_dly = 0;
	dma_vgip_chn1_delay_reg.dma_ivs_dly = u32_cal_ivs_dly;	// according liyu chen suggestion
	IoReg_Write32(vgip_delay_reg_addr[dev], dma_vgip_chn1_delay_reg.regValue);

	if(src == VGIP_SRC_ATV)
	{
		dma_vgip_chn1_delay_reg.regValue = IoReg_Read32(vgip_delay_reg_addr[dev]);
		if(VD_27M_OFF != fw_video_get_27mhz_mode((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)))
		{
#ifdef AV_HSYNC_PULSE_START_REFINE
			if((get_ic_version() == VERSION_A)&&(get_AVD_Input_Source() == _SRC_CVBS)) //av
				dma_vgip_chn1_delay_reg.dma_ihs_dly = 0x55;
			else
#endif
				dma_vgip_chn1_delay_reg.dma_ihs_dly = _H_POSITION_MOVE_TOLERANCE * 2;
		}
		else
		{
			dma_vgip_chn1_delay_reg.dma_ihs_dly = _H_POSITION_MOVE_TOLERANCE;
		}

		IoReg_Write32(vgip_delay_reg_addr[dev], dma_vgip_chn1_delay_reg.regValue);

		dma_vgip_chn1_act_hsta_width_reg.regValue = IoReg_Read32(vgip_hsta_width_reg_addr[dev]);
		if(dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta < dma_vgip_chn1_delay_reg.dma_ihs_dly){
			rtd_pr_i3ddma_warn("#####[%s(%d)]prevent overflow, dma_ih_act_sta=%d,dma_ihs_dly=%d\n",__func__,__LINE__,dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta,dma_vgip_chn1_delay_reg.dma_ihs_dly);
			dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta = _H_ACT_STA_PRE_DEF - dma_vgip_chn1_delay_reg.dma_ihs_dly;
		}else
			dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta = dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta - dma_vgip_chn1_delay_reg.dma_ihs_dly;
		IoReg_Write32(vgip_hsta_width_reg_addr[dev], dma_vgip_chn1_act_hsta_width_reg.regValue);
	}
#ifdef CONFIG_SUPPORT_SRC_VGA
	if ((IDDMA_GetVFEInfoByPort(dev, SLR_INPUT_VSC_SOURCE_TYPE) == VSC_INPUTSRC_ADC) && (get_ADC_Input_Source() == _SRC_VGA)){
		Scaler_SetHPosition(Scaler_GetHPosition());
		Scaler_SetVPosition(Scaler_GetVPosition());
	}
#endif
	if((IDDMA_Get_DMAWorkMode(dev) == IDDMA_DISPLAY_MODE) && dvrif_i3ddma_compression_get_enable()) {
		if(src == VGIP_SRC_ATV) {
			dma_vgip_chn1_act_hsta_width_reg.regValue = IoReg_Read32(vgip_hsta_width_reg_addr[dev]);
			/*analog mode enable pqc, if h_start>32 do (h_delay +=32, h_start -=32)*/
			if(dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta > 32) {
				dma_vgip_chn1_delay_reg.regValue = IoReg_Read32(vgip_delay_reg_addr[dev]);
				dma_vgip_chn1_delay_reg.dma_ihs_dly = dma_vgip_chn1_delay_reg.dma_ihs_dly + 32;
				IoReg_Write32(vgip_delay_reg_addr[dev], dma_vgip_chn1_delay_reg.regValue);
				dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta = dma_vgip_chn1_act_hsta_width_reg.dma_ih_act_sta - 32;
				IoReg_Write32(vgip_hsta_width_reg_addr[dev], dma_vgip_chn1_act_hsta_width_reg.regValue);
			}
		}
		else if(isHDMIOrDP) {
			dma_vgip_chn1_delay_reg.regValue = IoReg_Read32(vgip_delay_reg_addr[dev]);
			dma_vgip_chn1_delay_reg.dma_ihs_dly = 32;
			IoReg_Write32(vgip_delay_reg_addr[dev], dma_vgip_chn1_delay_reg.regValue);
		}
	}

	dma_cts_fifo_ctl_reg.regValue = IoReg_Read32(vgip_cts_fifo_ctl_reg_addr[dev]);
	dma_cts_fifo_ctl_reg.dma_en_ctsfifo_vsrst_vgip = 1;
	dma_cts_fifo_ctl_reg.dma_force_ctsfifo_rstn_vgip = 1;
	//dma_cts_fifo_ctl_reg.dma_force_ctsfifo_rstn_sdnr = 1;
	//dma_cts_fifo_ctl_reg.dma_en_ctsfifo_vsrst_sdnr = 0;
	IoReg_Write32(vgip_cts_fifo_ctl_reg_addr[dev], dma_cts_fifo_ctl_reg.regValue);

	return;
}


void fw_scaler_ctrl_dma_vgip_regen_vsync(unsigned char dev, unsigned char enable)
{
	dma_vgip_dma_vgip_regen_vs_ctrl1_RBUS dma_vgip_dma_vgip_regen_vs_ctrl1_reg;
	dma_vgip_dma_vgip_regen_vs_ctrl2_RBUS dma_vgip_dma_vgip_regen_vs_ctrl2_reg;

	unsigned int input_src_htotal = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN);
	unsigned int input_src_vtotal = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN);
	unsigned int input_src_v_act  = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE);

	if(enable) {
		if((input_src_vtotal - input_src_v_act) < FAKE_PORCH_MIN_V_BLANK) {
				rtd_printk(KERN_NOTICE, TAG_NAME_VSC, "#####[%s(%d)]Abnormal timing can not enable fake vsync!\n", __FUNCTION__, __LINE__);
				return;
		}
		Set_Val_DP_Vsync_Regen_Pos(dev, DP_REGEN_VSYNC_AT_DMA_VGIP);
		dma_vgip_dma_vgip_regen_vs_ctrl1_reg.regValue = IoReg_Read32(vgip_regen_vs_ctrl1_reg_addr[dev]);
		dma_vgip_dma_vgip_regen_vs_ctrl1_reg.dma_fake_porch_thr_line = FAKE_PORCH_THR_LINE;
		IoReg_Write32(vgip_regen_vs_ctrl1_reg_addr[dev], dma_vgip_dma_vgip_regen_vs_ctrl1_reg.regValue);

		dma_vgip_dma_vgip_regen_vs_ctrl2_reg.regValue = IoReg_Read32(vgip_regen_vs_ctrl2_reg_addr[dev]);
		dma_vgip_dma_vgip_regen_vs_ctrl2_reg.dma_fake_porch_en = enable;
		dma_vgip_dma_vgip_regen_vs_ctrl2_reg.dma_fake_porch_sel = 1;
		dma_vgip_dma_vgip_regen_vs_ctrl2_reg.dma_fake_vs_width = 2;
		dma_vgip_dma_vgip_regen_vs_ctrl2_reg.dma_fake_porch_line_total = is_scaler_input_2p_mode(dev)?input_src_htotal:input_src_htotal/2;
		IoReg_Write32(vgip_regen_vs_ctrl2_reg_addr[dev], dma_vgip_dma_vgip_regen_vs_ctrl2_reg.regValue);
	} else {
		dma_vgip_dma_vgip_regen_vs_ctrl2_reg.regValue = IoReg_Read32(vgip_regen_vs_ctrl2_reg_addr[dev]);
		dma_vgip_dma_vgip_regen_vs_ctrl2_reg.dma_fake_porch_en = 0;
		dma_vgip_dma_vgip_regen_vs_ctrl2_reg.dma_fake_porch_sel = 0;
		IoReg_Write32(vgip_regen_vs_ctrl2_reg_addr[dev], dma_vgip_dma_vgip_regen_vs_ctrl2_reg.regValue);

		I3DDMA_reset_i3tovo_vs_sel(dev); // reset i3tovo_vs_sel
	}
}

void avd_start_compensation(void)
{//for avd i3ddma start compensation, because final state maybe need compensation
	extern StructDisplayInfo *Get_AVD_ScalerDispinfo(void);
	extern struct semaphore* get_vdc_detectsemaphore(void);
	extern unsigned char get_avd_start_need_compensation(void);
	extern void set_avd_start_need_compensation(unsigned char TorF);
	StructDisplayInfo *avd_info = NULL;
	dma_vgip_dma_vgip_act_hsta_width_RBUS dma_vgip_act_hsta_width_reg;
	dma_vgip_dma_vgip_act_vsta_length_RBUS dma_vgip_act_vsta_length_reg;
	dma_vgip_dma_vgip_delay_RBUS dma_vgip_chn1_delay_reg;
	if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_AVD)
		return;
	if(!get_avd_start_need_compensation())//no need compensation
		return;
	set_avd_start_need_compensation(FALSE);
	dma_vgip_chn1_delay_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_DELAY_reg);
	dma_vgip_act_hsta_width_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_ACT_HSTA_Width_reg);
	dma_vgip_act_vsta_length_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_ACT_VSTA_Length_reg);
	avd_info =  Get_AVD_ScalerDispinfo();
	if((avd_info->IPH_ACT_STA_PRE != (dma_vgip_chn1_delay_reg.dma_ihs_dly + dma_vgip_act_hsta_width_reg.dma_ih_act_sta)) ||
		(avd_info->IPV_ACT_STA_PRE != (dma_vgip_chn1_delay_reg.dma_ivs_dly + dma_vgip_act_vsta_length_reg.dma_iv_act_sta))
		)
	{
#ifndef BUILD_QUICK_SHOW
		down(get_vdc_detectsemaphore());
#endif
		dma_vgip_chn1_delay_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_DELAY_reg);
		dma_vgip_act_hsta_width_reg.regValue = IoReg_Read32(DMA_VGIP_DMA_VGIP_ACT_HSTA_Width_reg);
		if(avd_info->IPH_ACT_STA_PRE < dma_vgip_chn1_delay_reg.dma_ihs_dly){
			rtd_pr_i3ddma_warn("#####[%s(%d)]prevent overflow, IPH_ACT_STA_PRE=%d,dma_ihs_dly=%d\n",__func__,__LINE__,avd_info->IPH_ACT_STA_PRE,dma_vgip_chn1_delay_reg.dma_ihs_dly);
			dma_vgip_act_hsta_width_reg.dma_ih_act_sta = _H_ACT_STA_PRE_DEF - dma_vgip_chn1_delay_reg.dma_ihs_dly;
		}else
			dma_vgip_act_hsta_width_reg.dma_ih_act_sta = avd_info->IPH_ACT_STA_PRE - dma_vgip_chn1_delay_reg.dma_ihs_dly;
		IoReg_Write32(DMA_VGIP_DMA_VGIP_ACT_HSTA_Width_reg, dma_vgip_act_hsta_width_reg.regValue);

		dma_vgip_act_vsta_length_reg.regValue 	= IoReg_Read32(DMA_VGIP_DMA_VGIP_ACT_VSTA_Length_reg);
		if(avd_info->IPV_ACT_STA_PRE < dma_vgip_chn1_delay_reg.dma_ivs_dly){
			rtd_pr_i3ddma_warn("#####[%s(%d)]prevent overflow, IPV_ACT_STA_PRE=%d,dma_ivs_dly=%d\n",__func__,__LINE__,avd_info->IPV_ACT_STA_PRE,dma_vgip_chn1_delay_reg.dma_ivs_dly);
			dma_vgip_act_vsta_length_reg.dma_iv_act_sta = _V_ACT_STA_PRE_DEF - dma_vgip_chn1_delay_reg.dma_ivs_dly;
		}else
			dma_vgip_act_vsta_length_reg.dma_iv_act_sta = avd_info->IPV_ACT_STA_PRE - dma_vgip_chn1_delay_reg.dma_ivs_dly;
		IoReg_Write32(DMA_VGIP_DMA_VGIP_ACT_VSTA_Length_reg, dma_vgip_act_vsta_length_reg.regValue);
#ifndef BUILD_QUICK_SHOW
		up(get_vdc_detectsemaphore());
#endif
		rtd_pr_i3ddma_notice("#####[%s(%d)]run avd_start_compensation####\n",__func__,__LINE__);
	}
}

/*merlin4 new function*/
//Re-order control for HDMI Dolby
void drvif_I3DDMA_set_reorder_mode(unsigned char enable)
{
	h3ddma_i3ddma_enable_RBUS i3ddma_enable_reg;

	i3ddma_enable_reg.regValue = IoReg_Read32(H3DDMA_I3DDMA_enable_reg);
	i3ddma_enable_reg.reorder = (enable) ? 1 : 0;
	IoReg_Write32(H3DDMA_I3DDMA_enable_reg, i3ddma_enable_reg.regValue );
}

