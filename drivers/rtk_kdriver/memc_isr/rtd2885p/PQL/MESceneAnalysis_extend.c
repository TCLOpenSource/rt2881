#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "rtk_vip_logger.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_reg_def.h"

#define MEMC_EN_AI (0)
#if MEMC_EN_AI
#include <ioctrl/ai/ai_cmd_id.h>
#endif
#ifdef BUILD_QUICK_SHOW
#define abs(x)                                    ((x<0) ? x : -x)
#endif
#include "memc_isr/PQL/MESceneAnalysis.h"

#ifdef CONFIG_ARM64 //ARM32 compatible
//#include "io.h"

// for register dump
#include <tvscalercontrol/io/ioregdrv.h>

#undef rtd_outl
#define rtd_outl(x, y)     								IoReg_Write32(x,y)
#undef rtd_inl
#define rtd_inl(x)     									IoReg_Read32(x)
#undef rtd_maskl
#define rtd_maskl(x, y, z)     							IoReg_Mask32(x,y,z)
#undef rtd_setbits
#define rtd_setbits(offset, Mask) rtd_outl(offset, (rtd_inl(offset) | Mask))
#undef rtd_clearbits
#define rtd_clearbits(offset, Mask) rtd_outl(offset, ((rtd_inl(offset) & ~(Mask))))
#else
#include "io.h"
#endif

//#undef VIPprintf
//#define VIPprintf(fmt, args...)	VIPprintfPrivate(VIP_LOGLEVEL_DEBUG,VIP_LOGMODULE_MEMC_IP30_DEBUG,fmt,##args)

extern unsigned int MA_print_count;
#define ROSPrintf_MEMC_IP30(fmt, args...) VIPprintf(VIP_LOGLEVEL_DEBUG,VIP_LOGMODULE_MEMC_IP30_DEBUG,MA_print_count,fmt,##args)
#define ROSPrintf_MEMC_IP30_interface1(fmt, args...) VIPprintf(VIP_LOGLEVEL_DEBUG,VIP_LOGMODULE_MEMC_IP30_interface1_DEBUG,MA_print_count,fmt,##args)
#define ROSPrintf_MEMC_IP30_interface2(fmt, args...) VIPprintf(VIP_LOGLEVEL_DEBUG,VIP_LOGMODULE_MEMC_IP30_interface2_DEBUG,MA_print_count,fmt,##args)
#define ROSPrintf_MEMC_IP30_interface3(fmt, args...) VIPprintf(VIP_LOGLEVEL_DEBUG,VIP_LOGMODULE_MEMC_IP30_interface3_DEBUG,MA_print_count,fmt,##args)
#define ROSPrintf_MEMC_IP30_interface4(fmt, args...) VIPprintf(VIP_LOGLEVEL_DEBUG,VIP_LOGMODULE_MEMC_IP30_interface4_DEBUG,MA_print_count,fmt,##args)
#define ROSPrintf_MEMC_IP30_interface5(fmt, args...) VIPprintf(VIP_LOGLEVEL_DEBUG,VIP_LOGMODULE_MEMC_IP30_interface5_DEBUG,MA_print_count,fmt,##args)
#define ROSPrintf_MEMC_IP30_interface6(fmt, args...) VIPprintf(VIP_LOGLEVEL_DEBUG,VIP_LOGMODULE_MEMC_IP30_interface6_DEBUG,MA_print_count,fmt,##args)
#define ROSPrintf_MEMC_IP30_interface7(fmt, args...) VIPprintf(VIP_LOGLEVEL_DEBUG,VIP_LOGMODULE_MEMC_IP30_interface7_DEBUG,MA_print_count,fmt,##args)
#define MEMC_IP30_NULL_Print_Cnt (300)
#define CLIP(min, max, val) ((val < min) ? min : (val > max ? max : val))


//////////////////////////////////////////////////////////////////////////

#define FRAME_CHANGE_CNT	5

unsigned char pre_unstable_score[32] = {0};

extern unsigned char scalerVIP_Get_MEMCPatternFlag_Identification(unsigned short prj,unsigned short nNumber);
//////////////////////////////////////////////////////////////////////////

inline VOID _Median3_(int x0, int x1, int x2, int *med_val, unsigned char *med_idx);
inline VOID _Median5_(int x0, int x1, int x2, int x3, int x4, int *med_val, unsigned char *med_idx);

VOID Me_sceneAnalysis_Init_RTK2885pp(_OUTPUT_ME_SCENE_ANALYSIS *pOutput)
{
	/* SIS MEMC Initial here */
	MEMC_MiddleWare_StatusInit_RTK2885pp(pOutput);

	pOutput->u1_RP_detect_true = 0;
}

VOID Me_sceneAnalysis_Proc_OutputIntp_RTK2885pp(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput, int iSliceIdx)
{
	MEMC_MiddleWare_StatusUpdate_RTK2885pp(pParam, pOutput, iSliceIdx);
}

/* SIS MEMC function implement here */
VOID MEMC_MiddleWare_StatusInit_RTK2885pp(_OUTPUT_ME_SCENE_ANALYSIS *pOutput)
{
	pOutput->SIS_MEMC.Frame_Couter = 0;
}

VOID MEMC_MiddleWare_StatusUpdate_RTK2885pp(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput, int iSliceIdx)
{
	if (pOutput->SIS_MEMC.Frame_Couter > 65535)
		pOutput->SIS_MEMC.Frame_Couter = 0;
	else
		pOutput->SIS_MEMC.Frame_Couter++;
    
	if (iSliceIdx == 0)
	{
        MEMC_Motion_Info_Calc_RTK2885pp(pParam, pOutput, iSliceIdx);
        MEMC_VAR_LPF_Calc_RTK2885pp(pParam, pOutput, iSliceIdx);
        MEMC_Unstable_Score_Calc_RTK2885pp(pParam, pOutput, iSliceIdx, 1);
	}
	else if (iSliceIdx == 1)
	{

	}
	else if (iSliceIdx == 2)
	{
	}
	else if (iSliceIdx == 3)
	{
	}
	else if (iSliceIdx == 4)
	{
	}
	else
	{
		// don't use function put here
	}
    MEMC_Unstable_Score_Calc_RTK2885pp(pParam, pOutput, iSliceIdx, 0);
}

