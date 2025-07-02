
#include <linux/interrupt.h>
#include <rbus/ppoverlay_reg.h>   // dtg
#include <rbus/gdma4_reg.h>
#include <rbus/osdovl4_reg.h>

#include <rbus/gdma5_reg.h>
#include <rbus/osdovl5_reg.h>
#include <rbus/osdtg45_reg.h>

#include <rtk_crt.h>

#ifdef RTK_GDMA_TEST_OSD1
#include <rbus/gdma_reg.h>
#include <rbus/ppoverlay_reg.h>
#endif

#include <gal/rtk_gdma_driver.h>  //gdma_dev
#include <rtk_kdriver/io.h>


#include <gal/rtk_gdma_export.h>
#include "rtk_osdExt_intr.h"
#include "rtk_osdExt_debug.h"

uint64_t gVSYNC_frame_index[GDMA_PLANE_MAXNUM]={0};

unsigned int g_dma_osdAcc_count[GDMA_PLANE_MAXNUM] = {0};
//unsigned int g_osd_acc_sync_print_num = 0 ;
unsigned int g_dma_osdUDF_count[GDMA_PLANE_MAXNUM] = {0};
unsigned int g_dma_osdOVF_count[GDMA_PLANE_MAXNUM] = {0};
unsigned int g_dma_tfbcAcc_count[GDMA_PLANE_MAXNUM] = {0};

unsigned int g_sr_udfl_count[GDMA_PLANE_MAXNUM] = {0};

unsigned int g_osd_intr_error[GDMA_PLANE_MAXNUM] = {0};



	extern unsigned int g_osd_acc_sync_num[GDMA_PLANE_MAXNUM];
	extern unsigned int g_osd_acc_sync_print_num ;
	extern unsigned int g_osd_udfl_num[GDMA_PLANE_MAXNUM] ;
	extern unsigned int g_sr_udfl_num[OSD_SR_MAXNUM] ;


static int g_IntrPrintCount = 100;

// #define RTK_GDMA_TEST_6748_OSD1
//#define RTK_GDMA_TEST_OSD_EXT //due to rbus name conflict. TEST_OSD1/TEST_OSD_EXT are mutual 


//#ifdef RTK_GDMA_TEST_OSD1
#if 1 

	#ifdef ENABLE_VSYNC_NOTIFY
	extern struct completion gdma_vsync_completion;

	extern spinlock_t vsync_completion_lock;

	extern int64_t gOSD1_VSYNC_USER_TIME;
	extern int64_t gOSD1_VSYNC_USER_TIME_PREV;


	//atomic64_t gOSD1_VSYNC_USER_TIME = ATOMIC64_INIT(0);
	//atomic64_t gOSD1_VSYNC_USER_TIME_PREV = ATOMIC64_INIT(0);
	#endif//

	#ifdef GDMA_ENABLE_EXP_TIME
		extern struct completion  gdma_expTime_completion ;
		extern spinlock_t expTime_completion_lock;
	#endif//

	//unsigned int gVsyncPeriod_NS;
	extern unsigned int gVsyncDiff_NS;

	//int64_t gGDMA_EXPTime_StartWait = 0;
	//int64_t gGDMA_StartWait_VsyncTime = 0;


	//int64_t gGDMA_EXPTime_Send = 0;
	//int64_t gGDMA_LAST_EXP_Time = 0;
	//int64_t gGDMA_PIC_LAST_SIGNAL_Time = 0;

	#ifdef TRIPLE_BUFFER_SEMAPHORE

		extern int gSEMAPHORE_MAX;
		extern int gSemaphore[GDMA_PLANE_MAXNUM];

	#endif

	extern int gDebugExp_loglevel;

	//SR debug
	#define PRINT_LIMIT_ACC  4

	

	#if IS_ENABLED(CONFIG_RTK_KDRIVER_SUPPORT)

		extern int pll_info_bush(void);
		extern int pll_info_disp(void);
	#endif//


#endif//


	#ifdef GDMA4_ENABLE_VSYNC_NOTIFY
	DECLARE_COMPLETION(gdma4_vsync_completion);

	DEFINE_SPINLOCK(gdma4_vsync_completion_lock);

	int64_t gGDMA4_VSYNC_USER_TIME;
	int64_t gGDMA4_VSYNC_USER_TIME_PREV;

	unsigned int gGDMA4_VsyncDiff_NS;

	//atomic64_t gOSD1_VSYNC_USER_TIME = ATOMIC64_INIT(0);
	//atomic64_t gOSD1_VSYNC_USER_TIME_PREV = ATOMIC64_INIT(0);
	#endif//GDMA4_ENABLE_VSYNC_NOTIFY

	#ifdef GDMA_ENABLE_EXP_TIME
		DECLARE_COMPLETION(gdma4_expTime_completion);
		DEFINE_SPINLOCK(gdma4_expTime_completion_lock);
	#endif//

	#ifdef GDMA5_ENABLE_EXP_TIME
		DECLARE_COMPLETION( gdma5_expTime_completion );
		DEFINE_SPINLOCK(expTime5_completion_lock);

	#endif//GDMA5_ENABLE_EXP_TIME

	//unsigned int gVsyncPeriod_NS;
	extern unsigned int gVsyncDiff_NS;



