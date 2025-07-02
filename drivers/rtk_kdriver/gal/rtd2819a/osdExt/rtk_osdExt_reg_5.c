#include <rbus/ppoverlay_reg.h>   // dtg
#include <rbus/gdma4_reg.h>
#include <rbus/osdovl4_reg.h>

#include <rbus/gdma5_reg.h>
#include <rbus/osdovl5_reg.h>
#include <rbus/osdtg45_reg.h>
#include <rbus/dc2h2_cap_reg.h>
#include <rbus/dc2h2_scaledown_reg.h>

#include <rbus/dc2h2_cap_reg.h> 

#include <rtk_crt.h>
#include <gal/rtk_gdma_driver.h>
#include "rtk_osdExt_main.h" 
#include "rtk_osdExt_driver.h"
#include "rtk_osdExt_debug.h"
#include "rtk_osdExt_sr.h"



extern gdma_dev *gdma_devices;
extern unsigned long gdma_total_ddr_size;


// for non standard test flow
// ASSUME no runtime change
void GDMA_OSD5_preFrame_Set( void )
{
	ppoverlay_osd5dtg_dv_den_start_end_RBUS osddtg_dv_den_start_end_rbus;
	ppoverlay_osd5dtg_dh_den_start_end_RBUS osddtg_dh_den_start_end_rbus;
	
	osdtg45_osd5_precrop_dv_den_RBUS o5_precrop_dv_den_rbus;
	osdtg45_osd5_precrop_dh_den_RBUS o5_precrop_dh_den_rbus;


	osddtg_dv_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd5dtg_DV_DEN_Start_End_reg);
	osddtg_dh_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd5dtg_DH_DEN_Start_End_reg);


	o5_precrop_dv_den_rbus.regValue = OSD_RTD_INL( OSDTG45_osd5_precrop_dv_den_reg );
	o5_precrop_dh_den_rbus.regValue = OSD_RTD_INL( OSDTG45_osd5_precrop_dh_den_reg );

	if( o5_precrop_dv_den_rbus.osd5_precrop_dv_den_sta != osddtg_dv_den_start_end_rbus.osd5dtg_dv_den_sta
		|| o5_precrop_dv_den_rbus.osd5_precrop_dv_den_end != osddtg_dv_den_start_end_rbus.osd5dtg_dv_den_end
		|| o5_precrop_dh_den_rbus.osd5_precrop_dh_den_sta != osddtg_dh_den_start_end_rbus.osd5dtg_dh_den_sta
		|| o5_precrop_dh_den_rbus.osd5_precrop_dh_den_end != osddtg_dh_den_start_end_rbus.osd5dtg_dh_den_end
	 )
	{
	
		o5_precrop_dv_den_rbus.osd5_precrop_den_en = 0; //always 0 
		o5_precrop_dv_den_rbus.osd5_precrop_dv_den_sta = osddtg_dv_den_start_end_rbus.osd5dtg_dv_den_sta;
		o5_precrop_dv_den_rbus.osd5_precrop_dv_den_end = osddtg_dv_den_start_end_rbus.osd5dtg_dv_den_end;


		o5_precrop_dh_den_rbus.osd5_precrop_dh_den_sta = osddtg_dh_den_start_end_rbus.osd5dtg_dh_den_sta;
		o5_precrop_dh_den_rbus.osd5_precrop_dh_den_end = osddtg_dh_den_start_end_rbus.osd5dtg_dh_den_end;

		OSD_RTD_OUTL( OSDTG45_osd5_precrop_dv_den_reg, o5_precrop_dv_den_rbus.regValue );
		OSD_RTD_OUTL( OSDTG45_osd5_precrop_dh_den_reg, o5_precrop_dh_den_rbus.regValue );

		GDMAEXT_DUMP("osd5 set precrop:0x%x 0x%x",o5_precrop_dh_den_rbus.regValue, o5_precrop_dv_den_rbus.regValue);
	}

}


void GDMA_OSD5_preFrame_SetPrecrop( int width, int height )
{
	osdtg45_osd5_precrop_dv_den_RBUS o5_precrop_dv_den_rbus;
	osdtg45_osd5_precrop_dh_den_RBUS o5_precrop_dh_den_rbus;

	o5_precrop_dv_den_rbus.osd5_precrop_den_en = 0;
	o5_precrop_dv_den_rbus.osd5_precrop_dv_den_sta = 0 ;
	o5_precrop_dv_den_rbus.osd5_precrop_dv_den_end =  height;

	o5_precrop_dh_den_rbus.osd5_precrop_dh_den_sta = 0;
	o5_precrop_dh_den_rbus.osd5_precrop_dh_den_end = width;
	
	OSD_RTD_OUTL( OSDTG45_osd5_precrop_dv_den_reg, o5_precrop_dv_den_rbus.regValue );
	OSD_RTD_OUTL( OSDTG45_osd5_precrop_dh_den_reg, o5_precrop_dh_den_rbus.regValue );

	GDMAEXT_DUMP("osd5 set2 precrop:0x%x 0x%x [%d %d]",o5_precrop_dh_den_rbus.regValue, o5_precrop_dv_den_rbus.regValue,
		o5_precrop_dh_den_rbus.osd5_precrop_dh_den_end, o5_precrop_dv_den_rbus.osd5_precrop_dv_den_end );



}

