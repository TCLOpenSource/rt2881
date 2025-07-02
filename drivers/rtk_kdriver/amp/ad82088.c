#ifndef BUILD_QUICK_SHOW
#include <linux/i2c.h>
#include <linux/delay.h>
#else
#include <no_os/slab.h>
#include <timer.h>
#endif
#include "rtk_amp_interface.h"
#include <rtk_kdriver/rtk_gpio.h>
#include <rtk_kdriver/pcbMgr.h>
#include "ad82088.h"

#ifndef BUILD_QUICK_SHOW
static AD82088_REG AD82088_InitTbl[] = {
		{0x00, 0x04},//##State_Control_1
		{0x01, 0x81},//##State_Control_2
		{0x02, 0x7f},//##State_Control_3
		{0x03, 0x14},//##Master_volume_control
		{0x04, 0x18},//##Channel_1_volume_control
		{0x05, 0x18},//##Channel_2_volume_control
		{0x06, 0x18},//##Channel_3_volume_control
		{0x07, 0x18},//##Channel_4_volume_control
		{0x08, 0x18},//##Channel_5_volume_control
		{0x09, 0x18},//##Channel_6_volume_control
		{0x0a, 0x10},//##Bass_Tone_Boost_and_Cut
		{0x0b, 0x10},//##treble_Tone_Boost_and_Cut
		{0x0c, 0x98},//##State_Control_4
		{0x0d, 0x00},//##Channel_1_configuration_registers
		{0x0e, 0x00},//##Channel_2_configuration_registers
		{0x0f, 0x00},//##Channel_3_configuration_registers
		{0x10, 0x00},//##Channel_4_configuration_registers
		{0x11, 0x00},//##Channel_5_configuration_registers
		{0x12, 0x00},//##Channel_6_configuration_registers
		{0x13, 0x00},//##Channel_7_configuration_registers
		{0x14, 0x00},//##Channel_8_configuration_registers
		{0x15, 0x6a},//##DRC1_limiter_attack/release_rate
		{0x16, 0x6a},//##DRC2_limiter_attack/release_rate
		{0x17, 0x6a},//##DRC3_limiter_attack/release_rate
		{0x18, 0x6a},//##DRC4_limiter_attack/release_rate
		{0x19, 0x06},//##Error_Delay
		{0x1a, 0x32},//##State_Control_5
		{0x1b, 0x01},//##HVUV_selection
		{0x1c, 0x00},//##State_Control_6
		{0x1d, 0x7f},//##Coefficient_RAM_Base_Address
		{0x1e, 0x00},//##Top_8-bits_of_coefficients_A1
		{0x1f, 0x00},//##Middle_8-bits_of_coefficients_A1
		{0x20, 0x00},//##Bottom_8-bits_of_coefficients_A1
		{0x21, 0x00},//##Top_8-bits_of_coefficients_A2
		{0x22, 0x00},//##Middle_8-bits_of_coefficients_A2
		{0x23, 0x00},//##Bottom_8-bits_of_coefficients_A2
		{0x24, 0x00},//##Top_8-bits_of_coefficients_B1
		{0x25, 0x00},//##Middle_8-bits_of_coefficients_B1
		{0x26, 0x00},//##Bottom_8-bits_of_coefficients_B1
		{0x27, 0x00},//##Top_8-bits_of_coefficients_B2
		{0x28, 0x00},//##Middle_8-bits_of_coefficients_B2
		{0x29, 0x00},//##Bottom_8-bits_of_coefficients_B2
		{0x2a, 0x40},//##Top_8-bits_of_coefficients_A0
		{0x2b, 0x00},//##Middle_8-bits_of_coefficients_A0
		{0x2c, 0x00},//##Bottom_8-bits_of_coefficients_A0
		{0x2d, 0x40},//##Coefficient_R/W_control
		{0x2e, 0x00},//##Protection_Enable/Disable
		{0x2f, 0x00},//##Memory_BIST_status
		{0x30, 0x00},//##Power_Stage_Status(Read_only)
		{0x31, 0x00},//##PWM_Output_Control
		{0x32, 0x00},//##Test_Mode_Control_Reg.
		{0x33, 0x6d},//##Qua-Ternary/Ternary_Switch_Level
		{0x34, 0x00},//##Volume_Fine_tune
		{0x35, 0x00},//##Volume_Fine_tune
		{0x36, 0x60},//##OC_bypass_&_GVDD_selection
		{0x37, 0x52},//##Device_ID_register
		{0x38, 0x00},//##RAM1_test_register_address
		{0x39, 0x00},//##Top_8-bits_of_RAM1_Data
		{0x3a, 0x00},//##Middle_8-bits_of_RAM1_Data
		{0x3b, 0x00},//##Bottom_8-bits_of_RAM1_Data
		{0x3c, 0x00},//##RAM1_test_r/w_control
		{0x3d, 0x00},//##RAM2_test_register_address
		{0x3e, 0x00},//##Top_8-bits_of_RAM2_Data
		{0x3f, 0x00},//##Middle_8-bits_of_RAM2_Data
		{0x40, 0x00},//##Bottom_8-bits_of_RAM2_Data
		{0x41, 0x00},//##RAM2_test_r/w_control
		{0x42, 0x00},//##Level_Meter_Clear
		{0x43, 0x00},//##Power_Meter_Clear
		{0x44, 0x20},//##TOP_of_C1_Level_Meter
		{0x45, 0x00},//##Middle_of_C1_Level_Meter
		{0x46, 0x00},//##Bottom_of_C1_Level_Meter
		{0x47, 0x20},//##TOP_of_C2_Level_Meter
		{0x48, 0x00},//##Middle_of_C2_Level_Meter
		{0x49, 0x00},//##Bottom_of_C2_Level_Meter
		{0x4a, 0x00},//##TOP_of_C3_Level_Meter
		{0x4b, 0x00},//##Middle_of_C3_Level_Meter
		{0x4c, 0x00},//##Bottom_of_C3_Level_Meter
		{0x4d, 0x00},//##TOP_of_C4_Level_Meter
		{0x4e, 0x00},//##Middle_of_C4_Level_Meter
		{0x4f, 0x00},//##Bottom_of_C4_Level_Meter
		{0x50, 0x00},//##TOP_of_C5_Level_Meter
		{0x51, 0x00},//##Middle_of_C5_Level_Meter
		{0x52, 0x00},//##Bottom_of_C5_Level_Meter
		{0x53, 0x00},//##TOP_of_C6_Level_Meter
		{0x54, 0x00},//##Middle_of_C6_Level_Meter
		{0x55, 0x00},//##Bottom_of_C6_Level_Meter
		{0x56, 0x00},//##TOP_of_C7_Level_Meter
		{0x57, 0x00},//##Middle_of_C7_Level_Meter
		{0x58, 0x00},//##Bottom_of_C7_Level_Meter
		{0x59, 0x00},//##TOP_of_C8_Level_Meter
		{0x5a, 0x00},//##Middle_of_C8_Level_Meter
		{0x5b, 0x00},//##Bottom_of_C8_Level_Meter
		{0x5c, 0x06},//##I2S_Data_Output_Selection_Register
		{0x5d, 0x00},//##Reserve
		{0x5e, 0x00},//##Reserve
		{0x5f, 0x00},//##Reserve
		{0x60, 0x00},//##Reserve
		{0x61, 0x00},//##Reserve
		{0x62, 0x00},//##Reserve
		{0x63, 0x00},//##Reserve
		{0x64, 0x00},//##Reserve
		{0x65, 0x00},//##Reserve
		{0x66, 0x00},//##Reserve
		{0x67, 0x00},//##Reserve
		{0x68, 0x00},//##Reserve
		{0x69, 0x00},//##Reserve
		{0x6a, 0x00},//##Reserve
		{0x6b, 0x00},//##Reserve
		{0x6c, 0x00},//##Reserve
		{0x6d, 0x00},//##Reserve
		{0x6e, 0x00},//##Reserve
		{0x6f, 0x00},//##Reserve
		{0x70, 0x00},//##Reserve
		{0x71, 0x00},//##Reserve
		{0x72, 0x00},//##Reserve
		{0x73, 0x00},//##Reserve
		{0x74, 0x00},//##Mono_Key_High_Byte
		{0x75, 0x00},//##Mono_Key_Low_Byte
		{0x76, 0x00},//##Boost_Control
		{0x77, 0x07},//##Hi-res_Item
		{0x78, 0x40},//##Test_Mode_register
		{0x79, 0x62},//##Boost_Strap_OV/UV_Selection
		{0x7a, 0x8c},//##OC_Selection_2
		{0x7b, 0x55},//##MBIST_User_Program_Top_Byte_Even
		{0x7c, 0x55},//##MBIST_User_Program_Middle_Byte_Even
		{0x7d, 0x55},//##MBIST_User_Program_Bottom_Byte_Even
		{0x7e, 0x55},//##MBIST_User_Program_Top_Byte_Odd
		{0x7f, 0x55},//##MBIST_User_Program_Middle_Byte_Odd
		{0x80, 0x55},//##MBIST_User_Program_Bottom_Byte_Odd
		{0x81, 0x00},//##ERROR_clear_register
		{0x82, 0x0c},//##Minimum_duty_test
		{0x83, 0x06},//##Reserve
		{0x84, 0xfe},//##Reserve
		{0x85, 0xfe},//##Reserve
};


