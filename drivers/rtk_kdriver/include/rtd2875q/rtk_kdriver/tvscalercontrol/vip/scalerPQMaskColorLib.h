#ifndef _SCALER_PQMASKCOLORLIB_H_
#define _SCALER_PQMASKCOLORLIB_H_
/*******************************************************************************
 * Header files
*******************************************************************************/
#include <tvscalercontrol/vip/scalerPQMaskCtrl.h>
#include <tvscalercontrol/vip/peaking.h>
#include <tvscalercontrol/vip/icm.h>
#include <rtk_kadp_se.h>
#include <scaler/vipCommon.h>

/*******************************************************************************
 * Definitions
*******************************************************************************/
#define PQMASK_HISTO_BIN_SFT	(5)
#define PQMASK_HISTO_BIN		(1<<PQMASK_HISTO_BIN_SFT)

/*******************************************************************************
 * Structure
*******************************************************************************/
enum {
	/* 0 */ PQMASK_COLOR_PALETTE_DEPTH = 0,
	/* 1 */ PQMASK_COLOR_PALETTE_SEMANTIC,
	/* 2 */ PQMASK_COLOR_PALETTE_DEFOCUS,
	/* 3 */ PQMASK_COLOR_PALETTE_MAX,
};

enum {
	/* 0 */ PQMASK_COLORSTRUCT_MGR = 0,
	/* 1 */ PQMASK_COLORSTRUCT_SEMATNIIC_HISTO,
	/* 2 */ PQMASK_COLORSTRUCT_DEPTH_HISTO,
	/* 3 */ PQMASK_COLORSTRUCT_PALETTE,
	/* 4 */ PQMASK_COLORSTRUCT_NUM,
};

enum {
	/* 0 */ PQMASK_IMGCH_Y = 0,
	/* 1 */ PQMASK_IMGCH_U,
	/* 2 */ PQMASK_IMGCH_V,
	/* 3 */ PQMASK_IMGCH_NUM,
};

enum {
	/* 0 */ PQMASK_DEPDIST_FAR = 0,
	/* 1 */ PQMASK_DEPDIST_MID,
	/* 2 */ PQMASK_DEPDIST_NEAR,
	/* 3 */ PQMASK_DEPDIST_NUM,
};

typedef struct PQMASK_COLOR_NR {
	unsigned int bNrEnable;
	unsigned int bIDecontEnable;
	unsigned int bIBlockDecontEnable;
} PQMASK_COLOR_NR_T;

typedef struct PQMASK_COLOR_ICM_SAT_CTRL {
	unsigned char bEnable;
	unsigned char InputSel;
	unsigned char GlbBlendRatio;
	// gain by sat
	unsigned char bGainBySatEnable;
	unsigned char GainBySatMode;
	unsigned char GainBySatCurve[ICM_PQMASK_SAT_GAIN_CURVE_LEN];
	// gain by hue
	unsigned char bGainByHueEnable;
	unsigned char GainByHueCurve[ICM_PQMASK_HUE_GAIN_CURVE_LEN];
} PQMASK_COLOR_ICM_SAT_CTRL_T;

typedef struct PQMASK_COLOR_ICM_HUE_CTRL {
	unsigned char bEnable;
	unsigned char InputSel;
	unsigned int TargetHue;
	// gain by hue
	unsigned char bGainByHueEnable;
	unsigned char GainByHueCurve[ICM_PQMASK_HUE_GAIN_CURVE_LEN];
} PQMASK_COLOR_ICM_HUE_CTRL_T;

typedef struct PQMASK_COLOR_ICM_INT_CTRL {
	unsigned char bEnable;
	unsigned char InputSel;
} PQMASK_COLOR_ICM_INT_CTRL_T ;

typedef struct PQMASK_COLOR_ICM {
	PQMASK_COLOR_ICM_SAT_CTRL_T ObjectSat[2];
	PQMASK_COLOR_ICM_HUE_CTRL_T ObjectHue[2];
	PQMASK_COLOR_ICM_INT_CTRL_T Intensity;
	unsigned int SatGain;
	unsigned int SatOffset;
	unsigned int IntGain;
	unsigned int IntOffset;
} PQMASK_COLOR_ICM_T;

typedef struct PQMASK_COLOR_LC {
	unsigned int bToneMappingEnable;
	unsigned int ToneMappingOffset;
	unsigned int ToneMappingGain;
	unsigned int bSharpEnable;
	unsigned int SharpOffset;
	unsigned int SharpGain;
} PQMASK_COLOR_LC_T;

