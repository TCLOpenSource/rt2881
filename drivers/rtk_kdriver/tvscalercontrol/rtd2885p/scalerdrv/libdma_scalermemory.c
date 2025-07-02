/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2024
  * All rights reserved.
  * ========================================================================*/

/*================= File Description =======================================*/
/**
 * @file
 * 	This file is for scaler libdma register control.
 *
 * @author 	$Author$
 * @date		$Date$
 * @version	$Revision$
 * @ingroup	clock
 */

/**
 * @addtogroup clock
 * @{
 */

/*============================ Module dependency  ===========================*/
#ifndef BUILD_QUICK_SHOW
#include <linux/delay.h>
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
#include <rtk_kdriver/RPCDriver.h>
#include <linux/pageremap.h>
#include <uapi/linux/const.h>
#include <linux/mm.h>
#include <linux/auth.h>
#include <mach/platform.h>
#include <linux/math64.h>

#endif

#include <tvscalercontrol/scalerdrv/scaler_libdma.h>
#include <rbus/M8P_mdomain_cap_reg.h>
#include <rbus/M8P_mdomain_disp_reg.h>
#include <rbus/M8P_mdom_sub_cap_reg.h>
#include <rbus/M8P_mdom_sub_disp_reg.h>
#include <rbus/scaledown_reg.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/vodma_reg.h>
#include <rbus/timer_reg.h>

#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
#else
	#include <rtk_kdriver/scalercommon/scalerCommon.h>
#endif

#include <scaler/libdma_scalerDrvCommon.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/scaler/scalerstruct.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <tvscalercontrol/scalerdrv/libdma_scalermemory.h>


#ifndef BUILD_QUICK_SHOW
 #include "tvscalercontrol/vdc/video.h"

#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <VideoRPC_System.h>
	#include <VideoRPC_System_data.h>
#else
#include <rpc/VideoRPC_System.h>
#include <rpc/VideoRPC_System_data.h>
#endif

#include <mach/rtk_platform.h>
#include <rtd_log/rtd_module_log.h>
#else
#include <sysdefs.h>
#include <mach/rtk_platform.h>
#include <no_os/semaphore.h>
#include <no_os/spinlock.h>
#include <no_os/spinlock_types.h>
#include <no_os/slab.h>
#include <timer.h>
#include <rtk_kdriver/rmm/rmm.h>
#include <rmm/rmm_common.h>
#include <rtd_log/rtd_module_log.h>
#include <no_os/pageremap.h>
#include "vby1/panel_api.h"
#include <no_os/math64.h>
#endif

#include <tvscalercontrol/scalerdrv/zoom_smoothtoggle.h>
#include <tvscalercontrol/scalerdrv/zoom_smoothtoggle_lib.h>
#include <tvscalercontrol/scalerdrv/mode.h>
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <tvscalercontrol/i3ddma/i3ddma.h>
#include <scaler_vscdev.h>
#include <scaler_vbedev.h>

#include <rtk_dc_mt.h>

// for register dump
#include <tvscalercontrol/io/ioregdrv.h>
#include <rbus/sys_reg_reg.h>

#ifdef BUILD_QUICK_SHOW
#define  PAGE_ALIGN(x)  (x)
#undef  BUG
#define BUG() do { \
	rtd_pr_scaler_memory_info("U-Boot BUG at %s:%d!\n", __FILE__, __LINE__); \
} while (0)
#endif

/*===================================  Types ================================*/
/*================================== Variables ==============================*/
static unsigned int mdomain_main_buffer_size = 0;
static unsigned int mdomain_sub_buffer_size = 0;
static unsigned int mdomain_main_buffer_number = 0;
static unsigned int mdomain_sub_buffer_number = 0;
static unsigned char main_block_mode_enable = FALSE;
static unsigned char sub_block_mode_enable = FALSE;
unsigned long libdma_FilmModeCachaddr = 0;
unsigned long libdma_FilmModePhyadddr = 0;
/*================================ Definitions ==============================*/
#define MAIN_FIFO_GATE_TH   0x400  // DIC suggestion
#define MAIN_FIFO_GATE_TH_1 0x800
#define MAIN_FIFO_GATE_TH_2 0xc00
#define MAIN_FIFO_GATE_TH_3 0xf70
/*================================== Function ===============================*/

unsigned int dvrif_memory_get_buffer_size(unsigned char display)
{
	if(display == SLR_MAIN_DISPLAY)
		return mdomain_main_buffer_size;
	else
		return mdomain_sub_buffer_size;
}

void dvrif_memory_set_buffer_size(unsigned char display, unsigned int buffer_size)
{
	if(display == SLR_MAIN_DISPLAY)
		mdomain_main_buffer_size = buffer_size;
	else
		mdomain_sub_buffer_size = buffer_size;
}

unsigned int dvrif_memory_get_buffer_number(unsigned char display)
{
	if(display == SLR_MAIN_DISPLAY)
		return mdomain_main_buffer_number;
	else
		return mdomain_sub_buffer_number;
}

void dvrif_memory_set_buffer_number(unsigned char display, unsigned int buffer_number)
{
	if(display == SLR_MAIN_DISPLAY)
		mdomain_main_buffer_number = buffer_number;
	else
		mdomain_sub_buffer_number = buffer_number;
}

unsigned char dvrif_memory_get_block_mode_enable(unsigned char display)
{
	if(display == SLR_MAIN_DISPLAY)
		return main_block_mode_enable;
	else
		return sub_block_mode_enable;
}

void dvrif_memory_set_block_mode_enable(unsigned char display, unsigned char enable)
{
	if(display == SLR_MAIN_DISPLAY)
		main_block_mode_enable = enable;
	else
		sub_block_mode_enable = enable;
}

unsigned int memory_get_pixel_size(PixelSizeInfo *pPixelSizeInfo)
{
    unsigned int PixelSize;

    if (pPixelSizeInfo == NULL){
        rtd_pr_scaler_memory_err("pPixelSizeInfo is NULL! -- %s\n", __FUNCTION__);
        return 0;
    }

    if (pPixelSizeInfo->YCMode == MEM_YC_COMBINE){

        if(pPixelSizeInfo->Format == MEM_444){
            if(pPixelSizeInfo->BitNum == MEM_10_BIT) {		// 4:4:4 format     10bit
                PixelSize = 30;
            } else {			// 4:4:4 format     8bit
                PixelSize = 24;
            }
        }else{
            if(pPixelSizeInfo->BitNum == MEM_10_BIT) {		// 4:2:2 format     10bit
                PixelSize = 20;
            } else {			// 4:2:2 format     8bit
                PixelSize = 16;
            }
        }
    } else {
        if(pPixelSizeInfo->BitNum == MEM_10_BIT) {		// yc separate     10bit
            PixelSize = 10;
        } else {			// yc separate     8bit
            PixelSize = 8;
        }
    }

    return PixelSize;
}

unsigned int libdma_memory_get_line_size(unsigned int Width, PixelSizeInfo *pPixelSizeInfo)
{
    unsigned int LineSize;

    LineSize = drvif_memory_get_data_align(Width * memory_get_pixel_size(pPixelSizeInfo), 128);
    LineSize = (unsigned int)SHR(LineSize, 7);

    rtd_pr_scaler_memory_info("LineSize=%d -- %s\n", LineSize, __FUNCTION__);
    return LineSize;
}


/*============================================================================*/
/**
 * libdma_memory_get_capture_size
 * Calculate the memory size of capture. The size is counted by 4:2:2/4:4:4 data format,
 * and frame-access/line access.
 *
 * @param <info> { disp-info structure }
 * @param <AccessType> { Access_Type  }
 * @param <TotalSize> { size of capture }
 * @return { none }
 *
 */
unsigned int libdma_memory_get_capture_size(unsigned char display)
{
    unsigned int TotalSize;
    unsigned int Width;
    unsigned int Length;
    PixelSizeInfo pixelSizeInfo;

    Width = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_WID);
    Length = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_LEN);

    if (dvrif_memory_get_block_mode_enable(display)){
        pixelSizeInfo.YCMode = MEM_YC_SEPARATE;
        pixelSizeInfo.BitNum = MEM_8_BIT;
        pixelSizeInfo.Format = MEM_420;
    } else {
        pixelSizeInfo.YCMode = MEM_YC_COMBINE;
        pixelSizeInfo.BitNum = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_10BIT);
        pixelSizeInfo.Format = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_422CAP);
    }


    TotalSize = libdma_memory_get_line_size(Width, &pixelSizeInfo);


    if (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME)
        TotalSize = TotalSize * Length;	// multiple total_lines if frame-access

    rtd_pr_scaler_memory_emerg("[Mdomain]: TotalSize=%x,capWid=%d,capLen=%d,10Bit=%d,422=%d\n", TotalSize, Width, Length, pixelSizeInfo.BitNum, pixelSizeInfo.Format);

    return TotalSize;
}

unsigned int libdma_memory_get_display_size(unsigned char display)
{
    unsigned int TotalSize;
    unsigned int Width;
    unsigned int Length;
    PixelSizeInfo pixelSizeInfo;

    Width = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID);
    Length = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN);

    if (dvrif_memory_get_block_mode_enable(display)){
        pixelSizeInfo.YCMode = MEM_YC_SEPARATE;
        pixelSizeInfo.BitNum = MEM_8_BIT;
        pixelSizeInfo.Format = MEM_420;
    } else {
        pixelSizeInfo.YCMode = MEM_YC_COMBINE;
        pixelSizeInfo.BitNum = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_10BIT);
        pixelSizeInfo.Format = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_422CAP);
    }


    TotalSize = libdma_memory_get_line_size(Width, &pixelSizeInfo);


    if (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME)
        TotalSize = TotalSize * Length;	// multiple total_lines if frame-access

    rtd_pr_scaler_memory_emerg("[Mdomain]: TotalSize=%x,capWid=%d,capLen=%d,10Bit=%d,422=%d\n", TotalSize, Width, Length, pixelSizeInfo.BitNum, pixelSizeInfo.Format);

    return TotalSize;
}

unsigned int libdma_memory_get_capture_line_size_compression(unsigned char display, unsigned char compression_bits)
{
	unsigned short cmp_line_sum_bit;
    unsigned int capWid = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_WID);


	rtd_pr_scaler_memory_info("[Modmain] compression_bits=%x, capWid=%x\n", compression_bits, capWid);

	//width need to align 32 for new PQC @Crixus 20170615
	if((capWid % 32) != 0)
		capWid = capWid + (32 - (capWid % 32));

    cmp_line_sum_bit = calc_comp_line_sum_bit(capWid, compression_bits);

	rtd_pr_scaler_memory_info("[Modmain] libdma_memory_get_capture_line_size_compression LineSize=%d\n", cmp_line_sum_bit);

	return cmp_line_sum_bit;
}

unsigned int libdma_memory_get_capture_frame_size_compression(unsigned char display, unsigned char compression_bits)
{
	unsigned int TotalSize;
	unsigned int capLen = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_LEN);
	unsigned int capWid = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_WID);

	//rtd_pr_scaler_memory_emerg("[crixus]capWid div32 = %d\n", capWid);
	TotalSize = capWid * compression_bits;
	//rtd_pr_scaler_memory_emerg("[crixus]TotalSize compress = %d\n",TotalSize);
	TotalSize = drvif_memory_get_data_align(TotalSize, 64);// (pixels of per line * bits / 64), unit: 64bits
	TotalSize = TotalSize * capLen;

	//rtd_pr_scaler_memory_emerg("[crixus]libdma_memory_get_line_size LineSize=%d\n",TotalSize);
	//rtd_pr_scaler_memory_emerg("[crixus]:TotalSize=%x,capWid=%x,capLen=%x\n", TotalSize,capWid,capLen);
	return TotalSize;
}

unsigned int libdma_drv_get_caveout_buffer_size_by_platform(unsigned int pixel_bits)
{
	unsigned int width;
	unsigned int len;
	unsigned int buffersize = 0;
    calculate_carveout_size(SLR_MAIN_DISPLAY);
    width = Get_Val_Max_Width();
    len = Get_Val_Max_Len();

    if((width % 32) != 0)
    {
        width = width + (32 - (width % 32));
    }

	buffersize = MDOMAIN_CMP_LINE_MODE_BUFFER_SIZE(width, len, pixel_bits);

	return buffersize;
}

void libdma_mdomain_handler_onlycap(void)
{
	unsigned char display;
	//M8P_mdomain_vi_sub_sub_gctl_RBUS vi_gctl_reg;

	display = Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY);

	set_m_domain_setting_err_status(display, MDOMAIN_SETTING_NO_ERR); // reset M domain setting error status
	
    dvrif_memory_set_block_mode_enable(display, FALSE);
    dvrif_memory_set_cap_mode(display, MEMCAPTYPE_FRAME);
    dvrif_memory_set_buffer_number(display, 3);
    
    memory_frc_main_set_capture();

    /*
	vi_gctl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_VI_SUB_SUB_GCTL_reg);
	if(vi_gctl_reg.vsce1 == 0) {
		vi_gctl_reg.vsce1 = 1;
		IoReg_Write32(M8P_MDOMAIN_VI_SUB_SUB_GCTL_reg, vi_gctl_reg.regValue);
	}
    */

	if(get_m_domain_setting_err_status(display) == MDOMAIN_SETTING_NO_ERR)
		libdma_drvif_memory_EnableMCap(display);
	else
		rtd_pr_scaler_memory_emerg("[%s] %d ERROR! Don't enable Mcap_en. (display=%d, err_status=%d)\n", __FUNCTION__, __LINE__, display, get_m_domain_setting_err_status(display));

}
void libdma_mcap_outstanding_ctrl(unsigned char display, unsigned char enable)
{
	unsigned int cap_outstanding = 0;

	cap_outstanding = enable ? _WDMA_MAX_OUTSTANDING : 0;

	if(display == SLR_MAIN_DISPLAY)
	{
		M8P_mdomain_cap_frc_cap0_wr_ctrl_RBUS mdomain_cap_frc_cap0_wr_ctrl_reg;

		mdomain_cap_frc_cap0_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg);

		if(mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_max_outstanding != cap_outstanding)
		{
			if(mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_enable)
			{
				libdma_drvif_memory_DisableMCap(display);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable main cap_en, before enable main cap_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdomain_cap_frc_cap0_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg);
			mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_max_outstanding = cap_outstanding;//max
			IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg, mdomain_cap_frc_cap0_wr_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set main cap_outstanding %d\n",__FUNCTION__, __LINE__, mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_max_outstanding);
		}
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else
	{//sub
		M8P_mdom_sub_cap_frc_cap1_wr_ctrl_RBUS mdom_sub_cap_frc_cap1_wr_ctrl_reg;

		mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg);

		if(mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_max_outstanding != cap_outstanding)
		{
			if(mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_enable)
			{
				libdma_drvif_memory_DisableMCap(display);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable sub cap_en, before enable sub cap_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg);
			mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_max_outstanding = cap_outstanding;//max
			IoReg_Write32(M8P_MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg, mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set sub cap_outstanding %d\n",__FUNCTION__, __LINE__, mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_max_outstanding);
		}
	}
#endif
}

void mcap_dmac_outstanding_ctrl(unsigned char display, unsigned char enable)
{
	unsigned int cap_outstanding = 0;

	cap_outstanding = enable ? _WDMA_MAX_OUTSTANDING : 0;

	if(display == SLR_MAIN_DISPLAY)
	{
		M8P_mdomain_cap_frc_cap0c_wr_ctrl_RBUS mdomain_cap_frc_cap0c_wr_ctrl_reg;

		mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg);

		if(mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_max_outstanding != cap_outstanding)
		{
			if(mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_enable)
			{
				libdma_drvif_memory_DisableMCap(display);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable main cap_en, before enable main cap_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg);
			mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_max_outstanding = cap_outstanding;//max
			IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg, mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set main cap_outstanding %d\n",__FUNCTION__, __LINE__, mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_max_outstanding);
		}
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else
	{//sub
		M8P_mdom_sub_cap_frc_cap1c_wr_ctrl_RBUS mdom_sub_cap_frc_cap1c_wr_ctrl_reg;

		mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg);

		if(mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_max_outstanding != cap_outstanding)
		{
			if(mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_enable)
			{
				libdma_drvif_memory_DisableMCap(display);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable sub cap_en, before enable sub cap_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg);
			mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_max_outstanding = cap_outstanding;//max
			IoReg_Write32(M8P_MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg, mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set sub cap_outstanding %d\n",__FUNCTION__, __LINE__, mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_max_outstanding);
		}
	}
#endif
}

void libdma_mdisp_outstanding_ctrl(unsigned char display, unsigned char enable)
{
	unsigned int disp_outstanding = 0;

	disp_outstanding = enable ? _RDMA_MAX_OUTSTANDING : 0;

	if(display == SLR_MAIN_DISPLAY)
	{
		M8P_mdomain_disp_frc_disp0_rd_ctrl_RBUS mdomain_disp_frc_disp0_rd_ctrl_reg;

		mdomain_disp_frc_disp0_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg);

		if(mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_max_outstanding != disp_outstanding)
		{
			if(mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_enable)
			{
				libdma_drvif_memory_EnableMDisp_Or_DisableMDisp(display, _DISABLE);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable main disp_en, before enable main disp_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdomain_disp_frc_disp0_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg);
			mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_max_outstanding = disp_outstanding;
			IoReg_Write32(M8P_MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg, mdomain_disp_frc_disp0_rd_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set main disp_outstanding %d\n",__FUNCTION__, __LINE__, mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_max_outstanding);
		}
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else
	{//sub
		M8P_mdom_sub_disp_frc_disp1_rd_ctrl_RBUS mdom_sub_disp_frc_disp1_rd_ctrl_reg;

		mdom_sub_disp_frc_disp1_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg);

		if(mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_max_outstanding != disp_outstanding)
		{
			if(mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_enable)
			{
				libdma_drvif_memory_EnableMDisp_Or_DisableMDisp(display, _DISABLE);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable sub disp_en, before enable sub disp_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdom_sub_disp_frc_disp1_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg);
			mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_max_outstanding = disp_outstanding;
			IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg, mdom_sub_disp_frc_disp1_rd_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set sub disp_outstanding %d\n",__FUNCTION__, __LINE__, mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_max_outstanding);
		}
	}
#endif
}

void mdisp_dmac_outstanding_ctrl(unsigned char display, unsigned char enable)
{
	unsigned int disp_outstanding = 0;

	disp_outstanding = enable ? _RDMA_MAX_OUTSTANDING : 0;

	if(display == SLR_MAIN_DISPLAY)
	{
		M8P_mdomain_disp_frc_disp0c_rd_ctrl_RBUS mdomain_disp_frc_disp0c_rd_ctrl_reg;

		mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg);

		if(mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_max_outstanding != disp_outstanding)
		{
			if(mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_enable)
			{
				libdma_drvif_memory_EnableMDisp_Or_DisableMDisp(display, _DISABLE);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable main disp_en, before enable main disp_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg);
			mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_max_outstanding = disp_outstanding;
			IoReg_Write32(M8P_MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg, mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set main disp_outstanding %d\n",__FUNCTION__, __LINE__, mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_max_outstanding);
		}
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else
	{//sub
		M8P_mdom_sub_disp_frc_disp1c_rd_ctrl_RBUS mdom_sub_disp_frc_disp1c_rd_ctrl_reg;

		mdom_sub_disp_frc_disp1c_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_reg);

		if(mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_max_outstanding != disp_outstanding)
		{
			if(mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_dma_enable)
			{
				libdma_drvif_memory_EnableMDisp_Or_DisableMDisp(display, _DISABLE);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable sub disp_en, before enable sub disp_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdom_sub_disp_frc_disp1c_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_reg);
			mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_max_outstanding = disp_outstanding;
			IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_reg, mdom_sub_disp_frc_disp1c_rd_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set sub disp_outstanding %d\n",__FUNCTION__, __LINE__, mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_max_outstanding);
		}
	}
#endif
}

void libdma_drvif_memory_set_dbuffer_write(unsigned char display)
{
	// write data & apply double buffer
	if(display == SLR_MAIN_DISPLAY)
		IoReg_SetBits(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg, M8P_MDOMAIN_DISP_Disp0_db_ctrl_disp0_db_apply_mask);
	else if(display == SLR_SUB_DISPLAY)
		IoReg_SetBits(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg, M8P_MDOM_SUB_DISP_Disp1_db_ctrl_disp1_db_apply_mask);
}

//USER:LewisLee DATE:2012/08/23
//to maje sure have set correct setting
//to prevent M domain setting error with video FW
void libdma_dvrif_memory_set_frc_style(UINT8 Display, UINT8 ucEnable)
{
	M8P_mdomain_cap_cap0_pxl_wrap_ctrl_0_RBUS mdomain_cap_cap0_pxl_wrap_ctrl_0_reg;

	if(SLR_MAIN_DISPLAY == Display)
	{
		mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg);

		if(_ENABLE == ucEnable)
		{
			if(_DISABLE == mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en)
			{
				mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en = _ENABLE;
				IoReg_Write32(M8P_MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg, mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue);

				libdma_drvif_memory_set_dbuffer_write(Display);
				rtd_pr_scaler_memory_debug("libdma_dvrif_memory_set_frc_style, set input fast than display\n");
			}
		}
		else// if(_DISABLE == ucEnable)
		{
			if(_ENABLE == mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en)
			{
				mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en = _DISABLE;
				IoReg_Write32(M8P_MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg, mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue);

				libdma_drvif_memory_set_dbuffer_write(Display);
				rtd_pr_scaler_memory_debug("libdma_dvrif_memory_set_frc_style, set input slower than display\n");
			}
		}
	}
}

//128bit unit
unsigned int calc_comp_line_sum_bit(unsigned int comp_wid, unsigned int comp_ratio)
{
	unsigned short cmp_line_sum_bit, sub_pixel_num;
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

	rtd_pr_scaler_memory_info("cmp_line_sum_bit=0x%x function=%s, line=%d\n", cmp_line_sum_bit, __FUNCTION__, __LINE__);

	return cmp_line_sum_bit;
}

void libdma_dvrif_memory_comp_setting(UINT8 enable_compression, UINT16 comp_wid, UINT16 comp_len, UINT8 comp_ratio){
	//M-domain use PQC and PQDC from merlin3 @Crixus 20170515
	M8P_mdomain_cap_dispm0_pq_cmp_RBUS mdomain_cap_dispm0_pq_cmp_reg;
	M8P_mdomain_cap_dispm0_pq_cmp_pair_RBUS mdomain_cap_dispm0_pq_cmp_pair_reg;
	M8P_mdomain_cap_dispm0_pq_cmp_bit_RBUS mdomain_cap_dispm0_pq_cmp_bit_reg;
	M8P_mdomain_cap_dispm0_pq_cmp_enable_RBUS mdomain_cap_dispm0_pq_cmp_enable_reg;
	M8P_mdomain_cap_dispm0_pq_cmp_allocate_RBUS mdomain_cap_dispm0_pq_cmp_allocate_reg;
	M8P_mdomain_cap_dispm0_pq_cmp_poor_RBUS mdomain_cap_dispm0_pq_cmp_poor_reg;
	M8P_mdomain_cap_dispm0_pq_cmp_blk0_add0_RBUS mdomain_cap_dispm0_pq_cmp_blk0_add0_reg;
	M8P_mdomain_cap_dispm0_pq_cmp_blk0_add1_RBUS mdomain_cap_dispm0_pq_cmp_blk0_add1_reg;
	M8P_mdomain_cap_dispm0_pq_cmp_balance_RBUS mdomain_cap_dispm0_pq_cmp_balance_reg;
	M8P_mdomain_cap_dispm0_pq_cmp_smooth_RBUS mdomain_cap_dispm0_pq_cmp_smooth_reg;
	M8P_mdomain_cap_dispm0_pq_cmp_guarantee_RBUS mdomain_cap_dispm0_pq_cmp_guarantee_reg;
	M8P_mdomain_cap_dispm0_pq_cmp_bit_llb_RBUS mdomain_cap_dispm0_pq_cmp_bit_llb_reg;
	M8P_mdomain_cap_dispm0_pq_cmp_st_RBUS mdomain_cap_dispm0_pq_cmp_st_reg;

	M8P_mdomain_disp_disp0_pq_decmp_RBUS mdomain_disp_disp0_pq_decmp_reg;
	M8P_mdomain_disp_disp0_pq_decmp_pair_RBUS mdomain_disp_disp0_pq_decmp_pair_reg;
	M8P_mdomain_disp_disp0_pq_decmp_sat_en_RBUS mdomain_disp_disp0_pq_decmp_sat_en_reg;
	//M8P_mdomain_disp_ddr_mainctrl_RBUS mdomain_disp_ddr_mainctrl_reg;
	//M8P_mdomain_cap_smooth_tog_ctrl_0_RBUS mdomain_cap_smooth_tog_ctrl_0_reg;
	//unsigned int TotalSize = 0;
	extern unsigned char pc_mode_run_422(void);
	extern unsigned char pc_mode_run_444(void);
	unsigned short cmp_line_sum_bit;
	unsigned int cmp_width_div32 = 0, frame_limit_bit = 0;
	unsigned int llb_tolerance = 0, llb_x_blk_sta = 0, Llb_init = 0, Llb_step = 0;

	rtd_pr_scaler_memory_debug("[crixus]enable_compression = %d, comp_wid = %d, comp_len = %d, comp_ratio = %d\n",enable_compression, comp_wid, comp_len, comp_ratio);

	if(enable_compression == TRUE){

		// Enable PQC clk
		//mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);
		//mdomain_cap_smooth_tog_ctrl_0_reg.in1_pqc_clken = 0;  // 0: enable
		//IoReg_Write32(M8P_MDOMAIN_CAP_Smooth_tog_ctrl_0_reg, mdomain_cap_smooth_tog_ctrl_0_reg.regValue);

		/*
			width need to align 32

			line sum bit = (width * compression bit + 256) / 128

			for new PQC and PQDC @Crixus 20170615
		*/

		if((comp_wid % 32) != 0){
			//drop bits
			//IoReg_Write32(M8P_MDOMAIN_DISP_DDR_MainAddrDropBits_reg, (32 - (comp_wid % 32)));
			comp_wid = comp_wid + (32 - (comp_wid % 32));
		}


		#if 0
		if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == COMPRESSION_LINE_MODE){
			TotalSize = (comp_wid * comp_ratio + 256) * 10 / 128;
			if((TotalSize % 10) != 0){
				TotalSize = TotalSize / 10 + 1;
			}else{
				TotalSize = TotalSize / 10;
			}
		}
		#endif
		//rtd_pr_scaler_memory_emerg("[crixus]comp_wid = %d, comp_wid/32 = %d\n", comp_wid, (comp_wid/32));
		//rtd_pr_scaler_memory_emerg("[crixus]comp_len = %d\n", comp_len);
		//rtd_pr_scaler_memory_emerg("[crixus]TotalSize = %d\n", TotalSize);


		/*=======================PQC Setting======================================*/
		cmp_width_div32 = comp_wid / 32;
		frame_limit_bit = comp_ratio << 2;
		//capture compression setting
		mdomain_cap_dispm0_pq_cmp_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_reg);
		mdomain_cap_dispm0_pq_cmp_reg.cmp_height = comp_len;//set length
		mdomain_cap_dispm0_pq_cmp_reg.cmp_width_div32 = comp_wid / 32;//set width
		IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_reg, mdomain_cap_dispm0_pq_cmp_reg.regValue);

		//compression bits setting
		mdomain_cap_dispm0_pq_cmp_bit_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BIT_reg);
        mdomain_cap_dispm0_pq_cmp_bit_reg.frame_limit_bit = comp_ratio*4;
		mdomain_cap_dispm0_pq_cmp_bit_reg.block_limit_bit = 0x3f;//PQC fine tune setting
		mdomain_cap_dispm0_pq_cmp_bit_reg.first_line_more_bit = 0x20;
		IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BIT_reg, mdomain_cap_dispm0_pq_cmp_bit_reg.regValue);

		//compression other setting
		mdomain_cap_dispm0_pq_cmp_pair_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_PAIR_reg);
		#if 0
		if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_10BIT) == TRUE)
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_bit_width = 1; //10 bits
		else
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_bit_width = 0;//8 bits
		#endif

		mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_bit_width = 1; //10 bits



		if(get_vpq_check_pc_rgb444_mode_flag() == _ENABLE)
		{
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_color = 0; // RGB , pure RGB case
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 0; // 444 , pure RGB case
		}
		else
		{
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_color = 1; // YUV
			if ((Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP) == FALSE))
			{
				if(pc_mode_run_422())
				{
					mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 1; // 422
				}
				else if(pc_mode_run_444())
				{
					mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 0; // 444
				}
				else
				{
					mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 1; // 422
				}
			}
			else
			{
				mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 0; // 422
			}
		}
		mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_pair_para = 1;

#if 0
		//if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP) == TRUE){
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 0;// 444, let hw dynamic switch 444 or 422.
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_color = 1;
		//}else{
		//	mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 0;//444
		//	mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_color = 1;//currently no case rgb channel
		//}