#else
//###################Register_Table {Register Address, Register Data}####################
static AD82088_REG AD82088_InitTbl[]= {
		{0x00, 0x04},//##State_Control_1
		{0x01, 0x81},//##State_Control_2
		{0x02, 0x00},//##State_Control_3
		{0x03, 0x12},//##Master_volume_control
		{0x04, 0x16},//##Channel_1_volume_control
		{0x05, 0x16},//##Channel_2_volume_control
		{0x06, 0x18},//##Channel_3_volume_control
		{0x07, 0x18},//##Channel_4_volume_control
		{0x08, 0x18},//##Channel_5_volume_control
		{0x09, 0x18},//##Channel_6_volume_control
		{0x0a, 0x10},//##Bass_Tone_Boost_and_Cut
		{0x0b, 0x10},//##treble_Tone_Boost_and_Cut
		{0x0c, 0x98},//##State_Control_4
		{0x0d, 0x00},//##Channel_1_configuration_registers
		{0x0e, 0x00},//##Channel_2_configuration_registers
		{0x0f, 0x00},//##Channel_3_configuration_registers
		{0x10, 0x00},//##Channel_4_configuration_registers
		{0x11, 0x00},//##Channel_5_configuration_registers
		{0x12, 0x00},//##Channel_6_configuration_registers
		{0x13, 0x00},//##Channel_7_configuration_registers
		{0x14, 0x00},//##Channel_8_configuration_registers
		{0x15, 0x6a},//##DRC1_limiter_attack/release_rate
		{0x16, 0x6a},//##DRC2_limiter_attack/release_rate
		{0x17, 0x6a},//##DRC3_limiter_attack/release_rate
		{0x18, 0x6a},//##DRC4_limiter_attack/release_rate
		{0x19, 0x06},//##Error_Delay
		{0x1a, 0x32},//##State_Control_5
		{0x1b, 0x85},//##HVUV_selection
		{0x1c, 0x07},//##State_Control_6
		{0x1d, 0x7f},//##Coefficient_RAM_Base_Address
		{0x1e, 0x00},//##Top_8-bits_of_coefficients_A1
		{0x1f, 0x00},//##Middle_8-bits_of_coefficients_A1
		{0x20, 0x00},//##Bottom_8-bits_of_coefficients_A1
		{0x21, 0x1f},//##Top_8-bits_of_coefficients_A2
		{0x22, 0xb3},//##Middle_8-bits_of_coefficients_A2
		{0x23, 0xc5},//##Bottom_8-bits_of_coefficients_A2
		{0x24, 0x3f},//##Top_8-bits_of_coefficients_B1
		{0x25, 0x66},//##Middle_8-bits_of_coefficients_B1
		{0x26, 0xd8},//##Bottom_8-bits_of_coefficients_B1
		{0x27, 0xe0},//##Top_8-bits_of_coefficients_B2
		{0x28, 0x97},//##Middle_8-bits_of_coefficients_B2
		{0x29, 0xc4},//##Bottom_8-bits_of_coefficients_B2
		{0x2a, 0x1f},//##Top_8-bits_of_coefficients_A0
		{0x2b, 0xb3},//##Middle_8-bits_of_coefficients_A0
		{0x2c, 0xc5},//##Bottom_8-bits_of_coefficients_A0
		{0x2d, 0x40},//##Coefficient_R/W_control
		{0x2e, 0x00},//##Protection_Enable/Disable
		{0x2f, 0x00},//##Memory_BIST_status
		{0x30, 0x00},//##Power_Stage_Status(Read_only)
		{0x31, 0x00},//##PWM_Output_Control
		{0x32, 0x00},//##Test_Mode_Control_Reg.
		{0x33, 0x6d},//##Qua-Ternary/Ternary_Switch_Level
		{0x34, 0x00},//##Volume_Fine_tune
		{0x35, 0x00},//##Volume_Fine_tune
		{0x36, 0x60},//##OC_bypass_&_GVDD_selection
		{0x37, 0x52},//##Device_ID_register
		{0x38, 0x00},//##RAM1_test_register_address
		{0x39, 0x00},//##Top_8-bits_of_RAM1_Data
		{0x3a, 0x00},//##Middle_8-bits_of_RAM1_Data
		{0x3b, 0x00},//##Bottom_8-bits_of_RAM1_Data
		{0x3c, 0x00},//##RAM1_test_r/w_control
		{0x3d, 0x00},//##RAM2_test_register_address
		{0x3e, 0x00},//##Top_8-bits_of_RAM2_Data
		{0x3f, 0x00},//##Middle_8-bits_of_RAM2_Data
		{0x40, 0x00},//##Bottom_8-bits_of_RAM2_Data
		{0x41, 0x00},//##RAM2_test_r/w_control
		{0x42, 0x00},//##Level_Meter_Clear
		{0x43, 0x00},//##Power_Meter_Clear
		{0x44, 0x7b},//##TOP_of_C1_Level_Meter
		{0x45, 0x42},//##Middle_of_C1_Level_Meter
		{0x46, 0x43},//##Bottom_of_C1_Level_Meter
		{0x47, 0x67},//##TOP_of_C2_Level_Meter
		{0x48, 0x39},//##Middle_of_C2_Level_Meter
		{0x49, 0xd4},//##Bottom_of_C2_Level_Meter
		{0x4a, 0x00},//##TOP_of_C3_Level_Meter
		{0x4b, 0x00},//##Middle_of_C3_Level_Meter
		{0x4c, 0x00},//##Bottom_of_C3_Level_Meter
		{0x4d, 0x00},//##TOP_of_C4_Level_Meter
		{0x4e, 0x00},//##Middle_of_C4_Level_Meter
		{0x4f, 0x00},//##Bottom_of_C4_Level_Meter
		{0x50, 0x00},//##TOP_of_C5_Level_Meter
		{0x51, 0x00},//##Middle_of_C5_Level_Meter
		{0x52, 0x00},//##Bottom_of_C5_Level_Meter
		{0x53, 0x00},//##TOP_of_C6_Level_Meter
		{0x54, 0x00},//##Middle_of_C6_Level_Meter
		{0x55, 0x00},//##Bottom_of_C6_Level_Meter
		{0x56, 0x00},//##TOP_of_C7_Level_Meter
		{0x57, 0x00},//##Middle_of_C7_Level_Meter
		{0x58, 0x00},//##Bottom_of_C7_Level_Meter
		{0x59, 0x00},//##TOP_of_C8_Level_Meter
		{0x5a, 0x00},//##Middle_of_C8_Level_Meter
		{0x5b, 0x00},//##Bottom_of_C8_Level_Meter
		{0x5c, 0x06},//##I2S_Data_Output_Selection_Register
		{0x5d, 0x00},//##Reserve
		{0x5e, 0x00},//##Reserve
		{0x5f, 0x00},//##Reserve
		{0x60, 0x00},//##Reserve
		{0x61, 0x00},//##Reserve
		{0x62, 0x00},//##Reserve
		{0x63, 0x00},//##Reserve
		{0x64, 0x00},//##Reserve
		{0x65, 0x00},//##Reserve
		{0x66, 0x00},//##Reserve
		{0x67, 0x00},//##Reserve
		{0x68, 0x00},//##Reserve
		{0x69, 0x00},//##Reserve
		{0x6a, 0x00},//##Reserve
		{0x6b, 0x00},//##Reserve
		{0x6c, 0x00},//##Reserve
		{0x6d, 0x00},//##Reserve
		{0x6e, 0x00},//##Reserve
		{0x6f, 0x00},//##Reserve
		{0x70, 0x00},//##Reserve
		{0x71, 0x00},//##Reserve
		{0x72, 0x00},//##Reserve
		{0x73, 0x00},//##Reserve
		{0x74, 0x00},//##Mono_Key_High_Byte
		{0x75, 0x00},//##Mono_Key_Low_Byte
		{0x76, 0x00},//##Boost_Control
		{0x77, 0x07},//##Hi-res_Item
		{0x78, 0x40},//##Test_Mode_register
		{0x79, 0x62},//##Boost_Strap_OV/UV_Selection
		{0x7a, 0x8c},//##OC_Selection_2
		{0x7b, 0x55},//##MBIST_User_Program_Top_Byte_Even
		{0x7c, 0x55},//##MBIST_User_Program_Middle_Byte_Even
		{0x7d, 0x55},//##MBIST_User_Program_Bottom_Byte_Even
		{0x7e, 0x55},//##MBIST_User_Program_Top_Byte_Odd
		{0x7f, 0x55},//##MBIST_User_Program_Middle_Byte_Odd
		{0x80, 0x55},//##MBIST_User_Program_Bottom_Byte_Odd
		{0x81, 0x00},//##ERROR_clear_register
		{0x82, 0x0c},//##Minimum_duty_test
		{0x83, 0x06},//##Reserve
		{0x84, 0xfe},//##ERR_reg
		{0x85, 0xca},//##ERR_record
};

