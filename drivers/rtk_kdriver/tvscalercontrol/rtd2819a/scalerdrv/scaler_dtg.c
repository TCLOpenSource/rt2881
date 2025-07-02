#include <tvscalercontrol/io/ioregdrv.h>
#include <scaler/panelCommon.h>
#include <tvscalercontrol/scalerdrv/scaler_dtg.h>
#include <rtd_log/rtd_module_log.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/pll27x_reg_reg.h>
#include <rbus/timer_reg.h>
#include <rbus/vgip_reg.h>
#include <rbus/pll_reg_reg.h>
#ifdef BUILD_QUICK_SHOW
#include <string.h>
#else
#include <linux/slab.h>
#endif

unsigned char *scaler_dtg_get_dtg_type_str(DTG_TYPE type)
{
    switch(type)
    {
        case DTG_MASTER:
            return "DTG_MASTER";

        case DTG_MASTER2:
            return "DTG_MASTER2";

        case DTG_UZU:
            return "DTG_UZU";

        case DTG_D2PRE:
            return "DTG_D2PRE";

        case DTG_OSD123:
            return "DTG_OSD123";

        case DTG_OSD4:
            return "DTG_OSD4";

        case DTG_OSD5:
            return "DTG_OSD5";

        case DTG_D2POST:
            return "DTG_D2POST";

        case DTG_D2POST2:
            return "DTG_D2POST2";

        default:
            return "Unknow";
    }
}

DTG_APP_TYPE dtg_app_type = DTG_DISPLAY_MONITOR;

DTG_APP_TYPE scaler_dtg_get_app_type(void)
{
    return dtg_app_type;
}

void scaler_dtg_set_app_type(DTG_APP_TYPE app_type)
{
    DTG_PRINT("[%s %d]app_type:%d\n", __FUNCTION__, __LINE__, app_type);
    dtg_app_type = app_type;
}

static int scaler_dtg_master_handler_fraction(DTG_HANDLER handler, DTG_FRACTION_INFO *fraction)
{
    ppoverlay_dtg_m_remove_input_vsync_RBUS dtg_m_remove_input_vsync_reg;
    ppoverlay_dtg_m_multiple_vsync_RBUS dtg_m_multiple_vsync_reg;
    int ret = 0;

    if(fraction == NULL)
    {
        DTG_PRINT("[%s %d][Error] fraction parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    dtg_m_remove_input_vsync_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_M_Remove_input_vsync_reg);
    dtg_m_multiple_vsync_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_M_multiple_vsync_reg);

    if(handler == DTG_SET)
    {
        dtg_m_remove_input_vsync_reg.remove_half_ivs = fraction->remove_half_ivs;
        dtg_m_remove_input_vsync_reg.remove_half_ivs_mode2 = fraction->rm;
        IoReg_Write32(PPOVERLAY_DTG_M_Remove_input_vsync_reg, dtg_m_remove_input_vsync_reg.regValue);

        dtg_m_multiple_vsync_reg.dtg_m_multiple_vsync = fraction->mul;
        IoReg_Write32(PPOVERLAY_DTG_M_multiple_vsync_reg, dtg_m_multiple_vsync_reg.regValue);

        DTG_PRINT("[%s %d]handler:%s rm:%d, mul:%d\n", __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), fraction->rm, fraction->mul);

    }
    else if(handler == DTG_GET)
    {
        fraction->remove_half_ivs = dtg_m_remove_input_vsync_reg.remove_half_ivs;
        fraction->rm = dtg_m_remove_input_vsync_reg.remove_half_ivs_mode2;
        fraction->mul = dtg_m_multiple_vsync_reg.dtg_m_multiple_vsync;
    }

    return ret;
}

static int scaler_dtg_master2_handler_fraction(DTG_HANDLER handler, DTG_FRACTION_INFO *fraction)
{
    int ret = 0;

    //master2 dtg rm/mul always set 1:1, not sw consider

    return ret;
}

static int scaler_dtg_uzu_handler_fraction(DTG_HANDLER handler, DTG_FRACTION_INFO *fraction)
{
    ppoverlay_uzudtg_fractional_fsync_RBUS uzudtg_fractional_fsync_reg;

    int ret = 0;

    if(fraction == NULL)
    {
        DTG_PRINT("[%s %d][Error] fraction parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    uzudtg_fractional_fsync_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_fractional_fsync_reg);

    if(handler == DTG_SET)
    {
        uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs = fraction->remove_half_ivs;
        uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs_alignmode = fraction->remove_half_ivs_alignmode;
        uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs_mode = fraction->rm;
        uzudtg_fractional_fsync_reg.uzudtg_multiple_vsync = fraction->mul;
        IoReg_Write32(PPOVERLAY_uzudtg_fractional_fsync_reg, uzudtg_fractional_fsync_reg.regValue);

        DTG_PRINT("[%s %d]handler:%s, rm:%d, mul:%d\n", __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), fraction->rm, fraction->mul);
    }
    else if(handler == DTG_GET)
    {
        fraction->remove_half_ivs = uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs;
        fraction->remove_half_ivs_alignmode = uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs_alignmode;
        fraction->rm = uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs_mode;
        fraction->mul = uzudtg_fractional_fsync_reg.uzudtg_multiple_vsync;
    }

    return ret;
}

static int scaler_dtg_d2pre_handler_fraction(DTG_HANDLER handler, DTG_FRACTION_INFO *fraction)
{
    int ret = 0;

    //master2 dtg rm/mul always herit uzudtg
    DTG_PRINT("[%s %d]d2pre aways herit uzu\n", __FUNCTION__, __LINE__);

    return ret;
}

static int scaler_dtg_osd123_handler_fraction(DTG_HANDLER handler, DTG_FRACTION_INFO *fraction)
{
    ppoverlay_osddtg_fractional_fsync_RBUS ppoverlay_osddtg_fractional_fsync_reg;

    int ret = 0;

    if(fraction == NULL)
    {
        DTG_PRINT("[%s %d][Error] fraction parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_osddtg_fractional_fsync_reg.regValue = IoReg_Read32(PPOVERLAY_osddtg_fractional_fsync_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_osddtg_fractional_fsync_reg.osddtg_remove_half_ivs_mode = fraction->rm;
        ppoverlay_osddtg_fractional_fsync_reg.osddtg_multiple_vsync  = fraction->mul;
        ppoverlay_osddtg_fractional_fsync_reg.osddtg_remove_multiple_inherit = fraction->inherit;
        ppoverlay_osddtg_fractional_fsync_reg.osddtg_remove_half_ivs = fraction->remove_half_ivs;
        ppoverlay_osddtg_fractional_fsync_reg.osddtg_remove_half_ivs_alignmode = fraction->remove_half_ivs_alignmode;

        IoReg_Write32(PPOVERLAY_osddtg_fractional_fsync_reg, ppoverlay_osddtg_fractional_fsync_reg.regValue);

        DTG_PRINT("[%s %d]handler:%s, rm:%d, mul:%d, fraction->inherit:%d\n", __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), fraction->rm, fraction->mul, fraction->inherit);

    }
    else if(handler == DTG_GET)
    {
        fraction->rm = ppoverlay_osddtg_fractional_fsync_reg.osddtg_remove_half_ivs_mode;
        fraction->mul = ppoverlay_osddtg_fractional_fsync_reg.osddtg_multiple_vsync;
        fraction->inherit = ppoverlay_osddtg_fractional_fsync_reg.osddtg_remove_multiple_inherit;
        fraction->remove_half_ivs = ppoverlay_osddtg_fractional_fsync_reg.osddtg_remove_half_ivs;
        fraction->remove_half_ivs_alignmode = ppoverlay_osddtg_fractional_fsync_reg.osddtg_remove_half_ivs_alignmode;
    }

    return ret;

}
static int scaler_dtg_osd4_handler_fraction(DTG_HANDLER handler, DTG_FRACTION_INFO *fraction)
{
    ppoverlay_osd4dtg_fractional_fsync_RBUS ppoverlay_osd4dtg_fractional_fsync_reg;

    int ret = 0;

    if(fraction == NULL)
    {
        DTG_PRINT("[%s %d][Error] fraction parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_osd4dtg_fractional_fsync_reg.regValue = IoReg_Read32(PPOVERLAY_osd4dtg_fractional_fsync_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_osd4dtg_fractional_fsync_reg.osd4dtg_remove_half_ivs_mode = fraction->rm;
        ppoverlay_osd4dtg_fractional_fsync_reg.osd4dtg_multiple_vsync  = fraction->mul;
        ppoverlay_osd4dtg_fractional_fsync_reg.osd4dtg_remove_multiple_inherit = fraction->inherit;
        ppoverlay_osd4dtg_fractional_fsync_reg.osd4dtg_remove_half_ivs = fraction->remove_half_ivs;
        ppoverlay_osd4dtg_fractional_fsync_reg.osd4dtg_remove_half_ivs_alignmode = fraction->remove_half_ivs_alignmode;

        IoReg_Write32(PPOVERLAY_osd4dtg_fractional_fsync_reg, ppoverlay_osd4dtg_fractional_fsync_reg.regValue);

        DTG_PRINT("[%s %d]handler:%s, rm:%d, mul:%d, fraction->inherit:%d\n", __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), fraction->rm, fraction->mul, fraction->inherit);

    }
    else if(handler == DTG_GET)
    {
        fraction->rm = ppoverlay_osd4dtg_fractional_fsync_reg.osd4dtg_remove_half_ivs_mode;
        fraction->mul = ppoverlay_osd4dtg_fractional_fsync_reg.osd4dtg_multiple_vsync;
        fraction->inherit = ppoverlay_osd4dtg_fractional_fsync_reg.osd4dtg_remove_multiple_inherit;
        fraction->remove_half_ivs = ppoverlay_osd4dtg_fractional_fsync_reg.osd4dtg_remove_half_ivs;
        fraction->remove_half_ivs_alignmode = ppoverlay_osd4dtg_fractional_fsync_reg.osd4dtg_remove_half_ivs_alignmode;
    }

    return ret;
}
static int scaler_dtg_osd5_handler_fraction(DTG_HANDLER handler, DTG_FRACTION_INFO *fraction)
{
    ppoverlay_osd5dtg_fractional_fsync_RBUS ppoverlay_osd5dtg_fractional_fsync_reg;

    int ret = 0;

    if(fraction == NULL)
    {
        DTG_PRINT("[%s %d][Error] fraction parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_osd5dtg_fractional_fsync_reg.regValue = IoReg_Read32(PPOVERLAY_osd5dtg_fractional_fsync_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_osd5dtg_fractional_fsync_reg.osd5dtg_remove_half_ivs_mode = fraction->rm;
        ppoverlay_osd5dtg_fractional_fsync_reg.osd5dtg_multiple_vsync  = fraction->mul;
        ppoverlay_osd5dtg_fractional_fsync_reg.osd5dtg_remove_multiple_inherit = fraction->inherit;
        ppoverlay_osd5dtg_fractional_fsync_reg.osd5dtg_remove_half_ivs = fraction->remove_half_ivs;
        ppoverlay_osd5dtg_fractional_fsync_reg.osd5dtg_remove_half_ivs_alignmode = fraction->remove_half_ivs_alignmode;

        IoReg_Write32(PPOVERLAY_osd5dtg_fractional_fsync_reg, ppoverlay_osd5dtg_fractional_fsync_reg.regValue);

        DTG_PRINT("[%s %d]handler:%s, rm:%d, mul:%d, fraction->inherit:%d\n", __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), fraction->rm, fraction->mul, fraction->inherit);

    }
    else if(handler == DTG_GET)
    {
        fraction->rm = ppoverlay_osd5dtg_fractional_fsync_reg.osd5dtg_remove_half_ivs_mode;
        fraction->mul = ppoverlay_osd5dtg_fractional_fsync_reg.osd5dtg_multiple_vsync;
        fraction->inherit = ppoverlay_osd5dtg_fractional_fsync_reg.osd5dtg_remove_multiple_inherit;
        fraction->remove_half_ivs = ppoverlay_osd5dtg_fractional_fsync_reg.osd5dtg_remove_half_ivs;
        fraction->remove_half_ivs_alignmode = ppoverlay_osd5dtg_fractional_fsync_reg.osd5dtg_remove_half_ivs_alignmode;
    }

    return ret;
}
static int scaler_dtg_d2post_handler_fraction(DTG_HANDLER handler, DTG_FRACTION_INFO *fraction)
{
    int ret = 0;

    ppoverlay_memcdtg_fractional_fsync_RBUS ppoverlay_memcdtg_fractional_fsync_reg;

    if(fraction == NULL)
    {
        DTG_PRINT("[%s %d][Error] fraction parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_memcdtg_fractional_fsync_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_fractional_fsync_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_memcdtg_fractional_fsync_reg.memcdtg_remove_half_ivs_mode = fraction->rm;
        ppoverlay_memcdtg_fractional_fsync_reg.memcdtg_multiple_vsync  = fraction->mul;
        ppoverlay_memcdtg_fractional_fsync_reg.memcdtg_remove_multiple_inherit = fraction->inherit;
        ppoverlay_memcdtg_fractional_fsync_reg.memcdtg_remove_half_ivs = fraction->remove_half_ivs;
        ppoverlay_memcdtg_fractional_fsync_reg.memcdtg_remove_half_ivs_alignmode = fraction->remove_half_ivs_alignmode;

        IoReg_Write32(PPOVERLAY_memcdtg_fractional_fsync_reg, ppoverlay_memcdtg_fractional_fsync_reg.regValue);

        DTG_PRINT("[%s %d]handler:%s, rm:%d, mul:%d, fraction->inherit:%d\n", __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), fraction->rm, fraction->mul, fraction->inherit);

    }
    else if(handler == DTG_GET)
    {
        fraction->rm = ppoverlay_memcdtg_fractional_fsync_reg.memcdtg_remove_half_ivs_mode;
        fraction->mul = ppoverlay_memcdtg_fractional_fsync_reg.memcdtg_multiple_vsync;
        fraction->inherit = ppoverlay_memcdtg_fractional_fsync_reg.memcdtg_remove_multiple_inherit;
        fraction->remove_half_ivs = ppoverlay_memcdtg_fractional_fsync_reg.memcdtg_remove_half_ivs;
        fraction->remove_half_ivs_alignmode = ppoverlay_memcdtg_fractional_fsync_reg.memcdtg_remove_half_ivs_alignmode;
    }

    return ret;
}

//always herit d2post
static int scaler_dtg_d2post2_handler_fraction(DTG_HANDLER handler, DTG_FRACTION_INFO *fraction)
{
    ppoverlay_d2post2dtg_fractional_fsync_RBUS ppoverlay_d2post2dtg_fractional_fsync_reg;

    int ret = 0;

    if(fraction == NULL)
    {
        DTG_PRINT("[%s %d][Error] fraction parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_d2post2dtg_fractional_fsync_reg.regValue = IoReg_Read32(PPOVERLAY_d2post2dtg_fractional_fsync_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_d2post2dtg_fractional_fsync_reg.d2post2dtg_remove_multiple_inherit = fraction->inherit;
        IoReg_Write32(PPOVERLAY_d2post2dtg_fractional_fsync_reg, ppoverlay_d2post2dtg_fractional_fsync_reg.regValue);

         DTG_PRINT("[%s %d]handler:%s, fraction->inherit:%d\n", __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), fraction->inherit);

    }
    else if(handler == DTG_GET)
    {
        fraction->inherit = ppoverlay_d2post2dtg_fractional_fsync_reg.d2post2dtg_remove_multiple_inherit;
    }

    return ret;
}

//timing
static int scaler_dtg_master_handler_timing(DTG_HANDLER handler, DTG_TIMING_INFO *timing)
{
    int ret = 0;
    ppoverlay_dv_total_RBUS ppoverlay_dv_total_reg;
    ppoverlay_dh_total_last_line_length_RBUS ppoverlay_dh_total_last_line_length_reg;
    ppoverlay_dh_width_RBUS ppoverlay_dh_width_reg;
    ppoverlay_dv_length_RBUS ppoverlay_dv_length_reg;

    if(timing == NULL)
    {
        DTG_PRINT("[%s %d][Error] timing parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_DV_total_reg);
    ppoverlay_dh_width_reg.regValue = IoReg_Read32(PPOVERLAY_DH_Width_reg);
    ppoverlay_dv_length_reg.regValue = IoReg_Read32(PPOVERLAY_DV_Length_reg);
    ppoverlay_dh_total_last_line_length_reg.regValue = IoReg_Read32(PPOVERLAY_DH_Total_Last_Line_Length_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_dv_total_reg.dv_total = timing->vtotal;
        IoReg_Write32(PPOVERLAY_DV_total_reg, ppoverlay_dv_total_reg.regValue);

        ppoverlay_dh_total_last_line_length_reg.dh_total = timing->htotal;
        ppoverlay_dh_total_last_line_length_reg.dh_total_last_line = timing->htotal_last_line;
        IoReg_Write32(PPOVERLAY_DH_Total_Last_Line_Length_reg, ppoverlay_dh_total_last_line_length_reg.regValue);

        ppoverlay_dh_width_reg.dh_width = timing->dh_width;
        IoReg_Write32(PPOVERLAY_DH_Width_reg, ppoverlay_dh_width_reg.regValue);

        ppoverlay_dv_length_reg.dv_length = timing->dv_length;
        IoReg_Write32(PPOVERLAY_DV_Length_reg, ppoverlay_dv_length_reg.regValue);
    }
    else if(handler == DTG_GET)
    {
        timing->vtotal = ppoverlay_dv_total_reg.dv_total;
        timing->htotal = ppoverlay_dh_total_last_line_length_reg.dh_total;
        timing->htotal_last_line = ppoverlay_dh_total_last_line_length_reg.dh_total_last_line;
        timing->dh_width = ppoverlay_dh_width_reg.dh_width;
        timing->dv_length = ppoverlay_dv_length_reg.dv_length;
    }

    return ret;

}

static int scaler_dtg_master2_handler_timing(DTG_HANDLER handler, DTG_TIMING_INFO *timing)
{
    int ret = 0;

    ppoverlay_disp2_dv_total_RBUS ppoverlay_disp2_dv_total_reg;
    ppoverlay_disp2_dh_total_last_line_length_RBUS ppoverlay_disp2_dh_total_last_line_length_reg;
    ppoverlay_disp2_dh_width_RBUS ppoverlay_disp2_dh_width_reg;
    ppoverlay_disp2_dv_length_RBUS ppoverlay_disp2_dv_length_reg;

    if(timing == NULL)
    {
        DTG_PRINT("[%s %d][Error] timing parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_disp2_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_Disp2_DV_total_reg);
    ppoverlay_disp2_dh_total_last_line_length_reg.regValue = IoReg_Read32(PPOVERLAY_DISP2_DH_Total_Last_Line_Length_reg);
    ppoverlay_disp2_dh_width_reg.regValue = IoReg_Read32(PPOVERLAY_DISP2_DH_Width_reg);
    ppoverlay_disp2_dv_length_reg.regValue = IoReg_Read32(PPOVERLAY_DISP2_DV_Length_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_disp2_dv_total_reg.disp2_dv_total = timing->vtotal;
        IoReg_Write32(PPOVERLAY_Disp2_DV_total_reg, ppoverlay_disp2_dv_total_reg.regValue);

        ppoverlay_disp2_dh_total_last_line_length_reg.disp2_dh_total = timing->htotal;
        ppoverlay_disp2_dh_total_last_line_length_reg.disp2_dh_total_last_line = timing->htotal_last_line;
        IoReg_Write32(PPOVERLAY_DISP2_DH_Total_Last_Line_Length_reg, ppoverlay_disp2_dh_total_last_line_length_reg.regValue);

        ppoverlay_disp2_dh_width_reg.disp2_dh_width = timing->dh_width;
        IoReg_Write32(PPOVERLAY_DISP2_DH_Width_reg, ppoverlay_disp2_dh_width_reg.regValue);

        ppoverlay_disp2_dv_length_reg.disp2_dv_length = timing->dv_length;
        IoReg_Write32(PPOVERLAY_DISP2_DV_Length_reg, ppoverlay_disp2_dv_length_reg.regValue);
    }
    else if(handler == DTG_GET)
    {
        timing->vtotal = ppoverlay_disp2_dv_total_reg.disp2_dv_total;
        timing->htotal_last_line = ppoverlay_disp2_dh_total_last_line_length_reg.disp2_dh_total_last_line;
        timing->htotal = ppoverlay_disp2_dh_total_last_line_length_reg.disp2_dh_total;
        timing->dh_width = ppoverlay_disp2_dh_width_reg.disp2_dh_width;
        timing->dv_length = ppoverlay_disp2_dv_length_reg.disp2_dv_length;
    }

    return ret;
}

static int scaler_dtg_uzu_handler_timing(DTG_HANDLER handler, DTG_TIMING_INFO *timing)
{
    int ret = 0;
    ppoverlay_uzudtg_dv_total_RBUS uzudtg_dv_total_reg;
    ppoverlay_uzudtg_dh_total_RBUS uzudtg_dh_total_reg;
    ppoverlay_dh_den_start_end_RBUS dh_den_start_end_reg;
    ppoverlay_dv_den_start_end_RBUS dv_den_start_end_reg;

    if(timing == NULL)
    {
        DTG_PRINT("[%s %d][Error] timing parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    uzudtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DH_TOTAL_reg);
    uzudtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DV_TOTAL_reg);
    dh_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_DH_DEN_Start_End_reg);
    dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_DV_DEN_Start_End_reg);

    if(handler == DTG_SET)
    {
        uzudtg_dh_total_reg.uzudtg_dh_total = timing->htotal;
        uzudtg_dh_total_reg.uzudtg_dh_total_last_line = timing->htotal_last_line;
        IoReg_Write32(PPOVERLAY_uzudtg_DH_TOTAL_reg, uzudtg_dh_total_reg.regValue);

        uzudtg_dv_total_reg.uzudtg_dv_total = timing->vtotal;
        IoReg_Write32(PPOVERLAY_uzudtg_DV_TOTAL_reg, uzudtg_dv_total_reg.regValue);

        dh_den_start_end_reg.dh_den_sta = timing->hstart;
        dh_den_start_end_reg.dh_den_end = timing->hend;
        IoReg_Write32(PPOVERLAY_DH_DEN_Start_End_reg, dh_den_start_end_reg.regValue);

        dv_den_start_end_reg.dv_den_sta = timing->vstart;
        dv_den_start_end_reg.dv_den_end = timing->vend;
        IoReg_Write32(PPOVERLAY_DV_DEN_Start_End_reg, dv_den_start_end_reg.regValue);

        DTG_PRINT("[%s %d]handler:%s, hstart:%d, hend:%d, vstart:%d ,vend:%d, htotal:%d, htotal_last_line:%d, vtotal:%d\n",
        __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), timing->hstart, timing->hend, timing->vstart, timing->vend,
            timing->htotal, timing->htotal_last_line, timing->vtotal);
    }
    else if(handler == DTG_GET)
    {
        timing->htotal = uzudtg_dh_total_reg.uzudtg_dh_total;
        timing->htotal_last_line = uzudtg_dh_total_reg.uzudtg_dh_total_last_line;
        timing->vtotal = uzudtg_dv_total_reg.uzudtg_dv_total;
        timing->hstart = dh_den_start_end_reg.dh_den_sta;
        timing->hend = dh_den_start_end_reg.dh_den_end;
        timing->vstart = dv_den_start_end_reg.dv_den_sta;
        timing->vend = dv_den_start_end_reg.dv_den_end;
    }

    return ret;
}

static int scaler_dtg_d2pre_handler_timing(DTG_HANDLER handler, DTG_TIMING_INFO *timing)
{
    int ret = 0;

    ppoverlay_d2predtg_dv_total_RBUS ppoverlay_d2predtg_dv_total_reg;
    ppoverlay_d2predtg_dh_total_RBUS ppoverlay_d2predtg_dh_total_reg;
    ppoverlay_d2predtg_dh_den_start_end_RBUS ppoverlay_d2predtg_dh_den_start_end_reg;
    ppoverlay_d2predtg_dv_den_start_end_RBUS ppoverlay_d2predtg_dv_den_start_end_reg;

    if(timing == NULL)
    {
        DTG_PRINT("[%s %d][Error] timing parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_d2predtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_D2PREDTG_DH_TOTAL_reg);
    ppoverlay_d2predtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_D2PREDTG_DV_TOTAL_reg);
    ppoverlay_d2predtg_dh_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_D2PREDTG_DH_DEN_Start_End_reg);
    ppoverlay_d2predtg_dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_D2PREDTG_DV_DEN_Start_End_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_d2predtg_dh_total_reg.d2predtg_dh_total = timing->htotal;
        ppoverlay_d2predtg_dh_total_reg.d2predtg_dh_total_last_line = timing->htotal_last_line;
        IoReg_Write32(PPOVERLAY_D2PREDTG_DH_TOTAL_reg, ppoverlay_d2predtg_dh_total_reg.regValue);

        ppoverlay_d2predtg_dv_total_reg.d2predtg_dv_total = timing->vtotal;
        IoReg_Write32(PPOVERLAY_D2PREDTG_DV_TOTAL_reg, ppoverlay_d2predtg_dv_total_reg.regValue);

        ppoverlay_d2predtg_dh_den_start_end_reg.d2predtg_dh_den_sta = timing->hstart;
        ppoverlay_d2predtg_dh_den_start_end_reg.d2predtg_dh_den_end = timing->hend;
        IoReg_Write32(PPOVERLAY_D2PREDTG_DH_DEN_Start_End_reg, ppoverlay_d2predtg_dh_den_start_end_reg.regValue);

        ppoverlay_d2predtg_dv_den_start_end_reg.d2predtg_dv_den_sta = timing->vstart;
        ppoverlay_d2predtg_dv_den_start_end_reg.d2predtg_dv_den_end = timing->vend;
        IoReg_Write32(PPOVERLAY_D2PREDTG_DV_DEN_Start_End_reg, ppoverlay_d2predtg_dv_den_start_end_reg.regValue);

        DTG_PRINT("[%s %d]%s hstart:%d, hend:%d, vstart:%d ,vend:%d, htotal:%d, htotal_last_line:%d, vtotal:%d\n",
        __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), timing->hstart, timing->hend, timing->vstart, timing->vend,
            timing->htotal, timing->htotal_last_line, timing->vtotal);
    }
    else if(handler == DTG_GET)
    {
        timing->htotal = ppoverlay_d2predtg_dh_total_reg.d2predtg_dh_total;
        timing->htotal_last_line = ppoverlay_d2predtg_dh_total_reg.d2predtg_dh_total_last_line;
        timing->vtotal = ppoverlay_d2predtg_dv_total_reg.d2predtg_dv_total;
        timing->hstart = ppoverlay_d2predtg_dh_den_start_end_reg.d2predtg_dh_den_sta;
        timing->hend = ppoverlay_d2predtg_dh_den_start_end_reg.d2predtg_dh_den_end;
        timing->vstart = ppoverlay_d2predtg_dv_den_start_end_reg.d2predtg_dv_den_sta;
        timing->vend = ppoverlay_d2predtg_dv_den_start_end_reg.d2predtg_dv_den_end;
    }

    return ret;
}

static int scaler_dtg_osd123_handler_timing(DTG_HANDLER handler, DTG_TIMING_INFO *timing)
{
    int ret = 0;

    ppoverlay_osd_dtg_dh_width_RBUS ppoverlay_osd_dtg_dh_width_reg;
    ppoverlay_osd_dtg_dv_length_RBUS ppoverlay_osd_dtg_dv_length_reg;
    ppoverlay_osddtg_dv_total_RBUS ppoverlay_osddtg_dv_total_reg;
    ppoverlay_osddtg_dh_total_RBUS ppoverlay_osddtg_dh_total_reg;
    ppoverlay_osddtg_dh_den_start_end_RBUS ppoverlay_osddtg_dh_den_start_end_reg;
    ppoverlay_osddtg_dv_den_start_end_RBUS ppoverlay_osddtg_dv_den_start_end_reg;

    if(timing == NULL)
    {
        DTG_PRINT("[%s %d][Error] timing parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_osd_dtg_dh_width_reg.regValue = IoReg_Read32(PPOVERLAY_osd_dtg_dh_width_reg);
    ppoverlay_osd_dtg_dv_length_reg.regValue = IoReg_Read32(PPOVERLAY_osd_dtg_dv_length_reg);
    ppoverlay_osddtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_osddtg_DH_TOTAL_reg);
    ppoverlay_osddtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_osddtg_DV_TOTAL_reg);
    ppoverlay_osddtg_dh_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_osddtg_DH_DEN_Start_End_reg);
    ppoverlay_osddtg_dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_osddtg_DV_DEN_Start_End_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_osd_dtg_dh_width_reg.pif_osd_dh_width = timing->dh_width;
        IoReg_Write32(PPOVERLAY_osd_dtg_dh_width_reg, ppoverlay_osd_dtg_dh_width_reg.regValue);

        ppoverlay_osd_dtg_dv_length_reg.pif_osd_dv_length = timing->dv_length;
        IoReg_Write32(PPOVERLAY_osd_dtg_dv_length_reg, ppoverlay_osd_dtg_dv_length_reg.regValue);

        ppoverlay_osddtg_dh_total_reg.osddtg_dh_total = timing->htotal;
        ppoverlay_osddtg_dh_total_reg.osddtg_dh_total_last_line = timing->htotal_last_line;
        IoReg_Write32(PPOVERLAY_osddtg_DH_TOTAL_reg, ppoverlay_osddtg_dh_total_reg.regValue);

        ppoverlay_osddtg_dv_total_reg.osddtg_dv_total = timing->vtotal;
        IoReg_Write32(PPOVERLAY_osddtg_DV_TOTAL_reg, ppoverlay_osddtg_dv_total_reg.regValue);

        ppoverlay_osddtg_dh_den_start_end_reg.osddtg_dh_den_sta = timing->hstart;
        ppoverlay_osddtg_dh_den_start_end_reg.osddtg_dh_den_end = timing->hend;
        IoReg_Write32(PPOVERLAY_osddtg_DH_DEN_Start_End_reg, ppoverlay_osddtg_dh_den_start_end_reg.regValue);

        ppoverlay_osddtg_dv_den_start_end_reg.osddtg_dv_den_sta = timing->vstart;
        ppoverlay_osddtg_dv_den_start_end_reg.osddtg_dv_den_end = timing->vend;
        IoReg_Write32(PPOVERLAY_osddtg_DV_DEN_Start_End_reg, ppoverlay_osddtg_dv_den_start_end_reg.regValue);

        DTG_PRINT("[%s %d]%s hstart:%d, hend:%d, vstart:%d ,vend:%d, htotal:%d, htotal_last_line:%d, vtotal:%d\n",
        __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), timing->hstart, timing->hend, timing->vstart, timing->vend,
            timing->htotal, timing->htotal_last_line, timing->vtotal);
    }
    else if(handler == DTG_GET)
    {
        timing->htotal = ppoverlay_osddtg_dh_total_reg.osddtg_dh_total;
        timing->htotal_last_line = ppoverlay_osddtg_dh_total_reg.osddtg_dh_total_last_line;
        timing->vtotal = ppoverlay_osddtg_dv_total_reg.osddtg_dv_total;
        timing->hstart = ppoverlay_osddtg_dh_den_start_end_reg.osddtg_dh_den_sta;
        timing->hend = ppoverlay_osddtg_dh_den_start_end_reg.osddtg_dh_den_end;
        timing->vstart = ppoverlay_osddtg_dv_den_start_end_reg.osddtg_dv_den_sta;
        timing->vend = ppoverlay_osddtg_dv_den_start_end_reg.osddtg_dv_den_end;
    }

    return ret;

}
static int scaler_dtg_osd4_handler_timing(DTG_HANDLER handler, DTG_TIMING_INFO *timing)
{
    int ret = 0;

    ppoverlay_osd4dtg_dv_total_RBUS ppoverlay_osd4dtg_dv_total_reg;
    ppoverlay_osd4dtg_dh_total_RBUS ppoverlay_osd4dtg_dh_total_reg;
    ppoverlay_osd4dtg_dh_den_start_end_RBUS ppoverlay_osd4dtg_dh_den_start_end_reg;
    ppoverlay_osd4dtg_dv_den_start_end_RBUS ppoverlay_osd4dtg_dv_den_start_end_reg;

    if(timing == NULL)
    {
        DTG_PRINT("[%s %d][Error] timing parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_osd4dtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_osd4dtg_DH_TOTAL_reg);
    ppoverlay_osd4dtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_osd4dtg_DV_TOTAL_reg);
    ppoverlay_osd4dtg_dh_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_osd4dtg_DH_DEN_Start_End_reg);
    ppoverlay_osd4dtg_dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_osd4dtg_DV_DEN_Start_End_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_osd4dtg_dh_total_reg.osd4dtg_dh_total = timing->htotal;
        ppoverlay_osd4dtg_dh_total_reg.osd4dtg_dh_total_last_line = timing->htotal_last_line;
        IoReg_Write32(PPOVERLAY_osd4dtg_DH_TOTAL_reg, ppoverlay_osd4dtg_dh_total_reg.regValue);

        ppoverlay_osd4dtg_dv_total_reg.osd4dtg_dv_total = timing->vtotal;
        IoReg_Write32(PPOVERLAY_osd4dtg_DV_TOTAL_reg, ppoverlay_osd4dtg_dv_total_reg.regValue);

        ppoverlay_osd4dtg_dh_den_start_end_reg.osd4dtg_dh_den_sta = timing->hstart;
        ppoverlay_osd4dtg_dh_den_start_end_reg.osd4dtg_dh_den_end = timing->hend;
        IoReg_Write32(PPOVERLAY_osd4dtg_DH_DEN_Start_End_reg, ppoverlay_osd4dtg_dh_den_start_end_reg.regValue);

        ppoverlay_osd4dtg_dv_den_start_end_reg.osd4dtg_dv_den_sta = timing->vstart;
        ppoverlay_osd4dtg_dv_den_start_end_reg.osd4dtg_dv_den_end = timing->vend;
        IoReg_Write32(PPOVERLAY_osd4dtg_DV_DEN_Start_End_reg, ppoverlay_osd4dtg_dv_den_start_end_reg.regValue);

        DTG_PRINT("[%s %d]%s hstart:%d, hend:%d, vstart:%d ,vend:%d, htotal:%d, htotal_last_line:%d, vtotal:%d\n",
        __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), timing->hstart, timing->hend, timing->vstart, timing->vend,
            timing->htotal, timing->htotal_last_line, timing->vtotal);
    }
    else if(handler == DTG_GET)
    {
        timing->htotal = ppoverlay_osd4dtg_dh_total_reg.osd4dtg_dh_total;
        timing->htotal_last_line = ppoverlay_osd4dtg_dh_total_reg.osd4dtg_dh_total_last_line;
        timing->vtotal = ppoverlay_osd4dtg_dv_total_reg.osd4dtg_dv_total;
        timing->hstart = ppoverlay_osd4dtg_dh_den_start_end_reg.osd4dtg_dh_den_sta;
        timing->hend = ppoverlay_osd4dtg_dh_den_start_end_reg.osd4dtg_dh_den_end;
        timing->vstart = ppoverlay_osd4dtg_dv_den_start_end_reg.osd4dtg_dv_den_sta;
        timing->vend = ppoverlay_osd4dtg_dv_den_start_end_reg.osd4dtg_dv_den_end;
    }

    return ret;
}
static int scaler_dtg_osd5_handler_timing(DTG_HANDLER handler, DTG_TIMING_INFO *timing)
{
    int ret = 0;
    ppoverlay_osd5dtg_dv_total_RBUS ppoverlay_osd5dtg_dv_total_reg;
    ppoverlay_osd5dtg_dh_total_RBUS ppoverlay_osd5dtg_dh_total_reg;
    ppoverlay_osd5dtg_dh_den_start_end_RBUS ppoverlay_osd5dtg_dh_den_start_end_reg;
    ppoverlay_osd5dtg_dv_den_start_end_RBUS ppoverlay_osd5dtg_dv_den_start_end_reg;

    if(timing == NULL)
    {
        DTG_PRINT("[%s %d][Error] timing parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_osd5dtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_osd5dtg_DH_TOTAL_reg);
    ppoverlay_osd5dtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_osd5dtg_DV_TOTAL_reg);
    ppoverlay_osd5dtg_dh_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_osd5dtg_DH_DEN_Start_End_reg);
    ppoverlay_osd5dtg_dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_osd5dtg_DV_DEN_Start_End_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_osd5dtg_dh_total_reg.osd5dtg_dh_total = timing->htotal;
        ppoverlay_osd5dtg_dh_total_reg.osd5dtg_dh_total_last_line = timing->htotal_last_line;
        IoReg_Write32(PPOVERLAY_osd5dtg_DH_TOTAL_reg, ppoverlay_osd5dtg_dh_total_reg.regValue);

        ppoverlay_osd5dtg_dv_total_reg.osd5dtg_dv_total = timing->vtotal;
        IoReg_Write32(PPOVERLAY_osd5dtg_DV_TOTAL_reg, ppoverlay_osd5dtg_dv_total_reg.regValue);

        ppoverlay_osd5dtg_dh_den_start_end_reg.osd5dtg_dh_den_sta = timing->hstart;
        ppoverlay_osd5dtg_dh_den_start_end_reg.osd5dtg_dh_den_end = timing->hend;
        IoReg_Write32(PPOVERLAY_osd5dtg_DH_DEN_Start_End_reg, ppoverlay_osd5dtg_dh_den_start_end_reg.regValue);

        ppoverlay_osd5dtg_dv_den_start_end_reg.osd5dtg_dv_den_sta = timing->vstart;
        ppoverlay_osd5dtg_dv_den_start_end_reg.osd5dtg_dv_den_end = timing->vend;
        IoReg_Write32(PPOVERLAY_osd5dtg_DV_DEN_Start_End_reg, ppoverlay_osd5dtg_dv_den_start_end_reg.regValue);

        DTG_PRINT("[%s %d]%s hstart:%d, hend:%d, vstart:%d ,vend:%d, htotal:%d, htotal_last_line:%d, vtotal:%d\n",
        __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), timing->hstart, timing->hend, timing->vstart, timing->vend,
            timing->htotal, timing->htotal_last_line, timing->vtotal);
    }
    else if(handler == DTG_GET)
    {
        timing->htotal = ppoverlay_osd5dtg_dh_total_reg.osd5dtg_dh_total;
        timing->htotal_last_line = ppoverlay_osd5dtg_dh_total_reg.osd5dtg_dh_total_last_line;
        timing->vtotal = ppoverlay_osd5dtg_dv_total_reg.osd5dtg_dv_total;
        timing->hstart = ppoverlay_osd5dtg_dh_den_start_end_reg.osd5dtg_dh_den_sta;
        timing->hend = ppoverlay_osd5dtg_dh_den_start_end_reg.osd5dtg_dh_den_end;
        timing->vstart = ppoverlay_osd5dtg_dv_den_start_end_reg.osd5dtg_dv_den_sta;
        timing->vend = ppoverlay_osd5dtg_dv_den_start_end_reg.osd5dtg_dv_den_end;
    }

    return ret;
}
static int scaler_dtg_d2post_handler_timing(DTG_HANDLER handler, DTG_TIMING_INFO *timing)
{
    int ret = 0;

    ppoverlay_memcdtg_dv_total_RBUS ppoverlay_memcdtg_dv_total_reg;
    ppoverlay_memcdtg_dh_total_RBUS ppoverlay_memcdtg_dh_total_reg;
    ppoverlay_memcdtg_dv_den_start_end_RBUS ppoverlay_memcdtg_dv_den_start_end_reg;
    ppoverlay_memcdtg_dh_den_start_end_RBUS ppoverlay_memcdtg_dh_den_start_end_reg;
    ppoverlay_memcdtg_free_dv_total_RBUS ppoverlay_memcdtg_free_dv_total_reg;
    ppoverlay_memcdtg_free_dh_total_RBUS ppoverlay_memcdtg_free_dh_total_reg;

    if(timing == NULL)
    {
        DTG_PRINT("[%s %d][Error] timing parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_memcdtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DH_TOTAL_reg);
    ppoverlay_memcdtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DV_TOTAL_reg);
    ppoverlay_memcdtg_dh_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DH_DEN_Start_End_reg);
    ppoverlay_memcdtg_dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg);
    ppoverlay_memcdtg_free_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_FREE_DV_TOTAL_reg);
    ppoverlay_memcdtg_free_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_FREE_DH_TOTAL_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_memcdtg_dh_total_reg.memcdtg_dh_total = timing->htotal;
        ppoverlay_memcdtg_dh_total_reg.memcdtg_dh_total_last_line = timing->htotal_last_line;
        IoReg_Write32(PPOVERLAY_memcdtg_DH_TOTAL_reg, ppoverlay_memcdtg_dh_total_reg.regValue);

        ppoverlay_memcdtg_dv_total_reg.memcdtg_dv_total = timing->vtotal;
        IoReg_Write32(PPOVERLAY_memcdtg_DV_TOTAL_reg, ppoverlay_memcdtg_dv_total_reg.regValue);

        ppoverlay_memcdtg_dh_den_start_end_reg.memcdtg_dh_den_sta = timing->hstart;
        ppoverlay_memcdtg_dh_den_start_end_reg.memcdtg_dh_den_end = timing->hend;
        IoReg_Write32(PPOVERLAY_memcdtg_DH_DEN_Start_End_reg, ppoverlay_memcdtg_dh_den_start_end_reg.regValue);

        ppoverlay_memcdtg_dv_den_start_end_reg.memcdtg_dv_den_sta = timing->vstart;
        ppoverlay_memcdtg_dv_den_start_end_reg.memcdtg_dv_den_end = timing->vend;
        IoReg_Write32(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg, ppoverlay_memcdtg_dv_den_start_end_reg.regValue);

        ppoverlay_memcdtg_free_dv_total_reg.memcdtg_free_dv_total = timing->free_run_vtotal;
        IoReg_Write32(PPOVERLAY_memcdtg_FREE_DV_TOTAL_reg, ppoverlay_memcdtg_free_dv_total_reg.regValue);

        ppoverlay_memcdtg_free_dh_total_reg.memcdtg_free_dh_total = timing->free_run_htotal;
        ppoverlay_memcdtg_free_dh_total_reg.memcdtg_free_dh_total_last_line =  timing->free_run_htotal_last_line;
        IoReg_Write32(PPOVERLAY_memcdtg_FREE_DH_TOTAL_reg, ppoverlay_memcdtg_free_dh_total_reg.regValue);

        DTG_PRINT("[%s %d]%s hstart:%d, hend:%d, vstart:%d ,vend:%d, htotal:%d, htotal_last_line:%d, vtotal:%d\n",
        __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), timing->hstart, timing->hend, timing->vstart, timing->vend,
            timing->htotal, timing->htotal_last_line, timing->vtotal);
    }
    else if(handler == DTG_GET)
    {
        timing->htotal = ppoverlay_memcdtg_dh_total_reg.memcdtg_dh_total;
        timing->htotal_last_line = ppoverlay_memcdtg_dh_total_reg.memcdtg_dh_total_last_line;
        timing->vtotal = ppoverlay_memcdtg_dv_total_reg.memcdtg_dv_total;
        timing->hstart = ppoverlay_memcdtg_dh_den_start_end_reg.memcdtg_dh_den_sta;
        timing->hend = ppoverlay_memcdtg_dh_den_start_end_reg.memcdtg_dh_den_end;
        timing->vstart = ppoverlay_memcdtg_dv_den_start_end_reg.memcdtg_dv_den_sta;
        timing->vend = ppoverlay_memcdtg_dv_den_start_end_reg.memcdtg_dv_den_end;
        timing->free_run_htotal = ppoverlay_memcdtg_free_dh_total_reg.memcdtg_free_dh_total;
        timing->free_run_htotal_last_line = ppoverlay_memcdtg_free_dh_total_reg.memcdtg_free_dh_total_last_line;
        timing->free_run_vtotal = ppoverlay_memcdtg_free_dv_total_reg.memcdtg_free_dv_total;
    }

    return ret;
}