int GDMA_OSD5_DevCB_init(GDMA_DISPLAY_PLANE plane, void *info)
{
    //gdma_dev* gdma = (gdma_dev*) info ; 
//    gdma_dev* gdma = (gdma_dev*) info ; 

	
	osdovl5_mixer5_ctrl2_RBUS mixer_ctrl2;
	gdma5_o5_tfbdc_config_RBUS tfbdc_config;
	//osdovl5_mixer5_layer_sel_RBUS mixer_layer_sel_reg; //useless
	//sys_reg_sys_dispclksel_RBUS sys_dispclksel_reg;

	gdma5_osd5_RBUS GDMA_osd;

	//GDMA_Clk_Select(0, true);
	GDMAExt_ClkEnable(GDMA_PLANE_OSD5, 1);

	#if 0 //keep default
	{
	gdma5_dma_RBUS  gdma_dma_reg;
	gdma_dma_reg.regValue = OSD_RTD_INL(GDMA5_DMA_reg);
		gdma_dma_reg.afbc5_req_num = 0xF;  //default 16
		gdma_dma_reg.osd5_req_num = 0xF;	//default 7
	OSD_RTD_OUTL(GDMA5_DMA_reg, gdma_dma_reg.regValue);
	}
	#endif//

	GDMA_OSD5_preFrame_Set();


	GDMA_osd.wi_endian = 0;
	GDMA_osd.osd = 1;
	GDMA_osd.write_data = 1;
	OSD_RTD_OUTL(GDMA5_OSD5_reg, GDMA_osd.regValue );

	/*tfbc group cannot dynamic change*/
	tfbdc_config.regValue = rtd_inl(GDMA5_O5_TFBDC_CONFIG_reg);
	tfbdc_config.tfbdc_lossy_group_control = 1;
	tfbdc_config.tfbdc_swizzle = 4;
	tfbdc_config.tfbdc_lossy_min_channel_override = 0;
	rtd_outl(GDMA5_O5_TFBDC_CONFIG_reg, tfbdc_config.regValue);
	

 	OSD_RTD_OUTL(GDMA5_OSD5_CTRL_reg, ~1);

	GDMAExt_config_line_buffer( GDMA_PLANE_OSD5, 0 );

	GDMA_OSD5_DevCB_set_ProgDone (GDMA_PLANE_OSD5, GMDA_OSD_DONE_MODE_SINGLE, NULL);


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

	GDMAExt_config_fbc( GDMA_PLANE_OSD5 );

	mixer_ctrl2.regValue = OSD_RTD_INL(OSDOVL5_Mixer5_CTRL2_reg);
	
#if OSD_DETECTION_SUPPORT
	mixer_ctrl2.measure_osd_zone_en = 1;
	mixer_ctrl2.measure_osd_zone_type = 0; //A=0, R=0, G=0, B=0
#endif
	mixer_ctrl2.mixer_en = 1;

   	OSD_RTD_OUTL(OSDOVL5_Mixer5_CTRL2_reg, mixer_ctrl2.regValue);

	/* wdma_bl set to 'd80 by HW RD suggestion */
	/*dma_bl_2_reg.regValue = rtd_inl(GDMA_DMA_BL_2_reg);
		dma_bl_2_reg.wdma_bl = 80;
		OSD_RTD_OUTL(GDMA_DMA_BL_2_reg, dma_bl_2_reg.regValue);
	*/

	#if 0 //no layer sel
	mixer_layer_sel_reg.regValue = OSD_RTD_INL(OSDOVL5_Mixer5_layer_sel_reg);
	mixer_layer_sel_reg.c0_sel = 0; //video 
	mixer_layer_sel_reg.c1_sel = 1;  //osd4
	OSD_RTD_OUTL(OSDOVL5_Mixer5_layer_sel_reg, mixer_layer_sel_reg.regValue);
	#endif//


	GDMAExt_config_mid_blend();  // null currently

	GDMA_OSD5_DevCB_set_ProgDone (GDMA_PLANE_OSD5, GMDA_OSD_DONE_MODE_SINGLE, NULL);


	{
		gdma_total_ddr_size = 0x60000000;
		gdma_total_ddr_size = get_memory_size_total() - 0x100;

		GDMA_EXT_LOG(GDMA_EXT_LOG_DEBUG, "[osd4] get total memsize = %x\n", gdma_total_ddr_size);

		if (gdma_total_ddr_size) {
			if (gdma_total_ddr_size) {
			OSD_RTD_OUTL(GDMA5_AFBC5_MIN_reg, 0x0);
			OSD_RTD_OUTL(GDMA5_AFBC5_MAX_reg, gdma_total_ddr_size);
			OSD_RTD_OUTL(GDMA5_OSD5_MIN_reg, 0x0);
			OSD_RTD_OUTL(GDMA5_OSD5_MAX_reg, gdma_total_ddr_size-0x1000);
		} else {
			GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s gdma_total_ddr_size is empty!\n", __func__);
		}
		} else {
			GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s gdma_total_ddr_size is empty!\n", __func__);
		}
	}


	return 0;
}
int GDMA_OSD5_DevCB_preset_GDMA( GDMA_PIC_DATA *curPic, struct gdma_receive_work *ptr_work)
{
	GDMA_WIN *win = NULL ;
	gdma_dev *gdma = &gdma_devices[0];

	GDMA_DISPLAY_PLANE disPlane = ptr_work->disPlane;
	GDMA_5_REG_CONTENT *pReg = &curPic->reg_5_content;

	int rotateBit = gdma->ctrl.enableVFlip;
	int canvasW, canvasH;

	
	win = &curPic->OSDwin;

	canvasW = win->winWH.width;
	canvasH = win->winWH.height;

		/*  set clear region */
		if (curPic->clear_x.value != 0 || curPic->clear_y.value != 0) {
			pReg->osd_clear1_reg.regValue = curPic->clear_x.value;
			pReg->osd_clear2_reg.regValue = curPic->clear_y.value;
			pReg->osd_reg.regValue = GDMA5_OSD5_clear_region_en(1) | GDMA5_OSD5_write_data(1);
		} else {
			pReg->osd_reg.regValue = GDMA5_OSD5_clear_region_en(1) | GDMA5_OSD5_write_data(0);
		}

		if (gdma->ctrl.osdfirstEn[disPlane] == 0) {

			pReg->osd_ctrl_reg.regValue = GDMA5_OSD5_CTRL_write_data(1) | GDMA5_OSD5_CTRL_osd5_en(curPic->show) 
										| GDMA5_OSD5_CTRL_rotate(rotateBit);

			gdma->ctrl.osdEn[disPlane] = 1;
			gdma->ctrl.osdfirstEn[disPlane] = 1;
			
			GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s, win=0x%lx, phy=0x%x\n", __FUNCTION__, win, virt_to_phys(win));
		} else {
			/*  if osdEn changes to 0, driver will force curPic->show to zero and disable OSD */
			if (gdma->ctrl.osdEn[disPlane] == 0)
				curPic->show = 0;

			pReg->osd_ctrl_reg.regValue = GDMA5_OSD5_CTRL_write_data(1) | GDMA4_OSD4_CTRL_osd4_en(curPic->show) | GDMA5_OSD5_CTRL_rotate(rotateBit);
		}

		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, " curPic->show=%d, gdma->ctrl.osdEn[%d]=%d \n",  curPic->show, disPlane, gdma->ctrl.osdEn[disPlane]);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," osd%d  win=0x%lx, top=0x%x gdma->f_inputsrc_4k2k=%d \n",  disPlane, win, win->top_addr, gdma->f_inputsrc_4k2k);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," input src[x,y,w,h]= [%d,%d,%d,%d] \n",  win->winXY.x, win->winXY.y, canvasW, canvasH);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," output[x,y,w,h]= [%d,%d,%d,%d] \n",  win->dst_x, win->dst_y, win->dst_width ,win->dst_height);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," plane_alpha_r=0x%x, plane_alpha_a=0x%x \n",  curPic->plane_ar.plane_alpha_r, curPic->plane_ar.plane_alpha_a);
		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE," plane_alpha_g=0x%x, plane_alpha_b=0x%x \n",  curPic->plane_gb.plane_alpha_g, curPic->plane_gb.plane_alpha_b);

		pReg->osd_wi_reg.regValue = GDMA5_OSD5_WI_addr (virt_to_phys(win));
		
        if (pReg->osd_wi_reg.regValue == 0) {
            #if defined(CONFIG_ARM64)
                GDMA_ERR( "[GDMA5] addr = 0!!!!!!!! win = 0x%lx gIdxQwr = %d pitch = 0x%x\n", (unsigned long)win, ptr_work->picQwr, win->pitch);
            #else
                GDMA_ERR( "[GDMA5] addr = 0!!!!!!!! win = 0x%x gIdxQwr = %d pitch = 0x%x\n", (unsigned int)win, ptr_work->picQwr, win->pitch);
            #endif
        } else if (pReg->osd_wi_reg.regValue > gdma_total_ddr_size) {
        
            #if defined(CONFIG_ARM64)
                GDMA_ERR( "[GDMA5] addr = 0x%lx, win info addr over range!!\n", (unsigned long)win);
            #else
                GDMA_ERR( "[GDMA5] addr = 0x%x, win info addr over range!!\n", (unsigned int)win);
            #endif
        }
		//pReg->osd_size_reg.regValue = GDMA_OSD1_SIZE_w(canvasW) | GDMA_OSD1_SIZE_h(canvasH);
		pReg->osd_size_reg.regValue = GDMA5_OSD5_SIZE_w(canvasW) | GDMA5_OSD5_SIZE_h(canvasH);

	if( curPic->pqdc_enable )
	{
		//

	}

	return 0;
}

