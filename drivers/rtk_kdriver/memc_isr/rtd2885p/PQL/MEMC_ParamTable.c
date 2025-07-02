//#include <memc_isr/include/memc_lib.h>
#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
//#ifdef H5C2
//#include "../common/include/rbus/mdomain_disp_main_reg.h"  //mdomain_disp_reg.h //spec_change
//#endif
//#include <tvscalercontrol/scaler/scalerstruct.h>
//#include "memc_isr/MID/mid_mode.h"
//#include <tvscalercontrol/panel/panelapi.h>
//#include "memc_reg_def.h"
//#include "memc_isr/include/PQLAPI.h"
#include "memc_isr/PQL/MEMC_ParamTable.h"

const _MEMC_STYLE_TABLE MEMC_STYLE_TABLE = {
{
	//FB_STYLE_NORMAL
	{
		1, // u1_GFB_MVChaos_total_en
		1, // u1_GFB_MVChaos_SC_en
		0, // u1_GFB_MVChaos_unconf_en
		80, // u7_GFB_MVChaos_SC_th
		47, // u8_GFB_MVChaos_unconf_th
		
		1, // u1_GFB_FastMotion_en
		159, // u8_GFB_FastMotion_mv_th
		15, // u5_GFB_FastMotion_large_rmv_num

		1, // u1_LFB_Dtl_curve_en
		20, // u8_LFB_Dtl_strength_ratio
		
		1, // u1_LFB_MV_diff_curve_en
		40, // u8_LFB_MV_diff_strength_ratio

		0, // u10_LFB_dh_ph_fbgen_min //0~1023
		1023, // u10_LFB_dh_ph_fbgen_max; //0~1023
		5, // u8_LFB_dh_ph_fbgen_slp; //0~255
		1, // u4_LFB_dh_ph_fbgen_sft //0~15
	},

	//FB_STYLE_MORE
	{
		1, // u1_GFB_MVChaos_total_en
		1, // u1_GFB_MVChaos_SC_en
		0, // u1_GFB_MVChaos_unconf_en
		80, // u7_GFB_MVChaos_SC_th
		47, // u8_GFB_MVChaos_unconf_th
		
		1, // u1_GFB_FastMotion_en
		159, // u8_GFB_FastMotion_mv_th
		15, // u5_GFB_FastMotion_large_rmv_num
	
		1, // u1_LFB_Dtl_curve_en
		30, // u8_LFB_Dtl_strength_ratio
		
		1, // u1_LFB_MV_diff_curve_en
		50, // u8_LFB_MV_diff_strength_ratio
	
		0, // u10_LFB_dh_ph_fbgen_min //0~1023
		1023, // u10_LFB_dh_ph_fbgen_max; //0~1023
		6, // u8_LFB_dh_ph_fbgen_slp; //0~255
		0, // u4_LFB_dh_ph_fbgen_sft //0~15
	},

	//FB_STYLE_LESS
	{
		1, // u1_GFB_MVChaos_total_en
		1, // u1_GFB_MVChaos_SC_en
		0, // u1_GFB_MVChaos_unconf_en
		80, // u7_GFB_MVChaos_SC_th
		47, // u8_GFB_MVChaos_unconf_th
		
		0, // u1_GFB_FastMotion_en
		159, // u8_GFB_FastMotion_mv_th
		15, // u5_GFB_FastMotion_large_rmv_num
	
		1, // u1_LFB_Dtl_curve_en
		5, // u8_LFB_Dtl_strength_ratio
		
		1, // u1_LFB_MV_diff_curve_en
		5, // u8_LFB_MV_diff_strength_ratio
	
		0, // u10_LFB_dh_ph_fbgen_min //0~1023
		1023, // u10_LFB_dh_ph_fbgen_max; //0~1023
		2, // u8_LFB_dh_ph_fbgen_slp; //0~255
		4, // u4_LFB_dh_ph_fbgen_sft //0~15
	},

	//FB_STYLE_CINEMA
	{
		1, // u1_GFB_MVChaos_total_en
		1, // u1_GFB_MVChaos_SC_en
		0, // u1_GFB_MVChaos_unconf_en
		80, // u7_GFB_MVChaos_SC_th
		47, // u8_GFB_MVChaos_unconf_th
		
		1, // u1_GFB_FastMotion_en
		159, // u8_GFB_FastMotion_mv_th
		15, // u5_GFB_FastMotion_large_rmv_num
	
		1, // u1_LFB_Dtl_curve_en
		48, // u8_LFB_Dtl_strength_ratio
		
		1, // u1_LFB_MV_diff_curve_en
		80, // u8_LFB_MV_diff_strength_ratio
	
		0, // u10_LFB_dh_ph_fbgen_min //0~1023
		1023, // u10_LFB_dh_ph_fbgen_max; //0~1023
		6, // u8_LFB_dh_ph_fbgen_slp; //0~255
		0, // u4_LFB_dh_ph_fbgen_sft //0~15
	},
},

//ME_STYLE
{
	//ME_STYLE_NORMAL
	{
		//flp_alp
		0,
		{0x40,0x30,0x20,0x10,0x0,0x0,0x0,0x40,0x30,0x20,0x10,0x0,0x0,0x0},

		//LFSR
		0,
		{CTRL_NC,CTRL_NC,CTRL_NC,CTRL_NC,CTRL_NC,CTRL_NC,CTRL_NC,CTRL_NC,CTRL_NC,CTRL_NC,CTRL_NC,CTRL_NC,CTRL_NC,CTRL_NC},

		//ME_cand
		0,
		{CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE,CTRL_NONE},
	},
	//ME_STYLE_SLOW
	{
		//flp_alp
		1,
		{0x20,0x1c,0x18,0x14,0x10,0xe,0xa,0x20,0x1c,0x18,0x14,0x10,0xe,0xa}, /*2*/ //repeat MID/HIGH

		//LFSR
		1,
		{0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1}, /*9*/ //repeat MID/HIGH
		
		//ME_cand
		1,
		{CTRL_NONE,0,CTRL_NONE,CTRL_NONE,CTRL_NONE,0,0,0,0,CTRL_NONE,0,CTRL_NONE,CTRL_NONE,CTRL_NONE,0,0,0,0,CTRL_NONE,CTRL_NONE,CTRL_NONE,0,CTRL_NONE,CTRL_NONE,CTRL_NONE,0,0,0},	/*8 repeat broken*/
	},
},
};

