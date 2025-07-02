//Copyright (C) 2007-2013 Realtek Semiconductor Corporation.
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "rtk_ddomain_isr.h"
#include <rbus/ppoverlay_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/vgip_reg.h>
#include <rbus/ldspi_reg.h>
#include "vgip_isr/scalerAI.h"
#include "vgip_isr/scalerVIP.h"
#include <util/rtk_util_isr_wait.h>
#include <tvscalercontrol/vip/scalerColor.h>
#include <tvscalercontrol/vip/scalerColor_tv043.h>
#include <tvscalercontrol/scaler/scalercolorlib.h>
#include <rtk_ai.h>
#include <tvscalercontrol/vip/ai_pq.h>
// for register dump
#include <tvscalercontrol/io/ioregdrv.h>
#include <base_types.h>
#include "rbus/blu_reg.h"
#include "rbus/ldspi_reg.h"
#include "rbus/timer_reg.h"
#include <mach/rtk_platform.h>

#undef rtd_outl
#define rtd_outl(x, y)     								IoReg_Write32(x,y)
#undef rtd_inl
#define rtd_inl(x)     									IoReg_Read32(x)
#undef rtd_maskl
#define rtd_maskl(x, y, z)     							IoReg_Mask32(x,y,z)
#undef rtd_setbits
#define rtd_setbits(offset, Mask) rtd_outl(offset, (rtd_inl(offset) | Mask))
#undef rtd_clearbits
#define rtd_clearbits(offset, Mask) rtd_outl(offset, ((rtd_inl(offset) & ~(Mask))))

/* Function Prototype */
extern unsigned char power_on_enable_dtg_ie2_interrupt(void);
extern void vbe_disp_debug_framerate_change_msg(void);
static unsigned int mv_den_end_count;
static unsigned int mv_den_sta_count;
static unsigned int memcdtg_den_sta_count;
static unsigned int memcdtg_den_end_count;
static unsigned int ld_hw_blight_end_count;
static unsigned int ld_hist_done_count;
static unsigned int ldspi_tx_done_count;
static unsigned int ldspi_tx_done_1_count;


static int ddomain_isr_suspend(struct platform_device *dev, pm_message_t state);
static int ddomain_isr_resume(struct platform_device *dev);

static struct platform_device *ddomain_isr_platform_devs = NULL;
static struct platform_driver ddomain_isr_platform_driver = {
#ifdef CONFIG_PM
	.suspend		= ddomain_isr_suspend,
	.resume			= ddomain_isr_resume,
#endif
	.driver = {
		.name		= "ddomain_isr",
		.bus		= &platform_bus_type,
	},
};


#ifdef CONFIG_PM
static int ddomain_isr_suspend(struct platform_device *dev, pm_message_t state)
{
	ppoverlay_dtg_ie_2_RBUS ppoverlay_dtg_ie_2_reg;

	rtd_pr_ddomain_isr_notice("[DDOMAIN_ISR]%s %d\n",__func__,__LINE__);

	//enable dtg_ie2 interrupt
	ppoverlay_dtg_ie_2_reg.regValue = rtd_inl(PPOVERLAY_DTG_ie_2_reg);
	ppoverlay_dtg_ie_2_reg.mv_den_sta_event_ie_2 = 0;
	ppoverlay_dtg_ie_2_reg.mv_den_end_event_ie_2 = 0;
    ppoverlay_dtg_ie_2_reg.memcdtgv_den_sta_event_ie_2 = 0;
    //ppoverlay_dtg_ie_2_reg.memcdtgv_den_end_event_ie_2 = 0;
	rtd_outl(PPOVERLAY_DTG_ie_2_reg, ppoverlay_dtg_ie_2_reg.regValue);

	rtd_pr_ddomain_isr_notice("[DDOMAIN_ISR] suspend done\n");

	return 0;
}

