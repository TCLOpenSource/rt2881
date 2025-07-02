#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_isr/PQL/MEMC_ParamTable.h"
#include "memc_reg_def.h"
#include <rbus/color_mb_peaking_reg.h>
#include <rbus/color_sharp_reg.h>
#include <rbus/color_reg.h>
#include <tvscalercontrol/scaler/scalerstruct.h>
#include "memc_isr/include/memc_identify_ctrl.h"
#include "memc_isr/include/memc_identify_tv001.h"
#include "memc_isr/include/memc_identify_tv002.h"
#include "memc_isr/include/memc_identify_tv006.h"
#include "memc_isr/include/memc_identify_tv010.h"
#include "memc_isr/include/memc_identify_tv030.h"

#ifndef Start_Print_info_t
typedef struct
{
	// fwused_2
	/*[0:7]*/ unsigned char Delay_Time;
	/*[8 ] */ unsigned char Y_Hist;
	/*[9 ] */ unsigned char Hue_Hist;
	/*[10] */ unsigned char Sat_Mean;
	/*[11] */ unsigned char Dcc_1;
	/*[12] */ unsigned char Dcc_2;
	/*[13] */ unsigned char DCC_3;
	/*[14] */ unsigned char Rtnr_1;
	/*[15] */ unsigned char Rtnr_2;
	/*[16] */ unsigned char Mpeg_Nr;
	/*[17] */ unsigned char Snr;
	/*[18] */ unsigned char Sharpness_1;
	/*[19] */ unsigned char Sharpness_2;
	/*[20] */ unsigned char DI_1;
	/*[21] */ unsigned char DI_2;
	/*[22] */ unsigned char Film_Mode;
	/*[23] */ unsigned char HIST;
	/*[24] */ unsigned char Reverse_2;
	/*[25] */ unsigned char Reverse_3;
	/*[26] */ unsigned char FormattedLog;
	// fwused_3
	/*[0 ]*/ unsigned char ID_Pattern_ALL;
	/*[1 ]*/ unsigned char ID_Pattern_2;
	/*[2 ]*/ unsigned char ID_Pattern_4;
	/*[3 ]*/ unsigned char ID_Pattern_5;
	/*[4 ]*/ unsigned char ID_Pattern_7;
	/*[5 ]*/ unsigned char ID_Pattern_12;
	/*[6 ]*/ unsigned char ID_Pattern_23;
	/*[7 ]*/ unsigned char ID_Pattern_24;
	/*[8 ]*/ unsigned char ID_Pattern_35;
	/*[9 ]*/ unsigned char ID_Pattern_47;
	/*[10]*/ unsigned char ID_Pattern_107;
	/*[11]*/ unsigned char ID_Pattern_123;
	/*[12]*/ unsigned char ID_Pattern_132;
	/*[13]*/ unsigned char ID_Pattern_133;
	/*[14]*/ unsigned char ID_Pattern_154;
	/*[15]*/ unsigned char ID_Pattern_191;
	/*[16]*/ unsigned char ID_Pattern_192;
	/*[17]*/ unsigned char ID_Pattern_193;
	/*[18]*/ unsigned char ID_Pattern_194;
	/*[19]*/ unsigned char ID_Pattern_195;
	/*[20]*/ unsigned char ID_Pattern_139;
	/*[21]*/ unsigned char ID_Pattern_196;
	/*[22]*/ unsigned char ID_Pattern_14;
	/*[23]*/ unsigned char someothers;
	/*[24]*/ unsigned char ID_Pattern_32;
	/*[25]*/ unsigned char ID_Pattern_147;
	/*[26]*/ unsigned char Reverse_10;
	/*[27]*/ unsigned char Reverse_11;
	/*[28]*/ unsigned char VD_LOG_ENABLE;
	/*[29]*/ unsigned char ColorBar;
	/*[30]*/ unsigned char Slow_Motion1;
	/*[31]*/ unsigned char Slow_Motion2;
	/*[32]*/ unsigned char RF_flower;

	/*[0:9]*/ unsigned char YH_Show_Pattern_id;
	/*[10:14]*/ unsigned char YH_Show_Project_id;
	/*[15]*/ unsigned char YH_Show_en;
}Start_Print_info_t;
#endif //Start_Print_info_t

#define DATABASE_CONFIG			(1)
#define DETECT_SCENE_CONFIG		(1)
#define APPLY_SETTING_CONFIG	(1)

#define MAX_Y_THRESHOLD (1024)
#define MAX_HUE_THRESHOLD (1024)

extern Start_Print_info_t Start_Print;//for scalerDi.cpp ROSprintrf( %0) issue !!!
extern bool g_MEMC_Identify_Pattern_Clear;
extern unsigned char g_ProjectID;
extern unsigned short hue_hist_ratio[COLOR_HUE_HISTOGRAM_LEVEL];
extern unsigned short Y_hist_ratio[TV006_VPQ_chrm_bin];
extern unsigned int vpq_project_id;
extern unsigned int vpqex_project_id;
extern bool g_new_sport_mode;

extern unsigned char g_Q13722_flag;
extern unsigned char g_181_flag;
extern unsigned char g_393_flag;

extern unsigned short scalerVIP_ratio_inner_product(unsigned short *vector1, unsigned short *vector2, unsigned char length);
extern unsigned char Scaler_MEMC_Get_Performance_Checking_Database_index(void);
extern unsigned char MEMC_Lib_get_Adaptive_Stream_Flag(VOID);
extern _clues* scaler_GetShare_Memory_SmartPic_Clue_Struct(void);
extern void UnsignedValue_To_Char(unsigned int u32_Val, char* Ascii);
extern BOOL MEMC_Is_4k1k(void);

#define MEMC_PERFORMANCE_SETTING_TABLE_MAX (_MAX_(MEMC_PERFORMANCE_SETTING_TABLE_MAX_RTK2885P, MEMC_PERFORMANCE_SETTING_TABLE_MAX_RTK2885PP))

unsigned char g_show_delay_time = 10;
unsigned char DynamicOptimizeMEMC_HoldCnt[MEMC_PERFORMANCE_SETTING_TABLE_MAX];
bool DynamicOptimizeMEMC_RestoreEnable[MEMC_PERFORMANCE_SETTING_TABLE_MAX];
bool DynamicOptimizeMEMC_SpecialScene[MEMC_SPECIAL_SCENE_TABLE_MAX];
bool MEMC_Status_Table[MEMC_STATUS_TABLE_MAX];
unsigned char MEMC_Status_HoldCnt[MEMC_STATUS_TABLE_MAX];
SLR_VIP_TABLE_MEMC g_IDENTIFY_TABLE = {0};
bool g_Bin_Table_isValid[255] = {0};

unsigned short g_Sizeof_MEMC_setting_Table_RTK2885p = 0;
unsigned short g_Sizeof_MEMC_setting_Table_RTK2885pp = 0;

const unsigned int BIT_VAL[32] =
{
	0x1,
	0x3,
	0x7,
	0xF,
	0x1F,
	0x3F,
	0x7F,
	0xFF,
	0x1FF,
	0x3FF,
	0x7FF,
	0xFFF,
	0x1FFF,
	0x3FFF,
	0x7FFF,
	0xFFFF,
	0x1FFFF,
	0x3FFFF,
	0x7FFFF,
	0xFFFFF,
	0x1FFFFF,
	0x3FFFFF,
	0x7FFFFF,
	0xFFFFFF,
	0x1FFFFFF,
	0x3FFFFFF,
	0x7FFFFFF,
	0xFFFFFFF,
	0x1FFFFFFF,
	0x3FFFFFFF,
	0x7FFFFFFF,
	0xFFFFFFFF,
};

_IDENTIFY_SINGLE_PATTERN_INFO DynamicOptimizeMEMC_Info_Tool = {0};
BOOL MEMC_setting_Vaild[MEMC_PERFORMANCE_SETTING_TABLE_MAX] = {0};

