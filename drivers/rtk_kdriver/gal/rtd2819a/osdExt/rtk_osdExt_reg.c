#include <rbus/ppoverlay_reg.h>   // dtg
#include <rbus/gdma4_reg.h>
#include <rbus/osdovl4_reg.h>

#include <rbus/gdma5_reg.h>
#include <rbus/osdovl5_reg.h>
#include <rbus/osdtg45_reg.h>

#include <rtk_crt.h>
#include <gal/rtk_gdma_driver.h>
#include "rtk_osdExt_main.h" 
#include "rtk_osdExt_driver.h"
#include "rtk_osdExt_debug.h"
#include "rtk_osdExt_sr.h"


#ifdef RTK_GDMA_TEST_OSD1
extern void GDMA_Clk_Select(int src_type, bool enable); // osd1
extern void gdma_config_line_buffer(void);
extern void gdma_config_fbc(void);
extern void gdma_config_mid_blend(void); // mixer blend flactor

extern int drv_scaleup(uint8_t enable, GDMA_DISPLAY_PLANE disPlane); // FIXME_TODO remove, use local function
#endif//

//if osd need enable DTG ( otherwise controled by scaler )
int gGDMAExt_enable_DTG_Set = 1;

extern gdma_dev *gdma_devices;
extern unsigned long gdma_total_ddr_size;


GDMAExt_device_cb g_osd_cb[GDMA_PLANE_ALL_MAXNUM];



#ifdef RTK_GDMA_TEST_OSD1

//from probe()
int GDMA_OSD1_DevCB_init(GDMA_DISPLAY_PLANE plane, void *info)
{
    gdma_dev* gdma = (gdma_dev*) info ; 
    gdma_dma_RBUS  gdma_dma_reg;
	
	osdovl_mixer_ctrl2_RBUS mixer_ctrl2;
	osdovl_mixer_layer_sel_RBUS mixer_layer_sel_reg;
	//sys_reg_sys_dispclksel_RBUS sys_dispclksel_reg;

	unsigned int onlineProgDone = GDMA_CTRL_write_data(1);

	//GDMA_Clk_Select(0, true);

	gdma_dma_reg.regValue = OSD_RTD_INL(GDMA_DMA_reg);
	#ifdef RTK_GDMA_TEST_6748_OSD1
		gdma_dma_reg.tfbc1_req_num = 0xF;
		gdma_dma_reg.tfbc2_req_num = 0xF;
	#else
		gdma_dma_reg.afbc1_req_num = 0xF;
		gdma_dma_reg.afbc2_req_num = 0xF;
	#endif//

	OSD_RTD_OUTL(GDMA_DMA_reg, gdma_dma_reg.regValue);



	gdma->GDMA_osd.wi_endian = 0; 
 
	gdma->GDMA_osd.osd = 1;
	gdma->GDMA_osd.write_data = 1;
	


	OSD_RTD_OUTL(GDMA_OSD1_reg, *((u32 *)&gdma->GDMA_osd));
	OSD_RTD_OUTL(GDMA_OSD2_reg, *((u32 *)&gdma->GDMA_osd));
	OSD_RTD_OUTL(GDMA_OSD3_reg, *((u32 *)&gdma->GDMA_osd));

 	//OSD_RTD_OUTL(GDMA_OSD1_CTRL_reg, ~1);
	gdma_config_line_buffer();

	/* OSD programming done */
	onlineProgDone |= GDMA_CTRL_osd1_prog_done(1);
	onlineProgDone |= GDMA_CTRL_osd2_prog_done(1);
	onlineProgDone |= GDMA_CTRL_osd3_prog_done(1);
	OSD_RTD_OUTL(GDMA_CTRL_reg, onlineProgDone);

#if 1

#if 0
	/* wait prog done */
	while ((rtd_inl(GDMA_CTRL_reg)&prog_done_mask) != 0) {
		gdma_usleep(1000);
		to_cnt++;
		if (to_cnt > 40) {
			rtd_pr_gdma_debug("%s, GDMA HW something wrong. Please check \n", __FUNCTION__);
			break;
		}
	}
	rtd_pr_gdma_debug("%s, 1st passed \n", __FUNCTION__);
#endif

	/*
	*	initialization flow
	*	1. line buffer setting
	*	2. OSD_SR switch
	*	3. go_middle_blend could be set together
	*	4. then write all online path programming done
	*/
	gdma_config_fbc();

	mixer_ctrl2.regValue = OSD_RTD_INL(OSDOVL_Mixer_CTRL2_reg);
	
#if OSD_DETECTION_SUPPORT
	mixer_ctrl2.measure_osd_zone_en = 1;
	mixer_ctrl2.measure_osd_zone_type = 0; //A=0, R=0, G=0, B=0
#endif

	mixer_ctrl2.video_src_sel = 1;
	mixer_ctrl2.mixero1_en    = 1;

	mixer_ctrl2.mixero2_en    = 1;
	mixer_ctrl2.mixero3_en = 1;
	mixer_ctrl2.mixer_en = 1;

   // if( mixer_init == 0 )
    {
//      mixer_ctrl2.mixero1_en = 0;
//      mixer_ctrl2.mixero2_en = 0;
        mixer_ctrl2.mixero3_en = 0;
//      mixer_ctrl2.mixer_en   = 0;
    }
	OSD_RTD_OUTL(OSDOVL_Mixer_CTRL2_reg, mixer_ctrl2.regValue);

	/* wdma_bl set to 'd80 by HW RD suggestion */
	/*dma_bl_2_reg.regValue = rtd_inl(GDMA_DMA_BL_2_reg);
		dma_bl_2_reg.wdma_bl = 80;
		OSD_RTD_OUTL(GDMA_DMA_BL_2_reg, dma_bl_2_reg.regValue);
	*/

	/* mixer layer sel: video keep at c5 place */
	mixer_layer_sel_reg.regValue = OSD_RTD_INL(OSDOVL_Mixer_layer_sel_reg);
	mixer_layer_sel_reg.c0_sel = GDMA_PLANE_OSD1;
	mixer_layer_sel_reg.c1_sel = GDMA_PLANE_OSD2;
	mixer_layer_sel_reg.c2_sel = GDMA_PLANE_OSD3;
	mixer_layer_sel_reg.c3_sel = 0;
	OSD_RTD_OUTL(OSDOVL_Mixer_layer_sel_reg, mixer_layer_sel_reg.regValue);

	gdma_config_mid_blend();

	onlineProgDone |= GDMA_CTRL_osd1_prog_done(1);
	onlineProgDone |= GDMA_CTRL_osd2_prog_done(1);
	onlineProgDone |= GDMA_CTRL_osd3_prog_done(1);

	/* OSD programming done */
	OSD_RTD_OUTL(GDMA_CTRL_reg, onlineProgDone);



		/* gdma INT rounting to SCPU */
	OSD_RTD_OUTL(SYS_REG_INT_CTRL_SCPU_reg, SYS_REG_INT_CTRL_SCPU_osd_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_write_data(1));

	/* enable GDMA interrupt */
	OSD_RTD_OUTL(GDMA_OSD_INTST_reg, ~1); /*  clear status */


#ifdef TRIPLE_BUFFER_SEMAPHORE
	OSD_RTD_OUTL(GDMA_OSD_INTEN_reg, GDMA_OSD_INTEN_write_data(1) | GDMA_OSD_INTEN_osd1_vact_end(1) | GDMA_OSD_INTEN_osd1_vsync(1));
#else
	OSD_RTD_OUTL(GDMA_OSD_INTEN_reg, GDMA_OSD_INTEN_write_data(1) | GDMA_OSD_INTEN_osd1_vact_end(1));
#endif

	{
		gdma_total_ddr_size = 0x60000000;
		gdma_total_ddr_size = get_memory_size_total() - 0x100;

		GDMA_EXT_LOG(GDMA_EXT_LOG_DEBUG, "[GDMA] get total memsize = %x\n", gdma_total_ddr_size);

		if (gdma_total_ddr_size) {
			OSD_RTD_OUTL(GDMA_TFBC1_MIN_reg, 0x0);
			OSD_RTD_OUTL(GDMA_TFBC1_MAX_reg, gdma_total_ddr_size);
			OSD_RTD_OUTL(GDMA_OSD1_MIN_reg, 0x0);
			OSD_RTD_OUTL(GDMA_OSD1_MAX_reg, gdma_total_ddr_size-0x1000);
		} else {
			GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s gdma_total_ddr_size is empty!\n", __func__);
		}
	}


 #endif//

	// request_irq   FIXME_TODO: move to osdExt_linux.c

    return 0;
}


