#ifndef _ME_SCENE_ANALYSIS_DEF_H
#define _ME_SCENE_ANALYSIS_DEF_H
//#define PRINT_GMV
//#define BG_MV_GET


typedef struct
{
	unsigned char  u1_RepeatPattern_en;
	unsigned char  u1_Natural_HighFreq_en;
	unsigned char  u1_LightSword_en;
	unsigned char  u1_BigAPLDiff_en;

	// casino
	unsigned int u16_casino_rimLft_th;
	unsigned int u16_casino_rimRht_th;
	unsigned int u16_casino_rimUp_th;
	unsigned int u16_casino_rimDn_th;
	unsigned char  u8_casino_rim_delta;

	unsigned char  u1_EPG_en;

	unsigned char  u1_DSR_en;//DynamicSearchRange_detect, u1_DSR_en

	unsigned char u1_over_searchrange_en;
	unsigned char u1_force_panning;
	unsigned char u8_over_searchrange_thl;

	// occl extension
	unsigned char u1_occl_type2_enable;
	unsigned char u8_occl_ext_bg_mv_diff_thl;
	unsigned char u1_occl_ext_gmv_weight_mode;
	unsigned char u4_occl_ext_2nd_gmv_uconf_en_thl;
	unsigned char u4_occl_ext_2nd_gmv_uconf_dis_thl;
	unsigned char u1_occl_ext_force_protection_off;
	unsigned char u1_occl_ext_force_lvl_en;
	unsigned char u3_occl_ext_force_lvl;
	unsigned char u4_occl_ext_rgn_thl;
	unsigned char u1_occl_ext_avoidLogo_en;
	unsigned char u8_occl_ext_LogoCount_thr;

	// post correction
	unsigned char u1_dh_postcorr_log_en;
	unsigned char u3_dh_postcorr_force_lvl;
	unsigned char u1_dh_postcorr_force_lvl_en;
	unsigned char u1_dh_postcorrt_dyn_calc_en;
	unsigned char u8_dh_postcorr_LogoCount_thr;

	unsigned char u8_dh_postcorr_GmvMvy_thr;
	unsigned char u8_dh_postcorr_y2x_ratio_x0;
	unsigned char u8_dh_postcorr_y2x_ratio_x1;
	unsigned char u8_dh_postcorr_y2x_ratio_y0;
	unsigned char u8_dh_postcorr_y2x_ratio_y1;

	// main object motion
	unsigned char u4_MainObjectMove_bgMv_thl;
	unsigned char u4_MainObjectMove_objBgMvDiff_thl;
	unsigned char u8_MainObjectMove_dtl_thl;
	unsigned char u8_MainObjectMove_thl_1;
	unsigned char u8_MainObjectMove_thl_2;
	unsigned char u8_MainObjectMove_1st_uconf_thr;
	unsigned char u8_MainObjectMove_2nd_uconf_thr;

	// ME median filter
	bool  b_relMV_panning_en;
	bool  b_prd_en;
	bool  b_logo_en;
	bool  b_show_mode;
	bool  b_pan_noFB_en;
	bool  b_multi_GMV_en;
	unsigned char u2_medFLT_sel;
	unsigned char u3_show_mode2;
	unsigned char u8_mvDiff_thl;
	unsigned char u8_rFBss_thl;
	unsigned char u8_rSCss_thl;
	unsigned char u8_big_relMV_cnt_thl;
	unsigned char u8_rChaos_cnt_thl;
	unsigned char u8_panning_gmv_cnt_thl;
	unsigned char u4_panning_gmv_unconf_thl;
	unsigned char u8_rPrd_thl;
	unsigned char u8_rPrd_cnt_thl;
	// ME median filter
	bool  b_medFLT_adpt_en;
	bool  b_pfvd_en;

	// repeat periodic pattern
	bool b_FixRepeat_en;
	bool b_FixRepeat_db_en;

	// detect sport scene
	bool  u1_detect_SportScene_en;
	bool u1_chg_color_matrix_en;

	// detect rotate windmill
	bool u1_detect_Rotate_WindMill_en;
}_PARAM_ME_SCENE_ANALYSIS;

/*
	SMART INTEGRATION STATUS (SIS) for MEMC ver. by LearnRPG@20150911

	Collection and combination all those status by long time experience.
	We suggest in kernal API don't have below code
	(that's mean kernal API must independent operation)
	1. no global variable, must use function parameter be input.
	2. don't change global status in function, must use middle layer to update status.

*/