#ifdef RTK_GDMA_TEST_OSD1
irqreturn_t GDMA_OSD1_irq_handler(int irq, void *dev_id)
{
	/*volatile decomp_inten_RBUS decomp_inten_reg;*/
	volatile gdma_dma_intst_RBUS dma_intst_reg;
	gdma_dev *gdma = dev_id;


	volatile int osd1_vend = 0, osd1_vend_3D = 0, osd_vend = 0;/* , i; */
	volatile int osd1_finish = 0;
	volatile int osd1_sync = 0;
	volatile int osd1_acc_under = 0;


	/*int lineCnt = (*(volatile u32 *)GET_MAPPED_RBUS_ADDR(0xB8028248)) & 0xfff; */
	volatile unsigned int osd_intst_reg = rtd_inl(GDMA_OSD_INTST_reg);
	volatile unsigned int osd_intst_3d_reg = rtd_inl(GDMA_OSD_INTST_3D_reg);
	volatile unsigned int osd_inten_reg = rtd_inl(GDMA_OSD_INTEN_reg);
	volatile unsigned int osd_inten_3d_reg = rtd_inl(GDMA_OSD_INTEN_3D_reg);

#if defined(_TEST_CODE_GDMA_) || defined(_TEST_CODE_OSDCOMP_)
#if !defined(CONFIG_ARM64)
	flush_cache_all();
#endif
#endif
	if (GDMA_OSD_INTEN_get_osd1_fin(osd_inten_reg))
		osd1_finish = GDMA_OSD_INTST_get_osd1_fin(osd_intst_reg);
	
	if (GDMA_OSD_INTEN_get_osd1_vsync(osd_inten_reg))
		osd1_sync = GDMA_OSD_INTST_get_osd1_vsync(osd_intst_reg);

#ifndef USING_GDMA_VSYNC
	if (GDMA_OSD_INTEN_get_osd1_vact_end(osd_inten_reg))
		osd1_vend     = GDMA_OSD_INTST_get_osd1_vact_end(osd_intst_reg);
#else
	if (GDMA_OSD_INTEN_get_osd1_vsync(osd_inten_reg))
		osd1_vend     = GDMA_OSD_INTST_get_osd1_vsync(osd_intst_reg);
#endif
	if (GDMA_OSD_INTEN_3D_get_osd1_vact_end(osd_inten_3d_reg))
		osd1_vend_3D  = GDMA_OSD_INTST_3D_get_osd1_vact_end(osd_intst_3d_reg);
	
	osd_vend      = gdma->ctrl.enable3D ? (osd1_vend_3D || (osd1_vend && gdma->ctrl.forceUpdate)) : osd1_vend;

#ifdef USING_GDMA_VSYNC
	if (GDMA_CTRL_get_osd1_prog_done(rtd_inl(GDMA_CTRL_reg)) == 0)
#endif
	{

		if (osd_vend) {

#ifdef TRIPLE_BUFFER_SEMAPHORE
			int lineCnt = rtd_inl(PPOVERLAY_new_meas0_linecnt_real_reg) & 0x1fff;
			if (lineCnt < (rtd_inl(PPOVERLAY_DV_DEN_Start_End_reg) & 0x1fff))
				;/*DBG_PRINT(KERN_EMERG"gdma:vend isr @ %d\n", lineCnt);*/ /*  check interrupt timing in front-porch time? */
#endif


#if 0
			decomp_inten_reg.regValue = rtd_inl(GDMA_DECOMP_INTEN_reg);
			decomp_inten_reg.head_error_clr = 1;
			decomp_inten_reg.pix_fifo_underflow_clr = 1;
			rtd_outl(GDMA_DECOMP_INTEN_reg, decomp_inten_reg.regValue);
			decomp_inten_reg.head_error_clr = 0;
			decomp_inten_reg.pix_fifo_underflow_clr = 0;
			rtd_outl(GDMA_DECOMP_INTEN_reg, decomp_inten_reg.regValue);
#endif
			//GDMA_Update (gdma, 1);	// move to main process


#ifndef USING_GDMA_VSYNC
				rtd_outl(GDMA_OSD_INTST_reg, GDMA_OSD_INTST_write_data(0) | GDMA_OSD_INTST_osd1_vact_end(1));
#else
				rtd_outl(GDMA_OSD_INTST_reg, GDMA_OSD_INTST_write_data(0) | GDMA_OSD_INTST_osd1_vsync(1));
#endif

			gdma->ctrl.forceUpdate = 0;
		}

#ifdef TRIPLE_BUFFER_SEMAPHORE
		/*else if (osd1_sync) {*/
		if (osd1_sync) {

			#ifdef ENABLE_VSYNC_NOTIFY
				gOSD1_VSYNC_USER_TIME_PREV = gOSD1_VSYNC_USER_TIME;
				gOSD1_VSYNC_USER_TIME = ktime_to_ns (ktime_get());

				gVsyncDiff_NS = gOSD1_VSYNC_USER_TIME - gOSD1_VSYNC_USER_TIME_PREV;

			#endif
	
			//if (gSemaphore[GDMA_PLANE_OSD1] > 0 && gdma->pendingRPC[GDMA_PLANE_OSD1] && (GDMA_CTRL_get_osd1_prog_done(rtd_inl(GDMA_CTRL_reg)) == 0)) {
			if ( gdma->pendingRPC[GDMA_PLANE_OSD1] && (GDMA_CTRL_get_osd1_prog_done(rtd_inl(GDMA_CTRL_reg)) == 0)) {
				gdma->pendingRPC[GDMA_PLANE_OSD1] = 0;
				up(&gdma->updateSem[GDMA_PLANE_OSD1]);
			}

			//if (gSemaphore[GDMA_PLANE_OSD2] > 0 && gdma->pendingRPC[GDMA_PLANE_OSD2] && (GDMA_CTRL_get_osd2_prog_done(rtd_inl(GDMA_CTRL_reg)) == 0)) {
			if (  gdma->pendingRPC[GDMA_PLANE_OSD2] && (GDMA_CTRL_get_osd2_prog_done(rtd_inl(GDMA_CTRL_reg)) == 0)) {
				gdma->pendingRPC[GDMA_PLANE_OSD2] = 0;
				up(&gdma->updateSem[GDMA_PLANE_OSD2]);
			}
	

			rtd_outl(GDMA_OSD_INTST_reg, GDMA_OSD_INTST_write_data(0) | GDMA_OSD_INTST_osd1_vsync(1));


			#ifdef GDMA_ENABLE_EXP_TIME
			{
				unsigned long exp_lock_flags;
				spin_lock_irqsave(&expTime_completion_lock, exp_lock_flags);
				
				complete( &gdma_expTime_completion );
				
				spin_unlock_irqrestore(&expTime_completion_lock, exp_lock_flags);

			}
			#endif//GDMA_ENABLE_EXP_TIME
			
			#if ENABLE_VSYNC_NOTIFY
			{
				unsigned long vsync_lock_flags;
				spin_lock_irqsave(&vsync_completion_lock, vsync_lock_flags);

				complete( &gdma_vsync_completion );

				spin_unlock_irqrestore(&vsync_completion_lock, vsync_lock_flags);
			}
			#endif//ENABLE_VSYNC_NOTIFY
		}
#endif
		else {
			if (osd1_vend) {
#ifndef USING_GDMA_VSYNC
				rtd_outl(GDMA_OSD_INTST_reg, GDMA_OSD_INTST_write_data(0) | GDMA_OSD_INTST_osd1_vact_end(1));
#else
				rtd_outl(GDMA_OSD_INTST_reg, GDMA_OSD_INTST_write_data(0) | GDMA_OSD_INTST_osd1_vsync(1));
#endif
				gdma->ctrl.forceUpdate = 1;
			}
		}
	}
#ifdef USING_GDMA_VSYNC
	else {
		rtd_pr_gdma_debug(KERN_EMERG"OSD1 not prog_done\n");
	}
#endif

	if (osd_vend) {
		/* if (gdma->vsync_enable) */
		{
			//queue_work(psOsdSyncWorkQueue, &sOsdSyncWork);
		}
		if (gdma->GDMA_CallBack)
			gdma->GDMA_CallBack (dev_id, osd1_vend_3D);
#if IS_ENABLED(CONFIG_DRM_REALTEK)
		if(gdma->GDMA_IrqCallBack)
			gdma->GDMA_IrqCallBack(gdma->IrqCallBackData);
#endif
	}


	if (OSD_SR_OSD_SR_1_UDFLOW_get_osd_sr_in_udflow(rtd_inl(OSD_SR_OSD_SR_1_UDFLOW_reg))) {
		osd1_acc_under = 1;
		g_sr_udfl_num[OSD_SR_1]++;
#if IS_ENABLED(CONFIG_RTK_KDRIVER_SUPPORT)
		rtd_pr_gdma_debug(KERN_EMERG"GDMA: osd1_sr underflow, source GDMA_osd1, busH=%d Mhz, dclk=%d Mhz, num=%d\n",
			pll_info_bush(), pll_info_disp(), g_sr_udfl_num[OSD_SR_1]);
#endif
		rtd_outl(OSD_SR_OSD_SR_1_UDFLOW_reg, OSD_SR_OSD_SR_1_UDFLOW_osd_sr_in_udflow(1));
	}

	if (OSD_SR_OSD_SR_2_UDFLOW_get_osd_sr_in_udflow(rtd_inl(OSD_SR_OSD_SR_2_UDFLOW_reg))) {
		osd1_acc_under = 1;
		g_sr_udfl_num[OSD_SR_2]++;
#if IS_ENABLED(CONFIG_RTK_KDRIVER_SUPPORT)
		rtd_pr_gdma_debug(KERN_EMERG"GDMA: osd2_sr underflow, source GDMA_osd2, busH=%d Mhz, dclk=%d Mhz, num=%d\n",
			pll_info_bush(), pll_info_disp(), g_sr_udfl_num[OSD_SR_2]);
#else
		rtd_pr_gdma_debug(KERN_EMERG"GDMA: osd2_sr underflow, source GDMA_osd2, busH=%d Mhz, dclk=%d Mhz, num=%d\n",
			GDMA_BusH_DClk_info(0), GDMA_BusH_DClk_info(1), g_sr_udfl_num[OSD_SR_2]);
#endif

		rtd_outl(OSD_SR_OSD_SR_2_UDFLOW_reg, OSD_SR_OSD_SR_2_UDFLOW_osd_sr_in_udflow(1));
	}

	dma_intst_reg.regValue = rtd_inl(GDMA_DMA_INTST_reg);
	if (dma_intst_reg.osd1_acc_sync) {
		osd1_acc_under = 1;
		g_osd_acc_sync_num[GDMA_PLANE_OSD1]++;
		if (g_osd_acc_sync_print_num < PRINT_LIMIT_ACC){
			g_osd_acc_sync_print_num++;
			//if(Get_DISPLAY_PANEL_OLED_TYPE() == FALSE)
			rtd_pr_gdma_err(KERN_ERR"[GDMA][DMA.ERR] OSD-%d accident sync [%d]  num = %d!!\n", GDMA_PLANE_OSD1,g_osd_acc_sync_print_num,g_osd_acc_sync_num[GDMA_PLANE_OSD1]);
		}
		rtd_pr_gdma_debug(KERN_EMERG"GDMA: OSD1 accident sync! num = %d\n", g_osd_acc_sync_num[GDMA_PLANE_OSD1]);
	}else{
		g_osd_acc_sync_print_num = 0;
	}

	if (dma_intst_reg.osd2_acc_sync) {
		osd1_acc_under = 1;
		g_osd_acc_sync_num[GDMA_PLANE_OSD2]++;
		rtd_pr_gdma_debug(KERN_EMERG"GDMA: OSD2 accident sync! num = %d\n", g_osd_acc_sync_num[GDMA_PLANE_OSD2]);
	}
	if (dma_intst_reg.osd3_acc_sync) {
		osd1_acc_under = 1;
		g_osd_acc_sync_num[GDMA_PLANE_OSD3]++;
		rtd_pr_gdma_debug(KERN_EMERG"GDMA: OSD3 accident sync! num = %d\n", g_osd_acc_sync_num[GDMA_PLANE_OSD3]);
	}
	if (dma_intst_reg.osd1_udfl) {
		osd1_acc_under = 1;
		g_osd_udfl_num[GDMA_PLANE_OSD1]++;
		if (g_osd_acc_sync_print_num < PRINT_LIMIT_ACC){
			g_osd_acc_sync_print_num++;
			//if(Get_DISPLAY_PANEL_OLED_TYPE() == FALSE)
			rtd_pr_gdma_err(KERN_ERR"GDMA: OSD1 under flow! num = %d\n", g_osd_udfl_num[GDMA_PLANE_OSD1]);
		}
		rtd_pr_gdma_debug(KERN_EMERG"GDMA: OSD1 under flow! num = %d\n", g_osd_udfl_num[GDMA_PLANE_OSD1]);
	}
	if (dma_intst_reg.osd2_udfl) {
		osd1_acc_under = 1;

		g_osd_udfl_num[GDMA_PLANE_OSD2]++;
		rtd_pr_gdma_debug(KERN_EMERG"GDMA: OSD2 under flow! num = %d\n", g_osd_udfl_num[GDMA_PLANE_OSD2]);
	}
	if (dma_intst_reg.osd3_udfl) {
		osd1_acc_under = 1;
		g_osd_udfl_num[GDMA_PLANE_OSD3]++;
		rtd_pr_gdma_debug(KERN_EMERG"GDMA: OSD3 under flow! num = %d\n", g_osd_udfl_num[GDMA_PLANE_OSD3]);
	}
	
	dma_intst_reg.write_data = 0;
	rtd_outl(GDMA_DMA_INTST_reg, dma_intst_reg.regValue);
	/*  conflict condition check */
	/*if (rtd_inl(GDMA_blend_setting_conflict_reg))*/
	/*	Warning("GDMA: Conflict condition happened, [0x%x] %x", GDMA_blend_setting_conflict_reg, rtd_inl(GDMA_blend_setting_conflict_reg));*/

#if 0   /*  frame-rate calculation by Vsync */
	static unsigned int gdmaINT_cnt;
	static unsigned int spentTime, sta;
	if (gdmaINT_cnt == 0 && sta == 0) {
		sta = rtd_inl(SCPU_CLK90K_LO_reg);
		/* Warning("[GDMA] Starting counting for FPS, sta=%d \n", sta); */
	} else {
		if (rtd_inl(SCPU_CLK90K_LO_reg) > sta) {
			spentTime = (rtd_inl(SCPU_CLK90K_LO_reg) - sta);
			gdmaINT_cnt++;
		} else if (gdmaINT_cnt < 1000) {
			if (gdmaINT_cnt) {
				Warning(KERN_EMERG"[GDMA]FPS=%d, spend time %d, income time %d \n", (gdmaINT_cnt / (spentTime/90000)), spentTime, gdmaINT_cnt);
			}
		}

		if (gdmaINT_cnt >= 1000) {
			Warning(KERN_EMERG"[GDMA]FPS=%d, spend time %d, income time %d \n", (gdmaINT_cnt / (spentTime/90000)), spentTime, gdmaINT_cnt);
			sta = spentTime = gdmaINT_cnt = 0;
		}

		/* Warning("[GDMA] Counting , gdmaINT_cnt=%d \n", gdmaINT_cnt); */
	}
#endif
	if (!(osd1_vend || osd1_vend_3D || osd1_finish || osd1_sync || osd1_acc_under)) {
		return IRQ_NONE;
	}

	return IRQ_HANDLED;
}


