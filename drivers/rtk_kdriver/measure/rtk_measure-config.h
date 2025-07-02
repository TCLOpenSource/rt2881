#ifndef __RTK_MEASURE_CONFIG_H__
#define __RTK_MEASURE_CONFIG_H__

// Rbus
#include <rbus/sys_reg_reg.h>
#include <rbus/smartfit_reg.h>
#include <rbus/offms_reg.h>
#include <rbus/onms_reg.h>
#include <rbus/sub_vgip_reg.h>
#include <rbus/vgip_reg.h>
#if defined(CONFIG_ARCH_RTK2885P)
#include <rbus/common_rx_420_reg.h>
#endif
#if defined(CONFIG_ARCH_RTK2819A)
#include <rbus/hdmi_dp_common_reg.h>
#include <rbus/hdmi_dp_common_misc_reg.h>
#endif

// register wrapper
#define GET_VGIP_CHNx_CTRL_ADDR()           (vgip_ch ? SUB_VGIP_VGIP_CHN2_CTRL_reg : VGIP_VGIP_CHN1_CTRL_reg)
#define GET_VGIP_CHNx_STATUS_ADDR()         (vgip_ch ? SUB_VGIP_VGIP_CHN2_STATUS_reg : VGIP_VGIP_CHN1_STATUS_reg)
#define GET_VGIP_CHNx_V_CAP_ADDR()          (vgip_ch ? SUB_VGIP_VGIP_CHN2_ACT_VSTA_Length_reg : VGIP_VGIP_CHN1_ACT_VSTA_Length_reg)
#define GET_VGIP_CHNx_H_CAP_ADDR()          (vgip_ch ? SUB_VGIP_VGIP_CHN2_ACT_HSTA_Width_reg : VGIP_VGIP_CHN1_ACT_HSTA_Width_reg)
#define GET_VGIP_CHNx_DELAY_ADDR()          (vgip_ch ? SUB_VGIP_VGIP_CHN2_DELAY_reg : VGIP_VGIP_CHN1_DELAY_reg)

#define GET_SFT_AUTO_FIELD_VADDR()              (SMARTFIT_AUTO_FIELD_reg)
#define GET_SFT_AUTO_H_BOUNDARY_VADDR()         (SMARTFIT_AUTO_H_BOUNDARY_reg)
#define GET_SFT_AUTO_V_BOUNDARY_VADDR()         (SMARTFIT_AUTO_V_BOUNDARY_reg)
#define GET_SFT_AUTO_ADJ_VADDR()                    (SMARTFIT_AUTO_ADJ_reg)
#define GET_SFT_AUTO_RESULT_VSTA_END_VADDR()    (SMARTFIT_AUTO_RESULT_VSTA_END_reg)
#define GET_SFT_AUTO_RESULT_HSTA_END_VADDR()    (SMARTFIT_AUTO_RESULT_HSTA_END_reg)

// HW Capability
#if defined(CONFIG_ARCH_RTK2885P) || defined(CONFIG_ARCH_RTK6702) || defined(CONFIG_ARCH_RTK6748) || defined(CONFIG_ARCH_RTK2875Q) || defined(CONFIG_ARCH_RTK2819A)
#define OFFMS_SUPPORT_DEMODE    1
#define ONMS_SUPPORT_DEMODE     1
#else
#define OFFMS_SUPPORT_DEMODE    0
#define ONMS_SUPPORT_DEMODE     0
#endif
#if defined(CONFIG_ARCH_RTK2819A)
#define  OFFMS_MES_RESULT_HSTA_END_get_off_hx_sta(data)     OFFMS_MES_RESULT_HSTA_END_MIN_get_off_hx_sta(data)
#define  OFFMS_MES_RESULT_HSTA_END_get_off_hx_end(data)     OFFMS_MES_RESULT_HSTA_END_MIN_get_off_hx_end(data)
#define  OFFMS_MES_RESULT_HSTA_END_reg                      OFFMS_MES_RESULT_HSTA_END_MIN_reg
#define  SYS_REG_SYS_SRST3_rstn_dispi1_mask                 SYS_REG_SYS_SRST3_rstn_dispipre1_mask
#define  SYS_REG_SYS_SRST3_rstn_dispi2_mask                 SYS_REG_SYS_SRST3_rstn_dispipre2_mask
#define  SYS_REG_SYS_SRST3_rstn_dispi3_mask                 SYS_REG_SYS_SRST3_rstn_dispipre3_mask
#endif
// Measure
#define HDMI_MEASURE_I3DDMA_SMART_FIT   1

#endif // __RTK_MEASURE_CONFIG_H__
