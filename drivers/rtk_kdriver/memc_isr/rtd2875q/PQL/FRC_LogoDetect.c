#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/FRC_LogoDetect.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_isr/PQL/MEMC_ParamTable.h"
#include "memc_isr/PQL/CalibMEMCInfo.h"
#include "memc_isr/PQL/Read_ComReg.h"
#include "memc_reg_def.h"


extern _MEMC_PARAM_TABLE MEMC_PARAM_TABLE;
extern _Param_Dehalo_Logo_Region Dehalo_Logo_Region_Suggest_Val_With_Res;
///////////////////////////////////////////////////////////////////////////////////////////
/*
PQL Improvement:
1. PQL run at PC use _WIN32. (need change to more clear define)
2. Param must be init in order, gen PQL_Glb_Param.c (it's not convenient, can same as output?)


*/
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
extern unsigned char g_48_flag;
extern unsigned char g_190_flag;
extern unsigned char g_244_flag;

VOID FRC_LogoDet_Init(_OUTPUT_FRC_LGDet *pOutput)
{
// HS merlin7
	unsigned int u32_RB_val;
// HS merlin7
	int i;
	pOutput->s16_blklogo_glbclr_cnt = 0;

	for (i=0; i<LOGO_RG_32; i++)
	{
		*(pOutput->s16_blklogo_rgclr_cnt + i) = 0;
	}

	pOutput->u1_rg_dh_clr_rg0 = 0;
	pOutput->u1_rg_dh_clr_rg1 = 0;
	pOutput->u1_rg_dh_clr_rg2 = 0;
	pOutput->u1_rg_dh_clr_rg3 = 0;

	pOutput->u8_rg_dh_clr_lgclr_hold_frm = 0;
	pOutput->u8_rg_dh_clr_static_hold_frm = 0;

	pOutput->u32_logo_hsty_clr_idx = 0;
	pOutput->u32_logo_hsty_clr_lgclr_idx = 0;
	pOutput->u1_logo_cnt_glb_clr_status = 0;
	pOutput->u1_logo_sc_glb_clr_status  = 0;

	pOutput->u1_logo_static_status = 0;

	pOutput->u31_logo_sad_pre = 0;
	pOutput->u1_logo_sc_status = 0;
	pOutput->u8_logo_sc_hold_frm = 0;

	pOutput->u1_logo_sc_dtldif_status = 0;
	pOutput->u1_logo_sc_Gmv_status = 0;
	pOutput->u1_logo_sc_saddif_status = 0;
	pOutput->u8_Rg_static_num = 0;
	pOutput->u32_Rg_unstatic_idx = 0;

	pOutput->u1_logo_netflix_status = 0;
	pOutput->u8_logo_netflix_hold_frm = 0;

	pOutput->u1_logo_lg16s_patch_status   = 0;
	pOutput->u8_logo_lg16s_patch_hold_frm = 0;

	pOutput->u1_logo_lg16s_patch_Gapli_status     = 0;
	pOutput->u1_logo_lg16s_patch_Gaplp_status     = 0;
	pOutput->u1_logo_lg16s_patch_Gdtl_status     = 0;
	pOutput->u1_logo_lg16s_patch_Gsad_status     = 0;
	pOutput->u1_logo_lg16s_patch_Gmv_status      = 0;
	pOutput->u1_logo_lg16s_patch_Rdtl_num_status = 0;

	// UXN patch
	pOutput->u1_lg_UXN_patch_det = 0;

	// Defualt value storage
	ReadRegister(KME_LOGO0_KME_LOGO0_68_reg, 0, 10, &pOutput->DefaultVal.reg_km_logo_blkgrdsum2_th);
	ReadRegister(KME_LOGO0_KME_LOGO0_EC_reg, 8, 8, &pOutput->DefaultVal.reg_km_logo_blkadphstystep_en);
	ReadRegister(KME_LOGO0_KME_LOGO0_EC_reg, 0, 3, &pOutput->DefaultVal.reg_km_logo_blkhsty_nstep);
	ReadRegister(KME_LOGO0_KME_LOGO0_EC_reg, 4, 7, &pOutput->DefaultVal.reg_km_logo_blkhsty_pstep);

	ReadRegister(MC2_MC2_98_reg,8, 8, &pOutput->DefaultVal.reg_mc_logo_vlpf_en);
	ReadRegister(KME_LOGO0_KME_LOGO0_10_reg, 0, 4, &pOutput->DefaultVal.reg_km_logo_iir_alpha);
	ReadRegister(MC2_MC2_20_reg, 0, 1, &pOutput->DefaultVal.reg_mc_logo_en);

	ReadRegister(KME_LOGO1_KME_LOGO1_10_reg,  0,  3,  &pOutput->DefaultVal.blkhsty_pth[0]);
	ReadRegister(KME_LOGO1_KME_LOGO1_10_reg,  4,  7,  &pOutput->DefaultVal.blkhsty_pth[1]);
	ReadRegister(KME_LOGO1_KME_LOGO1_10_reg,  8,  11,  &pOutput->DefaultVal.blkhsty_pth[2]);
	ReadRegister(KME_LOGO1_KME_LOGO1_10_reg,  12,  15,  &pOutput->DefaultVal.blkhsty_pth[3]);
	ReadRegister(KME_LOGO1_KME_LOGO1_10_reg,  16,  19,  &pOutput->DefaultVal.blkhsty_pth[4]);
	ReadRegister(KME_LOGO1_KME_LOGO1_10_reg,  20,  23,  &pOutput->DefaultVal.blkhsty_pth[5]);
	ReadRegister(KME_LOGO1_KME_LOGO1_10_reg,  24,  27,  &pOutput->DefaultVal.blkhsty_pth[6]);
	ReadRegister(KME_LOGO1_KME_LOGO1_10_reg,  28,  31,  &pOutput->DefaultVal.blkhsty_pth[7]);
	ReadRegister(KME_LOGO1_KME_LOGO1_14_reg,  0,  3,  &pOutput->DefaultVal.blkhsty_pth[8]);
	ReadRegister(KME_LOGO1_KME_LOGO1_14_reg,  4,  7,  &pOutput->DefaultVal.blkhsty_pth[9]);
	ReadRegister(KME_LOGO1_KME_LOGO1_14_reg, 8, 11, &pOutput->DefaultVal.blkhsty_pth[10]);
	ReadRegister(KME_LOGO1_KME_LOGO1_14_reg, 12, 15, &pOutput->DefaultVal.blkhsty_pth[11]);
	ReadRegister(KME_LOGO1_KME_LOGO1_14_reg, 16, 19, &pOutput->DefaultVal.blkhsty_pth[12]);
	ReadRegister(KME_LOGO1_KME_LOGO1_14_reg, 20, 23, &pOutput->DefaultVal.blkhsty_pth[13]);
	ReadRegister(KME_LOGO1_KME_LOGO1_14_reg, 24, 27, &pOutput->DefaultVal.blkhsty_pth[14]);

	ReadRegister(KME_LOGO1_KME_LOGO1_18_reg,  0,  3,  &pOutput->DefaultVal.blkhsty_nth[0]);
	ReadRegister(KME_LOGO1_KME_LOGO1_18_reg,  4,  7,  &pOutput->DefaultVal.blkhsty_nth[1]);
	ReadRegister(KME_LOGO1_KME_LOGO1_18_reg,  8,  11,  &pOutput->DefaultVal.blkhsty_nth[2]);
	ReadRegister(KME_LOGO1_KME_LOGO1_18_reg,  12,  15,  &pOutput->DefaultVal.blkhsty_nth[3]);
	ReadRegister(KME_LOGO1_KME_LOGO1_18_reg,  16,  19,  &pOutput->DefaultVal.blkhsty_nth[4]);
	ReadRegister(KME_LOGO1_KME_LOGO1_18_reg,  20,  23,  &pOutput->DefaultVal.blkhsty_nth[5]);
	ReadRegister(KME_LOGO1_KME_LOGO1_18_reg,  24,  27,  &pOutput->DefaultVal.blkhsty_nth[6]);
	ReadRegister(KME_LOGO1_KME_LOGO1_18_reg,  28,  31,  &pOutput->DefaultVal.blkhsty_nth[7]);
	ReadRegister(KME_LOGO1_KME_LOGO1_1C_reg,  0,  3,  &pOutput->DefaultVal.blkhsty_nth[8]);
	ReadRegister(KME_LOGO1_KME_LOGO1_1C_reg,  4,  7,  &pOutput->DefaultVal.blkhsty_nth[9]);
	ReadRegister(KME_LOGO1_KME_LOGO1_1C_reg, 8, 11, &pOutput->DefaultVal.blkhsty_nth[10]);
	ReadRegister(KME_LOGO1_KME_LOGO1_1C_reg, 12, 15, &pOutput->DefaultVal.blkhsty_nth[11]);
	ReadRegister(KME_LOGO1_KME_LOGO1_1C_reg, 16, 19, &pOutput->DefaultVal.blkhsty_nth[12]);
	ReadRegister(KME_LOGO1_KME_LOGO1_1C_reg, 20, 23, &pOutput->DefaultVal.blkhsty_nth[13]);
	ReadRegister(KME_LOGO1_KME_LOGO1_1C_reg, 24, 27, &pOutput->DefaultVal.blkhsty_nth[14]);

	ReadRegister(KME_LOGO0_KME_LOGO0_A4_reg,  0,  7,  &pOutput->DefaultVal.blksamethr_l[0]);
	ReadRegister(KME_LOGO0_KME_LOGO0_A4_reg,  8,  15,  &pOutput->DefaultVal.blksamethr_l[1]);
	ReadRegister(KME_LOGO0_KME_LOGO0_A4_reg,  16,  23,  &pOutput->DefaultVal.blksamethr_l[2]);
	ReadRegister(KME_LOGO0_KME_LOGO0_A4_reg,  24,  31,  &pOutput->DefaultVal.blksamethr_l[3]);
	ReadRegister(KME_LOGO0_KME_LOGO0_A8_reg,  0,  7,  &pOutput->DefaultVal.blksamethr_l[4]);
	ReadRegister(KME_LOGO0_KME_LOGO0_A8_reg,  8,  15,  &pOutput->DefaultVal.blksamethr_l[5]);
	ReadRegister(KME_LOGO0_KME_LOGO0_A8_reg,  16,  23,  &pOutput->DefaultVal.blksamethr_l[6]);
	ReadRegister(KME_LOGO0_KME_LOGO0_A8_reg,  24,  31,  &pOutput->DefaultVal.blksamethr_l[7]);
	ReadRegister(KME_LOGO0_KME_LOGO0_AC_reg,  0,  7,  &pOutput->DefaultVal.blksamethr_l[8]);
	ReadRegister(KME_LOGO0_KME_LOGO0_AC_reg,  8,  15,  &pOutput->DefaultVal.blksamethr_l[9]);
	ReadRegister(KME_LOGO0_KME_LOGO0_AC_reg, 16, 23, &pOutput->DefaultVal.blksamethr_l[10]);
	ReadRegister(KME_LOGO0_KME_LOGO0_AC_reg, 24, 31, &pOutput->DefaultVal.blksamethr_l[11]);
	ReadRegister(KME_LOGO0_KME_LOGO0_B0_reg, 0, 7, &pOutput->DefaultVal.blksamethr_l[12]);
	ReadRegister(KME_LOGO0_KME_LOGO0_B0_reg, 8, 15, &pOutput->DefaultVal.blksamethr_l[13]);
	ReadRegister(KME_LOGO0_KME_LOGO0_B0_reg, 16, 23, &pOutput->DefaultVal.blksamethr_l[14]);

	ReadRegister(KME_LOGO0_KME_LOGO0_C4_reg,  0,  7,  &pOutput->DefaultVal.blksamethr_a[0]);
	ReadRegister(KME_LOGO0_KME_LOGO0_C4_reg,  8,  15,  &pOutput->DefaultVal.blksamethr_a[1]);
	ReadRegister(KME_LOGO0_KME_LOGO0_C4_reg,  16,  23,  &pOutput->DefaultVal.blksamethr_a[2]);
	ReadRegister(KME_LOGO0_KME_LOGO0_C4_reg,  24,  31,  &pOutput->DefaultVal.blksamethr_a[3]);
	ReadRegister(KME_LOGO0_KME_LOGO0_C8_reg,  0,  7,  &pOutput->DefaultVal.blksamethr_a[4]);
	ReadRegister(KME_LOGO0_KME_LOGO0_C8_reg,  8,  15,  &pOutput->DefaultVal.blksamethr_a[5]);
	ReadRegister(KME_LOGO0_KME_LOGO0_C8_reg,  16,  23,  &pOutput->DefaultVal.blksamethr_a[6]);
	ReadRegister(KME_LOGO0_KME_LOGO0_C8_reg,  24,  31,  &pOutput->DefaultVal.blksamethr_a[7]);
	ReadRegister(KME_LOGO0_KME_LOGO0_CC_reg,  0,  7,  &pOutput->DefaultVal.blksamethr_a[8]);
	ReadRegister(KME_LOGO0_KME_LOGO0_CC_reg,  8,  15,  &pOutput->DefaultVal.blksamethr_a[9]);
	ReadRegister(KME_LOGO0_KME_LOGO0_CC_reg, 16, 23, &pOutput->DefaultVal.blksamethr_a[10]);
	ReadRegister(KME_LOGO0_KME_LOGO0_CC_reg, 24, 31, &pOutput->DefaultVal.blksamethr_a[11]);
	ReadRegister(KME_LOGO0_KME_LOGO0_D0_reg, 0, 7, &pOutput->DefaultVal.blksamethr_a[12]);
	ReadRegister(KME_LOGO0_KME_LOGO0_D0_reg, 8, 15, &pOutput->DefaultVal.blksamethr_a[13]);
	ReadRegister(KME_LOGO0_KME_LOGO0_D0_reg, 16, 23, &pOutput->DefaultVal.blksamethr_a[14]);

	ReadRegister(KME_LOGO0_KME_LOGO0_B4_reg,  0,  7,  &pOutput->DefaultVal.blksamethr_h[0]);
	ReadRegister(KME_LOGO0_KME_LOGO0_B4_reg,  8,  15,  &pOutput->DefaultVal.blksamethr_h[1]);
	ReadRegister(KME_LOGO0_KME_LOGO0_B4_reg,  16,  23,  &pOutput->DefaultVal.blksamethr_h[2]);
	ReadRegister(KME_LOGO0_KME_LOGO0_B4_reg,  24,  31,  &pOutput->DefaultVal.blksamethr_h[3]);
	ReadRegister(KME_LOGO0_KME_LOGO0_B8_reg,  0,  7,  &pOutput->DefaultVal.blksamethr_h[4]);
	ReadRegister(KME_LOGO0_KME_LOGO0_B8_reg,  8,  15,  &pOutput->DefaultVal.blksamethr_h[5]);
	ReadRegister(KME_LOGO0_KME_LOGO0_B8_reg,  16,  23,  &pOutput->DefaultVal.blksamethr_h[6]);
	ReadRegister(KME_LOGO0_KME_LOGO0_B8_reg,  24,  31,  &pOutput->DefaultVal.blksamethr_h[7]);
	ReadRegister(KME_LOGO0_KME_LOGO0_BC_reg,  0,  7,  &pOutput->DefaultVal.blksamethr_h[8]);
	ReadRegister(KME_LOGO0_KME_LOGO0_BC_reg,  8,  15,  &pOutput->DefaultVal.blksamethr_h[9]);
	ReadRegister(KME_LOGO0_KME_LOGO0_BC_reg, 16, 23, &pOutput->DefaultVal.blksamethr_h[10]);
	ReadRegister(KME_LOGO0_KME_LOGO0_BC_reg, 24, 31, &pOutput->DefaultVal.blksamethr_h[11]);
	ReadRegister(KME_LOGO0_KME_LOGO0_C0_reg, 0, 7, &pOutput->DefaultVal.blksamethr_h[12]);
	ReadRegister(KME_LOGO0_KME_LOGO0_C0_reg, 8, 15, &pOutput->DefaultVal.blksamethr_h[13]);
	ReadRegister(KME_LOGO0_KME_LOGO0_C0_reg, 16, 23, &pOutput->DefaultVal.blksamethr_h[14]);

	// Dehalo logo process default value
	ReadRegister(KME_DEHALO2_KME_DEHALO2_90_reg, 0, 2, &pOutput->DefaultVal.dh_logo_bypass[0][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_90_reg, 4, 6, &pOutput->DefaultVal.dh_logo_bypass[0][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_90_reg, 12, 14, &pOutput->DefaultVal.dh_logo_bypass[0][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_90_reg, 8, 10, &pOutput->DefaultVal.dh_logo_bypass[0][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_90_reg, 15, 15, &pOutput->DefaultVal.dh_logo_bypass[0][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_90_reg, 0, 2, &pOutput->DefaultVal.dh_logo_bypass[0][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_90_reg, 4, 6, &pOutput->DefaultVal.dh_logo_bypass[0][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_90_reg, 12, 14, &pOutput->DefaultVal.dh_logo_bypass[0][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_90_reg, 8, 10, &pOutput->DefaultVal.dh_logo_bypass[0][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_90_reg, 15, 15, &pOutput->DefaultVal.dh_logo_bypass[0][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_A8_reg, 1, 3, &pOutput->DefaultVal.dh_logo_bypass[1][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_A8_reg, 4, 6, &pOutput->DefaultVal.dh_logo_bypass[1][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_A8_reg, 10, 12, &pOutput->DefaultVal.dh_logo_bypass[1][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_A8_reg, 7, 9, &pOutput->DefaultVal.dh_logo_bypass[1][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_A8_reg, 0, 0, &pOutput->DefaultVal.dh_logo_bypass[1][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_E8_reg, 0, 2, &pOutput->DefaultVal.dh_logo_bypass[2][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_E8_reg, 3, 5, &pOutput->DefaultVal.dh_logo_bypass[2][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_E8_reg, 9, 11, &pOutput->DefaultVal.dh_logo_bypass[2][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_E8_reg, 6, 8, &pOutput->DefaultVal.dh_logo_bypass[2][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_E8_reg, 12, 12, &pOutput->DefaultVal.dh_logo_bypass[2][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_14_reg, 0, 2, &pOutput->DefaultVal.dh_logo_bypass[3][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_14_reg, 3, 5, &pOutput->DefaultVal.dh_logo_bypass[3][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_14_reg, 9, 11, &pOutput->DefaultVal.dh_logo_bypass[3][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_14_reg, 6, 8, &pOutput->DefaultVal.dh_logo_bypass[3][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_14_reg, 12, 12, &pOutput->DefaultVal.dh_logo_bypass[3][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_14_reg, 16, 18, &pOutput->DefaultVal.dh_logo_bypass[4][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_14_reg, 19, 21, &pOutput->DefaultVal.dh_logo_bypass[4][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_14_reg, 25, 27, &pOutput->DefaultVal.dh_logo_bypass[4][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_14_reg, 22, 24, &pOutput->DefaultVal.dh_logo_bypass[4][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_14_reg, 28, 28, &pOutput->DefaultVal.dh_logo_bypass[4][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_18_reg, 0, 2, &pOutput->DefaultVal.dh_logo_bypass[5][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_18_reg, 3, 5, &pOutput->DefaultVal.dh_logo_bypass[5][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_18_reg, 9, 11, &pOutput->DefaultVal.dh_logo_bypass[5][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_18_reg, 6, 8, &pOutput->DefaultVal.dh_logo_bypass[5][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_18_reg, 12, 12, &pOutput->DefaultVal.dh_logo_bypass[5][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_18_reg, 16, 18, &pOutput->DefaultVal.dh_logo_bypass[6][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_18_reg, 19, 21, &pOutput->DefaultVal.dh_logo_bypass[6][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_18_reg, 25, 27, &pOutput->DefaultVal.dh_logo_bypass[6][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_18_reg, 22, 24, &pOutput->DefaultVal.dh_logo_bypass[6][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_18_reg, 28, 28, &pOutput->DefaultVal.dh_logo_bypass[6][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_1C_reg, 0, 2, &pOutput->DefaultVal.dh_logo_bypass[7][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_1C_reg, 3, 5, &pOutput->DefaultVal.dh_logo_bypass[7][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_1C_reg, 9, 11, &pOutput->DefaultVal.dh_logo_bypass[7][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_1C_reg, 6, 8, &pOutput->DefaultVal.dh_logo_bypass[7][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_1C_reg, 12, 12, &pOutput->DefaultVal.dh_logo_bypass[7][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_1C_reg, 16, 18, &pOutput->DefaultVal.dh_logo_bypass[8][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_1C_reg, 19, 21, &pOutput->DefaultVal.dh_logo_bypass[8][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_1C_reg, 25, 27, &pOutput->DefaultVal.dh_logo_bypass[8][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_1C_reg, 22, 24, &pOutput->DefaultVal.dh_logo_bypass[8][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_1C_reg, 28, 28, &pOutput->DefaultVal.dh_logo_bypass[8][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_20_reg, 0, 2, &pOutput->DefaultVal.dh_logo_bypass[9][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_20_reg, 3, 5, &pOutput->DefaultVal.dh_logo_bypass[9][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_20_reg, 9, 11, &pOutput->DefaultVal.dh_logo_bypass[9][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_20_reg, 6, 8, &pOutput->DefaultVal.dh_logo_bypass[9][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_20_reg, 12, 12, &pOutput->DefaultVal.dh_logo_bypass[9][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_20_reg, 16, 18, &pOutput->DefaultVal.dh_logo_bypass[10][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_20_reg, 19, 21, &pOutput->DefaultVal.dh_logo_bypass[10][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_20_reg, 25, 27, &pOutput->DefaultVal.dh_logo_bypass[10][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_20_reg, 22, 24, &pOutput->DefaultVal.dh_logo_bypass[10][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_20_reg, 28, 28, &pOutput->DefaultVal.dh_logo_bypass[10][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_24_reg, 0, 2, &pOutput->DefaultVal.dh_logo_bypass[11][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_24_reg, 3, 5, &pOutput->DefaultVal.dh_logo_bypass[11][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_24_reg, 9, 11, &pOutput->DefaultVal.dh_logo_bypass[11][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_24_reg, 6, 8, &pOutput->DefaultVal.dh_logo_bypass[11][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_24_reg, 12, 12, &pOutput->DefaultVal.dh_logo_bypass[11][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_24_reg, 16, 18, &pOutput->DefaultVal.dh_logo_bypass[12][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_24_reg, 19, 21, &pOutput->DefaultVal.dh_logo_bypass[12][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_24_reg, 25, 27, &pOutput->DefaultVal.dh_logo_bypass[12][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_24_reg, 22, 24, &pOutput->DefaultVal.dh_logo_bypass[12][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_24_reg, 28, 28, &pOutput->DefaultVal.dh_logo_bypass[12][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_28_reg, 0, 2, &pOutput->DefaultVal.dh_logo_bypass[13][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_28_reg, 3, 5, &pOutput->DefaultVal.dh_logo_bypass[13][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_28_reg, 9, 11, &pOutput->DefaultVal.dh_logo_bypass[13][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_28_reg, 6, 8, &pOutput->DefaultVal.dh_logo_bypass[13][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_28_reg, 12, 12, &pOutput->DefaultVal.dh_logo_bypass[13][4]);

	ReadRegister(KME_DEHALO2_KME_DEHALO2_28_reg, 16, 18, &pOutput->DefaultVal.dh_logo_bypass[14][0]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_28_reg, 19, 21, &pOutput->DefaultVal.dh_logo_bypass[14][1]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_28_reg, 25, 27, &pOutput->DefaultVal.dh_logo_bypass[14][2]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_28_reg, 22, 24, &pOutput->DefaultVal.dh_logo_bypass[14][3]);
	ReadRegister(KME_DEHALO2_KME_DEHALO2_28_reg, 28, 28, &pOutput->DefaultVal.dh_logo_bypass[14][4]);

// HS merlin7
	ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 30, 30, &u32_RB_val);	// bool_dh_logo_15rgn_ctrl_en
	if( u32_RB_val == 0 )	// 3 regions settings
	{
	// Clear threshold
		ReadRegister(KME_DEHALO2_KME_DEHALO2_84_reg, 12, 21, &pOutput->DefaultVal.dh_logo_RgnThr[0][0]);	// HS dh_logo_sad_d_th_rg0
		ReadRegister(KME_DEHALO2_KME_DEHALO2_84_reg,  6, 11, &pOutput->DefaultVal.dh_logo_RgnThr[1][0]);	// HS dh_logo_mv_d_th_rg0
		ReadRegister(KME_DEHALO2_KME_DEHALO2_84_reg,  0,  5, &pOutput->DefaultVal.dh_logo_RgnThr[2][0]);	// HS dh_logo_mv_d_diff_th_rg0
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A0_reg, 16, 25, &pOutput->DefaultVal.dh_logo_RgnThr[3][0]);	// HS dh_logo_sad_t_th_rg0
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A0_reg,  0,  7, &pOutput->DefaultVal.dh_logo_RgnThr[4][0]);	// HS dh_logo_mv_t_th_rg0
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A0_reg,  8, 15, &pOutput->DefaultVal.dh_logo_RgnThr[5][0]);	// HS dh_logo_tmv_num_rg0
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A0_reg, 26, 27, &pOutput->DefaultVal.dh_logo_RgnThr[6][0]);	// HS dh_logo_tmv_mode_rg0

		ReadRegister(KME_DEHALO2_KME_DEHALO2_AC_reg, 12, 21, &pOutput->DefaultVal.dh_logo_RgnThr[0][1]);	// HS dh_logo_sad_d_th_rg1
		ReadRegister(KME_DEHALO2_KME_DEHALO2_AC_reg,  6, 11, &pOutput->DefaultVal.dh_logo_RgnThr[1][1]);	// HS dh_logo_mv_d_th_rg1
		ReadRegister(KME_DEHALO2_KME_DEHALO2_AC_reg,  0,  5, &pOutput->DefaultVal.dh_logo_RgnThr[2][1]);	// HS dh_logo_mv_d_diff_th_rg1
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A4_reg, 16, 25, &pOutput->DefaultVal.dh_logo_RgnThr[3][1]);	// HS dh_logo_sad_t_th_rg1
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A4_reg,  0,  7, &pOutput->DefaultVal.dh_logo_RgnThr[4][1]);	// HS dh_logo_mv_t_th_rg1
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A4_reg,  8, 15, &pOutput->DefaultVal.dh_logo_RgnThr[5][1]);	// HS dh_logo_tmv_num_rg1
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A4_reg, 26, 27, &pOutput->DefaultVal.dh_logo_RgnThr[6][1]);	// HS dh_logo_tmv_mode_rg1

		ReadRegister(KME_DEHALO2_KME_DEHALO2_08_reg,  0,  9, &pOutput->DefaultVal.dh_logo_RgnThr[0][2]);	// HS sad_d_th_rg2
		ReadRegister(KME_DEHALO2_KME_DEHALO2_E8_reg, 19, 24, &pOutput->DefaultVal.dh_logo_RgnThr[1][2]);	// HS mv_d_th_rg2
		ReadRegister(KME_DEHALO2_KME_DEHALO2_E8_reg, 13, 18, &pOutput->DefaultVal.dh_logo_RgnThr[2][2]);	// HS mv_d_diff_th_rg2
		ReadRegister(KME_DEHALO2_KME_DEHALO2_10_reg,  8, 17, &pOutput->DefaultVal.dh_logo_RgnThr[3][2]);	// HS sad_t_th_rg2
		ReadRegister(KME_DEHALO2_KME_DEHALO2_08_reg, 10, 17, &pOutput->DefaultVal.dh_logo_RgnThr[4][2]);	// HS mv_t_th_rg2
		ReadRegister(KME_DEHALO2_KME_DEHALO2_10_reg,  0,  7, &pOutput->DefaultVal.dh_logo_RgnThr[5][2]);	// HS tmv_num_rg2
		ReadRegister(KME_DEHALO2_KME_DEHALO2_E8_reg, 25, 26, &pOutput->DefaultVal.dh_logo_RgnThr[6][2]);	// HS tmv_mode_rg2
	}	
	else if( u32_RB_val == 1 )	// 15 regions settings
	{
		ReadRegister(KME_DEHALO2_KME_DEHALO2_84_reg, 12, 21, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][0]);	// HS dh_logo_sad_d_th_rg0
		ReadRegister(KME_DEHALO2_KME_DEHALO2_84_reg,  6, 11, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][0]);	// HS dh_logo_mv_d_th_rg0
		ReadRegister(KME_DEHALO2_KME_DEHALO2_84_reg,  0,  5, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][0]);	// HS dh_logo_mv_d_diff_th_rg0
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A0_reg, 16, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][0]);	// HS dh_logo_sad_t_th_rg0
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A0_reg,  0,  7, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][0]);	// HS dh_logo_mv_t_th_rg0
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A0_reg,  8, 15, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][0]);	// HS dh_logo_tmv_num_rg0
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A0_reg, 26, 27, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][0]);	// HS dh_logo_tmv_mode_rg0

		ReadRegister(KME_DEHALO2_KME_DEHALO2_AC_reg, 12, 21, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][1]);	// HS dh_logo_sad_d_th_rg1
		ReadRegister(KME_DEHALO2_KME_DEHALO2_AC_reg,  6, 11, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][1]);	// HS dh_logo_mv_d_th_rg1
		ReadRegister(KME_DEHALO2_KME_DEHALO2_AC_reg,  0,  5, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][1]);	// HS dh_logo_mv_d_diff_th_rg1
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A4_reg, 16, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][1]);	// HS dh_logo_sad_t_th_rg1
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A4_reg,  0,  7, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][1]);	// HS dh_logo_mv_t_th_rg1
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A4_reg,  8, 15, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][1]);	// HS dh_logo_tmv_num_rg1
		ReadRegister(KME_DEHALO2_KME_DEHALO2_A4_reg, 26, 27, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][1]);	// HS dh_logo_tmv_mode_rg1

		ReadRegister(KME_DEHALO2_KME_DEHALO2_08_reg,  0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][2]);	// HS sad_d_th_rg2
		ReadRegister(KME_DEHALO2_KME_DEHALO2_E8_reg, 19, 24, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][2]);	// HS mv_d_th_rg2
		ReadRegister(KME_DEHALO2_KME_DEHALO2_E8_reg, 13, 18, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][2]);	// HS mv_d_diff_th_rg2
		ReadRegister(KME_DEHALO2_KME_DEHALO2_10_reg,  8, 17, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][2]);	// HS sad_t_th_rg2
		ReadRegister(KME_DEHALO2_KME_DEHALO2_08_reg, 10, 17, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][2]);	// HS mv_t_th_rg2
		ReadRegister(KME_DEHALO2_KME_DEHALO2_10_reg,  0,  7, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][2]);	// HS tmv_num_rg2
		ReadRegister(KME_DEHALO2_KME_DEHALO2_E8_reg, 25, 26, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][2]);	// HS tmv_mode_rg2

		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg3_reg,            0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][3]);	// dh_logo_sad_d_th_rg3
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg3_reg,           20, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][3]);	// dh_logo_mv_d_th_rg3
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg3_reg,           26, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][3]);	// dh_logo_mv_d_diff_th_rg3
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg3_reg,           10, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][3]);	// dh_logo_sad_t_th_rg3
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg3_rg4_reg,        8, 15, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][3]);	// dh_logo_mv_t_th_rg3
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg3_rg4_reg,        0,  7, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][3]);	// dh_logo_tmv_num_rg3
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 6,  7, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][3]);	// dh_logo_tmv_mode_rg3

		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg4_reg,            0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][4]);	// dh_logo_sad_d_th_rg4
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg4_reg,           20, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][4]);	// dh_logo_mv_d_th_rg4
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg4_reg,           26, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][4]);	// dh_logo_mv_d_diff_th_rg4
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg4_reg,           10, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][4]);	// dh_logo_sad_t_th_rg4
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg3_rg4_reg,       24, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][4]);	// dh_logo_mv_t_th_rg4
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg3_rg4_reg,       16, 23, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][4]);	// dh_logo_tmv_num_rg4
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 8,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][4]);	// dh_logo_tmv_mode_rg4

		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg5_reg,             0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][5]);	// dh_logo_sad_d_th_rg5
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg5_reg,            20, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][5]);	// dh_logo_mv_d_th_rg5
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg5_reg,            26, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][5]);	// dh_logo_mv_d_diff_th_rg5
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg5_reg,            10, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][5]);	// dh_logo_sad_t_th_rg5
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg5_rg6_reg,         8, 15, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][5]);	// dh_logo_mv_t_th_rg5
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg5_rg6_reg,         0,  7, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][5]);	// dh_logo_tmv_num_rg5
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 10, 11, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][5]);	// dh_logo_tmv_mode_rg5

		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg6_reg,             0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][6]);	// dh_logo_sad_d_th_rg6
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg6_reg,            20, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][6]);	// dh_logo_mv_d_th_rg6
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg6_reg,            26, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][6]);	// dh_logo_mv_d_diff_th_rg6
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg6_reg,            10, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][6]);	// dh_logo_sad_t_th_rg6
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg5_rg6_reg,        24, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][6]);	// dh_logo_mv_t_th_rg6
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg5_rg6_reg,        16, 23, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][6]);	// dh_logo_tmv_num_rg6
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 12, 13, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][6]);	// dh_logo_tmv_mode_rg6

		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg7_reg,             0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][7]);	// dh_logo_sad_d_th_rg7
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg7_reg,            20, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][7]);	// dh_logo_mv_d_th_rg7
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg7_reg,            26, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][7]);	// dh_logo_mv_d_diff_th_rg7
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg7_reg,            10, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][7]);	// dh_logo_sad_t_th_rg7
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg7_rg8_reg,         8, 15, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][7]);	// dh_logo_mv_t_th_rg7
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg7_rg8_reg,         0,  7, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][7]);	// dh_logo_tmv_num_rg7
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 14, 15, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][7]);	// dh_logo_tmv_mode_rg7

		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg8_reg,             0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][8]);	// dh_logo_sad_d_th_rg8
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg8_reg,            20, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][8]);	// dh_logo_mv_d_th_rg8
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg8_reg,            26, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][8]);	// dh_logo_mv_d_diff_th_rg8
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg8_reg,            10, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][8]);	// dh_logo_sad_t_th_rg8
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg7_rg8_reg,        24, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][8]);	// dh_logo_mv_t_th_rg8
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg7_rg8_reg,        16, 23, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][8]);	// dh_logo_tmv_num_rg8
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 16, 17, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][8]);	// dh_logo_tmv_mode_rg8

		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg9_reg,             0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][9]);	// dh_logo_sad_d_th_rg9
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg9_reg,            20, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][9]);	// dh_logo_mv_d_th_rg9
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg9_reg,            26, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][9]);	// dh_logo_mv_d_diff_th_rg9
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rg9_reg,            10, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][9]);	// dh_logo_sad_t_th_rg9
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg9_rga_reg,         8, 15, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][9]);	// dh_logo_mv_t_th_rg9
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg9_rga_reg,         0,  7, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][9]);	// dh_logo_tmv_num_rg9
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 18, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][9]);	// dh_logo_tmv_mode_rg9

		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rga_reg,             0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][10]);	// dh_logo_sad_d_th_rga
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rga_reg,            20, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][10]);	// dh_logo_mv_d_th_rga
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rga_reg,            26, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][10]);	// dh_logo_mv_d_diff_th_rga
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rga_reg,            10, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][10]);	// dh_logo_sad_t_th_rga
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg9_rga_reg,        24, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][10]);	// dh_logo_mv_t_th_rga
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rg9_rga_reg,        16, 23, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][10]);	// dh_logo_tmv_num_rga
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 20, 21, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][10]);	// dh_logo_tmv_mode_rga

		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rgb_reg,             0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][11]);		// dh_logo_sad_d_th_rgb
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rgb_reg,            20, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][11]);	// dh_logo_mv_d_th_rgb
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rgb_reg,            26, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][11]);	// dh_logo_mv_d_diff_th_rgb
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rgb_reg,            10, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][11]);	// dh_logo_sad_t_th_rgb
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rgb_rgc_reg,         8, 15, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][11]);	// dh_logo_mv_t_th_rgb
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rgb_rgc_reg,         0,  7, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][11]);		// dh_logo_tmv_num_rgb
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 22, 23, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][11]);	// dh_logo_tmv_mode_rgb

		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rgc_reg,             0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][12]);		// dh_logo_sad_d_th_rgc
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rgc_reg,            20, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][12]);	// dh_logo_mv_d_th_rgc
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rgc_reg,            26, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][12]);	// dh_logo_mv_d_diff_th_rgc
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rgc_reg,            10, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][12]);	// dh_logo_sad_t_th_rgc
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rgb_rgc_reg,        24, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][12]);	// dh_logo_mv_t_th_rgc
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rgb_rgc_reg,        16, 23, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][12]);	// dh_logo_tmv_num_rgc
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 24, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][12]);	// dh_logo_tmv_mode_rgc

		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rgd_reg,             0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][13]);		// dh_logo_sad_d_th_rgd
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rgd_reg,            20, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][13]);	// dh_logo_mv_d_th_rgd
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rgd_reg,            26, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][13]);	// dh_logo_mv_d_diff_th_rgd
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rgd_reg,            10, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][13]);	// dh_logo_sad_t_th_rgd
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rgd_rge_reg,         8, 15, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][13]);	// dh_logo_mv_t_th_rgd
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rgd_rge_reg,         0,  7, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][13]);		// dh_logo_tmv_num_rgd
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 26, 27, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][13]);	// dh_logo_tmv_mode_rgd

		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rge_reg,             0,  9, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[0][14]);		// dh_logo_sad_d_th_rge
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rge_reg,            20, 25, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[1][14]);	// dh_logo_mv_d_th_rge
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rge_reg,            26, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[2][14]);	// dh_logo_mv_d_diff_th_rge
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_1_rge_reg,            10, 19, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[3][14]);	// dh_logo_sad_t_th_rge
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rgd_rge_reg,        24, 31, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[4][14]);	// dh_logo_mv_t_th_rge
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_2_rgd_rge_reg,        16, 23, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[5][14]);	// dh_logo_tmv_num_rge
		ReadRegister(KME_DEHALO6_dh_logo_ctrl_func_en_tmv_mode_reg, 28, 29, &pOutput->DefaultVal.dh_logo_RgnThr_15rgn[6][14]);	// dh_logo_tmv_mode_rge
	}