#endif//RTK_GDMA_TEST_OSD1



void GDMA4_dump_intr_status_err( void )
{

	static unsigned int ccc4 = 0;

	if(++ccc4 == g_IntrPrintCount ) {
#if IS_ENABLED(CONFIG_RTK_KDRIVER_SUPPORT)
		rtd_pr_gdma_emerg(KERN_EMERG"GDMA: osd4_sr underflow, source GDMA_osd1, busH=%d Mhz, dclk=%d Mhz, num=%d\n",
			pll_info_bush(), pll_info_disp(), g_sr_udfl_count[GDMA_PLANE_OSD4]);
#else
		rtd_pr_gdma_emerg(KERN_EMERG"GDMA: osd4_sr underflow, source GDMA_osd1, busH=%d Mhz, dclk=%d Mhz, num=%d\n",
			GDMA_BusH_DClk_info(0), GDMA_BusH_DClk_info(1), g_sr_udfl_count[GDMA_PLANE_OSD4]);
#endif

		ccc4 = 0;
	}
}

irqreturn_t GDMA_OSD4_irq_handler(int irq, void *dev_id)
{
	volatile gdma4_dma_intst_RBUS dma_intst_reg;
	volatile gdma4_osd_intst_RBUS osd_intst_reg;
	volatile gdma4_osd_inten_RBUS osd_inten_reg;
	gdma_dev *gdma = dev_id;
	static unsigned int ccc= 0;

	volatile int osd_vend = 0;/* , i; */
	volatile int osd_finish = 0;
	volatile int osd_sync = 0;
	volatile int dma_ints_err = 0;


	/*int lineCnt = (*(volatile u32 *)GET_MAPPED_RBUS_ADDR(0xB8028248)) & 0xfff; */
	osd_intst_reg.regValue = rtd_inl(GDMA4_OSD_INTST_reg);
	osd_inten_reg.regValue = rtd_inl(GDMA4_OSD_INTEN_reg);

#if defined(_TEST_CODE_GDMA_) || defined(_TEST_CODE_OSDCOMP_)
#if !defined(CONFIG_ARM64)
	flush_cache_all();
#endif

#endif
	if ( osd_inten_reg.osd4_fin )
		osd_finish = osd_intst_reg.osd4_fin;
	
	if ( osd_inten_reg.osd4_vsync )
		osd_sync = osd_intst_reg.osd4_vsync;

	if ( osd_inten_reg.osd4_vact_end )
		osd_vend = osd_intst_reg.osd4_vact_end;


	//check all intr status first !

	// SR under
	if ( OSD_SR45_OSD_SR_4_UDFLOW_get_osd_sr_in_udflow( rtd_inl(OSD_SR45_OSD_SR_4_UDFLOW_reg)) ) {
		dma_ints_err = 1;

		g_sr_udfl_count[GDMA_PLANE_OSD4]++;

		rtd_outl( OSD_SR45_OSD_SR_4_UDFLOW_reg,  OSD_SR45_OSD_SR_4_UDFLOW_osd_sr_in_udflow(1)  );
	}


	dma_intst_reg.regValue = rtd_inl(GDMA4_DMA_INTST_reg);

	if ( dma_intst_reg.osd4_acc_sync ) {
		dma_ints_err = 1;

		g_dma_osdAcc_count[GDMA_PLANE_OSD4]++;

		if ( ccc == g_IntrPrintCount ) {
			GDMAEXT_DUMP("OSD4 accident sync! num = %d\n", g_dma_osdAcc_count[GDMA_PLANE_OSD4]);
		}
	} else{
		g_osd_acc_sync_print_num = 0;
	}


	if (dma_intst_reg.osd4_udfl) {
		dma_ints_err = 1;
		g_dma_osdUDF_count[GDMA_PLANE_OSD4]++;  

		if ( ccc == g_IntrPrintCount ) {
			GDMAEXT_DUMP("OSD4 under flow! num = %d\n", g_dma_osdUDF_count[GDMA_PLANE_OSD4]);
		}
	}

	if (dma_intst_reg.osd4_ovfl) {
		dma_ints_err = 1;
		g_dma_osdOVF_count[GDMA_PLANE_OSD4]++;  

		if ( ccc == g_IntrPrintCount ) {
			GDMAEXT_DUMP("OSD4 overflow! num = %d\n", g_dma_osdOVF_count[GDMA_PLANE_OSD4]);
		}
	}

	if (dma_intst_reg.afbc4_acc_sync) {
		dma_ints_err = 1;
		g_dma_tfbcAcc_count[GDMA_PLANE_OSD4]++;  

		if ( ccc == g_IntrPrintCount ) {
			GDMAEXT_DUMP("OSD4 overflow! num = %d\n", g_dma_tfbcAcc_count[GDMA_PLANE_OSD4]);
		}
	}



	++ccc;
	{
		if( ccc == g_IntrPrintCount) {
			//GDMAEXT_DUMP("OSD4 intr val: 0x%x 0x%x\n", osd_intst_reg.regValue,  osd_inten_reg.regValue);

			ccc = 0;
		}

	}

	// not check dma_ints_err ( check GDMA4_DMA_INTEN_reg first )
	if ( !(osd_vend ||  osd_finish || osd_sync ) )  {
		return IRQ_NONE;
	}


	if (osd_sync) 
	{
		#ifdef GDMA4_ENABLE_VSYNC_NOTIFY
			gGDMA4_VSYNC_USER_TIME_PREV = gGDMA4_VSYNC_USER_TIME;
			gGDMA4_VSYNC_USER_TIME = ktime_to_ns (ktime_get());

			gGDMA4_VsyncDiff_NS = gGDMA4_VSYNC_USER_TIME - gGDMA4_VSYNC_USER_TIME_PREV;

		#endif//GDMA4_ENABLE_VSYNC_NOTIFY

		#if 1
		if (gSemaphore[GDMA_PLANE_OSD4] > 0 && gdma->pendingRPC[GDMA_PLANE_OSD4] && (GDMA4_CTRL4_get_osd4_prog_done(rtd_inl(GDMA4_CTRL4_reg)) == 0)) {
			gdma->pendingRPC[GDMA_PLANE_OSD4] = 0;
			up(&gdma->updateSem[GDMA_PLANE_OSD4]);
		}
		#endif//0


		#ifdef GDMA4_ENABLE_EXP_TIME
		{
			unsigned long exp_lock_flags;
			spin_lock_irqsave(&expTime4_completion_lock, exp_lock_flags);
			
			complete( &gdma4_expTime_completion );
			
			spin_unlock_irqrestore(&expTime4_completion_lock, exp_lock_flags);

		}
		#endif//GDMA_ENABLE_EXP_TIME
		
		#ifdef GDMA4_ENABLE_VSYNC_NOTIFY
		{
			unsigned long vsync_lock_flags;
			spin_lock_irqsave(&gdma4_vsync_completion_lock, vsync_lock_flags);

			complete( &gdma4_vsync_completion );

			spin_unlock_irqrestore(&gdma4_vsync_completion_lock, vsync_lock_flags);
		}
		#endif//GDMA4_ENABLE_VSYNC_NOTIFY


		++gVSYNC_frame_index[GDMA_PLANE_OSD4];
	}

	


	if (osd_vend) {
		#if IS_ENABLED(CONFIG_DRM_REALTEK)
			//if(gdma->GDMA_IrqCallBack)
			//	gdma->GDMA_IrqCallBack(gdma->IrqCallBackData);
		#endif
	}


	



	//clear intr status	

	if (osd_vend) {
		rtd_outl(GDMA4_OSD_INTST_reg, GDMA4_OSD_INTST_write_data(0) | GDMA4_OSD_INTST_osd4_vact_end(1));
	}

	if (osd_sync) {
		rtd_outl(GDMA4_OSD_INTST_reg, GDMA4_OSD_INTST_write_data(0) | GDMA4_OSD_INTST_osd4_vsync(1));
	}
	
	
	dma_intst_reg.write_data = 0;
	rtd_outl(GDMA4_DMA_INTST_reg, dma_intst_reg.regValue);


	if( dma_ints_err) {
		GDMA4_dump_intr_status_err();
		

	}

	/*  conflict condition check */
	/*if (rtd_inl(GDMA_blend_setting_conflict_reg))*/
	/*	Warning("GDMA: Conflict condition happened, [0x%x] %x", GDMA_blend_setting_conflict_reg, rtd_inl(GDMA_blend_setting_conflict_reg));*/

#if 0   /*  frame-rate calculation by Vsync */
	static unsigned int gdmaINT_cnt;
	static unsigned int spentTime, sta;
	if (gdmaINT_cnt == 0 && sta == 0) {
		sta = rtd_inl(SCPU_CLK90K_LO_reg);
		/* Warning("[GDMA] Starting counting for FPS, sta=%d \n", sta); */
	} else {
		if (rtd_inl(SCPU_CLK90K_LO_reg) > sta) {
			spentTime = (rtd_inl(SCPU_CLK90K_LO_reg) - sta);
			gdmaINT_cnt++;
		} else if (gdmaINT_cnt < 1000) {
			if (gdmaINT_cnt) {
				Warning(KERN_EMERG"[GDMA]FPS=%d, spend time %d, income time %d \n", (gdmaINT_cnt / (spentTime/90000)), spentTime, gdmaINT_cnt);
			}
		}

		if (gdmaINT_cnt >= 1000) {
			Warning(KERN_EMERG"[GDMA]FPS=%d, spend time %d, income time %d \n", (gdmaINT_cnt / (spentTime/90000)), spentTime, gdmaINT_cnt);
			sta = spentTime = gdmaINT_cnt = 0;
		}

		/* Warning("[GDMA] Counting , gdmaINT_cnt=%d \n", gdmaINT_cnt); */
	}
#endif


	return IRQ_HANDLED;
}

