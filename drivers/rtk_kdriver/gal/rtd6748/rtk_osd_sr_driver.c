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
#include <rtk_kdriver/io.h>
#include <gal/rtk_gdma_driver.h>
//for WBSKTASKWBS-1768 : OSD Scale up issue
#define USE_OSD_SR_ZERO_POS 1
extern gdma_dev *gdma_devices;
extern int gdma_nr_devs;
#include <gal/rtk_gdma_split.h>
extern int change_OSD_mode;
extern GDMA_MONITOR_CTL  gGDMA_Monitor_ctl;

/* for OSD shift function */
extern CONFIG_OSDSHIFT_STRUCT g_osdshift_ctrl;

/* ------------------------------------- */
/*           Super Resolution            */
/* ------------------------------------- */
void drv_superresolution_init(void)
{
#if defined(CONFIG_ARCH_RTK288O) || defined(CONFIG_ARCH_RTK2875) || defined(CONFIG_ARCH_RTK2885M) || defined(CONFIG_ARCH_RTK6748)
#else
	gdma_line_buffer_end_RBUS line_buffer_end_reg;
#endif

	/*
	osd_sr switch
	*/
#if defined(CONFIG_ARCH_RTK288O) || defined(CONFIG_ARCH_RTK2875) || defined(CONFIG_ARCH_RTK2885M) || defined(CONFIG_ARCH_RTK6748)
#else
	line_buffer_end_reg.regValue = 0;
	line_buffer_end_reg.osdsr1_switch_to = GDMA_PLANE_OSD1;
	line_buffer_end_reg.osdsr2_switch_to = GDMA_PLANE_OSD3;
	rtd_outl(GDMA_line_buffer_end_reg, line_buffer_end_reg.regValue);
#endif

	/*
	coefficients
	*/ /* Yvonne:Maybe need to modify */
	/* rtd_outl( OSD_SR_HCOEFF_VADDR, 0x00043804 ); */
	/* rtd_outl( OSD_SR_VCOEFF_VADDR , 0x00043804 ); */

	osd_sr_set_table_gal( 0 , osd_sr_get__gal_osdsr_table_txt() );
	osd_sr_set_table_gal( OSD_SR_OSD_SR_2_GainMap_Ctrl0_reg-OSD_SR_OSD_SR_1_GainMap_Ctrl0_reg , osd_sr_get__gal_osdsr_table_txt() );
}

void drv_superresolution_LPF_Gain(GDMA_DISPLAY_PLANE disPlane)
{
	uint32_t offset = 0;
#if defined(CONFIG_ARCH_RTK288O) || defined(CONFIG_ARCH_RTK2875) || defined(CONFIG_ARCH_RTK2885M) || defined(CONFIG_ARCH_RTK6748)
		if (disPlane == GDMA_PLANE_OSD1)
			offset = 0x0;
		else if (disPlane == GDMA_PLANE_OSD2)
			offset = 0x80;
#else
	gdma_line_buffer_end_RBUS line_buffer_end_reg;
	*(u32 *)&line_buffer_end_reg = 0;

	*(unsigned int *)&line_buffer_end_reg = rtd_inl(GDMA_line_buffer_end_reg);
	line_buffer_end_reg.regValue = rtd_inl(GDMA_line_buffer_end_reg);
	if (line_buffer_end_reg.osdsr1_switch_to == disPlane)
		offset = 0x0;
	else if (line_buffer_end_reg.osdsr2_switch_to == disPlane)
		offset = 0x80;
#endif
	osd_sr_set_table_gal( offset , osd_sr_get__gal_osdsr_table_txt() );
}

void drv_superresolution_LPF_Gain_Disable(GDMA_DISPLAY_PLANE disPlane)
{
	uint32_t offset = 0;
#if defined(CONFIG_ARCH_RTK288O) || defined(CONFIG_ARCH_RTK2875) || defined(CONFIG_ARCH_RTK2885M) || defined(CONFIG_ARCH_RTK6748)
		if (disPlane == GDMA_PLANE_OSD1)
			offset = 0x0;
		else if (disPlane == GDMA_PLANE_OSD2)
			offset = 0x80;
#else
	gdma_line_buffer_end_RBUS line_buffer_end_reg;
	*(u32 *)&line_buffer_end_reg = 0;

	*(unsigned int *)&line_buffer_end_reg = rtd_inl(GDMA_line_buffer_end_reg);
	line_buffer_end_reg.regValue = rtd_inl(GDMA_line_buffer_end_reg);
	if (line_buffer_end_reg.osdsr1_switch_to == disPlane)
		offset = 0x0;
	else if (line_buffer_end_reg.osdsr2_switch_to == disPlane)
		offset = 0x80;
#endif
	osd_sr_set_table_gal( offset , osd_sr_get__gal_osdsr_table_txt() );
}


void drv_superresolution_ch_switch(GDMA_DISPLAY_PLANE src, OSD_SR_CH_SEL dest)
{
#if defined(CONFIG_ARCH_RTK288O) || defined(CONFIG_ARCH_RTK2875) || defined(CONFIG_ARCH_RTK2885M) || defined(CONFIG_ARCH_RTK6748)
#else
	gdma_line_buffer_end_RBUS line_buffer_end_reg;

	line_buffer_end_reg.regValue = rtd_inl(GDMA_line_buffer_end_reg);
	
	if (dest == OSD1_SR)
		line_buffer_end_reg.osdsr1_switch_to = src;
	else if (dest == OSD2_SR)
		line_buffer_end_reg.osdsr2_switch_to = src;
	rtd_outl(GDMA_line_buffer_end_reg, line_buffer_end_reg.regValue);
#endif
}