#endif
		//Cal line sum bit
		cmp_line_sum_bit = calc_comp_line_sum_bit(comp_wid, comp_ratio);


		// Both line mode & frame mode need to set cmp_line_sum_bit;
		mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_line_sum_bit = cmp_line_sum_bit;

		if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == COMPRESSION_LINE_MODE){
			//use line mode
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_line_mode = 1;
			//mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_line_sum_bit = TotalSize;
		}
		else{
			//use frame mode
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_line_mode = 0;
			//mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_line_sum_bit = 0;
		}

	if(get_hdmi_vrr_4k60_mode()
#ifndef BUILD_QUICK_SHOW
        || get_mdomain_vflip_flag(SLR_MAIN_DISPLAY)
#endif
        )
	{
		mdomain_cap_dispm0_pq_cmp_pair_reg.two_line_prediction_en = 0;
	}
	else
	{
		mdomain_cap_dispm0_pq_cmp_pair_reg.two_line_prediction_en = 1;
	}
	mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_qp_mode = 0;
	//mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_dic_mode_en = 1;//Mark2 removed
	mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_jump4_en = 1;
	mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_jump6_en = 1;
	IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_PAIR_reg, mdomain_cap_dispm0_pq_cmp_pair_reg.regValue);

	//compression llb setting
	llb_tolerance = 6;
	llb_x_blk_sta = 0;
	Llb_init = frame_limit_bit * 4 + llb_tolerance * 32;
	Llb_step = (Llb_init *16 - frame_limit_bit * 64) / cmp_width_div32;
	mdomain_cap_dispm0_pq_cmp_bit_llb_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BIT_LLB_reg);
	mdomain_cap_dispm0_pq_cmp_bit_llb_reg.llb_x_blk_sta = llb_x_blk_sta;
	mdomain_cap_dispm0_pq_cmp_bit_llb_reg.llb_init = Llb_init;
	mdomain_cap_dispm0_pq_cmp_bit_llb_reg.llb_step = Llb_step;
	IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BIT_LLB_reg, mdomain_cap_dispm0_pq_cmp_bit_llb_reg.regValue);

	//compression st setting
	mdomain_cap_dispm0_pq_cmp_st_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_ST_reg);
	//mdomain_cap_dispm0_pq_cmp_st_reg.cmp_readro_sel = //default;
	IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_ST_reg, mdomain_cap_dispm0_pq_cmp_st_reg.regValue);

	//compression PQC constrain @Crixus 20170626
	mdomain_cap_dispm0_pq_cmp_enable_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_ENABLE_reg);
	if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == COMPRESSION_LINE_MODE){
		mdomain_cap_dispm0_pq_cmp_enable_reg.first_line_more_en = 0;
	}
	else{
		mdomain_cap_dispm0_pq_cmp_enable_reg.first_line_more_en = 1;
	}
	//RGB444 setting for PQC @Crixus 20170718
	rtd_pr_scaler_memory_info("SLR_DISP_422CAP=%x, pc_mode_run_422=%x, pc_mode_run_444=%x\n",Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP), pc_mode_run_422(), pc_mode_run_444());
	if ((Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP) == FALSE)){
		if(pc_mode_run_422())
		{
			mdomain_cap_dispm0_pq_cmp_enable_reg.g_ratio = 16;
			mdomain_cap_dispm0_pq_cmp_enable_reg.do_422_mode = 1;//422 format
		}
		else if(pc_mode_run_444())
		{
			mdomain_cap_dispm0_pq_cmp_enable_reg.g_ratio = 12;
			mdomain_cap_dispm0_pq_cmp_enable_reg.do_422_mode = 2;//444 format
		}
		else
		{
			mdomain_cap_dispm0_pq_cmp_enable_reg.g_ratio = 16;
			mdomain_cap_dispm0_pq_cmp_enable_reg.do_422_mode = 1;//422 format
		}
	}
	else{
		mdomain_cap_dispm0_pq_cmp_enable_reg.g_ratio = 14;
		mdomain_cap_dispm0_pq_cmp_enable_reg.do_422_mode = 0;//422 format
	}

		mdomain_cap_dispm0_pq_cmp_enable_reg.first_predict_nc_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.blk0_add_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.blk0_add_half_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.add_last2blk_over_curve_bit = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.balance_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.variation_maxmin_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.dynamic_allocate_ratio_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.dynamic_sum_line_rst = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.not_rich_do_422_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.rb_lossy_do_422_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.not_enough_bit_do_422_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.poor_limit_min_qp_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.first_predict_nc_mode = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.overcurve_lossy_en = 1;
		//mdomain_cap_dispm0_pq_cmp_enable_reg.master_qp_en = 1;//fix work around need to check by dic
		IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_ENABLE_reg, mdomain_cap_dispm0_pq_cmp_enable_reg.regValue);

		mdomain_cap_dispm0_pq_cmp_allocate_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_ALLOCATE_reg);
		//RGB444 setting for PQC @Crixus 20170718
		if ((Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP) == FALSE)){
			if(pc_mode_run_422())
			{
				mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
				mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
			}
			else if(pc_mode_run_444())
			{
				mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 13;
				mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 9;
			}
			else
			{
				mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
				mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
			}
		}
		else{
			mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
			mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
		}

		mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_less = 4;
		mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_more = 2;
		mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_line = 2;
		IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_ALLOCATE_reg, mdomain_cap_dispm0_pq_cmp_allocate_reg.regValue);

		mdomain_cap_dispm0_pq_cmp_poor_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_POOR_reg);
		#if 0
		if(pc_mode_run_444()){
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp4=0x14;
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp3=0x06;
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp2=0x03;
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp1=0x03;
		} else {
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp4=0x14;
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp3=0x0c;
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp2=0x06;
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp1=0x03;
		}

	if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC) && (!get_vsc_run_adaptive_stream(SLR_MAIN_DISPLAY)))
	{
		mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp4=0x13; // for issue KTASKWBS-10111 for normal VDEC
	}
	#endif
	mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp4=12;
	mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp3=9;
	mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp2=4;
	mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp1=3;

	IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_POOR_reg, mdomain_cap_dispm0_pq_cmp_poor_reg.regValue);

	//Update PQC constrain setting for PQC @Crixus 20170725
	mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BLK0_ADD0_reg);
	mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BLK0_ADD1_reg);
	if(comp_ratio == COMPRESSION_10_BITS){// minmal compression bit setting
		mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.blk0_add_value32 = 1;
		mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.blk0_add_value16 = 1;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value8 = 1;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value4 = 1;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value2 = 1;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value1 = 0;
		//mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 0;
	}
	else{
		mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.blk0_add_value32 = 16;
		mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.blk0_add_value16 = 8;
		mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.blk0_add_value0 = 0;
		//mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 0;

		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value8 = 4;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value4 = 2;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value2 = 1;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value1 = 1;
	}
	IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BLK0_ADD0_reg, mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.regValue);
	IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BLK0_ADD1_reg, mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.regValue);


		mdomain_cap_dispm0_pq_cmp_balance_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BALANCE_reg);
		mdomain_cap_dispm0_pq_cmp_balance_reg.balance_rb_ov_th = 0;
		mdomain_cap_dispm0_pq_cmp_balance_reg.balance_rb_ud_th = 3;
		mdomain_cap_dispm0_pq_cmp_balance_reg.balance_rb_give = 3;
		mdomain_cap_dispm0_pq_cmp_balance_reg.balance_g_ud_th = 3;
		mdomain_cap_dispm0_pq_cmp_balance_reg.balance_g_ov_th = 0;
		mdomain_cap_dispm0_pq_cmp_balance_reg.balance_g_give = 3;
		mdomain_cap_dispm0_pq_cmp_balance_reg.variation_maxmin_th = 30;
		mdomain_cap_dispm0_pq_cmp_balance_reg.variation_maxmin_th2 = 3;
		IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BALANCE_reg, mdomain_cap_dispm0_pq_cmp_balance_reg.regValue);


		mdomain_cap_dispm0_pq_cmp_smooth_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_SMOOTH_reg);
		mdomain_cap_dispm0_pq_cmp_smooth_reg.variation_near_num_th = 4;
		mdomain_cap_dispm0_pq_cmp_smooth_reg.variation_value_th = 3;
		mdomain_cap_dispm0_pq_cmp_smooth_reg.variation_num_th = 3;
		mdomain_cap_dispm0_pq_cmp_smooth_reg.not_enough_bit_do_422_qp = 5;
		mdomain_cap_dispm0_pq_cmp_smooth_reg.not_rich_do_422_th = 4;
		mdomain_cap_dispm0_pq_cmp_smooth_reg.rb_lossy_do_422_qp_level = 0;
		//mdomain_cap_dispm0_pq_cmp_smooth_reg.dic_mode_entry_th = 15;//Mark2 removed

		IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_SMOOTH_reg, mdomain_cap_dispm0_pq_cmp_smooth_reg.regValue);

		mdomain_cap_dispm0_pq_cmp_guarantee_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_Guarantee_reg);
		//mdomain_cap_dispm0_pq_cmp_guarantee_reg.dic_mode_qp_th_g = 1;//Mark2 removed
		//mdomain_cap_dispm0_pq_cmp_guarantee_reg.dic_mode_qp_th_rb = 1;//Mark2 removed
		IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_Guarantee_reg, mdomain_cap_dispm0_pq_cmp_guarantee_reg.regValue);


	/*=======================PQDC Setting======================================*/

    if (get_hdmi_vrr_4k60_mode()) {
        comp_len=comp_len/2;
    }
	//display de-compression setting
	mdomain_disp_disp0_pq_decmp_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DISP0_PQ_DECMP_reg);
	mdomain_disp_disp0_pq_decmp_reg.decmp_height = comp_len;
	mdomain_disp_disp0_pq_decmp_reg.decmp_width_div32 = comp_wid / 32;
	IoReg_Write32(M8P_MDOMAIN_DISP_DISP0_PQ_DECMP_reg, mdomain_disp_disp0_pq_decmp_reg.regValue);

	//de-compression bits setting
	mdomain_disp_disp0_pq_decmp_pair_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DISP0_PQ_DECMP_PAIR_reg);
	#if 0
	if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_10BIT) == TRUE)
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_bit_width = 1;//10 bits
	else
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_bit_width = 0;//8 bits
	#endif

	mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_bit_width = 1;//10 bits

	#if 0
	//decompression YUV444 @Crixus 20160902
	//if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP) == TRUE){
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 0;//444
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_color = 1;
	//}else{
	//	mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 0;//444
	//	mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_color = 1;
	//}
	#endif


	if(get_vpq_check_pc_rgb444_mode_flag() == _ENABLE)
	{
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_color = 0; // RGB , pure RGB case
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 0; // 444 , pure RGB case
	}
	else
	{
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_color = 1; // YUV
		if ((Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP) == FALSE))
		{
			if(pc_mode_run_422())
			{
				mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 1; // 422
			}
			else if(pc_mode_run_444())
			{
				mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 0; // 444
			}
			else
			{
				mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 1; // 422
			}
		}
		else
		{
			mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 0; // 422
		}
	}

		// Both line mode & frame mode need to set cmp_line_sum_bit;
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_line_sum_bit = cmp_line_sum_bit;

		if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == COMPRESSION_LINE_MODE){
			//default use line mode
			mdomain_disp_disp0_pq_decmp_pair_reg.decmp_line_mode = 1;
			//mdomain_disp_disp0_pq_decmp_pair_reg.decmp_line_sum_bit = TotalSize;
		}
		else{
			//use frame mode
			mdomain_disp_disp0_pq_decmp_pair_reg.decmp_line_mode = 0;
			//mdomain_disp_disp0_pq_decmp_pair_reg.decmp_line_sum_bit = 0;
		}

	if(get_hdmi_vrr_4k60_mode()
#ifndef BUILD_QUICK_SHOW
        || get_mdomain_vflip_flag(SLR_MAIN_DISPLAY)
#endif
        )
	{
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_two_line_prediction_en = 0;
	} else
	{
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_two_line_prediction_en = 1;
	}
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_qp_mode = 0;
		//mdomain_disp_disp0_pq_decmp_pair_reg.decmp_dic_mode_en = 1; //Mark2 removed
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_jump4_en = 1;
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_jump6_en = 1;

		IoReg_Write32(M8P_MDOMAIN_DISP_DISP0_PQ_DECMP_PAIR_reg, mdomain_disp_disp0_pq_decmp_pair_reg.regValue);

		//enable saturation for PQ
		mdomain_disp_disp0_pq_decmp_sat_en_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DISP0_PQ_DECMP_SAT_EN_reg);
		mdomain_disp_disp0_pq_decmp_sat_en_reg.saturation_en = 1;
		//mdomain_disp_disp0_pq_decmp_sat_en_reg.decmp_ctrl_para_1 = //reserved;
		//mdomain_disp_disp0_pq_decmp_sat_en_reg.decmp_ctrl_para_0 = //reserved;
		mdomain_disp_disp0_pq_decmp_sat_en_reg.saturation_type = 0;
		IoReg_Write32(M8P_MDOMAIN_DISP_DISP0_PQ_DECMP_SAT_EN_reg, mdomain_disp_disp0_pq_decmp_sat_en_reg.regValue);

		//enable PQDC clock
		//mdomain_disp_ddr_mainctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DDR_MainCtrl_reg);
		//mdomain_disp_ddr_mainctrl_reg.main_decompression_clk_en = 1;
		//IoReg_Write32(M8P_MDOMAIN_DISP_DDR_MainCtrl_reg, mdomain_disp_ddr_mainctrl_reg.regValue);

		//enable compression
		mdomain_cap_dispm0_pq_cmp_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_reg);
		mdomain_cap_dispm0_pq_cmp_reg.cmp_en = 1;
		IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_reg, mdomain_cap_dispm0_pq_cmp_reg.regValue);

		//enable de-compression
		mdomain_disp_disp0_pq_decmp_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DISP0_PQ_DECMP_reg);
		mdomain_disp_disp0_pq_decmp_reg.decmp_en = 1;
		IoReg_Write32(M8P_MDOMAIN_DISP_DISP0_PQ_DECMP_reg, mdomain_disp_disp0_pq_decmp_reg.regValue);
	}
	else{
		//disable compression
		mdomain_cap_dispm0_pq_cmp_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_reg);
		mdomain_cap_dispm0_pq_cmp_reg.cmp_en = 0;
		IoReg_Write32(M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_reg, mdomain_cap_dispm0_pq_cmp_reg.regValue);

		//disable PQC clk
		//mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);
		//mdomain_cap_smooth_tog_ctrl_0_reg.in1_pqc_clken = 1; // 1: disable
		//IoReg_Write32(M8P_MDOMAIN_CAP_Smooth_tog_ctrl_0_reg, mdomain_cap_smooth_tog_ctrl_0_reg.regValue);

		//disable de-compression
		mdomain_disp_disp0_pq_decmp_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DISP0_PQ_DECMP_reg);
		mdomain_disp_disp0_pq_decmp_reg.decmp_en = 0;
		IoReg_Write32(M8P_MDOMAIN_DISP_DISP0_PQ_DECMP_reg, mdomain_disp_disp0_pq_decmp_reg.regValue);

		//disable PQDC clock
		//mdomain_disp_ddr_mainctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DDR_MainCtrl_reg);
		//mdomain_disp_ddr_mainctrl_reg.main_decompression_clk_en = 0;
		//IoReg_Write32(M8P_MDOMAIN_DISP_DDR_MainCtrl_reg, mdomain_disp_ddr_mainctrl_reg.regValue);
	}
}


void libdma_dvrif_memory_comp_setting_sub(UINT8 enable_compression, UINT16 comp_wid, UINT16 comp_len, UINT8 comp_ratio){

	M8P_mdom_sub_cap_dispm1_pq_cmp_RBUS mdom_sub_cap_dispm1_pq_cmp_reg;
	M8P_mdom_sub_cap_dispm1_pq_cmp_pair_RBUS mdom_sub_cap_dispm1_pq_cmp_pair_reg;
	M8P_mdom_sub_cap_dispm1_pq_cmp_bit_RBUS mdom_sub_cap_dispm1_pq_cmp_bit_reg;
	M8P_mdom_sub_cap_dispm1_pq_cmp_enable_RBUS mdom_sub_cap_dispm1_pq_cmp_enable_reg;
	M8P_mdom_sub_cap_dispm1_pq_cmp_allocate_RBUS mdom_sub_cap_dispm1_pq_cmp_allocate_reg;
	M8P_mdom_sub_cap_dispm1_pq_cmp_poor_RBUS mdom_sub_cap_dispm1_pq_cmp_poor_reg;
	M8P_mdom_sub_cap_dispm1_pq_cmp_blk0_add0_RBUS mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg;
	M8P_mdom_sub_cap_dispm1_pq_cmp_blk0_add1_RBUS mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg;
	M8P_mdom_sub_cap_dispm1_pq_cmp_balance_RBUS mdom_sub_cap_dispm1_pq_cmp_balance_reg;
	M8P_mdom_sub_cap_dispm1_pq_cmp_smooth_RBUS mdom_sub_cap_dispm1_pq_cmp_smooth_reg;
	M8P_mdom_sub_cap_dispm1_pq_cmp_guarantee_RBUS mdom_sub_cap_dispm1_pq_cmp_guarantee_reg;

	M8P_mdom_sub_disp_disp1_pq_decmp_RBUS mdom_sub_disp_disp1_pq_decmp_reg;
	M8P_mdom_sub_disp_disp1_pq_decmp_pair_RBUS mdom_sub_disp_disp1_pq_decmp_pair_reg;
	M8P_mdom_sub_disp_disp1_pq_decmp_sat_en_RBUS mdom_sub_disp_disp1_pq_decmp_sat_en_reg;
	//M8P_mdomain_disp_ddr_subctrl_RBUS mdomain_disp_ddr_subctrl_reg;

	//unsigned int TotalSize = 0;
    unsigned char s_cmp_alpha_en = 0;
    unsigned short s_cmp_line_sum_bit, s_sub_pixel_num;
	unsigned int s_cmp_line_sum_bit_pure_rgb, s_cmp_line_sum_bit_pure_a, s_cmp_line_sum_bit_32_dummy, s_cmp_line_sum_bit_128_dummy, s_cmp_line_sum_bit_real;
	extern unsigned char pc_mode_run_422(void);
	extern unsigned char pc_mode_run_444(void);
	unsigned int cmp_width_div32 = 0, frame_limit_bit = 0;

	rtd_pr_scaler_memory_emerg("[crixus]enable_compression = %d, comp_wid = %d, comp_len = %d, comp_ratio = %d\n",enable_compression, comp_wid, comp_len, comp_ratio);

	if(enable_compression == TRUE){

		// Enable PQC clk
		//mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);
		//mdomain_cap_smooth_tog_ctrl_0_reg.in2_pqc_clken = 0; // 0: enable
		//IoReg_Write32(M8P_MDOMAIN_CAP_Smooth_tog_ctrl_0_reg, mdomain_cap_smooth_tog_ctrl_0_reg.regValue);

		/*
			width need to align 32

			line sum bit = (width * compression bit + 256) / 128

			for new PQC and PQDC @Crixus 20170615
		*/

		if((comp_wid % 32) != 0){
			//drop bits
			//IoReg_Write32(M8P_MDOMAIN_DISP_DDR_SubAddrDropBits_reg, (32 - (comp_wid % 32)));
			comp_wid = comp_wid + (32 - (comp_wid % 32));
		}


		//if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_LINE_MODE){
		//	TotalSize = (comp_wid * comp_ratio + 256) * 10 / 128;
		//	if((TotalSize % 10) != 0){
		//		TotalSize = TotalSize / 10 + 1;
		//	}else{
		//		TotalSize = TotalSize / 10;
		//	}
		//}
		//rtd_pr_scaler_memory_emerg("[crixus]comp_wid = %d, comp_wid/32 = %d\n", comp_wid, (comp_wid/32));
		//rtd_pr_scaler_memory_emerg("[crixus]comp_len = %d\n", comp_len);
		//rtd_pr_scaler_memory_emerg("[crixus]TotalSize = %d\n", TotalSize);


		/*=======================PQC Setting======================================*/
		cmp_width_div32 = comp_wid / 32;
		frame_limit_bit = comp_ratio << 2;
		//capture compression setting
		mdom_sub_cap_dispm1_pq_cmp_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_reg);
		mdom_sub_cap_dispm1_pq_cmp_reg.cmp_height = comp_len;//set length
		mdom_sub_cap_dispm1_pq_cmp_reg.cmp_width_div32 = comp_wid / 32;//set width
		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_reg, mdom_sub_cap_dispm1_pq_cmp_reg.regValue);

		//compression bits setting
		mdom_sub_cap_dispm1_pq_cmp_bit_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_BIT_reg);
		if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_LINE_MODE){
			mdom_sub_cap_dispm1_pq_cmp_bit_reg.frame_limit_bit = comp_ratio*4;
			//mdom_sub_cap_dispm1_pq_cmp_bit_reg.line_limit_bit = comp_ratio;//Mark2 removed
		}
		else{//frame mode, line limit bit need to add margin to avoid the peak bandwidth
			mdom_sub_cap_dispm1_pq_cmp_bit_reg.frame_limit_bit = comp_ratio*4;
			//mdom_sub_cap_dispm1_pq_cmp_bit_reg.line_limit_bit = comp_ratio + M_DOMAIN_PQC_LINE_MARGIN;
		}
		mdom_sub_cap_dispm1_pq_cmp_bit_reg.block_limit_bit = 0x3f;//PQC fine tune setting
		mdom_sub_cap_dispm1_pq_cmp_bit_reg.first_line_more_bit = 0x20;
		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_BIT_reg, mdom_sub_cap_dispm1_pq_cmp_bit_reg.regValue);

		//compression other setting
		mdom_sub_cap_dispm1_pq_cmp_pair_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_PAIR_reg);
		#if 0
		if(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_10BIT) == TRUE)
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_bit_width = 1; //10 bits
		else
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_bit_width = 0;//8 bits
		#endif
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_bit_width = 1; //10 bits

        mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_color = 1;		
		if(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) == FALSE){
			if(pc_mode_run_422())
			{
				mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_format = 1; // 422
			}
			else if(pc_mode_run_444())
			{
				mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_format = 0; // 444
			}
			else
			{
				mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_format = 1; // 422
			}
		}else{
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_format = 1; // 422
		}
		
        //Cal line sum bit
		s_sub_pixel_num = (s_cmp_alpha_en)? 4 : 3;
	    s_cmp_line_sum_bit_pure_rgb = (cmp_width_div32 * 32) * (frame_limit_bit>>2);
		s_cmp_line_sum_bit_pure_a = (s_cmp_alpha_en)?((cmp_width_div32 * 32)* (frame_limit_bit>>2)):(0);
		s_cmp_line_sum_bit_32_dummy = (6+1) * 32 * s_sub_pixel_num;
		s_cmp_line_sum_bit_128_dummy = 0;
		s_cmp_line_sum_bit_real = s_cmp_line_sum_bit_pure_rgb + s_cmp_line_sum_bit_pure_a + s_cmp_line_sum_bit_32_dummy + s_cmp_line_sum_bit_128_dummy;
		s_cmp_line_sum_bit = ((s_cmp_line_sum_bit_real+128)>>8)*2;//ceil((cmp_line_sum_bit_real)/128)*2;

		rtd_pr_scaler_memory_info("s_sub_pixel_num=%x, s_cmp_line_sum_bit_pure_rgb=%x, s_cmp_line_sum_bit_pure_a=%x, s_cmp_line_sum_bit_32_dummy=%x, s_cmp_line_sum_bit_128_dummy=%x\n", s_sub_pixel_num, s_cmp_line_sum_bit_pure_rgb, s_cmp_line_sum_bit_pure_a, s_cmp_line_sum_bit_32_dummy, s_cmp_line_sum_bit_128_dummy);
		rtd_pr_scaler_memory_info("s_cmp_line_sum_bit_real=%x\n", s_cmp_line_sum_bit_real);
		rtd_pr_scaler_memory_info("s_cmp_line_sum_bit=%x\n", s_cmp_line_sum_bit);

        // Both line mode & frame mode need to set cmp_line_sum_bit;
        mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_line_sum_bit = s_cmp_line_sum_bit;
		if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_LINE_MODE){
			//use line mode
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_line_mode = 1;
			//mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_line_sum_bit = TotalSize;
		}
		else{
			//use frame mode
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_line_mode = 0;
			//mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_line_sum_bit = 0;
		}

		if(get_hdmi_vrr_4k60_mode()
#ifndef BUILD_QUICK_SHOW
            || get_mdomain_vflip_flag(SLR_SUB_DISPLAY)
#endif
            )
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.two_line_prediction_en = 0;
		else
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.two_line_prediction_en = 1;

		mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_qp_mode = 0;
		//mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_dic_mode_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_jump4_en = 1;
		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_PAIR_reg, mdom_sub_cap_dispm1_pq_cmp_pair_reg.regValue);

		//compression PQC constrain @Crixus 20170626
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_ENABLE_reg);
		if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_LINE_MODE){
			mdom_sub_cap_dispm1_pq_cmp_enable_reg.fisrt_line_more_en = 0;
		}
		else{
			mdom_sub_cap_dispm1_pq_cmp_enable_reg.fisrt_line_more_en = 1;
		}
		//RGB444 setting for PQC @Crixus 20170718
		if ((Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) == FALSE)){
			if(pc_mode_run_422())
			{
				mdom_sub_cap_dispm1_pq_cmp_enable_reg.g_ratio = 14;
				mdom_sub_cap_dispm1_pq_cmp_enable_reg.do_422_mode = 1;//422 format
			}
			else if(pc_mode_run_444())
			{
				mdom_sub_cap_dispm1_pq_cmp_enable_reg.g_ratio = 12;
				mdom_sub_cap_dispm1_pq_cmp_enable_reg.do_422_mode = 2;//444 format
			}
			else
			{
				mdom_sub_cap_dispm1_pq_cmp_enable_reg.g_ratio = 14;
				mdom_sub_cap_dispm1_pq_cmp_enable_reg.do_422_mode = 1;//422 format
			}
		}
		else{
			mdom_sub_cap_dispm1_pq_cmp_enable_reg.g_ratio = 14;
			mdom_sub_cap_dispm1_pq_cmp_enable_reg.do_422_mode = 1;//dynamic format
		}

		mdom_sub_cap_dispm1_pq_cmp_enable_reg.first_predict_nc_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.blk0_add_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.blk0_add_half_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.add_last2blk_over_curve_bit = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.balance_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.variation_maxmin_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.dynamic_allocate_ratio_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.dynamic_sum_line_rst = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.not_rich_do_422_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.rb_lossy_do_422_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.not_enough_bit_do_422_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.poor_limit_min_qp_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.first_predict_nc_mode = 1;
		//mdom_sub_cap_dispm1_pq_cmp_enable_reg.master_qp_en = 1;//fix work around need to check by dic
		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_ENABLE_reg, mdom_sub_cap_dispm1_pq_cmp_enable_reg.regValue);

		mdom_sub_cap_dispm1_pq_cmp_allocate_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_ALLOCATE_reg);
		//RGB444 setting for PQC @Crixus 20170718
		if ((Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) == FALSE)){
			if(pc_mode_run_422())
			{
				mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
				mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
			}
			else if(pc_mode_run_444())
			{
				mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 13;
				mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 9;
			}
			else
			{
				mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
				mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
			}
		}
		else{
			mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
			mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
		}
		mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_less = 4;
		mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_more = 2;
		mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_line = 2;

		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_ALLOCATE_reg, mdom_sub_cap_dispm1_pq_cmp_allocate_reg.regValue);

		mdom_sub_cap_dispm1_pq_cmp_poor_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_POOR_reg);
		#if 0
		if(pc_mode_run_444()){
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp4=0x14;
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp3=0x06;
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp2=0x03;
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp1=0x03;
		} else {
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp4=0x14;
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp3=0x0c;
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp2=0x06;
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp1=0x03;
		}
		if((Get_DisplayMode_Src(SLR_SUB_DISPLAY) == VSC_INPUTSRC_VDEC) && (!get_vsc_run_adaptive_stream(SLR_SUB_DISPLAY)))
		{
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp4=0x13; // for issue KTASKWBS-10111 for normal VDEC
		}
		#endif

		mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp4=12;
		mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp3=9;
		mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp2=4;
		mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp1=3;

		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_POOR_reg, mdom_sub_cap_dispm1_pq_cmp_poor_reg.regValue);

		//Update PQC constrain setting for PQC @Crixus 20170725
		mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_BLK0_ADD0_reg);
		mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_BLK0_ADD1_reg);
		if(comp_ratio == COMPRESSION_10_BITS){// minmal compression bit setting
			mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.blk0_add_value32 = 1;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.blk0_add_value16 = 1;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value8 = 1;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value4 = 1;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value2 = 1;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value1 = 0;
			//mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 0;
		}
		else{
			mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.blk0_add_value32 = 16;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.blk0_add_value16 = 8;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.blk0_add_value0 = 0;
			//mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 0;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value8 = 4;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value4 = 2;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value2 = 1;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value1 = 1;
		}
		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_BLK0_ADD0_reg, mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.regValue);
		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_BLK0_ADD1_reg, mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.regValue);

		mdom_sub_cap_dispm1_pq_cmp_balance_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_BALANCE_reg);
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.balance_rb_ov_th = 0;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.balance_rb_ud_th = 3;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.balance_rb_give = 3;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.balance_g_ud_th = 3;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.balance_g_ov_th = 0;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.balance_g_give = 3;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.variation_maxmin_th = 30;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.variation_maxmin_th2 = 3;
		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_BALANCE_reg, mdom_sub_cap_dispm1_pq_cmp_balance_reg.regValue);


		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_SMOOTH_reg);
		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.variation_near_num_th = 4;
		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.variation_value_th = 3;
		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.variation_num_th = 3;
		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.not_enough_bit_do_422_qp = 5;
		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.not_rich_do_422_th = 4;
		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.rb_lossy_do_422_qp_level = 0;
		//mdom_sub_cap_dispm1_pq_cmp_smooth_reg.dic_mode_entry_th = 15;

		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_SMOOTH_reg, mdom_sub_cap_dispm1_pq_cmp_smooth_reg.regValue);

		mdom_sub_cap_dispm1_pq_cmp_guarantee_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_Guarantee_reg);
		//mdom_sub_cap_dispm1_pq_cmp_guarantee_reg.dic_mode_qp_th_g = 1;
		//mdom_sub_cap_dispm1_pq_cmp_guarantee_reg.dic_mode_qp_th_rb = 1;
		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_Guarantee_reg, mdom_sub_cap_dispm1_pq_cmp_guarantee_reg.regValue);

		/*=======================Sub PQDC Setting======================================*/

		//display de-compression setting
		mdom_sub_disp_disp1_pq_decmp_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_DISP1_PQ_DECMP_reg);
		mdom_sub_disp_disp1_pq_decmp_reg.decmp_height = comp_len;
		mdom_sub_disp_disp1_pq_decmp_reg.decmp_width_div32 = comp_wid / 32;
		IoReg_Write32(M8P_MDOM_SUB_DISP_DISP1_PQ_DECMP_reg, mdom_sub_disp_disp1_pq_decmp_reg.regValue);

		//de-compression bits setting
		mdom_sub_disp_disp1_pq_decmp_pair_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_DISP1_PQ_DECMP_PAIR_reg);
		#if 0
		if(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_10BIT) == TRUE)
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_bit_width = 1;//10 bits
		else
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_bit_width = 0;//8 bits
		#endif
		mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_bit_width = 1;//10 bits

		#if 0
		//decompression YUV444 @Crixus 20160902
		//if(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) == TRUE){
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_format = 0;//444
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_color = 1;
		/*}else{
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_format = 0;//444
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_color = 1;
		}*/
		#endif



		mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_color = 1; // YUV
		if(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) == FALSE)
		{
			if(pc_mode_run_422())
			{
				mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_format = 1; // 422
			}
			else if(pc_mode_run_444())
			{
				mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_format = 0; // 444
			}
			else
			{
				mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_format = 1; // 422
			}
		}
		else
		{
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_format = 0; // 422
		}
		


        mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_line_sum_bit = s_cmp_line_sum_bit;
		if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_LINE_MODE){
			//default use line mode
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_line_mode = 1;
			//mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_line_sum_bit = TotalSize;
		}
		else{
			//use frame mode
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_line_mode = 0;
			//mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_line_sum_bit = 0;
		}

		if(get_hdmi_vrr_4k60_mode()
#ifndef BUILD_QUICK_SHOW
            || get_mdomain_vflip_flag(SLR_SUB_DISPLAY)
#endif
            )
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_two_line_prediction_en = 0;
		else
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_two_line_prediction_en = 1;

		mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_qp_mode = 0;
		//mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_dic_mode_en = 1;
		mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_jump4_en = 1;
		IoReg_Write32(M8P_MDOM_SUB_DISP_DISP1_PQ_DECMP_PAIR_reg, mdom_sub_disp_disp1_pq_decmp_pair_reg.regValue);

		//enable saturation for PQ
		mdom_sub_disp_disp1_pq_decmp_sat_en_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_DISP1_PQ_DECMP_SAT_EN_reg);
		mdom_sub_disp_disp1_pq_decmp_sat_en_reg.saturation_en = 1;
		mdom_sub_disp_disp1_pq_decmp_sat_en_reg.saturation_type = 0;
		IoReg_Write32(M8P_MDOM_SUB_DISP_DISP1_PQ_DECMP_SAT_EN_reg, mdom_sub_disp_disp1_pq_decmp_sat_en_reg.regValue);

		//enable clock
		//mdomain_disp_ddr_subctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DDR_SubCtrl_reg);
		//mdomain_disp_ddr_subctrl_reg.sub_decompression_clk_en = 1;
		//IoReg_Write32(M8P_MDOMAIN_DISP_DDR_SubCtrl_reg, mdomain_disp_ddr_subctrl_reg.regValue);

		//enable compression
		mdom_sub_cap_dispm1_pq_cmp_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_reg);
		mdom_sub_cap_dispm1_pq_cmp_reg.cmp_en = 1;
		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_reg, mdom_sub_cap_dispm1_pq_cmp_reg.regValue);

		//enable de-compression
		mdom_sub_disp_disp1_pq_decmp_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_DISP1_PQ_DECMP_reg);
		mdom_sub_disp_disp1_pq_decmp_reg.decmp_en = 1;
		IoReg_Write32(M8P_MDOM_SUB_DISP_DISP1_PQ_DECMP_reg, mdom_sub_disp_disp1_pq_decmp_reg.regValue);

	}
	else{
		//disable compression & de-compression
		//disable compression
		mdom_sub_cap_dispm1_pq_cmp_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_reg);
		mdom_sub_cap_dispm1_pq_cmp_reg.cmp_en = 0;
		IoReg_Write32(M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_reg, mdom_sub_cap_dispm1_pq_cmp_reg.regValue);

		// Disable PQC clk
		//mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);
		//mdomain_cap_smooth_tog_ctrl_0_reg.in2_pqc_clken = 1; // 1: disable
		//IoReg_Write32(M8P_MDOMAIN_CAP_Smooth_tog_ctrl_0_reg, mdomain_cap_smooth_tog_ctrl_0_reg.regValue);


		//disable de-compression
		mdom_sub_disp_disp1_pq_decmp_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_DISP1_PQ_DECMP_reg);
		mdom_sub_disp_disp1_pq_decmp_reg.decmp_en = 0;
		IoReg_Write32(M8P_MDOM_SUB_DISP_DISP1_PQ_DECMP_reg, mdom_sub_disp_disp1_pq_decmp_reg.regValue);

		//disable clock
