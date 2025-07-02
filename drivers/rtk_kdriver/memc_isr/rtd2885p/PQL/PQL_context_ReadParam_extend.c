#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_reg_def.h"

/*Change the setting runtime!!  By this function!! */

VOID PQL_ContextProc_ReadParam_RTK2885pp(_PQLPARAMETER *pParam)
{
    unsigned int u32_RB_val;

    {
        // write com reg
        ReadRegister(M8P_SOFTWARE_SOFTWARE_00_reg, 0, 31, &u32_RB_val);
        pParam->_param_wrt_comreg.u1_Wrt_ComReg_Proc_en                  = (u32_RB_val >>  0) & 0x1;
        pParam->_param_wrt_comreg.u1_Wrt_ComReg_IPPRE_en                 = (u32_RB_val >>  1) & 0x1;
        pParam->_param_wrt_comreg.u1_Wrt_ComReg_BBD_en                   = (u32_RB_val >>  2) & 0x1;
        pParam->_param_wrt_comreg.u1_Wrt_ComReg_LOGO_en                  = (u32_RB_val >>  3) & 0x1;
        pParam->_param_wrt_comreg.u1_Wrt_ComReg_HME1_en                  = (u32_RB_val >>  4) & 0x1;
        pParam->_param_wrt_comreg.u1_Wrt_ComReg_ME15_en                  = (u32_RB_val >>  5) & 0x1;
        pParam->_param_wrt_comreg.u1_Wrt_ComReg_MISC_intputInterrupt_en  = (u32_RB_val >>  6) & 0x1;
        pParam->_param_wrt_comreg.u1_Wrt_ComReg_ME2_en                   = (u32_RB_val >>  7) & 0x1;
        pParam->_param_wrt_comreg.u1_Wrt_ComReg_DH_en                    = (u32_RB_val >>  8) & 0x1;
        pParam->_param_wrt_comreg.u1_Wrt_ComReg_MC_en                    = (u32_RB_val >>  9) & 0x1;
        pParam->_param_wrt_comreg.u1_Wrt_ComReg_MISC_outputInterrupt_en  = (u32_RB_val >> 10) & 0x1;


        ReadRegister(M8P_SOFTWARE_SOFTWARE_01_reg, 0, 31, &u32_RB_val);
        pParam->_param_wrt_comreg.u1_RimCtrl_wrt_en              = (u32_RB_val >> 0) & 0x01;
        pParam->_param_wrt_comreg.u1_PeriodicCtrl_lpf_wrt_en     = (u32_RB_val >> 1) & 0x01;
        pParam->_param_wrt_comreg.u1_PeriodicCtrl_avgbv_wrt_en   = (u32_RB_val >> 2) & 0x01;
        pParam->_param_wrt_comreg.u1_PeriodicCtrl_frq_wrt_en     = (u32_RB_val >> 3) & 0x01;
        pParam->_param_wrt_comreg.u1_SobjCtrl_wrt_en             = (u32_RB_val >> 4) & 0x1;


        ReadRegister(M8P_SOFTWARE_SOFTWARE_02_reg, 0, 31, &u32_RB_val);
        pParam->_param_read_comreg.u1_BBD_log_en    = (u32_RB_val >>  0) & 0x1;
        pParam->_param_rimctrl.u1_RimCtrl_en      = (u32_RB_val >>  1) & 0x1;
        pParam->_param_rimctrl.u1_log_en          = (u32_RB_val >>  2) & 0x1;
        pParam->_param_rimctrl.u1_rim_debounce_en = (u32_RB_val >>  3) & 0x1;
        pParam->_param_rimctrl.u1_sel_rim         = (u32_RB_val >>  4) & 0x1;      
        pParam->_param_mc_lbmcswitch.u1_lbmc_log_en       = (u32_RB_val >>  5) & 0x1; 
        pParam->_param_mc_lbmcswitch.u8_lbmcSwitch_bypass = (u32_RB_val >>  8) & 0x3;   // 0: auto, 1: normal, 2: single_up, 3: single_dow


        ReadRegister(M8P_SOFTWARE_SOFTWARE_03_reg, 0, 31, &u32_RB_val);
        pParam->_param_sobj_ctrl.u1_Sobj_en         =  (u32_RB_val >>  0) & 0x1;
        pParam->_param_sobj_ctrl.u1_Sobj_color_en   =  (u32_RB_val >>  1) & 0x1;   
      

        ReadRegister(M8P_SOFTWARE_SOFTWARE_04_reg, 0, 31, &u32_RB_val);
        pParam->_param_wrt_comreg.u1_mc_var_lpf_wrt_en       = (u32_RB_val >>  0) & 0x01;
        pParam->_param_wrt_comreg.u1_dehalo_occl_wrt_en      = (u32_RB_val >>  1) & 0x01;
        pParam->_param_wrt_comreg.u1_me15_invalid_wrt_en     = (u32_RB_val >>  2) & 0x01;
        pParam->_param_wrt_comreg.u1_hme1_near_freq_wrt_en   = (u32_RB_val >>  3) & 0x01;
        pParam->_param_wrt_comreg.u1_hme1_3drs_wrt_en        = (u32_RB_val >>  4) & 0x01;
        pParam->_param_wrt_comreg.u1_me2_3drs_wrt_en         = (u32_RB_val >>  5) & 0x01;
        pParam->_param_wrt_comreg.u1_ipme_lpf_wrt_en         = (u32_RB_val >>  6) & 0x01;
        pParam->_param_wrt_comreg.u1_mc_deflicker_wrt_en     = (u32_RB_val >>  7) & 0x01;
        
        pParam->_param_wrt_comreg.u1_hme1_mvmask_wrt_en     = (u32_RB_val >>  8) & 0x01;
        pParam->_param_wrt_comreg.u1_hme1_freqpush_wrt_en     = (u32_RB_val >>  9) & 0x01;
        pParam->_param_wrt_comreg.u1_me2_brokenfix_wrt_en     = (u32_RB_val >>  10) & 0x01;
        pParam->_param_wrt_comreg.u1_dh_occl_mvpred_wrt_en     = (u32_RB_val >>  11) & 0x01;
        pParam->_param_wrt_comreg.u1_mc_gfb_wrt_en     = (u32_RB_val >>  12) & 0x01;

        pParam->_param_me_sceneAnalysis.u1_motion_info_calc_en      = (u32_RB_val >> 16) & 0x01;
        pParam->_param_me_sceneAnalysis.u1_mc_var_lpf_calc_en       = (u32_RB_val >> 17) & 0x01;
        pParam->_param_me_sceneAnalysis.u1_unstable_score_calc_en   = (u32_RB_val >> 18) & 0x01;
        pParam->_param_flicker_ctrl.u1_mc_deflicker_calc_en         = (u32_RB_val >> 19) & 0x01;

        pParam->_param_fadeinout_ctrl.u1_fadeinout_calc_en         = (u32_RB_val >> 26) & 0x01;
        pParam->_param_fblevelctrl.u1_FBLevelCtrl_en         = (u32_RB_val >> 27) & 0x01;

        ReadRegister(M8P_SOFTWARE_SOFTWARE_05_reg, 0, 31, &u32_RB_val);
        pParam->_param_rimctrl.u12_rim_froce_top = (u32_RB_val >>  0) & 0xfff;    
        pParam->_param_rimctrl.u12_rim_froce_bot = (u32_RB_val >> 16) & 0xfff;    
        pParam->_param_rimctrl.u1_rim_force_en      = (u32_RB_val >> 31) & 0x1; 
            
        ReadRegister(M8P_SOFTWARE_SOFTWARE_06_reg, 0, 31, &u32_RB_val);
        pParam->_param_rimctrl.u12_rim_froce_rht = (u32_RB_val >>  0) & 0xfff;    
        pParam->_param_rimctrl.u12_rim_froce_lft = (u32_RB_val >> 16) & 0xfff;

        // logo FW control
        ReadRegister(M8P_SOFTWARE_SOFTWARE_07_reg, 0, 31, &u32_RB_val);
        pParam->_param_logo_ctrl.u1_logo_pql_en                     = (u32_RB_val >>  0) & 0x01;
        pParam->_param_logo_ctrl.u1_logo_longterm_logo_en           = (u32_RB_val >>  1) & 0x01;
        pParam->_param_logo_ctrl.u1_logo_dehalo_logo_en             = (u32_RB_val >>  2) & 0x01;
        pParam->_param_logo_ctrl.u1_logo_dehalo_logo_mode           = (u32_RB_val >>  3) & 0x03;    // 0: by longterm, 1: force strong, 2: force weak
        pParam->_param_logo_ctrl.u1_logo_dehalo_logo_boundary_en    = (u32_RB_val >>  5) & 0x01;
        pParam->_param_logo_ctrl.u1_logo_counter_en                 = (u32_RB_val >>  6) & 0x01;
        pParam->_param_filmDetectctrl.u1_mixMode_det_en = (u32_RB_val >> 14) & 0x1;    
        pParam->_param_filmDetectctrl.u1_mixMode_en = (u32_RB_val >> 15) & 0x1;    
        pParam->_param_filmDetectctrl.u32_filmDet_cadence_en = (u32_RB_val >> 16) & 0xffff;    

        // phase table
        ReadRegister(M8P_SOFTWARE_SOFTWARE_08_reg, 0, 31, &u32_RB_val);
        pParam->_param_frc_phtable.u1_phT_autoGen_en          =       u32_RB_val          & 0x01;
        pParam->_param_frc_phtable.u8_dejudder_lvl            =      (u32_RB_val >>  1)   & 0x1F;
        pParam->_param_frc_phtable.u8_deblur_lvl              =      (u32_RB_val >>  6)   & 0x1F;
        pParam->_param_frc_phtable.u2_candece_id_mode         =      (u32_RB_val >> 11)   & 0x03;
        pParam->_param_frc_phtable.u4_cadence_id              =      (u32_RB_val >> 13)   & 0x0F;
        pParam->_param_frc_phtable.in_3dFormat                =      (u32_RB_val >> 17)   & 0x07;
        pParam->_param_frc_phtable.out_3dFormat               =      (u32_RB_val >> 20)   & 0x03;
        pParam->_param_frc_phtable.u2_outMode                 =      (u32_RB_val >> 22)   & 0x03;
        pParam->_param_frc_phtable.u1_outLR_LRsamePh          =      (u32_RB_val >> 24)   & 0x01;
        pParam->_param_frc_phtable.u1_filmPh_sw_en            =      (u32_RB_val >> 25)   & 0x01;
        pParam->_param_frc_phtable.u1_filmPh_pf_en            =      (u32_RB_val >> 26)   & 0x01;
        pParam->_param_frc_phtable.u1_force_gen_table_en            =      (u32_RB_val >> 27)   & 0x01;

        // film detect
        ReadRegister(M8P_SOFTWARE_SOFTWARE_09_reg, 0, 31, &u32_RB_val);
        pParam->_param_filmDetectctrl.u1_filmDet_bypass			=  u32_RB_val        & 0x01;
        pParam->_param_filmDetectctrl.u1_FrcCadSupport_En		= (u32_RB_val >> 1) & 0x1;
        pParam->_param_filmDetectctrl.u3_DbgPrt_Rgn         = (u32_RB_val >> 2) & 0x1FF;
        pParam->_param_filmDetectctrl.u1_BadEditSlowOut_En		= (u32_RB_val >> 11) & 0x1;
        pParam->_param_filmDetectctrl.u1_QuickSwitch_En		= (u32_RB_val >> 12) & 0x1;
        pParam->_param_filmDetectctrl.u1_StillDetect_En			= (u32_RB_val >> 13) & 0x1;
        pParam->_param_filmDetectctrl.u1_DbgPrt_FRChg_En		= (u32_RB_val >> 14) & 0x1;

        ReadRegister(M8P_SOFTWARE_SOFTWARE_10_reg, 0, 31, &u32_RB_val);
        pParam->_param_filmDetectctrl.u4_Mot_rgn_diffWgt   =    u32_RB_val            & 0x0F;
        pParam->_param_filmDetectctrl.u4_Mot_all_diffWgt   =   (u32_RB_val  >>  4)    & 0x0F;
        pParam->_param_filmDetectctrl.u27_Mot_all_min      =   (u32_RB_val  >>  8)    & 0xFFFFFF;

        ReadRegister(M8P_SOFTWARE_SOFTWARE_11_reg, 0, 31, &u32_RB_val);
        pParam->_param_filmDetectctrl.u4_enter_22_cntGain     =    u32_RB_val            & 0x0F;
        pParam->_param_filmDetectctrl.u4_enter_32_cntGain     =   (u32_RB_val  >>  4)    & 0x0F;
        pParam->_param_filmDetectctrl.u4_enter_else_cntGain   =   (u32_RB_val  >>  8)    & 0x0F;
        pParam->_param_filmDetectctrl.u4_quit_bigMot_wgt      =   (u32_RB_val  >> 12)    & 0x0F;
        pParam->_param_filmDetectctrl.u4_quit_smlMot_wgt      =   (u32_RB_val  >> 16)    & 0x0F;
        pParam->_param_filmDetectctrl.u1_quit_motWgt_auto_en  =   (u32_RB_val  >> 20)    & 0x01;
        pParam->_param_filmDetectctrl.u5_dbg_param1           =   (u32_RB_val  >> 21)    & 0x1F;
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0]     =   (u32_RB_val  >> 26)    & 0x01;
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[1]     =   (u32_RB_val  >> 27)    & 0x01;
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[2]     =   (u32_RB_val  >> 28)    & 0x01;
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[3]     =   (u32_RB_val  >> 29)    & 0x01;
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[4]     =   (u32_RB_val  >> 30)    & 0x01;

        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[5]     =   pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0];
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[6]     =   pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0];
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[7]     =   pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0];
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[8]     =   pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0];
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[9]     =   pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0];
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[10]     =   pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0];
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[11]     =   pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0];
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[12]     =   pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0];
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[13]     =   pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0];
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[14]     =   pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0];
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[15]     =   pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0];
        pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[16]     =   pParam->_param_filmDetectctrl.u1_mixMode_rgnEn[0];

        ReadRegister(M8P_SOFTWARE_SOFTWARE_12_reg, 0, 31, &u32_RB_val);
        pParam->_param_filmDetectctrl.u8_mixMode_enter_cntTh     =     u32_RB_val           & 0xFF;
        pParam->_param_filmDetectctrl.u8_mixMode_cnt_max         =    (u32_RB_val  >>  8)   & 0xFF;
        pParam->_param_filmDetectctrl.u8_quit_cnt_th             =    (u32_RB_val  >> 16)   & 0xFF;
        pParam->_param_filmDetectctrl.u8_quit_prd_th             =    (u32_RB_val  >> 24)   & 0xFF;

        ReadRegister(M8P_SOFTWARE_SOFTWARE_13_reg, 0, 31, &u32_RB_val);
        pParam->_param_filmDetectctrl.u27_mixMode_rgnMove_minMot     =     u32_RB_val          & 0xFFFFFFF;
        pParam->_param_filmDetectctrl.u4_mixMode_rgnMove_motWgt      =    (u32_RB_val >> 28)   & 0x0F;

        ReadRegister(M8P_SOFTWARE_SOFTWARE_14_reg, 0, 31, &u32_RB_val);
        pParam->_param_filmDetectctrl.u32_dbg_param2      =     u32_RB_val          & 0xFFFFFFFF;

        ReadRegister(M8P_SOFTWARE_SOFTWARE_15_reg, 0, 31, &u32_RB_val);
        pParam->_param_filmDetectctrl.u32_dbg_cnt         =     u32_RB_val          & 0xFFFFFFFF;
	}