/* ------------------------------------- */
/*           Super Resolution            */
/* ------------------------------------- */
void drv_scaleup_init(void)
{
	osd_sr_osd_sr_1_scaleup_ctrl0_RBUS osd_sr1_scaleup_ctrl0_reg;

	osd_sr1_scaleup_ctrl0_reg.regValue = rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg);
	osd_sr1_scaleup_ctrl0_reg.v_zoom_en = 0;
	osd_sr1_scaleup_ctrl0_reg.ver_ini = 0xff;
	osd_sr1_scaleup_ctrl0_reg.ver_factor = 0;

	rtd_outl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg, osd_sr1_scaleup_ctrl0_reg.regValue);
	rtd_outl(OSD_SR_OSD_SR_2_Scaleup_Ctrl0_reg, osd_sr1_scaleup_ctrl0_reg.regValue);
}

int drv_scaleup(uint8_t enable, GDMA_DISPLAY_PLANE disPlane)
{
	gdma_dev *gdma = &gdma_devices[0];
#if defined(CONFIG_ARCH_RTK288O) || defined(CONFIG_ARCH_RTK2875) || defined(CONFIG_ARCH_RTK2885M) || defined(CONFIG_ARCH_RTK6748)
#else
	gdma_line_buffer_end_RBUS line_buffer_end_reg;
#endif
	unsigned int offset = 0;
	GDMA_PIC_DATA *curPic;
	GDMA_REG_CONTENT *pReg = NULL;

	curPic = gdma->pic[disPlane] + gdma->curPic[disPlane];
	pReg = &curPic->reg_content;
#if defined(CONFIG_ARCH_RTK288O) || defined(CONFIG_ARCH_RTK2875) || defined(CONFIG_ARCH_RTK2885M) || defined(CONFIG_ARCH_RTK6748)
		if (disPlane == GDMA_PLANE_OSD1)
			offset = 0x0;
		else if (disPlane == GDMA_PLANE_OSD2)
			offset = 0x80;
		else if (disPlane == GDMA_PLANE_OSD3)
			offset = 0x100;
#else
	*(unsigned int *)&line_buffer_end_reg = rtd_inl(GDMA_line_buffer_end_reg);
	line_buffer_end_reg.regValue = rtd_inl(GDMA_line_buffer_end_reg);
	if (line_buffer_end_reg.osdsr1_switch_to == disPlane)
		offset = 0x0;
	else if (line_buffer_end_reg.osdsr2_switch_to == disPlane)
		offset = 0x80;
	else if (line_buffer_end_reg.osdsr3_switch_to == disPlane)
		offset = 0x100;
#endif
	else {
		DBG_SR_PRINT(KERN_EMERG"%s, No match with OSDSRx_switch_to to OSD%d !! \n", __FUNCTION__, disPlane) ;
		return GDMA_SUCCESS;
	}

	DBG_SR_PRINT(KERN_EMERG"%s, line_buffer_end_reg=0x%x\n", __FUNCTION__, line_buffer_end_reg.regValue) ;

	if (enable) {
		rtd_outl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, pReg->osd_sr_scaleup_ctrl0_reg.regValue);
		rtd_outl(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, pReg->osd_sr_scaleup_ctrl1_reg.regValue);
		rtd_outl(OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset, pReg->osd_sr_v_location_ctrl_reg.regValue);
		rtd_outl(OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset, pReg->osd_sr_h_location_ctrl_reg.regValue);

		/* enhance OSD sharpness by designer suggestion */
		rtd_outl(OSD_SR_OSD_SR_1_Ctrl_reg+offset, pReg->osd_sr_ctrl_reg.regValue);

		if ((pReg->osd_sr_scaleup_ctrl1_reg.h_zoom_en != 0) || (pReg->osd_sr_scaleup_ctrl0_reg.v_zoom_en != 0))
			fwif_color_set_scaleosd_sharpness_driver(0, 1);
		else
			fwif_color_set_scaleosd_sharpness_driver(0, 0);

		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset)) ;
		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset)) ;
		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_V_Location_Ctrl_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset)) ;
		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_H_Location_Ctrl_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset)) ;
	} else {
		/*  OSD_SR 1/2/3 disable */
		rtd_outl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, pReg->osd_sr_scaleup_ctrl0_reg.regValue);
		rtd_outl(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, pReg->osd_sr_scaleup_ctrl1_reg.regValue);
		rtd_outl(OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset, pReg->osd_sr_v_location_ctrl_reg.regValue);
		rtd_outl(OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset, pReg->osd_sr_h_location_ctrl_reg.regValue);

		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset)) ;
		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset)) ;
		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_V_Location_Ctrl_reg[0x%x]		 0x%x\n", OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset)) ;
		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_H_Location_Ctrl_reg[0x%x]		 0x%x\n", OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset, rtd_inl(OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset)) ;
	}


	return GDMA_SUCCESS;
}