static int ddomain_isr_resume(struct platform_device *dev)
{
	sys_reg_int_ctrl_scpu_RBUS sys_reg_int_ctrl_scpu_reg;

	rtd_pr_ddomain_isr_notice("[DDOMAIN_ISR]%s %d\n",__func__,__LINE__);

	// enable route to SCPU,  Dctl_int_2_scpu_routing_en
	sys_reg_int_ctrl_scpu_reg.regValue = 0;
	sys_reg_int_ctrl_scpu_reg.dctl_int_2_scpu_routing_en = 1;
	sys_reg_int_ctrl_scpu_reg.write_data = 1;
	rtd_outl(SYS_REG_INT_CTRL_SCPU_reg, sys_reg_int_ctrl_scpu_reg.regValue);

	//enable dtg_ie2 interrupt
	power_on_enable_dtg_ie2_interrupt();

	rtd_pr_ddomain_isr_notice("[DDOMAIN_ISR] resume done\n");

	return 0;
}

#endif

unsigned int ddomain_isr_get_mv_den_end_count(void)
{
	return mv_den_end_count;
}

unsigned int ddomain_isr_get_mv_den_sta_count(void)
{
    return mv_den_sta_count;
}

unsigned int ddomain_isr_get_memcdtg_den_sta_count(void)
{
    return memcdtg_den_sta_count;
}

unsigned int ddomain_isr_get_memcdtg_den_end_count(void)
{
	return memcdtg_den_end_count;
}

EXPORT_SYMBOL(ddomain_isr_get_mv_den_end_count);
EXPORT_SYMBOL(ddomain_isr_get_mv_den_sta_count);
EXPORT_SYMBOL(ddomain_isr_get_memcdtg_den_sta_count);
EXPORT_SYMBOL(ddomain_isr_get_memcdtg_den_end_count);

#if defined(CONFIG_RTK_AI_DRV)
extern unsigned char bAIInited;
extern UINT8 reset_face_apply;
//extern UINT8 vpq_stereo_face;
extern RTK_AI_PQ_mode aipq_mode;
extern unsigned char PQ_set_done;
extern DRV_AI_Ctrl_table ai_ctrl;
extern int ai_face_rtk_mode;
extern int dcc_user_curve129[129];
extern unsigned char dcc_user_curve_write_flag;
extern void dcc_user_curve_write_table_tv006(UINT8 display, UINT8 Total_Curve_segment, int *Apply_Curve, int *to_SRAM_Curve, UINT8 *write_flag, UINT8 *apply_flag);
#endif
/*LD DEMO*/
extern unsigned char LDDemoMode_init_done;
extern unsigned char vpq_led_LocalDimmingDemoCtrlMode;
extern unsigned char vpq_led_LocalDimmingDemoCtrlSpeed;
extern void fwif_color_set_LD_DemoMode_ISR(void);
extern unsigned char fwif_color_get_LD_SPI_BoostEnable(void);
extern void fwif_color_LD_SW_Boost_Process(void);
extern unsigned char drvif_color_get_Temp(void);
extern void fwif_color_set_LED_LDCtrlDemoMode(BOOLEAN bCtrl);
extern unsigned char gLDSetLUT_flag;
DRV_LD_LDSPI_DATASRAM_TYPE tLD_LDSPI_DataSRAM;

RUTIL_ISR_WAIT_ADV_EXPORT(ISR_WAIT_MV_DEN_STA);
RUTIL_ISR_WAIT_ADV_EXPORT(ISR_WAIT_MV_DEN_END);
RUTIL_ISR_WAIT_ADV_EXPORT(ISR_WAIT_MEMCDTG_DEN_STA);
extern unsigned int Get_DISP_DEN_END_VPOS(void);
extern unsigned int Get_DISP_VERTICAL_TOTAL(void);
extern BOOL vbe_getEndTimePrintEn(void);
unsigned int end_time_max=0,end_time_count=0;
#define DDMOAIN_ISR_ERROR_PRINT_MAX_NUM		500
unsigned int ddomain_den_start_count=0,ddomain_den_end_count=0;
unsigned int ddomain_before_den_start_linecount=0;
unsigned int ddomain_after_den_start_linecount=0;
unsigned int ddomain_before_den_end_linecount=0;
unsigned int ddomain_after_den_end_linecount=0;

unsigned int ddomain_before_den_start_error_num=0;
unsigned int ddomain_after_den_start_error_num=0;
unsigned int ddomain_before_den_end_error_num=0;
unsigned int ddomain_after_den_end_error_num=0;
#define DEN_START_MODE      1
#define DEN_END_MODE      0