//int GDMAExt_PreOSDReg_Set_GDMA ( GDMA_PIC_DATA *curPic, struct gdma_receive_work *ptr_work)
int GDMA_OSD1_DevCB_preset_GDMA ( GDMA_PIC_DATA *curPic, struct gdma_receive_work *ptr_work)
{
	GDMA_WIN *win = NULL ;
	gdma_dev *gdma = &gdma_devices[0];

	GDMA_DISPLAY_PLANE disPlane = ptr_work->disPlane;
	GDMA_REG_CONTENT *pReg = &curPic->reg_content;

	int rotateBit = gdma->ctrl.enableVFlip;
	int canvasW, canvasH;

	
	win = &curPic->OSDwin;

	canvasW = win->winWH.width;
	canvasH = win->winWH.height;

		/*  set clear region */
		if (curPic->clear_x.value != 0 || curPic->clear_y.value != 0) {
			pReg->osd_clear1_reg.regValue = curPic->clear_x.value;
			pReg->osd_clear2_reg.regValue = curPic->clear_y.value;
			pReg->osd_reg.regValue = GDMA_OSD1_clear_region_en(1) | GDMA_OSD1_write_data(1);
		} else {
			pReg->osd_reg.regValue = GDMA_OSD1_clear_region_en(1) | GDMA_OSD1_write_data(0);
		}

		if (gdma->ctrl.osdfirstEn[disPlane] == 0) {

			pReg->osd_ctrl_reg.regValue = GDMA_OSD1_CTRL_write_data(1) | GDMA_OSD1_CTRL_osd1_en(curPic->show) | GDMA_OSD1_CTRL_rotate(rotateBit);

			gdma->ctrl.osdEn[disPlane] = 1;
			gdma->ctrl.osdfirstEn[disPlane] = 1;
			DBG_PRINT(KERN_EMERG"%s, win=0x%lx, phy=0x%x\n", __FUNCTION__, win, virt_to_phys(win));
		} else {
			/*  if osdEn changes to 0, driver will force curPic->show to zero and disable OSD */
			if (gdma->ctrl.osdEn[disPlane] == 0)
				curPic->show = 0;

			pReg->osd_ctrl_reg.regValue = GDMA_OSD1_CTRL_write_data(1) | GDMA_OSD1_CTRL_osd1_en(curPic->show) | GDMA_OSD1_CTRL_rotate(rotateBit);


		}

		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, " curPic->show=%d, gdma->ctrl.osdEn[%d]=%d \n",  curPic->show, disPlane, gdma->ctrl.osdEn[disPlane]);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," osd%d  win=0x%lx, top=0x%x gdma->f_inputsrc_4k2k=%d \n",  disPlane, win, win->top_addr, gdma->f_inputsrc_4k2k);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," input src[x,y,w,h]= [%d,%d,%d,%d] \n",  win->winXY.x, win->winXY.y, canvasW, canvasH);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," output[x,y,w,h]= [%d,%d,%d,%d] \n",  win->dst_x, win->dst_y, win->dst_width ,win->dst_height);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," plane_alpha_r=0x%x, plane_alpha_a=0x%x \n",  curPic->plane_ar.plane_alpha_r, curPic->plane_ar.plane_alpha_a);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," plane_alpha_g=0x%x, plane_alpha_b=0x%x \n",  curPic->plane_gb.plane_alpha_g, curPic->plane_gb.plane_alpha_b);

		pReg->osd_wi_reg.regValue = GDMA_OSD1_WI_addr (virt_to_phys(win));
		
        if (pReg->osd_wi_reg.regValue == 0) {
            #if defined(CONFIG_ARM64)
                GDMA_ERR( "[GDMA] addr = 0!!!!!!!! win = 0x%lx gIdxQwr = %d pitch = 0x%x\n", (unsigned long)win, ptr_work->picQwr, win->pitch);
            #else
                GDMA_ERR( "[GDMA] addr = 0!!!!!!!! win = 0x%x gIdxQwr = %d pitch = 0x%x\n", (unsigned int)win, ptr_work->picQwr, win->pitch);
            #endif
        } else if (pReg->osd_wi_reg.regValue > gdma_total_ddr_size) {
        
            #if defined(CONFIG_ARM64)
                GDMA_ERR( "[GDMA] addr = 0x%lx, win info addr over range!!\n", (unsigned long)win);
            #else
                GDMA_ERR( "[GDMA] addr = 0x%x, win info addr over range!!\n", (unsigned int)win);
            #endif
        }
		//pReg->osd_size_reg.regValue = GDMA_OSD1_SIZE_w(canvasW) | GDMA_OSD1_SIZE_h(canvasH);
		pReg->osd_size_reg.regValue = GDMA_OSD1_SIZE_w(canvasW) | GDMA_OSD1_SIZE_h(canvasH);

	return 0;
}

// GDMAExt_PreOSDReg_Set_Mixer ( GDMA_PIC_DATA *curPic)
int GDMA_OSD1_DevCB_preset_Mixer( GDMA_PIC_DATA *curPic )
{
	GDMA_REG_CONTENT *pReg = &curPic->reg_content;
	// GDMA_DISPLAY_PLANE disPlane = ptr_work->disPlane;

	//OSDExt add
	GDMA_DISPLAY_PLANE disPlane = curPic->plane;

/*  TODO: Mixer : online blend order & plane alpha */
		if (curPic->onlineOrder == C0) {
			pReg->mixer_layer_sel_reg.c0_sel = disPlane;

			if ((curPic->plane_ar.plane_alpha_r != 0xff) || (curPic->plane_ar.plane_alpha_a != 0xff))
				pReg->mixer_layer_sel_reg.c0_plane_alpha_en = 1;

			if ((curPic->plane_gb.plane_alpha_g != 0xff) || (curPic->plane_gb.plane_alpha_b != 0xff))
				pReg->mixer_layer_sel_reg.c0_plane_alpha_en = 1;

			pReg->mixer_c0_plane_alpha1_reg.regValue = curPic->plane_ar.value;
			pReg->mixer_c0_plane_alpha2_reg.regValue = curPic->plane_gb.value;
		} else if (curPic->onlineOrder == C1) {
			pReg->mixer_layer_sel_reg.c1_sel = disPlane;

			if ((curPic->plane_ar.plane_alpha_r != 0xff) || (curPic->plane_ar.plane_alpha_a != 0xff))
				pReg->mixer_layer_sel_reg.c1_plane_alpha_en = 1;

			if ((curPic->plane_gb.plane_alpha_g != 0xff) || (curPic->plane_gb.plane_alpha_b != 0xff))
				pReg->mixer_layer_sel_reg.c1_plane_alpha_en = 1;

			pReg->mixer_c1_plane_alpha1_reg.regValue = curPic->plane_ar.value;
			pReg->mixer_c1_plane_alpha2_reg.regValue = curPic->plane_gb.value;
		} else if (curPic->onlineOrder == C2) {
			pReg->mixer_layer_sel_reg.c2_sel = disPlane;

			if ((curPic->plane_ar.plane_alpha_r != 0xff) || (curPic->plane_ar.plane_alpha_a != 0xff))
				pReg->mixer_layer_sel_reg.c2_plane_alpha_en = 1;

			if ((curPic->plane_gb.plane_alpha_g != 0xff) || (curPic->plane_gb.plane_alpha_b != 0xff))
				pReg->mixer_layer_sel_reg.c2_plane_alpha_en = 1;

			pReg->mixer_c2_plane_alpha1_reg.regValue = curPic->plane_ar.value;
			pReg->mixer_c2_plane_alpha2_reg.regValue = curPic->plane_gb.value;
		} else if (curPic->onlineOrder == C3) {
			pReg->mixer_layer_sel_reg.c3_sel = disPlane;

			if ((curPic->plane_ar.plane_alpha_r != 0xff) || (curPic->plane_ar.plane_alpha_a != 0xff))
				pReg->mixer_layer_sel_reg.c3_plane_alpha_en = 1;

			if ((curPic->plane_gb.plane_alpha_g != 0xff) || (curPic->plane_gb.plane_alpha_b != 0xff))
				pReg->mixer_layer_sel_reg.c3_plane_alpha_en = 1;

			pReg->mixer_c3_plane_alpha1_reg.regValue = curPic->plane_ar.value;
			pReg->mixer_c3_plane_alpha2_reg.regValue = curPic->plane_gb.value;
		}

	return 0;
}

int GDMA_OSD1_DevCB_set_PresetOSD(GDMA_DISPLAY_PLANE plane, GDMA_PIC_DATA *curPic, VO_RECTANGLE* srcWin, VO_RECTANGLE* dispWin,
                            struct gdma_receive_work *ptr_work)
{
	int err = 0;

	err = GDMA_OSD1_DevCB_preset_GDMA(curPic, ptr_work);
	
	err = GDMA_OSD1_DevCB_preset_SR( 1, *srcWin, *dispWin, curPic);


	err = GDMA_OSD1_DevCB_preset_Mixer( curPic );
	

	return err;
}