int drv_PreScale(uint8_t enable, VO_RECTANGLE srcWin, VO_RECTANGLE dispWin,
		GDMA_DISPLAY_PLANE disPlane, struct gdma_receive_work *ptr_work)
{
	gdma_dev *gdma = &gdma_devices[0];
#if defined(CONFIG_ARCH_RTK288O) || defined(CONFIG_ARCH_RTK2875) || defined(CONFIG_ARCH_RTK2885M) || defined(CONFIG_ARCH_RTK6748)
#else
	gdma_line_buffer_end_RBUS line_buffer_end_reg;
#endif
	osd_sr_osd_sr_1_scaleup_ctrl0_RBUS osd_sr_scaleup_ctrl0_reg;
	osd_sr_osd_sr_1_scaleup_ctrl1_RBUS osd_sr_scaleup_ctrl1_reg;
	osd_sr_osd_sr_1_h_location_ctrl_RBUS osd_sr_h_location_ctrl_reg;
	osd_sr_osd_sr_1_v_location_ctrl_RBUS osd_sr_v_location_ctrl_reg;
	osd_sr_osd_sr_1_ctrl_RBUS osd_sr_ctrl_reg;
	unsigned int ver_factor = 0,width_src=0,width_disp=0;
	unsigned int hor_factor = 0,height_src=0,height_disp=0;
	unsigned int offset = 0, bypass_flag = 0;
	GDMA_PIC_DATA *curPic;
	GDMA_REG_CONTENT *pReg = NULL;
	unsigned int hor_factor_change = 0, ver_factor_change=0;

	curPic = gdma->pic[disPlane] + ptr_work->picQwr;
	pReg = &curPic->reg_content;
#if defined(CONFIG_ARCH_RTK288O) || defined(CONFIG_ARCH_RTK2875) || defined(CONFIG_ARCH_RTK2885M) || defined(CONFIG_ARCH_RTK6748)
	if (disPlane == GDMA_PLANE_OSD1)
		offset = 0x0;
	else if (disPlane == GDMA_PLANE_OSD2)
		offset = 0x80;
#else
	*(unsigned int *)&line_buffer_end_reg = rtd_inl(GDMA_line_buffer_end_reg);
	line_buffer_end_reg.regValue = rtd_inl(GDMA_line_buffer_end_reg);
	if (line_buffer_end_reg.osdsr1_switch_to == disPlane)
		offset = 0x0;
	else if (line_buffer_end_reg.osdsr2_switch_to == disPlane)
		offset = 0x80;
#endif
	DBG_SR_PRINT(KERN_EMERG"%s, line_buffer_end_reg=0x%x\n", __FUNCTION__, line_buffer_end_reg.regValue);


	if (enable && (bypass_flag == 0)) {
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 0;
#if USE_OSD_SR_ZERO_POS
		if (g_osdshift_ctrl.shift_v_enable && disPlane == (GDMA_DISPLAY_PLANE)g_osdshift_ctrl.plane)
		{
			if (g_osdshift_ctrl.v_shift_pixel > 0)	// shift down
			{
				osd_sr_scaleup_ctrl0_reg.ver_ini = 0xff;
			}else	// shift up
			{
				if ( g_osdshift_ctrl.para_v_K)
					osd_sr_scaleup_ctrl0_reg.ver_ini = 0x7f;
				else
			osd_sr_scaleup_ctrl0_reg.ver_ini = 0xff;
			}
		}else
			osd_sr_scaleup_ctrl0_reg.ver_ini = 0xff;

#else
		osd_sr_scaleup_ctrl0_reg.ver_ini = 0;
#endif
		osd_sr_scaleup_ctrl0_reg.ver_factor = 0;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;

		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;
#if USE_OSD_SR_ZERO_POS
		if (g_osdshift_ctrl.shift_h_enable && disPlane == (GDMA_DISPLAY_PLANE)g_osdshift_ctrl.plane)
		{
			if (g_osdshift_ctrl.h_shift_pixel > 0)	// shift right
			{
				osd_sr_scaleup_ctrl1_reg.hor_ini = 0xff;
			}else	// shift left
			{
				if ( g_osdshift_ctrl.para_h_K)
					osd_sr_scaleup_ctrl1_reg.hor_ini = 0x7f;
				else
					osd_sr_scaleup_ctrl1_reg.hor_ini = 0xff;
			}
		} else
		osd_sr_scaleup_ctrl1_reg.hor_ini = 0xff;
#else
		osd_sr_scaleup_ctrl1_reg.hor_ini = 0;
#endif
		osd_sr_scaleup_ctrl1_reg.hor_factor = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;


		if (g_osdshift_ctrl.shift_v_enable && disPlane == (GDMA_DISPLAY_PLANE)g_osdshift_ctrl.plane){
			osd_sr_v_location_ctrl_reg.osd_height = dispWin.height-abs(g_osdshift_ctrl.v_shift_pixel);
			dispWin.height -= abs(g_osdshift_ctrl.v_shift_pixel);
			if ((g_osdshift_ctrl.v_shift_pixel > 0)) {
				// shift down
				osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y+g_osdshift_ctrl.v_shift_pixel;
			} else {
		osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y;
			}
		} else if (g_osdshift_ctrl.shift_v_enable && disPlane == GDMA_PLANE_OSD3) {
		osd_sr_v_location_ctrl_reg.osd_height = dispWin.height;
			if ((g_osdshift_ctrl.v_shift_pixel > 0)) {
				// shift down
				//osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y+g_osdshift_ctrl.v_shift_pixel;
				osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y;
			} else {
				osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y;
			}
		}else{
			osd_sr_v_location_ctrl_reg.osd_height = dispWin.height;
			osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y;
		}
		pReg->osd_sr_v_location_ctrl_reg.regValue = osd_sr_v_location_ctrl_reg.regValue;



		if (g_osdshift_ctrl.shift_h_enable && disPlane == (GDMA_DISPLAY_PLANE)g_osdshift_ctrl.plane){
			osd_sr_h_location_ctrl_reg.osd_width = dispWin.width-abs(g_osdshift_ctrl.h_shift_pixel);
			dispWin.width -= abs(g_osdshift_ctrl.h_shift_pixel);

			if(g_osdshift_ctrl.h_shift_pixel > 0) {
				// shift right
				osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x+g_osdshift_ctrl.h_shift_pixel;
				//osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x;
			}else{
				osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x;
			}
		} else if (g_osdshift_ctrl.shift_h_enable && disPlane == GDMA_PLANE_OSD3) {
			osd_sr_h_location_ctrl_reg.osd_width = dispWin.width;
			if(g_osdshift_ctrl.h_shift_pixel > 0) {
				// shift right
				//osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x+g_osdshift_ctrl.h_shift_pixel;
				osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x;
			}else{
				osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x;
			}

		}else {
			osd_sr_h_location_ctrl_reg.osd_width = dispWin.width;
			osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x;
		}
		pReg->osd_sr_h_location_ctrl_reg.regValue = osd_sr_h_location_ctrl_reg.regValue;

		/*
		OSD_H_STA+OSD_WIDTH , OSD_V_STA+OSD_HEIGHT < video range
		input_width >=16
		*/
		if (srcWin.width == 0 || srcWin.height == 0 || dispWin.width == 0 || dispWin.height == 0) {
			pr_debug("GDMA division by :srcWin.width = %d,srcWin.height=%d\n", srcWin.width, srcWin.height) ;
			pr_debug("GDMA division by :dispWin.width = %d,dispWin.height=%d\n", dispWin.width, dispWin.height) ;
		}
		DBG_SR_PRINT("%s:srcWin.width = %d,srcWin.height=%d\n", __FUNCTION__, srcWin.width, srcWin.height) ;
		DBG_SR_PRINT("%s:dispWin.width = %d,dispWin.height=%d\n", __FUNCTION__, dispWin.width, dispWin.height) ;

		/* online mode : h_front_porch >= 16, v_back_porch >= 16 */
		if ((((dispWin.x+dispWin.width) > (gdma->dispWin[disPlane].width+1)) ||
			 ((dispWin.y+dispWin.height) > (gdma->dispWin[disPlane].height+1)))) {
			/* Under flow */

			pr_debug("%s:dispWin.x = %d,dispWin.y=%d\n", __FUNCTION__, dispWin.x, dispWin.y) ;
			pr_debug("%s:dispWin.width = %d,dispWin.height=%d\n", __FUNCTION__, dispWin.width, dispWin.height) ;
			pr_debug("%s:gdma->dispWin[%d].width = %d,gdma->dispWin[%d].height=%d\n", __FUNCTION__,
				   disPlane, gdma->dispWin[disPlane].width, disPlane, gdma->dispWin[disPlane].height) ;

			pr_debug("%s:data underflow\n", __FUNCTION__) ;
			return -1;
		//} else if ((srcWin.width != dispWin.width) && ((srcWin.width < 16) || (srcWin.height < 4))) {
		//	pr_debug("%s:srcWin.width = %d [>=16], srcWin.height = %d [>=4] \n", __FUNCTION__, srcWin.width, srcWin.height) ;
		//	return -1;
		} else {
		
			width_src = srcWin.width;
			height_src = srcWin.height;
			width_disp = dispWin.width;
			height_disp = dispWin.height;		
			
			if (dispWin.width != 0)
			hor_factor = (width_src<<20)/width_disp;
			if (dispWin.height != 0)
			ver_factor = (height_src<<20)/height_disp;
			
			//printk(KERN_EMERG"%s hor_factor=0x%x,ver_factor=0x%x\n", __FUNCTION__, hor_factor, ver_factor) ;
			DBG_SR_PRINT(KERN_EMERG"%s hor_factor=0x%x,ver_factor=0x%x\n", __FUNCTION__, hor_factor, ver_factor) ;

			if ((srcWin.height < dispWin.height) && (ver_factor)) { /*  no scaling down */
				osd_sr_scaleup_ctrl0_reg.v_zoom_en = 1;
#if USE_OSD_SR_ZERO_POS
				osd_sr_scaleup_ctrl0_reg.ver_ini = 0xff;
#else
				osd_sr_scaleup_ctrl0_reg.ver_ini = srcWin.y;
#endif
				osd_sr_scaleup_ctrl0_reg.ver_factor = ver_factor;

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
			if((OSD_SR_OSD_SR_1_Scaleup_Ctrl0_get_ver_factor(rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset)) !=	pReg->osd_sr_scaleup_ctrl0_reg.ver_factor) )
			{
				ver_factor_change = 1;
			}
			
			if((OSD_SR_OSD_SR_1_Scaleup_Ctrl1_get_hor_factor(rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset)) !=	pReg->osd_sr_scaleup_ctrl1_reg.hor_factor) )
			{
				hor_factor_change = 1;
			}
			if (bypass_flag == 0 && (ver_factor_change || hor_factor_change)) {
					/* disable LPF and set back to default value when scaling function is NOT enable */
				osd_sr_set_table_gal( offset , osd_sr_get__gal_osdsr_table_txt() );
			}

			/* enhance OSD sharpness by designer suggestion */
			if (osd_sr_scaleup_ctrl0_reg.v_zoom_en || osd_sr_scaleup_ctrl1_reg.h_zoom_en) {
				osd_sr_ctrl_reg.sr_maxmin_limit_en = 1;
				osd_sr_ctrl_reg.sr_maxmin_shift = 0x20;
				osd_sr_ctrl_reg.sr_var_thd = 0x20;
				pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;

			}
		}
		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, pReg->osd_sr_scaleup_ctrl0_reg.regValue) ;
		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, pReg->osd_sr_scaleup_ctrl1_reg.regValue) ;
		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_V_Location_Ctrl_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset, pReg->osd_sr_v_location_ctrl_reg.regValue) ;
		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_H_Location_Ctrl_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset, pReg->osd_sr_h_location_ctrl_reg.regValue) ;
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
		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl0_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, pReg->osd_sr_scaleup_ctrl0_reg.regValue) ;
		DBG_SR_PRINT(KERN_EMERG"OSD_SR_X_Scaleup_Ctrl1_reg[0x%x]        0x%x\n", OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, pReg->osd_sr_scaleup_ctrl1_reg.regValue) ;

		osd_sr_v_location_ctrl_reg.osd_v_sta = dispWin.y;
		osd_sr_v_location_ctrl_reg.osd_height = dispWin.height;
		pReg->osd_sr_v_location_ctrl_reg.regValue = osd_sr_v_location_ctrl_reg.regValue;

		osd_sr_h_location_ctrl_reg.osd_h_sta = dispWin.x;
		osd_sr_h_location_ctrl_reg.osd_width = dispWin.width;
		pReg->osd_sr_h_location_ctrl_reg.regValue = osd_sr_h_location_ctrl_reg.regValue;
	}
	if((change_OSD_mode == GDMA_SPLIT_MODE_1 ||
	  change_OSD_mode == GDMA_SPLIT_MODE_2 ||
	  change_OSD_mode == GDMA_SPLIT_MODE_3 ||
	  (gGDMA_Monitor_ctl.enable==1))
	  || (!osd_sr_scaleup_ctrl0_reg.v_zoom_en && !osd_sr_scaleup_ctrl1_reg.h_zoom_en)
		){
		osd_sr_scaleup_ctrl0_reg.v_zoom_en = 1;
		osd_sr_scaleup_ctrl0_reg.ver_factor = 0xfffff;
		osd_sr_scaleup_ctrl0_reg.ver_tap_num = 0x3;
		pReg->osd_sr_scaleup_ctrl0_reg.regValue = osd_sr_scaleup_ctrl0_reg.regValue;
		osd_sr_scaleup_ctrl1_reg.h_zoom_en = 0;
		pReg->osd_sr_scaleup_ctrl1_reg.regValue = osd_sr_scaleup_ctrl1_reg.regValue;	
		osd_sr_ctrl_reg.identity_en = 1;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;
		osd_sr_set_lpf_gain( offset , 0 );
	}else{
		osd_sr_ctrl_reg.identity_en = 0;
		pReg->osd_sr_ctrl_reg.regValue = osd_sr_ctrl_reg.regValue;
	}

	
	
	return GDMA_SUCCESS;
}


