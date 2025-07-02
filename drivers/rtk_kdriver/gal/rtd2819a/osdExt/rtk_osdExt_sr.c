#include <linux/module.h>
#include <linux/string.h>
#include <linux/moduleparam.h>
#include <linux/types.h>    /* size_t         */
#include <linux/export.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>

#include <mach/system.h>
#include <rbus/osdovl_reg.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/osd_sr45_reg.h>
#include <rbus/osd_sr_reg.h>

#include <rtk_kdriver/io.h>
#include <gal/rtk_gdma_driver.h>

#include "rtk_osdExt_driver.h"
#include "rtk_osdExt_debug.h"
// #include <gal/rtk_gdma_split.h>


#define DBGEXT_SR_PRINT(s, args...) GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE ,s, ## args)



//for WBSKTASKWBS-1768 : OSD Scale up issue
//[fixme ?]
#define USE_OSD_SR_ZERO_POS 1

extern int gDebugGDMA_loglevel;


extern gdma_dev *gdma_devices;

#ifdef RTK_GDMA_TEST_OSD1
int GDMA_OSD1_DevCB_initSR(void)
{
	osd_sr_osd_sr_1_scaleup_ctrl0_RBUS osd_sr1_scaleup_ctrl0_reg;

	//drv_superresolution_init
	osd_sr_osd_sr_1_gainmap_ctrl0_RBUS osd_sr1_gainmap_ctl0_reg;

	osd_sr1_gainmap_ctl0_reg.regValue = rtd_inl(OSD_SR_OSD_SR_1_GainMap_Ctrl0_reg);
	osd_sr1_gainmap_ctl0_reg.sr_lpf_range_en = 1;
	osd_sr1_gainmap_ctl0_reg.sr_lpf_range_step = 3;
	osd_sr1_gainmap_ctl0_reg.sr_lpf_edge_en = 1;
	osd_sr1_gainmap_ctl0_reg.sr_lpf_edge_step = 0;
	OSD_RTD_OUTL(OSD_SR_OSD_SR_1_GainMap_Ctrl0_reg, osd_sr1_gainmap_ctl0_reg.regValue);
	OSD_RTD_OUTL(OSD_SR_OSD_SR_2_GainMap_Ctrl0_reg, osd_sr1_gainmap_ctl0_reg.regValue);


	//drv_scaleup_init(void)
	osd_sr1_scaleup_ctrl0_reg.regValue = rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg);
	osd_sr1_scaleup_ctrl0_reg.v_zoom_en = 0;
	osd_sr1_scaleup_ctrl0_reg.ver_ini = 0xff;
	osd_sr1_scaleup_ctrl0_reg.ver_factor = 0;

	OSD_RTD_OUTL(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg, osd_sr1_scaleup_ctrl0_reg.regValue);
	OSD_RTD_OUTL(OSD_SR_OSD_SR_2_Scaleup_Ctrl0_reg, osd_sr1_scaleup_ctrl0_reg.regValue);

	return 0;
}
// drv_PreScale
int GDMA_OSD1_DevCB_preset_SR(int enable, VO_RECTANGLE srcWin, VO_RECTANGLE dispWin, GDMA_PIC_DATA* curPic)
{
	gdma_dev *gdma = &gdma_devices[0];

	osd_sr_osd_sr_1_scaleup_ctrl0_RBUS osd_sr_scaleup_ctrl0_reg;
	osd_sr_osd_sr_1_scaleup_ctrl1_RBUS osd_sr_scaleup_ctrl1_reg;
	osd_sr_osd_sr_1_h_location_ctrl_RBUS osd_sr_h_location_ctrl_reg;
	osd_sr_osd_sr_1_v_location_ctrl_RBUS osd_sr_v_location_ctrl_reg;

	osd_sr_osd_sr_1_ctrl_RBUS osd_sr_ctrl_reg;
	osd_sr_osd_sr_1_gainy_ctrl0_RBUS osd_sr_gainy_ctrl0_reg;
	osd_sr_osd_sr_1_gainmap_ctrl0_RBUS osd_sr_gainmap_ctrl_reg;
	unsigned int ver_factor = 0,width_src=0,width_disp=0;
	unsigned int hor_factor = 0,height_src=0,height_disp=0;
	unsigned int offset = 0, bypass_flag = 0;
	//GDMA_PIC_DATA *curPic;
	GDMA_DISPLAY_PLANE disPlane = curPic->plane;

	GDMA_REG_CONTENT *pReg = NULL;

	//curPic = gdma->pic[disPlane] + ptr_work->picQwr;

	pReg = &curPic->reg_content;

	if (disPlane == GDMA_PLANE_OSD1)
		offset = 0x0;
	else if (disPlane == GDMA_PLANE_OSD2)
		offset = 0x80;



	/* online mode : h_front_porch >= 16, v_back_porch >= 16 */ // old code move to top 
	if ((((dispWin.x+dispWin.width) > (gdma->dispWin[disPlane].width+1)) ||
		((dispWin.y+dispWin.height) > (gdma->dispWin[disPlane].height+1)))) 
	{
			/* Under flow */
			rtd_pr_gdma_debug("%s:dispWin.x = %d,dispWin.y=%d\n", __FUNCTION__, dispWin.x, dispWin.y) ;
			rtd_pr_gdma_debug("%s:dispWin.width = %d,dispWin.height=%d\n", __FUNCTION__, dispWin.width, dispWin.height) ;
			rtd_pr_gdma_debug("%s:gdma->dispWin[%d].width = %d,gdma->dispWin[%d].height=%d\n", __FUNCTION__,
				   disPlane, gdma->dispWin[disPlane].width, disPlane, gdma->dispWin[disPlane].height) ;

			rtd_pr_gdma_debug("%s:data underflow\n", __FUNCTION__) ;
			return -1;
	}

	if (bypass_flag == 0) {
		/* disable LPF and set back to default value when scaling function is NOT enable */
		osd_sr_ctrl_reg.regValue = rtd_inl(OSD_SR_OSD_SR_1_Ctrl_reg+offset);
		osd_sr_ctrl_reg.sr_maxmin_limit_en = 1;
		osd_sr_ctrl_reg.sr_maxmin_shift = 0x0;
		osd_sr_ctrl_reg.sr_var_thd = 0x40;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;

		osd_sr_gainy_ctrl0_reg.regValue = rtd_inl(OSD_SR_OSD_SR_1_GainY_Ctrl0_reg+offset);
		osd_sr_gainy_ctrl0_reg.sr_lpf_gain = 0x0;
		pReg->osd_sr_gainy_ctrl0_reg.regValue = osd_sr_gainy_ctrl0_reg.regValue;

		osd_sr_gainmap_ctrl_reg.regValue = rtd_inl(OSD_SR_OSD_SR_1_GainMap_Ctrl0_reg+offset);
		osd_sr_gainmap_ctrl_reg.sr_lpf_edge_step = 0x3;
		osd_sr_gainmap_ctrl_reg.sr_lpf_range_en = 0;
		osd_sr_gainmap_ctrl_reg.sr_lpf_range_step = 0;
		osd_sr_gainmap_ctrl_reg.sr_lpf_range_thd = 0x0;
		pReg->osd_sr_gainmap_ctrl_reg.regValue = osd_sr_gainmap_ctrl_reg.regValue;
	}

	if (enable && (bypass_flag == 0)) {
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 0;
#if USE_OSD_SR_ZERO_POS
			osd_sr_scaleup_ctrl0_reg.ver_ini = 0xff;
#else
		osd_sr_scaleup_ctrl0_reg.ver_ini = 0;
#endif
		osd_sr_scaleup_ctrl0_reg.ver_factor = 0;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;

		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;
#if USE_OSD_SR_ZERO_POS
		osd_sr_scaleup_ctrl1_reg.hor_ini = 0xff;
#else
		osd_sr_scaleup_ctrl1_reg.hor_ini = 0;
#endif
		osd_sr_scaleup_ctrl1_reg.hor_factor = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;

		{
			osd_sr_v_location_ctrl_reg.osd_height = dispWin.height;
			osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y;
		}
		pReg->osd_sr_v_location_ctrl_reg.regValue = osd_sr_v_location_ctrl_reg.regValue;


		{
			osd_sr_h_location_ctrl_reg.osd_width = dispWin.width;
			osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x;
		}
		pReg->osd_sr_h_location_ctrl_reg.regValue = osd_sr_h_location_ctrl_reg.regValue;

		/*
		OSD_H_STA+OSD_WIDTH , OSD_V_STA+OSD_HEIGHT < video range
		input_width >=16
		*/
		
		DBGEXT_SR_PRINT("%s:srcWin.width = %d,srcWin.height=%d\n", __FUNCTION__, srcWin.width, srcWin.height) ;
		DBGEXT_SR_PRINT("%s:dispWin.width = %d,dispWin.height=%d\n", __FUNCTION__, dispWin.width, dispWin.height) ;

		{
		
			width_src = srcWin.width;
			height_src = srcWin.height;
			width_disp = dispWin.width;
			height_disp = dispWin.height;		
			
			if (dispWin.width != 0)
				hor_factor = (width_src<<20)/width_disp;
			if (dispWin.height != 0)
				ver_factor = (height_src<<20)/height_disp;
			
			#if 0			
			if (dispWin.width != 0)
				hor_factor = (srcWin.width<<20)/dispWin.width;
			if (dispWin.height != 0)
				ver_factor = (srcWin.height<<20)/dispWin.height;
			#endif
			//rtd_pr_gdma_err(KERN_EMERG"%s hor_factor=0x%x,ver_factor=0x%x\n", __FUNCTION__, hor_factor, ver_factor) ;
			DBGEXT_SR_PRINT(KERN_EMERG"%s hor_factor=0x%x,ver_factor=0x%x\n", __FUNCTION__, hor_factor, ver_factor) ;

			if ((srcWin.height < dispWin.height) && (ver_factor)) { /*  no scaling down */
				osd_sr_scaleup_ctrl0_reg.v_zoom_en = 1;
#if USE_OSD_SR_ZERO_POS
				osd_sr_scaleup_ctrl0_reg.ver_ini = 0xff;
#else
				osd_sr_scaleup_ctrl0_reg.ver_ini = srcWin.y;
#endif
				osd_sr_scaleup_ctrl0_reg.ver_factor = ver_factor;

#if 0			/* OSR_SR auto mode */
				if (srcWin.width <= 2560)
					osd_sr_scaleup_ctrl0_reg.ver_tap_num = 1;
				else if (srcWin.width <= 5120)
					osd_sr_scaleup_ctrl0_reg.ver_tap_num = 2;
				if (osd_sr_scaleup_ctrl0_reg.v_zoom_en == 0)
					osd_sr_scaleup_ctrl0_reg.ver_tap_num = 3;
#endif
				pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;
			}

			if ((srcWin.width < dispWin.width) && (hor_factor)) { /*  no scaling down */
				osd_sr_scaleup_ctrl1_reg.h_zoom_en = 1;
#if USE_OSD_SR_ZERO_POS
				osd_sr_scaleup_ctrl1_reg.hor_ini = 0xff;
#else
				osd_sr_scaleup_ctrl1_reg.hor_ini = srcWin.x;
#endif
				osd_sr_scaleup_ctrl1_reg.hor_factor = hor_factor;
				pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;
			}

			/* enhance OSD sharpness by designer suggestion */
			if (osd_sr_scaleup_ctrl0_reg.v_zoom_en || osd_sr_scaleup_ctrl1_reg.h_zoom_en) {
				osd_sr_ctrl_reg.sr_maxmin_limit_en = 1;
				osd_sr_ctrl_reg.sr_maxmin_shift = 0x20;
				osd_sr_ctrl_reg.sr_var_thd = 0x20;
				pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;

				osd_sr_gainy_ctrl0_reg.sr_lpf_gain = 0x1f;
				pReg->osd_sr_gainy_ctrl0_reg.regValue = osd_sr_gainy_ctrl0_reg.regValue;

				osd_sr_gainmap_ctrl_reg.sr_lpf_edge_step = 0x0;
				osd_sr_gainmap_ctrl_reg.sr_lpf_range_en = 1;
				osd_sr_gainmap_ctrl_reg.sr_lpf_range_step = 1;
				osd_sr_gainmap_ctrl_reg.sr_lpf_range_thd = 0x8;
				pReg->osd_sr_gainmap_ctrl_reg.regValue = osd_sr_gainmap_ctrl_reg.regValue;
			}
		}
		DBGEXT_SR_PRINT("OSD_SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, pReg->osd_sr_scaleup_ctrl0_reg.regValue) ;
		DBGEXT_SR_PRINT("OSD_SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, pReg->osd_sr_scaleup_ctrl1_reg.regValue) ;
		DBGEXT_SR_PRINT("OSD_SR_X_V_Location_Ctrl_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset, pReg->osd_sr_v_location_ctrl_reg.regValue) ;
		DBGEXT_SR_PRINT("OSD_SR_X_H_Location_Ctrl_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset, pReg->osd_sr_h_location_ctrl_reg.regValue) ;
	} else {
		/*  OSD_SR 1/2/3 disable */
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 0;
		osd_sr_scaleup_ctrl0_reg.ver_ini = 0x00;
		osd_sr_scaleup_ctrl0_reg.ver_factor = 0;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;

		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;
		osd_sr_scaleup_ctrl1_reg.hor_ini = 0;
		osd_sr_scaleup_ctrl1_reg.hor_factor = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;
		DBGEXT_SR_PRINT("OSD_SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, pReg->osd_sr_scaleup_ctrl0_reg.regValue) ;
		DBGEXT_SR_PRINT("OSD_SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, pReg->osd_sr_scaleup_ctrl1_reg.regValue) ;

		osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y;
		osd_sr_v_location_ctrl_reg.osd_height = dispWin.height;
		pReg->osd_sr_v_location_ctrl_reg.regValue = osd_sr_v_location_ctrl_reg.regValue;

		osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x;
		osd_sr_h_location_ctrl_reg.osd_width = dispWin.width;
		pReg->osd_sr_h_location_ctrl_reg.regValue = osd_sr_h_location_ctrl_reg.regValue;
	}

	#if 0
	if(change_OSD_mode == GDMA_SPLIT_MODE_1 ||
	  change_OSD_mode == GDMA_SPLIT_MODE_2 ||
	  change_OSD_mode == GDMA_SPLIT_MODE_3 ||
	  (gGDMA_Monitor_ctl.enable==1)
	 ) {
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 1;
		osd_sr_scaleup_ctrl0_reg.ver_factor = 0xfffff;
		osd_sr_scaleup_ctrl0_reg.ver_tap_num = 0x3;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;
		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;	
		osd_sr_ctrl_reg.identity_en = 1;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;
	}else
	#endif//
	{
		osd_sr_ctrl_reg.identity_en = 0;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;
	}

	if (!osd_sr_scaleup_ctrl0_reg.v_zoom_en && !osd_sr_scaleup_ctrl1_reg.h_zoom_en) {
		osd_sr_ctrl_reg.identity_en = 1;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;
	}

	return GDMA_SUCCESS;
}

//int drv_scaleup(uint8_t enable, GDMA_DISPLAY_PLANE disPlane)
int GDMA_OSD1_DevCB_set_SR(uint8_t enable, GDMA_DISPLAY_PLANE disPlane, GDMA_PIC_DATA* curPic)
{
	unsigned int offset = 0;
	GDMA_REG_CONTENT *pReg = NULL;

	
	pReg = &curPic->reg_content;

	if (disPlane == GDMA_PLANE_OSD1)
		offset = 0x0;
	else if (disPlane == GDMA_PLANE_OSD2)
		offset = 0x80;
	else if (disPlane == GDMA_PLANE_OSD3)
		offset = 0x100;
	else {
		DBGEXT_SR_PRINT(KERN_EMERG"%s, No match with OSDSRx_switch_to to OSD%d !! \n", __FUNCTION__, disPlane) ;
		return GDMA_SUCCESS;
	}


	if (enable) {
		OSD_RTD_OUTL(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, pReg->osd_sr_scaleup_ctrl0_reg.regValue);
		OSD_RTD_OUTL(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, pReg->osd_sr_scaleup_ctrl1_reg.regValue);
		OSD_RTD_OUTL(OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset, pReg->osd_sr_v_location_ctrl_reg.regValue);
		OSD_RTD_OUTL(OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset, pReg->osd_sr_h_location_ctrl_reg.regValue);

		/* enhance OSD sharpness by designer suggestion */
		OSD_RTD_OUTL(OSD_SR_OSD_SR_1_Ctrl_reg+offset, pReg->osd_sr_ctrl_reg.regValue);
		OSD_RTD_OUTL(OSD_SR_OSD_SR_1_GainY_Ctrl0_reg+offset, pReg->osd_sr_gainy_ctrl0_reg.regValue);
		OSD_RTD_OUTL(OSD_SR_OSD_SR_1_GainMap_Ctrl0_reg+offset, pReg->osd_sr_gainmap_ctrl_reg.regValue);

		if ((pReg->osd_sr_scaleup_ctrl1_reg.h_zoom_en != 0) || (pReg->osd_sr_scaleup_ctrl0_reg.v_zoom_en != 0))
			fwif_color_set_scaleosd_sharpness_driver(0, 1);
		else
			fwif_color_set_scaleosd_sharpness_driver(0, 0);

		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset)) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset)) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_V_Location_Ctrl_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset)) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_H_Location_Ctrl_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset)) ;
	} else {
		/*  OSD_SR 1/2/3 disable */
		OSD_RTD_OUTL(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, pReg->osd_sr_scaleup_ctrl0_reg.regValue);
		OSD_RTD_OUTL(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, pReg->osd_sr_scaleup_ctrl1_reg.regValue);
		OSD_RTD_OUTL(OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset, pReg->osd_sr_v_location_ctrl_reg.regValue);
		OSD_RTD_OUTL(OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset, pReg->osd_sr_h_location_ctrl_reg.regValue);

		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset)) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset)) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_V_Location_Ctrl_reg[0x%x]		 0x%x\n", OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset)) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_H_Location_Ctrl_reg[0x%x]		 0x%x\n", OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset)) ;
	}


	return GDMA_SUCCESS;
}

