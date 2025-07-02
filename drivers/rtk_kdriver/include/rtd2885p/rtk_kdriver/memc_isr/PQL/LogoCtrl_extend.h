#ifndef _FRC_LOGOCTRL_H_
#define _FRC_LOGOCTRL_H_

#define LOGO_DDR_W (480)
#define LOGO_DDR_H (270)
#define LOGO_DDR_ROUGH_W (60) // 480/4
#define LOGO_DDR_ROUGH_H (34)  // 270/4

#define BLK_LOGO_CNT    (960*540)

#define LOGO_DEBUG_PRINTF   (0)

typedef enum {
	LOGO_CORNER_LU = 0,	// left up corner
	LOGO_CORNER_RU,		// right up corner
	LOGO_CORNER_LD,		// left down corner
	LOGO_CORNER_RD,		// right down corner
	LOGO_CORNER_NUM,
}_MEMC_LOGO_CORNER_INDEX;

typedef enum {
	LOGO_LEFT_UP = 0,
	LOGO_RIGHT_UP,
	LOGO_LEFT_LOW,
	LOGO_RIGHT_LOW,
//	LOGO_CENTER_LOW,
	LOGO_POSITION_NUM,
} MEMC_LOGO_POSITION;

typedef enum {
	CORNER_LOGO_LEFT_UP = 0,
	CORNER_LOGO_RIGHT_UP,
	CORNER_LOGO_LEFT_LOW,
	CORNER_LOGO_RIGHT_LOW,
	CORNER_LOGO_POSITION_NUM,
} MEMC_CORNER_LOGO_POSITION;

typedef enum {
	LOGO_UP = 0,
	LOGO_LEFT,
	LOGO_LOW,
	LOGO_RIGHT,
	LOGO_FINE_TUNE_POSITION_NUM,
} MEMC_LOGO_FINE_TUNE_POSITION;

typedef struct
{
	unsigned short u16_start_x;
	unsigned short u16_end_x;
	unsigned short u16_start_y;
	unsigned short u16_end_y;
} MEMC_Logo_Boundary;

typedef struct
{
	unsigned short u16_start_x;
	unsigned short u16_end_x;
	unsigned short u16_start_y;
	unsigned short u16_end_y;
	unsigned short u16_logo_cnt;
	bool u1_vaild;
	bool u1_stable;
	bool u1_get_logo;
} MEMC_Logo_Boundary_Info;


/* Param Definition*/
typedef struct
{
    // Logo Registers
    unsigned int blkhsty_pth[32];
    unsigned int blkhsty_nth[32];
    unsigned int blksamethr_l[32];
    unsigned int blksamethr_a[32];
    unsigned int blksamethr_h[32];
    unsigned int reg_mc_logo_vlpf_en;
    unsigned int reg_km_logo_iir_alpha;
    unsigned int reg_mc_logo_en;
} _MEMC_LOGO_DEFAULT_VAL_M8P;

typedef struct
{
    unsigned char  u1_logo_pql_en;
    unsigned char  u1_logo_sw_clr_en;
    unsigned char  u1_logo_rg_clr_en;
    unsigned char  u1_logo_glb_clr_en;
    unsigned char  u1_logo_sc_detect_en;

    unsigned char  u1_logo_longterm_logo_en;
    unsigned char  u1_logo_dehalo_logo_en;
    unsigned char  u1_logo_dehalo_logo_mode;
    unsigned char  u1_logo_dehalo_logo_boundary_en;
    unsigned char  u1_logo_counter_en;
    
}_PARAM_LOGO_CTRL;

/* Output Definition*/
//typedef struct
//{
//    unsigned int u14_pixlogo_rgcnt[RG_32];
//    unsigned int u20_pixlogo_rgaccY[RG_32];
//}_OUTPUT_FRC_LGDet_RB;
//
//typedef struct
//{
//    unsigned short u14_pixlogo_rgcnt[RG_32];
//    unsigned int u20_pixlogo_rgY[RG_32];
//    unsigned int u32_frm_cnt;
//}_OUTPUT_FRC_LGDet_DynSet;
//
//
////total
//typedef struct
//{
//    _OUTPUT_FRC_LGDet_RB       sLogo_rb;
//    _OUTPUT_FRC_LGDet_DynSet   sLogo_dyn_set;
//}_OUTPUT_FRC_LGDet;