int osd_sr_init_module(void)
{

	drv_superresolution_init();
	drv_scaleup_init();

	return GDMA_SUCCESS;
}

void osd_sr_cleanup_module(void)
{

	drv_superresolution_init();
	drv_scaleup_init();

}


/** @brief configure OSD location by x,y coordinate and disable H/V_zoom for offline compress process
*   @param disPlane OSD1~5 display plane
*   @param x		x coordinate
*   @param y		y coordinate
*   @return GDMA_SUCCESS
*/
int osd_sr_configLocation(GDMA_DISPLAY_PLANE disPlane, int x, int y)
{
#if 0
	unsigned int offset = 0;
	line_buffer_end_RBUS line_buffer_end_reg;
	osd_sr_1_h_location_ctrl_RBUS osd_sr_h_location;
	osd_sr_1_v_location_ctrl_RBUS osd_sr_v_location;
	osd_sr_osd_bypass_4_location_ctrl_RBUS osd_sr_bypass_location;
	osd_sr_1_scaleup_ctrl0_RBUS osd_sr_scaleup_ctrl0_reg;
	osd_sr_1_scaleup_ctrl1_RBUS osd_sr_scaleup_ctrl1_reg;

	*(unsigned int *)&line_buffer_end_reg = rtd_inl(GDMA_line_buffer_end_reg);
	line_buffer_end_reg.regValue = rtd_inl(GDMA_line_buffer_end_reg);
	if (line_buffer_end_reg.osdsr1_switch_to == disPlane)
		offset = 0x0;
	else if (line_buffer_end_reg.osdsr2_switch_to == disPlane)
		offset = 0x80;

	/*  OSD_SR1/2/3 */
	osd_sr_scaleup_ctrl0_reg.regValue = 0;
	osd_sr_scaleup_ctrl1_reg.regValue = 0;
	osd_sr_h_location.regValue = rtd_inl(OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset);
	osd_sr_v_location.regValue = rtd_inl(OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset);
	osd_sr_h_location.osd_h_sta = x;
	osd_sr_v_location.osd_v_sta = y;
	rtd_outl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset, osd_sr_scaleup_ctrl0_reg.regValue);
	rtd_outl(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset, osd_sr_scaleup_ctrl1_reg.regValue);
	rtd_outl(OSD_SR_OSD_SR_1_V_Location_Ctrl_reg+offset, osd_sr_v_location.regValue);
	rtd_outl(OSD_SR_OSD_SR_1_H_Location_Ctrl_reg+offset, osd_sr_h_location.regValue);