int GDMA_OSD1_DevCB_SetGDMA(GDMA_DISPLAY_PLANE disPlane, void *info)
{
	gdma_dev* gdma = (gdma_dev*) info ;
	
	GDMA_PIC_DATA *curPic;
	unsigned int offset = 0;
	osdovl_mixer_layer_sel_RBUS mixer_layer_sel_reg;


	GDMA_REG_CONTENT *pReg = NULL;
	unsigned int cx_sel_old = 0, cx_sel_new = 0;

	curPic = gdma->pic[disPlane] + gdma->curPic[disPlane];
	pReg = &curPic->reg_content;

	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s line=%d disPlane=%d, workqueueIdx=%d \n", __FUNCTION__, __LINE__, disPlane, curPic->workqueueIdx);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE,"%s gdma->curPic[%d]=%d, curPic->onlineOrder=%d\n", __FUNCTION__, disPlane, gdma->curPic[disPlane], curPic->onlineOrder);


	if (disPlane == GDMA_PLANE_OSD2)
		offset = 0x100;
	else if (disPlane == GDMA_PLANE_OSD3)
		offset = 0x200;


	/*  set clear region */
	OSD_RTD_OUTL(GDMA_OSD1_CLEAR1_reg+offset, pReg->osd_clear1_reg.regValue);
	OSD_RTD_OUTL(GDMA_OSD1_CLEAR2_reg+offset, pReg->osd_clear2_reg.regValue);
	OSD_RTD_OUTL(GDMA_OSD1_reg + offset, pReg->osd_reg.regValue);

		OSD_RTD_OUTL(GDMA_OSD1_CTRL_reg+offset, ~1);
	OSD_RTD_OUTL(GDMA_OSD1_CTRL_reg+offset, pReg->osd_ctrl_reg.regValue);

	OSD_RTD_OUTL(GDMA_OSD1_WI_reg+offset, pReg->osd_wi_reg.regValue);
	//OSD_RTD_OUTL(GDMA_OSD1_WI_3D_reg+offset, pReg->osd_wi_3d_reg.regValue);
	OSD_RTD_OUTL(GDMA_OSD1_SIZE_reg+offset, pReg->osd_size_reg.regValue);

	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_CLEAR1_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_clear1_reg.regValue);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_CLEAR2_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_clear2_reg.regValue);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_reg.regValue);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_CTRL_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_ctrl_reg.regValue);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_WI_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_wi_reg.regValue);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_WI_3D_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_wi_3d_reg.regValue);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE,, "%s GDMA_OSD%d_SIZE_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_size_reg.regValue);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_line_buffer_end_reg=0x%x \n", __FUNCTION__, pReg->lb_end_reg.regValue);

	/*  call osd_sr  */
	GDMA_OSD1_DevCB_set_SR(1, disPlane, curPic);



	/*  TODO: Mixer : online blend order & plane alpha
	*	because k blend-factor setting, mixer layer order will reverse
	*/
	mixer_layer_sel_reg.regValue = OSD_RTD_INL(OSDOVL_Mixer_layer_sel_reg);
	if (curPic->onlineOrder == C0) {
		cx_sel_old = mixer_layer_sel_reg.c0_sel;
		cx_sel_new = mixer_layer_sel_reg.c0_sel = pReg->mixer_layer_sel_reg.c0_sel;

		if (pReg->mixer_layer_sel_reg.c0_plane_alpha_en != mixer_layer_sel_reg.c0_plane_alpha_en)
			mixer_layer_sel_reg.c0_plane_alpha_en = pReg->mixer_layer_sel_reg.c0_plane_alpha_en;

		OSD_RTD_OUTL(OSDOVL_Mixer_c0_plane_alpha1_reg, pReg->mixer_c0_plane_alpha1_reg.regValue);
		OSD_RTD_OUTL(OSDOVL_Mixer_c0_plane_alpha2_reg, pReg->mixer_c0_plane_alpha2_reg.regValue);
	} else if (curPic->onlineOrder == C1) {
		cx_sel_old = mixer_layer_sel_reg.c1_sel;
		cx_sel_new = mixer_layer_sel_reg.c1_sel = pReg->mixer_layer_sel_reg.c1_sel;

		if (pReg->mixer_layer_sel_reg.c1_plane_alpha_en != mixer_layer_sel_reg.c1_plane_alpha_en)
			mixer_layer_sel_reg.c1_plane_alpha_en = pReg->mixer_layer_sel_reg.c1_plane_alpha_en;

		OSD_RTD_OUTL(OSDOVL_Mixer_c1_plane_alpha1_reg, pReg->mixer_c1_plane_alpha1_reg.regValue);
		OSD_RTD_OUTL(OSDOVL_Mixer_c1_plane_alpha2_reg, pReg->mixer_c1_plane_alpha2_reg.regValue);
	} else if (curPic->onlineOrder == C2) {
		cx_sel_old = mixer_layer_sel_reg.c2_sel;
		cx_sel_new = mixer_layer_sel_reg.c2_sel = pReg->mixer_layer_sel_reg.c2_sel;

		if (pReg->mixer_layer_sel_reg.c2_plane_alpha_en != mixer_layer_sel_reg.c2_plane_alpha_en)
			mixer_layer_sel_reg.c2_plane_alpha_en = pReg->mixer_layer_sel_reg.c2_plane_alpha_en;

		OSD_RTD_OUTL(OSDOVL_Mixer_c2_plane_alpha1_reg, pReg->mixer_c2_plane_alpha1_reg.regValue);
		OSD_RTD_OUTL(OSDOVL_Mixer_c2_plane_alpha2_reg, pReg->mixer_c2_plane_alpha2_reg.regValue);
	} else if (curPic->onlineOrder == C3) {
		cx_sel_old = mixer_layer_sel_reg.c3_sel;
		cx_sel_new = mixer_layer_sel_reg.c3_sel = pReg->mixer_layer_sel_reg.c3_sel;

		if (pReg->mixer_layer_sel_reg.c3_plane_alpha_en != mixer_layer_sel_reg.c3_plane_alpha_en)
			mixer_layer_sel_reg.c3_plane_alpha_en = pReg->mixer_layer_sel_reg.c3_plane_alpha_en;

		OSD_RTD_OUTL(OSDOVL_Mixer_c3_plane_alpha1_reg, pReg->mixer_c3_plane_alpha1_reg.regValue);
		OSD_RTD_OUTL(OSDOVL_Mixer_c3_plane_alpha2_reg, pReg->mixer_c3_plane_alpha2_reg.regValue);
	}


	/* conflict protect */
	if (mixer_layer_sel_reg.c0_sel == cx_sel_new && curPic->onlineOrder != C0)
		mixer_layer_sel_reg.c0_sel = cx_sel_old;
	else if (mixer_layer_sel_reg.c1_sel == cx_sel_new && curPic->onlineOrder != C1)
		mixer_layer_sel_reg.c1_sel = cx_sel_old;
	else if (mixer_layer_sel_reg.c2_sel == cx_sel_new && curPic->onlineOrder != C2)
		mixer_layer_sel_reg.c2_sel = cx_sel_old;
	else if (mixer_layer_sel_reg.c3_sel == cx_sel_new && curPic->onlineOrder != C3)
		mixer_layer_sel_reg.c3_sel = cx_sel_old;

	#if 0
	if(enable_osd1osd3mixerorder) {
		mixer_layer_sel_reg.c0_sel = GDMA_PLANE_OSD2;
		mixer_layer_sel_reg.c1_sel = GDMA_PLANE_OSD1;
	}
	#endif//

	OSD_RTD_OUTL(OSDOVL_Mixer_layer_sel_reg, mixer_layer_sel_reg.regValue);

	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE,"%s, GDMA_line_buffer_end_reg=0x%x, OSDOVL_Mixer_layer_sel_reg=0x%x \n", __FUNCTION__, rtd_inl(GDMA_line_buffer_end_reg), mixer_layer_sel_reg.regValue);


	/*  call register debug.... */
	/* GDMA_RegDebug(); */

	return 0;
}

int GDMA_OSD1_DevCB_setMixerDone(GDMA_DISPLAY_PLANE disPlane, int waitFinish, void *info)
{
	osdovl_osd_db_ctrl_RBUS osd_db_ctrl_reg;


	osd_db_ctrl_reg.regValue = rtd_inl(OSDOVL_OSD_Db_Ctrl_reg);
	osd_db_ctrl_reg.db_load = 1;

	OSD_RTD_OUTL(OSDOVL_OSD_Db_Ctrl_reg, osd_db_ctrl_reg.regValue); /*  mixer apply */

	//GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s, w osd_db_ctrl = 0x%x \n", __FUNCTION__, osd_db_ctrl_reg.regValue);

	return 0;
}


int GDMA_OSD1_DevCB_set_ProgDone(GDMA_DISPLAY_PLANE disPlane, GMDA_OSD_DONE_MODE mode, void *info)
{
	unsigned int onlineProgDone = GDMA_CTRL_write_data(1);

	onlineProgDone |= GDMA_CTRL_osd1_prog_done(1);
	onlineProgDone |= GDMA_CTRL_osd2_prog_done(1);
	onlineProgDone |= GDMA_CTRL_osd3_prog_done(1);

	OSD_RTD_OUTL(GDMA_CTRL_reg, onlineProgDone);  /*  gdma & sr programming done */


	return 0;
}

int GDMA_OSD1_DevCB_getStatus(GDMA_DISPLAY_PLANE plane, GDMAExt_Status name, void *info, int* ret_status)
{
	int ret_err = 0;

	switch(name)
	{

		case GDMA_STATUS_INTR_ROUTING:
			*ret_status = OSD_RTD_INL(SYS_REG_INT_CTRL_SCPU_reg) & SYS_REG_INT_CTRL_SCPU_osd_int_scpu_routing_en_mask;
			

			break;

		default:
			GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "invalid query status %d p:%d\n", name, plane);

	}

	return ret_err;

}



#endif//RTK_GDMA_TEST_OSD1

int GDMA_OSD1_DevCB_DumpStatus(GDMA_DISPLAY_PLANE plane, int level)
{
	ppoverlay_osddtg_dv_total_RBUS osddtg_dv_total_rbus;
	ppoverlay_osddtg_dh_total_RBUS osddtg_dh_total_rbus;
	ppoverlay_osddtg_dv_den_start_end_RBUS osddtg_dv_den_start_end_rbus;
	ppoverlay_osddtg_dh_den_start_end_RBUS osddtg_dh_den_start_end_rbus;
	ppoverlay_osddtg_control_RBUS osddtg_control_rbus;

	(void)plane;
	(void)level;

	osddtg_dv_total_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osddtg_DV_TOTAL_reg);
	osddtg_dh_total_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osddtg_DH_TOTAL_reg);
	osddtg_dv_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osddtg_DV_DEN_Start_End_reg);
	osddtg_dh_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osddtg_DH_DEN_Start_End_reg);

	osddtg_control_rbus.regValue = OSD_RTD_INL( PPOVERLAY_OSDDTG_CONTROL_reg );

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, "osd1  dtg  %u %u(%u) hDen:%u %u \n", osddtg_dv_total_rbus.osddtg_dv_total,  
		osddtg_dh_total_rbus.osddtg_dh_total, osddtg_dh_total_rbus.osddtg_dh_total_last_line, 
		osddtg_dv_den_start_end_rbus.osddtg_dv_den_sta, osddtg_dv_den_start_end_rbus.osddtg_dv_den_end,
		osddtg_dh_den_start_end_rbus.osddtg_dh_den_sta, osddtg_dh_den_start_end_rbus.osddtg_dh_den_end
		);

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, " dtg h:%u(last:%u) v:%u  hDen:[%u %u](%u) vDen:[%u %u](%u)\n", 
		osddtg_dh_total_rbus.osddtg_dh_total, osddtg_dh_total_rbus.osddtg_dh_total_last_line, 
		osddtg_dv_total_rbus.osddtg_dv_total,  
		osddtg_dh_den_start_end_rbus.osddtg_dh_den_sta, osddtg_dh_den_start_end_rbus.osddtg_dh_den_end, osddtg_dh_den_start_end_rbus.osddtg_dh_den_end - osddtg_dh_den_start_end_rbus.osddtg_dh_den_sta,
		osddtg_dv_den_start_end_rbus.osddtg_dv_den_sta, osddtg_dv_den_start_end_rbus.osddtg_dv_den_end, osddtg_dv_den_start_end_rbus.osddtg_dv_den_end - osddtg_dv_den_start_end_rbus.osddtg_dv_den_sta
	);


	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, "osd1 dtgCtrl en:%u split:%u fsyncEn:%u by_hw:%u fST:%u cnt_sync:%u resetEn:%u sel:%u mode:%u\n", 
	  osddtg_control_rbus.osddtg_en, osddtg_control_rbus.osd_split, 
	  osddtg_control_rbus.osddtg_fsync_en, osddtg_control_rbus.osddtg_frc2fsync_by_hw, osddtg_control_rbus.osddtg_frc_fsync_status,
	  osddtg_control_rbus.osddtg_line_cnt_sync, osddtg_control_rbus.osddtg_frame_cnt_reset_en, osddtg_control_rbus.osddtg_fsync_select,
      osddtg_control_rbus.osddtg_mode_revised
	);


	//gdma_osd_winfo_dump(void);


	return 0;
}

///////////////// start new OSD ////////////////////////////////////////

