
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
#include <rtk_kdriver/io.h>
#include <tvscalercontrol/vip/rgb_sh.h>

/* ----- ----- ----- ----- ----- VARIABLES ----- ----- ----- ----- ----- */
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

void VPQ_RgbShp_RegCtrl_SetRgb_shp_reg_db_apply(void) {
	// RTK2851F add
	#if defined(CONFIG_ARCH_RTK2851F)
	unsigned int z_RBUS_reg;
	z_RBUS_reg = rtd_inl( COLOR_D_VC_Global_CTRL_DB_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_D_VC_Global_CTRL_DB_db_load_mask) | (COLOR_D_VC_Global_CTRL_DB_db_load( 1 ) );
	rtd_outl( COLOR_D_VC_Global_CTRL_DB_reg , z_RBUS_reg );
	#endif
}

void VPQ_RgbShp_RegCtrl_SetRgb_shp_reg_table(VIP_RGB_SHP_LEVEL_TABLE* x) {
	// RTK2851F add
	#if defined(CONFIG_ARCH_RTK2851F)
	unsigned int z_RBUS_reg;
	
	if( !x ) {
		return;
	}
	
	z_RBUS_reg = rtd_inl( COLOR_DLCTI_RGB_SHPNR_CTRL_RGB2Y_1_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_RGB2Y_1_rgb2y_g_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_RGB2Y_1_rgb2y_g( x->rgb2y.rgb2y_g ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_RGB2Y_1_rgb2y_r_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_RGB2Y_1_rgb2y_r( x->rgb2y.rgb2y_r ) );
	rtd_outl( COLOR_DLCTI_RGB_SHPNR_CTRL_RGB2Y_1_reg , z_RBUS_reg );

	z_RBUS_reg = rtd_inl( COLOR_DLCTI_RGB_SHPNR_CTRL_RGB2Y_2_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_RGB2Y_2_rgb_sharp_enable_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_RGB2Y_2_rgb_sharp_enable( x->rgb2y.rgb_sharp_enable ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_RGB2Y_2_rgb2y_b_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_RGB2Y_2_rgb2y_b( x->rgb2y.rgb2y_b ) );
	rtd_outl( COLOR_DLCTI_RGB_SHPNR_CTRL_RGB2Y_2_reg , z_RBUS_reg );

	z_RBUS_reg = rtd_inl( COLOR_DLCTI_RGB_SHPNR_CTRL_HPF_1_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_HPF_1_hpf_c_1_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_HPF_1_hpf_c_1( x->hpf5.hpf_c_1 ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_HPF_1_hpf_c_0_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_HPF_1_hpf_c_0( x->hpf5.hpf_c_0 ) );
	rtd_outl( COLOR_DLCTI_RGB_SHPNR_CTRL_HPF_1_reg , z_RBUS_reg );

	z_RBUS_reg = rtd_inl( COLOR_DLCTI_RGB_SHPNR_CTRL_HPF_2_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_HPF_2_hpf_c_2_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_HPF_2_hpf_c_2( x->hpf5.hpf_c_2 ) );
	rtd_outl( COLOR_DLCTI_RGB_SHPNR_CTRL_HPF_2_reg , z_RBUS_reg );

	z_RBUS_reg = rtd_inl( COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_1_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_1_d2_gain_pos_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_1_d2_gain_pos( x->getd2v.d2_gain_pos ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_1_d2_gain_neg_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_1_d2_gain_neg( x->getd2v.d2_gain_neg ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_1_d2_gpos2_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_1_d2_gpos2( x->getd2v.d2_gpos2 ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_1_d2_gneg2_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_1_d2_gneg2( x->getd2v.d2_gneg2 ) );
	rtd_outl( COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_1_reg , z_RBUS_reg );

	z_RBUS_reg = rtd_inl( COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_2_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_2_d2_lv_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_2_d2_lv( x->getd2v.d2_lv ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_2_d2_lv2_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_2_d2_lv2( x->getd2v.d2_lv2 ) );
	rtd_outl( COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_2_reg , z_RBUS_reg );

	z_RBUS_reg = rtd_inl( COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_3_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_3_d2_hv_pos_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_3_d2_hv_pos( x->getd2v.d2_hv_pos ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_3_d2_hv_neg_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_3_d2_hv_neg( x->getd2v.d2_hv_neg ) );
	rtd_outl( COLOR_DLCTI_RGB_SHPNR_CTRL_CURVE_MAP_3_reg , z_RBUS_reg );

	z_RBUS_reg = rtd_inl( COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_1_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_1_emf_offset_2_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_1_emf_offset_2( x->emf.emf_offset_2 ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_1_emf_offset_1_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_1_emf_offset_1( x->emf.emf_offset_1 ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_1_emf_offset_0_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_1_emf_offset_0( x->emf.emf_offset_0 ) );
	rtd_outl( COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_1_reg , z_RBUS_reg );

	z_RBUS_reg = rtd_inl( COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_2_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_2_emf_gain_2_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_2_emf_gain_2( x->emf.emf_gain_2 ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_2_emf_gain_1_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_2_emf_gain_1( x->emf.emf_gain_1 ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_2_emf_gain_0_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_2_emf_gain_0( x->emf.emf_gain_0 ) );
	rtd_outl( COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_2_reg , z_RBUS_reg );

	z_RBUS_reg = rtd_inl( COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_3_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_3_emf_enable_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_3_emf_enable( x->emf.emf_enable ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_3_emf_range_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_3_emf_range( x->emf.emf_range_h ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_3_emf_seg_x_1_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_3_emf_seg_x_1( x->emf.emf_seg_x_1 ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_3_emf_seg_x_0_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_3_emf_seg_x_0( x->emf.emf_seg_x_0 ) );
	rtd_outl( COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_3_reg , z_RBUS_reg );

	z_RBUS_reg = rtd_inl( COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_4_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_4_emf_weak_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_4_emf_weak( x->emf.emf_weak ) );
	rtd_outl( COLOR_DLCTI_RGB_SHPNR_CTRL_EMF_4_reg , z_RBUS_reg );

	z_RBUS_reg = rtd_inl( COLOR_DLCTI_RGB_SHPNR_CTRL_RGBLIMIT_reg );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_RGBLIMIT_rgblimit_pos_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_RGBLIMIT_rgblimit_pos( x->rgb2y.rgblimit_pos ) );
	z_RBUS_reg = ( z_RBUS_reg & ~COLOR_DLCTI_RGB_SHPNR_CTRL_RGBLIMIT_rgblimit_neg_mask) | (COLOR_DLCTI_RGB_SHPNR_CTRL_RGBLIMIT_rgblimit_neg( x->rgb2y.rgblimit_neg ) );
	rtd_outl( COLOR_DLCTI_RGB_SHPNR_CTRL_RGBLIMIT_reg , z_RBUS_reg );
	#endif
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