static void den_isr_print(char mode,unsigned int enterLineCount,unsigned int OutLineCount)
{
    if(mode == DEN_END_MODE)
    {
        ddomain_den_end_count++;
	    if(enterLineCount < Get_DISP_DEN_END_VPOS())
        {
            ddomain_after_den_end_error_num++;
            ddomain_before_den_end_error_num++;
            ddomain_before_den_end_linecount = enterLineCount;
        }
        else if(OutLineCount < Get_DISP_DEN_END_VPOS())
        {
            ddomain_after_den_end_error_num++;
        }

        if(OutLineCount >= enterLineCount)
            OutLineCount = OutLineCount - enterLineCount;
        else
            OutLineCount = OutLineCount + Get_DISP_VERTICAL_TOTAL() - enterLineCount;//such as:enter 10, out 1
        if(ddomain_after_den_end_linecount < OutLineCount)
		    ddomain_after_den_end_linecount = OutLineCount;
        if(ddomain_den_end_count >= DDMOAIN_ISR_ERROR_PRINT_MAX_NUM)
		{
			ddomain_den_end_count = 0;
			if(ddomain_after_den_end_error_num || ddomain_before_den_end_error_num)
			{
				rtd_pr_ddomain_isr_notice("!!notice:den end isr-before:(%d)-count=%d,max spend:(%d)-count=%d!!!!!!!!\n",ddomain_before_den_end_linecount,ddomain_before_den_end_error_num,ddomain_after_den_end_linecount,ddomain_after_den_end_error_num);
				ddomain_after_den_end_error_num = 0;
                ddomain_before_den_end_error_num = 0;
			}
            ddomain_after_den_end_linecount = 0;
		}
    }
    else
    {
        ddomain_den_start_count++;
	    if(enterLineCount > Get_DISP_DEN_END_VPOS())
        {
            ddomain_after_den_start_error_num++;
            ddomain_before_den_start_error_num++;
            ddomain_before_den_start_linecount = enterLineCount;
        }
        else if(OutLineCount > Get_DISP_DEN_END_VPOS())
        {
            ddomain_after_den_start_error_num++;
        }

        if(OutLineCount >= enterLineCount)
            OutLineCount = OutLineCount - enterLineCount;
        else
            OutLineCount = OutLineCount + Get_DISP_VERTICAL_TOTAL() - enterLineCount;//such as:enter 10, out 1
        if(ddomain_after_den_start_linecount < OutLineCount)
		    ddomain_after_den_start_linecount = OutLineCount;
        if(ddomain_den_start_count >= DDMOAIN_ISR_ERROR_PRINT_MAX_NUM)
		{
			ddomain_den_start_count = 0;
			if(ddomain_after_den_start_error_num || ddomain_before_den_start_error_num)
			{
				rtd_pr_ddomain_isr_notice("!!notice:den start isr-before:(%d)-count=%d,max spend:(%d)-count=%d!!!!!!!!\n",ddomain_before_den_start_linecount,ddomain_before_den_start_error_num,ddomain_after_den_start_linecount,ddomain_after_den_start_error_num);
				ddomain_after_den_start_error_num = 0;
                ddomain_before_den_start_error_num = 0;
			}
            ddomain_after_den_start_linecount = 0;
		}
    }
}