void GDMAExt_config_line_buffer( GDMA_DISPLAY_PLANE plane, int is_4k )
{
	if( plane == GDMA_PLANE_OSD4) {
		// GDMA4_osd4_line_buffer_sta_reg 
		gdma4_osd4_line_buffer_sta_RBUS line_buffer_sta_rbus;
		gdma4_osd4_line_buffer_size_RBUS line_buffer_size_rbus;


		if( is_4k) {
		// 4k bypass
		line_buffer_size_rbus.l3 = 6; 
		line_buffer_sta_rbus.l3 = 0;

		line_buffer_sta_rbus.l6 = 6;
		line_buffer_size_rbus.l6 = 0;


		}
		else {
		//default 
		line_buffer_size_rbus.l3 = 4; 
		line_buffer_sta_rbus.l3 = 0;

		line_buffer_sta_rbus.l6 = 4;
		line_buffer_size_rbus.l6 = 2;

		}


		OSD_RTD_OUTL(GDMA4_osd4_line_buffer_sta_reg, line_buffer_sta_rbus.regValue);
		OSD_RTD_OUTL(GDMA4_osd4_line_buffer_size_reg, line_buffer_size_rbus.regValue);

		//(1)	buffer_sta is controlled by osd4_prog_done. do outside 
		
	}
	else if ( plane == GDMA_PLANE_OSD5 ) {
	
		gdma5_osd5_line_buffer_sta_RBUS line_buffer_sta_rbus;
		gdma5_osd5_line_buffer_size_RBUS line_buffer_size_rbus;

		if( is_4k) {
			line_buffer_size_rbus.l7 = 6;
			line_buffer_sta_rbus.l7 = 0;

			line_buffer_sta_rbus.l8 = 6;
			line_buffer_size_rbus.l8 = 0;

		}
		else {
			line_buffer_size_rbus.l7 = 4;
			line_buffer_sta_rbus.l7 = 0;

			line_buffer_sta_rbus.l8 = 4;
			line_buffer_size_rbus.l8 = 2;
		}


		OSD_RTD_OUTL(GDMA5_osd5_line_buffer_sta_reg, line_buffer_sta_rbus.regValue);
		OSD_RTD_OUTL(GDMA5_osd5_line_buffer_size_reg, line_buffer_size_rbus.regValue);


	}


}


void GDMAExt_config_mid_blend(void )
{


}


void GDMAExt_config_fbc( GDMA_DISPLAY_PLANE plane )
{
	if( plane == GDMA_PLANE_OSD4) {
		gdma4_dma_swap_RBUS dma_swap_reg;

		dma_swap_reg.regValue = rtd_inl(GDMA4_DMA_SWAP_reg);
		dma_swap_reg.afbc4_1b = 1;
		dma_swap_reg.afbc4_2b = 1;
		dma_swap_reg.afbc4_4b = 1;
		dma_swap_reg.afbc4_8b = 1;
		
		#if 0
			dma_swap_reg.osd4_1b
 			dma_swap_reg.osd4_2b
 			dma_swap_reg.osd4_4b
 			dma_swap_reg.osd4_8b

		#endif//
		
		OSD_RTD_OUTL(GDMA4_DMA_SWAP_reg, dma_swap_reg.regValue);
	}
	else if ( plane == GDMA_PLANE_OSD5 ) {
		gdma5_dma_swap_RBUS dma_swap_reg;

		dma_swap_reg.regValue = rtd_inl(GDMA5_DMA_SWAP_reg);
		dma_swap_reg.afbc5_1b = 1;
		dma_swap_reg.afbc5_2b = 1;
		dma_swap_reg.afbc5_4b = 1;
		dma_swap_reg.afbc5_8b = 1;
		
		#if 0
			dma_swap_reg.osd5_1b
 			dma_swap_reg.osd5_2b
 			dma_swap_reg.osd5_4b
 			dma_swap_reg.osd5_8b

		#endif//
		
		OSD_RTD_OUTL(GDMA5_DMA_SWAP_reg, dma_swap_reg.regValue);
	}

}

//caller need call mutex_lock(&gOSD_MGR_LOCK);
void GDMAExt_ClkEnable(GDMA_DISPLAY_PLANE plane, int enable)
{

	if( plane == VO_GRAPHIC_OSD4 ) {

		if(enable) {
			//CLKEN = 0
			OSD_RTD_OUTL( SYS_REG_SYS_CLKEN5_reg, SYS_REG_SYS_CLKEN5_clken_gdma4_mask | SYS_REG_SYS_CLKEN5_write_data(0) );
			udelay(100);
	
			//RSTN = 0
			OSD_RTD_OUTL( SYS_REG_SYS_SRST5_reg, SYS_REG_SYS_SRST5_rstn_gdma4_mask | SYS_REG_SYS_SRST5_write_data(0) );
			udelay(100);
	
			//RSTN = 1
			OSD_RTD_OUTL( SYS_REG_SYS_SRST5_reg, SYS_REG_SYS_SRST5_rstn_gdma4_mask | SYS_REG_SYS_SRST5_write_data(0x1) );
			udelay(100);
	
			//CLKEN = 1
			OSD_RTD_OUTL( SYS_REG_SYS_CLKEN5_reg, (SYS_REG_SYS_CLKEN5_clken_gdma4_mask | SYS_REG_SYS_CLKEN5_clken_gdma4_rbus_mask )
				| SYS_REG_SYS_CLKEN5_write_data(0x1) );
			udelay(100);

		}
		else {
			//not disable _rbus 
			//CLKEN = 0   [11]
			OSD_RTD_OUTL( SYS_REG_SYS_CLKEN5_reg, SYS_REG_SYS_CLKEN5_clken_gdma4_mask | SYS_REG_SYS_CLKEN5_write_data(0) );
			udelay(100);

			//RSTN = 0  [11]
			OSD_RTD_OUTL( SYS_REG_SYS_SRST5_reg, SYS_REG_SYS_SRST5_rstn_gdma4_mask | SYS_REG_SYS_SRST5_write_data(0) );
			udelay(100);
		}

	}
	else if (plane == VO_GRAPHIC_OSD5 ) {
		if(enable) {

			//CLKEN = 0
			OSD_RTD_OUTL( SYS_REG_SYS_CLKEN5_reg, SYS_REG_SYS_CLKEN5_clken_gdma5_mask | SYS_REG_SYS_CLKEN5_write_data(0) );
			udelay(100);
	
			//RSTN = 0
			OSD_RTD_OUTL( SYS_REG_SYS_SRST5_reg, SYS_REG_SYS_SRST5_rstn_gdma5_mask | SYS_REG_SYS_SRST5_write_data(0) );
			udelay(100);
	
			//RSTN = 1
			OSD_RTD_OUTL( SYS_REG_SYS_SRST5_reg, SYS_REG_SYS_SRST5_rstn_gdma5_mask | SYS_REG_SYS_SRST5_write_data(0x1) );
			udelay(100);
	
			//CLKEN = 1
			OSD_RTD_OUTL( SYS_REG_SYS_CLKEN5_reg, (SYS_REG_SYS_CLKEN5_clken_gdma5_mask | SYS_REG_SYS_CLKEN5_clken_gdma5_rbus_mask )
				| SYS_REG_SYS_CLKEN5_write_data(0x1) );
			udelay(100);
		}
		else {
			//CLKEN = 0
			OSD_RTD_OUTL( SYS_REG_SYS_CLKEN5_reg, SYS_REG_SYS_CLKEN5_clken_gdma5_mask | SYS_REG_SYS_CLKEN5_write_data(0) );
			udelay(100);

			//RSTN = 0
			OSD_RTD_OUTL( SYS_REG_SYS_SRST5_reg, SYS_REG_SYS_SRST5_rstn_gdma5_mask | SYS_REG_SYS_SRST5_write_data(0) );
			udelay(100);
		}


	}


	rtd_pr_gdma_warn("osdEx clk:%d plane:%d", enable, plane);


}


void GDMAExt_getDispSize(GDMA_DISPLAY_PLANE plane, VO_RECTANGLE *disp_rec)
{
	if( plane == GDMA_PLANE_OSD4 ) {
		ppoverlay_osd4dtg_dv_den_start_end_RBUS dv_den_start_end_rbus;
		ppoverlay_osd4dtg_dh_den_start_end_RBUS dh_den_start_end_rbus;
	

		dv_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd4dtg_DV_DEN_Start_End_reg);
		dh_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd4dtg_DH_DEN_Start_End_reg);

		disp_rec->width = dh_den_start_end_rbus.osd4dtg_dh_den_end - dh_den_start_end_rbus.osd4dtg_dh_den_sta;
		disp_rec->height = dv_den_start_end_rbus.osd4dtg_dv_den_end - dv_den_start_end_rbus.osd4dtg_dv_den_sta;

	}
	else if(  plane == GDMA_PLANE_OSD5 ) {

		ppoverlay_osd5dtg_dv_den_start_end_RBUS dv_den_start_end_rbus;
		ppoverlay_osd5dtg_dh_den_start_end_RBUS dh_den_start_end_rbus;
	
		dv_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd5dtg_DV_DEN_Start_End_reg);
		dh_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd5dtg_DH_DEN_Start_End_reg);

		disp_rec->width = dh_den_start_end_rbus.osd5dtg_dh_den_end - dh_den_start_end_rbus.osd5dtg_dh_den_sta;
		disp_rec->height = dv_den_start_end_rbus.osd5dtg_dv_den_end - dv_den_start_end_rbus.osd5dtg_dv_den_sta;

	}
	else if( plane == GDMA_PLANE_OSD1 
			|| plane == GDMA_PLANE_OSD2 
	)
	{
		getDispSize(disp_rec);  //call org osd1 

	}


	//#ifndef GDMA_CONFIG_ENABLE_FPGA
	#if 0
	if( disp_rec->width < 1280 || disp_rec->height < 720 
	   ||  disp_rec->width > 6000 || disp_rec->height > 6000 
	   ) {

		GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, "getDispSize may err osd:%d %d %d %d %d \n", plane, disp_rec->x, disp_rec->y,disp_rec->width, disp_rec->height) ;
	}
	#endif//

}