// HS merlin7

	// OBME1 pattern selection
	{
		#if 1
		kme_vbuf_top_kme_vbuf_top_8c_RBUS	kme_vbuf_top_kme_vbuf_top_8c;	
		kme_vbuf_top_kme_vbuf_top_8c.regValue = rtd_inl(KME_VBUF_TOP_KME_VBUF_TOP_8C_reg);

		pOutput->DefaultVal.me1_ip_dc_obme_mode = kme_vbuf_top_kme_vbuf_top_8c.vbuf_ip_dc_obme_mode;
		pOutput->DefaultVal.me1_pi_dc_obme_mode	= kme_vbuf_top_kme_vbuf_top_8c.vbuf_pi_dc_obme_mode;
		pOutput->DefaultVal.me1_ip_dc_obme_mode_sel	= kme_vbuf_top_kme_vbuf_top_8c.vbuf_ip_dc_obme_mode_sel;
		pOutput->DefaultVal.me1_pi_dc_obme_mode_sel = kme_vbuf_top_kme_vbuf_top_8c.vbuf_pi_dc_obme_mode_sel;
		pOutput->DefaultVal.me1_ip_ac_obme_mode	= kme_vbuf_top_kme_vbuf_top_8c.vbuf_ip_ac_obme_mode;
		pOutput->DefaultVal.me1_pi_ac_obme_mode = kme_vbuf_top_kme_vbuf_top_8c.vbuf_pi_ac_obme_mode;
		#else
		ReadRegister(KME_VBUF_TOP_KME_VBUF_TOP_8C_reg,22,22, &pOutput->DefaultVal.me1_ip_dc_obme_mode_sel);
		ReadRegister(KME_VBUF_TOP_KME_VBUF_TOP_8C_reg,18,19, &pOutput->DefaultVal.me1_ip_dc_obme_mode);
		ReadRegister(KME_VBUF_TOP_KME_VBUF_TOP_8C_reg,24,25, &pOutput->DefaultVal.me1_ip_ac_obme_mode);
		ReadRegister(KME_VBUF_TOP_KME_VBUF_TOP_8C_reg,20,21, &pOutput->DefaultVal.me1_pi_dc_obme_mode_sel);
		ReadRegister(KME_VBUF_TOP_KME_VBUF_TOP_8C_reg,20,21, &pOutput->DefaultVal.me1_pi_dc_obme_mode);
		ReadRegister(KME_VBUF_TOP_KME_VBUF_TOP_8C_reg,26,27, &pOutput->DefaultVal.me1_pi_ac_obme_mode);
		#endif
	}

	//OBME2 pattern selection
	{
		#if 1
		kme_me2_vbuf_top_kme_me2_vbuf_top_d8_RBUS kme_me2_vbuf_top_kme_me2_vbuf_top_d8;
		kme_me2_vbuf_top_kme_me2_vbuf_top_dc_RBUS kme_me2_vbuf_top_kme_me2_vbuf_top_dc;
		
		kme_me2_vbuf_top_kme_me2_vbuf_top_d8.regValue = rtd_inl(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg);
		pOutput->DefaultVal.me2_1st_ac_obme_mode = kme_me2_vbuf_top_kme_me2_vbuf_top_d8.kme_me2_vbuf_1st_ac_obme_mode;
		pOutput->DefaultVal.me2_2nd_ac_obme_mode = kme_me2_vbuf_top_kme_me2_vbuf_top_d8.kme_me2_vbuf_2nd_ac_obme_mode;
		pOutput->DefaultVal.me2_fsearch_ac_obme_mode = kme_me2_vbuf_top_kme_me2_vbuf_top_d8.kme_me2_vbuf_fsearch_ac_obme_mode;
		pOutput->DefaultVal.me2_1st_dc_obme_mode =  kme_me2_vbuf_top_kme_me2_vbuf_top_d8.kme_me2_vbuf_1st_dc_obme_mode;
		pOutput->DefaultVal.me2_2nd_dc_obme_mode =  kme_me2_vbuf_top_kme_me2_vbuf_top_d8.kme_me2_vbuf_2nd_dc_obme_mode;

		kme_me2_vbuf_top_kme_me2_vbuf_top_dc.regValue = rtd_inl(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_DC_reg);
		pOutput->DefaultVal.me2_fsearch_dc_obme_mode = kme_me2_vbuf_top_kme_me2_vbuf_top_dc.kme_me2_vbuf_fsearch_dc_obme_mode;
		pOutput->DefaultVal.me2_1st_dc_obme_mode_sel = kme_me2_vbuf_top_kme_me2_vbuf_top_dc.kme_me2_vbuf_1st_dc_obme_mode_sel;
		pOutput->DefaultVal.me2_2nd_dc_obme_mode_sel = kme_me2_vbuf_top_kme_me2_vbuf_top_dc.kme_me2_vbuf_2nd_dc_obme_mode_sel;
		pOutput->DefaultVal.me2_fsearch_dc_obme_mode_sel= kme_me2_vbuf_top_kme_me2_vbuf_top_dc.kme_me2_vbuf_fsearch_dc_obme_mode_sel;
		
		#else
			
		ReadRegister(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_DC_reg,2,2, &pOutput->DefaultVal.me2_1st_dc_obme_mode_sel);
		ReadRegister(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg,28,29, &pOutput->DefaultVal.me2_1st_dc_obme_mode);
		ReadRegister(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg,21,22, &pOutput->DefaultVal.me2_1st_ac_obme_mode);
		ReadRegister(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_DC_reg,3,3, &pOutput->DefaultVal.me2_2nd_dc_obme_mode_sel);
		ReadRegister(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg,30,31, &pOutput->DefaultVal.me2_2nd_dc_obme_mode);
		ReadRegister(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_D8_reg,23,24, &pOutput->DefaultVal.me2_2nd_ac_obme_mode);
		#endif
	}
	// sld need only clear block logo result when necessary, use erosion threshold to do it
	ReadRegister(KME_LOGO2_KME_LOGO2_28_reg, 9, 14, &pOutput->DefaultVal.u6_logo_blklogopostdlt_th);
	ReadRegister(KME_LOGO2_KME_LOGO2_28_reg, 16, 21, &pOutput->DefaultVal.u6_logo_blklogopost_ero_th);
	ReadRegister(KME_LOGO2_KME_LOGO2_2C_reg, 9, 16, &pOutput->DefaultVal.u6_logo_pxllogopostdlt_th);
}
extern char circle4_flag;
VOID FRC_LgDet_BlkClrCtrl(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	unsigned int idx;
	unsigned int acc_blk_num;
	unsigned int acc_left_blk_num = 0;
	unsigned int acc_right_blk_num = 0;
	unsigned int acc_top_blk_num = 0;
	unsigned int acc_bottom_blk_num = 0;
	signed short*  hold_cnt_rg  = pOutput->s16_blklogo_rgclr_cnt;
	unsigned char i = 0;
	unsigned short u11_bg_mv = _ABS_(s_pContext->_output_read_comreg.dh_gmv_bgx) + _ABS_(s_pContext->_output_read_comreg.dh_gmv_bgy);
	PQL_OUTPUT_FRAME_RATE  out_fmRate = 0;
	out_fmRate = s_pContext->_external_data._output_frameRate;
	
#if Pro_tv0302875P
	if(s_pContext->_output_filmDetectctrl.u8_cur_cadence_Id[_FILM_ALL]==_CAD_55 && out_fmRate<=_PQL_OUT_60HZ)//_Cadence55_speical_setting
	{
		return;
	}
#endif

	acc_blk_num = 0;
	for (idx = 0; idx < LOGO_RG_32; idx++)
	{
		unsigned int blklogo_rg_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[idx];
		int u8_clr_rg_thr;
		u8_clr_rg_thr = ((idx/8 )>= 1 && idx/8 <= 2)? pParam->u8_clr_rg_thr_l : pParam->u8_clr_rg_thr  ;
		if ((((blklogo_rg_cnt * 256) / (BLK_LOGO_RG_BLKCNT_GOLDEN)) >= u8_clr_rg_thr) && pParam->u1_logo_rg_clr_en == 1)
		{
			pOutput->u32_blklogo_clr_idx = pOutput->u32_blklogo_clr_idx | (1 << idx);
			hold_cnt_rg[idx] = pParam->u6_rgClr_holdtime;
		}
		else if ((hold_cnt_rg[idx] > 1)  && pParam->u1_logo_rg_clr_en == 1)
		{
			pOutput->u32_blklogo_clr_idx = pOutput->u32_blklogo_clr_idx | (1 << idx);
			hold_cnt_rg[idx] = _MAX_(hold_cnt_rg[idx] - 1,0);
		}

		acc_blk_num += blklogo_rg_cnt;

		if (idx < 16)
		{
			acc_top_blk_num    += blklogo_rg_cnt;
		}
		else
		{
			acc_bottom_blk_num += blklogo_rg_cnt;
		}
		if (idx%8 <4)
		{
			acc_left_blk_num  += blklogo_rg_cnt;
		}
		else
		{
			acc_right_blk_num += blklogo_rg_cnt;
		}
	}

	//rtd_pr_memc_notice( "[%s][%d][%d,%d,%d,%d,%d]\n",__FUNCTION__,pOutput->u1_logo_static_status, acc_blk_num,acc_left_blk_num,acc_right_blk_num,acc_top_blk_num,acc_bottom_blk_num);

#if 0
	if(((((acc_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN)) >= pParam->u8_clr_glb_thr) && pParam->u1_logo_glb_clr_en == 1)){
		rtd_pr_memc_emerg("1");
	}
	if(((((acc_left_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (pParam->u1_logo_left_half_clr_en  == 1))){
		rtd_pr_memc_emerg("2");
	}
	if(((((acc_right_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (pParam->u1_logo_right_half_clr_en  == 1))){
		rtd_pr_memc_emerg("3");
	}
	if(((((acc_top_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (pParam->u1_logo_top_half_clr_en  == 1))){
		rtd_pr_memc_emerg("4");
	}
	if(((((acc_bottom_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (pParam->u1_logo_bot_half_clr_en  == 1))){
		rtd_pr_memc_emerg("5");
	}
	if(pOutput->u1_logo_static_status != 0){
		rtd_pr_memc_emerg("6");
	}
	if(u11_bg_mv<=1){
		rtd_pr_memc_emerg("7");
	}

	rtd_pr_memc_emerg("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
	((acc_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN)),pParam->u8_clr_glb_thr
	,((acc_left_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)),pParam->u8_clr_half_thr
	,((acc_right_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)),pParam->u8_clr_half_thr
	,((acc_top_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)),pParam->u8_clr_half_thr
	,((acc_bottom_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)),pParam->u8_clr_half_thr);
#endif

#if RTK_MEMC_Performance_tunging_from_tv001
		if ((((((acc_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN)) >= pParam->u8_clr_glb_thr) && pParam->u1_logo_glb_clr_en == 1)
	|| ((((acc_left_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (pParam->u1_logo_left_half_clr_en  == 1))
	|| ((((acc_right_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (pParam->u1_logo_right_half_clr_en  == 1))
	|| ((((acc_top_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (pParam->u1_logo_top_half_clr_en  == 1))
	|| ((((acc_bottom_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (pParam->u1_logo_bot_half_clr_en  == 1)))
	&& ( pOutput->u1_logo_static_status != 0))
#else
	if ((((((acc_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN)) >= pParam->u8_clr_glb_thr) && pParam->u1_logo_glb_clr_en == 1)
		|| ((((acc_left_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (pParam->u1_logo_left_half_clr_en  == 1))
		|| ((((acc_right_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (pParam->u1_logo_right_half_clr_en  == 1))
		|| ((((acc_top_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (pParam->u1_logo_top_half_clr_en  == 1))
		|| ((((acc_bottom_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (pParam->u1_logo_bot_half_clr_en  == 1)))
		&& ( pOutput->u1_logo_static_status != 0 || u11_bg_mv<=1)) //pOutput->u1_logo_static_status != 0 is for #317(need keep logo), //u11_bg_mv<=1 is for 0011_HDHR(need clear logo)

#endif
	{
		pOutput->u32_blklogo_clr_idx    = /*pOutput->u32_blklogo_clr_idx |*/ 0xFFFFFFFF;
		pOutput->s16_blklogo_glbclr_cnt = pParam->u6_glbClr_holdtime;
		pOutput->u1_blkclr_glbstatus = 1;
		pOutput->u1_logo_cnt_glb_clr_status = 1;
		for(i=0;i<32;i++){
			pOutput->u8_dh_logo_temporal_counter[i] = 0;
		}
	}
	else if (pOutput->s16_blklogo_glbclr_cnt > 1 &&  pParam->u1_logo_glb_clr_en == 1)
	{

		pOutput->u32_blklogo_clr_idx    = /*pOutput->u32_blklogo_clr_idx |*/ 0xFFFFFFFF;
		pOutput->s16_blklogo_glbclr_cnt = _MAX_(pOutput->s16_blklogo_glbclr_cnt - 1,0);
		pOutput->u1_blkclr_glbstatus = 1;
		pOutput->u1_logo_cnt_glb_clr_status = 1;
		for(i=0;i<32;i++){
			pOutput->u8_dh_logo_temporal_counter[i] = 0;
		}
	}
	else
	{
		pOutput->u32_blklogo_clr_idx = 0;
		pOutput->s16_blklogo_glbclr_cnt = 0;
		pOutput->u1_blkclr_glbstatus = 0;
		pOutput->u1_logo_cnt_glb_clr_status = 0;
	}
}

//current, use blk_logo idx to clear SC logo (global all)
//default is logoClr=1, holdHsty=1, clrAlpha=1
VOID FRC_LgDet_SCCtrl(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned char i = 0;

	pOutput->u5_iir_alpha_out = pParam->u5_iir_alpha;
	if (s_pContext->_output_read_comreg.u1_sc_status_logo_rb == 1)
	{
		if (pOutput->u1_logo_netflix_status == 0 && pParam->u1_logo_sc_logo_clr_en == 1) //sc signal is consider holdtime.
		{
			pOutput->u32_blklogo_clr_idx = /*pOutput->u32_blklogo_clr_idx |*/ 0xFFFFFFFF;
			pOutput->u1_blkclr_glbstatus = 1;
			pOutput->u1_logo_sc_glb_clr_status = 1;
			for(i=0;i<32;i++){
				pOutput->u8_dh_logo_temporal_counter[i] = 0;
			}
		}
		if (pParam->u1_logo_sc_alpha_ctrl_en == 1)
		{
			pOutput->u5_iir_alpha_out = 0;
		}
	}
	else
	{	if (pParam->u1_logo_sc_alpha_ctrl_en == 1)
		{
			pOutput->u5_iir_alpha_out = pParam->u5_iir_alpha;
		}
	}
}

VOID FRC_LgDet_SC_FastDetectionCtrl(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	signed short u11_gmv_mvx = (s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb)>>1;
	signed short u10_gmv_mvy = (s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb)>>1;

	if(pParam->u1_logo_sc_FastDet_en == 1)
	{
		if (s_pContext->_output_read_comreg.u1_sc_status_logo_rb == 1)
		{			
			pOutput->u8_logo_sc_FastDet_cntholdfrm = pParam->u8_logo_sc_FastDet_rstholdfrm ;
		}
		else			
		{ 
			if(s_pContext->_output_me_sceneAnalysis.u2_panning_flag == 1 && ( _ABS_(u11_gmv_mvx)>10 || _ABS_(u10_gmv_mvy)>10 )){
				if(pOutput->u8_logo_sc_FastDet_cntholdfrm>2){
					pOutput->u8_logo_sc_FastDet_cntholdfrm = 5;
				}
				pOutput->u8_logo_sc_FastDet_cntholdfrm = (pOutput->u8_logo_sc_FastDet_cntholdfrm > 0)? pOutput->u8_logo_sc_FastDet_cntholdfrm - 1 : 0;
			}else{
				if(( _ABS_(u11_gmv_mvx)>10 || _ABS_(u10_gmv_mvy)>10 )){
					pOutput->u8_logo_sc_FastDet_cntholdfrm = (pOutput->u8_logo_sc_FastDet_cntholdfrm > 0)? pOutput->u8_logo_sc_FastDet_cntholdfrm - 1 : 0;
				}else{
					if(pOutput->u8_logo_sc_FastDet_cntholdfrm>7){
						pOutput->u8_logo_sc_FastDet_cntholdfrm = (pOutput->u8_logo_sc_FastDet_cntholdfrm > 0)? pOutput->u8_logo_sc_FastDet_cntholdfrm - 7 : 0;
					}else{
						pOutput->u8_logo_sc_FastDet_cntholdfrm = (pOutput->u8_logo_sc_FastDet_cntholdfrm > 0)? pOutput->u8_logo_sc_FastDet_cntholdfrm - 1 : 0;	
					}
				}
			}
		}

		if( pOutput->u8_logo_sc_FastDet_cntholdfrm > 0 )
			pOutput->u1_logo_sc_FastDet_status = 1;
		else
			pOutput->u1_logo_sc_FastDet_status = 0;
	}
	else
	{
		pOutput->u1_logo_sc_FastDet_status = 0;
		pOutput->u8_logo_sc_FastDet_cntholdfrm = 0;
	}
}

/// bai change
//VOID FRC_LgDet_RgDhClr(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
//{
//	const _PQLCONTEXT *s_pContext = GetPQLContext();
//	unsigned int rg_tl_blk_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[0] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[1];
//	unsigned int rg_tr_blk_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[6] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[7];
//	unsigned int rg_bl_blk_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[24] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[25];
//	unsigned int rg_br_blk_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[30] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[31];
//
//	unsigned int rg_tl_blk_logo_dh_clr_cnt = (rg_tl_blk_logo_cnt >= s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[0])?(rg_tl_blk_logo_cnt - s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[0]) : 0;
//	unsigned int rg_tr_blk_logo_dh_clr_cnt = (rg_tr_blk_logo_cnt >= s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[1])?(rg_tr_blk_logo_cnt - s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[1]) : 0;
//	unsigned int rg_bl_blk_logo_dh_clr_cnt = (rg_bl_blk_logo_cnt >= s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[2])?(rg_bl_blk_logo_cnt - s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[2]) : 0;
//	unsigned int rg_br_blk_logo_dh_clr_cnt = (rg_br_blk_logo_cnt >= s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[3])?(rg_br_blk_logo_cnt - s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[3]) : 0;
//
//	unsigned int rg_logo_diff_prm  = pParam->u16_RgDhClr_thr_h - pParam->u16_RgDhClr_thr_l;
//	unsigned int rg_logo_slope      = rg_logo_diff_prm <= 0 ? 0 : 16 * 255 / rg_logo_diff_prm;
//
//	unsigned char  rg_tl_blk_logo_alp = _IncreaseCurveMapping(rg_tl_blk_logo_dh_clr_cnt, pParam->u16_RgDhClr_thr_l, 0, 16, rg_logo_slope, 8);
//	unsigned char  rg_tr_blk_logo_alp = _IncreaseCurveMapping(rg_tr_blk_logo_dh_clr_cnt, pParam->u16_RgDhClr_thr_l, 0, 16, rg_logo_slope, 8);
//	unsigned char  rg_bl_blk_logo_alp = _IncreaseCurveMapping(rg_bl_blk_logo_dh_clr_cnt, pParam->u16_RgDhClr_thr_l, 0, 16, rg_logo_slope, 8);
//	unsigned char  rg_br_blk_logo_alp = _IncreaseCurveMapping(rg_br_blk_logo_dh_clr_cnt, pParam->u16_RgDhClr_thr_l, 0, 16, rg_logo_slope, 8);
//
//
//	if( pParam->u1_RgDhClr_thr_en == 1)
//	{
//		pOutput->u1_rg_dh_clr_rg0 = (rg_tl_blk_logo_alp>=pParam->u5_RgDhClr_cnt_th) ? 1:0;
//		pOutput->u1_rg_dh_clr_rg1 = (rg_tr_blk_logo_alp>=pParam->u5_RgDhClr_cnt_th) ? 1:0;
//		pOutput->u1_rg_dh_clr_rg2 = (rg_bl_blk_logo_alp>=pParam->u5_RgDhClr_cnt_th) ? 1:0;
//		pOutput->u1_rg_dh_clr_rg3 = (rg_br_blk_logo_alp>=pParam->u5_RgDhClr_cnt_th) ? 1:0;
//	}
//	else
//	{
//		pOutput->u1_rg_dh_clr_rg0 =  0;
//		pOutput->u1_rg_dh_clr_rg1 =  0;
//		pOutput->u1_rg_dh_clr_rg2 =  0;
//		pOutput->u1_rg_dh_clr_rg3 =  0;
//	}
//
//	if (pOutput->u32_blklogo_clr_idx == 0xFFFFFFFF)
//	{
//		pOutput->u1_rg_dh_clr_rg0 =  0;
//		pOutput->u1_rg_dh_clr_rg1 =  0;
//		pOutput->u1_rg_dh_clr_rg2 =  0;
//		pOutput->u1_rg_dh_clr_rg3 =  0;
//
//		pOutput->u8_rg_dh_clr_lgclr_hold_frm = pParam->u8_rg_dh_clr_lgclr_hold_frm;
//	}
//	else if (pOutput->u8_rg_dh_clr_lgclr_hold_frm > 0)
//	{
//		pOutput->u1_rg_dh_clr_rg0 =  0;
//		pOutput->u1_rg_dh_clr_rg1 =  0;
//		pOutput->u1_rg_dh_clr_rg2 =  0;
//		pOutput->u1_rg_dh_clr_rg3 =  0;
//		pOutput->u8_rg_dh_clr_lgclr_hold_frm = pOutput->u8_rg_dh_clr_lgclr_hold_frm - 1;
//	}
//	else
//	{
//		pOutput->u8_rg_dh_clr_lgclr_hold_frm = 0;
//	}
//
//    if (pParam->u1_RgDhClr_static_en == 1)
//    {
//		if (pOutput->u1_logo_static_status == 1)
//		{
//			pOutput->u1_rg_dh_clr_rg0 =  0;
//			pOutput->u1_rg_dh_clr_rg1 =  0;
//			pOutput->u1_rg_dh_clr_rg2 =  0;
//			pOutput->u1_rg_dh_clr_rg3 =  0;
//			pOutput->u8_rg_dh_clr_static_hold_frm = pParam->u8_rg_dh_clr_static_hold_frm;
//		}
//		else if (pOutput->u8_rg_dh_clr_static_hold_frm > 0)
//		{
//			pOutput->u1_rg_dh_clr_rg0 =  0;
//			pOutput->u1_rg_dh_clr_rg1 =  0;
//			pOutput->u1_rg_dh_clr_rg2 =  0;
//			pOutput->u1_rg_dh_clr_rg3 =  0;
//			pOutput->u8_rg_dh_clr_static_hold_frm = pOutput->u8_rg_dh_clr_static_hold_frm - 1;
//		}
//		else
//		{
//			pOutput->u8_rg_dh_clr_static_hold_frm = 0;
//		}
//    }
//}

VOID FRC_LgDet_RgDhClr(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int u16_tl_dh_clrcnt;
	unsigned int u16_tr_dh_clrcnt;
	unsigned int u16_bl_dh_clrcnt;
	unsigned int u16_br_dh_clrcnt;

	unsigned int rg_tl_blk_logo_dh_clr_cnt;
	unsigned int rg_tr_blk_logo_dh_clr_cnt;
	unsigned int rg_bl_blk_logo_dh_clr_cnt;
	unsigned int rg_br_blk_logo_dh_clr_cnt;

	unsigned int rg_tl_blk_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[0] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[1];
	unsigned int rg_tr_blk_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[6] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[7];
	unsigned int rg_bl_blk_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[24] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[25];
	unsigned int rg_br_blk_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[30] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[31];

	if (s_pContext->_output_wrt_comreg.u1_LG_mb_wrt_en == 1)
	{
		u16_tl_dh_clrcnt = s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[0] - s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt_top[2];
		u16_tr_dh_clrcnt = s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[1] - s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt_top[2];
		u16_bl_dh_clrcnt = s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[2] - s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt_bot[2];
		u16_br_dh_clrcnt = s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[3] - s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt_bot[2];
	}
	else
	{
		u16_tl_dh_clrcnt = 	s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[0];
		u16_tr_dh_clrcnt = 	s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[1];
		u16_bl_dh_clrcnt = 	s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[2];
		u16_br_dh_clrcnt = 	s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt[3];
	}

	rg_tl_blk_logo_dh_clr_cnt = (rg_tl_blk_logo_cnt >= u16_tl_dh_clrcnt)?(rg_tl_blk_logo_cnt - u16_tl_dh_clrcnt) : 0;
	rg_tr_blk_logo_dh_clr_cnt = (rg_tr_blk_logo_cnt >= u16_tr_dh_clrcnt)?(rg_tr_blk_logo_cnt - u16_tr_dh_clrcnt) : 0;
	rg_bl_blk_logo_dh_clr_cnt = (rg_bl_blk_logo_cnt >= u16_bl_dh_clrcnt)?(rg_bl_blk_logo_cnt - u16_bl_dh_clrcnt) : 0;
	rg_br_blk_logo_dh_clr_cnt = (rg_br_blk_logo_cnt >= u16_br_dh_clrcnt)?(rg_br_blk_logo_cnt - u16_br_dh_clrcnt) : 0;


	if( pParam->u1_RgDhClr_thr_en == 1)
	{
		pOutput->u1_rg_dh_clr_rg0 = (rg_tl_blk_logo_dh_clr_cnt>=pParam->u16_RgDhClr_thr_l && rg_tl_blk_logo_dh_clr_cnt<=pParam->u16_RgDhClr_thr_h) ? 1:0;
		pOutput->u1_rg_dh_clr_rg1 = (rg_tr_blk_logo_dh_clr_cnt>=pParam->u16_RgDhClr_thr_l && rg_tl_blk_logo_dh_clr_cnt<=pParam->u16_RgDhClr_thr_h) ? 1:0;
		pOutput->u1_rg_dh_clr_rg2 = (rg_bl_blk_logo_dh_clr_cnt>=pParam->u16_RgDhClr_thr_l && rg_tl_blk_logo_dh_clr_cnt<=pParam->u16_RgDhClr_thr_h) ? 1:0;
		pOutput->u1_rg_dh_clr_rg3 = (rg_br_blk_logo_dh_clr_cnt>=pParam->u16_RgDhClr_thr_l && rg_tl_blk_logo_dh_clr_cnt<=pParam->u16_RgDhClr_thr_h) ? 1:0;
	}
	else
	{
		pOutput->u1_rg_dh_clr_rg0 =  0;
		pOutput->u1_rg_dh_clr_rg1 =  0;
		pOutput->u1_rg_dh_clr_rg2 =  0;
		pOutput->u1_rg_dh_clr_rg3 =  0;
	}

	if (pOutput->u32_blklogo_clr_idx == 0xFFFFFFFF)
	{
		pOutput->u1_rg_dh_clr_rg0 =  0;
		pOutput->u1_rg_dh_clr_rg1 =  0;
		pOutput->u1_rg_dh_clr_rg2 =  0;
		pOutput->u1_rg_dh_clr_rg3 =  0;

		pOutput->u8_rg_dh_clr_lgclr_hold_frm = pParam->u8_rg_dh_clr_lgclr_hold_frm;
	}
	else if (pOutput->u8_rg_dh_clr_lgclr_hold_frm > 0)
	{
		pOutput->u1_rg_dh_clr_rg0 =  0;
		pOutput->u1_rg_dh_clr_rg1 =  0;
		pOutput->u1_rg_dh_clr_rg2 =  0;
		pOutput->u1_rg_dh_clr_rg3 =  0;
		pOutput->u8_rg_dh_clr_lgclr_hold_frm = pOutput->u8_rg_dh_clr_lgclr_hold_frm - 1;
	}
	else
	{
		pOutput->u8_rg_dh_clr_lgclr_hold_frm = 0;
	}

    if (pParam->u1_RgDhClr_static_en == 1)
    {
		if (pOutput->u1_logo_static_status == 1)
		{
			pOutput->u1_rg_dh_clr_rg0 =  0;
			pOutput->u1_rg_dh_clr_rg1 =  0;
			pOutput->u1_rg_dh_clr_rg2 =  0;
			pOutput->u1_rg_dh_clr_rg3 =  0;
			pOutput->u8_rg_dh_clr_static_hold_frm = pParam->u8_rg_dh_clr_static_hold_frm;
		}
		else if (pOutput->u8_rg_dh_clr_static_hold_frm > 0)
		{
			pOutput->u1_rg_dh_clr_rg0 =  0;
			pOutput->u1_rg_dh_clr_rg1 =  0;
			pOutput->u1_rg_dh_clr_rg2 =  0;
			pOutput->u1_rg_dh_clr_rg3 =  0;
			pOutput->u8_rg_dh_clr_static_hold_frm = pOutput->u8_rg_dh_clr_static_hold_frm - 1;
		}
		else
		{
			pOutput->u8_rg_dh_clr_static_hold_frm = 0;
		}
    }
}
VOID FRC_LgDet_RgHstyClr(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
#if 1
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	//unsigned int u16_cnt_totalNum1 = (s_pContext->_output_read_comreg.u8_me1_Vbuf_Hsize_rb * s_pContext->_output_read_comreg.u8_me1_Vbuf_Vsize_rb);

	unsigned char u1_sc_stat = s_pContext->_output_read_comreg.u1_sc_status_logo_rb;
	unsigned int u11_Gmv_mvx = _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb);
	unsigned int u10_Gmv_mvy = _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb);

	//horizatal_speed = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	//vertical_speed  = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
#if 0
	rtd_pr_memc_notice( "[logo][GMV]->%d,%d,%d,%d\r\n",u11_Gmv_mvx,u10_Gmv_mvy,s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb,s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb);
	rtd_pr_memc_notice( "[logo][GMV]sad->%d,%d\r\n",s_pContext->_output_read_comreg.u30_me_aSAD_rb,ME1_TotalBlkNum_Golden);
#endif	


	//rtd_pr_memc_notice( "[%s][%d,%d,%d,%d]\n",__FUNCTION__,pParam->u1_GlbHstyClr_en, pParam->u1_RgHstyClr_Sc_en,u1_sc_stat,pParam->u1_RgHstyClr_Sc_en);

	if (pParam->u1_GlbHstyClr_en == 1 && ((pParam->u1_RgHstyClr_Sc_en == 1 && u1_sc_stat == 1) || (pParam->u1_RgHstyClr_Sc_en == 0)))
	{
		//rtd_pr_memc_notice( "[%s][%d,%d,%d,%d]\n",__FUNCTION__,(u11_Gmv_mvx + u10_Gmv_mvy), s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb,s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb,s_pContext->_output_read_comreg.u30_me_aSAD_rb);
		
		if ((u11_Gmv_mvx + u10_Gmv_mvy) < pParam->u16_RgHstyClr_Gmv_thr &&
			s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb > pParam->u8_RgHstyClr_Gmv_cnt_thr &&
			s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb < pParam->u12_RgHstyClr_Gmv_unconf_thr &&
			s_pContext->_output_read_comreg.u30_me_aSAD_rb < pParam->u10_RgHstyClr_Gsad_thr*ME1_TotalBlkNum_Golden)
		{
			pOutput->u32_logo_hsty_clr_idx = /*pOutput->u32_logo_hsty_clr_idx |*/ 0xFFFFFFFF;
			pOutput->u1_logo_static_status = 1;
		}
		else
		{
			pOutput->u1_logo_static_status = 0;
		}

	}
	#if 0
	rtd_pr_memc_notice( "[logo][GMV]static_status->%d\r\n",pOutput->u1_logo_static_status);
	#endif
#else
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int idx;
	//unsigned int u16_cnt_totalNum1 = (s_pContext->_output_read_comreg.u8_me1_Vbuf_Hsize_rb * s_pContext->_output_read_comreg.u8_me1_Vbuf_Vsize_rb);
	unsigned int u16_cnt_totalNum1 = ME1_TotalBlkNum_Golden;
	unsigned char u1_sc_stat = s_pContext->_output_read_comreg.u1_sc_status_logo_rb;
	unsigned int u11_Gmv_mvx = _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb);
	unsigned int u10_Gmv_mvy = _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb);
	unsigned int u11_Rgmv_mvx;
	unsigned int u10_Rgmv_mvy;
	unsigned int u11_Rgmv2_mvx;
	unsigned int u10_Rgmv2_mvy;
	unsigned int u32_subtitle_rgn = 0;
	unsigned int u32_Rg_static_idx = 0;
	unsigned int u32_Rg_unstatic_idx = 0;
	unsigned char  u8_Rg_static_num = 0;
	if (pParam->u1_GlbHstyClr_en == 1 && ((pParam->u1_RgHstyClr_Sc_en == 1 && u1_sc_stat == 1) || (pParam->u1_RgHstyClr_Sc_en == 0)))
	{
		if ((u11_Gmv_mvx + u10_Gmv_mvy) < pParam->u16_RgHstyClr_Gmv_thr &&
			s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb > pParam->u8_RgHstyClr_Gmv_cnt_thr &&
			s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb < pParam->u12_RgHstyClr_Gmv_unconf_thr &&
			s_pContext->_output_read_comreg.u30_me_aSAD_rb/u16_cnt_totalNum1 < pParam->u10_RgHstyClr_Gsad_thr)
		{
			pOutput->u32_logo_hsty_clr_idx = /*pOutput->u32_logo_hsty_clr_idx |*/ 0xFFFFFFFF;
			pOutput->u1_logo_static_status = 1;
		}
		else
		{
			pOutput->u1_logo_static_status = 0;
		}

	}

	for (idx = 0; idx < LOGO_RG_32; idx++)
	{
		u11_Rgmv_mvx = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[idx]);
		u10_Rgmv_mvy = _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[idx]);
		if ((u11_Rgmv_mvx + u10_Rgmv_mvy) < pParam->u16_RgHstyClr_mv_thr &&
			s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[idx] > pParam->u12_RgHstyClr_mv_cnt_thr &&
			s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[idx] < pParam->u12_RgHstyClr_mv_unconf_thr &&
			s_pContext->_output_read_comreg.u25_me_rSAD_rb[idx]/u16_cnt_totalNum < pParam->u10_RgHstyClr_Rsad_thr)
		{
			u32_Rg_static_idx = u32_Rg_static_idx | (1 << idx);
			u8_Rg_static_num = u8_Rg_static_num + 1;
		}
		else
		{
			u32_Rg_unstatic_idx = u32_Rg_unstatic_idx| (1 << idx);
		}

		//special for rgn26/27/28/29 - maybe subtitle.
		{
			unsigned char u1_subtitle_bmv_noclr_en = 1;
			u11_Rgmv2_mvx = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[idx]);
			u10_Rgmv2_mvy = _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[idx]);
			if(idx >= 26 && idx <=29 && u1_subtitle_bmv_noclr_en)
			{
				if((u11_Rgmv_mvx + u10_Rgmv_mvy) > 30 ||(u11_Rgmv2_mvx + u10_Rgmv2_mvy) > 30
				)
				{
					u32_subtitle_rgn = u32_subtitle_rgn | (1 << idx);
				}

			}
		}
	}


	if (pParam->u1_logo_static_bg_en == 1)
	{
		u32_Rg_unstatic_idx = u32_Rg_unstatic_idx & 0x3CFFFF3C & (~u32_subtitle_rgn);
		pOutput->u8_Rg_static_num  = u8_Rg_static_num;
		pOutput->u32_Rg_unstatic_idx = ((u8_Rg_static_num > pParam->u6_logo_static_bg_th_l) && (u8_Rg_static_num < pParam->u6_logo_static_bg_th_h)) ? u32_Rg_unstatic_idx : 0;

	}else
	{
		pOutput->u8_Rg_static_num  = 0;
		pOutput->u32_Rg_unstatic_idx = 0;
	}

	if (pParam->u1_RgHstyClr_en == 1 && ((pParam->u1_RgHstyClr_Sc_en == 1 && u1_sc_stat == 1) || (pParam->u1_RgHstyClr_Sc_en == 0)))
	{
		for (idx = 0; idx < LOGO_RG_32; idx++)
		{
			u11_Rgmv_mvx = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[idx]);
			u10_Rgmv_mvy = _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[idx]);
			if ((u11_Rgmv_mvx + u10_Rgmv_mvy) < pParam->u16_RgHstyClr_mv_thr &&
				s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[idx] > pParam->u12_RgHstyClr_mv_cnt_thr &&
				s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[idx] < pParam->u12_RgHstyClr_mv_unconf_thr &&
				s_pContext->_output_read_comreg.u25_me_rSAD_rb[idx]/u16_cnt_totalNum < pParam->u10_RgHstyClr_Rsad_thr)
			{
				pOutput->u32_logo_hsty_clr_idx = pOutput->u32_logo_hsty_clr_idx | (1 << idx);
			}
		}
	}
	if (pParam->u1_RgHstyClr_glbclr_close_en == 1 && pOutput->u1_logo_sc_glb_clr_status == 0 && pOutput->u1_logo_cnt_glb_clr_status == 1)
	{
		pOutput->u32_logo_hsty_clr_idx = 0;
	}
#endif
	pOutput->u32_logo_hsty_clr_lgclr_idx = pOutput->u32_logo_hsty_clr_idx;
}

VOID FRC_LgDet_SC_Detect(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	unsigned int idx;
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	if (pParam->u1_logo_sc_detect_en)
	{

		unsigned char  u1_logo_sc_status;
		unsigned char  u1_logo_sc_dtldiff_status;
		unsigned int sad_diff = _MAX_(pOutput->u31_logo_sad_pre,s_pContext->_output_read_comreg.u30_me_aSAD_rb)-_MIN_(pOutput->u31_logo_sad_pre , s_pContext->_output_read_comreg.u30_me_aSAD_rb);
		unsigned char  u1_logo_sc_saddif_status = sad_diff > pParam->u31_logo_sc_saddiff_th ? 1 : 0;
		unsigned char  u1_logo_sc_Gmv_status;
		unsigned int reg_dtl_diff_sum = 0;

		if ((s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb < pParam->u6_logo_sc_Gmv_cnt_th) && (s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb >= pParam->u12_logo_sc_Gmv_unconf_th))
		{
			u1_logo_sc_Gmv_status = 1;
		}
		else
		{
			u1_logo_sc_Gmv_status = 0;
		}
		for (idx = 0; idx < LOGO_RG_32; idx++)
		{
			reg_dtl_diff_sum = reg_dtl_diff_sum + (_MAX_(s_pContext->_output_read_comreg.u20_me_rDTL_rb[idx],pOutput->u20_logo_dtl_pre[idx])- _MIN_(s_pContext->_output_read_comreg.u20_me_rDTL_rb[idx],pOutput->u20_logo_dtl_pre[idx]));
		}
		reg_dtl_diff_sum = reg_dtl_diff_sum >> 5;
		u1_logo_sc_dtldiff_status = reg_dtl_diff_sum > pParam->u20_logo_sc_dtldif_th ? 1 : 0;

		pOutput->u1_logo_sc_saddif_status = u1_logo_sc_saddif_status;
		pOutput->u1_logo_sc_Gmv_status    = u1_logo_sc_Gmv_status;
		pOutput->u1_logo_sc_dtldif_status = u1_logo_sc_dtldiff_status;

		if (pParam->u1_logo_sc_Gmv_en == 1)
		{
			u1_logo_sc_status = ((u1_logo_sc_saddif_status && pParam->u1_logo_sc_saddif_en) ||
				                 (u1_logo_sc_dtldiff_status && pParam->u1_logo_sc_dtldif_en)) && u1_logo_sc_Gmv_status;
		}
		else if (pParam->u1_logo_sc_saddif_en==1 && pParam->u1_logo_sc_dtldif_en ==1)
		{
			u1_logo_sc_status = u1_logo_sc_saddif_status && u1_logo_sc_dtldiff_status;
		}
		else
		{
			u1_logo_sc_status = (u1_logo_sc_saddif_status && pParam->u1_logo_sc_saddif_en) ||
				                (u1_logo_sc_dtldiff_status && pParam->u1_logo_sc_dtldif_en) ;
		}

		if (u1_logo_sc_status == 1)
		{
			pOutput->u1_logo_sc_status = 1;
			pOutput->u8_logo_sc_hold_frm = pParam->u8_logo_sc_hold_frm;
		}
		else if (pOutput->u8_logo_sc_hold_frm > 0)
		{
			pOutput->u1_logo_sc_status = 1;
			pOutput->u8_logo_sc_hold_frm = pOutput->u8_logo_sc_hold_frm - 1;
		}
		else
		{
			pOutput->u1_logo_sc_status = 0;
		}
	}
	else
	{
		pOutput->u1_logo_sc_status = 0;

	}
	// update  pre
	pOutput->u31_logo_sad_pre = s_pContext->_output_read_comreg.u30_me_aSAD_rb;
	for (idx = 0; idx < LOGO_RG_32; idx++)
	{
		pOutput->u20_logo_dtl_pre[idx] = s_pContext->_output_read_comreg.u20_me_rDTL_rb[idx];
	}
}

VOID FRC_LgDet_pure_panning_detect(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	PQL_OUTPUT_RESOLUTION output_Resolution = s_pContext->_external_data._output_resolution;
	short u11_gmv_mvx = (s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb);
	short u10_gmv_mvy = (s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb);
	unsigned int u32_gmv_cnt = s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb;
	short u11_2nd_gmv_mvx = (s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb);
	short u10_2nd_gmv_mvy = (s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb);
	unsigned int u32_2nd_gmv_cnt = s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb;
	unsigned int i=0, cnt = 0, cnt1 = 0, cnt2 = 0;
	unsigned int u32_RB_val;
	short  u10_rmv_mvy[32];
	short  u11_rmv_mvx[32];
	unsigned short   rmv_rcnt_rb[32];
	unsigned char  Pure_panning_status = 0;
	static unsigned int fcnt =0;
	unsigned short Boundary_check[2];//0:Top, 1:Bot
	unsigned short Boundary_check_th[2];
	unsigned char  Full_Boundary_check= 1;
	unsigned short   rmv_check_status[3][32]={0};
	static short u11_gmv_mvx_pre = 0;
	static short u10_gmv_mvy_pre = 0;
	static unsigned char Pure_panning_2_cnt = 0;
	static unsigned char tmp_Pure_panning_status = 0;

	ReadRegister(HARDWARE_HARDWARE_58_reg, 0, 31, &u32_RB_val);//bit 8
	Boundary_check[0] = s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP];
	Boundary_check[1] = s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_BOT];
	Boundary_check_th[0] = (MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.s16_resolution_height*209)>>11; //height:2160 -> th0:220
	Boundary_check_th[1] = (MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.s16_resolution_height*1830)>>11; //height:2160 -> th1:1930

	if(Boundary_check[0]>Boundary_check_th[0] && Boundary_check[1]<=Boundary_check_th[1]){
		Full_Boundary_check = 0;
	}else{
		Full_Boundary_check = 1;
	}

	for(i=0;i<32;i++)
	{
		u11_rmv_mvx[i]=s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i];
		u10_rmv_mvy[i]=s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i];
		rmv_rcnt_rb[i]=s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[i];		

		if(_ABS_DIFF_(u11_rmv_mvx[i],u11_gmv_mvx)<=1 && _ABS_DIFF_(u10_rmv_mvy[i],u10_gmv_mvy)<=1 && rmv_rcnt_rb[i]>=800){
			rmv_check_status[0][i]=1;
			cnt++;
		}else{
			rmv_check_status[0][i]=0;
		}

		if(_ABS_DIFF_(u11_rmv_mvx[i],u11_gmv_mvx)<10 && _ABS_DIFF_(u10_rmv_mvy[i],u10_gmv_mvy)<=3 && rmv_rcnt_rb[i]>=500){
			rmv_check_status[1][i]=1;
			cnt1++;//rmv_mvy_diff small 
		}else{
			rmv_check_status[1][i]=0;
		}
		
		if(_ABS_DIFF_(u11_rmv_mvx[i],u11_gmv_mvx)<3 && _ABS_DIFF_(u10_rmv_mvy[i],u10_gmv_mvy)<=10 && rmv_rcnt_rb[i]>=500){
			rmv_check_status[2][i]=1;
			cnt2++;//rmv_mvx_diff small
		}else{
			rmv_check_status[2][i]=0;
		}
	 }

	if ( ((u32_RB_val >>  8) & 0x01) ==1 ){
		for(i=0;i<32;i++)
		{
			//rtd_pr_memc_emerg("[%s][%d][%d][,%d,%d,%d,]\n",__FUNCTION__, __LINE__,i,rmv_check_status[0][i],rmv_check_status[1][i],rmv_check_status[2][i]);
		}
	}

	if((s_pContext->_output_me_sceneAnalysis.u2_panning_flag == 1)&&(u32_gmv_cnt>30000)&&(u11_gmv_mvx>=5 && _ABS_(u10_gmv_mvy)<=1) && (cnt>=30)){
		Pure_panning_status = 1;// panning mode 1 (slow V) 	
	}else if((s_pContext->_output_me_sceneAnalysis.u2_panning_flag == 1)&&(u32_gmv_cnt>20000)&&(_ABS_(u11_gmv_mvx)>=5 && _ABS_(u10_gmv_mvy)<=5) && (_ABS_(u11_2nd_gmv_mvx)<=5 && _ABS_(u10_2nd_gmv_mvy)<=10 && u32_2nd_gmv_cnt<=400) && (cnt1>=25)){
			if(((rmv_check_status[0][26]==0 && rmv_check_status[0][27]==0 && rmv_check_status[0][28]==0 && rmv_check_status[0][29]==0) && (cnt>=28))
				&& ((rmv_check_status[0][26]==0 && rmv_check_status[0][27]==0 && rmv_check_status[0][28]==0) && (cnt>=29))){
				Pure_panning_status = 9;//panning mode 4 (with logo)	
			}else if(((_ABS_(u11_gmv_mvx)>=25 && _ABS_(u10_gmv_mvy)<=1) || 
				      (_ABS_(u10_gmv_mvy)<=1 && cnt2<15 && _ABS_(u11_2nd_gmv_mvx)<=1 && _ABS_(u10_2nd_gmv_mvy)<=1 )) && 
				      (u32_2nd_gmv_cnt<=200)){
				Pure_panning_status = 7;//slow panning mode mix
			}else{	
				//K24987, logo error issue : #42, #182, #184,  #192
				Pure_panning_2_cnt = Pure_panning_2_cnt + 1;
				tmp_Pure_panning_status = 2;
			}
	}else if((u32_gmv_cnt>25000)&&(u10_gmv_mvy>=70 || u10_gmv_mvy<=-50)&& (u32_2nd_gmv_cnt<=500) && (cnt1>=26)){
		Pure_panning_status = 3;//fast V panning 
	}else if((u32_gmv_cnt>25000)&&(u11_gmv_mvx>=70 || u11_gmv_mvx<=-50)&& (u32_2nd_gmv_cnt<=500) && (cnt2>=26)){
		Pure_panning_status = 4;//fast H panning
	}else if((s_pContext->_output_me_sceneAnalysis.u2_panning_flag == 1)&&(u32_gmv_cnt>22000)&&(_ABS_(u11_gmv_mvx)<=35 && _ABS_(u10_gmv_mvy)<=5) && (_ABS_(u11_2nd_gmv_mvx)<=5 && _ABS_(u10_2nd_gmv_mvy)<=10 && u32_2nd_gmv_cnt<=100) && (cnt2>=25)){
		Pure_panning_status = 5;//slow H panning mode
	}else if((u32_gmv_cnt>20000)&&(_ABS_(u11_gmv_mvx)>=10 && _ABS_(u11_gmv_mvx)<=60 && _ABS_(u10_gmv_mvy)<=80) &&(Full_Boundary_check==0)){
		Pure_panning_status = 6;//slow panning mode mix 	
	}else if((u32_gmv_cnt>22000)&&(_ABS_(u11_gmv_mvx)<=10 && _ABS_(u10_gmv_mvy)<=10) 
				&& ((_ABS_(u11_gmv_mvx)<=15 && _ABS_(u10_2nd_gmv_mvy)>=40 && _ABS_(u10_2nd_gmv_mvy)<=80 && u32_2nd_gmv_cnt<=450) || (_ABS_(u10_2nd_gmv_mvy)<=20 && u32_2nd_gmv_cnt<=400))
				&& (cnt1>=22 || cnt2>=22)){
				Pure_panning_status = 7;//slow panning mode mix 	
	}else if((s_pContext->_output_me_sceneAnalysis.u2_panning_flag == 1)&&(u32_gmv_cnt>20000)&& (_ABS_(u11_gmv_mvx)>=5 || _ABS_(u10_gmv_mvy)>=5) 
			&& (_ABS_(u11_2nd_gmv_mvx)<=5 && _ABS_(u10_2nd_gmv_mvy)<=10 && u32_2nd_gmv_cnt<=1600) && (cnt>=25)){
			if((cnt>=30)){
				Pure_panning_status = 8;//panning mode 3
			}else if((rmv_check_status[0][26]==0 && rmv_check_status[0][27]==0 && rmv_check_status[0][28]==0 && rmv_check_status[0][29]==0) && (cnt>=28)){
				Pure_panning_status = 9;//panning mode 4 (with logo)
			}else{
				Pure_panning_status = 0;
			}			
	}else if((s_pContext->_output_me_sceneAnalysis.u2_panning_flag == 1)&&(u32_gmv_cnt>18000)&&(_ABS_(u11_gmv_mvx)>=40 && _ABS_(u11_gmv_mvx)<=60 && _ABS_(u10_gmv_mvy)<=15) 
				&& ((_ABS_(u11_2nd_gmv_mvx)<=8 && _ABS_(u10_2nd_gmv_mvy)<=8 && u32_2nd_gmv_cnt<=400)||(_ABS_(u11_2nd_gmv_mvx)<=2 && _ABS_(u10_2nd_gmv_mvy)<=2 && u32_2nd_gmv_cnt<=1200)) && (cnt1>=20 && cnt2>=18 )){
			Pure_panning_status = 10;
	}else{		
		Pure_panning_status = 0;
	}
#if 1
	//K24987, logo error issue : #42, #182, #184,  #192
	if(Pure_panning_2_cnt>=10 && tmp_Pure_panning_status==2){		
		Pure_panning_2_cnt = _MIN_(Pure_panning_2_cnt, 20);
		Pure_panning_status = tmp_Pure_panning_status;		
	}else{
		Pure_panning_2_cnt = 0;
		tmp_Pure_panning_status = Pure_panning_status;
	}
#endif
	if (Pure_panning_status != 0)
	{	
		pOutput->u1_logo_PurePanning_status = Pure_panning_status;
		pOutput->u8_logo_PurePanning_hold_frm = 20;//same as netflix_hold_frm		
	}
	else if (pOutput->u8_logo_PurePanning_hold_frm > 0)
	{		
		pOutput->u8_logo_PurePanning_hold_frm = pOutput->u8_logo_PurePanning_hold_frm - 1;
	}
	else
	{
		pOutput->u1_logo_PurePanning_status = 0;
		pOutput->u8_logo_PurePanning_hold_frm = 0;
	}
	

	if ( ((u32_RB_val >>  8) & 0x01) ==1 ){
		if( fcnt % 10 == 0 ){
			rtd_pr_memc_emerg("[%s][%d][,%d,%d][,%d,%d,%d,][,%d,%d,%d,][,%d,%d,%d,][,%d,][,%d,][,%d,%d,]\n",__FUNCTION__, __LINE__,pOutput->u1_logo_PurePanning_status,s_pContext->_output_me_sceneAnalysis.u2_panning_flag, 
				cnt,cnt1,cnt2,u11_gmv_mvx,u10_gmv_mvy,u32_gmv_cnt,u11_2nd_gmv_mvx,u10_2nd_gmv_mvy,u32_2nd_gmv_cnt, pOutput->u8_logo_PurePanning_hold_frm
				,s_pContext->_output_me_sceneAnalysis.u1_RP_detect_true,Pure_panning_status,Pure_panning_2_cnt);
		}
	}

	if(fcnt<=0xffff){
		fcnt++;	
	}
else{
		fcnt=0;
	}

	u11_gmv_mvx_pre = u11_gmv_mvx;
	u10_gmv_mvy_pre = u10_gmv_mvy;
	
}



VOID FRC_LgDet_BG_still_detect(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	short u11_gmv_mvx = (s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb)>>1;
	short u10_gmv_mvy = (s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb)>>1;
	short  u10_rmv_mvy[32];
	short  u11_rmv_mvx[32];
	unsigned int i=0, cnt = 0, cnt1 = 0;
	unsigned int u32_RB_val;
	static unsigned int fcnt =0;
	//unsigned int rPRD_rb_sum=0;
	unsigned int lt_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[0] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[1];
	unsigned int rt_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[6] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[7];
	unsigned int lb_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[24];
	unsigned int rb_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[31];
	unsigned char  BG_still_status = 0;

	ReadRegister(HARDWARE_HARDWARE_58_reg, 0, 31, &u32_RB_val);//bit 13
	
	for(i=0;i<32;i++)
	{
		u11_rmv_mvx[i]=s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i];
		u10_rmv_mvy[i]=s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i];
		//u13_me_rPRD_rb[i]= s_pContext->_output_read_comreg.u13_me_rPRD_rb[i];
		if(_ABS_(u11_rmv_mvx[i])<4 && _ABS_(u10_rmv_mvy[i])<=4){
			cnt++;
		}

#if RTK_MEMC_Performance_tunging_from_tv001
		if(_ABS_(u11_rmv_mvx[i]-u11_gmv_mvx*2)<4 && _ABS_(u10_rmv_mvy[i]-u10_gmv_mvy*2)<=4)
#else
		if(_ABS_DIFF_(u11_rmv_mvx[i],u11_gmv_mvx*2)<4 && _ABS_DIFF_(u10_rmv_mvy[i],u10_gmv_mvy*2)<=4)
#endif
		{
			cnt1++;
		}
	 }
	

	if(((lt_logo_cnt<30)&&(rt_logo_cnt)<80) &&(lb_logo_cnt<100)&&(rb_logo_cnt<200)
		&& ((((cnt>=25) || (cnt1>=25)) && (u11_gmv_mvx <=6 && u10_gmv_mvy <= 2)) || ((u11_gmv_mvx <=40 && u10_gmv_mvy <= 2)&&(s_pContext->_output_me_sceneAnalysis.u2_panning_flag ==1)))){
		BG_still_status = 1;
	}else{
		BG_still_status = 0;
	}	
	
	if ( (fcnt % 10 == 0  && ((u32_RB_val >>  13) & 0x01) ==1 )){
		//rtd_pr_memc_emerg("[%s][%d][,%d,%d,%d,][,%d,%d,][,%d,%d,%d,]\n",__FUNCTION__, __LINE__,BG_still_status, cnt,cnt1,u11_gmv_mvx,u10_gmv_mvy,(lt_logo_cnt+rt_logo_cnt),lb_logo_cnt,rb_logo_cnt);
	}

	if (BG_still_status == 1)
	{
		pOutput->u1_logo_BG_still_status = 1;
		pOutput->u8_logo_BG_still_hold_frm = 15;//same as netflix_hold_frm
	}
	else if (pOutput->u8_logo_BG_still_hold_frm > 0)
	{
		pOutput->u1_logo_BG_still_status = 1;
		pOutput->u8_logo_BG_still_hold_frm = pOutput->u8_logo_BG_still_hold_frm - 1;
	}
	else
	{
		pOutput->u1_logo_BG_still_status = 0;
		pOutput->u8_logo_BG_still_hold_frm = 0;
	}

	if(fcnt<=0xffff){
		fcnt++;	
	}else{
		fcnt=0;
	}
	
}

VOID FRC_LgDet_KeyRgn_detect(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int lt_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[0] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[1];
	unsigned int rt_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[6] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[7];
	unsigned int cb_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[27] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[28];
	unsigned int lb_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[24];
	unsigned int rb_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[31];
	unsigned char  BG_KeyRgn_status = 0;


	if((lt_logo_cnt>150)||(rt_logo_cnt>150)||(cb_logo_cnt>500)||(lb_logo_cnt>100)||(rb_logo_cnt>100)){
		BG_KeyRgn_status = 1;
	}else{
		BG_KeyRgn_status = 0;
	}


	if (BG_KeyRgn_status == 1)
	{
		pOutput->u1_logo_KeyRgn_status = 1;
		pOutput->u8_logo_KeyRgn_hold_frm = 15;//same as netflix_hold_frm
	}
	else if (pOutput->u8_logo_KeyRgn_hold_frm > 0)
	{
		pOutput->u1_logo_KeyRgn_status = 1;
		pOutput->u8_logo_KeyRgn_hold_frm = pOutput->u8_logo_KeyRgn_hold_frm - 1;
	}
	else
	{
		pOutput->u1_logo_KeyRgn_status = 0;
		pOutput->u8_logo_KeyRgn_hold_frm = 0;
	}

	
}

VOID FRC_LgDet_netflix_detect(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int lt_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[0] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[1];
	unsigned int rt_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[6] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[7];
	unsigned int lb_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[24];
	unsigned int rb_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[31];
	unsigned char  netflix_status = 0;

	if (pParam->u1_logo_netflix_detct_en == 1)
	{
		if (lt_logo_cnt >= pParam->u20_logo_netflix_detect_lt_th_l && lt_logo_cnt <= pParam->u20_logo_netflix_detect_lt_th_h &&
			rt_logo_cnt >= pParam->u20_logo_netflix_detect_rt_th_l && rt_logo_cnt <= pParam->u20_logo_netflix_detect_rt_th_h &&
			lb_logo_cnt >= pParam->u20_logo_netflix_detect_lb_th_l && lb_logo_cnt <= pParam->u20_logo_netflix_detect_lb_th_h &&
			rb_logo_cnt >= pParam->u20_logo_netflix_detect_rb_th_l && rb_logo_cnt <= pParam->u20_logo_netflix_detect_rb_th_h &&
			pOutput->u1_logo_cnt_glb_clr_status == 0)
		{
			netflix_status = 1;
		}
		else
		{
			netflix_status = 0;
		}
	}
	else
	{
		netflix_status = 0;
	}


	if (netflix_status == 1)
	{
		pOutput->u1_logo_netflix_status = 1;
		pOutput->u8_logo_netflix_hold_frm = pParam->u8_logo_netflix_hold_frm;
	}
	else if (pOutput->u8_logo_netflix_hold_frm > 0)
	{
		pOutput->u1_logo_netflix_status = 1;
		pOutput->u8_logo_netflix_hold_frm = pOutput->u8_logo_netflix_hold_frm - 1;
	}
	else
	{
		pOutput->u1_logo_netflix_status = 0;
		pOutput->u8_logo_netflix_hold_frm = 0;
	}
}

VOID FRC_LogoDet_NearRim_logo(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int lt_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[0];
	unsigned int rt_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[7];
	unsigned int lb_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[24];
	unsigned int rb_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[31];	
	static unsigned int tmp_lt_logo_cnt[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static unsigned int tmp_rt_logo_cnt[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static unsigned int tmp_lb_logo_cnt[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static unsigned int tmp_rb_logo_cnt[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned char  NearRim_logo_status = 0;
	unsigned int u32_RB_val;
	unsigned int i = 0;
	unsigned int temp_sum_lt_logo_cnt = 0,temp_sum_rt_logo_cnt = 0,temp_sum_lb_logo_cnt = 0,temp_sum_rb_logo_cnt = 0;
	static unsigned int temp_lt_logo_cnt_avg = 0,temp_rt_logo_cnt_avg = 0,temp_lb_logo_cnt_avg = 0,temp_rb_logo_cnt_avg = 0;
	static unsigned int fcnt =0;

	ReadRegister(HARDWARE_HARDWARE_58_reg, 0, 31, &u32_RB_val);//bit 13

	tmp_lt_logo_cnt[16]=lt_logo_cnt;
	tmp_rt_logo_cnt[16]=rt_logo_cnt;
	tmp_lb_logo_cnt[16]=lb_logo_cnt;
	tmp_rb_logo_cnt[16]=rb_logo_cnt;
	
	for(i=0;i<16;i++){
		//first in and first out	
		tmp_lt_logo_cnt[i]=tmp_lt_logo_cnt[i+1];		
		tmp_rt_logo_cnt[i]=tmp_rt_logo_cnt[i+1];
		tmp_lb_logo_cnt[i]=tmp_lb_logo_cnt[i+1];
		tmp_rb_logo_cnt[i]=tmp_rb_logo_cnt[i+1];	
		//sum by temp logo cnt
		temp_sum_lt_logo_cnt = temp_sum_lt_logo_cnt + tmp_lt_logo_cnt[i];	
		temp_sum_rt_logo_cnt = temp_sum_rt_logo_cnt + tmp_rt_logo_cnt[i];
		temp_sum_lb_logo_cnt = temp_sum_lb_logo_cnt + tmp_lb_logo_cnt[i];
		temp_sum_rb_logo_cnt = temp_sum_rb_logo_cnt + tmp_rb_logo_cnt[i];			
	}
	temp_lt_logo_cnt_avg = 	temp_sum_lt_logo_cnt>>4;
	temp_rt_logo_cnt_avg = 	temp_sum_rt_logo_cnt>>4;
	temp_lb_logo_cnt_avg = 	temp_sum_lb_logo_cnt>>4;
	temp_rb_logo_cnt_avg = 	temp_sum_rb_logo_cnt>>4;

	if((temp_lt_logo_cnt_avg>130 && temp_lt_logo_cnt_avg<180)&&(temp_rt_logo_cnt_avg<=20)&&(temp_lb_logo_cnt_avg<=20)&&(temp_rb_logo_cnt_avg<=20)){
		NearRim_logo_status = 1;
	}else{
		NearRim_logo_status = 0;
	}

	if ( (fcnt % 10 == 0  && ((u32_RB_val >>  13) & 0x01) ==1 )){
		//rtd_pr_memc_emerg("[%s][%d][,%d,][,%d,%d,%d,%d,]\n",__FUNCTION__, __LINE__,NearRim_logo_status,temp_lt_logo_cnt_avg,temp_rt_logo_cnt_avg,temp_lb_logo_cnt_avg,temp_rb_logo_cnt_avg);
	}

	if (NearRim_logo_status == 1)
	{
		pOutput->u1_logo_NearRim_logo_status = 1;
		pOutput->u8_logo_NearRim_logo_hold_frm = 20;//pParam->u8_logo_netflix_hold_frm;
	}
	else if (pOutput->u8_logo_NearRim_logo_hold_frm > 0)
	{
		pOutput->u1_logo_NearRim_logo_status = 1;
		pOutput->u8_logo_NearRim_logo_hold_frm = pOutput->u8_logo_NearRim_logo_hold_frm - 1;
	}
	else
	{
		pOutput->u1_logo_NearRim_logo_status = 0;
		pOutput->u8_logo_NearRim_logo_hold_frm = 0;
	}

	if(fcnt<=0xfffA){
		fcnt++;	
	}else{
		fcnt=0;
	}

	pOutput->lt_logo_cnt_avg_IIR = temp_lt_logo_cnt_avg;
	pOutput->rt_logo_cnt_avg_IIR = temp_rt_logo_cnt_avg;
	pOutput->lb_logo_cnt_avg_IIR = temp_lb_logo_cnt_avg;
	pOutput->rb_logo_cnt_avg_IIR = temp_rb_logo_cnt_avg;

}

// HS merlin8 logo

VOID FRC_Logo_long_term_logo_rg_replace_mv(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int dhlogo_rgcnt[32] = {0};
	unsigned int replace_mv_en = 0;
	unsigned int replace_mv_write_en = 0;
	unsigned int blklogo_rg_th = 100; // from reg
	unsigned int temporal_pos_step = 1; // from reg 
	unsigned int temporal_neg_step = 3; // from reg
	unsigned int temporal_counter_th = 16; // from reg
	unsigned int temporal_counter = 0;
	unsigned int logo_rgn_count = 0;
	unsigned int logo_false_detect_replace_mv_off_en = 0;
	unsigned int logo_false_detect_rgn_th = 12;
	int i = 0, reg_write_value = 0;

	ReadRegister(SOFTWARE3_SOFTWARE3_48_reg, 31, 31, &replace_mv_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_48_reg, 30, 30, &replace_mv_write_en);
	ReadRegister(SOFTWARE3_SOFTWARE3_48_reg,  0, 10, &blklogo_rg_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_48_reg, 11, 15, &temporal_pos_step);
	ReadRegister(SOFTWARE3_SOFTWARE3_48_reg, 16, 20, &temporal_neg_step);
	ReadRegister(SOFTWARE3_SOFTWARE3_48_reg, 21, 28, &temporal_counter_th);

	ReadRegister(SOFTWARE2_SOFTWARE2_13_reg, 1, 6, &logo_false_detect_rgn_th);
	ReadRegister(SOFTWARE2_SOFTWARE2_13_reg, 0, 0, &logo_false_detect_replace_mv_off_en);

	if( replace_mv_en == 1 )
	{
		logo_rgn_count = 0;
		for(i=0;i<32;i++)
		{
			dhlogo_rgcnt[i] = s_pContext->_output_read_comreg.u10_dhlogo_rgcnt[i];
			temporal_counter = pOutput->u8_dh_logo_temporal_counter[i];

			if( dhlogo_rgcnt[i] >= blklogo_rg_th )
			{
				pOutput->u8_dh_logo_temporal_counter[i] = _CLIP_( temporal_counter + temporal_pos_step, 0, 1023);
			}
			else
			{
				pOutput->u8_dh_logo_temporal_counter[i] = _CLIP_( (int)temporal_counter - (int)temporal_neg_step, 0, 1023);
			}

			if( pOutput->u8_dh_logo_temporal_counter[i] > temporal_counter_th )	// long-term logo rg
			{
				reg_write_value = reg_write_value + ( 1 << i );
				logo_rgn_count = logo_rgn_count + 1;
			}
		}

		// replace mv off for logo false detect
		if( (logo_false_detect_replace_mv_off_en == 1) && (logo_rgn_count > logo_false_detect_rgn_th) )
		{
			reg_write_value = 0;			
		}		

		#if (IC_K24)
		if( replace_mv_write_en == 1 )
		{
			WriteRegister(MC3_logo_dh_apply_flag_reg, 0, 31, reg_write_value);
		}
		#endif
	}

}
// HS merlin8 logo
static _str_logo_p_step_table logo_pstep_table[6]={
	{0, 5, 8},
	{0, 3, 5}, // original
	{0, 2, 4},
	{0, 1, 2},
	{0, 1, 1},
	{0, 0, 0}, // low
};


static _str_logo_n_step_table logo_nstep_table[6]={
	{0, 5, 8}, // original
	{0, 3, 5},
	{0, 2, 4},
	{0, 1, 2},
	{0, 1, 1},
	{0, 0, 0}, // low
};

_str_logo_p_step_table *fwif_MEMC_get_Logo_Pstep_table(unsigned char level)
{
	if( level < 2 ){
		return (_str_logo_p_step_table *)&logo_pstep_table[level];
	}else{
		return (_str_logo_p_step_table *)&logo_pstep_table[0];
	}
}

_str_logo_n_step_table *fwif_MEMC_get_Logo_Nstep_table(unsigned char level)
{
	if( level < 5 ){
		return (_str_logo_n_step_table *)&logo_nstep_table[level];
	}else{
		return (_str_logo_n_step_table *)&logo_nstep_table[0];
	}
}
extern unsigned char g_510_flag, g_511_flag, g_246_flag, g_476_flag, g_248_flag, g_60028_flag;

VOID FRC_LogoDet_StillLogoDetection(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int u10_blklogo_rgcnt[32] = {0}, zmv_rg_cnt[32] = {0};
	static unsigned int u10_blklogo_rgcnt_pre[32] = {0}, zmv_rg_cnt_pre[32] = {0}; // , zmv_rg_cnt_pool[32][8] = {0}, zmv_diff_pool[32][8] = {0};
	unsigned int still_logo_det_en = 0;
	unsigned int log_en = 0, log_en2 = 0, test_en = 0, test_en2 = 0, p_step_mode = 0, p_step_status_en = 0, n_step_mode = 0, n_step_status_en = 0, force_en = 0, force_type = 0;
	unsigned int temporal_counter = 0;
	signed short gmv_x_1st = 0, gmv_y_1st = 0, rmv_x_1st[32] = {0}, rmv_y_1st[32] = {0}, rmv_x_2nd[32] = {0}, rmv_y_2nd[32] = {0};
	unsigned short rmv_cnt_1st[32] = {0}, rmv_unconf_1st[32] = {0}, rmv_cnt_2nd[32] = {0}, rmv_unconf_2nd[32] = {0};
	unsigned int gmv_cnt_1st = 0, zmv_rg_cnt_avg = 0, zmv_diff_avg = 0, zmv_all_cnt = 0;
	static unsigned char subtitle_queue[32][300] = {0};
	unsigned int subtitle_th = 0, subtitle_th2 = 0, subtitle_sum = 0, fwcontrol_video189 = 0;
	unsigned char logo_case = 0;
	unsigned int u32_hist_sum = 0, u32_hist_0 = s_pContext->_output_me_sceneAnalysis.u25_yuv_hist[0];
	bool u1_hist0_high = false;
	static unsigned char status = 0, sc_cnt = 0, frame_cnt = 0;
	int still_logo_step = 0, zmv_deviation[32] = {0}, logo_cnt_veriation = 0, zmv_cnt_veriation = 0;
	unsigned int sc_status = 0, logo_cnt_num = 0, zmv_cnt_num = 0, logo_cnt_diff = 0, zmv_cnt_diff = 0;
	static unsigned int logo_cnt_avg[32] = {0}, zmv_cnt_avg[32] = {0};
	unsigned char i = 0;
	int j = 0;
	_str_logo_p_step_table *tmp_logo_p_step_table = fwif_MEMC_get_Logo_Pstep_table(0);
	_str_logo_n_step_table *tmp_logo_n_step_table = fwif_MEMC_get_Logo_Nstep_table(0);
	unsigned char p_step_level = 0, n_step_level = 0;
	static unsigned char p_step_level_pre = 0, n_step_level_pre = 0;
	unsigned int dh_logo_rgn_h2 = Dehalo_Logo_Region_Suggest_Val_With_Res.dh_logo_rgn_h2;
	unsigned int dh_logo_rgn_h4 = Dehalo_Logo_Region_Suggest_Val_With_Res.dh_logo_rgn_h4;
	unsigned int dh_logo_rgn_v1 = Dehalo_Logo_Region_Suggest_Val_With_Res.dh_logo_rgn_v1;

	ReadRegister(SOFTWARE2_SOFTWARE2_48_reg, 31, 31, &still_logo_det_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_48_reg, 30, 30, &log_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_48_reg, 29, 29, &log_en2);
	ReadRegister(SOFTWARE2_SOFTWARE2_48_reg, 28, 28, &test_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_48_reg, 27, 27, &force_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_48_reg, 26, 26, &force_type);
	ReadRegister(SOFTWARE2_SOFTWARE2_48_reg, 23, 24, &test_en2);
	ReadRegister(SOFTWARE2_SOFTWARE2_48_reg, 22, 22, &n_step_status_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_48_reg, 19, 21, &n_step_mode);
	ReadRegister(SOFTWARE2_SOFTWARE2_48_reg, 15, 15, &p_step_status_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_48_reg, 12, 14, &p_step_mode);
	ReadRegister(SOFTWARE2_SOFTWARE2_49_reg, 0, 7, &subtitle_th); // 
	ReadRegister(SOFTWARE2_SOFTWARE2_49_reg, 8, 11, &subtitle_th2); // 
	ReadRegister(KME_ME1_TOP1_ME1_WLC1_00_reg, 0, 0, &sc_status);
	ReadRegister(HARDWARE_HARDWARE_25_reg, 9, 9, &fwcontrol_video189);
	gmv_x_1st = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	gmv_y_1st = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
	gmv_cnt_1st = s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb;

	for(i=0;i<32;i++)
	{
		u10_blklogo_rgcnt[i] = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[i];
		rmv_x_1st[i] = s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i];
		rmv_y_1st[i] = s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i];
		rmv_cnt_1st[i] = s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[i];
		rmv_unconf_1st[i] = s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[i];
		rmv_x_2nd[i] = s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[i];
		rmv_y_2nd[i] = s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[i];
		rmv_cnt_2nd[i] = s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[i];
		rmv_unconf_2nd[i] = s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[i];
		zmv_rg_cnt[i] = s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[i];
		zmv_all_cnt += zmv_rg_cnt[i];
		u32_hist_sum += s_pContext->_output_me_sceneAnalysis.u25_yuv_hist[i];
		if(test_en2 == 0){
			logo_cnt_avg[i] = (logo_cnt_avg[i] * 7 + u10_blklogo_rgcnt_pre[i])>>3;
			zmv_cnt_avg[i] = (zmv_cnt_avg[i] * 7 + zmv_rg_cnt_pre[i])>>3;
		}
		else if(test_en2 == 1){
			logo_cnt_avg[i] = (logo_cnt_avg[i] * 3 + u10_blklogo_rgcnt_pre[i])>>2;
			zmv_cnt_avg[i] = (zmv_cnt_avg[i] * 3 + zmv_rg_cnt_pre[i])>>2;
		}
		else if(test_en2 == 2){
			logo_cnt_avg[i] = u10_blklogo_rgcnt_pre[i];
			zmv_cnt_avg[i] = zmv_rg_cnt_pre[i];
		}
	}
	// check whether image is dark by yuv histogram target:#405
	u1_hist0_high = (u32_hist_0*100>u32_hist_sum*90) ? true : false;

	if( still_logo_det_en == 1 )
	{
		frame_cnt++;
		if(frame_cnt >= 100){
			frame_cnt = 0;
		}
		if(sc_status){
			sc_cnt = 0;
		}

		if((_ABS_(gmv_x_1st) > 4 || _ABS_(gmv_y_1st) > 2) && (gmv_cnt_1st > 24000) && test_en){
			unsigned int gmv_sum = 0;
			gmv_sum = _ABS_(gmv_x_1st) + _ABS_(gmv_y_1st) + 10;
			if(gmv_sum < 50)
				gmv_sum = 50;
			WriteRegister(KME_ME1_TOP2_KME_ME1_TOP2_0C_reg, 8, 19, gmv_sum); // change TH for rmv_cnt_1st
			status = 1;
		}
		else{
			if(status == 1){
				WriteRegister(KME_ME1_TOP2_KME_ME1_TOP2_0C_reg, 8, 19, 50);
				status = 0;
			}
		}

		// N_step
		if(n_step_status_en == 1){
			if(zmv_all_cnt >= 240*135*85/100){ // no motion
				n_step_level = 5;
				tmp_logo_n_step_table = fwif_MEMC_get_Logo_Nstep_table(n_step_level);
			}
			else if(s_pContext->_output_me_sceneAnalysis.u2_panning_flag == 0){ // no panning
				n_step_level = n_step_mode; // default 3
				if(g_511_flag || g_476_flag || g_246_flag){
					n_step_level = 4;
				}
				tmp_logo_n_step_table = fwif_MEMC_get_Logo_Nstep_table(n_step_level);
			}
			else{
				n_step_level = 0;
				tmp_logo_n_step_table = fwif_MEMC_get_Logo_Nstep_table(n_step_level);
			}
		}
		else{
			n_step_level = 0;
			tmp_logo_n_step_table = fwif_MEMC_get_Logo_Nstep_table(n_step_level);
		}
		if(n_step_level != n_step_level_pre){
			WriteRegister(KME_LOGO0_KME_LOGO0_D8_reg, 0, 3, tmp_logo_n_step_table->y0);
			WriteRegister(KME_LOGO0_KME_LOGO0_D8_reg, 4, 7, tmp_logo_n_step_table->y1);
			WriteRegister(KME_LOGO0_KME_LOGO0_D8_reg, 8, 11, tmp_logo_n_step_table->y2);
			#if (IC_K24)
			WriteRegister(KME_LOGO3_km_logo_blk_480x270_6_reg, 24, 27, tmp_logo_n_step_table->y0);
			WriteRegister(KME_LOGO3_km_logo_blk_480x270_6_reg, 28, 31, tmp_logo_n_step_table->y1);
			WriteRegister(KME_LOGO3_km_logo_blk_480x270_7_reg, 16, 19, tmp_logo_n_step_table->y2);
			#endif
		}

		// P_step
		if(p_step_status_en == 1){
			if(zmv_all_cnt >= 240*135*85/100){ // no motion
				p_step_level = 5;
				tmp_logo_p_step_table = fwif_MEMC_get_Logo_Pstep_table(p_step_level);
			}
			else{
				p_step_level = p_step_mode; // default 1
				if(g_510_flag || g_246_flag){
					p_step_level = 0;
				}
				tmp_logo_p_step_table = fwif_MEMC_get_Logo_Pstep_table(p_step_level);
			}
		}
		else{
			p_step_level = 1;
			tmp_logo_p_step_table = fwif_MEMC_get_Logo_Pstep_table(p_step_level);
		}
		if(p_step_level != p_step_level_pre){
			//WriteRegister(KME_LOGO0_KME_LOGO0_E4_reg, 0, 3, tmp_logo_p_step_table->y0); // move to Write_ComReg.c
			//WriteRegister(KME_LOGO0_KME_LOGO0_E4_reg, 4, 7, tmp_logo_p_step_table->y1);
			//WriteRegister(KME_LOGO0_KME_LOGO0_E4_reg, 8, 11, tmp_logo_p_step_table->y2);
			#if (IC_K24)
			WriteRegister(KME_LOGO3_km_logo_blk_480x270_5_reg, 24, 27, tmp_logo_p_step_table->y0);
			WriteRegister(KME_LOGO3_km_logo_blk_480x270_5_reg, 28, 31, tmp_logo_p_step_table->y1);
			WriteRegister(KME_LOGO3_km_logo_blk_480x270_7_reg, 0, 3, tmp_logo_p_step_table->y2);
			#endif
		}

		for(i=0;i<32;i++)
		{
			zmv_rg_cnt_avg = 0;
			zmv_cnt_diff = 0;
			zmv_diff_avg = 0;
			//zmv_deviation = 0;
			temporal_counter = pOutput->u32_still_logo_cnt[i];

			if(i < 24){
				logo_cnt_num = 34*30;
				zmv_cnt_num = 34*30;
			}
			else{
				logo_cnt_num = 33*30;
				zmv_cnt_num = 33*30;
			}

			#if 1
			//==== subtitle detect ====//
			subtitle_sum = 0;
			if((2 <= i && i <= 5) || (26 <= i && i <= 29)){
				if(u10_blklogo_rgcnt[i] >= u10_blklogo_rgcnt_pre[i] + subtitle_th || u10_blklogo_rgcnt_pre[i] >= u10_blklogo_rgcnt[i] + subtitle_th){
					subtitle_queue[i][0] = 1;
				}
				else{
					subtitle_queue[i][0] = 0;
				}

				for(j = 0; j < 300; j++){
					subtitle_sum += subtitle_queue[i][j];
				}

				if(subtitle_sum >= subtitle_th2){
					pOutput->u32_subtitle_flag[i] = 1;
				}
				else{
					pOutput->u32_subtitle_flag[i] = 0;
				}
				if(log_en2){
 					rtd_pr_memc_notice("[%s][subtitle_sum[%d],%d,][subtitle_queue[%d][0],%d,][u10_blklogo_rgcnt,%d,%d,][still_logo_cnt,%d,][subtitle_flag,%d,]\n\r", __FUNCTION__,
						i, subtitle_sum, i, subtitle_queue[i][0], u10_blklogo_rgcnt[i], u10_blklogo_rgcnt_pre[i], pOutput->u32_still_logo_cnt[i], pOutput->u32_subtitle_flag[i]);
					rtd_pr_memc_notice("[%s][subtitle_queue][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]", 
						__FUNCTION__, subtitle_queue[i][0], subtitle_queue[i][1], subtitle_queue[i][2], subtitle_queue[i][3], subtitle_queue[i][4], subtitle_queue[i][5], subtitle_queue[i][6], subtitle_queue[i][7],
						subtitle_queue[i][8], subtitle_queue[i][9], subtitle_queue[i][10], subtitle_queue[i][11], subtitle_queue[i][12], subtitle_queue[i][13], subtitle_queue[i][14], subtitle_queue[i][15],
						subtitle_queue[i][16], subtitle_queue[i][17], subtitle_queue[i][18], subtitle_queue[i][19], subtitle_queue[i][20], subtitle_queue[i][21], subtitle_queue[i][22], subtitle_queue[i][23],
						subtitle_queue[i][24], subtitle_queue[i][25], subtitle_queue[i][26], subtitle_queue[i][27], subtitle_queue[i][28], subtitle_queue[i][29], subtitle_queue[i][30], subtitle_queue[i][31]);
				}
			}
			else{
				pOutput->u32_subtitle_flag[i] = 0;
			}

			//==== still logo detect ====//
			if(u10_blklogo_rgcnt[i] >= logo_cnt_avg[i]){
				logo_cnt_diff = u10_blklogo_rgcnt[i] - logo_cnt_avg[i];
				logo_cnt_veriation = logo_cnt_diff*1000/logo_cnt_num;
			}
			else{
				logo_cnt_diff = logo_cnt_avg[i] - u10_blklogo_rgcnt[i];
				logo_cnt_veriation = -1*(logo_cnt_diff*1000/logo_cnt_num);
			}
			if(zmv_rg_cnt[i] >= zmv_cnt_avg[i]){
				zmv_cnt_diff = zmv_rg_cnt[i] - zmv_cnt_avg[i];
				zmv_cnt_veriation = zmv_cnt_diff*1000/zmv_cnt_num;
			}
			else{
				zmv_cnt_diff = zmv_cnt_avg[i] - zmv_rg_cnt[i];
				zmv_cnt_veriation = -1*(zmv_cnt_diff*1000/zmv_cnt_num);
			}
			if(zmv_rg_cnt[i] >= u10_blklogo_rgcnt[i]){
				zmv_deviation[i] = zmv_rg_cnt[i] - u10_blklogo_rgcnt[i];
			}
			else{
				zmv_deviation[i] = -1*(u10_blklogo_rgcnt[i] - zmv_rg_cnt[i]);
			}

			if(zmv_rg_cnt[i] > zmv_cnt_num*80/100 /*|| (_ABS_(rmv_x_1st[i]) <= 4 && _ABS_(rmv_y_1st[i]) <= 2 && rmv_cnt_1st[i] >= zmv_cnt_num*75/100)*/){
				// logo_case_1 : no motion, do nothing
				logo_case = 1;
			}
  			else if(zmv_rg_cnt[i] <= zmv_cnt_num*1/100){
  			//else if(zmv_rg_cnt[i] == 0){
				// logo_case_2 : big motion, decrease
				logo_case = 2;
			}
			else{ // other case
				if(u10_blklogo_rgcnt[i] < logo_cnt_num*4/100){
					// logo_case_3 : no logo, decrease
					logo_case = 3;
				}
				else if(u10_blklogo_rgcnt[i] > logo_cnt_num*60/100){
					// logo_case_4 : too much logo, do nothing
					logo_case = 4;
				}
				else{
					if(_ABS_(logo_cnt_veriation) <= 5){
						// increase high
						if(_ABS_(zmv_cnt_veriation) <= 5){
							logo_case = 5; // logo_cnt & zmv_cnt is very stable, increase highest
						}
						else if(5 < _ABS_(zmv_cnt_veriation) && _ABS_(zmv_cnt_veriation) <= 10){
							logo_case = 6; // 
						}
						else if(10 < _ABS_(zmv_cnt_veriation) && _ABS_(zmv_cnt_veriation) <= 30){
							logo_case = 7; // 
						}
						else{ // if(zmv_cnt_veriation < -10){
							logo_case = 8; // 
						}
					}
					else if(5 < _ABS_(logo_cnt_veriation) && _ABS_(logo_cnt_veriation) <= 10){
						// increase low
						if(_ABS_(zmv_cnt_veriation) <= 5){
							logo_case = 9; // 
						}
						else if(5 < _ABS_(zmv_cnt_veriation) && _ABS_(zmv_cnt_veriation) <= 10){
							logo_case = 10; // 
						}
						else if(10 < _ABS_(zmv_cnt_veriation) && _ABS_(zmv_cnt_veriation) <= 30){
							logo_case = 11; // 
						}
						else{ // if(zmv_cnt_veriation < -10){
							logo_case = 12; // 
						}
					}
					else if(10 < _ABS_(logo_cnt_veriation) && _ABS_(logo_cnt_veriation) <= 30){
						// increase low
						if(_ABS_(zmv_cnt_veriation) <= 5){
							logo_case = 13; // 
						}
						else if(5 < _ABS_(zmv_cnt_veriation) && _ABS_(zmv_cnt_veriation) <= 10){
							logo_case = 14; // 
						}
						else if(10 < _ABS_(zmv_cnt_veriation) && _ABS_(zmv_cnt_veriation) <= 30){
							logo_case = 15; // 
						}
						else{ // if(zmv_cnt_veriation < -10){
							logo_case = 16; // 
						}
					}
					else{ // if(logo_cnt_veriation < -10){
						// decrease
						if(_ABS_(zmv_cnt_veriation) <= 5){
							logo_case = 17; // 
						}
						else if(5 < _ABS_(zmv_cnt_veriation) && _ABS_(zmv_cnt_veriation) <= 10){
							logo_case = 18; // 
						}
						else if(10 < _ABS_(zmv_cnt_veriation) && _ABS_(zmv_cnt_veriation) <= 30){
							logo_case = 19; // 
						}
						else{ // if(zmv_cnt_veriation < -10){
							logo_case = 20; // 
						}
					}
				}
			}

			switch (logo_case){
				case 1:
					still_logo_step = -2;
					break;
				case 2:
					still_logo_step = -9;
					break;
				case 3:
					still_logo_step = -4;
					break;
				case 4:
					still_logo_step = 0;
					break;
				case 5:
					still_logo_step = 14;
					break;
				case 6:
					still_logo_step = 13;
					break;
				case 7:
					still_logo_step = 12;
					break;
				case 8:
					still_logo_step = 11;
					break;
				case 9:
					still_logo_step = 9;
					break;
				case 10:
					still_logo_step = 7;
					break;
				case 11:
					still_logo_step = 5;
					break;
				case 12:
					still_logo_step = 3;
					break;
				case 13:
					still_logo_step = 2;
					break;
				case 14:
					still_logo_step = 1;
					break;
				case 15:
					still_logo_step = 0;
					break;
				case 16:
					still_logo_step = -1;
					break;
				case 17:
					still_logo_step = -3;
					break;
				case 18:
					still_logo_step = -5;
					break;
				case 19:
					still_logo_step = -7;
					break;
				case 20:
					still_logo_step = -9;
					break;
				default:
					still_logo_step = 0;
					break;
			}
			//if(_ABS_(rmv_x_1st[i]) <= 4 && _ABS_(rmv_y_1st[i]) > 2 && rmv_cnt_1st[i] >= zmv_cnt_num/2)
			//	still_logo_step = (still_logo_step - 5)>>1;

			if((g_48_flag == 1) && still_logo_step > 0){
				still_logo_step = -2;
			}
			else if(g_244_flag && (i == 7) && still_logo_step > 0){
				still_logo_step = -2;
			}
			else if(g_510_flag && (i <= 25 || 30 <= i) && still_logo_step > 0){
				still_logo_step = -2;
			}
			else if(g_60028_flag && (i==0 || i==1 || i==8 || i==9 || i==16 || i==17 || i==24 || i==25) && still_logo_step > 0)
			{
				still_logo_step = -2;
			}
			else if(g_248_flag && (2 <= i && i <= 6) && still_logo_step > 0){
				still_logo_step = -2;
			}
			else if(u1_hist0_high && ((5 <= i && i <= 7)||(13 <= i && i <= 15)||(21 <= i && i <= 23)||(29 <= i)) && still_logo_step > 0){
				still_logo_step = -2;
			}
			else if(fwcontrol_video189 && ((11 <= i && i <= 12) || (19 <= i && i <= 20)) && still_logo_step > 0){
				still_logo_step = -2;
			}

			if(((8 <= i && i <= 17) || i == 22 || i == 23) && still_logo_step > 0){
				still_logo_step = still_logo_step/4;
			}

			if(pOutput->u32_subtitle_flag[i] == 1 && logo_case > 2 && still_logo_step < -1){
				still_logo_step = -1;
			}

			pOutput->u32_still_logo_cnt[i] = _CLIP_( (int)temporal_counter + (int)still_logo_step, 0, 1023);

			if(log_en){
				rtd_pr_memc_notice("[%s][region,%d,%d,][logo_case,%d,%d,][logo_cnt,%d,%d,%d,][zmv_cnt,%d,%d,%d,][sp_flg,%d,%d,%d,%d]\n\r", __FUNCTION__,
					i, pOutput->u32_still_logo_cnt[i], logo_case, still_logo_step, u10_blklogo_rgcnt[i], logo_cnt_avg[i], logo_cnt_veriation, zmv_rg_cnt[i], zmv_cnt_avg[i], zmv_cnt_veriation, g_48_flag, g_190_flag, u1_hist0_high, fwcontrol_video189);
			}
			#endif

			if(force_en){
				if(force_type){
					pOutput->u32_still_logo_cnt[i] = 1023;
				}
				else{
					pOutput->u32_still_logo_cnt[i] = 0;
				}
			}

			if(sc_cnt > 0)
				sc_cnt--;

		}


		for(i=0;i<32;i++)
		{
			u10_blklogo_rgcnt_pre[i] = u10_blklogo_rgcnt[i];
			zmv_rg_cnt_pre[i] = zmv_rg_cnt[i];
			if((2 <= i && i <= 5) || (26 <= i && i <= 29)){
				for(j = 0; j < 299; j++){
					subtitle_queue[i][299-j] = subtitle_queue[i][298-j];
				}
				subtitle_queue[i][0] = 0;
			}
		}
		p_step_level_pre = p_step_level;
		n_step_level_pre = n_step_level;

		// dynamic change dehalo_logo_region
		if(g_476_flag && (pOutput->u32_still_logo_cnt[27] >= 800 && pOutput->u32_still_logo_cnt[28] >= 800 && pOutput->u32_still_logo_cnt[0] < 200 && pOutput->u32_still_logo_cnt[1] < 200 
			&& pOutput->u32_still_logo_cnt[2] < 200 && pOutput->u32_still_logo_cnt[3] < 200 && pOutput->u32_still_logo_cnt[4] < 200 && pOutput->u32_still_logo_cnt[5] < 200 
			&& pOutput->u32_still_logo_cnt[6] < 200 && pOutput->u32_still_logo_cnt[7] < 200 && pOutput->u32_still_logo_cnt[8] < 200 && pOutput->u32_still_logo_cnt[9] < 200 
			&& pOutput->u32_still_logo_cnt[10] < 200 && pOutput->u32_still_logo_cnt[11] < 200 && pOutput->u32_still_logo_cnt[12] < 200 && pOutput->u32_still_logo_cnt[13] < 200 
			&& pOutput->u32_still_logo_cnt[14] < 200 && pOutput->u32_still_logo_cnt[15] < 200 && pOutput->u32_still_logo_cnt[16] < 200 && pOutput->u32_still_logo_cnt[17] < 200
			&& pOutput->u32_still_logo_cnt[18] < 200 && pOutput->u32_still_logo_cnt[19] < 200 && pOutput->u32_still_logo_cnt[20] < 200 && pOutput->u32_still_logo_cnt[21] < 200
			&& pOutput->u32_still_logo_cnt[22] < 200 && pOutput->u32_still_logo_cnt[23] < 200 && pOutput->u32_still_logo_cnt[24] < 200 && pOutput->u32_still_logo_cnt[25] < 200
			&& pOutput->u32_still_logo_cnt[26] < 200 && pOutput->u32_still_logo_cnt[29] < 200 && pOutput->u32_still_logo_cnt[30] < 200 && pOutput->u32_still_logo_cnt[31] < 200)){
			dh_logo_rgn_h2 = (dh_logo_rgn_h2*933)>>10; // for #476 4k:82
		}
		WriteRegister(KME_DEHALO2_KME_DEHALO2_98_reg, 16, 23, dh_logo_rgn_h2);//4k:90

		if(pOutput->u32_still_logo_cnt[7] > 800 && pOutput->u32_still_logo_cnt[6] < 200 && pOutput->u32_still_logo_cnt[14] < 200 
			&& pOutput->u32_still_logo_cnt[22] < 200 && pOutput->u32_still_logo_cnt[30] < 200){
			dh_logo_rgn_h4 = (dh_logo_rgn_h4*1215)>>10; // for #248 4k:210
		}
		WriteRegister(KME_DEHALO2_KME_DEHALO2_9C_reg, 0, 7, dh_logo_rgn_h4); // for #474 //4k:177

		if(pOutput->u32_still_logo_cnt[18] > 800 && pOutput->u32_still_logo_cnt[19] > 800 && pOutput->u32_still_logo_cnt[20] > 800 
			&& pOutput->u32_still_logo_cnt[21] > 800 && pOutput->u32_still_logo_cnt[0] < 200 && pOutput->u32_still_logo_cnt[1] < 200 
			&& pOutput->u32_still_logo_cnt[2] < 200 && pOutput->u32_still_logo_cnt[3] < 200 && pOutput->u32_still_logo_cnt[4] < 200
			&& pOutput->u32_still_logo_cnt[5] < 200 && pOutput->u32_still_logo_cnt[6] < 200 && pOutput->u32_still_logo_cnt[7] < 200
			&& pOutput->u32_still_logo_cnt[8] < 200 && pOutput->u32_still_logo_cnt[9] < 200 && pOutput->u32_still_logo_cnt[10] < 200 
			&& pOutput->u32_still_logo_cnt[11] < 200 && pOutput->u32_still_logo_cnt[12] < 200 && pOutput->u32_still_logo_cnt[13] < 200 
			&& pOutput->u32_still_logo_cnt[14] < 200 && pOutput->u32_still_logo_cnt[15] < 200){
			dh_logo_rgn_v1 = (dh_logo_rgn_v1*1935)>>10; // for #480 4k:68
		}
		WriteRegister(KME_DEHALO2_KME_DEHALO2_94_reg, 8, 15, dh_logo_rgn_v1); //4k:36
	}
	else{
		for(i=0;i<32;i++)
		{
			pOutput->u32_still_logo_cnt[i] = 0;
			//for(j = 0; j < 8; j++){
			//	zmv_rg_cnt_pool[i][j] = 0;
			//}
			for(j = 0; j < 300; j++){
				subtitle_queue[i][j] = 0;
			}
		}
		WriteRegister(KME_ME1_TOP2_KME_ME1_TOP2_0C_reg, 8, 19, 50);
	}

	if(log_en){
		#if (IC_K24)
		unsigned int mc_local_lpf_cnt[32]={0};
		ReadRegister(MC3_mc_local_lpf_cnt0_reg, 0, 14, &mc_local_lpf_cnt[0]);
		ReadRegister(MC3_mc_local_lpf_cnt0_reg, 15, 29, &mc_local_lpf_cnt[1]);
		ReadRegister(MC3_mc_local_lpf_cnt1_reg, 0, 14, &mc_local_lpf_cnt[2]);
		ReadRegister(MC3_mc_local_lpf_cnt1_reg, 15, 29, &mc_local_lpf_cnt[3]);
		ReadRegister(MC3_mc_local_lpf_cnt2_reg, 0, 14, &mc_local_lpf_cnt[4]);
		ReadRegister(MC3_mc_local_lpf_cnt2_reg, 15, 29, &mc_local_lpf_cnt[5]);
		ReadRegister(MC3_mc_local_lpf_cnt3_reg, 0, 14, &mc_local_lpf_cnt[6]);
		ReadRegister(MC3_mc_local_lpf_cnt3_reg, 15, 29, &mc_local_lpf_cnt[7]);
		ReadRegister(MC3_mc_local_lpf_cnt4_reg, 0, 14, &mc_local_lpf_cnt[8]);
		ReadRegister(MC3_mc_local_lpf_cnt4_reg, 15, 29, &mc_local_lpf_cnt[9]);
		ReadRegister(MC3_mc_local_lpf_cnt5_reg, 0, 14, &mc_local_lpf_cnt[10]);
		ReadRegister(MC3_mc_local_lpf_cnt5_reg, 15, 29, &mc_local_lpf_cnt[11]);
		ReadRegister(MC3_mc_local_lpf_cnt6_reg, 0, 14, &mc_local_lpf_cnt[12]);
		ReadRegister(MC3_mc_local_lpf_cnt6_reg, 15, 29, &mc_local_lpf_cnt[13]);
		ReadRegister(MC3_mc_local_lpf_cnt7_reg, 0, 14, &mc_local_lpf_cnt[14]);
		ReadRegister(MC3_mc_local_lpf_cnt7_reg, 15, 29, &mc_local_lpf_cnt[15]);
		ReadRegister(MC3_mc_local_lpf_cnt8_reg, 0, 14, &mc_local_lpf_cnt[16]);
		ReadRegister(MC3_mc_local_lpf_cnt8_reg, 15, 29, &mc_local_lpf_cnt[17]);
		ReadRegister(MC3_mc_local_lpf_cnt9_reg, 0, 14, &mc_local_lpf_cnt[18]);
		ReadRegister(MC3_mc_local_lpf_cnt9_reg, 15, 29, &mc_local_lpf_cnt[19]);
		ReadRegister(MC3_mc_local_lpf_cnt10_reg, 0, 14, &mc_local_lpf_cnt[20]);
		ReadRegister(MC3_mc_local_lpf_cnt10_reg, 15, 29, &mc_local_lpf_cnt[21]);
		ReadRegister(MC3_mc_local_lpf_cnt11_reg, 0, 14, &mc_local_lpf_cnt[22]);
		ReadRegister(MC3_mc_local_lpf_cnt11_reg, 15, 29, &mc_local_lpf_cnt[23]);
		ReadRegister(MC3_mc_local_lpf_cnt12_reg, 0, 14, &mc_local_lpf_cnt[24]);
		ReadRegister(MC3_mc_local_lpf_cnt12_reg, 15, 29, &mc_local_lpf_cnt[25]);
		ReadRegister(MC3_mc_local_lpf_cnt13_reg, 0, 14, &mc_local_lpf_cnt[26]);
		ReadRegister(MC3_mc_local_lpf_cnt13_reg, 15, 29, &mc_local_lpf_cnt[27]);
		ReadRegister(MC3_mc_local_lpf_cnt14_reg, 0, 14, &mc_local_lpf_cnt[28]);
		ReadRegister(MC3_mc_local_lpf_cnt14_reg, 15, 29, &mc_local_lpf_cnt[29]);
		ReadRegister(MC3_mc_local_lpf_cnt15_reg, 0, 14, &mc_local_lpf_cnt[30]);
		ReadRegister(MC3_mc_local_lpf_cnt15_reg, 15, 29, &mc_local_lpf_cnt[31]);
		#endif

		rtd_pr_memc_notice("[%s][GMV_1st,%d,%d,][GMV_2nd,%d,%d,][GMV_1st_cnt,%d,%d,][GMV_2nd_cnt,%d,%d,][zmv_all_cnt,%d,][u1_logo_PurePanning_status,%d,][APL,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb, s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb,
			s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb, s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb,
			s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb, s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb,
			s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb, s_pContext->_output_read_comreg.u12_me_GMV_2nd_unconf_rb, 
			zmv_all_cnt, pOutput->u1_logo_PurePanning_status, s_pContext->_output_read_comreg.u26_me_aAPLp_rb);
		rtd_pr_memc_notice("[%s][aa][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]", 
			__FUNCTION__, u10_blklogo_rgcnt[0], u10_blklogo_rgcnt[1], u10_blklogo_rgcnt[2], u10_blklogo_rgcnt[3], u10_blklogo_rgcnt[4], u10_blklogo_rgcnt[5], u10_blklogo_rgcnt[6], u10_blklogo_rgcnt[7],
			u10_blklogo_rgcnt[8], u10_blklogo_rgcnt[9], u10_blklogo_rgcnt[10], u10_blklogo_rgcnt[11], u10_blklogo_rgcnt[12], u10_blklogo_rgcnt[13], u10_blklogo_rgcnt[14], u10_blklogo_rgcnt[15],
			u10_blklogo_rgcnt[16], u10_blklogo_rgcnt[17], u10_blklogo_rgcnt[18], u10_blklogo_rgcnt[19], u10_blklogo_rgcnt[20], u10_blklogo_rgcnt[21], u10_blklogo_rgcnt[22], u10_blklogo_rgcnt[23],
			u10_blklogo_rgcnt[24], u10_blklogo_rgcnt[25], u10_blklogo_rgcnt[26], u10_blklogo_rgcnt[27], u10_blklogo_rgcnt[28], u10_blklogo_rgcnt[29], u10_blklogo_rgcnt[30], u10_blklogo_rgcnt[31]);
		rtd_pr_memc_notice("[%s][bb][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]", 
			__FUNCTION__, pOutput->u32_still_logo_cnt[0], pOutput->u32_still_logo_cnt[1], pOutput->u32_still_logo_cnt[2], pOutput->u32_still_logo_cnt[3], pOutput->u32_still_logo_cnt[4], pOutput->u32_still_logo_cnt[5], pOutput->u32_still_logo_cnt[6], pOutput->u32_still_logo_cnt[7],
			pOutput->u32_still_logo_cnt[8], pOutput->u32_still_logo_cnt[9], pOutput->u32_still_logo_cnt[10], pOutput->u32_still_logo_cnt[11], pOutput->u32_still_logo_cnt[12], pOutput->u32_still_logo_cnt[13], pOutput->u32_still_logo_cnt[14], pOutput->u32_still_logo_cnt[15],
			pOutput->u32_still_logo_cnt[16], pOutput->u32_still_logo_cnt[17], pOutput->u32_still_logo_cnt[18], pOutput->u32_still_logo_cnt[19], pOutput->u32_still_logo_cnt[20], pOutput->u32_still_logo_cnt[21], pOutput->u32_still_logo_cnt[22], pOutput->u32_still_logo_cnt[23],
			pOutput->u32_still_logo_cnt[24], pOutput->u32_still_logo_cnt[25], pOutput->u32_still_logo_cnt[26], pOutput->u32_still_logo_cnt[27], pOutput->u32_still_logo_cnt[28], pOutput->u32_still_logo_cnt[29], pOutput->u32_still_logo_cnt[30], pOutput->u32_still_logo_cnt[31]);
		rtd_pr_memc_notice("[%s][me1_bv_stats_rg_cnt][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[0], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[1], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[2], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[3], 
			s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[4], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[5], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[6], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[7], 
			s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[8], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[9], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[10], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[11], 
			s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[12], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[13], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[14], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[15], 
			s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[16], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[17], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[18], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[19], 
			s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[20], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[21], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[22], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[23], 
			s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[24], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[25], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[26], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[27], 
			s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[28], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[29], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[30], s_pContext->_output_read_comreg.me1_bv_stats_rg_cnt[31]);
		rtd_pr_memc_notice("[%s][s11_me_rMV_1st_vx_rb][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[0], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[1], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[2], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[3], 
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[4], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[5], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[6], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[7], 
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[8], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[9], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[10], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[11], 
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[12], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[13], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[14], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[15], 
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[16], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[17], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[18], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[19], 
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[20], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[21], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[22], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[23], 
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[24], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[25], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[26], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[27], 
			s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[28], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[29], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[30], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[31]);
		rtd_pr_memc_notice("[%s][s10_me_rMV_1st_vy_rb][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[0], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[1], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[2], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[3], 
			s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[4], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[5], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[6], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[7], 
			s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[8], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[9], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[10], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[11], 
			s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[12], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[13], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[14], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[15], 
			s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[16], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[17], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[18], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[19], 
			s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[20], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[21], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[22], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[23], 
			s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[24], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[25], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[26], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[27], 
			s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[28], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[29], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[30], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[31]);
		rtd_pr_memc_notice("[%s][u12_me_rMV_1st_cnt_rb][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[0], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[1], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[2], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[3], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[4], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[5], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[6], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[7], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[8], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[9], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[10], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[11], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[12], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[13], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[14], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[15], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[16], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[17], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[18], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[19], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[20], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[21], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[22], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[23], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[24], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[25], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[26], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[27], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[28], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[29], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[30], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[31]);
#if (IC_K24)
		rtd_pr_memc_notice("[%s][u10_freq_statistic_cnt3_IIR][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[0][0], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[0][1], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[0][2], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[0][3], 
			s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[0][4], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[0][5], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[0][6], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[0][7], 
			s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[1][0], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[1][1], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[1][2], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[1][3], 
			s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[1][4], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[1][5], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[1][6], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[1][7], 
			s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[2][0], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[2][1], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[2][2], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[2][3], 
			s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[2][4], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[2][5], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[2][6], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[2][7], 
			s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[3][0], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[3][1], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[3][2], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[3][3], 
			s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[3][4], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[3][5], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[3][6], s_pContext->_output_periodic_manage.u10_freq_statistic_cnt3_IIR[3][7]);
		rtd_pr_memc_notice("[%s][mc_local_lpf_cnt][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__,
			mc_local_lpf_cnt[0], mc_local_lpf_cnt[1], mc_local_lpf_cnt[2], mc_local_lpf_cnt[3], 
			mc_local_lpf_cnt[4], mc_local_lpf_cnt[5], mc_local_lpf_cnt[6], mc_local_lpf_cnt[7], 
			mc_local_lpf_cnt[8], mc_local_lpf_cnt[9], mc_local_lpf_cnt[10], mc_local_lpf_cnt[11], 
			mc_local_lpf_cnt[12], mc_local_lpf_cnt[13], mc_local_lpf_cnt[14], mc_local_lpf_cnt[15], 
			mc_local_lpf_cnt[16], mc_local_lpf_cnt[17], mc_local_lpf_cnt[18], mc_local_lpf_cnt[19], 
			mc_local_lpf_cnt[20], mc_local_lpf_cnt[21], mc_local_lpf_cnt[22], mc_local_lpf_cnt[23], 
			mc_local_lpf_cnt[24], mc_local_lpf_cnt[25], mc_local_lpf_cnt[26], mc_local_lpf_cnt[27], 
			mc_local_lpf_cnt[28], mc_local_lpf_cnt[29], mc_local_lpf_cnt[30], mc_local_lpf_cnt[31]);
#endif
/*
		rtd_pr_memc_notice("[%s][u12_me_rMV_1st_unconf_rb][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[0], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[1], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[2], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[3], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[4], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[5], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[6], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[7], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[8], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[9], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[10], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[11], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[12], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[13], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[14], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[15], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[16], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[17], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[18], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[19], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[20], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[21], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[22], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[23], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[24], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[25], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[26], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[27], 
			s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[28], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[29], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[30], s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[31]);
		rtd_pr_memc_notice("[%s][s11_me_rMV_2nd_vx_rb][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[0], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[1], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[2], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[3], 
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[4], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[5], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[6], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[7], 
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[8], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[9], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[10], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[11], 
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[12], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[13], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[14], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[15], 
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[16], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[17], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[18], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[19], 
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[20], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[21], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[22], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[23], 
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[24], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[25], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[26], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[27], 
			s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[28], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[29], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[30], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[31]);
		rtd_pr_memc_notice("[%s][s10_me_rMV_2nd_vy_rb][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[0], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[1], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[2], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[3], 
			s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[4], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[5], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[6], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[7], 
			s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[8], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[9], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[10], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[11], 
			s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[12], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[13], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[14], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[15], 
			s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[16], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[17], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[18], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[19], 
			s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[20], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[21], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[22], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[23], 
			s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[24], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[25], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[26], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[27], 
			s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[28], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[29], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[30], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[31]);
		rtd_pr_memc_notice("[%s][u12_me_rMV_2nd_cnt_rb][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[0], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[1], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[2], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[3], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[4], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[5], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[6], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[7], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[8], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[9], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[10], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[11], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[12], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[13], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[14], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[15], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[16], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[17], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[18], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[19], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[20], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[21], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[22], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[23], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[24], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[25], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[26], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[27], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[28], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[29], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[30], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[31]);
		rtd_pr_memc_notice("[%s][u12_me_rMV_2nd_unconf_rb][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[0], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[1], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[2], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[3], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[4], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[5], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[6], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[7], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[8], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[9], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[10], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[11], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[12], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[13], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[14], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[15], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[16], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[17], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[18], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[19], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[20], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[21], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[22], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[23], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[24], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[25], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[26], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[27], 
			s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[28], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[29], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[30], s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[31]);
		rtd_pr_memc_notice("[%s][u20_me_rDTL_rb][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__,
			s_pContext->_output_read_comreg.u20_me_rDTL_rb[0], s_pContext->_output_read_comreg.u20_me_rDTL_rb[1], s_pContext->_output_read_comreg.u20_me_rDTL_rb[2], s_pContext->_output_read_comreg.u20_me_rDTL_rb[3], 
			s_pContext->_output_read_comreg.u20_me_rDTL_rb[4], s_pContext->_output_read_comreg.u20_me_rDTL_rb[5], s_pContext->_output_read_comreg.u20_me_rDTL_rb[6], s_pContext->_output_read_comreg.u20_me_rDTL_rb[7], 
			s_pContext->_output_read_comreg.u20_me_rDTL_rb[8], s_pContext->_output_read_comreg.u20_me_rDTL_rb[9], s_pContext->_output_read_comreg.u20_me_rDTL_rb[10], s_pContext->_output_read_comreg.u20_me_rDTL_rb[11], 
			s_pContext->_output_read_comreg.u20_me_rDTL_rb[12], s_pContext->_output_read_comreg.u20_me_rDTL_rb[13], s_pContext->_output_read_comreg.u20_me_rDTL_rb[14], s_pContext->_output_read_comreg.u20_me_rDTL_rb[15], 
			s_pContext->_output_read_comreg.u20_me_rDTL_rb[16], s_pContext->_output_read_comreg.u20_me_rDTL_rb[17], s_pContext->_output_read_comreg.u20_me_rDTL_rb[18], s_pContext->_output_read_comreg.u20_me_rDTL_rb[19], 
			s_pContext->_output_read_comreg.u20_me_rDTL_rb[20], s_pContext->_output_read_comreg.u20_me_rDTL_rb[21], s_pContext->_output_read_comreg.u20_me_rDTL_rb[22], s_pContext->_output_read_comreg.u20_me_rDTL_rb[23], 
			s_pContext->_output_read_comreg.u20_me_rDTL_rb[24], s_pContext->_output_read_comreg.u20_me_rDTL_rb[25], s_pContext->_output_read_comreg.u20_me_rDTL_rb[26], s_pContext->_output_read_comreg.u20_me_rDTL_rb[27], 
			s_pContext->_output_read_comreg.u20_me_rDTL_rb[28], s_pContext->_output_read_comreg.u20_me_rDTL_rb[29], s_pContext->_output_read_comreg.u20_me_rDTL_rb[30], s_pContext->_output_read_comreg.u20_me_rDTL_rb[31]);
*/
	}
}

extern unsigned char g_510_flag, g_511_flag, g_246_flag, g_476_flag;

VOID FRC_LogoDet_NearRim_dh_blklogo(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned char  NearRim_logo_status[5] = {0,0,0,0,0};
	unsigned int  NearRim_dh_blklogo_th[5] = {100,100,200,300,200};
	unsigned int u32_RB_val;
	unsigned int i = 0;
	static unsigned int fcnt =0;

	unsigned int  dh_blklogo_cnt_0_1 = 0, dh_blklogo_cnt_6_7 = 0, dh_blklogo_cnt_24_25 = 0, dh_blklogo_cnt_26_29 = 0, dh_blklogo_cnt_30_31 = 0;
	static unsigned int tmp_dh_blklogo_cnt_0_1[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static unsigned int tmp_dh_blklogo_cnt_6_7[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static unsigned int tmp_dh_blklogo_cnt_24_25[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static unsigned int tmp_dh_blklogo_cnt_26_29[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static unsigned int tmp_dh_blklogo_cnt_30_31[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned int temp_sum_blklogo_cnt_0_1 = 0,temp_sum_blklogo_cnt_6_7 = 0, temp_sum_blklogo_cnt_24_25 = 0,temp_sum_blklogo_cnt_26_29 = 0, temp_sum_blklogo_cnt_30_31 = 0;
	static unsigned int temp_blklogo_cnt_avg[5] = {0,0,0,0,0};

	ReadRegister(HARDWARE_HARDWARE_58_reg, 0, 31, &u32_RB_val);//bit 13

	dh_blklogo_cnt_0_1 = s_pContext->_output_read_comreg.u10_dh_blklogo_idx0_cnt[0]+s_pContext->_output_read_comreg.u10_dh_blklogo_idx0_cnt[1];
	dh_blklogo_cnt_6_7 = s_pContext->_output_read_comreg.u10_dh_blklogo_idx0_cnt[6]+s_pContext->_output_read_comreg.u10_dh_blklogo_idx0_cnt[7];
	dh_blklogo_cnt_24_25 = s_pContext->_output_read_comreg.u10_dh_blklogo_idx0_cnt[24]+s_pContext->_output_read_comreg.u10_dh_blklogo_idx0_cnt[25];
	dh_blklogo_cnt_26_29 = s_pContext->_output_read_comreg.u10_dh_blklogo_idx0_cnt[26]+s_pContext->_output_read_comreg.u10_dh_blklogo_idx0_cnt[27]+

							s_pContext->_output_read_comreg.u10_dh_blklogo_idx0_cnt[28]+s_pContext->_output_read_comreg.u10_dh_blklogo_idx0_cnt[29];
	dh_blklogo_cnt_30_31 = s_pContext->_output_read_comreg.u10_dh_blklogo_idx0_cnt[30]+s_pContext->_output_read_comreg.u10_dh_blklogo_idx0_cnt[31];

	tmp_dh_blklogo_cnt_0_1[16] = dh_blklogo_cnt_0_1;
	tmp_dh_blklogo_cnt_6_7[16] = dh_blklogo_cnt_6_7;
	tmp_dh_blklogo_cnt_24_25[16] = dh_blklogo_cnt_24_25;
	tmp_dh_blklogo_cnt_26_29[16] = dh_blklogo_cnt_26_29;
	tmp_dh_blklogo_cnt_30_31[16] = dh_blklogo_cnt_30_31;	
	
	for(i=0;i<16;i++){	

		//first in and first out for dh_blklogo
		tmp_dh_blklogo_cnt_0_1[i] =    	tmp_dh_blklogo_cnt_0_1[i+1]; 
		tmp_dh_blklogo_cnt_6_7[i] =	    tmp_dh_blklogo_cnt_6_7[i+1]; 
		tmp_dh_blklogo_cnt_24_25[i] =	tmp_dh_blklogo_cnt_24_25[i+1];
		tmp_dh_blklogo_cnt_26_29[i] =	tmp_dh_blklogo_cnt_26_29[i+1];
		tmp_dh_blklogo_cnt_30_31[i] =	tmp_dh_blklogo_cnt_30_31[i+1];
		//sum by temp logo cnt  for dh_blklogo
		temp_sum_blklogo_cnt_0_1 = temp_sum_blklogo_cnt_0_1 + tmp_dh_blklogo_cnt_0_1[i];	
		temp_sum_blklogo_cnt_6_7 = temp_sum_blklogo_cnt_6_7 + tmp_dh_blklogo_cnt_6_7[i];
		temp_sum_blklogo_cnt_24_25 = temp_sum_blklogo_cnt_24_25 + tmp_dh_blklogo_cnt_24_25[i];
		temp_sum_blklogo_cnt_26_29 = temp_sum_blklogo_cnt_26_29 + tmp_dh_blklogo_cnt_26_29[i];
		temp_sum_blklogo_cnt_30_31 = temp_sum_blklogo_cnt_30_31 + tmp_dh_blklogo_cnt_30_31[i];	
		
	}

	temp_blklogo_cnt_avg[0] = 	temp_sum_blklogo_cnt_0_1>>4;//top-left
	temp_blklogo_cnt_avg[1] = 	temp_sum_blklogo_cnt_6_7>>4;//opt-right
	temp_blklogo_cnt_avg[2] = 	temp_sum_blklogo_cnt_24_25>>4;//bottom-left
	temp_blklogo_cnt_avg[3] = 	temp_sum_blklogo_cnt_26_29>>4;//bottom-middle
	temp_blklogo_cnt_avg[4] = 	temp_sum_blklogo_cnt_30_31>>4;//bottom-right

	for(i=0;i<5;i++){
		
		if(temp_blklogo_cnt_avg[i]>NearRim_dh_blklogo_th[i]){
			NearRim_logo_status[i] = 1;
		}else{
			NearRim_logo_status[i] = 0;
		}		
	}
	
	for(i=0;i<5;i++){	
		
		if (NearRim_logo_status[i] == 1)
		{
			pOutput->u1_logo_NearRim_dh_blklogo_status[i] = 1;
			pOutput->u8_logo_NearRim_dh_blklogo_hold_frm[i] = 20;//pParam->u8_logo_netflix_hold_frm;
		}
		else if (pOutput->u8_logo_NearRim_dh_blklogo_hold_frm[i] > 0)
		{
			pOutput->u1_logo_NearRim_dh_blklogo_status[i] = 1;
			pOutput->u8_logo_NearRim_dh_blklogo_hold_frm[i] = pOutput->u8_logo_NearRim_dh_blklogo_hold_frm[i] - 1;
		}
		else
		{
			pOutput->u1_logo_NearRim_dh_blklogo_status[i] = 0;
			pOutput->u8_logo_NearRim_dh_blklogo_hold_frm[i] = 0;
		}

		if ( (fcnt % 10 == 0  && ((u32_RB_val >>  13) & 0x01) ==1 )){
				rtd_pr_memc_emerg("[%s][%d][,%d,][,%d,%d,%d,%d,]\n",__FUNCTION__, __LINE__,i,temp_blklogo_cnt_avg[i],NearRim_dh_blklogo_th[i],NearRim_logo_status[i],pOutput->u8_logo_NearRim_dh_blklogo_hold_frm[i]);
		}
		
	}

	if(fcnt<=0xfffA){
		fcnt++;	
	}
else{
		fcnt=0;
	}

}

VOID FRC_LgDet_LG_16s_Patch(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned char  u1_apli_status = 0;
	unsigned char  u1_aplp_status = 0;
	unsigned char  u1_gdtl_status = 0;
	unsigned char  u1_gsad_status = 0;
	unsigned char  u1_gmv_status  = 0;
	unsigned char  u1_rdtl_num_status;
	unsigned int idx;
	unsigned int rdtl_good_num  = 0;

	unsigned char  u1_lg16s_patch_status = 0;

	//unsigned int u16_cnt_totalNum = (s_pContext->_output_read_comreg.u8_me1_Vbuf_Hsize_rb * s_pContext->_output_read_comreg.u8_me1_Vbuf_Vsize_rb);
	unsigned int u16_cnt_totalNum = ME1_TotalBlkNum_Golden;	

	// apl
	unsigned int apli_avg = s_pContext->_output_read_comreg.u26_me_aAPLi_rb / u16_cnt_totalNum; // coverity ID:717588 // (u16_cnt_totalNum > 0 ) ? (s_pContext->_output_read_comreg.u26_me_aAPLi_rb / u16_cnt_totalNum) : s_pContext->_output_read_comreg.u26_me_aAPLi_rb;
	unsigned int aplp_avg = s_pContext->_output_read_comreg.u26_me_aAPLp_rb / u16_cnt_totalNum; // coverity ID:717588 // (u16_cnt_totalNum > 0 ) ? (s_pContext->_output_read_comreg.u26_me_aAPLp_rb / u16_cnt_totalNum) : s_pContext->_output_read_comreg.u26_me_aAPLp_rb;

	if (pParam->u1_logo_lg16s_patch_Gapl_en == 1)
	{
		u1_apli_status = ((apli_avg >= pParam->u10_logo_lg16s_patch_Gapl_th_l) && (apli_avg <= pParam->u10_logo_lg16s_patch_Gapl_th_h))? 1 : 0;
		u1_aplp_status = ((aplp_avg >= pParam->u10_logo_lg16s_patch_Gapl_th_l) && (aplp_avg <= pParam->u10_logo_lg16s_patch_Gapl_th_h))? 1 : 0;
	}
	else
	{
		u1_apli_status = 1;
		u1_aplp_status = 1;
	}

	// glb dtl
	if(pParam->u1_logo_lg16s_patch_Gdtl_en == 1)
	{
		u1_gdtl_status = ((s_pContext->_output_read_comreg.u25_me_aDTL_rb >= pParam->u25_logo_lg16s_patch_Gdtl_th_l) &&
			              (s_pContext->_output_read_comreg.u25_me_aDTL_rb <= pParam->u25_logo_lg16s_patch_Gdtl_th_h)) ? 1 : 0;
	}
	else
	{
		u1_gdtl_status = 1;
	}

	// glb sad
	if (pParam->u1_logo_lg16s_patch_Gsad_en == 1)
	{
		u1_gsad_status = ((s_pContext->_output_read_comreg.u30_me_aSAD_rb >= pParam->u28_logo_lg16s_patch_Gsad_th_l) &&
			              (s_pContext->_output_read_comreg.u30_me_aSAD_rb <= pParam->u28_logo_lg16s_patch_Gsad_th_h)) ? 1 : 0;
	}
	else
	{
		u1_gsad_status = 1;
	}

	// GMV

	if (pParam->u1_logo_lg16s_patch_Gmv_en == 1)
	{
		if ( (s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb >= pParam->u11_logo_lg16s_patch_Gmvx_th_l) &&
			(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb <= pParam->u11_logo_lg16s_patch_Gmvx_th_h) &&
			(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb <= pParam->u11_logo_lg16s_patch_Gmvy_th)   &&
			(s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb >= pParam->u6_logo_lg16s_patch_Gmv_cnt_th) &&
			(s_pContext->_output_read_comreg.u12_me_GMV_1st_unconf_rb <= pParam->u12_logo_lg16s_patch_Gmv_unconf_th))
		{
			u1_gmv_status = 1;
		}
		else
		{
			u1_gmv_status = 0;
		}
	}
	else
	{
		u1_gmv_status = 1;
	}

	// region dtl num
	if (pParam->u1_logo_lg16s_patch_Rdtl_num_en == 1)
	{
		for (idx = 0; idx <LOGO_RG_32; idx ++)
		{

			rdtl_good_num = ((s_pContext->_output_read_comreg.u20_me_rDTL_rb[idx] >= pParam->u20_logo_lg16s_patch_Rdtl_th_l) &&
				(s_pContext->_output_read_comreg.u20_me_rDTL_rb[idx] <= pParam->u20_logo_lg16s_patch_Rdtl_th_h)) ? (rdtl_good_num + 1) : rdtl_good_num;
		}

		u1_rdtl_num_status = rdtl_good_num >= pParam->u6_logo_lg16s_patch_Rdtl_num_th ? 1 : 0;
	}
	else
	{
		u1_rdtl_num_status = 1;
	}

	if ((pParam->u1_logo_lg16s_patch_Gapl_en == 1) ||
		(pParam->u1_logo_lg16s_patch_Gdtl_en == 1) ||
		(pParam->u1_logo_lg16s_patch_Gsad_en == 1) ||
		(pParam->u1_logo_lg16s_patch_Gmv_en == 1)  ||
		(pParam->u1_logo_lg16s_patch_Rdtl_num_en == 1))
	{
		u1_lg16s_patch_status = u1_apli_status && u1_aplp_status && u1_gdtl_status && u1_gsad_status && u1_gmv_status && u1_rdtl_num_status;
	}
	else
	{
		u1_lg16s_patch_status = 0;
	}

	pOutput->u1_logo_lg16s_patch_Gapli_status     = u1_apli_status;
	pOutput->u1_logo_lg16s_patch_Gaplp_status     = u1_aplp_status;
	pOutput->u1_logo_lg16s_patch_Gdtl_status      = u1_gdtl_status;
	pOutput->u1_logo_lg16s_patch_Gsad_status      = u1_gsad_status;
	pOutput->u1_logo_lg16s_patch_Gmv_status       = u1_gmv_status;
	pOutput->u1_logo_lg16s_patch_Rdtl_num_status  = u1_rdtl_num_status;

	if (u1_lg16s_patch_status == 1)
	{
		pOutput->u1_logo_lg16s_patch_status = 1;
		pOutput->u8_logo_lg16s_patch_hold_frm = pParam->u8_logo_lg16s_patch_hold_frm;
	}
	else if (pOutput->u8_logo_lg16s_patch_hold_frm > 0)
	{
		pOutput->u1_logo_lg16s_patch_status = 1;
		pOutput->u8_logo_lg16s_patch_hold_frm = pOutput->u8_logo_lg16s_patch_hold_frm - 1;
	}
	else
	{
		pOutput->u1_logo_lg16s_patch_status = 0;
		pOutput->u8_logo_lg16s_patch_hold_frm = 0;
	}
}

VOID FRC_LgDet_RgDhAdp_Rimunbalance(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned char u1_rg_dh_clr_rg0;
	unsigned int blk_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[26] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[27] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[28] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[29];
	unsigned int dh_clr_cnt = s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt_bot[0] + s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt_bot[1] + s_pContext->_output_read_comreg.u16_blklogo_dh_clrcnt_bot[2];

	unsigned int blk_logo_dh_clr_cnt = (blk_logo_cnt >= dh_clr_cnt)?(blk_logo_cnt - dh_clr_cnt) : 0;
	if( pParam->u1_RgDhClr_thr_en == 1 && blk_logo_dh_clr_cnt>=pParam->u16_RgDhadp_Rimunbalance_thr_l && blk_logo_dh_clr_cnt<=pParam->u16_RgDhadp_Rimunbalance_thr_h)
	{
		u1_rg_dh_clr_rg0 = 1;
	}
	else
	{
		u1_rg_dh_clr_rg0 =  0;
	}

	if (u1_rg_dh_clr_rg0 == 1)
	{
		pOutput->u1_RgDhadp_Rimunbalance_status =  1;

		pOutput->u8_rg_dh_clr_lgclr_hold_frm = pParam->u8_RgDhadp_Rimunbalance_hold_frm;
	}
	else if (pOutput->u8_RgDhadp_Rimunbalance_hold_frm > 0)
	{
		pOutput->u1_RgDhadp_Rimunbalance_status =  1;
		pOutput->u8_RgDhadp_Rimunbalance_hold_frm = pOutput->u8_RgDhadp_Rimunbalance_hold_frm - 1;
	}
	else
	{
		pOutput->u1_RgDhadp_Rimunbalance_status =  0;
	}

}


VOID FRC_LgDet_LG_UXN_Patch(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int lt_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[0]
					+ s_pContext->_output_read_comreg.u10_blklogo_rgcnt[1]
					+ s_pContext->_output_read_comreg.u10_blklogo_rgcnt[2];
	unsigned int rt_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[7];
	unsigned int md_logo_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[8]
					+ s_pContext->_output_read_comreg.u10_blklogo_rgcnt[9]
					+ s_pContext->_output_read_comreg.u10_blklogo_rgcnt[10]
					+ s_pContext->_output_read_comreg.u10_blklogo_rgcnt[11]
					+ s_pContext->_output_read_comreg.u10_blklogo_rgcnt[12]
					+ s_pContext->_output_read_comreg.u10_blklogo_rgcnt[13]
					+ s_pContext->_output_read_comreg.u10_blklogo_rgcnt[14]
					+ s_pContext->_output_read_comreg.u10_blklogo_rgcnt[15];

	unsigned char u1_lt_islogo_cond = 0;
	unsigned char u1_rt_islogo_cond = 0;
	unsigned char u1_md_nologo_cond = 0;
	unsigned char u1_bgmove_cond = 1;
	unsigned char u1_rim_cond = 1;
	unsigned char u1_rg_bright_cond = 1;

	unsigned char u1_lt_islogo_cond_en = 1;
	unsigned char u1_rt_islogo_cond_en = 1;
	unsigned char u1_md_nologo_cond_en = 1;

	unsigned char u1_lg_UXN_patch_en = pParam->u1_lg_UXN_patch_en;//(U32_DATA1 >>31); //
	unsigned char u1_lg_UXN_patch_det = 0;
	static unsigned char u8_lg_UXN_patch_cnt = 0;
	unsigned char u8_lg_UXN_patch_cnt_th = 0x8;//(U32_DATA1 & 0xff);
	unsigned char u8_lg_UXN_patch_cnt_max = 0xC;//((U32_DATA1>>8) & 0xff);

	//unsigned int U32_DATA1 = 0;
	//ReadRegister(KME_TOP_KME_TOP_34_reg, 0, 31, &U32_DATA1);
	u1_lt_islogo_cond_en = 1;//(U32_DATA1 >>30) & 0x1;
	u1_rt_islogo_cond_en = 1;//(U32_DATA1 >>29) & 0x1;
	u1_md_nologo_cond_en = 1;//(U32_DATA1 >>28) & 0x1;

	//unsigned int U32_DATA2;
	//ReadRegister(SOFTWARE_SOFTWARE_43_reg, 0, 31, &U32_DATA2);


	//unsigned int U32_DATA = 0;
	//ReadRegister(SOFTWARE_SOFTWARE_07_reg, 0, 31, &U32_DATA);

	// lt_islogo_cond
	{
		unsigned int u10_logo_cnt_th_l = 0x10<<4;//((U32_DATA & 0xff)<<4);
		unsigned int u10_logo_cnt_th_h = 0x30<<4;//((U32_DATA>>8) & 0xff)<<4;

		static unsigned int lt_logo_cnt_pre = 0;

		unsigned char u1_lt_logoCnt_cond = 0;

		u1_lt_logoCnt_cond = (lt_logo_cnt > u10_logo_cnt_th_l && lt_logo_cnt < u10_logo_cnt_th_h) ? 1 : 0;

		u1_lt_islogo_cond = (u1_lt_logoCnt_cond) ? 1 : 0;

		u1_lt_islogo_cond = (u1_lt_islogo_cond & u1_lt_islogo_cond_en) || (!u1_lt_islogo_cond_en);

		lt_logo_cnt_pre = lt_logo_cnt;
	}

	// rt_islogo_cond
	{
		unsigned int u10_logo_cnt_th2 = 0;//((U32_DATA>>24)& 0xf)<<4;
		unsigned int u10_logo_cnt_thH = 660;//((240/8)*(135/4))*2/3;

		unsigned char u1_rt_logoCnt_cond = 0;

		u1_rt_logoCnt_cond = (rt_logo_cnt > u10_logo_cnt_th2 && rt_logo_cnt < u10_logo_cnt_thH) ? 1 : 0;

		u1_rt_islogo_cond = (u1_rt_logoCnt_cond & u1_rt_islogo_cond_en) || (!u1_rt_islogo_cond_en);
	}

	// md_islogo_cond
	{
		unsigned int u10_logo_cnt_th3 = 0xf<<4;//((U32_DATA>>28) & 0xf)<<4;

		unsigned char u1_md_logoCnt_cond = 0;

		u1_md_logoCnt_cond = (md_logo_cnt < u10_logo_cnt_th3) ? 1: 0;

		u1_md_nologo_cond = (u1_md_logoCnt_cond & u1_md_nologo_cond_en) || (!u1_md_nologo_cond_en);
	}

	// bg move
	{
		//unsigned int u17_hpan_cnt = s_pContext->_output_read_comreg.u17_me_Hpan_cnt_rb;
		//unsigned int u17_vpan_cnt = s_pContext->_output_read_comreg.u17_me_Vpan_cnt_rb;
		//unsigned int u16_hvpan_th = (U32_DATA2 & 0xFFFF);

		unsigned char u1_bgmove_mode_sel = 1;//(U32_DATA2>>31)&0x1;
		unsigned char u8_bgmove_cnt_th = 0x32;//(U32_DATA2>>16)&0xff;

		unsigned char idx = 0;
		unsigned char u8_bmvx_cnt = 0;
		unsigned char u8_bmvy_cnt = 0;
		unsigned char u8_bmvx_th = 0x10;
		unsigned char u8_bmvy_th = 0xC;
		for (idx=0; idx<32; idx++)
		{
			unsigned char u1_bmvx_cond =		(_ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[idx]) > u8_bmvx_th) ||
									(_ABS_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[idx]) > u8_bmvx_th);
			unsigned char u1_bmvy_cond =		(_ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[idx]) > u8_bmvy_th) ||
									(_ABS_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[idx]) > u8_bmvy_th);
			u8_bmvx_cnt = (u1_bmvx_cond || u1_bmvy_cond) ? (u8_bmvx_cnt+1) : u8_bmvx_cnt;
			u8_bmvy_cnt = u8_bmvx_cnt;//u1_bmvy_cond ? (u8_bmvy_cnt+1) : u8_bmvy_cnt;
		}

		//u1_bgmove_cond = ((u17_hpan_cnt+u17_vpan_cnt) > u16_hvpan_th) ? 1 : 0;

		if(u1_bgmove_mode_sel==1){
			u1_bgmove_cond = ((u8_bmvx_cnt+u8_bmvy_cnt) > u8_bgmove_cnt_th) ? 1 : 0;
		}
	}

	// rim cond
	{
		unsigned char u1_rim_tb_cond = 1;
		unsigned char u1_rim_lr_cond = 1;
		unsigned char u8_rim_tb_th = Convert_UHD_Param_To_Current_RES(32, PARAM_DIRECTION_Y);//8*MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.u8_scale_V; // 4k: u8_rim_tb_th=32
		unsigned char u8_rim_lr_th = Convert_UHD_Param_To_Current_RES(4, PARAM_DIRECTION_X);//1*MEMC_PARAM_TABLE.RIM_Param[output_Resolution].RimCtrl_Param.u8_scale_H; // 4k: u8_rim_lr_th=4

		u1_rim_tb_cond = (s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_TOP] < u8_rim_tb_th)
					&& (s_pContext->_output_rimctrl.u12_out_resolution[_RIM_BOT] < (s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_BOT] + u8_rim_tb_th));
		u1_rim_lr_cond = (s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_LFT] < u8_rim_lr_th)
					&& (s_pContext->_output_rimctrl.u12_out_resolution[_RIM_RHT] < (s_pContext->_output_rimctrl.u12_Rim_Result[_RIM_RHT] + u8_rim_lr_th));

		u1_rim_cond = (u1_rim_tb_cond && u1_rim_lr_cond);
	}

	// bright cond
	{
		unsigned int u20_me_rAPLi = s_pContext->_output_read_comreg.u20_me_rAPLi_rb[7];
		unsigned int u20_me_rAPLp = s_pContext->_output_read_comreg.u20_me_rAPLp_rb[7];
		unsigned int u20_me_rAPL = (u20_me_rAPLi + u20_me_rAPLp)/2;

		unsigned int u20_me_rAPL_th = 0x40000;

		u1_rg_bright_cond = (u20_me_rAPL > u20_me_rAPL_th) ? 1 : 0;
	}

	// UXN patch detect
	if(  u1_lt_islogo_cond && u1_rt_islogo_cond && u1_md_nologo_cond
	&& u1_bgmove_cond && u1_rim_cond
	&& u1_lg_UXN_patch_en)
	{
		u8_lg_UXN_patch_cnt ++;
	}
	else
	{
		u8_lg_UXN_patch_cnt = (u8_lg_UXN_patch_cnt > 0) ? (u8_lg_UXN_patch_cnt - 1) : 0;
	}

	u8_lg_UXN_patch_cnt = _CLIP_UBOUND_(u8_lg_UXN_patch_cnt, u8_lg_UXN_patch_cnt_max);

	u1_lg_UXN_patch_det = (u8_lg_UXN_patch_cnt > u8_lg_UXN_patch_cnt_th) ? 1 : 0;


	// judge
	pOutput->u1_lg_UXN_patch_det = u1_lg_UXN_patch_det;

#if 0
	WriteRegister(KPOST_TOP_KPOST_TOP_A0_reg, 0, 3, u1_lt_islogo_cond);
	WriteRegister(KPOST_TOP_KPOST_TOP_A0_reg, 4, 7, u1_rt_islogo_cond);
	WriteRegister(KPOST_TOP_KPOST_TOP_A0_reg, 8, 11, u1_md_nologo_cond);
	WriteRegister(KPOST_TOP_KPOST_TOP_A0_reg, 12, 15, u1_bgmove_cond);

	WriteRegister(KPOST_TOP_KPOST_TOP_A0_reg, 16, 19, _MIN_(u8_lg_UXN_patch_cnt, 0xF));
	WriteRegister(KPOST_TOP_KPOST_TOP_A0_reg, 20, 23, u1_lg_UXN_patch_det);
	WriteRegister(KPOST_TOP_KPOST_TOP_A0_reg, 24, 27, u1_rg_bright_cond);
	WriteRegister(KPOST_TOP_KPOST_TOP_A0_reg, 28, 31, u1_rim_cond);
#endif

}


VOID FRC_LgDet_CloseVar(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
    const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned char u1_hw_sc_signal = s_pContext->_output_read_comreg.u1_sc_status_rb;

	if (u1_hw_sc_signal == 1)
	{
	    pOutput->u32_logo_closeVar_hold_cnt = 60;
	}
	else if (pOutput->u32_logo_closeVar_hold_cnt > 0)
	{
	    pOutput->u32_logo_closeVar_hold_cnt = pOutput->u32_logo_closeVar_hold_cnt - 1;
	}
	else
	{
	    pOutput->u32_logo_closeVar_hold_cnt = 0;
	}

}

VOID FRC_LgDet_LogoHaloEnhance(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	//const _PQLCONTEXT *s_pContext = GetPQLContext();
}

VOID FRC_LgDet_DynamicOBME(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	//const _PQLCONTEXT *s_pContext = GetPQLContext();
}

VOID FRC_LgDet_RgnDistribute(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	unsigned int iter = 0;

	//unsigned int lt_corner_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[0] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[1];
	//unsigned int rt_corner_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[6] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[7];
	//unsigned int lb_corner_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[24];
	//unsigned int rb_corner_cnt = s_pContext->_output_read_comreg.u10_blklogo_rgcnt[31];

	unsigned int top_cnt = 0;
	unsigned int bot_cnt = 0;

	for(iter=0; iter<8; iter++)
	{
		top_cnt = top_cnt + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[0+iter];
		bot_cnt = bot_cnt + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[24+iter];
	}

	if( bot_cnt > ((BLK_LOGO_RG_BLKCNT_GOLDEN*8) >> 1) )
	{
		pOutput->u1_logo_rgn_distribute[LOGO_RGN_BOT] = 1;
	}
	else
	{
		pOutput->u1_logo_rgn_distribute[LOGO_RGN_BOT] = 0;
	}

}


typedef struct dhProcLvlThr
{
	int Inc_Z2LRatio;
	int Dec_Z2LRatio;
	int Inc_Z2LDiff;
	int Dec_Z2LDiff;
	int Inc_DevVal;
	int Dec_DevVal;
} dhProcLvlThr;

VOID FRC_DhLgProc_H_MovingCase(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
#ifdef FUNC_LOG
#undef FUNC_LOG
#endif
#define FUNC_LOG 1

#if FUNC_LOG
	// print related code
	static unsigned char printkCounter = 0;
	unsigned int printkEnable = 0;
	unsigned int printkPeriod = 1;
#endif	

	unsigned char RgnIdx = 0, i = 0;
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	const unsigned char RgnIdxREMAP[32] = // current only separate the region0/1, but dehalo logo process has 0/1/2 regions
	{
		0,	0,	0,	0,	0,	0,	0,	0,
		1,	1,	1,	1,	1,	1,	1,	1,
		2,	2,	2,	2,	2,	2,	2,	2,
		3,	3,	3,	3,	3,	3,	3,	3
	};

	int s32_RgnRmvSumX[4] = {0}, s32_RgnRmvSumY[4] = {0}, s32_RgnDTLSum[4] = {0};
	static int s32_RgnRmvSumX_iir[4] = {0}, s32_RgnRmvSumY_iir[4] = {0}, s32_RgnDTLSum_iir[4] = {0};
	unsigned int  u32_RgnRmvDevX[4] = {0}, u32_RgnRmvDevY[4] = {0}, u32_RgnDTLDev[4] = {0};
	static unsigned int u32_RgnRmvDevX_iir[4] = {0}, u32_RgnRmvDevY_iir[4] = {0}, u32_RgnDTLDev_iir[4] = {0};
	unsigned int RmvDevX_iir_th = 400, RmvDevY_iir_th = 300;
	unsigned int RmvDevX_iir_stable_csae = 0, RmvDevY_iir_stable_csae = 0;
	unsigned int test_case1 = 0, test_case2 = 0, test_case3 = 0;


	for(RgnIdx=0; RgnIdx<32; RgnIdx++)
	{
		s32_RgnRmvSumX[RgnIdxREMAP[RgnIdx]] = s32_RgnRmvSumX[RgnIdxREMAP[RgnIdx]] + s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx];
		s32_RgnRmvSumY[RgnIdxREMAP[RgnIdx]] = s32_RgnRmvSumY[RgnIdxREMAP[RgnIdx]] + s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx];
		s32_RgnDTLSum[RgnIdxREMAP[RgnIdx]]	= s32_RgnDTLSum[RgnIdxREMAP[RgnIdx]]  +	s_pContext->_output_read_comreg.u20_me_rDTL_rb[RgnIdx];	
	}

	for(RgnIdx=0; RgnIdx<32; RgnIdx++)
	{
		// deviation
		if( RgnIdxREMAP[RgnIdx] == 0 )
		{
			u32_RgnRmvDevX[0] = u32_RgnRmvDevX[0] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*8,s32_RgnRmvSumX[0]);
			u32_RgnRmvDevY[0] = u32_RgnRmvDevY[0] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*8,s32_RgnRmvSumY[0]);
			u32_RgnDTLDev[0] = u32_RgnDTLDev[0] + _ABS_DIFF_(s_pContext->_output_read_comreg.u20_me_rDTL_rb[RgnIdx]*8,s32_RgnDTLSum[0]);
		}
		else if( RgnIdxREMAP[RgnIdx] == 1 )
		{
			u32_RgnRmvDevX[1] = u32_RgnRmvDevX[1] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*8,s32_RgnRmvSumX[1]);
			u32_RgnRmvDevY[1] = u32_RgnRmvDevY[1] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*8,s32_RgnRmvSumY[1]);
			u32_RgnDTLDev[1] = u32_RgnDTLDev[1] + _ABS_DIFF_(s_pContext->_output_read_comreg.u20_me_rDTL_rb[RgnIdx]*8,s32_RgnDTLSum[1]);		
		}
		else if( RgnIdxREMAP[RgnIdx] == 2 )
		{
			u32_RgnRmvDevX[2] = u32_RgnRmvDevX[2] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*8,s32_RgnRmvSumX[2]);
			u32_RgnRmvDevY[2] = u32_RgnRmvDevY[2] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*8,s32_RgnRmvSumY[2]);
			u32_RgnDTLDev[2] = u32_RgnDTLDev[2] + _ABS_DIFF_(s_pContext->_output_read_comreg.u20_me_rDTL_rb[RgnIdx]*8,s32_RgnDTLSum[2]);		
		}
		else if( RgnIdxREMAP[RgnIdx] == 3 )
		{
			u32_RgnRmvDevX[3] = u32_RgnRmvDevX[3] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*8,s32_RgnRmvSumX[3]);
			u32_RgnRmvDevY[3] = u32_RgnRmvDevY[3] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*8,s32_RgnRmvSumY[3]);
			u32_RgnDTLDev[3] = u32_RgnDTLDev[3] + _ABS_DIFF_(s_pContext->_output_read_comreg.u20_me_rDTL_rb[RgnIdx]*8,s32_RgnDTLSum[3]);	
		}

#if 0//FUNC_LOG
		ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 29, 29, &printkEnable);
		ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 24, 27, &printkPeriod);
		printkPeriod = (printkPeriod+1);//*8;
		
		if( printkCounter == 0 && printkEnable == 1 )
		{
			rtd_pr_memc_debug("DEV_%d_u32_RgnDTLDev = ( %d  ) ( %d ) ( %d , %d) (%d)\n",RgnIdx,u32_RgnDTLDev[RgnIdxREMAP[RgnIdx]],
						RgnIdxREMAP[RgnIdx], s_pContext->_output_read_comreg.u20_me_rDTL_rb[RgnIdx], s32_RgnDTLSum[RgnIdxREMAP[RgnIdx]],
						_ABS_DIFF_(s_pContext->_output_read_comreg.u20_me_rDTL_rb[RgnIdx]*8,s32_RgnDTLSum[RgnIdxREMAP[RgnIdx]]));	
		}
		
#endif	
		
		
	}

	// iir process
	for(i=0; i<4; i++)
	{
		u32_RgnRmvDevX_iir[i] = (u32_RgnRmvDevX_iir[i] + u32_RgnRmvDevX[i]) >> 1;
		u32_RgnRmvDevY_iir[i] = (u32_RgnRmvDevY_iir[i] + u32_RgnRmvDevY[i]) >> 1;
		u32_RgnDTLDev_iir[i]  = (u32_RgnDTLDev_iir[i]  + u32_RgnDTLDev[i] ) >> 1;
		s32_RgnRmvSumX_iir[i] = (s32_RgnRmvSumX_iir[i] + s32_RgnRmvSumX[i]) >> 1;
		s32_RgnRmvSumY_iir[i] = (s32_RgnRmvSumY_iir[i] + s32_RgnRmvSumY[i]) >> 1;		
		s32_RgnDTLSum_iir[i] = (s32_RgnDTLSum_iir[i] + s32_RgnDTLSum[i]) >> 1;

		if(u32_RgnRmvDevX_iir[i] < RmvDevX_iir_th){
		 	RmvDevX_iir_stable_csae++;
		}

		if(u32_RgnRmvDevY_iir[i] < RmvDevY_iir_th){
			RmvDevY_iir_stable_csae++;
		}

	}


	if((((s32_RgnDTLSum_iir[0]>400000) && (s32_RgnDTLSum_iir[0]<=700000)&&(u32_RgnDTLDev_iir[0]>1500000) && (u32_RgnDTLDev_iir[0]<=2200000)) &&
		 ((s32_RgnDTLSum_iir[1]>650000) && (s32_RgnDTLSum_iir[1]<=860000)&&(u32_RgnDTLDev_iir[1]>400000)  && (u32_RgnDTLDev_iir[1]<=1100000)) &&
		 ((s32_RgnDTLSum_iir[2]>500000) && (s32_RgnDTLSum_iir[2]<=740000)&&(u32_RgnDTLDev_iir[2]>530000)  && (u32_RgnDTLDev_iir[2]<=1200000)) &&
		 ((s32_RgnDTLSum_iir[3]>160000) && (s32_RgnDTLSum_iir[3]<=360000)&&(u32_RgnDTLDev_iir[3]>450000)  && (u32_RgnDTLDev_iir[3]<=1200000))	)
		 &&
		 ((RmvDevX_iir_stable_csae >= 3) && (RmvDevY_iir_stable_csae >= 3) && (u32_RgnRmvDevX_iir[3] > 800))
		 &&
		 (((s32_RgnRmvSumX_iir[0] > -60 && s32_RgnRmvSumX_iir[0] <= -10)&&(s32_RgnRmvSumY_iir[0] > -20 && s32_RgnRmvSumY_iir[0] <= 50)) &&
		 ((s32_RgnRmvSumX_iir[1] > -60 && s32_RgnRmvSumX_iir[1] <= -10)&&(s32_RgnRmvSumY_iir[1] > -25 && s32_RgnRmvSumY_iir[1] <= 50)) &&
		 ((s32_RgnRmvSumX_iir[2] > -80 && s32_RgnRmvSumX_iir[2] <= -10)&&(s32_RgnRmvSumY_iir[2] > -25 && s32_RgnRmvSumY_iir[2] <= 40)) &&
		 ((s32_RgnRmvSumX_iir[3] > -800 && s32_RgnRmvSumX_iir[3] <= -300)&&(s32_RgnRmvSumY_iir[3] > -160 && s32_RgnRmvSumY_iir[3] <= 0))	))
	{
		pOutput->u3_logo_dhRgnProc_H_Moving_case = 1;
	}else if((s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[7]>60 && s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[7]<190) &&
			  (s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[7]>-15 && s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[7]<0) &&
			  ((s32_RgnRmvSumX_iir[0] > 600 && s32_RgnRmvSumX_iir[0] <= 1700)&&(s32_RgnRmvSumY_iir[0] > -80 && s32_RgnRmvSumY_iir[0] <= 0)) && 
			  ((s32_RgnDTLSum_iir[0]>250000) && (s32_RgnDTLSum_iir[0]<=450000)&&(u32_RgnDTLDev_iir[0]>300000) && (u32_RgnDTLDev_iir[0]<=550000)))
	{
		pOutput->u3_logo_dhRgnProc_H_Moving_case = 2;
	}else{
		pOutput->u3_logo_dhRgnProc_H_Moving_case = 0;
	}

	if(((s32_RgnDTLSum_iir[0]>400000) && (s32_RgnDTLSum_iir[0]<=700000)&&(u32_RgnDTLDev_iir[0]>1500000) && (u32_RgnDTLDev_iir[0]<=2200000)) &&
		 ((s32_RgnDTLSum_iir[1]>650000) && (s32_RgnDTLSum_iir[1]<=860000)&&(u32_RgnDTLDev_iir[1]>400000)  && (u32_RgnDTLDev_iir[1]<=1100000)) &&
		 ((s32_RgnDTLSum_iir[2]>500000) && (s32_RgnDTLSum_iir[2]<=740000)&&(u32_RgnDTLDev_iir[2]>530000)  && (u32_RgnDTLDev_iir[2]<=1200000)) &&
		 ((s32_RgnDTLSum_iir[3]>160000) && (s32_RgnDTLSum_iir[3]<=360000)&&(u32_RgnDTLDev_iir[3]>450000)  && (u32_RgnDTLDev_iir[3]<=1200000))	)
	{
		test_case1 = 1;
	}

	if((RmvDevX_iir_stable_csae >= 3) && (RmvDevY_iir_stable_csae >= 3) && (u32_RgnRmvDevX_iir[3] > 800))
	{
		test_case2 = 1;
	}

	if(((s32_RgnRmvSumX_iir[0] > -60 && s32_RgnRmvSumX_iir[0] <= -10)&&(s32_RgnRmvSumY_iir[0] > -20 && s32_RgnRmvSumY_iir[0] <= 50)) &&
		 ((s32_RgnRmvSumX_iir[1] > -60 && s32_RgnRmvSumX_iir[1] <= -10)&&(s32_RgnRmvSumY_iir[1] > -25 && s32_RgnRmvSumY_iir[1] <= 50)) &&
		 ((s32_RgnRmvSumX_iir[2] > -80 && s32_RgnRmvSumX_iir[2] <= -10)&&(s32_RgnRmvSumY_iir[2] > -25 && s32_RgnRmvSumY_iir[2] <= 40)) &&
		 ((s32_RgnRmvSumX_iir[3] > -800 && s32_RgnRmvSumX_iir[3] <= -300)&&(s32_RgnRmvSumY_iir[3] > -160 && s32_RgnRmvSumY_iir[3] <= 0))	)
	{
		test_case3 = 1;
	}



#if FUNC_LOG
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 29, 29, &printkEnable);
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 24, 27, &printkPeriod);
	printkPeriod = (printkPeriod+1);//*8;

	if( printkCounter == 0 && printkEnable == 1 )
	{
		rtd_pr_memc_debug("H_Moving_case = %5d ( %d, %d, %d,)\n",pOutput->u3_logo_dhRgnProc_H_Moving_case, test_case1, test_case2, test_case3);	
		if(pOutput->u3_logo_dhRgnProc_H_Moving_case!=2){
			rtd_pr_memc_debug("H_Moving_case_2 = (, %d, %d, %d, %d, %d, %d,)\n",
				s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[7],s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[7],
			  	s32_RgnRmvSumX_iir[0],s32_RgnRmvSumY_iir[0], s32_RgnDTLSum_iir[0],u32_RgnDTLDev_iir[0]	);
		}	
	}
		
				
	if( printkCounter == 0 && printkEnable == 1 )
	{
		rtd_pr_memc_debug("[DEV]RgnDevX = ( %5d %5d %5d %5d ), RgnDevY = ( %5d %5d %5d %5d ), RgnDevDTL = ( %5d %5d %5d %5d )\n",
				u32_RgnRmvDevX[0],u32_RgnRmvDevX[1],u32_RgnRmvDevX[2],u32_RgnRmvDevX[3],
				u32_RgnRmvDevY[0],u32_RgnRmvDevY[1],u32_RgnRmvDevY[2],u32_RgnRmvDevY[3],
				u32_RgnDTLDev[0],u32_RgnDTLDev[1],u32_RgnDTLDev[2],u32_RgnDTLDev[3] );	

		rtd_pr_memc_debug("[DEV_iir]RgnDevX_iir = ( %5d %5d %5d %5d ), RgnDevY_iir = ( %5d %5d %5d %5d ), RgnDevDTL_iir = ( %5d %5d %5d %5d )\n",
				u32_RgnRmvDevX_iir[0],u32_RgnRmvDevX_iir[1],u32_RgnRmvDevX_iir[2],u32_RgnRmvDevX_iir[3],
				u32_RgnRmvDevY_iir[0],u32_RgnRmvDevY_iir[1],u32_RgnRmvDevY_iir[2],u32_RgnRmvDevY_iir[3],
				u32_RgnDTLDev_iir[0],u32_RgnDTLDev_iir[1],u32_RgnDTLDev_iir[2],u32_RgnDTLDev_iir[3] );	
	

		rtd_pr_memc_debug("[Sum]RmvSumX = ( %5d %5d %5d %5d ), RmvSumY = ( %5d %5d %5d %5d ), DTLSum = ( %5d %5d %5d %5d )\n",
				s32_RgnRmvSumX[0],s32_RgnRmvSumX[1],s32_RgnRmvSumX[2],s32_RgnRmvSumX[3],
				s32_RgnRmvSumY[0],s32_RgnRmvSumY[1],s32_RgnRmvSumY[2],s32_RgnRmvSumY[3],
				s32_RgnDTLSum[0],s32_RgnDTLSum[1],s32_RgnDTLSum[2],s32_RgnDTLSum[3]);	
		
		rtd_pr_memc_debug("[Sum_iir]RmvSumX_iir = ( %5d %5d %5d %5d ), RmvSumY_iir = ( %5d %5d %5d %5d ), SumDTL_iir = ( %5d %5d %5d %5d )\n",
				s32_RgnRmvSumX_iir[0],s32_RgnRmvSumX_iir[1],s32_RgnRmvSumX_iir[2],s32_RgnRmvSumX_iir[3],
				s32_RgnRmvSumY_iir[0],s32_RgnRmvSumY_iir[1],s32_RgnRmvSumY_iir[2],s32_RgnRmvSumY_iir[3],
				s32_RgnDTLSum_iir[0],s32_RgnDTLSum_iir[1],s32_RgnDTLSum_iir[2],s32_RgnDTLSum_iir[3]);	
				
				
	}

	printkCounter = printkCounter + 1;
	if( printkCounter >= printkPeriod )
		printkCounter = 0;
	
#endif	


#ifdef FUNC_LOG
#undef FUNC_LOG
#endif

}



#if RTK_MEMC_Performance_tunging_from_tv001

VOID FRC_DhLgProc_DynamicLevel(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
#ifdef FUNC_LOG
#undef FUNC_LOG
#endif
#define FUNC_LOG 1

#if FUNC_LOG
	// print related code
	static unsigned char printkCounter = 0;
	unsigned int printkEnable = 0;
	unsigned int printkPeriod = 1;
	unsigned int u1_force_dhProc_lvl_en[6] = {0};
	unsigned int u3_force_dhProc_lvl[6] = {0};
#endif


	const _PQLCONTEXT *s_pContext = GetPQLContext();
	const unsigned char RgnIdxREMAP[32] = // current only separate the region0/1, but dehalo logo process has 0/1/2 regions
	{
		0,	0,	1,	1,	1,	1,	2,	2,
		1,	1,	1,	1,	1,	1,	1,	1,
		1,	1,	1,	1,	1,	1,	1,	1,
		3,	3,	1,	4,	4,	1,	5,	5
	};

	unsigned char RgnIdx = 0, i = 0;
	static unsigned int u32_RgnZmvCnt[32] = {0};
	unsigned int u32_1stGmvZDiff = 0;
	unsigned int u32_2ndGmvZDiff = 0;
	unsigned int ZmvDiffThr = 6;

	unsigned int u16_LogoCnt[6] = {0};
	unsigned int u16_ZmvCnt[6] = {0};
	static unsigned int u16_LogoCnt_iir[6] = {0};
	static unsigned int u16_ZmvCnt_iir[6] = {0};
	unsigned int u32_SumLogoCnt = 0;
	unsigned int u32_SumZmvCnt = 0;

	int s32_RgnRmvSumX[6] = {0}, s32_RgnRmvSumY[6] = {0};
	unsigned int u32_RgnRmvDevX[6] = {0}, u32_RgnRmvDevY[6] = {0};
	static unsigned int u32_RgnRmvDevX_iir[6] = {0}, u32_RgnRmvDevY_iir[6] = {0};
	static unsigned int s32_RgnRmvSumX_iir[6] = {0}, s32_RgnRmvSumY_iir[6] = {0};

	int s32_ZmvLogoDiff = 0;
	const dhProcLvlThr LvlThr[3] =
	{
		// /*DC = don't care*/
		// level up
		// 1. ZmvCnt <= LogoCnt * Z2L_IncRatio/64
		// 2. ZmvCnt - LogoCnt <= Inc_Z2LDiff
		// 3. Mv Deviation <= Inc_DevVal
		// level down
		// 1. ZmvCnt >= LogoCnt * (1+(Z2L_IncRatio/64))
		// 2. ZmvCnt - LogoCnt >= IncAbsVal
		// 3. Mv Deviation >= Dec_DevVal
		//		Inc_Z2LRatio	Dec_Z2LRatio	Inc_Z2LDiff	Dec_Z2LDiff	Inc_DevVal	Dec_DevVal
		/*0*/ {	50,				0/*DC*/,		-600,		0/*DC*/,	6400,		0/*DC*/	}, // default level (weak)
		/*1*/ {	5,				68,				-3000,		400,		2800,		6600	},
		/*2*/ {	0/*DC*/,		12,				0/*DC*/,	-2800,		0/*DC*/,	3000	}  // strong (strictly panning)
	};


#if FUNC_LOG
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 14, 17, &ZmvDiffThr);
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 29, 29, &printkEnable);
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 24, 27, &printkPeriod);
	printkPeriod = (printkPeriod+1);//*8;

	for(RgnIdx=0; RgnIdx<32; RgnIdx++)
	{
		// 240*135 domain
		u32_1stGmvZDiff = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]) + _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]);
		u32_2ndGmvZDiff = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[RgnIdx]) + _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[RgnIdx]);

		/*if( printkCounter == 0 && printkEnable == 1 )
		{
			rtd_pr_memc_info("[%2d] z1=%5d (%5d %5d) (%5d) (%4d) z2=%5d (%5d %5d) (%5d) (%4d) p=%4d t=%4d sad=%4d %d\n",
				RgnIdx,
				s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx], u32_1stGmvZDiff, s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[RgnIdx],
				s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[RgnIdx], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[RgnIdx], u32_2ndGmvZDiff, s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[RgnIdx],
				s_pContext->_output_read_comreg.u13_me_rPRD_rb[RgnIdx], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx]+s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx]+s_pContext->_output_read_comreg.u13_me_rPRD_rb[RgnIdx],
				s_pContext->_output_read_comreg.u25_me_rSAD_rb[RgnIdx]/1020, s_pContext->_output_read_comreg.u25_me_rSAD_rb[RgnIdx]
			);
		}*/
	}
