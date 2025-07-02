#include <linux/sync_file.h>  //fence

#include <scaler/panelCommon.h> // DTG_APP_TYPE 

#include "rtk_osdExt_main.h"
//#include <gal/rtk_osdExt_driver.h>
#include "rtk_osdExt_reg.h"

#include "rtk_osdExt_driver.h"
#include "rtk_osdExt_debug.h"


#include "rtk_osdExt_sr.h"

#include "rtk_gdma_fence.h"

//[FIXME] remove dependence
#include "gal/rtk_gdma_driver.h"

extern unsigned long gdma_total_ddr_size;
extern unsigned int g_StartCheckFrameCnt;
extern struct gdma_receive_work sGdmaReceiveWork[GDMA_MAX_PIC_Q_SIZE];



static DEFINE_MUTEX(gOSD_MGR_LOCK);

int gVsyncSource_id = 0;
DEFINE_MUTEX(gOSD_VSYNC_SOURCE_LOCK);


int g_osdExt_stop_update = 0;

int g_osdMode_send = GDMAEXT_OSD_MODE_OSD1; //set to osd 1 only by default 

//expTime related
extern int64_t gGDMA_EXPTime_StartWait;
extern int64_t gGDMA_StartWait_VsyncTime;

extern int64_t gGDMA_EXPTime_Send;
extern int64_t gGDMA_LAST_EXP_Time;
extern int64_t gGDMA_PIC_LAST_SIGNAL_Time;
extern unsigned int gVsyncPeriod_NS ;
extern unsigned int gVsyncDiff_NS ;
extern int64_t gOSD1_VSYNC_USER_TIME ;
extern int64_t gOSD1_VSYNC_USER_TIME_PREV ;


extern int gDTG_APP_TYPE_osd;


#ifndef RTK_GDMA_TEST_6748_OSD1
 // old register name on new soc
	#define GDMA_TFBC1_MIN_reg GDMA_AFBC1_MIN_reg
	#define GDMA_TFBC1_MAX_reg GDMA_AFBC1_MAX_reg

	#define GDMA_DBG_OSD_WI_2_reg GDMA_DBG_OSD_WI_18_reg

#endif



#ifdef RTK_GDMA_TEST_OSD1
// FIXME_TODO to remove this block
extern void GDMA_OSDReg_Set(GDMA_DISPLAY_PLANE disPlane);

#endif//

#ifdef GDMA_ENABLE_EXP_TIME
extern void GDMA_VerConvert_v2_to_v1(GRAPHIC_LAYERS_OBJECT* v1_data, GRAPHIC_LAYERS_OBJECT_V2* v2_data);

struct completion my_completion;
extern struct completion  gdma_expTime_completion;
extern spinlock_t expTime_completion_lock;



#endif//

extern void print_log_expTme( int tag, int print_level, int64_t  cur_expectedPresentTime);


extern void GDMA_OSD5_PQDC_Set( GDMA_WIN *win );

#ifdef TRIPLE_BUFFER_SEMAPHORE
extern int gSEMAPHORE_MAX;
extern int gSemaphore[GDMA_PLANE_MAXNUM];
#endif


extern gdma_dev *gdma_devices;


extern GDMAExt_device_cb g_osd_cb[GDMA_PLANE_ALL_MAXNUM];

//extern gdmaExt_dev *gdmaExt_devices;




irqreturn_t GDMAExt_irq_handler(int irq, void *dev_id)
{
	GDMA_EXT_LOG( GDMA_EXT_LOG_ALL, "%s fixme\n", __FUNCTION__);

    	return IRQ_HANDLED;
}



int GDMAExt_init_main(gdma_dev *gdma)
{
	GDMA_EXT_LOG( GDMA_EXT_LOG_ALL, "%s fixme\n", __FUNCTION__);

	//init all callback function
	GDMAExt_init_register_callback();

	#ifdef RTK_GDMA_TEST_OSD1
		g_osd_cb[GDMA_PLANE_OSD1].init(GDMA_PLANE_OSD1, gdma );

	#endif//
	
	if( gdma->osd4_init_on) {
		if( g_osd_cb[GDMA_PLANE_OSD4].init != NULL)  
		{
	 		g_osd_cb[GDMA_PLANE_OSD4].init( GDMA_PLANE_OSD4, gdma );
		}
	}

	if( gdma->osd5_init_on) { 
		
		if( g_osd_cb[GDMA_PLANE_OSD5].init != NULL)  {
			g_osd_cb[GDMA_PLANE_OSD5].init( GDMA_PLANE_OSD5, gdma );
		}

	}
	 
 

	return 0;
}

int GDMAExt_IS_EXIST(GDMA_DISPLAY_PLANE plane )
{
	gdma_dev *gdma = &gdma_devices[0];
	
	if( plane == GDMA_PLANE_OSD4 ) {
		return gdma->osd4_init_on;
	}

	if( plane == GDMA_PLANE_OSD5) {
		return gdma->osd5_init_on;
	}


	return 0;

}



int GDMAExt_init_interrupt(GDMA_DISPLAY_PLANE plane, int enable )
{
	
    GDMA_EXT_LOG( GDMA_EXT_LOG_ALL, "%s fixme\n", __FUNCTION__);
 
   if( enable != 0 && enable != 1 ) {

	GDMAEXT_WARNING("init hw interrupt FAIL :%d %d", plane, enable);

	return -1;

   }
    
	if( plane == GDMA_PLANE_OSD1 )
	{
	  	// 6748 0x18000290
		GDMA_OUTL(SYS_REG_INT_CTRL_SCPU_reg, SYS_REG_INT_CTRL_SCPU_osd_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_write_data(enable));

		GDMA_OUTL(GDMA_OSD_INTST_reg, ~1); /*  clear status */
		// OSD_RTD_OUTL(GDMA_OSD_INTEN_reg, GDMA_OSD_INTEN_write_data(1) | GDMA_OSD_INTEN_osd1_vact_end(1) | GDMA_OSD_INTEN_osd1_vsync(1));
		GDMA_OUTL(GDMA_OSD_INTEN_reg, GDMA_OSD_INTEN_osd1_vsync(1) | GDMA_OSD_INTEN_write_data(enable) );
	}
	else if ( plane == GDMA_PLANE_OSD4 )
	{

		GDMA_OUTL(SYS_REG_INT_CTRL_SCPU_2_reg, SYS_REG_INT_CTRL_SCPU_2_osd4_int_scpu_routing_en(1) | SYS_REG_INT_CTRL_SCPU_2_write_data(enable));

		GDMA_OUTL(GDMA4_OSD_INTST_reg, ~1); /*  clear status */

		#if 1
		GDMA_OUTL(GDMA4_OSD_INTEN_reg, GDMA4_OSD_INTEN_osd4_vsync(1) | GDMA4_OSD_INTEN_write_data(enable) );
		#else
		// GDMA4_OSD_INTEN_osd4_vact_end(1)
		GDMA_OUTL(GDMA4_OSD_INTEN_reg, GDMA4_OSD_INTEN_write_data(1) | GDMA4_OSD_INTEN_osd4_vsync(1) | GDMA4_OSD_INTEN_afbc4_ov_range(1) 
			| GDMA4_OSD_INTEN_osd4_ov_range(1) );
		#endif//

	}
	else if ( plane == GDMA_PLANE_OSD5 )
	{
		GDMA_OUTL(SYS_REG_INT_CTRL_SCPU_2_reg, SYS_REG_INT_CTRL_SCPU_2_osd5_int_scpu_routing_en(1) | SYS_REG_INT_CTRL_SCPU_2_write_data(enable));

		GDMA_OUTL(GDMA5_OSD_INTST_reg, ~1); /*  clear status */

		#if 1
		GDMA_OUTL(GDMA5_OSD_INTEN_reg, GDMA5_OSD_INTEN_osd5_vsync(1) | GDMA5_OSD_INTEN_write_data(enable) );
		#else
		GDMA_OUTL(GDMA5_OSD_INTEN_reg, GDMA5_OSD_INTEN_write_data(1) | GDMA5_OSD_INTEN_osd5_vsync(1) | GDMA5_OSD_INTEN_afbc5_ov_range(1) 
			| GDMA5_OSD_INTEN_osd5_ov_range(1) );
		#endif//
		
	}
	//#endif// RTK_ENABLE_GDMA_EXT_REGISTER


		GDMAEXT_DUMP("init hw interrupt %d en:%d", plane, enable);

	return 0;
}



int GDMAExt_PreOSDReg_Set(struct gdma_receive_work *ptr_work)
{
	VO_RECTANGLE srcWin;
	VO_RECTANGLE dispWin;

	GDMA_WIN *win = NULL ;
	
	gdma_dev *gdma = &gdma_devices[0];

	GDMA_PIC_DATA *curPic;
	GDMA_REG_CONTENT *pReg = NULL;
	GDMA_DISPLAY_PLANE disPlane = ptr_work->disPlane;
	dma_addr_t addr;


	memset(&srcWin, 0, sizeof(VO_RECTANGLE));
	memset(&dispWin, 0, sizeof(VO_RECTANGLE));

	curPic = gdma->pic[disPlane] + ptr_work->picQwr;
	pReg = &curPic->reg_content;
	win = &curPic->OSDwin;

	if ( ! win->used ) {
		GDMA_EXT_LOG( GDMA_EXT_LOG_WARNING, "%s, win->used is NOT one, something wrong \n", __FUNCTION__);
		return -1;
	}

	GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s, disPlane=%d ptr_work->picQwr=%d \n", __FUNCTION__, disPlane, ptr_work->picQwr );

	
	if( curPic->plane != ptr_work->disPlane ) {
		// assert for new setting

		GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "plane mismatch! %d %d \n", curPic->plane, ptr_work->disPlane );
		return -1;
	}


	dispWin.x = win->dst_x;
	dispWin.y = win->dst_y;

	if (win->dst_width != 0 && win->dst_height != 0) {

		dispWin.width = win->dst_width;
		dispWin.height = win->dst_height;

	} else {

		//default win  [todo] [fixme] need to query other value ?
		VO_RECTANGLE disp_rec;
		//GDMAExt_getDispSize(GDMA_DISPLAY_PLANE plane, VO_RECTANGLE *disp_rec)
		GDMAExt_getDispSize(disPlane, &disp_rec);

		dispWin.width = disp_rec.width;
		dispWin.height = disp_rec.height;
	}

	memset(&srcWin, 0, sizeof(VO_RECTANGLE) );


	srcWin.x = win->winXY.x;
	srcWin.y = win->winXY.y;
	srcWin.width = win->winWH.width;
	srcWin.height = win->winWH.height;

	addr = dma_map_single(gdma->dev, (void *)win, sizeof(GDMA_WIN), DMA_TO_DEVICE);


	
	g_osd_cb[disPlane].presetOSD(curPic->plane, curPic, &srcWin, &dispWin, ptr_work);



	dma_sync_single_for_device(gdma->dev, addr, sizeof(GDMA_WIN), DMA_TO_DEVICE);
	dma_unmap_single(gdma->dev, addr, sizeof(GDMA_WIN), DMA_TO_DEVICE);

	/*  register value was ready */
	curPic->workqueueDone = 1;

		/*  call register debug.... */
		/* GDMA_RegDebug(); */
	 
	
	
	return 0;

}