int GDMAExt_modify_mixer(GDMA_MODIFY_MIXER_CMD* mixer_cmd)
{
	if( mixer_cmd->plane == VO_GRAPHIC_OSD4 ) 
	{

		osdovl4_mixer4_ctrl2_RBUS mixer_ctrl2;
	
		*(volatile u32 *)&mixer_ctrl2 = rtd_inl(OSDOVL4_Mixer4_CTRL2_reg);


    	mixer_ctrl2.mixero4_en  = mixer_cmd->enable;

		if(  mixer_cmd->enable ) {
			mixer_ctrl2.mixer_en = 1;
		}

		OSD_RTD_OUTL(OSDOVL4_Mixer4_CTRL2_reg, *(volatile u32 *)&mixer_ctrl2);

	}
	else if ( mixer_cmd->plane == VO_GRAPHIC_OSD5 ) {

		osdovl5_mixer5_ctrl2_RBUS mixer_ctrl2;
	
		*(volatile u32 *)&mixer_ctrl2 = rtd_inl(OSDOVL5_Mixer5_CTRL2_reg);


    	mixer_ctrl2.mixer_en  = mixer_cmd->enable;

		OSD_RTD_OUTL(OSDOVL5_Mixer5_CTRL2_reg, *(volatile u32 *)&mixer_ctrl2);
	}



	return 0;
}

void GDMAExt_OSD4_Clk_Select(int src_type, bool enable)
{
	#if 0

	#endif//

}

int GDMA_OSD4_DevCB_init(GDMA_DISPLAY_PLANE plane, void *info)
{
    //gdma_dev* gdma = (gdma_dev*) info ; 
//    gdma_dev* gdma = (gdma_dev*) info ; 
    
	
	osdovl4_mixer4_ctrl2_RBUS mixer_ctrl2;
	osdovl4_mixer4_layer_sel_RBUS mixer_layer_sel_reg;
	gdma4_o4_tfbdc_config_RBUS tfbdc_config;
	//sys_reg_sys_dispclksel_RBUS sys_dispclksel_reg;

	gdma4_osd4_RBUS GDMA_osd;


	
	GDMAExt_ClkEnable(GDMA_PLANE_OSD4, 1);

	#if 0 //keep default
	{
	gdma4_dma_RBUS  gdma_dma_reg;
	gdma_dma_reg.regValue = OSD_RTD_INL(GDMA4_DMA_reg);
		gdma_dma_reg.afbc4_req_num = 32;	//default 32
		//gdma_dma_reg.osd4_req_num = 7;
	OSD_RTD_OUTL(GDMA4_DMA_reg, gdma_dma_reg.regValue);
	}
	#endif//

	GDMA_osd.wi_endian = 0;
	GDMA_osd.osd = 1;
	GDMA_osd.write_data = 1;
	OSD_RTD_OUTL(GDMA4_OSD4_reg, GDMA_osd.regValue );
	
	/*tfbc group cannot dynamic change*/
	tfbdc_config.regValue = rtd_inl(GDMA4_O4_TFBDC_CONFIG_reg);
	tfbdc_config.tfbdc_lossy_group_control = 1;
	tfbdc_config.tfbdc_swizzle = 4;
	tfbdc_config.tfbdc_lossy_min_channel_override = 0;
	rtd_outl(GDMA4_O4_TFBDC_CONFIG_reg, tfbdc_config.regValue);
	


 	OSD_RTD_OUTL(GDMA4_OSD4_CTRL_reg, ~1);

	GDMAExt_config_line_buffer( GDMA_PLANE_OSD4, 0 );

	GDMA_OSD4_DevCB_set_ProgDone ( GDMA_PLANE_OSD4, GMDA_OSD_DONE_MODE_SINGLE, NULL);


	#if 0
		/* wait prog done */
		while ((rtd_inl(GDMA_CTRL_reg)&prog_done_mask) != 0) {
			gdma_usleep(1000);
			to_cnt++;
			if (to_cnt > 40) {
				GDMA_EXT_LOG(GDMA_EXT_LOG_WARNING, "%s, GDMA HW something wrong. Please check \n", __FUNCTION__);
				break;
			}
		}
		
	#endif

	GDMAExt_config_fbc( GDMA_PLANE_OSD4 );

	mixer_ctrl2.regValue = OSD_RTD_INL(OSDOVL4_Mixer4_CTRL2_reg);
	
#if OSD_DETECTION_SUPPORT
	mixer_ctrl2.measure_osd_zone_en = 1;
	mixer_ctrl2.measure_osd_zone_type = 0; //A=0, R=0, G=0, B=0
#endif



	mixer_ctrl2.video_src_sel = 1;
	mixer_ctrl2.mixero4_en    = 1;
	mixer_ctrl2.mixer_en = 1;

   	OSD_RTD_OUTL(OSDOVL4_Mixer4_CTRL2_reg, mixer_ctrl2.regValue);

	/* wdma_bl set to 'd80 by HW RD suggestion */
	/*dma_bl_2_reg.regValue = rtd_inl(GDMA_DMA_BL_2_reg);
		dma_bl_2_reg.wdma_bl = 80;
		OSD_RTD_OUTL(GDMA_DMA_BL_2_reg, dma_bl_2_reg.regValue);
	*/

	/* mixer layer sel: video keep at c5 place */
	mixer_layer_sel_reg.regValue = OSD_RTD_INL(OSDOVL4_Mixer4_layer_sel_reg);
	mixer_layer_sel_reg.c0_sel = 1; // osd4
	mixer_layer_sel_reg.c1_sel = 0; // video
	OSD_RTD_OUTL(OSDOVL4_Mixer4_layer_sel_reg, mixer_layer_sel_reg.regValue);

	GDMAExt_config_mid_blend();  // null currently

	GDMA_OSD4_DevCB_set_ProgDone ( GDMA_PLANE_OSD4, GMDA_OSD_DONE_MODE_SINGLE, NULL);



	

	

	{
		gdma_total_ddr_size = 0x60000000;
		gdma_total_ddr_size = get_memory_size_total() - 0x100;

		GDMA_EXT_LOG(GDMA_EXT_LOG_DEBUG, "[osd4] get total memsize = %x\n", gdma_total_ddr_size);

		if (gdma_total_ddr_size) {
			OSD_RTD_OUTL(GDMA4_AFBC4_MIN_reg, 0x0);
			OSD_RTD_OUTL(GDMA4_AFBC4_MAX_reg, gdma_total_ddr_size);
			OSD_RTD_OUTL(GDMA4_OSD4_MIN_reg, 0x0);
			OSD_RTD_OUTL(GDMA4_OSD4_MAX_reg, gdma_total_ddr_size-0x1000);
		} else {
			GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s gdma_total_ddr_size is empty!\n", __func__);
		}
	}


	return 0;
}


int GDMA_OSD4_DevCB_preset_GDMA( GDMA_PIC_DATA *curPic, struct gdma_receive_work *ptr_work)
{
	GDMA_WIN *win = NULL ;
	gdma_dev *gdma = &gdma_devices[0];

	GDMA_DISPLAY_PLANE disPlane = ptr_work->disPlane;
	GDMA_4_REG_CONTENT *pReg = &curPic->reg_4_content;

	int rotateBit = gdma->ctrl.enableVFlip;
	int canvasW, canvasH;

	
	win = &curPic->OSDwin;

	canvasW = win->winWH.width;
	canvasH = win->winWH.height;

		/*  set clear region */
		if (curPic->clear_x.value != 0 || curPic->clear_y.value != 0) {
			pReg->osd_clear1_reg.regValue = curPic->clear_x.value;
			pReg->osd_clear2_reg.regValue = curPic->clear_y.value;
			pReg->osd_reg.regValue = GDMA4_OSD4_clear_region_en(1) | GDMA4_OSD4_write_data(1);
		} else {
			pReg->osd_reg.regValue = GDMA4_OSD4_clear_region_en(1) | GDMA4_OSD4_write_data(0);
		}

		if (gdma->ctrl.osdfirstEn[disPlane] == 0) {

			pReg->osd_ctrl_reg.regValue = GDMA4_OSD4_CTRL_write_data(1) | GDMA4_OSD4_CTRL_osd4_en(curPic->show) 
										| GDMA4_OSD4_CTRL_rotate(rotateBit);

			gdma->ctrl.osdEn[disPlane] = 1;
			gdma->ctrl.osdfirstEn[disPlane] = 1;
			
			GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s, win=0x%lx, phy=0x%x\n", __FUNCTION__, win, virt_to_phys(win));
		} else {
			/*  if osdEn changes to 0, driver will force curPic->show to zero and disable OSD */
			if (gdma->ctrl.osdEn[disPlane] == 0)
				curPic->show = 0;

			pReg->osd_ctrl_reg.regValue = GDMA4_OSD4_CTRL_write_data(1) | GDMA4_OSD4_CTRL_osd4_en(curPic->show) | GDMA4_OSD4_CTRL_rotate(rotateBit);


		}

		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, " curPic->show=%d, gdma->ctrl.osdEn[%d]=%d \n",  curPic->show, disPlane, gdma->ctrl.osdEn[disPlane]);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," osd%d  win=0x%lx, top=0x%x f_inputsrc_4k2k=%d \n",  disPlane, win, win->top_addr, gdma->f_inputsrc_4k2k);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," input src[x,y,w,h]= [%d,%d,%d,%d] \n",  win->winXY.x, win->winXY.y, canvasW, canvasH);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," output[x,y,w,h]= [%d,%d,%d,%d] \n",  win->dst_x, win->dst_y, win->dst_width ,win->dst_height);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," plane_alpha_r=0x%x, plane_alpha_a=0x%x \n",  curPic->plane_ar.plane_alpha_r, curPic->plane_ar.plane_alpha_a);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," plane_alpha_g=0x%x, plane_alpha_b=0x%x \n",  curPic->plane_gb.plane_alpha_g, curPic->plane_gb.plane_alpha_b);

		pReg->osd_wi_reg.regValue = GDMA4_OSD4_WI_addr (virt_to_phys(win));
		
        if (pReg->osd_wi_reg.regValue == 0) {
            #if defined(CONFIG_ARM64)
                GDMA_ERR( "[GDMA4] addr = 0!!!!!!!! win = 0x%lx gIdxQwr = %d pitch = 0x%x\n", (unsigned long)win, ptr_work->picQwr, win->pitch);
            #else
                GDMA_ERR( "[GDMA4] addr = 0!!!!!!!! win = 0x%x gIdxQwr = %d pitch = 0x%x\n", (unsigned int)win, ptr_work->picQwr, win->pitch);
            #endif
        } else if (pReg->osd_wi_reg.regValue > gdma_total_ddr_size) {
        
            #if defined(CONFIG_ARM64)
                GDMA_ERR( "[GDMA4] addr = 0x%lx, win info addr over range!!\n", (unsigned long)win);
            #else
                GDMA_ERR( "[GDMA4] addr = 0x%x, win info addr over range!!\n", (unsigned int)win);
            #endif
        }
		//pReg->osd_size_reg.regValue = GDMA_OSD1_SIZE_w(canvasW) | GDMA_OSD1_SIZE_h(canvasH);
		pReg->osd_size_reg.regValue = GDMA4_OSD4_SIZE_w(canvasW) | GDMA4_OSD4_SIZE_h(canvasH);

	return 0;
}