typedef struct _MemcStatus
{
/*
	unsigned int reg_vbuf_ph_1st_lfsr_mask_x;
	unsigned int reg_vbuf_ph_1st_lfsr_mask_y;
	unsigned int reg_vbuf_ph_2nd_lfsr_mask_x;
	unsigned int reg_vbuf_ph_2nd_lfsr_mask_y;

	unsigned int reg_vbuf_ip_1st_lfsr_mask_x;
	unsigned int reg_vbuf_ip_1st_lfsr_mask_y;
	unsigned int reg_vbuf_ip_2nd_lfsr_mask_x;
	unsigned int reg_vbuf_ip_2nd_lfsr_mask_y;

	unsigned int reg_vbuf_pi_1st_lfsr_mask_x;
	unsigned int reg_vbuf_pi_1st_lfsr_mask_y;
	unsigned int reg_vbuf_pi_2nd_lfsr_mask_x;
	unsigned int reg_vbuf_pi_2nd_lfsr_mask_y;

	unsigned int reg_metop_ss_ip_update0_penalty;
	unsigned int reg_metop_ss_ip_update1_penalty;
	unsigned int reg_metop_ss_pi_update0_penalty;
	unsigned int reg_metop_ss_pi_update1_penalty;
	unsigned int reg_metop_ss_ph_update0_penalty;
	unsigned int reg_metop_ss_ph_update1_penalty;

	unsigned int reg_metop_ds_ip_update0_penalty;
	unsigned int reg_metop_ds_ip_update1_penalty;
	unsigned int reg_metop_ds_pi_update0_penalty;
	unsigned int reg_metop_ds_pi_update1_penalty;
	unsigned int reg_metop_ds_ph_update0_penalty;
	unsigned int reg_metop_ds_ph_update1_penalty;

	unsigned char  u3_ip_1st_cand1_offsetx;
	unsigned char  u3_ip_1st_cand5_offsetx;
	unsigned char  u3_ip_1st_update1_offsetx;
	unsigned char  u3_pi_1st_cand1_offsetx;
	unsigned char  u3_pi_1st_cand5_offsetx;
	unsigned char  u3_pi_1st_update1_offsetx;
	unsigned char  u3_ph_1st_cand1_offsetx;
	unsigned char  u3_ph_1st_cand2_offsetx;
	unsigned char  u3_ph_1st_cand2_offsety;
	unsigned char  u3_ph_1st_cand5_offsetx;
	unsigned char  u3_ph_1st_update1_offsetx;

	unsigned char  u3_ip_2nd_cand1_offsetx;
	unsigned char  u3_ip_2nd_cand5_offsetx;
	unsigned char  u3_ip_2nd_update1_offsetx;
	unsigned char  u3_pi_2nd_cand1_offsetx;
	unsigned char  u3_pi_2nd_cand5_offsetx;
	unsigned char  u3_pi_2nd_update1_offsetx;
	unsigned char  u3_ph_2nd_cand1_offsetx;
	unsigned char  u3_ph_2nd_cand2_offsetx;
	unsigned char  u3_ph_2nd_cand2_offsety;
	unsigned char  u3_ph_2nd_cand5_offsetx;
	unsigned char  u3_ph_2nd_update1_offsetx;

	unsigned int reg_mc_var_lpf_en;
	unsigned int reg_mc_var_lpf_mvd_x_th;

	unsigned char reg_metop_sad_th_mode_sel;
	unsigned char reg_metop_ss_ip_penalty_g;
	unsigned char reg_metop_ss_pi_penalty_g;
	unsigned char reg_metop_ss_ph_penalty_g;
	unsigned char reg_metop_ds_ip_penalty_g;
	unsigned char reg_metop_ds_pi_penalty_g;
	unsigned char reg_metop_ds_ph_penalty_g;

	unsigned char reg_metop_ss_ip_penalty_z;
	unsigned char reg_metop_ss_pi_penalty_z;
	unsigned char reg_metop_ss_ph_penalty_z;
	unsigned char reg_metop_ds_ip_penalty_z;
	unsigned char reg_metop_ds_pi_penalty_z;
	unsigned char reg_metop_ds_ph_penalty_z;

	unsigned char reg_dh_empty_proc_en;

	unsigned int reg_metop_sc_clr_saddiff_th;
	unsigned char reg_pql_fb_tc_iir;

        //RO reg
	unsigned int u25_rdbk_dtl_all;
	unsigned int u25_rdbk_dtl_all_pre;
	unsigned int u25_rdbk_dtl_all_intervel5;
	unsigned int u25_rdbk_dtl_all_intervel5_pre;
	unsigned char  u8_rgn_apl_p_rb[32];
	unsigned char  u8_rgn_apl_p_rb_pre[32];

	//analysis result
	unsigned char u1_apl_inc;
	unsigned char u1_apl_dec;
*/
	unsigned int reg_vbuf_ip_1st_cand0_en;
	unsigned int reg_vbuf_ip_1st_cand1_en;
	unsigned int reg_vbuf_ip_1st_cand2_en;
	unsigned int reg_vbuf_ip_1st_cand3_en;
	unsigned int reg_vbuf_ip_1st_cand4_en;
	unsigned int reg_vbuf_ip_1st_cand5_en;
	unsigned int reg_vbuf_ip_1st_cand6_en;
	unsigned int reg_vbuf_ip_1st_cand7_en;
	unsigned int reg_vbuf_ip_1st_cand8_en;
	unsigned int reg_vbuf_pi_1st_cand0_en;
	unsigned int reg_vbuf_pi_1st_cand1_en;
	unsigned int reg_vbuf_pi_1st_cand2_en;
	unsigned int reg_vbuf_pi_1st_cand3_en;
	unsigned int reg_vbuf_pi_1st_cand4_en;
	unsigned int reg_vbuf_pi_1st_cand5_en;
	unsigned int reg_vbuf_pi_1st_cand6_en;
	unsigned int reg_vbuf_pi_1st_cand7_en;
	unsigned int reg_vbuf_pi_1st_cand8_en;

	unsigned int reg_vbuf_ip_1st_update1_en;
	unsigned int reg_vbuf_ip_1st_update2_en;
	unsigned int reg_vbuf_ip_1st_update3_en;
	unsigned int reg_vbuf_ip_1st_update0_en;

	unsigned int reg_vbuf_pi_1st_update1_en;
	unsigned int reg_vbuf_pi_1st_update2_en;
	unsigned int reg_vbuf_pi_1st_update3_en;
	unsigned int reg_vbuf_pi_1st_update0_en;

	// candidate position
	unsigned int reg_vbuf_ip_1st_cand0_offsetx;
	unsigned int reg_vbuf_ip_1st_cand0_offsety;
	unsigned int reg_vbuf_ip_1st_cand1_offsetx;
	unsigned int reg_vbuf_ip_1st_cand1_offsety;
	unsigned int reg_vbuf_ip_1st_cand2_offsetx;
	unsigned int reg_vbuf_ip_1st_cand2_offsety;
	unsigned int reg_vbuf_ip_1st_cand3_offsetx;
	unsigned int reg_vbuf_ip_1st_cand3_offsety;
	unsigned int reg_vbuf_ip_1st_cand4_offsetx;
	unsigned int reg_vbuf_ip_1st_cand4_offsety;
	unsigned int reg_vbuf_ip_1st_cand5_offsetx;
	unsigned int reg_vbuf_ip_1st_cand5_offsety;
	unsigned int reg_vbuf_ip_1st_cand6_offsetx;
	unsigned int reg_vbuf_ip_1st_cand6_offsety;
	unsigned int reg_vbuf_ip_1st_cand7_offsetx;
	unsigned int reg_vbuf_ip_1st_cand7_offsety;

	unsigned int reg_vbuf_pi_1st_cand0_offsetx;
	unsigned int reg_vbuf_pi_1st_cand0_offsety;
	unsigned int reg_vbuf_pi_1st_cand1_offsetx;
	unsigned int reg_vbuf_pi_1st_cand1_offsety;
	unsigned int reg_vbuf_pi_1st_cand2_offsetx;
	unsigned int reg_vbuf_pi_1st_cand2_offsety;
	unsigned int reg_vbuf_pi_1st_cand3_offsetx;
	unsigned int reg_vbuf_pi_1st_cand3_offsety;
	unsigned int reg_vbuf_pi_1st_cand4_offsetx;
	unsigned int reg_vbuf_pi_1st_cand4_offsety;
	unsigned int reg_vbuf_pi_1st_cand5_offsetx;
	unsigned int reg_vbuf_pi_1st_cand5_offsety;
	unsigned int reg_vbuf_pi_1st_cand6_offsetx;
	unsigned int reg_vbuf_pi_1st_cand6_offsety;
	unsigned int reg_vbuf_pi_1st_cand7_offsetx;
	unsigned int reg_vbuf_pi_1st_cand7_offsety;

	unsigned int reg_vbuf_ip_1st_update0_offsetx;
	unsigned int reg_vbuf_ip_1st_update0_offsety;
	unsigned int reg_vbuf_ip_1st_update1_offsetx;
	unsigned int reg_vbuf_ip_1st_update1_offsety;
	unsigned int reg_vbuf_ip_1st_update2_offsetx;
	unsigned int reg_vbuf_ip_1st_update2_offsety;
	unsigned int reg_vbuf_ip_1st_update3_offsetx;
	unsigned int reg_vbuf_ip_1st_update3_offsety;

	unsigned int reg_vbuf_pi_1st_update0_offsetx;
	unsigned int reg_vbuf_pi_1st_update0_offsety;
	unsigned int reg_vbuf_pi_1st_update1_offsetx;
	unsigned int reg_vbuf_pi_1st_update1_offsety;
	unsigned int reg_vbuf_pi_1st_update2_offsetx;
	unsigned int reg_vbuf_pi_1st_update2_offsety;
	unsigned int reg_vbuf_pi_1st_update3_offsetx;
	unsigned int reg_vbuf_pi_1st_update3_offsety;

	// candidate penalty
	unsigned int reg_me1_ip_cddpnt_st0;
	unsigned int reg_me1_ip_cddpnt_st1;
	unsigned int reg_me1_ip_cddpnt_st2;
	unsigned int reg_me1_ip_cddpnt_st3;
	unsigned int reg_me1_ip_cddpnt_st4;
	unsigned int reg_me1_ip_cddpnt_st5;
	unsigned int reg_me1_ip_cddpnt_st6;
	unsigned int reg_me1_ip_cddpnt_st7;
	unsigned int reg_me1_pi_cddpnt_st0;
	unsigned int reg_me1_pi_cddpnt_st1;
	unsigned int reg_me1_pi_cddpnt_st2;
	unsigned int reg_me1_pi_cddpnt_st3;
	unsigned int reg_me1_pi_cddpnt_st4;
	unsigned int reg_me1_pi_cddpnt_st5;
	unsigned int reg_me1_pi_cddpnt_st6;
	unsigned int reg_me1_pi_cddpnt_st7;

	unsigned int reg_me1_ip_cddpnt_rnd0;
	unsigned int reg_me1_ip_cddpnt_rnd1;
	unsigned int reg_me1_ip_cddpnt_rnd2;
	unsigned int reg_me1_ip_cddpnt_rnd3;
	unsigned int reg_me1_pi_cddpnt_rnd0;
	unsigned int reg_me1_pi_cddpnt_rnd1;
	unsigned int reg_me1_pi_cddpnt_rnd2;
	unsigned int reg_me1_pi_cddpnt_rnd3;

	unsigned int reg_me1_ip_cddpnt_gmv;
	unsigned int reg_me1_pi_cddpnt_gmv;


       //  h/v  scalar down filter
	unsigned int reg_ipme_h_flp_alp0    ;
	unsigned int reg_ipme_h_flp_alp1     ;
	unsigned int reg_ipme_h_flp_alp2     ;
	unsigned int reg_ipme_h_flp_alp3     ;
	unsigned int reg_ipme_h_flp_alp4     ;
	unsigned int reg_ipme_h_flp_alp5     ;
	unsigned int reg_ipme_h_flp_alp6     ;
	unsigned int reg_ipme_v_flp_alp0     ;
	unsigned int reg_ipme_v_flp_alp1     ;
	unsigned int reg_ipme_v_flp_alp2     ;
	unsigned int reg_ipme_v_flp_alp3     ;
	unsigned int reg_ipme_v_flp_alp4     ;
	unsigned int reg_ipme_v_flp_alp5     ;
	unsigned int reg_ipme_v_flp_alp6     ;

       //me1 mvdiff curve
	unsigned int reg_me1_mvd_cuv_x1   ;
	unsigned int reg_me1_mvd_cuv_x2   ;
	unsigned int reg_me1_mvd_cuv_x3   ;
	unsigned int reg_me1_mvd_cuv_y1   ;
	unsigned int reg_me1_mvd_cuv_y2   ;
	unsigned int reg_me1_mvd_cuv_y3   ;
	unsigned int reg_me1_mvd_cuv_slope1    ;
	unsigned int reg_me1_mvd_cuv_slope2    ;
	 //me1 adptpnt rnd curve
	unsigned int reg_me1_adptpnt_rnd_y1   ;
	unsigned int reg_me1_adptpnt_rnd_y2   ;
	unsigned int reg_me1_adptpnt_rnd_y3   ;
	unsigned int reg_me1_adptpnt_rnd_y4    ;
	unsigned int reg_me1_adptpnt_rnd_slope2    ;

	unsigned int reg_me1_gmv_sel;
	unsigned int reg_me1_gmvd_sel;

	unsigned int reg_me1_gmvd_ucf_x1;
	unsigned int reg_me1_gmvd_ucf_x2;
	unsigned int reg_me1_gmvd_ucf_x3;
	unsigned int reg_me1_gmvd_ucf_y1;
	unsigned int reg_me1_gmvd_ucf_y2;
	unsigned int reg_me1_gmvd_ucf_y3;
	unsigned int reg_me1_gmvd_ucf_slope1;
	unsigned int reg_me1_gmvd_ucf_slope2;

	unsigned int reg_me1_gmvd_cnt_x1;
	unsigned int reg_me1_gmvd_cnt_x2;
	unsigned int reg_me1_gmvd_cnt_x3;
	unsigned int reg_me1_gmvd_cnt_y1;
	unsigned int reg_me1_gmvd_cnt_y2;
	unsigned int reg_me1_gmvd_cnt_y3;
	unsigned int reg_me1_gmvd_cnt_slope1;
	unsigned int reg_me1_gmvd_cnt_slope2;

	unsigned int reg_me1_pi_gmvd_cost_limt;
	unsigned int reg_me1_ip_gmvd_cost_limt;
	unsigned int reg_me2_ph_gmvd_cost_limt;

	unsigned int u25_rdbk_dtl_all;
	unsigned int u25_rdbk_dtl_all_pre;
	unsigned int u25_rdbk_dtl_all_intervel5;
	unsigned int u25_rdbk_dtl_all_intervel5_pre;

	unsigned int reg_mc_scene_change_fb;
	unsigned int reg_me1_sc_saddiff_th;

	unsigned int   u8_dynME_acdc_bld_x_act[3];
	unsigned int   u6_dynME_acdc_bld_y_act[3];
	int  s9_dynME_acdc_bld_s_act[2];
	unsigned int   u8_dynME_ac_pix_sad_limt_act;
	unsigned int   u4_acdc_bld_psad_shift_act;

	unsigned int   u8_dynME2_acdc_bld_x_act[3];
	unsigned int   u6_dynME2_acdc_bld_y_act[3];
	int  s9_dynME2_acdc_bld_s_act[2];

	unsigned int reg_kmc_blend_y_alpha;
	unsigned int reg_kmc_blend_uv_alpha;
	unsigned int reg_kmc_blend_logo_y_alpha;
	unsigned int reg_kmc_blend_logo_uv_alpha;
	//============  me1 drp  k5lp===
	unsigned int reg_me1_drp_cuv_y0   ;
	unsigned int reg_me1_drp_cuv_y1   ;
	unsigned int reg_me1_drp_cuv_y2   ;

	//============  local fb===
	unsigned char reg_dh_dtl_curve_x1;
	unsigned char reg_dh_dtl_curve_x2;	
	unsigned char reg_dh_dtl_curve_y1;
	unsigned char reg_dh_dtl_curve_y2;
	unsigned short reg_dh_fblvl_mvdiff_x1;
	unsigned short reg_dh_fblvl_mvdiff_x2;
	unsigned short reg_dh_fblvl_mvdiff_x3;
	unsigned short reg_dh_fblvl_mvdiff_y1;
	unsigned short reg_dh_fblvl_mvdiff_y2;
	unsigned short reg_dh_fblvl_mvdiff_y3;
	//==============

	//============  sobj ===
	unsigned char reg_mc_sobj_ymin0;
	unsigned char reg_mc_sobj_ymin1;
	unsigned char reg_mc_sobj_slop1;
	//======================
	//============me2  sobj ===
	unsigned int reg_me2_sec_small_object_sad_th;
	unsigned int reg_me2_sec_small_object_sm_mv_th;
	unsigned int reg_me2_sec_small_object_ip_pi_mvdiff_gain;
	unsigned int reg_me2_sec_small_object_ph_mvdiff_gain;
	//======================

	//============  lfsr_mask ===
	unsigned char reg_vbuf_ip_1st_lfsr_mask0_x ;
	unsigned char reg_vbuf_ip_1st_lfsr_mask0_y ;
	unsigned char reg_vbuf_ip_1st_lfsr_mask1_x ;
	unsigned char reg_vbuf_ip_1st_lfsr_mask1_y ;
	unsigned char reg_vbuf_ip_1st_lfsr_mask2_x ;
	unsigned char reg_vbuf_ip_1st_lfsr_mask2_y ;
	unsigned char reg_vbuf_ip_1st_lfsr_mask3_x ;
	unsigned char reg_vbuf_ip_1st_lfsr_mask3_y ;

	unsigned char reg_vbuf_pi_1st_lfsr_mask0_x ;
	unsigned char reg_vbuf_pi_1st_lfsr_mask0_y ;
	unsigned char reg_vbuf_pi_1st_lfsr_mask1_x ;
	unsigned char reg_vbuf_pi_1st_lfsr_mask1_y ;
	unsigned char reg_vbuf_pi_1st_lfsr_mask2_x ;
	unsigned char reg_vbuf_pi_1st_lfsr_mask2_y ;
	unsigned char reg_vbuf_pi_1st_lfsr_mask3_x ;
	unsigned char reg_vbuf_pi_1st_lfsr_mask3_y ;
	//======================

	//============  gmv control ===
	unsigned char reg_vbuf_ip_1st_gmv_en;
	unsigned char reg_vbuf_pi_1st_gmv_en;
	//======================

	//============  zmv control ===
	unsigned char reg_vbuf_ip_1st_zmv_en;
	unsigned char reg_vbuf_pi_1st_zmv_en;
	unsigned int reg_me1_ip_cddpnt_zmv;
	unsigned int reg_me1_pi_cddpnt_zmv;
	//======================

	//=======================
	unsigned int reg_vbuf_invalid_cnt_th;
	//=======================

	//=========CRTC=========
	unsigned int reg_crtc1_frc_vtrig_dly;
	unsigned int reg_crtc1_mc_vtrig_dly_long;
	unsigned int reg_crtc1_lbmc_vtrig_dly;
	//======================

	unsigned int reg_vbuf_ip_dc_obme_mode_sel;
	unsigned int reg_vbuf_pi_dc_obme_mode_sel;
	unsigned int reg_vbuf_ip_dc_obme_mode;
	unsigned int reg_vbuf_pi_dc_obme_mode;
	unsigned int reg_dehalo2_90;
	unsigned int reg_dehalo2_A8;
	unsigned int reg_dehalo2_E8;
	unsigned int reg_dehalo2_14;
	unsigned int reg_dehalo2_18;
	unsigned int reg_dehalo2_1C;
	unsigned int reg_dehalo2_20;
	unsigned int reg_dehalo2_24;
	unsigned int reg_dehalo2_28;	
}MemcStatus;