#if PQL_DYNAMIC_ADJUST_FIX_ISSUE
    ReadRegister(M8P_HARDWARE_HARDWARE_57_reg, 0, 31, &u32_RB_val);
    pParam->_param_read_comreg.u1_low_delay_test01            = (u32_RB_val >> 2)  & 0x1;
#endif

	//basic info check
	{
		ReadRegister(M8P_SOFTWARE2_SOFTWARE2_05_reg, 0, 31, &u32_RB_val);
		pParam->_param_info_check.u1_Show_APL_Info = (u32_RB_val >> 0) & 0x1;
		pParam->_param_info_check.u1_Show_DTL_Info = (u32_RB_val >> 1) & 0x1;
		pParam->_param_info_check.u1_Show_SAD_Info = (u32_RB_val >> 2) & 0x1;
		
		ReadRegister(M8P_SOFTWARE2_SOFTWARE2_07_reg, 0, 31, &u32_RB_val);
		pParam->_param_info_check.u1_Check_PTG_CALIB_SINGLE_SAD_INDEX = (u32_RB_val >> 0) & 0xff;
		pParam->_param_info_check.u1_Check_PTG_CALIB_SINGLE_SAD_en = (u32_RB_val >> 8) & 0x1;
	
		ReadRegister(M8P_SOFTWARE2_SOFTWARE2_08_reg, 0, 31, &u32_RB_val);
		pParam->_param_info_check.u1_Check_Env_PTG_APL = (u32_RB_val >> 0) & 0x1;
		pParam->_param_info_check.u1_Check_Env_PTG_POS = (u32_RB_val >> 1) & 0x1;
		pParam->_param_info_check.u1_Check_Env_PTG_SQUARE = (u32_RB_val >> 2) & 0x1;
		pParam->_param_info_check.u1_Check_Env_PTG_MV_INFO = (u32_RB_val >> 3) & 0x1;
		pParam->_param_info_check.u1_Check_Env_DDR_BOUNDARY = (u32_RB_val >> 4) & 0x1;
		pParam->_param_info_check.u1_Check_Env_RIM_BOUNDARY = (u32_RB_val >> 5) & 0x1;
		pParam->_param_info_check.u1_Check_Env_LBMC = (u32_RB_val >> 6) & 0x1;
		pParam->_param_info_check.u1_Check_Env_REGION = (u32_RB_val >> 7) & 0x1;
		pParam->_param_info_check.u1_InfoCheck_en_db = (u32_RB_val >> 28) & 0x1;
		pParam->_param_info_check.u1_InfoCheck_log_en = (u32_RB_val >> 29) & 0x1;
		pParam->_param_info_check.u1_Check_Env_Shutdown = (u32_RB_val >> 30) & 0x1;
		pParam->_param_info_check.u1_Check_Env_all_en = (u32_RB_val >> 31) & 0x1;

		ReadRegister(M8P_SOFTWARE2_SOFTWARE2_09_reg, 0, 31, &u32_RB_val);
		pParam->_param_info_check.u1_Check_PTG_CALIB_APL = (u32_RB_val >> 0) & 0x1;
		pParam->_param_info_check.u1_Check_PTG_CALIB_DTL = (u32_RB_val >> 1) & 0x1;
		pParam->_param_info_check.u1_Check_PTG_CALIB_SAD = (u32_RB_val >> 2) & 0x1;
		pParam->_param_info_check.u1_Check_PTG_CALIB_SC = (u32_RB_val >> 3) & 0x1;
		pParam->_param_info_check.u1_Check_PTG_CALIB_LBMC = (u32_RB_val >> 4) & 0x1;
		pParam->_param_info_check.u1_Check_PTG_CALIB_YUV = (u32_RB_val >> 5) & 0x1;
		pParam->_param_info_check.u1_Check_PTG_CALIB_LOGO_DETECT = (u32_RB_val >> 6) & 0x1;
		pParam->_param_info_check.u1_Check_PTG_CALIB_MV_CONVERGENCY = (u32_RB_val >> 7) & 0x1;
		pParam->_param_info_check.u1_Check_PTG_CALIB_SAD_CONVERGENCY = (u32_RB_val >> 8) & 0x1;
		pParam->_param_info_check.u1_Check_PTG_CALIB_LOGO_CONVERGENCY = (u32_RB_val >> 9) & 0x1;
		pParam->_param_info_check.u1_Check_PTG_CALIB_Shutdown = (u32_RB_val >> 30) & 0x1;
		pParam->_param_info_check.u1_Check_PTG_CALIB_all_en = (u32_RB_val >> 31) & 0x1;
		
		ReadRegister(M8P_SOFTWARE2_SOFTWARE2_10_reg, 0, 31, &u32_RB_val);
		pParam->_param_info_check.u1_Check_REALSTREAM_CALIB_MV_INFO = (u32_RB_val >> 0) & 0x1;
		pParam->_param_info_check.u1_Check_REALSTREAM_CALIB_FB_BADREGION = (u32_RB_val >> 1) & 0x1;
		pParam->_param_info_check.u1_Check_REALSTREAM_CALIB_MV_INFO_Shutdown = (u32_RB_val >> 16) & 0x1;
		pParam->_param_info_check.u1_Check_REALSTREAM_CALIB_FB_BADREGION_Shutdown = (u32_RB_val >> 17) & 0x1;

		ReadRegister(M8P_SOFTWARE2_SOFTWARE2_11_reg, 31, 31, &u32_RB_val);
		pParam->_param_info_check.u1_AutoRegression_en_db = (u32_RB_val >> 31) & 0x1;
		pParam->_param_info_check.u1_AutoRegression_reset = (u32_RB_val >> 30) & 0x1;
		pParam->_param_info_check.u1_AutoRegression_log_en = (u32_RB_val >> 28) & 0x1;

		pParam->_param_info_check.u1_AutoRegression_Verify_all = (u32_RB_val >> 0) & 0x1;
		pParam->_param_info_check.u1_AutoRegression_Verify_single = (u32_RB_val >> 1) & 0x1;
		pParam->_param_info_check.u1_AutoRegression_GetData_all = (u32_RB_val >> 2) & 0x1;
		pParam->_param_info_check.u1_AutoRegression_GetData_single = (u32_RB_val >> 3) & 0x1;
			
		pParam->_param_info_check.u1_AutoRegression_log_SC = (u32_RB_val >> 8) & 0x1;
		pParam->_param_info_check.u1_AutoRegression_log_GFB = (u32_RB_val >> 9) & 0x1;
		pParam->_param_info_check.u1_AutoRegression_log_LFB = (u32_RB_val >> 10) & 0x1;
		pParam->_param_info_check.u1_AutoRegression_log_LOGO = (u32_RB_val >> 11) & 0x1;
		
		pParam->_param_info_check.u8_AutoRegression_stream_id = (u32_RB_val >> 16) & 0xff;
	}

}