#endif
	return GDMA_SUCCESS ;
}

/** @brief check OSD_SR channel HW on/off status
 *  @param OSD plane
 *  @return 1: OSD_SRx on; 0: OSD_SRx off
*/
int osd_sr_check_onoff_status(GDMA_DISPLAY_PLANE disPlane)
{
	unsigned int offset = 0, bypass_flag = 0;

#if defined(CONFIG_ARCH_RTK288O) || defined(CONFIG_ARCH_RTK2875) || defined(CONFIG_ARCH_RTK2885M) || defined(CONFIG_ARCH_RTK6748)
	if (disPlane == GDMA_PLANE_OSD1)
		offset = 0x0;
	else if (disPlane == GDMA_PLANE_OSD2)
		offset = 0x80;
#else
	gdma_line_buffer_end_RBUS line_buffer_end_reg;

	*(unsigned int *)&line_buffer_end_reg = rtd_inl(GDMA_line_buffer_end_reg);
	line_buffer_end_reg.regValue = rtd_inl(GDMA_line_buffer_end_reg);
	if (line_buffer_end_reg.osdsr1_switch_to == disPlane)
		offset = 0x0;
	else if (line_buffer_end_reg.osdsr2_switch_to == disPlane)
		offset = 0x80;
#endif	
	if (bypass_flag)
		return 0;

	if (OSD_SR_OSD_SR_1_Scaleup_Ctrl0_get_v_zoom_en(rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg+offset)))
		return 1;

	if (OSD_SR_OSD_SR_1_Scaleup_Ctrl1_get_h_zoom_en(rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg+offset)))
		return 1;

	return 0;
}

