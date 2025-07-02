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
if(get_mach_type()==MACH_ARCH_RTK2885P){

        // merlin8 IC , run merlin8 flow
        //PHY Setting
        //REG_TOP_IN_2[0]  0: select HDMI 1.4 fast switch signal  1: select HDMI 2.1 fast switch signal
        hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<0), 0);
        //REG_TOP_IN_8<1>=1'b1
        hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<0), (0<<0));//]port-0 fast switch signal delay enable bit
        hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<0), (1<<0));
        //REG_TOP_IN_8<4:2>=1'b100
        hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(0x7<<2), (0x4<<2));

        //REG_P#_ACDR_*_8<1>=1'b1
        //REG_P#_ACDR_*_8<2>=1'b0 a 1
        //0x1800DA90[7][6]
        hdmi_mask(HDMIRX_PHY_r0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
        hdmi_mask(HDMIRX_PHY_r0_reg, ~(0x1<<7), 0);    //reset delay cell dff

        // 0x1800DA70[7][6]
        hdmi_mask(HDMIRX_PHY_g0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
        hdmi_mask(HDMIRX_PHY_g0_reg, ~(0x1<<7), 0);    //reset delay cell dff

        // 0x1800DA50[7][6]
        hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
        hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<7), 0);    //reset delay cell dff

        // 0x1800DA30[7][6]
        hdmi_mask(HDMIRX_PHY_ck0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
        hdmi_mask(HDMIRX_PHY_ck0_reg, ~(0x1<<7), 0);    //reset delay cell dff

        // DPHY
        // reset HD21 DPHY
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<11), 1<<11);   // Power on reg_p0_pow_save_bypass_ck by FW
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<10), 1<<10);   // Power on reg_p0_pow_save_bypass_r by FW
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<9), 1<<9);     // Power on reg_p0_pow_save_bypass_g by FW
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<8), 1<<8);    // Power onreg_p0_pow_save_bypass_b by FW
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<2), 0);       // reg_p0_pow_on_manual For MAC power saving, 1 : power on  0 : power off
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en    
       hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

	//hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0xF<<4), (0xF<<4));   //[7]~[4]ck,r,g,b  manual power setting = On 
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 1<<7);   // reg_p0_pow_on_manual_ck_en
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 1<<6);   // reg_p0_pow_on_manual_r_en
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 1<<5);     //  reg_p0_pow_on_manual_g_en
	hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 1<<4);    //  reg_p0_pow_on_manual_b_en
       hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<3), 1<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

        // release reset
        hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<0), (1<<0));  // fast switch release reset p0 b Reset_n for all digital CDR of B lane
        hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<1), (1<<1));  // fast switch release reset p0 g Reset_n for all digital CDR of G lane
        hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<2), (1<<2));  // fast switch release reset p0 r Reset_n for all digital CDR of R lane

        hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<4), (1<<4));  // fast switch release reset p0 b Reset_n for digital CDR of B lane
        hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<5), (1<<5));  // fast switch release reset p0 g Reset_n for digital CDR of G lane
        hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<6), (1<<6));  // fast switch release reset p0 r Reset_n for digital CDR of R lane


}
else //if(get_mach_type()==MACH_ARCH_RTK2885P2)||if(get_mach_type()==MACH_ARCH_RTK2885PP
{
        //merlin8p IC , run merlin8p flow

        //PHY Setting
        //REG_TOP_IN_2[0]  0: select HDMI 1.4 fast switch signal  1: select HDMI 2.1 fast switch signal
        hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<0), 0);
        //REG_TOP_IN_8<1>=1'b1
        hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<0), (0<<0));//]port-0 fast switch signal delay enable bit
        hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<0), (1<<0));
        //REG_TOP_IN_8<4:2>=1'b100
        hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(0x7<<2), (0x4<<2));

        //REG_P#_ACDR_*_8<1>=1'b1
        //REG_P#_ACDR_*_8<2>=1'b0 a 1
        //0x1800DA90[7][6]
        hdmi_mask(HDMIRX_PHY_r0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
        hdmi_mask(HDMIRX_PHY_r0_P_reg, ~(0x1<<7), 0);    //reset delay cell dff

        // 0x1800DA70[7][6]
        hdmi_mask(HDMIRX_PHY_g0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
        hdmi_mask(HDMIRX_PHY_g0_P_reg, ~(0x1<<7), 0);    //reset delay cell dff

        // 0x1800DA50[7][6]
        hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
        hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<7), 0);    //reset delay cell dff

        // 0x1800DA30[7][6]
        hdmi_mask(HDMIRX_PHY_ck0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
        hdmi_mask(HDMIRX_PHY_ck0_P_reg, ~(0x1<<7), 0);    //reset delay cell dff

        // DPHY
        // reset HD21 DPHY
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<11), 1<<11);   // Power on reg_p0_pow_save_bypass_ck by FW
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<10), 1<<10);   // Power on reg_p0_pow_save_bypass_r by FW
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<9), 1<<9);     // Power on reg_p0_pow_save_bypass_g by FW
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<8), 1<<8);    // Power onreg_p0_pow_save_bypass_b by FW
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<2), 0);       // reg_p0_pow_on_manual For MAC power saving, 1 : power on  0 : power off
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en    
       hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

	//hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0xF<<4), (0xF<<4));   //[7]~[4]ck,r,g,b  manual power setting = On 
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 1<<7);   // reg_p0_pow_on_manual_ck_en
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 1<<6);   // reg_p0_pow_on_manual_r_en
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 1<<5);     //  reg_p0_pow_on_manual_g_en
	hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 1<<4);    //  reg_p0_pow_on_manual_b_en
       hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<3), 1<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

        // release reset
        hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<0), (1<<0));  // fast switch release reset p0 b Reset_n for all digital CDR of B lane
        hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<1), (1<<1));  // fast switch release reset p0 g Reset_n for all digital CDR of G lane
        hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<2), (1<<2));  // fast switch release reset p0 r Reset_n for all digital CDR of R lane

        hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<4), (1<<4));  // fast switch release reset p0 b Reset_n for digital CDR of B lane
        hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<5), (1<<5));  // fast switch release reset p0 g Reset_n for digital CDR of G lane
        hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<6), (1<<6));  // fast switch release reset p0 r Reset_n for digital CDR of R lane

        
}

}