// GDMAExt_PreOSDReg_Set_Mixer ( GDMA_PIC_DATA *curPic)
int GDMA_OSD5_DevCB_preset_Mixer( GDMA_PIC_DATA *curPic )
{
	GDMA_5_REG_CONTENT* pReg = &curPic->reg_5_content;
	

	// check to set plane alpha  
	// C0 only 
	
	//pReg->mixer_layer_sel_reg.c0_sel = 1; //fixed to 1

		if ( (curPic->plane_ar.plane_alpha_r != 0xff) || (curPic->plane_ar.plane_alpha_a != 0xff) 
			|| 	(curPic->plane_gb.plane_alpha_g != 0xff) || (curPic->plane_gb.plane_alpha_b != 0xff) 
		) {
			pReg->mixer_layer_sel_reg.c0_plane_alpha_en = 1;
		}

	pReg->mixer_c0_plane_alpha1_reg.regValue = curPic->plane_ar.value;
	pReg->mixer_c0_plane_alpha2_reg.regValue = curPic->plane_gb.value;

	
	// . a_det_osd4_en  ; //enable alpha detect

	return 0;
}

/*

  GDMAExt_PreOSDReg_Set_GDMA(curPic, ptr_work);
  GDMAExt_PreOSDReg_Set_SR( 1, &srcWin, &dispWin, curPic);
  GDMAExt_PreOSDReg_Set_Mixer ( curPic );

*/
int GDMA_OSD5_DevCB_set_PresetOSD(GDMA_DISPLAY_PLANE plane, GDMA_PIC_DATA *curPic, VO_RECTANGLE* srcWin, VO_RECTANGLE* dispWin,
                                 struct gdma_receive_work *ptr_work)
{
	int err = 0;

	err = GDMA_OSD5_DevCB_preset_GDMA(curPic, ptr_work);
	
