#ifndef _MEMC_PARAMTABLE_H
#define _MEMC_PARAMTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#define u32IPME1_Golden_HSize		(960)
#define u32IPME1_Golden_VSize		(540)
#define u32PLogo_Golden_HSize		(960)
#define u32PLogo_Golden_VSize		(540)
#define u32BLogo_Golden_BlkNum		(240)
#define u32BLogo_Golden_RowNum		(135)
#define u32ME2_Golden_BlockNum		(480)
#define u32ME2_Golden_RowNum		(270)

typedef struct
{
	unsigned int dh_logo_rgn_v0;
	unsigned int dh_logo_rgn_v1;
	unsigned int dh_logo_rgn_v2;
	unsigned int dh_logo_rgn_v3;
	unsigned int dh_logo_rgn_h0;
	unsigned int dh_logo_rgn_h1;
	unsigned int dh_logo_rgn_h2;
	unsigned int dh_logo_rgn_h3;
	unsigned int dh_logo_rgn_h4;
	unsigned int dh_logo_rgn_h5;
}_Param_Dehalo_Logo_Region;

typedef enum {
	LOGO_DETECT_EDGE_HORIZONTAL = 0,
	LOGO_DETECT_EDGE_P45,
	LOGO_DETECT_EDGE_VERTICAL,
	LOGO_DETECT_EDGE_N45,
	LOGO_DETECT_EDGE_NUM,
}_LOGO_DETECT_EDGE_TABLE;

typedef enum {
	SEARCH_RANGE_TYPE_OVER_SEARCH_RANGE = 0,
	SEARCH_RANGE_TYPE_NORMAL,
	SEARCH_RANGE_TYPE_NUM,
}_SEARCH_RANGE_TYPE_TABLE;

typedef enum {
	FB_STYLE_NORMAL,
	FB_STYLE_MORE,
	FB_STYLE_LESS,
	FB_STYLE_CINEMA,
	FB_STYLE_NUM,
}_FB_STYLE_TABLE;

typedef enum {
	ME_STYLE_NORMAL,
	ME_STYLE_SLOW,
	ME_STYLE_NUM,
}_ME_STYLE_TABLE;

#define FB_STYLE_DEFAULT_4k1k (FB_STYLE_MORE)
#define ME_STYLE_DEFAULT_4k1k (ME_STYLE_SLOW)

typedef struct
{
	unsigned char u8_logo_blkgrdhor_th;
	unsigned char u8_logo_blkgrdp45_th;
	unsigned char u8_logo_blkgrdver_th;
	unsigned char u8_logo_blkgrdn45_th;
}_LOGO_DETECT_EDGE_Param;

typedef struct
{
	bool u1_mc_fblvl_filter_en;
	unsigned char u2_mc_fblvl_filter_mode;
	unsigned char u8_mc_fblvl_filter_cut;
	unsigned char u8_mc_fblvl_filter_th;
	unsigned char u8_mc_fblvl_filter_avg_th;
	unsigned char u6_mc_fblvl_filter_num; 
}_MC_FBLVL_FILTER_PARAM;

typedef struct
{
	unsigned char u4_logo_erosion_th_panning;
	unsigned char u4_logo_erosion_th_fastmotion;
	unsigned char u4_logo_erosion_th_normal;
}_MC_LOGO_EROSION_TH_Param;

typedef struct
{
	unsigned int u32_tuning_threshold_PureVideo;
	unsigned int u32_tuning_threshold_Panning;
}_SC_TUNING_TH_PARAM;

typedef struct
{
	unsigned int u8_Hist_Diff_Ratio;
}_YHistBigChg_PARAM;

typedef struct
{
	unsigned char ratio_th_l;
	unsigned char ratio_th_h;
}_SAD_DIFF_RMV_RATIO_TH;

typedef struct
{
	unsigned char u7_ME1_Y;
	unsigned char u7_ME2_Y;
}_SEARCH_RANGE_CLIP_PARAM;

typedef struct
{
	unsigned int u32_FB_bad_reg_low_th;
	unsigned int u32_FB_bad_reg_mid_th;
	unsigned int u32_FB_bad_reg_high_th;
}_FB_BAD_REGION_TH_PARAM;

typedef struct
{
	unsigned int u27_Fb_tc_th_low;
	unsigned int u27_Fb_tc_th_high;
}_FB_TC_TH_PARAM;

typedef struct
{
	unsigned short u12_bbd_h_precise_active_th;
	unsigned short u12_bbd_h_sketchy_active_th;
	unsigned short u13_bbd_v_precise_active_th;
	unsigned short u13_bbd_v_sketchy_active_th;
}_BBD_ACTIVE_TH_Param;

typedef struct
{
	unsigned short s16_resolution_width;
	unsigned short s16_resolution_height;
	unsigned short s16_rim_bound[4];//_RIM_NUM
	unsigned char u8_H_Divide;
	unsigned char u8_V_Divide;
	unsigned char u8_SlowIn_MinShiftX;
	unsigned char u8_SlowIn_MinShiftY;
	unsigned char u8_RimDiffTh_cof;
	unsigned int u32_apl_size;
	unsigned char u8_scale_H;
	unsigned char u8_scale_V;
}_RimCtrl_Param;

typedef struct
{
	unsigned char u8_top_rim;
	unsigned int u32_sad_offset;
}_SAD_SHIFT_PARAM;

typedef struct
{
	_RimCtrl_Param RimCtrl_Param;
	_BBD_ACTIVE_TH_Param BBD_ACTIVE_TH_Param;
}_RIM_Param;