#endif

	for(RgnIdx=0; RgnIdx<32; RgnIdx++)
	{
		s32_RgnRmvSumX[RgnIdxREMAP[RgnIdx]] = s32_RgnRmvSumX[RgnIdxREMAP[RgnIdx]] + s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx];
		s32_RgnRmvSumY[RgnIdxREMAP[RgnIdx]] = s32_RgnRmvSumY[RgnIdxREMAP[RgnIdx]] + s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx];
	}

	for(RgnIdx=0; RgnIdx<32; RgnIdx++)
	{
		// deviation
		if( RgnIdxREMAP[RgnIdx] == 0 )
		{
			u32_RgnRmvDevX[0] = u32_RgnRmvDevX[0] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*2,s32_RgnRmvSumX[0]);
			u32_RgnRmvDevY[0] = u32_RgnRmvDevY[0] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*2,s32_RgnRmvSumY[0]);
		}		
		else if( RgnIdxREMAP[RgnIdx] == 2 )
		{
			u32_RgnRmvDevX[2] = u32_RgnRmvDevX[2] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*2,s32_RgnRmvSumX[2]);
			u32_RgnRmvDevY[2] = u32_RgnRmvDevY[2] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*2,s32_RgnRmvSumY[2]);		
		}
		else if( RgnIdxREMAP[RgnIdx] == 3 )
		{
			u32_RgnRmvDevX[3] = u32_RgnRmvDevX[3] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*2,s32_RgnRmvSumX[3]);
			u32_RgnRmvDevY[3] = u32_RgnRmvDevY[3] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*2,s32_RgnRmvSumY[3]);	
		}
		else if( RgnIdxREMAP[RgnIdx] == 4 )
		{
			u32_RgnRmvDevX[4] = u32_RgnRmvDevX[4] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*2,s32_RgnRmvSumX[4]);
			u32_RgnRmvDevY[4] = u32_RgnRmvDevY[4] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*2,s32_RgnRmvSumY[4]);			
		}	
		else if( RgnIdxREMAP[RgnIdx] == 5 )	
		{
			u32_RgnRmvDevX[5] = u32_RgnRmvDevX[5] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*2,s32_RgnRmvSumX[5]);
			u32_RgnRmvDevY[5] = u32_RgnRmvDevY[5] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*2,s32_RgnRmvSumY[5]);			
		}
		else
		{
			u32_RgnRmvDevX[1] = u32_RgnRmvDevX[1] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*22,s32_RgnRmvSumX[1]);
			u32_RgnRmvDevY[1] = u32_RgnRmvDevY[1] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*22,s32_RgnRmvSumY[1]);
		}

		// 240*135 domain
		u32_1stGmvZDiff = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]) + _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]);
		u32_2ndGmvZDiff = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[RgnIdx]) + _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[RgnIdx]);

		// there are only 990 block at last statistic region
		if( RgnIdx>=24 )
			u32_RgnZmvCnt[RgnIdx] = 990;
		else
			u32_RgnZmvCnt[RgnIdx] = 1020;

		// guess the zmv amount
		if( u32_1stGmvZDiff > ZmvDiffThr && s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx] != 0 )
		{
			u32_RgnZmvCnt[RgnIdx] = (u32_RgnZmvCnt[RgnIdx] >= s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx])? u32_RgnZmvCnt[RgnIdx]-s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx] : 0;
		}
		if( u32_2ndGmvZDiff > ZmvDiffThr && s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx] != 0 )
		{
			u32_RgnZmvCnt[RgnIdx] = (u32_RgnZmvCnt[RgnIdx] >= s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx])? u32_RgnZmvCnt[RgnIdx]-s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx] : 0;
		}
		u32_RgnZmvCnt[RgnIdx] = (u32_RgnZmvCnt[RgnIdx] >= s_pContext->_output_read_comreg.u13_me_rPRD_rb[RgnIdx])? u32_RgnZmvCnt[RgnIdx]-s_pContext->_output_read_comreg.u13_me_rPRD_rb[RgnIdx] :  0;

		// add the logo count and zmv count to each region counter
		u16_LogoCnt[RgnIdxREMAP[RgnIdx]] = u16_LogoCnt[RgnIdxREMAP[RgnIdx]] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[RgnIdx];
		u16_ZmvCnt[RgnIdxREMAP[RgnIdx]] = u16_ZmvCnt[RgnIdxREMAP[RgnIdx]] + u32_RgnZmvCnt[RgnIdx];

