#include "hdmi_dp_common_sel.h"
#include <rtk_kdriver/rtk_otp_region_api.h>

//--------------------------------------------------
// Func  : lib_hdmi_get_hdmi_dp_sel
//
// Desc  : get hdmi dp_sel value
//
// Param : N/A
//
// Retn  : 0 : HDMI, others: DP
//--------------------------------------------------
unsigned char lib_hdmi_get_hdmi_dp_sel(void)
{
    return SYS_REG_SYS_CLKSEL_get_hdmi_dp_sel(hdmi_in(SYS_REG_SYS_CLKSEL_reg));
}

//--------------------------------------------------
// Func  : lib_hdmi_get_hdmi_dp_sel
//
// Desc  : set hdmi dp_sel value
//
// Param : 0 : HDMI, others: DP
//
// Retn  : N/A
//--------------------------------------------------
void lib_hdmi_set_hdmi_dp_sel(unsigned char en)
{
    hdmi_mask(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_hdmi_dp_sel_mask, (en) ? SYS_REG_SYS_CLKSEL_hdmi_dp_sel_mask : 0);
}

//--------------------------------------------------
// Func  : _is_hdmi_dp_combo_platform
//
// Desc  : check platform has combo port or not
//
// Param : N/A
//
// Retn  : 0 : platform has no hdmi dp combo port
//         1 : platform has hdmi dp combo port
//--------------------------------------------------
unsigned char _is_hdmi_dp_combo_platform(void)
{
#if IS_ENABLED(CONFIG_RTK_KDRV_OTP)
    static unsigned char g_hdmi_dp_combo = 0xF;

    if (g_hdmi_dp_combo==0xF)
    {
        if (rtk_get_set_type() == SET_TYPE_TV || rtk_otp_field_read_int_by_name("chip_type_dp_rx_disable") == 1)
            g_hdmi_dp_combo = 0;   // no combo port
        else
            g_hdmi_dp_combo = 1;   // might have combo port
    }

    return g_hdmi_dp_combo;
#else
    return 0;
#endif
}

#ifdef CONFIG_RTK_KDRV_HDMI_DYNAMIC_HDMI_DP_SEL_SWITCH

#define  SYS_REG_DPRX_SYS_SRST_reg                                 SYS_REG_SYS_SRST4_reg
#define  SYS_REG_DPRX_SYS_SRST_RSTN_DPRX_MASK                      SYS_REG_SYS_SRST4_rstn_dprx_mask
#define  SYS_REG_DPRX_SYS_SRST_WRITE_DATA(data)                    SYS_REG_SYS_SRST4_write_data(data)

#define  SYS_REG_DPRX_SYS_CLKEN_reg                                SYS_REG_SYS_CLKEN4_reg
#define  SYS_REG_DPRX_SYS_CLKEN_CLKEN_DPRX_MASK                    SYS_REG_SYS_CLKEN4_clken_dprx_mask
#define  SYS_REG_DPRX_SYS_CLKEN_WRITE_DATA(data)                   SYS_REG_SYS_CLKEN4_write_data(data)

//--------------------------------------------------
// Func  : _update_hdmi_dp_sel_setting
//
// Desc  : for RTD2885P, bits hdmi_dp_sel is used to
//         control the following setting:
//         1. combo port fifo path:
//         2. DSCD clock input path.
//         where bit value =0 measn to HDMI,
//         and value 1 means to DPRX
//
//         FW needs to choose this value carefully,
//         otherwise the HW will work abnormally
//
// Param : hdmi_dp_sel : current setting
//
// Retn  : expected hdmi_dp_sel
//--------------------------------------------------
static unsigned char _update_hdmi_dp_sel_setting(
    unsigned char           hdmi_dp_sel
    )
{
    if (_is_hdmi_dp_combo_platform()==0)
        return 0;

    // if DP is CRT off, force hdmi_dp_sel to 0
    if ((hdmi_in(SYS_REG_DPRX_SYS_SRST_reg) & SYS_REG_DPRX_SYS_SRST_RSTN_DPRX_MASK)==0 ||
        (hdmi_in(SYS_REG_DPRX_SYS_CLKEN_reg) & SYS_REG_DPRX_SYS_CLKEN_CLKEN_DPRX_MASK)==0)
        return 0;

    // scenario check
    if (newbase_hdmi_is_dispsrc(SLR_MAIN_DISPLAY))   // current source is hdmi
    {
        unsigned char port = newbase_hdmi_get_current_display_port();

        // Check if HDMI port needs DSC ?
        if (newbase_rxphy_get_frl_mode(newbase_hdmi_get_current_display_port()) != MODE_TMDS)
        {
            if (hdmi_dp_sel)
            {
                HDMI_WARN("[SHARE_DSCD] port = %d, frl_mode=%d/%d to HDMI\n", port, newbase_rxphy_get_frl_mode(port), MODE_TMDS);
            }
            return 0;
        }
    }
    else
        return 1;   // switch combo port to DPRX

    return hdmi_dp_sel;
}


