#ifndef __MEMC_REG_DEF_H__
#define __MEMC_REG_DEF_H__

//////////// Merlin8 ////////////
//page 0X
#include <rbus/kmc_top_reg.h>				//00
#include <rbus/ippre_reg.h>				//01
#include <rbus/pqc_pqdc_reg.h>				//02, 08
#include <rbus/decpr_ptg_reg.h>			//03
#include <rbus/mc_dma_reg.h>				//04, 1E
#include <rbus/ippre1_reg.h>				//05
#include <rbus/mc_reg.h>					//06
#include <rbus/mc2_reg.h>					//07
#include <rbus/lbmc_reg.h>					//09
#include <rbus/bbd_reg.h>					//0A
#include <rbus/kmc_meter_top_reg.h>		//0B
#include <rbus/mc3_reg.h>					//0C

//page 2X

#include <rbus/kme_top_reg.h>				// 20
#include <rbus/mvinfo_dma_reg.h>			// 21
#include <rbus/kme_vbuf_top_reg.h>			// 22
#include <rbus/kme_me1_top2_reg.h>		// 23
#include <rbus/kme_me1_top3_reg.h>		// 24
#include <rbus/kme_me1_top4_reg.h>		// 25
#include <rbus/kme_me1_top5_reg.h>		// 26
#include <rbus/me_share_dma_reg.h>		// 27, 53, 69
#include <rbus/kme_me1_top10_reg.h>		// 28
#include <rbus/kme_lbme_top_reg.h>		// 29
#include <rbus/kme_me2_calc1_reg.h>		// 2A
#include <rbus/kme_logo2_reg.h>			// 2B
#include <rbus/kme_ipme_reg.h>			// 2C
#include <rbus/kme_ipme1_reg.h>			// 2D
#include <rbus/kme_logo0_reg.h>			// 2E
#include <rbus/kme_logo1_reg.h>			// 2F

//page 3X
#include <rbus/kme_dm_top0_reg.h>			// 30
#include <rbus/kme_dm_top1_reg.h>			// 31
#include <rbus/kme_dm_top2_reg.h>			// 32
#include <rbus/kme_dehalo3_reg.h>			// 33
#include <rbus/kme_me1_top0_reg.h>		// 34
#include <rbus/kme_me1_top1_reg.h>		// 35
#include <rbus/kme_lbme2_top_reg.h>		// 36
#include <rbus/kme_me2_vbuf_top_reg.h>	// 37
#include <rbus/kme_me2_calc0_reg.h>		// 38
#include <rbus/kme_me1_top6_reg.h>		// 39
#include <rbus/kme_me1_top7_reg.h>		// 3A
#include <rbus/kme_me1_top8_reg.h>		// 3B
#include <rbus/kme_me1_top9_reg.h>		// 3C
#include <rbus/kme_dehalo_reg.h>			// 3D
#include <rbus/kme_dehalo2_reg.h>			// 3E

//page 4X
#include <rbus/kpost_top_reg.h>			// 40
#include <rbus/crtc1_reg.h>					// 41
#include <rbus/kme_logo3_reg.h>			// 42
#include <rbus/kphase2_reg.h>				// 43
#include <rbus/kphase_reg.h>				// 44
#include <rbus/hardware_reg.h>				// 45
#include <rbus/software_reg.h>				// 46
#include <rbus/software1_reg.h>			// 47
#include <rbus/software2_reg.h>			// 48
#include <rbus/software3_reg.h>			// 49

//page 5X
#include <rbus/dbus_wrapper_reg.h>			// 50, 51
//#include <rbus/me_share_dma_reg.h>		// 53
#include <rbus/kme_ipme2_reg.h>			// 54
#include <rbus/kme_me1_top11_reg.h>		// 56
#include <rbus/kme_dehalo6_reg.h>			// 57
#include <rbus/kme_pqc_pqdc_reg.h>		// 58, 59
#include <rbus/kme_dehalo4_reg.h>			// 5A
#include <rbus/kme_dehalo5_reg.h>			// 5B, 5C
#include <rbus/kme_algo_reg.h>				// 5D
#include <rbus/kme_me1_bg0_reg.h>			// 5E
#include <rbus/kme_me1_bg1_reg.h>			// 5F