const unsigned int MEMC_setting_Table_RTK2885p[/*MEMC_PERFORMANCE_SETTING_TABLE_MAX*/][MEMC_PERFORMANCE_SETTING_ITEM_MAX][MEMC_PERFORMANCE_SETTING_MAX] = 
{
	//#0
	//do nothing -> return all values
	{},

	//#1
	/*casino_royale_halo_1_09_00.mp4*/
	/*casino_royale_halo_1_09_00.mp4-one*/
	/*THE MATRIX RELOADED.4khdr_5_short.mp4*/
	/*25003_philips演示_25fps 1Min18s~1Min30S HSR*/
	/*30003_123_30fps.mpg HSR*/
	//RANDMASK_MOST_FAR
	{
		// set enum id
		{RANDMASK_MOST_FAR,0,0,0},
		
		// setting
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31, 0xffffffff},//reg_vbuf_pi_1st_lfsr_mask 0~3 x&y
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31, 0xffffffff},//reg_vbuf_ip_1st_lfsr_mask 0~3 x&y
	},

	//#2
	/*THE_ECLIPSE.fhd24_01.mkv*/
	/*大宋宫词(da song gong ci)*/
	/*60fps-402_Philips test sequence.mpg*/
	/*8009*/
	/*60fps-402_Philips test sequence.mpg 3s HSR*/
	//RANDMASK_MOST_NEAR
	{
		// set enum id
		{RANDMASK_MOST_NEAR,0,0,0},
		
		// setting
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31, 0x11111111},//reg_vbuf_pi_1st_lfsr_mask 0~3 x&y
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31, 0x11111111},//reg_vbuf_ip_1st_lfsr_mask 0~3 x&y
	},

	//#3 HALO
	/*Martian.4KHDR_4_contrast.mp4*/
	/*Blade Runner.4khdr_06.mp4*/
	/*CAPTAIN_AMERICA__THE_FIRST_AVENGER_4KHDR15.mp4*/
	/*Transformers.4khdr_1.mp4*/
	/*MI_III.c03.short.mp4*/
	/*DTS X?³æ?æ¼ç¤ºï¼é?ï¼?mkv 2:40, 9:11*/
	/*30006 33s*/
	/*7002 一个桶_25Hz_淡入淡出 5:46*/
	/*60017 下边界破碎1 0:07*/
	/*长津湖 0:10 0:40*/
	/*C0013 1:08*/
	/*60026*/
	/*7006 0:20*/
	/*史莱姆破碎 0:12~0:18*/
	/*刘老根 第五步01片头后 15s~21s*/
	//--#02-11_04_databroken_pirate
	//--#8.sydney5_motion24p.m2ts 11s
	//--#8.sydney5_motion24p.m2ts 9s
	//--#7_8.sydney5_motion24p 22s
	//--#7_8.sydney5_motion24p 29s
	//--#7_8.sydney5_motion24p 25s
	//--#30006 33s
	//--#猫妖传 3:30
	//402_Philips test sequence_60fps
	//0001_Satelite_1920x1080_24fps
	//DEHALO_CLOSE
	{
		// set enum id
		{DEHALO_CLOSE,0,0,0},
		
		// setting
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0},//FRC_TOP__KME_DEHALO__reg_dh_en
		{KME_DEHALO5_PHMV_FIX_35_reg, 22,22, 1},
	},

	//#4 LFB levle=0
	/*Satelite_1920x1080_24_362.avi*/
	/*4K.znds.com]ShutUpAndPlay.Berlin.2014.2160p.HDTV.HEVC-jTV-50p-3840x2160*/
	/*8004_脱口秀大会 0:05*/
	/*4K023transporter_explosion*/
	/*30012_IDT_Eureka_moon*/
	/*c001 0:01*/
	//0:28-0:29   0:58-1:00   01:46-01:48	 02:33-02:34
	//--#c001 1s
	//LFB_LV0
	{
		// set enum id
		{LFB_LV0,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg, 14, 14, 1},//FRC_TOP__MC__reg_mc_fb_lvl_force_en
		{MC_MC_28_reg, 15, 22, 0},//FRC_TOP__MC__reg_mc_fb_lvl_force_value
	},

	//#5 LEB level=255
	/*MI3_17_M2TS*/
	/*TobyFree.com - 8K UHD Test Pattern H.264 MP4 [4320p].mp4*/
	/*1080P-motion-res-test-pattern*/
	/*7.12.02.movZonePlate_AVC1080P60.mp4*/
	/*Transformer.3.fly_men_short*/
	/*avsync_cont-4000*/
	/*list 311《龙马精神.MP4》1m15s*/
	/*0013*/
	/*7010三十而已71_1*/
	/*雪中悍刀行第25集*/
	/*圣托白云 HSR*/
	//LFB_LV255
	{
		// set enum id
		{LFB_LV255,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg, 14, 14, 1},//FRC_TOP__MC__reg_mc_fb_lvl_force_en
		{MC_MC_28_reg, 15, 22, 255},//FRC_TOP__MC__reg_mc_fb_lvl_force_value
	},

	//#6 GFB level=0
	//GFB_LV0
	{
		// set enum id
		{GFB_LV0,0,0,0},
		
		// setting
		{SOFTWARE_SOFTWARE_10_reg, 0, 0, 0},
	},

	//#7 GFB level=255
	//GFB_LV255
	{
		// set enum id
		{GFB_LV255,0,0,0},
		
		// setting
		{SOFTWARE_SOFTWARE_11_reg, 31, 31, 1},
	},
			
	//#8 LEB level=0, GFB level=0
	//--#8008
	//--#24003 11s
	//--#30018 6s
	//GFB_LV0_LFB_LV0
	{
		// set enum id
		{GFB_LV0_LFB_LV0,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg,14,14,0x1},
		{MC_MC_28_reg,15,22,0x0},
		{SOFTWARE_SOFTWARE_10_reg, 0, 0, 0},
	},

	//#9 HALO close and LFB=0
	/*#240001_Satelite 0:04*/
	/*8008*/
	/*瑞士 2:08*/
	/*60015-Woke from home 13s*/
	/*60016_work form home2 10s 12s*/
	/*DTS X?³æ?æ¼ç¤ºï¼é?ï¼?mkv 4:56*/
	//--#04_Montage_P 85s 90s 95s
	//--#瑞士 2:08
	//60026
	//DEHALO_CLOSE_LFB_LV0
	{
		// set enum id
		{DEHALO_CLOSE_LFB_LV0,0,0,0},
		
		// setting
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0},//FRC_TOP__KME_DEHALO__reg_dh_en
		{KME_DEHALO5_PHMV_FIX_35_reg, 22,22, 1},
		{KMC_BI_bi_top_01_reg, 0, 0, 0},
		{MC_MC_28_reg,		  14,14, 1},
		{MC_MC_28_reg,		  15,22, 0},
	},

	//#10 HALO close and GFB=0, LFB=0
	// 60016_work form home2 10s 12s
	//GFB_LV0_LFB_LV0_DEHALO_CLOSE
	{
		// set enum id
		{GFB_LV0_LFB_LV0_DEHALO_CLOSE,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0}, 	// local FB
		{MC_MC_28_reg,14,14,0x1},	// local FB
		{MC_MC_28_reg,15,22,0x0},	// local FB
		{SOFTWARE_SOFTWARE_10_reg, 0, 0, 0},
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
	},

	//#11 undo mask
	/*雪中悍刀行第25集 0:20, 1:24, 03:17*/
	/*雪中悍刀行 3:32*/
	//15-HD_H_R
	//list130 20s
	//list130 01:24s
	//list130 03:17s
	//list138 10s
	//list139 40s
	// C0013 1:08
	//UNDOMASK_CLOSE
	{
		// set enum id
		{UNDOMASK_CLOSE,0,0,0},
		
		// setting
		{KME_ALGO_ME1_MEDFLT_CTRL_00_reg, 18, 18, 0},
	},

	//#14 flp_alp more smooth
	//FLP_ALP_MORE_SMOOTH
	{
		// set enum id
		{FLP_ALP_MORE_SMOOTH,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg,  0,  8, 0x16},
		{KME_IPME_KME_IPME_8C_reg,  9, 17, 0x16},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 0x14},
		{KME_IPME_KME_IPME_90_reg,  0,  8, 0x14},
		{KME_IPME_KME_IPME_90_reg,  9, 17, 0x12},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 0x12},
		{KME_IPME_KME_IPME_94_reg,  0,  8, 0x10},
		{KME_IPME_KME_IPME_94_reg,  9, 17, 0x16},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 0x16},
		{KME_IPME_KME_IPME_98_reg,  0,  8, 0x14},
		{KME_IPME_KME_IPME_98_reg,  9, 17, 0x14},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 0x12},
		{KME_IPME_KME_IPME_04_reg,  0,  8, 0x12},
		{KME_IPME_KME_IPME_04_reg,  9, 17, 0x10},
	},

	//#14 flp_alp more sharp
	//FLP_ALP_MORE_SHARP
	{
		// set enum id
		{FLP_ALP_MORE_SHARP,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg,	0,	8, 0x20},
		{KME_IPME_KME_IPME_8C_reg,	9, 17, 0x1c},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 0x18},
		{KME_IPME_KME_IPME_90_reg,	0,	8, 0x14},
		{KME_IPME_KME_IPME_90_reg,	9, 17, 0x10},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 0xe},
		{KME_IPME_KME_IPME_94_reg,	0,	8, 0xa},
		{KME_IPME_KME_IPME_94_reg,	9, 17, 0x20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 0x1c},
		{KME_IPME_KME_IPME_98_reg,	0,	8, 0x18},
		{KME_IPME_KME_IPME_98_reg,	9, 17, 0x14},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 0x10},
		{KME_IPME_KME_IPME_04_reg,	0,	8, 0xe},
		{KME_IPME_KME_IPME_04_reg,	9, 17, 0xa},
	},
	
	//#16 flp_alp most smooth
	//DMN Cityscapes 720p60 32s
	//FLP_ALP_MOST_SMOOTH
	{
		// set enum id
		{FLP_ALP_MOST_SMOOTH,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg,	0,	8, 20},
		{KME_IPME_KME_IPME_8C_reg,	9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg,	0,	8, 20},
		{KME_IPME_KME_IPME_90_reg,	9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},
	
		{KME_IPME_KME_IPME_94_reg,	0,	8, 19},
		{KME_IPME_KME_IPME_94_reg,	9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg,	0,	8, 20},
		{KME_IPME_KME_IPME_98_reg,	9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg,	0,	8, 19},
		{KME_IPME_KME_IPME_04_reg,	9, 17, 19},
	},

	//#14 flp_alp most sharp
	//FLP_ALP_MOST_SHARP
	{
		// set enum id
		{FLP_ALP_MOST_SHARP,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg,  0,  8, 0x40},
		{KME_IPME_KME_IPME_8C_reg,  9, 17, 0x30},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 0x20},
		{KME_IPME_KME_IPME_90_reg,  0,  8, 0x10},
		{KME_IPME_KME_IPME_90_reg,  9, 17, 0x0},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 0x0},
		{KME_IPME_KME_IPME_94_reg,  0,  8, 0x0},
		{KME_IPME_KME_IPME_94_reg,  9, 17, 0x40},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 0x30},
		{KME_IPME_KME_IPME_98_reg,  0,  8, 0x20},
		{KME_IPME_KME_IPME_98_reg,  9, 17, 0x10},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 0x0},
		{KME_IPME_KME_IPME_04_reg,  0,  8, 0x0},
		{KME_IPME_KME_IPME_04_reg,  9, 17, 0x0},
	},

	//list120 15s
	//--Hivi_640pixel_00057.m2ts & HIvi_960pixel_00058.m2ts
	//--#电视的觉醒年代 2:10
	//--#聖托里尼 5:09s
	//FLP_ALP_MOST_SMOOTH_RANDMASK_MORE_NEAR
	{
		// set enum id
		{FLP_ALP_MOST_SMOOTH_RANDMASK_MORE_NEAR,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},
	
		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},
	
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0x33333333},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0x33333333},
	},

	//--#土耳其 16s
	//--#土耳其 23s
	//--#雪中捍刀行 7:55
	//FLP_ALP_MOST_SMOOTH_RANDMASK_MORE_NEAR_UNDOMASK_CLOSE
	{
		// set enum id
		{FLP_ALP_MOST_SMOOTH_RANDMASK_MORE_NEAR_UNDOMASK_CLOSE,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},
		
		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},

		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0x33333333},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0x33333333},
		{KME_ALGO_ME1_MEDFLT_CTRL_00_reg, 18,18,0},
	},
	
	//--#2049
	//FLP_ALP_MOST_SMOOTH_LFB_LV0
	{
		// set enum id
		{FLP_ALP_MOST_SMOOTH_LFB_LV0,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},
		
		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},

		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg,14,14,0x1},
		{MC_MC_28_reg,15,22,0x0},
	},

	//--#402 05s
	//--#23_DK_Zoomin_crop_i 0s
	//--#13_05_periodical_MI3_stair 0s
	//--#25005huanlerenjian1 50s
	//--#C011 北京风景人抖动 2 5 10 11s
	//--#240008_DK_Zoomin
	//--#30006 45s
	//FLP_ALP_MOST_SMOOTH_RANDMASK_MOST_NEAR
	{
		// set enum id
		{FLP_ALP_MOST_SMOOTH_RANDMASK_MOST_NEAR,0,0,0},

		// setting
		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},
		
		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},
	
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0x11111111},
	},
	
	//--#17_DNM hrnm 720p60 78s
	//FLP_ALP_MOST_SMOOTH_RANDMASK_MOST_NEAR_DEHALO_CLOSE
	{
		// set enum id
		{FLP_ALP_MOST_SMOOTH_RANDMASK_MOST_NEAR_DEHALO_CLOSE,0,0,0},

		// setting
		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},
	
		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},
	
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31,0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31,0x11111111},

		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
	},
	
	//--#240001_Satelite 4s white plate
	//DEHALO_CLOSE_LFB_LV0_RANDMASK_MOST_FAR
	{
		// set enum id
		{DEHALO_CLOSE_LFB_LV0_RANDMASK_MOST_FAR,0,0,0},

		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg,14,14,0x1},
		{MC_MC_28_reg,15,22,0x0},
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0xffffffff},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0xffffffff},
	},
	
	//下馬卡頓 播放片源，画面下方字幕抖动
	//ME1_ME2_ZMV_CAND_EN
	{
		//set enum id
		{ME1_ME2_ZMV_CAND_EN,0,0,0},
		
		// setting
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_30_reg, 16, 16, 1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 15, 15, 1},
	},

	//0014_24 1:00
	//0014_24 2:08
	//我和我的祖國
	//DYNAMIC_REPEAT_CLOSE_GFB_LV0
	{
		//set enum id
		{DYNAMIC_REPEAT_CLOSE_GFB_LV0,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_63_reg, 31, 31, 0},  //disable repeat broken set
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
	},

	//#29 repeat OFF
	//DYNAMIC_REPEAT_OFF
	{
		//set enum id
		{DYNAMIC_REPEAT_OFF,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_63_reg,  0,  0, 1},
		{SOFTWARE3_SOFTWARE3_63_reg,  1,  2, 0},
	},

	//#30 repeat LOW
	//DYNAMIC_REPEAT_LOW
	{
		//set enum id
		{DYNAMIC_REPEAT_LOW,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_63_reg,  0,  0, 1},
		{SOFTWARE3_SOFTWARE3_63_reg,  1,  2, 1},
	},

	//#30 repeat MID
	//DYNAMIC_REPEAT_MID
	{
		//set enum id
		{DYNAMIC_REPEAT_MID,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_63_reg,  0,  0, 1},
		{SOFTWARE3_SOFTWARE3_63_reg,  1,  2, 2},
	},
	
	//--#003_rotterdam_logo 17s
	//REPEAT_MID_LFB_CLOSE
	{
		//set enum id
		{REPEAT_MID_LFB_CLOSE,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_63_reg,0, 2, 5},//set RepeatMode_MID
		{KMC_BI_bi_top_01_reg,0,0,0x0}, 	// local FB
		{MC_MC_28_reg,14,14,0x1},			// local FB
		{MC_MC_28_reg,15,22,0x0},			// local FB
	},
	
	//--#60027自然世界-鹰之力1
	//EAGLE_1
	{
		//set enum id
		{EAGLE_1,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_63_reg,0, 2, 1},//set RepeatMode_OFF
		{KME_IPME_KME_IPME_2C_reg,10,17,6},
		{SOFTWARE_SOFTWARE_03_reg,16,23,24},
	},
	
	//--#60028自然世界-鹰之力2
	//EAGLE_POWER2
	{
		//set enum id
		{EAGLE_POWER2,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_63_reg,0, 2, 1},//set RepeatMode_OFF
		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},
	
		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},
		
		{KME_ME1_BG1_ME_BG_S4_4_reg, 24, 24, 0x1},
		{KME_ME1_BG1_ME_BG_S4_3_reg, 0, 7, 255},	 
		{KME_ME1_BG1_ME_BG_S4_3_reg, 8, 15, 230},
		{KME_DEHALO5_PHMV_FIX_5_reg, 15, 15, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 7, 7, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 6, 6, 0},
		{KME_DEHALO5_PHMV_FIX_37_reg, 16, 16, 0},
		{KME_DEHALO5_PHMV_FIX_38_reg, 0, 0, 0},
		
		{HARDWARE_HARDWARE_24_reg, 28, 28, 0},
		{HARDWARE_HARDWARE_24_reg, 31, 31, 0},
		{HARDWARE_HARDWARE_24_reg, 30, 30, 0},
		{HARDWARE_HARDWARE_24_reg, 18, 18, 0},
	
		{KME_LOGO0_KME_LOGO0_14_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_18_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_1C_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_20_reg,0,31,0x0},
	
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0x11111111},
			
		{KMC_BI_bi_top_01_reg,0,0,0x0}, 	// local FB
		{MC_MC_28_reg,14,14,0x1},				// local FB
		{MC_MC_28_reg,15,22,0x0},				// local FB
	
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 15, 15, 0},//gmv candidate
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 14, 14, 0},//gmv candidate
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,	1,	1, 0},//candidate
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,	4,	4, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 11, 11, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,	0,	0, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,	3,	3, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 10, 10, 0},
	},

	//#6 meander up to low
	//MEANDER_UP_TO_LOW
	{
		//set enum id
		{MEANDER_UP_TO_LOW,0,0,0},
		
		// setting
		{KME_TOP_KME_TOP_08_reg, 2, 2, 1}, //reg_frame_meander_force_en
		{KME_TOP_KME_TOP_08_reg, 3, 3, 0}, //reg_frame_scan_meander_value
	},

	//#6 meander low to up
	/*list 299《UHD60f_S01.mp4*/
	/*0016_Sweep1080P_24fps.m2ts*/
	//MEANDER_LOW_TO_UP
	{
		//set enum id
		{MEANDER_LOW_TO_UP,0,0,0},
		
		// setting
		{KME_TOP_KME_TOP_08_reg, 2, 2, 1}, //reg_frame_meander_force_en
		{KME_TOP_KME_TOP_08_reg, 3, 3, 1}, //reg_frame_scan_meander_value
	},
	
	/*25018_JIEWU22.mp4*/
	/*25017_JIEWU11 HSR*/
	/*30005_COM_Train_30fps.mpg*/
	//FB_STYLE_CINEMA_MEANDER_UP_TO_LOW
	{
		//set enum id
		{FB_STYLE_CINEMA_MEANDER_UP_TO_LOW,0,0,0},
		
		// setting
		{KME_TOP_KME_TOP_08_reg, 2, 3, 1},//meander up to low
		{SOFTWARE2_SOFTWARE2_39_reg, 0, 3, FB_STYLE_CINEMA},
	},
	
	/*30005_COM_Train_30fps.mpg*/
	//FB_STYLE_CINEMA_MEANDER_LOW_TO_UP
	{
		//set enum id
		{FB_STYLE_CINEMA_MEANDER_LOW_TO_UP,0,0,0},
		
		// setting
		{KME_TOP_KME_TOP_08_reg, 2, 3, 3},//meander low to up
		{SOFTWARE2_SOFTWARE2_39_reg, 0, 3, FB_STYLE_CINEMA},
	},

	//--#broken sutitle
	//BROKEN_SUBTITLE
	{
		//set enum id
		{BROKEN_SUBTITLE,0,0,0},
		
		// setting
		{KME_DEHALO2_KME_DEHALO2_24_reg, 6, 8,7},
		{KME_DEHALO2_KME_DEHALO2_24_reg,22,24,7},
		{KME_LOGO0_KME_LOGO0_D8_reg, 0, 3,0x1},
		{KME_LOGO0_KME_LOGO0_D8_reg, 4, 7,0x1},
		{KME_LOGO0_KME_LOGO0_D8_reg, 8,11,0x1},
		{KME_LOGO0_KME_LOGO0_E4_reg, 0, 3,0xf},
		{KME_LOGO0_KME_LOGO0_E4_reg, 4, 7,0xf},
		{KME_LOGO0_KME_LOGO0_E4_reg, 8,11,0xf},
	},

	//--#broken subtitle八百 1:00:15
	//--#broken subtitle八百 1:00:24
	//BROKEN_SUBTITLE_BLKLOGO_ONLY
	{
		//set enum id
		{BROKEN_SUBTITLE_BLKLOGO_ONLY,0,0,0},
		
		// setting
		{KME_DEHALO2_KME_DEHALO2_24_reg, 6, 8,7},
		{KME_DEHALO2_KME_DEHALO2_24_reg,22,24,7},
		{KME_LOGO0_KME_LOGO0_D8_reg, 0, 3,0x1},
		{KME_LOGO0_KME_LOGO0_D8_reg, 4, 7,0x1},
		{KME_LOGO0_KME_LOGO0_D8_reg, 8,11,0x1},
		{KME_LOGO0_KME_LOGO0_E4_reg, 0, 3,0xf},
		{KME_LOGO0_KME_LOGO0_E4_reg, 4, 7,0xf},
		{KME_LOGO0_KME_LOGO0_E4_reg, 8,11,0xf},
		{MC2_MC2_20_reg,6,9,2},
		{LBMC_LBMC_88_reg,24,25,2},
	},

	//#12 close block logo
	//--#12-DNM hrnm 720p60 17s
	//--#22_PAL_1_VTS_08_1 35s
	//BLKLOGO_CLOSE
	{
		//set enum id
		{BLKLOGO_CLOSE,0,0,0},
		
		// setting
		{KME_LOGO0_KME_LOGO0_14_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_18_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_1C_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_20_reg,0,31,0x0},
	},
	
	//--#12-DNM hrnm 720p60 128s
	//LFB_LV0_BLKLOGO_CLOSE
	{
		//set enum id
		{LFB_LV0_BLKLOGO_CLOSE,0,0,0},
		
		// setting
		{KME_LOGO0_KME_LOGO0_14_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_18_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_1C_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_20_reg,0,31,0x0},
	
		{KMC_BI_bi_top_01_reg,0,0,0x0}, 	// local FB
		{MC_MC_28_reg,14,14,0x1},	// local FB
		{MC_MC_28_reg,15,22,0x0},	// local FB
	},

	//--#04_DNM hrnm 720p60 5s
	//RANDMASK_MOST_NEAR_BLKLOGO_CLOSE
	{
		//set enum id
		{RANDMASK_MOST_NEAR_BLKLOGO_CLOSE,0,0,0},
		
		// setting
		{KME_LOGO0_KME_LOGO0_14_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_18_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_1C_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_20_reg,0,31,0x0},
	
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31,0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31,0x11111111}
	},

	//--#04_Montage_P 100s
	//FLP_ALP_MOST_SMOOTH_RANDMASK_MOST_NEAR_BLKLOGO_CLOSE
	{
		//set enum id
		{FLP_ALP_MOST_SMOOTH_RANDMASK_MOST_NEAR_BLKLOGO_CLOSE,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},
		
		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},
	
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31,0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31,0x11111111},
	
		{KME_LOGO0_KME_LOGO0_14_reg,0,31,0},
		{KME_LOGO0_KME_LOGO0_18_reg,0,31,0},
		{KME_LOGO0_KME_LOGO0_1C_reg,0,31,0},
		{KME_LOGO0_KME_LOGO0_20_reg,0,31,0},
	},

	/*Rotterdam_1080P24_short*/
	//DEHALO_CLOSE_LFB_LV0_BLKLOGO_CLOSE
	{
		//set enum id
		{DEHALO_CLOSE_LFB_LV0_BLKLOGO_CLOSE,0,0,0},
		
		// setting
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0},//FRC_TOP__KME_DEHALO__reg_dh_en
		{KME_DEHALO5_PHMV_FIX_35_reg, 22,22, 1},
		
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg, 14, 14, 1},//FRC_TOP__MC__reg_mc_fb_lvl_force_en
		{MC_MC_28_reg, 15, 22, 0},//FRC_TOP__MC__reg_mc_fb_lvl_force_value
	
		{KME_LOGO0_KME_LOGO0_14_reg,0,31,0},
		{KME_LOGO0_KME_LOGO0_18_reg,0,31,0},
		{KME_LOGO0_KME_LOGO0_1C_reg,0,31,0},
		{KME_LOGO0_KME_LOGO0_20_reg,0,31,0},
	},

	//--#猫妖传 4:04
	//GFB_LV0_LFB_LV0_LOGO_CLOSE
	{
		//set enum id
		{GFB_LV0_LFB_LV0_LOGO_CLOSE,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg,14,14,0x1},
		{MC_MC_28_reg,15,22,0x0},
		{SOFTWARE_SOFTWARE_10_reg, 0, 0, 0},
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_54_reg, 18, 18, 0x0},
	},

	/*DMN Cityscapes 720p60-breakup.mp4*/
	/*Samsara 54"42:fire black hole*/
	/*Samsara 1"29"14:sun light flash when passing through high frequency*/
	/*Valley of fire_breakup.mp4*/
	/*HQV_text_rot35_MPG1080P24.m2ts*/
	/*list 311《龙马精神.MP4》2m40s*/
	//DEHALO_CLOSE_LFB_LV0_LOGO_CLOSE
	{
		//set enum id
		{DEHALO_CLOSE_LFB_LV0_LOGO_CLOSE,0,0,0},
		
		// setting
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0},//FRC_TOP__KME_DEHALO__reg_dh_en
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg, 14, 14, 1},//FRC_TOP__MC__reg_mc_fb_lvl_force_en
		{MC_MC_28_reg, 15, 22, 0},//FRC_TOP__MC__reg_mc_fb_lvl_force_value
		//{FRC_TOP__PQL_0__pql_patch_dummy_e4_ADDR, 29, 28, 0},//Merlin7 does not have it
		{MC2_MC2_20_reg, 0, 1, 0},//logo off
	},

	//#3
	/*fusion_PW_120Hz_l_Robot_(SD_NTSC)_02.mpg*/
	/*list 252 C011-北京风景*/
	/*list 262 人眼红色拖尾.ts*/
	/*E001. racing play.mp4*/
	/*25022*/
	//DH_DTL_GAIN_EN_DTL_SHIFT_5_DTL_CURVE_Y1_128
	{
		//set enum id
		{DH_DTL_GAIN_EN_DTL_SHIFT_5_DTL_CURVE_Y1_128,0,0,0},
		
		// setting
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0},//FRC_TOP__KME_DEHALO__reg_dh_en
		{KME_DEHALO_KME_DEHALO_FC_reg, 23, 23, 1},//FRC_TOP__KME_DEHALO__dh_dtl_gain_en
		{KME_DEHALO_KME_DEHALO_FC_reg, 20, 22, 5},//FRC_TOP__KME_DEHALO__dh_fb_dtl_shift
		{KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 128},//FRC_TOP__KME_DEHALO__reg_dh_dtl_curve_y1
	},

	//#12
	/*XingFuDaoWanJia 幸福到万家*/
	/*list 311《龙马精神.MP4》3m20s*/
	/*402_Philips test sequence_60fps 1Min50~1Min53S HSR*/
	//XINGFUDAOWANJIA
	{
		//set enum id
		{XINGFUDAOWANJIA,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg, 14, 14, 1},
		{MC_MC_28_reg, 15, 22, 0},
		{KME_ME1_TOP1_ME1_COMMON1_10_reg, 8, 17, 32},
		{KME_ME1_TOP1_ME1_COMMON1_10_reg,18, 27, 32},
		{KME_ME1_TOP1_ME1_COMMON1_12_reg, 16, 31, 65532},
		{KME_TOP_KME_TOP_04_reg, 29, 29, 0},
	},

	//--#04_Montage_P 5s
	//--#25003 10s
	//--#30006 60s
	//RIM_CLOSE
	{
		//set enum id
		{RIM_CLOSE,0,0,0},
		
		// setting
		{HARDWARE_HARDWARE_00_reg,0,0,0x0},
	},

	//--#0080 0s
	/*25003_philips演示_25fps 1Min10s~1Min15S HSR*/
	/*长津湖画面抖动破碎*/
	/*大宋宫词结尾*/
	/*大宋宫词结尾 HSR*/
	/*0009_Hall_Pan_24fps HSR*/
	/*30003_123_30fps.mpg*/
	/*huoche HSR*/
	//LOGO_CLOSE
	{
		//set enum id
		{LOGO_CLOSE,0,0,0},
		
		// setting
		{KME_VBUF_TOP_KME_VBUF_TOP_54_reg, 18, 18, 0x0},
	},

	//BTS
	// 开端-03集
	//BTS_SETTING
	{
		//set enum id
		{BTS_SETTING,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg, 0, 26, 0x502814},
		{KME_IPME_KME_IPME_90_reg, 0, 26, 0x4c2814},
		{KME_IPME_KME_IPME_94_reg, 0, 26, 0x502813},
		{KME_IPME_KME_IPME_98_reg, 0, 26, 0x502814},
		{KME_IPME_KME_IPME_04_reg, 0, 17, 0x2613},
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,	31, 0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,	31, 0x11111111},
		//me2 dediff ph en
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_4C_reg,1,1, 0},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_84_reg,1,1, 0},
	
		//== zmv ==
		//me1
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 16, 16,	0},//ip_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 15, 15,	0},//pi_en		
		{KME_ME1_TOP0_KME_ME1_TOP0_28_reg, 16, 28, 0},//ip_pnt
		{KME_ME1_TOP0_KME_ME1_TOP0_68_reg, 16, 28, 0},//pi_pnt
		//me2
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_30_reg, 16, 16, 0},//1st
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_34_reg, 19, 19, 0},	//2nd		
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_60_reg, 24, 31, 0},//1st 	
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_9C_reg, 0, 7, 0},//2nd
		
		//==ip/pi cand==
		//ip
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 1, 1,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 2, 2,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 3, 3,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 4, 4,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 6, 6,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 7, 7,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 8, 8,	0},
		//pi
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 0, 0,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 1, 1,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 2, 2,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 3, 3,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 5, 5,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 6, 6,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 7, 7,	0},
				
		//== update cand ==
		//me1 ip
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,	13, 13,  0},
		//me1 pi
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,	12, 12,  0},
		//me2 1st
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_30_reg,	18, 23, 0},
		//me2 2nd
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_34_reg,	21, 26,  0},
	
		{KME_ALGO_ME1_MEDFLT_CTRL_00_reg, 18, 18, 0x1},
	},

	//#15
	/*06.Sharks.mp4*/	
	/*H_Move_fast(1080_24p)*/
	/*刘老根 第五步01片头后 15s~21s*/
	/*9009_史莱姆20_24Hz*/
	/*list 257 《C004samll object handing_24fps*/
	/*list 258 《风味人间4谷物星球第六集34min35s》*/
	/*8010*/
	//DEHALO_CLOSE_LFB_LV0_Y_100_UV_255
	{
		//set enum id
		{DEHALO_CLOSE_LFB_LV0_Y_100_UV_255,0,0,0},
		
		// setting
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0},//FRC_TOP__KME_DEHALO__reg_dh_en
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg, 14, 14, 1},//FRC_TOP__MC__reg_mc_fb_lvl_force_en
		{MC_MC_28_reg, 15, 22, 0},//FRC_TOP__MC__reg_mc_fb_lvl_force_value
		{IPPRE_IPPRE_04_reg, 4, 11, 100},//FRC_TOP__IPPRE__reg_kmc_blend_y_alpha
		{IPPRE_IPPRE_04_reg, 16, 23, 255},//FRC_TOP__IPPRE__reg_kmc_blend_uv_alpha
	},

	//#11
	/*7.12.02.movZonePlate_AVC1080P60.mp4*/
	/*01.E_CMO-23_98psf.mp4*/
	//LFB_LV0_MC_BLD_0
	{
		//set enum id
		{LFB_LV0_MC_BLD_0,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg, 14, 14, 1},//FRC_TOP__MC__reg_mc_fb_lvl_force_en
		{MC_MC_28_reg, 15, 22, 0},//FRC_TOP__MC__reg_mc_fb_lvl_force_value
		//{KIWI_REG(FRC_TOP__PQL_1__pql_fb_ctrl_en_ADDR), 0},//Merlin7 do not have it 
		{MC_MC_B0_reg, 20, 27, 0},//FRC_TOP__MC__reg_mc_bld_flbk_lvl_ADDR
	},
	
	//--#25005huanlerenjian1 15s
	//--#24004 0s
	//FLP_ALP_MOST_SMOOTH_RANDMASK_MOST_NEAR_BLKLOGO_CLOSE_DEHALO_CONDTION_CLOSE
	{
		//set enum id
		{FLP_ALP_MOST_SMOOTH_RANDMASK_MOST_NEAR_BLKLOGO_CLOSE_DEHALO_CONDTION_CLOSE,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},
		
		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},
		
		{KME_ME1_BG1_ME_BG_S4_4_reg, 24, 24, 0x1},
		{KME_ME1_BG1_ME_BG_S4_3_reg, 0, 7, 255},	 
		{KME_ME1_BG1_ME_BG_S4_3_reg, 8, 15, 230},
		{KME_DEHALO5_PHMV_FIX_5_reg, 15, 15, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 7, 7, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 6, 6, 0},
		{KME_DEHALO5_PHMV_FIX_37_reg, 16, 16, 0},
		{KME_DEHALO5_PHMV_FIX_38_reg, 0, 0, 0},
		
		{HARDWARE_HARDWARE_24_reg, 28, 28, 0},
		{HARDWARE_HARDWARE_24_reg, 31, 31, 0},
		{HARDWARE_HARDWARE_24_reg, 30, 30, 0},
		{HARDWARE_HARDWARE_24_reg, 18, 18, 0},
		
		{KME_LOGO0_KME_LOGO0_14_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_18_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_1C_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_20_reg,0,31,0x0},
		
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0x11111111},
	},

	/*Rotterdam_1080P24_short*/
	//DEHALO_CLOSE_LFB_LV0_BLKLOGO_CLOSE_ME2SOBJ_3
	{
		//set enum id
		{DEHALO_CLOSE_LFB_LV0_BLKLOGO_CLOSE_ME2SOBJ_3,0,0,0},
		
		// setting
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0},//FRC_TOP__KME_DEHALO__reg_dh_en
		{KME_DEHALO5_PHMV_FIX_35_reg, 22,22, 1},
		
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg, 14, 14, 1},//FRC_TOP__MC__reg_mc_fb_lvl_force_en
		{MC_MC_28_reg, 15, 22, 0},//FRC_TOP__MC__reg_mc_fb_lvl_force_value

		{KME_LOGO0_KME_LOGO0_14_reg,0,31,0},
		{KME_LOGO0_KME_LOGO0_18_reg,0,31,0},
		{KME_LOGO0_KME_LOGO0_1C_reg,0,31,0},
		{KME_LOGO0_KME_LOGO0_20_reg,0,31,0},

		{SOFTWARE3_SOFTWARE3_04_reg, 24, 26, 3},
	},

	//#33
	/*7.9.09.IDT_HD_H_R_MPG1080P24.mp4*/
	//IDT_HD_H_R_MPG1080P24
	{
		//set enum id
		{IDT_HD_H_R_MPG1080P24,0,0,0},
		
		// setting
		{COLOR_SHARP_DM_NR_SHP_CTRL_reg, 0, 0, 0},//  sharpness peaking
		{COLOR_SHARP_SHP_DLTI_CTRL_1_reg, 0, 0, 0},//  sharpness dlti
		{COLOR_MB_PEAKING_MB_PEAKING_CTRL_reg, 0, 0, 0},//	Multiband peaking
		{COLOR_MB_PEAKING_MB_Double_Buffer_CTRL_reg, 0, 0, 1},//  Multiband peaking DB apply
		{KME_DEHALO_KME_DEHALO_70_reg, 4, 4,1},//FRC_TOP__KME_DEHALO__dh_pfv_conf_gen_andor_fb_sel
		{KME_DEHALO_KME_DEHALO_B0_reg, 12, 19,0},//FRC_TOP__KME_DEHALO__reg_dh_occl_gen_sim_thr
		{KME_DEHALO_KME_DEHALO_D4_reg, 6, 11,15},//FRC_TOP__KME_DEHALO__dh_pfv_gen_force_dehalo_level
		{MC2_MC2_50_reg,0,1, 0},//FRC_TOP__MC2__reg_mc_var_lpf_en
		//{FRC_TOP__PQL_0__pql_patch_dummy_e4_ADDR,7,7, 0},//Merlin7 does not have it
		{MC2_MC2_20_reg, 0 ,1, 0},	//FRC_TOP__MC2__reg_mc_logo_en	
		//{FRC_TOP__PQL_0__pql_patch_dummy_e4_ADDR,5,5, 0},//Merlin7 does not have it
	},

	/*Passion Cocoa Milk Buns*/// standing at the middle
	/*Passion Cocoa Milk Buns*/ // kneading the dough
	//PASSION_COCOA_MILK_BUNS
	{
		//set enum id
		{PASSION_COCOA_MILK_BUNS,0,0,0},
		
		// setting
		{KME_DEHALO_KME_DEHALO_FC_reg, 23, 23, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 4, 4, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 5, 5, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 6, 6, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 7, 7, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 8, 8, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 12, 12, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 13, 13, 0},
		
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_40_reg, 13, 16, 1},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_40_reg,	9, 12, 1},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31, 0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31, 0x11111111},
	},

	/*PQ_LGSTB_2K60p.mp4 "Squid Game" sub-title*/
	//SQUIDGAME_SUBTITLE
	{
		//set enum id
		{SQUIDGAME_SUBTITLE,0,0,0},
		
		// setting
		{KME_DEHALO2_KME_DEHALO2_24_reg,  6,  8, 7},
		{KME_DEHALO2_KME_DEHALO2_24_reg, 22, 24, 7},
		{KME_DEHALO2_KME_DEHALO2_28_reg,  6,  8, 7},
		{KME_LOGO2_KME_LOGO2_28_reg, 9, 14, 3},
		{KME_LOGO2_KME_LOGO2_2C_reg, 9, 16, 4},
	},

	//--#04_Montage_P 5s
	//--#22_PAL_1_VTS_08_1 184s 102s 109s 187s
	//--#22_PAL_1_VTS_08_1 229s
	//MONTAGE_P_5
	{
		//set enum id
		{MONTAGE_P_5,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},

		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},

		{KME_ME1_BG1_ME_BG_S4_4_reg, 24, 24, 0x1},
		{KME_ME1_BG1_ME_BG_S4_3_reg, 0, 7, 255},	 
		{KME_ME1_BG1_ME_BG_S4_3_reg, 8, 15, 230},
		{KME_DEHALO5_PHMV_FIX_5_reg, 15, 15, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 7, 7, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 6, 6, 0},
		{KME_DEHALO5_PHMV_FIX_37_reg, 16, 16, 0},
		{KME_DEHALO5_PHMV_FIX_38_reg, 0, 0, 0},

		{HARDWARE_HARDWARE_24_reg, 28, 28, 0},
		{HARDWARE_HARDWARE_24_reg, 31, 31, 0},
		{HARDWARE_HARDWARE_24_reg, 30, 30, 0},
		{HARDWARE_HARDWARE_24_reg, 18, 18, 0},

		{KME_LOGO0_KME_LOGO0_14_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_18_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_1C_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_20_reg,0,31,0x0},

		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0x11111111},
	},

	//--#04_Montage_P 46s
	//MONTAGE_P_46
	{
		//set enum id
		{MONTAGE_P_46,0,0,0},
		
		// setting
		{KME_DEHALO5_PHMV_FIX_8_reg, 26, 30, 10},
		{KME_DEHALO5_PHMV_FIX_6_reg, 16, 21, 11},
		{KME_DEHALO5_PHMV_FIX_2A_reg, 4, 13, 290},
		
		{KME_ME1_BG1_ME_BG_S4_4_reg, 24, 24, 0x1},
		{KME_ME1_BG1_ME_BG_S4_3_reg, 0, 7, 255},	 
		{KME_ME1_BG1_ME_BG_S4_3_reg, 8, 15, 230},
		{KME_DEHALO5_PHMV_FIX_5_reg, 15, 15, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 7, 7, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 6, 6, 0},
		{KME_DEHALO5_PHMV_FIX_37_reg, 16, 16, 0},
		{KME_DEHALO5_PHMV_FIX_38_reg, 0, 0, 0},
		
		{HARDWARE_HARDWARE_24_reg, 28, 28, 0},
		{HARDWARE_HARDWARE_24_reg, 31, 31, 0},
		{HARDWARE_HARDWARE_24_reg, 30, 30, 0},
		{HARDWARE_HARDWARE_24_reg, 18, 18, 0},
	},

	//--#12-DNM hrnm 720p60 21s
	//DNM_HRNM_21
	{
		//set enum id
		{DNM_HRNM_21,0,0,0},
		
		// setting
		{KME_ME1_BG1_ME_BG_S4_4_reg, 24, 24, 0x1},
		{KME_ME1_BG1_ME_BG_S4_3_reg, 0, 7, 255},	 
		{KME_ME1_BG1_ME_BG_S4_3_reg, 8, 15, 230},
		{KME_DEHALO5_PHMV_FIX_5_reg, 15, 15, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 7, 7, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 6, 6, 0},
		{KME_DEHALO5_PHMV_FIX_37_reg, 16, 16, 0},
		{KME_DEHALO5_PHMV_FIX_38_reg, 0, 0, 0},
		
		{HARDWARE_HARDWARE_24_reg, 28, 28, 0},
		{HARDWARE_HARDWARE_24_reg, 31, 31, 0},
		{HARDWARE_HARDWARE_24_reg, 30, 30, 0},
		{HARDWARE_HARDWARE_24_reg, 18, 18, 0},
	},

	//--#15-HD_H_R_32_i
	//HD_H_R_32_I
	{
		//set enum id
		{HD_H_R_32_I,0,0,0},
		
		// setting
		{HARDWARE_HARDWARE_56_reg, 31, 31, 0x0},//_param_read_comreg.read_enable
		{MC2_MC2_50_reg,0,1,0x0},
		{KME_ME1_BG0_ME1_BG_15_reg,31,31,1},
		{KME_ME1_BG0_ME1_BG_15_reg,10,30,0},
		{KME_ME1_BG1_ME_BG_RANGE_2_reg,8,15,240},
		{KME_ME1_BG1_ME_BG_RANGE_2_reg,24,31,135},
		{KME_ME1_BG1_ME_BG_TH0_reg,0,9,5},
		{KME_ME1_BG1_ME_BG_TH0_reg,10,19,5},
		{KME_ME1_BG1_ME_BG_TH0_reg,20,29,8},
		{KME_ME1_BG0_ME1_BG_14_reg,0,7,5},
		{KME_ME1_BG0_ME1_BG_1_reg,13,17,1},
		{KME_ME1_BG0_ME1_BG_1_reg,8,12,10},
		{KME_ME1_BG0_ME1_BG_2_reg,13,17,1},
		{KME_ME1_BG0_ME1_BG_2_reg,8,12,6},
		{KME_ME1_BG0_ME1_BG_11_reg,30,31,3},
		{KME_ME1_BG0_ME1_BG_13_reg,26,26,0},

		{KME_DEHALO5_PHMV_FIX_25_reg,16,25,0x2},
		{KME_DEHALO5_PHMV_FIX_3_reg,0,11,0x32},
		{KME_DEHALO5_PHMV_FIX_29_reg,24,24,0x1},
		{KME_DEHALO5_PHMV_FIX_2_reg,4,4,0x1},
		{KME_DEHALO5_PHMV_FIX_1D_reg,16,25,0x3ff},
		{KME_DEHALO5_PHMV_FIX_1E_reg,0,6,0x5},
		{KME_DEHALO5_PHMV_FIX_1E_reg,8,14,0xc},
		{KME_DEHALO5_PHMV_FIX_1E_reg,16,22,0x5},
		{KME_DEHALO5_PHMV_FIX_1E_reg,24,30,0xc},
		{KME_DEHALO5_PHMV_FIX_2A_reg,4,13,0x32},
		{KME_DEHALO5_PHMV_FIX_5_reg,15,15,0x0},
		{KME_DEHALO5_PHMV_FIX_6_reg,6,7,0x0},
		{KME_DEHALO5_PHMV_FIX_38_reg,0,0,0x0},
		{KME_DEHALO5_PHMV_FIX_6_reg,16,21,0xa},
		{KME_DEHALO5_PHMV_FIX_6_reg,24,29,0xa},
		{KME_DEHALO5_PHMV_FIX_23_reg,16,16,0x1},
		{KME_DEHALO5_PHMV_FIX_21_reg,0,31,0xff825000},
		{KME_DEHALO5_PHMV_FIX_23_reg,17,17,0x1},
		{SOFTWARE1_SOFTWARE1_49_reg,31,31,0x0},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,24,26,0x2},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,16,18,0x2},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,9,11,0x2},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,2,4,0x2},

		{HARDWARE_HARDWARE_24_reg, 28, 28, 0},
		{HARDWARE_HARDWARE_24_reg, 30, 31, 0},
		{HARDWARE_HARDWARE_24_reg, 18, 18, 0},

		{COLOR_D_VC_Global_CTRL_reg, 0, 0, 0},// local contrast
		{COLOR_D_VC_Global_CTRL_reg, 21, 21, 0},// local contrast
	},

	//--#19 monosco4K_Hscroll_10pix_50Hz_long-50p-3840x2160
	//MONOSCO
	{
		//set enum id
		{MONOSCO,0,0,0},
		
		// setting
		{MC_MC_D8_reg, 31, 31, 0x1},
		{MC_MC_D8_reg, 16, 26, 0x7F6},//2038
		{KME_LOGO0_KME_LOGO0_14_reg, 0, 31, 0x0},
		{KME_LOGO0_KME_LOGO0_18_reg, 0, 31, 0x0},
		{KME_LOGO0_KME_LOGO0_1C_reg, 0, 31, 0x0},
		{KME_LOGO0_KME_LOGO0_20_reg, 0, 31, 0x0},
	
		{HARDWARE_HARDWARE_57_reg,17,17, 0},
		{MC2_MC2_50_reg, 0, 0, 0},
		{MC2_MC2_50_reg, 1, 1, 0},
	},

	//--#20-Montage_M 104s
	//MONTAGE_M_104
	{
		//set enum id
		{MONTAGE_M_104,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
		{MC_MC_28_reg,14,14,0x1},
		{MC_MC_28_reg,15,22,0x0},
		{HARDWARE_HARDWARE_06_reg,0,1,0x0}, 	// dehalo
	},

	//--#20-22_PAL_1_VTS_08_1 0s
	//--#22_PAL_1_VTS_08_1 10s
	//PAL_1_VTS_08_1_10
	{
		//set enum id
		{PAL_1_VTS_08_1_10,0,0,0},
		
		// setting
		{KME_ME1_BG1_ME_BG_S4_4_reg, 24, 24, 0x1},
		{KME_ME1_BG1_ME_BG_S4_3_reg, 0, 7, 255},	 
		{KME_ME1_BG1_ME_BG_S4_3_reg, 8, 15, 230},
		{KME_DEHALO5_PHMV_FIX_5_reg, 15, 15, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 7, 7, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 6, 6, 0},
		{KME_DEHALO5_PHMV_FIX_37_reg, 16, 16, 0},
		{KME_DEHALO5_PHMV_FIX_38_reg, 0, 0, 0},

		{HARDWARE_HARDWARE_24_reg, 28, 28, 0},
		{HARDWARE_HARDWARE_24_reg, 31, 31, 0},
		{HARDWARE_HARDWARE_24_reg, 30, 30, 0},
		{HARDWARE_HARDWARE_24_reg, 18, 18, 0},
	},

	//--#10_03_boundary_BBC1 3s
	//ZMV_CAND_ENABLE_MC_BLD_MVD_MAX
	{
		//set enum id
		{ZMV_CAND_ENABLE_MC_BLD_MVD_MAX,0,0,0},
		
		// setting
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,16,16,1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,15,15,1},
		{MC_MC_B4_reg,24,31,255},
		{MC_MC_B4_reg,16,23,255},
	},

	//--DMN Cityscapes 720p60.ts
	//CITYSCAPES
	{
		//set enum id
		{CITYSCAPES,0,0,0},
		
		// setting
		{MC_MC_28_reg,14,14, 1},
		{MC_MC_28_reg,15,22, 0},
		{KME_ME1_TOP1_ME1_COMMON1_10_reg, 8,17, 32},
		{KME_ME1_TOP1_ME1_COMMON1_10_reg,18,27, 32},
		{KME_ME1_TOP1_ME1_COMMON1_12_reg,16,31, 65532},
		{KME_TOP_KME_TOP_04_reg,29,29, 0},
		{IPPRE_IPPRE_04_reg,16,23, 150},
	},

	//#34
	/*3.QTEK_6.5PPF_00081*/ 
	/*monosco4K_Hscroll_10pix_50Hz_long_50p-3840*2160*/
	//LESS_CAND_SHARPNESS_CLOSE_MULTIBAND_PEAKING_CLOSE
	{
		//set enum id
		{LESS_CAND_SHARPNESS_CLOSE_MULTIBAND_PEAKING_CLOSE,0,0,0},
		
		// setting
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 3, 3, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand2_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 4, 4, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand3_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 5, 5, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand4_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 6, 6, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand5_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 7, 7, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand6_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 8, 8, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand7_en
	
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 2, 2, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand2_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 3, 3, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand3_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 4, 4, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand4_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 5, 5, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand5_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 6, 6, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand6_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 7, 7, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand7_en
		{COLOR_SHARP_DM_NR_SHP_CTRL_reg, 0, 0, 0},//  sharpness peaking
		{COLOR_SHARP_SHP_DLTI_CTRL_1_reg, 0, 0, 0},//  sharpness dlti
		{COLOR_MB_PEAKING_MB_PEAKING_CTRL_reg, 0, 0, 0},//	Multiband peaking
		{COLOR_MB_PEAKING_MB_Double_Buffer_CTRL_reg, 0, 0, 1},//  Multiband peaking DB apply
	},

	//--#20-Montage_M 116s 120s
	//BLKLOGO_GRDTH_60_120_80_220
	{
		//set enum id
		{BLKLOGO_GRDTH_60_120_80_220,0,0,0},
		
		// setting
		{KME_LOGO0_KME_LOGO0_64_reg, 0, 7, 60},
		{KME_LOGO0_KME_LOGO0_64_reg, 8,15,120},
		{KME_LOGO0_KME_LOGO0_64_reg,16,23, 80},
		{KME_LOGO0_KME_LOGO0_64_reg,24,31,220},
	},

	//--#16-H_Move_fast with negative rmvx
	//H_MOVE_FAST_NEGATIVE_RMVX
	{
		//set enum id
		{H_MOVE_FAST_NEGATIVE_RMVX,0,0,0},
		
		// setting
		{HARDWARE_HARDWARE_26_reg, 20, 20, 0x1},
		{HARDWARE_HARDWARE_24_reg, 28, 28, 0x0},//D560
		{HARDWARE_HARDWARE_24_reg, 31, 31, 0x0},//D560

		{KME_ME1_BG1_ME_BG_RANGE_2_reg, 8, 15, 0x0},//EF3C
		{KME_ME1_BG1_ME_BG_RANGE_2_reg, 24, 31, 0x0},//EF3C

		{KME_DEHALO_KME_DEHALO_FC_reg, 0, 7, 0x0},//CDFC
		{HARDWARE_HARDWARE_57_reg, 7, 7, 0x0},//D5E4

		{KME_DEHALO5_PHMV_FIX_1D_reg, 10, 13, 0xf},//EB74
		{KME_DEHALO5_PHMV_FIX_1E_reg, 0, 6, 0x1f},//EB78
		{KME_DEHALO5_PHMV_FIX_1E_reg, 8, 14, 0x12},//EB78
		{KME_DEHALO5_PHMV_FIX_1E_reg, 16, 22, 0x8},//EB78
		{KME_DEHALO5_PHMV_FIX_1E_reg, 24, 30, 0x13},//EB78

		{KME_DEHALO5_PHMV_FIX_2A_reg, 4, 13, 0x12c},//EBAC
		{KME_DEHALO5_PHMV_FIX_5_reg, 15, 15, 0x0},//EB14
		{KME_DEHALO5_PHMV_FIX_6_reg, 6, 6, 0x0},//EB18
		{KME_DEHALO5_PHMV_FIX_6_reg, 7, 7, 0x0},//EB18
		{KME_DEHALO5_PHMV_FIX_6_reg, 16, 21, 0xa},//EB18
		{KME_DEHALO5_PHMV_FIX_38_reg, 0, 0, 0x0},//EBE4
		{KME_DEHALO5_PHMV_FIX_37_reg, 16, 16, 0x0},//EBE0

		{KME_DEHALO5_PHMV_FIX_8_reg, 0, 9, 0x1f},//EB20
		{KME_DEHALO5_PHMV_FIX_4_reg, 0, 4, 0x8},//EB10
		{KME_DEHALO5_PHMV_FIX_4_reg, 8, 12, 0x4},//EB10
		{KME_DEHALO5_PHMV_FIX_33_reg, 3, 3, 0x0},//EBD0
		{KME_DEHALO5_PHMV_FIX_33_reg, 4, 4, 0x0},//EBD0
		{KME_DEHALO5_PHMV_FIX_25_reg, 16, 25, 0x1f},//EB94

		{KMC_BI_bi_top_01_reg,0,0,0x1},
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x1},
		{MC_MC_28_reg,14,14,0x0},
		{MC_MC_28_reg,15,22,0x0},
		{HARDWARE_HARDWARE_06_reg,0,1,0x1}, 	// dehalo
		{KME_DEHALO5_PHMV_FIX_33_reg,3,4,1},
		{KME_TOP_KME_TOP_04_reg,29,29,1},
	},

	//--#16-H_Move_fast with positive rmvx
	//H_MOVE_FAST_POSITIVE_RMVX
	{
		//set enum id
		{H_MOVE_FAST_POSITIVE_RMVX,0,0,0},
		
		// setting
		{HARDWARE_HARDWARE_26_reg, 20, 20, 0x0},
		{HARDWARE_HARDWARE_24_reg, 28, 28, 0x1},//D560
		{HARDWARE_HARDWARE_24_reg, 31, 31, 0x1},//D560

		{KME_ME1_BG1_ME_BG_RANGE_2_reg, 8, 15, 0x0},//EF3C
		{KME_ME1_BG1_ME_BG_RANGE_2_reg, 24, 31, 0x0},//EF3C

		{KME_DEHALO_KME_DEHALO_FC_reg, 0, 7, 0x80},//CDFC
		{HARDWARE_HARDWARE_57_reg, 7, 7, 0x1},//D5E4

		{KME_DEHALO5_PHMV_FIX_1D_reg, 10, 13, 0x0},//EB74
		{KME_DEHALO5_PHMV_FIX_1E_reg, 0, 6, 0x0c},//EB78
		{KME_DEHALO5_PHMV_FIX_1E_reg, 8, 14, 0x05},//EB78
		{KME_DEHALO5_PHMV_FIX_1E_reg, 16, 22, 0x0c},//EB78
		{KME_DEHALO5_PHMV_FIX_1E_reg, 24, 30, 0x05},//EB78

		{KME_DEHALO5_PHMV_FIX_2A_reg, 4, 13, 0x2bc},//EBAC
		{KME_DEHALO5_PHMV_FIX_5_reg, 15, 15, 0x1},//EB14
		{KME_DEHALO5_PHMV_FIX_6_reg, 6, 6, 0x1},//EB18
		{KME_DEHALO5_PHMV_FIX_6_reg, 7, 7, 0x1},//EB18
		{KME_DEHALO5_PHMV_FIX_6_reg, 16, 21, 0x14},//EB18
		{KME_DEHALO5_PHMV_FIX_38_reg, 0, 0, 0x1},//EBE4
		{KME_DEHALO5_PHMV_FIX_37_reg, 16, 16, 0x0},//EBE0

		{KME_DEHALO5_PHMV_FIX_8_reg, 0, 9, 0x15},//EB20
		{KME_DEHALO5_PHMV_FIX_4_reg, 0, 4, 0x4},//EB10
		{KME_DEHALO5_PHMV_FIX_4_reg, 8, 12, 0x4},//EB10
		{KME_DEHALO5_PHMV_FIX_33_reg, 3, 3, 0x1},//EBD0
		{KME_DEHALO5_PHMV_FIX_33_reg, 4, 4, 0x1},//EBD0
		{KME_DEHALO5_PHMV_FIX_25_reg, 16, 25, 0x14},//EB94 

		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
		{MC_MC_28_reg,14,14,0x1},
		{MC_MC_28_reg,15,22,0x0},
		{HARDWARE_HARDWARE_06_reg,0,1,0x0}, 	// dehalo
		{KME_DEHALO5_PHMV_FIX_33_reg,3,4,0},
		{KME_TOP_KME_TOP_04_reg,29,29,0},//meander disble
	},

	//little boy
	//LITTLE_BOY
	{
		//set enum id
		{LITTLE_BOY,0,0,0},
		
		// setting
		{KME_DEHALO2_DH_OCCL_EXTEND_1_reg,4,8,	10},
		{KME_DEHALO2_DH_OCCL_EXTEND_1_reg,13,13,  1},
		{KME_DEHALO2_DH_OCCL_EXTEND_1_reg,16,23, 16},
		{KME_DEHALO2_DH_OCCL_EXTEND_1_reg,24,31, 16},
		{KME_DEHALO2_KME_DEHALO_POSTCORR_reg,0,3, 12},
		{KME_DEHALO2_KME_DEHALO_POSTCORR_reg,4,7,  12},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,16,18,  3},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,19,21,  0},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,24,26, 3},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,27,29, 0},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,2,4,	1},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,5,7,	0},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,9,11, 1},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,12,14, 0},
		{KME_DEHALO2_DH_OCCL_EXTEND_2_reg,4,5,	1},
		{KME_DEHALO2_DH_OCCL_EXTEND_2_reg,8,9, 1},
		{KME_DEHALO2_DH_OCCL_EXTEND_2_reg,12,14,   1},
		{KME_DEHALO2_DH_OCCL_EXTEND_3_reg,8,15, 32},
		{KME_DEHALO2_DH_OCCL_EXTEND_3_reg,24,31, 40},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,8,8, 1},
	},

	//K-13536 issue
	//K13536_Q13835
	{
		//set enum id
		{K13536_Q13835,0,0,0},
		
		// setting
		{KME_VBUF_TOP_KME_VBUF_TOP_4C_reg, 6, 10, 0x8},
	},

	//++#pattern 298 daredevil boy on zidoo player.
	//PATTERN_298_DAREDEVIL_BOY
	{
		//set enum id
		{PATTERN_298_DAREDEVIL_BOY,0,0,0},
		
		// setting
		{MC2_MC2_50_reg, 0, 1, 0x3},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 5, 5, 0x1},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 4, 4, 0x1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 4, 4, 0x1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 3, 3, 0x1},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 13, 13, 0x1},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 12, 12, 0x1},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 11, 11, 0x1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 12, 12, 0x1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 11, 11, 0x1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 10, 10, 0x1},
		{KME_ME1_TOP0_KME_ME1_TOP0_1C_reg, 24, 31, 0x28},
		{KME_ME1_TOP0_KME_ME1_TOP0_20_reg, 16, 23, 0x28},
		{KME_ME1_TOP0_KME_ME1_TOP0_20_reg,	8, 15, 0x28},
		{KME_ME1_TOP0_KME_ME1_TOP0_20_reg,	0,	7, 0x28},
		{KME_ME1_TOP0_KME_ME1_TOP0_5C_reg, 24, 31, 0x28},
		{KME_ME1_TOP0_KME_ME1_TOP0_60_reg, 16, 23, 0x30},
		{KME_ME1_TOP0_KME_ME1_TOP0_60_reg,	8, 15, 0x30},
		{KME_ME1_TOP0_KME_ME1_TOP0_60_reg,	0,	7, 0x28},
		{KME_DEHALO2_DH_OCCL_EXTEND_1_reg,4,8,	10},
		{KME_DEHALO2_DH_OCCL_EXTEND_1_reg,13,13,  1},
		{KME_DEHALO2_DH_OCCL_EXTEND_1_reg,16,23, 16},
		{KME_DEHALO2_DH_OCCL_EXTEND_1_reg,24,31, 16},
		{KME_DEHALO2_KME_DEHALO_POSTCORR_reg,0,3, 12},
		{KME_DEHALO2_KME_DEHALO_POSTCORR_reg,4,7,  12},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,16,18,  3},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,19,21,  0},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,24,26, 3},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,27,29, 0},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,2,4,	1},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,5,7,	0},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,9,11, 1},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,12,14, 0},
		{KME_DEHALO2_DH_OCCL_EXTEND_2_reg,4,5,	1},
		{KME_DEHALO2_DH_OCCL_EXTEND_2_reg,8,9, 1},
		{KME_DEHALO2_DH_OCCL_EXTEND_2_reg,12,14,   1},
		{KME_DEHALO2_DH_OCCL_EXTEND_3_reg,8,15, 32},
		{KME_DEHALO2_DH_OCCL_EXTEND_3_reg,24,31, 40},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,8,8, 1},
	},

	//PATTERN_181
	{
		//set enum id
		{PATTERN_181,0,0,0},
		
		// setting
		{HARDWARE_HARDWARE_57_reg, 14, 14, 0x0}, // u1_scCtrl_wrt_en;
		{KME_ME1_TOP1_ME1_SCENE1_00_reg, 9, 13, 0x0},
	},

	//PQTTERN_214
	//modify setting too long
	{
		//set enum id
		{PQTTERN_214,0,0,0},
		
		// setting
		{KME_DEHALO5_PHMV_FIX_0_reg,28,29,0x1}, //pick bg mv
		{KME_DEHALO5_PHMV_FIX_2_reg,4,4,0x1},  //penalty_en
		{KME_DEHALO5_PHMV_FIX_29_reg,24,24,0x1}, //divide en
		{KME_DEHALO5_PHMV_FIX_2B_reg,8,15,0x4F}, //phase_th1
		{KME_DEHALO5_PHMV_FIX_2C_reg,16,23,0x32}, //phase_th6
		{KME_DEHALO5_PHMV_FIX_2E_reg,0,7,0x23}, //phase th12
		{KME_DEHALO5_PHMV_FIX_5_reg,15,15,0x0},//pred_old_diff_cut_en
		{KME_DEHALO5_PHMV_FIX_38_reg,0,0,0x0}, //bad pred diff cut en
		{KME_DEHALO5_PHMV_FIX_37_reg,6,6,0x0}, //bad pred cut en
		{KME_DEHALO5_PHMV_FIX_5_reg,24,29,0x29}, //bad old cov
		{KME_DEHALO5_PHMV_FIX_5_reg,16,21,0x28}, //bad pred cov
		{KME_DEHALO5_PHMV_FIX_6_reg,16,21,0xe}, //pred sun cut th
		{KME_DEHALO5_PHMV_FIX_6_reg,8,13,0x29}, //bad old ucov
		{KME_DEHALO5_PHMV_FIX_6_reg,0,5,0x28}, //bad pred ucov
		{KME_DEHALO5_PHMV_FIX_8_reg,26,30,0x9}, //me1 pred th0
		{KME_DEHALO5_PHMV_FIX_1A_reg,26,31,0x0}, //correct cut
		{KME_DEHALO5_PHMV_FIX_1D_reg,10,13,0x0}, //extend cut
		{KME_DEHALO5_PHMV_FIX_1E_reg,16,22,0xa}, //extend ucov th0
		{KME_DEHALO5_PHMV_FIX_1E_reg,0,6,0xa},  //extend cov th0
		{KME_DEHALO5_PHMV_FIX_21_reg,8,15,0x19}, // color protect th max0
		{KME_DEHALO5_PHMV_FIX_1_reg,2,2,0x1}, // mv filter en
		{KME_DEHALO5_PHMV_FIX_3_reg,0,11,0x258}, //posdiff th1
		{KME_DEHALO5_PHMV_FIX_2_reg,18,26,0xdf}, //posdiff th2
		{KME_DEHALO5_PHMV_FIX_3_reg,16,24,0x1a4}, //posdiff th3
		{KME_DEHALO5_PHMV_FIX_1C_reg,0,6,0x57}, //useless bg th
		{KME_DEHALO5_PHMV_FIX_4_reg,16,20,0xe}, //filter cov th0
		{KME_DEHALO5_PHMV_FIX_4_reg,8,12,0x2}, //filter ucov cut
		{KME_DEHALO5_PHMV_FIX_4_reg,0,4,0x2}, //filter cov cut
		{KME_DEHALO5_PHMV_FIX_7_reg,8,12,0xe}, //filter ucov th0
		{KME_DEHALO5_PHMV_FIX_33_reg,2,2,0x1}, // ver en
		{KME_DEHALO5_PHMV_FIX_33_reg,8,9,0x1}, //ver y range
		{KME_DEHALO5_PHMV_FIX_35_reg,0,7,0x14},  //pred y2
		{KME_DEHALO5_PHMV_FIX_43_reg,6,6,0x0}, // blend
		{KME_DEHALO5_PHMV_FIX_35_reg,24,24,0x0},  //bg change en
		{KME_DEHALO5_PHMV_FIX_35_reg,18,18,0x0},  //bg sum en
		{KME_DEHALO5_PHMV_FIX_1_reg,15,15,0x1}, // extend cond4
		{KME_DEHALO5_PHMV_FIX_1F_reg,8,14,0x57},  //bg max
		{KME_DEHALO5_PHMV_FIX_1F_reg,0,6,0x54},  //bg min
		{KME_DEHALO5_PHMV_FIX_1C_reg,16,23, 0x14},
		{KME_DEHALO5_PHMV_FIX_1D_reg,16,25, 0x3ff},
		{KME_DEHALO5_PHMV_FIX_1C_reg,24,30, 0x64},
		{KME_DEHALO5_PHMV_FIX_10_reg,24,31, 0x3f},
#if 0 //modify setting too long
		{SOFTWARE1_SOFTWARE1_49_reg,31,31,0x0},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,0,1,0x3},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,2,4,0x4},        
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,5,7,0x1},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,8,8,0x1},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,9,11,0x4},  
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,12,14,0x1}, 
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,16,18,0x4},        
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,19,21,0x1},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,24,26,0x4},
		{KME_DEHALO2_DH_OCCL_EXTEND_0_reg,27,29,0x1},

		//fw init
		{KME_DEHALO5_PHMV_FIX_A_reg,10,11,0x2},
		{KME_DEHALO5_PHMV_FIX_A_reg,26,30,0x3}, 
		{KME_DEHALO5_PHMV_FIX_0_reg,20,20,0x1}, 
		{KME_DEHALO5_PHMV_FIX_1E_reg,8,14,0x5},
		{KME_DEHALO5_PHMV_FIX_1E_reg,24,30,0x5},
		{KME_DEHALO5_PHMV_FIX_37_reg,16,16,0x0},
		{KME_DEHALO5_PHMV_FIX_9_reg,16,25,0x4b},  
		{KME_DEHALO5_PHMV_FIX_1_reg,14,14,0x1}, 
		{KME_DEHALO5_PHMV_FIX_23_reg,16,16,0x1},
		{KME_DEHALO5_PHMV_FIX_23_reg,17,17,0x0},
		{KME_DEHALO5_PHMV_FIX_38_reg,6,6,0x0},
		{KME_DEHALO5_PHMV_FIX_8_reg,0,9,0x15},
		{KME_DEHALO5_PHMV_FIX_6_reg,14,14,0x1},
		{KME_DEHALO5_PHMV_FIX_6_reg,7,7,0x1},
		{KME_DEHALO5_PHMV_FIX_6_reg,6,6,0x1},
		{KME_DEHALO5_PHMV_FIX_1C_reg,16,23, 0x0},
		{KME_DEHALO5_PHMV_FIX_1C_reg,24,30, 0x57},
		{KME_DEHALO5_PHMV_FIX_F_reg,16,25, 0x12},
		{KME_DEHALO5_PHMV_FIX_11_reg,0,9, 0x23},
		{KME_DEHALO5_PHMV_FIX_11_reg,16,25, 0x3c}, 
		{KME_DEHALO5_PHMV_FIX_10_reg,24,31, 0x0},
		{KME_DEHALO5_PHMV_FIX_35_reg,25,30, 0x8},
		{KME_DEHALO5_PHMV_FIX_36_reg,10,15, 0x8},

		{HARDWARE_HARDWARE_24_reg , 19, 28, 0x0},
		{HARDWARE_HARDWARE_24_reg , 31, 31, 0x0},    

		{HARDWARE_HARDWARE_26_reg,31,31, 0x0}, //u8_dh_condition_dhbypass