//always herit d2post
static int scaler_dtg_d2post2_handler_timing(DTG_HANDLER handler, DTG_TIMING_INFO *timing)
{
    int ret = 0;

    ppoverlay_d2post2dtg_dv_total_RBUS ppoverlay_d2post2dtg_dv_total_reg;
    ppoverlay_d2post2dtg_dh_total_RBUS ppoverlay_d2post2dtg_dh_total_reg;
    ppoverlay_d2post2dtg_dv_den_start_end_RBUS ppoverlay_d2post2dtg_dv_den_start_end_reg;
    ppoverlay_d2post2dtg_dh_den_start_end_RBUS ppoverlay_d2post2dtg_dh_den_start_end_reg;
    ppoverlay_d2post2dtg_free_dv_total_RBUS ppoverlay_d2post2dtg_free_dv_total_reg;
    ppoverlay_d2post2dtg_free_dh_total_RBUS ppoverlay_d2post2dtg_free_dh_total_reg;

    if(timing == NULL)
    {
        DTG_PRINT("[%s %d][Error] timing parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }


    ppoverlay_d2post2dtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_d2post2dtg_DH_TOTAL_reg);
    ppoverlay_d2post2dtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_d2post2dtg_DV_TOTAL_reg);
    ppoverlay_d2post2dtg_dh_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_d2post2dtg_DH_DEN_Start_End_reg);
    ppoverlay_d2post2dtg_dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_d2post2dtg_DV_DEN_Start_End_reg);
    ppoverlay_d2post2dtg_free_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_d2post2dtg_FREE_DV_TOTAL_reg);
    ppoverlay_d2post2dtg_free_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_d2post2dtg_FREE_DH_TOTAL_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_d2post2dtg_dh_total_reg.d2post2dtg_dh_total = timing->htotal;
        ppoverlay_d2post2dtg_dh_total_reg.d2post2dtg_dh_total_last_line = timing->htotal_last_line;
        IoReg_Write32(PPOVERLAY_d2post2dtg_DH_TOTAL_reg, ppoverlay_d2post2dtg_dh_total_reg.regValue);

        ppoverlay_d2post2dtg_dv_total_reg.d2post2dtg_dv_total = timing->vtotal;
        IoReg_Write32(PPOVERLAY_d2post2dtg_DV_TOTAL_reg, ppoverlay_d2post2dtg_dv_total_reg.regValue);

        ppoverlay_d2post2dtg_dh_den_start_end_reg.d2post2dtg_dh_den_sta = timing->hstart;
        ppoverlay_d2post2dtg_dh_den_start_end_reg.d2post2dtg_dh_den_end = timing->hend;
        IoReg_Write32(PPOVERLAY_d2post2dtg_DH_DEN_Start_End_reg, ppoverlay_d2post2dtg_dh_den_start_end_reg.regValue);

        ppoverlay_d2post2dtg_dv_den_start_end_reg.d2post2dtg_dv_den_sta = timing->vstart;
        ppoverlay_d2post2dtg_dv_den_start_end_reg.d2post2dtg_dv_den_end = timing->vend;
        IoReg_Write32(PPOVERLAY_d2post2dtg_DV_DEN_Start_End_reg, ppoverlay_d2post2dtg_dv_den_start_end_reg.regValue);

        ppoverlay_d2post2dtg_free_dv_total_reg.d2post2dtg_free_dv_total = timing->free_run_vtotal;
        IoReg_Write32(PPOVERLAY_d2post2dtg_FREE_DV_TOTAL_reg, ppoverlay_d2post2dtg_free_dh_total_reg.regValue);

        ppoverlay_d2post2dtg_free_dh_total_reg.d2post2dtg_free_dh_total = timing->free_run_htotal;
        ppoverlay_d2post2dtg_free_dh_total_reg.d2post2dtg_free_dh_total_last_line = timing->free_run_htotal_last_line;
        IoReg_Write32(PPOVERLAY_d2post2dtg_FREE_DH_TOTAL_reg, ppoverlay_d2post2dtg_free_dh_total_reg.regValue);

        DTG_PRINT("[%s %d]%s hstart:%d, hend:%d, vstart:%d ,vend:%d, htotal:%d, htotal_last_line:%d, vtotal:%d\n",
            __FUNCTION__, __LINE__, (handler == DTG_SET) ? "DTG_SET" : ((handler == DTG_GET) ? "DTG_GET" : "UNKNOWN"), timing->hstart, timing->hend, timing->vstart, timing->vend,
                timing->htotal, timing->htotal_last_line, timing->vtotal);

    }
    else if(handler == DTG_GET)
    {
        timing->htotal = ppoverlay_d2post2dtg_dh_total_reg.d2post2dtg_dh_total;
        timing->htotal_last_line = ppoverlay_d2post2dtg_dh_total_reg.d2post2dtg_dh_total_last_line;
        timing->vtotal = ppoverlay_d2post2dtg_dv_total_reg.d2post2dtg_dv_total;
        timing->hstart = ppoverlay_d2post2dtg_dh_den_start_end_reg.d2post2dtg_dh_den_sta;
        timing->hend = ppoverlay_d2post2dtg_dh_den_start_end_reg.d2post2dtg_dh_den_end;
        timing->vstart = ppoverlay_d2post2dtg_dv_den_start_end_reg.d2post2dtg_dv_den_sta;
        timing->vend = ppoverlay_d2post2dtg_dv_den_start_end_reg.d2post2dtg_dv_den_end;
        timing->free_run_htotal = ppoverlay_d2post2dtg_free_dh_total_reg.d2post2dtg_free_dh_total;
        timing->free_run_htotal_last_line = ppoverlay_d2post2dtg_free_dh_total_reg.d2post2dtg_free_dh_total_last_line;
        timing->free_run_vtotal = ppoverlay_d2post2dtg_free_dv_total_reg.d2post2dtg_free_dv_total;
    }

    return ret;
}

//control
static int scaler_dtg_master_handler_control(DTG_HANDLER handler, DTG_CONTROL_INFO *control)
{
    int ret = 0;
    ppoverlay_display_timing_ctrl1_RBUS ppoverlay_display_timing_ctrl1_reg;
    ppoverlay_display_timing_ctrl2_RBUS ppoverlay_display_timing_ctrl2_reg;

    if(control == NULL)
    {
        DTG_PRINT("[%s %d][Error] control parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
    ppoverlay_display_timing_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL2_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_display_timing_ctrl2_reg.frc2fsync_en = control->frc2fsync_en;
        ppoverlay_display_timing_ctrl2_reg.shpnr_line_mode_sel = control->shpnr_line_mode_sel;
        IoReg_Write32(PPOVERLAY_Display_Timing_CTRL2_reg, ppoverlay_display_timing_ctrl2_reg.regValue);

        ppoverlay_display_timing_ctrl1_reg.disp_en = control->dtg_en;
        ppoverlay_display_timing_ctrl1_reg.disp_fsync_en = control->fsync_en;
        ppoverlay_display_timing_ctrl1_reg.disp_fix_last_line_new = control->fix_last_line_new;
        ppoverlay_display_timing_ctrl1_reg.fsync_fll_mode = control->fsync_fll_mode;
        ppoverlay_display_timing_ctrl1_reg.mdtg_line_cnt_sync = control->line_cnt_sync;
        ppoverlay_display_timing_ctrl1_reg.disp_frc_on_fsync = control->frc_on_fsync;
        IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, ppoverlay_display_timing_ctrl1_reg.regValue);
    }
    else if(handler == DTG_GET)
    {
        control->dtg_en = ppoverlay_display_timing_ctrl1_reg.disp_en;
        control->fsync_en = ppoverlay_display_timing_ctrl1_reg.disp_fsync_en;
        control->fix_last_line_new = ppoverlay_display_timing_ctrl1_reg.disp_fix_last_line_new;
        control->fsync_fll_mode = ppoverlay_display_timing_ctrl1_reg.fsync_fll_mode;
        control->line_cnt_sync = ppoverlay_display_timing_ctrl1_reg.mdtg_line_cnt_sync;
        control->wde_to_free_run = ppoverlay_display_timing_ctrl1_reg.wde_to_free_run;
        control->wde_to_free_run_dprx = ppoverlay_display_timing_ctrl1_reg.wde_to_free_run_dprx;
        control->wde_to_free_run_hdmi = ppoverlay_display_timing_ctrl1_reg.wde_to_free_run_hdmi;
        control->frc_fsync_status = ppoverlay_display_timing_ctrl1_reg.disp_frc_fsync;
        control->frc2fsync_en = ppoverlay_display_timing_ctrl2_reg.frc2fsync_en;
        control->frc_on_fsync = ppoverlay_display_timing_ctrl1_reg.disp_frc_on_fsync;
        control->shpnr_line_mode_sel = ppoverlay_display_timing_ctrl2_reg.shpnr_line_mode_sel;
    }

    return ret;

}