int GDMAExt_PreUpdate(struct gdma_receive_work *ptr_work)
{
	// struct work_struct *psWork = &(work->GdmaReceiveWork);
	//struct gdma_receive_work *ptr_work;
	//ptr_work = (struct gdma_receive_work *) psWork;


	if ( ptr_work->used != 1) {
		GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "GDMAExt_PreUpdate invalid \n");

		return -1;
	}

	GDMAExt_PreOSDReg_Set(ptr_work);


	#if 0
	
		GDMAExt_PreOSDReg




	#endif//0
	
	return 0;
}


#if 0
//old/temp version

#else 
//newer GDMAExt_Update

int GDMAExt_Update( gdma_dev *gdma)
{
	//int inISR = 1 ; 

	volatile int Qend=0, loop = 0;
	volatile GDMA_PIC_DATA *curPic;
	volatile int disPlane, disPlane_tmp, cnt = 0,  curPicIdx = 0;

	//volatile static int osdSyncStamp[GDMA_PLANE_MAXNUM] = {0};	/*  keep the setting which picture has syncstamp */

	int QueueFlag[GDMA_PLANE_ALL_MAXNUM] = {0};	/*  1: indicate this queue has picture to show */

	unsigned int updatedOSDx = 0;				/* actual updated OSD number */

	#ifdef RTK_GDMA_TEST_OSD1
		int onlinePlane[] = {GDMA_PLANE_OSD1, GDMA_PLANE_OSD2};
	#else
		int onlinePlane[] = {GDMA_PLANE_OSD4, GDMA_PLANE_OSD5};
	#endif//

	int onlineMaxNum = sizeof(onlinePlane)/sizeof(onlinePlane[0]) ;
	int  onlineIdx = 0;
	
	unsigned int onlineProgDone = GDMA_CTRL_write_data(1);



	/* search all updated picture of osd plane  */
	for (  onlineIdx = 0; onlineIdx < onlineMaxNum; onlineIdx++ ) 
	{
		
		disPlane = onlinePlane[onlineIdx];

		#ifdef TRIPLE_BUFFER_SEMAPHORE
		if ((gdma->picQwr[disPlane] > gdma->curPic[disPlane])  && (gdma->picQwr[disPlane] - gdma->curPic[disPlane]) > 2)
			GDMA_EXT_LOG(GDMA_EXT_LOG_WARNING, "GDMA line=%d : r %d,w %d c %d \n", __LINE__, gdma->picQrd[disPlane], gdma->picQwr[disPlane], gdma->curPic[disPlane]);
		else {
			if ((gdma->picQwr[disPlane] < gdma->curPic[disPlane]) && ((gdma->picQwr[disPlane]+GDMA_MAX_PIC_Q_SIZE) >= gdma->curPic[disPlane])  && ((gdma->picQwr[disPlane]+GDMA_MAX_PIC_Q_SIZE) - gdma->curPic[disPlane]) > 2)
				GDMA_EXT_LOG(GDMA_EXT_LOG_WARNING, "GDMA line=%d : r %d,w %d c %d \n", __LINE__, gdma->picQrd[disPlane], gdma->picQwr[disPlane], gdma->curPic[disPlane]);
		}
		#else
			if ((gdma->picQwr[disPlane] > gdma->curPic[disPlane])  && (gdma->picQwr[disPlane] - gdma->curPic[disPlane]) >= 2)
				GDMA_EXT_LOG(GDMA_EXT_LOG_WARNING, "GDMA line=%d : OSD%d r %d,w %d c %d \n", __LINE__, disPlane, gdma->picQrd[disPlane], gdma->picQwr[disPlane], gdma->curPic[disPlane]);
			else {
				if ((gdma->picQwr[disPlane] < gdma->curPic[disPlane]) && ((gdma->picQwr[disPlane]+GDMA_MAX_PIC_Q_SIZE) >= gdma->curPic[disPlane])  && ((gdma->picQwr[disPlane]+GDMA_MAX_PIC_Q_SIZE) - gdma->curPic[disPlane]) >= 2)
					GDMA_EXT_LOG(GDMA_EXT_LOG_WARNING, "GDMA line=%d : OSD%d r %d,w %d c %d \n", __LINE__, disPlane, gdma->picQrd[disPlane], gdma->picQwr[disPlane], gdma->curPic[disPlane]);
			}
		#endif

		/* check next picture */
		{
		
			gdma->ctrl.sync[disPlane] = 1; // force to sync

			GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, " OSD[%d] ctrl.sync on, r %d, w %d cur %d\n",  disPlane, gdma->picQrd[disPlane], gdma->picQwr[disPlane], gdma->curPic[disPlane] );
			
			curPic = gdma->pic[disPlane] + gdma->curPic[disPlane];
			curPic->repeatCnt++;
			

			if ( gdma->curPic[disPlane] != gdma->picQwr[disPlane] ) { 
				//if (osdSyncStamp[disPlane] != 1) 
				{
					curPicIdx = gdma->ctrl.displayEachPic ? ((gdma->curPic[disPlane] + 1) & (GDMA_MAX_PIC_Q_SIZE - 1)) : gdma->picQwr[disPlane];
    
					//update to new curPic
					curPic = gdma->pic[disPlane] + curPicIdx;

					if ( curPic->workqueueDone == 1 ) 
					{    /*  register value was ready */

						//advance to next 
						gdma->curPic[disPlane] = curPicIdx;

						QueueFlag[disPlane] = 1;
						GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s, disPlane=%d, Queue Next pic c %d, w %d\n", __FUNCTION__, disPlane, gdma->curPic[disPlane], gdma->picQwr[disPlane]);

					} else {
						GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "error workqueueDone=0 %d %d \n", disPlane, curPicIdx );

						continue;
					}

				}


			} 
			else {
				//plane not update .. 
				//#ifdef GDMA_REPEAT_LOG
				#if 1 
					static int prepic;
					if (prepic == gdma->curPic[disPlane]) {
						GDMAEXT_DEBUG("rept %d(%d)\n", gdma->curPic[disPlane], curPic->repeatCnt);
					}
					prepic = gdma->curPic[disPlane];
				#endif

			}
			
			/*  check Queueflag if picture wants to show */
			cnt += QueueFlag[disPlane];



		}

	}

	#if 0
	onlineMaxNum = 0;
	if (QueueFlag[GDMA_PLANE_OSD1] == 1) {
		onlineMaxNum++;
	}
	if (QueueFlag[GDMA_PLANE_OSD2] == 1) {
		onlineMaxNum++;
	}
	if (QueueFlag[GDMA_PLANE_OSD3] == 1) {
				onlineMaxNum++;
	}
	#endif//


	if (cnt == 0) {
		GDMAEXT_DEBUG( "%s, no pic can update. cnt=%d \n", __FUNCTION__, cnt);
		return GDMA_SUCCESS;		/*  means no picture to show */
	}
		
	//check intr 
	
	/* update picture info to register */
	for (onlineIdx = cnt = 0; onlineIdx < onlineMaxNum; onlineIdx++) {

		disPlane = onlinePlane[onlineIdx];

		#if 0
		int status = 0; 
		g_osd_cb[disPlane].getStatus(disPlane, GDMA_STATUS_INTR_ROUTING, gdma, &status );
		if( status == GDMA_STATUS_OFF)
		if ( rtd_inl(SYS_REG_INT_CTRL_SCPU_reg) & SYS_REG_INT_CTRL_SCPU_osd_int_scpu_routing_en_mask ) 
		{
			GDMA_EXT_LOG(GDMA_EXT_LOG_WARNING, "no intr enabled. don't send pic %d\n", disPlane);
			continue;
		}
		#endif//

		if ( gdma->ctrl.sync[disPlane] && (QueueFlag[disPlane] == 1)) {

			//gdma->curPic[disPlane] index
			curPic = gdma->pic[disPlane] + gdma->curPic[disPlane];

			GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s, OSD[%d] c ptr = %d \n", __FUNCTION__, disPlane, gdma->curPic[disPlane]);

			{ /* Normal case for OSD display */
				/*  call GDMA HW setting */
				//GDMA_OSDReg_Set(disPlane);
				//GDMA_OSD1_DevCB_SetGDMA(disPlane, gdma);
				if( g_osd_cb[disPlane].setGDMA != NULL ) {

					g_osd_cb[disPlane].setGDMA(disPlane, gdma);
				}
				else {
					GDMA_EXT_LOG(GDMA_EXT_LOG_FATAL, " fatal not register cb for %d\n", disPlane );
				}
				
				/*  work was done in workqueue */
				sGdmaReceiveWork[curPic->workqueueIdx].used = 0;
			}

			
			//update picQrd (read pointer)
			#if 1
				disPlane_tmp = disPlane;

				Qend = ((gdma->curPic[disPlane_tmp] + GDMA_MAX_PIC_Q_SIZE - 2) & (GDMA_MAX_PIC_Q_SIZE - 1));
				gdma->picCount[disPlane_tmp]--;
				gdma->picQrd[disPlane_tmp] = Qend;


				#ifdef TRIPLE_BUFFER_SEMAPHORE
					gSemaphore[disPlane_tmp]++;
					if (gSemaphore[disPlane_tmp] > gSEMAPHORE_MAX) {
						GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "gdma : gSemaphore[%d] %d\n", disPlane_tmp, gSemaphore[disPlane_tmp]);
						gSemaphore[disPlane_tmp] = gSEMAPHORE_MAX;
					}
				#endif




				// QueueFlag[disPlane_tmp] = 0;  // -> clear later
			#else
				//old
			for (loop = 0; loop < onlineMaxNum; loop++) {
				disPlane_tmp = onlinePlane[loop];

				if (QueueFlag[disPlane_tmp] == 1 && osdSyncStamp[disPlane_tmp] == 0) {

					GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, " change picQrd to end. QueueFlag loop:%d p:%d  \n", loop, disPlane_tmp);// this line print out 

					Qend = ((gdma->curPic[disPlane_tmp] + GDMA_MAX_PIC_Q_SIZE - 2) & (GDMA_MAX_PIC_Q_SIZE - 1));
					gdma->picCount[disPlane_tmp]--;
					gdma->picQrd[disPlane_tmp] = Qend;

					if (disPlane_tmp == GDMA_PLANE_OSD1)
						updatedOSDx |= GDMA_CTRL_osd1_prog_done(1);
					else if (disPlane_tmp == GDMA_PLANE_OSD2)
						updatedOSDx |= GDMA_CTRL_osd2_prog_done(1);
					else if (disPlane_tmp == GDMA_PLANE_OSD3)
						updatedOSDx |= GDMA_CTRL_osd3_prog_done(1);
						

					#ifdef TRIPLE_BUFFER_SEMAPHORE
					gSemaphore[disPlane_tmp]++;
					if (gSemaphore[disPlane_tmp] > gSEMAPHORE_MAX) {
						rtd_pr_gdma_debug(KERN_EMERG"gdma : gSemaphore[%d] %d\n", disPlane_tmp, gSemaphore[disPlane_tmp]);
						gSemaphore[disPlane_tmp] = gSEMAPHORE_MAX;
					}
					#endif

					/* clear queue flag */
					QueueFlag[disPlane_tmp] = 0;

					cnt++;
				}
			}
			#endif//

		}
		else{
			//GDMAEXT_DUMP("plane %d not sync or queue %d %d\n", disPlane, gdma->ctrl.sync[disPlane], QueueFlag[disPlane]  );


		}

	}

	//kkk TODO, clear curPic->repeatCnt ??


	// apply mixer and progDone

	for (loop = 0; loop < onlineMaxNum; loop++) 
	{
		disPlane = onlinePlane[loop];

		if( QueueFlag[disPlane] == 0 ) {
			continue;
		}

			//kkk fixmeTODO, check  updatedOSDx and move outside 
			#if 0
			onlineProgDone |= GDMA_CTRL_osd1_prog_done(1);
			onlineProgDone |= GDMA_CTRL_osd2_prog_done(1);
			onlineProgDone |= GDMA_CTRL_osd3_prog_done(1);

			#endif//


		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, " do set progDone :%d ", disPlane );

		if( g_osd_cb[disPlane].setMixer != NULL ) {
			g_osd_cb[disPlane].setMixer(disPlane, gdma);

			
		}

		if( g_osd_cb[disPlane].set_ProgDone != NULL ) {

			GMDA_OSD_DONE_MODE sync_mode = GMDA_OSD_DONE_MODE_SINGLE;

			if( gdma->osd45_sync ) {
				sync_mode = GMDA_OSD_DONE_MODE_SYNC45;
			}

			g_osd_cb[disPlane].set_ProgDone(disPlane, sync_mode, gdma);

			GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s, updatedOSDx = 0x%x \n", __FUNCTION__, updatedOSDx);
			GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, "%s, w onlineProgDone = 0x%x \n", __FUNCTION__, onlineProgDone);
		}

		

	}




	return 0;
}