	err = GDMA_OSD5_DevCB_preset_SR( 1, *srcWin, *dispWin, curPic);

	err = GDMA_OSD5_DevCB_preset_Mixer( curPic );


	return err;
}

int GDMA_OSD5_DevCB_setMixerDone(GDMA_DISPLAY_PLANE disPlane, int waitFinish, void *info )
{
	osdovl5_osd5_db_ctrl_RBUS osd_db_ctrl_reg;
	int ltimeout =0xff;

	osd_db_ctrl_reg.regValue = rtd_inl(OSDOVL5_OSD5_Db_Ctrl_reg);
	osd_db_ctrl_reg.db_load = 1;

	OSD_RTD_OUTL(OSDOVL5_OSD5_Db_Ctrl_reg, osd_db_ctrl_reg.regValue); /*  mixer apply */

	osd_db_ctrl_reg.regValue = rtd_inl(OSDOVL5_OSD5_Db_Ctrl_reg);

	if ( waitFinish == 1) {

		while(osd_db_ctrl_reg.db_load && ltimeout--) {
		    osd_db_ctrl_reg.regValue = rtd_inl(OSDOVL5_OSD5_Db_Ctrl_reg);
		    mdelay(1);
		}

	}

	GDMAEXT_DUMP("%s mixer4 :0x%x", __FUNCTION__, OSD_RTD_INL( OSDOVL5_Mixer5_CTRL2_reg)  );
	
	return 0;
}


int GDMA_OSD5_DevCB_set_ProgDone(GDMA_DISPLAY_PLANE disPlane, GMDA_OSD_DONE_MODE mode, void *info)
{
	unsigned int onlineProgDone = GDMA5_CTRL5_write_data_mask;

	if( mode == GMDA_OSD_DONE_MODE_SYNC45 ) {
		//osd4's progDone callback don't outl in this mode. controled here after osd 5 flow done.
		
		onlineProgDone |= GDMA4_CTRL4_o4o5_prog_done(1);

		OSD_RTD_OUTL(GDMA4_CTRL4_reg, onlineProgDone);
	}
	else {

		onlineProgDone |= GDMA5_CTRL5_osd5_prog_done(1);
		OSD_RTD_OUTL(GDMA5_CTRL5_reg, onlineProgDone);
	}

	// no o4o5_prog_done(1)

	



	return 0;
}



int GDMA_OSD5_DevCB_SetGDMA(GDMA_DISPLAY_PLANE disPlane, void *info)
{

	//MUST_FIXME  use osd 4 reg 

	gdma_dev* gdma = (gdma_dev*) info ;
	
	GDMA_PIC_DATA *curPic;
	


	GDMA_5_REG_CONTENT *pReg = NULL;  //GDMA_REG_CONTENT


	curPic = gdma->pic[disPlane] + gdma->curPic[disPlane];
	//pReg = &curPic->reg_content;
	pReg = &curPic->reg_5_content;

	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s line=%d disPlane=%d, workqueueIdx=%d \n", __FUNCTION__, __LINE__, disPlane, curPic->workqueueIdx);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE,"%s gdma->curPic[%d]=%d, curPic->onlineOrder=%d\n", __FUNCTION__, disPlane, gdma->curPic[disPlane], curPic->onlineOrder);

	#if 0
	if (disPlane == GDMA_PLANE_OSD2)
		offset = 0x100;
	else if (disPlane == GDMA_PLANE_OSD3)
		offset = 0x200;
	#endif//

	GDMA_OSD5_preFrame_Set();

	// start set registers 

	/*  set clear region */
	OSD_RTD_OUTL(GDMA5_OSD5_CLEAR1_reg, pReg->osd_clear1_reg.regValue);
	OSD_RTD_OUTL(GDMA5_OSD5_CLEAR2_reg, pReg->osd_clear2_reg.regValue);
	OSD_RTD_OUTL(GDMA5_OSD5_reg, pReg->osd_reg.regValue);

	OSD_RTD_OUTL(GDMA5_OSD5_CTRL_reg , ~1);  // 0xfffffffe
	OSD_RTD_OUTL(GDMA5_OSD5_CTRL_reg , pReg->osd_ctrl_reg.regValue);

	OSD_RTD_OUTL(GDMA5_OSD5_WI_reg , pReg->osd_wi_reg.regValue);
	OSD_RTD_OUTL(GDMA5_OSD5_SIZE_reg , pReg->osd_size_reg.regValue);

	if( curPic->pqdc_enable ) {
		//[TODO] enable/disalbe only once ? (need recode prev_status in this Pic ?)
		OSD_RTD_OUTL (GDMA5_OSD5_PQ_DECMP_reg, pReg->pq_decmp_reg.regValue);
		OSD_RTD_OUTL (GDMA5_OSD5_PQ_DECMP_PAIR_reg, pReg->pq_decmp_pair_reg.regValue);
	}



	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_CLEAR1_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_clear1_reg.regValue);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_CLEAR2_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_clear2_reg.regValue);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_reg.regValue);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_CTRL_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_ctrl_reg.regValue);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_OSD%d_WI_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_wi_reg.regValue);
	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE,, "%s GDMA_OSD%d_SIZE_reg=0x%x \n", __FUNCTION__, disPlane, pReg->osd_size_reg.regValue);
	//GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s GDMA_line_buffer_end_reg=0x%x \n", __FUNCTION__, pReg->lb_end_reg.regValue);


	
	GDMA_OSD5_DevCB_set_SR(1, disPlane, curPic);




	return 0;
}