static int scaler_dtg_master2_handler_control(DTG_HANDLER handler, DTG_CONTROL_INFO *control)
{
    int ret = 0;
    ppoverlay_display_timing_ctrl6_RBUS ppoverlay_display_timing_ctrl6_reg;

    if(control == NULL)
    {
        DTG_PRINT("[%s %d][Error] control parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_display_timing_ctrl6_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL6_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_display_timing_ctrl6_reg.disp2_en = control->dtg_en;
        ppoverlay_display_timing_ctrl6_reg.disp2_fsync_en = control->fsync_en;
        IoReg_Write32(PPOVERLAY_Display_Timing_CTRL6_reg, ppoverlay_display_timing_ctrl6_reg.regValue);
    }
    else if(handler == DTG_GET)
    {
        control->dtg_en = ppoverlay_display_timing_ctrl6_reg.disp2_en;
        control->fsync_en = ppoverlay_display_timing_ctrl6_reg.disp2_fsync_en;
        control->frc_fsync_status = ppoverlay_display_timing_ctrl6_reg.disp2_frc_fsync;
        control->wde_to_free_run = ppoverlay_display_timing_ctrl6_reg.disp2_wde_to_free_run;
        control->wde_to_free_run_dprx = ppoverlay_display_timing_ctrl6_reg.disp2_wde_to_free_run_dprx;
        control->wde_to_free_run_hdmi = ppoverlay_display_timing_ctrl6_reg.disp2_wde_to_free_run_hdmi;
    }

    return ret;

}

static int scaler_dtg_uzu_handler_control(DTG_HANDLER handler, DTG_CONTROL_INFO *control)
{
    int ret = 0;
    ppoverlay_uzudtg_countrol_RBUS ppoverlay_uzudtg_countrol_reg;

    if(control == NULL)
    {
        DTG_PRINT("[%s %d][Error] control parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_uzudtg_countrol_reg.regValue = IoReg_Read32(PPOVERLAY_UZUDTG_COUNTROL_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_uzudtg_countrol_reg.uzudtg_fsync_select = control->fsync_select;
        ppoverlay_uzudtg_countrol_reg.uzudtg_frame_cnt_reset_en = control->frame_cnt_reset_en;
        ppoverlay_uzudtg_countrol_reg.uzudtg_frc2fsync_by_hw = control->frc2fsync_by_hw;
        ppoverlay_uzudtg_countrol_reg.uzudtg_line_cnt_sync = control->line_cnt_sync;
        ppoverlay_uzudtg_countrol_reg.uzudtg_fsync_en = control->fsync_en;
        ppoverlay_uzudtg_countrol_reg.uzudtg_en = control->dtg_en;
        ppoverlay_uzudtg_countrol_reg.uzudtg_mode_revised = control->mode_revised;
        IoReg_Write32(PPOVERLAY_UZUDTG_COUNTROL_reg, ppoverlay_uzudtg_countrol_reg.regValue);
    }
    else if(handler == DTG_GET)
    {
        control->dtg_en = ppoverlay_uzudtg_countrol_reg.uzudtg_en;
        control->fsync_en = ppoverlay_uzudtg_countrol_reg.uzudtg_fsync_en;
        control->frc2fsync_by_hw = ppoverlay_uzudtg_countrol_reg.uzudtg_frc2fsync_by_hw;
        control->frc_fsync_status = ppoverlay_uzudtg_countrol_reg.uzudtg_frc_fsync_status;
        control->line_cnt_sync = ppoverlay_uzudtg_countrol_reg.uzudtg_line_cnt_sync;
        control->mode_revised = ppoverlay_uzudtg_countrol_reg.uzudtg_mode_revised;
        control->fsync_select = ppoverlay_uzudtg_countrol_reg.uzudtg_fsync_select;
    }

    return ret;

}


static int scaler_dtg_d2pre_handler_control(DTG_HANDLER handler, DTG_CONTROL_INFO *control)
{
    int ret = 0;
    ppoverlay_d2predtg_control_RBUS ppoverlay_d2predtg_control_reg;

    if(control == NULL)
    {
        DTG_PRINT("[%s %d][Error] control parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_d2predtg_control_reg.regValue = IoReg_Read32(PPOVERLAY_D2PREDTG_CONTROL_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_d2predtg_control_reg.d2predtg_fsync_en = control->fsync_en;
        ppoverlay_d2predtg_control_reg.d2predtg_frc2fsync_by_hw = control->frc2fsync_by_hw;
        ppoverlay_d2predtg_control_reg.d2predtg_en = control->dtg_en;
        ppoverlay_d2predtg_control_reg.d2predtg_line_cnt_sync = control->line_cnt_sync;
        ppoverlay_d2predtg_control_reg.d2predtg_mode_revised = control->mode_revised;
        IoReg_Write32(PPOVERLAY_D2PREDTG_CONTROL_reg, ppoverlay_d2predtg_control_reg.regValue);
    }
    else if(handler == DTG_GET)
    {
        control->dtg_en = ppoverlay_d2predtg_control_reg.d2predtg_en;
        control->fsync_en = ppoverlay_d2predtg_control_reg.d2predtg_fsync_en;
        control->frc2fsync_by_hw = ppoverlay_d2predtg_control_reg.d2predtg_frc2fsync_by_hw;
        control->frc_fsync_status = ppoverlay_d2predtg_control_reg.d2predtg_frc_fsync_status;
        control->line_cnt_sync = ppoverlay_d2predtg_control_reg.d2predtg_line_cnt_sync;
        control->mode_revised = ppoverlay_d2predtg_control_reg.d2predtg_mode_revised;
    }

    return ret;

}


static int scaler_dtg_osd123_handler_control(DTG_HANDLER handler, DTG_CONTROL_INFO *control)
{
    int ret = 0;
    ppoverlay_osddtg_control_RBUS ppoverlay_osddtg_control_reg;

    if(control == NULL)
    {
        DTG_PRINT("[%s %d][Error] control parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_osddtg_control_reg.regValue = IoReg_Read32(PPOVERLAY_OSDDTG_CONTROL_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_osddtg_control_reg.osddtg_en = control->dtg_en;
        ppoverlay_osddtg_control_reg.osddtg_frame_cnt_reset_en = control->frame_cnt_reset_en;
        ppoverlay_osddtg_control_reg.osddtg_frc2fsync_by_hw = control->frc2fsync_by_hw;
        ppoverlay_osddtg_control_reg.osddtg_fsync_en = control->fsync_en;
        ppoverlay_osddtg_control_reg.osddtg_fsync_select = control->fsync_select;
        ppoverlay_osddtg_control_reg.osd_split = control->osd_split;
        ppoverlay_osddtg_control_reg.osddtg_line_cnt_sync = control->line_cnt_sync;
        ppoverlay_osddtg_control_reg.osddtg_mode_revised = control->mode_revised;
        IoReg_Write32(PPOVERLAY_OSDDTG_CONTROL_reg, ppoverlay_osddtg_control_reg.regValue);
    }
    else if(handler == DTG_GET)
    {
        control->dtg_en = ppoverlay_osddtg_control_reg.osddtg_en;
        control->frame_cnt_reset_en = ppoverlay_osddtg_control_reg.osddtg_frame_cnt_reset_en;
        control->frc2fsync_by_hw = ppoverlay_osddtg_control_reg.osddtg_frc2fsync_by_hw;
        control->fsync_en = ppoverlay_osddtg_control_reg.osddtg_fsync_en;
        control->fsync_select = ppoverlay_osddtg_control_reg.osddtg_fsync_select;
        control->osd_split = ppoverlay_osddtg_control_reg.osd_split;
        control->line_cnt_sync = ppoverlay_osddtg_control_reg.osddtg_line_cnt_sync;
        control->mode_revised = ppoverlay_osddtg_control_reg.osddtg_mode_revised;
        control->frc_fsync_status = ppoverlay_osddtg_control_reg.osddtg_frc_fsync_status;
    }

    return ret;

}

static int scaler_dtg_osd4_handler_control(DTG_HANDLER handler, DTG_CONTROL_INFO *control)
{
    int ret = 0;
    ppoverlay_osd4dtg_control_RBUS ppoverlay_osd4dtg_control_reg;

    if(control == NULL)
    {
        DTG_PRINT("[%s %d][Error] control parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_osd4dtg_control_reg.regValue = IoReg_Read32(PPOVERLAY_OSD4DTG_CONTROL_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_osd4dtg_control_reg.osd4dtg_en = control->dtg_en;
        ppoverlay_osd4dtg_control_reg.osd4dtg_frame_cnt_reset_en = control->frame_cnt_reset_en;
        ppoverlay_osd4dtg_control_reg.osd4dtg_frc2fsync_by_hw = control->frc2fsync_by_hw;
        ppoverlay_osd4dtg_control_reg.osd4dtg_fsync_en = control->fsync_en;
        ppoverlay_osd4dtg_control_reg.osd4dtg_fsync_select = control->fsync_select;
        ppoverlay_osd4dtg_control_reg.osd4_split = control->osd_split;
        ppoverlay_osd4dtg_control_reg.osd4dtg_line_cnt_sync = control->line_cnt_sync;
        ppoverlay_osd4dtg_control_reg.osd4dtg_mode_revised = control->mode_revised;
        IoReg_Write32(PPOVERLAY_OSD4DTG_CONTROL_reg, ppoverlay_osd4dtg_control_reg.regValue);
    }
    else if(handler == DTG_GET)
    {
        control->dtg_en = ppoverlay_osd4dtg_control_reg.osd4dtg_en;
        control->frame_cnt_reset_en = ppoverlay_osd4dtg_control_reg.osd4dtg_frame_cnt_reset_en;
        control->frc2fsync_by_hw = ppoverlay_osd4dtg_control_reg.osd4dtg_frc2fsync_by_hw;
        control->fsync_en = ppoverlay_osd4dtg_control_reg.osd4dtg_fsync_en;
        control->fsync_select = ppoverlay_osd4dtg_control_reg.osd4dtg_fsync_select;
        control->osd_split = ppoverlay_osd4dtg_control_reg.osd4_split;
        control->line_cnt_sync = ppoverlay_osd4dtg_control_reg.osd4dtg_line_cnt_sync;
        control->mode_revised = ppoverlay_osd4dtg_control_reg.osd4dtg_mode_revised;
        control->frc_fsync_status = ppoverlay_osd4dtg_control_reg.osd4dtg_frc_fsync_status;
    }

    return ret;

}

static int scaler_dtg_osd5_handler_control(DTG_HANDLER handler, DTG_CONTROL_INFO *control)
{
    int ret = 0;
    ppoverlay_osd5dtg_control_RBUS ppoverlay_osd5dtg_control_reg;

    if(control == NULL)
    {
        DTG_PRINT("[%s %d][Error] control parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_osd5dtg_control_reg.regValue = IoReg_Read32(PPOVERLAY_OSD5DTG_CONTROL_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_osd5dtg_control_reg.osd5dtg_en = control->dtg_en;
        ppoverlay_osd5dtg_control_reg.osd5dtg_frame_cnt_reset_en = control->frame_cnt_reset_en;
        ppoverlay_osd5dtg_control_reg.osd5dtg_frc2fsync_by_hw = control->frc2fsync_by_hw;
        ppoverlay_osd5dtg_control_reg.osd5dtg_fsync_en = control->fsync_en;
        ppoverlay_osd5dtg_control_reg.osd5_split = control->osd_split;
        ppoverlay_osd5dtg_control_reg.osd5dtg_line_cnt_sync = control->line_cnt_sync;
        ppoverlay_osd5dtg_control_reg.osd5dtg_mode_revised = control->mode_revised;
        IoReg_Write32(PPOVERLAY_OSD5DTG_CONTROL_reg, ppoverlay_osd5dtg_control_reg.regValue);
    }
    else if(handler == DTG_GET)
    {
        control->dtg_en = ppoverlay_osd5dtg_control_reg.osd5dtg_en;
        control->frame_cnt_reset_en = ppoverlay_osd5dtg_control_reg.osd5dtg_frame_cnt_reset_en;
        control->frc2fsync_by_hw = ppoverlay_osd5dtg_control_reg.osd5dtg_frc2fsync_by_hw;
        control->fsync_en = ppoverlay_osd5dtg_control_reg.osd5dtg_fsync_en;
        control->osd_split = ppoverlay_osd5dtg_control_reg.osd5_split;
        control->line_cnt_sync = ppoverlay_osd5dtg_control_reg.osd5dtg_line_cnt_sync;
        control->mode_revised = ppoverlay_osd5dtg_control_reg.osd5dtg_mode_revised;
        control->frc_fsync_status = ppoverlay_osd5dtg_control_reg.osd5dtg_frc_fsync_status;
    }

    return ret;

}

static int scaler_dtg_d2post_handler_control(DTG_HANDLER handler, DTG_CONTROL_INFO *control)
{
    int ret = 0;
    ppoverlay_memcdtg_control_RBUS ppoverlay_memcdtg_control_reg;

    if(control == NULL)
    {
        DTG_PRINT("[%s %d][Error] control parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_memcdtg_control_reg.regValue = IoReg_Read32(PPOVERLAY_MEMCDTG_CONTROL_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_memcdtg_control_reg.memcdtg_en = control->dtg_en;
        ppoverlay_memcdtg_control_reg.memcdtg_frame_cnt_reset_en = control->frame_cnt_reset_en;
        ppoverlay_memcdtg_control_reg.memcdtg_frc2fsync_by_hw = control->frc2fsync_by_hw;
        ppoverlay_memcdtg_control_reg.memcdtg_fsync_en = control->fsync_en;
        ppoverlay_memcdtg_control_reg.memcdtg_line_cnt_sync = control->line_cnt_sync;
        ppoverlay_memcdtg_control_reg.memcdtg_mode_revised = control->mode_revised;
        ppoverlay_memcdtg_control_reg.memcdtg_free_vh_total_en = control->free_vh_total_en;
        IoReg_Write32(PPOVERLAY_MEMCDTG_CONTROL_reg, ppoverlay_memcdtg_control_reg.regValue);
    }
    else if(handler == DTG_GET)
    {
        control->dtg_en = ppoverlay_memcdtg_control_reg.memcdtg_en;
        control->frame_cnt_reset_en = ppoverlay_memcdtg_control_reg.memcdtg_frame_cnt_reset_en;
        control->frc2fsync_by_hw = ppoverlay_memcdtg_control_reg.memcdtg_frc2fsync_by_hw;
        control->fsync_en = ppoverlay_memcdtg_control_reg.memcdtg_fsync_en;
        control->free_vh_total_en = ppoverlay_memcdtg_control_reg.memcdtg_free_vh_total_en;
        control->line_cnt_sync = ppoverlay_memcdtg_control_reg.memcdtg_line_cnt_sync;
        control->mode_revised = ppoverlay_memcdtg_control_reg.memcdtg_mode_revised;
        control->frc_fsync_status = ppoverlay_memcdtg_control_reg.memcdtg_frc_fsync_status;
        control->free_vh_total_en = ppoverlay_memcdtg_control_reg.memcdtg_free_vh_total_en;
    }

    return ret;

}

//always herit d2post
static int scaler_dtg_d2post2_handler_control(DTG_HANDLER handler, DTG_CONTROL_INFO *control)
{
    int ret = 0;
    ppoverlay_d2post2dtg_control_RBUS ppoverlay_d2post2dtg_control_reg;

    if(control == NULL)
    {
        DTG_PRINT("[%s %d][Error] control parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_d2post2dtg_control_reg.regValue = IoReg_Read32(PPOVERLAY_D2POST2DTG_CONTROL_reg);

    if(handler == DTG_SET)
    {
        ppoverlay_d2post2dtg_control_reg.d2post2dtg_en = control->dtg_en;
        ppoverlay_d2post2dtg_control_reg.d2post2dtg_frc2fsync_by_hw = control->frc2fsync_by_hw;
        ppoverlay_d2post2dtg_control_reg.d2post2dtg_fsync_en = control->fsync_en;
        ppoverlay_d2post2dtg_control_reg.d2post2dtg_line_cnt_sync = control->line_cnt_sync;
        ppoverlay_d2post2dtg_control_reg.d2post2dtg_mode_revised = control->mode_revised;
        ppoverlay_d2post2dtg_control_reg.d2post2dtg_free_vh_total_en = control->free_vh_total_en;
        IoReg_Write32(PPOVERLAY_D2POST2DTG_CONTROL_reg, ppoverlay_d2post2dtg_control_reg.regValue);
    }
    else if(handler == DTG_GET)
    {
        control->dtg_en = ppoverlay_d2post2dtg_control_reg.d2post2dtg_en;
        control->frc2fsync_by_hw = ppoverlay_d2post2dtg_control_reg.d2post2dtg_frc2fsync_by_hw;
        control->fsync_en = ppoverlay_d2post2dtg_control_reg.d2post2dtg_fsync_en;
        control->free_vh_total_en = ppoverlay_d2post2dtg_control_reg.d2post2dtg_free_vh_total_en;
        control->line_cnt_sync = ppoverlay_d2post2dtg_control_reg.d2post2dtg_line_cnt_sync;
        control->mode_revised = ppoverlay_d2post2dtg_control_reg.d2post2dtg_mode_revised;
        control->frc_fsync_status = ppoverlay_d2post2dtg_control_reg.d2post2dtg_frc_fsync_status;
        control->free_vh_total_en = ppoverlay_d2post2dtg_control_reg.d2post2dtg_free_vh_total_en;
    }

    return ret;
}

int scaler_dtg_set_fraction(DTG_TYPE dtgType, DTG_FRACTION_INFO *fraction)
{
    int ret = 0;

    if(dtgType > DTG_MAX || fraction == NULL)
    {
        DTG_PRINT("[%s %d][Error] unknow dtg type\n", __FUNCTION__, __LINE__);
        return -1;
    }

    switch(dtgType)
    {
        case DTG_MASTER:
            ret = scaler_dtg_master_handler_fraction(DTG_SET, fraction);
            break;

        case DTG_MASTER2:
            ret = scaler_dtg_master2_handler_fraction(DTG_SET, fraction);
            break;

        case DTG_UZU:
            ret = scaler_dtg_uzu_handler_fraction(DTG_SET, fraction);
            break;

        case DTG_D2PRE:
            ret = scaler_dtg_d2pre_handler_fraction(DTG_SET, fraction);
            break;

        case DTG_OSD123:
            ret = scaler_dtg_osd123_handler_fraction(DTG_SET, fraction);
            break;

        case DTG_OSD4:
            ret = scaler_dtg_osd4_handler_fraction(DTG_SET, fraction);
            break;

        case DTG_OSD5:
            ret = scaler_dtg_osd5_handler_fraction(DTG_SET, fraction);
            break;

        case DTG_D2POST:
            ret = scaler_dtg_d2post_handler_fraction(DTG_SET, fraction);
            break;

        case DTG_D2POST2:
            ret = scaler_dtg_d2post2_handler_fraction(DTG_SET, fraction);
            break;

        default:
            break;
    }

    return ret;
}

int scaler_dtg_get_fraction(DTG_TYPE dtgType, DTG_FRACTION_INFO *fraction)
{
    int ret = 0;

    if(dtgType > DTG_MAX || fraction == NULL)
    {
        DTG_PRINT("[%s %d][Error] unknow dtg type\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(fraction, 0, sizeof(DTG_FRACTION_INFO));

    switch(dtgType)
    {
        case DTG_MASTER:
            ret = scaler_dtg_master_handler_fraction(DTG_GET, fraction);
            break;

        case DTG_MASTER2:
            ret = scaler_dtg_master2_handler_fraction(DTG_GET, fraction);
            break;

        case DTG_UZU:
            ret = scaler_dtg_uzu_handler_fraction(DTG_GET, fraction);
            break;

        case DTG_D2PRE:
            ret = scaler_dtg_d2pre_handler_fraction(DTG_GET, fraction);
            break;

        case DTG_OSD123:
            ret = scaler_dtg_osd123_handler_fraction(DTG_GET, fraction);
            break;

        case DTG_OSD4:
            ret = scaler_dtg_osd4_handler_fraction(DTG_GET, fraction);
            break;

        case DTG_OSD5:
            ret = scaler_dtg_osd5_handler_fraction(DTG_GET, fraction);
            break;

        case DTG_D2POST:
            ret = scaler_dtg_d2post_handler_fraction(DTG_GET, fraction);
            break;

        case DTG_D2POST2:
            ret = scaler_dtg_d2post2_handler_fraction(DTG_GET, fraction);
            break;

        default:
            break;
    }

    return ret;
}

int scaler_dtg_set_timing(DTG_TYPE dtgType, DTG_TIMING_INFO *timing)
{
    int ret = 0;

    if(dtgType > DTG_MAX || timing == NULL)
    {
        DTG_PRINT("[%s %d][Error] unknow dtg type\n", __FUNCTION__, __LINE__);
        return -1;
    }

    switch(dtgType)
    {
        case DTG_MASTER:
            ret = scaler_dtg_master_handler_timing(DTG_SET, timing);
            break;

        case DTG_MASTER2:
            ret = scaler_dtg_master2_handler_timing(DTG_SET, timing);
            break;

        case DTG_UZU:
            ret = scaler_dtg_uzu_handler_timing(DTG_SET, timing);
            break;

        case DTG_D2PRE:
            ret = scaler_dtg_d2pre_handler_timing(DTG_SET, timing);
            break;

        case DTG_OSD123:
            ret = scaler_dtg_osd123_handler_timing(DTG_SET, timing);
            break;

        case DTG_OSD4:
            ret = scaler_dtg_osd4_handler_timing(DTG_SET, timing);
            break;

        case DTG_OSD5:
            ret = scaler_dtg_osd5_handler_timing(DTG_SET, timing);
            break;

        case DTG_D2POST:
            ret = scaler_dtg_d2post_handler_timing(DTG_SET, timing);
            break;

        case DTG_D2POST2:
            ret = scaler_dtg_d2post2_handler_timing(DTG_SET, timing);
            break;

        default:
            break;
    }

    return ret;
}

int scaler_dtg_get_timing(DTG_TYPE dtgType, DTG_TIMING_INFO *timing)
{
    int ret = 0;

    if(dtgType > DTG_MAX || timing == NULL)
    {
        DTG_PRINT("[%s %d][Error] unknow dtg type\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(timing, 0, sizeof(DTG_TIMING_INFO));

    switch(dtgType)
    {
        case DTG_MASTER:
            ret = scaler_dtg_master_handler_timing(DTG_GET, timing);
            break;

        case DTG_MASTER2:
            ret = scaler_dtg_master2_handler_timing(DTG_GET, timing);
            break;

        case DTG_UZU:
            ret = scaler_dtg_uzu_handler_timing(DTG_GET, timing);
            break;

        case DTG_D2PRE:
            ret = scaler_dtg_d2pre_handler_timing(DTG_GET, timing);
            break;

        case DTG_OSD123:
            ret = scaler_dtg_osd123_handler_timing(DTG_GET, timing);
            break;

        case DTG_OSD4:
            ret = scaler_dtg_osd4_handler_timing(DTG_GET, timing);
            break;

        case DTG_OSD5:
            ret = scaler_dtg_osd5_handler_timing(DTG_GET, timing);
            break;

        case DTG_D2POST:
            ret = scaler_dtg_d2post_handler_timing(DTG_GET, timing);
            break;

        case DTG_D2POST2:
            ret = scaler_dtg_d2post2_handler_timing(DTG_GET, timing);
            break;

        default:
            break;
    }

    return ret;
}

int scaler_dtg_set_control(DTG_TYPE dtgType, DTG_CONTROL_INFO *control)
{
    int ret = 0;

    if(dtgType > DTG_MAX || control == NULL)
    {
        DTG_PRINT("[%s %d][Error] unknow dtg type\n", __FUNCTION__, __LINE__);
        return -1;
    }

    switch(dtgType)
    {
        case DTG_MASTER:
            ret = scaler_dtg_master_handler_control(DTG_SET, control);
            break;

        case DTG_MASTER2:
            ret = scaler_dtg_master2_handler_control(DTG_SET, control);
            break;

        case DTG_UZU:
            ret = scaler_dtg_uzu_handler_control(DTG_SET, control);
            break;

        case DTG_D2PRE:
            ret = scaler_dtg_d2pre_handler_control(DTG_SET, control);
            break;

        case DTG_OSD123:
            ret = scaler_dtg_osd123_handler_control(DTG_SET, control);
            break;

        case DTG_OSD4:
            ret = scaler_dtg_osd4_handler_control(DTG_SET, control);
            break;

        case DTG_OSD5:
            ret = scaler_dtg_osd5_handler_control(DTG_SET, control);
            break;

        case DTG_D2POST:
            ret = scaler_dtg_d2post_handler_control(DTG_SET, control);
            break;

        case DTG_D2POST2:
            ret = scaler_dtg_d2post2_handler_control(DTG_SET, control);
            break;

        default:
            break;
    }

    return ret;
}

int scaler_dtg_get_control(DTG_TYPE dtgType, DTG_CONTROL_INFO *control)
{
    int ret = 0;

    if(dtgType > DTG_MAX || control == NULL)
    {
        DTG_PRINT("[%s %d][Error] unknow dtg type\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(control, 0, sizeof(DTG_CONTROL_INFO));

    switch(dtgType)
    {
        case DTG_MASTER:
            ret = scaler_dtg_master_handler_control(DTG_GET, control);
            break;

        case DTG_MASTER2:
            ret = scaler_dtg_master2_handler_control(DTG_GET, control);
            break;

        case DTG_UZU:
            ret = scaler_dtg_uzu_handler_control(DTG_GET, control);
            break;

        case DTG_D2PRE:
            ret = scaler_dtg_d2pre_handler_control(DTG_GET, control);
            break;

        case DTG_OSD123:
            ret = scaler_dtg_osd123_handler_control(DTG_GET, control);
            break;

        case DTG_OSD4:
            ret = scaler_dtg_osd4_handler_control(DTG_GET, control);
            break;

        case DTG_OSD5:
            ret = scaler_dtg_osd5_handler_control(DTG_GET, control);
            break;

        case DTG_D2POST:
            ret = scaler_dtg_d2post_handler_control(DTG_GET, control);
            break;

        case DTG_D2POST2:
            ret = scaler_dtg_d2post2_handler_control(DTG_GET, control);
            break;

        default:
            break;
    }

    return ret;
}

int scaler_dtg_set_frc2fs_den_target(DTG_TYPE dtgType, unsigned int *den_target)
{
    int ret = 0;
    ppoverlay_frc_to_frame_sync_den_end_ctrl_RBUS ppoverlay_frc_to_frame_sync_den_end_ctrl_reg;
    ppoverlay_frc_to_frame_sync_den_end_ctrl2_RBUS ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg;
    ppoverlay_frc_to_frame_sync_den_end_ctrl3_RBUS ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg;

    if(dtgType > DTG_MAX || den_target == NULL)
    {
        DTG_PRINT("[%s %d][Error] unknow dtg type\n", __FUNCTION__, __LINE__);
        return -1;
    }

    switch(dtgType)
    {
        case DTG_MASTER:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl_reg);
                ppoverlay_frc_to_frame_sync_den_end_ctrl_reg.dv_den_cnt_target = *den_target;
                IoReg_Write32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl_reg, ppoverlay_frc_to_frame_sync_den_end_ctrl_reg.regValue);
            }
            break;

        case DTG_OSD123:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl_reg);
                ppoverlay_frc_to_frame_sync_den_end_ctrl_reg.osd_split_dv_den_cnt_target = *den_target;
                IoReg_Write32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl_reg, ppoverlay_frc_to_frame_sync_den_end_ctrl_reg.regValue);
            }
            break;

        case DTG_OSD4:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl3_reg);
                ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg.osd4dtg_dv_den_cnt_target = *den_target;
                IoReg_Write32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl3_reg, ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg.regValue);
            }
            break;

        case DTG_D2POST:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl2_reg);
                ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg.memcdtg_dv_den_cnt_target = *den_target;
                IoReg_Write32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl2_reg, ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg.regValue);
            }
            break;

        default:
            break;
    }

    DTG_PRINT("[%s %d]den_target:%d, dtgType:%s\n", __FUNCTION__, __LINE__, *den_target, scaler_dtg_get_dtg_type_str(dtgType));

    return ret;
}

int scaler_dtg_get_frc2fs_den_target(DTG_TYPE dtgType, unsigned int *den_target)
{
    int ret = 0;
    ppoverlay_frc_to_frame_sync_den_end_ctrl_RBUS ppoverlay_frc_to_frame_sync_den_end_ctrl_reg;
    ppoverlay_frc_to_frame_sync_den_end_ctrl2_RBUS ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg;
    ppoverlay_frc_to_frame_sync_den_end_ctrl3_RBUS ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg;

    if(dtgType > DTG_MAX || den_target == NULL)
    {
        DTG_PRINT("[%s %d][Error] unknow dtg type or den_target parameter error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(den_target, 0, sizeof(unsigned int));

    switch(dtgType)
    {
        case DTG_MASTER:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl_reg);
                *den_target = ppoverlay_frc_to_frame_sync_den_end_ctrl_reg.dv_den_cnt_target;
            }
            break;

        case DTG_OSD123:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl_reg);
                *den_target = ppoverlay_frc_to_frame_sync_den_end_ctrl_reg.osd_split_dv_den_cnt_target;
            }
            break;

        case DTG_OSD4:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl3_reg);
                *den_target = ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg.osd4dtg_dv_den_cnt_target;
            }
            break;

        case DTG_D2POST:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl2_reg);
                *den_target = ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg.memcdtg_dv_den_cnt_target;
            }
            break;

        default:
            break;
    }

    DTG_PRINT("[%s %d]den_target:%d, dtgType:%s\n", __FUNCTION__, __LINE__, *den_target, scaler_dtg_get_dtg_type_str(dtgType));

    return ret;
}

