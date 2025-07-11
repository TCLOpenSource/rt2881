#include <memc_isr/include/memc_lib.h>
#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#ifdef H5C2
#include "../common/include/rbus/mdomain_disp_main_reg.h"  //mdomain_disp_reg.h //spec_change
#endif
#include <tvscalercontrol/scaler/scalerstruct.h>
#include <tvscalercontrol/panel/panelapi.h>
#include "memc_reg_def.h"
#include "memc_isr/PQL/CalibMEMCInfo.h"

const _CALIB_SINGLE_PARAM CALIB_PARAM_RTK2885p[_PQL_OUT_RESOLUTION_MAX][CALIB_PARAM_NUM] = {

	{ //_PQL_OUT_1920x1080
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0, 	 		0}, //gmv cnt 			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
		
	{ //_PQL_OUT_1920x540
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0, 	 		0}, //gmv cnt 			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
		
	{ //_PQL_OUT_3840x2160
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0, 	 		0}, //gmv cnt 			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
		
	{ //_PQL_OUT_3840x1080
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	2,			0,			0}, //APL				Gain:2		Offset:0
		{	2,			0,			0}, //DTL				Gain:2		Offset:0
		{	2,			0,			0}, //SAD				Gain:2		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	2,			0,			0}, //mv				Gain:2		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	2,			0, 	 		0}, //gmv cnt 			Gain:2		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
		
	{ //_PQL_OUT_3840x540
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0, 	 		0}, //gmv cnt 			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
		
	{ //_PQL_OUT_768X480
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0, 	 		0}, //gmv cnt 			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
	
	{ //_PQL_OUT_1280X720
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0, 	 		0}, //gmv cnt 			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
		
	{ //_PQL_OUT_7680X4320
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0, 	 		0}, //gmv cnt 			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},

	{ //_PQL_OUT_2560x1440
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{2271,			10, 		0}, //APL				Gain:2.218		Offset:0
		{2337,			10, 		0}, //DTL				Gain:2.282		Offset:0
		{1607,			10, 	32211}, //SAD				Gain:1.569		Offset:32211
		{	1,			 0, 		0}, //sc th 			Gain:0.637		Offset:0
		{1536,			10, 		0}, //mv				Gain:1.5		Offset:0
		{ 683,			10, 		0}, //mv SR 			Gain:0.667		Offset:0
		{2758,			10, 	 	0}, //gmv cnt 			Gain:1.197*2.25	Offset:0
		{	9,			 2,			0}, //logo cnt 			Gain:2.25		Offset:0
		{	3,			 0, 		0}, //dhLogo_sad_th 	Gain:3			Offset:0
		{ 683,			10, 		0}, //dhLogo_mv_th		Gain:0.667		Offset:0
		{ 683,			10, 		0}, //dhLogo_mv_diff_th Gain:1.5		Offset:0
		{2368,			10,			0}, //periodic_cnt		Gain:2.312		Offset:0
	},
};

const _CALIB_SINGLE_PARAM CALIB_PARAM_RTK2885pp[_PQL_OUT_RESOLUTION_MAX][CALIB_PARAM_NUM] = {

	{ //_PQL_OUT_1920x1080
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0,			0}, //gmv cnt			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
		
	{ //_PQL_OUT_1920x540
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0,			0}, //gmv cnt			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
		
	{ //_PQL_OUT_3840x2160
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0,			0}, //gmv cnt			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
		
	{ //_PQL_OUT_3840x1080
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	2,			0,			0}, //APL				Gain:2		Offset:0
		{	2,			0,			0}, //DTL				Gain:2		Offset:0
		{	2,			0,			0}, //SAD				Gain:2		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	2,			0,			0}, //mv				Gain:2		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	2,			0,			0}, //gmv cnt			Gain:2		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
		
	{ //_PQL_OUT_3840x540
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0,			0}, //gmv cnt			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
		
	{ //_PQL_OUT_768X480
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0,			0}, //gmv cnt			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
	
	{ //_PQL_OUT_1280X720
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0,			0}, //gmv cnt			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},
		
	{ //_PQL_OUT_7680X4320
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{	1,			0,			0}, //APL				Gain:1		Offset:0
		{	1,			0,			0}, //DTL				Gain:1		Offset:0
		{	1,			0,			0}, //SAD				Gain:1		Offset:0
		{	1,			0,			0}, //sc th 			Gain:1		Offset:0
		{	1,			0,			0}, //mv				Gain:1		Offset:0
		{	1,			0,			0}, //mv SR 			Gain:1		Offset:0
		{	1,			0,			0}, //gmv cnt			Gain:1		Offset:0
		{	1,			0,			0}, //logo cnt			Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_sad_th 	Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_th		Gain:1		Offset:0
		{	1,			0,			0}, //dhLogo_mv_diff_th Gain:1		Offset:0
		{	1,			0,			0}, //periodic_cnt		Gain:1		Offset:0
	},

	{ //_PQL_OUT_2560x1440
	//gain_scale	gain_shift	  offset   ex: new_data = (old_data*gain_scale)>>gain_shift + offset
		{2271,			10, 		0}, //APL				Gain:2.218		Offset:0
		{2337,			10, 		0}, //DTL				Gain:2.282		Offset:0
		{1607,			10, 	32211}, //SAD				Gain:1.569		Offset:32211
		{	1,			 0, 		0}, //sc th 			Gain:0.637		Offset:0
		{1536,			10, 		0}, //mv				Gain:1.5		Offset:0
		{ 683,			10, 		0}, //mv SR 			Gain:0.667		Offset:0
		{2758,			10, 		0}, //gmv cnt			Gain:1.197*2.25 Offset:0
		{	9,			 2, 		0}, //logo cnt			Gain:2.25		Offset:0
		{	3,			 0, 		0}, //dhLogo_sad_th 	Gain:3			Offset:0
		{ 683,			10, 		0}, //dhLogo_mv_th		Gain:0.667		Offset:0
		{ 683,			10, 		0}, //dhLogo_mv_diff_th Gain:1.5		Offset:0
		{2368,			10, 		0}, //periodic_cnt		Gain:2.312		Offset:0
	},
};

