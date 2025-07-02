//merlin8
#define M8_MDOMAIN_CAP_START_REGISTER_0                                            (0xb8027200)
#define M8_MDOMAIN_CAP_END_REGISTER_0                                              (0xb80272f4)
#define M8_MDOMAIN_CAP_REGISTER_NUM_0                                              ((M8_MDOMAIN_CAP_END_REGISTER_0 - M8_MDOMAIN_CAP_START_REGISTER_0 + 4)/4)
#define M8_MDOMAIN_CAP_register_range_0(addr)                                      (((addr>=M8_MDOMAIN_CAP_START_REGISTER_0)&&(addr<=M8_MDOMAIN_CAP_END_REGISTER_0))?1:0)

#define M8_MDOMAIN_CAP_DDR_range_0(addr)                                           (Mdomain_cap_0[(addr-M8_MDOMAIN_CAP_START_REGISTER_0)/4])

#define M8_MDOMAIN_CAP_START_REGISTER_1                                            (0xb8027800)
#define M8_MDOMAIN_CAP_END_REGISTER_1                                              (0xb802786c)
#define M8_MDOMAIN_CAP_REGISTER_NUM_1                                              ((M8_MDOMAIN_CAP_END_REGISTER_1 - M8_MDOMAIN_CAP_START_REGISTER_1 + 4)/4)
#define M8_MDOMAIN_CAP_register_range_1(addr)                                      (((addr>=M8_MDOMAIN_CAP_START_REGISTER_1)&&(addr<=M8_MDOMAIN_CAP_END_REGISTER_1))?1:0)

#define M8_MDOMAIN_CAP_DDR_range_1(addr)                                           (Mdomain_cap_1[(addr-M8_MDOMAIN_CAP_START_REGISTER_1)/4])

#define M8_MDOMAIN_CAP_START_REGISTER_2                                            (0xb8027700)
#define M8_MDOMAIN_CAP_END_REGISTER_2                                              (0xb8027710)
#define M8_MDOMAIN_CAP_REGISTER_NUM_2                                              ((M8_MDOMAIN_CAP_END_REGISTER_2 - M8_MDOMAIN_CAP_START_REGISTER_2 + 4)/4)
#define M8_MDOMAIN_CAP_register_range_2(addr)                                      (((addr>=M8_MDOMAIN_CAP_START_REGISTER_2)&&(addr<=M8_MDOMAIN_CAP_END_REGISTER_2))?1:0)

#define M8_MDOMAIN_CAP_DDR_range_2(addr)                                           (Mdomain_cap_2[(addr-M8_MDOMAIN_CAP_START_REGISTER_2)/4])


//merlin8p
#include <rbus/M8P_mdomain_cap_reg.h>
#include <rbus/M8P_mdom_sub_cap_reg.h>

#define M8p_MDOMAIN_CAP_START_REGISTER_0                                            (0xb8027000)
#define M8P_MDOMAIN_CAP_END_REGISTER_0                                              (0xB80271F4)
#define M8P_MDOMAIN_CAP_REGISTER_NUM_0                                              ((M8P_MDOMAIN_CAP_END_REGISTER_0 - M8P_MDOMAIN_CAP_REGISTER_NUM_0 + 4)/4)
#define M8P_MDOMAIN_CAP_register_range_0(addr)                                      (((addr>=M8P_MDOMAIN_CAP_REGISTER_NUM_0)&&(addr<=M8P_MDOMAIN_CAP_END_REGISTER_0))?1:0)

#define M8P_MDOMAIN_CAP_DDR_range_0(addr)                                           ((((addr>=M8P_MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_line_step_reg)&&(addr<=M8P_MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg)) \
                                                                                    || ((addr>=M8P_MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_line_step_reg) && (addr<=M8P_MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_word_reg)) \
                                                                                    || ((addr>=M8P_MDOMAIN_CAP_Cap0_buf_control_reg) && (addr<=M8P_MDOMAIN_CAP_frc_cap0_wdma_vs_delay_reg)) \
                                                                                    || ((addr>=M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_reg) && (addr<=M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BALANCE_reg)) \
                                                                                    || ((addr>=M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BLK0_ADD0_reg) && (addr<=M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_BLK0_ADD1_reg)) \
                                                                                    || (addr==M8P_MDOMAIN_CAP_DISPM0_PQ_CMP_QP_ST_reg) \
                                                                                    || ((addr==M8P_MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg)) \
                                                                                    || ((addr==M8P_MDOMAIN_CAP_cap0_block_repeat_drop_reg)))?1:0)

#define M8P_MDOMAIN_CAP_START_REGISTER_1                                            (0xB8027220)
#define M8P_MDOMAIN_CAP_END_REGISTER_1                                              (0xB80273F4)
#define M8P_MDOMAIN_CAP_REGISTER_NUM_1                                              ((M8P_MDOMAIN_CAP_END_REGISTER_1 - M8P_MDOMAIN_CAP_START_REGISTER_1 + 4)/4)
#define M8P_MDOMAIN_CAP_register_range_1(addr)                                      (((addr>=M8P_MDOMAIN_CAP_START_REGISTER_1)&&(addr<=M8P_MDOMAIN_CAP_END_REGISTER_1))?1:0)

