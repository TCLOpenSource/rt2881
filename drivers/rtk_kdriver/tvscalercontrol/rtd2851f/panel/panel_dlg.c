#ifndef BUILD_QUICK_SHOW
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <rtk_kdriver/io.h>
#include <generated/autoconf.h>
#include <linux/semaphore.h>
#else
#include <timer.h>
#include <string.h>
#include <vby1/panel_api.h>
#endif
#include <tvscalercontrol/panel/panel_dlg.h>
#include <tvscalercontrol/panel/panel_dlg_customer.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/sfg_reg.h>
#include <rbus/osdovl_reg.h>
#include <panelConfigParameter.h>
#include <mach/platform.h>
#include <scaler/scalerCommon.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/hdmirx/hdmifun.h>
#include <rtd_log/rtd_module_log.h>

#ifdef CONFIG_DYNAMIC_PANEL_SELECT
static unsigned char s_vbeDlgEnable = 0;
static DLG_CALLBACK handler_dlg_call_back = NULL;
static PANEL_DLG_MODE s_panelDlgMode = PANEL_DLG_DEFAULT;
static PANEL_DLG_MODE s_panelDlgModeByAp = PANEL_DLG_DEFAULT;
static unsigned char s_dlg_rerunning_scaler = 0;
static unsigned int s_vbeDlgEnableChangeCount = 0;
static unsigned int print_dlg_message_cnt = 0;
static unsigned char panel_vrr_mode = 0;
static unsigned char panel_freesync_mode = 0;

static void panel_dlg_handle_dlg_setting(PANEL_DLG_MODE dlgMode, unsigned int customerIndex);
static void panel_dlg_init_down_sample(PANEL_DLG_MODE dlgMode);
static void panel_dlg_init_parameter_info(PANEL_DLG_MODE panelDlgMode);
static PANEL_DLG_MODE panel_dlg_get_dlg_mode(PANEL_HSR_MODE hsrMode, unsigned int customerIndex);
static void panel_dlg_load_dtg_setting(PANEL_DLG_MODE panelDlgMode);
static void panel_dlg_init_memc_bypass(PANEL_DLG_MODE dlgMode);

#define DLG_GET_HDMI_TIMING_COUNT  20


/*
* dlg hal api
*/

void HAL_VBE_Panel_Set_DlgEnable(unsigned char dlgEnable)
{
    s_vbeDlgEnable = dlgEnable;
}

unsigned char HAL_VBE_Panel_Get_DlgEnable(void)
{
    return s_vbeDlgEnable;
}

unsigned int HAL_VBE_DISP_GetOutputFrameRateByMemcDtg(void)
{
    unsigned int framerate = 0;

    ppoverlay_memcdtg_dvs_cnt_RBUS ppoverlay_memcdtg_dvs_cnt_reg;

    ppoverlay_memcdtg_dvs_cnt_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DVS_cnt_reg);

    if(ppoverlay_memcdtg_dvs_cnt_reg.memcdtg_dvs_cnt != 0)
    {
        framerate = (27000000 / ppoverlay_memcdtg_dvs_cnt_reg.memcdtg_dvs_cnt);
    }

    return framerate;
}

unsigned char panel_dlg_get_down_sample(void)
{
    return panel_dlg_customer_get_down_sample();
}

/**
** if vrr off, but device also send vrr info, hsr off, then we can not run vrr flow
*/
unsigned char panel_dlg_get_panel_vrr_freesync_timing_is_valid(void)
{
    MEASURE_TIMING_T vfehdmitiminginfo = {0};
    unsigned int panelSupportMaxFrameRate = 0;
    static int printCnt = 0;

    if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI)
    {
        drvif_Hdmi_GetRawTimingInfo(&vfehdmitiminginfo);

        panelSupportMaxFrameRate = Get_DISPLAY_REFRESH_RATE() * 1000;

        //if input timing over range of panel panelSupportMaxFrameRate 0.5Hz, not run vrr
        if(vfehdmitiminginfo.run_vrr && (vfehdmitiminginfo.v_freq > panelSupportMaxFrameRate) && ((vfehdmitiminginfo.v_freq - panelSupportMaxFrameRate) > 500))
        {
            if(printCnt == 0)
            {
                printCnt = 200;
                DLG_PRINT("[Error]hsr panel, vrr timing overrange panel can support max frameRate, so go non vrr flow input timing(%dHz), panel support max framerate(%dHz)\n", vfehdmitiminginfo.v_freq / 1000, panelSupportMaxFrameRate / 1000);
            }

            printCnt--;

            return 0;
        }
    }

    return 1;
}