#endif
	},

	//PATTERN_45
	{
		//set enum id
		{PATTERN_45,0,0,0},
		
		// setting
		{KME_DEHALO5_PHMV_FIX_1_reg,14,14,0x0},//reg_dh_bg_extend_cond3_en 45
		{KME_DEHALO5_PHMV_FIX_1_reg,1,1,0x1},//reg_dh_pstflt_occl_sel 45
		{KME_DEHALO5_PHMV_FIX_1_reg,0,0,0x1},//reg_dh_pstflt_mv_sel 45
		{KME_DEHALO5_PHMV_FIX_21_reg,8,15,0x64},  //reg_dh_color_protect_th0_max 45
		{KME_DEHALO5_PHMV_FIX_35_reg,25,30,0x6},  //reg_dh_pred_bg_change_pfv_th 45
		{KME_DEHALO5_PHMV_FIX_36_reg,10,15,0x6},  //reg_dh_pred_bg_change_ppfv_th 45
		{KME_DEHALO5_PHMV_FIX_38_reg,11,11,0x1},  //reg_dh_fg_r_big_en 45
		{KME_DEHALO5_PHMV_FIX_38_reg,9,9,0x1},	//reg_dh_fg_l_big_en 45
		{KME_DEHALO5_PHMV_FIX_38_reg,7,7,0x1},	//reg_dh_fg_c_big_en 45
		{KME_DEHALO5_PHMV_FIX_38_reg,6,6,0x1},	//reg_dh_fg_protect_by_apl_en 45
		{KME_DEHALO5_PHMV_FIX_3B_reg,24,31,0xf},  //reg_dh_fg_lr_big_min_cur_th 45
		{KME_DEHALO5_PHMV_FIX_3B_reg,16,23,0xf},  //reg_dh_fg_lr_big_min_pre_th 45
		{KME_DEHALO5_PHMV_FIX_3B_reg,8,15,0xf},  //reg_dh_fg_lr_big_max_cur_th 45
		{KME_DEHALO5_PHMV_FIX_3B_reg,0,7,0xf},	//reg_dh_fg_lr_big_max_pre_th 45
		{KME_DEHALO5_PHMV_FIX_43_reg,8,13,0x5},  //reg_dh_fg_me2_sim_max_diff_th 45
		{KME_DEHALO5_PHMV_FIX_43_reg,0,5,0x5},	//reg_dh_fg_me2_sim_min_diff_th 45
		
		{HARDWARE_HARDWARE_24_reg , 19, 28, 0x0},
		{HARDWARE_HARDWARE_24_reg , 31, 31, 0x0},	
	},

	//++Rotation_Guanyin
	//ROTATION_GUANYIN
	{
		//set enum id
		{ROTATION_GUANYIN,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_24_reg, 28, 28, 1},//Rotate_Windmill_Detect's u1_RotatAlgo_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 14, 14, 1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 13, 13, 1},
		{KME_VBUF_TOP_KME_VBUF_TOP_20_reg, 6,  8,	7},
		{KME_VBUF_TOP_KME_VBUF_TOP_20_reg, 9, 11,	1},
		{KME_VBUF_TOP_KME_VBUF_TOP_20_reg,18, 20,	1},
		{KME_VBUF_TOP_KME_VBUF_TOP_20_reg,21, 23,	1},
		{KME_VBUF_TOP_KME_VBUF_TOP_2C_reg, 6,  8,	7},
		{KME_VBUF_TOP_KME_VBUF_TOP_2C_reg, 9, 11,	1},
		{KME_VBUF_TOP_KME_VBUF_TOP_2C_reg,18, 20,	1},
		{KME_VBUF_TOP_KME_VBUF_TOP_2C_reg,21, 23,	1},
	},

	//--#30016-hor-dispatch2
	//HOR_DISPATCH2
	{
		//set enum id
		{HOR_DISPATCH2,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},
		
		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},
	
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0x11111111},
	
		{KME_ME1_TOP1_ME1_COMMON1_10_reg, 8,27,0xfffff},
		{KME_ME1_TOP1_ME1_COMMON1_11_reg, 0,29,0x3fffffff},
		{KME_ME1_TOP1_ME1_COMMON1_12_reg, 0,31,0xfffcfffc},
		
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_30_reg,0,4,0x0},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_30_reg,8,12,0x0},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_34_reg,3,7,0x0},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_34_reg,11,15,0x0},
	
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,0,16,0xe96d},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,0,15,0x7c94},
		{KME_VBUF_TOP_KME_VBUF_TOP_20_reg,0,23,0xe413ff},
		{KME_VBUF_TOP_KME_VBUF_TOP_2C_reg,0,23,0xe413ff},
	
		{KME_LOGO0_KME_LOGO0_AC_reg, 16, 23, 255},
		{KME_LOGO0_KME_LOGO0_AC_reg, 24, 31, 255},
		{KME_LOGO0_KME_LOGO0_B0_reg,  0,  7, 255},
		{KME_LOGO0_KME_LOGO0_B0_reg,  8, 15, 255},
		{KME_LOGO0_KME_LOGO0_B0_reg, 16, 23, 255},
	
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0},
	},

	//djntest38 soccer
	//DJNTEST38_SOCCER_1
	{
		//set enum id
		{DJNTEST38_SOCCER_1,0,0,0},
		
		// setting
		{KME_ME1_BG1_ME_BG_DUMMY_2_reg,20 ,20 , 1},
	},

	//djntest38 soccer
	//DJNTEST38_SOCCER_2
	{
		//set enum id
		{DJNTEST38_SOCCER_2,0,0,0},
		
		// setting
		{KME_ME1_TOP10_PRED_MV_CAND_1_reg,28,28,0x1},
	},

	//--#24007 46s
	//PATTERN_24007_46S
	{
		//set enum id
		{PATTERN_24007_46S,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0}, // local FB
		{MC_MC_28_reg,14,14,0x1},			// local FB
		{MC_MC_28_reg,15,22,0x0},			// local FB
		{SOFTWARE_SOFTWARE_10_reg, 0, 0, 0},
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
		{KME_ME1_TOP1_ME1_SCENE1_07_reg,0,30,0x7fffffff},
		{KME_ALGO_ME1_MEDFLT_CTRL_00_reg,18,18,0x0},
		{KME_ME2_CALC0_KME_ME2_CALC0_B0_reg,0,0,0x0},
	
		{KME_IPME_KME_IPME_8C_reg, 0, 26, 0x502814},
		{KME_IPME_KME_IPME_90_reg, 0, 26, 0x4c2814},
		{KME_IPME_KME_IPME_94_reg, 0, 26, 0x502813},
		{KME_IPME_KME_IPME_98_reg, 0, 26, 0x502814},
		{KME_IPME_KME_IPME_04_reg, 0, 17, 0x2613},
	
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0x57575757},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0x57575757},
	},

	//LALALAND_1_14_25
	{
		//set enum id
		{LALALAND_1_14_25,0,0,0},
		
		// setting
		{IPPRE_IPPRE_04_reg, 4, 11, 167},//kmc_blend_y_alpha
	},

	//MULTIBAND_PEAKING_CLOSE
	{
		//set enum id
		{MULTIBAND_PEAKING_CLOSE,0,0,0},
		
		// setting
		{COLOR_MB_PEAKING_MB_PEAKING_CTRL_reg, 0, 0, 0},//  Multiband peaking
		{COLOR_MB_PEAKING_MB_Double_Buffer_CTRL_reg, 0, 0, 1},//  Multiband peaking DB apply
	},

	//SHARPNESS_CLOSE_MULTIBAND_PEAKING_CLOSE
	{
		//set enum id
		{SHARPNESS_CLOSE_MULTIBAND_PEAKING_CLOSE,0,0,0},
		
		// setting
		{COLOR_SHARP_DM_NR_SHP_CTRL_reg, 0, 0, 0},//  sharpness peaking
		{COLOR_SHARP_SHP_DLTI_CTRL_1_reg, 0, 0, 0},//  sharpness dlti
		{COLOR_MB_PEAKING_MB_PEAKING_CTRL_reg, 0, 0, 0},//  Multiband peaking
		{COLOR_MB_PEAKING_MB_Double_Buffer_CTRL_reg, 0, 0, 1},//  Multiband peaking DB apply
	},

	//TEST_FILM32
	{
		//set enum id
		{TEST_FILM32,0,0,0},
		
		// setting
		{MC_MC_B0_reg, 8, 10, 0x4},
		{KME_TOP_KME_TOP_04_reg,30, 30, 0x1},
	},

	//ART_OF_FLIGHT_18_27
	{
		//set enum id
		{ART_OF_FLIGHT_18_27,0,0,0},
		
		// setting
		{IPPRE_IPPRE_04_reg, 4, 11, 252},
		{IPPRE_IPPRE_04_reg, 16, 23, 0xa0},
		{IPPRE_IPPRE_04_reg, 24, 25, 0x1},
	},

	//Y_ALPHA_252_UV_ALPHA_160_MODE_1
	{
		//set enum id
		{Y_ALPHA_252_UV_ALPHA_160_MODE_1,0,0,0},
		
		// setting
		{IPPRE_IPPRE_04_reg, 4, 11, 252},
		{IPPRE_IPPRE_04_reg, 16, 23, 0xa0},
		{IPPRE_IPPRE_04_reg, 24, 25, 0x1},
	},

	//Y_ALPHA_176
	{
		//set enum id
		{Y_ALPHA_176,0,0,0},
		
		// setting
		{IPPRE_IPPRE_04_reg, 4, 11, 0xb0},
	},

	//Y_ALPHA_252
	{
		//set enum id
		{Y_ALPHA_252,0,0,0},
		
		// setting
		{IPPRE_IPPRE_04_reg, 4, 11, 252},
	},

	//MSPG PATTER 7 for MEMC
	//MSPG_PATTERN_7
	{
		//set enum id
		{MSPG_PATTERN_7,0,0,0},
		
		// setting
		{IPPRE_IPPRE_0C_reg, 0, 7, 0x30},

		{KME_IPME_KME_IPME_8C_reg, 0, 8, 0x28},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 0x24},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 0x20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 0x18},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 0x14},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 0xa},
		{KME_IPME_KME_IPME_94_reg, 0, 8, 0x8},

		{KME_IPME_KME_IPME_94_reg, 9, 17, 0x28},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 0x24},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 0x20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 0x18},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 0x14},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 0xa},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 0x8},

		{KME_VBUF_TOP_KME_VBUF_TOP_3C_reg,0,8, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_3C_reg,9,15, 0},
#if (IC_K5LP || IC_K6LP || IC_K8LP)
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_E0_reg,0,7,0},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_E0_reg,8,14,0},
#endif
	},

	//--#8004_脱口秀大会 0s
	//LFB_CLOSE_BLKLOGO_GRDTH_255_255_255_255
	{
		//set enum id
		{LFB_CLOSE_BLKLOGO_GRDTH_255_255_255_255,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0}, 	// local FB
		{MC_MC_28_reg,14,14,0x1},				// local FB
		{MC_MC_28_reg,15,22,0x0},				// local FB
		{KME_LOGO0_KME_LOGO0_64_reg,0,7,255},
		{KME_LOGO0_KME_LOGO0_64_reg,8,15,255},
		{KME_LOGO0_KME_LOGO0_64_reg,16,23,255},
		{KME_LOGO0_KME_LOGO0_64_reg,24,31,255},
	},

	//--#C002samll object handing
	//C002
	{
		//set enum id
		{C002,0,0,0},
		
		// setting
		{KME_ME1_TOP10_PRED_MV_CAND_1_reg,28,28,0x1},
		{KME_VBUF_TOP_KME_VBUF_TOP_1C_reg,  6,  8,	1},   
		{KME_VBUF_TOP_KME_VBUF_TOP_1C_reg,  9, 11,	1}, 
		{KME_VBUF_TOP_KME_VBUF_TOP_1C_reg, 12, 14,	2},   
		{KME_VBUF_TOP_KME_VBUF_TOP_1C_reg, 15, 17,	2},
		{KME_ME1_TOP0_KME_ME1_TOP0_20_reg, 16, 23,	0},   
		{KME_ME1_TOP0_KME_ME1_TOP0_20_reg, 24, 31,	0},   
		
		{KME_VBUF_TOP_KME_VBUF_TOP_28_reg,  6,  8,	1},   
		{KME_VBUF_TOP_KME_VBUF_TOP_28_reg,  9, 11,	1}, 
		{KME_VBUF_TOP_KME_VBUF_TOP_28_reg, 12, 14,	2},   
		{KME_VBUF_TOP_KME_VBUF_TOP_28_reg, 15, 17,	2},
		{KME_ME1_TOP0_KME_ME1_TOP0_60_reg, 16, 23,	0},   
		{KME_ME1_TOP0_KME_ME1_TOP0_60_reg, 24, 31,	0}, 
		
		{KME_DEHALO5_PHMV_FIX_35_reg,22,22,1},
		{IPPRE_IPPRE_04_reg, 16, 23, 100},  //uv_alpha

		{MC_MC_B4_reg, 16, 23, 240},
		{MC_MC_B4_reg, 24, 31, 240},
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0xffffffff},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0xffffffff},
	},

	//--#broken subtitle 霹靂嬌娃_伊斯坦堡
	//BLKLOGO_ONLY_LOGO_PSTEP_2_10_15
	{
		//set enum id
		{BLKLOGO_ONLY_LOGO_PSTEP_2_10_15,0,0,0},
		
		// setting
		{KME_LOGO0_KME_LOGO0_E4_reg, 0, 3,0x2},
		{KME_LOGO0_KME_LOGO0_E4_reg, 4, 7,0xa},
		{KME_LOGO0_KME_LOGO0_E4_reg, 8,11,0xf},
		{MC2_MC2_20_reg,6,9,2},
		{LBMC_LBMC_88_reg,24,25,2},
	},
	
	//--#broken subtitle 霹靂嬌娃 1:01:00
	//BLKLOGO_ONLY_LOGO_NSTEP_1_1_1
	{
		//set enum id
		{BLKLOGO_ONLY_LOGO_NSTEP_1_1_1,0,0,0},
		
		// setting
		{KME_DEHALO2_KME_DEHALO2_24_reg, 6, 8,7},
		{KME_DEHALO2_KME_DEHALO2_24_reg,22,24,7},
		{KME_LOGO0_KME_LOGO0_D8_reg, 0, 3,0x1},
		{KME_LOGO0_KME_LOGO0_D8_reg, 4, 7,0x1},
		{KME_LOGO0_KME_LOGO0_D8_reg, 8,11,0x1},
		{MC2_MC2_20_reg,6,9,2},
		{LBMC_LBMC_88_reg,24,25,2},
	},

	//--#土耳其 logo
	//SC_TH_0xFFFF
	{
		//set enum id
		{SC_TH_0xFFFF,0,0,0},
		
		// setting
		{KME_ME1_TOP1_ME1_SCENE1_07_reg,0,30,0xffff},
	},

	//--#24004 19s
	//FLP_ALP_MOST_SMOOTH_RANDMASK_MOST_NEAR_BLKLOGO_CLOSE_DEHALO_CONDTION_CLOSE_RANCAND_CLOSE
	{
		//set enum id
		{FLP_ALP_MOST_SMOOTH_RANDMASK_MOST_NEAR_BLKLOGO_CLOSE_DEHALO_CONDTION_CLOSE_RANCAND_CLOSE,0,0,0},
		
		// setting
		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},
	
		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},
		
		{KME_ME1_BG1_ME_BG_S4_4_reg, 24, 24, 0x1},
		{KME_ME1_BG1_ME_BG_S4_3_reg, 0, 7, 255},	 
		{KME_ME1_BG1_ME_BG_S4_3_reg, 8, 15, 230},
		{KME_DEHALO5_PHMV_FIX_5_reg, 15, 15, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 7, 7, 0},
		{KME_DEHALO5_PHMV_FIX_6_reg, 6, 6, 0},
		{KME_DEHALO5_PHMV_FIX_37_reg, 16, 16, 0},
		{KME_DEHALO5_PHMV_FIX_38_reg, 0, 0, 0},
	
		{HARDWARE_HARDWARE_24_reg, 28, 28, 0},
		{HARDWARE_HARDWARE_24_reg, 31, 31, 0},
		{HARDWARE_HARDWARE_24_reg, 30, 30, 0},
		{HARDWARE_HARDWARE_24_reg, 18, 18, 0},
	
		{KME_LOGO0_KME_LOGO0_14_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_18_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_1C_reg,0,31,0x0},
		{KME_LOGO0_KME_LOGO0_20_reg,0,31,0x0},
	
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0x11111111},

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,11,13,0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,10,12,0x0},
	},

	//--#猫妖传 2:33
	//LFB_LV0_BLKSAMETH_RGN9_200
	{
		//set enum id
		{LFB_LV0_BLKSAMETH_RGN9_200,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0}, // local FB
		{MC_MC_28_reg,14,14,0x1},			// local FB
		{MC_MC_28_reg,15,22,0x0},			// local FB
		{KME_LOGO0_KME_LOGO0_AC_reg,8,15,200},
	},

	//Telop_Protection_J-Dorama1
	//--#24002 0s
	//list 272《0017_Urban_Acrobat_24fps.m2ts》
	//FORCE_VIDEO
	{
		//set enum id
		{FORCE_VIDEO,0,0,0},
		
		// setting
		{SOFTWARE1_SOFTWARE1_12_reg, 0, 3, 0},
		{SOFTWARE1_SOFTWARE1_12_reg, 4, 4, 1},
	},

	//--#402 35s
	//BOT_ROW_BLKSAMETH_100
	{
		//set enum id
		{BOT_ROW_BLKSAMETH_100,0,0,0},
		
		// setting
		{KME_LOGO0_KME_LOGO0_AC_reg, 16, 23, 100},
		{KME_LOGO0_KME_LOGO0_AC_reg, 24, 31, 100},
		{KME_LOGO0_KME_LOGO0_B0_reg, 0, 7, 100},
		{KME_LOGO0_KME_LOGO0_B0_reg, 8, 15, 100},
		{KME_LOGO0_KME_LOGO0_B0_reg, 16, 23, 100},
	},

	//--#25003 95s
	//DEHALO_CLOSE_LFB_LV0_CLOSE_DH_DETECT
	{
		//set enum id
		{DEHALO_CLOSE_LFB_LV0_CLOSE_DH_DETECT,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
		{MC_MC_28_reg,14,14,0x1},
		{MC_MC_28_reg,15,22,0x0},
		{HARDWARE_HARDWARE_06_reg,0,1,0x0},
	},

	//--#60015 5s
	//SC_HOLD_CNT_PRE_CNT
	{
		//set enum id
		{SC_HOLD_CNT_PRE_CNT,0,0,0},
		
		// setting
		{KME_ME1_TOP1_ME1_SCENE1_01_reg,0,31,0x842108},
		{KME_ME1_TOP1_ME1_SCENE1_02_reg,0,31,0x2040803},
	},

	//--#3006 5s
	//FLP_ALP_MOST_SMOOTH_RANDMASK_MOST_FAR_DEHALO_CLOSE_LFB_LV0_CAND_CLOSE
	{
		//set enum id
		{FLP_ALP_MOST_SMOOTH_RANDMASK_MOST_FAR_DEHALO_CLOSE_LFB_LV0_CAND_CLOSE,0,0,0},
		
		// setting
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 0, 0, 0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 1, 1, 0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 3, 3, 0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 5, 5, 0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 6, 6, 0x0},

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 1, 1, 0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 2, 2, 0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 4, 4, 0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 7, 7, 0x0},

		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},

		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg,14,14,0x1},
		{MC_MC_28_reg,15,22,0x0},

		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},

		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},

		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0xffffffff},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0xffffffff},
	},

	//C001 13s
	//DEHALO_CLOSE_BLKSAMETH_RGN4_90_LOGO_PSTEP_0_1_2
	{
		//set enum id
		{DEHALO_CLOSE_BLKSAMETH_RGN4_90_LOGO_PSTEP_0_1_2,0,0,0},
		
		// setting
		{KME_LOGO0_KME_LOGO0_A8_reg,0,7,90},
		{KME_LOGO0_KME_LOGO0_E4_reg,0,3,0},
		{KME_LOGO0_KME_LOGO0_E4_reg,4,7,1},
		{KME_LOGO0_KME_LOGO0_E4_reg,8,11,2},
		{KME_DEHALO5_PHMV_FIX_35_reg,28,28,1},
	},

	//--#spiderman 17:03
	//GFB_LV0_LFB_LV0_DEHALO_CLOSE_RIM_CLOSE
	{
		//set enum id
		{GFB_LV0_LFB_LV0_DEHALO_CLOSE_RIM_CLOSE,0,0,0},
		
		// setting
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg,14,14,0x1},
		{MC_MC_28_reg,15,22,0x0},
		{SOFTWARE_SOFTWARE_10_reg, 0, 0, 0},
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
		{SOFTWARE_SOFTWARE_46_reg,3,3,0},
	},

	// VTS_02_1
	//VTS_SETTING
	{
		//set enum id
		{VTS_SETTING,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_63_reg, 31, 31, 0},  //diable repeat broken set
	
		{KMC_BI_bi_top_01_reg, 0, 0, 0},	//FB
		{MC_MC_28_reg, 14, 14, 1},	//FB
		{MC_MC_28_reg, 15, 22, 0},	//FB
		{SOFTWARE_SOFTWARE_10_reg, 0, 0, 0},		//FB
	
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0xffffffff},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0xffffffff},
	
		{KME_IPME_KME_IPME_8C_reg,	0,	8, 20}, //apl 0 //20
		{KME_IPME_KME_IPME_8C_reg,	9, 17, 20}, //apl 1 //20
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20}, //apl 2 //20
		{KME_IPME_KME_IPME_90_reg,	0,	8, 20}, //apl 3 //20
		{KME_IPME_KME_IPME_90_reg,	9, 17, 20}, //apl 4 //20
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19}, //apl 5 //19
		{KME_IPME_KME_IPME_94_reg,	0,	8, 19}, //apl 6 //19
	
		{KME_IPME_KME_IPME_94_reg,	9, 17, 20}, //apl 0 //20
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20}, //apl 1 //20s
		{KME_IPME_KME_IPME_98_reg,	0,	8, 20}, //apl 2 //20
		{KME_IPME_KME_IPME_98_reg,	9, 17, 20}, //apl 3 //20
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20}, //apl 4 //20
		{KME_IPME_KME_IPME_04_reg,	0,	8, 19}, //apl 5 //19
		{KME_IPME_KME_IPME_04_reg,	9, 17, 19}, //apl 6 //19
	
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 14, 14, 1},//en
		{KME_VBUF_TOP_KME_VBUF_TOP_18_reg,	0, 29, 0x21b2c36},
		{KME_VBUF_TOP_KME_VBUF_TOP_1C_reg,	0, 17, 0x12416},
		{KME_VBUF_TOP_KME_VBUF_TOP_20_reg,	0, 23, 0x4b25b6},
	
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 13, 13, 1},//en
		{KME_VBUF_TOP_KME_VBUF_TOP_24_reg,	0, 29, 0x21b2c36},
		{KME_VBUF_TOP_KME_VBUF_TOP_28_reg,	0, 17, 0x12416},
		{KME_VBUF_TOP_KME_VBUF_TOP_2C_reg,	0, 23, 0x4b25b6},
		
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_4C_reg,	8, 25, 0x2cb6},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_60_reg,	0, 11, 0x496},
	
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_84_reg,	7, 24, 0x6496},//ME2 offset
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_98_reg,	0, 11, 0xcb6},
	
		{HARDWARE_HARDWARE_57_reg, 9,  9, 0x0},
		{KME_ME2_CALC0_KME_ME2_CALC0_DC_reg, 0, 0, 0},	 //me2_fst_sobj
		{KME_ME2_CALC0_KME_ME2_CALC0_D4_reg, 16, 16, 0}, //me2_sec_sobj
		{MC_MC_B4_reg, 16, 31, 0}, //mc_bld_mvd_y_max
		{MC_MC_B0_reg, 16, 18, 2}, //mc_bld_w_type
	},

	// santorini 3:07
	//SANTORINI_3m07
	{
		//set enum id
		{SANTORINI_3m07,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_63_reg, 31, 31, 0},  //diable repeat broken set
		
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 14, 14, 1},//en
		{KME_VBUF_TOP_KME_VBUF_TOP_20_reg, 18, 20, 1},
		{KME_VBUF_TOP_KME_VBUF_TOP_20_reg, 21, 23, 0},
	
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 13, 13, 1},//en
		{KME_VBUF_TOP_KME_VBUF_TOP_2C_reg, 18, 20, 7},
		{KME_VBUF_TOP_KME_VBUF_TOP_2C_reg, 21, 23, 0},
	
		{KME_ME1_TOP0_KME_ME1_TOP0_24_reg, 16, 23, 32},
		{KME_ME1_TOP0_KME_ME1_TOP0_24_reg, 24, 31, 32},
		{KME_ME1_TOP0_KME_ME1_TOP0_28_reg,	0,	7, 32},
		{KME_ME1_TOP0_KME_ME1_TOP0_28_reg,	8, 15, 32},
	
		{KME_ME1_TOP0_KME_ME1_TOP0_64_reg, 16, 23, 32},
		{KME_ME1_TOP0_KME_ME1_TOP0_64_reg, 24, 31, 32},
		{KME_ME1_TOP0_KME_ME1_TOP0_68_reg,	0,	7, 32},
		{KME_ME1_TOP0_KME_ME1_TOP0_68_reg,	8, 15, 32},
	
		{IPPRE_IPPRE_04_reg,  4, 11, 255},
		{IPPRE_IPPRE_04_reg, 16, 23, 4},
	},

	//15-HD_H_R
	//HD_H_R_GOLF
	{
		//set enum id
		{HD_H_R_GOLF,0,0,0},
		
		// setting
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0x37373737},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0x37373737},
		
		{KME_ME1_TOP0_KME_ME1_TOP0_24_reg, 16, 23, 0},//me1_IP cand update0 pnt
		{KME_ME1_TOP0_KME_ME1_TOP0_24_reg, 24, 31, 0},//me1_IP cand update1 pnt
		{KME_ME1_TOP0_KME_ME1_TOP0_28_reg,  0,  7, 0},//me1_IP cand update2 pnt

		{KME_ME1_TOP0_KME_ME1_TOP0_64_reg, 16, 23, 0},//me1_PI cand update0 pnt
		{KME_ME1_TOP0_KME_ME1_TOP0_64_reg, 24, 31, 0},//me1_PI cand update1 pnt
		{KME_ME1_TOP0_KME_ME1_TOP0_68_reg,  0,  7, 0},//me1_PI cand update2 pnt
	},

	//6009 to 6010
	//DYNAMIC_SEARCH_RANGE_CLOSE_MVX_RANGE_0
	{
		//set enum id
		{DYNAMIC_SEARCH_RANGE_CLOSE_MVX_RANGE_0,0,0,0},
		
		// setting
		{HARDWARE_HARDWARE_36_reg, 0, 31, 0}, //close dynamic search range
		{KME_VBUF_TOP_KME_VBUF_TOP_3C_reg, 0,	8,	0},//vbuf_mvx_range
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_E0_reg,0,7,	0},//kme_me2_vbuf_mvx_clip_range
	},

	//白色景物
	//ENTER_CNTGAIN_15
	{
		//set enum id
		{ENTER_CNTGAIN_15,0,0,0},
		
		// setting
		{SOFTWARE_SOFTWARE_02_reg, 4, 7, 0xf}, //u4_enter_32_cntGain
	},

	//7006 bad editor
	//DYNAMIC_REPEAT_MID_LFB_LV0_DEHALO_CLOSE_UNDOMASK_CLOSE_ZMVCAND_EN
	{
		//set enum id
		{DYNAMIC_REPEAT_MID_LFB_LV0_DEHALO_CLOSE_UNDOMASK_CLOSE_ZMVCAND_EN,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_63_reg, 0, 0, 1}, //D9FC
		{SOFTWARE3_SOFTWARE3_63_reg, 1, 2, 2}, //D9FC
	
		{KME_ALGO_ME1_MEDFLT_CTRL_00_reg,18,18,0x0},

		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
		{KME_DEHALO5_PHMV_FIX_35_reg,22,22,0x1},

		{KMC_BI_bi_top_01_reg,0,0,0x0},		// local FB
		{MC_MC_28_reg,14,14,0x1},				// local FB
		{MC_MC_28_reg,15,22,0x0},				// local FB

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 16, 16, 1},//ME1_IP zmv enable
		{KME_ME1_TOP0_KME_ME1_TOP0_28_reg, 16, 28, 0},//ME1_IP zmv pnt
	},

	//7006 20s
	//DEHALO_CLOSE_ZMVCAND_EN
	{
		//set enum id
		{DEHALO_CLOSE_ZMVCAND_EN,0,0,0},
		
		// setting
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 16, 16, 1},//ME1_IP zmv enable
		{KME_ME1_TOP0_KME_ME1_TOP0_28_reg, 16, 28, 0},//ME1_IP zmv pnt
	},

	//ZiMuDouDong
	//ZIMUDOUDONG_SET
	{
		//set enum id
		{ZIMUDOUDONG_SET,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_63_reg, 0, 0, 1}, //D9FC
		{SOFTWARE3_SOFTWARE3_63_reg, 1, 2, 0}, //D9FC
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0,0x0},
		{KME_LOGO0_KME_LOGO0_D8_reg, 4,  7, 15}, //n step
		{KME_LOGO0_KME_LOGO0_D8_reg, 8, 11, 15}, //n step
		{KME_LOGO0_KME_LOGO0_E4_reg, 4,  7, 1},  //p step
		{KME_LOGO0_KME_LOGO0_E4_reg, 8, 11, 1},  //p step
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,	31, 0xffffffff},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,	31, 0xffffffff},
		{IPPRE_IPPRE_04_reg,16,23,128},
	},

	// 25003 runner
	//DYNAMIC_REPEAT_OFF_DEAHLO_CLOSE_Y_ALPHA_224_UV_ALPHA_255
	{
		//set enum id
		{DYNAMIC_REPEAT_OFF_DEAHLO_CLOSE_Y_ALPHA_224_UV_ALPHA_255,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_63_reg, 0, 0, 1}, //D9FC
		{SOFTWARE3_SOFTWARE3_63_reg, 1, 2, 0}, //D9FC
		{IPPRE_IPPRE_04_reg,  4, 11, 0xe0},  //y_alpha
		{IPPRE_IPPRE_04_reg, 16, 23, 0xff},  //uv_alpha
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0},
	},

	/*0014_24_10_52s halo*/
	/*0014 50S~57S halo*/
	//HALO_0014_24_10_52s
	{
		//set enum id
		{HALO_0014_24_10_52s,0,0,0},
		
		// setting
		{HARDWARE_HARDWARE_44_reg, 0, 0, 1},
	},

	/*7011三十而已123_1.ts 20s*/
	//SADSHIHERYI_123_1
	{
		//set enum id
		{SADSHIHERYI_123_1,0,0,0},
		
		// setting
		{MC_MC_28_reg, 14, 14, 0},
		{MC_MC_28_reg, 15, 22, 0},
		{KME_ME1_TOP1_ME1_COMMON1_10_reg, 8, 17, 0},
		{KME_ME1_TOP1_ME1_COMMON1_10_reg,18, 27, 0},
		{KME_ME1_TOP1_ME1_COMMON1_12_reg, 16, 31, 5},
		{KME_TOP_KME_TOP_04_reg, 29, 29, 1},
	},

	//babai zimu logo halo
	//BABAI_ZIMU_LOGO_HALO
	{
		//set enum id
		{BABAI_ZIMU_LOGO_HALO,0,0,0},
		
		// setting
		{SOFTWARE3_SOFTWARE3_20_reg,1, 1 , 0x1},
	},

	//鷹之力 播放片源，画面下方字幕抖动
	//DEHALO_CLOSE_ME1_ME2_ZMV_CAND_EN
	{
		//set enum id
		{DEHALO_CLOSE_ME1_ME2_ZMV_CAND_EN,0,0,0},
		
		// setting
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_30_reg, 16, 16, 1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 15, 15, 1},
		{KME_DEHALO5_PHMV_FIX_35_reg, 22, 22, 1},
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0},
	},

	//太空旅客(tai kong lyu ke)
	//DEHALO_CLOSE_ME1_ME2_ZMV_CAND_EN_GMV_CAND_CLOSE
	{
		//set enum id
		{DEHALO_CLOSE_ME1_ME2_ZMV_CAND_EN_GMV_CAND_CLOSE,0,0,0},
		
		// setting
		{KME_DEHALO5_PHMV_FIX_35_reg, 22, 22, 0x1},
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 15, 15, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 14, 14, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 16, 16, 1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 15, 15, 1},
	},

	//史莱姆破碎
	//DEHALO_OFF_FB_STYLE_CINEMA
	{
		//set enum id
		{DEHALO_OFF_FB_STYLE_CINEMA,0,0,0},
		
		// setting
		{KME_DEHALO5_PHMV_FIX_35_reg,22,22,0x1},
		{KME_DEHALO_KME_DEHALO_10_reg,00,00,0x0},
		{SOFTWARE2_SOFTWARE2_39_reg, 0, 3, FB_STYLE_CINEMA},
	},

	//24002_transporter_explosion_1920x1080_24fps.avi
	//DEHALO_OFF_FB_STYLE_MORE
	{
		//set enum id
		{DEHALO_OFF_FB_STYLE_MORE,0,0,0},
		
		// setting
		{KME_DEHALO5_PHMV_FIX_35_reg,22,22,0x1},
		{KME_DEHALO_KME_DEHALO_10_reg,00,00,0x0},
		{SOFTWARE2_SOFTWARE2_39_reg, 0, 3, FB_STYLE_MORE},
	},

	//FengChuiBanXia Episode 1 风吹半夏第一集
	//FENGCHUIBANXIA 
	{
		//set enum id
		{FENGCHUIBANXIA,0,0,0},
		
		// setting
		{SOFTWARE_SOFTWARE_46_reg, 0, 0, 0},
		{LBMC_LBMC_24_reg, 24, 27, 9},
		{LBMC_LBMC_24_reg, 28, 31, 9},
		{LBMC_LBMC_24_reg, 20, 23, 9},

		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg,3,3,0},
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg,5,5,0},

		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_30_reg, 16, 16, 1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 15, 15, 1},

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 16, 16, 1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 15, 15, 1},

		{KME_LOGO1_KME_LOGO1_A8_reg,  0,  3, 15  }, // km_logo_pxlhsty_pxldf_pstep_y0
		{KME_LOGO1_KME_LOGO1_A8_reg,  4,  7, 15  }, // km_logo_pxlhsty_pxldf_pstep_y1
		{KME_LOGO1_KME_LOGO1_A8_reg,  8,  11, 15  }, // km_logo_pxlhsty_pxldf_pstep_y2
	},

	//jujishou 1h26min_1k 狙击手_1h26min_1k
	//JUJISHOU_1H26MIN
	{
		//set enum id
		{JUJISHOU_1H26MIN,0,0,0},
		
		// setting
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 16, 16, 1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 15, 15, 1},

		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg,3,3,0},
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg,5,5,0},

		{KME_LOGO1_KME_LOGO1_A8_reg,  0,  3, 15  }, // km_logo_pxlhsty_pxldf_pstep_y0
		{KME_LOGO1_KME_LOGO1_A8_reg,  4,  7, 15  }, // km_logo_pxlhsty_pxldf_pstep_y1
		{KME_LOGO1_KME_LOGO1_A8_reg,  8,  11, 15  }, // km_logo_pxlhsty_pxldf_pstep_y2
	},

	//8009
	//DH_DTL_GAIN_EN_DTL_SHIFT_5_DTL_CURVE_Y1_128_RANDMASK_MOST_NEAR
	{
		//set enum id
		{DH_DTL_GAIN_EN_DTL_SHIFT_5_DTL_CURVE_Y1_128_RANDMASK_MOST_NEAR,0,0,0},
		
		// setting
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0},//FRC_TOP__KME_DEHALO__reg_dh_en
		{KME_DEHALO_KME_DEHALO_FC_reg, 23, 23, 1},//FRC_TOP__KME_DEHALO__dh_dtl_gain_en
		{KME_DEHALO_KME_DEHALO_FC_reg, 20, 22, 5},//FRC_TOP__KME_DEHALO__dh_fb_dtl_shift
		{KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 128},//FRC_TOP__KME_DEHALO__reg_dh_dtl_curve_y1
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31, 0x11111111},//reg_vbuf_pi_1st_lfsr_mask 0~3 x&y
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31, 0x11111111},//reg_vbuf_ip_1st_lfsr_mask 0~3 x&y
	},

	//25002 1m05s girl
	//PATTERN_25002_1m05s
	{
		//set enum id
		{PATTERN_25002_1m05s,0,0,0},
		
		// setting
		{KME_ME1_BG0_ME1_BG_11_reg,31,31, 0x1}, //bg_s3_cov_data_en
		{KME_ME1_BG0_ME1_BG_11_reg,30,30, 0x1}, //bg_s3_ucov_data_en
		{KME_ME1_BG1_ME_BG_RANGE_1_reg,8,15,240},
		{KME_ME1_BG1_ME_BG_RANGE_1_reg,24,31,135},
		{KME_DEHALO5_PHMV_FIX_35_reg,23,23,0x0},
		{KME_ME1_BG0_ME1_BG_3_reg, 0, 0, 0},
		{KME_ME1_BG0_ME1_BG_11_reg,0,0, 0x0},
		{KME_DEHALO5_PHMV_FIX_23_reg,16,16,0x0},
		{KME_ME1_BG0_ME1_BG_14_reg,26,26,0x0},
		{HARDWARE_HARDWARE_24_reg , 19, 28, 0x0},
		{HARDWARE_HARDWARE_24_reg , 31, 31, 0x0},
	},

	//0012_HOFD_24fps.mpg.txt
	//ME_STYLE_SLOW_FB_STYLE_CINEMA_UNDOMASK_CLOSE_LFB_LV0_GFB_MORE
	{
		//set enum id
		{ME_STYLE_SLOW_FB_STYLE_CINEMA_UNDOMASK_CLOSE_LFB_LV0_GFB_MORE,0,0,0},
		
		// setting
		{SOFTWARE2_SOFTWARE2_39_reg,  4,  7, ME_STYLE_SLOW},
		{SOFTWARE2_SOFTWARE2_39_reg,  0,  3, FB_STYLE_CINEMA},//FB_STYLE_CINEMA
		{KME_ALGO_ME1_MEDFLT_CTRL_00_reg, 18, 18, 0},
		
		//GFB by sad
		{SOFTWARE3_SOFTWARE3_57_reg, 8, 15, 31},
		{SOFTWARE3_SOFTWARE3_57_reg, 0,  7, 1},
		
		//sc table
		{SOFTWARE1_SOFTWARE1_25_reg, 24, 24, 1},
		{SOFTWARE1_SOFTWARE1_25_reg, 25, 27, 4},
		
		{KMC_BI_bi_top_01_reg,0,0,0x0}, 	// local FB
		{MC_MC_28_reg, 14, 14, 1},
		{MC_MC_28_reg, 15, 22, 255},
	},

	// #31 请叫我总监01-8分钟.mp4
	//ME_STYLE_SLOW_FB_STYLE_CINEMA_MEANDER_UP_TO_LOW_UNDO_MASK_CLOSE
	{
		//set enum id
		{ME_STYLE_SLOW_FB_STYLE_CINEMA_MEANDER_UP_TO_LOW_UNDO_MASK_CLOSE,0,0,0},
		
		// setting
		{KME_TOP_KME_TOP_08_reg, 2, 3, 1},//meander up to low
		{SOFTWARE2_SOFTWARE2_39_reg,  4,  7, ME_STYLE_SLOW},
		{SOFTWARE2_SOFTWARE2_39_reg,  0,  3, FB_STYLE_CINEMA},//FB_STYLE_CINEMA
		{KME_DEHALO6_dh_logo_ctrl_1_rgb_reg, 20, 25, 40},
		{KME_DEHALO6_dh_logo_ctrl_1_rgc_reg, 20, 25, 40},
		{KME_DEHALO6_dh_logo_ctrl_1_rgd_reg, 20, 25, 40},
		{KME_ALGO_ME1_MEDFLT_CTRL_00_reg, 18, 18, 0},
	},

	//1080_24HZ_Pendulum.mpg
	//RANDMASK_MOST_FAR_MEUPDSTGH_CLOSE
	{
		//set enum id
		{RANDMASK_MOST_FAR_MEUPDSTGH_CLOSE,0,0,0},
		
		// setting
		{HARDWARE_HARDWARE_57_reg, 9,	9, 0x0},
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0xffffffff},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0xffffffff},
	},

	//0011_HDHR_24fps.mpg
	//FLP_ALP_VERY_SHARP_LOGO_CLOSE
	{
		//set enum id
		{FLP_ALP_VERY_SHARP_LOGO_CLOSE,0,0,0},
		
		// setting
		{KME_VBUF_TOP_KME_VBUF_TOP_54_reg, 18, 18, 0},
		{KME_IPME_KME_IPME_8C_reg, 0, 8, 42},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 33},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 26},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 13},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 9},
		{KME_IPME_KME_IPME_94_reg, 0, 8, 7},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 42},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 33},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 26},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 19},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 13},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 9},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 7},
	},

	//60fps-402_Philips test sequence.mpg 15S~20S 36S HSR
	//RANDMASK_MOST_FAR_LOGO_CLOSE
	{
		//set enum id
		{RANDMASK_MOST_FAR_LOGO_CLOSE,0,0,0},
		
		// setting
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31, 0xffffffff},//reg_vbuf_pi_1st_lfsr_mask 0~3 x&y
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31, 0xffffffff},//reg_vbuf_ip_1st_lfsr_mask 0~3 x&y
		{KME_VBUF_TOP_KME_VBUF_TOP_54_reg, 18, 18, 0},
	},

	//VTS_02_1.VOB
	//RIM_V_SKETCHY_TH_0_PRECISE_TH_0
	{
		//set enum id
		{RIM_V_SKETCHY_TH_0_PRECISE_TH_0,0,0,0},
		
		// setting
		{BBD_BBD_INPUT_V_THRESHOLD_CTRL_reg,  0, 12, 0},
		{BBD_BBD_INPUT_V_THRESHOLD_CTRL_reg, 16, 28, 0},
	},

	// 0001_Satelite
	//GFB_LV0_RIMCOMP_DISABLE_SC_TB_2
	{
		//set enum id
		{GFB_LV0_RIMCOMP_DISABLE_SC_TB_2,0,0,0},
		
		// setting
		{HARDWARE_HARDWARE_43_reg, 5, 5, 1},//u1_RimComp_disable
		{SOFTWARE_SOFTWARE_10_reg, 0, 0, 0},//GFB diable

		{SOFTWARE1_SOFTWARE1_25_reg, 24, 24, 1}, //force scene change table
		{SOFTWARE1_SOFTWARE1_25_reg, 25, 27, 2}, //force scene change table
	},

	/*25002 50s man*/
	//PATTERN_25002_50s_MAN
	{
		//set enum id
		{PATTERN_25002_50s_MAN,0,0,0},
		
		// setting
		{KME_DEHALO5_PHMV_FIX_0_reg,12,12,0x0},  //reg_dh_only_mv_fix_en 45
		{KME_DEHALO5_PHMV_FIX_1_reg,14,14,0x0},//reg_dh_bg_extend_cond3_en 45
		{KME_DEHALO5_PHMV_FIX_1_reg,1,1,0x1},//reg_dh_pstflt_occl_sel 45
		{KME_DEHALO5_PHMV_FIX_1_reg,0,0,0x1},//reg_dh_pstflt_mv_sel 45
		{KME_DEHALO5_PHMV_FIX_21_reg,8,15,60},	//reg_dh_color_protect_th0_max 45 //ori 120
		{KME_DEHALO5_PHMV_FIX_35_reg,25,30,0x6},  //reg_dh_pred_bg_change_pfv_th 45
		{KME_DEHALO5_PHMV_FIX_36_reg,10,15,0x6},  //reg_dh_pred_bg_change_ppfv_th 45
		{KME_DEHALO5_PHMV_FIX_38_reg,11,11,0x1},  //reg_dh_fg_r_big_en 45
		{KME_DEHALO5_PHMV_FIX_38_reg,9,9,0x1},	//reg_dh_fg_l_big_en 45
		{KME_DEHALO5_PHMV_FIX_38_reg,7,7,0x1},	//reg_dh_fg_c_big_en 45
		{KME_DEHALO5_PHMV_FIX_38_reg,6,6,0x1},	//reg_dh_fg_protect_by_apl_en 45
		{KME_DEHALO5_PHMV_FIX_3B_reg,24,31,0xf},  //reg_dh_fg_lr_big_min_cur_th 45
		{KME_DEHALO5_PHMV_FIX_3B_reg,16,23,0xf},  //reg_dh_fg_lr_big_min_pre_th 45
		{KME_DEHALO5_PHMV_FIX_3B_reg,8,15,0xf},  //reg_dh_fg_lr_big_max_cur_th 45
		{KME_DEHALO5_PHMV_FIX_3B_reg,0,7,0xf},	//reg_dh_fg_lr_big_max_pre_th 45
		{KME_DEHALO5_PHMV_FIX_43_reg,8,13,0x5},  //reg_dh_fg_me2_sim_max_diff_th 45
		{KME_DEHALO5_PHMV_FIX_43_reg,0,5,0x5},	//reg_dh_fg_me2_sim_min_diff_th 45
		
		{HARDWARE_HARDWARE_24_reg , 19, 28, 0x0},
		{HARDWARE_HARDWARE_24_reg , 31, 31, 0x0},  
		
		{KME_DEHALO5_PHMV_FIX_5_reg,15,15,0},//pred_old_diff_cut
		{KME_DEHALO5_PHMV_FIX_6_reg,7,7,0},//bad_pred_cut
		{KME_DEHALO5_PHMV_FIX_6_reg,6,6,0},//bad_old_cut
		{KME_DEHALO5_PHMV_FIX_35_reg,24,24,0},//bg_change_cut
		{KME_DEHALO5_PHMV_FIX_38_reg,0,0,0},//bg_change_cut
		
		{KME_DEHALO5_PHMV_FIX_1E_reg,0,6,8},//cov more
		{KME_DEHALO5_PHMV_FIX_1E_reg,8,14,8},//cov less
		{KME_DEHALO5_PHMV_FIX_1E_reg,16,22,8},//ucov more
		{KME_DEHALO5_PHMV_FIX_1E_reg,24,30,8},//ucov less
		
		{KME_DEHALO5_PHMV_FIX_6_reg,16,21,0xa},//sum_cut_th
		
		{KME_DEHALO5_PHMV_FIX_2A_reg,4,13,300},//cond4 posdiff th
		
		{KME_DEHALO5_PHMV_FIX_4_reg,8,12,0x2},//occl cut 
		{KME_DEHALO5_PHMV_FIX_4_reg,0,4,0x2}, //occl cut
		
		{KME_ME1_BG0_ME1_BG_11_reg,31,31, 0x1},  //bg_s3_cov_data_en
		{KME_ME1_BG0_ME1_BG_11_reg,30,30, 0x1},  //bg_s3_ucov_data_en
		
		{KME_DEHALO5_PHMV_FIX_0_reg,13,13,0x0}, //logo protect
		{KME_DEHALO5_PHMV_FIX_1C_reg,8,14,30},	  //37	
	},