extern unsigned char set_LocalContrast_table_ISR;
extern unsigned char write_invgamma_table;
irqreturn_t ddomain_isr(int irq, void *dev_id)
{
	ppoverlay_dtg_ie_2_RBUS dtg_ie_2_reg;
	ppoverlay_dtg_pending_status_2_RBUS dtg_pending_status_2_reg;
	blu_ld_int_RBUS blu_ld_int_reg;
	blu_ld_pending_status_RBUS blu_ld_pending_status_reg;
	ldspi_wclr_ctrl_RBUS ldspi_wclr_ctrl_reg;
	ldspi_rx_data_ctrl_RBUS ldspi_rx_data_ctrl_reg;
	UINT8 handle_mv_den_sta_flag = 0;
	UINT8 handle_mv_den_end_flag = 0;
    UINT8 handle_memc_den_sta_flag = 0;
    //UINT8 handle_memc_den_end_flag = 0;
	UINT8 handle_disp_frc2fsync_flag = 0;
	UINT8 handle_hw_blight_end_flag = 0;
	UINT8 handle_hist_done_flag = 0;
	unsigned int ret=IRQ_NONE;
	extern void drvif_scaler_enable_frc2fsync_interrupt(UINT8 bEnable);
	extern void drvif_scaler_enable_frc2fsync_HW_speedup_vtotal(UINT8 bEnable);
    extern unsigned char write_inv_OG;
    extern unsigned char write_Color_Mapping;
    extern unsigned char check_avi_info;
    extern unsigned char update_color_map;
	static int LD_tx_done_cnt=0;
	extern unsigned final_OutGAMMA_R[Bin_Num_Gamma / 2], final_OutGAMMA_G[Bin_Num_Gamma / 2], final_OutGAMMA_B[Bin_Num_Gamma / 2];	
	unsigned int t0, t1, uzuInfo, lineCNT0, lineCNT1;
#if defined(CONFIG_RTK_AI_DRV)
	//UINT8 AI_PQ_set_flag = ((vpq_stereo_face!=AI_PQ_AP_OFF) || reset_face_apply);
	UINT8 AI_PQ_set_flag = ((aipq_mode.ap_mode!=AI_MODE_OFF) || reset_face_apply);
#ifdef For_BringUp_Disable	
	int dcc_user_curve_sram[TableSeg_num_Max] = {0};
	unsigned char dcc_user_curve_apply_flag = 0;
#endif
#endif
	enum MACH_TYPE type = get_mach_type();

	dtg_ie_2_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_ie_2_reg);
	dtg_pending_status_2_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_pending_status_2_reg);
	if(type == MACH_ARCH_RTK2851F) {
		blu_ld_int_reg.regValue = IoReg_Read32(BLU_LD_int_reg);
		blu_ld_pending_status_reg.regValue = IoReg_Read32(BLU_LD_pending_status_reg);
		ldspi_wclr_ctrl_reg.regValue = IoReg_Read32(LDSPI_WCLR_CTRL_reg);
		ldspi_rx_data_ctrl_reg.regValue = IoReg_Read32(LDSPI_RX_data_CTRL_reg);
	}
    handle_mv_den_sta_flag   = dtg_pending_status_2_reg.mv_den_sta_event_2;
    handle_mv_den_end_flag   = dtg_pending_status_2_reg.mv_den_end_event_2;
    handle_memc_den_sta_flag = dtg_pending_status_2_reg.memcdtgv_den_sta_event_2;
    //handle_memc_den_end_flag = dtg_pending_status_2_reg.memcdtgv_den_end_event_2;
	handle_disp_frc2fsync_flag = dtg_pending_status_2_reg.disp_frc2fsync_event_2;
	if(type == MACH_ARCH_RTK2851F) {
		handle_hw_blight_end_flag = blu_ld_pending_status_reg.hw_blight_end_int2;
		handle_hist_done_flag = blu_ld_pending_status_reg.hist_done_int2;
	}


	if((dtg_ie_2_reg.disp_frc2fsync_event_ie_2==1)&&(handle_disp_frc2fsync_flag == 1)){
			rtd_pr_ddomain_isr_notice("#fsync ok (vl:%d, ul:%d)\n", VGIP_VGIP_CHN1_LC_get_ch1_line_cnt(IoReg_Read32(VGIP_VGIP_CHN1_LC_reg)),
			PPOVERLAY_new_meas2_linecnt_real_get_uzudtg_dly_line_cnt_rt(IoReg_Read32(PPOVERLAY_new_meas2_linecnt_real_reg)));
#if 0	//I2D tracking debug
			rtd_pr_ddomain_isr_notice("i2d_phase = %d, LC = [%d, %d, %d]\n", PPOVERLAY_I2D_MEAS_phase_get_i2d_phase_err(IoReg_Read32(PPOVERLAY_I2D_MEAS_phase_reg)), VODMA_VODMA_LINE_ST_get_line_cnt(IoReg_Read32(VODMA_VODMA_LINE_ST_reg)), SCALEDOWN_ICH1_line_cnt_get_sdnr_ch1_line_cnt(IoReg_Read32(SCALEDOWN_ICH1_line_cnt_reg)), PPOVERLAY_new_meas2_linecnt_real_get_uzudtg_dly_line_cnt_rt(IoReg_Read32(PPOVERLAY_new_meas2_linecnt_real_reg)));
#endif
			drvif_scaler_enable_frc2fsync_interrupt(0);
			drvif_scaler_enable_frc2fsync_HW_speedup_vtotal(0);
            ret = IRQ_HANDLED;
	}

	if (dtg_ie_2_reg.mv_den_sta_event_ie_2 && handle_mv_den_sta_flag) {//data start isr
	    unsigned int enterLineCount,OutLineCount;
		enterLineCount = PPOVERLAY_new_meas2_linecnt_real_get_uzudtg_dly_line_cnt_rt(IoReg_Read32(PPOVERLAY_new_meas2_linecnt_real_reg));

	    if(write_inv_OG)
               fwif_color_write_InvOutputGamma_curve(final_OutGAMMA_R, final_OutGAMMA_G, final_OutGAMMA_B, 1, 1);
        if(write_Color_Mapping)
                fwif_color_write_Color_Mapping();
        
        RUTIL_ISR_WAIT_ADV_ISR(ISR_WAIT_MV_DEN_STA);    // it's fast

		fwif_color_DI_RTNR_Timing_Measure_Check_ISR();

		mv_den_sta_count++;
		dtg_pending_status_2_reg.regValue=0;
		dtg_pending_status_2_reg.mv_den_sta_event_2 = 1;//write clear status
		IoReg_Write32(PPOVERLAY_DTG_pending_status_2_reg, dtg_pending_status_2_reg.regValue);
        BSP_ERROR_CHECK_REPORT(NULL);//check bsp error status
		ret = IRQ_HANDLED;
		OutLineCount = PPOVERLAY_new_meas2_linecnt_real_get_uzudtg_dly_line_cnt_rt(IoReg_Read32(PPOVERLAY_new_meas2_linecnt_real_reg));
		if(check_vsc_connect_source())
		{
		    den_isr_print(DEN_START_MODE,enterLineCount,OutLineCount);
		}
	}

	if (dtg_ie_2_reg.mv_den_end_event_ie_2 && handle_mv_den_end_flag) {
		unsigned int tick1=0, tick2=0,tickTemp=0,enterLineCount,OutLineCount;
		if(vbe_getEndTimePrintEn())
		{
			tick1 = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);
		}
        enterLineCount = PPOVERLAY_new_meas2_linecnt_real_get_uzudtg_dly_line_cnt_rt(IoReg_Read32(PPOVERLAY_new_meas2_linecnt_real_reg));
        //RUTIL_ISR_WAIT_ADV_ISR(ISR_WAIT_MV_DEN_END);    // it's fast and used to enable VO gating only
		if (set_LocalContrast_table_ISR == 1)
		{			
			set_LocalContrast_table_ISR = 2;
			uzuInfo = IoReg_Read32(PPOVERLAY_DV_DEN_Start_End_reg);
			t0 = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg); 
			lineCNT0 = PPOVERLAY_new_meas2_linecnt_real_get_uzudtg_dly_line_cnt_rt(IoReg_Read32(PPOVERLAY_new_meas2_linecnt_real_reg));
			Scaler_fwif_color_set_LocalContrast_table(0, Scaler_GetLocalContrastTable());		
			fwif_color_set_HV_Num_Fac_Size_LC();
			fwif_color_set_Size_LC_Enable_check();
			set_LocalContrast_table_ISR = 0;
			t1 = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg); 
			lineCNT1 = PPOVERLAY_new_meas2_linecnt_real_get_uzudtg_dly_line_cnt_rt(IoReg_Read32(PPOVERLAY_new_meas2_linecnt_real_reg));
			rtd_pr_ddomain_isr_info("LC_ISR, info=%x,t=%d,%d,l=%d,%d,\n", uzuInfo, t0, t1, lineCNT0, lineCNT1);		
		}

		/* inv_gamma L2F*/
		if(write_invgamma_table == 1)
		{		
			write_invgamma_table = 2;
			fwif_color_invGamma_gamma_write_ctrl(SLR_MAIN_DISPLAY, 1);
			write_invgamma_table = 0;
		}

		Scaler_Reset_Sharpness_Table();