typedef struct _ScalerMEMCGlobalStatus
{
	MemcStatus MEMC_STATUS;

	unsigned int Frame_Couter;

} ScalerMEMCGlobalStatus;

typedef struct
{
	ScalerMEMCGlobalStatus SIS_MEMC;

	unsigned char u1_RP_detect_true;
	unsigned char u1_Region_Periodic_detect_true;
	unsigned char u1_casino_RP_detect_true;
	unsigned char u1_Avenger_RP_detect_true;
	unsigned char u1_Natural_HighFreq_detect_true;
	unsigned int u32_new_saddiff_th;
	unsigned int u32_new_saddiff_th_pre;
	int   s10_me_GMV_1st_vy_rb_iir;
	unsigned char  u6_me_GMV_1st_cnt_rb_iir;
	unsigned int u12_me_GMV_1st_unconf_rb_iir;
	unsigned char  u1_chaos_true;
	unsigned char  u8_chaos_apply;
	unsigned char   u8_big_apldiff_hold_cnt;
	unsigned char  u1_ToL10_RP_true;
	unsigned int  u32_glb_sad_pre;
	unsigned int	u32_big_apldiff_saddiff_th;
	unsigned char	u8_big_apldiff_sc_hold_cnt;
	unsigned int	u32_big_apldiff_sad_pre;
	unsigned char	u8_EPG_apply;
	unsigned char	u8_EPG_moveROI;
	unsigned int	u16_simple_scene_score_pre;

	//panning detect
	unsigned char	u2_panning_flag;
	
	// general motion dir
	unsigned char	u2_GMV_single_move;
	// general detail dir
	unsigned char	u2_mcDtl_HV;

	// pure vertical panning
	unsigned char   u1_pure_TopDownPan_flag;
	unsigned char   u8_pure_TopDownPan_cnt;

	// boundary vertical panning
	unsigned char	u1_boundary_v_panning_flag;
	unsigned char	u8_boundary_v_panning_cnt;

	// boundary horizontal panning
	unsigned char	u1_boundary_h_panning_flag;
	unsigned char	u8_boundary_h_panning_cnt;

	unsigned char u1_RP_DTLPRD_move_check;

	unsigned char u3_Dynamic_MVx_Step;
	unsigned char u3_Dynamic_MVy_Step;

	unsigned char u1_MainObject_Move;

	// over search range
	unsigned char u1_over_searchrange;

	// median filter
	bool  b_do_ME_medFLT;
	bool  b_big_relMV;
	bool  b_Chaos;
	bool  b_TCss;
	bool  b_Prd;
	bool  b_prd_scene;
	bool  b_logo_scene;
	bool  b_g_panning;
	bool  b_2grp_GMV;
	bool  b_DRP_special_scene_check;
	unsigned char u8_big_relMV_cnt;
	unsigned char u8_rChaos_cnt;
	unsigned char u8_rTCss_cnt;
	unsigned char u8_rPrd_cnt;
	unsigned char u8_panning_dbCnt;
	unsigned char u8_prd_dbCnt;	
	unsigned char u8_big_relMV_dbCnt;
	unsigned char u8_rChaos_dbCnt;

	unsigned char u3_occl_ext_level;
	unsigned char u3_occl_post_corr_level;

	unsigned int u32_aFrmMot_4Max;
	unsigned int u32_cFrmMot_4Max;
	unsigned char u1_Q13835_flag;
	unsigned char u1_RP_035_flag;
	unsigned char u1_RP_053_flag;
	unsigned char u1_RP_062_1_flag;
	unsigned char u1_RP_062_2_flag;
	unsigned char u1_RP_095_flag;
	unsigned char u1_RP_ID_flag;
	unsigned char u1_zmv_on_flag;
	unsigned int u32_RP_ID_Show;
	unsigned char u1_BTS_DRP_detect_true;
	unsigned char u1_SAN_ANDREAS_DRP_detect_true;
	unsigned char u1_Adele_DRP_detect_true;
	unsigned char u8_ES_rChaos_cnt_tmp[32];
	bool    b_ES_Chaos_tmp;
	unsigned char mvDiff_tmp[32];
	unsigned char big_relMV_cnt_tmp[32];

	// adaptive median filter
	bool b_do_adpt_med9flt;
	bool b_do_pfvd;

	//gmv ctrl
	int s1_gmv_calc_mvx;
	int s1_gmv_calc_mvy;

	unsigned char u1_vpan_for_nearrim;
	unsigned char u1_hpan_for_nearrim;

	bool u1_still_highfrequency_image_detect;
	unsigned char u1_Brightness_Chg_Cnt;
	unsigned char u8_RepeatMode;
	//unsigned char u8_RepeatCase;
	bool u1_SportScene;

	// identify pattern
	bool u1_tool_detect_pattern;
	bool u1_bin_detect_pattern;

	// mean absolute deviation 
	bool u1_uniform_panning;
	unsigned char u2_uniform_panning_direction;
	unsigned char u8_MAD_x;
	unsigned char u8_MAD_y;

	unsigned char u2_uniform_panning;

	signed short s11_avg_rmv_x;
	signed short s10_avg_rmv_y;
	unsigned short u11_max_abs_rmv_x;
	unsigned short u10_max_abs_rmv_y;

	// detect fast motion logo
	bool u1_fast_motion_logo;

	unsigned char u1_CMR_pattern;
	unsigned char u1_CMR_pattern_apply;
	unsigned char u2_strong_pure_panning_flag;
	unsigned char u8_Panning_Adjust_obme;//07.15.10.Star Wars- The Force Awakens_20.mp4

	// detect 6008_20fps for cad 2332
	bool u1_detect_6008;

	// detect horizontaly movin golf(0011_HDHR_24fps)
	bool u1_detect_HDHR_Golf;

    bool u1_table_v_en;
    unsigned char u8_me1_max_dtl;
    bool u1_local_deflicker_en;
    bool u1_single_side_byphase_en;
    unsigned char u8_v_lpf_coef[33][4];
    bool u1_wrt_byphase_en;
    bool u1_wrt_poly_en;
	bool u1_wrt_local_lpf_en;
//    bool u1_wrt_local_lpf_en;
    unsigned char u8_mc_local_lpf_coef[2];
    unsigned char u8_lbmc_local_lpf_y3;
    unsigned char u8_lbmc_local_lpf_y1;
    unsigned char u8_lbmc_local_lpf_y2;
    unsigned char u8_lbmc_local_lpf_slope0;
    unsigned char u8_lbmc_local_lpf_slope1;
    unsigned char u8_lbmc_local_lpf_slope2;
    unsigned char u8_mc_local_lpf_cnt_thr;
	unsigned char u8_lbmc_local_lpf_x1;
	unsigned char u8_lbmc_local_lpf_x2;
	unsigned char u8_lbmc_local_lpf_x3;

	unsigned short u11_lt_1st_max_rmv;
	unsigned short u11_rt_1st_max_rmv;
	unsigned short u11_lb_1st_max_rmv;
	unsigned short u11_rb_1st_max_rmv;
	unsigned short u11_lt_2nd_max_rmv;
	unsigned short u11_rt_2nd_max_rmv;
	unsigned short u11_lb_2nd_max_rmv;
	unsigned short u11_rb_2nd_max_rmv;
	unsigned short u11_max_1st_rmv_with_logo;
	unsigned short u11_max_2nd_rmv_with_logo;

	//rotate windmill
	bool u1_Rotate_Windmill_en;
	unsigned char u2_Rotate_Windmill_direction;

	//cross motion	
	unsigned char u5_cross_obj_num;//num: 0~15

	//moving text flg for #036
	bool u1_MovingText_flg;

	//YUV hist for #405
	unsigned int u25_yuv_hist[32];

	//info for MEMC_Repeat_Periodic_detect
	unsigned short u12_max_rmv_diff;
	unsigned char u8_Large_Mot_cnt;

	// detect kim yuna
	bool u1_detect_kimyuna;

		//himedia player menu
	bool u1_detect_player_menu;
	bool u1_rim_match;
	bool u1_apl_match;
	bool u1_mv_match;
	bool u1_dtl_match;
	unsigned char u8_player_menu_match_cnt;
	unsigned char u8_CarLogo_repeat_broken;
}_OUTPUT_ME_SCENE_ANALYSIS;