#if 0 //SRNN_TBD
typedef struct PQMASK_COLOR_SRNN_IN {
	unsigned int reserved1;
	unsigned int reserved2;
	unsigned int reserved3;
	unsigned int reserved4;
} PQMASK_COLOR_SRNN_IN_T;

typedef struct PQMASK_COLOR_SRNN_OUT {
	unsigned int reserved1;
	unsigned int reserved2;
	unsigned int reserved3;
	unsigned int reserved4;
} PQMASK_COLOR_SRNN_OUT_T; 
#endif

typedef struct PQMASK_COLOR_TBL {
	PQMASK_COLOR_NR_T Nr;
	PQMASK_COLOR_ICM_T ICM;
	DRV_LABEL_PK Sharpness;
	PQMASK_COLOR_LC_T LocalContrast;
	//PQMASK_COLOR_SRNN_IN_T SRNN_IN; SRNN_TBD
	//PQMASK_COLOR_SRNN_OUT_T SRNN_OUT;
} PQMASK_COLOR_TBL_T;

typedef struct COLOR_ARGB {
	unsigned char A;
	unsigned char R;
	unsigned char G;
	unsigned char B;
} COLOR_ARGB_T;

typedef struct PQMASK_DEPTH_CURVE_LOCK {
	unsigned char bIsInit;
	spinlock_t CurveLock;
} PQMASK_DEPTH_CURVE_LOCK_T;

typedef struct PQMASK_HIST_INFO {
	unsigned int enable;
	unsigned int width;
	unsigned int height;
	unsigned int hJmp;
	unsigned int vJmp;
} PQMASK_HIST_INFO_T;

typedef struct PQMASK_HIST_DATA {
	unsigned int TotalPts;
	unsigned int Histogram[PQMASK_HISTO_BIN];
	unsigned int TotalPercent; // thousandth total
	unsigned int NorHist[PQMASK_HISTO_BIN]; // thousandth histogram
	unsigned int Apl;
} PQMASK_HIST_DATA_T;

typedef struct PQMASK_HIST {
	PQMASK_HIST_INFO_T Info;
	PQMASK_HIST_DATA_T Data;
} PQMASK_HIST_T;

typedef struct PQMASK_COLOR_STATUS {
	// scene change
	unsigned char bSceneChange;
	int SCRstFrm;
	unsigned int LocalContrastRstWeight;
	//
	int ForceCurFrm;
	// reload palette
	unsigned char bReloadDepPalette;
	unsigned char bReloadSemPalette;
	unsigned char bReloadDefocusPalette;
	// palette control by Genre
	unsigned char AiGenre;
	unsigned char bGenerCGStatusChange;
	int Genre_CG_Frm;
	unsigned char bReloadDepthPalette_AiGenre_CG;
	unsigned char bReloadSemanticPalette_AiGenre_CG;
	unsigned char bReloadDefocusPalette_AiGenre_CG;
	unsigned int SrnnInRstWeight;
	unsigned int SrnnOutRstWeight;
} PQMASK_COLOR_STATUS_T;

typedef struct PQMASK_COLOR_PARAM {
	// depth temporal filter
	unsigned char DepthFusiontRatio_Pre; // effect on depth pre information data
	unsigned char DepthFusiontRatio_Cur; // effect on depth cur information data
	// informaiton fusion
	unsigned char FusionRatio_Dep; // effect on depth
	unsigned char FusionRatio_Sem; // effect on semantic
	unsigned char FusionRatio_Defocus; // effect on defocus
	// temporal filter control
	unsigned char FusionRatio_Pre; // effect on pre
	unsigned char FusionRatio_Cur; // effect on cur
	// spatial filter control
	unsigned char SpatialFltCycle;
	unsigned int SEHorCoef[4]; // for SE
	unsigned int SEVerCoef[4];
	unsigned char HorOddType;
	unsigned char VerOddType;
	// scene change
	unsigned char bSceneChangeEnable;
	unsigned int SceneChangeRstFrm;
	unsigned char ScFusionRatio_toRst_Pre;
	unsigned char ScFusionRatio_toRst_Cur;
	unsigned char ScFusionRatio_toTarget_Pre;
	unsigned char ScFusionRatio_toTarget_Cur;
	// palette control by Genre
	unsigned char bGenre_CG_Enable;
	unsigned int Genre_CG_toTargetFrm;
	unsigned char Genre_CG_FunctionRatio_Pre;
	unsigned char Genre_CG_FunctionRatio_Cur;
	// rounding selection
	unsigned char bRoundActionManualSet;
	unsigned char RoundAction_TNR;
	unsigned char RoundAction_DepthTNR;
	unsigned char RoundAction_DepthSemFusion;
} PQMASK_COLOR_PARAM_T;

