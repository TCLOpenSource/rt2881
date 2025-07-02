#include <rbus/mdomain_cap_reg.h>
#include <rbus/mdom_sub_cap_reg.h>

#define MDOMAIN_CAP_START_REGISTER_0                                            (0xb8027000)
#define MDOMAIN_CAP_END_REGISTER_0                                              (0xB80271F4)
#define MDOMAIN_CAP_REGISTER_NUM_0                                              ((MDOMAIN_CAP_END_REGISTER_0 - MDOMAIN_CAP_START_REGISTER_0 + 4)/4)
#define MDOMAIN_CAP_register_range_0(addr)                                      (((addr>=MDOMAIN_CAP_START_REGISTER_0)&&(addr<=MDOMAIN_CAP_END_REGISTER_0))?1:0)

#define MDOMAIN_CAP_DDR_range_0(addr)                                           ((((addr>=MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_line_step_reg)&&(addr<=MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg)) \
                                                                                    || ((addr>=MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_line_step_reg) && (addr<=MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_word_reg)) \
                                                                                    || ((addr>=MDOMAIN_CAP_Cap0_buf_control_reg) && (addr<=MDOMAIN_CAP_frc_cap0_wdma_vs_delay_reg)) \
                                                                                    || ((addr>=MDOMAIN_CAP_DISPM0_PQ_CMP_reg) && (addr<=MDOMAIN_CAP_DISPM0_PQ_CMP_BALANCE_reg)) \
                                                                                    || ((addr>=MDOMAIN_CAP_DISPM0_PQ_CMP_BLK0_ADD0_reg) && (addr<=MDOMAIN_CAP_DISPM0_PQ_CMP_BLK0_ADD1_reg)) \
                                                                                    || (addr==MDOMAIN_CAP_DISPM0_PQ_CMP_QP_ST_reg) \
                                                                                    || ((addr==MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg)) \
                                                                                    || ((addr==MDOMAIN_CAP_cap0_block_repeat_drop_reg)))?1:0)

#define MDOMAIN_CAP_START_REGISTER_1                                            (0xB8027220)
#define MDOMAIN_CAP_END_REGISTER_1                                              (0xB80273F4)
#define MDOMAIN_CAP_REGISTER_NUM_1                                              ((MDOMAIN_CAP_END_REGISTER_1 - MDOMAIN_CAP_START_REGISTER_1 + 4)/4)
#define MDOMAIN_CAP_register_range_1(addr)                                      (((addr>=MDOMAIN_CAP_START_REGISTER_1)&&(addr<=MDOMAIN_CAP_END_REGISTER_1))?1:0)

#define MDOMAIN_CAP_DDR_range_1(addr)                                           ((((addr>=MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_line_step_reg)&&(addr<=MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg)) \
                                                                                    || ((addr>=MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_line_step_reg) && (addr<=MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_word_reg)) \
                                                                                    || ((addr>=MDOM_SUB_CAP_Cap1_buf_control_reg) && (addr<=MDOM_SUB_CAP_cap1_buffer_change_line_num_0_reg)) \
                                                                                    || ((addr>=MDOM_SUB_CAP_cap1_Pxl_Wrap_Ctrl_0_reg) && (addr<=MDOM_SUB_CAP_frc_cap1_wdma_vs_delay_reg)) \
                                                                                    || ((addr>=MDOM_SUB_CAP_DISPM1_PQ_CMP_reg) && (addr<=MDOM_SUB_CAP_DISPM1_PQ_CMP_BALANCE_reg)) \
                                                                                    || (addr==MDOM_SUB_CAP_DISPM1_PQ_CMP_QP_ST_reg) \
                                                                                    || (addr==MDOM_SUB_CAP_DISPM1_PQ_CMP_BP_MODE_reg) \
                                                                                    || ((addr>=MDOM_SUB_CAP_DISPM1_PQ_CMP_BLK0_ADD0_reg) && (addr<=MDOM_SUB_CAP_DISPM1_PQ_CMP_BLK0_ADD1_reg)))?1:0)

#define MDOMAIN_CAP_START_REGISTER_2                                            (0)
#define MDOMAIN_CAP_END_REGISTER_2                                              (0)
#define MDOMAIN_CAP_REGISTER_NUM_2                                              ((MDOMAIN_CAP_END_REGISTER_2 - MDOMAIN_CAP_START_REGISTER_2 + 4)/4)
#define MDOMAIN_CAP_register_range_2(addr)                                      (((addr>=MDOMAIN_CAP_START_REGISTER_2)&&(addr<=MDOMAIN_CAP_END_REGISTER_2))?1:0)

#define MDOMAIN_CAP_DDR_range_2(addr)                                           0//(Mdomain_cap_2[(addr-MDOMAIN_CAP_START_REGISTER_2)/4])