enum
{
	eOSDSR_LEVEL_3x5_3x3,
	eOSDSR_LEVEL_3x5_txt,
	eOSDSR_LEVEL_3x5_3x5,
	eOSDSR_LEVEL_3x5__count
};

DRV_OSDSR_3x5 gal_osdsr_3x5[eOSDSR_LEVEL_3x5__count]={
	{ 0 , 0 , 0 },
	{ 1 , 1 , 1 },
	{ 1 , 1 , 1 },
};

void osd_sr_set_3x5( unsigned int offset , DRV_OSDSR_3x5* x )
{
	osd_sr_osd_sr_1_filter_ctrl0_RBUS reg_osd_sr_osd_sr_1_filter_ctrl0_reg;
	
	if( !x )
	{
		return;
	}
	
	reg_osd_sr_osd_sr_1_filter_ctrl0_reg.regValue = rtd_inl( OSD_SR_OSD_SR_1_Filter_Ctrl0_reg+offset );
	
	reg_osd_sr_osd_sr_1_filter_ctrl0_reg.sr_lpf_range = x->sr_lpf_range;
	reg_osd_sr_osd_sr_1_filter_ctrl0_reg.sr_edge_range = x->sr_edge_range;
	
	rtd_outl( OSD_SR_OSD_SR_1_Filter_Ctrl0_reg+offset , reg_osd_sr_osd_sr_1_filter_ctrl0_reg.regValue );
}

enum
{
	eOSDSR_LEVEL_overshoot_oil,
	eOSDSR_LEVEL_overshoot_txt,
	eOSDSR_LEVEL_overshoot_spec,
	eOSDSR_LEVEL_overshoot__count
};

DRV_OSDSR_overshoot gal_osdsr_overshoot[eOSDSR_LEVEL_overshoot__count]={
	{ 1 , 0 , 255 },
	{ 1 , 0x20 , 0x20 },
	{ 1 , 0x20 , 0x20 },
};

void osd_sr_set_overshoot( unsigned int offset , DRV_OSDSR_overshoot* x )
{
	osd_sr_osd_sr_1_ctrl_RBUS osd_sr_ctrl_reg;
	
	if( !x )
	{
		return;
	}
	
	osd_sr_ctrl_reg.regValue = rtd_inl( OSD_SR_OSD_SR_1_Ctrl_reg+offset );
	osd_sr_ctrl_reg.sr_maxmin_limit_en = x->sr_maxmin_limit_en;
	osd_sr_ctrl_reg.sr_maxmin_shift = x->sr_maxmin_shift;
	osd_sr_ctrl_reg.sr_var_thd = x->sr_var_thd;
	
	rtd_outl( OSD_SR_OSD_SR_1_Ctrl_reg+offset , osd_sr_ctrl_reg.regValue );
}

enum
{
	eOSDSR_LEVEL_lpf_range_off,
	eOSDSR_LEVEL_lpf_range_txt,
	eOSDSR_LEVEL_lpf_range_spec,
	eOSDSR_LEVEL_lpf_range_skin_low,
	eOSDSR_LEVEL_lpf_range_skin_mid,
	eOSDSR_LEVEL_lpf_range_skin_high,
	eOSDSR_LEVEL_lpf_range_paint_low,
	eOSDSR_LEVEL_lpf_range_paint_mid,
	eOSDSR_LEVEL_lpf_range_paint_high,
	eOSDSR_LEVEL_lpf_range__count
};