#define M8P_MDOMAIN_CAP_DDR_range_1(addr)                                           ((((addr>=M8P_MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_line_step_reg)&&(addr<=M8P_MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg)) \
                                                                                    || ((addr>=M8P_MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_line_step_reg) && (addr<=M8P_MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_word_reg)) \
                                                                                    || ((addr>=M8P_MDOM_SUB_CAP_Cap1_buf_control_reg) && (addr<=M8P_MDOM_SUB_CAP_cap1_buffer_change_line_num_0_reg)) \
                                                                                    || ((addr>=M8P_MDOM_SUB_CAP_cap1_Pxl_Wrap_Ctrl_0_reg) && (addr<=M8P_MDOM_SUB_CAP_frc_cap1_wdma_vs_delay_reg)) \
                                                                                    || ((addr>=M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_reg) && (addr<=M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_BALANCE_reg)) \
                                                                                    || (addr==M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_QP_ST_reg) \
                                                                                    || ((addr>=M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_BLK0_ADD0_reg) && (addr<=M8P_MDOM_SUB_CAP_DISPM1_PQ_CMP_BLK0_ADD1_reg)))?1:0)

#define M8P_MDOMAIN_CAP_START_REGISTER_2                                            (0)
#define M8P_MDOMAIN_CAP_END_REGISTER_2                                              (0)
#define M8P_MDOMAIN_CAP_REGISTER_NUM_2                                              ((M8P_MDOMAIN_CAP_END_REGISTER_2 - M8P_MDOMAIN_CAP_START_REGISTER_2 + 4)/4)
#define M8P_MDOMAIN_CAP_register_range_2(addr)                                      (((addr>=M8P_MDOMAIN_CAP_START_REGISTER_2)&&(addr<=M8P_MDOMAIN_CAP_END_REGISTER_2))?1:0)

#define M8P_MDOMAIN_CAP_DDR_range_2(addr)                                           0//(Mdomain_cap_2[(addr-M8P_MDOMAIN_CAP_START_REGISTER_2)/4])




#define MDOMAIN_CAP_START_REGISTER_0                                            ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_START_REGISTER_0) : (M8P_MDOMAIN_CAP_START_REGISTER_0))                                                                               
#define MDOMAIN_CAP_END_REGISTER_0                                              ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_END_REGISTER_0) : (M8P_MDOMAIN_CAP_END_REGISTER_0)) 
#define MDOMAIN_CAP_REGISTER_NUM_0                                              ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_REGISTER_NUM_0) : (M8P_MDOMAIN_CAP_REGISTER_NUM_0)) 
#define MDOMAIN_CAP_register_range_0(addr)                                      ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_register_range_0(addr)) : (M8P_MDOMAIN_CAP_register_range_0(addr))) 

#define MDOMAIN_CAP_DDR_range_0(addr)                                           ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_DDR_range_0(addr)) : (M8P_MDOMAIN_CAP_DDR_range_0(addr))) 

#define MDOMAIN_CAP_START_REGISTER_1                                            ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_START_REGISTER_1) : (M8P_MDOMAIN_CAP_START_REGISTER_1))                                                                               
#define MDOMAIN_CAP_END_REGISTER_1                                              ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_END_REGISTER_1) : (M8P_MDOMAIN_CAP_END_REGISTER_1)) 
#define MDOMAIN_CAP_REGISTER_NUM_1                                             ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_REGISTER_NUM_1) : (M8P_MDOMAIN_CAP_REGISTER_NUM_1)) 
#define MDOMAIN_CAP_register_range_1(addr)                                      ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_register_range_1(addr)) : (M8P_MDOMAIN_CAP_register_range_1(addr))) 

#define MDOMAIN_CAP_DDR_range_1(addr)                                           ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_DDR_range_1(addr)) : (M8P_MDOMAIN_CAP_DDR_range_1(addr))) 

#define MDOMAIN_CAP_START_REGISTER_2                                            ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_START_REGISTER_2) : (M8P_MDOMAIN_CAP_START_REGISTER_2))                                                                               
#define MDOMAIN_CAP_END_REGISTER_2                                              ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_END_REGISTER_2) : (M8P_MDOMAIN_CAP_END_REGISTER_2)) 
#define MDOMAIN_CAP_REGISTER_NUM_2                                              ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_REGISTER_NUM_2) : (M8P_MDOMAIN_CAP_REGISTER_NUM_2)) 
#define MDOMAIN_CAP_register_range_2(addr)                                      ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_register_range_2(addr)) : (M8P_MDOMAIN_CAP_register_range_2(addr))) 

#define MDOMAIN_CAP_DDR_range_2(addr)                                           ((get_mach_type() == MACH_ARCH_RTK2885P) ? (M8_MDOMAIN_CAP_DDR_range_2(addr)) : (M8P_MDOMAIN_CAP_DDR_range_2(addr))) 


