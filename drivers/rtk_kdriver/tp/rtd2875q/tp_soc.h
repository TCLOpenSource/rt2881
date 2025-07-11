#ifndef __TP_SOC_H__
#define __TP_SOC_H__

#define TP_SPEED_MHZ                              250   /* defined by CRT spec */

#define TPO_CLK_INIT_PRE_DIV                      0x5
#define TPO_CLK_INIT_POST_DIV                     0x14

#define TPO_CLK_CIP_PRE_DIV                       9
#define TPO_CLK_CIP_POST_DIV                      9

#define REG_TPO_BYPASS_CLK_EXIST

#define SUPPORT_TP_PLL_DISPB

#define SUPPORT_TP_CP_SAVING_MODE

/* DIC resloved on RL6982-1494*/
//#define MULTI_MTP_HAS_FR_CNT_PROBLEM 1

#endif