//###################RAM1_Table {RAM1 Address, Top Byte Of RAM1 Data , Middle Byte Of RAM1 Data , Botton Byte Of RAM1 Data }####################
static unsigned char AD82088_ram1_tab[][4]= {
                     {0x00, 0xc0, 0x98, 0x76},//##Channel_1_EQ1_A1 
                     {0x01, 0x1f, 0xb3, 0xc5},//##Channel_1_EQ1_A2 
                     {0x02, 0x3f, 0x66, 0xd8},//##Channel_1_EQ1_B1 
                     {0x03, 0xe0, 0x97, 0xc4},//##Channel_1_EQ1_B2 
                     {0x04, 0x1f, 0xb3, 0xc5},//##Channel_1_EQ1_A0 
                     {0x05, 0xc0, 0x98, 0x76},//##Channel_1_EQ2_A1 
                     {0x06, 0x1f, 0xb3, 0xc5},//##Channel_1_EQ2_A2 
                     {0x07, 0x3f, 0x66, 0xd8},//##Channel_1_EQ2_B1 
                     {0x08, 0xe0, 0x97, 0xc4},//##Channel_1_EQ2_B2 
                     {0x09, 0x1f, 0xb3, 0xc5},//##Channel_1_EQ2_A0 
                     {0x0a, 0x00, 0x00, 0x00},//##Channel_1_EQ3_A1 
                     {0x0b, 0x00, 0x00, 0x00},//##Channel_1_EQ3_A2 
                     {0x0c, 0x00, 0x00, 0x00},//##Channel_1_EQ3_B1 
                     {0x0d, 0x00, 0x00, 0x00},//##Channel_1_EQ3_B2 
                     {0x0e, 0x20, 0x00, 0x00},//##Channel_1_EQ3_A0 
                     {0x0f, 0x00, 0x00, 0x00},//##Channel_1_EQ4_A1 
                     {0x10, 0x00, 0x00, 0x00},//##Channel_1_EQ4_A2 
                     {0x11, 0x00, 0x00, 0x00},//##Channel_1_EQ4_B1 
                     {0x12, 0x00, 0x00, 0x00},//##Channel_1_EQ4_B2 
                     {0x13, 0x20, 0x00, 0x00},//##Channel_1_EQ4_A0 
                     {0x14, 0x00, 0x00, 0x00},//##Channel_1_EQ5_A1 
                     {0x15, 0x00, 0x00, 0x00},//##Channel_1_EQ5_A2 
                     {0x16, 0x00, 0x00, 0x00},//##Channel_1_EQ5_B1 
                     {0x17, 0x00, 0x00, 0x00},//##Channel_1_EQ5_B2 
                     {0x18, 0x20, 0x00, 0x00},//##Channel_1_EQ5_A0 
                     {0x19, 0x00, 0x00, 0x00},//##Channel_1_EQ6_A1 
                     {0x1a, 0x00, 0x00, 0x00},//##Channel_1_EQ6_A2 
                     {0x1b, 0x00, 0x00, 0x00},//##Channel_1_EQ6_B1 
                     {0x1c, 0x00, 0x00, 0x00},//##Channel_1_EQ6_B2 
                     {0x1d, 0x20, 0x00, 0x00},//##Channel_1_EQ6_A0 
                     {0x1e, 0x00, 0x00, 0x00},//##Channel_1_EQ7_A1 
                     {0x1f, 0x00, 0x00, 0x00},//##Channel_1_EQ7_A2 
                     {0x20, 0x00, 0x00, 0x00},//##Channel_1_EQ7_B1 
                     {0x21, 0x00, 0x00, 0x00},//##Channel_1_EQ7_B2 
                     {0x22, 0x20, 0x00, 0x00},//##Channel_1_EQ7_A0 
                     {0x23, 0x00, 0x00, 0x00},//##Channel_1_EQ8_A1 
                     {0x24, 0x00, 0x00, 0x00},//##Channel_1_EQ8_A2 
                     {0x25, 0x00, 0x00, 0x00},//##Channel_1_EQ8_B1 
                     {0x26, 0x00, 0x00, 0x00},//##Channel_1_EQ8_B2 
                     {0x27, 0x20, 0x00, 0x00},//##Channel_1_EQ8_A0 
                     {0x28, 0x00, 0x00, 0x00},//##Channel_1_EQ9_A1 
                     {0x29, 0x00, 0x00, 0x00},//##Channel_1_EQ9_A2 
                     {0x2a, 0x00, 0x00, 0x00},//##Channel_1_EQ9_B1 
                     {0x2b, 0x00, 0x00, 0x00},//##Channel_1_EQ9_B2 
                     {0x2c, 0x20, 0x00, 0x00},//##Channel_1_EQ9_A0 
                     {0x2d, 0x00, 0x00, 0x00},//##Channel_1_EQ10_A1 
                     {0x2e, 0x00, 0x00, 0x00},//##Channel_1_EQ10_A2 
                     {0x2f, 0x00, 0x00, 0x00},//##Channel_1_EQ10_B1 
                     {0x30, 0x00, 0x00, 0x00},//##Channel_1_EQ10_B2 
                     {0x31, 0x20, 0x00, 0x00},//##Channel_1_EQ10_A0 
                     {0x32, 0x00, 0x00, 0x00},//##Channel_1_EQ11_A1 
                     {0x33, 0x00, 0x00, 0x00},//##Channel_1_EQ11_A2 
                     {0x34, 0x00, 0x00, 0x00},//##Channel_1_EQ11_B1 
                     {0x35, 0x00, 0x00, 0x00},//##Channel_1_EQ11_B2 
                     {0x36, 0x20, 0x00, 0x00},//##Channel_1_EQ11_A0 
                     {0x37, 0x00, 0x00, 0x00},//##Channel_1_EQ12_A1 
                     {0x38, 0x00, 0x00, 0x00},//##Channel_1_EQ12_A2 
                     {0x39, 0x00, 0x00, 0x00},//##Channel_1_EQ12_B1 
                     {0x3a, 0x00, 0x00, 0x00},//##Channel_1_EQ12_B2 
                     {0x3b, 0x20, 0x00, 0x00},//##Channel_1_EQ12_A0 
                     {0x3c, 0x00, 0x00, 0x00},//##Channel_1_EQ13_A1 
                     {0x3d, 0x00, 0x00, 0x00},//##Channel_1_EQ13_A2 
                     {0x3e, 0x00, 0x00, 0x00},//##Channel_1_EQ13_B1 
                     {0x3f, 0x00, 0x00, 0x00},//##Channel_1_EQ13_B2 
                     {0x40, 0x20, 0x00, 0x00},//##Channel_1_EQ13_A0 
                     {0x41, 0x00, 0x00, 0x00},//##Channel_1_EQ14_A1 
                     {0x42, 0x00, 0x00, 0x00},//##Channel_1_EQ14_A2 
                     {0x43, 0x00, 0x00, 0x00},//##Channel_1_EQ14_B1 
                     {0x44, 0x00, 0x00, 0x00},//##Channel_1_EQ14_B2 
                     {0x45, 0x20, 0x00, 0x00},//##Channel_1_EQ14_A0 
                     {0x46, 0x00, 0x00, 0x00},//##Channel_1_EQ15_A1 
                     {0x47, 0x00, 0x00, 0x00},//##Channel_1_EQ15_A2 
                     {0x48, 0x00, 0x00, 0x00},//##Channel_1_EQ15_B1 
                     {0x49, 0x00, 0x00, 0x00},//##Channel_1_EQ15_B2 
                     {0x4a, 0x20, 0x00, 0x00},//##Channel_1_EQ15_A0 
                     {0x4b, 0x7f, 0xff, 0xff},//##Channel_1_Mixer1 
                     {0x4c, 0x00, 0x00, 0x00},//##Channel_1_Mixer2 
                     {0x4d, 0x7f, 0xff, 0xff},//##Channel_1_Prescale 
                     {0x4e, 0x7f, 0xff, 0xff},//##Channel_1_Postscale 
                     {0x4f, 0xc7, 0xb6, 0x91},//##A0_of_L_channel_SRS_HPF 
                     {0x50, 0x38, 0x49, 0x6e},//##A1_of_L_channel_SRS_HPF 
                     {0x51, 0x0c, 0x46, 0xf8},//##B1_of_L_channel_SRS_HPF 
                     {0x52, 0x0e, 0x81, 0xb9},//##A0_of_L_channel_SRS_LPF 
                     {0x53, 0xf2, 0x2c, 0x12},//##A1_of_L_channel_SRS_LPF 
                     {0x54, 0x0f, 0xca, 0xbb},//##B1_of_L_channel_SRS_LPF 
                     {0x55, 0x20, 0x00, 0x00},//##CH1.2_Power_Clipping 
                     {0x56, 0x01, 0xda, 0x1f},//##CCH1.2_DRC1_Attack_threshold 
                     {0x57, 0x01, 0xbf, 0x99},//##CH1.2_DRC1_Release_threshold 
                     {0x58, 0x20, 0x00, 0x00},//##CH3.4_DRC2_Attack_threshold 
                     {0x59, 0x08, 0x00, 0x00},//##CH3.4_DRC2_Release_threshold 
                     {0x5a, 0x20, 0x00, 0x00},//##CH5.6_DRC3_Attack_threshold 
                     {0x5b, 0x08, 0x00, 0x00},//##CH5.6_DRC3_Release_threshold 
                     {0x5c, 0x20, 0x00, 0x00},//##CH7.8_DRC4_Attack_threshold 
                     {0x5d, 0x08, 0x00, 0x00},//##CH7.8_DRC4_Release_threshold 
                     {0x5e, 0x00, 0x00, 0x1a},//##Noise_Gate_Attack_Level 
                     {0x5f, 0x00, 0x00, 0x53},//##Noise_Gate_Release_Level 
                     {0x60, 0x01, 0x2c, 0x00},//##DRC1_Energy_Coefficient 
                     {0x61, 0x00, 0x20, 0x00},//##DRC2_Energy_Coefficient 
                     {0x62, 0x00, 0x80, 0x00},//##DRC3_Energy_Coefficient 
                     {0x63, 0x00, 0x80, 0x00},//##DRC4_Energy_Coefficient 
                     {0x64, 0x00, 0x00, 0x00},//DRC1_Power_Meter
                     {0x65, 0x00, 0x00, 0x00},//DRC3_Power_Mete
                     {0x66, 0x00, 0x00, 0x00},//DRC5_Power_Meter
                     {0x67, 0x00, 0x00, 0x00},//DRC7_Power_Meter
                     {0x68, 0x00, 0x00, 0x00},//##Channel_1_DEQ1_A1 
                     {0x69, 0x00, 0x00, 0x00},//##Channel_1_DEQ1_A2
                     {0x6a, 0x00, 0x00, 0x00},//##Channel_1_DEQ1_B1
                     {0x6b, 0x00, 0x00, 0x00},//##Channel_1_DEQ1_B2 
                     {0x6c, 0x20, 0x00, 0x00},//##Channel_1_DEQ1_A0
                     {0x6d, 0x00, 0x00, 0x00},//##Channel_1_DEQ2_A1 
                     {0x6e, 0x00, 0x00, 0x00},//##Channel_1_DEQ2_A2 
                     {0x6f, 0x00, 0x00, 0x00},//##Channel_1_DEQ2_B1 
                     {0x70, 0x00, 0x00, 0x00},//##Channel_1_DEQ2_B2 
                     {0x71, 0x20, 0x00, 0x00},//##Channel_1_DEQ2_A0 
                     {0x72, 0x00, 0x00, 0x00},//##Channel_1_DEQ3_A1 
                     {0x73, 0x00, 0x00, 0x00},//##Channel_1_DEQ3_A2 
                     {0x74, 0x00, 0x00, 0x00},//##Channel_1_DEQ3_B1 
                     {0x75, 0x00, 0x00, 0x00},//##Channel_1_DEQ3_B2 
                     {0x76, 0x20, 0x00, 0x00},//##Channel_1_DEQ3_A0 
                     {0x77, 0x00, 0x00, 0x00},//##Channel_1_DEQ4_A1 
                     {0x78, 0x00, 0x00, 0x00},//##Channel_1_DEQ4_A2 
                     {0x79, 0x00, 0x00, 0x00},//##Channel_1_DEQ4_B1 
                     {0x7a, 0x00, 0x00, 0x00},//##Channel_1_DEQ4_B2 
                     {0x7b, 0x20, 0x00, 0x00},//##Channel_1_DEQ4_A0 
                     {0x7c, 0x00, 0x00, 0x00},//##Reserve
                     {0x7d, 0x00, 0x00, 0x00},//##Reserve
                     {0x7e, 0x00, 0x00, 0x00},//##Reserve
                     {0x7f, 0x00, 0x00, 0x00},//##Reserve
                     };