//--------------------------------------------------
// Func  : newbase_hdmi_check_hdmi_dp_sel_setting
//
// Desc  : for RTD2885P, bits hdmi_dp_sel is used to
//         control the following setting:
//         1. combo port fifo path:
//         2. DSCD clock input path.
//         where bit value =0 measn to HDMI,
//         and value 1 means to DPRX
//
//         FW needs to choose this value carefully,
//         otherwise the HW will work abnormally
//
// Param : N/A
//
// Retn  : expected hdmi_dp_sel
//--------------------------------------------------
unsigned char newbase_hdmi_check_hdmi_dp_sel_setting(void)
{
    unsigned char cur_hdmi_dp_sel = lib_hdmi_get_hdmi_dp_sel();
    unsigned char hdmi_dp_sel = _update_hdmi_dp_sel_setting(cur_hdmi_dp_sel);

    if  (cur_hdmi_dp_sel != hdmi_dp_sel)
        lib_hdmi_set_hdmi_dp_sel(hdmi_dp_sel);          // change hdmi dp sel

    return hdmi_dp_sel;
}

extern unsigned char lib_hdmi_dsc_get_hdmi_dsc_enable(void);

#if IS_ENABLED(CONFIG_SUPPORT_SCALER)
#define _dp_is_dispsrc()        (GET_DISPLAY_MODE_SRC(SLR_MAIN_DISPLAY)==VSC_INPUTSRC_DP)
#else
#define _dp_is_dispsrc()        (0)
#endif

//--------------------------------------------------
// Func  : newbase_hdmi_get_hdmi_vpll_ownership
//
// Desc  : check hdmi vpll ownership
//         for RTD2885P, the VPLL will be used by
//         DPRX DSCD.
//         if DPRX DSCD is running, HDMI FW should
//         not operate VPLL
//
// Param : N/A
//
// Retn  : 0 : hdmi vpll is not ready for HDMI
//         1 : hdmi vpll is ready for HDMI
//--------------------------------------------------
unsigned char newbase_hdmi_get_hdmi_vpll_ownership(void)
{
    if (_is_hdmi_dp_combo_platform()==0)  // no hdmi/dp combo port (DEMETER)
        return TRUE;

    // check VPLL is occupied by DPRX or not
    if (_dp_is_dispsrc() && lib_hdmi_dsc_get_crt_on()==1 && lib_hdmi_dsc_get_hdmi_dsc_enable()==0)  // HDMI is occupied by DPRX
    {
        HDMI_INFO("newbase_hdmi_get_hdmi_vpll_ownership=0, VPLL occupied DP DSCD (hdmi_dp_sel=%d, dsc_crt_on=%d, hdmi_dsc_enable=%d)\n",
            _dp_is_dispsrc(), lib_hdmi_dsc_get_crt_on(), lib_hdmi_dsc_get_hdmi_dsc_enable());
        return FALSE;
    }

    return TRUE;
}

#endif // CONFIG_RTK_KDRV_HDMI_DYNAMIC_HDMI_DP_SEL_SWITCH

//--------------------------------------------------
// Func  : newbase_hdmi_get_common_420_selection
//
// Desc  : check common 420 ownership
//
// Param : N/A
//
// Retn  : 0 : NOT select HDMI
//         1 : select HDMI
//--------------------------------------------------
unsigned char newbase_hdmi_get_common_420_selection(void)
{
    unsigned char yuv420_in_select = 0;

    if (_is_hdmi_dp_combo_platform()==0)  // no hdmi/dp combo port (DEMETER)
        return TRUE;

    yuv420_in_select = newbase_hdmi_hd21_get_yuv420_in_sel();

    if (!(yuv420_in_select == 0 // HDMI
        || yuv420_in_select == 2)) // HDMI + DSCD
    {
        HDMI_WARN("[newbase_hdmi_get_common_420_selection] yuv420_in_sel =%d\n",  yuv420_in_select);
        return FALSE;
    }
    return TRUE;
}

