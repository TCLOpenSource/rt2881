#ifndef _LOGO_DDR_H_
#define _LOGO_DDR_H_
#if 0 //move to LogoCtrl.h
#define LOGO_DDR_W (480)
#define LOGO_DDR_H (270)
#define LOGO_DDR_ROUGH_W (60) // 480/4
#define LOGO_DDR_ROUGH_H (34)  // 270/4

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
} _OUTPUT_DDR_Logo;

void Logo_DDR_Set_update_flag(bool flag_en);
void Logo_DDR_init(void);
void Logo_DDR_Proc(_OUTPUT_DDR_Logo *pOutput);
#endif
#endif