#if FUNC_LOG
		/*if( printkCounter == 0 && printkEnable == 1 )
		{
			rtd_pr_memc_info("[%2d] z=%4d l=%4d ", RgnIdx, u32_RgnZmvCnt[RgnIdx], s_pContext->_output_read_comreg.u10_blklogo_rgcnt[RgnIdx]);
			if( (RgnIdx+1)%8 == 0 )
				rtd_pr_memc_info("\n");
		}*/
#endif

	}

	// iir process
	for(i=0; i<6; i++)
	{
		u16_LogoCnt_iir[i] = (u16_LogoCnt[i] + u16_LogoCnt_iir[i]) >> 1;
		u16_ZmvCnt_iir[i] = (u16_ZmvCnt[i] + u16_ZmvCnt_iir[i]) >> 1;
		u32_SumLogoCnt = u32_SumLogoCnt + u16_LogoCnt_iir[i];
		u32_SumZmvCnt = u32_SumZmvCnt + u16_ZmvCnt_iir[i];
		u32_RgnRmvDevX_iir[i] = (u32_RgnRmvDevX_iir[i] + u32_RgnRmvDevX[i]) >> 1;
		u32_RgnRmvDevY_iir[i] = (u32_RgnRmvDevY_iir[i] + u32_RgnRmvDevY[i]) >> 1;
		s32_RgnRmvSumX_iir[i] = (s32_RgnRmvSumX_iir[i] + s32_RgnRmvSumX[i]) >> 1;
		s32_RgnRmvSumY_iir[i] = (s32_RgnRmvSumY_iir[i] + s32_RgnRmvSumY[i]) >> 1;
	}
	s32_ZmvLogoDiff = (u32_SumZmvCnt-u32_SumLogoCnt);