#endif  //  RTK_GDMA_TEST_OSD1


#ifdef RTK_ENABLE_GDMA_EXT

//int GDMAExt_SR_init( void ) 
int GDMA_OSD4_DevCB_initSR(void)
{
	//drv_superresolution_init
	osd_sr45_osd_sr_4_gainmap_ctrl0_RBUS gainmap_ctl0_reg;

	gainmap_ctl0_reg.regValue = rtd_inl(OSD_SR45_OSD_SR_4_GainMap_Ctrl0_reg);
	gainmap_ctl0_reg.sr_lpf_range_en = 1;
	gainmap_ctl0_reg.sr_lpf_range_step = 3;
	gainmap_ctl0_reg.sr_lpf_edge_en = 1;
	gainmap_ctl0_reg.sr_lpf_edge_step = 0;
	OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_GainMap_Ctrl0_reg, gainmap_ctl0_reg.regValue);

	//drv_scaleup_init(void)
	{
		osd_sr45_osd_sr_4_scaleup_ctrl0_RBUS  scaleup_ctrl0_reg;
		scaleup_ctrl0_reg.regValue = rtd_inl( OSD_SR45_OSD_SR_4_Scaleup_Ctrl0_reg );
		scaleup_ctrl0_reg.v_zoom_en = 0;
		scaleup_ctrl0_reg.ver_ini = 0xff;
		scaleup_ctrl0_reg.ver_factor = 0;

	OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_Scaleup_Ctrl0_reg, scaleup_ctrl0_reg.regValue);

	}
	return 0;
}


