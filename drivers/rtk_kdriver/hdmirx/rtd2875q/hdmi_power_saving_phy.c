#include <mach/rtk_platform.h>
#include <mach/platform.h>
#include "hdmi_reg.h"
#include "hdmi_common.h"
#include "hdmi_reg_phy.h"   // for HDMI PHY
//------------------------------------------------------------------------------
// Power Saving Control
//------------------------------------------------------------------------------

void lib_hdmi_phy_reset_power_saving_state(unsigned char nport)
{
    PWR_FLOW_INFO("HDMI[p%d] phy reset power saveing state\n", nport);

    if (hdmi_bypass_phy_ps_mode)
    {
        PWR_FLOW_INFO("hdmi_bypass_phy_ps_mode is enabled, do nothing...\n");
        return ;
    }
#ifdef CONFIG_RTK_KDRV_HDMI_ENABLE_PHY
        switch(nport)
        {
        case 0:

        // reset APHY Copy from Merlin7
        hdmi_mask(HDMIRX_2P0_PHY_P0_R4_reg, ~0x1, 1); //fast switch circuit enable
        hdmi_mask(HDMIRX_2P0_PHY_P0_G4_reg, ~0x1, 1); //fast switch circuit enable
        hdmi_mask(HDMIRX_2P0_PHY_P0_B4_reg, ~0x1, 1); //fast switch circuit enable
        hdmi_mask(HDMIRX_2P0_PHY_P0_CK4_reg, ~(0x3<<2), (0x3<<2)); //fast switch circuit enable

        // DPHY
        // reset HD20 DPHY
        hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);    //reg_p0_hd14_pow_save_bypass bypass power saving (HDMI-1.4)
        hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), 0x0);        //reg_p0_hd14_pow_on_manual_en power on cntrol by MAC  (HDMI-1.4)
        hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask);    //reg_p0_hd14_pow_on_manual manual power setting = On (HDMI-1.4)

        // release reset
        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask);  //P0_R_DIG_RST_N fast switch reset p0 r
        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask);  //P0_G_DIG_RST_N fast switch reset p0 g
        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask);  //P0_B_DIG_RST_N fast switch reset p0 b

        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask);  //P0_R_CDR_RST_N  fast switch reset p0 r        
        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask);  //P0_G_CDR_RST_N fast switch reset p0 g
        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask);  //P0_B_CDR_RST_N fast switch reset p0 b
        break;

        case 1:

        // reset APHY Copy from Merlin7
        hdmi_mask(HDMIRX_2P0_PHY_P1_R4_reg, ~0x1, 1); //fast switch circuit enable
        hdmi_mask(HDMIRX_2P0_PHY_P1_G4_reg, ~0x1, 1); //fast switch circuit enable
        hdmi_mask(HDMIRX_2P0_PHY_P1_B4_reg, ~0x1, 1); //fast switch circuit enable
        hdmi_mask(HDMIRX_2P0_PHY_P1_CK4_reg, ~(0x3<<2), (0x3<<2)); //fast switch circuit enable

        // DPHY
        // reset HD20 DPHY
        hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);    //reg_p0_hd14_pow_save_bypass bypass power saving (HDMI-1.4)
        hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), 0x0);        //reg_p0_hd14_pow_on_manual_en power on cntrol by MAC  (HDMI-1.4)
        hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask);    //reg_p0_hd14_pow_on_manual manual power setting = On (HDMI-1.4)

        // release reset
        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask);  //P0_R_DIG_RST_N fast switch reset p0 r
        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask);  //P0_G_DIG_RST_N fast switch reset p0 g
        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask);  //P0_B_DIG_RST_N fast switch reset p0 b

        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask);  //P0_R_CDR_RST_N  fast switch reset p0 r        
        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask);  //P0_G_CDR_RST_N fast switch reset p0 g
        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask);  //P0_B_CDR_RST_N fast switch reset p0 b
        break;

        case 2:

        // reset APHY Copy from Merlin7
        hdmi_mask(HDMIRX_2P0_PHY_P2_R4_reg, ~0x1, 1); //fast switch circuit enable
        hdmi_mask(HDMIRX_2P0_PHY_P2_G4_reg, ~0x1, 1); //fast switch circuit enable
        hdmi_mask(HDMIRX_2P0_PHY_P2_B4_reg, ~0x1, 1); //fast switch circuit enable
        hdmi_mask(HDMIRX_2P0_PHY_P2_CK4_reg, ~(0x3<<2), (0x3<<2)); //fast switch circuit enable

        // DPHY
        // reset HD20 DPHY
        hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);    //reg_p0_hd14_pow_save_bypass bypass power saving (HDMI-1.4)
        hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), 0x0);        //reg_p0_hd14_pow_on_manual_en power on cntrol by MAC  (HDMI-1.4)
        hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask);    //reg_p0_hd14_pow_on_manual manual power setting = On (HDMI-1.4)

        // release reset
        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask);  //P0_R_DIG_RST_N fast switch reset p0 r
        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask);  //P0_G_DIG_RST_N fast switch reset p0 g
        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask);  //P0_B_DIG_RST_N fast switch reset p0 b

        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask);  //P0_R_CDR_RST_N  fast switch reset p0 r        
        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask);  //P0_G_CDR_RST_N fast switch reset p0 g
        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask);  //P0_B_CDR_RST_N fast switch reset p0 b
        break;

    }
	
#endif
}