//		mdomain_disp_ddr_subctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DDR_SubCtrl_reg);
//		mdomain_disp_ddr_subctrl_reg.sub_decompression_clk_en = 0;
//		IoReg_Write32(M8P_MDOMAIN_DISP_DDR_SubCtrl_reg, mdomain_disp_ddr_subctrl_reg.regValue);
	}
}

unsigned int libdma_mdomain_input_fast_flag[MAX_DISP_CHANNEL_NUM] = {0};
void libdma_check_and_set_mdomain_input_fast(unsigned char display)
{
    libdma_mdomain_input_fast_flag[display] = 0;
#ifndef BUILD_QUICK_SHOW
    if(display == SLR_MAIN_DISPLAY)
    {
        // Input source frame rate faster than display (main_frc_style = 1)
        // M-display IVS refer to front of VGIP signal (for HDMI FP 3D, IVS refer to the source v-sync)
        if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) > ( Get_DISPLAY_REFRESH_RATE() * 1000 + 500 ) ) && (Get_vscFilmMode() == FALSE) )
        {
            libdma_mdomain_input_fast_flag[display] = 1;
            rtd_pr_scaler_memory_info("main input fast case\n");
        }
    
        if(Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_FRAMESYNC) == FALSE)
        {
            if(Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_ACTIVE)
            {
                libdma_drv_memory_display_check_sub_frc_style();
                if(libdma_mdomain_input_fast_flag[SLR_SUB_DISPLAY])
                    libdma_drv_memory_display_set_input_fast(SLR_SUB_DISPLAY, TRUE);
                else
                    libdma_drv_memory_display_set_input_fast(SLR_SUB_DISPLAY, FALSE);
            }
        }
    }
    else
    {
        libdma_drv_memory_display_check_sub_frc_style();
    }
#endif

    if((FALSE ==Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_FRAMESYNC)) && (libdma_mdomain_input_fast_flag[display]))
    {
        libdma_drv_memory_display_set_input_fast(display, TRUE);
        libdma_mdomain_input_fast_flag[display] = 0;
    }
    else
    {
        libdma_drv_memory_display_set_input_fast(display, FALSE);
    }
}

bool libdma_get_mdomain_input_fast_status(unsigned char display)
{
	bool input_fast_flag = FALSE;

	if(display == SLR_MAIN_DISPLAY)
	{
		input_fast_flag = M8P_MDOMAIN_CAP_Cap0_buf_control_get_cap0_input_fast(IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg));
	}
	else if(display == SLR_SUB_DISPLAY)
	{
		input_fast_flag = M8P_MDOM_SUB_CAP_Cap1_buf_control_get_cap1_input_fast(IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg));
	}

	rtd_pr_scaler_memory_info("[%s] %d display=%d, input_fast=%d\n", __FUNCTION__, __LINE__, display, input_fast_flag);

	return input_fast_flag;
}

void libdma_drv_memory_wait_disp_last_write_done(unsigned char display, unsigned char wait_frame, unsigned char game_mode_case)
{
	//game_mode_case means call by game mode flow
	unsigned int count;
	unsigned char pre_disp_idx;
	unsigned char cur_disp_idx;

	if(display == SLR_MAIN_DISPLAY)
	{
		do
		{
			if((0 == M8P_MDOMAIN_DISP_FRC_disp0_buffer_select_control_get_disp0_buf_num(IoReg_Read32(M8P_MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg))) || (1 == M8P_MDOMAIN_DISP_FRC_disp0_buffer_select_control_get_disp0_buf_force_en(IoReg_Read32(M8P_MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg))))
			{
				rtd_pr_scaler_memory_info("[%s(%d)] single buffer no need wait block status\n", __FUNCTION__, __LINE__);
				return;
			}
			if(game_mode_case && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE))
			{
				rtd_pr_scaler_memory_info("[%s] %d game mode case not active return\n", __FUNCTION__, __LINE__);
				return;
			}
			count = 40;
			pre_disp_idx = M8P_MDOMAIN_DISP_disp0_buf_status_0_get_disp0_buf_index_cur(IoReg_Read32(M8P_MDOMAIN_DISP_disp0_buf_status_0_reg));
			//rtd_pr_scaler_memory_info("[%s] %d M domain main done-0, disp pre idx = %d\n", __FUNCTION__, __LINE__, pre_disp_idx);

			while((M8P_MDOMAIN_DISP_Disp0_db_ctrl_get_disp0_db_apply(IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg))) && --count)
			{
#ifdef BUILD_QUICK_SHOW
				msleep(10);//no busy delay 10 ms
#else
				msleep(0);//no busy delay 10 ms
#endif
			}

			cur_disp_idx = M8P_MDOMAIN_DISP_disp0_buf_status_0_get_disp0_buf_index_cur(IoReg_Read32(M8P_MDOMAIN_DISP_disp0_buf_status_0_reg));
			rtd_pr_scaler_memory_info("[%s] %d M domain main done-1, disp pre_index=%d, new_idx=%d, count=%d, mdisp_apply=%d\n", __FUNCTION__, __LINE__, pre_disp_idx, cur_disp_idx, count, (M8P_MDOMAIN_DISP_Disp0_db_ctrl_get_disp0_db_apply(IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg))));

			if(count == 0)
			{
#ifdef BUILD_QUICK_SHOW
				printf2("M domain main timeout-1 !\n");
#else
				rtd_pr_scaler_memory_err("[%s] %d M domain main timeout-1 !\n", __FUNCTION__, __LINE__);
#endif
			}

			// [KTASKWBS-5316] wait M-display write done if display block index == 0
			// if(MDOMAIN_DISP_DDR_MainStatus_get_main_block_select(IoReg_Read32(M8P_MDOMAIN_DISP_DDR_MainStatus_reg)) == pre_disp_idx)
			if(cur_disp_idx == pre_disp_idx)
			{
				count = 40;

				while((M8P_MDOMAIN_DISP_disp0_buf_status_0_get_disp0_buf_index_cur(IoReg_Read32(M8P_MDOMAIN_DISP_disp0_buf_status_0_reg)) == pre_disp_idx) && --count)
				{
					if(game_mode_case && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE))
					{
						rtd_pr_scaler_memory_info("[%s] %d game mode case not active return\n", __FUNCTION__, __LINE__);
						return;
					}
#ifdef BUILD_QUICK_SHOW
					msleep(10);//no busy delay 10 ms
#else
					msleep(0);//no busy delay 10 ms
#endif
				}
				cur_disp_idx = M8P_MDOMAIN_DISP_disp0_buf_status_0_get_disp0_buf_index_cur(IoReg_Read32(M8P_MDOMAIN_DISP_disp0_buf_status_0_reg));
				rtd_pr_scaler_memory_info("[%s] %d M domain main done-2(check again), disp new_idx=%d, count=%d\n", __FUNCTION__, __LINE__, cur_disp_idx, count);
				if(count == 0)
				{
#ifdef BUILD_QUICK_SHOW
					printf2("M domain main timeout-2 !\n");
#else
					rtd_pr_scaler_memory_err("[%s] %d M domain main timeout-2 !\n", __FUNCTION__, __LINE__);
#endif
				}
			}
			wait_frame--;
		}while(wait_frame != 0);
	}
	else
	{
		do
		{
			if ((0 == M8P_MDOM_SUB_DISP_FRC_disp1_buffer_select_control_get_disp1_buf_num(IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg))) || (1 == M8P_MDOM_SUB_DISP_FRC_disp1_buffer_select_control_get_disp1_buf_force_en(IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg))))
			{
				rtd_pr_scaler_memory_info("[%s(%d)] single buffer no need wait block status\n", __FUNCTION__, __LINE__);
				return;
			}

			count = 40;
			pre_disp_idx = M8P_MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(M8P_MDOM_SUB_DISP_disp1_buf_status_0_reg));

			while((M8P_MDOM_SUB_DISP_Disp1_db_ctrl_get_disp1_db_apply(IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg))) && --count)
			{
#ifdef BUILD_QUICK_SHOW
				msleep(10);//no busy delay 10 ms
#else
				msleep(0);//no busy delay 10 ms
#endif
			}

			cur_disp_idx = M8P_MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(M8P_MDOM_SUB_DISP_disp1_buf_status_0_reg));
			rtd_pr_scaler_memory_info("[%s] %d M domain sub done-1, disp pre_index=%d, new_idx=%d, count=%d, mdisp_apply=%d\n", __FUNCTION__, __LINE__, pre_disp_idx, cur_disp_idx, count, (M8P_MDOM_SUB_DISP_Disp1_db_ctrl_get_disp1_db_apply(IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg))));

			if(count == 0)
				rtd_pr_scaler_memory_err("[%s] %d M domain sub timeout-1 !\n", __FUNCTION__, __LINE__);

			// [KTASKWBS-5316] wait M-display write done if display block index == 0
			// if(MDOMAIN_DISP_DDR_SubStatus_get_sub_block_select(IoReg_Read32(M8P_MDOMAIN_DISP_DDR_SubStatus_reg)) == pre_disp_idx)
			if(cur_disp_idx == pre_disp_idx)
			{
				count = 40;

				while((M8P_MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(M8P_MDOM_SUB_DISP_disp1_buf_status_0_reg)) == pre_disp_idx) && --count)
				{
#ifdef BUILD_QUICK_SHOW
					msleep(10);//no busy delay 10 ms
#else
					msleep(0);//no busy delay 10 ms
#endif
				}
				cur_disp_idx = M8P_MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(M8P_MDOM_SUB_DISP_disp1_buf_status_0_reg));
				rtd_pr_scaler_memory_info("[%s] %d M domain sub done-2(check again), disp new_idx=%d, count=%d\n", __FUNCTION__, __LINE__, cur_disp_idx, count);

				if(count == 0)
				{
#ifdef BUILD_QUICK_SHOW
					printf2("M domain sub timeout-2 !\n");
#else
					rtd_pr_scaler_memory_err("[%s] %d M domain sub timeout-2 !\n", __FUNCTION__, __LINE__);
#endif
				}
			}
			wait_frame--;
		}while(wait_frame != 0);
	}

}

void libdma_drv_memory_wait_cap_last_write_done(unsigned char display, unsigned char wait_frame, unsigned char game_mode_case)
{
	//game_mode_case means call by game mode flow
	M8P_mdomain_cap_cap0_ddr_fifostatus_RBUS mdomain_cap_cap0_ddr_fifostatus_reg;
    M8P_mdom_sub_cap_cap1_ddr_fifostatus_RBUS mdom_sub_cap_cap1_ddr_fifostatus_reg;
	unsigned int count;
	unsigned char pre_cap_idx;
	unsigned char cur_cap_idx;

	//Add input fast case. When input dast case, need to wait Mdisp write done. (Not wait Mcap write done)
	if(libdma_get_mdomain_input_fast_status(display) == TRUE)
	{
		rtd_pr_scaler_memory_info("[%s] %d input fast case, wait disp last write done\n", __FUNCTION__, __LINE__);
        //input fast case need wait less than 2 frame
        if(wait_frame < 2)
        {
            wait_frame = 2;
        }

		libdma_drv_memory_wait_disp_last_write_done(display, wait_frame, game_mode_case);
		return;
	}

	if(display == SLR_MAIN_DISPLAY)
	{
		do
		{
			if(0 == M8P_MDOMAIN_CAP_Cap0_buf_control_get_cap0_buf_num(IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg)))
			{
				rtd_pr_scaler_memory_info("[%s(%d)] single buffer no need wait block status\n", __FUNCTION__, __LINE__);
				return;
			}
			if(game_mode_case && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE))
			{
				rtd_pr_scaler_memory_info("[%s] %d game mode case not active return\n", __FUNCTION__, __LINE__);
				return;
			}
			count = 40;
			while(count && (M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_get_cap0_first_wr_flag(IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg)) == 0))
			{
#ifdef BUILD_QUICK_SHOW
				msleep(10);//no busy delay 10 ms
#else
				msleep(0);//no busy delay 10 ms
#endif
				count --;
			}

			pre_cap_idx = M8P_MDOMAIN_DISP_disp0_buf_status_0_get_cap0_buf_index_cur(IoReg_Read32(M8P_MDOMAIN_DISP_disp0_buf_status_0_reg));

			rtd_pr_scaler_memory_info("[%s] %d main wait first write finish count=%d, pre_cap_idx=%d \n", __FUNCTION__, __LINE__, count, pre_cap_idx);

			count = 40;
			//rtd_pr_scaler_memory_info("[%s] %d M domain main done-0, cap pre idx = %d\n", __FUNCTION__, __LINE__, pre_cap_idx);

			/*check memory cap down*/
			IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg, IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg));

			while((M8P_MDOMAIN_DISP_Disp0_db_ctrl_get_disp0_db_en(IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg))) && (M8P_MDOMAIN_DISP_Disp0_db_ctrl_get_disp0_db_apply(IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg))) && --count)
			{
                if ((M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_get_cap0_dma_enable(IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg)) == 0) && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE))
                {
                    rtd_pr_scaler_memory_notice("%s m cap is disable, return!", __FUNCTION__);
                    return;
                }
#ifdef BUILD_QUICK_SHOW
				msleep(10);//no busy delay 10 ms
#else
				msleep(0);//no busy delay 10 ms
#endif
			}

			cur_cap_idx = M8P_MDOMAIN_DISP_disp0_buf_status_0_get_cap0_buf_index_cur(IoReg_Read32(M8P_MDOMAIN_DISP_disp0_buf_status_0_reg));
			rtd_pr_scaler_memory_info("[%s] %d M domain main done-1, cap pre_index=%d, new_id x=%d, count=%d, mdisp_db_apply=%d,mdisp_db_en=%d\n", __FUNCTION__, __LINE__, pre_cap_idx, cur_cap_idx, count, (M8P_MDOMAIN_DISP_Disp0_db_ctrl_get_disp0_db_apply(IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg))),(M8P_MDOMAIN_DISP_Disp0_db_ctrl_get_disp0_db_en(IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg))));

			if(count == 0)
			{
#ifdef BUILD_QUICK_SHOW
				printf2("M domain main timeout-1 !\n");
#else
				rtd_pr_scaler_memory_err("[%s] %d M domain main timeout-1 !\n", __FUNCTION__, __LINE__);
#endif
			}

			// [KTASKWBS-5316] wait M-capture write done if capure block index == 0
			if(cur_cap_idx == pre_cap_idx)
			{
				count = 40;
				/*check memory cap down*/
				mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
				mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end = 1;
				IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg, mdomain_cap_cap0_ddr_fifostatus_reg.regValue);

				while((M8P_MDOMAIN_DISP_disp0_buf_status_0_get_cap0_buf_index_cur(IoReg_Read32(M8P_MDOMAIN_DISP_disp0_buf_status_0_reg)) == pre_cap_idx) && --count)
				{
					if(game_mode_case && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE))
					{
						rtd_pr_scaler_memory_info("[%s] %d game mode case not active return\n", __FUNCTION__, __LINE__);
						return;
					}
                    if ((M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_get_cap0_dma_enable(IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg)) == 0) && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE))
                    {
                        rtd_pr_scaler_memory_notice("%s m cap is disable, return!", __FUNCTION__);
                        return;
                    }
#ifdef BUILD_QUICK_SHOW
					msleep(10);//no busy delay 10 ms
#else
					msleep(0);//no busy delay 10 ms
#endif
				}
				cur_cap_idx = M8P_MDOMAIN_DISP_disp0_buf_status_0_get_cap0_buf_index_cur(IoReg_Read32(M8P_MDOMAIN_DISP_disp0_buf_status_0_reg));
				rtd_pr_scaler_memory_info("[%s] %d M domain main done-2(check again), cap new_idx=%d, count=%d\n", __FUNCTION__, __LINE__, cur_cap_idx, count);
				if(count == 0)
				{
#ifdef BUILD_QUICK_SHOW
					printf2("M domain main timeout-2 !\n");
#else
					rtd_pr_scaler_memory_err("[%s] %d M domain main timeout-2 !\n", __FUNCTION__, __LINE__);
#endif
				}
			}
			wait_frame--;

			//if wait_frame > 0, clear again
			if(wait_frame > 0)
			{
                mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
				mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end = 1;
				IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg, mdomain_cap_cap0_ddr_fifostatus_reg.regValue);
			}
		}while(wait_frame != 0);
	}
	else
	{
		do
		{
			if(0 == M8P_MDOM_SUB_CAP_Cap1_buf_control_get_cap1_buf_num(IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg)))
			{
				rtd_pr_scaler_memory_info("[%s(%d)] single buffer no need wait block status\n", __FUNCTION__, __LINE__);
				return;
			}

			count = 40;
			while(count && (M8P_MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_get_cap1_first_wr_flag(IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg)) == 0))
			{
				msleep(0);//no busy delay 10 ms
				count --;
			}
			pre_cap_idx = M8P_MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(M8P_MDOM_SUB_DISP_disp1_buf_status_0_reg));

			rtd_pr_scaler_memory_info("[%s] %d sub wait first write finish count=%d, pre_cap_idx=%d \n", __FUNCTION__, __LINE__, count, pre_cap_idx);

			count = 40;

			/*check memory cap down*/
			//clear last write flag before wait cap index jump
			mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg);
			mdom_sub_cap_cap1_ddr_fifostatus_reg.cap1_dma_frame_end = 1;
			IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg, mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue);

			while((M8P_MDOM_SUB_DISP_Disp1_db_ctrl_get_disp1_db_apply(IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg))) && --count)
			{
#ifdef BUILD_QUICK_SHOW
				msleep(10);//no busy delay 10 ms
#else
				msleep(0);//no busy delay 10 ms
#endif
			}

			cur_cap_idx = M8P_MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(M8P_MDOM_SUB_DISP_disp1_buf_status_0_reg));
			rtd_pr_scaler_memory_info("[%s] %d M domain sub done-1, cap pre_index=%d, new_idx=%d, count=%d, mdisp_apply=%d\n", __FUNCTION__, __LINE__, pre_cap_idx, cur_cap_idx, count, (M8P_MDOM_SUB_DISP_Disp1_db_ctrl_get_disp1_db_apply(IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg))));

			if(count == 0)
				rtd_pr_scaler_memory_err("[%s] %d M domain sub timeout-1 !\n", __FUNCTION__, __LINE__);

			// [KTASKWBS-5316] wait M-capture write done if capure block index == 0
			// if(MDOMAIN_CAP_DDR_In2Status_get_in2_block_select(IoReg_Read32(M8P_MDOMAIN_CAP_DDR_In2Status_reg)) == pre_cap_idx)
			if(cur_cap_idx == pre_cap_idx)
			{
				count = 40;
				/*check memory cap down*/
                mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg);
                mdom_sub_cap_cap1_ddr_fifostatus_reg.cap1_dma_frame_end = 1;
                IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg, mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue);

				while((M8P_MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(M8P_MDOM_SUB_DISP_disp1_buf_status_0_reg)) == pre_cap_idx) && --count)
				{
#ifdef BUILD_QUICK_SHOW
					msleep(10);//no busy delay 10 ms
#else
					msleep(0);//no busy delay 10 ms
#endif
				}
				cur_cap_idx = M8P_MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(M8P_MDOM_SUB_DISP_disp1_buf_status_0_reg));
				rtd_pr_scaler_memory_info("[%s] %d M domain sub done-2(check again), cap new_idx=%d, count=%d\n", __FUNCTION__, __LINE__, cur_cap_idx, count);

				if(count == 0)
				{
#ifdef BUILD_QUICK_SHOW
					printf2("M domain sub timeout-2 !\n");
#else
					rtd_pr_scaler_memory_err("[%s] %d M domain sub timeout-2 !\n", __FUNCTION__, __LINE__);
#endif
				}
			}
			wait_frame--;
		}while(wait_frame != 0);
	}
}

void libdma_wait_m_cap_last_write_done(unsigned char display)
{
#ifndef BUILD_QUICK_SHOW
	if (display == SLR_MAIN_DISPLAY)
	{
		/* add main double buffer apply @Crixus 20160126 */
		imd_smooth_main_double_buffer_all_apply();
		if ((Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC) == FALSE))
		{
			/* Update the M-domain last done @Crixus 20161013 */
			if(drv_memory_Get_multibuffer_flag())
                libdma_drv_memory_wait_cap_last_write_done(SLR_MAIN_DISPLAY, drv_memory_Get_multibuffer_number()-1, FALSE);
            else if(drv_memory_Get_multibuffer_changed())
                libdma_drv_memory_wait_cap_last_write_done(SLR_MAIN_DISPLAY, drv_memory_Get_multibuffer_number_pre()-1, FALSE);
			else
				libdma_drv_memory_wait_cap_last_write_done(SLR_MAIN_DISPLAY, 1, FALSE);
		}
	}
	else
	{
		/* add sub double buffer apply @Crixus 20160126 */
		imd_smooth_sub_double_buffer_all_apply();

		if(Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC) == FALSE)
		{
			if (!get_sub_OutputVencMode())
			{/* sub m domain iterrupt open so don't need wait lastwrite @qing_liu --s4ap */
				/* Update the M-domain last done @Crixus 20161013 */
				libdma_drv_memory_wait_cap_last_write_done(SLR_SUB_DISPLAY, 1, FALSE);
			}
		}
	}
#endif
}

//==============Gamd mode==============//


extern unsigned char Get_Factory_SelfDiagnosis_Mode(void);
extern unsigned char get_AVD_Input_Source(void);

extern unsigned char get_vo_camera_flow_flag(unsigned char ch);
void libdma_drv_memory_gamemode_sub_switch_single_buffer(unsigned char enable)
{//enable: true = single   false: triple  4k120 case
    unsigned int timeout = 0x3ffff;
    M8P_mdom_sub_cap_cap1_reg_doublbuffer_RBUS mdom_sub_cap_cap1_reg_doublbuffer_reg;
    M8P_mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;
    M8P_mdom_sub_cap_cap1_buf_control_RBUS mdom_sub_cap_cap1_buf_control_reg;
    M8P_mdom_sub_disp_frc_disp1_buffer_select_control_RBUS mdom_sub_disp_frc_disp1_buffer_select_control_reg;
    M8P_mdom_sub_cap_cap1_ddr_fifostatus_RBUS mdom_sub_cap_cap1_ddr_fifostatus_reg;
    M8P_mdom_sub_cap_cap1_buffer_change_line_num_0_RBUS mdom_sub_cap_cap1_buffer_change_line_num_0_reg;

    UINT8 cap_db_en = 0;
    unsigned int remove = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAC_FS_REMOVE_RATIO);
    unsigned int multiple = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAC_FS_MULTIPLE_RATIO);
    if(enable)
    {//one buffer
        mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
        cap_db_en = mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en;
        mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en = 1;
        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_reg_doublbuffer_reg,mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);

        //enable M-disp double buffer
        mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg);
        //disp_db_en = mdom_sub_disp_disp1_db_ctrl_reg.disp1_double_enable;
        mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en = 1;
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);

    /////////////////////////////////////////////////////////////////////////////////////////
        timeout = 0x3ffff;

        //first change M-disp.
        if (remove < multiple)
        {
            mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg);
            mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_distance = 0;
            IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg, mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue);
        } else {
            mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg);
            mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_force_en = 1;
            mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_force_addr = 2;
            IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg, mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue);

        }

        mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);
        while((mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply == 1) && (timeout--)){
            mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg);
        }

        timeout = 0x3ffff;

        if(!get_hdmi_vrr_4k60_mode()){
            //change M-cap
            if (remove < multiple)
            {
                mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg);
                mdom_sub_cap_cap1_buf_control_reg.cap1_buf_change_mode = 3;
                IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg, mdom_sub_cap_cap1_buf_control_reg.regValue);

                mdom_sub_cap_cap1_buffer_change_line_num_0_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_cap1_buffer_change_line_num_0_reg);
                mdom_sub_cap_cap1_buffer_change_line_num_0_reg.cap1_buf_change_line_num_sw = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_LEN) * (multiple - remove) / multiple;
                IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_buffer_change_line_num_0_reg, mdom_sub_cap_cap1_buffer_change_line_num_0_reg.regValue);
            } else {
                mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg);
                mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_en = 1;
                mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_addr = 2;
                IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg, mdom_sub_cap_cap1_buf_control_reg.regValue);
            }

            mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_apply = 1;
            IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_reg_doublbuffer_reg,mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);
            while((mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_apply == 1) && (timeout--)){
                mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
            }

        }
        rtd_pr_new_game_mode_notice("[Game Mode]4k120 sub M-domain game mode enable!!\n");

        /////////////////////////////////////////////////////////////////////////////////////////

        //resume M-cap double buffer setting
        mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
        if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_SEARCH) {
            mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en = 0;
        } else {
            mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en = cap_db_en;
        }
        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_reg_doublbuffer_reg, mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);
    }
    else
    {//three buffer
        timeout = 0x3ffff;
        //enable M-cap double buffer
        mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
        cap_db_en = mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en;
        mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en = 1;
        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_reg_doublbuffer_reg,mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);

        //enable M-disp double buffer
        mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg);
        //disp_db_en = ddr_mainsubctrl_reg.disp1_double_enable;
        mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en = 1;
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg, mdom_sub_disp_disp1_db_ctrl_reg.regValue);
        /////////////////////////////////////////////////////////////////////////////////////////

        if(!get_hdmi_vrr_4k60_mode()){
            //first set M-cap to triple buffer
            mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg);
            mdom_sub_cap_cap1_buf_control_reg.cap1_buf_change_mode = 0;
            mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_en = 0;
            mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_addr = 0;
            IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg, mdom_sub_cap_cap1_buf_control_reg.regValue);

            mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_apply = 1;
            IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_reg_doublbuffer_reg, mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);
            while((mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_apply == 1) && (timeout--)){
                mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
            }
        }

#if 0//for debug
        if(index_check_flag){
            mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DDR_In1Status_reg);
            mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DDR_MainStatus_reg);
            rtd_pr_scaler_memory_emerg("[Game Mode]2.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
        }
#endif

        timeout = 0x3ffff;

        //wait M-cap write latest 1 frame.
        /*check memory cap down*/
        mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg);
        mdom_sub_cap_cap1_ddr_fifostatus_reg.cap1_dma_frame_end = 1;
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg, mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue);

        mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg);
        while(!(mdom_sub_cap_cap1_ddr_fifostatus_reg.cap1_dma_frame_end) && --timeout){
            mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg);
        }

#if 0//for debug
        if(index_check_flag){
            mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DDR_In1Status_reg);
            mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DDR_MainStatus_reg);
            rtd_pr_scaler_memory_emerg("[Game Mode]3.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
        }
#endif

        timeout = 0x3ffff;

        //change M-disp.
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg);
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_distance = 1;
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_force_en = 0;
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_force_addr = 0;
        IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg, mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue);


        mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);
        while((mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply == 1) && (timeout--)){
            mdom_sub_disp_disp1_db_ctrl_reg.regValue =IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg);
        }

#if 0//for debug
        if(index_check_flag){
            mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DDR_In1Status_reg);
            mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DDR_MainStatus_reg);
            rtd_pr_scaler_memory_emerg("[Game Mode]4.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
        }
#endif
        //if(Get_DISPLAY_PANEL_OLED_TYPE()== TRUE){
        //  msleep(100);//wait M-cap write latest 2 frame.
        //}

        rtd_pr_new_game_mode_notice("[Game Mode]4k120 sub M-domain game mode disable!!\n");

        /////////////////////////////////////////////////////////////////////////////////////////
        //resume M-cap double buffer setting
        mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
        if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_SEARCH) {
            mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en = 0;
        } else {
            mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en = cap_db_en;
        }
        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_reg_doublbuffer_reg, mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);
    }

}

void libdma_drv_memory_GameMode_Switch_SingleBuffer(void){
	unsigned int timeout = 0x3ffff;
	M8P_mdomain_cap_cap0_reg_doublbuffer_RBUS mdomain_cap_cap0_reg_doublbuffer_reg;
	M8P_mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;
//	M8P_ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
	M8P_mdomain_cap_cap0_buf_control_RBUS mdomain_cap_cap0_buf_control_reg;
    M8P_mdomain_cap_cap0_buffer_change_line_num_0_RBUS mdomain_cap_cap0_buffer_change_line_num_0_reg;
	M8P_mdomain_disp_frc_disp0_buffer_select_control_RBUS mdomain_disp_frc_disp0_buffer_select_control_reg;

	UINT8 cap_db_en = 0;
    unsigned int remove = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAC_FS_REMOVE_RATIO);
    unsigned int multiple = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAC_FS_MULTIPLE_RATIO);
	//UINT8 disp_db_en = 0;
	//UINT8 display_dtg_db_en = 0;

	//enable M-cap double buffer
	mdomain_cap_cap0_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	cap_db_en = mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en;
	mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = 1;
	IoReg_Write32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);

	//enable M-disp double buffer
	mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
	//disp_db_en = ddr_mainsubctrl_reg.disp1_double_enable;
	mdomain_disp_disp0_db_ctrl_reg.disp0_db_en = 1;
	IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);

/////////////////////////////////////////////////////////////////////////////////////////
	timeout = 0x3ffff;

	//first change M-disp.
    if (remove < multiple)
    {
        mdomain_disp_frc_disp0_buffer_select_control_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg);
        mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_distance = 0;
        IoReg_Write32(M8P_MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg, mdomain_disp_frc_disp0_buffer_select_control_reg.regValue);
    }
    else {
        mdomain_disp_frc_disp0_buffer_select_control_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg);
        mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_en = 1;
        if(get_vo_camera_flow_flag(Get_DisplayMode_Port(SLR_MAIN_DISPLAY))==1)
            mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_addr = 0;
        else
            mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_addr = 2;
        IoReg_Write32(M8P_MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg, mdomain_disp_frc_disp0_buffer_select_control_reg.regValue);
    }


	mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply = 1;
	IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);
	while((mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply == 1) && (timeout--)){
		mdomain_disp_disp0_db_ctrl_reg.regValue =IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
	}

	timeout = 0x3ffff;

	//change M-cap
    if (remove < multiple)
    {
        mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg);
        mdomain_cap_cap0_buf_control_reg.cap0_buf_change_mode = 3;
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);

        mdomain_cap_cap0_buffer_change_line_num_0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_buffer_change_line_num_0_reg);
        mdomain_cap_cap0_buffer_change_line_num_0_reg.cap0_buf_change_line_num_sw = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN) * (multiple - remove) / multiple;
        IoReg_Write32(M8P_MDOMAIN_CAP_cap0_buffer_change_line_num_0_reg, mdomain_cap_cap0_buffer_change_line_num_0_reg.regValue);
    } else {
        mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg);
        mdomain_cap_cap0_buf_control_reg.cap0_buf_force_en = 1;
        if(get_vo_camera_flow_flag(Get_DisplayMode_Port(SLR_MAIN_DISPLAY))==1)
            mdomain_cap_cap0_buf_control_reg.cap0_buf_force_addr = 0;
        else
            mdomain_cap_cap0_buf_control_reg.cap0_buf_force_addr = 2;
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);
    }

	mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_apply = 1;
	IoReg_Write32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);
	while((mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_apply == 1) && (timeout--)){
		mdomain_cap_cap0_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	}
	rtd_pr_new_game_mode_notice("[Game Mode]M-domain game mode enable!!\n");

	/////////////////////////////////////////////////////////////////////////////////////////

	//resume M-cap double buffer setting
	mdomain_cap_cap0_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_SEARCH) {
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = 0;
	} else {
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = cap_db_en;
	}
	IoReg_Write32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg, mdomain_cap_cap0_reg_doublbuffer_reg.regValue);



}