int scaler_dtg_set_frc2fs_speedup(DTG_TYPE dtgType, DTG_SPEEDUP_INFO *speedup)
{
    int ret = 0;

    ppoverlay_frc_to_frame_sync_den_end_ctrl2_RBUS ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg;
    ppoverlay_frc_to_frame_sync_den_end_ctrl3_RBUS ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg;
    ppoverlay_frc2fsync_speedup_ctrl_RBUS ppoverlay_frc2fsync_speedup_ctrl_reg;
    ppoverlay_frc_to_frame_sync_den_end_ctrl4_RBUS ppoverlay_frc_to_frame_sync_den_end_ctrl4_reg;

    if(dtgType > DTG_MAX || speedup == NULL)
    {
        DTG_PRINT("[%s %d][Error] unknow dtg type or speedup parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    switch(dtgType)
    {
        case DTG_MASTER:
            {
                ppoverlay_frc2fsync_speedup_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_FRC2fsync_speedup_ctrl_reg);
                ppoverlay_frc2fsync_speedup_ctrl_reg.speedup_dv_total = speedup->speedup_dv_total;
                ppoverlay_frc2fsync_speedup_ctrl_reg.frc2fsync_speedup_by_vtotal_en = speedup->frc2fsync_speedup_by_vtotal_en;
                IoReg_Write32(PPOVERLAY_FRC2fsync_speedup_ctrl_reg, ppoverlay_frc2fsync_speedup_ctrl_reg.regValue);
            }
            break;

        case DTG_OSD123:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl4_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl4_reg);
                ppoverlay_frc_to_frame_sync_den_end_ctrl4_reg.osddtg_speedup_dv_total = speedup->speedup_dv_total;
                ppoverlay_frc_to_frame_sync_den_end_ctrl4_reg.osddtg_speedup_en = speedup->frc2fsync_speedup_by_vtotal_en;
                IoReg_Write32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl4_reg, ppoverlay_frc_to_frame_sync_den_end_ctrl4_reg.regValue);
            }
            break;

        case DTG_OSD4:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl3_reg);
                ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg.osd4dtg_speedup_dv_total = speedup->speedup_dv_total;
                ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg.osd4dtg_speedup_en = speedup->frc2fsync_speedup_by_vtotal_en;
                IoReg_Write32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl3_reg, ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg.regValue);
            }
            break;

        case DTG_D2POST:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl2_reg);
                ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg.memcdtg_speedup_dv_total = speedup->speedup_dv_total;
                ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg.memcdtg_speedup_en = speedup->frc2fsync_speedup_by_vtotal_en;
                IoReg_Write32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl2_reg, ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg.regValue);
            }
            break;

        default:
            break;
    }

    return ret;
}

int scaler_dtg_get_frc2fs_speedup(DTG_TYPE dtgType, DTG_SPEEDUP_INFO *speedup)
{
    int ret = 0;

    ppoverlay_frc_to_frame_sync_den_end_ctrl2_RBUS ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg;
    ppoverlay_frc_to_frame_sync_den_end_ctrl3_RBUS ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg;
    ppoverlay_frc2fsync_speedup_ctrl_RBUS ppoverlay_frc2fsync_speedup_ctrl_reg;
    ppoverlay_frc_to_frame_sync_den_end_ctrl4_RBUS ppoverlay_frc_to_frame_sync_den_end_ctrl4_reg;

    if(dtgType > DTG_MAX || speedup == NULL)
    {
        DTG_PRINT("[%s %d][Error] unknow dtg type or speedup parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(speedup, 0, sizeof(DTG_SPEEDUP_INFO));

    switch(dtgType)
    {
        case DTG_MASTER:
            {
                ppoverlay_frc2fsync_speedup_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_FRC2fsync_speedup_ctrl_reg);
                speedup->speedup_dv_total = ppoverlay_frc2fsync_speedup_ctrl_reg.speedup_dv_total;
                speedup->frc2fsync_speedup_by_vtotal_en = ppoverlay_frc2fsync_speedup_ctrl_reg.frc2fsync_speedup_by_vtotal_en;
            }
            break;

        case DTG_OSD123:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl4_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl4_reg);
                speedup->speedup_dv_total = ppoverlay_frc_to_frame_sync_den_end_ctrl4_reg.osddtg_speedup_dv_total;
                speedup->frc2fsync_speedup_by_vtotal_en = ppoverlay_frc_to_frame_sync_den_end_ctrl4_reg.osddtg_speedup_en;
            }
            break;

        case DTG_OSD4:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl3_reg);
                speedup->speedup_dv_total = ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg.osd4dtg_speedup_dv_total;
                speedup->frc2fsync_speedup_by_vtotal_en = ppoverlay_frc_to_frame_sync_den_end_ctrl3_reg.osd4dtg_speedup_en;
            }
            break;

        case DTG_D2POST:
            {
                ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_frc_to_frame_sync_den_end_ctrl2_reg);
                speedup->speedup_dv_total = ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg.memcdtg_speedup_dv_total;
                speedup->frc2fsync_speedup_by_vtotal_en = ppoverlay_frc_to_frame_sync_den_end_ctrl2_reg.memcdtg_speedup_en;
            }
            break;

        default:
            break;
    }

    return ret;
}

int scaler_dtg_set_disp_ctr4(DTG_DISP_CTRL4 *dtg_disp_ctrl4)
{
    int ret = 0;

    ppoverlay_display_timing_ctrl4_RBUS ppoverlay_display_timing_ctrl4_reg;

    if(dtg_disp_ctrl4 == NULL)
    {
        DTG_PRINT("[%s %d][Error] d2post_in_sel parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_display_timing_ctrl4_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL4_reg);
    ppoverlay_display_timing_ctrl4_reg.d2post_in_sel = dtg_disp_ctrl4->d2post_in_sel;
    ppoverlay_display_timing_ctrl4_reg.dual_display_mode = dtg_disp_ctrl4->dual_display_mode;
    ppoverlay_display_timing_ctrl4_reg.d2pre_to_osd4_in_sel = dtg_disp_ctrl4->d2pre_to_osd4_in_sel;
    ppoverlay_display_timing_ctrl4_reg.d2pre_to_osd_in_sel = dtg_disp_ctrl4->d2pre_to_osd_in_sel;
    ppoverlay_display_timing_ctrl4_reg.dual_display_dsce_en = dtg_disp_ctrl4->dual_display_dsce_en;
    ppoverlay_display_timing_ctrl4_reg.dual_display_den_combine = dtg_disp_ctrl4->dual_display_den_combine;
    IoReg_Write32(PPOVERLAY_Display_Timing_CTRL4_reg, ppoverlay_display_timing_ctrl4_reg.regValue);

    return ret;
}

int scaler_dtg_get_disp_ctr4(DTG_DISP_CTRL4 *dtg_disp_ctrl4)
{
    int ret = 0;
    ppoverlay_display_timing_ctrl4_RBUS ppoverlay_display_timing_ctrl4_reg;

    if(dtg_disp_ctrl4 == NULL)
    {
        DTG_PRINT("[%s %d][Error] d2post_in_sel parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(dtg_disp_ctrl4, 0, sizeof(DTG_DISP_CTRL4));

    ppoverlay_display_timing_ctrl4_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL4_reg);

    dtg_disp_ctrl4->d2post_in_sel = ppoverlay_display_timing_ctrl4_reg.d2post_in_sel;
    dtg_disp_ctrl4->dual_display_mode = ppoverlay_display_timing_ctrl4_reg.dual_display_mode;
    dtg_disp_ctrl4->d2pre_to_osd4_in_sel = ppoverlay_display_timing_ctrl4_reg.d2pre_to_osd4_in_sel;
    dtg_disp_ctrl4->d2pre_to_osd_in_sel = ppoverlay_display_timing_ctrl4_reg.d2pre_to_osd_in_sel;
    dtg_disp_ctrl4->dual_display_dsce_en = ppoverlay_display_timing_ctrl4_reg.dual_display_dsce_en;
    dtg_disp_ctrl4->dual_display_den_combine = ppoverlay_display_timing_ctrl4_reg.dual_display_den_combine;

    return ret;
}

int scaler_dtg_get_input_frame_cnt(DTG_TYPE dtgType, unsigned int *frame_cnt)
{
    int ret = 0;
    ppoverlay_dtg_frame_cnt1_RBUS ppoverlay_dtg_frame_cnt1_reg;
    ppoverlay_dtg_frame_cnt2_RBUS ppoverlay_dtg_frame_cnt2_reg;
    ppoverlay_dtg_frame_cnt3_RBUS ppoverlay_dtg_frame_cnt3_reg;
    ppoverlay_dtg_frame_cnt4_RBUS ppoverlay_dtg_frame_cnt4_reg;

    if(dtgType > DTG_MAX || frame_cnt == NULL)
    {
        DTG_PRINT("[%s %d][Error] frame_cnt parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_dtg_frame_cnt1_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_FRAME_CNT1_reg);
    ppoverlay_dtg_frame_cnt2_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_FRAME_CNT2_reg);
    ppoverlay_dtg_frame_cnt3_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_FRAME_CNT3_reg);
    ppoverlay_dtg_frame_cnt4_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_FRAME_CNT4_reg);

    switch(dtgType)
    {
        case DTG_MASTER:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt3_reg.dtg_input_frame_cnt;
            }
            break;

        case DTG_MASTER2:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt3_reg.dtg_m2_input_frame_cnt;
            }
            break;

        case DTG_UZU:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt1_reg.uzudtg_input_frame_cnt;
            }
            break;

        case DTG_OSD123:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt2_reg.osddtg_input_frame_cnt;
            }
            break;

        case DTG_OSD4:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt4_reg.osd4dtg_input_frame_cnt;
            }
            break;

        case DTG_OSD5:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt4_reg.osd5dtg_input_frame_cnt;
            }
            break;

        case DTG_D2POST:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt1_reg.memcdtg_input_frame_cnt;
            }
            break;

        default:
            break;
    }

    return ret;
}

int scaler_dtg_get_output_frame_cnt(DTG_TYPE dtgType, unsigned int *frame_cnt)
{
    int ret = 0;
    ppoverlay_dtg_frame_cnt1_RBUS ppoverlay_dtg_frame_cnt1_reg;
    ppoverlay_dtg_frame_cnt2_RBUS ppoverlay_dtg_frame_cnt2_reg;
    ppoverlay_dtg_frame_cnt4_RBUS ppoverlay_dtg_frame_cnt4_reg;

    if(dtgType > DTG_MAX || frame_cnt == NULL)
    {
        DTG_PRINT("[%s %d][Error] frame_cnt parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_dtg_frame_cnt1_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_FRAME_CNT1_reg);
    ppoverlay_dtg_frame_cnt2_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_FRAME_CNT2_reg);
    ppoverlay_dtg_frame_cnt4_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_FRAME_CNT4_reg);

    switch(dtgType)
    {
        case DTG_MASTER:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt2_reg.dtg_output_frame_cnt;
            }
            break;

        case DTG_MASTER2:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt2_reg.dtg_m2_output_frame_cnt;
            }
            break;

        case DTG_UZU:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt1_reg.uzudtg_output_frame_cnt;
            }
            break;

        case DTG_OSD123:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt2_reg.osddtg_output_frame_cnt;
            }
            break;

        case DTG_OSD4:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt4_reg.osd4dtg_output_frame_cnt;
            }
            break;

        case DTG_OSD5:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt4_reg.osd5dtg_output_frame_cnt;
            }
            break;

        case DTG_D2POST:
            {
                *frame_cnt = ppoverlay_dtg_frame_cnt1_reg.memcdtg_output_frame_cnt;
            }
            break;

        default:
            break;
    }

    return ret;
}

static int scaler_dtg_hanler_double_buffer_ctrl(DTG_DOUBLE_BUFFER_DX dx, DTG_DOUBLE_BUFFER_OPERATOR handler, unsigned char *value)
{
    int ret = 0;

    ppoverlay_double_buffer_ctrl_RBUS ppoverlay_double_buffer_ctrl_reg;

    ppoverlay_double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);

    if(DOUBLE_BUFFER_D0 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl_reg.dmainreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl_reg.dmainreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl_reg.dmainreg_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl_reg.dmainreg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl_reg.dmainreg_dbuf_set;
        }
    }
    else if(DOUBLE_BUFFER_D7 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl_reg.dreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl_reg.dreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl_reg.dreg_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl_reg.dreg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl_reg.dreg_dbuf_set;
        }
    }
    else if(DOUBLE_BUFFER_D8 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl_reg.drm_multireg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl_reg.drm_multireg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl_reg.drm_multireg_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl_reg.drm_multireg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl_reg.drm_multireg_dbuf_set;
        }
        else if(handler == DTG_SET_SEL)
        {
            ppoverlay_double_buffer_ctrl_reg.drm_multireg_dbuf_read_sel = *value;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_SET_VSYNC_SEL)
        {
            ppoverlay_double_buffer_ctrl_reg.drm_multireg_dbuf_vsync_sel = *value;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_VSYNC_SEL)
        {
            *value = ppoverlay_double_buffer_ctrl_reg.drm_multireg_dbuf_vsync_sel;
        }
    }
    else if(DOUBLE_BUFFER_D12 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl_reg.orbit_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl_reg.orbit_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl_reg.orbit_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl_reg.orbit_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl_reg.orbit_dbuf_set;
        }
    }
    else if(DOUBLE_BUFFER_D18 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl_reg.dreg2_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl_reg.dreg2_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl_reg.dreg2_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl_reg.dreg2_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl_reg.dreg2_dbuf_set;
        }
    }

    return ret;
}

static int scaler_dtg_hanler_double_buffer_ctrl_2(DTG_DOUBLE_BUFFER_DX dx, DTG_DOUBLE_BUFFER_OPERATOR handler, unsigned char *value)
{
    int ret = 0;

    ppoverlay_double_buffer_ctrl2_RBUS ppoverlay_double_buffer_ctrl2_reg;

    ppoverlay_double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);

    if(DOUBLE_BUFFER_D10 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl2_reg.uzu2dtgreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl2_reg.uzu2dtgreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.uzu2dtgreg_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl2_reg.uzu2dtgreg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.uzu2dtgreg_dbuf_set;
        }
    }
    else if(DOUBLE_BUFFER_D6 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl2_reg.osddtgreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl2_reg.osddtgreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.osddtgreg_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl2_reg.osddtgreg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.osddtgreg_dbuf_set;
        }
        else if(handler == DTG_SET_VSYNC_SEL)
        {
            ppoverlay_double_buffer_ctrl2_reg.osddtg_dbuf_vsync_sel = *value;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_VSYNC_SEL)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.osddtg_dbuf_vsync_sel;
        }
    }
    else if(DOUBLE_BUFFER_D4 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_set;
        }
        else if(handler == DTG_SET_VSYNC_SEL)
        {
            ppoverlay_double_buffer_ctrl2_reg.uzudtg_dbuf_vsync_sel = *value;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_VSYNC_SEL)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.uzudtg_dbuf_vsync_sel;
        }
    }
    else if(DOUBLE_BUFFER_D5 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl2_reg.memcdtgreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl2_reg.memcdtgreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.memcdtgreg_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl2_reg.memc_dtgreg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.memc_dtgreg_dbuf_set;
        }
        else if(handler == DTG_SET_VSYNC_SEL)
        {
            ppoverlay_double_buffer_ctrl2_reg.memcdtg_dbuf_vsync_sel = *value;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_VSYNC_SEL)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.memcdtg_dbuf_vsync_sel;
        }
    }

    return ret;
}


static int scaler_dtg_hanler_double_buffer_ctrl_3(DTG_DOUBLE_BUFFER_DX dx, DTG_DOUBLE_BUFFER_OPERATOR handler, unsigned char *value)
{
    int ret = 0;

    ppoverlay_double_buffer_ctrl3_RBUS ppoverlay_double_buffer_ctrl3_reg;
    ppoverlay_double_buffer_ctrl2_RBUS ppoverlay_double_buffer_ctrl2_reg;

    ppoverlay_double_buffer_ctrl3_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
    ppoverlay_double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);

    if(DOUBLE_BUFFER_D16 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl3_reg.d2predtgreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl3_reg.d2predtgreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl3_reg.d2predtgreg_dbuf_en;
        }
        else if(handler == DTG_SET_SEL)
        {
            ppoverlay_double_buffer_ctrl3_reg.d2predtgreg_dbuf_read_sel = *value;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl3_reg.d2predtgreg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl3_reg.d2predtgreg_dbuf_set;
        }
        else if(handler == DTG_SET_VSYNC_SEL)
        {
            ppoverlay_double_buffer_ctrl2_reg.d2predtg_dbuf_vsync_sel = *value;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_VSYNC_SEL)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.d2predtg_dbuf_vsync_sel;
        }
    }
    else if(DOUBLE_BUFFER_D15 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl3_reg.osd4dtgreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl3_reg.osd4dtgreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl3_reg.osd4dtgreg_dbuf_en;
        }
        else if(handler == DTG_SET_SEL)
        {
            ppoverlay_double_buffer_ctrl3_reg.osd4dtgreg_dbuf_read_sel = *value;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl3_reg.osd4dtgreg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl3_reg.osd4dtgreg_dbuf_set;
        }
        else if(handler == DTG_SET_VSYNC_SEL)
        {
            ppoverlay_double_buffer_ctrl2_reg.osd4dtg_dbuf_vsync_sel = *value;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_VSYNC_SEL)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.osd4dtg_dbuf_vsync_sel;
        }
    }
    else if(DOUBLE_BUFFER_D14 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_set;
        }
    }
    else if(DOUBLE_BUFFER_D13 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl3_reg.d1bg_dtgreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl3_reg.d1bg_dtgreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl3_reg.d1bg_dtgreg_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl3_reg.d1bg_dtgreg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl3_reg.d1bg_dtgreg_dbuf_set;
        }
    }
    else if(DOUBLE_BUFFER_D9 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl3_reg.dmainreg_other_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl3_reg.dmainreg_other_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl3_reg.dmainreg_other_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl3_reg.dmainreg_other_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl3_reg.dmainreg_other_dbuf_set;
        }
    }
    else if(DOUBLE_BUFFER_D19 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl3_reg.osd5dtgreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl3_reg.osd5dtgreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl3_reg.osd5dtgreg_dbuf_en;
        }
        else if(handler == DTG_SET_SEL)
        {
            ppoverlay_double_buffer_ctrl3_reg.osd5dtgreg_dbuf_read_sel = *value;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl3_reg.osd5dtgreg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl3_reg.osd5dtgreg_dbuf_set;
        }
        else if(handler == DTG_SET_VSYNC_SEL)
        {
            ppoverlay_double_buffer_ctrl2_reg.osd5dtg_dbuf_vsync_sel = *value;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
        }
        else if(handler == DTG_GET_VSYNC_SEL)
        {
            *value = ppoverlay_double_buffer_ctrl2_reg.osd5dtg_dbuf_vsync_sel;
        }
    }

    return ret;
}

static int scaler_dtg_hanler_double_buffer_ctrl_4(DTG_DOUBLE_BUFFER_DX dx, DTG_DOUBLE_BUFFER_OPERATOR handler, unsigned char *value)
{
    int ret = 0;

    ppoverlay_double_buffer_ctrl4_RBUS ppoverlay_double_buffer_ctrl4_reg;

    ppoverlay_double_buffer_ctrl4_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL4_reg);

    if(DOUBLE_BUFFER_D3 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_double_buffer_ctrl4_reg.dsubreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL4_reg, ppoverlay_double_buffer_ctrl4_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_double_buffer_ctrl4_reg.dsubreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL4_reg, ppoverlay_double_buffer_ctrl4_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_double_buffer_ctrl4_reg.dsubreg_dbuf_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_double_buffer_ctrl4_reg.dsubreg_dbuf_set = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL4_reg, ppoverlay_double_buffer_ctrl4_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_double_buffer_ctrl4_reg.dsubreg_dbuf_set;
        }
    }

    return ret;
}

static int scaler_dtg_hanler_double_buffer_high_light_window_ctrl(DTG_DOUBLE_BUFFER_DX dx, DTG_DOUBLE_BUFFER_OPERATOR handler, unsigned char *value)
{
    int ret = 0;

    ppoverlay_highlight_window_db_ctrl_RBUS ppoverlay_highlight_window_db_ctrl_reg;

    ppoverlay_highlight_window_db_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Highlight_window_DB_CTRL_reg);

    if(DOUBLE_BUFFER_D1 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_highlight_window_db_ctrl_reg.hlw_db_en = 1;
            IoReg_Write32(PPOVERLAY_Highlight_window_DB_CTRL_reg, ppoverlay_highlight_window_db_ctrl_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_highlight_window_db_ctrl_reg.hlw_db_en = 0;
            IoReg_Write32(PPOVERLAY_Highlight_window_DB_CTRL_reg, ppoverlay_highlight_window_db_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_highlight_window_db_ctrl_reg.hlw_db_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_highlight_window_db_ctrl_reg.hlw_db_apply = 1;
            IoReg_Write32(PPOVERLAY_Highlight_window_DB_CTRL_reg, ppoverlay_highlight_window_db_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_highlight_window_db_ctrl_reg.hlw_db_apply;
        }
    }

    return ret;
}


static int scaler_dtg_hanler_double_buffer_iv2dv_ctrl(DTG_DOUBLE_BUFFER_DX dx, DTG_DOUBLE_BUFFER_OPERATOR handler, unsigned char *value)
{
    int ret = 0;

    ppoverlay_iv2dv_double_buffer_ctrl_RBUS ppoverlay_iv2dv_double_buffer_ctrl_reg;

    ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg);

    if(DOUBLE_BUFFER_D17 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv2_db_en = 1;
            IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv2_db_en = 0;
            IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv2_db_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv2_db_apply = 1;
            IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv2_db_apply;
        }
        else if(handler == DTG_SET_VSYNC_SEL)
        {
            ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv2_db_vsync_sel = *value;
            IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_VSYNC_SEL)
        {
            *value = ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv2_db_vsync_sel;
        }
    }
    else if(DOUBLE_BUFFER_D2 == dx)
    {
        if(handler == DTG_SET_EN)
        {
            ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_en = 1;
            IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_SET_DISABLE)
        {
            ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_en = 0;
            IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_DB)
        {
            *value = ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_en;
        }
        else if(handler == DTG_SET_APPLY)
        {
            ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_apply = 1;
            IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_APPLY)
        {
            *value = ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_apply;
        }
        else if(handler == DTG_SET_VSYNC_SEL)
        {
            ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_vsync_sel = *value;
            IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue);
        }
        else if(handler == DTG_GET_VSYNC_SEL)
        {
            *value = ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_vsync_sel;
        }
    }
    return ret;
}