//page 6X
#include <rbus/kme_bist_reg.h>				// 60
#include <rbus/kmc_bist_bisr_reg.h>			// 61
#include <rbus/kmc_bi_reg.h>				// 62, 63
#include <rbus/kme_algo2_reg.h>			// 64
#include <rbus/kme_me0_reg.h>				// 65
#include <rbus/kme_me1_bg2_reg.h>			// 66
#include <rbus/kme_dm_top3_reg.h>			// 67, 68
//#include <rbus/me_share_dma_reg.h>		// 69
#include <rbus/kme_algo3_reg.h>			// 6A

//others
#include <rbus/sys_reg_reg.h>			//for MEMC CLK control
#include <rbus/ppoverlay_reg.h>	
#include <rbus/timer_reg.h>	
#include <rbus/memc_mux_reg.h>
#include <rbus/kme_dehalo4_reg.h>

//////////// Merlin8P ////////////
#include "rbus/M8P_bbd_reg.h"                       // 00 (90)
#include "rbus/M8P_rthw_ippre_top_reg.h"            // 01 (91)
#include "rbus/M8P_rtmc_ippre_reg.h"                // 02 (92)
#include "rbus/M8P_rthw_mc_top_reg.h"               // 03 (93)
#include "rbus/M8P_rthw_ipmc_top_reg.h"             // 04 (94)
#include "rbus/M8P_rthw_post_top_reg.h"             // 05 (95)
#include "rbus/M8P_rthw_mc_dma_reg.h"               // 06~07 (96~97)
#include "rbus/M8P_rthw_mc_pqc_pqdc_reg.h"          // 08~09 (98~99)
#include "rbus/M8P_rthw_mc_meter_top_reg.h"         // 0A (9A)
#include "rbus/M8P_rthw_mc_dbus_wrapper_reg.h"      // 0B (9B)
#include "rbus/M8P_rthw_mc_bist_bisr_control_reg.h" // 0C (9C)
#include "rbus/M8P_rthw_lbmc_decpr_ptg_reg.h"       // 0D (9D)
#include "rbus/M8P_rthw_mc_ptg_reg.h"               // 0E (9E)
#include "rbus/M8P_rthw_lbmc_reg.h"                 // 0F (9F)
 
//page 1X
#include "rbus/M8P_rtmc_lbmc_reg.h"         // 16 (A6)
#include "rbus/M8P_rtmc_lbmc_table_reg.h"   // 17 (A7)
#include "rbus/M8P_rtmc_reg.h"              // 18 (A8)
#include "rbus/M8P_rtmc2_reg.h"             // 19 (A9)
#include "rbus/M8P_rthw_mc_pqc_pqdc_2p_reg.h"   // 1A~1C (AA~AC)
#include "rbus/M8P_rthw_post_top2_reg.h"    // 1D (AD)
//#include "rbus/M8P_rtmc_ippre_logo_reg.h"   // 1E (AE)
//#include "rbus/M8P_rtmc_ippre_logo2_reg.h"  // 1F (AF)
 
//page 2X
#include "rbus/M8P_rthw_me_vbuf_top_reg.h"  // 20 (B0)
#include "rbus/M8P_rtme_hme1_top0_reg.h"    // 21 (B1)
#include "rbus/M8P_rtme_hme1_top1_reg.h"    // 22 (B2)
#include "rbus/M8P_rtme_hme1_top2_reg.h"    // 23 (B3)
#include "rbus/M8P_rtme_hme1_top3_reg.h"    // 24 (B4)
#include "rbus/M8P_rtme_hme1_top4_reg.h"    // 25 (B5)
#include "rbus/M8P_rtme_hme1_top5_reg.h"    // 26 (B6)
#include "rbus/M8P_rtme_hme1_top6_reg.h"    // 27 (B7)
#include "rbus/M8P_rtme_hme1_top7_reg.h"    // 28 (B8)
#include "rbus/M8P_rtme_hme1_top8_reg.h"    // 29 (B9)
#include "rbus/M8P_rtme_hme1_top9_reg.h"    // 2A (BA)
#include "rbus/M8P_rtme_hme1_top10_reg.h"   // 2B (BB)
#include "rbus/M8P_rtme_hme1_top11_reg.h"   // 2C (BC)
#include "rbus/M8P_rtme_hme1_top12_reg.h"   // 2D (BD)
//#include "rbus/M8P_rthw_me_top2_reg.h"      // 2F (BF)
 