// struct for logo bounding box

typedef struct
{
	int start_x;
	int end_x;
	int start_y;
	int end_y;
} MEMC_Logo_Boundary_M8P;

typedef struct
{
	int start_x;
	int end_x;
	int start_y;
	int end_y;
	int logo_cnt;
	bool valid;
	bool stable;
	int stable_cnt;
	int logo_cnt_diff;
	int logo_cnt_prv;
	int logo_area_prv;
	bool get_logo;	
	int logo_long_term;
	
	bool bounding_box_rmv_cond;
	int prv_start_x;
	int prv_end_x;
	int prv_start_y;
	int prv_end_y;

    int width_height_ratio;
    int logo_cnt_area_ratio;

	int width;
	int height;
	int area;

    int rmv_x;
    int rmv_y;

    bool final_valid;
}MEMC_Logo_Boundary_Info_M8P;


// struct for logo bounding box







// struct for logo bounding box


typedef struct
{
	////////// Merlin8 logo ddr //////////
	MEMC_Logo_Boundary_Info DDR_Logo_raw[LOGO_POSITION_NUM];
	MEMC_Logo_Boundary_Info DDR_Logo_cur[LOGO_POSITION_NUM];
	MEMC_Logo_Boundary_Info DDR_Logo_replacemv[LOGO_POSITION_NUM];
	MEMC_Logo_Boundary_Info DDR_Logo_dhlogo[LOGO_POSITION_NUM];
	MEMC_Logo_Boundary_Info DDR_Logo_target[LOGO_POSITION_NUM];
	unsigned int Area_pre[LOGO_POSITION_NUM];
	unsigned char target_stable_cnt[LOGO_POSITION_NUM];
	bool u1_apply_replacemv[4];
	MEMC_Logo_Boundary_Info DDR_Logo_IIR[LOGO_POSITION_NUM];
//	MEMC_Logo_Boundary_Info DDR_Logo_hold_boundary[LOGO_POSITION_NUM];
//	unsigned char DDR_Logo_hold_cnt[LOGO_POSITION_NUM];
//	MEMC_Logo_Boundary FineTuneInfo[LOGO_POSITION_NUM][LOGO_FINE_TUNE_POSITION_NUM];

	////////// Merlin8p //////////
    _MEMC_LOGO_DEFAULT_VAL_M8P DefaultVal;

    unsigned int  u32_blklogo_clr_idx;

    unsigned int u8_logo_sc_hold_frm;
    unsigned char  u1_logo_sc_status;

    unsigned int u32_sobel_logo_clear_flag;


    unsigned int long_term_logo_counter[32];


    // logo detection temporal counter flag
    bool zmv_cnt_sum_cond;
    bool small_gmv_cond;
    bool hammock_condition;
    bool input_frame_cond;
   

    // Dehalo Logo Registers
    
    // cond1, cond2 registers
    unsigned int dh_logo_mv_th[32];
    unsigned int dh_logo_sad_mvd_th[32];
    unsigned int dh_logo_sad_ofs[32];

    // cond3, cond4 registers
    unsigned int dh_logo_max_mv_th;
    unsigned int dh_logo_zmv_cnt_th[32];
    unsigned int dh_logo_mvd_3x3_cnt_th[32];
    unsigned int dh_logo_mvd_3x3_th[32];
    unsigned int dh_logo_large_mv_cnt_th[32];
    unsigned int dh_logo_zmv_cnt_diff_th[32];
    unsigned int dh_logo_var_cnt_th[32];

    // dehalo logo boundary registers
    unsigned int dh_logo_boundary_x[7];
    unsigned int dh_logo_boundary_y[3];
}_OUTPUT_LOGO_CTRL;

/* Function Declaration*/

////////// Merlin8 //////////
void Logo_DDR_Set_update_flag_RTK2885p(bool flag_en);
void Logo_DDR_init_RTK2885p(void);
void Logo_DDR_Proc_RTK2885p(_OUTPUT_LOGO_CTRL *pOutput);

////////// Merlin8p //////////
VOID LogoCtrl_Init_RTK2885pp(_OUTPUT_LOGO_CTRL *pOutput);
VOID LogoCtrl_Proc_RTK2885pp(const _PARAM_LOGO_CTRL *pParam, _OUTPUT_LOGO_CTRL *pOutput);

#endif