#if defined(CONFIG_RTK_AI_DRV)
#if 0 //disable first. please fixed me if AI on
	dcc_user_curve_write_table_tv006(SLR_MAIN_DISPLAY, TableSeg_num_Max, dcc_user_curve129, dcc_user_curve_sram, &dcc_user_curve_write_flag, &dcc_user_curve_apply_flag);
#endif	
	if(bAIInited && AI_PQ_set_flag)
	{
		#if 0  //disable first,please fixed me if AI on
		if(ai_ctrl.ai_global3.ip_isr_ctrl)// avoid cmd from v4l2, hal, osd..
		{
			if(ai_face_rtk_mode==1)
			{
				drvif_color_AI_obj_dcc_init(1);
				drvif_color_AI_obj_icm_init(1);
				drvif_color_AI_obj_srp_init(1);
			}
			else
			{
				drvif_color_AI_obj_dcc_init(0);
				drvif_color_AI_obj_icm_init(0);
				drvif_color_AI_obj_srp_init(0);
			}
		}
		/*if(vpq_stereo_face!=AI_PQ_AP_OFF && ai_ctrl.ai_global3.ip_isr_ctrl)
		{
			drvif_color_AI_obj_srp_init(1);
			drvif_color_AI_obj_dcc_init();
		}*/

		scalerAI_face_PQ_set();
		#endif
		PQ_set_done = 1;
	}
