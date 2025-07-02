
/* ----- ----- ----- ----- ----- HEADER ----- ----- ----- ----- ----- */
#ifndef BUILD_QUICK_SHOW
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/export.h>
#include <linux/semaphore.h>
#include <mach/system.h>

#else
#include <no_os/printk.h>
#include <no_os/slab.h>
#include <include/string.h>

#endif
#include <rbus/color_reg.h>
#include <rbus/color_dlcti_reg.h>
//#include <rbus/rgb_shpnr_reg.h> //k25lp removed
#include <rtk_kdriver/io.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/vip/rgb_sh.h>

/* ----- ----- ----- ----- ----- VARIABLES ----- ----- ----- ----- ----- */

enum
{
	eRGB_SHP_LEVEL_off,
	eRGB_SHP_LEVEL_low,
	eRGB_SHP_LEVEL_mid,
	eRGB_SHP_LEVEL_high,
	eRGB_SHP_LEVEL_3tap,
	eRGB_SHP_LEVEL_5tap12,
	eRGB_SHP_LEVEL_5tap11,
	eRGB_SHP_LEVEL_5tap21,
	eRGB_SHP_LEVEL_5tap30,
	eRGB_SHP_LEVEL_5tap41,
	eRGB_SHP_LEVEL_5tapring,
	eRGB_SHP_LEVEL__count
};

VIP_RGB_SHP_LEVEL_TABLE gVipRgb_shpTable[eRGB_SHP_LEVEL__count]={
	{
		{ 0 , 0 , 109 , 366 , 37 , 0 , 0 },
		{ 240 , -120 , 0 },
		{ 1 , 8 , 16 , 8 , 16 , 0 , 0 , 2 },
		{ 0 , 1 , 0 , 0 , 0 , 0 , 10 , 0 , 0 , 40 , -80 , 40 },
	},
	{// eRGB_SHP_LEVEL_low
		{ 1 , 0 , 109 , 366 , 37 , 0 , 0 },
		{ 128 , -64 , 0 },
		{ 1 , 104 , 208 , 104 , 208 , 0 , 0 , 2 },
		{ 0 , 1 , 0 , 0 , 0 , 0 , 10 , 0 , 0 , 40 , -80 , 40 },
	},
	{// eRGB_SHP_LEVEL_mid
		{ 1 , 0 , 109 , 366 , 37 , 0 , 0 },
		{ 128 , -43 , -21 },
		{ 1 , 104 , 208 , 104 , 208 , 0 , 0 , 2 },
		{ 0 , 1 , 0 , 0 , 0 , 0 , 10 , 0 , 0 , 40 , -80 , 40 },
	},
	{// eRGB_SHP_LEVEL_high
		{ 1 , 0 , 109 , 366 , 37 , 0 , 0 },
		{ 128 , -32 , -32 },
		{ 1 , 104 , 208 , 104 , 208 , 0 , 0 , 2 },
		{ 0 , 1 , 0 , 0 , 0 , 0 , 10 , 0 , 0 , 40 , -80 , 40 },
	},
	{// eRGB_SHP_LEVEL_3tap
		{ 1 , 0 , 109 , 366 , 37 , 0 , 0 },
		{ 120 , -60 , 0 },
		{ 1 , 104 , 208 , 104 , 208 , 0 , 0 , 2 },
		{ 0 , 1 , 0 , 0 , 0 , 0 , 10 , 0 , 0 , 40 , -80 , 40 },
	},
	{// eRGB_SHP_LEVEL_5tap12
		{ 1 , 0 , 109 , 366 , 37 , 0 , 0 },
		{ 120 , -40 , -20 },
		{ 1 , 104 , 208 , 104 , 208 , 0 , 0 , 2 },
		{ 0 , 1 , 0 , 0 , 0 , 0 , 10 , 0 , 0 , 40 , -80 , 40 },
	},
	{// eRGB_SHP_LEVEL_5tap11
		{ 1 , 0 , 109 , 366 , 37 , 0 , 0 },
		{ 120 , -30 , -30 },
		{ 1 , 104 , 208 , 104 , 208 , 0 , 0 , 2 },
		{ 0 , 1 , 0 , 0 , 0 , 0 , 10 , 0 , 0 , 40 , -80 , 40 },
	},
	{// eRGB_SHP_LEVEL_5tap21
		{ 1 , 0 , 109 , 366 , 37 , 0 , 0 },
		{ 120 , -20 , -40 },
		{ 1 , 104 , 208 , 104 , 208 , 0 , 0 , 2 },
		{ 0 , 1 , 0 , 0 , 0 , 0 , 10 , 0 , 0 , 40 , -80 , 40 },
	},
	{// eRGB_SHP_LEVEL_5tap30
		{ 1 , 0 , 109 , 366 , 37 , 0 , 0 },
		{ 120 , -0 , -60 },
		{ 1 , 104 , 208 , 104 , 208 , 0 , 0 , 2 },
		{ 0 , 1 , 0 , 0 , 0 , 0 , 10 , 0 , 0 , 40 , -80 , 40 },
	},
	{// eRGB_SHP_LEVEL_5tap41
		{ 1 , 0 , 109 , 366 , 37 , 0 , 0 },
		{ 60 , 30 , -60 },
		{ 1 , 104 , 208 , 104 , 208 , 0 , 0 , 2 },
		{ 0 , 1 , 0 , 0 , 0 , 0 , 10 , 0 , 0 , 40 , -80 , 40 },
	},
	{// eRGB_SHP_LEVEL_5tapring
		{ 1 , 0 , 109 , 366 , 37 , 0 , 0 },
		{ 80 , -60 , 20 },
		{ 1 , 104 , 208 , 104 , 208 , 0 , 0 , 2 },
		{ 0 , 1 , 0 , 0 , 0 , 0 , 10 , 0 , 0 , 40 , -80 , 40 },
	},
};