#if 0 //no use
	//#4
	/*4K.znds.com]ShutUpAndPlay.Berlin.2014.2160p.HDTV.HEVC-jTV-50p-3840x2160*/
	//0:40-0:43
	{
		{KME_DEHALO_KME_DEHALO_FC_reg, 23, 23, 0},
	},

	//#16
	/*taxi3_ntsc_n1.m2t*/
	/*The Art of Flight disk: 46"26 antenna broken when scene pans*/
	/*刘老根 第五步01片头后 15s~21s*/
	{
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0},//FRC_TOP__KME_DEHALO__reg_dh_en
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg, 14, 14, 1},//FRC_TOP__MC__reg_mc_fb_lvl_force_en
		{MC_MC_28_reg, 15, 22, 0},//FRC_TOP__MC__reg_mc_fb_lvl_force_value
		{KME_ME2_CALC0_KME_ME2_CALC0_D4_reg, 16, 16, 0},//FRC_TOP__KME_ME2_CALC0__reg_kme_me2_sec_small_object_en
		{KME_ME2_CALC0_KME_ME2_CALC0_DC_reg, 0, 0, 0},//FRC_TOP__KME_ME2_CALC0__reg_kme_me2_fst_small_object_en
	},

	//#17
	/*Happy_Gilmore_Longdrive_1080I60*/
	/*刘老根 第五步01片头后 15s~21s*/
	{
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 0},//FRC_TOP__KME_DEHALO__reg_dh_en
		{MC_MC_B4_reg, 16, 23, 255},//FRC_TOP__MC__reg_mc_bld_mvd_y_min
		{MC_MC_B4_reg, 24 ,31, 255},//FRC_TOP__MC__reg_mc_bld_mvd_y_max
		{MC_MC_B8_reg,11,11, 0},//FRC_TOP__MC__reg_mc_sobj_en_ADDR
	},

	//#18
	/*FRC_Spideman_hd_demo_04.mkv*/
	{
		{KME_DEHALO_KME_DEHALO_10_reg, 0, 0, 1},//FRC_TOP__KME_DEHALO__reg_dh_en
		{KME_DEHALO_KME_DEHALO_70_reg, 4, 4,1},//FRC_TOP__KME_DEHALO__dh_pfv_conf_gen_andor_fb_sel
		{KME_DEHALO_KME_DEHALO_B0_reg, 12, 19,0},//FRC_TOP__KME_DEHALO__reg_dh_occl_gen_sim_thr
		{KME_DEHALO_KME_DEHALO_D4_reg, 6, 11,63},//FRC_TOP__KME_DEHALO__dh_pfv_gen_force_dehalo_level
		{KME_DEHALO_KME_DEHALO_D4_reg, 18, 23,63},//FRC_TOP__KME_DEHALO__dh_pfv_gen_force_dehalo_level2
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg, 14, 14, 1},//FRC_TOP__MC__reg_mc_fb_lvl_force_en
		{MC_MC_28_reg, 15, 22, 95},//FRC_TOP__MC__reg_mc_fb_lvl_force_value
	},

	//#19
	//youtube: Above NYC-Filmed in 12K
	//playback 8K-Switzerland-24p-20180713.mp4(tv030)
	{	
		{KME_ME1_TOP0_KME_ME1_DRP_reg, 31, 31, 0x1},//FRC_TOP__KME_ME1_TOP0__me1_drp_en_ADDR
	},

	//#20
	/*00027.m2ts*/
	{
		{MC2_MC2_50_reg,0,1, 0},//FRC_TOP__MC2__reg_mc_var_lpf_en
	},

	//#22
	/*Midway.4khdr_13_Propeller*/
	{
		{MC2_MC2_20_reg, 0, 1, 0},//FRC_TOP__MC2__reg_mc_logo_en
		{KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 0x32},//FRC_TOP__KME_DEHALO__reg_dh_dtl_curve_y1
		{KME_DEHALO_KME_DEHALO_FC_reg, 20, 22, 6},//FRC_TOP__KME_DEHALO__dh_fb_dtl_shift
	},
	
	//#23
	/*Valley of fire_breakup.mp4*/	
	{
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,15,15,1},//reg_vbuf_ip_1st_gmv_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,16,16,1},//reg_vbuf_ip_1st_zmv_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,15,15,1},//reg_vbuf_pi_1st_gmv_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,16,16,1},//reg_vbuf_pi_1st_zmv_en
		{KME_ME1_TOP2_KME_ME1_TOP2_34_reg,0,0,0},//FRC_TOP__KME_ME1_TOP2__me1_acdc_pk_en
		{KME_ME2_CALC0_KME_ME2_CALC0_A4_reg,9,9,0},//FRC_TOP__KME_ME2_CALC0__me2_acdc_pk_en
	},

	//#24
	//Montage_M_ROT-720p50.mpg
	{
		//{FRC_TOP__PQL_0__pql_patch_dummy_e4_ADDR,7,7, 0},//Merlin7 does not have it
		{KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 0},//FRC_TOP__KME_DEHALO__reg_dh_dtl_curve_y1
		{KME_DEHALO_KME_DEHALO_FC_reg, 0, 7, 0x00},//FRC_TOP__KME_DEHALO__reg_dh_dtl_curve_y2_ADDR
		{KMC_BI_bi_top_01_reg,2,3, 0},//FRC_TOP__KMC_BI__bi_fb_mode_ADDR
		//{FRC_TOP__PQL_1__pql_wr_dhclose_wrt_en_ADDR,4,4, 0},//Merlin7 does not have it
		{KME_DEHALO_KME_DEHALO_BC_reg, 17, 22, 0x3f},//FRC_TOP__KME_DEHALO__reg_dh_pst_occl_type_thr_ADDR
		{MC_MC_B8_reg,11,11, 0},//FRC_TOP__MC__reg_mc_sobj_en_ADDR
		{MC_MC_B4_reg,16,23, 0x00},//FRC_TOP__MC__reg_mc_bld_mvd_y_min_ADDR
		{MC_MC_B4_reg,24,31, 0x00},//FRC_TOP__MC__reg_mc_bld_mvd_y_min_ADDR
		//{KIWI_REG(FRC_TOP__PQL_1__pql_fb_ctrl_en_ADDR), 0},//Merlin7 do not have it 
		{MC_MC_B0_reg, 20 ,27, 0},//FRC_TOP__MC__reg_mc_bld_flbk_lvl
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg, 14, 14, 1},//FRC_TOP__MC__reg_mc_fb_lvl_force_en
		{MC_MC_28_reg, 15, 22, 0},//FRC_TOP__MC__reg_mc_fb_lvl_force_value
	},

	//#25
	//Emperor_offical_trailer  33s
	{
		//	BG repeat detect
		//{FRC_TOP__PQL_0__pql_patch_dummy_e8_ADDR, 11, 11, 1},//Merlin7 does not have it
		//gmvd
		//pOutput->u1_casino_RP_detect_true =1;
		
		//	lfsr_mask
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31, 0x33333333},//reg_vbuf_pi_1st_lfsr_mask 0~3 x&y
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31, 0x33333333},//reg_vbuf_ip_1st_lfsr_mask 0~3 x&y
		// ME1 cddpnt rnd
		{KME_ME1_TOP0_KME_ME1_TOP0_24_reg, 16, 23, 0xff},//FRC_TOP__KME_ME1_TOP0__reg_me1_ip_cddpnt_rnd0_ADDR
		{KME_ME1_TOP0_KME_ME1_TOP0_24_reg, 24, 31, 0xff},//FRC_TOP__KME_ME1_TOP0__reg_me1_ip_cddpnt_rnd1_ADDR
		{KME_ME1_TOP0_KME_ME1_TOP0_28_reg, 0, 7,   0xff},//FRC_TOP__KME_ME1_TOP0__reg_me1_ip_cddpnt_rnd2_ADDR
		{KME_ME1_TOP0_KME_ME1_TOP0_64_reg, 16, 23, 0xff},//FRC_TOP__KME_ME1_TOP0__reg_me1_pi_cddpnt_rnd0_ADDR
		{KME_ME1_TOP0_KME_ME1_TOP0_64_reg, 24, 31, 0xff},//FRC_TOP__KME_ME1_TOP0__reg_me1_pi_cddpnt_rnd1_ADDR
		{KME_ME1_TOP0_KME_ME1_TOP0_68_reg, 0, 7,   0xff},//FRC_TOP__KME_ME1_TOP0__reg_me1_pi_cddpnt_rnd2_ADDR
		// ME1 adptpnt rnd curve
		{KME_ME1_TOP1_ME1_COMMON1_23_reg, 0, 5,  0x3f},//  FRC_TOP__KME_ME1_TOP1__reg_me1_adptpnt_rnd_y1_ADDR
		{KME_ME1_TOP1_ME1_COMMON1_23_reg, 8, 13, 0x3f},// FRC_TOP__KME_ME1_TOP1__reg_me1_adptpnt_rnd_y2_ADDR
		{KME_ME1_TOP1_ME1_COMMON1_23_reg, 14, 19,0x3f},// FRC_TOP__KME_ME1_TOP1__reg_me1_adptpnt_rnd_y3_ADDR
		{KME_ME1_TOP1_ME1_COMMON1_23_reg, 20, 25,0x3f},//FRC_TOP__KME_ME1_TOP1__reg_me1_adptpnt_rnd_y4_ADDR
	},

	//#26
	////Art of Flight 00:02:28
	//Art of Flight  2:23
	//Art of Flight 00:34 /2:23
	{
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 4, 4, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand3_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 5, 5, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand4_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 6, 6, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand5_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 7, 7, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand6_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 12, 12, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_update1_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 13, 13, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_update2_en
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_40_reg, 13, 16, 1},//FRC_TOP__KME_ME2_VBUF_TOP__reg_kme_me2_vbuf_lfsr_y_mask
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_40_reg, 9, 12, 1},//FRC_TOP__KME_ME2_VBUF_TOP__reg_kme_me2_vbuf_lfsr_x_mask
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31, 0x11111111},//reg_vbuf_pi_1st_lfsr_mask 0~3 x&y
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31, 0x11111111},//reg_vbuf_ip_1st_lfsr_mask 0~3 x&y
	},

	//#27
	/*Passion Cocoa Milk Buns*/ // standing at the middle
	{
		{KME_DEHALO_KME_DEHALO_FC_reg, 23, 23, 1},//FRC_TOP__KME_DEHALO__dh_dtl_gain_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 4, 4, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand3_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 5, 5, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand4_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 6, 6, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand5_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 7, 7, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand6_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 12, 12, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_update1_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 13, 13, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_update2_en
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_40_reg, 13, 16, 1},//FRC_TOP__KME_ME2_VBUF_TOP__reg_kme_me2_vbuf_lfsr_y_mask
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_40_reg, 9, 12, 1},//FRC_TOP__KME_ME2_VBUF_TOP__reg_kme_me2_vbuf_lfsr_x_mask
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31, 0x11111111},//reg_vbuf_pi_1st_lfsr_mask 0~3 x&y
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31, 0x11111111},//reg_vbuf_ip_1st_lfsr_mask 0~3 x&y
	},

	//#28
	//S&M saech on a hammock
	{
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 4, 4, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand3_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 5, 5, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand4_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 6, 6, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand5_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 7, 7, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_cand6_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 12, 12, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_update1_en
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 13, 13, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_1st_update2_en
		{KME_ME1_TOP0_KME_ME1_TOP0_2C_reg, 0, 12, 0},//FRC_TOP__KME_ME1_TOP0__reg_me1_ip_cddpnt_gmv
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 15, 15, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_zmv_en
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg, 28, 29, 3},//FRC_TOP__KME_ME2_VBUF_TOP__kme_me2_vbuf_1st_dc_obme_mode
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg, 30, 31, 3},//FRC_TOP__KME_ME2_VBUF_TOP__kme_me2_vbuf_2nd_dc_obme_mode
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg, 21, 22, 0},//FRC_TOP__KME_ME2_VBUF_TOP__kme_me2_vbuf_1st_ac_obme_mode
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg, 23, 24, 0},//FRC_TOP__KME_ME2_VBUF_TOP__kme_me2_vbuf_2nd_ac_obme_mode
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_DC_reg, 2, 2, 0},//FRC_TOP__KME_ME2_VBUF_TOP__kme_me2_vbuf_1st_dc_obme_mode_sel
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_DC_reg, 3, 3, 0},//FRC_TOP__KME_ME2_VBUF_TOP__kme_me2_vbuf_2nd_dc_obme_mode_sel
		{KME_VBUF_TOP_KME_VBUF_TOP_8C_reg, 18, 19, 3},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_dc_obme_mode
		{KME_VBUF_TOP_KME_VBUF_TOP_8C_reg, 20, 21, 3},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_dc_obme_mode
		{KME_VBUF_TOP_KME_VBUF_TOP_8C_reg, 22, 22, 0},//FRC_TOP__KME_VBUF_TOP__vbuf_ip_dc_obme_mode_sel
		{KME_VBUF_TOP_KME_VBUF_TOP_8C_reg, 23, 23, 0},//FRC_TOP__KME_VBUF_TOP__vbuf_pi_dc_obme_mode_sel
		{KME_VBUF_TOP_KME_VBUF_TOP_8C_reg, 24, 25, 1},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_ip_ac_obme_mode
		{KME_VBUF_TOP_KME_VBUF_TOP_8C_reg, 26, 27, 1},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_ac_obme_mode
	},

	//#31
	/*DNM_hrnm 720p60*/ //walking stick
	{
		{KMC_BI_bi_top_01_reg,0,0,0x0},
		{MC_MC_28_reg, 14, 14, 1},//FRC_TOP__MC__reg_mc_fb_lvl_force_en
		{MC_MC_28_reg, 15, 22, 0},//FRC_TOP__MC__reg_mc_fb_lvl_force_value
		{KME_ME1_TOP1_ME1_COMMON1_10_reg, 8, 17, 32},//FRC_TOP__KME_ME1_TOP1__reg_me1_mvd_dgain_y1
		{KME_ME1_TOP1_ME1_COMMON1_10_reg, 18, 27, 32},//FRC_TOP__KME_ME1_TOP1__reg_me1_mvd_dgain_y2
		{KME_ME1_TOP1_ME1_COMMON1_12_reg, 16, 31, 65530},//FRC_TOP__KME_ME1_TOP1__reg_me1_mvd_dgain_slope2
		{KME_TOP_KME_TOP_04_reg, 29, 29, 0},//FRC_TOP__KME_TOP__reg_frame_meander_enable
		{KME_ME1_TOP1_ME1_COMMON1_06_reg, 0, 9, 0},//FRC_TOP__KME_ME1_TOP1__reg_me1_mvd_cuv_x1
		{KME_ME1_TOP1_ME1_COMMON1_06_reg, 10, 19, 40},//FRC_TOP__KME_ME1_TOP1__reg_me1_mvd_cuv_x2
		{KME_ME1_TOP1_ME1_COMMON1_06_reg, 20 ,29, 120},//FRC_TOP__KME_ME1_TOP1__reg_me1_mvd_cuv_x3
		{KME_ME1_TOP1_ME1_COMMON1_07_reg, 0, 9, 0},//FRC_TOP__KME_ME1_TOP1__reg_me1_mvd_cuv_y1
		{KME_ME1_TOP1_ME1_COMMON1_07_reg, 10, 19, 21},//FRC_TOP__KME_ME1_TOP1__reg_me1_mvd_cuv_y2
		{KME_ME1_TOP1_ME1_COMMON1_07_reg, 20 ,29, 32},//FRC_TOP__KME_ME1_TOP1__reg_me1_mvd_cuv_y3
		{KME_ME1_TOP1_ME1_COMMON1_08_reg, 0 ,12, 34},//FRC_TOP__KME_ME1_TOP1__reg_me1_mvd_cuv_slope1
		{KME_ME1_TOP1_ME1_COMMON1_08_reg, 13, 25, 9},//FRC_TOP__KME_ME1_TOP1__reg_me1_mvd_cuv_slope2
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 0, 0, 1},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand0_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 1, 1, 1},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand1_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 2, 2, 1},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand2_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 3, 3, 1},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand3_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 4, 4, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand4_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 5, 5, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand5_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 6, 6, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand6_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 7, 7, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand7_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 8, 8, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand8_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 9, 9, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_cand9_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 10, 10, 1},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_update0_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 11, 11, 1},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_update1_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 12, 12, 1},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_update2_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 13, 13, 0},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_update3_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 14, 14, 1},//FRC_TOP__KME_VBUF_TOP__reg_vbuf_pi_1st_gmv_en
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31, 0x33333333},//reg_vbuf_pi_1st_lfsr_mask 0~3 x&y
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31, 0x33333333},//reg_vbuf_ip_1st_lfsr_mask 0~3 x&y
	},

	//#32
	/*7_Harry Potter and The Sorerer's Stone*/
	/*DNM_hrnm 720p60*/ //0:47 rolling-blade girl
	/*07_Montage_P*/ //0:48-0:51 rolling-blade girl
	{
		{COLOR_SHARP_DM_NR_SHP_CTRL_reg, 0, 0, 0},//  sharpness peaking
		{COLOR_SHARP_SHP_DLTI_CTRL_1_reg, 0, 0, 0},//  sharpness dlti
		{COLOR_MB_PEAKING_MB_PEAKING_CTRL_reg, 0, 0, 0},//  Multiband peaking
		{COLOR_MB_PEAKING_MB_Double_Buffer_CTRL_reg, 0, 0, 1},//  Multiband peaking DB apply
		{KME_DEHALO_KME_DEHALO_70_reg, 4, 4,1},//FRC_TOP__KME_DEHALO__dh_pfv_conf_gen_andor_fb_sel
		{KME_DEHALO_KME_DEHALO_B0_reg, 12, 19,0},//FRC_TOP__KME_DEHALO__reg_dh_occl_gen_sim_thr
		{KME_DEHALO_KME_DEHALO_D4_reg, 6, 11,63},//FRC_TOP__KME_DEHALO__dh_pfv_gen_force_dehalo_level
		{KME_DEHALO_KME_DEHALO_D4_reg, 18, 23,63},//FRC_TOP__KME_DEHALO__dh_pfv_gen_force_dehalo_level2
	},