//int GDMAExt_SR_PreScale(int enable, VO_RECTANGLE srcWin, VO_RECTANGLE dispWin, GDMA_PIC_DATA* curPic)
int GDMA_OSD4_DevCB_preset_SR(int enable, VO_RECTANGLE srcWin, VO_RECTANGLE dispWin, GDMA_PIC_DATA* curPic)
{
	gdma_dev *gdma = &gdma_devices[0];


	osd_sr45_osd_sr_4_scaleup_ctrl0_RBUS osd_sr_scaleup_ctrl0_reg;
	osd_sr45_osd_sr_4_scaleup_ctrl1_RBUS osd_sr_scaleup_ctrl1_reg;
	osd_sr45_osd_sr_4_h_location_ctrl_RBUS osd_sr_h_location_ctrl_reg;
	osd_sr45_osd_sr_4_v_location_ctrl_RBUS osd_sr_v_location_ctrl_reg;

	osd_sr45_osd_sr_4_ctrl_RBUS osd_sr_ctrl_reg;
	osd_sr45_osd_sr_4_gainy_ctrl0_RBUS osd_sr_gainy_ctrl0_reg;
	osd_sr45_osd_sr_4_gainmap_ctrl0_RBUS osd_sr_gainmap_ctrl_reg;
	unsigned int ver_factor = 0, width_src=0, width_disp=0;
	unsigned int hor_factor = 0, height_src=0, height_disp=0;
	unsigned int bypass_flag = 0;
	//GDMA_PIC_DATA *curPic;
	GDMA_DISPLAY_PLANE disPlane = curPic->plane;

	GDMA_4_REG_CONTENT *pReg = NULL;

	//curPic = gdma->pic[disPlane] + ptr_work->picQwr;

	pReg = &curPic->reg_4_content;

	width_src = srcWin.width;
	height_src = srcWin.height;
	width_disp = dispWin.width;
	height_disp = dispWin.height;


	if( disPlane != GDMA_PLANE_OSD4) {
		GDMAEXT_ERROR("invalid plane when set osd 4 :%d", disPlane);
	}

	/* online mode : h_front_porch >= 16, v_back_porch >= 16 */ // old code move to top 
	if ((((dispWin.x+dispWin.width) > (gdma->dispWin[disPlane].width+1)) ||
		((dispWin.y+dispWin.height) > (gdma->dispWin[disPlane].height+1)))) 
	{
			// caller's dispWin > device dispWin => may occur in 4/5 testing env.
			// use device disp to replace caller's one 
			
			#if 0
			GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "%s:curDisp %d %d %d %d. deviceDisp:%d %d %d %d err\n",  __FUNCTION__, 
				dispWin.x, dispWin.y, dispWin.width, dispWin.height, 
				gdma->dispWin[disPlane].x, gdma->dispWin[disPlane].y,
				gdma->dispWin[disPlane].width, gdma->dispWin[disPlane].height) ;

				return -1;
			#else
				memcpy(&dispWin, &(gdma->dispWin[disPlane]), sizeof(dispWin) ) ;

			#endif//

			//GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "%s:data underflow\n", __FUNCTION__) ;

			
	}

	/*
		OSD_H_STA+OSD_WIDTH , OSD_V_STA+OSD_HEIGHT < video range
		input_width >=16
	*/
		
	DBGEXT_SR_PRINT("oPlane:%d srcWin:%d %d dispWin:%d %d\n", disPlane, srcWin.width, srcWin.height, dispWin.width, dispWin.height) ;

	if (dispWin.width != 0)
		hor_factor = (width_src<<20)/width_disp;

	if (dispWin.height != 0)
		ver_factor = (height_src<<20)/height_disp;
	
	#if 0			
	if (dispWin.width != 0)
		hor_factor = (srcWin.width<<20)/dispWin.width;
	if (dispWin.height != 0)
		ver_factor = (srcWin.height<<20)/dispWin.height;
	#endif


	if (bypass_flag == 0) {
		/* disable LPF and set back to default value when scaling function is NOT enable */
		osd_sr_ctrl_reg.regValue = rtd_inl( OSD_SR45_OSD_SR_4_Ctrl_reg );
		osd_sr_ctrl_reg.sr_maxmin_shift = 0x20;
		osd_sr_ctrl_reg.sr_var_thd = 0x20;
		osd_sr_ctrl_reg.sr_maxmin_limit_en = 1;
		//osd_sr_ctrl_reg.rgb_extension_mode = 0;
		//osd_sr_ctrl_reg.YUV_data = 0; //0: rgb

		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;

		osd_sr_gainy_ctrl0_reg.regValue = rtd_inl( OSD_SR45_OSD_SR_4_GainY_Ctrl0_reg );
		osd_sr_gainy_ctrl0_reg.sr_lpf_gain = 0x0;
		pReg->osd_sr_gainy_ctrl0_reg.regValue = osd_sr_gainy_ctrl0_reg.regValue;

		osd_sr_gainmap_ctrl_reg.regValue = rtd_inl( OSD_SR45_OSD_SR_4_GainMap_Ctrl0_reg );
		osd_sr_gainmap_ctrl_reg.sr_lpf_edge_step = 0x3;
		osd_sr_gainmap_ctrl_reg.sr_lpf_range_en = 0;
		osd_sr_gainmap_ctrl_reg.sr_lpf_range_step = 0;
		osd_sr_gainmap_ctrl_reg.sr_lpf_range_thd = 0x0;
		pReg->osd_sr_gainmap_ctrl_reg.regValue = osd_sr_gainmap_ctrl_reg.regValue;
	}

	if (enable && (bypass_flag == 0)) {
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 0;

		#if USE_OSD_SR_ZERO_POS
			osd_sr_scaleup_ctrl0_reg.ver_ini = 0xff;
		#else
		osd_sr_scaleup_ctrl0_reg.ver_ini = 0;
		#endif

		osd_sr_scaleup_ctrl0_reg.ver_factor = 0;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;

		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;

		#if USE_OSD_SR_ZERO_POS
		osd_sr_scaleup_ctrl1_reg.hor_ini = 0xff;
		#else
		osd_sr_scaleup_ctrl1_reg.hor_ini = 0;
		#endif

		osd_sr_scaleup_ctrl1_reg.hor_factor = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;

		{
			osd_sr_v_location_ctrl_reg.osd_height = dispWin.height;
			osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y;
		}
		pReg->osd_sr_v_location_ctrl_reg.regValue = osd_sr_v_location_ctrl_reg.regValue;


		{
			osd_sr_h_location_ctrl_reg.osd_width = dispWin.width;
			osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x;
		}
		pReg->osd_sr_h_location_ctrl_reg.regValue = osd_sr_h_location_ctrl_reg.regValue;

		
		{
			DBGEXT_SR_PRINT("%s hor_factor=0x%x,ver_factor=0x%x\n", __FUNCTION__, hor_factor, ver_factor) ;


			if ((srcWin.height < dispWin.height) && (ver_factor)) { 
				osd_sr_scaleup_ctrl0_reg.v_zoom_en = 1;
				
				#if USE_OSD_SR_ZERO_POS
					osd_sr_scaleup_ctrl0_reg.ver_ini = 0xff;
				#else
					osd_sr_scaleup_ctrl0_reg.ver_ini = srcWin.y;
				#endif

				osd_sr_scaleup_ctrl0_reg.ver_factor = ver_factor;

			#if 0			/* OSR_SR auto mode */
				if (srcWin.width <= 2560)
					osd_sr_scaleup_ctrl0_reg.ver_tap_num = 1;
				else if (srcWin.width <= 5120)
					osd_sr_scaleup_ctrl0_reg.ver_tap_num = 2;
				if (osd_sr_scaleup_ctrl0_reg.v_zoom_en == 0)
					osd_sr_scaleup_ctrl0_reg.ver_tap_num = 3;
			#endif

				pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;
			}

			if ((srcWin.width < dispWin.width) && (hor_factor)) {

				osd_sr_scaleup_ctrl1_reg.h_zoom_en = 1;

				#if USE_OSD_SR_ZERO_POS
					osd_sr_scaleup_ctrl1_reg.hor_ini = 0xff;
				#else
					osd_sr_scaleup_ctrl1_reg.hor_ini = srcWin.x;
				#endif

				osd_sr_scaleup_ctrl1_reg.hor_factor = hor_factor;
				pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;
			}

			/* enhance OSD sharpness by designer suggestion */
			if (osd_sr_scaleup_ctrl0_reg.v_zoom_en || osd_sr_scaleup_ctrl1_reg.h_zoom_en) {
				osd_sr_ctrl_reg.sr_maxmin_limit_en = 1;
				osd_sr_ctrl_reg.sr_maxmin_shift = 0x20;
				osd_sr_ctrl_reg.sr_var_thd = 0x20;
				pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;

				osd_sr_gainy_ctrl0_reg.sr_lpf_gain = 0x1f;
				pReg->osd_sr_gainy_ctrl0_reg.regValue = osd_sr_gainy_ctrl0_reg.regValue;

				osd_sr_gainmap_ctrl_reg.sr_lpf_edge_step = 0x0;
				osd_sr_gainmap_ctrl_reg.sr_lpf_range_en = 1;
				osd_sr_gainmap_ctrl_reg.sr_lpf_range_step = 1;
				osd_sr_gainmap_ctrl_reg.sr_lpf_range_thd = 0x8;
				pReg->osd_sr_gainmap_ctrl_reg.regValue = osd_sr_gainmap_ctrl_reg.regValue;
			}
		}
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl0_reg[0x%x]		0x%x\n", OSD_SR45_OSD_SR_4_Scaleup_Ctrl0_reg, pReg->osd_sr_scaleup_ctrl0_reg.regValue) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl1_reg[0x%x]     0x%x\n", OSD_SR45_OSD_SR_4_Scaleup_Ctrl1_reg, pReg->osd_sr_scaleup_ctrl1_reg.regValue) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_V_Location_Ctrl_reg[0x%x]   0x%x\n", OSD_SR45_OSD_SR_4_V_Location_Ctrl_reg, pReg->osd_sr_v_location_ctrl_reg.regValue) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_H_Location_Ctrl_reg[0x%x]   0x%x\n", OSD_SR45_OSD_SR_4_H_Location_Ctrl_reg, pReg->osd_sr_h_location_ctrl_reg.regValue) ;
	} else {
		/*  OSD_SR 1/2/3 disable */
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 0;
		osd_sr_scaleup_ctrl0_reg.ver_ini = 0x00;
		osd_sr_scaleup_ctrl0_reg.ver_factor = 0;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;

		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;
		osd_sr_scaleup_ctrl1_reg.hor_ini = 0;
		osd_sr_scaleup_ctrl1_reg.hor_factor = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_4_Scaleup_Ctrl0_reg, pReg->osd_sr_scaleup_ctrl0_reg.regValue) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_4_Scaleup_Ctrl1_reg, pReg->osd_sr_scaleup_ctrl1_reg.regValue) ;

		osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y;
		osd_sr_v_location_ctrl_reg.osd_height = dispWin.height;
		pReg->osd_sr_v_location_ctrl_reg.regValue = osd_sr_v_location_ctrl_reg.regValue;

		osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x;
		osd_sr_h_location_ctrl_reg.osd_width = dispWin.width;
		pReg->osd_sr_h_location_ctrl_reg.regValue = osd_sr_h_location_ctrl_reg.regValue;
	}

	#if 0
	if(change_OSD_mode == GDMA_SPLIT_MODE_1 ||
	  change_OSD_mode == GDMA_SPLIT_MODE_2 ||
	  change_OSD_mode == GDMA_SPLIT_MODE_3 ||
	  (gGDMA_Monitor_ctl.enable==1)
	 ) {
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 1;
		osd_sr_scaleup_ctrl0_reg.ver_factor = 0xfffff;
		osd_sr_scaleup_ctrl0_reg.ver_tap_num = 0x3;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;
		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;	
		osd_sr_ctrl_reg.identity_en = 1;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;
	}else
	#endif//
	if( gdma->enable_identity[GDMA_PLANE_OSD4] )
	{
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 1;
		osd_sr_scaleup_ctrl0_reg.ver_factor = 0xfffff;
		osd_sr_scaleup_ctrl0_reg.ver_tap_num = 0x3;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;

		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;	


		osd_sr_h_location_ctrl_reg.osd_width =  width_src;
		pReg->osd_sr_h_location_ctrl_reg.regValue = osd_sr_h_location_ctrl_reg.regValue;

		osd_sr_v_location_ctrl_reg.osd_height = height_src;
		pReg->osd_sr_v_location_ctrl_reg.regValue = osd_sr_v_location_ctrl_reg.regValue;

		osd_sr_ctrl_reg.identity_en = 1;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;
		//osd OSD_WIDTH/h need == input size 

	}
	else 
	{
		osd_sr_ctrl_reg.identity_en = 0;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;
	}

	#if 0
	if (!osd_sr_scaleup_ctrl0_reg.v_zoom_en && !osd_sr_scaleup_ctrl1_reg.h_zoom_en) {
		osd_sr_ctrl_reg.identity_en = 1;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;
	}
	#endif//

	return GDMA_SUCCESS;
}