// GDMAExt_PreOSDReg_Set_Mixer ( GDMA_PIC_DATA *curPic)
int GDMA_OSD4_DevCB_preset_Mixer( GDMA_PIC_DATA *curPic )
{
	GDMA_4_REG_CONTENT* pReg = &curPic->reg_4_content;
	
	//OSDExt add
	GDMA_DISPLAY_PLANE disPlane = curPic->plane;

	// check to set plane alpha  
	// C0, C1 only 
	if (curPic->onlineOrder == C0) {
		pReg->mixer_layer_sel_reg.c0_sel = disPlane;

		if ( (curPic->plane_ar.plane_alpha_r != 0xff) || (curPic->plane_ar.plane_alpha_a != 0xff) 
			|| 	(curPic->plane_gb.plane_alpha_g != 0xff) || (curPic->plane_gb.plane_alpha_b != 0xff) 
		) {
			pReg->mixer_layer_sel_reg.c0_plane_alpha_en = 1;
		}

		pReg->mixer_c0_plane_alpha1_reg.regValue = curPic->plane_ar.value;
		pReg->mixer_c0_plane_alpha2_reg.regValue = curPic->plane_gb.value;

	} else if (curPic->onlineOrder == C1) {
		pReg->mixer_layer_sel_reg.c1_sel = disPlane;

		if ((curPic->plane_ar.plane_alpha_r != 0xff) || (curPic->plane_ar.plane_alpha_a != 0xff) 
			|| (curPic->plane_gb.plane_alpha_g != 0xff) || (curPic->plane_gb.plane_alpha_b != 0xff) 
		) {
			pReg->mixer_layer_sel_reg.c1_plane_alpha_en = 1;
		}

		pReg->mixer_c1_plane_alpha1_reg.regValue = curPic->plane_ar.value;
		pReg->mixer_c1_plane_alpha2_reg.regValue = curPic->plane_gb.value;

	} 

	// . a_det_osd4_en  ; //enable alpha detect

	return 0;
}


/*

  GDMAExt_PreOSDReg_Set_GDMA(curPic, ptr_work);
  GDMAExt_PreOSDReg_Set_SR( 1, &srcWin, &dispWin, curPic);
  GDMAExt_PreOSDReg_Set_Mixer ( curPic );

*/
int GDMA_OSD4_DevCB_set_PresetOSD(GDMA_DISPLAY_PLANE plane, GDMA_PIC_DATA *curPic, VO_RECTANGLE* srcWin, VO_RECTANGLE* dispWin,
                                 struct gdma_receive_work *ptr_work)
{
	int err = 0;

	err = GDMA_OSD4_DevCB_preset_GDMA(curPic, ptr_work);
	
	err = GDMA_OSD4_DevCB_preset_SR( 1, *srcWin, *dispWin, curPic);

	err = GDMA_OSD4_DevCB_preset_Mixer( curPic );


	return err;
}

int GDMA_OSD4_DevCB_setMixerDone(GDMA_DISPLAY_PLANE disPlane, int waitFinish, void *info )
{
	osdovl4_osd4_db_ctrl_RBUS osd_db_ctrl_reg;
	int ltimeout =0xff;

	osd_db_ctrl_reg.regValue = rtd_inl(OSDOVL4_OSD4_Db_Ctrl_reg);
	osd_db_ctrl_reg.db_load = 1;

	OSD_RTD_OUTL(OSDOVL4_OSD4_Db_Ctrl_reg, osd_db_ctrl_reg.regValue); /*  mixer apply */

	osd_db_ctrl_reg.regValue = rtd_inl(OSDOVL4_OSD4_Db_Ctrl_reg);

	if ( waitFinish == 1) {

		while(osd_db_ctrl_reg.db_load && ltimeout--) {
		    osd_db_ctrl_reg.regValue = rtd_inl(OSDOVL4_OSD4_Db_Ctrl_reg);
		    mdelay(1);
		}

	}

	GDMAEXT_DUMP("%s mixer4 :0x%x", __FUNCTION__, OSD_RTD_INL( OSDOVL4_Mixer4_CTRL2_reg)  );
	
	return 0;
}


int GDMA_OSD4_DevCB_set_ProgDone(GDMA_DISPLAY_PLANE disPlane, GMDA_OSD_DONE_MODE mode, void *info)
{
	unsigned int onlineProgDone = GDMA4_CTRL4_write_data_mask;

	if( mode == GMDA_OSD_DONE_MODE_SYNC45) {
		//done in osd 5's progDone

	} else {
		onlineProgDone |= GDMA4_CTRL4_osd4_prog_done(1);

			//  GDMA4_CTRL4_o4o5_prog_done(1)
		OSD_RTD_OUTL(GDMA4_CTRL4_reg, onlineProgDone);


	}




	return 0;
}

int GDMA_OSD4_DevCB_SetGDMA(GDMA_DISPLAY_PLANE disPlane, void *info)
{

	//MUST_FIXME  use osd 4 reg 

	gdma_dev* gdma = (gdma_dev*) info ;
	
	GDMA_PIC_DATA *curPic;

	osdovl_mixer_layer_sel_RBUS mixer_layer_sel_reg;
	

	GDMA_4_REG_CONTENT* pReg = NULL;	// GDMA_REG_CONTENT


	curPic = gdma->pic[disPlane] + gdma->curPic[disPlane];
	//pReg = &curPic->reg_content;
	pReg = &curPic->reg_4_content;

	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s line=%d disPlane=%d, workqueueIdx=%d \n", __FUNCTION__, __LINE__, disPlane, curPic->workqueueIdx);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE,"%s gdma->curPic[%d]=%d, curPic->onlineOrder=%d\n", __FUNCTION__, disPlane, gdma->curPic[disPlane], curPic->onlineOrder);

	#if 0
	if (disPlane == GDMA_PLANE_OSD2)
		offset = 0x100;
	else if (disPlane == GDMA_PLANE_OSD3)
		offset = 0x200;
	#endif//

	/*  set clear region */
	OSD_RTD_OUTL(GDMA4_OSD4_CLEAR1_reg, pReg->osd_clear1_reg.regValue);
	OSD_RTD_OUTL(GDMA4_OSD4_CLEAR2_reg, pReg->osd_clear2_reg.regValue);
	OSD_RTD_OUTL(GDMA4_OSD4_reg , pReg->osd_reg.regValue);

	OSD_RTD_OUTL(GDMA4_OSD4_CTRL_reg , ~1);  // 0xfffffffe
	OSD_RTD_OUTL(GDMA4_OSD4_CTRL_reg , pReg->osd_ctrl_reg.regValue);

	OSD_RTD_OUTL(GDMA4_OSD4_WI_reg , pReg->osd_wi_reg.regValue);
	OSD_RTD_OUTL(GDMA4_OSD4_SIZE_reg , pReg->osd_size_reg.regValue);

		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_CLEAR1_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_clear1_reg.regValue);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_CLEAR2_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_clear2_reg.regValue);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_reg.regValue);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_CTRL_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_ctrl_reg.regValue);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_WI_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_wi_reg.regValue);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE,, "%s GDMA_OSD%d_SIZE_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_size_reg.regValue);
		//GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_line_buffer_end_reg=0x%x \n", __FUNCTION__, pReg->lb_end_reg.regValue);

	/*  call osd_sr  */
	GDMA_OSD4_DevCB_set_SR(1, disPlane, curPic);


	
	mixer_layer_sel_reg.regValue = OSD_RTD_INL(OSDOVL4_Mixer4_layer_sel_reg);

	#if 0
  {
		unsigned int cx_sel_old = 0, cx_sel_new = 0;

	if (curPic->onlineOrder == C0) {
		cx_sel_old = mixer_layer_sel_reg.c0_sel;
		cx_sel_new = mixer_layer_sel_reg.c0_sel = pReg->mixer_layer_sel_reg.c0_sel;

		if (pReg->mixer_layer_sel_reg.c0_plane_alpha_en != mixer_layer_sel_reg.c0_plane_alpha_en)
			mixer_layer_sel_reg.c0_plane_alpha_en = pReg->mixer_layer_sel_reg.c0_plane_alpha_en;

		OSD_RTD_OUTL(OSDOVL4_Mixer4_c0_plane_alpha1_reg, pReg->mixer_c0_plane_alpha1_reg.regValue);
		OSD_RTD_OUTL(OSDOVL4_Mixer4_c0_plane_alpha2_reg, pReg->mixer_c0_plane_alpha2_reg.regValue);

	} else if (curPic->onlineOrder == C1) {
		cx_sel_old = mixer_layer_sel_reg.c1_sel;
		cx_sel_new = mixer_layer_sel_reg.c1_sel = pReg->mixer_layer_sel_reg.c1_sel;

		if (pReg->mixer_layer_sel_reg.c1_plane_alpha_en != mixer_layer_sel_reg.c1_plane_alpha_en)
			mixer_layer_sel_reg.c1_plane_alpha_en = pReg->mixer_layer_sel_reg.c1_plane_alpha_en;

		OSD_RTD_OUTL(OSDOVL4_Mixer4_c1_plane_alpha1_reg, pReg->mixer_c1_plane_alpha1_reg.regValue);
		OSD_RTD_OUTL(OSDOVL4_Mixer4_c1_plane_alpha2_reg, pReg->mixer_c1_plane_alpha2_reg.regValue);
	}
	
	/* conflict protect */
	if (mixer_layer_sel_reg.c0_sel == cx_sel_new && curPic->onlineOrder != C0)
		mixer_layer_sel_reg.c0_sel = cx_sel_old;
	else if (mixer_layer_sel_reg.c1_sel == cx_sel_new && curPic->onlineOrder != C1)
		mixer_layer_sel_reg.c1_sel = cx_sel_old;

	OSD_RTD_OUTL(OSDOVL4_Mixer4_layer_sel_reg, mixer_layer_sel_reg.regValue);
  }
  
	#endif//

	//GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE,"%s, line_buffer_end_reg=0x%x, layer_sel_reg=0x%x \n", __FUNCTION__, rtd_inl(GDMA_line_buffer_end_reg), mixer_layer_sel_reg.regValue);


	return 0;
}


