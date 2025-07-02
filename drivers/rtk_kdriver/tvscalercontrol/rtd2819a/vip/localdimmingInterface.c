/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2014
  * All rights reserved.
  * ========================================================================*/
  /*================= File Description =======================================*/
  /**
   * @file
   *  This file is for FILM related functions.
   *
   * @author  $eva_sun$
   * @date    $2020.12.17$
   * @version $0.0$
   */

  /**
   * @addtogroup film
   * @{
   */

  /*============================ Module dependency	===========================*/

#include <linux/slab.h>
//#define malloc(size)        vmalloc(size, CTL_KERN)
#define malloc(size)        kmalloc(size, GFP_KERNEL)
#define free(addr)          kfree(addr)
//#define free(addr)          vfree(addr)

#include <tvscalercontrol/vip/localdimmingInterface.h>
#include "rtk_vip_logger.h"
#include <tvscalercontrol/scaler/vipinclude.h> // for typedef that used in VIP_reg_def.h
#include <tvscalercontrol/vip/vip_reg_def.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/vip/scalerColor.h>
#include <rtk_kdriver/io.h>
#include <rtd_log/rtd_module_log.h>

/////*debug*/
//#include "ldspi_reg.h"
//#include "vipCommon.h"
//#include "localdimmingInterface.h"


void drvif_set_LdInterface_Table(DRV_LDINTERFACE_Table *ptr)
{
#if 1
    /*baisc*/
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    ldspi_as_wr_ctrl_RBUS     ldspi_as_wr_ctrl_reg;
    ldspi_out_data_ctrl_RBUS  ldspi_out_data_ctrl_reg;
    ldspi_out_data_form_RBUS  ldspi_out_data_form_reg;
    ldspi_ld_spi_v_total_RBUS ldspi_ld_spi_v_total_reg;
    ldspi_ld_spi_h_total_RBUS ldspi_ld_spi_h_total_reg;
    ldspi_ld_spi_reproduce_mode_RBUS ldspi_ld_spi_reproduce_mode_reg;
    ldspi_data_sram_special_ctrl_0_RBUS  ldspi_data_sram_special_ctrl_0_reg;
    ldspi_rx_data_ctrl_RBUS   ldspi_rx_data_ctrl_reg;
    ldspi_timing_spread_ctrl_RBUS  ldspi_timing_spread_ctrl_reg;
    ldspi_free_run_ctrl_RBUS   ldspi_free_run_ctrl_reg;
    //timing related
    ldspi_send_delay_RBUS  ldspi_send_delay_reg;
    ldspi_data_unit_delay_RBUS ldspi_data_unit_delay_reg;
    ldspi_multi_format_unit_delay_RBUS ldspi_multi_format_unit_delay_reg;
    ldspi_v_sync_duty_RBUS  ldspi_v_sync_duty_reg;
    ldspi_data_h_time_RBUS  ldspi_data_h_time_reg;
    ldspi_cs_hold_time_RBUS  ldspi_cs_hold_time_reg;
    ldspi_sck_hold_time_RBUS ldspi_sck_hold_time_reg;
    ldspi_timing_ctrl_RBUS   ldspi_timing_ctrl_reg;
    ldspi_ld_spi_ld_timing_fune_tune_RBUS ldspi_ld_spi_ld_timing_fune_tune_reg;  
    /*apl bpl :not in table*/
    /*driver related:not in table*/   
    /*rx related:not in table*/    
    //extra: timing delay	    
    
    DRV_LDINTERFACE_TimingReg LDINTERFACE_TimingReg = {0,0,0,0,0,0,0,0,0};
    /*Read Register*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_as_wr_ctrl_reg.regValue = IoReg_Read32(LDSPI_AS_WR_CTRL_reg);
    ldspi_out_data_ctrl_reg.regValue = IoReg_Read32(LDSPI_Out_data_CTRL_reg);
    ldspi_out_data_form_reg.regValue = IoReg_Read32(LDSPI_Out_data_form_reg);
    ldspi_ld_spi_v_total_reg.regValue = IoReg_Read32(LDSPI_ld_spi_v_total_reg);
    ldspi_ld_spi_h_total_reg.regValue = IoReg_Read32(LDSPI_ld_spi_h_total_reg);
    ldspi_ld_spi_reproduce_mode_reg.regValue = IoReg_Read32(LDSPI_ld_spi_reproduce_mode_reg);
    ldspi_data_sram_special_ctrl_0_reg.regValue = IoReg_Read32(LDSPI_Data_SRAM_SPECIAL_CTRL_0_reg);
    ldspi_rx_data_ctrl_reg.regValue =  IoReg_Read32(LDSPI_RX_data_CTRL_reg);
    ldspi_timing_spread_ctrl_reg.regValue = IoReg_Read32(LDSPI_Timing_Spread_CTRL_reg);
    ldspi_free_run_ctrl_reg.regValue = IoReg_Read32(LDSPI_FREE_RUN_CTRL_reg);
    //Timing related
    ldspi_send_delay_reg.regValue = IoReg_Read32(LDSPI_Send_delay_reg);
    ldspi_data_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Data_unit_delay_reg);
    ldspi_multi_format_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Multi_format_unit_delay_reg);
    ldspi_v_sync_duty_reg.regValue = IoReg_Read32(LDSPI_V_sync_duty_reg);
    ldspi_data_h_time_reg.regValue = IoReg_Read32(LDSPI_Data_h_time_reg);
    ldspi_cs_hold_time_reg.regValue = IoReg_Read32(LDSPI_Cs_hold_time_reg);
    ldspi_sck_hold_time_reg.regValue = IoReg_Read32(LDSPI_sck_hold_time_reg);
    ldspi_timing_ctrl_reg.regValue = IoReg_Read32(LDSPI_Timing_CTRL_reg);

    /*basic control*/
    ldspi_ld_ctrl_reg.ld_spi_en = 1;
    ldspi_ld_ctrl_reg.start_enable = 0;//disable start enable
    ldspi_ld_ctrl_reg.ld_mode   = ptr->LDINTERFACE_Basic_Ctrl.ld_mode;
    ldspi_ld_ctrl_reg.as_led_reset   = ptr->LDINTERFACE_Basic_Ctrl.as_led_reset;
    ldspi_ld_ctrl_reg.as_led_with_cs = ptr->LDINTERFACE_Basic_Ctrl.as_led_with_cs;
    ldspi_ld_ctrl_reg.separate_block = ptr->LDINTERFACE_Basic_Ctrl.separate_block;
    ldspi_ld_ctrl_reg.out_data_align = ptr->LDINTERFACE_Basic_Ctrl.out_data_align;
	ldspi_ld_ctrl_reg.in_data_align = ptr->LDINTERFACE_Basic_Ctrl.in_data_align;
    /*basic control:send mode*/
    switch((_SEND_MODE)ptr->LDINTERFACE_Basic_Ctrl.send_mode){
        case VSYNC_MODE:
            ldspi_ld_ctrl_reg.send_follow_vsync = 1;
        break;
        default:
            ldspi_ld_ctrl_reg.send_follow_vsync = 0;
        break;
    }
    ldspi_free_run_ctrl_reg.ldspi_send_trigger_mode = ptr->LDINTERFACE_Basic_Ctrl.send_mode;
    ldspi_free_run_ctrl_reg.free_run_period  = ptr->LDINTERFACE_Basic_Ctrl.free_run_period;
    /*basic control:as mode:w_package_repeater & force tx mode*/
    ldspi_as_wr_ctrl_reg.w_package_repeater = ptr->LDINTERFACE_Basic_Ctrl.w_package_repeater;

    IoReg_Write32(LDSPI_LD_CTRL_reg,ldspi_ld_ctrl_reg.regValue);//disable start_enable
    /*Output*/
    ldspi_out_data_ctrl_reg.data_endian = 0;
    ldspi_out_data_ctrl_reg.header_format = ptr->LDINTERFACE_Output.header_format;
    ldspi_out_data_form_reg.header_units  = ptr->LDINTERFACE_Output.header_units;
    ldspi_out_data_ctrl_reg.output_data_format = ptr->LDINTERFACE_Output.output_data_format;
    ldspi_out_data_ctrl_reg.output_units  = ptr->LDINTERFACE_Output.output_units-1;
    ldspi_out_data_ctrl_reg.tailer_format = ptr->LDINTERFACE_Output.tailer_format;
    ldspi_out_data_form_reg.tailer_units  = ptr->LDINTERFACE_Output.tailer_units;
    ldspi_multi_format_unit_delay_reg.delay_format = ptr->LDINTERFACE_Output.delay_format;
    ldspi_multi_format_unit_delay_reg.multi_add  = ptr->LDINTERFACE_Output.multi_add;
    // ldspi_multi_format_unit_delay2_reg.last_zero_dir = ptr->LDINTERFACE_Output.last_zero_dir; (after merlin9)
    // ldspi_multi_format_unit_delay2_reg.last_zero_dir = ptr->LDINTERFACE_Output.last_zero_padding; (after merlin9)
    /*DRV_LDINTERFACE_INV*/
    ldspi_ld_ctrl_reg.tx_clk_inv = ptr->LDINTERFACE_Inv.tx_clk_inv;
    ldspi_ld_ctrl_reg.tx_cs_inv  = ptr->LDINTERFACE_Inv.tx_cs_inv;
    ldspi_ld_ctrl_reg.tx_sdo_inv = ptr->LDINTERFACE_Inv.tx_sdo_inv;
    ldspi_ld_ctrl_reg.tx_vsync_inv = ptr->LDINTERFACE_Inv.tx_vsync_inv;
    /*LDINTERFACE_Repeat*/
    ldspi_data_sram_special_ctrl_0_reg.hrepeat = ptr->LDINTERFACE_Repeat.hrepeat;
    ldspi_data_sram_special_ctrl_0_reg.vrepeat = ptr->LDINTERFACE_Repeat.vrepeat;
    ldspi_data_sram_special_ctrl_0_reg.vrepeat_width = ptr->LDINTERFACE_Repeat.vrepeat_width;
    /*LDINTERFACE_Reproduce*/
    ldspi_ld_spi_v_total_reg.dv_total_src_sel = ptr->LDINTERFACE_Reproduce.dv_total_src_sel;
    ldspi_ld_spi_v_total_reg.dv_total_sync_dtg= ptr->LDINTERFACE_Reproduce.dv_total_sync_dtg;
    ldspi_ld_spi_v_total_reg.ld_spi_dv_total  = ptr->LDINTERFACE_Reproduce.ld_spi_dv_total;
    ldspi_ld_spi_h_total_reg.ld_spi_dh_total  = ptr->LDINTERFACE_Reproduce.ld_spi_dh_total;
    ldspi_ld_spi_reproduce_mode_reg.dv_reproduce_mode = ptr->LDINTERFACE_Reproduce.dv_reproduce_mode;

    /*DRV_LDINTERFACE_TimingReal:calculate*/
    drvif_cal_LdInterface_TimingReg(&ptr->LDINTERFACE_TimingReal,&LDINTERFACE_TimingReg);
    /*DRV_LDINTERFACE_TimingReal:set register*/
    ldspi_send_delay_reg.data_send_delay = LDINTERFACE_TimingReg.data_send_delay;
    ldspi_data_unit_delay_reg.each_unit_delay = LDINTERFACE_TimingReg.each_unit_delay;
    ldspi_multi_format_unit_delay_reg.multi_unit_delay = LDINTERFACE_TimingReg.multi_unit_delay;
    ldspi_v_sync_duty_reg.vsync_d = LDINTERFACE_TimingReg.vsync_d;
    ldspi_data_h_time_reg.cs_end_delay_time = LDINTERFACE_TimingReg.cs_end_delay_time;
    ldspi_cs_hold_time_reg.cs_hold_time = LDINTERFACE_TimingReg.cs_hold_time;
    ldspi_sck_hold_time_reg.sck_hold_time = LDINTERFACE_TimingReg.sck_hold_time;
    ldspi_timing_ctrl_reg.sck_h = LDINTERFACE_TimingReg.sck_h;
    ldspi_timing_ctrl_reg.sck_l = LDINTERFACE_TimingReg.sck_l;

    //jira: RL6748-1927 ,add ld_backlight_timing_delay 0x7	
    ldspi_ld_spi_ld_timing_fune_tune_reg.ld_backlight_timing_delay = 0x7;	
    /*write register*/
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
    IoReg_Write32(LDSPI_FREE_RUN_CTRL_reg,   ldspi_free_run_ctrl_reg.regValue);
    IoReg_Write32(LDSPI_AS_WR_CTRL_reg,ldspi_as_wr_ctrl_reg.regValue);
    IoReg_Write32(LDSPI_Out_data_CTRL_reg,ldspi_out_data_ctrl_reg.regValue);
    IoReg_Write32(LDSPI_Out_data_form_reg,ldspi_out_data_form_reg.regValue);
    IoReg_Write32(LDSPI_ld_spi_v_total_reg,ldspi_ld_spi_v_total_reg.regValue);
    IoReg_Write32(LDSPI_ld_spi_h_total_reg,ldspi_ld_spi_h_total_reg.regValue);
    IoReg_Write32(LDSPI_ld_spi_reproduce_mode_reg,   ldspi_ld_spi_reproduce_mode_reg.regValue);
    IoReg_Write32(LDSPI_Data_SRAM_SPECIAL_CTRL_0_reg,ldspi_data_sram_special_ctrl_0_reg.regValue);
    IoReg_Write32(LDSPI_RX_data_CTRL_reg,      ldspi_rx_data_ctrl_reg.regValue);
    IoReg_Write32(LDSPI_Timing_Spread_CTRL_reg,ldspi_timing_spread_ctrl_reg.regValue);
    //timing related
    IoReg_Write32(LDSPI_Send_delay_reg,   ldspi_send_delay_reg.regValue);
    IoReg_Write32(LDSPI_Data_unit_delay_reg,ldspi_data_unit_delay_reg.regValue);
    IoReg_Write32(LDSPI_Multi_format_unit_delay_reg,ldspi_multi_format_unit_delay_reg.regValue);
    IoReg_Write32(LDSPI_V_sync_duty_reg,  ldspi_v_sync_duty_reg.regValue);
    IoReg_Write32(LDSPI_Data_h_time_reg,  ldspi_data_h_time_reg.regValue);
    IoReg_Write32(LDSPI_Cs_hold_time_reg, ldspi_cs_hold_time_reg.regValue);
    IoReg_Write32(LDSPI_sck_hold_time_reg,ldspi_sck_hold_time_reg.regValue);
    IoReg_Write32(LDSPI_Timing_CTRL_reg,  ldspi_timing_ctrl_reg.regValue);
    //jira: RL6748-1927 ,add ld_backlight_timing_delay 0x7	
    IoReg_Write32(LDSPI_ld_spi_LD_timing_fune_tune_reg,  ldspi_ld_spi_ld_timing_fune_tune_reg.regValue);

    /*after settings done,enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
#endif
}

void drvif_set_LdInterface_StartEn(UINT8 enable)
{
    ldspi_ld_ctrl_RBUS  ldspi_ld_ctrl_reg;
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = enable;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}

void drvif_set_LdInterface_Enable(UINT8 enable)
{
    ldspi_ld_ctrl_RBUS  ldspi_ld_ctrl_reg;
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.ld_spi_en    = 1;
    ldspi_ld_ctrl_reg.start_enable = enable;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}

void drvif_set_LdInterface_SendTrigger(void)
{
    ldspi_ld_ctrl_RBUS  ldspi_ld_ctrl_reg;
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.send_trigger    = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}

void drvif_cal_LdInterface_TimingReg(DRV_LDINTERFACE_TimingReal *ptrReal, DRV_LDINTERFACE_TimingReg *ptrReg)
{
    /*Timing Real 2 Register*/
    UINT32 tmpVal;
    UINT32 regVal;
    const UINT32 baseUnit = 4;

    /*sck_l*/
    tmpVal = 10*(ptrReal->T3/baseUnit) -10 + 5;
    regVal = (UINT32)tmpVal/10;
    ptrReg->sck_l = regVal;
    /*vsync_d*/
    tmpVal = 10*ptrReal->T5/baseUnit/(128)-10 + 5;
    regVal = (UINT32)tmpVal/10;
    ptrReg->vsync_d = regVal;
    /*data_send_delay*/
    tmpVal = 10*(ptrReal->T6/baseUnit) - 100 + 5;
    regVal = (UINT32)tmpVal/10;
    ptrReg->data_send_delay = regVal;
    /*sck_h*/
    tmpVal = 10*ptrReal->T2/baseUnit - 10*ptrReg->sck_l - 20 + 5 ;
    regVal = (UINT32)tmpVal/10;
    ptrReg->sck_h = regVal;
    /*multi_unit_delay & each_unit_delay*/
    tmpVal = 10*(ptrReal->T4/baseUnit - ptrReg->sck_l - ptrReg->sck_h) + 5;
    regVal = (UINT32)tmpVal/10;
    if(ptrReal->fMultiUnit){
        ptrReg->multi_unit_delay = regVal;
        ptrReg->each_unit_delay = 0;
    }else{
        ptrReg->each_unit_delay = regVal;
        ptrReg->multi_unit_delay = 0;
    }
    /*cs_hold_time*/
    tmpVal = 10*(ptrReal->T1)/baseUnit - 10*ptrReg->data_send_delay - 100 + 5;
    regVal = (UINT32)tmpVal/10;
    ptrReg->cs_hold_time = regVal;
    /*sck_hold_time*/
    tmpVal = 10*(ptrReal->T7)/baseUnit - 10*ptrReg->sck_l + 5;
    regVal = (UINT32)tmpVal/10;
    ptrReg->sck_hold_time = regVal;
    /*cs_end_delay_time*/
    tmpVal = 10*(ptrReal->cs_end)/baseUnit + 5;
    regVal = (UINT32)tmpVal/10;
    ptrReg->cs_end_delay_time = regVal;
}