#endif

		fwif_color_icm_set_pillar_by_ISR();
		scalerVIP_RADCR_main();

		mv_den_end_count++;
		dtg_pending_status_2_reg.regValue=0;
		dtg_pending_status_2_reg.mv_den_end_event_2 = 1;//write clear status
		IoReg_Write32(PPOVERLAY_DTG_pending_status_2_reg, dtg_pending_status_2_reg.regValue);
		ret = IRQ_HANDLED;
        OutLineCount = PPOVERLAY_new_meas2_linecnt_real_get_uzudtg_dly_line_cnt_rt(IoReg_Read32(PPOVERLAY_new_meas2_linecnt_real_reg));
		if(check_vsc_connect_source())
		{
		    den_isr_print(DEN_END_MODE,enterLineCount,OutLineCount);
		}

		if(vbe_getEndTimePrintEn())
		{
			tick2 = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);
			if(tick2 >= tick1)
				tickTemp = tick2 - tick1;
			else //loop back
				tickTemp = (0xffffffff - tick1) + tick2;
			if(end_time_max < tickTemp)
				end_time_max = tickTemp;
			if(end_time_count > 60)
			{
				end_time_count = 0;
				rtd_pr_ddomain_isr_notice("!!!!!!!!!max den_end run time=%d!!!!!!!!! \n",end_time_max);
				end_time_max = 0;
			}
			else
				end_time_count++;
		}
	}

    if (dtg_ie_2_reg.memcdtgv_den_sta_event_ie_2 && handle_memc_den_sta_flag) {
        vbe_disp_debug_framerate_change_msg();
        RUTIL_ISR_WAIT_ADV_ISR(ISR_WAIT_MEMCDTG_DEN_STA);

        memcdtg_den_sta_count++;
        dtg_pending_status_2_reg.regValue=0;
        dtg_pending_status_2_reg.memcdtgv_den_sta_event_2 = 1;//write clear status
        IoReg_Write32(PPOVERLAY_DTG_pending_status_2_reg, dtg_pending_status_2_reg.regValue);
        ret = IRQ_HANDLED;
    }

#if 0
    if (dtg_ie_2_reg.memcdtgv_den_end_event_ie_2 && handle_memc_den_end_flag) {

        memcdtg_den_end_count++;
        dtg_pending_status_2_reg.regValue=0;
        dtg_pending_status_2_reg.memcdtgv_den_end_event_2 = 1;//write clear status
        IoReg_Write32(PPOVERLAY_DTG_pending_status_2_reg, dtg_pending_status_2_reg.regValue);
        ret = IRQ_HANDLED;
    }