int GDMA_OSD4_DevCB_set_SR(uint8_t enable, GDMA_DISPLAY_PLANE disPlane, GDMA_PIC_DATA* curPic)
{
	GDMA_4_REG_CONTENT *pReg = NULL;

	pReg = &curPic->reg_4_content;


	if (enable) {
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_Scaleup_Ctrl0_reg	, pReg->osd_sr_scaleup_ctrl0_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_Scaleup_Ctrl1_reg, pReg->osd_sr_scaleup_ctrl1_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_V_Location_Ctrl_reg, pReg->osd_sr_v_location_ctrl_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_H_Location_Ctrl_reg, pReg->osd_sr_h_location_ctrl_reg.regValue);

		/* enhance OSD sharpness by designer suggestion */
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_Ctrl_reg, pReg->osd_sr_ctrl_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_GainY_Ctrl0_reg, pReg->osd_sr_gainy_ctrl0_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_GainMap_Ctrl0_reg, pReg->osd_sr_gainmap_ctrl_reg.regValue);

		#if 0
		if ((pReg->osd_sr_scaleup_ctrl1_reg.h_zoom_en != 0) || (pReg->osd_sr_scaleup_ctrl0_reg.v_zoom_en != 0))
			fwif_color_set_scaleosd_sharpness_driver(0, 1);
		else
			fwif_color_set_scaleosd_sharpness_driver(0, 0);
		#endif//

		DBGEXT_SR_PRINT("out SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_4_Scaleup_Ctrl0_reg, rtd_inl(OSD_SR45_OSD_SR_4_Scaleup_Ctrl0_reg)) ;
		DBGEXT_SR_PRINT("out SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_4_Scaleup_Ctrl1_reg, rtd_inl(OSD_SR45_OSD_SR_4_Scaleup_Ctrl1_reg)) ;
		DBGEXT_SR_PRINT("out SR_X_V_Location_Ctrl_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_4_V_Location_Ctrl_reg, rtd_inl(OSD_SR45_OSD_SR_4_V_Location_Ctrl_reg)) ;
		DBGEXT_SR_PRINT("out SR_X_H_Location_Ctrl_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_4_H_Location_Ctrl_reg, rtd_inl(OSD_SR45_OSD_SR_4_H_Location_Ctrl_reg)) ;
	} else {
		/*  OSD_SR disable */
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_Scaleup_Ctrl0_reg, pReg->osd_sr_scaleup_ctrl0_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_Scaleup_Ctrl1_reg, pReg->osd_sr_scaleup_ctrl1_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_V_Location_Ctrl_reg, pReg->osd_sr_v_location_ctrl_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_4_H_Location_Ctrl_reg, pReg->osd_sr_h_location_ctrl_reg.regValue);

		DBGEXT_SR_PRINT("out SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_4_Scaleup_Ctrl0_reg, rtd_inl(OSD_SR45_OSD_SR_4_Scaleup_Ctrl0_reg)) ;
		DBGEXT_SR_PRINT("out SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_4_Scaleup_Ctrl1_reg, rtd_inl(OSD_SR45_OSD_SR_4_Scaleup_Ctrl1_reg)) ;
		DBGEXT_SR_PRINT("out SR_X_V_Location_Ctrl_reg[0x%x]		 0x%x\n", OSD_SR45_OSD_SR_4_V_Location_Ctrl_reg, rtd_inl(OSD_SR45_OSD_SR_4_V_Location_Ctrl_reg)) ;
		DBGEXT_SR_PRINT("out SR_X_H_Location_Ctrl_reg[0x%x]		 0x%x\n", OSD_SR45_OSD_SR_4_H_Location_Ctrl_reg, rtd_inl(OSD_SR45_OSD_SR_4_H_Location_Ctrl_reg)) ;
	}


	return GDMA_SUCCESS;
}


	/////////////////   OSD5 			/////////////////////////////////////////


int GDMA_OSD5_DevCB_initSR(void)
{
	//drv_superresolution_init
	osd_sr45_osd_sr_5_gainmap_ctrl0_RBUS gainmap_ctl0_reg;

	gainmap_ctl0_reg.regValue = rtd_inl(OSD_SR45_OSD_SR_5_GainMap_Ctrl0_reg);
	gainmap_ctl0_reg.sr_lpf_range_en = 1;
	gainmap_ctl0_reg.sr_lpf_range_step = 3;
	gainmap_ctl0_reg.sr_lpf_edge_en = 1;
	gainmap_ctl0_reg.sr_lpf_edge_step = 0;
	OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_GainMap_Ctrl0_reg, gainmap_ctl0_reg.regValue);


	//drv_scaleup_init(void)
	{
		osd_sr45_osd_sr_5_scaleup_ctrl0_RBUS  scaleup_ctrl0_reg;

		scaleup_ctrl0_reg.regValue = rtd_inl( OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg );
		scaleup_ctrl0_reg.v_zoom_en = 0;
		scaleup_ctrl0_reg.ver_ini = 0xff;
		scaleup_ctrl0_reg.ver_factor = 0;

	OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg, scaleup_ctrl0_reg.regValue);

	}

	return 0;
}

int GDMA_OSD5_DevCB_preset_SR(int enable, VO_RECTANGLE srcWin, VO_RECTANGLE dispWin, GDMA_PIC_DATA* curPic)
{
	gdma_dev *gdma = &gdma_devices[0];


	osd_sr45_osd_sr_5_scaleup_ctrl0_RBUS osd_sr_scaleup_ctrl0_reg;
	osd_sr45_osd_sr_5_scaleup_ctrl1_RBUS osd_sr_scaleup_ctrl1_reg;
	osd_sr45_osd_sr_5_h_location_ctrl_RBUS osd_sr_h_location_ctrl_reg;
	osd_sr45_osd_sr_5_v_location_ctrl_RBUS osd_sr_v_location_ctrl_reg;

	osd_sr45_osd_sr_5_ctrl_RBUS osd_sr_ctrl_reg;
	osd_sr45_osd_sr_5_gainy_ctrl0_RBUS osd_sr_gainy_ctrl0_reg;
	osd_sr45_osd_sr_5_gainmap_ctrl0_RBUS osd_sr_gainmap_ctrl_reg;
	unsigned int ver_factor = 0, width_src=0, width_disp=0;
	unsigned int hor_factor = 0, height_src=0, height_disp=0;
	unsigned int  bypass_flag = 0;
	//GDMA_PIC_DATA *curPic;
	GDMA_DISPLAY_PLANE disPlane = curPic->plane;

	GDMA_5_REG_CONTENT *pReg = NULL;

	//curPic = gdma->pic[disPlane] + ptr_work->picQwr;
	osd_sr_scaleup_ctrl0_reg.regValue = 0;
	osd_sr_scaleup_ctrl1_reg.regValue = 0;
	osd_sr_h_location_ctrl_reg.regValue = 0;
	osd_sr_v_location_ctrl_reg.regValue = 0;


	pReg = &curPic->reg_5_content;

	width_src = srcWin.width;
	height_src = srcWin.height;
	width_disp = dispWin.width;
	height_disp = dispWin.height;

	/* online mode : h_front_porch >= 16, v_back_porch >= 16 */ // old code move to top 
	if ((((dispWin.x+dispWin.width) > (gdma->dispWin[disPlane].width+1)) ||
		((dispWin.y+dispWin.height) > (gdma->dispWin[disPlane].height+1)))) 
	{
			#if 0
			GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "%s:curDisp %d %d %d %d. deviceDisp:%d %d %d %d err\n",  __FUNCTION__, 
				dispWin.x, dispWin.y, dispWin.width, dispWin.height, 
				gdma->dispWin[disPlane].x, gdma->dispWin[disPlane].y,
				gdma->dispWin[disPlane].width, gdma->dispWin[disPlane].height) ;

				return -1;
			#else
				memcpy(&dispWin, &(gdma->dispWin[disPlane]), sizeof(dispWin) ) ;

			#endif//
	}

	/*
		OSD_H_STA+OSD_WIDTH , OSD_V_STA+OSD_HEIGHT < video range
		input_width >=16
	*/
		
	DBGEXT_SR_PRINT("oPlane:%d srcWin:%d %d dispWin:%d %d\n", disPlane, srcWin.width, srcWin.height, dispWin.width, dispWin.height) ;

	if (dispWin.width != 0)
		hor_factor = (width_src<<20)/width_disp;

	if (dispWin.height != 0)
		ver_factor = (height_src<<20)/height_disp;
	
	#if 0			
	if (dispWin.width != 0)
		hor_factor = (srcWin.width<<20)/dispWin.width;
	if (dispWin.height != 0)
		ver_factor = (srcWin.height<<20)/dispWin.height;
	#endif


	if (bypass_flag == 0) {
		/* disable LPF and set back to default value when scaling function is NOT enable */
		osd_sr_ctrl_reg.regValue = rtd_inl( OSD_SR45_OSD_SR_5_Ctrl_reg );
		osd_sr_ctrl_reg.sr_maxmin_limit_en = 1;
		osd_sr_ctrl_reg.sr_maxmin_shift = 0x0;
		osd_sr_ctrl_reg.sr_var_thd = 0x40;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;

		osd_sr_gainy_ctrl0_reg.regValue = rtd_inl( OSD_SR45_OSD_SR_5_GainY_Ctrl0_reg );
		osd_sr_gainy_ctrl0_reg.sr_lpf_gain = 0x0;
		pReg->osd_sr_gainy_ctrl0_reg.regValue = osd_sr_gainy_ctrl0_reg.regValue;

		osd_sr_gainmap_ctrl_reg.regValue = rtd_inl( OSD_SR45_OSD_SR_5_GainMap_Ctrl0_reg );
		osd_sr_gainmap_ctrl_reg.sr_lpf_edge_step = 0x3;
		osd_sr_gainmap_ctrl_reg.sr_lpf_range_en = 0;
		osd_sr_gainmap_ctrl_reg.sr_lpf_range_step = 0;
		osd_sr_gainmap_ctrl_reg.sr_lpf_range_thd = 0x0;
		pReg->osd_sr_gainmap_ctrl_reg.regValue = osd_sr_gainmap_ctrl_reg.regValue;
	}

	if (enable && (bypass_flag == 0)) {
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 0;

		#if USE_OSD_SR_ZERO_POS
			osd_sr_scaleup_ctrl0_reg.ver_ini = 0xff;
		#else
		osd_sr_scaleup_ctrl0_reg.ver_ini = 0;
		#endif

		osd_sr_scaleup_ctrl0_reg.ver_factor = 0;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;

		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;

		#if USE_OSD_SR_ZERO_POS
		osd_sr_scaleup_ctrl1_reg.hor_ini = 0xff;
		#else
		osd_sr_scaleup_ctrl1_reg.hor_ini = 0;
		#endif

		osd_sr_scaleup_ctrl1_reg.hor_factor = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;

		{
			osd_sr_v_location_ctrl_reg.osd_height = dispWin.height;
			osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y;
		}
		pReg->osd_sr_v_location_ctrl_reg.regValue = osd_sr_v_location_ctrl_reg.regValue;


		{
			osd_sr_h_location_ctrl_reg.osd_width = dispWin.width;
			osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x;
		}
		pReg->osd_sr_h_location_ctrl_reg.regValue = osd_sr_h_location_ctrl_reg.regValue;

		
		{
			DBGEXT_SR_PRINT("%s hor_factor=0x%x,ver_factor=0x%x\n", __FUNCTION__, hor_factor, ver_factor) ;


			if ((srcWin.height < dispWin.height) && (ver_factor)) { /*  no scaling down */
				osd_sr_scaleup_ctrl0_reg.v_zoom_en = 1;
				
			#if USE_OSD_SR_ZERO_POS
				osd_sr_scaleup_ctrl0_reg.ver_ini = 0xff;
			#else
				osd_sr_scaleup_ctrl0_reg.ver_ini = srcWin.y;
			#endif

				osd_sr_scaleup_ctrl0_reg.ver_factor = ver_factor;

			#if 0			/* OSR_SR auto mode */
				if (srcWin.width <= 2560)
					osd_sr_scaleup_ctrl0_reg.ver_tap_num = 1;
				else if (srcWin.width <= 5120)
					osd_sr_scaleup_ctrl0_reg.ver_tap_num = 2;
				if (osd_sr_scaleup_ctrl0_reg.v_zoom_en == 0)
					osd_sr_scaleup_ctrl0_reg.ver_tap_num = 3;
			#endif

				pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;
			}

			if ((srcWin.width < dispWin.width) && (hor_factor)) { /*  no scaling down */
				osd_sr_scaleup_ctrl1_reg.h_zoom_en = 1;

			#if USE_OSD_SR_ZERO_POS
				osd_sr_scaleup_ctrl1_reg.hor_ini = 0xff;
			#else
				osd_sr_scaleup_ctrl1_reg.hor_ini = srcWin.x;
			#endif

				osd_sr_scaleup_ctrl1_reg.hor_factor = hor_factor;
				pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;
			}

			/* enhance OSD sharpness by designer suggestion */
			if (osd_sr_scaleup_ctrl0_reg.v_zoom_en || osd_sr_scaleup_ctrl1_reg.h_zoom_en) {
				osd_sr_ctrl_reg.sr_maxmin_limit_en = 1;
				osd_sr_ctrl_reg.sr_maxmin_shift = 0x20;
				osd_sr_ctrl_reg.sr_var_thd = 0x20;
				pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;

				osd_sr_gainy_ctrl0_reg.sr_lpf_gain = 0x1f;
				pReg->osd_sr_gainy_ctrl0_reg.regValue = osd_sr_gainy_ctrl0_reg.regValue;

				osd_sr_gainmap_ctrl_reg.sr_lpf_edge_step = 0x0;
				osd_sr_gainmap_ctrl_reg.sr_lpf_range_en = 1;
				osd_sr_gainmap_ctrl_reg.sr_lpf_range_step = 1;
				osd_sr_gainmap_ctrl_reg.sr_lpf_range_thd = 0x8;
				pReg->osd_sr_gainmap_ctrl_reg.regValue = osd_sr_gainmap_ctrl_reg.regValue;
			}
		}
		DBGEXT_SR_PRINT("preSet Scaleup_Ctrl0[0x%x]		0x%x\n", OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg, pReg->osd_sr_scaleup_ctrl0_reg.regValue) ;
		DBGEXT_SR_PRINT("preSet Scaleup_Ctrl1 [0x%x]     0x%x\n", OSD_SR45_OSD_SR_5_Scaleup_Ctrl1_reg, pReg->osd_sr_scaleup_ctrl1_reg.regValue) ;
		DBGEXT_SR_PRINT("preSet V_Location_Ctrl_reg[0x%x]   0x%x\n", OSD_SR45_OSD_SR_5_V_Location_Ctrl_reg, pReg->osd_sr_v_location_ctrl_reg.regValue) ;
		DBGEXT_SR_PRINT("OpreSet H_Location_Ctrl_reg[0x%x]   0x%x\n", OSD_SR45_OSD_SR_5_H_Location_Ctrl_reg, pReg->osd_sr_h_location_ctrl_reg.regValue) ;
	} else {
		/*  OSD_SR 1/2/3 disable */
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 0;
		osd_sr_scaleup_ctrl0_reg.ver_ini = 0x00;
		osd_sr_scaleup_ctrl0_reg.ver_factor = 0;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;

		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;
		osd_sr_scaleup_ctrl1_reg.hor_ini = 0;
		osd_sr_scaleup_ctrl1_reg.hor_factor = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;
		DBGEXT_SR_PRINT("preSet Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg, pReg->osd_sr_scaleup_ctrl0_reg.regValue) ;
		DBGEXT_SR_PRINT("preSet Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_5_Scaleup_Ctrl1_reg, pReg->osd_sr_scaleup_ctrl1_reg.regValue) ;

		osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y;
		osd_sr_v_location_ctrl_reg.osd_height = dispWin.height;
		pReg->osd_sr_v_location_ctrl_reg.regValue = osd_sr_v_location_ctrl_reg.regValue;

		osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x;
		osd_sr_h_location_ctrl_reg.osd_width = dispWin.width;
		pReg->osd_sr_h_location_ctrl_reg.regValue = osd_sr_h_location_ctrl_reg.regValue;
	}

	#if 0
	if(change_OSD_mode == GDMA_SPLIT_MODE_1 ||
	  change_OSD_mode == GDMA_SPLIT_MODE_2 ||
	  change_OSD_mode == GDMA_SPLIT_MODE_3 ||
	  (gGDMA_Monitor_ctl.enable==1)
	 ) {
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 1;
		osd_sr_scaleup_ctrl0_reg.ver_factor = 0xfffff;
		osd_sr_scaleup_ctrl0_reg.ver_tap_num = 0x3;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;
		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;	
		osd_sr_ctrl_reg.identity_en = 1;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;
	}else
	#endif//
	if( gdma->enable_identity[GDMA_PLANE_OSD5] )
	{ //sr 4,5 setting is different 
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 1;
		osd_sr_scaleup_ctrl0_reg.ver_factor = 0x000fffff;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;

		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;

		osd_sr_h_location_ctrl_reg.osd_width =  width_src;
		pReg->osd_sr_h_location_ctrl_reg.regValue = osd_sr_h_location_ctrl_reg.regValue;

		osd_sr_v_location_ctrl_reg.osd_height = height_src;
		pReg->osd_sr_v_location_ctrl_reg.regValue = osd_sr_v_location_ctrl_reg.regValue;

		
		osd_sr_ctrl_reg.identity_en = 1;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;
		//osd OSD_WIDTH/h need == input size 

	}
	else
	{
		osd_sr_ctrl_reg.identity_en = 0;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;
	}
 

	return GDMA_SUCCESS;
}


int GDMA_OSD5_DevCB_set_SR(uint8_t enable, GDMA_DISPLAY_PLANE disPlane, GDMA_PIC_DATA* curPic)
{
	#if 1
	GDMA_5_REG_CONTENT *pReg = NULL;

	pReg = &curPic->reg_5_content;


	if (enable) {
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg	, pReg->osd_sr_scaleup_ctrl0_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_Scaleup_Ctrl1_reg, pReg->osd_sr_scaleup_ctrl1_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_V_Location_Ctrl_reg, pReg->osd_sr_v_location_ctrl_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_H_Location_Ctrl_reg, pReg->osd_sr_h_location_ctrl_reg.regValue);

		/* enhance OSD sharpness by designer suggestion */
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_Ctrl_reg, pReg->osd_sr_ctrl_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_GainY_Ctrl0_reg, pReg->osd_sr_gainy_ctrl0_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_GainMap_Ctrl0_reg, pReg->osd_sr_gainmap_ctrl_reg.regValue);

		#if 0 //need check for osd 5
		if ((pReg->osd_sr_scaleup_ctrl1_reg.h_zoom_en != 0) || (pReg->osd_sr_scaleup_ctrl0_reg.v_zoom_en != 0))
			fwif_color_set_scaleosd_sharpness_driver(0, 1);
		else
			fwif_color_set_scaleosd_sharpness_driver(0, 0);
		#endif//

		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg, rtd_inl(OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg)) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_5_Scaleup_Ctrl1_reg, rtd_inl(OSD_SR45_OSD_SR_5_Scaleup_Ctrl1_reg)) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_V_Location_Ctrl_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_5_V_Location_Ctrl_reg, rtd_inl(OSD_SR45_OSD_SR_5_V_Location_Ctrl_reg)) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_H_Location_Ctrl_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_5_H_Location_Ctrl_reg, rtd_inl(OSD_SR45_OSD_SR_5_H_Location_Ctrl_reg)) ;
	} else {
	 
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg, pReg->osd_sr_scaleup_ctrl0_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_Scaleup_Ctrl1_reg, pReg->osd_sr_scaleup_ctrl1_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_V_Location_Ctrl_reg, pReg->osd_sr_v_location_ctrl_reg.regValue);
		OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_H_Location_Ctrl_reg, pReg->osd_sr_h_location_ctrl_reg.regValue);

		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg, rtd_inl(OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg)) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR45_OSD_SR_5_Scaleup_Ctrl1_reg, rtd_inl(OSD_SR45_OSD_SR_5_Scaleup_Ctrl1_reg)) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_V_Location_Ctrl_reg[0x%x]		 0x%x\n", OSD_SR45_OSD_SR_5_V_Location_Ctrl_reg, rtd_inl(OSD_SR45_OSD_SR_5_V_Location_Ctrl_reg)) ;
		DBGEXT_SR_PRINT(KERN_EMERG"OSD_SR_X_H_Location_Ctrl_reg[0x%x]		 0x%x\n", OSD_SR45_OSD_SR_5_H_Location_Ctrl_reg, rtd_inl(OSD_SR45_OSD_SR_5_H_Location_Ctrl_reg)) ;
	}
 #endif//

	return GDMA_SUCCESS;
}

#endif//RTK_ENABLE_GDMA_EXT