void drvif_cal_LdInterface_TimingReal(DRV_LDINTERFACE_TimingReal *ptrReal, DRV_LDINTERFACE_TimingReg *ptrReg)
{
    /*register 2 timing real*/
    UINT32 tmpVal;
    const UINT32 baseUnit = 4;
    /*T1*/
    tmpVal = ptrReg->data_send_delay + ptrReg->cs_hold_time + 10;
    ptrReal->T1 = (10*tmpVal *baseUnit + 5)/10;
    /*T1_star*/
    tmpVal = ptrReg->data_send_delay + ptrReg->sck_hold_time +ptrReg->sck_l + 10;
    ptrReal->T1_star = (10*tmpVal *baseUnit + 5)/10;
    /*T2*/
    tmpVal = ptrReg->sck_l + ptrReg->sck_h + 2;
    ptrReal->T2 = (10*tmpVal *baseUnit + 5)/10;
    /*T3*/
    tmpVal = ptrReg->sck_l + 1;
    ptrReal->T3 = (10*tmpVal *baseUnit + 5)/10;
    /*T4*/
    if(ptrReg->each_unit_delay>0){
        tmpVal = ptrReg->each_unit_delay + ptrReg->sck_l + ptrReg->sck_h;
    }else if(ptrReg->multi_unit_delay>0){
        tmpVal = ptrReg->multi_unit_delay + ptrReg->sck_l + ptrReg->sck_h;
    }else{
        tmpVal = ptrReg->sck_h + ptrReg->sck_l;
    }
    ptrReal->T4 = (10*tmpVal *baseUnit + 5)/10;
    /*T5*/
    tmpVal = (ptrReg->vsync_d + 1)*128;
    ptrReal->T5 = (10*tmpVal *baseUnit + 5)/10;
    /*T6*/
    tmpVal = ptrReg->data_send_delay + 10;
    ptrReal->T6 = (10*tmpVal *baseUnit + 5)/10;
    /*T7*/
    tmpVal = ptrReg->sck_hold_time + ptrReg->sck_l;
    ptrReal->T7 = (10*tmpVal *baseUnit + 5)/10;
    /*cs_end_delay*/
    tmpVal = ptrReg->cs_end_delay_time;
    ptrReal->cs_end = (10*tmpVal *baseUnit + 5)/10;

    /*printk(KERN_EMERG "Timing:%d %d %d %d %d %d %d %d %d %d \n",\
           (int)ptrReal->T1,\
           (int)ptrReal->T1_star,\
           (int)ptrReal->T2,\
           (int)ptrReal->T3,\
           (int)ptrReal->T4,\
           (int)ptrReal->T5,\
           (int)ptrReal->T6,\
           (int)ptrReal->T7,\
           (int)ptrReal->cs_end,\
           (int)ptrReal->fMultiUnit);
      */

}

