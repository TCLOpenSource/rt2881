#ifndef __HDMI_DP_COMMON_SEL_H__
#define __HDMI_DP_COMMON_SEL_H__

extern unsigned char lib_hdmi_get_hdmi_dp_sel(void);
extern void lib_hdmi_set_hdmi_dp_sel(unsigned char en);

#ifdef CONFIG_RTK_KDRV_HDMI_DYNAMIC_HDMI_DP_SEL_SWITCH

extern unsigned char newbase_hdmi_check_hdmi_dp_sel_setting(void);
extern unsigned char newbase_hdmi_get_hdmi_vpll_ownership(void);

#else

#define newbase_hdmi_check_hdmi_dp_sel_setting()     lib_hdmi_get_hdmi_dp_sel()
#define newbase_hdmi_get_hdmi_vpll_ownership()       TRUE
#endif

extern unsigned char newbase_hdmi_get_common_420_selection(void);

#endif //__HDMI_DP_COMMON_SEL_H__