void libdma_drv_memory_GameMode_Switch_TripleBuffer(void){
	unsigned int timeout = 0x3ffff;
	M8P_mdomain_cap_cap0_reg_doublbuffer_RBUS mdomain_cap_cap0_reg_doublbuffer_reg;
	M8P_mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;
//	M8P_ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
	M8P_mdomain_cap_cap0_buf_control_RBUS mdomain_cap_cap0_buf_control_reg;
    M8P_mdomain_disp_frc_disp0_buffer_select_control_RBUS mdomain_disp_frc_disp0_buffer_select_control_reg;
	M8P_mdomain_cap_cap0_ddr_fifostatus_RBUS mdomain_cap_cap0_ddr_fifostatus_reg;

#if 0//for debug
	M8P_mdomain_cap_ddr_in1status_RBUS mdomain_cap_ddr_in1status_reg;
	M8P_mdomain_disp_ddr_mainstatus_RBUS mdomain_disp_ddr_mainstatus_reg;
	UINT8 index_check_flag = 0;//for debug
#endif

	UINT8 cap_db_en = 0;
	//UINT8 disp_db_en = 0;
	//UINT8 display_dtg_db_en = 0;
	//unsigned int count = 0x3fffff;

	//enable M-cap double buffer
	mdomain_cap_cap0_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	cap_db_en = mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en;
	mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = 1;
	IoReg_Write32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);

	//enable M-disp double buffer
	mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
	//disp_db_en = ddr_mainsubctrl_reg.disp1_double_enable;
	mdomain_disp_disp0_db_ctrl_reg.disp0_db_en  = 1;
	IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);

#if 0//for debug
	if(index_check_flag){
		mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DDR_In1Status_reg);
		mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DDR_MainStatus_reg);
		rtd_pr_scaler_memory_emerg("[Game Mode]1.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
	}
#endif
	/////////////////////////////////////////////////////////////////////////////////////////

	//first set M-cap to triple buffer
	mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg);
	mdomain_cap_cap0_buf_control_reg.cap0_buf_force_en = 0;
    mdomain_cap_cap0_buf_control_reg.cap0_buf_force_addr = 0;
    mdomain_cap_cap0_buf_control_reg.cap0_buf_change_mode = 0;
	IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);

	mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_apply = 1;
	IoReg_Write32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);
	while((mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_apply == 1) && (timeout--)){
		mdomain_cap_cap0_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	}

#if 0//for debug
	if(index_check_flag){
		mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DDR_In1Status_reg);
		mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DDR_MainStatus_reg);
		rtd_pr_scaler_memory_emerg("[Game Mode]2.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
	}
#endif

	timeout = 0x3ffff;

	//wait M-cap write latest 1 frame.
    /*check memory cap down*/
	mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
    mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end = 1;
    IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg, mdomain_cap_cap0_ddr_fifostatus_reg.regValue);

	mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
	while(!(mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end) && --timeout){
		mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
	}

#if 0//for debug
	if(index_check_flag){
		mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DDR_In1Status_reg);
		mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DDR_MainStatus_reg);
		rtd_pr_scaler_memory_emerg("[Game Mode]3.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
	}
#endif

	timeout = 0x3ffff;

	//change M-disp.
	mdomain_disp_frc_disp0_buffer_select_control_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg);
    mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_distance = 1;
    mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_en = 0;
    mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_addr = 0;
    IoReg_Write32(M8P_MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg, mdomain_disp_frc_disp0_buffer_select_control_reg.regValue);


	mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply  = 1;
	IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);
	while((mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply == 1) && (timeout--)){
		mdomain_disp_disp0_db_ctrl_reg.regValue =IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
	}

#if 0//for debug
	if(index_check_flag){
		mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_DDR_In1Status_reg);
		mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DDR_MainStatus_reg);
		rtd_pr_scaler_memory_emerg("[Game Mode]4.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
	}
#endif
	//if(Get_DISPLAY_PANEL_OLED_TYPE()== TRUE){
	//	msleep(100);//wait M-cap write latest 2 frame.
	//}

	rtd_pr_new_game_mode_notice("[Game Mode]M-domain game mode disable!!\n");

	/////////////////////////////////////////////////////////////////////////////////////////
	//resume M-cap double buffer setting
	mdomain_cap_cap0_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_SEARCH) {
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = 0;
	} else {
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = cap_db_en;
	}
	IoReg_Write32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg, mdomain_cap_cap0_reg_doublbuffer_reg.regValue);



}
#ifndef BUILD_QUICK_SHOW
static void wait_m_cap_done(unsigned char display)
{//use for freeze wait cature done at vgip end
	unsigned int timeout = 0x3ffff;
	unsigned int vgip_linecount = 0;
	M8P_mdomain_cap_cap0_ddr_fifostatus_RBUS mdomain_cap_cap0_ddr_fifostatus_reg;

	if(display == SLR_MAIN_DISPLAY)
	{
		vgip_linecount = SCALEDOWN_ICH1_line_cnt_get_sdnr_ch1_line_cnt(IoReg_Read32(SCALEDOWN_ICH1_line_cnt_reg));//record current vgip line count
        mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
		while(!(mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end) && (SCALEDOWN_ICH1_line_cnt_get_sdnr_ch1_line_cnt(IoReg_Read32(SCALEDOWN_ICH1_line_cnt_reg)) >= vgip_linecount)
			&& (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_ACTIVE) && --timeout){//check m domain capture done or vgip over vsync or not active state
			
            mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
			if(drvif_mode_check_dma_onlinemeasure_status() == FALSE)//online fail
				break;
		}
	}
	else
	{

	}

}

//M-domain freeze firmware mode @Crixus 20170512, sync from k3 elieli
void libdma_drv_memory_freeze_fw_mode(unsigned char display, unsigned char enable, unsigned char freeze_block)
{
    M8P_mdomain_cap_cap0_buf_control_RBUS mdomain_cap_cap0_buf_control_reg;
    mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg);

	//game mode case, disable cap to freeze
	if(display == SLR_MAIN_DISPLAY)
	{
		if(!get_mdomain_driver_status())//m domain not ready no need to freeze
			return;

		if((drv_memory_get_game_mode_flag() == 1) && (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_VDEC) &&
            (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_CAMERA)){//seamless only hdmi has game mode case
			   if(enable == _ENABLE)
			       wait_m_cap_done(SLR_MAIN_DISPLAY);//disable cap need wait capture done
			   if(enable == _ENABLE){
					   libdma_drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);
			   }
			   else{
					   libdma_drvif_memory_EnableMCap(SLR_MAIN_DISPLAY);
			   }

		} else {
                //capture change to 1-buffer mode to freeze disp
                if(enable == _ENABLE){
                    mdomain_cap_cap0_buf_control_reg.cap0_buf_force_en = 1;
                    mdomain_cap_cap0_buf_control_reg.cap0_buf_force_addr = freeze_block;
                }
                else{
                    mdomain_cap_cap0_buf_control_reg.cap0_buf_force_en = 0;
                }
                IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);
		}
	}
}
#endif

void libdma_drv_memory_display_set_input_fast(unsigned char display, unsigned char enable)
{
	M8P_mdomain_cap_cap0_buf_control_RBUS mdomain_cap_cap0_buf_control_reg;
    M8P_mdom_sub_cap_cap1_buf_control_RBUS mdom_sub_cap_cap1_buf_control_reg;
    M8P_mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;
	M8P_mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;

	if(display == SLR_MAIN_DISPLAY) {
		if(enable == true) {
            mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg);
            mdomain_cap_cap0_buf_control_reg.cap0_input_fast = 1;
            IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);

//			mdomain_disp_main_disp_blocksel_option_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_Main_disp_blocksel_option_reg);
//			mdomain_disp_main_disp_blocksel_option_reg.main_force_blocksel_toggle_en = 1;
//			mdomain_disp_main_disp_blocksel_option_reg.main_force_blocksel_toggle_mode = 1;
//			mdomain_disp_main_disp_blocksel_option_reg.main_force_blocksel_toggle_line = Get_DISP_DEN_END_VPOS();
//			IoReg_Write32(M8P_MDOMAIN_DISP_Main_disp_blocksel_option_reg,  mdomain_disp_main_disp_blocksel_option_reg.regValue);

			mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
			mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply = 1;
			IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg,  mdomain_disp_disp0_db_ctrl_reg.regValue);
		} else {
            mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg);
            mdomain_cap_cap0_buf_control_reg.cap0_input_fast = 0;
            IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);

//			mdomain_disp_main_disp_blocksel_option_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_Main_disp_blocksel_option_reg);
//			mdomain_disp_main_disp_blocksel_option_reg.main_force_blocksel_toggle_en = 0;
//			mdomain_disp_main_disp_blocksel_option_reg.main_force_blocksel_toggle_mode = 0;
//			mdomain_disp_main_disp_blocksel_option_reg.main_force_blocksel_toggle_line = 0;
//			IoReg_Write32(M8P_MDOMAIN_DISP_Main_disp_blocksel_option_reg,  mdomain_disp_main_disp_blocksel_option_reg.regValue);

			mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
			mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply = 1;
			IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg,  mdomain_disp_disp0_db_ctrl_reg.regValue);
		}
	} else {
		if(enable == true) {
            mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg);
            mdom_sub_cap_cap1_buf_control_reg.cap1_input_fast = 1;
            IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg, mdom_sub_cap_cap1_buf_control_reg.regValue);

//			mdomain_disp_sub_disp_blocksel_option_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_Sub_disp_blocksel_option_reg);
//			mdomain_disp_sub_disp_blocksel_option_reg.sub_force_blocksel_toggle_en = 1;
//			mdomain_disp_sub_disp_blocksel_option_reg.sub_force_blocksel_toggle_mode = 1;
//			mdomain_disp_sub_disp_blocksel_option_reg.sub_force_blocksel_toggle_line = Get_DISP_DEN_END_VPOS();
//			IoReg_Write32(M8P_MDOMAIN_DISP_Sub_disp_blocksel_option_reg,  mdomain_disp_sub_disp_blocksel_option_reg.regValue);

			mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg);
			mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
			IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg,  mdom_sub_disp_disp1_db_ctrl_reg.regValue);
		} else {
            mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg);
            mdom_sub_cap_cap1_buf_control_reg.cap1_input_fast = 0;
            IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg, mdom_sub_cap_cap1_buf_control_reg.regValue);

//			mdomain_disp_sub_disp_blocksel_option_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_Sub_disp_blocksel_option_reg);
//			mdomain_disp_sub_disp_blocksel_option_reg.sub_force_blocksel_toggle_en = 0;
//			mdomain_disp_sub_disp_blocksel_option_reg.sub_force_blocksel_toggle_mode = 0;
//			mdomain_disp_sub_disp_blocksel_option_reg.sub_force_blocksel_toggle_line = 0;
//			IoReg_Write32(M8P_MDOMAIN_DISP_Sub_disp_blocksel_option_reg,  mdomain_disp_sub_disp_blocksel_option_reg.regValue);

			mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg);
			mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
			IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg,  mdom_sub_disp_disp1_db_ctrl_reg.regValue);
		}
	}
}

void libdma_drv_memory_display_check_sub_frc_style(void)
{
	unsigned int uzudtg_output_framerate = 0;
	unsigned int uzudtg_dh_total = PPOVERLAY_uzudtg_DH_TOTAL_get_uzudtg_dh_total(IoReg_Read32(PPOVERLAY_uzudtg_DH_TOTAL_reg)) + 1;
	unsigned int uzudtg_dv_total = PPOVERLAY_uzudtg_DV_TOTAL_get_uzudtg_dv_total(IoReg_Read32(PPOVERLAY_uzudtg_DV_TOTAL_reg)) + 1;
	unsigned int remove = PPOVERLAY_DTG_M_Remove_input_vsync_get_remove_half_ivs_mode2(IoReg_Read32(PPOVERLAY_DTG_M_Remove_input_vsync_reg)) + 1;
	unsigned int multiple = PPOVERLAY_DTG_M_multiple_vsync_get_dtg_m_multiple_vsync(IoReg_Read32(PPOVERLAY_DTG_M_multiple_vsync_reg)) + 1;

	if (!get_sub_OutputVencMode())
	{//sub m domain iterrupt open so don't need wait lastwrite @qing_liu --s4ap
		//Update the M-domain last done @Crixus 20161013
		libdma_drv_memory_wait_cap_last_write_done(SLR_SUB_DISPLAY, 1, FALSE);

		if( get_panel_pixel_mode() > PANEL_1_PIXEL )
			uzudtg_output_framerate = Get_DISPLAY_CLOCK_TYPICAL() / uzudtg_dh_total * 2000 / uzudtg_dv_total * remove / multiple;
		else
			uzudtg_output_framerate = Get_DISPLAY_CLOCK_TYPICAL() / uzudtg_dh_total * 1000 / uzudtg_dv_total * remove / multiple;

		if(
			( uzudtg_output_framerate < ( Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_V_FREQ_1000) - 500 ) ) &&
			(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_INTERLACE) == FALSE)) // If sub path is interlace timing, always set sub input slow and CH2_FDROP_EN = 1 - WOSQRTK-17868
			libdma_mdomain_input_fast_flag[SLR_SUB_DISPLAY] = 1;
		else
			libdma_mdomain_input_fast_flag[SLR_SUB_DISPLAY] = 0;

		rtd_pr_scaler_memory_info("[%s] check sub input style, sub_frc_style=%d, main_fr=%d, sub_vfreq=%d\n", __FUNCTION__, libdma_mdomain_input_fast_flag[SLR_SUB_DISPLAY], uzudtg_output_framerate, Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_V_FREQ));
	}
}

unsigned int libdma_drv_Calculate_m_pre_read_value(void)
{
	//dic suggest mac9q pre_read need set 1
	unsigned int m_pre_read = 1;
#if 0
	unsigned int DV_den_start = Get_DISP_DEN_STA_VPOS();
	unsigned int orbit_mv_extend_max_half = 0;
	unsigned int time_borrow = 0;
	unsigned int total_interval = 0;
	M8P_ppoverlay_uzudtg_orbit_main_ctrl0_RBUS ppoverlay_uzudtg_orbit_main_ctrl0_reg;
	M8P_ppoverlay_uzudtg_stage1_time_borrow_RBUS ppoverlay_uzudtg_stage1_time_borrow_reg;

	if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC))
	{
		m_pre_read = _M_DATA_FS_PRE_READ;
	}
	else
	{
		ppoverlay_uzudtg_stage1_time_borrow_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_stage1_time_borrow_reg);
		time_borrow = ppoverlay_uzudtg_stage1_time_borrow_reg.stage1_time_borrow_v;
		rtd_pr_scaler_memory_notice("[%s] DV_den_start = %d, total_interval += time_borrow : %d\n", __FUNCTION__, DV_den_start, time_borrow);
		total_interval += time_borrow;

		// ===== sync from K24 ( version : read settings from register ), merline8 doese not have orbit function now - EricTang 20230928 =====
		ppoverlay_uzudtg_orbit_main_ctrl0_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_orbit_main_ctrl0_reg);
		if( ( Get_DISPLAY_PANEL_OLED_TYPE() == TRUE ) && ( ppoverlay_uzudtg_orbit_main_ctrl0_reg.orbit_m_overscan_en == 1 ) )	// orbit_mv_extend_max only valid in OLED overscan case
		{
			orbit_mv_extend_max_half = ppoverlay_uzudtg_orbit_main_ctrl0_reg.orbit_mv_extend_max / 2;	// Note : orbit_mv_extend_max must be 2N
			rtd_pr_scaler_memory_notice("[%s] total_interval += orbit_mv_extend_max_half : %d\n", __FUNCTION__, orbit_mv_extend_max_half);
			total_interval += orbit_mv_extend_max_half;
		}
		// ===================================================================================================================================

		if( ( get_panel_res() == PANEL_RES_FHD ) || ( get_panel_res() == PANEL_RES_HD ) )
		{
			rtd_pr_scaler_memory_notice("[%s] total_interval += _M_PRE_READ_MARGIN_FHD : %d\n", __FUNCTION__, _M_PRE_READ_MARGIN_FHD);
			total_interval += _M_PRE_READ_MARGIN_FHD;	// _M_PRE_READ_MARGIN_FHD need to be at least larger than 4 ( m_pre_read <=2=> db_latch_line <=2=> DV_den_start )
		}
		else
		{
			rtd_pr_scaler_memory_notice("[%s] total_interval += _M_PRE_READ_MARGIN : %d\n", __FUNCTION__, _M_PRE_READ_MARGIN);
			total_interval += _M_PRE_READ_MARGIN;	// _M_PRE_READ_MARGIN need to be at least larger than 4 ( m_pre_read <=2=> db_latch_line <=2=> DV_den_start )
		}

		if( DV_den_start >= total_interval )
		{
			m_pre_read = DV_den_start - total_interval;
		}
		else
		{
			m_pre_read = 0;
			rtd_pr_scaler_memory_err("[%s][Error] DV_den_start - total_interval < 0\n", __FUNCTION__);
		}
	}

	rtd_pr_scaler_memory_notice("[%s] m_pre_read = %d!!\n", __FUNCTION__, m_pre_read);
#endif

	return m_pre_read;
}

void memory_frc_set_pre_read_v_start(unsigned char display, unsigned int pre_read)
{
    if (display == SLR_MAIN_DISPLAY){
        M8P_mdomain_disp_disp0_pre_rd_ctrl_RBUS mdomain_disp_disp0_pre_rd_ctrl_reg;
        M8P_mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;

    #ifndef BUILD_QUICK_SHOW
        Check_smooth_toggle_update_flag(SLR_MAIN_DISPLAY);
    #endif

        mdomain_disp_disp0_pre_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_pre_rd_ctrl_reg);
        rtd_pr_scaler_memory_info("[%s] change main_pre_rd_v_start from %d -> %d\n", __FUNCTION__, mdomain_disp_disp0_pre_rd_ctrl_reg.disp0_pre_rd_v_start, pre_read);
        mdomain_disp_disp0_pre_rd_ctrl_reg.disp0_pre_rd_v_start = pre_read;
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_pre_rd_ctrl_reg, mdomain_disp_disp0_pre_rd_ctrl_reg.regValue);

        mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
        if( mdomain_disp_disp0_db_ctrl_reg.disp0_db_en == 1 )
        {
            mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply = 1;
            IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);
        }

        //drvif_srnn_set_pre_rd_start();
        scaler_disp_dtg_set_db_latch_line();
    } else {
        M8P_mdom_sub_disp_disp1_pre_rd_ctrl_RBUS mdom_sub_disp_disp1_pre_rd_ctrl_reg;
        M8P_mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;

        mdom_sub_disp_disp1_pre_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_pre_rd_ctrl_reg);
         rtd_pr_scaler_memory_info("[%s] change sub_pre_rd_v_start from %d -> %d\n", __FUNCTION__, mdom_sub_disp_disp1_pre_rd_ctrl_reg.disp1_pre_rd_v_start, pre_read);
        mdom_sub_disp_disp1_pre_rd_ctrl_reg.disp1_pre_rd_v_start = pre_read;
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_pre_rd_ctrl_reg, mdom_sub_disp_disp1_pre_rd_ctrl_reg.regValue);

        mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg);
        if (mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en == 1)
        {
            mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
            IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg, mdom_sub_disp_disp1_db_ctrl_reg.regValue);
        }
    }
}

void libdma_drv_memory_set_vo_gating_threshold(void)
{
    M8P_mdomain_disp_disp0_fsync_ctrl_1_RBUS mdomain_disp_disp0_fsync_ctrl_1_reg;
    M8P_mdomain_disp_disp0_fsync_ctrl_0_RBUS mdomain_disp_disp0_fsync_ctrl_0_reg;

    M8P_mdom_sub_disp_disp1_fsync_ctrl_1_RBUS mdom_sub_disp_disp1_fsync_ctrl_1_reg;
    M8P_mdom_sub_disp_disp1_fsync_ctrl_0_RBUS mdom_sub_disp_disp1_fsync_ctrl_0_reg;

    //main
    mdomain_disp_disp0_fsync_ctrl_1_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DISP0_FSYNC_CTRL_1_reg);
    mdomain_disp_disp0_fsync_ctrl_1_reg.disp0_fifo_gate_th1 = MAIN_FIFO_GATE_TH_1;
    mdomain_disp_disp0_fsync_ctrl_1_reg.disp0_fifo_gate_th0 = MAIN_FIFO_GATE_TH;
    IoReg_Write32(M8P_MDOMAIN_DISP_DISP0_FSYNC_CTRL_1_reg, mdomain_disp_disp0_fsync_ctrl_1_reg.regValue);

    mdomain_disp_disp0_fsync_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DISP0_FSYNC_CTRL_0_reg);
    mdomain_disp_disp0_fsync_ctrl_0_reg.disp0_fifo_gate_th3 = MAIN_FIFO_GATE_TH_3;
    mdomain_disp_disp0_fsync_ctrl_0_reg.disp0_fifo_gate_th2 = MAIN_FIFO_GATE_TH_2;
    IoReg_Write32(M8P_MDOMAIN_DISP_DISP0_FSYNC_CTRL_0_reg, mdomain_disp_disp0_fsync_ctrl_0_reg.regValue);

    //sub
    mdom_sub_disp_disp1_fsync_ctrl_1_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_DISP1_FSYNC_CTRL_1_reg);
    mdom_sub_disp_disp1_fsync_ctrl_1_reg.disp1_fifo_gate_th1 = MAIN_FIFO_GATE_TH_1;
    mdom_sub_disp_disp1_fsync_ctrl_1_reg.disp1_fifo_gate_th0 = MAIN_FIFO_GATE_TH;

    IoReg_Write32(M8P_MDOM_SUB_DISP_DISP1_FSYNC_CTRL_1_reg, mdom_sub_disp_disp1_fsync_ctrl_1_reg.regValue);
    mdom_sub_disp_disp1_fsync_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_DISP1_FSYNC_CTRL_0_reg);
    mdom_sub_disp_disp1_fsync_ctrl_0_reg.disp1_fifo_gate_th3 = MAIN_FIFO_GATE_TH_3;
    mdom_sub_disp_disp1_fsync_ctrl_0_reg.disp1_fifo_gate_th2 = MAIN_FIFO_GATE_TH_2;
    IoReg_Write32(M8P_MDOM_SUB_DISP_DISP1_FSYNC_CTRL_0_reg, mdom_sub_disp_disp1_fsync_ctrl_0_reg.regValue);

}

void libdma_wait_sub_disp_double_buffer_apply(void)
{

	unsigned int timeout = 50;
    M8P_mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;

	mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
	IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);

	mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg);
	while ((mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply == 1) && (timeout--)) {
		mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg);
		usleep_range(5000, 5000);
	}

}

void libdma_drvif_memory_EnableMDisp_Or_DisableMDisp(unsigned char display, unsigned char enable)
{
    M8P_mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;
	unsigned char use_db_disable = FALSE;//if data frame sync and gatting enable need to using db apply
	int apply_timeout_cnt = 10;

	rtd_pr_scaler_memory_info("[%s] %d (90K=%d) display=%d, enable=%d\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), display, enable);

	if(enable == _ENABLE)
	{
		//enable main/sub disp
		IoReg_SetBits(display? M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg: M8P_MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg, display? M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_disp1_dma_enable_mask: M8P_MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_disp0_dma_enable_mask);
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            IoReg_SetBits(display? M8P_MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_reg: M8P_MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg, display? M8P_MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_disp1c_dma_enable_mask: M8P_MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_disp0c_dma_enable_mask);
        }
		//wait double buffer apply
		libdma_drvif_memory_set_dbuffer_write(display);
	}
	else
	{
		if(display == SLR_MAIN_DISPLAY)
		{
			M8P_mdomain_cap_cap0_pxl_wrap_ctrl_0_RBUS mdomain_cap_cap0_pxl_wrap_ctrl_0_reg;
			mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg);
			if(mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en && (VODMA_VODMA_CLKGEN_get_en_fifo_full_gate(IoReg_Read32(VODMA_VODMA_CLKGEN_reg))))
				use_db_disable = TRUE;//gatting enable using db apply

			if(use_db_disable)
			{
				//enable double buffer
				mdomain_disp_disp0_db_ctrl_reg.regValue =IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
				mdomain_disp_disp0_db_ctrl_reg.disp0_db_en=1;
				IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);
			}

			//FRC
			mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg);
			mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en = 0;
			IoReg_Write32(M8P_MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg, mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue);

			//disable main disp
			IoReg_ClearBits(M8P_MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg, M8P_MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_disp0_dma_enable_mask);
            IoReg_ClearBits(M8P_MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg, M8P_MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_disp0c_dma_enable_mask);

			//wait double buffer apply
			libdma_drvif_memory_set_dbuffer_write(SLR_MAIN_DISPLAY);

			if(use_db_disable || mdomain_disp_disp0_db_ctrl_reg.disp0_db_en)
			{
				mdomain_disp_disp0_db_ctrl_reg.regValue =IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
				while(apply_timeout_cnt-- && (mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply))
				{
					mdomain_disp_disp0_db_ctrl_reg.regValue =IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
					msleep(10);
				}
				if(!apply_timeout_cnt)
					rtd_pr_scaler_memory_err("[%s] %d using db to disable wait timeout!!\n", __FUNCTION__, __LINE__);
				else
					rtd_pr_scaler_memory_info("[%s] %d using db to disable\n", __FUNCTION__, __LINE__);
			}
			mdomain_disp_disp0_db_ctrl_reg.regValue =IoReg_Read32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg);
			mdomain_disp_disp0_db_ctrl_reg.disp0_db_en=0;
			IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);
		}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
		else
		{
			//disable sub disp
            IoReg_ClearBits(M8P_MDOM_SUB_DISP_Disp1_db_ctrl_reg, M8P_MDOM_SUB_DISP_Disp1_db_ctrl_disp1_db_en_mask);
			IoReg_ClearBits(M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg, M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_disp1_dma_enable_mask);

			//wait double buffer apply
			libdma_drvif_memory_set_dbuffer_write(SLR_SUB_DISPLAY);
		}
#endif
	}
}

void libdma_drvif_memory_EnableMCap(unsigned char display)
{
	rtd_pr_scaler_memory_info("[%s] %d (90K=%d) display=%d\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), display);

	IoReg_SetBits(display ? M8P_MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg : M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg, display? M8P_MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_cap1_dma_enable_mask: M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_cap0_dma_enable_mask);
    if (dvrif_memory_get_block_mode_enable(display) == TRUE){
        IoReg_SetBits(display ? M8P_MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg : M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg, display? M8P_MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_cap1c_dma_enable_mask: M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_cap0c_dma_enable_mask);
    }
 
	if (display == SLR_MAIN_DISPLAY)
	{
		/*wclr cap buf done status*/
        M8P_mdomain_cap_cap0_ddr_fifostatus_RBUS mdomain_cap_cap0_ddr_fifostatus_reg;
        mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
        mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end = 1;
        mdomain_cap_cap0_ddr_fifostatus_reg.cap0_first_wr_flag = 1;
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg, mdomain_cap_cap0_ddr_fifostatus_reg.regValue);
	}
	else
	{
        M8P_mdom_sub_cap_cap1_ddr_fifostatus_RBUS mdom_sub_cap_cap1_ddr_fifostatus_reg;
        mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg);
        mdom_sub_cap_cap1_ddr_fifostatus_reg.cap1_dma_frame_end = 1;
        mdom_sub_cap_cap1_ddr_fifostatus_reg.cap1_first_wr_flag = 1;
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg, mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue);
	}

}

void libdma_drvif_memory_DisableMCap(unsigned char display)
{
    M8P_mdomain_cap_frc_cap0_wr_ctrl_RBUS mdomain_cap_frc_cap0_wr_ctrl_reg;
    M8P_mdom_sub_cap_frc_cap1_wr_ctrl_RBUS mdom_sub_cap_frc_cap1_wr_ctrl_reg;
    M8P_mdomain_cap_frc_cap0c_wr_ctrl_RBUS mdomain_cap_frc_cap0c_wr_ctrl_reg;
    M8P_mdom_sub_cap_frc_cap1c_wr_ctrl_RBUS mdom_sub_cap_frc_cap1c_wr_ctrl_reg;

    rtd_pr_scaler_memory_info("[%s] %d (90K=%d) display=%d\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), display);

    if (SLR_MAIN_DISPLAY == display)
    {
        mdomain_cap_frc_cap0_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg);

        if (TRUE == mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_enable)
        {
            //Disable MCap
            rtd_pr_scaler_memory_info("[Mdomain] Disable cap main dma and clear db\n");
            IoReg_ClearBits(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg, M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_cap0_db_en_mask);

            mdomain_cap_frc_cap0_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg);
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_enable = 0;
            IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg, mdomain_cap_frc_cap0_wr_ctrl_reg.regValue);
        }

        mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg);
        if (TRUE == mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_enable)
        {
            rtd_pr_scaler_memory_info("[Mdomain] Disable cap main dmac and clear db\n");
            IoReg_ClearBits(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg, M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_cap0_db_en_mask);

            mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg);
            mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_enable = 0;
            IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg, mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue);
        }
    }
    else
    {
        mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg);

        if (TRUE == mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_enable)
        {
            //Disable MCap
            rtd_pr_scaler_memory_info("[Mdomain] Disable cap sub dma and clear db\n");
            IoReg_ClearBits(M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg, M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_disp1_dma_enable_mask);
            
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg);
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_enable = 0;
            IoReg_Write32(M8P_MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg, mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue);
        }

        mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg);
        if (TRUE == mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_enable)
        {
            rtd_pr_scaler_memory_info("[Mdomain] Disable cap sub dmac and clear db\n");
            IoReg_ClearBits(M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg, M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_disp1_dma_enable_mask);
            
            mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg);
            mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_enable = 0;
            IoReg_Write32(M8P_MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg, mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue);
        }
    }

}

#ifdef BUILD_QUICK_SHOW

static MDOMAIN_MALLOC_STRUCT mdomainBufferInfo[MDOMAIN_MAX] =
{
    {0, 0, 0},
    {0, 0, 0}
};

unsigned long get_vbm_heap_qshow_scaler_address(unsigned long *size);

void libdma_scaler_quickshow_memery_init(void)
{
    unsigned int i = 0;
    unsigned char ucStatus = 0;
    unsigned long ulAddr = 0 ;
    unsigned long ulGetSize = 0 ;
    unsigned long allocSize = 0;
    unsigned long main_max_size = 0, i3_max_size = 0, sub_max_size = 0;
	main_max_size = ((MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K120_CO_BUFFER) > (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K)) ? (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K120_CO_BUFFER) : (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K);
	sub_max_size = ((MDOMAIN_SUB_TWO_PIXEL_BUFFER_SIZE_PC_MODE) > (MDOMAIN_SUB_BUFFER_SIZE)) ? (MDOMAIN_SUB_TWO_PIXEL_BUFFER_SIZE_PC_MODE) : (MDOMAIN_SUB_BUFFER_SIZE);
	i3_max_size = (_2K_1K_24BIT_SIZE);//from I3DDMA_DolbyVision_HDMI_Init

    for(i = 0; i < MDOMAIN_MAX; i++)
    {
        ucStatus = mdomainBufferInfo[i].status;
        if(ucStatus != 0)
        {
            break;
        }
    }

    if(ucStatus == 0)
    {
        allocSize = main_max_size*3 + sub_max_size*3 + i3_max_size*3;

        ulAddr = get_vbm_heap_qshow_scaler_address(&ulGetSize);

        rtd_pr_scaler_memory_info("[%s %d]ulAddr:0x%08x, sulGetSize:0x%08x\n", __FUNCTION__, __LINE__, ulAddr, ulGetSize);

        if(ulAddr != 0 && (ulGetSize >= allocSize))
        {
            mdomainBufferInfo[MDOMAIN_MAIN].phyAddr = ulAddr;
            mdomainBufferInfo[MDOMAIN_MAIN].size = main_max_size*3 + i3_max_size*3;
            mdomainBufferInfo[MDOMAIN_MAIN].status = 1;

            mdomainBufferInfo[MDOMAIN_SUB].phyAddr = mdomainBufferInfo[MDOMAIN_MAIN].phyAddr + mdomainBufferInfo[MDOMAIN_MAIN].size;
            mdomainBufferInfo[MDOMAIN_SUB].size = sub_max_size*3;
            mdomainBufferInfo[MDOMAIN_SUB].status = 1;
        }
        else
        {
			printf2("[%s %d]qs memory get failed\n", __FUNCTION__, __LINE__);
        }
    }
}