int GDMA_OSD5_DevCB_DumpStatus(GDMA_DISPLAY_PLANE plane, int level)
{
	(void)plane;
	(void)level;


  {	//DTG
	ppoverlay_osd5dtg_dv_total_RBUS osddtg_dv_total_rbus;
	ppoverlay_osd5dtg_dh_total_RBUS osddtg_dh_total_rbus;
	ppoverlay_osd5dtg_dv_den_start_end_RBUS osddtg_dv_den_start_end_rbus;
	ppoverlay_osd5dtg_dh_den_start_end_RBUS osddtg_dh_den_start_end_rbus;
	ppoverlay_osd5dtg_control_RBUS osddtg_control_rbus;

	osdtg45_osd5_precrop_dv_den_RBUS  osd5_precrop_dv_den_rbus;
	osdtg45_osd5_precrop_dh_den_RBUS precrop_dh_den_rbus;


	osddtg_dv_total_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd5dtg_DV_TOTAL_reg);
	osddtg_dh_total_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd5dtg_DH_TOTAL_reg);
	osddtg_dv_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd5dtg_DV_DEN_Start_End_reg);
	osddtg_dh_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_osd5dtg_DH_DEN_Start_End_reg);

	osddtg_control_rbus.regValue = OSD_RTD_INL( PPOVERLAY_OSD5DTG_CONTROL_reg );

	GDMA_EXT_LOG(GDMA_EXT_LOG_DUMP, "\n\n\n=====+ OSD5 +=====");

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, " dtg h:%u(last:%u) v:%u hDen:[%u %u](%u) vDen:[%u %u](%u)\n", 
		osddtg_dh_total_rbus.osd5dtg_dh_total, osddtg_dh_total_rbus.osd5dtg_dh_total_last_line, 
		osddtg_dv_total_rbus.osd5dtg_dv_total,  
		osddtg_dh_den_start_end_rbus.osd5dtg_dh_den_sta, osddtg_dh_den_start_end_rbus.osd5dtg_dh_den_end, osddtg_dh_den_start_end_rbus.osd5dtg_dh_den_end - osddtg_dh_den_start_end_rbus.osd5dtg_dh_den_sta,
		osddtg_dv_den_start_end_rbus.osd5dtg_dv_den_sta, osddtg_dv_den_start_end_rbus.osd5dtg_dv_den_end, osddtg_dv_den_start_end_rbus.osd5dtg_dv_den_end - osddtg_dv_den_start_end_rbus.osd5dtg_dv_den_sta
		);

	//osd5 no fsync_select
	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, "osd dtgCtrl en:%u split:%u fsyncEn:%u by_hw:%u fST:%u cnt_sync:%u resetEn:%u mode:%u\n", 
	  osddtg_control_rbus.osd5dtg_en, osddtg_control_rbus.osd5_split, 
	  osddtg_control_rbus.osd5dtg_fsync_en, osddtg_control_rbus.osd5dtg_frc2fsync_by_hw, osddtg_control_rbus.osd5dtg_frc_fsync_status,
	  osddtg_control_rbus.osd5dtg_line_cnt_sync, osddtg_control_rbus.osd5dtg_frame_cnt_reset_en, 
      osddtg_control_rbus.osd5dtg_mode_revised
	);


	osd5_precrop_dv_den_rbus.regValue = OSD_RTD_INL( OSDTG45_osd5_precrop_dv_den_reg );
	precrop_dh_den_rbus.regValue = OSD_RTD_INL( OSDTG45_osd5_precrop_dh_den_reg );

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, " precrop_dv_den en:%d(1:en for just/overScan)  H:[%d %d](%d) v:[%u %u](%u)\n",
		osd5_precrop_dv_den_rbus.osd5_precrop_den_en, precrop_dh_den_rbus.osd5_precrop_dh_den_sta, precrop_dh_den_rbus.osd5_precrop_dh_den_end
		, precrop_dh_den_rbus.osd5_precrop_dh_den_end - precrop_dh_den_rbus.osd5_precrop_dh_den_sta
		, osd5_precrop_dv_den_rbus.osd5_precrop_dv_den_sta,	osd5_precrop_dv_den_rbus.osd5_precrop_dv_den_end
		, osd5_precrop_dv_den_rbus.osd5_precrop_dv_den_end - osd5_precrop_dv_den_rbus.osd5_precrop_dv_den_sta
	);

  }


  {
	//d2post
	ppoverlay_d2post2dtg_dv_total_RBUS d2post2dtg_dv_total_rbus;
	ppoverlay_d2post2dtg_dh_total_RBUS d2post2dtg_dh_total_rbus;
	ppoverlay_d2post2dtg_dv_den_start_end_RBUS d2post2dtg_dv_den_start_end_rbus;
	ppoverlay_d2post2dtg_dh_den_start_end_RBUS d2post2dtg_dh_den_start_end_rbus;


	ppoverlay_d2post2dtg_control3_RBUS d2post2dtg_control3_rbus;
	ppoverlay_d2post2dtg_control4_RBUS d2post2dtg_control4_rbus;

	d2post2dtg_dv_total_rbus.regValue = OSD_RTD_INL(PPOVERLAY_d2post2dtg_DV_TOTAL_reg);
	d2post2dtg_dh_total_rbus.regValue = OSD_RTD_INL(PPOVERLAY_d2post2dtg_DH_TOTAL_reg);
	d2post2dtg_dv_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_d2post2dtg_DV_DEN_Start_End_reg);
	d2post2dtg_dh_den_start_end_rbus.regValue = OSD_RTD_INL(PPOVERLAY_d2post2dtg_DH_DEN_Start_End_reg);

		//osddtg_control_rbus.regValue = OSD_RTD_INL( PPOVERLAY_d2post2dtg_CONTROL_reg );

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, "d2post dtg h:%u(last:%u) v:%u hDen:[%u %u](%u) vDen:[%u %u](%u)\n", 
		d2post2dtg_dh_total_rbus.d2post2dtg_dh_total, d2post2dtg_dh_total_rbus.d2post2dtg_dh_total_last_line, 
		d2post2dtg_dv_total_rbus.d2post2dtg_dv_total,  
		d2post2dtg_dh_den_start_end_rbus.d2post2dtg_dh_den_sta, d2post2dtg_dh_den_start_end_rbus.d2post2dtg_dh_den_end, d2post2dtg_dh_den_start_end_rbus.d2post2dtg_dh_den_end - d2post2dtg_dh_den_start_end_rbus.d2post2dtg_dh_den_sta,
		d2post2dtg_dv_den_start_end_rbus.d2post2dtg_dv_den_sta, d2post2dtg_dv_den_start_end_rbus.d2post2dtg_dv_den_end, d2post2dtg_dv_den_start_end_rbus.d2post2dtg_dv_den_end - d2post2dtg_dv_den_start_end_rbus.d2post2dtg_dv_den_sta
		);

	d2post2dtg_control3_rbus.regValue = OSD_RTD_INL( PPOVERLAY_D2POST2DTG_CONTROL3_reg );
	d2post2dtg_control4_rbus.regValue = OSD_RTD_INL( PPOVERLAY_D2POST2DTG_CONTROL4_reg );

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, "d2ctrl3 dsce_regen_pixel:%d regen DH [%d %d](%d)\n", d2post2dtg_control3_rbus.dsce_regen_pixel, 
		d2post2dtg_control4_rbus.dsce_regen_dh_den_sta, d2post2dtg_control4_rbus.dsce_regen_dh_den_end,
		d2post2dtg_control4_rbus.dsce_regen_dh_den_end - d2post2dtg_control4_rbus.dsce_regen_dh_den_sta
	);

  }


  {
	gdma5_osd5_size_RBUS osd_size_rbus;
	osdovl5_mixer5_ctrl2_RBUS mixer5_ctrl2_rbus;
	gdma5_osd5_ctrl_RBUS osd_ctrl_rbus;
	osdovl5_mixer5_patgen_globle_ctrl0_RBUS ptg_ctrl_rbus;
	
	osd_size_rbus.regValue = OSD_RTD_INL( GDMA5_OSD5_SIZE_reg );
	
	osd_ctrl_rbus.regValue = OSD_RTD_INL( GDMA5_OSD5_CTRL_reg );

	mixer5_ctrl2_rbus.regValue = OSD_RTD_INL( OSDOVL5_Mixer5_CTRL2_reg );


	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL," osd size %d %d\n", osd_size_rbus.w, osd_size_rbus.h );
	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL," Ctrl en:%d rot:%d d3:%d\n", 
        osd_ctrl_rbus.osd5_en, osd_ctrl_rbus.rotate, osd_ctrl_rbus.d3_mode
	);

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL," mixer on:%d zoneType:%d zoneEn:%d\n",
			mixer5_ctrl2_rbus.mixer_en, mixer5_ctrl2_rbus.measure_osd_zone_type, mixer5_ctrl2_rbus.measure_osd_zone_en
		);

	// ptg
	ptg_ctrl_rbus.regValue = OSD_RTD_INL(OSDOVL5_Mixer5_PATGEN_Globle_Ctrl0_reg);

	GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL," ptg en:%d field:%d mode:%d sync:%d denMode:%d\n", 
		ptg_ctrl_rbus.patgen_sel, ptg_ctrl_rbus.patgen_field, ptg_ctrl_rbus.patgen_mode, ptg_ctrl_rbus.patgen_sync,
		ptg_ctrl_rbus.patgen_den_mode
	);
  }

  {
	

	gdma5_osd_intst_RBUS  osd_intst_rbus;
	gdma5_osd_inten_RBUS int_en_rbus;

	gdma5_dma_inten_RBUS dma_intEn_rbus;
	gdma5_dma_intst_RBUS dma_intst_rbus;
	

	int_en_rbus.regValue = OSD_RTD_INL(  GDMA5_OSD_INTEN_reg );
	osd_intst_rbus.regValue = OSD_RTD_INL( GDMA5_OSD_INTST_reg  );

	dma_intEn_rbus.regValue = OSD_RTD_INL(GDMA5_DMA_INTEN_reg);
	dma_intst_rbus.regValue = OSD_RTD_INL(GDMA5_DMA_INTST_reg);

	

	GDMAEXT_DUMP("\tosd intST vsync:%d vend:%d vfin:%d ov:%d %d\n", osd_intst_rbus.osd5_vsync, osd_intst_rbus.osd5_vact_end
		, osd_intst_rbus.osd5_fin, osd_intst_rbus.osd5_ov_range, osd_intst_rbus.afbc5_ov_range );
	GDMAEXT_DUMP("\tdma intST under:%d ov:%d acc:%d tfbcAcc:%d\n", dma_intst_rbus.osd5_udfl, dma_intst_rbus.osd5_ovfl
		, dma_intst_rbus.osd5_acc_sync, dma_intst_rbus.afbc5_acc_sync);

	GDMAEXT_DUMP("\tosd intrEn vsync:%d vend:%d vfin:%d ov:%d %d\n", int_en_rbus.osd5_vsync, int_en_rbus.osd5_vact_end
		, int_en_rbus.osd5_fin, int_en_rbus.osd5_ov_range, int_en_rbus.afbc5_ov_range );
	GDMAEXT_DUMP("\tdma intrEn under:%d ov:%d acc:%d tfbcAcc:%d\n", dma_intEn_rbus.osd5_udfl, dma_intEn_rbus.osd5_ovfl
		, dma_intEn_rbus.osd5_acc_sync, dma_intEn_rbus.afbc5_acc_sync);

		gdmaExt_osd_winfo_dump( GDMA_PLANE_OSD5 );

  }

	{
		//dc2h
		dc2h2_cap_dc2h2_ctrl_RBUS cap_dc2h2_ctrl_rbus;
		//dc2h2_cap_dc2h2_cap0_blk0_ctrl0_RBUS target_addr_rbus;
		dc2h2_cap_dc2h2_3dmaskto2d_ctrl_RBUS threeD_maskto2d_ctrl_rbus;
		dc2h2_scaledown_dc2h2_uzd_ctrl0_RBUS scaledown_uzd_ctrl0_rbus;
		dc2h2_scaledown_dc2h2_uzd_ibuff_ctrl_RBUS down_uzd_ibuff_ctrl_rbus;

		dc2h2_cap_dc2h2_cap0_cap_ctl0_RBUS cap0_cap_ctl0_rbus;
		
		int i;
		//
		unsigned int target_addr[] = { DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL0_reg, DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL0_reg,
			DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL0_reg, DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL0_reg};

		cap_dc2h2_ctrl_rbus.regValue = OSD_RTD_INL( DC2H2_CAP_DC2H2_CTRL_reg );


		GDMAEXT_DUMP("dc2h2_ctrl clken_disp:%d in_sel:%d in_vact:%d dc_arbiter_soft_rst:%d\n",	
			cap_dc2h2_ctrl_rbus.clken_disp_dc2h2, cap_dc2h2_ctrl_rbus.dc2h2_in_sel, cap_dc2h2_ctrl_rbus.dc2h2_in_vact,
			cap_dc2h2_ctrl_rbus.dc2h2_dc_arbiter_soft_rst
		);

		for(i=0; i< sizeof(target_addr)/sizeof(target_addr[0]); ++i  ) {

			GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, "dc2h addr_%d reg:0x%x val:0x:%x\n", i, target_addr[i], OSD_RTD_INL( target_addr[i]) );



		}

		cap0_cap_ctl0_rbus.regValue = OSD_RTD_INL( DC2H2_CAP_DC2H2_CAP0_Cap_CTL0_reg );
		GDMAEXT_DUMP(" hwMode:%d frc:%d(0:inputSlow) #num:%d idxFW:%d idxHW:%d\n", cap0_cap_ctl0_rbus.dc2h2_cap0_buf_index_mode, 
			cap0_cap_ctl0_rbus.dc2h2_cap0_frc_style, cap0_cap_ctl0_rbus.dc2h2_cap0_buf_total_num,
			cap0_cap_ctl0_rbus.dc2h2_cap0_buf_index_fw, cap0_cap_ctl0_rbus.dc2h2_cap0_buf_index_dbg
		);


		threeD_maskto2d_ctrl_rbus.regValue = OSD_RTD_INL(DC2H2_CAP_DC2H2_3DMaskTo2D_Ctrl_reg);


		// [xxx:16]: pixel per line 
		GDMAEXT_DUMP(" dc2h en:%d mode:%d 3dfmt:%d pixelPerLine:%d\n",
			threeD_maskto2d_ctrl_rbus.dc2h2_3dmaskto2d_en, threeD_maskto2d_ctrl_rbus.dc2h2_3dmaskto2d_mode, threeD_maskto2d_ctrl_rbus.dc2h2_3dmaskto2d_3dformat,
			threeD_maskto2d_ctrl_rbus.dc2h2_3dmaskto2d_h_size );

		scaledown_uzd_ctrl0_rbus.regValue = OSD_RTD_INL( DC2H2_SCALEDOWN_DC2H2_UZD_CTRL0_reg );

		GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL," h-en:%d v-en:%d mode:%d vCompEn:%d hCy:%d %d vCy:%d %d fmt:%d oBit%d inv:%d dumm:%d %d cut:%d\n",
			scaledown_uzd_ctrl0_rbus.h_zoom_en, scaledown_uzd_ctrl0_rbus.v_zoom_en, scaledown_uzd_ctrl0_rbus.buffer_mode, scaledown_uzd_ctrl0_rbus.video_comp_en,
			scaledown_uzd_ctrl0_rbus.h_c_table_sel, scaledown_uzd_ctrl0_rbus.h_y_table_sel,	scaledown_uzd_ctrl0_rbus.v_c_table_sel,	scaledown_uzd_ctrl0_rbus.v_y_table_sel,
			scaledown_uzd_ctrl0_rbus.sort_fmt, scaledown_uzd_ctrl0_rbus.out_bit, scaledown_uzd_ctrl0_rbus.odd_inv ,	
			scaledown_uzd_ctrl0_rbus.dummy18066400_23_13, scaledown_uzd_ctrl0_rbus.truncationctrl, scaledown_uzd_ctrl0_rbus.cutout_en
		);

		down_uzd_ibuff_ctrl_rbus.regValue = OSD_RTD_INL( DC2H2_SCALEDOWN_DC2H2_UZD_IBUFF_CTRL_reg );
		
		GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL,"ibuf_h:%d ibuf_v:%d \n",
			down_uzd_ibuff_ctrl_rbus.ibuf_h_size, down_uzd_ibuff_ctrl_rbus.ibuf_v_size
		);


		{
		//pqc
		gdma5_osd5_pq_decmp_irq_st_RBUS pqc_status;

		pqc_status.regValue = OSD_RTD_INL(GDMA5_OSD5_PQ_DECMP_IRQ_st_reg);

		//err_ch: [0-7]
		GDMAEXT_DUMP("PQDC under:%d notFin:%d fifoOver:%d(%d) all:%d error_ch:0x%x\n", pqc_status.underflow_irq_st,
			pqc_status.not_finish_irq_st, pqc_status.in_fifo_overflow_st, pqc_status.ring_fifo_overflow_st, pqc_status.pqdc_irq
			, pqc_status.regValue & 0xFF
		);
		
		}

	}



	return 0;
}