//###################RAM2_Table {RAM2 Address, Top Byte Of RAM2 Data , Middle Byte Of RAM2 Data , Botton Byte Of RAM2 Data }####################
static unsigned char AD82088_ram2_tab[][4]= {
                     {0x00, 0xc1, 0x7a, 0x70},//##Channel_2_EQ1_A1 
                     {0x01, 0x1f, 0x42, 0xc8},//##Channel_2_EQ1_A2 
                     {0x02, 0x3e, 0x81, 0x46},//##Channel_2_EQ1_B1 
                     {0x03, 0xe1, 0x76, 0x27},//##Channel_2_EQ1_B2 
                     {0x04, 0x1f, 0x42, 0xc8},//##Channel_2_EQ1_A0 
                     {0x05, 0xc1, 0xc5, 0x0d},//##Channel_2_EQ2_A1 
                     {0x06, 0x1f, 0x1d, 0x79},//##Channel_2_EQ2_A2 
                     {0x07, 0x3e, 0x34, 0xce},//##Channel_2_EQ2_B1 
                     {0x08, 0xe1, 0xbe, 0xe9},//##Channel_2_EQ2_B2 
                     {0x09, 0x1f, 0x1d, 0x79},//##Channel_2_EQ2_A0 
                     {0x0a, 0xc0, 0x9a, 0x3e},//##Channel_2_EQ3_A1 
                     {0x0b, 0x1f, 0x91, 0x4a},//##Channel_2_EQ3_A2 
                     {0x0c, 0x3f, 0x65, 0xc2},//##Channel_2_EQ3_B1 
                     {0x0d, 0xe0, 0x8d, 0xb9},//##Channel_2_EQ3_B2 
                     {0x0e, 0x1f, 0xe0, 0xfc},//##Channel_2_EQ3_A0 
                     {0x0f, 0xc0, 0xd8, 0xf1},//##Channel_2_EQ4_A1 
                     {0x10, 0x1f, 0x5e, 0xb9},//##Channel_2_EQ4_A2 
                     {0x11, 0x3f, 0x27, 0x0f},//##Channel_2_EQ4_B1 
                     {0x12, 0xe0, 0xbc, 0xdb},//##Channel_2_EQ4_B2 
                     {0x13, 0x1f, 0xe4, 0x6c},//##Channel_2_EQ4_A0 
                     {0x14, 0xc3, 0x90, 0xa8},//##Channel_2_EQ5_A1 
                     {0x15, 0x1d, 0xf7, 0x71},//##Channel_2_EQ5_A2 
                     {0x16, 0x3c, 0x6f, 0x58},//##Channel_2_EQ5_B1 
                     {0x17, 0xe2, 0x61, 0x93},//##Channel_2_EQ5_B2 
                     {0x18, 0x1f, 0xa6, 0xfc},//##Channel_2_EQ5_A0 
                     {0x19, 0x00, 0x00, 0x00},//##Channel_2_EQ6_A1 
                     {0x1a, 0x00, 0x00, 0x00},//##Channel_2_EQ6_A2 
                     {0x1b, 0x00, 0x00, 0x00},//##Channel_2_EQ6_B1 
                     {0x1c, 0x00, 0x00, 0x00},//##Channel_2_EQ6_B2 
                     {0x1d, 0x20, 0x00, 0x00},//##Channel_2_EQ6_A0 
                     {0x1e, 0x00, 0x00, 0x00},//##Channel_2_EQ7_A1 
                     {0x1f, 0x00, 0x00, 0x00},//##Channel_2_EQ7_A2 
                     {0x20, 0x00, 0x00, 0x00},//##Channel_2_EQ7_B1 
                     {0x21, 0x00, 0x00, 0x00},//##Channel_2_EQ7_B2 
                     {0x22, 0x20, 0x00, 0x00},//##Channel_2_EQ7_A0 
                     {0x23, 0x00, 0x00, 0x00},//##Channel_2_EQ8_A1 
                     {0x24, 0x00, 0x00, 0x00},//##Channel_2_EQ8_A2 
                     {0x25, 0x00, 0x00, 0x00},//##Channel_2_EQ8_B1 
                     {0x26, 0x00, 0x00, 0x00},//##Channel_2_EQ8_B2 
                     {0x27, 0x20, 0x00, 0x00},//##Channel_2_EQ8_A0 
                     {0x28, 0x00, 0x00, 0x00},//##Channel_2_EQ9_A1 
                     {0x29, 0x00, 0x00, 0x00},//##Channel_2_EQ9_A2 
                     {0x2a, 0x00, 0x00, 0x00},//##Channel_2_EQ9_B1 
                     {0x2b, 0x00, 0x00, 0x00},//##Channel_2_EQ9_B2 
                     {0x2c, 0x20, 0x00, 0x00},//##Channel_2_EQ9_A0 
                     {0x2d, 0x00, 0x00, 0x00},//##Channel_2_EQ10_A1 
                     {0x2e, 0x00, 0x00, 0x00},//##Channel_2_EQ10_A2 
                     {0x2f, 0x00, 0x00, 0x00},//##Channel_2_EQ10_B1 
                     {0x30, 0x00, 0x00, 0x00},//##Channel_2_EQ10_B2 
                     {0x31, 0x20, 0x00, 0x00},//##Channel_2_EQ10_A0 
                     {0x32, 0x00, 0x00, 0x00},//##Channel_2_EQ11_A1 
                     {0x33, 0x00, 0x00, 0x00},//##Channel_2_EQ11_A2 
                     {0x34, 0x00, 0x00, 0x00},//##Channel_2_EQ11_B1 
                     {0x35, 0x00, 0x00, 0x00},//##Channel_2_EQ11_B2 
                     {0x36, 0x20, 0x00, 0x00},//##Channel_2_EQ11_A0 
                     {0x37, 0x00, 0x00, 0x00},//##Channel_2_EQ12_A1 
                     {0x38, 0x00, 0x00, 0x00},//##Channel_2_EQ12_A2 
                     {0x39, 0x00, 0x00, 0x00},//##Channel_2_EQ12_B1 
                     {0x3a, 0x00, 0x00, 0x00},//##Channel_2_EQ12_B2 
                     {0x3b, 0x20, 0x00, 0x00},//##Channel_2_EQ12_A0 
                     {0x3c, 0x00, 0x00, 0x00},//##Channel_2_EQ13_A1 
                     {0x3d, 0x00, 0x00, 0x00},//##Channel_2_EQ13_A2 
                     {0x3e, 0x00, 0x00, 0x00},//##Channel_2_EQ13_B1 
                     {0x3f, 0x00, 0x00, 0x00},//##Channel_2_EQ13_B2 
                     {0x40, 0x20, 0x00, 0x00},//##Channel_2_EQ13_A0 
                     {0x41, 0x00, 0x00, 0x00},//##Channel_2_EQ14_A1 
                     {0x42, 0x00, 0x00, 0x00},//##Channel_2_EQ14_A2 
                     {0x43, 0x00, 0x00, 0x00},//##Channel_2_EQ14_B1 
                     {0x44, 0x00, 0x00, 0x00},//##Channel_2_EQ14_B2 
                     {0x45, 0x20, 0x00, 0x00},//##Channel_2_EQ14_A0 
                     {0x46, 0x00, 0x00, 0x00},//##Channel_2_EQ15_A1 
                     {0x47, 0x00, 0x00, 0x00},//##Channel_2_EQ15_A2 
                     {0x48, 0x00, 0x00, 0x00},//##Channel_2_EQ15_B1 
                     {0x49, 0x00, 0x00, 0x00},//##Channel_2_EQ15_B2 
                     {0x4a, 0x20, 0x00, 0x00},//##Channel_2_EQ15_A0 
                     {0x4b, 0x00, 0x00, 0x00},//##Channel_2_Mixer1 
                     {0x4c, 0x7f, 0xff, 0xff},//##Channel_2_Mixer2 
                     {0x4d, 0x7f, 0xff, 0xff},//##Channel_2_Prescale 
                     {0x4e, 0x7f, 0xff, 0xff},//##Channel_2_Postscale 
                     {0x4f, 0xc7, 0xb6, 0x91},//##A0_of_R_channel_SRS_HPF 
                     {0x50, 0x38, 0x49, 0x6e},//##A1_of_R_channel_SRS_HPF 
                     {0x51, 0x0c, 0x46, 0xf8},//##B1_of_R_channel_SRS_HPF 
                     {0x52, 0x0e, 0x81, 0xb9},//##A0_of_R_channel_SRS_LPF 
                     {0x53, 0xf2, 0x2c, 0x12},//##A1_of_R_channel_SRS_LPF 
                     {0x54, 0x0f, 0xca, 0xbb},//##B1_of_R_channel_SRS_LPF 
                     {0x55, 0x00, 0x00, 0x00},//##Reserve
                     {0x56, 0x00, 0x00, 0x00},//##Reserve
                     {0x57, 0x00, 0x00, 0x00},//##Reserve
                     {0x58, 0x00, 0x00, 0x00},//##Reserve
                     {0x59, 0x00, 0x00, 0x00},//##Reserve
                     {0x5a, 0x00, 0x00, 0x00},//##Reserve
                     {0x5b, 0x00, 0x00, 0x00},//##Reserve
                     {0x5c, 0x00, 0x00, 0x00},//##Reserve
                     {0x5d, 0x00, 0x00, 0x00},//##Reserve
                     {0x5e, 0x00, 0x00, 0x00},//##Reserve
                     {0x5f, 0x00, 0x00, 0x00},//##Reserve
                     {0x60, 0x00, 0x00, 0x00},//##Reserve
                     {0x61, 0x00, 0x00, 0x00},//##Reserve
                     {0x62, 0x00, 0x00, 0x00},//##Reserve
                     {0x63, 0x00, 0x00, 0x00},//##Reserve
                     {0x64, 0x00, 0x00, 0x00},//DRC2_Power_Meter
                     {0x65, 0x00, 0x00, 0x00},//DRC4_Power_Mete
                     {0x66, 0x00, 0x00, 0x00},//DRC6_Power_Meter
                     {0x67, 0x00, 0x00, 0x00},//DRC8_Power_Meter
                     {0x68, 0x00, 0x00, 0x00},//##Channel_2_DEQ1_A1 
                     {0x69, 0x00, 0x00, 0x00},//##Channel_2_DEQ1_A2
                     {0x6a, 0x00, 0x00, 0x00},//##Channel_2_DEQ1_B1
                     {0x6b, 0x00, 0x00, 0x00},//##Channel_2_DEQ1_B2 
                     {0x6c, 0x20, 0x00, 0x00},//##Channel_2_DEQ1_A0
                     {0x6d, 0x00, 0x00, 0x00},//##Channel_2_DEQ2_A1 
                     {0x6e, 0x00, 0x00, 0x00},//##Channel_2_DEQ2_A2 
                     {0x6f, 0x00, 0x00, 0x00},//##Channel_2_DEQ2_B1 
                     {0x70, 0x00, 0x00, 0x00},//##Channel_2_DEQ2_B2 
                     {0x71, 0x20, 0x00, 0x00},//##Channel_2_DEQ2_A0 
                     {0x72, 0x00, 0x00, 0x00},//##Channel_2_DEQ3_A1 
                     {0x73, 0x00, 0x00, 0x00},//##Channel_2_DEQ3_A2 
                     {0x74, 0x00, 0x00, 0x00},//##Channel_2_DEQ3_B1 
                     {0x75, 0x00, 0x00, 0x00},//##Channel_2_DEQ3_B2 
                     {0x76, 0x20, 0x00, 0x00},//##Channel_2_DEQ3_A0 
                     {0x77, 0x00, 0x00, 0x00},//##Channel_2_DEQ4_A1 
                     {0x78, 0x00, 0x00, 0x00},//##Channel_2_DEQ4_A2 
                     {0x79, 0x00, 0x00, 0x00},//##Channel_2_DEQ4_B1 
                     {0x7a, 0x00, 0x00, 0x00},//##Channel_2_DEQ4_B2 
                     {0x7b, 0x20, 0x00, 0x00},//##Channel_2_DEQ4_A0 
                     {0x7c, 0x00, 0x00, 0x00},//##Reserve
                     {0x7d, 0x00, 0x00, 0x00},//##Reserve
                     {0x7e, 0x00, 0x00, 0x00},//##Reserve
                     {0x7f, 0x00, 0x00, 0x00},//##Reserve
};