void panel_dlg_operator_panel_vrr_mode(unsigned char enable)
{
    if(panel_vrr_mode != enable)
    {
        panel_dlg_customer_operator_panel_vrr_freesync_mode(enable);
    }

    panel_vrr_mode = enable;
}

void panel_dlg_operator_panel_freesync_mode(unsigned char enable)
{
    if(panel_freesync_mode != enable)
    {
        panel_dlg_customer_operator_panel_vrr_freesync_mode(enable);
    }

    panel_freesync_mode = enable;
}

unsigned char panel_dlg_get_dlg_support_all_timing(void)
{
    return Get_DISPLAY_HSR_SUPPORT_ALL_TIMING();
}

void panel_dlg_set_cur_dlg_mode(PANEL_DLG_MODE dlgMode)
{
    s_panelDlgMode = dlgMode;
}

PANEL_DLG_MODE panel_dlg_get_cur_dlg_mode(void)
{
    return s_panelDlgMode;
}

void panel_dlg_set_dlg_mode_by_ap(unsigned char *dlgMode)
{
    if(dlgMode != NULL)
    {
        if(strcmp(dlgMode, "PANEL_DLG_DEFAULT") == 0)
        {
             s_panelDlgModeByAp = PANEL_DLG_DEFAULT;
        }
        else if (strcmp(dlgMode, "PANEL_DLG_4k1k120") == 0)
        {
            s_panelDlgModeByAp = PANEL_DLG_4k1k120;
        }
        else if (strcmp(dlgMode, "PANEL_DLG_4k1k144") == 0)
        {
            s_panelDlgModeByAp = PANEL_DLG_4k1k144;
        }
        else if (strcmp(dlgMode, "PANEL_DLG_4k2k144") == 0)
        {
            s_panelDlgModeByAp = PANEL_DLG_4k2k144;
        }
        else if (strcmp(dlgMode, "PANEL_DLG_4k1k240") == 0)
        {
            s_panelDlgModeByAp = PANEL_DLG_4k1k240;
        }
        else if (strcmp(dlgMode, "PANEL_DLG_4k1k288") == 0)
        {
            s_panelDlgModeByAp = PANEL_DLG_4k1k288;
        }
    }
    else
    {
        DLG_PRINT("Error dlg mode error\n");
    }
}

PANEL_DLG_MODE panel_dlg_get_dlg_mode_by_ap(void)
{
    return s_panelDlgModeByAp;
}

void panel_dlg_set_scaler_rerunning_status(unsigned char status)
{
    s_dlg_rerunning_scaler = status;
}

unsigned char panel_dlg_get_scaler_rerunning_status(void)
{
    return s_dlg_rerunning_scaler;
}

void panel_dlg_set_dlg_change_count(unsigned int count)
{
    s_vbeDlgEnableChangeCount = count;
}

unsigned int panel_dlg_get_dlg_change_count(void)
{
    return s_vbeDlgEnableChangeCount;
}