#endif//

void GDMAExt_CopyLayerInfo_V3(GDMA_PICTURE_OBJECT *dest, GDMA_LAYER_OBJECT_V3 *src)
{
	//static int buffer_count = 0;

	dest->address = src->address;
	dest->alpha = src->alpha;
	dest->clear_x = src->clear_x;
	dest->clear_y = src->clear_y;
	dest->colorkey = src->colorkey;
	dest->context = src->context;
	dest->decompress = src->decompress;
	dest->dst_height = src->dst_height;
	dest->dst_width = src->dst_width;
	dest->dst_x = src->dst_x;
	dest->dst_y = src->dst_y;
	dest->format = src->format;
	dest->fbdc_format = src->fbdc_format;
	dest->key_en = src->key_en;
	dest->layer_used = 1;
	dest->paletteIndex = src->paletteIndex;
	dest->picLayout = src->picLayout;
	dest->pitch = src->pitch;
	dest->plane_ar.value = src->plane_ar.value;
	dest->plane_gb.value = src->plane_gb.value;
	dest->show = src->show;
	dest->src_type = src->src_type;
	dest->syncInfo.syncstamp = src->syncstamp;
	dest->width = src->width;
	dest->height = src->height;

	//caller default is 0
	#if 0
	if( (src->compressed_ratio == TFBC_HWC_NON_COMPRESS) ) {
		dest->rgb_order = 0;
	}
	#endif//
	if( dest->format == VO_OSD_COLOR_FORMAT_ARGB2101010_LITTLE ||
	    dest->format == VO_OSD_COLOR_FORMAT_ARGB2101010
	 ){ // input is ABGR
		dest->rgb_order = 1;
	}

	if( src->decompress==1   ) {
		if( VO_OSD_COLOR_FORMAT_PQDC == dest->format ) {
			
			dest->rgb_order = 0;
		}
		else {
			dest->rgb_order = 1;
		}

	}

	dest->scale_factor = src->scale_factor;
	if(src->compressed_ratio == TFBC_HWC_NON_COMPRESS)
		dest->compressed_ratio = 0;
	else
		dest->compressed_ratio = src->compressed_ratio;

	//#if defined(CONFIG_ARCH_RTK2875Q)
#if 0
	if(src->compressed_ratio != TFBC_HWC_NON_COMPRESS && src->size_0 != 0 && src->size_1!= 0)
	{
		dest->multi_seg.buffer_size1 = src->size_0;
		dest->multi_seg.buffer_size2 = src->size_1;
		dest->multi_seg.phy_addr1 = src->addr_0;
		dest->multi_seg.phy_addr2 = src->addr_1;
		dest->address = buffer_count * (src->size_0 + src->size_1); // this would be virtual top address
		dest->multi_seg.vir_top_addr = dest->address; //

		buffer_count++;
		if(buffer_count >2)
			buffer_count =0;
	}
#endif	

	GDMAEXT_TMP("CopyLayerInfo 0x%lx to 0x%lx  dst:[%d %d %d %d %d] addr:0x%x srcType:%d\n", src, dest, 
		dest->dst_x, dest->dst_y, dest->width, dest->height, dest->pitch, dest->address, dest->src_type
	);
	//[fixme]  //need copy extra V3 fields from src ?


}