void write_ram_eq_func(int amp_i2c_id,unsigned short addr)
{
    unsigned char data[5];
    int index = 0;

    for(index = 0; index < (sizeof(AD82088_ram1_tab)/4); index ++)
    {
        data[0] = 0x1d;
        data[1] = AD82088_ram1_tab[index][0];
        data[2] = AD82088_ram1_tab[index][1];
        data[3] = AD82088_ram1_tab[index][2];
        data[4] = AD82088_ram1_tab[index][3];

        if (i2c_master_send_ex_flag(amp_i2c_id, addr , data, 5 ,I2C_M_FAST_SPEED) < 0)
            AMP_ERR("program AD82088 failed\n");

        data[0] = 0x2d;
        data[1] = 0x1;
	
        if (i2c_master_send_ex_flag(amp_i2c_id, addr , data, 2 ,I2C_M_FAST_SPEED) < 0)
            AMP_ERR("program AD82088 failed\n");

    }
    for(index = 0; index < (sizeof(AD82088_ram2_tab)/4); index ++)
    {
        data[0] = 0x1d;
        data[1] = AD82088_ram2_tab[index][0];
        data[2] = AD82088_ram2_tab[index][1];
        data[3] = AD82088_ram2_tab[index][2];
        data[4] = AD82088_ram2_tab[index][3];

        if (i2c_master_send_ex_flag(amp_i2c_id, addr , data, 5 ,I2C_M_FAST_SPEED) < 0)
            AMP_ERR("program AD82088 failed\n");

        data[0] = 0x2d;
        data[1] = 0x41;
	
        if (i2c_master_send_ex_flag(amp_i2c_id, addr , data, 2 ,I2C_M_FAST_SPEED) < 0)
            AMP_ERR("program AD82088 failed\n");

    }


}