unsigned char panel_dlg_check_dlg_stable(unsigned int currentCount, unsigned int threshold)
{
#if 0
    static unsigned int stableCount = 0;

    static unsigned int preViousCount = 0;

    if(ABS(currentCount, preViousCount) == threshold)
    {
        stableCount++;
    }
    else
    {
        stableCount = 0;
    }

    preViousCount = currentCount;

    if(stableCount >= HSR_ON_OFF_STABLE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
#else
    return 1;
#endif
}

unsigned char panel_dlg_check_dlg_mode_change(void)
{
    if(panel_dlg_check_dlg_stable(panel_dlg_get_dlg_change_count(), 0))
    {
        if(HAL_VBE_Panel_Get_DlgEnable())
        {
            if(panel_dlg_get_cur_dlg_mode() == panel_dlg_get_panel_dlg_mode())
            {
                return 0;
            }
            else
            {
                DLG_PRINT("dlg dlgMode %d -> %d\n", panel_dlg_get_cur_dlg_mode(), panel_dlg_get_panel_dlg_mode());
            }
        }
        else
        {
            if(panel_dlg_get_cur_dlg_mode() == panel_dlg_get_dlg_mode_by_ap())
            {
                return 0;
            }
            else
            {
                 DLG_PRINT("dlg dlgMode %d -> %d\n", panel_dlg_get_cur_dlg_mode(), panel_dlg_get_dlg_mode_by_ap());   
            }
        }
    }
    else
    {
        return 0;
    }

    return 1;
}


PANEL_DLG_MODE panel_dlg_get_panel_dlg_mode(void)
{
    PANEL_DLG_MODE dlgMode;

    if(HAL_VBE_Panel_Get_DlgEnable())
    {
        dlgMode = panel_dlg_get_timing_support_dlg(Get_DISPLAY_HSR_MODE(), Get_DISPLAY_PANEL_CUSTOM_INDEX());
    }
    else
    {
        dlgMode = panel_dlg_get_dlg_mode_by_ap();
    }

    return dlgMode;
}

static void panel_dlg_handle_dlg_setting(PANEL_DLG_MODE dlgMode, unsigned int customerIndex)
{
    panel_dlg_init_down_sample(dlgMode);

    panel_dlg_init_memc_bypass(dlgMode);

    panel_dlg_switch_panel_mode(dlgMode);

    panel_dlg_init_parameter_info(dlgMode);

    panel_dlg_load_dtg_setting(dlgMode);

    panel_dlg_pmic_setting(dlgMode);
}

static void panel_dlg_init_memc_bypass(PANEL_DLG_MODE dlgMode)
{
    panel_dlg_customer_init_memc_bypass(dlgMode);
}

static void panel_dlg_init_down_sample(PANEL_DLG_MODE dlgMode)
{
    panel_dlg_customer_init_down_sample(dlgMode);
}

static void panel_dlg_load_dtg_setting(PANEL_DLG_MODE panelDlgMode)
{
    panel_dlg_customer_timing_setting(panelDlgMode);
}

void panel_dlg_pmic_setting(PANEL_DLG_MODE panelDlgMode)
{
    //if(Get_DISPLAY_HSR_MODE() != PANEL_HSR_OFF)
    {
        panel_dlg_customer_pmic_setting_by_config(panelDlgMode);
    }
}

static void panel_dlg_init_parameter_info(PANEL_DLG_MODE panelDlgMode)
{
    PANEL_DLG_PARAMETER_INFO dlg_panel_parameter_info = {0};

    PANEL_CONFIG_PARAMETER panelConfigParameter = {0};

#ifndef UT_flag 
    memcpy(&panelConfigParameter, (PANEL_CONFIG_PARAMETER *)&platform_info.panel_parameter, sizeof(PANEL_CONFIG_PARAMETER));
#endif

    DLG_PRINT("panelDlgMode:%d\n", panelDlgMode);

    switch(panelDlgMode)
    {
        case PANEL_DLG_4k1k120:
            panel_dlg_customer_get_4k1k120_dlg_parameter_info(&dlg_panel_parameter_info);
            break;

        case PANEL_DLG_4k1k144:
            panel_dlg_customer_get_4k1k144_dlg_parameter_info(&dlg_panel_parameter_info);
            break;

        case PANEL_DLG_4k2k144:
            panel_dlg_customer_get_4k2k144_dlg_parameter_info(&dlg_panel_parameter_info);
            break;

        case PANEL_DLG_4k1k240:
            panel_dlg_customer_get_4k1k240_dlg_parameter_info(&dlg_panel_parameter_info);
            break;

        case PANEL_DLG_4k1k288:
            panel_dlg_customer_get_4k1k288_dlg_parameter_info(&dlg_panel_parameter_info);
            break;

        case PANEL_DLG_DEFAULT:
            panel_dlg_customer_get_default_dlg_parameter_info(&dlg_panel_parameter_info);
            break;

        default:
            DLG_PRINT("Error dlg mode, please check it\n");
            return;
    }

    panelConfigParameter.iCONFIG_DISPLAY_REFRESH_RATE = dlg_panel_parameter_info.refresh;
    panelConfigParameter.iCONFIG_DISPLAY_CLOCK_TYPICAL = dlg_panel_parameter_info.typical;
    panelConfigParameter.iCONFIG_DISP_HORIZONTAL_TOTAL = dlg_panel_parameter_info.htotal;
    panelConfigParameter.iCONFIG_DISP_VERTICAL_TOTAL = dlg_panel_parameter_info.vtotal;
    panelConfigParameter.iCONFIG_DISP_DEN_STA_HPOS = dlg_panel_parameter_info.den_hstart;
    panelConfigParameter.iCONFIG_DISP_DEN_END_HPOS = dlg_panel_parameter_info.den_hend;
    panelConfigParameter.iCONFIG_DISP_DEN_STA_VPOS = dlg_panel_parameter_info.den_vstart;
    panelConfigParameter.iCONFIG_DISP_DEN_END_VPOS = dlg_panel_parameter_info.den_vend;
    panelConfigParameter.iCONFIG_DISP_ACT_STA_HPOS = dlg_panel_parameter_info.act_hstart;
    panelConfigParameter.iCONFIG_DISP_ACT_END_HPOS = dlg_panel_parameter_info.act_hend;
    panelConfigParameter.iCONFIG_DISP_ACT_STA_VPOS = dlg_panel_parameter_info.act_vstart;
    panelConfigParameter.iCONFIG_DISP_ACT_END_VPOS = dlg_panel_parameter_info.act_vend;
    panelConfigParameter.iCONFIG_DISP_VERTICAL_TOTAL_60Hz_MIN = dlg_panel_parameter_info.min_vtotal;
    panelConfigParameter.iCONFIG_DISP_VERTICAL_TOTAL_50Hz_MAX = dlg_panel_parameter_info.max_vtotal;
    panelConfigParameter.iCONFIG_DISP_HSYNC_LASTLINE = dlg_panel_parameter_info.hsync_last_line;

    DLG_PRINT("panelConfigParameter.refresh:%d\n", panelConfigParameter.iCONFIG_DISPLAY_REFRESH_RATE);
    DLG_PRINT("panelConfigParameter.typical:%d\n", panelConfigParameter.iCONFIG_DISPLAY_CLOCK_TYPICAL);
    DLG_PRINT("panelConfigParameter.htotal:%d\n", panelConfigParameter.iCONFIG_DISP_HORIZONTAL_TOTAL);
    DLG_PRINT("panelConfigParameter.vtotal:%d\n", panelConfigParameter.iCONFIG_DISP_VERTICAL_TOTAL);
    DLG_PRINT("panelConfigParameter.den_hstart:%d\n", panelConfigParameter.iCONFIG_DISP_DEN_STA_HPOS);
    DLG_PRINT("panelConfigParameter.den_hend:%d\n", panelConfigParameter.iCONFIG_DISP_DEN_END_HPOS);
    DLG_PRINT("panelConfigParameter.den_vstart:%d\n", panelConfigParameter.iCONFIG_DISP_DEN_STA_VPOS);
    DLG_PRINT("panelConfigParameter.den_vend:%d\n", panelConfigParameter.iCONFIG_DISP_DEN_END_VPOS);
    DLG_PRINT("panelConfigParameter.act_hstart:%d\n", panelConfigParameter.iCONFIG_DISP_ACT_STA_HPOS);
    DLG_PRINT("panelConfigParameter.act_hend:%d\n", panelConfigParameter.iCONFIG_DISP_ACT_END_HPOS);
    DLG_PRINT("panelConfigParameter.act_vstart:%d\n", panelConfigParameter.iCONFIG_DISP_ACT_STA_VPOS);
    DLG_PRINT("panelConfigParameter.act_vend:%d\n", panelConfigParameter.iCONFIG_DISP_ACT_END_VPOS);
    DLG_PRINT("panelConfigParameter.min_vtotal:%d\n", panelConfigParameter.iCONFIG_DISP_VERTICAL_TOTAL_60Hz_MIN);
    DLG_PRINT("panelConfigParameter.max_vtotal:%d\n", panelConfigParameter.iCONFIG_DISP_VERTICAL_TOTAL_50Hz_MAX);
    DLG_PRINT("panelConfigParameter.hsync_last_line:%d\n", panelConfigParameter.iCONFIG_DISP_HSYNC_LASTLINE);

    Panel_InitParameter(&panelConfigParameter); 
}

void panel_dlg_switch_panel_mode(PANEL_DLG_MODE panelDlgMode)
{
    if(Get_DISPLAY_HSR_MODE() != PANEL_HSR_OFF)
    {
        panel_dlg_customer_switch_panel_mode(panelDlgMode);
    }
}

static PANEL_DLG_MODE panel_dlg_get_dlg_mode(PANEL_HSR_MODE hsrMode, unsigned int customerIndex)
{
    PANEL_DLG_MODE panelDlgMode = PANEL_DLG_DEFAULT;

    if(hsrMode == PANEL_HSR_4k1k240)
    {
        if(panel_dlg_get_dlg_support_all_timing())
        {
            panelDlgMode  = PANEL_DLG_4k1k240;
        }
        else
        {
            //hdmi 2k1k240 input support support 4k1k240 output
            if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI)
            {
                MEASURE_TIMING_T vfehdmitiminginfo = {0};
                unsigned char count = DLG_GET_HDMI_TIMING_COUNT;
                do
                {
                    if(drvif_Hdmi_GetRawTimingInfo(&vfehdmitiminginfo))
                    {
                        if((vfehdmitiminginfo.v_freq > 200000)
                        && (vfehdmitiminginfo.v_total < 1600))
                        {
                            panelDlgMode  = PANEL_DLG_4k1k240;
                        }

                        break;
                    }
                    else
                    {
                        //DLG_PRINT("ERROR get hdmi timing error\n");
                        usleep_range(10000, 10000);
                    }
                }while(--count);

                if(count == 0)
                {
                    panelDlgMode = panel_dlg_get_cur_dlg_mode();
                    DLG_PRINT("ERROR not get valid hdmi timing, panelDlgMode = %d\n", panelDlgMode);
                }
            }
        }
    }
    else if(hsrMode == PANEL_HSR_4k1k120)
    {
        panelDlgMode = PANEL_DLG_4k1k120;
        if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI)
        {
            MEASURE_TIMING_T vfehdmitiminginfo = {0};
            unsigned char count = DLG_GET_HDMI_TIMING_COUNT;
            do
            {
                if(drvif_Hdmi_GetRawTimingInfo(&vfehdmitiminginfo))
                {
                    if((vfehdmitiminginfo.v_freq > 143500)
                        && (vfehdmitiminginfo.v_freq < 144500))
                    {
                        //hdmi 144 input support support 144 output
                        panelDlgMode = PANEL_DLG_4k1k144;
                    }

                    break;
                }
                else
                {
                    //DLG_PRINT("ERROR get hdmi timing error\n");
                    usleep_range(10000, 10000);
                }
            }while(--count);

            if(count == 0)
            {
                panelDlgMode = panel_dlg_get_cur_dlg_mode();
                DLG_PRINT("ERROR not get valid hdmi timing, panelDlgMode = %d\n", panelDlgMode);
            }
        }
    }
    else if(hsrMode == PANEL_HSR_4k1k288)
    {
        if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI)
        {
            MEASURE_TIMING_T vfehdmitiminginfo = {0};
            unsigned char count = DLG_GET_HDMI_TIMING_COUNT;
            do
            {
                if(drvif_Hdmi_GetRawTimingInfo(&vfehdmitiminginfo))
                {
                    if((vfehdmitiminginfo.v_freq > 250000)
                        && (vfehdmitiminginfo.v_total < 1600))
                    {
                        //hdmi 2k1k288 input support support 4k1k288 output
                        panelDlgMode  = PANEL_DLG_4k1k288;
                    }
                    else if((vfehdmitiminginfo.v_freq > 200000)
                        && (vfehdmitiminginfo.v_total < 1600))
                    {
                        panelDlgMode  = PANEL_DLG_4k1k240;

                        //dclk default 4k2k144
                        if(Get_DEFAULT_DISPLAY_REFRESH_RATE() == 144)
                        {
                            panelDlgMode = PANEL_DLG_4k1k288;
                        }
                    }
                    else if((vfehdmitiminginfo.v_freq > 143500)
                        && (vfehdmitiminginfo.v_freq < 144500))
                    {
                        //hdmi 144 input support support 144 output
                        panelDlgMode = PANEL_DLG_4k2k144;

                        //dclk default 4k2k144
                        if(Get_DEFAULT_DISPLAY_REFRESH_RATE() == 144)
                        {
                            panelDlgMode = PANEL_DLG_DEFAULT;
                        }
                    }

                    break;
                }
                else
                {
                    //DLG_PRINT("ERROR get hdmi timing error\n");
                    usleep_range(10000, 10000);
                }
            }while(--count);

            if(count == 0)
            {
                panelDlgMode = panel_dlg_get_cur_dlg_mode();
                DLG_PRINT("ERROR not get valid hdmi timing, panelDlgMode = %d\n", panelDlgMode);
            }
        }
    }
    else
    {
        panelDlgMode = PANEL_DLG_DEFAULT;
    }

    return panelDlgMode;
}