typedef struct
{
	unsigned short Rim_Info[_RIM_NUM];
	unsigned int DTL_min;
	unsigned int DTL_max;
	unsigned int APL_min;
	unsigned int APL_max;
	bool u1_HSR;
}_Player_Menu_Info;

typedef enum
{
	Pan_Dir_None,
	Pan_Dir_X,
	Pan_Dir_Y,
}_MEMC_Panning_Direction;

typedef enum
{
	MEMC_Uniform_Panning_None,
	MEMC_Uniform_Panning_Low,
	MEMC_Uniform_Panning_Mid,
	MEMC_Uniform_Panning_High,
	MEMC_Uniform_Panning_Num,
}_MEMC_Uniform_Panning_lvl;

typedef enum {
	LOGO_CORNER_LU = 0,	// left up corner
	LOGO_CORNER_RU,		// right up corner
	LOGO_CORNER_LD,		// left down corner
	LOGO_CORNER_RD,		// right down corner
	LOGO_CORNER_NUM,
}_LOGO_CORNER_INDEX;
	
typedef enum {
	RepeatMode_OFF = 0,
	RepeatMode_LOW,
	RepeatMode_MID,
	RepeatMode_HIGH,
	RepeatMode_BIGSAD,
	RepeatMode_SC,
}_REPEAT_MODE_LIST;