/*

 dump_wininfo_Status
 dump_tg_Status
 dump_GDMA_Status
 dump_SR_Status
 dump_intr_Status
 dump_vsync_Status



  @level:
	GDMA_EXT_LOG_ALL: dump all


	GDMA_EXT_LOG_VERBOSE: dump basic winInfo, SR status

*/
int GDMA_OSD4_DevCB_DumpStatus(GDMA_DISPLAY_PLANE plane, int level)
{
	(void)plane;
	(void)level;
	
	{
	//memctg. control by disp_end ?
	ppoverlay_memcdtg_dv_total_RBUS memcdtg_dv_total_rbus;

	ppoverlay_memcdtg_dh_total_RBUS memcdtg_dh_total_rbus;
	ppoverlay_memcdtg_dv_den_start_end_RBUS memcdtg_dv_den_start_end_rbus;
	ppoverlay_memcdtg_dh_den_start_end_RBUS memcdtg_dh_den_start_end_rbus;

	//ppoverlay_memcdtg_control_RBUS osddtg_control_rbus;

	memcdtg_dv_total_rbus.regValue = OSD_RTD_INL(PPOVERLAY_memcdtg_DV_TOTAL_reg);
	memcdtg_dh_total_rbus.regValue = OSD_RTD_INL(PPOVERLAY_memcdtg_DH_TOTAL_reg);
	memcdtg_dv_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg);
	memcdtg_dh_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_memcdtg_DH_DEN_Start_End_reg);
	//osddtg_control_rbus.regValue = OSD_RTD_INL( PPOVERLAY_memcdtg_CONTROL_reg );

	GDMA_EXT_LOG(GDMA_EXT_LOG_DUMP, "\n\n\n=====+ OSD4 +=====");

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, "memc dtg  h:%u(last:%u) v:%u hDen:[%u %u](%u) vDen:[%u %u](%u) \n", 
		memcdtg_dh_total_rbus.memcdtg_dh_total, memcdtg_dh_total_rbus.memcdtg_dh_total_last_line, 
		memcdtg_dv_total_rbus.memcdtg_dv_total,  
		memcdtg_dh_den_start_end_rbus.memcdtg_dh_den_sta, memcdtg_dh_den_start_end_rbus.memcdtg_dh_den_end, memcdtg_dh_den_start_end_rbus.memcdtg_dh_den_end - memcdtg_dh_den_start_end_rbus.memcdtg_dh_den_sta,
		memcdtg_dv_den_start_end_rbus.memcdtg_dv_den_sta, memcdtg_dv_den_start_end_rbus.memcdtg_dv_den_end, memcdtg_dv_den_start_end_rbus.memcdtg_dv_den_end - memcdtg_dv_den_start_end_rbus.memcdtg_dv_den_sta
		);
  }

	{
		//DTG
	ppoverlay_osd4dtg_dv_total_RBUS osddtg_dv_total_rbus;
	ppoverlay_osd4dtg_dh_total_RBUS osddtg_dh_total_rbus;
	ppoverlay_osd4dtg_dv_den_start_end_RBUS osddtg_dv_den_start_end_rbus;
	ppoverlay_osd4dtg_dh_den_start_end_RBUS osddtg_dh_den_start_end_rbus;
	ppoverlay_osd4dtg_control_RBUS osddtg_control_rbus;

	osddtg_dv_total_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd4dtg_DV_TOTAL_reg);
	osddtg_dh_total_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd4dtg_DH_TOTAL_reg);
	osddtg_dv_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd4dtg_DV_DEN_Start_End_reg);
	osddtg_dh_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd4dtg_DH_DEN_Start_End_reg);

	osddtg_control_rbus.regValue = OSD_RTD_INL( PPOVERLAY_OSD4DTG_CONTROL_reg );

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, " dtg h:%u(last:%u) v:%u  hDen:[%u %u](%u) vDen:[%u %u](%u)\n", 
		osddtg_dh_total_rbus.osd4dtg_dh_total, osddtg_dh_total_rbus.osd4dtg_dh_total_last_line, 
		osddtg_dv_total_rbus.osd4dtg_dv_total,  
		osddtg_dh_den_start_end_rbus.osd4dtg_dh_den_sta, osddtg_dh_den_start_end_rbus.osd4dtg_dh_den_end, osddtg_dh_den_start_end_rbus.osd4dtg_dh_den_end - osddtg_dh_den_start_end_rbus.osd4dtg_dh_den_sta,
		osddtg_dv_den_start_end_rbus.osd4dtg_dv_den_sta, osddtg_dv_den_start_end_rbus.osd4dtg_dv_den_end, osddtg_dv_den_start_end_rbus.osd4dtg_dv_den_end - osddtg_dv_den_start_end_rbus.osd4dtg_dv_den_sta
	);

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, " dtgCtrl en:%u split:%u fsyncEn:%u by_hw:%u fST:%u cnt_sync:%u resetEn:%u sel:%u mode:%u\n", 
	  osddtg_control_rbus.osd4dtg_en, osddtg_control_rbus.osd4_split, 
	  osddtg_control_rbus.osd4dtg_fsync_en, osddtg_control_rbus.osd4dtg_frc2fsync_by_hw, osddtg_control_rbus.osd4dtg_frc_fsync_status,
	  osddtg_control_rbus.osd4dtg_line_cnt_sync, osddtg_control_rbus.osd4dtg_frame_cnt_reset_en, osddtg_control_rbus.osd4dtg_fsync_select,
      osddtg_control_rbus.osd4dtg_mode_revised
	);
  }

  

  { // dump_GDMA_Status
	gdma4_osd4_size_RBUS  osd4_size_rbus;
	osdovl4_mixer4_ctrl2_RBUS mixer4_ctrl2_rbus;
	gdma4_osd4_ctrl_RBUS osd4_ctrl_rbus;

	osd4_size_rbus.regValue = OSD_RTD_INL( GDMA4_OSD4_SIZE_reg );

	osd4_ctrl_rbus.regValue = OSD_RTD_INL( GDMA4_OSD4_CTRL_reg );

			
	mixer4_ctrl2_rbus.regValue = OSD_RTD_INL( OSDOVL4_Mixer4_CTRL2_reg );

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL," osd size %d %d\n", osd4_size_rbus.w, osd4_size_rbus.h );

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL," Ctrl en:%d rot:%d d3:%d\n", 
        osd4_ctrl_rbus.osd4_en, osd4_ctrl_rbus.rotate, osd4_ctrl_rbus.d3_mode
		);

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL," mixer on:%d o4_on:%d o4V_on:%d vAlp:0x%x vSrcSe;:%d forceNoO:%d zoneType:%d zoneEn:%d\n",
        mixer4_ctrl2_rbus.mixer_en, mixer4_ctrl2_rbus.mixero4_en, mixer4_ctrl2_rbus.video_en, 
    	mixer4_ctrl2_rbus.video_alpha, mixer4_ctrl2_rbus.video_src_sel, mixer4_ctrl2_rbus.force_bypass_osd_blend,
		mixer4_ctrl2_rbus.measure_osd_zone_type, mixer4_ctrl2_rbus.measure_osd_zone_en
	);

	GDMAEXT_DUMP(" mixer4 setErr:0x%x", OSD_RTD_INL( OSDOVL4_Mixer4_setting_error_reg)  );
	

  }

  {
		//gdma4_osd4_status_RBUS  osd_status_rbus;
		// OSD_INTST

	gdma4_osd_intst_RBUS  osd_intst_rbus;
	gdma4_osd_inten_RBUS int_en_rbus;

	gdma4_dma_inten_RBUS dma_intEn_rbus;
	gdma4_dma_intst_RBUS dma_intst_rbus;
	

	int_en_rbus.regValue = rtd_inl(  GDMA4_OSD_INTEN_reg );
	osd_intst_rbus.regValue = OSD_RTD_INL( GDMA4_OSD_INTST_reg  );

	dma_intEn_rbus.regValue = OSD_RTD_INL(GDMA4_DMA_INTEN_reg);
	dma_intst_rbus.regValue = OSD_RTD_INL(GDMA4_DMA_INTST_reg);

	

	GDMAEXT_DUMP("\tosd intST vsync:%d vend:%d vfin:%d ov:%d %d\n", osd_intst_rbus.osd4_vsync, osd_intst_rbus.osd4_vact_end
		, osd_intst_rbus.osd4_fin, osd_intst_rbus.osd4_ov_range, osd_intst_rbus.afbc4_ov_range );
	GDMAEXT_DUMP("\tdma intST under:%d ov:%d acc:%d tfbcAcc:%d\n", dma_intst_rbus.osd4_udfl, dma_intst_rbus.osd4_ovfl
		, dma_intst_rbus.osd4_acc_sync, dma_intst_rbus.afbc4_acc_sync);

	GDMAEXT_DUMP("\tosd intrEn vsync:%d vend:%d vfin:%d ov:%d %d\n", int_en_rbus.osd4_vsync, int_en_rbus.osd4_vact_end
		, int_en_rbus.osd4_fin, int_en_rbus.osd4_ov_range, int_en_rbus.afbc4_ov_range );
	GDMAEXT_DUMP("\tdma intrEn under:%d ov:%d acc:%d tfbcAcc:%d\n", dma_intEn_rbus.osd4_udfl, dma_intEn_rbus.osd4_ovfl
		, dma_intEn_rbus.osd4_acc_sync, dma_intEn_rbus.afbc4_acc_sync);


		gdmaExt_osd_winfo_dump( GDMA_PLANE_OSD4 );
  }

	


	return 0;
}


int GDMA_OSD4_DevCB_DumpD2(GDMA_DISPLAY_PLANE plane, void *info)
{
	int err = 0;
	ppoverlay_display_timing_ctrl4_RBUS timing_ctrl4_rbus;

	timing_ctrl4_rbus.regValue = OSD_RTD_INL( PPOVERLAY_Display_Timing_CTRL4_reg );

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, "timing_ctrl4 val:0x%x post_in_sel:%d pre_to_osd_in_sel:%d pre_to_osd4_in_sel:%d dual_den_combine:%d \
		dual_dsce_en:%d dual_display_mode:%d\n", timing_ctrl4_rbus.regValue, 
		timing_ctrl4_rbus.d2post_in_sel, timing_ctrl4_rbus.d2pre_to_osd_in_sel, timing_ctrl4_rbus.d2pre_to_osd4_in_sel, 
		timing_ctrl4_rbus.dual_display_den_combine, timing_ctrl4_rbus.dual_display_dsce_en, timing_ctrl4_rbus.dual_display_mode 
	);



	return err;
}