#endif


void ad82088_amp_reset(void)
{
    mdelay(30);
    AMP_WARN("Ad82088_amp_reset\n");
    rtk_amp_pin_set("PIN_AMP_RESET", 0);
    rtk_amp_pin_set("PIN_AMP_MUTE", 0);
    mdelay(10);
    rtk_amp_pin_set("PIN_AMP_RESET", 1);
    mdelay(1);
    rtk_amp_pin_set("PIN_AMP_MUTE", 1);
    mdelay(30);
}

void ad82088_amp_mute_set(int value)
{
    int ret = 0;
    unsigned long long amp_mute_value = 0;
    ret = pcb_mgr_get_enum_info_byname("PIN_AMP_MUTE",&amp_mute_value);
    if(ret == 0){
        rtk_amp_pin_set("PIN_AMP_MUTE", value);
    }
}

void ad82088_func(int amp_i2c_id,unsigned short addr)
{
    unsigned char data[2];
    unsigned char data_len=1;
    unsigned char index=0;
#ifdef AD82088_DEBUG
    unsigned char i;
#endif
    int retry_cnt = 5;

    ad82088_amp_mute_set(1);
    mdelay(20);

    AMP_ERR("\nAD82088 Initial Start\n");

    // software reset amp
    data[0] = 0x1a;
    data[1] = 0x12;
    for (index = 0; index <= retry_cnt; index++) {
        if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1, I2C_M_FAST_SPEED) < 0) { //--reset amp
            AMP_ERR("%s line: %d, program AD82088 failed (retry=%d)\n", __func__, __LINE__, index);
            mdelay(5);
            if(index == retry_cnt) return;
        }
        else {
            break;
        }
    }
    mdelay(10);

    data[0] = 0x1a;
    data[1] = 0x32;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1, I2C_M_FAST_SPEED) < 0) //--Normal operation
        AMP_ERR("%s line: %d, program AD82088 failed\n", __func__, __LINE__);
    mdelay(25);

    data[0] = 0x02;
    data[1] = 0x7f;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1, I2C_M_FAST_SPEED) < 0) //--mute amp
        AMP_ERR("%s line: %d, program AD82088 failed\n", __func__, __LINE__);

    for(index = 0; index < (sizeof(AD82088_InitTbl)/sizeof(AD82088_REG)); index ++)
    {
        data[0] = AD82088_InitTbl[index].bAddr;
        data_len = 1;
        data[1] = AD82088_InitTbl[index].bArray;
        if(data[0] == 0x02)
            continue;

#ifdef AD82088_DEBUG
        AMP_WARN("===write addr:0x%x len:%d===\n",data[0],data_len);
        AMP_WARN("%x ", data[1]);
#endif
        if (i2c_master_send_ex_flag(amp_i2c_id, addr , data, data_len+1 ,I2C_M_FAST_SPEED) < 0)
            AMP_ERR("program AD82088 failed\n");
    }

    data[0] = 0x02;
    data[1] = 0x7f;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1 ,I2C_M_FAST_SPEED) < 0) //--mute amp
        AMP_ERR("%s line: %d, program AD82088 failed\n", __func__, __LINE__);
    mdelay(5);
	