int GDMAExt_ReceivePicture( PICTURE_LAYERS_OBJECT *data )
{
	gdma_dev *gdma = &gdma_devices[0];
	GDMA_PIC_DATA *pic = NULL;
	GDMA_WIN *win = NULL;
	COMPOSE_PICTURE_OBJECT *com_picObj = NULL;
	int idx=0, ilayer=0, emptyIdx = 0, idxQwr = 0, sema_ret=0;
	GDMA_PICTURE_OBJECT *picObj = NULL;
	GDMA_DISPLAY_PLANE disPlane = GDMA_PLANE_OSD4;

	VO_RECTANGLE disp_res;
	int onlinePlane[GDMA_PLANE_MAXNUM], onlineMaxNum = 0;
	unsigned long saveBufAddr = 0;
	unsigned long timeout = jiffies + (HZ/50);	/* 20 ms */


	
	#if 0 //#if IS_ENABLED(CONFIG_REALTEK_PCBMGR)
		PANEL_CONFIG_PARAMETER *panel_parameter = (PANEL_CONFIG_PARAMETER *)&platform_info.panel_parameter;

		rtd_pr_gdma_debug("[GDMA] panel vflip=%d \n", panel_parameter->iVFLIP);
		if (panel_parameter->iVFLIP)	{
			/*	SFG_CTRL_0 */
			OSD_RTD_OUTL(SFG_SFG_CTRL_0_reg, rtd_inl(SFG_SFG_CTRL_0_reg) | SFG_SFG_CTRL_0_h_flip_en(1));
			/*if bootcode has already enable vfip bit ,
			  we don't have to reset it again */
			if(GDMA_OSD1_CTRL_get_rotate(rtd_inl(GDMA_OSD1_CTRL_reg)) ==0) {
			GDMA_ConfigVFlip(1);
		}
			gdma->ctrl.enableVFlip = 1;
		}
	#endif

	/* protection to unreasonable parameters */
	if (data != NULL && data->layer_num) {
		if (data->layer[0].normal_num) {
			if (data->layer[0].normal[0].dst_width < data->layer[0].normal[0].width) {
				GDMAEXT_ERROR("%s, wrong width size, src=%d, dst=%d\n", __FUNCTION__, data->layer[0].normal[0].width, data->layer[0].normal[0].dst_width);
				return GDMA_FAIL;
			}
			if (data->layer[0].normal[0].dst_height < data->layer[0].normal[0].height) {
				GDMAEXT_ERROR("%s, wrong height size, src=%d, dst=%d\n", __FUNCTION__, data->layer[0].normal[0].height, data->layer[0].normal[0].dst_height);
				return GDMA_FAIL;
			}
		}
	}

	down(&gdma->sem_receiveExt);  //move before getDispSize, to keep disp size consist
	GDMA_ENTER_CRITICAL();
	

	GDMA_EXT_LOG(10, "%s, data->layer_num=%d\n", __FUNCTION__, data->layer_num);


	/* avoid interrupt routing to SCPU disable within 100 frames */
	#if 0
	if (g_StartCheckFrameCnt) {
		g_StartCheckFrameCnt--;
		//rtd_pr_gdma_err(KERN_EMERG"%s, g_StartCheckFrameCnt=%d \n", __FUNCTION__, g_StartCheckFrameCnt);
		if ((rtd_inl(SYS_REG_INT_CTRL_SCPU_reg) & SYS_REG_INT_CTRL_SCPU_osd_int_scpu_routing_en_mask) == 0) {
			GDMA_EXT_LOG(3,"%s, GDMA interrupt routing to SCPU disable ! \n", __FUNCTION__);
			OSD_RTD_OUTL(SYS_REG_INT_CTRL_SCPU_reg, SYS_REG_INT_CTRL_SCPU_osd_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_write_data(1));
		}
	}
	#endif//0

	for (ilayer = 0; ilayer < data->layer_num; ilayer++) 
	{

		/*  search the empty work space */
		for (idx = 0; idx < GDMA_MAX_PIC_Q_SIZE; idx++) {
			if (sGdmaReceiveWork[idx].used == 0) {
				sGdmaReceiveWork[idx].used = 1;
				break;
			}
		}

		
		emptyIdx = idx;
		GDMA_EXT_LOG(3, "%s, emptyIdx=%d\n", __FUNCTION__, emptyIdx);

		if (emptyIdx == GDMA_MAX_PIC_Q_SIZE) {
			GDMA_ERR(  "%s, Warning! GDMA workqueue is full \n", __FUNCTION__);
			GDMA_EXIT_CRITICAL();
			up(&gdma->sem_receiveExt);
			return COMP_BUF_WORK_FULL;
		}

		com_picObj = &data->layer[ilayer];
		if (com_picObj->normal_num > 0)
			picObj = &com_picObj->normal[0];		/* just one normal OSD for one online path  */

		else if (com_picObj->fbdc_num > 0)
			picObj = &com_picObj->fbdc[0];
		else {
			GDMA_ERR(  "%s, PICTURE_LAYERS_OBJECT's COMPOSE_PICTURE_OBJECT has NO layer \n", __FUNCTION__);
			GDMA_EXIT_CRITICAL();
			up(&gdma->sem_receiveExt);
			return GDMA_FAIL;
		}

	
		//if (picObj->layer_used == 1) 
		{
			// (GDMA_PICTURE_OBJECT) picObj->plane is VO_GRAPHIC_PLANE, gdma's internal GDMA_PIC_DATA's plane is GDMA_PLANE_xx
			if (picObj->plane == VO_GRAPHIC_OSD1)
				disPlane = GDMA_PLANE_OSD1;
			else if (picObj->plane == VO_GRAPHIC_OSD2)
				disPlane = GDMA_PLANE_OSD2;
			else if (picObj->plane == VO_GRAPHIC_OSD4)
				disPlane = GDMA_PLANE_OSD4;
			else if (picObj->plane == VO_GRAPHIC_OSD5)
				disPlane = GDMA_PLANE_OSD5;


			// DTG timing maybe changed at run-time, so always read DTG timing to decide the panel resolution
			GDMAExt_getDispSize(disPlane, &disp_res);

			GDMA_EXT_LOG(10, "%s, Panel [%d,%d]\n", __FUNCTION__, disp_res.width, disp_res.height);


			onlinePlane[onlineMaxNum++] = disPlane;
			gdma->dispWin[disPlane].width = disp_res.width;
			gdma->dispWin[disPlane].height = disp_res.height;
			GDMA_EXT_LOG(6, "%s, gdma->dispWin[%d].width=%d, height=%d \n", __FUNCTION__, disPlane, gdma->dispWin[disPlane].width, gdma->dispWin[disPlane].height);

			if (gdma->picQrd[disPlane] == ((gdma->picQwr[disPlane] + 1) & (GDMA_MAX_PIC_Q_SIZE - 1))) {
				GDMA_EXT_LOG(2, "GDMA: pic Queue %d fullness r %d,w %d,c %d\n", disPlane, gdma->picQrd[disPlane], gdma->picQwr[disPlane], gdma->curPic[disPlane]);
				sGdmaReceiveWork[emptyIdx].used = 0;	/* free work */

				/* drop frames and go ahead for in-stop on firse use
				*  SE maybe block by debug thread, then sends the queued picture very quickly when system not busy
				*/
				GDMA_EXT_LOG(2,  "GDMA: disPlane=%d pid=%d\n", disPlane, current->pid);
				//kkk: don't reset other work, 
				#if 0
				for (idx = 0; idx < GDMA_MAX_PIC_Q_SIZE; idx++)
					sGdmaReceiveWork[idx].used = 0;
				#endif//

				gdma->picQwr[disPlane] = gdma->curPic[disPlane];
				gdma->picQrd[disPlane] = ((gdma->curPic[disPlane] - 2) & (GDMA_MAX_PIC_Q_SIZE - 1));

				/* return -99; */
				/* continue; */
			} else if (gdma->ctrl.zeroBuffer) {
				GDMA_EXT_LOG(2, "GDMA: zeroBuffer is 1\n");
				GDMA_EXIT_CRITICAL();
				sGdmaReceiveWork[emptyIdx].used = 0;	/* free work */
				up(&gdma->sem_receiveExt);
				return GDMA_SUCCESS;
			}
			DBG_PRINT(KERN_EMERG"R\n");

			//get new curPic
			pic = gdma->pic[disPlane] + (idxQwr = (gdma->picQwr[disPlane] + 1) & (GDMA_MAX_PIC_Q_SIZE - 1));

			memset(pic, 0, sizeof(GDMA_PIC_DATA));

			pic->seg_num = picObj->seg_num;
			pic->current_comp_idx = picObj->current_comp_idx;
			pic->cookie = data->cookie;
			pic->show = picObj->show;
			pic->syncInfo = picObj->syncInfo;
			pic->workqueueDone = 0;

				

			pic->plane = disPlane;  //kkk add
			
            //RTK mark for netfix
            // [FIXME][TODO]  move to caller and wrapper with GDMA_ASSIGN_ORDER
            #if 0
			if(enable_osd1osd3mixerorder)
			{
				if (picObj->plane == VO_GRAPHIC_OSD1)
					pic->onlineOrder = C1;
				else if (picObj->plane == VO_GRAPHIC_OSD2)
					pic->onlineOrder = C0;
				else
					pic->onlineOrder = com_picObj->onlineOrder;
			}
			else  {
				pic->onlineOrder = com_picObj->onlineOrder;
			}
			#endif//

			pic->clear_x.value = picObj->clear_x.value;
			pic->clear_y.value = picObj->clear_y.value;
			*(char *)&pic->status = 0;
			pic->repeatCnt = 0;
			pic->context   = picObj->context;
			pic->workqueueIdx = emptyIdx;
			pic->isFromOffline = picObj->isFromOffline;
			pic->scale_factor = picObj->scale_factor;

			/*  mixer's plane alpha setting, only for normal OSD */
			/*if (com_picObj->normal_num > 0) */{
				pic->plane_ar.value = picObj->plane_ar.value;
				pic->plane_gb.value = picObj->plane_gb.value;
			}

			win = &pic->OSDwin;
			//[FIXME] ? reset win every time ?

			
			win->nxtAddr.addr     = 0;
			win->nxtAddr.last     = 1;

			/* win->winXY.x          = (gdma->f_box_mode && gdma->box_dst_height == VIDEO_USERDEFINED)? picObj->x: picObj->dst_x; //picObj->dst_x; //picObj->x; */
			/* win->winXY.y          = (gdma->f_box_mode && gdma->box_dst_height == VIDEO_USERDEFINED)? picObj->y: picObj->dst_y; //picObj->dst_y; //picObj->y; */

			/*  canvas X, Y always set the X, Y from picture */
			/*  destination x, y, width, height are for final resolution and position on panel */
			win->winXY.x          = picObj->x;
			win->winXY.y          = picObj->y;


			win->winWH.width      = picObj->width;
			win->pitch            = picObj->pitch;

			
			win->winWH.height     = picObj->height;
            
			win->attr.extendMode  = 0;
			win->attr.rgbOrder    = picObj->rgb_order;/* VO_OSD_COLOR_RGB; */
			win->attr.objType     = 0;
			win->attr.type        = picObj->format & 0x1f;
			win->attr.littleEndian = picObj->format >> 5;
			win->attr.alphaEn     = (win->attr.alpha   = picObj->alpha) >  0 ? 1 : 0;

			//if (gdma->f_palette_update == TRUE)
				win->attr.keepPreCLUT = 0;
			//else
			//	win->attr.keepPreCLUT = 1;
			//gdma->f_palette_update = FALSE;
			win->attr.clut_fmt    = picObj->paletteformat;/* rgba format */
			
			//if (win->attr.type < 3)
				// win->CLUT_addr = virt_to_phys(gdma->OSD_CLUT[picObj->paletteIndex]);  //

			if (picObj->decompress) {
				win->objOffset.objXoffset = 0;
				win->objOffset.objYoffset = 0;
			} else {

				//rtd_pr_gdma_debug(KERN_EMERG"GDMA, %s,picObj->dst_x=%d picObj->dst_y=%d \n", __FUNCTION__,picObj->dst_x,picObj->dst_y);
				//this for fix when cursor move out of bound of panel(0,0),need do offset.
					if(picObj->dst_x<0) {
						win->objOffset.objXoffset = abs(picObj->dst_x);
						picObj->dst_x=0;
					} else {
						win->objOffset.objXoffset = picObj->x;
					}
					
					if( picObj->dst_y<0 ) {
						win->objOffset.objYoffset = gdma->ctrl.enableVFlip ? 0 : abs(picObj->dst_y);
						picObj->dst_y=0;
					} else {
						win->objOffset.objYoffset = gdma->ctrl.enableVFlip ? 0 : picObj->y;
					}
			}

			win->dst_x = picObj->dst_x;
			win->dst_y = picObj->dst_y;
			


			/*  destination width/height set as the panel resolution */
			if (picObj->dst_width == 0)     /*  0 means scaling to panel resolution */
				win->dst_width = disp_res.width;
			else
				win->dst_width = picObj->dst_width;
			
			if (picObj->dst_height == 0)    /*  0 means scaling to panel resolution */
				win->dst_height = disp_res.height;
			else
				win->dst_height = picObj->dst_height;

			win->attr.compress = 0;

			if (picObj->src_type == SRC_NORMAL_PQDC) {
				win->attr.compress = 1;
			}

			win->used = 1;
			
			win->colorKey.keyEn   = (picObj->colorkey != -1);
			win->colorKey.key     = picObj->colorkey;


			saveBufAddr = win->top_addr         = picObj->address + (gdma->ctrl.enableVFlip ? picObj->y + picObj->height - 1 : 0) * picObj->pitch;


			if( picObj->src_type == SRC_NORMAL_PQDC) {
				// PQC update win->pitch here

				// pic->pqdc_enable = 1;
				GDMA_OSD5_PQDC_Set( win );



			}
		
//for ANDROIDTV-305 test
			if( saveBufAddr >= gdma_total_ddr_size){
				dump_stack();
				rtd_pr_gdma_info("%s, saveBufAddr=%x is over OSD_MAX_PHY_ADDR(%x) \n", __FUNCTION__,saveBufAddr,gdma_total_ddr_size);
				GDMA_EXIT_CRITICAL();
				up(&gdma->sem_receiveExt);
				return GDMA_FAIL;
			}



			/*  TODO: FBDC information */
			win->attr.IMGcompress =  (com_picObj->fbdc_num > 0) ? 1 : 0;
			if (win->attr.IMGcompress) {
				win->attr.type = VO_OSD_COLOR_FORMAT_ARGB8888 & 0x1f;
				win->attr.littleEndian = VO_OSD_COLOR_FORMAT_ARGB8888 >> 5;
				win->attr.alphaEn = 0;		/* FBDC has alpha fucntion itself */
				win->attr.alpha = 0;		/* FBDC has alpha fucntion itself */
				win->CLUT_addr = 0;
				win->dst_x = win->dst_y = 0;
			}
			
			if( com_picObj->fbdc_num )
			{ 
			//#if  defined(CONFIG_ARCH_RTK6748) , CONFIG_ARCH_RTK2875Q
				win->pitch = picObj->pitch;
				win->fbdc1_addr = picObj->address + ((picObj->width * picObj->height / 64) * 8); // 1 tile header = 64bit/64 pxl
				win->tfbc_format.tfbc_req_format = 0x0C; // ARGB8888
				win->tfbc_format.tfbc_req_lossy = picObj->compressed_ratio;
									win->tfbc_format.tfbc_argb_order = 0;
				win->objOffset.objYoffset = (unsigned short)((picObj->address&0xffff0000)>>16);
				win->objOffset.objXoffset = (unsigned short)(picObj->address&0x0000ffff);
			
				win->afbc_totaltile.x_total_tile = picObj->width;
				win->afbc_totaltile.y_total_tile = picObj->height;
				win->afbc_xy_pixel.x_pic_pixel = picObj->width;
				win->afbc_xy_pixel.y_pic_pixel = picObj->height;
				win->afbc_xy_sta.x_sta = picObj->x;
				win->afbc_xy_sta.y_sta = (gdma->ctrl.enableVFlip ? (picObj->height - 1) : picObj->y);
					
				
				/* calculate the canvas size */
				win->winWH.width      = ((picObj->width+picObj->dst_x) > win->winWH.width) ? (picObj->width+picObj->dst_x) : win->winWH.width;
				win->winWH.height     = ((picObj->height+picObj->dst_y) > win->winWH.height) ? (picObj->height+picObj->dst_y) : win->winWH.height;

				//GDMA_PRINT(8,  "kkk ratio:%d addr:0x%x\n", win->tfbc_format.tfbc_req_lossy, picObj->address );

			}


		#if 0  /* check OSD over panel resolution */
			/*
			rtd_pr_gdma_debug(KERN_EMERG" [GDMA] picObj->dst_x=%d, picObj->dst_width=%d, picObj->width=%d ! \n ", picObj->dst_x, picObj->dst_width, picObj->width);
			rtd_pr_gdma_debug(KERN_EMERG" [GDMA] picObj->dst_y=%d, picObj->dst_height=%d, picObj->height=%d ! \n ", picObj->dst_y, picObj->dst_height, picObj->height);
			*/
		#endif// 0. endof check OSD over panel resolution

#if !defined(CONFIG_ARM64)
	dsb();
#endif


			GDMAEXT_DEBUG( "%s disPlane=%d win->attr.compress=%d pqdcEn:%d\n", __FUNCTION__, disPlane, win->attr.compress, pic->pqdc_enable );
			GDMAEXT_DEBUG( " pic->seg_num = %d, pic->context = %d\n",  pic->seg_num, pic->context);
			GDMAEXT_DEBUG( " win->winWH.width = %d, win->winWH.height= %d\n",  win->winWH.width, win->winWH.height);
			GDMAEXT_DEBUG( " win->dst_width = %d, win->dst_height= %d\n",  win->dst_width, win->dst_height);
			GDMAEXT_DEBUG( " win->pitch = %d,  win->attr.compress = %d\n",  win->pitch, win->attr.compress);
			GDMAEXT_DEBUG(" win addr: 0x%x 0x%x 0x%x 0x%x\n", win->top_addr, win->bot_addr, win->fbdc3_addr, win->fbdc4_addr);
			GDMAEXT_DEBUG(" picObj->alpha=0x%x, pic->show = %d\n",  picObj->alpha, pic->show);
			GDMAEXT_DEBUG( "%s, pic->syncstamp=%lld, matchnum=%d \n", __FUNCTION__, pic->syncInfo.syncstamp, pic->syncInfo.matchNum);
			

			gdma->picCount[disPlane]++;
			gdma->picQwr[disPlane] = idxQwr;
			GDMAEXT_LOG("%s, gdma->picQwr[%d]=%d \n", __FUNCTION__, disPlane, gdma->picQwr[disPlane]);

			/*  push work to workqueue */
			sGdmaReceiveWork[emptyIdx].disPlane = disPlane;
			sGdmaReceiveWork[emptyIdx].picQwr = idxQwr;
			/*
			*	Does NOT use workqueue mechanism to avoid system block sometimes
			*	driver calculates the register value immediately
			*/
			GDMAExt_PreUpdate(&(sGdmaReceiveWork[emptyIdx]));

#ifdef TRIPLE_BUFFER_SEMAPHORE
			gSemaphore[disPlane]--;
			if (gSemaphore[disPlane] < 0) {
				rtd_pr_gdma_debug(KERN_EMERG"gdma : gSemaphore[%d] %d\n", disPlane, gSemaphore[disPlane]);
				gSemaphore[disPlane] = 0;
			}

			if (gSemaphore[disPlane] == 0)
				if (!gdma->pendingRPC[disPlane])
					gdma->pendingRPC[disPlane] = 1;
#endif

			#if 0
			if (rtd_inl(SYS_REG_INT_CTRL_SCPU_reg) & SYS_REG_INT_CTRL_SCPU_osd_int_scpu_routing_en_mask) 
			{
				GDMA_Update(gdma, 1);
			}
			else {
				#if 0
				for (idx = 0; idx < GDMA_MAX_PIC_Q_SIZE; idx++)
					sGdmaReceiveWork[idx].used = 0;
				#endif//
				
				gdma->curPic[disPlane] = gdma->picQwr[disPlane];
				gdma->picQrd[disPlane] = ((gdma->curPic[disPlane] - 2) & (GDMA_MAX_PIC_Q_SIZE - 1));
				gdma->picCount[disPlane] = 0;
			}
			#endif//

		}
	}

	#if 1 //kkk  TODO future
		//to set correct curPic (need reference old GDMA_Update code )


	#endif//



	#if 1 //kkk test TODO, move this 
	//for (ilayer = 0; ilayer < data->layer_num; ilayer++) 
	{
		#if 0 
		if ( ! rtd_inl(SYS_REG_INT_CTRL_SCPU_reg) & SYS_REG_INT_CTRL_SCPU_osd_int_scpu_routing_en_mask) 
		{
			// FIXME_TODO check different intr for O4 / O5

			//intr disabled, don't send this pic ?
			foreach plane's xxx_idx
			{
				sGdmaReceiveWork[xxx].used = 0;
			}
				
				gdma->curPic[disPlane] = gdma->picQwr[disPlane];
				gdma->picQrd[disPlane] = ((gdma->curPic[disPlane] - 2) & (GDMA_MAX_PIC_Q_SIZE - 1));
				gdma->picCount[disPlane] = 0;

			continue;
		}
		#endif//

		GDMAExt_Update(gdma);
		

	}

	#endif//0




	GDMA_EXT_LOG(8, "%s, layer loop done \n", __FUNCTION__);
	GDMA_EXIT_CRITICAL();
	up(&gdma->sem_receiveExt);

#ifdef TRIPLE_BUFFER_SEMAPHORE
	GDMA_EXT_LOG(8, "%s, online updated picture MaxNum=%d  \n", __FUNCTION__, onlineMaxNum);

	{
		GDMAEXT_OSD_MODE mode = GDMAExt_getOSDMode();

		disPlane = onlinePlane[0];
		 
		sema_ret = down_timeout(&gdma->updateSem[disPlane], msecs_to_jiffies(32));

		timeout = jiffies + (HZ/50);	/* 20 ms */


		if ( (gDTG_APP_TYPE_osd ==  DTG_DUAL_DISPLAY)  ||
			 ( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD4) && onlineMaxNum==1 )
			)
		{
			
			while ( rtd_inl(GDMA4_CTRL4_reg) && (disPlane==GDMA_PLANE_OSD4)) {
				if ( saveBufAddr == rtd_inl(GDMA4_DBG_OSD_WI_24_reg) )
					break;

				if (time_before(jiffies, timeout) == 0)
					break;

				gdma_usleep(1000);
			}

			if (sema_ret == -ETIME) {
				gdma->pendingRPC[disPlane] = 0;
				gSemaphore[disPlane]  = gSEMAPHORE_MAX;
			}

			//GDMA_EXT_LOGT(GDMA_EXT_LOG_DUMP, "o4Addr 0x%lx 0x%lx\n", saveBufAddr, rtd_inl(GDMA4_DBG_OSD_WI_24_reg) );
			
		}
		else if ( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD5) && onlineMaxNum==1 ) 
		{
			while ( rtd_inl(GDMA5_CTRL5_reg) && (disPlane==GDMA_PLANE_OSD5)) {
				
				if ( saveBufAddr == rtd_inl(GDMA5_DBG_OSD5_WI_24_reg) )
					break;

				if (time_before(jiffies, timeout) == 0)
					break;

				gdma_usleep(1000);
			}

			//GDMA_EXT_LOGT(GDMA_EXT_LOG_DUMP, "o5Addr 0x%lx 0x%lx\n", saveBufAddr, rtd_inl(GDMA5_DBG_OSD5_WI_24_reg) );

		} 
		else
		{  // [FIXME] leave and ack fence in isr



		}
		

	}
	//GDMA_EXIT_CRITICAL();

	//up(&gdma->sem_receiveExt);