VOID MEMC_Motion_Info_Calc_RTK2885pp(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput, int iSliceIdx)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	static _Motion_Temporal_Info motion_temporal_info[5];
	int s32_med_val, global_score, max_global_score=-2147483648LL;
	unsigned char u8_med_idx, u8_i, u8_j, u8_min_rmv_idx, u8_max_rmv_idx;
	short s11_global_bgmvx, s10_global_bgmvy, s11_global_fgmvx = 0, s10_global_fgmvy = 0, s11_temp_global_fgmvx, s10_temp_global_fgmvy;
	short s11_med_rgn_bgmvx[32], s10_med_rgn_bgmvy[32], s11_rgn_bgmvx[32], s10_rgn_bgmvy[32], s11_rgn_fgmvx[32], s10_rgn_fgmvy[32];
	unsigned short u10_rgn_fg_mvdiff[32], u10_rgn_med_t_mvdiff[32], u10_rgn_fg_ucf[32], u10_rgn_bg_rmvdiff[32];
	
	unsigned short med_t_mvdiff, rmv_grp_mvdiff, rmv_mvdiff0 , rmv_mvdiff1, rmv_mvdiff2, gmv_grp0_mv, min_rmv_ucf;
	int gmv_mvdiff, gmv_grp01_mvdiff, rmv_mvdiff, mvdiff=0, med_mvdiff, gmv_max_mvy, gmv_max_mvx, gmv_max_mv;
	unsigned short non_zero_cnt_rmvdiff=0, l_non_zero_cnt_rmvdiff=0, r_non_zero_cnt_rmvdiff=0;
	unsigned char non_zero_cnt=0, l_non_zero_cnt=0, r_non_zero_cnt=0;
	unsigned short max_bg_mv_cnt=0, min_fg_mvdiff = 65535, cur_fg_mvdiff, global_fg_mvdiff=65535, global_bg_mvdiff;
	unsigned char med_grp_cnt=0, last_grp_cnt=0;
	bool u1_apl_upsample_en;
	unsigned short min_thr, mvdiff_thr, fg_ucf_cnt=0, fg_ucf_sum=0, fg_cnt=0, global_bgmv_score, min_global_bgmv_score=65535;
	short max_global_bgmvy=-16384, max_global_bgmvx=-16384, min_global_fgmvy=16383, min_global_fgmvx=16383;
	unsigned short t_gmv_mvdiff, u10_global_fgmv_cnt_mean=0, u10_global_fgmv_cnt_var=0;
	short f_max_gmv_mvy, f_max_gmv_mvx, f_min_gmv_mvy, f_min_gmv_mvx;
	short l_max_gmv_mvy, l_max_gmv_mvx, l_min_gmv_mvy, l_min_gmv_mvx;
	int bgmvy, bgmvx, med_bgmvy, med_bgmvx, final_bgmvy, final_bgmvx;
	unsigned short t_mvdiff, t_grp0_mvdiff, t_grp1_mvdiff, t_grp2_mvdiff, grp_mvdiff, bgmv_mvdiff, rgn_bgmv_mvdiff, statis_score, min_score=65535;
	unsigned char t_mvdiff_idx;
	bool same_cond, occl_cond;
	short s16_m, s16_n, s16_rgn_idx;
	unsigned short max_unstb_score = 0;
	bool fast_motion_cond;
	unsigned int print_in_func, print_result;
	if (pParam->u1_motion_info_calc_en == 0)
		return;
	
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);

	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 1, 1, &print_result);

	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	// med 5 global bgmv
	_Median5_(
		_ABS_(motion_temporal_info[0].s10_global_t_bgmvy) + _ABS_(motion_temporal_info[0].s11_global_t_bgmvx),
		_ABS_(motion_temporal_info[1].s10_global_t_bgmvy) + _ABS_(motion_temporal_info[1].s11_global_t_bgmvx),
		_ABS_(motion_temporal_info[2].s10_global_t_bgmvy) + _ABS_(motion_temporal_info[2].s11_global_t_bgmvx),
		_ABS_(motion_temporal_info[3].s10_global_t_bgmvy) + _ABS_(motion_temporal_info[3].s11_global_t_bgmvx),
		_ABS_(motion_temporal_info[4].s10_global_t_bgmvy) + _ABS_(motion_temporal_info[4].s11_global_t_bgmvx),
		&s32_med_val,
		&u8_med_idx);
	s10_global_bgmvy = motion_temporal_info[u8_med_idx].s10_global_t_bgmvy;
	s11_global_bgmvx = motion_temporal_info[u8_med_idx].s11_global_t_bgmvx;

	if ((s_pContext->_output_read_comreg.u32_me_sc_pattern_rb & 0x7f) > 0)
	{
		s10_global_bgmvy = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
		s11_global_bgmvx = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	}
	
	// med 5 rgn bgmv
	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		_Median5_(
			_ABS_(motion_temporal_info[0].s10_global_t_rgn_bgmvy[u8_i]) + _ABS_(motion_temporal_info[0].s11_global_t_rgn_bgmvx[u8_i]),
			_ABS_(motion_temporal_info[1].s10_global_t_rgn_bgmvy[u8_i]) + _ABS_(motion_temporal_info[1].s11_global_t_rgn_bgmvx[u8_i]),
			_ABS_(motion_temporal_info[2].s10_global_t_rgn_bgmvy[u8_i]) + _ABS_(motion_temporal_info[2].s11_global_t_rgn_bgmvx[u8_i]),
			_ABS_(motion_temporal_info[3].s10_global_t_rgn_bgmvy[u8_i]) + _ABS_(motion_temporal_info[3].s11_global_t_rgn_bgmvx[u8_i]),
			_ABS_(motion_temporal_info[4].s10_global_t_rgn_bgmvy[u8_i]) + _ABS_(motion_temporal_info[4].s11_global_t_rgn_bgmvx[u8_i]),
			&s32_med_val,
			&u8_med_idx);
		s10_med_rgn_bgmvy[u8_i] = motion_temporal_info[u8_med_idx].s10_global_t_rgn_bgmvy[u8_i];
		s11_med_rgn_bgmvx[u8_i] = motion_temporal_info[u8_med_idx].s11_global_t_rgn_bgmvx[u8_i];
	}


	// calc cur frame rgn bgmv
	gmv_grp0_mv = _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb) + _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb);
	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		s10_rgn_bgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me15_invld_mv_vy_rb[u8_i];
		s11_rgn_bgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me15_invld_mv_vx_rb[u8_i];
		med_t_mvdiff = _ABS_DIFF_(s10_med_rgn_bgmvy[u8_i], s10_rgn_bgmvy[u8_i]) +\
					   _ABS_DIFF_(s11_med_rgn_bgmvx[u8_i], s11_rgn_bgmvx[u8_i]);
		

		if (s_pContext->_output_read_comreg.u10_me15_invld_cnt_rb[u8_i] < 8)
		{
			s10_rgn_bgmvy[u8_i] = s10_global_bgmvy;
			s11_rgn_bgmvx[u8_i] = s11_global_bgmvx;
		}

		rmv_grp_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i]) +\
						 _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i]);

		rmv_mvdiff0 = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i], s10_rgn_bgmvy[u8_i]) +\
					  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i], s11_rgn_bgmvx[u8_i]);
		rmv_mvdiff1 = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i], s10_rgn_bgmvy[u8_i]) +\
					  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i], s11_rgn_bgmvx[u8_i]);
		rmv_mvdiff2 = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u8_i], s10_rgn_bgmvy[u8_i]) +\
					  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u8_i], s11_rgn_bgmvx[u8_i]);

		u8_min_rmv_idx = _IDX_MIN3_(rmv_mvdiff0, rmv_mvdiff1, rmv_mvdiff2);
		if (u8_min_rmv_idx == 0)
		{
			min_rmv_ucf = s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[u8_i];
		}
		else if (u8_min_rmv_idx == 1)
		{
			min_rmv_ucf = s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[u8_i];
		}
		else
		{
			min_rmv_ucf = s_pContext->_output_read_comreg.u10_me_rMV_3rd_unconf_rb[u8_i];
		}
		
		u10_rgn_bg_rmvdiff[u8_i] = _MIN_(_MIN_(rmv_mvdiff0, rmv_mvdiff1), rmv_mvdiff2);
		if ((u10_rgn_bg_rmvdiff[u8_i] < _CLIP_(rmv_grp_mvdiff >> 2, 0, 64)) && \
			(gmv_grp0_mv < 4))
		{
			s10_rgn_bgmvy[u8_i] = s10_global_bgmvy;
			s11_rgn_bgmvx[u8_i] = s11_global_bgmvx;
		}
		else if(min_rmv_ucf < 16)
		{
			if (u8_min_rmv_idx == 0)
			{
				s10_rgn_bgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i];
				s11_rgn_bgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i];
			}
			else if (u8_min_rmv_idx == 1)
			{
				s10_rgn_bgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i];
				s11_rgn_bgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i];
			}
			else
			{
				s10_rgn_bgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u8_i];
				s11_rgn_bgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u8_i];
			}
		}
		else
		{
			rmv_mvdiff0 = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb, s10_rgn_bgmvy[u8_i]) +\
						  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb, s11_rgn_bgmvx[u8_i]);
			rmv_mvdiff1 = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb, s10_rgn_bgmvy[u8_i]) +\
						  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb, s11_rgn_bgmvx[u8_i]);
			rmv_mvdiff2 = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb, s10_rgn_bgmvy[u8_i]) +\
						  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb, s11_rgn_bgmvx[u8_i]);
			u8_min_rmv_idx = _IDX_MIN3_(rmv_mvdiff0, rmv_mvdiff1, rmv_mvdiff2);
			if (u8_min_rmv_idx == 0)
			{
				s10_rgn_bgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
				s11_rgn_bgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
			}
			else if (u8_min_rmv_idx == 1)
			{
				s10_rgn_bgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb;
				s11_rgn_bgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb;
			}
			else
			{
				s10_rgn_bgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb;
				s11_rgn_bgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb;
			}
		}

		// calc rgn info
		rmv_mvdiff0 = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i], s10_rgn_bgmvy[u8_i]) +\
					  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i], s11_rgn_bgmvx[u8_i]);
		rmv_mvdiff1 = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i], s10_rgn_bgmvy[u8_i]) +\
					  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i], s11_rgn_bgmvx[u8_i]);
		rmv_mvdiff2 = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u8_i], s10_rgn_bgmvy[u8_i]) +\
					  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u8_i], s11_rgn_bgmvx[u8_i]);
		
		u8_max_rmv_idx = _IDX_MAX3_(rmv_mvdiff0, rmv_mvdiff1, rmv_mvdiff2);
		u10_rgn_med_t_mvdiff[u8_i] = med_t_mvdiff;
		if (u8_max_rmv_idx == 0)
		{
			s10_rgn_fgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i];
			s11_rgn_fgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i];
			u10_rgn_fg_mvdiff[u8_i] = rmv_mvdiff0;
			u10_rgn_fg_ucf[u8_i] = s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[u8_i];
		}
		else if (u8_max_rmv_idx == 1)
		{
			s10_rgn_fgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i];
			s11_rgn_fgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i];
			u10_rgn_fg_mvdiff[u8_i] = rmv_mvdiff1;
			u10_rgn_fg_ucf[u8_i] = s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[u8_i];
		}
		else
		{
			s10_rgn_fgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u8_i];
			s11_rgn_fgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u8_i];
			u10_rgn_fg_mvdiff[u8_i] = rmv_mvdiff2;
			u10_rgn_fg_ucf[u8_i] = s_pContext->_output_read_comreg.u10_me_rMV_3rd_unconf_rb[u8_i];
		}
		
		
	}


	// calc mvdiff
	gmv_max_mvy = _MAX_(_MAX_(_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb), _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb)), _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb));
	gmv_max_mvx = _MAX_(_MAX_(_ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb), _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb)), _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb));
	gmv_max_mv =  _MAX_(_MAX_(_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb) + _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb),
							   _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb) + _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb)), 
						_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb) + _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb));
	gmv_mvdiff =  gmv_max_mvy - \
				  _MIN_(_MIN_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb, s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb), s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb) + \
				  gmv_max_mvx - \
				  _MIN_(_MIN_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb, s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb), s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb);
	gmv_grp01_mvdiff =  _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb, s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb) + \
						_ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb, s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb);
	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		rmv_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i]) + \
					 _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i]);

		// center rmvdiff
		if (((u8_i % 8) != 0) && ((u8_i % 8) != 7) && (u8_i > 7) && (u8_i < 24))
		{
			mvdiff = _MAX_(mvdiff, rmv_mvdiff);

			if (s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[u8_i] > 10)
			{
				non_zero_cnt++;
				non_zero_cnt_rmvdiff += rmv_mvdiff;
				if ((u8_i % 8) < 4)
				{
					l_non_zero_cnt++;
					l_non_zero_cnt_rmvdiff+= rmv_mvdiff;
				}
				else
				{
					r_non_zero_cnt++;
					r_non_zero_cnt_rmvdiff += rmv_mvdiff;
				}
			}
		pOutput->u10_rmv_mvdiff[u8_i] = rmv_mvdiff;
		}
	}
	non_zero_cnt_rmvdiff = non_zero_cnt_rmvdiff / _CLIP_(non_zero_cnt, 1, 31);
	l_non_zero_cnt_rmvdiff = l_non_zero_cnt_rmvdiff / _CLIP_(l_non_zero_cnt, 1, 31);
	r_non_zero_cnt_rmvdiff = r_non_zero_cnt_rmvdiff / _CLIP_(r_non_zero_cnt, 1, 31);
	_Median3_(non_zero_cnt_rmvdiff, l_non_zero_cnt_rmvdiff, r_non_zero_cnt_rmvdiff, &med_mvdiff, &u8_med_idx);
	for (u8_i = 0; u8_i < 4; u8_i++)
	{
		motion_temporal_info[u8_i].u10_med_mvdiff_t = motion_temporal_info[u8_i + 1].u10_med_mvdiff_t;
	}
	motion_temporal_info[4].u10_med_mvdiff_t= med_mvdiff;
	_Median5_(
		motion_temporal_info[0].u10_med_mvdiff_t,
		motion_temporal_info[1].u10_med_mvdiff_t,
		motion_temporal_info[2].u10_med_mvdiff_t,
		motion_temporal_info[3].u10_med_mvdiff_t,
		motion_temporal_info[4].u10_med_mvdiff_t,
		&med_mvdiff, &u8_med_idx);

	mvdiff = _MAX_(mvdiff, med_mvdiff);
	
	// calc global bgmv fgmv
	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		if (((u8_i % 8) != 0) && ((u8_i % 8) != 7) && (u8_i > 7) && (u8_i < 24) && (s_pContext->_output_read_comreg.u10_me15_invld_cnt_rb > 0))
		{
			global_score = s_pContext->_output_read_comreg.u10_me15_invld_cnt_rb[u8_i] * _MAX_((mvdiff - 128) >> 5, 2) - \
						   _ABS_(u10_rgn_fg_mvdiff[u8_i] - mvdiff) * _MAX_((128 - _MAX_(mvdiff - 128, 0)) >> 7, 1) - \
						   (u10_rgn_bg_rmvdiff[u8_i] << 3) - \
						   (_ABS_DIFF_(s10_rgn_bgmvy[u8_i], s10_global_bgmvy) + _ABS_DIFF_(s11_rgn_bgmvx[u8_i], s11_global_bgmvx)) - \
						   u10_rgn_med_t_mvdiff[u8_i];
			if (gmv_max_mv > 200)
			{
				for (s16_m = -1; s16_m < 2; s16_m++)
				{
					for (s16_n = -1; s16_n < 2; s16_n++)
					{
						s16_rgn_idx = _CLIP_((short)u8_i + s16_m * 8 + s16_n, 0, 31);
						max_unstb_score = _MAX_(max_unstb_score, pre_unstable_score[s16_rgn_idx]);
					}
				}

				global_score = global_score - max_unstb_score * 4;
			}
			if (global_score > max_global_score)
			{
				max_global_score = global_score;
				s10_global_bgmvy = s10_rgn_bgmvy[u8_i];
				s11_global_bgmvx = s11_rgn_bgmvx[u8_i];
			}

			max_bg_mv_cnt = _MAX_(max_bg_mv_cnt, s_pContext->_output_read_comreg.u10_me15_invld_cnt_rb[u8_i]);
		}
	}
	
	if (((s_pContext->_output_read_comreg.u32_me_sc_pattern_rb & 0xf) > 0) || (max_bg_mv_cnt < 16) || (s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb > 25000))
	{
		s10_global_bgmvy = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
		s11_global_bgmvx = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	}

	max_global_score = -2147483648LL;
	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		if (((u8_i % 8) != 0) && ((u8_i % 8) != 7) && (u8_i > 7) && (u8_i < 24))
		{
			global_score = s_pContext->_output_read_comreg.u10_me15_invld_cnt_rb[u8_i] * _MAX_((mvdiff - 128) >> 5, 2) + \
						   (_ABS_DIFF_(s10_rgn_fgmvy[u8_i], s10_global_bgmvy) + _ABS_DIFF_(s11_rgn_fgmvx[u8_i], s11_global_bgmvx)) - \
						   (u10_rgn_fg_ucf[u8_i] << 3);
						   
			if (global_score > max_global_score)
			{
				max_global_score = global_score;
				s10_global_fgmvy = s10_rgn_fgmvy[u8_i];
				s11_global_fgmvx = s11_rgn_fgmvx[u8_i];
			}
		}
	}
	if (print_result)
	{
		rtd_pr_memc_emerg("//********** [MEMC DH cond 2][bgmvy: %d][bgmvx: %d][fgmvy: %d][fgmvx: %d] **********//\n", s10_global_bgmvy, s11_global_bgmvx,s10_global_fgmvy,s11_global_fgmvx);
	}
	
	for (u8_i = 0; u8_i < 4; u8_i++)
	{
		motion_temporal_info[u8_i].s10_global_t_bgmvy = motion_temporal_info[u8_i + 1].s10_global_t_bgmvy;
		motion_temporal_info[u8_i].s11_global_t_bgmvx = motion_temporal_info[u8_i + 1].s11_global_t_bgmvx;
		motion_temporal_info[u8_i].s10_global_t_fgmvy = motion_temporal_info[u8_i + 1].s10_global_t_fgmvy;
		motion_temporal_info[u8_i].s11_global_t_fgmvx = motion_temporal_info[u8_i + 1].s11_global_t_fgmvx;
		for (u8_j = 0; u8_j < 32; u8_j++)
		{
			motion_temporal_info[u8_i].s10_global_t_rgn_bgmvy[u8_j] = motion_temporal_info[u8_i + 1].s10_global_t_rgn_bgmvy[u8_j];
			motion_temporal_info[u8_i].s11_global_t_rgn_bgmvx[u8_j] = motion_temporal_info[u8_i + 1].s11_global_t_rgn_bgmvx[u8_j];
		}

	}
	motion_temporal_info[4].s10_global_t_bgmvy = s10_global_bgmvy;
	motion_temporal_info[4].s11_global_t_bgmvx = s11_global_bgmvx;
	motion_temporal_info[4].s10_global_t_fgmvy = s10_global_fgmvy;
	motion_temporal_info[4].s11_global_t_fgmvx = s11_global_fgmvx;
	if (print_result)
	{
		rtd_pr_memc_emerg("//********** [MEMC DH cond 2-2][bgmvy: %d %d %d %d %d] **********//\n", 
			motion_temporal_info[0].s10_global_t_bgmvy,
			motion_temporal_info[1].s10_global_t_bgmvy,
			motion_temporal_info[2].s10_global_t_bgmvy,
			motion_temporal_info[3].s10_global_t_bgmvy,
			motion_temporal_info[4].s10_global_t_bgmvy);
		rtd_pr_memc_emerg("//********** [MEMC DH cond 2-2][bgmvx: %d %d %d %d %d] **********//\n", 
			motion_temporal_info[0].s11_global_t_bgmvx,
			motion_temporal_info[1].s11_global_t_bgmvx,
			motion_temporal_info[2].s11_global_t_bgmvx,
			motion_temporal_info[3].s11_global_t_bgmvx,
			motion_temporal_info[4].s11_global_t_bgmvx);
		rtd_pr_memc_emerg("//********** [MEMC DH cond 2-2][fgmvy: %d %d %d %d %d] **********//\n",
			motion_temporal_info[0].s10_global_t_fgmvy,
			motion_temporal_info[1].s10_global_t_fgmvy,
			motion_temporal_info[2].s10_global_t_fgmvy,
			motion_temporal_info[3].s10_global_t_fgmvy,
			motion_temporal_info[4].s10_global_t_fgmvy);
		rtd_pr_memc_emerg("//********** [MEMC DH cond 2-2][fgmvx: %d %d %d %d %d] **********//\n",
			motion_temporal_info[0].s11_global_t_fgmvx,
			motion_temporal_info[1].s11_global_t_fgmvx,
			motion_temporal_info[2].s11_global_t_fgmvx,
			motion_temporal_info[3].s11_global_t_fgmvx,
			motion_temporal_info[4].s11_global_t_fgmvx);
	}
	for (u8_j = 0; u8_j < 32; u8_j++)
	{
		motion_temporal_info[4].s10_global_t_rgn_bgmvy[u8_j] = s10_rgn_bgmvy[u8_i];
		motion_temporal_info[4].s11_global_t_rgn_bgmvx[u8_j] = s11_rgn_bgmvx[u8_i];
	}
	_Median5_(
		motion_temporal_info[0].s10_global_t_bgmvy + motion_temporal_info[0].s11_global_t_bgmvx,
		motion_temporal_info[1].s10_global_t_bgmvy + motion_temporal_info[1].s11_global_t_bgmvx,
		motion_temporal_info[2].s10_global_t_bgmvy + motion_temporal_info[2].s11_global_t_bgmvx,
		motion_temporal_info[3].s10_global_t_bgmvy + motion_temporal_info[3].s11_global_t_bgmvx,
		motion_temporal_info[4].s10_global_t_bgmvy + motion_temporal_info[4].s11_global_t_bgmvx,
		&s32_med_val, &u8_med_idx);
	s10_global_bgmvy = motion_temporal_info[u8_med_idx].s10_global_t_bgmvy;
	s11_global_bgmvx = motion_temporal_info[u8_med_idx].s11_global_t_bgmvx;
	
	_Median5_(
		motion_temporal_info[0].s10_global_t_fgmvy + motion_temporal_info[0].s11_global_t_fgmvx,
		motion_temporal_info[1].s10_global_t_fgmvy + motion_temporal_info[1].s11_global_t_fgmvx,
		motion_temporal_info[2].s10_global_t_fgmvy + motion_temporal_info[2].s11_global_t_fgmvx,
		motion_temporal_info[3].s10_global_t_fgmvy + motion_temporal_info[3].s11_global_t_fgmvx,
		motion_temporal_info[4].s10_global_t_fgmvy + motion_temporal_info[4].s11_global_t_fgmvx,
		&s32_med_val, &u8_med_idx);
	s10_global_fgmvy = motion_temporal_info[u8_med_idx].s10_global_t_fgmvy;
	s11_global_fgmvx = motion_temporal_info[u8_med_idx].s11_global_t_fgmvx;

	for (u8_i = 0; u8_i < 32; u8_i++)
	{

		if ((s_pContext->_output_read_comreg.u10_me15_invld_cnt_rb[u8_i] > 24) && (u10_rgn_bg_rmvdiff[u8_i] < 32))
		{
			if ((_ABS_DIFF_(s10_rgn_bgmvy[u8_i], s10_global_bgmvy) + _ABS_DIFF_(s11_rgn_bgmvx[u8_i], s11_global_bgmvx)) < \
				 (_ABS_DIFF_(s10_rgn_bgmvy[u8_i], motion_temporal_info[4].s10_global_t_bgmvy) + _ABS_DIFF_(s11_rgn_bgmvx[u8_i], motion_temporal_info[4].s11_global_t_bgmvx)))
			{
				med_grp_cnt++;
			}
			else
			{
				last_grp_cnt++;
			}
		}
	}
	if (mvdiff && ((last_grp_cnt >> 1) > med_grp_cnt))
	{
		s10_global_bgmvy = motion_temporal_info[4].s10_global_t_bgmvy;
		s11_global_bgmvx = motion_temporal_info[4].s11_global_t_bgmvx;
	}

	
	if ((s_pContext->_output_read_comreg.u32_me_sc_pattern_rb & 0xf) > 0)
	{
		s10_global_bgmvy = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
		s11_global_bgmvx = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	}
	if (print_result)
	{
		rtd_pr_memc_emerg("//********** [MEMC DH cond 3][bgmvy: %d][bgmvx: %d][fgmvy: %d][fgmvx: %d] **********//\n", s10_global_bgmvy, s11_global_bgmvx,s10_global_fgmvy,s11_global_fgmvx);
	}

	s10_temp_global_fgmvy = s10_global_fgmvy;
	s11_temp_global_fgmvx = s11_global_fgmvx;
	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		cur_fg_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i], s10_global_fgmvy) + \
						_ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i], s11_global_fgmvx);
		if (cur_fg_mvdiff < min_fg_mvdiff)
		{
			min_fg_mvdiff = cur_fg_mvdiff;
			s10_temp_global_fgmvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i];
			s11_temp_global_fgmvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i];
		}

		cur_fg_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i], s10_global_fgmvy) + \
						_ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i], s11_global_fgmvx);
		if (cur_fg_mvdiff < min_fg_mvdiff)
		{
			min_fg_mvdiff = cur_fg_mvdiff;
			s10_temp_global_fgmvy = s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i];
			s11_temp_global_fgmvx = s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i];
		}
		
	}
	s10_global_fgmvy = s10_temp_global_fgmvy;
	s11_global_fgmvx = s11_temp_global_fgmvx;
	
	
	for (u8_j = 0; u8_j < 5; u8_j++)
	{
		min_global_fgmvy = _MIN_(motion_temporal_info[u8_j].s10_global_t_fgmvy, min_global_fgmvy);
		min_global_fgmvx = _MIN_(motion_temporal_info[u8_j].s11_global_t_fgmvx, min_global_fgmvx);
		max_global_bgmvy = _MAX_(motion_temporal_info[u8_j].s10_global_t_bgmvy, max_global_bgmvy);
		max_global_bgmvx = _MAX_(motion_temporal_info[u8_j].s11_global_t_bgmvx, max_global_bgmvx);
	}
	min_thr = _ABS_DIFF_(min_global_fgmvy, max_global_bgmvy) + _ABS_DIFF_(min_global_fgmvx, max_global_bgmvx);
	min_thr = (min_thr * 3) >> 1;
	mvdiff_thr = _CLIP_((med_mvdiff + mvdiff) >> 3, min_thr, 128);
	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		if (((u8_i % 8) != 0) && ((u8_i % 8) != 7) && (u8_i > 7) && (u8_i < 24))
		{
			// 1st
			global_fg_mvdiff = 65535;
			for (u8_j = 0; u8_j < 5; u8_j++)
			{
				global_fg_mvdiff = _MIN_(global_fg_mvdiff, 
										 _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i], motion_temporal_info[u8_j].s10_global_t_fgmvy) + \
										 _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i], motion_temporal_info[u8_j].s11_global_t_fgmvx));
			}

			global_bg_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i], s10_global_bgmvy) + \
							   _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i], s11_global_bgmvx);
			
			if ((global_fg_mvdiff < mvdiff_thr) && (global_bg_mvdiff > global_fg_mvdiff) && (s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[u8_i] > 0))
			{
				fg_ucf_cnt++;
				fg_ucf_sum += s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[u8_i];
				fg_cnt += s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[u8_i];
			}

			// 2nd
			global_fg_mvdiff = 65535;
			for (u8_j = 0; u8_j < 5; u8_j++)
			{
				global_fg_mvdiff = _MIN_(global_fg_mvdiff, 
										 _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i], motion_temporal_info[u8_j].s10_global_t_fgmvy) + \
										 _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i], motion_temporal_info[u8_j].s11_global_t_fgmvx));
			}

			global_bg_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i], s10_global_bgmvy) + \
							   _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i], s11_global_bgmvx);
			
			if ((global_fg_mvdiff < mvdiff_thr) && (global_bg_mvdiff > global_fg_mvdiff) && (s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[u8_i] > 0))
			{
				fg_ucf_cnt++;
				fg_ucf_sum += s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[u8_i];
				fg_cnt += s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[u8_i];
			}

			// 3rd
			global_fg_mvdiff = 65535;
			for (u8_j = 0; u8_j < 5; u8_j++)
			{
				global_fg_mvdiff = _MIN_(global_fg_mvdiff, 
										 _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u8_i], motion_temporal_info[u8_j].s10_global_t_fgmvy) + \
										 _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u8_i], motion_temporal_info[u8_j].s11_global_t_fgmvx));
			}

			global_bg_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u8_i], s10_global_bgmvy) + \
							   _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u8_i], s11_global_bgmvx);
			
			if ((global_fg_mvdiff < mvdiff_thr) && (global_bg_mvdiff > global_fg_mvdiff) && (s_pContext->_output_read_comreg.u10_me_rMV_3rd_cnt_rb[u8_i] > 0))
			{
				fg_ucf_cnt++;
				fg_ucf_sum += s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[u8_i];
				fg_cnt += s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[u8_i];
			}
		}
	}
	
	for (u8_i = 0; u8_i < 4; u8_i++)
	{
		motion_temporal_info[u8_i].u10_global_t_fgmv_cnt = motion_temporal_info[u8_i + 1].u10_global_t_fgmv_cnt;
		motion_temporal_info[u8_i].u10_global_t_fgmv_ucf = motion_temporal_info[u8_i + 1].u10_global_t_fgmv_ucf;
		
	}
	motion_temporal_info[4].u10_global_t_fgmv_cnt = fg_cnt;
	motion_temporal_info[4].u10_global_t_fgmv_ucf = fg_ucf_sum / _MAX_(fg_ucf_cnt, 1);

	// 1st
	global_bgmv_score =  (_ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb , s10_global_bgmvy) + \
						  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb , s11_global_bgmvx))- \
						 (s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb >> 10)+ \
						 (s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb >> 1);
	if (global_bgmv_score < min_global_bgmv_score)
	{
		min_global_bgmv_score = global_bgmv_score;
		s10_global_bgmvy = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
		s11_global_bgmvx = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	}

	// 2nd
	global_bgmv_score =  (_ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb , s10_global_bgmvy) + \
						  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb , s11_global_bgmvx))- \
						 (s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb >> 10)+ \
						 (s_pContext->_output_read_comreg.u12_me_GMV_2nd_unconf_rb >> 1);
	if (global_bgmv_score < min_global_bgmv_score)
	{
		min_global_bgmv_score = global_bgmv_score;
		s10_global_bgmvy = s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb;
		s11_global_bgmvx = s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb;
	}

	// 3rd
	global_bgmv_score =  (_ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb , s10_global_bgmvy) + \
						  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb , s11_global_bgmvx))- \
						 (s_pContext->_output_read_comreg.u16_me_GMV_3rd_cnt_rb >> 10)+ \
						 (s_pContext->_output_read_comreg.u10_me_GMV_3rd_unconf_rb >> 1);
	if (global_bgmv_score < min_global_bgmv_score)
	{
		min_global_bgmv_score = global_bgmv_score;
		s10_global_bgmvy = s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb;
		s11_global_bgmvx = s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb;
	}

	gmv_grp01_mvdiff = _MAX_(_ABS_DIFF_(s10_global_fgmvy, s10_global_bgmvy) + _ABS_DIFF_(s11_global_fgmvx , s11_global_bgmvx), gmv_grp01_mvdiff);
	med_mvdiff = _MIN_(med_mvdiff, gmv_grp01_mvdiff);

	// calc mv change
	for (u8_i = 0; u8_i < 4; u8_i++)
	{
		motion_temporal_info[u8_i].s10_t_gmv_mvy[0] = motion_temporal_info[u8_i + 1].s10_t_gmv_mvy[0];
		motion_temporal_info[u8_i].s11_t_gmv_mvx[0] = motion_temporal_info[u8_i + 1].s11_t_gmv_mvx[0];
		motion_temporal_info[u8_i].s10_t_gmv_mvy[1] = motion_temporal_info[u8_i + 1].s10_t_gmv_mvy[1];
		motion_temporal_info[u8_i].s11_t_gmv_mvx[1] = motion_temporal_info[u8_i + 1].s11_t_gmv_mvx[1];
		motion_temporal_info[u8_i].s10_t_gmv_mvy[2] = motion_temporal_info[u8_i + 1].s10_t_gmv_mvy[2];
		motion_temporal_info[u8_i].s11_t_gmv_mvx[2] = motion_temporal_info[u8_i + 1].s11_t_gmv_mvx[2];
	}
	motion_temporal_info[4].s10_t_gmv_mvy[0] = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
	motion_temporal_info[4].s11_t_gmv_mvx[0] = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	motion_temporal_info[4].s10_t_gmv_mvy[1] = s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb;
	motion_temporal_info[4].s11_t_gmv_mvx[1] = s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb;
	motion_temporal_info[4].s10_t_gmv_mvy[2] = s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb;
	motion_temporal_info[4].s11_t_gmv_mvx[2] = s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb;

	

	f_max_gmv_mvy = _MAX_(_MAX_(motion_temporal_info[0].s10_t_gmv_mvy[0], motion_temporal_info[0].s10_t_gmv_mvy[1]), motion_temporal_info[0].s10_t_gmv_mvy[2]);
	f_max_gmv_mvx = _MAX_(_MAX_(motion_temporal_info[0].s11_t_gmv_mvx[0], motion_temporal_info[0].s11_t_gmv_mvx[1]), motion_temporal_info[0].s11_t_gmv_mvx[2]);
	f_min_gmv_mvy = _MIN_(_MIN_(motion_temporal_info[0].s10_t_gmv_mvy[0], motion_temporal_info[0].s10_t_gmv_mvy[1]), motion_temporal_info[0].s10_t_gmv_mvy[2]);
	f_min_gmv_mvx = _MIN_(_MIN_(motion_temporal_info[0].s11_t_gmv_mvx[0], motion_temporal_info[0].s11_t_gmv_mvx[1]), motion_temporal_info[0].s11_t_gmv_mvx[2]);

	l_max_gmv_mvy = _MAX_(_MAX_(motion_temporal_info[4].s10_t_gmv_mvy[0], motion_temporal_info[4].s10_t_gmv_mvy[1]), motion_temporal_info[4].s10_t_gmv_mvy[2]);
	l_max_gmv_mvx = _MAX_(_MAX_(motion_temporal_info[4].s11_t_gmv_mvx[0], motion_temporal_info[4].s11_t_gmv_mvx[1]), motion_temporal_info[4].s11_t_gmv_mvx[2]);
	l_min_gmv_mvy = _MIN_(_MIN_(motion_temporal_info[4].s10_t_gmv_mvy[0], motion_temporal_info[4].s10_t_gmv_mvy[1]), motion_temporal_info[4].s10_t_gmv_mvy[2]);
	l_min_gmv_mvx = _MIN_(_MIN_(motion_temporal_info[4].s11_t_gmv_mvx[0], motion_temporal_info[4].s11_t_gmv_mvx[1]), motion_temporal_info[4].s11_t_gmv_mvx[2]);
	

	t_gmv_mvdiff = _MAX_(
		_ABS_DIFF_(f_max_gmv_mvy, l_max_gmv_mvy) + _ABS_DIFF_(f_max_gmv_mvx, l_max_gmv_mvx),
		_ABS_DIFF_(f_min_gmv_mvy, l_min_gmv_mvy) + _ABS_DIFF_(f_min_gmv_mvx, l_min_gmv_mvx));

	// apl upsample cond
	u1_apl_upsample_en = (mvdiff < 80) && \
						 (_ABS_(s10_global_fgmvy) < 16) && \
						 ((_ABS_(s11_global_fgmvx) > 16) || s_pContext->_output_read_comreg.u26_me_glb_dtl_rb > 1500000);


	// calc output rgn bgmv
	
	bgmvy = (_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb) < 140) ? s10_global_bgmvy : s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
	bgmvx = (_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb) < 140) ? s11_global_bgmvx : s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;

	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		for (u8_j = 0; u8_j < 4; u8_j++)
		{
			motion_temporal_info[u8_j].s10_me_t_rgn_bgmvy[u8_i] = motion_temporal_info[u8_j + 1].s10_me_t_rgn_bgmvy[u8_i];
			motion_temporal_info[u8_j].s11_me_t_rgn_bgmvx[u8_i] = motion_temporal_info[u8_j + 1].s11_me_t_rgn_bgmvx[u8_i];
		}

		if (s_pContext->_output_read_comreg.u10_me15_invld_cnt_rb[u8_i] > 20)
		{
			t_grp0_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i], s_pContext->_output_read_comreg.s10_me15_invld_mv_vy_rb[u8_i]) + \
							_ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i], s_pContext->_output_read_comreg.s11_me15_invld_mv_vx_rb[u8_i]);
			t_grp1_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i], s_pContext->_output_read_comreg.s10_me15_invld_mv_vy_rb[u8_i]) + \
							_ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i], s_pContext->_output_read_comreg.s11_me15_invld_mv_vx_rb[u8_i]);
			t_grp2_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u8_i], s_pContext->_output_read_comreg.s10_me15_invld_mv_vy_rb[u8_i]) + \
							_ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u8_i], s_pContext->_output_read_comreg.s11_me15_invld_mv_vx_rb[u8_i]);

			if ((t_grp1_mvdiff < t_grp0_mvdiff) && (t_grp1_mvdiff < t_grp2_mvdiff))
			{
				t_mvdiff = t_grp1_mvdiff;
				t_mvdiff_idx = 1;
				rgn_bgmv_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i], s10_rgn_bgmvy[u8_i]) + \
								  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i], s11_rgn_bgmvx[u8_i]);
			}
			else if ((t_grp2_mvdiff < t_grp0_mvdiff) && (t_grp2_mvdiff < t_grp1_mvdiff))
			{
				t_mvdiff = t_grp2_mvdiff;
				t_mvdiff_idx = 2;
				rgn_bgmv_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u8_i], s10_rgn_bgmvy[u8_i]) + \
								  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u8_i], s11_rgn_bgmvx[u8_i]);
			}
			else
			{
				t_mvdiff = t_grp0_mvdiff;
				t_mvdiff_idx = 0;
				rgn_bgmv_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i], s10_rgn_bgmvy[u8_i]) + \
								  _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i], s11_rgn_bgmvx[u8_i]);
			}
			
			grp_mvdiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i]) + \
						 _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i]);

			bgmv_mvdiff = _ABS_DIFF_(bgmvy, s_pContext->_output_read_comreg.s10_me15_invld_mv_vy_rb[u8_i]) + \
						  _ABS_DIFF_(bgmvx, s_pContext->_output_read_comreg.s11_me15_invld_mv_vx_rb[u8_i]);
			same_cond = (mvdiff > 48) && \
						(rgn_bgmv_mvdiff < _MAX_(grp_mvdiff >> 1, 16));
			occl_cond = _MIN_(s_pContext->_output_read_comreg.u12_dh_cov_cnt_rb[u8_i], s_pContext->_output_read_comreg.u12_dh_ucov_cnt_rb[u8_i]) < 10;

			if ((t_mvdiff < 32) && ((grp_mvdiff > 64) || (bgmv_mvdiff > _CLIP_(med_mvdiff >> 2, 16, 32))) && occl_cond && same_cond)
			{
				if (t_mvdiff_idx == 0)
				{
					motion_temporal_info[4].s10_me_t_rgn_bgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i];
					motion_temporal_info[4].s11_me_t_rgn_bgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i];
				}
				else if (t_mvdiff_idx == 1)
				{
					motion_temporal_info[4].s10_me_t_rgn_bgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i];
					motion_temporal_info[4].s11_me_t_rgn_bgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i];
				}
				else
				{
					motion_temporal_info[4].s10_me_t_rgn_bgmvy[u8_i] = s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u8_i];
					motion_temporal_info[4].s11_me_t_rgn_bgmvx[u8_i] = s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u8_i];
				}
			}
			else
			{
				motion_temporal_info[4].s10_me_t_rgn_bgmvy[u8_i] = bgmvy;
				motion_temporal_info[4].s11_me_t_rgn_bgmvx[u8_i] = bgmvx;
			}
		}
		else
		{
			motion_temporal_info[4].s10_me_t_rgn_bgmvy[u8_i] = bgmvy;
			motion_temporal_info[4].s11_me_t_rgn_bgmvx[u8_i] = bgmvx;
		}

		_Median5_(
			motion_temporal_info[0].s10_me_t_rgn_bgmvy[u8_i],
			motion_temporal_info[1].s10_me_t_rgn_bgmvy[u8_i],
			motion_temporal_info[2].s10_me_t_rgn_bgmvy[u8_i],
			motion_temporal_info[3].s10_me_t_rgn_bgmvy[u8_i],
			motion_temporal_info[4].s10_me_t_rgn_bgmvy[u8_i],
			&med_bgmvy,
			&u8_med_idx);
		_Median5_(
			motion_temporal_info[0].s11_me_t_rgn_bgmvx[u8_i],
			motion_temporal_info[1].s11_me_t_rgn_bgmvx[u8_i],
			motion_temporal_info[2].s11_me_t_rgn_bgmvx[u8_i],
			motion_temporal_info[3].s11_me_t_rgn_bgmvx[u8_i],
			motion_temporal_info[4].s11_me_t_rgn_bgmvx[u8_i],
			&med_bgmvx,
			&u8_med_idx);

		// rmv grp0
		statis_score = s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[u8_i];
		statis_score = (statis_score > 32) ? (statis_score + 32) : (statis_score << 1);
		statis_score += _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i], med_bgmvy) + \
						_ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i], med_bgmvx);
		if (statis_score < min_score)
		{
			min_score = statis_score;
			final_bgmvy = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i];
			final_bgmvx = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i];
		}

		// rmv grp1
		statis_score = s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[u8_i];
		statis_score = (statis_score > 32) ? (statis_score + 32) : (statis_score << 1);
		statis_score += _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i], med_bgmvy) + \
						_ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i], med_bgmvx);
		if (statis_score < min_score)
		{
			min_score = statis_score;
			final_bgmvy = s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i];
			final_bgmvx = s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i];
		}

		// rmv grp2
		statis_score = s_pContext->_output_read_comreg.u10_me_rMV_3rd_unconf_rb[u8_i];
		statis_score = (statis_score > 32) ? (statis_score + 32) : (statis_score << 1);
		statis_score += _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u8_i], med_bgmvy) + \
						_ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u8_i], med_bgmvx);
		if (statis_score < min_score)
		{
			min_score = statis_score;
			final_bgmvy = s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u8_i];
			final_bgmvx = s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u8_i];
		}

		// gmv grp0
		statis_score = s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb;
		statis_score = (statis_score > 32) ? (statis_score + 32) : (statis_score << 1);
		statis_score += _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb, med_bgmvy) + \
						_ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb, med_bgmvx);
		if (statis_score < min_score)
		{
			min_score = statis_score;
			final_bgmvy = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
			final_bgmvx = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
		}

		// gmv grp1
		statis_score = s_pContext->_output_read_comreg.u12_me_GMV_2nd_unconf_rb;
		statis_score = (statis_score > 32) ? (statis_score + 32) : (statis_score << 1);
		statis_score += _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb, med_bgmvy) + \
						_ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb, med_bgmvx);
		if (statis_score < min_score)
		{
			min_score = statis_score;
			final_bgmvy = s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb;
			final_bgmvx = s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb;
		}

		// gmv grp2
		statis_score = s_pContext->_output_read_comreg.u10_me_GMV_3rd_unconf_rb;
		statis_score = (statis_score > 32) ? (statis_score + 32) : (statis_score << 1);
		statis_score += _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb, med_bgmvy) + \
						_ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb, med_bgmvx);
		if (statis_score < min_score)
		{
			min_score = statis_score;
			final_bgmvy = s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb;
			final_bgmvx = s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb;
		}

		if (((s_pContext->_output_read_comreg.u32_me_sc_pattern_rb & 0x7f) > 0) || \
			(_MIN_(s_pContext->_output_read_comreg.u12_dh_cov_cnt_rb[u8_i], s_pContext->_output_read_comreg.u12_dh_ucov_cnt_rb[u8_i]) > 32))
		{
			final_bgmvy = s10_global_bgmvy;
			final_bgmvx = s11_global_bgmvx;
		}
		
		pOutput->s10_rgn_bgmvy[u8_i] = final_bgmvy;
		pOutput->s11_rgn_bgmvx[u8_i] = final_bgmvx;
	}

	
	for (u8_j = 0; u8_j < 4; u8_j++)
	{
		u10_global_fgmv_cnt_mean += motion_temporal_info[u8_j].u10_global_t_fgmv_cnt;
	}
	u10_global_fgmv_cnt_mean /= 5;
	for (u8_j = 0; u8_j < 4; u8_j++)
	{
		u10_global_fgmv_cnt_var += _ABS_DIFF_(motion_temporal_info[u8_j].u10_global_t_fgmv_cnt, u10_global_fgmv_cnt_mean);
	}
	fast_motion_cond = (gmv_max_mvy > 150) || \
					   (gmv_max_mvx > 200) || \
					   (gmv_max_mv > 300) || \
					   (gmv_max_mvy > (gmv_max_mvx * 5));
	
	// output
	pOutput->u10_mvdiff = mvdiff;
	pOutput->s10_global_bgmvy = s10_global_bgmvy;
	pOutput->s11_global_bgmvx = s11_global_bgmvx;
	pOutput->s10_global_fgmvy = s10_global_fgmvy;
	pOutput->s11_global_fgmvx = s11_global_fgmvx;
	pOutput->u10_global_fgmv_cnt_mean = u10_global_fgmv_cnt_mean;
	pOutput->u10_global_fgmv_cnt_var = u10_global_fgmv_cnt_var;
	pOutput->u10_gmv_grp01_mvdiff = gmv_grp01_mvdiff;
	pOutput->u16_gmv_max_mv = gmv_max_mv;
	pOutput->u10_t_gmv_mvdiff = t_gmv_mvdiff;
	pOutput->u10_med_mvdiff = med_mvdiff;
	pOutput->u1_apl_upsample_en = u1_apl_upsample_en;
	pOutput->u1_fast_motion_cond = fast_motion_cond;
	if (print_result)
	{
		rtd_pr_memc_emerg("//********** [MEMC DH cond][gmv_grp01_diff: %d][mvdiff: %d][med_mvdiff: %d] **********//\n", gmv_grp01_mvdiff, mvdiff, med_mvdiff);
		rtd_pr_memc_emerg("//********** [MEMC DH cond][bgmvy: %d][bgmvx: %d][fgmvy: %d][fgmvx: %d] **********//\n", s10_global_bgmvy, s11_global_bgmvx,s10_global_fgmvy,s11_global_fgmvx);
	}

}