_CALIB_SINGLE_PARAM CALIB_PARAM[_PQL_OUT_RESOLUTION_MAX][CALIB_PARAM_NUM] = {0};

const _CALIB_RESOLUTION_PARAM RESOLUTION_PARAM[CALIB_PARAM_RES_NUM][_PQL_OUT_RESOLUTION_MAX] =
{
	//UHD_PARAM_To_CUR_RES
	{
		//_PQL_OUT_1920x1080
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		1,			1,				1,				1},

		//_PQL_OUT_1920x540
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		1,			1,				1,				2},

		//_PQL_OUT_3840x2160
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		1,			0,				1,				0},

		//_PQL_OUT_3840x1080
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		1,			0,				1,				1},

		//_PQL_OUT_3840x540
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		1,			0,				1,				2},

		//_PQL_OUT_768X480
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		51,			8,				57,				8},
		
		//_PQL_OUT_1280X720
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		85,			8,				85,				8},

		//_PQL_OUT_7680X4320
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		2,			0,				2,				0},

		//_PQL_OUT_2560x1440
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		172,		8,				171,			8},
	},
	//UHD_RATIO_To_CUR_RES
	{
		//_PQL_OUT_1920x1080
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		2,			0,				2,				0},

		//_PQL_OUT_1920x540
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		2,			0,				4,				0},

		//_PQL_OUT_3840x2160
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		1,			0,				1,				0},

		//_PQL_OUT_3840x1080
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		1,			0,				2,				0},

		//_PQL_OUT_3840x540
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		1,			0,				4,				0},

		//_PQL_OUT_768X480
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		5,			0,				9,				1},
		
		//_PQL_OUT_1280X720
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		3,			0,				3,				0},

		//_PQL_OUT_7680X4320
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		1,			1,				1,				1},

		//_PQL_OUT_2560x1440
		//gain_scale_X	gain_shift_X	gain_scale_Y	gain_shift_Y
		{		3,			1,				3,				1},
	},
};

void CalibMEMCInfo_init(void)
{
#ifdef BUILD_QUICK_SHOW

#else
	if(RUN_MERLIN8P()){
		memcpy(CALIB_PARAM, CALIB_PARAM_RTK2885pp, sizeof(CALIB_PARAM));
	}else if(RUN_MERLIN8()){
		memcpy(CALIB_PARAM, CALIB_PARAM_RTK2885p, sizeof(CALIB_PARAM));
	}//RUN_MERLIN8P & RUN_MERLIN8
#endif
}

unsigned int ReadComReg_Calib_Data(unsigned int u32_Data, unsigned char param_index, int u32_Min, unsigned int u32_Max, bool u1_RegionData)
{
	PQL_OUTPUT_RESOLUTION output_Resolution = GetPQLContext()->_external_data._output_resolution;
	int s32_Offset = (u1_RegionData) ? ((CALIB_PARAM[output_Resolution][param_index].s32_Offset)>>5) : CALIB_PARAM[output_Resolution][param_index].s32_Offset;

	if(u32_Data < s32_Offset){
		return (unsigned int)_MAX_(_MIN_((((unsigned long long)u32_Data * CALIB_PARAM[output_Resolution][param_index].u24_Gain_Scale) >> (CALIB_PARAM[output_Resolution][param_index].u8_Gain_Shift)), u32_Max), u32_Min);
	}
	else if(u32_Data < s32_Offset*9){
		s32_Offset = (u32_Data-s32_Offset)>>3;
		return (unsigned int)_MAX_(_MIN_((((unsigned long long)u32_Data * CALIB_PARAM[output_Resolution][param_index].u24_Gain_Scale) >> (CALIB_PARAM[output_Resolution][param_index].u8_Gain_Shift))+s32_Offset, u32_Max), u32_Min);
	}
	return (unsigned int)_MAX_(_MIN_((((unsigned long long)u32_Data * CALIB_PARAM[output_Resolution][param_index].u24_Gain_Scale) >> (CALIB_PARAM[output_Resolution][param_index].u8_Gain_Shift))+s32_Offset, u32_Max), u32_Min);
}

