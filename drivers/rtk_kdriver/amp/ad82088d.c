/*
 * ad82088d.c  --  ad82088d ALSA SoC Audio driver
 *
 * Copyright 1998 Elite Semiconductor Memory Technology
 *
 * Author: ESMT Audio/Power Product BU Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public  version 2 as
 * published by the Free Software Foundation.
 */
#ifndef BUILD_QUICK_SHOW
#include <linux/i2c.h>
#include <linux/delay.h>
#else
#include <timer.h>
#endif
#include "rtk_amp_interface.h"
#include <rtk_kdriver/pcbMgr.h>
#include "ad82088d.h"

typedef struct
{
    unsigned char bAddr;
    unsigned char bArray;
}AD82088D_REG;

#define AD82088D_NUM_SUPPLIES	ARRAY_SIZE(ad82088d_supply_names)

static AD82088D_REG AD82088D_InitTbl[] = {
                     {0x00, 0x04},//##AGC_ALPHA
                     {0x01, 0x82},//##State_Control_2
                     {0x02, 0x00},//##State_Control_3
                     {0x03, 0x18},//##Master_volume_control
                     {0x04, 0x18},//##Channel_1_volume_control
                     {0x05, 0x18},//##Channel_2_volume_control
                     {0x06, 0x18},//##Channel_3_volume_control
                     {0x07, 0x18},//##Channel_4_volume_control
                     {0x08, 0x18},//##Channel_5_volume_control
                     {0x09, 0x18},//##Channel_6_volume_control
                     {0x0a, 0x00},//##Reserve
                     {0x0b, 0x00},//##Reserve
                     {0x0c, 0x90},//##State_Control_4
                     {0x0d, 0xc0},//##Channel_1_configuration_registers
                     {0x0e, 0xc0},//##Channel_2_configuration_registers
                     {0x0f, 0xc0},//##Channel_3_configuration_registers
                     {0x10, 0xc0},//##Channel_4_configuration_registers
                     {0x11, 0xc0},//##Channel_5_configuration_registers
                     {0x12, 0xc0},//##Channel_6_configuration_registers
                     {0x13, 0xc0},//##Channel_7_configuration_registers
                     {0x14, 0xc0},//##Channel_8_configuration_registers
                     {0x15, 0x62},//##Q_PWM_DUTY 
                     {0x16, 0x20},//##clock_and_FS_detection
                     {0x17, 0xf9},//##clock_det
                     {0x18, 0x40},//##DTC
                     {0x19, 0x06},//##Error_Delay
                     {0x1a, 0xaa},//##State_Control_5
                     {0x1b, 0x80},//##State_Control_6
                     {0x1c, 0x80},//##State_Control_7
                     {0x1d, 0x00},//##Coefficient_RAM_Base_Address
                     {0x1e, 0x00},//##First_4-bits_of_coefficients_A1
                     {0x1f, 0x00},//##Second_8-bits_of_coefficients_A1
                     {0x20, 0x00},//##Third_8-bits_of_coefficients_A1
                     {0x21, 0x00},//##Fourth-bits_of_coefficients_A1
                     {0x22, 0x00},//##First_4-bits_of_coefficients_A2
                     {0x23, 0x00},//##Second_8-bits_of_coefficients_A2
                     {0x24, 0x00},//##Third_8-bits_of_coefficients_A2
                     {0x25, 0x00},//##Fourth_8-bits_of_coefficients_A2
                     {0x26, 0x00},//##First_4-bits_of_coefficients_B1
                     {0x27, 0x00},//##Second_8-bits_of_coefficients_B1
                     {0x28, 0x00},//##Third_8-bits_of_coefficients_B1
                     {0x29, 0x00},//##Fourth_8-bits_of_coefficients_B1
                     {0x2a, 0x00},//##First_4-bits_of_coefficients_B2
                     {0x2b, 0x00},//##Second_8-bits_of_coefficients_B2
                     {0x2c, 0x00},//##Third_8-bits_of_coefficients_B2
                     {0x2d, 0x00},//##Fourth-bits_of_coefficients_B2
                     {0x2e, 0x00},//##First_4-bits_of_coefficients_A0
                     {0x2f, 0x80},//##Second_8-bits_of_coefficients_A0
                     {0x30, 0x00},//##Third_8-bits_of_coefficients_A0
                     {0x31, 0x00},//##Fourth_8-bits_of_coefficients_A0
                     {0x32, 0x00},//##Coefficient_RAM_R/W_control
                     {0x33, 0x06},//##State_Control_8
                     {0x34, 0x00},//##AGL_control
                     {0x35, 0x00},//##Volume_Fine_tune
                     {0x36, 0x00},//##Volume_Fine_tune
                     {0x37, 0x6c},//##Device_ID_register
                     {0x38, 0x00},//##Level_Meter_Clear
                     {0x39, 0x00},//##Power_Meter_Clear
                     {0x3a, 0x00},//##First_8bits_of_C1_Level_Meter
                     {0x3b, 0x07},//##Second_8bits_of_C1_Level_Meter
                     {0x3c, 0xa4},//##Third_8bits_of_C1_Level_Meter
                     {0x3d, 0xa3},//##Fourth_8bits_of_C1_Level_Meter
                     {0x3e, 0x00},//##First_8bits_of_C2_Level_Meter
                     {0x3f, 0x07},//##Second_8bits_of_C2_Level_Meter
                     {0x40, 0x95},//##Third_8bits_of_C2_Level_Meter
                     {0x41, 0x92},//##Fourth_8bits_of_C2_Level_Meter
                     {0x42, 0x00},//##First_8bits_of_C3_Level_Meter
                     {0x43, 0x00},//##Second_8bits_of_C3_Level_Meter
                     {0x44, 0x00},//##Third_8bits_of_C3_Level_Meter
                     {0x45, 0x00},//##Fourth_8bits_of_C3_Level_Meter
                     {0x46, 0x00},//##First_8bits_of_C4_Level_Meter
                     {0x47, 0x00},//##Second_8bits_of_C4_Level_Meter
                     {0x48, 0x00},//##Third_8bits_of_C4_Level_Meter
                     {0x49, 0x00},//##Fourth_8bits_of_C4_Level_Meter
                     {0x4a, 0x00},//##First_8bits_of_C5_Level_Meter
                     {0x4b, 0x00},//##Second_8bits_of_C5_Level_Meter
                     {0x4c, 0x00},//##Third_8bits_of_C5_Level_Meter
                     {0x4d, 0x00},//##Fourth_8bits_of_C5_Level_Meter
                     {0x4e, 0x00},//##First_8bits_of_C6_Level_Meter
                     {0x4f, 0x00},//##Second_8bits_of_C6_Level_Meter
                     {0x50, 0x00},//##Third_8bits_of_C6_Level_Meter
                     {0x51, 0x00},//##Fourth_8bits_of_C6_Level_Meter
                     {0x52, 0x00},//##First_8bits_of_C7_Level_Meter
                     {0x53, 0x00},//##Second_8bits_of_C7_Level_Meter
                     {0x54, 0x00},//##Third_8bits_of_C7_Level_Meter
                     {0x55, 0x00},//##Fourth_8bits_of_C7_Level_Meter
                     {0x56, 0x00},//##First_8bits_of_C8_Level_Meter
                     {0x57, 0x00},//##Second_8bits_of_C8_Level_Meter
                     {0x58, 0x00},//##Third_8bits_of_C8_Level_Meter
                     {0x59, 0x00},//##Fourth_8bits_of_C8_Level_Meter
                     {0x5a, 0x07},//##I2S_data_output_selection_register
                     {0x5b, 0x00},//##Mono_Key_High_Byte
                     {0x5c, 0x00},//##Mono_Key_Low_Byte
                     {0x5d, 0x07},//##Hi-res_Item
                     {0x5e, 0x00},//##TDM_Word_Width_Selection
                     {0x5f, 0x00},//##TDM_Offset
                     {0x60, 0x00},//##Channel1_EQ_bypass_1
                     {0x61, 0x00},//##Channel1_EQ_bypass_2
                     {0x62, 0x00},//##Channel1_EQ_bypass_3
                     {0x63, 0x00},//##Channel2_EQ_bypass_1
                     {0x64, 0x00},//##Channel2_EQ_bypass_2
                     {0x65, 0x00},//##Channel2_EQ_bypass_3
                     {0x66, 0x00},//##Compensate_filter
                     {0x67, 0x6a},//##Quatenary_Ternary_Switch_Level
                     {0x68, 0x0e},//##PWM_shift
                     {0x69, 0xff},//##Error_Register
                     {0x6a, 0xff},//##Error_Latch_Register
                     {0x6b, 0x00},//##Error_Clear_Register
                     {0x6c, 0x02},//##FS_and_PMF_read_out
                     {0x6d, 0x88},//##OC_Selection
                     {0x6e, 0x01},//##PD_Control
                     {0x6f, 0x60},//##OC_Bypass_GVDD_Selection
                     {0x70, 0x62},//##BSOV_BSUV_Selection
                     {0x71, 0x40},//##Testmode_register1
                     {0x72, 0xcc},//##Testmode_register2
                     {0x73, 0x00},//##ATTACK_HOLD_SET
                     {0x74, 0x00},//##Reserve
                     {0x75, 0x05},//##First_4bits_of_MBIST_User_Program_Even
                     {0x76, 0x55},//##Second_8bits_of_MBIST_User_Program_Even
                     {0x77, 0x55},//##Third_8bits_of_MBIST_User_Program_Even
                     {0x78, 0x55},//##Fourth_8bits_of_MBIST_User_Program_Even
                     {0x79, 0x55},//##Fifth_8bits_of_MBIST_User_Program_Even
                     {0x7a, 0x05},//##First_8bits_of_MBIST_User_Program_Odd
                     {0x7b, 0x55},//##Second_8bits_of_MBIST_User_Program_Odd
                     {0x7c, 0x55},//##Third_8bits_of_MBIST_User_Program_Odd
                     {0x7d, 0x55},//##Fourth_8bits_of_MBIST_User_Program_Odd
                     {0x7e, 0x55},//##Fifth_8bits_of_MBIST_User_Program_Odd
                     {0x7f, 0x06},//##IPD_FBD_read
                     {0x80, 0x80},//##pll_control
                     {0x81, 0x09},//##pll_control2
                     {0x82, 0x00},//##Protection_register_set
                     {0x83, 0x00},//##Memory_MBIST_status
                     {0x84, 0x00},//##PWM_output_control
                     {0x85, 0x00},//##Testmode_control_register
                     {0x86, 0x00},//##RAM1_test_register_address
                     {0x87, 0x00},//##First_4bits_of_RAM1_data
                     {0x88, 0x00},//##Second_8bits_of_RAM1_Data
                     {0x89, 0x00},//##Third_8bits_of_RAM1_data
                     {0x8a, 0x00},//##Fourth_8bits_of_RAM1_Data
                     {0x8b, 0x00},//##Fifth_8bits_of_RAM1_Data
                     {0x8c, 0x00},//##RAM1_test_r/w_control
                     {0x8d, 0x00},//##RAM2_test_register_address
                     {0x8e, 0x00},//##First_4bits_of_RAM2_data
                     {0x8f, 0x00},//##Second_8bits_of_RAM2_Data
                     {0x90, 0x00},//##Third_8bits_of_RAM2_data
                     {0x91, 0x00},//##Fourth_8bits_of_RAM2_Data
                     {0x92, 0x00},//##Fifth_8bits_of_RAM2_Data
                     {0x93, 0x00},//##RAM2_test_r/w_control
                     {0x94, 0x10},//##CLKDET_read_out2
                     {0x95, 0x02},//##CLKDET_read_out3
                     {0x96, 0x09},//##CLKDET_read_out4
                     {0x97, 0x90},//##CLKDET_read_out5
                     {0x98, 0x3f},//##CLKDET_read_out6
                     {0x99, 0x00},//##CLKDET_read_out7
                     {0x9a, 0x02},//##CLKDET_read_out8
                     {0x9b, 0x04},//##RE_COUNT_CTRL
                     {0x9c, 0xc9},//##RE_COUNT_Stable_CTRL
                     {0x9d, 0x00},//##BURN_EN1
                     {0x9e, 0x00},//##BURN_EN2
                     {0x9f, 0x16},//##OSC_TRIM_CTRL
                     {0xa0, 0x00},//##RAM_clock_gating_cell_control
                     {0xa1, 0x04},//##FS96k_window
                     {0xa2, 0x04},//##FS48k_window
                     {0xa3, 0x04},//##FS16k_window
                     {0xa4, 0x04},//##FS8k_window
                     {0xa5, 0x06},//##GVDD_UV_clear_OC
                     {0xa6, 0x80},//##SYNC_LR
                     {AD82088D_END_FLAG ,0x00}
};