VOID MEMC_VAR_LPF_Calc_RTK2885pp(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput, int iSliceIdx)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	M8P_rtmc2_rtmc2_24_RBUS M8P_rtmc2_rtmc2_24_reg;
	unsigned char var_lpf_gain, var_lpf_sample_mode;
	static short var_lpf_tap_cnt=15;
	bool var_lpf_dynamic_gain_en;
	unsigned int print_in_func;
	if (pParam->u1_mc_var_lpf_calc_en == 0)
		return;
	
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);

	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	M8P_rtmc2_rtmc2_24_reg.regValue = rtd_inl(M8P_RTMC2_RTMC2_24_reg);
	var_lpf_gain = M8P_rtmc2_rtmc2_24_reg.mc_var_lpf_gain;
	if (s_pContext->_output_me_sceneAnalysis.u1_freq_scene || \
	   (s_pContext->_output_me_sceneAnalysis.u6_unstable_score > 32) || \
	   (s_pContext->_output_me_sceneAnalysis.u6_global_unstb_cnt > 32))
	{
		var_lpf_gain--;
	}
	else
	{
		var_lpf_gain++;
	}
	var_lpf_gain = _CLIP_(var_lpf_gain, 
		_MAX_(16 - ((_MAX_(s_pContext->_output_me_sceneAnalysis.u10_mvdiff - 128, 0) + _MAX_(s_pContext->_output_me_sceneAnalysis.u6_unstable_score - 64, 0)) >> 4), 8), 
		32);

	if ((s_pContext->_output_me_sceneAnalysis.u10_mvdiff > 100) || \
		(s_pContext->_output_me_sceneAnalysis.u1_freq_scene) || \
		(s_pContext->_output_me_sceneAnalysis.u6_global_unstb_cnt > 32))
	{
		var_lpf_tap_cnt++;
	}
	else
	{
		var_lpf_tap_cnt--;
	}
	var_lpf_tap_cnt = _CLIP_(var_lpf_tap_cnt, 0, 30);
	
	if (((var_lpf_tap_cnt <= 20) && \
		  ((_ABS_(s_pContext->_output_me_sceneAnalysis.s10_global_fgmvy) + _ABS_(s_pContext->_output_me_sceneAnalysis.s11_global_fgmvx)) < 16)) || \
		  (s_pContext->_output_me_sceneAnalysis.s10_global_fgmvy))
	{
		var_lpf_dynamic_gain_en = 0;
	}
	else
	{
		var_lpf_dynamic_gain_en = 1;
	}

	if (var_lpf_tap_cnt > 25)
	{
		var_lpf_sample_mode = 3;
	}
	else if (var_lpf_tap_cnt > 20)
	{
		var_lpf_sample_mode = 2;
	}
	else
	{
		var_lpf_sample_mode = 1;
	}

	pOutput->u6_var_lpf_gain = var_lpf_gain;
	pOutput->u1_var_lpf_dynamic_gain_en= var_lpf_dynamic_gain_en;
	pOutput->u2_var_lpf_sample_mode = var_lpf_sample_mode;
	
}