#if FUNC_LOG
#if 0
		if( printkCounter == 0 && printkEnable == 1 )
		{	
			rtd_pr_memc_info("SumZ=%5d, SumL=%5d, Diff=%d\n", u32_SumZmvCnt, u32_SumLogoCnt, s32_ZmvLogoDiff);
			rtd_pr_memc_info("RgnZ=%5d %5d %5d %5d %5d %5d,\n",u16_ZmvCnt_iir[0], u16_ZmvCnt_iir[1],u16_ZmvCnt_iir[2],u16_ZmvCnt_iir[3],u16_ZmvCnt_iir[4],u16_ZmvCnt_iir[5]);
			rtd_pr_memc_info("RgnL=%5d %5d %5d %5d %5d %5d,\n",u16_LogoCnt_iir[0], u16_LogoCnt_iir[1],u16_LogoCnt_iir[2], u16_LogoCnt_iir[3],u16_LogoCnt_iir[4], u16_LogoCnt_iir[5] );
	#if 1
			rtd_pr_memc_info("RgnDevX = ( %5d %5d %5d %5d %5d %5d ), RgnDevY = ( %5d %5d %5d %5d %5d %5d )\n",
					u32_RgnRmvDevX[0],u32_RgnRmvDevX[1],u32_RgnRmvDevX[2],u32_RgnRmvDevX[3],u32_RgnRmvDevX[4],u32_RgnRmvDevX[5],
					u32_RgnRmvDevY[0],u32_RgnRmvDevY[1],u32_RgnRmvDevY[2],u32_RgnRmvDevY[3],u32_RgnRmvDevY[4],u32_RgnRmvDevY[5]);	
		
			rtd_pr_memc_info("RmvSumX = ( %5d %5d %5d %5d %5d %5d ), RmvSumY = ( %5d %5d %5d %5d %5d %5d )\n",
					s32_RgnRmvSumX_iir[0],s32_RgnRmvSumX_iir[1],s32_RgnRmvSumX_iir[2],s32_RgnRmvSumX_iir[3],s32_RgnRmvSumX_iir[4],s32_RgnRmvSumX_iir[5],
					s32_RgnRmvSumY_iir[0],s32_RgnRmvSumY_iir[1],s32_RgnRmvSumY_iir[2],s32_RgnRmvSumY_iir[3],s32_RgnRmvSumY_iir[4],s32_RgnRmvSumY_iir[5]);				
			
			rtd_pr_memc_info("RgnDeviir = (%5d %5d ) %5d, (%5d %5d ) %5d, (%5d %5d ) %5d, (%5d %5d ) %5d, (%5d %5d ) %5d, (%5d %5d ) %5d\n",
				u32_RgnRmvDevX_iir[0]*16, u32_RgnRmvDevY_iir[0]*16, (u32_RgnRmvDevX_iir[0]+u32_RgnRmvDevY_iir[0])*16,(u32_RgnRmvDevX_iir[1]*16)/11, (u32_RgnRmvDevY_iir[1]*16)/11, ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*16)/11,
				u32_RgnRmvDevX_iir[2]*16, u32_RgnRmvDevY_iir[2]*16, (u32_RgnRmvDevX_iir[2]+u32_RgnRmvDevY_iir[2])*16,u32_RgnRmvDevX_iir[3]*16, u32_RgnRmvDevY_iir[3]*16, (u32_RgnRmvDevX_iir[3]+u32_RgnRmvDevY_iir[3])*16,
				u32_RgnRmvDevX_iir[4]*16, u32_RgnRmvDevY_iir[4]*16, (u32_RgnRmvDevX_iir[4]+u32_RgnRmvDevY_iir[4])*16,u32_RgnRmvDevX_iir[5]*16, u32_RgnRmvDevY_iir[5]*16, (u32_RgnRmvDevX_iir[5]+u32_RgnRmvDevY_iir[5])*16
				);
	#else
		
			rtd_pr_memc_info("RgnDev = (%5d %5d) %5d (%5d %5d) %5d\n",
				u32_RgnRmvDevX_iir[0]*4, u32_RgnRmvDevY_iir[0]*4, (u32_RgnRmvDevX_iir[0]+u32_RgnRmvDevY_iir[0])*4,
				u32_RgnRmvDevX_iir[1]*4/3, u32_RgnRmvDevY_iir[1]*4/3, (u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*4/3
			);
	#endif	
			rtd_pr_memc_info("rgnclv=%1d %1d %1d %1d %1d %1d\n", pOutput->u3_logo_dhRgnProc_lvl[0], pOutput->u3_logo_dhRgnProc_lvl[1], pOutput->u3_logo_dhRgnProc_lvl[2], pOutput->u3_logo_dhRgnProc_lvl[3]
										, pOutput->u3_logo_dhRgnProc_lvl[4], pOutput->u3_logo_dhRgnProc_lvl[5]);
		}