static int scaler_dtg_handler_double_buffer(DTG_DOUBLE_BUFFER_DX dx, DTG_DOUBLE_BUFFER_OPERATOR handler, unsigned char *value)
{
    int ret = 0;

    switch(dx)
    {
        case DOUBLE_BUFFER_D0:
            ret = scaler_dtg_hanler_double_buffer_ctrl(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D1:
            ret = scaler_dtg_hanler_double_buffer_high_light_window_ctrl(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D2:
            ret = scaler_dtg_hanler_double_buffer_iv2dv_ctrl(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D3:
            ret = scaler_dtg_hanler_double_buffer_ctrl_4(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D4:
            ret = scaler_dtg_hanler_double_buffer_ctrl_2(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D5:
            ret = scaler_dtg_hanler_double_buffer_ctrl_2(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D6:
            ret = scaler_dtg_hanler_double_buffer_ctrl_2(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D7:
            ret = scaler_dtg_hanler_double_buffer_ctrl(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D8:
            ret = scaler_dtg_hanler_double_buffer_ctrl(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D9:
            ret = scaler_dtg_hanler_double_buffer_ctrl_3(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D10:
            ret = scaler_dtg_hanler_double_buffer_ctrl_2(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D11:
            break;
        case DOUBLE_BUFFER_D12:
            ret = scaler_dtg_hanler_double_buffer_ctrl(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D13:
            ret = scaler_dtg_hanler_double_buffer_ctrl_3(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D14:
            ret = scaler_dtg_hanler_double_buffer_ctrl_3(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D15:
            ret = scaler_dtg_hanler_double_buffer_ctrl_3(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D16:
            ret = scaler_dtg_hanler_double_buffer_ctrl_3(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D17:
            ret = scaler_dtg_hanler_double_buffer_iv2dv_ctrl(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D18:
            ret = scaler_dtg_hanler_double_buffer_ctrl(dx, handler, value);
            break;
        case DOUBLE_BUFFER_D19:
            ret = scaler_dtg_hanler_double_buffer_ctrl_3(dx, handler, value);
            break;
        default:
            break;
    }

    return ret;
}

int scaler_dtg_set_double_buffer_enable(DTG_DOUBLE_BUFFER_DX dx)
{
    unsigned char value = 0;

    return scaler_dtg_handler_double_buffer(dx, DTG_SET_EN, &value);
}

int scaler_dtg_set_double_buffer_disable(DTG_DOUBLE_BUFFER_DX dx)
{
    unsigned char value = 0;

    return scaler_dtg_handler_double_buffer(dx, DTG_SET_DISABLE, &value);
}

int scaler_dtg_get_double_buffer(DTG_DOUBLE_BUFFER_DX dx, unsigned char *value)
{
    if(value == NULL)
    {
        DTG_PRINT("[%s %d][Error] value parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    return scaler_dtg_handler_double_buffer(dx, DTG_GET_DB, value);
}

int scaler_dtg_set_double_buffer_apply(DTG_DOUBLE_BUFFER_DX dx)
{
    unsigned char value = 0;
    return scaler_dtg_handler_double_buffer(dx, DTG_SET_APPLY, &value);
}

int scaler_dtg_set_double_buffer_read_sel(DTG_DOUBLE_BUFFER_DX dx, unsigned char *value)
{
    return scaler_dtg_handler_double_buffer(dx, DTG_SET_SEL, value);
}

int scaler_dtg_get_double_buffer_apply(DTG_DOUBLE_BUFFER_DX dx, unsigned char *value)
{
    if(value == NULL)
    {
        DTG_PRINT("[%s %d][Error] value parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    return scaler_dtg_handler_double_buffer(dx, DTG_GET_APPLY, value);
}

int scaler_dtg_set_double_buffer_vsync_sel(DTG_DOUBLE_BUFFER_DX dx, unsigned char *value)
{
    if(value == NULL)
    {
        DTG_PRINT("[%s %d][Error] value parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    return scaler_dtg_handler_double_buffer(dx, DTG_SET_VSYNC_SEL, value);
}

int scaler_dtg_get_double_buffer_vsync_sel(DTG_DOUBLE_BUFFER_DX dx, unsigned char *value)
{
    if(value == NULL)
    {
        DTG_PRINT("[%s %d][Error] value parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    return scaler_dtg_handler_double_buffer(dx, DTG_GET_VSYNC_SEL, value);
}

int scaler_dtg_set_stage1_sm_ctrl(DTG_STAGE1_SMT_CTRL *stage1_smt)
{
    int ret = 0;

    ppoverlay_dispd_stage1_sm_ctrl_RBUS ppoverlay_dispd_stage1_sm_ctrl_reg;

    if(stage1_smt == NULL)
    {
        DTG_PRINT("[%s %d][Error] stage1_smt parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_dispd_stage1_sm_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_dispd_stage1_sm_ctrl_reg);

    ppoverlay_dispd_stage1_sm_ctrl_reg.dispd_smt1_dtg_sel = stage1_smt->dispd_smt1_dtg_sel;
    ppoverlay_dispd_stage1_sm_ctrl_reg.dispd_smt2_dtg_sel = stage1_smt->dispd_smt2_dtg_sel;
    ppoverlay_dispd_stage1_sm_ctrl_reg.dispd_smt2_protect = stage1_smt->dispd_smt2_protect;
    ppoverlay_dispd_stage1_sm_ctrl_reg.dispd_stage1_smooth_toggle_protect = stage1_smt->dispd_stage1_smooth_toggle_protect;

    IoReg_Write32(PPOVERLAY_dispd_stage1_sm_ctrl_reg, ppoverlay_dispd_stage1_sm_ctrl_reg.regValue);

    return ret;
}

int scaler_dtg_get_stage1_sm_ctrl(DTG_STAGE1_SMT_CTRL *stage1_smt)
{
    int ret = 0;

    ppoverlay_dispd_stage1_sm_ctrl_RBUS ppoverlay_dispd_stage1_sm_ctrl_reg;

    if(stage1_smt == NULL)
    {
        DTG_PRINT("[%s %d][Error] stage1_smt parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(stage1_smt, 0, sizeof(DTG_STAGE1_SMT_CTRL));

    ppoverlay_dispd_stage1_sm_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_dispd_stage1_sm_ctrl_reg);

    stage1_smt->dispd_smt1_dtg_sel = ppoverlay_dispd_stage1_sm_ctrl_reg.dispd_smt1_dtg_sel;
    stage1_smt->dispd_smt2_dtg_sel = ppoverlay_dispd_stage1_sm_ctrl_reg.dispd_smt2_dtg_sel;
    stage1_smt->dispd_smt2_protect = ppoverlay_dispd_stage1_sm_ctrl_reg.dispd_smt2_protect;
    stage1_smt->dispd_stage1_smooth_toggle_protect = ppoverlay_dispd_stage1_sm_ctrl_reg.dispd_stage1_smooth_toggle_protect;

    return ret;
}

int scaler_dtg_set_stage2_sm_ctrl(DTG_STAGE2_SMT_CTRL *stage2_smt)
{
    int ret = 0;

    ppoverlay_dispd_stage2_sm_ctrl_RBUS ppoverlay_dispd_stage2_sm_ctrl_reg;

    if(stage2_smt == NULL)
    {
        DTG_PRINT("[%s %d][Error] stage2_smt parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_dispd_stage2_sm_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_dispd_stage2_sm_ctrl_reg);

    ppoverlay_dispd_stage2_sm_ctrl_reg.dispd_smt3_dtg_sel = stage2_smt->dispd_smt3_dtg_sel;
    ppoverlay_dispd_stage2_sm_ctrl_reg.dispd_stage2_smooth_toggle_protect = stage2_smt->dispd_stage2_smooth_toggle_protect;

    IoReg_Write32(PPOVERLAY_dispd_stage2_sm_ctrl_reg, ppoverlay_dispd_stage2_sm_ctrl_reg.regValue);

    return ret;
}

int scaler_dtg_get_stage2_sm_ctrl(DTG_STAGE2_SMT_CTRL *stage2_smt)
{
    int ret = 0;

    ppoverlay_dispd_stage2_sm_ctrl_RBUS ppoverlay_dispd_stage2_sm_ctrl_reg;

    if(stage2_smt == NULL)
    {
        DTG_PRINT("[%s %d][Error] stage2_smt parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(stage2_smt, 0, sizeof(DTG_STAGE2_SMT_CTRL));

    ppoverlay_dispd_stage2_sm_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_dispd_stage2_sm_ctrl_reg);

    stage2_smt->dispd_smt3_dtg_sel = ppoverlay_dispd_stage2_sm_ctrl_reg.dispd_smt3_dtg_sel;
    stage2_smt->dispd_stage2_smooth_toggle_protect = ppoverlay_dispd_stage2_sm_ctrl_reg.dispd_stage2_smooth_toggle_protect;

    return ret;
}

int scaler_dtg_get_line_cnt(DTG_TYPE dtgType, unsigned int *line_cnt)
{
    int ret = 0;

    ppoverlay_new_meas0_linecnt_real_RBUS ppoverlay_new_meas0_linecnt_real_reg;
    ppoverlay_new_meas1_linecnt_real_RBUS ppoverlay_new_meas1_linecnt_real_reg;
    ppoverlay_new_meas2_linecnt_real_RBUS ppoverlay_new_meas2_linecnt_real_reg;
    ppoverlay_new_meas3_linecnt_real_RBUS ppoverlay_new_meas3_linecnt_real_reg;
    ppoverlay_new_meas4_linecnt_real_RBUS ppoverlay_new_meas4_linecnt_real_reg;

    ppoverlay_new_meas0_linecnt_real_reg.regValue = IoReg_Read32(PPOVERLAY_new_meas0_linecnt_real_reg);
    ppoverlay_new_meas1_linecnt_real_reg.regValue = IoReg_Read32(PPOVERLAY_new_meas1_linecnt_real_reg);
    ppoverlay_new_meas2_linecnt_real_reg.regValue = IoReg_Read32(PPOVERLAY_new_meas2_linecnt_real_reg);
    ppoverlay_new_meas3_linecnt_real_reg.regValue = IoReg_Read32(PPOVERLAY_new_meas3_linecnt_real_reg);
    ppoverlay_new_meas4_linecnt_real_reg.regValue = IoReg_Read32(PPOVERLAY_new_meas4_linecnt_real_reg);

    if(line_cnt == NULL)
    {
        DTG_PRINT("[%s %d][Error] line_cnt parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    switch (dtgType)
    {
    case DTG_MASTER:
        *line_cnt = ppoverlay_new_meas0_linecnt_real_reg.line_cnt_rt;
        break;
    case DTG_MASTER2:
        *line_cnt = ppoverlay_new_meas0_linecnt_real_reg.disp2_line_cnt_rt;
        break;
    case DTG_UZU:
        *line_cnt = ppoverlay_new_meas1_linecnt_real_reg.uzudtg_line_cnt_rt;
        break;
    case DTG_D2PRE:
        *line_cnt = ppoverlay_new_meas4_linecnt_real_reg.d2predtg_line_cnt_rt;
        break;
    case DTG_OSD123:
        *line_cnt = ppoverlay_new_meas2_linecnt_real_reg.osddtg_line_cnt_rt;
        break;
    case DTG_OSD4:
        *line_cnt = ppoverlay_new_meas3_linecnt_real_reg.osd4dtg_line_cnt_rt;
        break;
    case DTG_OSD5:
        *line_cnt = ppoverlay_new_meas3_linecnt_real_reg.osd5dtg_line_cnt_rt;
        break;
    case DTG_D2POST:
        *line_cnt = ppoverlay_new_meas1_linecnt_real_reg.memcdtg_line_cnt_rt;
        break;
    default:
        break;
    }

    return ret;
}

int scaler_dtg_get_line_cnt_pre(DTG_TYPE dtgType, unsigned int *line_cnt)
{
    int ret = 0;

    ppoverlay_new_dvs1_linecnt_pre_RBUS ppoverlay_new_dvs1_linecnt_pre_reg;
    ppoverlay_new_dvs2_linecnt_pre_RBUS ppoverlay_new_dvs2_linecnt_pre_reg;
    ppoverlay_new_dvs3_linecnt_pre_RBUS ppoverlay_new_dvs3_linecnt_pre_reg;
    ppoverlay_new_dvs4_linecnt_pre_RBUS ppoverlay_new_dvs4_linecnt_pre_reg;

    ppoverlay_new_dvs1_linecnt_pre_reg.regValue = IoReg_Read32(PPOVERLAY_new_dvs1_linecnt_pre_reg);
    ppoverlay_new_dvs2_linecnt_pre_reg.regValue = IoReg_Read32(PPOVERLAY_new_dvs2_linecnt_pre_reg);
    ppoverlay_new_dvs3_linecnt_pre_reg.regValue = IoReg_Read32(PPOVERLAY_new_dvs3_linecnt_pre_reg);
    ppoverlay_new_dvs4_linecnt_pre_reg.regValue = IoReg_Read32(PPOVERLAY_new_dvs4_linecnt_pre_reg);

    if(line_cnt == NULL)
    {
        DTG_PRINT("[%s %d][Error] line_cnt parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    switch (dtgType)
    {
    case DTG_MASTER:
        *line_cnt = ppoverlay_new_dvs1_linecnt_pre_reg.dvs_line_cnt_pre;
        break;
    case DTG_MASTER2:
        *line_cnt = ppoverlay_new_dvs2_linecnt_pre_reg.dvs2_line_cnt_pre;
        break;
    case DTG_UZU:
        *line_cnt = ppoverlay_new_dvs1_linecnt_pre_reg.uzudtg_dvs_line_cnt_pre;
        break;
    case DTG_D2PRE:
        *line_cnt = ppoverlay_new_dvs4_linecnt_pre_reg.d2predtg_dvs_line_cnt_pre;
        break;
    case DTG_OSD123:
        *line_cnt = ppoverlay_new_dvs3_linecnt_pre_reg.osddtg_dvs_line_cnt_pre;
        break;
    case DTG_OSD4:
        *line_cnt = ppoverlay_new_dvs3_linecnt_pre_reg.osd4dtg_dvs_line_cnt_pre;
        break;
    case DTG_OSD5:
        *line_cnt = ppoverlay_new_dvs4_linecnt_pre_reg.d2predtg_dvs_line_cnt_pre;
        break;
    case DTG_D2POST:
        *line_cnt = ppoverlay_new_dvs2_linecnt_pre_reg.memcdtg_dvs_line_cnt_pre;
        break;
    default:
        break;
    }

    return ret;
}

int scaler_dtg_get_line_cnt_dly(DTG_TYPE dtgType, unsigned int *line_cnt)
{
    int ret = 0;

    ppoverlay_new_meas2_linecnt_real_RBUS ppoverlay_new_meas2_linecnt_real_reg;

    ppoverlay_new_meas2_linecnt_real_reg.regValue = IoReg_Read32(PPOVERLAY_new_meas2_linecnt_real_reg);

    if(line_cnt == NULL)
    {
        DTG_PRINT("[%s %d][Error] line_cnt parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    switch (dtgType)
    {
    case DTG_UZU:
        *line_cnt = ppoverlay_new_meas2_linecnt_real_reg.uzudtg_dly_line_cnt_rt;
        break;
    default:
        break;
    }

    return ret;
}

int scaler_dtg_set_line_delay(DTG_TYPE dtgType, DTG_DELAY_INFO *dtg_delay_info)
{
    int ret = 0;
    ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;
    ppoverlay_d2predtg_control2_RBUS ppoverlay_d2predtg_control2_reg;
    ppoverlay_osddtg_control2_RBUS ppoverlay_osddtg_control2_reg;
    ppoverlay_osd4dtg_control2_RBUS ppoverlay_osd4dtg_control2_reg;
    ppoverlay_osd5dtg_control2_RBUS ppoverlay_osd5dtg_control2_reg;
    ppoverlay_memcdtg_control3_RBUS ppoverlay_memcdtg_control3_reg;
    ppoverlay_d2post2dtg_control2_RBUS ppoverlay_d2post2dtg_control2_reg;

    if(dtg_delay_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] dtg_delay_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    DTG_PRINT("[%s %d]dtgtype:%s, dtg_delay_line:%d, dtgm2uzuvs_pixel:%d\n", __FUNCTION__, __LINE__, scaler_dtg_get_dtg_type_str(dtgType), dtg_delay_info->dtg_delay_line, dtg_delay_info->dtg_delay_pixel);

    switch (dtgType)
    {
    case DTG_UZU:
        ppoverlay_uzudtg_control1_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control1_reg);
        ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line = dtg_delay_info->dtg_delay_line;
        ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_pixel = dtg_delay_info->dtg_delay_pixel;
        IoReg_Write32(PPOVERLAY_uzudtg_control1_reg, ppoverlay_uzudtg_control1_reg.regValue);
        break;
    case DTG_D2PRE:
        ppoverlay_d2predtg_control2_reg.regValue = IoReg_Read32(PPOVERLAY_D2PREDTG_CONTROL2_reg);
        ppoverlay_d2predtg_control2_reg.dtgm2d2prevs_line = dtg_delay_info->dtg_delay_line;
        ppoverlay_d2predtg_control2_reg.dtgm2d2prevs_pixel = dtg_delay_info->dtg_delay_pixel;
        IoReg_Write32(PPOVERLAY_D2PREDTG_CONTROL2_reg, ppoverlay_d2predtg_control2_reg.regValue);
        break;
    case DTG_OSD123:
        ppoverlay_osddtg_control2_reg.regValue = IoReg_Read32(PPOVERLAY_OSDDTG_CONTROL2_reg);
        ppoverlay_osddtg_control2_reg.dtgm2osdvs_line = dtg_delay_info->dtg_delay_line;
        ppoverlay_osddtg_control2_reg.dtgm2osdvs_pixel = dtg_delay_info->dtg_delay_pixel;
        IoReg_Write32(PPOVERLAY_OSDDTG_CONTROL2_reg, ppoverlay_osddtg_control2_reg.regValue);
        break;
    case DTG_OSD4:
        ppoverlay_osd4dtg_control2_reg.regValue = IoReg_Read32(PPOVERLAY_OSD4DTG_CONTROL2_reg);
        ppoverlay_osd4dtg_control2_reg.dtgm2osd4vs_line = dtg_delay_info->dtg_delay_line;
        ppoverlay_osd4dtg_control2_reg.dtgm2osd4vs_pixel = dtg_delay_info->dtg_delay_pixel;
        IoReg_Write32(PPOVERLAY_OSD4DTG_CONTROL2_reg, ppoverlay_osd4dtg_control2_reg.regValue);
        break;
    case DTG_OSD5:
        ppoverlay_osd5dtg_control2_reg.regValue = IoReg_Read32(PPOVERLAY_OSD5DTG_CONTROL2_reg);
        ppoverlay_osd5dtg_control2_reg.dtgm2osd5vs_line = dtg_delay_info->dtg_delay_line;
        ppoverlay_osd5dtg_control2_reg.dtgm2osd5vs_pixel = dtg_delay_info->dtg_delay_pixel;
        IoReg_Write32(PPOVERLAY_OSD5DTG_CONTROL2_reg, ppoverlay_osd5dtg_control2_reg.regValue);
        break;
    case DTG_D2POST:
        ppoverlay_memcdtg_control3_reg.regValue = IoReg_Read32(PPOVERLAY_MEMCDTG_CONTROL3_reg);
        ppoverlay_memcdtg_control3_reg.dtgm2goldenpostvs_line = dtg_delay_info->dtg_delay_line;
        ppoverlay_memcdtg_control3_reg.dtgm2goldenpostvs_pixel = dtg_delay_info->dtg_delay_pixel;
        IoReg_Write32(PPOVERLAY_MEMCDTG_CONTROL3_reg, ppoverlay_memcdtg_control3_reg.regValue);
        break;
    case DTG_D2POST2:
        ppoverlay_d2post2dtg_control2_reg.regValue = IoReg_Read32(PPOVERLAY_D2POST2DTG_CONTROL2_reg);
        ppoverlay_d2post2dtg_control2_reg.dtgm2d2post2vs_line = dtg_delay_info->dtg_delay_line;
        ppoverlay_d2post2dtg_control2_reg.dtgm2d2post2vs_pixel = dtg_delay_info->dtg_delay_pixel;
        IoReg_Write32(PPOVERLAY_D2POST2DTG_CONTROL2_reg, ppoverlay_d2post2dtg_control2_reg.regValue);
        break;
    default:
        break;
    }

    return ret;
}

int scaler_dtg_get_line_delay(DTG_TYPE dtgType, DTG_DELAY_INFO *dtg_delay_info)
{
    int ret = 0;

    ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;
    ppoverlay_d2predtg_control2_RBUS ppoverlay_d2predtg_control2_reg;
    ppoverlay_osddtg_control2_RBUS ppoverlay_osddtg_control2_reg;
    ppoverlay_osd4dtg_control2_RBUS ppoverlay_osd4dtg_control2_reg;
    ppoverlay_osd5dtg_control2_RBUS ppoverlay_osd5dtg_control2_reg;
    ppoverlay_memcdtg_control3_RBUS ppoverlay_memcdtg_control3_reg;
    ppoverlay_d2post2dtg_control2_RBUS ppoverlay_d2post2dtg_control2_reg;

    if(dtg_delay_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] dtg_delay_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(dtg_delay_info, 0, sizeof(DTG_DELAY_INFO));

    switch (dtgType)
    {
    case DTG_UZU:
        ppoverlay_uzudtg_control1_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control1_reg);
        dtg_delay_info->dtg_delay_line = ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line;
        dtg_delay_info->dtg_delay_pixel = ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_pixel;
        break;
    case DTG_D2PRE:
        ppoverlay_d2predtg_control2_reg.regValue = IoReg_Read32(PPOVERLAY_D2PREDTG_CONTROL2_reg);
        dtg_delay_info->dtg_delay_line = ppoverlay_d2predtg_control2_reg.dtgm2d2prevs_line;
        dtg_delay_info->dtg_delay_pixel = ppoverlay_d2predtg_control2_reg.dtgm2d2prevs_pixel;
        break;
    case DTG_OSD123:
        ppoverlay_osddtg_control2_reg.regValue = IoReg_Read32(PPOVERLAY_OSDDTG_CONTROL2_reg);
        dtg_delay_info->dtg_delay_line = ppoverlay_osddtg_control2_reg.dtgm2osdvs_line;
        dtg_delay_info->dtg_delay_pixel = ppoverlay_osddtg_control2_reg.dtgm2osdvs_pixel;
        break;
    case DTG_OSD4:
        ppoverlay_osd4dtg_control2_reg.regValue = IoReg_Read32(PPOVERLAY_OSD4DTG_CONTROL2_reg);
        dtg_delay_info->dtg_delay_line = ppoverlay_osd4dtg_control2_reg.dtgm2osd4vs_line;
        dtg_delay_info->dtg_delay_pixel = ppoverlay_osd4dtg_control2_reg.dtgm2osd4vs_pixel;
        break;
    case DTG_OSD5:
        ppoverlay_osd5dtg_control2_reg.regValue = IoReg_Read32(PPOVERLAY_OSD5DTG_CONTROL2_reg);
        dtg_delay_info->dtg_delay_line = ppoverlay_osd5dtg_control2_reg.dtgm2osd5vs_line;
        dtg_delay_info->dtg_delay_pixel = ppoverlay_osd5dtg_control2_reg.dtgm2osd5vs_pixel;
        break;
    case DTG_D2POST:
        ppoverlay_memcdtg_control3_reg.regValue = IoReg_Read32(PPOVERLAY_MEMCDTG_CONTROL3_reg);
        dtg_delay_info->dtg_delay_line = ppoverlay_memcdtg_control3_reg.dtgm2goldenpostvs_line;
        dtg_delay_info->dtg_delay_pixel = ppoverlay_memcdtg_control3_reg.dtgm2goldenpostvs_pixel;
        break;
    case DTG_D2POST2:
        ppoverlay_d2post2dtg_control2_reg.regValue = IoReg_Read32(PPOVERLAY_D2POST2DTG_CONTROL2_reg);
        dtg_delay_info->dtg_delay_line = ppoverlay_d2post2dtg_control2_reg.dtgm2d2post2vs_line;
        dtg_delay_info->dtg_delay_pixel = ppoverlay_d2post2dtg_control2_reg.dtgm2d2post2vs_pixel;
        break;
    default:
        break;
    }

    DTG_PRINT("[%s %d]dtgtype:%s, dtg_delay_line:%d, dtgm2uzuvs_pixel:%d\n", __FUNCTION__, __LINE__, scaler_dtg_get_dtg_type_str(dtgType), dtg_delay_info->dtg_delay_line, dtg_delay_info->dtg_delay_pixel);

    return ret;
}

int scaler_dtg_set_srnn_pre_delay(DTG_DELAY_INFO *dtg_delay_info)
{
    int ret = 0;

    ppoverlay_srnn_control_RBUS ppoverlay_srnn_control_reg;

    if(dtg_delay_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] dtg_delay_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_srnn_control_reg.regValue = IoReg_Read32(PPOVERLAY_srnn_control_reg);
    ppoverlay_srnn_control_reg.dtgm2mdom_pre_vs_line = dtg_delay_info->dtg_delay_line;
    ppoverlay_srnn_control_reg.dtgm2mdom_pre_vs_pixel = dtg_delay_info->dtg_delay_pixel;

    IoReg_Write32(PPOVERLAY_srnn_control_reg, ppoverlay_srnn_control_reg.regValue);

    //DTG_PRINT("[%s %d]GET Srnn pre delay, dtg_delay_line:%d, dtgm2uzuvs_pixel:%d\n", __FUNCTION__, __LINE__,  dtg_delay_info->dtg_delay_line, dtg_delay_info->dtg_delay_pixel);

    return ret;
}

int scaler_dtg_get_srnn_pre_delay(DTG_DELAY_INFO *dtg_delay_info)
{
    int ret = 0;

    ppoverlay_srnn_control_RBUS ppoverlay_srnn_control_reg;

    if(dtg_delay_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] dtg_delay_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(dtg_delay_info, 0, sizeof(DTG_DELAY_INFO));

    ppoverlay_srnn_control_reg.regValue = IoReg_Read32(PPOVERLAY_srnn_control_reg);
    dtg_delay_info->dtg_delay_line = ppoverlay_srnn_control_reg.dtgm2mdom_pre_vs_line;
    dtg_delay_info->dtg_delay_pixel = ppoverlay_srnn_control_reg.dtgm2mdom_pre_vs_pixel;

    DTG_PRINT("[%s %d]SET Srnn pre delay, dtg_delay_line:%d, dtgm2uzuvs_pixel:%d\n", __FUNCTION__, __LINE__,  dtg_delay_info->dtg_delay_line, dtg_delay_info->dtg_delay_pixel);

    return ret;
}

int scaler_dtg_get_db_latch_line(unsigned int *db_latch_line)
{
    int ret = 0;

    ppoverlay_uzudtg_control4_RBUS ppoverlay_uzudtg_control4_reg;

    if(db_latch_line == NULL)
    {
        DTG_PRINT("[%s %d][Error] db_latch_line parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_uzudtg_control4_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control4_reg);

    *db_latch_line = ppoverlay_uzudtg_control4_reg.db_latch_line;

    //DTG_PRINT("[%s %d]GET *db_latch_line:%d\n", __FUNCTION__, __LINE__,  *db_latch_line);

    return ret;
}

int scaler_dtg_set_db_latch_line(unsigned int *db_latch_line)
{
    int ret = 0;

    ppoverlay_uzudtg_control4_RBUS ppoverlay_uzudtg_control4_reg;

    if(db_latch_line == NULL)
    {
        DTG_PRINT("[%s %d][Error] db_latch_line parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_uzudtg_control4_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control4_reg);

    ppoverlay_uzudtg_control4_reg.db_latch_line = *db_latch_line;

    DTG_PRINT("[%s %d]SET *db_latch_line:%d\n", __FUNCTION__, __LINE__,  *db_latch_line);

    IoReg_Write32(PPOVERLAY_uzudtg_control4_reg, ppoverlay_uzudtg_control4_reg.regValue);

    return ret;
}

int scaler_dtg_set_uzu_control3(UZU_DTG_CONTROL3 *control3)
{
    int ret = 0;

    ppoverlay_uzudtg_control3_RBUS ppoverlay_uzudtg_control3_reg;

    if(control3 == NULL)
    {
        DTG_PRINT("[%s %d][Error] control3 parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_uzudtg_control3_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control3_reg);

    ppoverlay_uzudtg_control3_reg.s1ip_max_line_delay = control3->max_line_delay;
    ppoverlay_uzudtg_control3_reg.s1ip_line_delay_sw_mode = control3->line_delay_sw_mode;
    ppoverlay_uzudtg_control3_reg.s1ip_mode_vs_latch_en = control3->mode_vs_latch_en;
    ppoverlay_uzudtg_control3_reg.align_output_timing = control3->align_output_timing;

    DTG_PRINT("[%s %d]SET max_line_delay:%d, line_delay_sw_mode:%d, mode_vs_latch_en:%d, align_output_timing:%d\n", __FUNCTION__, __LINE__,  control3->max_line_delay, control3->line_delay_sw_mode, control3->mode_vs_latch_en, control3->align_output_timing);

    IoReg_Write32(PPOVERLAY_uzudtg_control3_reg, ppoverlay_uzudtg_control3_reg.regValue);

    return ret;
}

int scaler_dtg_get_uzu_control3(UZU_DTG_CONTROL3 *control3)
{
    int ret = 0;

    ppoverlay_uzudtg_control3_RBUS ppoverlay_uzudtg_control3_reg;

    if(control3 == NULL)
    {
        DTG_PRINT("[%s %d][Error] control3 parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(control3, 0, sizeof(UZU_DTG_CONTROL3));

    ppoverlay_uzudtg_control3_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control3_reg);

    control3->max_line_delay = ppoverlay_uzudtg_control3_reg.s1ip_max_line_delay;
    control3->line_delay_sw_mode = ppoverlay_uzudtg_control3_reg.s1ip_line_delay_sw_mode;
    control3->mode_vs_latch_en = ppoverlay_uzudtg_control3_reg.s1ip_mode_vs_latch_en;
    control3->align_output_timing = ppoverlay_uzudtg_control3_reg.align_output_timing;

    //DTG_PRINT("[%s %d]GET max_line_delay:%d, line_delay_sw_mode:%d, mode_vs_latch_en:%d, align_output_timing:%d\n", __FUNCTION__, __LINE__,  control3->max_line_delay, control3->line_delay_sw_mode, control3->mode_vs_latch_en, control3->align_output_timing);

    return ret;
}

int scaler_dtg_set_time_borrow(TIME_BORROW_INFO *time_borrow)
{
    int ret = 0;

    ppoverlay_uzudtg_stage1_time_borrow_RBUS ppoverlay_uzudtg_stage1_time_borrow_reg;

    if(time_borrow == NULL)
    {
        DTG_PRINT("[%s %d][Error] time_borrow parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_uzudtg_stage1_time_borrow_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_stage1_time_borrow_reg);

    ppoverlay_uzudtg_stage1_time_borrow_reg.stage1_time_borrow_sw_mode = time_borrow->time_borrow_sw_mode;
    ppoverlay_uzudtg_stage1_time_borrow_reg.stage1_time_borrow_v = time_borrow->time_borrow_v;
    ppoverlay_uzudtg_stage1_time_borrow_reg.stage1_time_borrow_h = time_borrow->time_borrow_h;

    DTG_PRINT("[%s %d]SET time_borrow_sw_mode:%d, time_borrow_v:%d, time_borrow_h:%d\n", __FUNCTION__, __LINE__,  time_borrow->time_borrow_sw_mode, time_borrow->time_borrow_v, time_borrow->time_borrow_h);

    IoReg_Write32(PPOVERLAY_uzudtg_stage1_time_borrow_reg, ppoverlay_uzudtg_stage1_time_borrow_reg.regValue);

    return ret;
}

int scaler_dtg_get_time_borrow(TIME_BORROW_INFO *time_borrow)
{
    int ret = 0;

    ppoverlay_uzudtg_stage1_time_borrow_RBUS ppoverlay_uzudtg_stage1_time_borrow_reg;

    if(time_borrow == NULL)
    {
        DTG_PRINT("[%s %d][Error] time_borrow parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(time_borrow, 0, sizeof(TIME_BORROW_INFO));

    ppoverlay_uzudtg_stage1_time_borrow_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_stage1_time_borrow_reg);

    time_borrow->time_borrow_sw_mode = ppoverlay_uzudtg_stage1_time_borrow_reg.stage1_time_borrow_sw_mode;
    time_borrow->time_borrow_v = ppoverlay_uzudtg_stage1_time_borrow_reg.stage1_time_borrow_v;
    time_borrow->time_borrow_h = ppoverlay_uzudtg_stage1_time_borrow_reg.stage1_time_borrow_h;

    //DTG_PRINT("[%s %d]GET time_borrow_sw_mode:%d, time_borrow_v:%d, time_borrow_h:%d\n", __FUNCTION__, __LINE__,  time_borrow->time_borrow_sw_mode, time_borrow->time_borrow_v, time_borrow->time_borrow_h);

    return ret;
}

int scaler_dtg_set_main_den_start_end(H_V_START_END_INFO *start_end_info)
{
    int ret = 0;

    ppoverlay_main_den_v_start_end_RBUS ppoverlay_main_den_v_start_end_reg;
    ppoverlay_main_den_h_start_end_RBUS ppoverlay_main_den_h_start_end_reg;

    if(start_end_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] start_end_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_main_den_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_DEN_H_Start_End_reg);
    ppoverlay_main_den_h_start_end_reg.mh_den_sta = start_end_info->h_start;
    ppoverlay_main_den_h_start_end_reg.mh_den_end = start_end_info->h_end;

    ppoverlay_main_den_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_DEN_V_Start_End_reg);
    ppoverlay_main_den_v_start_end_reg.mv_den_sta = start_end_info->v_start;
    ppoverlay_main_den_v_start_end_reg.mv_den_end = start_end_info->v_end;

    IoReg_Write32(PPOVERLAY_MAIN_DEN_V_Start_End_reg, ppoverlay_main_den_v_start_end_reg.regValue);
    IoReg_Write32(PPOVERLAY_MAIN_DEN_H_Start_End_reg, ppoverlay_main_den_h_start_end_reg.regValue);

    DTG_PRINT("[%s %d]SET h_start:%d, h_end:%d, v_start:%d, v_end:%d\n", __FUNCTION__, __LINE__,  start_end_info->h_start, start_end_info->h_end, start_end_info->v_start, start_end_info->v_end);

    return ret;
}

int scaler_dtg_get_main_den_start_end(H_V_START_END_INFO *start_end_info)
{
    int ret = 0;

    ppoverlay_main_den_v_start_end_RBUS ppoverlay_main_den_v_start_end_reg;
    ppoverlay_main_den_h_start_end_RBUS ppoverlay_main_den_h_start_end_reg;

    if(start_end_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] start_end_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(start_end_info, 0, sizeof(H_V_START_END_INFO));

    ppoverlay_main_den_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_DEN_H_Start_End_reg);
    start_end_info->h_start = ppoverlay_main_den_h_start_end_reg.mh_den_sta;
    start_end_info->h_end = ppoverlay_main_den_h_start_end_reg.mh_den_end;

    ppoverlay_main_den_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_DEN_V_Start_End_reg);
    start_end_info->v_start = ppoverlay_main_den_v_start_end_reg.mv_den_sta;
    start_end_info->v_end = ppoverlay_main_den_v_start_end_reg.mv_den_end;

    //DTG_PRINT("[%s %d]GET h_start:%d, h_end:%d, v_start:%d, v_end:%d\n", __FUNCTION__, __LINE__,  start_end_info->h_start, start_end_info->h_end, start_end_info->v_start, start_end_info->v_end);

    return ret;
}

int scaler_dtg_set_main_background_start_end(H_V_START_END_INFO *start_end_info)
{
    int ret = 0;

    ppoverlay_main_background_h_start_end_RBUS ppoverlay_main_background_h_start_end_reg;
    ppoverlay_main_background_v_start_end_RBUS ppoverlay_main_background_v_start_end_reg;

    if(start_end_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] start_end_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_main_background_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Background_H_Start_End_reg);
    ppoverlay_main_background_h_start_end_reg.mh_bg_sta = start_end_info->h_start;
    ppoverlay_main_background_h_start_end_reg.mh_bg_end = start_end_info->h_end;

    ppoverlay_main_background_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Background_V_Start_End_reg);
    ppoverlay_main_background_v_start_end_reg.mv_bg_sta = start_end_info->v_start;
    ppoverlay_main_background_v_start_end_reg.mv_bg_end = start_end_info->v_end;


    IoReg_Write32(PPOVERLAY_MAIN_Background_H_Start_End_reg, ppoverlay_main_background_h_start_end_reg.regValue);
    IoReg_Write32(PPOVERLAY_MAIN_Background_V_Start_End_reg, ppoverlay_main_background_v_start_end_reg.regValue);

    DTG_PRINT("[%s %d]SET h_start:%d, h_end:%d, v_start:%d, v_end:%d\n", __FUNCTION__, __LINE__,  start_end_info->h_start, start_end_info->h_end, start_end_info->v_start, start_end_info->v_end);

    return ret;
}

int scaler_dtg_get_main_background_start_end(H_V_START_END_INFO *start_end_info)
{
    int ret = 0;

    ppoverlay_main_background_h_start_end_RBUS ppoverlay_main_background_h_start_end_reg;
    ppoverlay_main_background_v_start_end_RBUS ppoverlay_main_background_v_start_end_reg;

    if(start_end_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] start_end_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(start_end_info, 0, sizeof(H_V_START_END_INFO));

    ppoverlay_main_background_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Background_H_Start_End_reg);
    start_end_info->h_start = ppoverlay_main_background_h_start_end_reg.mh_bg_sta;
    start_end_info->h_end = ppoverlay_main_background_h_start_end_reg.mh_bg_end;

    ppoverlay_main_background_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Background_V_Start_End_reg);
    start_end_info->v_start = ppoverlay_main_background_v_start_end_reg.mv_bg_sta;
    start_end_info->v_end = ppoverlay_main_background_v_start_end_reg.mv_bg_end;

    //DTG_PRINT("[%s %d]GET h_start:%d, h_end:%d, v_start:%d, v_end:%d\n", __FUNCTION__, __LINE__,  start_end_info->h_start, start_end_info->h_end, start_end_info->v_start, start_end_info->v_end);

    return ret;
}

int scaler_dtg_set_main_active_start_end(H_V_START_END_INFO *start_end_info)
{
    int ret = 0;

    ppoverlay_main_active_h_start_end_RBUS ppoverlay_main_active_h_start_end_reg;
    ppoverlay_main_active_v_start_end_RBUS ppoverlay_main_active_v_start_end_reg;

    if(start_end_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] start_end_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_main_active_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Active_H_Start_End_reg);
    ppoverlay_main_active_h_start_end_reg.mh_act_sta = start_end_info->h_start;
    ppoverlay_main_active_h_start_end_reg.mh_act_end = start_end_info->h_end;

    ppoverlay_main_active_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Active_V_Start_End_reg);
    ppoverlay_main_active_v_start_end_reg.mv_act_sta = start_end_info->v_start;
    ppoverlay_main_active_v_start_end_reg.mv_act_end = start_end_info->v_end;


    IoReg_Write32(PPOVERLAY_MAIN_Active_H_Start_End_reg, ppoverlay_main_active_h_start_end_reg.regValue);
    IoReg_Write32(PPOVERLAY_MAIN_Active_V_Start_End_reg, ppoverlay_main_active_v_start_end_reg.regValue);

    DTG_PRINT("[%s %d]SET h_start:%d, h_end:%d, v_start:%d, v_end:%d\n", __FUNCTION__, __LINE__,  start_end_info->h_start, start_end_info->h_end, start_end_info->v_start, start_end_info->v_end);

    return ret;
}

int scaler_dtg_get_main_active_start_end(H_V_START_END_INFO *start_end_info)
{
    int ret = 0;

    ppoverlay_main_active_h_start_end_RBUS ppoverlay_main_active_h_start_end_reg;
    ppoverlay_main_active_v_start_end_RBUS ppoverlay_main_active_v_start_end_reg;

    if(start_end_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] start_end_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(start_end_info, 0, sizeof(H_V_START_END_INFO));

    ppoverlay_main_active_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Active_H_Start_End_reg);
    start_end_info->h_start = ppoverlay_main_active_h_start_end_reg.mh_act_sta;
    start_end_info->h_end = ppoverlay_main_active_h_start_end_reg.mh_act_end;

    ppoverlay_main_active_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Active_V_Start_End_reg);
    start_end_info->v_start = ppoverlay_main_active_v_start_end_reg.mv_act_sta;
    start_end_info->v_end = ppoverlay_main_active_v_start_end_reg.mv_act_end;

    //DTG_PRINT("[%s %d]GET h_start:%d, h_end:%d, v_start:%d, v_end:%d\n", __FUNCTION__, __LINE__,  start_end_info->h_start, start_end_info->h_end, start_end_info->v_start, start_end_info->v_end);

    return ret;
}

//SUB
int scaler_dtg_set_sub_den_start_end(H_V_START_END_INFO *start_end_info)
{
    int ret = 0;

    ppoverlay_sub_den_h_start_width_RBUS ppoverlay_sub_den_h_start_width_reg;
    ppoverlay_sub_den_v_start_length_RBUS ppoverlay_sub_den_v_start_length_reg;

    if(start_end_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] start_end_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_sub_den_h_start_width_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_DEN_H_Start_Width_reg);
    ppoverlay_sub_den_h_start_width_reg.sh_den_sta = start_end_info->h_start;
    ppoverlay_sub_den_h_start_width_reg.sh_den_width = start_end_info->h_width;

    ppoverlay_sub_den_v_start_length_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_DEN_V_Start_Length_reg);
    ppoverlay_sub_den_v_start_length_reg.sv_den_sta = start_end_info->v_start;
    ppoverlay_sub_den_v_start_length_reg.sv_den_length = start_end_info->v_len;

    IoReg_Write32(PPOVERLAY_SUB_DEN_H_Start_Width_reg, ppoverlay_sub_den_h_start_width_reg.regValue);
    IoReg_Write32(PPOVERLAY_SUB_DEN_V_Start_Length_reg, ppoverlay_sub_den_v_start_length_reg.regValue);

    DTG_PRINT("[%s %d]SET h_start:%d, h_end:%d, v_start:%d, v_end:%d\n", __FUNCTION__, __LINE__,  start_end_info->h_start, start_end_info->h_width, start_end_info->v_start, start_end_info->v_len);

    return ret;
}

int scaler_dtg_get_sub_den_start_end(H_V_START_END_INFO *start_end_info)
{
    int ret = 0;

    ppoverlay_sub_den_h_start_width_RBUS ppoverlay_sub_den_h_start_width_reg;
    ppoverlay_sub_den_v_start_length_RBUS ppoverlay_sub_den_v_start_length_reg;

    if(start_end_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] start_end_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(start_end_info, 0, sizeof(H_V_START_END_INFO));

    ppoverlay_sub_den_h_start_width_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_DEN_H_Start_Width_reg);
    start_end_info->h_start = ppoverlay_sub_den_h_start_width_reg.sh_den_sta;
    start_end_info->h_width = ppoverlay_sub_den_h_start_width_reg.sh_den_width;

    ppoverlay_sub_den_v_start_length_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_DEN_V_Start_Length_reg);
    start_end_info->v_start = ppoverlay_sub_den_v_start_length_reg.sv_den_sta;
    start_end_info->v_len = ppoverlay_sub_den_v_start_length_reg.sv_den_length;

    //DTG_PRINT("[%s %d]GET h_start:%d, h_end:%d, v_start:%d, v_end:%d\n", __FUNCTION__, __LINE__,  start_end_info->h_start, start_end_info->h_width, start_end_info->v_start, start_end_info->v_len);

    return ret;
}

int scaler_dtg_set_sub_background_start_end(H_V_START_END_INFO *start_end_info)
{
    int ret = 0;

    ppoverlay_sub_background_h_start_end_RBUS ppoverlay_sub_background_h_start_end_reg;
    ppoverlay_sub_background_v_start_end_RBUS ppoverlay_sub_background_v_start_end_reg;

    if(start_end_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] start_end_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_sub_background_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Background_H_Start_End_reg);
    ppoverlay_sub_background_h_start_end_reg.sh_bg_sta = start_end_info->h_start;
    ppoverlay_sub_background_h_start_end_reg.sh_bg_end = start_end_info->h_end;

    ppoverlay_sub_background_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Background_V_Start_End_reg);
    ppoverlay_sub_background_v_start_end_reg.sv_bg_sta = start_end_info->v_start;
    ppoverlay_sub_background_v_start_end_reg.sv_bg_end = start_end_info->v_end;


    IoReg_Write32(PPOVERLAY_SUB_Background_H_Start_End_reg, ppoverlay_sub_background_h_start_end_reg.regValue);
    IoReg_Write32(PPOVERLAY_SUB_Background_V_Start_End_reg, ppoverlay_sub_background_v_start_end_reg.regValue);

    DTG_PRINT("[%s %d]SET h_start:%d, h_end:%d, v_start:%d, v_end:%d\n", __FUNCTION__, __LINE__,  start_end_info->h_start, start_end_info->h_end, start_end_info->v_start, start_end_info->v_end);

    return ret;
}

int scaler_dtg_get_sub_background_start_end(H_V_START_END_INFO *start_end_info)
{
    int ret = 0;

    ppoverlay_sub_background_h_start_end_RBUS ppoverlay_sub_background_h_start_end_reg;
    ppoverlay_sub_background_v_start_end_RBUS ppoverlay_sub_background_v_start_end_reg;

    if(start_end_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] start_end_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(start_end_info, 0, sizeof(H_V_START_END_INFO));

    ppoverlay_sub_background_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Background_H_Start_End_reg);
    start_end_info->h_start = ppoverlay_sub_background_h_start_end_reg.sh_bg_sta;
    start_end_info->h_end = ppoverlay_sub_background_h_start_end_reg.sh_bg_end;

    ppoverlay_sub_background_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Background_V_Start_End_reg);
    start_end_info->v_start = ppoverlay_sub_background_v_start_end_reg.sv_bg_sta;
    start_end_info->v_end = ppoverlay_sub_background_v_start_end_reg.sv_bg_end;

    //DTG_PRINT("[%s %d]GET h_start:%d, h_end:%d, v_start:%d, v_end:%d\n", __FUNCTION__, __LINE__,  start_end_info->h_start, start_end_info->h_end, start_end_info->v_start, start_end_info->v_end);

    return ret;
}

int scaler_dtg_set_sub_active_start_end(H_V_START_END_INFO *start_end_info)
{
    int ret = 0;

    ppoverlay_sub_active_h_start_end_RBUS ppoverlay_sub_active_h_start_end_reg;
    ppoverlay_sub_active_v_start_end_RBUS ppoverlay_sub_active_v_start_end_reg;

    if(start_end_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] start_end_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_sub_active_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Active_H_Start_End_reg);
    ppoverlay_sub_active_h_start_end_reg.sh_act_sta = start_end_info->h_start;
    ppoverlay_sub_active_h_start_end_reg.sh_act_end = start_end_info->h_end;

    ppoverlay_sub_active_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Active_V_Start_End_reg);
    ppoverlay_sub_active_v_start_end_reg.sv_act_sta = start_end_info->v_start;
    ppoverlay_sub_active_v_start_end_reg.sv_act_end = start_end_info->v_end;


    IoReg_Write32(PPOVERLAY_SUB_Active_H_Start_End_reg, ppoverlay_sub_active_h_start_end_reg.regValue);
    IoReg_Write32(PPOVERLAY_SUB_Active_V_Start_End_reg, ppoverlay_sub_active_v_start_end_reg.regValue);

    DTG_PRINT("[%s %d]SET h_start:%d, h_end:%d, v_start:%d, v_end:%d\n", __FUNCTION__, __LINE__,  start_end_info->h_start, start_end_info->h_end, start_end_info->v_start, start_end_info->v_end);

    return ret;
}

int scaler_dtg_get_sub_active_start_end(H_V_START_END_INFO *start_end_info)
{
    int ret = 0;

    ppoverlay_sub_active_h_start_end_RBUS ppoverlay_sub_active_h_start_end_reg;
    ppoverlay_sub_active_v_start_end_RBUS ppoverlay_sub_active_v_start_end_reg;

    if(start_end_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] start_end_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(start_end_info, 0, sizeof(H_V_START_END_INFO));

    ppoverlay_sub_active_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Active_H_Start_End_reg);
    start_end_info->h_start = ppoverlay_sub_active_h_start_end_reg.sh_act_sta;
    start_end_info->h_end = ppoverlay_sub_active_h_start_end_reg.sh_act_end;

    ppoverlay_sub_active_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Active_V_Start_End_reg);
    start_end_info->v_start = ppoverlay_sub_active_v_start_end_reg.sv_act_sta;
    start_end_info->v_end = ppoverlay_sub_active_v_start_end_reg.sv_act_end;

    return ret;
}

int scaler_dtg_set_iv2dv(DTG_TYPE dtgType, IV2DV_DELAY_INFO *iv2dv_info)
{
    int ret = 0;

    ppoverlay_fs_iv_dv_fine_tuning2_RBUS ppoverlay_fs_iv_dv_fine_tuning2_reg;
    ppoverlay_dctl_iv2dv_ihs_ctrl_RBUS ppoverlay_dctl_iv2dv_ihs_ctrl_reg;
    ppoverlay_fs_iv_dv_fine_tuning5_RBUS ppoverlay_fs_iv_dv_fine_tuning5_reg;

    ppoverlay_fs_iv2_dv_fine_tuning2_RBUS ppoverlay_fs_iv2_dv_fine_tuning2_reg;
    ppoverlay_dctl_iv2dv2_ihs_ctrl_RBUS ppoverlay_dctl_iv2dv2_ihs_ctrl_reg;
    ppoverlay_fs_iv2_dv_fine_tuning5_RBUS ppoverlay_fs_iv2_dv_fine_tuning5_reg;

    if(iv2dv_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] iv2dv_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }


    if(dtgType == DTG_MASTER)
    {
        ppoverlay_fs_iv_dv_fine_tuning2_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning2_reg);
        ppoverlay_fs_iv_dv_fine_tuning2_reg.iv2dv_xclk_en = iv2dv_info->iv2dv_xclk_en;
        ppoverlay_fs_iv_dv_fine_tuning2_reg.iv2dv_pixel = iv2dv_info->iv2dv_pixel;
        ppoverlay_fs_iv_dv_fine_tuning2_reg.iv2dv_pixel2 = iv2dv_info->iv2dv_pixel_2;
        IoReg_Write32(PPOVERLAY_FS_IV_DV_Fine_Tuning2_reg, ppoverlay_fs_iv_dv_fine_tuning2_reg.regValue);

        ppoverlay_fs_iv_dv_fine_tuning5_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg);
        ppoverlay_fs_iv_dv_fine_tuning5_reg.iv2dv_line = iv2dv_info->iv2dv_line;
        ppoverlay_fs_iv_dv_fine_tuning5_reg.iv2dv_line_2 = iv2dv_info->iv2dv_line_2;
        IoReg_Write32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg, ppoverlay_fs_iv_dv_fine_tuning5_reg.regValue);

        ppoverlay_dctl_iv2dv_ihs_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_dctl_iv2dv_ihs_ctrl_reg);
        ppoverlay_dctl_iv2dv_ihs_ctrl_reg.iv2dv_ih_slf_gen_en = iv2dv_info->iv2dv_ih_slf_gen_en;
        ppoverlay_dctl_iv2dv_ihs_ctrl_reg.iv2dv_ih_slf_width = iv2dv_info->iv2dv_ih_slf_width;
        IoReg_Write32(PPOVERLAY_dctl_iv2dv_ihs_ctrl_reg, ppoverlay_dctl_iv2dv_ihs_ctrl_reg.regValue);
    }
    else if(dtgType == DTG_MASTER2)
    {
        ppoverlay_fs_iv2_dv_fine_tuning2_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV2_DV_Fine_Tuning2_reg);
        ppoverlay_fs_iv2_dv_fine_tuning2_reg.iv2dv2_xclk_en = iv2dv_info->iv2dv_xclk_en;
        ppoverlay_fs_iv2_dv_fine_tuning2_reg.iv2dv2_pixel = iv2dv_info->iv2dv_pixel;
        ppoverlay_fs_iv2_dv_fine_tuning2_reg.iv2dv2_pixel2 = iv2dv_info->iv2dv_pixel_2;
        IoReg_Write32(PPOVERLAY_FS_IV_DV_Fine_Tuning2_reg, ppoverlay_fs_iv2_dv_fine_tuning2_reg.regValue);

        ppoverlay_fs_iv2_dv_fine_tuning5_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV2_DV_Fine_Tuning5_reg);
        ppoverlay_fs_iv2_dv_fine_tuning5_reg.iv2dv2_line = iv2dv_info->iv2dv_line;
        ppoverlay_fs_iv2_dv_fine_tuning5_reg.iv2dv2_line_2 = iv2dv_info->iv2dv_line_2;
        IoReg_Write32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg, ppoverlay_fs_iv2_dv_fine_tuning5_reg.regValue);

        ppoverlay_dctl_iv2dv2_ihs_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_dctl_iv2dv2_ihs_ctrl_reg);
        ppoverlay_dctl_iv2dv2_ihs_ctrl_reg.iv2dv2_ih_slf_gen_en = iv2dv_info->iv2dv_ih_slf_gen_en;
        ppoverlay_dctl_iv2dv2_ihs_ctrl_reg.iv2dv2_ih_slf_width = iv2dv_info->iv2dv_ih_slf_width;
        IoReg_Write32(PPOVERLAY_dctl_iv2dv_ihs_ctrl_reg, ppoverlay_dctl_iv2dv2_ihs_ctrl_reg.regValue);
    }
    else
    {
        DTG_PRINT("[%s %d][Error] dtg type error dtgType:%d\n", __FUNCTION__, __LINE__, dtgType);
    }

    return ret;
}

int scaler_dtg_get_iv2dv(DTG_TYPE dtgType, IV2DV_DELAY_INFO *iv2dv_info)
{
    int ret = 0;

    ppoverlay_fs_iv_dv_fine_tuning2_RBUS ppoverlay_fs_iv_dv_fine_tuning2_reg;
    ppoverlay_dctl_iv2dv_ihs_ctrl_RBUS ppoverlay_dctl_iv2dv_ihs_ctrl_reg;
    ppoverlay_fs_iv_dv_fine_tuning5_RBUS ppoverlay_fs_iv_dv_fine_tuning5_reg;

    ppoverlay_fs_iv2_dv_fine_tuning2_RBUS ppoverlay_fs_iv2_dv_fine_tuning2_reg;
    ppoverlay_dctl_iv2dv2_ihs_ctrl_RBUS ppoverlay_dctl_iv2dv2_ihs_ctrl_reg;
    ppoverlay_fs_iv2_dv_fine_tuning5_RBUS ppoverlay_fs_iv2_dv_fine_tuning5_reg;

    if(iv2dv_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] iv2dv_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(iv2dv_info, 0, sizeof(IV2DV_DELAY_INFO));

    if(dtgType == DTG_MASTER)
    {
        ppoverlay_fs_iv_dv_fine_tuning2_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning2_reg);
        iv2dv_info->iv2dv_xclk_en = ppoverlay_fs_iv_dv_fine_tuning2_reg.iv2dv_xclk_en;
        iv2dv_info->iv2dv_pixel = ppoverlay_fs_iv_dv_fine_tuning2_reg.iv2dv_pixel;
        iv2dv_info->iv2dv_pixel_2 = ppoverlay_fs_iv_dv_fine_tuning2_reg.iv2dv_pixel2;

        ppoverlay_fs_iv_dv_fine_tuning5_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg);
        iv2dv_info->iv2dv_line = ppoverlay_fs_iv_dv_fine_tuning5_reg.iv2dv_line;
        iv2dv_info->iv2dv_line_2 = ppoverlay_fs_iv_dv_fine_tuning5_reg.iv2dv_line_2;

        ppoverlay_dctl_iv2dv_ihs_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_dctl_iv2dv_ihs_ctrl_reg);
        iv2dv_info->iv2dv_ih_slf_gen_en = ppoverlay_dctl_iv2dv_ihs_ctrl_reg.iv2dv_ih_slf_gen_en;
        iv2dv_info->iv2dv_ih_slf_width =  ppoverlay_dctl_iv2dv_ihs_ctrl_reg.iv2dv_ih_slf_width;
    }
    else if(dtgType == DTG_MASTER2)
    {
        ppoverlay_fs_iv2_dv_fine_tuning2_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV2_DV_Fine_Tuning2_reg);
        iv2dv_info->iv2dv_xclk_en = ppoverlay_fs_iv2_dv_fine_tuning2_reg.iv2dv2_xclk_en;
        iv2dv_info->iv2dv_pixel = ppoverlay_fs_iv2_dv_fine_tuning2_reg.iv2dv2_pixel;
        iv2dv_info->iv2dv_pixel_2 = ppoverlay_fs_iv2_dv_fine_tuning2_reg.iv2dv2_pixel2;

        ppoverlay_fs_iv2_dv_fine_tuning5_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV2_DV_Fine_Tuning5_reg);
        iv2dv_info->iv2dv_line = ppoverlay_fs_iv2_dv_fine_tuning5_reg.iv2dv2_line;
        iv2dv_info->iv2dv_line_2 = ppoverlay_fs_iv2_dv_fine_tuning5_reg.iv2dv2_line_2;

        ppoverlay_dctl_iv2dv2_ihs_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_dctl_iv2dv2_ihs_ctrl_reg);
        iv2dv_info->iv2dv_ih_slf_gen_en = ppoverlay_dctl_iv2dv2_ihs_ctrl_reg.iv2dv2_ih_slf_gen_en;
        iv2dv_info->iv2dv_ih_slf_width = ppoverlay_dctl_iv2dv2_ihs_ctrl_reg.iv2dv2_ih_slf_width;
    }
    else
    {
        DTG_PRINT("[%s %d][Error] dtg type error dtgType:%d\n", __FUNCTION__, __LINE__, dtgType);
    }

    return ret;
}

int scaler_dtg_set_abonormal_dvs(DTG_TYPE dtgType, ABNORMAL_DVS_INFO *abnormal_info)
{
    int ret = 0;
    ppoverlay_dvs_abnormal_control_RBUS ppoverlay_dvs_abnormal_control_reg;
    ppoverlay_dvs2_abnormal_control_RBUS ppoverlay_dvs2_abnormal_control_reg;

    if(abnormal_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] abnormal_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    if(dtgType == DTG_MASTER)
    {
        ppoverlay_dvs_abnormal_control_reg.regValue = IoReg_Read32(PPOVERLAY_DVS_Abnormal_Control_reg);
        ppoverlay_dvs_abnormal_control_reg.dvs_abnormal_long_en = abnormal_info->abnormal_long_en;
        ppoverlay_dvs_abnormal_control_reg.dvs_abnormal_measure_en = abnormal_info->abnormal_measure_en;
        ppoverlay_dvs_abnormal_control_reg.dvs_abnormal_short_en = abnormal_info->abnormal_short_en;
        ppoverlay_dvs_abnormal_control_reg.dvs_short_line_th = abnormal_info->short_line_th;
        ppoverlay_dvs_abnormal_control_reg.dvs_long_line_th = abnormal_info->long_line_th;

        IoReg_Write32(PPOVERLAY_DVS_Abnormal_Control_reg, ppoverlay_dvs_abnormal_control_reg.regValue);
    }
    else if(dtgType == DTG_MASTER2)
    {
        ppoverlay_dvs2_abnormal_control_reg.regValue = IoReg_Read32(PPOVERLAY_DVS2_Abnormal_Control_reg);
        ppoverlay_dvs2_abnormal_control_reg.dvs2_abnormal_long_en = abnormal_info->abnormal_long_en;
        ppoverlay_dvs2_abnormal_control_reg.dvs2_abnormal_measure_en = abnormal_info->abnormal_measure_en;
        ppoverlay_dvs2_abnormal_control_reg.dvs2_abnormal_short_en = abnormal_info->abnormal_short_en;
        ppoverlay_dvs2_abnormal_control_reg.dvs2_short_line_th = abnormal_info->short_line_th;
        ppoverlay_dvs2_abnormal_control_reg.dvs2_long_line_th = abnormal_info->long_line_th;

        IoReg_Write32(PPOVERLAY_DVS2_Abnormal_Control_reg, ppoverlay_dvs2_abnormal_control_reg.regValue);
    }
    else
    {
        DTG_PRINT("[%s %d][Error] dtg type error dtgType:%d\n", __FUNCTION__, __LINE__, dtgType);
    }

    DTG_PRINT("[%s %d]SET abnormal_long_en:%d, abnormal_measure_en:%d, abnormal_short_en:%d, short_line_th:%d, long_line_th:%d\n", __FUNCTION__, __LINE__, abnormal_info->abnormal_long_en, abnormal_info->abnormal_measure_en,
            abnormal_info->abnormal_short_en, abnormal_info->short_line_th, abnormal_info->long_line_th);

    return ret;
}

int scaler_dtg_get_abonormal_dvs(DTG_TYPE dtgType, ABNORMAL_DVS_INFO *abnormal_info)
{
    int ret = 0;

    ppoverlay_dvs_abnormal_control_RBUS ppoverlay_dvs_abnormal_control_reg;
    ppoverlay_dvs2_abnormal_control_RBUS ppoverlay_dvs2_abnormal_control_reg;

    if(abnormal_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] abnormal_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(abnormal_info, 0, sizeof(ABNORMAL_DVS_INFO));

    if(dtgType == DTG_MASTER)
    {
        ppoverlay_dvs_abnormal_control_reg.regValue = IoReg_Read32(PPOVERLAY_DVS_Abnormal_Control_reg);
        abnormal_info->abnormal_long_en = ppoverlay_dvs_abnormal_control_reg.dvs_abnormal_long_en;
        abnormal_info->abnormal_measure_en = ppoverlay_dvs_abnormal_control_reg.dvs_abnormal_measure_en;
        abnormal_info->abnormal_short_en = ppoverlay_dvs_abnormal_control_reg.dvs_abnormal_short_en;
        abnormal_info->short_line_th = ppoverlay_dvs_abnormal_control_reg.dvs_short_line_th;
        abnormal_info->long_line_th = ppoverlay_dvs_abnormal_control_reg.dvs_long_line_th;
    }
    else if(dtgType == DTG_MASTER2)
    {
        ppoverlay_dvs2_abnormal_control_reg.regValue = IoReg_Read32(PPOVERLAY_DVS2_Abnormal_Control_reg);
        abnormal_info->abnormal_long_en = ppoverlay_dvs2_abnormal_control_reg.dvs2_abnormal_long_en;
        abnormal_info->abnormal_measure_en = ppoverlay_dvs2_abnormal_control_reg.dvs2_abnormal_measure_en;
        abnormal_info->abnormal_short_en = ppoverlay_dvs2_abnormal_control_reg.dvs2_abnormal_short_en;
        abnormal_info->short_line_th = ppoverlay_dvs2_abnormal_control_reg.dvs2_short_line_th;
        abnormal_info->long_line_th = ppoverlay_dvs2_abnormal_control_reg.dvs2_long_line_th;
    }
    else
    {
        DTG_PRINT("[%s %d][Error] dtg type error dtgType:%d\n", __FUNCTION__, __LINE__, dtgType);
    }

    //DTG_PRINT("[%s %d]GET abnormal_long_en:%d, abnormal_measure_en:%d, abnormal_short_en:%d, short_line_th:%d, long_line_th:%d\n", __FUNCTION__, __LINE__, abnormal_info->abnormal_long_en, abnormal_info->abnormal_measure_en,
    //abnormal_info->abnormal_short_en, abnormal_info->short_line_th, abnormal_info->long_line_th);

    return ret;
}

int scaler_dtg_set_main_display_control(DISPLAY_CONTROL_INFO *display_control)
{
    int ret = 0;
    ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;

    if(display_control == NULL)
    {
        DTG_PRINT("[%s %d][Error] display_control parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);
    ppoverlay_main_display_control_rsv_reg.disp2_main_wd_to_free_run = display_control->disp2_wd_to_free_run;
    ppoverlay_main_display_control_rsv_reg.main_wd_to_background = display_control->wd_to_background;
    ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run = display_control->wd_to_free_run;
    ppoverlay_main_display_control_rsv_reg.m_hbd_en = display_control->hbd_en;
    ppoverlay_main_display_control_rsv_reg.m_bd_trans = display_control->bd_trans;
    ppoverlay_main_display_control_rsv_reg.m_force_bg = display_control->force_bg;
    ppoverlay_main_display_control_rsv_reg.m_disp_en = display_control->disp_en;

    IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);

    return ret;
}

int scaler_dtg_get_main_display_control(DISPLAY_CONTROL_INFO *display_control)
{
    int ret = 0;
    ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;

    if(display_control == NULL)
    {
        DTG_PRINT("[%s %d][Error] display_control parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(display_control, 0, sizeof(DISPLAY_CONTROL_INFO));

    ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);
    display_control->disp2_wd_to_free_run = ppoverlay_main_display_control_rsv_reg.disp2_main_wd_to_free_run;
    display_control->wd_to_background = ppoverlay_main_display_control_rsv_reg.main_wd_to_background;
    display_control->wd_to_free_run = ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run;
    display_control->hbd_en = ppoverlay_main_display_control_rsv_reg.m_hbd_en;
    display_control->bd_trans = ppoverlay_main_display_control_rsv_reg.m_bd_trans;
    display_control->force_bg = ppoverlay_main_display_control_rsv_reg.m_force_bg;
    display_control->disp_en = ppoverlay_main_display_control_rsv_reg.m_disp_en;

    return ret;
}

int scaler_dtg_set_sub_display_control(DISPLAY_CONTROL_INFO *display_control)
{
    int ret = 0;
    ppoverlay_mp_layout_force_to_background_RBUS ppoverlay_mp_layout_force_to_background_reg;
    ppoverlay_mp_disp_enable_RBUS ppoverlay_mp_disp_enable_reg;

    if(display_control == NULL)
    {
        DTG_PRINT("[%s %d][Error] display_control parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_mp_layout_force_to_background_reg.regValue = IoReg_Read32(PPOVERLAY_MP_Layout_Force_to_Background_reg);
    ppoverlay_mp_layout_force_to_background_reg.disp2_sub_wd_to_free_run = display_control->disp2_wd_to_free_run;
    ppoverlay_mp_layout_force_to_background_reg.sub_wd_to_background = display_control->wd_to_background;
    ppoverlay_mp_layout_force_to_background_reg.sub_wd_to_free_run = display_control->wd_to_free_run;
    ppoverlay_mp_layout_force_to_background_reg.s_bd_trans = display_control->bd_trans;
    ppoverlay_mp_layout_force_to_background_reg.s_force_bg = display_control->force_bg;
    ppoverlay_mp_layout_force_to_background_reg.s_disp_en = display_control->disp_en;

    ppoverlay_mp_disp_enable_reg.regValue = IoReg_Read32(PPOVERLAY_MP_Disp_Enable_reg);
    ppoverlay_mp_disp_enable_reg.mp_hbd_en = display_control->hbd_en;

    IoReg_Write32(PPOVERLAY_MP_Layout_Force_to_Background_reg, ppoverlay_mp_layout_force_to_background_reg.regValue);
    IoReg_Write32(PPOVERLAY_MP_Disp_Enable_reg, ppoverlay_mp_disp_enable_reg.regValue);

    return ret;
}

int scaler_dtg_get_sub_display_control(DISPLAY_CONTROL_INFO *display_control)
{
    int ret = 0;

    ppoverlay_mp_layout_force_to_background_RBUS ppoverlay_mp_layout_force_to_background_reg;
    ppoverlay_mp_disp_enable_RBUS ppoverlay_mp_disp_enable_reg;

    if(display_control == NULL)
    {
        DTG_PRINT("[%s %d][Error] display_control parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(display_control, 0, sizeof(DISPLAY_CONTROL_INFO));

    ppoverlay_mp_layout_force_to_background_reg.regValue = IoReg_Read32(PPOVERLAY_MP_Layout_Force_to_Background_reg);
    display_control->disp2_wd_to_free_run = ppoverlay_mp_layout_force_to_background_reg.disp2_sub_wd_to_free_run;
    display_control->wd_to_background = ppoverlay_mp_layout_force_to_background_reg.sub_wd_to_background;
    display_control->wd_to_free_run = ppoverlay_mp_layout_force_to_background_reg.sub_wd_to_free_run;
    display_control->bd_trans = ppoverlay_mp_layout_force_to_background_reg.s_bd_trans;
    display_control->force_bg = ppoverlay_mp_layout_force_to_background_reg.s_force_bg;
    display_control->disp_en = ppoverlay_mp_layout_force_to_background_reg.s_disp_en;

    ppoverlay_mp_disp_enable_reg.regValue = IoReg_Read32(PPOVERLAY_MP_Disp_Enable_reg);
    display_control->hbd_en = ppoverlay_mp_disp_enable_reg.mp_hbd_en;

    return ret;
}

int scaler_dtg_set_systhesis_n_ratio(FIX_LAST_LINE_SYSTHESIS_N_RATIO *ratio)
{
    int ret = 0;

    ppoverlay_fix_last_line_systhesisn_ratio_RBUS ppoverlay_fix_last_line_systhesisn_ratio_reg;

    if(ratio == NULL)
    {
        DTG_PRINT("[%s %d][Error] ratio parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_fix_last_line_systhesisn_ratio_reg.regValue = IoReg_Read32(PPOVERLAY_fix_last_line_systhesisN_ratio_reg);
    ppoverlay_fix_last_line_systhesisn_ratio_reg.dv_rst_sscg_sel = ratio->dv_rst_sscg_sel;
    ppoverlay_fix_last_line_systhesisn_ratio_reg.max_multiple_synthesisn = ratio->max_multiple_synthesisN;
    ppoverlay_fix_last_line_systhesisn_ratio_reg.synthesisn = ratio->synthesisN;
    ppoverlay_fix_last_line_systhesisn_ratio_reg.synthesisn_ratio = ratio->synthesisN_ratio;

    IoReg_Write32(PPOVERLAY_fix_last_line_systhesisN_ratio_reg, ppoverlay_fix_last_line_systhesisn_ratio_reg.regValue);

    return ret;
}

int scaler_dtg_get_systhesis_n_ratio(FIX_LAST_LINE_SYSTHESIS_N_RATIO *ratio)
{
    int ret = 0;

    ppoverlay_fix_last_line_systhesisn_ratio_RBUS ppoverlay_fix_last_line_systhesisn_ratio_reg;

    if(ratio == NULL)
    {
        DTG_PRINT("[%s %d][Error] ratio parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(ratio, 0, sizeof(FIX_LAST_LINE_SYSTHESIS_N_RATIO));

    ppoverlay_fix_last_line_systhesisn_ratio_reg.regValue = IoReg_Read32(PPOVERLAY_fix_last_line_systhesisN_ratio_reg);
    ratio->dv_rst_sscg_sel = ppoverlay_fix_last_line_systhesisn_ratio_reg.dv_rst_sscg_sel;
    ratio->max_multiple_synthesisN = ppoverlay_fix_last_line_systhesisn_ratio_reg.max_multiple_synthesisn;
    ratio->synthesisN = ppoverlay_fix_last_line_systhesisn_ratio_reg.synthesisn;
    ratio->synthesisN_ratio = ppoverlay_fix_last_line_systhesisn_ratio_reg.synthesisn_ratio;

    return ret;
}

int scaler_dtg_set_fix_last_line_mode_lock_status(FIX_LAST_LINE_MODE_LOCK_STATUS *status)
{
    int ret = 0;

    ppoverlay_fix_last_line_mode_lock_status_RBUS ppoverlay_fix_last_line_mode_lock_status_reg;

    if(status == NULL)
    {
        DTG_PRINT("[%s %d][Error] status parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_fix_last_line_mode_lock_status_reg.regValue = IoReg_Read32(PPOVERLAY_fix_last_line_mode_lock_status_reg);

    ppoverlay_fix_last_line_mode_lock_status_reg.offset_ft_lock_en = status->offset_ft_lock_en;
    ppoverlay_fix_last_line_mode_lock_status_reg.t_s = status->t_s;
    ppoverlay_fix_last_line_mode_lock_status_reg.lock_th = status->lock_th;

    IoReg_Write32(PPOVERLAY_fix_last_line_mode_lock_status_reg, ppoverlay_fix_last_line_mode_lock_status_reg.regValue);

    return ret;
}

int scaler_dtg_get_fix_last_line_mode_lock_status(FIX_LAST_LINE_MODE_LOCK_STATUS *status)
{
    int ret = 0;

    ppoverlay_fix_last_line_mode_lock_status_RBUS ppoverlay_fix_last_line_mode_lock_status_reg;

    if(status == NULL)
    {
        DTG_PRINT("[%s %d][Error] status parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(status, 0, sizeof(FIX_LAST_LINE_MODE_LOCK_STATUS));

    ppoverlay_fix_last_line_mode_lock_status_reg.regValue = IoReg_Read32(PPOVERLAY_fix_last_line_mode_lock_status_reg);

    status->offset_ft_lock_en = ppoverlay_fix_last_line_mode_lock_status_reg.offset_ft_lock_en;
    status->t_s = ppoverlay_fix_last_line_mode_lock_status_reg.t_s;
    status->lock_th = ppoverlay_fix_last_line_mode_lock_status_reg.lock_th;

    return ret;
}

int scaler_dtg_set_i2d_ctrl_0(I2D_CTRL_0 *ctrl_0)
{
    int ret = 0;
    ppoverlay_i2d_ctrl_0_RBUS ppoverlay_i2d_ctrl_0_reg;

    if(ctrl_0 == NULL)
    {
        DTG_PRINT("[%s %d][Error] ctrl_0 parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_i2d_ctrl_0_reg.regValue = IoReg_Read32(PPOVERLAY_I2D_CTRL_0_reg);
    ppoverlay_i2d_ctrl_0_reg.i2d_special_dly_en = ctrl_0->i2d_special_dly_en;
    ppoverlay_i2d_ctrl_0_reg.i2d_special_dly_mode = ctrl_0->i2d_special_dly_mod;
    ppoverlay_i2d_ctrl_0_reg.i2d_dly_new_mode_en = ctrl_0->i2d_dly_new_mode_en;
    ppoverlay_i2d_ctrl_0_reg.i2d_dly_lower = ctrl_0->i2d_dly_lower;
    ppoverlay_i2d_ctrl_0_reg.i2d_dly_n_line_mode = ctrl_0->i2d_dly_n_line_mode;
    ppoverlay_i2d_ctrl_0_reg.i2d_dly_upper = ctrl_0->i2d_dly_upper;

    IoReg_Write32(PPOVERLAY_I2D_CTRL_0_reg, ppoverlay_i2d_ctrl_0_reg.regValue);

    return ret;
}

int scaler_dtg_get_i2d_ctrl_0(I2D_CTRL_0 *ctrl_0)
{
    int ret = 0;
    ppoverlay_i2d_ctrl_0_RBUS ppoverlay_i2d_ctrl_0_reg;

    if(ctrl_0 == NULL)
    {
        DTG_PRINT("[%s %d][Error] ctrl_0 parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(ctrl_0, 0, sizeof(I2D_CTRL_0));

    ppoverlay_i2d_ctrl_0_reg.regValue = IoReg_Read32(PPOVERLAY_I2D_CTRL_0_reg);
    ctrl_0->i2d_special_dly_en = ppoverlay_i2d_ctrl_0_reg.i2d_special_dly_en;
    ctrl_0->i2d_special_dly_mod = ppoverlay_i2d_ctrl_0_reg.i2d_special_dly_mode;
    ctrl_0->i2d_dly_new_mode_en = ppoverlay_i2d_ctrl_0_reg.i2d_dly_new_mode_en;
    ctrl_0->i2d_dly_lower = ppoverlay_i2d_ctrl_0_reg.i2d_dly_lower;
    ctrl_0->i2d_dly_n_line_mode = ppoverlay_i2d_ctrl_0_reg.i2d_dly_n_line_mode;
    ctrl_0->i2d_dly_upper = ppoverlay_i2d_ctrl_0_reg.i2d_dly_upper;

    return ret;
}

int scaler_dtg_set_i2d_ctrl_1(I2D_CTRL_1 *ctrl_1)
{
    int ret = 0;
    ppoverlay_i2d_ctrl_1_RBUS ppoverlay_i2d_ctrl_1_reg;

    if(ctrl_1 == NULL)
    {
        DTG_PRINT("[%s %d][Error] ctrl_1 parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_i2d_ctrl_1_reg.regValue = IoReg_Read32(PPOVERLAY_I2D_CTRL_1_reg);
    ppoverlay_i2d_ctrl_1_reg.i2d_sw_mode_en = ctrl_1->i2d_sw_mode_en;
    ppoverlay_i2d_ctrl_1_reg.i2d_dly_sub_line_max = ctrl_1->i2d_dly_sub_line_max;
    ppoverlay_i2d_ctrl_1_reg.i2d_sw_go = ctrl_1->i2d_sw_go;
    ppoverlay_i2d_ctrl_1_reg.i2d_measure_done = ctrl_1->i2d_measure_done;
    ppoverlay_i2d_ctrl_1_reg.i2d_dly_add_line_max = ctrl_1->i2d_dly_add_line_max;

    IoReg_Write32(PPOVERLAY_I2D_CTRL_1_reg, ppoverlay_i2d_ctrl_1_reg.regValue);

    return ret;
}

int scaler_dtg_get_i2d_ctrl_1(I2D_CTRL_1 *ctrl_1)
{
    int ret = 0;

    ppoverlay_i2d_ctrl_1_RBUS ppoverlay_i2d_ctrl_1_reg;

    if(ctrl_1 == NULL)
    {
        DTG_PRINT("[%s %d][Error] ctrl_1 parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(ctrl_1, 0, sizeof(I2D_CTRL_1));

    ppoverlay_i2d_ctrl_1_reg.regValue = IoReg_Read32(PPOVERLAY_I2D_CTRL_1_reg);

    ctrl_1->i2d_sw_mode_en = ppoverlay_i2d_ctrl_1_reg.i2d_sw_mode_en;
    ctrl_1->i2d_dly_sub_line_max = ppoverlay_i2d_ctrl_1_reg.i2d_dly_sub_line_max;
    ctrl_1->i2d_sw_go = ppoverlay_i2d_ctrl_1_reg.i2d_sw_go;
    ctrl_1->i2d_measure_done = ppoverlay_i2d_ctrl_1_reg.i2d_measure_done;
    ctrl_1->i2d_dly_add_line_max = ppoverlay_i2d_ctrl_1_reg.i2d_dly_add_line_max;

    return ret;
}

int scaler_dtg_set_dispd_smooth_toggle1(DISPD_SMOOTH_TOGGLE1 *toggle1)
{
    int ret = 0;

    ppoverlay_dispd_smooth_toggle1_RBUS ppoverlay_dispd_smooth_toggle1_reg;

    if(toggle1 == NULL)
    {
        DTG_PRINT("[%s %d][Error] toggle1 parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_dispd_smooth_toggle1_reg.regValue = IoReg_Read32(PPOVERLAY_dispd_smooth_toggle1_reg);
    ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_clk_mode_revised2 = toggle1->dispd_stage1_clk_mode_revised2;
    ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_src_sel2 = toggle1->dispd_stage1_smooth_toggle_src_sel2;
    ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_apply2 = toggle1->dispd_stage1_smooth_toggle_apply2;
    ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_clk_mode_revised = toggle1->dispd_stage1_clk_mode_revised;
    ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_src_sel = toggle1->dispd_stage1_smooth_toggle_src_sel;
    ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_apply = toggle1->dispd_stage1_smooth_toggle_apply;

    IoReg_Write32(PPOVERLAY_dispd_smooth_toggle1_reg, ppoverlay_dispd_smooth_toggle1_reg.regValue);

    return ret;
}

int scaler_dtg_get_dispd_smooth_toggle1(DISPD_SMOOTH_TOGGLE1 *toggle1)
{
    int ret = 0;

    ppoverlay_dispd_smooth_toggle1_RBUS ppoverlay_dispd_smooth_toggle1_reg;

    if(toggle1 == NULL)
    {
        DTG_PRINT("[%s %d][Error] toggle1 parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(toggle1, 0, sizeof(DISPD_SMOOTH_TOGGLE1));

    ppoverlay_dispd_smooth_toggle1_reg.regValue = IoReg_Read32(PPOVERLAY_dispd_smooth_toggle1_reg);

    toggle1->dispd_stage1_clk_mode_revised2 = ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_clk_mode_revised2;
    toggle1->dispd_stage1_smooth_toggle_src_sel2 = ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_src_sel2;
    toggle1->dispd_stage1_smooth_toggle_apply2 = ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_apply2;
    toggle1->dispd_stage1_clk_mode_revised = ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_clk_mode_revised;
    toggle1->dispd_stage1_smooth_toggle_src_sel = ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_src_sel;
    toggle1->dispd_stage1_smooth_toggle_apply = ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_apply;

    return ret;
}

int scaler_dtg_set_dispd_smooth_toggle2(DISPD_SMOOTH_TOGGLE2 *toggle2)
{
    int ret = 0;

    ppoverlay_dispd_smooth_toggle2_RBUS ppoverlay_dispd_smooth_toggle2_reg;

    if(toggle2 == NULL)
    {
        DTG_PRINT("[%s %d][Error] toggle2 parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ppoverlay_dispd_smooth_toggle2_reg.regValue = IoReg_Read32(PPOVERLAY_dispd_smooth_toggle2_reg);
    ppoverlay_dispd_smooth_toggle2_reg.dispd_stage2_clk_mode_revised = toggle2->dispd_stage2_clk_mode_revised;
    ppoverlay_dispd_smooth_toggle2_reg.dispd_stage2_smooth_toggle_src_sel = toggle2->dispd_stage2_smooth_toggle_src_sel;
    ppoverlay_dispd_smooth_toggle2_reg.dispd_stage2_smooth_toggle_apply = toggle2->dispd_stage2_smooth_toggle_apply;

    IoReg_Write32(PPOVERLAY_dispd_smooth_toggle2_reg, ppoverlay_dispd_smooth_toggle2_reg.regValue);

    return ret;
}

int scaler_dtg_get_dispd_smooth_toggle2(DISPD_SMOOTH_TOGGLE2 *toggle2)
{
    int ret = 0;

    ppoverlay_dispd_smooth_toggle2_RBUS ppoverlay_dispd_smooth_toggle2_reg;

    if(toggle2 == NULL)
    {
        DTG_PRINT("[%s %d][Error] toggle2 parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(toggle2, 0, sizeof(DISPD_SMOOTH_TOGGLE2));

    ppoverlay_dispd_smooth_toggle2_reg.regValue = IoReg_Read32(PPOVERLAY_dispd_smooth_toggle2_reg);

    toggle2->dispd_stage2_clk_mode_revised = ppoverlay_dispd_smooth_toggle2_reg.dispd_stage2_clk_mode_revised;
    toggle2->dispd_stage2_smooth_toggle_src_sel = ppoverlay_dispd_smooth_toggle2_reg.dispd_stage2_smooth_toggle_src_sel;
    toggle2->dispd_stage2_smooth_toggle_apply = ppoverlay_dispd_smooth_toggle2_reg.dispd_stage2_smooth_toggle_apply;

    return ret;
}

int scaler_dtg_set_clk(DTG_TYPE dtg_type, DTG_CLK_INFO *dtg_clk_info)
{
    int ret = 0;

    sys_reg_sys_dispclksel_RBUS sys_dispclksel_reg;
    sys_reg_sys_dclk_gate_sel2_RBUS sys_reg_sys_dclk_gate_sel2_reg;
    sys_reg_sys_dispclk_fract_RBUS sys_reg_sys_dispclk_fract_reg;
    sys_reg_sys_dispclk_fract_1_RBUS sys_reg_sys_dispclk_fract_1_reg;
    sys_reg_sys_dispclk_fract_2_RBUS sys_reg_sys_dispclk_fract_2_reg;
    sys_reg_sys_dclk_gate_sel0_RBUS sys_reg_sys_dclk_gate_sel0_reg;

    if(dtg_clk_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] dtg_clk_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    sys_dispclksel_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLKSEL_reg);
    sys_reg_sys_dclk_gate_sel2_reg.regValue = IoReg_Read32(SYS_REG_SYS_DCLK_GATE_SEL2_reg);
    sys_reg_sys_dispclk_fract_1_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLK_FRACT_1_reg);
    sys_reg_sys_dispclk_fract_2_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLK_FRACT_2_reg);
    sys_reg_sys_dispclk_fract_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLK_FRACT_reg);
    sys_reg_sys_dclk_gate_sel0_reg.regValue = IoReg_Read32(SYS_REG_SYS_DCLK_GATE_SEL0_reg);

    if(DTG_UZU == dtg_type)
    {
        sys_dispclksel_reg.dclk_fract_en = dtg_clk_info->fract_en;
        sys_dispclksel_reg.dclk_fract_a = dtg_clk_info->fract_a;
        sys_dispclksel_reg.dclk_fract_b = dtg_clk_info->fract_b;
        sys_dispclksel_reg.clken_disp_stage1 = dtg_clk_info->clken_disp_stage1;
        sys_dispclksel_reg.d1_clk_root_sel = dtg_clk_info->root_sel;
        IoReg_Write32(SYS_REG_SYS_DISPCLKSEL_reg, sys_dispclksel_reg.regValue);

        sys_reg_sys_dclk_gate_sel2_reg.d1_gate_sel = dtg_clk_info->gate_sel;
        IoReg_Write32(SYS_REG_SYS_DCLK_GATE_SEL2_reg, sys_reg_sys_dclk_gate_sel2_reg.regValue);
    }
    else if(DTG_OSD123 == dtg_type)
    {
        sys_dispclksel_reg.blend_clk_root_sel = dtg_clk_info->root_sel;
        IoReg_Write32(SYS_REG_SYS_DISPCLKSEL_reg, sys_dispclksel_reg.regValue);

        sys_reg_sys_dispclk_fract_1_reg.blend_fract_en = dtg_clk_info->fract_en;
        sys_reg_sys_dispclk_fract_1_reg.blend_fract_a = dtg_clk_info->fract_a;
        sys_reg_sys_dispclk_fract_1_reg.blend_fract_b = dtg_clk_info->fract_b;
        IoReg_Write32(SYS_REG_SYS_DISPCLK_FRACT_1_reg, sys_reg_sys_dispclk_fract_1_reg.regValue);

        sys_reg_sys_dclk_gate_sel2_reg.blend_gate_sel = dtg_clk_info->gate_sel;
        IoReg_Write32(SYS_REG_SYS_DCLK_GATE_SEL2_reg, sys_reg_sys_dclk_gate_sel2_reg.regValue);

        sys_reg_sys_dispclk_fract_2_reg.blend_clk_fract_sel = dtg_clk_info->clk_sel;
        IoReg_Write32(SYS_REG_SYS_DISPCLK_FRACT_2_reg, sys_reg_sys_dispclk_fract_2_reg.regValue);

        sys_reg_sys_dclk_gate_sel0_reg.dclk_osd_f1p_gate_sel = dtg_clk_info->osd_f1p_gate_sel;
        IoReg_Write32(SYS_REG_SYS_DCLK_GATE_SEL0_reg, sys_reg_sys_dclk_gate_sel0_reg.regValue);
    }
    else if(DTG_OSD4 == dtg_type)
    {
        sys_dispclksel_reg.blend4_clk_root_sel = dtg_clk_info->root_sel;
        IoReg_Write32(SYS_REG_SYS_DISPCLKSEL_reg, sys_dispclksel_reg.regValue);

        sys_reg_sys_dispclk_fract_1_reg.blend4_fract_en = dtg_clk_info->fract_en;
        sys_reg_sys_dispclk_fract_1_reg.blend4_fract_a = dtg_clk_info->fract_a;
        sys_reg_sys_dispclk_fract_1_reg.blend4_fract_b = dtg_clk_info->fract_b;
        IoReg_Write32(SYS_REG_SYS_DISPCLK_FRACT_1_reg, sys_reg_sys_dispclk_fract_1_reg.regValue);

        sys_reg_sys_dclk_gate_sel2_reg.blend4_gate_sel = dtg_clk_info->gate_sel;
        IoReg_Write32(SYS_REG_SYS_DCLK_GATE_SEL2_reg, sys_reg_sys_dclk_gate_sel2_reg.regValue);

        sys_reg_sys_dispclk_fract_2_reg.blend4_clk_fract_sel = dtg_clk_info->clk_sel;
        IoReg_Write32(SYS_REG_SYS_DISPCLK_FRACT_2_reg, sys_reg_sys_dispclk_fract_2_reg.regValue);
    }
    else if(DTG_OSD5 == dtg_type)
    {
        sys_reg_sys_dclk_gate_sel2_reg.blend5_gate_sel = dtg_clk_info->gate_sel;
        IoReg_Write32(SYS_REG_SYS_DCLK_GATE_SEL2_reg, sys_reg_sys_dclk_gate_sel2_reg.regValue);

        sys_reg_sys_dispclk_fract_2_reg.blend5_clk_fract_sel = dtg_clk_info->clk_sel;
        IoReg_Write32(SYS_REG_SYS_DISPCLK_FRACT_2_reg, sys_reg_sys_dispclk_fract_2_reg.regValue);
    }
    else if(DTG_D2PRE == dtg_type)
    {
        sys_dispclksel_reg.d2pre_clk_root_sel = dtg_clk_info->root_sel;
        IoReg_Write32(SYS_REG_SYS_DISPCLKSEL_reg, sys_dispclksel_reg.regValue);

        sys_reg_sys_dispclk_fract_reg.dclk_s2_fract_en = dtg_clk_info->fract_en;
        sys_reg_sys_dispclk_fract_reg.dclk_s2_fract_a = dtg_clk_info->fract_a;
        sys_reg_sys_dispclk_fract_reg.dclk_s2_fract_b = dtg_clk_info->fract_b;
        IoReg_Write32(SYS_REG_SYS_DISPCLK_FRACT_reg, sys_reg_sys_dispclk_fract_reg.regValue);

        sys_reg_sys_dclk_gate_sel2_reg.d2pre_gate_sel = dtg_clk_info->gate_sel;
        IoReg_Write32(SYS_REG_SYS_DCLK_GATE_SEL2_reg, sys_reg_sys_dclk_gate_sel2_reg.regValue);
    }
    else if(DTG_D2POST == dtg_type)
    {
        sys_reg_sys_dclk_gate_sel2_reg.d2post_gate_sel = dtg_clk_info->gate_sel;
        sys_reg_sys_dclk_gate_sel2_reg.d2post_lout_gate_sel = dtg_clk_info->lout_gate_sel;
        IoReg_Write32(SYS_REG_SYS_DCLK_GATE_SEL2_reg, sys_reg_sys_dclk_gate_sel2_reg.regValue);
    }
    else
    {
        ret = -1;
    }

    DTG_PRINT("[%s %d]GET clk root_sel:%d, fract_en:%d, fract_a:%d, fract_b:%d, gate_sel:%d, clk_sel:%d, dtg_type:%s\n",
        __FUNCTION__, __LINE__, dtg_clk_info->root_sel, dtg_clk_info->fract_en, dtg_clk_info->fract_a,
        dtg_clk_info->fract_b, dtg_clk_info->gate_sel, dtg_clk_info->clk_sel, scaler_dtg_get_dtg_type_str(dtg_type));

    return ret;
}

int scaler_dtg_get_clk(DTG_TYPE dtg_type, DTG_CLK_INFO *dtg_clk_info)
{
    int ret = 0;

    sys_reg_sys_dispclksel_RBUS sys_dispclksel_reg;
    sys_reg_sys_dclk_gate_sel2_RBUS sys_reg_sys_dclk_gate_sel2_reg;
    sys_reg_sys_dispclk_fract_RBUS sys_reg_sys_dispclk_fract_reg;
    sys_reg_sys_dispclk_fract_1_RBUS sys_reg_sys_dispclk_fract_1_reg;
    sys_reg_sys_dispclk_fract_2_RBUS sys_reg_sys_dispclk_fract_2_reg;
    sys_reg_sys_dclk_gate_sel0_RBUS sys_reg_sys_dclk_gate_sel0_reg;

    if(dtg_clk_info == NULL)
    {
        DTG_PRINT("[%s %d][Error] dtg_clk_info parameter is error\n", __FUNCTION__, __LINE__);
        return -1;
    }

    sys_dispclksel_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLKSEL_reg);
    sys_reg_sys_dclk_gate_sel2_reg.regValue = IoReg_Read32(SYS_REG_SYS_DCLK_GATE_SEL2_reg);
    sys_reg_sys_dispclk_fract_1_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLK_FRACT_1_reg);
    sys_reg_sys_dispclk_fract_2_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLK_FRACT_2_reg);
    sys_reg_sys_dispclk_fract_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLK_FRACT_reg);
    sys_reg_sys_dclk_gate_sel0_reg.regValue = IoReg_Read32(SYS_REG_SYS_DCLK_GATE_SEL0_reg);

    memset(dtg_clk_info, 0, sizeof(DTG_CLK_INFO));

    if(DTG_UZU == dtg_type)
    {
        dtg_clk_info->fract_en = sys_dispclksel_reg.dclk_fract_en;
        dtg_clk_info->fract_a = sys_dispclksel_reg.dclk_fract_a;
        dtg_clk_info->fract_b = sys_dispclksel_reg.dclk_fract_b;
        dtg_clk_info->root_sel = sys_dispclksel_reg.d1_clk_root_sel;
        dtg_clk_info->gate_sel = sys_reg_sys_dclk_gate_sel2_reg.d1_gate_sel;
        dtg_clk_info->clken_disp_stage1 = sys_dispclksel_reg.clken_disp_stage1;
    }
    else if(DTG_OSD123 == dtg_type)
    {
        dtg_clk_info->root_sel = sys_dispclksel_reg.blend_clk_root_sel;
        dtg_clk_info->fract_en = sys_reg_sys_dispclk_fract_1_reg.blend_fract_en;
        dtg_clk_info->fract_a = sys_reg_sys_dispclk_fract_1_reg.blend_fract_a;
        dtg_clk_info->fract_b = sys_reg_sys_dispclk_fract_1_reg.blend_fract_b;
        dtg_clk_info->gate_sel = sys_reg_sys_dclk_gate_sel2_reg.blend_gate_sel;
        dtg_clk_info->clk_sel = sys_reg_sys_dispclk_fract_2_reg.blend_clk_fract_sel;
        dtg_clk_info->osd_f1p_gate_sel = sys_reg_sys_dclk_gate_sel0_reg.dclk_osd_f1p_gate_sel;
    }
     else if(DTG_OSD4 == dtg_type)
    {
        dtg_clk_info->root_sel = sys_dispclksel_reg.blend4_clk_root_sel;
        dtg_clk_info->fract_en = sys_reg_sys_dispclk_fract_1_reg.blend4_fract_en;
        dtg_clk_info->fract_a = sys_reg_sys_dispclk_fract_1_reg.blend4_fract_a;
        dtg_clk_info->fract_b = sys_reg_sys_dispclk_fract_1_reg.blend4_fract_b;
        dtg_clk_info->gate_sel = sys_reg_sys_dclk_gate_sel2_reg.blend4_gate_sel;
        dtg_clk_info->clk_sel = sys_reg_sys_dispclk_fract_2_reg.blend4_clk_fract_sel;
    }
    else if(DTG_OSD5 == dtg_type)
    {
        dtg_clk_info->gate_sel = sys_reg_sys_dclk_gate_sel2_reg.blend5_gate_sel;
        dtg_clk_info->clk_sel = sys_reg_sys_dispclk_fract_2_reg.blend5_clk_fract_sel;
    }
    else if(DTG_D2PRE == dtg_type)
    {
        dtg_clk_info->root_sel = sys_dispclksel_reg.d2pre_clk_root_sel;
        dtg_clk_info->fract_en = sys_reg_sys_dispclk_fract_reg.dclk_s2_fract_en;
        dtg_clk_info->fract_a = sys_reg_sys_dispclk_fract_reg.dclk_s2_fract_a;
        dtg_clk_info->fract_b = sys_reg_sys_dispclk_fract_reg.dclk_s2_fract_b;
        dtg_clk_info->gate_sel = sys_reg_sys_dclk_gate_sel2_reg.d2pre_gate_sel;
    }
    else if(DTG_D2POST == dtg_type)
    {
         dtg_clk_info->gate_sel = sys_reg_sys_dclk_gate_sel2_reg.d2post_gate_sel;
         dtg_clk_info->lout_gate_sel = sys_reg_sys_dclk_gate_sel2_reg.d2post_lout_gate_sel;
    }
    else
    {
        DTG_PRINT("[%s %d]Error Unknow dtg type %s\n", __FUNCTION__, __LINE__, scaler_dtg_get_dtg_type_str(dtg_type));
    }

    //DTG_PRINT("[%s %d]GET clk root_sel:%d, fract_en:%d, fract_a:%d, fract_b:%d, gate_sel:%d, clk_sel:%d, dtg_type:%s\n",
    //    __FUNCTION__, __LINE__, dtg_clk_info->root_sel, dtg_clk_info->fract_en, dtg_clk_info->fract_a,
    //    dtg_clk_info->fract_b, dtg_clk_info->gate_sel, dtg_clk_info->clk_sel, scaler_dtg_get_dtg_type_str(dtg_type));

    return ret;
}

int scaler_dtg_set_dpll_fsync_tracking_enable(void)
{
    int ret = 0;

    pll27x_reg_dpll_in_fsync_tracking_ctrl_RBUS pll27x_reg_dpll_in_fsync_tracking_ctrl_reg;

    pll27x_reg_dpll_in_fsync_tracking_ctrl_reg.regValue = IoReg_Read32(PLL27X_REG_DPLL_IN_FSYNC_TRACKING_CTRL_reg);
    pll27x_reg_dpll_in_fsync_tracking_ctrl_reg.fll_tracking_en = 1;
    IoReg_Write32(PLL27X_REG_DPLL_IN_FSYNC_TRACKING_CTRL_reg, pll27x_reg_dpll_in_fsync_tracking_ctrl_reg.regValue);

    return ret;
}

int scaler_dtg_set_dpll_fsync_tracking_disable(void)
{
    int ret = 0;

    pll27x_reg_dpll_in_fsync_tracking_ctrl_RBUS pll27x_reg_dpll_in_fsync_tracking_ctrl_reg;

    pll27x_reg_dpll_in_fsync_tracking_ctrl_reg.regValue = IoReg_Read32(PLL27X_REG_DPLL_IN_FSYNC_TRACKING_CTRL_reg);
    pll27x_reg_dpll_in_fsync_tracking_ctrl_reg.fll_tracking_en = 0;
    IoReg_Write32(PLL27X_REG_DPLL_IN_FSYNC_TRACKING_CTRL_reg, pll27x_reg_dpll_in_fsync_tracking_ctrl_reg.regValue);

    return ret;
}

int scaler_dtg_set_dpll_sys_dclkss_enable(void)
{
    pll27x_reg_sys_dclkss_RBUS pll27x_reg_sys_dclkss_reg;

    pll27x_reg_sys_dclkss_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_DCLKSS_reg);
    pll27x_reg_sys_dclkss_reg.dclk_ss_en = 1;
    pll27x_reg_sys_dclkss_reg.dclk_ss_load = 1;
    IoReg_Write32(PLL27X_REG_SYS_DCLKSS_reg, pll27x_reg_sys_dclkss_reg.regValue);

    return 0;
}

int scaler_dtg_set_dpll_sys_dclkss_disable(void)
{
    pll27x_reg_sys_dclkss_RBUS pll27x_reg_sys_dclkss_reg;

    pll27x_reg_sys_dclkss_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_DCLKSS_reg);
    pll27x_reg_sys_dclkss_reg.dclk_ss_en = 0;
    pll27x_reg_sys_dclkss_reg.dclk_ss_load = 0;
    IoReg_Write32(PLL27X_REG_SYS_DCLKSS_reg, pll27x_reg_sys_dclkss_reg.regValue);

    return 0;
}

int scaler_dtg_clear_abnormal_dvs_status(DTG_TYPE type)
{
    ppoverlay_dtg_pending_status_RBUS	ppoverlay_dtg_pending_status_reg;
    ppoverlay_dtg_pending_status_1_2_RBUS ppoverlay_dtg_pending_status_1_2_reg;

    if(type == DTG_MASTER)
    {
        ppoverlay_dtg_pending_status_reg.regValue = 0;
        ppoverlay_dtg_pending_status_reg.dvs_long = 1;
        ppoverlay_dtg_pending_status_reg.dvs_short = 1;

        IoReg_Write32(PPOVERLAY_DTG_pending_status_reg, ppoverlay_dtg_pending_status_reg.regValue);
    }
    else if(type == DTG_MASTER2)
    {
        ppoverlay_dtg_pending_status_1_2_reg.regValue = 0;
        ppoverlay_dtg_pending_status_1_2_reg.dvs2_short = 1;
        ppoverlay_dtg_pending_status_1_2_reg.dvs2_long = 1;

        IoReg_Write32(PPOVERLAY_DTG_pending_status_1_2_reg, ppoverlay_dtg_pending_status_1_2_reg.regValue);
    }
    else
    {
        DTG_PRINT("[%s %d]Error Unknow dtg type %s\n", __FUNCTION__, __LINE__, scaler_dtg_get_dtg_type_str(type));
    }

    return 0;
}

int scaler_dtg_get_abnormal_dvs_long_flag(DTG_TYPE dtg_type)
{
    ppoverlay_dtg_pending_status_RBUS	ppoverlay_dtg_pending_status_reg;
    ppoverlay_dtg_pending_status_1_2_RBUS ppoverlay_dtg_pending_status_1_2_reg;

    ppoverlay_dtg_pending_status_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_pending_status_reg);
    ppoverlay_dtg_pending_status_1_2_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_pending_status_1_2_reg);

    if(dtg_type == DTG_MASTER)
    {
        return ppoverlay_dtg_pending_status_reg.dvs_long;
    }
    else if(dtg_type == DTG_MASTER2)
    {
        return ppoverlay_dtg_pending_status_1_2_reg.dvs2_long;
    }

    return 0;
}

int scaler_dtg_get_abnormal_dvs_short_flag(DTG_TYPE dtg_type)
{
    ppoverlay_dtg_pending_status_RBUS	ppoverlay_dtg_pending_status_reg;
    ppoverlay_dtg_pending_status_1_2_RBUS ppoverlay_dtg_pending_status_1_2_reg;

    ppoverlay_dtg_pending_status_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_pending_status_reg);
    ppoverlay_dtg_pending_status_1_2_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_pending_status_1_2_reg);

    if(dtg_type == DTG_MASTER)
    {
        return ppoverlay_dtg_pending_status_reg.dvs_short;
    }
    else if(dtg_type == DTG_MASTER2)
    {
        return ppoverlay_dtg_pending_status_1_2_reg.dvs2_short;
    }

    return 0;
}

unsigned int scaler_dtg_get_time_for_90K(void)
{
    unsigned int time = 0;

    time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg) / 90;

    return time;
}

int scaler_dtg_set_frc2fsync_interrupt(DTG_TYPE type, unsigned char enable)
{
    ppoverlay_dtg_ie_2_RBUS ppoverlay_dtg_ie_2_reg;
    ppoverlay_dtg_pending_status_2_RBUS ppoverlay_dtg_pending_2_status_reg;
    int ret = 0;

    if(type == DTG_MASTER)
    {
        ppoverlay_dtg_pending_2_status_reg.regValue = 0;
        ppoverlay_dtg_pending_2_status_reg.disp_frc2fsync_event_2 = 1;
        IoReg_Write32(PPOVERLAY_DTG_pending_status_2_reg, ppoverlay_dtg_pending_2_status_reg.regValue);
        ppoverlay_dtg_ie_2_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_ie_2_reg);

        if(enable)
        {
            ppoverlay_dtg_ie_2_reg.disp_frc2fsync_event_ie_2 = 1;
        }
        else
        {
            ppoverlay_dtg_ie_2_reg.disp_frc2fsync_event_ie_2 = 0;
        }

        IoReg_Write32(PPOVERLAY_DTG_ie_2_reg, ppoverlay_dtg_ie_2_reg.regValue);
    }
    else if(type == DTG_MASTER2)
    {
        //frc2fsync just do direct
    }
    else if(type == DTG_OSD123)
    {
        ppoverlay_dtg_pending_2_status_reg.regValue = 0;
        ppoverlay_dtg_pending_2_status_reg.osddtg_frc2fsync_event_2 = 1;
        IoReg_Write32(PPOVERLAY_DTG_pending_status_2_reg, ppoverlay_dtg_pending_2_status_reg.regValue);
        ppoverlay_dtg_ie_2_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_ie_2_reg);

        if(enable)
        {
            ppoverlay_dtg_ie_2_reg.osddtg_frc2fsync_event_ie_2 = 1;
        }
        else
        {
            ppoverlay_dtg_ie_2_reg.osddtg_frc2fsync_event_ie_2 = 0;
        }

        IoReg_Write32(PPOVERLAY_DTG_ie_2_reg, ppoverlay_dtg_ie_2_reg.regValue);
    }

    return ret;
}

int scaler_dtg_set_fsync_ch_sel(DTG_TYPE dtg_type, unsigned char fsync_ch_sel)
{
    vgip_data_path_select_RBUS data_path_select_reg;

    if(dtg_type == DTG_MASTER)
    {
        data_path_select_reg.regValue = IoReg_Read32(VGIP_Data_Path_Select_reg);

        data_path_select_reg.fsync_ch_sel = fsync_ch_sel;

        IoReg_Write32(VGIP_Data_Path_Select_reg, data_path_select_reg.regValue);
    }

    return 0;
}

int scaler_dtg_set_dsce_region_pixel(DTG_TYPE dtg_type, unsigned int *pixel)
{
    int ret = 0;
    ppoverlay_d2post2dtg_control3_RBUS ppoverlay_d2post2dtg_control3_reg;

    if(pixel == NULL)
    {
        DTG_PRINT("[%s %d]ERROR parameter is NULL\n", __FUNCTION__, __LINE__);
        return -1;
    }

    if(dtg_type == DTG_D2POST2)
    {
        ppoverlay_d2post2dtg_control3_reg.regValue = IoReg_Read32(PPOVERLAY_D2POST2DTG_CONTROL3_reg);
        ppoverlay_d2post2dtg_control3_reg.dsce_regen_pixel = *pixel;

        IoReg_Write32(PPOVERLAY_D2POST2DTG_CONTROL3_reg, ppoverlay_d2post2dtg_control3_reg.regValue);
    }

    return ret;
}

int scaler_dtg_get_dsce_region_pixel(DTG_TYPE dtg_type, unsigned int *pixel)
{
    int ret = 0;
    ppoverlay_d2post2dtg_control3_RBUS ppoverlay_d2post2dtg_control3_reg;

    if(pixel == NULL)
    {
        DTG_PRINT("[%s %d]ERROR parameter is NULL\n", __FUNCTION__, __LINE__);
        return -1;
    }

    if(dtg_type == DTG_D2POST2)
    {
        ppoverlay_d2post2dtg_control3_reg.regValue = IoReg_Read32(PPOVERLAY_D2POST2DTG_CONTROL3_reg);
        *pixel = ppoverlay_d2post2dtg_control3_reg.dsce_regen_pixel;
    }

    return ret;
}

int scaler_dtg_set_dsce_region_dh_start_end(DTG_TYPE dtg_type, unsigned int *dh_start, unsigned int *dh_end)
{
    int ret = 0;
    ppoverlay_d2post2dtg_control4_RBUS ppoverlay_d2post2dtg_control4_reg;

    if(dh_start == NULL || dh_end == NULL)
    {
        DTG_PRINT("[%s %d]ERROR parameter is NULL\n", __FUNCTION__, __LINE__);
        return -1;
    }

    if(dtg_type == DTG_D2POST2)
    {
        ppoverlay_d2post2dtg_control4_reg.regValue = IoReg_Read32(PPOVERLAY_D2POST2DTG_CONTROL4_reg);
        ppoverlay_d2post2dtg_control4_reg.dsce_regen_dh_den_sta = *dh_start;
        ppoverlay_d2post2dtg_control4_reg.dsce_regen_dh_den_end = *dh_end;

        IoReg_Write32(PPOVERLAY_D2POST2DTG_CONTROL4_reg, ppoverlay_d2post2dtg_control4_reg.regValue);
    }

    return ret;
}

int scaler_dtg_get_dsce_region_dh_start_end(DTG_TYPE dtg_type, unsigned int *dh_start, unsigned int *dh_end)
{
    int ret = 0;
    ppoverlay_d2post2dtg_control4_RBUS ppoverlay_d2post2dtg_control4_reg;

    if(dh_start == NULL || dh_end == NULL)
    {
        DTG_PRINT("[%s %d]ERROR parameter is NULL\n", __FUNCTION__, __LINE__);
        return -1;
    }

    if(dtg_type == DTG_D2POST2)
    {
        ppoverlay_d2post2dtg_control4_reg.regValue = IoReg_Read32(PPOVERLAY_D2POST2DTG_CONTROL4_reg);
        *dh_start = ppoverlay_d2post2dtg_control4_reg.dsce_regen_dh_den_sta;
        *dh_end = ppoverlay_d2post2dtg_control4_reg.dsce_regen_dh_den_end;
    }

    return ret;
}

int scaler_dtg_get_uvc_pll_info(UVC_PLL_INFO *info)
{
    pll_reg_sys_pll_uvc_1_RBUS pll_reg_sys_pll_uvc_1_reg;
    pll_reg_sys_pll_uvc_2_RBUS pll_reg_sys_pll_uvc_2_reg;

    if(info == NULL)
    {
        DTG_PRINT("[%s %d]ERROR parameter is NULL\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(info, 0, sizeof(UVC_PLL_INFO));

    pll_reg_sys_pll_uvc_1_reg.regValue = IoReg_Read32(PLL_REG_SYS_PLL_UVC_1_reg);
    pll_reg_sys_pll_uvc_2_reg.regValue = IoReg_Read32(PLL_REG_SYS_PLL_UVC_2_reg);

    info->f_code = pll_reg_sys_pll_uvc_2_reg.plluvc_fcode;
    info->n_code = pll_reg_sys_pll_uvc_2_reg.plluvc_ncode;
    info->pre_div = pll_reg_sys_pll_uvc_1_reg.plluvc_prediv;
    info->o_code = pll_reg_sys_pll_uvc_1_reg.plluvc_o;

    return 0;
}

int scaler_dtg_set_all_double_buffer_disable(void)
{
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D0);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D1);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D2);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D3);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D4);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D5);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D6);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D7);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D8);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D9);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D10);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D11);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D12);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D13);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D14);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D15);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D16);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D17);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D19);
    scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D19);

    return 0;
}