//#define v20_algo_en

VOID MEMC_Unstable_Score_Calc_RTK2885pp(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput, int iSliceIdx, bool update_input_info)
{
	
	unsigned char u8_i;
	unsigned int print_in_func;
#ifdef v20_algo_en
	unsigned char u8_j, u8_k;

	const _PQLCONTEXT *s_pContext = GetPQLContext();
	const unsigned char input_num_data = 4;
	static _Unstb_Input_Temporal_Info unstb_input_t_info[input_num_data];
	static int t_adpt_cnt = 0, small_unstb_cnt=0;
	static int apl_diff_t[20] = {0};
	unsigned short invalid_mv_unstb[32], invalid_mv_gmv_diff[32];
	unsigned int unstb_score[32];
	int mean_invalid_mvy, mean_invalid_mvx, mean_cnt, temp_min_diff, min_diff;
	int avg_gmv, mv_consist, cur_thr;
	unsigned char gain;
	int mean_rmvy, mean_rmvx, ucf_score, cur_ucf, tmp_ucf, invld_unstb, max_rmv;
	unsigned short mean_cov_cnt, mean_ucov_cnt, occl_chaos, cov_unstb, ucov_unstb, tcss_gain;
	unsigned short max_unstb_score = 0, mean_unstb_score = 0, unstb_score_cnt = 0;
	bool cond2_t = 0, cond2=0, cond1=0;
	static char g_unstb_cnt=0, g_mvd_unstb_cnt=0;
	unsigned char apl_diff_pos_cnt=0, apl_diff_neg_cnt=0;
	static int lfb_base64_th = 128;
	int lfb_gmv_grp1_max_th=280, iir_coef, th=128, cnt_coring = 0, max_mv;
	unsigned short final_unstb_score[32];
	short ii, jj, idx, max_idx, div;

#ifdef MEMC_120_model
	const unsigned char output_num_data = 10;
	
#else
	const unsigned char output_num_data = 5;
#endif
	static _Unstb_Output_Temporal_Info unstb_output_t_info[output_num_data];
#endif
	if (pParam->u1_unstable_score_calc_en == 0)
		return;

	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);