#endif
#endif
		//RgnProc_case control
		if((u16_LogoCnt_iir[2]>200 && u16_LogoCnt_iir[1] < 100) && (u16_ZmvCnt_iir[2]<=20) && (u32_RgnRmvDevX[2]<=30 && u32_RgnRmvDevY[2]<=20))
		{
			pOutput->u3_logo_dhRgnProc_case[2] = 1;
			
		}else if((u16_LogoCnt_iir[0]<60 && u16_LogoCnt_iir[1]>=190 && u16_LogoCnt_iir[2]<110 && u16_LogoCnt_iir[4]<10) && 
				 (u16_LogoCnt_iir[3]>130 && u16_LogoCnt_iir[3]<=350) && 
				 (u16_LogoCnt_iir[5]>250 && u16_LogoCnt_iir[5]<550)){
	
			pOutput->u3_logo_dhRgnProc_case[3] = 1;
			
		}else{
			
			for(i=0; i<6; i++){
				pOutput->u3_logo_dhRgnProc_case[i] = 0;
			}		
	
		}
#if FUNC_LOG
		if( printkCounter == 0 && printkEnable == 1 )
		{
			/*rtd_pr_memc_info("hRgnProc_case = ( %5d %5d %5d %5d %5d %5d )\n",
					pOutput->u3_logo_dhRgnProc_case[0],pOutput->u3_logo_dhRgnProc_case[1],pOutput->u3_logo_dhRgnProc_case[2],pOutput->u3_logo_dhRgnProc_case[3],
					pOutput->u3_logo_dhRgnProc_case[4],pOutput->u3_logo_dhRgnProc_case[5]);*/
		}	
#endif


	// region condition
	if( u32_SumZmvCnt >= (((240*135)*14)>>4) || s_pContext->_output_me_sceneAnalysis.u2_panning_flag !=1 || 
		(s_pContext->_output_read_comreg.u10_blklogo_rgcnt[1]<=400 && s_pContext->_output_read_comreg.u10_blklogo_rgcnt[7]<=400)) // still image and non-panning
	{
		for(i=0; i<6; i++){
			pOutput->u3_logo_dhRgnProc_lvl[i] = 0;
		}
	}
	else
	{
		// region 0 condition

		// region 1 condition
		// deviation very large
		// some region with logo and zmv appears
#if FUNC_LOG
		if( printkCounter == 0 && printkEnable == 1 )
		{
			/*
			rtd_pr_memc_info("inc %1d %1d %1d\n",
				( (u16_ZmvCnt_iir[1]*64) <= (u16_LogoCnt_iir[1]*LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_Z2LRatio) ),
				( s32_ZmvLogoDiff <= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_Z2LDiff ),
				( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*16/11) <= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_DevVal )
			);
			rtd_pr_memc_info("dec %1d %1d %1d\n",
				( (u16_ZmvCnt_iir[1]*64) >= (u16_LogoCnt_iir[1]*LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_Z2LRatio) ),
				( s32_ZmvLogoDiff >= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_Z2LDiff ),
				( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*16/11) >= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_DevVal )
			);
			*/
		}
#endif
		if( (u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*4/3 > 8300 ) // complex scene
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = 0;
		}
		else if( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*4/3) <= 200 ) // bamtori
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = 2;
		}
		else if( ( (u16_ZmvCnt_iir[1]*64) <= (u16_LogoCnt_iir[1]*LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_Z2LRatio) ) &&
			( s32_ZmvLogoDiff <= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_Z2LDiff ) &&
			( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*4/3) <= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_DevVal ) ) // inc
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = (pOutput->u3_logo_dhRgnProc_lvl[1]<2)? pOutput->u3_logo_dhRgnProc_lvl[1]+1 : 2;
		}
		else if( ( (u16_ZmvCnt_iir[1]*64) >= (u16_LogoCnt_iir[1]*LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_Z2LRatio) ) &&
				 ( s32_ZmvLogoDiff >= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_Z2LDiff ) &&
				 ( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*4/3) >= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_DevVal ) ) // dec
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = (pOutput->u3_logo_dhRgnProc_lvl[1]>1) ? pOutput->u3_logo_dhRgnProc_lvl[1]-1 : 0;
		}

		// temporally make it the same
		for(i=0; i<6; i++){
			if( (pOutput->u3_logo_dhRgnProc_lvl[i]!=pOutput->u3_logo_dhRgnProc_lvl[1]) && pOutput->u3_logo_dhRgnProc_lvl[1]!=0 ){
				pOutput->u3_logo_dhRgnProc_lvl[i] = pOutput->u3_logo_dhRgnProc_lvl[1];
			}
		}

	}

#if FUNC_LOG
#if 0
	
		ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 23, 23, &u1_force_dhProc_lvl_en[0]);
		ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 22, 22, &u1_force_dhProc_lvl_en[1]);
	
		ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 20, 21, &u3_force_dhProc_lvl[0]);
		ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 18, 19, &u3_force_dhProc_lvl[1]);	
		if( u1_force_dhProc_lvl_en[0] != 0 )
		{
			pOutput->u3_logo_dhRgnProc_lvl[0] = _MIN_(u3_force_dhProc_lvl[0], 2);
		}
		if( u1_force_dhProc_lvl_en[1] != 0 )
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = _MIN_(u3_force_dhProc_lvl[1], 2);
		}
	
			if( printkCounter == 0 && printkEnable == 1 )
		{
			rtd_pr_memc_info("olv=%1d %1d, force=%1d %1d\n", pOutput->u3_logo_dhRgnProc_lvl[0], pOutput->u3_logo_dhRgnProc_lvl[1], u1_force_dhProc_lvl_en[0], u1_force_dhProc_lvl_en[1]);
		}
		
	
#else
		ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 23, 23, &u1_force_dhProc_lvl_en[0]);
		ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 22, 22, &u1_force_dhProc_lvl_en[1]);
		ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 0, 0, &u1_force_dhProc_lvl_en[2]);
		ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 1, 1, &u1_force_dhProc_lvl_en[3]);
		ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 2, 2, &u1_force_dhProc_lvl_en[4]);
		ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 3, 3, &u1_force_dhProc_lvl_en[5]);
		
		ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 20, 21, &u3_force_dhProc_lvl[0]);
		ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 18, 19, &u3_force_dhProc_lvl[1]);	
		ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 10, 11, &u3_force_dhProc_lvl[2]);
		ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 12, 13, &u3_force_dhProc_lvl[3]);
		ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 14, 15, &u3_force_dhProc_lvl[4]);
		ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 16, 17, &u3_force_dhProc_lvl[5]);
	
		if( u1_force_dhProc_lvl_en[0] != 0 )
		{
			pOutput->u3_logo_dhRgnProc_lvl[0] = _MIN_(u3_force_dhProc_lvl[0], 2);
		}
		if( u1_force_dhProc_lvl_en[1] != 0 )
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = _MIN_(u3_force_dhProc_lvl[1], 2);
		}
		if( u1_force_dhProc_lvl_en[2] != 0 )
		{
			pOutput->u3_logo_dhRgnProc_lvl[2] = _MIN_(u3_force_dhProc_lvl[2], 2);
		}
		if( u1_force_dhProc_lvl_en[3] != 0 )
		{
			pOutput->u3_logo_dhRgnProc_lvl[3] = _MIN_(u3_force_dhProc_lvl[3], 2);
		}	
		if( u1_force_dhProc_lvl_en[4] != 0 )
		{
			pOutput->u3_logo_dhRgnProc_lvl[4] = _MIN_(u3_force_dhProc_lvl[4], 2);
		}
		if( u1_force_dhProc_lvl_en[5] != 0 )
		{
			pOutput->u3_logo_dhRgnProc_lvl[5] = _MIN_(u3_force_dhProc_lvl[5], 2);
		}	
	
		if( printkCounter == 0 && printkEnable == 1 )
		{
			/*
			rtd_pr_memc_info("olv=%1d %1d %1d %1d %1d %1d, force=%1d %1d %1d %1d %1d %1d\n", 
					pOutput->u3_logo_dhRgnProc_lvl[0], pOutput->u3_logo_dhRgnProc_lvl[1], pOutput->u3_logo_dhRgnProc_lvl[2], pOutput->u3_logo_dhRgnProc_lvl[3],
					pOutput->u3_logo_dhRgnProc_lvl[4], pOutput->u3_logo_dhRgnProc_lvl[5], u1_force_dhProc_lvl_en[0], u1_force_dhProc_lvl_en[1],
					u1_force_dhProc_lvl_en[2], u1_force_dhProc_lvl_en[3], u1_force_dhProc_lvl_en[4], u1_force_dhProc_lvl_en[5]);
			*/		
		}	
#endif

	printkCounter = printkCounter + 1;
	if( printkCounter >= printkPeriod )
		printkCounter = 0;
#endif


#ifdef FUNC_LOG
#undef FUNC_LOG
#endif
}


#else

VOID FRC_DhLgProc_DynamicLevel(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
#ifdef FUNC_LOG
#undef FUNC_LOG
#endif
#define FUNC_LOG 1

#if FUNC_LOG
	// print related code
	static unsigned char printkCounter = 0;
	unsigned int printkEnable = 0;
	unsigned int printkPeriod = 1;
	unsigned int u1_force_dhProc_lvl_en[6] = {0};
	unsigned int u3_force_dhProc_lvl[6] = {0};
#endif

	const _PQLCONTEXT *s_pContext = GetPQLContext();
	const unsigned char RgnIdxREMAP[32] = // current only separate the region0/1, but dehalo logo process has 0/1/2 regions
	{
		0,	0,	1,	1,	1,	1,	2,	2,
		1,	1,	1,	1,	1,	1,	1,	1,
		1,	1,	1,	1,	1,	1,	1,	1,
		3,	3,	1,	4,	4,	1,	5,	5
	};

	unsigned char RgnIdx = 0, i = 0;
	static unsigned int u32_RgnZmvCnt[32] = {0};
	unsigned int u32_1stGmvZDiff = 0;
	unsigned int u32_2ndGmvZDiff = 0;
	unsigned int ZmvDiffThr = 6;

	unsigned int u16_LogoCnt[6] = {0};
	unsigned int u16_ZmvCnt[6] = {0};
	static unsigned int u16_LogoCnt_iir[6] = {0};
	static unsigned int u16_ZmvCnt_iir[6] = {0};
	unsigned int u32_SumLogoCnt = 0;
	unsigned int u32_SumZmvCnt = 0;

	int s32_RgnRmvSumX[6] = {0}, s32_RgnRmvSumY[6] = {0};
	unsigned int u32_RgnRmvDevX[6] = {0}, u32_RgnRmvDevY[6] = {0};
	static unsigned int u32_RgnRmvDevX_iir[6] = {0}, u32_RgnRmvDevY_iir[6] = {0};
	static unsigned int s32_RgnRmvSumX_iir[6] = {0}, s32_RgnRmvSumY_iir[6] = {0}, tmp_s32_RgnRmvSumX_iir = 0;
	static signed int RgnRmvSumX_iir[6] = {0}, RgnRmvSumY_iir[6] = {0};

	int s32_ZmvLogoDiff = 0, s32_RmvSumX_iir_avg_case1 = 0;
	int uniform_check_cnt = 0, uniform_check = 0, corner_logo_check = 0;
	static unsigned int dhRgnProc_case_cnt_1 = 4, dhRgnProc_case_cnt_2 = 4, dhRgnProc_case_cnt_3 = 4;
	const dhProcLvlThr LvlThr[3] =
	{
		// /*DC = don't care*/
		// level up
		// 1. ZmvCnt <= LogoCnt * Z2L_IncRatio/64
		// 2. ZmvCnt - LogoCnt <= Inc_Z2LDiff
		// 3. Mv Deviation <= Inc_DevVal
		// level down
		// 1. ZmvCnt >= LogoCnt * (1+(Z2L_IncRatio/64))
		// 2. ZmvCnt - LogoCnt >= IncAbsVal
		// 3. Mv Deviation >= Dec_DevVal
		//		Inc_Z2LRatio	Dec_Z2LRatio	Inc_Z2LDiff	Dec_Z2LDiff	Inc_DevVal	Dec_DevVal
		/*0*/ {	50,				0/*DC*/,		-600,		0/*DC*/,	6400,		0/*DC*/	}, // default level (weak)
		/*1*/ {	5,				68,				-3000,		400,		2800,		6600	},
		/*2*/ {	0/*DC*/,		12,				0/*DC*/,	-2800,		0/*DC*/,	3000	}  // strong (strictly panning)
	};


#if FUNC_LOG
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 14, 17, &ZmvDiffThr);
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 29, 29, &printkEnable);
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 24, 27, &printkPeriod);
	printkPeriod = (printkPeriod+1);//*8;

	for(RgnIdx=0; RgnIdx<32; RgnIdx++)
	{
		// 240*135 domain
		u32_1stGmvZDiff = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]) + _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]);
		u32_2ndGmvZDiff = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[RgnIdx]) + _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[RgnIdx]);
				
		if( printkCounter == 0 && printkEnable == 1)
		{
			rtd_pr_memc_debug("[%2d] z1=%5d (%5d %5d ) (%5d ) (%4d ) z2=%5d (%5d %5d ) (%5d ) (%4d ) p=%4d t=%4d sad=%4d %d\n",
				RgnIdx,
				s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx], s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx], u32_1stGmvZDiff, s_pContext->_output_read_comreg.u12_me_rMV_1st_unconf_rb[RgnIdx],
				s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx], s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[RgnIdx], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[RgnIdx], u32_2ndGmvZDiff, s_pContext->_output_read_comreg.u12_me_rMV_2nd_unconf_rb[RgnIdx],
				s_pContext->_output_read_comreg.u13_me_rPRD_rb[RgnIdx], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx]+s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx]+s_pContext->_output_read_comreg.u13_me_rPRD_rb[RgnIdx],
				s_pContext->_output_read_comreg.u25_me_rSAD_rb[RgnIdx]/1020, s_pContext->_output_read_comreg.u25_me_rSAD_rb[RgnIdx]
			);
		}
	}
