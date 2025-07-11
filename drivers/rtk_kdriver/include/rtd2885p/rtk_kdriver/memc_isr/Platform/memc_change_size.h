#ifndef _MEMC_CHANGE_SIZE_HEADER_INCLUDE_
#define _MEMC_CHANGE_SIZE_HEADER_INCLUDE_

#include "memc_isr/include/memc_lib.h"
#define ENABLE_4K1K (1)

#if 0
/* TYPE */
#endif
typedef struct MEMC_SIZE_PARAM
{
	unsigned int ImageHTotal;
	unsigned int ImageHSize;
	unsigned int ImageVSize;
	unsigned int Mode;
	unsigned int FHD_Mode;
	unsigned int NO_MERLIN4_MEMC_SIM; // ??
	unsigned int debug; // ??
	unsigned int RealTiming; //?
	unsigned int OutputNumOf4Pxl;
	unsigned int SceneOn;
	unsigned int SearchRangeLimit;
	unsigned int Merlin5Timing; // ?
	unsigned int BringUpLogo; // ?
	unsigned int InputFrameRate;
	unsigned int OutputFrameRate;
	unsigned int ME12_Merge;
	unsigned int FullDMASetting;
	unsigned int MEBaseAddr;
	// ippre
	unsigned int IPPRE_DummyLine;
	// ipme
	unsigned int IPME_DnRatio;
	// logo
	unsigned int LOGO_Enable;
	unsigned int LOGO_Switch;
	// me1
	unsigned int ME1_DoubleScan;
	unsigned int ME1_Meander;
	// me2
	unsigned int ME2_2x2;
	unsigned int ME2_DoubleSacn;
	unsigned int ME2_RefreshScan;
	// lbmc
	unsigned int LBMC_FullRange;
	unsigned int LBMC_NLineOption;
	unsigned int LBMC_PwrOpt;
	// mc
	unsigned int MC_LineNum;
	// PQC
	unsigned int PQC_Mode;
	unsigned int PQC_MEMode;
	// PC Mode
	unsigned int PC_Mode;
	unsigned int PC_RepeatMode;
	unsigned int PC_SelPZ;
	// 8 buffer
	unsigned int MC_8_buffer;

	int kme_dm_bg_1bit;
	int kme_me1_bg_mask_en;  // bg_mask dma enable

	// below fixed value
	int normal_hf_prefetch_unit_x2;
	int normal_hf_sr_unit_x2;

} MEMC_SIZE_PARAM;

typedef struct IMAGE_SIZE
{
	unsigned int ImageHSize;
	unsigned int ImageVSize;
}IMAGE_SIZE;

/*
enum
{
	MEMC_SIZE_2K = 0,	//FHD		,1920*1080
	MEMC_SIZE_3K,	 	//QHD		,2560*1440
	MEMC_SIZE_4K,	 	//UHD		,3840*2160
	MEMC_SIZE_8K,		//8KUHD		,7680*4320
	MEMC_SIZE_NUM
};
*/

enum
{
	MEMC_SIZE_2K = 0,	//FHD		,1920*1080
	MEMC_SIZE_3K,	 	//QHD		,2550*1440
	MEMC_SIZE_4K,	 	//UHD		,3840*2160
#if ENABLE_4K1K
	MEMC_SIZE_4K1K,		//DFHD		,3840*1080
#endif
	MEMC_SIZE_8K,		//8KUHD		,7680*4320
	MEMC_SIZE_8K_120,	//8KUHD 120	,7680*4320
	MEMC_SIZE_NUM
};


enum
{
	MEMC_LoadSizeParam_Defalut = 0,
	MEMC_LoadSizeParam_ByScaler,
	MEMC_LoadSizeParam_NUM
};


#if 0
/* FUNCTIONS */
#endif
void MEMC_RTICE_Trigger(void);
void MEMC_RTICE_SetParam(MEMC_SIZE_PARAM* ptr);
void MEMC_RTICE_GetParam(MEMC_SIZE_PARAM* ptr);
void MEMC_ChangeSize_init(const unsigned char MEMC_SZ_SEL);
void MEMC_ChangeSize_init_AfterQS(const unsigned char MEMC_SZ_SEL);
void MEMC_ChangeSize_CRTC_to_scaler(void);



#if 1//#if RTK_MEMC_Performance_tunging_from_tv001 //k23
void MEMC_ChangeSize_main(const unsigned char MEMC_SZ_SEL, unsigned int ME_base_addr, MEMC_INOUT_MODE emom, MEMC_OUTPUT_FRAMERATE Out_FrameRate, unsigned int MEMC_LoadSizeParam_Type);
#else
void MEMC_ChangeSize_main(const unsigned char MEMC_SZ_SEL, unsigned int ME_base_addr, MEMC_INOUT_MODE emom, MEMC_OUTPUT_FRAMERATE Out_FrameRate);
#endif




IMAGE_SIZE MEMC_ChangeSize_GetImageSize(void);										


#endif