#ifdef v20_algo_en
	if (update_input_info)
	{
		if (print_in_func == 1)
		{
			rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][update input info][%d] **********//\n", __FUNCTION__, __LINE__);
		}
		for (u8_i=0;u8_i < (input_num_data-1);u8_i++)
		{
			unstb_input_t_info[u8_i] = unstb_input_t_info[u8_i+1];
		}

		
		for (u8_i=0;u8_i < 32;u8_i++)
		{
			unstb_input_t_info[input_num_data-1].me15_invalid_cnt[u8_i] = s_pContext->_output_read_comreg.u10_me15_invld_cnt_rb[u8_i];
			unstb_input_t_info[input_num_data-1].me15_invalid_rm_cnt[u8_i] = s_pContext->_output_read_comreg.u10_me15_invld_rm_cnt_rb[u8_i];

			unstb_input_t_info[input_num_data-1].me15_invalid_mvy[u8_i] = s_pContext->_output_read_comreg.s10_me15_invld_mv_vy_rb[u8_i];
			unstb_input_t_info[input_num_data-1].me15_invalid_mvx[u8_i] = s_pContext->_output_read_comreg.s11_me15_invld_mv_vx_rb[u8_i];

			unstb_input_t_info[input_num_data-1].me1_rmv_cnt[u8_i][0] = s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[u8_i];
			unstb_input_t_info[input_num_data-1].me1_rmv_ucf[u8_i][0] = s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[u8_i];
			unstb_input_t_info[input_num_data-1].me1_rmv_mvy[u8_i][0] = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_i];
			unstb_input_t_info[input_num_data-1].me1_rmv_mvx[u8_i][0] = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i];
			
			unstb_input_t_info[input_num_data-1].me1_rmv_cnt[u8_i][1] = s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[u8_i];
			unstb_input_t_info[input_num_data-1].me1_rmv_ucf[u8_i][1] = s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[u8_i];
			unstb_input_t_info[input_num_data-1].me1_rmv_mvy[u8_i][1] = s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[u8_i];
			unstb_input_t_info[input_num_data-1].me1_rmv_mvx[u8_i][1] = s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[u8_i];
			
			unstb_input_t_info[input_num_data-1].me1_rmv_cnt[u8_i][2] = s_pContext->_output_read_comreg.u10_me_rMV_3rd_cnt_rb[u8_i];
			unstb_input_t_info[input_num_data-1].me1_rmv_ucf[u8_i][2] = s_pContext->_output_read_comreg.u10_me_rMV_3rd_unconf_rb[u8_i];
			unstb_input_t_info[input_num_data-1].me1_rmv_mvy[u8_i][2] = s_pContext->_output_read_comreg.s10_me_rMV_3rd_vy_rb[u8_i];
			unstb_input_t_info[input_num_data-1].me1_rmv_mvx[u8_i][2] = s_pContext->_output_read_comreg.s11_me_rMV_3rd_vx_rb[u8_i];

			unstb_input_t_info[input_num_data-1].me1_rgn_tcss[u8_i] = s_pContext->_output_read_comreg.u12_me_rgn_tcss_rb[u8_i];
			unstb_input_t_info[input_num_data-1].me1_lfb_cnt[u8_i] = s_pContext->_output_read_comreg.u12_me_rgn_fblvl_rb[u8_i];

		}

		
		unstb_input_t_info[input_num_data-1].me1_gmv_cnt[0] = s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb;
		unstb_input_t_info[input_num_data-1].me1_gmv_ucf[0] = s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb;
		unstb_input_t_info[input_num_data-1].me1_gmv_mvy[0] = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
		unstb_input_t_info[input_num_data-1].me1_gmv_mvx[0] = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
		
		unstb_input_t_info[input_num_data-1].me1_gmv_cnt[1] = s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb;
		unstb_input_t_info[input_num_data-1].me1_gmv_ucf[1] = s_pContext->_output_read_comreg.u12_me_GMV_2nd_unconf_rb;
		unstb_input_t_info[input_num_data-1].me1_gmv_mvy[1] = s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb;
		unstb_input_t_info[input_num_data-1].me1_gmv_mvx[1] = s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb;
		
		unstb_input_t_info[input_num_data-1].me1_gmv_cnt[2] = s_pContext->_output_read_comreg.u16_me_GMV_3rd_cnt_rb;
		unstb_input_t_info[input_num_data-1].me1_gmv_ucf[2] = s_pContext->_output_read_comreg.u10_me_GMV_3rd_unconf_rb;
		unstb_input_t_info[input_num_data-1].me1_gmv_mvy[2] = s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb;
		unstb_input_t_info[input_num_data-1].me1_gmv_mvx[2] = s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb;
		return;
	}