const _MEMC_PARAM_TABLE MEMC_PARAM_TABLE = {

	//SAD_SHIFT_PARAM 
	//get the SAD_SHIFT_INFO method: https://wiki.realtek.com/x/9Rs2Eg
	{
	//top_rim	sad_offset
		{10,	0x3bfe2}, /*_PQL_OUT_1920x1080*/
		{10,	0x3bfe2}, /*_PQL_OUT_1920x540*/
		{10,	0x3bfe2}, /*_PQL_OUT_3840x2160*/
		{ 5,	0x1dff1}, /*_PQL_OUT_3840x1080*/
		{10,	0x3bfe2}, /*_PQL_OUT_3840x540*/
		{10,	0x3bfe2}, /*_PQL_OUT_768x480*/
		{10,	0x3bfe2}, /*_PQL_OUT_1280x720*/
		{10,	0x3bfe2}, /*_PQL_OUT_7680x4320*/
		{ 8,	0x27FEC}, /*_PQL_OUT_2560x1440*/
	},
	
	//RIM_Param
	{
	//												RimCtrl_Param																	  BBD_ACTIVE_TH_Param
	//	width	height		bound(T,B,R,L)		   divide	slowin_min_shift   RimDiffTh_cof	apl_size	scale(H,V)		H_PRECISE	H_SKETCHY	V_PRECISE	V_SKETCHY
		{{1920, 1080,	{180,  900, 1600,  320},	2, 1,		12, 12, 			1,			64800,		2, 2},			{64,		256,		32, 		128}},	/*_PQL_OUT_1920x1080*/	
		{{1920,	540,	{ 90,  450,  160, 3200},	2, 0,		12,  6, 			1,			64800,		2, 1},			{64,		256,		16, 		64 }},	/*_PQL_OUT_1920x540*/	
		{{3840, 2160,	{360, 1800, 3200,  640},	3, 2,		24, 24, 			1,			64800,		4, 4},			{128,		512,		64, 		256}},	/*_PQL_OUT_3840x2160*/	
		{{3840, 1080,	{180,  900, 3200,  640},	3, 1,		24, 12, 			1,			64800,		4, 2},			{128,		512,		32, 		128}},	/*_PQL_OUT_3840x1080*/	
		{{3840,	540,	{ 90,  450, 3200,  640},	3, 0,		24,  6, 			1,			64800,		4, 1},			{128,		512,		16, 		64 }},	/*_PQL_OUT_3840x540*/	
		{{ 768,	480,	{ 80,  400,  640,  128},	0, 0,		 5,  5, 			1,			64800,		1, 1},			{26,		102,		14, 		57 }},	/*_PQL_OUT_768x480*/	
		{{1280,	720,	{120,  600, 1066,  214},	1, 0,		 8,  8, 			1,			64800,		1, 1},			{43,		171,		21, 		85 }},	/*_PQL_OUT_1280x720*/	
		{{7680, 4320,	{720, 3600, 6400, 1280},	4, 3,		48, 48, 			2,			64800,		8, 8},			{256,		1024,		128,		512}},	/*_PQL_OUT_7680x4320*/	
		{{2560, 1440,	{240, 1200, 2100,  420},	3, 2,		24, 24, 			1,			64800,		3, 3},			{96,		384,		48, 		192}},	/*_PQL_OUT_2560x1440*/	
	},
	
	//LOGO_Param
	{	
	// 	  LOGO_DETECT_EDGE_Param			 MC_LOGO_EROSION_TH_Param
	//	hor 	P45 	ver 	N45			panning	fast-motion   normal
		{{0x3,	0x6,	0x4,	0xb},		{3, 		6,			6}},	/*_PQL_OUT_1920x1080*/
		{{0x3,	0x6,	0x4,	0xb},		{3, 		6,			6}},	/*_PQL_OUT_1920x540*/
		{{0x3,	0x6,	0x4,	0xb},		{3, 		6,			6}},	/*_PQL_OUT_3840x2160*/ //{0x9, 0xc, 0x8, 0xb}
		{{0x3,	0x6,	0x4,	0xb},		{3, 		6,			6}},	/*_PQL_OUT_3840x1080*/
		{{0x3,	0x6,	0x4,	0xb},		{3, 		6,			6}},	/*_PQL_OUT_3840x540*/
		{{0x3,	0x6,	0x4,	0xb},		{3, 		6,			6}},	/*_PQL_OUT_768x480*/
		{{0x3,	0x6,	0x4,	0xb},		{3, 		6,			6}},	/*_PQL_OUT_1280x720*/
		{{0x3,	0x6,	0x4,	0xb},		{3, 		6,			6}},	/*_PQL_OUT_7680x4320*/
		{{0xa,	0x15,	0xe,	0x26},		{5, 		6,			8}},	/*_PQL_OUT_2560x1440*/
	},
	
	//FB_Param
	{
	//			FB_BAD_REGION_TH					FB_TC_TH					MC_FBLVL_FILTER_PARAM				u8_TempConsis_shift
	//	 low th 	mid th		high th 	  low th	high th 	enable	mode	cut 	th		avg_th	num
		{{0xe5a8,	0x1C710,	0x4A710},	{0x177e20,	0x1d7e20},	{0x1,	0x2,	0x1e,	0x07,	0x04,	0x8},			0}, /*_PQL_OUT_1920x1080*/
		{{0xe5a8,	0x1C710,	0x4A710},	{0x177e20,	0x1d7e20},	{0x1,	0x2,	0x1e,	0x07,	0x04,	0x8},			0}, /*_PQL_OUT_1920x540*/
		{{0xDF51,	0x3A651,	0x5CF28},	{0x177e20,	0x1d7e20},	{0x1,	0x0,	0x1e,	0x0c,	0x04,	0xf},			0}, /*_PQL_OUT_3840x2160*/
//		{{0x22E4C,	0x91fcc,	0xe85e6},	{0x177e20,	0x1d7e20},	{0x1,	0x0,	0x1e,	0x0c,	0x04,	0xf},			0}, /*_PQL_OUT_3840x2160*/
//		{{0x1bea3,	0x74ca3,	0xb9e51},	{0x1071e3,	0x1d7e20},	{0x1,	0x0,	0x1e,	0x0c,	0x04,	0xf},			0}, /*_PQL_OUT_3840x2160*/
		{{0xe5a8,	0x1C710,	0x4A710},	{0x177e20,	0x1d7e20},	{0x1,	0x0,	0x1e,	0x0c,	0x04,	0xf},			1}, /*_PQL_OUT_3840x1080*/
		{{0xe5a8,	0x1C710,	0x4A710},	{0x177e20,	0x1d7e20},	{0x1,	0x0,	0x1e,	0x0c,	0x04,	0xf},			0}, /*_PQL_OUT_3840x540*/
		{{0xe5a8,	0x1C710,	0x4A710},	{0x177e20,	0x1d7e20},	{0x1,	0x2,	0x1e,	0x07,	0x04,	0x8},			0}, /*_PQL_OUT_768x480*/
		{{0xe5a8,	0x1C710,	0x4A710},	{0x177e20,	0x1d7e20},	{0x1,	0x2,	0x1e,	0x07,	0x04,	0x8},			0}, /*_PQL_OUT_1280x720*/
		{{0xe5a8,	0x1C710,	0x4A710},	{0x177e20,	0x1d7e20},	{0x1,	0x0,	0x1e,	0x0c,	0x04,	0xf},			0}, /*_PQL_OUT_7680x4320*/
		{{0xef3d,	0x3267a,	0x62031},	{0x177e20,	0x1d7e20},	{0x1,	0x0,	0x1e,	0x0c,	0x04,	0xf},			0}, /*_PQL_OUT_2560x1440*/
	},
	
	//SC_Param
	{
	//		scCtrl_tuning_th
	//	ratio_l	ratio_h	pure_video	 	panning	 hist_diff_ratio
		{{0xa,	0xd},	{0x980000,		0x400000},		{70}}, /*_PQL_OUT_1920x1080*/
		{{0xa,	0xd},	{0x980000,		0x400000},		{70}}, /*_PQL_OUT_1920x540*/
		{{0xa,	0xd},	{0x8e7fba,		0x3a497c},		{70}}, /*_PQL_OUT_3840x2160*/
		{{0xa,	0xd},	{0xAF0000,		0x400000},		{70}}, /*_PQL_OUT_3840x1080*/
		{{0xa,	0xd},	{0x980000,		0x400000},		{70}}, /*_PQL_OUT_3840x540*/
		{{0xa,	0xd},	{0x980000,		0x400000},		{70}}, /*_PQL_OUT_768x480*/
		{{0xa,	0xd},	{0x980000,		0x400000},		{70}}, /*_PQL_OUT_1280x720*/
		{{0xa,	0xd},	{0x980000,		0x400000},		{70}}, /*_PQL_OUT_7680x4320*/
		{{0xa,	0xd},	{0x1370d92, 	0x4dcd6c},		{50}}, /*_PQL_OUT_2560x1440*/
	},
	
	//MV_SEARCHRANGE_Param
	{
	//		  SEARCH_RANGE_CLIP
	// 	   oversearch	   normal		random_mask_offset
		{ {{0x2d, 0x2d}, {0x2a, 0x2a}}, 		0}, /*_PQL_OUT_1920x1080*/	
		{ {{0x2d, 0x2d}, {0x2a, 0x2a}}, 		0}, /*_PQL_OUT_1920x540*/	
		{ {{0x2d, 0x2d}, {0x2a, 0x2a}}, 		0}, /*_PQL_OUT_3840x2160*/	
		{ {{0x2d, 0x2d}, {0x2a, 0x2a}}, 		0}, /*_PQL_OUT_3840x1080*/	
		{ {{0x2d, 0x2d}, {0x2a, 0x2a}}, 		0}, /*_PQL_OUT_3840x540*/	
		{ {{0x2d, 0x2d}, {0x2a, 0x2a}}, 		0}, /*_PQL_OUT_768x480*/	
		{ {{0x2d, 0x2d}, {0x2a, 0x2a}}, 		0}, /*_PQL_OUT_1280x720*/	
		{ {{0x2d, 0x2d}, {0x2a, 0x2a}}, 		0}, /*_PQL_OUT_7680x4320*/	
		{ {{0x1e, 0x1e}, {0x1c, 0x1c}}, 		-1}, /*_PQL_OUT_2560x1440*/	
	},

	//MC_Param
	{
	//mc_vartap_sel		knr_half_vactive_table
		{0,						540}, /*_PQL_OUT_1920x1080*/
		{0,						540}, /*_PQL_OUT_1920x540*/
		{2,						540}, /*_PQL_OUT_3840x2160*/
		{2,						272}, /*_PQL_OUT_3840x1080*/
		{2,						540}, /*_PQL_OUT_3840x540*/
		{0,						540}, /*_PQL_OUT_768x480*/
		{0,						540}, /*_PQL_OUT_1280x720*/
		{2,						540}, /*_PQL_OUT_7680x4320*/
		{1,						540}, /*_PQL_OUT_2560x1440*/
	},

	//DEHALO_PARAM
	{
	//dh_phflt_med9flt_data_sel
		{1}, /*_PQL_OUT_1920x1080*/
		{1}, /*_PQL_OUT_1920x540*/
		{2}, /*_PQL_OUT_3840x2160*/
		{2}, /*_PQL_OUT_3840x1080*/
		{2}, /*_PQL_OUT_3840x540*/
		{1}, /*_PQL_OUT_768x480*/
		{1}, /*_PQL_OUT_1280x720*/
		{2}, /*_PQL_OUT_7680x4320*/
		{2}, /*_PQL_OUT_2560x1440*/
	},
	
	//BI_PARAM
	{
	//bi_blk_res_sel
		{1}, /*_PQL_OUT_1920x1080*/
		{1}, /*_PQL_OUT_1920x540*/
		{0}, /*_PQL_OUT_3840x2160*/
		{0}, /*_PQL_OUT_3840x1080*/
		{0}, /*_PQL_OUT_3840x540*/
		{1}, /*_PQL_OUT_768x480*/
		{1}, /*_PQL_OUT_1280x720*/
		{0}, /*_PQL_OUT_7680x4320*/
		{1}, /*_PQL_OUT_2560x1440*/
	},

	//ACTIVE_PARAM
	{
	//u1_special_v_active	v_active
		{0,						0}, /*_PQL_OUT_1920x1080*/
		{0,						0}, /*_PQL_OUT_1920x540*/
		{0,						0}, /*_PQL_OUT_3840x2160*/
		{1,						272}, /*_PQL_OUT_3840x1080*/
		{0,						0}, /*_PQL_OUT_3840x540*/
		{0,						0}, /*_PQL_OUT_768x480*/
		{0,						0}, /*_PQL_OUT_1280x720*/
		{0,						0}, /*_PQL_OUT_7680x4320*/
		{0,						0}, /*_PQL_OUT_2560x1440*/
	},

	//PERIODIC_PARAM
	{
	//u10_logn_term_cnt_th
		{240}, /*_PQL_OUT_1920x1080*/
		{240}, /*_PQL_OUT_1920x540*/
		{240}, /*_PQL_OUT_3840x2160*/
		{240}, /*_PQL_OUT_3840x1080*/
		{240}, /*_PQL_OUT_3840x540*/
		{240}, /*_PQL_OUT_768x480*/
		{240}, /*_PQL_OUT_1280x720*/
		{240}, /*_PQL_OUT_7680x4320*/
		{120}, /*_PQL_OUT_2560x1440*/
	},
};