VIP_RGB_SHP_LEVEL_TABLE gVipRgb_shpTemp = 
{
	{ 1 , 0 , 109 , 366 , 37 , 0 , 0 },
	{ 240 , -120 , 0 },
	{ 1 , 104 , 208 , 104 , 208 , 0 , 0 , 2 },
	{ 0 , 1 , 0 , 0 , 0 , 0 , 10 , 0 , 0 , 40 , -80 , 40 },
};

/* ----- ----- ----- ----- ----- FUNCTIONS ----- ----- ----- ----- ----- */

#ifndef VPQ_SET_REG_FIELD
#define VPQ_SET_REG_FIELD(REG,FIELD,VALUE) do{ (REG) = ((REG) & ~((unsigned int)FIELD##_mask)) + (FIELD((unsigned int)VALUE)); } while(0);
#endif

int VPQ_RgbShp_Lib_Get_value_byGain(int x,int xfenzi,int xfenmu)
{
	if( xfenmu<1 )
	{
		return x;
	}
	x = x<<1;
	x = x*xfenzi;
	x = x/xfenmu;
	x = x+1;
	x = x>>1;
	return x;
}

void VPQ_RgbShp_RegCtrl_SetRgb_shp_reg_db_apply(void)
{
	// rtd2875q no show
}

void VPQ_RgbShp_RegCtrl_SetRgb_shp_reg_table(VIP_RGB_SHP_LEVEL_TABLE* x)
{
	// rtd2875q no show
}

void VPQ_RgbShp_Lib_SetRgb_shp_PqLevel(unsigned char gain)
{
	if( gain>=eRGB_SHP_LEVEL__count )
	{
		return;
	}
	VPQ_RgbShp_RegCtrl_SetRgb_shp_reg_table( gVipRgb_shpTable+gain );
}

void VPQ_RgbShp_Lib_SetRgb_shp_Default(void)
{
	VPQ_RgbShp_Lib_SetRgb_shp_PqLevel( 1 );
}

void VPQ_RgbShp_Lib_SetRgb_shp_byGain(unsigned char x_table,int xfenzi,int xfenmu)
{
	if( x_table>=eRGB_SHP_LEVEL__count )
	{
		return;
	}
	memcpy( &gVipRgb_shpTemp , gVipRgb_shpTable+x_table , sizeof(gVipRgb_shpTemp) );
	if( xfenmu>0 && xfenzi<xfenmu )
	{
		if( xfenzi<0 )
		{
			xfenzi = 0;
		}
		gVipRgb_shpTemp.getd2v.d2_gain_pos = VPQ_RgbShp_Lib_Get_value_byGain( gVipRgb_shpTemp.getd2v.d2_gain_pos , xfenzi , xfenmu );
		gVipRgb_shpTemp.getd2v.d2_gain_neg = VPQ_RgbShp_Lib_Get_value_byGain( gVipRgb_shpTemp.getd2v.d2_gain_neg , xfenzi , xfenmu );
		gVipRgb_shpTemp.getd2v.d2_hv_pos = VPQ_RgbShp_Lib_Get_value_byGain( gVipRgb_shpTemp.getd2v.d2_hv_pos , xfenzi , xfenmu );
		gVipRgb_shpTemp.getd2v.d2_hv_neg = VPQ_RgbShp_Lib_Get_value_byGain( gVipRgb_shpTemp.getd2v.d2_hv_neg , xfenzi , xfenmu );
	}
	VPQ_RgbShp_RegCtrl_SetRgb_shp_reg_table( &gVipRgb_shpTemp );
}