#endif

	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}

	pOutput->u6_unstable_score = 0;
	pOutput->u6_mean_unstable_score = 0;
	pOutput->u6_temporal_adpt_cnt = 0;
	pOutput->u6_unstable_score_cnt = 0;
	pOutput->u5_small_unstb_cnt = 0;
	pOutput->u6_global_unstb_cnt = 0;
	pOutput->u1_freq_scene = 0;
	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		pOutput->u6_rgn_unstable_score[u8_i] = 0;
		pre_unstable_score[u8_i] = 0;
	}

#ifdef v20_algo_en
	
	for (u8_i=0;u8_i < (output_num_data-1);u8_i++)
	{
		unstb_output_t_info[u8_i] = unstb_output_t_info[u8_i+1];
	}
	
	for (u8_i=0;u8_i < 32;u8_i++)
	{
		unstb_output_t_info[output_num_data-1].dh_cov_cnt[u8_i] = s_pContext->_output_read_comreg.u12_dh_cov_cnt_rb[u8_i];
		unstb_output_t_info[output_num_data-1].dh_ucov_cnt[u8_i] = s_pContext->_output_read_comreg.u12_dh_ucov_cnt_rb[u8_i];
	}

	avg_gmv = 0;
	for (u8_j=0;u8_j < input_num_data;u8_j++)
	{
		avg_gmv += _ABS_(unstb_input_t_info[u8_j].me1_gmv_mvy[0]);
		avg_gmv += _ABS_(unstb_input_t_info[u8_j].me1_gmv_mvx[0]);
	}
	avg_gmv /= input_num_data;

	if (avg_gmv < 128)
		gain = 32 - (_MAX_((avg_gmv - 64), 0) >> 4);
	else if (avg_gmv < 192)
		gain = 28 - (_MAX_((avg_gmv - 128), 0) >> 3);
	else if (avg_gmv < 256)
		gain = 12 - (_MAX_((avg_gmv - 192), 0) >> 4);
	else
		gain = 8;

	for (u8_i=0;u8_i < 32;u8_i++)
	{
		mean_invalid_mvy = 0;
		mean_invalid_mvx = 0;
		mean_cnt = 0;
		for (u8_j=0;u8_j < (input_num_data-1);u8_j++)
		{
			mean_invalid_mvy += unstb_input_t_info[u8_j].me15_invalid_mvy[u8_i] * unstb_input_t_info[u8_j].me15_invalid_cnt[u8_i];
			mean_invalid_mvx += unstb_input_t_info[u8_j].me15_invalid_mvx[u8_i] * unstb_input_t_info[u8_j].me15_invalid_cnt[u8_i];
			mean_cnt += (int)unstb_input_t_info[u8_j].me15_invalid_cnt;
		}
		mean_invalid_mvy = mean_invalid_mvy / _MAX_(mean_cnt, 1);
		mean_invalid_mvx = mean_invalid_mvx / _MAX_(mean_cnt, 1);
		invalid_mv_unstb[u8_i] = _ABS_DIFF_(unstb_input_t_info[(input_num_data-1)].me15_invalid_mvy[u8_i], mean_invalid_mvy) + \
								 _ABS_DIFF_(unstb_input_t_info[(input_num_data-1)].me15_invalid_mvx[u8_i], mean_invalid_mvx);

		min_diff = 65535;
		for (u8_j=0;u8_j < input_num_data;u8_j++)
		{
			temp_min_diff = _ABS_DIFF_(unstb_input_t_info[(input_num_data-1)].me15_invalid_mvy[u8_i], unstb_input_t_info[u8_j].me1_gmv_mvy[0]) + \
							_ABS_DIFF_(unstb_input_t_info[(input_num_data-1)].me15_invalid_mvx[u8_i], unstb_input_t_info[u8_j].me1_gmv_mvx[0]);
			if (temp_min_diff < min_diff)
			{
				min_diff = temp_min_diff;
				invalid_mv_gmv_diff[u8_i] = temp_min_diff;
			}
		}

		unstb_score[u8_i] = 0;
		invalid_mv_unstb[u8_i] = (invalid_mv_unstb[u8_i] * _CLIP_(((invalid_mv_gmv_diff[u8_i] * gain) >> 5) - 16, 0, 32)) >> 5;
		for (u8_k=0;u8_k < 3;u8_k++)
		{
			mean_rmvy = 0;
			mean_rmvx = 0;
			for (u8_j=0;u8_j < input_num_data;u8_j++)
			{
				mean_rmvy += unstb_input_t_info[u8_j].me1_rmv_mvy[u8_i][u8_k];
				mean_rmvx += unstb_input_t_info[u8_j].me1_rmv_mvx[u8_i][u8_k];
			}
			mean_rmvy /= input_num_data;
			mean_rmvx /= input_num_data;
			tmp_ucf = unstb_input_t_info[input_num_data - 1].me1_rmv_ucf[u8_i][u8_k] + invalid_mv_unstb[u8_i];
			cur_ucf = unstb_input_t_info[input_num_data - 1].me1_rmv_cnt[u8_i][u8_k] * ((tmp_ucf * tmp_ucf) >> 4);
			for (u8_j=0;u8_j < input_num_data;u8_j++)
			{
				mv_consist = _ABS_DIFF_(unstb_input_t_info[u8_j].me1_rmv_mvy[u8_i][u8_k], mean_rmvy) + \
							 _ABS_DIFF_(unstb_input_t_info[u8_j].me1_rmv_mvx[u8_i][u8_k], mean_rmvx);
				ucf_score = (unstb_input_t_info[u8_j].me1_rmv_cnt[u8_i][u8_k] * \
							 (unstb_input_t_info[u8_j].me1_rmv_ucf[u8_i][u8_k] * unstb_input_t_info[u8_j].me1_rmv_ucf[u8_i][u8_k])) >> 4;
				ucf_score = (ucf_score + cur_ucf) >> 1;
				unstb_score[u8_i] += mv_consist * ucf_score;
			}
		}
		unstb_score[u8_i] = unstb_score[u8_i] >> 10;
		unstb_score[u8_i] = unstb_score[u8_i] / (input_num_data * 3);

		invld_unstb = unstb_input_t_info[input_num_data - 1].me15_invalid_cnt[u8_i] * unstb_input_t_info[input_num_data - 1].me15_invalid_rm_cnt[u8_i];
		invld_unstb = _MAX_(invld_unstb, 0);
		if (s_pContext->_output_me_sceneAnalysis.u10_med_mvdiff < 32)
			invld_unstb = _MIN_(invld_unstb, 100);

		unstb_score[u8_i] = (unstb_score[u8_i] * invld_unstb) >> 15;


		mean_cov_cnt = 0;
		mean_ucov_cnt = 0;
		for (u8_j=0;u8_j < output_num_data;u8_j++)
		{
			mean_cov_cnt += unstb_output_t_info[u8_j].dh_cov_cnt[u8_i];
			mean_ucov_cnt += unstb_output_t_info[u8_j].dh_ucov_cnt[u8_i];
		}
		mean_cov_cnt = mean_cov_cnt / output_num_data;
		mean_ucov_cnt = mean_ucov_cnt / output_num_data;
		occl_chaos = _MIN_(_MAX_(mean_cov_cnt, unstb_output_t_info[output_num_data - 1].dh_cov_cnt[u8_i]),
						   _MAX_(mean_ucov_cnt, unstb_output_t_info[output_num_data - 1].dh_ucov_cnt[u8_i])),
		occl_chaos = _MAX_(occl_chaos, _MIN_(_MAX_(invalid_mv_unstb[u8_i] - 16, 0) * 24, 512));
		unstb_score[u8_i] = (unstb_score[u8_i] * occl_chaos) >> 6;

		cov_unstb = 0;
		ucov_unstb = 0;
		max_rmv = 0;
		for (u8_j=0;u8_j < output_num_data;u8_j++)
		{
			cov_unstb += _ABS_DIFF_(unstb_output_t_info[u8_j].dh_cov_cnt[u8_i], mean_cov_cnt);
			ucov_unstb += _ABS_DIFF_(unstb_output_t_info[u8_j].dh_ucov_cnt[u8_i], mean_ucov_cnt);
			
		}
		for (u8_j=0;u8_j < input_num_data;u8_j++)
		{
			max_rmv = _MAX_(max_rmv, _ABS_(unstb_input_t_info[u8_j].me1_rmv_mvy[u8_i][0]) + _ABS_(unstb_input_t_info[u8_j].me1_rmv_mvx[u8_i][0]));
		}

		cur_thr = 0;
		for (u8_j=0;u8_j < (output_num_data - 1);u8_j++)
		{
			cur_thr += unstb_output_t_info[u8_j].tcss_score[u8_i];
			
		}
		cur_thr /= (output_num_data - 1);
		cur_thr = _CLIP_(cur_thr, 2000, 6000);
		unstb_output_t_info[output_num_data - 1].tcss_score[u8_i] = _MAX_((unstb_input_t_info[input_num_data-1].me1_rgn_tcss[u8_i] - \
																		   _MAX_((max_rmv - 64), 0) * 64 - cur_thr) >> 1, 1);
		tcss_gain = 0;
		for (u8_j=0;u8_j < output_num_data;u8_j++)
		{
			tcss_gain = _MAX_(tcss_gain, unstb_output_t_info[u8_j].tcss_score[u8_i]);
		}
		tcss_gain = _MAX_(tcss_gain, _MAX_(invalid_mv_unstb[u8_i] - 16, 0) * 128 - ((cur_thr - 2000) >> 1));
		tcss_gain = _CLIP_(tcss_gain, 1, 2048);
		unstb_score[u8_i] = (unstb_score[u8_i] * _MAX_(cov_unstb, ucov_unstb) * tcss_gain) >> 11;
		unstb_score[u8_i] = unstb_score[u8_i] / 5120;
		if (s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 128)
			unstb_score[u8_i] = unstb_score[u8_i] * (_CLIP_(avg_gmv - 32, 0, 64) >> 6);


		max_unstb_score = _MAX_(max_unstb_score, unstb_score[u8_i]);
		mean_unstb_score += unstb_score[u8_i];
		if (unstb_score[u8_i] > 32)
			unstb_score_cnt++;
	}
	if (mean_unstb_score > 32)
		t_adpt_cnt = 60;
	else
		t_adpt_cnt -= 1;

	if ((s_pContext->_output_me_sceneAnalysis.u10_mvdiff < 128) &&
		(s_pContext->_output_me_sceneAnalysis.u10_mvdiff > 64) &&
		max_unstb_score > 64 &&
		unstb_score_cnt < 4)
		small_unstb_cnt += 8;
	else
		small_unstb_cnt -= 1;
	small_unstb_cnt = _CLIP_(small_unstb_cnt, 0, 32);
	
	t_adpt_cnt = _MAX_(t_adpt_cnt, 0);


	
	for (u8_i=0;u8_i < 32;u8_i++)
	{
		if (_MIN_(unstb_output_t_info[output_num_data-1].dh_cov_cnt[u8_i],
				  unstb_output_t_info[output_num_data-1].dh_ucov_cnt[u8_i]) > \
			 (_MAX_(unstb_output_t_info[output_num_data-1].dh_cov_cnt[u8_i],
				   unstb_output_t_info[output_num_data-1].dh_ucov_cnt[u8_i]) >> 1))
		{
			cond2_t = 1;
			if (((u8_i % 8) != 0) && ((u8_i % 8) != 7) && (u8_i > 7) && (u8_i < 24) && (unstb_score[u8_i] > 63))
				cond2=1;
		}
	}
	
	cond1 = (s_pContext->_output_me_sceneAnalysis.u10_global_fgmv_cnt_var > (s_pContext->_output_me_sceneAnalysis.u10_global_fgmv_cnt_mean >> 1)) && \
			 (s_pContext->_output_me_sceneAnalysis.u10_mvdiff > 90) && \
			 ((max_unstb_score > 127) || cond2_t);
	
	cond2 = cond2 && (s_pContext->_output_me_sceneAnalysis.u10_mvdiff > 90);

	if (cond1)
	{
		g_unstb_cnt += ((s_pContext->_output_read_comreg.u32_me_sc_pattern_rb & 0x7) > 0) ? 3 : 2;
	}
	else
	{
		g_unstb_cnt -= 1;
	}
	g_unstb_cnt = _CLIP_(g_unstb_cnt, 0, 64);

	if (cond1)
	{
		g_mvd_unstb_cnt += ((s_pContext->_output_read_comreg.u32_me_sc_pattern_rb & 0x7) > 0) ? 3 : 2;
		if (cond2)
			g_mvd_unstb_cnt += 31;
	}
	else if (cond2)
	{
		g_mvd_unstb_cnt += 33;
	}
	else
	{
		g_mvd_unstb_cnt -= 1;
	}
	g_mvd_unstb_cnt = _CLIP_(g_mvd_unstb_cnt, 0, 64);

	for (u8_i=0; u8_i<19; u8_i++)
	{
		apl_diff_t[u8_i] = apl_diff_t[u8_i+1];
	}
	apl_diff_t[19] = (int)s_pContext->_output_read_comreg.u26_me_glb_aplp_rb - (int)s_pContext->_output_read_comreg.u26_me_glb_apli_rb;

	for (u8_i=0; u8_i<20; u8_i++)
	{
		if (apl_diff_t[u8_i] >= 0)
			apl_diff_pos_cnt++;
		else
			apl_diff_neg_cnt++;
	}
	mean_unstb_score = (mean_unstb_score + 16) >> 5;
	
	for (u8_j=0;u8_j < input_num_data;u8_j++)
	{
		cnt_coring += unstb_input_t_info[u8_j].me1_gmv_cnt[0];
	}
	cnt_coring = _MAX_(((cnt_coring / input_num_data) - 10000 ) >> 5, 0);
	cnt_coring = (cnt_coring * (64 - g_unstb_cnt) + 32) >> 6;
	if (g_unstb_cnt > 16)
	{
		th = 128 + ((mean_unstb_score * _MAX_(s_pContext->_output_me_sceneAnalysis.u16_gmv_max_mv - 128 - cnt_coring, 1) + 8) >> 4);
		th = _CLIP_(th, 128, 255);
	}
	iir_coef = lfb_base64_th > th ? 29 : 3;
	lfb_base64_th = (lfb_base64_th * iir_coef + th * (32 - iir_coef) + 16) >> 5;
	max_mv = _MAX_(_ABS_(s_pContext->_output_me_sceneAnalysis.s10_global_bgmvy) + _ABS_(s_pContext->_output_me_sceneAnalysis.s11_global_bgmvx),
				   _ABS_(s_pContext->_output_me_sceneAnalysis.s10_global_fgmvy) + _ABS_(s_pContext->_output_me_sceneAnalysis.s11_global_fgmvx));
	lfb_base64_th = _CLIP_(_MIN_(lfb_base64_th, max_mv), 128, 255);
	if ((_MAX_(_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb) + _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb),
				_ABS_(s_pContext->_output_read_comreg.s10_me_GMV_3rd_vy_rb) + _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_3rd_vx_rb)) > 0) && \
		 (g_unstb_cnt > 32))
	{
		for (u8_j=0;u8_j < input_num_data;u8_j++)
		{
			lfb_gmv_grp1_max_th = _MIN_(_ABS_(unstb_input_t_info[u8_j].me1_gmv_mvx[1]) + _ABS_(unstb_input_t_info[u8_j].me1_gmv_mvy[1]), lfb_gmv_grp1_max_th);
			lfb_gmv_grp1_max_th = _MIN_(_ABS_(unstb_input_t_info[u8_j].me1_gmv_mvx[2]) + _ABS_(unstb_input_t_info[u8_j].me1_gmv_mvy[2]), lfb_gmv_grp1_max_th);
		}
	}
	lfb_gmv_grp1_max_th = _CLIP_(lfb_gmv_grp1_max_th, 0, 511);

	
	for (u8_j=0;u8_j < 4;u8_j++)
	{
		for (u8_i=0;u8_i < 8;u8_i++)
		{
			final_unstb_score[u8_j*4+u8_i] = 0;
			for (jj=-1;jj <= 1;jj++)
			{
				for (ii=-1;ii <= 1;ii++)
				{
					idx = _CLIP_((short)u8_j + jj, 0, 3) * 8 + _CLIP_((short)u8_i + ii, 0, 7);
					if (unstb_score[idx] > final_unstb_score[u8_j*4+u8_i])
					{
						final_unstb_score[u8_j*4+u8_i] = unstb_score[idx];
						max_idx = (jj + 1) * 3 + ii + 1;
					}
					
				}
			}
			if (max_idx == 4)
				div = 0;
			else if ((max_idx == 0) || (max_idx == 2) || (max_idx == 6) || (max_idx == 8))
				div = 3;
			else
				div = 2;
			final_unstb_score[u8_j*4+u8_i] = final_unstb_score[u8_j*4+u8_i] >> div;
			pOutput->u1_large_rgn_unstb[u8_i] = max_idx != 4;

		}
	}
	
	pOutput->u6_unstable_score = max_unstb_score;
	pOutput->u6_mean_unstable_score = mean_unstb_score;
	pOutput->u6_temporal_adpt_cnt = t_adpt_cnt;
	pOutput->u6_unstable_score_cnt = unstb_score_cnt;
	pOutput->u5_small_unstb_cnt = small_unstb_cnt;
	pOutput->u6_global_unstb_cnt = g_unstb_cnt;
	pOutput->u6_global_mvd_unstb_cnt = g_mvd_unstb_cnt;
	pOutput->u1_apl_var_cond = _MIN_(apl_diff_pos_cnt, apl_diff_neg_cnt) > 4;
	pOutput->u8_lfb_base64_th = lfb_base64_th;
	pOutput->u9_lfb_gmv_grp1_max_th = lfb_gmv_grp1_max_th;
	pOutput->u16_avg_gmv = avg_gmv;
	pOutput->u1_freq_scene = 0;
	for (u8_i = 0; u8_i < 32; u8_i++)
	{
		pOutput->u6_rgn_unstable_score[u8_i] = unstb_score[u8_i];
		pOutput->u8_final_rgn_unstable_score[u8_i] = final_unstb_score[u8_i];
		pre_unstable_score[u8_i] = unstb_score[u8_i];
	}