void lib_hdmi_phy_enter_power_saving_mode(unsigned char nport, HDMI_MODE mode, unsigned char br_swap)
{
    unsigned int hd21_mac = 0;
    PWR_EMG("HDMI[p%d] phy enter power saveing mode (mode=%d, br_swap=%d) \n", nport, mode, br_swap);

    if (hdmi_bypass_phy_ps_mode)
    {
        PWR_EMG("hdmi_bypass_phy_ps_mode is enabled, do nothing...\n");
        return ;
    }

if(get_mach_type()==MACH_ARCH_RTK2885P){

        // merlin8 IC , run merlin8 flow	
        //REG_TOP_IN_2[0]  0: select HDMI 1.4 fast switch signal  1: select HDMI 2.1 fast switch signal
        hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<0), 0);
        //REG_TOP_IN_2[0]=1'b1
        hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<1), (0<<1));//]port-0 fast switch signal delay enable bit
        hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<1), (1<<1));
        //REG_TOP_IN_2[0]<4:2>=1'b100
        hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(0x7<<2), (0x4<<2));
	
   switch(mode)
    {

    case HDMI_20:
            //APHY Setting
	     //REG_TOP_IN_2[0]  0: select HDMI 1.4 fast switch signal  1: select HDMI 2.1 fast switch signal
	     hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<0), 0);

            //REG_TOP_IN_2<1>=1'b1
            hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<1), (0<<1));//]port-0 fast switch signal delay enable bit
            hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<1), (1<<1));//]port-0 fast switch signal delay enable bit

            //REG_TOP_IN_2<4:2>=1'b100
            hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(0x7<<2), (0x4<<2));

            // POWER OFF FLOW HD20
            //0x1800DA90
            hdmi_mask(HDMIRX_PHY_r0_reg, ~(0x1<<6), 0);    // R Lane : fast switch circuit disable

            // 0x1800DA30
            hdmi_mask(HDMIRX_PHY_ck0_reg, ~(0x1<<6), 0);    // CK Lane : fast switch circuit disable

            // 0x1800DA70
            hdmi_mask(HDMIRX_PHY_g0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
            hdmi_mask(HDMIRX_PHY_g0_reg, ~(0x1<<7), 0);    //reset delay cell dff
            hdmi_mask(HDMIRX_PHY_g0_reg, ~(0x1<<7), 1<<7);//Must set before [6]
            hdmi_mask(HDMIRX_PHY_g0_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable

            // 0x1800DA68 [31:24] REG_P0_ACDR_B_8
            hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
            hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<7), 0);    //reset delay cell dff
            hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<7), 1<<7);//Must set before [6]
            hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable

            // Digital Phy Setting
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)
            hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<0), 0);  // fast switch  reset p0 b Reset_n for all digital CDR of B lane
            hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<1), 0);  // fast switch reset p0 g Reset_n for all digital CDR of G lane
            hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<4), 0);  // fast switch  reset p0 b Reset_n for digital CDR of B lane
            hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<5), 0);  // fast switch  reset p0 g Reset_n for digital CDR of G lane

           hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)

            // POWR OFF
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<11), 1<<11);   // Power on reg_p0_pow_save_bypass_ck by FW
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<10), 1<<10);   // Power on reg_p0_pow_save_bypass_r by FW
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<9), 1<<9);     // Power on reg_p0_pow_save_bypass_g by FW
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<8), 1<<8);    // Power onreg_p0_pow_save_bypass_b by FW
            
           // hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<14), 1<<14);  //pow_on_manual_en Power on by FW
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 1<<7);   // reg_p0_pow_on_manual_ck_en
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 1<<6);   // reg_p0_pow_on_manual_r_en
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 1<<5);     //  reg_p0_pow_on_manual_g_en
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 1<<4);    //  reg_p0_pow_on_manual_b_en
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<3), 1<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<2), 0);      // Power save manual mode control signal manul_power_on=OFF (should be default value)
        break;

    //////////////////////////////////////////////////////////////////////////////////////////
    //Logic b lane on/off, g lane always off, r/ck lane always on		
    case HDMI_20_VS_WIN_OPP:
            //APHY Setting
	     //REG_TOP_IN_2[0]  0: select HDMI 1.4 fast switch signal  1: select HDMI 2.1 fast switch signal
	     hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<0), 0);

            //REG_TOP_IN_2<1>=1'b1
            hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<1), (0<<1));//]port-0 fast switch signal delay enable bit
            hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<1), (1<<1));//]port-0 fast switch signal delay enable bit

            //REG_TOP_IN_2<4:2>=1'b100
            hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(0x7<<2), (0x4<<2));

            // POWER OFF FLOW HD20
            //0x1800DA90
            hdmi_mask(HDMIRX_PHY_r0_reg, ~(0x1<<6), 0);    // R Lane : ffast switch circuit disable
            // 0x1800DA30
            hdmi_mask(HDMIRX_PHY_ck0_reg, ~(0x1<<6), 0);    // CK Lane : ffast switch circuit disable

            // 0x1800DA88 [31:24] REG_P0_ACDR_G_8
            hdmi_mask(HDMIRX_PHY_g0_reg, ~(0x1<<6), 0);    // G Lane : ffast switch circuit disable


            hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
            hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<7), 0);    //reset delay cell dff
            hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<7), 1<<7);//Must set before [6]
            udelay(100);            
            hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable

            // Digital Phy Setting
           hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)
           hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<1), 0);  // fast switch reset p0 g Reset_n for all digital CDR of G lane
           hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<5), 0);  // fast switch release reset p0 g Reset_n for digital CDR of G lane

            // POWR OFF
            // hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<15), 0<<15);  // Disable power saving byass (Control by Mac)
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<11), 0<<11);   // Power save manual mode control signal
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<10), 1<<10);   // Power on reg_p0_pow_save_bypass_r by FW
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<9), 1<<9);     // Power on reg_p0_pow_save_bypass_g by FW
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<8), 0<<8);    // Power onreg_p0_pow_save_bypass_b by Mac
            
           // hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<14), 1<<14);  //pow_on_manual_en Power on by FW
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           
            
            hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<2), 0);      // Power save manual mode control signal manul_power_on=OFF (should be default value)
        break;
    ///////////////////////////////////////////////////////////////////////////////////////////
    case HDMI_14:
    case HDMI_21:

            hd21_mac = (hdmi_in(HDMIRX_PHY_top_in_0_reg) & (1<<0)) ? 1 : 0;

            if ((mode==HDMI_21 && !hd21_mac) || (mode!=HDMI_21 && hd21_mac) ||
                (hdmi_in(HDMIRX_PHY_r0_reg) & (1<<6))==0 ||
                (hdmi_in(HDMIRX_PHY_g0_reg) & (1<<6))==0 ||
                (hdmi_in(HDMIRX_PHY_b0_reg) & (1<<6))==0)
            {
                //PHY Setting
                //REG_TOP_IN_2[0]  0:
                hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<0), (mode==HDMI_21) ? (1<<0) : 0);
	        //REG_TOP_IN_8<1>=1'b1
	        hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<1), (0<<1));//]port-0 fast switch signal delay enable bit
	        hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(1<<1), (1<<1));
				
                //REG_TOP_IN_8<4:2>=1'b100
                hdmi_mask(HDMIRX_PHY_top_in_0_reg, ~(0x7<<2), (0x4<<2));

                //REG_P#_ACDR_*_8<1>=1'b1
                //REG_P#_ACDR_*_8<2>=1'b0 a 1
                //0x1800DAA8 [31:24] REG_P0_ACDR_R_8
               hdmi_mask(HDMIRX_PHY_r0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
               hdmi_mask(HDMIRX_PHY_r0_reg, ~(0x1<<7), 0);    //reset delay cell dff
               hdmi_mask(HDMIRX_PHY_r0_reg, ~(0x1<<7), 1<<7);//Must set before [6]
               udelay(100);            
               hdmi_mask(HDMIRX_PHY_r0_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable

                // 0x1800DA88 [31:24] REG_P0_ACDR_G_8
               hdmi_mask(HDMIRX_PHY_g0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
               hdmi_mask(HDMIRX_PHY_g0_reg, ~(0x1<<7), 0);    //reset delay cell dff
               hdmi_mask(HDMIRX_PHY_g0_reg, ~(0x1<<7), 1<<7);//Must set before [6]
               udelay(100);            
               hdmi_mask(HDMIRX_PHY_g0_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable

                // 0x1800DA68 [31:24] REG_P0_ACDR_B_8
               hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
               hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<7), 0);    //reset delay cell dff
               hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<7), 1<<7);//Must set before [6]
               udelay(100);            
               hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable

                // 0x1800DA48 [31:24] REG_P0_ACDR_CK_8
               hdmi_mask(HDMIRX_PHY_ck0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
               hdmi_mask(HDMIRX_PHY_ck0_reg, ~(0x1<<7), 0);    //reset delay cell dff
               hdmi_mask(HDMIRX_PHY_ck0_reg, ~(0x1<<7), 1<<7);//Must set before [6]
               udelay(100);            
               hdmi_mask(HDMIRX_PHY_ck0_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable
            }

 
            if (mode==HDMI_21)
            {
                hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<1), 1<<1);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)

                //hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<29), 1<<29);   // manual power on = ON  (HDMI-2.1)
                hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<2), 1<<2);      // Power save manual mode control signal manul_power_on=ON(should be default value)
                //hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<30), 0);       // power on cntrol by MAC (HDMI-2.1)
               hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
               hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
               hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
               hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en
               hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           


                //hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<31), 0);       // disable fast switch bypass (HDMI-2.1)
	         hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<11), 0);   // Power on reg_p0_pow_save_bypass_ck by MAC
	         hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<10), 0);   // Power on reg_p0_pow_save_bypass_r by MAC
	         hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<9), 0);     // Power on reg_p0_pow_save_bypass_g by MAC
	         hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<8), 0);    // Power onreg_p0_pow_save_bypass_b by MAC
            }
            else
            {
               hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)
               // hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<13), 1<<13);   // manual power on = ON (HDMI-1.4)
                hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<2), 0x1<<2);      // Power save manual mode control signal manul_power_on=ON (should be default value)

                //hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<14), 0);       // power on cntrol by MAC  (HDMI-1.4)
                hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
                hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
                hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
                hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en           
                hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

                //hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<15), 0);       // disable fast switch bypass (HDMI-1.4)
                hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<10), 0);   // Power on reg_p0_pow_save_bypass_r by MAC
                hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<9), 0);     // Power on reg_p0_pow_save_bypass_g by MAC
                hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<8), 0);    // Power onreg_p0_pow_save_bypass_b by MAC
            }
        break;

    }


}
else //if(get_mach_type()==MACH_ARCH_RTK2885P2)||if(get_mach_type()==MACH_ARCH_RTK2885PP
{
        //merlin8p IC , run merlin8p flow

        //REG_TOP_IN_2[0]  0: select HDMI 1.4 fast switch signal  1: select HDMI 2.1 fast switch signal
        hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<0), 0);
        //REG_TOP_IN_2[0]=1'b1
        hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<1), (0<<1));//]port-0 fast switch signal delay enable bit
        hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<1), (1<<1));
        //REG_TOP_IN_2[0]<4:2>=1'b100
        hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(0x7<<2), (0x4<<2));
	
   switch(mode)
    {

    case HDMI_20:
            //APHY Setting
	     //REG_TOP_IN_2[0]  0: select HDMI 1.4 fast switch signal  1: select HDMI 2.1 fast switch signal
	     hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<0), 0);

            //REG_TOP_IN_2<1>=1'b1
            hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<1), (0<<1));//]port-0 fast switch signal delay enable bit
            hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<1), (1<<1));//]port-0 fast switch signal delay enable bit

            //REG_TOP_IN_2<4:2>=1'b100
            hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(0x7<<2), (0x4<<2));

            // POWER OFF FLOW HD20
            //0x1800DA90
            hdmi_mask(HDMIRX_PHY_r0_P_reg, ~(0x1<<6), 0);    // R Lane : fast switch circuit disable

            // 0x1800DA30
            hdmi_mask(HDMIRX_PHY_ck0_P_reg, ~(0x1<<6), 0);    // CK Lane : fast switch circuit disable

            // 0x1800DA70
            hdmi_mask(HDMIRX_PHY_g0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
            hdmi_mask(HDMIRX_PHY_g0_P_reg, ~(0x1<<7), 0);    //reset delay cell dff
            hdmi_mask(HDMIRX_PHY_g0_P_reg, ~(0x1<<7), 1<<7);//Must set before [6]
            hdmi_mask(HDMIRX_PHY_g0_P_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable

            // 0x1800DA68 [31:24] REG_P0_ACDR_B_8
            hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
            hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<7), 0);    //reset delay cell dff
            hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<7), 1<<7);//Must set before [6]
            hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable

            // Digital Phy Setting
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)
            hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<0), 0);  // fast switch  reset p0 b Reset_n for all digital CDR of B lane
            hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<1), 0);  // fast switch reset p0 g Reset_n for all digital CDR of G lane
            hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<4), 0);  // fast switch  reset p0 b Reset_n for digital CDR of B lane
            hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<5), 0);  // fast switch  reset p0 g Reset_n for digital CDR of G lane

           hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)

            // POWR OFF
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<11), 1<<11);   // Power on reg_p0_pow_save_bypass_ck by FW
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<10), 1<<10);   // Power on reg_p0_pow_save_bypass_r by FW
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<9), 1<<9);     // Power on reg_p0_pow_save_bypass_g by FW
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<8), 1<<8);    // Power onreg_p0_pow_save_bypass_b by FW
            
           // hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<14), 1<<14);  //pow_on_manual_en Power on by FW
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 1<<7);   // reg_p0_pow_on_manual_ck_en
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 1<<6);   // reg_p0_pow_on_manual_r_en
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 1<<5);     //  reg_p0_pow_on_manual_g_en
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 1<<4);    //  reg_p0_pow_on_manual_b_en
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<3), 1<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<2), 0);      // Power save manual mode control signal manul_power_on=OFF (should be default value)
        break;

    //////////////////////////////////////////////////////////////////////////////////////////
    //Logic b lane on/off, g lane always off, r/ck lane always on		
    case HDMI_20_VS_WIN_OPP:
            //APHY Setting
	     //REG_TOP_IN_2[0]  0: select HDMI 1.4 fast switch signal  1: select HDMI 2.1 fast switch signal
	     hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<0), 0);

            //REG_TOP_IN_2<1>=1'b1
            hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<1), (0<<1));//]port-0 fast switch signal delay enable bit
            hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<1), (1<<1));//]port-0 fast switch signal delay enable bit

            //REG_TOP_IN_2<4:2>=1'b100
            hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(0x7<<2), (0x4<<2));

            // POWER OFF FLOW HD20
            //0x1800DA90
            hdmi_mask(HDMIRX_PHY_r0_P_reg, ~(0x1<<6), 0);    // R Lane : ffast switch circuit disable
            // 0x1800DA30
            hdmi_mask(HDMIRX_PHY_ck0_P_reg, ~(0x1<<6), 0);    // CK Lane : ffast switch circuit disable

            // 0x1800DA88 [31:24] REG_P0_ACDR_G_8
            hdmi_mask(HDMIRX_PHY_g0_P_reg, ~(0x1<<6), 0);    // G Lane : ffast switch circuit disable


            hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
            hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<7), 0);    //reset delay cell dff
            hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<7), 1<<7);//Must set before [6]
            udelay(100);            
            hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable

            // Digital Phy Setting
           hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)
           hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<1), 0);  // fast switch reset p0 g Reset_n for all digital CDR of G lane
           hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<5), 0);  // fast switch release reset p0 g Reset_n for digital CDR of G lane

            // POWR OFF
            // hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<15), 0<<15);  // Disable power saving byass (Control by Mac)
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<11), 0<<11);   // Power save manual mode control signal
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<10), 1<<10);   // Power on reg_p0_pow_save_bypass_r by FW
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<9), 1<<9);     // Power on reg_p0_pow_save_bypass_g by FW
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<8), 0<<8);    // Power onreg_p0_pow_save_bypass_b by Mac
            
           // hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<14), 1<<14);  //pow_on_manual_en Power on by FW
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           
            
            hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<2), 0);      // Power save manual mode control signal manul_power_on=OFF (should be default value)
        break;
    ///////////////////////////////////////////////////////////////////////////////////////////
    case HDMI_14:
    case HDMI_21:

            hd21_mac = (hdmi_in(HDMIRX_PHY_top_in_0_P_reg) & (1<<0)) ? 1 : 0;

            if ((mode==HDMI_21 && !hd21_mac) || (mode!=HDMI_21 && hd21_mac) ||
                (hdmi_in(HDMIRX_PHY_r0_P_reg) & (1<<6))==0 ||
                (hdmi_in(HDMIRX_PHY_g0_P_reg) & (1<<6))==0 ||
                (hdmi_in(HDMIRX_PHY_b0_P_reg) & (1<<6))==0)
            {
                //PHY Setting
                //REG_TOP_IN_2[0]  0:
                hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<0), (mode==HDMI_21) ? (1<<0) : 0);
	        //REG_TOP_IN_8<1>=1'b1
	        hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<1), (0<<1));//]port-0 fast switch signal delay enable bit
	        hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(1<<1), (1<<1));
				
                //REG_TOP_IN_8<4:2>=1'b100
                hdmi_mask(HDMIRX_PHY_top_in_0_P_reg, ~(0x7<<2), (0x4<<2));

                //REG_P#_ACDR_*_8<1>=1'b1
                //REG_P#_ACDR_*_8<2>=1'b0 a 1
                //0x1800DAA8 [31:24] REG_P0_ACDR_R_8
               hdmi_mask(HDMIRX_PHY_r0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
               hdmi_mask(HDMIRX_PHY_r0_P_reg, ~(0x1<<7), 0);    //reset delay cell dff
               hdmi_mask(HDMIRX_PHY_r0_P_reg, ~(0x1<<7), 1<<7);//Must set before [6]
               udelay(100);            
               hdmi_mask(HDMIRX_PHY_r0_P_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable

                // 0x1800DA88 [31:24] REG_P0_ACDR_G_8
               hdmi_mask(HDMIRX_PHY_g0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
               hdmi_mask(HDMIRX_PHY_g0_P_reg, ~(0x1<<7), 0);    //reset delay cell dff
               hdmi_mask(HDMIRX_PHY_g0_P_reg, ~(0x1<<7), 1<<7);//Must set before [6]
               udelay(100);            
               hdmi_mask(HDMIRX_PHY_g0_P_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable

                // 0x1800DA68 [31:24] REG_P0_ACDR_B_8
               hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
               hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<7), 0);    //reset delay cell dff
               hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<7), 1<<7);//Must set before [6]
               udelay(100);            
               hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable

                // 0x1800DA48 [31:24] REG_P0_ACDR_CK_8
               hdmi_mask(HDMIRX_PHY_ck0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
               hdmi_mask(HDMIRX_PHY_ck0_P_reg, ~(0x1<<7), 0);    //reset delay cell dff
               hdmi_mask(HDMIRX_PHY_ck0_P_reg, ~(0x1<<7), 1<<7);//Must set before [6]
               udelay(100);            
               hdmi_mask(HDMIRX_PHY_ck0_P_reg, ~(0x1<<6), 1<<6);//fast switch circuit enable
            }

 
            if (mode==HDMI_21)
            {
                hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<1), 1<<1);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)

                //hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<29), 1<<29);   // manual power on = ON  (HDMI-2.1)
                hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<2), 1<<2);      // Power save manual mode control signal manul_power_on=ON(should be default value)
                //hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<30), 0);       // power on cntrol by MAC (HDMI-2.1)
               hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
               hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
               hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
               hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en
               hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           


                //hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<31), 0);       // disable fast switch bypass (HDMI-2.1)
	         hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<11), 0);   // Power on reg_p0_pow_save_bypass_ck by MAC
	         hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<10), 0);   // Power on reg_p0_pow_save_bypass_r by MAC
	         hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<9), 0);     // Power on reg_p0_pow_save_bypass_g by MAC
	         hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<8), 0);    // Power onreg_p0_pow_save_bypass_b by MAC
            }
            else
            {
               hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)
               // hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_P_reg, ~(0x1<<13), 1<<13);   // manual power on = ON (HDMI-1.4)
                hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<2), 0x1<<2);      // Power save manual mode control signal manul_power_on=ON (should be default value)

                //hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<14), 0);       // power on cntrol by MAC  (HDMI-1.4)
                hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
                hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
                hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
                hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en           
                hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

                //hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<15), 0);       // disable fast switch bypass (HDMI-1.4)
                hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<10), 0);   // Power on reg_p0_pow_save_bypass_r by MAC
                hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<9), 0);     // Power on reg_p0_pow_save_bypass_g by MAC
                hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<8), 0);    // Power onreg_p0_pow_save_bypass_b by MAC
            }
        break;

    }




}
   
}

