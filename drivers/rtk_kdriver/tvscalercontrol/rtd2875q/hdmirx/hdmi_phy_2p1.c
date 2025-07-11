/*=============================================================
 * Copyright (c)      Realtek Semiconductor Corporation, 2016
 *
 * All rights reserved.
 *
 *============================================================*/

/*======================= Description ============================
 *
 * file: 		hdmiPhy.c
 *
 * author: 	Iron Man, Tony Stark
 * date:
 * version: 	3.0
 *
 *============================================================*/

/*========================Header Files============================*/
#ifndef BUILD_QUICK_SHOW
#include <linux/kthread.h> //kthread_create()�Bkthread_run()
#include <linux/err.h> //IS_ERR()�BPTR_ERR()
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#endif
#include <mach/platform.h>
#include "hdmi_common.h"
//#include "hdmi_phy.h"
#include "hdmi_phy_dfe.h"
#include "hdmi_phy_2p1.h"
#include "hdmi_hdcp.h"
#include "hdmi_scdc.h"
#include "hdmi_reg.h"
#include "hdmi_vfe_config.h"
#include <rtk_kdriver/rtk_otp_region_api.h>
extern int rtk_otp_field_read_int_by_name(const char *name);
extern HDMI_PORT_INFO_T hdmi_rx[HDMI_PORT_TOTAL_NUM];
int factory_or_power_only_mode_hd21_eq_ofst = 2;
unsigned char PHY_DBG_MSG=0;
/**********************************************************************************************
*
*	Marco or Definitions
*
**********************************************************************************************/
#define HDMI_PHY_DBG(fmt, args...)  if(PHY_DBG_MSG) rtd_printk(KERN_DEBUG,  "[HDMI][PHY][L7]", fmt, ##args)
#define HDMI_PHY_INFO(fmt, args...)  if(PHY_DBG_MSG) rtd_printk(KERN_INFO,  "[HDMI][PHY][L6]", fmt, ##args)
#define HDMI_PHY_WARN(fmt, args...)  if(PHY_DBG_MSG) rtd_printk(KERN_WARNING,  "[HDMI][PHY][L4]", fmt, ##args)

#define HDMI_FRL_TAP0234_ADP_EN		0
#define HD21_PHY_Z0        			0x11
#define PHY_P0_Z0 	0xa //Soc Target Z0 90 ohm
#define PHY_P1_Z0		0xa//Soc Target Z0 90 ohm
#define PHY_P2_Z0		0xa//Soc Target Z0 90 ohm
#define PHY_P3_Z0		0xa//Soc Target Z0 90 ohm


#define HD21_PHY_LDO        			0x4	//LDO default value

#define HDMI_EQ_LE_NOT_ZERO_ENABLE
#define HD21_CLK_OVER_300M			1
#define FRL_DCDR_3G_6G_MODE		0

#define HD21_MAX_ERR_CHECK_CNT	200
#define HD21_DISP_ERR_CNT_THRESHOLD	4
#define LINK_TRAINING_TX_INFO_DEBUG	0

#define ADP_CNT			9
#define CALIB_CNT		7
#define ADP_TIME		6     //0.64 * 9
#define CALIB_TIME		5    //0.64 * 7
unsigned char Vth_value=3;// Vth = 3 for long Cable

#ifndef MAX
    #define MAX(a,b)                (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
    #define MIN(a,b)                (((a) < (b)) ? (a) : (b))
#endif

u32 phy_reg_mapping(u32 nport, u32 addr)
{
	if (addr >= 0x1800DA00 && addr <= 0x1800DDFF)//PHY
	{
		addr -= 0x1800DA00;
		switch(nport) {
		case 0:  addr += 0xB800DA00; break;
		case 1:  addr += 0xB803BA00; break;
		case 2:  addr += 0xB803CA00; break;
		case 3:  addr += 0xB80B2A00; break;
		default: addr += 0xB800DA00; break;
        	}
	}
	else if (addr >= 0x1800DE00 && addr <= 0x1800DEFF)//DFE
	{
		addr -= 0x1800DE00;
		switch(nport) {
		case 0:  addr += 0xB800DE00; break;
		case 1:  addr += 0xB803BE00; break;
		case 2:  addr += 0xB803CE00; break;
		case 3:  addr += 0xB80B2E00; break;
		default: addr += 0xB800DE00; break;
		}
	}
	else
	{
		HDMI_WARN("%s unknown address = %08x\n", __func__, addr);
	}
	//HDMI_WARN("p[%d] addr=%08x\n", nport, addr);
	return addr;
}

#define ScalerTimer_DelayXms(x)                 udelay(x*1000)
#define _phy_rtd_outl(addr, val)                    rtd_outl(phy_reg_mapping(nport, addr), val)
#define _phy_rtd_part_outl(addr, msb, lsb, val)     rtd_part_outl(phy_reg_mapping(nport, addr), msb, lsb, val)
#define _phy_rtd_inl(addr, val)                     rtd_inl(phy_reg_mapping(nport, addr))
#define _phy_rtd_part_inl(addr, msb, lsb)           rtd_part_inl(phy_reg_mapping(nport, addr), msb, lsb)
#define _phy_rtd_maskl(x,y,z)  		rtd_maskl(phy_reg_mapping(nport, x), y, z)


/**********************************************************************************************
*
*	Const Declarations
*
**********************************************************************************************/

////////////////////////////////////////////
//HDMI//////

#if 1
APHY_Fix_tmds_1_T APHY_Fix_tmds_1[] =
{
	{TMDS_5p94G,0x1},
	{TMDS_2p97G,0x1},
	{TMDS_1p485G,0x1},
	{TMDS_0p742G,0x1},
	{TMDS_0p25G,0x0},
	{TMDS_4p445G,0x1},
	{TMDS_3p7125G,0x1},
	{TMDS_2p2275G,0x1},
	{TMDS_1p85625G,0x1},
	{TMDS_1p11375G,0x1},
	{TMDS_0p928125G,0x1},
	{TMDS_0p54G,0x1},
	{TMDS_0p405G,0x1},
	{TMDS_0p3375G,0x1},
	{TMDS_0p27G,0x1},
};

APHY_Para_tmds_1_T APHY_Para_tmds_1[] =
{
	{TMDS_5p94G,0x0,0x0,0x0,0x3,0x3,0x3,0x3,0x24,0x0,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x1,0x1,0x1,0x1,0x1,0x1,0xb,0xb,0xb,0xb,0xb,0xb,0x0,0x0,0x0,0x3,0x3,0x3,0x1,0x1,0x1,0xd,0xd,0xd,0xf,0xf,0xf,
0xf,0xf,0xf,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x2,0x2,0x2,0x1,0x1,0x1,0x1,0x1,0x1,0xf,0xf,0xf,0x0,0x0,0x0,0x0,0x0,0x0,0x10,0x10,0x10,0x0,0x0,0x0,0x2,0x2,0x2},
	{TMDS_2p97G,0x0,0x0,0x0,0x3,0x3,0x3,0x3,0x6,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x3,0x3,0x3,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x2,0x2,0x2,0x1,0x1,0x1,0xf,0xf,0xf,0x0,0x0,0x0,0x1,0x1,0x1,0x6,0x6,0x6,0x0,0x0,0x0,0x1,0x1,0x1},
	{TMDS_1p485G,0x0,0x0,0x0,0x3,0x3,0x3,0x3,0x24,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x3,0x3,0x3,0x3,0x3,0x3,0x1,0x1,0x1,0xf,0xf,0xf,0x0,0x0,0x0,0x2,0x2,0x2,0x6,0x6,0x6,0x1,0x1,0x1,0x0,0x0,0x0},
	{TMDS_0p742G,0x1,0x1,0x1,0x3,0x3,0x3,0x3,0x24,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x3,0x3,0x3,0x3,0x3,0x3,0x1,0x1,0x1,0xf,0xf,0xf,0x0,0x0,0x0,0x3,0x3,0x3,0x10,0x10,0x10,0x1,0x1,0x1,0x0,0x0,0x0},
	{TMDS_0p25G,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x4c,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x3,0x3,0x3,0x3,0x3,0x3,0x0,0x0,0x0,0x7,0x7,0x7,0x6,0x6,0x6,0x4,0x4,0x4,0x4c,0x4c,0x4c,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_4p445G,0x0,0x0,0x0,0x2,0x2,0x2,0x2,0x24,0x0,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x1,0x1,0x1,0x1,0x1,0x1,0xb,0xb,0xb,0xb,0xb,0xb,0x0,0x0,0x0,0x3,0x3,0x3,0x1,0x1,0x1,0xd,0xd,0xd,0xf,0xf,0xf,
0xf,0xf,0xf,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x2,0x2,0x2,0x1,0x1,0x1,0x0,0x0,0x0,0xc,0xc,0xc,0x3,0x3,0x3,0x0,0x0,0x0,0x10,0x10,0x10,0x0,0x0,0x0,0x2,0x2,0x2},
	{TMDS_3p7125G,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x24,0x0,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x1,0x1,0x1,0x1,0x1,0x1,0xb,0xb,0xb,0xb,0xb,0xb,0x0,0x0,0x0,0x3,0x3,0x3,0x1,0x1,0x1,0xd,0xd,0xd,0xf,0xf,0xf,
0xf,0xf,0xf,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x2,0x2,0x2,0x1,0x1,0x1,0x0,0x0,0x0,0x7,0x7,0x7,0x6,0x6,0x6,0x0,0x0,0x0,0x10,0x10,0x10,0x0,0x0,0x0,0x2,0x2,0x2},
	{TMDS_2p2275G,0x0,0x0,0x0,0x2,0x2,0x2,0x2,0x24,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x3,0x3,0x3,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x2,0x2,0x2,0x0,0x0,0x0,0xc,0xc,0xc,0x3,0x3,0x3,0x1,0x1,0x1,0x6,0x6,0x6,0x0,0x0,0x0,0x1,0x1,0x1},
	{TMDS_1p85625G,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x24,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x3,0x3,0x3,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x2,0x2,0x2,0x0,0x0,0x0,0x7,0x7,0x7,0x6,0x6,0x6,0x1,0x1,0x1,0x6,0x6,0x6,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_1p11375G,0x0,0x0,0x0,0x2,0x2,0x2,0x2,0x24,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x3,0x3,0x3,0x3,0x3,0x3,0x0,0x0,0x0,0xc,0xc,0xc,0x3,0x3,0x3,0x2,0x2,0x2,0x6,0x6,0x6,0x1,0x1,0x1,0x0,0x0,0x0},
	{TMDS_0p928125G,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x24,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x3,0x3,0x3,0x3,0x3,0x3,0x0,0x0,0x0,0x7,0x7,0x7,0x6,0x6,0x6,0x2,0x2,0x2,0x6,0x6,0x6,0x1,0x1,0x1,0x0,0x0,0x0},
	{TMDS_0p54G,0x1,0x1,0x1,0x2,0x2,0x2,0x2,0x24,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x3,0x3,0x3,0x3,0x3,0x3,0x0,0x0,0x0,0xc,0xc,0xc,0x3,0x3,0x3,0x3,0x3,0x3,0x10,0x10,0x10,0x1,0x1,0x1,0x0,0x0,0x0},
	{TMDS_0p405G,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x24,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x3,0x3,0x3,0x3,0x3,0x3,0x0,0x0,0x0,0x7,0x7,0x7,0x6,0x6,0x6,0x3,0x3,0x3,0x10,0x10,0x10,0x1,0x1,0x1,0x0,0x0,0x0},
	{TMDS_0p3375G,0x1,0x1,0x1,0x3,0x3,0x3,0x3,0x24,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x3,0x3,0x3,0x3,0x3,0x3,0x1,0x1,0x1,0xf,0xf,0xf,0x0,0x0,0x0,0x4,0x4,0x4,0x4c,0x4c,0x4c,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p27G,0x1,0x1,0x1,0x2,0x2,0x2,0x2,0x24,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x1,0x1,0x1,0x3,0x3,0x3,0x3,0x3,0x3,0x0,0x0,0x0,0xc,0xc,0xc,0x3,0x3,0x3,0x4,0x4,0x4,0x4c,0x4c,0x4c,0x0,0x0,0x0,0x0,0x0,0x0},
};

DFE_ini_tmds_1_T DFE_ini_tmds_1[] =
{
	{TMDS_5p94G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
	{TMDS_2p97G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
	{TMDS_1p485G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
	{TMDS_0p742G,0x5,0x10,0x5,0x10,0x5,0x10,0xf,0x10},
	{TMDS_0p25G,0x0,0x19,0x0,0x19,0x0,0x19,0x0,0x19},
	{TMDS_4p445G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
	{TMDS_3p7125G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
	{TMDS_2p2275G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
	{TMDS_1p85625G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
	{TMDS_1p11375G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
	{TMDS_0p928125G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
	{TMDS_0p54G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
	{TMDS_0p405G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
	{TMDS_0p3375G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
	{TMDS_0p27G,0xf,0x10,0xf,0x10,0xf,0x10,0xf,0x10},
};

DPHY_Para_tmds_1_T DPHY_Para_tmds_1[] =
{
	{TMDS_5p94G,0x3,0x3,0x3,0x3,0x8,0x8,0x8,0x8,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x2,0x98,0xf,0x2,0x98,0xf,0x2,
0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
	{TMDS_2p97G,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x2,0x2,0x2,0x1,0x1,0x1,0x2,0x98,0x7,0x2,0x98,0x7,0x2,
0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
	{TMDS_1p485G,0x8,0x8,0x8,0x8,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x3,0x3,0x3,0x1,0x1,0x1,0x2,0x98,0xf,0x2,0x98,0xf,0x2,
0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
	{TMDS_0p742G,0x8,0x8,0x8,0x8,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x3,0x3,0x3,0x1,0x1,0x1,0x6,0x98,0xd,0x6,0x98,0xd,0x6,
0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd},
	{TMDS_0p25G,0x8,0x8,0x8,0x8,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x1,0x1,0x1,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,
0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x0,0x0,0x0},
	{TMDS_4p445G,0x3,0x3,0x3,0x3,0x8,0x8,0x8,0x8,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x2,0x98,0xf,0x2,0x98,0xf,0x2,
0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
	{TMDS_3p7125G,0x4,0x4,0x4,0x4,0x8,0x8,0x8,0x8,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0x0,0x0,0x2,0x98,0xf,0x2,0x98,0xf,0x2,
0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
	{TMDS_2p2275G,0x8,0x8,0x8,0x8,0x4,0x4,0x4,0x4,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x2,0x2,0x2,0x1,0x1,0x1,0x2,0x98,0x7,0x2,0x98,0x7,0x2,
0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
	{TMDS_1p85625G,0x8,0x8,0x8,0x8,0x4,0x4,0x4,0x4,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x2,0x2,0x2,0x1,0x1,0x1,0x2,0x98,0x7,0x2,0x98,0x7,0x2,
0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0x7,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
	{TMDS_1p11375G,0x8,0x8,0x8,0x8,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x1,0x1,0x1,0x2,0x98,0xf,0x2,0x98,0xf,0x2,
0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
	{TMDS_0p928125G,0x8,0x8,0x8,0x8,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x1,0x1,0x1,0x2,0x98,0xf,0x2,0x98,0xf,0x2,
0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
	{TMDS_0p54G,0x8,0x8,0x8,0x8,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x1,0x1,0x1,0x6,0x98,0xd,0x6,0x98,0xd,0x6,
0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
	{TMDS_0p405G,0x8,0x8,0x8,0x8,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x1,0x1,0x1,0x6,0x98,0xd,0x6,0x98,0xd,0x6,
0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x6,0x98,0xd,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
	{TMDS_0p3375G,0x8,0x8,0x8,0x8,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x1,0x1,0x1,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,
0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
	{TMDS_0p27G,0x8,0x8,0x8,0x8,0x4,0x4,0x4,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3,0x1,0x1,0x1,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,
0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x6,0xa0,0xf,0x2,0x98,0xf,0x2,0x98,0xf,0x2,0x98,0xf},
};

Koffset_tmds_1_T Koffset_tmds_1[] =
{
	{TMDS_5p94G,0x1,0x1,0x1,0x1,0x1,0x1,0xf,0xf,0xf},
	{TMDS_2p97G,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3},
	{TMDS_1p485G,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1},
	{TMDS_0p742G,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1},
	{TMDS_0p25G,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1},
	{TMDS_4p445G,0x1,0x1,0x1,0x1,0x1,0x1,0xf,0xf,0xf},
	{TMDS_3p7125G,0x1,0x1,0x1,0x1,0x1,0x1,0xf,0xf,0xf},
	{TMDS_2p2275G,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3},
	{TMDS_1p85625G,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x3,0x3},
	{TMDS_1p11375G,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1},
	{TMDS_0p928125G,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1},
	{TMDS_0p54G,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1},
	{TMDS_0p405G,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1},
	{TMDS_0p3375G,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1},
	{TMDS_0p27G,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1},
};

LEQ_VTH_Tap0_3_4_Adapt_tmds_1_T LEQ_VTH_Tap0_3_4_Adapt_tmds_1[] =
{
	{TMDS_5p94G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{TMDS_2p97G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_1p485G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p742G,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p25G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{TMDS_4p445G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{TMDS_3p7125G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{TMDS_2p2275G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_1p85625G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_1p11375G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p928125G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p54G,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p405G,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p3375G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{TMDS_0p27G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
};

Tap0_to_Tap4_Adapt_tmds_1_T Tap0_to_Tap4_Adapt_tmds_1[] =
{
	{TMDS_5p94G,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{TMDS_2p97G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_1p485G,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p742G,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p25G,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{TMDS_4p445G,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{TMDS_3p7125G,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{TMDS_2p2275G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_1p85625G,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_1p11375G,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p928125G,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p54G,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p405G,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
	{TMDS_0p3375G,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{TMDS_0p27G,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
};

void TMDS_Main_Seq(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	HDMI_EMG("Version:HDMI_TMDS_setting_20230905\n");
	if(tmds_timing==TMDS_5p94G || tmds_timing==TMDS_2p97G || tmds_timing==TMDS_1p485G || tmds_timing==TMDS_0p742G || tmds_timing==TMDS_4p445G || tmds_timing==TMDS_3p7125G || tmds_timing==TMDS_2p2275G || tmds_timing==TMDS_1p85625G || tmds_timing==TMDS_1p11375G || tmds_timing==TMDS_0p928125G || tmds_timing==TMDS_0p54G || tmds_timing==TMDS_0p405G )
	{
		DCDR_settings_tmds_4_func(nport, tmds_timing);
		DFE_ini_tmds_1_func(nport, tmds_timing);
		DPHY_Fix_tmds_1_func(nport, tmds_timing);
		DPHY_Para_tmds_1_func(nport, tmds_timing);
		APHY_Fix_tmds_1_func(nport, tmds_timing);
		APHY_Para_tmds_1_func(nport, tmds_timing);
		DPHY_Init_Flow_tmds_1_func(nport, tmds_timing);
		APHY_Init_Flow_tmds_1_func(nport, tmds_timing);
		DCDR_settings_tmds_1_func(nport, tmds_timing);
		DCDR_settings_tmds_2_func(nport, tmds_timing);
		Koffset_tmds_1_func(nport, tmds_timing);
		LEQ_VTH_Tap0_3_4_Adapt_tmds_1_func(nport, tmds_timing);
		Koffset_tmds_1_func(nport, tmds_timing);
		Tap0_to_Tap4_Adapt_tmds_1_func(nport, tmds_timing);
		Koffset_tmds_1_func(nport, tmds_timing);
		DCDR_settings_tmds_3_func(nport, tmds_timing);
	}
	else if(tmds_timing==TMDS_0p25G || tmds_timing==TMDS_0p3375G || tmds_timing==TMDS_0p27G )
	{
		DCDR_settings_tmds_4_func(nport, tmds_timing);
		DFE_ini_tmds_1_func(nport, tmds_timing);
		DPHY_Fix_tmds_1_func(nport, tmds_timing);
		DPHY_Para_tmds_1_func(nport, tmds_timing);
		APHY_Fix_tmds_1_func(nport, tmds_timing);
		APHY_Para_tmds_1_func(nport, tmds_timing);
		DPHY_Init_Flow_tmds_1_func(nport, tmds_timing);
		APHY_Init_Flow_tmds_1_func(nport, tmds_timing);
		DCDR_settings_tmds_1_func(nport, tmds_timing);
		DCDR_settings_tmds_2_func(nport, tmds_timing);
		Koffset_tmds_1_func(nport, tmds_timing);
		Manual_DFE_tmds_1_func(nport, tmds_timing);
		Koffset_tmds_1_func(nport, tmds_timing);
		DCDR_settings_tmds_3_func(nport, tmds_timing);
	}
}

void DCDR_settings_tmds_4_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	//Init reset
	_phy_rtd_part_outl(0x1800da40, 17, 17, 0); //PI_DIV_RSTB
	_phy_rtd_part_outl(0x1800da60, 17, 17, 0); //PI_DIV_RSTB
	_phy_rtd_part_outl(0x1800da80, 17, 17, 0); //PI_DIV_RSTB
	_phy_rtd_part_outl(0x1800daa0, 17, 17, 0); //PI_DIV_RSTB
}

void DFE_ini_tmds_1_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	_phy_rtd_outl(0x1800dc70, 0x00000000);
	//R lane
	_phy_rtd_outl(0x1800de00, 0x00000000);
	_phy_rtd_outl(0x1800de04, 0x00000000);
	_phy_rtd_outl(0x1800de08, 0x20C00000);
	_phy_rtd_outl(0x1800de0c, 0x05b45c00);
	_phy_rtd_outl(0x1800de10, 0xc0004000);
	_phy_rtd_outl(0x1800de14, 0x00280000);
	_phy_rtd_outl(0x1800de18, 0x1038ffe0);
	_phy_rtd_outl(0x1800de1c, 0x7FF07FE0);
	_phy_rtd_outl(0x1800de20, 0x3FFF83E0);
	_phy_rtd_outl(0x1800de24, 0x00000000);
	_phy_rtd_outl(0x1800de28, 0x009D00E7);
	_phy_rtd_outl(0x1800de2c, 0xFF00FFC0);
	_phy_rtd_outl(0x1800de30, 0xff005400);
	//G lane
	_phy_rtd_outl(0x1800de40, 0x00000000);
	_phy_rtd_outl(0x1800de44, 0x00000000);
	_phy_rtd_outl(0x1800de48, 0x20C00000);
	_phy_rtd_outl(0x1800de4c, 0x05b45c00);
	_phy_rtd_outl(0x1800de50, 0xc0004000);
	_phy_rtd_outl(0x1800de54, 0x00280000);
	_phy_rtd_outl(0x1800de58, 0x1038ffe0);
	_phy_rtd_outl(0x1800de5c, 0x7FF07FE0);
	_phy_rtd_outl(0x1800de60, 0x3FFF83E0);
	_phy_rtd_outl(0x1800de64, 0x00000000);
	_phy_rtd_outl(0x1800de68, 0x009D00E7);
	_phy_rtd_outl(0x1800de6c, 0xFF00FFC0);
	_phy_rtd_outl(0x1800de70, 0xff005400);
	//B lane
	_phy_rtd_outl(0x1800de80, 0x00000000);
	_phy_rtd_outl(0x1800de84, 0x00000000);
	_phy_rtd_outl(0x1800de88, 0x20C00000);
	_phy_rtd_outl(0x1800de8c, 0x05b45c00);
	_phy_rtd_outl(0x1800de90, 0xc0004000);
	_phy_rtd_outl(0x1800de94, 0x00280000);
	_phy_rtd_outl(0x1800de98, 0x1038ffe0);
	_phy_rtd_outl(0x1800de9c, 0x7FF07FE0);
	_phy_rtd_outl(0x1800dea0, 0x3FFF83E0);
	_phy_rtd_outl(0x1800dea4, 0x00000000);
	_phy_rtd_outl(0x1800dea8, 0x009D00E7);
	_phy_rtd_outl(0x1800deac, 0xFF00FFC0);
	_phy_rtd_outl(0x1800deb0, 0xff005400);
	//CK lane
	_phy_rtd_outl(0x1800dec0, 0x00000000);
	_phy_rtd_outl(0x1800dec4, 0x00000000);
	_phy_rtd_outl(0x1800dec8, 0x20C00000);
	_phy_rtd_outl(0x1800decc, 0x05b45c00);
	_phy_rtd_outl(0x1800ded0, 0xc0004000);
	_phy_rtd_outl(0x1800ded4, 0x00280000);
	_phy_rtd_outl(0x1800ded8, 0x1038ffe0);
	_phy_rtd_outl(0x1800dedc, 0x7FF07FE0);
	_phy_rtd_outl(0x1800dee0, 0x3e0383e0);
	_phy_rtd_outl(0x1800dee4, 0x00000000);
	_phy_rtd_outl(0x1800dee8, 0x009D00E7);
	_phy_rtd_outl(0x1800deec, 0xFF00FFC0);
	_phy_rtd_outl(0x1800def0, 0xff005400);
	// *************************************************************************************************************************************
	//B-Lane_initial DFE Value_Start
	_phy_rtd_part_outl(0x1800dea8, 9, 5, 14); //VTHP_INIT_B[4:0]	9:5	R/W	0x0F		VTHP INIT
	_phy_rtd_part_outl(0x1800dea8, 4, 0, 14); //VTHN_INIT_B[4:0]	4:0	R/W	0x0F		VTHN INIT
	_phy_rtd_part_outl(0x1800dea0, 4, 0, DFE_ini_tmds_1[tmds_timing].LEQ_INIT_B_4_0_62_1); //LEQ_INIT_B[4:0]	4:0		R/W	0x00	LEQ INIT
	_phy_rtd_part_outl(0x1800dea4, 31, 24, DFE_ini_tmds_1[tmds_timing].TAP0_INIT_B_7_0_63_1); //TAP0_INIT_B[7:0]	31:24	R/W	0x00	TAP0 INIT
	_phy_rtd_part_outl(0x1800dea4, 23, 18, 0); //TAP1_INIT_B[5:0]	23:18	R/W	0x00	TAP1 INIT
	_phy_rtd_part_outl(0x1800dea4, 17, 12, 0); //TAP2_INIT_B[5:0]	17:12	R/W	0x00	TAP2 INIT
	_phy_rtd_part_outl(0x1800dea4, 11, 6, 0); //TAP3_INIT_B[5:0]	11:6	R/W	0x00	TAP3 INIT
	_phy_rtd_part_outl(0x1800dea4, 5, 0, 0); //TAP4_INIT_B[5:0]	5:0		R/W	0x00	TAP4 INIT
	//B-Lane DFE COEF LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 0, 0, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP0 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 1, 1, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP1 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 2, 2, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP2 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 3, 3, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP3 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 4, 4, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP4 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 5, 5, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de88, 31, 31, 1); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 4, 0, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 5, 5, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de88, 31, 31, 0x0); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	//B-Lane_initial DFE Value_Stop
	// *************************************************************************************************************************************
	//G-Lane_initial DFE Value_Start
	_phy_rtd_part_outl(0x1800de68, 9, 5, 14); //VTHP_INIT_B[4:0]	9:5	R/W	0x0F		VTHP INIT
	_phy_rtd_part_outl(0x1800de68, 4, 0, 14); //VTHN_INIT_B[4:0]	4:0	R/W	0x0F		VTHN INIT
	_phy_rtd_part_outl(0x1800de60, 4, 0, DFE_ini_tmds_1[tmds_timing].LEQ_INIT_G_4_0_84_1); //LEQ_INIT_B[4:0]	4:0		R/W	0x00	LEQ INIT
	_phy_rtd_part_outl(0x1800de64, 31, 24, DFE_ini_tmds_1[tmds_timing].TAP0_INIT_G_7_0_85_1); //TAP0_INIT_B[7:0]	31:24	R/W	0x00	TAP0 INIT
	_phy_rtd_part_outl(0x1800de64, 23, 18, 0); //TAP1_INIT_B[5:0]	23:18	R/W	0x00	TAP1 INIT
	_phy_rtd_part_outl(0x1800de64, 17, 12, 0); //TAP2_INIT_B[5:0]	17:12	R/W	0x00	TAP2 INIT
	_phy_rtd_part_outl(0x1800de64, 11, 6, 0); //TAP3_INIT_B[5:0]	11:6	R/W	0x00	TAP3 INIT
	_phy_rtd_part_outl(0x1800de64, 5, 0, 0); //TAP4_INIT_B[5:0]	5:0		R/W	0x00	TAP4 INIT
	//G-Lane DFE COEF LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 0, 0, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP0 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 1, 1, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP1 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 2, 2, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP2 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 3, 3, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP3 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 4, 4, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP4 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 5, 5, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de48, 31, 31, 1); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 4, 0, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 5, 5, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de48, 31, 31, 0x0); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	//G-Lane_initial DFE Value_Stop
	// *************************************************************************************************************************************
	//R-Lane_initial DFE Value_Start
	_phy_rtd_part_outl(0x1800de28, 9, 5, 14); //VTHP_INIT_B[4:0]	9:5	R/W	0x0F		VTHP INIT
	_phy_rtd_part_outl(0x1800de28, 4, 0, 14); //VTHN_INIT_B[4:0]	4:0	R/W	0x0F		VTHN INIT
	_phy_rtd_part_outl(0x1800de20, 4, 0, DFE_ini_tmds_1[tmds_timing].LEQ_INIT_R_4_0_106_1); //LEQ_INIT_B[4:0]	4:0		R/W	0x00	LEQ INIT
	_phy_rtd_part_outl(0x1800de24, 31, 24, DFE_ini_tmds_1[tmds_timing].TAP0_INIT_R_7_0_107_1); //TAP0_INIT_B[7:0]	31:24	R/W	0x00	TAP0 INIT
	_phy_rtd_part_outl(0x1800de24, 23, 18, 0); //TAP1_INIT_B[5:0]	23:18	R/W	0x00	TAP1 INIT
	_phy_rtd_part_outl(0x1800de24, 17, 12, 0); //TAP2_INIT_B[5:0]	17:12	R/W	0x00	TAP2 INIT
	_phy_rtd_part_outl(0x1800de24, 11, 6, 0); //TAP3_INIT_B[5:0]	11:6	R/W	0x00	TAP3 INIT
	_phy_rtd_part_outl(0x1800de24, 5, 0, 0); //TAP4_INIT_B[5:0]	5:0		R/W	0x00	TAP4 INIT
	//R-Lane DFE COEF LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 0, 0, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP0 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 1, 1, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP1 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 2, 2, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP2 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 3, 3, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP3 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 4, 4, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP4 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 5, 5, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de08, 31, 31, 1); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 4, 0, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 5, 5, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de08, 31, 31, 0x0); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	//R-Lane_initial DFE Value_Stop
	// *************************************************************************************************************************************
	//CK-Lane_initial DFE Value_Start
	_phy_rtd_part_outl(0x1800dee8, 9, 5, 14); //VTHP_INIT_B[4:0]	9:5	R/W	0x0F		VTHP INIT
	_phy_rtd_part_outl(0x1800dee8, 4, 0, 14); //VTHN_INIT_B`[4:0]	4:0	R/W	0x0F		VTHN INIT
	_phy_rtd_part_outl(0x1800dee0, 4, 0, DFE_ini_tmds_1[tmds_timing].LEQ_INIT_CK_4_0_128_1); //LEQ_INIT_B[4:0]	4:0		R/W	0x00	LEQ INIT
	_phy_rtd_part_outl(0x1800dee4, 31, 24, DFE_ini_tmds_1[tmds_timing].TAP0_INIT_CK_7_0_129_1); //TAP0_INIT_B[7:0]	31:24	R/W	0x00	TAP0 INIT
	_phy_rtd_part_outl(0x1800dee4, 23, 18, 0); //TAP1_INIT_B[5:0]	23:18	R/W	0x00	TAP1 INIT
	_phy_rtd_part_outl(0x1800dee4, 17, 12, 0); //TAP2_INIT_B[5:0]	17:12	R/W	0x00	TAP2 INIT
	_phy_rtd_part_outl(0x1800dee4, 11, 6, 0); //TAP3_INIT_B[5:0]	11:6	R/W	0x00	TAP3 INIT
	_phy_rtd_part_outl(0x1800dee4, 5, 0, 0); //TAP4_INIT_B[5:0]	5:0		R/W	0x00	TAP4 INIT
	//CK-Lane DFE COEF LOAD IN
	_phy_rtd_part_outl(0x1800decc, 0, 0, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP0 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 1, 1, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP1 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 2, 2, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP2 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 3, 3, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP3 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 4, 4, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP4 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 5, 5, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800dec8, 31, 31, 1); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800decc, 4, 0, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP LOAD IN
	_phy_rtd_part_outl(0x1800decc, 5, 5, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800dec8, 31, 31, 0x0); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	//CK-Lane_initial DFE Value_Stop
	//**************************************************************************************************************************************
	_phy_rtd_part_outl(0x1800def4, 17, 15, 6); //rotation_mode Adaption flow control mode
	_phy_rtd_part_outl(0x1800deb4, 17, 15, 6); //rotation_mode Adaption flow control mode
	_phy_rtd_part_outl(0x1800de74, 17, 15, 6); //rotation_mode Adaption flow control mode
	_phy_rtd_part_outl(0x1800de34, 17, 15, 6); //rotation_mode Adaption flow control mode
}

void DPHY_Fix_tmds_1_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	//DPHY fix start
	_phy_rtd_part_outl(0x1800db00, 0, 0, 0x1); //[DPHY fix]  reg_p0_b_dig_rst_n
	_phy_rtd_part_outl(0x1800db00, 1, 1, 0x1); //[DPHY fix]  reg_p0_g_dig_rst_n
	_phy_rtd_part_outl(0x1800db00, 2, 2, 0x1); //[DPHY fix]  reg_p0_r_dig_rst_n
	_phy_rtd_part_outl(0x1800db00, 3, 3, 0x1); //[DPHY fix]  reg_p0_ck_dig_rst_n
	_phy_rtd_part_outl(0x1800db00, 4, 4, 0x1); //[DPHY fix]  reg_p0_b_cdr_rst_n
	_phy_rtd_part_outl(0x1800db00, 5, 5, 0x1); //[DPHY fix]  reg_p0_g_cdr_rst_n
	_phy_rtd_part_outl(0x1800db00, 6, 6, 0x1); //[DPHY fix]  reg_p0_r_cdr_rst_n
	_phy_rtd_part_outl(0x1800db00, 7, 7, 0x1); //[DPHY fix]  reg_p0_ck_cdr_rst_n
	_phy_rtd_part_outl(0x1800db00, 8, 8, 0x0); //[DPHY fix]  reg_p0_b_clk_inv
	_phy_rtd_part_outl(0x1800db00, 9, 9, 0x0); //[DPHY fix]  reg_p0_g_clk_inv
	_phy_rtd_part_outl(0x1800db00, 10, 10, 0x0); //[DPHY fix]  reg_p0_r_clk_inv
	_phy_rtd_part_outl(0x1800db00, 11, 11, 0x0); //[DPHY fix]  reg_p0_ck_clk_inv
	_phy_rtd_part_outl(0x1800db00, 12, 12, 0x0); //[DPHY fix]  reg_p0_b_shift_inv
	_phy_rtd_part_outl(0x1800db00, 13, 13, 0x0); //[DPHY fix]  reg_p0_g_shift_inv
	_phy_rtd_part_outl(0x1800db00, 14, 14, 0x0); //[DPHY fix]  reg_p0_r_shift_inv
	_phy_rtd_part_outl(0x1800db00, 15, 15, 0x0); //[DPHY fix]  reg_p0_ck_shift_inv
	_phy_rtd_part_outl(0x1800db00, 24, 24, 0x0); //[DPHY fix]  reg_p0_b_data_order
	_phy_rtd_part_outl(0x1800db00, 25, 25, 0x0); //[DPHY fix]  reg_p0_g_data_order
	_phy_rtd_part_outl(0x1800db00, 26, 26, 0x0); //[DPHY fix]  reg_p0_r_data_order
	_phy_rtd_part_outl(0x1800db00, 27, 27, 0x0); //[DPHY fix]  reg_p0_ck_data_order
	_phy_rtd_part_outl(0x1800db00, 28, 28, 0x0); //[DPHY fix]  reg_p0_b_dyn_kp_en
	_phy_rtd_part_outl(0x1800db00, 29, 29, 0x0); //[DPHY fix]  reg_p0_g_dyn_kp_en
	_phy_rtd_part_outl(0x1800db00, 30, 30, 0x0); //[DPHY fix]  reg_p0_r_dyn_kp_en
	_phy_rtd_part_outl(0x1800db00, 31, 31, 0x0); //[DPHY fix]  reg_p0_ck_dyn_kp_en
	_phy_rtd_part_outl(0x1800db04, 24, 24, 0x0); //[]  reg_p0_b_dfe_edge_out
	_phy_rtd_part_outl(0x1800db04, 25, 25, 0x0); //[]  reg_p0_b_dfe_edge_out
	_phy_rtd_part_outl(0x1800db04, 26, 26, 0x0); //[]  reg_p0_r_dfe_edge_out
	_phy_rtd_part_outl(0x1800db04, 27, 27, 0x0); //[]  reg_p0_ck_dfe_edge_out
	_phy_rtd_part_outl(0x1800db10, 0, 0, 0x0); //[]  reg_p0_b_bypass_sdm_int
	_phy_rtd_part_outl(0x1800db10, 11, 1, 0x000); //[]  reg_p0_b_int_init
	_phy_rtd_part_outl(0x1800db10, 21, 12, 0x000); //[]  reg_p0_b_acc2_period
	_phy_rtd_part_outl(0x1800db10, 22, 22, 0x0); //[]  reg_p0_b_acc2_manual
	_phy_rtd_part_outl(0x1800db10, 23, 23, 0x0); //[]  reg_p0_b_squ_tri
	_phy_rtd_part_outl(0x1800db14, 0, 0, 0x0); //[]  reg_p0_g_bypass_sdm_int
	_phy_rtd_part_outl(0x1800db14, 11, 1, 0x000); //[]  reg_p0_g_int_init
	_phy_rtd_part_outl(0x1800db14, 21, 12, 0x000); //[]  reg_p0_g_acc2_period
	_phy_rtd_part_outl(0x1800db14, 22, 22, 0x0); //[]  reg_p0_g_acc2_manual
	_phy_rtd_part_outl(0x1800db14, 23, 23, 0x0); //[]  reg_p0_g_squ_tri
	_phy_rtd_part_outl(0x1800db18, 0, 0, 0x0); //[]  reg_p0_r_bypass_sdm_int
	_phy_rtd_part_outl(0x1800db18, 11, 1, 0x000); //[]  reg_p0_r_int_init
	_phy_rtd_part_outl(0x1800db18, 21, 12, 0x000); //[]  reg_p0_r_acc2_period
	_phy_rtd_part_outl(0x1800db18, 22, 22, 0x0); //[]  reg_p0_r_acc2_manual
	_phy_rtd_part_outl(0x1800db18, 23, 23, 0x0); //[]  reg_p0_r_squ_tri
	_phy_rtd_part_outl(0x1800db1c, 0, 0, 0x0); //[]  reg_p0_ck_bypass_sdm_int
	_phy_rtd_part_outl(0x1800db1c, 11, 1, 0x000); //[]  reg_p0_ck_int_init
	_phy_rtd_part_outl(0x1800db1c, 21, 12, 0x000); //[]  reg_p0_ck_acc2_period
	_phy_rtd_part_outl(0x1800db1c, 22, 22, 0x0); //[]  reg_p0_ck_acc2_manual
	_phy_rtd_part_outl(0x1800db1c, 23, 23, 0x0); //[]  reg_p0_ck_squ_tri
	_phy_rtd_part_outl(0x1800db20, 0, 0, 0x0); //[]  reg_p0_b_pi_m_mode
	_phy_rtd_part_outl(0x1800db20, 3, 1, 0x0); //[]  reg_p0_b_testout_sel
	_phy_rtd_part_outl(0x1800db20, 8, 4, 0x00); //[]  reg_p0_b_timer_lpf
	_phy_rtd_part_outl(0x1800db20, 13, 9, 0x00); //[]  reg_p0_b_timer_eq
	_phy_rtd_part_outl(0x1800db20, 18, 14, 0x00); //[]  reg_p0_b_timer_ber
	_phy_rtd_part_outl(0x1800db24, 0, 0, 0x0); //[]  reg_p0_g_pi_m_mode
	_phy_rtd_part_outl(0x1800db24, 3, 1, 0x0); //[]  reg_p0_g_testout_sel
	_phy_rtd_part_outl(0x1800db24, 8, 4, 0x00); //[]  reg_p0_g_timer_lpf
	_phy_rtd_part_outl(0x1800db24, 13, 9, 0x00); //[]  reg_p0_g_timer_eq
	_phy_rtd_part_outl(0x1800db24, 18, 14, 0x00); //[]  reg_p0_g_timer_ber
	_phy_rtd_part_outl(0x1800db28, 0, 0, 0x0); //[]  reg_p0_r_pi_m_mode
	_phy_rtd_part_outl(0x1800db28, 3, 1, 0x0); //[]  reg_p0_r_testout_sel
	_phy_rtd_part_outl(0x1800db28, 8, 4, 0x00); //[]  reg_p0_r_timer_lpf
	_phy_rtd_part_outl(0x1800db28, 13, 9, 0x00); //[]  reg_p0_r_timer_eq
	_phy_rtd_part_outl(0x1800db28, 18, 14, 0x00); //[]  reg_p0_r_timer_ber
	_phy_rtd_part_outl(0x1800db2c, 0, 0, 0x0); //[]  reg_p0_ck_pi_m_mode
	_phy_rtd_part_outl(0x1800db2c, 3, 1, 0x0); //[]  reg_p0_ck_testout_sel
	_phy_rtd_part_outl(0x1800db2c, 8, 4, 0x00); //[]  reg_p0_ck_timer_lpf
	_phy_rtd_part_outl(0x1800db2c, 13, 9, 0x00); //[]  reg_p0_ck_timer_eq
	_phy_rtd_part_outl(0x1800db2c, 18, 14, 0x00); //[]  reg_p0_ck_timer_ber
	_phy_rtd_outl(0x1800db30, 0x000000ff); //[]  reg_p0_b_st_m_value
	_phy_rtd_outl(0x1800db34, 0x000000ff); //[]  reg_p0_g_st_m_value
	_phy_rtd_outl(0x1800db38, 0x000000ff); //[]  reg_p0_r_st_m_value
	_phy_rtd_outl(0x1800db3c, 0x000000ff); //[]  reg_p0_ck_st_m_value
	//Clock Ready
	_phy_rtd_part_outl(0x1800db40, 0, 0, 0x0); //[]  reg_p0_b_force_clkrdy
	_phy_rtd_part_outl(0x1800db40, 1, 1, 0x0); //[]  reg_p0_g_force_clkrdy
	_phy_rtd_part_outl(0x1800db40, 2, 2, 0x0); //[]  reg_p0_r_force_clkrdy
	_phy_rtd_part_outl(0x1800db40, 3, 3, 0x0); //[]  reg_p0_ck_force_clkrdy
	_phy_rtd_part_outl(0x1800db40, 7, 4, 0x2); //[]  reg_p0_b_timer_clk
	_phy_rtd_part_outl(0x1800db40, 11, 8, 0x2); //[]  reg_p0_g_timer_clk
	_phy_rtd_part_outl(0x1800db40, 15, 12, 0x2); //[]  reg_p0_r_timer_clk
	_phy_rtd_part_outl(0x1800db40, 19, 16, 0x2); //[]  reg_p0_ck_timer_clk
	_phy_rtd_part_outl(0x1800db44, 11, 8, 0xf); //[DPHY fix]  reg_p0_b_offset_delay_cnt
	_phy_rtd_part_outl(0x1800db44, 13, 12, 0x0); //[DPHY fix]  reg_p0_b_offset_stable_cnt
	_phy_rtd_part_outl(0x1800db44, 19, 14, 0x0b); //[DPHY fix]  reg_p0_b_offset_divisor
	_phy_rtd_part_outl(0x1800db44, 26, 20, 0x28); //[DPHY fix]  reg_p0_b_offset_timeout
	_phy_rtd_part_outl(0x1800db48, 0, 0, 0x0); //[]  reg_p0_b_offset_en_ope
	_phy_rtd_part_outl(0x1800db48, 1, 1, 0x0); //[]  reg_p0_b_offset_pc_ope
	_phy_rtd_part_outl(0x1800db48, 6, 2, 0x00); //[]  reg_p0_b_offset_ini_ope
	_phy_rtd_part_outl(0x1800db48, 7, 7, 0x0); //[]  reg_p0_b_offset_gray_en_ope
	_phy_rtd_part_outl(0x1800db48, 8, 8, 0x0); //[]  reg_p0_b_offset_manual_ope
	_phy_rtd_part_outl(0x1800db48, 9, 9, 0x0); //[]  reg_p0_b_offset_z0_ok_ope
	_phy_rtd_part_outl(0x1800db48, 10, 10, 0x0); //[]  reg_p0_b_offset_en_opo
	_phy_rtd_part_outl(0x1800db48, 11, 11, 0x0); //[]  reg_p0_b_offset_pc_opo
	_phy_rtd_part_outl(0x1800db48, 16, 12, 0x00); //[]  reg_p0_b_offset_ini_opo
	_phy_rtd_part_outl(0x1800db48, 17, 17, 0x0); //[]  reg_p0_b_offset_gray_en_opo
	_phy_rtd_part_outl(0x1800db48, 18, 18, 0x0); //[]  reg_p0_b_offset_manual_opo
	_phy_rtd_part_outl(0x1800db48, 19, 19, 0x0); //[]  reg_p0_b_offset_z0_ok_opo
	_phy_rtd_part_outl(0x1800db48, 20, 20, 0x0); //[]  reg_p0_b_offset_en_one
	_phy_rtd_part_outl(0x1800db48, 21, 21, 0x0); //[]  reg_p0_b_offset_pc_one
	_phy_rtd_part_outl(0x1800db48, 26, 22, 0x00); //[]  reg_p0_b_offset_ini_one
	_phy_rtd_part_outl(0x1800db48, 27, 27, 0x0); //[]  reg_p0_b_offset_gray_en_one
	_phy_rtd_part_outl(0x1800db48, 28, 28, 0x0); //[]  reg_p0_b_offset_manual_one
	_phy_rtd_part_outl(0x1800db48, 29, 29, 0x0); //[]  reg_p0_b_offset_z0_ok_one
	_phy_rtd_part_outl(0x1800db4c, 0, 0, 0x0); //[DPHY fix]  reg_p0_b_offset_en_ono
	_phy_rtd_part_outl(0x1800db4c, 1, 1, 0x0); //[DPHY fix]  reg_p0_b_offset_pc_ono
	_phy_rtd_part_outl(0x1800db4c, 6, 2, 0x00); //[DPHY fix]  reg_p0_b_offset_ini_ono
	_phy_rtd_part_outl(0x1800db4c, 7, 7, 0x0); //[DPHY fix]  reg_p0_b_offset_gray_en_ono
	_phy_rtd_part_outl(0x1800db4c, 8, 8, 0x0); //[DPHY fix]  reg_p0_b_offset_manual_ono
	_phy_rtd_part_outl(0x1800db4c, 9, 9, 0x0); //[DPHY fix]  reg_p0_b_offset_z0_ok_ono
	_phy_rtd_part_outl(0x1800db4c, 16, 12, 0x00); //[DPHY fix]  reg_p0_b_offset_ini_de
	_phy_rtd_part_outl(0x1800db4c, 17, 17, 0x0); //[DPHY fix]  reg_p0_b_offset_gray_en_de
	_phy_rtd_part_outl(0x1800db4c, 26, 22, 0x00); //[DPHY fix]  reg_p0_b_offset_ini_do
	_phy_rtd_part_outl(0x1800db4c, 27, 27, 0x0); //[DPHY fix]  reg_p0_b_offset_gray_en_do
	_phy_rtd_part_outl(0x1800db50, 6, 2, 0x00); //[DPHY fix]  reg_p0_b_offset_ini_ee
	_phy_rtd_part_outl(0x1800db50, 7, 7, 0x0); //[DPHY fix]  reg_p0_b_offset_gray_en_ee
	_phy_rtd_part_outl(0x1800db50, 16, 12, 0x00); //[DPHY fix]  reg_p0_b_offset_ini_eo
	_phy_rtd_part_outl(0x1800db50, 17, 17, 0x0); //[DPHY fix]  reg_p0_b_offset_gray_en_eo
	_phy_rtd_part_outl(0x1800db50, 26, 22, 0x00); //[DPHY fix]  reg_p0_b_offset_ini_eq
	_phy_rtd_part_outl(0x1800db50, 27, 27, 0x0); //[DPHY fix]  reg_p0_b_offset_gray_en_eq
	_phy_rtd_part_outl(0x1800db50, 29, 29, 0x0); //[DPHY fix]  reg_p0_b_offset_z0_ok_eq
	_phy_rtd_part_outl(0x1800db54, 11, 8, 0xf); //[DPHY fix]  reg_p0_g_offset_delay_cnt
	_phy_rtd_part_outl(0x1800db54, 13, 12, 0x0); //[DPHY fix]  reg_p0_g_offset_stable_cnt
	_phy_rtd_part_outl(0x1800db54, 19, 14, 0x0b); //[DPHY fix]  reg_p0_g_offset_divisor
	_phy_rtd_part_outl(0x1800db54, 26, 20, 0x28); //[DPHY fix]  reg_p0_g_offset_timeout
	_phy_rtd_part_outl(0x1800db58, 0, 0, 0x0); //[]  reg_p0_g_offset_en_ope
	_phy_rtd_part_outl(0x1800db58, 1, 1, 0x0); //[]  reg_p0_g_offset_pc_ope
	_phy_rtd_part_outl(0x1800db58, 6, 2, 0x00); //[]  reg_p0_g_offset_ini_ope
	_phy_rtd_part_outl(0x1800db58, 7, 7, 0x0); //[]  reg_p0_g_offset_gray_en_ope
	_phy_rtd_part_outl(0x1800db58, 8, 8, 0x0); //[]  reg_p0_g_offset_manual_ope
	_phy_rtd_part_outl(0x1800db58, 9, 9, 0x0); //[]  reg_p0_g_offset_z0_ok_ope
	_phy_rtd_part_outl(0x1800db58, 10, 10, 0x0); //[]  reg_p0_g_offset_en_opo
	_phy_rtd_part_outl(0x1800db58, 11, 11, 0x0); //[]  reg_p0_g_offset_pc_opo
	_phy_rtd_part_outl(0x1800db58, 16, 12, 0x00); //[]  reg_p0_g_offset_ini_opo
	_phy_rtd_part_outl(0x1800db58, 17, 17, 0x0); //[]  reg_p0_g_offset_gray_en_opo
	_phy_rtd_part_outl(0x1800db58, 18, 18, 0x0); //[]  reg_p0_g_offset_manual_opo
	_phy_rtd_part_outl(0x1800db58, 19, 19, 0x0); //[]  reg_p0_g_offset_z0_ok_opo
	_phy_rtd_part_outl(0x1800db58, 20, 20, 0x0); //[]  reg_p0_g_offset_en_one
	_phy_rtd_part_outl(0x1800db58, 21, 21, 0x0); //[]  reg_p0_g_offset_pc_one
	_phy_rtd_part_outl(0x1800db58, 26, 22, 0x00); //[]  reg_p0_g_offset_ini_one
	_phy_rtd_part_outl(0x1800db58, 27, 27, 0x0); //[]  reg_p0_g_offset_gray_en_one
	_phy_rtd_part_outl(0x1800db58, 28, 28, 0x0); //[]  reg_p0_g_offset_manual_one
	_phy_rtd_part_outl(0x1800db58, 29, 29, 0x0); //[]  reg_p0_g_offset_z0_ok_one
	_phy_rtd_part_outl(0x1800db5c, 0, 0, 0x0); //[DPHY fix]  reg_p0_g_offset_en_ono
	_phy_rtd_part_outl(0x1800db5c, 6, 2, 0x00); //[DPHY fix]  reg_p0_g_offset_ini_ono
	_phy_rtd_part_outl(0x1800db5c, 7, 7, 0x0); //[DPHY fix]  reg_p0_g_offset_gray_en_ono
	_phy_rtd_part_outl(0x1800db5c, 1, 1, 0x0); //[DPHY fix]  reg_p0_g_offset_pc_ono
	_phy_rtd_part_outl(0x1800db5c, 8, 8, 0x0); //[DPHY fix]  reg_p0_g_offset_manual_ono
	_phy_rtd_part_outl(0x1800db5c, 9, 9, 0x0); //[DPHY fix]  reg_p0_g_offset_z0_ok_ono
	_phy_rtd_part_outl(0x1800db5c, 16, 12, 0x00); //[DPHY fix]  reg_p0_g_offset_ini_de
	_phy_rtd_part_outl(0x1800db5c, 17, 17, 0x0); //[DPHY fix]  reg_p0_g_offset_gray_en_de
	_phy_rtd_part_outl(0x1800db5c, 26, 22, 0x00); //[DPHY fix]  reg_p0_g_offset_ini_do
	_phy_rtd_part_outl(0x1800db5c, 27, 27, 0x0); //[DPHY fix]  reg_p0_g_offset_gray_en_do
	_phy_rtd_part_outl(0x1800db60, 6, 2, 0x00); //[DPHY fix]  reg_p0_g_offset_ini_ee
	_phy_rtd_part_outl(0x1800db60, 7, 7, 0x0); //[DPHY fix]  reg_p0_g_offset_gray_en_ee
	_phy_rtd_part_outl(0x1800db60, 16, 12, 0x00); //[DPHY fix]  reg_p0_g_offset_ini_eo
	_phy_rtd_part_outl(0x1800db60, 17, 17, 0x0); //[DPHY fix]  reg_p0_g_offset_gray_en_eo
	_phy_rtd_part_outl(0x1800db60, 26, 22, 0x00); //[DPHY fix]  reg_p0_g_offset_ini_eq
	_phy_rtd_part_outl(0x1800db60, 27, 27, 0x0); //[DPHY fix]  reg_p0_g_offset_gray_en_eq
	_phy_rtd_part_outl(0x1800db60, 29, 29, 0x0); //[DPHY fix]  reg_p0_g_offset_z0_ok_eq
	_phy_rtd_part_outl(0x1800db64, 11, 8, 0xf); //[DPHY fix]  reg_p0_r_offset_delay_cnt
	_phy_rtd_part_outl(0x1800db64, 13, 12, 0x0); //[DPHY fix]  reg_p0_r_offset_stable_cnt
	_phy_rtd_part_outl(0x1800db64, 19, 14, 0x0b); //[DPHY fix]  reg_p0_r_offset_divisor
	_phy_rtd_part_outl(0x1800db64, 26, 20, 0x28); //[DPHY fix]  reg_p0_r_offset_timeout
	_phy_rtd_part_outl(0x1800db68, 0, 0, 0x0); //[]  reg_p0_r_offset_en_ope
	_phy_rtd_part_outl(0x1800db68, 1, 1, 0x0); //[]  reg_p0_r_offset_pc_ope
	_phy_rtd_part_outl(0x1800db68, 6, 2, 0x00); //[]  reg_p0_r_offset_ini_ope
	_phy_rtd_part_outl(0x1800db68, 7, 7, 0x0); //[]  reg_p0_r_offset_gray_en_ope
	_phy_rtd_part_outl(0x1800db68, 8, 8, 0x0); //[]  reg_p0_r_offset_manual_ope
	_phy_rtd_part_outl(0x1800db68, 9, 9, 0x0); //[]  reg_p0_r_offset_z0_ok_ope
	_phy_rtd_part_outl(0x1800db68, 10, 10, 0x0); //[]  reg_p0_r_offset_en_opo
	_phy_rtd_part_outl(0x1800db68, 11, 11, 0x0); //[]  reg_p0_r_offset_pc_opo
	_phy_rtd_part_outl(0x1800db68, 16, 12, 0x00); //[]  reg_p0_r_offset_ini_opo
	_phy_rtd_part_outl(0x1800db68, 17, 17, 0x0); //[]  reg_p0_r_offset_gray_en_opo
	_phy_rtd_part_outl(0x1800db68, 18, 18, 0x0); //[]  reg_p0_r_offset_manual_opo
	_phy_rtd_part_outl(0x1800db68, 19, 19, 0x0); //[]  reg_p0_r_offset_z0_ok_opo
	_phy_rtd_part_outl(0x1800db68, 20, 20, 0x0); //[]  reg_p0_r_offset_en_one
	_phy_rtd_part_outl(0x1800db68, 21, 21, 0x0); //[]  reg_p0_r_offset_pc_one
	_phy_rtd_part_outl(0x1800db68, 26, 22, 0x00); //[]  reg_p0_r_offset_ini_one
	_phy_rtd_part_outl(0x1800db68, 27, 27, 0x0); //[]  reg_p0_r_offset_gray_en_one
	_phy_rtd_part_outl(0x1800db68, 28, 28, 0x0); //[]  reg_p0_r_offset_manual_one
	_phy_rtd_part_outl(0x1800db68, 29, 29, 0x0); //[]  reg_p0_r_offset_z0_ok_one
	_phy_rtd_part_outl(0x1800db6c, 0, 0, 0x0); //[DPHY fix]  reg_p0_r_offset_en_ono
	_phy_rtd_part_outl(0x1800db6c, 1, 1, 0x0); //[DPHY fix]  reg_p0_r_offset_pc_ono
	_phy_rtd_part_outl(0x1800db6c, 6, 2, 0x00); //[DPHY fix]  reg_p0_r_offset_ini_ono
	_phy_rtd_part_outl(0x1800db6c, 7, 7, 0x0); //[DPHY fix]  reg_p0_r_offset_gray_en_ono
	_phy_rtd_part_outl(0x1800db6c, 8, 8, 0x0); //[DPHY fix]  reg_p0_r_offset_manual_ono
	_phy_rtd_part_outl(0x1800db6c, 9, 9, 0x0); //[DPHY fix]  reg_p0_r_offset_z0_ok_ono
	_phy_rtd_part_outl(0x1800db6c, 16, 12, 0x00); //[DPHY fix]  reg_p0_r_offset_ini_de
	_phy_rtd_part_outl(0x1800db6c, 17, 17, 0x0); //[DPHY fix]  reg_p0_r_offset_gray_en_de
	_phy_rtd_part_outl(0x1800db6c, 26, 22, 0x00); //[DPHY fix]  reg_p0_r_offset_ini_do
	_phy_rtd_part_outl(0x1800db6c, 27, 27, 0x0); //[DPHY fix]  reg_p0_r_offset_gray_en_do
	_phy_rtd_part_outl(0x1800db70, 6, 2, 0x00); //[DPHY fix]  reg_p0_r_offset_ini_ee
	_phy_rtd_part_outl(0x1800db70, 7, 7, 0x0); //[DPHY fix]  reg_p0_r_offset_gray_en_ee
	_phy_rtd_part_outl(0x1800db70, 16, 12, 0x00); //[DPHY fix]  reg_p0_r_offset_ini_eo
	_phy_rtd_part_outl(0x1800db70, 17, 17, 0x0); //[DPHY fix]  reg_p0_r_offset_gray_en_eo
	_phy_rtd_part_outl(0x1800db70, 26, 22, 0x00); //[DPHY fix]  reg_p0_r_offset_ini_eq
	_phy_rtd_part_outl(0x1800db70, 27, 27, 0x0); //[DPHY fix]  reg_p0_r_offset_gray_en_eq
	_phy_rtd_part_outl(0x1800db70, 29, 29, 0x0); //[DPHY fix]  reg_p0_r_offset_z0_ok_eq
	_phy_rtd_part_outl(0x1800db74, 11, 8, 0xf); //[DPHY fix]  reg_p0_ck_offset_delay_cnt
	_phy_rtd_part_outl(0x1800db74, 13, 12, 0x0); //[DPHY fix]  reg_p0_ck_offset_stable_cnt
	_phy_rtd_part_outl(0x1800db74, 19, 14, 0x0b); //[DPHY fix]  reg_p0_ck_offset_divisor
	_phy_rtd_part_outl(0x1800db74, 26, 20, 0x28); //[DPHY fix]  reg_p0_ck_offset_timeout
	_phy_rtd_part_outl(0x1800db78, 0, 0, 0x0); //[]  reg_p0_ck_offset_en_ope
	_phy_rtd_part_outl(0x1800db78, 1, 1, 0x0); //[]  reg_p0_ck_offset_pc_ope
	_phy_rtd_part_outl(0x1800db78, 6, 2, 0x00); //[]  reg_p0_ck_offset_ini_ope
	_phy_rtd_part_outl(0x1800db78, 7, 7, 0x0); //[]  reg_p0_ck_offset_gray_en_ope
	_phy_rtd_part_outl(0x1800db78, 8, 8, 0x0); //[]  reg_p0_ck_offset_manual_ope
	_phy_rtd_part_outl(0x1800db78, 9, 9, 0x0); //[]  reg_p0_ck_offset_z0_ok_ope
	_phy_rtd_part_outl(0x1800db78, 10, 10, 0x0); //[]  reg_p0_ck_offset_en_opo
	_phy_rtd_part_outl(0x1800db78, 11, 11, 0x0); //[]  reg_p0_ck_offset_pc_opo
	_phy_rtd_part_outl(0x1800db78, 16, 12, 0x00); //[]  reg_p0_ck_offset_ini_opo
	_phy_rtd_part_outl(0x1800db78, 17, 17, 0x0); //[]  reg_p0_ck_offset_gray_en_opo
	_phy_rtd_part_outl(0x1800db78, 18, 18, 0x0); //[]  reg_p0_ck_offset_manual_opo
	_phy_rtd_part_outl(0x1800db78, 19, 19, 0x0); //[]  reg_p0_ck_offset_z0_ok_opo
	_phy_rtd_part_outl(0x1800db78, 20, 20, 0x0); //[]  reg_p0_ck_offset_en_one
	_phy_rtd_part_outl(0x1800db78, 21, 21, 0x0); //[]  reg_p0_ck_offset_pc_one
	_phy_rtd_part_outl(0x1800db78, 26, 22, 0x00); //[]  reg_p0_ck_offset_ini_one
	_phy_rtd_part_outl(0x1800db78, 27, 27, 0x0); //[]  reg_p0_ck_offset_gray_en_one
	_phy_rtd_part_outl(0x1800db78, 28, 28, 0x0); //[]  reg_p0_ck_offset_manual_one
	_phy_rtd_part_outl(0x1800db78, 29, 29, 0x0); //[]  reg_p0_ck_offset_z0_ok_one
	_phy_rtd_part_outl(0x1800db7c, 0, 0, 0x0); //[DPHY fix]  reg_p0_ck_offset_en_ono
	_phy_rtd_part_outl(0x1800db7c, 1, 1, 0x0); //[DPHY fix]  reg_p0_ck_offset_pc_ono
	_phy_rtd_part_outl(0x1800db7c, 6, 2, 0x00); //[DPHY fix]  reg_p0_ck_offset_ini_ono
	_phy_rtd_part_outl(0x1800db7c, 7, 7, 0x0); //[DPHY fix]  reg_p0_ck_offset_gray_en_ono
	_phy_rtd_part_outl(0x1800db7c, 8, 8, 0x0); //[DPHY fix]  reg_p0_ck_offset_manual_ono
	_phy_rtd_part_outl(0x1800db7c, 9, 9, 0x0); //[DPHY fix]  reg_p0_ck_offset_z0_ok_ono
	_phy_rtd_part_outl(0x1800db7c, 16, 12, 0x00); //[DPHY fix]  reg_p0_ck_offset_ini_de
	_phy_rtd_part_outl(0x1800db7c, 17, 17, 0x0); //[DPHY fix]  reg_p0_ck_offset_gray_en_de
	_phy_rtd_part_outl(0x1800db7c, 26, 22, 0x00); //[DPHY fix]  reg_p0_ck_offset_ini_do
	_phy_rtd_part_outl(0x1800db7c, 27, 27, 0x0); //[DPHY fix]  reg_p0_ck_offset_gray_en_do
	_phy_rtd_part_outl(0x1800db80, 6, 2, 0x00); //[DPHY fix]  reg_p0_ck_offset_ini_ee
	_phy_rtd_part_outl(0x1800db80, 7, 7, 0x0); //[DPHY fix]  reg_p0_ck_offset_gray_en_ee
	_phy_rtd_part_outl(0x1800db80, 16, 12, 0x00); //[DPHY fix]  reg_p0_ck_offset_ini_eo
	_phy_rtd_part_outl(0x1800db80, 17, 17, 0x0); //[DPHY fix]  reg_p0_ck_offset_gray_en_eo
	_phy_rtd_part_outl(0x1800db80, 26, 22, 0x00); //[DPHY fix]  reg_p0_ck_offset_ini_eq
	_phy_rtd_part_outl(0x1800db80, 27, 27, 0x0); //[DPHY fix]  reg_p0_ck_offset_gray_en_eq
	_phy_rtd_part_outl(0x1800db80, 29, 29, 0x0); //[DPHY fix]  reg_p0_ck_offset_z0_ok_eq
	_phy_rtd_part_outl(0x1800db88, 2, 2, 0x0); //[DPHY fix]  reg_p0_b_pn_swap_en
	_phy_rtd_part_outl(0x1800db88, 3, 3, 0x1); //[DPHY fix]  reg_p0_b_dfe_data_en
	_phy_rtd_part_outl(0x1800db88, 7, 5, 0x3); //[DPHY fix]  reg_p0_b_inbuf_num
	_phy_rtd_part_outl(0x1800db88, 10, 10, 0x0); //[DPHY fix]  reg_p0_g_pn_swap_en
	_phy_rtd_part_outl(0x1800db88, 11, 11, 0x1); //[DPHY fix]  reg_p0_g_dfe_data_en
	_phy_rtd_part_outl(0x1800db88, 15, 13, 0x3); //[DPHY fix]  reg_p0_g_inbuf_num
	_phy_rtd_part_outl(0x1800db88, 18, 18, 0x0); //[DPHY fix]  reg_p0_r_pn_swap_en
	_phy_rtd_part_outl(0x1800db88, 19, 19, 0x1); //[DPHY fix]  reg_p0_r_dfe_data_en
	_phy_rtd_part_outl(0x1800db88, 23, 21, 0x3); //[DPHY fix]  reg_p0_r_inbuf_num
	_phy_rtd_part_outl(0x1800db88, 26, 26, 0x0); //[DPHY fix]  reg_p0_ck_pn_swap_en
	_phy_rtd_part_outl(0x1800db88, 27, 27, 0x1); //[DPHY fix]  reg_p0_ck_dfe_data_en
	_phy_rtd_part_outl(0x1800db88, 31, 29, 0x3); //[DPHY fix]  reg_p0_ck_inbuf_num
	_phy_rtd_part_outl(0x1800db8c, 0, 0, 0x0); //[]  reg_p0_b_001_enable
	_phy_rtd_part_outl(0x1800db8c, 1, 1, 0x0); //[]  reg_p0_b_101_enable
	_phy_rtd_part_outl(0x1800db8c, 2, 2, 0x0); //[]  reg_p0_b_en_bec_acc
	_phy_rtd_part_outl(0x1800db8c, 3, 3, 0x0); //[]  reg_p0_b_en_bec_read
	_phy_rtd_part_outl(0x1800db8c, 12, 12, 0x0); //[]  reg_p0_g_001_enable
	_phy_rtd_part_outl(0x1800db8c, 13, 13, 0x0); //[]  reg_p0_g_101_enable
	_phy_rtd_part_outl(0x1800db8c, 14, 14, 0x0); //[]  reg_p0_g_en_bec_acc
	_phy_rtd_part_outl(0x1800db8c, 15, 15, 0x0); //[]  reg_p0_g_en_bec_read
	_phy_rtd_part_outl(0x1800db90, 0, 0, 0x0); //[]  reg_p0_r_001_enable
	_phy_rtd_part_outl(0x1800db90, 1, 1, 0x0); //[]  reg_p0_r_101_enable
	_phy_rtd_part_outl(0x1800db90, 2, 2, 0x0); //[]  reg_p0_r_en_bec_acc
	_phy_rtd_part_outl(0x1800db90, 3, 3, 0x0); //[]  reg_p0_r_en_bec_read
	_phy_rtd_part_outl(0x1800db90, 12, 12, 0x0); //[]  reg_p0_ck_001_enable
	_phy_rtd_part_outl(0x1800db90, 13, 13, 0x0); //[]  reg_p0_ck_101_enable
	_phy_rtd_part_outl(0x1800db90, 14, 14, 0x0); //[]  reg_p0_ck_en_bec_acc
	_phy_rtd_part_outl(0x1800db90, 15, 15, 0x0); //[]  reg_p0_ck_en_bec_read
	_phy_rtd_part_outl(0x1800dbb8, 0, 0, 0x0); //[DPHY fix]  reg_p0_b_cp2adp_en
	_phy_rtd_part_outl(0x1800dbb8, 1, 1, 0x0); //[DPHY fix]  reg_p0_b_stable_time_mode
	_phy_rtd_part_outl(0x1800dbb8, 2, 2, 0x0); //[DPHY fix]  reg_p0_b_bypass_k_band_mode
	_phy_rtd_part_outl(0x1800dbb8, 18, 18, 0x0); //[DPHY fix]  reg_p0_b_calib_late
	_phy_rtd_part_outl(0x1800dbb8, 19, 19, 0x0); //[DPHY fix]  reg_p0_b_calib_manual
	_phy_rtd_part_outl(0x1800dbb8, 22, 20, 0x2); //[DPHY fix]  reg_p0_b_calib_time
	_phy_rtd_part_outl(0x1800dbb8, 27, 23, 0x0c); //[DPHY fix]  reg_p0_b_adp_time
	_phy_rtd_part_outl(0x1800dbb8, 28, 28, 0x0); //[DPHY fix]  reg_p0_b_adap_eq_off
	_phy_rtd_part_outl(0x1800dbb8, 29, 29, 0x0); //[DPHY fix]  reg_p0_b_cp_en_manual
	_phy_rtd_part_outl(0x1800dbb8, 30, 30, 0x0); //[DPHY fix]  reg_p0_b_adp_en_manual
	_phy_rtd_part_outl(0x1800dbb8, 31, 31, 0x1); //[DPHY fix]  reg_p0_b_auto_mode
	_phy_rtd_part_outl(0x1800dbbc, 31, 28, 0x8); //[DPHY fix] reg_p0_b_cp2adp_time
	_phy_rtd_part_outl(0x1800dbc0, 1, 1, 0x1); //[] reg_p0_b_calib_reset_sel
	_phy_rtd_part_outl(0x1800dbc0, 3, 2, 0x0); //[] reg_p0_b_vc_sel
	_phy_rtd_part_outl(0x1800dbc0, 5, 4, 0x0); //[] reg_p0_b_cdr_c
	_phy_rtd_part_outl(0x1800dbc0, 11, 6, 0x00); //[] reg_p0_b_cdr_r
	_phy_rtd_part_outl(0x1800dbc0, 31, 12, 0xa9878); //[] reg_p0_b_cdr_cp
	_phy_rtd_part_outl(0x1800dbc4, 2, 0, 0x0); //[DPHY fix] reg_p0_b_init_time
	_phy_rtd_part_outl(0x1800dbc4, 7, 3, 0x04); //[DPHY fix] reg_p0_b_cp_time
	_phy_rtd_part_outl(0x1800dbc8, 6, 2, 0x01); //[DPHY fix]  reg_p0_b_timer_6
	_phy_rtd_part_outl(0x1800dbc8, 7, 7, 0x0); //[DPHY fix]  reg_p0_b_timer_5
	_phy_rtd_part_outl(0x1800dbc8, 11, 8, 0x7); //[DPHY fix]  reg_p0_b_vco_fine_init
	_phy_rtd_part_outl(0x1800dbc8, 15, 12, 0x7); //[DPHY fix]  reg_p0_b_vco_coarse_init
	_phy_rtd_part_outl(0x1800dbc8, 16, 16, 0x0); //[DPHY fix]  reg_p0_b_bypass_coarse_k_mode
	_phy_rtd_part_outl(0x1800dbc8, 18, 17, 0x0); //[DPHY fix]  reg_p0_b_vco_fine_time_sel
	_phy_rtd_part_outl(0x1800dbc8, 19, 19, 0x0); //[DPHY fix]  reg_p0_b_xtal_24m_en
	_phy_rtd_part_outl(0x1800dbc8, 23, 20, 0x0); //[DPHY fix]  reg_p0_b_vco_fine_manual
	_phy_rtd_part_outl(0x1800dbc8, 28, 28, 0x0); //[DPHY fix]  reg_p0_b_coarse_calib_manual
	_phy_rtd_part_outl(0x1800dbc8, 29, 29, 0x0); //[DPHY fix]  reg_p0_b_acdr_ckfld_en
	_phy_rtd_part_outl(0x1800dbc8, 30, 30, 0x0); //[DPHY fix]  reg_p0_b_pll_to_acdr_manual_en
	_phy_rtd_part_outl(0x1800dbc8, 31, 31, 0x0); //[DPHY fix]  reg_p0_b_pll_to_acdr_rdy_manual
	_phy_rtd_part_outl(0x1800dbcc, 4, 0, 0x01); //[]  reg_p0_b_timer_4
	_phy_rtd_part_outl(0x1800dbcc, 7, 5, 0x0); //[]  reg_p0_b_vc_chg_time
	_phy_rtd_part_outl(0x1800dbcc, 16, 16, 0x1); //[]  reg_p0_b_old_mode
	_phy_rtd_part_outl(0x1800dbcc, 21, 17, 0x10); //[]  reg_p0_b_slope_band
	_phy_rtd_part_outl(0x1800dbcc, 22, 22, 0x0); //[]  reg_p0_b_slope_manual
	_phy_rtd_part_outl(0x1800dbcc, 23, 23, 0x0); //[]  reg_p0_b_rxidle_bypass
	_phy_rtd_part_outl(0x1800dbcc, 24, 24, 0x0); //[]  reg_p0_b_pfd_bypass
	_phy_rtd_part_outl(0x1800dbcc, 29, 25, 0x00); //[]  reg_p0_b_pfd_time
	_phy_rtd_part_outl(0x1800dbcc, 30, 30, 0x0); //[]  reg_p0_b_pfd_en_manual
	_phy_rtd_part_outl(0x1800dbcc, 31, 31, 0x0); //[]  reg_p0_b_start_en_manual
	_phy_rtd_part_outl(0x1800dbd0, 9, 5, 0x08); //[]  reg_p0_b_cp_time_2
	_phy_rtd_part_outl(0x1800dbd8, 0, 0, 0x0); //[DPHY fix]  reg_p0_g_cp2adp_en
	_phy_rtd_part_outl(0x1800dbd8, 1, 1, 0x0); //[DPHY fix]  reg_p0_g_stable_time_mode
	_phy_rtd_part_outl(0x1800dbd8, 2, 2, 0x0); //[DPHY fix]  reg_p0_g_bypass_k_band_mode
	_phy_rtd_part_outl(0x1800dbd8, 18, 18, 0x0); //[DPHY fix]  reg_p0_g_calib_late
	_phy_rtd_part_outl(0x1800dbd8, 19, 19, 0x0); //[DPHY fix]  reg_p0_g_calib_manual
	_phy_rtd_part_outl(0x1800dbd8, 22, 20, 0x2); //[DPHY fix]  reg_p0_g_calib_time
	_phy_rtd_part_outl(0x1800dbd8, 27, 23, 0x0c); //[DPHY fix]  reg_p0_g_adp_time
	_phy_rtd_part_outl(0x1800dbd8, 28, 28, 0x0); //[DPHY fix]  reg_p0_g_adap_eq_off
	_phy_rtd_part_outl(0x1800dbd8, 29, 29, 0x0); //[DPHY fix]  reg_p0_g_cp_en_manual
	_phy_rtd_part_outl(0x1800dbd8, 30, 30, 0x0); //[DPHY fix]  reg_p0_g_adp_en_manual
	_phy_rtd_part_outl(0x1800dbd8, 31, 31, 0x1); //[DPHY fix]  reg_p0_g_auto_mode
	_phy_rtd_part_outl(0x1800dbdc, 31, 28, 0x8); //[DPHY fix] reg_p0_g_cp2adp_time
	_phy_rtd_part_outl(0x1800dbe0, 1, 1, 0x1); //[] reg_p0_g_calib_reset_sel
	_phy_rtd_part_outl(0x1800dbe0, 3, 2, 0x0); //[] reg_p0_g_vc_sel
	_phy_rtd_part_outl(0x1800dbe0, 5, 4, 0x0); //[] reg_p0_g_cdr_c
	_phy_rtd_part_outl(0x1800dbe0, 11, 6, 0x00); //[] reg_p0_g_cdr_r
	_phy_rtd_part_outl(0x1800dbe0, 31, 12, 0xa9878); //[] reg_p0_g_cdr_cp
	_phy_rtd_part_outl(0x1800dbe4, 2, 0, 0x0); //[DPHY fix] reg_p0_g_init_time
	_phy_rtd_part_outl(0x1800dbe4, 7, 3, 0x04); //[DPHY fix] reg_p0_g_cp_time
	_phy_rtd_part_outl(0x1800dbe8, 6, 2, 0x01); //[DPHY fix]  reg_p0_g_timer_6
	_phy_rtd_part_outl(0x1800dbe8, 7, 7, 0x0); //[DPHY fix]  reg_p0_g_timer_5
	_phy_rtd_part_outl(0x1800dbe8, 11, 8, 0x7); //[DPHY fix]  reg_p0_g_vco_fine_init
	_phy_rtd_part_outl(0x1800dbe8, 15, 12, 0x7); //[DPHY fix]  reg_p0_g_vco_coarse_init
	_phy_rtd_part_outl(0x1800dbe8, 16, 16, 0x0); //[DPHY fix]  reg_p0_g_bypass_coarse_k_mode
	_phy_rtd_part_outl(0x1800dbe8, 18, 17, 0x0); //[DPHY fix]  reg_p0_g_vco_fine_time_sel
	_phy_rtd_part_outl(0x1800dbe8, 19, 19, 0x0); //[DPHY fix]  reg_p0_g_xtal_24m_en
	_phy_rtd_part_outl(0x1800dbe8, 23, 20, 0x0); //[DPHY fix]  reg_p0_g_vco_fine_manual
	_phy_rtd_part_outl(0x1800dbe8, 28, 28, 0x0); //[DPHY fix]  reg_p0_g_coarse_calib_manual
	_phy_rtd_part_outl(0x1800dbe8, 29, 29, 0x0); //[DPHY fix]  reg_p0_g_acdr_ckfld_en
	_phy_rtd_part_outl(0x1800dbe8, 30, 30, 0x0); //[DPHY fix]  reg_p0_g_pll_to_acdr_manual_en
	_phy_rtd_part_outl(0x1800dbe8, 31, 31, 0x0); //[DPHY fix]  reg_p0_g_pll_to_acdr_rdy_manual
	_phy_rtd_part_outl(0x1800dbec, 4, 0, 0x01); //[]  reg_p0_g_timer_4
	_phy_rtd_part_outl(0x1800dbec, 7, 5, 0x0); //[]  reg_p0_g_vc_chg_time
	_phy_rtd_part_outl(0x1800dbec, 16, 16, 0x1); //[]  reg_p0_g_old_mode
	_phy_rtd_part_outl(0x1800dbec, 21, 17, 0x10); //[]  reg_p0_g_slope_band
	_phy_rtd_part_outl(0x1800dbec, 22, 22, 0x0); //[]  reg_p0_g_slope_manual
	_phy_rtd_part_outl(0x1800dbec, 23, 23, 0x0); //[]  reg_p0_g_rxidle_bypass
	_phy_rtd_part_outl(0x1800dbec, 24, 24, 0x0); //[]  reg_p0_g_pfd_bypass
	_phy_rtd_part_outl(0x1800dbec, 29, 25, 0x00); //[]  reg_p0_g_pfd_time
	_phy_rtd_part_outl(0x1800dbec, 30, 30, 0x0); //[]  reg_p0_g_pfd_en_manual
	_phy_rtd_part_outl(0x1800dbec, 31, 31, 0x0); //[]  reg_p0_g_start_en_manual
	_phy_rtd_part_outl(0x1800dbf0, 9, 5, 0x08); //[]  reg_p0_g_cp_time_2
	_phy_rtd_part_outl(0x1800dbf4, 0, 0, 0x0); //[DPHY fix]  reg_p0_r_cp2adp_en
	_phy_rtd_part_outl(0x1800dbf4, 1, 1, 0x0); //[DPHY fix]  reg_p0_r_stable_time_mode
	_phy_rtd_part_outl(0x1800dbf4, 2, 2, 0x0); //[DPHY fix]  reg_p0_r_bypass_k_band_mode
	_phy_rtd_part_outl(0x1800dbf4, 18, 18, 0x0); //[DPHY fix]  reg_p0_r_calib_late
	_phy_rtd_part_outl(0x1800dbf4, 19, 19, 0x0); //[DPHY fix]  reg_p0_r_calib_manual
	_phy_rtd_part_outl(0x1800dbf4, 22, 20, 0x2); //[DPHY fix]  reg_p0_r_calib_time
	_phy_rtd_part_outl(0x1800dbf4, 27, 23, 0x0c); //[DPHY fix]  reg_p0_r_adp_time
	_phy_rtd_part_outl(0x1800dbf4, 28, 28, 0x0); //[DPHY fix]  reg_p0_r_adap_eq_off
	_phy_rtd_part_outl(0x1800dbf4, 29, 29, 0x0); //[DPHY fix]  reg_p0_r_cp_en_manual
	_phy_rtd_part_outl(0x1800dbf4, 30, 30, 0x0); //[DPHY fix]  reg_p0_r_adp_en_manual
	_phy_rtd_part_outl(0x1800dbf4, 31, 31, 0x1); //[DPHY fix]  reg_p0_r_auto_mode
	_phy_rtd_part_outl(0x1800dbf8, 31, 28, 0x8); //[DPHY fix] reg_p0_r_cp2adp_time
	_phy_rtd_part_outl(0x1800dbfc, 1, 1, 0x1); //[] reg_p0_r_calib_reset_sel
	_phy_rtd_part_outl(0x1800dbfc, 3, 2, 0x0); //[] reg_p0_r_vc_sel
	_phy_rtd_part_outl(0x1800dbfc, 5, 4, 0x0); //[] reg_p0_r_cdr_c
	_phy_rtd_part_outl(0x1800dbfc, 11, 6, 0x00); //[] reg_p0_r_cdr_r
	_phy_rtd_part_outl(0x1800dbfc, 31, 12, 0xa9878); //[] reg_p0_r_cdr_cp
	_phy_rtd_part_outl(0x1800dc00, 2, 0, 0x0); //[DPHY fix] reg_p0_r_init_time
	_phy_rtd_part_outl(0x1800dc00, 7, 3, 0x04); //[DPHY fix] reg_p0_r_cp_time
	_phy_rtd_part_outl(0x1800dc04, 6, 2, 0x01); //[DPHY fix]  reg_p0_r_timer_6
	_phy_rtd_part_outl(0x1800dc04, 7, 7, 0x0); //[DPHY fix]  reg_p0_r_timer_5
	_phy_rtd_part_outl(0x1800dc04, 11, 8, 0x7); //[DPHY fix]  reg_p0_r_vco_fine_init
	_phy_rtd_part_outl(0x1800dc04, 15, 12, 0x7); //[DPHY fix]  reg_p0_r_vco_coarse_init
	_phy_rtd_part_outl(0x1800dc04, 16, 16, 0x0); //[DPHY fix]  reg_p0_r_bypass_coarse_k_mode
	_phy_rtd_part_outl(0x1800dc04, 18, 17, 0x0); //[DPHY fix]  reg_p0_r_vco_fine_time_sel
	_phy_rtd_part_outl(0x1800dc04, 19, 19, 0x0); //[DPHY fix]  reg_p0_r_xtal_24m_en
	_phy_rtd_part_outl(0x1800dc04, 23, 20, 0x0); //[DPHY fix]  reg_p0_r_vco_fine_manual
	_phy_rtd_part_outl(0x1800dc04, 28, 28, 0x0); //[DPHY fix]  reg_p0_r_coarse_calib_manual
	_phy_rtd_part_outl(0x1800dc04, 29, 29, 0x0); //[DPHY fix]  reg_p0_r_acdr_ckfld_en
	_phy_rtd_part_outl(0x1800dc04, 30, 30, 0x0); //[DPHY fix]  reg_p0_r_pll_to_acdr_manual_en
	_phy_rtd_part_outl(0x1800dc04, 31, 31, 0x0); //[DPHY fix]  reg_p0_r_pll_to_acdr_rdy_manual
	_phy_rtd_part_outl(0x1800dc08, 4, 0, 0x01); //[]  reg_p0_r_timer_4
	_phy_rtd_part_outl(0x1800dc08, 7, 5, 0x0); //[]  reg_p0_r_vc_chg_time
	_phy_rtd_part_outl(0x1800dc08, 16, 16, 0x1); //[]  reg_p0_r_old_mode
	_phy_rtd_part_outl(0x1800dc08, 21, 17, 0x10); //[]  reg_p0_r_slope_band
	_phy_rtd_part_outl(0x1800dc08, 22, 22, 0x0); //[]  reg_p0_r_slope_manual
	_phy_rtd_part_outl(0x1800dc08, 23, 23, 0x0); //[]  reg_p0_r_rxidle_bypass
	_phy_rtd_part_outl(0x1800dc08, 24, 24, 0x0); //[]  reg_p0_r_pfd_bypass
	_phy_rtd_part_outl(0x1800dc08, 29, 25, 0x00); //[]  reg_p0_r_pfd_time
	_phy_rtd_part_outl(0x1800dc08, 30, 30, 0x0); //[]  reg_p0_r_pfd_en_manual
	_phy_rtd_part_outl(0x1800dc08, 31, 31, 0x0); //[]  reg_p0_r_start_en_manual
	_phy_rtd_part_outl(0x1800dc0c, 9, 5, 0x08); //[]  reg_p0_r_cp_time_2
	_phy_rtd_part_outl(0x1800dc10, 0, 0, 0x0); //[DPHY fix]  reg_p0_ck_cp2adp_en
	_phy_rtd_part_outl(0x1800dc10, 1, 1, 0x0); //[DPHY fix]  reg_p0_ck_stable_time_mode
	_phy_rtd_part_outl(0x1800dc10, 2, 2, 0x0); //[DPHY fix]  reg_p0_ck_bypass_k_band_mode
	_phy_rtd_part_outl(0x1800dc10, 18, 18, 0x0); //[DPHY fix]  reg_p0_ck_calib_late
	_phy_rtd_part_outl(0x1800dc10, 19, 19, 0x0); //[DPHY fix]  reg_p0_ck_calib_manual
	_phy_rtd_part_outl(0x1800dc10, 22, 20, 0x2); //[DPHY fix]  reg_p0_ck_calib_time
	_phy_rtd_part_outl(0x1800dc10, 27, 23, 0x0c); //[DPHY fix]  reg_p0_ck_adp_time
	_phy_rtd_part_outl(0x1800dc10, 28, 28, 0x0); //[DPHY fix]  reg_p0_ck_adap_eq_off
	_phy_rtd_part_outl(0x1800dc10, 29, 29, 0x0); //[DPHY fix]  reg_p0_ck_cp_en_manual
	_phy_rtd_part_outl(0x1800dc10, 30, 30, 0x0); //[DPHY fix]  reg_p0_ck_adp_en_manual
	_phy_rtd_part_outl(0x1800dc10, 31, 31, 0x1); //[DPHY fix]  reg_p0_ck_auto_mode
	_phy_rtd_part_outl(0x1800dc14, 31, 28, 0x8); //[DPHY fix] reg_p0_ck_cp2adp_time
	_phy_rtd_part_outl(0x1800dc18, 1, 1, 0x1); //[] reg_p0_ck_calib_reset_sel
	_phy_rtd_part_outl(0x1800dc18, 3, 2, 0x0); //[] reg_p0_ck_vc_sel
	_phy_rtd_part_outl(0x1800dc18, 5, 4, 0x0); //[] reg_p0_ck_cdr_c
	_phy_rtd_part_outl(0x1800dc18, 11, 6, 0x00); //[] reg_p0_ck_cdr_r
	_phy_rtd_part_outl(0x1800dc18, 31, 12, 0xa9878); //[] reg_p0_ck_cdr_cp
	_phy_rtd_part_outl(0x1800dc1c, 2, 0, 0x0); //[DPHY fix] reg_p0_ck_init_time
	_phy_rtd_part_outl(0x1800dc1c, 7, 3, 0x04); //[DPHY fix] reg_p0_ck_cp_time
	_phy_rtd_part_outl(0x1800dc20, 6, 2, 0x01); //[DPHY fix]  reg_p0_ck_timer_6
	_phy_rtd_part_outl(0x1800dc20, 7, 7, 0x0); //[DPHY fix]  reg_p0_ck_timer_5
	_phy_rtd_part_outl(0x1800dc20, 11, 8, 0x7); //[DPHY fix]  reg_p0_ck_vco_fine_init
	_phy_rtd_part_outl(0x1800dc20, 15, 12, 0x7); //[DPHY fix]  reg_p0_ck_vco_coarse_init
	_phy_rtd_part_outl(0x1800dc20, 16, 16, 0x0); //[DPHY fix]  reg_p0_ck_bypass_coarse_k_mode
	_phy_rtd_part_outl(0x1800dc20, 18, 17, 0x0); //[DPHY fix]  reg_p0_ck_vco_fine_time_sel
	_phy_rtd_part_outl(0x1800dc20, 19, 19, 0x0); //[DPHY fix]  reg_p0_ck_xtal_24m_en
	_phy_rtd_part_outl(0x1800dc20, 23, 20, 0x0); //[DPHY fix]  reg_p0_ck_vco_fine_manual
	_phy_rtd_part_outl(0x1800dc20, 28, 28, 0x0); //[DPHY fix]  reg_p0_ck_coarse_calib_manual
	_phy_rtd_part_outl(0x1800dc20, 29, 29, 0x0); //[DPHY fix]  reg_p0_ck_acdr_ckfld_en
	_phy_rtd_part_outl(0x1800dc20, 30, 30, 0x0); //[DPHY fix]  reg_p0_ck_pll_to_acdr_manual_en
	_phy_rtd_part_outl(0x1800dc20, 31, 31, 0x0); //[DPHY fix]  reg_p0_ck_pll_to_acdr_rdy_manual
	_phy_rtd_part_outl(0x1800dc24, 4, 0, 0x01); //[]  reg_p0_ck_timer_4
	_phy_rtd_part_outl(0x1800dc24, 7, 5, 0x0); //[]  reg_p0_ck_vc_chg_time
	_phy_rtd_part_outl(0x1800dc24, 16, 16, 0x1); //[]  reg_p0_ck_old_mode
	_phy_rtd_part_outl(0x1800dc24, 21, 17, 0x10); //[]  reg_p0_ck_slope_band
	_phy_rtd_part_outl(0x1800dc24, 22, 22, 0x0); //[]  reg_p0_ck_slope_manual
	_phy_rtd_part_outl(0x1800dc24, 23, 23, 0x0); //[]  reg_p0_ck_rxidle_bypass
	_phy_rtd_part_outl(0x1800dc24, 24, 24, 0x0); //[]  reg_p0_ck_pfd_bypass
	_phy_rtd_part_outl(0x1800dc24, 29, 25, 0x00); //[]  reg_p0_ck_pfd_time
	_phy_rtd_part_outl(0x1800dc24, 30, 30, 0x0); //[]  reg_p0_ck_pfd_en_manual
	_phy_rtd_part_outl(0x1800dc24, 31, 31, 0x0); //[]  reg_p0_ck_start_en_manual
	_phy_rtd_part_outl(0x1800dc28, 9, 5, 0x08); //[]  reg_p0_ck_cp_time_2
	//MOD
	_phy_rtd_part_outl(0x1800dc2c, 0, 0, 0x1); //[]  reg_p0_ck_md_rstb
	_phy_rtd_part_outl(0x1800dc2c, 1, 1, 0x1); //[]  reg_p0_ck_md_auto
	_phy_rtd_part_outl(0x1800dc2c, 3, 2, 0x0); //[]  reg_p0_ck_md_sel
	_phy_rtd_part_outl(0x1800dc2c, 7, 4, 0x0); //[]  reg_p0_ck_md_adj
	_phy_rtd_part_outl(0x1800dc2c, 13, 8, 0x03); //[]  reg_p0_ck_md_threshold
	_phy_rtd_part_outl(0x1800dc2c, 20, 14, 0x00); //[]  reg_p0_ck_md_debounce
	_phy_rtd_part_outl(0x1800dc2c, 25, 21, 0x00); //[]  reg_p0_ck_error_limit
	_phy_rtd_part_outl(0x1800dc2c, 29, 26, 0x0); //[]  reg_p0_ck_md_reserved
	_phy_rtd_part_outl(0x1800dc30, 2, 0, 0x0); //[]  reg_p0_mod_sel
	//PLLCDR
	_phy_rtd_part_outl(0x1800dc34, 0, 0, 0x0); //[]  reg_p0_b_pllcdr_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 1, 1, 0x0); //[]  reg_p0_g_pllcdr_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 2, 2, 0x0); //[]  reg_p0_r_pllcdr_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 3, 3, 0x0); //[]  reg_p0_ck_pllcdr_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 4, 4, 0x0); //[]  reg_p0_b_pllcdr_vcoband_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 5, 5, 0x0); //[]  reg_p0_g_pllcdr_vcoband_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 6, 6, 0x0); //[]  reg_p0_r_pllcdr_vcoband_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 7, 7, 0x0); //[]  reg_p0_ck_pllcdr_vcoband_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 19, 16, 0x0); //[]  reg_p0_b_acdr_icp_sel_manual
	_phy_rtd_part_outl(0x1800dc34, 23, 20, 0x0); //[]  reg_p0_g_acdr_icp_sel_manual
	_phy_rtd_part_outl(0x1800dc34, 27, 24, 0x0); //[]  reg_p0_r_acdr_icp_sel_manual
	_phy_rtd_part_outl(0x1800dc34, 31, 28, 0x3); //[]  reg_p0_ck_acdr_icp_sel_manual
	_phy_rtd_part_outl(0x1800dc38, 3, 0, 0x0); //[]  reg_p0_b_acdr_vco_coarse_i_manual
	_phy_rtd_part_outl(0x1800dc38, 7, 4, 0x0); //[]  reg_p0_g_acdr_vco_coarse_i_manual
	_phy_rtd_part_outl(0x1800dc38, 11, 8, 0x0); //[]  reg_p0_r_acdr_vco_coarse_i_manual
	_phy_rtd_part_outl(0x1800dc38, 15, 12, 0x0); //[]  reg_p0_ck_acdr_vco_coarse_i_manual
	_phy_rtd_part_outl(0x1800dc38, 19, 16, 0x0); //[]  reg_p0_b_acdr_vco_fine_i_manual
	_phy_rtd_part_outl(0x1800dc38, 23, 20, 0x0); //[]  reg_p0_g_acdr_vco_fine_i_manual
	_phy_rtd_part_outl(0x1800dc38, 27, 24, 0x0); //[]  reg_p0_r_acdr_vco_fine_i_manual
	_phy_rtd_part_outl(0x1800dc38, 31, 28, 0x0); //[]  reg_p0_ck_acdr_vco_fine_i_manual
	_phy_rtd_part_outl(0x1800dc6c, 0, 0, 0x0); //[DPHY fix]  reg_p0_acdr_en
	_phy_rtd_part_outl(0x1800dc6c, 2, 2, 0x0); //[DPHY fix]  reg_p0_pow_on_manual
	_phy_rtd_part_outl(0x1800dc6c, 3, 3, 0x0); //[DPHY fix]  reg_p0_pow_on_manual_aphy_en
	_phy_rtd_part_outl(0x1800dc6c, 4, 4, 0x0); //[DPHY fix]  reg_p0_pow_on_manual_b_en
	_phy_rtd_part_outl(0x1800dc6c, 5, 5, 0x0); //[DPHY fix]  reg_p0_pow_on_manual_g_en
	_phy_rtd_part_outl(0x1800dc6c, 6, 6, 0x0); //[DPHY fix]  reg_p0_pow_on_manual_r_en
	_phy_rtd_part_outl(0x1800dc6c, 7, 7, 0x0); //[DPHY fix]  reg_p0_pow_on_manual_ck_en
	_phy_rtd_part_outl(0x1800dc6c, 8, 8, 0x0); //[DPHY fix]  reg_p0_pow_save_bypass_b
	_phy_rtd_part_outl(0x1800dc6c, 9, 9, 0x0); //[DPHY fix]  reg_p0_pow_save_bypass_g
	_phy_rtd_part_outl(0x1800dc6c, 10, 10, 0x0); //[DPHY fix]  reg_p0_pow_save_bypass_r
	_phy_rtd_part_outl(0x1800dc6c, 11, 11, 0x0); //[DPHY fix]  reg_p0_pow_save_bypass_ck
	_phy_rtd_part_outl(0x1800dc6c, 12, 12, 0x0); //[DPHY fix]  reg_p0_pow_save_xtal_24m_enable
	_phy_rtd_part_outl(0x1800dc6c, 14, 13, 0x3); //[DPHY fix]  reg_p0_pow_save_rst_dly_sel
	_phy_rtd_part_outl(0x1800dc70, 17, 16, 0x0); //[DPHY fix]  reg_tap1_mask
	_phy_rtd_part_outl(0x1800dc7c, 31, 31, 0x0); //[DPHY fix]  reg_port_out_sel
	_phy_rtd_part_outl(0x1800dc80, 7, 0, 0x00); //[]  reg_p0_dig_reserved_3
	_phy_rtd_part_outl(0x1800dc80, 15, 8, 0x00); //[]  reg_p0_dig_reserved_2
	_phy_rtd_part_outl(0x1800dc80, 23, 16, 0x00); //[]  reg_p0_dig_reserved_1
	_phy_rtd_part_outl(0x1800dc80, 31, 24, 0x00); //[]  reg_p0_dig_reserved_0
}

void DPHY_Para_tmds_1_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	// DPHY para start
	_phy_rtd_part_outl(0x1800db04, 5, 2, DPHY_Para_tmds_1[tmds_timing].b_kp_2_1); //[]  reg_p0_b_kp, integer
	_phy_rtd_part_outl(0x1800db04, 1, 0, 0x0); //[]  reg_p0_b_kp , fraction
	_phy_rtd_part_outl(0x1800db04, 11, 8, DPHY_Para_tmds_1[tmds_timing].g_kp_4_1); //[]  reg_p0_g_kp, integer
	_phy_rtd_part_outl(0x1800db04, 7, 6, 0x0); //[]  reg_p0_g_kp , fraction
	_phy_rtd_part_outl(0x1800db04, 17, 14, DPHY_Para_tmds_1[tmds_timing].r_kp_6_1); //[]  reg_p0_r_kp, integer
	_phy_rtd_part_outl(0x1800db04, 13, 12, 0x0); //[]  reg_p0_r_kp , fraction
	_phy_rtd_part_outl(0x1800db04, 23, 20, DPHY_Para_tmds_1[tmds_timing].ck_kp_8_1); //[]  reg_p0_ck_kp, integer
	_phy_rtd_part_outl(0x1800db04, 19, 18, 0x0); //[]  reg_p0_ck_kp , fraction
	_phy_rtd_part_outl(0x1800db08, 5, 0, DPHY_Para_tmds_1[tmds_timing].b_bbw_kp_10_1); //[]  reg_p0_b_bbw_kp
	_phy_rtd_part_outl(0x1800db08, 11, 6, DPHY_Para_tmds_1[tmds_timing].g_bbw_kp_11_1); //[]  reg_p0_g_bbw_kp
	_phy_rtd_part_outl(0x1800db08, 17, 12, DPHY_Para_tmds_1[tmds_timing].r_bbw_kp_12_1); //[]  reg_p0_r_bbw_kp
	_phy_rtd_part_outl(0x1800db08, 23, 18, DPHY_Para_tmds_1[tmds_timing].ck_bbw_kp_13_1); //[]  reg_p0_ck_bbw_kp
	_phy_rtd_part_outl(0x1800db0c, 2, 0, DPHY_Para_tmds_1[tmds_timing].b_ki_14_1); //[]  reg_p0_b_ki
	_phy_rtd_part_outl(0x1800db0c, 5, 3, DPHY_Para_tmds_1[tmds_timing].g_ki_15_1); //[]  reg_p0_g_ki
	_phy_rtd_part_outl(0x1800db0c, 8, 6, DPHY_Para_tmds_1[tmds_timing].r_ki_16_1); //[]  reg_p0_r_ki
	_phy_rtd_part_outl(0x1800db0c, 11, 9, DPHY_Para_tmds_1[tmds_timing].ck_ki_17_1); //[]  reg_p0_ck_ki
	_phy_rtd_part_outl(0x1800db0c, 18, 16, 0x0); //[]  reg_p0_b_bbw_ki
	_phy_rtd_part_outl(0x1800db0c, 21, 19, 0x0); //[]  reg_p0_g_bbw_ki
	_phy_rtd_part_outl(0x1800db0c, 24, 22, 0x0); //[]  reg_p0_r_bbw_ki
	_phy_rtd_part_outl(0x1800db0c, 27, 25, 0x0); //[]  reg_p0_ck_bbw_ki
	_phy_rtd_part_outl(0x1800db0c, 12, 12, DPHY_Para_tmds_1[tmds_timing].b_st_mode_22_1); //[]  reg_p0_b_st_mode
	_phy_rtd_part_outl(0x1800db0c, 13, 13, DPHY_Para_tmds_1[tmds_timing].g_st_mode_23_1); //[]  reg_p0_g_st_mode
	_phy_rtd_part_outl(0x1800db0c, 14, 14, DPHY_Para_tmds_1[tmds_timing].r_st_mode_24_1); //[]  reg_p0_r_st_mode
	_phy_rtd_part_outl(0x1800db0c, 15, 15, DPHY_Para_tmds_1[tmds_timing].ck_st_mode_25_1); //[]  reg_p0_ck_st_mode
	_phy_rtd_part_outl(0x1800db0c, 28, 28, 0x0); //[]  reg_p0_b_bbw_st_mode
	_phy_rtd_part_outl(0x1800db0c, 29, 29, 0x0); //[]  reg_p0_g_bbw_st_mode
	_phy_rtd_part_outl(0x1800db0c, 30, 30, 0x0); //[]  reg_p0_r_bbw_st_mode
	_phy_rtd_part_outl(0x1800db0c, 31, 31, 0x0); //[]  reg_p0_ck_bbw_st_mode
	_phy_rtd_part_outl(0x1800db00, 17, 16, DPHY_Para_tmds_1[tmds_timing].b_rate_sel_30_1); //[DPHY para]  reg_p0_b_rate_sel
	_phy_rtd_part_outl(0x1800db00, 19, 18, DPHY_Para_tmds_1[tmds_timing].g_rate_sel_31_1); //[DPHY para]  reg_p0_g_rate_sel
	_phy_rtd_part_outl(0x1800db00, 21, 20, DPHY_Para_tmds_1[tmds_timing].r_rate_sel_32_1); //[DPHY para]  reg_p0_r_rate_sel
	_phy_rtd_part_outl(0x1800db88, 1, 1, DPHY_Para_tmds_1[tmds_timing].b_clk_div2_en_33_1); //[DPHY para]  reg_p0_b_clk_div2_en
	_phy_rtd_part_outl(0x1800db88, 9, 9, DPHY_Para_tmds_1[tmds_timing].g_clk_div2_en_34_1); //[DPHY para]  reg_p0_g_clk_div2_en
	_phy_rtd_part_outl(0x1800db88, 17, 17, DPHY_Para_tmds_1[tmds_timing].r_clk_div2_en_35_1); //[DPHY para]  reg_p0_r_clk_div2_en
	_phy_rtd_part_outl(0x1800db8c, 25, 24, 0x0); //[DPHY para]  reg_p0_b_format_sel
	_phy_rtd_part_outl(0x1800db8c, 27, 26, 0x0); //[DPHY para]  reg_p0_g_format_sel
	_phy_rtd_part_outl(0x1800db8c, 29, 28, 0x0); //[DPHY para]  reg_p0_r_format_sel
	_phy_rtd_part_outl(0x1800db8c, 31, 30, 0x0); //[DPHY para]  reg_p0_ck_format_sel
	_phy_rtd_part_outl(0x1800dc3c, 7, 0, DPHY_Para_tmds_1[tmds_timing].b_acdr_pll_config_1_40_1); //[]  reg_p0_b_acdr_pll_config_1
	_phy_rtd_part_outl(0x1800dc3c, 15, 8, DPHY_Para_tmds_1[tmds_timing].b_acdr_pll_config_2_41_1); //[]  reg_p0_b_acdr_pll_config_2
	_phy_rtd_part_outl(0x1800dc3c, 23, 16, DPHY_Para_tmds_1[tmds_timing].b_acdr_pll_config_3_42_1); //[]  reg_p0_b_acdr_pll_config_3
	_phy_rtd_part_outl(0x1800dc3c, 31, 24, 0x00); //[]  reg_p0_b_acdr_pll_config_4
	_phy_rtd_part_outl(0x1800dc40, 7, 0, DPHY_Para_tmds_1[tmds_timing].b_acdr_cdr_config_1_44_1); //[]  reg_p0_b_acdr_cdr_config_1
	_phy_rtd_part_outl(0x1800dc40, 15, 8, DPHY_Para_tmds_1[tmds_timing].b_acdr_cdr_config_2_45_1); //[]  reg_p0_b_acdr_cdr_config_2
	_phy_rtd_part_outl(0x1800dc40, 23, 16, DPHY_Para_tmds_1[tmds_timing].b_acdr_cdr_config_3_46_1); //[]  reg_p0_b_acdr_cdr_config_3
	_phy_rtd_part_outl(0x1800dc40, 31, 24, 0x00); //[]  reg_p0_b_acdr_cdr_config_4
	_phy_rtd_part_outl(0x1800dc44, 7, 0, DPHY_Para_tmds_1[tmds_timing].b_acdr_manual_config_1_48_1); //[]  reg_p0_b_acdr_manual_config_1
	_phy_rtd_part_outl(0x1800dc44, 15, 8, DPHY_Para_tmds_1[tmds_timing].b_acdr_manual_config_2_49_1); //[]  reg_p0_b_acdr_manual_config_2
	_phy_rtd_part_outl(0x1800dc44, 23, 16, DPHY_Para_tmds_1[tmds_timing].b_acdr_manual_config_3_50_1); //[]  reg_p0_b_acdr_manual_config_3
	_phy_rtd_part_outl(0x1800dc44, 31, 24, 0x00); //[]  reg_p0_b_acdr_manual_config_4
	_phy_rtd_part_outl(0x1800dc48, 7, 0, DPHY_Para_tmds_1[tmds_timing].g_acdr_pll_config_1_52_1); //[]  reg_p0_g_acdr_pll_config_1
	_phy_rtd_part_outl(0x1800dc48, 15, 8, DPHY_Para_tmds_1[tmds_timing].g_acdr_pll_config_2_53_1); //[]  reg_p0_g_acdr_pll_config_2
	_phy_rtd_part_outl(0x1800dc48, 23, 16, DPHY_Para_tmds_1[tmds_timing].g_acdr_pll_config_3_54_1); //[]  reg_p0_g_acdr_pll_config_3
	_phy_rtd_part_outl(0x1800dc48, 31, 24, 0x00); //[]  reg_p0_g_acdr_pll_config_4
	_phy_rtd_part_outl(0x1800dc4c, 7, 0, DPHY_Para_tmds_1[tmds_timing].g_acdr_cdr_config_1_56_1); //[]  reg_p0_g_acdr_cdr_config_1
	_phy_rtd_part_outl(0x1800dc4c, 15, 8, DPHY_Para_tmds_1[tmds_timing].g_acdr_cdr_config_2_57_1); //[]  reg_p0_g_acdr_cdr_config_2
	_phy_rtd_part_outl(0x1800dc4c, 23, 16, DPHY_Para_tmds_1[tmds_timing].g_acdr_cdr_config_3_58_1); //[]  reg_p0_g_acdr_cdr_config_3
	_phy_rtd_part_outl(0x1800dc4c, 31, 24, 0x00); //[]  reg_p0_g_acdr_cdr_config_4
	_phy_rtd_part_outl(0x1800dc50, 7, 0, DPHY_Para_tmds_1[tmds_timing].g_acdr_manual_config_1_60_1); //[]  reg_p0_g_acdr_manual_config_1
	_phy_rtd_part_outl(0x1800dc50, 15, 8, DPHY_Para_tmds_1[tmds_timing].g_acdr_manual_config_2_61_1); //[]  reg_p0_g_acdr_manual_config_2
	_phy_rtd_part_outl(0x1800dc50, 23, 16, DPHY_Para_tmds_1[tmds_timing].g_acdr_manual_config_3_62_1); //[]  reg_p0_g_acdr_manual_config_3
	_phy_rtd_part_outl(0x1800dc50, 31, 24, 0x00); //[]  reg_p0_g_acdr_manual_config_4
	_phy_rtd_part_outl(0x1800dc54, 7, 0, DPHY_Para_tmds_1[tmds_timing].r_acdr_pll_config_1_64_1); //[]  reg_p0_r_acdr_pll_config_1
	_phy_rtd_part_outl(0x1800dc54, 15, 8, DPHY_Para_tmds_1[tmds_timing].r_acdr_pll_config_2_65_1); //[]  reg_p0_r_acdr_pll_config_2
	_phy_rtd_part_outl(0x1800dc54, 23, 16, DPHY_Para_tmds_1[tmds_timing].r_acdr_pll_config_3_66_1); //[]  reg_p0_r_acdr_pll_config_3
	_phy_rtd_part_outl(0x1800dc54, 31, 24, 0x00); //[]  reg_p0_r_acdr_pll_config_4
	_phy_rtd_part_outl(0x1800dc58, 7, 0, DPHY_Para_tmds_1[tmds_timing].r_acdr_cdr_config_1_68_1); //[]  reg_p0_r_acdr_cdr_config_1
	_phy_rtd_part_outl(0x1800dc58, 15, 8, DPHY_Para_tmds_1[tmds_timing].r_acdr_cdr_config_2_69_1); //[]  reg_p0_r_acdr_cdr_config_2
	_phy_rtd_part_outl(0x1800dc58, 23, 16, DPHY_Para_tmds_1[tmds_timing].r_acdr_cdr_config_3_70_1); //[]  reg_p0_r_acdr_cdr_config_3
	_phy_rtd_part_outl(0x1800dc58, 31, 24, 0x00); //[]  reg_p0_r_acdr_cdr_config_4
	_phy_rtd_part_outl(0x1800dc5c, 7, 0, DPHY_Para_tmds_1[tmds_timing].r_acdr_manual_config_1_72_1); //[]  reg_p0_r_acdr_manual_config_1
	_phy_rtd_part_outl(0x1800dc5c, 15, 8, DPHY_Para_tmds_1[tmds_timing].r_acdr_manual_config_2_73_1); //[]  reg_p0_r_acdr_manual_config_2
	_phy_rtd_part_outl(0x1800dc5c, 23, 16, DPHY_Para_tmds_1[tmds_timing].r_acdr_manual_config_3_74_1); //[]  reg_p0_r_acdr_manual_config_3
	_phy_rtd_part_outl(0x1800dc5c, 31, 24, 0x00); //[]  reg_p0_r_acdr_manual_config_4
	_phy_rtd_part_outl(0x1800dc60, 7, 0, DPHY_Para_tmds_1[tmds_timing].ck_acdr_pll_config_1_76_1); //[]  reg_p0_ck_acdr_pll_config_1
	_phy_rtd_part_outl(0x1800dc60, 15, 8, DPHY_Para_tmds_1[tmds_timing].ck_acdr_pll_config_2_77_1); //[]  reg_p0_ck_acdr_pll_config_2
	_phy_rtd_part_outl(0x1800dc60, 23, 16, DPHY_Para_tmds_1[tmds_timing].ck_acdr_pll_config_3_78_1); //[]  reg_p0_ck_acdr_pll_config_3
	_phy_rtd_part_outl(0x1800dc60, 31, 24, 0x00); //[]  reg_p0_ck_acdr_pll_config_4
	_phy_rtd_part_outl(0x1800dc64, 7, 0, DPHY_Para_tmds_1[tmds_timing].ck_acdr_cdr_config_1_80_1); //[]  reg_p0_ck_acdr_cdr_config_1
	_phy_rtd_part_outl(0x1800dc64, 15, 8, DPHY_Para_tmds_1[tmds_timing].ck_acdr_cdr_config_2_81_1); //[]  reg_p0_ck_acdr_cdr_config_2
	_phy_rtd_part_outl(0x1800dc64, 23, 16, DPHY_Para_tmds_1[tmds_timing].ck_acdr_cdr_config_3_82_1); //[]  reg_p0_ck_acdr_cdr_config_3
	_phy_rtd_part_outl(0x1800dc64, 31, 24, 0x00); //[]  reg_p0_ck_acdr_cdr_config_4
	_phy_rtd_part_outl(0x1800dc68, 7, 0, DPHY_Para_tmds_1[tmds_timing].ck_acdr_manual_config_1_84_1); //[]  reg_p0_ck_acdr_manual_config_1
	_phy_rtd_part_outl(0x1800dc68, 15, 8, DPHY_Para_tmds_1[tmds_timing].ck_acdr_manual_config_2_85_1); //[]  reg_p0_ck_acdr_manual_config_2
	_phy_rtd_part_outl(0x1800dc68, 23, 16, DPHY_Para_tmds_1[tmds_timing].ck_acdr_manual_config_3_86_1); //[]  reg_p0_ck_acdr_manual_config_3
	_phy_rtd_part_outl(0x1800dc68, 31, 24, 0x00); //[]  reg_p0_ck_acdr_manual_config_4
}

void APHY_Fix_tmds_1_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	//APHY fix start
	//_phy_rtd_part_outl(0x1800da04, 4, 0, 0x08); //[]  REG_Z0_ADJR_0  set z0 from otp value in lib_hdmi21_z0_calibration
	_phy_rtd_part_outl(0x1800da04, 5, 5, 0x01); //[]  REG_P0_Z0_Z0POW_FIX
	_phy_rtd_part_outl(0x1800da10, 25, 24, 0x03); //[]  0x3 Strong level	
	_phy_rtd_part_outl(0x1800da10, 28, 28, 0x01); //[]  REG_P0_Z0_Z0POW_CK
	_phy_rtd_part_outl(0x1800da10, 29, 29, 0x01); //[]  REG_P0_Z0_Z0POW_B
	_phy_rtd_part_outl(0x1800da10, 30, 30, 0x01); //[]  REG_P0_Z0_Z0POW_G
	_phy_rtd_part_outl(0x1800da10, 31, 31, 0x01); //[]  REG_P0_Z0_Z0POW_R
	_phy_rtd_part_outl(0x1800da04, 6, 6, 0x0); //[]  Z0_P_OFF
	_phy_rtd_part_outl(0x1800da04, 7, 7, 0x0); //[]  Z0_N_OFF
	_phy_rtd_part_outl(0x1800da04, 9, 9, 0x0); //[]  REG_Z0_FT_PN_SHORT_EN
	_phy_rtd_part_outl(0x1800da10, 0, 0, 0x0); //[]  TOP_IN_1, FAST_SW_SEL
	_phy_rtd_part_outl(0x1800da10, 1, 1, 0x0); //[]  TOP_IN_1, DIV_EN
	_phy_rtd_part_outl(0x1800da10, 4, 2, 0x0); //[]  TOP_IN_1, DIV_SEL
	_phy_rtd_part_outl(0x1800da10, 5, 5, 0x0); //[]  TOP_IN_1, V_STROBE_EN
	_phy_rtd_part_outl(0x1800da10, 6, 6, 0x1); //[]  TOP_IN_1, CK_TX to B/G/R EN
	_phy_rtd_part_outl(0x1800da10, 7, 7, 0x0); //[]  TOP_IN_1, dummy
	_phy_rtd_part_outl(0x1800da10, 11, 8, 0x0); //[] TOP_IN_2, test signal sel
	_phy_rtd_part_outl(0x1800da10, 13, 12, 0x0); //[] TOP_IN_2, test mode sel
	_phy_rtd_part_outl(0x1800da10, 14, 14, 0x1); //[] TOP_IN_2, CK_TX EN
	_phy_rtd_part_outl(0x1800da10, 15, 15, 0x0); //[] TOP_IN_2, CK_MD SEL
	_phy_rtd_part_outl(0x1800da10, 23, 16, 0x1); //[]  TOP_IN_3, dummy
	_phy_rtd_part_outl(0x1800da14, 7, 0, 0x10); //[]  REG_TOP_IN_5 dummy
	_phy_rtd_part_outl(0x1800da18, 0, 0, 0x1); //[APHY fix]  REG_HDMIRX_BIAS_EN bandgap reference power
	_phy_rtd_part_outl(0x1800da18, 10, 8, 0x0); //[APHY fix]  REG_BG_RBGLOOP2 dummy
	_phy_rtd_part_outl(0x1800da18, 13, 11, 0x0); //[APHY fix]  REG_BG_RBG dummy
	_phy_rtd_part_outl(0x1800da18, 15, 14, 0x0); //[APHY fix]  REG_BG_RBG2 dummy
	_phy_rtd_part_outl(0x1800da18, 16, 16, 0x0); //[APHY fix]  REG_BG_POW dummy
	_phy_rtd_part_outl(0x1800da18, 17, 17, 0x0); //[APHY fix]  REG_BG_ENVBGUP
	_phy_rtd_part_outl(0x1800da20, 7, 0, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da20, 15, 8, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da20, 23, 16, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da20, 31, 24, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da24, 7, 0, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da24, 15, 8, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da24, 23, 16, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da24, 31, 24, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da48, 22, 20, 4); //[DPHY fix] REG_P0_ACDR_CK_7[6:4] (VCO SEL_BAND_V2I)
	_phy_rtd_part_outl(0x1800da68, 22, 20, 4); //[DPHY fix] REG_P0_ACDR_CK_7[6:4] (VCO SEL_BAND_V2I)
	_phy_rtd_part_outl(0x1800da88, 22, 20, 4); //[DPHY fix] REG_P0_ACDR_CK_7[6:4] (VCO SEL_BAND_V2I)
	_phy_rtd_part_outl(0x1800daa8, 22, 20, 4); //[DPHY fix] REG_P0_ACDR_CK_7[6:4] (VCO SEL_BAND_V2I)
	_phy_rtd_part_outl(0x1800da30, 0, 0, 0x0); //[APHY fix]  CK-Lane input off EN
	_phy_rtd_part_outl(0x1800da30, 1, 1, 0x0); //[APHY fix]  CK-Lane INPOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da30, 2, 2, 0x0); //[APHY fix]  CK-Lane INNOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da30, 3, 3, 0x1); //[APHY fix]  CK-Lane POW_AC_COUPLE
	_phy_rtd_part_outl(0x1800da30, 5, 4, 0x0); //[APHY fix]  CK-Lane RXVCM_SEL[1:0]
	_phy_rtd_part_outl(0x1800da34, 21, 21, 0x1); //[APHY fix]   	CK-Lane RS_CAL_EN
	_phy_rtd_part_outl(0x1800da34, 25, 25, 0x1); //[APHY fix]   	CK-Lane POW_DATALANE_BIAS
	_phy_rtd_part_outl(0x1800da34, 26, 26, 0x0); //[APHY fix]   	CK-Lane REG_FORCE_STARTUP(Const-GM)
	_phy_rtd_part_outl(0x1800da34, 27, 27, 0x0); //[APHY fix]   	CK-Lane REG_POWB_STARTUP
	_phy_rtd_part_outl(0x1800da38, 8, 8, 0x0); //[APHY fix]  		CK-Lane POW_DFE
	_phy_rtd_part_outl(0x1800da38, 14, 12, 0x6); //[APHY fix] 		CK-Lane DFE_SUMAMP_ ISEL
	_phy_rtd_part_outl(0x1800da38, 15, 15, 0x0); //[APHY fix] 		CK-Lane DFE_SUMAMP_DCGAIN_MAX
	_phy_rtd_part_outl(0x1800da38, 19, 18, 0x0); //[APHY fix] 		CK-Lane Dummy
	_phy_rtd_part_outl(0x1800da38, 20, 20, 0x0); //[APHY fix] 		CK-Lane DFE CKI_DELAY_EN
	_phy_rtd_part_outl(0x1800da38, 21, 21, 0x0); //[APHY fix] 		CK-Lane DFE CKIB_DELAY_EN
	_phy_rtd_part_outl(0x1800da38, 22, 22, 0x0); //[APHY fix] 		CK-Lane DFE CKQ_DELAY_EN
	_phy_rtd_part_outl(0x1800da38, 23, 23, 0x0); //[APHY fix] 		CK-Lane DFE CKQB_DELAY_EN
	_phy_rtd_part_outl(0x1800da38, 24, 24, 0x0); //[APHY fix] 		CK-Lane EN_EYE_MNT
	_phy_rtd_part_outl(0x1800da38, 25, 25, 0x0); //[APHY fix] 		CK-Lane DEMUX input clock phase select:(for eye mnt)
	_phy_rtd_part_outl(0x1800da38, 26, 26, 0x1); //[APHY fix] 		CK-Lane VTH+OFFSET select. 0:VTH, 1:VTH+OFFSET
	_phy_rtd_part_outl(0x1800da38, 27, 27, 0x0); //[APHY fix] 		CK-Lane DA_EG_VOS_PULLLOW(koffset disable)
	_phy_rtd_part_outl(0x1800da38, 31, 29, 0x0); //[APHY fix] 			CK-Lane dummy
	_phy_rtd_part_outl(0x1800da3c, 2, 0, 0x0); //[APHY fix]  	CK-Lane DFE_TAP_DELAY
	_phy_rtd_part_outl(0x1800da3c, 8, 8, 0x0); //[APHY fix]   	CK-Lane rstb of dtata-lane test div8
	_phy_rtd_part_outl(0x1800da3c, 16, 16, 0x0); //[APHY fix]  		CK-Lane TRANSITION CNT enable.
	_phy_rtd_part_outl(0x1800da3c, 17, 17, 0x0); //[APHY fix]  		CK-Lane QCG1 injection-locked EN
	_phy_rtd_part_outl(0x1800da3c, 18, 18, 0x0); //[APHY fix]  		CK-Lane QCG2_EN
	_phy_rtd_part_outl(0x1800da3c, 19, 19, 0x0); //[APHY fix]  		CK-Lane QCG2 injection-locked EN
	_phy_rtd_part_outl(0x1800da3c, 20, 20, 0x0); //[APHY fix]  		CK-Lane QCG FLD divider(/8) rstb
	_phy_rtd_part_outl(0x1800da3c, 21, 21, 0x0); //[APHY fix]  		CK-Lane QCG FLD SEL
	_phy_rtd_part_outl(0x1800da3c, 22, 22, 0x1); //[APHY fix]  		CK-Lane CK_FLD SEL.
	_phy_rtd_part_outl(0x1800da3c, 23, 23, 0x1); //[APHY fix]  		CK-Lane CK-lane clk to B/G/R EN.
	_phy_rtd_part_outl(0x1800da40, 8, 8, 0x1); //[APHY fix]  CK-Lane PI_EN
	_phy_rtd_part_outl(0x1800da40, 12, 12, 0x0); //[APHY fix]  CK-Lane QCG1 Cload contrl SEL
	_phy_rtd_part_outl(0x1800da40, 13, 13, 0x0); //[APHY fix]  CK-Lane QCG2 Cload contrl SEL
	_phy_rtd_part_outl(0x1800da40, 15, 15, 0x0); //[APHY fix]  CK-Lane BIAS_PI_CUR_SEL
	_phy_rtd_part_outl(0x1800da40, 21, 21, 0x0); //[APHY fix]  CK-Lane PI_EYE_EN
	_phy_rtd_part_outl(0x1800da40, 23, 23, 0x0); //[APHY fix]  CK-Lane KI SEL
	_phy_rtd_part_outl(0x1800da40, 25, 25, 0x0); //[APHY fix]  CK-Lane FKP_RSTB_SEL
	_phy_rtd_part_outl(0x1800da40, 27, 26, 0x0); //[APHY fix]  CK-Lane KI_CK_SEL
	_phy_rtd_part_outl(0x1800da40, 29, 29, 0x0); //[APHY fix]  CK-Lane ST_CODE SEL
	_phy_rtd_part_outl(0x1800da40, 30, 30, 0x0); //[APHY fix]  CK-Lane QCG Ckin SEL.
	_phy_rtd_part_outl(0x1800da44, 3, 2, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_DIV_FLD[1:0]
	_phy_rtd_part_outl(0x1800da44, 5, 4, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_DIV_REF[1:0]
	_phy_rtd_part_outl(0x1800da44, 7, 6, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_DIV_PLL[1:0]
	_phy_rtd_part_outl(0x1800da44, 9, 8, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_DIV_TRAINING[1:0]
	_phy_rtd_part_outl(0x1800da44, 12, 12, 0x0); //[APHY fix]  CK-Lane dummy
	_phy_rtd_part_outl(0x1800da44, 13, 13, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_FLD_0CKFB_1CKREF
	_phy_rtd_part_outl(0x1800da44, 15, 14, 0x0); //[APHY fix]  CK-Lane no use
	_phy_rtd_part_outl(0x1800da44, 18, 18, 0x1); //[APHY fix]  CK-Lane ACDR_RSTB_DIV_PLL ... removed
	_phy_rtd_part_outl(0x1800da44, 24, 24, 0x0); //[APHY fix]  CK-Lane ACDR_EN_UPDN_PULSE_FILTER
	_phy_rtd_part_outl(0x1800da44, 25, 25, 0x1); //[APHY fix]  CK-Lane ACDR_RSTB_UPDN ... dummy
	_phy_rtd_part_outl(0x1800da44, 27, 26, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_UPDN_WIDTH[1:0]
	_phy_rtd_part_outl(0x1800da44, 28, 28, 0x1); //[APHY fix]  CK-Lane ACDR_POW_LPF_IDAC
	_phy_rtd_part_outl(0x1800da44, 31, 29, 0x2); //[APHY fix]  CK-Lane ACDR_SEL_LPF_IDAC[2:0]
	_phy_rtd_part_outl(0x1800da48, 0, 0, APHY_Fix_tmds_1[tmds_timing].ACDR_CK_5_93_1); //[APHY fix]  CK-Lane ACDR_POW_CP
	_phy_rtd_part_outl(0x1800da48, 2, 2, 0x0); //[APHY fix]  CK-Lane ACDR_POW_IDN_BBPD
	_phy_rtd_part_outl(0x1800da48, 3, 3, 0x0); //[APHY fix]  CK-Lane no use
	_phy_rtd_part_outl(0x1800da48, 6, 4, 0x7); //[APHY fix]  CK-Lane ACDR_SEL_TIE_IDN_BBPD
	_phy_rtd_part_outl(0x1800da48, 7, 7, 0x1); //[APHY fix]  CK-Lane ACDR_POW_IBIAS_IDN_HV
	_phy_rtd_part_outl(0x1800da48, 8, 8, 0x1); //[APHY fix]  CK-Lane ACDR_POW_VCO
	_phy_rtd_part_outl(0x1800da48, 9, 9, 0x1); //[APHY fix]  CK-Lane ACDR_POW_VCO_VDAC
	_phy_rtd_part_outl(0x1800da48, 11, 10, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_V15_VDAC
	_phy_rtd_part_outl(0x1800da48, 14, 14, 0x0); //[APHY fix]  CK-Lane no use
	_phy_rtd_part_outl(0x1800da48, 15, 15, 0x0); //[APHY fix]  CK-Lane no use
	_phy_rtd_part_outl(0x1800da48, 16, 16, 0x0); //[APHY fix]  CK-Lane ACDR_POW_TEST_MODE
	_phy_rtd_part_outl(0x1800da48, 18, 17, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_TEST_MODE
	_phy_rtd_part_outl(0x1800da48, 31, 31, 0x0); //[APHY fix]  CK-Lane no use
	_phy_rtd_part_outl(0x1800da4c, 15, 13, 0x1); //[APHY fix]  CK-Lane no use
	_phy_rtd_part_outl(0x1800da4c, 19, 16, 0x0); //[APHY fix]  CK-Lane LE1_ISEL_IN_2 -> dummy
	_phy_rtd_part_outl(0x1800da4c, 31, 25, 0x44); //[APHY fix]  CK-Lane no use for LEQ
	_phy_rtd_part_outl(0x1800da50, 0, 0, 0x0); //[APHY fix]  B-Lane input off EN
	_phy_rtd_part_outl(0x1800da70, 0, 0, 0x0); //[APHY fix]  B-Lane input off EN
	_phy_rtd_part_outl(0x1800da90, 0, 0, 0x0); //[APHY fix]  B-Lane input off EN
	_phy_rtd_part_outl(0x1800da50, 1, 1, 0x0); //[APHY fix]  		B-Lane INPOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da70, 1, 1, 0x0); //[APHY fix]  		B-Lane INPOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da90, 1, 1, 0x0); //[APHY fix]  		B-Lane INPOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da50, 2, 2, 0x0); //[APHY fix]  		B-Lane INNOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da70, 2, 2, 0x0); //[APHY fix]  		B-Lane INNOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da90, 2, 2, 0x0); //[APHY fix]  		B-Lane INNOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da50, 3, 3, 0x1); //[APHY fix]  		B-Lane POW_AC_COUPLE
	_phy_rtd_part_outl(0x1800da70, 3, 3, 0x1); //[APHY fix]  		B-Lane POW_AC_COUPLE
	_phy_rtd_part_outl(0x1800da90, 3, 3, 0x1); //[APHY fix]  		B-Lane POW_AC_COUPLE
	_phy_rtd_part_outl(0x1800da50, 5, 4, 0x1); //[APHY fix]  		B-Lane RXVCM_SEL[1:0]
	_phy_rtd_part_outl(0x1800da70, 5, 4, 0x1); //[APHY fix]  		B-Lane RXVCM_SEL[1:0]
	_phy_rtd_part_outl(0x1800da90, 5, 4, 0x1); //[APHY fix]  		B-Lane RXVCM_SEL[1:0]
	_phy_rtd_part_outl(0x1800da54, 21, 21, 0x1); //[APHY fix]   		B-Lane RS_CAL_EN
	_phy_rtd_part_outl(0x1800da74, 21, 21, 0x1); //[APHY fix]   		B-Lane RS_CAL_EN
	_phy_rtd_part_outl(0x1800da94, 21, 21, 0x1); //[APHY fix]   		B-Lane RS_CAL_EN
	_phy_rtd_part_outl(0x1800da54, 25, 25, 0x1); //[APHY fix]   		B-Lane POW_DATALANE_BIAS
	_phy_rtd_part_outl(0x1800da74, 25, 25, 0x1); //[APHY fix]   		B-Lane POW_DATALANE_BIAS
	_phy_rtd_part_outl(0x1800da94, 25, 25, 0x1); //[APHY fix]   		B-Lane POW_DATALANE_BIAS
	_phy_rtd_part_outl(0x1800da54, 26, 26, 0x0); //[APHY fix]   		B-Lane REG_FORCE_STARTUP(Const-GM)
	_phy_rtd_part_outl(0x1800da74, 26, 26, 0x0); //[APHY fix]   		B-Lane REG_FORCE_STARTUP(Const-GM)
	_phy_rtd_part_outl(0x1800da94, 26, 26, 0x0); //[APHY fix]   		B-Lane REG_FORCE_STARTUP(Const-GM)
	_phy_rtd_part_outl(0x1800da54, 27, 27, 0x0); //[APHY fix]   		B-Lane REG_POWB_STARTUP
	_phy_rtd_part_outl(0x1800da74, 27, 27, 0x0); //[APHY fix]   		B-Lane REG_POWB_STARTUP
	_phy_rtd_part_outl(0x1800da94, 27, 27, 0x0); //[APHY fix]   		B-Lane REG_POWB_STARTUP
	_phy_rtd_part_outl(0x1800da58, 8, 8, 0x1); //[APHY fix]  		B-Lane POW_DFE
	_phy_rtd_part_outl(0x1800da78, 8, 8, 0x1); //[APHY fix]  		B-Lane POW_DFE
	_phy_rtd_part_outl(0x1800da98, 8, 8, 0x1); //[APHY fix]  		B-Lane POW_DFE
	_phy_rtd_part_outl(0x1800da58, 14, 12, 0x6); //[APHY fix] 		B-Lane DFE_SUMAMP_ ISEL
	_phy_rtd_part_outl(0x1800da78, 14, 12, 0x6); //[APHY fix] 		B-Lane DFE_SUMAMP_ ISEL
	_phy_rtd_part_outl(0x1800da98, 14, 12, 0x6); //[APHY fix] 		B-Lane DFE_SUMAMP_ ISEL
	_phy_rtd_part_outl(0x1800da58, 15, 15, 0x0); //[APHY fix] 		B-Lane DFE_SUMAMP_DCGAIN_MAX
	_phy_rtd_part_outl(0x1800da78, 15, 15, 0x0); //[APHY fix] 		B-Lane DFE_SUMAMP_DCGAIN_MAX
	_phy_rtd_part_outl(0x1800da98, 15, 15, 0x0); //[APHY fix] 		B-Lane DFE_SUMAMP_DCGAIN_MAX
	_phy_rtd_part_outl(0x1800da58, 19, 18, 0x0); //[APHY fix] 		B-Lane Dummy
	_phy_rtd_part_outl(0x1800da78, 19, 18, 0x0); //[APHY fix] 		B-Lane Dummy
	_phy_rtd_part_outl(0x1800da98, 19, 18, 0x0); //[APHY fix] 		B-Lane Dummy
	_phy_rtd_part_outl(0x1800da58, 20, 20, 0x0); //[APHY fix] 		B-Lane DFE CKI_DELAY_EN
	_phy_rtd_part_outl(0x1800da78, 20, 20, 0x0); //[APHY fix] 		B-Lane DFE CKI_DELAY_EN
	_phy_rtd_part_outl(0x1800da98, 20, 20, 0x0); //[APHY fix] 		B-Lane DFE CKI_DELAY_EN
	_phy_rtd_part_outl(0x1800da58, 21, 21, 0x0); //[APHY fix] 		B-Lane DFE CKIB_DELAY_EN
	_phy_rtd_part_outl(0x1800da78, 21, 21, 0x0); //[APHY fix] 		B-Lane DFE CKIB_DELAY_EN
	_phy_rtd_part_outl(0x1800da98, 21, 21, 0x0); //[APHY fix] 		B-Lane DFE CKIB_DELAY_EN
	_phy_rtd_part_outl(0x1800da58, 22, 22, 0x0); //[APHY fix] 		B-Lane DFE CKQ_DELAY_EN
	_phy_rtd_part_outl(0x1800da78, 22, 22, 0x0); //[APHY fix] 		B-Lane DFE CKQ_DELAY_EN
	_phy_rtd_part_outl(0x1800da98, 22, 22, 0x0); //[APHY fix] 		B-Lane DFE CKQ_DELAY_EN
	_phy_rtd_part_outl(0x1800da58, 23, 23, 0x0); //[APHY fix] 		B-Lane DFE CKQB_DELAY_EN
	_phy_rtd_part_outl(0x1800da78, 23, 23, 0x0); //[APHY fix] 		B-Lane DFE CKQB_DELAY_EN
	_phy_rtd_part_outl(0x1800da98, 23, 23, 0x0); //[APHY fix] 		B-Lane DFE CKQB_DELAY_EN
	_phy_rtd_part_outl(0x1800da58, 24, 24, 0x0); //[APHY fix] 		B-Lane EN_EYE_MNT
	_phy_rtd_part_outl(0x1800da78, 24, 24, 0x0); //[APHY fix] 		B-Lane EN_EYE_MNT
	_phy_rtd_part_outl(0x1800da98, 24, 24, 0x0); //[APHY fix] 		B-Lane EN_EYE_MNT
	_phy_rtd_part_outl(0x1800da58, 25, 25, 0x0); //[APHY fix] 		B-Lane DEMUX input clock phase select:(for eye mnt)
	_phy_rtd_part_outl(0x1800da78, 25, 25, 0x0); //[APHY fix] 		B-Lane DEMUX input clock phase select:(for eye mnt)
	_phy_rtd_part_outl(0x1800da98, 25, 25, 0x0); //[APHY fix] 		B-Lane DEMUX input clock phase select:(for eye mnt)
	_phy_rtd_part_outl(0x1800da58, 26, 26, 0x1); //[APHY fix] 		B-Lane VTH+OFFSET select. 0:VTH, 1:VTH+OFFSET
	_phy_rtd_part_outl(0x1800da78, 26, 26, 0x1); //[APHY fix] 		B-Lane VTH+OFFSET select. 0:VTH, 1:VTH+OFFSET
	_phy_rtd_part_outl(0x1800da98, 26, 26, 0x1); //[APHY fix] 		B-Lane VTH+OFFSET select. 0:VTH, 1:VTH+OFFSET
	_phy_rtd_part_outl(0x1800da58, 27, 27, 0x0); //[APHY fix] 		B-Lane DA_EG_VOS_PULLLOW(koffset disable)
	_phy_rtd_part_outl(0x1800da78, 27, 27, 0x0); //[APHY fix] 		B-Lane DA_EG_VOS_PULLLOW(koffset disable)
	_phy_rtd_part_outl(0x1800da98, 27, 27, 0x0); //[APHY fix] 		B-Lane DA_EG_VOS_PULLLOW(koffset disable)
	_phy_rtd_part_outl(0x1800da58, 31, 29, 0x0); //[APHY fix] 			B-Lane dummy
	_phy_rtd_part_outl(0x1800da78, 31, 29, 0x0); //[APHY fix] 			B-Lane dummy
	_phy_rtd_part_outl(0x1800da98, 31, 29, 0x0); //[APHY fix] 			B-Lane dummy
	_phy_rtd_part_outl(0x1800da5c, 2, 0, 0x0); //[APHY fix]  	B-Lane DFE_TAP_DELAY
	_phy_rtd_part_outl(0x1800da7c, 2, 0, 0x0); //[APHY fix]  	B-Lane DFE_TAP_DELAY
	_phy_rtd_part_outl(0x1800da9c, 2, 0, 0x0); //[APHY fix]  	B-Lane DFE_TAP_DELAY
	_phy_rtd_part_outl(0x1800da5c, 8, 8, 0x0); //[APHY fix]   	B-Lane rstb of dtata-lane test div8
	_phy_rtd_part_outl(0x1800da7c, 8, 8, 0x0); //[APHY fix]   	B-Lane rstb of dtata-lane test div8
	_phy_rtd_part_outl(0x1800da9c, 8, 8, 0x0); //[APHY fix]   	B-Lane rstb of dtata-lane test div8
	_phy_rtd_part_outl(0x1800da5c, 16, 16, 0x0); //[APHY fix]  		B-Lane TRANSITION CNT enable.
	_phy_rtd_part_outl(0x1800da7c, 16, 16, 0x0); //[APHY fix]  		B-Lane TRANSITION CNT enable.
	_phy_rtd_part_outl(0x1800da9c, 16, 16, 0x0); //[APHY fix]  		B-Lane TRANSITION CNT enable.
	_phy_rtd_part_outl(0x1800da5c, 17, 17, 0x1); //[APHY fix]  		B-Lane QCG1 injection-locked EN
	_phy_rtd_part_outl(0x1800da7c, 17, 17, 0x1); //[APHY fix]  		B-Lane QCG1 injection-locked EN
	_phy_rtd_part_outl(0x1800da9c, 17, 17, 0x1); //[APHY fix]  		B-Lane QCG1 injection-locked EN
	_phy_rtd_part_outl(0x1800da5c, 18, 18, 0x0); //[APHY fix]  		B-Lane QCG2_EN
	_phy_rtd_part_outl(0x1800da7c, 18, 18, 0x0); //[APHY fix]  		B-Lane QCG2_EN
	_phy_rtd_part_outl(0x1800da9c, 18, 18, 0x0); //[APHY fix]  		B-Lane QCG2_EN
	_phy_rtd_part_outl(0x1800da5c, 19, 19, 0x0); //[APHY fix]  		B-Lane QCG2 injection-locked EN
	_phy_rtd_part_outl(0x1800da7c, 19, 19, 0x0); //[APHY fix]  		B-Lane QCG2 injection-locked EN
	_phy_rtd_part_outl(0x1800da9c, 19, 19, 0x0); //[APHY fix]  		B-Lane QCG2 injection-locked EN
	_phy_rtd_part_outl(0x1800da5c, 20, 20, 0x0); //[APHY fix]  		B-Lane QCG FLD divider(/8) rstb
	_phy_rtd_part_outl(0x1800da7c, 20, 20, 0x0); //[APHY fix]  		B-Lane QCG FLD divider(/8) rstb
	_phy_rtd_part_outl(0x1800da9c, 20, 20, 0x0); //[APHY fix]  		B-Lane QCG FLD divider(/8) rstb
	_phy_rtd_part_outl(0x1800da5c, 21, 21, 0x0); //[APHY fix]  		B-Lane QCG FLD SEL
	_phy_rtd_part_outl(0x1800da7c, 21, 21, 0x0); //[APHY fix]  		B-Lane QCG FLD SEL
	_phy_rtd_part_outl(0x1800da9c, 21, 21, 0x0); //[APHY fix]  		B-Lane QCG FLD SEL
	_phy_rtd_part_outl(0x1800da5c, 22, 22, 0x1); //[APHY fix]  		B-Lane CK_FLD SEL.
	_phy_rtd_part_outl(0x1800da7c, 22, 22, 0x1); //[APHY fix]  		B-Lane CK_FLD SEL.
	_phy_rtd_part_outl(0x1800da9c, 22, 22, 0x1); //[APHY fix]  		B-Lane CK_FLD SEL.
	_phy_rtd_part_outl(0x1800da5c, 23, 23, 0x0); //[APHY fix]  		B-Lane CK-lane clk to B/G/R EN.
	_phy_rtd_part_outl(0x1800da7c, 23, 23, 0x0); //[APHY fix]  		B-Lane CK-lane clk to B/G/R EN.
	_phy_rtd_part_outl(0x1800da9c, 23, 23, 0x0); //[APHY fix]  		B-Lane CK-lane clk to B/G/R EN.
	_phy_rtd_part_outl(0x1800da60, 8, 8, 0x1); //[APHY fix]  B-Lane PI_EN
	_phy_rtd_part_outl(0x1800da80, 8, 8, 0x1); //[APHY fix]  B-Lane PI_EN
	_phy_rtd_part_outl(0x1800daa0, 8, 8, 0x1); //[APHY fix]  B-Lane PI_EN
	_phy_rtd_part_outl(0x1800da60, 12, 12, 0x0); //[APHY fix]  B-Lane QCG1 Cload contrl SEL
	_phy_rtd_part_outl(0x1800da80, 12, 12, 0x0); //[APHY fix]  B-Lane QCG1 Cload contrl SEL
	_phy_rtd_part_outl(0x1800daa0, 12, 12, 0x0); //[APHY fix]  B-Lane QCG1 Cload contrl SEL
	_phy_rtd_part_outl(0x1800da60, 13, 13, 0x0); //[APHY fix]  B-Lane QCG2 Cload contrl SEL
	_phy_rtd_part_outl(0x1800da80, 13, 13, 0x0); //[APHY fix]  B-Lane QCG2 Cload contrl SEL
	_phy_rtd_part_outl(0x1800daa0, 13, 13, 0x0); //[APHY fix]  B-Lane QCG2 Cload contrl SEL
	_phy_rtd_part_outl(0x1800da60, 15, 15, 0x0); //[APHY fix]  B-Lane BIAS_PI_CUR_SEL
	_phy_rtd_part_outl(0x1800da80, 15, 15, 0x0); //[APHY fix]  B-Lane BIAS_PI_CUR_SEL
	_phy_rtd_part_outl(0x1800daa0, 15, 15, 0x0); //[APHY fix]  B-Lane BIAS_PI_CUR_SEL
	_phy_rtd_part_outl(0x1800da60, 21, 21, 0x0); //[APHY fix]  B-Lane PI_EYE_EN
	_phy_rtd_part_outl(0x1800da80, 21, 21, 0x0); //[APHY fix]  B-Lane PI_EYE_EN
	_phy_rtd_part_outl(0x1800daa0, 21, 21, 0x0); //[APHY fix]  B-Lane PI_EYE_EN
	_phy_rtd_part_outl(0x1800da60, 23, 23, 0x0); //[APHY fix]  B-Lane KI SEL
	_phy_rtd_part_outl(0x1800da80, 23, 23, 0x0); //[APHY fix]  B-Lane KI SEL
	_phy_rtd_part_outl(0x1800daa0, 23, 23, 0x0); //[APHY fix]  B-Lane KI SEL
	_phy_rtd_part_outl(0x1800da60, 25, 25, 0x0); //[APHY fix]  B-Lane FKP_RSTB_SEL
	_phy_rtd_part_outl(0x1800da80, 25, 25, 0x0); //[APHY fix]  B-Lane FKP_RSTB_SEL
	_phy_rtd_part_outl(0x1800daa0, 25, 25, 0x0); //[APHY fix]  B-Lane FKP_RSTB_SEL
	_phy_rtd_part_outl(0x1800da60, 27, 26, 0x0); //[APHY fix]  B-Lane KI_CK_SEL
	_phy_rtd_part_outl(0x1800da80, 27, 26, 0x0); //[APHY fix]  B-Lane KI_CK_SEL
	_phy_rtd_part_outl(0x1800daa0, 27, 26, 0x0); //[APHY fix]  B-Lane KI_CK_SEL
	_phy_rtd_part_outl(0x1800da60, 29, 29, 0x0); //[APHY fix]  B-Lane ST_CODE SEL
	_phy_rtd_part_outl(0x1800da80, 29, 29, 0x0); //[APHY fix]  B-Lane ST_CODE SEL
	_phy_rtd_part_outl(0x1800daa0, 29, 29, 0x0); //[APHY fix]  B-Lane ST_CODE SEL
	_phy_rtd_part_outl(0x1800da60, 30, 30, 0x0); //[APHY fix]  B-Lane QCG Ckin SEL.
	_phy_rtd_part_outl(0x1800da80, 30, 30, 0x0); //[APHY fix]  B-Lane QCG Ckin SEL.
	_phy_rtd_part_outl(0x1800daa0, 30, 30, 0x0); //[APHY fix]  B-Lane QCG Ckin SEL.
	_phy_rtd_part_outl(0x1800da64, 3, 2, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_FLD[1:0]
	_phy_rtd_part_outl(0x1800da84, 3, 2, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_FLD[1:0]
	_phy_rtd_part_outl(0x1800daa4, 3, 2, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_FLD[1:0]
	_phy_rtd_part_outl(0x1800da64, 5, 4, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_REF[1:0]
	_phy_rtd_part_outl(0x1800da84, 5, 4, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_REF[1:0]
	_phy_rtd_part_outl(0x1800daa4, 5, 4, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_REF[1:0]
	_phy_rtd_part_outl(0x1800da64, 7, 6, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_PLL[1:0]
	_phy_rtd_part_outl(0x1800da84, 7, 6, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_PLL[1:0]
	_phy_rtd_part_outl(0x1800daa4, 7, 6, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_PLL[1:0]
	_phy_rtd_part_outl(0x1800da64, 9, 8, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_TRAINING[1:0]
	_phy_rtd_part_outl(0x1800da84, 9, 8, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_TRAINING[1:0]
	_phy_rtd_part_outl(0x1800daa4, 9, 8, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_TRAINING[1:0]
	_phy_rtd_part_outl(0x1800da64, 12, 12, 0x0); //[APHY fix]  B-Lane dummy
	_phy_rtd_part_outl(0x1800da84, 12, 12, 0x0); //[APHY fix]  B-Lane dummy
	_phy_rtd_part_outl(0x1800daa4, 12, 12, 0x0); //[APHY fix]  B-Lane dummy
	_phy_rtd_part_outl(0x1800da64, 13, 13, 0x0); //[APHY fix]  B-Lane ACDR_SEL_FLD_0CKFB_1CKREF
	_phy_rtd_part_outl(0x1800da84, 13, 13, 0x0); //[APHY fix]  B-Lane ACDR_SEL_FLD_0CKFB_1CKREF
	_phy_rtd_part_outl(0x1800daa4, 13, 13, 0x0); //[APHY fix]  B-Lane ACDR_SEL_FLD_0CKFB_1CKREF
	_phy_rtd_part_outl(0x1800da64, 15, 14, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da84, 15, 14, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800daa4, 15, 14, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da64, 18, 18, 0x0); //[APHY fix]  B-Lane ACDR_RSTB_DIV_PLL ... removed
	_phy_rtd_part_outl(0x1800da84, 18, 18, 0x0); //[APHY fix]  B-Lane ACDR_RSTB_DIV_PLL ... removed
	_phy_rtd_part_outl(0x1800daa4, 18, 18, 0x0); //[APHY fix]  B-Lane ACDR_RSTB_DIV_PLL ... removed
	_phy_rtd_part_outl(0x1800da64, 24, 24, 0x1); //[APHY fix]  B-Lane ACDR_EN_UPDN_PULSE_FILTER
	_phy_rtd_part_outl(0x1800da84, 24, 24, 0x1); //[APHY fix]  B-Lane ACDR_EN_UPDN_PULSE_FILTER
	_phy_rtd_part_outl(0x1800daa4, 24, 24, 0x1); //[APHY fix]  B-Lane ACDR_EN_UPDN_PULSE_FILTER
	_phy_rtd_part_outl(0x1800da64, 25, 25, 0x1); //[APHY fix]  B-Lane CMU_SEL_CP
	_phy_rtd_part_outl(0x1800da84, 25, 25, 0x1); //[APHY fix]  B-Lane CMU_SEL_CP
	_phy_rtd_part_outl(0x1800daa4, 25, 25, 0x1); //[APHY fix]  B-Lane CMU_SEL_CP
	_phy_rtd_part_outl(0x1800da64, 27, 26, 0x0); //[APHY fix]  B-Lane ACDR_SEL_UPDN_WIDTH[1:0]
	_phy_rtd_part_outl(0x1800da84, 27, 26, 0x0); //[APHY fix]  B-Lane ACDR_SEL_UPDN_WIDTH[1:0]
	_phy_rtd_part_outl(0x1800daa4, 27, 26, 0x0); //[APHY fix]  B-Lane ACDR_SEL_UPDN_WIDTH[1:0]
	_phy_rtd_part_outl(0x1800da64, 28, 28, 0x1); //[APHY fix]  B-Lane ACDR_POW_LPF_IDAC
	_phy_rtd_part_outl(0x1800da84, 28, 28, 0x1); //[APHY fix]  B-Lane ACDR_POW_LPF_IDAC
	_phy_rtd_part_outl(0x1800daa4, 28, 28, 0x1); //[APHY fix]  B-Lane ACDR_POW_LPF_IDAC
	_phy_rtd_part_outl(0x1800da64, 31, 29, 0x2); //[APHY fix]  B-Lane ACDR_SEL_LPF_IDAC[2:0]
	_phy_rtd_part_outl(0x1800da84, 31, 29, 0x2); //[APHY fix]  B-Lane ACDR_SEL_LPF_IDAC[2:0]
	_phy_rtd_part_outl(0x1800daa4, 31, 29, 0x2); //[APHY fix]  B-Lane ACDR_SEL_LPF_IDAC[2:0]
	_phy_rtd_part_outl(0x1800da68, 0, 0, 0x1); //[APHY fix]  B-Lane ACDR_POW_CP
	_phy_rtd_part_outl(0x1800da88, 0, 0, 0x1); //[APHY fix]  B-Lane ACDR_POW_CP
	_phy_rtd_part_outl(0x1800daa8, 0, 0, 0x1); //[APHY fix]  B-Lane ACDR_POW_CP
	_phy_rtd_part_outl(0x1800da68, 2, 2, 0x0); //[APHY fix]  B-Lane ACDR_POW_IDN_BBPD
	_phy_rtd_part_outl(0x1800da88, 2, 2, 0x0); //[APHY fix]  B-Lane ACDR_POW_IDN_BBPD
	_phy_rtd_part_outl(0x1800daa8, 2, 2, 0x0); //[APHY fix]  B-Lane ACDR_POW_IDN_BBPD
	_phy_rtd_part_outl(0x1800da68, 3, 3, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da88, 3, 3, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800daa8, 3, 3, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da68, 6, 4, 0x2); //[APHY fix]  B-Lane ACDR_SEL_TIE_IDN_BBPD
	_phy_rtd_part_outl(0x1800da88, 6, 4, 0x2); //[APHY fix]  B-Lane ACDR_SEL_TIE_IDN_BBPD
	_phy_rtd_part_outl(0x1800daa8, 6, 4, 0x2); //[APHY fix]  B-Lane ACDR_SEL_TIE_IDN_BBPD
	_phy_rtd_part_outl(0x1800da68, 7, 7, 0x1); //[APHY fix]  B-Lane ACDR_POW_IBIAS_IDN_HV
	_phy_rtd_part_outl(0x1800da88, 7, 7, 0x1); //[APHY fix]  B-Lane ACDR_POW_IBIAS_IDN_HV
	_phy_rtd_part_outl(0x1800daa8, 7, 7, 0x1); //[APHY fix]  B-Lane ACDR_POW_IBIAS_IDN_HV
	_phy_rtd_part_outl(0x1800da68, 8, 8, 0x1); //[APHY fix]  B-Lane ACDR_POW_VCO
	_phy_rtd_part_outl(0x1800da88, 8, 8, 0x1); //[APHY fix]  B-Lane ACDR_POW_VCO
	_phy_rtd_part_outl(0x1800daa8, 8, 8, 0x1); //[APHY fix]  B-Lane ACDR_POW_VCO
	_phy_rtd_part_outl(0x1800da68, 9, 9, 0x1); //[APHY fix]  B-Lane ACDR_POW_VCO_VDAC
	_phy_rtd_part_outl(0x1800da88, 9, 9, 0x1); //[APHY fix]  B-Lane ACDR_POW_VCO_VDAC
	_phy_rtd_part_outl(0x1800daa8, 9, 9, 0x1); //[APHY fix]  B-Lane ACDR_POW_VCO_VDAC
	_phy_rtd_part_outl(0x1800da68, 11, 10, 0x0); //[APHY fix]  B-Lane ACDR_SEL_V15_VDAC
	_phy_rtd_part_outl(0x1800da88, 11, 10, 0x0); //[APHY fix]  B-Lane ACDR_SEL_V15_VDAC
	_phy_rtd_part_outl(0x1800daa8, 11, 10, 0x0); //[APHY fix]  B-Lane ACDR_SEL_V15_VDAC
	_phy_rtd_part_outl(0x1800da68, 14, 14, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da88, 14, 14, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800daa8, 14, 14, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da68, 15, 15, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da88, 15, 15, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800daa8, 15, 15, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da68, 16, 16, 0x0); //[APHY fix]  B-Lane ACDR_POW_TEST_MODE
	_phy_rtd_part_outl(0x1800da88, 16, 16, 0x0); //[APHY fix]  B-Lane ACDR_POW_TEST_MODE
	_phy_rtd_part_outl(0x1800daa8, 16, 16, 0x0); //[APHY fix]  B-Lane ACDR_POW_TEST_MODE
	_phy_rtd_part_outl(0x1800da68, 18, 17, 0x0); //[APHY fix]  B-Lane ACDR_SEL_TEST_MODE
	_phy_rtd_part_outl(0x1800da88, 18, 17, 0x0); //[APHY fix]  B-Lane ACDR_SEL_TEST_MODE
	_phy_rtd_part_outl(0x1800daa8, 18, 17, 0x0); //[APHY fix]  B-Lane ACDR_SEL_TEST_MODE
	_phy_rtd_part_outl(0x1800da68, 31, 31, 0x1); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da88, 31, 31, 0x1); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800daa8, 31, 31, 0x1); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da6c, 13, 13, 0x1); //[APHY fix]  B-Lane ckref sel. 0:xtal, 1:tx or train clk
	_phy_rtd_part_outl(0x1800da8c, 13, 13, 0x1); //[APHY fix]  B-Lane ckref sel. 0:xtal, 1:tx or train clk
	_phy_rtd_part_outl(0x1800daac, 13, 13, 0x1); //[APHY fix]  B-Lane ckref sel. 0:xtal, 1:tx or train clk
	_phy_rtd_part_outl(0x1800da6c, 14, 14, 0x1); //[APHY fix]  B-Lane ckref sel. 0:train_clk, 1:tx_clk
	_phy_rtd_part_outl(0x1800da8c, 14, 14, 0x1); //[APHY fix]  B-Lane ckref sel. 0:train_clk, 1:tx_clk
	_phy_rtd_part_outl(0x1800daac, 14, 14, 0x1); //[APHY fix]  B-Lane ckref sel. 0:train_clk, 1:tx_clk
	_phy_rtd_part_outl(0x1800da6c, 15, 15, 0x0); //[APHY fix]  B-Lane ckbk sel. 0:M-DIV, 1:PRESCALER
	_phy_rtd_part_outl(0x1800da8c, 15, 15, 0x0); //[APHY fix]  B-Lane ckbk sel. 0:M-DIV, 1:PRESCALER
	_phy_rtd_part_outl(0x1800daac, 15, 15, 0x0); //[APHY fix]  B-Lane ckbk sel. 0:M-DIV, 1:PRESCALER
	_phy_rtd_part_outl(0x1800da6c, 19, 16, 0x0); //[APHY fix]  B-Lane LE1_ISEL_IN_2 -> dummy
	_phy_rtd_part_outl(0x1800da8c, 19, 16, 0x0); //[APHY fix]  B-Lane LE1_ISEL_IN_2 -> dummy
	_phy_rtd_part_outl(0x1800daac, 19, 16, 0x0); //[APHY fix]  B-Lane LE1_ISEL_IN_2 -> dummy
	_phy_rtd_part_outl(0x1800da6c, 31, 28, 0x00); //[APHY fix]  B-Lane no use for LEQ
	_phy_rtd_part_outl(0x1800da8c, 31, 28, 0x00); //[APHY fix]  B-Lane no use for LEQ
	_phy_rtd_part_outl(0x1800daac, 31, 28, 0x00); //[APHY fix]  B-Lane no use for LEQ
	_phy_rtd_part_outl(0x1800da6c, 25, 25, 0x1); //[APHY fix]  B-Lane WDPOW
	_phy_rtd_part_outl(0x1800da8c, 25, 25, 0x1); //[APHY fix]  B-Lane WDPOW
	_phy_rtd_part_outl(0x1800daac, 25, 25, 0x1); //[APHY fix]  B-Lane WDPOW
	_phy_rtd_part_outl(0x1800da6c, 26, 26, 0x0); //[APHY fix]  B-Lane WDRST
	_phy_rtd_part_outl(0x1800da8c, 26, 26, 0x0); //[APHY fix]  B-Lane WDRST
	_phy_rtd_part_outl(0x1800daac, 26, 26, 0x0); //[APHY fix]  B-Lane WDRST
	_phy_rtd_part_outl(0x1800da6c, 27, 27, 0x0); //[APHY fix]  B-Lane WDSET
	_phy_rtd_part_outl(0x1800da8c, 27, 27, 0x0); //[APHY fix]  B-Lane WDSET
	_phy_rtd_part_outl(0x1800daac, 27, 27, 0x0); //[APHY fix]  B-Lane WDSET
	_phy_rtd_part_outl(0x1800dab4, 0, 0, 0x1); //[APHY fix]  Non-Hysteresis Amp Power
	_phy_rtd_part_outl(0x1800dab4, 3, 1, 0x5); //[APHY fix]  CK Mode hysteresis amp Hysteresis adjustment[2:0]
	_phy_rtd_part_outl(0x1800dab4, 5, 5, 0x0); //[APHY fix]  CK Mode detect source
	_phy_rtd_part_outl(0x1800dab4, 6, 6, 0x0); //[APHY fix]  Hysteresis amp power
	_phy_rtd_part_outl(0x1800dab4, 8, 8, 0x1); //[APHY fix]  CMU_WDPOW
	_phy_rtd_part_outl(0x1800dab4, 9, 9, 0x0); //[APHY fix]  CMU_WDSET
	_phy_rtd_part_outl(0x1800dab4, 10, 10, 0x0); //[APHY fix]  CMU_WDRST
	_phy_rtd_part_outl(0x1800dab4, 11, 11, 0x1); //[APHY fix]  no use
	_phy_rtd_part_outl(0x1800dab4, 12, 12, 0x0); //[APHY fix]  EN_TST_CKBK
	_phy_rtd_part_outl(0x1800dab4, 13, 13, 0x0); //[APHY fix]  EN_TST_CKREF
	_phy_rtd_part_outl(0x1800dab4, 15, 14, 0x0); //[APHY fix]  no use
	_phy_rtd_part_outl(0x1800dab4, 23, 16, 0x00); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dab4, 31, 24, 0x00); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dab8, 7, 0, 0xf4); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dab8, 9, 9, 0x1); //[APHY fix]  no use
	_phy_rtd_part_outl(0x1800dab8, 10, 10, 0x1); //[APHY fix]  CMU_SEL_CS
	_phy_rtd_part_outl(0x1800dab8, 15, 11, 0x09); //[APHY fix]  no use
	_phy_rtd_part_outl(0x1800dab8, 23, 16, 0x06); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dab8, 31, 24, 0x07); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dabc, 7, 0, 0x44); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dabc, 15, 8, 0xef); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dabc, 23, 16, 0x0f); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dabc, 31, 24, 0x00); //[APHY fix]  dummy
}

void APHY_Para_tmds_1_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	// APHY para start
	_phy_rtd_part_outl(0x1800da64, 19, 19, 0x1); //[APHY para]  B-Lane  ACDR_SEL_HS_CLK
	_phy_rtd_part_outl(0x1800da84, 19, 19, 0x1); //[APHY para]  G-Lane  ACDR_SEL_HS_CLK
	_phy_rtd_part_outl(0x1800daa4, 19, 19, 0x1); //[APHY para]  R-Lane  ACDR_SEL_HS_CLK
	_phy_rtd_part_outl(0x1800da44, 19, 19, 0x1); //[APHY para]  CK-Lane  ACDR_SEL_HS_CLK
	_phy_rtd_part_outl(0x1800da48, 1, 1, 0x0); //[APHY para]  CK-Lane ACDR_POW_CP_INTG2_CORE
	_phy_rtd_part_outl(0x1800da68, 1, 1, APHY_Para_tmds_1[tmds_timing].ACDR_B_5_7_1); //[APHY para]  B-Lane ACDR_POW_CP_INTG2_CORE
	_phy_rtd_part_outl(0x1800da88, 1, 1, APHY_Para_tmds_1[tmds_timing].ACDR_G_5_8_1); //[APHY para]  G-Lane ACDR_POW_CP_INTG2_CORE
	_phy_rtd_part_outl(0x1800daa8, 1, 1, APHY_Para_tmds_1[tmds_timing].ACDR_R_5_9_1); //[APHY para]  R-Lane ACDR_POW_CP_INTG2_CORE
	_phy_rtd_part_outl(0x1800da48, 13, 12, APHY_Para_tmds_1[tmds_timing].ACDR_CK_6_10_1); //[APHY para]  CK-Lane ACDR_SEL_BAND_CAP
	_phy_rtd_part_outl(0x1800da68, 13, 12, APHY_Para_tmds_1[tmds_timing].ACDR_B_6_11_1); //[APHY para]  B-Lane ACDR_SEL_BAND_CAP
	_phy_rtd_part_outl(0x1800da88, 13, 12, APHY_Para_tmds_1[tmds_timing].ACDR_G_6_12_1); //[APHY para]  G-Lane ACDR_SEL_BAND_CAP
	_phy_rtd_part_outl(0x1800daa8, 13, 12, APHY_Para_tmds_1[tmds_timing].ACDR_R_6_13_1); //[APHY para]  R-Lane ACDR_SEL_BAND_CAP
	_phy_rtd_part_outl(0x1800da4c, 7, 0, APHY_Para_tmds_1[tmds_timing].ACDR_CK_9_14_1); //[APHY para]  CK-Lane CMU_SEL_M_DIV[7:0]
	_phy_rtd_part_outl(0x1800da4c, 8, 8, 0x0); //[APHY para]  CK-Lane CMU_SEL_M_DIV[8]
	_phy_rtd_part_outl(0x1800da4c, 9, 9, 0x0); //[APHY para]  CK-Lane CMU_SEL_PRE_M_DIV
	_phy_rtd_part_outl(0x1800da4c, 10, 10, APHY_Para_tmds_1[tmds_timing].ACDR_CK_10_17_1); //[APHY para]  CK-Lane CMU_SEL_POST_M_DIV
	_phy_rtd_part_outl(0x1800da4c, 21, 20, APHY_Para_tmds_1[tmds_timing].ACDR_CK_11_18_1); //[APHY para]  CK-Lane RLSEL_LE1_2
	_phy_rtd_part_outl(0x1800da50, 9, 8, APHY_Para_tmds_1[tmds_timing].B_2_19_1); //[APHY para]  		B-Lane RLSEL_LE1
	_phy_rtd_part_outl(0x1800da70, 9, 8, APHY_Para_tmds_1[tmds_timing].B_2_19_1_32); //[APHY para]  		B-Lane RLSEL_LE1
	_phy_rtd_part_outl(0x1800da90, 9, 8, APHY_Para_tmds_1[tmds_timing].B_2_19_1_64); //[APHY para]  		B-Lane RLSEL_LE1
	_phy_rtd_part_outl(0x1800da50, 11, 10, APHY_Para_tmds_1[tmds_timing].B_2_20_1); //[APHY para]  		B-Lane RLSEL_LE2
	_phy_rtd_part_outl(0x1800da70, 11, 10, APHY_Para_tmds_1[tmds_timing].B_2_20_1_32); //[APHY para]  		B-Lane RLSEL_LE2
	_phy_rtd_part_outl(0x1800da90, 11, 10, APHY_Para_tmds_1[tmds_timing].B_2_20_1_64); //[APHY para]  		B-Lane RLSEL_LE2
	_phy_rtd_part_outl(0x1800da50, 13, 12, APHY_Para_tmds_1[tmds_timing].B_2_21_1); //[APHY para]  		B-Lane RLSEL_NC
	_phy_rtd_part_outl(0x1800da70, 13, 12, APHY_Para_tmds_1[tmds_timing].B_2_21_1_32); //[APHY para]  		B-Lane RLSEL_NC
	_phy_rtd_part_outl(0x1800da90, 13, 12, APHY_Para_tmds_1[tmds_timing].B_2_21_1_64); //[APHY para]  		B-Lane RLSEL_NC
	_phy_rtd_part_outl(0x1800da50, 15, 14, APHY_Para_tmds_1[tmds_timing].B_2_22_1); //[APHY para]  		B-Lane RLSEL_TAP0
	_phy_rtd_part_outl(0x1800da70, 15, 14, APHY_Para_tmds_1[tmds_timing].B_2_22_1_32); //[APHY para]  		B-Lane RLSEL_TAP0
	_phy_rtd_part_outl(0x1800da90, 15, 14, APHY_Para_tmds_1[tmds_timing].B_2_22_1_64); //[APHY para]  		B-Lane RLSEL_TAP0
	_phy_rtd_part_outl(0x1800da50, 19, 16, APHY_Para_tmds_1[tmds_timing].B_3_23_1); //[APHY para]  		B-Lane RSSEL_LE1_1
	_phy_rtd_part_outl(0x1800da70, 19, 16, APHY_Para_tmds_1[tmds_timing].B_3_23_1_32); //[APHY para]  		B-Lane RSSEL_LE1_1
	_phy_rtd_part_outl(0x1800da90, 19, 16, APHY_Para_tmds_1[tmds_timing].B_3_23_1_64); //[APHY para]  		B-Lane RSSEL_LE1_1
	_phy_rtd_part_outl(0x1800da50, 23, 20, APHY_Para_tmds_1[tmds_timing].B_3_24_1); //[APHY para]  		B-Lane RSSEL_LE1_2
	_phy_rtd_part_outl(0x1800da70, 23, 20, APHY_Para_tmds_1[tmds_timing].B_3_24_1_32); //[APHY para]  		B-Lane RSSEL_LE1_2
	_phy_rtd_part_outl(0x1800da90, 23, 20, APHY_Para_tmds_1[tmds_timing].B_3_24_1_64); //[APHY para]  		B-Lane RSSEL_LE1_2
	_phy_rtd_part_outl(0x1800da50, 27, 24, APHY_Para_tmds_1[tmds_timing].B_4_25_1); //[APHY para]  		B-Lane RSSEL_LE2
	_phy_rtd_part_outl(0x1800da70, 27, 24, APHY_Para_tmds_1[tmds_timing].B_4_25_1_32); //[APHY para]  		B-Lane RSSEL_LE2
	_phy_rtd_part_outl(0x1800da90, 27, 24, APHY_Para_tmds_1[tmds_timing].B_4_25_1_64); //[APHY para]  		B-Lane RSSEL_LE2
	_phy_rtd_part_outl(0x1800da50, 29, 28, APHY_Para_tmds_1[tmds_timing].B_4_26_1); //[APHY para]  		B-Lane RSSEL_TAP0
	_phy_rtd_part_outl(0x1800da70, 29, 28, APHY_Para_tmds_1[tmds_timing].B_4_26_1_32); //[APHY para]  		B-Lane RSSEL_TAP0
	_phy_rtd_part_outl(0x1800da90, 29, 28, APHY_Para_tmds_1[tmds_timing].B_4_26_1_64); //[APHY para]  		B-Lane RSSEL_TAP0
	_phy_rtd_part_outl(0x1800da50, 31, 30, APHY_Para_tmds_1[tmds_timing].B_4_27_1); //[APHY para]  	B-Lane KOFF_RANGE
	_phy_rtd_part_outl(0x1800da70, 31, 30, APHY_Para_tmds_1[tmds_timing].B_4_27_1_32); //[APHY para]  	B-Lane KOFF_RANGE
	_phy_rtd_part_outl(0x1800da90, 31, 30, APHY_Para_tmds_1[tmds_timing].B_4_27_1_64); //[APHY para]  	B-Lane KOFF_RANGE
	_phy_rtd_part_outl(0x1800da54, 3, 0, APHY_Para_tmds_1[tmds_timing].B_5_28_1); //[APHY para]    	B-Lane LE1_ISEL_IN
	_phy_rtd_part_outl(0x1800da74, 3, 0, APHY_Para_tmds_1[tmds_timing].B_5_28_1_32); //[APHY para]    	B-Lane LE1_ISEL_IN
	_phy_rtd_part_outl(0x1800da94, 3, 0, APHY_Para_tmds_1[tmds_timing].B_5_28_1_64); //[APHY para]    	B-Lane LE1_ISEL_IN
	_phy_rtd_part_outl(0x1800da54, 7, 4, APHY_Para_tmds_1[tmds_timing].B_5_29_1); //[APHY para]    	B-Lane LE2_ISEL_IN
	_phy_rtd_part_outl(0x1800da74, 7, 4, APHY_Para_tmds_1[tmds_timing].B_5_29_1_32); //[APHY para]    	B-Lane LE2_ISEL_IN
	_phy_rtd_part_outl(0x1800da94, 7, 4, APHY_Para_tmds_1[tmds_timing].B_5_29_1_64); //[APHY para]    	B-Lane LE2_ISEL_IN
	_phy_rtd_part_outl(0x1800da54, 11, 8, APHY_Para_tmds_1[tmds_timing].B_6_30_1); //[APHY para]   		B-Lane LE_NC_ISEL_IN
	_phy_rtd_part_outl(0x1800da74, 11, 8, APHY_Para_tmds_1[tmds_timing].B_6_30_1_32); //[APHY para]   		B-Lane LE_NC_ISEL_IN
	_phy_rtd_part_outl(0x1800da94, 11, 8, APHY_Para_tmds_1[tmds_timing].B_6_30_1_64); //[APHY para]   		B-Lane LE_NC_ISEL_IN
	_phy_rtd_part_outl(0x1800da54, 15, 12, 0xf); //[APHY para]  		B-Lane TAP0_ISEL_IN
	_phy_rtd_part_outl(0x1800da74, 15, 12, 0xf); //[APHY para]  		B-Lane TAP0_ISEL_IN
	_phy_rtd_part_outl(0x1800da94, 15, 12, 0xf); //[APHY para]  		B-Lane TAP0_ISEL_IN
	_phy_rtd_part_outl(0x1800da54, 17, 16, APHY_Para_tmds_1[tmds_timing].B_7_32_1); //[APHY para]   		B-Lane LE_IHALF[1:0]
	_phy_rtd_part_outl(0x1800da74, 17, 16, APHY_Para_tmds_1[tmds_timing].B_7_32_1_32); //[APHY para]   		B-Lane LE_IHALF[1:0]
	_phy_rtd_part_outl(0x1800da94, 17, 16, APHY_Para_tmds_1[tmds_timing].B_7_32_1_64); //[APHY para]   		B-Lane LE_IHALF[1:0]
	_phy_rtd_part_outl(0x1800da54, 18, 18, APHY_Para_tmds_1[tmds_timing].B_7_33_1); //[APHY para]   		B-Lane NC_IHALF
	_phy_rtd_part_outl(0x1800da74, 18, 18, APHY_Para_tmds_1[tmds_timing].B_7_33_1_32); //[APHY para]   		B-Lane NC_IHALF
	_phy_rtd_part_outl(0x1800da94, 18, 18, APHY_Para_tmds_1[tmds_timing].B_7_33_1_64); //[APHY para]   		B-Lane NC_IHALF
	_phy_rtd_part_outl(0x1800da54, 19, 19, APHY_Para_tmds_1[tmds_timing].B_7_34_1); //[APHY para]   		B-Lane TAP0_IHALF
	_phy_rtd_part_outl(0x1800da74, 19, 19, APHY_Para_tmds_1[tmds_timing].B_7_34_1_32); //[APHY para]   		B-Lane TAP0_IHALF
	_phy_rtd_part_outl(0x1800da94, 19, 19, APHY_Para_tmds_1[tmds_timing].B_7_34_1_64); //[APHY para]   		B-Lane TAP0_IHALF
	_phy_rtd_part_outl(0x1800da54, 20, 20, 0x0); //[APHY para]   		B-Lane EN_LEQ6G
	_phy_rtd_part_outl(0x1800da74, 20, 20, 0x0); //[APHY para]   		B-Lane EN_LEQ6G
	_phy_rtd_part_outl(0x1800da94, 20, 20, 0x0); //[APHY para]   		B-Lane EN_LEQ6G
	_phy_rtd_part_outl(0x1800da54, 22, 22, APHY_Para_tmds_1[tmds_timing].B_7_36_1); //[APHY para]   	B-Lane POW_NC
	_phy_rtd_part_outl(0x1800da74, 22, 22, APHY_Para_tmds_1[tmds_timing].B_7_36_1_32); //[APHY para]   	B-Lane POW_NC
	_phy_rtd_part_outl(0x1800da94, 22, 22, APHY_Para_tmds_1[tmds_timing].B_7_36_1_64); //[APHY para]   	B-Lane POW_NC
	_phy_rtd_part_outl(0x1800da54, 30, 28, 0x3); //[APHY para]   		B-Lane DATALANE_BIAS_ISEL[2:0]
	_phy_rtd_part_outl(0x1800da74, 30, 28, 0x3); //[APHY para]   		B-Lane DATALANE_BIAS_ISEL[2:0]
	_phy_rtd_part_outl(0x1800da94, 30, 28, 0x3); //[APHY para]   		B-Lane DATALANE_BIAS_ISEL[2:0]
	_phy_rtd_part_outl(0x1800da54, 31, 31, 0x1); //[APHY para]   		B-Lane POW_LEQ_RL
	_phy_rtd_part_outl(0x1800da74, 31, 31, 0x1); //[APHY para]   		B-Lane POW_LEQ_RL
	_phy_rtd_part_outl(0x1800da94, 31, 31, 0x1); //[APHY para]   		B-Lane POW_LEQ_RL
	_phy_rtd_part_outl(0x1800da58, 1, 1, APHY_Para_tmds_1[tmds_timing].B_9_39_1); //[APHY para]  		B-Lane SEL_CMFB_LS
	_phy_rtd_part_outl(0x1800da78, 1, 1, APHY_Para_tmds_1[tmds_timing].B_9_39_1_32); //[APHY para]  		B-Lane SEL_CMFB_LS
	_phy_rtd_part_outl(0x1800da98, 1, 1, APHY_Para_tmds_1[tmds_timing].B_9_39_1_64); //[APHY para]  		B-Lane SEL_CMFB_LS
	_phy_rtd_part_outl(0x1800da58, 3, 2, 0x3); //[APHY para]  		B-Lane LEQ_CUR_ADJ
	_phy_rtd_part_outl(0x1800da78, 3, 2, 0x3); //[APHY para]  		B-Lane LEQ_CUR_ADJ
	_phy_rtd_part_outl(0x1800da98, 3, 2, 0x3); //[APHY para]  		B-Lane LEQ_CUR_ADJ
	_phy_rtd_part_outl(0x1800da58, 5, 4, 0x0); //[APHY para]  		B-Lane PTAT_CUR_ADJ
	_phy_rtd_part_outl(0x1800da78, 5, 4, 0x0); //[APHY para]  		B-Lane PTAT_CUR_ADJ
	_phy_rtd_part_outl(0x1800da98, 5, 4, 0x0); //[APHY para]  		B-Lane PTAT_CUR_ADJ
	_phy_rtd_part_outl(0x1800da58, 6, 6, 0x0); //[APHY para]  		B-Lane REG_BIAS_POW_CON_GM
	_phy_rtd_part_outl(0x1800da78, 6, 6, 0x0); //[APHY para]  		B-Lane REG_BIAS_POW_CON_GM
	_phy_rtd_part_outl(0x1800da98, 6, 6, 0x0); //[APHY para]  		B-Lane REG_BIAS_POW_CON_GM
	_phy_rtd_part_outl(0x1800da58, 11, 9, 0x0); //[APHY para] 		B-Lane PTAT_CUR_ADJ_FINE
	_phy_rtd_part_outl(0x1800da78, 11, 9, 0x0); //[APHY para] 		B-Lane PTAT_CUR_ADJ_FINE
	_phy_rtd_part_outl(0x1800da98, 11, 9, 0x0); //[APHY para] 		B-Lane PTAT_CUR_ADJ_FINE
	_phy_rtd_part_outl(0x1800da58, 16, 16, 0x1); //[APHY para] 		B-Lane FR_CK_SEL
	_phy_rtd_part_outl(0x1800da78, 16, 16, 0x1); //[APHY para] 		B-Lane FR_CK_SEL
	_phy_rtd_part_outl(0x1800da98, 16, 16, 0x1); //[APHY para] 		B-Lane FR_CK_SEL
	_phy_rtd_part_outl(0x1800da5c, 11, 9, APHY_Para_tmds_1[tmds_timing].B_14_45_1); //[APHY para]   	B-Lane DEMUX_PIX_RATE_SEL<2:0>
	_phy_rtd_part_outl(0x1800da7c, 11, 9, APHY_Para_tmds_1[tmds_timing].B_14_45_1_32); //[APHY para]   	B-Lane DEMUX_PIX_RATE_SEL<2:0>
	_phy_rtd_part_outl(0x1800da9c, 11, 9, APHY_Para_tmds_1[tmds_timing].B_14_45_1_64); //[APHY para]   	B-Lane DEMUX_PIX_RATE_SEL<2:0>
	_phy_rtd_part_outl(0x1800da5c, 12, 12, 0x1); //[APHY para]   	B-Lane DEMUX_FR_CK_SEL
	_phy_rtd_part_outl(0x1800da7c, 12, 12, 0x1); //[APHY para]   	B-Lane DEMUX_FR_CK_SEL
	_phy_rtd_part_outl(0x1800da9c, 12, 12, 0x1); //[APHY para]   	B-Lane DEMUX_FR_CK_SEL
	_phy_rtd_part_outl(0x1800da5c, 15, 13, APHY_Para_tmds_1[tmds_timing].B_14_47_1); //[APHY para]   	B-Lane DEMUX_RATE_SEL
	_phy_rtd_part_outl(0x1800da7c, 15, 13, APHY_Para_tmds_1[tmds_timing].B_14_47_1_32); //[APHY para]   	B-Lane DEMUX_RATE_SEL
	_phy_rtd_part_outl(0x1800da9c, 15, 13, APHY_Para_tmds_1[tmds_timing].B_14_47_1_64); //[APHY para]   	B-Lane DEMUX_RATE_SEL
	_phy_rtd_part_outl(0x1800da5c, 24, 24, 0x0); //[APHY para]  		B-Lane QCG1_SEL_BIAS_ST
	_phy_rtd_part_outl(0x1800da7c, 24, 24, 0x0); //[APHY para]  		B-Lane QCG1_SEL_BIAS_ST
	_phy_rtd_part_outl(0x1800da9c, 24, 24, 0x0); //[APHY para]  		B-Lane QCG1_SEL_BIAS_ST
	_phy_rtd_part_outl(0x1800da5c, 25, 25, 0x0); //[APHY para]  		B-Lane QCG1_SEL_BIAS_RG
	_phy_rtd_part_outl(0x1800da7c, 25, 25, 0x0); //[APHY para]  		B-Lane QCG1_SEL_BIAS_RG
	_phy_rtd_part_outl(0x1800da9c, 25, 25, 0x0); //[APHY para]  		B-Lane QCG1_SEL_BIAS_RG
	_phy_rtd_part_outl(0x1800da5c, 27, 26, APHY_Para_tmds_1[tmds_timing].B_16_50_1); //[APHY para]  		B-Lane QCG1 Rload SEL
	_phy_rtd_part_outl(0x1800da7c, 27, 26, APHY_Para_tmds_1[tmds_timing].B_16_50_1_32); //[APHY para]  		B-Lane QCG1 Rload SEL
	_phy_rtd_part_outl(0x1800da9c, 27, 26, APHY_Para_tmds_1[tmds_timing].B_16_50_1_64); //[APHY para]  		B-Lane QCG1 Rload SEL
	_phy_rtd_part_outl(0x1800da5c, 31, 28, APHY_Para_tmds_1[tmds_timing].B_16_51_1); //[APHY para]  		B-Lane QCG1 Cload SEL
	_phy_rtd_part_outl(0x1800da7c, 31, 28, APHY_Para_tmds_1[tmds_timing].B_16_51_1_32); //[APHY para]  		B-Lane QCG1 Cload SEL
	_phy_rtd_part_outl(0x1800da9c, 31, 28, APHY_Para_tmds_1[tmds_timing].B_16_51_1_64); //[APHY para]  		B-Lane QCG1 Cload SEL
	_phy_rtd_part_outl(0x1800da60, 11, 9, APHY_Para_tmds_1[tmds_timing].B_18_52_1); //[APHY para]  B-Lane PI_ISEL<2:0>
	_phy_rtd_part_outl(0x1800da80, 11, 9, APHY_Para_tmds_1[tmds_timing].B_18_52_1_32); //[APHY para]  B-Lane PI_ISEL<2:0>
	_phy_rtd_part_outl(0x1800daa0, 11, 9, APHY_Para_tmds_1[tmds_timing].B_18_52_1_64); //[APHY para]  B-Lane PI_ISEL<2:0>
	_phy_rtd_part_outl(0x1800da60, 14, 14, 0x1); //[APHY para]  B-Lane PI_CSEL
	_phy_rtd_part_outl(0x1800da80, 14, 14, 0x1); //[APHY para]  B-Lane PI_CSEL
	_phy_rtd_part_outl(0x1800daa0, 14, 14, 0x1); //[APHY para]  B-Lane PI_CSEL
	_phy_rtd_part_outl(0x1800da60, 20, 18, APHY_Para_tmds_1[tmds_timing].B_19_54_1); //[APHY para]  B-Lane PI_DIV_SEL
	_phy_rtd_part_outl(0x1800da80, 20, 18, APHY_Para_tmds_1[tmds_timing].B_19_54_1_32); //[APHY para]  B-Lane PI_DIV_SEL
	_phy_rtd_part_outl(0x1800daa0, 20, 18, APHY_Para_tmds_1[tmds_timing].B_19_54_1_64); //[APHY para]  B-Lane PI_DIV_SEL
	_phy_rtd_part_outl(0x1800da60, 22, 22, 0x0); //[APHY para]  B-Lane DCDR ckout SEL
	_phy_rtd_part_outl(0x1800da80, 22, 22, 0x0); //[APHY para]  B-Lane DCDR ckout SEL
	_phy_rtd_part_outl(0x1800daa0, 22, 22, 0x0); //[APHY para]  B-Lane DCDR ckout SEL
	_phy_rtd_part_outl(0x1800da60, 31, 31, 0x1); //[APHY para]  B-Lane DFE Ckin SEL.
	_phy_rtd_part_outl(0x1800da80, 31, 31, 0x1); //[APHY para]  B-Lane DFE Ckin SEL.
	_phy_rtd_part_outl(0x1800daa0, 31, 31, 0x1); //[APHY para]  B-Lane DFE Ckin SEL.
	_phy_rtd_part_outl(0x1800da6c, 7, 0, APHY_Para_tmds_1[tmds_timing].ACDR_B_9_57_1); //[APHY para]  B-Lane CMU_SEL_M_DIV[7:0]
	_phy_rtd_part_outl(0x1800da8c, 7, 0, APHY_Para_tmds_1[tmds_timing].ACDR_B_9_57_1_32); //[APHY para]  B-Lane CMU_SEL_M_DIV[7:0]
	_phy_rtd_part_outl(0x1800daac, 7, 0, APHY_Para_tmds_1[tmds_timing].ACDR_B_9_57_1_64); //[APHY para]  B-Lane CMU_SEL_M_DIV[7:0]
	_phy_rtd_part_outl(0x1800da6c, 8, 8, 0x0); //[APHY para]  B-Lane CMU_SEL_M_DIV[8]
	_phy_rtd_part_outl(0x1800da8c, 8, 8, 0x0); //[APHY para]  B-Lane CMU_SEL_M_DIV[8]
	_phy_rtd_part_outl(0x1800daac, 8, 8, 0x0); //[APHY para]  B-Lane CMU_SEL_M_DIV[8]
	_phy_rtd_part_outl(0x1800da6c, 9, 9, APHY_Para_tmds_1[tmds_timing].ACDR_B_10_59_1); //[APHY para]  B-Lane CMU_SEL_PRE_M_DIV
	_phy_rtd_part_outl(0x1800da8c, 9, 9, APHY_Para_tmds_1[tmds_timing].ACDR_B_10_59_1_32); //[APHY para]  B-Lane CMU_SEL_PRE_M_DIV
	_phy_rtd_part_outl(0x1800daac, 9, 9, APHY_Para_tmds_1[tmds_timing].ACDR_B_10_59_1_64); //[APHY para]  B-Lane CMU_SEL_PRE_M_DIV
	_phy_rtd_part_outl(0x1800da6c, 10, 10, 0x1); //[APHY para]  B-Lane CMU_SEL_POST_M_DIV
	_phy_rtd_part_outl(0x1800da8c, 10, 10, 0x1); //[APHY para]  B-Lane CMU_SEL_POST_M_DIV
	_phy_rtd_part_outl(0x1800daac, 10, 10, 0x1); //[APHY para]  B-Lane CMU_SEL_POST_M_DIV
	_phy_rtd_part_outl(0x1800da6c, 21, 20, APHY_Para_tmds_1[tmds_timing].ACDR_B_11_61_1); //[APHY para]  B-Lane RLSEL_LE1_2
	_phy_rtd_part_outl(0x1800da8c, 21, 20, APHY_Para_tmds_1[tmds_timing].ACDR_B_11_61_1_32); //[APHY para]  B-Lane RLSEL_LE1_2
	_phy_rtd_part_outl(0x1800daac, 21, 20, APHY_Para_tmds_1[tmds_timing].ACDR_B_11_61_1_64); //[APHY para]  B-Lane RLSEL_LE1_2
	_phy_rtd_part_outl(0x1800da6c, 23, 22, 0x0); //[APHY para]  B-Lane RLSEL_NC_2
	_phy_rtd_part_outl(0x1800da8c, 23, 22, 0x0); //[APHY para]  B-Lane RLSEL_NC_2
	_phy_rtd_part_outl(0x1800daac, 23, 22, 0x0); //[APHY para]  B-Lane RLSEL_NC_2
	_phy_rtd_part_outl(0x1800da6c, 24, 24, 0x0); //[APHY para]  B-Lane POW_NC
	_phy_rtd_part_outl(0x1800da8c, 24, 24, 0x0); //[APHY para]  B-Lane POW_NC
	_phy_rtd_part_outl(0x1800daac, 24, 24, 0x0); //[APHY para]  B-Lane POW_NC
	_phy_rtd_part_outl(0x1800dab4, 4, 4, 0x0); //[APHY para]  CMU input clock source select:
	_phy_rtd_part_outl(0x1800dab4, 7, 7, 0x1); //[APHY para]  CMU_CKXTAL_SEL
	_phy_rtd_part_outl(0x1800dab8, 8, 8, 0x1); //[APHY para  CMU_SEL_CP
}

void DPHY_Init_Flow_tmds_1_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	// DPHY init flow start
	_phy_rtd_part_outl(0x1800db88, 4, 4, 0x0); //[DPHY init flow]  reg_p0_b_fifo_en
	_phy_rtd_part_outl(0x1800db88, 12, 12, 0x0); //[DPHY init flow]  reg_p0_g_fifo_en
	_phy_rtd_part_outl(0x1800db88, 20, 20, 0x0); //[DPHY init flow]  reg_p0_r_fifo_en
	_phy_rtd_part_outl(0x1800db88, 28, 28, 0x0); //[DPHY init flow]  reg_p0_ck_fifo_en
	_phy_rtd_part_outl(0x1800dc6c, 1, 1, 0x0); //[DPHY init flow]  reg_p0_pow_save_10b18b_sel
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0x0e); //[DPHY init flow]  reg_p0_dig_debug_sel
	//DPHY fld flow start
	_phy_rtd_part_outl(0x1800dbb8, 10, 3, 0x08); //[DPHY fld flow]  reg_p0_b_divide_num
	_phy_rtd_part_outl(0x1800dbbc, 11, 0, 0x0db); //[DPHY fld flow] reg_p0_b_lock_dn_limit
	_phy_rtd_part_outl(0x1800dbbc, 27, 16, 0x0e1); //[DPHY fld flow] reg_p0_b_lock_up_limit
	_phy_rtd_part_outl(0x1800dbc4, 19, 8, 0x0db); //[DPHY fld flow] reg_p0_b_coarse_lock_dn_limit
	_phy_rtd_part_outl(0x1800dbc4, 31, 20, 0x0e1); //[DPHY fld flow] reg_p0_b_coarse_lock_up_limit
	_phy_rtd_part_outl(0x1800dbc8, 1, 1, 0x1); //[DPHY fld flow]  reg_p0_b_fld_rstb
	_phy_rtd_part_outl(0x1800dbd8, 10, 3, 0x08); //[DPHY fld flow]  reg_p0_g_divide_num
	_phy_rtd_part_outl(0x1800dbdc, 11, 0, 0x0db); //[DPHY fld flow] reg_p0_g_lock_dn_limit
	_phy_rtd_part_outl(0x1800dbdc, 27, 16, 0x0e1); //[DPHY fld flow] reg_p0_g_lock_up_limit
	_phy_rtd_part_outl(0x1800dbe4, 19, 8, 0x0db); //[DPHY fld flow] reg_p0_g_coarse_lock_dn_limit
	_phy_rtd_part_outl(0x1800dbe4, 31, 20, 0x0e1); //[DPHY fld flow] reg_p0_g_coarse_lock_up_limit
	_phy_rtd_part_outl(0x1800dbe8, 1, 1, 0x1); //[DPHY fld flow]  reg_p0_g_fld_rstb
	_phy_rtd_part_outl(0x1800dbf4, 10, 3, 0x08); //[DPHY fld flow]  reg_p0_r_divide_num
	_phy_rtd_part_outl(0x1800dbf8, 11, 0, 0x0db); //[DPHY fld flow] reg_p0_r_lock_dn_limit
	_phy_rtd_part_outl(0x1800dbf8, 27, 16, 0x0e1); //[DPHY fld flow] reg_p0_r_lock_up_limit
	_phy_rtd_part_outl(0x1800dc00, 19, 8, 0x0db); //[DPHY fld flow] reg_p0_r_coarse_lock_dn_limit
	_phy_rtd_part_outl(0x1800dc00, 31, 20, 0x0e1); //[DPHY fld flow] reg_p0_r_coarse_lock_up_limit
	_phy_rtd_part_outl(0x1800dc04, 1, 1, 0x1); //[DPHY fld flow]  reg_p0_r_fld_rstb
	_phy_rtd_part_outl(0x1800dc10, 10, 3, 0x08); //[DPHY fld flow]  reg_p0_ck_divide_num
	_phy_rtd_part_outl(0x1800dc14, 11, 0, 0x0db); //[DPHY fld flow] reg_p0_ck_lock_dn_limit
	_phy_rtd_part_outl(0x1800dc14, 27, 16, 0x0e1); //[DPHY fld flow] reg_p0_ck_lock_up_limit
	_phy_rtd_part_outl(0x1800dc1c, 19, 8, 0x0db); //[DPHY fld flow] reg_p0_ck_coarse_lock_dn_limit
	_phy_rtd_part_outl(0x1800dc1c, 31, 20, 0x0e1); //[DPHY fld flow] reg_p0_ck_coarse_lock_up_limit
	_phy_rtd_part_outl(0x1800dc20, 1, 1, 0x1); //[DPHY fld flow]  reg_p0_ck_fld_rstb
	_phy_rtd_part_outl(0x1800dd10, 26, 26, 0x1); //[ADPHY Init flow]  reg_adphy_rst
	_phy_rtd_part_outl(0x1800dd1c, 0, 0, 0x0); //[ADPHY Init flow]  prevent address merging
	_phy_rtd_part_outl(0x1800dd10, 31, 31, 0x1); //[ADPHY Init flow]  reg_adphy_latch_clk_0
	_phy_rtd_part_outl(0x1800dd10, 30, 30, 0x1); //[ADPHY Init flow]  reg_adphy_latch_clk_1
	_phy_rtd_part_outl(0x1800dd1c, 0, 0, 0x0); //[ADPHY Init flow]  prevent address merging
	_phy_rtd_part_outl(0x1800dd10, 31, 31, 0x0); //[ADPHY Init flow]  reg_adphy_latch_clk_0
	_phy_rtd_part_outl(0x1800dd10, 30, 30, 0x0); //[ADPHY Init flow]  reg_adphy_latch_clk_1
}

void APHY_Init_Flow_tmds_1_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	_phy_rtd_part_outl(0x1800da30, 0, 0, 1); //INOFF_EN =0 ->1 ck off
	udelay(100);
	_phy_rtd_part_outl(0x1800da30, 0, 0, 0); //INOFF_EN =0 ->1 ck on

	// APHY init flow start
	_phy_rtd_part_outl(0x1800da68, 19, 19, 0x1); //B //REG_P0_ACDR_B_7 [3] TIE VCTRL = 1
	_phy_rtd_part_outl(0x1800da88, 19, 19, 0x1); //G //REG_P0_ACDR_B_7 [3] TIE VCTRL = 1
	_phy_rtd_part_outl(0x1800daa8, 19, 19, 0x1); //R //REG_P0_ACDR_B_7 [3] TIE VCTRL = 1
	_phy_rtd_part_outl(0x1800da48, 19, 19, 0x1); //CK //REG_P0_ACDR_B_7 [3] TIE VCTRL = 1
	udelay(100);
	_phy_rtd_part_outl(0x1800da18, 1, 1, 0x0); //[APHY init flow]  REG_CK_LATCH Before PSM, latch the register value of PHY. 0->1: Latch
	_phy_rtd_part_outl(0x1800da30, 6, 6, 0x0); //[APHY init flow]  CK-Lane FAST_SW_EN
	_phy_rtd_part_outl(0x1800da30, 7, 7, 0x0); //[APHY init flow]  CK-Lane FAST_SW_DELAY_EN
	_phy_rtd_part_outl(0x1800da38, 17, 17, 0x1); //[APHY init flow] 	CK-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da40, 16, 16, 0x0); //[APHY init flow]  CK-Lane DCDR_RSTB
	_phy_rtd_part_outl(0x1800da40, 17, 17, 0x0); //[APHY init flow]  CK-Lane PI_DIV rstb
	_phy_rtd_part_outl(0x1800da40, 24, 24, 0x0); //[APHY init flow]  CK-Lane FKP_RSTB (FKP:Fast KP)
	_phy_rtd_part_outl(0x1800da40, 28, 28, 0x0); //[APHY init flow]  CK-Lane FKT EN
	_phy_rtd_part_outl(0x1800da44, 1, 1, 0x0); //[APHY init flow]  CK-Lane ACDR_RSTB_CLK_FLD
	_phy_rtd_part_outl(0x1800da44, 16, 16, 0x0); //[APHY init flow]  CK-Lane ACDR_RSTB_DIV_IQ
	_phy_rtd_part_outl(0x1800da44, 16, 16, 0x1); //[APHY init flow]  CK-Lane ACDR_RSTB_DIV_IQ
	_phy_rtd_part_outl(0x1800da44, 20, 20, 0x1); //[APHY para]  CK-Lane ACDR_SEL_0FR_1HR_DIV_IQ
	_phy_rtd_part_outl(0x1800da48, 24, 24, 0x0); //[APHY init flow]  CK-Lane RSTB_BBPD_KP_KI
	_phy_rtd_part_outl(0x1800da4c, 11, 11, 0x1); //[APHY init flow]  CK-Lane RSTB_PRESCALER
	_phy_rtd_part_outl(0x1800da4c, 12, 12, 0x1); //[APHY init flow]  CK-Lane RSTB_M_DIV
	_phy_rtd_part_outl(0x1800da44, 0, 0, 0x0); //[APHY init flow]  CK-Lane ACDR_RSTB_PFD	
	udelay(100);
	_phy_rtd_part_outl(0x1800da44, 0, 0, 0x1); //[APHY init flow]  CK-Lane ACDR_RSTB_PFD
	_phy_rtd_part_outl(0x1800da50, 6, 6, 0x0); //[APHY init flow]  	B-Lane FAST_SW_EN
	_phy_rtd_part_outl(0x1800da70, 6, 6, 0x0); //[APHY init flow]  	B-Lane FAST_SW_EN
	_phy_rtd_part_outl(0x1800da90, 6, 6, 0x0); //[APHY init flow]  	B-Lane FAST_SW_EN
	_phy_rtd_part_outl(0x1800da50, 7, 7, 0x0); //[APHY init flow]  	B-Lane FAST_SW_DELAY_EN
	_phy_rtd_part_outl(0x1800da70, 7, 7, 0x0); //[APHY init flow]  	B-Lane FAST_SW_DELAY_EN
	_phy_rtd_part_outl(0x1800da90, 7, 7, 0x0); //[APHY init flow]  	B-Lane FAST_SW_DELAY_EN
	_phy_rtd_part_outl(0x1800da58, 17, 17, 0x1); //[APHY init flow] 	B-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da78, 17, 17, 0x1); //[APHY init flow] 	B-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da98, 17, 17, 0x1); //[APHY init flow] 	B-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da60, 16, 16, 0x1); //[APHY init flow]  B-Lane DCDR_RSTB
	_phy_rtd_part_outl(0x1800da80, 16, 16, 0x1); //[APHY init flow]  B-Lane DCDR_RSTB
	_phy_rtd_part_outl(0x1800daa0, 16, 16, 0x1); //[APHY init flow]  B-Lane DCDR_RSTB
	_phy_rtd_part_outl(0x1800da60, 17, 17, 0x1); //[APHY init flow]  B-Lane PI_DIV rstb
	_phy_rtd_part_outl(0x1800da80, 17, 17, 0x1); //[APHY init flow]  B-Lane PI_DIV rstb
	_phy_rtd_part_outl(0x1800daa0, 17, 17, 0x1); //[APHY init flow]  B-Lane PI_DIV rstb
	_phy_rtd_part_outl(0x1800da60, 24, 24, 0x0); //[APHY init flow]  B-Lane FKP_RSTB (FKP:Fast KP)
	_phy_rtd_part_outl(0x1800da80, 24, 24, 0x0); //[APHY init flow]  B-Lane FKP_RSTB (FKP:Fast KP)
	_phy_rtd_part_outl(0x1800daa0, 24, 24, 0x0); //[APHY init flow]  B-Lane FKP_RSTB (FKP:Fast KP)
	_phy_rtd_part_outl(0x1800da60, 28, 28, 0x0); //[APHY init flow]  B-Lane FKT EN
	_phy_rtd_part_outl(0x1800da80, 28, 28, 0x0); //[APHY init flow]  B-Lane FKT EN
	_phy_rtd_part_outl(0x1800daa0, 28, 28, 0x0); //[APHY init flow]  B-Lane FKT EN
	_phy_rtd_part_outl(0x1800da64, 1, 1, 0x0); //[APHY init flow]  B-Lane ACDR_RSTB_CLK_FLD
	_phy_rtd_part_outl(0x1800da84, 1, 1, 0x0); //[APHY init flow]  B-Lane ACDR_RSTB_CLK_FLD
	_phy_rtd_part_outl(0x1800daa4, 1, 1, 0x0); //[APHY init flow]  B-Lane ACDR_RSTB_CLK_FLD
	_phy_rtd_part_outl(0x1800da64, 16, 16, 0x0); //[APHY init flow]  B-Lane ACDR_RSTB_DIV_IQ
	_phy_rtd_part_outl(0x1800da84, 16, 16, 0x0); //[APHY init flow]  B-Lane ACDR_RSTB_DIV_IQ
	_phy_rtd_part_outl(0x1800daa4, 16, 16, 0x0); //[APHY init flow]  B-Lane ACDR_RSTB_DIV_IQ
	_phy_rtd_part_outl(0x1800da64, 16, 16, 0x1); //[APHY init flow]  B-Lane ACDR_RSTB_DIV_IQ
	_phy_rtd_part_outl(0x1800da84, 16, 16, 0x1); //[APHY init flow]  B-Lane ACDR_RSTB_DIV_IQ
	_phy_rtd_part_outl(0x1800daa4, 16, 16, 0x1); //[APHY init flow]  B-Lane ACDR_RSTB_DIV_IQ
	_phy_rtd_part_outl(0x1800da64, 20, 20, 0x0); //[APHY init flow]  B-Lane RSTB_REF_FB_DIV
	_phy_rtd_part_outl(0x1800da84, 20, 20, 0x0); //[APHY init flow]  B-Lane RSTB_REF_FB_DIV
	_phy_rtd_part_outl(0x1800daa4, 20, 20, 0x0); //[APHY init flow]  B-Lane RSTB_REF_FB_DIV
	_phy_rtd_part_outl(0x1800da64, 20, 20, 0x1); //[APHY init flow]  B-Lane RSTB_REF_FB_DIV
	_phy_rtd_part_outl(0x1800da84, 20, 20, 0x1); //[APHY init flow]  B-Lane RSTB_REF_FB_DIV
	_phy_rtd_part_outl(0x1800daa4, 20, 20, 0x1); //[APHY init flow]  B-Lane RSTB_REF_FB_DIV
	_phy_rtd_part_outl(0x1800da68, 24, 24, 0x1); //[APHY init flow]  B-Lane RSTB_BBPD_KP_KI
	_phy_rtd_part_outl(0x1800da88, 24, 24, 0x1); //[APHY init flow]  B-Lane RSTB_BBPD_KP_KI
	_phy_rtd_part_outl(0x1800daa8, 24, 24, 0x1); //[APHY init flow]  B-Lane RSTB_BBPD_KP_KI
	_phy_rtd_part_outl(0x1800da6c, 11, 11, 0x0); //[APHY init flow]  B-Lane RSTB_PRESCALER
	_phy_rtd_part_outl(0x1800da8c, 11, 11, 0x0); //[APHY init flow]  B-Lane RSTB_PRESCALER
	_phy_rtd_part_outl(0x1800daac, 11, 11, 0x0); //[APHY init flow]  B-Lane RSTB_PRESCALER
	_phy_rtd_part_outl(0x1800da6c, 11, 11, 0x1); //[APHY init flow]  B-Lane RSTB_PRESCALER
	_phy_rtd_part_outl(0x1800da8c, 11, 11, 0x1); //[APHY init flow]  B-Lane RSTB_PRESCALER
	_phy_rtd_part_outl(0x1800daac, 11, 11, 0x1); //[APHY init flow]  B-Lane RSTB_PRESCALER
	_phy_rtd_part_outl(0x1800da6c, 12, 12, 0x0); //[APHY init flow]  B-Lane RSTB_M_DIV
	_phy_rtd_part_outl(0x1800da8c, 12, 12, 0x0); //[APHY init flow]  B-Lane RSTB_M_DIV
	_phy_rtd_part_outl(0x1800daac, 12, 12, 0x0); //[APHY init flow]  B-Lane RSTB_M_DIV
	_phy_rtd_part_outl(0x1800da6c, 12, 12, 0x1); //[APHY init flow]  B-Lane RSTB_M_DIV
	_phy_rtd_part_outl(0x1800da8c, 12, 12, 0x1); //[APHY init flow]  B-Lane RSTB_M_DIV
	_phy_rtd_part_outl(0x1800daac, 12, 12, 0x1); //[APHY init flow]  B-Lane RSTB_M_DIV
	_phy_rtd_part_outl(0x1800da64, 0, 0, 0x0); //[APHY init flow]  B-Lane ACDR_RSTB_PFD
	_phy_rtd_part_outl(0x1800da84, 0, 0, 0x0); //[APHY init flow]  B-Lane ACDR_RSTB_PFD
	_phy_rtd_part_outl(0x1800daa4, 0, 0, 0x0); //[APHY init flow]  B-Lane ACDR_RSTB_PFD
	udelay(100);
	_phy_rtd_part_outl(0x1800da64, 0, 0, 0x1); //[APHY init flow]  B-Lane ACDR_RSTB_PFD
	_phy_rtd_part_outl(0x1800da84, 0, 0, 0x1); //[APHY init flow]  B-Lane ACDR_RSTB_PFD
	_phy_rtd_part_outl(0x1800daa4, 0, 0, 0x1); //[APHY init flow]  B-Lane ACDR_RSTB_PFD
	_phy_rtd_part_outl(0x1800da68, 19, 19, 0x0); //B //REG_P0_ACDR_B_7 [3] TIE VCTRL = 0
	_phy_rtd_part_outl(0x1800da88, 19, 19, 0x0); //G //REG_P0_ACDR_B_7 [3] TIE VCTRL = 0
	_phy_rtd_part_outl(0x1800daa8, 19, 19, 0x0); //R //REG_P0_ACDR_B_7 [3] TIE VCTRL = 0
	_phy_rtd_part_outl(0x1800da48, 19, 19, 0x0); //CK //REG_P0_ACDR_B_7 [3] TIE VCTRL= 0
	//APHY acdr flow start
	_phy_rtd_part_outl(0x1800dafc, 0, 0, 0x0); //[APHY acdr flow]  P0_ACDR_B_ACDR_EN_AFN
	_phy_rtd_part_outl(0x1800dafc, 1, 1, 0x0); //[APHY acdr flow]  P0_ACDR_G_ACDR_EN_AFN
	_phy_rtd_part_outl(0x1800dafc, 2, 2, 0x0); //[APHY acdr flow]  P0_ACDR_R_ACDR_EN_AFN
	_phy_rtd_part_outl(0x1800dafc, 3, 3, 0x0); //[APHY acdr flow]  P0_ACDR_CK_ACDR_EN_AFN
	_phy_rtd_part_outl(0x1800dafc, 4, 4, 0x1); //[APHY acdr flow]  P0_B_DEMUX_RSTB
	_phy_rtd_part_outl(0x1800dafc, 5, 5, 0x1); //[APHY acdr flow]  P0_G_DEMUX_RSTB
	_phy_rtd_part_outl(0x1800dafc, 6, 6, 0x1); //[APHY acdr flow]  P0_R_DEMUX_RSTB
	_phy_rtd_part_outl(0x1800dafc, 7, 7, 0x0); //[APHY acdr flow]  P0_CK_DEMUX_RSTB
	_phy_rtd_part_outl(0x1800da3c, 7, 7, 0x0); //[APHY acdr flow]  		CK-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da5c, 7, 7, 0x1); //[APHY acdr flow]  		B-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da7c, 7, 7, 0x1); //[APHY acdr flow]  		G-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da9c, 7, 7, 0x1); //[APHY acdr flow]  R-Lane DFE_ADAPT_EN
}

void DCDR_settings_tmds_1_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	_phy_rtd_part_outl(0x1800da58, 17, 17, 0); //BBPD_rstb
	_phy_rtd_part_outl(0x1800da78, 17, 17, 0); //BBPD_rstb
	_phy_rtd_part_outl(0x1800da98, 17, 17, 0); //BBPD_rstb
	_phy_rtd_part_outl(0x1800da58, 17, 17, 1); //BBPD_rstb
	_phy_rtd_part_outl(0x1800da78, 17, 17, 1); //BBPD_rstb
	_phy_rtd_part_outl(0x1800da98, 17, 17, 1); //BBPD_rstb
	//#====================================== CMU RESET
	_phy_rtd_part_outl(0x1800dafc, 4, 4, 0x0); //DEMUX_RSTB=0
	_phy_rtd_part_outl(0x1800dafc, 5, 5, 0x0); //DEMUX_RSTB=1
	_phy_rtd_part_outl(0x1800dafc, 6, 6, 0x0); //DEMUX_RSTB=0
	_phy_rtd_part_outl(0x1800dafc, 7, 7, 0x0); //DEMUX_RSTB=1
	_phy_rtd_part_outl(0x1800dafc, 4, 4, 0x1); //DEMUX_RSTB=0
	_phy_rtd_part_outl(0x1800dafc, 5, 5, 0x1); //DEMUX_RSTB=1
	_phy_rtd_part_outl(0x1800dafc, 6, 6, 0x1); //DEMUX_RSTB=0
	_phy_rtd_part_outl(0x1800dafc, 7, 7, 0x1); //DEMUX_RSTB=1
	//ScalerTimer_DelayXms(5);000
	//#===================================== phase IV : set pow_save_bypass == 0
	//#====================================== cdr reset release
}

void DCDR_settings_tmds_2_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	_phy_rtd_part_outl(0x1800db88, 4, 4, 0x0); //fifo_en
	_phy_rtd_part_outl(0x1800db88, 12, 12, 0x0); //fifo_en
	_phy_rtd_part_outl(0x1800db88, 20, 20, 0x0); //fifo_en
	_phy_rtd_part_outl(0x1800db88, 28, 28, 0x0); //fifo_en
	_phy_rtd_part_outl(0x1800db00, 0, 0, 0x0);
	_phy_rtd_part_outl(0x1800db00, 1, 1, 0x0);
	_phy_rtd_part_outl(0x1800db00, 2, 2, 0x0);
	_phy_rtd_part_outl(0x1800db00, 3, 3, 0x0);
	_phy_rtd_part_outl(0x1800db00, 4, 4, 0x0);
	_phy_rtd_part_outl(0x1800db00, 5, 5, 0x0);
	_phy_rtd_part_outl(0x1800db00, 6, 6, 0x0);
	_phy_rtd_part_outl(0x1800db00, 7, 7, 0x0);
	_phy_rtd_part_outl(0x1800db00, 0, 0, 0x1);
	_phy_rtd_part_outl(0x1800db00, 1, 1, 0x1);
	_phy_rtd_part_outl(0x1800db00, 2, 2, 0x1);
	_phy_rtd_part_outl(0x1800db00, 3, 3, 0x1);
	_phy_rtd_part_outl(0x1800db00, 4, 4, 0x1);
	_phy_rtd_part_outl(0x1800db00, 5, 5, 0x1);
	_phy_rtd_part_outl(0x1800db00, 6, 6, 0x1);
	_phy_rtd_part_outl(0x1800db00, 7, 7, 0x1);
}

void Koffset_tmds_1_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	_phy_rtd_part_outl(0x1800dcb0, 15, 15, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 14, 14, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 13, 13, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 12, 12, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 11, 11, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 10, 10, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 9, 9, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 8, 8, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dc04, 0, 0, 0); //R finetunstart off
	_phy_rtd_part_outl(0x1800dbe8, 0, 0, 0); //G finetunstart off
	_phy_rtd_part_outl(0x1800dbc8, 0, 0, 0); //B finetunstart off
	_phy_rtd_part_outl(0x1800dc20, 0, 0, 0); //CK finetunstart off
	//========================================= STEP1 Data_Even KOFF =================================================
	_phy_rtd_part_outl(0x1800da58, 28, 28, 1); //#B  DFE_TAP0_ICOM_EN=1
	_phy_rtd_part_outl(0x1800da5c, 6, 3, 0); //#B  DFE_TAP_EN[4:1] = 0
	_phy_rtd_part_outl(0x1800da78, 28, 28, 1); //#G  DFE_TAP0_ICOM_EN=1
	_phy_rtd_part_outl(0x1800da7c, 6, 3, 0); //#G  DFE_TAP_EN[4:1] = 0
	_phy_rtd_part_outl(0x1800da98, 28, 28, 1); //#R  DFE_TAP0_ICOM_EN=1
	_phy_rtd_part_outl(0x1800da9c, 6, 3, 0); //#R  DFE_TAP_EN[4:1] = 0
	_phy_rtd_outl(0x1800db44, 0x0282cf00); //B_KOFF rstn,  //[26:20] timout = 0101000, [19:14] divisor = 001000, [11:8] delay count = 11
	_phy_rtd_outl(0x1800db54, 0x0282cf00); //G_KOFF rstn
	_phy_rtd_outl(0x1800db64, 0x0282cf00); //R_KOFF rstn
	_phy_rtd_part_outl(0x1800db44, 0, 0, 1); //Release B_KOFF rstn
	_phy_rtd_part_outl(0x1800db54, 0, 0, 1); //Release G_KOFF rstn
	_phy_rtd_part_outl(0x1800db64, 0, 0, 1); //Release R_KOFF rstn
	_phy_rtd_part_outl(0x1800dca0, 30, 28, 2); //reg_p0_b_offset_auto_en_cnt
	_phy_rtd_part_outl(0x1800dca4, 30, 28, 2); //reg_p0_g_offset_auto_en_cnt
	_phy_rtd_part_outl(0x1800dca8, 30, 28, 2); //reg_p0_r_offset_auto_en_cnt
	_phy_rtd_part_outl(0x1800db44, 11, 8, 0xf); //reg_p0_b_offset_delay_cnt
	_phy_rtd_part_outl(0x1800db54, 11, 8, 0xf); //reg_p0_g_offset_delay_cnt
	_phy_rtd_part_outl(0x1800db64, 11, 8, 0xf); //reg_p0_r_offset_delay_cnt
	//===B/G/R LANE KOFF
	_phy_rtd_part_outl(0x1800db4c, 28, 28, 0); //manaul_do=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db5c, 28, 28, 0); //manaul_do=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db6c, 28, 28, 0); //manaul_do=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db4c, 18, 18, 0); //manaul_de=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db5c, 18, 18, 0); //manaul_de=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db6c, 18, 18, 0); //manaul_de=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db50, 28, 28, 0); //manaul_eq=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db60, 28, 28, 0); //manaul_eq=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db70, 28, 28, 0); //manaul_eq=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db50, 18, 18, 0); //manaul_eo=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db60, 18, 18, 0); //manaul_eo=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db70, 18, 18, 0); //manaul_eo=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db50, 8, 8, 0); //manaul_ee=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db60, 8, 8, 0); //manaul_ee=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db70, 8, 8, 0); //manaul_ee=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db48, 18, 18, 0); //manaul_opo=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db58, 18, 18, 0); //manaul_opo=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db68, 18, 18, 0); //manaul_opo=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db48, 8, 8, 0); //manaul_ope=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db58, 8, 8, 0); //manaul_ope=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800db68, 8, 8, 0); //manaul_ope=0, 0:Manual; 1:Auto
	_phy_rtd_part_outl(0x1800da50, 0, 0, 1); //INOFF_EN =0 ->1
	_phy_rtd_part_outl(0x1800da70, 0, 0, 1); //INOFF_EN =0 ->1
	_phy_rtd_part_outl(0x1800da90, 0, 0, 1); //INOFF_EN =0 ->1
	_phy_rtd_part_outl(0x1800da58, 0, 0, 0); //REG_B_9[0] POW_CMFB_LEQ 1->0
	_phy_rtd_part_outl(0x1800da78, 0, 0, 0); //REG_B_9[0] POW_CMFB_LEQ 1->0
	_phy_rtd_part_outl(0x1800da98, 0, 0, 0); //REG_B_9[0] POW_CMFB_LEQ 1->0
	_phy_rtd_part_outl(0x1800da54, 22, 22, Koffset_tmds_1[tmds_timing].B_7_42_1); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  1->0
	_phy_rtd_part_outl(0x1800da74, 22, 22, Koffset_tmds_1[tmds_timing].B_7_42_1_32); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  1->0
	_phy_rtd_part_outl(0x1800da94, 22, 22, Koffset_tmds_1[tmds_timing].B_7_42_1_64); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  1->0
	_phy_rtd_part_outl(0x1800da54, 23, 23, 0); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  1->0
	_phy_rtd_part_outl(0x1800da74, 23, 23, 0); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  1->0
	_phy_rtd_part_outl(0x1800da94, 23, 23, 0); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  1->0
	_phy_rtd_part_outl(0x1800da54, 24, 24, 0); //REG_B_8[0] POW_LEQ:  1->0
	_phy_rtd_part_outl(0x1800da74, 24, 24, 0); //REG_B_8[0] POW_LEQ:  1->0
	_phy_rtd_part_outl(0x1800da94, 24, 24, 0); //REG_B_8[0] POW_LEQ:  1->0
	//B-Lane z0_ok
	_phy_rtd_part_outl(0x1800db50, 19, 19, 1); //[19]z0_ok_eo=1
	_phy_rtd_part_outl(0x1800db60, 19, 19, 1); //[19]z0_ok_eo=1
	_phy_rtd_part_outl(0x1800db70, 19, 19, 1); //[19]z0_ok_eo=1
	_phy_rtd_part_outl(0x1800db50, 9, 9, 1); //[9] z0_ok_ee=1
	_phy_rtd_part_outl(0x1800db60, 9, 9, 1); //[9] z0_ok_ee=1
	_phy_rtd_part_outl(0x1800db70, 9, 9, 1); //[9] z0_ok_ee=1
	_phy_rtd_part_outl(0x1800db4c, 29, 29, 1); //[29]z0_ok_do=1
	_phy_rtd_part_outl(0x1800db5c, 29, 29, 1); //[29]z0_ok_do=1
	_phy_rtd_part_outl(0x1800db6c, 29, 29, 1); //[29]z0_ok_do=1
	_phy_rtd_part_outl(0x1800db4c, 19, 19, 1); //[19]z0_ok_de=1
	_phy_rtd_part_outl(0x1800db5c, 19, 19, 1); //[19]z0_ok_de=1
	_phy_rtd_part_outl(0x1800db6c, 19, 19, 1); //[19]z0_ok_de=1
	_phy_rtd_part_outl(0x1800db48, 19, 19, 1); //[19] z0_ok_opo=1
	_phy_rtd_part_outl(0x1800db58, 19, 19, 1); //[19] z0_ok_opo=1
	_phy_rtd_part_outl(0x1800db68, 19, 19, 1); //[19] z0_ok_opo=1
	_phy_rtd_part_outl(0x1800db48, 9, 9, 1); //[9] z0_ok_ope=1
	_phy_rtd_part_outl(0x1800db58, 9, 9, 1); //[9] z0_ok_ope=1
	_phy_rtd_part_outl(0x1800db68, 9, 9, 1); //[9] z0_ok_ope=1
	_phy_rtd_part_outl(0x1800db4c, 21, 21, 0); //[20]offset_pc_do=1
	_phy_rtd_part_outl(0x1800db5c, 21, 21, 0); //[20]offset_pc_do=1
	_phy_rtd_part_outl(0x1800db6c, 21, 21, 0); //[20]offset_pc_do=1
	_phy_rtd_part_outl(0x1800db4c, 11, 11, 0); //[20]offset_pc_de=1
	_phy_rtd_part_outl(0x1800db5c, 11, 11, 0); //[20]offset_pc_de=1
	_phy_rtd_part_outl(0x1800db6c, 11, 11, 0); //[20]offset_pc_de=1
	_phy_rtd_part_outl(0x1800db50, 1, 1, 0); //[20]offset_pc_ee=1
	_phy_rtd_part_outl(0x1800db60, 1, 1, 0); //[20]offset_pc_ee=1
	_phy_rtd_part_outl(0x1800db70, 1, 1, 0); //[20]offset_pc_ee=1
	_phy_rtd_part_outl(0x1800db50, 11, 11, 0); //[20]offset_pc_eo=1
	_phy_rtd_part_outl(0x1800db60, 11, 11, 0); //[20]offset_pc_eo=1
	_phy_rtd_part_outl(0x1800db70, 11, 11, 0); //[20]offset_pc_eo=1
	_phy_rtd_part_outl(0x1800db50, 21, 21, 0); //[21]offfset_pc_eq=1
	_phy_rtd_part_outl(0x1800db60, 21, 21, 0); //[21]offfset_pc_eq=1
	_phy_rtd_part_outl(0x1800db70, 21, 21, 0); //[21]offfset_pc_eq=1
	_phy_rtd_part_outl(0x1800db48, 1, 1, 0); //[1]offset_pc_ope=0
	_phy_rtd_part_outl(0x1800db58, 1, 1, 0); //[1]offset_pc_ope=0
	_phy_rtd_part_outl(0x1800db68, 1, 1, 0); //[1]offset_pc_ope=0
	_phy_rtd_part_outl(0x1800db48, 11, 11, 0); //[11]offset_pc_opo=0
	_phy_rtd_part_outl(0x1800db58, 11, 11, 0); //[11]offset_pc_opo=0
	_phy_rtd_part_outl(0x1800db68, 11, 11, 0); //[11]offset_pc_opo=0
	//Enable B-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db4c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db5c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db6c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db4c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db5c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db6c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db50, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db60, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db70, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db50, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db60, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db70, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db48, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db58, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db68, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db48, 0, 0, 1); //[0]offset_en_ope=1
	_phy_rtd_part_outl(0x1800db58, 0, 0, 1); //[0]offset_en_ope=1
	_phy_rtd_part_outl(0x1800db68, 0, 0, 1); //[0]offset_en_ope=1
	ScalerTimer_DelayXms(1);
	//Check B Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0x8); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db44, 7, 4, 0x9); //reg_p0_b_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EOOffset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db44, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db44, 29, 29));
	//Check G Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0x8); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db54, 7, 4, 0x9); //reg_p0_g_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EOOffset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db54, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db54, 29, 29));
	//Check R Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0x8); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db64, 7, 4, 0x9); //reg_p0_r_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EOOffset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db64, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db64, 29, 29));
	//Disable B-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db4c, 10, 10, 0); //[10]offset_en_de=1->0
	_phy_rtd_part_outl(0x1800db5c, 10, 10, 0); //[10]offset_en_de=1->0
	_phy_rtd_part_outl(0x1800db6c, 10, 10, 0); //[10]offset_en_de=1->0
	_phy_rtd_part_outl(0x1800db4c, 20, 20, 0); //[20]offset_en_do=1->0
	_phy_rtd_part_outl(0x1800db5c, 20, 20, 0); //[20]offset_en_do=1->0
	_phy_rtd_part_outl(0x1800db6c, 20, 20, 0); //[20]offset_en_do=1->0
	_phy_rtd_part_outl(0x1800db50, 0, 0, 0); //[20]offset_en_ee=1->0
	_phy_rtd_part_outl(0x1800db60, 0, 0, 0); //[20]offset_en_ee=1->0
	_phy_rtd_part_outl(0x1800db70, 0, 0, 0); //[20]offset_en_ee=1->0
	_phy_rtd_part_outl(0x1800db50, 10, 10, 0); //[20]offset_en_eo=1->0
	_phy_rtd_part_outl(0x1800db60, 10, 10, 0); //[20]offset_en_eo=1->0
	_phy_rtd_part_outl(0x1800db70, 10, 10, 0); //[20]offset_en_eo=1->0
	_phy_rtd_part_outl(0x1800db48, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db58, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db68, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db48, 0, 0, 0); //[0]offset_en_ope=1->0
	_phy_rtd_part_outl(0x1800db58, 0, 0, 0); //[0]offset_en_ope=1->0
	_phy_rtd_part_outl(0x1800db68, 0, 0, 0); //[0]offset_en_ope=1->0
	//B-Lane Koffset result READ  *******************************************************************
	_phy_rtd_part_outl(0x1800db44, 7, 4, 0); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 1); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 2); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 3); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 4); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 5); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	//G-Lane Koffset result READ  *******************************************************************
	_phy_rtd_part_outl(0x1800db54, 7, 4, 0); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 1); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 2); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 3); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 4); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 5); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	//R-Lane Koffset result READ  *******************************************************************
	_phy_rtd_part_outl(0x1800db64, 7, 4, 0); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 1); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 2); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 3); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 4); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 5); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	//========================================= STEP2 LEQ KOFF =================================================
	_phy_rtd_part_outl(0x1800da54, 22, 22, Koffset_tmds_1[tmds_timing].B_7_140_1); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  0->1
	_phy_rtd_part_outl(0x1800da74, 22, 22, Koffset_tmds_1[tmds_timing].B_7_140_1_32); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  0->1
	_phy_rtd_part_outl(0x1800da94, 22, 22, Koffset_tmds_1[tmds_timing].B_7_140_1_64); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  0->1
	_phy_rtd_part_outl(0x1800da54, 23, 23, 1); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  0->1
	_phy_rtd_part_outl(0x1800da74, 23, 23, 1); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  0->1
	_phy_rtd_part_outl(0x1800da94, 23, 23, 1); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  0->1
	_phy_rtd_part_outl(0x1800da54, 24, 24, 1); //REG_B_8[0] POW_LEQ:  0->1
	_phy_rtd_part_outl(0x1800da74, 24, 24, 1); //REG_B_8[0] POW_LEQ:  0->1
	_phy_rtd_part_outl(0x1800da94, 24, 24, 1); //REG_B_8[0] POW_LEQ:  0->1
	_phy_rtd_part_outl(0x1800da58, 0, 0, 1); //REG_B_9[0] POW_CMFB_LEQ 0->1
	_phy_rtd_part_outl(0x1800da78, 0, 0, 1); //REG_B_9[0] POW_CMFB_LEQ 0->1
	_phy_rtd_part_outl(0x1800da98, 0, 0, 1); //REG_B_9[0] POW_CMFB_LEQ 0->1
	_phy_rtd_part_outl(0x1800db50, 19, 19, 0); //[19]z0_ok_eo=0
	_phy_rtd_part_outl(0x1800db60, 19, 19, 0); //[19]z0_ok_eo=0
	_phy_rtd_part_outl(0x1800db70, 19, 19, 0); //[19]z0_ok_eo=0
	_phy_rtd_part_outl(0x1800db50, 9, 9, 0); //[9] z0_ok_ee=0
	_phy_rtd_part_outl(0x1800db60, 9, 9, 0); //[9] z0_ok_ee=0
	_phy_rtd_part_outl(0x1800db70, 9, 9, 0); //[9] z0_ok_ee=0
	_phy_rtd_part_outl(0x1800db4c, 29, 29, 0); //[19]z0_ok_do=0
	_phy_rtd_part_outl(0x1800db5c, 29, 29, 0); //[19]z0_ok_do=0
	_phy_rtd_part_outl(0x1800db6c, 29, 29, 0); //[19]z0_ok_do=0
	_phy_rtd_part_outl(0x1800db50, 20, 20, 1); //[21]offset_en_eq=0->1
	_phy_rtd_part_outl(0x1800db60, 20, 20, 1); //[21]offset_en_eq=0->1
	_phy_rtd_part_outl(0x1800db70, 20, 20, 1); //[21]offset_en_eq=0->1
	ScalerTimer_DelayXms(1);
	HDMI_PHY_INFO("p0_b_offset_eq_ok_ro	EQ OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db44, 30, 30));
	HDMI_PHY_INFO("p0_b_offset_eq_ok_ro	EQ OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db54, 30, 30));
	HDMI_PHY_INFO("p0_b_offset_eq_ok_ro	EQ OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db64, 30, 30));
	_phy_rtd_part_outl(0x1800db50, 20, 20, 0); //[20]offset_en_eq=0
	_phy_rtd_part_outl(0x1800db60, 20, 20, 0); //[20]offset_en_eq=0
	_phy_rtd_part_outl(0x1800db70, 20, 20, 0); //[20]offset_en_eq=0
	//B-Lane LEQ Offset Result
	_phy_rtd_part_outl(0x1800db44, 7, 4, 8); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 8:EQ
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	//G-Lane LEQ Offset Result
	_phy_rtd_part_outl(0x1800db54, 7, 4, 8); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 8:EQ
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	//R-Lane LEQ Offset Result
	_phy_rtd_part_outl(0x1800db64, 7, 4, 8); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 8:EQ
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	//========================================= STEP3 All DCVS KOFF ==
	//===B/G/R LANE KOFF Data/Edge
	_phy_rtd_part_outl(0x1800db4c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db5c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db6c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db4c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db5c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db6c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db50, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db60, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db70, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db50, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db60, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db70, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db48, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db58, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db68, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db48, 0, 0, 1); //[0]offset_en_ope=1
	_phy_rtd_part_outl(0x1800db58, 0, 0, 1); //[0]offset_en_ope=1
	_phy_rtd_part_outl(0x1800db68, 0, 0, 1); //[0]offset_en_ope=1
	ScalerTimer_DelayXms(1);
	//Check Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0x8); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db44, 7, 4, 0x9); //reg_p0_b_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EOOffset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db44, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db44, 29, 29));
	//G-Lane DCVS Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0xA); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db54, 7, 4, 0x9); //reg_p0_g_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db54, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db54, 29, 29));
	//R-Lane DCVS Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0xC); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db64, 7, 4, 0x9); //reg_p0_r_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db64, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db64, 29, 29));
	//Disable B-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db4c, 10, 10, 0); //[10]offset_en_de=1->0
	_phy_rtd_part_outl(0x1800db5c, 10, 10, 0); //[10]offset_en_de=1->0
	_phy_rtd_part_outl(0x1800db6c, 10, 10, 0); //[10]offset_en_de=1->0
	_phy_rtd_part_outl(0x1800db4c, 20, 20, 0); //[20]offset_en_do=1->0
	_phy_rtd_part_outl(0x1800db5c, 20, 20, 0); //[20]offset_en_do=1->0
	_phy_rtd_part_outl(0x1800db6c, 20, 20, 0); //[20]offset_en_do=1->0
	_phy_rtd_part_outl(0x1800db50, 0, 0, 0); //[20]offset_en_ee=1->0
	_phy_rtd_part_outl(0x1800db60, 0, 0, 0); //[20]offset_en_ee=1->0
	_phy_rtd_part_outl(0x1800db70, 0, 0, 0); //[20]offset_en_ee=1->0
	_phy_rtd_part_outl(0x1800db50, 10, 10, 0); //[20]offset_en_eo=1->0
	_phy_rtd_part_outl(0x1800db60, 10, 10, 0); //[20]offset_en_eo=1->0
	_phy_rtd_part_outl(0x1800db70, 10, 10, 0); //[20]offset_en_eo=1->0
	_phy_rtd_part_outl(0x1800db48, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db58, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db68, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db48, 0, 0, 0); //[0]offset_en_ope=1->0
	_phy_rtd_part_outl(0x1800db58, 0, 0, 0); //[0]offset_en_ope=1->0
	_phy_rtd_part_outl(0x1800db68, 0, 0, 0); //[0]offset_en_ope=1->0
	//B-Lane LEQ & DCVS Offset Result
	_phy_rtd_part_outl(0x1800db44, 7, 4, 0); //reg_p0_b_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 1); //reg_p0_b_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 2); //reg_p0_b_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 3); //reg_p0_b_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 8); //reg_p0_b_offset_coef_sel[4:0]
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 4); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 5); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	//G-Lane LEQ & DCVS Offset Result
	_phy_rtd_part_outl(0x1800db54, 7, 4, 0); //reg_p0_g_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 1); //reg_p0_g_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 2); //reg_p0_g_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 3); //reg_p0_g_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 8); //reg_p0_g_offset_coef_sel[4:0]
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 4); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 5); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	//R-Lane LEQ & DCVS Offset Result
	_phy_rtd_part_outl(0x1800db64, 7, 4, 0); //reg_p0_r_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 1); //reg_p0_r_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 2); //reg_p0_r_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 3); //reg_p0_r_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 8); //reg_p0_r_offset_coef_sel[4:0]
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 4); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 5); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	//Input on
	_phy_rtd_part_outl(0x1800da50, 0, 0, 0); //INOFF_EN =0 Input_on
	_phy_rtd_part_outl(0x1800da70, 0, 0, 0); //INOFF_EN =0 Input_on
	_phy_rtd_part_outl(0x1800da90, 0, 0, 0); //INOFF_EN =0 Input_on
	_phy_rtd_part_outl(0x1800da58, 28, 28, 0); //#B  DFE_TAP0_ICOM_EN=0
	_phy_rtd_part_outl(0x1800da5c, 6, 3, Koffset_tmds_1[tmds_timing].B_13_251_1); //#B  DFE_TAP_EN[4:1] = 0xf
	_phy_rtd_part_outl(0x1800da78, 28, 28, 0); //#G  DFE_TAP0_ICOM_EN=0
	_phy_rtd_part_outl(0x1800da7c, 6, 3, Koffset_tmds_1[tmds_timing].G_13_253_1); //#G  DFE_TAP_EN[4:1] = 0xf
	_phy_rtd_part_outl(0x1800da98, 28, 28, 0); //#R  DFE_TAP0_ICOM_EN=0
	_phy_rtd_part_outl(0x1800da9c, 6, 3, Koffset_tmds_1[tmds_timing].R_13_255_1); //#R  DFE_TAP_EN[4:1] = 0xf
	_phy_rtd_part_outl(0x1800dc04, 0, 0, 0); //R finetunstart on
	_phy_rtd_part_outl(0x1800dbe8, 0, 0, 0); //G finetunstart on
	_phy_rtd_part_outl(0x1800dbc8, 0, 0, 0); //B finetunstart on
	_phy_rtd_part_outl(0x1800db00, 0, 0, 0x0);
	_phy_rtd_part_outl(0x1800db00, 1, 1, 0x0);
	_phy_rtd_part_outl(0x1800db00, 2, 2, 0x0);
	_phy_rtd_part_outl(0x1800db00, 3, 3, 0x0);
	_phy_rtd_part_outl(0x1800db00, 4, 4, 0x0);
	_phy_rtd_part_outl(0x1800db00, 5, 5, 0x0);
	_phy_rtd_part_outl(0x1800db00, 6, 6, 0x0);
	_phy_rtd_part_outl(0x1800db00, 7, 7, 0x0);
	_phy_rtd_part_outl(0x1800db00, 0, 0, 0x1);
	_phy_rtd_part_outl(0x1800db00, 1, 1, 0x1);
	_phy_rtd_part_outl(0x1800db00, 2, 2, 0x1);
	_phy_rtd_part_outl(0x1800db00, 3, 3, 0x1);
	_phy_rtd_part_outl(0x1800db00, 4, 4, 0x1);
	_phy_rtd_part_outl(0x1800db00, 5, 5, 0x1);
	_phy_rtd_part_outl(0x1800db00, 6, 6, 0x1);
	_phy_rtd_part_outl(0x1800db00, 7, 7, 0x1);	
}

void LEQ_VTH_Tap0_3_4_Adapt_tmds_1_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	_phy_rtd_part_outl(0x1800dcb0, 15, 15, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 14, 14, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 13, 13, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 12, 12, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 11, 11, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 10, 10, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 9, 9, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 8, 8, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800de08, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de08, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1	
	_phy_rtd_part_outl(0x1800dc70, 0, 0, 1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 1, 1, 1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 2, 2, 1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 3, 3, 1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 4, 4, 1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 5, 5, 1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 6, 6, 1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 7, 7, 1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 12, 12, 1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 13, 13, 1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 14, 14, 1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 15, 15, 1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 8, 8, 1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc70, 9, 9, 1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc70, 10, 10, 1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc70, 11, 11, 1); //dfe_en=1
	//DFE_Adaptation
	//===CK LANE TAP0 & LEQ
	_phy_rtd_outl(0x1800decc, 0x00005400); //Timer, set eqfe_en,servo_en,timer_en=1
	//===B LANE TAP0 & LEQ
	_phy_rtd_outl(0x1800de8c, 0x00005400); //Timer, set eqfe_en,servo_en,timer_en=1
	//===G LANE TAP0 & LEQ
	_phy_rtd_outl(0x1800de4c, 0x00005400); //Timer, set eqfe_en,servo_en,timer_en=1
	//===R LANE TAP0 & LEQ
	_phy_rtd_outl(0x1800de0c, 0x00005400); //Timer, set eqfe_en,servo_en,timer_en=1
	_phy_rtd_part_outl(0x1800de38, 23, 23, 1); //Inverse vth sign polarity
	_phy_rtd_part_outl(0x1800de78, 23, 23, 1); //Inverse vth sign polarity
	_phy_rtd_part_outl(0x1800deb8, 23, 23, 1); //Inverse vth sign polarity
	_phy_rtd_part_outl(0x1800def8, 23, 23, 1); //Inverse vth sign polarity
	_phy_rtd_part_outl(0x1800de10, 31, 31, 1); //leq1_inv=1
	_phy_rtd_part_outl(0x1800de50, 31, 31, 1); //leq1_inv=1
	_phy_rtd_part_outl(0x1800de90, 31, 31, 1); //leq1_inv=1
	_phy_rtd_part_outl(0x1800ded0, 31, 31, 1); //leq1_inv=1
	_phy_rtd_part_outl(0x1800de10, 30, 30, 1); //leq2_inv=0
	_phy_rtd_part_outl(0x1800de50, 30, 30, 1); //leq2_inv=0
	_phy_rtd_part_outl(0x1800de90, 30, 30, 1); //leq2_inv=0
	_phy_rtd_part_outl(0x1800ded0, 30, 30, 1); //leq2_inv=0
	_phy_rtd_part_outl(0x1800de14, 17, 15, 0); //LEQ_GAIN1=0
	_phy_rtd_part_outl(0x1800de54, 17, 15, 0); //LEQ_GAIN1=0
	_phy_rtd_part_outl(0x1800de94, 17, 15, 0); //LEQ_GAIN1=0
	_phy_rtd_part_outl(0x1800ded4, 17, 15, 0); //LEQ_GAIN1=0
	_phy_rtd_part_outl(0x1800de14, 14, 12, 0); //LEQ_GAIN2=0
	_phy_rtd_part_outl(0x1800de54, 14, 12, 0); //LEQ_GAIN2=0
	_phy_rtd_part_outl(0x1800de94, 14, 12, 0); //LEQ_GAIN2=0
	_phy_rtd_part_outl(0x1800ded4, 14, 12, 0); //LEQ_GAIN2=0
	_phy_rtd_part_outl(0x1800de18, 22, 21, 1); //LEQ1_trans_mode=1
	_phy_rtd_part_outl(0x1800de58, 22, 21, 1); //LEQ1_trans_mode=1
	_phy_rtd_part_outl(0x1800de98, 22, 21, 1); //LEQ1_trans_mode=1
	_phy_rtd_part_outl(0x1800ded8, 22, 21, 1); //LEQ1_trans_mode=1
	_phy_rtd_part_outl(0x1800de18, 20, 19, 3); //LEQ2_trans_mode=3
	_phy_rtd_part_outl(0x1800de58, 20, 19, 3); //LEQ2_trans_mode=3
	_phy_rtd_part_outl(0x1800de98, 20, 19, 3); //LEQ2_trans_mode=3
	_phy_rtd_part_outl(0x1800ded8, 20, 19, 3); //LEQ2_trans_mode=3
	_phy_rtd_part_outl(0x1800de18, 28, 26, 4); //tap0_gain=4
	_phy_rtd_part_outl(0x1800de58, 28, 26, 4); //tap0_gain=4
	_phy_rtd_part_outl(0x1800de98, 28, 26, 4); //tap0_gain=4
	_phy_rtd_part_outl(0x1800ded8, 28, 26, 4); //tap0_gain=4
	_phy_rtd_part_outl(0x1800de18, 25, 23, 4); //tap0_gain=4
	_phy_rtd_part_outl(0x1800de58, 25, 23, 4); //tap0_gain=4
	_phy_rtd_part_outl(0x1800de98, 25, 23, 4); //tap0_gain=4
	_phy_rtd_part_outl(0x1800ded8, 25, 23, 4); //tap0_gain=4
	_phy_rtd_part_outl(0x1800de0c, 26, 24, 5); //20200824_tap_timer=5
	_phy_rtd_part_outl(0x1800de4c, 26, 24, 5); //20200824_tap_timer=5
	_phy_rtd_part_outl(0x1800de8c, 26, 24, 5); //20200824_tap_timer=5
	_phy_rtd_part_outl(0x1800decc, 26, 24, 5); //20200824_tap_timer=5
	_phy_rtd_part_outl(0x1800de0c, 20, 18, 5); //20200825_LEQ_timer=5
	_phy_rtd_part_outl(0x1800de4c, 20, 18, 5); //20200825_LEQ_timer=5
	_phy_rtd_part_outl(0x1800de8c, 20, 18, 5); //20200825_LEQ_timer=5
	_phy_rtd_part_outl(0x1800decc, 20, 18, 5); //20200825_LEQ_timer=5
	_phy_rtd_part_outl(0x1800de0c, 23, 21, 5); //20200825_VTH_timer=5
	_phy_rtd_part_outl(0x1800de4c, 23, 21, 5); //20200825_VTH_timer=5
	_phy_rtd_part_outl(0x1800de8c, 23, 21, 5); //20200825_VTH_timer=5
	_phy_rtd_part_outl(0x1800decc, 23, 21, 5); //20200825_VTH_timer=5
	_phy_rtd_part_outl(0x1800de0c, 13, 13, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].EQFE_EN_R_48_1); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de4c, 13, 13, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].EQFE_EN_R_48_1_64); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de8c, 13, 13, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].EQFE_EN_R_48_1_128); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800decc, 13, 13, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].EQFE_EN_R_48_1_192); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de28, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de68, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dea8, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dee8, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de28, 26, 26, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_50_1); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800de68, 26, 26, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_50_1_64); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800dea8, 26, 26, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_50_1_128); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800dee8, 26, 26, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_50_1_192); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800de28, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800de68, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800dea8, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800dee8, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800de28, 28, 28, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_52_1); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800de68, 28, 28, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_52_1_64); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800dea8, 28, 28, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_52_1_128); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800dee8, 28, 28, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_52_1_192); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800de28, 29, 29, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_53_1); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800de68, 29, 29, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_53_1_64); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800dea8, 29, 29, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_53_1_128); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800dee8, 29, 29, LEQ_VTH_Tap0_3_4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_53_1_192); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800de08, 22, 22, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 22, 22, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 22, 22, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 22, 22, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de08, 23, 23, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 23, 23, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 23, 23, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 23, 23, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de1c, 31, 30, 0x1); //tap0_adjust_b =1
	_phy_rtd_part_outl(0x1800de5c, 31, 30, 0x1); //tap0_adjust_b =1
	_phy_rtd_part_outl(0x1800de9c, 31, 30, 0x1); //tap0_adjust_b =1
	_phy_rtd_part_outl(0x1800dedc, 31, 30, 0x1); //tap0_adjust_b =1
	ScalerTimer_DelayXms(5);
	//LEQ_DFE adapt
	_phy_rtd_part_outl(0x1800de0c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de4c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de8c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800decc, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de28, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800de68, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800dea8, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800dee8, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800de28, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800de68, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800dea8, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800dee8, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800de28, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800de68, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800dea8, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800dee8, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800de28, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800de68, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800dea8, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800dee8, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800de08, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=0
	_phy_rtd_part_outl(0x1800de48, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=0
	_phy_rtd_part_outl(0x1800de88, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=0
	_phy_rtd_part_outl(0x1800dec8, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=0
	_phy_rtd_part_outl(0x1800de08, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de28, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de68, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dea8, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dee8, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
}

void Tap0_to_Tap4_Adapt_tmds_1_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	_phy_rtd_part_outl(0x1800dcb0, 15, 15, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 14, 14, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 13, 13, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 12, 12, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 11, 11, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 10, 10, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 9, 9, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 8, 8, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dc70, 0, 0, 1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 1, 1, 1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 2, 2, 1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 3, 3, 1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 4, 4, 1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 5, 5, 1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 6, 6, 1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 7, 7, 1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 12, 12, 1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 13, 13, 1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 14, 14, 1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 15, 15, 1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 8, 8, 1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc70, 9, 9, 1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc70, 10, 10, 1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc70, 11, 11, 1); //dfe_en=1
	//R-Lane Tap0-4 Adapt
	_phy_rtd_part_outl(0x1800de0c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de4c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de8c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800decc, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de28, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de68, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dea8, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dee8, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de28, 26, 26, 1); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800de68, 26, 26, 1); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800dea8, 26, 26, 1); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800dee8, 26, 26, 1); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800de28, 27, 27, Tap0_to_Tap4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_29_1); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800de68, 27, 27, Tap0_to_Tap4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_29_1_64); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800dea8, 27, 27, Tap0_to_Tap4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_29_1_128); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800dee8, 27, 27, Tap0_to_Tap4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_29_1_192); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800de28, 28, 28, Tap0_to_Tap4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_30_1); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800de68, 28, 28, Tap0_to_Tap4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_30_1_64); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800dea8, 28, 28, Tap0_to_Tap4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_30_1_128); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800dee8, 28, 28, Tap0_to_Tap4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_30_1_192); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800de28, 29, 29, Tap0_to_Tap4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_31_1); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800de68, 29, 29, Tap0_to_Tap4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_31_1_64); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800dea8, 29, 29, Tap0_to_Tap4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_31_1_128); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800dee8, 29, 29, Tap0_to_Tap4_Adapt_tmds_1[tmds_timing].DFE_ADAPT_EN_R_6_0_31_1_192); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800de08, 22, 22, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 22, 22, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 22, 22, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 22, 22, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de08, 23, 23, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 23, 23, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 23, 23, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 23, 23, 1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	ScalerTimer_DelayXms(5);	
	//CK/B/G/R-Lane Tap0-4 Adapt
	_phy_rtd_part_outl(0x1800de0c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de4c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de8c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800decc, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de28, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de68, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dea8, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dee8, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de28, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=1
	_phy_rtd_part_outl(0x1800de68, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=1
	_phy_rtd_part_outl(0x1800dea8, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=1
	_phy_rtd_part_outl(0x1800dee8, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=1
	_phy_rtd_part_outl(0x1800de28, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=1
	_phy_rtd_part_outl(0x1800de68, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=1
	_phy_rtd_part_outl(0x1800dea8, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=1
	_phy_rtd_part_outl(0x1800dee8, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=1
	_phy_rtd_part_outl(0x1800de28, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800de68, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800dea8, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800dee8, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800de28, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800de68, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800dea8, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800dee8, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800de08, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de08, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
}

void DCDR_settings_tmds_3_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	_phy_rtd_part_outl(0x1800db00, 0, 0, 0x0);
	_phy_rtd_part_outl(0x1800db00, 1, 1, 0x0);
	_phy_rtd_part_outl(0x1800db00, 2, 2, 0x0);
	_phy_rtd_part_outl(0x1800db00, 3, 3, 0x0);
	_phy_rtd_part_outl(0x1800db00, 4, 4, 0x0);
	_phy_rtd_part_outl(0x1800db00, 5, 5, 0x0);
	_phy_rtd_part_outl(0x1800db00, 6, 6, 0x0);
	_phy_rtd_part_outl(0x1800db00, 7, 7, 0x0);
	_phy_rtd_part_outl(0x1800db00, 0, 0, 0x1);
	_phy_rtd_part_outl(0x1800db00, 1, 1, 0x1);
	_phy_rtd_part_outl(0x1800db00, 2, 2, 0x1);
	_phy_rtd_part_outl(0x1800db00, 3, 3, 0x1);
	_phy_rtd_part_outl(0x1800db00, 4, 4, 0x1);
	_phy_rtd_part_outl(0x1800db00, 5, 5, 0x1);
	_phy_rtd_part_outl(0x1800db00, 6, 6, 0x1);
	_phy_rtd_part_outl(0x1800db00, 7, 7, 0x1);
	//DFE Adaptation power off
	_phy_rtd_part_outl(0x1800da3c, 7, 7, 0); //Mark2_Modify; CK-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da5c, 7, 7, 0); //Mark2_Modify; B-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da7c, 7, 7, 0); //Mark2_Modify; G-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da9c, 7, 7, 0); //Mark2_Modify; R-Lane DFE_ADAPT_EN
	ScalerTimer_DelayXms(1);
	_phy_rtd_part_outl(0x1800db88, 4, 4, 0x1); //fifo_en
	_phy_rtd_part_outl(0x1800db88, 12, 12, 0x1); //fifo_en
	_phy_rtd_part_outl(0x1800db88, 20, 20, 0x1); //fifo_en
	_phy_rtd_part_outl(0x1800db88, 28, 28, 0x1); //fifo_en
}

void Manual_DFE_tmds_1_func(unsigned char nport, TMDS_TIMING_PARAM_T tmds_timing)
{
	// *************************************************************************************************************************************
	//B-Lane_initial DFE Value_Start
	_phy_rtd_part_outl(0x1800dcb0, 15, 15, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 14, 14, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 13, 13, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 12, 12, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 11, 11, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 10, 10, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 9, 9, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 8, 8, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dea8, 9, 5, 5); //VTHP_INIT_B[4:0]	9:5	R/W	0x0F		VTHP INIT
	_phy_rtd_part_outl(0x1800dea8, 4, 0, 5); //VTHN_INIT_B[4:0]	4:0	R/W	0x0F		VTHN INIT
	_phy_rtd_part_outl(0x1800dea0, 4, 0, 0); //LEQ_INIT_B[4:0]	4:0		R/W	0x00	LEQ INIT
	_phy_rtd_part_outl(0x1800dea4, 31, 24, 25); //TAP0_INIT_B[7:0]	31:24	R/W	0x00	TAP0 INIT
	_phy_rtd_part_outl(0x1800dea4, 23, 18, 0); //TAP1_INIT_B[5:0]	23:18	R/W	0x00	TAP1 INIT
	_phy_rtd_part_outl(0x1800dea4, 17, 12, 0); //TAP2_INIT_B[5:0]	17:12	R/W	0x00	TAP2 INIT
	_phy_rtd_part_outl(0x1800dea4, 11, 6, 0); //TAP3_INIT_B[5:0]	11:6	R/W	0x00	TAP3 INIT
	_phy_rtd_part_outl(0x1800dea4, 5, 0, 0); //TAP4_INIT_B[5:0]	5:0		R/W	0x00	TAP4 INIT
	//B-Lane DFE COEF LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 0, 0, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP0 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 1, 1, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP1 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 2, 2, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP2 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 3, 3, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP3 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 4, 4, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP4 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 5, 5, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de88, 31, 31, 1); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 4, 0, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 5, 5, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de88, 31, 31, 0x0); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	//B-Lane_initial DFE Value_Stop
	//**************************************************************************************************************************************
	// *************************************************************************************************************************************
	//G-Lane_initial DFE Value_Start
	_phy_rtd_part_outl(0x1800de68, 9, 5, 5); //VTHP_INIT_B[4:0]	9:5	R/W	0x0F		VTHP INIT
	_phy_rtd_part_outl(0x1800de68, 4, 0, 5); //VTHN_INIT_B[4:0]	4:0	R/W	0x0F		VTHN INIT
	_phy_rtd_part_outl(0x1800de60, 4, 0, 0); //LEQ_INIT_B[4:0]	4:0		R/W	0x00	LEQ INIT
	_phy_rtd_part_outl(0x1800de64, 31, 24, 25); //TAP0_INIT_B[7:0]	31:24	R/W	0x00	TAP0 INIT
	_phy_rtd_part_outl(0x1800de64, 23, 18, 0); //TAP1_INIT_B[5:0]	23:18	R/W	0x00	TAP1 INIT
	_phy_rtd_part_outl(0x1800de64, 17, 12, 0); //TAP2_INIT_B[5:0]	17:12	R/W	0x00	TAP2 INIT
	_phy_rtd_part_outl(0x1800de64, 11, 6, 0); //TAP3_INIT_B[5:0]	11:6	R/W	0x00	TAP3 INIT
	_phy_rtd_part_outl(0x1800de64, 5, 0, 0); //TAP4_INIT_B[5:0]	5:0		R/W	0x00	TAP4 INIT
	//G-Lane DFE COEF LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 0, 0, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP0 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 1, 1, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP1 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 2, 2, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP2 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 3, 3, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP3 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 4, 4, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP4 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 5, 5, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de48, 31, 31, 1); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 4, 0, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 5, 5, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de48, 31, 31, 0x0); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	//G-Lane_initial DFE Value_Stop
	//**************************************************************************************************************************************
	// *************************************************************************************************************************************
	//R-Lane_initial DFE Value_Start
	_phy_rtd_part_outl(0x1800de28, 9, 5, 5); //VTHP_INIT_B[4:0]	9:5	R/W	0x0F		VTHP INIT
	_phy_rtd_part_outl(0x1800de28, 4, 0, 5); //VTHN_INIT_B[4:0]	4:0	R/W	0x0F		VTHN INIT
	_phy_rtd_part_outl(0x1800de20, 4, 0, 0); //LEQ_INIT_B[4:0]	4:0		R/W	0x00	LEQ INIT
	_phy_rtd_part_outl(0x1800de24, 31, 24, 25); //TAP0_INIT_B[7:0]	31:24	R/W	0x00	TAP0 INIT
	_phy_rtd_part_outl(0x1800de24, 23, 18, 0); //TAP1_INIT_B[5:0]	23:18	R/W	0x00	TAP1 INIT
	_phy_rtd_part_outl(0x1800de24, 17, 12, 0); //TAP2_INIT_B[5:0]	17:12	R/W	0x00	TAP2 INIT
	_phy_rtd_part_outl(0x1800de24, 11, 6, 0); //TAP3_INIT_B[5:0]	11:6	R/W	0x00	TAP3 INIT
	_phy_rtd_part_outl(0x1800de24, 5, 0, 0); //TAP4_INIT_B[5:0]	5:0		R/W	0x00	TAP4 INIT
	//R-Lane DFE COEF LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 0, 0, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP0 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 1, 1, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP1 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 2, 2, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP2 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 3, 3, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP3 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 4, 4, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP4 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 5, 5, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de08, 31, 31, 1); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 4, 0, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 5, 5, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de08, 31, 31, 0x0); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	//R-Lane_initial DFE Value_Stop
	//**************************************************************************************************************************************
	// *************************************************************************************************************************************
	//CK-Lane_initial DFE Value_Start
	_phy_rtd_part_outl(0x1800dee8, 9, 5, 5); //VTHP_INIT_B[4:0]	9:5	R/W	0x0F		VTHP INIT
	_phy_rtd_part_outl(0x1800dee8, 4, 0, 5); //VTHN_INIT_B[4:0]	4:0	R/W	0x0F		VTHN INIT
	_phy_rtd_part_outl(0x1800dee0, 4, 0, 0); //LEQ_INIT_B[4:0]	4:0		R/W	0x00	LEQ INIT
	_phy_rtd_part_outl(0x1800dee4, 31, 24, 25); //TAP0_INIT_B[7:0]	31:24	R/W	0x00	TAP0 INIT
	_phy_rtd_part_outl(0x1800dee4, 23, 18, 0); //TAP1_INIT_B[5:0]	23:18	R/W	0x00	TAP1 INIT
	_phy_rtd_part_outl(0x1800dee4, 17, 12, 0); //TAP2_INIT_B[5:0]	17:12	R/W	0x00	TAP2 INIT
	_phy_rtd_part_outl(0x1800dee4, 11, 6, 0); //TAP3_INIT_B[5:0]	11:6	R/W	0x00	TAP3 INIT
	_phy_rtd_part_outl(0x1800dee4, 5, 0, 0); //TAP4_INIT_B[5:0]	5:0		R/W	0x00	TAP4 INIT
	//CK-Lane DFE COEF LOAD IN
	_phy_rtd_part_outl(0x1800decc, 0, 0, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP0 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 1, 1, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP1 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 2, 2, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP2 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 3, 3, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP3 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 4, 4, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP4 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 5, 5, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800dec8, 31, 31, 1); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800decc, 4, 0, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP LOAD IN
	_phy_rtd_part_outl(0x1800decc, 5, 5, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800dec8, 31, 31, 0x0); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800def4, 17, 15, 3); //Adaption flow control mode
	_phy_rtd_part_outl(0x1800deb4, 17, 15, 3); //Adaption flow control mode
	_phy_rtd_part_outl(0x1800de74, 17, 15, 3); //Adaption flow control mode
	_phy_rtd_part_outl(0x1800de34, 17, 15, 3); //Adaption flow control mode
	//CK-Lane_initial DFE Value_Stop
	//**************************************************************************************************************************************
}

#endif


/////////////////////////////////////////////



#if 1
finetunestart_on_T finetunestart_on[] =
{
	{FRL_12G,0x1},
	{FRL_10G,0x1},
	{FRL_8G,0x1},
	{FRL_6G,0x1},
	{FRL_3G,0x0},
};

APHY_Para_frl_1_T APHY_Para_frl_1[] =
{
	{FRL_12G,0x3,0x3,0x3,0x1,0xb,0xb,0x3,0x3,0x2,0xf,0xf,0xc,0x0,0x0,0x1,0x0,0x1,0x0,0x0,0x1,0x3,0x1,0x6b,0x1,0x3,0x3,0x1,0x3,0x3,0x3,0x1,0xb,0xb,0x3,0x3,0x2,0xf,0xf,0xc,0x0,0x0,0x1,0x0,0x1,0x0,0x0,0x1,0x3,0x1,0x6b,0x1,0x3,0x3,0x1,0x3,0x3,0x3,0x1,0xb,0xb,0x3,0x3,0x2,0xf,0xf,0xc,0x0,0x0,0x1,0x0,0x1,0x0,0x0,0x1,0x3,0x1,0x6b,0x1,0x3,0x3,0x1,0x3,0x3,0x3,0x1,0xb,0xb,0x3,0x3,0x2,0xf,0xf,0xc,0x0,0x0,0x1,0x0,0x1,0x0,0x0,0x1,0x3,0x1,0x6b,0x1,0x3,0x3,0x1},	
	{FRL_10G,0x2,0x3,0x3,0x1,0xd,0xd,0x1,0x3,0x2,0xf,0xf,0xd,0x0,0x0,0x1,0x0,0x1,0x0,0x1,0x1,0x2,0x2,0xb5,0x0,0x2,0x3,0x1,0x2,0x3,0x3,0x1,0xd,0xd,0x1,0x3,0x2,0xf,0xf,0xd,0x0,0x0,0x1,0x0,0x1,0x0,0x1,0x1,0x2,0x2,0xb5,0x0,0x2,0x3,0x1,0x2,0x3,0x3,0x1,0xd,0xd,0x1,0x3,0x2,0xf,0xf,0xd,0x0,0x0,0x1,0x0,0x1,0x0,0x1,0x1,0x2,0x2,0xb5,0x0,0x2,0x3,0x1,0x2,0x3,0x3,0x1,0xd,0xd,0x1,0x3,0x2,0xf,0xf,0xd,0x0,0x0,0x1,0x0,0x1,0x0,0x1,0x1,0x2,0x2,0xb5,0x0,0x2,0x3,0x1},
	{FRL_8G,0x2,0x3,0x3,0x1,0xc,0xc,0x1,0x3,0x2,0xf,0xf,0xd,0x0,0x0,0x1,0x0,0x1,0x0,0x0,0x1,0x0,0x3,0x90,0x0,0x2,0x3,0x1,0x2,0x3,0x3,0x1,0xc,0xc,0x1,0x3,0x2,0xf,0xf,0xd,0x0,0x0,0x1,0x0,0x1,0x0,0x0,0x1,0x0,0x3,0x90,0x0,0x2,0x3,0x1,0x2,0x3,0x3,0x1,0xc,0xc,0x1,0x3,0x2,0xf,0xf,0xd,0x0,0x0,0x1,0x0,0x1,0x0,0x0,0x1,0x0,0x3,0x90,0x0,0x2,0x3,0x1,0x2,0x3,0x3,0x1,0xc,0xc,0x1,0x3,0x2,0xf,0xf,0xd,0x0,0x0,0x1,0x0,0x1,0x0,0x0,0x1,0x0,0x3,0x90,0x0,0x2,0x3,0x1},
	{FRL_6G,0x2,0x3,0x1,0x1,0x7,0x5,0x2,0x3,0x1,0xd,0xf,0xf,0x1,0x0,0x1,0x1,0x0,0x1,0x1,0x1,0x3,0x2,0xda,0x0,0x2,0x0,0x0,0x2,0x3,0x1,0x1,0x7,0x5,0x2,0x3,0x1,0xd,0xf,0xf,0x1,0x0,0x1,0x1,0x0,0x1,0x1,0x1,0x3,0x2,0xda,0x0,0x2,0x0,0x0,0x2,0x3,0x1,0x1,0x7,0x5,0x2,0x3,0x1,0xd,0xf,0xf,0x1,0x0,0x1,0x1,0x0,0x1,0x1,0x1,0x3,0x2,0xda,0x0,0x2,0x0,0x0,0x2,0x3,0x1,0x1,0x7,0x5,0x2,0x3,0x1,0xd,0xf,0xf,0x1,0x0,0x1,0x1,0x0,0x1,0x1,0x1,0x3,0x2,0xda,0x0,0x2,0x0,0x0},
	{FRL_3G,0x1,0x1,0x0,0x0,0x3,0x3,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x1,0x0,0x1,0x1,0x0,0x3,0x3,0xda,0x0,0x1,0x0,0x0,0x1,0x1,0x0,0x0,0x3,0x3,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x1,0x0,0x1,0x1,0x0,0x3,0x3,0xda,0x0,0x1,0x0,0x0,0x1,0x1,0x0,0x0,0x3,0x3,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x1,0x0,0x1,0x1,0x0,0x3,0x3,0xda,0x0,0x1,0x0,0x0,0x1,0x1,0x0,0x0,0x3,0x3,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x1,0x0,0x1,0x1,0x0,0x3,0x3,0xda,0x0,0x1,0x0,0x0},
};

DPHY_Init_Flow_frl_1_T DPHY_Init_Flow_frl_1[] =
{
	{FRL_12G,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356},
	{FRL_10G,0x2a2,0x2cc,0x2a2,0x2cc,0x2a2,0x2cc,0x2a2,0x2cc,0x2a2,0x2cc,0x2a2,0x2cc,0x2a2,0x2cc,0x2a2,0x2cc},
	{FRL_8G,0x215,0x243,0x215,0x243,0x215,0x243,0x215,0x243,0x215,0x243,0x215,0x243,0x215,0x243,0x215,0x243},
	{FRL_6G,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356},
	{FRL_3G,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356,0x32c,0x356},
};

DPHY_Para_frl_1_T DPHY_Para_frl_1[] =
{
	{FRL_12G,0x77740,0x77740,0x77740,0x77740,0x0,0x0,0x0,0x0},
	{FRL_10G,0x77750,0x77750,0x77750,0x77750,0x0,0x0,0x0,0x0},
	{FRL_8G,0x77750,0x77750,0x77750,0x77750,0x0,0x0,0x0,0x0},
	{FRL_6G,0x77770,0x77770,0x77770,0x77770,0x1,0x1,0x1,0x1},
	{FRL_3G,0x77760,0x77760,0x77760,0x77760,0x1,0x1,0x1,0x1},
};

Koffset_frl_1_T Koffset_frl_1[] =
{
	{FRL_12G,0x3,0x3,0x3,0x3,0xf,0xf,0xf,0xf},
	{FRL_10G,0x3,0x3,0x3,0x3,0xf,0xf,0xf,0xf},
	{FRL_8G,0x3,0x3,0x3,0x3,0xf,0xf,0xf,0xf},
	{FRL_6G,0x3,0x3,0x3,0x3,0xf,0xf,0xf,0xf},
	{FRL_3G,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x3},
};

LEQ_VTH_Tap0_3_4_Adapt_frl_1_T LEQ_VTH_Tap0_3_4_Adapt_frl_1[] =
{
	{FRL_12G,0x2,0x1,0x1,0x2,0x1,0x1,0x2,0x1,0x1,0x2,0x1,0x1,0x1},
	{FRL_10G,0x2,0x1,0x1,0x2,0x1,0x1,0x2,0x1,0x1,0x2,0x1,0x1,0x1},
	{FRL_8G,0x2,0x1,0x1,0x2,0x1,0x1,0x2,0x1,0x1,0x2,0x1,0x1,0x1},
	{FRL_6G,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{FRL_3G,0x1,0x0,0x0,0x1,0x0,0x0,0x1,0x0,0x0,0x1,0x0,0x0,0x0},
};

Tap0_to_Tap4_Adapt_frl_1_T Tap0_to_Tap4_Adapt_frl_1[] =
{
	{FRL_12G,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{FRL_10G,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{FRL_8G,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{FRL_6G,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
	{FRL_3G,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
};

void FRL_Main_Seq(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	HDMI_EMG("Version:Merlin8_HDMI_FRL_setting_0510 \n");
	DFE_ini_frl_1_func(nport, frl_timing);
	DPHY_Fix_frl_1_func(nport, frl_timing);
	DPHY_Para_frl_1_func(nport, frl_timing);
	APHY_Fix_frl_1_func(nport, frl_timing);
	APHY_Para_frl_1_func(nport, frl_timing);
	DPHY_Init_Flow_frl_1_func(nport, frl_timing);
	APHY_Init_Flow_frl_1_func(nport, frl_timing);
	ACDR_settings_frl_1_func(nport, frl_timing);
	ACDR_settings_frl_2_func(nport, frl_timing);
	Koffset_frl_1_func(nport, frl_timing);
	LEQ_VTH_Tap0_3_4_Adapt_frl_1_func(nport, frl_timing);
	Koffset_frl_1_func(nport, frl_timing);
	Tap0_to_Tap4_Adapt_frl_1_func(nport, frl_timing);
	Koffset_frl_1_func(nport, frl_timing);
	ACDR_settings_frl_3_func(nport, frl_timing);
	ACDR_settings_frl_4_func(nport, frl_timing);
}

void DFE_ini_frl_1_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	_phy_rtd_outl(0x1800dc70, 0x00000000);
	//R lane                  
	_phy_rtd_outl(0x1800de00, 0x00000000);
	_phy_rtd_outl(0x1800de04, 0x00000000);
	_phy_rtd_outl(0x1800de08, 0x20C00000);
	_phy_rtd_outl(0x1800de0c, 0x05b47c00);
	_phy_rtd_outl(0x1800de10, 0xc0004000);
	_phy_rtd_outl(0x1800de14, 0x00280000);
	_phy_rtd_outl(0x1800de18, 0x1238ffe0);
	_phy_rtd_outl(0x1800de1c, 0x7FF07FE0);
	_phy_rtd_outl(0x1800de20, 0x3FFF83E0);
	_phy_rtd_outl(0x1800de24, 0x00000000);
	_phy_rtd_outl(0x1800de28, 0x009C00E7);
	_phy_rtd_outl(0x1800de2c, 0xFF00FFC0);
	_phy_rtd_outl(0x1800de30, 0xff005400);
	//G lane                  
	_phy_rtd_outl(0x1800de40, 0x00000000);
	_phy_rtd_outl(0x1800de44, 0x00000000);
	_phy_rtd_outl(0x1800de48, 0x20C00000);
	_phy_rtd_outl(0x1800de4c, 0x05b47c00);
	_phy_rtd_outl(0x1800de50, 0xc0004000);
	_phy_rtd_outl(0x1800de54, 0x00280000);
	_phy_rtd_outl(0x1800de58, 0x1238ffe0);
	_phy_rtd_outl(0x1800de5c, 0x7FF07FE0);
	_phy_rtd_outl(0x1800de60, 0x3FFF83E0);
	_phy_rtd_outl(0x1800de64, 0x00000000);
	_phy_rtd_outl(0x1800de68, 0x009C00E7);
	_phy_rtd_outl(0x1800de6c, 0xFF00FFC0);
	_phy_rtd_outl(0x1800de70, 0xff005400);
	//B lane                  
	_phy_rtd_outl(0x1800de80, 0x00000000);
	_phy_rtd_outl(0x1800de84, 0x00000000);
	_phy_rtd_outl(0x1800de88, 0x20C00000);
	_phy_rtd_outl(0x1800de8c, 0x05b47c00);
	_phy_rtd_outl(0x1800de90, 0xc0004000);
	_phy_rtd_outl(0x1800de9c, 0x7FF07FE0);
	_phy_rtd_outl(0x1800de94, 0x00280000);
	_phy_rtd_outl(0x1800de98, 0x1238ffe0);
	_phy_rtd_outl(0x1800dea0, 0x3FFF83E0);
	_phy_rtd_outl(0x1800dea4, 0x00000000);
	_phy_rtd_outl(0x1800dea8, 0x009C00E7);
	_phy_rtd_outl(0x1800deac, 0xFF00FFC0);
	_phy_rtd_outl(0x1800deb0, 0xff005400);
	//CK lane                 
	_phy_rtd_outl(0x1800dec0, 0x00000000);
	_phy_rtd_outl(0x1800dec4, 0x00000000);
	_phy_rtd_outl(0x1800dec8, 0x20C00000);
	_phy_rtd_outl(0x1800decc, 0x05b47c00);
	_phy_rtd_outl(0x1800ded0, 0xc0004000);
	_phy_rtd_outl(0x1800ded4, 0x00280000);
	_phy_rtd_outl(0x1800ded8, 0x1238ffe0);
	_phy_rtd_outl(0x1800dedc, 0x7FF07FE0);
	_phy_rtd_outl(0x1800dee0, 0x3FFF83E0);
	_phy_rtd_outl(0x1800dee4, 0x00000000);
	_phy_rtd_outl(0x1800dee8, 0x009C00E7);
	_phy_rtd_outl(0x1800deec, 0xFF00FFC0);
	_phy_rtd_outl(0x1800def0, 0xff005400);
	// *************************************************************************************************************************************
	//B-Lane_initial DFE Value_Start
	_phy_rtd_part_outl(0x1800dea8, 9, 5, 14); //VTHP_INIT_B[4:0]	9:5	R/W	0x0F		VTHP INIT
	_phy_rtd_part_outl(0x1800dea8, 4, 0, 14); //VTHN_INIT_B[4:0]	4:0	R/W	0x0F		VTHN INIT
	_phy_rtd_part_outl(0x1800dea0, 4, 0, 15); //LEQ_INIT_B[4:0]	4:0		R/W	0x00	LEQ INIT
	_phy_rtd_part_outl(0x1800dea4, 31, 24, 16); //TAP0_INIT_B[7:0]	31:24	R/W	0x00	TAP0 INIT
	_phy_rtd_part_outl(0x1800dea4, 23, 18, 2); //TAP1_INIT_B[5:0]	23:18	R/W	0x00	TAP1 INIT
	_phy_rtd_part_outl(0x1800dea4, 17, 12, 2); //TAP2_INIT_B[5:0]	17:12	R/W	0x00	TAP2 INIT
	_phy_rtd_part_outl(0x1800dea4, 11, 6, 0); //TAP3_INIT_B[5:0]	11:6	R/W	0x00	TAP3 INIT
	_phy_rtd_part_outl(0x1800dea4, 5, 0, 0); //TAP4_INIT_B[5:0]	5:0		R/W	0x00	TAP4 INIT
	//B-Lane DFE COEF LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 0, 0, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP0 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 1, 1, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP1 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 2, 2, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP2 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 3, 3, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP3 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 4, 4, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP4 LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 5, 5, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de88, 31, 31, 1); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 4, 0, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP LOAD IN
	_phy_rtd_part_outl(0x1800de8c, 5, 5, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de88, 31, 31, 0x0); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	//B-Lane_initial DFE Value_Stop
	// *************************************************************************************************************************************
	//G-Lane_initial DFE Value_Start
	_phy_rtd_part_outl(0x1800de68, 9, 5, 14); //VTHP_INIT_B[4:0]	9:5	R/W	0x0F		VTHP INIT
	_phy_rtd_part_outl(0x1800de68, 4, 0, 14); //VTHN_INIT_B[4:0]	4:0	R/W	0x0F		VTHN INIT
	_phy_rtd_part_outl(0x1800de60, 4, 0, 15); //LEQ_INIT_B[4:0]	4:0		R/W	0x00	LEQ INIT
	_phy_rtd_part_outl(0x1800de64, 31, 24, 16); //TAP0_INIT_B[7:0]	31:24	R/W	0x00	TAP0 INIT
	_phy_rtd_part_outl(0x1800de64, 23, 18, 2); //TAP1_INIT_B[5:0]	23:18	R/W	0x00	TAP1 INIT
	_phy_rtd_part_outl(0x1800de64, 17, 12, 2); //TAP2_INIT_B[5:0]	17:12	R/W	0x00	TAP2 INIT
	_phy_rtd_part_outl(0x1800de64, 11, 6, 0); //TAP3_INIT_B[5:0]	11:6	R/W	0x00	TAP3 INIT
	_phy_rtd_part_outl(0x1800de64, 5, 0, 0); //TAP4_INIT_B[5:0]	5:0		R/W	0x00	TAP4 INIT
	//G-Lane DFE COEF LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 0, 0, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP0 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 1, 1, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP1 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 2, 2, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP2 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 3, 3, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP3 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 4, 4, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP4 LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 5, 5, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de48, 31, 31, 1); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 4, 0, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP LOAD IN
	_phy_rtd_part_outl(0x1800de4c, 5, 5, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de48, 31, 31, 0x0); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	//G-Lane_initial DFE Value_Stop
	// *************************************************************************************************************************************
	//R-Lane_initial DFE Value_Start
	_phy_rtd_part_outl(0x1800de28, 9, 5, 14); //VTHP_INIT_B[4:0]	9:5	R/W	0x0F		VTHP INIT
	_phy_rtd_part_outl(0x1800de28, 4, 0, 14); //VTHN_INIT_B[4:0]	4:0	R/W	0x0F		VTHN INIT
	_phy_rtd_part_outl(0x1800de20, 4, 0, 15); //LEQ_INIT_B[4:0]	4:0		R/W	0x00	LEQ INIT
	_phy_rtd_part_outl(0x1800de24, 31, 24, 16); //TAP0_INIT_B[7:0]	31:24	R/W	0x00	TAP0 INIT
	_phy_rtd_part_outl(0x1800de24, 23, 18, 2); //TAP1_INIT_B[5:0]	23:18	R/W	0x00	TAP1 INIT
	_phy_rtd_part_outl(0x1800de24, 17, 12, 2); //TAP2_INIT_B[5:0]	17:12	R/W	0x00	TAP2 INIT
	_phy_rtd_part_outl(0x1800de24, 11, 6, 0); //TAP3_INIT_B[5:0]	11:6	R/W	0x00	TAP3 INIT
	_phy_rtd_part_outl(0x1800de24, 5, 0, 0); //TAP4_INIT_B[5:0]	5:0		R/W	0x00	TAP4 INIT
	//R-Lane DFE COEF LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 0, 0, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP0 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 1, 1, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP1 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 2, 2, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP2 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 3, 3, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP3 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 4, 4, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP4 LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 5, 5, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de08, 31, 31, 1); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 4, 0, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP LOAD IN
	_phy_rtd_part_outl(0x1800de0c, 5, 5, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800de08, 31, 31, 0x0); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	//R-Lane_initial DFE Value_Stop
	// *************************************************************************************************************************************
	//CK-Lane_initial DFE Value_Start
	_phy_rtd_part_outl(0x1800dee8, 9, 5, 14); //VTHP_INIT_B[4:0]	9:5	R/W	0x0F		VTHP INIT
	_phy_rtd_part_outl(0x1800dee8, 4, 0, 14); //VTHN_INIT_B`[4:0]	4:0	R/W	0x0F		VTHN INIT
	_phy_rtd_part_outl(0x1800dee0, 4, 0, 15); //LEQ_INIT_B[4:0]	4:0		R/W	0x00	LEQ INIT
	_phy_rtd_part_outl(0x1800dee4, 31, 24, 16); //TAP0_INIT_B[7:0]	31:24	R/W	0x00	TAP0 INIT
	_phy_rtd_part_outl(0x1800dee4, 23, 18, 2); //TAP1_INIT_B[5:0]	23:18	R/W	0x00	TAP1 INIT
	_phy_rtd_part_outl(0x1800dee4, 17, 12, 2); //TAP2_INIT_B[5:0]	17:12	R/W	0x00	TAP2 INIT
	_phy_rtd_part_outl(0x1800dee4, 11, 6, 0); //TAP3_INIT_B[5:0]	11:6	R/W	0x00	TAP3 INIT
	_phy_rtd_part_outl(0x1800dee4, 5, 0, 0); //TAP4_INIT_B[5:0]	5:0		R/W	0x00	TAP4 INIT
	//CK-Lane DFE COEF LOAD IN
	_phy_rtd_part_outl(0x1800decc, 0, 0, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP0 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 1, 1, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP1 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 2, 2, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP2 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 3, 3, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP3 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 4, 4, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP4 LOAD IN
	_phy_rtd_part_outl(0x1800decc, 5, 5, 1); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800dec8, 31, 31, 1); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	_phy_rtd_part_outl(0x1800decc, 4, 0, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	TAP LOAD IN
	_phy_rtd_part_outl(0x1800decc, 5, 5, 0); //TAP_LOAD_IN_B[4:0]	4:0	R/W	0x00	Vth LOAD IN
	_phy_rtd_part_outl(0x1800dec8, 31, 31, 0x0); //EQ_SELREG_B	31	R/W	0x0	EQ SELREG	LEQ LOAD IN
	//CK-Lane_initial DFE Value_Stop
	//**************************************************************************************************************************************
	_phy_rtd_part_outl(0x1800def4, 17, 15, 6); //rotation_mode Adaption flow control mode
	_phy_rtd_part_outl(0x1800deb4, 17, 15, 6); //rotation_mode Adaption flow control mode
	_phy_rtd_part_outl(0x1800de74, 17, 15, 6); //rotation_mode Adaption flow control mode
	_phy_rtd_part_outl(0x1800de34, 17, 15, 6); //rotation_mode Adaption flow control mode
}

void DPHY_Fix_frl_1_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	//DPHY fix start
	_phy_rtd_part_outl(0x1800db00, 0, 0, 0x1); //[DPHY fix]  reg_p0_b_dig_rst_n
	_phy_rtd_part_outl(0x1800db00, 1, 1, 0x1); //[DPHY fix]  reg_p0_g_dig_rst_n
	_phy_rtd_part_outl(0x1800db00, 2, 2, 0x1); //[DPHY fix]  reg_p0_r_dig_rst_n
	_phy_rtd_part_outl(0x1800db00, 3, 3, 0x1); //[DPHY fix]  reg_p0_ck_dig_rst_n
	_phy_rtd_part_outl(0x1800db00, 4, 4, 0x0); //[DPHY fix]  reg_p0_b_cdr_rst_n
	_phy_rtd_part_outl(0x1800db00, 5, 5, 0x0); //[DPHY fix]  reg_p0_g_cdr_rst_n
	_phy_rtd_part_outl(0x1800db00, 6, 6, 0x0); //[DPHY fix]  reg_p0_r_cdr_rst_n
	_phy_rtd_part_outl(0x1800db00, 7, 7, 0x0); //[DPHY fix]  reg_p0_ck_cdr_rst_n
	_phy_rtd_part_outl(0x1800db00, 8, 8, 0x0); //[DPHY fix]  reg_p0_b_clk_inv
	_phy_rtd_part_outl(0x1800db00, 9, 9, 0x0); //[DPHY fix]  reg_p0_g_clk_inv
	_phy_rtd_part_outl(0x1800db00, 10, 10, 0x0); //[DPHY fix]  reg_p0_r_clk_inv
	_phy_rtd_part_outl(0x1800db00, 11, 11, 0x0); //[DPHY fix]  reg_p0_ck_clk_inv
	_phy_rtd_part_outl(0x1800db00, 12, 12, 0x0); //[DPHY fix]  reg_p0_b_shift_inv
	_phy_rtd_part_outl(0x1800db00, 13, 13, 0x0); //[DPHY fix]  reg_p0_g_shift_inv
	_phy_rtd_part_outl(0x1800db00, 14, 14, 0x0); //[DPHY fix]  reg_p0_r_shift_inv
	_phy_rtd_part_outl(0x1800db00, 15, 15, 0x0); //[DPHY fix]  reg_p0_ck_shift_inv
	_phy_rtd_part_outl(0x1800db00, 24, 24, 0x0); //[DPHY fix]  reg_p0_b_data_order
	_phy_rtd_part_outl(0x1800db00, 25, 25, 0x0); //[DPHY fix]  reg_p0_g_data_order
	_phy_rtd_part_outl(0x1800db00, 26, 26, 0x0); //[DPHY fix]  reg_p0_r_data_order
	_phy_rtd_part_outl(0x1800db00, 27, 27, 0x0); //[DPHY fix]  reg_p0_ck_data_order
	_phy_rtd_part_outl(0x1800db00, 28, 28, 0x0); //[DPHY fix]  reg_p0_b_dyn_kp_en
	_phy_rtd_part_outl(0x1800db00, 29, 29, 0x0); //[DPHY fix]  reg_p0_g_dyn_kp_en
	_phy_rtd_part_outl(0x1800db00, 30, 30, 0x0); //[DPHY fix]  reg_p0_r_dyn_kp_en
	_phy_rtd_part_outl(0x1800db00, 31, 31, 0x0); //[DPHY fix]  reg_p0_ck_dyn_kp_en
	_phy_rtd_part_outl(0x1800db04, 5, 0, 0x00); //[]  reg_p0_b_kp
	_phy_rtd_part_outl(0x1800db04, 11, 6, 0x00); //[]  reg_p0_g_kp
	_phy_rtd_part_outl(0x1800db04, 17, 12, 0x00); //[]  reg_p0_r_kp
	_phy_rtd_part_outl(0x1800db04, 23, 18, 0x00); //[]  reg_p0_ck_kp
	_phy_rtd_part_outl(0x1800db04, 24, 24, 0x0); //[]  reg_p0_b_dfe_edge_out
	_phy_rtd_part_outl(0x1800db04, 25, 25, 0x0); //[]  reg_p0_b_dfe_edge_out
	_phy_rtd_part_outl(0x1800db04, 26, 26, 0x0); //[]  reg_p0_r_dfe_edge_out
	_phy_rtd_part_outl(0x1800db04, 27, 27, 0x0); //[]  reg_p0_ck_dfe_edge_out
	_phy_rtd_part_outl(0x1800db08, 5, 0, 0x0c); //[]  reg_p0_b_bbw_kp
	_phy_rtd_part_outl(0x1800db08, 11, 6, 0x0c); //[]  reg_p0_g_bbw_kp
	_phy_rtd_part_outl(0x1800db08, 17, 12, 0x0c); //[]  reg_p0_r_bbw_kp
	_phy_rtd_part_outl(0x1800db08, 23, 18, 0x0c); //[]  reg_p0_ck_bbw_kp
	_phy_rtd_part_outl(0x1800db0c, 2, 0, 0x1); //[]  reg_p0_b_ki
	_phy_rtd_part_outl(0x1800db0c, 5, 3, 0x1); //[]  reg_p0_g_ki
	_phy_rtd_part_outl(0x1800db0c, 8, 6, 0x1); //[]  reg_p0_r_ki
	_phy_rtd_part_outl(0x1800db0c, 11, 9, 0x1); //[]  reg_p0_ck_ki
	_phy_rtd_part_outl(0x1800db0c, 12, 12, 0x0); //[]  reg_p0_b_st_mode
	_phy_rtd_part_outl(0x1800db0c, 13, 13, 0x0); //[]  reg_p0_g_st_mode
	_phy_rtd_part_outl(0x1800db0c, 14, 14, 0x0); //[]  reg_p0_r_st_mode
	_phy_rtd_part_outl(0x1800db0c, 15, 15, 0x0); //[]  reg_p0_ck_st_mode
	_phy_rtd_part_outl(0x1800db0c, 18, 16, 0x1); //[]  reg_p0_b_bbw_ki
	_phy_rtd_part_outl(0x1800db0c, 21, 19, 0x1); //[]  reg_p0_g_bbw_ki
	_phy_rtd_part_outl(0x1800db0c, 24, 22, 0x1); //[]  reg_p0_r_bbw_ki
	_phy_rtd_part_outl(0x1800db0c, 27, 25, 0x1); //[]  reg_p0_ck_bbw_ki
	_phy_rtd_part_outl(0x1800db0c, 28, 28, 0x0); //[]  reg_p0_b_bbw_st_mode
	_phy_rtd_part_outl(0x1800db0c, 29, 29, 0x0); //[]  reg_p0_g_bbw_st_mode
	_phy_rtd_part_outl(0x1800db0c, 30, 30, 0x0); //[]  reg_p0_r_bbw_st_mode
	_phy_rtd_part_outl(0x1800db0c, 31, 31, 0x0); //[]  reg_p0_ck_bbw_st_mode
	_phy_rtd_part_outl(0x1800db10, 0, 0, 0x0); //[]  reg_p0_b_bypass_sdm_int
	_phy_rtd_part_outl(0x1800db10, 11, 1, 0x000); //[]  reg_p0_b_int_init
	_phy_rtd_part_outl(0x1800db10, 21, 12, 0x000); //[]  reg_p0_b_acc2_period
	_phy_rtd_part_outl(0x1800db10, 22, 22, 0x0); //[]  reg_p0_b_acc2_manual
	_phy_rtd_part_outl(0x1800db10, 23, 23, 0x0); //[]  reg_p0_b_squ_tri
	_phy_rtd_part_outl(0x1800db14, 0, 0, 0x0); //[]  reg_p0_g_bypass_sdm_int
	_phy_rtd_part_outl(0x1800db14, 11, 1, 0x000); //[]  reg_p0_g_int_init
	_phy_rtd_part_outl(0x1800db14, 21, 12, 0x000); //[]  reg_p0_g_acc2_period
	_phy_rtd_part_outl(0x1800db14, 22, 22, 0x0); //[]  reg_p0_g_acc2_manual
	_phy_rtd_part_outl(0x1800db14, 23, 23, 0x0); //[]  reg_p0_g_squ_tri
	_phy_rtd_part_outl(0x1800db18, 0, 0, 0x0); //[]  reg_p0_r_bypass_sdm_int
	_phy_rtd_part_outl(0x1800db18, 11, 1, 0x000); //[]  reg_p0_r_int_init
	_phy_rtd_part_outl(0x1800db18, 21, 12, 0x000); //[]  reg_p0_r_acc2_period
	_phy_rtd_part_outl(0x1800db18, 22, 22, 0x0); //[]  reg_p0_r_acc2_manual
	_phy_rtd_part_outl(0x1800db18, 23, 23, 0x0); //[]  reg_p0_r_squ_tri
	_phy_rtd_part_outl(0x1800db1c, 0, 0, 0x0); //[]  reg_p0_ck_bypass_sdm_int
	_phy_rtd_part_outl(0x1800db1c, 11, 1, 0x000); //[]  reg_p0_ck_int_init
	_phy_rtd_part_outl(0x1800db1c, 21, 12, 0x000); //[]  reg_p0_ck_acc2_period
	_phy_rtd_part_outl(0x1800db1c, 22, 22, 0x0); //[]  reg_p0_ck_acc2_manual
	_phy_rtd_part_outl(0x1800db1c, 23, 23, 0x0); //[]  reg_p0_ck_squ_tri
	_phy_rtd_part_outl(0x1800db20, 0, 0, 0x0); //[]  reg_p0_b_pi_m_mode
	_phy_rtd_part_outl(0x1800db20, 3, 1, 0x0); //[]  reg_p0_b_testout_sel
	_phy_rtd_part_outl(0x1800db20, 8, 4, 0x00); //[]  reg_p0_b_timer_lpf
	_phy_rtd_part_outl(0x1800db20, 13, 9, 0x00); //[]  reg_p0_b_timer_eq
	_phy_rtd_part_outl(0x1800db20, 18, 14, 0x00); //[]  reg_p0_b_timer_ber
	_phy_rtd_part_outl(0x1800db24, 0, 0, 0x0); //[]  reg_p0_g_pi_m_mode
	_phy_rtd_part_outl(0x1800db24, 3, 1, 0x0); //[]  reg_p0_g_testout_sel
	_phy_rtd_part_outl(0x1800db24, 8, 4, 0x00); //[]  reg_p0_g_timer_lpf
	_phy_rtd_part_outl(0x1800db24, 13, 9, 0x00); //[]  reg_p0_g_timer_eq
	_phy_rtd_part_outl(0x1800db24, 18, 14, 0x00); //[]  reg_p0_g_timer_ber
	_phy_rtd_part_outl(0x1800db28, 0, 0, 0x0); //[]  reg_p0_r_pi_m_mode
	_phy_rtd_part_outl(0x1800db28, 3, 1, 0x0); //[]  reg_p0_r_testout_sel
	_phy_rtd_part_outl(0x1800db28, 8, 4, 0x00); //[]  reg_p0_r_timer_lpf
	_phy_rtd_part_outl(0x1800db28, 13, 9, 0x00); //[]  reg_p0_r_timer_eq
	_phy_rtd_part_outl(0x1800db28, 18, 14, 0x00); //[]  reg_p0_r_timer_ber
	_phy_rtd_part_outl(0x1800db2c, 0, 0, 0x0); //[]  reg_p0_ck_pi_m_mode
	_phy_rtd_part_outl(0x1800db2c, 3, 1, 0x0); //[]  reg_p0_ck_testout_sel
	_phy_rtd_part_outl(0x1800db2c, 8, 4, 0x00); //[]  reg_p0_ck_timer_lpf
	_phy_rtd_part_outl(0x1800db2c, 13, 9, 0x00); //[]  reg_p0_ck_timer_eq
	_phy_rtd_part_outl(0x1800db2c, 18, 14, 0x00); //[]  reg_p0_ck_timer_ber
	_phy_rtd_outl(0x1800db30, 0x000000ff); //[]  reg_p0_b_st_m_value
	_phy_rtd_outl(0x1800db34, 0x000000ff); //[]  reg_p0_g_st_m_value
	_phy_rtd_outl(0x1800db38, 0x000000ff); //[]  reg_p0_r_st_m_value
	_phy_rtd_outl(0x1800db3c, 0x000000ff); //[]  reg_p0_ck_st_m_value
	//Clock Ready
	_phy_rtd_part_outl(0x1800db40, 0, 0, 0x0); //[]  reg_p0_b_force_clkrdy
	_phy_rtd_part_outl(0x1800db40, 1, 1, 0x0); //[]  reg_p0_g_force_clkrdy
	_phy_rtd_part_outl(0x1800db40, 2, 2, 0x0); //[]  reg_p0_r_force_clkrdy
	_phy_rtd_part_outl(0x1800db40, 3, 3, 0x0); //[]  reg_p0_ck_force_clkrdy
	_phy_rtd_part_outl(0x1800db40, 7, 4, 0x2); //[]  reg_p0_b_timer_clk
	_phy_rtd_part_outl(0x1800db40, 11, 8, 0x2); //[]  reg_p0_g_timer_clk
	_phy_rtd_part_outl(0x1800db40, 15, 12, 0x2); //[]  reg_p0_r_timer_clk
	_phy_rtd_part_outl(0x1800db40, 19, 16, 0x2); //[]  reg_p0_ck_timer_clk
	_phy_rtd_part_outl(0x1800db44, 11, 8, 0xf); //script=0xf ; realsim=0x0
	_phy_rtd_part_outl(0x1800db44, 13, 12, 0x0); //[DPHY fix]  reg_p0_b_offset_stable_cnt
	_phy_rtd_part_outl(0x1800db44, 19, 14, 0x0b); //[DPHY fix]  reg_p0_b_offset_divisor
	_phy_rtd_part_outl(0x1800db44, 26, 20, 0x28); //[DPHY fix]  reg_p0_b_offset_timeout
	_phy_rtd_part_outl(0x1800db48, 0, 0, 0x0); //[]  reg_p0_b_offset_en_ope
	_phy_rtd_part_outl(0x1800db48, 1, 1, 0x0); //[]  reg_p0_b_offset_pc_ope
	_phy_rtd_part_outl(0x1800db48, 6, 2, 0x0); //[]  reg_p0_b_offset_ini_ope
	_phy_rtd_part_outl(0x1800db48, 7, 7, 0x0); //[]  reg_p0_b_offset_gray_en_ope
	_phy_rtd_part_outl(0x1800db48, 8, 8, 0x0); //[]  reg_p0_b_offset_manual_ope
	_phy_rtd_part_outl(0x1800db48, 9, 9, 0x0); //[]  reg_p0_b_offset_z0_ok_ope
	_phy_rtd_part_outl(0x1800db48, 10, 10, 0x0); //[]  reg_p0_b_offset_en_opo
	_phy_rtd_part_outl(0x1800db48, 11, 11, 0x0); //[]  reg_p0_b_offset_pc_opo
	_phy_rtd_part_outl(0x1800db48, 16, 12, 0x0); //[]  reg_p0_b_offset_ini_opo
	_phy_rtd_part_outl(0x1800db48, 17, 17, 0x0); //[]  reg_p0_b_offset_gray_en_opo
	_phy_rtd_part_outl(0x1800db48, 18, 18, 0x0); //[]  reg_p0_b_offset_manual_opo
	_phy_rtd_part_outl(0x1800db48, 19, 19, 0x0); //[]  reg_p0_b_offset_z0_ok_opo
	_phy_rtd_part_outl(0x1800db48, 20, 20, 0x0); //[]  reg_p0_b_offset_en_one
	_phy_rtd_part_outl(0x1800db48, 21, 21, 0x0); //[]  reg_p0_b_offset_pc_one
	_phy_rtd_part_outl(0x1800db48, 26, 22, 0x0); //[]  reg_p0_b_offset_ini_one
	_phy_rtd_part_outl(0x1800db48, 27, 27, 0x0); //[]  reg_p0_b_offset_gray_en_one
	_phy_rtd_part_outl(0x1800db48, 28, 28, 0x0); //[]  reg_p0_b_offset_manual_one
	_phy_rtd_part_outl(0x1800db48, 29, 29, 0x0); //[]  reg_p0_b_offset_z0_ok_one
	_phy_rtd_part_outl(0x1800db4c, 0, 0, 0x0); //[DPHY fix]  reg_p0_b_offset_en_ono
	_phy_rtd_part_outl(0x1800db4c, 1, 1, 0x0); //[DPHY fix]  reg_p0_b_offset_pc_ono
	_phy_rtd_part_outl(0x1800db4c, 6, 2, 0x0); //[DPHY fix]  reg_p0_b_offset_ini_ono
	_phy_rtd_part_outl(0x1800db4c, 7, 7, 0x0); //[DPHY fix]  reg_p0_b_offset_gray_en_ono
	_phy_rtd_part_outl(0x1800db4c, 8, 8, 0x0); //[DPHY fix]  reg_p0_b_offset_manual_ono
	_phy_rtd_part_outl(0x1800db4c, 9, 9, 0x0); //[DPHY fix]  reg_p0_b_offset_z0_ok_ono
	_phy_rtd_part_outl(0x1800db4c, 16, 12, 0x0); //[DPHY fix]  reg_p0_b_offset_ini_de
	_phy_rtd_part_outl(0x1800db4c, 17, 17, 0x0); //[DPHY fix]  reg_p0_b_offset_gray_en_de
	_phy_rtd_part_outl(0x1800db4c, 26, 22, 0x0); //[DPHY fix]  reg_p0_b_offset_ini_do
	_phy_rtd_part_outl(0x1800db4c, 27, 27, 0x0); //[DPHY fix]  reg_p0_b_offset_gray_en_do
	_phy_rtd_part_outl(0x1800db50, 6, 2, 0x0); //[DPHY fix]  reg_p0_b_offset_ini_ee
	_phy_rtd_part_outl(0x1800db50, 7, 7, 0x0); //[DPHY fix]  reg_p0_b_offset_gray_en_ee
	_phy_rtd_part_outl(0x1800db50, 16, 12, 0x0); //[DPHY fix]  reg_p0_b_offset_ini_eo
	_phy_rtd_part_outl(0x1800db50, 17, 17, 0x0); //[DPHY fix]  reg_p0_b_offset_gray_en_eo
	_phy_rtd_part_outl(0x1800db50, 26, 22, 0x0); //[DPHY fix]  reg_p0_b_offset_ini_eq
	_phy_rtd_part_outl(0x1800db50, 27, 27, 0x0); //[DPHY fix]  reg_p0_b_offset_gray_en_eq
	_phy_rtd_part_outl(0x1800db50, 29, 29, 0x0); //[DPHY fix]  reg_p0_b_offset_z0_ok_eq
	_phy_rtd_part_outl(0x1800db54, 11, 8, 0xf); //script=0xf ; realsim=0x0
	_phy_rtd_part_outl(0x1800db54, 13, 12, 0x0); //[DPHY fix]  reg_p0_g_offset_stable_cnt
	_phy_rtd_part_outl(0x1800db54, 19, 14, 0x0b); //[DPHY fix]  reg_p0_g_offset_divisor
	_phy_rtd_part_outl(0x1800db54, 26, 20, 0x28); //[DPHY fix]  reg_p0_g_offset_timeout
	_phy_rtd_part_outl(0x1800db58, 0, 0, 0x0); //[]  reg_p0_g_offset_en_ope
	_phy_rtd_part_outl(0x1800db58, 1, 1, 0x0); //[]  reg_p0_g_offset_pc_ope
	_phy_rtd_part_outl(0x1800db58, 6, 2, 0x0); //[]  reg_p0_g_offset_ini_ope
	_phy_rtd_part_outl(0x1800db58, 7, 7, 0x0); //[]  reg_p0_g_offset_gray_en_ope
	_phy_rtd_part_outl(0x1800db58, 8, 8, 0x0); //[]  reg_p0_g_offset_manual_ope
	_phy_rtd_part_outl(0x1800db58, 9, 9, 0x0); //[]  reg_p0_g_offset_z0_ok_ope
	_phy_rtd_part_outl(0x1800db58, 10, 10, 0x0); //[]  reg_p0_g_offset_en_opo
	_phy_rtd_part_outl(0x1800db58, 11, 11, 0x0); //[]  reg_p0_g_offset_pc_opo
	_phy_rtd_part_outl(0x1800db58, 16, 12, 0x0); //[]  reg_p0_g_offset_ini_opo
	_phy_rtd_part_outl(0x1800db58, 17, 17, 0x0); //[]  reg_p0_g_offset_gray_en_opo
	_phy_rtd_part_outl(0x1800db58, 18, 18, 0x0); //[]  reg_p0_g_offset_manual_opo
	_phy_rtd_part_outl(0x1800db58, 19, 19, 0x0); //[]  reg_p0_g_offset_z0_ok_opo
	_phy_rtd_part_outl(0x1800db58, 20, 20, 0x0); //[]  reg_p0_g_offset_en_one
	_phy_rtd_part_outl(0x1800db58, 21, 21, 0x0); //[]  reg_p0_g_offset_pc_one
	_phy_rtd_part_outl(0x1800db58, 26, 22, 0x0); //[]  reg_p0_g_offset_ini_one
	_phy_rtd_part_outl(0x1800db58, 27, 27, 0x0); //[]  reg_p0_g_offset_gray_en_one
	_phy_rtd_part_outl(0x1800db58, 28, 28, 0x0); //[]  reg_p0_g_offset_manual_one
	_phy_rtd_part_outl(0x1800db58, 29, 29, 0x0); //[]  reg_p0_g_offset_z0_ok_one
	_phy_rtd_part_outl(0x1800db5c, 0, 0, 0x0); //[DPHY fix]  reg_p0_g_offset_en_ono
	_phy_rtd_part_outl(0x1800db5c, 6, 2, 0x0); //[DPHY fix]  reg_p0_g_offset_ini_ono
	_phy_rtd_part_outl(0x1800db5c, 7, 7, 0x0); //[DPHY fix]  reg_p0_g_offset_gray_en_ono
	_phy_rtd_part_outl(0x1800db5c, 1, 1, 0x0); //[DPHY fix]  reg_p0_g_offset_pc_ono
	_phy_rtd_part_outl(0x1800db5c, 8, 8, 0x0); //[DPHY fix]  reg_p0_g_offset_manual_ono
	_phy_rtd_part_outl(0x1800db5c, 9, 9, 0x0); //[DPHY fix]  reg_p0_g_offset_z0_ok_ono
	_phy_rtd_part_outl(0x1800db5c, 16, 12, 0x0); //[DPHY fix]  reg_p0_g_offset_ini_de
	_phy_rtd_part_outl(0x1800db5c, 17, 17, 0x0); //[DPHY fix]  reg_p0_g_offset_gray_en_de
	_phy_rtd_part_outl(0x1800db5c, 26, 22, 0x0); //[DPHY fix]  reg_p0_g_offset_ini_do
	_phy_rtd_part_outl(0x1800db5c, 27, 27, 0x0); //[DPHY fix]  reg_p0_g_offset_gray_en_do
	_phy_rtd_part_outl(0x1800db60, 6, 2, 0x0); //[DPHY fix]  reg_p0_g_offset_ini_ee
	_phy_rtd_part_outl(0x1800db60, 7, 7, 0x0); //[DPHY fix]  reg_p0_g_offset_gray_en_ee
	_phy_rtd_part_outl(0x1800db60, 16, 12, 0x0); //[DPHY fix]  reg_p0_g_offset_ini_eo
	_phy_rtd_part_outl(0x1800db60, 17, 17, 0x0); //[DPHY fix]  reg_p0_g_offset_gray_en_eo
	_phy_rtd_part_outl(0x1800db60, 26, 22, 0x0); //[DPHY fix]  reg_p0_g_offset_ini_eq
	_phy_rtd_part_outl(0x1800db60, 27, 27, 0x0); //[DPHY fix]  reg_p0_g_offset_gray_en_eq
	_phy_rtd_part_outl(0x1800db60, 29, 29, 0x0); //[DPHY fix]  reg_p0_g_offset_z0_ok_eq
	_phy_rtd_part_outl(0x1800db64, 11, 8, 0xf); //script=0xf ; realsim=0x0
	_phy_rtd_part_outl(0x1800db64, 13, 12, 0x0); //[DPHY fix]  reg_p0_r_offset_stable_cnt
	_phy_rtd_part_outl(0x1800db64, 19, 14, 0x0b); //[DPHY fix]  reg_p0_r_offset_divisor
	_phy_rtd_part_outl(0x1800db64, 26, 20, 0x28); //[DPHY fix]  reg_p0_r_offset_timeout
	_phy_rtd_part_outl(0x1800db68, 0, 0, 0x0); //[]  reg_p0_r_offset_en_ope
	_phy_rtd_part_outl(0x1800db68, 1, 1, 0x0); //[]  reg_p0_r_offset_pc_ope
	_phy_rtd_part_outl(0x1800db68, 6, 2, 0x0); //[]  reg_p0_r_offset_ini_ope
	_phy_rtd_part_outl(0x1800db68, 7, 7, 0x0); //[]  reg_p0_r_offset_gray_en_ope
	_phy_rtd_part_outl(0x1800db68, 8, 8, 0x0); //[]  reg_p0_r_offset_manual_ope
	_phy_rtd_part_outl(0x1800db68, 9, 9, 0x0); //[]  reg_p0_r_offset_z0_ok_ope
	_phy_rtd_part_outl(0x1800db68, 10, 10, 0x0); //[]  reg_p0_r_offset_en_opo
	_phy_rtd_part_outl(0x1800db68, 11, 11, 0x0); //[]  reg_p0_r_offset_pc_opo
	_phy_rtd_part_outl(0x1800db68, 16, 12, 0x0); //[]  reg_p0_r_offset_ini_opo
	_phy_rtd_part_outl(0x1800db68, 17, 17, 0x0); //[]  reg_p0_r_offset_gray_en_opo
	_phy_rtd_part_outl(0x1800db68, 18, 18, 0x0); //[]  reg_p0_r_offset_manual_opo
	_phy_rtd_part_outl(0x1800db68, 19, 19, 0x0); //[]  reg_p0_r_offset_z0_ok_opo
	_phy_rtd_part_outl(0x1800db68, 20, 20, 0x0); //[]  reg_p0_r_offset_en_one
	_phy_rtd_part_outl(0x1800db68, 21, 21, 0x0); //[]  reg_p0_r_offset_pc_one
	_phy_rtd_part_outl(0x1800db68, 26, 22, 0x0); //[]  reg_p0_r_offset_ini_one
	_phy_rtd_part_outl(0x1800db68, 27, 27, 0x0); //[]  reg_p0_r_offset_gray_en_one
	_phy_rtd_part_outl(0x1800db68, 28, 28, 0x0); //[]  reg_p0_r_offset_manual_one
	_phy_rtd_part_outl(0x1800db68, 29, 29, 0x0); //[]  reg_p0_r_offset_z0_ok_one
	_phy_rtd_part_outl(0x1800db6c, 0, 0, 0x0); //[DPHY fix]  reg_p0_r_offset_en_ono
	_phy_rtd_part_outl(0x1800db6c, 1, 1, 0x0); //[DPHY fix]  reg_p0_r_offset_pc_ono
	_phy_rtd_part_outl(0x1800db6c, 6, 2, 0x0); //[DPHY fix]  reg_p0_r_offset_ini_ono
	_phy_rtd_part_outl(0x1800db6c, 7, 7, 0x0); //[DPHY fix]  reg_p0_r_offset_gray_en_ono
	_phy_rtd_part_outl(0x1800db6c, 8, 8, 0x0); //[DPHY fix]  reg_p0_r_offset_manual_ono
	_phy_rtd_part_outl(0x1800db6c, 9, 9, 0x0); //[DPHY fix]  reg_p0_r_offset_z0_ok_ono
	_phy_rtd_part_outl(0x1800db6c, 16, 12, 0x0); //[DPHY fix]  reg_p0_r_offset_ini_de
	_phy_rtd_part_outl(0x1800db6c, 17, 17, 0x0); //[DPHY fix]  reg_p0_r_offset_gray_en_de
	_phy_rtd_part_outl(0x1800db6c, 26, 22, 0x0); //[DPHY fix]  reg_p0_r_offset_ini_do
	_phy_rtd_part_outl(0x1800db6c, 27, 27, 0x0); //[DPHY fix]  reg_p0_r_offset_gray_en_do
	_phy_rtd_part_outl(0x1800db70, 6, 2, 0x0); //[DPHY fix]  reg_p0_r_offset_ini_ee
	_phy_rtd_part_outl(0x1800db70, 7, 7, 0x0); //[DPHY fix]  reg_p0_r_offset_gray_en_ee
	_phy_rtd_part_outl(0x1800db70, 16, 12, 0x0); //[DPHY fix]  reg_p0_r_offset_ini_eo
	_phy_rtd_part_outl(0x1800db70, 17, 17, 0x0); //[DPHY fix]  reg_p0_r_offset_gray_en_eo
	_phy_rtd_part_outl(0x1800db70, 26, 22, 0x0); //[DPHY fix]  reg_p0_r_offset_ini_eq
	_phy_rtd_part_outl(0x1800db70, 27, 27, 0x0); //[DPHY fix]  reg_p0_r_offset_gray_en_eq
	_phy_rtd_part_outl(0x1800db70, 29, 29, 0x0); //[DPHY fix]  reg_p0_r_offset_z0_ok_eq
	_phy_rtd_part_outl(0x1800db74, 11, 8, 0xf); //script=0xf ; realsim=0x0
	_phy_rtd_part_outl(0x1800db74, 13, 12, 0x0); //[DPHY fix]  reg_p0_ck_offset_stable_cnt
	_phy_rtd_part_outl(0x1800db74, 19, 14, 0x0b); //[DPHY fix]  reg_p0_ck_offset_divisor
	_phy_rtd_part_outl(0x1800db74, 26, 20, 0x28); //[DPHY fix]  reg_p0_ck_offset_timeout
	_phy_rtd_part_outl(0x1800db78, 0, 0, 0x0); //[]  reg_p0_ck_offset_en_ope
	_phy_rtd_part_outl(0x1800db78, 1, 1, 0x0); //[]  reg_p0_ck_offset_pc_ope
	_phy_rtd_part_outl(0x1800db78, 6, 2, 0x0); //[]  reg_p0_ck_offset_ini_ope
	_phy_rtd_part_outl(0x1800db78, 7, 7, 0x0); //[]  reg_p0_ck_offset_gray_en_ope
	_phy_rtd_part_outl(0x1800db78, 8, 8, 0x0); //[]  reg_p0_ck_offset_manual_ope
	_phy_rtd_part_outl(0x1800db78, 9, 9, 0x0); //[]  reg_p0_ck_offset_z0_ok_ope
	_phy_rtd_part_outl(0x1800db78, 10, 10, 0x0); //[]  reg_p0_ck_offset_en_opo
	_phy_rtd_part_outl(0x1800db78, 11, 11, 0x0); //[]  reg_p0_ck_offset_pc_opo
	_phy_rtd_part_outl(0x1800db78, 16, 12, 0x0); //[]  reg_p0_ck_offset_ini_opo
	_phy_rtd_part_outl(0x1800db78, 17, 17, 0x0); //[]  reg_p0_ck_offset_gray_en_opo
	_phy_rtd_part_outl(0x1800db78, 18, 18, 0x0); //[]  reg_p0_ck_offset_manual_opo
	_phy_rtd_part_outl(0x1800db78, 19, 19, 0x0); //[]  reg_p0_ck_offset_z0_ok_opo
	_phy_rtd_part_outl(0x1800db78, 20, 20, 0x0); //[]  reg_p0_ck_offset_en_one
	_phy_rtd_part_outl(0x1800db78, 21, 21, 0x0); //[]  reg_p0_ck_offset_pc_one
	_phy_rtd_part_outl(0x1800db78, 26, 22, 0x0); //[]  reg_p0_ck_offset_ini_one
	_phy_rtd_part_outl(0x1800db78, 27, 27, 0x0); //[]  reg_p0_ck_offset_gray_en_one
	_phy_rtd_part_outl(0x1800db78, 28, 28, 0x0); //[]  reg_p0_ck_offset_manual_one
	_phy_rtd_part_outl(0x1800db78, 29, 29, 0x0); //[]  reg_p0_ck_offset_z0_ok_one
	_phy_rtd_part_outl(0x1800db7c, 0, 0, 0x0); //[DPHY fix]  reg_p0_ck_offset_en_ono
	_phy_rtd_part_outl(0x1800db7c, 1, 1, 0x0); //[DPHY fix]  reg_p0_ck_offset_pc_ono
	_phy_rtd_part_outl(0x1800db7c, 6, 2, 0x0); //[DPHY fix]  reg_p0_ck_offset_ini_ono
	_phy_rtd_part_outl(0x1800db7c, 7, 7, 0x0); //[DPHY fix]  reg_p0_ck_offset_gray_en_ono
	_phy_rtd_part_outl(0x1800db7c, 8, 8, 0x0); //[DPHY fix]  reg_p0_ck_offset_manual_ono
	_phy_rtd_part_outl(0x1800db7c, 9, 9, 0x0); //[DPHY fix]  reg_p0_ck_offset_z0_ok_ono
	_phy_rtd_part_outl(0x1800db7c, 16, 12, 0x0); //[DPHY fix]  reg_p0_ck_offset_ini_de
	_phy_rtd_part_outl(0x1800db7c, 17, 17, 0x0); //[DPHY fix]  reg_p0_ck_offset_gray_en_de
	_phy_rtd_part_outl(0x1800db7c, 26, 22, 0x0); //[DPHY fix]  reg_p0_ck_offset_ini_do
	_phy_rtd_part_outl(0x1800db7c, 27, 27, 0x0); //[DPHY fix]  reg_p0_ck_offset_gray_en_do
	_phy_rtd_part_outl(0x1800db80, 6, 2, 0x0); //[DPHY fix]  reg_p0_ck_offset_ini_ee
	_phy_rtd_part_outl(0x1800db80, 7, 7, 0x0); //[DPHY fix]  reg_p0_ck_offset_gray_en_ee
	_phy_rtd_part_outl(0x1800db80, 16, 12, 0x0); //[DPHY fix]  reg_p0_ck_offset_ini_eo
	_phy_rtd_part_outl(0x1800db80, 17, 17, 0x0); //[DPHY fix]  reg_p0_ck_offset_gray_en_eo
	_phy_rtd_part_outl(0x1800db80, 26, 22, 0x0); //[DPHY fix]  reg_p0_ck_offset_ini_eq
	_phy_rtd_part_outl(0x1800db80, 27, 27, 0x0); //[DPHY fix]  reg_p0_ck_offset_gray_en_eq
	_phy_rtd_part_outl(0x1800db80, 29, 29, 0x0); //[DPHY fix]  reg_p0_ck_offset_z0_ok_eq
	_phy_rtd_part_outl(0x1800db88, 2, 2, 0x0); //[DPHY fix]  reg_p0_b_pn_swap_en
	_phy_rtd_part_outl(0x1800db88, 3, 3, 0x1); //[DPHY fix]  reg_p0_b_dfe_data_en
	_phy_rtd_part_outl(0x1800db88, 7, 5, 0x3); //[DPHY fix]  reg_p0_b_inbuf_num
	_phy_rtd_part_outl(0x1800db88, 10, 10, 0x0); //[DPHY fix]  reg_p0_g_pn_swap_en
	_phy_rtd_part_outl(0x1800db88, 11, 11, 0x1); //[DPHY fix]  reg_p0_g_dfe_data_en
	_phy_rtd_part_outl(0x1800db88, 15, 13, 0x3); //[DPHY fix]  reg_p0_g_inbuf_num
	_phy_rtd_part_outl(0x1800db88, 18, 18, 0x0); //[DPHY fix]  reg_p0_r_pn_swap_en
	_phy_rtd_part_outl(0x1800db88, 19, 19, 0x1); //[DPHY fix]  reg_p0_r_dfe_data_en
	_phy_rtd_part_outl(0x1800db88, 23, 21, 0x3); //[DPHY fix] reg_p0_r_inbuf_num
	_phy_rtd_part_outl(0x1800db88, 26, 26, 0x0); //[DPHY fix]  reg_p0_ck_pn_swap_en
	_phy_rtd_part_outl(0x1800db88, 27, 27, 0x1); //[DPHY fix]  reg_p0_ck_dfe_data_en
	_phy_rtd_part_outl(0x1800db88, 31, 29, 0x3); //[DPHY fix]  reg_p0_ck_inbuf_num
	_phy_rtd_part_outl(0x1800db8c, 0, 0, 0x0); //[]  reg_p0_b_001_enable
	_phy_rtd_part_outl(0x1800db8c, 1, 1, 0x0); //[]  reg_p0_b_101_enable
	_phy_rtd_part_outl(0x1800db8c, 2, 2, 0x0); //[]  reg_p0_b_en_bec_acc
	_phy_rtd_part_outl(0x1800db8c, 3, 3, 0x0); //[]  reg_p0_b_en_bec_read
	_phy_rtd_part_outl(0x1800db8c, 12, 12, 0x0); //[]  reg_p0_g_001_enable
	_phy_rtd_part_outl(0x1800db8c, 13, 13, 0x0); //[]  reg_p0_g_101_enable
	_phy_rtd_part_outl(0x1800db8c, 14, 14, 0x0); //[]  reg_p0_g_en_bec_acc
	_phy_rtd_part_outl(0x1800db8c, 15, 15, 0x0); //[]  reg_p0_g_en_bec_read
	_phy_rtd_part_outl(0x1800db90, 0, 0, 0x0); //[]  reg_p0_r_001_enable
	_phy_rtd_part_outl(0x1800db90, 1, 1, 0x0); //[]  reg_p0_r_101_enable
	_phy_rtd_part_outl(0x1800db90, 2, 2, 0x0); //[]  reg_p0_r_en_bec_acc
	_phy_rtd_part_outl(0x1800db90, 3, 3, 0x0); //[]  reg_p0_r_en_bec_read
	_phy_rtd_part_outl(0x1800db90, 12, 12, 0x0); //[]  reg_p0_ck_001_enable
	_phy_rtd_part_outl(0x1800db90, 13, 13, 0x0); //[]  reg_p0_ck_101_enable
	_phy_rtd_part_outl(0x1800db90, 14, 14, 0x0); //[]  reg_p0_ck_en_bec_acc
	_phy_rtd_part_outl(0x1800db90, 15, 15, 0x0); //[]  reg_p0_ck_en_bec_read
	_phy_rtd_part_outl(0x1800dbb8, 0, 0, 0x0); //[DPHY fix]  reg_p0_b_cp2adp_en
	_phy_rtd_part_outl(0x1800dbb8, 1, 1, 0x0); //[DPHY fix]  reg_p0_b_stable_time_mode
	_phy_rtd_part_outl(0x1800dbb8, 2, 2, 0x0); //[DPHY fix]  reg_p0_b_bypass_k_band_mode
	_phy_rtd_part_outl(0x1800dbb8, 18, 18, 0x0); //[DPHY fix]  reg_p0_b_calib_late
	_phy_rtd_part_outl(0x1800dbb8, 19, 19, 0x0); //[DPHY fix]  reg_p0_b_calib_manual
	_phy_rtd_part_outl(0x1800dbb8, 22, 20, 0x2); //[DPHY fix]  reg_p0_b_calib_time
	_phy_rtd_part_outl(0x1800dbb8, 27, 23, 0x0c); //script=0x0c ; realsim=0x1
	_phy_rtd_part_outl(0x1800dbb8, 28, 28, 0x0); //[DPHY fix]  reg_p0_b_adap_eq_off
	_phy_rtd_part_outl(0x1800dbb8, 29, 29, 0x0); //[DPHY fix]  reg_p0_b_cp_en_manual
	_phy_rtd_part_outl(0x1800dbb8, 30, 30, 0x0); //[DPHY fix]  reg_p0_b_adp_en_manual
	_phy_rtd_part_outl(0x1800dbb8, 31, 31, 0x1); //[DPHY fix]  reg_p0_b_auto_mode
	_phy_rtd_part_outl(0x1800dbbc, 31, 28, 0x8); //[DPHY fix] reg_p0_b_cp2adp_time
	_phy_rtd_part_outl(0x1800dbc0, 1, 1, 0x1); //[] reg_p0_b_calib_reset_sel
	_phy_rtd_part_outl(0x1800dbc0, 3, 2, 0x0); //[] reg_p0_b_vc_sel
	_phy_rtd_part_outl(0x1800dbc0, 5, 4, 0x0); //[] reg_p0_b_cdr_c
	_phy_rtd_part_outl(0x1800dbc0, 11, 6, 0x00); //[] reg_p0_b_cdr_r
	_phy_rtd_part_outl(0x1800dbc4, 2, 0, 0x0); //[DPHY fix] reg_p0_b_init_time
	_phy_rtd_part_outl(0x1800dbc4, 7, 3, 0x04); //[DPHY fix] reg_p0_b_cp_time
	_phy_rtd_part_outl(0x1800dbc8, 6, 2, 0x01); //[DPHY fix]  reg_p0_b_timer_6
	_phy_rtd_part_outl(0x1800dbc8, 7, 7, 0x0); //[DPHY fix]  reg_p0_b_timer_5
	_phy_rtd_part_outl(0x1800dbc8, 11, 8, 0x7); //[DPHY fix]  reg_p0_b_vco_fine_init
	_phy_rtd_part_outl(0x1800dbc8, 15, 12, 0x7); //[DPHY fix]  reg_p0_b_vco_coarse_init
	_phy_rtd_part_outl(0x1800dbc8, 16, 16, 0x0); //[DPHY fix]  reg_p0_b_bypass_coarse_k_mode
	_phy_rtd_part_outl(0x1800dbc8, 18, 17, 0x0); //[DPHY fix]  reg_p0_b_vco_fine_time_sel
	_phy_rtd_part_outl(0x1800dbc8, 19, 19, 0x0); //[DPHY fix]  reg_p0_b_xtal_24m_en
	_phy_rtd_part_outl(0x1800dbc8, 23, 20, 0x0); //[DPHY fix]  reg_p0_b_vco_fine_manual
	_phy_rtd_part_outl(0x1800dbc8, 27, 24, 0x0); //[DPHY fix]  reg_p0_b_vco_coarse_manual
	_phy_rtd_part_outl(0x1800dbc8, 28, 28, 0x0); //[DPHY fix]  reg_p0_b_coarse_calib_manual
	_phy_rtd_part_outl(0x1800dbc8, 29, 29, 0x0); //[DPHY fix]  reg_p0_b_acdr_ckfld_en
	_phy_rtd_part_outl(0x1800dbc8, 30, 30, 0x0); //[DPHY fix]  reg_p0_b_pll_to_acdr_manual_en
	_phy_rtd_part_outl(0x1800dbc8, 31, 31, 0x0); //[DPHY fix]  reg_p0_b_pll_to_acdr_rdy_manual
	_phy_rtd_part_outl(0x1800dbcc, 4, 0, 0x01); //[]  reg_p0_b_timer_4
	_phy_rtd_part_outl(0x1800dbcc, 7, 5, 0x0); //[]  reg_p0_b_vc_chg_time
	_phy_rtd_part_outl(0x1800dbcc, 16, 16, 0x1); //[]  reg_p0_b_old_mode
	_phy_rtd_part_outl(0x1800dbcc, 21, 17, 0x10); //[]  reg_p0_b_slope_band
	_phy_rtd_part_outl(0x1800dbcc, 22, 22, 0x0); //[]  reg_p0_b_slope_manual
	_phy_rtd_part_outl(0x1800dbcc, 23, 23, 0x0); //[]  reg_p0_b_rxidle_bypass
	_phy_rtd_part_outl(0x1800dbcc, 24, 24, 0x0); //[]  reg_p0_b_pfd_bypass
	_phy_rtd_part_outl(0x1800dbcc, 29, 25, 0x00); //[]  reg_p0_b_pfd_time
	_phy_rtd_part_outl(0x1800dbcc, 30, 30, 0x0); //[]  reg_p0_b_pfd_en_manual
	_phy_rtd_part_outl(0x1800dbcc, 31, 31, 0x0); //[]  reg_p0_b_start_en_manual
	_phy_rtd_part_outl(0x1800dbd0, 9, 5, 0x08); //[]  reg_p0_b_cp_time_2
	_phy_rtd_part_outl(0x1800dbd8, 0, 0, 0x0); //[DPHY fix]  reg_p0_g_cp2adp_en
	_phy_rtd_part_outl(0x1800dbd8, 1, 1, 0x0); //[DPHY fix]  reg_p0_g_stable_time_mode
	_phy_rtd_part_outl(0x1800dbd8, 2, 2, 0x0); //[DPHY fix]  reg_p0_g_bypass_k_band_mode
	_phy_rtd_part_outl(0x1800dbd8, 18, 18, 0x0); //[DPHY fix]  reg_p0_g_calib_late
	_phy_rtd_part_outl(0x1800dbd8, 19, 19, 0x0); //[DPHY fix]  reg_p0_g_calib_manual
	_phy_rtd_part_outl(0x1800dbd8, 22, 20, 0x2); //[DPHY fix]  reg_p0_g_calib_time
	_phy_rtd_part_outl(0x1800dbd8, 27, 23, 0x0c); //script=0x0c ; realsim=0x1
	_phy_rtd_part_outl(0x1800dbd8, 28, 28, 0x0); //[DPHY fix]  reg_p0_g_adap_eq_off
	_phy_rtd_part_outl(0x1800dbd8, 29, 29, 0x0); //[DPHY fix]  reg_p0_g_cp_en_manual
	_phy_rtd_part_outl(0x1800dbd8, 30, 30, 0x0); //[DPHY fix]  reg_p0_g_adp_en_manual
	_phy_rtd_part_outl(0x1800dbd8, 31, 31, 0x1); //[DPHY fix]  reg_p0_g_auto_mode
	_phy_rtd_part_outl(0x1800dbdc, 31, 28, 0x8); //[DPHY fix] reg_p0_g_cp2adp_time
	_phy_rtd_part_outl(0x1800dbe0, 1, 1, 0x1); //[] reg_p0_g_calib_reset_sel
	_phy_rtd_part_outl(0x1800dbe0, 3, 2, 0x0); //[] reg_p0_g_vc_sel
	_phy_rtd_part_outl(0x1800dbe0, 5, 4, 0x0); //[] reg_p0_g_cdr_c
	_phy_rtd_part_outl(0x1800dbe0, 11, 6, 0x00); //[] reg_p0_g_cdr_r
	_phy_rtd_part_outl(0x1800dbe4, 2, 0, 0x0); //[DPHY fix] reg_p0_g_init_time
	_phy_rtd_part_outl(0x1800dbe4, 7, 3, 0x04); //[DPHY fix] reg_p0_g_cp_time
	_phy_rtd_part_outl(0x1800dbe8, 6, 2, 0x01); //[DPHY fix]  reg_p0_g_timer_6
	_phy_rtd_part_outl(0x1800dbe8, 7, 7, 0x0); //[DPHY fix]  reg_p0_g_timer_5
	_phy_rtd_part_outl(0x1800dbe8, 11, 8, 0x7); //[DPHY fix]  reg_p0_g_vco_fine_init
	_phy_rtd_part_outl(0x1800dbe8, 15, 12, 0x7); //[DPHY fix]  reg_p0_g_vco_coarse_init
	_phy_rtd_part_outl(0x1800dbe8, 16, 16, 0x0); //[DPHY fix]  reg_p0_g_bypass_coarse_k_mode
	_phy_rtd_part_outl(0x1800dbe8, 18, 17, 0x0); //[DPHY fix]  reg_p0_g_vco_fine_time_sel
	_phy_rtd_part_outl(0x1800dbe8, 19, 19, 0x0); //[DPHY fix]  reg_p0_g_xtal_24m_en
	_phy_rtd_part_outl(0x1800dbe8, 23, 20, 0x0); //[DPHY fix]  reg_p0_g_vco_fine_manual
	_phy_rtd_part_outl(0x1800dbe8, 27, 24, 0x0); //[DPHY fix]  reg_p0_g_vco_coarse_manual
	_phy_rtd_part_outl(0x1800dbe8, 28, 28, 0x0); //[DPHY fix]  reg_p0_g_coarse_calib_manual
	_phy_rtd_part_outl(0x1800dbe8, 29, 29, 0x0); //[DPHY fix]  reg_p0_g_acdr_ckfld_en
	_phy_rtd_part_outl(0x1800dbe8, 30, 30, 0x0); //[DPHY fix]  reg_p0_g_pll_to_acdr_manual_en
	_phy_rtd_part_outl(0x1800dbe8, 31, 31, 0x0); //[DPHY fix]  reg_p0_g_pll_to_acdr_rdy_manual
	_phy_rtd_part_outl(0x1800dbec, 4, 0, 0x01); //[]  reg_p0_g_timer_4
	_phy_rtd_part_outl(0x1800dbec, 7, 5, 0x0); //[]  reg_p0_g_vc_chg_time
	_phy_rtd_part_outl(0x1800dbec, 16, 16, 0x1); //[]  reg_p0_g_old_mode
	_phy_rtd_part_outl(0x1800dbec, 21, 17, 0x10); //[]  reg_p0_g_slope_band
	_phy_rtd_part_outl(0x1800dbec, 22, 22, 0x0); //[]  reg_p0_g_slope_manual
	_phy_rtd_part_outl(0x1800dbec, 23, 23, 0x0); //[]  reg_p0_g_rxidle_bypass
	_phy_rtd_part_outl(0x1800dbec, 24, 24, 0x0); //[]  reg_p0_g_pfd_bypass
	_phy_rtd_part_outl(0x1800dbec, 29, 25, 0x00); //[]  reg_p0_g_pfd_time
	_phy_rtd_part_outl(0x1800dbec, 30, 30, 0x0); //[]  reg_p0_g_pfd_en_manual
	_phy_rtd_part_outl(0x1800dbec, 31, 31, 0x0); //[]  reg_p0_g_start_en_manual
	_phy_rtd_part_outl(0x1800dbf0, 9, 5, 0x08); //[]  reg_p0_g_cp_time_2
	_phy_rtd_part_outl(0x1800dbf4, 0, 0, 0x0); //[DPHY fix]  reg_p0_r_cp2adp_en
	_phy_rtd_part_outl(0x1800dbf4, 1, 1, 0x0); //[DPHY fix]  reg_p0_r_stable_time_mode
	_phy_rtd_part_outl(0x1800dbf4, 2, 2, 0x0); //[DPHY fix]  reg_p0_r_bypass_k_band_mode
	_phy_rtd_part_outl(0x1800dbf4, 18, 18, 0x0); //[DPHY fix]  reg_p0_r_calib_late
	_phy_rtd_part_outl(0x1800dbf4, 19, 19, 0x0); //[DPHY fix]  reg_p0_r_calib_manual
	_phy_rtd_part_outl(0x1800dbf4, 22, 20, 0x2); //[DPHY fix]  reg_p0_r_calib_time
	_phy_rtd_part_outl(0x1800dbf4, 27, 23, 0x0c); //script=0x0c ; realsim=0x1
	_phy_rtd_part_outl(0x1800dbf4, 28, 28, 0x0); //[DPHY fix]  reg_p0_r_adap_eq_off
	_phy_rtd_part_outl(0x1800dbf4, 29, 29, 0x0); //[DPHY fix]  reg_p0_r_cp_en_manual
	_phy_rtd_part_outl(0x1800dbf4, 30, 30, 0x0); //[DPHY fix]  reg_p0_r_adp_en_manual
	_phy_rtd_part_outl(0x1800dbf4, 31, 31, 0x1); //[DPHY fix]  reg_p0_r_auto_mode
	_phy_rtd_part_outl(0x1800dbf8, 31, 28, 0x8); //[DPHY fix] reg_p0_r_cp2adp_time
	_phy_rtd_part_outl(0x1800dbfc, 1, 1, 0x1); //[] reg_p0_r_calib_reset_sel
	_phy_rtd_part_outl(0x1800dbfc, 3, 2, 0x0); //[] reg_p0_r_vc_sel
	_phy_rtd_part_outl(0x1800dbfc, 5, 4, 0x0); //[] reg_p0_r_cdr_c
	_phy_rtd_part_outl(0x1800dbfc, 11, 6, 0x00); //[] reg_p0_r_cdr_r
	_phy_rtd_part_outl(0x1800dc00, 2, 0, 0x0); //[DPHY fix] reg_p0_r_init_time
	_phy_rtd_part_outl(0x1800dc00, 7, 3, 0x04); //[DPHY fix] reg_p0_r_cp_time
	_phy_rtd_part_outl(0x1800dc04, 6, 2, 0x01); //[DPHY fix]  reg_p0_r_timer_6
	_phy_rtd_part_outl(0x1800dc04, 7, 7, 0x0); //[DPHY fix]  reg_p0_r_timer_5
	_phy_rtd_part_outl(0x1800dc04, 11, 8, 0x7); //[DPHY fix]  reg_p0_r_vco_fine_init
	_phy_rtd_part_outl(0x1800dc04, 15, 12, 0x7); //[DPHY fix]  reg_p0_r_vco_coarse_init
	_phy_rtd_part_outl(0x1800dc04, 16, 16, 0x0); //[DPHY fix]  reg_p0_r_bypass_coarse_k_mode
	_phy_rtd_part_outl(0x1800dc04, 18, 17, 0x0); //[DPHY fix]  reg_p0_r_vco_fine_time_sel
	_phy_rtd_part_outl(0x1800dc04, 19, 19, 0x0); //[DPHY fix]  reg_p0_r_xtal_24m_en
	_phy_rtd_part_outl(0x1800dc04, 23, 20, 0x0); //[DPHY fix]  reg_p0_r_vco_fine_manual
	_phy_rtd_part_outl(0x1800dc04, 27, 24, 0x0); //[DPHY fix]  reg_p0_r_vco_coarse_manual
	_phy_rtd_part_outl(0x1800dc04, 28, 28, 0x0); //[DPHY fix]  reg_p0_r_coarse_calib_manual
	_phy_rtd_part_outl(0x1800dc04, 29, 29, 0x0); //[DPHY fix]  reg_p0_r_acdr_ckfld_en
	_phy_rtd_part_outl(0x1800dc04, 30, 30, 0x0); //[DPHY fix]  reg_p0_r_pll_to_acdr_manual_en
	_phy_rtd_part_outl(0x1800dc04, 31, 31, 0x0); //[DPHY fix]  reg_p0_r_pll_to_acdr_rdy_manual
	_phy_rtd_part_outl(0x1800dc08, 4, 0, 0x01); //[]  reg_p0_r_timer_4
	_phy_rtd_part_outl(0x1800dc08, 7, 5, 0x0); //[]  reg_p0_r_vc_chg_time
	_phy_rtd_part_outl(0x1800dc08, 16, 16, 0x1); //[]  reg_p0_r_old_mode
	_phy_rtd_part_outl(0x1800dc08, 21, 17, 0x10); //[]  reg_p0_r_slope_band
	_phy_rtd_part_outl(0x1800dc08, 22, 22, 0x0); //[]  reg_p0_r_slope_manual
	_phy_rtd_part_outl(0x1800dc08, 23, 23, 0x0); //[]  reg_p0_r_rxidle_bypass
	_phy_rtd_part_outl(0x1800dc08, 24, 24, 0x0); //[]  reg_p0_r_pfd_bypass
	_phy_rtd_part_outl(0x1800dc08, 29, 25, 0x00); //[]  reg_p0_r_pfd_time
	_phy_rtd_part_outl(0x1800dc08, 30, 30, 0x0); //[]  reg_p0_r_pfd_en_manual
	_phy_rtd_part_outl(0x1800dc08, 31, 31, 0x0); //[]  reg_p0_r_start_en_manual
	_phy_rtd_part_outl(0x1800dc0c, 9, 5, 0x08); //[]  reg_p0_r_cp_time_2
	_phy_rtd_part_outl(0x1800dc10, 0, 0, 0x0); //[DPHY fix]  reg_p0_ck_cp2adp_en
	_phy_rtd_part_outl(0x1800dc10, 1, 1, 0x0); //[DPHY fix]  reg_p0_ck_stable_time_mode
	_phy_rtd_part_outl(0x1800dc10, 2, 2, 0x0); //[DPHY fix]  reg_p0_ck_bypass_k_band_mode
	_phy_rtd_part_outl(0x1800dc10, 18, 18, 0x0); //[DPHY fix]  reg_p0_ck_calib_late
	_phy_rtd_part_outl(0x1800dc10, 19, 19, 0x0); //[DPHY fix]  reg_p0_ck_calib_manual
	_phy_rtd_part_outl(0x1800dc10, 22, 20, 0x2); //[DPHY fix]  reg_p0_ck_calib_time
	_phy_rtd_part_outl(0x1800dc10, 27, 23, 0x0c); //script=0x0c ; realsim=0x1
	_phy_rtd_part_outl(0x1800dc10, 28, 28, 0x0); //[DPHY fix]  reg_p0_ck_adap_eq_off
	_phy_rtd_part_outl(0x1800dc10, 29, 29, 0x0); //[DPHY fix]  reg_p0_ck_cp_en_manual
	_phy_rtd_part_outl(0x1800dc10, 30, 30, 0x0); //[DPHY fix]  reg_p0_ck_adp_en_manual
	_phy_rtd_part_outl(0x1800dc10, 31, 31, 0x1); //[DPHY fix]  reg_p0_ck_auto_mode
	_phy_rtd_part_outl(0x1800dc14, 31, 28, 0x8); //[DPHY fix] reg_p0_ck_cp2adp_time
	_phy_rtd_part_outl(0x1800dc18, 1, 1, 0x1); //[] reg_p0_ck_calib_reset_sel
	_phy_rtd_part_outl(0x1800dc18, 3, 2, 0x0); //[] reg_p0_ck_vc_sel
	_phy_rtd_part_outl(0x1800dc18, 5, 4, 0x0); //[] reg_p0_ck_cdr_c
	_phy_rtd_part_outl(0x1800dc18, 11, 6, 0x00); //[] reg_p0_ck_cdr_r
	_phy_rtd_part_outl(0x1800dc1c, 2, 0, 0x0); //[DPHY fix] reg_p0_ck_init_time
	_phy_rtd_part_outl(0x1800dc1c, 7, 3, 0x04); //[DPHY fix] reg_p0_ck_cp_time
	_phy_rtd_part_outl(0x1800dc20, 6, 2, 0x01); //[DPHY fix]  reg_p0_ck_timer_6
	_phy_rtd_part_outl(0x1800dc20, 7, 7, 0x0); //[DPHY fix]  reg_p0_ck_timer_5
	_phy_rtd_part_outl(0x1800dc20, 11, 8, 0x7); //[DPHY fix]  reg_p0_ck_vco_fine_init
	_phy_rtd_part_outl(0x1800dc20, 15, 12, 0x7); //[DPHY fix]  reg_p0_ck_vco_coarse_init
	_phy_rtd_part_outl(0x1800dc20, 16, 16, 0x0); //[DPHY fix]  reg_p0_ck_bypass_coarse_k_mode
	_phy_rtd_part_outl(0x1800dc20, 18, 17, 0x0); //[DPHY fix]  reg_p0_ck_vco_fine_time_sel
	_phy_rtd_part_outl(0x1800dc20, 19, 19, 0x0); //[DPHY fix]  reg_p0_ck_xtal_24m_en
	_phy_rtd_part_outl(0x1800dc20, 23, 20, 0x0); //[DPHY fix]  reg_p0_ck_vco_fine_manual
	_phy_rtd_part_outl(0x1800dc20, 27, 24, 0x0); //[DPHY fix]  reg_p0_ck_vco_coarse_manual
	_phy_rtd_part_outl(0x1800dc20, 28, 28, 0x0); //[DPHY fix]  reg_p0_ck_coarse_calib_manual
	_phy_rtd_part_outl(0x1800dc20, 29, 29, 0x0); //[DPHY fix]  reg_p0_ck_acdr_ckfld_en
	_phy_rtd_part_outl(0x1800dc20, 30, 30, 0x0); //[DPHY fix]  reg_p0_ck_pll_to_acdr_manual_en
	_phy_rtd_part_outl(0x1800dc20, 31, 31, 0x0); //[DPHY fix]  reg_p0_ck_pll_to_acdr_rdy_manual
	_phy_rtd_part_outl(0x1800dc24, 4, 0, 0x01); //[]  reg_p0_ck_timer_4
	_phy_rtd_part_outl(0x1800dc24, 7, 5, 0x0); //[]  reg_p0_ck_vc_chg_time
	_phy_rtd_part_outl(0x1800dc24, 16, 16, 0x1); //[]  reg_p0_ck_old_mode
	_phy_rtd_part_outl(0x1800dc24, 21, 17, 0x10); //[]  reg_p0_ck_slope_band
	_phy_rtd_part_outl(0x1800dc24, 22, 22, 0x0); //[]  reg_p0_ck_slope_manual
	_phy_rtd_part_outl(0x1800dc24, 23, 23, 0x0); //[]  reg_p0_ck_rxidle_bypass
	_phy_rtd_part_outl(0x1800dc24, 24, 24, 0x0); //[]  reg_p0_ck_pfd_bypass
	_phy_rtd_part_outl(0x1800dc24, 29, 25, 0x00); //[]  reg_p0_ck_pfd_time
	_phy_rtd_part_outl(0x1800dc24, 30, 30, 0x0); //[]  reg_p0_ck_pfd_en_manual
	_phy_rtd_part_outl(0x1800dc24, 31, 31, 0x0); //[]  reg_p0_ck_start_en_manual
	_phy_rtd_part_outl(0x1800dc28, 9, 5, 0x08); //[]  reg_p0_ck_cp_time_2
	//MOD
	_phy_rtd_part_outl(0x1800dc2c, 0, 0, 0x1); //[]  reg_p0_ck_md_rstb
	_phy_rtd_part_outl(0x1800dc2c, 1, 1, 0x1); //[]  reg_p0_ck_md_auto
	_phy_rtd_part_outl(0x1800dc2c, 3, 2, 0x0); //[]  reg_p0_ck_md_sel
	_phy_rtd_part_outl(0x1800dc2c, 7, 4, 0x0); //[]  reg_p0_ck_md_adj
	_phy_rtd_part_outl(0x1800dc2c, 13, 8, 0x03); //[]  reg_p0_ck_md_threshold
	_phy_rtd_part_outl(0x1800dc2c, 20, 14, 0x00); //[]  reg_p0_ck_md_debounce
	_phy_rtd_part_outl(0x1800dc2c, 25, 21, 0x00); //[]  reg_p0_ck_error_limit
	_phy_rtd_part_outl(0x1800dc2c, 29, 26, 0x0); //[]  reg_p0_ck_md_reserved
	_phy_rtd_part_outl(0x1800dc30, 2, 0, 0x0); //[]  reg_p0_mod_sel
	//PLLCDR
	_phy_rtd_part_outl(0x1800dc34, 0, 0, 0x0); //[]  reg_p0_b_pllcdr_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 1, 1, 0x0); //[]  reg_p0_g_pllcdr_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 2, 2, 0x0); //[]  reg_p0_r_pllcdr_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 3, 3, 0x0); //[]  reg_p0_ck_pllcdr_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 4, 4, 0x1); //[]  reg_p0_b_pllcdr_vcoband_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 5, 5, 0x1); //[]  reg_p0_g_pllcdr_vcoband_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 6, 6, 0x1); //[]  reg_p0_r_pllcdr_vcoband_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 7, 7, 0x1); //[]  reg_p0_ck_pllcdr_vcoband_manual_enable
	_phy_rtd_part_outl(0x1800dc34, 19, 16, 0x0); //[]  reg_p0_b_acdr_icp_sel_manual
	_phy_rtd_part_outl(0x1800dc34, 23, 20, 0x0); //[]  reg_p0_g_acdr_icp_sel_manual
	_phy_rtd_part_outl(0x1800dc34, 27, 24, 0x0); //[]  reg_p0_r_acdr_icp_sel_manual
	_phy_rtd_part_outl(0x1800dc34, 31, 28, 0x0); //[]  reg_p0_ck_acdr_icp_sel_manual
	_phy_rtd_part_outl(0x1800dc38, 19, 16, 0x0); //[]  reg_p0_b_acdr_vco_fine_i_manual
	_phy_rtd_part_outl(0x1800dc38, 23, 20, 0x0); //[]  reg_p0_g_acdr_vco_fine_i_manual
	_phy_rtd_part_outl(0x1800dc38, 27, 24, 0x0); //[]  reg_p0_r_acdr_vco_fine_i_manual
	_phy_rtd_part_outl(0x1800dc38, 31, 28, 0x0); //[]  reg_p0_ck_acdr_vco_fine_i_manual
	_phy_rtd_part_outl(0x1800dc6c, 0, 0, 0x1); //[DPHY fix]  reg_p0_acdr_en
	_phy_rtd_part_outl(0x1800dc6c, 2, 2, 0x1); //[DPHY fix]  reg_p0_pow_on_manual
	_phy_rtd_part_outl(0x1800dc6c, 3, 3, 0x1); //[DPHY fix]  reg_p0_pow_on_manual_aphy_en
	_phy_rtd_part_outl(0x1800dc6c, 4, 4, 0x0); //[DPHY fix]  reg_p0_pow_on_manual_b_en
	_phy_rtd_part_outl(0x1800dc6c, 5, 5, 0x0); //[DPHY fix]  reg_p0_pow_on_manual_g_en
	_phy_rtd_part_outl(0x1800dc6c, 6, 6, 0x0); //[DPHY fix]  reg_p0_pow_on_manual_r_en
	_phy_rtd_part_outl(0x1800dc6c, 7, 7, 0x0); //[DPHY fix]  reg_p0_pow_on_manual_ck_en
	_phy_rtd_part_outl(0x1800dc6c, 8, 8, 0x1); //[DPHY fix]  reg_p0_pow_save_bypass_b
	_phy_rtd_part_outl(0x1800dc6c, 9, 9, 0x1); //[DPHY fix]  reg_p0_pow_save_bypass_g
	_phy_rtd_part_outl(0x1800dc6c, 10, 10, 0x1); //[DPHY fix]  reg_p0_pow_save_bypass_r
	_phy_rtd_part_outl(0x1800dc6c, 11, 11, 0x1); //[DPHY fix]  reg_p0_pow_save_bypass_ck
	_phy_rtd_part_outl(0x1800dc6c, 12, 12, 0x0); //[DPHY fix]  reg_p0_pow_save_xtal_24m_enable
	_phy_rtd_part_outl(0x1800dc6c, 14, 13, 0x3); //[DPHY fix]  reg_p0_pow_save_rst_dly_sel
	_phy_rtd_part_outl(0x1800dc70, 17, 16, 0x0); //[DPHY fix]  reg_tap1_mask
	_phy_rtd_part_outl(0x1800dc7c, 31, 31, 0x0); //[DPHY fix]  reg_port_out_sel
	_phy_rtd_part_outl(0x1800dc80, 7, 0, 0x00); //[]  reg_p0_dig_reserved_3
	_phy_rtd_part_outl(0x1800dc80, 15, 8, 0x00); //[]  reg_p0_dig_reserved_2
	_phy_rtd_part_outl(0x1800dc80, 23, 16, 0x00); //[]  reg_p0_dig_reserved_1
	_phy_rtd_part_outl(0x1800dc80, 31, 24, 0x00); //[]  reg_p0_dig_reserved_0
}

void DPHY_Para_frl_1_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	// DPHY para start
	_phy_rtd_part_outl(0x1800dbc0, 31, 12, DPHY_Para_frl_1[frl_timing].b_cdr_cp_2_1); //[] reg_p0_b_cdr_cp
	_phy_rtd_part_outl(0x1800dbe0, 31, 12, DPHY_Para_frl_1[frl_timing].g_cdr_cp_3_1); //[] reg_p0_g_cdr_cp
	_phy_rtd_part_outl(0x1800dbfc, 31, 12, DPHY_Para_frl_1[frl_timing].r_cdr_cp_4_1); //[] reg_p0_r_cdr_cp
	_phy_rtd_part_outl(0x1800dc18, 31, 12, DPHY_Para_frl_1[frl_timing].ck_cdr_cp_5_1); //[] reg_p0_ck_cdr_cp
	_phy_rtd_part_outl(0x1800db00, 17, 16, DPHY_Para_frl_1[frl_timing].b_rate_sel_6_1); //[DPHY para]  reg_p0_b_rate_sel
	_phy_rtd_part_outl(0x1800db00, 19, 18, DPHY_Para_frl_1[frl_timing].g_rate_sel_7_1); //[DPHY para]  reg_p0_g_rate_sel
	_phy_rtd_part_outl(0x1800db00, 21, 20, DPHY_Para_frl_1[frl_timing].r_rate_sel_8_1); //[DPHY para]  reg_p0_r_rate_sel
	_phy_rtd_part_outl(0x1800db00, 23, 22, DPHY_Para_frl_1[frl_timing].ck_rate_sel_9_1); //[DPHY para]  reg_p0_ck_rate_sel
	_phy_rtd_part_outl(0x1800db88, 1, 1, 0x0); //[DPHY para]  reg_p0_b_clk_div2_en
	_phy_rtd_part_outl(0x1800db88, 9, 9, 0x0); //[DPHY para]  reg_p0_g_clk_div2_en
	_phy_rtd_part_outl(0x1800db88, 17, 17, 0x0); //[DPHY para]  reg_p0_r_clk_div2_en
	_phy_rtd_part_outl(0x1800db88, 25, 25, 0x0); //[DPHY para]  reg_p0_ck_clk_div2_en
	_phy_rtd_part_outl(0x1800db8c, 25, 24, 0x1); //[DPHY para]  reg_p0_b_format_sel
	_phy_rtd_part_outl(0x1800db8c, 27, 26, 0x1); //[DPHY para]  reg_p0_g_format_sel
	_phy_rtd_part_outl(0x1800db8c, 29, 28, 0x1); //[DPHY para]  reg_p0_r_format_sel
	_phy_rtd_part_outl(0x1800db8c, 31, 30, 0x1); //[DPHY para]  reg_p0_ck_format_sel
	_phy_rtd_part_outl(0x1800dc3c, 7, 0, 0x0a); //[]  reg_p0_b_acdr_pll_config_1
	_phy_rtd_part_outl(0x1800dc3c, 15, 8, 0xac); //[]  reg_p0_b_acdr_pll_config_2
	_phy_rtd_part_outl(0x1800dc3c, 23, 16, 0x0f); //[]  reg_p0_b_acdr_pll_config_3
	_phy_rtd_part_outl(0x1800dc3c, 31, 24, 0x00); //[]  reg_p0_b_acdr_pll_config_4
	_phy_rtd_part_outl(0x1800dc40, 7, 0, 0x0b); //[]  reg_p0_b_acdr_cdr_config_1
	_phy_rtd_part_outl(0x1800dc40, 15, 8, 0x81); //[]  reg_p0_b_acdr_cdr_config_2
	_phy_rtd_part_outl(0x1800dc40, 23, 16, 0x07); //0x03 for JT
	_phy_rtd_part_outl(0x1800dc40, 31, 24, 0x00); //[]  reg_p0_b_acdr_cdr_config_4
	_phy_rtd_part_outl(0x1800dc44, 7, 0, 0x00); //[]  reg_p0_b_acdr_manual_config_1
	_phy_rtd_part_outl(0x1800dc44, 15, 8, 0x00); //[]  reg_p0_b_acdr_manual_config_2
	_phy_rtd_part_outl(0x1800dc44, 23, 16, 0x00); //[]  reg_p0_b_acdr_manual_config_3
	_phy_rtd_part_outl(0x1800dc44, 31, 24, 0x00); //[]  reg_p0_b_acdr_manual_config_4
	_phy_rtd_part_outl(0x1800dc48, 7, 0, 0x0a); //[]  reg_p0_g_acdr_pll_config_1
	_phy_rtd_part_outl(0x1800dc48, 15, 8, 0xac); //[]  reg_p0_g_acdr_pll_config_2
	_phy_rtd_part_outl(0x1800dc48, 23, 16, 0x0f); //[]  reg_p0_g_acdr_pll_config_3
	_phy_rtd_part_outl(0x1800dc48, 31, 24, 0x00); //[]  reg_p0_g_acdr_pll_config_4
	_phy_rtd_part_outl(0x1800dc4c, 7, 0, 0x0b); //[]  reg_p0_g_acdr_cdr_config_1
	_phy_rtd_part_outl(0x1800dc4c, 15, 8, 0x81); //[]  reg_p0_g_acdr_cdr_config_2
	_phy_rtd_part_outl(0x1800dc4c, 23, 16, 0x07); //0x03 for JT
	_phy_rtd_part_outl(0x1800dc4c, 31, 24, 0x00); //[]  reg_p0_g_acdr_cdr_config_4
	_phy_rtd_part_outl(0x1800dc50, 7, 0, 0x00); //[]  reg_p0_g_acdr_manual_config_1
	_phy_rtd_part_outl(0x1800dc50, 15, 8, 0x00); //[]  reg_p0_g_acdr_manual_config_2
	_phy_rtd_part_outl(0x1800dc50, 23, 16, 0x00); //[]  reg_p0_g_acdr_manual_config_3
	_phy_rtd_part_outl(0x1800dc50, 31, 24, 0x00); //[]  reg_p0_g_acdr_manual_config_4
	_phy_rtd_part_outl(0x1800dc54, 7, 0, 0x0a); //[]  reg_p0_r_acdr_pll_config_1
	_phy_rtd_part_outl(0x1800dc54, 15, 8, 0xac); //[]  reg_p0_r_acdr_pll_config_2
	_phy_rtd_part_outl(0x1800dc54, 23, 16, 0x0f); //[]  reg_p0_r_acdr_pll_config_3
	_phy_rtd_part_outl(0x1800dc54, 31, 24, 0x00); //[]  reg_p0_r_acdr_pll_config_4
	_phy_rtd_part_outl(0x1800dc58, 7, 0, 0x0b); //[]  reg_p0_r_acdr_cdr_config_1
	_phy_rtd_part_outl(0x1800dc58, 15, 8, 0x81); //[]  reg_p0_r_acdr_cdr_config_2
	_phy_rtd_part_outl(0x1800dc58, 23, 16, 0x07); //0x03 for JT
	_phy_rtd_part_outl(0x1800dc58, 31, 24, 0x00); //[]  reg_p0_r_acdr_cdr_config_4
	_phy_rtd_part_outl(0x1800dc5c, 7, 0, 0x00); //[]  reg_p0_r_acdr_manual_config_1
	_phy_rtd_part_outl(0x1800dc5c, 15, 8, 0x00); //[]  reg_p0_r_acdr_manual_config_2
	_phy_rtd_part_outl(0x1800dc5c, 23, 16, 0x00); //[]  reg_p0_r_acdr_manual_config_3
	_phy_rtd_part_outl(0x1800dc5c, 31, 24, 0x00); //[]  reg_p0_r_acdr_manual_config_4
	_phy_rtd_part_outl(0x1800dc60, 7, 0, 0x0e); //[]  reg_p0_ck_acdr_pll_config_1
	_phy_rtd_part_outl(0x1800dc60, 15, 8, 0xbc); //[]  reg_p0_ck_acdr_pll_config_2
	_phy_rtd_part_outl(0x1800dc60, 23, 16, 0x0f); //[]  reg_p0_ck_acdr_pll_config_3
	_phy_rtd_part_outl(0x1800dc60, 31, 24, 0x00); //[]  reg_p0_ck_acdr_pll_config_4
	_phy_rtd_part_outl(0x1800dc64, 7, 0, 0x0b); //[]  reg_p0_ck_acdr_cdr_config_1
	_phy_rtd_part_outl(0x1800dc64, 15, 8, 0x81); //[]  reg_p0_ck_acdr_cdr_config_2
	_phy_rtd_part_outl(0x1800dc64, 23, 16, 0x07); //0x03 for JT
	_phy_rtd_part_outl(0x1800dc64, 31, 24, 0x00); //[]  reg_p0_ck_acdr_cdr_config_4
	_phy_rtd_part_outl(0x1800dc68, 7, 0, 0x00); //[]  reg_p0_ck_acdr_manual_config_1
	_phy_rtd_part_outl(0x1800dc68, 15, 8, 0x00); //[]  reg_p0_ck_acdr_manual_config_2
	_phy_rtd_part_outl(0x1800dc68, 23, 16, 0x00); //[]  reg_p0_ck_acdr_manual_config_3
	_phy_rtd_part_outl(0x1800dc68, 31, 24, 0x00); //[]  reg_p0_ck_acdr_manual_config_4
}

void APHY_Fix_frl_1_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	//APHY fix start
	//_phy_rtd_part_outl(0x1800da04, 4, 0, 0x08); //[]  REG_Z0_ADJR_0  set z0 from otp value in lib_hdmi21_z0_calibration
	_phy_rtd_part_outl(0x1800da04, 5, 5, 0x01); //[]  REG_P0_Z0_Z0POW_FIX
	_phy_rtd_part_outl(0x1800da10, 25, 24, 0x3); //[]  0x3 Strong level	
	_phy_rtd_part_outl(0x1800da10, 28, 28, 0x01); //[]  REG_P0_Z0_Z0POW_CK
	_phy_rtd_part_outl(0x1800da10, 29, 29, 0x01); //[]  REG_P0_Z0_Z0POW_B
	_phy_rtd_part_outl(0x1800da10, 30, 30, 0x01); //[]  REG_P0_Z0_Z0POW_G
	_phy_rtd_part_outl(0x1800da10, 31, 31, 0x01); //[]  REG_P0_Z0_Z0POW_R
	_phy_rtd_part_outl(0x1800da04, 6, 6, 0x0); //[]  Z0_P_OFF
	_phy_rtd_part_outl(0x1800da04, 7, 7, 0x0); //[]  Z0_N_OFF
	_phy_rtd_part_outl(0x1800da04, 9, 9, 0x0); //[]  REG_Z0_FT_PN_SHORT_EN
	_phy_rtd_part_outl(0x1800da10, 0, 0, 0x0); //[]  TOP_IN_1, FAST_SW_SEL
	_phy_rtd_part_outl(0x1800da10, 1, 1, 0x0); //[]  TOP_IN_1, DIV_EN
	_phy_rtd_part_outl(0x1800da10, 4, 2, 0x0); //[]  TOP_IN_1, DIV_SEL
	_phy_rtd_part_outl(0x1800da10, 5, 5, 0x0); //[]  TOP_IN_1, V_STROBE_EN
	_phy_rtd_part_outl(0x1800da10, 6, 6, 0x0); //[]  TOP_IN_1, CK_TX to B/G/R EN
	_phy_rtd_part_outl(0x1800da10, 7, 7, 0x0); //[]  TOP_IN_1, dummy
	_phy_rtd_part_outl(0x1800da10, 11, 8, 0x0); //[] TOP_IN_2, test signal sel
	_phy_rtd_part_outl(0x1800da10, 13, 12, 0x0); //[] TOP_IN_2, test mode sel
	_phy_rtd_part_outl(0x1800da10, 14, 14, 0x1); //[] TOP_IN_2, CK_TX EN
	_phy_rtd_part_outl(0x1800da10, 15, 15, 0x0); //[] TOP_IN_2, CK_MD SEL
	_phy_rtd_part_outl(0x1800da10, 23, 16, 0x1); //[]  TOP_IN_3, CKDET
	_phy_rtd_part_outl(0x1800da14, 7, 0, 0x0); //[]  REG_TOP_IN_5 dummy
	_phy_rtd_part_outl(0x1800da18, 0, 0, 0x1); //[APHY fix]  REG_HDMIRX_BIAS_EN bandgap reference power
	_phy_rtd_part_outl(0x1800da18, 10, 8, 0x0); //[APHY fix]  REG_BG_RBGLOOP2 dummy
	_phy_rtd_part_outl(0x1800da18, 13, 11, 0x0); //[APHY fix]  REG_BG_RBG dummy
	_phy_rtd_part_outl(0x1800da18, 15, 14, 0x0); //[APHY fix]  REG_BG_RBG2 dummy
	_phy_rtd_part_outl(0x1800da18, 16, 16, 0x0); //[APHY fix]  REG_BG_POW dummy
	_phy_rtd_part_outl(0x1800da18, 17, 17, 0x0); //[APHY fix]  REG_BG_ENVBGUP
	_phy_rtd_part_outl(0x1800da20, 7, 0, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da20, 15, 8, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da20, 23, 16, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da20, 31, 24, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da24, 7, 0, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da24, 15, 8, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da24, 23, 16, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da24, 31, 24, 0x00); //[]  dummy
	_phy_rtd_part_outl(0x1800da68, 22, 20, 4); //[DPHY fix] REG_P0_ACDR_B_7[6:4] (VCO SEL_BAND_V2I)
	_phy_rtd_part_outl(0x1800da88, 22, 20, 4); //[DPHY fix] REG_P0_ACDR_G_7[6:4] (VCO SEL_BAND_V2I)
	_phy_rtd_part_outl(0x1800daa8, 22, 20, 4); //[DPHY fix] REG_P0_ACDR_R_7[6:4] (VCO SEL_BAND_V2I)
	_phy_rtd_part_outl(0x1800da48, 22, 20, 4); //[DPHY fix] REG_P0_ACDR_CK_7[6:4] (VCO SEL_BAND_V2I)
	_phy_rtd_part_outl(0x1800da30, 0, 0, 0x0); //[APHY fix]  CK-Lane input off EN
	_phy_rtd_part_outl(0x1800da30, 1, 1, 0x0); //[APHY fix]  CK-Lane INPOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da30, 2, 2, 0x0); //[APHY fix]  CK-Lane INNOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da30, 3, 3, 0x1); //[APHY fix]  CK-Lane POW_AC_COUPLE
	_phy_rtd_part_outl(0x1800da30, 5, 4, 0x1); //[APHY fix]  CK-Lane RXVCM_SEL[1:0]
	_phy_rtd_part_outl(0x1800da34, 21, 21, 0x1); //[APHY fix]   	CK-Lane RS_CAL_EN
	_phy_rtd_part_outl(0x1800da34, 25, 25, 0x1); //[APHY fix]   	CK-Lane POW_DATALANE_BIAS
	_phy_rtd_part_outl(0x1800da34, 26, 26, 0x0); //[APHY fix]   	CK-Lane REG_FORCE_STARTUP(Const-GM)
	_phy_rtd_part_outl(0x1800da34, 27, 27, 0x0); //[APHY fix]   	CK-Lane REG_POWB_STARTUP
	_phy_rtd_part_outl(0x1800da38, 8, 8, 0x1); //[APHY fix]  		CK-Lane POW_DFE
	_phy_rtd_part_outl(0x1800da38, 14, 12, 0x6); //[APHY fix] 		CK-Lane DFE_SUMAMP_ ISEL
	_phy_rtd_part_outl(0x1800da38, 15, 15, 0x0); //[APHY fix] 		CK-Lane DFE_SUMAMP_DCGAIN_MAX
	_phy_rtd_part_outl(0x1800da38, 19, 18, 0x0); //[APHY fix] 		CK-Lane Dummy
	_phy_rtd_part_outl(0x1800da38, 20, 20, 0x0); //[APHY fix] 		CK-Lane DFE CKI_DELAY_EN
	_phy_rtd_part_outl(0x1800da38, 21, 21, 0x0); //[APHY fix] 		CK-Lane DFE CKIB_DELAY_EN
	_phy_rtd_part_outl(0x1800da38, 22, 22, 0x0); //[APHY fix] 		CK-Lane DFE CKQ_DELAY_EN
	_phy_rtd_part_outl(0x1800da38, 23, 23, 0x0); //[APHY fix] 		CK-Lane DFE CKQB_DELAY_EN
	_phy_rtd_part_outl(0x1800da38, 24, 24, 0x0); //[APHY fix] 		CK-Lane EN_EYE_MNT
	_phy_rtd_part_outl(0x1800da38, 25, 25, 0x0); //[APHY fix] 		CK-Lane DEMUX input clock phase select:(for eye mnt)
	_phy_rtd_part_outl(0x1800da38, 26, 26, 0x1); //[APHY fix] 		CK-Lane VTH+OFFSET select. 0:VTH, 1:VTH+OFFSET
	_phy_rtd_part_outl(0x1800da38, 27, 27, 0x0); //[APHY fix] 		CK-Lane DA_EG_VOS_PULLLOW(koffset disable)
	_phy_rtd_part_outl(0x1800da38, 31, 29, 0x0); //[APHY fix] 			CK-Lane dummy
	_phy_rtd_part_outl(0x1800da3c, 2, 0, 0x0); //[APHY fix]  	CK-Lane DFE_TAP_DELAY
	_phy_rtd_part_outl(0x1800da3c, 7, 7, 0x1); //[APHY fix]   		CK-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da3c, 8, 8, 0x0); //[APHY fix]   	CK-Lane rstb of dtata-lane test div8
	_phy_rtd_part_outl(0x1800da3c, 16, 16, 0x1); //[APHY fix]  		CK-Lane TRANSITION CNT enable.
	_phy_rtd_part_outl(0x1800da3c, 17, 17, 0x0); //[APHY fix]  		CK-Lane QCG1 injection-locked EN
	_phy_rtd_part_outl(0x1800da3c, 18, 18, 0x1); //[APHY fix]  		CK-Lane QCG2_EN
	_phy_rtd_part_outl(0x1800da3c, 19, 19, 0x0); //[APHY fix]  		CK-Lane QCG2 injection-locked EN
	_phy_rtd_part_outl(0x1800da3c, 20, 20, 0x1); //[APHY fix]  		CK-Lane QCG FLD divider(/8) rstb
	_phy_rtd_part_outl(0x1800da3c, 21, 21, 0x1); //[APHY fix]  		CK-Lane QCG FLD SEL
	_phy_rtd_part_outl(0x1800da3c, 22, 22, 0x0); //[APHY fix]  		CK-Lane CK_FLD SEL.
	_phy_rtd_part_outl(0x1800da3c, 23, 23, 0x0); //[APHY fix]  		CK-Lane CK-lane clk to B/G/R EN.
	_phy_rtd_part_outl(0x1800da40, 8, 8, 0x0); //[APHY fix]  CK-Lane PI_EN
	_phy_rtd_part_outl(0x1800da40, 12, 12, 0x0); //[APHY fix]  CK-Lane QCG1 Cload contrl SEL
	_phy_rtd_part_outl(0x1800da40, 13, 13, 0x0); //[APHY fix]  CK-Lane QCG2 Cload contrl SEL
	_phy_rtd_part_outl(0x1800da40, 15, 15, 0x0); //[APHY fix]  CK-Lane BIAS_PI_CUR_SEL
	_phy_rtd_part_outl(0x1800da40, 21, 21, 0x0); //[APHY fix]  CK-Lane PI_EYE_EN
	_phy_rtd_part_outl(0x1800da40, 23, 23, 0x0); //[APHY fix]  CK-Lane KI SEL
	_phy_rtd_part_outl(0x1800da40, 25, 25, 0x0); //[APHY fix]  CK-Lane FKP_RSTB_SEL
	_phy_rtd_part_outl(0x1800da40, 27, 26, 0x0); //[APHY fix]  CK-Lane KI_CK_SEL
	_phy_rtd_part_outl(0x1800da40, 29, 29, 0x0); //[APHY fix]  CK-Lane ST_CODE SEL
	_phy_rtd_part_outl(0x1800da40, 30, 30, 0x0); //[APHY fix]  CK-Lane QCG Ckin SEL.
	_phy_rtd_part_outl(0x1800da44, 3, 2, 0x1); //[APHY fix]  CK-Lane ACDR_SEL_DIV_FLD[1:0]
	_phy_rtd_part_outl(0x1800da44, 5, 4, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_DIV_REF[1:0]
	_phy_rtd_part_outl(0x1800da44, 7, 6, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_DIV_PLL[1:0]
	_phy_rtd_part_outl(0x1800da44, 9, 8, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_DIV_TRAINING[1:0]
	_phy_rtd_part_outl(0x1800da44, 12, 12, 0x0); //[APHY fix]  CK-Lane dummy
	_phy_rtd_part_outl(0x1800da44, 13, 13, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_FLD_0CKFB_1CKREF
	_phy_rtd_part_outl(0x1800da44, 15, 14, 0x0); //[APHY fix]  CK-Lane no use
	_phy_rtd_part_outl(0x1800da44, 18, 18, 0x1); //[APHY fix]  CK-Lane ACDR_RSTB_DIV_PLL ... removed
	_phy_rtd_part_outl(0x1800da44, 24, 24, 0x0); //[APHY fix]  CK-Lane ACDR_EN_UPDN_PULSE_FILTER
	_phy_rtd_part_outl(0x1800da44, 25, 25, 0x1); //[APHY fix]  CK-Lane ACDR_RSTB_UPDN ... dummy
	_phy_rtd_part_outl(0x1800da44, 27, 26, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_UPDN_WIDTH[1:0]
	_phy_rtd_part_outl(0x1800da44, 28, 28, 0x1); //[APHY fix]  CK-Lane ACDR_POW_LPF_IDAC
	_phy_rtd_part_outl(0x1800da44, 31, 29, 0x3); //[APHY fix]  CK-Lane ACDR_SEL_LPF_IDAC[2:0]
	_phy_rtd_part_outl(0x1800da48, 0, 0, 0x1); //[APHY fix]  CK-Lane ACDR_POW_CP
	_phy_rtd_part_outl(0x1800da48, 2, 2, 0x1); //[APHY fix]  CK-Lane ACDR_POW_IDN_BBPD
	_phy_rtd_part_outl(0x1800da48, 3, 3, 0x0); //[APHY fix]  CK-Lane no use
	_phy_rtd_part_outl(0x1800da48, 6, 4, 0x7); //[APHY fix]  CK-Lane ACDR_SEL_TIE_IDN_BBPD
	_phy_rtd_part_outl(0x1800da48, 7, 7, 0x0); //[APHY fix]  CK-Lane ACDR_POW_IBIAS_IDN_HV
	_phy_rtd_part_outl(0x1800da48, 8, 8, 0x1); //[APHY fix]  CK-Lane ACDR_POW_VCO
	_phy_rtd_part_outl(0x1800da48, 9, 9, 0x1); //[APHY fix]  CK-Lane ACDR_POW_VCO_VDAC
	_phy_rtd_part_outl(0x1800da48, 11, 10, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_V15_VDAC
	_phy_rtd_part_outl(0x1800da48, 14, 14, 0x0); //[APHY fix]  CK-Lane no use
	_phy_rtd_part_outl(0x1800da48, 15, 15, 0x0); //[APHY fix]  CK-Lane no use
	_phy_rtd_part_outl(0x1800da48, 16, 16, 0x0); //[APHY fix]  CK-Lane ACDR_POW_TEST_MODE
	_phy_rtd_part_outl(0x1800da48, 18, 17, 0x0); //[APHY fix]  CK-Lane ACDR_SEL_TEST_MODE
	_phy_rtd_part_outl(0x1800da48, 31, 31, 0x0); //[APHY fix]  CK-Lane no use
	_phy_rtd_part_outl(0x1800da4c, 15, 13, 0x0); //[APHY fix]  CK-Lane no use
	_phy_rtd_part_outl(0x1800da4c, 19, 16, 0x0); //[APHY fix]  CK-Lane LE1_ISEL_IN_2 -> dummy
	_phy_rtd_part_outl(0x1800da4c, 31, 25, 0x44); //[APHY fix]  CK-Lane no use for LEQ
	_phy_rtd_part_outl(0x1800da50, 0, 0, 0x0); //[APHY fix]  B-Lane input off EN
	_phy_rtd_part_outl(0x1800da50, 1, 1, 0x0); //[APHY fix]  		B-Lane INPOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da50, 2, 2, 0x0); //[APHY fix]  		B-Lane INNOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da50, 3, 3, 0x1); //[APHY fix]  		B-Lane POW_AC_COUPLE
	_phy_rtd_part_outl(0x1800da50, 5, 4, 0x1); //[APHY fix]  		B-Lane RXVCM_SEL[1:0]
	_phy_rtd_part_outl(0x1800da54, 21, 21, 0x1); //[APHY fix]   		B-Lane RS_CAL_EN
	_phy_rtd_part_outl(0x1800da54, 25, 25, 0x1); //[APHY fix]   		B-Lane POW_DATALANE_BIAS
	_phy_rtd_part_outl(0x1800da54, 26, 26, 0x0); //[APHY fix]   		B-Lane REG_FORCE_STARTUP(Const-GM)
	_phy_rtd_part_outl(0x1800da54, 27, 27, 0x0); //[APHY fix]   		B-Lane REG_POWB_STARTUP
	_phy_rtd_part_outl(0x1800da58, 8, 8, 0x1); //[APHY fix]  		B-Lane POW_DFE
	_phy_rtd_part_outl(0x1800da58, 14, 12, 0x6); //[APHY fix] 		B-Lane DFE_SUMAMP_ ISEL
	_phy_rtd_part_outl(0x1800da58, 15, 15, 0x0); //[APHY fix] 		B-Lane DFE_SUMAMP_DCGAIN_MAX
	_phy_rtd_part_outl(0x1800da58, 19, 18, 0x0); //[APHY fix] 		B-Lane Dummy
	_phy_rtd_part_outl(0x1800da58, 20, 20, 0x0); //[APHY fix] 		B-Lane DFE CKI_DELAY_EN
	_phy_rtd_part_outl(0x1800da58, 21, 21, 0x0); //[APHY fix] 		B-Lane DFE CKIB_DELAY_EN
	_phy_rtd_part_outl(0x1800da58, 22, 22, 0x0); //[APHY fix] 		B-Lane DFE CKQ_DELAY_EN
	_phy_rtd_part_outl(0x1800da58, 23, 23, 0x0); //[APHY fix] 		B-Lane DFE CKQB_DELAY_EN
	_phy_rtd_part_outl(0x1800da58, 24, 24, 0x0); //[APHY fix] 		B-Lane EN_EYE_MNT
	_phy_rtd_part_outl(0x1800da58, 25, 25, 0x0); //[APHY fix] 		B-Lane DEMUX input clock phase select:(for eye mnt)
	_phy_rtd_part_outl(0x1800da58, 26, 26, 0x1); //[APHY fix] 		B-Lane VTH+OFFSET select. 0:VTH, 1:VTH+OFFSET
	_phy_rtd_part_outl(0x1800da58, 27, 27, 0x0); //[APHY fix] 		B-Lane DA_EG_VOS_PULLLOW(koffset disable)
	_phy_rtd_part_outl(0x1800da58, 31, 29, 0x0); //[APHY fix] 			B-Lane dummy
	_phy_rtd_part_outl(0x1800da5c, 2, 0, 0x0); //[APHY fix]  	B-Lane DFE_TAP_DELAY
	_phy_rtd_part_outl(0x1800da5c, 7, 7, 0x1); //[APHY fix]   		B-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da5c, 8, 8, 0x0); //[APHY fix]   	B-Lane rstb of dtata-lane test div8
	_phy_rtd_part_outl(0x1800da5c, 16, 16, 0x1); //[APHY fix]  		B-Lane TRANSITION CNT enable.
	_phy_rtd_part_outl(0x1800da5c, 17, 17, 0x0); //[APHY fix]  		B-Lane QCG1 injection-locked EN
	_phy_rtd_part_outl(0x1800da5c, 18, 18, 0x1); //[APHY fix]  		B-Lane QCG2_EN
	_phy_rtd_part_outl(0x1800da5c, 19, 19, 0x0); //[APHY fix]  		B-Lane QCG2 injection-locked EN
	_phy_rtd_part_outl(0x1800da5c, 20, 20, 0x1); //[APHY fix]  		B-Lane QCG FLD divider(/8) rstb
	_phy_rtd_part_outl(0x1800da5c, 21, 21, 0x1); //[APHY fix]  		B-Lane QCG FLD SEL
	_phy_rtd_part_outl(0x1800da5c, 22, 22, 0x0); //[APHY fix]  		B-Lane CK_FLD SEL.
	_phy_rtd_part_outl(0x1800da5c, 23, 23, 0x0); //[APHY fix]  		B-Lane CK-lane clk to B/G/R EN.
	_phy_rtd_part_outl(0x1800da60, 8, 8, 0x0); //[APHY fix]  B-Lane PI_EN
	_phy_rtd_part_outl(0x1800da60, 12, 12, 0x0); //[APHY fix]  B-Lane QCG1 Cload contrl SEL
	_phy_rtd_part_outl(0x1800da60, 13, 13, 0x0); //[APHY fix]  B-Lane QCG2 Cload contrl SEL
	_phy_rtd_part_outl(0x1800da60, 15, 15, 0x0); //[APHY fix]  B-Lane BIAS_PI_CUR_SEL
	_phy_rtd_part_outl(0x1800da60, 21, 21, 0x0); //[APHY fix]  B-Lane PI_EYE_EN
	_phy_rtd_part_outl(0x1800da60, 23, 23, 0x0); //[APHY fix]  B-Lane KI SEL
	_phy_rtd_part_outl(0x1800da60, 25, 25, 0x0); //[APHY fix]  B-Lane FKP_RSTB_SEL
	_phy_rtd_part_outl(0x1800da60, 27, 26, 0x0); //[APHY fix]  B-Lane KI_CK_SEL
	_phy_rtd_part_outl(0x1800da60, 29, 29, 0x0); //[APHY fix]  B-Lane ST_CODE SEL
	_phy_rtd_part_outl(0x1800da60, 30, 30, 0x0); //[APHY fix]  B-Lane QCG Ckin SEL.
	_phy_rtd_part_outl(0x1800da64, 3, 2, 0x1); //[APHY fix]  B-Lane ACDR_SEL_DIV_FLD[1:0]
	_phy_rtd_part_outl(0x1800da64, 5, 4, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_REF[1:0]
	_phy_rtd_part_outl(0x1800da64, 7, 6, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_PLL[1:0]
	_phy_rtd_part_outl(0x1800da64, 9, 8, 0x0); //[APHY fix]  B-Lane ACDR_SEL_DIV_TRAINING[1:0]
	_phy_rtd_part_outl(0x1800da64, 12, 12, 0x0); //[APHY fix]  B-Lane dummy
	_phy_rtd_part_outl(0x1800da64, 13, 13, 0x0); //[APHY fix]  B-Lane ACDR_SEL_FLD_0CKFB_1CKREF
	_phy_rtd_part_outl(0x1800da64, 15, 14, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da64, 18, 18, 0x1); //[APHY fix]  B-Lane ACDR_RSTB_DIV_PLL ... removed
	_phy_rtd_part_outl(0x1800da64, 24, 24, 0x0); //[APHY fix]  B-Lane ACDR_EN_UPDN_PULSE_FILTER
	_phy_rtd_part_outl(0x1800da64, 25, 25, 0x1); //[APHY fix]  B-Lane ACDR_RSTB_UPDN ... dummy
	_phy_rtd_part_outl(0x1800da64, 27, 26, 0x0); //[APHY fix]  B-Lane ACDR_SEL_UPDN_WIDTH[1:0]
	_phy_rtd_part_outl(0x1800da64, 28, 28, 0x1); //[APHY fix]  B-Lane ACDR_POW_LPF_IDAC
	_phy_rtd_part_outl(0x1800da64, 31, 29, 0x3); //[APHY fix]  B-Lane ACDR_SEL_LPF_IDAC[2:0]
	_phy_rtd_part_outl(0x1800da68, 0, 0, 0x1); //[APHY fix]  B-Lane ACDR_POW_CP
	_phy_rtd_part_outl(0x1800da68, 2, 2, 0x1); //[APHY fix]  B-Lane ACDR_POW_IDN_BBPD
	_phy_rtd_part_outl(0x1800da68, 3, 3, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da68, 6, 4, 0x7); //[APHY fix]  B-Lane ACDR_SEL_TIE_IDN_BBPD
	_phy_rtd_part_outl(0x1800da68, 7, 7, 0x0); //[APHY fix]  B-Lane ACDR_POW_IBIAS_IDN_HV
	_phy_rtd_part_outl(0x1800da68, 8, 8, 0x1); //[APHY fix]  B-Lane ACDR_POW_VCO
	_phy_rtd_part_outl(0x1800da68, 9, 9, 0x1); //[APHY fix]  B-Lane ACDR_POW_VCO_VDAC
	_phy_rtd_part_outl(0x1800da68, 11, 10, 0x0); //[APHY fix]  B-Lane ACDR_SEL_V15_VDAC
	_phy_rtd_part_outl(0x1800da68, 14, 14, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da68, 15, 15, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da68, 16, 16, 0x0); //[APHY fix]  B-Lane ACDR_POW_TEST_MODE
	_phy_rtd_part_outl(0x1800da68, 18, 17, 0x0); //[APHY fix]  B-Lane ACDR_SEL_TEST_MODE
	_phy_rtd_part_outl(0x1800da68, 31, 31, 0x0); //[APHY fix]  B-Lane no use
	_phy_rtd_part_outl(0x1800da6c, 13, 13, 0x1); //[APHY fix]  B-Lane ckref sel. 0:xtal, 1:tx or train clk
	_phy_rtd_part_outl(0x1800da6c, 14, 14, 0x0); //[APHY fix]  B-Lane ckref sel. 0:train_clk, 1:tx_clk
	_phy_rtd_part_outl(0x1800da6c, 15, 15, 0x1); //[APHY fix]  B-Lane ckbk sel. 0:M-DIV, 1:PRESCALER
	_phy_rtd_part_outl(0x1800da6c, 19, 16, 0x0); //[APHY fix]  B-Lane LE1_ISEL_IN_2 -> dummy
	_phy_rtd_part_outl(0x1800da6c, 31, 25, 0x41); //[APHY fix]  B-Lane no use for LEQ
	_phy_rtd_part_outl(0x1800da70, 0, 0, 0x0); //[APHY fix]  G-Lane input off EN
	_phy_rtd_part_outl(0x1800da70, 1, 1, 0x0); //[APHY fix]  		G-Lane INPOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da70, 2, 2, 0x0); //[APHY fix]  		G-Lane INNOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da70, 3, 3, 0x1); //[APHY fix]  		G-Lane POW_AC_COUPLE
	_phy_rtd_part_outl(0x1800da70, 5, 4, 0x1); //[APHY fix]  		G-Lane RXVCM_SEL[1:0]
	_phy_rtd_part_outl(0x1800da74, 21, 21, 0x1); //[APHY fix]   		G-Lane RS_CAL_EN
	_phy_rtd_part_outl(0x1800da74, 25, 25, 0x1); //[APHY fix]   		G-Lane POW_DATALANE_BIAS
	_phy_rtd_part_outl(0x1800da74, 26, 26, 0x0); //[APHY fix]   		G-Lane REG_FORCE_STARTUP(Const-GM)
	_phy_rtd_part_outl(0x1800da74, 27, 27, 0x0); //[APHY fix]   		G-Lane REG_POWB_STARTUP
	_phy_rtd_part_outl(0x1800da78, 8, 8, 0x1); //[APHY fix]  		G-Lane POW_DFE
	_phy_rtd_part_outl(0x1800da78, 14, 12, 0x6); //[APHY fix] 		G-Lane DFE_SUMAMP_ ISEL
	_phy_rtd_part_outl(0x1800da78, 15, 15, 0x0); //[APHY fix] 		G-Lane DFE_SUMAMP_DCGAIN_MAX
	_phy_rtd_part_outl(0x1800da78, 19, 18, 0x0); //[APHY fix] 		G-Lane Dummy
	_phy_rtd_part_outl(0x1800da78, 20, 20, 0x0); //[APHY fix] 		G-Lane DFE CKI_DELAY_EN
	_phy_rtd_part_outl(0x1800da78, 21, 21, 0x0); //[APHY fix] 		G-Lane DFE CKIB_DELAY_EN
	_phy_rtd_part_outl(0x1800da78, 22, 22, 0x0); //[APHY fix] 		G-Lane DFE CKQ_DELAY_EN
	_phy_rtd_part_outl(0x1800da78, 23, 23, 0x0); //[APHY fix] 		G-Lane DFE CKQB_DELAY_EN
	_phy_rtd_part_outl(0x1800da78, 24, 24, 0x0); //[APHY fix] 		G-Lane EN_EYE_MNT
	_phy_rtd_part_outl(0x1800da78, 25, 25, 0x0); //[APHY fix] 		G-Lane DEMUX input clock phase select:(for eye mnt)
	_phy_rtd_part_outl(0x1800da78, 26, 26, 0x1); //[APHY fix] 		G-Lane VTH+OFFSET select. 0:VTH, 1:VTH+OFFSET
	_phy_rtd_part_outl(0x1800da78, 27, 27, 0x0); //[APHY fix] 		G-Lane DA_EG_VOS_PULLLOW(koffset disable)
	_phy_rtd_part_outl(0x1800da78, 31, 29, 0x0); //[APHY fix] 			G-Lane dummy
	_phy_rtd_part_outl(0x1800da7c, 2, 0, 0x0); //[APHY fix]  	G-Lane DFE_TAP_DELAY
	_phy_rtd_part_outl(0x1800da7c, 7, 7, 0x1); //[APHY fix]   		G-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da7c, 8, 8, 0x0); //[APHY fix]   	G-Lane rstb of dtata-lane test div8
	_phy_rtd_part_outl(0x1800da7c, 16, 16, 0x1); //[APHY fix]  		G-Lane TRANSITION CNT enable.
	_phy_rtd_part_outl(0x1800da7c, 17, 17, 0x0); //[APHY fix]  		G-Lane QCG1 injection-locked EN
	_phy_rtd_part_outl(0x1800da7c, 18, 18, 0x1); //[APHY fix]  		G-Lane QCG2_EN
	_phy_rtd_part_outl(0x1800da7c, 19, 19, 0x0); //[APHY fix]  		G-Lane QCG2 injection-locked EN
	_phy_rtd_part_outl(0x1800da7c, 20, 20, 0x1); //[APHY fix]  		G-Lane QCG FLD divider(/8) rstb
	_phy_rtd_part_outl(0x1800da7c, 21, 21, 0x1); //[APHY fix]  		G-Lane QCG FLD SEL
	_phy_rtd_part_outl(0x1800da7c, 22, 22, 0x0); //[APHY fix]  		G-Lane CK_FLD SEL.
	_phy_rtd_part_outl(0x1800da7c, 23, 23, 0x0); //[APHY fix]  		G-Lane CK-lane clk to B/G/R EN.
	_phy_rtd_part_outl(0x1800da80, 8, 8, 0x0); //[APHY fix]  G-Lane PI_EN
	_phy_rtd_part_outl(0x1800da80, 12, 12, 0x0); //[APHY fix]  G-Lane QCG1 Cload contrl SEL
	_phy_rtd_part_outl(0x1800da80, 13, 13, 0x0); //[APHY fix]  G-Lane QCG2 Cload contrl SEL
	_phy_rtd_part_outl(0x1800da80, 15, 15, 0x0); //[APHY fix]  G-Lane BIAS_PI_CUR_SEL
	_phy_rtd_part_outl(0x1800da80, 21, 21, 0x0); //[APHY fix]  G-Lane PI_EYE_EN
	_phy_rtd_part_outl(0x1800da80, 23, 23, 0x0); //[APHY fix]  G-Lane KI SEL
	_phy_rtd_part_outl(0x1800da80, 25, 25, 0x0); //[APHY fix]  G-Lane FKP_RSTB_SEL
	_phy_rtd_part_outl(0x1800da80, 27, 26, 0x0); //[APHY fix]  G-Lane KI_CK_SEL
	_phy_rtd_part_outl(0x1800da80, 29, 29, 0x0); //[APHY fix]  G-Lane ST_CODE SEL
	_phy_rtd_part_outl(0x1800da80, 30, 30, 0x0); //[APHY fix]  G-Lane QCG Ckin SEL.
	_phy_rtd_part_outl(0x1800da84, 3, 2, 0x1); //[APHY fix]  G-Lane ACDR_SEL_DIV_FLD[1:0]
	_phy_rtd_part_outl(0x1800da84, 5, 4, 0x0); //[APHY fix]  G-Lane ACDR_SEL_DIV_REF[1:0]
	_phy_rtd_part_outl(0x1800da84, 7, 6, 0x0); //[APHY fix]  G-Lane ACDR_SEL_DIV_PLL[1:0]
	_phy_rtd_part_outl(0x1800da84, 9, 8, 0x0); //[APHY fix]  G-Lane ACDR_SEL_DIV_TRAINING[1:0]
	_phy_rtd_part_outl(0x1800da84, 12, 12, 0x0); //[APHY fix]  G-Lane dummy
	_phy_rtd_part_outl(0x1800da84, 13, 13, 0x0); //[APHY fix]  G-Lane ACDR_SEL_FLD_0CKFB_1CKREF
	_phy_rtd_part_outl(0x1800da84, 15, 14, 0x0); //[APHY fix]  G-Lane no use
	_phy_rtd_part_outl(0x1800da84, 18, 18, 0x1); //[APHY fix]  G-Lane ACDR_RSTB_DIV_PLL ... removed
	_phy_rtd_part_outl(0x1800da84, 24, 24, 0x0); //[APHY fix]  G-Lane ACDR_EN_UPDN_PULSE_FILTER
	_phy_rtd_part_outl(0x1800da84, 25, 25, 0x1); //[APHY fix]  G-Lane ACDR_RSTB_UPDN ... dummy
	_phy_rtd_part_outl(0x1800da84, 27, 26, 0x0); //[APHY fix]  G-Lane ACDR_SEL_UPDN_WIDTH[1:0]
	_phy_rtd_part_outl(0x1800da84, 28, 28, 0x1); //[APHY fix]  G-Lane ACDR_POW_LPF_IDAC
	_phy_rtd_part_outl(0x1800da84, 31, 29, 0x3); //[APHY fix]  G-Lane ACDR_SEL_LPF_IDAC[2:0]
	_phy_rtd_part_outl(0x1800da88, 0, 0, 0x1); //[APHY fix]  G-Lane ACDR_POW_CP
	_phy_rtd_part_outl(0x1800da88, 2, 2, 0x1); //[APHY fix]  G-Lane ACDR_POW_IDN_BBPD
	_phy_rtd_part_outl(0x1800da88, 3, 3, 0x0); //[APHY fix]  G-Lane no use
	_phy_rtd_part_outl(0x1800da88, 6, 4, 0x7); //[APHY fix]  G-Lane ACDR_SEL_TIE_IDN_BBPD
	_phy_rtd_part_outl(0x1800da88, 7, 7, 0x0); //[APHY fix]  G-Lane ACDR_POW_IBIAS_IDN_HV
	_phy_rtd_part_outl(0x1800da88, 8, 8, 0x1); //[APHY fix]  G-Lane ACDR_POW_VCO
	_phy_rtd_part_outl(0x1800da88, 9, 9, 0x1); //[APHY fix]  G-Lane ACDR_POW_VCO_VDAC
	_phy_rtd_part_outl(0x1800da88, 11, 10, 0x0); //[APHY fix]  G-Lane ACDR_SEL_V15_VDAC
	_phy_rtd_part_outl(0x1800da88, 14, 14, 0x0); //[APHY fix]  G-Lane no use
	_phy_rtd_part_outl(0x1800da88, 15, 15, 0x0); //[APHY fix]  G-Lane no use
	_phy_rtd_part_outl(0x1800da88, 16, 16, 0x0); //[APHY fix]  G-Lane ACDR_POW_TEST_MODE
	_phy_rtd_part_outl(0x1800da88, 18, 17, 0x0); //[APHY fix]  G-Lane ACDR_SEL_TEST_MODE
	_phy_rtd_part_outl(0x1800da88, 31, 31, 0x0); //[APHY fix]  G-Lane no use
	_phy_rtd_part_outl(0x1800da8c, 13, 13, 0x1); //[APHY fix]  G-Lane ckref sel. 0:xtal, 1:tx or train clk
	_phy_rtd_part_outl(0x1800da8c, 14, 14, 0x0); //[APHY fix]  G-Lane ckref sel. 0:train_clk, 1:tx_clk
	_phy_rtd_part_outl(0x1800da8c, 15, 15, 0x1); //[APHY fix]  G-Lane ckbk sel. 0:M-DIV, 1:PRESCALER
	_phy_rtd_part_outl(0x1800da8c, 19, 16, 0x0); //[APHY fix]  G-Lane LE1_ISEL_IN_2 -> dummy
	_phy_rtd_part_outl(0x1800da8c, 31, 25, 0x41); //[APHY fix]  G-Lane no use for LEQ
	_phy_rtd_part_outl(0x1800da90, 0, 0, 0x0); //[APHY fix]  R-Lane input off EN
	_phy_rtd_part_outl(0x1800da90, 1, 1, 0x0); //[APHY fix]  		R-Lane INPOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da90, 2, 2, 0x0); //[APHY fix]  		R-Lane INNOFF_SINGLE_EN
	_phy_rtd_part_outl(0x1800da90, 3, 3, 0x1); //[APHY fix]  		R-Lane POW_AC_COUPLE
	_phy_rtd_part_outl(0x1800da90, 5, 4, 0x1); //[APHY fix]  		R-Lane RXVCM_SEL[1:0]
	_phy_rtd_part_outl(0x1800da94, 21, 21, 0x1); //[APHY fix]   		R-Lane RS_CAL_EN
	_phy_rtd_part_outl(0x1800da94, 25, 25, 0x1); //[APHY fix]   		R-Lane POW_DATALANE_BIAS
	_phy_rtd_part_outl(0x1800da94, 26, 26, 0x0); //[APHY fix]   		R-Lane REG_FORCE_STARTUP(Const-GM)
	_phy_rtd_part_outl(0x1800da94, 27, 27, 0x0); //[APHY fix]   		R-Lane REG_POWB_STARTUP
	_phy_rtd_part_outl(0x1800da98, 8, 8, 0x1); //[APHY fix]  		R-Lane POW_DFE
	_phy_rtd_part_outl(0x1800da98, 14, 12, 0x6); //[APHY fix] 		R-Lane DFE_SUMAMP_ ISEL
	_phy_rtd_part_outl(0x1800da98, 15, 15, 0x0); //[APHY fix] 		R-Lane DFE_SUMAMP_DCGAIN_MAX
	_phy_rtd_part_outl(0x1800da98, 19, 18, 0x0); //[APHY fix] 		R-Lane Dummy
	_phy_rtd_part_outl(0x1800da98, 20, 20, 0x0); //[APHY fix] 		R-Lane DFE CKI_DELAY_EN
	_phy_rtd_part_outl(0x1800da98, 21, 21, 0x0); //[APHY fix] 		R-Lane DFE CKIB_DELAY_EN
	_phy_rtd_part_outl(0x1800da98, 22, 22, 0x0); //[APHY fix] 		R-Lane DFE CKQ_DELAY_EN
	_phy_rtd_part_outl(0x1800da98, 23, 23, 0x0); //[APHY fix] 		R-Lane DFE CKQB_DELAY_EN
	_phy_rtd_part_outl(0x1800da98, 24, 24, 0x0); //[APHY fix] 		R-Lane EN_EYE_MNT
	_phy_rtd_part_outl(0x1800da98, 25, 25, 0x0); //[APHY fix] 		R-Lane DEMUX input clock phase select:(for eye mnt)
	_phy_rtd_part_outl(0x1800da98, 26, 26, 0x1); //[APHY fix] 		R-Lane VTH+OFFSET select. 0:VTH, 1:VTH+OFFSET
	_phy_rtd_part_outl(0x1800da98, 27, 27, 0x0); //[APHY fix] 		R-Lane DA_EG_VOS_PULLLOW(koffset disable)
	_phy_rtd_part_outl(0x1800da98, 31, 29, 0x0); //[APHY fix] 			R-Lane dummy
	_phy_rtd_part_outl(0x1800da9c, 2, 0, 0x0); //[APHY fix]  	R-Lane DFE_TAP_DELAY
	_phy_rtd_part_outl(0x1800da9c, 7, 7, 0x1); //[APHY fix]   		R-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da9c, 8, 8, 0x0); //[APHY fix]   	R-Lane rstb of dtata-lane test div8
	_phy_rtd_part_outl(0x1800da9c, 16, 16, 0x1); //[APHY fix]  		R-Lane TRANSITION CNT enable.
	_phy_rtd_part_outl(0x1800da9c, 17, 17, 0x0); //[APHY fix]  		R-Lane QCG1 injection-locked EN
	_phy_rtd_part_outl(0x1800da9c, 18, 18, 0x1); //[APHY fix]  		R-Lane QCG2_EN
	_phy_rtd_part_outl(0x1800da9c, 19, 19, 0x0); //[APHY fix]  		R-Lane QCG2 injection-locked EN
	_phy_rtd_part_outl(0x1800da9c, 20, 20, 0x1); //[APHY fix]  		R-Lane QCG FLD divider(/8) rstb
	_phy_rtd_part_outl(0x1800da9c, 21, 21, 0x1); //[APHY fix]  		R-Lane QCG FLD SEL
	_phy_rtd_part_outl(0x1800da9c, 22, 22, 0x0); //[APHY fix]  		R-Lane CK_FLD SEL.
	_phy_rtd_part_outl(0x1800da9c, 23, 23, 0x0); //[APHY fix]  		R-Lane CK-lane clk to B/G/R EN.
	_phy_rtd_part_outl(0x1800daa0, 8, 8, 0x0); //[APHY fix]  R-Lane PI_EN
	_phy_rtd_part_outl(0x1800daa0, 12, 12, 0x0); //[APHY fix]  R-Lane QCG1 Cload contrl SEL
	_phy_rtd_part_outl(0x1800daa0, 13, 13, 0x0); //[APHY fix]  R-Lane QCG2 Cload contrl SEL
	_phy_rtd_part_outl(0x1800daa0, 15, 15, 0x0); //[APHY fix]  R-Lane BIAS_PI_CUR_SEL
	_phy_rtd_part_outl(0x1800daa0, 21, 21, 0x0); //[APHY fix]  R-Lane PI_EYE_EN
	_phy_rtd_part_outl(0x1800daa0, 23, 23, 0x0); //[APHY fix]  R-Lane KI SEL
	_phy_rtd_part_outl(0x1800daa0, 25, 25, 0x0); //[APHY fix]  R-Lane FKP_RSTB_SEL
	_phy_rtd_part_outl(0x1800daa0, 27, 26, 0x0); //[APHY fix]  R-Lane KI_CK_SEL
	_phy_rtd_part_outl(0x1800daa0, 29, 29, 0x0); //[APHY fix]  R-Lane ST_CODE SEL
	_phy_rtd_part_outl(0x1800daa0, 30, 30, 0x0); //[APHY fix]  R-Lane QCG Ckin SEL.
	_phy_rtd_part_outl(0x1800daa4, 3, 2, 0x1); //[APHY fix]  R-Lane ACDR_SEL_DIV_FLD[1:0]
	_phy_rtd_part_outl(0x1800daa4, 5, 4, 0x0); //[APHY fix]  R-Lane ACDR_SEL_DIV_REF[1:0]
	_phy_rtd_part_outl(0x1800daa4, 7, 6, 0x0); //[APHY fix]  R-Lane ACDR_SEL_DIV_PLL[1:0]
	_phy_rtd_part_outl(0x1800daa4, 9, 8, 0x0); //[APHY fix]  R-Lane ACDR_SEL_DIV_TRAINING[1:0]
	_phy_rtd_part_outl(0x1800daa4, 12, 12, 0x0); //[APHY fix]  R-Lane dummy
	_phy_rtd_part_outl(0x1800daa4, 13, 13, 0x0); //[APHY fix]  R-Lane ACDR_SEL_FLD_0CKFB_1CKREF
	_phy_rtd_part_outl(0x1800daa4, 15, 14, 0x0); //[APHY fix]  R-Lane no use
	_phy_rtd_part_outl(0x1800daa4, 18, 18, 0x1); //[APHY fix]  R-Lane ACDR_RSTB_DIV_PLL ... removed
	_phy_rtd_part_outl(0x1800daa4, 24, 24, 0x0); //[APHY fix]  R-Lane ACDR_EN_UPDN_PULSE_FILTER
	_phy_rtd_part_outl(0x1800daa4, 25, 25, 0x1); //[APHY fix]  R-Lane ACDR_RSTB_UPDN ... dummy
	_phy_rtd_part_outl(0x1800daa4, 27, 26, 0x0); //[APHY fix]  R-Lane ACDR_SEL_UPDN_WIDTH[1:0]
	_phy_rtd_part_outl(0x1800daa4, 28, 28, 0x1); //[APHY fix]  R-Lane ACDR_POW_LPF_IDAC
	_phy_rtd_part_outl(0x1800daa4, 31, 29, 0x3); //[APHY fix]  R-Lane ACDR_SEL_LPF_IDAC[2:0]
	_phy_rtd_part_outl(0x1800daa8, 0, 0, 0x1); //[APHY fix]  R-Lane ACDR_POW_CP
	_phy_rtd_part_outl(0x1800daa8, 2, 2, 0x1); //[APHY fix]  R-Lane ACDR_POW_IDN_BBPD
	_phy_rtd_part_outl(0x1800daa8, 3, 3, 0x0); //[APHY fix]  R-Lane no use
	_phy_rtd_part_outl(0x1800daa8, 6, 4, 0x7); //[APHY fix]  R-Lane ACDR_SEL_TIE_IDN_BBPD
	_phy_rtd_part_outl(0x1800daa8, 7, 7, 0x0); //[APHY fix]  R-Lane ACDR_POW_IBIAS_IDN_HV
	_phy_rtd_part_outl(0x1800daa8, 8, 8, 0x1); //[APHY fix]  R-Lane ACDR_POW_VCO
	_phy_rtd_part_outl(0x1800daa8, 9, 9, 0x1); //[APHY fix]  R-Lane ACDR_POW_VCO_VDAC
	_phy_rtd_part_outl(0x1800daa8, 11, 10, 0x0); //[APHY fix]  R-Lane ACDR_SEL_V15_VDAC
	_phy_rtd_part_outl(0x1800daa8, 14, 14, 0x0); //[APHY fix]  R-Lane no use
	_phy_rtd_part_outl(0x1800daa8, 15, 15, 0x0); //[APHY fix]  R-Lane no use
	_phy_rtd_part_outl(0x1800daa8, 16, 16, 0x0); //[APHY fix]  R-Lane ACDR_POW_TEST_MODE
	_phy_rtd_part_outl(0x1800daa8, 18, 17, 0x0); //[APHY fix]  R-Lane ACDR_SEL_TEST_MODE
	_phy_rtd_part_outl(0x1800daa8, 31, 31, 0x0); //[APHY fix]  R-Lane no use
	_phy_rtd_part_outl(0x1800daac, 13, 13, 0x1); //[APHY fix]  R-Lane ckref sel. 0:xtal, 1:tx or train clk
	_phy_rtd_part_outl(0x1800daac, 14, 14, 0x0); //[APHY fix]  R-Lane ckref sel. 0:train_clk, 1:tx_clk
	_phy_rtd_part_outl(0x1800daac, 15, 15, 0x1); //[APHY fix]  R-Lane ckbk sel. 0:M-DIV, 1:PRESCALER
	_phy_rtd_part_outl(0x1800daac, 19, 16, 0x0); //[APHY fix]  R-Lane LE1_ISEL_IN_2 -> dummy
	_phy_rtd_part_outl(0x1800daac, 31, 25, 0x41); //[APHY fix]  R-Lane no use for LEQ
	_phy_rtd_part_outl(0x1800dab4, 0, 0, 0x1); //[APHY fix]  Non-Hysteresis Amp Power
	_phy_rtd_part_outl(0x1800dab4, 3, 1, 0x3); //[APHY fix]  CK Mode hysteresis amp Hysteresis adjustment[2:0]
	_phy_rtd_part_outl(0x1800dab4, 5, 5, 0x1); //[APHY fix]  CK Mode detect source Select hysteresis path
	_phy_rtd_part_outl(0x1800dab4, 6, 6, 0x1); //[APHY fix]  Hysteresis amp power
	_phy_rtd_part_outl(0x1800dab4, 8, 8, 0x1); //[APHY fix]  CMU_WDPOW
	_phy_rtd_part_outl(0x1800dab4, 9, 9, 0x0); //[APHY fix]  CMU_WDSET
	_phy_rtd_part_outl(0x1800dab4, 10, 10, 0x0); //[APHY fix]  CMU_WDRST
	_phy_rtd_part_outl(0x1800dab4, 11, 11, 0x1); //[APHY fix]  no use
	_phy_rtd_part_outl(0x1800dab4, 12, 12, 0x0); //[APHY fix]  EN_TST_CKBK
	_phy_rtd_part_outl(0x1800dab4, 13, 13, 0x0); //[APHY fix]  EN_TST_CKREF
	_phy_rtd_part_outl(0x1800dab4, 15, 14, 0x0); //[APHY fix]  no use
	_phy_rtd_part_outl(0x1800dab4, 23, 16, 0x6b); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dab4, 31, 24, 0x04); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dab8, 7, 0, 0x00); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dab8, 9, 9, 0x0); //[APHY fix]  no use
	_phy_rtd_part_outl(0x1800dab8, 10, 10, 0x0); //[APHY fix]  CMU_SEL_CS
	_phy_rtd_part_outl(0x1800dab8, 15, 11, 0x00); //[APHY fix]  no use
	_phy_rtd_part_outl(0x1800dab8, 23, 16, 0x00); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dab8, 31, 24, 0x00); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dabc, 7, 0, 0x00); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dabc, 15, 8, 0x00); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dabc, 23, 16, 0x00); //[APHY fix]  dummy
	_phy_rtd_part_outl(0x1800dabc, 31, 24, 0x00); //[APHY fix]  dummy
}

void APHY_Para_frl_1_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	// APHY para start
	_phy_rtd_part_outl(0x1800da30, 9, 8, APHY_Para_frl_1[frl_timing].CK_2_2_1); //[APHY para]  CK-Lane RLSEL_LE1
	_phy_rtd_part_outl(0x1800da30, 11, 10, APHY_Para_frl_1[frl_timing].CK_2_3_1); //[APHY para]  CK-Lane RLSEL_LE2
	_phy_rtd_part_outl(0x1800da30, 13, 12, APHY_Para_frl_1[frl_timing].CK_2_4_1); //[APHY para]  CK-Lane RLSEL_NC
	_phy_rtd_part_outl(0x1800da30, 15, 14, APHY_Para_frl_1[frl_timing].CK_2_5_1); //[APHY para]  CK-Lane RLSEL_TAP0
	_phy_rtd_part_outl(0x1800da30, 19, 16, APHY_Para_frl_1[frl_timing].CK_3_6_1); //[APHY para]  CK-Lane RSSEL_LE1_1
	_phy_rtd_part_outl(0x1800da30, 23, 20, APHY_Para_frl_1[frl_timing].CK_3_7_1); //[APHY para]  CK-Lane RSSEL_LE1_2
	_phy_rtd_part_outl(0x1800da30, 27, 24, APHY_Para_frl_1[frl_timing].CK_4_8_1); //[APHY para]  CK-Lane RSSEL_LE2
	_phy_rtd_part_outl(0x1800da30, 29, 28, APHY_Para_frl_1[frl_timing].CK_4_9_1); //[APHY para]  CK-Lane RSSEL_TAP0
	_phy_rtd_part_outl(0x1800da30, 31, 30, APHY_Para_frl_1[frl_timing].CK_4_10_1); //[APHY para]  	CK-Lane KOFF_RANGE
	_phy_rtd_part_outl(0x1800da34, 3, 0, APHY_Para_frl_1[frl_timing].CK_5_11_1); //[APHY para]    CK-Lane LE1_ISEL_IN
	_phy_rtd_part_outl(0x1800da34, 7, 4, APHY_Para_frl_1[frl_timing].CK_5_12_1); //[APHY para]    CK-Lane LE2_ISEL_IN
	_phy_rtd_part_outl(0x1800da34, 11, 8, APHY_Para_frl_1[frl_timing].CK_6_13_1); //[APHY para]   	CK-Lane LE_NC_ISEL_IN
	_phy_rtd_part_outl(0x1800da34, 15, 12, 0xf); //[APHY para]  	CK-Lane TAP0_ISEL_IN
	_phy_rtd_part_outl(0x1800da34, 17, 16, 0x0); //[APHY para]   	CK-Lane LE_IHALF[1:0]
	_phy_rtd_part_outl(0x1800da34, 18, 18, APHY_Para_frl_1[frl_timing].CK_7_16_1); //[APHY para]   	CK-Lane NC_IHALF
	_phy_rtd_part_outl(0x1800da34, 19, 19, APHY_Para_frl_1[frl_timing].CK_7_17_1); //[APHY para]   	CK-Lane TAP0_IHALF
	_phy_rtd_part_outl(0x1800da34, 20, 20, 0x0); //[APHY para]   	CK-Lane EN_LEQ6G
	_phy_rtd_part_outl(0x1800da34, 22, 22, APHY_Para_frl_1[frl_timing].CK_7_19_1); //[APHY para]   	CK-Lane POW_NC
	_phy_rtd_part_outl(0x1800da34, 30, 28, 0x3); //[APHY para]   CK-Lane DATALANE_BIAS_ISEL[2:0]
	_phy_rtd_part_outl(0x1800da34, 31, 31, 0x1); //[APHY para]   CK-Lane POW_LEQ_RL
	_phy_rtd_part_outl(0x1800da38, 1, 1, 0x0); //[APHY para]  		CK-Lane SEL_CMFB_LS
	_phy_rtd_part_outl(0x1800da38, 3, 2, 0x3); //[APHY para]  		CK-Lane LEQ_CUR_ADJ
	_phy_rtd_part_outl(0x1800da38, 5, 4, 0x0); //[APHY para]  		CK-Lane PTAT_CUR_ADJ
	_phy_rtd_part_outl(0x1800da38, 6, 6, 0x0); //[APHY para]  		CK-Lane REG_BIAS_POW_CON_GM
	_phy_rtd_part_outl(0x1800da38, 11, 9, 0x0); //[APHY para] 		CK-Lane PTAT_CUR_ADJ_FINE
	_phy_rtd_part_outl(0x1800da38, 16, 16, APHY_Para_frl_1[frl_timing].CK_11_27_1); //[APHY para] 		CK-Lane FR_CK_SEL
	_phy_rtd_part_outl(0x1800da3c, 11, 9, APHY_Para_frl_1[frl_timing].CK_14_28_1); //[APHY para]   	CK-Lane DEMUX_PIX_RATE_SEL<2:0>
	_phy_rtd_part_outl(0x1800da3c, 12, 12, APHY_Para_frl_1[frl_timing].CK_14_29_1); //[APHY para]   	CK-Lane DEMUX_FR_CK_SEL
	_phy_rtd_part_outl(0x1800da3c, 15, 13, 0x1); //[APHY para]   	CK-Lane DEMUX_RATE_SEL
	_phy_rtd_part_outl(0x1800da3c, 24, 24, 0x0); //[APHY para]  		CK-Lane QCG1_SEL_BIAS_ST
	_phy_rtd_part_outl(0x1800da3c, 25, 25, 0x0); //[APHY para]  		CK-Lane QCG1_SEL_BIAS_RG
	_phy_rtd_part_outl(0x1800da3c, 27, 26, 0x0); //[APHY para]  		CK-Lane QCG1 Rload SEL
	_phy_rtd_part_outl(0x1800da3c, 31, 28, 0xd); //[APHY para]  		CK-Lane QCG1 Cload SEL
	_phy_rtd_part_outl(0x1800da40, 0, 0, 0x0); //[APHY para]  CK-Lane QCG2_SEL_BIAS_ST
	_phy_rtd_part_outl(0x1800da40, 1, 1, 0x0); //[APHY para]  CK-Lane QCG2_SEL_BIAS_RG
	_phy_rtd_part_outl(0x1800da40, 3, 2, 0x0); //[APHY para]  CK-Lane QCG2 Rload SEL
	_phy_rtd_part_outl(0x1800da40, 7, 4, 0xd); //[APHY para]  CK-Lane QCG2 Cload SEL
	_phy_rtd_part_outl(0x1800da40, 11, 9, 0x0); //[APHY para]  CK-Lane PI_ISEL<2:0>
	_phy_rtd_part_outl(0x1800da40, 14, 14, 0x0); //[APHY para]  CK-Lane PI_CSEL
	_phy_rtd_part_outl(0x1800da40, 20, 18, 0x4); //[APHY para]  CK-Lane PI_DIV_SEL
	_phy_rtd_part_outl(0x1800da40, 22, 22, 0x1); //[APHY para]  CK-Lane DCDR ckout SEL
	_phy_rtd_part_outl(0x1800da40, 31, 31, 0x0); //[APHY para]  CK-Lane DFE Ckin SEL.
	_phy_rtd_part_outl(0x1800da44, 11, 10, APHY_Para_frl_1[frl_timing].ACDR_CK_2_44_1); //[APHY para]  CK-Lane ACDR_SEL_IDNBIAS_LV[1:0]
	_phy_rtd_part_outl(0x1800da44, 17, 17, 0x1); //[APHY para]  CK-Lane ACDR_RSTB_DIV_BAND_2OR4_LV
	_phy_rtd_part_outl(0x1800da44, 19, 19, APHY_Para_frl_1[frl_timing].ACDR_CK_3_46_1); //[APHY para]  CK-Lane  ACDR_SEL_HS_CLK
	_phy_rtd_part_outl(0x1800da44, 20, 20, 0x1); //[APHY para]  CK-Lane ACDR_SEL_0FR_1HR_DIV_IQ
	_phy_rtd_part_outl(0x1800da44, 23, 21, 0x0); //[APHY para]  CK-Lane ACDR_SEL_DIV_BAND_2OR4_LV
	_phy_rtd_part_outl(0x1800da48, 1, 1, 0x0); //[APHY para]  CK-Lane ACDR_POW_CP_INTG2_CORE
	_phy_rtd_part_outl(0x1800da48, 13, 12, APHY_Para_frl_1[frl_timing].ACDR_CK_6_50_1); //[APHY para]  CK-Lane ACDR_SEL_BAND_CAP
	_phy_rtd_part_outl(0x1800da48, 26, 25, APHY_Para_frl_1[frl_timing].ACDR_CK_8_51_1); //[APHY para]  CK-Lane BBPD_KP_SEL[1:0]
	_phy_rtd_part_outl(0x1800da48, 28, 27, 0x1); //[APHY para]  CK-Lane BBPD_KI_SEL[1:0]
	_phy_rtd_part_outl(0x1800da48, 29, 29, 0x0); //[APHY para]  CK-Lane BBPD_BYPASS_CTN_KP
	_phy_rtd_part_outl(0x1800da48, 30, 30, 0x0); //[APHY para]  CK-Lane BBPD_BYPASS_CTN_KI
	_phy_rtd_part_outl(0x1800da4c, 7, 0, APHY_Para_frl_1[frl_timing].ACDR_CK_9_55_1); //[APHY para]  CK-Lane CMU_SEL_M_DIV[7:0]
	_phy_rtd_part_outl(0x1800da4c, 8, 8, 0x0); //[APHY para]  CK-Lane CMU_SEL_M_DIV[8]
	_phy_rtd_part_outl(0x1800da4c, 9, 9, 0x0); //[APHY para]  CK-Lane CMU_SEL_PRE_M_DIV
	_phy_rtd_part_outl(0x1800da4c, 10, 10, APHY_Para_frl_1[frl_timing].ACDR_CK_10_58_1); //[APHY para]  CK-Lane CMU_SEL_POST_M_DIV
	_phy_rtd_part_outl(0x1800da4c, 21, 20, APHY_Para_frl_1[frl_timing].ACDR_CK_11_59_1); //[APHY para]  CK-Lane RLSEL_LE1_2
	_phy_rtd_part_outl(0x1800da4c, 23, 22, APHY_Para_frl_1[frl_timing].ACDR_CK_11_60_1); //[APHY para]  CK-Lane RLSEL_NC_2
	_phy_rtd_part_outl(0x1800da4c, 24, 24, APHY_Para_frl_1[frl_timing].ACDR_CK_12_61_1); //[APHY para]  CK-Lane POW_NC
	_phy_rtd_part_outl(0x1800da50, 9, 8, APHY_Para_frl_1[frl_timing].B_2_62_1); //[APHY para]  		B-Lane RLSEL_LE1
	_phy_rtd_part_outl(0x1800da50, 11, 10, APHY_Para_frl_1[frl_timing].B_2_63_1); //[APHY para]  		B-Lane RLSEL_LE2
	_phy_rtd_part_outl(0x1800da50, 13, 12, APHY_Para_frl_1[frl_timing].B_2_64_1); //[APHY para]  		B-Lane RLSEL_NC
	_phy_rtd_part_outl(0x1800da50, 15, 14, APHY_Para_frl_1[frl_timing].B_2_65_1); //[APHY para]  		B-Lane RLSEL_TAP0
	_phy_rtd_part_outl(0x1800da50, 19, 16, APHY_Para_frl_1[frl_timing].B_3_66_1); //[APHY para]  		B-Lane RSSEL_LE1_1
	_phy_rtd_part_outl(0x1800da50, 23, 20, APHY_Para_frl_1[frl_timing].B_3_67_1); //[APHY para]  		B-Lane RSSEL_LE1_2
	_phy_rtd_part_outl(0x1800da50, 27, 24, APHY_Para_frl_1[frl_timing].B_4_68_1); //[APHY para]  		B-Lane RSSEL_LE2
	_phy_rtd_part_outl(0x1800da50, 29, 28, APHY_Para_frl_1[frl_timing].B_4_69_1); //[APHY para]  		B-Lane RSSEL_TAP0
	_phy_rtd_part_outl(0x1800da50, 31, 30, APHY_Para_frl_1[frl_timing].B_4_70_1); //[APHY para]  	B-Lane KOFF_RANGE
	_phy_rtd_part_outl(0x1800da54, 3, 0, APHY_Para_frl_1[frl_timing].B_5_71_1); //[APHY para]    	B-Lane LE1_ISEL_IN
	_phy_rtd_part_outl(0x1800da54, 7, 4, APHY_Para_frl_1[frl_timing].B_5_72_1); //[APHY para]    	B-Lane LE2_ISEL_IN
	_phy_rtd_part_outl(0x1800da54, 11, 8, APHY_Para_frl_1[frl_timing].B_6_73_1); //[APHY para]   		B-Lane LE_NC_ISEL_IN
	_phy_rtd_part_outl(0x1800da54, 15, 12, 0xf); //[APHY para]  		B-Lane TAP0_ISEL_IN
	_phy_rtd_part_outl(0x1800da54, 17, 16, 0x0); //[APHY para]   		B-Lane LE_IHALF[1:0]
	_phy_rtd_part_outl(0x1800da54, 18, 18, APHY_Para_frl_1[frl_timing].B_7_76_1); //[APHY para]   		B-Lane NC_IHALF
	_phy_rtd_part_outl(0x1800da54, 19, 19, APHY_Para_frl_1[frl_timing].B_7_77_1); //[APHY para]   		B-Lane TAP0_IHALF
	_phy_rtd_part_outl(0x1800da54, 20, 20, 0x0); //[APHY para]   		B-Lane EN_LEQ6G
	_phy_rtd_part_outl(0x1800da54, 22, 22, APHY_Para_frl_1[frl_timing].B_7_79_1); //[APHY para]   	B-Lane POW_NC
	_phy_rtd_part_outl(0x1800da54, 30, 28, 0x3); //[APHY para]   		B-Lane DATALANE_BIAS_ISEL[2:0]
	_phy_rtd_part_outl(0x1800da54, 31, 31, 0x1); //[APHY para]   		B-Lane POW_LEQ_RL
	_phy_rtd_part_outl(0x1800da58, 1, 1, 0x0); //[APHY para]  		B-Lane SEL_CMFB_LS
	_phy_rtd_part_outl(0x1800da58, 3, 2, 0x3); //[APHY para]  		B-Lane LEQ_CUR_ADJ
	_phy_rtd_part_outl(0x1800da58, 5, 4, 0x0); //[APHY para]  		B-Lane PTAT_CUR_ADJ
	_phy_rtd_part_outl(0x1800da58, 6, 6, 0x0); //[APHY para]  		B-Lane REG_BIAS_POW_CON_GM
	_phy_rtd_part_outl(0x1800da58, 11, 9, 0x0); //[APHY para] 		B-Lane PTAT_CUR_ADJ_FINE
	_phy_rtd_part_outl(0x1800da58, 16, 16, APHY_Para_frl_1[frl_timing].B_11_87_1); //[APHY para] 		B-Lane FR_CK_SEL
	_phy_rtd_part_outl(0x1800da5c, 11, 9, APHY_Para_frl_1[frl_timing].B_14_88_1); //[APHY para]   	B-Lane DEMUX_PIX_RATE_SEL<2:0>
	_phy_rtd_part_outl(0x1800da5c, 12, 12, APHY_Para_frl_1[frl_timing].B_14_89_1); //[APHY para]   	B-Lane DEMUX_FR_CK_SEL
	_phy_rtd_part_outl(0x1800da5c, 15, 13, 0x1); //[APHY para]   	B-Lane DEMUX_RATE_SEL
	_phy_rtd_part_outl(0x1800da5c, 24, 24, 0x0); //[APHY para]  		B-Lane QCG1_SEL_BIAS_ST
	_phy_rtd_part_outl(0x1800da5c, 25, 25, 0x0); //[APHY para]  		B-Lane QCG1_SEL_BIAS_RG
	_phy_rtd_part_outl(0x1800da5c, 27, 26, 0x0); //[APHY para]  		B-Lane QCG1 Rload SEL
	_phy_rtd_part_outl(0x1800da5c, 31, 28, 0xd); //[APHY para]  		B-Lane QCG1 Cload SEL
	_phy_rtd_part_outl(0x1800da60, 0, 0, 0x0); //[APHY para]  B-Lane QCG2_SEL_BIAS_ST
	_phy_rtd_part_outl(0x1800da60, 1, 1, 0x0); //[APHY para]  B-Lane QCG2_SEL_BIAS_RG
	_phy_rtd_part_outl(0x1800da60, 3, 2, 0x0); //[APHY para]  B-Lane QCG2 Rload SEL
	_phy_rtd_part_outl(0x1800da60, 7, 4, 0xd); //[APHY para]  B-Lane QCG2 Cload SEL
	_phy_rtd_part_outl(0x1800da60, 11, 9, 0x0); //[APHY para]  B-Lane PI_ISEL<2:0>
	_phy_rtd_part_outl(0x1800da60, 14, 14, 0x0); //[APHY para]  B-Lane PI_CSEL
	_phy_rtd_part_outl(0x1800da60, 20, 18, 0x4); //[APHY para]  B-Lane PI_DIV_SEL
	_phy_rtd_part_outl(0x1800da60, 22, 22, 0x1); //[APHY para]  B-Lane DCDR ckout SEL
	_phy_rtd_part_outl(0x1800da60, 31, 31, 0x0); //[APHY para]  B-Lane DFE Ckin SEL.
	_phy_rtd_part_outl(0x1800da64, 11, 10, APHY_Para_frl_1[frl_timing].ACDR_B_2_104_1); //[APHY para]  B-Lane ACDR_SEL_IDNBIAS_LV[1:0]
	_phy_rtd_part_outl(0x1800da64, 17, 17, 0x1); //[APHY para]  B-Lane ACDR_RSTB_DIV_BAND_2OR4_LV
	_phy_rtd_part_outl(0x1800da64, 19, 19, APHY_Para_frl_1[frl_timing].ACDR_B_3_106_1); //[APHY para]  B-Lane  ACDR_SEL_HS_CLK
	_phy_rtd_part_outl(0x1800da64, 20, 20, 0x1); //[APHY para]  B-Lane ACDR_SEL_0FR_1HR_DIV_IQ
	_phy_rtd_part_outl(0x1800da64, 23, 21, 0x0); //[APHY para]  B-Lane ACDR_SEL_DIV_BAND_2OR4_LV
	_phy_rtd_part_outl(0x1800da68, 1, 1, 0x0); //[APHY para]  B-Lane ACDR_POW_CP_INTG2_CORE
	_phy_rtd_part_outl(0x1800da68, 13, 12, APHY_Para_frl_1[frl_timing].ACDR_B_6_110_1); //[APHY para]  B-Lane ACDR_SEL_BAND_CAP
	_phy_rtd_part_outl(0x1800da68, 26, 25, APHY_Para_frl_1[frl_timing].ACDR_B_8_111_1); //[APHY para]  B-Lane BBPD_KP_SEL[1:0]
	_phy_rtd_part_outl(0x1800da68, 28, 27, 0x1); //[APHY para]  B-Lane BBPD_KI_SEL[1:0]
	_phy_rtd_part_outl(0x1800da68, 29, 29, 0x0); //[APHY para]  B-Lane BBPD_BYPASS_CTN_KP
	_phy_rtd_part_outl(0x1800da68, 30, 30, 0x0); //[APHY para]  B-Lane BBPD_BYPASS_CTN_KI
	_phy_rtd_part_outl(0x1800da6c, 7, 0, APHY_Para_frl_1[frl_timing].ACDR_B_9_115_1); //[APHY para]  B-Lane CMU_SEL_M_DIV[7:0]
	_phy_rtd_part_outl(0x1800da6c, 8, 8, 0x0); //[APHY para]  B-Lane CMU_SEL_M_DIV[8]
	_phy_rtd_part_outl(0x1800da6c, 9, 9, 0x0); //[APHY para]  B-Lane CMU_SEL_PRE_M_DIV
	_phy_rtd_part_outl(0x1800da6c, 10, 10, APHY_Para_frl_1[frl_timing].ACDR_B_10_118_1); //[APHY para]  B-Lane CMU_SEL_POST_M_DIV
	_phy_rtd_part_outl(0x1800da6c, 21, 20, APHY_Para_frl_1[frl_timing].ACDR_B_11_119_1); //[APHY para]  B-Lane RLSEL_LE1_2
	_phy_rtd_part_outl(0x1800da6c, 23, 22, APHY_Para_frl_1[frl_timing].ACDR_B_11_120_1); //[APHY para]  B-Lane RLSEL_NC_2
	_phy_rtd_part_outl(0x1800da6c, 24, 24, APHY_Para_frl_1[frl_timing].ACDR_B_12_121_1); //[APHY para]  B-Lane POW_NC
	_phy_rtd_part_outl(0x1800da70, 9, 8, APHY_Para_frl_1[frl_timing].G_2_122_1); //[APHY para]  		G-Lane RLSEL_LE1
	_phy_rtd_part_outl(0x1800da70, 11, 10, APHY_Para_frl_1[frl_timing].G_2_123_1); //[APHY para]  		G-Lane RLSEL_LE2
	_phy_rtd_part_outl(0x1800da70, 13, 12, APHY_Para_frl_1[frl_timing].G_2_124_1); //[APHY para]  		G-Lane RLSEL_NC
	_phy_rtd_part_outl(0x1800da70, 15, 14, APHY_Para_frl_1[frl_timing].G_2_125_1); //[APHY para]  		G-Lane RLSEL_TAP0
	_phy_rtd_part_outl(0x1800da70, 19, 16, APHY_Para_frl_1[frl_timing].G_3_126_1); //[APHY para]  		G-Lane RSSEL_LE1_1
	_phy_rtd_part_outl(0x1800da70, 23, 20, APHY_Para_frl_1[frl_timing].G_3_127_1); //[APHY para]  		G-Lane RSSEL_LE1_2
	_phy_rtd_part_outl(0x1800da70, 27, 24, APHY_Para_frl_1[frl_timing].G_4_128_1); //[APHY para]  		G-Lane RSSEL_LE2
	_phy_rtd_part_outl(0x1800da70, 29, 28, APHY_Para_frl_1[frl_timing].G_4_129_1); //[APHY para]  		G-Lane RSSEL_TAP0
	_phy_rtd_part_outl(0x1800da70, 31, 30, APHY_Para_frl_1[frl_timing].G_4_130_1); //[APHY para]  	G-Lane KOFF_RANGE
	_phy_rtd_part_outl(0x1800da74, 3, 0, APHY_Para_frl_1[frl_timing].G_5_131_1); //[APHY para]    	G-Lane LE1_ISEL_IN
	_phy_rtd_part_outl(0x1800da74, 7, 4, APHY_Para_frl_1[frl_timing].G_5_132_1); //[APHY para]    	G-Lane LE2_ISEL_IN
	_phy_rtd_part_outl(0x1800da74, 11, 8, APHY_Para_frl_1[frl_timing].G_6_133_1); //[APHY para]   		G-Lane LE_NC_ISEL_IN
	_phy_rtd_part_outl(0x1800da74, 15, 12, 0xf); //[APHY para]  		G-Lane TAP0_ISEL_IN
	_phy_rtd_part_outl(0x1800da74, 17, 16, 0x0); //[APHY para]   		G-Lane LE_IHALF[1:0]
	_phy_rtd_part_outl(0x1800da74, 18, 18, APHY_Para_frl_1[frl_timing].G_7_136_1); //[APHY para]   		G-Lane NC_IHALF
	_phy_rtd_part_outl(0x1800da74, 19, 19, APHY_Para_frl_1[frl_timing].G_7_137_1); //[APHY para]   		G-Lane TAP0_IHALF
	_phy_rtd_part_outl(0x1800da74, 20, 20, 0x0); //[APHY para]   		G-Lane EN_LEQ6G
	_phy_rtd_part_outl(0x1800da74, 22, 22, APHY_Para_frl_1[frl_timing].G_7_139_1); //[APHY para]   	G-Lane POW_NC
	_phy_rtd_part_outl(0x1800da74, 30, 28, 0x3); //[APHY para]   		G-Lane DATALANE_BIAS_ISEL[2:0]
	_phy_rtd_part_outl(0x1800da74, 31, 31, 0x1); //[APHY para]   		G-Lane POW_LEQ_RL
	_phy_rtd_part_outl(0x1800da78, 1, 1, 0x0); //[APHY para]  		G-Lane SEL_CMFB_LS
	_phy_rtd_part_outl(0x1800da78, 3, 2, 0x3); //[APHY para]  		G-Lane LEQ_CUR_ADJ
	_phy_rtd_part_outl(0x1800da78, 5, 4, 0x0); //[APHY para]  		G-Lane PTAT_CUR_ADJ
	_phy_rtd_part_outl(0x1800da78, 6, 6, 0x0); //[APHY para]  		G-Lane REG_BIAS_POW_CON_GM
	_phy_rtd_part_outl(0x1800da78, 11, 9, 0x0); //[APHY para] 		G-Lane PTAT_CUR_ADJ_FINE
	_phy_rtd_part_outl(0x1800da78, 16, 16, APHY_Para_frl_1[frl_timing].G_11_147_1); //[APHY para] 		G-Lane FR_CK_SEL
	_phy_rtd_part_outl(0x1800da7c, 11, 9, APHY_Para_frl_1[frl_timing].G_14_148_1); //[APHY para]   	G-Lane DEMUX_PIX_RATE_SEL<2:0>
	_phy_rtd_part_outl(0x1800da7c, 12, 12, APHY_Para_frl_1[frl_timing].G_14_149_1); //[APHY para]   	G-Lane DEMUX_FR_CK_SEL
	_phy_rtd_part_outl(0x1800da7c, 15, 13, 0x1); //[APHY para]   	G-Lane DEMUX_RATE_SEL
	_phy_rtd_part_outl(0x1800da7c, 24, 24, 0x0); //[APHY para]  		G-Lane QCG1_SEL_BIAS_ST
	_phy_rtd_part_outl(0x1800da7c, 25, 25, 0x0); //[APHY para]  		G-Lane QCG1_SEL_BIAS_RG
	_phy_rtd_part_outl(0x1800da7c, 27, 26, 0x0); //[APHY para]  		G-Lane QCG1 Rload SEL
	_phy_rtd_part_outl(0x1800da7c, 31, 28, 0xd); //[APHY para]  		G-Lane QCG1 Cload SEL
	_phy_rtd_part_outl(0x1800da80, 0, 0, 0x0); //[APHY para]  G-Lane QCG2_SEL_BIAS_ST
	_phy_rtd_part_outl(0x1800da80, 1, 1, 0x0); //[APHY para]  G-Lane QCG2_SEL_BIAS_RG
	_phy_rtd_part_outl(0x1800da80, 3, 2, 0x0); //[APHY para]  G-Lane QCG2 Rload SEL
	_phy_rtd_part_outl(0x1800da80, 7, 4, 0xd); //[APHY para]  G-Lane QCG2 Cload SEL
	_phy_rtd_part_outl(0x1800da80, 11, 9, 0x0); //[APHY para]  G-Lane PI_ISEL<2:0>
	_phy_rtd_part_outl(0x1800da80, 14, 14, 0x0); //[APHY para]  G-Lane PI_CSEL
	_phy_rtd_part_outl(0x1800da80, 20, 18, 0x4); //[APHY para]  G-Lane PI_DIV_SEL
	_phy_rtd_part_outl(0x1800da80, 22, 22, 0x1); //[APHY para]  G-Lane DCDR ckout SEL
	_phy_rtd_part_outl(0x1800da80, 31, 31, 0x0); //[APHY para]  G-Lane DFE Ckin SEL.
	_phy_rtd_part_outl(0x1800da84, 11, 10, APHY_Para_frl_1[frl_timing].ACDR_G_2_164_1); //[APHY para]  G-Lane ACDR_SEL_IDNBIAS_LV[1:0]
	_phy_rtd_part_outl(0x1800da84, 17, 17, 0x1); //[APHY para]  G-Lane ACDR_RSTB_DIV_BAND_2OR4_LV
	_phy_rtd_part_outl(0x1800da84, 19, 19, APHY_Para_frl_1[frl_timing].ACDR_G_3_166_1); //[APHY para]  G-Lane  ACDR_SEL_HS_CLK
	_phy_rtd_part_outl(0x1800da84, 20, 20, 0x1); //[APHY para]  G-Lane ACDR_SEL_0FR_1HR_DIV_IQ
	_phy_rtd_part_outl(0x1800da84, 23, 21, 0x0); //[APHY para]  G-Lane ACDR_SEL_DIV_BAND_2OR4_LV
	_phy_rtd_part_outl(0x1800da88, 1, 1, 0x0); //[APHY para]  G-Lane ACDR_POW_CP_INTG2_CORE
	_phy_rtd_part_outl(0x1800da88, 13, 12, APHY_Para_frl_1[frl_timing].ACDR_G_6_170_1); //[APHY para]  G-Lane ACDR_SEL_BAND_CAP
	_phy_rtd_part_outl(0x1800da88, 26, 25, APHY_Para_frl_1[frl_timing].ACDR_G_8_171_1); //[APHY para]  G-Lane BBPD_KP_SEL[1:0]
	_phy_rtd_part_outl(0x1800da88, 28, 27, 0x1); //[APHY para]  G-Lane BBPD_KI_SEL[1:0]
	_phy_rtd_part_outl(0x1800da88, 29, 29, 0x0); //[APHY para]  G-Lane BBPD_BYPASS_CTN_KP
	_phy_rtd_part_outl(0x1800da88, 30, 30, 0x0); //[APHY para]  G-Lane BBPD_BYPASS_CTN_KI
	_phy_rtd_part_outl(0x1800da8c, 7, 0, APHY_Para_frl_1[frl_timing].ACDR_G_9_175_1); //[APHY para]  G-Lane CMU_SEL_M_DIV[7:0]
	_phy_rtd_part_outl(0x1800da8c, 8, 8, 0x0); //[APHY para]  G-Lane CMU_SEL_M_DIV[8]
	_phy_rtd_part_outl(0x1800da8c, 9, 9, 0x0); //[APHY para]  G-Lane CMU_SEL_PRE_M_DIV
	_phy_rtd_part_outl(0x1800da8c, 10, 10, APHY_Para_frl_1[frl_timing].ACDR_G_10_178_1); //[APHY para]  G-Lane CMU_SEL_POST_M_DIV
	_phy_rtd_part_outl(0x1800da8c, 21, 20, APHY_Para_frl_1[frl_timing].ACDR_G_11_179_1); //[APHY para]  G-Lane RLSEL_LE1_2
	_phy_rtd_part_outl(0x1800da8c, 23, 22, APHY_Para_frl_1[frl_timing].ACDR_G_11_180_1); //[APHY para]  G-Lane RLSEL_NC_2
	_phy_rtd_part_outl(0x1800da8c, 24, 24, APHY_Para_frl_1[frl_timing].ACDR_G_12_181_1); //[APHY para]  G-Lane POW_NC
	_phy_rtd_part_outl(0x1800da90, 9, 8, APHY_Para_frl_1[frl_timing].R_2_182_1); //[APHY para]  		R-Lane RLSEL_LE1
	_phy_rtd_part_outl(0x1800da90, 11, 10, APHY_Para_frl_1[frl_timing].R_2_183_1); //[APHY para]  		R-Lane RLSEL_LE2
	_phy_rtd_part_outl(0x1800da90, 13, 12, APHY_Para_frl_1[frl_timing].R_2_184_1); //[APHY para]  		R-Lane RLSEL_NC
	_phy_rtd_part_outl(0x1800da90, 15, 14, APHY_Para_frl_1[frl_timing].R_2_185_1); //[APHY para]  		R-Lane RLSEL_TAP0
	_phy_rtd_part_outl(0x1800da90, 19, 16, APHY_Para_frl_1[frl_timing].R_3_186_1); //[APHY para]  		R-Lane RSSEL_LE1_1
	_phy_rtd_part_outl(0x1800da90, 23, 20, APHY_Para_frl_1[frl_timing].R_3_187_1); //[APHY para]  		R-Lane RSSEL_LE1_2
	_phy_rtd_part_outl(0x1800da90, 27, 24, APHY_Para_frl_1[frl_timing].R_4_188_1); //[APHY para]  		R-Lane RSSEL_LE2
	_phy_rtd_part_outl(0x1800da90, 29, 28, APHY_Para_frl_1[frl_timing].R_4_189_1); //[APHY para]  		R-Lane RSSEL_TAP0
	_phy_rtd_part_outl(0x1800da90, 31, 30, APHY_Para_frl_1[frl_timing].R_4_190_1); //[APHY para]  	R-Lane KOFF_RANGE
	_phy_rtd_part_outl(0x1800da94, 3, 0, APHY_Para_frl_1[frl_timing].R_5_191_1); //[APHY para]    	R-Lane LE1_ISEL_IN
	_phy_rtd_part_outl(0x1800da94, 7, 4, APHY_Para_frl_1[frl_timing].R_5_192_1); //[APHY para]    	R-Lane LE2_ISEL_IN
	_phy_rtd_part_outl(0x1800da94, 11, 8, APHY_Para_frl_1[frl_timing].R_6_193_1); //[APHY para]   		R-Lane LE_NC_ISEL_IN
	_phy_rtd_part_outl(0x1800da94, 15, 12, 0xf); //[APHY para]  		R-Lane TAP0_ISEL_IN
	_phy_rtd_part_outl(0x1800da94, 17, 16, 0x0); //[APHY para]   		R-Lane LE_IHALF[1:0]
	_phy_rtd_part_outl(0x1800da94, 18, 18, APHY_Para_frl_1[frl_timing].R_7_196_1); //[APHY para]   		R-Lane NC_IHALF
	_phy_rtd_part_outl(0x1800da94, 19, 19, APHY_Para_frl_1[frl_timing].R_7_197_1); //[APHY para]   		R-Lane TAP0_IHALF
	_phy_rtd_part_outl(0x1800da94, 20, 20, 0x0); //[APHY para]   		R-Lane EN_LEQ6G
	_phy_rtd_part_outl(0x1800da94, 22, 22, APHY_Para_frl_1[frl_timing].R_7_199_1); //[APHY para]   	R-Lane POW_NC
	_phy_rtd_part_outl(0x1800da94, 30, 28, 0x3); //[APHY para]   		R-Lane DATALANE_BIAS_ISEL[2:0]
	_phy_rtd_part_outl(0x1800da94, 31, 31, 0x1); //[APHY para]   		R-Lane POW_LEQ_RL
	_phy_rtd_part_outl(0x1800da98, 1, 1, 0x0); //[APHY para]  		R-Lane SEL_CMFB_LS
	_phy_rtd_part_outl(0x1800da98, 3, 2, 0x3); //[APHY para]  		R-Lane LEQ_CUR_ADJ
	_phy_rtd_part_outl(0x1800da98, 5, 4, 0x0); //[APHY para]  		R-Lane PTAT_CUR_ADJ
	_phy_rtd_part_outl(0x1800da98, 6, 6, 0x0); //[APHY para]  		R-Lane REG_BIAS_POW_CON_GM
	_phy_rtd_part_outl(0x1800da98, 11, 9, 0x0); //[APHY para] 		R-Lane PTAT_CUR_ADJ_FINE
	_phy_rtd_part_outl(0x1800da98, 16, 16, APHY_Para_frl_1[frl_timing].R_11_207_1); //[APHY para] 		R-Lane FR_CK_SEL
	_phy_rtd_part_outl(0x1800da9c, 11, 9, APHY_Para_frl_1[frl_timing].R_14_208_1); //[APHY para]   	R-Lane DEMUX_PIX_RATE_SEL<2:0>
	_phy_rtd_part_outl(0x1800da9c, 12, 12, APHY_Para_frl_1[frl_timing].R_14_209_1); //[APHY para]   	R-Lane DEMUX_FR_CK_SEL
	_phy_rtd_part_outl(0x1800da9c, 15, 13, 0x1); //[APHY para]   	R-Lane DEMUX_RATE_SEL
	_phy_rtd_part_outl(0x1800da9c, 24, 24, 0x0); //[APHY para]  		R-Lane QCG1_SEL_BIAS_ST
	_phy_rtd_part_outl(0x1800da9c, 25, 25, 0x0); //[APHY para]  		R-Lane QCG1_SEL_BIAS_RG
	_phy_rtd_part_outl(0x1800da9c, 27, 26, 0x0); //[APHY para]  		R-Lane QCG1 Rload SEL
	_phy_rtd_part_outl(0x1800da9c, 31, 28, 0xd); //[APHY para]  		R-Lane QCG1 Cload SEL
	_phy_rtd_part_outl(0x1800daa0, 0, 0, 0x0); //[APHY para]  R-Lane QCG2_SEL_BIAS_ST
	_phy_rtd_part_outl(0x1800daa0, 1, 1, 0x0); //[APHY para]  R-Lane QCG2_SEL_BIAS_RG
	_phy_rtd_part_outl(0x1800daa0, 3, 2, 0x0); //[APHY para]  R-Lane QCG2 Rload SEL
	_phy_rtd_part_outl(0x1800daa0, 7, 4, 0xd); //[APHY para]  R-Lane QCG2 Cload SEL
	_phy_rtd_part_outl(0x1800daa0, 11, 9, 0x0); //[APHY para]  R-Lane PI_ISEL<2:0>
	_phy_rtd_part_outl(0x1800daa0, 14, 14, 0x0); //[APHY para]  R-Lane PI_CSEL
	_phy_rtd_part_outl(0x1800daa0, 20, 18, 0x4); //[APHY para]  R-Lane PI_DIV_SEL
	_phy_rtd_part_outl(0x1800daa0, 22, 22, 0x1); //[APHY para]  R-Lane DCDR ckout SEL
	_phy_rtd_part_outl(0x1800daa0, 31, 31, 0x0); //[APHY para]  R-Lane DFE Ckin SEL.
	_phy_rtd_part_outl(0x1800daa4, 11, 10, APHY_Para_frl_1[frl_timing].ACDR_R_2_224_1); //[APHY para]  R-Lane ACDR_SEL_IDNBIAS_LV[1:0]
	_phy_rtd_part_outl(0x1800daa4, 17, 17, 0x1); //[APHY para]  R-Lane ACDR_RSTB_DIV_BAND_2OR4_LV
	_phy_rtd_part_outl(0x1800daa4, 19, 19, APHY_Para_frl_1[frl_timing].ACDR_R_3_226_1); //[APHY para]  R-Lane  ACDR_SEL_HS_CLK
	_phy_rtd_part_outl(0x1800daa4, 20, 20, 0x1); //[APHY para]  R-Lane ACDR_SEL_0FR_1HR_DIV_IQ
	_phy_rtd_part_outl(0x1800daa4, 23, 21, 0x0); //[APHY para]  R-Lane ACDR_SEL_DIV_BAND_2OR4_LV
	_phy_rtd_part_outl(0x1800daa8, 1, 1, 0x0); //[APHY para]  R-Lane ACDR_POW_CP_INTG2_CORE
	_phy_rtd_part_outl(0x1800daa8, 13, 12, APHY_Para_frl_1[frl_timing].ACDR_R_6_230_1); //[APHY para]  R-Lane ACDR_SEL_BAND_CAP
	_phy_rtd_part_outl(0x1800daa8, 26, 25, APHY_Para_frl_1[frl_timing].ACDR_R_8_231_1); //[APHY para]  R-Lane BBPD_KP_SEL[1:0]
	_phy_rtd_part_outl(0x1800daa8, 28, 27, 0x1); //[APHY para]  R-Lane BBPD_KI_SEL[1:0]
	_phy_rtd_part_outl(0x1800daa8, 29, 29, 0x0); //[APHY para]  R-Lane BBPD_BYPASS_CTN_KP
	_phy_rtd_part_outl(0x1800daa8, 30, 30, 0x0); //[APHY para]  R-Lane BBPD_BYPASS_CTN_KI
	_phy_rtd_part_outl(0x1800daac, 7, 0, APHY_Para_frl_1[frl_timing].ACDR_R_9_235_1); //[APHY para]  R-Lane CMU_SEL_M_DIV[7:0]
	_phy_rtd_part_outl(0x1800daac, 8, 8, 0x0); //[APHY para]  R-Lane CMU_SEL_M_DIV[8]
	_phy_rtd_part_outl(0x1800daac, 9, 9, 0x0); //[APHY para]  R-Lane CMU_SEL_PRE_M_DIV
	_phy_rtd_part_outl(0x1800daac, 10, 10, APHY_Para_frl_1[frl_timing].ACDR_R_10_238_1); //[APHY para]  R-Lane CMU_SEL_POST_M_DIV
	_phy_rtd_part_outl(0x1800daac, 21, 20, APHY_Para_frl_1[frl_timing].ACDR_R_11_239_1); //[APHY para]  R-Lane RLSEL_LE1_2
	_phy_rtd_part_outl(0x1800daac, 23, 22, APHY_Para_frl_1[frl_timing].ACDR_R_11_240_1); //[APHY para]  R-Lane RLSEL_NC_2
	_phy_rtd_part_outl(0x1800daac, 24, 24, APHY_Para_frl_1[frl_timing].ACDR_R_12_241_1); //[APHY para]  R-Lane POW_NC
	_phy_rtd_part_outl(0x1800dab4, 4, 4, 0x0); //[APHY para]  CMU input clock source select:
	_phy_rtd_part_outl(0x1800dab4, 7, 7, 0x0); //[APHY para]  CMU_CKXTAL_SEL
	_phy_rtd_part_outl(0x1800dab8, 8, 8, 0x0); //[APHY para  CMU_SEL_CP
}

void DPHY_Init_Flow_frl_1_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	// DPHY init flow start
	_phy_rtd_part_outl(0x1800db88, 4, 4, 0x0); //[DPHY init flow]  reg_p0_b_fifo_en
	_phy_rtd_part_outl(0x1800db88, 12, 12, 0x0); //[DPHY init flow]  reg_p0_g_fifo_en
	_phy_rtd_part_outl(0x1800db88, 20, 20, 0x0); //[DPHY init flow]  reg_p0_r_fifo_en
	_phy_rtd_part_outl(0x1800db88, 28, 28, 0x0); //[DPHY init flow]  reg_p0_ck_fifo_en
	_phy_rtd_part_outl(0x1800dc6c, 1, 1, 0x1); //[DPHY init flow]  reg_p0_pow_save_10b18b_sel
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0x0e); //[DPHY init flow]  reg_p0_dig_debug_sel
	//DPHY fld flow start
	_phy_rtd_part_outl(0x1800dbb8, 10, 3, 0x1e); //[DPHY fld flow]  reg_p0_b_divide_num
	_phy_rtd_part_outl(0x1800dbbc, 11, 0, DPHY_Init_Flow_frl_1[frl_timing].b_lock_dn_limit_10_1); //[DPHY fld flow] reg_p0_b_lock_dn_limit
	_phy_rtd_part_outl(0x1800dbbc, 27, 16, DPHY_Init_Flow_frl_1[frl_timing].b_lock_up_limit_11_1); //[DPHY fld flow] reg_p0_b_lock_up_limit
	_phy_rtd_part_outl(0x1800dbc4, 19, 8, DPHY_Init_Flow_frl_1[frl_timing].b_coarse_lock_dn_limit_12_1); //[DPHY fld flow] reg_p0_b_coarse_lock_dn_limit
	_phy_rtd_part_outl(0x1800dbc4, 31, 20, DPHY_Init_Flow_frl_1[frl_timing].b_coarse_lock_up_limit_13_1); //[DPHY fld flow] reg_p0_b_coarse_lock_up_limit
	_phy_rtd_part_outl(0x1800dbc8, 1, 1, 0x1); //[DPHY fld flow]  reg_p0_b_fld_rstb
	_phy_rtd_part_outl(0x1800dbd8, 10, 3, 0x1e); //[DPHY fld flow]  reg_p0_g_divide_num
	_phy_rtd_part_outl(0x1800dbdc, 11, 0, DPHY_Init_Flow_frl_1[frl_timing].g_lock_dn_limit_16_1); //[DPHY fld flow] reg_p0_g_lock_dn_limit
	_phy_rtd_part_outl(0x1800dbdc, 27, 16, DPHY_Init_Flow_frl_1[frl_timing].g_lock_up_limit_17_1); //[DPHY fld flow] reg_p0_g_lock_up_limit
	_phy_rtd_part_outl(0x1800dbe4, 19, 8, DPHY_Init_Flow_frl_1[frl_timing].g_coarse_lock_dn_limit_18_1); //[DPHY fld flow] reg_p0_g_coarse_lock_dn_limit
	_phy_rtd_part_outl(0x1800dbe4, 31, 20, DPHY_Init_Flow_frl_1[frl_timing].g_coarse_lock_up_limit_19_1); //[DPHY fld flow] reg_p0_g_coarse_lock_up_limit
	_phy_rtd_part_outl(0x1800dbe8, 1, 1, 0x1); //[DPHY fld flow]  reg_p0_g_fld_rstb
	_phy_rtd_part_outl(0x1800dbf4, 10, 3, 0x1e); //[DPHY fld flow]  reg_p0_r_divide_num
	_phy_rtd_part_outl(0x1800dbf8, 11, 0, DPHY_Init_Flow_frl_1[frl_timing].r_lock_dn_limit_22_1); //[DPHY fld flow] reg_p0_r_lock_dn_limit
	_phy_rtd_part_outl(0x1800dbf8, 27, 16, DPHY_Init_Flow_frl_1[frl_timing].r_lock_up_limit_23_1); //[DPHY fld flow] reg_p0_r_lock_up_limit
	_phy_rtd_part_outl(0x1800dc00, 19, 8, DPHY_Init_Flow_frl_1[frl_timing].r_coarse_lock_dn_limit_24_1); //[DPHY fld flow] reg_p0_r_coarse_lock_dn_limit
	_phy_rtd_part_outl(0x1800dc00, 31, 20, DPHY_Init_Flow_frl_1[frl_timing].r_coarse_lock_up_limit_25_1); //[DPHY fld flow] reg_p0_r_coarse_lock_up_limit
	_phy_rtd_part_outl(0x1800dc04, 1, 1, 0x1); //[DPHY fld flow]  reg_p0_r_fld_rstb
	_phy_rtd_part_outl(0x1800dc10, 10, 3, 0x1e); //[DPHY fld flow]  reg_p0_ck_divide_num
	_phy_rtd_part_outl(0x1800dc14, 11, 0, DPHY_Init_Flow_frl_1[frl_timing].ck_lock_dn_limit_28_1); //[DPHY fld flow] reg_p0_ck_lock_dn_limit
	_phy_rtd_part_outl(0x1800dc14, 27, 16, DPHY_Init_Flow_frl_1[frl_timing].ck_lock_up_limit_29_1); //[DPHY fld flow] reg_p0_ck_lock_up_limit
	_phy_rtd_part_outl(0x1800dc1c, 19, 8, DPHY_Init_Flow_frl_1[frl_timing].ck_coarse_lock_dn_limit_30_1); //[DPHY fld flow] reg_p0_ck_coarse_lock_dn_limit
	_phy_rtd_part_outl(0x1800dc1c, 31, 20, DPHY_Init_Flow_frl_1[frl_timing].ck_coarse_lock_up_limit_31_1); //[DPHY fld flow] reg_p0_ck_coarse_lock_up_limit
	_phy_rtd_part_outl(0x1800dc20, 1, 1, 0x1); //[DPHY fld flow]  reg_p0_ck_fld_rstb
	_phy_rtd_part_outl(0x1800dd10, 26, 26, 0x1); //[ADPHY Init flow]  reg_adphy_rst
	_phy_rtd_part_outl(0x1800dd1c, 0, 0, 0x0); //[ADPHY Init flow]  prevent address merging
	_phy_rtd_part_outl(0x1800dd10, 31, 31, 0x1); //[ADPHY Init flow]  reg_adphy_latch_clk_0=1
	_phy_rtd_part_outl(0x1800dd10, 30, 30, 0x1); //[ADPHY Init flow]  reg_adphy_latch_clk_1=1
	_phy_rtd_part_outl(0x1800dd1c, 0, 0, 0x0); //[ADPHY Init flow]  prevent address merging
	_phy_rtd_part_outl(0x1800dd10, 31, 31, 0x0); //[ADPHY Init flow]  reg_adphy_latch_clk_0=0
	_phy_rtd_part_outl(0x1800dd10, 30, 30, 0x0); //[ADPHY Init flow]  reg_adphy_latch_clk_1=0
}

void APHY_Init_Flow_frl_1_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	// APHY init flow start
	_phy_rtd_part_outl(0x1800da68, 19, 19, 0x1); //B //REG_P0_ACDR_B_7 [3] TIE VCTRL = 1
	_phy_rtd_part_outl(0x1800da88, 19, 19, 0x1); //G //REG_P0_ACDR_B_7 [3] TIE VCTRL = 1
	_phy_rtd_part_outl(0x1800daa8, 19, 19, 0x1); //R //REG_P0_ACDR_B_7 [3] TIE VCTRL = 1
	_phy_rtd_part_outl(0x1800da48, 19, 19, 0x1); //CK //REG_P0_ACDR_B_7 [3] TIE VCTRL = 1
	_phy_rtd_part_outl(0x1800da18, 1, 1, 0x0); //[APHY init flow]  REG_CK_LATCH Before PSM, latch the register value of PHY. 0->1: Latch
	_phy_rtd_part_outl(0x1800da30, 6, 6, 0x0); //[APHY init flow]  CK-Lane FAST_SW_EN
	_phy_rtd_part_outl(0x1800da30, 7, 7, 0x0); //[APHY init flow]  CK-Lane FAST_SW_DELAY_EN
	_phy_rtd_part_outl(0x1800da38, 17, 17, 0x1); //[APHY init flow] 	CK-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da40, 16, 16, 0x0); //[APHY init flow]  CK-Lane DCDR_RSTB
	_phy_rtd_part_outl(0x1800da40, 17, 17, 0x0); //[APHY init flow]  CK-Lane PI_DIV rstb
	_phy_rtd_part_outl(0x1800da40, 24, 24, 0x0); //[APHY init flow]  CK-Lane FKP_RSTB (FKP:Fast KP)
	_phy_rtd_part_outl(0x1800da40, 28, 28, 0x1); //[APHY init flow]  CK-Lane FKT EN
	_phy_rtd_part_outl(0x1800da44, 1, 1, 0x1); //[APHY init flow]  CK-Lane ACDR_RSTB_CLK_FLD
	_phy_rtd_part_outl(0x1800da44, 16, 16, 0x1); //[APHY init flow]  CK-Lane ACDR_RSTB_DIV_IQ
	_phy_rtd_part_outl(0x1800da48, 24, 24, 0x1); //[APHY init flow]  CK-Lane RSTB_BBPD_KP_KI
	_phy_rtd_part_outl(0x1800da4c, 11, 11, 0x1); //[APHY init flow]  CK-Lane RSTB_PRESCALER
	_phy_rtd_part_outl(0x1800da4c, 12, 12, 0x1); //[APHY init flow]  CK-Lane RSTB_M_DIV
	udelay(100);
	_phy_rtd_part_outl(0x1800da44, 0, 0, 0x1); //[APHY init flow]  CK-Lane ACDR_RSTB_PFD
	_phy_rtd_part_outl(0x1800da50, 6, 6, 0x0); //[APHY init flow]  	B-Lane FAST_SW_EN
	_phy_rtd_part_outl(0x1800da50, 7, 7, 0x0); //[APHY init flow]  	B-Lane FAST_SW_DELAY_EN
	_phy_rtd_part_outl(0x1800da58, 17, 17, 0x1); //[APHY init flow] 	B-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da60, 16, 16, 0x0); //[APHY init flow]  B-Lane DCDR_RSTB
	_phy_rtd_part_outl(0x1800da60, 17, 17, 0x0); //[APHY init flow]  B-Lane PI_DIV rstb
	_phy_rtd_part_outl(0x1800da60, 24, 24, 0x0); //[APHY init flow]  B-Lane FKP_RSTB (FKP:Fast KP)
	_phy_rtd_part_outl(0x1800da60, 28, 28, 0x1); //[APHY init flow]  B-Lane FKT EN
	_phy_rtd_part_outl(0x1800da64, 1, 1, 0x1); //[APHY init flow]  B-Lane ACDR_RSTB_CLK_FLD
	_phy_rtd_part_outl(0x1800da64, 16, 16, 0x1); //[APHY init flow]  B-Lane ACDR_RSTB_DIV_IQ
	_phy_rtd_part_outl(0x1800da68, 24, 24, 0x1); //[APHY init flow]  B-Lane RSTB_BBPD_KP_KI
	_phy_rtd_part_outl(0x1800da6c, 11, 11, 0x1); //[APHY init flow]  B-Lane RSTB_PRESCALER
	_phy_rtd_part_outl(0x1800da6c, 12, 12, 0x1); //[APHY init flow]  B-Lane RSTB_M_DIV
	udelay(100);
	_phy_rtd_part_outl(0x1800da64, 0, 0, 0x1); //[APHY init flow]  B-Lane ACDR_RSTB_PFD
	_phy_rtd_part_outl(0x1800da70, 6, 6, 0x0); //[APHY init flow]  	G-Lane FAST_SW_EN
	_phy_rtd_part_outl(0x1800da70, 7, 7, 0x0); //[APHY init flow]  	G-Lane FAST_SW_DELAY_EN
	_phy_rtd_part_outl(0x1800da78, 17, 17, 0x1); //[APHY init flow] 	G-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da80, 16, 16, 0x0); //[APHY init flow]  G-Lane DCDR_RSTB
	_phy_rtd_part_outl(0x1800da80, 17, 17, 0x0); //[APHY init flow]  G-Lane PI_DIV rstb
	_phy_rtd_part_outl(0x1800da80, 24, 24, 0x0); //[APHY init flow]  G-Lane FKP_RSTB (FKP:Fast KP)
	_phy_rtd_part_outl(0x1800da80, 28, 28, 0x1); //[APHY init flow]  G-Lane FKT EN
	_phy_rtd_part_outl(0x1800da84, 1, 1, 0x1); //[APHY init flow]  G-Lane ACDR_RSTB_CLK_FLD
	_phy_rtd_part_outl(0x1800da84, 16, 16, 0x1); //[APHY init flow]  G-Lane ACDR_RSTB_DIV_IQ
	_phy_rtd_part_outl(0x1800da88, 24, 24, 0x1); //[APHY init flow]  G-Lane RSTB_BBPD_KP_KI
	_phy_rtd_part_outl(0x1800da8c, 11, 11, 0x1); //[APHY init flow]  G-Lane RSTB_PRESCALER
	_phy_rtd_part_outl(0x1800da8c, 12, 12, 0x1); //[APHY init flow]  G-Lane RSTB_M_DIV
	udelay(100);
	_phy_rtd_part_outl(0x1800da84, 0, 0, 0x1); //[APHY init flow]  G-Lane ACDR_RSTB_PFD
	_phy_rtd_part_outl(0x1800da90, 6, 6, 0x0); //[APHY init flow]  	R-Lane FAST_SW_EN
	_phy_rtd_part_outl(0x1800da90, 7, 7, 0x0); //[APHY init flow]  	R-Lane FAST_SW_DELAY_EN
	_phy_rtd_part_outl(0x1800da98, 17, 17, 0x1); //[APHY init flow] 	R-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800daa0, 16, 16, 0x0); //[APHY init flow]  R-Lane DCDR_RSTB
	_phy_rtd_part_outl(0x1800daa0, 17, 17, 0x0); //[APHY init flow]  R-Lane PI_DIV rstb
	_phy_rtd_part_outl(0x1800daa0, 24, 24, 0x0); //[APHY init flow]  R-Lane FKP_RSTB (FKP:Fast KP)
	_phy_rtd_part_outl(0x1800daa0, 28, 28, 0x1); //[APHY init flow]  R-Lane FKT EN
	_phy_rtd_part_outl(0x1800daa4, 1, 1, 0x1); //[APHY init flow]  R-Lane ACDR_RSTB_CLK_FLD
	_phy_rtd_part_outl(0x1800daa4, 16, 16, 0x1); //[APHY init flow]  R-Lane ACDR_RSTB_DIV_IQ
	_phy_rtd_part_outl(0x1800daa8, 24, 24, 0x1); //[APHY init flow]  R-Lane RSTB_BBPD_KP_KI
	_phy_rtd_part_outl(0x1800daac, 11, 11, 0x1); //[APHY init flow]  R-Lane RSTB_PRESCALER
	_phy_rtd_part_outl(0x1800daac, 12, 12, 0x1); //[APHY init flow]  R-Lane RSTB_M_DIV
	udelay(100);
	_phy_rtd_part_outl(0x1800daa4, 0, 0, 0x1); //[APHY init flow]  R-Lane ACDR_RSTB_PFD
	_phy_rtd_part_outl(0x1800da68, 19, 19, 0x0); //B //REG_P0_ACDR_B_7 [3] TIE VCTRL = 0
	_phy_rtd_part_outl(0x1800da88, 19, 19, 0x0); //G //REG_P0_ACDR_B_7 [3] TIE VCTRL = 0
	_phy_rtd_part_outl(0x1800daa8, 19, 19, 0x0); //R //REG_P0_ACDR_B_7 [3] TIE VCTRL = 0
	_phy_rtd_part_outl(0x1800da48, 19, 19, 0x0); //CK //REG_P0_ACDR_B_7 [3] TIE VCTRL = 0
	//APHY acdr flow start
	_phy_rtd_part_outl(0x1800dafc, 0, 0, 0x1); //[APHY acdr flow]  P0_ACDR_B_ACDR_EN_AFN
	_phy_rtd_part_outl(0x1800dafc, 1, 1, 0x1); //[APHY acdr flow]  P0_ACDR_G_ACDR_EN_AFN
	_phy_rtd_part_outl(0x1800dafc, 2, 2, 0x1); //[APHY acdr flow]  P0_ACDR_R_ACDR_EN_AFN
	_phy_rtd_part_outl(0x1800dafc, 3, 3, 0x1); //[APHY acdr flow]  P0_ACDR_CK_ACDR_EN_AFN
	_phy_rtd_part_outl(0x1800dafc, 4, 4, 0x1); //[APHY acdr flow]  P0_B_DEMUX_RSTB
	_phy_rtd_part_outl(0x1800dafc, 5, 5, 0x1); //[APHY acdr flow]  P0_G_DEMUX_RSTB
	_phy_rtd_part_outl(0x1800dafc, 6, 6, 0x1); //[APHY acdr flow]  P0_R_DEMUX_RSTB
	_phy_rtd_part_outl(0x1800dafc, 7, 7, 0x1); //[APHY acdr flow]  P0_CK_DEMUX_RSTB
}

void ACDR_settings_frl_1_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	//#====================================== CMU RESET
	_phy_rtd_part_outl(0x1800dafc, 4, 4, 0x0);
	_phy_rtd_part_outl(0x1800dafc, 5, 5, 0x0);
	_phy_rtd_part_outl(0x1800dafc, 6, 6, 0x0);
	_phy_rtd_part_outl(0x1800dafc, 7, 7, 0x0);
	_phy_rtd_part_outl(0x1800dafc, 4, 4, 0x1);
	_phy_rtd_part_outl(0x1800dafc, 5, 5, 0x1);
	_phy_rtd_part_outl(0x1800dafc, 6, 6, 0x1);
	_phy_rtd_part_outl(0x1800dafc, 7, 7, 0x1);
	//#====================================== phase I
	//#====================================== FLD reset
	_phy_rtd_part_outl(0x1800dbc8, 0, 0, 0x0);
	_phy_rtd_part_outl(0x1800dbe8, 0, 0, 0x0);
	_phy_rtd_part_outl(0x1800dc04, 0, 0, 0x0);
	_phy_rtd_part_outl(0x1800dc20, 0, 0, 0x0);
	ScalerTimer_DelayXms(1);
	//ScalerTimer_DelayXms(5);000
	//#============= HDMI FRL RX flow checked by jfsyeh 20221110
	//#================ PLL closed loop done, ready for offsetK
}

void ACDR_settings_frl_2_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	//#15000
	_phy_rtd_part_outl(0x1800da38, 17, 17, 0); //CK-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da58, 17, 17, 0); //B-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da78, 17, 17, 0); //G-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da98, 17, 17, 0); //R-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da38, 17, 17, 1); //CK-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da58, 17, 17, 1); //B-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da78, 17, 17, 1); //G-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800da98, 17, 17, 1); //R-Lane BBPD_RSTB
	_phy_rtd_part_outl(0x1800db88, 27, 27, 1); //DFE data enable
	_phy_rtd_part_outl(0x1800db88, 19, 19, 1); //DFE data enable
	_phy_rtd_part_outl(0x1800db88, 11, 11, 1); //DFE data enable
	_phy_rtd_part_outl(0x1800db88, 3, 3, 1); //DFE data enable
}

void Koffset_frl_1_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	_phy_rtd_part_outl(0x1800dc04, 0, 0, 0); //R finetunstart off
	_phy_rtd_part_outl(0x1800dbe8, 0, 0, 0); //G finetunstart off
	_phy_rtd_part_outl(0x1800dbc8, 0, 0, 0); //B finetunstart off
	_phy_rtd_part_outl(0x1800dc20, 0, 0, 0); //CK finetunstart off
	ScalerTimer_DelayXms(1);
	//========================================= STEP1 Data_Even KOFF =================================================
	_phy_rtd_part_outl(0x1800dcb0, 15, 15, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 14, 14, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 13, 13, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 12, 12, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 11, 11, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 10, 10, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 9, 9, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 8, 8, 0); //offset_code/vth select
	_phy_rtd_part_outl(0x1800da58, 28, 28, 1); //#B  DFE_TAP0_ICOM_EN=1
	_phy_rtd_part_outl(0x1800da5c, 6, 3, 0); //#B  DFE_TAP_EN[4:1] = 0
	_phy_rtd_part_outl(0x1800da38, 28, 28, 1); //#CK DFE_TAP0_ICOM_EN=1
	_phy_rtd_part_outl(0x1800da3c, 6, 3, 0); //#CK DFE_TAP_EN[4:1] = 0
	_phy_rtd_part_outl(0x1800da78, 28, 28, 1); //#G  DFE_TAP0_ICOM_EN=1
	_phy_rtd_part_outl(0x1800da7c, 6, 3, 0); //#G  DFE_TAP_EN[4:1] = 0
	_phy_rtd_part_outl(0x1800da98, 28, 28, 1); //#R  DFE_TAP0_ICOM_EN=1
	_phy_rtd_part_outl(0x1800da9c, 6, 3, 0); //#R  DFE_TAP_EN[4:1] = 0
	_phy_rtd_outl(0x1800db44, 0x0282cf00); //B_KOFF rstn,  //[26:20] timout = 0101000, [19:14] divisor = 001000, [11:8] delay count = 11
	_phy_rtd_outl(0x1800db54, 0x0282cf00); //G_KOFF rstn
	_phy_rtd_outl(0x1800db64, 0x0282cf00); //R_KOFF rstn
	_phy_rtd_outl(0x1800db74, 0x0282cf00); //CK_KOFF rstn
	_phy_rtd_part_outl(0x1800db44, 0, 0, 1); //Release B_KOFF rstn
	_phy_rtd_part_outl(0x1800db54, 0, 0, 1); //Release G_KOFF rstn
	_phy_rtd_part_outl(0x1800db64, 0, 0, 1); //Release R_KOFF rstn
	_phy_rtd_part_outl(0x1800db74, 0, 0, 1); //Release CK_KOFF rstn
	_phy_rtd_part_outl(0x1800dca0, 30, 28, 0x2); //after offset_en=1wait time 0=0us ; 2=10us
	_phy_rtd_part_outl(0x1800dca4, 30, 28, 0x2); //after offset_en=1wait time 0=0us ; 2=10us
	_phy_rtd_part_outl(0x1800dca8, 30, 28, 0x2); //after offset_en=1wait time 0=0us ; 2=10us
	_phy_rtd_part_outl(0x1800dcac, 30, 28, 0x2); //after offset_en=1wait time 0=0us ; 2=10us
	//===BLANE KOFF SETTINGS
	_phy_rtd_part_outl(0x1800db4c, 28, 28, 0); //manaul_do=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db4c, 18, 18, 0); //manaul_de=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db50, 28, 28, 0); //manaul_eq=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db50, 18, 18, 0); //manaul_eo=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db50, 8, 8, 0); //manaul_ee=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db48, 18, 18, 0); //manaul_opo=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db48, 8, 8, 0); //manaul_ope=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800da50, 0, 0, 1); //INOFF_EN =0 ->1
	_phy_rtd_part_outl(0x1800da58, 0, 0, 0); //REG_B_9[0] POW_CMFB_LEQ 1->0
	_phy_rtd_part_outl(0x1800da54, 23, 22, 0); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  1->0
	_phy_rtd_part_outl(0x1800da54, 24, 24, 0); //REG_B_8[0] POW_LEQ:  1->0
	//B-Lane z0_ok
	_phy_rtd_part_outl(0x1800db50, 19, 19, 1); //[19]z0_ok_eo=1
	_phy_rtd_part_outl(0x1800db50, 9, 9, 1); //[9] z0_ok_ee=1
	_phy_rtd_part_outl(0x1800db4c, 29, 29, 1); //[29]z0_ok_do=1
	_phy_rtd_part_outl(0x1800db4c, 19, 19, 1); //[19]z0_ok_de=1
	_phy_rtd_part_outl(0x1800db48, 19, 19, 1); //[19] z0_ok_opo=1
	_phy_rtd_part_outl(0x1800db48, 9, 9, 1); //[9] z0_ok_ope=1
	_phy_rtd_part_outl(0x1800db4c, 21, 21, 0); //[21]offset_pc_do=0
	_phy_rtd_part_outl(0x1800db4c, 11, 11, 0); //[11]offset_pc_de=0
	_phy_rtd_part_outl(0x1800db50, 1, 1, 0); //[1]offset_pc_ee=0
	_phy_rtd_part_outl(0x1800db50, 11, 11, 0); //[11]offset_pc_eo=0
	_phy_rtd_part_outl(0x1800db50, 21, 21, 0); //[21]offset_pc_eq=0
	_phy_rtd_part_outl(0x1800db48, 1, 1, 0); //[1]offset_pc_ope=0
	_phy_rtd_part_outl(0x1800db48, 11, 11, 0); //[11]offset_pc_opo=0
	//===G LANE KOFF SETTINGS
	_phy_rtd_part_outl(0x1800db5c, 28, 28, 0); //manaul_do=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db5c, 18, 18, 0); //manaul_de=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db60, 28, 28, 0); //manaul_eq=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db60, 18, 18, 0); //manaul_eo=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db60, 8, 8, 0); //manaul_ee=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db58, 18, 18, 0); //manaul_opo=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db58, 8, 8, 0); //manaul_ope=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800da70, 0, 0, 1); //INOFF_EN =0 ->1
	_phy_rtd_part_outl(0x1800da78, 0, 0, 0); //REG_B_9[0] POW_CMFB_LEQ 1->0
	_phy_rtd_part_outl(0x1800da74, 23, 22, 0); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  1->0
	_phy_rtd_part_outl(0x1800da74, 24, 24, 0); //REG_B_8[0] POW_LEQ:  1->0
	//G-Lane z0_ok
	_phy_rtd_part_outl(0x1800db60, 19, 19, 1); //[19]z0_ok_eo=1
	_phy_rtd_part_outl(0x1800db60, 9, 9, 1); //[9] z0_ok_ee=1
	_phy_rtd_part_outl(0x1800db5c, 29, 29, 1); //[29]z0_ok_do=1
	_phy_rtd_part_outl(0x1800db5c, 19, 19, 1); //[19]z0_ok_de=1
	_phy_rtd_part_outl(0x1800db58, 19, 19, 1); //[19] z0_ok_opo=1
	_phy_rtd_part_outl(0x1800db58, 9, 9, 1); //[9] z0_ok_ope=1
	_phy_rtd_part_outl(0x1800db5c, 21, 21, 0); //[21]offset_pc_do=0
	_phy_rtd_part_outl(0x1800db5c, 11, 11, 0); //[11]offset_pc_de=0
	_phy_rtd_part_outl(0x1800db60, 1, 1, 0); //[1]offset_pc_ee=0
	_phy_rtd_part_outl(0x1800db60, 11, 11, 0); //[11]offset_pc_eo=0
	_phy_rtd_part_outl(0x1800db60, 21, 21, 0); //[21]offset_pc_eq=0
	_phy_rtd_part_outl(0x1800db58, 1, 1, 0); //[1]offset_pc_ope=0
	_phy_rtd_part_outl(0x1800db58, 11, 11, 0); //[11]offset_pc_opo=0
	//===R LANE KOFF SETTINGS
	_phy_rtd_part_outl(0x1800db6c, 28, 28, 0); //manaul_do=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db6c, 18, 18, 0); //manaul_de=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db70, 28, 28, 0); //manaul_eq=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db70, 18, 18, 0); //manaul_eo=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db70, 8, 8, 0); //manaul_ee=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db68, 18, 18, 0); //manaul_opo=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db68, 8, 8, 0); //manaul_ope=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800da90, 0, 0, 1); //INOFF_EN =0 ->1
	_phy_rtd_part_outl(0x1800da98, 0, 0, 0); //REG_B_9[0] POW_CMFB_LEQ 1->0
	_phy_rtd_part_outl(0x1800da94, 23, 22, 0); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  1->0
	_phy_rtd_part_outl(0x1800da94, 24, 24, 0); //REG_B_8[0] POW_LEQ:  1->0
	//R-Lane z0_ok
	_phy_rtd_part_outl(0x1800db70, 19, 19, 1); //[19]z0_ok_eo=1
	_phy_rtd_part_outl(0x1800db70, 9, 9, 1); //[9] z0_ok_ee=1
	_phy_rtd_part_outl(0x1800db6c, 29, 29, 1); //[29]z0_ok_do=1
	_phy_rtd_part_outl(0x1800db6c, 19, 19, 1); //[19]z0_ok_de=1
	_phy_rtd_part_outl(0x1800db68, 19, 19, 1); //[19] z0_ok_opo=1
	_phy_rtd_part_outl(0x1800db68, 9, 9, 1); //[9] z0_ok_ope=1
	_phy_rtd_part_outl(0x1800db6c, 21, 21, 0); //[21]offset_pc_do=0
	_phy_rtd_part_outl(0x1800db6c, 11, 11, 0); //[11]offset_pc_de=0
	_phy_rtd_part_outl(0x1800db70, 1, 1, 0); //[1]offset_pc_ee=0
	_phy_rtd_part_outl(0x1800db70, 11, 11, 0); //[11]offset_pc_eo=0
	_phy_rtd_part_outl(0x1800db70, 21, 21, 0); //[21]offset_pc_eq=0
	_phy_rtd_part_outl(0x1800db68, 1, 1, 0); //[1]offset_pc_ope=0
	_phy_rtd_part_outl(0x1800db68, 11, 11, 0); //[11]offset_pc_opo=0
	//===CK LANE KOFF SETTINGS
	_phy_rtd_part_outl(0x1800db7c, 28, 28, 0); //manaul_do=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db7c, 18, 18, 0); //manaul_de=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db80, 28, 28, 0); //manaul_eq=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db80, 18, 18, 0); //manaul_eo=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db80, 8, 8, 0); //manaul_ee=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db78, 18, 18, 0); //manaul_opo=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800db78, 8, 8, 0); //manaul_ope=0, 0:Auto; 1:Manual
	_phy_rtd_part_outl(0x1800da30, 0, 0, 1); //INOFF_EN =0 ->1
	_phy_rtd_part_outl(0x1800da38, 0, 0, 0); //REG_B_9[0] POW_CMFB_LEQ 1->0
	_phy_rtd_part_outl(0x1800da34, 23, 22, 0); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  1->0
	_phy_rtd_part_outl(0x1800da34, 24, 24, 0); //REG_B_8[0] POW_LEQ:  1->0
	//CK-Lane z0_ok
	_phy_rtd_part_outl(0x1800db80, 19, 19, 1); //[19]z0_ok_eo=1
	_phy_rtd_part_outl(0x1800db80, 9, 9, 1); //[9] z0_ok_ee=1
	_phy_rtd_part_outl(0x1800db7c, 29, 29, 1); //[29]z0_ok_do=1
	_phy_rtd_part_outl(0x1800db7c, 19, 19, 1); //[19]z0_ok_de=1
	_phy_rtd_part_outl(0x1800db78, 19, 19, 1); //[19] z0_ok_opo=1
	_phy_rtd_part_outl(0x1800db78, 9, 9, 1); //[9] z0_ok_ope=1
	_phy_rtd_part_outl(0x1800db7c, 21, 21, 0); //[21]offset_pc_do=0
	_phy_rtd_part_outl(0x1800db7c, 11, 11, 0); //[11]offset_pc_de=0
	_phy_rtd_part_outl(0x1800db80, 1, 1, 0); //[1]offset_pc_ee=0
	_phy_rtd_part_outl(0x1800db80, 11, 11, 0); //[11]offset_pc_eo=0
	_phy_rtd_part_outl(0x1800db80, 21, 21, 0); //[21]offset_pc_eq=0
	_phy_rtd_part_outl(0x1800db78, 1, 1, 0); //[1]offset_pc_ope=0
	_phy_rtd_part_outl(0x1800db78, 11, 11, 0); //[11]offset_pc_opo=0
	//Enable B-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db4c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db4c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db50, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db50, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db48, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db48, 0, 0, 1); //[0]offset_en_ope=1
	//Enable G-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db5c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db5c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db60, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db60, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db58, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db58, 0, 0, 1); //[0]offset_en_ope=1
	//Enable R-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db6c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db6c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db70, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db70, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db68, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db68, 0, 0, 1); //[0]offset_en_ope=1
	//Enable CK-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db7c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db7c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db80, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db80, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db78, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db78, 0, 0, 1); //[0]offset_en_ope=1
	ScalerTimer_DelayXms(1);
	//Check Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0x8); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db44, 7, 4, 0x9); //reg_p0_b_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db44, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db44, 29, 29));
	//Check Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0xA); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db54, 7, 4, 0x9); //reg_p0_g_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EOOffset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db54, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db54, 29, 29));
	//Check Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0xC); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db64, 7, 4, 0x9); //reg_p0_g_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EOOffset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db64, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db64, 29, 29));
	//Check Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0xE); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db74, 7, 4, 0x9); //reg_p0_ck_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EOOffset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db74, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db74, 29, 29));
	//Disable B-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db4c, 10, 10, 0); //[10]offset_en_de=1->0
	_phy_rtd_part_outl(0x1800db4c, 20, 20, 0); //[20]offset_en_do=1->0
	_phy_rtd_part_outl(0x1800db50, 0, 0, 0); //[20]offset_en_ee=1->0
	_phy_rtd_part_outl(0x1800db50, 10, 10, 0); //[20]offset_en_eo=1->0
	_phy_rtd_part_outl(0x1800db48, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db48, 0, 0, 0); //[0]offset_en_ope=1->0
	//Disable G-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db5c, 10, 10, 0); //[10]offset_en_de=1->0
	_phy_rtd_part_outl(0x1800db5c, 20, 20, 0); //[20]offset_en_do=1->0
	_phy_rtd_part_outl(0x1800db60, 0, 0, 0); //[20]offset_en_ee=1->0
	_phy_rtd_part_outl(0x1800db60, 10, 10, 0); //[20]offset_en_eo=1->0
	_phy_rtd_part_outl(0x1800db58, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db58, 0, 0, 0); //[0]offset_en_ope=1->0
	//Disable R-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db6c, 10, 10, 0); //[10]offset_en_de=1->0
	_phy_rtd_part_outl(0x1800db6c, 20, 20, 0); //[20]offset_en_do=1->0
	_phy_rtd_part_outl(0x1800db70, 0, 0, 0); //[20]offset_en_ee=1->0
	_phy_rtd_part_outl(0x1800db70, 10, 10, 0); //[20]offset_en_eo=1->0
	_phy_rtd_part_outl(0x1800db68, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db68, 0, 0, 0); //[0]offset_en_ope=1->0
	//Disable CK-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db7c, 10, 10, 0); //[10]offset_en_de=1->0
	_phy_rtd_part_outl(0x1800db7c, 20, 20, 0); //[20]offset_en_do=1->0
	_phy_rtd_part_outl(0x1800db80, 0, 0, 0); //[20]offset_en_ee=1->0
	_phy_rtd_part_outl(0x1800db80, 10, 10, 0); //[20]offset_en_eo=1->0
	_phy_rtd_part_outl(0x1800db78, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db78, 0, 0, 0); //[0]offset_en_ope=1->0
	//B-Lane DCVS Offset Result
	_phy_rtd_part_outl(0x1800db44, 7, 4, 0); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 1); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 2); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 3); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 4); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 5); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	//G-Lane DCVS Offset Result
	_phy_rtd_part_outl(0x1800db54, 7, 4, 0); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 1); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 2); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 3); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 4); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 5); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	//R-Lane DCVS Offset Result
	_phy_rtd_part_outl(0x1800db64, 7, 4, 0); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 1); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 2); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 3); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 4); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 5); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	//CK-Lane DCVS Offset Result
	_phy_rtd_part_outl(0x1800db74, 7, 4, 0); //reg_p0_ck_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	_phy_rtd_part_outl(0x1800db74, 7, 4, 1); //reg_p0_ck_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	_phy_rtd_part_outl(0x1800db74, 7, 4, 2); //reg_p0_ck_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	_phy_rtd_part_outl(0x1800db74, 7, 4, 3); //reg_p0_ck_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	_phy_rtd_part_outl(0x1800db74, 7, 4, 4); //reg_p0_ck_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	_phy_rtd_part_outl(0x1800db74, 7, 4, 5); //reg_p0_ck_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	//========================================= STEP2 LEQ KOFF =================================================
	//===B LANE KOFF LEQ SETTINGS
	_phy_rtd_part_outl(0x1800da54, 23, 22, Koffset_frl_1[frl_timing].B_7_284_1); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  0->1
	_phy_rtd_part_outl(0x1800da54, 24, 24, 1); //REG_B_8[0] POW_LEQ:  0->1
	_phy_rtd_part_outl(0x1800da58, 0, 0, 1); //REG_B_9[0] POW_CMFB_LEQ 0->1
	_phy_rtd_part_outl(0x1800db50, 19, 19, 0); //[19]z0_ok_eo=0
	_phy_rtd_part_outl(0x1800db50, 9, 9, 0); //[9] z0_ok_ee=0
	_phy_rtd_part_outl(0x1800db4c, 29, 29, 0); //[19]z0_ok_do=0
	//===G LANE KOFF LEQ SETTINGS
	_phy_rtd_part_outl(0x1800da74, 23, 22, Koffset_frl_1[frl_timing].G_7_291_1); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  0->1
	_phy_rtd_part_outl(0x1800da74, 24, 24, 1); //REG_B_8[0] POW_LEQ:  0->1
	_phy_rtd_part_outl(0x1800da78, 0, 0, 1); //REG_B_9[0] POW_CMFB_LEQ 0->1
	_phy_rtd_part_outl(0x1800db60, 19, 19, 0); //[19]z0_ok_eo=0
	_phy_rtd_part_outl(0x1800db60, 9, 9, 0); //[9] z0_ok_ee=0
	_phy_rtd_part_outl(0x1800db5c, 29, 29, 0); //[19]z0_ok_do=0
	//===R LANE KOFF LEQ SETTINGS
	_phy_rtd_part_outl(0x1800da94, 23, 22, Koffset_frl_1[frl_timing].R_7_298_1); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  0->1
	_phy_rtd_part_outl(0x1800da94, 24, 24, 1); //REG_B_8[0] POW_LEQ:  0->1
	_phy_rtd_part_outl(0x1800da98, 0, 0, 1); //REG_B_9[0] POW_CMFB_LEQ 0->1
	_phy_rtd_part_outl(0x1800db70, 19, 19, 0); //[19]z0_ok_eo=0
	_phy_rtd_part_outl(0x1800db70, 9, 9, 0); //[9] z0_ok_ee=0
	_phy_rtd_part_outl(0x1800db6c, 29, 29, 0); //[19]z0_ok_do=0
	//===CK LANE KOFF LEQ SETTINGS
	_phy_rtd_part_outl(0x1800da34, 23, 22, Koffset_frl_1[frl_timing].CK_7_305_1); //REG_B_7[7:6] POW_LEQ_KOFF, POW_NC:  0->1
	_phy_rtd_part_outl(0x1800da34, 24, 24, 1); //REG_B_8[0] POW_LEQ:  0->1
	_phy_rtd_part_outl(0x1800da38, 0, 0, 1); //REG_B_9[0] POW_CMFB_LEQ 0->1
	_phy_rtd_part_outl(0x1800db80, 19, 19, 0); //[19]z0_ok_eo=0
	_phy_rtd_part_outl(0x1800db80, 9, 9, 0); //[9] z0_ok_ee=0
	_phy_rtd_part_outl(0x1800db7c, 29, 29, 0); //[19]z0_ok_do=0
	_phy_rtd_part_outl(0x1800db50, 20, 20, 1); //[21]offset_en_eq=0->1
	_phy_rtd_part_outl(0x1800db60, 20, 20, 1); //[21]offset_en_eq=0->1
	_phy_rtd_part_outl(0x1800db70, 20, 20, 1); //[21]offset_en_eq=0->1
	_phy_rtd_part_outl(0x1800db80, 20, 20, 1); //[21]offset_en_eq=0->1
	ScalerTimer_DelayXms(1);
	HDMI_PHY_INFO("p0_b_offset_eq_ok_ro	EQ OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db44, 30, 30));
	HDMI_PHY_INFO("p0_g_offset_eq_ok_ro EQ OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db54, 30, 30));
	HDMI_PHY_INFO("p0_r_offset_eq_ok_ro EQ OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db64, 30, 30));
	HDMI_PHY_INFO("p0_ck_offset_eq_ok_ro EQ OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db74, 30, 30));
	_phy_rtd_part_outl(0x1800db50, 20, 20, 0); //[20]offset_en_eq=0
	_phy_rtd_part_outl(0x1800db60, 20, 20, 0); //[20]offset_en_eq=0
	_phy_rtd_part_outl(0x1800db70, 20, 20, 0); //[20]offset_en_eq=0
	_phy_rtd_part_outl(0x1800db80, 20, 20, 0); //[20]offset_en_eq=0
	//B-Lane LEQ Offset Result
	_phy_rtd_part_outl(0x1800db44, 7, 4, 8); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 8:EQ
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	//G-Lane LEQ Offset Result
	_phy_rtd_part_outl(0x1800db54, 7, 4, 8); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 8:EQ
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	//R-Lane LEQ Offset Result
	_phy_rtd_part_outl(0x1800db64, 7, 4, 8); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 8:EQ
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	//CK-Lane LEQ Offset Result
	_phy_rtd_part_outl(0x1800db74, 7, 4, 8); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 8:EQ
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	//========================================= STEP3 All DCVS KOFF ==
	//===B LANE KOFF Data/Edge
	_phy_rtd_part_outl(0x1800db4c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db4c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db50, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db50, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db48, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db48, 0, 0, 1); //[0]offset_en_ope=1
	//===G LANE KOFF Data/Edge
	_phy_rtd_part_outl(0x1800db5c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db5c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db60, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db60, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db58, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db58, 0, 0, 1); //[0]offset_en_ope=1
	//===R LANE KOFF Data/Edge
	_phy_rtd_part_outl(0x1800db6c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db6c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db70, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db70, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db68, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db68, 0, 0, 1); //[0]offset_en_ope=1
	//===CK LANE KOFF Data/Edge
	_phy_rtd_part_outl(0x1800db7c, 10, 10, 1); //[10]offset_en_de=1
	_phy_rtd_part_outl(0x1800db7c, 20, 20, 1); //[20]offset_en_do=1
	_phy_rtd_part_outl(0x1800db80, 0, 0, 1); //[20]offset_en_ee=1
	_phy_rtd_part_outl(0x1800db80, 10, 10, 1); //[20]offset_en_eo=1
	_phy_rtd_part_outl(0x1800db78, 10, 10, 1); //[10]offset_en_opo=1
	_phy_rtd_part_outl(0x1800db78, 0, 0, 1); //[0]offset_en_ope=1
	ScalerTimer_DelayXms(1);
	//B-Lane DCVS Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0x8); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db44, 7, 4, 0x9); //reg_p0_b_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EOOffset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db44, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db44, 29, 29));
	//G-Lane DCVS Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0xA); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db54, 7, 4, 0x9); //reg_p0_g_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db54, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db54, 29, 29));
	//R-Lane DCVS Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0xC); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db64, 7, 4, 0x9); //reg_p0_r_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db64, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db64, 29, 29));
	//CK-Lane DCVS Offset cal. OK Check
	_phy_rtd_part_outl(0x1800dc7c, 5, 0, 0xE); //reg_p0_dig_debug_sel; Debug port0 signal selection
	_phy_rtd_part_outl(0x1800db74, 7, 4, 0x9); //reg_p0_ck_offset_coef_sel
	HDMI_PHY_INFO("DE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 27, 27));
	HDMI_PHY_INFO("DO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 26, 26));
	HDMI_PHY_INFO("EE Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 25, 25));
	HDMI_PHY_INFO("EO Offset cal. OK Check = %d\n",_phy_rtd_part_inl(0x1800dc84, 24, 24));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db74, 28, 28));
	HDMI_PHY_INFO("DATA OFFSET OK = %d\n",_phy_rtd_part_inl(0x1800db74, 29, 29));
	//Disable B-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db4c, 10, 10, 0); //[10]offset_en_de=1->0
	_phy_rtd_part_outl(0x1800db4c, 20, 20, 0); //[20]offset_en_do=1->0
	_phy_rtd_part_outl(0x1800db50, 0, 0, 0); //[20]offset_en_ee=1->0
	_phy_rtd_part_outl(0x1800db50, 10, 10, 0); //[20]offset_en_eo=1->0
	_phy_rtd_part_outl(0x1800db48, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db48, 0, 0, 0); //[0]offset_en_ope=1->0
	//Disable G-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db5c, 10, 10, 0); //[10]offset_en_de=0
	_phy_rtd_part_outl(0x1800db5c, 20, 20, 0); //[20]offset_en_do=0
	_phy_rtd_part_outl(0x1800db60, 0, 0, 0); //[20]offset_en_ee=0
	_phy_rtd_part_outl(0x1800db60, 10, 10, 0); //[20]offset_en_eo=0
	_phy_rtd_part_outl(0x1800db58, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db58, 0, 0, 0); //[0]offset_en_ope=1->0
	//Disable R-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db6c, 10, 10, 0); //[10]offset_en_de=0
	_phy_rtd_part_outl(0x1800db6c, 20, 20, 0); //[20]offset_en_do=0
	_phy_rtd_part_outl(0x1800db70, 0, 0, 0); //[20]offset_en_ee=0
	_phy_rtd_part_outl(0x1800db70, 10, 10, 0); //[20]offset_en_eo=0
	_phy_rtd_part_outl(0x1800db68, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db68, 0, 0, 0); //[0]offset_en_ope=1->0
	//Disable CK-Lane DCVS KOFF
	_phy_rtd_part_outl(0x1800db7c, 10, 10, 0); //[10]offset_en_de=0
	_phy_rtd_part_outl(0x1800db7c, 20, 20, 0); //[20]offset_en_do=0
	_phy_rtd_part_outl(0x1800db80, 0, 0, 0); //[20]offset_en_ee=0
	_phy_rtd_part_outl(0x1800db80, 10, 10, 0); //[20]offset_en_eo=0
	_phy_rtd_part_outl(0x1800db78, 10, 10, 0); //[10]offset_en_opo=1->0
	_phy_rtd_part_outl(0x1800db78, 0, 0, 0); //[0]offset_en_ope=1->0
	//B-Lane LEQ & DCVS Offset Result
	_phy_rtd_part_outl(0x1800db44, 7, 4, 0); //reg_p0_b_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 1); //reg_p0_b_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 2); //reg_p0_b_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 3); //reg_p0_b_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 8); //reg_p0_b_offset_coef_sel[4:0]
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 4); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	_phy_rtd_part_outl(0x1800db44, 7, 4, 5); //reg_p0_b_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 4, 0));
	//G-Lane LEQ & DCVS Offset Result
	_phy_rtd_part_outl(0x1800db54, 7, 4, 0); //reg_p0_g_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 1); //reg_p0_g_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 2); //reg_p0_g_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 3); //reg_p0_g_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 8); //reg_p0_g_offset_coef_sel[4:0]
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 4); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	_phy_rtd_part_outl(0x1800db54, 7, 4, 5); //reg_p0_g_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 12, 8));
	//R-Lane LEQ & DCVS Offset Result
	_phy_rtd_part_outl(0x1800db64, 7, 4, 0); //reg_p0_r_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 1); //reg_p0_r_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 2); //reg_p0_r_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 3); //reg_p0_r_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 8); //reg_p0_r_offset_coef_sel[4:0]
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 4); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	_phy_rtd_part_outl(0x1800db64, 7, 4, 5); //reg_p0_r_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 20, 16));
	//CK-Lane LEQ & DCVS Offset Result
	_phy_rtd_part_outl(0x1800db74, 7, 4, 0); //reg_p0_ck_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	_phy_rtd_part_outl(0x1800db74, 7, 4, 1); //reg_p0_ck_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Data_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	_phy_rtd_part_outl(0x1800db74, 7, 4, 2); //reg_p0_ck_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	_phy_rtd_part_outl(0x1800db74, 7, 4, 3); //reg_p0_ck_offset_coef_sel[4:0]
	HDMI_PHY_INFO("Edge_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	_phy_rtd_part_outl(0x1800db74, 7, 4, 8); //reg_p0_ck_offset_coef_sel[4:0]
	HDMI_PHY_INFO("EQ_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	_phy_rtd_part_outl(0x1800db74, 7, 4, 4); //reg_p0_ck_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Even_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	_phy_rtd_part_outl(0x1800db74, 7, 4, 5); //reg_p0_ck_offset_coef_sel[4:0]	//0:Data_Even, 1:Data_Odd, 2:Edge_Even, 3:Edge_Odd, 4:OFFP_Even, 5:OFFP_Odd, 8:EQ
	HDMI_PHY_INFO("OFFP_Odd_Koffset = %d\n",_phy_rtd_part_inl(0x1800db84, 28, 24));
	//Input on
	_phy_rtd_part_outl(0x1800da50, 0, 0, 0); //INOFF_EN =0 Input_on
	_phy_rtd_part_outl(0x1800da70, 0, 0, 0); //INOFF_EN =0 Input_on
	_phy_rtd_part_outl(0x1800da90, 0, 0, 0); //INOFF_EN =0 Input_on
	_phy_rtd_part_outl(0x1800da30, 0, 0, 0); //INOFF_EN =0 Input_on
	_phy_rtd_part_outl(0x1800da58, 28, 28, 0); //#B  DFE_TAP0_ICOM_EN=0
	_phy_rtd_part_outl(0x1800da5c, 6, 3, Koffset_frl_1[frl_timing].B_13_494_1); //#B  DFE_TAP_EN[4:1] = 0xf
	_phy_rtd_part_outl(0x1800da38, 28, 28, 0); //#CK DFE_TAP0_ICOM_EN=0
	_phy_rtd_part_outl(0x1800da3c, 6, 3, Koffset_frl_1[frl_timing].CK_13_496_1); //#CK DFE_TAP_EN[4:1] = 0xf
	_phy_rtd_part_outl(0x1800da78, 28, 28, 0); //#G  DFE_TAP0_ICOM_EN=0
	_phy_rtd_part_outl(0x1800da7c, 6, 3, Koffset_frl_1[frl_timing].G_13_498_1); //#G  DFE_TAP_EN[4:1] = 0xf
	_phy_rtd_part_outl(0x1800da98, 28, 28, 0); //#R  DFE_TAP0_ICOM_EN=0
	_phy_rtd_part_outl(0x1800da9c, 6, 3, Koffset_frl_1[frl_timing].R_13_500_1); //#R  DFE_TAP_EN[4:1] = 0xf
}

void LEQ_VTH_Tap0_3_4_Adapt_frl_1_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	_phy_rtd_part_outl(0x1800dc04, 0, 0, 0); //finetunstart off
	_phy_rtd_part_outl(0x1800dbe8, 0, 0, 0); //finetunstart off
	_phy_rtd_part_outl(0x1800dbc8, 0, 0, 0); //finetunstart off
	_phy_rtd_part_outl(0x1800dc20, 0, 0, 0); //finetunstart off
	ScalerTimer_DelayXms(1);
	_phy_rtd_part_outl(0x1800dcb0, 15, 15, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 14, 14, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 13, 13, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 12, 12, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 11, 11, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 10, 10, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 9, 9, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 8, 8, 1); //offset_code/vth select
	//DFE_Adaptation
	//===CK LANE TAP0 & LEQ & Tap3 & Tap4 Adapt
	_phy_rtd_outl(0x1800decc, 0x00007400); //Timer, set eqfe_en,servo_en,timer_en=1
	_phy_rtd_part_outl(0x1800def8, 23, 23, 1); //Inverse vth sign polarity
	_phy_rtd_part_outl(0x1800ded0, 31, 31, 1); //leq1_inv=1
	_phy_rtd_part_outl(0x1800ded0, 30, 30, 1); //leq2_inv=0
	_phy_rtd_part_outl(0x1800ded4, 17, 15, 0); //LEQ_GAIN1=0, realsim=7
	_phy_rtd_part_outl(0x1800ded4, 14, 12, 0); //LEQ_GAIN2=0
	_phy_rtd_part_outl(0x1800ded4, 8, 6, 0); //TAP1_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800ded4, 5, 3, 0); //TAP2_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800ded4, 2, 0, 0); //TAP3_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800ded8, 31, 29, 0); //TAP4_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800ded8, 22, 21, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].LEQ1_TRANS_MODE_CK_1_0_25_1); //20211122_LEQ1_trans_mode=2
	_phy_rtd_part_outl(0x1800ded8, 20, 19, 3); //LEQ2_trans_mode=3
	_phy_rtd_part_outl(0x1800ded8, 28, 26, 2); //tap0_gain=4
	_phy_rtd_part_outl(0x1800ded8, 25, 23, 2); //tap0_gain=4
	_phy_rtd_part_outl(0x1800decc, 26, 24, 5); //20200824_tap_timer=5
	_phy_rtd_part_outl(0x1800decc, 20, 18, 5); //20200825_LEQ_timer=5
	_phy_rtd_part_outl(0x1800decc, 23, 21, 5); //20200825_VTH_timer=5
	_phy_rtd_part_outl(0x1800decc, 13, 13, 1); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800dee8, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dee8, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800dee8, 27, 27, 0); //20211122_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800dee8, 28, 28, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_CK_6_0_36_1); //20211122_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800dee8, 29, 29, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_CK_6_0_37_1); //20211122_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800dec8, 22, 22, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 23, 23, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dedc, 31, 30, 0x1); //tap0_adjust_b =1
	//===B LANE  TAP0 & LEQ & Tap3 & Tap4 Adapt
	_phy_rtd_outl(0x1800de8c, 0x00007400); //Timer, set eqfe_en,servo_en,timer_en=1
	_phy_rtd_part_outl(0x1800deb8, 23, 23, 1); //Inverse vth sign polarity
	_phy_rtd_part_outl(0x1800de90, 31, 31, 1); //leq1_inv=1
	_phy_rtd_part_outl(0x1800de90, 30, 30, 1); //leq2_inv=0
	_phy_rtd_part_outl(0x1800de94, 17, 15, 0); //LEQ_GAIN1=0, realsim=7
	_phy_rtd_part_outl(0x1800de94, 14, 12, 0); //LEQ_GAIN2=0
	_phy_rtd_part_outl(0x1800de94, 8, 6, 0); //TAP1_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800de94, 5, 3, 0); //TAP2_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800de94, 2, 0, 0); //TAP3_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800de98, 31, 29, 0); //TAP4_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800de98, 22, 21, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].LEQ1_TRANS_MODE_B_1_0_52_1); //20211122_LEQ1_trans_mode=2
	_phy_rtd_part_outl(0x1800de98, 20, 19, 3); //LEQ2_trans_mode=3
	_phy_rtd_part_outl(0x1800de98, 28, 26, 2); //tap0_gain=4
	_phy_rtd_part_outl(0x1800de98, 25, 23, 2); //tap0_gain=4
	_phy_rtd_part_outl(0x1800de8c, 26, 24, 5); //20200824_tap_timer=5
	_phy_rtd_part_outl(0x1800de8c, 20, 18, 5); //20200825_LEQ_timer=5
	_phy_rtd_part_outl(0x1800de8c, 23, 21, 5); //20200825_VTH_timer=5
	_phy_rtd_part_outl(0x1800de8c, 13, 13, 1); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800dea8, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dea8, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800dea8, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800dea8, 28, 28, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_B_6_0_63_1); //20211122_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800dea8, 29, 29, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_B_6_0_64_1); //20211122_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800de88, 22, 22, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 23, 23, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de9c, 31, 30, 0x1); //tap0_adjust_b =1
	//===G LANE  TAP0 & LEQ & Tap3 & Tap4 Adapt
	_phy_rtd_outl(0x1800de4c, 0x00007400); //Timer, set eqfe_en,servo_en,timer_en=1
	_phy_rtd_part_outl(0x1800de78, 23, 23, 1); //Inverse vth sign polarity
	_phy_rtd_part_outl(0x1800de50, 31, 31, 1); //leq1_inv=1
	_phy_rtd_part_outl(0x1800de50, 30, 30, 1); //leq2_inv=0
	_phy_rtd_part_outl(0x1800de54, 17, 15, 0); //LEQ_GAIN1=0, realsim=7
	_phy_rtd_part_outl(0x1800de54, 14, 12, 0); //LEQ_GAIN2=0
	_phy_rtd_part_outl(0x1800de54, 8, 6, 0); //TAP1_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800de54, 5, 3, 0); //TAP2_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800de54, 2, 0, 0); //TAP3_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800de58, 31, 29, 0); //TAP4_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800de58, 22, 21, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].LEQ1_TRANS_MODE_G_1_0_79_1); //20211122_LEQ1_trans_mode=2
	_phy_rtd_part_outl(0x1800de58, 20, 19, 3); //LEQ2_trans_mode=3
	_phy_rtd_part_outl(0x1800de58, 28, 26, 2); //tap0_gain=4
	_phy_rtd_part_outl(0x1800de58, 25, 23, 2); //tap0_gain=4
	_phy_rtd_part_outl(0x1800de4c, 26, 24, 5); //20200824_tap_timer=5
	_phy_rtd_part_outl(0x1800de4c, 20, 18, 5); //20200825_LEQ_timer=5
	_phy_rtd_part_outl(0x1800de4c, 23, 21, 5); //20200825_VTH_timer=5
	_phy_rtd_part_outl(0x1800de4c, 13, 13, 1); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de68, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de68, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800de68, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800de68, 28, 28, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_G_6_0_90_1); //20211122_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800de68, 29, 29, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_G_6_0_91_1); //20211122_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800de48, 22, 22, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 23, 23, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de5c, 31, 30, 0x1); //tap0_adjust_b =1
	//===R LANE TAP0 & LEQ & Tap3 & Tap4 Adapt
	_phy_rtd_outl(0x1800de0c, 0x00007400); //Timer, set eqfe_en,servo_en,timer_en=1
	_phy_rtd_part_outl(0x1800de38, 23, 23, 1); //Inverse vth sign polarity
	_phy_rtd_part_outl(0x1800de10, 31, 31, 1); //leq1_inv=1
	_phy_rtd_part_outl(0x1800de10, 30, 30, 1); //leq2_inv=0
	_phy_rtd_part_outl(0x1800de14, 17, 15, 0); //LEQ_GAIN1=0, realsim=7
	_phy_rtd_part_outl(0x1800de14, 14, 12, 0); //LEQ_GAIN2=0
	_phy_rtd_part_outl(0x1800de14, 8, 6, 0); //TAP1_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800de14, 5, 3, 0); //TAP2_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800de14, 2, 0, 0); //TAP3_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800de18, 31, 29, 0); //TAP4_GAIN=0, realsim=7
	_phy_rtd_part_outl(0x1800de18, 22, 21, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].LEQ1_TRANS_MODE_R_1_0_106_1); //20211122_LEQ1_trans_mode=2
	_phy_rtd_part_outl(0x1800de18, 20, 19, 3); //LEQ2_trans_mode=3
	_phy_rtd_part_outl(0x1800de18, 28, 26, 2); //tap0_gain=4
	_phy_rtd_part_outl(0x1800de18, 25, 23, 2); //tap0_gain=4
	_phy_rtd_part_outl(0x1800de0c, 26, 24, 5); //20200824_tap_timer=5
	_phy_rtd_part_outl(0x1800de0c, 20, 18, 5); //20200825_LEQ_timer=5
	_phy_rtd_part_outl(0x1800de0c, 23, 21, 5); //20200825_VTH_timer=5
	_phy_rtd_part_outl(0x1800de0c, 13, 13, 1); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de28, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=1
	_phy_rtd_part_outl(0x1800de28, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800de28, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800de28, 28, 28, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_R_6_0_117_1); //20211122_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800de28, 29, 29, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_R_6_0_118_1); //20211122_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800de08, 22, 22, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de08, 23, 23, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de1c, 31, 30, 0x1); //tap0_adjust_b =1
	//Start DFE adapt
	_phy_rtd_part_outl(0x1800dc70, 0, 0, 0x1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 1, 1, 0x1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 2, 2, 0x1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 3, 3, 0x1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 4, 4, 0x1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 5, 5, 0x1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 6, 6, 0x1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 7, 7, 0x1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 12, 12, 0x1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 13, 13, 0x1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 14, 14, 0x1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 15, 15, 0x1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 8, 8, 0x1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc70, 9, 9, 0x1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc70, 10, 10, 0x1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc70, 11, 11, 0x1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc04, 0, 0, 1); //finetunstart on
	_phy_rtd_part_outl(0x1800dbe8, 0, 0, 1); //finetunstart on
	_phy_rtd_part_outl(0x1800dbc8, 0, 0, 1); //finetunstart on
	_phy_rtd_part_outl(0x1800dc20, 0, 0, LEQ_VTH_Tap0_3_4_Adapt_frl_1[frl_timing].ck_fine_tune_start_142_1); //finetunstart on
	ScalerTimer_DelayXms(5);
	//CK-Lane Tap0-4 Adapt
	_phy_rtd_part_outl(0x1800decc, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800dee8, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dee8, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=1
	_phy_rtd_part_outl(0x1800dee8, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=1
	_phy_rtd_part_outl(0x1800dee8, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800dee8, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800dec8, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	//B-Lane Tap0-4 Adapt
	_phy_rtd_part_outl(0x1800de8c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800dea8, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dea8, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=1
	_phy_rtd_part_outl(0x1800dea8, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=1
	_phy_rtd_part_outl(0x1800dea8, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800dea8, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800de88, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	//G-Lane Tap0-4 Adapt
	_phy_rtd_part_outl(0x1800de4c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de68, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de68, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=1
	_phy_rtd_part_outl(0x1800de68, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=1
	_phy_rtd_part_outl(0x1800de68, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800de68, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800de48, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	//R-Lane Tap0-4 Adapt
	_phy_rtd_part_outl(0x1800de0c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de28, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de28, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=1
	_phy_rtd_part_outl(0x1800de28, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=1
	_phy_rtd_part_outl(0x1800de28, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800de28, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800de08, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de08, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
}

void Tap0_to_Tap4_Adapt_frl_1_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	_phy_rtd_part_outl(0x1800dc04, 0, 0, 0); //finetunstart off
	_phy_rtd_part_outl(0x1800dbe8, 0, 0, 0); //finetunstart off
	_phy_rtd_part_outl(0x1800dbc8, 0, 0, 0); //finetunstart off
	_phy_rtd_part_outl(0x1800dc20, 0, 0, 0); //finetunstart off
	ScalerTimer_DelayXms(1);
	_phy_rtd_part_outl(0x1800dcb0, 15, 15, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 14, 14, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 13, 13, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 12, 12, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 11, 11, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 10, 10, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 9, 9, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dcb0, 8, 8, 1); //offset_code/vth select
	_phy_rtd_part_outl(0x1800dc70, 0, 0, 0x1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 1, 1, 0x1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 2, 2, 0x1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 3, 3, 0x1); //ck,r,g,b rstb_eq=1
	_phy_rtd_part_outl(0x1800dc70, 4, 4, 0x1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 5, 5, 0x1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 6, 6, 0x1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 7, 7, 0x1); //ck,r,g,b leq_en=1
	_phy_rtd_part_outl(0x1800dc70, 12, 12, 0x1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 13, 13, 0x1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 14, 14, 0x1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 15, 15, 0x1); //ck,r,g,b vth_en=1
	_phy_rtd_part_outl(0x1800dc70, 8, 8, 0x1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc70, 9, 9, 0x1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc70, 10, 10, 0x1); //dfe_en=1
	_phy_rtd_part_outl(0x1800dc70, 11, 11, 0x1); //dfe_en=1
	//CK-Lane
	_phy_rtd_part_outl(0x1800decc, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800dee8, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dee8, 26, 26, 1); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800dee8, 27, 27, 1); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800dee8, 28, 28, Tap0_to_Tap4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_CK_6_0_34_1); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800dee8, 29, 29, Tap0_to_Tap4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_CK_6_0_35_1); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800dec8, 22, 22, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 23, 23, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	//B-Lane
	_phy_rtd_part_outl(0x1800de8c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800dea8, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dea8, 26, 26, 1); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800dea8, 27, 27, 1); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800dea8, 28, 28, Tap0_to_Tap4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_B_6_0_43_1); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800dea8, 29, 29, Tap0_to_Tap4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_B_6_0_44_1); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800de88, 22, 22, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 23, 23, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	//G-Lane
	_phy_rtd_part_outl(0x1800de4c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de68, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de68, 26, 26, 1); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800de68, 27, 27, 1); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800de68, 28, 28, Tap0_to_Tap4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_G_6_0_52_1); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800de68, 29, 29, Tap0_to_Tap4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_G_6_0_53_1); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800de48, 22, 22, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 23, 23, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	//R-Lane
	_phy_rtd_part_outl(0x1800de0c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de28, 25, 25, 1); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de28, 26, 26, 1); //20200824_DFE_ADAP_EN_tap1=0
	_phy_rtd_part_outl(0x1800de28, 27, 27, 1); //20200824_DFE_ADAP_EN_tap2=0
	_phy_rtd_part_outl(0x1800de28, 28, 28, Tap0_to_Tap4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_R_6_0_61_1); //20200824_DFE_ADAP_EN_tap3=0
	_phy_rtd_part_outl(0x1800de28, 29, 29, Tap0_to_Tap4_Adapt_frl_1[frl_timing].DFE_ADAPT_EN_R_6_0_62_1); //20200824_DFE_ADAP_EN_tap4=0
	_phy_rtd_part_outl(0x1800de08, 22, 22, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de08, 23, 23, 0x1); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dc04, 0, 0, 1); //R finetunstart on
	_phy_rtd_part_outl(0x1800dbe8, 0, 0, 1); //G finetunstart on
	_phy_rtd_part_outl(0x1800dbc8, 0, 0, 1); //B finetunstart on
	_phy_rtd_part_outl(0x1800dc20, 0, 0, Tap0_to_Tap4_Adapt_frl_1[frl_timing].ck_fine_tune_start_68_1); //CK finetunstart on
	ScalerTimer_DelayXms(5);
	//CK-Lane
	_phy_rtd_part_outl(0x1800decc, 13, 13, 0); //20210807_DFE ADAP LE=0
#if !HDMI_FRL_TAP0234_ADP_EN
	_phy_rtd_part_outl(0x1800dee8, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
#else

#endif
	_phy_rtd_part_outl(0x1800dee8, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=1
#if !HDMI_FRL_TAP0234_ADP_EN
	_phy_rtd_part_outl(0x1800dee8, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=1
	_phy_rtd_part_outl(0x1800dee8, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800dee8, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=1
#else
	
#endif
	_phy_rtd_part_outl(0x1800dec8, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800dec8, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	//B-Lane
	_phy_rtd_part_outl(0x1800de8c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800dea8, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800dea8, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=1
	_phy_rtd_part_outl(0x1800dea8, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=1
	_phy_rtd_part_outl(0x1800dea8, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800dea8, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800de88, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de88, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	//G-Lane
	_phy_rtd_part_outl(0x1800de4c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de68, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de68, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=1
	_phy_rtd_part_outl(0x1800de68, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=1
	_phy_rtd_part_outl(0x1800de68, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800de68, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800de48, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de48, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	//R-Lane
	_phy_rtd_part_outl(0x1800de0c, 13, 13, 0); //20210807_DFE ADAP LE=0
	_phy_rtd_part_outl(0x1800de28, 25, 25, 0); //20210807_DFE_ADAP_EN_tap0=0
	_phy_rtd_part_outl(0x1800de28, 26, 26, 0); //20200824_DFE_ADAP_EN_tap1=1
	_phy_rtd_part_outl(0x1800de28, 27, 27, 0); //20200824_DFE_ADAP_EN_tap2=1
	_phy_rtd_part_outl(0x1800de28, 28, 28, 0); //20200824_DFE_ADAP_EN_tap3=1
	_phy_rtd_part_outl(0x1800de28, 29, 29, 0); //20200824_DFE_ADAP_EN_tap4=1
	_phy_rtd_part_outl(0x1800de08, 22, 22, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
	_phy_rtd_part_outl(0x1800de08, 23, 23, 0); //VTH_DFE_EN_B=1, VTH_EN_B=1
}

void ACDR_settings_frl_3_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	_phy_rtd_part_outl(0x1800dbc8, 0, 0, 0x1); //finetunestart_b
	_phy_rtd_part_outl(0x1800dbe8, 0, 0, 0x1); //finetunestart_g
	_phy_rtd_part_outl(0x1800dc04, 0, 0, 0x1); //finetunestart_r
	_phy_rtd_part_outl(0x1800dc20, 0, 0, finetunestart_on[frl_timing].ck_fine_tune_start_4_3); //finetunestart_ck
	//#===================================== phase III, ACDR to koffset mode
}

void ACDR_settings_frl_4_func(unsigned char nport, FRL_VALUE_PARAM_T frl_timing)
{
	_phy_rtd_part_outl(0x1800db88, 4, 4, 0x0);
	_phy_rtd_part_outl(0x1800db88, 12, 12, 0x0);
	_phy_rtd_part_outl(0x1800db88, 20, 20, 0x0);
	_phy_rtd_part_outl(0x1800db88, 28, 28, 0x0);
	_phy_rtd_part_outl(0x1800db00, 0, 0, 0x0);
	_phy_rtd_part_outl(0x1800db00, 1, 1, 0x0);
	_phy_rtd_part_outl(0x1800db00, 2, 2, 0x0);
	_phy_rtd_part_outl(0x1800db00, 3, 3, 0x0);
	_phy_rtd_part_outl(0x1800db00, 0, 0, 0x1);
	_phy_rtd_part_outl(0x1800db00, 1, 1, 0x1);
	_phy_rtd_part_outl(0x1800db00, 2, 2, 0x1);
	_phy_rtd_part_outl(0x1800db00, 3, 3, 0x1);
	//DFE Adaptation power off
	_phy_rtd_part_outl(0x1800da3c, 7, 7, 0); //Mark2_Modify; CK-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da5c, 7, 7, 0); //Mark2_Modify; B-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da7c, 7, 7, 0); //Mark2_Modify; G-Lane DFE_ADAPT_EN
	_phy_rtd_part_outl(0x1800da9c, 7, 7, 0); //Mark2_Modify; R-Lane DFE_ADAPT_EN
	ScalerTimer_DelayXms(1);
	_phy_rtd_part_outl(0x1800db88, 4, 4, 0x1);
	_phy_rtd_part_outl(0x1800db88, 12, 12, 0x1);
	_phy_rtd_part_outl(0x1800db88, 20, 20, 0x1);
	_phy_rtd_part_outl(0x1800db88, 28, 28, 0x1);
	//$finish;
}

#endif

////////////////////////////////////////////



typedef struct {
	// factory mode
	unsigned char eq_mode;  // 0: auto adaptive  1: manual mode
	unsigned char manual_eq[4];
}HDMIRX_PHY_FACTORY_T;


#if BIST_PHY_SCAN
typedef enum {
	PHY_INIT_SCAN,
	PHY_ICP_SCAN,
	PHY_KP_SCAN,
	PHY_END_SCAN,
}HDMIRX_PHY_SCAN_T;
#endif


typedef enum {
	EQ_MANUAL_OFF,
	EQ_MANUAL_ON,
}HDMIRX_PHY_EQ_MODE;


/**********************************************************************************************
*
*	Variables
*
**********************************************************************************************/
extern unsigned int clock_bound_3g;
extern unsigned int clock_bound_1p5g;
extern unsigned int clock_bound_110m;
extern unsigned int clock_bound_45m;


extern HDMIRX_PHY_STRUCT_T phy_st[HDMI_PORT_TOTAL_NUM];

extern unsigned int phy_isr_en[HDMI_PORT_TOTAL_NUM];

extern HDMIRX_PHY_FACTORY_T phy_fac_eq_st[HDMI_PORT_TOTAL_NUM];

extern char m_disparity_rst_handle_once;
extern char m_scdc_rst_handle_once;

#if BIST_PHY_SCAN
extern HDMIRX_PHY_SCAN_T  phy_scan_state;
#endif


/**********************************************************************************************
*
*	Funtion Declarations
*
**********************************************************************************************/


unsigned char newbase_hdmi21_set_phy(unsigned char port, unsigned int b, unsigned char frl_mode,unsigned char lane);



//dfe
void newbase_hdmi21_dump_dfe_para(unsigned char nport, unsigned char lane_mode);
void lib_hdmi21_dump_dfe_tap0_le(unsigned char nport,unsigned char lane_mode);
unsigned char newbase_hdmi21_tmds_dfe_record(unsigned char nport, unsigned int clk);
unsigned char newbase_hdmi21_tmds_dfe_close(unsigned char nport, unsigned int clk, unsigned char lane_mode, unsigned char phy_state);
unsigned char newbase_hdmi21_frl_dfe_close(unsigned char nport, unsigned int clk, unsigned char lane_mode,unsigned char phy_state);
void newbase_hdmi21_tmds_dfe_hi_speed(unsigned char nport, unsigned int b_clk, unsigned char lane_mode);
void newbase_hdmi21_tmds_dfe_6g_long_cable_patch(unsigned char nport);
//extern unsigned char newbase_hdmi_dfe_hi_speed_close(unsigned char nport);
void newbase_hdmi21_tmds_dfe_mi_speed(unsigned char nport, unsigned int b_clk, unsigned char lane_mode);
void newbase_hdmi21_tmds_dfe_mid_adapthve(unsigned char nport, unsigned int b_clk);

//Z0
void lib_hdmi21_z0_calibration(void);
void lib_hdmi21_z300_sel(unsigned char nport, unsigned char mode);  // 0: active mode 1:vcm mode



//eq setting
#ifdef CONFIG_POWER_SAVING_MODE
void lib_hdmi21_ac_couple_power_en(unsigned char nport,unsigned char en);
void lib_hdmi21_eq_pi_power_en(unsigned char nport,unsigned char en, unsigned char frl_mode,unsigned char lane_mode);
#endif

		
//dfe setting

void lib_hdmi21_dfe_power(unsigned char nport, unsigned char en, unsigned char lane_mode);



void lib_hdmi21_dfe_config_setting(unsigned char nport,unsigned char lane_mode);



//PLL/DEMUX/CDR
void lib_hdmi21_cmu_pll_en(unsigned char nport, unsigned char enable,unsigned char frl_mode);
void lib_hdmi21_demux_ck_vcopll_rst(unsigned char nport, unsigned char rst, unsigned char lane_mode);
void lib_hdmi21_cdr_rst(unsigned char nport, unsigned char rst,unsigned char frl_mode,unsigned char lane_mode);

//Watch Dog
void lib_hdmi21_wdog(unsigned char nport,unsigned char frl_mode,unsigned char lane_mode);
void lib_hdmi21_cmu_rst(unsigned char port, unsigned char rst);

#if 1


void lib_hdmi21_single_end(unsigned char nport, unsigned char channel, unsigned char p_off, unsigned char n_off);


//acdr
void lib_hdmi21_dfe_param_init(unsigned char nport, unsigned int b, unsigned char dfe_mode, unsigned char lane_mode);


//high measure clk detect
int lib_clk_lane_transition_detect(unsigned char nport);
#endif

int newbase_hdmi21_rxphy_get_frl_info(unsigned char port, unsigned char frl_mode);
void newbase_hdmi21_rxphy_set_frl_info(unsigned char port,unsigned char frl_mode);


/**********************************************************************************************
*
*	Function Body
*
**********************************************************************************************/


unsigned char newbase_rxphy_frl_measure(unsigned char port)
{
	//if (port >= HD20_PORT)
		//return 0;

	if (phy_st[port].frl_mode != phy_st[port].pre_frl_mode) {
		HDMI_EMG("frl mdoe change %d -> %d", phy_st[port].pre_frl_mode,phy_st[port].frl_mode);
		newbase_hdmi21_rxphy_set_frl_info(port,phy_st[port].frl_mode);
		phy_st[port].pre_frl_mode = phy_st[port].frl_mode;
		phy_st[port].clk_pre = phy_st[port].clk;
		phy_st[port].clk_debounce_count= 0;
		phy_st[port].clk_unstable_count = 0;
		phy_st[port].clk_stable = 0;
		phy_st[port].phy_proc_state = PHY_PROC_INIT;
		SET_HDMI_DETECT_EVENT(port, HDMI_DETECT_FRL_MODE_CHANGE);
	}

	return 1;
	
}

void newbase_hdmi21_rxphy_set_frl_info(unsigned char port,unsigned char frl_mode)
{
	switch(frl_mode)
	{
	case 0:
		phy_st[port].lane_num = HDMI_3LANE;
		phy_st[port].frl_mode = MODE_TMDS;
		break;
	case 1:
		phy_st[port].lane_num = HDMI_3LANE;
		phy_st[port].frl_mode = MODE_FRL_3G_3_LANE;
		phy_st[port].clk = FRL_3G_B;
		break;
	case 2:
		phy_st[port].lane_num = HDMI_3LANE;
		phy_st[port].frl_mode = MODE_FRL_6G_3_LANE;
		phy_st[port].clk = FRL_6G_B;
		break;
	case 3:   // FRL:6G/4Ln
		phy_st[port].lane_num = HDMI_4LANE;
		phy_st[port].frl_mode = MODE_FRL_6G_4_LANE;
		phy_st[port].clk = FRL_6G_B;
		break;
	case 4:   // FRL:8G/4Ln
		phy_st[port].lane_num = HDMI_4LANE;
		phy_st[port].frl_mode = MODE_FRL_8G_4_LANE;
		phy_st[port].clk = FRL_8G_B;
		break;
	case 5:   // FRL:10G/4Ln
		phy_st[port].lane_num = HDMI_4LANE;
		phy_st[port].frl_mode = MODE_FRL_10G_4_LANE;
		phy_st[port].clk = FRL_10G_B;
		break;
	case 6:   // FRL:12G/4Ln
		phy_st[port].lane_num = HDMI_4LANE;
		phy_st[port].frl_mode = MODE_FRL_12G_4_LANE;
		phy_st[port].clk = FRL_12G_B;
		break;
	default:   // FRL:6G/4Ln
		phy_st[port].lane_num = HDMI_4LANE;
		phy_st[port].frl_mode = MODE_FRL_6G_4_LANE;
		phy_st[port].clk = FRL_6G_B;
		break;
	}

	phy_st[port].pre_frl_mode = phy_st[port].frl_mode;;
}

unsigned char newbase_hdmi_scdc_get_frl_mode(unsigned char port)
{
	unsigned char frl_mode = MODE_TMDS;
	if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_LINK_TRAINING_BYPASS ) > 0)
	{//bypass link training, force frl mode
	    frl_mode = (unsigned char)GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_LINK_TRAINING_BYPASS );
	}
	else
	{// TX polling FLT Ready and then send change FRL rate
		if (lib_hdmi_scdc_read(port,  SCDC_STATUS_FLAGS) & SCDC_STATUS_FLT_READY)
		{
			frl_mode = (lib_hdmi_scdc_get_sink_config_31(port) & SCDC_FRL_RATE);
		} else {
			frl_mode = MODE_TMDS;
		}
	}
	return frl_mode;
	
}

unsigned char newbase_rxphy_get_frl_mode(unsigned char port)
{
	return phy_st[port].frl_mode;
}

void newbase_rxphy_set_frl_mode(unsigned char port, unsigned char frl_mode)
{

	if (frl_mode == MODE_TMDS)
		phy_st[port].pre_frl_mode = MODE_TMDS;  //for update frl clock if change from tmds to frl
	
	phy_st[port].frl_mode = frl_mode;

//	phy_st[port].lane_num = HDMI_4LANE;
}

void newbase_hdmi_frl_set_phy(unsigned char port, unsigned char frl_mode)
{
	newbase_rxphy_set_frl_mode(port,frl_mode);
    	
       HDMI_EMG("[link training] frl mdoe change %d -> %d", phy_st[port].pre_frl_mode,phy_st[port].frl_mode);
	newbase_hdmi21_rxphy_set_frl_info(port,phy_st[port].frl_mode);
	phy_st[port].pre_frl_mode = phy_st[port].frl_mode;
	phy_st[port].clk_pre = phy_st[port].clk;
	phy_st[port].clk_debounce_count= 0;
	phy_st[port].clk_unstable_count = 0;
	phy_st[port].clk_stable = 0;
	phy_st[port].phy_proc_state = PHY_PROC_INIT;
	SET_HDMI_DETECT_EVENT(port, HDMI_DETECT_PHY_RESET);
	
    		newbase_rxphy_frl_job(port);

}



static unsigned char m_wrong_direction_cnt[4][4];
static unsigned char enable_adj_tap1[4][4];
static unsigned char adj_tap1_done[4][4];
static unsigned char adj_le_done[4][4];
static unsigned char adj_parameter_changed[4][4];
static unsigned char acdr_finetune_en_toggle_once[4][4];;
static unsigned int ber_sum[4];
static unsigned int ber_sum_cnt[4];
unsigned int scdc_extend[4];
void newbase_hdmi_init_disparity_ltp_var(unsigned char nport)
{
	//disp var
	phy_st[nport].disp_max_cnt = 0;
	phy_st[nport].disp_start = 0;
	phy_st[nport].tap1_adp_step[0] = 1;
	phy_st[nport].tap1_adp_step[1] = 1;
	phy_st[nport].tap1_adp_step[2] = 1;
	phy_st[nport].tap1_adp_step[3] = 1;

	phy_st[nport].tap2_adp_step[0] = 1;
	phy_st[nport].tap2_adp_step[1] = 1;
	phy_st[nport].tap2_adp_step[2] = 1;
	phy_st[nport].tap2_adp_step[3] = 1;
	
	phy_st[nport].le_adp_step[0] = 1;
	phy_st[nport].le_adp_step[1] = 1;
	phy_st[nport].le_adp_step[2] = 1;
	phy_st[nport].le_adp_step[3] = 1;
	//phy_st[nport].wrong_direction_cnt[0] = -3;
	//phy_st[nport].wrong_direction_cnt[1] = -3;
	//phy_st[nport].wrong_direction_cnt[2] = -3;
	//phy_st[nport].wrong_direction_cnt[3] = -3;

	m_wrong_direction_cnt[nport][0] = 0;
	m_wrong_direction_cnt[nport][1] = 0;
	m_wrong_direction_cnt[nport][2] = 0;
	m_wrong_direction_cnt[nport][3] = 0;

	adj_tap1_done[nport][0] = 0;
	adj_tap1_done[nport][1] = 0;
	adj_tap1_done[nport][2] = 0;
	adj_tap1_done[nport][3] = 0;

	adj_le_done[nport][0] = 0;
	adj_le_done[nport][1] = 0;
	adj_le_done[nport][2] = 0;
	adj_le_done[nport][3] = 0;

	adj_parameter_changed[nport][0] = 1;
	adj_parameter_changed[nport][1] = 1;
	adj_parameter_changed[nport][2] = 1;
	adj_parameter_changed[nport][3] = 1;
	
	enable_adj_tap1[nport][0] = 0;
	enable_adj_tap1[nport][1] = 0;
	enable_adj_tap1[nport][2] = 0;
	enable_adj_tap1[nport][3] = 0;
	ber_sum[nport]=0;
	ber_sum_cnt[nport]=0;
	scdc_extend[nport]=0;

	acdr_finetune_en_toggle_once[nport][0] = 0;
	acdr_finetune_en_toggle_once[nport][1] = 0;
	acdr_finetune_en_toggle_once[nport][2] = 0;
	acdr_finetune_en_toggle_once[nport][3] = 0;


	//phy_st[nport].disp_err_pre[0] = phy_st[nport].disp_err_pre[1] = phy_st[nport].disp_err_pre[2]  = phy_st[nport].disp_err_pre[3] = 0;
	//phy_st[nport].disp_err_after[0] = phy_st[nport].disp_err_after[1] = phy_st[nport].disp_err_after[2] = phy_st[nport].disp_err_after[3] = 0;
	phy_st[nport].disp_adp_stage[0] = phy_st[nport].disp_adp_stage[1] = phy_st[nport].disp_adp_stage[2] =phy_st[nport].disp_adp_stage[3] = 0;
	//phy_st[nport].disp_err_diff1[0] = phy_st[nport].disp_err_diff1[1]= phy_st[nport].disp_err_diff1[2]= phy_st[nport].disp_err_diff1[3] = 0;
	//phy_st[nport].disp_err_diff2[0] = phy_st[nport].disp_err_diff2[1]= phy_st[nport].disp_err_diff2[2]= phy_st[nport].disp_err_diff2[3] = 0;
	phy_st[nport].disp_err_zero_cnt[0] = phy_st[nport].disp_err_zero_cnt[1]= phy_st[nport].disp_err_zero_cnt[2]= phy_st[nport].disp_err_zero_cnt[3] = 0;
	phy_st[nport].disp_BER_zero_cnt[0] = phy_st[nport].disp_BER_zero_cnt[1]= phy_st[nport].disp_BER_zero_cnt[2]= phy_st[nport].disp_BER_zero_cnt[3] = 0;
	
	//phy_st[nport].disp_err_t[0] = phy_st[nport].disp_err_t[1]= phy_st[nport].disp_err_t[2]= phy_st[nport].disp_err_t[3] = 0;
	//phy_st[nport].err_occur_t1[0] = phy_st[nport].err_occur_t1[1]= phy_st[nport].err_occur_t1[2]= phy_st[nport].err_occur_t1[3] = 0;
	//phy_st[nport].err_occur_t2[0] = phy_st[nport].err_occur_t2[1]= phy_st[nport].err_occur_t2[2]= phy_st[nport].err_occur_t2[3] = 0;
	//phy_st[nport].err_occur_t3[0] = phy_st[nport].err_occur_t3[1]= phy_st[nport].err_occur_t3[2]= phy_st[nport].err_occur_t3[3] = 0;
	phy_st[nport].disp_timer_cnt[0] = phy_st[nport].disp_timer_cnt[1]= phy_st[nport].disp_timer_cnt[2]= phy_st[nport].disp_timer_cnt[3] = 5; 
	
#if PHY_ADAPTIVE_BY_LTP_PAT_AFTER_LTP_PASS
	//ltp var
	phy_st[nport].ltp_err_pre[0] = phy_st[nport].ltp_err_pre[1] = phy_st[nport].ltp_err_pre[2]  = phy_st[nport].ltp_err_pre[3] = 0;
	phy_st[nport].ltp_err_zero_cnt[0] = phy_st[nport].ltp_err_zero_cnt[1] =phy_st[nport].ltp_err_zero_cnt[2] =phy_st[nport].ltp_err_zero_cnt[3] = 0;
	phy_st[nport].ltp_err_start[0] = phy_st[nport].ltp_err_start[1] = phy_st[nport].ltp_err_start[2] =phy_st[nport].ltp_err_start[3] = 0;
	phy_st[nport].ltp_err_end [0] =  phy_st[nport].ltp_err_end [1] = phy_st[nport].ltp_err_end [2]  = phy_st[nport].ltp_err_end [3] = 0;
#endif

}

//bool enable_adj_tap1=FALSE;
//unsigned char acdr_finetune_en_toggle_once = FALSE;
//dfe adaptive according disparity error , this algorithm is by garran
static unsigned int  TimeStamp0[4][4];
static unsigned int  TimeStamp1[4][4];

extern FRL_LT_FSM_STATUS_T lt_fsm_status[HDMI_PORT_TOTAL_NUM];
void lib_hdmi21_phy_error_handling(unsigned char nport)
{
	unsigned int current_time_ms = hdmi_get_system_time_ms();
	#define TAP1_MAX_VALUE	31
	#define TAP2_MAX_VALUE	31
	#define LE_MAX_VALUE	31
	unsigned char align_status;
	int lane = 0;
	unsigned int  TimeStamp2;
	//unsigned int BER2_TH=500;	//Clayton, Add TH for change tuning direction
	//bool enable_adj_le=FALSE;

	if ((newbase_hdmi_get_power_saving_state(nport)==PS_FSM_POWER_SAVING_ON))     // do not do power saving when HDMI is power off
		return ;

	if (phy_st[nport].lane_num == HDMI_4LANE)
		align_status = (hdmi_in(HD21_channel_align_c_reg)  & 0x1) & (hdmi_in(HD21_channel_align_r_reg)  & 0x1) & ( hdmi_in(HD21_channel_align_g_reg) & 0x1) & (hdmi_in(HD21_channel_align_b_reg) & 0x1);
	else
		align_status = (hdmi_in(HD21_channel_align_r_reg)  & 0x1) & ( hdmi_in(HD21_channel_align_g_reg) & 0x1) & (hdmi_in(HD21_channel_align_b_reg) & 0x1);

	if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD21_PHY_LOG_ENABLE ))
		HDMI_PRINTF("[disp init]disp_start=%d, Current Time(ms)=%d, align_status=%d\n",phy_st[nport].disp_start, current_time_ms, align_status);			

	if (align_status) {

		if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD21_PHY_LOG_ENABLE )) //for phy log debugging
		{
			newbase_hdmi21_dump_dfe_para(nport,phy_st[nport].lane_num);
		}

		
		if (phy_st[nport].disp_start == 0) 
		{
			TimeStamp0[nport][0]=hdmi_get_system_time_ms();//Record T0
			TimeStamp0[nport][1]=hdmi_get_system_time_ms();//Record T0
			TimeStamp0[nport][2]=hdmi_get_system_time_ms();//Record T0
			TimeStamp0[nport][3]=hdmi_get_system_time_ms();//Record T0
			
			lib_hdmi_hd21_fw_disparity_error_status(nport,phy_st[nport].disp_err0);//Record error cnt
			phy_st[nport].disp_start = 1;
			phy_st[nport].disp_adp_stage[0]  = phy_st[nport].disp_adp_stage[1]  = phy_st[nport].disp_adp_stage[2] = phy_st[nport].disp_adp_stage[3]  = 1;
			phy_st[nport].disp_timer_cnt[0] = phy_st[nport].disp_timer_cnt[1]= phy_st[nport].disp_timer_cnt[2]= phy_st[nport].disp_timer_cnt[3] = 5;  //100ms
		}
		else if (phy_st[nport].disp_start == 1)
		{

			if (phy_st[nport].disp_timer_cnt[lane] == 0)
			{  //near 100ms
				TimeStamp1[nport][0]=hdmi_get_system_time_ms();//Record T0
				TimeStamp1[nport][1]=hdmi_get_system_time_ms();//Record T0
				TimeStamp1[nport][2]=hdmi_get_system_time_ms();//Record T0
				TimeStamp1[nport][3]=hdmi_get_system_time_ms();//Record T0
				lib_hdmi_hd21_fw_disparity_error_status(nport,phy_st[nport].disp_err1);
				phy_st[nport].disp_start = 2;
				phy_st[nport].disp_adp_stage[0]  = phy_st[nport].disp_adp_stage[1]  = phy_st[nport].disp_adp_stage[2] = phy_st[nport].disp_adp_stage[3]  = 1;
				phy_st[nport].disp_timer_cnt[0] = phy_st[nport].disp_timer_cnt[1]= phy_st[nport].disp_timer_cnt[2]= phy_st[nport].disp_timer_cnt[3] = 5;  //100ms
			}
			else 
			{
				phy_st[nport].disp_timer_cnt[lane]--;
			}
	
		}
		else if (phy_st[nport].disp_start == 2)
		{
			if (phy_st[nport].disp_timer_cnt[lane] == 0) 
			{  //near 100ms
				TimeStamp2=hdmi_get_system_time_ms();
				lib_hdmi_hd21_fw_disparity_error_status(nport,phy_st[nport].disp_err2);
				for (lane = 0; lane < phy_st[nport].lane_num ; lane++) 
				{
					if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD21_PHY_LOG_ENABLE ))
					{
						HDMI_PRINTF("[disp ing]lane=%d, disp_adp_stage[lane]=%d, m_wrong_direction_cnt=%d, disp_err_zero_cnt[lane]=%d \n",lane,
						phy_st[nport].disp_adp_stage[lane], m_wrong_direction_cnt[nport][lane], phy_st[nport].disp_err_zero_cnt[lane] );
					}


					phy_st[nport].disp_BER1[lane]= (phy_st[nport].disp_err1[lane] - phy_st[nport].disp_err0[lane])*10000/(TimeStamp1[nport][lane]-TimeStamp0[nport][lane]);
					phy_st[nport].disp_BER2_temp[lane]= (phy_st[nport].disp_err2[lane] - phy_st[nport].disp_err1[lane])*10000/(TimeStamp2-TimeStamp1[nport][lane]);
					phy_st[nport].disp_BER2[lane]=phy_st[nport].disp_BER2_temp[lane];

					if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD21_PHY_LOG_ENABLE ))
					{
						HDMI_PRINTF("[disp adp]lane=%d, disp_BER1=%d, disp_BER2_temp=%d, disp_err2=%d, disp_err1=%d, disp_err0=%d,TimeStamp2=%d, TimeStamp1=%d, TimeStamp0=%d\n", 
						lane, 
						phy_st[nport].disp_BER1[lane],
						phy_st[nport].disp_BER2_temp[lane],
						phy_st[nport].disp_err2[lane],
						phy_st[nport].disp_err1[lane],
						phy_st[nport].disp_err0[lane],
						TimeStamp2,
						TimeStamp1[nport][lane],
						TimeStamp0[nport][lane]);
					}
					//if((GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1 , HDMI_FLOW_CFG2_CTS_EXTEND_MODE) == TRUE))
					if((lt_fsm_status[nport].tx_flt_no_train == TRUE)&&(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1 , HDMI_FLOW_CFG2_CTS_EXTEND_MODE) == TRUE))
					{

						if(m_scdc_rst_handle_once && (scdc_extend[nport]==FALSE))// 850ms flag ==TRUE
						{
							if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD21_PHY_LOG_ENABLE ))
							{
								HDMI_EMG("[disp adp stage1] m_scdc_rst_handle_once= %d  disp_err_zero_cnt=%d  port[%d]lane:%d\n",m_scdc_rst_handle_once,phy_st[nport].disp_err_zero_cnt[lane],nport,lane);
							}
							if(phy_st[nport].disp_BER2[lane] > 0)
							{
								HDMI_EMG("Start Extend tuning process: @ port[%d]lane:%d  disp_BER2=%d \n",nport,lane,phy_st[nport].disp_BER2[lane]);
								// Call MAC_API1 Here;
								newbase_hdmi_restart_scdc_extend(nport);
								scdc_extend[nport]=TRUE;
							}

						}	
					}
					//Check for Stop tuning condition
					if (phy_st[nport].disp_err_zero_cnt[lane] >=10000) 
					{
						//phy_st[nport].disp_adp_stage[lane] = 2;//end Marked for keep on tuing  never stop 
						phy_st[nport].disp_err_zero_cnt[lane] = 10000;//Set to Maxmum error cnt protect
						if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD21_PHY_LOG_ENABLE ))
						{
							HDMI_PRINTF("[disp adp stage1] lane=%d, disp_err_zero_cnt %d over(tap1=%d)\n",lane,phy_st[nport].disp_err_zero_cnt[lane] , phy_st[nport].dfe_t[lane].tap1);
						}
					}


					if(phy_st[nport].disp_BER2[lane] < 300)
					{
						phy_st[nport].disp_BER_zero_cnt[lane] = phy_st[nport].disp_BER_zero_cnt[lane] +1;
						
						if(phy_st[nport].disp_BER_zero_cnt[lane]>30)
						{
							if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD21_PHY_LOG_ENABLE ))
							{
								HDMI_PRINTF("[disp stage1] le < 300 le& tap2 adj - Inverse step=%d (nport=%d, lane=%d le=%d, tap2=%d, m_wrong_direction_cnt=%d)\n",phy_st[nport].le_adp_step[lane], nport, lane,phy_st[nport].dfe_t[lane].le, phy_st[nport].dfe_t[lane].tap2,m_wrong_direction_cnt[nport][lane]);
							}
							if(adj_le_done[nport][lane]==FALSE)
							{
								adj_le_done[nport][lane]=TRUE;
								enable_adj_tap1[nport][lane]= TRUE;
								m_wrong_direction_cnt[nport][lane]=0;
							}
							phy_st[nport].disp_BER_zero_cnt[lane] = 0;

						}
					}
					else
					{
						phy_st[nport].disp_BER_zero_cnt[lane] = 0;
					}


					//Dummy now for No tap1_done case
					if((adj_le_done[nport][lane]==TRUE)&&(adj_tap1_done[nport][lane]==TRUE))
					{
						phy_st[nport].disp_adp_stage[lane] = 2;//end
						phy_st[nport].disp_err_zero_cnt[lane] = 0;
						HDMI_PRINTF("[disp End]ADP Done  lane=%d, disp_err_zero_cnt %d over(tap1=%d)\n",lane,phy_st[nport].disp_err_zero_cnt[lane] , phy_st[nport].dfe_t[lane].tap1);
					}

	
					if (phy_st[nport].disp_adp_stage[lane] == 2) 
					{  //end
						HDMI_PRINTF("[disp End] (lane=%d tap1=%d) tap2=%d)\n",lane,phy_st[nport].dfe_t[lane].tap1, phy_st[nport].dfe_t[lane].tap2);
						continue;
					}
					else if (phy_st[nport].disp_adp_stage[lane] == 1)
					{
						if(((phy_st[nport].disp_BER1[lane]>100000))&&(acdr_finetune_en_toggle_once[nport][lane]==FALSE))
						{
							HDMI_EMG("Too much error @ port[%d]lane:%d\n",nport,lane);
							HDMI_EMG("hdmi21_set_phy again \n");
							newbase_hdmi21_set_phy(PHY_PROC_INIT, phy_st[PHY_PROC_INIT].clk,phy_st[PHY_PROC_INIT].frl_mode,phy_st[PHY_PROC_INIT].lane_num);
							newbase_hdmi_reset_meas_counter(PHY_PROC_INIT);

							acdr_finetune_en_toggle_once[nport][lane]=TRUE;
						}
						else
						{
							acdr_finetune_en_toggle_once[nport][lane]=TRUE;//No need toggle in this adj

						}
						
						//if((phy_st[nport].disp_err2[lane] - phy_st[nport].disp_err1[lane]) <= 1)	//Clayton: orig: err2=err1 / new: err2-err1 <= 1, 1is acceptable
						if((phy_st[nport].disp_err2[lane] - phy_st[nport].disp_err1[lane]) == 0)	
						{
							if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD21_PHY_LOG_ENABLE ))
							{
								HDMI_PRINTF("Almost no error increased , error @ port[%d]lane:%d, err2= %d, err1= %d\n",nport,lane, phy_st[nport].disp_err2[lane], phy_st[nport].disp_err1[lane]);
							}
							phy_st[nport].disp_err_zero_cnt[lane] = phy_st[nport].disp_err_zero_cnt[lane] + 1;					


							phy_st[nport].disp_start = 2;

						}
						else
						{
							if ((phy_st[nport].disp_BER2[lane]<= phy_st[nport].disp_BER1[lane])||(adj_parameter_changed[nport][lane]== TRUE))
							{
								if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD21_PHY_LOG_ENABLE ))
								{
									HDMI_PRINTF("[disp stage1]  lane=%d, disp_err_zero_cnt %d  disp_BER2[lane]=%d)\n",lane,phy_st[nport].disp_err_zero_cnt[lane] , phy_st[nport].disp_BER2[lane]);
								}
								if (adj_parameter_changed[nport][lane]== TRUE)
								{
									adj_parameter_changed[nport][lane]= FALSE;
								}

								phy_st[nport].disp_err_zero_cnt[lane]=0; //Clayton

								if((enable_adj_tap1[nport][lane]==TRUE) || (adj_le_done[nport][lane]==TRUE)) 
								{
									
									phy_st[nport].dfe_t[lane].tap1 = phy_st[nport].dfe_t[lane].tap1 +2* phy_st[nport].tap1_adp_step[lane];
									if(phy_st[nport].dfe_t[lane].tap1<(-31))
									{
										phy_st[nport].dfe_t[lane].tap1 = (-31);
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										if(adj_le_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= FALSE;//change  to LE adj
											adj_parameter_changed[nport][lane]= TRUE;
										}
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
										HDMI_EMG("check tap1 hit boundary, lane = %d, tap1 = %d\n", lane, phy_st[nport].dfe_t[lane].tap1);			
									}
									else if(phy_st[nport].dfe_t[lane].tap1>TAP1_MAX_VALUE)
									{
										phy_st[nport].dfe_t[lane].tap1 = TAP1_MAX_VALUE;
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										if(adj_le_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= FALSE;//change  to LE adj
											adj_parameter_changed[nport][lane]= TRUE;
										}
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
										HDMI_EMG("check tap1 hit boundary, lane = %d, tap1 = %d\n", lane, phy_st[nport].dfe_t[lane].tap1);			
									}
									if(adj_tap1_done[nport][lane]==FALSE)
									{
										lib_hdmi_dfe_init_tap1(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap1);
										HDMI_PRINTF("[disp stage1] tap1 adj (lane=%d tap1=%d)\n",lane,phy_st[nport].dfe_t[lane].tap1);

									}


								}
								else //adj le
								{
								
									phy_st[nport].dfe_t[lane].le = phy_st[nport].dfe_t[lane].le + phy_st[nport].le_adp_step[lane];

									if(phy_st[nport].dfe_t[lane].le<0)
									{
										phy_st[nport].dfe_t[lane].le = 0;
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										if(adj_tap1_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
											adj_parameter_changed[nport][lane]= TRUE;
										}
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
#if !HDMI_FRL_TAP0234_ADP_EN
										phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
#else 

#endif
										HDMI_EMG("check le hit boundary, lane = %d, le = %d\n", lane, phy_st[nport].dfe_t[lane].le);			
									}
									else if(phy_st[nport].dfe_t[lane].le>LE_MAX_VALUE)
									{

										phy_st[nport].dfe_t[lane].le = LE_MAX_VALUE;
										if(adj_tap1_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
											adj_parameter_changed[nport][lane]= TRUE;
										}
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
#if !HDMI_FRL_TAP0234_ADP_EN										
										phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
#else 

#endif										
										HDMI_EMG(" check le hit boundary, lane = %d, le = %d\n", lane, phy_st[nport].dfe_t[lane].le);			
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
									}
									else    //Change Tap2 value only when LE not in boundary condition
									{
#if !HDMI_FRL_TAP0234_ADP_EN
										phy_st[nport].dfe_t[lane].tap2 = phy_st[nport].dfe_t[lane].tap2 +2* phy_st[nport].tap2_adp_step[lane];
										if(phy_st[nport].dfe_t[lane].tap2<-31)
										{
											phy_st[nport].dfe_t[lane].tap2 = -31;
											//phy_st[nport].disp_adp_stage[lane] = 3;  // end
											//enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
											HDMI_EMG("check le hit boundary, lane = %d, tap2= %d\n", lane, phy_st[nport].dfe_t[lane].tap2);			
										}
										else if(phy_st[nport].dfe_t[lane].tap2>31)
										{
											phy_st[nport].dfe_t[lane].tap2 = 31;
											HDMI_EMG("check le hit boundary, lane = %d, tap2 = %d\n", lane, phy_st[nport].dfe_t[lane].tap2);			
											//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										}
#else 

#endif
									}

									if(adj_le_done[nport][lane]==FALSE)
									{
										lib_hdmi_dfe_init_le(nport,(1<<lane),phy_st[nport].dfe_t[lane].le);
#if !HDMI_FRL_TAP0234_ADP_EN
										lib_hdmi_dfe_init_tap2(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap2);										
#else 

#endif								
										if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD21_PHY_LOG_ENABLE ))
										{
											HDMI_PRINTF("[disp stage1] le & tap2 adj (lane=%d le=%d tap2=%d)\n",lane,phy_st[nport].dfe_t[lane].le,phy_st[nport].dfe_t[lane].tap2);
										}
									}

								}
								
								phy_st[nport].disp_start = 2;
								phy_st[nport].disp_err0[lane]=phy_st[nport].disp_err1[lane];
								phy_st[nport].disp_err1[lane]=phy_st[nport].disp_err2[lane];
								TimeStamp0[nport][lane]=TimeStamp1[nport][lane];
								TimeStamp1[nport][lane]=TimeStamp2;

							}
							else if (phy_st[nport].disp_BER2[lane]>phy_st[nport].disp_BER1[lane])
							{ 
								phy_st[nport].disp_err_zero_cnt[lane]=0;	//Clayton

								if((enable_adj_tap1[nport][lane]==TRUE) || (adj_le_done[nport][lane]==TRUE)) //clayton
								{
									phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
									phy_st[nport].dfe_t[lane].tap1 = phy_st[nport].dfe_t[lane].tap1 + 2*phy_st[nport].tap1_adp_step[lane];
									m_wrong_direction_cnt[nport][lane] ++;
									if(m_wrong_direction_cnt[nport][lane]>1 )
									{
										m_wrong_direction_cnt[nport][lane]=0;
										enable_adj_tap1[nport][lane]= FALSE;//change  to LE adj
										adj_parameter_changed[nport][lane]= TRUE;
									}
									
									if(phy_st[nport].dfe_t[lane].tap1<(-31))
									{
										phy_st[nport].dfe_t[lane].tap1 = (-31);
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										if(adj_le_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= FALSE;//change  to LE adj
											adj_parameter_changed[nport][lane]= TRUE;
										}
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
										HDMI_EMG("check tap1 hit boundary, lane = %d, tap1 = %d\n", lane, phy_st[nport].dfe_t[lane].tap1);			
									}
									else if(phy_st[nport].dfe_t[lane].tap1>TAP1_MAX_VALUE)
									{
										phy_st[nport].dfe_t[lane].tap1 = TAP1_MAX_VALUE;
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										if(adj_le_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= FALSE;//change  to LE adj
											adj_parameter_changed[nport][lane]= TRUE;
										}
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
										HDMI_EMG("check tap1 hit boundary, lane = %d, tap1 = %d\n", lane, phy_st[nport].dfe_t[lane].tap1);			
									}
									if(adj_tap1_done[nport][lane]==FALSE)
									{
										lib_hdmi_dfe_init_tap1(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap1);
										if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD21_PHY_LOG_ENABLE ))
										{
											HDMI_PRINTF("[disp stage1] tap1 adj - Inverse step=%d (nport=%d, lane=%d tap1=%d, m_wrong_direction_cnt=%d)\n",phy_st[nport].tap1_adp_step[lane], nport, lane,phy_st[nport].dfe_t[lane].tap1,  m_wrong_direction_cnt[nport][lane]);
										}

										if ((phy_st[nport].disp_BER2[lane] <25) )
										{
											//adj_tap1_done[nport][lane]=TRUE;
											HDMI_PRINTF("[disp stage1] disp_BER2<25, tap1 adj ctn (lane=%d le=%d)\n",lane,phy_st[nport].dfe_t[lane].tap1);
											//enable_adj_tap1[nport][lane]= FALSE;
											//m_wrong_direction_cnt[nport][lane]=0;
										}
									}

								}
								else //adj le
								{
									phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
#if !HDMI_FRL_TAP0234_ADP_EN									
									phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
#else 

#endif								
									phy_st[nport].dfe_t[lane].le = phy_st[nport].dfe_t[lane].le + phy_st[nport].le_adp_step[lane];

									m_wrong_direction_cnt[nport][lane] ++;
									if(m_wrong_direction_cnt[nport][lane]>1 )
									{
										m_wrong_direction_cnt[nport][lane]=0;
										enable_adj_tap1[nport][lane]= TRUE;//change  to TPA1 adj
										adj_parameter_changed[nport][lane]= TRUE;
									}

									if(phy_st[nport].dfe_t[lane].le<0)
									{
										phy_st[nport].dfe_t[lane].le = 0;
										if(adj_tap1_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
											adj_parameter_changed[nport][lane]= TRUE;
										}										
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
#if !HDMI_FRL_TAP0234_ADP_EN
										phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
#else

#endif								
										HDMI_EMG("check le hit boundary, lane = %d, le = %d\n", lane, phy_st[nport].dfe_t[lane].le);			
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
									}
									else if(phy_st[nport].dfe_t[lane].le>LE_MAX_VALUE)	//Clayton
									{
										phy_st[nport].dfe_t[lane].le = LE_MAX_VALUE;	//Clayton: fix coding wrong
										m_wrong_direction_cnt[nport][lane]=0;
										if(adj_tap1_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
											adj_parameter_changed[nport][lane]= TRUE;
										}										
										phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
#if !HDMI_FRL_TAP0234_ADP_EN
										phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
#else

#endif								
										HDMI_EMG("check le hit boundary, lane = %d, le = %d\n", lane, phy_st[nport].dfe_t[lane].le);			
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
									}
									else     //Change Tap2 value only when LE not in boundary condition
									{
#if !HDMI_FRL_TAP0234_ADP_EN
										phy_st[nport].dfe_t[lane].tap2 = phy_st[nport].dfe_t[lane].tap2 +2* phy_st[nport].tap2_adp_step[lane];
										if(phy_st[nport].dfe_t[lane].tap2<-31)
										{
											phy_st[nport].dfe_t[lane].tap2 = -31;
											//phy_st[nport].disp_adp_stage[lane] = 3;  // end
											//enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
											HDMI_EMG("check le hit boundary, lane = %d, tap2 = %d\n", lane, phy_st[nport].dfe_t[lane].tap2);			
										}
										else if(phy_st[nport].dfe_t[lane].tap2>31)
										{
											//enable_adj_tap1[nport][lane]= TRUE;	//Clayton: fix coding wrong
											phy_st[nport].dfe_t[lane].tap2 = 31;
											HDMI_EMG("check le hit boundary, lane = %d, tap2 = %d\n", lane, phy_st[nport].dfe_t[lane].tap2);			
											//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										}
#else 

#endif								
									}

									if(adj_le_done[nport][lane]==FALSE)
									{
										lib_hdmi_dfe_init_le(nport,(1<<lane),phy_st[nport].dfe_t[lane].le);
#if !HDMI_FRL_TAP0234_ADP_EN
										lib_hdmi_dfe_init_tap2(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap2);										
#else 

#endif								
										if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD21_PHY_LOG_ENABLE ))
										{
											HDMI_PRINTF("[disp stage1] le& tap2 adj - Inverse step=%d (nport=%d, lane=%d le=%d, tap2=%d, m_wrong_direction_cnt=%d)\n",phy_st[nport].le_adp_step[lane], nport, lane,phy_st[nport].dfe_t[lane].le, phy_st[nport].dfe_t[lane].tap2,m_wrong_direction_cnt[nport][lane]);
										}
									}

								}

								phy_st[nport].disp_start = 2;
								phy_st[nport].disp_err0[lane]=phy_st[nport].disp_err1[lane];
								phy_st[nport].disp_err1[lane]=phy_st[nport].disp_err2[lane];
								TimeStamp0[nport][lane]=TimeStamp1[nport][lane];
								TimeStamp1[nport][lane]=TimeStamp2;
							}


						}

					}

				}

				if((lt_fsm_status[nport].tx_flt_no_train == TRUE)&&(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1 , HDMI_FLOW_CFG2_CTS_EXTEND_MODE) == TRUE))
				{
					if(m_scdc_rst_handle_once && (scdc_extend[nport]==TRUE))// 850ms flag ==TRUE
					{

						if (phy_st[nport].lane_num == HDMI_4LANE)
						{
							ber_sum[nport]= phy_st[nport].disp_BER2[0]+phy_st[nport].disp_BER2[1]+phy_st[nport].disp_BER2[2]+phy_st[nport].disp_BER2[3];
						}
						else
						{
							ber_sum[nport]= phy_st[nport].disp_BER2[0]+phy_st[nport].disp_BER2[1]+phy_st[nport].disp_BER2[2];

						}
						if(ber_sum[nport] <1)
						{
							ber_sum_cnt[nport]=ber_sum_cnt[nport]+1;
							if(ber_sum_cnt[nport]>5)
							{
								HDMI_EMG("End Extend tuning process: @ port[%d] ber_sum_cnt=%d \n",nport, ber_sum_cnt[nport]);
								// Call MAC_API2 Here;
								newbase_hdmi_force_stop_scdc_extend(nport);
								ber_sum_cnt[nport]=0;
								scdc_extend[nport]=FALSE;
							}

						}
						else
						{
							ber_sum_cnt[nport]=0;
						}
						
					}

				}
				if (phy_st[nport].lane_num == HDMI_4LANE)
				{
					if ((phy_st[nport].disp_adp_stage[0] == 2) & (phy_st[nport].disp_adp_stage[1] == 2) & (phy_st[nport].disp_adp_stage[2] == 2) & (phy_st[nport].disp_adp_stage[3] == 2))
					{
						phy_st[nport].disp_start = 3; //ending
						HDMI_EMG("[disp stage2] port=%d, 4 lane done, dfe tap1(0,1,2,3)=(%d,%d,%d,%d)\\n",
							nport, phy_st[nport].dfe_t[0].tap1, phy_st[nport].dfe_t[1].tap1, phy_st[nport].dfe_t[2].tap1, phy_st[nport].dfe_t[3].tap1);
					}
				}
				else
				{
					if ((phy_st[nport].disp_adp_stage[0] == 2) & (phy_st[nport].disp_adp_stage[1] == 2) & (phy_st[nport].disp_adp_stage[2] == 2))  
					{
						phy_st[nport].disp_start = 3; //ending
						HDMI_EMG("[disp stage2] port=%d, 3 lane done, dfe tap1(0,1,2)=(%d,%d,%d)\\n",
							nport, phy_st[nport].dfe_t[0].tap1, phy_st[nport].dfe_t[1].tap1, phy_st[nport].dfe_t[2].tap1);
					}	
				}

			}
			else 
			{
				phy_st[nport].disp_timer_cnt[lane]--;
			}
		
		}
		else if(phy_st[nport].disp_start==3)
		{//End lib_hdmi21_phy_error_handling
			if((lt_fsm_status[nport].tx_flt_no_train == TRUE)&&(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1 , HDMI_FLOW_CFG2_CTS_EXTEND_MODE) == TRUE))
			{//to do 
				newbase_hdmi_scdc_hw_mask_enable(nport, 0);
				HDMI_PRINTF("[disp stage3] port=%d, Adjust finished, close extend\n", nport);
				phy_st[nport].disp_start =4;
			}
		}
		else 
		{
			//phy_st[nport].disp_start == 3 do nothing

		}
	}


}

void lib_hdmi20_phy_error_handling(unsigned char nport)
{
#if 0
	unsigned int current_time_ms = hdmi_get_system_time_ms();
	#define TAP1_MAX_VALUE	31
	#define TAP2_MAX_VALUE	31
	#define LE_MAX_VALUE	30
	int lane = 0;
	unsigned int  TimeStamp2;
	unsigned char mode;
	mode = DFE_P0_REG_DFE_MODE_P0_get_adapt_mode_p0(hdmi_in(DFE_REG_DFE_MODE_reg));
	if ((newbase_hdmi_get_power_saving_state(nport)==PS_FSM_POWER_SAVING_ON))     // do not do power saving when HDMI is power off
		return ;

	for (lane=0; lane< 3; lane++) {
		phy_st[nport].dfe_t[lane].tap1 = lib_hdmi_dfe_get_tap1(nport, lane);
		phy_st[nport].dfe_t[lane].tap2 = lib_hdmi_dfe_get_tap2(nport, lane);
		phy_st[nport].dfe_t[lane].le = lib_hdmi_dfe_get_le(nport, lane);
		phy_st[nport].dfe_t[lane].lemax = lib_hdmi_dfe_get_lemax(nport, lane);
		phy_st[nport].dfe_t[lane].lemin = lib_hdmi_dfe_get_lemin(nport, lane);
	}
	
	if (mode == 2) 
	{
		if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE ))
			HDMI_PRINTF("[disp init]disp_start=%d, Current Time(ms)=%d\n",phy_st[nport].disp_start, current_time_ms);			


		if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
		{
			newbase_hdmi21_dump_dfe_para(nport,phy_st[nport].lane_num);
		}
		
		if (phy_st[nport].disp_start == 0) 
		{
			
			for (lane=0; lane< 3; lane++) {
				phy_st[nport].dfe_t[lane].tap1 = lib_hdmi_dfe_get_tap1(nport, lane);
				phy_st[nport].dfe_t[lane].tap2 = lib_hdmi_dfe_get_tap2(nport, lane);
				phy_st[nport].dfe_t[lane].le = lib_hdmi_dfe_get_le(nport, lane);


				lib_hdmi_dfe_init_le(nport,(1<<lane),phy_st[nport].dfe_t[lane].le);

				phy_st[nport].dfe_t[lane].tap1 = lib_hdmi_dfe_get_tap1(nport, lane);
				phy_st[nport].dfe_t[lane].tap2 = lib_hdmi_dfe_get_tap2(nport, lane);
				phy_st[nport].dfe_t[lane].le = lib_hdmi_dfe_get_le(nport, lane);
				HDMI_PRINTF("[disp init]ADD LE tdisp_start=%d, phy_st[nport].dfe_t[lane].le=%d\n",phy_st[nport].disp_start, phy_st[nport].dfe_t[lane].le);			
			}

			newbase_hdmi_fw_ced_measure(nport, phy_st[nport].ced_err0, 60);

			phy_st[nport].disp_start = 1;
			phy_st[nport].disp_adp_stage[0]  = phy_st[nport].disp_adp_stage[1]  = phy_st[nport].disp_adp_stage[2] = phy_st[nport].disp_adp_stage[3]  = 1;
			phy_st[nport].disp_timer_cnt[0] = phy_st[nport].disp_timer_cnt[1]= phy_st[nport].disp_timer_cnt[2]= phy_st[nport].disp_timer_cnt[3] = 5;  //100ms
		}
		else if (phy_st[nport].disp_start == 1)
		{

			if (phy_st[nport].disp_timer_cnt[lane] == 0)
			{  //near 100ms
				newbase_hdmi_fw_ced_measure(nport, phy_st[nport].ced_err1, 60);

				phy_st[nport].disp_start = 2;
				phy_st[nport].disp_adp_stage[0]  = phy_st[nport].disp_adp_stage[1]  = phy_st[nport].disp_adp_stage[2] = phy_st[nport].disp_adp_stage[3]  = 1;
				phy_st[nport].disp_timer_cnt[0] = phy_st[nport].disp_timer_cnt[1]= phy_st[nport].disp_timer_cnt[2]= phy_st[nport].disp_timer_cnt[3] = 5;  //100ms
			}
			else 
			{
				phy_st[nport].disp_timer_cnt[lane]--;
			}
	
		}
		else if (phy_st[nport].disp_start == 2)
		{
			if (phy_st[nport].disp_timer_cnt[lane] == 0) 
			{  //near 100ms
				TimeStamp2=hdmi_get_system_time_ms();
				//lib_hdmi_hd21_fw_disparity_error_status(nport,phy_st[nport].disp_err2);
				newbase_hdmi_fw_ced_measure(nport, phy_st[nport].ced_err2, 60);
				for (lane = 0; lane < phy_st[nport].lane_num ; lane++) 
				{

					if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
					{
						HDMI_PRINTF("[disp ing]lane=%d, disp_adp_stage[lane]=%d, m_wrong_direction_cnt=%d, disp_err_zero_cnt[lane]=%d \n",lane,
						phy_st[nport].disp_adp_stage[lane], m_wrong_direction_cnt[nport][lane], phy_st[nport].disp_err_zero_cnt[lane] );

						//if(m_wrong_direction_cnt[nport][lane] >10)
						//{
						//	HDMI_EMG("[Disp end]wrong_direction over, nport=%d, lane=%d, m_wrong_direction_cnt[nport][lane] =%d\n", nport, lane, m_wrong_direction_cnt[nport][lane] );
						//	m_wrong_direction_cnt[nport][lane] =0;
						//	phy_st[nport].disp_adp_stage[lane] = 2;  //end
						//}

						HDMI_PRINTF("[disp adp]lane=%d,  ced_err2=%d, ced_err1=%d, ced_err0=%d\n", 
						lane, 
						phy_st[nport].ced_err2[lane],
						phy_st[nport].ced_err1[lane],
						phy_st[nport].ced_err0[lane]);
					}

					//Check for Stop tuning condition
					if (phy_st[nport].disp_err_zero_cnt[lane] >=10000) 
					{
						//phy_st[nport].disp_adp_stage[lane] = 2;//end Marked for keep on tuing  never stop 
						phy_st[nport].disp_err_zero_cnt[lane] = 10000;//Set to Maxmum error cnt protect
						if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
						{
							HDMI_PRINTF("[disp adp stage1] lane=%d, disp_err_zero_cnt %d over(tap1=%d)\n",lane,phy_st[nport].disp_err_zero_cnt[lane] , phy_st[nport].dfe_t[lane].tap1);
						}
					}

					//Dummy now for No tap1_done case
					if((adj_le_done[nport][lane]==TRUE)&&(adj_tap1_done[nport][lane]==TRUE))
					{
						phy_st[nport].disp_adp_stage[lane] = 2;//end
						phy_st[nport].disp_err_zero_cnt[lane] = 0;
						HDMI_PRINTF("[disp End]ADP Done  lane=%d, disp_err_zero_cnt %d over(tap1=%d)\n",lane,phy_st[nport].disp_err_zero_cnt[lane] , phy_st[nport].dfe_t[lane].tap1);
					}

	
					if (phy_st[nport].disp_adp_stage[lane] == 2) 
					{  //end
						HDMI_PRINTF("[disp End] (lane=%d tap1=%d) tap2=%d)\n",lane,phy_st[nport].dfe_t[lane].tap1, phy_st[nport].dfe_t[lane].tap2);
						continue;
					}
					else if (phy_st[nport].disp_adp_stage[lane] == 1)
					{

					#if HDMI20_PHY_ERROR_CORRECTION_EN // le tap1
						if(((phy_st[nport].ced_err2[lane] ) <= 10)||(HDMI_ABS(phy_st[nport].ced_err2[lane],phy_st[nport].ced_err1[lane])<10))	
						{
							if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
							{
								HDMI_PRINTF("Almost no error increased , error @ port[%d]lane:%d, err2= %d, err1= %d\n",nport,lane, phy_st[nport].ced_err2[lane], phy_st[nport].ced_err1[lane]);
							}
							phy_st[nport].disp_err_zero_cnt[lane] = phy_st[nport].disp_err_zero_cnt[lane] + 1;					
							phy_st[nport].disp_start = 2;
						}
						else
						{
							//if ((phy_st[nport].ced_err2[lane]<= phy_st[nport].ced_err1[lane])||(adj_parameter_changed[nport][lane]== TRUE))
							//if ((phy_st[nport].ced_err2[lane]< phy_st[nport].ced_err1[lane])&&((phy_st[nport].ced_err1[lane]- phy_st[nport].ced_err2[lane])>10))
							//if ((phy_st[nport].ced_err2[lane]<= phy_st[nport].ced_err1[lane])||(adj_parameter_changed[nport][lane]== TRUE))
							if ((phy_st[nport].ced_err2[lane]<=phy_st[nport].ced_err1[lane])&&((phy_st[nport].ced_err1[lane]- phy_st[nport].ced_err2[lane])>20))
							//if (phy_st[nport].ced_err2[lane]< phy_st[nport].ced_err1[lane])
							{

								if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
								{
									HDMI_PRINTF("[disp stage1]  lane=%d, disp_err_zero_cnt %d  ced_err2[lane]=%d)\n",lane,phy_st[nport].disp_err_zero_cnt[lane] , phy_st[nport].ced_err2[lane]);
								}
								if (adj_parameter_changed[nport][lane]== TRUE)
								{
									adj_parameter_changed[nport][lane]= FALSE;
								}

								phy_st[nport].disp_err_zero_cnt[lane]=0; //

								if((enable_adj_tap1[nport][lane]==TRUE) || (adj_le_done[nport][lane]==TRUE)) //
								{
									
									phy_st[nport].dfe_t[lane].tap1 = phy_st[nport].dfe_t[lane].tap1 +2* phy_st[nport].tap1_adp_step[lane];
									if(phy_st[nport].dfe_t[lane].tap1<0)
									{
										phy_st[nport].dfe_t[lane].tap1 = 0;
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										if(adj_le_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= FALSE;//change  to LE adj
											adj_parameter_changed[nport][lane]= TRUE;
										}
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
										HDMI_EMG("check tap1 hit boundary, lane = %d, tap1 = %d\n", lane, phy_st[nport].dfe_t[lane].tap1);			
									}
									else if(phy_st[nport].dfe_t[lane].tap1>TAP1_MAX_VALUE)
									{
										phy_st[nport].dfe_t[lane].tap1 = TAP1_MAX_VALUE;
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										if(adj_le_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= FALSE;//change  to LE adj
											adj_parameter_changed[nport][lane]= TRUE;
										}
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
										HDMI_EMG("check tap1 hit boundary, lane = %d, tap1 = %d\n", lane, phy_st[nport].dfe_t[lane].tap1);			
									}
									if(adj_tap1_done[nport][lane]==FALSE)
									{
										lib_hdmi_dfe_init_tap1(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap1);
										if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
										{
											HDMI_PRINTF("[disp stage1] tap1 adj (lane=%d tap1=%d)\n",lane,phy_st[nport].dfe_t[lane].tap1);
										}
									}

								}
								else //adj le
								{
								
									phy_st[nport].dfe_t[lane].le = phy_st[nport].dfe_t[lane].le + phy_st[nport].le_adp_step[lane];

									if(phy_st[nport].dfe_t[lane].le<0)
									{
										phy_st[nport].dfe_t[lane].le = 0;
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										if(adj_tap1_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
											adj_parameter_changed[nport][lane]= TRUE;
										}
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
										phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
										HDMI_EMG("check le hit boundary, lane = %d, le = %d\n", lane, phy_st[nport].dfe_t[lane].le);			
									}
									else if(phy_st[nport].dfe_t[lane].le>LE_MAX_VALUE)
									//else if(phy_st[nport].dfe_t[lane].le>(phy_st[nport].dfe_t[lane].lemax+5))
									{

										phy_st[nport].dfe_t[lane].le = LE_MAX_VALUE;
										//phy_st[nport].dfe_t[lane].le = phy_st[nport].dfe_t[lane].lemax+5;
										if(adj_tap1_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
											adj_parameter_changed[nport][lane]= TRUE;
										}
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
										phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
										HDMI_EMG(" check le hit boundary, lane = %d, le = %d\n", lane, phy_st[nport].dfe_t[lane].le);			
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
									}
									else    //Change Tap2 value only when LE not in boundary condition
									{
										phy_st[nport].dfe_t[lane].tap2 = phy_st[nport].dfe_t[lane].tap2 +2* phy_st[nport].tap2_adp_step[lane];
										if(phy_st[nport].dfe_t[lane].tap2<-31)
										{
											phy_st[nport].dfe_t[lane].tap2 = -31;
											//phy_st[nport].disp_adp_stage[lane] = 3;  // end
											//enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
											HDMI_EMG("check le hit boundary, lane = %d, tap2= %d\n", lane, phy_st[nport].dfe_t[lane].tap2);			
										}
										else if(phy_st[nport].dfe_t[lane].tap2>31)
										{
											//enable_adj_tap1[nport][lane]= TRUE;	//
											phy_st[nport].dfe_t[lane].tap2 = 31;
											HDMI_EMG("check le hit boundary, lane = %d, tap2 = %d\n", lane, phy_st[nport].dfe_t[lane].tap2);			
											//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										}
									}
									if(adj_le_done[nport][lane]==FALSE)
									{
										lib_hdmi_dfe_init_le(nport,(1<<lane),phy_st[nport].dfe_t[lane].le);
										lib_hdmi_dfe_init_tap2(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap2);										
										if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
										{
											HDMI_PRINTF("[disp stage1] le & tap2 adj (lane=%d le=%d tap2=%d)\n",lane,phy_st[nport].dfe_t[lane].le,phy_st[nport].dfe_t[lane].tap2);

										}
									}
								}
							}
							//else if (phy_st[nport].ced_err2[lane]>phy_st[nport].ced_err1[lane])
							else if ((phy_st[nport].ced_err2[lane]>phy_st[nport].ced_err1[lane])&&((phy_st[nport].ced_err2[lane]- phy_st[nport].ced_err1[lane])>20))
							{ 
								phy_st[nport].disp_err_zero_cnt[lane]=0;	//

								if((enable_adj_tap1[nport][lane]==TRUE) || (adj_le_done[nport][lane]==TRUE)) //
								{
									phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
									phy_st[nport].dfe_t[lane].tap1 = phy_st[nport].dfe_t[lane].tap1 + 2*phy_st[nport].tap1_adp_step[lane];
									m_wrong_direction_cnt[nport][lane] ++;
									if(m_wrong_direction_cnt[nport][lane]>1 )
									{
										m_wrong_direction_cnt[nport][lane]=0;
										enable_adj_tap1[nport][lane]= FALSE;//change  to LE adj
										adj_parameter_changed[nport][lane]= TRUE;
									}
									
									if(phy_st[nport].dfe_t[lane].tap1<0)
									{
										phy_st[nport].dfe_t[lane].tap1 = 0;
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										if(adj_le_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= FALSE;//change  to LE adj
											adj_parameter_changed[nport][lane]= TRUE;
										}
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
										HDMI_EMG("check tap1 hit boundary, lane = %d, tap1 = %d\n", lane, phy_st[nport].dfe_t[lane].tap1);			
									}
									else if(phy_st[nport].dfe_t[lane].tap1>TAP1_MAX_VALUE)
									{
										phy_st[nport].dfe_t[lane].tap1 = TAP1_MAX_VALUE;
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										if(adj_le_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= FALSE;//change  to LE adj
											adj_parameter_changed[nport][lane]= TRUE;
										}
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
										HDMI_EMG("check tap1 hit boundary, lane = %d, tap1 = %d\n", lane, phy_st[nport].dfe_t[lane].tap1);			
									}
									if(adj_tap1_done[nport][lane]==FALSE)
									{
										lib_hdmi_dfe_init_tap1(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap1);
										if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
										{
											HDMI_PRINTF("[disp stage1] tap1 adj - Inverse step=%d (nport=%d, lane=%d tap1=%d, m_wrong_direction_cnt=%d)\n",phy_st[nport].tap1_adp_step[lane], nport, lane,phy_st[nport].dfe_t[lane].tap1,  m_wrong_direction_cnt[nport][lane]);
										}
										/*
										if ((phy_st[nport].ced_err2[lane] <25) )
										{
											//adj_tap1_done[nport][lane]=TRUE;
											HDMI_PRINTF("[disp stage1] ced_err2<25, tap1 adj ctn (lane=%d le=%d)\n",lane,phy_st[nport].dfe_t[lane].tap1);
											//enable_adj_tap1[nport][lane]= FALSE;
											//m_wrong_direction_cnt[nport][lane]=0;
										}*/
									}

								}
								else //adj le
								{
									phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
									
									phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];

									phy_st[nport].dfe_t[lane].le = phy_st[nport].dfe_t[lane].le + phy_st[nport].le_adp_step[lane];

									m_wrong_direction_cnt[nport][lane] ++;
									if(m_wrong_direction_cnt[nport][lane]>1 )
									{
										m_wrong_direction_cnt[nport][lane]=0;
										enable_adj_tap1[nport][lane]= TRUE;//change  to TPA1 adj
										adj_parameter_changed[nport][lane]= TRUE;
									}

									if(phy_st[nport].dfe_t[lane].le<0)
									{
										phy_st[nport].dfe_t[lane].le = 0;
										if(adj_tap1_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
											adj_parameter_changed[nport][lane]= TRUE;
										}										
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
										phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
										HDMI_EMG("check le hit boundary, lane = %d, le = %d\n", lane, phy_st[nport].dfe_t[lane].le);			
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
									}
									 else if(phy_st[nport].dfe_t[lane].le>LE_MAX_VALUE)
									{

										phy_st[nport].dfe_t[lane].le = LE_MAX_VALUE;
										m_wrong_direction_cnt[nport][lane]=0;
										if(adj_tap1_done[nport][lane]==FALSE)
										{
											enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
											adj_parameter_changed[nport][lane]= TRUE;
										}										
										phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
										phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
										HDMI_EMG("check le hit boundary, lane = %d, le = %d\n", lane, phy_st[nport].dfe_t[lane].le);			
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
									}
									else     //Change Tap2 value only when LE not in boundary condition
									{
										phy_st[nport].dfe_t[lane].tap2 = phy_st[nport].dfe_t[lane].tap2 +2* phy_st[nport].tap2_adp_step[lane];
										if(phy_st[nport].dfe_t[lane].tap2<-31)
										{
											phy_st[nport].dfe_t[lane].tap2 = -31;
											//phy_st[nport].disp_adp_stage[lane] = 3;  // end
											//enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
											HDMI_EMG("check le hit boundary, lane = %d, tap2 = %d\n", lane, phy_st[nport].dfe_t[lane].tap2);			
										}
										else if(phy_st[nport].dfe_t[lane].tap2>31)
										{
											//enable_adj_tap1[nport][lane]= TRUE;
											phy_st[nport].dfe_t[lane].tap2 = 31;
											HDMI_EMG("check le hit boundary, lane = %d, tap2 = %d\n", lane, phy_st[nport].dfe_t[lane].tap2);			
											//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										}
									}

									if(adj_le_done[nport][lane]==FALSE)
									{
										lib_hdmi_dfe_init_le(nport,(1<<lane),phy_st[nport].dfe_t[lane].le);
										lib_hdmi_dfe_init_tap2(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap2);										
										if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
										{
											HDMI_PRINTF("[disp stage1] le& tap2 adj - Inverse step=%d (nport=%d, lane=%d le=%d, tap2=%d, m_wrong_direction_cnt=%d)\n",phy_st[nport].le_adp_step[lane], nport, lane,phy_st[nport].dfe_t[lane].le, phy_st[nport].dfe_t[lane].tap2,m_wrong_direction_cnt[nport][lane]);
										}
									}
								}
							}

						}
						#endif
					#if 0//HDMI20_PHY_ERROR_CORRECTION_EN //LE only
						//if(((phy_st[nport].ced_err2[lane] ) <= 10)||(HDMI_ABS(phy_st[nport].ced_err2[lane],phy_st[nport].ced_err1[lane])<10))	
						if((phy_st[nport].ced_err2[lane] ) <= 10)	
						{
							if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
							{
								HDMI_PRINTF("Almost no error increased , error @ port[%d]lane:%d, err2= %d, err1= %d\n",nport,lane, phy_st[nport].ced_err2[lane], phy_st[nport].ced_err1[lane]);
							}
							phy_st[nport].disp_err_zero_cnt[lane] = phy_st[nport].disp_err_zero_cnt[lane] + 1;					
							phy_st[nport].disp_start = 2;
						}
						else
						{
							//if ((phy_st[nport].ced_err2[lane]<= phy_st[nport].ced_err1[lane])||(adj_parameter_changed[nport][lane]== TRUE))
							if ((phy_st[nport].ced_err2[lane]<=phy_st[nport].ced_err1[lane])&&((phy_st[nport].ced_err1[lane]- phy_st[nport].ced_err2[lane])>20))
							//if (phy_st[nport].ced_err2[lane]< phy_st[nport].ced_err1[lane])
							{

								if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
								{
									HDMI_PRINTF("[disp stage1]  lane=%d, disp_err_zero_cnt %d  ced_err2[lane]=%d)\n",lane,phy_st[nport].disp_err_zero_cnt[lane] , phy_st[nport].ced_err2[lane]);
								}
								//if (adj_parameter_changed[nport][lane]== TRUE)
								//{
								//	adj_parameter_changed[nport][lane]= FALSE;
								//}

								phy_st[nport].disp_err_zero_cnt[lane]=0; //

								
								phy_st[nport].dfe_t[lane].le = phy_st[nport].dfe_t[lane].le + phy_st[nport].le_adp_step[lane];

								if(phy_st[nport].dfe_t[lane].le<0)
								{
									phy_st[nport].dfe_t[lane].le = 0;
									//phy_st[nport].disp_adp_stage[lane] = 3;  // end
									//if(adj_tap1_done[nport][lane]==FALSE)
									//{
										//enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
										//adj_parameter_changed[nport][lane]= TRUE;
									//}
									m_wrong_direction_cnt[nport][lane]=0;
									phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
									phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
									HDMI_EMG("check le hit boundary, lane = %d, le = %d\n", lane, phy_st[nport].dfe_t[lane].le);			
								}
								else if(phy_st[nport].dfe_t[lane].le>LE_MAX_VALUE)
								//else if(phy_st[nport].dfe_t[lane].le>(phy_st[nport].dfe_t[lane].lemax+5))
								{

									phy_st[nport].dfe_t[lane].le = LE_MAX_VALUE;
									//phy_st[nport].dfe_t[lane].le = phy_st[nport].dfe_t[lane].lemax+5;
									//if(adj_tap1_done[nport][lane]==FALSE)
									//{
										//enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
										//adj_parameter_changed[nport][lane]= TRUE;
									//}
									m_wrong_direction_cnt[nport][lane]=0;
									phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
									phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
									HDMI_EMG(" check le hit boundary, lane = %d, le = %d\n", lane, phy_st[nport].dfe_t[lane].le);			
									//phy_st[nport].disp_adp_stage[lane] = 3;  // end
								}
								else    //Change Tap2 value only when LE not in boundary condition
								{
									phy_st[nport].dfe_t[lane].tap2 = phy_st[nport].dfe_t[lane].tap2 +2* phy_st[nport].tap2_adp_step[lane];
									if(phy_st[nport].dfe_t[lane].tap2<-31)
									{
										phy_st[nport].dfe_t[lane].tap2 = -31;
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										//enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
										HDMI_EMG("check le hit boundary, lane = %d, tap2= %d\n", lane, phy_st[nport].dfe_t[lane].tap2);			
									}
									else if(phy_st[nport].dfe_t[lane].tap2>31)
									{
										//enable_adj_tap1[nport][lane]= TRUE;	//
										phy_st[nport].dfe_t[lane].tap2 = 31;
										HDMI_EMG("check le hit boundary, lane = %d, tap2 = %d\n", lane, phy_st[nport].dfe_t[lane].tap2);			
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
									}
								}
								if(adj_le_done[nport][lane]==FALSE)
								{
									lib_hdmi_dfe_init_le(nport,(1<<lane),phy_st[nport].dfe_t[lane].le);
									lib_hdmi_dfe_init_tap2(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap2);										
									if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
									{
										HDMI_PRINTF("[disp stage1] le & tap2 adj (lane=%d le=%d tap2=%d)\n",lane,phy_st[nport].dfe_t[lane].le,phy_st[nport].dfe_t[lane].tap2);

										}
								}
							}
							//else if (phy_st[nport].ced_err2[lane]>phy_st[nport].ced_err1[lane])
							else if ((phy_st[nport].ced_err2[lane]>phy_st[nport].ced_err1[lane])&&((phy_st[nport].ced_err2[lane]- phy_st[nport].ced_err1[lane])>20))
							{ 
								phy_st[nport].disp_err_zero_cnt[lane]=0;	//
								
								//else //adj le
								phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
								
								phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];

								phy_st[nport].dfe_t[lane].le = phy_st[nport].dfe_t[lane].le + phy_st[nport].le_adp_step[lane];

								m_wrong_direction_cnt[nport][lane] ++;
								//if(m_wrong_direction_cnt[nport][lane]>1 )
								//{
								//	m_wrong_direction_cnt[nport][lane]=0;
									//enable_adj_tap1[nport][lane]= TRUE;//change  to TPA1 adj
									//adj_parameter_changed[nport][lane]= TRUE;
								//}

								if(phy_st[nport].dfe_t[lane].le<0)
								{
									phy_st[nport].dfe_t[lane].le = 0;
									//if(adj_tap1_done[nport][lane]==FALSE)
									//{
										//enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
										//adj_parameter_changed[nport][lane]= TRUE;
									//}										
									//m_wrong_direction_cnt[nport][lane]=0;
									phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
									phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
									HDMI_EMG("check le hit boundary, lane = %d, le = %d\n", lane, phy_st[nport].dfe_t[lane].le);			
									//phy_st[nport].disp_adp_stage[lane] = 3;  // end
								}
								 else if(phy_st[nport].dfe_t[lane].le>LE_MAX_VALUE)
								//else if(phy_st[nport].dfe_t[lane].le>(phy_st[nport].dfe_t[lane].lemax+5))
								{

									phy_st[nport].dfe_t[lane].le = LE_MAX_VALUE;
									//phy_st[nport].dfe_t[lane].le = phy_st[nport].dfe_t[lane].lemax+5;
									m_wrong_direction_cnt[nport][lane]=0;
									if(adj_tap1_done[nport][lane]==FALSE)
									{
										//enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
										//adj_parameter_changed[nport][lane]= TRUE;
									}										
									phy_st[nport].le_adp_step[lane] = - 1 *phy_st[nport].le_adp_step[lane];
									phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];
									HDMI_EMG("check le hit boundary, lane = %d, le = %d\n", lane, phy_st[nport].dfe_t[lane].le);			
									//phy_st[nport].disp_adp_stage[lane] = 3;  // end
								}
								else     //Change Tap2 value only when LE not in boundary condition
								{
									phy_st[nport].dfe_t[lane].tap2 = phy_st[nport].dfe_t[lane].tap2 +2* phy_st[nport].tap2_adp_step[lane];
									if(phy_st[nport].dfe_t[lane].tap2<-31)
									{
										phy_st[nport].dfe_t[lane].tap2 = -31;
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										//enable_adj_tap1[nport][lane]= TRUE;//change  to TAP1 adj
										HDMI_EMG("check le hit boundary, lane = %d, tap2 = %d\n", lane, phy_st[nport].dfe_t[lane].tap2);			
									}
									else if(phy_st[nport].dfe_t[lane].tap2>31)
									{
										//enable_adj_tap1[nport][lane]= TRUE;
										phy_st[nport].dfe_t[lane].tap2 = 31;
										HDMI_EMG("check le hit boundary, lane = %d, tap2 = %d\n", lane, phy_st[nport].dfe_t[lane].tap2);			
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
									}
								}

								if(adj_le_done[nport][lane]==FALSE)
								{
									lib_hdmi_dfe_init_le(nport,(1<<lane),phy_st[nport].dfe_t[lane].le);
									lib_hdmi_dfe_init_tap2(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap2);										
									if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
									{
										HDMI_PRINTF("[disp stage1] le& tap2 adj - Inverse step=%d (nport=%d, lane=%d le=%d, tap2=%d, m_wrong_direction_cnt=%d)\n",phy_st[nport].le_adp_step[lane], nport, lane,phy_st[nport].dfe_t[lane].le, phy_st[nport].dfe_t[lane].tap2,m_wrong_direction_cnt[nport][lane]);
									}
								}
							}

						}
						#endif
						
						phy_st[nport].disp_start = 2;
						phy_st[nport].ced_err0[lane]=phy_st[nport].ced_err1[lane];
						phy_st[nport].ced_err1[lane]=phy_st[nport].ced_err2[lane];

					}

				}

				if (phy_st[nport].lane_num == HDMI_4LANE)
				{
					if ((phy_st[nport].disp_adp_stage[0] == 2) & (phy_st[nport].disp_adp_stage[1] == 2) & (phy_st[nport].disp_adp_stage[2] == 2) & (phy_st[nport].disp_adp_stage[3] == 2))
					{
						phy_st[nport].disp_start = 3; //ending
						HDMI_EMG("[disp stage2] port=%d, 4 lane done, dfe tap1(0,1,2,3)=(%d,%d,%d,%d)\\n",
							nport, phy_st[nport].dfe_t[0].tap1, phy_st[nport].dfe_t[1].tap1, phy_st[nport].dfe_t[2].tap1, phy_st[nport].dfe_t[3].tap1);
					}
				}
				else
				{
					if ((phy_st[nport].disp_adp_stage[0] == 2) & (phy_st[nport].disp_adp_stage[1] == 2) & (phy_st[nport].disp_adp_stage[2] == 2))  
					{
						phy_st[nport].disp_start = 3; //ending
						HDMI_EMG("[disp stage2] port=%d, 3 lane done, dfe tap1(0,1,2)=(%d,%d,%d)\\n",
							nport, phy_st[nport].dfe_t[0].tap1, phy_st[nport].dfe_t[1].tap1, phy_st[nport].dfe_t[2].tap1);
					}	
				}

			}
			else 
			{
				phy_st[nport].disp_timer_cnt[lane]--;
			}
		
		}
		else if(phy_st[nport].disp_start==3)
		{//End lib_hdmi20_phy_error_handling
			#if 0
			if((lt_fsm_status[nport].tx_flt_no_train == TRUE)&&(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1 , HDMI_FLOW_CFG2_CTS_EXTEND_MODE) == TRUE))
			//if((GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1 , HDMI_FLOW_CFG2_CTS_EXTEND_MODE) == TRUE))
			{//to do 
				newbase_hdmi_scdc_hw_mask_enable(nport, 0);
				HDMI_PRINTF("[disp stage3] port=%d, Adjust finished, close extend\n", nport);
				phy_st[nport].disp_start =4;
			}
			#endif
		}
		else 
		{
			//phy_st[nport].disp_start == 3 do nothing

		}
	}
	else  //Mode ==3
	{

		if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
		{
			newbase_hdmi21_dump_dfe_para(nport,phy_st[nport].lane_num);
		}

		
		if (phy_st[nport].disp_start == 0) 
		{

			for (lane=0; lane< 3; lane++) {
				phy_st[nport].dfe_t[lane].tap1 = lib_hdmi_dfe_get_tap1(nport, lane);
				phy_st[nport].dfe_t[lane].tap2 = lib_hdmi_dfe_get_tap2(nport, lane);
				phy_st[nport].dfe_t[lane].le = lib_hdmi_dfe_get_le(nport, lane);

				phy_st[nport].dfe_t[lane].le_add_tap1 = phy_st[nport].dfe_t[lane].le+phy_st[nport].dfe_t[lane].tap1;

				lib_hdmi_dfe_init_tap1(nport,(1<<lane),phy_st[nport].dfe_t[lane].le_add_tap1);

				phy_st[nport].dfe_t[lane].tap1 = lib_hdmi_dfe_get_tap1(nport, lane);
				phy_st[nport].dfe_t[lane].tap2 = lib_hdmi_dfe_get_tap2(nport, lane);
				phy_st[nport].dfe_t[lane].le = lib_hdmi_dfe_get_le(nport, lane);

				HDMI_PRINTF("[disp init]ADD LEdisp_start=%d, phy_st[nport].dfe_t[lane].le_add_tap1=%d\n",phy_st[nport].disp_start, phy_st[nport].dfe_t[lane].le_add_tap1);			

			}

			
			newbase_hdmi_fw_ced_measure(nport, phy_st[nport].ced_err0, 60);

			phy_st[nport].disp_start = 1;
			phy_st[nport].disp_adp_stage[0]  = phy_st[nport].disp_adp_stage[1]  = phy_st[nport].disp_adp_stage[2] = phy_st[nport].disp_adp_stage[3]  = 1;
			phy_st[nport].disp_timer_cnt[0] = phy_st[nport].disp_timer_cnt[1]= phy_st[nport].disp_timer_cnt[2]= phy_st[nport].disp_timer_cnt[3] = 5;  //100ms
		}
		else if (phy_st[nport].disp_start == 1)
		{

			if (phy_st[nport].disp_timer_cnt[lane] == 0)
			{  //near 100ms
				newbase_hdmi_fw_ced_measure(nport, phy_st[nport].ced_err1, 60);

				phy_st[nport].disp_start = 2;
				phy_st[nport].disp_adp_stage[0]  = phy_st[nport].disp_adp_stage[1]  = phy_st[nport].disp_adp_stage[2] = phy_st[nport].disp_adp_stage[3]  = 1;
				phy_st[nport].disp_timer_cnt[0] = phy_st[nport].disp_timer_cnt[1]= phy_st[nport].disp_timer_cnt[2]= phy_st[nport].disp_timer_cnt[3] = 5;  //100ms
			}
			else 
			{
				phy_st[nport].disp_timer_cnt[lane]--;
			}
	
		}
		else if (phy_st[nport].disp_start == 2)
		{
			if (phy_st[nport].disp_timer_cnt[lane] == 0) 
			{  //near 100ms
				TimeStamp2=hdmi_get_system_time_ms();
				//lib_hdmi_hd21_fw_disparity_error_status(nport,phy_st[nport].disp_err2);
				newbase_hdmi_fw_ced_measure(nport, phy_st[nport].ced_err2, 60);
				for (lane = 0; lane < phy_st[nport].lane_num ; lane++) 
				{
					//HDMI_PRINTF("[disp ing]lane=%d, disp_adp_stage[lane]=%d, m_wrong_direction_cnt=%d, disp_err_zero_cnt[lane]=%d \n",lane,
					//phy_st[nport].disp_adp_stage[lane], m_wrong_direction_cnt[nport][lane], phy_st[nport].disp_err_zero_cnt[lane] );

					//if(m_wrong_direction_cnt[nport][lane] >10)
					//{
					//	HDMI_EMG("[Disp end]wrong_direction over, nport=%d, lane=%d, m_wrong_direction_cnt[nport][lane] =%d\n", nport, lane, m_wrong_direction_cnt[nport][lane] );
					//	m_wrong_direction_cnt[nport][lane] =0;
					//	phy_st[nport].disp_adp_stage[lane] = 2;  //end
					//}

					phy_st[nport].dfe_t[lane].le_add_tap1 = phy_st[nport].dfe_t[lane].le+phy_st[nport].dfe_t[lane].tap1;

					if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
					{
						HDMI_PRINTF("[disp adp]lane=%d,  ced_err2=%d, ced_err1=%d, ced_err0=%d\n", 
						lane, 
						phy_st[nport].ced_err2[lane],
						phy_st[nport].ced_err1[lane],
						phy_st[nport].ced_err0[lane]);
					}
					//Check for Stop tuning condition
					if (phy_st[nport].disp_err_zero_cnt[lane] >=10000) 
					{
						//phy_st[nport].disp_adp_stage[lane] = 2;//end Marked for keep on tuing  never stop 
						phy_st[nport].disp_err_zero_cnt[lane] = 10000;//Set to Maxmum error cnt protect
						if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
						{
							HDMI_PRINTF("[disp adp stage1] lane=%d, disp_err_zero_cnt %d over(tap1=%d)\n",lane,phy_st[nport].disp_err_zero_cnt[lane] , phy_st[nport].dfe_t[lane].tap1);
						}
					}

					//Dummy now for No tap1_done case
					if((adj_le_done[nport][lane]==TRUE)&&(adj_tap1_done[nport][lane]==TRUE))
					{
						phy_st[nport].disp_adp_stage[lane] = 2;//end
						phy_st[nport].disp_err_zero_cnt[lane] = 0;
						HDMI_PRINTF("[disp End]ADP Done  lane=%d, disp_err_zero_cnt %d over(tap1=%d)\n",lane,phy_st[nport].disp_err_zero_cnt[lane] , phy_st[nport].dfe_t[lane].tap1);
					}

	
					if (phy_st[nport].disp_adp_stage[lane] == 2) 
					{  //end
						HDMI_PRINTF("[disp End] (lane=%d tap1=%d) tap2=%d)\n",lane,phy_st[nport].dfe_t[lane].tap1, phy_st[nport].dfe_t[lane].tap2);
						continue;
					}
					else if (phy_st[nport].disp_adp_stage[lane] == 1)
					{

					 #if HDMI20_PHY_ERROR_CORRECTION_EN

						if((phy_st[nport].ced_err2[lane] ) <= 10)	
						{
							//HDMI_PRINTF("Almost no error increased , error @ port[%d]lane:%d, err2= %d, err1= %d\n",nport,lane, phy_st[nport].disp_err2[lane], phy_st[nport].disp_err1[lane]);
							if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
							{
								HDMI_PRINTF("Almost no error increased , error @ port[%d]lane:%d, err2= %d, err1= %d\n",nport,lane, phy_st[nport].ced_err2[lane], phy_st[nport].ced_err1[lane]);
							}

							phy_st[nport].disp_err_zero_cnt[lane] = phy_st[nport].disp_err_zero_cnt[lane] + 1;					
							phy_st[nport].disp_start = 2;
						}
						else
						{
							//if ((phy_st[nport].ced_err2[lane]<= phy_st[nport].ced_err1[lane])||(adj_parameter_changed[nport][lane]== TRUE))


							if ((phy_st[nport].ced_err2[lane]<=phy_st[nport].ced_err1[lane])&&((phy_st[nport].ced_err1[lane]- phy_st[nport].ced_err2[lane])>20))
							{

								if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
								{
									HDMI_PRINTF("[disp stage1]  lane=%d, disp_err_zero_cnt %d  ced_err2[lane]=%d)\n",lane,phy_st[nport].disp_err_zero_cnt[lane] , phy_st[nport].ced_err2[lane]);
								}
								if (adj_parameter_changed[nport][lane]== TRUE)
								{
									adj_parameter_changed[nport][lane]= FALSE;
								}

								phy_st[nport].disp_err_zero_cnt[lane]=0; //

								if(adj_tap1_done[nport][lane]==FALSE) //
								{
									//if(( phy_st[nport].dfe_t[lane].le)< (DFE_P0_REG_DFE_EN_L0_P0_get_le_min_lane0_p0(hdmi_in(DFE_REG_DFE_EN_L0_reg))))
									phy_st[nport].dfe_t[lane].le_add_tap1 = phy_st[nport].dfe_t[lane].le_add_tap1 +1* phy_st[nport].tap1_adp_step[lane];
		
									if(phy_st[nport].dfe_t[lane].le_add_tap1<0)
									{
										phy_st[nport].dfe_t[lane].le_add_tap1 = 0;
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
										phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];

										HDMI_EMG("check tap1 hit boundary, lane = %d, tap1 = %d\n", lane, phy_st[nport].dfe_t[lane].le_add_tap1);			
									}
									else if(phy_st[nport].dfe_t[lane].le_add_tap1>(TAP1_MAX_VALUE+(DFE_P0_REG_DFE_EN_L0_P0_get_le_min_lane0_p0(hdmi_in(DFE_REG_DFE_EN_L0_reg)))))
									{
										phy_st[nport].dfe_t[lane].le_add_tap1 = (TAP1_MAX_VALUE+(DFE_P0_REG_DFE_EN_L0_P0_get_le_min_lane0_p0(hdmi_in(DFE_REG_DFE_EN_L0_reg))));
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
										phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];

										HDMI_EMG("check tap1 hit boundary, lane = %d, le_add_tap1 = %d\n", lane, phy_st[nport].dfe_t[lane].le_add_tap1);			
									}

									lib_hdmi_dfe_init_tap1(nport,(1<<lane),phy_st[nport].dfe_t[lane].le_add_tap1);

									if(phy_st[nport].dfe_t[lane].le_add_tap1 <= (DFE_P0_REG_DFE_EN_L0_P0_get_le_min_lane0_p0(hdmi_in(DFE_REG_DFE_EN_L0_reg))))
									{
										phy_st[nport].dfe_t[lane].tap2 = phy_st[nport].dfe_t[lane].tap2 +2* phy_st[nport].tap2_adp_step[lane];
										lib_hdmi_dfe_init_tap2(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap2);
									}

									if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
									{
										HDMI_PRINTF("[disp stage1] tap1 adj (lane=%d le_add_tap1=%d tap2=%d)\n",lane,phy_st[nport].dfe_t[lane].le_add_tap1,phy_st[nport].dfe_t[lane].tap2);
									}
								}
							
								//phy_st[nport].disp_start = 2;
								//phy_st[nport].ced_err0[lane]=phy_st[nport].ced_err1[lane];
								//phy_st[nport].ced_err1[lane]=phy_st[nport].ced_err2[lane];

							}
							//else if (phy_st[nport].ced_err2[lane]>phy_st[nport].ced_err1[lane])
							else if ((phy_st[nport].ced_err2[lane]>phy_st[nport].ced_err1[lane])&&((phy_st[nport].ced_err2[lane]- phy_st[nport].ced_err1[lane])>20))
							{ 
								phy_st[nport].disp_err_zero_cnt[lane]=0;	//

								if(adj_tap1_done[nport][lane]==FALSE) //
								{
									phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];

									//if(( phy_st[nport].dfe_t[lane].le)< (DFE_P0_REG_DFE_EN_L0_P0_get_le_min_lane0_p0(hdmi_in(DFE_REG_DFE_EN_L0_reg))))

									phy_st[nport].dfe_t[lane].le_add_tap1 = phy_st[nport].dfe_t[lane].le_add_tap1 +1* phy_st[nport].tap1_adp_step[lane];
		
									if(phy_st[nport].dfe_t[lane].le_add_tap1<0)
									{
										phy_st[nport].dfe_t[lane].le_add_tap1 = 0;
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
										phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];

										HDMI_EMG("check tap1 hit boundary, lane = %d, tap1 = %d\n", lane, phy_st[nport].dfe_t[lane].le_add_tap1);			
									}
									else if(phy_st[nport].dfe_t[lane].le_add_tap1>(TAP1_MAX_VALUE+(DFE_P0_REG_DFE_EN_L0_P0_get_le_min_lane0_p0(hdmi_in(DFE_REG_DFE_EN_L0_reg)))))
									{
										phy_st[nport].dfe_t[lane].le_add_tap1 = (TAP1_MAX_VALUE+(DFE_P0_REG_DFE_EN_L0_P0_get_le_min_lane0_p0(hdmi_in(DFE_REG_DFE_EN_L0_reg))));
										//phy_st[nport].disp_adp_stage[lane] = 3;  // end
										m_wrong_direction_cnt[nport][lane]=0;
										phy_st[nport].tap1_adp_step[lane] = - 1 *phy_st[nport].tap1_adp_step[lane];
										phy_st[nport].tap2_adp_step[lane] = - 1 *phy_st[nport].tap2_adp_step[lane];

										HDMI_EMG("check tap1 hit boundary, lane = %d, le_add_tap1 = %d\n", lane, phy_st[nport].dfe_t[lane].le_add_tap1);			
									}
									lib_hdmi_dfe_init_tap1(nport,(1<<lane),phy_st[nport].dfe_t[lane].le_add_tap1);

									if(phy_st[nport].dfe_t[lane].le_add_tap1 <= (DFE_P0_REG_DFE_EN_L0_P0_get_le_min_lane0_p0(hdmi_in(DFE_REG_DFE_EN_L0_reg))))
									{
										phy_st[nport].dfe_t[lane].tap2 = phy_st[nport].dfe_t[lane].tap2 +2* phy_st[nport].tap2_adp_step[lane];
										lib_hdmi_dfe_init_tap2(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap2);
									}
									if(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_HD20_PHY_LOG_ENABLE )) //for phy log debugging
									{
										HDMI_PRINTF("[disp stage1] tap1 adj (lane=%d le_add_tap1=%d tap2=%d)\n",lane,phy_st[nport].dfe_t[lane].le_add_tap1,phy_st[nport].dfe_t[lane].tap2);
									}
								}

								//phy_st[nport].disp_start = 2;
								//phy_st[nport].ced_err0[lane]=phy_st[nport].ced_err1[lane];
								//phy_st[nport].ced_err1[lane]=phy_st[nport].ced_err2[lane];
							}


						}

					#endif

					}

					phy_st[nport].disp_start = 2;
					phy_st[nport].ced_err0[lane]=phy_st[nport].ced_err1[lane];
					phy_st[nport].ced_err1[lane]=phy_st[nport].ced_err2[lane];

				}

				phy_st[nport].dfe_t[lane].tap1 = lib_hdmi_dfe_get_tap1(nport, lane);
				phy_st[nport].dfe_t[lane].le = lib_hdmi_dfe_get_le(nport, lane);
				if (phy_st[nport].lane_num == HDMI_4LANE)
				{
					if ((phy_st[nport].disp_adp_stage[0] == 2) & (phy_st[nport].disp_adp_stage[1] == 2) & (phy_st[nport].disp_adp_stage[2] == 2) & (phy_st[nport].disp_adp_stage[3] == 2))
					{
						phy_st[nport].disp_start = 3; //ending
						HDMI_EMG("[disp stage2] port=%d, 4 lane done, dfe tap1(0,1,2,3)=(%d,%d,%d,%d)\\n",
							nport, phy_st[nport].dfe_t[0].tap1, phy_st[nport].dfe_t[1].tap1, phy_st[nport].dfe_t[2].tap1, phy_st[nport].dfe_t[3].tap1);
					}
				}
				else
				{
					if ((phy_st[nport].disp_adp_stage[0] == 2) & (phy_st[nport].disp_adp_stage[1] == 2) & (phy_st[nport].disp_adp_stage[2] == 2))  
					{
						phy_st[nport].disp_start = 3; //ending
						HDMI_EMG("[disp stage2] port=%d, 3 lane done, dfe tap1(0,1,2)=(%d,%d,%d)\\n",
							nport, phy_st[nport].dfe_t[0].tap1, phy_st[nport].dfe_t[1].tap1, phy_st[nport].dfe_t[2].tap1);
					}	
				}

			}
			else 
			{
				phy_st[nport].disp_timer_cnt[lane]--;
			}
		
		}
		else if(phy_st[nport].disp_start==3)
		{//End lib_hdmi20_phy_error_handling
			#if 0
			if((lt_fsm_status[nport].tx_flt_no_train == TRUE)&&(GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1 , HDMI_FLOW_CFG2_CTS_EXTEND_MODE) == TRUE))
			//if((GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1 , HDMI_FLOW_CFG2_CTS_EXTEND_MODE) == TRUE))
			{//to do 
				newbase_hdmi_scdc_hw_mask_enable(nport, 0);
				HDMI_PRINTF("[disp stage3] port=%d, Adjust finished, close extend\n", nport);
				phy_st[nport].disp_start =4;
			}
			#endif
		}
		else 
		{
			//phy_st[nport].disp_start == 3 do nothing

		}
	
	}

#endif
}




#if PHY_ADAPTIVE_BY_LTP_PAT_AFTER_LTP_PASS
//check ltp err cnt after finishing link training pass (for TEK CTS 20s ltp pattern still output)
void lib_hdmi_hd21_ltp_phy_adaptive_check(unsigned char nport, unsigned char lane_num)
{

	unsigned char lock[4];
	unsigned char lock_all = 0;
	unsigned int err_cnt[4];
	unsigned char lane = 0;


	if (phy_st[nport].ltp_state) {  //video state
		  lock[3] = hdmi_in(HD21_channel_align_c_reg)  & 0x1;
       	  lock[2] = hdmi_in(HD21_channel_align_r_reg)  & 0x1;
       	  lock[1] = hdmi_in(HD21_channel_align_g_reg) & 0x1;
      	         lock[0] = hdmi_in(HD21_channel_align_b_reg) & 0x1;
				 
		  if (lane_num == HDMI_4LANE) {
		  	if (lock[0] & lock[1] & lock[2] & lock[3])
				return;
		  } else {
			if (lock[0] & lock[1] & lock[2]) 
				return;
		  }
	}
	

	lock[0] = HD21_LT_B_get_lock_valid(hdmi_in(HD21_LT_B_reg));
   	lock[1] = HD21_LT_G_get_lock_valid(hdmi_in(HD21_LT_G_reg));
       lock[2] = HD21_LT_R_get_lock_valid(hdmi_in(HD21_LT_R_reg));
       lock[3] = HD21_LT_C_get_lock_valid(hdmi_in(HD21_LT_C_reg));

	if (lane_num == HDMI_4LANE) { 
		
		HDMI_PRINTF("[ltp_chk]ltp lock= %d %d %d %d\n",lock[0],lock[1],lock[2],lock[3]);
		lock_all = lock[0] & lock[1] & lock[2] & lock[3];
	}
	else {
		HDMI_PRINTF("[ltp_chk]ltp lock= %d %d %d\n",lock[0],lock[1],lock[2]);
		lock_all = lock[0] & lock[1] & lock[2];
	}

	 if (lock_all) {
		    hdmi_mask(HD21_LT_B_reg, ~(HD21_LT_B_popup_mask|_BIT4), HD21_LT_B_popup_mask);
		    hdmi_mask(HD21_LT_G_reg, ~(HD21_LT_G_popup_mask|_BIT4), HD21_LT_G_popup_mask);
		    hdmi_mask(HD21_LT_R_reg, ~(HD21_LT_R_popup_mask|_BIT4), HD21_LT_R_popup_mask);
		    if (lane_num == HDMI_4LANE)
		    	hdmi_mask(HD21_LT_C_reg, ~(HD21_LT_C_popup_mask|_BIT4), HD21_LT_C_popup_mask);

		    udelay(10);

		    if (lane_num == HDMI_4LANE)
				HDMI_PRINTF("[ltp_chk]%x %x %x %x\n",hdmi_in(HD21_LT_B_reg),hdmi_in(HD21_LT_G_reg),hdmi_in(HD21_LT_R_reg),hdmi_in(HD21_LT_C_reg));
		    else
				HDMI_PRINTF("[ltp_chk]%x %x %x\n",hdmi_in(HD21_LT_B_reg),hdmi_in(HD21_LT_G_reg),hdmi_in(HD21_LT_R_reg));
			
		    err_cnt[0] = (lock[0]) ? HD21_LT_B_get_err_cnt(hdmi_in(HD21_LT_B_reg)) : 0xFFFFFFFF;
		    err_cnt[1] = (lock[1]) ? HD21_LT_G_get_err_cnt(hdmi_in(HD21_LT_G_reg)) : 0xFFFFFFFF;
		    err_cnt[2] = (lock[2]) ? HD21_LT_R_get_err_cnt(hdmi_in(HD21_LT_R_reg)) : 0xFFFFFFFF;
		    if (lane_num == HDMI_4LANE)
		    	err_cnt[3] = (lock[3]) ? HD21_LT_C_get_err_cnt(hdmi_in(HD21_LT_C_reg)) : 0xFFFFFFFF;

		    if (lane_num == HDMI_4LANE) {
			    if ((err_cnt[0] == 0x7FFF) ||  (err_cnt[1] == 0x7FFF)  ||  (err_cnt[2] == 0x7FFF) || (err_cnt[3] == 0x7FFF))
			    {
					hdmi_mask(HD21_LT_B_reg, ~(HDMI21_P0_HD21_LT_B_reset_mask|_BIT4), HDMI21_P0_HD21_LT_B_reset_mask);
				       hdmi_mask(HD21_LT_G_reg, ~(HDMI21_P0_HD21_LT_G_reset_mask|_BIT4), HDMI21_P0_HD21_LT_G_reset_mask);
					hdmi_mask(HD21_LT_R_reg, ~(HDMI21_P0_HD21_LT_R_reset_mask|_BIT4), HDMI21_P0_HD21_LT_R_reset_mask);
					hdmi_mask(HD21_LT_C_reg, ~(HDMI21_P0_HD21_LT_C_reset_mask|_BIT4), HDMI21_P0_HD21_LT_C_reset_mask);
					HDMI_PRINTF("[ltp_chk]change ltp to video pattern\n");
					phy_st[nport].ltp_state = 1;
					return;
			    }
		     } else {
			    if ((err_cnt[0] == 0x7FFF) ||  (err_cnt[1] == 0x7FFF)  ||  (err_cnt[2] == 0x7FFF))
			    {
					hdmi_mask(HD21_LT_B_reg, ~(HDMI21_P0_HD21_LT_B_reset_mask|_BIT4), HDMI21_P0_HD21_LT_B_reset_mask);
				       hdmi_mask(HD21_LT_G_reg, ~(HDMI21_P0_HD21_LT_G_reset_mask|_BIT4), HDMI21_P0_HD21_LT_G_reset_mask);
					hdmi_mask(HD21_LT_R_reg, ~(HDMI21_P0_HD21_LT_R_reset_mask|_BIT4), HDMI21_P0_HD21_LT_R_reset_mask);
					HDMI_PRINTF("[ltp_chk]change ltp to video pattern\n");
					phy_st[nport].ltp_state = 1;
					return;
			    }
		     }

		    if (lane_num == HDMI_4LANE)
		    		HDMI_PRINTF("[ltp_chk]err_cnt= %d %d %d %d\n",err_cnt[0],err_cnt[1],err_cnt[2],err_cnt[3]);
		    else
				HDMI_PRINTF("[ltp_chk]err_cnt= %d %d %d\n",err_cnt[0],err_cnt[1],err_cnt[2]);

		    for(lane = 0 ; lane < lane_num ; ++lane) {
				#if 0	
			    	if(err_cnt[lane] > 0) {
					if (phy_st[nport].dfe_t[lane].tap1 > 24) {
						HDMI_PRINTF("[adp_1]tap1 is up without adaptive....\n");
						return ;
			    		}
					phy_st[nport].dfe_t[lane].tap1 = phy_st[nport].dfe_t[lane].tap1 + 2;		//tap1 + 2
					lib_hdmi_dfe_init_tap1(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap1);
					HDMI_PRINTF("[adp_1]  tap1 adj (lane=%d tap1=%d)\n",lane,phy_st[nport].dfe_t[lane].tap1);
					if (lane == 0)
						  hdmi_mask(HD21_LT_B_reg, ~(HDMI21_P0_HD21_LT_B_reset_mask|_BIT4), HDMI21_P0_HD21_LT_B_reset_mask);
					else if (lane == 1)
						  hdmi_mask(HD21_LT_G_reg, ~(HDMI21_P0_HD21_LT_G_reset_mask|_BIT4), HDMI21_P0_HD21_LT_G_reset_mask);
					else if (lane == 2)
						hdmi_mask(HD21_LT_R_reg, ~(HDMI21_P0_HD21_LT_R_reset_mask|_BIT4), HDMI21_P0_HD21_LT_R_reset_mask);
					else
						hdmi_mask(HD21_LT_C_reg, ~(HDMI21_P0_HD21_LT_C_reset_mask|_BIT4), HDMI21_P0_HD21_LT_C_reset_mask);
				}
				#else

				if (phy_st[nport].ltp_err_end[lane])
					continue;
				
				if(err_cnt[lane] > 0) {
					if ( phy_st[nport].ltp_err_start[lane] == 0)
					{
						phy_st[nport].ltp_err_pre[lane] = err_cnt[lane];
						phy_st[nport].dfe_t[lane].tap1 = phy_st[nport].dfe_t[lane].tap1 + 2;		//tap1 + 2
						lib_hdmi_dfe_init_tap1(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap1);
						HDMI_PRINTF("[ltp_chk] tap1 adj + 2 (lane=%d tap1=%d)\n",lane,phy_st[nport].dfe_t[lane].tap1);
						phy_st[nport].ltp_err_start[lane] = 1;
						
					} else {
						if (err_cnt[lane] >  phy_st[nport].ltp_err_pre[lane] ) {
							if (phy_st[nport].ltp_err_zero_cnt[lane] > 5) {
								phy_st[nport].dfe_t[lane].tap1 = phy_st[nport].dfe_t[lane].tap1 - 2;		//tap1 - 2
								lib_hdmi_dfe_init_tap1(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap1);
								HDMI_PRINTF("[ltp_chk] tap1 adj - 2 (lane=%d tap1=%d)\n",lane,phy_st[nport].dfe_t[lane].tap1);
								phy_st[nport].ltp_err_end[lane] =1;
							}
						} else if (err_cnt[lane] <=  phy_st[nport].ltp_err_pre[lane] )  {
							phy_st[nport].ltp_err_pre[lane] = err_cnt[lane];
							phy_st[nport].dfe_t[lane].tap1 = phy_st[nport].dfe_t[lane].tap1 + 2;		//tap1 + 2
							lib_hdmi_dfe_init_tap1(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap1);
							HDMI_PRINTF("[ltp_chk] tap1 adj + 2 (lane=%d tap1=%d)\n",lane,phy_st[nport].dfe_t[lane].tap1);
						} 
					}

					if (lane == 0)
						  hdmi_mask(HD21_LT_B_reg, ~(HDMI21_P0_HD21_LT_B_reset_mask|_BIT4), HDMI21_P0_HD21_LT_B_reset_mask);
					else if (lane == 1)
						  hdmi_mask(HD21_LT_G_reg, ~(HDMI21_P0_HD21_LT_G_reset_mask|_BIT4), HDMI21_P0_HD21_LT_G_reset_mask);
					else if (lane == 2)
						hdmi_mask(HD21_LT_R_reg, ~(HDMI21_P0_HD21_LT_R_reset_mask|_BIT4), HDMI21_P0_HD21_LT_R_reset_mask);
					else
						hdmi_mask(HD21_LT_C_reg, ~(HDMI21_P0_HD21_LT_C_reset_mask|_BIT4), HDMI21_P0_HD21_LT_C_reset_mask);	
					
					phy_st[nport].ltp_err_zero_cnt[lane] = 0;
				}
				else {
					phy_st[nport].ltp_err_zero_cnt[lane] = phy_st[nport].ltp_err_zero_cnt[lane]+1; //accumulate zero cnt
				}
				#endif
		    }	
	    }else {
			HDMI_PRINTF("[ltp_chk]ltp no lock\n");
	    }

   		
}
#endif

//check ltp err in link training state
void lib_hdmi_hd21_ltp_phy_adaptive(unsigned char nport, unsigned char lane_num)
{

    unsigned char lock[4];
    unsigned char lock_all;
    unsigned int err_cnt[4];
    unsigned char lane = 0;
    unsigned char timeout = GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT1, HDMI_FLOW_CFG2_LTP_PHY_ADAPTIVE_EN );

 
	    while(timeout >0)
	    {
		     lock[0] = HD21_LT_B_get_lock_valid(hdmi_in(HD21_LT_B_reg));
	   	     lock[1] = HD21_LT_G_get_lock_valid(hdmi_in(HD21_LT_G_reg));
	            lock[2] = HD21_LT_R_get_lock_valid(hdmi_in(HD21_LT_R_reg));
		     if (lane_num == HDMI_4LANE)
	            		lock[3] = HD21_LT_C_get_lock_valid(hdmi_in(HD21_LT_C_reg));

		    if (lane_num == HDMI_4LANE) {
			lock_all = lock[0] & lock[1] & lock[2] & lock[3];
		    	HDMI_PRINTF("[LTP] lock= %d %d %d %d\n",lock[0],lock[1],lock[2],lock[3]);
		    }
		    else {
			lock_all = lock[0] & lock[1] & lock[2];	
			HDMI_PRINTF("[LTP] lock= %d %d %d\n",lock[0],lock[1],lock[2]);
		     }	

		    if (lock_all) {
			    hdmi_mask(HD21_LT_B_reg, ~(HD21_LT_B_popup_mask|_BIT4), HD21_LT_B_popup_mask);
			    hdmi_mask(HD21_LT_G_reg, ~(HD21_LT_G_popup_mask|_BIT4), HD21_LT_G_popup_mask);
			    hdmi_mask(HD21_LT_R_reg, ~(HD21_LT_R_popup_mask|_BIT4), HD21_LT_R_popup_mask);
			     if (lane_num == HDMI_4LANE)
			    		hdmi_mask(HD21_LT_C_reg, ~(HD21_LT_C_popup_mask|_BIT4), HD21_LT_C_popup_mask);

			     udelay(10);

			     if (lane_num == HDMI_4LANE)
			    		HDMI_PRINTF("[LTP] %x %x %x %x\n",hdmi_in(HD21_LT_B_reg),hdmi_in(HD21_LT_G_reg),hdmi_in(HD21_LT_R_reg),hdmi_in(HD21_LT_C_reg));
			     else
				 	HDMI_PRINTF("[LTP] %x %x %x\n",hdmi_in(HD21_LT_B_reg),hdmi_in(HD21_LT_G_reg),hdmi_in(HD21_LT_R_reg));

				 
			    err_cnt[0] = (lock[0]) ? HD21_LT_B_get_err_cnt(hdmi_in(HD21_LT_B_reg)) : 0xFFFFFFFF;
			    err_cnt[1] = (lock[1]) ? HD21_LT_G_get_err_cnt(hdmi_in(HD21_LT_G_reg)) : 0xFFFFFFFF;
			    err_cnt[2] = (lock[2]) ? HD21_LT_R_get_err_cnt(hdmi_in(HD21_LT_R_reg)) : 0xFFFFFFFF;
			    if (lane_num == HDMI_4LANE)
			    	err_cnt[3] = (lock[3]) ? HD21_LT_C_get_err_cnt(hdmi_in(HD21_LT_C_reg)) : 0xFFFFFFFF;

			     if (lane_num == HDMI_4LANE) {
				    HDMI_PRINTF("[LTP] err_cnt= %d %d %d %d\n",err_cnt[0],err_cnt[1],err_cnt[2],err_cnt[3]);

				    if ((err_cnt[0] == 0x7FFF) ||  (err_cnt[1] == 0x7FFF)  ||  (err_cnt[2] == 0x7FFF) || (err_cnt[3] == 0x7FFF))
				    {
						hdmi_mask(HD21_LT_B_reg, ~(HDMI21_P0_HD21_LT_B_reset_mask|_BIT4), HDMI21_P0_HD21_LT_B_reset_mask);
					       hdmi_mask(HD21_LT_G_reg, ~(HDMI21_P0_HD21_LT_G_reset_mask|_BIT4), HDMI21_P0_HD21_LT_G_reset_mask);
						hdmi_mask(HD21_LT_R_reg, ~(HDMI21_P0_HD21_LT_R_reset_mask|_BIT4), HDMI21_P0_HD21_LT_R_reset_mask);
						hdmi_mask(HD21_LT_C_reg, ~(HDMI21_P0_HD21_LT_C_reset_mask|_BIT4), HDMI21_P0_HD21_LT_C_reset_mask);
						continue;
				    }
			     } else {
				    HDMI_PRINTF("[LTP] err_cnt= %d %d %d\n",err_cnt[0],err_cnt[1],err_cnt[2]);

				    if ((err_cnt[0] == 0x7FFF) ||  (err_cnt[1] == 0x7FFF)  ||  (err_cnt[2] == 0x7FFF))
				    {
						hdmi_mask(HD21_LT_B_reg, ~(HDMI21_P0_HD21_LT_B_reset_mask|_BIT4), HDMI21_P0_HD21_LT_B_reset_mask);
					       hdmi_mask(HD21_LT_G_reg, ~(HDMI21_P0_HD21_LT_G_reset_mask|_BIT4), HDMI21_P0_HD21_LT_G_reset_mask);
						hdmi_mask(HD21_LT_R_reg, ~(HDMI21_P0_HD21_LT_R_reset_mask|_BIT4), HDMI21_P0_HD21_LT_R_reset_mask);
						continue;
				    }

			     }
				

			    for(lane = 0 ; lane < lane_num ; ++lane) {
			    	if(err_cnt[lane] > 0) {
					phy_st[nport].dfe_t[lane].tap1 = phy_st[nport].dfe_t[lane].tap1 + 2;		//tap1 + 2
					lib_hdmi_dfe_init_tap1(nport,(1<<lane),phy_st[nport].dfe_t[lane].tap1);
					HDMI_PRINTF("[LTP] tap1 adj (lane=%d tap1=%d)\n",lane,phy_st[nport].dfe_t[lane].tap1);
					if (lane == 0)
						  hdmi_mask(HD21_LT_B_reg, ~(HDMI21_P0_HD21_LT_B_reset_mask|_BIT4), HDMI21_P0_HD21_LT_B_reset_mask);
					else if (lane == 1)
						  hdmi_mask(HD21_LT_G_reg, ~(HDMI21_P0_HD21_LT_G_reset_mask|_BIT4), HDMI21_P0_HD21_LT_G_reset_mask);
					else if (lane == 2)
						hdmi_mask(HD21_LT_R_reg, ~(HDMI21_P0_HD21_LT_R_reset_mask|_BIT4), HDMI21_P0_HD21_LT_R_reset_mask);
					else
						hdmi_mask(HD21_LT_C_reg, ~(HDMI21_P0_HD21_LT_C_reset_mask|_BIT4), HDMI21_P0_HD21_LT_C_reset_mask);
				}
			    }	
		    }else {
				HDMI_PRINTF("[LTP]no lock\n");
		    }
		     mdelay(10);
		     timeout--;
			
	    }
   	

}




void newbase_rxphy_frl_job(unsigned char p)
{
//#if LINK_TRAINING_TX_INFO_DEBUG
	unsigned int err[4];
	unsigned char nport = 0;
//#endif
	//if (p >= HD20_PORT) {
	//	HDMI_EMG("port=%d error %s need to HD21 port\n",p,__func__);
	//	return;
	//}

	if (lib_hdmi_get_fw_debug_bit(DEBUG_14_FRL_FORCE_MODE)) {
		return;
	}
	

	switch (phy_st[p].phy_proc_state) {
	case PHY_PROC_INIT:
		newbase_hdmi21_set_phy(p, phy_st[p].clk,phy_st[p].frl_mode,phy_st[p].lane_num);
		newbase_hdmi_init_disparity_ltp_var(p);
//check after		lib_hdmi_hd21_disparity_symbol_error_disable();
//		newbase_hdmi2p0_inc_scdc_toggle(p);

		phy_st[p].phy_proc_state= PHY_PROC_CLOSE_ADP;
			
	case PHY_PROC_CLOSE_ADP:

		if (newbase_hdmi21_frl_dfe_close(p, phy_st[p].clk,phy_st[p].lane_num,PHY_PROC_CLOSE_ADP)) {

			newbase_hdmi2p0_detect_config(p);

			phy_st[p].phy_proc_state = PHY_PROC_DONE;
			#if HDMI_FRL_TAP0234_ADP_EN
			lib_dfe_tap0234_adap_en(p, phy_st[p].lane_num, ON);
			#endif
		} else {
			phy_st[p].phy_proc_state = PHY_PROC_INIT;
			break;
		}

	case PHY_PROC_DONE:
		if (lib_hdmi_get_fw_debug_bit(DEBUG_18_PHY_DBG_MSG))
			debug_hdmi_dump_msg(p);

		if(GET_FRL_LT_FSM(p)>=LT_FSM_LTSP_PASS) {
//#if LINK_TRAINING_TX_INFO_DEBUG
			if(lt_fsm_status[p].tx_flt_no_train == TRUE)
			{
				lib_hdmi_char_err_get_scdc_ced(p, phy_st[p].frl_mode, err);

				nport = p;
				HDMI_PRINTF("[CEDF] port=%d, scdc err (b,g,r,ck)=%x %x %x %x), scdc_0x10= %x,  scdc_0x40=%x,%x,%x, m_disparity_rst_handle_once=%d\n",
				p,
				err[0],err[1],err[2],err[3],
				lib_hdmi_scdc_read(p,  SCDC_UPDATE_FLAGS),
				lib_hdmi_scdc_read(p,  SCDC_STATUS_FLAGS),
				lib_hdmi_scdc_read(p,  SCDC_STATUS_FLAGS_1),
				lib_hdmi_scdc_read(p,  SCDC_STATUS_FLAGS_2),
				m_disparity_rst_handle_once);
			}
//#endif

#if PHY_ADAPTIVE_BY_LTP_PAT_AFTER_LTP_PASS
			lib_hdmi_hd21_ltp_phy_adaptive_check(p,phy_st[p].lane_num);
#endif			

			//if (phy_st[p].disp_max_cnt <= HD21_MAX_ERR_CHECK_CNT)
			if (m_disparity_rst_handle_once)
			{
				lib_hdmi21_phy_error_handling(p);
			}


		}

		break;

	default:
		phy_st[p].phy_proc_state = PHY_PROC_INIT;
		break;

	}
}


void newbase_rxphy_tmds_job(unsigned char p)
{
	//if (p >= HD20_PORT) {
	//	HDMI_EMG("port error=%d %s need to HD20 port\n",p,__func__);
	//	return;
	//}
	
	switch (phy_st[p].phy_proc_state) {
	case PHY_PROC_INIT:
		if(newbase_hdmi21_set_phy(p, phy_st[p].clk,phy_st[p].frl_mode,phy_st[p].lane_num))
			newbase_rxphy_reset_setphy_proc(p);	
		newbase_hdmi_init_disparity_ltp_var(p);
		newbase_hdmi_reset_meas_counter(p);

		newbase_rxphy_set_apply_clock(p,phy_st[p].clk);
		newbase_hdmi_open_err_detect(p);
//		newbase_hdmi2p0_inc_scdc_toggle(p);

		if (phy_st[p].clk <= clock_bound_45m) {
			phy_st[p].phy_proc_state= PHY_PROC_CLOSE_ADP;
		} else {
			phy_st[p].phy_proc_state= PHY_PROC_RECORD;
		}
		break;
	case PHY_PROC_RECORD:
		if (newbase_hdmi21_tmds_dfe_record(p, phy_st[p].clk)) {
			phy_st[p].phy_proc_state = PHY_PROC_CLOSE_ADP;
			// go to PHY_PROC_CLOSE_ADP without taking a break;
		} else {
			break;
		}
	case PHY_PROC_CLOSE_ADP:
		if (newbase_hdmi21_tmds_dfe_close(p, phy_st[p].clk,phy_st[p].lane_num,PHY_PROC_CLOSE_ADP)) {
			newbase_hdmi2p0_detect_config(p);

			if (GET_FLOW_CFG(HDMI_FLOW_CFG_DFE, HDMI_FLOW_CFG5_FW_LE_SCAN_ENABLE))
				phy_st[p].phy_proc_state = PHY_PROC_FW_LE_SCAN;
			else
				phy_st[p].phy_proc_state = PHY_PROC_DONE;
#if BIST_DFE_SCAN
			if (lib_hdmi_get_fw_debug_bit(DEBUG_30_DFE_SCAN_TEST) && (phy_st[p].clk > clock_bound_45m)) {
				debug_hdmi_dfe_scan(p,phy_st[p].lane_num);
			}
#endif
#if BIST_PHY_SCAN
		if (lib_hdmi_get_fw_debug_bit(DEBUG_22_PHY_SCAN_TEST))
			debug_hdmi_phy_scan(p);
#endif

		} else {
			//phy_st[p].phy_proc_state = PHY_PROC_INIT;
			newbase_rxphy_reset_setphy_proc(p);
		}
		break;

	case PHY_PROC_FW_LE_SCAN:
		if (GET_FLOW_CFG(HDMI_FLOW_CFG_DFE, HDMI_FLOW_CFG5_FW_LE_SCAN_ENABLE))
		{
			if(newbase_hdmi_dfe_fw_le_scan(p)==NEED_RESET_PHY)
			{
				newbase_rxphy_reset_setphy_proc(p);
			}
			else
			{
				phy_st[p].phy_proc_state = PHY_PROC_DONE;
			}

		}
		else
		{
			phy_st[p].phy_proc_state = PHY_PROC_DONE;
		}
		break;

		
	case PHY_PROC_DONE:
		phy_st[p].recovery = 0;

		if (lib_hdmi_get_fw_debug_bit(DEBUG_18_PHY_DBG_MSG))
			debug_hdmi_dump_msg(p);
		#if HDMI20_PHY_ERROR_CORRECTION_EN
			lib_hdmi20_phy_error_handling(p);
		#endif

		break;




	/**********************************************************************/
	//
	//	Belowing are for recovery flow in check_mode stage.
	//
	/**********************************************************************/
	case PHY_PROC_RECOVER_6G_LONGCABLE:
		newbase_hdmi21_tmds_dfe_hi_speed(p, phy_st[p].clk,phy_st[p].lane_num);
		newbase_hdmi_err_detect_add(p);
		phy_st[p].phy_proc_state = PHY_PROC_RECOVER_6G_LONGCABLE_RECORD;
		break;
	case PHY_PROC_RECOVER_6G_LONGCABLE_RECORD:
		if (newbase_hdmi21_tmds_dfe_record(p, phy_st[p].clk)) {
			phy_st[p].phy_proc_state = PHY_PROC_RECOVER_6G_LONGCABLE_CLOSE_ADP;
			// go to PHY_PROC_RECOVER_HI_CLOSE_ADP;
		} else {
			break;
		}
	case PHY_PROC_RECOVER_6G_LONGCABLE_CLOSE_ADP:
		newbase_hdmi21_tmds_dfe_close(p, phy_st[p].clk, phy_st[p].lane_num,PHY_PROC_RECOVER_6G_LONGCABLE_CLOSE_ADP);
		phy_st[p].phy_proc_state = PHY_PROC_DONE;
		break;

	case PHY_PROC_RECOVER_HI:
		newbase_hdmi21_tmds_dfe_hi_speed(p, phy_st[p].clk,phy_st[p].lane_num);
		newbase_hdmi_err_detect_add(p);
		phy_st[p].phy_proc_state = PHY_PROC_RECORD;
		break;


	case PHY_PROC_RECOVER_MI:
		newbase_hdmi21_tmds_dfe_mi_speed(p, phy_st[p].clk,phy_st[p].lane_num);
		newbase_hdmi_err_detect_add(p);
		phy_st[p].phy_proc_state = PHY_PROC_RECORD;
		break;

	case PHY_PROC_RECOVER_MID_BAND_LONGCABLE:
		newbase_hdmi21_tmds_dfe_mid_adapthve(p, phy_st[p].clk);
		newbase_hdmi_err_detect_add(p);
		phy_st[p].phy_proc_state = PHY_PROC_DONE;
		break;

	default:
		phy_st[p].phy_proc_state = PHY_PROC_INIT;
		break;
	}
}


#if 0

void newbase_rxphy_force_clear_clk_st(unsigned char port)
{
	// case 1: clk=1080p50->1080p60, timing changed, but clock is the same, then force to re-set phy
	// case 2: clk=4k2kp60->1080p60, timing changed, but clock is the same, then force to re-set phy (also need to clear SCDC)
	//
#if BIST_DFE_SCAN
	if (lib_hdmi_get_fw_debug_bit(DEBUG_30_DFE_SCAN_TEST)) return;
#endif
	newbase_rxphy_init_struct(port);
}



unsigned int newbase_rxphy_get_clk(unsigned char port)
{
	return phy_st[port].clk;
}
unsigned int newbase_rxphy_get_clk_pre(unsigned char port)
{
	return phy_st[port].clk_pre;
}
#endif


#if 0
unsigned char newbase_rxphy_is_clk_stable(unsigned char port)
{
	//if frl mode no check clk, fix me
	if (hdmi_rx[port].hdmi_2p1_en) {
		return 1;
	}
	return phy_st[port].clk_stable;
}

unsigned char newbase_rxphy_is_tmds_mode(unsigned port)
{
	if (phy_st[port].frl_mode == MODE_TMDS)
		return 1;
	else 
		return 0;

}
#endif


#if 0
unsigned char newbase_rxphy_get_setphy_done(unsigned char port)
{
	return (phy_st[port].phy_proc_state == PHY_PROC_DONE);
}

void newbase_rxphy_reset_setphy_proc(unsigned char port)
{
	phy_st[port].clk_pre = 0;
	phy_st[port].clk= 0;
	phy_st[port].clk_debounce_count= 0;
	phy_st[port].clk_unstable_count = 0;
	phy_st[port].clk_stable = 0;

	//for frl mode
	phy_st[port].frl_mode = 0;
	phy_st[port].pre_frl_mode = 0;
	phy_st[port].phy_proc_state = PHY_PROC_INIT;
	HDMI_PRINTF("%s(%d)\n", __func__, port);
}

void newbase_hdmi_dfe_recovery_6g_long_cable(unsigned char port)
{
	phy_st[port].phy_proc_state = PHY_PROC_RECOVER_6G_LONGCABLE;
}

void newbase_hdmi_dfe_recovery_mi_speed(unsigned char port)
{
	phy_st[port].phy_proc_state = PHY_PROC_RECOVER_MI;
}

void newbase_hdmi_dfe_recovery_midband_long_cable(unsigned char port)
{
	phy_st[port].phy_proc_state = PHY_PROC_RECOVER_MID_BAND_LONGCABLE;

}

#endif

#if 0
void newbase_hdmi_set_frl_info(unsigned p, unsigned char frl_mode)
{
	switch(frl_mode)
	{
	case 0:
		phy_st[p].lane_num = HDMI_3LANE;
		phy_st[p].frl_mode = MODE_TMDS;
		break;
	case 1:
		phy_st[p].lane_num = HDMI_3LANE;
		phy_st[p].frl_mode = MODE_FRL_3G_3_LANE;
		phy_st[p].clk = FRL_3G;
		break;
	case 2:
		phy_st[p].lane_num = HDMI_3LANE;
		phy_st[p].frl_mode = MODE_FRL_6G_3_LANE;
		phy_st[p].clk = FRL_6G;
		break;
	case 3:   // FRL:6G/4Ln
		phy_st[p].lane_num = HDMI_4LANE;
		phy_st[p].frl_mode = MODE_FRL_6G_4_LANE;
		phy_st[p].clk = FRL_6G;
		break;
	case 4:   // FRL:6G/4Ln
		phy_st[p].lane_num = HDMI_4LANE;
		phy_st[p].frl_mode = MODE_FRL_8G_4_LANE;
		phy_st[p].clk = FRL_8G;
		break;
	case 5:   // FRL:6G/4Ln
		phy_st[p].lane_num = HDMI_4LANE;
		phy_st[p].frl_mode = MODE_FRL_10G_4_LANE;
		phy_st[p].clk = FRL_10G;
		break;
	case 6:   // FRL:6G/4Ln
		phy_st[p].lane_num = HDMI_4LANE;
		phy_st[p].frl_mode = MODE_FRL_12G_4_LANE;
		phy_st[p].clk = FRL_12G;
		break;
	default:
		phy_st[p].lane_num = HDMI_3LANE;
		phy_st[p].frl_mode = MODE_TMDS;
		break;
	}



}
#endif

void newbase_hdmi21_set_koffset_flag(unsigned char port,unsigned char en)
{
	phy_st[port].do_koffset = en;
}

int newbase_hdmi21_get_koffset_flag(unsigned char port)
{
	return phy_st[port].do_koffset;
}



void newbase_hdmi21_set_phy_frl_mode(unsigned port, unsigned char frl_mode)
{
	
	phy_st[port].frl_mode = frl_mode;
	
	switch (frl_mode)
	{
		case MODE_FRL_3G_3_LANE:
			phy_st[port].lane_num = HDMI_3LANE;
			break;
		case MODE_FRL_6G_3_LANE:
			phy_st[port].lane_num = HDMI_3LANE;
			break;
		case MODE_FRL_6G_4_LANE:
			phy_st[port].lane_num = HDMI_4LANE;
			break;
		case MODE_FRL_8G_4_LANE:
			phy_st[port].lane_num = HDMI_4LANE;
			break;
		case MODE_FRL_10G_4_LANE:
			phy_st[port].lane_num = HDMI_4LANE;
			break;
		case MODE_FRL_12G_4_LANE:
			phy_st[port].lane_num = HDMI_4LANE;
			break;
		default:
			phy_st[port].lane_num = HDMI_3LANE;
			break;
	}

}



int newbase_hdmi21_rxphy_get_frl_info(unsigned char port, unsigned char frl_mode)
{
	unsigned int b;
	
	switch(frl_mode)
	{
		case MODE_FRL_3G_3_LANE:
			b = FRL_3G_B;
			break;
		case MODE_FRL_6G_3_LANE:
			b = FRL_6G_B;
			break;
		case MODE_FRL_6G_4_LANE:
			b = FRL_6G_B;
			break;
		case MODE_FRL_8G_4_LANE:
			b = FRL_8G_B;
			break;
		case MODE_FRL_10G_4_LANE:
			b = FRL_10G_B;
			break;
		case MODE_FRL_12G_4_LANE:
			b = FRL_12G_B;
			break;	
		default:
			b = 0;
			break;		
	}

	return b;

}

#if 0
void newbase_hdmi_set_eq_mode(unsigned char nport, unsigned char mode)
{
	phy_fac_eq_st[nport].eq_mode = mode;

	if (phy_fac_eq_st[nport].eq_mode == EQ_MANUAL_OFF) {
		phy_st[nport].clk_pre = 0;
		phy_st[nport].clk= 0;
		phy_st[nport].clk_debounce_count= 0;
		phy_st[nport].clk_unstable_count = 0;
		phy_st[nport].clk_stable = 0;
		phy_st[nport].phy_proc_state = PHY_PROC_INIT;
	}

}


void newbase_hdmi_manual_eq_ch(unsigned char nport,unsigned char ch,unsigned char eq_ch)
{
//	unsigned char bgrc;
//	unsigned char mode;

#if HDMI_DFE_BRSWAP
	if (nport == HDMI_PORT3)  {
		if (ch == 0) {
			phy_fac_eq_st[nport].manual_eq[2] = eq_ch;
		}
		else if (ch == 1) {
			phy_fac_eq_st[nport].manual_eq[1] = eq_ch;
		}
		else if (ch == 2) {
			phy_fac_eq_st[nport].manual_eq[0] = eq_ch;

		} else {
			return;
		}

	} else {

	if (ch == 0) {
		phy_fac_eq_st[nport].manual_eq[0] = eq_ch;
	}
	else if (ch == 1) {
		phy_fac_eq_st[nport].manual_eq[1] = eq_ch;
	}
	else if (ch == 2) {
		phy_fac_eq_st[nport].manual_eq[2] = eq_ch;

	} else {
		return;
	}
	}

#else
	if (ch == 0) {
		phy_fac_eq_st[nport].manual_eq[0] = eq_ch;
	}
	else if (ch == 1) {
		phy_fac_eq_st[nport].manual_eq[1] = eq_ch;
	}
	else if (ch == 2) {
		phy_fac_eq_st[nport].manual_eq[2] = eq_ch;

	} else {
		return;
	}
#endif

	phy_st[nport].clk_pre = 0;
	phy_st[nport].clk= 0;
	phy_st[nport].clk_debounce_count= 0;
	phy_st[nport].clk_unstable_count = 0;
	phy_st[nport].clk_stable = 0;
	phy_st[nport].phy_proc_state = PHY_PROC_INIT;
	

}



void newbase_hdmi_manual_eq(unsigned char nport, unsigned char eq_ch0, unsigned char eq_ch1, unsigned char eq_ch2)
{ 
	unsigned char bgrc;
	unsigned char mode;

	//phy_st[nport].dfe_t[0].le ~ phy_st[nport].dfe_t[3].le
	/*
	if (lane_mode == HDMI_4LANE)
		bgrc = LN_ALL;
	else
	*/
	if (phy_fac_eq_st[nport].eq_mode == EQ_MANUAL_OFF)
		return;
	

	phy_fac_eq_st[nport].manual_eq[0] = eq_ch0;
	phy_fac_eq_st[nport].manual_eq[1] = eq_ch1;
	phy_fac_eq_st[nport].manual_eq[2] = eq_ch2;
	
	bgrc = (LN_R|LN_G|LN_B);
	mode = DFE_P0_REG_DFE_MODE_P0_get_adapt_mode_p0(hdmi_in(DFE_REG_DFE_MODE_reg));


	if (mode == 2) {
/*
		if (bgrc&LN_CK) {
			hdmi_mask(DFE_REG_DFE_EN_L3_reg,
				~(DFE_HD21_REG_DFE_EN_L3_P0_le_init_lane3_p0_mask), (DFE_HD21_REG_DFE_EN_L3_P0_le_init_lane3_p0(value)));
		}
*/
		
		//le 0~24
		if (bgrc&LN_R) {
			hdmi_mask(DFE_REG_DFE_EN_L2_reg,
				~(DFE_P0_REG_DFE_EN_L2_P0_le_init_lane2_p0_mask), (DFE_P0_REG_DFE_EN_L2_P0_le_init_lane2_p0(eq_ch2)));
		}
		if (bgrc&LN_G) {
			hdmi_mask(DFE_REG_DFE_EN_L1_reg,
				~(DFE_P0_REG_DFE_EN_L1_P0_le_init_lane1_p0_mask), (DFE_P0_REG_DFE_EN_L1_P0_le_init_lane1_p0(eq_ch1)));
		}
		if (bgrc&LN_B) {
			hdmi_mask(DFE_REG_DFE_EN_L0_reg,
				~(DFE_P0_REG_DFE_EN_L0_P0_le_init_lane0_p0_mask), (DFE_P0_REG_DFE_EN_L0_P0_le_init_lane0_p0(eq_ch0)));
		}

		lib_hdmi_dfe_load_in_adp(nport, bgrc, LOAD_IN_INIT_LE);

	}
	 else if (mode == 3) {
		
		//mode 3
			//0~24
		/*	
		if (bgrc&LN_CK) {
			hdmi_mask(DFE_REG_DFE_INIT0_L3_reg,
				~(DFE_HD21_REG_DFE_INIT0_L3_P0_tap1_init_lane3_p0_mask), (DFE_HD21_REG_DFE_INIT0_L3_P0_tap1_init_lane3_p0(value)));
		}
		*/
		
		if (bgrc&LN_R) {
			hdmi_mask(DFE_REG_DFE_INIT0_L2_reg,
				~(DFE_P0_REG_DFE_INIT0_L2_P0_tap1_init_lane2_p0_mask), (DFE_P0_REG_DFE_INIT0_L2_P0_tap1_init_lane2_p0(eq_ch2)));
		}
		if (bgrc&LN_G) {
			hdmi_mask(DFE_REG_DFE_INIT0_L1_reg,
				~(DFE_P0_REG_DFE_INIT0_L1_P0_tap1_init_lane1_p0_mask), (DFE_P0_REG_DFE_INIT0_L1_P0_tap1_init_lane1_p0(eq_ch1)));
		}
		if (bgrc&LN_B) {
			hdmi_mask(DFE_REG_DFE_INIT0_L0_reg,
				~(DFE_P0_REG_DFE_INIT0_L0_P0_tap1_init_lane0_p0_mask), (DFE_P0_REG_DFE_INIT0_L0_P0_tap1_init_lane0_p0(eq_ch0)));
		}

		lib_hdmi_dfe_load_in_adp(nport, bgrc, LOAD_IN_INIT_TAP1);
	}	
	else {
		//do nothing
	}


}

#endif


unsigned char newbase_hdmi21_set_phy(unsigned char nport, unsigned int b, unsigned char frl_mode,unsigned char lane)
{
	FRL_VALUE_PARAM_T FRL_Timing;
	TMDS_TIMING_PARAM_T TMDS_Timing= TMDS_5p94G;	
	unsigned int Delta_b=30;//30 ~=3MHz TMDS_CLK = (b x 27)/256
	HDMI_WARN("%s start[p%d]b=%d frl=%d lane=%d\n", __func__, nport, b,frl_mode,lane);
	if (lib_hdmi_get_fw_debug_bit(DEBUG_18_PHY_DBG_MSG))
	{
		PHY_DBG_MSG=1;
	}
	else
	{
		PHY_DBG_MSG=0;
	}
	if (b == 0) return 1;

#ifdef HDMI_FIX_HDMI_POWER_SAVING
	newbase_hdmi_reset_power_saving_state(nport);    // reset power saving state
#endif

	lib_hdmi_mac_reset(nport);

	//Need to be modify by input timing index
	switch(b)
	{
		case FRL_12G_B: FRL_Timing = FRL_12G;break;	
		case FRL_10G_B: FRL_Timing = FRL_10G;break;	
		case FRL_8G_B:	FRL_Timing = FRL_8G;break;	
		case FRL_6G_B:	FRL_Timing = FRL_6G;break;	
		case FRL_3G_B:	FRL_Timing = FRL_3G;break;	
		default: FRL_Timing = FRL_12G;break;
	}

////////////////
//TMDS clock define
//#define TMDS_6G		5688
//#define TMDS_5G		4740
//#define TMDS_4G		3792
//#define TMDS_3G		2844
//#define TMDS_1p5G	1422
//#define TMDS_742M    704
//#define TMDS_270M    255
//// TMDS_CLK = (b x 27)/256 ,  b= TMDS_CLK*256/27

	if (frl_mode == 0)
	{
		HDMI_WARN("HDMI Port[%d] frl_mode == MODE_TMDS!! (b=%d)\n", nport, b);

		if (b > (4214+Delta_b)) 
		{
			TMDS_Timing= TMDS_5p94G;
			HDMI_WARN("HDMI Port[%d]  TMDS_5_94G! (b=%d)\n", nport, b);
		} 
		else if ((b > (3520+Delta_b)) && (b <= (4214+Delta_b))) 
		{
			TMDS_Timing= TMDS_4p445G;
			HDMI_WARN("HDMI Port[%d]  TMDS_4_445G! (b=%d)\n", nport, b);
		} 

		else if ((b > (3168+Delta_b)) && (b <= (3520+Delta_b))) 
		{
			TMDS_Timing= TMDS_3p7125G;
			HDMI_WARN("HDMI Port[%d]  TMDS_3_7125G! (b=%d)\n", nport, b);
		} 
		else if ((b > (2122+Delta_b)) && (b <= (3816+Delta_b))) 
		{
			TMDS_Timing= TMDS_2p97G;
			HDMI_WARN("HDMI Port[%d]  TMDS_2_97G! (b=%d)\n", nport, b);
		} 
		else if ((b > (1760+Delta_b)) && (b <= (2122+Delta_b))) 
		{
			TMDS_Timing= TMDS_2p2275G;
			HDMI_WARN("HDMI Port[%d]  TMDS_2_2275G! (b=%d)\n", nport, b);
		} 

		else if ((b > (1408+Delta_b)) && (b <= (1760+Delta_b))) 
		{
			TMDS_Timing= TMDS_1p85625G;
			HDMI_WARN("HDMI Port[%d]  TMDS_1_85625G! (b=%d)\n", nport, b);
		} 
		else if ((b > (1056+Delta_b)) && (b <= (1408+Delta_b))) 
		{
			TMDS_Timing= TMDS_1p485G;
			HDMI_WARN("HDMI Port[%d]  TMDS_1_485G! (b=%d)\n", nport, b);
		} 
		else if ((b > (968+Delta_b)) && (b <=(1056+Delta_b))) 
		{
			TMDS_Timing= TMDS_1p11375G;
			HDMI_WARN("HDMI Port[%d]  TMDS_1_11375G! (b=%d)\n", nport, b);
		} 
		else if ((b > (792+Delta_b)) && (b <=(968+Delta_b)))
		{
			TMDS_Timing= TMDS_0p928125G;
			HDMI_WARN("HDMI Port[%d]  TMDS_0_928125G! (b=%d)\n", nport, b);
		}
		else if ((b > (512+Delta_b)) && (b <=(792+Delta_b)))
		{
			TMDS_Timing= TMDS_0p742G;
			HDMI_WARN("HDMI Port[%d]  TMDS_0_742G! (b=%d)\n", nport, b);
		}
		else if ((b > (384+Delta_b)) && (b <=(512+Delta_b))) 
		{
			TMDS_Timing= TMDS_0p54G;
			HDMI_WARN("HDMI Port[%d]  TMDS_0_54G! (b=%d)\n", nport, b);
		} 
		else if ((b > (352+Delta_b)) && (b <=(384+Delta_b)))
		{
			TMDS_Timing= TMDS_0p405G;
			HDMI_WARN("HDMI Port[%d]  TMDS_0_405G! (b=%d)\n", nport, b);
		}

		else if ((b > (252+Delta_b)) && (b <=(352+Delta_b)))
		{
			TMDS_Timing= TMDS_0p3375G;
			HDMI_WARN("HDMI Port[%d]  TMDS_0_3375G! (b=%d)\n", nport, b);
		}
		else if ((b > (238+Delta_b)) && (b <=(252+Delta_b)))
		{
			TMDS_Timing= TMDS_0p27G;
			HDMI_WARN("HDMI Port[%d]  TMDS_0_27G! (b=%d)\n", nport, b);
		}
		else		
		{
			TMDS_Timing= TMDS_0p25G;
			HDMI_WARN("HDMI Port[%d]  TMDS_0_25G! (b=%d)\n", nport, b);
		}

	}

	udelay(1);

#ifdef FORCE_SCRIPT_BASED_PHY_SETTING  // for Bringup only....
	if (frl_mode == MODE_TMDS)
	{
		//if((TMDS_Timing== TMDS_Timing_6G)||(TMDS_Timing== TMDS_Timing_3G)||(TMDS_Timing== TMDS_Timing_1P5G)||(TMDS_Timing== TMDS_Timing_742M))
		if(0)
		{
	   		//newbase_hdmi_set_tmds_phy_script(nport, TMDS_Timing);
			//Fixed_DFE_LE_TAP0(nport,b,lane);
			newbase_hdmi21_dump_dfe_para(nport,lane);		
			goto set_phy_done;
		}
	}
	else
	{
		//if((FRL_Timing== FRL_Timing_3G)||(FRL_Timing== FRL_Timing_12G)||(FRL_Timing== FRL_Timing_6G))
		if(0)
		{
	   		//newbase_hdmi_set_phy_script(nport, frl_mode);
			newbase_hdmi21_dump_dfe_para(nport,lane);		
			goto set_phy_done;
		}
	}
#endif

#ifdef CONFIG_POWER_SAVING_MODE
	lib_hdmi21_cdr_rst(nport, 0, frl_mode,lane);
	lib_hdmi21_demux_ck_vcopll_rst(nport, 0, lane);
	lib_hdmi21_ac_couple_power_en(nport,1);
	lib_hdmi21_eq_pi_power_en(nport,1,frl_mode,lane);
//	lib_hdmi21_cmu_rst(port, 1);
	lib_hdmi21_cmu_pll_en(nport, 1,frl_mode);
#endif

	if (frl_mode == MODE_TMDS)
	{
		TMDS_Main_Seq(nport,TMDS_Timing);
	}
	else
	{

		FRL_Main_Seq(nport,FRL_Timing);
	}
	//Dump DFE
	newbase_hdmi21_dump_dfe_para(nport,lane);	

/////////////////////////////////////////////////////////////////////////////////////////////

//set_phy_done:

	if(newbase_hdcp_get_auth_mode(nport) == HDCP_OFF || newbase_hdcp_get_auth_mode(nport) == NO_HDCP )
	{// No AKE_INIT of HDCP22, open HDCP14 and HDCP22 at the same time
		HDMI_WARN("HDMI Port[%d] HDCP Enable after set phy!! (b=%d)\n", nport, b);
		newbase_hdmi_switch_hdcp_mode(nport, NO_HDCP);
		newbase_hdmi_reset_hdcp_once(nport, FALSE);
	}
	else
	{
		HDMI_WARN("HDMI Port[%d] HDCP already start before set phy!! (b=%d)\n", nport, b);
	}



	
	lib_hdmi_mac_release(nport, frl_mode);

	return 0;
}

unsigned char newbase_hdmi21_tmds_dfe_record(unsigned char nport, unsigned int clk)
{
	if (lib_hdmi_dfe_is_record_enable(nport)) {
		lib_hdmi_dfe_record_enable(nport, 0);
		return TRUE; // stop recocrding
	} else {
		lib_hdmi_dfe_record_enable(nport, 1);
		return FALSE; // start recoding min/max
	}
}


unsigned char newbase_hdmi21_frl_dfe_close(unsigned char nport, unsigned int clk, unsigned char lane_mode,unsigned char phy_state)
{
	unsigned char lane;
	
	//lib_hdmi_set_dfe_close(nport,lane_mode);
	
	for (lane=0; lane< lane_mode; lane++) {
		phy_st[nport].dfe_t[lane].tap0max = lib_hdmi_dfe_get_tap0max(nport, lane);
		phy_st[nport].dfe_t[lane].tap0min = lib_hdmi_dfe_get_tap0min(nport, lane);
		phy_st[nport].dfe_t[lane].vth = lib_hdmi_dfe_get_vth(nport, lane);
		phy_st[nport].dfe_t[lane].tap0 = lib_hdmi_dfe_get_tap0(nport, lane);
		phy_st[nport].dfe_t[lane].tap1 = lib_hdmi_dfe_get_tap1(nport, lane);
		phy_st[nport].dfe_t[lane].tap2 = lib_hdmi_dfe_get_tap2(nport, lane);
		phy_st[nport].dfe_t[lane].tap3 = lib_hdmi_dfe_get_tap3(nport, lane);
		phy_st[nport].dfe_t[lane].tap4 = lib_hdmi_dfe_get_tap4(nport, lane);
		phy_st[nport].dfe_t[lane].le = lib_hdmi_dfe_get_le(nport, lane);
		phy_st[nport].dfe_t[lane].lemax = lib_hdmi_dfe_get_lemax(nport, lane);
		phy_st[nport].dfe_t[lane].lemin = lib_hdmi_dfe_get_lemin(nport, lane);
	}

	newbase_hdmi21_dump_dfe_para(nport,lane_mode);
#if HDMI_FRL_TAP0234_ADP_EN
	lib_hdmi21_dfe_power(nport, 1,lane_mode); //tap0234 adapt for CTS
#else
	lib_hdmi21_dfe_power(nport, 0,lane_mode);
#endif
	return TRUE;


}



unsigned char newbase_hdmi21_tmds_dfe_close(unsigned char nport, unsigned int clk, unsigned char lane_mode,unsigned char phy_state)
{
	unsigned char lane;
	
	lib_hdmi_set_dfe_close(nport,lane_mode);

	switch (phy_state) {
	case PHY_PROC_CLOSE_ADP:
		/*
		if (clk > 2900) { // > 3G
			lib_hdmi_load_le_max(nport,lane_mode);
		} else*/ {
			lib_hdmi_dfe_examine_le_coef(nport, clk,lane_mode);
		}

		newbase_hdmi_tap2_adjust_with_le(nport,clk,lane_mode);
			
		if (newbase_hdmi_dfe_threshold_check(nport,clk,lane_mode) && (phy_st[nport].dfe_thr_chk_cnt < 30)) {
			phy_st[nport].dfe_thr_chk_cnt++;
			return FALSE;
		}

		break;
	case PHY_PROC_RECOVER_6G_LONGCABLE_CLOSE_ADP:
		newbase_hdmi21_tmds_dfe_6g_long_cable_patch(nport); //check FRL mode need this patch or not ?
		break;
	default:
		break;
	}

	
	if (phy_fac_eq_st[nport].eq_mode == EQ_MANUAL_ON) {		
		newbase_hdmi_manual_eq(nport,	phy_fac_eq_st[nport].manual_eq[0] ,phy_fac_eq_st[nport].manual_eq[1] ,phy_fac_eq_st[nport].manual_eq[2]); 
	}
	
	for (lane=0; lane< lane_mode; lane++) {
		phy_st[nport].dfe_t[lane].tap0max = lib_hdmi_dfe_get_tap0max(nport, lane);
		phy_st[nport].dfe_t[lane].tap0min = lib_hdmi_dfe_get_tap0min(nport, lane);
		phy_st[nport].dfe_t[lane].vth = lib_hdmi_dfe_get_vth(nport, lane);
		phy_st[nport].dfe_t[lane].tap0 = lib_hdmi_dfe_get_tap0(nport, lane);
		phy_st[nport].dfe_t[lane].tap1 = lib_hdmi_dfe_get_tap1(nport, lane);
		phy_st[nport].dfe_t[lane].tap2 = lib_hdmi_dfe_get_tap2(nport, lane);
		phy_st[nport].dfe_t[lane].tap3 = 0;
		phy_st[nport].dfe_t[lane].tap4 = 0;
		phy_st[nport].dfe_t[lane].le = lib_hdmi_dfe_get_le(nport, lane);
		phy_st[nport].dfe_t[lane].lemax = lib_hdmi_dfe_get_lemax(nport, lane);
		phy_st[nport].dfe_t[lane].lemin = lib_hdmi_dfe_get_lemin(nport, lane);
	}

	newbase_hdmi21_dump_dfe_para(nport,lane_mode);

	return TRUE;
}



void newbase_hdmi21_tmds_dfe_hi_speed(unsigned char nport, unsigned int b_clk,unsigned char lane_mode)
{  
	HDMI_PRINTF("%s\n", __func__);
#if 0
	lib_hdmi21_dfe_power(nport, 1,lane_mode);
	lib_hdmi21_dfe_param_init(nport,b_clk,2,lane_mode);

	lib_dfe_tap0_le_adp_en(nport,lane_mode,1);
	udelay(1);
	lib_hdmi_dfe_record_enable(nport, 1);
	udelay(500);
	lib_hdmi_dfe_record_enable(nport, 0);
	lib_dfe_tap0_disable(nport,lane_mode);
	lib_hdmi_load_tap0_max(nport,lane_mode);
	udelay(100);
	lib_dfe_le_disable(nport,lane_mode);
	lib_hdmi21_dump_dfe_tap0_le(nport,lane_mode);

	if (b_clk > 2700) {  // 3G need tap2 but below not need
		// ENABLE  TAP0~TAP2
		hdmi_mask(DFE_REG_DFE_EN_L0_reg,~(REG_dfe_adapt_en_lane0_TAP0|REG_dfe_adapt_en_lane0_TAP1|REG_dfe_adapt_en_lane0_TAP2),(REG_dfe_adapt_en_lane0_TAP0|REG_dfe_adapt_en_lane0_TAP1|REG_dfe_adapt_en_lane0_TAP2));
		hdmi_mask(DFE_REG_DFE_EN_L1_reg,~(REG_dfe_adapt_en_lane1_TAP0|REG_dfe_adapt_en_lane1_TAP1|REG_dfe_adapt_en_lane1_TAP2),(REG_dfe_adapt_en_lane1_TAP0|REG_dfe_adapt_en_lane1_TAP1|REG_dfe_adapt_en_lane1_TAP2));
		hdmi_mask(DFE_REG_DFE_EN_L2_reg,~(REG_dfe_adapt_en_lane2_TAP0|REG_dfe_adapt_en_lane2_TAP1|REG_dfe_adapt_en_lane2_TAP2),(REG_dfe_adapt_en_lane2_TAP0|REG_dfe_adapt_en_lane2_TAP1|REG_dfe_adapt_en_lane2_TAP2));
		if (lane_mode == HDMI_4LANE)
				hdmi_mask(DFE_REG_DFE_EN_L3_reg,~(REG_dfe_adapt_en_lane3_TAP0|REG_dfe_adapt_en_lane3_TAP1|REG_dfe_adapt_en_lane3_TAP2),(REG_dfe_adapt_en_lane3_TAP0|REG_dfe_adapt_en_lane3_TAP1|REG_dfe_adapt_en_lane3_TAP2));

	} else {

		// ENABLE  TAP0~TAP1
		hdmi_mask(DFE_REG_DFE_EN_L0_reg,~(REG_dfe_adapt_en_lane0_TAP0|REG_dfe_adapt_en_lane0_TAP1),(REG_dfe_adapt_en_lane0_TAP0|REG_dfe_adapt_en_lane0_TAP1));
		hdmi_mask(DFE_REG_DFE_EN_L1_reg,~(REG_dfe_adapt_en_lane1_TAP0|REG_dfe_adapt_en_lane1_TAP1),(REG_dfe_adapt_en_lane1_TAP0|REG_dfe_adapt_en_lane1_TAP1));
		hdmi_mask(DFE_REG_DFE_EN_L2_reg,~(REG_dfe_adapt_en_lane2_TAP0|REG_dfe_adapt_en_lane2_TAP1),(REG_dfe_adapt_en_lane2_TAP0|REG_dfe_adapt_en_lane2_TAP1));
		if (lane_mode == HDMI_4LANE)
				hdmi_mask(DFE_REG_DFE_EN_L3_reg,~(REG_dfe_adapt_en_lane3_TAP0|REG_dfe_adapt_en_lane3_TAP1),(REG_dfe_adapt_en_lane3_TAP0|REG_dfe_adapt_en_lane3_TAP1));

	}

	newbase_hdmi21_set_koffset_flag(nport,0); //recover not do koffset , becasue input off will mute display
#endif	
}

void newbase_hdmi21_tmds_dfe_6g_long_cable_patch(unsigned char nport)
{   
#if 0
	signed char tap2;
	unsigned char lemax;

	tap2 = lib_hdmi_dfe_get_tap2(nport, 1);
	lib_hdmi_dfe_init_tap2(nport, LN_G, MIN(tap2+6, 10));

	lemax = lib_hdmi_dfe_get_lemax(nport, 1);
	lemax += lib_hdmi_dfe_get_tap1max(nport, 1);
	lib_hdmi_dfe_init_tap1(nport, LN_G, MIN(lemax+4, 63));

	HDMI_EMG("%s end\n", __func__);
#endif	
}


void newbase_hdmi21_tmds_dfe_mi_speed(unsigned char nport, unsigned int b_clk,unsigned char lane_mode)
{
	HDMI_PRINTF("%s\n", __func__);
#if 0
	lib_hdmi21_dfe_power(nport, 1,lane_mode);
	lib_hdmi21_dfe_param_init(nport,b_clk,2,lane_mode);

	lib_dfe_tap0_le_adp_en(nport,lane_mode,1);
	udelay(1);
	lib_hdmi_dfe_record_enable(nport, 1);
	udelay(500);
	lib_hdmi_dfe_record_enable(nport, 0);
	lib_dfe_tap0_disable(nport,lane_mode);
	lib_hdmi_load_tap0_max(nport,lane_mode);
	udelay(100);
	lib_dfe_le_disable(nport,lane_mode);
	lib_hdmi21_dump_dfe_tap0_le(nport,lane_mode);

	if (b_clk > 2700) {  // 3G need tap2 but below not need
		// ENABLE  TAP0~TAP2
		hdmi_mask(DFE_REG_DFE_EN_L0_reg,~(REG_dfe_adapt_en_lane0_TAP0|REG_dfe_adapt_en_lane0_TAP1|REG_dfe_adapt_en_lane0_TAP2),(REG_dfe_adapt_en_lane0_TAP0|REG_dfe_adapt_en_lane0_TAP1|REG_dfe_adapt_en_lane0_TAP2));
		hdmi_mask(DFE_REG_DFE_EN_L1_reg,~(REG_dfe_adapt_en_lane1_TAP0|REG_dfe_adapt_en_lane1_TAP1|REG_dfe_adapt_en_lane1_TAP2),(REG_dfe_adapt_en_lane1_TAP0|REG_dfe_adapt_en_lane1_TAP1|REG_dfe_adapt_en_lane1_TAP2));
		hdmi_mask(DFE_REG_DFE_EN_L2_reg,~(REG_dfe_adapt_en_lane2_TAP0|REG_dfe_adapt_en_lane2_TAP1|REG_dfe_adapt_en_lane2_TAP2),(REG_dfe_adapt_en_lane2_TAP0|REG_dfe_adapt_en_lane2_TAP1|REG_dfe_adapt_en_lane2_TAP2));
		if (lane_mode == HDMI_4LANE)
				hdmi_mask(DFE_REG_DFE_EN_L3_reg,~(REG_dfe_adapt_en_lane3_TAP0|REG_dfe_adapt_en_lane3_TAP1|REG_dfe_adapt_en_lane3_TAP2),(REG_dfe_adapt_en_lane3_TAP0|REG_dfe_adapt_en_lane3_TAP1|REG_dfe_adapt_en_lane3_TAP2));

	} else {

		// ENABLE  TAP0~TAP1
		hdmi_mask(DFE_REG_DFE_EN_L0_reg,~(REG_dfe_adapt_en_lane0_TAP0|REG_dfe_adapt_en_lane0_TAP1),(REG_dfe_adapt_en_lane0_TAP0|REG_dfe_adapt_en_lane0_TAP1));
		hdmi_mask(DFE_REG_DFE_EN_L1_reg,~(REG_dfe_adapt_en_lane1_TAP0|REG_dfe_adapt_en_lane1_TAP1),(REG_dfe_adapt_en_lane1_TAP0|REG_dfe_adapt_en_lane1_TAP1));
		hdmi_mask(DFE_REG_DFE_EN_L2_reg,~(REG_dfe_adapt_en_lane2_TAP0|REG_dfe_adapt_en_lane2_TAP1),(REG_dfe_adapt_en_lane2_TAP0|REG_dfe_adapt_en_lane2_TAP1));
		if (lane_mode == HDMI_4LANE)
				hdmi_mask(DFE_REG_DFE_EN_L3_reg,~(REG_dfe_adapt_en_lane3_TAP0|REG_dfe_adapt_en_lane3_TAP1),(REG_dfe_adapt_en_lane3_TAP0|REG_dfe_adapt_en_lane3_TAP1));

	}	

	newbase_hdmi21_set_koffset_flag(nport,0); //recover not do koffset , becasue input off will mute display
#endif	
}

void newbase_hdmi21_tmds_dfe_mid_adapthve(unsigned char nport, unsigned int b_clk)
{
	unsigned char le0;

//	HDMI_PRINTF("%s\n", __func__);
	le0 = lib_hdmi_dfe_get_le(nport,0);

	if (le0 >= 8)
		return;
//	le1 = lib_hdmi_dfe_get_le(nport,1);
//	le2 = lib_hdmi_dfe_get_le(nport,2);

	lib_hdmi_dfe_init_le(nport, (LN_R|LN_G|LN_B), (le0 + 2));

	HDMI_PRINTF("le=%x\n", (le0 + 2));

}




void newbase_hdmi21_dump_dfe_para(unsigned char nport, unsigned char lane_mode)
{
	unsigned char lane;

	for (lane=0; lane<lane_mode; lane++) {
		DFE_WARN("HDMI[p%d]  ********************Lane%d******************\n", nport,lane);
		DFE_WARN("HDMI[p%d]  Lane%d Vth=0x%x\n", nport,lane, phy_st[nport].dfe_t[lane].vth);
		DFE_WARN("HDMI[p%d] Lane%d Tap0=0x%x, max=0x%02x, min=0x%02x\n",nport,

			lane, phy_st[nport].dfe_t[lane].tap0, phy_st[nport].dfe_t[lane].tap0max, phy_st[nport].dfe_t[lane].tap0min);

		DFE_WARN("HDMI[p%d]  Lane%d Tap1=%+d , Tap2=%+d Tap3=%+d Tap4=%+d\n",
			nport,lane, phy_st[nport].dfe_t[lane].tap1, phy_st[nport].dfe_t[lane].tap2,phy_st[nport].dfe_t[lane].tap3,phy_st[nport].dfe_t[lane].tap4);

		DFE_WARN("HDMI[p%d]  Lane%d LE=0x%02x, max=0x%02x, min=0x%02x\n",
			nport,lane, phy_st[nport].dfe_t[lane].le, phy_st[nport].dfe_t[lane].lemax, phy_st[nport].dfe_t[lane].lemin);
	}

}

void lib_hdmi21_dump_dfe_tap0_le(unsigned char nport,unsigned char lane_mode)
{

	unsigned char lane;

	for (lane=0; lane< lane_mode; lane++) {
		phy_st[nport].dfe_t[lane].tap0max = lib_hdmi_dfe_get_tap0max(nport, lane);
		phy_st[nport].dfe_t[lane].tap0min = lib_hdmi_dfe_get_tap0min(nport, lane);
		phy_st[nport].dfe_t[lane].vth = lib_hdmi_dfe_get_vth(nport, lane);
		phy_st[nport].dfe_t[lane].tap0 = lib_hdmi_dfe_get_tap0(nport, lane);
		phy_st[nport].dfe_t[lane].le = lib_hdmi_dfe_get_le(nport, lane);
		phy_st[nport].dfe_t[lane].lemax = lib_hdmi_dfe_get_lemax(nport, lane);
		phy_st[nport].dfe_t[lane].lemin = lib_hdmi_dfe_get_lemin(nport, lane);
	}

	for (lane=0; lane<lane_mode; lane++) {
		DFE_WARN("HDMI[p%d] ********************Lane%d******************\n",nport, lane);
		DFE_WARN("HDMI[p%d]  Lane%d Vth=0x%x\n",nport, lane, phy_st[nport].dfe_t[lane].vth);
		DFE_WARN("HDMI[p%d]  Lane%d Tap0=0x%x, max=0x%02x, min=0x%02x\n",nport,lane, phy_st[nport].dfe_t[lane].tap0, phy_st[nport].dfe_t[lane].tap0max, phy_st[nport].dfe_t[lane].tap0min);
		DFE_WARN("HDMI[p%d] Lane%d LE=0x%02x, max=0x%02x, min=0x%02x\n",nport,lane, phy_st[nport].dfe_t[lane].le, phy_st[nport].dfe_t[lane].lemax, phy_st[nport].dfe_t[lane].lemin);
	}
	
}


#ifdef CONFIG_POWER_SAVING_MODE
void newbase_hdmi21_phy_pw_saving(unsigned char port)
{	
	//if (port >= HD20_PORT) {
	//	return;
	//}
	lib_hdmi21_cdr_rst(port, 1, MODE_TMDS,HDMI_4LANE);
	lib_hdmi21_demux_ck_vcopll_rst(port, 1, HDMI_4LANE);

	lib_hdmi21_ac_couple_power_en(port,0);
	lib_hdmi21_eq_pi_power_en(port,0,MODE_TMDS,HDMI_4LANE);
//	lib_hdmi21_cmu_rst(port, 1);
	lib_hdmi21_cmu_pll_en(port, 0,MODE_TMDS);
	
}

#endif

void lib_hdmi_vd_en(unsigned char nport, unsigned char en)
{
	if (en) 
	{
		_phy_rtd_part_outl(0x1800da10, 25, 24, 0x3);
	}
	else
	{
		_phy_rtd_part_outl(0x1800da10, 25, 24, 0x0);
	}	
		
}


void lib_hdmi21_hysteresis_en(unsigned char en)
{

	unsigned char nport;
	if (en) 
	{
		for (nport=0; nport<HDMI_PORT_TOTAL_NUM; nport++)
		{
			if (newbase_hdmi_phy_port_status(nport) == HDMI_PHY_PORT_NOTUSED)
				continue;
			hdmi_mask(HDMIRX_PHY_ck_cmu_1_reg,~(cmu_ck_1_HYS_AMP_EN|cmu_ck_1_MD_DET_SRC|cmu_ck_1_HYS_WIN_SEL|cmu_ck_1_CKIN_SEL|cmu_ck_1_NON_HYS_AMP_EN),
			(cmu_ck_1_HYS_AMP_EN|cmu_ck_1_NON_HYS_AMP_EN|cmu_ck_1_MD_DET_SRC|cmu_ck_1_ENHANCE_BIAS_10|cmu_ck_1_CKIN_SEL));
		}
	}
	else 
	{
		for (nport=0; nport<HDMI_PORT_TOTAL_NUM; nport++)
		{
			if (newbase_hdmi_phy_port_status(nport) == HDMI_PHY_PORT_NOTUSED)
				continue;

			hdmi_mask(HDMIRX_PHY_ck_cmu_1_reg,~(cmu_ck_1_HYS_AMP_EN|cmu_ck_1_MD_DET_SRC|cmu_ck_1_HYS_WIN_SEL|cmu_ck_1_CKIN_SEL|cmu_ck_1_NON_HYS_AMP_EN),
			(cmu_ck_1_ENHANCE_BIAS_01|cmu_ck_1_CKIN_SEL|cmu_ck_1_NON_HYS_AMP_EN));
		}
	}




}

void lib_hdmi21_stb_tmds_clk_wakeup_en(unsigned char nport, unsigned char en)
{
	if (en) {
		//Disable CLK Detection
		rtd_outl(0xb8060034,0x101); //Release STB_TOP HDMI CLK detect RSTN
		rtd_outl(0xb8060044,0x101); //Enable  STB_TOP HDMI CLK detect CLK
		hdmi_mask(0xb806005c, ~(_BIT1|_BIT0), 0);	//Hdmi_dp_ck_data_detect p0/p1 enable to hdmi combo phy
																	//Hdmi_ck_data_detect p2/p3 enable hdmi pure phy
		//Enable CLK Detection
		hdmi_mask(HDMIRX_PHY_top_in_2_reg, ~(REG_HDMIRX_BIAS_EN), REG_HDMIRX_BIAS_EN); //MBIAS setting
		hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(REG_TOP_IN_4_Z0_POW|REG_TOP_IN_3_CKDETECT_ENHANCE_I|REG_TOP_IN_3_REG_RSTB_CKDETECT|REG_TOP_IN_3_CKDETECT_ENHANCE|REG_TOP_IN_3_REG_EN_CKDETECT|REG_TOP_IN_3_REG_EN_CKIN),
			(REG_TOP_IN_4_Z0_POW|REG_TOP_IN_3_CKDETECT_ENHANCE_I|REG_TOP_IN_3_REG_RSTB_CKDETECT|REG_TOP_IN_3_CKDETECT_ENHANCE|REG_TOP_IN_3_REG_EN_CKDETECT|REG_TOP_IN_3_REG_EN_CKIN));
		hdmi_mask(HDMIRX_PHY_top_in_1_reg, ~(Combo_Port_CLK_Detect_Enable), Combo_Port_CLK_Detect_Enable); //Combo port CLK_Detect_Enable

		//CK Latch setting
		hdmi_mask(HDMIRX_PHY_top_in_2_reg, ~(CK_LATCH), 0); //latch bit clear
		hdmi_mask(HDMIRX_PHY_top_in_2_reg, ~(CK_LATCH), CK_LATCH);	//latch bit set
		hdmi_mask(HDMIRX_PHY_top_in_2_reg, ~(CK_LATCH), 0); //latch bit clear

		//Enable CLK Detection
		hdmi_mask(0xb806005c, ~(_BIT1|_BIT0), (_BIT1|_BIT0));	//Hdmi_dp_ck_data_detect p0/p1 enable to hdmi combo phy
																					//Hdmi_ck_data_detect p2/p3 enable hdmi pure phy

	}else {
		//Disable CLK Detection
		hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(REG_TOP_IN_3_REG_EN_CKDETECT|REG_TOP_IN_3_REG_EN_CKIN), 0);
		hdmi_mask(HDMIRX_PHY_top_in_1_reg, ~(Combo_Port_CLK_Detect_Enable), 0); //Combo port CLK_Detect_Enable

		//Latch setting
		hdmi_mask(HDMIRX_PHY_top_in_2_reg, ~(CK_LATCH), CK_LATCH);	//latch bit set
		if(nport==0)
		{
			
			hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
			hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear
		}
		else if(nport==1)
		{
			
			hdmi_mask(HDMIRX_PHY_ADPHY_REG4_P1_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
			hdmi_mask(HDMIRX_PHY_ADPHY_REG4_P1_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear
		}

		hdmi_mask(HDMIRX_PHY_top_in_2_reg, ~(CK_LATCH), 0);	//latch bit clear
		if(nport==0)
		{
			hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
			hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear
		}
		else if(nport==1)
		{
			hdmi_mask(HDMIRX_PHY_ADPHY_REG4_P1_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
			hdmi_mask(HDMIRX_PHY_ADPHY_REG4_P1_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear
		}
		
	}
}

void lib_hdmi21_phy_init(void)
{
	unsigned char nport;

	HDMI_EMG("%s start ====\n", __func__);
	lib_hdmi21_z0_calibration();
	
	for (nport=0; nport<HDMI_PORT_TOTAL_NUM; nport++)
	{
		if (newbase_hdmi_phy_port_status(nport) == HDMI_PHY_PORT_NOTUSED)
			continue;
	/*	
		DPHY_Fix_tmds_1_func(nport, TMDS_5p94G);
		APHY_Fix_tmds_1_func(nport, TMDS_5p94G);
		DPHY_Init_Flow_tmds_1_func(nport, TMDS_5p94G);
		APHY_Init_Flow_tmds_1_func(nport, TMDS_5p94G);
		*/
		//=====[P0_CLK_DETECT]=====//
		//*********************************** MBIAS
		_phy_rtd_outl(0x1800da18,0x00000001);//REG_HDMIRX_BIAS_EN
		//*********************************** AC-couple
		_phy_rtd_outl(0x1800da30,0x00000000);//INOFF_EN
		_phy_rtd_outl(0x1800da30,0x00000000);//[APHY fix]  B-Lane INPOFF_SINGLE_EN
		_phy_rtd_outl(0x1800da30,0x00000000);//[APHY fix]  B-Lane INNOFF_SINGLE_EN
		_phy_rtd_outl(0x1800da30,0x00000008);//[APHY fix]  B-Lane POW_AC_COUPLE
		_phy_rtd_outl(0x1800da30,0x00000018);//[APHY fix]  B-Lane RXVCM_SEL[1:0]
		//*********************************** non-hys amp 
		_phy_rtd_outl(0x1800dab4,0x00000001);//[APHY fix]  Non-Hysteresis Amp Power
		_phy_rtd_outl(0x1800dab4,0x00000007);//[APHY fix]  CK Mode hysteresis amp Hysteresis adjustment[2:0]
		_phy_rtd_outl(0x1800dab4,0x00000007);//[APHY para]  CMU input clock source select. 0: non-hys & xtal mux, 1:hys
		_phy_rtd_outl(0x1800dab4,0x00000007);//[APHY fix]  CK Mode detect source. 0:non-hys, 1:hys
		_phy_rtd_outl(0x1800dab4,0x00000007);//[APHY fix]  Hysteresis amp power
		_phy_rtd_outl(0x1800dab4,0x00000087);//[APHY para]  CMU_CKXTAL_SEL. 0:xtal , 1:non-hys
		//*********************************** CK_TX & MD SEL
		_phy_rtd_outl(0x1800da10,0x00004000);//[] TOP_IN_2, CK_TX EN
		_phy_rtd_outl(0x1800da10,0x00004000);//[] TOP_IN_2, CK_MD SEL
		//*********************************** Z0
		_phy_rtd_part_outl(0x1800da04, 5, 5, 0x01); //[]  REG_P0_Z0_Z0POW_FIX
		_phy_rtd_part_outl(0x1800da10, 25, 24, 0x03); //[]  0x3 Strong level	
		_phy_rtd_part_outl(0x1800da10, 28, 28, 0x01); //[]  REG_P0_Z0_Z0POW_CK
		_phy_rtd_part_outl(0x1800da10, 29, 29, 0x01); //[]  REG_P0_Z0_Z0POW_B
		_phy_rtd_part_outl(0x1800da10, 30, 30, 0x01); //[]  REG_P0_Z0_Z0POW_G
		_phy_rtd_part_outl(0x1800da10, 31, 31, 0x01); //[]  REG_P0_Z0_Z0POW_R
		_phy_rtd_part_outl(0x1800da04, 6, 6, 0x0); //[]  Z0_P_OFF
		_phy_rtd_part_outl(0x1800da04, 7, 7, 0x0); //[]  Z0_N_OFF
		_phy_rtd_part_outl(0x1800da04, 9, 9, 0x0); //[]  REG_Z0_FT_PN_SHORT_EN
		//*********************************** MD
		_phy_rtd_outl(0x1800da10,0x00004800);//TOP_IN_2<3>  ,ck_test group sel.  0:CK test group1, 1:CK test group2
		_phy_rtd_outl(0x1800da10,0x00004a00);//REG_TOP_IN_4<2:0>     ,ck_test sel in group1 and group2
		_phy_rtd_outl(0x1800dab4,0x00001087);//CKBK_TEST_EN
		_phy_rtd_outl(0x1800dc2c,0x00000000);//Clock Detect debounce
		_phy_rtd_outl(0x1800dc2c,0x00000000);//MD reset, delay time>100ns
		_phy_rtd_outl(0x1800dc2c,0x00000001);
		
	}
/*
	for (nport=HDMI_PORT0; nport<HDMI_PORT_TOTAL_NUM; nport++) {

		if(newbase_hdmi_phy_port_status(nport) == HDMI_PHY_PORT_NOTUSED)
			continue;

		lib_hdmi21_z300_sel(nport, 1);
		_phy_rtd_part_outl(0x1800dd10, 31, 31, 0x1); //[ADPHY Init flow]  reg_adphy_latch_clk_0
		_phy_rtd_part_outl(0x1800dd10, 30, 30, 0x0); //[ADPHY Init flow]  reg_adphy_latch_clk_1
		
	}
*/
	HDMI_EMG("%s end ====\n", __func__);

}


void lib_hdmi21_z0_set(unsigned char nport, unsigned char lane, unsigned char enable, unsigned char CLK_Wake_Up)
{
	HDMI_EMG("Port(%d)%s(%d)(%d)\n",nport, __func__,enable,CLK_Wake_Up);

	CLK_Wake_Up = CLK_Wake_Up ? 1: 0;

	if ((enable == 0) && (CLK_Wake_Up == 1))
	{
		enable =1;
	}
	//CK Latch setting 0
	hdmi_mask(HDMIRX_PHY_top_in_2_reg, ~(CK_LATCH), 0); //latch bit clear
	if(nport==0)
	{
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear
	}
	else if(nport==1)
	{
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_P1_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_P1_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear
	}
		
	hdmi_mask(HDMIRX_PHY_z0k_reg, ~(HDMIRX_PHY_Z0K_reg_z0_p_off_mask), HDMIRX_PHY_Z0K_reg_z0_p_off(!enable));
	hdmi_mask(HDMIRX_PHY_z0k_reg, ~(HDMIRX_PHY_Z0K_reg_z0_n_off_mask), HDMIRX_PHY_Z0K_reg_z0_n_off(!enable));
	if(nport==0)
	{
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear
	}
	else if(nport==1)
	{
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_P1_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_P1_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear
	}

	//CK Latch setting 1
	hdmi_mask(HDMIRX_PHY_top_in_2_reg, ~(CK_LATCH), CK_LATCH);	//latch bit set
	if(nport==0)
	{
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear
	}
	else if(nport==1)
	{
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_P1_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_P1_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear
	}
	//CK Latch setting 0
	hdmi_mask(HDMIRX_PHY_top_in_2_reg, ~(CK_LATCH), 0); //latch bit clear
	if(nport==0)
	{
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear
	}
	else if(nport==1)
	{
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_P1_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
		hdmi_mask(HDMIRX_PHY_ADPHY_REG4_P1_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear
	}
	
}


void lib_hdmi21_z300_sel(unsigned char nport, unsigned char mode)  // 0: active mode 1:vcm mode
{
	
	if (mode) {
		hdmi_mask(HDMIRX_PHY_z0k_reg, ~(HDMIRX_PHY_Z0K_reg_p0_z0_z0pow_fix_mask), HDMIRX_PHY_Z0K_reg_p0_z0_z0pow_fix(mode));
	} else {
		hdmi_mask(HDMIRX_PHY_z0k_reg, ~(HDMIRX_PHY_Z0K_reg_p0_z0_z0pow_fix_mask), HDMIRX_PHY_Z0K_reg_p0_z0_z0pow_fix(mode));
	}


}




void lib_hdmi_cmu_rst(unsigned char nport, unsigned char rst)
{
#if 0  //h5 check after
	const unsigned int reg_ck3[HDMI_PORT_TOTAL_NUM] = {
		DFE_HDMI_RX_PHY_P0_CK3_reg,
		DFE_HDMI_RX_PHY_P1_CK3_reg,
		DFE_HDMI_RX_PHY_P2_CK3_reg,
		DFE_HDMI_RX_PHY_P0_HD21_P0_CMU3_reg
	};

	if (port >= HDMI_PORT_TOTAL_NUM) {
		return;
	}

	if (rst) {
		hdmi_mask(reg_ck3[port], ~(P0_ck_9_CMU_PFD_RSTB|_BIT3|P0_ck_9_CMU_WDRST|_BIT1), (_BIT3|P0_ck_9_CMU_WDRST));	//PFD_RSTB  reset
	} else {
		hdmi_mask(reg_ck3[port], ~(P0_ck_9_CMU_PFD_RSTB|_BIT3|P0_ck_9_CMU_WDRST|_BIT1), (P0_ck_9_CMU_PFD_RSTB|_BIT1));  //release, normal
	}
#endif
}


void lib_hdmi21_dfe_power(unsigned char nport, unsigned char en, unsigned char lane_mode)
{
#if 0

	hdmi_mask(HDMIRX_PHY_R2_reg, ~(b_9_DFE_ADAPTION_POW_EN|b_9_DFE_TAP_EN), (HD21_RX_PHY_P0_DFE_ADAPTION_EN(en)|HD21_RX_PHY_P0_b_tap_en(0xf)));
	hdmi_mask(HDMIRX_PHY_G2_reg, ~(b_9_DFE_ADAPTION_POW_EN|b_9_DFE_TAP_EN), (HD21_RX_PHY_P0_DFE_ADAPTION_EN(en)|HD21_RX_PHY_P0_b_tap_en(0xf)));
	hdmi_mask(HDMIRX_PHY_B2_reg, ~(b_9_DFE_ADAPTION_POW_EN|b_9_DFE_TAP_EN), (HD21_RX_PHY_P0_DFE_ADAPTION_EN(en)|HD21_RX_PHY_P0_b_tap_en(0xf)));

	if (lane_mode == HDMI_4LANE)
		hdmi_mask(HDMIRX_PHY_CK2_reg, ~(b_9_DFE_ADAPTION_POW_EN|b_9_DFE_TAP_EN), (HD21_RX_PHY_P0_DFE_ADAPTION_EN(en)|HD21_RX_PHY_P0_b_tap_en(0xf)));
#endif
		
}

void lib_hdmi21_eq_set_clock_boundary(unsigned char nport)
{

        
            clock_bound_3g = TMDS_3G;
            clock_bound_1p5g = TMDS_1p5G;
            clock_bound_45m = 430;
            clock_bound_110m = 1042;
        

        //HDMI_PRINTF("clock_bound_3g=%d\n", clock_bound_3g);

}

#ifdef CONFIG_POWER_SAVING_MODE
#define  AC_COPULE_POW       (_BIT3)    //Mac8p
#define  RXVCM_SEL			(_BIT5|_BIT4)//Mac8p Need check with AIC
#define  RXVCM_10			(_BIT6)   //Mac8p
void lib_hdmi21_ac_couple_power_en(unsigned char nport,unsigned char en)
{
#if 1
	if (en) {
		hdmi_mask(HDMIRX_PHY_b0_reg, ~RXVCM_SEL, RXVCM_10);
		hdmi_mask(HDMIRX_PHY_g0_reg, ~RXVCM_SEL, RXVCM_10);
		hdmi_mask(HDMIRX_PHY_r0_reg, ~RXVCM_SEL, RXVCM_10);
		hdmi_mask(HDMIRX_PHY_ck0_reg, ~RXVCM_SEL, RXVCM_10);		
		
		hdmi_mask(HDMIRX_PHY_b0_reg, ~AC_COPULE_POW, AC_COPULE_POW);
		hdmi_mask(HDMIRX_PHY_g0_reg, ~AC_COPULE_POW, AC_COPULE_POW);
		hdmi_mask(HDMIRX_PHY_r0_reg, ~AC_COPULE_POW, AC_COPULE_POW);
		hdmi_mask(HDMIRX_PHY_ck0_reg, ~AC_COPULE_POW, AC_COPULE_POW);  //check power consumption for 3 lane frl mode
	} else {
		hdmi_mask(HDMIRX_PHY_b0_reg, ~AC_COPULE_POW, 0x0);
		hdmi_mask(HDMIRX_PHY_g0_reg, ~AC_COPULE_POW, 0x0);
		hdmi_mask(HDMIRX_PHY_r0_reg, ~AC_COPULE_POW, 0x0);
		hdmi_mask(HDMIRX_PHY_ck0_reg, ~AC_COPULE_POW, AC_COPULE_POW);   //for hysteresis amp always open

	}
#endif
}


void lib_hdmi21_eq_pi_power_en(unsigned char nport,unsigned char en, unsigned char frl_mode,unsigned char lane_mode)
{
	if (en) {

		hdmi_mask(HDMIRX_PHY_b1_reg, ~(POW_LEQ|RS_CAL_EN),  POW_LEQ|RS_CAL_EN);
		hdmi_mask(HDMIRX_PHY_g1_reg, ~(POW_LEQ|RS_CAL_EN) , POW_LEQ|RS_CAL_EN);
		hdmi_mask(HDMIRX_PHY_r1_reg, ~(POW_LEQ|RS_CAL_EN), POW_LEQ|RS_CAL_EN);
		
	} 
	else 
	{
		hdmi_mask(HDMIRX_PHY_b1_reg, ~(POW_LEQ|RS_CAL_EN),  0x0);
		hdmi_mask(HDMIRX_PHY_g1_reg, ~(POW_LEQ|RS_CAL_EN) , 0x0);
		hdmi_mask(HDMIRX_PHY_r1_reg, ~(POW_LEQ|RS_CAL_EN),  0x0);
	}
	
}
#endif






//cdr & dcdr mode ck lane & data lane enable
void lib_hdmi21_cmu_pll_en(unsigned char nport, unsigned char enable, unsigned char frl_mode)
{
#if 1
	if (enable)
	{	

		hdmi_mask(HDMIRX_PHY_ck_cmu_1_reg,~(cmu_ck_1_CKIN_SEL),cmu_ck_1_CKIN_SEL);

		hdmi_mask(HDMIRX_PHY_fld_regd18,~(acdr_manual_RDY|acdr_manual_CMU_SEL),0x0);//R
		hdmi_mask(HDMIRX_PHY_fld_regd11,~(acdr_manual_RDY|acdr_manual_CMU_SEL),0x0);//G
		hdmi_mask(HDMIRX_PHY_fld_regd04,~(acdr_manual_RDY|acdr_manual_CMU_SEL), 0x0);//B
		hdmi_mask(HDMIRX_PHY_fld_regd25,~(acdr_manual_RDY|acdr_manual_CMU_SEL), 0x0);//CK
		//r lane on
		hdmi_mask(HDMIRX_PHY_acdr_r0_reg,~(acdr_PFD_RSTB|acdr_DIV_IQ_RSTB), acdr_PFD_RSTB|acdr_DIV_IQ_RSTB);
		hdmi_mask(HDMIRX_PHY_acdr_r1_reg,~(acdr_VCO_EN), acdr_VCO_EN);//1: on
		//g lane on
		hdmi_mask(HDMIRX_PHY_acdr_g0_reg,~(acdr_PFD_RSTB|acdr_DIV_IQ_RSTB), acdr_PFD_RSTB|acdr_DIV_IQ_RSTB);
		hdmi_mask(HDMIRX_PHY_acdr_g1_reg,~(acdr_VCO_EN), acdr_VCO_EN);//1: on
		//b lane on
		hdmi_mask(HDMIRX_PHY_acdr_b0_reg,~(acdr_PFD_RSTB|acdr_DIV_IQ_RSTB), acdr_PFD_RSTB|acdr_DIV_IQ_RSTB);
		hdmi_mask(HDMIRX_PHY_acdr_b1_reg,~(acdr_VCO_EN), acdr_VCO_EN);//1: on
		//ck lane on
		hdmi_mask(HDMIRX_PHY_acdr_ck0_reg,~(acdr_PFD_RSTB|acdr_DIV_IQ_RSTB), acdr_PFD_RSTB|acdr_DIV_IQ_RSTB);
		hdmi_mask(HDMIRX_PHY_acdr_ck1_reg,~(acdr_VCO_EN), acdr_VCO_EN);//1: on			

	} 
	else 
	{ //disable default tmds clock

		hdmi_mask(HDMIRX_PHY_ck_cmu_1_reg,~(cmu_ck_1_CKIN_SEL),cmu_ck_1_CKIN_SEL);
		//default from register
		hdmi_mask(HDMIRX_PHY_fld_regd18,~(acdr_manual_RDY|acdr_manual_CMU_SEL),(acdr_manual_RDY_FROM_REG));//R
		hdmi_mask(HDMIRX_PHY_fld_regd11,~(acdr_manual_RDY|acdr_manual_CMU_SEL),(acdr_manual_RDY_FROM_REG));//G
		hdmi_mask(HDMIRX_PHY_fld_regd04,~(acdr_manual_RDY|acdr_manual_CMU_SEL), (acdr_manual_RDY_FROM_REG));//B
		hdmi_mask(HDMIRX_PHY_fld_regd25,~(acdr_manual_RDY|acdr_manual_CMU_SEL), (acdr_manual_RDY_FROM_REG));//CK

		//r lane off
		hdmi_mask(HDMIRX_PHY_acdr_r0_reg,~(acdr_PFD_RSTB|acdr_DIV_IQ_RSTB), 0x0);
		hdmi_mask(HDMIRX_PHY_acdr_r1_reg,~(acdr_VCO_EN), 0x0);//0: 0ff
		//g lane off
		hdmi_mask(HDMIRX_PHY_acdr_g0_reg,~(acdr_PFD_RSTB|acdr_DIV_IQ_RSTB), 0x0);
		hdmi_mask(HDMIRX_PHY_acdr_g1_reg,~(acdr_VCO_EN), 0x0);//0: 0ff
		//b lane off
		hdmi_mask(HDMIRX_PHY_acdr_b0_reg,~(acdr_PFD_RSTB|acdr_DIV_IQ_RSTB), 0x0);
		hdmi_mask(HDMIRX_PHY_acdr_b1_reg,~(acdr_VCO_EN), 0x0);//0: 0ff
		//ck lane off
		hdmi_mask(HDMIRX_PHY_acdr_ck0_reg,~(acdr_PFD_RSTB|acdr_DIV_IQ_RSTB), 0x0);
		hdmi_mask(HDMIRX_PHY_acdr_ck1_reg,~(acdr_VCO_EN), 0x0);//0: 0ff		}

	}
#endif	
	
}


void lib_hdmi21_demux_ck_vcopll_rst(unsigned char nport, unsigned char rst, unsigned char lane_mode)
{
// rst = 1: reset
// rst = 0: normal working operation
	unsigned int rgb_demux;

	rgb_demux=HDMIRX_PHY_HD21_REGA00_p0_r_demux_rstb_mask|HDMIRX_PHY_HD21_REGA00_p0_g_demux_rstb_mask|HDMIRX_PHY_HD21_REGA00_p0_b_demux_rstb_mask;

	if (rst) {
		hdmi_mask(HDMIRX_PHY_rega00, ~rgb_demux, 0);
		
	} else {

		hdmi_mask(HDMIRX_PHY_rega00, ~rgb_demux,rgb_demux );

	}

}




void lib_hdmi21_cdr_rst(unsigned char nport, unsigned char rst, unsigned char frl_mode,unsigned char lane_mode)
{
#if 1
// rst = 1: reset
// rst = 0: normal working operation
	unsigned int rst_reg,dig_bit,cdr_bit;
        // release reset


	 rst_reg = HDMIRX_PHY_cdr_regd00;
	 dig_bit = HDMIRX_PHY_HD21_P0_CDR_REGD00_reg_p0_r_dig_rst_n_mask|HDMIRX_PHY_HD21_P0_CDR_REGD00_reg_p0_g_dig_rst_n_mask|HDMIRX_PHY_HD21_P0_CDR_REGD00_reg_p0_b_dig_rst_n_mask;
	 cdr_bit =HDMIRX_PHY_HD21_P0_CDR_REGD00_reg_p0_r_cdr_rst_n_mask|HDMIRX_PHY_HD21_P0_CDR_REGD00_reg_p0_g_cdr_rst_n_mask|HDMIRX_PHY_HD21_P0_CDR_REGD00_reg_p0_b_cdr_rst_n_mask;
	

	if (rst) {
		hdmi_mask(rst_reg, ~dig_bit, 0);
		hdmi_mask(rst_reg, ~cdr_bit, 0);

	} else {
		hdmi_mask(rst_reg, ~(dig_bit|cdr_bit), (dig_bit|cdr_bit));
	}

#endif
}

void lib_hdmi21_wdog(unsigned char nport,unsigned char frl_mode,unsigned char lane_mode)
{
#if 0
	if (nport >= HD21_PORT_TOTAL_NUM)
		return;

	hdmi_mask(HDMIRX_PHY_REGD07_reg, ~(HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_bypass_eqen_rdy_mask|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_bypass_data_rdy_mask|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_bypass_pi_mask|
		                                               HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_data_rdy_time_mask|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_en_clkout_manual_mask|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_en_eqen_manual_mask|
		                                                HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_en_data_manual_mask|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_eqen_rdy_time_mask|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_pll_wd_base_time_mask|
		                                                HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_pll_wd_rst_wid_mask|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_pll_wd_time_rdy_mask|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_p0_pll_wd_ckrdy_ro_mask|
		                                                HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_p0_wdog_rst_n_mask),
		                                                	HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_bypass_clk_rdy(1)| HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_bypass_eqen_rdy(1)|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_bypass_data_rdy(1)|
		                                                 HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_data_rdy_time(4)| HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_en_clkout_manual(1)|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_en_eqen_manual(1)|
									HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_en_data_manual(1)|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_eqen_rdy_time(4)|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_pll_wd_base_time(2)|
									HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_pll_wd_rst_wid(1)|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_pll_wd_time_rdy(1) |HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_p0_pll_wd_ckrdy_ro(1)|
									HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_p0_wdog_rst_n(1));


	hdmi_mask(HDMIRX_PHY_REGD08_reg, ~(HDMIRX_2P1_PHY_P0_HD21_Y1_REGD08_reg_p0_wd_sdm_en_mask),0);
	udelay(5);
	hdmi_mask(HDMIRX_PHY_REGD08_reg, ~(HDMIRX_2P1_PHY_P0_HD21_Y1_REGD08_reg_p0_wd_sdm_en_mask), HDMIRX_2P1_PHY_P0_HD21_Y1_REGD08_reg_p0_wd_sdm_en_mask);	//adams modify, 20160130	//Load CMU M code


	if (frl_mode == MODE_TMDS) {
		hdmi_mask(HDMIRX_PHY_REGD07_reg,~HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_acdr_l0_en_mask,0x0);
		hdmi_mask(HDMIRX_PHY_REGD09_reg,~(HDMIRX_2P1_PHY_P0_HD21_Y1_REGD09_reg_p0_acdr_l3_en_mask|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD09_reg_p0_acdr_l2_en_mask|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD09_reg_p0_acdr_l1_en_mask),0x0);
	} else {
		hdmi_mask(HDMIRX_PHY_REGD07_reg,~(HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_acdr_l0_en_mask),HDMIRX_2P1_PHY_P0_HD21_Y1_REGD07_reg_p0_acdr_l0_en_mask);
		hdmi_mask(HDMIRX_PHY_REGD09_reg,~(HDMIRX_2P1_PHY_P0_HD21_Y1_REGD09_reg_p0_acdr_l2_en_mask|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD09_reg_p0_acdr_l1_en_mask),(HDMIRX_2P1_PHY_P0_HD21_Y1_REGD09_reg_p0_acdr_l2_en_mask|HDMIRX_2P1_PHY_P0_HD21_Y1_REGD09_reg_p0_acdr_l1_en_mask));
		if (lane_mode == HDMI_4LANE) {
			hdmi_mask(HDMIRX_PHY_REGD09_reg,~(HDMIRX_2P1_PHY_P0_HD21_Y1_REGD09_reg_p0_acdr_l3_en_mask),HDMIRX_2P1_PHY_P0_HD21_Y1_REGD09_reg_p0_acdr_l3_en_mask);
		} else {
			hdmi_mask(HDMIRX_PHY_REGD09_reg,~(HDMIRX_2P1_PHY_P0_HD21_Y1_REGD09_reg_p0_acdr_l3_en_mask),0x0);
		}
	}
#endif
}

void lib_hdmi21_dfe_config_setting(unsigned char nport,unsigned char lane_mode)
{
        lib_hdmi_mac_afifo_enable(nport,lane_mode);
}




//0x18038138
//0x18038138[31:0]: z0 value 4 port
//port0_z0=0x18038138[12:8] port0_done=0x18038138[13]
//port1_z0=0x18038138[18:14] port1_done=0x18038138[19]
//port2_z0=0x18038138[24:20] port2_done=0x18038138[25]
//port3_z0=0x18038138[30:26] port3_done=0x18038138[31]

extern bool AP_SET_HDMI_ext_trace_z0; 
extern  unsigned char ext_trace_z0; 

void lib_hdmi21_z0_calibration(void)
{
 	#define Z0_DIFF_ERR		0x0
		//unsigned int z0_otp_value = rtd_inl(EFUSE_DATAO_120_reg);
		unsigned int HDMI_Z0_Para1 = rtk_otp_field_read_int_by_name("hw_hdmi_rx_z0");
	 
	 unsigned char nport;
	 unsigned char z0_otp_done;
		unsigned char z0_0 = PHY_P0_Z0, z0_1 = PHY_P1_Z0,z0_2 = PHY_P2_Z0,z0_3 = PHY_P3_Z0;    //z0 for 4 port
	 unsigned char ext_trace_z0_0,ext_trace_z0_1,ext_trace_z0_2,ext_trace_z0_3; //external z0 for 4 port
	 unsigned long long val = 0;  

	 ext_trace_z0_0 = ext_trace_z0_1 = ext_trace_z0_2 = ext_trace_z0_3 =(0x5 - Z0_DIFF_ERR);  // 1 step 1.5 ohm  5.4 ohm = 4 step
	 z0_otp_done = ((HDMI_Z0_Para1 & 0x00000020) >> 5) | ((HDMI_Z0_Para1 & 0x00000800) >> 10) | ((HDMI_Z0_Para1 & 0x00020000) >> 15) | ((HDMI_Z0_Para1 & 0x00800000) >> 20);

	 
	  if (AP_SET_HDMI_ext_trace_z0==TRUE) {
		ext_trace_z0_0 = ext_trace_z0- Z0_DIFF_ERR;
		ext_trace_z0_1 = ext_trace_z0- Z0_DIFF_ERR;
		ext_trace_z0_2 = ext_trace_z0- Z0_DIFF_ERR;
		ext_trace_z0_3 = ext_trace_z0- Z0_DIFF_ERR;
		
	  }else   if (pcb_mgr_get_enum_info_byname("HDMI_EXT_Z0_CFG", &val)==0) {
		ext_trace_z0_0 = (val & 0x000000FF)- Z0_DIFF_ERR;
		ext_trace_z0_1 = ((val & 0x0000FF00) >> 8) - Z0_DIFF_ERR;
		ext_trace_z0_2 = ((val & 0x00FF0000) >> 16) - Z0_DIFF_ERR;
		ext_trace_z0_3 = ((val & 0xFF000000) >> 24) - Z0_DIFF_ERR;

	  } else {
			HDMI_PRINTF("NOT GET  EXTERNAL PCB TRACE Z0\n");
	  }
	  
	HDMI_PRINTF("p0_ext_trace_z0=%x  p1_ext_trace_z0=%x p2_ext_trace_z0=%x	p3_ext_trace_z0=%x\n",ext_trace_z0_0,ext_trace_z0_1,ext_trace_z0_2,ext_trace_z0_3);
	
	 
	HDMI_PRINTF("Z0 OTP done_bit=%x value=%x\n",z0_otp_done,HDMI_Z0_Para1);

	if (z0_otp_done & 0x1) {
		z0_0 = HDMI_Z0_Para1 & 0x0000001F;
	}
	else
		z0_0=PHY_P0_Z0;

	if(ext_trace_z0_0==0)
		z0_0 = z0_0 - 2;// 

	if (z0_otp_done & 0x2) {
		z0_1 = (HDMI_Z0_Para1 & 0x000007C0) >> 6;
	}
	else
		z0_1=PHY_P1_Z0;

	if(ext_trace_z0_1==0)
		z0_1 = z0_1 - 2;// 

	if (z0_otp_done & 0x4) {
		z0_2 = (HDMI_Z0_Para1 & 0x0001F000) >> 12;
	}
	else
		z0_2=PHY_P2_Z0;

	if(ext_trace_z0_2==0)
		z0_2 = z0_2 - 2;//

	if (z0_otp_done & 0x8) {
		z0_3 = (HDMI_Z0_Para1 & 0x007c0000) >> 18;
	}
	else
		z0_3=PHY_P3_Z0;

	if(ext_trace_z0_3==0)
		z0_3 = z0_3 - 2;//

	HDMI_PRINTF("z0_0=%x z0_1=%x z0_2=%x z0_3=%x\n",z0_0,z0_1,z0_2,z0_3);
	for (nport=0; nport<HDMI_PORT_TOTAL_NUM; nport++)
	{
		if (newbase_hdmi_phy_port_status(nport) == HDMI_PHY_PORT_NOTUSED)
		{
			HDMI_PRINTF("lib_hdmi_z0_calibration, skip not used port %d\n", nport );
			continue;
		}

		if (HDMI_PORT0 == nport)
		{
			hdmi_mask(HDMIRX_PHY_z0k_reg, ~(HDMIRX_PHY_Z0K_reg_z0_adjr_0_mask),
				(HDMIRX_PHY_Z0K_reg_z0_adjr_0(z0_0)));
		}
		else if (HDMI_PORT1 == nport)
		{
			hdmi_mask(HDMIRX_PHY_z0k_reg, ~(HDMIRX_PHY_Z0K_reg_z0_adjr_0_mask),
				(HDMIRX_PHY_Z0K_reg_z0_adjr_0(z0_1)));
		}
		else if (HDMI_PORT2 == nport)
		{
			hdmi_mask(HDMIRX_PHY_z0k_reg, ~(HDMIRX_PHY_Z0K_reg_z0_adjr_0_mask),
				(HDMIRX_PHY_Z0K_reg_z0_adjr_0(z0_2)));
		}
		else if (HDMI_PORT3 == nport)
		{
			hdmi_mask(HDMIRX_PHY_z0k_reg, ~(HDMIRX_PHY_Z0K_reg_z0_adjr_0_mask),
				(HDMIRX_PHY_Z0K_reg_z0_adjr_0(z0_3)));
		}	  
	}
	HDMI_PRINTF("z0_0=%x z0_1=%x z0_2=%x z0_3=%x\n",z0_0,z0_1,z0_2,z0_3);

	hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), reg_adphy_latch_clk_0);	//latch bit set
	hdmi_mask(HDMIRX_PHY_ADPHY_REG4_reg, ~(reg_adphy_latch_clk_0), 0);	//latch bit clear

}



#if 0
void lib_hdmi21_single_end(unsigned char nport, unsigned char channel, unsigned char p_off, unsigned char n_off)
{
// channel = 0(R), 1(G), 2(B) 3(CK)
	hdmi_mask(reg_rgb1[nport][channel], ~(P0_b_1_inputoff_p), p_off<<2);
	hdmi_mask(reg_rgb1[nport][channel], ~(P0_b_1_inputoff_n), n_off<<2);

	if (nport == HDMI_PORT3) {
		if (channel == 3) {
			hdmi_mask(DFE_HDMI_RX_PHY_P0_HD21_P0_CK1_reg, ~(P0_b_1_inputoff_p), p_off<<2);
			hdmi_mask(DFE_HDMI_RX_PHY_P0_HD21_P0_CK1_reg, ~(P0_b_1_inputoff_n), n_off<<2);
		}
	}
}
#endif


// 1: clk lane  0: not clk lane
int lib_clk_lane_high_width_measure(unsigned char nport)
{
	unsigned int num_0[3],num_1[3];
	int max_num_1, min_num_1;
	int max_num_0;
	unsigned int x = 0,offset_n = 0;
	static unsigned int test = 0;

	HDMI_EMG("lib_clk_lane_high_width_measure\n");

	if (test == 0) {
	//force FRL 6G to open CDR CLK and EQ  before use transition detect
		//newbase_hdmi21_set_phy_eq(HD21_PORT, FRL_6G,MODE_FRL_6G_4_LANE,HDMI_4LANE);
		newbase_hdmi21_set_phy(HD21_PORT, FRL_6G,MODE_FRL_6G_4_LANE,HDMI_4LANE);
		lib_hdmi_set_dfe_close(HD21_PORT,HDMI_4LANE);
		//test = test+1;
	}
	HDMI_EMG("test=%d\n",test);
	
	
	num_0[0] = num_0[1] = num_0[2] = 0;
	num_1[0] = num_1[1] = num_1[2] = 0;
	
	max_num_1 = MAX(num_1[0],MAX(num_1[1], num_1[2]));
	min_num_1 = MIN(num_1[0],MIN(num_1[1], num_1[2]));
	max_num_0 = MAX(num_0[0],MAX(num_0[1], num_0[2]));

	HDMI_EMG("num_0 %d %d %d\n",num_0[0],num_0[1],num_0[2]);
	HDMI_EMG("num_1 %d %d %d\n",num_1[0],num_1[1],num_1[2]);
	HDMI_EMG("max_num_1=%d min_num_1=%d %d\n",max_num_1,min_num_1,0<<1);
	x = (max_num_1 + min_num_1) / 2;

	offset_n = 5;

	if ((x == 0) || (x <= offset_n))
		return 0;
	if (HDMI_ABS(max_num_0 , max_num_1) >= 5) {
		HDMI_EMG("max_num_1 and max_num_0 over range...\n");
		return 0;
	}

	HDMI_EMG("x=%d offset_n=%d\n",x,offset_n);

	return 1;
}


#if 0
//dphy output sel to mac
void lib_hdmi_out_port_sel(unsigned char nport)
{

	if (nport == 0) {
		hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD30_reg,~HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD30_reg_port_out_sel_mask,0x0);
	} else {
		hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD30_reg,~HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD30_reg_port_out_sel_mask,HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD30_reg_port_out_sel_mask);
	}

}
#endif




void lib_hdmi21_dfe_param_init(unsigned char nport, unsigned int b, unsigned char dfe_mode, unsigned char lane_mode)
{
}