unsigned long libdma_qs_get_memory_from_vbm(MDOMAIN_MEMORY_QUERY_ID id, unsigned long allocSize)
{
    unsigned long ulAddr = 0;

    if(id >= MDOMAIN_MAX || (mdomainBufferInfo[id].status == 0))
    {
		printf2("[%s %d]query id or status error id:%d\n", __FUNCTION__, __LINE__, id);
        return 0;
    }

    switch(id)
    {
        case MDOMAIN_MAIN:
            {
                if(judge_scaler_run_i3ddma_vo_path()){
                    //allocSize = i3 + mdomain max size
                    allocSize += (((MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K120_CO_BUFFER) > (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K)) ? (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K120_CO_BUFFER) : (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K));
                }
                if(allocSize <= mdomainBufferInfo[MDOMAIN_MAIN].size)
                {
                    ulAddr = mdomainBufferInfo[MDOMAIN_MAIN].phyAddr;
                }
                else
                {
                    printf2("[%s %d]error get memory failed\n", __FUNCTION__, __LINE__);
                }
            }
            break;

        case MDOMAIN_SUB:
            {
                if(allocSize <= mdomainBufferInfo[MDOMAIN_SUB].size)
                {
                    ulAddr = mdomainBufferInfo[MDOMAIN_SUB].phyAddr;
                }
                else
                {
                    printf2("[%s %d]error get memory failed\n", __FUNCTION__, __LINE__);
                }
            }
            break;

        default:
            break;
    }

    rtd_pr_scaler_memory_info("[%s %d]ulAddr:0x%08x, id:%d, needSize:0x%08x, mdomainBufferInfo[MDOMAIN_SUB].size:0x%08x\n", __FUNCTION__, __LINE__, ulAddr, id, allocSize, mdomainBufferInfo[id].size);

    return ulAddr;
}
#endif

void memory_frc_decide_capture_mode(unsigned char display)
{
    if((Scaler_InputSrcGetMainChType() == _SRC_VO) && (Scaler_VOFrom(Scaler_Get_CurVoInfo_plane()) == 1))	// yychao+ : use single buffer for JPEG source
    {
        dvrif_memory_set_cap_mode(display, MEMCAPTYPE_FRAME);
    }
    else
    {
        if(dvrif_memory_get_compression_mode(display) == COMPRESSION_LINE_MODE)
        {
            dvrif_memory_set_cap_mode(display, MEMCAPTYPE_LINE);
        }
        else
        {
            dvrif_memory_set_cap_mode(display, MEMCAPTYPE_FRAME);
        }
    }
}

void memory_frc_decide_buffer_number(unsigned char display)
{
#ifdef ENABLE_3_BUFFER_DELAY_MODE
#ifndef BUILD_QUICK_SHOW
    if (get_video_delay_type() == M_DOMAIN_VIDEO_DELAY){
        dvrif_memory_set_buffer_number(display, 3 + get_scaler_video_delay_number());
    } else
#endif
    {
        dvrif_memory_set_buffer_number(display, 3);
    }
#else
    if((Scaler_InputSrcGetMainChType() == _SRC_VO) && (Scaler_VOFrom(Scaler_Get_CurVoInfo_plane()) == 1))
    {
        dvrif_memory_set_buffer_number(display, 1);
    } else {
        dvrif_memory_set_buffer_number(display, 3);
    }
#endif
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
    if (Get_rotate_function(display)){
        dvrif_memory_set_buffer_number(display, 1);
    }
#endif
#endif
}

void memory_set_capture_byte_swap(unsigned char display)
{
    M8P_mdomain_cap_frc_cap0_wr_ctrl_RBUS mdomain_cap_frc_cap0_wr_ctrl_reg;
    M8P_mdom_sub_cap_frc_cap1_wr_ctrl_RBUS mdom_sub_cap_frc_cap1_wr_ctrl_reg;

    //frank@03132013 add below code for GPU access YUV format ++
    if(display){

        mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg);
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
        if (Get_rotate_function(display)) {
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_1byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_2byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_4byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_8byte_swap = 1;
        } else 
#endif
#endif
        if(Scaler_DispGetStatus(display,SLR_DISP_422CAP))
        {
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_1byte_swap = 0;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_2byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_4byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_8byte_swap = 0;
        }
        else
        {
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_1byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_2byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_4byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_8byte_swap = 0;
        }
        IoReg_Write32(M8P_MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg, mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue);
    }else{
        mdomain_cap_frc_cap0_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg);
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
        if (Get_rotate_function(display)) {
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_1byte_swap = 1;
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_2byte_swap = 1;
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_4byte_swap = 1;
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_8byte_swap = 1;
        } else 
#endif
#endif
        if(Scaler_DispGetStatus(display,SLR_DISP_10BIT))
        {
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_1byte_swap = 0;
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_2byte_swap = 0;
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_4byte_swap = 0;
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_8byte_swap = 0;
        }
        else
        {
            if(Scaler_DispGetStatus(display,SLR_DISP_422CAP))
            {
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_1byte_swap = 0;
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_2byte_swap = 1;
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_4byte_swap = 1;
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_8byte_swap = 0;
            }
            else
            {
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_1byte_swap = 1;
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_2byte_swap = 1;
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_4byte_swap = 1;
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_8byte_swap = 0;
            }
        }
        IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg, mdomain_cap_frc_cap0_wr_ctrl_reg.regValue);
    }
}

void memory_set_capture_ds_path(unsigned char display)
{
    unsigned int CapWidth = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_WID);
	unsigned int CapLength = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_LEN);

    if (display == SLR_MAIN_DISPLAY)
    {
        M8P_mdomain_cap_cap0_ds_ctrl_0_RBUS mdomain_cap_cap0_ds_ctrl_0_reg;
        M8P_mdomain_cap_cap0_pxl_wrap_ctrl_0_RBUS mdomain_cap_cap0_pxl_wrap_ctrl_0_reg;
        M8P_mdomain_cap_frc_cap0_pack_ctrl_RBUS mdomain_cap_frc_cap0_pack_ctrl_reg;

        // ds ctrl
        mdomain_cap_cap0_ds_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_DS_Ctrl_0_reg);
        mdomain_cap_cap0_ds_ctrl_0_reg.ds_hor_mode = 0;
        mdomain_cap_cap0_ds_ctrl_0_reg.ds_ver_mode = 0;
        if (dvrif_memory_get_block_mode_enable(display) == TRUE) {
            mdomain_cap_cap0_ds_ctrl_0_reg.cap0_ds444_mode = 3;
            mdomain_cap_cap0_ds_ctrl_0_reg.cap0_ds444_en = 0;
        } else {
            mdomain_cap_cap0_ds_ctrl_0_reg.cap0_ds444_mode = 0;
            if (dvrif_memory_compression_get_enable(display)){
                mdomain_cap_cap0_ds_ctrl_0_reg.cap0_ds444_en = 0;
            } else {
                mdomain_cap_cap0_ds_ctrl_0_reg.cap0_ds444_en = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
            }
            
        }
        IoReg_Write32(M8P_MDOMAIN_CAP_cap0_DS_Ctrl_0_reg, mdomain_cap_cap0_ds_ctrl_0_reg.regValue);

        rtd_pr_scaler_memory_emerg("[Mdomain][Main] ds_ver_en = %d, ds_hor_en = %d\n", mdomain_cap_cap0_ds_ctrl_0_reg.cap0_ds444_mode, mdomain_cap_cap0_ds_ctrl_0_reg.cap0_ds444_en);

        // pxl wrap
        mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg);
        mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en = 0;
        mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_dummy_format = (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME);
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_data_format = MEM_8_BIT;
            mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_wrap_mode = 7;
        } else {
            mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_data_format = Scaler_DispGetStatus(display, SLR_DISP_10BIT);
            mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_wrap_mode = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
        }
        IoReg_Write32(M8P_MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg, mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue);

        rtd_pr_scaler_memory_emerg("[Mdomain][Main] pack_wrap_mode = %d, pack_dummy_format = %d, pack_data_format = %d\n",
            mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_wrap_mode,  mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_dummy_format, mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_data_format);

        // pack ctrl
        mdomain_cap_frc_cap0_pack_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_frc_cap0_pack_ctrl_reg);
        mdomain_cap_frc_cap0_pack_ctrl_reg.cap0_frame_acc_mode = (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME);
        mdomain_cap_frc_cap0_pack_ctrl_reg.cap0_vact = drvif_memory_get_data_align(CapLength, 2);
        mdomain_cap_frc_cap0_pack_ctrl_reg.cap0_hact = drvif_memory_get_data_align(CapWidth, 2);
        IoReg_Write32(M8P_MDOMAIN_CAP_frc_cap0_pack_ctrl_reg, mdomain_cap_frc_cap0_pack_ctrl_reg.regValue);
    } else
    {
        M8P_mdom_sub_cap_cap1_ds_ctrl_0_RBUS mdom_sub_cap_cap1_ds_ctrl_0_reg;
        M8P_mdom_sub_cap_cap1_pxl_wrap_ctrl_0_RBUS mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg;
        M8P_mdom_sub_cap_frc_cap1_pack_ctrl_RBUS mdom_sub_cap_frc_cap1_pack_ctrl_reg;
  
            // ds ctrl
        mdom_sub_cap_cap1_ds_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_cap1_DS_Ctrl_0_reg);
        mdom_sub_cap_cap1_ds_ctrl_0_reg.ds_hor_mode = 0;
        mdom_sub_cap_cap1_ds_ctrl_0_reg.ds_ver_mode = 0;
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdom_sub_cap_cap1_ds_ctrl_0_reg.cap1_ds444_mode = 3;
            mdom_sub_cap_cap1_ds_ctrl_0_reg.cap1_ds444_en = 0;
        } else {
            mdom_sub_cap_cap1_ds_ctrl_0_reg.cap1_ds444_mode = 0;
            if (dvrif_memory_compression_get_enable(display)){
                mdom_sub_cap_cap1_ds_ctrl_0_reg.cap1_ds444_en = 0;
            } else {
                mdom_sub_cap_cap1_ds_ctrl_0_reg.cap1_ds444_en = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
            }
        }
        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_DS_Ctrl_0_reg, mdom_sub_cap_cap1_ds_ctrl_0_reg.regValue);

        rtd_pr_scaler_memory_emerg("[Mdomain][Sub] ds_ver_en = %d, ds_hor_en = %d\n", mdom_sub_cap_cap1_ds_ctrl_0_reg.cap1_ds444_mode, mdom_sub_cap_cap1_ds_ctrl_0_reg.cap1_ds444_en);

        // pxl wrap
        mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_cap1_Pxl_Wrap_Ctrl_0_reg);
        mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.disp1_fs_en = 0;
        mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_dummy_format = (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME);

        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_data_format = MEM_8_BIT;
            mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_wrap_mode = 7;
        } else {
            mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_data_format = Scaler_DispGetStatus(display, SLR_DISP_10BIT);
            mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_wrap_mode = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
        }
        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_Pxl_Wrap_Ctrl_0_reg, mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.regValue);
        
        rtd_pr_scaler_memory_emerg("[Mdomain][Sub] pack_wrap_mode = %d, pack_dummy_format = %d, pack_data_format = %d\n",
            mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_wrap_mode,  mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_dummy_format, mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_data_format);

        // pack ctrl
        mdom_sub_cap_frc_cap1_pack_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_frc_cap1_pack_ctrl_reg);
        mdom_sub_cap_frc_cap1_pack_ctrl_reg.cap1_frame_acc_mode = (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME);
        mdom_sub_cap_frc_cap1_pack_ctrl_reg.cap1_vact = drvif_memory_get_data_align(CapLength, 2);
        mdom_sub_cap_frc_cap1_pack_ctrl_reg.cap1_hact = drvif_memory_get_data_align(CapWidth, 2);
        IoReg_Write32(M8P_MDOM_SUB_CAP_frc_cap1_pack_ctrl_reg, mdom_sub_cap_frc_cap1_pack_ctrl_reg.regValue);
    }
}

void memory_main_set_wdma(void)
{
	M8P_mdomain_cap_frc_cap0_num_bl_wrap_line_step_RBUS mdomain_cap_frc_cap0_num_bl_wrap_line_step_reg;
	M8P_mdomain_cap_frc_cap0_num_bl_wrap_ctl_RBUS mdomain_cap_frc_cap0_num_bl_wrap_ctl_reg;
	M8P_mdomain_cap_frc_cap0_num_bl_wrap_word_RBUS mdomain_cap_frc_cap0_num_bl_wrap_word_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
	unsigned int CapLength = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN);

	if(dvrif_memory_compression_get_enable(SLR_MAIN_DISPLAY) == TRUE)
	{
		if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE)
		{
			TotalSize = libdma_memory_get_capture_line_size_compression(SLR_MAIN_DISPLAY, dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY));//compression test
		}
		else{
			TotalSize = libdma_memory_get_capture_frame_size_compression(SLR_MAIN_DISPLAY, dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY));
		}
	} else
    {
        TotalSize = libdma_memory_get_capture_size(SLR_MAIN_DISPLAY);
    }

	// cap0_line_step
	if (dvrif_memory_get_cap_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE)
	{
		LineStep = TotalSize;
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_emerg("[Mdomain][Main] RDMA0 TotalSize = %d, LineStep = %d\n", TotalSize,  LineStep);

	mdomain_cap_frc_cap0_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_line_step_reg);
	mdomain_cap_frc_cap0_num_bl_wrap_line_step_reg.cap0_line_step = LineStep;
	IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_line_step_reg, mdomain_cap_frc_cap0_num_bl_wrap_line_step_reg.regValue);


	// cap0_burst_len & cap0_line_num
	mdomain_cap_frc_cap0_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_ctl_reg);
	mdomain_cap_frc_cap0_num_bl_wrap_ctl_reg.cap0_line_num = CapLength;
	mdomain_cap_frc_cap0_num_bl_wrap_ctl_reg.cap0_burst_len = _WDMA_BURSTLENGTH;
	IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_ctl_reg, mdomain_cap_frc_cap0_num_bl_wrap_ctl_reg.regValue);

	// line_burst_num
	mdomain_cap_frc_cap0_num_bl_wrap_word_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_word_reg);
	mdomain_cap_frc_cap0_num_bl_wrap_word_reg.cap0_addr_toggle_mode = 0;
	mdomain_cap_frc_cap0_num_bl_wrap_word_reg.cap0_line_burst_num = TotalSize;
	IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_word_reg, mdomain_cap_frc_cap0_num_bl_wrap_word_reg.regValue);

    // byte_swap
    memory_set_capture_byte_swap(SLR_MAIN_DISPLAY);

    // outstanding
    libdma_mcap_outstanding_ctrl(SLR_MAIN_DISPLAY, _ENABLE);

}

void memory_sub_set_wdma(void)
{
	M8P_mdom_sub_cap_frc_cap1_num_bl_wrap_line_step_RBUS mdom_sub_cap_frc_cap1_num_bl_wrap_line_step_reg;
	M8P_mdom_sub_cap_frc_cap1_num_bl_wrap_ctl_RBUS mdom_sub_cap_frc_cap1_num_bl_wrap_ctl_reg;
	M8P_mdom_sub_cap_frc_cap1_num_bl_wrap_word_RBUS mdom_sub_cap_frc_cap1_num_bl_wrap_word_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
	unsigned int CapLength = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_LEN);

    if(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
        {
            TotalSize = libdma_memory_get_capture_line_size_compression(SLR_SUB_DISPLAY, dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));//compression test
        }
        else{
            TotalSize = libdma_memory_get_capture_frame_size_compression(SLR_SUB_DISPLAY, dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));
        }
    } else {
        TotalSize = libdma_memory_get_capture_size(SLR_SUB_DISPLAY);
    }

	// cap0_line_step
	if (dvrif_memory_get_cap_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
	{
		LineStep = TotalSize;
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_emerg("[Mdomain][Sub] RDMA1 TotalSize = %d, LineStep = %d\n", TotalSize,  LineStep);

    mdom_sub_cap_frc_cap1_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_line_step_reg);
    mdom_sub_cap_frc_cap1_num_bl_wrap_line_step_reg.cap1_line_step = LineStep;
    IoReg_Write32(M8P_MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_line_step_reg, mdom_sub_cap_frc_cap1_num_bl_wrap_line_step_reg.regValue);

	// cap0_burst_len & cap0_line_num
    mdom_sub_cap_frc_cap1_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_ctl_reg);
    mdom_sub_cap_frc_cap1_num_bl_wrap_ctl_reg.cap1_line_num = CapLength;
    mdom_sub_cap_frc_cap1_num_bl_wrap_ctl_reg.cap1_burst_len = _WDMA_BURSTLENGTH;
    IoReg_Write32(M8P_MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_ctl_reg, mdom_sub_cap_frc_cap1_num_bl_wrap_ctl_reg.regValue);

	// line_burst_num
    mdom_sub_cap_frc_cap1_num_bl_wrap_word_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_word_reg);
    mdom_sub_cap_frc_cap1_num_bl_wrap_word_reg.cap1_addr_toggle_mode = 0;
    mdom_sub_cap_frc_cap1_num_bl_wrap_word_reg.cap1_line_burst_num = TotalSize;
    IoReg_Write32(M8P_MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_word_reg, mdom_sub_cap_frc_cap1_num_bl_wrap_word_reg.regValue);

    // byte_swap
    memory_set_capture_byte_swap(SLR_SUB_DISPLAY);

    // outstanding
    libdma_mcap_outstanding_ctrl(SLR_SUB_DISPLAY, _ENABLE);

}

void memory_main_set_wdma_c(void)
{
    M8P_mdomain_cap_frc_cap0c_num_bl_wrap_line_step_RBUS mdomain_cap_frc_cap0c_num_bl_wrap_line_step_reg;
    M8P_mdomain_cap_frc_cap0c_num_bl_wrap_ctl_RBUS mdomain_cap_frc_cap0c_num_bl_wrap_ctl_reg;
    M8P_mdomain_cap_frc_cap0c_num_bl_wrap_word_RBUS mdomain_cap_frc_cap0c_num_bl_wrap_word_reg;
    M8P_mdomain_cap_frc_cap0c_wr_ctrl_RBUS mdomain_cap_frc_cap0c_wr_ctrl_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
	unsigned int CapLength = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN);

    if(dvrif_memory_compression_get_enable(SLR_MAIN_DISPLAY) == TRUE)
	{
		if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE)
		{
			TotalSize = libdma_memory_get_capture_line_size_compression(SLR_MAIN_DISPLAY, dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY));//compression test
		}
		else{
			TotalSize = libdma_memory_get_capture_frame_size_compression(SLR_MAIN_DISPLAY, dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY));
		}
	} else
    {
        TotalSize = libdma_memory_get_capture_size(SLR_MAIN_DISPLAY);
    }

	// cap0_line_step
	if (dvrif_memory_get_cap_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE)
	{
		LineStep = TotalSize;
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_emerg("[Mdomain][Main] RDMA0C TotalSize = %d, LineStep = %d\n", TotalSize,  LineStep);

    mdomain_cap_frc_cap0c_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_line_step_reg);
    mdomain_cap_frc_cap0c_num_bl_wrap_line_step_reg.cap0c_line_step = LineStep;
    IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_line_step_reg, mdomain_cap_frc_cap0c_num_bl_wrap_line_step_reg.regValue);

	// cap0_burst_len & cap0_line_num
    mdomain_cap_frc_cap0c_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_ctl_reg);
    mdomain_cap_frc_cap0c_num_bl_wrap_ctl_reg.cap0c_line_num = CapLength;
    mdomain_cap_frc_cap0c_num_bl_wrap_ctl_reg.cap0c_burst_len = _WDMA_BURSTLENGTH;
    IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_ctl_reg, mdomain_cap_frc_cap0c_num_bl_wrap_ctl_reg.regValue);

	// line_burst_num
    mdomain_cap_frc_cap0c_num_bl_wrap_word_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_word_reg);
    mdomain_cap_frc_cap0c_num_bl_wrap_word_reg.cap0c_addr_toggle_mode = 0;
    mdomain_cap_frc_cap0c_num_bl_wrap_word_reg.cap0c_line_burst_num = TotalSize;
    IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_word_reg, mdomain_cap_frc_cap0c_num_bl_wrap_word_reg.regValue);

    // byte_swap
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
    if (Get_rotate_function(SLR_MAIN_DISPLAY)) {
        mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg);
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_8byte_swap = 1;
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_4byte_swap = 1;
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_2byte_swap = 1;
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_1byte_swap = 1;
        IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg, mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue);
    } else
#endif
#endif
    {
        mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg);
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_8byte_swap = 0;
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_4byte_swap = 0;
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_2byte_swap = 0;
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_1byte_swap = 0;
        IoReg_Write32(M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg, mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue);
    }


    // outstanding
    mcap_dmac_outstanding_ctrl(SLR_MAIN_DISPLAY, _ENABLE);

}

void memory_sub_set_wdma_c(void)
{
    M8P_mdom_sub_cap_frc_cap1c_num_bl_wrap_line_step_RBUS mdom_sub_cap_frc_cap1c_num_bl_wrap_line_step_reg;
    M8P_mdom_sub_cap_frc_cap1c_num_bl_wrap_ctl_RBUS mdom_sub_cap_frc_cap1c_num_bl_wrap_ctl_reg;
    M8P_mdom_sub_cap_frc_cap1c_num_bl_wrap_word_RBUS mdom_sub_cap_frc_cap1c_num_bl_wrap_word_reg;
    M8P_mdom_sub_cap_frc_cap1c_wr_ctrl_RBUS mdom_sub_cap_frc_cap1c_wr_ctrl_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
	unsigned int CapLength = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_LEN);

    if(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
        {
            TotalSize = libdma_memory_get_capture_line_size_compression(SLR_SUB_DISPLAY, dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));//compression test
        }
        else{
            TotalSize = libdma_memory_get_capture_frame_size_compression(SLR_SUB_DISPLAY, dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));
        }
    } else {
        TotalSize = libdma_memory_get_capture_size(SLR_SUB_DISPLAY);
    }

	// cap0_line_step
	if (dvrif_memory_get_cap_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
	{
		LineStep = TotalSize;
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_emerg("[Mdomain][Sub] RDMA1C TotalSize = %d, LineStep = %d\n", TotalSize,  LineStep);

    mdom_sub_cap_frc_cap1c_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_line_step_reg);
    mdom_sub_cap_frc_cap1c_num_bl_wrap_line_step_reg.cap1c_line_step = LineStep;
    IoReg_Write32(M8P_MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_line_step_reg, mdom_sub_cap_frc_cap1c_num_bl_wrap_line_step_reg.regValue);

	// cap0_burst_len & cap0_line_num
    mdom_sub_cap_frc_cap1c_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_ctl_reg);
    mdom_sub_cap_frc_cap1c_num_bl_wrap_ctl_reg.cap1c_line_num = CapLength;
    mdom_sub_cap_frc_cap1c_num_bl_wrap_ctl_reg.cap1c_burst_len = _WDMA_BURSTLENGTH;
    IoReg_Write32(M8P_MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_ctl_reg, mdom_sub_cap_frc_cap1c_num_bl_wrap_ctl_reg.regValue);

	// line_burst_num
    mdom_sub_cap_frc_cap1c_num_bl_wrap_word_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_word_reg);
    mdom_sub_cap_frc_cap1c_num_bl_wrap_word_reg.cap1c_addr_toggle_mode = 0;
    mdom_sub_cap_frc_cap1c_num_bl_wrap_word_reg.cap1c_line_burst_num = TotalSize;
    IoReg_Write32(M8P_MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_word_reg, mdom_sub_cap_frc_cap1c_num_bl_wrap_word_reg.regValue);

    // byte_swap
    mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg);
    mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_8byte_swap = 0;
    mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_4byte_swap = 0;
    mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_2byte_swap = 0;
    mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_1byte_swap = 0;
    IoReg_Write32(M8P_MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg, mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue);

    // outstanding
    mcap_dmac_outstanding_ctrl(SLR_SUB_DISPLAY, _ENABLE);

}

unsigned int memory_frc_main_decide_buffer_size(void)
{
	unsigned int buffer_size = 0;
    unsigned char display = SLR_MAIN_DISPLAY;
    if(dvrif_memory_compression_get_enable(display)==_ENABLE) {
		//RGB444 mode use 24bits @Crixus 20160902
		//K6 M-domain =13M, so only can use 16bit for PQC
		if (dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY)==COMPRESSION_16_BITS) {
			//buffer_size = MDOMAIN_MAIN_BUFFER_SIZE_24BITS;
				buffer_size = libdma_drv_get_caveout_buffer_size_by_platform(MDOMAIN_16BITS);
		} else {
				buffer_size = libdma_drv_get_caveout_buffer_size_by_platform(MDOMAIN_12BITS);
		}

		if(drv_memory_Get_multibuffer_flag()) {
			buffer_size = MDOMAIN_MAIN_BUFFER_SIZE_MULT4K_12BITS;
		}
	} else {/*qiangzhou:gamemode and livezoom not open compress mode now,
					so borrow di&rtnr memory,so video should not enter di&rtnr in this case*/
#ifndef BUILD_QUICK_SHOW
		if((display == SLR_MAIN_DISPLAY) && Get_rotate_function(SLR_MAIN_DISPLAY)) {
			buffer_size = libdma_drv_get_caveout_buffer_size_by_platform(MDOMAIN_12BITS);
		} else
#endif
		{
			buffer_size = libdma_drv_get_caveout_buffer_size_by_platform(MDOMAIN_16BITS);
		}
	}
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
    if (Get_rotate_function(SLR_MAIN_DISPLAY)){
        buffer_size = Get_Val_rotate_block_size();
    }
#endif
#endif

#ifdef CONFIG_MDOMAIN_FORCE_CARVEOUT
    buffer_size = MDOMAIN_CMP_LINE_MODE_BUFFER_SIZE(1920, 1080, 30);
#endif
 
    dvrif_memory_set_buffer_size(SLR_MAIN_DISPLAY, buffer_size);
    rtd_pr_scaler_memory_emerg("[Mdomain][Main] frc buffer size = %x\n", buffer_size);

	return buffer_size;
}

unsigned int memory_frc_sub_decide_buffer_size(void)
{
    unsigned int buffer_size = 0;
    //sub use cap max size @Crixus 20151231

    buffer_size = MDOMAIN_SUB_BUFFER_SIZE;

    dvrif_memory_set_buffer_size(SLR_SUB_DISPLAY, buffer_size);
    rtd_pr_scaler_memory_emerg("[Mdomain][Sub] frc buffer size = %x\n", buffer_size);

    return buffer_size;
}