#ifdef BUILD_QUICK_SHOW
    write_ram_eq_func(amp_i2c_id ,addr);
    udelay(100);
#endif

    data[0] = 0x02;
    data[1] = 0x00;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1 ,I2C_M_FAST_SPEED) < 0) //--unmute amp
        AMP_ERR("%s line: %d, program AD82088 failed\n", __func__, __LINE__);
    AMP_ERR("\nAD82088 Initial End\n");
}

int ad82088_dump_all(const char *buf, int amp_i2c_id,unsigned short slave_addr)
{
    return 0;
}

int ad82088_mute_set(int on_off, int amp_i2c_id, unsigned short slave_addr)
{
    unsigned char data[2] ={0};

    data[0] = 0x02;
    if(on_off){
        AMP_WARN("AD82120 unmute amp\n");
        data[1] = 0x00;    //--unmute amp
    }
    else{
        AMP_WARN("AD82120 mute amp\n");
        data[1] = 0x7f;    //--mute amp
    }
    return i2c_master_send_ex(amp_i2c_id, slave_addr, data, 2); 
}

int ad82088_param_get(unsigned char *reg, int amp_i2c_id, unsigned short slave_addr)
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

    result = i2c_master_recv_ex(amp_i2c_id,slave_addr,NULL,0,data,AD82120_DATA_SIZE);

    if(result<0) {
        AMP_ERR("Read AMP REG:0x%x failed !! \n",*reg);
        return (-1);
    }

    AMP_WARN("Addr:0x%x Reg:0x%x Data:0x%x \n", slave_addr,addr[0],data[0]);
    return ret;
}

int ad82088_param_set(unsigned char *data_wr, int amp_i2c_id, unsigned short slave_addr)
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