void drvif_set_LdInterface_AplBpl(DRV_LDINTERFACE_APLBPL *ptr)
{
    ldspi_ld_ctrl_RBUS                   ldspi_ld_ctrl_reg;
    ldspi_ld_spi_sram_hw_write_done_RBUS ldspi_ld_spi_sram_hw_write_done_reg;
    ldspi_ld_spi_apl_mode_RBUS           ldspi_ld_spi_apl_mode_reg;
    ldspi_ld_spi_apl_mode_1_RBUS         ldspi_ld_spi_apl_mode_1_reg;
    ldspi_bpl_ctrl_RBUS                  ldspi_bpl_ctrl_reg;
    ldspi_bpl_ctrl_1_RBUS                ldspi_bpl_ctrl_1_reg;    
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*update info&write register*/
    ldspi_ld_spi_sram_hw_write_done_reg.sram_hw_write_num_set = ptr->sram_hw_write_num_set;
    ldspi_ld_spi_apl_mode_reg.apl_mode = ptr->apl_mode;
    ldspi_ld_spi_apl_mode_reg.apl_average = ptr->apl_average;
    ldspi_ld_spi_apl_mode_1_reg.apl_avg_num = ptr->apl_avg_num;
    ldspi_ld_spi_apl_mode_1_reg.apl_spitotal = ptr->apl_spitotal;
    ldspi_bpl_ctrl_reg.bpl_mode = ptr->bpl_mode;
    ldspi_bpl_ctrl_reg.bpl_result = ptr->bpl_average;
    ldspi_bpl_ctrl_reg.bpl_compare = ptr->bpl_compare;
    ldspi_bpl_ctrl_reg.apl_bpl_map_shift = ptr->apl_bpl_map_shift;
    ldspi_bpl_ctrl_1_reg.bpl_sw_r   = ptr->bpl_sw_r;
    ldspi_bpl_ctrl_1_reg.bpl_result_r   = ptr->bpl_average_r;
    IoReg_Write32(LDSPI_ld_spi_sram_hw_write_done_reg,ldspi_ld_spi_sram_hw_write_done_reg.regValue);
    IoReg_Write32(LDSPI_ld_spi_apl_mode_reg,ldspi_ld_spi_apl_mode_reg.regValue);
    IoReg_Write32(LDSPI_ld_spi_apl_mode_1_reg,ldspi_ld_spi_apl_mode_1_reg.regValue);
    IoReg_Write32(LDSPI_BPL_CTRL_reg,ldspi_bpl_ctrl_reg.regValue);
    IoReg_Write32(LDSPI_BPL_CTRL_1_reg,ldspi_bpl_ctrl_1_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}

void drvif_get_LdInterface_AplBplStatus(DRV_LDINTERFACE_APLBPLStatus *ptr)
{
    ldspi_ld_spi_apl_mode_3_RBUS   ldspi_ld_spi_apl_mode_3_reg;
    ldspi_ld_spi_apl_mode_4_RBUS   ldspi_ld_spi_apl_mode_4_reg;
    ldspi_ld_spi_apl_mode_2_RBUS   ldspi_ld_spi_apl_mode_2_reg;
    ldspi_ld_spi_apl_mode_RBUS    ldspi_ld_spi_apl_mode_reg;
    ldspi_bpl_ctrl_RBUS           ldspi_bpl_ctrl_reg;
    ldspi_bpl_ctrl_1_RBUS         ldspi_bpl_ctrl_1_reg;

    ldspi_ld_spi_apl_mode_reg.regValue   = IoReg_Read32(LDSPI_ld_spi_apl_mode_reg);
    ldspi_ld_spi_apl_mode_2_reg.regValue = IoReg_Read32(LDSPI_ld_spi_apl_mode_2_reg);
    ldspi_ld_spi_apl_mode_3_reg.regValue = IoReg_Read32(LDSPI_ld_spi_apl_mode_3_reg);
    ldspi_ld_spi_apl_mode_4_reg.regValue = IoReg_Read32(LDSPI_ld_spi_apl_mode_4_reg);
    ldspi_bpl_ctrl_reg.regValue          = IoReg_Read32(LDSPI_BPL_CTRL_reg);
    ldspi_bpl_ctrl_1_reg.regValue        = IoReg_Read32(LDSPI_BPL_CTRL_1_reg);

    ptr->apl_average      = ldspi_ld_spi_apl_mode_reg.apl_average;
    ptr->apl_max          = ldspi_ld_spi_apl_mode_3_reg.apl_max;
    ptr->apl_min          = ldspi_ld_spi_apl_mode_3_reg.apl_min;
    ptr->apl_average_r = ldspi_ld_spi_apl_mode_2_reg.apl_average_r;//mac9q usage, reserved in merlin9
    ptr->apl_max_r     = ldspi_ld_spi_apl_mode_4_reg.apl_max_r;//mac9q usage, reserved in merlin9
    ptr->apl_min_r     = ldspi_ld_spi_apl_mode_4_reg.apl_min_r;//mac9q usage, reserved in merlin9
    ptr->bpl_average    = ldspi_bpl_ctrl_reg.bpl_result;
    ptr->bpl_average_r  = ldspi_bpl_ctrl_1_reg.bpl_result_r;
}


void drvif_set_LdInterface_Checksum(DRV_LDINTERFACE_Checksum *ptr)
{
    ldspi_ld_spi_check_sum_ctrl_RBUS    ldspi_ld_spi_check_sum_ctrl_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*update info&write register*/
    ldspi_ld_spi_check_sum_ctrl_reg.check_sum_minus = ptr->check_sum_minus;
    ldspi_ld_spi_check_sum_ctrl_reg.check_sum_position = ptr->check_sum_position;
    ldspi_ld_spi_check_sum_ctrl_reg.check_sum_start = ptr->check_sum_start;
    IoReg_Write32(LDSPI_ld_spi_check_sum_ctrl_reg,ldspi_ld_spi_check_sum_ctrl_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}

void drvif_set_LdInterface_SdoAdjust(DRV_LDINTERFACE_SdoAdjust *ptr)
{
    ldspi_driver_ctrl_5_RBUS  ldspi_driver_ctrl_5_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*update info&write register*/
    ldspi_driver_ctrl_5_reg.sdo_phase_adjust0 = ptr->sdo_phase_adjust0;
    ldspi_driver_ctrl_5_reg.sdo_phase_adjust1 = ptr->sdo_phase_adjust1;
    ldspi_driver_ctrl_5_reg.sdo_phase_adjust2 = 0;//res in mac9q
    ldspi_driver_ctrl_5_reg.sdo_phase_adjust3 = 0;//res in mac9q
    IoReg_Write32(LDSPI_DRIVER_CTRL_5_reg,ldspi_driver_ctrl_5_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}



void drvif_set_LdInterface_DoubleSram(DRV_LDINTERFACE_DoubleSram *ptr)
{
#if 0
    ldspi_double_sram_crtl_RBUS    ldspi_double_sram_crtl_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*update info&write register*/
    ldspi_double_sram_crtl_reg.ldspi_buf_sel = ptr->ldspi_buf_sel;
    ldspi_double_sram_crtl_reg.ld_buf_sel = ptr->ld_buf_sel;
    ldspi_double_sram_crtl_reg.datasram_double_en = ptr->datasram_double_en;
    IoReg_Write32(LDSPI_DOUBLE_SRAM_CRTL_reg,ldspi_double_sram_crtl_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
#endif
}

void drvif_set_LdInterface_DoubleVs(DRV_LDINTERFACE_DoubleVs *ptr)
{
    ldspi_db_vsync_RBUS    ldspi_db_vsync_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*update info&write register*/
    //ldspi_db_vsync_reg.memc_output_frame_en = ptr->memc_output_frame_en; (mac9q no support memc)
    ldspi_db_vsync_reg.remove_num_sel = ptr->remove_num_sel;
    ldspi_db_vsync_reg.db_vs_en = ptr->db_vs_en;
    IoReg_Write32(LDSPI_db_vsync_reg,ldspi_db_vsync_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}

void drvif_set_LdInterface_CurrentGain(DRV_LDINTERFACE_CurrentGain *ptr)
{
#if 0
    //for ic after merlin9
    ldspi_bpl_current_mode_ctrl1_RBUS   ldspi_bpl_current_mode_ctrl1_reg;
    ldspi_bpl_current_mode_ctrl2_RBUS   ldspi_bpl_current_mode_ctrl2_reg;
    ldspi_bpl_current_mode_ctrl3_RBUS   ldspi_bpl_current_mode_ctrl3_reg;
    ldspi_bpl_current_mode_ctrl4_RBUS   ldspi_bpl_current_mode_ctrl4_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*update info&write register*/
    ldspi_bpl_current_mode_ctrl1_reg.bpl_current_mode_en = ptr->bpl_current_mode_en;
    ldspi_bpl_current_mode_ctrl1_reg.bpl_current_apl_sw_set = ptr->bpl_Current_APL_SW_set;//set to 0
    ldspi_bpl_current_mode_ctrl1_reg.bpl_current_shift = ptr->bpl_current_shift;
    ldspi_bpl_current_mode_ctrl1_reg.bpl_initial_gain = ptr->bpl_initial_gain;

    ldspi_bpl_current_mode_ctrl4_reg.bpl_bl_map_en = ptr->bpl_map_en;
    ldspi_bpl_current_mode_ctrl4_reg.bpl_bl_map_th = ptr->bpl_map_th;

    ldspi_bpl_current_mode_ctrl2_reg.bpl_current_smooth_en = ptr->smooth_en;
    ldspi_bpl_current_mode_ctrl2_reg.bpl_current_smooth_th1 = ptr->smooth_th1;
    ldspi_bpl_current_mode_ctrl2_reg.bpl_current_smooth_th2 = ptr->smooth_th2;

    ldspi_bpl_current_mode_ctrl3_reg.bpl_current_smooth_step1 = ptr->smooth_step1;
    ldspi_bpl_current_mode_ctrl3_reg.bpl_current_smooth_step2 = ptr->smooth_step2;
    ldspi_bpl_current_mode_ctrl3_reg.bpl_current_smooth_step3 = ptr->smooth_step3;

    IoReg_Write32(LDSPI_BPL_CURRENT_MODE_CTRL1_reg,ldspi_bpl_current_mode_ctrl1_reg.regValue);
    IoReg_Write32(LDSPI_BPL_CURRENT_MODE_CTRL2_reg,ldspi_bpl_current_mode_ctrl2_reg.regValue);
    IoReg_Write32(LDSPI_BPL_CURRENT_MODE_CTRL3_reg,ldspi_bpl_current_mode_ctrl3_reg.regValue);
    IoReg_Write32(LDSPI_BPL_CURRENT_MODE_CTRL4_reg,ldspi_bpl_current_mode_ctrl4_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
#endif
}

void drvif_set_LdInterface_CurrentGainUpdate(void)
{
#if 0
    //for ic after merlin9
    ldspi_bpl_current_mode_ctrl1_RBUS   ldspi_bpl_current_mode_ctrl1_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*update info&write register*/
    ldspi_bpl_current_mode_ctrl1_reg.regValue = IoReg_Read32(LDSPI_BPL_CURRENT_MODE_CTRL1_reg);
    ldspi_bpl_current_mode_ctrl1_reg.bpl_current_apl_sw_set = 1;
    IoReg_Write32(LDSPI_BPL_CURRENT_MODE_CTRL1_reg,ldspi_bpl_current_mode_ctrl1_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
#endif
}

void drvif_get_LdInterface_CurrentGainStatus(DRV_LDINTERFACE_CurrentGainStatus *ptrReg)
{
#if 0
    //for ic after merlin9
    ldspi_bpl_current_mode_ctrl1_RBUS   ldspi_bpl_current_mode_ctrl1_reg;
    ldspi_bpl_current_mode_flag_RBUS    ldspi_bpl_current_mode_flag_reg;

    ldspi_bpl_current_mode_ctrl1_reg.regValue = IoReg_Read32(LDSPI_BPL_CURRENT_MODE_CTRL1_reg);
    ldspi_bpl_current_mode_flag_reg.regValue  = IoReg_Read32(LDSPI_BPL_CURRENT_MODE_FLAG_reg);

    ptrReg->bpl_gain_update           = ldspi_bpl_current_mode_ctrl1_reg.bpl_gain_update;
    ptrReg->bpl_update_error          = ldspi_bpl_current_mode_ctrl1_reg.bpl_update_error;
    ptrReg->apl_avg_update            = ldspi_bpl_current_mode_ctrl1_reg.apl_avg_update;
    ptrReg->bpl_current_gain_diff_dir = ldspi_bpl_current_mode_flag_reg.bpl_current_gain_diff_dir;
    ptrReg->bpl_current_gain          = ldspi_bpl_current_mode_flag_reg.bpl_current_cur_gain;
    ptrReg->bpl_current_gain_diff     = ldspi_bpl_current_mode_flag_reg.bpl_current_gain_diff;
    ptrReg->bpl_current_pre_gain      = ldspi_bpl_current_mode_flag_reg.bpl_current_pre_gain;
#endif
}

void drvif_set_LdInterface_AppendDataCtrl(DRV_LDINTERFACE_AppendDataCtrl *ptr)
{
#if 0
    //for ic after merlin9
    ldspi_append_data_crtl_RBUS    ldspi_append_data_crtl_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*update info&write register*/
    ldspi_append_data_crtl_reg.append_bit15_value = ptr->app_bit15_val;
    ldspi_append_data_crtl_reg.append_bit14_value = ptr->app_bit14_val;
    ldspi_append_data_crtl_reg.append_bit13_value = ptr->app_bit13_val;
    ldspi_append_data_crtl_reg.append_bit12_value = ptr->app_bit12_val;
    ldspi_append_data_crtl_reg.append_bit11_value = ptr->app_bit11_val;
    ldspi_append_data_crtl_reg.append_bit10_value = ptr->app_bit10_val;
    ldspi_append_data_crtl_reg.append_bit9_value = ptr->app_bit09_val;
    ldspi_append_data_crtl_reg.append_bit8_value = ptr->app_bit08_val;
    ldspi_append_data_crtl_reg.append_bit7_value = ptr->app_bit07_val;
    ldspi_append_data_crtl_reg.append_bit6_value = ptr->app_bit06_val;
    ldspi_append_data_crtl_reg.append_bit5_value = ptr->app_bit05_val;
    ldspi_append_data_crtl_reg.append_bit4_value = ptr->app_bit04_val;
    ldspi_append_data_crtl_reg.append_bit3_value = ptr->app_bit03_val;
    ldspi_append_data_crtl_reg.append_bit2_value = ptr->app_bit02_val;
    ldspi_append_data_crtl_reg.append_bit1_value = ptr->app_bit01_val;
    ldspi_append_data_crtl_reg.append_bit0_value = ptr->app_bit00_val;
    ldspi_append_data_crtl_reg.append_bit15_en = ptr->app_bit15_en;
    ldspi_append_data_crtl_reg.append_bit14_en = ptr->app_bit14_en;
    ldspi_append_data_crtl_reg.append_bit13_en = ptr->app_bit13_en;
    ldspi_append_data_crtl_reg.append_bit12_en = ptr->app_bit12_en;
    ldspi_append_data_crtl_reg.append_bit11_en = ptr->app_bit11_en;
    ldspi_append_data_crtl_reg.append_bit10_en = ptr->app_bit10_en;
    ldspi_append_data_crtl_reg.append_bit9_en  = ptr->app_bit09_en;
    ldspi_append_data_crtl_reg.append_bit8_en  = ptr->app_bit08_en;
    ldspi_append_data_crtl_reg.append_bit7_en  = ptr->app_bit07_en;
    ldspi_append_data_crtl_reg.append_bit6_en  = ptr->app_bit06_en;
    ldspi_append_data_crtl_reg.append_bit5_en  = ptr->app_bit05_en;
    ldspi_append_data_crtl_reg.append_bit4_en  = ptr->app_bit04_en;
    ldspi_append_data_crtl_reg.append_bit3_en  = ptr->app_bit03_en;
    ldspi_append_data_crtl_reg.append_bit2_en  = ptr->app_bit02_en;
    ldspi_append_data_crtl_reg.append_bit1_en  = ptr->app_bit01_en;
    ldspi_append_data_crtl_reg.append_bit0_en  = ptr->app_bit00_en;
    IoReg_Write32(LDSPI_APPEND_DATA_CRTL_reg,ldspi_append_data_crtl_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
#endif
}

void drvif_set_LdInterface_ChannelSwap(DRV_LDINTERFACE_ChannelSwap *ptr)
{
    ldspi_ch_swap_sel_RBUS    ldspi_ch_swap_sel_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*update info&write register*/
    ldspi_ch_swap_sel_reg.port0_map = ptr->port0_map;
    ldspi_ch_swap_sel_reg.port1_map = ptr->port1_map;
    ldspi_ch_swap_sel_reg.port2_map = ptr->port2_map;
    ldspi_ch_swap_sel_reg.port3_map = ptr->port3_map;
    IoReg_Write32(LDSPI_CH_SWAP_SEL_reg,ldspi_ch_swap_sel_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}

void drvif_set_LdInterface_RxCtrl(DRV_LDINTERFACE_RXCTRL *ptr)
{
    ldspi_ch_swap_sel_RBUS      ldspi_ch_swap_sel_reg;
    ldspi_ld_spi_read_ctrl_RBUS ldspi_ld_spi_read_ctrl_reg;
    ldspi_rx_data_ctrl_RBUS     ldspi_rx_data_ctrl_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*update info&write register*/
    ldspi_ch_swap_sel_reg.port0_map = ptr->read_port0_swap;
    ldspi_ld_spi_read_ctrl_reg.read_pos = ptr->read_pos;
    ldspi_ld_spi_read_ctrl_reg.read_count = ptr->read_count;
    ldspi_rx_data_ctrl_reg.rx_data_format = ptr->rx_data_format;
    ldspi_rx_data_ctrl_reg.rx_done_int_en = ptr->rx_done_int_en;
    IoReg_Write32(LDSPI_ld_spi_read_ctrl_reg,ldspi_ld_spi_read_ctrl_reg.regValue);
    IoReg_Write32(LDSPI_CH_SWAP_SEL_reg,ldspi_ch_swap_sel_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}


void drvif_get_LdInterface_RxData(UINT16 *rxData)
{
    ldspi_rx_dataw0_RBUS  ldspi_rx_dataw0_reg;
    ldspi_rx_data_ctrl_RBUS     ldspi_rx_data_ctrl_reg;
    unsigned int addr = LDSPI_RX_dataW0_reg;
    int i = 0;

    rxData = (UINT16*)malloc(sizeof(UINT16)*16); // set size when usage and set free after usage
    memset(rxData, 0, 16);
    for(i = 0;i<16;i+=2){
        ldspi_rx_dataw0_reg.regValue = IoReg_Read32(addr);
        rxData[i] = ldspi_rx_dataw0_reg.rx_data00;
        rxData[i+1] = ldspi_rx_dataw0_reg.rx_data01;
        addr +=4;
    }
    /*after read rx data, clear rx data*/
    ldspi_rx_data_ctrl_reg.regValue = IoReg_Read32(LDSPI_RX_data_CTRL_reg);
    ldspi_rx_data_ctrl_reg.read_data_clr = 1;
    IoReg_Write32(LDSPI_RX_data_CTRL_reg,   ldspi_rx_data_ctrl_reg.regValue);
}


/*inBplPtr : 256 uint10 data*/
void drvif_set_LdInterface_aplbplTable(UINT16 *inBplPtr)
{
    ldspi_bpl_addr_ctrl_RBUS    ldspi_bpl_addr_ctrl_reg;
    ldspi_bpl_rwport_RBUS       ldspi_bpl_rwport_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;

    int tableSize = 256;
    int i = 0;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*write into table*/
    //write index for 0x0
    ldspi_bpl_addr_ctrl_reg.regValue = 0;
    ldspi_bpl_rwport_reg.regValue = 0;
    ldspi_bpl_addr_ctrl_reg.bpl_rw_pointer_set = 0;
    IoReg_Write32(LDSPI_BPL_Addr_CTRL_reg,ldspi_bpl_addr_ctrl_reg.regValue);
    //write table
    for(i =0;i<tableSize;i++){
        ldspi_bpl_rwport_reg.regValue = 0;
        ldspi_bpl_rwport_reg.bpl_rw_data_port = inBplPtr[i];
        IoReg_Write32(LDSPI_BPL_Rwport_reg,ldspi_bpl_rwport_reg.regValue);
    }
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
    /*end settings*/

}

/*inBvPtr : 256 uint8 data*/
void drvif_set_LdInterface_bvTable(UINT16 *inBvPtr)
{
#if 0
    //bv not support in mac9q
    ldspi_bv_lut_1_RBUS    ldspi_bv_lut_1_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;

    int tableSize = 256;
    int i =0;
    unsigned int addr = LDSPI_BV_LUT_1_reg;

    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*write bv info*/
    for(i = 0;i<tableSize;i+=4){
        ldspi_bv_lut_1_reg.regValue = 0;
        ldspi_bv_lut_1_reg.bv_coef_00 = inBvPtr[i];
        ldspi_bv_lut_1_reg.bv_coef_01 = inBvPtr[i+1];
        ldspi_bv_lut_1_reg.bv_coef_02 = inBvPtr[i+2];
        ldspi_bv_lut_1_reg.bv_coef_03 = inBvPtr[i+3];      
        IoReg_Write32(addr,ldspi_bv_lut_1_reg.regValue);
        addr +=4;
    }
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
    /*end settings*/
#endif    
}

void drvif_get_LdInterface_Table(DRV_LDINTERFACE_Table *ptr)
{
    /*baisc*/
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    ldspi_as_wr_ctrl_RBUS     ldspi_as_wr_ctrl_reg;
    ldspi_out_data_ctrl_RBUS  ldspi_out_data_ctrl_reg;
    ldspi_out_data_form_RBUS  ldspi_out_data_form_reg;
    ldspi_ld_spi_v_total_RBUS ldspi_ld_spi_v_total_reg;
    ldspi_ld_spi_h_total_RBUS ldspi_ld_spi_h_total_reg;
    ldspi_ld_spi_reproduce_mode_RBUS ldspi_ld_spi_reproduce_mode_reg;
    ldspi_data_sram_special_ctrl_0_RBUS ldspi_data_sram_special_ctrl_0_reg;
    ldspi_rx_data_ctrl_RBUS   ldspi_rx_data_ctrl_reg;
    ldspi_timing_spread_ctrl_RBUS  ldspi_timing_spread_ctrl_reg;
    ldspi_free_run_ctrl_RBUS   ldspi_free_run_ctrl_reg;
    //timing related
    ldspi_send_delay_RBUS  ldspi_send_delay_reg;
    ldspi_data_unit_delay_RBUS ldspi_data_unit_delay_reg;
    ldspi_multi_format_unit_delay_RBUS ldspi_multi_format_unit_delay_reg;
    //ldspi_multi_format_unit_delay2_RBUS ldspi_multi_format_unit_delay2_reg;  （after merlin9 support）
    ldspi_v_sync_duty_RBUS  ldspi_v_sync_duty_reg;
    ldspi_data_h_time_RBUS  ldspi_data_h_time_reg;
    ldspi_cs_hold_time_RBUS  ldspi_cs_hold_time_reg;
    ldspi_sck_hold_time_RBUS ldspi_sck_hold_time_reg;
    ldspi_timing_ctrl_RBUS   ldspi_timing_ctrl_reg;
    //extra: timing delay

    DRV_LDINTERFACE_TimingReg LDINTERFACE_TimingReg = {0,0,0,0,0,0,0,0,0};
    DRV_LDINTERFACE_TimingReg *ptrReg;

    ptrReg = &LDINTERFACE_TimingReg;
    /*Read Register*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_as_wr_ctrl_reg.regValue = IoReg_Read32(LDSPI_AS_WR_CTRL_reg);
    ldspi_out_data_ctrl_reg.regValue = IoReg_Read32(LDSPI_Out_data_CTRL_reg);
    ldspi_out_data_form_reg.regValue = IoReg_Read32(LDSPI_Out_data_form_reg);
    ldspi_ld_spi_v_total_reg.regValue = IoReg_Read32(LDSPI_ld_spi_v_total_reg);
    ldspi_ld_spi_h_total_reg.regValue = IoReg_Read32(LDSPI_ld_spi_h_total_reg);
    ldspi_ld_spi_reproduce_mode_reg.regValue = IoReg_Read32(LDSPI_ld_spi_reproduce_mode_reg);
    ldspi_data_sram_special_ctrl_0_reg.regValue = IoReg_Read32(LDSPI_Data_SRAM_SPECIAL_CTRL_0_reg);
    ldspi_rx_data_ctrl_reg.regValue =  IoReg_Read32(LDSPI_RX_data_CTRL_reg);
    ldspi_timing_spread_ctrl_reg.regValue = IoReg_Read32(LDSPI_Timing_Spread_CTRL_reg);
    ldspi_free_run_ctrl_reg.regValue  = IoReg_Read32(LDSPI_FREE_RUN_CTRL_reg);
    //Timing related
    ldspi_send_delay_reg.regValue = IoReg_Read32(LDSPI_Send_delay_reg);
    ldspi_data_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Data_unit_delay_reg);
    ldspi_multi_format_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Multi_format_unit_delay_reg);
    ldspi_v_sync_duty_reg.regValue =  IoReg_Read32(LDSPI_V_sync_duty_reg);
    ldspi_data_h_time_reg.regValue =  IoReg_Read32(LDSPI_Data_h_time_reg);
    ldspi_cs_hold_time_reg.regValue = IoReg_Read32(LDSPI_Cs_hold_time_reg);
    ldspi_sck_hold_time_reg.regValue = IoReg_Read32(LDSPI_sck_hold_time_reg);
    ldspi_timing_ctrl_reg.regValue = IoReg_Read32(LDSPI_Timing_CTRL_reg);

    ptr->LDINTERFACE_Basic_Ctrl.as_led_reset = ldspi_ld_ctrl_reg.as_led_reset;
    ptr->LDINTERFACE_Basic_Ctrl.as_led_with_cs = ldspi_ld_ctrl_reg.as_led_with_cs;
    ptr->LDINTERFACE_Basic_Ctrl.out_data_align = ldspi_ld_ctrl_reg.out_data_align;
    ptr->LDINTERFACE_Basic_Ctrl.in_data_align = 0;//no need indata align after merlin8
    ptr->LDINTERFACE_Basic_Ctrl.ld_mode    = ldspi_ld_ctrl_reg.ld_mode;
    ptr->LDINTERFACE_Basic_Ctrl.send_mode = ldspi_free_run_ctrl_reg.ldspi_send_trigger_mode;//send mode
    ptr->LDINTERFACE_Basic_Ctrl.separate_block = ldspi_ld_ctrl_reg.separate_block;
    ptr->LDINTERFACE_Basic_Ctrl.w_package_repeater = ldspi_as_wr_ctrl_reg.w_package_repeater;
    ptr->LDINTERFACE_Basic_Ctrl.free_run_period = ldspi_free_run_ctrl_reg.free_run_period;//free run:add in k8

    ptr->LDINTERFACE_Inv.tx_clk_inv = ldspi_ld_ctrl_reg.tx_clk_inv;
    ptr->LDINTERFACE_Inv.tx_cs_inv = ldspi_ld_ctrl_reg.tx_cs_inv;
    ptr->LDINTERFACE_Inv.tx_sdo_inv = ldspi_ld_ctrl_reg.tx_sdo_inv;
    ptr->LDINTERFACE_Inv.tx_vsync_inv = ldspi_ld_ctrl_reg.tx_vsync_inv;

    ptr->LDINTERFACE_Output.data_sram_separate = ldspi_out_data_form_reg.data_sram_separate;
    ptr->LDINTERFACE_Output.header_units = ldspi_out_data_form_reg.header_units;
    ptr->LDINTERFACE_Output.tailer_units = ldspi_out_data_form_reg.tailer_units;
    ptr->LDINTERFACE_Output.output_units = ldspi_out_data_ctrl_reg.output_units;
    ptr->LDINTERFACE_Output.header_format = ldspi_out_data_ctrl_reg.header_format;
    ptr->LDINTERFACE_Output.tailer_format = ldspi_out_data_ctrl_reg.tailer_format;
    ptr->LDINTERFACE_Output.output_data_format = ldspi_out_data_ctrl_reg.output_data_format;
    ptr->LDINTERFACE_Output.delay_format = ldspi_multi_format_unit_delay_reg.delay_format;
    ptr->LDINTERFACE_Output.multi_add = ldspi_multi_format_unit_delay_reg.multi_add;
    ptr->LDINTERFACE_Output.shift_dir = ldspi_data_sram_special_ctrl_0_reg.data_sram_sft_ctrl>>4;
    ptr->LDINTERFACE_Output.shift_num = ldspi_data_sram_special_ctrl_0_reg.data_sram_sft_ctrl&0xf;
    //ptr->LDINTERFACE_Output.last_zero_dir = ldspi_multi_format_unit_delay2_reg.last_zero_dir;       （after merlin9 support）
    //ptr->LDINTERFACE_Output.last_zero_padding = ldspi_multi_format_unit_delay2_reg.last_zero_append;（after merlin9 support）

    ptr->LDINTERFACE_Repeat.hrepeat = ldspi_data_sram_special_ctrl_0_reg.hrepeat;
    ptr->LDINTERFACE_Repeat.vrepeat = ldspi_data_sram_special_ctrl_0_reg.vrepeat;
    ptr->LDINTERFACE_Repeat.vrepeat_width = ldspi_data_sram_special_ctrl_0_reg.vrepeat_width;

    ptr->LDINTERFACE_Reproduce.dv_reproduce_mode = ldspi_ld_spi_reproduce_mode_reg.dv_reproduce_mode;
    ptr->LDINTERFACE_Reproduce.ld_spi_dh_total = ldspi_ld_spi_v_total_reg.ld_spi_dv_total;
    ptr->LDINTERFACE_Reproduce.ld_spi_dh_total = ldspi_ld_spi_h_total_reg.ld_spi_dh_total;
    ptr->LDINTERFACE_Reproduce.dv_total_src_sel = ldspi_ld_spi_v_total_reg.dv_total_src_sel;
    ptr->LDINTERFACE_Reproduce.dv_total_sync_dtg = ldspi_ld_spi_v_total_reg.dv_total_sync_dtg;

    drvif_get_LdInterface_TimingReg(ptrReg);
    drvif_cal_LdInterface_TimingReal(&ptr->LDINTERFACE_TimingReal,ptrReg);
}

UINT8 drvif_get_LdInterface_CheckTriggerClear(void)
{
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    return (UINT8)ldspi_ld_ctrl_reg.send_trigger;
}

void drvif_set_LdInterface_BasicCtrl(DRV_LDINTERFACE_Basic_Ctrl *ptr)
{
    /*baisc*/
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    ldspi_as_wr_ctrl_RBUS     ldspi_as_wr_ctrl_reg;
    ldspi_free_run_ctrl_RBUS   ldspi_free_run_ctrl_reg;
    //extra: timing delay	
    ldspi_ld_spi_ld_timing_fune_tune_RBUS ldspi_ld_spi_ld_timing_fune_tune_reg;  	
    /*Read Register*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_free_run_ctrl_reg.regValue = IoReg_Read32(LDSPI_FREE_RUN_CTRL_reg);
    ldspi_as_wr_ctrl_reg.regValue = IoReg_Read32(LDSPI_AS_WR_CTRL_reg);
    /*basic control*/
    ldspi_ld_ctrl_reg.ld_mode   = ptr->ld_mode;
    ldspi_ld_ctrl_reg.as_led_reset   = ptr->as_led_reset;
    ldspi_ld_ctrl_reg.as_led_with_cs = ptr->as_led_with_cs;
    ldspi_ld_ctrl_reg.separate_block = ptr->separate_block;
    ldspi_ld_ctrl_reg.out_data_align = ptr->out_data_align;
    ldspi_ld_ctrl_reg.in_data_align = ptr->in_data_align;
    /*basic control:send mode*/
    switch((_SEND_MODE)ptr->send_mode){
        case VSYNC_MODE:
            	ldspi_ld_ctrl_reg.send_follow_vsync = 1;
        break;
	 case FREERUN_MODE_WAIT_LD:
        	ldspi_ld_ctrl_reg.send_trigger= 1;
	   	ldspi_ld_ctrl_reg.send_follow_vsync=1 ;
        break;
	 case FREERUN_MODE_AUTO:
        	ldspi_ld_ctrl_reg.send_trigger= 1;
	   	ldspi_ld_ctrl_reg.send_follow_vsync=1 ;
        break;
        case TRIGGER_MODE:
            ldspi_ld_ctrl_reg.send_follow_vsync = 0;
        break;
    }
    ldspi_free_run_ctrl_reg.ldspi_send_trigger_mode = ptr->send_mode;
    ldspi_free_run_ctrl_reg.free_run_period  = ptr->free_run_period;
    /*basic control:as mode:w_package_repeater & force tx mode*/
    ldspi_as_wr_ctrl_reg.w_package_repeater = ptr->w_package_repeater;
    //jira: RL6748-1927 ,add ld_backlight_timing_delay 0x7	
    ldspi_ld_spi_ld_timing_fune_tune_reg.ld_backlight_timing_delay = 0x7;	
	
    /*write register*/
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
    IoReg_Write32(LDSPI_FREE_RUN_CTRL_reg,   ldspi_free_run_ctrl_reg.regValue);
    IoReg_Write32(LDSPI_AS_WR_CTRL_reg,ldspi_as_wr_ctrl_reg.regValue);
    //jira: RL6748-1927 ,add ld_backlight_timing_delay 0x7	
    IoReg_Write32(LDSPI_ld_spi_LD_timing_fune_tune_reg,  ldspi_ld_spi_ld_timing_fune_tune_reg.regValue);
}

void drvif_get_LdInterface_BasicCtrl(DRV_LDINTERFACE_Basic_Ctrl *ptr)
{
    /*baisc*/
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    ldspi_as_wr_ctrl_RBUS     ldspi_as_wr_ctrl_reg;
    ldspi_free_run_ctrl_RBUS   ldspi_free_run_ctrl_reg;

    /*Read Register*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_as_wr_ctrl_reg.regValue = IoReg_Read32(LDSPI_AS_WR_CTRL_reg);
    ldspi_free_run_ctrl_reg.regValue  = IoReg_Read32(LDSPI_FREE_RUN_CTRL_reg);

    ptr->as_led_reset = ldspi_ld_ctrl_reg.as_led_reset;
    ptr->as_led_with_cs = ldspi_ld_ctrl_reg.as_led_with_cs;
    ptr->out_data_align = ldspi_ld_ctrl_reg.out_data_align;
    ptr->ld_mode    = ldspi_ld_ctrl_reg.ld_mode;
    ptr->send_mode = ldspi_free_run_ctrl_reg.ldspi_send_trigger_mode;//send mode
    ptr->separate_block = ldspi_ld_ctrl_reg.separate_block;
    ptr->w_package_repeater = ldspi_as_wr_ctrl_reg.w_package_repeater;
    ptr->free_run_period = ldspi_free_run_ctrl_reg.free_run_period;//free run:add in k



}

void drvif_set_LdInterface_LdMode(UINT8 ld_mode)
{
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    ldspi_ld_ctrl_reg.regValue = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.ld_mode  = ld_mode;
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg,ldspi_ld_ctrl_reg.regValue);//disable start_enable
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,ldspi_ld_ctrl_reg.regValue);//enable start_enable
}


void drvif_set_LdInterface_Output(DRV_LDINTERFACE_Output *ptr)
{
    ldspi_out_data_ctrl_RBUS  ldspi_out_data_ctrl_reg;
    ldspi_out_data_form_RBUS  ldspi_out_data_form_reg;
    ldspi_multi_format_unit_delay_RBUS        ldspi_multi_format_unit_delay_reg;
    //ldspi_multi_format_unit_delay2_RBUS        ldspi_multi_format_unit_delay2_reg;
    ldspi_data_sram_special_ctrl_0_RBUS ldspi_data_sram_special_ctrl_0_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*update output info：Read*/
    ldspi_out_data_ctrl_reg.regValue = IoReg_Read32(LDSPI_Out_data_CTRL_reg);
    ldspi_out_data_form_reg.regValue = IoReg_Read32(LDSPI_Out_data_form_reg);
    ldspi_multi_format_unit_delay_reg.regValue =  IoReg_Read32(LDSPI_Multi_format_unit_delay_reg);
//    ldspi_multi_format_unit_delay2_reg.regValue = IoReg_Read32(LDSPI_Multi_format_unit_delay2_reg);
    ldspi_data_sram_special_ctrl_0_reg.regValue = IoReg_Read32(LDSPI_Data_SRAM_SPECIAL_CTRL_0_reg);
    /*update output info：Set*/
    ldspi_out_data_ctrl_reg.data_endian = 0;
    ldspi_out_data_ctrl_reg.header_format = ptr->header_format;
    ldspi_out_data_form_reg.header_units  = ptr->header_units;
    ldspi_out_data_ctrl_reg.output_data_format = ptr->output_data_format;
    ldspi_out_data_ctrl_reg.output_units  = ptr->output_units-1;
    ldspi_out_data_ctrl_reg.tailer_format = ptr->tailer_format;
    ldspi_out_data_form_reg.tailer_units  = ptr->tailer_units;
    ldspi_multi_format_unit_delay_reg.delay_format = ptr->delay_format;
    ldspi_multi_format_unit_delay_reg.multi_add = ptr->multi_add;
    ldspi_out_data_form_reg.data_sram_separate= ptr->data_sram_separate;
    ldspi_data_sram_special_ctrl_0_reg.data_sram_sft_ctrl = (ptr->shift_dir<<4)|(ptr->shift_num);
    //ldspi_multi_format_unit_delay2_reg.last_zero_dir = ptr->last_zero_dir;
    //ldspi_multi_format_unit_delay2_reg.last_zero_append = ptr->last_zero_padding;
    /*update output info：Write*/
    IoReg_Write32(LDSPI_Out_data_CTRL_reg,ldspi_out_data_ctrl_reg.regValue);
    IoReg_Write32(LDSPI_Out_data_form_reg,ldspi_out_data_form_reg.regValue);
    IoReg_Write32(LDSPI_Multi_format_unit_delay_reg,ldspi_multi_format_unit_delay_reg.regValue);
    //IoReg_Write32(LDSPI_Multi_format_unit_delay2_reg,ldspi_multi_format_unit_delay2_reg.regValue);
    IoReg_Write32(LDSPI_Data_SRAM_SPECIAL_CTRL_0_reg,   ldspi_data_sram_special_ctrl_0_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}

void drvif_set_LdInterface_DataUnits(UINT32 data_unit)
{
    ldspi_out_data_ctrl_RBUS  ldspi_out_data_ctrl_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*update output unit*/
    ldspi_out_data_ctrl_reg.regValue = IoReg_Read32(LDSPI_Out_data_CTRL_reg);
    ldspi_out_data_ctrl_reg.output_units  = data_unit-1;
    IoReg_Write32(LDSPI_Out_data_CTRL_reg,ldspi_out_data_ctrl_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}

void drvif_set_LdInterface_Inv(DRV_LDINTERFACE_INV *ptr)
{
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*Read Register*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    /*DRV_LDINTERFACE_INV*/
    ldspi_ld_ctrl_reg.tx_clk_inv = ptr->tx_clk_inv;
    ldspi_ld_ctrl_reg.tx_cs_inv  = ptr->tx_cs_inv;
    ldspi_ld_ctrl_reg.tx_sdo_inv = ptr->tx_sdo_inv;
    ldspi_ld_ctrl_reg.tx_vsync_inv = ptr->tx_vsync_inv;
    /*write register*/
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}

void drvif_set_LdInterface_Repeat(DRV_LDINTERFACE_Repeat *ptr)
{
    ldspi_data_sram_special_ctrl_0_RBUS ldspi_data_sram_special_ctrl_0_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*Read Register*/
    ldspi_data_sram_special_ctrl_0_reg.regValue = IoReg_Read32(LDSPI_Data_SRAM_SPECIAL_CTRL_0_reg);
    /*LDINTERFACE_Repeat*/
    ldspi_data_sram_special_ctrl_0_reg.hrepeat = ptr->hrepeat;
    ldspi_data_sram_special_ctrl_0_reg.vrepeat = ptr->vrepeat;
    ldspi_data_sram_special_ctrl_0_reg.vrepeat_width = ptr->vrepeat_width;
    /*write register*/
    IoReg_Write32(LDSPI_Data_SRAM_SPECIAL_CTRL_0_reg,   ldspi_data_sram_special_ctrl_0_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}


void drvif_set_LdInterface_Reproduce(DRV_LDINTERFACE_Reproduce *ptr)
{
    ldspi_ld_spi_v_total_RBUS ldspi_ld_spi_v_total_reg;
    ldspi_ld_spi_h_total_RBUS ldspi_ld_spi_h_total_reg;
    ldspi_ld_spi_reproduce_mode_RBUS ldspi_ld_spi_reproduce_mode_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*Read Register*/
    ldspi_ld_spi_v_total_reg.regValue = IoReg_Read32(LDSPI_ld_spi_v_total_reg);
    ldspi_ld_spi_h_total_reg.regValue = IoReg_Read32(LDSPI_ld_spi_h_total_reg);
    ldspi_ld_spi_reproduce_mode_reg.regValue = IoReg_Read32(LDSPI_ld_spi_reproduce_mode_reg);
    /*LDINTERFACE_Reproduce*/
    ldspi_ld_spi_v_total_reg.dv_total_src_sel = ptr->dv_total_src_sel;
    ldspi_ld_spi_v_total_reg.dv_total_sync_dtg= ptr->dv_total_sync_dtg;
    ldspi_ld_spi_v_total_reg.ld_spi_dv_total  = ptr->ld_spi_dv_total;
    ldspi_ld_spi_h_total_reg.ld_spi_dh_total  = ptr->ld_spi_dh_total;
    ldspi_ld_spi_reproduce_mode_reg.dv_reproduce_mode = ptr->dv_reproduce_mode;
    /*write register*/
    IoReg_Write32(LDSPI_ld_spi_v_total_reg,ldspi_ld_spi_v_total_reg.regValue);
    IoReg_Write32(LDSPI_ld_spi_h_total_reg,ldspi_ld_spi_h_total_reg.regValue);
    IoReg_Write32(LDSPI_ld_spi_reproduce_mode_reg,   ldspi_ld_spi_reproduce_mode_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}


void drvif_set_LdInterface_Spread(DRV_LDINTERFACE_Spread *ptr)
{
    ldspi_timing_spread_ctrl_RBUS  ldspi_timing_spread_ctrl_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*Read Register*/
    ldspi_timing_spread_ctrl_reg.regValue = IoReg_Read32(LDSPI_Timing_Spread_CTRL_reg);
    /*LDINTERFACE_Spread*/
    ldspi_timing_spread_ctrl_reg.sck_spread_unit_num = ptr->sck_spread_unit;
    ldspi_timing_spread_ctrl_reg.sck_spread_max = ptr->sck_spread_max;
    ldspi_timing_spread_ctrl_reg.sck_spread_step = ptr->sck_spread_step;
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}


void drvif_set_LdInterface_IntEn(DRV_LDINTERFACE_IntEn *ptr)
{
    ldspi_rx_data_ctrl_RBUS   ldspi_rx_data_ctrl_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*Read Register*/
    ldspi_rx_data_ctrl_reg.regValue =  IoReg_Read32(LDSPI_RX_data_CTRL_reg);
    /*LDINTERFACE_IntEn*/
    ldspi_rx_data_ctrl_reg.ld_spi_int1_en = ptr->ld_spi_int1_en;
    ldspi_rx_data_ctrl_reg.ld_spi_int2_en = ptr->ld_spi_int2_en;
    ldspi_rx_data_ctrl_reg.tx_done_int_en = ptr->tx_done_int_en;
    ldspi_rx_data_ctrl_reg.sram_hw_write_done_int_en = ptr->sram_hw_write_done_int_en;
    /*write register*/
    IoReg_Write32(LDSPI_RX_data_CTRL_reg,      ldspi_rx_data_ctrl_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}


void drvif_set_LdInterface_TimingReal(DRV_LDINTERFACE_TimingReal *ptr)
{
    //timing related
    ldspi_send_delay_RBUS  ldspi_send_delay_reg;
    ldspi_data_unit_delay_RBUS ldspi_data_unit_delay_reg;
    ldspi_multi_format_unit_delay_RBUS ldspi_multi_format_unit_delay_reg;
    ldspi_v_sync_duty_RBUS  ldspi_v_sync_duty_reg;
    ldspi_data_h_time_RBUS  ldspi_data_h_time_reg;
    ldspi_cs_hold_time_RBUS  ldspi_cs_hold_time_reg;
    ldspi_sck_hold_time_RBUS ldspi_sck_hold_time_reg;
    ldspi_timing_ctrl_RBUS   ldspi_timing_ctrl_reg;
    DRV_LDINTERFACE_TimingReg LDINTERFACE_TimingReg = {0,0,0,0,0,0,0,0,0};
    //Timing related
    ldspi_send_delay_reg.regValue = IoReg_Read32(LDSPI_Send_delay_reg);
    ldspi_data_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Data_unit_delay_reg);
    ldspi_multi_format_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Multi_format_unit_delay_reg);
    ldspi_v_sync_duty_reg.regValue = IoReg_Read32(LDSPI_V_sync_duty_reg);
    ldspi_data_h_time_reg.regValue = IoReg_Read32(LDSPI_Data_h_time_reg);
    ldspi_cs_hold_time_reg.regValue = IoReg_Read32(LDSPI_Cs_hold_time_reg);
    ldspi_sck_hold_time_reg.regValue = IoReg_Read32(LDSPI_sck_hold_time_reg);
    ldspi_timing_ctrl_reg.regValue = IoReg_Read32(LDSPI_Timing_CTRL_reg);
    /*DRV_LDINTERFACE_TimingReal:calculate*/
    drvif_cal_LdInterface_TimingReg(ptr,&LDINTERFACE_TimingReg);
    /*DRV_LDINTERFACE_TimingReal:set register*/
    ldspi_send_delay_reg.data_send_delay = LDINTERFACE_TimingReg.data_send_delay;
    ldspi_data_unit_delay_reg.each_unit_delay = LDINTERFACE_TimingReg.each_unit_delay;
    ldspi_multi_format_unit_delay_reg.multi_unit_delay = LDINTERFACE_TimingReg.multi_unit_delay;
    ldspi_v_sync_duty_reg.vsync_d = LDINTERFACE_TimingReg.vsync_d;
    ldspi_data_h_time_reg.cs_end_delay_time = LDINTERFACE_TimingReg.cs_end_delay_time;
    ldspi_cs_hold_time_reg.cs_hold_time = LDINTERFACE_TimingReg.cs_hold_time;
    ldspi_sck_hold_time_reg.sck_hold_time = LDINTERFACE_TimingReg.sck_hold_time;
    ldspi_timing_ctrl_reg.sck_h = LDINTERFACE_TimingReg.sck_h;
    ldspi_timing_ctrl_reg.sck_l = LDINTERFACE_TimingReg.sck_h;
    /*write timing related*/
    IoReg_Write32(LDSPI_Send_delay_reg,   ldspi_send_delay_reg.regValue);
    IoReg_Write32(LDSPI_Data_unit_delay_reg,ldspi_data_unit_delay_reg.regValue);
    IoReg_Write32(LDSPI_Multi_format_unit_delay_reg,ldspi_multi_format_unit_delay_reg.regValue);
    IoReg_Write32(LDSPI_V_sync_duty_reg,  ldspi_v_sync_duty_reg.regValue);
    IoReg_Write32(LDSPI_Data_h_time_reg,  ldspi_data_h_time_reg.regValue);
    IoReg_Write32(LDSPI_Cs_hold_time_reg, ldspi_cs_hold_time_reg.regValue);
    IoReg_Write32(LDSPI_sck_hold_time_reg,ldspi_sck_hold_time_reg.regValue);
    IoReg_Write32(LDSPI_Timing_CTRL_reg,  ldspi_timing_ctrl_reg.regValue);
}


void drvif_set_LdInterface_TimingReg(DRV_LDINTERFACE_TimingReg *ptrReg)
{
    //timing related
    ldspi_send_delay_RBUS  ldspi_send_delay_reg;
    ldspi_data_unit_delay_RBUS ldspi_data_unit_delay_reg;
    ldspi_multi_format_unit_delay_RBUS ldspi_multi_format_unit_delay_reg;
    ldspi_v_sync_duty_RBUS  ldspi_v_sync_duty_reg;
    ldspi_data_h_time_RBUS  ldspi_data_h_time_reg;
    ldspi_cs_hold_time_RBUS  ldspi_cs_hold_time_reg;
    ldspi_sck_hold_time_RBUS ldspi_sck_hold_time_reg;
    ldspi_timing_ctrl_RBUS   ldspi_timing_ctrl_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);

    //Timing related
    ldspi_send_delay_reg.regValue = IoReg_Read32(LDSPI_Send_delay_reg);
    ldspi_data_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Data_unit_delay_reg);
    ldspi_multi_format_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Multi_format_unit_delay_reg);
    ldspi_v_sync_duty_reg.regValue = IoReg_Read32(LDSPI_V_sync_duty_reg);
    ldspi_data_h_time_reg.regValue = IoReg_Read32(LDSPI_Data_h_time_reg);
    ldspi_cs_hold_time_reg.regValue = IoReg_Read32(LDSPI_Cs_hold_time_reg);
    ldspi_sck_hold_time_reg.regValue = IoReg_Read32(LDSPI_sck_hold_time_reg);
    ldspi_timing_ctrl_reg.regValue = IoReg_Read32(LDSPI_Timing_CTRL_reg);
    /*DRV_LDINTERFACE_TimingReal:set register*/
    ldspi_send_delay_reg.data_send_delay = ptrReg->data_send_delay;
    ldspi_data_unit_delay_reg.each_unit_delay = ptrReg->each_unit_delay;
    ldspi_multi_format_unit_delay_reg.multi_unit_delay = ptrReg->multi_unit_delay;
    ldspi_v_sync_duty_reg.vsync_d = ptrReg->vsync_d;
    ldspi_data_h_time_reg.cs_end_delay_time = ptrReg->cs_end_delay_time;
    ldspi_cs_hold_time_reg.cs_hold_time = ptrReg->cs_hold_time;
    ldspi_sck_hold_time_reg.sck_hold_time = ptrReg->sck_hold_time;
    ldspi_timing_ctrl_reg.sck_h = ptrReg->sck_h;
    ldspi_timing_ctrl_reg.sck_l = ptrReg->sck_l;
    /*write timing related*/
    IoReg_Write32(LDSPI_Send_delay_reg,   ldspi_send_delay_reg.regValue);
    IoReg_Write32(LDSPI_Data_unit_delay_reg,ldspi_data_unit_delay_reg.regValue);
    IoReg_Write32(LDSPI_Multi_format_unit_delay_reg,ldspi_multi_format_unit_delay_reg.regValue);
    IoReg_Write32(LDSPI_V_sync_duty_reg,  ldspi_v_sync_duty_reg.regValue);
    IoReg_Write32(LDSPI_Data_h_time_reg,  ldspi_data_h_time_reg.regValue);
    IoReg_Write32(LDSPI_Cs_hold_time_reg, ldspi_cs_hold_time_reg.regValue);
    IoReg_Write32(LDSPI_sck_hold_time_reg,ldspi_sck_hold_time_reg.regValue);
    IoReg_Write32(LDSPI_Timing_CTRL_reg,  ldspi_timing_ctrl_reg.regValue);
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}

void drvif_get_LdInterface_Output(DRV_LDINTERFACE_Output *ptrReg)
{
    ldspi_out_data_ctrl_RBUS  ldspi_out_data_ctrl_reg;
    ldspi_out_data_form_RBUS  ldspi_out_data_form_reg;
    ldspi_multi_format_unit_delay_RBUS ldspi_multi_format_unit_delay_reg;
    ldspi_data_sram_special_ctrl_0_RBUS ldspi_data_sram_special_ctrl_0_reg;
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    /*disable start*/
    ldspi_ld_ctrl_reg.regValue     = IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 0;
    IoReg_Write32(LDSPI_LD_CTRL_reg, ldspi_ld_ctrl_reg.regValue);
    /*grep output info read*/
    ldspi_out_data_ctrl_reg.regValue = IoReg_Read32(LDSPI_Out_data_CTRL_reg);
    ldspi_out_data_form_reg.regValue = IoReg_Read32(LDSPI_Out_data_form_reg);
    ldspi_multi_format_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Multi_format_unit_delay_reg);
    ldspi_data_sram_special_ctrl_0_reg.regValue = IoReg_Read32(LDSPI_Data_SRAM_SPECIAL_CTRL_0_reg);
    /*update output info point*/
    //ldspi_out_data_ctrl_reg.data_endian = 0; 
    ptrReg->header_format = ldspi_out_data_ctrl_reg.header_format;
    ptrReg->header_units = ldspi_out_data_form_reg.header_units;
    ptrReg->output_data_format = ldspi_out_data_ctrl_reg.output_data_format;
    ptrReg->output_units = ldspi_out_data_ctrl_reg.output_units+1;
    ptrReg->tailer_format = ldspi_out_data_ctrl_reg.tailer_format;
    ptrReg->tailer_units = ldspi_out_data_form_reg.tailer_units;
    ptrReg->delay_format= ldspi_multi_format_unit_delay_reg.delay_format;
    ptrReg->multi_add= ldspi_multi_format_unit_delay_reg.multi_add;
    ptrReg->data_sram_separate = ldspi_out_data_form_reg.data_sram_separate;
    ptrReg->shift_dir = ldspi_data_sram_special_ctrl_0_reg.data_sram_sft_ctrl>>4;
    ptrReg->shift_num = ldspi_data_sram_special_ctrl_0_reg.data_sram_sft_ctrl&0x15;
    /*enable start enable*/
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    ldspi_ld_ctrl_reg.start_enable = 1;
    IoReg_Write32(LDSPI_LD_CTRL_reg,   ldspi_ld_ctrl_reg.regValue);
}

UINT8 drvif_get_LdInterface_Enable(void)
{
    ldspi_ld_ctrl_RBUS        ldspi_ld_ctrl_reg;
    ldspi_ld_ctrl_reg.regValue =    IoReg_Read32(LDSPI_LD_CTRL_reg);
    if(ldspi_ld_ctrl_reg.start_enable&&ldspi_ld_ctrl_reg.ld_spi_en)
        return 1;
    else
        return 0;
}

void drvif_get_LdInterface_TimingReg(DRV_LDINTERFACE_TimingReg *ptrReg)
{
    ldspi_send_delay_RBUS  ldspi_send_delay_reg;
    ldspi_data_unit_delay_RBUS ldspi_data_unit_delay_reg;
    ldspi_multi_format_unit_delay_RBUS ldspi_multi_format_unit_delay_reg;
    ldspi_v_sync_duty_RBUS  ldspi_v_sync_duty_reg;
    ldspi_data_h_time_RBUS  ldspi_data_h_time_reg;
    ldspi_cs_hold_time_RBUS  ldspi_cs_hold_time_reg;
    ldspi_sck_hold_time_RBUS ldspi_sck_hold_time_reg;
    ldspi_timing_ctrl_RBUS   ldspi_timing_ctrl_reg;
    ldspi_send_delay_reg.regValue = IoReg_Read32(LDSPI_Send_delay_reg);
    ldspi_data_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Data_unit_delay_reg);
    ldspi_multi_format_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Multi_format_unit_delay_reg);
    ldspi_v_sync_duty_reg.regValue = IoReg_Read32(LDSPI_V_sync_duty_reg);
    ldspi_data_h_time_reg.regValue = IoReg_Read32(LDSPI_Data_h_time_reg);
    ldspi_cs_hold_time_reg.regValue = IoReg_Read32(LDSPI_Cs_hold_time_reg);
    ldspi_sck_hold_time_reg.regValue = IoReg_Read32(LDSPI_sck_hold_time_reg);
    ldspi_timing_ctrl_reg.regValue = IoReg_Read32(LDSPI_Timing_CTRL_reg);
    ptrReg->cs_end_delay_time = ldspi_data_h_time_reg.cs_end_delay_time;
    ptrReg->cs_hold_time      = ldspi_cs_hold_time_reg.cs_hold_time;
    ptrReg->data_send_delay   = ldspi_send_delay_reg.data_send_delay;
    ptrReg->each_unit_delay   = ldspi_data_unit_delay_reg.each_unit_delay;
    ptrReg->multi_unit_delay  = ldspi_multi_format_unit_delay_reg.multi_unit_delay;
    ptrReg->sck_h             = ldspi_timing_ctrl_reg.sck_h;
    ptrReg->sck_l             = ldspi_timing_ctrl_reg.sck_l;
    ptrReg->vsync_d           = ldspi_v_sync_duty_reg.vsync_d;
}

void drvif_get_LdInterface_TimingReal(DRV_LDINTERFACE_TimingReal *ptrReal)
{
    ldspi_send_delay_RBUS  ldspi_send_delay_reg;
    ldspi_data_unit_delay_RBUS ldspi_data_unit_delay_reg;
    ldspi_multi_format_unit_delay_RBUS ldspi_multi_format_unit_delay_reg;
    ldspi_v_sync_duty_RBUS  ldspi_v_sync_duty_reg;
    ldspi_data_h_time_RBUS  ldspi_data_h_time_reg;
    ldspi_cs_hold_time_RBUS  ldspi_cs_hold_time_reg;
    ldspi_sck_hold_time_RBUS ldspi_sck_hold_time_reg;
    ldspi_timing_ctrl_RBUS   ldspi_timing_ctrl_reg;
    DRV_LDINTERFACE_TimingReg LDINTERFACE_TimingReg = {0,0,0,0,0,0,0,0,0};
    ldspi_send_delay_reg.regValue = IoReg_Read32(LDSPI_Send_delay_reg);
    ldspi_data_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Data_unit_delay_reg);
    ldspi_multi_format_unit_delay_reg.regValue = IoReg_Read32(LDSPI_Multi_format_unit_delay_reg);
    ldspi_v_sync_duty_reg.regValue = IoReg_Read32(LDSPI_V_sync_duty_reg);
    ldspi_data_h_time_reg.regValue = IoReg_Read32(LDSPI_Data_h_time_reg);
    ldspi_cs_hold_time_reg.regValue = IoReg_Read32(LDSPI_Cs_hold_time_reg);
    ldspi_sck_hold_time_reg.regValue = IoReg_Read32(LDSPI_sck_hold_time_reg);
    ldspi_timing_ctrl_reg.regValue = IoReg_Read32(LDSPI_Timing_CTRL_reg);
    LDINTERFACE_TimingReg.cs_end_delay_time = ldspi_data_h_time_reg.cs_end_delay_time;
    LDINTERFACE_TimingReg.cs_hold_time      = ldspi_cs_hold_time_reg.cs_hold_time;
    LDINTERFACE_TimingReg.data_send_delay   = ldspi_send_delay_reg.data_send_delay;
    LDINTERFACE_TimingReg.each_unit_delay   = ldspi_data_unit_delay_reg.each_unit_delay;
    LDINTERFACE_TimingReg.multi_unit_delay  = ldspi_multi_format_unit_delay_reg.multi_unit_delay;
    LDINTERFACE_TimingReg.sck_h             = ldspi_timing_ctrl_reg.sck_h;
    LDINTERFACE_TimingReg.sck_l             = ldspi_timing_ctrl_reg.sck_l;
    LDINTERFACE_TimingReg.vsync_d           = ldspi_v_sync_duty_reg.vsync_d;
    drvif_cal_LdInterface_TimingReal(ptrReal,&LDINTERFACE_TimingReg);
}


void drvif_get_LdInterface_Status(DRV_LDINTERFACE_Status *ptr)
{
    ldspi_wclr_ctrl_RBUS ldspi_wclr_ctrl_reg;
    ldspi_ld_spi_ld_spi_timing_error_RBUS  ldspi_ld_spi_ld_spi_timing_error_reg;
    ldspi_free_run_ctrl2_RBUS    ldspi_free_run_ctrl2_reg;
    ldspi_ld_spi_apl_mode_RBUS   ldspi_ld_spi_apl_mode_reg;
    ldspi_bpl_ctrl_RBUS          ldspi_bpl_ctrl_reg;

    ldspi_wclr_ctrl_reg.regValue = IoReg_Read32(LDSPI_WCLR_CTRL_reg);
    ldspi_ld_spi_ld_spi_timing_error_reg.regValue = IoReg_Read32(LDSPI_ld_spi_LD_SPI_Timing_Error_reg);
    ldspi_free_run_ctrl2_reg.regValue = IoReg_Read32(LDSPI_FREE_RUN_CTRL2_reg);
    ldspi_ld_spi_apl_mode_reg.regValue = IoReg_Read32(LDSPI_ld_spi_apl_mode_reg);
    ldspi_bpl_ctrl_reg.regValue       = IoReg_Read32(LDSPI_BPL_CTRL_reg);

    ptr->tx_done = ldspi_wclr_ctrl_reg.tx_done;
    ptr->tx_done1 = ldspi_wclr_ctrl_reg.tx_done_1;
    ptr->error_status = ldspi_wclr_ctrl_reg.error_status;
    ptr->sram_hw_write_done  = ldspi_wclr_ctrl_reg.sram_hw_write_done;
    ptr->ld_timing_error  = ldspi_ld_spi_ld_spi_timing_error_reg.ld_timing_error;
    ptr->ld_spi_timing_Error = ldspi_ld_spi_ld_spi_timing_error_reg.ld_spi_timing_error;
    ptr->ld_spi_freerun_error = ldspi_free_run_ctrl2_reg.ld_spi_freerun_error;
    ptr->apl_average          = ldspi_ld_spi_apl_mode_reg.apl_average;
    ptr->bpl_result           = ldspi_bpl_ctrl_reg.bpl_result;
}


void printTableInfo(DRV_LDINTERFACE_Table *ptr)
{
    rtd_pr_vpq_emerg("LDSPI:PRINT Table Info\n");
    rtd_pr_vpq_emerg("BASIC:%d %d %d %d %d %d %d %d %d\n",\
           (int)ptr->LDINTERFACE_Basic_Ctrl.ld_mode,\
           (int)ptr->LDINTERFACE_Basic_Ctrl.separate_block,\
           (int)ptr->LDINTERFACE_Basic_Ctrl.send_mode,\
           (int)ptr->LDINTERFACE_Basic_Ctrl.out_data_align,\
           (int)ptr->LDINTERFACE_Basic_Ctrl.as_led_with_cs,\
           (int)ptr->LDINTERFACE_Basic_Ctrl.as_led_reset,\
           (int)ptr->LDINTERFACE_Basic_Ctrl.as_force_tx,\
           (int)ptr->LDINTERFACE_Basic_Ctrl.w_package_repeater,\
           (int)ptr->LDINTERFACE_Basic_Ctrl.free_run_period);
    rtd_pr_vpq_emerg("Output:%d %d %d %d %d %d %d %d\n",\
           (int)ptr->LDINTERFACE_Output.output_units,\
           (int)ptr->LDINTERFACE_Output.output_data_format,\
           (int)ptr->LDINTERFACE_Output.tailer_units,\
           (int)ptr->LDINTERFACE_Output.tailer_format,\
           (int)ptr->LDINTERFACE_Output.header_units,\
           (int)ptr->LDINTERFACE_Output.header_format,\
           (int)ptr->LDINTERFACE_Output.delay_format,\
           (int)ptr->LDINTERFACE_Output.data_sram_separate);
    rtd_pr_vpq_emerg("INV:%d %d %d %d\n",\
           (int)ptr->LDINTERFACE_Inv.tx_sdo_inv,\
           (int)ptr->LDINTERFACE_Inv.tx_cs_inv,\
           (int)ptr->LDINTERFACE_Inv.tx_clk_inv,\
           (int)ptr->LDINTERFACE_Inv.tx_vsync_inv);
    rtd_pr_vpq_emerg("Repeat:%d %d %d\n",\
           (int)ptr->LDINTERFACE_Repeat.vrepeat_width,\
           (int)ptr->LDINTERFACE_Repeat.vrepeat,\
           (int)ptr->LDINTERFACE_Repeat.hrepeat);
    rtd_pr_vpq_emerg("Reproduce:%d %d %d %d %d\n",\
           (int)ptr->LDINTERFACE_Reproduce.dv_reproduce_mode,\
           (int)ptr->LDINTERFACE_Reproduce.ld_spi_dh_total,\
           (int)ptr->LDINTERFACE_Reproduce.ld_spi_dv_total,\
           (int)ptr->LDINTERFACE_Reproduce.dv_total_src_sel,\
           (int)ptr->LDINTERFACE_Reproduce.dv_total_sync_dtg);
    rtd_pr_vpq_emerg("Timing:%d %d %d %d %d %d %d %d %d %d \n",\
           (int)ptr->LDINTERFACE_TimingReal.T1,\
           (int)ptr->LDINTERFACE_TimingReal.T1_star,\
           (int)ptr->LDINTERFACE_TimingReal.T2,\
           (int)ptr->LDINTERFACE_TimingReal.T3,\
           (int)ptr->LDINTERFACE_TimingReal.T4,\
           (int)ptr->LDINTERFACE_TimingReal.T5,\
           (int)ptr->LDINTERFACE_TimingReal.T6,\
           (int)ptr->LDINTERFACE_TimingReal.T7,\
           (int)ptr->LDINTERFACE_TimingReal.cs_end,\
           (int)ptr->LDINTERFACE_TimingReal.fMultiUnit);
}

void drvif_LdInterface_Test(void)
{
    DRV_LDINTERFACE_APLBPL aplBpl;
    DRV_LDINTERFACE_Checksum checksum;
    DRV_LDINTERFACE_SdoAdjust sdoAdj;
    DRV_LDINTERFACE_APLBPL* ptrAplBpl = &aplBpl; // test apl bpl info
    DRV_LDINTERFACE_Checksum* ptrChecksum = &checksum;
    DRV_LDINTERFACE_SdoAdjust* ptrSdoAdj = &sdoAdj;
    extern DRV_LDINTERFACE_Table LDINTERFACE_Table[0];

    /*APL_BPL Info Init*/
    ptrAplBpl->apl_mode    = 1;   /*1.apl enable=>hw mode  2:disable=>sw mode*/
    ptrAplBpl->apl_average = 0; /*use when sw mode write*/
    ptrAplBpl->apl_avg_num = 192; /*real calcultae num*/
    ptrAplBpl->apl_spitotal= 192;/*outputunit - header - tailer +1*/
    ptrAplBpl->bpl_mode   = 1;    /*1.bpl enable=>hw mode  2:disable=>sw mode*/
    ptrAplBpl->bpl_average = 1; /*（bpl_result）use when sw mode write*/
    ptrAplBpl->sram_hw_write_num_set = 192;/*same as output number*/
    ptrAplBpl->bpl_compare = 1; /*1.bpl compare mode 0:original bpl mode*/
    ptrAplBpl->apl_bpl_map_shift = 0; /*0:[7:0] 1:[8:1]...8:[15:8]*/
    ptrAplBpl->bpl_sw            = 0x3f;   /*for compare mode info,used when bpl ccompare:1*/
    ptrAplBpl->bpl_sw_r          = 0x4f; /*for dual panel,compare mode info,used when bpl ccompare:1*/
    ptrAplBpl->bpl_average_r      = 1; /*for dual panel,write when not comapre mode and sw mode*/
    drvif_set_LdInterface_AplBpl(ptrAplBpl);
    ptrChecksum->check_sum_minus = 1;
    ptrChecksum->check_sum_start = 2;
    ptrChecksum->check_sum_position = 3;
    drvif_set_LdInterface_Checksum(ptrChecksum);
    ptrSdoAdj->sdo_phase_adjust0 = 1;
    ptrSdoAdj->sdo_phase_adjust1 = 2;
    ptrSdoAdj->sdo_phase_adjust2 = 3;
    ptrSdoAdj->sdo_phase_adjust3 = 4;
    drvif_set_LdInterface_SdoAdjust(ptrSdoAdj);
    
#if 1 // Test Set Table => Pass
    // extern DRV_LDINTERFACE_Table LDINTERFACE_Table[0];
    drvif_set_LdInterface_Table(&LDINTERFACE_Table[0]);
#endif

}

bool check_LDSPI_IndexSRAM_status_NULL(DRV_LD_LDSPI_INDEXSRAM_TYPE *LDSPI_IndexSRAM){
	/*Null Protection*/
	if (LDSPI_IndexSRAM == NULL) {
		rtd_pr_vpq_emerg("drvif_color_set_LDInterface_SPI_IndexSRAM_Data_Continuous, LDSPI_IndexSRAM is NULL\n");
		return true;
	}
	if (LDSPI_IndexSRAM->SRAM_Value == NULL) {
		rtd_pr_vpq_emerg("drvif_color_set_LDInterface_SPI_IndexSRAM_Data_Continuous, LDSPI_IndexSRAM->SRAM_Value is NULL\n");
		return true;
	}

	if ((LDSPI_IndexSRAM->SRAM_Position+LDSPI_IndexSRAM->SRAM_Length)>5280) {
		rtd_pr_vpq_emerg("drvif_color_set_LDInterface_SPI_IndexSRAM_Data_Continuous, Data too much\n");
		return true;
	}
	rtd_pr_vpq_emerg("check_LDSPI_IndexSRAM_status_NULL done\n");
	return false;
	
}

void drvif_color_set_LDInterface_SPI_IndexSRAM_Data_Continuous(DRV_LD_LDSPI_INDEXSRAM_TYPE *LDSPI_IndexSRAM, unsigned char Wait_Tx_Done)
{
	unsigned short i;

	unsigned int timeoutcnt = 0x32500;
	//unsigned int dummy;
	unsigned char tx_done;
	
	ldspi_outindex_addr_ctrl_RBUS ldspi_outindex_addr_ctrl;
	ldspi_outindex_rwport_RBUS ldspi_outindex_rwport;
	ldspi_wclr_ctrl_RBUS ldspi_wclr_ctrl;

	/*Null Protection*/
	if (check_LDSPI_IndexSRAM_status_NULL(LDSPI_IndexSRAM)){
		return;
	}
		
	if (Wait_Tx_Done == 1) {/*Normal Usage Wait tx_done*/
	
		do {
			ldspi_wclr_ctrl.regValue = IoReg_Read32(LDSPI_WCLR_CTRL_reg);
			tx_done = ldspi_wclr_ctrl.tx_done;
			if(tx_done == 1) {
				ldspi_outindex_addr_ctrl.regValue = IoReg_Read32(LDSPI_Outindex_Addr_CTRL_reg);
				ldspi_outindex_addr_ctrl.outindex_rw_pointer_set = LDSPI_IndexSRAM->SRAM_Position;
				IoReg_Write32(LDSPI_Outindex_Addr_CTRL_reg, ldspi_outindex_addr_ctrl.regValue);
				for (i=0; i<LDSPI_IndexSRAM->SRAM_Length; i++){
					ldspi_outindex_rwport.outindex_rw_data_port = *(LDSPI_IndexSRAM->SRAM_Value+i); 
					IoReg_Write32(LDSPI_Outindex_Rwport_reg, ldspi_outindex_rwport.regValue);		
					}
				return;
			}
		} while(--timeoutcnt != 0);
		
		if(timeoutcnt == 0){
			rtd_pr_vpq_emerg("drvif_color_set_LDInterface_SPI_IndexSRAM_Data_Continuous, timeoutcnt == 0\n");
		}
	}
	else {
			ldspi_outindex_addr_ctrl.regValue = IoReg_Read32(LDSPI_Outindex_Addr_CTRL_reg);
			ldspi_outindex_addr_ctrl.outindex_rw_pointer_set = LDSPI_IndexSRAM->SRAM_Position;
			IoReg_Write32(LDSPI_Outindex_Addr_CTRL_reg, ldspi_outindex_addr_ctrl.regValue);
			for (i=0; i<LDSPI_IndexSRAM->SRAM_Length; i++){
				ldspi_outindex_rwport.outindex_rw_data_port = *(LDSPI_IndexSRAM->SRAM_Value+i); 
				IoReg_Write32(LDSPI_Outindex_Rwport_reg, ldspi_outindex_rwport.regValue);		
				}
			rtd_pr_vpq_emerg("drvif_color_set_LDInterface_SPI_IndexSRAM_Data_Continuous, done without wait_tv_done\n");
			return;
		}
}

/*LDSPI Function*/
void drvif_color_set_LDSPI_TXDone_ISR_En(unsigned char en)
{
	ldspi_rx_data_ctrl_RBUS ldspi_rx_data_ctrl_reg;
	
	ldspi_rx_data_ctrl_reg.regValue = IoReg_Read32(LDSPI_RX_data_CTRL_reg);
	
	ldspi_rx_data_ctrl_reg.tx_done_int_en = en;
	ldspi_rx_data_ctrl_reg.ld_spi_int2_en = 1;
	
	IoReg_Write32(LDSPI_RX_data_CTRL_reg, ldspi_rx_data_ctrl_reg.regValue);
}