#else
	GDMA_EXIT_CRITICAL();
	//up(&gdma->sem_receiveExt);
#endif

	GDMA_EXT_LOG(7, "%s, finish \n", __FUNCTION__);
	return GDMA_SUCCESS;
 
}

int GDMAExt_ReceiveGraphicLayers_exp(GRAPHIC_LAYERS_OBJECT_V3 *layers)
{
	int err = -2;
	int64_t cur_expectedPresentTime;
	int64_t startWait_vsyncTime;
	int64_t diff_to_last ;
	

	if( layers->version != GDMA_GRAPHIC_LAYERS_OBJECT_V3_VER ||
		layers->size != sizeof(GRAPHIC_LAYERS_OBJECT_V3) ) 
	{
		rtd_pr_gdma_crit("invalid ReceiveGraphicLayers_exp ver:0x%x 0x%x %d %d\n", 
			layers->version, GDMA_GRAPHIC_LAYERS_OBJECT_V3_VER, layers->size , sizeof(GRAPHIC_LAYERS_OBJECT_V3)
			);
		
		return -1;
	}
	
	cur_expectedPresentTime = layers->expectedPresentTime;

	startWait_vsyncTime = gOSD1_VSYNC_USER_TIME;  // gOSD1_VSYNC_USER_TIME is ns
	gGDMA_StartWait_VsyncTime = startWait_vsyncTime;

	diff_to_last = cur_expectedPresentTime - startWait_vsyncTime;


	gGDMA_EXPTime_StartWait = ktime_to_ns( ktime_get() );
	
	//if( (diff_to_last ) <= (gVsyncPeriod_NS + (gVsyncPeriod_NS/2) )  ) 
	if( (diff_to_last) <= ( gVsyncPeriod_NS *2   )  ) 
	{
		gGDMA_EXPTime_Send =  gGDMA_EXPTime_StartWait ;
		
		err = GDMAExt_startHandleLayers(layers);

			print_log_expTme(GDMA_EXPT_TAG_WAIT_VSYNC0, 9, cur_expectedPresentTime); //put after GDMA_ReceiveGraphicLayers

		gGDMA_LAST_EXP_Time = cur_expectedPresentTime;



		return err;
		
	}

	
	{
		long rrr = 0;
	
		ktime_t ktime_1;
		ktime_t ktime_2;
		s64 wait_time_us ;
		unsigned int timeout_microSec = 34000;
		const unsigned int timeout_warn_us = 21000;
		unsigned long vsync_lock_flags;
	
		unsigned long timeout_val = usecs_to_jiffies(timeout_microSec);


	do {

	
		//if( 
			
		spin_lock_irqsave(&expTime_completion_lock, vsync_lock_flags);
	
			reinit_completion( &gdma_expTime_completion );  //[FIXME] ? how to prevent isr called twice ?
					
		spin_unlock_irqrestore(&expTime_completion_lock, vsync_lock_flags);
	
		ktime_1 = ktime_get();		
	
		rrr = wait_for_completion_interruptible_timeout( &gdma_expTime_completion, timeout_val);
	
		ktime_2 = ktime_get();
		wait_time_us =	ktime_us_delta(ktime_2, ktime_1); //us == micro second
	
		if( rrr == 0 )
		{
			// timeout
	
			if( timeout_warn_us < (unsigned int)wait_time_us )	{
						//timeout + signaled, warning only 
					VSYNC_PRINT(3, " warn waitVsync long? %d  diffT:%lld\n",
								timeout_microSec, wait_time_us
				);
	
			}
					
		}	
		else if( rrr == -ERESTARTSYS ) 
		{
			//interrupted 
			VSYNC_PRINT(3, "waitVsync break intr %lld \n", wait_time_us);
		}


		//diff_to_last = cur_expectedPresentTime - ktime_to_ns(ktime_2);
		diff_to_last = cur_expectedPresentTime - ktime_to_ns(ktime_2);

		GDMA_EXPTIME_LOG(9, "\twakeup(us) %lld w:%lld(us)\n", diff_to_last/1000, wait_time_us );
			
		//if( (cur_expectedPresentTime - last_vsyncTime ) <= (gVsyncPeriod_NS + (gVsyncPeriod_NS/2) )  ) 
		if( (diff_to_last) <= ( gVsyncPeriod_NS  )  ) 
		{

			break;
		}		

		// if > 1 sec, break immediately

	 }while( 1 ) ;

	
		//check presentation time
	
	}
	
	gGDMA_EXPTime_Send =  ktime_to_ns( ktime_get() );

	err = GDMAExt_startHandleLayers(layers);

		print_log_expTme(GDMA_EXPT_TAG_WAIT_VSYNCX, 9, cur_expectedPresentTime );	 //put after call GDMA_ReceiveGraphicLayers()

	gGDMA_LAST_EXP_Time = cur_expectedPresentTime;

	return err;

}