PANEL_DLG_MODE panel_dlg_get_timing_support_dlg(PANEL_HSR_MODE hsrMode, unsigned int customerIndex)
{
    PANEL_DLG_MODE panelDlgMode = PANEL_DLG_DEFAULT;

    if(HAL_VBE_Panel_Get_DlgEnable())
    {
        return panel_dlg_get_dlg_mode(hsrMode, customerIndex);
    }
    else
    {
        if(hsrMode == PANEL_HSR_4k1k288)
        {
            if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI)
            {
                MEASURE_TIMING_T vfehdmitiminginfo = {0};
                unsigned char count = DLG_GET_HDMI_TIMING_COUNT;
                do
                {
                    if(drvif_Hdmi_GetRawTimingInfo(&vfehdmitiminginfo))
                    {
                        if((vfehdmitiminginfo.v_freq > 250000)
                            && (vfehdmitiminginfo.v_total < 1600))
                        {
                            //hsr disable hdmi 2k1k288 input support support 4k2k144 output
                            panelDlgMode  = PANEL_DLG_4k2k144;
                        }
                        else if((vfehdmitiminginfo.v_freq > 143500)
                            && (vfehdmitiminginfo.v_freq < 144500))
                        {
                            //hdmi 144 input support support 144 output
                            panelDlgMode = PANEL_DLG_4k2k144;
                        }

                        break;
                    }
                    else
                    {
                        //DLG_PRINT("ERROR get hdmi timing error\n");
                        usleep_range(10000, 10000);
                    }
                }while(--count);

                if(count == 0)
                {
                    panelDlgMode = panel_dlg_get_cur_dlg_mode();
                    DLG_PRINT("ERROR not get valid hdmi timing, panelDlgMode = %d\n", panelDlgMode);
                }
            }
            else
            {
                panelDlgMode = PANEL_DLG_DEFAULT;
            }

            //dclk default 4k2k144
            if(Get_DEFAULT_DISPLAY_REFRESH_RATE() == 144)
            {
                panelDlgMode = PANEL_DLG_DEFAULT;
            }
        }
        else
        {
            panelDlgMode = PANEL_DLG_DEFAULT;
        }
    }

    return panelDlgMode;
}