DRV_OSDSR_lpf_range gal_osdsr_lpf_range[eOSDSR_LEVEL_lpf_range__count]={
	{ 0 , 8 , 0 , 1 },
	{ 1 , 64 , 8 , 2 },
	{ 1 , 8 , 0 , 1 },
	{ 1 , 64 , 0 , 2 },
	{ 1 , 32 , 0 , 2 },
	{ 1 , 0 , 0 , 2 },
	{ 1 , 96 , 8 , 2 },
	{ 1 , 64 , 8 , 2 },
	{ 1 , 32 , 8 , 2 },
};

void osd_sr_set_lpf_range( unsigned int offset , DRV_OSDSR_lpf_range* x )
{
	osd_sr_osd_sr_1_gainmap_ctrl0_RBUS osd_sr_gainmap_ctrl_reg;
	
	if( !x )
	{
		return;
	}
	
	osd_sr_gainmap_ctrl_reg.regValue = rtd_inl( OSD_SR_OSD_SR_1_GainMap_Ctrl0_reg+offset );
	osd_sr_gainmap_ctrl_reg.sr_lpf_range_en = x->sr_lpf_range_en;
	osd_sr_gainmap_ctrl_reg.sr_lpf_range_thd = x->sr_lpf_range_thd;
	osd_sr_gainmap_ctrl_reg.sr_lpf_range_gain = x->sr_lpf_range_gain;
	osd_sr_gainmap_ctrl_reg.sr_lpf_range_step = x->sr_lpf_range_step;
	
	rtd_outl( OSD_SR_OSD_SR_1_GainMap_Ctrl0_reg+offset , osd_sr_gainmap_ctrl_reg.regValue );
}

enum
{
	eOSDSR_LEVEL_lpf_edge_off,
	eOSDSR_LEVEL_lpf_edge_txt,
	eOSDSR_LEVEL_lpf_edge_spec,
	eOSDSR_LEVEL_lpf_edge__count
};

DRV_OSDSR_lpf_edge gal_osdsr_lpf_edge[eOSDSR_LEVEL_lpf_edge__count]={
	{ 0 , 0 , 0 , 0 , 0 },
	{ 0 , 0 , 0 , 0 , 1 },
	{ 1 , 0 , 0 , 0 , 0 },
};

void osd_sr_set_lpf_edge( unsigned int offset , DRV_OSDSR_lpf_edge* x )
{
	osd_sr_osd_sr_1_gainmap_ctrl0_RBUS osd_sr_gainmap_ctrl_reg;
	osd_sr_osd_sr_1_filter_ctrl0_RBUS osd_sr_1_filter_ctrl0_reg;
	
	if( !x )
	{
		return;
	}
	
	osd_sr_gainmap_ctrl_reg.regValue = rtd_inl( OSD_SR_OSD_SR_1_GainMap_Ctrl0_reg+offset );
	osd_sr_1_filter_ctrl0_reg.regValue = rtd_inl( OSD_SR_OSD_SR_1_Filter_Ctrl0_reg+offset );
	
	osd_sr_gainmap_ctrl_reg.sr_lpf_edge_en = x->sr_lpf_edge_en;
	osd_sr_gainmap_ctrl_reg.sr_lpf_edge_thd = x->sr_lpf_edge_thd;
	osd_sr_gainmap_ctrl_reg.sr_lpf_edge_gain = x->sr_lpf_edge_gain;
	osd_sr_gainmap_ctrl_reg.sr_lpf_edge_step = x->sr_lpf_edge_step;
	osd_sr_1_filter_ctrl0_reg.sr_delta_gain = x->sr_delta_gain;
	
	rtd_outl( OSD_SR_OSD_SR_1_GainMap_Ctrl0_reg+offset , osd_sr_gainmap_ctrl_reg.regValue );
	rtd_outl( OSD_SR_OSD_SR_1_Filter_Ctrl0_reg+offset , osd_sr_1_filter_ctrl0_reg.regValue );
}

enum
{
	eOSDSR_LEVEL_lpmm_off,
	eOSDSR_LEVEL_lpmm_txt,
	eOSDSR_LEVEL_lpmm_circle,
	eOSDSR_LEVEL_lpmm_mid,
	eOSDSR_LEVEL_lpmm_bone,
	eOSDSR_LEVEL_lpmm__count
};

DRV_OSDSR_lpmm gal_osdsr_lpmm[eOSDSR_LEVEL_lpmm__count]={
	{ 0 , 6 , 12 },
	{ 1 , 8 , 12 },
	{ 1 , 6 , 12 },
	{ 1 , 8 , 12 },
	{ 1 , 8 , 15 },
};

void osd_sr_set_lpmm( unsigned int offset , DRV_OSDSR_lpmm* x )
{
	// not implemented yet
}

DRV_OSDSR_table gal_osdsr_table_txt={
	63	,// short sr_lpf_gain;
	eOSDSR_LEVEL_3x5_txt	,// short v_3x5;
	eOSDSR_LEVEL_overshoot_txt	,// short v_overshoot;
	eOSDSR_LEVEL_lpf_range_txt	,// short v_lpf_range;
	eOSDSR_LEVEL_lpf_edge_txt	,// short v_lpf_edge;
	eOSDSR_LEVEL_lpmm_txt	,// short v_lpmm;
};
// avoid variable not found error , so use function
DRV_OSDSR_table* osd_sr_get__gal_osdsr_table_txt(void)
{
	return &gal_osdsr_table_txt;
}