void ext_dump_PICTURE_OBJECT(int layer_idx, GDMA_PICTURE_OBJECT*  pic )
{
	GDMAEXT_DUMP("\tdumpPic%d: 0x%lx addr:0x%lx osd:%d type:%d, fmt:0x%x(0x%x) [%d %d %d %d %d] dst:[%d %d %d %d] \n", layer_idx
		, pic, pic->address
		,pic->plane, pic->src_type, pic->format, pic->fbdc_format, pic->x, pic->y, pic->width, pic->height, pic->pitch
		,pic->dst_x, pic->dst_y, pic->dst_width, pic->dst_height );

	GDMAEXT_DUMP("\t\t comp:%d ratio:%d decmp:%d a:0x%x, lay:%d, scale:%d\n", 
		pic->compressed, pic->compressed_ratio, pic->decompress, pic->alpha
		,pic->picLayout, pic->scale_factor);
	GDMAEXT_DUMP("\t\t used:%d show:%d palete 0x%x %d rgb:%d order:%d\n", 
		pic->layer_used, pic->show, pic->paletteformat, pic->paletteIndex
		,pic->rgb_order, pic->offlineOrder );

} 

void ext_dump_LAYERS_OBJECT(PICTURE_LAYERS_OBJECT* layer_all )
{
	int i;
	COMPOSE_PICTURE_OBJECT* com_picObj=NULL;
	GDMA_PICTURE_OBJECT* picObj=NULL;
	
	GDMAEXT_DUMP("DumpMiddlelayers 0x%lx num:%d cook:0x%x", layer_all, layer_all->layer_num, layer_all->cookie);

	for(i =0; i < layer_all->layer_num; ++i)
	{
		com_picObj = &layer_all->layer[i];

		if (com_picObj->normal_num > 0)
			picObj = &com_picObj->normal[0];		/* just one normal OSD for one online path  */

		else if (com_picObj->fbdc_num > 0)
			picObj = &com_picObj->fbdc[0];
		else {
			GDMAEXT_DUMP(  "%s, com_picObj:%p no layer \n", __FUNCTION__, com_picObj);
		}

		GDMAEXT_DUMP("  layerTop:0x%lx  norm:%d fbc:%d\n", com_picObj, com_picObj->normal_num, com_picObj->fbdc_num );

		ext_dump_PICTURE_OBJECT(i, picObj);
	}
   

}
// new core function, can replace GDMAExt_ReceiveGraphicLayers_exp(GRAPHIC_LAYERS_OBJECT_V3 *data)
int GDMAExt_startHandleLayers( GRAPHIC_LAYERS_OBJECT_V3*  data )
{
	int err;

	PICTURE_LAYERS_OBJECT*  sendGdmaLayerObj = NULL;
	PICTURE_LAYERS_OBJECT* picLayerObj = NULL;


	if(g_osdExt_stop_update) {
		static int cc = 0;


		if(++cc == 500 ) {

			GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "osdExt stop update:%d \n", g_osdExt_stop_update); 
		}

		goto SIGNAL_FENCE_LABEL;
	}

	if (sendGdmaLayerObj == NULL)
		sendGdmaLayerObj = (PICTURE_LAYERS_OBJECT *)kmalloc(sizeof(PICTURE_LAYERS_OBJECT), GFP_KERNEL);

	//if (picLayerObj == NULL || sendGdmaLayerObj == NULL ) {
	if (  sendGdmaLayerObj == NULL ) {

		GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "%s, kmalloc %p %p failed... \n", __FUNCTION__, sendGdmaLayerObj, picLayerObj);
		return -100;
	}

	// memset(picLayerObj, 0x0, sizeof(PICTURE_LAYERS_OBJECT));
	memset(sendGdmaLayerObj, 0x0, sizeof(PICTURE_LAYERS_OBJECT));


	err = GDMAExt_ReceiveGraphicLayers(data, sendGdmaLayerObj);

	if( unlikely(gDebugGDMAExt_loglevel >= GDMA_EXT_LOG_ALL) ) 
		ext_dump_LAYERS_OBJECT(sendGdmaLayerObj);

	#if 0
		if (picObj->decompress) {

				gdma_config_line_buffer_afbc(FBC_ON);
		} else {
			gdma_config_line_buffer_afbc(FBC_OFF);  // FBC_OFF is not used
		}
	#endif//0



   if ( sendGdmaLayerObj->layer_num > 0) {
		err = GDMAExt_ReceivePicture( sendGdmaLayerObj );
	}

	if ( sendGdmaLayerObj ) {
		kfree( sendGdmaLayerObj );
	}


	//can't read sendGdmaLayerObj below !


    SIGNAL_FENCE_LABEL:

	    // to signal gdma fence
	// [TODO][FIXME]
    if (VALID_GDMA_MAGIC_KEY(data->gdma_fence_magic_key))
    {
        if (data->gdma_fence_handle) {
			//[FIXME] kkk  ()
            GDMA_SignalFence((struct gdma_fence *) ((uintptr_t)data->gdma_fence_handle));

			#ifdef GDMA_ENABLE_EXP_TIME

				gGDMA_PIC_LAST_SIGNAL_Time = ktime_to_ns( ktime_get() );
			#endif//
            //rtd_pr_gdma_emerg("fence signal %llu\n", data->gdma_fence_handle);
            
        }
    }
     else { 
		GDMA_EXT_LOG(3, "gdma magic key FAIL! (0x%8x)?\n", data->gdma_fence_magic_key);
   	}

	return 0;
}