int gPQDC_setEnabled = 0;

void GDMA_OSD5_PQDC_Set( GDMA_WIN *win )
{

	//
	unsigned int target_addr[] = { DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL0_reg, DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL0_reg,
			DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL0_reg, DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL0_reg};


	win->fbdc1_addr = 	OSD_RTD_INL( target_addr[0]);
	win->fbdc2_addr = OSD_RTD_INL( target_addr[1]);

	win->fbdc3_addr = OSD_RTD_INL( target_addr[2]);
	win->fbdc4_addr = OSD_RTD_INL( target_addr[3]);


	{
		dc2h2_cap_dc2h2_cap0_wr_dma_num_bl_wrap_line_step_RBUS dc2h_line_step_rbus;

		dc2h_line_step_rbus.regValue = OSD_RTD_INL(DC2H2_CAP_DC2H2_CAP0_WR_DMA_num_bl_wrap_line_step_reg);

		#if 0
		if( enable ) {
			pq_decmp_rbus->decmp_en = 1;
		}
		else {
			pq_decmp_rbus->decmp_en = 0;
		}
		#endif//

		win->pitch = dc2h_line_step_rbus.regValue;
	}


}

void GDMA_OSD5_PQDC_Init(int enable)
{

	//if( gPQDC_setEnabled == 0) 
	{
		gdma5_osd5_buffer_ctrl_RBUS buf_ctrl_reg;


		gdma5_osd5_pq_decmp_RBUS pq_decmp_rbus;
		gdma5_osd5_pq_decmp_pair_RBUS pq_decmp_pair_rbus;


		pq_decmp_rbus.regValue =  OSD_RTD_INL(DC2H2_CAP_DC2H2_PQ_CMP_reg);
		pq_decmp_pair_rbus.regValue = OSD_RTD_INL(DC2H2_CAP_DC2H2_PQ_CMP_PAIR_reg);

		OSD_RTD_OUTL(GDMA5_OSD5_PQ_DECMP_reg, pq_decmp_rbus.regValue);

		OSD_RTD_OUTL(GDMA5_OSD5_PQ_DECMP_PAIR_reg, pq_decmp_pair_rbus.regValue);

		//0: hw 
		buf_ctrl_reg.buf_index_mode = 0;

		//0: fw, 1:dc2h
		buf_ctrl_reg.buf_index_sw_src = 1;
		buf_ctrl_reg.osd_buf_index_distance = 1;
		
		buf_ctrl_reg.buf_index_hw_total_num = 3;   //0x3: 4 bufs
		buf_ctrl_reg.buf_index_hw_freeze = 0;
		// 0:input slow
		buf_ctrl_reg.dma_frc_style = 0;
		//buf_ctrl_reg.buf_index_sw_sel  (f/w addr)

		OSD_RTD_OUTL(GDMA5_OSD5_BUFFER_CTRL_reg, buf_ctrl_reg.regValue);

		gPQDC_setEnabled = 1;
	}


}