void osd_sr_set_lpf_gain( unsigned int offset , short sr_lpf_gain )
{
	osd_sr_osd_sr_1_gainy_ctrl0_RBUS osd_sr_gainy_ctrl0_reg;
	osd_sr_gainy_ctrl0_reg.regValue = rtd_inl(OSD_SR_OSD_SR_1_GainY_Ctrl0_reg+offset);
	osd_sr_gainy_ctrl0_reg.sr_lpf_gain = sr_lpf_gain;
	rtd_outl( OSD_SR_OSD_SR_1_GainY_Ctrl0_reg+offset , osd_sr_gainy_ctrl0_reg.regValue );
}

void osd_sr_set_table_gal( unsigned int offset , DRV_OSDSR_table* x )
{
	short z=0;
	
	if( !x )
	{
		return;
	}
	
	osd_sr_set_lpf_gain( offset , x -> sr_lpf_gain );
	
	z = x -> v_3x5;
	if( 0<=z && z<eOSDSR_LEVEL_3x5__count )
	{
		osd_sr_set_3x5( offset , gal_osdsr_3x5+z );
	}
	
	z = x -> v_overshoot;
	if( 0<=z && z<eOSDSR_LEVEL_overshoot__count )
	{
		osd_sr_set_overshoot( offset , gal_osdsr_overshoot+z );
	}
	
	z = x -> v_lpf_range;
	if( 0<=z && z<eOSDSR_LEVEL_lpf_range__count )
	{
		osd_sr_set_lpf_range( offset , gal_osdsr_lpf_range+z );
	}
	
	z = x -> v_lpf_edge;
	if( 0<=z && z<eOSDSR_LEVEL_lpf_edge__count )
	{
		osd_sr_set_lpf_edge( offset , gal_osdsr_lpf_edge+z );
	}
	
	z = x -> v_lpmm;
	if( 0<=z && z<eOSDSR_LEVEL_lpmm__count )
	{
		osd_sr_set_lpmm( offset , gal_osdsr_lpmm+z );
	}
}

void osd_sr_update_txt( short* x )
{
	int i=0;
	if( !x )
	{
		return;
	}
	gal_osdsr_table_txt.sr_lpf_gain = x[i];i++;
	gal_osdsr_3x5[eOSDSR_LEVEL_3x5_txt].sr_maxmin_range = x[i];i++;
	gal_osdsr_3x5[eOSDSR_LEVEL_3x5_txt].sr_lpf_range = x[i];i++;
	gal_osdsr_3x5[eOSDSR_LEVEL_3x5_txt].sr_edge_range = x[i];i++;
	gal_osdsr_overshoot[eOSDSR_LEVEL_overshoot_txt].sr_maxmin_limit_en = x[i];i++;
	gal_osdsr_overshoot[eOSDSR_LEVEL_overshoot_txt].sr_maxmin_shift = x[i];i++;
	gal_osdsr_overshoot[eOSDSR_LEVEL_overshoot_txt].sr_var_thd = x[i];i++;
	gal_osdsr_lpf_range[eOSDSR_LEVEL_lpf_range_txt].sr_lpf_range_en = x[i];i++;
	gal_osdsr_lpf_range[eOSDSR_LEVEL_lpf_range_txt].sr_lpf_range_thd = x[i];i++;
	gal_osdsr_lpf_range[eOSDSR_LEVEL_lpf_range_txt].sr_lpf_range_gain = x[i];i++;
	gal_osdsr_lpf_range[eOSDSR_LEVEL_lpf_range_txt].sr_lpf_range_step = x[i];i++;
	gal_osdsr_lpf_edge[eOSDSR_LEVEL_lpf_edge_txt].sr_lpf_edge_en = x[i];i++;
	gal_osdsr_lpf_edge[eOSDSR_LEVEL_lpf_edge_txt].sr_lpf_edge_thd = x[i];i++;
	gal_osdsr_lpf_edge[eOSDSR_LEVEL_lpf_edge_txt].sr_lpf_edge_gain = x[i];i++;
	gal_osdsr_lpf_edge[eOSDSR_LEVEL_lpf_edge_txt].sr_lpf_edge_step = x[i];i++;
	gal_osdsr_lpf_edge[eOSDSR_LEVEL_lpf_edge_txt].sr_delta_gain = x[i];i++;
	// gal_osdsr_lpmm[eOSDSR_LEVEL_lpmm_txt].sr_lpmm_en = x[i];i++;
	// gal_osdsr_lpmm[eOSDSR_LEVEL_lpmm_txt].sr_lpmm_1px = x[i];i++;
	// gal_osdsr_lpmm[eOSDSR_LEVEL_lpmm_txt].sr_lpmm_corner = x[i];i++;
}

EXPORT_SYMBOL(osd_sr_cleanup_module);
EXPORT_SYMBOL(drv_superresolution_LPF_Gain);
EXPORT_SYMBOL(osd_sr_init_module);
EXPORT_SYMBOL(drv_scaleup);
EXPORT_SYMBOL(drv_superresolution_ch_switch);
EXPORT_SYMBOL(osd_sr_configLocation);
EXPORT_SYMBOL(osd_sr_check_onoff_status);