// no check g_osdExt_stop_update
int GDMAExt_startHandleLayers_internal( GRAPHIC_LAYERS_OBJECT_V3*  data )
{
	int err;

	PICTURE_LAYERS_OBJECT*  sendGdmaLayerObj = NULL;
	PICTURE_LAYERS_OBJECT* picLayerObj = NULL;


	sendGdmaLayerObj = (PICTURE_LAYERS_OBJECT *)kmalloc(sizeof(PICTURE_LAYERS_OBJECT), GFP_KERNEL);

	//if (picLayerObj == NULL || sendGdmaLayerObj == NULL ) {
	if (  sendGdmaLayerObj == NULL ) {

		GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "%s, kmalloc %p %p failed... \n", __FUNCTION__, sendGdmaLayerObj, picLayerObj);
		return -100;
	}

	// memset(picLayerObj, 0x0, sizeof(PICTURE_LAYERS_OBJECT));
	memset(sendGdmaLayerObj, 0x0, sizeof(PICTURE_LAYERS_OBJECT));


	err = GDMAExt_ReceiveGraphicLayers(data, sendGdmaLayerObj);

	if( unlikely(gDebugGDMAExt_loglevel >= GDMA_EXT_LOG_ALL) ) 
		ext_dump_LAYERS_OBJECT(sendGdmaLayerObj);

	#if 0
		if (picObj->decompress) {

				gdma_config_line_buffer_afbc(FBC_ON);
		} else {
			gdma_config_line_buffer_afbc(FBC_OFF);  // FBC_OFF is not used
		}
	#endif//0



   if ( sendGdmaLayerObj->layer_num > 0) {
		err = GDMAExt_ReceivePicture( sendGdmaLayerObj );
	}

	if ( sendGdmaLayerObj ) {
		kfree( sendGdmaLayerObj );
	}
	//can't read sendGdmaLayerObj below !


	    // to signal gdma fence
	// [TODO][FIXME]
    if (VALID_GDMA_MAGIC_KEY(data->gdma_fence_magic_key))
    {
        if (data->gdma_fence_handle) {
			//[FIXME] kkk  ()
            GDMA_SignalFence((struct gdma_fence *) ((uintptr_t)data->gdma_fence_handle));

			#ifdef GDMA_ENABLE_EXP_TIME

				gGDMA_PIC_LAST_SIGNAL_Time = ktime_to_ns( ktime_get() );
			#endif//
            //rtd_pr_gdma_emerg("fence signal %llu\n", data->gdma_fence_handle);
            
        }
    }
     else { 
		GDMA_EXT_LOG(3, "gdma magic key FAIL! (0x%8x)?\n", data->gdma_fence_magic_key);
	}




	return 0;
}

int GDMAExt_ReceiveGraphicLayers(GRAPHIC_LAYERS_OBJECT_V3 *data,  PICTURE_LAYERS_OBJECT*  sendGdmaLayerObj )
{
	gdma_dev *gdma = &gdma_devices[0];
	
	int err = -50;
	int loop = 0, gdmaLayerNum = 0;
	int  fbdcIdx = 0, normalIdx = 0;
	COMPOSE_PICTURE_OBJECT *com_picObj = NULL;
	//, *com2_picObj = NULL;
	//static PICTURE_LAYERS_OBJECT *picLayerObj = NULL;
	PICTURE_LAYERS_OBJECT *picLayerObj = NULL;
	
	GDMA_PICTURE_OBJECT *layer_obj = NULL;
	
	
	u64 syncstamp_tmp;

	int hasDecompress = 0, hasCompress = 0;

	int layerIdx = 0 ;
	VO_GRAPHIC_PLANE onlinePlane = VO_GRAPHIC_OSD1;

	//int num4k2kFBDC = 0, num2k1kFBDC = 0, num4k2kNormal = 0, num2k1kNormal = 0, start2k1kIdx = -1;

	//VO_RECTANGLE disp_res;
	//getDispSize(&disp_res);

	if (data->layer_num < 0 || data->layer_num > GDMA_LAYER_NUM) {
		GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "%s, invalid #layers:%d \n", __FUNCTION__, data->layer_num );
		return -100;
	}

	GDMA_EXT_LOG(GDMA_EXT_LOG_ALL, "%s ++ \n", __FUNCTION__  );


	down(&gdma->sem_gfxreceive);


	if (picLayerObj == NULL)
		picLayerObj = (PICTURE_LAYERS_OBJECT *)kmalloc(sizeof(PICTURE_LAYERS_OBJECT), GFP_KERNEL);



	if (picLayerObj == NULL || sendGdmaLayerObj == NULL ) {
		up(&gdma->sem_gfxreceive);
		GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "%s, kmalloc failed... \n", __FUNCTION__);
		return -100;
	}

	/* get a random syncstamp */
	syncstamp_tmp = get_random_int();
	//DBG_PRINT(KERN_EMERG"%s, random syncstamp = %lld \n", __FUNCTION__, syncstamp_tmp);

	/* memset(&osdcomp_picObj,0x0,sizeof(GRAPHIC_LAYERS_OBJECT)); */

	memset(picLayerObj, 0x0, sizeof(PICTURE_LAYERS_OBJECT));
	//memset(sendGdmaLayerObj, 0x0, sizeof(PICTURE_LAYERS_OBJECT));  //set in caller

	/*  collect & check all layers information */
	for (loop = 0; loop < data->layer_num; loop++) {

		
		#if 0  //don't check, bcos doesn't check osd layer here
		if (disp_res.width == FOURK_W && disp_res.height == TWOK_H && data->layer[loop].scale_factor == 1) {

			if (data->layer[loop].src_type == SRC_NORMAL) {
				/*rtd_pr_gdma_debug(KERN_EMERG"%s, Layer[%d] 4K2K layer is NOT FBDC type \n", __FUNCTION__, loop);*/
				num4k2kNormal++;
			}
			else if (data->layer[loop].src_type == SRC_FBDC) {
				num4k2kFBDC++;
			}
		}
		/*  related to 2K1K or below */
		else if (data->layer[loop].width <= TWOK_W && data->layer[loop].height <= ONEK_H) {
			if (start2k1kIdx == -1)
				start2k1kIdx = loop;
			if (data->layer[loop].src_type == SRC_FBDC)
				num2k1kFBDC++;
			else if (data->layer[loop].src_type == SRC_NORMAL)
				num2k1kNormal++;
		}
		#endif//

		/* check compress picture */
		if (data->layer[loop].compressed)
			hasCompress = 1;

		GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, " Layer[%d] type %d compress %d \n",  loop, data->layer[loop].src_type, data->layer[loop].compressed);
	}

	/* force sticking syncstamp except for compress layers */


		//set default:
	


	// check && set basic info
	for (loop = 0; loop < data->layer_num; loop++)
	{

		#if 0
		/* only 3 normal layers which sets in OSD1/2/4, driver could NOT switch OSDSRx_switch_to register */
		if (data->layer_num == 3 && loop == 2 && data->layer[loop].src_type == SRC_NORMAL) {
			onlinePlane = VO_GRAPHIC_OSD4;
			normalIdx = 0;
			layerIdx++;
		}
		#endif//

		onlinePlane = (VO_GRAPHIC_PLANE)(data->layer[loop].param_osd.osd_plane);
	
		if (data->layer[loop].src_type == SRC_FBDC) {

			GDMAExt_CopyLayerInfo_V3(&picLayerObj->layer[layerIdx].fbdc[fbdcIdx], &data->layer[loop]);
			
			 {
				picLayerObj->layer[layerIdx].fbdc[fbdcIdx].plane = onlinePlane;
				picLayerObj->layer[layerIdx].onlineOrder = (E_BLEND_ORDER)layerIdx;
			}

			picLayerObj->layer[layerIdx].fbdc_num++;

			hasDecompress = 1;
		}
		else if (data->layer[loop].src_type == SRC_NORMAL
		   || data->layer[loop].src_type == SRC_NORMAL_1
		   || data->layer[loop].src_type == SRC_NORMAL_PQDC 
		   ){
		   
			GDMAExt_CopyLayerInfo_V3(&picLayerObj->layer[layerIdx].normal[normalIdx], &data->layer[loop]);

			{
				picLayerObj->layer[layerIdx].normal[normalIdx].plane = onlinePlane;
				
				#if 0
				if(data->layer[loop].src_type == SRC_NORMAL_1)
					picLayerObj->layer[layerIdx].onlineOrder = C2;
				else
					picLayerObj->layer[layerIdx].onlineOrder = (E_BLEND_ORDER)layerIdx;
				#endif//

			}

			picLayerObj->layer[layerIdx].normal_num++;
		}
		else {

				GDMAEXT_ERROR("idx %d, invalid srcType\n", loop, data->layer[loop].src_type); 

		}


		++layerIdx;
	}

	
	picLayerObj->layer_num = layerIdx;

	if( unlikely(gDebugGDMAExt_loglevel >= GDMA_EXT_LOG_ALL) ) {
		GDMAEXT_TMP("dump tmp layerInfo 0x%x\n", picLayerObj);
		ext_dump_LAYERS_OBJECT(picLayerObj);
	}


	/*
	 *  if picture want to compress or blend, then go osd_comp_receive_picture
	 *  else call GDMA_ReceivePicture
	 */
	
	GDMAEXT_TMP("  start proc middle layers :%d \n", __LINE__ );

	for (loop = 0; loop < picLayerObj->layer_num; loop++)
	{
		com_picObj = &picLayerObj->layer[loop];

		
		if (com_picObj->fbdc_num > 0) {
			layer_obj = &com_picObj->fbdc[0];

			GDMAEXT_TMP("fbdc_num=%d\n", com_picObj->fbdc_num);
		}
		else if (com_picObj->normal_num > 0) {
			layer_obj = &com_picObj->normal[0];

			GDMAEXT_TMP("norm num=%d\n", com_picObj->normal_num);

		}
		else 
			GDMAEXT_TMP(" Eerror no have fbdc/normal layer %d  \n", loop);

		
			// sendCompLayerObj->layer[compLayerNum] = picLayerObj->layer[loop];
		//sendGdmaLayerObj->layer[gdmaLayerNum] = picLayerObj->layer[loop];

		memcpy(&(sendGdmaLayerObj->layer[gdmaLayerNum]), &(picLayerObj->layer[loop]), sizeof(sendGdmaLayerObj->layer[gdmaLayerNum]) );


		gdmaLayerNum++;

		GDMAEXT_TMP("  End of layers :%d \n", loop);
	}
	

	sendGdmaLayerObj->layer_num = gdmaLayerNum;



	/* only for HW cursor control and go online path */
	sendGdmaLayerObj->cookie = data->cookie;
	
	up(&gdma->sem_gfxreceive);

	#if 0
	if (gdmaLayerNum > 0){
		err = GDMA_ReceivePicture(sendGdmaLayerObj);
		if(gdma_suspend_disable_osd == 1){			
			GDMA_ConfigOSDxEnableFast(GDMA_PLANE_OSD1, 1);
			gdma_suspend_disable_osd = 0;			
		}
	}
	#endif//0

	if (picLayerObj) {

		kfree( picLayerObj );
	}

	return err;

}