//page 3X
#include "rbus/M8P_rtme_iphme_top0_reg.h"   // 30 (C0)
#include "rbus/M8P_rtme_iphme_top1_reg.h"   // 31 (C1)
#include "rbus/M8P_rtme_iphme0_reg.h"       // 32 (C2)
#include "rbus/M8P_rtme_iphme1_reg.h"       // 33 (C3)
#include "rbus/M8P_rtme_iphme2_reg.h"       // 34 (C4)
#include "rbus/M8P_rtme_iphme3_reg.h"       // 35 (C5)
#include "rbus/M8P_rtme_iphme4_reg.h"       // 36 (C6)
#include "rbus/M8P_rtme_me15_0_reg.h"       // 37 (C7)
#include "rbus/M8P_rtme_me15_1_reg.h"       // 38 (C8)
#include "rbus/M8P_rtme_me2_calc0_reg.h"    // 39 (C9)
#include "rbus/M8P_rtme_dehalo0_reg.h"      // 3A (CA)
#include "rbus/M8P_rtme_dehalo1_reg.h"      // 3B (CB)
#include "rbus/M8P_rtme_dehalo_logo_reg.h"  // 3C (CC)
#include "rbus/M8P_rtme_logo0_reg.h"        // 3D (CD)
#include "rbus/M8P_rtme_logo1_reg.h"        // 3E (CE)
#include "rbus/M8P_rtme_logo2_reg.h"        // 3F (CF)
 
//page 4X
#include "rbus/M8P_rthw_lbme_top_reg.h"             // 40 (D0)
#include "rbus/M8P_rthw_crtc1_reg.h"                // 41 (D1)
#include "rbus/M8P_rthw_me_phase_control_reg.h"     // 42 (D2)
#include "rbus/M8P_rthw_phase2_reg.h"               // 43 (D3)
#include "rbus/M8P_rthw_phase_reg.h"                // 44 (D4)
#include <rbus/M8P_hardware_reg.h>                  // 45 (D5)
#include <rbus/M8P_software_reg.h>                  // 46 (D6)
#include <rbus/M8P_software1_reg.h>                 // 47 (D7)
#include <rbus/M8P_software2_reg.h>                 // 48 (D8)
#include <rbus/M8P_software3_reg.h>                 // 49 (D9)
#include "rbus/M8P_rthw_me_timing_control_reg.h"    // 4A (DA)
#include "rbus/M8P_rthw_me_dbgshow_reg.h"           // 4B (DB)
#include "rbus/M8P_rthw_me_dbus_wrapper_reg.h"      // 4C (DC)
#include "rbus/M8P_rthw_me_pqc_pqdc_reg.h"          // 4D (DD)
#include "rbus/M8P_rthw_me_bist_bisr_control_reg.h" // 4E (DE)
#include "rbus/M8P_rthw_phase_to_sram_reg.h"        // 4F (DF)
 
//page 5X
#include "rbus/M8P_rtme_logo3_reg.h"        // 51 (E1)
#include "rbus/M8P_rtme_logo4_reg.h"        // 52 (E2)
#include "rbus/M8P_rtme_me2_calc1_reg.h"    // 53 (E3)
#include "rbus/M8P_rtme_logo5_reg.h"        // 54 (E4)
#include "rbus/M8P_rtme_dehalo_logo2_reg.h" // 55 (E5)
 
//page 6X
#include "rbus/M8P_rthw_me_dma_reg.h"       // 60 (F0)
#endif

/*-----------------------------------------------------------------------------------------*/