#endif

	for(RgnIdx=0; RgnIdx<32; RgnIdx++)
	{
		s32_RgnRmvSumX[RgnIdxREMAP[RgnIdx]] = s32_RgnRmvSumX[RgnIdxREMAP[RgnIdx]] + s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx];
		s32_RgnRmvSumY[RgnIdxREMAP[RgnIdx]] = s32_RgnRmvSumY[RgnIdxREMAP[RgnIdx]] + s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx];	
	}

	for(RgnIdx=0; RgnIdx<32; RgnIdx++)
	{
		// deviation
		if( RgnIdxREMAP[RgnIdx] == 0 )
		{
			u32_RgnRmvDevX[0] = u32_RgnRmvDevX[0] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*2,s32_RgnRmvSumX[0]);
			u32_RgnRmvDevY[0] = u32_RgnRmvDevY[0] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*2,s32_RgnRmvSumY[0]);
		}		
		else if( RgnIdxREMAP[RgnIdx] == 2 )
		{
			u32_RgnRmvDevX[2] = u32_RgnRmvDevX[2] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*2,s32_RgnRmvSumX[2]);
			u32_RgnRmvDevY[2] = u32_RgnRmvDevY[2] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*2,s32_RgnRmvSumY[2]);		
		}
		else if( RgnIdxREMAP[RgnIdx] == 3 )
		{
			u32_RgnRmvDevX[3] = u32_RgnRmvDevX[3] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*2,s32_RgnRmvSumX[3]);
			u32_RgnRmvDevY[3] = u32_RgnRmvDevY[3] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*2,s32_RgnRmvSumY[3]);	
		}
		else if( RgnIdxREMAP[RgnIdx] == 4 )
		{
			u32_RgnRmvDevX[4] = u32_RgnRmvDevX[4] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*2,s32_RgnRmvSumX[4]);
			u32_RgnRmvDevY[4] = u32_RgnRmvDevY[4] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*2,s32_RgnRmvSumY[4]);			
		}	
		else if( RgnIdxREMAP[RgnIdx] == 5 )	
		{
			u32_RgnRmvDevX[5] = u32_RgnRmvDevX[5] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*2,s32_RgnRmvSumX[5]);
			u32_RgnRmvDevY[5] = u32_RgnRmvDevY[5] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*2,s32_RgnRmvSumY[5]);			
		}
		else
		{
			u32_RgnRmvDevX[1] = u32_RgnRmvDevX[1] + _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]*22,s32_RgnRmvSumX[1]);
			u32_RgnRmvDevY[1] = u32_RgnRmvDevY[1] + _ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]*22,s32_RgnRmvSumY[1]);
		}

		// 240*135 domain
		u32_1stGmvZDiff = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]) + _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]);
		u32_2ndGmvZDiff = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[RgnIdx]) + _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[RgnIdx]);

		// there are only 990 block at last statistic region
		if( RgnIdx>=24 )
			u32_RgnZmvCnt[RgnIdx] = 990;
		else
			u32_RgnZmvCnt[RgnIdx] = 1020;

		// guess the zmv amount
		if( u32_1stGmvZDiff > ZmvDiffThr && s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx] != 0 )
		{
			u32_RgnZmvCnt[RgnIdx] = (u32_RgnZmvCnt[RgnIdx] >= s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx])? u32_RgnZmvCnt[RgnIdx]-s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx] : 0;
		}
		if( u32_2ndGmvZDiff > ZmvDiffThr && s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx] != 0 )
		{
			u32_RgnZmvCnt[RgnIdx] = (u32_RgnZmvCnt[RgnIdx] >= s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx])? u32_RgnZmvCnt[RgnIdx]-s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx] : 0;
		}
		u32_RgnZmvCnt[RgnIdx] = (u32_RgnZmvCnt[RgnIdx] >= s_pContext->_output_read_comreg.u13_me_rPRD_rb[RgnIdx])? u32_RgnZmvCnt[RgnIdx]-s_pContext->_output_read_comreg.u13_me_rPRD_rb[RgnIdx] :  0;

		// add the logo count and zmv count to each region counter
		u16_LogoCnt[RgnIdxREMAP[RgnIdx]] = u16_LogoCnt[RgnIdxREMAP[RgnIdx]] + s_pContext->_output_read_comreg.u10_blklogo_rgcnt[RgnIdx];
		u16_ZmvCnt[RgnIdxREMAP[RgnIdx]] = u16_ZmvCnt[RgnIdxREMAP[RgnIdx]] + u32_RgnZmvCnt[RgnIdx];

#if FUNC_LOG
		if( printkCounter == 0 && printkEnable == 1 )
		{
			rtd_pr_memc_debug("[%2d] z=%4d l=%4d ", RgnIdx, u32_RgnZmvCnt[RgnIdx], s_pContext->_output_read_comreg.u10_blklogo_rgcnt[RgnIdx]);
			if( (RgnIdx+1)%8 == 0 )
				rtd_pr_memc_debug("\n");
		}
#endif

	}

	// iir process
	for(i=0; i<6; i++)
	{
		u16_LogoCnt_iir[i] = (u16_LogoCnt[i] + u16_LogoCnt_iir[i]) >> 1;
		u16_ZmvCnt_iir[i] = (u16_ZmvCnt[i] + u16_ZmvCnt_iir[i]) >> 1;
		u32_SumLogoCnt = u32_SumLogoCnt + u16_LogoCnt_iir[i];
		u32_SumZmvCnt = u32_SumZmvCnt + u16_ZmvCnt_iir[i];
		u32_RgnRmvDevX_iir[i] = (u32_RgnRmvDevX_iir[i] + u32_RgnRmvDevX[i]) >> 1;
		u32_RgnRmvDevY_iir[i] = (u32_RgnRmvDevY_iir[i] + u32_RgnRmvDevY[i]) >> 1;
		s32_RgnRmvSumX_iir[i] = (s32_RgnRmvSumX_iir[i] + s32_RgnRmvSumX[i]) >> 1;
		s32_RgnRmvSumY_iir[i] = (s32_RgnRmvSumY_iir[i] + s32_RgnRmvSumY[i]) >> 1;
		RgnRmvSumX_iir[i] = (RgnRmvSumX_iir[i] + s32_RgnRmvSumX[i]) >> 1;
		RgnRmvSumY_iir[i] = (RgnRmvSumY_iir[i] + s32_RgnRmvSumY[i]) >> 1;
	}
	s32_ZmvLogoDiff = (u32_SumZmvCnt-u32_SumLogoCnt);
	s32_RmvSumX_iir_avg_case1 = ((RgnRmvSumX_iir[1]/11)+RgnRmvSumX_iir[3]+RgnRmvSumX_iir[4]+RgnRmvSumX_iir[5])>>2;

	//uniform mv-h check
	for(i=0; i<6; i++){
		
		if((i!=0)&&(i!=2)){
			if(i==1){
				tmp_s32_RgnRmvSumX_iir = s32_RgnRmvSumX_iir[i]/11;
			}else{
				tmp_s32_RgnRmvSumX_iir = s32_RgnRmvSumX_iir[i];
			}
			if( (_ABS_DIFF_( tmp_s32_RgnRmvSumX_iir,s32_RmvSumX_iir_avg_case1 ) < 10) && (_ABS_(RgnRmvSumY_iir[i]) < 5)){
				uniform_check_cnt++ ;
			}
		}
	}

	//coner logo check
	if( u16_ZmvCnt_iir[0]<=70 && u16_ZmvCnt_iir[2]<=70 && u16_ZmvCnt_iir[3]<=70 && u16_ZmvCnt_iir[4]<=70 && u16_ZmvCnt_iir[5]<=200 &&
		u16_LogoCnt_iir[0]<=300 && u16_LogoCnt_iir[2]<=300 && u16_LogoCnt_iir[3]<=350 && u16_LogoCnt_iir[4]<=300 && u16_LogoCnt_iir[5]<=350){
		corner_logo_check = 1;
	}else{
		corner_logo_check = 0;
	}
	

	if(uniform_check_cnt == 4){
		uniform_check = 1;
	}else{
		uniform_check = 0;
	}

//======== RgnProc_case control ==================
	if(uniform_check==1 && corner_logo_check ==1 )
	{
		dhRgnProc_case_cnt_1 =4;
		pOutput->u3_logo_dhRgnProc_case[1] = 1;
	}else{

		if(dhRgnProc_case_cnt_1>0){
			dhRgnProc_case_cnt_1--;
		}else{	
			dhRgnProc_case_cnt_1=0;
			pOutput->u3_logo_dhRgnProc_case[1] = 0;
		}
	}

	if((u16_LogoCnt_iir[2]>200 && u16_LogoCnt_iir[1] < 100) && (u16_ZmvCnt_iir[2]<=20) && (u32_RgnRmvDevX[2]<=30 && u32_RgnRmvDevY[2]<=20))
	{
		dhRgnProc_case_cnt_2 =4;
		pOutput->u3_logo_dhRgnProc_case[2] = 1;
		
	}else{

		if(dhRgnProc_case_cnt_2>0){
			dhRgnProc_case_cnt_2--;
		}else{	
			dhRgnProc_case_cnt_2=0;
			pOutput->u3_logo_dhRgnProc_case[2] = 0;
		}
	}

	if((u16_LogoCnt_iir[0]<60 && u16_LogoCnt_iir[1]>=190 && u16_LogoCnt_iir[2]<110 && u16_LogoCnt_iir[4]<10) && 
			 (u16_LogoCnt_iir[3]>130 && u16_LogoCnt_iir[3]<=350) && 
			 (u16_LogoCnt_iir[5]>250 && u16_LogoCnt_iir[5]<550)){
		dhRgnProc_case_cnt_3 =4;
		pOutput->u3_logo_dhRgnProc_case[3] = 1;
		
	}else if( (((u32_RgnRmvDevX_iir[3]+u32_RgnRmvDevY_iir[3])*16) <= 350 && s32_ZmvLogoDiff<-500 ) ||
				((u16_ZmvCnt_iir[0]<=100 && u16_ZmvCnt_iir[2]<=100 && u16_ZmvCnt_iir[3]<=100&& u16_ZmvCnt_iir[4]<=100 && u16_ZmvCnt_iir[5]<=200) && 
				 (u16_LogoCnt_iir[0]<=400 && u16_LogoCnt_iir[2]<=200 && u16_LogoCnt_iir[3]<=400 && u16_LogoCnt_iir[4]<=200 && u16_LogoCnt_iir[5]<=200 )) // 
			)
	{	
		dhRgnProc_case_cnt_3 =4;
		pOutput->u3_logo_dhRgnProc_case[3] = 2;
	}else{

		if(dhRgnProc_case_cnt_3>0){
			dhRgnProc_case_cnt_3--;
		}else{	
			dhRgnProc_case_cnt_3=0;
			pOutput->u3_logo_dhRgnProc_case[3] = 0;
		}
	}

	
	
#if FUNC_LOG
	if( printkCounter == 0 && printkEnable == 1 )
	{
		rtd_pr_memc_debug("hRgnProc_case = ( %5d %5d %5d %5d %5d %5d )(%5d, %5d, %5d)\n",
				pOutput->u3_logo_dhRgnProc_case[0],pOutput->u3_logo_dhRgnProc_case[1],pOutput->u3_logo_dhRgnProc_case[2],pOutput->u3_logo_dhRgnProc_case[3],
				pOutput->u3_logo_dhRgnProc_case[4],pOutput->u3_logo_dhRgnProc_case[5],uniform_check_cnt, corner_logo_check, s32_RmvSumX_iir_avg_case1 );	
	}	
#endif

	

#if FUNC_LOG
	if( printkCounter == 0 && printkEnable == 1 )
	{	
		rtd_pr_memc_debug("SumZ=%5d, SumL=%5d, Diff=%d\n", u32_SumZmvCnt, u32_SumLogoCnt, s32_ZmvLogoDiff);
		rtd_pr_memc_debug("RgnZ=%5d %5d %5d %5d %5d %5d,\n",u16_ZmvCnt_iir[0], u16_ZmvCnt_iir[1],u16_ZmvCnt_iir[2],u16_ZmvCnt_iir[3],u16_ZmvCnt_iir[4],u16_ZmvCnt_iir[5]);
		rtd_pr_memc_debug("RgnL=%5d %5d %5d %5d %5d %5d,\n",u16_LogoCnt_iir[0], u16_LogoCnt_iir[1],u16_LogoCnt_iir[2], u16_LogoCnt_iir[3],u16_LogoCnt_iir[4], u16_LogoCnt_iir[5] );
	#if 1
		rtd_pr_memc_debug("RgnDevX = ( %5d %5d %5d %5d %5d %5d ), RgnDevY = ( %5d %5d %5d %5d %5d %5d )\n",
				u32_RgnRmvDevX[0],u32_RgnRmvDevX[1],u32_RgnRmvDevX[2],u32_RgnRmvDevX[3],u32_RgnRmvDevX[4],u32_RgnRmvDevX[5],
				u32_RgnRmvDevY[0],u32_RgnRmvDevY[1],u32_RgnRmvDevY[2],u32_RgnRmvDevY[3],u32_RgnRmvDevY[4],u32_RgnRmvDevY[5]);	
	
		rtd_pr_memc_debug("RmvSumX = ( %5d %5d %5d %5d %5d %5d ), RmvSumY = ( %5d %5d %5d %5d %5d %5d )\n",
				RgnRmvSumX_iir[0],RgnRmvSumX_iir[1],RgnRmvSumX_iir[2],RgnRmvSumX_iir[3],RgnRmvSumX_iir[4],RgnRmvSumX_iir[5],
				RgnRmvSumY_iir[0],RgnRmvSumY_iir[1],RgnRmvSumY_iir[2],RgnRmvSumY_iir[3],RgnRmvSumY_iir[4],RgnRmvSumY_iir[5]);				
		
		rtd_pr_memc_debug("RgnDeviir = (%5d %5d ) %5d, (%5d %5d ) %5d, (%5d %5d ) %5d, (%5d %5d ) %5d, (%5d %5d ) %5d, (%5d %5d ) %5d\n",
			u32_RgnRmvDevX_iir[0]*16, u32_RgnRmvDevY_iir[0]*16, (u32_RgnRmvDevX_iir[0]+u32_RgnRmvDevY_iir[0])*16,(u32_RgnRmvDevX_iir[1]*16)/11, (u32_RgnRmvDevY_iir[1]*16)/11, ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*16)/11,
			u32_RgnRmvDevX_iir[2]*16, u32_RgnRmvDevY_iir[2]*16, (u32_RgnRmvDevX_iir[2]+u32_RgnRmvDevY_iir[2])*16,u32_RgnRmvDevX_iir[3]*16, u32_RgnRmvDevY_iir[3]*16, (u32_RgnRmvDevX_iir[3]+u32_RgnRmvDevY_iir[3])*16,
			u32_RgnRmvDevX_iir[4]*16, u32_RgnRmvDevY_iir[4]*16, (u32_RgnRmvDevX_iir[4]+u32_RgnRmvDevY_iir[4])*16,u32_RgnRmvDevX_iir[5]*16, u32_RgnRmvDevY_iir[5]*16, (u32_RgnRmvDevX_iir[5]+u32_RgnRmvDevY_iir[5])*16
			);
	#else
	
		rtd_pr_memc_debug("RgnDev = (%5d %5d) %5d (%5d %5d) %5d\n",
			u32_RgnRmvDevX_iir[0]*4, u32_RgnRmvDevY_iir[0]*4, (u32_RgnRmvDevX_iir[0]+u32_RgnRmvDevY_iir[0])*4,
			u32_RgnRmvDevX_iir[1]*4/3, u32_RgnRmvDevY_iir[1]*4/3, (u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*4/3
		);
	#endif	
		rtd_pr_memc_debug("rgnclv=%1d %1d %1d %1d %1d %1d\n", pOutput->u3_logo_dhRgnProc_lvl[0], pOutput->u3_logo_dhRgnProc_lvl[1], pOutput->u3_logo_dhRgnProc_lvl[2], pOutput->u3_logo_dhRgnProc_lvl[3]
									, pOutput->u3_logo_dhRgnProc_lvl[4], pOutput->u3_logo_dhRgnProc_lvl[5]);
	}
#endif
	
	//RgnProc_lvl control
	if( u32_SumZmvCnt >= (((240*135)*14)>>4) || s_pContext->_output_me_sceneAnalysis.u2_panning_flag !=1 || 
		(s_pContext->_output_read_comreg.u10_blklogo_rgcnt[1]<=400 && s_pContext->_output_read_comreg.u10_blklogo_rgcnt[7]<=400)) // still image and non-panning
	{
		for(i=0; i<6; i++){
			pOutput->u3_logo_dhRgnProc_lvl[i] = 0;
		}
	}
	else
	{		
		// region 0 condition
		
		// region 1 condition
		// deviation very large
		// some region with logo and zmv appears
#if FUNC_LOG
		if( printkCounter == 0 && printkEnable == 1 )
		{
			rtd_pr_memc_debug("inc %1d %1d %1d\n",
				( (u16_ZmvCnt_iir[1]*64) <= (u16_LogoCnt_iir[1]*LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_Z2LRatio) ),
				( s32_ZmvLogoDiff <= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_Z2LDiff ),
				( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*16/11) <= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_DevVal )
			);
			rtd_pr_memc_debug("dec %1d %1d %1d\n",
				( (u16_ZmvCnt_iir[1]*64) >= (u16_LogoCnt_iir[1]*LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_Z2LRatio) ),
				( s32_ZmvLogoDiff >= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_Z2LDiff ),
				( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*16/11) >= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_DevVal )
			);
		}
#endif

#if RTK_MEMC_Performance_tunging_from_tv001
		if( (u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*4/3 > 8300 ) // complex scene
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = 0;
		}
		else if( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*4/3) <= 200 ) // bamtori
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = 2;
		}
		else if( ( (u16_ZmvCnt_iir[1]*64) <= (u16_LogoCnt_iir[1]*LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_Z2LRatio) ) &&
			( s32_ZmvLogoDiff <= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_Z2LDiff ) &&
			( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*4/3) <= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_DevVal ) ) // inc
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = (pOutput->u3_logo_dhRgnProc_lvl[1]<2)? pOutput->u3_logo_dhRgnProc_lvl[1]+1 : 2;
		}
		else if( ( (u16_ZmvCnt_iir[1]*64) >= (u16_LogoCnt_iir[1]*LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_Z2LRatio) ) &&
				 ( s32_ZmvLogoDiff >= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_Z2LDiff ) &&
				 ( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*4/3) >= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_DevVal ) ) // dec
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = (pOutput->u3_logo_dhRgnProc_lvl[1]>1) ? pOutput->u3_logo_dhRgnProc_lvl[1]-1 : 0;
		}		
#else
		if( (u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*16/11 > 8300 ) // complex scene
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = 0;
		}
		else if( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*16/11) <= 200 ) // bamtori
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = 2;
		}
		else if( ( (u16_ZmvCnt_iir[1]*64) <= (u16_LogoCnt_iir[1]*LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_Z2LRatio) ) &&
			( s32_ZmvLogoDiff <= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_Z2LDiff ) &&
			( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*16/11) <= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Inc_DevVal ) ) // inc
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = (pOutput->u3_logo_dhRgnProc_lvl[1]<2)? pOutput->u3_logo_dhRgnProc_lvl[1]+1 : 2;
		}
		else if( ( (u16_ZmvCnt_iir[1]*64) >= (u16_LogoCnt_iir[1]*LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_Z2LRatio) ) &&
				 ( s32_ZmvLogoDiff >= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_Z2LDiff ) &&
				 ( ((u32_RgnRmvDevX_iir[1]+u32_RgnRmvDevY_iir[1])*16/11) >= LvlThr[pOutput->u3_logo_dhRgnProc_lvl[1]].Dec_DevVal ) ) // dec
		{
			pOutput->u3_logo_dhRgnProc_lvl[1] = (pOutput->u3_logo_dhRgnProc_lvl[1]>1) ? pOutput->u3_logo_dhRgnProc_lvl[1]-1 : 0;
		}	
#endif

	// temporally make it the same
	for(i=0; i<6; i++){
		if( (pOutput->u3_logo_dhRgnProc_lvl[i]!=pOutput->u3_logo_dhRgnProc_lvl[1]) && pOutput->u3_logo_dhRgnProc_lvl[1]!=0 ){
			pOutput->u3_logo_dhRgnProc_lvl[i] = pOutput->u3_logo_dhRgnProc_lvl[1];
		}
	}



	}


#if FUNC_LOG
#if 0

	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 23, 23, &u1_force_dhProc_lvl_en[0]);
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 22, 22, &u1_force_dhProc_lvl_en[1]);

	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 20, 21, &u3_force_dhProc_lvl[0]);
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 18, 19, &u3_force_dhProc_lvl[1]);	
	if( u1_force_dhProc_lvl_en[0] != 0 )
	{
		pOutput->u3_logo_dhRgnProc_lvl[0] = _MIN_(u3_force_dhProc_lvl[0], 2);
	}
	if( u1_force_dhProc_lvl_en[1] != 0 )
	{
		pOutput->u3_logo_dhRgnProc_lvl[1] = _MIN_(u3_force_dhProc_lvl[1], 2);
	}

		if( printkCounter == 0 && printkEnable == 1 )
	{
		rtd_pr_memc_debug("olv=%1d %1d, force=%1d %1d\n", pOutput->u3_logo_dhRgnProc_lvl[0], pOutput->u3_logo_dhRgnProc_lvl[1], u1_force_dhProc_lvl_en[0], u1_force_dhProc_lvl_en[1]);
	}
	

#else
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 23, 23, &u1_force_dhProc_lvl_en[0]);
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 22, 22, &u1_force_dhProc_lvl_en[1]);
	ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 0, 0, &u1_force_dhProc_lvl_en[2]);
	ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 1, 1, &u1_force_dhProc_lvl_en[3]);
	ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 2, 2, &u1_force_dhProc_lvl_en[4]);
	ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 3, 3, &u1_force_dhProc_lvl_en[5]);
	
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 20, 21, &u3_force_dhProc_lvl[0]);
	ReadRegister(SOFTWARE1_SOFTWARE1_21_reg, 18, 19, &u3_force_dhProc_lvl[1]);	
	ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 10, 11, &u3_force_dhProc_lvl[2]);
	ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 12, 13, &u3_force_dhProc_lvl[3]);
	ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 14, 15, &u3_force_dhProc_lvl[4]);
	ReadRegister(SOFTWARE1_SOFTWARE1_25_reg, 16, 17, &u3_force_dhProc_lvl[5]);

	if( u1_force_dhProc_lvl_en[0] != 0 )
	{
		pOutput->u3_logo_dhRgnProc_lvl[0] = _MIN_(u3_force_dhProc_lvl[0], 2);
	}
	if( u1_force_dhProc_lvl_en[1] != 0 )
	{
		pOutput->u3_logo_dhRgnProc_lvl[1] = _MIN_(u3_force_dhProc_lvl[1], 2);
	}
	if( u1_force_dhProc_lvl_en[2] != 0 )
	{
		pOutput->u3_logo_dhRgnProc_lvl[2] = _MIN_(u3_force_dhProc_lvl[2], 2);
	}
	if( u1_force_dhProc_lvl_en[3] != 0 )
	{
		pOutput->u3_logo_dhRgnProc_lvl[3] = _MIN_(u3_force_dhProc_lvl[3], 2);
	}	
	if( u1_force_dhProc_lvl_en[4] != 0 )
	{
		pOutput->u3_logo_dhRgnProc_lvl[4] = _MIN_(u3_force_dhProc_lvl[4], 2);
	}
	if( u1_force_dhProc_lvl_en[5] != 0 )
	{
		pOutput->u3_logo_dhRgnProc_lvl[5] = _MIN_(u3_force_dhProc_lvl[5], 2);
	}	

	if( printkCounter == 0 && printkEnable == 1 )
	{
		rtd_pr_memc_debug("olv=%1d %1d %1d %1d %1d %1d, force=%1d %1d %1d %1d %1d %1d\n", 
				pOutput->u3_logo_dhRgnProc_lvl[0], pOutput->u3_logo_dhRgnProc_lvl[1], pOutput->u3_logo_dhRgnProc_lvl[2], pOutput->u3_logo_dhRgnProc_lvl[3],
				pOutput->u3_logo_dhRgnProc_lvl[4], pOutput->u3_logo_dhRgnProc_lvl[5], u1_force_dhProc_lvl_en[0], u1_force_dhProc_lvl_en[1],
				u1_force_dhProc_lvl_en[2], u1_force_dhProc_lvl_en[3], u1_force_dhProc_lvl_en[4], u1_force_dhProc_lvl_en[5]);
	}	
#endif


	printkCounter = printkCounter + 1;
	if( printkCounter >= printkPeriod )
		printkCounter = 0;
#endif


#ifdef FUNC_LOG
#undef FUNC_LOG
#endif
}

#endif


#ifdef CONFIG_SUPPORT_SLD_BY_MEMC_DEMURA
VOID FRC_SLD_PxlTargetYVal(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	// consider the regional apl and dtl to set the regional target y value
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	// for regional apl/dtl with basic value
	unsigned char RgnIdx = 0;
	unsigned int RgnAPL = 0;
	unsigned int RgnDTL = 0;
	unsigned int RgnBlkNum = (240*135)/32;

	unsigned int LT_RgnAplAvg = (s_pContext->_output_read_comreg.u20_me_rAPLi_rb[0] + s_pContext->_output_read_comreg.u20_me_rAPLp_rb[0] +
						   s_pContext->_output_read_comreg.u20_me_rAPLi_rb[1] + s_pContext->_output_read_comreg.u20_me_rAPLp_rb[1] +
						   s_pContext->_output_read_comreg.u20_me_rAPLi_rb[2] + s_pContext->_output_read_comreg.u20_me_rAPLp_rb[2])/6;
	unsigned int RgnAplAvgWithouCurBlk = 0;

	unsigned int LT_apl_l = pParam->u8_logo_rglt_pxldf_apl_thr_l; // 60
	unsigned int LT_apl_h = pParam->u8_logo_rglt_pxldf_apl_thr_h; // 220
	unsigned int LT_val_l = pParam->u8_logo_rglt_pxldf_val_thr_l; // 190
	unsigned int LT_val_h = pParam->u8_logo_rglt_pxldf_val_thr_h; // 230

	// modify by statistic
	unsigned char HistoIdx = 0;
	unsigned int u32_TotalHistNum = 1;
	unsigned int u32_HistoPartial_H = 0, u8_Ratio_H = 0; // high apl part of the histogram
	unsigned int u32_HistoPartial_M = 0, u8_Ratio_M = 0; // middle apl part of the histogram
	unsigned int u32_HistoPartial_L = 0, u8_Ratio_L = 0; // low apl part of the histogram
	unsigned char Hist2AplOft_H = 0, Hist2AplOft_M = 0, Hist2AplOft_L = 0;

#if 0
	static unsigned char printkCounter = 0;
	const unsigned char printkEnable = pParam->u1_logo_sld_debug_print_en;
	const unsigned int printkPeriod = (pParam->u1_logo_sld_debug_print_period + 1) * 16;
#endif
	if (MEMC_Pixel_LOGO_For_SW_SLD != 1)
		return;
	// -------------------- histogram --------------------
	for(HistoIdx=0; HistoIdx<32; HistoIdx++)
	{
		u32_TotalHistNum = u32_TotalHistNum + s_pContext->_output_read_comreg.u32_yuv_meter[HistoIdx];
		if(HistoIdx < 4)
			u32_HistoPartial_L = u32_HistoPartial_L + s_pContext->_output_read_comreg.u32_yuv_meter[HistoIdx];
		if(HistoIdx >= 4 && HistoIdx < 28)
			u32_HistoPartial_M = u32_HistoPartial_M + s_pContext->_output_read_comreg.u32_yuv_meter[HistoIdx];
		if(HistoIdx >= 28)
			u32_HistoPartial_H = u32_HistoPartial_H + s_pContext->_output_read_comreg.u32_yuv_meter[HistoIdx];
	}

	if( u32_TotalHistNum >= 1400*240 )
	{
		u8_Ratio_H = (u32_HistoPartial_H*100)/u32_TotalHistNum;
		u8_Ratio_M = (u32_HistoPartial_M*100)/u32_TotalHistNum;
		u8_Ratio_L = (u32_HistoPartial_L*100)/u32_TotalHistNum;

		if( u8_Ratio_H == 0 )
			Hist2AplOft_H = 99;

		Hist2AplOft_L = (u8_Ratio_L<=40)? 0 : _MIN_(u8_Ratio_L-40, 25);
	}

#if 0
	if( printkEnable == 1 && printkCounter == 0 )
	{
		for(HistoIdx=0; HistoIdx<32; HistoIdx++)
		{
			rtd_pr_memc_debug("[%2d] %6d ", HistoIdx, s_pContext->_output_read_comreg.u32_yuv_meter[HistoIdx]);
			if((HistoIdx+1)%4==0)
				rtd_pr_memc_debug("\n");
		}
		rtd_pr_memc_debug("\n");
		rtd_pr_memc_debug("TotalCnt = %d\n", u32_TotalHistNum);
		rtd_pr_memc_debug("L = %8d, r = %d, o = %d\n", u32_HistoPartial_L, u8_Ratio_L, Hist2AplOft_L);
		rtd_pr_memc_debug("M = %8d, r = %d, o = %d\n", u32_HistoPartial_M, u8_Ratio_M, Hist2AplOft_M);
		rtd_pr_memc_debug("H = %8d, r = %d, o = %d\n", u32_HistoPartial_H, u8_Ratio_H, Hist2AplOft_H);
	}
#endif

	// -------------------- region target value --------------------
	for(RgnIdx = 0; RgnIdx<3; RgnIdx++)
	{
		RgnAPL = (s_pContext->_output_read_comreg.u20_me_rAPLi_rb[RgnIdx] + s_pContext->_output_read_comreg.u20_me_rAPLp_rb[RgnIdx]) >> 1;
		RgnDTL = s_pContext->_output_read_comreg.u20_me_rDTL_rb[RgnIdx];
		RgnAplAvgWithouCurBlk = (LT_RgnAplAvg*6-s_pContext->_output_read_comreg.u20_me_rAPLi_rb[RgnIdx]-s_pContext->_output_read_comreg.u20_me_rAPLp_rb[RgnIdx]) >> 2;

#if 0
		// current rgn apl different from others too much
		if( printkEnable == 1 && printkCounter == 0 )
		{
			rtd_pr_memc_debug("[%d] RgnAplwoCur = %d thr = %d curApl = %d\n", RgnIdx, RgnAplAvgWithouCurBlk, (80*RgnBlkNum), RgnAPL);
		}
#endif

		if( RgnAplAvgWithouCurBlk > RgnAPL && (RgnAplAvgWithouCurBlk-RgnAPL) >= (80*RgnBlkNum) )
		{
			RgnAPL = RgnAplAvgWithouCurBlk;
			//RgnAPL = (RgnAPL*3+LT_RgnAplAvg*5) >> 3;
#if 0
			if( printkEnable == 1 && printkCounter == 0 )
			{
				rtd_pr_memc_debug("[%d] FixAPL = %d (%d)\n", RgnIdx, RgnAPL, (RgnAPL*3+LT_RgnAplAvg*5) >> 3);
			}
#endif
		}

		// get the basic output value by apl
		if( RgnDTL >= 8000/*10000*/ )
		{
			if( RgnAPL >= LT_apl_h*RgnBlkNum)
				pOutput->u8_PxlRgnDfy[RgnIdx] = LT_val_h;
			else if(RgnAPL <= LT_apl_l*RgnBlkNum)
				pOutput->u8_PxlRgnDfy[RgnIdx] = LT_val_l;
			else
				pOutput->u8_PxlRgnDfy[RgnIdx] = ((LT_val_h-LT_val_l)*(RgnAPL-LT_apl_l*RgnBlkNum))/((LT_apl_h-LT_apl_l)*RgnBlkNum) + LT_val_l;
		}
		else
		{
			pOutput->u8_PxlRgnDfy[RgnIdx] = LT_val_h;
#if 0
			if( printkEnable == 1 && printkCounter == 0 )
			{
				rtd_pr_memc_debug("[%d] dtl low, fix value\n", RgnIdx);
			}
#endif
		}
#if 0
		if( printkEnable == 1 && printkCounter == 0 )
		{
			rtd_pr_memc_debug("[%d] RgnBlks = %d apl bound = %d %d\n", RgnIdx, RgnBlkNum, LT_apl_h*RgnBlkNum, LT_apl_l*RgnBlkNum);
			rtd_pr_memc_debug("[%d] apl = %d, dtl = %d, value = %d\n", RgnIdx, RgnAPL, RgnDTL, pOutput->u8_PxlRgnDfy[RgnIdx]);
		}
#endif
	}

	// -------------------- decide the final target value --------------------
	for(RgnIdx = 0; RgnIdx<3; RgnIdx++)
	{
		pOutput->u8_PxlRgnDfy[RgnIdx] = pOutput->u8_PxlRgnDfy[RgnIdx] + Hist2AplOft_L + Hist2AplOft_M + Hist2AplOft_H;
		pOutput->u8_PxlRgnDfy[RgnIdx] = _CLIP_(pOutput->u8_PxlRgnDfy[RgnIdx], LT_val_l, LT_val_h);
#if 0
		if( printkEnable == 1 && printkCounter == 0 )
		{
			rtd_pr_memc_debug("[%d] final val = %d\n", RgnIdx, pOutput->u8_PxlRgnDfy[RgnIdx]);
		}
#endif
	}
#if 0
	printkCounter = printkCounter + 1;
	if( printkCounter >= printkPeriod )
		printkCounter = 0;
#endif
}

VOID FRC_SLD_PxlRgnClr(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	// cond1. backgournd and logo are the same > clear
	// cond2. flat / motion / too much logo > clear
	const _PQLCONTEXT *s_pContext = GetPQLContext();
#if 0
	static unsigned char printkCounter = 0;
	const unsigned char printkEnable = pParam->u1_logo_sld_debug_print_en;
	const unsigned int printkPeriod = (pParam->u1_logo_sld_debug_print_period + 1) * 16;
#endif
	unsigned char RgnIdx = 0;

	static unsigned int u32_RgnApl[32] = {0};
	static unsigned int u32_RgnDtl[32] = {0};
	static unsigned int u32_RgnZmvCnt[32] = {0};
	static unsigned int u32_RgnPxlCnt[32] = {0};

	unsigned int u32_1stGmvZDiff = 0;
	unsigned int u32_2ndGmvZDiff = 0;
	unsigned int u32_1st2ndGmvDiff = 0;
	//unsigned char u3_RgnLogoSmlMvStatus = 0;

	const unsigned int u32_PxlRgClrCntThr = ((960*540*pParam->u32_logo_rgclr_cnt_thr)/100)/32;
	unsigned int u32_RgnClrCmpThr = 0;

	// output
	unsigned int u32_ClrSigByApl = 0;
	unsigned int u32_ClrSigByDtl = 0;
	unsigned int u32_ClrSigByCnt = 0;
	unsigned int u32_ClrSigByCmp = 0;

	if ((MEMC_Pixel_LOGO_For_SW_SLD != 1) && (MEMC_Pixel_LOGO_For_SW_SLD != 2))
		return;
	
	// -------------------- Histogram --------------------
	for(RgnIdx=0; RgnIdx<32; RgnIdx++)
	{
		// 240*135 domain
		u32_RgnApl[RgnIdx] = 0;
		u32_RgnApl[RgnIdx] = (s_pContext->_output_read_comreg.u20_me_rAPLi_rb[RgnIdx] + s_pContext->_output_read_comreg.u20_me_rAPLp_rb[RgnIdx]+1012)/2024;

		// 240*135 domain
		u32_RgnDtl[RgnIdx] = 0;
		u32_RgnDtl[RgnIdx] = (s_pContext->_output_read_comreg.u20_me_rDTL_rb[RgnIdx]+506)/1024;

		// 960x540 domain
		u32_RgnPxlCnt[RgnIdx] = 0;
		u32_RgnPxlCnt[RgnIdx] = s_pContext->_output_read_comreg.u14_pxllogo_rgcnt[RgnIdx];

		// 240*135 domain
		u32_1stGmvZDiff = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx]) + _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx]);
		u32_2ndGmvZDiff = _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[RgnIdx]) + _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[RgnIdx]);
		u32_1st2ndGmvDiff = _ABS_DIFF_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx] , s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[RgnIdx]) +
							_ABS_DIFF_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx] , s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[RgnIdx]);

		u32_RgnZmvCnt[RgnIdx] = 1020;
		if( u32_1stGmvZDiff > pParam->u8_logo_rgclr_logo_mv_thr )
		{
			u32_RgnZmvCnt[RgnIdx] = (u32_RgnZmvCnt[RgnIdx] >= s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx])? u32_RgnZmvCnt[RgnIdx]-s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx] : 0;
			//u3_RgnLogoSmlMvStatus = u3_RgnLogoSmlMvStatus | 0x1;
		}
		if( u32_2ndGmvZDiff > pParam->u8_logo_rgclr_logo_mv_thr )
		{
			u32_RgnZmvCnt[RgnIdx] = (u32_RgnZmvCnt[RgnIdx] >= s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx])? u32_RgnZmvCnt[RgnIdx]-s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx] : 0;
			//u3_RgnLogoSmlMvStatus = u3_RgnLogoSmlMvStatus | 0x2;
		}
#if 0
#if 1 // normal log

		if( printkEnable == 1 && printkCounter == 0 )
		{

			rtd_pr_memc_debug("[%2d] apl= %3d dtl= %3d lg= %5d zmv= %4d ", RgnIdx, u32_RgnApl[RgnIdx], u32_RgnDtl[RgnIdx], u32_RgnPxlCnt[RgnIdx], u32_RgnZmvCnt[RgnIdx]);
			if((RgnIdx+1)%4==0)
				rtd_pr_memc_debug("\n");
		}
#else // debugging detail log
		if( printkEnable == 1 && printkCounter == 0 && (RgnIdx<=7 || RgnIdx>=24) )
		{
			rtd_pr_memc_debug("[%2d] (%5d %5d %4d) (%5d %5d %4d) final = %4d, flg = %d\n",
				RgnIdx,
				s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[RgnIdx], s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[RgnIdx], s_pContext->_output_read_comreg.u12_me_rMV_1st_cnt_rb[RgnIdx],
				s_pContext->_output_read_comreg.s11_me_rMV_2nd_vx_rb[RgnIdx], s_pContext->_output_read_comreg.s10_me_rMV_2nd_vy_rb[RgnIdx], s_pContext->_output_read_comreg.u12_me_rMV_2nd_cnt_rb[RgnIdx],
				u32_RgnZmvCnt[RgnIdx], u3_RgnLogoSmlMvStatus
			);

		}