int GDMA_OSD4_DevCB_SetD2(GDMA_DISPLAY_PLANE plane, void *info)
{
	int err = 0;

		GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL,"in %s %d\n", __FUNCTION__, __LINE__ );



	return err;
}

int GDMA_OSD4_DevCB_ConfigVideoMixerEnable(bool flag)
{
	osdovl4_mixer4_ctrl2_RBUS mixer4_ctrl2;
	osdovl4_osd4_db_ctrl_RBUS osd4_db_ctrl_reg;
	int err = 0;

	mixer4_ctrl2.regValue = OSD_RTD_INL(OSDOVL4_Mixer4_CTRL2_reg);
	mixer4_ctrl2.video_en = flag;
	osd4_db_ctrl_reg.regValue = OSD_RTD_INL(OSDOVL4_OSD4_Db_Ctrl_reg);
	osd4_db_ctrl_reg.db_en = 1;
	osd4_db_ctrl_reg.db_load = 1;

	OSD_RTD_OUTL(OSDOVL4_Mixer4_CTRL2_reg, mixer4_ctrl2.regValue);
	
	OSD_RTD_OUTL(OSDOVL4_OSD4_Db_Ctrl_reg, osd4_db_ctrl_reg.regValue); 
	
	return err;
}


int GDMA_OSD4_DevCB_ConfigVideoColorRange (CONFIG_VIDEO_COLORKEY_PARA video_color)
{
	int err = 0;
	osdovl4_mixer4_video_color_range_r_RBUS osdovl4_mixer4_video_color_range_r_reg;
	osdovl4_mixer4_video_color_range_g_RBUS osdovl4_mixer4_video_color_range_g_reg;
	osdovl4_mixer4_video_color_range_b_RBUS osdovl4_mixer4_video_color_range_b_reg;
	osdovl4_mixer4_video_color_replace_rg_RBUS osdovl4_mixer4_video_color_replace_rg_reg;
	osdovl4_mixer4_video_color_replace_b_RBUS osdovl4_mixer4_video_color_replace_b_reg;
	osdovl4_osd4_db_ctrl_RBUS osd4_db_ctrl_reg;

	osdovl4_mixer4_video_color_range_r_reg.regValue = OSD_RTD_INL(OSDOVL4_Mixer4_video_color_range_r_reg);
	osdovl4_mixer4_video_color_range_g_reg.regValue = OSD_RTD_INL(OSDOVL4_Mixer4_video_color_range_g_reg);
	osdovl4_mixer4_video_color_range_b_reg.regValue = OSD_RTD_INL(OSDOVL4_Mixer4_video_color_range_b_reg);
	osdovl4_mixer4_video_color_replace_rg_reg.regValue = OSD_RTD_INL(OSDOVL4_Mixer4_video_color_replace_rg_reg);
	osdovl4_mixer4_video_color_replace_b_reg.regValue = OSD_RTD_INL(OSDOVL4_Mixer4_video_color_replace_b_reg);

	osdovl4_mixer4_video_color_range_r_reg.color_range_replace_mode = video_color.replace_mode;
	osdovl4_mixer4_video_color_range_r_reg.color_key_r_max = video_color.color_key_r_max;
	osdovl4_mixer4_video_color_range_r_reg.color_key_r_min = video_color.color_key_r_min;
	osdovl4_mixer4_video_color_range_g_reg.color_key_g_max = video_color.color_key_g_max;
	osdovl4_mixer4_video_color_range_g_reg.color_key_g_min = video_color.color_key_g_min;
	osdovl4_mixer4_video_color_range_b_reg.color_key_b_max = video_color.color_key_b_max;
	osdovl4_mixer4_video_color_range_b_reg.color_key_b_min = video_color.color_key_b_min;
	osdovl4_mixer4_video_color_replace_rg_reg.color_replace_r = video_color.color_replace_r;
	osdovl4_mixer4_video_color_replace_rg_reg.color_replace_g = video_color.color_replace_g;
	osdovl4_mixer4_video_color_replace_b_reg.color_replace_b = video_color.color_replace_b;

	
	OSD_RTD_OUTL(OSDOVL4_Mixer4_video_color_range_r_reg,osdovl4_mixer4_video_color_range_r_reg.regValue);	
	OSD_RTD_OUTL(OSDOVL4_Mixer4_video_color_range_g_reg,osdovl4_mixer4_video_color_range_g_reg.regValue);
	OSD_RTD_OUTL(OSDOVL4_Mixer4_video_color_range_b_reg,osdovl4_mixer4_video_color_range_b_reg.regValue);
	OSD_RTD_OUTL(OSDOVL4_Mixer4_video_color_replace_rg_reg,osdovl4_mixer4_video_color_replace_rg_reg.regValue);
	OSD_RTD_OUTL(OSDOVL4_Mixer4_video_color_replace_b_reg,osdovl4_mixer4_video_color_replace_b_reg.regValue);
	
	osd4_db_ctrl_reg.regValue = OSD_RTD_INL(OSDOVL4_OSD4_Db_Ctrl_reg);	
	osd4_db_ctrl_reg.db_en = 1;
	osd4_db_ctrl_reg.db_load = 1;
	OSD_RTD_OUTL(OSDOVL4_OSD4_Db_Ctrl_reg, osd4_db_ctrl_reg.regValue); 

	return err;
}




int GDMAExt_init_register_callback(void)
{
	#ifdef RTK_GDMA_TEST_OSD1
	g_osd_cb[GDMA_PLANE_OSD1].init = GDMA_OSD1_DevCB_init;
	g_osd_cb[GDMA_PLANE_OSD1].deInit = NULL;
	g_osd_cb[GDMA_PLANE_OSD4].initSR = GDMA_OSD1_DevCB_initSR;
	g_osd_cb[GDMA_PLANE_OSD1].setGDMA = GDMA_OSD1_DevCB_SetGDMA;
	g_osd_cb[GDMA_PLANE_OSD1].setSR = NULL;
	g_osd_cb[GDMA_PLANE_OSD1].setMixer = NULL;
	g_osd_cb[GDMA_PLANE_OSD1].setMixerDone = GDMA_OSD1_DevCB_setMixerDone;
	g_osd_cb[GDMA_PLANE_OSD1].set_ProgDone = GDMA_OSD1_DevCB_set_ProgDone;
	g_osd_cb[GDMA_PLANE_OSD1].get_isrStatus = NULL;
	g_osd_cb[GDMA_PLANE_OSD1].dumpStatus = GDMA_OSD1_DevCB_DumpStatus;
	
    g_osd_cb[GDMA_PLANE_OSD2].init = GDMA_OSD1_DevCB_init;
	g_osd_cb[GDMA_PLANE_OSD2].deInit = NULL;
	g_osd_cb[GDMA_PLANE_OSD2].setGDMA = GDMA_OSD1_DevCB_SetGDMA;
	g_osd_cb[GDMA_PLANE_OSD2].setSR = NULL;
	g_osd_cb[GDMA_PLANE_OSD2].setMixer = GDMA_OSD1_DevCB_setMixerDone;
	g_osd_cb[GDMA_PLANE_OSD2].set_ProgDone = GDMA_OSD1_DevCB_set_ProgDone;
	g_osd_cb[GDMA_PLANE_OSD2].get_isrStatus = NULL;

	//g_osd_cb[GDMA_PLANE_OSD3].

	#endif//RTK_GDMA_TEST_OSD1

	#ifdef RTK_ENABLE_GDMA_EXT
	g_osd_cb[GDMA_PLANE_OSD4].init = GDMA_OSD4_DevCB_init;
	g_osd_cb[GDMA_PLANE_OSD4].deInit = NULL;
	g_osd_cb[GDMA_PLANE_OSD4].initSR = GDMA_OSD4_DevCB_initSR ;

	g_osd_cb[GDMA_PLANE_OSD4].presetOSD = GDMA_OSD4_DevCB_set_PresetOSD;
	g_osd_cb[GDMA_PLANE_OSD4].setGDMA = GDMA_OSD4_DevCB_SetGDMA;
	g_osd_cb[GDMA_PLANE_OSD4].setSR = NULL;
	g_osd_cb[GDMA_PLANE_OSD4].setMixer = NULL;
	g_osd_cb[GDMA_PLANE_OSD4].set_ProgDone = GDMA_OSD4_DevCB_set_ProgDone;
	g_osd_cb[GDMA_PLANE_OSD4].get_isrStatus = NULL;
	g_osd_cb[GDMA_PLANE_OSD4].dumpStatus = GDMA_OSD4_DevCB_DumpStatus;
	g_osd_cb[GDMA_PLANE_OSD4].setD2 = GDMA_OSD4_DevCB_SetD2;
	g_osd_cb[GDMA_PLANE_OSD4].dumpD2 = GDMA_OSD4_DevCB_DumpD2;

	g_osd_cb[GDMA_PLANE_OSD4].setMixerDone = GDMA_OSD4_DevCB_setMixerDone;
	
	
    g_osd_cb[GDMA_PLANE_OSD5].init = GDMA_OSD5_DevCB_init;
	g_osd_cb[GDMA_PLANE_OSD5].deInit = NULL;
	g_osd_cb[GDMA_PLANE_OSD5].initSR = GDMA_OSD5_DevCB_initSR ;

	g_osd_cb[GDMA_PLANE_OSD5].presetOSD = GDMA_OSD5_DevCB_set_PresetOSD;
	g_osd_cb[GDMA_PLANE_OSD5].setGDMA = GDMA_OSD5_DevCB_SetGDMA;
		g_osd_cb[GDMA_PLANE_OSD5].setSR = NULL;
		g_osd_cb[GDMA_PLANE_OSD5].setMixer = NULL;
	g_osd_cb[GDMA_PLANE_OSD5].set_ProgDone = GDMA_OSD5_DevCB_set_ProgDone;
	g_osd_cb[GDMA_PLANE_OSD5].get_isrStatus = NULL;
	g_osd_cb[GDMA_PLANE_OSD5].dumpStatus = GDMA_OSD5_DevCB_DumpStatus;

	g_osd_cb[GDMA_PLANE_OSD5].setMixerDone = GDMA_OSD5_DevCB_setMixerDone;

	#endif//RTK_ENABLE_GDMA_EXT



	return 0;
}


