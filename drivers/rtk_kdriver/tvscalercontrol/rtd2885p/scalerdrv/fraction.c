#include <rtd_log/rtd_module_log.h>
#include <tvscalercontrol/scalerdrv/fraction.h>
#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
#else
	#include <rtk_kdriver/scalercommon/scalerCommon.h>
#endif

/*
* fraction remove, mutilple setting table
*/

struct fraction_info fractionInfo[] =
{
    {V_FREQ_24000_mOFFSET,  V_FREQ_24500,           {   {60,    48,     1,  2},             // cinema, force framerate on adaptive stream
                                                        {60,    60,     2,  5},
                                                        {120,   120,    1,  5},
                                                        {144,   120,    1,  5},             // VDEC in 4K144Hz panel by shrinking 4K120Hz panel's H & V porch
                                                        {144,   144,    1,  6},
                                                        {160,   144,    1,  6},
                                                        {165,   144,    1,  6},
                                                        {240,   240,    1,  10} }},
    {V_FREQ_24500,          V_FREQ_25000_pOFFSET,   {   {60,    50,     1,  2},
//                                                        {60,    60,     5,  12},
                                                        {120,   100,    1,  4},
//                                                        {120,   120,    5,  24},
                                                        {144,   125,    1,  5},
                                                        {160,   150,    1,  6},
                                                        {165,   150,    1,  6},
                                                        {240,   225,    1,  9}  }},
    {V_FREQ_30000_mOFFSET,  V_FREQ_30000_pOFFSET,   {   {60,    60,     1,  2},
                                                        {120,   120,    1,  4},
                                                        {144,   120,    1,  4},
                                                        {160,   150,    1,  5},
                                                        {165,   150,    1,  5},
                                                        {240,   240,    1,  8}  }},
    {V_FREQ_48000_mOFFSET,  V_FREQ_48000_pOFFSET,   {   {60,    48,     1,  1},
//                                                        {60,    60,     4,  5},
                                                        {120,   96,     1,  2},
                                                        {144,   144,    1,  3},
                                                        {160,   144,    1,  3},
                                                        {165,   144,    1,  3},
                                                        {240,   240,    1,  5}  }},
    {V_FREQ_50000_mOFFSET,  V_FREQ_50000_pOFFSET,   {   {60,    50,     1,  1},
                                                        {60,    60,     5,  6},
                                                        {120,   100,    1,  2},
//                                                        {120,   120,    5,  12},
                                                        {144,   100,    1,  2},
                                                        {160,   150,    1,  3},
                                                        {165,   150,    1,  3},
                                                        {240,   200,    1,  4}  }},
    {V_FREQ_60000_mOFFSET,  V_FREQ_60000_pOFFSET,   {   {60,    60,     1,  1},
                                                        {120,   120,    1,  2},
                                                        {144,   120,    1,  2},
                                                        {160,   120,    1,  2},
                                                        {165,   120,    1,  2},
                                                        {240,   240,    1,  4}  }},
    {V_FREQ_100000_mOFFSET, V_FREQ_100000_pOFFSET,  {   {60,    50,     2,  1},             // 2K1K 100Hz
                                                        {120,   100,    1,  1},
                                                        {144,   100,    1,  1},
                                                        {160,   100,    1,  1},
                                                        {165,   100,    1,  1},
                                                        {240,   200,    1,  2}  }},
    {V_FREQ_120000_mOFFSET, V_FREQ_120000_pOFFSET,  {   {60,    60,     2,  1},             // 2K1K 120Hz
                                                        {120,   120,    1,  1},
                                                        {144,   120,    1,  1},
                                                        {160,   120,    1,  1},
                                                        {165,   120,    1,  1},
                                                        {240,   240,    1,  2}  }},
    {V_FREQ_144000_mOFFSET, V_FREQ_144000_pOFFSET,  {   {144,   144,    1,  1},
                                                        {160,   144,    1,  1},
                                                        {165,   144,    1,  1},
                                                        {240,   144,    1,  1},
                                                        {288,   288,    1,  2}  }},
    {V_FREQ_165000_mOFFSET, V_FREQ_165000_pOFFSET,  {   {165,   165,    1,  1},
                                                        {240,   165,    1,  1}  }},
    {V_FREQ_200000_mOFFSET, V_FREQ_200000_pOFFSET,  {   {60,    50,     4,  1},             // 2K1K 200Hz
                                                        {120,   100,    2,  1},             // 2K1K 200Hz
                                                        {144,   100,    2,  1},             // 2K1K 200Hz
                                                        {240,   200,    1,  1}  }},
    {V_FREQ_240000_mOFFSET, V_FREQ_240000_pOFFSET,  {   {60,    60,     4,  1},             // 2K1K 240Hz
                                                        {120,   120,    2,  1},             // 2K1K 240Hz
                                                        {144,   120,    2,  1},             // 2K1K 240Hz
                                                        {240,   240,    1,  1}  }},
    {V_FREQ_288000_mOFFSET, V_FREQ_288000_pOFFSET,  {   {144,   144,    2,  1},             // 2K1K 288Hz
                                                        {288,   288,    1,  1}  }}
};

void get_scaler_remove_multiple(unsigned int inputFrameRate, unsigned int dtg_master_v_freq, unsigned int panel_refresh, unsigned int *remove, unsigned int *multiple)
{
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int count = sizeof(fractionInfo) / sizeof(struct fraction_info);

	if( ( remove == NULL ) || ( multiple == NULL ) )
	{
		rtd_pr_vbe_err("[%s][line:%d][ERROR] param error\n", __FUNCTION__, __LINE__);
		return;
	}

	for( i = 0; i < count ; i++ )
	{
		if( ( inputFrameRate >= fractionInfo[i].inputFrameRateStart ) && ( inputFrameRate < fractionInfo[i].inputFrameRateEnd ) )
		{
			for( j = 0 ; j < REMOVE_MULTIPLE_MAX ; j++ )
			{
				if( panel_refresh == fractionInfo[i].rm_multiple[j].panel_refresh )
				{
					if( dtg_master_v_freq == fractionInfo[i].rm_multiple[j].dtg_master_v_freq )
					{
						*remove = fractionInfo[i].rm_multiple[j].remove;
						*multiple = fractionInfo[i].rm_multiple[j].multiple;
						rtd_pr_vbe_notice("[%s] panel_refresh : %d, inputFrameRate : %d, dtg_master_v_freq : %d, remove : multiple = %d : %d\n", __FUNCTION__, panel_refresh, inputFrameRate, dtg_master_v_freq, *remove, *multiple);
						return;
					}
				}
			}
		}
	}

	if( panel_refresh > 60 )
	{
		if( inputFrameRate > 90000 )
		{
			*remove = 1;
			*multiple = 1;
		}
		else
		{
			*remove = 1;
			*multiple = 2;
		}
	}
	else
	{
		*remove = 1;
		*multiple = 1;
	}

	rtd_pr_vbe_notice("[%s] ERROR : cannot find valied remove/mutiple info, please check remove/mutiple info config!!!, defalut(%d:%d)\n", __FUNCTION__, *remove, *multiple);
}