int ad82088d_mute_set(int on_off, int amp_i2c_id,unsigned short slave_addr)
{
    unsigned char data[2] ={0};

    data[0] = 0x02;
    if(on_off){
        AMP_WARN("AD82050 unmute amp\n");
        data[1] = 0x00;    //--unmute amp
    }
    else{
        AMP_WARN("AD82050 mute amp\n");
        data[1] = 0x7f;    //--mute amp
    }
    return i2c_master_send_ex(amp_i2c_id, slave_addr, data, 2); 

}



void ad82088d_amp_reset(void)
{

    AMP_WARN("Ad82088D_amp_reset\n");
    rtk_amp_pin_set("PIN_AMP_RESET", 1);
    mdelay(1);
    rtk_amp_pin_set("PIN_AMP_MUTE", 1);
    mdelay(30);
}

void ad82088d_func(int amp_i2c_id, unsigned short addr)
{
    unsigned char data[2];
    unsigned char data_len=1;
    unsigned char index=0;
    AMP_WARN("\nAD82088D Initial Start\n");
    // software reset amp
    data[0] = 0x1a;
    data[1] = 0x00;
    if(i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1 ,I2C_M_FAST_SPEED) < 0){
        AMP_ERR("AD82088D err @ _line : %d \n", __LINE__);
    } //--reset amp
    mdelay(5);

    data[0] = 0x1a;
    data[1] = 0x20;
    if(i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1 ,I2C_M_FAST_SPEED) < 0){
        AMP_ERR("AD82088D err @ _line : %d \n", __LINE__);
    } //--reset amp
    mdelay(20);

    data[0] = 0x02;
    data[1] = 0x7f;
    if(i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1 ,I2C_M_FAST_SPEED) < 0){
        AMP_ERR("AD82088D err @ _line : %d \n", __LINE__);
    } //--mute amp

    for(index = 0; index < (sizeof(AD82088D_InitTbl)/sizeof(AD82088D_REG)); index ++)
    {

        data[0] = AD82088D_InitTbl[index].bAddr;
        data_len = 1;
        data[1] = AD82088D_InitTbl[index].bArray;
        if(data[0] == AD82088D_END_FLAG)
            break;
#ifdef AD82088D_DEBUG
        AMP_WARN("===write addr:0x%x len:%d===\n",data[0],data_len);
        AMP_WARN("%x ", data[1]);
#endif
        if (i2c_master_send_ex_flag(amp_i2c_id, addr , data, data_len+1 ,I2C_M_FAST_SPEED) < 0)
            AMP_ERR("program AD82088D failed\n");
    }

    data[0] = 0x02;
    data[1] = 0x00;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1 ,I2C_M_FAST_SPEED) < 0) //--unmute amp
        AMP_ERR("%s line: %d, program AD82088D failed\n", __func__, __LINE__);
    AMP_WARN("\nAD82088D Initial End\n");
}