void lib_hdmi_phy_enter_power_saving_mode(unsigned char nport, HDMI_MODE mode, unsigned char br_swap)
{
#ifdef CONFIG_RTK_KDRV_HDMI_ENABLE_PHY

    PWR_EMG("HDMI[p%d] phy enter power saveing mode (mode=%d, br_swap=%d) \n", nport, mode, br_swap);

    if (hdmi_bypass_phy_ps_mode)
    {
        PWR_EMG("hdmi_bypass_phy_ps_mode is enabled, do nothing...\n");
        return ;
    }

        switch(nport)
        {
        case 0: //Port0
	   switch(mode)
	    {

	    case HDMI_20:
	        //APHY Setting
	            //0x1803CA50[31:24] REG_P0_CK_4
	            hdmi_mask(HDMIRX_2P0_PHY_P0_CK1_reg, ~(0x3<<24), 0x1<<24);  // power saving signal delay select. Fxtal/8 /16 /1024 /2048
	            hdmi_mask(HDMIRX_2P0_PHY_P0_CK1_reg, ~(0x1<<27), 0x1<<27);  // fast switch circuit enable
	            hdmi_mask(HDMIRX_2P0_PHY_P0_CK1_reg, ~(0x1<<26), 0x1<<26);  // power saving signal delay enable
	            
	            if (br_swap)
	            {
	                // Analog
	                hdmi_mask(HDMIRX_2P0_PHY_P0_R4_reg, ~0x1, 1); //fast switch circuit enable  (B Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_G4_reg, ~0x1, 1); //fast switch circuit enable  (G Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_B4_reg, ~0x1, 0); //fast switch circuit disable (R Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_CK4_reg, ~0x1, 0); //fast switch circuit enable (CK Lane)

	                // Digital
		        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask), 0);  //P0_R_DIG_RST_N fast switch reset p0 r
		        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), 0);  //P0_G_DIG_RST_N fast switch reset p0 g
		        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask), 0);  //P0_R_CDR_RST_N  fast switch reset p0 r        
		        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),0);  //P0_G_CDR_RST_N fast switch reset p0 g
	            }
	            else
	            {
	                // Analog
	                hdmi_mask(HDMIRX_2P0_PHY_P0_R4_reg, ~0x1, 0); //fast switch circuit disable (R Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_G4_reg, ~0x1, 1); //fast switch circuit enable  (G Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_B4_reg, ~0x1, 1); //fast switch circuit enable  (B Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_CK4_reg, ~0x1, 0); //fast switch circuit enable (CK Lane)

	                // Digital
		        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), 0);  //P0_G_DIG_RST_N fast switch reset p0 g
		        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask), 0);  //P0_B_DIG_RST_N fast switch reset p0 b
		        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),0);  //P0_G_CDR_RST_N fast switch reset p0 g
		        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask), 0);  //P0_B_CDR_RST_N fast switch reset p0 b
					
	            }
	            
	            // POWR OFF DPHY
			hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);    //reg_p0_hd14_pow_save_bypass bypass power saving (HDMI-1.4)
			hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_r_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_r_only_mask);    //Power on reg_p0_hd14_pow_save_bypass_r_only by FW
			hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_g_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_g_only_mask);    //Power on reg_p0_hd14_pow_save_bypass_g_only by FW
			hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_b_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_b_only_mask);    //Power onreg_p0_hd14_pow_save_bypass_b_only by FW
			hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask);        //Power on by FW
			hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), 0);          //reg_p0_hd14_pow_on_manual manual power=OFF

	        break;

	    //////////////////////////////////////////////////////////////////////////////////////////
	    //Logic b lane on/off, g lane always off, r/ck lane always on		
	    case HDMI_20_VS_WIN_OPP:
	            //0x1803CA50[31:24] REG_P0_CK_4 Copy from Merlin7
	            hdmi_mask(HDMIRX_2P0_PHY_P0_CK1_reg, ~(0x3<<24), 0x1<<24);  // power saving signal delay select. Fxtal/8 /16 /1024 /2048
	            hdmi_mask(HDMIRX_2P0_PHY_P0_CK1_reg, ~(0x1<<27), 0x1<<27);  // fast switch circuit enable
	            hdmi_mask(HDMIRX_2P0_PHY_P0_CK1_reg, ~(0x1<<26), 0x1<<26);  // power saving signal delay enable

	            if (br_swap)
	            {
	                // Analog
	                hdmi_mask(HDMIRX_2P0_PHY_P0_R4_reg, ~0x1, 1); //fast switch circuit enable  (B Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_G4_reg, ~0x1, 0); //fast switch circuit disable  (G Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_B4_reg, ~0x1, 0); //fast switch circuit disable (R Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_CK4_reg, ~0x1, 0); //fast switch circuit disable (CK Lane)

	                // Digital
	                hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<30), 0);  // fast switch reset p2 g OFF
	                //hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<31), 0);  // fast switch reset p2 r
	                hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<27), 0);  // fast switch reset p2 g OFF
	                //hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<28), 0);  // fast switch reset p2 r

	                // POWR OFF DPHY
	                hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0, ~(0x1<<15), 0<<15);    //  Disable power saving byass (Control by Mac)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0, ~(0x1<<10), 0<<10);    //reg_p0_hd14_pow_save_bypass_r_only Power on by Mac
	                hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0, ~(0x1<<9), 1<<9);    // reg_p0_hd14_pow_save_bypass_g_onlyPower on by FW
	                hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0, ~(0x1<<8), 1<<8);    //reg_p0_hd14_pow_save_bypass_b_only Power on by FW
	            }
	            else
	            {
	                // Analog
	                hdmi_mask(HDMIRX_2P0_PHY_P0_R4_reg, ~0x1, 0); //fast switch circuit disable (R Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_G4_reg, ~0x1, 0); //fast switch circuit disable  (G Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_B4_reg, ~0x1, 1); //fast switch circuit enable  (B Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_CK4_reg, ~0x1, 0); //fast switch circuit disable (CK Lane)

	                // Digital
	                hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<30), 0);  // fast switch reset p2 g OFF
	                //hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<29), 0);  // fast switch reset p2 b
	                hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<27), 0);  // fast switch reset p2 g OFF
	                //hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<26), 0);  // fast switch reset p2 b

	                // POWR OFF DPHY
	                hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0, ~(0x1<<15), 0<<15);    //  Disable power saving byass (Control by Mac)
	                hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0, ~(0x1<<10), 1<<10);    //reg_p0_hd14_pow_save_bypass_r_only Power on by FW
	                hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0, ~(0x1<<9), 1<<9);    // reg_p0_hd14_pow_save_bypass_g_onlyPower on by FW
	                hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0, ~(0x1<<8), 0<<8);    //reg_p0_hd14_pow_save_bypass_b_only Power on by Mac
	            }

	            hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0, ~(0x1<<14), 0<<14);    // Power on by Mac
	            hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0, ~(0x1<<13), 0<<13);    // manul_power_on=OFF (should be default value)
	        break;
	    ///////////////////////////////////////////////////////////////////////////////////////////
	    case HDMI_14:
	    case HDMI_21:

	            hdmi_mask(HDMIRX_2P0_PHY_P0_R4_reg, ~0x1, 1); //fast switch circuit enable
	            hdmi_mask(HDMIRX_2P0_PHY_P0_G4_reg, ~0x1, 1); //fast switch circuit enable
	            hdmi_mask(HDMIRX_2P0_PHY_P0_B4_reg, ~0x1, 1); //fast switch circuit enable

	            if (mode==HDMI_14) 
	            {
		          // POWR OFF DPHY
			   hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);    //reg_p0_hd14_pow_save_bypass bypass power saving (HDMI-1.4)
			   hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), 0);        //reg_p0_hd14_pow_on_manual_en power on cntrol by MAC  (HDMI-1.4)
			   hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), 0);    //reg_p0_hd14_pow_on_manual manual power setting = On (HDMI-1.4)
	            }
	        break;

		}
	break;

        case 1: //Port1
	   switch(mode)
	    {

	    case HDMI_20:
	        //APHY Setting
	            //0x1803CA50[31:24] REG_P0_CK_4
	            hdmi_mask(HDMIRX_2P0_PHY_P1_CK1_reg, ~(0x3<<24), 0x1<<24);  // power saving signal delay select. Fxtal/8 /16 /1024 /2048
	            hdmi_mask(HDMIRX_2P0_PHY_P1_CK1_reg, ~(0x1<<27), 0x1<<27);  // fast switch circuit enable
	            hdmi_mask(HDMIRX_2P0_PHY_P1_CK1_reg, ~(0x1<<26), 0x1<<26);  // power saving signal delay enable
	            
	            if (br_swap)
	            {
	                // Analog
	                hdmi_mask(HDMIRX_2P0_PHY_P1_R4_reg, ~0x1, 1); //fast switch circuit enable  (B Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_G4_reg, ~0x1, 1); //fast switch circuit enable  (G Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_B4_reg, ~0x1, 0); //fast switch circuit disable (R Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_CK4_reg, ~0x1, 0); //fast switch circuit enable (CK Lane)

	                // Digital
		        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask), 0);  //P0_R_DIG_RST_N fast switch reset p0 r
		        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), 0);  //P0_G_DIG_RST_N fast switch reset p0 g
		        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask), 0);  //P0_R_CDR_RST_N  fast switch reset p0 r        
		        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),0);  //P0_G_CDR_RST_N fast switch reset p0 g
	            }
	            else
	            {
	                // Analog
	                hdmi_mask(HDMIRX_2P0_PHY_P1_R4_reg, ~0x1, 0); //fast switch circuit disable (R Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_G4_reg, ~0x1, 1); //fast switch circuit enable  (G Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_B4_reg, ~0x1, 1); //fast switch circuit enable  (B Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_CK4_reg, ~0x1, 0); //fast switch circuit enable (CK Lane)

	                // Digital
		        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), 0);  //P0_G_DIG_RST_N fast switch reset p0 g
		        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask), 0);  //P0_B_DIG_RST_N fast switch reset p0 b
		        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),0);  //P0_G_CDR_RST_N fast switch reset p0 g
		        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask), 0);  //P0_B_CDR_RST_N fast switch reset p0 b
					
	            }
	            
	            // POWR OFF DPHY
			hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);    //reg_p0_hd14_pow_save_bypass bypass power saving (HDMI-1.4)
			hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_r_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_r_only_mask);    //Power on reg_p0_hd14_pow_save_bypass_r_only by FW
			hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_g_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_g_only_mask);    //Power on reg_p0_hd14_pow_save_bypass_g_only by FW
			hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_b_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_b_only_mask);    //Power onreg_p0_hd14_pow_save_bypass_b_only by FW
			hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask);        //Power on by FW
			hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), 0);          //reg_p0_hd14_pow_on_manual manual power=OFF

	        break;

	    //////////////////////////////////////////////////////////////////////////////////////////
	    //Logic b lane on/off, g lane always off, r/ck lane always on		
	    case HDMI_20_VS_WIN_OPP:
	            //0x1803CA50[31:24] REG_P0_CK_4 Copy from Merlin7
	            hdmi_mask(HDMIRX_2P0_PHY_P1_CK1_reg, ~(0x3<<24), 0x1<<24);  // power saving signal delay select. Fxtal/8 /16 /1024 /2048
	            hdmi_mask(HDMIRX_2P0_PHY_P1_CK1_reg, ~(0x1<<27), 0x1<<27);  // fast switch circuit enable
	            hdmi_mask(HDMIRX_2P0_PHY_P1_CK1_reg, ~(0x1<<26), 0x1<<26);  // power saving signal delay enable

	            if (br_swap)
	            {
	                // Analog
	                hdmi_mask(HDMIRX_2P0_PHY_P1_R4_reg, ~0x1, 1); //fast switch circuit enable  (B Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_G4_reg, ~0x1, 0); //fast switch circuit disable  (G Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_B4_reg, ~0x1, 0); //fast switch circuit disable (R Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_CK4_reg, ~0x1, 0); //fast switch circuit disable (CK Lane)

	                // Digital
	                hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<30), 0);  // fast switch reset p2 g OFF
	                //hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<31), 0);  // fast switch reset p2 r
	                hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<27), 0);  // fast switch reset p2 g OFF
	                //hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<28), 0);  // fast switch reset p2 r

	                // POWR OFF DPHY
	                hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0, ~(0x1<<15), 0<<15);    //  Disable power saving byass (Control by Mac)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0, ~(0x1<<10), 0<<10);    //reg_p0_hd14_pow_save_bypass_r_only Power on by Mac
	                hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0, ~(0x1<<9), 1<<9);    // reg_p0_hd14_pow_save_bypass_g_onlyPower on by FW
	                hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0, ~(0x1<<8), 1<<8);    //reg_p0_hd14_pow_save_bypass_b_only Power on by FW
	            }
	            else
	            {
	                // Analog
	                hdmi_mask(HDMIRX_2P0_PHY_P1_R4_reg, ~0x1, 0); //fast switch circuit disable (R Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_G4_reg, ~0x1, 0); //fast switch circuit disable  (G Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_B4_reg, ~0x1, 1); //fast switch circuit enable  (B Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_CK4_reg, ~0x1, 0); //fast switch circuit disable (CK Lane)

	                // Digital
	                hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<30), 0);  // fast switch reset p2 g OFF
	                //hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<29), 0);  // fast switch reset p2 b
	                hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<27), 0);  // fast switch reset p2 g OFF
	                //hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<26), 0);  // fast switch reset p2 b

	                // POWR OFF DPHY
	                hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0, ~(0x1<<15), 0<<15);    //  Disable power saving byass (Control by Mac)
	                hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0, ~(0x1<<10), 1<<10);    //reg_p0_hd14_pow_save_bypass_r_only Power on by FW
	                hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0, ~(0x1<<9), 1<<9);    // reg_p0_hd14_pow_save_bypass_g_onlyPower on by FW
	                hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0, ~(0x1<<8), 0<<8);    //reg_p0_hd14_pow_save_bypass_b_only Power on by Mac
	            }

	            hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0, ~(0x1<<14), 0<<14);    // Power on by Mac
	            hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0, ~(0x1<<13), 0<<13);    // manul_power_on=OFF (should be default value)
	        break;
	    ///////////////////////////////////////////////////////////////////////////////////////////
	    case HDMI_14:
	    case HDMI_21:

	            hdmi_mask(HDMIRX_2P0_PHY_P1_R4_reg, ~0x1, 1); //fast switch circuit enable
	            hdmi_mask(HDMIRX_2P0_PHY_P1_G4_reg, ~0x1, 1); //fast switch circuit enable
	            hdmi_mask(HDMIRX_2P0_PHY_P1_B4_reg, ~0x1, 1); //fast switch circuit enable

	            if (mode==HDMI_14) 
	            {
		          // POWR OFF DPHY
			   hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);    //reg_p0_hd14_pow_save_bypass bypass power saving (HDMI-1.4)
			   hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), 0);        //reg_p0_hd14_pow_on_manual_en power on cntrol by MAC  (HDMI-1.4)
			   hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), 0);    //reg_p0_hd14_pow_on_manual manual power setting = On (HDMI-1.4)
	            }
	        break;

		}
	break;


        case 2: //Port2
	   switch(mode)
	    {

	    case HDMI_20:
	        //APHY Setting
	            //0x1803CA50[31:24] REG_P0_CK_4
	            hdmi_mask(HDMIRX_2P0_PHY_P2_CK1_reg, ~(0x3<<24), 0x1<<24);  // power saving signal delay select. Fxtal/8 /16 /1024 /2048
	            hdmi_mask(HDMIRX_2P0_PHY_P2_CK1_reg, ~(0x1<<27), 0x1<<27);  // fast switch circuit enable
	            hdmi_mask(HDMIRX_2P0_PHY_P2_CK1_reg, ~(0x1<<26), 0x1<<26);  // power saving signal delay enable
	            
	            if (br_swap)
	            {
	                // Analog
	                hdmi_mask(HDMIRX_2P0_PHY_P2_R4_reg, ~0x1, 1); //fast switch circuit enable  (B Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_G4_reg, ~0x1, 1); //fast switch circuit enable  (G Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_B4_reg, ~0x1, 0); //fast switch circuit disable (R Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_CK4_reg, ~0x1, 0); //fast switch circuit enable (CK Lane)

	                // Digital
		        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask), 0);  //P0_R_DIG_RST_N fast switch reset p0 r
		        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), 0);  //P0_G_DIG_RST_N fast switch reset p0 g
		        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask), 0);  //P0_R_CDR_RST_N  fast switch reset p0 r        
		        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),0);  //P0_G_CDR_RST_N fast switch reset p0 g
	            }
	            else
	            {
	                // Analog
	                hdmi_mask(HDMIRX_2P0_PHY_P2_R4_reg, ~0x1, 0); //fast switch circuit disable (R Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_G4_reg, ~0x1, 1); //fast switch circuit enable  (G Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_B4_reg, ~0x1, 1); //fast switch circuit enable  (B Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_CK4_reg, ~0x1, 0); //fast switch circuit enable (CK Lane)

	                // Digital
		        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), 0);  //P0_G_DIG_RST_N fast switch reset p0 g
		        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask), 0);  //P0_B_DIG_RST_N fast switch reset p0 b
		        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),0);  //P0_G_CDR_RST_N fast switch reset p0 g
		        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask), 0);  //P0_B_CDR_RST_N fast switch reset p0 b
					
	            }
	            
	            // POWR OFF DPHY
			hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);    //reg_p0_hd14_pow_save_bypass bypass power saving (HDMI-1.4)
			hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_r_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_r_only_mask);    //Power on reg_p0_hd14_pow_save_bypass_r_only by FW
			hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_g_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_g_only_mask);    //Power on reg_p0_hd14_pow_save_bypass_g_only by FW
			hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_b_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_b_only_mask);    //Power onreg_p0_hd14_pow_save_bypass_b_only by FW
			hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask);        //Power on by FW
			hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), 0);          //reg_p0_hd14_pow_on_manual manual power=OFF

	        break;

	    //////////////////////////////////////////////////////////////////////////////////////////
	    //Logic b lane on/off, g lane always off, r/ck lane always on		
	    case HDMI_20_VS_WIN_OPP:
	            //0x1803CA50[31:24] REG_P0_CK_4 Copy from Merlin7
	            hdmi_mask(HDMIRX_2P0_PHY_P2_CK1_reg, ~(0x3<<24), 0x1<<24);  // power saving signal delay select. Fxtal/8 /16 /1024 /2048
	            hdmi_mask(HDMIRX_2P0_PHY_P2_CK1_reg, ~(0x1<<27), 0x1<<27);  // fast switch circuit enable
	            hdmi_mask(HDMIRX_2P0_PHY_P2_CK1_reg, ~(0x1<<26), 0x1<<26);  // power saving signal delay enable

	            if (br_swap)
	            {
	                // Analog
	                hdmi_mask(HDMIRX_2P0_PHY_P2_R4_reg, ~0x1, 1); //fast switch circuit enable  (B Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_G4_reg, ~0x1, 0); //fast switch circuit disable  (G Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_B4_reg, ~0x1, 0); //fast switch circuit disable (R Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_CK4_reg, ~0x1, 0); //fast switch circuit disable (CK Lane)

	                // Digital
	                hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<30), 0);  // fast switch reset p2 g OFF
	                //hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<31), 0);  // fast switch reset p2 r
	                hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<27), 0);  // fast switch reset p2 g OFF
	                //hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<28), 0);  // fast switch reset p2 r

	                // POWR OFF DPHY
	                hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0, ~(0x1<<15), 0<<15);    //  Disable power saving byass (Control by Mac)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0, ~(0x1<<10), 0<<10);    //reg_p0_hd14_pow_save_bypass_r_only Power on by Mac
	                hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0, ~(0x1<<9), 1<<9);    // reg_p0_hd14_pow_save_bypass_g_onlyPower on by FW
	                hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0, ~(0x1<<8), 1<<8);    //reg_p0_hd14_pow_save_bypass_b_only Power on by FW
	            }
	            else
	            {
	                // Analog
	                hdmi_mask(HDMIRX_2P0_PHY_P2_R4_reg, ~0x1, 0); //fast switch circuit disable (R Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_G4_reg, ~0x1, 0); //fast switch circuit disable  (G Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_B4_reg, ~0x1, 1); //fast switch circuit enable  (B Lane)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_CK4_reg, ~0x1, 0); //fast switch circuit disable (CK Lane)

	                // Digital
	                hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<30), 0);  // fast switch reset p2 g OFF
	                //hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<29), 0);  // fast switch reset p2 b
	                hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<27), 0);  // fast switch reset p2 g OFF
	                //hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<26), 0);  // fast switch reset p2 b

	                // POWR OFF DPHY
	                hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0, ~(0x1<<15), 0<<15);    //  Disable power saving byass (Control by Mac)
	                hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0, ~(0x1<<10), 1<<10);    //reg_p0_hd14_pow_save_bypass_r_only Power on by FW
	                hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0, ~(0x1<<9), 1<<9);    // reg_p0_hd14_pow_save_bypass_g_onlyPower on by FW
	                hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0, ~(0x1<<8), 0<<8);    //reg_p0_hd14_pow_save_bypass_b_only Power on by Mac
	            }

	            hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0, ~(0x1<<14), 0<<14);    // Power on by Mac
	            hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0, ~(0x1<<13), 0<<13);    // manul_power_on=OFF (should be default value)
	        break;
	    ///////////////////////////////////////////////////////////////////////////////////////////
	    case HDMI_14:
	    case HDMI_21:

	            hdmi_mask(HDMIRX_2P0_PHY_P2_R4_reg, ~0x1, 1); //fast switch circuit enable
	            hdmi_mask(HDMIRX_2P0_PHY_P2_G4_reg, ~0x1, 1); //fast switch circuit enable
	            hdmi_mask(HDMIRX_2P0_PHY_P2_B4_reg, ~0x1, 1); //fast switch circuit enable

	            if (mode==HDMI_14) 
	            {
		          // POWR OFF DPHY
			   hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);    //reg_p0_hd14_pow_save_bypass bypass power saving (HDMI-1.4)
			   hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), 0);        //reg_p0_hd14_pow_on_manual_en power on cntrol by MAC  (HDMI-1.4)
			   hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), 0);    //reg_p0_hd14_pow_on_manual manual power setting = On (HDMI-1.4)
	            }
	        break;

		}
	break;





	}
	   