#endif
#endif

	}


	for(RgnIdx=0; RgnIdx<32; RgnIdx++)
	{
		// -------------------- clear by apl --------------------
		if( u32_RgnApl[RgnIdx] >= pParam->u32_logo_rgclr_apl_thr )
			u32_ClrSigByApl = (u32_ClrSigByApl | (1<<RgnIdx));

		// -------------------- clear by dtl --------------------
		if( u32_RgnDtl[RgnIdx] <= pParam->u32_logo_rgclr_dtl_thr && u32_RgnApl[RgnIdx] >= pParam->u32_logo_rgclr_apl2dtl_thr )
			u32_ClrSigByDtl = (u32_ClrSigByDtl | (1<<RgnIdx));

		// -------------------- clear by logo count --------------------
		if( u32_RgnPxlCnt[RgnIdx] >= u32_PxlRgClrCntThr )
			u32_ClrSigByCnt = (u32_ClrSigByCnt | (1<<RgnIdx));

		// -------------------- clear by logo and zmv count comparison --------------------
		u32_RgnClrCmpThr = ((u32_RgnZmvCnt[RgnIdx]*pParam->u32_logo_rgclr_cmp_gain_thr)/64 + pParam->u32_logo_rgclr_cmp_ofst_thr*8);
		if( (u32_RgnPxlCnt[RgnIdx]/16) >= u32_RgnClrCmpThr )
			u32_ClrSigByCmp = (u32_ClrSigByCmp | (1<<RgnIdx));

#if 0
#if 1
		if( printkEnable == 1 && printkCounter == 0 )
#else
		if( printkEnable == 1 && printkCounter == 0 && (RgnIdx<=7 || RgnIdx>=24) )
#endif
		{
			rtd_pr_memc_debug("[%2d] cmp = %4d, thr = %4d ", RgnIdx, (u32_RgnPxlCnt[RgnIdx]/4), u32_RgnClrCmpThr );
			if((RgnIdx+1)%4==0)
				rtd_pr_memc_debug("\n");
		}
#endif
	}

	pOutput->u32_PxlRgClrSig = 0;
	if( pParam->u1_logo_rgclr_apl_en == 1 )
		pOutput->u32_PxlRgClrSig = pOutput->u32_PxlRgClrSig | u32_ClrSigByApl;
	if( pParam->u1_logo_rgclr_dtl_en == 1 )
		pOutput->u32_PxlRgClrSig = pOutput->u32_PxlRgClrSig | u32_ClrSigByDtl;
	if( pParam->u1_logo_rgclr_cnt_en == 1 )
		pOutput->u32_PxlRgClrSig = pOutput->u32_PxlRgClrSig | u32_ClrSigByCnt;
	if( pParam->u1_logo_rgclr_cmp_en == 1 )
		pOutput->u32_PxlRgClrSig = pOutput->u32_PxlRgClrSig | u32_ClrSigByCmp;
#if 0
	if( printkEnable == 1 && printkCounter == 0 )
	{
		rtd_pr_memc_debug("PxlCntThr = %d, Sig = apl[%08x] dtl[%08x] cnt[%08x] cmp[%08x] out[%08x]\n",
			u32_PxlRgClrCntThr,
			u32_ClrSigByApl, u32_ClrSigByDtl, u32_ClrSigByCnt, u32_ClrSigByCmp,
			pOutput->u32_PxlRgClrSig);
	}

	printkCounter = printkCounter + 1;
	if( printkCounter >= printkPeriod )
		printkCounter = 0;
#endif
}
#endif

VOID FRC_LogoDet_Proc(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	unsigned int i = 0;

	if (pParam->u1_logo_pql_en == 1)
	{
		//const _PQLCONTEXT *s_pContext		= GetPQLContext();
		pOutput->u32_blklogo_clr_idx		= 0;
		pOutput->u1_blkclr_glbstatus		= 0;
		pOutput->u1_logo_cnt_glb_clr_status = 0;
		pOutput->u1_logo_sc_glb_clr_status  = 0;
		pOutput->u1_sw_clr_en_out           = pParam->u1_logo_sw_clr_en;
		pOutput->u4_blk_pix_merge_type_out	= pParam->u4_blk_pix_merge_type;
		pOutput->u32_logo_hsty_clr_idx      = 0;
		pOutput->u32_logo_hsty_clr_lgclr_idx = 0;

		FRC_LgDet_RgHstyClr(pParam, pOutput);
		FRC_LgDet_BlkClrCtrl(pParam, pOutput);
		FRC_LgDet_netflix_detect(pParam, pOutput);
		FRC_LgDet_SCCtrl(pParam, pOutput);
		FRC_LgDet_CloseVar(pParam, pOutput);
		//FRC_LgDet_RgDhClr(pParam, pOutput);
		//FRC_LgDet_RgHstyClr(pParam, pOutput);
		FRC_LgDet_SC_Detect(pParam, pOutput);
		FRC_LgDet_SC_FastDetectionCtrl(pParam, pOutput);
		FRC_LgDet_RgnDistribute(pParam, pOutput);
		//FRC_LgDet_LG_16s_Patch(pParam, pOutput);
		//FRC_LgDet_RgDhAdp_Rimunbalance(pParam, pOutput);
		//FRC_LgDet_LG_UXN_Patch(pParam, pOutput);
#ifdef CONFIG_SUPPORT_SLD_BY_MEMC_DEMURA
		FRC_SLD_PxlTargetYVal(pParam, pOutput);
		FRC_SLD_PxlRgnClr(pParam, pOutput);
#endif
		FRC_LgDet_LogoHaloEnhance(pParam, pOutput);
		FRC_LgDet_DynamicOBME(pParam, pOutput);
		FRC_DhLgProc_DynamicLevel(pParam, pOutput);
		FRC_DhLgProc_H_MovingCase(pParam, pOutput);
		FRC_LgDet_BG_still_detect(pParam, pOutput);
		FRC_LgDet_KeyRgn_detect(pParam, pOutput);
		FRC_LgDet_pure_panning_detect(pParam, pOutput);
		FRC_LogoDet_NearRim_logo(pParam, pOutput);
		FRC_LogoDet_NearRim_dh_blklogo(pParam, pOutput);

	// HS merlin8 logo
			FRC_Logo_long_term_logo_rg_replace_mv(pParam,pOutput);
	// HS merlin8 logo
			FRC_LogoDet_StillLogoDetection(pParam,pOutput);
	}
	else
	{
	    pOutput->u1_sw_clr_en_out = 1;
		pOutput->u4_blk_pix_merge_type_out = 0;
		pOutput->u32_blklogo_clr_idx  = 0;
		pOutput->u5_iir_alpha_out = 0x1F;
		pOutput->u32_logo_hsty_clr_idx      = 0;
		pOutput->u32_logo_hsty_clr_lgclr_idx = 0;

		pOutput->u1_logo_netflix_status = 0;
		pOutput->u8_logo_netflix_hold_frm = 0;

		pOutput->u1_logo_lg16s_patch_status   = 0;
		pOutput->u8_logo_lg16s_patch_hold_frm = 0;

		pOutput->u1_logo_lg16s_patch_Gapli_status     = 0;
		pOutput->u1_logo_lg16s_patch_Gaplp_status     = 0;
		pOutput->u1_logo_lg16s_patch_Gdtl_status     = 0;
		pOutput->u1_logo_lg16s_patch_Gsad_status     = 0;
		pOutput->u1_logo_lg16s_patch_Gmv_status      = 0;
		pOutput->u1_logo_lg16s_patch_Rdtl_num_status = 0;

		pOutput->u1_logo_sc_FastDet_status = 0;
		pOutput->u8_logo_sc_FastDet_cntholdfrm = 0;

		pOutput->u1_logo_BG_still_status = 0;
		pOutput->u8_logo_BG_still_hold_frm = 0;
		pOutput->u1_logo_KeyRgn_status = 0;
		pOutput->u8_logo_KeyRgn_hold_frm = 0;
		pOutput->u1_logo_PurePanning_status = 0;
		pOutput->u8_logo_PurePanning_hold_frm = 0;
		pOutput->u1_logo_NearRim_logo_status = 0;
		pOutput->u8_logo_NearRim_logo_hold_frm = 0;

		for(i=0;i<5;i++){
			pOutput->u1_logo_NearRim_dh_blklogo_status[i] = 0;
			pOutput->u8_logo_NearRim_dh_blklogo_hold_frm[i] = 0;
		}
	}
}


#if 0 // rremove from Merlin2

VOID FRC_LogoDet_Init(_OUTPUT_FRC_LGDet *pOutput)
{
	int i;
	pOutput->s16_blklogo_glbclr_cnt = 0;
	//pOutput->s16_pixlogo_glbclr_cnt = 0;

	//pOutput->s16_SC_clrAlpha_cnt = 0;
	//pOutput->s16_SC_clrLogo_cnt  = 0;
	//pOutput->s16_SC_holdHsty_cnt = 0;

	for (i=0; i<LOGO_RG_32; i++)
	{
		*(pOutput->s16_blklogo_rgclr_cnt + i) = 0;
		//*(pOutput->s16_pixlogo_rgclr_cnt + i) = 0;
	}

	//WriteRegister(KME_LOGO1_KME_LOGO1_E0_reg,30,30, 0);
}

VOID FRC_LgDet_RB(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	unsigned int idx;

	//////////////////////////////////////////////////////////////////////////
	for (idx=0; idx < RG_32/2; idx++)
	{
		ReadRegister(KME_LOGO2_KME_LOGO2_40_reg + idx *4,
			0,
			13, pOutput->u14_pixlogo_rgcnt + idx*2);

		ReadRegister(KME_LOGO2_KME_LOGO2_40_reg + idx *4,
			14,
			27, pOutput->u14_pixlogo_rgcnt + idx*2 + 1);
	}

	for (idx=0; idx < RG_32; idx++)
	{
		ReadRegister(KME_LOGO2_KME_LOGO2_80_reg + idx *4,
			0,
			19, pOutput->u20_pixlogo_rgaccY + idx);
	}

	for (idx=0; idx < RG_32; idx++)
	{
		ReadRegister(KME_LOGO2_KME_LOGO2_80_reg + idx *4,
			20,
			29, pOutput->u10_blklogo_rgcnt + idx);
	}


	//readback gmv
	for (idx=0; idx < RG_32; idx++)
	{
		unsigned int tmp;
		ReadRegister(FRC_TOP__KME_ME_TOP2__regr_metop_gmv_gmvratio00_rdback_ADDR + idx*4,
			0, 7, &tmp);
		*(pOutput->s16_gmv_y_max + idx) = tmp < 128 ? tmp : tmp-256;
		ReadRegister(FRC_TOP__KME_ME_TOP2__regr_metop_gmv_gmvratio00_rdback_ADDR + idx*4,
			8, 16, &tmp);
		*(pOutput->s16_gmv_x_max + idx) = tmp < 256 ? tmp : tmp-512;
		ReadRegister(FRC_TOP__KME_ME_TOP2__regr_metop_gmv_gmvratio00_rdback_ADDR + idx*4,
			24, 31,&tmp);
		*(pOutput->u8_gmv_rat_max + idx) = tmp;


		ReadRegister(FRC_TOP__KME_ME_TOP5__regr_metop_gmv_gmvsec00_rdback_ADDR + idx*4,
			0, 7, &tmp);
		*(pOutput->s16_gmv_y_sec + idx) = tmp < 128 ? tmp : tmp-256;
		ReadRegister(FRC_TOP__KME_ME_TOP5__regr_metop_gmv_gmvsec00_rdback_ADDR + idx*4,
			8, 16, &tmp);
		*(pOutput->s16_gmv_x_sec + idx) = tmp < 256 ? tmp : tmp-512;
		ReadRegister(FRC_TOP__KME_ME_TOP5__regr_metop_gmv_gmvsec00_rdback_ADDR + idx*4,
			24, 31,&tmp);
		*(pOutput->u8_gmv_rat_sec + idx) = tmp;
	}

	for (idx=0; idx < RG_32-2; idx++)
	{
		ReadRegister(FRC_TOP__KME_ME_TOP2__regr_metop_rdbk_sad_region00_ADDR + idx *4,
			FRC_TOP__KME_ME_TOP2__regr_metop_rdbk_sad_region00_BITSTART,
			FRC_TOP__KME_ME_TOP2__regr_metop_rdbk_sad_region00_BITEND, pOutput->u32_region_sad_sum + idx);
	}

	ReadRegister(FRC_TOP__KME_ME_TOP3__regr_metop_rdbk_sad_region36_ADDR + idx *4,
		FRC_TOP__KME_ME_TOP3__regr_metop_rdbk_sad_region36_BITSTART,
		FRC_TOP__KME_ME_TOP3__regr_metop_rdbk_sad_region36_BITEND, pOutput->u32_region_sad_sum + 30);

	ReadRegister(FRC_TOP__KME_ME_TOP3__regr_metop_rdbk_sad_region37_ADDR + idx *4,
		FRC_TOP__KME_ME_TOP3__regr_metop_rdbk_sad_region37_BITSTART,
		FRC_TOP__KME_ME_TOP3__regr_metop_rdbk_sad_region37_BITEND, pOutput->u32_region_sad_sum + 31);

}



VOID FRC_LgDet_DS_RgY(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	unsigned int u32_logo_pxlrgydf_bypass = 0;

	if (pParam->u1_logo_dynY_en == 1)
	{
		int j,i;
		for (j = 0; j < RG_32/4; j++)
		{
			unsigned int data_tmp = 0;
			for (i = 0; i < 4; i++)
			{
				unsigned int pixlogo_rg_Y   = s_pContext->_output_FRC_LgDet.u20_pixlogo_rgaccY[j*4+i];
				unsigned int pixlogo_rg_cnt = s_pContext->_output_FRC_LgDet.u14_pixlogo_rgcnt[j*4+i];

				if (pixlogo_rg_cnt != 0)
				{
					pixlogo_rg_Y = (pixlogo_rg_Y << 2) / pixlogo_rg_cnt;
				}
				else
				{
					pixlogo_rg_Y = 128;
					u32_logo_pxlrgydf_bypass = u32_logo_pxlrgydf_bypass | (1<< (i + j*4));
				}

				data_tmp = data_tmp | (pixlogo_rg_Y << (i*8));
			}
			WriteRegister(KME_LOGO2_KME_LOGO2_00_reg+j*4,0,31, data_tmp);
		}
	}
	else
	{
		u32_logo_pxlrgydf_bypass = 0xFFFFFFFF;
	}

	WriteRegister(KME_LOGO2_KME_LOGO2_24_reg,0,31, u32_logo_pxlrgydf_bypass);
}

VOID FRC_LgDet_DS_Delg(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	/*	const _PQLCONTEXT *s_pContext = GetPQLContext();*/

	int* gmv_x_max = pOutput->s16_gmv_x_max;
	int* gmv_y_max = pOutput->s16_gmv_y_max;
	unsigned char*  gmv_r_max = pOutput->u8_gmv_rat_max;

	int* gmv_x_sec = pOutput->s16_gmv_x_sec;
	int* gmv_y_sec = pOutput->s16_gmv_y_sec;
	unsigned char*  gmv_r_sec = pOutput->u8_gmv_rat_sec;

	if (pParam->u1_logo_dynDelogo_en == 1)
	{
		//for giraffe's Philips logo, maybe need more detail feature.
		//current use first, sec gmv, and ratio.(ratio max is 128)
		//unsigned int idx;
		//unsigned int mvx[2], mvy[2], rat[2], mvsum[2];

		//for (idx = 0; idx<2; idx++)
		//{
		//	ReadRegister(FRC_TOP__KME_ME_TOP2__regr_metop_gmv_gmvratio00_rdback_ADDR + idx*4,
		//		0, 7, mvy + idx);
		//	ReadRegister(FRC_TOP__KME_ME_TOP2__regr_metop_gmv_gmvratio00_rdback_ADDR + idx*4,
		//		8, 16,mvx + idx);
		//	ReadRegister(FRC_TOP__KME_ME_TOP2__regr_metop_gmv_gmvratio00_rdback_ADDR + idx*4,
		//		24, 31,rat + idx);
		//	//abs
		//	mvx[idx]   = _CLIP_(mvx[idx], 0, 0xFF);
		//	mvy[idx]   = _CLIP_(mvy[idx], 0, 0x7F);
		//	mvsum[idx] = mvx[idx] + mvy[idx];
		//}

		//if (mvsum[0] >= pParam->u16_dynDelogo_mv_th && mvsum[1] >= pParam->u16_dynDelogo_mv_th &&
		//	rat[0] >= pParam->u8_dynDelogo_rat_th && rat[1] >= pParam->u8_dynDelogo_rat_th)
		//{
		//	//need test hor/ver for fast gmv motion, delogo.
		//	WriteRegister(FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_ADDR no mat,FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_BITSTART no mat,FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_BITEND no mat, 7);
		//	WriteRegister(FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_ADDR no mat,FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_BITSTART no mat,FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_BITEND no mat, 1);
		//}
		//else
		//{
		//	WriteRegister(FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_ADDR no mat,FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_BITSTART no mat,FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_BITEND no mat, 0);
		//	WriteRegister(FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_ADDR no mat,FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_BITSTART no mat,FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_BITEND no mat, 0);
		//}

		unsigned int idx;
		unsigned int gg_num = 0;
		for (idx=0; idx < RG_32; idx++)
		{
			unsigned int mv_diff  = abs(gmv_x_max[idx] - gmv_x_sec[idx]) + abs(gmv_y_max[idx] - gmv_y_sec[idx]);
			unsigned int mv_alpha = mv_diff <= pParam->u8_gg_mvd12_thr ? gmv_r_max[idx] + gmv_r_sec[idx] - gmv_r_sec[idx]*mv_diff/ pParam->u8_gg_mvd12_thr : gmv_r_max[idx];
			if ((abs(gmv_x_max[idx]) > pParam->u8_gg_mvx_thr) && (abs(gmv_y_max[idx]) > pParam->u8_gg_mvy_thr) &&
				(mv_alpha > pParam->u8_gg_alpha_thr) && (*(pOutput->u32_region_sad_sum + idx) < pParam->u32_gg_sad_thr))
			{
				gg_num++;
			}
		}

		if (gg_num > pParam->u8_gg_num_thr)
		{
			WriteRegister(FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_ADDR no mat,FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_BITSTART no mat,FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_BITEND no mat, 7);
			WriteRegister(FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_ADDR no mat,FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_BITSTART no mat,FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_BITEND no mat, 1);
		}
		else
		{
			WriteRegister(FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_ADDR no mat,FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_BITSTART no mat,FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_BITEND no mat, pParam->u8_reg_mvd);
			WriteRegister(FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_ADDR no mat,FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_BITSTART no mat,FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_BITEND no mat, pParam->u8_reg_tmv);
		}
	}
	else
	{
		//need set to default setting.
		WriteRegister(FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_ADDR no mat,FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_BITSTART no mat,FRC_TOP__DEHALO__reg_dh_logo_mvd_bypass_BITEND no mat, pParam->u8_reg_mvd);
		WriteRegister(FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_ADDR no mat,FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_BITSTART no mat,FRC_TOP__DEHALO__reg_dh_logo_tmv_bypass_BITEND no mat, pParam->u8_reg_tmv);
	}
}


VOID FRC_LgDet_DS_ThrStep(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	/*	const _PQLCONTEXT *s_pContext = GetPQLContext();*/

	if (pParam->u1_logo_dynBlkTh_en == 1 &&  pOutput->u32_frm_cnt >= pParam->u8_dynBlkTh_cnt)
	{
		WriteRegister(KME_LOGO0_KME_LOGO0_84_reg,25,25, 1);
	}
	else
	{
		WriteRegister(KME_LOGO0_KME_LOGO0_84_reg,25,25, 0);
	}

	if (pParam->u1_logo_dynPixTh_en == 1 &&  pOutput->u32_frm_cnt >= pParam->u8_dynPixTh_cnt)
	{
		WriteRegister(KME_LOGO1_KME_LOGO1_94_reg,8,8,   1);
	}
	else
	{
		WriteRegister(KME_LOGO1_KME_LOGO1_94_reg,8,8,   0);
	}


	if (pParam->u1_logo_dynBlkStep_en == 1 &&  pOutput->u32_frm_cnt >= pParam->u8_dynBlkStep_cnt)
	{
		WriteRegister(KME_LOGO0_KME_LOGO0_EC_reg,8,8, 1);
		WriteRegister(KME_LOGO0_KME_LOGO0_EC_reg,9,9, pParam->u1_blkHsty_upmet);
	}
	else
	{
		WriteRegister(KME_LOGO0_KME_LOGO0_EC_reg,8,8, 0);
		WriteRegister(KME_LOGO0_KME_LOGO0_EC_reg,9,9, 0);
	}

	if (pParam->u1_logo_dynPixStep_en == 1 &&  pOutput->u32_frm_cnt >= pParam->u8_dynPixStep_cnt)
	{
		WriteRegister(KME_LOGO1_KME_LOGO1_98_reg,10,10, 1);
	}
	else
	{
		WriteRegister(KME_LOGO1_KME_LOGO1_98_reg,10,10, 0);
	}
}



VOID FRC_LgDet_DynSet(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	//#1: Dyn regional Y set.
	FRC_LgDet_DS_RgY(pParam, pOutput);

	//#2: Dyn. use Adp. thr, step.
	FRC_LgDet_DS_ThrStep(pParam, pOutput);

	//#3. Fast,Good GMV and abandon dehalo de-logo (mvd,zmv)
	FRC_LgDet_DS_Delg(pParam, pOutput);

}

VOID FRC_LgDet_PixClrCtrl(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	unsigned int idx;
	unsigned int acc_pix_num;
	int*  hold_cnt_rg  = pOutput->s16_pixlogo_rgclr_cnt;

	acc_pix_num = 0;
	for (idx = 0; idx < RG_32; idx++)
	{
		unsigned int pixlogo_rg_cnt = s_pContext->_output_FRC_LgDet.u14_pixlogo_rgcnt[idx];
		if (((pixlogo_rg_cnt * 256 / PIX_LOGO_RG_PIXCNT_GOLDEN) >= pParam->u8_pixRgClr_thr) && pParam->u1_logo_pixRgClr_en == 1)
		{
			pOutput->u32_pixlogo_clr_idx = pOutput->u32_pixlogo_clr_idx | (1 << idx);
			hold_cnt_rg[idx] = pParam->u6_rgClr_holdtime;
		}
		else if ((hold_cnt_rg[idx] > 1) && pParam->u1_logo_pixRgClr_en == 1)
		{
			pOutput->u32_pixlogo_clr_idx = pOutput->u32_pixlogo_clr_idx | (1 << idx);
			hold_cnt_rg[idx] = _MAX_(hold_cnt_rg[idx] - 1,0);
#if LOGO_DEBUG_PRINTF
			if (idx == 15 && pParam->u1_printf_en==1)  LogPrintf(DBG_MSG,"pix logo region(idx=15) clear cnt: %d  \r\n",hold_cnt_rg[idx]);
#endif
		}

		acc_pix_num += pixlogo_rg_cnt;
	}

	if ((((acc_pix_num * 256) / (PIX_LOGO_PIXCNT_GOLDEN)) >= pParam->u8_pixGlbClr_thr) &&  pParam->u1_logo_pixGlbClr_en == 1)
	{
		pOutput->u32_pixlogo_clr_idx    = pOutput->u32_pixlogo_clr_idx | 0xFFFFFFFF;
		pOutput->s16_pixlogo_glbclr_cnt = pParam->u6_GlbClr_holdtime;
		pOutput->u1_pixclr_glbstatus    = 1;
	}
	else if (pOutput->s16_pixlogo_glbclr_cnt > 1 &&  pParam->u1_logo_pixGlbClr_en == 1)
	{
		pOutput->u32_pixlogo_clr_idx    = pOutput->u32_pixlogo_clr_idx | 0xFFFFFFFF;
		pOutput->s16_pixlogo_glbclr_cnt = _MAX_(pOutput->s16_pixlogo_glbclr_cnt - 1,0);
		pOutput->u1_pixclr_glbstatus    = 1;
	}

#if LOGO_DEBUG_PRINTF
	if ((pOutput->u32_frm_cnt%500) == 0 && pParam->u1_printf_en==1)  LogPrintf(DBG_MSG,"pix logo clear: %d  \r\n",pOutput->u1_pixclr_glbstatus);
	if ((pOutput->u32_frm_cnt%500) == 0 && pParam->u1_printf_en==1)  LogPrintf(DBG_MSG,"accum pix logo: %d  \r\n",acc_pix_num);
#endif

}

//for clear logo (glb, rg), just set the clr status.
VOID FRC_LgDet_BlkClrCtrl(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	unsigned int idx;
	unsigned int acc_blk_num;
	unsigned int acc_left_blk_num = 0;
	unsigned int acc_right_blk_num = 0;
	unsigned int acc_top_blk_num = 0;
	unsigned int acc_bottom_blk_num = 0;
	int*  hold_cnt_rg  = pOutput->s16_blklogo_rgclr_cnt;

	acc_blk_num = 0;
	for (idx = 0; idx < RG_32; idx++)
	{
		unsigned int blklogo_rg_cnt = s_pContext->_output_FRC_LgDet.u10_blklogo_rgcnt[idx];
		if ((((blklogo_rg_cnt * 256) / (BLK_LOGO_RG_BLKCNT_GOLDEN)) >= pParam->u8_clr_rg_thr) && pParam->u1_logo_rg_clr_en == 1)
		{
			pOutput->u32_blklogo_clr_idx = pOutput->u32_blklogo_clr_idx | (1 << idx);
			hold_cnt_rg[idx] = pParam->u6_rgClr_holdtime;
		}
		else if ((hold_cnt_rg[idx] > 1)  && pParam->u1_logo_rg_clr_en == 1)
		{
			pOutput->u32_blklogo_clr_idx = pOutput->u32_blklogo_clr_idx | (1 << idx);
			hold_cnt_rg[idx] = _MAX_(hold_cnt_rg[idx] - 1,0);
#if LOGO_DEBUG_PRINTF
			if (idx == 15 && pParam->u1_printf_en==1)  LogPrintf(DBG_MSG,"blk logo region(idx=15) clear cnt: %d  \r\n",hold_cnt_rg[idx]);
#endif
		}

		//CMO
		if ((((blklogo_rg_cnt * 256) / (BLK_LOGO_RG_BLKCNT_GOLDEN)) >= pParam->u8_cmo_clear_thr) && pParam->u1_logo_rg_clr_en == 1
					&& pParam->u1_logo_dynAlpha_en == 1 && (((pParam->u32_cmo_clear_idx >> idx) & 0x1) == 1))
		{
			pOutput->u32_blklogo_clr_idx = pOutput->u32_blklogo_clr_idx | (1 << idx);
#if LOGO_DEBUG_PRINTF
			if (pParam->u1_printf_en==1)	 LogPrintf(DBG_MSG,"CMO logo: %d region clear, cnt: %d  \r\n",idx,blklogo_rg_cnt);
#endif
		}

		acc_blk_num += blklogo_rg_cnt;

		if (idx < 16)
		{
			acc_top_blk_num += blklogo_rg_cnt;
		}
		else
		{
			acc_bottom_blk_num += blklogo_rg_cnt;
		}
		if (idx%8 <4)
		{
			acc_left_blk_num += blklogo_rg_cnt;
		}
		else
		{
			acc_right_blk_num += blklogo_rg_cnt;
		}

	}


	if (((((acc_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN)) >= pParam->u8_clr_glb_thr) && pParam->u1_logo_glb_clr_en == 1)
		|| ((((acc_left_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && ((pParam->u4_logo_half_clr_en & 0x1) == 1))
		|| ((((acc_right_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (((pParam->u4_logo_half_clr_en>>1) & 0x1) == 1))
		|| ((((acc_top_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (((pParam->u4_logo_half_clr_en>>2) & 0x1) == 1))
		|| ((((acc_bottom_blk_num * 256) / (BLK_LOGO_BLKCNT_GOLDEN/2)) >= pParam->u8_clr_half_thr) && (((pParam->u4_logo_half_clr_en>>3) & 0x1) == 1)))
	{
		pOutput->u32_blklogo_clr_idx    = pOutput->u32_blklogo_clr_idx | 0xFFFFFFFF;
		pOutput->s16_blklogo_glbclr_cnt = pParam->u6_GlbClr_holdtime;
		pOutput->u1_blkclr_glbstatus = 1;
	}
	else if (pOutput->s16_blklogo_glbclr_cnt > 1 &&  pParam->u1_logo_glb_clr_en == 1)
	{

		pOutput->u32_blklogo_clr_idx    = pOutput->u32_blklogo_clr_idx | 0xFFFFFFFF;
		pOutput->s16_blklogo_glbclr_cnt = _MAX_(pOutput->s16_blklogo_glbclr_cnt - 1,0);
		pOutput->u1_blkclr_glbstatus = 1;
	}

	//for disp.
	//if (pOutput->u1_blkclr_glbstatus == 1 || s_pContext->_output_read_comreg.u1_sc_status_rb == 1)
	//{
	//	WriteRegister(KPOST_TOP_KPOST_TOP_A0_reg,0,15, 1);
	//}
	//else
	//{
	//	WriteRegister(KPOST_TOP_KPOST_TOP_A0_reg,0,15, 0);
	//}

#if LOGO_DEBUG_PRINTF
	if ((pOutput->u32_frm_cnt%500) == 0 && pParam->u1_printf_en==1)  LogPrintf(DBG_MSG,"blk logo clear: %d  \r\n",pOutput->u1_blkclr_glbstatus);
	if ((pOutput->u32_frm_cnt%500) == 0 && pParam->u1_printf_en==1)  LogPrintf(DBG_MSG,"accum blk logo: %d  \r\n",acc_blk_num);
#endif
}

//current, use blk_logo idx to clear SC logo (global all)
//default is logoClr=1, holdHsty=1, clrAlpha=1
VOID FRC_LgDet_SCCtrl(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();

	pOutput->u5_alpha_bld           = pParam->u5_reg_iir_alpha;
	pOutput->u4_blklogo_pStep_cur   = pParam->u4_blklogo_pStep;
	pOutput->u4_pixlogo_pStep_cur   = pParam->u4_pixlogo_pStep;

	if (s_pContext->_output_read_comreg.u1_sc_status_rb == 1)
	{
		pOutput->u32_frm_cnt  = 0;  //for auto En
		if (pParam->u1_logo_SCalphaClr_en == 1)
		{
			pOutput->u5_alpha_bld = 0;
			pOutput->s16_SC_clrAlpha_cnt = pParam->u6_SCalpha_holdtime;
#if LOGO_DEBUG_PRINTF
			if (pParam->u1_printf_en==1)  LogPrintf(DBG_MSG,"alpha %d \r\n",pOutput->u5_alpha_bld);
#endif
		}
		if (pParam->u1_logo_SCLogoClr_en == 1)
		{
			//pOutput->u32_pixlogo_clr_idx = pOutput->u32_pixlogo_clr_idx | 0xFFFFFFFF;
			//pOutput->u1_pixclr_glbstatus = 1;

			pOutput->u32_blklogo_clr_idx = pOutput->u32_blklogo_clr_idx | 0xFFFFFFFF;
			pOutput->u1_blkclr_glbstatus = 1;

			pOutput->s16_SC_clrLogo_cnt  = pParam->u6_SCclrlogo_holdtime;
#if LOGO_DEBUG_PRINTF
			if (pParam->u1_printf_en==1)  LogPrintf(DBG_MSG,"SC clr pix logo \r\n");
#endif
		}
		if (pParam->u1_logo_SCHstyClr_en == 1)
		{
			pOutput->u4_blklogo_pStep_cur = 0;
			pOutput->u4_pixlogo_pStep_cur = 0;
			pOutput->s16_SC_holdHsty_cnt  = pParam->u6_SCclrHsty_holdtime;
#if LOGO_DEBUG_PRINTF
			if (pParam->u1_printf_en==1)	LogPrintf(DBG_MSG,"SC hold hsty \r\n");
#endif
		}
	}
	else
	{
		pOutput->u32_frm_cnt = _CLIP_(pOutput->u32_frm_cnt = pOutput->u32_frm_cnt + 1, 0, 0xFFFFFFF);
		if (pParam->u1_logo_SCalphaClr_en == 1 && pOutput->s16_SC_clrAlpha_cnt > 1)
		{
			pOutput->u5_alpha_bld = 0;
			pOutput->s16_SC_clrAlpha_cnt = _MAX_(pOutput->s16_SC_clrAlpha_cnt-1,0);
#if LOGO_DEBUG_PRINTF
			if (pParam->u1_printf_en==1)  LogPrintf(DBG_MSG,"alpha %d \r\n",pOutput->u5_alpha_bld);
#endif
		}
		if (pParam->u1_logo_SCLogoClr_en == 1 && pOutput->s16_SC_clrLogo_cnt > 1)
		{
			//pOutput->u32_pixlogo_clr_idx = pOutput->u32_pixlogo_clr_idx | 0xFFFFFFFF;
			//pOutput->u1_pixclr_glbstatus = 1;

			pOutput->u32_blklogo_clr_idx = pOutput->u32_blklogo_clr_idx | 0xFFFFFFFF;
			pOutput->u1_blkclr_glbstatus = 1;

			pOutput->s16_SC_clrLogo_cnt  = _MAX_(pOutput->s16_SC_clrLogo_cnt-1,0);
#if LOGO_DEBUG_PRINTF
			if (pParam->u1_printf_en==1)   LogPrintf(DBG_MSG,"SC clr pix logo, hold cnt:%d \r\n", pOutput->s16_SC_clrLogo_cnt);
#endif
		}
		if (pParam->u1_logo_SCLogoClr_en == 1 && pOutput->s16_SC_clrLogo_cnt > 1)
		{
			pOutput->u4_blklogo_pStep_cur = 0;
			pOutput->u4_pixlogo_pStep_cur = 0;
			pOutput->s16_SC_holdHsty_cnt  = _MAX_(pOutput->s16_SC_holdHsty_cnt-1,0);
#if LOGO_DEBUG_PRINTF
			if (pParam->u1_printf_en==1)  LogPrintf(DBG_MSG,"SC hold hsty, hold cnt:%d \r\n", pOutput->s16_SC_holdHsty_cnt);
#endif
		}
	}

	WriteRegister(KME_LOGO0_KME_LOGO0_10_reg,0,4,     pOutput->u5_alpha_bld);
	WriteRegister(KME_LOGO0_KME_LOGO0_EC_reg,4,7, pOutput->u4_blklogo_pStep_cur);
	WriteRegister(KME_LOGO1_KME_LOGO1_BC_reg,0,3, pOutput->u4_pixlogo_pStep_cur);
}

VOID FRC_LgDet_ClrAction(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{
	if(pParam->u4_reg_merge_type > 0 && pOutput->u32_blklogo_clr_idx > 0)
	{
		WriteRegister(MC2_MC2_20_reg,6,9, 0);
		WriteRegister(KME_LOGO1_KME_LOGO1_E4_reg,0,31, pOutput->u32_blklogo_clr_idx);
	}
	else
	{
		WriteRegister(MC2_MC2_20_reg,6,9, pParam->u4_reg_merge_type);
		WriteRegister(KME_LOGO1_KME_LOGO1_E4_reg,0,31, pOutput->u32_blklogo_clr_idx);
	}

	if ((pParam->u1_logo_blkRgClrHsty_en == 1) || (pParam->u1_logo_blkGlbClrHsty_en == 1 && pOutput->u1_blkclr_glbstatus == 1))
	{
		WriteRegister(KME_LOGO1_KME_LOGO1_EC_reg,0,31, pOutput->u32_blklogo_clr_idx);
	}
	else
	{
		WriteRegister(KME_LOGO1_KME_LOGO1_EC_reg,0,31, 0);
	}
}



VOID FRC_LogoDet_Proc(const _PARAM_FRC_LGDet *pParam, _OUTPUT_FRC_LGDet *pOutput)
{

	if (pParam->u1_logo_pql_en == 1)
	{

		FRC_LgDet_BlkClrCtrl(pParam, pOutput);
	}


	if (pParam->u1_logo_pql_en == 1)
	{
		const _PQLCONTEXT *s_pContext = GetPQLContext();
		unsigned char  u1_gmv_logoClr = s_pContext->_output_me_sceneAnalysis.u1_logoClr_gmv_true;

#if LOGO_DEBUG_PRINTF
		if ((pOutput->u32_frm_cnt%500) == 5 && pParam->u1_printf_en==1)   LogPrintf(DBG_MSG,"Logo PQL Enable \r\n");
#endif

		pOutput->u32_blklogo_clr_idx = 0;
		pOutput->u32_pixlogo_clr_idx = 0;
		pOutput->u1_blkclr_glbstatus = 0;
		pOutput->u1_pixclr_glbstatus = 0;

		//1st read-back
		FRC_LgDet_RB(pParam, pOutput);

		FRC_LgDet_SCCtrl(pParam, pOutput);

		if(u1_gmv_logoClr == 1)
		{
			pOutput->u32_blklogo_clr_idx = pOutput->u32_blklogo_clr_idx | 0xFFFFFFFF;
			pOutput->u1_blkclr_glbstatus = 1;
		}

		FRC_LgDet_BlkClrCtrl(pParam, pOutput);
		//FRC_LgDet_PixClrCtrl(pParam, pOutput);

		//use blk statics, use pixclr reg to clear.
		FRC_LgDet_ClrAction(pParam, pOutput);

		//local dynamic set, if clear/sc = manual setting.
		FRC_LgDet_DynSet(pParam, pOutput);
	}

}
#endif