void panel_dlg_sfg_inner_ptg(unsigned char bOnOff)
{
    sfg_sfg_force_bg_at_dif_RBUS sfg_sfg_force_bg_at_dif_reg;

    if(handler_dlg_call_back != NULL)
    {
        handler_dlg_call_back(E_DLG_WAIT_MEMC_DTG_PORCH);
    }

    sfg_sfg_force_bg_at_dif_reg.regValue = IoReg_Read32(SFG_SFG_FORCE_BG_AT_DIF_reg);

    sfg_sfg_force_bg_at_dif_reg.bg_dif_red = 0;
    sfg_sfg_force_bg_at_dif_reg.bg_dif_grn = 0;
    sfg_sfg_force_bg_at_dif_reg.bg_dif_blu = 0;

    if(bOnOff)
    {
        sfg_sfg_force_bg_at_dif_reg.en_force_bg_dif = 1;
    }
    else
    {
        sfg_sfg_force_bg_at_dif_reg.en_force_bg_dif = 0;
    }

    IoReg_Write32(SFG_SFG_FORCE_BG_AT_DIF_reg, sfg_sfg_force_bg_at_dif_reg.regValue);
}

static char *panel_dlg_get_dlg_mode_str(void)
{
    PANEL_DLG_MODE dlgMode = panel_dlg_get_cur_dlg_mode();

    switch(dlgMode)
    {
        case PANEL_DLG_DEFAULT:
            return "PANEL_DLG_DEFAULT";
        case PANEL_DLG_4k1k120:
            return "PANEL_DLG_4k1k120";
        case PANEL_DLG_4k1k144:
            return "PANEL_DLG_4k1k144";
        case PANEL_DLG_4k2k144:
            return "PANEL_DLG_4k2k144";
        case PANEL_DLG_4k1k240:
            return "PANEL_DLG_4k1k240";
        case PANEL_DLG_4k1k288:
            return "PANEL_DLG_4k1k288";
        default:
            return "Error DLG MODE";
    }
}