#endif
}

void lib_hdmi_phy_exit_power_saving_mode(unsigned char nport, HDMI_MODE mode, unsigned char br_swap)
{
    PWR_EMG("HDMI[p%d] phy exit power saveing mode (mode=%d) \n", nport, mode);

    if (hdmi_bypass_phy_ps_mode)
    {
        PWR_EMG("hdmi_bypass_phy_ps_mode is enabled, do nothing...\n");
        return ;
    }
#ifdef CONFIG_RTK_KDRV_HDMI_ENABLE_PHY
        switch(nport)
        {
		case 0: //Port0
		     switch(mode)
		    {
		    case HDMI_20:

				// DPHY ON
				hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);    //reg_p0_hd14_pow_save_bypass bypass power saving (HDMI-1.4)
				hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_r_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_r_only_mask);    //Power on reg_p0_hd14_pow_save_bypass_r_only by FW
				hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_g_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_g_only_mask);    //Power on reg_p0_hd14_pow_save_bypass_g_only by FW
				hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_b_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_b_only_mask);    //Power onreg_p0_hd14_pow_save_bypass_b_only by FW

				hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask);        //Power on by FW
				hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask);    //reg_p0_hd14_pow_on_manual manual power setting = On (HDMI-1.4)
				udelay(10);
				hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), 0);          //reg_p0_hd14_pow_on_manual_en Power on by MAC
				hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), 0);          //reg_p0_hd14_pow_on_manual manual power=OFF

		            // DPHY RESET
		            if (br_swap)
		            {
			        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask);  //P0_R_DIG_RST_N fast switch reset p0 r
			        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask);  //P0_G_DIG_RST_N fast switch reset p0 g
			        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask);  //P0_R_CDR_RST_N  fast switch reset p0 r        
			        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask);  //P0_G_CDR_RST_N fast switch reset p0 g

		            }
		            else
		            {
			        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask);  //P0_G_DIG_RST_N fast switch reset p0 g
			        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask);  //P0_B_DIG_RST_N fast switch reset p0 b
			        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask);  //P0_G_CDR_RST_N fast switch reset p0 g
			        hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask);  //P0_B_CDR_RST_N fast switch reset p0 b

		            }
		        break;

		    ////////////////////////////////////////////////////////////////////////////////////////
		    case HDMI_20_VS_WIN_OPP:
			 //Copy from Merlin7	
		            // DPHY ON
		            hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(0x1<<15), 1<<15);    // enable power saving byass (should be default value)
		            hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(0x1<<10), 1<<10);    //reg_p0_hd14_pow_save_bypass_r_only Power on by FW
		            hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(0x1<<9), 1<<9);    // reg_p0_hd14_pow_save_bypass_g_onlyPower on by FW
		            hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(0x1<<8), 1<<8);    //reg_p0_hd14_pow_save_bypass_b_only Power on by FW
		            hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(0x1<<14), 0);        // Power on by MAC
		            hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(0x1<<13), 1<<13);    // manual power=On

		            // DPHY RESET
		            if (br_swap)
		            {
		                hdmi_mask(HDMIRX_2P0_PHY_P0_R4_reg, ~0x1, 0); //fast switch circuit disable  (B Lane)
		                hdmi_mask(HDMIRX_2P0_PHY_P0_R4_reg, ~(0x1<<3), (1<<3));  //
		                hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<30), (1<<30));  // fast switch release reset p2 g
		                //hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<31), (1<<31));  // fast switch release reset p2 r
		                hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<27), (1<<27));  // fast switch release reset p2 g
		                //hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<28), (1<<28));  // fast switch release reset p2 r
		            }
		            else
		            {
		                hdmi_mask(HDMIRX_2P0_PHY_P0_B4_reg, ~0x1, 0); //fast switch circuit disable  (B Lane)
		                hdmi_mask(HDMIRX_2P0_PHY_P0_B4_reg, ~(0x1<<3), (1<<3));  //
		                hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<30), (1<<30));  // fast switch release reset p2 g
		                //hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<29), (1<<29));  // fast switch release reset p2 b
		                hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<27), (1<<27));  // fast switch release reset p2 g
		                //hdmi_mask(HDMIRX_2P0_PHY_P0_DCDR_REG0, ~(0x1<<26), (1<<26));  // fast switch release reset p2 b
		            }
		        break;	
		    ///////////////////////////////////////////////////////////////////////////////////////////
		    case HDMI_14:
		    case HDMI_21:
		            // DPHY ON
		            hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);      //reg_p0_hd14_pow_save_bypass enable power saving byass (should be default value)
		            hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask),0);      //reg_p0_hd14_pow_on_manual_en Power on by FW
		            hdmi_mask(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask);    //reg_p0_hd14_pow_on_manual manual power=ON
		        break;	
		    }
	break;	


		case 1: //Port1
		     switch(mode)
		    {
		    case HDMI_20:

				// DPHY ON
				hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);    //reg_p0_hd14_pow_save_bypass bypass power saving (HDMI-1.4)
				hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_r_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_r_only_mask);    //Power on reg_p0_hd14_pow_save_bypass_r_only by FW
				hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_g_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_g_only_mask);    //Power on reg_p0_hd14_pow_save_bypass_g_only by FW
				hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_b_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_b_only_mask);    //Power onreg_p0_hd14_pow_save_bypass_b_only by FW

				hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask);        //Power on by FW
				hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask);    //reg_p0_hd14_pow_on_manual manual power setting = On (HDMI-1.4)
				udelay(10);
				hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), 0);          //reg_p0_hd14_pow_on_manual_en Power on by MAC
				hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), 0);          //reg_p0_hd14_pow_on_manual manual power=OFF

		            // DPHY RESET
		            if (br_swap)
		            {
			        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask);  //P0_R_DIG_RST_N fast switch reset p0 r
			        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask);  //P0_G_DIG_RST_N fast switch reset p0 g
			        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask);  //P0_R_CDR_RST_N  fast switch reset p0 r        
			        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask);  //P0_G_CDR_RST_N fast switch reset p0 g

		            }
		            else
		            {
			        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask);  //P0_G_DIG_RST_N fast switch reset p0 g
			        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask);  //P0_B_DIG_RST_N fast switch reset p0 b
			        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask);  //P0_G_CDR_RST_N fast switch reset p0 g
			        hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask);  //P0_B_CDR_RST_N fast switch reset p0 b

		            }
		        break;

		    ////////////////////////////////////////////////////////////////////////////////////////
		    case HDMI_20_VS_WIN_OPP:
			 //Copy from Merlin7	
		            // DPHY ON
		            hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(0x1<<15), 1<<15);    // enable power saving byass (should be default value)
		            hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(0x1<<10), 1<<10);    //reg_p0_hd14_pow_save_bypass_r_only Power on by FW
		            hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(0x1<<9), 1<<9);    // reg_p0_hd14_pow_save_bypass_g_onlyPower on by FW
		            hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(0x1<<8), 1<<8);    //reg_p0_hd14_pow_save_bypass_b_only Power on by FW
		            hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(0x1<<14), 0);        // Power on by MAC
		            hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(0x1<<13), 1<<13);    // manual power=On

		            // DPHY RESET
		            if (br_swap)
		            {
		                hdmi_mask(HDMIRX_2P0_PHY_P1_R4_reg, ~0x1, 0); //fast switch circuit disable  (B Lane)
		                hdmi_mask(HDMIRX_2P0_PHY_P1_R4_reg, ~(0x1<<3), (1<<3));  //
		                hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<30), (1<<30));  // fast switch release reset p2 g
		                //hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<31), (1<<31));  // fast switch release reset p2 r
		                hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<27), (1<<27));  // fast switch release reset p2 g
		                //hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<28), (1<<28));  // fast switch release reset p2 r
		            }
		            else
		            {
		                hdmi_mask(HDMIRX_2P0_PHY_P1_B4_reg, ~0x1, 0); //fast switch circuit disable  (B Lane)
		                hdmi_mask(HDMIRX_2P0_PHY_P1_B4_reg, ~(0x1<<3), (1<<3));  //
		                hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<30), (1<<30));  // fast switch release reset p2 g
		                //hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<29), (1<<29));  // fast switch release reset p2 b
		                hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<27), (1<<27));  // fast switch release reset p2 g
		                //hdmi_mask(HDMIRX_2P0_PHY_P1_DCDR_REG0, ~(0x1<<26), (1<<26));  // fast switch release reset p2 b
		            }
		        break;	
		    ///////////////////////////////////////////////////////////////////////////////////////////
		    case HDMI_14:
		    case HDMI_21:
		            // DPHY ON
		            hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);      //reg_p0_hd14_pow_save_bypass enable power saving byass (should be default value)
		            hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask),0);      //reg_p0_hd14_pow_on_manual_en Power on by FW
		            hdmi_mask(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask);    //reg_p0_hd14_pow_on_manual manual power=ON
		        break;	
		    }
	break;	


		case 2: //Port2
		     switch(mode)
		    {
		    case HDMI_20:

				// DPHY ON
				hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);    //reg_p0_hd14_pow_save_bypass bypass power saving (HDMI-1.4)
				hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_r_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_r_only_mask);    //Power on reg_p0_hd14_pow_save_bypass_r_only by FW
				hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_g_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_g_only_mask);    //Power on reg_p0_hd14_pow_save_bypass_g_only by FW
				hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_b_only_mask),HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_b_only_mask);    //Power onreg_p0_hd14_pow_save_bypass_b_only by FW

				hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask);        //Power on by FW
				hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask);    //reg_p0_hd14_pow_on_manual manual power setting = On (HDMI-1.4)
				udelay(10);
				hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask), 0);          //reg_p0_hd14_pow_on_manual_en Power on by MAC
				hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), 0);          //reg_p0_hd14_pow_on_manual manual power=OFF

		            // DPHY RESET
		            if (br_swap)
		            {
			        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_dig_rst_n_mask);  //P0_R_DIG_RST_N fast switch reset p0 r
			        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask);  //P0_G_DIG_RST_N fast switch reset p0 g
			        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_r_cdr_rst_n_mask);  //P0_R_CDR_RST_N  fast switch reset p0 r        
			        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask);  //P0_G_CDR_RST_N fast switch reset p0 g

		            }
		            else
		            {
			        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_dig_rst_n_mask);  //P0_G_DIG_RST_N fast switch reset p0 g
			        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_dig_rst_n_mask);  //P0_B_DIG_RST_N fast switch reset p0 b
			        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask),HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_g_cdr_rst_n_mask);  //P0_G_CDR_RST_N fast switch reset p0 g
			        hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0_reg, ~(HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask), HDMIRX_2P0_PHY_P0_DCDR_REG0_p0_b_cdr_rst_n_mask);  //P0_B_CDR_RST_N fast switch reset p0 b

		            }
		        break;

		    ////////////////////////////////////////////////////////////////////////////////////////
		    case HDMI_20_VS_WIN_OPP:
			 //Copy from Merlin7	
		            // DPHY ON
		            hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(0x1<<15), 1<<15);    // enable power saving byass (should be default value)
		            hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(0x1<<10), 1<<10);    //reg_p0_hd14_pow_save_bypass_r_only Power on by FW
		            hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(0x1<<9), 1<<9);    // reg_p0_hd14_pow_save_bypass_g_onlyPower on by FW
		            hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(0x1<<8), 1<<8);    //reg_p0_hd14_pow_save_bypass_b_only Power on by FW
		            hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(0x1<<14), 0);        // Power on by MAC
		            hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(0x1<<13), 1<<13);    // manual power=On

		            // DPHY RESET
		            if (br_swap)
		            {
		                hdmi_mask(HDMIRX_2P0_PHY_P2_R4_reg, ~0x1, 0); //fast switch circuit disable  (B Lane)
		                hdmi_mask(HDMIRX_2P0_PHY_P2_R4_reg, ~(0x1<<3), (1<<3));  //
		                hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<30), (1<<30));  // fast switch release reset p2 g
		                //hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<31), (1<<31));  // fast switch release reset p2 r
		                hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<27), (1<<27));  // fast switch release reset p2 g
		                //hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<28), (1<<28));  // fast switch release reset p2 r
		            }
		            else
		            {
		                hdmi_mask(HDMIRX_2P0_PHY_P2_B4_reg, ~0x1, 0); //fast switch circuit disable  (B Lane)
		                hdmi_mask(HDMIRX_2P0_PHY_P2_B4_reg, ~(0x1<<3), (1<<3));  //
		                hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<30), (1<<30));  // fast switch release reset p2 g
		                //hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<29), (1<<29));  // fast switch release reset p2 b
		                hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<27), (1<<27));  // fast switch release reset p2 g
		                //hdmi_mask(HDMIRX_2P0_PHY_P2_DCDR_REG0, ~(0x1<<26), (1<<26));  // fast switch release reset p2 b
		            }
		        break;	
		    ///////////////////////////////////////////////////////////////////////////////////////////
		    case HDMI_14:
		    case HDMI_21:
		            // DPHY ON
		            hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_save_bypass_mask);      //reg_p0_hd14_pow_save_bypass enable power saving byass (should be default value)
		            hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_en_mask),0);      //reg_p0_hd14_pow_on_manual_en Power on by FW
		            hdmi_mask(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg, ~(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask), HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg_p0_hd14_pow_on_manual_mask);    //reg_p0_hd14_pow_on_manual manual power=ON
		        break;	
		    }
	break;	


	}