void memory_frc_main_alloc_buffer(void)
{
    unsigned char display = SLR_MAIN_DISPLAY;
#ifdef CONFIG_ARM64
    unsigned long align_addr = 0, query_addr = 0;
#else
    unsigned int align_addr = 0, query_addr = 0;
#endif
#ifndef CONFIG_ALL_SOURCE_DATA_FS
	unsigned int buffer_size_page_align = 0;
#endif
    void *m_domain_borrow_buffer = NULL;
#ifdef BUILD_QUICK_SHOW
	unsigned long i3ddma_highest_phyaddr = 0;
#endif
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY);

    //merlin4 memory do 96 bytes align @Crixus 20180321
    query_addr = get_query_start_address(QUERY_IDX_MDOMAIN_MAIN);
    buffer_size = drvif_memory_get_data_align(buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    align_addr = dvr_memory_alignment((unsigned long)query_addr, buffer_size);
#ifdef CONFIG_ARM64
    if (align_addr == (unsigned long)NULL)
        BUG();
#else
    if (align_addr == (unsigned int)NULL)
        BUG();
#endif

    //Just for reserved memory, update to use query driver @Crixus 20170817

#ifdef CONFIG_RTK_KDRV_DV_IDK_DUMP
    MemTag[MEMIDX_MAIN].StartAddr = align_addr;
    MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    MemTag[MEMIDX_MAIN_THIRD].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
#else
    MemTag[MEMIDX_MAIN_THIRD].StartAddr = align_addr;
    if((is_scaler_input_2p_mode(SLR_MAIN_DISPLAY)) && (get_force_i3ddma_enable(SLR_MAIN_DISPLAY) == FALSE))
    {
        if(i3ddmaCtrl.cap_buffer[0].phyaddr)
        {
            MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(i3ddmaCtrl.cap_buffer[0].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);//do not change order
            MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(i3ddmaCtrl.cap_buffer[0].phyaddr + buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
            MemTag[MEMIDX_MAIN_THIRD].StartAddr = drvif_memory_get_data_align(i3ddmaCtrl.cap_buffer[0].phyaddr + buffer_size*2, DMA_SPEEDUP_ALIGN_VALUE);

            if(carvedout_enable() == 0) // do NOT use carvedout memory, use CMA malloc
                CMA_reset_DCMT(MemTag[MEMIDX_MAIN].StartAddr, (MemTag[MEMIDX_MAIN_THIRD].StartAddr + buffer_size - MemTag[MEMIDX_MAIN].StartAddr), ADDITIONAL_CMA_DCMT_I3DDMA);
        }
        else
        {
#ifndef BUILD_QUICK_SHOW
            rtd_pr_scaler_memory_err("##[ERR] 4k 120 no memory##\r\n");
#else
            printf2("1 main modomain no memory\n");
#endif
            set_m_domain_setting_err_status(display, MDOMAIN_SETTING_ERR_MAIN_4K120_NO_MEM); // set error status and don't enable Mcap_en later
            return;
        }
    } else if (drv_memory_Get_multibuffer_flag()){
        rtd_pr_scaler_memory_info("[VideoDelay] multibuffer update mem tag\n");
        MemTag[MEMIDX_MAIN_EIGHTH].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[7].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_SEVENTH].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[6].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_SIXTH].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[5].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_FIFTH].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[4].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_FOURTH].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[3].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_THIRD].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[2].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[1].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[0].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
    }
    else if(
#ifndef BUILD_QUICK_SHOW
        is_m_domain_data_frc_need_borrow_memory() &&
#else
        judge_scaler_run_i3ddma_vo_path() ||
#endif
        (m_domain_4k_memory_from_type() == BORROW_FROM_VBM))
    {
        if(i3ddmaCtrl.cap_buffer[0].phyaddr)
        {
#ifdef BUILD_QUICK_SHOW
            if(judge_scaler_run_i3ddma_vo_path()){
                //i3 use vbm first half buffer , m-domain use second half buffer
                i3ddma_highest_phyaddr = rtd_inl(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_1_reg);
    MemTag[MEMIDX_MAIN_THIRD].StartAddr = drvif_memory_get_data_align(i3ddma_highest_phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
    MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(i3ddma_highest_phyaddr + buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(i3ddma_highest_phyaddr + buffer_size*2, DMA_SPEEDUP_ALIGN_VALUE);
            }else
#endif
            {
                MemTag[MEMIDX_MAIN_THIRD].StartAddr = drvif_memory_get_data_align(i3ddmaCtrl.cap_buffer[0].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
                MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(i3ddmaCtrl.cap_buffer[0].phyaddr + buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
                MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(i3ddmaCtrl.cap_buffer[0].phyaddr + buffer_size*2, DMA_SPEEDUP_ALIGN_VALUE);
            }

            if(carvedout_enable() == 0) // do NOT use carvedout memory, use CMA malloc
                CMA_reset_DCMT(MemTag[MEMIDX_MAIN_THIRD].StartAddr, (MemTag[MEMIDX_MAIN].StartAddr + buffer_size - MemTag[MEMIDX_MAIN_THIRD].StartAddr), ADDITIONAL_CMA_DCMT_I3DDMA);
        }
        else
        {
#ifndef BUILD_QUICK_SHOW
            rtd_pr_scaler_memory_err("##[ERR] 4k no memory cap##\r\n");
#else
            printf2("2 main modomain no memory\n");
#endif
            set_m_domain_setting_err_status(display, MDOMAIN_SETTING_ERR_MAIN_NO_MEM); // set error status and don't enable Mcap_en later
            return;
        }
    }
    else
    {
#ifndef BUILD_QUICK_SHOW
        if ((vdec_data_frc_need_borrow_cma_buffer(display)) || (is_m_domain_data_frc_need_borrow_memory() && m_domain_4k_memory_from_type() == BORROW_FROM_CMA)) {//4k frc and need borrow memory from cma
            unsigned long phy_addr_buffer_memory;
            unsigned char borrow_memory = TRUE;//need borrow memory from cma
            unsigned int borrow_size;//borrow size

            if(vdec_data_frc_need_borrow_cma_buffer(display)){
                borrow_size =  buffer_size;// borrow 1 buffer
                if(is_scaler_input_2p_mode(display))
                    borrow_size =  buffer_size * 2;
            }else{
                borrow_size = 2 * buffer_size;
            }

            rtd_pr_scaler_memory_notice("##func:%s main m domain borrow memory from CMA size:0x%x ###\r\n", __FUNCTION__, borrow_size);
            if(get_m_domain_borrow_buffer())
            {//alread has size from cma
                if(borrow_size <= get_m_domain_borrow_size())
                {//no need to borrow again
                    borrow_memory = FALSE;
                    m_domain_borrow_buffer = get_m_domain_borrow_buffer();//get original addr
                    phy_addr_buffer_memory = dvr_to_phys(m_domain_borrow_buffer);
                    if(vdec_data_frc_need_borrow_cma_buffer(display)){
                        MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(phy_addr_buffer_memory, DMA_SPEEDUP_ALIGN_VALUE);
                    }else{
                        MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(phy_addr_buffer_memory, DMA_SPEEDUP_ALIGN_VALUE);
                    }
                }
                else
                    set_m_domain_borrow_buffer(NULL, 0);//free memory before borrow big size
            }
            if(borrow_memory)
            {//need borrrow memory from cam
                m_domain_borrow_buffer = dvr_malloc(borrow_size);
                if(m_domain_borrow_buffer)
                {
                    phy_addr_buffer_memory = dvr_to_phys(m_domain_borrow_buffer);
                    set_m_domain_borrow_buffer(m_domain_borrow_buffer, borrow_size);//save addr and size
                    if(vdec_data_frc_need_borrow_cma_buffer(display)){
                        MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(phy_addr_buffer_memory, DMA_SPEEDUP_ALIGN_VALUE);
                    }else{
                        MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(phy_addr_buffer_memory, DMA_SPEEDUP_ALIGN_VALUE);
                    }
                }
                else
                {
                    rtd_pr_scaler_memory_err("###[Bug] func:%s %d no memory from cma ##\r\n",__FUNCTION__, __LINE__);
                    set_m_domain_setting_err_status(display, MDOMAIN_SETTING_ERR_MAIN_CMA_ALLOC_FAIL); // set error status and don't enable Mcap_en later
                    return;
                }
            }

            if(carvedout_enable() == 0) // do NOT use carvedout memory, use CMA malloc
                CMA_reset_DCMT(phy_addr_buffer_memory, borrow_size, ADDITIONAL_CMA_DCMT_VDEC_DATA_FRC_OR_MDOMAIN_CMA);
        } else
#endif
        {
            if((is_m_domain_data_frc_need_borrow_memory() && m_domain_4k_memory_from_type() == BORROW_FROM_MEMC) && (carvedout_enable() == 0))  // do NOT use carvedout memory, use CMA malloc
            {
                unsigned long addr_2 = 0;
                unsigned int size_2 = 0;

                if(CMA_borrow_m_domain_buffer_from_MEMC(MEMIDX_MDOMAIN_BORROW_MEMC_BUFFER_INDEX_2, &addr_2, &size_2) == TRUE)
                {
                    rtd_pr_scaler_memory_emerg("[borrow memory] HDMI VRR  m-domain use MEMC CMA index=2 size=0x%x, addr=0x%lx\n", size_2, addr_2);
                    addr_2 = dvr_memory_alignment(addr_2, buffer_size);

                    MemTag[MEMIDX_MAIN_SEC].StartAddr = addr_2;
                }
                else
                {
                    addr_2 = 0;
                    set_m_domain_setting_err_status(SLR_MAIN_DISPLAY, MDOMAIN_SETTING_ERR_MAIN_CMA_BORROW_FROM_MEMC_FAIL); // set error status and don't enable Mcap_en later
                    rtd_pr_scaler_memory_err("###[Bug] func:%s %d no memory borrow from MEMC CMA index=2 ##\n", __func__, __LINE__);
                    return;
                }
            }
            else  // use carvedout memory
                MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
        }
        if(vdec_data_frc_need_borrow_cma_buffer(display)){
            MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
        }else{
            if((is_m_domain_data_frc_need_borrow_memory() && m_domain_4k_memory_from_type() == BORROW_FROM_MEMC) && (carvedout_enable() == 0))  // do NOT use carvedout memory, use CMA malloc
            {
                unsigned long addr_1 = 0;
                unsigned int size_1 = 0;

                if(CMA_borrow_m_domain_buffer_from_MEMC(MEMIDX_MDOMAIN_BORROW_MEMC_BUFFER_INDEX_1, &addr_1, &size_1) == TRUE)
                {
                    rtd_pr_scaler_memory_emerg("[borrow memory] HDMI VRR  m-domain use MEMC CMA index=1 size=0x%x, addr=0x%lx\n", size_1, addr_1);
                    addr_1 = dvr_memory_alignment(addr_1, buffer_size);

                    MemTag[MEMIDX_MAIN].StartAddr = addr_1;
                }
                else
                {
                    addr_1 = 0;
                    set_m_domain_setting_err_status(SLR_MAIN_DISPLAY, MDOMAIN_SETTING_ERR_MAIN_CMA_BORROW_FROM_MEMC_FAIL); // set error status and don't enable Mcap_en later
                    rtd_pr_scaler_memory_err("###[Bug] func:%s %d no memory borrow from MEMC CMA index=1 ##\n", __func__, __LINE__);
                    return;
                }
            }
            else  // use carvedout memory
                MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
        }
    }
#endif
    //16 bytes alignment
    if(drv_memory_Get_multibuffer_flag())
    {
        MemTag[MEMIDX_MAIN_EIGHTH].StartAddr = MemTag[MEMIDX_MAIN_EIGHTH].StartAddr & 0xfffffff0;
        MemTag[MEMIDX_MAIN_SEVENTH].StartAddr = MemTag[MEMIDX_MAIN_SEVENTH].StartAddr & 0xfffffff0;
        MemTag[MEMIDX_MAIN_SIXTH].StartAddr = MemTag[MEMIDX_MAIN_SIXTH].StartAddr & 0xfffffff0;
        MemTag[MEMIDX_MAIN_FIFTH].StartAddr = MemTag[MEMIDX_MAIN_FIFTH].StartAddr & 0xfffffff0;
        MemTag[MEMIDX_MAIN_FOURTH].StartAddr = MemTag[MEMIDX_MAIN_FOURTH].StartAddr & 0xfffffff0;
    }
    MemTag[MEMIDX_MAIN_THIRD].StartAddr = MemTag[MEMIDX_MAIN_THIRD].StartAddr & 0xfffffff0;
    MemTag[MEMIDX_MAIN_SEC].StartAddr = MemTag[MEMIDX_MAIN_SEC].StartAddr& 0xfffffff0;
    MemTag[MEMIDX_MAIN].StartAddr = MemTag[MEMIDX_MAIN].StartAddr & 0xfffffff0;

    //free virtual memory
    //drvif_memory_free_fixed_virtual_memory();
    //set virtual memory size
    //rtd_pr_scaler_memory_emerg("buffer_size=%x\n", buffer_size);
    buffer_size_page_align = PAGE_ALIGN(buffer_size);
    //buffer_size = buffer_size_page_align;
    rtd_pr_scaler_memory_emerg("memory_set_capture PAGE_ALIGN(buffer_size)=%x\n", PAGE_ALIGN(buffer_size));
    drvif_memory_set_fixed_virtual_memory_size(buffer_size_page_align);
    MemTag[MEMIDX_MAIN_THIRD].Allocate_StartAddr = MemTag[MEMIDX_MAIN_THIRD].StartAddr;
    MemTag[MEMIDX_MAIN_THIRD].Size = buffer_size;
    MemTag[MEMIDX_MAIN_THIRD].Status = ALLOCATED_FROM_PLI;

    MemTag[MEMIDX_MAIN_SEC].Allocate_StartAddr = MemTag[MEMIDX_MAIN_SEC].StartAddr;
    MemTag[MEMIDX_MAIN_SEC].Size = buffer_size;
    MemTag[MEMIDX_MAIN_SEC].Status = ALLOCATED_FROM_PLI;
    MemTag[MEMIDX_MAIN_SEC].Allocate_VirAddr = 0;//not use

    MemTag[MEMIDX_MAIN].Allocate_StartAddr = MemTag[MEMIDX_MAIN].StartAddr;
    MemTag[MEMIDX_MAIN].Size = buffer_size;
    MemTag[MEMIDX_MAIN].Status = ALLOCATED_FROM_PLI;
    MemTag[MEMIDX_MAIN].Allocate_VirAddr = 0;//not use
}

void memory_frc_sub_alloc_buffer(void)
{
#ifdef ENABLE_3_BUFFER_DELAY_MODE
	void *sub_3rd_buffer;
#endif
	void *sub_2nd_buffer;
	void *sub_1st_buffer;
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY);
    unsigned char display = SLR_SUB_DISPLAY;

    if((is_scaler_input_2p_mode(SLR_MAIN_DISPLAY)) && (get_force_i3ddma_enable(SLR_MAIN_DISPLAY) == FALSE)) {
        if(i3ddmaCtrl.cap_buffer[0].phyaddr)
        {
            //QS hfr mode borrow i3 buffer
            MemTag[MEMIDX_SUB_THIRD].StartAddr = drvif_memory_get_data_align(i3ddmaCtrl.cap_buffer[0].phyaddr + buffer_size*3, DMA_SPEEDUP_ALIGN_VALUE);
            MemTag[MEMIDX_SUB_SEC].StartAddr = drvif_memory_get_data_align(i3ddmaCtrl.cap_buffer[0].phyaddr + buffer_size*4, DMA_SPEEDUP_ALIGN_VALUE);
            MemTag[MEMIDX_SUB].StartAddr = drvif_memory_get_data_align(i3ddmaCtrl.cap_buffer[0].phyaddr + buffer_size*5, DMA_SPEEDUP_ALIGN_VALUE);
            rtd_pr_scaler_memory_emerg("%s sub borrow memory from i3 size:0x%x,addr:0x%x\n", __FUNCTION__,buffer_size, MemTag[MEMIDX_SUB_THIRD].StartAddr);
        } else {
            rtd_pr_scaler_memory_emerg("4k 120 no i3 memory for sub\n");
            set_m_domain_setting_err_status(display, MDOMAIN_SETTING_ERR_SUB_4K120_NO_MEM); // set error status and don't enable Mcap_en later
            return;
        }
    } else {
#ifndef BUILD_QUICK_SHOW
#ifdef ENABLE_3_BUFFER_DELAY_MODE
        //I2rnd mode s-cap use 2-buffer
        sub_3rd_buffer = get_sub_m_domain_3rd_buf_address();
        if (sub_3rd_buffer != NULL)
            dvr_free(sub_3rd_buffer);

        sub_3rd_buffer = dvr_malloc(buffer_size);
        set_sub_m_domain_3rd_buf_address(sub_3rd_buffer);
        MemTag[MEMIDX_SUB_THIRD].StartAddr = dvr_to_phys(sub_3rd_buffer);
        rtd_pr_scaler_memory_info("set M-domain sub 3rd buffer 0x%px paddr 0x%08X, size=0x%x\n", sub_3rd_buffer, MemTag[MEMIDX_SUB_THIRD].StartAddr, buffer_size);
    //}
#endif // #ifdef ENABLE_3_BUFFER_DELAY_MODE

        sub_2nd_buffer = get_sub_m_domain_2nd_buf_address();
        if (sub_2nd_buffer != NULL)
            dvr_free(sub_2nd_buffer);

        sub_2nd_buffer = dvr_malloc(buffer_size);
        set_sub_m_domain_2nd_buf_address(sub_2nd_buffer);
        MemTag[MEMIDX_SUB_SEC].StartAddr = dvr_to_phys(sub_2nd_buffer);
        rtd_pr_scaler_memory_info("set M-domain sub 2nd buffer 0x%px paddr 0x%08X, size=0x%x\n", sub_2nd_buffer, MemTag[MEMIDX_SUB_SEC].StartAddr, buffer_size);

        sub_1st_buffer = get_sub_m_domain_1st_buf_address();
        if (sub_1st_buffer != NULL)
            dvr_free(sub_1st_buffer);

        sub_1st_buffer = dvr_malloc(buffer_size);
        set_sub_m_domain_1st_buf_address(sub_1st_buffer);
        MemTag[MEMIDX_SUB].StartAddr = dvr_to_phys(sub_1st_buffer);
        rtd_pr_scaler_memory_info("set M-domain sub 1st buffer 0x%px paddr 0x%08X, size=0x%x\n", sub_1st_buffer, MemTag[MEMIDX_SUB].StartAddr, buffer_size);
#else // #ifndef BUILD_QUICK_SHOW
        unsigned long startAddr = 0;
        startAddr = libdma_qs_get_memory_from_vbm(MDOMAIN_SUB, buffer_size * 3);
        if(startAddr != 0)
        {
            set_sub_m_domain_3rd_buf_address((void *)startAddr);
            MemTag[MEMIDX_SUB_THIRD].StartAddr = startAddr;
            set_sub_m_domain_2nd_buf_address((void *)(startAddr + buffer_size));
            MemTag[MEMIDX_SUB_SEC].StartAddr = startAddr + buffer_size;
            MemTag[MEMIDX_SUB].StartAddr = startAddr + buffer_size * 2;
            rtd_pr_scaler_memory_info("qs mdomain sub 3 buffer 3rd:0x%08x, 2nd:0x%08x, 1st:0x%08x, startAddr:0x%08x, needSize:0x%08x\n", MemTag[MEMIDX_SUB_THIRD].StartAddr, MemTag[MEMIDX_SUB_SEC].StartAddr, MemTag[MEMIDX_SUB].StartAddr, startAddr, buffer_size * 3);
        }
        else
        {
            printf2("sub modomain no memory\n");
        }
#endif // #ifndef BUILD_QUICK_SHOW

    }

}

void memory_main_update_capture_buffer(void)
{
    unsigned char cap_num = dvrif_memory_get_buffer_number(SLR_MAIN_DISPLAY);
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY);

    rtd_pr_scaler_memory_emerg("[Mdomain]update mcap buffer number = %d\n", cap_num);

    //M-domain start address
    if(cap_num > 0){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0_first_buf_addr_reg, MemTag[MEMIDX_MAIN].StartAddr);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_downlimit_0_reg, (MemTag[MEMIDX_MAIN].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_uplimit_0_reg, (MemTag[MEMIDX_MAIN].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN].StartAddr = %lx\n", MemTag[MEMIDX_MAIN].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN].StartAddr = %x\n", MemTag[MEMIDX_MAIN].StartAddr);
    #endif
    }

    if(cap_num > 1){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0_second_buf_addr_reg, MemTag[MEMIDX_MAIN_SEC].StartAddr);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_downlimit_1_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_uplimit_1_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_SEC].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_SEC].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_SEC].StartAddr = %x\n", MemTag[MEMIDX_MAIN_SEC].StartAddr);
    #endif
    }

    if(cap_num > 2){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0_third_buf_addr_reg, MemTag[MEMIDX_MAIN_THIRD].StartAddr);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_downlimit_2_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_uplimit_2_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_THIRD].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_THIRD].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_THIRD].StartAddr = %x\n", MemTag[MEMIDX_MAIN_THIRD].StartAddr);
    #endif
    }

    if(cap_num > 3){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0_forth_buf_addr_reg, MemTag[MEMIDX_MAIN_FOURTH].StartAddr);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_downlimit_3_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_uplimit_3_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_FOURTH].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_FOURTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_FOURTH].StartAddr = %x\n", MemTag[MEMIDX_MAIN_FOURTH].StartAddr);
    #endif
    }

    if(cap_num > 4){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0_fifth_buf_addr_reg, MemTag[MEMIDX_MAIN_FIFTH].StartAddr);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_downlimit_4_reg, (MemTag[MEMIDX_MAIN_FIFTH].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_uplimit_4_reg, (MemTag[MEMIDX_MAIN_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_FIFTH].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_FIFTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_FIFTH].StartAddr = %x\n", MemTag[MEMIDX_MAIN_FIFTH].StartAddr);
    #endif
    }

    if(cap_num > 5){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0_sixth_buf_addr_reg, MemTag[MEMIDX_MAIN_SIXTH].StartAddr);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_downlimit_5_reg, (MemTag[MEMIDX_MAIN_SIXTH].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_uplimit_5_reg, (MemTag[MEMIDX_MAIN_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_SIXTH].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_SIXTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_SIXTH].StartAddr = %x\n", MemTag[MEMIDX_MAIN_SIXTH].StartAddr);
    #endif
    }

    if(cap_num > 6){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0_seventh_buf_addr_reg, MemTag[MEMIDX_MAIN_SEVENTH].StartAddr);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_downlimit_6_reg, (MemTag[MEMIDX_MAIN_SEVENTH].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_uplimit_6_reg, (MemTag[MEMIDX_MAIN_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_SEVENTH].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_SEVENTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_SEVENTH].StartAddr = %x\n", MemTag[MEMIDX_MAIN_SEVENTH].StartAddr);
    #endif
    }

    if(cap_num > 7){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0_eightth_buf_addr_reg, MemTag[MEMIDX_MAIN_EIGHTH].StartAddr);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_downlimit_7_reg, (MemTag[MEMIDX_MAIN_EIGHTH].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_uplimit_7_reg, (MemTag[MEMIDX_MAIN_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_EIGHTH].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_EIGHTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_EIGHTH].StartAddr = %x\n", MemTag[MEMIDX_MAIN_EIGHTH].StartAddr);
    #endif
    }
}

void memory_sub_update_capture_buffer(void)
{
    unsigned char cap_num = dvrif_memory_get_buffer_number(SLR_SUB_DISPLAY);
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY);

    if(cap_num > 0){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_first_buf_addr_reg, MemTag[MEMIDX_SUB].StartAddr);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_downlimit_0_reg, (MemTag[MEMIDX_SUB].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_uplimit_0_reg, (MemTag[MEMIDX_SUB].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB].StartAddr = %lx\n", MemTag[MEMIDX_SUB].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB].StartAddr = %x\n", MemTag[MEMIDX_SUB].StartAddr);
    #endif
    }

    if(cap_num > 1){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_second_buf_addr_reg, MemTag[MEMIDX_SUB_SEC].StartAddr);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_downlimit_1_reg, (MemTag[MEMIDX_SUB_SEC].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_uplimit_1_reg, (MemTag[MEMIDX_SUB_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_SEC].StartAddr = %lx\n", MemTag[MEMIDX_SUB_SEC].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_SEC].StartAddr = %x\n", MemTag[MEMIDX_SUB_SEC].StartAddr);
    #endif
    }

    if(cap_num > 2){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_third_buf_addr_reg, MemTag[MEMIDX_SUB_THIRD].StartAddr);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_downlimit_2_reg, (MemTag[MEMIDX_SUB_THIRD].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_uplimit_2_reg, (MemTag[MEMIDX_SUB_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_THIRD].StartAddr = %lx\n", MemTag[MEMIDX_SUB_THIRD].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_THIRD].StartAddr = %x\n", MemTag[MEMIDX_SUB_THIRD].StartAddr);
    #endif
    }

    if(cap_num > 3){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_forth_buf_addr_reg, MemTag[MEMIDX_SUB_FOURTH].StartAddr);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_downlimit_3_reg, (MemTag[MEMIDX_SUB_FOURTH].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_uplimit_3_reg, (MemTag[MEMIDX_SUB_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_FOURTH].StartAddr = %lx\n", MemTag[MEMIDX_SUB_FOURTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_FOURTH].StartAddr = %x\n", MemTag[MEMIDX_SUB_FOURTH].StartAddr);
    #endif
    }

    if(cap_num > 4){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_fifth_buf_addr_reg, MemTag[MEMIDX_SUB_FIFTH].StartAddr);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_downlimit_4_reg, (MemTag[MEMIDX_SUB_FIFTH].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_uplimit_4_reg, (MemTag[MEMIDX_SUB_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_FIFTH].StartAddr = %lx\n", MemTag[MEMIDX_SUB_FIFTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_FIFTH].StartAddr = %x\n", MemTag[MEMIDX_SUB_FIFTH].StartAddr);
    #endif
    }

    if(cap_num > 5){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_sixth_buf_addr_reg, MemTag[MEMIDX_SUB_SIXTH].StartAddr);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_downlimit_5_reg, (MemTag[MEMIDX_SUB_SIXTH].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_uplimit_5_reg, (MemTag[MEMIDX_SUB_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_SIXTH].StartAddr = %lx\n", MemTag[MEMIDX_SUB_SIXTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_SIXTH].StartAddr = %x\n", MemTag[MEMIDX_SUB_SIXTH].StartAddr);
    #endif
    }

    if(cap_num > 6){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_seventh_buf_addr_reg, MemTag[MEMIDX_SUB_SEVENTH].StartAddr);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_downlimit_6_reg, (MemTag[MEMIDX_SUB_SEVENTH].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_uplimit_6_reg, (MemTag[MEMIDX_SUB_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_SEVENTH].StartAddr = %lx\n", MemTag[MEMIDX_SUB_SEVENTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_SEVENTH].StartAddr = %x\n", MemTag[MEMIDX_SUB_SEVENTH].StartAddr);
    #endif
    }

    if(cap_num > 7){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1_eightth_buf_addr_reg, MemTag[MEMIDX_SUB_EIGHTH].StartAddr);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_downlimit_7_reg, (MemTag[MEMIDX_SUB_EIGHTH].StartAddr & 0xfffffff0));
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_uplimit_7_reg, (MemTag[MEMIDX_SUB_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_EIGHTH].StartAddr = %lx\n", MemTag[MEMIDX_SUB_EIGHTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_EIGHTH].StartAddr = %x\n", MemTag[MEMIDX_SUB_EIGHTH].StartAddr);
    #endif
    }

}