void ad82088d_amp_mute_set(int value)
{
    int ret = 0;
    unsigned long long amp_mute_value = 0;
    ret = pcb_mgr_get_enum_info_byname("PIN_AMP_MUTE",&amp_mute_value);
    if(ret == 0){
        rtk_amp_pin_set("PIN_AMP_MUTE", value);
    }
}

int ad82088d_param_get(unsigned char *reg, int amp_i2c_id, unsigned short slave_addr)
{
    int ret = 0;
    int result = 0;
    unsigned char addr[1] ={0};
    unsigned char data[1] ={0};

    //Read
    addr[0] = reg[0];
    if ( i2c_master_send_ex(amp_i2c_id, slave_addr, &addr[0], 1) < 0 ){
        AMP_ERR("I2C Write Address Reg:0x%x failed\n",addr[0]);
        return (-1);
    }

    result = i2c_master_recv_ex(amp_i2c_id,slave_addr,NULL,0,data,AD82088D_DATA_SIZE);

    if(result<0) {
        AMP_ERR("Read AMP REG:0x%x failed !! \n",*reg);
        return (-1);
    }

    AMP_WARN("Addr:0x%x Reg:0x%x Data:0x%x \n", slave_addr,addr[0],data[0]);
    return ret;
}

int ad82088d_param_set(unsigned char *data_wr, int amp_i2c_id, unsigned short slave_addr)
{
    unsigned char send_data[2] ={0};

    send_data[0] = data_wr[0];
    send_data[1] = data_wr[1];

    if ( i2c_master_send_ex(amp_i2c_id, slave_addr, &send_data[0], 2) < 0 ){
        AMP_ERR("I2C Write Reg:0x%x Data:0x%x failed\n",send_data[0],send_data[1]);
        return (-1);
    }
    else{
        AMP_WARN("Addr:0x%x Reg:0x%x Data:0x%x \n", slave_addr,send_data[0],send_data[1]);
    }

    return 0;
}