typedef struct
{
	_LOGO_DETECT_EDGE_Param LOGO_DETECT_EDGE_Param;
	_MC_LOGO_EROSION_TH_Param MC_LOGO_EROSION_TH_Param;
}_LOGO_Param;

typedef struct
{
	_FB_BAD_REGION_TH_PARAM FB_BAD_REGION_TH_PARAM;
	_FB_TC_TH_PARAM FB_TC_TH_PARAM;
	_MC_FBLVL_FILTER_PARAM MC_FBLVL_FILTER_PARAM;
	unsigned char u8_TempConsis_shift;
}_FB_Param;

typedef struct
{
	_SAD_DIFF_RMV_RATIO_TH SAD_DIFF_RMV_RATIO;
	_SC_TUNING_TH_PARAM SC_TUNING_TH_PARAM;
	_YHistBigChg_PARAM YHistBigChg_PARAM;
}_SC_Param;

typedef struct
{
	_SEARCH_RANGE_CLIP_PARAM SEARCH_RANGE_CLIP_PARAM[SEARCH_RANGE_TYPE_NUM];
	char random_mask_offset;
}_MV_SEARCHRANGE_Param;

typedef struct
{
	unsigned char u2_mc_vartap_sel; // 0:9tap, 1:17tap, 2:33tap
	unsigned short u11_knr_half_vactive_table;
}_MC_Param;

typedef struct
{
	unsigned char u2_dh_phflt_med9flt_data_sel;
}_DEHALO_PARAM;

typedef struct
{
	unsigned char u2_bi_blk_res_sel; // 1:240x135, 0:480x270
}_BI_PARAM;

typedef struct
{
	bool u1_special_v_active;
	unsigned short v_active;
}_ACTIVE_PARAM;

typedef struct
{
	unsigned int u10_logn_term_cnt_th;
}_PERIODIC_PARAM;

typedef struct
{
	bool u1_GFB_MVChaos_total_en;
	bool u1_GFB_MVChaos_SC_en;
	bool u1_GFB_MVChaos_unconf_en;
	unsigned char u7_GFB_MVChaos_SC_th; //smaller value -> easy to apply GFB
	unsigned char u8_GFB_MVChaos_unconf_th; //smaller value -> easy to apply GFB
	
	bool u1_GFB_FastMotion_en;
	unsigned char u8_GFB_FastMotion_mv_th; //smaller value -> easy to apply GFB
	unsigned char u5_GFB_FastMotion_large_rmv_num; //smaller value -> easy to apply GFB

	bool u1_LFB_Dtl_curve_en;
	unsigned char u8_LFB_Dtl_strength_ratio; //0~255, dh_dtl_curve_y1~y2 //larger value -> easy to apply LFB
	
	bool u1_LFB_MV_diff_curve_en;
	unsigned char u8_LFB_MV_diff_strength_ratio; //0~255, dh_fblvl_mvdiff_y1~y3 //larger values -> easy to apply LFB

	unsigned short u10_LFB_dh_ph_fbgen_min; //0~1023 //larger value -> easy to apply LFB
	unsigned short u10_LFB_dh_ph_fbgen_max; //0~1023 //larger value -> easy to apply LFB
	unsigned char u8_LFB_dh_ph_fbgen_slp; //0~255 //larger value -> easy to apply LFB
	unsigned char u4_LFB_dh_ph_fbgen_sft; //0~15 //smaller value -> easy to apply LFB
	unsigned short u10_LFB_dh_ph_fbgen_th; //0~1023 //smaller value -> easy to apply LFB

	unsigned char u3_LFB_dh_fb_dtl_shift; //0~7 //smaller value -> easy to apply LFB
}_FB_STYLE_PARAM;

typedef struct
{
	bool u1_set_flp_alp;
	_str_ME_ipme_flp_alp_table flp_alp_table;

	bool u1_set_LFSR;
	_str_ME_Vbuf_LFSR_updStgh_table LFSR_table;

	bool u1_set_ME_cand;
	_str_ME_cand_table ME_cand_table;
}_ME_STYLE_PARAM;

typedef struct
{
	_SAD_SHIFT_PARAM SAD_SHIFT_Param[_PQL_OUT_RESOLUTION_MAX];
	_RIM_Param	RIM_Param[_PQL_OUT_RESOLUTION_MAX];
	_LOGO_Param LOGO_Param[_PQL_OUT_RESOLUTION_MAX];
	_FB_Param FB_Param[_PQL_OUT_RESOLUTION_MAX];
	_SC_Param SC_Param[_PQL_OUT_RESOLUTION_MAX];
	_MV_SEARCHRANGE_Param MV_SEARCHRANGE_Param[_PQL_OUT_RESOLUTION_MAX];
	_MC_Param MC_Param[_PQL_OUT_RESOLUTION_MAX];
	_DEHALO_PARAM DEHALO_PARAM[_PQL_OUT_RESOLUTION_MAX];
	_BI_PARAM BI_PARAM[_PQL_OUT_RESOLUTION_MAX];
	_ACTIVE_PARAM ACTIVE_PARAM[_PQL_OUT_RESOLUTION_MAX];
	_PERIODIC_PARAM PERIODIC_PARAM[_PQL_OUT_RESOLUTION_MAX];
}_MEMC_PARAM_TABLE;

typedef struct
{
	_FB_STYLE_PARAM FB_STYLE_PARAM[FB_STYLE_NUM];
	_ME_STYLE_PARAM ME_STYLE_PARAM[ME_STYLE_NUM];
}_MEMC_STYLE_TABLE;

#ifdef __cplusplus
}
#endif

#endif