void GDMA_OSD5_PQDC_Enable(int enable, GDMA_PIC_DATA *curPic )
{
	//GDMA_5_REG_CONTENT* pReg = &curPic->reg_5_content;
	//curPic = gdma->pic[disPlane] + gdma->curPic[disPlane];
	//pReg = &curPic->reg_content;


	{
		gdma5_osd5_buffer_ctrl_RBUS buf_ctrl_reg;

		gdma5_osd5_pq_decmp_RBUS* pq_decmp_rbus = &(curPic->reg_5_content.pq_decmp_reg);
		gdma5_osd5_pq_decmp_pair_RBUS* pq_decmp_pair_rbus =  &(curPic->reg_5_content.pq_decmp_pair_reg);


		pq_decmp_rbus->regValue =  OSD_RTD_INL(DC2H2_CAP_DC2H2_PQ_CMP_reg);
		pq_decmp_pair_rbus->regValue = OSD_RTD_INL(DC2H2_CAP_DC2H2_PQ_CMP_PAIR_reg);

		#if 0
		if( enable ) {
			pq_decmp_rbus->decmp_en = 1;
		}
		else {
			pq_decmp_rbus->decmp_en = 0;
		}
		#endif//

		//
		OSD_RTD_OUTL(GDMA5_OSD5_PQ_DECMP_reg, pq_decmp_rbus->regValue);

		OSD_RTD_OUTL(GDMA5_OSD5_PQ_DECMP_PAIR_reg, pq_decmp_pair_rbus->regValue);

		//0: hw 
		buf_ctrl_reg.buf_index_mode = 0;

		//0: fw, 1:dc2h
		buf_ctrl_reg.buf_index_sw_src = 1;
		buf_ctrl_reg.osd_buf_index_distance = 1;
		
		buf_ctrl_reg.buf_index_hw_total_num = 3;   //0x3: 4 bufs
		buf_ctrl_reg.buf_index_hw_freeze = 0;
		// 0:input slow
		buf_ctrl_reg.dma_frc_style = 0;
		//buf_ctrl_reg.buf_index_sw_sel  (f/w addr)

		OSD_RTD_OUTL(GDMA5_OSD5_BUFFER_CTRL_reg, buf_ctrl_reg.regValue);

	}

}