int GDMAExt_enable_Sync2OSD( int enable)
{

	gdma_dev *gdma = &gdma_devices[0];


	if(enable) {
		gdma->osd45_sync = 1;
	}
	else {

		gdma->osd45_sync = 0;

	}

	return 0;
}


int GDMAExt_enable_osdHW(int plane)
{
	int err = -1;

	OSD_FLOW_STATE osd4_cur, osd5_cur ;

	gdma_dev *gdma = &gdma_devices[0];

	int clk_en = 1;

  mutex_lock(&gOSD_MGR_LOCK);

	osd4_cur = gdma->osd4_status;
	osd5_cur = gdma->osd5_status;

	//[TODO] lock ?

	if( plane == GDMA_PLANE_OSD4)
	{ 
		//only enable clk in close mode
		if ( gdma->osd4_status == OSD_FLOW_CLOSE )
		{

			GDMAExt_ClkEnable(GDMA_PLANE_OSD4, clk_en);

			gdma->osd4_status = OSD_FLOW_NORMAL;

			GDMAEXT_DEBUGT(" enable osd4 when in status:%d \n", osd4_cur);

			err = 0;
		}
	}
	else if( plane == GDMA_PLANE_OSD5 )
	{
		if ( gdma->osd5_status == OSD_FLOW_CLOSE )
		{
			GDMAEXT_DEBUGT("enable osd4 when in status:%d \n", osd5_cur);

			GDMAExt_ClkEnable(GDMA_PLANE_OSD5, clk_en);

			gdma->osd5_status = OSD_FLOW_NORMAL;

			err = 0;
		}
	}

	
  mutex_unlock(&gOSD_MGR_LOCK);


	return err;

}

int GDMAExt_disalbe_osdHW(int plane)
{
	int err= 0;

	gdma_dev *gdma = &gdma_devices[0];
	OSD_FLOW_STATE osd4_cur, osd5_cur ;

  mutex_lock(&gOSD_MGR_LOCK);

	osd4_cur = gdma->osd4_status;
	osd5_cur = gdma->osd5_status;


	//[TODO] lock ?

	if( plane == GDMA_PLANE_OSD4)
	{
		if ( gdma->osd4_status != OSD_FLOW_CLOSE )
		{
			while( gdma->osd4_status == OSD_FLOW_TEMP) 
			{
				gdma_usleep(100); //sleep 0.5 ms

				GDMAEXT_DEBUGT(" wait osd4 to close :%d \n", gdma->osd4_status );
			} 

			gdma->osd4_status = OSD_FLOW_TO_CLOSE;
			GDMAEXT_DEBUGT(" close osd4 when in status:%d \n", osd4_cur);

			GDMAExt_ClkEnable(GDMA_PLANE_OSD4, 0);

			gdma->osd4_status = OSD_FLOW_CLOSE;
		}
	}
	else if( plane == GDMA_PLANE_OSD5 )
	{
		if ( gdma->osd5_status != OSD_FLOW_CLOSE )
		{
			while( gdma->osd5_status == OSD_FLOW_TEMP) 
			{
				gdma_usleep(100); //sleep 0.5 ms

				GDMAEXT_DEBUGT(" wait osd5 to close :%d \n", gdma->osd5_status );
			} 

			gdma->osd5_status = OSD_FLOW_TO_CLOSE;
			GDMAEXT_DEBUGT(" close osd5 in status:%d \n", osd5_cur);
			
			GDMAExt_ClkEnable(GDMA_PLANE_OSD5, 0);

			gdma->osd5_status = OSD_FLOW_CLOSE;
		}
	}

	
  mutex_unlock(&gOSD_MGR_LOCK);


	return err;

}

GDMAEXT_OSD_MODE GDMAExt_getOSDMode(void )
{
	return g_osdMode_send;

}

void GDMAExt_dumpOSDMode(void )
{
	GDMAEXT_OSD_MODE mode = GDMAExt_getOSDMode();

	if( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD1) ) {
		rtd_pr_gdecmp_emerg("o1");
	}

	if ( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD_DUAL) ) {
		rtd_pr_gdecmp_emerg(" dual");
	}
	else {

		if ( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD4) ) {
			rtd_pr_gdecmp_emerg(" o4");
		}

		if ( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD5 ) ) {
			rtd_pr_gdecmp_emerg(" o5");
		}


	}

	

}


void GDMAExt_setOSDMode( GDMAEXT_OSD_MODE mode)
{

	//mutex lock ??

	g_osdMode_send = mode;

	if( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD4) ) {
		#if 0
		OSD_RTD_OUTL(0xB8028F04, 0x000008C9);
		OSD_RTD_OUTL(0xB8028F08, 0x112F112F);
		OSD_RTD_OUTL(0xB8028F0C, 0x002D089D);
		OSD_RTD_OUTL(0xB8028F10, 0x01181018);
		OSD_RTD_OUTL(0xB8028F00, 0x90000011);
		
		
		OSD_RTD_OUTL(0xB8028700, 0x90000010);
		OSD_RTD_OUTL(0xB8028F80, 0x90000010);
		OSD_RTD_OUTL(0xB80280B0, 0x00000001);
		#endif//
	}
	else if ( GDMAOSD_MOD_HAS_MODE(mode,GDMAEXT_OSD_MODE_OSD5) ) {
		#if 0
		OSD_RTD_OUTL(0xB8028F84, 0x000008C9);
		OSD_RTD_OUTL(0xB8028F88, 0x112F112F);
		OSD_RTD_OUTL(0xB8028F8C, 0x002D089D);
		OSD_RTD_OUTL(0xB8028F90, 0x01181018);
		OSD_RTD_OUTL(0xB8028F80, 0x90000011);

		OSD_RTD_OUTL(0xB8028700, 0x90000010);
		OSD_RTD_OUTL(0xB8028F00, 0x90000010);
		OSD_RTD_OUTL(0xB80280B0, 0x00000002);
		#endif//
	}

	GDMAEXT_DUMP("setOSDMode:0x%x", mode);

}



int GDMAExt_SwitchVsyncSource(GDMA_DISPLAY_PLANE plane )
{
	int err = 0;
	int new_vsyncSource = 0; 

	GDMAEXT_OSD_MODE mode = GDMAExt_getOSDMode();


	if ( plane == GDMA_PLANE_OSD1) {

		if ( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD1) == 0 )  {
			GDMAEXT_ERROR("invalid vsync src to %d  mode:%d\n", plane, mode );
			goto ERR_SET_SOURCE;

		}

		new_vsyncSource = 0;
	}
	else if ( plane == GDMA_PLANE_OSD4 ) {

		if ( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD4) == 0 )  {
			GDMAEXT_ERROR("invalid vsync src to %d  mode:%d\n", plane, mode );
			goto ERR_SET_SOURCE;
		}

		new_vsyncSource = 4;
	}
	else if ( plane == GDMA_PLANE_OSD5 ) {

		if ( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD5 ) == 0 )  {
			GDMAEXT_ERROR("invalid vsync src to %d  mode:%d\n", plane, mode );
			goto ERR_SET_SOURCE;
		}

		new_vsyncSource = 5;
	} else {
		GDMAEXT_ERROR("invalid vsync src plane %d  mode:%d\n", plane, mode );
		goto ERR_SET_SOURCE;
	}

	

	mutex_lock(&gOSD_VSYNC_SOURCE_LOCK);

	gVsyncSource_id = new_vsyncSource;

	mutex_unlock(&gOSD_VSYNC_SOURCE_LOCK);
	

  ERR_SET_SOURCE:
 
	return err;
}


int GDMAExt_StoreRegisterData(GDMA_DISPLAY_PLANE plane )
{

	// osd_ctrl_data[GDMA_PLANE_OSD1] = rtd_inl(GDMA_OSD1_CTRL_reg);
	


	return 0;
}

int GDMAExt_RestoreRegisterData(GDMA_DISPLAY_PLANE plane )
{



	return 0;

}


int GDMAExt_Suspend(GDMA_DISPLAY_PLANE plane )
{
	int err = 0;




	GDMAExt_init_interrupt( plane, 0 );



	GDMAExt_disalbe_osdHW(plane);



	return err;
}

int GDMAExt_Resume(GDMA_DISPLAY_PLANE plane )
{
	int err = 0;

	if( GDMAExt_enable_osdHW( plane ) == 0 ) {
		GDMAExt_init_interrupt( plane, 1);
	}




	return err;



}