#endif
}

void lib_hdmi_phy_dump_pow_saving_reg(unsigned char nport)
{
#ifdef CONFIG_RTK_KDRV_HDMI_ENABLE_PHY
#if 1 // Phy fw is not ready yet    
    switch(nport)
    {
    case 0:
        PWR_EMG("HDMI[p%d] aphy power saving setting (%08x=%08x, b0=%d / %08x=%08x b0=%d , %08x=%08x  b0=%d \n",
            nport,
            (HDMIRX_2P0_PHY_P0_R4_reg),
            (hdmi_in(HDMIRX_2P0_PHY_P0_R4_reg)),
            (hdmi_in(HDMIRX_2P0_PHY_P0_R4_reg)) & 1,
            (HDMIRX_2P0_PHY_P0_G4_reg),
            (hdmi_in(HDMIRX_2P0_PHY_P0_G4_reg)),
            (hdmi_in(HDMIRX_2P0_PHY_P0_G4_reg)) & 1,
            (HDMIRX_2P0_PHY_P0_B4_reg),
            (hdmi_in(HDMIRX_2P0_PHY_P0_B4_reg)),
            (hdmi_in(HDMIRX_2P0_PHY_P0_B4_reg)) & 1);

        PWR_EMG("HDMI[p%d] dphy power saving setting (%08x=%08x, b13=%d, b14=%d, b15=%d)\n",
            nport,
            (HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg),
            (hdmi_in(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg)),
            (hdmi_in(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg)>>13) & 1,
            (hdmi_in(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg)>>14) & 1,
            (hdmi_in(HDMIRX_2P0_PHY_P0_PSAVE_REG0_reg)>>15) & 1);
        break;

    case 1:
        PWR_EMG("HDMI[p%d] aphy power saving setting (%08x=%08x, b0=%d / %08x=%08x b0=%d , %08x=%08x  b0=%d \n",
            nport,
            (HDMIRX_2P0_PHY_P1_R4_reg),
            (hdmi_in(HDMIRX_2P0_PHY_P1_R4_reg)),
            (hdmi_in(HDMIRX_2P0_PHY_P1_R4_reg)) & 1,
            (HDMIRX_2P0_PHY_P1_G4_reg),
            (hdmi_in(HDMIRX_2P0_PHY_P1_G4_reg)),
            (hdmi_in(HDMIRX_2P0_PHY_P1_G4_reg)) & 1,
            (HDMIRX_2P0_PHY_P0_B4_reg),
            (hdmi_in(HDMIRX_2P0_PHY_P1_B4_reg)),
            (hdmi_in(HDMIRX_2P0_PHY_P1_B4_reg)) & 1);

        PWR_EMG("HDMI[p%d] dphy power saving setting (%08x=%08x, b13=%d, b14=%d, b15=%d)\n",
            nport,
            (HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg),
            (hdmi_in(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg)),
            (hdmi_in(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg)>>13) & 1,
            (hdmi_in(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg)>>14) & 1,
            (hdmi_in(HDMIRX_2P0_PHY_P1_PSAVE_REG0_reg)>>15) & 1);
        break;


    case 2:
        PWR_EMG("HDMI[p%d] aphy power saving setting (%08x=%08x, b0=%d / %08x=%08x b0=%d , %08x=%08x  b0=%d \n",
            nport,
            (HDMIRX_2P0_PHY_P2_R4_reg),
            (hdmi_in(HDMIRX_2P0_PHY_P2_R4_reg)),
            (hdmi_in(HDMIRX_2P0_PHY_P2_R4_reg)) & 1,
            (HDMIRX_2P0_PHY_P0_G4_reg),
            (hdmi_in(HDMIRX_2P0_PHY_P2_G4_reg)),
            (hdmi_in(HDMIRX_2P0_PHY_P2_G4_reg)) & 1,
            (HDMIRX_2P0_PHY_P0_B4_reg),
            (hdmi_in(HDMIRX_2P0_PHY_P2_B4_reg)),
            (hdmi_in(HDMIRX_2P0_PHY_P2_B4_reg)) & 1);

        PWR_EMG("HDMI[p%d] dphy power saving setting (%08x=%08x, b13=%d, b14=%d, b15=%d)\n",
            nport,
            (HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg),
            (hdmi_in(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg)),
            (hdmi_in(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg)>>13) & 1,
            (hdmi_in(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg)>>14) & 1,
            (hdmi_in(HDMIRX_2P0_PHY_P2_PSAVE_REG0_reg)>>15) & 1);
        break;

    }

#endif
#endif
}