unsigned int Calib_Data(unsigned int u32_Data, unsigned int u32_Gain, unsigned char u8_Shift)
{
	return (unsigned int)(((unsigned long long)u32_Data*u32_Gain)>>u8_Shift);
}

int Calib_MV(int s32_Data)
{
	PQL_OUTPUT_RESOLUTION output_Resolution = GetPQLContext()->_external_data._output_resolution;

	return (int)(((long long)s32_Data*(CALIB_PARAM[output_Resolution][CALIB_PARAM_MV].u24_Gain_Scale))>>(CALIB_PARAM[output_Resolution][CALIB_PARAM_MV].u8_Gain_Shift));
}

// example UHD param = 100, FHD param = 50
unsigned int Convert_UHD_Param_To_Current_RES(unsigned int Param, unsigned char Param_Direction)
{
	PQL_OUTPUT_RESOLUTION output_Resolution = GetPQLContext()->_external_data._output_resolution;
	unsigned int u10_gain = (Param_Direction==PARAM_DIRECTION_X) ? RESOLUTION_PARAM[UHD_PARAM_To_CUR_RES][output_Resolution].u10_Gain_Scale_X : (Param_Direction==PARAM_DIRECTION_Y) ? RESOLUTION_PARAM[UHD_PARAM_To_CUR_RES][output_Resolution].u10_Gain_Scale_Y : RESOLUTION_PARAM[UHD_PARAM_To_CUR_RES][output_Resolution].u10_Gain_Scale_X*RESOLUTION_PARAM[UHD_PARAM_To_CUR_RES][output_Resolution].u10_Gain_Scale_Y;
	unsigned char u4_shift = (Param_Direction==PARAM_DIRECTION_X) ? RESOLUTION_PARAM[UHD_PARAM_To_CUR_RES][output_Resolution].u4_Gain_Shift_X : (Param_Direction==PARAM_DIRECTION_Y) ? RESOLUTION_PARAM[UHD_PARAM_To_CUR_RES][output_Resolution].u4_Gain_Shift_Y : RESOLUTION_PARAM[UHD_PARAM_To_CUR_RES][output_Resolution].u4_Gain_Shift_X+RESOLUTION_PARAM[UHD_PARAM_To_CUR_RES][output_Resolution].u4_Gain_Shift_Y;

	return (unsigned int)((((unsigned long long)Param)*u10_gain)>>u4_shift);
}

// example BOT_ratio
// UHD diff=216, BOT_ratio of UHD = 216/2160 = 0.1
// FHD diff=108, BOT_ratio of FHD = 108/1080 = 0.1
unsigned int Convert_Current_RES_RATIO_To_UHD(unsigned int Ratio, unsigned char Param_Direction)
{
	PQL_OUTPUT_RESOLUTION output_Resolution = GetPQLContext()->_external_data._output_resolution;
	unsigned int u10_gain = (Param_Direction==PARAM_DIRECTION_X) ? RESOLUTION_PARAM[UHD_RATIO_To_CUR_RES][output_Resolution].u10_Gain_Scale_X : (Param_Direction==PARAM_DIRECTION_Y) ? RESOLUTION_PARAM[UHD_RATIO_To_CUR_RES][output_Resolution].u10_Gain_Scale_Y : RESOLUTION_PARAM[UHD_RATIO_To_CUR_RES][output_Resolution].u10_Gain_Scale_X*RESOLUTION_PARAM[UHD_RATIO_To_CUR_RES][output_Resolution].u10_Gain_Scale_Y;
	unsigned char u4_shift = (Param_Direction==PARAM_DIRECTION_X) ? RESOLUTION_PARAM[UHD_RATIO_To_CUR_RES][output_Resolution].u4_Gain_Shift_X : (Param_Direction==PARAM_DIRECTION_Y) ? RESOLUTION_PARAM[UHD_RATIO_To_CUR_RES][output_Resolution].u4_Gain_Shift_Y : RESOLUTION_PARAM[UHD_RATIO_To_CUR_RES][output_Resolution].u4_Gain_Shift_X+RESOLUTION_PARAM[UHD_RATIO_To_CUR_RES][output_Resolution].u4_Gain_Shift_Y;

	return (unsigned int)((((unsigned long long)Ratio)*u10_gain)>>u4_shift);
}