typedef struct PQMASK_COLOR_MGR {
	// HW module setting
	PQMASK_COLOR_TBL_T ModuleSetting;
	// Weighting table
	unsigned char SemanticWeightTbl[PQMASK_MODULE_NUM][PQMASK_LABEL_NUM];
	unsigned char DepthWeightTbl[PQMASK_MODULE_NUM][PQMASK_MAPCURVE_LEN];
	unsigned char DefocusWeightTbl[PQMASK_MODULE_NUM][PQMASK_MAPCURVE_LEN];
	unsigned char DefocusEnableTbl[PQMASK_MODULE_NUM];
	// Histogram
	PQMASK_HIST_T DepthHist;
	PQMASK_HIST_T SemanticHist;
	//
	PQMASK_HIST_T ImgBySemanticHist[PQMASK_LABEL_NUM][PQMASK_IMGCH_NUM];
	PQMASK_HIST_INFO_T ImgByDepthHistInfo;
	unsigned char ThlLow; // far <= thl < mid
	unsigned char ThlHigh; // mid <= thl < near
	PQMASK_HIST_T ImgByDepthHist[PQMASK_DEPDIST_NUM][PQMASK_IMGCH_NUM];
	//
	PQMASK_COLOR_STATUS_T Status;
	PQMASK_COLOR_PARAM_T Param;
} PQMASK_COLOR_MGR_T;

/*******************************************************************************
 * Parameters
*******************************************************************************/

#if 0 // ml9 hw not support
/*******************************************************************************
 * functions
*******************************************************************************/
void scalerPQMaskColor_init(void);
void scalerPQMaskColor_exit(void);
void scalerPQMaskColor_disable(void);
void *scalerPQMaskColor_GetStruct(unsigned int InfoIndex);
void *scalerPQMaskColor_GetLockTable(void);

void scalerPQMaskColor_SemanticHistogram(void *pInput);
void scalerPQMaskColor_DepthHistogram(void *pInput);
void scalerPQMaskColor_Img2DepthHistogram(void *pImgInput, void *pInfoInput);
void scalerPQMaskColor_Img2SemanticHistogram(void);
void scalerPQMaskColor_Label2Clut(unsigned int group, KGAL_PALETTE_INFO_T *pPalette);
void scalerPQMaskColor_Depth2Clut(unsigned int group, KGAL_PALETTE_INFO_T *pPalette);
void scalerPQMaskColor_Defocus2Clut(unsigned int group, KGAL_PALETTE_INFO_T *pPalette);
void scalerPQMaskColor_SceneChange(void);
void scalerPQMaskColor_GetGenre(void);
void scalerPQMaskColor_ForceOutputCur(unsigned char Frames);
void scalerPQMaskColor_ConstructPalette(void);
void scalerPQMaskColor_SetReloadPalette(unsigned char PaletteSel);
void scalerPQMaskColor_SetSemanticWeight_SRNN(unsigned char * Buf); // for android, table from srnn.bin

// 
void scalerPQMaskColor_UpdateLabelPK(void);

//
int scalerPQMaskColor_LockTable(unsigned char TableSel);
int scalerPQMaskColor_UnLockTable(unsigned char TableSel);

// RTICE commands
void scalerPQMaskColor_GetSemanticWeight(unsigned char *Buf);
void scalerPQMaskColor_SetSemanticWeight(unsigned char *Buf);
void scalerPQMaskColor_GetDepthWeight(unsigned char *Buf);
void scalerPQMaskColor_SetDepthWeight(unsigned char *Buf);
void scalerPQMaskColor_GetDefocusWeight(unsigned char *Buf);
void scalerPQMaskColor_SetDefocusWeight(unsigned char *Buf);
void scalerPQMaskColor_GetDefocusWeight_SRNN(unsigned char *Buf, unsigned char Module);
void scalerPQMaskColor_SetDefocusWeight_SRNN(unsigned char *Buf);
void scalerPQMaskColor_GetDefocusEnableTbl(unsigned char *Buf);
void scalerPQMaskColor_SetDefocusEnableTbl(unsigned char *Buf);
void scalerPQMaskColor_GetDefocusEnableTbl_SRNN(unsigned char *Buf);
void scalerPQMaskColor_SetDefocusEnableTbl_SRNN(unsigned char *Buf);

// V4L2 commands
int scalerPQMask_V4L2_SetDepthWeight(unsigned char ModuleSel, unsigned char *Buf);
unsigned int scalerPQMask_V4L2_GetDepthAPL(void);
#endif
#endif