#endif
	if(type == MACH_ARCH_RTK2851F) {
		if (blu_ld_int_reg.hw_blight_end_ie2 && handle_hw_blight_end_flag && ldspi_rx_data_ctrl_reg.ld_spi_ints_en) {/*ld hw_blight_end interrupt*/

			ld_hw_blight_end_count++;
			/*HW BL end*/
			if (fwif_color_Get_LD_Init_Done() == true && (fwif_color_get_LD_To_LDSPI_Method_tv043()==MOVE_LD_TO_LDSPI_BYSW_120HZ || fwif_color_get_LD_To_LDSPI_Method_tv043()==MOVE_LD_TO_LDSPI_BYSW_60HZ)) {
				fwif_color_LD_Set_LD_To_LDSPI_BySW_Calculate(fwif_color_get_LD_To_LDSPI_Method_tv043());
				#if 0/*debug*/
					if (j2%1200==0)
					rtd_printk(KERN_INFO, TAG_NAME,"[hw_blight_end_ie2] [cnt=%d](90K = %d)\n\r",ld_hw_blight_end_count , rtd_inl(0xB801B6B8)/90);
					j2++;
				#endif
		
			}
					
			/*Write one clear*/
			blu_ld_pending_status_reg.regValue=0;
			blu_ld_pending_status_reg.hw_blight_end_int2 = 1;//write clear status			
			IoReg_Write32(BLU_LD_pending_status_reg, blu_ld_pending_status_reg.regValue);			
			ret = IRQ_HANDLED;
		}

		if (blu_ld_int_reg.hist_done_ie2 && handle_hist_done_flag && ldspi_rx_data_ctrl_reg.ld_spi_ints_en) {/*ld hist_done interrupt*/

			ld_hist_done_count++;
			blu_ld_pending_status_reg.regValue=0;
			blu_ld_pending_status_reg.hist_done_int2 = 1;//write clear status
			IoReg_Write32(BLU_LD_pending_status_reg, blu_ld_pending_status_reg.regValue);
			ret = IRQ_HANDLED;
		}

		if (ldspi_rx_data_ctrl_reg.tx_done_int_en && ldspi_wclr_ctrl_reg.tx_done && ldspi_rx_data_ctrl_reg.ld_spi_ints_en) {/*ldspi tx_done interrupt*/		
			ldspi_tx_done_count++;
			
			if (fwif_color_Get_LD_Init_Done() == true && (fwif_color_get_LD_To_LDSPI_Method_tv043()==MOVE_LD_TO_LDSPI_BYSW_120HZ || fwif_color_get_LD_To_LDSPI_Method_tv043()==MOVE_LD_TO_LDSPI_BYSW_60HZ)) {
				fwif_color_LD_Set_LD_To_LDSPI_BySW_Apply(); /*Apply*/
		
			}
			if (gLDSetLUT_flag==1) {
				drvif_color_set_LDSPI_DataSRAM_Data_Continuous(&tLD_LDSPI_DataSRAM, 0);
				gLDSetLUT_flag = 0;
			}			
			/*Hack Rhal behavior*/
			if (drvif_color_get_Temp() ){
				if (!LDDemoMode_init_done){
					fwif_color_set_LED_LDCtrlDemoMode(1);
					LD_tx_done_cnt = 0;
					rtd_pr_ddomain_isr_err("set LDDemoMode_init_done=%d\n", LDDemoMode_init_done);  
				}
			
			} else if ((!drvif_color_get_Temp()) & (vpq_led_LocalDimmingDemoCtrlMode==0)){
				if (LDDemoMode_init_done){
					fwif_color_set_LED_LDCtrlDemoMode(0);
					rtd_pr_ddomain_isr_err("set LDDemoMode_init_done=%d\n", LDDemoMode_init_done);  
				}
			}
			fwif_color_LD_SW_Boost_Process();
			if (LDDemoMode_init_done){	/* LD Demo mode*/
				if (LD_tx_done_cnt % vpq_led_LocalDimmingDemoCtrlSpeed  == 0){

					fwif_color_set_LD_DemoMode_ISR();
				}
				LD_tx_done_cnt++;
			}

			ldspi_wclr_ctrl_reg.regValue=0;
			ldspi_wclr_ctrl_reg.tx_done = 1;//write clear status
			IoReg_Write32(LDSPI_WCLR_CTRL_reg, ldspi_wclr_ctrl_reg.regValue);
			ret = IRQ_HANDLED;
		}
		if (ldspi_rx_data_ctrl_reg.tx_done_int_en && ldspi_wclr_ctrl_reg.tx_done_1 && ldspi_rx_data_ctrl_reg.ld_spi_ints_en) {/*ldspi tx_done_1 interrupt*/		
			ldspi_tx_done_1_count++;
			ldspi_wclr_ctrl_reg.regValue=0;
			ldspi_wclr_ctrl_reg.tx_done_1 = 1;//write clear status
			IoReg_Write32(LDSPI_WCLR_CTRL_reg, ldspi_wclr_ctrl_reg.regValue);
			ret = IRQ_HANDLED;
		}
	}

	return ret;
}