#endif	
}

inline VOID _Median5_(int x0, int x1, int x2, int x3, int x4, int *med_val, unsigned char *med_idx)
{
	int med0_val, med1_val, med2_val;
	unsigned char med0_idx, med1_idx, med2_idx, final_med_idx;
	_Median3_(x0, x1, x2, &med0_val, &med0_idx);
	_Median3_(x1, x2, x3, &med1_val, &med1_idx);
	_Median3_(x2, x3, x4, &med2_val, &med2_idx);

	_Median3_(med0_val, med1_val, med2_val, med_val, &final_med_idx);
	if (final_med_idx == 0)
	{
		*med_idx = med0_idx;
	}
	else if (final_med_idx == 1)
	{
		*med_idx = med1_idx + 1;
	}
	else
	{
		*med_idx = med2_idx + 2;
	}
}

inline VOID _Median3_(int x0, int x1, int x2, int *med_val, unsigned char *med_idx)
{
	if (x0 > x1)
	{
		if (x1 > x2)
		{
			*med_val = x1;
			*med_idx = 1;
		}
		else if (x0 > x2){
			*med_val = x2;
			*med_idx = 2;
		}
		else{
			*med_val = x0;
			*med_idx = 0;
		}
	}
	else
	{
		if (x0 > x2)
		{
			*med_val = x0;
			*med_idx = 0;
		}
		else if (x1 > x2){
			*med_val = x2;
			*med_idx = 2;
		}
		else{
			*med_val = x1;
			*med_idx = 1;
		}
	}
}

