#ifndef _PANEL_DLG_
#define _PANEL_DLG_
#include <scaler/scalerCommon.h>
#include <tvscalercontrol/panel/hsr_parse.h>

#define CONFIG_DYNAMIC_PANEL_SELECT 1

#define HSR_ON_OFF_STABLE (5)

#define DLG_PRINT(fmt, args...) \
    do              \
    {               \
        rtd_pr_vbe_notice("[DLG][%s %d]"fmt, __FUNCTION__, __LINE__, ##args);\
    }while(0)

typedef enum 
{
    PANEL_HSR_OFF,
    PANEL_HSR_4k1k120,
    PANEL_HSR_4k1k240,
    PANEL_HSR_4k1k288
}PANEL_HSR_MODE;

typedef enum dlg_callback_type
{
    E_DLG_INIT,
    E_DLG_FREE_RUN,
    E_DLG_DTG_FRACTION,
    E_DLG_NOTIFY_VCPU_UPGRAD_PANEL_PARAMETER,
    E_DLG_DISABLE_OSD,
    E_DLG_ENABLE_OSD,
    E_DLG_MEMC,
    E_DLG_ENABLE_DLG_MASK,
    E_DLG_DISABLE_DLG_MASK,
    E_DLG_SET_PRINT_DLG_MESSAGE_CNT,
    E_DLG_MEMC_LATENCY,
    E_DLG_WAIT_MEMC_DTG_PORCH,
    E_DLG_SEND_DLG_MODE_TO_SLAVE,
    E_DLG_SSCG_ENABLE,
    E_DLG_SSCG_DISABLE,
    E_DLG_VPQ,
    E_DLG_LD,
    E_DLG_WAIT_INPUT_ONE_FRAME,
    E_DLG_UPGRADE_PANEL_INFO,
    E_DLG_STOP_SMOOTHTOGGLE
}DLG_CALLBACK_TYPE;

typedef enum dpll_type
{
    DPLL_4k2k120,
    DPLL_4k2k144
}DPLL_TYPE;

typedef struct panel_dlg_parameter_info
{
    unsigned int refresh;
    unsigned int typical;
    unsigned int htotal;
    unsigned int vtotal;
    unsigned int den_hstart;
    unsigned int den_hend;
    unsigned int den_vstart;
    unsigned int den_vend;
    unsigned int act_hstart;
    unsigned int act_hend;
    unsigned int act_vstart;
    unsigned int act_vend;
    unsigned int min_vtotal;
    unsigned int max_vtotal;
    unsigned int hsync_last_line;
}PANEL_DLG_PARAMETER_INFO;

typedef void (*DLG_CALLBACK)(DLG_CALLBACK_TYPE type);


void HAL_VBE_Panel_Set_DlgEnable(unsigned char dlgEnable);
unsigned char HAL_VBE_Panel_Get_DlgEnable(void);
unsigned int HAL_VBE_DISP_GetOutputFrameRateByMemcDtg(void);
PANEL_DLG_MODE panel_dlg_get_panel_dlg_mode(void);
unsigned char panel_dlg_check_dlg_mode_change(void);
void panel_dlg_handle(PANEL_DLG_MODE panelDlgMode);
PANEL_DLG_MODE panel_dlg_get_cur_dlg_mode(void);
void panel_dlg_set_cur_dlg_mode(PANEL_DLG_MODE dlgMode);
void panel_dlg_sfg_inner_ptg(unsigned char bOnOff);
void panel_dlg_printf_msg(void);
unsigned char panel_dlg_get_dlg_support_all_timing(void);
void panel_dlg_init(DLG_CALLBACK callback);
void panel_dlg_set_print_dlg_message_cnt(unsigned int cnt);
unsigned char panel_dlg_get_panel_vrr_freesync_timing_is_valid(void);
void panel_dlg_operator_panel_vrr_mode(unsigned char enable);
void panel_dlg_operator_panel_freesync_mode(unsigned char enable);
unsigned char panel_dlg_get_down_sample(void);
void panel_dlg_tconless_setting(PANEL_DLG_MODE dlgMode);
void panel_dlg_set_dlg_mode_by_ap(unsigned char *dlgMode);
PANEL_DLG_MODE panel_dlg_get_dlg_mode_by_ap(void);
void panel_dlg_set_scaler_rerunning_status(unsigned char status);
unsigned char panel_dlg_get_scaler_rerunning_status(void);
void panel_dlg_set_dlg_change_count(unsigned int count);
unsigned int panel_dlg_get_dlg_change_count(void);
unsigned char panel_dlg_check_dlg_stable(unsigned int currentCount, unsigned int threshold);
PANEL_DLG_MODE panel_dlg_get_timing_support_dlg(PANEL_HSR_MODE hsrMode, unsigned int customerIndex);
unsigned char panel_dlg_get_bypass_memc(void);
void panel_dlg_set_bypass_memc(unsigned char bypass);
void panel_dlg_switch_panel_mode(PANEL_DLG_MODE panelDlgMode);
int panel_dlg_tcon_setting_init(unsigned char *src, int len, unsigned int type);
int panel_dlg_pmic_setting_init(unsigned char *src, int len, unsigned int type);
int panel_dlg_ini_parse(unsigned char *src, int len, unsigned int type);
void panel_dlg_pmic_setting(PANEL_DLG_MODE panelDlgMode);

#endif