#if 1 //setting from Identification_Pattern_wrtAction_TV001

	

	//repeat issue recognization (BTS WOSQRTK-15393)
	{
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,	31, 0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,	31, 0x11111111},
		//me2 dediff ph en
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_4C_reg,1,1, 0},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_84_reg,1,1, 0},
		
		//== zmv ==
		//me1
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 16, 16,	0},//ip_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 15, 15,	0},//pi_en		
		{KME_ME1_TOP0_KME_ME1_TOP0_28_reg, 16, 28, 0},//ip_pnt
		{KME_ME1_TOP0_KME_ME1_TOP0_68_reg, 16, 28, 0},//pi_pnt
		//me2
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_30_reg, 16, 16, 0},//1st
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_34_reg, 16, 16, 0},	//2nd		
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_60_reg, 24, 31, 0},//1st 	
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_9C_reg, 0, 7, 0},//2nd
		
		//==ip/pi cand==
		//ip
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 2, 2,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 3, 3,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 4, 4,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 6, 6,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 7, 7,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 8, 8,	0},
		//pi
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 2, 2,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 3, 3,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 4, 4,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 6, 6,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 7, 7,	0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 8, 8,	0},
				
		//== update cand ==
		//me1 ip
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,	13, 13,  0},
		//me1 pi
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,	12, 12,  0},
		//me2 1st
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_30_reg,	18, 23, 0},
		//me2 2nd
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_34_reg,	21, 26,  0},
	},
#endif //Identification_Pattern_wrtAction_TV001

#if 1 //Identification_Pattern_wrtAction_TV002
	//mixmode
	//Telop_Protection_J-Dorama2
	{
		{SOFTWARE1_SOFTWARE1_41_reg, 0, 1, 0x3},//all
		{SOFTWARE1_SOFTWARE1_41_reg, 4, 5, 0x3},//bottom
		{SOFTWARE1_SOFTWARE1_41_reg, 6, 7, 0x3},//left
		{SOFTWARE1_SOFTWARE1_41_reg, 8, 9, 0x3}, //right
	},

	//ultraviolet_1
	//ultraviolet_1 2
	{
		{KME_DEHALO_KME_DEHALO_F0_reg, 24, 31, 0x00},
		{KME_DEHALO_KME_DEHALO_F4_reg, 24, 31, 0x00},
		{KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 0x00},
		{KME_DEHALO_KME_DEHALO_FC_reg, 0, 7, 0x00},
	},

	//Bluray OSD + Train 3:2
	{
		{KME_DEHALO_KME_DEHALO_F0_reg, 24, 31, 0x08},
		{KME_DEHALO_KME_DEHALO_F4_reg, 24, 31, 0x80},
		{KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 0x04},
		{KME_DEHALO_KME_DEHALO_FC_reg, 0, 7, 0x10},

		{SOFTWARE1_SOFTWARE1_41_reg, 0, 1, 0},//all
		{SOFTWARE1_SOFTWARE1_41_reg, 4, 5, 0},//bottom
		{SOFTWARE1_SOFTWARE1_41_reg, 6, 7, 0},//left
		{SOFTWARE1_SOFTWARE1_41_reg, 8, 9, 0}, //right
	},

	//Animal Train HD(Frame In 3:2)
	{
		{KME_DEHALO_KME_DEHALO_F0_reg, 24, 31, 0x08},
		{KME_DEHALO_KME_DEHALO_F4_reg, 24, 31, 0x80},
		{KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 0x04},
		{KME_DEHALO_KME_DEHALO_FC_reg, 0, 7, 0x10},

	},

	//Dot32_Float32PinballMotion_EXT 20200902
	{
		{KME_VBUF_TOP_KME_VBUF_TOP_3C_reg, 0,8,0x40},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_E0_reg,0,7, 0x40},

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,3,3,0},	//20200904 new add
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,2,2 ,0},  //20200904 new add
	},

	//Periodical_Pattern_[007_Casino_Royale]
	{
		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31,	0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31,	0x11111111},
		//=================================
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_40_reg,9,12,1},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_40_reg,13,16,1},
	},

	//tv002 Netflix 1:57:02 ~	1:57:06
	{
		{KME_ME1_TOP0_KME_ME1_TOP0_10_reg, 0, 7, 0x00},  //gain0
		{KME_ME1_TOP0_KME_ME1_TOP0_10_reg, 8, 15, 0x00}, //gain1
		{KME_ME1_TOP0_KME_ME1_TOP0_10_reg, 16, 23, 0x64}, //gain2
		
		{KME_ME1_TOP0_KME_ME1_TOP0_14_reg, 0, 3, 0x1}, //mode
		{KME_ME1_TOP0_KME_ME1_TOP0_14_reg, 4, 13, 0xC8}, //limit
	},
#endif //Identification_Pattern_wrtAction_TV002

#if 1 //Identification_Pattern_wrtAction_TV010


	

















	

	


	//--#0080 0s
	{

		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},

		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},

		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0,31,0x11111111},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0,31,0x11111111},

		{KME_ME1_TOP1_ME1_COMMON1_10_reg, 8,17, 32},
		{KME_ME1_TOP1_ME1_COMMON1_10_reg,18,27, 32},
		{KME_ME1_TOP1_ME1_COMMON1_12_reg,16,31, 65532},
		{KME_TOP_KME_TOP_04_reg,29,29, 0},
		{KME_DEHALO5_PHMV_FIX_35_reg,28,28, 1},
	},




	

		//--#402 0s
		//--#402 70s
#endif //Identification_Pattern_wrtAction_TV010

#if 1 //Identification_Pattern_wrtAction_TV011
	

	

	



	

	


	//--#4_3.QTEK_6.5ppf_00081 0s
	{

		{MC_MC_D8_reg, 0, 9,0x0},
		{MC_MC_D8_reg,16,26,0x24},
		{MC_MC_D8_reg,31,31,0x1},

		{KME_IPME_KME_IPME_8C_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_8C_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_8C_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_90_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_90_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_90_reg, 18, 26, 19},

		{KME_IPME_KME_IPME_94_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_94_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_94_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_98_reg, 0, 8, 20},
		{KME_IPME_KME_IPME_98_reg, 9, 17, 20},
		{KME_IPME_KME_IPME_98_reg, 18, 26, 20},
		{KME_IPME_KME_IPME_04_reg, 0, 8, 19},
		{KME_IPME_KME_IPME_04_reg, 9, 17, 19},

		{KME_VBUF_TOP_KME_VBUF_TOP_0C_reg, 0, 31,0x33333333},
		{KME_VBUF_TOP_KME_VBUF_TOP_5C_reg, 0, 31,0x33333333},
	},
#endif //Identification_Pattern_wrtAction_TV011

#if 1 //Identification_Pattern_wrtAction_TV030
	//Snipper downtown 20s
	{

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,1,16,0x5C6B},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,0,15,0x5C6B},
	},

	//STAR TREK 12fps_01 6~9s
	{

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,1,16,0x5F07},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,0,15,0x5F07},	
		{KME_VBUF_TOP_KME_VBUF_TOP_28_reg,21,23,7},	
		{KME_VBUF_TOP_KME_VBUF_TOP_28_reg,27,29,6},	
		{KME_VBUF_TOP_KME_VBUF_TOP_1C_reg,21,23,7},	
		{KME_VBUF_TOP_KME_VBUF_TOP_1C_reg,27,29,6},
		{KME_ME1_TOP0_KME_ME1_TOP0_64_reg,0,15,0xA0A},
		{KME_ME1_TOP0_KME_ME1_TOP0_24_reg,0,15,0xA0A},
					
		{KMC_BI_bi_top_01_reg,00,00,0x0},
		{MC_MC_28_reg,14,14,0x1},
		{MC_MC_28_reg,15,22,0x0},
		{SOFTWARE_SOFTWARE_10_reg,00,00,0x0},

		{KME_DEHALO5_PHMV_FIX_35_reg,23,23,1},
		{KME_DEHALO_KME_DEHALO_10_reg,0,0,0},
		{HARDWARE_HARDWARE_58_reg,16,17,1},
		{SOFTWARE_SOFTWARE_46_reg,3,3,0},
		
		{MC_MC_50_reg,0,31,0x07550119},
		{MC_MC_54_reg,0,31,0x0efd0002},
		{MC_MC_58_reg,0,31,0x07570117},
		{MC_MC_5C_reg,0,31,0x0eff0000},
					
		{KME_LOGO1_KME_LOGO1_04_reg,0,31,0x7412ef00},
		{KME_LOGO1_KME_LOGO1_D8_reg,0,19,0xefc00},
		{KME_LOGO1_KME_LOGO1_DC_reg,0,19,0x73c4b},
		
		{MC2_MC2_9C_reg,0,31,0x0123bc00},
		{MC2_MC2_A0_reg,0,9,0x074},
		{MC2_MC2_A4_reg,0,29,0x04befc00},
		
		{MC2_MC2_A8_reg,0,9,0x1cf},
					
		{KME_IPME_KME_IPME_A0_reg,0,19,0x74c48},	
		
		{KME_IPME_KME_IPME_9C_reg,0,19,0x77402},
		{KME_VBUF_TOP_KME_VBUF_TOP_3C_reg,16,24,0x011},	
		{KME_VBUF_TOP_KME_VBUF_TOP_40_reg,0,31,0x001d5de00}, 
		
		{KME_VBUF_TOP_KME_VBUF_TOP_44_reg,0,31,0x00075445}, 
		{KME_VBUF_TOP_KME_VBUF_TOP_48_reg,0,31,0x003efc00}, 
		
		{KME_LBME_TOP_KME_LBME_TOP_00_reg,0,31,0x001df800}, 
		{KME_LBME_TOP_KME_LBME_TOP_04_reg,0,31,0x000ea845}, 
					
		{KME_LBME_TOP_KME_LBME_TOP_1C_reg,0,31,0x001df800}, 
		{KME_LBME_TOP_KME_LBME_TOP_20_reg,0,31,0x000eb046}, 
		
		{KME_VBUF_TOP_KME_VBUF_TOP_84_reg,0,31,0x0118ea11}, 
		
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_20_reg,0,31,0x077c010e}, 
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_24_reg,0,31,0x0001d422}, 
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_28_reg,0,31,0x03a845df}, 
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D4_reg,0,31,0xefc00a00}, 
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg,0,31,0xf2075445}, 
		
		{KME_LBME2_TOP_KME_LBME2_TOP_00_reg,0,31,0x001df800}, 
		{KME_LBME2_TOP_KME_LBME2_TOP_04_reg,0,21,0x010ea845}, 
	},

	//Pixelworks _01_BBC-Japan-12fps 2mins
	{

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 16, 16, 1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 15, 15, 1},
	},

	//07.07.02.CG_Flower_04-AVC1080P24.m2ts
	{

		{KME_ME1_TOP0_KME_ME1_TOP0_88_reg, 6, 6, 1},//me1_post_pfv_en
		{KME_ME1_TOP10_KME_ME1_REJUDGE_reg, 31, 31, 0},//me1_rejudge_apply
		{KME_ME1_TOP0_KME_ME1_TOP0_8C_reg, 15, 15, 0},//me1_post_adptpnt_ppi_en
		{KME_ME1_TOP0_KME_ME1_TOP0_88_reg, 8, 15, 0},//me1_post_cddpnt_ppi 
		{KME_ME1_TOP0_KME_ME1_TOP0_18_reg, 18, 20, 0},//me1_ip_cddpnt_st_rnd_gain
		{KME_ALGO_me12_cand_wsad_ctrl_reg, 0, 9, 0},//me1_wsad_offset_shift 
		{KME_ME1_TOP0_KME_ME1_TOP0_8C_reg, 13, 13, 1},//me1_post_eva_mode
		{KME_ME1_TOP0_KME_ME1_TOP0_88_reg, 4, 5, 2},//me1_post_pfv_mode
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,16,16,0},//me1_cond_zmv_cand_en
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,15,15,0},//me1_cond_zmv_cand_en
	},

	/*Soap Opera Effect test*/
	{

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,1,16,0x8108},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,0,15,0x8108},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg,21,22,2},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg,23,24,2},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg,28,29,0},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg,30,31,0},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_DC_reg,0,1,0},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_DC_reg,2,2,1},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_DC_reg,3,3,1},
		{KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_DC_reg,4,4,1},
	},

	/*Transformer.3.2011.1080p end*/
	{

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,1,16,0x84FF},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,0,15,0x84FF},
	},

#if 0 //use logo clean table
	//07.08.05.HQV-Transformer-1080P24.mp4
	{

		{KME_LOGO0_KME_LOGO0_D8_reg,0,3,0},
		{KME_LOGO0_KME_LOGO0_D8_reg,4,7,1},
		{KME_LOGO0_KME_LOGO0_D8_reg,8,11,2},

		{KME_LOGO0_KME_LOGO0_64_reg,0,7,2},
		{KME_LOGO0_KME_LOGO0_64_reg,8,15,3},
		{KME_LOGO0_KME_LOGO0_64_reg,16,23,2},
		{KME_LOGO0_KME_LOGO0_64_reg,24,31,6},

		//{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg,0,0,0},
			// 1.1 dh_logo_sad_d_th
		{KME_DEHALO2_KME_DEHALO2_84_reg, 12, 21, 250}, // dh_logo_sad_d_th_rg0
		{KME_DEHALO2_KME_DEHALO2_AC_reg, 12, 21, 250}, // dh_logo_sad_d_th_rg1
		{KME_DEHALO2_KME_DEHALO2_08_reg,  0,  9, 250}, // sad_d_th_rg2
		{KME_DEHALO6_dh_logo_ctrl_1_rg3_reg,  0,  9, 250}, // dh_logo_sad_d_th_rg3
		{KME_DEHALO6_dh_logo_ctrl_1_rg4_reg,  0,  9, 250}, // dh_logo_sad_d_th_rg4
		{KME_DEHALO6_dh_logo_ctrl_1_rg5_reg,  0,  9, 250}, // dh_logo_sad_d_th_rg5
		{KME_DEHALO6_dh_logo_ctrl_1_rg6_reg,  0,  9, 250}, // dh_logo_sad_d_th_rg6
		{KME_DEHALO6_dh_logo_ctrl_1_rg7_reg,  0,  9, 250}, // dh_logo_sad_d_th_rg7
		{KME_DEHALO6_dh_logo_ctrl_1_rg8_reg,  0,  9, 250}, // dh_logo_sad_d_th_rg8
		{KME_DEHALO6_dh_logo_ctrl_1_rg9_reg,  0,  9, 250}, // dh_logo_sad_d_th_rg9
		{KME_DEHALO6_dh_logo_ctrl_1_rga_reg,  0,  9, 250}, // dh_logo_sad_d_th_rga
		{KME_DEHALO6_dh_logo_ctrl_1_rgb_reg,  0,  9, 250}, // dh_logo_sad_d_th_rgb
		{KME_DEHALO6_dh_logo_ctrl_1_rgc_reg,  0,  9, 250}, // dh_logo_sad_d_th_rgc
		{KME_DEHALO6_dh_logo_ctrl_1_rgd_reg,  0,  9, 250}, // dh_logo_sad_d_th_rgd
		{KME_DEHALO6_dh_logo_ctrl_1_rge_reg,  0,  9, 250}, // dh_logo_sad_d_th_rge

		// 1.2 dh_logo_mv_d_th
		{KME_DEHALO2_KME_DEHALO2_84_reg,  6, 11, 31}, // dh_logo_mv_d_th_rg0
		{KME_DEHALO2_KME_DEHALO2_AC_reg,  6, 11, 31}, // dh_logo_mv_d_th_rg1
		{KME_DEHALO2_KME_DEHALO2_E8_reg, 19, 24, 31}, // mv_d_th_rg2
		{KME_DEHALO6_dh_logo_ctrl_1_rg3_reg, 20, 25, 3}, // dh_logo_mv_d_th_rg3
		{KME_DEHALO6_dh_logo_ctrl_1_rg4_reg, 20, 25, 3}, // dh_logo_mv_d_th_rg4
		{KME_DEHALO6_dh_logo_ctrl_1_rg5_reg, 20, 25, 31}, // dh_logo_mv_d_th_rg5
		{KME_DEHALO6_dh_logo_ctrl_1_rg6_reg, 20, 25, 31}, // dh_logo_mv_d_th_rg6
		{KME_DEHALO6_dh_logo_ctrl_1_rg7_reg, 20, 25, 31}, // dh_logo_mv_d_th_rg7
		{KME_DEHALO6_dh_logo_ctrl_1_rg8_reg, 20, 25, 31}, // dh_logo_mv_d_th_rg8
		{KME_DEHALO6_dh_logo_ctrl_1_rg9_reg, 20, 25, 31}, // dh_logo_mv_d_th_rg9
		{KME_DEHALO6_dh_logo_ctrl_1_rga_reg, 20, 25, 31}, // dh_logo_mv_d_th_rga
		{KME_DEHALO6_dh_logo_ctrl_1_rgb_reg, 20, 25, 31}, // dh_logo_mv_d_th_rgb
		{KME_DEHALO6_dh_logo_ctrl_1_rgc_reg, 20, 25, 31}, // dh_logo_mv_d_th_rgc
		{KME_DEHALO6_dh_logo_ctrl_1_rgd_reg, 20, 25, 31}, // dh_logo_mv_d_th_rgd
		{KME_DEHALO6_dh_logo_ctrl_1_rge_reg, 20, 25, 31}, // dh_logo_mv_d_th_rge

		// 1.3 dh_logo_mv_d_diff_th
		{KME_DEHALO2_KME_DEHALO2_84_reg,  0,  5, 3}, // dh_logo_mv_d_diff_th_rg0
		{KME_DEHALO2_KME_DEHALO2_AC_reg,  0,  5, 3}, // dh_logo_mv_d_diff_th_rg1
		{KME_DEHALO2_KME_DEHALO2_E8_reg, 13, 18, 3}, // mv_d_diff_th_rg2
		{KME_DEHALO6_dh_logo_ctrl_1_rg3_reg, 26, 31, 10}, // dh_logo_mv_d_diff_th_rg3
		{KME_DEHALO6_dh_logo_ctrl_1_rg4_reg, 26, 31, 10}, // dh_logo_mv_d_diff_th_rg4
		{KME_DEHALO6_dh_logo_ctrl_1_rg5_reg, 26, 31, 3}, // dh_logo_mv_d_diff_th_rg5
		{KME_DEHALO6_dh_logo_ctrl_1_rg6_reg, 26, 31, 3}, // dh_logo_mv_d_diff_th_rg6
		{KME_DEHALO6_dh_logo_ctrl_1_rg7_reg, 26, 31, 3}, // dh_logo_mv_d_diff_th_rg7
		{KME_DEHALO6_dh_logo_ctrl_1_rg8_reg, 26, 31, 3}, // dh_logo_mv_d_diff_th_rg8
		{KME_DEHALO6_dh_logo_ctrl_1_rg9_reg, 26, 31, 3}, // dh_logo_mv_d_diff_th_rg9
		{KME_DEHALO6_dh_logo_ctrl_1_rga_reg, 26, 31, 3}, // dh_logo_mv_d_diff_th_rga
		{KME_DEHALO6_dh_logo_ctrl_1_rgb_reg, 26, 31, 3}, // dh_logo_mv_d_diff_th_rgb
		{KME_DEHALO6_dh_logo_ctrl_1_rgc_reg, 26, 31, 3}, // dh_logo_mv_d_diff_th_rgc
		{KME_DEHALO6_dh_logo_ctrl_1_rgd_reg, 26, 31, 3}, // dh_logo_mv_d_diff_th_rgd
		{KME_DEHALO6_dh_logo_ctrl_1_rge_reg, 26, 31, 3}, // dh_logo_mv_d_diff_th_rge


		// 2. tmv condition

		// 2.1 dh_logo_sad_t_th
		{KME_DEHALO2_KME_DEHALO2_A0_reg, 16, 25, 16}, // dh_logo_sad_t_th_rg0
		{KME_DEHALO2_KME_DEHALO2_A4_reg, 16, 25, 16}, // dh_logo_sad_t_th_rg1
		{KME_DEHALO2_KME_DEHALO2_10_reg,  8, 17, 16}, // sad_t_th_rg2
		{KME_DEHALO6_dh_logo_ctrl_1_rg3_reg, 10, 19, 150}, // dh_logo_sad_t_th_rg3
		{KME_DEHALO6_dh_logo_ctrl_1_rg4_reg, 10, 19, 150}, // dh_logo_sad_t_th_rg4
		{KME_DEHALO6_dh_logo_ctrl_1_rg5_reg, 10, 19, 16}, // dh_logo_sad_t_th_rg5
		{KME_DEHALO6_dh_logo_ctrl_1_rg6_reg, 10, 19, 16}, // dh_logo_sad_t_th_rg6
		{KME_DEHALO6_dh_logo_ctrl_1_rg7_reg, 10, 19, 16}, // dh_logo_sad_t_th_rg7
		{KME_DEHALO6_dh_logo_ctrl_1_rg8_reg, 10, 19, 16}, // dh_logo_sad_t_th_rg8
		{KME_DEHALO6_dh_logo_ctrl_1_rg9_reg, 10, 19, 16}, // dh_logo_sad_t_th_rg9
		{KME_DEHALO6_dh_logo_ctrl_1_rga_reg, 10, 19, 16}, // dh_logo_sad_t_th_rga
		{KME_DEHALO6_dh_logo_ctrl_1_rgb_reg, 10, 19, 16}, // dh_logo_sad_t_th_rgb
		{KME_DEHALO6_dh_logo_ctrl_1_rgc_reg, 10, 19, 16}, // dh_logo_sad_t_th_rgc
		{KME_DEHALO6_dh_logo_ctrl_1_rgd_reg, 10, 19, 16}, // dh_logo_sad_t_th_rgd
		{KME_DEHALO6_dh_logo_ctrl_1_rge_reg, 10, 19, 16}, // dh_logo_sad_t_th_rge 

		// 2.2 dh_logo_mv_t_th
		{KME_DEHALO2_KME_DEHALO2_A0_reg,  0,  7, 16}, // dh_logo_mv_t_th_rg0
		{KME_DEHALO2_KME_DEHALO2_A4_reg,  0,  7, 16}, // dh_logo_mv_t_th_rg1
		{KME_DEHALO2_KME_DEHALO2_08_reg, 10, 17, 16}, // mv_t_th_rg2
		{KME_DEHALO6_dh_logo_ctrl_2_rg3_rg4_reg,  8, 15, 8}, // dh_logo_mv_t_th_rg3
		{KME_DEHALO6_dh_logo_ctrl_2_rg3_rg4_reg, 24, 31, 8}, // dh_logo_mv_t_th_rg4
		{KME_DEHALO6_dh_logo_ctrl_2_rg5_rg6_reg,  8, 15, 16}, // dh_logo_mv_t_th_rg5
		{KME_DEHALO6_dh_logo_ctrl_2_rg5_rg6_reg, 24, 31, 16}, // dh_logo_mv_t_th_rg6
		{KME_DEHALO6_dh_logo_ctrl_2_rg7_rg8_reg,  8, 15, 16}, // dh_logo_mv_t_th_rg7
		{KME_DEHALO6_dh_logo_ctrl_2_rg7_rg8_reg, 24, 31, 16}, // dh_logo_mv_t_th_rg8
		{KME_DEHALO6_dh_logo_ctrl_2_rg9_rga_reg,  8, 15, 16}, // dh_logo_mv_t_th_rg9
		{KME_DEHALO6_dh_logo_ctrl_2_rg9_rga_reg, 24, 31, 16}, // dh_logo_mv_t_th_rga
		{KME_DEHALO6_dh_logo_ctrl_2_rgb_rgc_reg,  8, 15, 16}, // dh_logo_mv_t_th_rgb
		{KME_DEHALO6_dh_logo_ctrl_2_rgb_rgc_reg, 24, 31, 16}, // dh_logo_mv_t_th_rgc
		{KME_DEHALO6_dh_logo_ctrl_2_rgd_rge_reg,  8, 15, 16}, // dh_logo_mv_t_th_rgd
		{KME_DEHALO6_dh_logo_ctrl_2_rgd_rge_reg, 24, 31, 16}, // dh_logo_mv_t_th_rge

		// 2.3 dh_logo_tmv_num
		{KME_DEHALO2_KME_DEHALO2_A0_reg,  8, 15, 37}, // dh_logo_tmv_num_rg0
		{KME_DEHALO2_KME_DEHALO2_A4_reg,  8, 15, 37}, // dh_logo_tmv_num_rg1
		{KME_DEHALO2_KME_DEHALO2_10_reg,  0,  7, 37}, // tmv_num_rg2
		{KME_DEHALO6_dh_logo_ctrl_2_rg3_rg4_reg,  0,  7, 3}, // dh_logo_tmv_num_rg3
		{KME_DEHALO6_dh_logo_ctrl_2_rg3_rg4_reg, 16, 23, 3}, // dh_logo_tmv_num_rg4
		{KME_DEHALO6_dh_logo_ctrl_2_rg5_rg6_reg,  0,  7, 37}, // dh_logo_tmv_num_rg5
		{KME_DEHALO6_dh_logo_ctrl_2_rg5_rg6_reg, 16, 23, 37}, // dh_logo_tmv_num_rg6
		{KME_DEHALO6_dh_logo_ctrl_2_rg7_rg8_reg,  0,  7, 37}, // dh_logo_tmv_num_rg7
		{KME_DEHALO6_dh_logo_ctrl_2_rg7_rg8_reg, 16, 23, 37}, // dh_logo_tmv_num_rg8
		{KME_DEHALO6_dh_logo_ctrl_2_rg9_rga_reg,  0,  7, 37}, // dh_logo_tmv_num_rg9
		{KME_DEHALO6_dh_logo_ctrl_2_rg9_rga_reg, 16, 23, 37}, // dh_logo_tmv_num_rga
		{KME_DEHALO6_dh_logo_ctrl_2_rgb_rgc_reg,  0,  7, 37}, // dh_logo_tmv_num_rgb
		{KME_DEHALO6_dh_logo_ctrl_2_rgb_rgc_reg, 16, 23, 37}, // dh_logo_tmv_num_rgc
		{KME_DEHALO6_dh_logo_ctrl_2_rgd_rge_reg,  0,  7, 37}, // dh_logo_tmv_num_rgd
		{KME_DEHALO6_dh_logo_ctrl_2_rgd_rge_reg, 16, 23, 37}, // dh_logo_tmv_num_rge

		// 2.4 dh_logo_tmv_mode
		{KME_DEHALO2_KME_DEHALO2_A0_reg, 26, 27, 0}, // dh_logo_tmv_mode_rg0
		{KME_DEHALO2_KME_DEHALO2_A4_reg, 26, 27, 0}, // dh_logo_tmv_mode_rg1
		{KME_DEHALO2_KME_DEHALO2_E8_reg, 25, 26, 0}, // tmv_mode_rg2
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg,  6,  7, 0}, // dh_logo_tmv_mode_rg3
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg,  8,  9, 0}, // dh_logo_tmv_mode_rg4
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 10, 11, 0}, // dh_logo_tmv_mode_rg5
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 12, 13, 0}, // dh_logo_tmv_mode_rg6
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 14, 15, 0}, // dh_logo_tmv_mode_rg7
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 16, 17, 0}, // dh_logo_tmv_mode_rg8
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 18, 19, 0}, // dh_logo_tmv_mode_rg9
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 20, 21, 0}, // dh_logo_tmv_mode_rga
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 22, 23, 0}, // dh_logo_tmv_mode_rgb
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 24, 25, 0}, // dh_logo_tmv_mode_rgc
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 26, 27, 0}, // dh_logo_tmv_mode_rgd
		{KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 28, 29, 0}, // dh_logo_tmv_mode_rge
	},
#endif

	/*Transformer.3.2011.1080p end*/
	//TCL2875P-5459 Rick & Morty S1E1,-18:51
	{

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg,16,16,1},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg,15,15,1},
	},
#endif //Identification_Pattern_wrtAction_TV030

#if 1 //Identification_Pattern_wrtAction_TV043
	/*7.12.02.movZonePlate_AVC1080P60.mp4*/
	/*01.E_CMO-23_98psf.mp4*/
	{

		{MC_MC_28_reg, 14, 14, 1},
		{MC_MC_28_reg, 15, 22, 0},
		{MC_MC_B0_reg, 20, 27, 0},
	},

	/*7.9.09.IDT_HD_H_R_MPG1080P24.mp4*/
	{

		{COLOR_SHARP_DM_NR_SHP_CTRL_reg, 0, 0, 0},//  sharpness peaking
		{COLOR_SHARP_SHP_DLTI_CTRL_1_reg, 0, 0, 0},//  sharpness dlti
		{COLOR_MB_PEAKING_MB_PEAKING_CTRL_reg, 0, 0, 0},//  Multiband peaking
		{COLOR_MB_PEAKING_MB_Double_Buffer_CTRL_reg, 0, 0, 0},//  Multiband peaking DB apply
		{KME_DEHALO_KME_DEHALO_70_reg, 4, 4, 1},
		{KME_DEHALO_KME_DEHALO_B0_reg, 12, 19, 0},
		{KME_DEHALO_KME_DEHALO_D4_reg, 6, 11,15}, 
		{MC2_MC2_50_reg,0,1, 0},
		{MC2_MC2_20_reg, 0, 1, 0},	
	},

	//Montage_M_ROT-720p50.mpg
	{

		{KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 0x00},
		{KME_DEHALO_KME_DEHALO_FC_reg, 0,7, 0x00},
		{KMC_BI_bi_top_01_reg,2,2, 0},
		{KME_DEHALO_KME_DEHALO_BC_reg,17,22, 0x3f},
		{MC_MC_B8_reg,11,11, 0},
		{MC_MC_B4_reg,16,23, 0x00},
		{MC_MC_B4_reg,24,31, 0x00},
		{MC_MC_B0_reg, 20 ,27, 0},
		{MC_MC_28_reg, 14, 14, 1},
		{MC_MC_28_reg, 15, 22, 0},
	},

	/*Midway.4khdr_13_Propeller*/
	{

		{MC2_MC2_20_reg, 0, 1, 0},
		{KME_DEHALO_KME_DEHALO_F8_reg, 24, 31, 0x32},
		{KME_DEHALO_KME_DEHALO_FC_reg, 20, 22, 6},
	},

	/*3.QTEK_6.5PPF_00081*/ /*monosco4K_Hscroll_10pix_50Hz_long_50p-3840*2160*/
	{

		{MC2_MC2_20_reg, 0, 1, 0},
		{KME_ME1_TOP0_KME_ME1_TOP0_2C_reg, 0, 12, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 3, 3, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 4, 4, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 5, 5, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 6, 6, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 7, 7, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 8, 8, 0},

		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 2, 2, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 3, 3, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 4, 4, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 5, 5, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 6, 6, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 7, 7, 0},

		{COLOR_SHARP_DM_NR_SHP_CTRL_reg, 0, 0, 0},//  sharpness peaking
		{COLOR_SHARP_SHP_DLTI_CTRL_1_reg, 0, 0, 0},//  sharpness dlti
		{COLOR_MB_PEAKING_MB_PEAKING_CTRL_reg, 0, 0, 0},//  Multiband peaking
		{COLOR_MB_PEAKING_MB_Double_Buffer_CTRL_reg, 0, 0, 1},//  Multiband peaking DB apply
	},

	/*4K.znds.com]ShutUpAndPlay.Berlin.2014.2160p.HDTV.HEVC-jTV-50p-3840x2160*/
	//0:40-0:43
	{

		{KME_DEHALO_KME_DEHALO_FC_reg, 23, 23, 0},
	},

	/*DMN Cityscapes 720p60*/
	{

		{MC_MC_28_reg, 14, 14, 1},
		{MC_MC_28_reg, 15, 22, 0},
		{KME_ME1_TOP1_ME1_COMMON1_10_reg, 8, 17,  32},
		{KME_ME1_TOP1_ME1_COMMON1_10_reg, 18, 27, 32},
		{KME_ME1_TOP1_ME1_COMMON1_12_reg, 16, 31, 65532},
		{KME_TOP_KME_TOP_04_reg, 29, 29, 0},
		{IPPRE_IPPRE_04_reg, 16, 23, 150},
	},

	/*Roku ultra box: Netflix: Breaking bad S5E5 33:55 standing man ghost after dust of truck*/
	/*Roku ultra box: Netflix: Lucifer S2E2, 9:20, broken on black building*/
	/*1080P-motion-res-test-pattern*/
	{

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 4, 4, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 5, 5, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 6, 6, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 7, 7, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 8, 8, 0},
		
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 3, 3, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 4, 4, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 5, 5, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 6, 6, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 7, 7, 0},
	},

	//Rting 24P ghost
	{

		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 3, 3, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 4, 4, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 5, 5, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 6, 6, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 7, 7, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_10_reg, 8, 8, 0},
		
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 2, 2, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 3, 3, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 4, 4, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 5, 5, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 6, 6, 0},
		{KME_VBUF_TOP_KME_VBUF_TOP_14_reg, 7, 7, 0},
	},
#endif //Identification_Pattern_wrtAction_TV043
#endif //no use
};

const unsigned int MEMC_setting_Table_RTK2885pp[/*MEMC_PERFORMANCE_SETTING_TABLE_MAX*/][MEMC_PERFORMANCE_SETTING_ITEM_MAX][MEMC_PERFORMANCE_SETTING_MAX] = 
{
	//#0
	//do nothing -> return all values
	{},

	//DEHALO_CLOSE_RTK2885PP
	{
		// set enum id
		{DEHALO_CLOSE_RTK2885PP,0,0,0},
		
		// setting
		{M8P_RTME_DEHALO0_RTME_DEHALO0_10_reg, 0, 0, 0},
	},
};