extern u32 gic_irq_find_mapping(u32 hwirq);//cnange interrupt register way
static int irq_ddomain_isr = -1;
static int __init ddomain_isr_init_irq(struct platform_device *pdev)
{
	sys_reg_int_ctrl_scpu_RBUS sys_reg_int_ctrl_scpu_reg;
	int irq;

	IoReg_Write32(PPOVERLAY_DTG_pending_status_2_reg, IoReg_Read32(PPOVERLAY_DTG_pending_status_2_reg));

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	/* Request IRQ */
	if(request_irq(irq,
                   ddomain_isr,
                   IRQF_SHARED,
                   "DDOMAIN ISR",
                   (void *)pdev))
	{
		rtd_pr_ddomain_isr_err("ddomain_isr: cannot register IRQ %d\n", irq);
		return -ENXIO;
	}

	irq_ddomain_isr = irq;

	// enable route to SCPU,  Dctl_int_2_scpu_routing_en
	sys_reg_int_ctrl_scpu_reg.regValue = 0;
	sys_reg_int_ctrl_scpu_reg.dctl_int_2_scpu_routing_en = 1;
	sys_reg_int_ctrl_scpu_reg.write_data = 1;
	rtd_outl(SYS_REG_INT_CTRL_SCPU_reg, sys_reg_int_ctrl_scpu_reg.regValue);

	//enable dtg_ie2 interrupt
	power_on_enable_dtg_ie2_interrupt();

	// rtd_pr_ddomain_isr_info("ddomain_isr: register IRQ virq:%d hwirq:%lu\n",
		// irq, irqd_to_hwirq(irq_get_irq_data(irq)));

	return 0;
}
#if 0
static char *vgip_isr_devnode(struct device *dev, mode_t *mode)
{
	return NULL;
}
#endif

int __init ddomain_isr_probe(struct platform_device *pdev)
{
	int result;

	result = ddomain_isr_init_irq(pdev);
	if (result < 0) {
		rtd_pr_ddomain_isr_err("ddomain_isr: can not register irq...\n");
		return result;
	}

	ddomain_isr_platform_devs = pdev;

	return 0;
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
__init int ddomain_isr_init_module(void)
#else
static __init int ddomain_isr_init_module(void)
#endif
{

	int result;

	if (platform_driver_probe(&ddomain_isr_platform_driver, ddomain_isr_probe) != 0) {
		rtd_pr_ddomain_isr_info("ddomain_isr: can not register platform driver...\n");
		result = -EINVAL;
		return result;
	}

	return 0;

}

void __exit  ddomain_isr_exit_module(void)
{
	if (irq_ddomain_isr > -1) {
		free_irq(irq_ddomain_isr, ddomain_isr_platform_devs);
		irq_ddomain_isr = -1;
	}
	platform_driver_unregister(&ddomain_isr_platform_driver);
	ddomain_isr_platform_devs = NULL;
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(ddomain_isr_init_module);
module_exit(ddomain_isr_exit_module);
#endif