void GDMA5_dump_intr_status_err( void )
{
	static unsigned int ccc1 = 0;

	if(++ccc1 == g_IntrPrintCount ) {

#if IS_ENABLED(CONFIG_RTK_KDRIVER_SUPPORT)
			rtd_pr_gdma_emerg(KERN_EMERG"osd5_sr underflow, source GDMA_osd1, busH=%d Mhz, dclk=%d Mhz, num=%d\n",
			pll_info_bush(), pll_info_disp(), g_sr_udfl_count[GDMA_PLANE_OSD4]);
#else
		rtd_pr_gdma_emerg(KERN_EMERG"osd5_sr underflow, source GDMA_osd1, busH=%d Mhz, dclk=%d Mhz, num=%d\n",
			GDMA_BusH_DClk_info(0), GDMA_BusH_DClk_info(1), g_sr_udfl_count[GDMA_PLANE_OSD4]);
#endif

	ccc1 = 0;
	}

	
}

irqreturn_t GDMA_OSD5_irq_handler(int irq, void *dev_id)
{

	volatile gdma5_dma_intst_RBUS dma_intst_reg;
	volatile gdma5_osd_intst_RBUS osd_intst_reg;
	volatile gdma5_osd_inten_RBUS osd_inten_reg;
	gdma_dev *gdma = dev_id;

	volatile int osd_vend = 0;/* , i; */
	volatile int osd_finish = 0;
	volatile int osd_sync = 0;
	volatile int dma_ints_err = 0;

	static unsigned int ccc= 1;


	/*int lineCnt = (*(volatile u32 *)GET_MAPPED_RBUS_ADDR(0xB8028248)) & 0xfff; */
	osd_intst_reg.regValue = rtd_inl(GDMA5_OSD_INTST_reg);
	osd_inten_reg.regValue = rtd_inl(GDMA5_OSD_INTEN_reg);

#if defined(_TEST_CODE_GDMA_) || defined(_TEST_CODE_OSDCOMP_)
#if !defined(CONFIG_ARM64)
	flush_cache_all();
#endif

#endif
	if ( osd_inten_reg.osd5_fin )
		osd_finish = osd_intst_reg.osd5_fin;
	
	if ( osd_inten_reg.osd5_vsync )
		osd_sync = osd_intst_reg.osd5_vsync;

	if ( osd_inten_reg.osd5_vact_end )
		osd_vend = osd_intst_reg.osd5_vact_end;


	//check all intr status first !

	// SR under
	if ( OSD_SR45_OSD_SR_5_UDFLOW_get_osd_sr_in_udflow( rtd_inl(OSD_SR45_OSD_SR_5_UDFLOW_reg)) ) {
		dma_ints_err = 1;

		g_sr_udfl_count[GDMA_PLANE_OSD4]++;

		rtd_outl( OSD_SR45_OSD_SR_5_UDFLOW_reg,  OSD_SR45_OSD_SR_5_UDFLOW_osd_sr_in_udflow(1)  );
	}


	dma_intst_reg.regValue = rtd_inl(GDMA5_DMA_INTST_reg);


	if ( dma_intst_reg.osd5_acc_sync ) {
		dma_ints_err = 1;

		g_dma_osdAcc_count[GDMA_PLANE_OSD5]++;

		#if 0
		if (g_osd_acc_sync_print_num < PRINT_LIMIT_ACC){
			g_osd_acc_sync_print_num++;
			//if(Get_DISPLAY_PANEL_OLED_TYPE() == FALSE)
			GDMAEXT_DUMP("[GDMA][DMA.ERR] OSD-%d accident sync [%d]  num = %d!!\n", GDMA_PLANE_OSD1,g_osd_acc_sync_print_num,g_osd_acc_sync_num[GDMA_PLANE_OSD1]);
		}
		#endif//
		if ( ccc == g_IntrPrintCount ) {
		GDMAEXT_DUMP("OSD5 accident sync! num = %d\n", g_dma_osdAcc_count[GDMA_PLANE_OSD5]);
		}
	}else{
		g_osd_acc_sync_print_num = 0;
	}



	if (dma_intst_reg.osd5_udfl) {
		dma_ints_err = 1;
		g_dma_osdUDF_count[GDMA_PLANE_OSD5]++;

		#if 0
		if ( g_osd_acc_sync_print_num < PRINT_LIMIT_ACC ) {
			g_osd_acc_sync_print_num++;
			//if(Get_DISPLAY_PANEL_OLED_TYPE() == FALSE)
			rtd_pr_gdma_err(KERN_ERR"GDMA: OSD1 under flow! num = %d\n", g_dma_osdUDF_count[GDMA_PLANE_OSD1]);
		}
		#endif//
		if( ccc == g_IntrPrintCount ) 
			GDMAEXT_DUMP("OSD5 dma under flow! num = %d\n", g_dma_osdUDF_count[GDMA_PLANE_OSD5]);

	}

	if (dma_intst_reg.osd5_ovfl) {
		dma_ints_err = 1;
		g_dma_osdOVF_count[GDMA_PLANE_OSD5]++;  

		if ( ccc == g_IntrPrintCount ) {
			GDMAEXT_DUMP("OSD5 overflow! num = %d\n", g_dma_osdOVF_count[GDMA_PLANE_OSD5]);
		}
	}

	if (dma_intst_reg.afbc5_acc_sync) {
		dma_ints_err = 1;
		g_dma_tfbcAcc_count[GDMA_PLANE_OSD5]++;  

		if ( ccc == g_IntrPrintCount ) {
			GDMAEXT_DUMP("OSD5 overflow! num = %d\n", g_dma_tfbcAcc_count[GDMA_PLANE_OSD5]);
		}
	}


	++ccc;
	{
		if( ccc == g_IntrPrintCount ) {
			//GDMAEXT_DUMP("OSD5 intr val: 0x%x 0x%x\n", osd_intst_reg.regValue,  osd_inten_reg.regValue);
			ccc = 0;
		}

	}

	// not check dma_ints_err ( check GDMA5_DMA_INTEN_reg first )
	if ( !(osd_vend ||  osd_finish || osd_sync ) )  {
		return IRQ_NONE;
	}


	if (osd_sync) 
	{
		#ifdef GDMA5_ENABLE_VSYNC_NOTIFY
			gGDMA5_VSYNC_USER_TIME_PREV = gGDMA5_VSYNC_USER_TIME;
			gGDMA5_VSYNC_USER_TIME = ktime_to_ns (ktime_get());

			gGDMA5_VsyncDiff_NS = gGDMA5_VSYNC_USER_TIME - gGDMA5_VSYNC_USER_TIME_PREV;

		#endif//GDMA5_ENABLE_VSYNC_NOTIFY

		#if 1
		if (gSemaphore[GDMA_PLANE_OSD5] > 0 && gdma->pendingRPC[GDMA_PLANE_OSD5] && (GDMA5_CTRL5_get_osd5_prog_done(rtd_inl(GDMA5_CTRL5_reg)) == 0)) {
			gdma->pendingRPC[GDMA_PLANE_OSD5] = 0;
			up(&gdma->updateSem[GDMA_PLANE_OSD5]);
		}
		#endif//0


		#ifdef GDMA5_ENABLE_EXP_TIME
		{
			unsigned long exp_lock_flags;
			spin_lock_irqsave(&expTime5_completion_lock, exp_lock_flags);
			
			complete( &gdma5_expTime_completion );
			
			spin_unlock_irqrestore(&expTime5_completion_lock, exp_lock_flags);

		}
		#endif//GDMA_ENABLE_EXP_TIME
		
		#if 0 // ENABLE_VSYNC_NOTIFY
		{
			unsigned long vsync_lock_flags;
			spin_lock_irqsave(&gdma5_vsync_completion_lock, vsync_lock_flags);

			complete( &gdma5_vsync_completion );

			spin_unlock_irqrestore(&gdma5_vsync_completion_lock, vsync_lock_flags);
		}
		#endif//ENABLE_VSYNC_NOTIFY


		++gVSYNC_frame_index[GDMA_PLANE_OSD5];
	}

	
	if (osd_vend) {
		#if IS_ENABLED(CONFIG_DRM_REALTEK)
			//if(gdma->GDMA_IrqCallBack)
			//	gdma->GDMA_IrqCallBack(gdma->IrqCallBackData);
		#endif
	}


	//clear intr status	

	if (osd_vend) {
		rtd_outl(GDMA5_OSD_INTST_reg, GDMA5_OSD_INTST_write_data(0) | GDMA5_OSD_INTST_osd5_vact_end(1));
	}

	if (osd_sync) {
		rtd_outl(GDMA5_OSD_INTST_reg, GDMA5_OSD_INTST_write_data(0) | GDMA5_OSD_INTST_osd5_vsync(1));
	}
	
	dma_intst_reg.write_data = 0;
	rtd_outl(GDMA5_DMA_INTST_reg, dma_intst_reg.regValue);

	if( dma_ints_err) {
		GDMA5_dump_intr_status_err();
		

	}



	return IRQ_HANDLED;
}