void memory_main_update_capture_C_buffer(unsigned char cap_num, unsigned int buffer_size)
{
    if(cap_num > 0){

    IoReg_Write32(M8P_MDOMAIN_CAP_cap0c_first_buf_addr_reg, (MemTag[MEMIDX_MAIN_C].StartAddr) & 0xfffffff0);

    IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_downlimit_0_reg, (MemTag[MEMIDX_MAIN_C].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_uplimit_0_reg, (MemTag[MEMIDX_MAIN_C].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 1){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0c_second_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_downlimit_1_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_uplimit_1_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0c_third_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_downlimit_2_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_uplimit_2_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0c_forth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_downlimit_3_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_uplimit_3_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0c_fifth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_FIFTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_downlimit_4_reg, (MemTag[MEMIDX_MAIN_C_FIFTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_uplimit_4_reg, (MemTag[MEMIDX_MAIN_C_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0c_sixth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SIXTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_downlimit_5_reg, (MemTag[MEMIDX_MAIN_C_SIXTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_uplimit_5_reg, (MemTag[MEMIDX_MAIN_C_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0c_seventh_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SEVENTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_downlimit_6_reg, (MemTag[MEMIDX_MAIN_C_SEVENTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_uplimit_6_reg, (MemTag[MEMIDX_MAIN_C_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(M8P_MDOMAIN_CAP_cap0c_eightth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_EIGHTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_downlimit_7_reg, (MemTag[MEMIDX_MAIN_C_EIGHTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0c_uplimit_7_reg, (MemTag[MEMIDX_MAIN_C_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

}

void memory_sub_update_capture_C_buffer(unsigned char cap_num, unsigned int buffer_size)
{
    if(cap_num > 0){

    IoReg_Write32(M8P_MDOM_SUB_CAP_cap1c_first_buf_addr_reg, (MemTag[MEMIDX_SUB_C].StartAddr) & 0xfffffff0);

    IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_downlimit_0_reg, (MemTag[MEMIDX_SUB_C].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_uplimit_0_reg, (MemTag[MEMIDX_SUB_C].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 1){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1c_second_buf_addr_reg, (MemTag[MEMIDX_SUB_C_SEC].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_downlimit_1_reg, (MemTag[MEMIDX_SUB_C_SEC].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_uplimit_1_reg, (MemTag[MEMIDX_SUB_C_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1c_third_buf_addr_reg, (MemTag[MEMIDX_SUB_C_THIRD].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_downlimit_2_reg, (MemTag[MEMIDX_SUB_C_THIRD].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_uplimit_2_reg, (MemTag[MEMIDX_SUB_C_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1c_forth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_FOURTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_downlimit_3_reg, (MemTag[MEMIDX_SUB_C_FOURTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_uplimit_3_reg, (MemTag[MEMIDX_SUB_C_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1c_fifth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_FIFTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_downlimit_4_reg, (MemTag[MEMIDX_SUB_C_FIFTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_uplimit_4_reg, (MemTag[MEMIDX_SUB_C_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1c_sixth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_SIXTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_downlimit_5_reg, (MemTag[MEMIDX_SUB_C_SIXTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_uplimit_5_reg, (MemTag[MEMIDX_SUB_C_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1c_seventh_buf_addr_reg, (MemTag[MEMIDX_SUB_C_SEVENTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_downlimit_6_reg, (MemTag[MEMIDX_SUB_C_SEVENTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_uplimit_6_reg, (MemTag[MEMIDX_SUB_C_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(M8P_MDOM_SUB_CAP_cap1c_eightth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_EIGHTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_downlimit_7_reg, (MemTag[MEMIDX_SUB_C_EIGHTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1c_uplimit_7_reg, (MemTag[MEMIDX_SUB_C_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

}

void memory_main_update_display_C_buffer(unsigned char cap_num, unsigned int buffer_size)
{
    if(cap_num > 0){

    IoReg_Write32(M8P_MDOMAIN_DISP_disp0c_first_buf_addr_reg, (MemTag[MEMIDX_MAIN_C].StartAddr) & 0xfffffff0);

    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_downlimit_0_reg, (MemTag[MEMIDX_MAIN_C].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_uplimit_0_reg, (MemTag[MEMIDX_MAIN_C].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 1){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0c_second_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_downlimit_1_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_uplimit_1_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0c_third_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_downlimit_2_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_uplimit_2_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0c_forth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_downlimit_3_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_uplimit_3_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0c_fifth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_FIFTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_downlimit_4_reg, (MemTag[MEMIDX_MAIN_C_FIFTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_uplimit_4_reg, (MemTag[MEMIDX_MAIN_C_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0c_sixth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SIXTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_downlimit_5_reg, (MemTag[MEMIDX_MAIN_C_SIXTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_uplimit_5_reg, (MemTag[MEMIDX_MAIN_C_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0c_seventh_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SEVENTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_downlimit_6_reg, (MemTag[MEMIDX_MAIN_C_SEVENTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_uplimit_6_reg, (MemTag[MEMIDX_MAIN_C_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0c_eightth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_EIGHTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_downlimit_7_reg, (MemTag[MEMIDX_MAIN_C_EIGHTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_uplimit_7_reg, (MemTag[MEMIDX_MAIN_C_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

}

void memory_sub_update_display_C_buffer(unsigned char cap_num, unsigned int buffer_size)
{
    if(cap_num > 0){

    IoReg_Write32(M8P_MDOM_SUB_DISP_disp1c_first_buf_addr_reg, (MemTag[MEMIDX_SUB_C].StartAddr) & 0xfffffff0);

    IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_downlimit_0_reg, (MemTag[MEMIDX_SUB_C].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_uplimit_0_reg, (MemTag[MEMIDX_SUB_C].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 1){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1c_second_buf_addr_reg, (MemTag[MEMIDX_SUB_C_SEC].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_downlimit_1_reg, (MemTag[MEMIDX_SUB_C_SEC].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_uplimit_1_reg, (MemTag[MEMIDX_SUB_C_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1c_third_buf_addr_reg, (MemTag[MEMIDX_SUB_C_THIRD].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_downlimit_2_reg, (MemTag[MEMIDX_SUB_C_THIRD].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_uplimit_2_reg, (MemTag[MEMIDX_SUB_C_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1c_forth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_FOURTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_downlimit_3_reg, (MemTag[MEMIDX_SUB_C_FOURTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_uplimit_3_reg, (MemTag[MEMIDX_SUB_C_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1c_fifth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_FIFTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_downlimit_4_reg, (MemTag[MEMIDX_SUB_C_FIFTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_uplimit_4_reg, (MemTag[MEMIDX_SUB_C_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1c_sixth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_SIXTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_downlimit_5_reg, (MemTag[MEMIDX_SUB_C_SIXTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_uplimit_5_reg, (MemTag[MEMIDX_SUB_C_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1c_seventh_buf_addr_reg, (MemTag[MEMIDX_SUB_C_SEVENTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_downlimit_6_reg, (MemTag[MEMIDX_SUB_C_SEVENTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_uplimit_6_reg, (MemTag[MEMIDX_SUB_C_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1c_eightth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_EIGHTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_downlimit_7_reg, (MemTag[MEMIDX_SUB_C_EIGHTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1c_uplimit_7_reg, (MemTag[MEMIDX_SUB_C_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

}

#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
void rotate_update_display_buffer_limit(void)
{
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY);

    IoReg_Write32(M8P_MDOMAIN_DISP_disp0_first_buf_addr_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_downlimit_0_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_uplimit_0_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);//need cut burstLen

    IoReg_Write32(M8P_MDOMAIN_DISP_disp0_second_buf_addr_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_downlimit_1_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_uplimit_1_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);//need cut burstLen

    IoReg_Write32(M8P_MDOMAIN_DISP_disp0_third_buf_addr_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_downlimit_2_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_uplimit_2_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);//need cut burstLen


    IoReg_Write32(M8P_MDOMAIN_DISP_disp0c_first_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_downlimit_0_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_uplimit_0_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    IoReg_Write32(M8P_MDOMAIN_DISP_disp0c_second_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_downlimit_1_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_uplimit_1_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    IoReg_Write32(M8P_MDOMAIN_DISP_disp0c_third_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_downlimit_2_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);
    IoReg_Write32(M8P_MDOMAIN_DISP_Disp0c_uplimit_2_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

}
#endif

void memory_frc_set_capture_block_control(unsigned char display)
{
    if (display == SLR_MAIN_DISPLAY)
    {
        M8P_mdomain_cap_cap0_buf_control_RBUS mdomain_cap_cap0_buf_control_reg;
        M8P_mdomain_cap_cap0_buffer_change_line_num_0_RBUS mdomain_cap_cap0_buffer_change_line_num_0_reg;

        mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg);
        mdomain_cap_cap0_buf_control_reg.cap0_buf_num = (dvrif_memory_get_buffer_number(display) == 0) ? 0 : (dvrif_memory_get_buffer_number(display) - 1);
        mdomain_cap_cap0_buf_control_reg.cap0_buf_distance = 0;
        mdomain_cap_cap0_buf_control_reg.cap0_buf_force_en = 0;
        mdomain_cap_cap0_buf_control_reg.cap0_buf_force_addr = 0;
        mdomain_cap_cap0_buf_control_reg.cap0_buf_freeze_en = 0;
        mdomain_cap_cap0_buf_control_reg.cap0_buf_change_mode = 0;
        IoReg_Write32(M8P_MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);

        mdomain_cap_cap0_buffer_change_line_num_0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_buffer_change_line_num_0_reg);
        mdomain_cap_cap0_buffer_change_line_num_0_reg.cap0_buf_change_line_num_sw = 0;
        IoReg_Write32(M8P_MDOMAIN_CAP_cap0_buffer_change_line_num_0_reg, mdomain_cap_cap0_buffer_change_line_num_0_reg.regValue);

        rtd_pr_scaler_memory_info("[Mdomain][Main] cap0_buf_num = %d\n", mdomain_cap_cap0_buf_control_reg.cap0_buf_num);
    } else
    {
        M8P_mdom_sub_cap_cap1_buf_control_RBUS mdom_sub_cap_cap1_buf_control_reg;
        mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg);
        mdom_sub_cap_cap1_buf_control_reg.cap1_buf_num = (dvrif_memory_get_buffer_number(display) == 0) ? 0 : (dvrif_memory_get_buffer_number(display) - 1);
        mdom_sub_cap_cap1_buf_control_reg.cap1_buf_distance = 0;
        mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_en = 0;
        mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_addr = 0;
        mdom_sub_cap_cap1_buf_control_reg.cap1_buf_freeze_en = 0;
        IoReg_Write32(M8P_MDOM_SUB_CAP_Cap1_buf_control_reg, mdom_sub_cap_cap1_buf_control_reg.regValue);

        rtd_pr_scaler_memory_info("[Mdomain][Sub] cap1_buf_num = %d\n", mdom_sub_cap_cap1_buf_control_reg.cap1_buf_num);
    }
}

void memory_fs_main_decide_buffer_size(void)
{
    unsigned int buffer_size = 0;

    if(dvrif_memory_compression_get_enable(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL))==_ENABLE) {
        //RGB444 mode use 24bits @Crixus 20160902
        //sync k8?
        if (dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY)==COMPRESSION_16_BITS) {
            //buffer_size = MDOMAIN_MAIN_BUFFER_SIZE_24BITS;
            buffer_size = libdma_drv_get_caveout_buffer_size_by_platform(MDOMAIN_16BITS);
        }
        else
        {
            buffer_size = libdma_drv_get_caveout_buffer_size_by_platform(MDOMAIN_12BITS);
        }
    } else {

            buffer_size = libdma_drv_get_caveout_buffer_size_by_platform(MDOMAIN_30BITS);
    }

    if(vdec_data_frc_need_borrow_cma_buffer(SLR_MAIN_DISPLAY)){
        buffer_size = MDOMAIN_MAIN_BUFFER_SIZE_12BITS_4K;
    }

    dvrif_memory_set_buffer_size(SLR_MAIN_DISPLAY, buffer_size);
    rtd_pr_scaler_memory_info("[Mdomain][Fsync][Main] buffer size = %x, %s\n", buffer_size, __FUNCTION__);
}

void memory_fs_sub_decide_buffer_size(void)
{
    unsigned int buffer_size = 0;

    buffer_size = MDOMAIN_SUB_BUFFER_SIZE;
    dvrif_memory_set_buffer_size(SLR_SUB_DISPLAY, buffer_size);
    rtd_pr_scaler_memory_info("[Mdomain] buffer size = %x, %s\n", buffer_size, __FUNCTION__);
}

void memory_fs_main_alloc_buffer(void)
{
#ifdef CONFIG_ARM64
	unsigned long align_addr = 0, query_addr = 0;
#else
	unsigned int align_addr = 0, query_addr = 0;
#endif
#ifndef CONFIG_ALL_SOURCE_DATA_FS
	unsigned int buffer_size_page_align = 0;
#endif
    unsigned long phy_addr_buffer_memory;
    unsigned char borrow_memory = TRUE;//need borrow memory from cma
    unsigned int borrow_size;//borrow size
    void *m_domain_borrow_buffer = NULL;
    SCALER_DISP_CHANNEL display = (SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY);
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY);

    //merlin4 memory do 96 bytes align @Crixus 20180321
    query_addr = get_query_start_address(QUERY_IDX_MDOMAIN_MAIN);
    buffer_size = drvif_memory_get_data_align(buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    align_addr = dvr_memory_alignment((unsigned long)query_addr, buffer_size);
    #ifdef CONFIG_ARM64
    if (align_addr == (unsigned long)NULL)
            BUG();
    #else
    if (align_addr == (unsigned int)NULL)
            BUG();
    #endif

        //Just for reserved memory, update to use query driver @Crixus 20170817
    MemTag[MEMIDX_MAIN_THIRD].StartAddr = align_addr;
    MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    if(vdec_data_frc_need_borrow_cma_buffer(display))
    {
        borrow_size =  buffer_size;
        rtd_pr_scaler_memory_notice("##func:%s main m domain borrow memory from CMA size:0x%x ###\r\n", __FUNCTION__, borrow_size);
        if(get_m_domain_borrow_buffer())
        {//alread has size from cma
            if(borrow_size <= get_m_domain_borrow_size())
            {//no need to borrow again
                borrow_memory = FALSE;
                m_domain_borrow_buffer = get_m_domain_borrow_buffer();//get original addr
                phy_addr_buffer_memory = dvr_to_phys(m_domain_borrow_buffer);
                MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(phy_addr_buffer_memory, DMA_SPEEDUP_ALIGN_VALUE);
            }
            else
                set_m_domain_borrow_buffer(NULL, 0);//free memory before borrow big size
        }
        if(borrow_memory)
        {//need borrrow memory from cam
            m_domain_borrow_buffer = dvr_malloc(borrow_size);
            if(m_domain_borrow_buffer)
            {
                phy_addr_buffer_memory = dvr_to_phys(m_domain_borrow_buffer);
                set_m_domain_borrow_buffer(m_domain_borrow_buffer, borrow_size);//save addr and size
                MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(phy_addr_buffer_memory, DMA_SPEEDUP_ALIGN_VALUE);
            }
            else
            {
                rtd_pr_scaler_memory_err("###[Bug] func:%s no memory from cma ##\r\n",__FUNCTION__);
                return;
            }
        }
    }

    //16 bytes alignment
    MemTag[MEMIDX_MAIN_THIRD].StartAddr = MemTag[MEMIDX_MAIN_THIRD].StartAddr& 0xfffffff0;
    MemTag[MEMIDX_MAIN_SEC].StartAddr = MemTag[MEMIDX_MAIN_SEC].StartAddr & 0xfffffff0;
    MemTag[MEMIDX_MAIN].StartAddr = MemTag[MEMIDX_MAIN].StartAddr & 0xfffffff0;

    //free virtual memory
    //drvif_memory_free_fixed_virtual_memory();
    //set virtual memory size
    //rtd_pr_scaler_memory_emerg("buffer_size=%x\n", buffer_size);
    buffer_size_page_align = PAGE_ALIGN(buffer_size);
    //buffer_size = buffer_size_page_align;
    rtd_pr_scaler_memory_emerg("memory_set_memory_size PAGE_ALIGN(buffer_size)=%x\n", PAGE_ALIGN(buffer_size));
    drvif_memory_set_fixed_virtual_memory_size(buffer_size_page_align);

    MemTag[MEMIDX_MAIN_THIRD].Allocate_StartAddr = MemTag[MEMIDX_MAIN_THIRD].StartAddr;
    MemTag[MEMIDX_MAIN_THIRD].Size = buffer_size;
    MemTag[MEMIDX_MAIN_THIRD].Status = ALLOCATED_FROM_PLI;

    MemTag[MEMIDX_MAIN_SEC].Allocate_StartAddr = MemTag[MEMIDX_MAIN_SEC].StartAddr;
    MemTag[MEMIDX_MAIN_SEC].Size = buffer_size;
    MemTag[MEMIDX_MAIN_SEC].Status = ALLOCATED_FROM_PLI;
    MemTag[MEMIDX_MAIN_SEC].Allocate_VirAddr = 0;//not use

    MemTag[MEMIDX_MAIN].Allocate_StartAddr = MemTag[MEMIDX_MAIN].StartAddr;
    MemTag[MEMIDX_MAIN].Size = buffer_size;
    MemTag[MEMIDX_MAIN].Status = ALLOCATED_FROM_PLI;
    MemTag[MEMIDX_MAIN].Allocate_VirAddr = 0;//not use
}

void memory_fs_sub_alloc_buffer(void)
{
#ifdef CONFIG_DUAL_CHANNEL
#ifdef ENABLE_3_BUFFER_DELAY_MODE
	void *sub_3rd_buffer;
#endif
	void *sub_2nd_buffer;
	void *sub_1st_buffer;
#endif
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY);

#ifdef ENABLE_3_BUFFER_DELAY_MODE
    //I2rnd mode s-cap use 2-buffer
    sub_3rd_buffer = get_sub_m_domain_3rd_buf_address();
    if (sub_3rd_buffer != NULL)
        dvr_free(sub_3rd_buffer);

    sub_3rd_buffer = dvr_malloc(buffer_size);
    set_sub_m_domain_3rd_buf_address(sub_3rd_buffer);
    MemTag[MEMIDX_SUB_THIRD].StartAddr = dvr_to_phys(sub_3rd_buffer);
    rtd_pr_scaler_memory_info("set M-domain sub 3rd buffer 0x%px paddr 0x%08X, size=0x%x\n", sub_3rd_buffer, MemTag[MEMIDX_SUB_THIRD].StartAddr, buffer_size);
#endif

    sub_2nd_buffer = get_sub_m_domain_2nd_buf_address();
    if (sub_2nd_buffer != NULL)
        dvr_free(sub_2nd_buffer);

    sub_2nd_buffer = dvr_malloc(buffer_size);
    set_sub_m_domain_2nd_buf_address(sub_2nd_buffer);
    MemTag[MEMIDX_SUB_SEC].StartAddr = dvr_to_phys(sub_2nd_buffer);
    rtd_pr_scaler_memory_info("set M-domain sub 2nd buffer 0x%px paddr 0x%08X, size=0x%x\n", sub_2nd_buffer, MemTag[MEMIDX_SUB_SEC].StartAddr, buffer_size);


    sub_1st_buffer = get_sub_m_domain_1st_buf_address();
    if (sub_1st_buffer != NULL)
        dvr_free(sub_1st_buffer);

    sub_1st_buffer = dvr_malloc(buffer_size);
    set_sub_m_domain_1st_buf_address(sub_1st_buffer);
    MemTag[MEMIDX_SUB].StartAddr = dvr_to_phys(sub_1st_buffer);
    rtd_pr_scaler_memory_info("set M-domain sub 1st buffer 0x%px paddr 0x%08X, size=0x%x\n", sub_1st_buffer, MemTag[MEMIDX_SUB].StartAddr, buffer_size);
}

void memory_set_display_byte_swap(unsigned char display)
{
    M8P_mdomain_disp_frc_disp0_rd_ctrl_RBUS mdomain_disp_frc_disp0_rd_ctrl_reg;
    M8P_mdom_sub_disp_frc_disp1_rd_ctrl_RBUS mdom_sub_disp_frc_disp1_rd_ctrl_reg;

    mdomain_disp_frc_disp0_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg);
	mdom_sub_disp_frc_disp1_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg);//update disp byte swap reg @Crixus 20170515
	if(display){
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
        if (Get_rotate_function(display)) {
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_1byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_2byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_4byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_8byte_swap = 1;
        } else
#endif
#endif
        if(Scaler_DispGetStatus(display, SLR_DISP_422CAP))
		{
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_1byte_swap = 0;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_2byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_4byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_8byte_swap = 0;
		}
		else
		{
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_1byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_2byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_4byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_8byte_swap = 0;
		}
	}else{
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
        if (Get_rotate_function(display)) {
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_1byte_swap = 1;
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_2byte_swap = 1;
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_4byte_swap = 1;
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_8byte_swap = 1;
        } else
#endif
#endif
        if(Scaler_DispGetStatus(display, SLR_DISP_10BIT))
        {
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_1byte_swap = 0;
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_2byte_swap = 0;
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_4byte_swap = 0;
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_8byte_swap = 0;
        }
        else
        {
            if(Scaler_DispGetStatus(display, SLR_DISP_422CAP))
            {
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_1byte_swap = 0;
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_2byte_swap = 1;
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_4byte_swap = 1;
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_8byte_swap = 0;
            }
            else
            {
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_1byte_swap = 1;
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_2byte_swap = 1;
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_4byte_swap = 1;
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_8byte_swap = 0;
            }
        }
	}

    IoReg_Write32(M8P_MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg, mdomain_disp_frc_disp0_rd_ctrl_reg.regValue);
    IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg, mdom_sub_disp_frc_disp1_rd_ctrl_reg.regValue);
}

void memory_set_display_us_path(unsigned char display)
{
    unsigned int DispWidth = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID);
	unsigned int DispLength = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN);

    if (display == SLR_MAIN_DISPLAY){
        M8P_mdomain_disp_disp0_us_ctrl_RBUS mdomain_disp_disp0_us_ctrl_reg;
        M8P_mdomain_disp_disp0_unpack_ctrl_RBUS mdomain_disp_disp0_unpack_ctrl_reg;
        M8P_mdomain_disp_frc_disp0_size_ctrl0_RBUS mdomain_disp_frc_disp0_size_ctrl0_reg;
            // us ctrl
        mdomain_disp_disp0_us_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_disp0_US_Ctrl_reg);
        mdomain_disp_disp0_us_ctrl_reg.disp0_hor_us_mode = 0;
        mdomain_disp_disp0_us_ctrl_reg.disp0_ver_us_mode = 0;
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdomain_disp_disp0_us_ctrl_reg.disp0_us444_mode = 3;
            mdomain_disp_disp0_us_ctrl_reg.disp0_us444_en = 0;
        } else {
            mdomain_disp_disp0_us_ctrl_reg.disp0_us444_mode = 0;
            if (dvrif_memory_compression_get_enable(display)){
                mdomain_disp_disp0_us_ctrl_reg.disp0_us444_en = 0;
            }
            else{
                mdomain_disp_disp0_us_ctrl_reg.disp0_us444_en = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
            }
        }
        IoReg_Write32(M8P_MDOMAIN_DISP_disp0_US_Ctrl_reg, mdomain_disp_disp0_us_ctrl_reg.regValue);

        rtd_pr_scaler_memory_info("[Mdomain][Main] US0 disp0_ver_en = %d, disp0_hor_en = %d\n", mdomain_disp_disp0_us_ctrl_reg.disp0_us444_mode, mdomain_disp_disp0_us_ctrl_reg.disp0_us444_en);
        
        // unpack_ctrl
        mdomain_disp_disp0_unpack_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_disp0_unpack_ctrl_reg);
        mdomain_disp_disp0_unpack_ctrl_reg.disp0_dummy_format = (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME);
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdomain_disp_disp0_unpack_ctrl_reg.disp0_data_format = MEM_8_BIT;
            mdomain_disp_disp0_unpack_ctrl_reg.disp0_wrap_mode = 10;
        } else {
            mdomain_disp_disp0_unpack_ctrl_reg.disp0_data_format = Scaler_DispGetStatus(display, SLR_DISP_10BIT);
            mdomain_disp_disp0_unpack_ctrl_reg.disp0_wrap_mode = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
        }
        IoReg_Write32(M8P_MDOMAIN_DISP_disp0_unpack_ctrl_reg, mdomain_disp_disp0_unpack_ctrl_reg.regValue);

        rtd_pr_scaler_memory_info("[Mdomain][Main] US0 disp0_wrap_mode = %d, disp0_dummy_format = %d, disp0_data_format = %d\n",
            mdomain_disp_disp0_unpack_ctrl_reg.disp0_wrap_mode, mdomain_disp_disp0_unpack_ctrl_reg.disp0_dummy_format, mdomain_disp_disp0_unpack_ctrl_reg.disp0_data_format);

        // size ctrl
        mdomain_disp_frc_disp0_size_ctrl0_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_disp0_size_ctrl0_reg);
        mdomain_disp_frc_disp0_size_ctrl0_reg.disp0_vact = drvif_memory_get_data_align(DispLength, 2);
        mdomain_disp_frc_disp0_size_ctrl0_reg.disp0_hact = drvif_memory_get_data_align(DispWidth, 2);
        IoReg_Write32(M8P_MDOMAIN_DISP_FRC_disp0_size_ctrl0_reg, mdomain_disp_frc_disp0_size_ctrl0_reg.regValue);
    } else {
        M8P_mdom_sub_disp_disp1_us_ctrl_RBUS mdom_sub_disp_disp1_us_ctrl_reg;
        M8P_mdom_sub_disp_disp1_unpack_ctrl_RBUS mdom_sub_disp_disp1_unpack_ctrl_reg;
        M8P_mdom_sub_disp_frc_disp1_size_ctrl0_RBUS mdom_sub_disp_frc_disp1_size_ctrl0_reg;

        // us ctrl
        mdom_sub_disp_disp1_us_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_disp1_US_Ctrl_reg);
        mdom_sub_disp_disp1_us_ctrl_reg.disp1_hor_us_mode = 0;
        mdom_sub_disp_disp1_us_ctrl_reg.disp1_ver_us_mode = 0;
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdom_sub_disp_disp1_us_ctrl_reg.disp1_us444_mode = 3;
            mdom_sub_disp_disp1_us_ctrl_reg.disp1_us444_en = 0;
        } else {
            mdom_sub_disp_disp1_us_ctrl_reg.disp1_us444_mode = 0;
            if (dvrif_memory_compression_get_enable(display)){
                mdom_sub_disp_disp1_us_ctrl_reg.disp1_us444_en = 0;
            } else {
                mdom_sub_disp_disp1_us_ctrl_reg.disp1_us444_en = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
            }
        }
        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1_US_Ctrl_reg, mdom_sub_disp_disp1_us_ctrl_reg.regValue);

        rtd_pr_scaler_memory_info("[Mdomain][Sub] US1 disp1_ver_en = %d, disp1_hor_en = %d\n", mdom_sub_disp_disp1_us_ctrl_reg.disp1_us444_mode, mdom_sub_disp_disp1_us_ctrl_reg.disp1_us444_en);

        // unpack_ctrl
        mdom_sub_disp_disp1_unpack_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_disp1_unpack_ctrl_reg);
        mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_dummy_format = (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME);
        if (dvrif_memory_get_block_mode_enable(display) == TRUE) {
            mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_data_format = MEM_8_BIT;
            mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_wrap_mode = 10;
        } else {
            mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_data_format = Scaler_DispGetStatus(display, SLR_DISP_10BIT);
            mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_wrap_mode = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
        }
        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1_unpack_ctrl_reg, mdom_sub_disp_disp1_unpack_ctrl_reg.regValue);
        
        rtd_pr_scaler_memory_info("[Mdomain][Sub] US1 disp1_wrap_mode = %d, disp1_dummy_format = %d, disp1_data_format = %d\n",
            mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_wrap_mode, mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_dummy_format, mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_data_format);

        // size ctrl
        mdom_sub_disp_frc_disp1_size_ctrl0_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_disp1_size_ctrl0_reg);
        mdom_sub_disp_frc_disp1_size_ctrl0_reg.disp1_vact = drvif_memory_get_data_align(DispLength, 2);
        mdom_sub_disp_frc_disp1_size_ctrl0_reg.disp1_hact = drvif_memory_get_data_align(DispWidth, 2);
        IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_disp1_size_ctrl0_reg, mdom_sub_disp_frc_disp1_size_ctrl0_reg.regValue);
    }
}

void memory_main_set_rdma(void)
{
    M8P_mdomain_disp_frc_disp0_num_bl_wrap_line_step_RBUS mdomain_disp_frc_disp0_num_bl_wrap_line_step_reg;
    M8P_mdomain_disp_frc_disp0_num_bl_wrap_ctl_RBUS mdomain_disp_frc_disp0_num_bl_wrap_ctl_reg;
    M8P_mdomain_disp_frc_disp0_num_bl_wrap_word_RBUS mdomain_disp_frc_disp0_num_bl_wrap_word_reg;
    M8P_mdomain_disp_disp0_dma_crop_RBUS mdomain_disp_disp0_dma_crop_reg;
    M8P_mdomain_disp_disp0_out_crop_RBUS mdomain_disp_disp0_out_crop_reg;
    M8P_mdomain_disp_disp0_dma_rmask_RBUS mdomain_disp_disp0_dma_rmask_reg;
    
    unsigned int TotalSize;
	unsigned int LineStep;
    unsigned int PixelSize;
	unsigned int DispWidth = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_WID);
	unsigned int DispLength = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_LEN);
    unsigned int x_start =  Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_HSTA);

    if(dvrif_memory_compression_get_enable(SLR_MAIN_DISPLAY) == TRUE)
    {
        PixelSize = dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY);
        if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE)
        {
            TotalSize = libdma_memory_get_capture_line_size_compression(SLR_MAIN_DISPLAY, PixelSize);//compression test
        }
        else{
            TotalSize = libdma_memory_get_capture_frame_size_compression(SLR_MAIN_DISPLAY, PixelSize);
        }
    } else{
        PixelSizeInfo pixelSizeInfo;

        pixelSizeInfo.YCMode = dvrif_memory_get_block_mode_enable(SLR_MAIN_DISPLAY);
        pixelSizeInfo.BitNum = Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_10BIT);
        pixelSizeInfo.Format = Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP);

        PixelSize = memory_get_pixel_size(&pixelSizeInfo);
        TotalSize = libdma_memory_get_display_size(SLR_MAIN_DISPLAY);
    }

    // line_step
	if (dvrif_memory_get_cap_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE)
	{

        LineStep = TotalSize;
#ifndef BUILD_QUICK_SHOW
        if (get_mdomain_vflip_flag(SLR_MAIN_DISPLAY) == TRUE){
            LineStep = ~LineStep + 1;
        }
#endif
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_info("[Mdomain][Main] RDMA0 TotalSize = %d, LineStep = %d\n", TotalSize, LineStep);

    mdomain_disp_frc_disp0_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0_num_bl_wrap_line_step_reg);
    mdomain_disp_frc_disp0_num_bl_wrap_line_step_reg.disp0_line_step = LineStep;
    IoReg_Write32(M8P_MDOMAIN_DISP_FRC_DISP0_num_bl_wrap_line_step_reg, mdomain_disp_frc_disp0_num_bl_wrap_line_step_reg.regValue);

    // line_num & burst_len
    mdomain_disp_frc_disp0_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0_num_bl_wrap_ctl_reg);
    mdomain_disp_frc_disp0_num_bl_wrap_ctl_reg.disp0_line_num = DispLength;
    mdomain_disp_frc_disp0_num_bl_wrap_ctl_reg.disp0_burst_len = _RDMA_BURSTLENGTH;
    IoReg_Write32(M8P_MDOMAIN_DISP_FRC_DISP0_num_bl_wrap_ctl_reg, mdomain_disp_frc_disp0_num_bl_wrap_ctl_reg.regValue);

    // line_burst_num
    mdomain_disp_frc_disp0_num_bl_wrap_word_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0_num_bl_wrap_word_reg);
    mdomain_disp_frc_disp0_num_bl_wrap_word_reg.disp0_addr_toggle_mode = 0;
    mdomain_disp_frc_disp0_num_bl_wrap_word_reg.disp0_line_burst_num = TotalSize;
    IoReg_Write32(M8P_MDOMAIN_DISP_FRC_DISP0_num_bl_wrap_word_reg, mdomain_disp_frc_disp0_num_bl_wrap_word_reg.regValue);

    
    mdomain_disp_disp0_dma_crop_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DISP0_DMA_CROP_reg);
    if (x_start > 0 && ((DispWidth * PixelSize) % 128 == 0)){
        mdomain_disp_disp0_dma_crop_reg.disp0_crop_end_align_128 = 1;
    } else {
        mdomain_disp_disp0_dma_crop_reg.disp0_crop_end_align_128 = 0;
    }
    IoReg_Write32(M8P_MDOMAIN_DISP_DISP0_DMA_CROP_reg, mdomain_disp_disp0_dma_crop_reg.regValue);

    mdomain_disp_disp0_out_crop_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_DISP0_OUT_CROP_reg);
    mdomain_disp_disp0_out_crop_reg.disp0_crop_sta_pxl = (x_start * PixelSize) % 128;
    IoReg_Write32(M8P_MDOMAIN_DISP_DISP0_OUT_CROP_reg, mdomain_disp_disp0_out_crop_reg.regValue);
  
    rtd_pr_scaler_memory_info("[Mdomain][Main] disp0_crop_sta_pxl = %d, disp0_crop_end_align_128 = %d\n", mdomain_disp_disp0_out_crop_reg.disp0_crop_sta_pxl, mdomain_disp_disp0_dma_crop_reg.disp0_crop_end_align_128);
    
    if((DispWidth % 32) != 0){
		DispWidth = DispWidth + (32 - (DispWidth % 32));
	}

    //single buffer aovid tear
    mdomain_disp_disp0_dma_rmask_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_disp0_dma_rmask_reg);
    mdomain_disp_disp0_dma_rmask_reg.disp0_read_first_disable = 1;
    mdomain_disp_disp0_dma_rmask_reg.disp0_read_mask_distance = (DispWidth*PixelSize/128)*2/_RDMA_BURSTLENGTH;
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
    if (Get_rotate_function(SLR_MAIN_DISPLAY)) {
        mdomain_disp_disp0_dma_rmask_reg.disp0_read_mask_force_en = 1;
        mdomain_disp_disp0_dma_rmask_reg.disp0_read_mask_force_value = 0;
    } else
#endif
#endif
    {
        mdomain_disp_disp0_dma_rmask_reg.disp0_read_mask_force_en = 1;
    }
    IoReg_Write32(M8P_MDOMAIN_DISP_disp0_dma_rmask_reg, mdomain_disp_disp0_dma_rmask_reg.regValue);

    // byte_swap
    memory_set_display_byte_swap(SLR_MAIN_DISPLAY);

    // outstanding
    libdma_mdisp_outstanding_ctrl(SLR_MAIN_DISPLAY, _ENABLE);
}

void memory_sub_set_rdma(void)
{
    M8P_mdom_sub_disp_frc_disp1_num_bl_wrap_line_step_RBUS mdom_sub_disp_frc_disp1_num_bl_wrap_line_step_reg;
    M8P_mdom_sub_disp_frc_disp1_num_bl_wrap_ctl_RBUS mdom_sub_disp_frc_disp1_num_bl_wrap_ctl_reg;
    M8P_mdom_sub_disp_frc_disp1_num_bl_wrap_word_RBUS mdom_sub_disp_frc_disp1_num_bl_wrap_word_reg;
    M8P_mdom_sub_disp_disp1_crop_RBUS mdom_sub_disp_disp1_crop_reg;
    M8P_mdom_sub_disp_disp1_out_crop_RBUS mdom_sub_disp_disp1_out_crop_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
    unsigned int PixelSize;
	unsigned int DispWidth = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_WID);
	unsigned int DispLength = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_LEN);
    unsigned int x_start =  Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_HSTA);

    if(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        PixelSize = dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY);
        if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
        {
            TotalSize = libdma_memory_get_capture_line_size_compression(SLR_SUB_DISPLAY, PixelSize);//compression test
        }
        else{
            TotalSize = libdma_memory_get_capture_frame_size_compression(SLR_SUB_DISPLAY, PixelSize);
        }
    } else{
        PixelSizeInfo pixelSizeInfo;

        pixelSizeInfo.YCMode = dvrif_memory_get_block_mode_enable(SLR_SUB_DISPLAY);
        pixelSizeInfo.BitNum = Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_10BIT);
        pixelSizeInfo.Format = Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP);

        PixelSize = memory_get_pixel_size(&pixelSizeInfo);
        TotalSize = libdma_memory_get_display_size(SLR_SUB_DISPLAY);
    }
    // line_step
	if (dvrif_memory_get_cap_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
	{
        LineStep = TotalSize;
#ifndef BUILD_QUICK_SHOW
		if (get_mdomain_vflip_flag(SLR_SUB_DISPLAY) == TRUE){
            LineStep = ~LineStep + 1;
        }
#endif
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_info("[Mdomain][Sub] RDMA1 TotalSize = %d, LineStep = %d\n", TotalSize, LineStep);

    mdom_sub_disp_frc_disp1_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_DISP1_num_bl_wrap_line_step_reg);
    mdom_sub_disp_frc_disp1_num_bl_wrap_line_step_reg.disp1_line_step = LineStep;
    IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_DISP1_num_bl_wrap_line_step_reg, mdom_sub_disp_frc_disp1_num_bl_wrap_line_step_reg.regValue);

    // line_num & burst_len
    mdom_sub_disp_frc_disp1_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_DISP1_num_bl_wrap_ctl_reg);
    mdom_sub_disp_frc_disp1_num_bl_wrap_ctl_reg.disp1_line_num = DispLength;
    mdom_sub_disp_frc_disp1_num_bl_wrap_ctl_reg.disp1_burst_len = _RDMA_BURSTLENGTH;
    IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_DISP1_num_bl_wrap_ctl_reg, mdom_sub_disp_frc_disp1_num_bl_wrap_ctl_reg.regValue);

    // line_burst_num
    mdom_sub_disp_frc_disp1_num_bl_wrap_word_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_DISP1_num_bl_wrap_word_reg);
    mdom_sub_disp_frc_disp1_num_bl_wrap_word_reg.disp1_addr_toggle_mode = 0;
    mdom_sub_disp_frc_disp1_num_bl_wrap_word_reg.disp1_line_burst_num = TotalSize;
    IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_DISP1_num_bl_wrap_word_reg, mdom_sub_disp_frc_disp1_num_bl_wrap_word_reg.regValue);

    // crop
    mdom_sub_disp_disp1_crop_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_DISP1_CROP_reg);
    if (x_start > 0 && ((DispWidth * PixelSize) % 128 == 0)){
        mdom_sub_disp_disp1_crop_reg.disp1_crop_end_align_128 = 1;
    } else {
        mdom_sub_disp_disp1_crop_reg.disp1_crop_end_align_128 = 0;
    }
    IoReg_Write32(M8P_MDOM_SUB_DISP_DISP1_CROP_reg, mdom_sub_disp_disp1_crop_reg.regValue);

    mdom_sub_disp_disp1_out_crop_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_DISP1_OUT_CROP_reg);
    mdom_sub_disp_disp1_out_crop_reg.disp1_crop_sta_pxl = (x_start * PixelSize) % 128;
    IoReg_Write32(M8P_MDOM_SUB_DISP_DISP1_OUT_CROP_reg, mdom_sub_disp_disp1_out_crop_reg.regValue);

    rtd_pr_scaler_memory_info("[Mdomain][Sub] disp1_crop_sta_pxl = %d, disp1_crop_end_align_128 = %d\n", mdom_sub_disp_disp1_out_crop_reg.disp1_crop_sta_pxl, mdom_sub_disp_disp1_out_crop_reg.disp1_crop_sta_pxl);

    // byte_swap
    memory_set_display_byte_swap(SLR_SUB_DISPLAY);

    // outstanding
    libdma_mdisp_outstanding_ctrl(SLR_SUB_DISPLAY, _ENABLE);
}

void memory_main_set_rdma_c(void)
{
    M8P_mdomain_disp_frc_disp0c_num_bl_wrap_line_step_RBUS mdomain_disp_frc_disp0c_num_bl_wrap_line_step_reg;
    M8P_mdomain_disp_frc_disp0c_num_bl_wrap_ctl_RBUS mdomain_disp_frc_disp0c_num_bl_wrap_ctl_reg;
    M8P_mdomain_disp_frc_disp0c_num_bl_wrap_word_RBUS mdomain_disp_frc_disp0c_num_bl_wrap_word_reg;
    M8P_mdomain_disp_frc_disp0c_rd_ctrl_RBUS mdomain_disp_frc_disp0c_rd_ctrl_reg;
	
    unsigned int TotalSize;
	unsigned int LineStep;
    unsigned int PixelSize;
	unsigned int DispLength = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_LEN) / 2;

    PixelSizeInfo pixelSizeInfo;
    pixelSizeInfo.YCMode = MEM_YC_SEPARATE;
    pixelSizeInfo.BitNum = MEM_8_BIT;
    pixelSizeInfo.Format = MEM_420;

    PixelSize = memory_get_pixel_size(&pixelSizeInfo);
    TotalSize = libdma_memory_get_display_size(SLR_MAIN_DISPLAY);

    // line_step

    LineStep = TotalSize;
#ifndef BUILD_QUICK_SHOW
    if (get_mdomain_vflip_flag(SLR_MAIN_DISPLAY) == TRUE){
        LineStep = ~LineStep + 1;
    }
#endif
    rtd_pr_scaler_memory_info("[Mdomain][Main] RDMA0C TotalSize = %d, LineStep = %d\n", TotalSize, LineStep);

    mdomain_disp_frc_disp0c_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0C_num_bl_wrap_line_step_reg);
    mdomain_disp_frc_disp0c_num_bl_wrap_line_step_reg.disp0c_line_step = LineStep;
    IoReg_Write32(M8P_MDOMAIN_DISP_FRC_DISP0C_num_bl_wrap_line_step_reg, mdomain_disp_frc_disp0c_num_bl_wrap_line_step_reg.regValue);

    // line_num & burst_len
    mdomain_disp_frc_disp0c_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0C_num_bl_wrap_ctl_reg);
    mdomain_disp_frc_disp0c_num_bl_wrap_ctl_reg.disp0c_line_num = DispLength;
    mdomain_disp_frc_disp0c_num_bl_wrap_ctl_reg.disp0c_burst_len = _RDMA_BURSTLENGTH;
    IoReg_Write32(M8P_MDOMAIN_DISP_FRC_DISP0C_num_bl_wrap_ctl_reg, mdomain_disp_frc_disp0c_num_bl_wrap_ctl_reg.regValue);

    // line_burst_num
    mdomain_disp_frc_disp0c_num_bl_wrap_word_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0C_num_bl_wrap_word_reg);
    mdomain_disp_frc_disp0c_num_bl_wrap_word_reg.disp0c_addr_toggle_mode = 1;
    mdomain_disp_frc_disp0c_num_bl_wrap_word_reg.disp0c_line_burst_num = TotalSize;
    IoReg_Write32(M8P_MDOMAIN_DISP_FRC_DISP0C_num_bl_wrap_word_reg, mdomain_disp_frc_disp0c_num_bl_wrap_word_reg.regValue);

    // byte_swap
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
    if (Get_rotate_function(SLR_MAIN_DISPLAY)) {
        mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg);
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_8byte_swap = 1;
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_4byte_swap = 1;
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_2byte_swap = 1;
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_1byte_swap = 1;
        IoReg_Write32(M8P_MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg, mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue);
    } else 
#endif
#endif
    {
        mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg);
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_8byte_swap = 0;
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_4byte_swap = 0;
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_2byte_swap = 0;
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_1byte_swap = 0;
        IoReg_Write32(M8P_MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg, mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue);
    }

    // outstanding
    mdisp_dmac_outstanding_ctrl(SLR_MAIN_DISPLAY, _ENABLE);
}

void memory_sub_set_rdma_c(void)
{
    M8P_mdom_sub_disp_frc_disp1c_num_bl_wrap_line_step_RBUS mdom_sub_disp_frc_disp1c_num_bl_wrap_line_step_reg;
    M8P_mdom_sub_disp_frc_disp1c_num_bl_wrap_ctl_RBUS mdom_sub_disp_frc_disp1c_num_bl_wrap_ctl_reg;
    M8P_mdom_sub_disp_frc_disp1c_num_bl_wrap_word_RBUS mdom_sub_disp_frc_disp1c_num_bl_wrap_word_reg;
    M8P_mdom_sub_disp_frc_disp1c_rd_ctrl_RBUS mdom_sub_disp_frc_disp1c_rd_ctrl_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
	unsigned int DispLength = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_LEN);

    if(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
        {
            TotalSize = libdma_memory_get_capture_line_size_compression(SLR_SUB_DISPLAY, dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));//compression test
        }
        else{
            TotalSize = libdma_memory_get_capture_frame_size_compression(SLR_SUB_DISPLAY, dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));
        }
    } else{
        TotalSize = libdma_memory_get_display_size(SLR_SUB_DISPLAY);
    }
    // line_step
	if (dvrif_memory_get_cap_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
	{
        LineStep = TotalSize;
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_info("[Mdomain][Sub] RDMA1C TotalSize = %d, LineStep = %d\n", TotalSize, LineStep);

    mdom_sub_disp_frc_disp1c_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_DISP1C_num_bl_wrap_line_step_reg);
    mdom_sub_disp_frc_disp1c_num_bl_wrap_line_step_reg.disp1c_line_step = LineStep;
    IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_DISP1C_num_bl_wrap_line_step_reg, mdom_sub_disp_frc_disp1c_num_bl_wrap_line_step_reg.regValue);

    // line_num & burst_len
    mdom_sub_disp_frc_disp1c_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_DISP1C_num_bl_wrap_ctl_reg);
    mdom_sub_disp_frc_disp1c_num_bl_wrap_ctl_reg.disp1c_line_num = DispLength / 2;
    mdom_sub_disp_frc_disp1c_num_bl_wrap_ctl_reg.disp1c_burst_len = _RDMA_BURSTLENGTH;
    IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_DISP1C_num_bl_wrap_ctl_reg, mdom_sub_disp_frc_disp1c_num_bl_wrap_ctl_reg.regValue);

    // line_burst_num
    mdom_sub_disp_frc_disp1c_num_bl_wrap_word_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_DISP1C_num_bl_wrap_word_reg);
    mdom_sub_disp_frc_disp1c_num_bl_wrap_word_reg.disp1c_addr_toggle_mode = 1;
    mdom_sub_disp_frc_disp1c_num_bl_wrap_word_reg.disp1c_line_burst_num = TotalSize;
    IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_DISP1C_num_bl_wrap_word_reg, mdom_sub_disp_frc_disp1c_num_bl_wrap_word_reg.regValue);

    // byte_swap
    mdom_sub_disp_frc_disp1c_rd_ctrl_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_reg);
    mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_dma_8byte_swap = 0;
    mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_dma_4byte_swap = 0;
    mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_dma_2byte_swap = 0;
    mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_dma_1byte_swap = 0;
    IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_reg, mdom_sub_disp_frc_disp1c_rd_ctrl_reg.regValue);
    
    // outstanding
    mdisp_dmac_outstanding_ctrl(SLR_SUB_DISPLAY, _ENABLE);
}

unsigned int memory_frc_calculate_display_shift_address(unsigned char display)
{
    unsigned int x_start =  Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_HSTA);
    unsigned int y_start =  Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_VSTA);
    unsigned int disp_wid = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID);
    unsigned int cap_len = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN);
    PixelSizeInfo pixelSizeInfo;
    unsigned int PixelSize;
    unsigned int TotalSize;
    unsigned int shift_addr = 0;

    if(dvrif_memory_compression_get_enable(display) == TRUE)
    {
        PixelSize = dvrif_memory_get_compression_bits(display);
        TotalSize = libdma_memory_get_capture_line_size_compression(display, PixelSize);

    } else {
        if (dvrif_memory_get_block_mode_enable(display)){
            pixelSizeInfo.YCMode = MEM_YC_SEPARATE;
            pixelSizeInfo.BitNum = MEM_8_BIT;
            pixelSizeInfo.Format = MEM_420;
        } else {
            pixelSizeInfo.YCMode = MEM_YC_COMBINE;
            pixelSizeInfo.BitNum = Scaler_DispGetStatus(display, SLR_DISP_10BIT);
            pixelSizeInfo.Format = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
        }
        TotalSize = libdma_memory_get_line_size(disp_wid, &pixelSizeInfo);
        PixelSize = memory_get_pixel_size(&pixelSizeInfo);
    }
#ifndef BUILD_QUICK_SHOW
    if (get_mdomain_vflip_flag(display) == FALSE){
        shift_addr = TotalSize * y_start + (PixelSize * x_start) / 128;
    } else 
#endif
    {
        shift_addr = TotalSize * (cap_len - y_start  - 1) + (PixelSize * x_start) / 128;
    }
    return shift_addr << 4;
}

void memory_main_update_display_buffer(void)
{
    unsigned char cap_num = dvrif_memory_get_buffer_number(SLR_MAIN_DISPLAY);
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY);
    unsigned int MemShiftAddr;

    if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE){
        MemShiftAddr = memory_frc_calculate_display_shift_address(SLR_MAIN_DISPLAY);
    } else {
        MemShiftAddr = 0;
    }

    rtd_pr_scaler_memory_emerg("[Mdomain][Main] update mdisp buffer number = %d\n", cap_num);

    if(cap_num > 0){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0_first_buf_addr_reg, (MemTag[MEMIDX_MAIN].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_downlimit_0_reg, (MemTag[MEMIDX_MAIN].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_uplimit_0_reg, (MemTag[MEMIDX_MAIN].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen
    }

    if(cap_num > 1){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0_second_buf_addr_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_downlimit_1_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_uplimit_1_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0_third_buf_addr_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_downlimit_2_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_uplimit_2_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0_forth_buf_addr_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_downlimit_3_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_uplimit_3_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0_fifth_buf_addr_reg, (MemTag[MEMIDX_MAIN_FIFTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_downlimit_4_reg, (MemTag[MEMIDX_MAIN_FIFTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_uplimit_4_reg, (MemTag[MEMIDX_MAIN_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0_sixth_buf_addr_reg, (MemTag[MEMIDX_MAIN_SIXTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_downlimit_5_reg, (MemTag[MEMIDX_MAIN_SIXTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_uplimit_5_reg, (MemTag[MEMIDX_MAIN_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0_seventh_buf_addr_reg, (MemTag[MEMIDX_MAIN_SEVENTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_downlimit_6_reg, (MemTag[MEMIDX_MAIN_SEVENTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_uplimit_6_reg, (MemTag[MEMIDX_MAIN_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(M8P_MDOMAIN_DISP_disp0_eightth_buf_addr_reg, (MemTag[MEMIDX_MAIN_EIGHTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_downlimit_7_reg, (MemTag[MEMIDX_MAIN_EIGHTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOMAIN_DISP_Disp0_uplimit_7_reg, (MemTag[MEMIDX_MAIN_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }
}

void memory_sub_update_display_buffer(void)
{
    unsigned char cap_num = dvrif_memory_get_buffer_number(SLR_SUB_DISPLAY);
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY);
    unsigned int MemShiftAddr;

    if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE){
        MemShiftAddr = memory_frc_calculate_display_shift_address(SLR_SUB_DISPLAY);
    } else {
        MemShiftAddr = 0;
    }
 
    rtd_pr_scaler_memory_emerg("[Mdomain][Sub] update mdisp buffer number = %d\n", cap_num);
    
    if(cap_num > 0){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1_first_buf_addr_reg, (MemTag[MEMIDX_SUB].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_downlimit_0_reg, (MemTag[MEMIDX_SUB].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_uplimit_0_reg, (MemTag[MEMIDX_SUB].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 1){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1_second_buf_addr_reg, (MemTag[MEMIDX_SUB_SEC].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_downlimit_1_reg, (MemTag[MEMIDX_SUB_SEC].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_uplimit_1_reg, (MemTag[MEMIDX_SUB_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1_third_buf_addr_reg, (MemTag[MEMIDX_SUB_THIRD].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_downlimit_2_reg, (MemTag[MEMIDX_SUB_THIRD].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_uplimit_2_reg, (MemTag[MEMIDX_SUB_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1_forth_buf_addr_reg, (MemTag[MEMIDX_SUB_FOURTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_downlimit_3_reg, (MemTag[MEMIDX_SUB_FOURTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_uplimit_3_reg, (MemTag[MEMIDX_SUB_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1_fifth_buf_addr_reg, (MemTag[MEMIDX_SUB_FIFTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_downlimit_4_reg, (MemTag[MEMIDX_SUB_FIFTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_uplimit_4_reg, (MemTag[MEMIDX_SUB_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1_sixth_buf_addr_reg, (MemTag[MEMIDX_SUB_SIXTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_downlimit_5_reg, (MemTag[MEMIDX_SUB_SIXTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_uplimit_5_reg, (MemTag[MEMIDX_SUB_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1_seventh_buf_addr_reg, (MemTag[MEMIDX_SUB_SEVENTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_downlimit_6_reg, (MemTag[MEMIDX_SUB_SEVENTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_uplimit_6_reg, (MemTag[MEMIDX_SUB_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(M8P_MDOM_SUB_DISP_disp1_eightth_buf_addr_reg, (MemTag[MEMIDX_SUB_EIGHTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_downlimit_7_reg, (MemTag[MEMIDX_SUB_EIGHTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(M8P_MDOM_SUB_DISP_Disp1_uplimit_7_reg, (MemTag[MEMIDX_SUB_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }
}

void memory_frc_set_display_block_control(unsigned char display)
{
    if (display == SLR_MAIN_DISPLAY)
    {
        M8P_mdomain_disp_frc_disp0_buffer_select_control_RBUS mdomain_disp_frc_disp0_buffer_select_control_reg;
        mdomain_disp_frc_disp0_buffer_select_control_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg);
        mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_num = (dvrif_memory_get_buffer_number(display) == 0) ? 0 : (dvrif_memory_get_buffer_number(display) - 1);
        mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_distance = 0;
        mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_en = 0;
        mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_addr = 0;
        IoReg_Write32(M8P_MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg, mdomain_disp_frc_disp0_buffer_select_control_reg.regValue);

    } else
    {
        M8P_mdom_sub_disp_frc_disp1_buffer_select_control_RBUS mdom_sub_disp_frc_disp1_buffer_select_control_reg;
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue = IoReg_Read32(M8P_MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg);
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_num = (dvrif_memory_get_buffer_number(display) == 0) ? 0 : (dvrif_memory_get_buffer_number(display) - 1);
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_distance = 0;
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_force_en = 0;
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_force_addr = 0;
        IoReg_Write32(M8P_MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg, mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue);
    }
}

void dvrif_memory_set_gatting_settings(unsigned char display, unsigned int gating_mask_line){
    if (display == SLR_MAIN_DISPLAY)
    {
        M8P_mdomain_cap_frc_cap0_timing_ctrl_RBUS mdomain_cap_frc_cap0_timing_ctrl_reg;
        M8P_mdomain_disp_disp0_fsbuf_ctrl_1_RBUS mdomain_disp_disp0_fsbuf_ctrl_1_reg;

        mdomain_cap_frc_cap0_timing_ctrl_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_frc_cap0_timing_ctrl_reg);
        mdomain_cap_frc_cap0_timing_ctrl_reg.cap0_htotal = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_H_LEN);
        IoReg_Write32(M8P_MDOMAIN_CAP_frc_cap0_timing_ctrl_reg, mdomain_cap_frc_cap0_timing_ctrl_reg.regValue);

        mdomain_disp_disp0_fsbuf_ctrl_1_reg.regValue = IoReg_Read32(M8P_MDOMAIN_DISP_disp0_FSBUF_CTRL_1_reg);
        mdomain_disp_disp0_fsbuf_ctrl_1_reg.disp0_fs_gating_mask_line = gating_mask_line;
        IoReg_Write32(M8P_MDOMAIN_DISP_disp0_FSBUF_CTRL_1_reg, mdomain_disp_disp0_fsbuf_ctrl_1_reg.regValue);

        rtd_pr_scaler_memory_emerg("cap0_htotal = %d, fs_gating_mask_line = %d\n", Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_H_LEN), gating_mask_line);
    }
}

void libdma_memory_set_main_displaywindow_change(void)
{
    memory_frc_set_pre_read_v_start(SLR_MAIN_DISPLAY, libdma_drv_Calculate_m_pre_read_value());

    memory_set_display_us_path(SLR_MAIN_DISPLAY);
    memory_main_set_rdma();
    memory_main_update_display_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_MAIN_DISPLAY) == TRUE)
    {
        memory_main_set_rdma_c();
        memory_main_update_display_C_buffer(3, dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY));
    }
}

void libdma_memory_set_sub_displaywindow_change(unsigned char changetomain)
{
    memory_frc_set_pre_read_v_start(SLR_SUB_DISPLAY, libdma_drv_Calculate_m_pre_read_value());

    memory_set_display_us_path(SLR_SUB_DISPLAY);
    memory_sub_set_rdma();
    memory_sub_update_display_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        memory_sub_set_rdma_c();
        memory_sub_update_display_C_buffer(3, dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY));
    }
}

void memory_frc_main_set_capture(void)
{
    M8P_mdomain_cap_cap0_reg_doublbuffer_RBUS mdomain_cap_cap0_reg_doublbuffer_reg;

	// disable cap db
	if(scaler_vsc_get_vr360_block_mdomain_doublebuffer() == FALSE){
		mdomain_cap_cap0_reg_doublbuffer_reg.regValue = IoReg_Read32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = 0;
		IoReg_Write32(M8P_MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);
	}

    memory_frc_main_decide_buffer_size();
    memory_frc_main_alloc_buffer();

    memory_set_capture_ds_path(SLR_MAIN_DISPLAY);
	memory_main_set_wdma();
    memory_main_update_capture_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_MAIN_DISPLAY) == TRUE)
    {
        memory_main_set_wdma_c();
        memory_main_update_capture_C_buffer(dvrif_memory_get_buffer_number(SLR_MAIN_DISPLAY),  dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY));
    }
    memory_frc_set_capture_block_control(SLR_MAIN_DISPLAY);
}

void memory_frc_sub_set_capture(void)
{
    memory_frc_sub_decide_buffer_size();
    // set capture memory for sub display
    memory_frc_sub_alloc_buffer();

    memory_set_capture_ds_path(SLR_SUB_DISPLAY);
    memory_sub_set_wdma();
    memory_sub_update_capture_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        memory_sub_set_wdma_c();
        memory_sub_update_capture_C_buffer(3,  dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY));
    }
    memory_frc_set_capture_block_control(SLR_SUB_DISPLAY);
}

void memory_frc_main_set_display(void)
{
    memory_frc_set_pre_read_v_start(SLR_MAIN_DISPLAY, libdma_drv_Calculate_m_pre_read_value());
    libdma_drv_memory_set_vo_gating_threshold();

    memory_set_display_us_path(SLR_MAIN_DISPLAY);
    memory_main_set_rdma();
    memory_main_update_display_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_MAIN_DISPLAY) == TRUE)
    {
        memory_main_set_rdma_c();
        memory_main_update_display_C_buffer(dvrif_memory_get_buffer_number(SLR_MAIN_DISPLAY), dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY));
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
        if (Get_rotate_function(SLR_MAIN_DISPLAY)) {
            rotate_update_display_buffer_limit();
        }
#endif
#endif
    }
    memory_frc_set_display_block_control(SLR_MAIN_DISPLAY);	
}

void memory_frc_sub_set_display(void)
{
    memory_frc_set_pre_read_v_start(SLR_SUB_DISPLAY, libdma_drv_Calculate_m_pre_read_value());
    libdma_drv_memory_set_vo_gating_threshold();

    memory_set_display_us_path(SLR_SUB_DISPLAY);
    memory_sub_set_rdma();
    memory_sub_update_display_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        memory_sub_set_rdma_c();
        memory_sub_update_display_C_buffer(dvrif_memory_get_buffer_number(SLR_SUB_DISPLAY), dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY));
    }
    memory_frc_set_display_block_control(SLR_SUB_DISPLAY);
}

void libdma_dvrif_memory_frc_handler(unsigned char display)
{
#ifndef UT_flag
	extern unsigned char compare_rerify_vsc_source_num_result(unsigned char display);
#endif // #ifndef UT_flag
	/* should not disable ddomain interrupt DTG_ie.mv_den_sta_event_ie*/
	//if(display == SLR_MAIN_DISPLAY)
	//	drvif_scaler_ddomain_switch_irq(_DISABLE);	// avoid to DSR to switch buffer when double bit not set

	set_m_domain_setting_err_status(display, MDOMAIN_SETTING_NO_ERR); // reset M domain setting error status
	dvrif_memory_set_gatting_settings(display, _M_DATA_FRC_GATTING_MASK_LINE);

#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
#ifndef BUILD_QUICK_SHOW
    dvrif_memory_set_block_mode_enable(display, Get_rotate_function(display));
#endif
#else
    dvrif_memory_set_block_mode_enable(display, FALSE);
#endif

	if (display == SLR_MAIN_DISPLAY) {	// Main
		down(get_main_mdomain_ctrl_semaphore());
#ifndef BUILD_QUICK_SHOW
#ifndef UT_flag
		if(!compare_rerify_vsc_source_num_result(SLR_MAIN_DISPLAY))
		{
			up(get_main_mdomain_ctrl_semaphore());
			rtd_pr_scaler_memory_info("### dvrif_memory_handler vsc source chnage return ###\r\n");
			return;
		}
#endif // #ifndef UT_flag
#endif // #ifndef BUILD_QUICK_SHOW
		//Disable M-disp when reset M-domain=>To Fixed DTV change issue @ Crixus 20141126
		libdma_drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_MAIN_DISPLAY, _DISABLE);
		libdma_drvif_memory_DisableMCap(SLR_MAIN_DISPLAY); // Disable M-cap before do M-cap related setting

		memory_frc_decide_capture_mode(display);

		memory_frc_decide_buffer_number(display);

        memory_frc_main_set_capture();
		memory_frc_main_set_display();

        if(check_sub_is_idle_for_srnn(display)) {
            set_sub_for_srnn(SUB_MDOMAIN_SRNN, 1);
        }
    }
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else {	//Sub enable
		if (Scaler_PipGetInfo(SLR_PIP_TYPE) <= SUBTYPE_PIPMLSR_4x3)
		{// Sub PIP

            memory_frc_decide_capture_mode(display);
            memory_frc_decide_buffer_number(display);

			memory_frc_sub_set_capture();
            memory_frc_sub_set_display();
		}
		else if (Scaler_PipGetInfo(SLR_PIP_TYPE) <= SUBTYPE_MP1L7B)
		{// Sub MP
			memory_frc_sub_set_capture();
            memory_frc_sub_set_display();
			Scaler_PipSetSubInfo(SLR_PIP_SUB_MPENABLE,1);
		}
		else
		{

            memory_frc_decide_capture_mode(display);
            memory_frc_decide_buffer_number(display);

			memory_frc_sub_set_capture();
            memory_frc_sub_set_display();
		}
	}
#endif //CONFIG_DUAL_CHANNEL  // There are two channels

	//compression & de-compression setting
	if(display== SLR_MAIN_DISPLAY)
	{
		if((Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) == _CHANNEL1) && (dvrif_memory_compression_get_enable(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)) == TRUE)){
			libdma_dvrif_memory_comp_setting(TRUE, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_WID), Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN), dvrif_memory_get_compression_bits(display));
			if(scaler_vsc_get_vr360_block_mdomain_doublebuffer()==FALSE)
				IoReg_SetBits(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg, M8P_MDOMAIN_DISP_Disp0_db_ctrl_disp0_db_apply_mask);
		}
		else{
			libdma_dvrif_memory_comp_setting(FALSE, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_WID), Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN), dvrif_memory_get_compression_bits(display));
			if(scaler_vsc_get_vr360_block_mdomain_doublebuffer()==FALSE)
				IoReg_SetBits(M8P_MDOMAIN_DISP_Disp0_db_ctrl_reg, M8P_MDOMAIN_DISP_Disp0_db_ctrl_disp0_db_apply_mask);
		}
	}
#ifdef CONFIG_DUAL_CHANNEL
	else
	{
		//enable or disable compress mode for sub path.
		libdma_dvrif_memory_comp_setting_sub(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) ? TRUE : FALSE, Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_WID), Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_LEN), dvrif_memory_get_compression_bits(display));
	}
#endif

#ifndef BUILD_QUICK_SHOW
    if(TRUE==Get_vscFilmMode())
	{
		//because film mode need 4 buffers
		libdma_FilmModeCachaddr = (unsigned long)dvr_malloc(drvif_memory_get_block_size(MEMIDX_MAIN));
		//libdma_FilmModePhyadddr = (unsigned int)virt_to_phys((void*)libdma_FilmModeCachaddr);
		#ifdef CONFIG_ARM64
		libdma_FilmModePhyadddr = (unsigned long)dvr_to_phys((void*)libdma_FilmModeCachaddr);
		#else
		libdma_FilmModePhyadddr = (unsigned int)dvr_to_phys((void*)libdma_FilmModeCachaddr);
		#endif
		filmmode_videofw_config(TRUE,libdma_FilmModePhyadddr);
	}
#endif

	//Game mode setting
	//Update the Game mode condition RGB444 @Crixus 20160222
	if (display == SLR_MAIN_DISPLAY) {
		//Eric@20181016 Vdec Direct Low Latency mode flow
		if(drv_memory_get_vdec_direct_low_latency_mode() && !((Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI)))
			libdma_drv_memory_GameMode_Switch_SingleBuffer();

#ifndef BUILD_QUICK_SHOW
		/*qiangzhou:all i3ddma source go vodma timing sync i3ddma,all i3ddma source should setting iv2pv delay*/
		if(((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_ADC)
			||((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_AVD)  && (get_AVD_Input_Source() != _SRC_TV))
			||((false == is_hdmi_dolby_vision_sink_led()) && (get_vsc_src_is_hdmi_or_dp())))//dolby case set false
			&&(!drvif_i3ddma_triplebuf_flag())&&(!drvif_i3ddma_triplebuf_by_timing_protect_panel()) && (display == SLR_MAIN_DISPLAY)
			&& !(Scaler_Get_FreeRun_To_FrameSync_By_Reset_VODMA() && vbe_get_HDMI_run_timing_framesync_condition())
			&& (get_force_i3ddma_enable(display))){
			drv_I3ddmaVodma_GameMode_iv2pv_delay(TRUE);
		}else if (display == SLR_MAIN_DISPLAY){
			drv_I3ddmaVodma_GameMode_iv2pv_delay(FALSE);
		}
#endif
	}

	//limit boundary enable @Crixus 20180320
	libdma_check_and_set_mdomain_input_fast(display);

	if(get_m_domain_setting_err_status(display) == MDOMAIN_SETTING_NO_ERR)
		libdma_drvif_memory_EnableMCap(display);
	else
		rtd_pr_scaler_memory_emerg("[%s] %d ERROR! Don't enable Mcap_en. (display=%d, err_status=%d)\n", __FUNCTION__, __LINE__, display, get_m_domain_setting_err_status(display));
	libdma_drvif_memory_EnableMDisp_Or_DisableMDisp(display, _ENABLE);
	
    if (display == SLR_MAIN_DISPLAY)
		up(get_main_mdomain_ctrl_semaphore());
}

void memory_fs_main_set_capture(void)
{
    memory_fs_main_decide_buffer_size();
    memory_fs_main_alloc_buffer();

    memory_set_capture_ds_path(SLR_MAIN_DISPLAY);
    memory_main_update_capture_buffer();
}

void memory_fs_sub_set_capture(void)
{
    memory_fs_sub_decide_buffer_size();
    memory_fs_sub_alloc_buffer();
    memory_sub_update_capture_buffer();
}

void memory_fs_main_set_display(void)
{
    memory_set_display_us_path(SLR_MAIN_DISPLAY);
    memory_main_update_display_buffer();
}

void memory_fs_sub_set_display(void)
{
    memory_set_display_us_path(SLR_SUB_DISPLAY);
    memory_sub_update_display_buffer();
}

void libdma_dvrif_memory_fs_handler(unsigned char display)
{
    unsigned char buf_num = 0;

    libdma_drv_memory_set_vo_gating_threshold();
    memory_frc_set_pre_read_v_start(SLR_MAIN_DISPLAY, libdma_drv_Calculate_m_pre_read_value());
	dvrif_memory_set_gatting_settings(display, _M_DATA_FS_GATTING_MASK_LINE);

    if (display == SLR_MAIN_DISPLAY)
    {
        if((Scaler_InputSrcGetMainChType() == _SRC_VO) && (Scaler_VOFrom(Scaler_Get_CurVoInfo_plane()) == 1))	// yychao+ : use single buffer for JPEG source
        {
            buf_num = 1;
            dvrif_memory_set_cap_mode(SLR_MAIN_DISPLAY, MEMCAPTYPE_FRAME);	//  CSW 0961105 change single buffer to double to slove 1080i moving picture not smooth
        } else
        {
            buf_num = 3;
	#if 0
            if((Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) == _CHANNEL1) && (dvrif_memory_compression_get_enable(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)) == TRUE)){
                //update M-domain PQC frame mode @Crixus 20170626
                if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == COMPRESSION_LINE_MODE){
                    dvrif_memory_set_cap_mode(SLR_MAIN_DISPLAY, MEMCAPTYPE_LINE);
                } else {
                    dvrif_memory_set_cap_mode(SLR_MAIN_DISPLAY, MEMCAPTYPE_FRAME);
                }
            } else {
                dvrif_memory_set_cap_mode(SLR_MAIN_DISPLAY, MEMCAPTYPE_FRAME);	//  CSW 0961105 change single buffer to double to slove 1080i moving picture not smooth
            }
	#endif
	    dvrif_memory_set_cap_mode(SLR_MAIN_DISPLAY, MEMCAPTYPE_LINE);
        }

#ifdef ENABLE_3_BUFFER_DELAY_MODE
        buf_num = 3;
#endif
        dvrif_memory_set_buffer_number(SLR_MAIN_DISPLAY, buf_num);

        memory_fs_main_set_capture();
        memory_fs_main_set_display();
    }
 #ifdef CONFIG_DUAL_CHANNEL
    else {
        if (Scaler_PipGetInfo(SLR_PIP_TYPE) <= SUBTYPE_PIPMLSR_4x3)
        {// Sub PIP
            buf_num = 3;
            dvrif_memory_set_cap_mode(SLR_SUB_DISPLAY, MEMCAPTYPE_FRAME);
        }
        else
        {
            if(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE){
                //update M-domain PQC frame mode @Crixus 20170626
                if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_LINE_MODE){
                    buf_num = 3;
                    dvrif_memory_set_cap_mode(SLR_SUB_DISPLAY, MEMCAPTYPE_LINE);
                } else {
                    buf_num = 3;
                    dvrif_memory_set_cap_mode(SLR_SUB_DISPLAY, MEMCAPTYPE_FRAME);
                }
            }else{
                buf_num = 3;
                dvrif_memory_set_cap_mode(SLR_SUB_DISPLAY, MEMCAPTYPE_FRAME); // memory, capture part for other cases
            }
        }

        dvrif_memory_set_buffer_number(SLR_SUB_DISPLAY, buf_num);

        memory_fs_sub_set_capture();
        memory_fs_sub_set_display();
    }
#endif

	//data fs need to disable compression clock @Crixus 20171227
	libdma_dvrif_memory_comp_setting(FALSE, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_WID), Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN), dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY));
}

/*======================== End of File =======================================*/
/**
*
* @}
*/