#if 1//RTK_MEMC_Performance_tunging_from_tv001
typedef enum {
	RepeatCase_None=0,
	RepeatCase_DetRpPanAndSlowMV=1,
	RepeatCase_DetRpAndSlowMV=2,
	RepeatCase_PanAndSlowMV=3,
	RepeatCase_VerySlowMV=4,
	RepeatCase_SlowMV=5,
	RepeatCase_Casino=6,
	RepeatCase_HoldOn=7,
	RepeatCase_HoldOn_SlowMV=8,
	RepeatCase_BIGSAD=9,
	RepeatCase_LargeMotion=0xb,
	RepeatCase_FastBoundaryMV=0xc,
	RepeatCase_FastMotion=0xd,
	RepeatCase_Sport=0xe,
	RepeatCase_SC=0xf,
} RepeatCase;


#else
typedef enum {
	RepeatCase_None = 0,
	// set repeat mode mid
	RepeatCase_DetRpPanAndSlowMV = 1,
	RepeatCase_DetRpAndSlowMV = 2,
	RepeatCase_PanAndSlowMV = 3,
	RepeatCase_VerySlowMV = 4,
	RepeatCase_SlowMV = 5,
	RepeatCase_Casino = 6,
	RepeatCase_HoldOn_SlowMV = 7,
	RepeatCase_UniformPan = 8,
	// set repeat mode low
	RepeatCase_HoldOn_NormalMV = 9,
	// set repeat mode off
	RepeatCase_LowCnt = 0xa,
	RepeatCase_SmallDtl = 0xb,
	RepeatCase_HighFreq = 0xc,
	RepeatCase_HighFreq_WithMV = 0xd,
	RepeatCase_Sport = 0xe,
	RepeatCase_SC = 0xf,
}_REPEAT_CASE_LIST;

