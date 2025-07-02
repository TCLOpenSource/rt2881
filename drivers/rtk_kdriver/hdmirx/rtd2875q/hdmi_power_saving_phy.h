#ifndef __HDMI_POWER_SAVING_PHY_H__
#define __HDMI_POWER_SAVING_PHY_H__

#ifdef CONFIG_RTK_KDRV_HDMI_ENABLE_PHY
//------------------------------------------------------------------
// Low Level Power Saving API - PHY
//------------------------------------------------------------------
extern void lib_hdmi_phy_reset_power_saving_state(unsigned char nport);
extern void lib_hdmi_phy_enter_power_saving_mode(unsigned char nport, HDMI_MODE mode, unsigned char br_swap);
extern void lib_hdmi_phy_exit_power_saving_mode(unsigned char nport, HDMI_MODE mode, unsigned char br_swap);
extern void lib_hdmi_phy_dump_pow_saving_reg(unsigned char nport);
#else
#define lib_hdmi_phy_enter_power_saving_mode(nport, mode,br_swap)
#define lib_hdmi_phy_exit_power_saving_mode(nport, mode,br_swap)
#define lib_hdmi_phy_reset_power_saving_state(port)
#define lib_hdmi_phy_dump_pow_saving_reg(port)
#endif

#endif //__HDMI_POWER_SAVING_PHY_H__