unsigned char get_special_scene_maxid(void)
{
	unsigned char u8_scene_max = 0;

	if(RUN_MERLIN8P()){
		u8_scene_max = (MEMC_SPECIAL_SCENE_TABLE_MAX_RTK2885PP>=255) ? 255 : MEMC_SPECIAL_SCENE_TABLE_MAX_RTK2885PP;
	}else if(RUN_MERLIN8()){
		u8_scene_max = (MEMC_SPECIAL_SCENE_TABLE_MAX>=255) ? 255 : MEMC_SPECIAL_SCENE_TABLE_MAX;
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u8_scene_max;
}

unsigned char get_status_table_maxid(void)
{
	unsigned char u8_status_table_max = 0;

	if(RUN_MERLIN8P()){
		u8_status_table_max = (MEMC_STATUS_TABLE_MAX_RTK2885PP>=255) ? 255 : MEMC_STATUS_TABLE_MAX_RTK2885PP;
	}else if(RUN_MERLIN8()){
		u8_status_table_max = (MEMC_STATUS_TABLE_MAX>=255) ? 255 : MEMC_STATUS_TABLE_MAX;
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u8_status_table_max;
}

unsigned short get_database_table_size(void)
{
	unsigned short u16_size = 0;

	if(RUN_MERLIN8P()){
		u16_size = g_Sizeof_MEMC_setting_Table_RTK2885pp;
	}else if(RUN_MERLIN8()){
		u16_size = g_Sizeof_MEMC_setting_Table_RTK2885p;
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u16_size;
}

unsigned short get_database_item_size(unsigned char item_id)
{
	unsigned short u16_size = 0;

	if(RUN_MERLIN8P()){
		u16_size = (sizeof(MEMC_setting_Table_RTK2885pp[item_id])>>4);
	}else if(RUN_MERLIN8()){
		u16_size = (sizeof(MEMC_setting_Table_RTK2885p[item_id])>>4);
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u16_size;
}

unsigned int get_database_enum_id(unsigned char setting_id, unsigned char item_id)
{
	unsigned int u32_enum_id = 0;

	if(RUN_MERLIN8P()){
		u32_enum_id = MEMC_setting_Table_RTK2885pp[setting_id][item_id][MEMC_PERFORMANCE_SETTING_enum_id];
	}else if(RUN_MERLIN8()){
		u32_enum_id = MEMC_setting_Table_RTK2885p[setting_id][item_id][MEMC_PERFORMANCE_SETTING_enum_id];
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_enum_id;
}

unsigned int get_database_reg_addr(unsigned char setting_id, unsigned char item_id)
{
	unsigned int u32_reg_addr = 0;

	if(RUN_MERLIN8P()){
		u32_reg_addr = MEMC_setting_Table_RTK2885pp[setting_id][item_id][MEMC_PERFORMANCE_SETTING_reg];
	}else if(RUN_MERLIN8()){
		u32_reg_addr = MEMC_setting_Table_RTK2885p[setting_id][item_id][MEMC_PERFORMANCE_SETTING_reg];
	}//RUN_MERLIN8P & RUN_MERLIN8

	return u32_reg_addr;
}

unsigned int get_database_low_bit(unsigned char setting_id, unsigned char item_id)
{
	unsigned int low_bit = 0;

	if(RUN_MERLIN8P()){
		low_bit = MEMC_setting_Table_RTK2885pp[setting_id][item_id][MEMC_PERFORMANCE_SETTING_low_bit];
	}else if(RUN_MERLIN8()){
		low_bit = MEMC_setting_Table_RTK2885p[setting_id][item_id][MEMC_PERFORMANCE_SETTING_low_bit];
	}//RUN_MERLIN8P & RUN_MERLIN8

	return low_bit;
}

unsigned int get_database_high_bit(unsigned char setting_id, unsigned char item_id)
{
	unsigned int high_bit = 0;

	if(RUN_MERLIN8P()){
		high_bit = MEMC_setting_Table_RTK2885pp[setting_id][item_id][MEMC_PERFORMANCE_SETTING_high_bit];
	}else if(RUN_MERLIN8()){
		high_bit = MEMC_setting_Table_RTK2885p[setting_id][item_id][MEMC_PERFORMANCE_SETTING_high_bit];
	}//RUN_MERLIN8P & RUN_MERLIN8

	return high_bit;
}

unsigned int get_database_reg_val(unsigned char setting_id, unsigned char item_id)
{
	unsigned int reg_val = 0;

	if(RUN_MERLIN8P()){
		reg_val = MEMC_setting_Table_RTK2885pp[setting_id][item_id][MEMC_PERFORMANCE_SETTING_value];
	}else if(RUN_MERLIN8()){
		reg_val = MEMC_setting_Table_RTK2885p[setting_id][item_id][MEMC_PERFORMANCE_SETTING_value];
	}//RUN_MERLIN8P & RUN_MERLIN8

	return reg_val;
}

unsigned char get_Performance_Setting_Num(void)
{
	unsigned short database_size = get_database_table_size();

	database_size = (database_size>255) ? 255 : database_size-1;
	return (unsigned char)database_size;
}

void Check_MEMC_identify_Bin(void)
{
	unsigned char u8_i = 0;
	unsigned char pattern_num = 0;
	memset(&g_Bin_Table_isValid, 0, sizeof(g_Bin_Table_isValid));

	// check whether identify data is valid or not
	for(u8_i=0; u8_i<255; u8_i++){
		g_Bin_Table_isValid[u8_i] = Check_Single_DynamicOptimizeArray(&g_IDENTIFY_TABLE.IDENTIFY_TABLE.idendify_info[u8_i], PROJECT_ID_BIN, u8_i, false);

		if(g_Bin_Table_isValid[u8_i]==true){
			pattern_num = u8_i+1;
		}
	}

	// show error info with invalid identify data
	for(u8_i=0; u8_i<pattern_num; u8_i++){
		if(g_Bin_Table_isValid[u8_i]==false){
			rtd_pr_memc_notice("[%s][%s] ERROR pattern(%d) is invalid\n", __FUNCTION__, __MEMC_CHECK_BIN__, u8_i);
			Check_Single_DynamicOptimizeArray(&g_IDENTIFY_TABLE.IDENTIFY_TABLE.idendify_info[u8_i], PROJECT_ID_BIN, u8_i, true);
		}
	}

	// update the number of identify data from bin
	g_IDENTIFY_TABLE.IDENTIFY_TABLE.pattern_num = pattern_num;
	rtd_pr_memc_notice("[%s][%s] Pattern number,%d\n", __FUNCTION__, __MEMC_CHECK_BIN__, pattern_num);
}

bool is_Prj_support_indentify(unsigned char u8_project_id)
{
#if NEW_IDENTIFY_FLOW_EN
	if(u8_project_id==VIP_Project_ID_TV001 || u8_project_id==VIP_Project_ID_TV002 || 
		u8_project_id==VIP_Project_ID_TV006 || u8_project_id==VIP_Project_ID_TV010 || 
		u8_project_id==VIP_Project_ID_TV030) {
		// use original flow: tv011, tv043
		return true;
	}
	return false;
#else
	return false;
#endif
}

bool is_indentify_Info_enable(unsigned char u8_project_id)
{
	if(is_Prj_support_indentify(u8_project_id)==false){
		return false;
	}

	if(u8_project_id==VIP_Project_ID_TV001){//VIP_Project_ID_TV001
		return true;
	}
	if(u8_project_id==VIP_Project_ID_TV002 && Pro_tv002){
		return true;
	}
	if(u8_project_id==VIP_Project_ID_TV006){
		return true;
	}
	if(u8_project_id==VIP_Project_ID_TV010 && Pro_tv010){
		return true;
	}
	if(u8_project_id==VIP_Project_ID_TV030 && Pro_tv030){
		return true;
	}
	if(u8_project_id==g_ProjectID){
		return true;
	}
	return false;
}

#if NEW_IDENTIFY_FLOW_EN
_IDENTIFY_INFO* get_idendify_Info_RTK2885p(unsigned char u8_project_id)
{
	if(u8_project_id==VIP_Project_ID_TV001){//VIP_Project_ID_TV001
		return &DynamicOptimizeMEMC_Info_RTK2885p_TV001;
	}
	if(u8_project_id==VIP_Project_ID_TV002){
		return &DynamicOptimizeMEMC_Info_RTK2885p_TV002;
	}
	if(u8_project_id==VIP_Project_ID_TV006){
		return &DynamicOptimizeMEMC_Info_RTK2885p_TV006;
	}
	if(u8_project_id==VIP_Project_ID_TV010){
		return &DynamicOptimizeMEMC_Info_RTK2885p_TV010;
	}
	if(u8_project_id==VIP_Project_ID_TV030){
		return &DynamicOptimizeMEMC_Info_RTK2885p_TV030;
	}
	#if 0// use original flow
	//if(u8_project_id==VIP_Project_ID_TV011){
	//	return &DynamicOptimizeMEMC_Info_RTK2885p_TV011;
	//}
	//if(u8_project_id==VIP_Project_ID_TV043){
	//	return &DynamicOptimizeMEMC_Info_RTK2885p_TV043;
	//}
	#endif
	return &DynamicOptimizeMEMC_Info_RTK2885p_TV001;
}
_IDENTIFY_INFO* get_idendify_Info_RTK2885pp(unsigned char u8_project_id)
{
	if(u8_project_id==VIP_Project_ID_TV001){//VIP_Project_ID_TV001
		return &DynamicOptimizeMEMC_Info_RTK2885pp_TV001;
	}
	if(u8_project_id==VIP_Project_ID_TV002){
		return &DynamicOptimizeMEMC_Info_RTK2885pp_TV002;
	}
	if(u8_project_id==VIP_Project_ID_TV006){
		return &DynamicOptimizeMEMC_Info_RTK2885pp_TV006;
	}
	if(u8_project_id==VIP_Project_ID_TV010){
		return &DynamicOptimizeMEMC_Info_RTK2885pp_TV010;
	}
	if(u8_project_id==VIP_Project_ID_TV030){
		return &DynamicOptimizeMEMC_Info_RTK2885pp_TV030;
	}
	#if 0// use original flow
	//if(u8_project_id==VIP_Project_ID_TV011){
	//	return &DynamicOptimizeMEMC_Info_RTK2885pp_TV011;
	//}
	//if(u8_project_id==VIP_Project_ID_TV043){
	//	return &DynamicOptimizeMEMC_Info_RTK2885pp_TV043;
	//}
	#endif
	return &DynamicOptimizeMEMC_Info_RTK2885pp_TV001;
}
_IDENTIFY_INFO* get_idendify_Info(unsigned char u8_project_id)
{
	_IDENTIFY_INFO* identify_info = NULL;

	if(RUN_MERLIN8P()){
		identify_info = get_idendify_Info_RTK2885pp(u8_project_id);
	}else if(RUN_MERLIN8()){
		identify_info = get_idendify_Info_RTK2885p(u8_project_id);
	}//RUN_MERLIN8P & RUN_MERLIN8

	return identify_info;
}
#endif

bool reg_addr_is_right(unsigned int reg_addr)
{
	if(reg_addr<0xB8020000){
		return false;
	}
	if(reg_addr>=0xB8030000 && reg_addr<0xB8090000){
		return false;
	}
	if(reg_addr>0xB809FFFC){
		return false;
	}
	if(((reg_addr>>2)<<2) != reg_addr){
		return false;
	}
	return true;
}

bool Setting_Row_Is_Empty(unsigned char set_id, unsigned char item_id)
{
	if(get_database_reg_addr(set_id, item_id)!=0){
		return false;
	}
	if(get_database_low_bit(set_id, item_id)!=0){
		return false;
	}
	if(get_database_high_bit(set_id, item_id)!=0){
		return false;
	}
	if(get_database_reg_val(set_id, item_id)!=0){
		return false;
	}
	return true;
}

#if DATABASE_CONFIG
//get_setting_table
void memc_identify_ctrl_apply_setting(unsigned char set_id, unsigned char u8_option)
{
	static unsigned int backup_reg_val[MEMC_PERFORMANCE_SETTING_TABLE_MAX][MEMC_PERFORMANCE_SETTING_ITEM_MAX] = {0};
	static bool u1_backup_setting[MEMC_PERFORMANCE_SETTING_TABLE_MAX] = {0};
	unsigned char u8_i = 0;
	unsigned short item_size = 0;
	unsigned int reg_addr = 0, low_bit = 0, high_bit = 0, reg_val = 0;
	unsigned short database_size = get_database_table_size();

	if(set_id >= database_size || u8_option >= MEMC_PERFORMANCE_SETTING_OPTION_NUM/*|| (setting_index > (sizeof(MEMC_setting_Table)/sizeof(MEMC_setting_Table[0]))-1)*/)
		return;

	if(MEMC_setting_Vaild[set_id]==false)
		return;

	// apply the setting
	if(u8_option == MEMC_PERFORMANCE_SETTING_OPTION_SET){

		//save the original register value
		if(!u1_backup_setting[set_id]){
			item_size = get_database_item_size(set_id);
			for(u8_i = 1; u8_i < item_size; u8_i++) // 4 (MEMC_PERFORMANCE_SETTING_MAX) * 4(unsigned int) = 16
			{
				// only save register value with valid setting row
				if(Setting_Row_Is_Empty(set_id, u8_i)==true){
					break;
				}

				reg_addr = get_database_reg_addr(set_id, u8_i);
				low_bit = get_database_low_bit(set_id, u8_i);
				high_bit = get_database_high_bit(set_id, u8_i);
				if(reg_addr_is_right(reg_addr)){
					//store the value when switching
					ReadRegister(reg_addr, low_bit, high_bit, &backup_reg_val[set_id][u8_i]);
				}else {
					rtd_pr_memc_emerg("[%s] Error(Abnormal register address) reg_addr,%x	set_id,%d,%d\n", __FUNCTION__, reg_addr, set_id, u8_i);
				}
			}
			u1_backup_setting[set_id]=true;
		}
		
		item_size = get_database_item_size(set_id);
		for(u8_i = 1; u8_i < item_size; u8_i++) // 4 (MEMC_PERFORMANCE_SETTING_MAX) * 4(unsigned int) = 16
		{
			reg_addr = get_database_reg_addr(set_id, u8_i);

			//write table value
			if(set_id != 0 && reg_addr != 0)
			{
				low_bit = get_database_low_bit(set_id, u8_i);
				high_bit = get_database_high_bit(set_id, u8_i);
				reg_val = get_database_reg_val(set_id, u8_i);
				WriteRegister(reg_addr, low_bit, high_bit, reg_val);
			}
		}
	}
	// restore the setting
	else if(u8_option == MEMC_PERFORMANCE_SETTING_OPTION_RESTORE){
		//restore the original register value
		item_size = get_database_item_size(set_id);
		for(u8_i = 1; u8_i < item_size; u8_i++) // 4 (MEMC_PERFORMANCE_SETTING_MAX) * 4(unsigned int) = 16
		{
			reg_addr = get_database_reg_addr(set_id, u8_i);
			if(reg_addr != 0){
				low_bit = get_database_low_bit(set_id, u8_i);
				high_bit = get_database_high_bit(set_id, u8_i);
				WriteRegister(reg_addr, low_bit, high_bit, backup_reg_val[set_id][u8_i]);
			}
		}

		u1_backup_setting[set_id]=false;
	}
}
#endif //DATABASE_CONFIG

void Set_MEMC_Status(_OUTPUT_WRT_COM_REG *pOutput, unsigned char status_id)
{
	_PQLCONTEXT *s_pContext = GetPQLContext_m();

	if(status_id >= MEMC_STATUS_TABLE_MAX){
		rtd_pr_memc_emerg("[%s][%d] status_id(%d) exceed max(%d)\n", status_id, MEMC_STATUS_TABLE_MAX);
		return;
	}

	if(status_id==MEMC_STATUS_CASINO_RP){
		pOutput->u1_casino_RP_detect_true=1;
	}
	else if(status_id==MEMC_STATUS_RP_0){
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true=0;
	}
	else if(status_id==MEMC_STATUS_RP_1){
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true=1;
	}
	else if(status_id==MEMC_STATUS_RP_2){
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true=2;
	}
	else if(status_id==MEMC_STATUS_RP_3){
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true=3;
	}
	else if(status_id==MEMC_STATUS_RP_4){
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true=4;
	}
	else if(status_id==MEMC_STATUS_HQV_MULTI_CADENCE){
		s_pContext->_output_wrt_comreg.u1_MC_Logo_Close_true =1;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_Slow_Convergence_true =1;
		s_pContext->_output_me_sceneAnalysis.u2_panning_flag =2;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_SLOW_CONVERGENCE_PANNING_2){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_Slow_Convergence_true =1;
		s_pContext->_output_me_sceneAnalysis.u2_panning_flag =2;
	}
	else if(status_id==MEMC_STATUS_GFB_SPECIALSCENE_TRUE){
		s_pContext->_output_wrt_comreg.u1_GFB_specialScene_true=1;
	}
	else if(status_id==MEMC_STATUS_GFB_SPECIAL_APPLY){
		s_pContext->_output_wrt_comreg.u1_GFB_Special_apply=1;
	}
	else if(status_id==MEMC_STATUS_BLACK_19YO1){
		pOutput->u8_01_Black_19YO1_flag=1;
	}
	else if(status_id==MEMC_STATUS_Q13722_FLAG){
		g_Q13722_flag = 1;
	}
	else if(status_id==MEMC_STATUS_GFB_FORCE_CUSTOM){
		s_pContext->_output_wrt_comreg.u1_GFB_force_Custom_true=1;
	}
	else if(status_id==MEMC_STATUS_181_FLAG){
		g_181_flag = 1;
	}
	else if(status_id==MEMC_STATUS_BTS_FLAG){
		pOutput->u1_BTS_Rp_flag=1;
	}
	else if(status_id==MEMC_STATUS_LALALAND_1_14_25){
		s_pContext->_output_me_sceneAnalysis.u3_Dynamic_MVx_Step=0;
		s_pContext->_output_me_sceneAnalysis.u3_Dynamic_MVy_Step=0;
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true =3;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_SLOW_CONVERGENCE){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_Slow_Convergence_true =1;
	}
	else if(status_id==MEMC_STATUS_SMALLOBJECT_1_PANNING_1){
		s_pContext->_output_me_sceneAnalysis.u2_panning_flag =1;
		s_pContext->_output_wrt_comreg.u1_MC_SmallObject_type=1;
	}
	else if(status_id==MEMC_STATUS_SMALLOBJECT_1_PANNING_1_RP_4){
		s_pContext->_output_me_sceneAnalysis.u2_panning_flag =1;
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true =4;
		s_pContext->_output_wrt_comreg.u1_MC_SmallObject_type=1;
	}
	else if(status_id==MEMC_STATUS_MC_LOGO_CLOSE_RP_4){
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true =4;
		s_pContext->_output_wrt_comreg.u1_MC_Logo_Close_true=1;
	}
	else if(status_id==MEMC_STATUS_SMALLOBJECT_1_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_MC_SmallObject_type=1;
		s_pContext->_output_dh_close.u1_closeDh_act =1;
	}
	else if(status_id==MEMC_STATUS_MULTIBAND_PEAKING_CLOSE){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=3;
		s_pContext->_output_wrt_comreg.u1_GFB_force_weak_type=1;
		s_pContext->_output_dh_close.u1_closeDh_act =1;
	}
	else if(status_id==MEMC_STATUS_TEST_FILM32){
		s_pContext->_output_wrt_comreg.u1_ME2_SmallObject_type=2;  // more me2
		s_pContext->_output_wrt_comreg.u1_GFB_force_zero_true=1;  // Because pq adjustment
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_wrt_comreg.u1_LFB_force_zero_true=1;
		s_pContext->_output_wrt_comreg.u1_MC_Logo_Close_true=1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_SLOW_CONVERGENCE_RP_1){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true =1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_RP_1){
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true =1;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCAND_SLOW_CONVERGENCE_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCand_Slow_Convergence_type=1;
		s_pContext->_output_dh_close.u1_closeDh_act =1;
	}
	else if(status_id==MEMC_STATUS_LFB_DTL_WEAK_3_FAST_CONV_1_CLOSEDH_ACT_DYNAMIC_STEP_0){
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=3;
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Fast_Convergence_type =1;
		s_pContext->_output_me_sceneAnalysis.u3_Dynamic_MVx_Step=0;
		s_pContext->_output_me_sceneAnalysis.u3_Dynamic_MVy_Step=0;
	}
	else if(status_id==MEMC_STATUS_ART_OF_FLIGHT_6M_33S){
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Fast_Convergence_type =2;
		s_pContext->_output_wrt_comreg.u1_GFB_force_zero_true=1;
		s_pContext->_output_wrt_comreg.u1_LFB_force_zero_true=1;
		s_pContext->_output_wrt_comreg.u1_KME_SR_increaseY_true=1;
		s_pContext->_output_wrt_comreg.u1_ME2_SmallObject_type =1;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Fast_Convergence_true=1;
	}
	else if(status_id==MEMC_STATUS_GFB_FORCE_ZERO_DH_SML_RGNACTION_CLOSE_CLOSEDH_ACT){
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_dh_close.u1_dh_sml_rgnAction_close=1;
		s_pContext->_output_wrt_comreg.u1_GFB_force_zero_true=1;
	}
	else if(status_id==MEMC_STATUS_GFB_FORCE_ZERO_LFB_DTL_WEAK_3_CLOSEDH_ACT){
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=3;
		s_pContext->_output_wrt_comreg.u1_GFB_force_zero_true=1;
	}
	else if(status_id==MEMC_STATUS_SMALLOBJECT_3_CLOSEDH_ACT_0){
		s_pContext->_output_dh_close.u1_closeDh_act =0;
		s_pContext->_output_wrt_comreg.u1_ME2_SmallObject_type=3;
	}
	else if(status_id==MEMC_STATUS_GFB_FORCE_MAX){
		s_pContext->_output_wrt_comreg.u1_GFB_force_MAX_true=1;
	}
	else if(status_id==MEMC_STATUS_RP_1_CASINO_RP_CLOSEDH_ACT_0){
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true =1;
		s_pContext->_output_dh_close.u1_closeDh_act =0;
		pOutput->u1_casino_RP_detect_true =1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_RP_1_LFB_FROCE_ZERO_CLOSEDH_ACT_LOGO_CLOSE){
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true =1;
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_wrt_comreg.u1_LFB_force_zero_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_MC_Logo_Close_true=1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_SLOW_CONVERGENCE_CLOSEDH_ACT_DH_SML_RGNACTION_CLOSE){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_dh_close.u1_dh_sml_rgnAction_close=1;
	}
	else if(status_id==MEMC_STATUS_CLOSEDH_ACT_DH_SML_RGNACTION_CLOSE){
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_dh_close.u1_dh_sml_rgnAction_close=1;
	}
	else if(status_id==MEMC_STATUS_LOGO_CLOSE){
		s_pContext->_output_wrt_comreg.u1_MC_Logo_Close_true=1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_SLOW_CONVERGENCE_KME_GMVD_COSE_1){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_KME_gmvd_cost_type=1;
	}
	else if(status_id==MEMC_STATUS_IPME_INCREASE_FLP_1){
		s_pContext->_output_wrt_comreg.u1_IPME_increase_flp_type=1;
	}
	else if(status_id==MEMC_STATUS_IPME_INCREASE_FLP_2){
		s_pContext->_output_wrt_comreg.u1_IPME_increase_flp_type=2;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
	}
	else if(status_id==MEMC_STATUS_CLOSEDH_ACT){
		s_pContext->_output_dh_close.u1_closeDh_act =1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_CLOSEDH_ACT_DH_SMLRGNACTION_CLOSE){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_dh_close.u1_dh_sml_rgnAction_close=1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_CLOSEDH_ACT_RP_4){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true =4;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_IPME_INCREASE_FLP_1_DRP_TYPE_1){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCand_Slow_Convergence_type=2;
		s_pContext->_output_wrt_comreg.u1_IPME_increase_flp_type=1;
		s_pContext->_output_wrt_comreg.u1_ME1_DRP_type = 1;
	}
	else if(status_id==MEMC_STATUS_LFB_DTL_WEAK_2){
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=2;
	}
	else if(status_id==MEMC_STATUS_IPME_INCREASE_FLP_2_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_IPME_increase_flp_type=2;
		s_pContext->_output_dh_close.u1_closeDh_act =1;
	}
	else if(status_id==MEMC_STATUS_ME1_RNDCANDDPNT_SLOW_CONVERGENCE_CLOSEDH_ACT_LFB_DTL_WEAK_1){
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
		s_pContext->_output_dh_close.u1_closeDh_act =1;
	}
	else if(status_id==MEMC_STATUS_ME1_RNDCANDDPNT_SLOW_CONVERGENCE_GFB_FROCE_WEAK_1_GFB_MODIFY_0_LFB_DTL_WEAK_1){
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_GFB_force_weak_type=1;
		s_pContext->_output_wrt_comreg.u1_GFB_modify_true=0;
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=1;
	}
	else if(status_id==MEMC_STATUS_LFB_DTL_WEAK_1_GFB_MODIFY){
		if(pOutput->u8_FB_lvl<0x60)
		{
			pOutput->u8_FB_lvl ++;
			s_pContext->_output_wrt_comreg.u1_GFB_modify_true=1;
		}
		else if( s_pContext->_output_fblevelctrl.u8_FBLevel >0x60)
		{
			s_pContext->_output_wrt_comreg.u1_GFB_modify_true=0;
		}
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_SLOW_CONVERGENCE_RP_4_CLOSEDH_ACT){
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true =4;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_SMALLOBJECT_4){
		s_pContext->_output_wrt_comreg.u1_ME2_SmallObject_type=4;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_RP_4){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true =4;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_DRP_1){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_DRP_type = 1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_SLOW_CONVERGENCE_LFB_DTL_WEAK_3){
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type =3;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_LFB_DTL_WEAK_1_CLOSEDH_ACT){
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type =1;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_SLOW_CONVERGENCE_CLOSEDH_ACT){
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
	}
	else if(status_id==MEMC_STATUS_ME1_CLOSEDH_ACT_SMALLOBJECT_3_LFB_FORCE_ZERO){
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_wrt_comreg.u1_LFB_force_zero_true=1;
		s_pContext->_output_wrt_comreg.u1_ME2_SmallObject_type=3;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
		s_pContext->_output_dh_close.u1_closeDh_act =0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_ADPTPNT_SLOW_CONVERGENCE_RNDCAND_3_RP_1_LOGO_CLOSE){
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true =1;
		s_pContext->_output_wrt_comreg.u1_MC_Logo_Close_true =1;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_adptpnt_rnd_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCand_Slow_Convergence_type=3;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_ADPTPNT_SLOW_CONVERGENCE_RNDCAND_2_RP_1_IPME_INCREASE_FLP_2_SMALLOBJECT_1){
		s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true =1;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=1;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCand_Slow_Convergence_type=2;
		s_pContext->_output_wrt_comreg.u1_ME1_adptpnt_rnd_gain_true=1;
		s_pContext->_output_wrt_comreg.u1_IPME_increase_flp_type=2;
		s_pContext->_output_wrt_comreg.u1_ME2_SmallObject_type=1;
	}
	else if(status_id==MEMC_STATUS_PEPLACEPROTECT){
		s_pContext->_external_data.u1_ReplaceProtect = true;
	}
	else if(status_id==MEMC_STATUS_402_PLANE){
		s_pContext->_external_info.u1_402_plane=true;
	}
	else if(status_id==MEMC_STATUS_IPR_DH_CLOSE){
		s_pContext->_output_dh_close.u1_IPR_dh_close = true;
	}
	else if(status_id==MEMC_STATUS_PANNING_1){
		s_pContext->_output_me_sceneAnalysis.u2_panning_flag =1;
	}
	else if(status_id==MEMC_STATUS_4K_STILL){
		s_pContext->_external_info.u1_4K_still = true;
	}
	else if(status_id==MEMC_STATUS_DH_COND_NEW_DHBYPASS){
		s_pContext->_output_frc_sceneAnalysis.u8_dh_condition_new_dhbypass_flag = 1;
	}
	else if(status_id==MEMC_STATUS_DH_COND_MIYUEZHUAN_HALO){
		s_pContext->_output_frc_sceneAnalysis.u8_dh_condition_miyuezhuan_halo = 1;
	}
	else if(status_id==MEMC_STATUS_DH_COND_MIYUEZHUAN_CARRIAGE){
		s_pContext->_output_frc_sceneAnalysis.u8_dh_condition_miyuezhuan_carriage = 1;
	}
	else if(status_id==MEMC_STATUS_DH_COND_MIYUEZHUAN_MANCROSS){
		s_pContext->_output_frc_sceneAnalysis.u8_dh_condition_miyuezhuan_mancross = 1;
	}
	else if(status_id==MEMC_STATUS_DH_COND_ROTATE_MV_BROKEN){
		s_pContext->_output_frc_sceneAnalysis.u8_dh_condition_rotate_mv_broken = 1;
	}
	else if(status_id==MEMC_STATUS_CLOSEDH_ACT_LFB_FORCE_ZERO){
		s_pContext->_output_dh_close.u1_closeDh_act =1;
		s_pContext->_output_wrt_comreg.u1_LFB_force_zero_true=1;
	}
	else if(status_id==MEMC_STATUS_SQUIDGAME_SUBTITLE)
		{s_pContext->_output_wrt_comreg.u1_SquidGame_subTitle_flag = 1;
	}
	else if(status_id==MEMC_STATUS_393_FLAG){
		g_393_flag = 1;
	}
	else if(status_id==MEMC_STATUS_GFB_FORCE_CUSTOM2){
		s_pContext->_output_wrt_comreg.u1_GFB_force_Custom_true2 = 1;
	}
}

void Restore_MEMC_Status(_OUTPUT_WRT_COM_REG *pOutput, unsigned char status_id)
{
	_PQLCONTEXT *s_pContext = GetPQLContext_m();

	if(status_id >= MEMC_STATUS_TABLE_MAX){
		rtd_pr_memc_emerg("[%s][%d] status_id(%d) exceed max(%d)\n", status_id, MEMC_STATUS_TABLE_MAX);
		return;
	}

	if(status_id==MEMC_STATUS_CASINO_RP){
		pOutput->u1_casino_RP_detect_true=0;
	}
	else if(status_id>=MEMC_STATUS_RP_0 && status_id<=MEMC_STATUS_RP_4){
		;
	}
	else if(status_id==MEMC_STATUS_HQV_MULTI_CADENCE){
		s_pContext->_output_wrt_comreg.u1_MC_Logo_Close_true =0;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_SLOW_CONVERGENCE_PANNING_2){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_Slow_Convergence_true =0;
	}
	else if(status_id==MEMC_STATUS_GFB_SPECIALSCENE_TRUE){
		s_pContext->_output_wrt_comreg.u1_GFB_specialScene_true=0;
	}
	else if(status_id==MEMC_STATUS_GFB_SPECIAL_APPLY){
		s_pContext->_output_wrt_comreg.u1_GFB_Special_apply=0;
	}
	else if(status_id==MEMC_STATUS_BLACK_19YO1){
		pOutput->u8_01_Black_19YO1_flag=0;
	}
	else if(status_id==MEMC_STATUS_Q13722_FLAG){
		g_Q13722_flag = 0;
	}
	else if(status_id==MEMC_STATUS_GFB_FORCE_CUSTOM){
		s_pContext->_output_wrt_comreg.u1_GFB_force_Custom_true=0;
	}
	else if(status_id==MEMC_STATUS_181_FLAG){
		g_181_flag = 0;
	}
	else if(status_id==MEMC_STATUS_BTS_FLAG){
		pOutput->u1_BTS_Rp_flag=0;
	}
	else if(status_id==MEMC_STATUS_LALALAND_1_14_25){
;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_SLOW_CONVERGENCE){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_Slow_Convergence_true =0;
	}
	else if(status_id==MEMC_STATUS_SMALLOBJECT_1_PANNING_1){
		s_pContext->_output_wrt_comreg.u1_MC_SmallObject_type=0;
	}
	else if(status_id==MEMC_STATUS_SMALLOBJECT_1_PANNING_1_RP_4){
		s_pContext->_output_wrt_comreg.u1_MC_SmallObject_type=0;
	}
	else if(status_id==MEMC_STATUS_MC_LOGO_CLOSE_RP_4){
		s_pContext->_output_wrt_comreg.u1_MC_Logo_Close_true=0;
	}
	else if(status_id==MEMC_STATUS_SMALLOBJECT_1_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_MC_SmallObject_type=0;
	}
	else if(status_id==MEMC_STATUS_MULTIBAND_PEAKING_CLOSE){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=0;
		s_pContext->_output_wrt_comreg.u1_GFB_force_weak_type=0;
	}
	else if(status_id==MEMC_STATUS_TEST_FILM32){
		s_pContext->_output_wrt_comreg.u1_ME2_SmallObject_type=0;
		s_pContext->_output_wrt_comreg.u1_GFB_force_zero_true=0;
		s_pContext->_output_wrt_comreg.u1_LFB_force_zero_true=0;
		s_pContext->_output_wrt_comreg.u1_MC_Logo_Close_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_SLOW_CONVERGENCE_RP_1){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_RP_1){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCAND_SLOW_CONVERGENCE_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCand_Slow_Convergence_type=0;
	}
	else if(status_id==MEMC_STATUS_LFB_DTL_WEAK_3_FAST_CONV_1_CLOSEDH_ACT_DYNAMIC_STEP_0){
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Fast_Convergence_type =0;
	}
	else if(status_id==MEMC_STATUS_ART_OF_FLIGHT_6M_33S){
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Fast_Convergence_type =0;
		s_pContext->_output_wrt_comreg.u1_GFB_force_zero_true=0;
		s_pContext->_output_wrt_comreg.u1_LFB_force_zero_true=0;
		s_pContext->_output_wrt_comreg.u1_KME_SR_increaseY_true=0;
		s_pContext->_output_wrt_comreg.u1_ME2_SmallObject_type =0;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Fast_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_GFB_FORCE_ZERO_DH_SML_RGNACTION_CLOSE_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_GFB_force_zero_true=0;
	}
	else if(status_id==MEMC_STATUS_GFB_FORCE_ZERO_LFB_DTL_WEAK_3_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=0;
		s_pContext->_output_wrt_comreg.u1_GFB_force_zero_true=0;
	}
	else if(status_id==MEMC_STATUS_SMALLOBJECT_3_CLOSEDH_ACT_0){
		s_pContext->_output_wrt_comreg.u1_ME2_SmallObject_type=0;
	}
	else if(status_id==MEMC_STATUS_GFB_FORCE_MAX){
		s_pContext->_output_wrt_comreg.u1_GFB_force_MAX_true=0;
	}
	else if(status_id==MEMC_STATUS_RP_1_CASINO_RP_CLOSEDH_ACT_0){
		pOutput->u1_casino_RP_detect_true =0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_RP_1_LFB_FROCE_ZERO_CLOSEDH_ACT_LOGO_CLOSE){
		s_pContext->_output_wrt_comreg.u1_LFB_force_zero_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_MC_Logo_Close_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_SLOW_CONVERGENCE_CLOSEDH_ACT_DH_SML_RGNACTION_CLOSE){
		s_pContext->_output_dh_close.u1_dh_sml_rgnAction_close=0;
	}
	else if(status_id==MEMC_STATUS_CLOSEDH_ACT_DH_SML_RGNACTION_CLOSE){
		s_pContext->_output_dh_close.u1_dh_sml_rgnAction_close=0;
	}
	else if(status_id==MEMC_STATUS_LOGO_CLOSE){
		s_pContext->_output_wrt_comreg.u1_MC_Logo_Close_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_SLOW_CONVERGENCE_KME_GMVD_COSE_1){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_KME_gmvd_cost_type=0;
	}
	else if(status_id==MEMC_STATUS_IPME_INCREASE_FLP_1){
		s_pContext->_output_wrt_comreg.u1_IPME_increase_flp_type=0;
	}
	else if(status_id==MEMC_STATUS_IPME_INCREASE_FLP_2){
		s_pContext->_output_wrt_comreg.u1_IPME_increase_flp_type=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_CLOSEDH_ACT){
;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_CLOSEDH_ACT_DH_SMLRGNACTION_CLOSE){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_CLOSEDH_ACT_RP_4){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_IPME_INCREASE_FLP_1_DRP_TYPE_1){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCand_Slow_Convergence_type=0;
		s_pContext->_output_wrt_comreg.u1_IPME_increase_flp_type=0;
		s_pContext->_output_wrt_comreg.u1_ME1_DRP_type = 0;
	}
	else if(status_id==MEMC_STATUS_LFB_DTL_WEAK_2){
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=0;
	}
	else if(status_id==MEMC_STATUS_IPME_INCREASE_FLP_2_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_IPME_increase_flp_type=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RNDCANDDPNT_SLOW_CONVERGENCE_CLOSEDH_ACT_LFB_DTL_WEAK_1){
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RNDCANDDPNT_SLOW_CONVERGENCE_GFB_FROCE_WEAK_1_GFB_MODIFY_0_LFB_DTL_WEAK_1){
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_GFB_force_weak_type=0;
		s_pContext->_output_wrt_comreg.u1_GFB_modify_true=0;
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=0;
	}
	else if(status_id==MEMC_STATUS_LFB_DTL_WEAK_1_GFB_MODIFY){
		s_pContext->_output_wrt_comreg.u1_GFB_modify_true=0;
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_SLOW_CONVERGENCE_RP_4_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_SMALLOBJECT_4){
		s_pContext->_output_wrt_comreg.u1_ME2_SmallObject_type=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_RP_4){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_DRP_1){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_DRP_type = 0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_SLOW_CONVERGENCE_LFB_DTL_WEAK_3){
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type =0;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_LFB_DTL_WEAK_1_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_LFB_Dtl_weak_type =0;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_SLOW_CONVERGENCE_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_CLOSEDH_ACT_SMALLOBJECT_3_LFB_FORCE_ZERO){
		s_pContext->_output_wrt_comreg.u1_LFB_force_zero_true=0;
		s_pContext->_output_wrt_comreg.u1_ME2_SmallObject_type=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_SLOW_CONVERGENCE_CLOSEDH_ACT){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_ADPTPNT_SLOW_CONVERGENCE_RNDCAND_3_RP_1_LOGO_CLOSE){
		s_pContext->_output_wrt_comreg.u1_MC_Logo_Close_true =0;
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_adptpnt_rnd_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCand_Slow_Convergence_type=0;
	}
	else if(status_id==MEMC_STATUS_ME1_RND_MASK_RNDCANDDPNT_ADPTPNT_SLOW_CONVERGENCE_RNDCAND_2_RP_1_IPME_INCREASE_FLP_2_SMALLOBJECT_1){
		s_pContext->_output_wrt_comreg.u1_ME1_rnd_mask_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCanddpnt_Slow_Convergence_true=0;
		s_pContext->_output_wrt_comreg.u1_ME1_rndCand_Slow_Convergence_type=0;
		s_pContext->_output_wrt_comreg.u1_ME1_adptpnt_rnd_gain_true=0;
		s_pContext->_output_wrt_comreg.u1_IPME_increase_flp_type=0;
		s_pContext->_output_wrt_comreg.u1_ME2_SmallObject_type=0;
	}
	else if(status_id==MEMC_STATUS_PEPLACEPROTECT){
		s_pContext->_external_data.u1_ReplaceProtect = false;
	}
	else if(status_id==MEMC_STATUS_402_PLANE){
		s_pContext->_external_info.u1_402_plane=false;
	}
	else if(status_id==MEMC_STATUS_IPR_DH_CLOSE){
		s_pContext->_output_dh_close.u1_IPR_dh_close = false;
	}
	else if(status_id==MEMC_STATUS_PANNING_1){
		;
	}
	else if(status_id==MEMC_STATUS_4K_STILL){
		s_pContext->_external_info.u1_4K_still = false;
	}
	else if(status_id==MEMC_STATUS_DH_COND_NEW_DHBYPASS){
		s_pContext->_output_frc_sceneAnalysis.u8_dh_condition_new_dhbypass_flag = 0;
	}
	else if(status_id==MEMC_STATUS_DH_COND_MIYUEZHUAN_HALO){
		s_pContext->_output_frc_sceneAnalysis.u8_dh_condition_miyuezhuan_halo = 0;
	}
	else if(status_id==MEMC_STATUS_DH_COND_MIYUEZHUAN_CARRIAGE){
		s_pContext->_output_frc_sceneAnalysis.u8_dh_condition_miyuezhuan_carriage = 0;
	}
	else if(status_id==MEMC_STATUS_DH_COND_MIYUEZHUAN_MANCROSS){
		s_pContext->_output_frc_sceneAnalysis.u8_dh_condition_miyuezhuan_mancross = 0;
	}
	else if(status_id==MEMC_STATUS_DH_COND_ROTATE_MV_BROKEN){
		s_pContext->_output_frc_sceneAnalysis.u8_dh_condition_rotate_mv_broken = 0;
	}
	else if(status_id==MEMC_STATUS_CLOSEDH_ACT_LFB_FORCE_ZERO){
		s_pContext->_output_dh_close.u1_closeDh_act =0;
		s_pContext->_output_wrt_comreg.u1_LFB_force_zero_true=0;
	}
	else if(status_id==MEMC_STATUS_SQUIDGAME_SUBTITLE)
		{s_pContext->_output_wrt_comreg.u1_SquidGame_subTitle_flag = 0;
	}
	else if(status_id==MEMC_STATUS_393_FLAG){
		g_393_flag = 0;
	}
	else if(status_id==MEMC_STATUS_GFB_FORCE_CUSTOM2){
		s_pContext->_output_wrt_comreg.u1_GFB_force_Custom_true2 = 0;
	}
}

void MEMC_Performance_Checking_Database(void)
{
	static unsigned int register_return[MEMC_PERFORMANCE_SETTING_ITEM_MAX] = {};
	int i = 0;
	unsigned int check_en = 0, reg_addr = 0, low_bit = 0, high_bit = 0, reg_val = 0;
	unsigned int check_index = Scaler_MEMC_Get_Performance_Checking_Database_index();
	unsigned short item_size = 0, database_size = get_database_table_size();
	static unsigned int pre_check_index = 0;
	ReadRegister(SOFTWARE1_SOFTWARE1_42_reg, 19, 19, &check_en);	//to clear any other MEMC isuue is not related to the desgin

	if(check_en == 0 || (pre_check_index == 0 && check_index == 0) || (pre_check_index >= database_size) || (check_index >= database_size))
		return;

	if(MEMC_setting_Vaild[check_index]==false)
		return;

	//Return value when switching(first time no need to return value)
	if(pre_check_index != check_index && pre_check_index != 0)
	{
		item_size = get_database_item_size(pre_check_index);
		for(i = 1; i < item_size; i++) // 4 (MEMC_PERFORMANCE_SETTING_MAX) * 4(unsigned int) = 16
		{
			reg_addr = get_database_reg_addr(pre_check_index, i);
			if(reg_addr != 0){
				low_bit = get_database_low_bit(pre_check_index, i);
				high_bit = get_database_high_bit(pre_check_index, i);
				WriteRegister(reg_addr, low_bit, high_bit, register_return[i]);
			}
		}
	}
	
	item_size = get_database_item_size(check_index);
	for(i = 1; i < item_size; i++) // 4 (MEMC_PERFORMANCE_SETTING_MAX) * 4(unsigned int) = 16
	{
		reg_addr = get_database_reg_addr(check_index, i);
		low_bit = get_database_low_bit(check_index, i);
		high_bit = get_database_high_bit(check_index, i);
		//store the value when switching
		if((pre_check_index != check_index) && (check_index != 0) && reg_addr != 0)
		{
			ReadRegister(reg_addr, low_bit, high_bit,&register_return[i]);
		}
		//write table value
		if(check_index != 0 && reg_addr != 0)
		{
			reg_val = get_database_reg_val(check_index, i);
			WriteRegister(reg_addr, low_bit, high_bit, reg_val);
		}
	}
	pre_check_index = check_index;
}

void memc_identify_ctrl_init(void)
{
	g_Sizeof_MEMC_setting_Table_RTK2885p = (sizeof(MEMC_setting_Table_RTK2885p)/sizeof(MEMC_setting_Table_RTK2885p[0]));
	g_Sizeof_MEMC_setting_Table_RTK2885pp = (sizeof(MEMC_setting_Table_RTK2885pp)/sizeof(MEMC_setting_Table_RTK2885pp[0]));

	// check the setting table
	Check_SettingTable();

	// update the number of identify patern with project
	Update_Pattern_Num();
	Check_DynamicOptimizeArray();

	memset(&DynamicOptimizeMEMC_HoldCnt, 0, sizeof(DynamicOptimizeMEMC_HoldCnt));
	memset(&MEMC_Status_HoldCnt, 0, sizeof(MEMC_Status_HoldCnt));
	memset(&DynamicOptimizeMEMC_RestoreEnable, 0, sizeof(DynamicOptimizeMEMC_RestoreEnable));
}

bool is_End_identify_sturct(_IDENTIFY_SINGLE_PATTERN_INFO *Idendify_Info)
{
	if(Idendify_Info->array_len != END_ARRAY_LEN){
		return false;
	}
	if(Idendify_Info->Param.Y_threshold != END_THRESHOLD){
		return false;
	}
	if(Idendify_Info->Param.Hue_threshold != END_THRESHOLD){
		return false;
	}
	return true;
}

void Update_Pattern_Num(void)
{
#if NEW_IDENTIFY_FLOW_EN
	_IDENTIFY_INFO *Idendify_Info = NULL;
	unsigned char project_id = 0, u8_i = 0, pattern_num = 0;

	for(project_id=0; project_id<VIP_Project_ID_MAX; project_id++){
		// only update with support project
		if(is_Prj_support_indentify(project_id)==false){
			continue;
		}

		pattern_num = 0;
		Idendify_Info = get_idendify_Info(project_id);
		// get the pattern number
		for(u8_i=0; u8_i<255; u8_i++){
			if(is_End_identify_sturct(&(Idendify_Info->idendify_info[u8_i]))==true){
				pattern_num = u8_i;
				break;
			}
		}
		// update the pattern number
		Idendify_Info->pattern_num = pattern_num;
		rtd_pr_memc_notice("[%s] Pro,%d, pattern num,%d\n", __FUNCTION__, project_id, Idendify_Info->pattern_num);
	}
#endif
}

void Update_show_delay_time(void)
{
	unsigned int show_delay_time;
	ReadRegister(SOFTWARE2_SOFTWARE2_46_reg, 16, 23, &show_delay_time);

	g_show_delay_time = show_delay_time;
}

unsigned char Get_show_delay_time(void)
{
	return g_show_delay_time;
}

void Identify_Pattern_Clear_Data(void)
{
	memset(&DynamicOptimizeMEMC_HoldCnt, 0, sizeof(DynamicOptimizeMEMC_HoldCnt));
	memset(&MEMC_Status_HoldCnt, 0, sizeof(MEMC_Status_HoldCnt));
}

bool Identify_Pattern_API(_IDENTIFY_SINGLE_PATTERN_INFO * idendify_info, unsigned char project_id, unsigned char pattern_id)
{
	static unsigned char u8_show_cnt = 0;
	unsigned char u8_i = 0, set_id = 0, status_id;
	unsigned short hue_ratio=0, y_ratio=0;
	unsigned int algo_en = 0;
	unsigned char u8_array_len = idendify_info->array_len;
	unsigned short y_ratio_th = idendify_info->Param.Y_threshold;
	unsigned short hue_ratio_th = idendify_info->Param.Hue_threshold;
	unsigned char special_scene_id = idendify_info->Param.special_scene_id;
	unsigned short database_size = get_database_table_size();
	unsigned char special_scene_maxid = get_special_scene_maxid();
	unsigned char status_table_maxid =get_status_table_maxid();
	bool u1_show = false;

	ReadRegister(SOFTWARE2_SOFTWARE2_45_reg,  6,  6, &algo_en);
	if(algo_en==false){
		return false;
	}

	if(u8_show_cnt >= Get_show_delay_time()){
		u1_show = true;
	}else {
		u8_show_cnt++;
	}

	for(u8_i=0; u8_i<u8_array_len; u8_i++){
		y_ratio = scalerVIP_ratio_inner_product((unsigned short *)(idendify_info->YH[YH_INDEX_Y][u8_i]), &Y_hist_ratio[0], TV006_VPQ_chrm_bin);
		hue_ratio = scalerVIP_ratio_inner_product((unsigned short *)(idendify_info->YH[YH_INDEX_H][u8_i]), &hue_hist_ratio[0], COLOR_HUE_HISTOGRAM_LEVEL);

		//Start_Print.YH_Show_en: 0xB809D8B8[15]
		//Start_Print.YH_Show_Pattern_id: 0xB809D8B8[0:9]
		//Start_Print.YH_Show_Project_id: 0xB809D8B8[10:14]
		if(u1_show && Start_Print.YH_Show_en && project_id==Start_Print.YH_Show_Project_id && pattern_id==Start_Print.YH_Show_Pattern_id) {
			u8_show_cnt = 0;
			rtd_pr_memc_emerg("Pro=%d, Pattern=%d, Row=%d, y_ratio=%d, hue_ratio=%d \n", project_id, pattern_id, u8_i, y_ratio, hue_ratio);
		}
		special_scene_id = (special_scene_id>=special_scene_maxid) ? 0 : special_scene_id;
		if(y_ratio>=y_ratio_th && hue_ratio>=hue_ratio_th && DynamicOptimizeMEMC_SpecialScene[special_scene_id]==true){
			set_id = idendify_info->Param.set_id;
			set_id = (set_id>=database_size) ? 0 : set_id;
			status_id = idendify_info->Param.status_id;
			status_id = (status_id>=status_table_maxid) ? 0 : status_id;

			DynamicOptimizeMEMC_HoldCnt[set_id] = idendify_info->Param.hold_cnt;
			MEMC_Status_HoldCnt[status_id] = idendify_info->Param.hold_cnt;

			return true;
		}
	}
	return false;
}

#if 0
void DynamicOptimizeArray_Show(void)
{
	unsigned int u1_show_en;
	unsigned int project_id = Start_Print.YH_Show_Project_id;//0xB809D8B8[10:14]
	unsigned int pattern_id = Start_Print.YH_Show_Pattern_id;//0xB809D8B8[0:9]
	unsigned char u8_i, u8_j, array_len = 0;
	_IDENTIFY_INFO *Idendify_Info = NULL;
	char data_string[255] = "";
	char tmp_string[8] = "";

	ReadRegister(SOFTWARE2_SOFTWARE2_46_reg, 30, 30, &u1_show_en);

	if(!u1_show_en){
		return;
	}

	WriteRegister(SOFTWARE2_SOFTWARE2_46_reg, 30, 30, 0);
	
	Idendify_Info = get_idendify_Info(project_id);
	array_len =Idendify_Info->idendify_info[pattern_id].array_len;

	rtd_pr_memc_emerg("[%s] Array length: %u\n", __FUNCTION__, Idendify_Info->idendify_info[pattern_id].array_len);

	rtd_pr_memc_emerg("[%s] Y: \n", __FUNCTION__);
	for(u8_i=0; u8_i<array_len; u8_i++){
		memset(&data_string, 0, sizeof(data_string));
		strcat(data_string, "{");

		for(u8_j=0; u8_j<32; u8_j++){
			memset(&tmp_string, 0, sizeof(tmp_string));
			UnsignedValue_To_Char(Idendify_Info->idendify_info[pattern_id].YH[YH_INDEX_Y][u8_i][u8_j], tmp_string);
			strcat(tmp_string, ",");
			strcat(data_string, tmp_string);
		}
		strcat(data_string, "},\n");
		rtd_pr_memc_emerg("[%s]	%s\n", __FUNCTION__, data_string);
	}

	rtd_pr_memc_emerg("[%s] H: \n", __FUNCTION__);
	for(u8_i=0; u8_i<array_len; u8_i++){
		memset(&data_string, 0, sizeof(data_string));
		strcat(data_string, "{");

		for(u8_j=0; u8_j<32; u8_j++){
			memset(&tmp_string, 0, sizeof(tmp_string));
			UnsignedValue_To_Char(Idendify_Info->idendify_info[pattern_id].YH[YH_INDEX_H][u8_i][u8_j], tmp_string);
			strcat(tmp_string, ",");
			strcat(data_string, tmp_string);
		}
		strcat(data_string, "},\n");
		rtd_pr_memc_emerg("[%s]	%s\n", __FUNCTION__, data_string);
	}

	rtd_pr_memc_emerg("[%s] INFO: \n", __FUNCTION__);
	rtd_pr_memc_emerg("[%s]	Y th: %u\n", __FUNCTION__, Idendify_Info->idendify_info[pattern_id].Param.Y_threshold);
	rtd_pr_memc_emerg("[%s]	H th: %u\n", __FUNCTION__, Idendify_Info->idendify_info[pattern_id].Param.Hue_threshold);
	rtd_pr_memc_emerg("[%s]	hold: %u\n", __FUNCTION__, Idendify_Info->idendify_info[pattern_id].Param.hold_cnt);
	rtd_pr_memc_emerg("[%s]	s id: %u\n", __FUNCTION__, Idendify_Info->idendify_info[pattern_id].Param.set_id);
}
#endif

void Check_SettingTable(void)
{
	unsigned char u8_i, set_id, bit_diff;
	unsigned int reg_addr, low_bit, high_bit, reg_value, set_enum_id;
	unsigned long long reg_max_val = 0;
	bool isValid = true;
	unsigned short database_size = get_database_table_size();

	memset(&MEMC_setting_Vaild, 0, sizeof(MEMC_setting_Vaild));

	rtd_pr_memc_notice("[%s][%s] setting_num,%d\n", __FUNCTION__, __MEMC_CHECK_SETTING_TABLE__, database_size);

	//check reg_addr, bit, reg_value isn't zero
	for(set_id=1; set_id<database_size; set_id++){
		set_enum_id = get_database_enum_id(set_id, 0);
		if(set_enum_id!=set_id){
			rtd_pr_memc_notice("[%s][%s] ERROR(set_enum_id is abnormal) set_enum_id,%u, set_id,%u\n", __FUNCTION__, __MEMC_CHECK_SETTING_TABLE__, set_enum_id, set_id);
			continue;
		}

		isValid = true;
		
		for(u8_i=1; u8_i<MEMC_PERFORMANCE_SETTING_ITEM_MAX; u8_i++){
			// system won't check with empty setting
			if(Setting_Row_Is_Empty(set_id, u8_i)==true){
				continue;
			}

			reg_addr = get_database_reg_addr(set_id,u8_i);
			high_bit = get_database_high_bit(set_id,u8_i);
			low_bit = get_database_low_bit(set_id,u8_i);
			reg_value = get_database_reg_val(set_id,u8_i);
			if(reg_addr_is_right(reg_addr)==false){
				rtd_pr_memc_notice("[%s][%s] ERROR(reg_addr is abnormal) set_id,%d, row_index,%d, reg_addr,0x%x\n", __FUNCTION__, __MEMC_CHECK_SETTING_TABLE__, set_id, u8_i, reg_addr);
				isValid = false;
			}
			if(high_bit>=32){
				rtd_pr_memc_notice("[%s][%s] ERROR(bitup is abnormal) set_id,%d, row_index,%d, high_bit,%u\n", __FUNCTION__, __MEMC_CHECK_SETTING_TABLE__, set_id, u8_i, high_bit);
				isValid = false;
			}
			if(low_bit>=32){
				rtd_pr_memc_notice("[%s][%s] ERROR(bitlow is abnormal) set_id,%d, row_index,%d, low_bit,%u\n", __FUNCTION__, __MEMC_CHECK_SETTING_TABLE__, set_id, u8_i, low_bit);
				isValid = false;
			}
			if(low_bit>high_bit){
				rtd_pr_memc_notice("[%s][%s] ERROR(bitlow > bitup) set_id,%d, row_index,%d, low_bit,%u, high_bit,%u\n", __FUNCTION__, __MEMC_CHECK_SETTING_TABLE__, set_id, u8_i, low_bit, high_bit);
				isValid = false;
			}
			bit_diff = _MIN_(high_bit-low_bit, 31);
			reg_max_val = BIT_VAL[bit_diff];
			if(reg_value>reg_max_val){
				rtd_pr_memc_notice("[%s][%s] ERROR(regvalue is abnormal) set_id,%d, row_index,%d, reg_value,%u\n", __FUNCTION__, __MEMC_CHECK_SETTING_TABLE__, set_id, u8_i, reg_value);
				isValid = false;
			}
		}
		
		MEMC_setting_Vaild[set_id] = isValid;
	}
}

bool Check_Single_DynamicOptimizeArray(_IDENTIFY_SINGLE_PATTERN_INFO *Idendify_Info, unsigned char project_id, unsigned char pattern_id, bool show_log)
{
	unsigned short sum_Y = 0, sum_H = 0;
	unsigned char u8_i = 0, row_index = 0;
	unsigned char array_len = Idendify_Info->array_len;
	unsigned short database_size = get_database_table_size();
	bool result = true;

	if(array_len==0){
		result = false;
		if(show_log){
			rtd_pr_memc_notice("[%s][%s] ERROR(Array is empty): project_id,%d, pattern_id,%d\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, project_id, pattern_id);
		}
	}
	else if(array_len>YH_HIST_ARRAY_LIST_MAX_LEN){
		result = false;
		if(show_log){
			rtd_pr_memc_notice("[%s][%s] ERROR(Array too long): project_id,%d, pattern_id,%d, array_len,%d\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, project_id, pattern_id, array_len);
		}
	}
	else{
		row_index = array_len-1;
		sum_Y = 0;
		sum_H = 0;

		for(u8_i=0; u8_i<32; u8_i++){
			sum_Y+= Idendify_Info->YH[YH_INDEX_Y][row_index][u8_i];
			sum_H+= Idendify_Info->YH[YH_INDEX_H][row_index][u8_i];
		}
		//check whether YH array is empty
		if(sum_Y==0){
			result = false;
			if(show_log){
				rtd_pr_memc_notice("[%s][%s] ERROR(Y array is empty): project_id,%d, pattern_id,%d, row_index,%d\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, project_id, pattern_id, row_index);
			}
		}
		if(sum_H==0){
			result = false;
			if(show_log){
				rtd_pr_memc_notice("[%s][%s] ERROR(H array is empty): project_id,%d, pattern_id,%d, row_index,%d\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, project_id, pattern_id, row_index);
			}
		}
	}

	//check whether YH array is match or not by checking next row of last row should be empty
	if(array_len<YH_HIST_ARRAY_LIST_MAX_LEN){
		row_index = array_len;
		for(u8_i=0; u8_i<32; u8_i++){
			if(Idendify_Info->YH[YH_INDEX_Y][row_index][u8_i] != 0){
				result = false;
				if(show_log){
					rtd_pr_memc_notice("[%s][%s] ERROR(Y array is not empty): project_id,%d, pattern_id,%d, array_len,%d, array_index,%d, num,%d\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, 
						project_id, pattern_id, row_index, u8_i, Idendify_Info->YH[YH_INDEX_Y][row_index][u8_i]);
				}
				break;
			}
			if(Idendify_Info->YH[YH_INDEX_H][row_index][u8_i] != 0){
				result = false;
				if(show_log){
					rtd_pr_memc_notice("[%s][%s] ERROR(H array is not empty): project_id,%d, pattern_id,%d, array_len,%d, array_index,%d, num,%d\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, 
						project_id, pattern_id, row_index, u8_i, Idendify_Info->YH[YH_INDEX_H][row_index][u8_i]);
				}
				break;
			}
		}
	}
	//check status id
	if(Idendify_Info->Param.status_id>=MEMC_STATUS_TABLE_MAX){
		result = false;
		if(show_log){
			rtd_pr_memc_notice("[%s][%s] ERROR(status_id exceed max(%d): project_id,%d, pattern_id,%d, status_id,%d\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, MEMC_STATUS_TABLE_MAX, project_id, pattern_id, Idendify_Info->Param.status_id);
		}
	}

	//check special scene id
	if(Idendify_Info->Param.special_scene_id>=MEMC_SPECIAL_SCENE_TABLE_MAX){
		result = false;
		if(show_log){
			rtd_pr_memc_notice("[%s][%s] ERROR(special_scene_id exceed max(%d): project_id,%d, pattern_id,%d, special_scene_id,%d\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, MEMC_SPECIAL_SCENE_TABLE_MAX, project_id, pattern_id, Idendify_Info->Param.special_scene_id);
		}
	}

	//check setting
	if(Idendify_Info->Param.status_id==0 && Idendify_Info->Param.set_id==0){
		result = false;
		if(show_log){
			rtd_pr_memc_notice("[%s][%s] ERROR(no set status or register): project_id,%d, pattern_id,%d,\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, project_id, pattern_id);
		}
	}

	//check y threshold
	if(Idendify_Info->Param.Y_threshold>MAX_Y_THRESHOLD){
		result = false;
		if(show_log){
			rtd_pr_memc_notice("[%s][%s] ERROR(Y threshold exceed max threshold(%d)): project_id,%d, pattern_id,%d, y_threhsold,%d\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, MAX_Y_THRESHOLD,
										project_id, pattern_id, Idendify_Info->Param.Y_threshold);
		}
	}
	//check hue threshold
	if(Idendify_Info->Param.Hue_threshold>MAX_HUE_THRESHOLD){
		result = false;
		if(show_log){
			rtd_pr_memc_notice("[%s][%s] ERROR(Hue threshold exceed max threshold(%d)): project_id,%d, pattern_id,%d, hue_threhsold,%d\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, MAX_HUE_THRESHOLD,
										project_id, pattern_id, Idendify_Info->Param.Hue_threshold);
		}
	}
	//check setting index
	if(Idendify_Info->Param.set_id>=database_size){
		result = false;
		if(show_log){
			rtd_pr_memc_notice("[%s][%s] ERROR(setting index exceed max value(%d)): project_id,%d, pattern_id,%d, set_id,%d\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, database_size,
										project_id, pattern_id, Idendify_Info->Param.set_id);
		}
	}

	return result;
}

void Check_DynamicOptimizeArray(void)
{
#if NEW_IDENTIFY_FLOW_EN
	unsigned char project_id, pattern_id, max_pattern_num;
	_IDENTIFY_INFO *Idendify_Info = NULL;

	for(project_id=0; project_id<VIP_Project_ID_MAX; project_id++){
		if(is_Prj_support_indentify(project_id)==false){
			continue;
		}
	
		Idendify_Info = get_idendify_Info(project_id);
		max_pattern_num = Idendify_Info->pattern_num;
		rtd_pr_memc_notice("[%s][%s] Pro,%d, max_pattern,%d\n", __FUNCTION__, __MEMC_IDENTIFY_DATA__, project_id, max_pattern_num);

		for(pattern_id=0; pattern_id<max_pattern_num; pattern_id++){
			Check_Single_DynamicOptimizeArray(&(Idendify_Info->idendify_info[pattern_id]), project_id, pattern_id, true);
		}
	}
#endif
}

void Identify_ctrl_Self_Check(void)
{
	unsigned int u1_check_Setting_table = 0, u1_check_bin_info = 0, u1_check_Identify_Info_table = 0;

	ReadRegister(SOFTWARE2_SOFTWARE2_46_reg, 29, 29, &u1_check_Setting_table);
	ReadRegister(SOFTWARE2_SOFTWARE2_46_reg, 30, 30, &u1_check_bin_info);
	ReadRegister(SOFTWARE2_SOFTWARE2_46_reg, 31, 31, &u1_check_Identify_Info_table);

	if(u1_check_Setting_table){
		Check_SettingTable();
		WriteRegister(SOFTWARE2_SOFTWARE2_46_reg, 29, 29, 0);
	}
	if(u1_check_bin_info){
		Check_MEMC_identify_Bin();
		WriteRegister(SOFTWARE2_SOFTWARE2_46_reg, 30, 30, 0);
	}
	if(u1_check_Identify_Info_table){
		Check_DynamicOptimizeArray();
		WriteRegister(SOFTWARE2_SOFTWARE2_46_reg, 31, 31, 0);
	}
	// for debug
	//DynamicOptimizeArray_Show();
}

#if DETECT_SCENE_CONFIG
void Identify_ctrl_detect_pattern(void)
{
	_PQLCONTEXT *s_pContext_write = GetPQLContext_m();
	unsigned char project_id = 0, pattern_id = 0, bin_pattern_num = 0;
	unsigned int log_en, bin_log_en;
	bool u1_detect = false, u1_bin_detect = false, u1_tool_detect = false;
#if NEW_IDENTIFY_FLOW_EN
	_IDENTIFY_INFO *Idendify_Info = NULL;
	unsigned char max_pattern_num = 0;
#endif

	ReadRegister(SOFTWARE1_SOFTWARE1_22_reg, 24, 24, &log_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_46_reg, 26, 26, &bin_log_en);

	Update_show_delay_time();

#if NEW_IDENTIFY_FLOW_EN
	// get the identify table from kdriver(memc_identify_tv001.h)
	for(project_id=0; project_id<VIP_Project_ID_MAX; project_id++){
		if(is_indentify_Info_enable(project_id)==false){
			continue;
		}
		// get the identify info table
		Idendify_Info = get_idendify_Info(project_id);
		
		// get the max pattern number
		max_pattern_num = Idendify_Info->pattern_num;

		// identify the pattern
		for(pattern_id=0; pattern_id<max_pattern_num; pattern_id++){
			u1_detect = Identify_Pattern_API(&Idendify_Info->idendify_info[pattern_id], project_id, pattern_id);

			if(log_en && u1_detect){
				rtd_pr_memc_emerg("[%s] project_id,%d, pattern_id,%d\n", __FUNCTION__, project_id, pattern_id);
			}
		}
	}
#endif

	// get the identify table from bin
	project_id = PROJECT_ID_BIN;
	bin_pattern_num = g_IDENTIFY_TABLE.IDENTIFY_TABLE.pattern_num;
	// idenitfy pattern with info from bin
	for(pattern_id=0; pattern_id<bin_pattern_num; pattern_id++){
		if(g_Bin_Table_isValid[pattern_id]==false){
			continue;
		}
		u1_detect = Identify_Pattern_API(&g_IDENTIFY_TABLE.IDENTIFY_TABLE.idendify_info[pattern_id], project_id, pattern_id);

		if(u1_detect){
			if(bin_log_en){
				rtd_pr_memc_emerg("[BIN] pattern_id,%d\n", pattern_id);
			}
			//update the identification flag for bin
			u1_bin_detect = true;
		}
	}

	// idenitfy pattern with info from tool
	u1_tool_detect = Identify_Pattern_API(&DynamicOptimizeMEMC_Info_Tool, 0, 0);
	
	//update the identification flag for bin
	s_pContext_write->_output_me_sceneAnalysis.u1_bin_detect_pattern = u1_bin_detect;

	// get the identify table from tool and update the identification flag
	s_pContext_write->_output_me_sceneAnalysis.u1_tool_detect_pattern = u1_tool_detect;
}
#endif //DETECT_SCENE_CONFIG

void Identify_Restore_All_Setting(void)
{
	unsigned char u8_i = 0;
	unsigned short database_size = get_database_table_size();

	for(u8_i=1; u8_i<database_size; u8_i++){
		if(DynamicOptimizeMEMC_HoldCnt[u8_i]>0){
			//restore setting
			memc_identify_ctrl_apply_setting(u8_i, MEMC_PERFORMANCE_SETTING_OPTION_RESTORE);

			//update hold cnt
			DynamicOptimizeMEMC_HoldCnt[u8_i]=0;
		}
	}
}

#if APPLY_SETTING_CONFIG
void Identify_ctrl_wrtAction(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	unsigned int u32_ID_Log_en=0;
	unsigned char u8_i = 0, u8_set_option = 0;
	unsigned short database_size = get_database_table_size();

	ReadRegister(SOFTWARE2_SOFTWARE2_46_reg, 28, 28, &u32_ID_Log_en);

	if(pParam->u1_Identification_Pattern_en== 0)
		return;

	// clear data when bypass identify pattern
	if(g_MEMC_Identify_Pattern_Clear){
		Identify_Restore_All_Setting();
		return;
	}

	for(u8_i=1; u8_i<database_size; u8_i++){
		if(DynamicOptimizeMEMC_HoldCnt[u8_i]>0){
			//update info
			pOutput->u1_IP_wrtAction_true=1;

			// hold cnt=1->restore the register, others->apply setting
			u8_set_option = (DynamicOptimizeMEMC_HoldCnt[u8_i]==1) ? MEMC_PERFORMANCE_SETTING_OPTION_RESTORE : MEMC_PERFORMANCE_SETTING_OPTION_SET;

			//write setting
			memc_identify_ctrl_apply_setting(u8_i, u8_set_option);

			//update hold cnt
			DynamicOptimizeMEMC_HoldCnt[u8_i]--;

			//show log
			if(u32_ID_Log_en==1){
				rtd_pr_memc_emerg("[%s] set_id,%d, hold_cnt,%d\n", __FUNCTION__, u8_i, DynamicOptimizeMEMC_HoldCnt[u8_i]);
			}
		}
	}
}
#endif //APPLY_SETTING_CONFIG

void Identify_Restore_All_Status(_OUTPUT_WRT_COM_REG *pOutput)
{
	unsigned char u8_i = 0;

	for(u8_i=1; u8_i<MEMC_STATUS_TABLE_MAX; u8_i++){
		if(MEMC_Status_HoldCnt[u8_i]>0){
			//restore status
			Restore_MEMC_Status(pOutput, u8_i);

			//update hold cnt
			MEMC_Status_HoldCnt[u8_i]=0;
		}
	}
}

void Identify_ctrl_Status_wrtAction(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput)
{
	unsigned int u32_ID_Log_en=0;
	unsigned char u8_i = 0, u8_set_option = 0;
	
	ReadRegister(SOFTWARE2_SOFTWARE2_46_reg, 28, 28, &u32_ID_Log_en);

	if(pParam->u1_Identification_Pattern_en== 0)
		return;

	// clear data when bypass identify pattern
	if(g_MEMC_Identify_Pattern_Clear){
		Identify_Restore_All_Status(pOutput);
		return;
	}

	for(u8_i=1; u8_i<MEMC_STATUS_TABLE_MAX; u8_i++){
		if(MEMC_Status_HoldCnt[u8_i]>0){
			//update info
			pOutput->u1_IP_wrtAction_true=1;

			// hold cnt=1->restore the register, others->apply setting
			u8_set_option = (MEMC_Status_HoldCnt[u8_i]==1) ? MEMC_STATUS_SETTING_OPTION_RESTORE : MEMC_STATUS_SETTING_OPTION_SET;

			// set status
			if(u8_set_option == MEMC_STATUS_SETTING_OPTION_SET){
				Set_MEMC_Status(pOutput, u8_i);
			}
			else if(u8_set_option == MEMC_STATUS_SETTING_OPTION_RESTORE){
				Restore_MEMC_Status(pOutput, u8_i);
			}

			//update hold cnt
			MEMC_Status_HoldCnt[u8_i]--;

			//show log
			if(u32_ID_Log_en==1){
				rtd_pr_memc_emerg("[%s] status_id,%d, hold_cnt,%d\n", __FUNCTION__, u8_i, MEMC_Status_HoldCnt[u8_i]);
			}
		}
	}
	
}

bool Identify_ctrl_get_specal_scene_status(unsigned char u8_index)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	//_clues* SmartPic_clue=NULL;
	//SmartPic_clue = scaler_GetShare_Memory_SmartPic_Clue_Struct();
	PQL_OUTPUT_RESOLUTION output_Resolution = s_pContext->_external_data._output_resolution;
	const _Param_By_Resolution_RimCtrl *s_pRimParam = GetRimParam(output_Resolution);
	PQL_INPUT_FRAME_RATE in_fmRate = s_pContext->_external_data._input_frameRate;
	PQL_OUTPUT_FRAME_RATE out_fmRate = s_pContext->_external_data._output_frameRate;
	unsigned char  u8_Mixmode_flag = s_pContext->_output_filmDetectctrl.u1_mixMode;
	unsigned char  u8_cadence_Id = s_pContext->_output_filmDetectctrl.u8_cur_cadence_out[_FILM_ALL];
	signed short u11_gmv_mvx = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	signed short u10_gmv_mvy = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
	unsigned int u25_me_aDTL_rb = s_pContext->_output_read_comreg.u25_me_aDTL_rb;
	unsigned int u26_APLp = s_pContext->_output_read_comreg.u26_me_aAPLp_rb;
	unsigned short u12_me_GMV_1st_unconf_rb = s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb;
	unsigned char u1_still_frame = s_pContext->_output_frc_sceneAnalysis.u1_still_frame;
	unsigned int u25_rgnSad_rb12 = s_pContext->_output_read_comreg.u25_me_rSAD_rb[12];
	unsigned int u25_rgnSad_rb23 = s_pContext->_output_read_comreg.u25_me_rSAD_rb[23];
	unsigned char u8_i = 0, positive_rmvx_counter = 0;
	unsigned short hue_ratio = 0;
	unsigned int u32_RB_Value = 0, u32_pattern298_zidoo_player_forbindden_en = 0, vtest=0;
	unsigned int special_pattern_transporter=0, rmvx7_for_miyuezhuan=0, dtl_index=0,dtl_cnt=0, readval440=0;
	unsigned int BOT_ratio = ((_ABS_(s_pContext->_output_rimctrl.u12_out_resolution[_RIM_BOT] - s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_BOT]))<<10)>>(9+s_pRimParam->u8_V_Divide); //2160: u8_V_Divide=2, 1080: u8_V_Divide=1
	unsigned int score = scaler_GetShare_Memory_SmartPic_Clue_Struct()->score_NaturalImage;
	ReadRegister(KME_ME1_BG1_ME_BG_DUMMY_0_reg, 18, 18, &special_pattern_transporter);

	if(u8_index>=MEMC_SPECIAL_SCENE_TABLE_MAX){
		rtd_pr_memc_emerg("[%s][%d] index(%d) exceed max(%d)\n", __FUNCTION__, __LINE__, u8_index, MEMC_SPECIAL_SCENE_TABLE_MAX);
		return false;
	}

	if(u8_index==MEMC_SCENE_NOT_TV006){
		if( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ){
			return true;
		}
	}
	//K24014_v3
	else if(u8_index==MEMC_SCENE_LOGO_PUREPANNING_2_OR_7){
		if(s_pContext->_output_FRC_LgDet.u1_logo_PurePanning_status == 2 || s_pContext->_output_FRC_LgDet.u1_logo_PurePanning_status == 7){
			return true;
		}
	}
	//--#04_Montage_P 5s
	else if(u8_index==MEMC_SCENE_MONTAGE_P_5S){
		if( u11_gmv_mvx>=20 && u11_gmv_mvx<=45 && u10_gmv_mvy >= 0 && u10_gmv_mvy <= -10 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//--#15-HD_H_R_32_i
	else if(u8_index==MEMC_SCENE_HD_H_R_32_I){
		if( u25_me_aDTL_rb > 1290000 && u11_gmv_mvx <=2 && u11_gmv_mvx >= 0 && u10_gmv_mvy>=-1 && u10_gmv_mvy<=1 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//--#19 monosco4K_Hscroll_10pix_50Hz_long-50p-3840x2160
	else if(u8_index==MEMC_SCENE_MONOSCO4K){
		if(u11_gmv_mvx < -8 && u11_gmv_mvx > -12 && u10_gmv_mvy == 0 && u1_still_frame == 0 && u25_me_aDTL_rb > 1200000 && u12_me_GMV_1st_unconf_rb < 5 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//--#23_DK_Zoomin_crop_i 0s
	else if(u8_index==MEMC_SCENE_23_DK_ZOOMIN_CROP_I){
		if( (u11_gmv_mvx >= 2045 || ( u11_gmv_mvx <= 5 && u11_gmv_mvx >= 0 ) ) && u10_gmv_mvy >=0 && u10_gmv_mvy <= 3 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )) {
			return true;
		}
	}
	//--#4_3.QTEK_6.5ppf_00081 0s
	else if(u8_index==MEMC_SCENE_QTEK_6_5PPF_00081){
		if( u11_gmv_mvx >= 23 && u11_gmv_mvx <= 25 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )) {
			return true;
		}
	}
	//--#7_8.sydney5_motion24p 22s
	else if(u8_index==MEMC_SCENE_SYDNEY5_MOTION24P_22S){
		if( u11_gmv_mvx >= -5 && u11_gmv_mvx <= -1 && u10_gmv_mvy >= 5 && u10_gmv_mvy <= 9 &&
			u1_still_frame == 0 && u25_me_aDTL_rb >= 1330000 && u25_me_aDTL_rb <= 1519000 &&
			u12_me_GMV_1st_unconf_rb >= 3 && u12_me_GMV_1st_unconf_rb <= 9 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )) {
			return true;
		}
		else if( u11_gmv_mvx >= 6 && u11_gmv_mvx <= 10 && u10_gmv_mvy >= 3 && u10_gmv_mvy <= 9 &&
			u1_still_frame == 0 && u25_me_aDTL_rb >= 1080000 && u25_me_aDTL_rb <= 1230500 &&
			u12_me_GMV_1st_unconf_rb >= 0 && u12_me_GMV_1st_unconf_rb <= 5 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )) {
			return true;
		}
		else if( u11_gmv_mvx >= 11 && u11_gmv_mvx <= 21 && u10_gmv_mvy >= 0 && u10_gmv_mvy <= 4 &&
			u1_still_frame == 0 && u25_me_aDTL_rb >= 924000 && u25_me_aDTL_rb <= 1029000 &&
			u12_me_GMV_1st_unconf_rb >= 1 && u12_me_GMV_1st_unconf_rb <= 2 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )) {
			return true;
		}
		else if( u11_gmv_mvx >= 4 && u11_gmv_mvx <= 5 && u10_gmv_mvy >= 0 && u10_gmv_mvy <= 1 &&
			u1_still_frame == 0 && u25_me_aDTL_rb >= 892500 && u25_me_aDTL_rb <= 935500 &&
			u12_me_GMV_1st_unconf_rb >= 2 && u12_me_GMV_1st_unconf_rb <= 3 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )) {
			return true;
		}
	}
	//--#7_8.sydney5_motion24p 29
	else if(u8_index==MEMC_SCENE_SYDNEY5_MOTION24P_29S){
		if( u11_gmv_mvx >= 11 && u11_gmv_mvx <= 13 && u10_gmv_mvy >= 2 && u10_gmv_mvy <= 4 &&
			u1_still_frame == 0 && u25_me_aDTL_rb >= 993000 && u25_me_aDTL_rb <= 1011500 &&
			u12_me_GMV_1st_unconf_rb >= 1 && u12_me_GMV_1st_unconf_rb <= 3 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )) {
			return true;
		}
	}
	//--#17_DNM hrnm 720p60 78s
	else if(u8_index==MEMC_SCENE_DNM_HRNM){
		if( u11_gmv_mvx >= -17 && u11_gmv_mvx <= -15 && u10_gmv_mvy >= 2 && u10_gmv_mvy <= 4 &&
			u1_still_frame == 0 && u25_me_aDTL_rb >= 494200 && u25_me_aDTL_rb <= 494500 &&
			u12_me_GMV_1st_unconf_rb >= 11 && u12_me_GMV_1st_unconf_rb <= 13 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//--#7_8.sydney5_motion24p 25s
	else if(u8_index==MEMC_SCENE_SYDNEY5_MOTION24P_25S){
		if( u11_gmv_mvx >= 6 && u11_gmv_mvx <= 10 && u10_gmv_mvy >= 3 && u10_gmv_mvy <= 9 &&
			u1_still_frame == 0 && u25_me_aDTL_rb >= 1080000 && u25_me_aDTL_rb <= 1230500 &&
			u12_me_GMV_1st_unconf_rb >= 0 && u12_me_GMV_1st_unconf_rb <= 5 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )) {
			return true;
		}
		else if( u11_gmv_mvx >= 11 && u11_gmv_mvx <= 21 && u10_gmv_mvy >= 0 && u10_gmv_mvy <= 4 &&
			u1_still_frame == 0 && u25_me_aDTL_rb >= 924000 && u25_me_aDTL_rb <= 1029000 &&
			u12_me_GMV_1st_unconf_rb >= 1 && u12_me_GMV_1st_unconf_rb <= 2 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )) {
			return true;
		}
		else if( u11_gmv_mvx >= 4 && u11_gmv_mvx <= 5 && u10_gmv_mvy >= 0 && u10_gmv_mvy <= 1 &&
			u1_still_frame == 0 && u25_me_aDTL_rb >= 892500 && u25_me_aDTL_rb <= 935500 &&
			u12_me_GMV_1st_unconf_rb >= 2 && u12_me_GMV_1st_unconf_rb <= 3 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )) {
			return true;
		}
	}
	//--#16-H_Move_fast with negative rmvx
	else if(u8_index==MEMC_SCENE_H_MOVE_FAST_NEGATIVE_RMVX){
		for(u8_i = 0; u8_i < 16; u8_i++) {
			if(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i] > 0){
				positive_rmvx_counter++;
			}
		}

		if(positive_rmvx_counter < 4 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//--#16-H_Move_fast with positive rmvx
	else if(u8_index==MEMC_SCENE_H_MOVE_FAST_POSITIVE_RMVX){
		for(u8_i = 0; u8_i < 16; u8_i++) {
			if(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_i] > 0){
				positive_rmvx_counter++;
			}
		}

		if(positive_rmvx_counter >= 4 &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//DK_Zoomin_24P&MI3_Stairs_24P begin
	else if(u8_index==MEMC_SCENE_MI3_STAIRS_24P){
		if(s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true >= 3 && ( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//Tokyo night 02:55
	else if(u8_index==MEMC_SCENE_TOKYO_NIGHT_2M_55S){
		if((_ABS_(u11_gmv_mvx>>1) <=1	&& _ABS_(u10_gmv_mvy>>1) <=1)&& (u25_me_aDTL_rb > 1100000)&&( u1_still_frame == 0)){
			return true;
		}
	}
	//xmen_apocalypse
	else if(u8_index==MEMC_SCENE_XMEN_APOCALYPSE){
		if((u25_me_aDTL_rb > 700000)&&( u1_still_frame == 0)){
			return true;
		}
	}
	//Rassia Fields
	else if(u8_index==MEMC_SCENE_ES_RASSIA_FLAG){
		if(s_pContext->_output_bRMV_rFB_ctrl.u8_ES_Rassia_ID_flag ==1){
			return true;
		}
	}
	//#214
	else if(u8_index==MEMC_SCENE_214){
		if((u11_gmv_mvx>=25)&&(u11_gmv_mvx<=80)){
			return true;
		}
	}
	//#181
	else if(u8_index==MEMC_SCENE_181){
		if((u11_gmv_mvx<=-34 && u11_gmv_mvx>=-44)&&(u10_gmv_mvy<=-55 && u10_gmv_mvy>=-68) && (u12_me_GMV_1st_unconf_rb<=6) && (u25_me_aDTL_rb<=3260000 && u25_me_aDTL_rb>=3200000)){
			return true;
		}
	}
	//#045
	else if(u8_index==MEMC_SCENE_045){
		if(s_pContext->_output_read_comreg.me1_gmv_bgcnt>8000){
			return true;
		}
	}
	//MSPG PATTER 7 for MEMC
	else if(u8_index==MEMC_SCENE_MSPG_PATTER_7){
		if(((u11_gmv_mvx!=0)&&(u10_gmv_mvy!=0))&&(s_pContext->_output_frc_sceneAnalysis.u8_OSDPictureModeChg_prt_apply!=0)){
			return true;
		}
	}//++ K-13536 issue
	else if(u8_index==MEMC_SCENE_K_13536){
		if(RUN_MERLIN8P()){
			return false;
		}//RUN_MERLIN8P & RUN_MERLIN8
		ReadRegister(KME_VBUF_TOP_KME_VBUF_TOP_44_reg,10,19,&u32_RB_Value);
		if(u32_RB_Value<=510){
			return true;
		}
	}
	//++ Q-13835
	else if(u8_index==MEMC_SCENE_Q_13835){
		if((Y_hist_ratio[2]>=10)&&(Y_hist_ratio[3]>=10)&&(( u8_cadence_Id == _CAD_VIDEO) ||(u8_Mixmode_flag==1))&&(s_pContext->_output_me_sceneAnalysis.u1_Q13835_flag==1)){
			return true;
		}
	}
	//#pattern 298 daredevil boy
	else if(u8_index==MEMC_SCENE_PATTERN_298_DAREDEVIL_BOY){
		ReadRegister(HARDWARE_HARDWARE_58_reg, 3, 3, &u32_pattern298_zidoo_player_forbindden_en);
		if(u32_pattern298_zidoo_player_forbindden_en== 1){
			return true;
		}
	}
	//++repeat issue recognization
	else if(u8_index==MEMC_SCENE_BTS_IN30_OR_IN60_CAD22){
		if((s_pContext->_output_me_sceneAnalysis.u1_BTS_DRP_detect_true == 1 )&&((in_fmRate == _PQL_IN_30HZ)||((in_fmRate == _PQL_IN_60HZ)&&( u8_cadence_Id == _CAD_22)))){
			return true;
		}
	}
	//xue_zhong_han_dao_xing window borken
	else if(u8_index==MEMC_SCENE_NOT_TV006_4K1K){
		if(MEMC_Is_4k1k()&&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//casino for MEMC
	else if(u8_index==MEMC_SCENE_0TO23_HUE_HIST_RATIO_0){
		hue_ratio = 0;
		for(u8_i=0;u8_i<24;u8_i++){
			hue_ratio += hue_hist_ratio[u8_i];
		}
		if(hue_ratio==0){
			return true;
		}
	}
	else if(u8_index==MEMC_SCENE_NOT_TV006_STILL_FRAME_0){
		if(u1_still_frame == 0&&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//regression test film32
	else if(u8_index==MEMC_SCENE_FILM_32){
		if(u25_rgnSad_rb12 ==0&&u25_rgnSad_rb23<=250&&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	else if(u8_index==MEMC_SCENE_PANNING_FLAG){
		if(s_pContext->_output_me_sceneAnalysis.u2_panning_flag){
			return true;
		}
	}
	//Art of Flight 01:03:18
	else if(u8_index==MEMC_SCENE_ART_OF_FLIGHT_01_03_18){
		if((u11_gmv_mvx >=40 && u11_gmv_mvx <=50)&&(u10_gmv_mvy >=-61 && u10_gmv_mvy <=-31)&&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//++ YE Test for 01_Black_19YO1 issue
	else if(u8_index==MEMC_SCENE_01_BLACK_19YO1){
		if(u11_gmv_mvx <=0 && u10_gmv_mvy <=2){
			return true;
		}
	}
	//MI3_roof 43s
	else if(u8_index==MEMC_SCENE_MI3_ROOF_43S){
		if((u11_gmv_mvx >=190 && u11_gmv_mvx <=310)&&(u10_gmv_mvy >=-7 && u10_gmv_mvy <=7)&&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//tv002 GLEE - Ride Wit Me #1
	else if(u8_index==MEMC_SCENE_BOT_RATIO_0){
		if(BOT_ratio==0){
			return true;
		}
	}
	//01_PerfumeLand_0915_M_4K_V1_01.mp4 8s  KTASKWBS-23028 KTASKWBS-23147
	else if(u8_index==MEMC_SCENE_K_23028){
		if(in_fmRate == _PQL_IN_60HZ && out_fmRate == _PQL_OUT_120HZ && u8_Mixmode_flag == 0 && u8_cadence_Id == _CAD_VIDEO &&
			s_pContext->_output_read_comreg.u20_me_rDTL_rb[10] > 60000 && s_pContext->_output_read_comreg.u20_me_rDTL_rb[11] > 60000 && s_pContext->_output_read_comreg.u20_me_rDTL_rb[12] > 60000 && s_pContext->_output_read_comreg.u20_me_rDTL_rb[13] > 60000 && 
			s_pContext->_output_read_comreg.u20_me_rDTL_rb[18] > 60000 && s_pContext->_output_read_comreg.u20_me_rDTL_rb[19] > 60000 && s_pContext->_output_read_comreg.u20_me_rDTL_rb[20] > 60000 && s_pContext->_output_read_comreg.u20_me_rDTL_rb[21] > 60000){
			return true;
		}
	}
	//test pattern Soap Opera Effect test 00:19
	else if(u8_index==MEMC_SCENE_SOAP_OPERA_EFFECT){
		if((u25_me_aDTL_rb >=3800000)&&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//Art of Flight 18:27
	else if(u8_index==MEMC_SCENE_ART_OF_FLIGHT_18_27){
		if((u11_gmv_mvx >=-58 && u11_gmv_mvx <=-36)&&(u10_gmv_mvy >=-72 && u10_gmv_mvy <=-33)  &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	else if(u8_index==MEMC_SCENE_GMV_0){
		if(u11_gmv_mvx ==0 && u10_gmv_mvy ==0){
			return true;
		}
	}
	//--#60017 下边界破碎1 7s
	else if(u8_index==MEMC_SCENE_60017_7S){
		if((u25_me_aDTL_rb>2300000&&u25_me_aDTL_rb<23000000)&&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//--#2049
	else if(u8_index==MEMC_SCENE_NOT_TV006_50IN100OUT_60IN120OUT_30IN120OUT){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&&((in_fmRate==_PQL_IN_50HZ && out_fmRate==_PQL_OUT_100HZ)||(in_fmRate==_PQL_IN_60HZ && out_fmRate==_PQL_OUT_120HZ)||(in_fmRate==_PQL_IN_30HZ && out_fmRate==_PQL_OUT_120HZ))){
			return true;
		}
	}
	//--#60028自然世界-鹰之力2 3s
	else if(u8_index==MEMC_SCENE_60028_3S){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& u25_me_aDTL_rb<=1300000 && ( u11_gmv_mvx<-200 || (u11_gmv_mvx>=-1 && u11_gmv_mvx<=1 && u10_gmv_mvy>=-1 && u10_gmv_mvy<=1) )){
			return true;
		}
	}
	//--#4K023transporter_explosion
	else if(u8_index==MEMC_SCENE_4K023_TRANSPORTER_EXPLOSION){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& special_pattern_transporter!=1){
			return true;
		}
	}
	//--#broken subtitle 雪中悍刀行
	else if(u8_index==MEMC_SCENE_XUE_ZHONG_HAN_DAO_XING_BROKEN_SUBTITLE){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&&
		MEMC_Lib_get_Adaptive_Stream_Flag()==1 && s_pContext->_external_data._input_frameRate==_PQL_IN_25HZ && 
		u12_me_GMV_1st_unconf_rb>=6 && u12_me_GMV_1st_unconf_rb<=27 && u1_still_frame == 0){
			return true;
		}
	}
	//--#broken subtitle八百 1:00:15
	else if(u8_index==MEMC_SCENE_BA_BAI_BROKEN_SUBTITLE_1_00_15){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&&
		MEMC_Lib_get_Adaptive_Stream_Flag()==1 && s_pContext->_external_data._input_frameRate==_PQL_IN_24HZ &&
		u10_gmv_mvy>=-15 && u10_gmv_mvy<=3 && u1_still_frame==0 && u25_me_aDTL_rb>=175000 && u25_me_aDTL_rb<=260000 && u12_me_GMV_1st_unconf_rb<=20){
			return true;
		}
	}
	//--#broken subtitle 霹靂嬌娃_伊斯坦堡
	else if(u8_index==MEMC_SCENE_PI_LI_JIAO_WAY_I_SI_TAN_BAO_BROKEN_SUBTITLE){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&&
		MEMC_Lib_get_Adaptive_Stream_Flag()==1 && s_pContext->_external_data._input_frameRate==_PQL_IN_24HZ &&
		u10_gmv_mvy>=-4 && u10_gmv_mvy<=3 && u1_still_frame==0 && u25_me_aDTL_rb>=440000 && u25_me_aDTL_rb<=1040000 && u12_me_GMV_1st_unconf_rb<=30){
			return true;
		}
	}
	//--#broken subtitle 霹靂嬌娃 1:01:00
	else if(u8_index==MEMC_SCENE_PI_LI_JIAO_WAY_1_01_00){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&&
		MEMC_Lib_get_Adaptive_Stream_Flag()==1 && s_pContext->_external_data._input_frameRate==_PQL_IN_24HZ &&
		u11_gmv_mvx>=-1 && u11_gmv_mvx<=16 && u1_still_frame==0 && u25_me_aDTL_rb>=200000 && u25_me_aDTL_rb<=550000 && u12_me_GMV_1st_unconf_rb<=32){
			return true;
		}
	}
	//--#broken subtitle八百 1:00:24
	else if(u8_index==MEMC_SCENE_BA_BAI_BROKEN_SUBTITLE_1_00_24){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& MEMC_Lib_get_Adaptive_Stream_Flag()==1 && s_pContext->_external_data._input_frameRate==_PQL_IN_24HZ){
			return true;
		}
	}
	//--#土耳其 16s
	else if(u8_index==MEMC_SCENE_TURKEY_16S){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && u11_gmv_mvx>=-2&&u11_gmv_mvx<=0&&u10_gmv_mvy==0&&u1_still_frame==0&&u12_me_GMV_1st_unconf_rb==0&& u25_me_aDTL_rb>=1700000){
			return true;
		}
	}
	//--#土耳其 23s
	else if(u8_index==MEMC_SCENE_TURKEY_23S){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && u11_gmv_mvx>=0&&u11_gmv_mvx<=1&&u10_gmv_mvy==0&&u1_still_frame==0&&u12_me_GMV_1st_unconf_rb==0&&u25_me_aDTL_rb>=1800000){
			return true;
		}
	}
	//--#土耳其 logo
	else if(u8_index==MEMC_SCENE_TURKEY_LOGO){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && ( u11_gmv_mvx==0 && u10_gmv_mvy==0 && u1_still_frame==0 && u12_me_GMV_1st_unconf_rb==0 && u25_me_aDTL_rb>=750000 && u25_me_aDTL_rb<=850000)){
			return true;
		}
	}
	//--#雪中悍刀行 3:32
	else if(u8_index==MEMC_SCENE_XUE_ZHONG_HAN_DAO_XING_03_32){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && ( u11_gmv_mvx>=-7 && u11_gmv_mvx<=11 &&u10_gmv_mvy>=-5 &&u10_gmv_mvy<=3 && u1_still_frame==0 && u12_me_GMV_1st_unconf_rb<=18 && u25_me_aDTL_rb>=900000 && u25_me_aDTL_rb<=1200000)){
			return true;
		}
	}
	//--#25005huanlerenjian1 50s
	else if(u8_index==MEMC_SCENE_25005_HUANLERENJIAN1_50S){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && (u11_gmv_mvx>=-48 && u11_gmv_mvx<=-45) && (u10_gmv_mvy>=-1 && u10_gmv_mvy<=1) && s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb<2){
			return true;
		}
	}
	//--#聖托里尼 5:09s
	else if(u8_index==MEMC_SCENE_SANTORINI_05_09){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& u11_gmv_mvx==0 && u10_gmv_mvy>=0 && u10_gmv_mvy<=1 && u1_still_frame==0 && u12_me_GMV_1st_unconf_rb<=4){
			return true;
		}
	}
	//--#雪中捍刀行 7:55
	else if(u8_index==MEMC_SCENE_XUE_ZHONG_HAN_DAO_XING_07_55){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& u11_gmv_mvx>=-40&&u11_gmv_mvx<=20&&u10_gmv_mvy>=-2&&u10_gmv_mvy<=5&&u1_still_frame==0){
			return true;
		}
	}
	//--#24004 19s
	else if(u8_index==MEMC_SCENE_24004_19S){
		if( ( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& (u11_gmv_mvx>=-9&&u11_gmv_mvx<=8) && (u10_gmv_mvy>=0&&u10_gmv_mvy<=3) && s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb < 6){
			return true;
		}
	}
	//--#30006 45s
	else if(u8_index==MEMC_SCENE_30006_45S){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& (u25_me_aDTL_rb>1100000&&u25_me_aDTL_rb<2500000)){
			return true;
		}
	}
	//--#瑞士 2:08
	else if(u8_index==MEMC_SCENE_SWISS_02_08){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && u11_gmv_mvx>=0 && u11_gmv_mvx<=4 && u10_gmv_mvy==0 && u1_still_frame==0 && u12_me_GMV_1st_unconf_rb<=1 && u25_me_aDTL_rb>=1510000 && u25_me_aDTL_rb<=1620000){
			return true;
		}
	}
	//--#30016-hor-dispatch2
	else if(u8_index==MEMC_SCENE_30016_HOR_DISPATCH2){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && (_ABS_(u11_gmv_mvx)+_ABS_(u10_gmv_mvy))<=3 && u1_still_frame==0 && u25_me_aDTL_rb>=370000 && u25_me_aDTL_rb<=560000){
			return true;
		}
	}
	//--#24002 0s
	else if(u8_index==MEMC_SCENE_24002_0S){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && in_fmRate==_PQL_IN_50HZ && out_fmRate==_PQL_OUT_100HZ){
			return true;
		}
	}
	//--#24003 11s
	else if(u8_index==MEMC_SCENE_24003_11S){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && ( u12_me_GMV_1st_unconf_rb<=5 && u25_me_aDTL_rb>=900000 && u25_me_aDTL_rb<=2580000 && u1_still_frame==0 )){
			return true;
		}
	}
	//C001 13s
	else if(u8_index==MEMC_SCENE_C001_13S){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && in_fmRate==_PQL_IN_30HZ){
			return true;
		}
	}
	//--#30003 40s  
	else if(u8_index==MEMC_SCENE_30003_40S){
		ReadRegister(HARDWARE_HARDWARE_44_reg, 0, 0, &readval440); 
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&&(!readval440)){
			return true;
		}
	}
	//--#30018 6s  //--#24003 11s
	else if(u8_index==MEMC_SCENE_30018_6S){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) &&( (u12_me_GMV_1st_unconf_rb<=5 && u25_me_aDTL_rb>=800000 && u25_me_aDTL_rb<=1100000 && u1_still_frame==0)||(u11_gmv_mvx>=-22 && u11_gmv_mvx<=-2 && u10_gmv_mvy>=40 && u10_gmv_mvy<=76) )){
			return true;
		}
	}
	//--#spiderman 17:03
	else if(u8_index==MEMC_SCENE_SPIDERMAN_17_03){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && u11_gmv_mvx<-70&&u11_gmv_mvx>-215&&u10_gmv_mvy<3&&u10_gmv_mvy>-10&&u12_me_GMV_1st_unconf_rb<55&&u12_me_GMV_1st_unconf_rb>10&&u1_still_frame==0 && special_pattern_transporter!=1){
			return true;
		}
	}
	//list130 20s
	else if(u8_index==MEMC_SCENE_LIST_130_20S){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& u11_gmv_mvx>=-4 && u11_gmv_mvx<=0 &&u10_gmv_mvy>=-1 &&u10_gmv_mvy<=1 &&u1_still_frame==0  && u25_me_aDTL_rb>=1350000 && u25_me_aDTL_rb<=1400000 && u12_me_GMV_1st_unconf_rb<=1){
			return true;
		}
	}
	//list130 01:24s
	else if(u8_index==MEMC_SCENE_LIST_130_01_24){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& u11_gmv_mvx>=-5 && u11_gmv_mvx<=1 &&u10_gmv_mvy>=0 &&u10_gmv_mvy<=6 &&u1_still_frame==0  && u25_me_aDTL_rb>=1600000 && u25_me_aDTL_rb<=1960000 && u12_me_GMV_1st_unconf_rb<=1){
			return true;
		}
	}
	//list130 03:17s
	else if(u8_index==MEMC_SCENE_LIST_130_03_17){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& u11_gmv_mvx>=-49 && u11_gmv_mvx<=-13 &&u10_gmv_mvy>=-11 &&u10_gmv_mvy<=-4 &&u1_still_frame==0  && u25_me_aDTL_rb>=1600000 && u25_me_aDTL_rb<=2880000 && u12_me_GMV_1st_unconf_rb==0){
			return true;
		}
	}
	//list138 10s
	else if(u8_index==MEMC_SCENE_LIST_138_10S){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&&u11_gmv_mvx>=-113 && u11_gmv_mvx<=70 &&u10_gmv_mvy>=-63 &&u10_gmv_mvy<=85 &&u1_still_frame==0  && u25_me_aDTL_rb>=630000 && u25_me_aDTL_rb<=1400000 && u12_me_GMV_1st_unconf_rb>=0 && u12_me_GMV_1st_unconf_rb<=26 ){
			return true;
		}
	}
	//list139 40s
	else if(u8_index==MEMC_SCENE_LIST_139_40S){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& u11_gmv_mvx>=-39 && u11_gmv_mvx<=-5 &&u10_gmv_mvy>=-39 &&u10_gmv_mvy<=30 &&u1_still_frame==0  && u25_me_aDTL_rb>=1800000 && u25_me_aDTL_rb<=2700000 && u12_me_GMV_1st_unconf_rb>=0 && u12_me_GMV_1st_unconf_rb<=5){
			return true;
		}
	}
	// 开端-03集
	else if(u8_index==MEMC_SCENE_KAI_DUAN_EPISODE_3){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&&u11_gmv_mvx>=-26 && u11_gmv_mvx<=6 && u10_gmv_mvy>=-2 && u10_gmv_mvy<=2 && u1_still_frame==0 && u25_me_aDTL_rb>=800000 && u25_me_aDTL_rb<=1400000 && u12_me_GMV_1st_unconf_rb>=0 && u12_me_GMV_1st_unconf_rb<=13 ){
			return true;
		}
	}
	//sport
	else if(u8_index==MEMC_SCENE_SPORT_SCENE){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && s_pContext->_output_me_sceneAnalysis.u1_SportScene == TRUE){
			return true;
		}
	}
	// VTS_02_1
	else if(u8_index==MEMC_SCENE_VTS_02_1){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& u1_still_frame==0 && u25_me_aDTL_rb>80000 && in_fmRate==_PQL_IN_30HZ && out_fmRate==_PQL_OUT_120HZ){
			return true;
		}
	}
	// no g_new_sport_mode
	else if(u8_index==MEMC_SCENE_NO_G_NEW_SPORT_MODE){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && g_new_sport_mode==false){
			return true;
		}
	}
	//白色景物
	else if(u8_index==MEMC_SCENE_NOT_TV006_GMV_0){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && u11_gmv_mvx==0 && u10_gmv_mvy==0){
			return true;
		}
	}
	//下馬卡頓 播放片源，画面下方字幕抖动
	else if(u8_index==MEMC_SCENE_XIA_MA_KA_DUN){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && (u25_me_aDTL_rb>1000000) && (2000000>u25_me_aDTL_rb) && MEMC_Is_4k1k()){
			return true;
		}
	}
	//djntest miyezhuan 2nd
	else if(u8_index==MEMC_SCENE_MIYEZHUAN_2ND){
		rmvx7_for_miyuezhuan = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[7];
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& rmvx7_for_miyuezhuan>-40 &&rmvx7_for_miyuezhuan<0){
			return true;
		}
	}
	//miyuezhuan 3rd
	else if(u8_index==MEMC_SCENE_MIYEZHUAN_3RD){
		rmvx7_for_miyuezhuan = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[7];
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )&& rmvx7_for_miyuezhuan<-120 ){
			return true;
		}
	}
	//狙击手_1h26min_1k
	else if(u8_index==MEMC_SCENE_JU_JI_SHOU_1_26_00){
		for(dtl_index=8;dtl_index<24;dtl_index++)
		{
			if(s_pContext->_output_read_comreg.u20_me_rDTL_rb[dtl_index]>=100000)
				dtl_cnt=dtl_cnt+2;
			else if(s_pContext->_output_read_comreg.u20_me_rDTL_rb[dtl_index]>=50000)
				dtl_cnt++;
		}
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && dtl_cnt>=16){
			return true;
		}
	}
	else if(u8_index==MEMC_SCENE_NOT_TV006_NOT_4K1K){
		if(MEMC_Is_4k1k()==FALSE &&( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	//6009 to 6010
	else if(u8_index==MEMC_SCENE_NOT_TV006_GMV_0_UNCONF_0_STILL_FRAME){
		if(( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 ) && u11_gmv_mvx==0 && u10_gmv_mvy==0 && u1_still_frame==1 && u12_me_GMV_1st_unconf_rb ==0){
			return true;
		}
	}
	//test pattern vertrez24p/Soap Opera Effect test 02:00/ 02:40 /03:40/03:30
	else if(u8_index==MEMC_SCENE_NOT_TV006_7TO12_HUE_HIST_RATIO_0){
		if(hue_hist_ratio[7]==0 && hue_hist_ratio[8]==0  && hue_hist_ratio[9]==0  && hue_hist_ratio[10]==0  && hue_hist_ratio[11]==0  && hue_hist_ratio[12]==0 && ( vpq_project_id != 0x00060000 || vpqex_project_id != 0x00060000 )){
			return true;
		}
	}
	else if(u8_index==MEMC_SCENE_60IN){
		if(in_fmRate == _PQL_IN_60HZ){
			return true;
		}
	}
	//07.07.02.CG_Flower_04-AVC1080P24.m2ts
	else if(u8_index==MEMC_SCENE_CG_FLOWER){
		if(((1500000>=u25_me_aDTL_rb &&u25_me_aDTL_rb>=1400000 && 1600000>=u26_APLp && u26_APLp>=1400000) && 1>=u11_gmv_mvx && u11_gmv_mvx >=-10 && 2>=u10_gmv_mvy && u10_gmv_mvy>=-2) || (((900000>=u25_me_aDTL_rb &&u25_me_aDTL_rb>=750000 && 1700000>=u26_APLp && u26_APLp>=1400000)||(1200000>=u25_me_aDTL_rb &&u25_me_aDTL_rb>=900000 && 1500000>=u26_APLp && u26_APLp>=1300000)) && -8>=u11_gmv_mvx && u11_gmv_mvx >=-20 && u10_gmv_mvy==0)){
			return true;
		}
	}
	//07.08.10.logo_zebra_1080P24.mkv
	else if(u8_index==MEMC_SCENE_LOGO_ZEBRA){
		if(950>=score && score >=800 && 4500000>=u26_APLp && u26_APLp>=4000000 && s_pContext->_output_filmDetectctrl.u8_cur_cadence_Id[_FILM_ALL]==_CAD_32){
			return true;
		}
	}
	//07.08.05.HQV-Transformer-1080P24.mp4
	else if(u8_index==MEMC_SCENE_HQV_TRANSFORMER){
		ReadRegister(SOFTWARE3_SOFTWARE3_46_reg, 16, 23, &vtest);//0xB809D9B8
		if(_ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb)>=(30+vtest)){
			return true;
		}
	}
	//Transformer.3.2011.1080p end
	else if(u8_index==MEMC_SCENE_ABS_GMVX_SMALLER_30){
		if(30>_ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb)){
			return true;
		}
	}
	
	return false;
}

void Identify_ctrl_update_specal_scene_status(void)
{
	unsigned char u8_i = 0;

	memset(&DynamicOptimizeMEMC_SpecialScene, 0, sizeof(DynamicOptimizeMEMC_SpecialScene));
	DynamicOptimizeMEMC_SpecialScene[0] = true;

	for(u8_i=1; u8_i<MEMC_SPECIAL_SCENE_TABLE_MAX; u8_i++){
		DynamicOptimizeMEMC_SpecialScene[u8_i] = Identify_ctrl_get_specal_scene_status(u8_i);
	}
}