#endif

VOID Me_sceneAnalysis_Init(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID Me_sceneAnalysis_Proc_OutputIntp(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput, int			 iSliceIdx);

VOID MEMC_MiddleWare_StatusInit(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_MiddleWare_CRTC_StatusInit(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_MiddleWare_Reg_Status_Cad1(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_MiddleWare_StatusUpdate(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput, int			 iSliceIdx);

VOID MEMC_LightSwordDetect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_RepeatPattern_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Natural_HighFreq_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Casino_RP_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
unsigned char MEMC_RepeatBG_detect(VOID);
unsigned char MEMC_BlackSC_detect(VOID);
unsigned char MEMC_RMV_Consistent_detect(VOID);
unsigned char MEMC_GMV_detect(unsigned char * u11_GMV_x_max_rb,unsigned char *u10_GMV_y_max_rb);
unsigned char MEMC_RGNDtlPRD_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
unsigned char MEMC_RGNDtl_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
unsigned char MEMC_RGNPRD_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
unsigned char MEMC_HVDtl_Move_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Periodic_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Region_Periodic_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Panning_Detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_MotionInfo_SLD(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Big_APLDiff_Detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_EPG_Protect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Simple_Scene_Detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_PureTopDownPanning_Detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_BoundaryHPanning_Detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_DynamicSearchRange_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_MainObject_Move_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Over_SearchRange_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Relative_Motion_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);

VOID MEMC_Occl_Ext_Detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_ME_OFFSET_Proc(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_RMV_Detect_DTV_Ch2_1_tv006(_OUTPUT_ME_SCENE_ANALYSIS *pOutput); 
VOID MEMC_Motion_Info_Detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput); 
VOID MEMC_boundary_mv_convergence_Detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_special_DRP_Detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);

VOID MEMC_adpt_med9flt_pfvd_condition(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_NearRim_Vpan_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Brightness_Chg_Detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);

VOID MEMC_info_Print(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Large_Motion_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam,_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_RMV_Quick_Change_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam,_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Repeat_Periodic_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam,_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_MAD_Rmv_detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Still_Highfrequency_Image_Detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_MAD_detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);


#ifdef BG_MV_GET
VOID MEMC_BG_MV_Get(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
#endif
VOID MEMC_Local_Flicker_detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
int SignedVal_To_UnsignedVal_Shift(int input_val);

void MEMC_KimYuna_Fast_logo_detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
void MEMC_Fast_Motion_Logo_Detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Sport_Scene_Detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_CMR_pattern_protect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Small_Motion_Detect_K24987(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Corner_Speed_Detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Cross_Motion_Detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
void MEMC_Movig_Text_Detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
void MEMC_YUV_Hist_Detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Horizontal_Moving_Golf(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Rotate_Windmill_Detect(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Panning_Adjust_obme(const _PARAM_ME_SCENE_ANALYSIS *pParam, _OUTPUT_ME_SCENE_ANALYSIS *pOutput);
VOID MEMC_Player_Menu_Detect(_OUTPUT_ME_SCENE_ANALYSIS *pOutput);

/////////////////////////////////////////////////////////////////////////////

DEBUGMODULE_BEGIN(_PARAM_ME_SCENE_ANALYSIS,_OUTPUT_ME_SCENE_ANALYSIS)
 //   ADD_DUMMY_PARAM()
	ADD_PARAM_DES(unsigned char,  u1_RepeatPattern_en,         1, "")
	ADD_PARAM_DES(unsigned char,  u1_Natural_HighFreq_en,         1, "")
	ADD_PARAM_DES(unsigned char,  u1_LightSword_en,         1, "")
	ADD_PARAM_DES(unsigned char,  u1_BigAPLDiff_en,		 1, "")
DEBUGMODULE_END

#endif