void panel_dlg_printf_msg(void)
{
    unsigned int outPutFrameRate = HAL_VBE_DISP_GetOutputFrameRateByMemcDtg();

    if(print_dlg_message_cnt > 0)
    {
        DLG_PRINT("DLG MSG==>\n");
        DLG_PRINT("DLG Enable: %d\n", HAL_VBE_Panel_Get_DlgEnable());
        DLG_PRINT("DLG OutputFrame: %dHz\n", outPutFrameRate);
        DLG_PRINT("DLG MODE: %s\n", panel_dlg_get_dlg_mode_str());
        print_dlg_message_cnt--;
    }
}

void panel_dlg_set_print_dlg_message_cnt(unsigned int cnt)
{
    print_dlg_message_cnt = cnt;
}

void panel_dlg_init(DLG_CALLBACK callback)
{
    handler_dlg_call_back = callback;
    handler_dlg_call_back(E_DLG_INIT);
}

unsigned char panel_dlg_get_bypass_memc(void)
{
    return panel_dlg_customer_get_bypass_memc();
}

void panel_dlg_set_bypass_memc(unsigned char bypass)
{
    panel_dlg_customer_set_bypass_memc(bypass);
}

void panel_dlg_handle(PANEL_DLG_MODE dlgMode)
{
    if(handler_dlg_call_back == NULL)
    {
        DLG_PRINT("Dlg call back not register\n");
        return;
    }

    if(panel_dlg_get_cur_dlg_mode() == dlgMode)
    {
        DLG_PRINT("dlg mode not change dlgMode:%d\n", dlgMode);
        handler_dlg_call_back(E_DLG_DISABLE_DLG_MASK);
        return;
    }

    panel_dlg_set_cur_dlg_mode(dlgMode);

    panel_dlg_sfg_inner_ptg(1);
    handler_dlg_call_back(E_DLG_SSCG_DISABLE);
    handler_dlg_call_back(E_DLG_STOP_SMOOTHTOGGLE);
    handler_dlg_call_back(E_DLG_FREE_RUN);
    handler_dlg_call_back(E_DLG_DISABLE_OSD);

    panel_dlg_handle_dlg_setting(dlgMode, Get_DISPLAY_PANEL_CUSTOM_INDEX());

    handler_dlg_call_back(E_DLG_SSCG_ENABLE);
    handler_dlg_call_back(E_DLG_NOTIFY_VCPU_UPGRAD_PANEL_PARAMETER);
    handler_dlg_call_back(E_DLG_UPGRADE_PANEL_INFO);
    handler_dlg_call_back(E_DLG_DTG_FRACTION);
    if(panel_dlg_get_dlg_support_all_timing())
    {
        //handler_dlg_call_back(E_DLG_MEMC);
    }

    handler_dlg_call_back(E_DLG_SET_PRINT_DLG_MESSAGE_CNT);
    handler_dlg_call_back(E_DLG_DISABLE_DLG_MASK);
    //handler_dlg_call_back(E_DLG_MEMC_LATENCY);
    handler_dlg_call_back(E_DLG_VPQ);
    handler_dlg_call_back(E_DLG_LD);
    handler_dlg_call_back(E_DLG_ENABLE_OSD);
    handler_dlg_call_back(E_DLG_WAIT_INPUT_ONE_FRAME);
    panel_dlg_sfg_inner_ptg(0);
}

void panel_dlg_tconless_setting(PANEL_DLG_MODE dlgMode)
{
    //if(Get_DISPLAY_HSR_MODE() != PANEL_HSR_OFF)
    {
        panel_dlg_customer_tconless_setting(dlgMode);
    }
}

int panel_dlg_tcon_setting_init(unsigned char *src, int len, unsigned int type)
{
    //if(Get_DISPLAY_HSR_MODE() != PANEL_HSR_OFF)
    {
        return hsr_parse_tcon_setting(src, len, type);
    }

    return 0;
}

int panel_dlg_pmic_setting_init(unsigned char *src, int len, unsigned int type)
{
    //if(Get_DISPLAY_HSR_MODE() != PANEL_HSR_OFF)
    {
        return hsr_parse_pmic_setting(src, len, type);
    }

    return 0;
}

int panel_dlg_ini_parse(unsigned char *src, int len, unsigned int type)
{
    //if(Get_DISPLAY_HSR_MODE() != PANEL_HSR_OFF)
    {
        return hsr_parse_ini(src, len, type);
    }

    return 0;
}

#endif