void lib_hdmi_phy_exit_power_saving_mode(unsigned char nport, HDMI_MODE mode, unsigned char br_swap)
{
    PWR_EMG("HDMI[p%d] phy exit power saveing mode (mode=%d) \n", nport, mode);

    if (hdmi_bypass_phy_ps_mode)
    {
        PWR_EMG("hdmi_bypass_phy_ps_mode is enabled, do nothing...\n");
        return ;
    }


if(get_mach_type()==MACH_ARCH_RTK2885P){

        // merlin8 IC , run merlin8 flow
    switch(mode)
    {
    case HDMI_20:

		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)

		// DPHY
		// reset HD21 DPHY
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<11), 1<<11);   // Power on reg_p0_pow_save_bypass_ck by FW
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<10), 1<<10);   // Power on reg_p0_pow_save_bypass_r by FW
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<9), 1<<9);     // Power on reg_p0_pow_save_bypass_g by FW
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<8), 1<<8);    // Power onreg_p0_pow_save_bypass_b by FW

		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<2), 0);       // reg_p0_pow_on_manual For MAC power saving, 1 : power on  0 : power off
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en    
	       hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           
		
		//hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0xF<<4), (0xF<<4));   //[7]~[4]ck,r,g,b  manual power setting = On 
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 1<<7);   // reg_p0_pow_on_manual_ck_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 1<<6);   // reg_p0_pow_on_manual_r_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 1<<5);     //  reg_p0_pow_on_manual_g_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 1<<4);    //  reg_p0_pow_on_manual_b_en
	       hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<3), 1<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

		// release reset
		hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<0), (1<<0));  // fast switch release reset p0 b Reset_n for all digital CDR of B lane
		hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<1), (1<<1));  // fast switch release reset p0 g Reset_n for all digital CDR of G lane
		hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<2), (1<<2));  // fast switch release reset p0 r Reset_n for all digital CDR of R lane

		hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<4), (1<<4));  // fast switch release reset p0 b Reset_n for digital CDR of B lane
		hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<5), (1<<5));  // fast switch release reset p0 g Reset_n for digital CDR of G lane
		hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<6), (1<<6));  // fast switch release reset p0 r Reset_n for digital CDR of R lane

		// DPHY ON
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<10), 1<<10);   // Power on reg_p0_pow_save_bypass_r by FW
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<9), 1<<9);     // Power on reg_p0_pow_save_bypass_g by FW
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<8), 1<<8);    // Power onreg_p0_pow_save_bypass_b by FW

		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<14), 1<<14);    // Power on by FW
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 1<<7);   // reg_p0_pow_on_manual_ck_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 1<<6);   // reg_p0_pow_on_manual_r_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 1<<5);     //  reg_p0_pow_on_manual_g_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 1<<4);    //  reg_p0_pow_on_manual_b_en


		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<13), (1<<13));  // manual power=ON
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<2), 0x1<<2);      // Power save manual mode control signal manul_power_on=ON (should be default value)
		udelay(10);
		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<14), 0);        // Power on control by MAC
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en           
	       hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<13), 0);        // manual power=OFF
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<2), 0x0<<2);      // Power save manual mode control signal manul_power_on=OFF (should be default value)

		// DPHY RESET
		hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<0), (1<<0));  // fast switch release reset p0 b Reset_n for all digital CDR of B lane
		hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<1), (1<<1));  // fast switch release reset p0 g Reset_n for all digital CDR of G lane
		hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<4), (1<<4));  // fast switch release reset p0 b Reset_n for digital CDR of B lane
		hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<5), (1<<5));  // fast switch release reset p0 g Reset_n for digital CDR of G lane

        break;

    ////////////////////////////////////////////////////////////////////////////////////////
	    case HDMI_20_VS_WIN_OPP:
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)
		// DPHY ON
		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<13), 1<<13);    // manual power on = ON  (HDMI-1.4)
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<2), 0x1<<2);      // Power save manual mode control signal manul_power_on=ON (should be default value)

		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<14), 0);        // Power on control by MAC   (HDMI-1.4)
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en   
	       hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<15), 1<<15);    // enable power saving byass  (HDMI-1.4)
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<10), 1<<10);   // Power on reg_p0_pow_save_bypass_r by FW
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<9), 1<<9);     // Power on reg_p0_pow_save_bypass_g by FW
		hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<8), 1<<8);    // Power onreg_p0_pow_save_bypass_b by FW

		// DPHY RESET
		hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<1), (1<<1));  // fast switch release reset p0 g Reset_n for all digital CDR of G lane
		hdmi_mask(HDMIRX_PHY_cdr_regd00, ~(0x1<<5), (1<<5));  // fast switch release reset p0 g Reset_n for digital CDR of G lane

		// 0x1800DA68 [31:24] REG_P0_ACDR_B_8
		hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
        break;	
    ///////////////////////////////////////////////////////////////////////////////////////////
    case HDMI_14:
    case HDMI_21:
		if ((hdmi_in(HDMIRX_PHY_top_in_0_reg) & (1<<0)))  // HDMI 2.1
		{
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<1), 1);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)
			//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<29), 1<<29);    // manual power on = ON  (HDMI-2.1)
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<2), 0x1<<2);      // Power save manual mode control signal manul_power_on=ON (should be default value)

			//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<30), 0);        // power on cntrol by MAC (HDMI-2.1)
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en   
	              hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

			//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<31), 0);        // disable fast switch bypass (HDMI-2.1)
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<11), 0);   // Power on reg_p0_pow_save_bypass_ck by MAC
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<10), 0);   // Power on reg_p0_pow_save_bypass_r by MAC
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<9), 0);     // Power on reg_p0_pow_save_bypass_g by MAC
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<8), 0);    // Power onreg_p0_pow_save_bypass_b by MAC
		}
		else
		{
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)
			// hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<13), 1<<13);    // manual power on = ON  (HDMI-1.4)
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<2), 0x1<<2);      // Power save manual mode control signal manul_power_on=ON (should be default value)

			//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<14), 0);        // Power on control by MAC   (HDMI-1.4)
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en                   
	              hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

			//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<15), 0);        // enable power saving byass  (HDMI-1.4)
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<10), 0);   // Power on reg_p0_pow_save_bypass_r by MAC
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<9), 0);     // Power on reg_p0_pow_save_bypass_g by MAC
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<8), 0);    // Power onreg_p0_pow_save_bypass_b by MAC
		}
        break;	

    }
	//0x1800DAA8 [31:24] REG_P0_ACDR_R_8
	hdmi_mask(HDMIRX_PHY_r0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
	// 0x1800DA88 [31:24] REG_P0_ACDR_G_8
	hdmi_mask(HDMIRX_PHY_g0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
	// 0x1800DA68 [31:24] REG_P0_ACDR_B_8
	hdmi_mask(HDMIRX_PHY_b0_reg, ~(0x1<<6), 0);    //fast switch circuit disable
	// 0x1800DA48 [31:24] REG_P0_ACDR_CK_8
	hdmi_mask(HDMIRX_PHY_ck0_reg, ~(0x1<<6), 0);    //fast switch circuit disable


}
else //if(get_mach_type()==MACH_ARCH_RTK2885P2)||if(get_mach_type()==MACH_ARCH_RTK2885PP
{
        //merlin8p IC , run merlin8p flow
    switch(mode)
    {
    case HDMI_20:

		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)

		// DPHY
		// reset HD21 DPHY
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<11), 1<<11);   // Power on reg_p0_pow_save_bypass_ck by FW
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<10), 1<<10);   // Power on reg_p0_pow_save_bypass_r by FW
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<9), 1<<9);     // Power on reg_p0_pow_save_bypass_g by FW
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<8), 1<<8);    // Power onreg_p0_pow_save_bypass_b by FW

		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<2), 0);       // reg_p0_pow_on_manual For MAC power saving, 1 : power on  0 : power off
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en    
	       hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           
		
		//hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0xF<<4), (0xF<<4));   //[7]~[4]ck,r,g,b  manual power setting = On 
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 1<<7);   // reg_p0_pow_on_manual_ck_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 1<<6);   // reg_p0_pow_on_manual_r_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 1<<5);     //  reg_p0_pow_on_manual_g_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 1<<4);    //  reg_p0_pow_on_manual_b_en
	       hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<3), 1<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

		// release reset
		hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<0), (1<<0));  // fast switch release reset p0 b Reset_n for all digital CDR of B lane
		hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<1), (1<<1));  // fast switch release reset p0 g Reset_n for all digital CDR of G lane
		hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<2), (1<<2));  // fast switch release reset p0 r Reset_n for all digital CDR of R lane

		hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<4), (1<<4));  // fast switch release reset p0 b Reset_n for digital CDR of B lane
		hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<5), (1<<5));  // fast switch release reset p0 g Reset_n for digital CDR of G lane
		hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<6), (1<<6));  // fast switch release reset p0 r Reset_n for digital CDR of R lane

		// DPHY ON
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<10), 1<<10);   // Power on reg_p0_pow_save_bypass_r by FW
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<9), 1<<9);     // Power on reg_p0_pow_save_bypass_g by FW
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<8), 1<<8);    // Power onreg_p0_pow_save_bypass_b by FW

		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<14), 1<<14);    // Power on by FW
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 1<<7);   // reg_p0_pow_on_manual_ck_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 1<<6);   // reg_p0_pow_on_manual_r_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 1<<5);     //  reg_p0_pow_on_manual_g_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 1<<4);    //  reg_p0_pow_on_manual_b_en


		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<13), (1<<13));  // manual power=ON
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<2), 0x1<<2);      // Power save manual mode control signal manul_power_on=ON (should be default value)
		udelay(10);
		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_P_reg, ~(0x1<<14), 0);        // Power on control by MAC
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en           
	       hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_P_reg, ~(0x1<<13), 0);        // manual power=OFF
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<2), 0x0<<2);      // Power save manual mode control signal manul_power_on=OFF (should be default value)

		// DPHY RESET
		hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<0), (1<<0));  // fast switch release reset p0 b Reset_n for all digital CDR of B lane
		hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<1), (1<<1));  // fast switch release reset p0 g Reset_n for all digital CDR of G lane
		hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<4), (1<<4));  // fast switch release reset p0 b Reset_n for digital CDR of B lane
		hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<5), (1<<5));  // fast switch release reset p0 g Reset_n for digital CDR of G lane

        break;

    ////////////////////////////////////////////////////////////////////////////////////////
	    case HDMI_20_VS_WIN_OPP:
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)
		// DPHY ON
		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_P_reg, ~(0x1<<13), 1<<13);    // manual power on = ON  (HDMI-1.4)
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<2), 0x1<<2);      // Power save manual mode control signal manul_power_on=ON (should be default value)

		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_P_reg, ~(0x1<<14), 0);        // Power on control by MAC   (HDMI-1.4)
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en   
	       hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

		//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg, ~(0x1<<15), 1<<15);    // enable power saving byass  (HDMI-1.4)
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<10), 1<<10);   // Power on reg_p0_pow_save_bypass_r by FW
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<9), 1<<9);     // Power on reg_p0_pow_save_bypass_g by FW
		hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<8), 1<<8);    // Power onreg_p0_pow_save_bypass_b by FW

		// DPHY RESET
		hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<1), (1<<1));  // fast switch release reset p0 g Reset_n for all digital CDR of G lane
		hdmi_mask(HDMIRX_PHY_cdr_P_regd00, ~(0x1<<5), (1<<5));  // fast switch release reset p0 g Reset_n for digital CDR of G lane

		// 0x1800DA68 [31:24] REG_P0_ACDR_B_8
		hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
        break;	
    ///////////////////////////////////////////////////////////////////////////////////////////
    case HDMI_14:
    case HDMI_21:
		if ((hdmi_in(HDMIRX_PHY_top_in_0_P_reg) & (1<<0)))  // HDMI 2.1
		{
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<1), 1);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)
			//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_P_reg, ~(0x1<<29), 1<<29);    // manual power on = ON  (HDMI-2.1)
			hdmi_mask(HDMIRX_PHY_psave_regd00, ~(0x1<<2), 0x1<<2);      // Power save manual mode control signal manul_power_on=ON (should be default value)

			//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_P_reg, ~(0x1<<30), 0);        // power on cntrol by MAC (HDMI-2.1)
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en   
	              hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

			//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_P_reg, ~(0x1<<31), 0);        // disable fast switch bypass (HDMI-2.1)
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<11), 0);   // Power on reg_p0_pow_save_bypass_ck by MAC
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<10), 0);   // Power on reg_p0_pow_save_bypass_r by MAC
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<9), 0);     // Power on reg_p0_pow_save_bypass_g by MAC
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<8), 0);    // Power onreg_p0_pow_save_bypass_b by MAC
		}
		else
		{
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<1), 0);   //[1]Power save signal from MAC 0: 10 bits (TMDS)  1: 18 bits(FRL)
			// hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_P_reg, ~(0x1<<13), 1<<13);    // manual power on = ON  (HDMI-1.4)
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<2), 0x1<<2);      // Power save manual mode control signal manul_power_on=ON (should be default value)

			//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_P_reg, ~(0x1<<14), 0);        // Power on control by MAC   (HDMI-1.4)
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<7), 0<<7);   // reg_p0_pow_on_manual_ck_en
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<6), 0<<6);   // reg_p0_pow_on_manual_r_en
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<5), 0<<5);     //  reg_p0_pow_on_manual_g_en
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<4), 0<<4);    //  reg_p0_pow_on_manual_b_en                   
	              hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<3), 0<<3);    //  reg_p0_pow_on_manual_aphy_en Enable power save manual mode for APHY           

			//hdmi_mask(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_P_reg, ~(0x1<<15), 0);        // enable power saving byass  (HDMI-1.4)
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<10), 0);   // Power on reg_p0_pow_save_bypass_r by MAC
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<9), 0);     // Power on reg_p0_pow_save_bypass_g by MAC
			hdmi_mask(HDMIRX_PHY_psave_P_regd00, ~(0x1<<8), 0);    // Power onreg_p0_pow_save_bypass_b by MAC
		}
        break;	

    }
	//0x1800DAA8 [31:24] REG_P0_ACDR_R_8
	hdmi_mask(HDMIRX_PHY_r0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
	// 0x1800DA88 [31:24] REG_P0_ACDR_G_8
	hdmi_mask(HDMIRX_PHY_g0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
	// 0x1800DA68 [31:24] REG_P0_ACDR_B_8
	hdmi_mask(HDMIRX_PHY_b0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable
	// 0x1800DA48 [31:24] REG_P0_ACDR_CK_8
	hdmi_mask(HDMIRX_PHY_ck0_P_reg, ~(0x1<<6), 0);    //fast switch circuit disable


}	

}

void lib_hdmi_phy_dump_pow_saving_reg(unsigned char nport)
{

if(get_mach_type()==MACH_ARCH_RTK2885P){
	
        // merlin8 IC , run merlin8 flow
#if 1 // Phy fw is not ready yet    
	PWR_EMG("HDMI[p%d] aphy power saving setting (%08x=%08x, b6=%d / %08x=%08x b6=%d , %08x=%08x  b6=%d)\n",
	nport,
	(HDMIRX_PHY_r0_reg),
	(hdmi_in(HDMIRX_PHY_r0_reg)),
	(hdmi_in(HDMIRX_PHY_r0_reg)>>6) & 1,
	(HDMIRX_PHY_g0_reg),
	(hdmi_in(HDMIRX_PHY_g0_reg)),
	(hdmi_in(HDMIRX_PHY_g0_reg)>>6) & 1,
	(HDMIRX_PHY_b0_reg),
	(hdmi_in(HDMIRX_PHY_b0_reg)),
	(hdmi_in(HDMIRX_PHY_b0_reg)>>6) & 1);
/*
        PWR_EMG("HDMI[p%d] dphy power saving setting (%08x=%08x, b[31:29]=%d%d%d, b[15:13]=%d%d%d)\n",
            nport,
            (HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg),
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)),
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)>>31) & 1,
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)>>30) & 1,
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)>>29) & 1,
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)>>15) & 1,
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)>>14) & 1,
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)>>13) & 1);*/
#endif
}
else //if(get_mach_type()==MACH_ARCH_RTK2885P2)||if(get_mach_type()==MACH_ARCH_RTK2885PP
{

        //merlin8p IC , run merlin8p flow

#if 1 // Phy fw is not ready yet    
	PWR_EMG("HDMI[p%d] aphy power saving setting (%08x=%08x, b6=%d / %08x=%08x b6=%d , %08x=%08x  b6=%d)\n",
	nport,
	(HDMIRX_PHY_r0_P_reg),
	(hdmi_in(HDMIRX_PHY_r0_P_reg)),
	(hdmi_in(HDMIRX_PHY_r0_P_reg)>>6) & 1,
	(HDMIRX_PHY_g0_P_reg),
	(hdmi_in(HDMIRX_PHY_g0_P_reg)),
	(hdmi_in(HDMIRX_PHY_g0_P_reg)>>6) & 1,
	(HDMIRX_PHY_b0_P_reg),
	(hdmi_in(HDMIRX_PHY_b0_P_reg)),
	(hdmi_in(HDMIRX_PHY_b0_P_reg)>>6) & 1);
/*
        PWR_EMG("HDMI[p%d] dphy power saving setting (%08x=%08x, b[31:29]=%d%d%d, b[15:13]=%d%d%d)\n",
            nport,
            (HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg),
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)),
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)>>31) & 1,
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)>>30) & 1,
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)>>29) & 1,
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)>>15) & 1,
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)>>14) & 1,
            (hdmi_in(HDMIRX_2P1_PHY_GLOBAL_HD21_Y1_REGD32_reg)>>13) & 1);*/
#endif


}

}

