#ifndef BUILD_QUICK_SHOW
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <generated/autoconf.h>
#else
#include <timer.h>
#include <string.h>
#endif

#ifndef UT_flag
#include <rtk_kdriver/io.h>
#include <rtk_kdriver/i2c-rtk-api.h>
#include <rtk_kdriver/rtk_gpio.h>
#endif

#include <tvscalercontrol/panel/panel_dlg.h>
#include <tvscalercontrol/panel/panel_dlg_customer.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/sfg_reg.h>
#include <rbus/osdovl_reg.h>
#include <rbus/od_reg.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/pll27x_reg_reg.h>
#include <rbus/scaleup_reg.h>
#include <rbus/two_step_uzu_reg.h>
#include <panelConfigParameter.h>
#include <mach/platform.h>
#include <mach/rtk_platform.h>
#include <scaler/scalerCommon.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/hdmirx/hdmifun.h>
#include <rtd_log/rtd_module_log.h>
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <rbus/tcon_reg.h>
#include <ioctrl/scaler/vsc_cmd_id.h>
#include <rtk_kdriver/pcbMgr.h>
#include <memc_isr/scalerMEMC.h>

#ifdef CONFIG_DYNAMIC_PANEL_SELECT
static unsigned char enable_down_sample = 0;
static unsigned char s_bypass_memc = 0;

#define DPLL_NCODE_MODE 1    //1 byapss ncode mode, 0 ncode mode

extern int spi_read_internal(unsigned int type, unsigned long offset, unsigned int count, void *buf);

static unsigned char panel_dlg_customer_get_hsr_index_by_dlg_mode(PANEL_DLG_MODE panelDlgMode)
{
    HSR_MODE_TYPE hsrIndex = HSR_NOMAL;

    switch(panelDlgMode)
    {
        case PANEL_DLG_DEFAULT:
            hsrIndex = HSR_NOMAL;
            break;

        case PANEL_DLG_4k1k120:
            hsrIndex = HSR_4k1k_120;
            break;

        case PANEL_DLG_4k1k144:
            hsrIndex = HSR_4k1k_144;
            break;

        case PANEL_DLG_4k2k144:
            hsrIndex = HSR_4k2k_144;
            break;

        case PANEL_DLG_4k1k240:
            hsrIndex = HSR_4k1k_240;
            break;

        case PANEL_DLG_4k1k288:
            hsrIndex = HSR_4k1k_288;
            break;

        default:
            break;
    }

    return hsrIndex;
}

static unsigned char panel_dlg_customer_pmic_update_auto_p_gamma_data(HSR_PMIC_DATA *hsrPmicData)
{
    char pmicName[PMIC_NAME_SIZE] = {0};
    unsigned char flash_buff[FLASH_READ_SIZE] = {0};
    int i = 0;

    if((hsrPmicData == NULL) || (hsrPmicData->len > PMIC_BIN_MAX_SIZE))
    {
        DLG_PRINT("[Error] parameter is NULL\n");
        return 1;
    }

    Get_PMIC_Name(pmicName);

    if (!strcmp(pmicName, "CS603"))
    {
        /* auto p gamma,
        load from flash 0xfe00a,           save to pmic 0x0a
        load from flash 0xfe010 ~ 0xfe027, save to pmic 0x10 ~ 0x27 */
        if(FLASH_READ(0xfe000, flash_buff, FLASH_READ_SIZE) == 0)
        {
            memcpy(&hsrPmicData->pmic_bin[0x0a], flash_buff + 0x0a, 1);

            memcpy(&hsrPmicData->pmic_bin[0x10], flash_buff + 0x10, 24);
        }

    }
    else if (!strcmp(pmicName, "G2732K"))
    {
        if(FLASH_READ(0x00000, flash_buff, FLASH_READ_SIZE) == 0)
        {
            unsigned int spi_gamma[14] = {0};
            unsigned int gamma_voltage[14] = {0};
            int gamma[14] = {0};
            unsigned int Vref = 13170000;    // from spec
            unsigned int Vavdd = 0;

            // get avdd
            Vavdd = 13500000 + (hsrPmicData->pmic_bin[0x00] * 100000);

            // copy flash vcom to pmic vcom
            hsrPmicData->pmic_bin[0x08] = flash_buff[2] << 1; // [7:1] VCOM1_NT
            // fixed pmic vcom_max (6.04 V) from spec
            hsrPmicData->pmic_bin[0x0A] = 0x33; // 6.04 V
            // fixed pmic vcom_min (3.98 V) from spec
            hsrPmicData->pmic_bin[0x0B] = 0x22; // 3.98 V
            // flash gamma 1~14 to pmic gamma 1~14
            for(i = 0; i < 14; i++)
            {
                // flash data flash_buff[3]~flash_buff[30](spi_gamma 1~14)
                // SPI data to SPI gamma
                spi_gamma[i] = (flash_buff[(2 * i) + 3] << 8) + flash_buff[(2 * i) + 4];

                // SPI gamma to gamma voltage
                gamma_voltage[i] = Vref / 1023 * spi_gamma[i];

                // gamma voltage to gamma
                gamma[i] = gamma_voltage[i] / (Vavdd / 1024);

                // gamma to gamma ic buffer
                hsrPmicData->pmic_bin[(2 * i) + 0x0C] = gamma[i] >> 8;
                hsrPmicData->pmic_bin[(2 * i) + 0x0D] = gamma[i] & 0xFF;
            }
        }
    }

    return 0;
}

static void panel_dlg_customer_clock_offset(unsigned short usDclkOffset)
{
    pll27x_reg_pll_ssc0_RBUS pll_ssc0_reg;

    pll_ssc0_reg.regValue = IoReg_Read32(PLL27X_REG_PLL_SSC0_reg);
    pll_ssc0_reg.fcode_t_ssc = usDclkOffset;
    IoReg_Write32(PLL27X_REG_PLL_SSC0_reg, pll_ssc0_reg.regValue);
}

static void panel_dlg_customer_clock_set_pll(UINT8 M_Code, UINT8 N_Code, UINT8 div, UINT8 CPC, UINT8 bypassN)
{
    pll27x_reg_sys_pll_disp1_RBUS pll_disp1_reg;
    pll27x_reg_sys_pll_disp2_RBUS pll_disp2_reg;
    pll27x_reg_sys_pll_disp3_RBUS pll_disp3_reg;
    pll27x_reg_pll_ssc0_RBUS crt_pll_ssc0_reg;
    pll27x_reg_pll_ssc2_RBUS crt_pll_ssc2_reg;
    pll27x_reg_sys_pll_disp4_RBUS pll_disp4_reg;

    if(div == 1)
    {
        div = 0;
    }
    else if(div == 2)
    {
        div = 1;
    }
    else if(div == 4)
    {
        div = 2;
    }
    else if(div == 8)
    {
            div = 3;
    }

#if  0
    pll_disp3_reg.regValue =  IoReg_Read32(PLL27X_REG_SYS_PLL_DISP3_reg);
    pll_disp3_reg.dpll_pow= 0;
    pll_disp3_reg.dpll_rstb = 0;
    pll_disp3_reg.dpll_freeze = 1;
    IoReg_Write32(PLL27X_REG_SYS_PLL_DISP3_reg, pll_disp3_reg.regValue);

    usleep_range(1000, 1000);
#endif

    crt_pll_ssc2_reg.regValue = IoReg_Read32(PLL27X_REG_PLL_SSC2_reg);
    crt_pll_ssc2_reg.bypass_pi = 0;
    crt_pll_ssc2_reg.en_pi_debug = 1;
    IoReg_Write32(PLL27X_REG_PLL_SSC2_reg, crt_pll_ssc2_reg.regValue);
    pll_disp4_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_PLL_DISP4_reg);
    pll_disp4_reg.dpll_pi_ibsel = 3;
    IoReg_Write32(PLL27X_REG_SYS_PLL_DISP4_reg, pll_disp4_reg.regValue);

    pll_disp2_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_PLL_DISP2_reg);
    pll_disp2_reg.dpll_x_bpsin = (bypassN > 0) ? 1  : 0;
    pll_disp2_reg.dpll_x_bpspi= 0;
    IoReg_Write32(PLL27X_REG_SYS_PLL_DISP2_reg, pll_disp2_reg.regValue);

    usleep_range(1000, 1000);

    pll_disp3_reg.regValue =  IoReg_Read32(PLL27X_REG_SYS_PLL_DISP3_reg);
    pll_disp1_reg.regValue = 0;
    pll_disp1_reg.dpll_m = M_Code - 3;
    pll_disp3_reg.dpll_o = div;
    pll_disp1_reg.dpll_n= (bypassN > 0) ? (N_Code - 1) : N_Code;

    pll_disp1_reg.dpll_ip = 5;

    pll_disp1_reg.dpll_cp = 1;
    pll_disp1_reg.dpll_rs = 3;

    pll_disp1_reg.dpll_ssc_inv = 0;

    IoReg_Write32(PLL27X_REG_SYS_PLL_DISP1_reg, pll_disp1_reg.regValue);
    IoReg_Write32(PLL27X_REG_SYS_PLL_DISP3_reg, pll_disp3_reg.regValue);
    usleep_range(1000, 1000);

#if 0
    pll_disp3_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_PLL_DISP3_reg);

    pll_disp3_reg.dpll_reser = 0;
    pll_disp3_reg.dpll_dummy = 0;

    pll_disp3_reg.dpll_wdrst = 0;

    pll_disp3_reg.dpll_pow= 1;
    pll_disp3_reg.dpll_rstb = 1;
    IoReg_Write32(PLL27X_REG_SYS_PLL_DISP3_reg, pll_disp3_reg.regValue);

    usleep_range(1000, 1000);
#endif

    crt_pll_ssc0_reg.regValue= IoReg_Read32(PLL27X_REG_PLL_SSC0_reg);
    crt_pll_ssc0_reg.oc_en = 1;
    IoReg_Write32(PLL27X_REG_PLL_SSC0_reg, crt_pll_ssc0_reg.regValue);

    usleep_range(1000, 1000);
#if 0
    pll_disp3_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_PLL_DISP3_reg);
    pll_disp3_reg.dpll_freeze = 0;
    IoReg_Write32(PLL27X_REG_SYS_PLL_DISP3_reg, pll_disp3_reg.regValue);
#endif
}

static unsigned char panel_dlg_customer_clock_cal_cpc(unsigned int M_Code, unsigned int N_Code)
{
    unsigned int ucTemp1;
    unsigned int  ucTemp2 = 0;

    if( N_Code <= 5)
    {
        ucTemp1 = ((unsigned int) M_Code  *10000 / 1673);
    }
    else
    {
        ucTemp1 = ( (unsigned int)M_Code *10000 / 17600);
    }

    ucTemp1 = ucTemp1 - 25;

    if(ucTemp1 > 300)
    {
        ucTemp1 -= 300;
        ucTemp2 += 16;
    }

    if(ucTemp1 > 200)
    {
        ucTemp1 -= 200;
        ucTemp2 += 8;
    }

    if(ucTemp1 > 100)
    {
        ucTemp1 -= 100;
        ucTemp2 |= 4;
    }

    if(ucTemp1 > 50)
    {
        ucTemp1 -= 50;
        ucTemp2 += 2;
    }

    if(ucTemp1 > 25)
    {
        ucTemp1 -= 25;
        ucTemp2 += 1;
    }

    ucTemp2 += 32;

    return ucTemp2;
}

static unsigned char panel_dlg_customer_clock_set_dclk(unsigned int ulFreq, unsigned char bypassN)
{
    unsigned char  cpc,div=1, ret=0;
    unsigned char cnt=0;
    unsigned short nMCode,nNCode, f_code;

    UINT32 dclk_Temp;
    pll27x_reg_pll_ssc0_RBUS crt_pll_ssc0_reg;
    pll27x_reg_pll_ssc5_RBUS crt_pll_ssc5_reg;

    DLG_PRINT("set dpll ulFreq:%d\n", ulFreq);

    if((ulFreq >= 400 * 1000000) && (ulFreq <= 700 * 1000000))
    {
        div = 1;
    }
    else if((ulFreq >= 200 * 1000000) && (ulFreq <= 350 * 1000000))
    {
        div = 2;
        ulFreq = ulFreq * 2;
    }
    else if((ulFreq >= 100  * 1000000) && (ulFreq <= 175 * 1000000))
    {
        div = 4;
        ulFreq = ulFreq * 4;
    }
    else
    {
        DLG_PRINT("\n Error Dclk Range!\n");
    }

    nNCode = 1;

    dclk_Temp = ulFreq * ((bypassN > 0) ? (nNCode) : (nNCode +  2)) / (27 * 100);

    nMCode = dclk_Temp / 10000;
    f_code = ((dclk_Temp % 10000) * 2048) / 10000;
    cpc = panel_dlg_customer_clock_cal_cpc(nMCode, nNCode);

    crt_pll_ssc0_reg.regValue= IoReg_Read32(PLL27X_REG_PLL_SSC0_reg);
    crt_pll_ssc0_reg.oc_en = 0;
    crt_pll_ssc0_reg.fcode_t_ssc = f_code;
    crt_pll_ssc0_reg.oc_step_set = 0x10;
    IoReg_Write32(PLL27X_REG_PLL_SSC0_reg, crt_pll_ssc0_reg.regValue);

    panel_dlg_customer_clock_set_pll(nMCode, nNCode, div, cpc, bypassN);

    panel_dlg_customer_clock_offset(f_code);

    crt_pll_ssc0_reg.regValue= IoReg_Read32(PLL27X_REG_PLL_SSC0_reg);
    crt_pll_ssc0_reg.oc_en = 1;
    IoReg_Write32(PLL27X_REG_PLL_SSC0_reg, crt_pll_ssc0_reg.regValue);

    cnt = 100;
    do
    {
        usleep_range(1000, 1000);
        crt_pll_ssc5_reg.regValue = IoReg_Read32(PLL27X_REG_PLL_SSC5_reg);
    }while((crt_pll_ssc5_reg.oc_done != 1) && (cnt--));


    crt_pll_ssc0_reg.regValue= IoReg_Read32(PLL27X_REG_PLL_SSC0_reg);
    crt_pll_ssc0_reg.oc_en = 0;
    IoReg_Write32(PLL27X_REG_PLL_SSC0_reg, crt_pll_ssc0_reg.regValue);

	return ret;
}

void panel_dlg_customer_operator_panel_vrr_freesync_mode(unsigned char enable)
{
    const HSR_VRR_MODE *hsr_vrr = hsr_get_vrr_info();
    
    DLG_PRINT("enable:%d, Get_DISPLAY_PANEL_CUSTOM_INDEX():%d\n", enable, Get_DISPLAY_PANEL_CUSTOM_INDEX());
    
    if(hsr_vrr->i2c_addr != 00)
    {
        unsigned char cmd[I2C_CMD_MAX_LEN] = {0};
        int ret = 0;
        if(hsr_vrr->wp_enable == 1)
        {
            ret = IO_Direct_Set("PIN_HSR_I2C_WP", 1);
            if(ret == -1)
            {
                DLG_PRINT("[ERROR]set wp failed, please check it\n");
            }
        }

        if(hsr_vrr->wp_vrr_before_mute_delay)
        {
            msleep(hsr_vrr->wp_vrr_before_mute_delay);
        }
        else
        {
            msleep(80);
        }

        memset(cmd, 0, I2C_CMD_MAX_LEN);
        memcpy(cmd, hsr_vrr->i2c_cmd_mute, I2C_CMD_MAX_LEN);

        if (i2c_master_send_ex(hsr_vrr->i2c_group_id, hsr_vrr->i2c_addr, cmd, hsr_vrr->i2c_len_mute) < 0)
        {
            DLG_PRINT("send i2c cmd failed\n");
        }

        if(hsr_vrr->wp_vrr_before_on_off_delay)
        {
            msleep(hsr_vrr->wp_vrr_before_on_off_delay);
        }
        else
        {
            msleep(100);
        }

        if(enable)
        {
            //2 cmd on
            memset(cmd, 0, I2C_CMD_MAX_LEN);
            memcpy(cmd, hsr_vrr->i2c_cmd_vrr_on, I2C_CMD_MAX_LEN);
            if (i2c_master_send_ex(hsr_vrr->i2c_group_id, hsr_vrr->i2c_addr, cmd, hsr_vrr->i2c_len_vrr_on) < 0)
            {
                DLG_PRINT("send i2c cmd failed\n");
            }
        }
        else
        {
            //2 cmd off
            memset(cmd, 0, I2C_CMD_MAX_LEN);
            memcpy(cmd, hsr_vrr->i2c_cmd_vrr_off, I2C_CMD_MAX_LEN);
            if (i2c_master_send_ex(hsr_vrr->i2c_group_id, hsr_vrr->i2c_addr, cmd, hsr_vrr->i2c_len_vrr_off) < 0)
            {
                DLG_PRINT("send i2c cmd failed\n");
            }
        }

        if(hsr_vrr->wp_vrr_before_unmute_delay)
        {
            msleep(hsr_vrr->wp_vrr_before_unmute_delay);
        }
        else
        {
            msleep(150);
        }

        //3 unmute
        memset(cmd, 0, I2C_CMD_MAX_LEN);
        memcpy(cmd, hsr_vrr->i2c_cmd_unmute, I2C_CMD_MAX_LEN);
        if (i2c_master_send_ex(hsr_vrr->i2c_group_id, hsr_vrr->i2c_addr, cmd, hsr_vrr->i2c_len_unmute) < 0)
        {
            DLG_PRINT("send i2c cmd failed\n");
        }

        if(hsr_vrr->wp_vrr_after_unmute_delay)
        {
            msleep(hsr_vrr->wp_vrr_after_unmute_delay);
        }
        else
        {
            //msleep(100);
        }

        if(hsr_vrr->wp_enable == 1)
        {
            ret = IO_Direct_Set("PIN_HSR_I2C_WP", 0);
            if(ret == -1)
            {
                DLG_PRINT("[ERROR]set wp failed, please check it\n");
            }
        }
    }
}

void panel_dlg_customer_switch_panel_mode(PANEL_DLG_MODE panelDlgMode)
{
    DLG_PRINT("dlg for switch panel mode\n");
    //1 i2c control panel switch
    if(hsr_get_switch_panel_type() == 1)
    {
        const HSR_SWITCH_PANEL_MODE_I2C *switch_panel = hsr_get_switch_panel_info_i2c();
        if(switch_panel != NULL)
        {
            unsigned int i2c_index = HSR_NOMAL;
            int i = 0;
            int loop = 0;
            unsigned char cmd[I2C_CMD_MAX_LEN] = {0};
            int ret = 0;
            if(switch_panel->wp_enable == 1)
            {
                ret = IO_Direct_Set("PIN_HSR_I2C_WP", 1);
                if(ret == -1)
                {
                    DLG_PRINT("[ERROR]set wp failed, please check it\n");
                }
            }

            if(switch_panel->wp_hsr_before_write_data_delay > 0)
            {
                DLG_PRINT("[%s %d]switch_panel->wp_hsr_before_write_data_delay:%d\n", __FUNCTION__, __LINE__, switch_panel->wp_hsr_before_write_data_delay);
                msleep(switch_panel->wp_hsr_before_write_data_delay);
            }
            else
            {
                msleep(150);
            }

            i2c_index = panel_dlg_customer_get_hsr_index_by_dlg_mode(panelDlgMode);

            for(loop = 0; loop < switch_panel->i2c_info[i2c_index].operator; loop++)
            {
                for(i = 0; i < switch_panel->i2c_info[i2c_index].i2c_data[loop].i2c_len; i++)
                {
                    DLG_PRINT("cmd=%02x\n", switch_panel->i2c_info[i2c_index].i2c_data[loop].i2c_cmd[i]);
                }

                memset(cmd, 0, I2C_CMD_MAX_LEN);
                memcpy(cmd, switch_panel->i2c_info[i2c_index].i2c_data[loop].i2c_cmd, I2C_CMD_MAX_LEN);

                if(switch_panel->i2c_info[i2c_index].i2c_data[loop].i2c_len == 0)
                {
                    DLG_PRINT("Error i2c len is 0 please check it\n");
                }

                if (i2c_master_send_ex(switch_panel->i2c_info[i2c_index].i2c_data[loop].i2c_group_id, switch_panel->i2c_info[i2c_index].i2c_data[loop].i2c_addr, 
                    cmd, switch_panel->i2c_info[i2c_index].i2c_data[loop].i2c_len) < 0)
                {
                    DLG_PRINT("send i2c cmd failed\n");
                }
            }

            if(switch_panel->wp_hsr_after_write_data_delay > 0)
            {
                DLG_PRINT("[%s %d]switch_panel->wp_after_write_data_delay:%d\n", __FUNCTION__, __LINE__, switch_panel->wp_hsr_after_write_data_delay);
                msleep(switch_panel->wp_hsr_after_write_data_delay);
            }
            else
            {
                //msleep(100);
            }

            if(switch_panel->wp_enable == 1)
            {
                ret = IO_Direct_Set("PIN_HSR_I2C_WP", 0);
                if(ret == -1)
                {
                    DLG_PRINT("[ERROR]set wp failed, please check it\n");
                }
            }
        }
    }
    // 1 gpio control switch panel mode
    else if(hsr_get_switch_panel_type() == 2)
    {
        const HSR_SWITCH_PANEL_MODE_GPIO *p_gpio = NULL;
        int index = HSR_NOMAL;
        int ret = 0;
        p_gpio = hsr_get_switch_panel_info_gpio();

        index = panel_dlg_customer_get_hsr_index_by_dlg_mode(panelDlgMode);

        if(p_gpio->high[index] == 1)
        {
            ret = IO_Direct_Set("PIN_HSR_EN", 1);
            if(ret == -1)
            {
                DLG_PRINT("[ERROR]set hsr enable failed, please check it\n");
            }
        }
        else if(p_gpio->high[index] == 0)
        {
            ret = IO_Direct_Set("PIN_HSR_EN", 0);
            if(ret == -1)
            {
                DLG_PRINT("[ERROR]set hsr diable failed, please check it\n");
            }
        }
        else
        {
            DLG_PRINT("gpio config error index:%d\n", index);
        }
    }
    else
    {
        DLG_PRINT("Warning unkown switch panel mode\n");
    }
}

void panel_dlg_customer_get_default_dlg_parameter_info(PANEL_DLG_PARAMETER_INFO *panel_dlg_parameter_info)
{
    PANEL_CONFIG_PARAMETER panelConfigParameter = {0};

#ifndef UT_flag 
    memcpy(&panelConfigParameter, (PANEL_CONFIG_PARAMETER *)&platform_info.panel_parameter, sizeof(PANEL_CONFIG_PARAMETER));
#endif

    if(panel_dlg_parameter_info != NULL)
    {
        panel_dlg_parameter_info->refresh = panelConfigParameter.iCONFIG_DISPLAY_REFRESH_RATE;
        panel_dlg_parameter_info->typical = panelConfigParameter.iCONFIG_DISPLAY_CLOCK_TYPICAL;
        panel_dlg_parameter_info->htotal = panelConfigParameter.iCONFIG_DISP_HORIZONTAL_TOTAL;
        panel_dlg_parameter_info->vtotal = panelConfigParameter.iCONFIG_DISP_VERTICAL_TOTAL;
        panel_dlg_parameter_info->den_hstart = panelConfigParameter.iCONFIG_DISP_DEN_STA_HPOS;
        panel_dlg_parameter_info->den_hend = panelConfigParameter.iCONFIG_DISP_DEN_END_HPOS;
        panel_dlg_parameter_info->den_vstart = panelConfigParameter.iCONFIG_DISP_DEN_STA_VPOS;
        panel_dlg_parameter_info->den_vend = panelConfigParameter.iCONFIG_DISP_DEN_END_VPOS;
        panel_dlg_parameter_info->act_hstart = panelConfigParameter.iCONFIG_DISP_ACT_STA_HPOS;
        panel_dlg_parameter_info->act_hend = panelConfigParameter.iCONFIG_DISP_ACT_END_HPOS;
        panel_dlg_parameter_info->act_vstart = panelConfigParameter.iCONFIG_DISP_ACT_STA_VPOS;
        panel_dlg_parameter_info->act_vend = panelConfigParameter.iCONFIG_DISP_ACT_END_VPOS;
        panel_dlg_parameter_info->min_vtotal = panelConfigParameter.iCONFIG_DISP_VERTICAL_TOTAL_60Hz_MIN;
        panel_dlg_parameter_info->max_vtotal = panelConfigParameter.iCONFIG_DISP_VERTICAL_TOTAL_50Hz_MAX;
        panel_dlg_parameter_info->hsync_last_line = panelConfigParameter.iCONFIG_DISP_HSYNC_LASTLINE;
    }
}

void panel_dlg_customer_get_4k1k120_dlg_parameter_info(PANEL_DLG_PARAMETER_INFO *panel_dlg_parameter_info)
{
    DLG_PRINT("dlg for 4k1k120 panel parameter\n");

    if(panel_dlg_parameter_info != NULL)
    {
        const HSR_PANEL_PARAMETER_INFO *hsr_parameter_info = hsr_get_panel_parameter_info();
        if(hsr_parameter_info != NULL)
        {
            if(hsr_parameter_info->panel_parameter[HSR_4k1k_120].refresh == 0)
            {
                DLG_PRINT("Error get panel parameter info failed, please check it\n");
                panel_dlg_parameter_info->refresh = 120;
                if(panel_dlg_customer_get_down_sample())
                {
                    panel_dlg_parameter_info->typical = 594000000;
                    panel_dlg_parameter_info->htotal = 4400;
                    panel_dlg_parameter_info->vtotal = 2250;
                    panel_dlg_parameter_info->den_hstart = 280;
                    panel_dlg_parameter_info->den_hend = 4120;
                    panel_dlg_parameter_info->den_vstart = 45;
                    panel_dlg_parameter_info->den_vend = 2205;
                    panel_dlg_parameter_info->act_hstart = 0;
                    panel_dlg_parameter_info->act_hend = 3840;
                    panel_dlg_parameter_info->act_vstart = 0;
                    panel_dlg_parameter_info->act_vend = 2160;
                    panel_dlg_parameter_info->min_vtotal = 2210;
                    panel_dlg_parameter_info->max_vtotal = 8000;
                    panel_dlg_parameter_info->hsync_last_line = 4400;
                }
                else
                {
                    panel_dlg_parameter_info->typical = 594000000;
                    panel_dlg_parameter_info->htotal = 4400;
                    panel_dlg_parameter_info->vtotal = 1125;
                    panel_dlg_parameter_info->den_hstart = 280;
                    panel_dlg_parameter_info->den_hend = 4120;
                    panel_dlg_parameter_info->den_vstart = 20;
                    panel_dlg_parameter_info->den_vend = 1100;
                    panel_dlg_parameter_info->act_hstart = 0;
                    panel_dlg_parameter_info->act_hend = 3840;
                    panel_dlg_parameter_info->act_vstart = 0;
                    panel_dlg_parameter_info->act_vend = 1080;
                    panel_dlg_parameter_info->min_vtotal = 1100;
                    panel_dlg_parameter_info->max_vtotal = 8000;
                    panel_dlg_parameter_info->hsync_last_line = 4400;
                }
            }
            else
            {
                panel_dlg_parameter_info->refresh = hsr_parameter_info->panel_parameter[HSR_4k1k_120].refresh;
                panel_dlg_parameter_info->typical = hsr_parameter_info->panel_parameter[HSR_4k1k_120].typical;
                panel_dlg_parameter_info->htotal = hsr_parameter_info->panel_parameter[HSR_4k1k_120].htotal;
                panel_dlg_parameter_info->vtotal = hsr_parameter_info->panel_parameter[HSR_4k1k_120].vtotal;
                panel_dlg_parameter_info->den_hstart = hsr_parameter_info->panel_parameter[HSR_4k1k_120].den_hstart;
                panel_dlg_parameter_info->den_hend = hsr_parameter_info->panel_parameter[HSR_4k1k_120].den_hend;
                panel_dlg_parameter_info->den_vstart = hsr_parameter_info->panel_parameter[HSR_4k1k_120].den_vstart;
                panel_dlg_parameter_info->den_vend = hsr_parameter_info->panel_parameter[HSR_4k1k_120].den_vend;
                panel_dlg_parameter_info->act_hstart = hsr_parameter_info->panel_parameter[HSR_4k1k_120].act_hstart;
                panel_dlg_parameter_info->act_hend = hsr_parameter_info->panel_parameter[HSR_4k1k_120].act_hend;
                panel_dlg_parameter_info->act_vstart = hsr_parameter_info->panel_parameter[HSR_4k1k_120].act_vstart;
                panel_dlg_parameter_info->act_vend = hsr_parameter_info->panel_parameter[HSR_4k1k_120].act_vend;
                panel_dlg_parameter_info->min_vtotal = hsr_parameter_info->panel_parameter[HSR_4k1k_120].min_vtotal;
                panel_dlg_parameter_info->max_vtotal = hsr_parameter_info->panel_parameter[HSR_4k1k_120].max_vtotal;
                panel_dlg_parameter_info->hsync_last_line = hsr_parameter_info->panel_parameter[HSR_4k1k_120].hsync_last_line;
            }
        }
        else
        {
            DLG_PRINT("Error get panel parameter info failed, please check it\n");
        }
    }
}

void panel_dlg_customer_get_4k1k144_dlg_parameter_info(PANEL_DLG_PARAMETER_INFO *panel_dlg_parameter_info)
{
    DLG_PRINT("dlg for 4k1k120 panel parameter\n");

    if(panel_dlg_parameter_info != NULL)
    {
        const HSR_PANEL_PARAMETER_INFO *hsr_parameter_info = hsr_get_panel_parameter_info();
        if(hsr_parameter_info != NULL)
        {
            if(hsr_parameter_info->panel_parameter[HSR_4k1k_144].refresh == 0)
            {
                DLG_PRINT("Error get panel parameter info failed, please check it\n");
                panel_dlg_parameter_info->refresh = 144;
                if(panel_dlg_customer_get_down_sample())
                {
                    panel_dlg_parameter_info->typical = 671616000;
                    panel_dlg_parameter_info->htotal = 4240;
                    panel_dlg_parameter_info->vtotal = 2200;
                    panel_dlg_parameter_info->den_hstart = 200;
                    panel_dlg_parameter_info->den_hend = 4040;
                    panel_dlg_parameter_info->den_vstart = 20;
                    panel_dlg_parameter_info->den_vend = 2180;
                    panel_dlg_parameter_info->act_hstart = 0;
                    panel_dlg_parameter_info->act_hend = 3840;
                    panel_dlg_parameter_info->act_vstart = 0;
                    panel_dlg_parameter_info->act_vend = 2160;
                    panel_dlg_parameter_info->min_vtotal = 2200;
                    panel_dlg_parameter_info->max_vtotal = 8000;
                    panel_dlg_parameter_info->hsync_last_line = 4240;
                }
                else
                {
                    panel_dlg_parameter_info->typical = 671616000;
                    panel_dlg_parameter_info->htotal = 4240;
                    panel_dlg_parameter_info->vtotal = 1100;
                    panel_dlg_parameter_info->den_hstart = 200;
                    panel_dlg_parameter_info->den_hend = 4040;
                    panel_dlg_parameter_info->den_vstart = 10;
                    panel_dlg_parameter_info->den_vend = 1090;
                    panel_dlg_parameter_info->act_hstart = 0;
                    panel_dlg_parameter_info->act_hend = 3840;
                    panel_dlg_parameter_info->act_vstart = 0;
                    panel_dlg_parameter_info->act_vend = 1080;
                    panel_dlg_parameter_info->min_vtotal = 1100;
                    panel_dlg_parameter_info->max_vtotal = 8000;
                    panel_dlg_parameter_info->hsync_last_line = 4240;
                }
            }
            else
            {
                panel_dlg_parameter_info->refresh = hsr_parameter_info->panel_parameter[HSR_4k1k_144].refresh;
                panel_dlg_parameter_info->typical = hsr_parameter_info->panel_parameter[HSR_4k1k_144].typical;
                panel_dlg_parameter_info->htotal = hsr_parameter_info->panel_parameter[HSR_4k1k_144].htotal;
                panel_dlg_parameter_info->vtotal = hsr_parameter_info->panel_parameter[HSR_4k1k_144].vtotal;
                panel_dlg_parameter_info->den_hstart = hsr_parameter_info->panel_parameter[HSR_4k1k_144].den_hstart;
                panel_dlg_parameter_info->den_hend = hsr_parameter_info->panel_parameter[HSR_4k1k_144].den_hend;
                panel_dlg_parameter_info->den_vstart = hsr_parameter_info->panel_parameter[HSR_4k1k_144].den_vstart;
                panel_dlg_parameter_info->den_vend = hsr_parameter_info->panel_parameter[HSR_4k1k_144].den_vend;
                panel_dlg_parameter_info->act_hstart = hsr_parameter_info->panel_parameter[HSR_4k1k_144].act_hstart;
                panel_dlg_parameter_info->act_hend = hsr_parameter_info->panel_parameter[HSR_4k1k_144].act_hend;
                panel_dlg_parameter_info->act_vstart = hsr_parameter_info->panel_parameter[HSR_4k1k_144].act_vstart;
                panel_dlg_parameter_info->act_vend = hsr_parameter_info->panel_parameter[HSR_4k1k_144].act_vend;
                panel_dlg_parameter_info->min_vtotal = hsr_parameter_info->panel_parameter[HSR_4k1k_144].min_vtotal;
                panel_dlg_parameter_info->max_vtotal = hsr_parameter_info->panel_parameter[HSR_4k1k_144].max_vtotal;
                panel_dlg_parameter_info->hsync_last_line = hsr_parameter_info->panel_parameter[HSR_4k1k_144].hsync_last_line;
            }
        }
        else
        {
            DLG_PRINT("Error get panel parameter info failed, please check it\n");
        }
    }
}

void panel_dlg_customer_get_4k2k144_dlg_parameter_info(PANEL_DLG_PARAMETER_INFO *panel_dlg_parameter_info)
{
    DLG_PRINT("dlg for 4k1k144 panel parameter\n");

    if(panel_dlg_parameter_info != NULL)
    {
        const HSR_PANEL_PARAMETER_INFO *hsr_parameter_info = hsr_get_panel_parameter_info();
        if(hsr_parameter_info != NULL)
        {
            if(hsr_parameter_info->panel_parameter[HSR_4k2k_144].refresh == 0)
            {
                DLG_PRINT("Error get panel parameter info failed, please check it\n");
                panel_dlg_parameter_info->refresh = 144;
                panel_dlg_parameter_info->typical = 671616000;
                panel_dlg_parameter_info->htotal = 4240;
                panel_dlg_parameter_info->vtotal = 2200;
                panel_dlg_parameter_info->den_hstart = 200;
                panel_dlg_parameter_info->den_hend = 4040;
                panel_dlg_parameter_info->den_vstart = 20;
                panel_dlg_parameter_info->den_vend = 2180;
                panel_dlg_parameter_info->act_hstart = 0;
                panel_dlg_parameter_info->act_hend = 3840;
                panel_dlg_parameter_info->act_vstart = 0;
                panel_dlg_parameter_info->act_vend = 2160;
                panel_dlg_parameter_info->min_vtotal = 2200;
                panel_dlg_parameter_info->max_vtotal = 5800;
                panel_dlg_parameter_info->hsync_last_line = 4240;
            }
            else
            {
                panel_dlg_parameter_info->refresh = hsr_parameter_info->panel_parameter[HSR_4k2k_144].refresh;
                panel_dlg_parameter_info->typical = hsr_parameter_info->panel_parameter[HSR_4k2k_144].typical;
                panel_dlg_parameter_info->htotal = hsr_parameter_info->panel_parameter[HSR_4k2k_144].htotal;
                panel_dlg_parameter_info->vtotal = hsr_parameter_info->panel_parameter[HSR_4k2k_144].vtotal;
                panel_dlg_parameter_info->den_hstart = hsr_parameter_info->panel_parameter[HSR_4k2k_144].den_hstart;
                panel_dlg_parameter_info->den_hend = hsr_parameter_info->panel_parameter[HSR_4k2k_144].den_hend;
                panel_dlg_parameter_info->den_vstart = hsr_parameter_info->panel_parameter[HSR_4k2k_144].den_vstart;
                panel_dlg_parameter_info->den_vend = hsr_parameter_info->panel_parameter[HSR_4k2k_144].den_vend;
                panel_dlg_parameter_info->act_hstart = hsr_parameter_info->panel_parameter[HSR_4k2k_144].act_hstart;
                panel_dlg_parameter_info->act_hend = hsr_parameter_info->panel_parameter[HSR_4k2k_144].act_hend;
                panel_dlg_parameter_info->act_vstart = hsr_parameter_info->panel_parameter[HSR_4k2k_144].act_vstart;
                panel_dlg_parameter_info->act_vend = hsr_parameter_info->panel_parameter[HSR_4k2k_144].act_vend;
                panel_dlg_parameter_info->min_vtotal = hsr_parameter_info->panel_parameter[HSR_4k2k_144].min_vtotal;
                panel_dlg_parameter_info->max_vtotal = hsr_parameter_info->panel_parameter[HSR_4k2k_144].max_vtotal;
                panel_dlg_parameter_info->hsync_last_line = hsr_parameter_info->panel_parameter[HSR_4k2k_144].hsync_last_line;
            }
        }
        else
        {
            DLG_PRINT("Error get panel parameter info failed, please check it\n");
        }
    }
}

void panel_dlg_customer_get_4k1k240_dlg_parameter_info(PANEL_DLG_PARAMETER_INFO *panel_dlg_parameter_info)
{
    DLG_PRINT("dlg for 4k1k240 panel parameter\n");

    if(panel_dlg_parameter_info != NULL)
    {
        const HSR_PANEL_PARAMETER_INFO *hsr_parameter_info = hsr_get_panel_parameter_info();
        if(hsr_parameter_info != NULL)
        {
            if(hsr_parameter_info->panel_parameter[HSR_4k1k_240].refresh == 0)
            {
                DLG_PRINT("Error get panel parameter info failed, please check it\n");
                panel_dlg_parameter_info->refresh = 240;
                panel_dlg_parameter_info->typical = 594000000;
                panel_dlg_parameter_info->htotal = 4400;
                panel_dlg_parameter_info->vtotal = 1125;
                panel_dlg_parameter_info->den_hstart = 280;
                panel_dlg_parameter_info->den_hend = 4120;
                panel_dlg_parameter_info->den_vstart = 20;
                panel_dlg_parameter_info->den_vend = 1100;
                panel_dlg_parameter_info->act_hstart = 0;
                panel_dlg_parameter_info->act_hend = 3840;
                panel_dlg_parameter_info->act_vstart = 0;
                panel_dlg_parameter_info->act_vend = 1080;
                panel_dlg_parameter_info->min_vtotal = 1109;
                panel_dlg_parameter_info->max_vtotal = 8000;
                panel_dlg_parameter_info->hsync_last_line = 4400;
            }
            else
            {
                panel_dlg_parameter_info->refresh = hsr_parameter_info->panel_parameter[HSR_4k1k_240].refresh;
                panel_dlg_parameter_info->typical = hsr_parameter_info->panel_parameter[HSR_4k1k_240].typical;
                panel_dlg_parameter_info->htotal = hsr_parameter_info->panel_parameter[HSR_4k1k_240].htotal;
                panel_dlg_parameter_info->vtotal = hsr_parameter_info->panel_parameter[HSR_4k1k_240].vtotal;
                panel_dlg_parameter_info->den_hstart = hsr_parameter_info->panel_parameter[HSR_4k1k_240].den_hstart;
                panel_dlg_parameter_info->den_hend = hsr_parameter_info->panel_parameter[HSR_4k1k_240].den_hend;
                panel_dlg_parameter_info->den_vstart = hsr_parameter_info->panel_parameter[HSR_4k1k_240].den_vstart;
                panel_dlg_parameter_info->den_vend = hsr_parameter_info->panel_parameter[HSR_4k1k_240].den_vend;
                panel_dlg_parameter_info->act_hstart = hsr_parameter_info->panel_parameter[HSR_4k1k_240].act_hstart;
                panel_dlg_parameter_info->act_hend = hsr_parameter_info->panel_parameter[HSR_4k1k_240].act_hend;
                panel_dlg_parameter_info->act_vstart = hsr_parameter_info->panel_parameter[HSR_4k1k_240].act_vstart;
                panel_dlg_parameter_info->act_vend = hsr_parameter_info->panel_parameter[HSR_4k1k_240].act_vend;
                panel_dlg_parameter_info->min_vtotal = hsr_parameter_info->panel_parameter[HSR_4k1k_240].min_vtotal;
                panel_dlg_parameter_info->max_vtotal = hsr_parameter_info->panel_parameter[HSR_4k1k_240].max_vtotal;
                panel_dlg_parameter_info->hsync_last_line = hsr_parameter_info->panel_parameter[HSR_4k1k_240].hsync_last_line;
            }
        }
        else
        {
            DLG_PRINT("Error get panel parameter info failed, please check it\n");
        }
    }
}

void panel_dlg_customer_get_4k1k288_dlg_parameter_info(PANEL_DLG_PARAMETER_INFO *panel_dlg_parameter_info)
{
    DLG_PRINT("dlg for 4k1k288 panel parameter\n");

    if(panel_dlg_parameter_info != NULL)
    {
        const HSR_PANEL_PARAMETER_INFO *hsr_parameter_info = hsr_get_panel_parameter_info();
        if(hsr_parameter_info != NULL)
        {
            if(hsr_parameter_info->panel_parameter[HSR_4k1k_288].refresh == 0)
            {
                DLG_PRINT("Error get panel parameter info failed, please check it\n");
                panel_dlg_parameter_info->refresh = 288;
                panel_dlg_parameter_info->typical = 686880000;
                panel_dlg_parameter_info->htotal = 4240;
                panel_dlg_parameter_info->vtotal = 1125;
                panel_dlg_parameter_info->den_hstart = 200;
                panel_dlg_parameter_info->den_hend = 4040;
                panel_dlg_parameter_info->den_vstart = 20;
                panel_dlg_parameter_info->den_vend = 1100;
                panel_dlg_parameter_info->act_hstart = 0;
                panel_dlg_parameter_info->act_hend = 3840;
                panel_dlg_parameter_info->act_vstart = 0;
                panel_dlg_parameter_info->act_vend = 1080;
                panel_dlg_parameter_info->min_vtotal = 1122;
                panel_dlg_parameter_info->max_vtotal = 8000;
                panel_dlg_parameter_info->hsync_last_line = 4240;
            }
            else
            {
                panel_dlg_parameter_info->refresh = hsr_parameter_info->panel_parameter[HSR_4k1k_288].refresh;
                panel_dlg_parameter_info->typical = hsr_parameter_info->panel_parameter[HSR_4k1k_288].typical;
                panel_dlg_parameter_info->htotal = hsr_parameter_info->panel_parameter[HSR_4k1k_288].htotal;
                panel_dlg_parameter_info->vtotal = hsr_parameter_info->panel_parameter[HSR_4k1k_288].vtotal;
                panel_dlg_parameter_info->den_hstart = hsr_parameter_info->panel_parameter[HSR_4k1k_288].den_hstart;
                panel_dlg_parameter_info->den_hend = hsr_parameter_info->panel_parameter[HSR_4k1k_288].den_hend;
                panel_dlg_parameter_info->den_vstart = hsr_parameter_info->panel_parameter[HSR_4k1k_288].den_vstart;
                panel_dlg_parameter_info->den_vend = hsr_parameter_info->panel_parameter[HSR_4k1k_288].den_vend;
                panel_dlg_parameter_info->act_hstart = hsr_parameter_info->panel_parameter[HSR_4k1k_288].act_hstart;
                panel_dlg_parameter_info->act_hend = hsr_parameter_info->panel_parameter[HSR_4k1k_288].act_hend;
                panel_dlg_parameter_info->act_vstart = hsr_parameter_info->panel_parameter[HSR_4k1k_288].act_vstart;
                panel_dlg_parameter_info->act_vend = hsr_parameter_info->panel_parameter[HSR_4k1k_288].act_vend;
                panel_dlg_parameter_info->min_vtotal = hsr_parameter_info->panel_parameter[HSR_4k1k_288].min_vtotal;
                panel_dlg_parameter_info->max_vtotal = hsr_parameter_info->panel_parameter[HSR_4k1k_288].max_vtotal;
                panel_dlg_parameter_info->hsync_last_line = hsr_parameter_info->panel_parameter[HSR_4k1k_288].hsync_last_line;
            }
        }
        else
        {
            DLG_PRINT("Error get panel parameter info failed, please check it\n");
        }
    }
}

void panel_dlg_customer_init_down_sample(PANEL_DLG_MODE panelDlgMode)
{
    DLG_PRINT("dlg for init_down_sample\n");

    if(panelDlgMode == PANEL_DLG_4k1k120)
    {
        enable_down_sample = Get_DISPLAY_HSR_DOWN_SAMPLE();
    }
    else
    {
        enable_down_sample = 0;
    }
}

//sfg_down_sample
unsigned char panel_dlg_customer_get_down_sample(void)
{
    return enable_down_sample;
}

unsigned char panel_dlg_customer_get_bypass_memc(void)
{
    return s_bypass_memc;
}

void panel_dlg_customer_set_bypass_memc(unsigned char bypass)
{
    s_bypass_memc = bypass;
}

void panel_dlg_customer_init_memc_bypass(PANEL_DLG_MODE panelDlgMode)
{
    if(panelDlgMode == PANEL_DLG_4k1k120)
    {
        panel_dlg_customer_set_bypass_memc(0);
    }
    else
    {
        panel_dlg_customer_set_bypass_memc(0);
    }
}

static void panel_dlg_customer_set_dpll_clk(DPLL_TYPE dpllType)
{
    DLG_PRINT("dpllType:%d, Get_DISPLAY_PANEL_CUSTOM_INDEX():%d\n", dpllType, Get_DISPLAY_PANEL_CUSTOM_INDEX());
    panel_dlg_customer_clock_set_dclk(Get_DISPLAY_CLOCK_TYPICAL(), DPLL_NCODE_MODE);
}


static void panel_dlg_dclk_setting(void)
{
    sys_reg_sys_dclk_gate_sel0_RBUS sys_reg_sys_dclk_gate_sel0_reg;
    sys_reg_sys_dclk_gate_sel1_RBUS sys_reg_sys_dclk_gate_sel1_reg;
    ppoverlay_dispd_smooth_toggle1_RBUS ppoverlay_dispd_smooth_toggle1_reg;
    unsigned int count = 0xfffff;

    DLG_PRINT("step 1 for dclk gate sel setting\n");

    sys_reg_sys_dclk_gate_sel0_reg.regValue = IoReg_Read32(SYS_REG_SYS_DCLK_GATE_SEL0_reg);
    sys_reg_sys_dclk_gate_sel1_reg.regValue = IoReg_Read32(SYS_REG_SYS_DCLK_GATE_SEL1_reg);

    if(get_panel_pixel_mode() > PANEL_1_PIXEL)
    {
        sys_reg_sys_dclk_gate_sel0_reg.dclk_s1_f1p_gate_sel = 0;
        sys_reg_sys_dclk_gate_sel0_reg.dclk_s1_f2p_gate_sel = 0;
        sys_reg_sys_dclk_gate_sel0_reg.dclk_s2_f1p_gate_sel = 0;
        sys_reg_sys_dclk_gate_sel0_reg.dclk_s2_f2p_gate_sel = 0;
        sys_reg_sys_dclk_gate_sel0_reg.dclk_osd_f1p_gate_sel = 1;
        sys_reg_sys_dclk_gate_sel0_reg.dclk_osd_f2p_gate_sel = 2;
    }
    else
    {
        sys_reg_sys_dclk_gate_sel0_reg.dclk_s1_f1p_gate_sel = 0;
        sys_reg_sys_dclk_gate_sel0_reg.dclk_s1_f2p_gate_sel = 1;
        sys_reg_sys_dclk_gate_sel0_reg.dclk_s2_f1p_gate_sel = 0;
        sys_reg_sys_dclk_gate_sel0_reg.dclk_s2_f2p_gate_sel = 1;
        sys_reg_sys_dclk_gate_sel0_reg.dclk_osd_f1p_gate_sel = 1;
        sys_reg_sys_dclk_gate_sel0_reg.dclk_osd_f2p_gate_sel = 2;
    }

#ifndef UT_flag
    drvif_clock_set_memc_div(&sys_reg_sys_dclk_gate_sel0_reg, &sys_reg_sys_dclk_gate_sel1_reg);
#endif

    IoReg_Write32(SYS_REG_SYS_DCLK_GATE_SEL0_reg, sys_reg_sys_dclk_gate_sel0_reg.regValue);
    IoReg_Write32(SYS_REG_SYS_DCLK_GATE_SEL1_reg, sys_reg_sys_dclk_gate_sel1_reg.regValue);
    scaler_disp_update_stage1_linerate_ratio(LINERATE_UPDATE_S1_F2P_GATE_SEL);

    //stage1 clk mode revised default set to 1
    ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_clk_mode_revised = 1;
    ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_src_sel = 1;
    ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_apply = 1;
    IoReg_Write32(PPOVERLAY_dispd_smooth_toggle1_reg, ppoverlay_dispd_smooth_toggle1_reg.regValue);

    ppoverlay_dispd_smooth_toggle1_reg.regValue = IoReg_Read32(PPOVERLAY_dispd_smooth_toggle1_reg);

    while((ppoverlay_dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_apply)  && --count)
    {
        ppoverlay_dispd_smooth_toggle1_reg.regValue = IoReg_Read32(PPOVERLAY_dispd_smooth_toggle1_reg);
    }

    if(count == 0)
    {
        DLG_PRINT("dispd_smooth_toggle1 timeout !!!\n");
    }
}

static void panel_dlg_masterdtg_setting(void)
{
    ppoverlay_dh_total_last_line_length_RBUS dh_total_last_line_length_reg;
    ppoverlay_dv_length_RBUS dv_length_reg;
    ppoverlay_dv_total_RBUS dv_total_reg;
    ppoverlay_dh_width_RBUS ppoverlay_dh_width_reg;

    DLG_PRINT("step 3 for master dtg setting\n");

    //Master DTG
    dh_total_last_line_length_reg.regValue = IoReg_Read32(PPOVERLAY_DH_Total_Last_Line_Length_reg);
    dh_total_last_line_length_reg.dh_total = Get_DISP_HORIZONTAL_TOTAL() - 1;
    dh_total_last_line_length_reg.dh_total_last_line = Get_DISP_HORIZONTAL_TOTAL() - 1;
    IoReg_Write32(PPOVERLAY_DH_Total_Last_Line_Length_reg, dh_total_last_line_length_reg.regValue);

    ppoverlay_dh_width_reg.regValue = IoReg_Read32(PPOVERLAY_DH_Width_reg);
    ppoverlay_dh_width_reg.dh_width = Get_DISP_HSYNC_WIDTH();
    IoReg_Write32(PPOVERLAY_DH_Width_reg, ppoverlay_dh_width_reg.regValue);

    dv_length_reg.regValue = IoReg_Read32(PPOVERLAY_DV_Length_reg);
    dv_length_reg.dv_length = Get_DISP_VSYNC_LENGTH();
    IoReg_Write32(PPOVERLAY_DV_Length_reg, dv_length_reg.regValue);

    dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_DV_total_reg);
    dv_total_reg.dv_total = Get_DISP_VERTICAL_TOTAL() - 1;
    IoReg_Write32(PPOVERLAY_DV_total_reg, dv_total_reg.regValue);
}

static void panel_dlg_uzudtg_setting(void)
{
    ppoverlay_uzudtg_dv_total_RBUS uzudtg_dv_total_reg;
    ppoverlay_uzudtg_dh_total_RBUS uzudtg_dh_total_reg;
    ppoverlay_dh_den_start_end_RBUS dh_den_start_end_reg;
    ppoverlay_dv_den_start_end_RBUS dv_den_start_end_reg;
    ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;
    scaleup_dm_uzu_input_size_RBUS scaleup_dm_uzu_input_size_reg;
    scaleup_ds_uzu_input_size_RBUS scaleup_ds_uzu_input_size_reg;
    two_step_uzu_dm_two_step_sr_input_size_RBUS two_step_uzu_dm_two_step_sr_input_size_reg;
    two_step_uzu_dm_two_step_sr_ctrl_RBUS two_step_uzu_dm_two_step_sr_ctrl_reg;

    DLG_PRINT("step 4 for uzu dtg setting\n");

    //UZU DTG
    uzudtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DV_TOTAL_reg);
    uzudtg_dv_total_reg.uzudtg_dv_total = Get_DISP_VERTICAL_TOTAL() - 1;
    IoReg_Write32(PPOVERLAY_uzudtg_DV_TOTAL_reg, uzudtg_dv_total_reg.regValue);

    uzudtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DH_TOTAL_reg);
    uzudtg_dh_total_reg.uzudtg_dh_total = Get_DISP_HORIZONTAL_TOTAL() - 1;
    uzudtg_dh_total_reg.uzudtg_dh_total_last_line = Get_DISP_HORIZONTAL_TOTAL() - 1;
    IoReg_Write32(PPOVERLAY_uzudtg_DH_TOTAL_reg, uzudtg_dh_total_reg.regValue);
	scaler_disp_update_stage1_linerate_ratio(LINERATE_UPDATE_UZUDTG_DH_TOTAL);

    dh_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_DH_DEN_Start_End_reg);
    dh_den_start_end_reg.dh_den_sta = Get_DISP_DEN_STA_HPOS();
    dh_den_start_end_reg.dh_den_end = Get_DISP_DEN_END_HPOS();
    IoReg_Write32(PPOVERLAY_DH_DEN_Start_End_reg, dh_den_start_end_reg.regValue);

    dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_DV_DEN_Start_End_reg);
    dv_den_start_end_reg.dv_den_sta = Get_DISP_DEN_STA_VPOS();
    dv_den_start_end_reg.dv_den_end = Get_DISP_DEN_END_VPOS();
    IoReg_Write32(PPOVERLAY_DV_DEN_Start_End_reg, dv_den_start_end_reg.regValue);
    
    if(get_panel_pixel_mode() > PANEL_1_PIXEL)
    {
        ppoverlay_uzudtg_control1_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control1_reg);
        ppoverlay_uzudtg_control1_reg.uzu_4k_120_mode = 1;
        IoReg_Write32(PPOVERLAY_uzudtg_control1_reg, ppoverlay_uzudtg_control1_reg.regValue);

        scaleup_dm_uzu_input_size_reg.regValue = IoReg_Read32(SCALEUP_DM_UZU_Input_Size_reg);
        scaleup_dm_uzu_input_size_reg.hor_input_size = Get_DISP_ACT_END_HPOS() - Get_DISP_ACT_STA_HPOS();
        scaleup_dm_uzu_input_size_reg.ver_input_size = Get_DISP_ACT_END_VPOS() - Get_DISP_ACT_STA_VPOS();
        IoReg_Write32(SCALEUP_DM_UZU_Input_Size_reg, scaleup_dm_uzu_input_size_reg.regValue);

        scaleup_ds_uzu_input_size_reg.regValue = IoReg_Read32(SCALEUP_DS_UZU_Input_Size_reg);
        scaleup_ds_uzu_input_size_reg.hor_input_size = Get_DISP_ACT_END_HPOS() - Get_DISP_ACT_STA_HPOS();
        scaleup_ds_uzu_input_size_reg.ver_input_size = Get_DISP_ACT_END_VPOS() - Get_DISP_ACT_STA_VPOS();
        IoReg_Write32(SCALEUP_DS_UZU_Input_Size_reg, scaleup_ds_uzu_input_size_reg.regValue);

        two_step_uzu_dm_two_step_sr_input_size_reg.regValue = IoReg_Read32(TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_reg);
        two_step_uzu_dm_two_step_sr_input_size_reg.hor_input_size = Get_DISP_ACT_END_HPOS() - Get_DISP_ACT_STA_HPOS();
        two_step_uzu_dm_two_step_sr_input_size_reg.ver_input_size = Get_DISP_ACT_END_VPOS() - Get_DISP_ACT_STA_VPOS();
        IoReg_Write32(TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_reg, two_step_uzu_dm_two_step_sr_input_size_reg.regValue);
    
        two_step_uzu_dm_two_step_sr_ctrl_reg.regValue = IoReg_Read32(TWO_STEP_UZU_DM_TWO_STEP_SR_Ctrl_reg);
        two_step_uzu_dm_two_step_sr_ctrl_reg.sr_mdom_sub_en = 1;
        IoReg_Write32(TWO_STEP_UZU_DM_TWO_STEP_SR_Ctrl_reg, two_step_uzu_dm_two_step_sr_ctrl_reg.regValue);
    }
    else
    {
        ppoverlay_uzudtg_control1_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control1_reg);
        ppoverlay_uzudtg_control1_reg.uzu_4k_120_mode = 0;
        IoReg_Write32(PPOVERLAY_uzudtg_control1_reg, ppoverlay_uzudtg_control1_reg.regValue);

        two_step_uzu_dm_two_step_sr_ctrl_reg.regValue = IoReg_Read32(TWO_STEP_UZU_DM_TWO_STEP_SR_Ctrl_reg);
        two_step_uzu_dm_two_step_sr_ctrl_reg.sr_mdom_sub_en = 0;
        IoReg_Write32(TWO_STEP_UZU_DM_TWO_STEP_SR_Ctrl_reg, two_step_uzu_dm_two_step_sr_ctrl_reg.regValue);
    }

}

static void panel_dlg_memcdtg_setting(void)
{
    ppoverlay_memc_mux_ctrl_bg_RBUS ppoverlay_memc_mux_ctrl_bg_reg;
    ppoverlay_memcdtg_dv_total_RBUS memcdtg_dv_total_reg;
    ppoverlay_memcdtg_dh_total_RBUS memcdtg_dh_total_reg;
    ppoverlay_memcdtg_dv_den_start_end_RBUS memcdtg_dv_den_start_end_reg;
    ppoverlay_memcdtg_dh_den_start_end_RBUS memcdtg_dh_den_start_end_reg;

    DLG_PRINT("step 5 for memc dtg setting\n");

    //MEMC DTG setting
    memcdtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DV_TOTAL_reg);
    memcdtg_dv_total_reg.memcdtg_dv_total = Get_DISP_VERTICAL_TOTAL() - 1;
    IoReg_Write32(PPOVERLAY_memcdtg_DV_TOTAL_reg, memcdtg_dv_total_reg.memcdtg_dv_total);

    memcdtg_dh_total_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DH_TOTAL_reg);
    memcdtg_dh_total_reg.memcdtg_dh_total = Get_DISP_HORIZONTAL_TOTAL() - 1;
    memcdtg_dh_total_reg.memcdtg_dh_total_last_line = Get_DISP_HORIZONTAL_TOTAL() - 1;
    IoReg_Write32(PPOVERLAY_memcdtg_DH_TOTAL_reg, memcdtg_dh_total_reg.regValue);

    memcdtg_dh_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DH_DEN_Start_End_reg);
    memcdtg_dh_den_start_end_reg.memcdtg_dh_den_sta = Get_DISP_DEN_STA_HPOS();
    memcdtg_dh_den_start_end_reg.memcdtg_dh_den_end = Get_DISP_DEN_END_HPOS();
    IoReg_Write32(PPOVERLAY_memcdtg_DH_DEN_Start_End_reg, memcdtg_dh_den_start_end_reg.regValue);

    memcdtg_dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg);
    memcdtg_dv_den_start_end_reg.memcdtg_dv_den_sta = Get_DISP_DEN_STA_VPOS();
    memcdtg_dv_den_start_end_reg.memcdtg_dv_den_end = Get_DISP_DEN_END_VPOS();
    IoReg_Write32(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg, memcdtg_dv_den_start_end_reg.regValue);

    ppoverlay_memc_mux_ctrl_bg_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg);
    ppoverlay_memc_mux_ctrl_bg_reg.memc_out_bg_en = 1;
    IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg, ppoverlay_memc_mux_ctrl_bg_reg.regValue);

}

static void panel_dlg_mtg_setting(void)
{
    ppoverlay_main_den_v_start_end_RBUS main_den_v_start_end_reg;
    ppoverlay_main_background_h_start_end_RBUS main_background_h_start_end_reg;
    ppoverlay_main_active_h_start_end_RBUS main_active_h_start_end_reg;
    ppoverlay_main_background_v_start_end_RBUS main_background_v_start_end_reg;
    ppoverlay_main_active_v_start_end_RBUS main_active_v_start_end_reg;
    ppoverlay_main_den_h_start_end_RBUS main_den_h_start_end_reg;

    DLG_PRINT("step 6 for mtg setting\n");

    //MTG
    main_den_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_DEN_H_Start_End_reg);
    main_den_h_start_end_reg.mh_den_sta = Get_DISP_ACT_STA_HPOS();
    main_den_h_start_end_reg.mh_den_end = Get_DISP_ACT_END_HPOS();
    IoReg_Write32(PPOVERLAY_MAIN_DEN_H_Start_End_reg, main_den_h_start_end_reg.regValue);

    main_den_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_DEN_V_Start_End_reg);
    main_den_v_start_end_reg.mv_den_sta = Get_DISP_ACT_STA_VPOS();
    main_den_v_start_end_reg.mv_den_end = Get_DISP_ACT_END_VPOS();
    IoReg_Write32(PPOVERLAY_MAIN_DEN_V_Start_End_reg, main_den_v_start_end_reg.regValue);

    main_background_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Background_H_Start_End_reg);
    main_background_h_start_end_reg.mh_bg_sta = Get_DISP_ACT_STA_HPOS();
    main_background_h_start_end_reg.mh_bg_end = Get_DISP_ACT_END_HPOS();
    IoReg_Write32(PPOVERLAY_MAIN_Background_H_Start_End_reg, main_background_h_start_end_reg.regValue);

    main_background_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Background_V_Start_End_reg);
    main_background_v_start_end_reg.mv_bg_sta = Get_DISP_ACT_STA_VPOS();
    main_background_v_start_end_reg.mv_bg_end = Get_DISP_ACT_END_VPOS();
    IoReg_Write32(PPOVERLAY_MAIN_Background_V_Start_End_reg, main_background_v_start_end_reg.regValue);

    main_active_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Active_H_Start_End_reg);
    main_active_h_start_end_reg.mh_act_sta = Get_DISP_ACT_STA_HPOS();
    main_active_h_start_end_reg.mh_act_end = Get_DISP_ACT_END_HPOS();
    IoReg_Write32(PPOVERLAY_MAIN_Active_H_Start_End_reg, main_active_h_start_end_reg.regValue);

    main_active_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Active_V_Start_End_reg);
    main_active_v_start_end_reg.mv_act_sta = Get_DISP_ACT_STA_VPOS();
    main_active_v_start_end_reg.mv_act_end = Get_DISP_ACT_END_VPOS();
    IoReg_Write32(PPOVERLAY_MAIN_Active_V_Start_End_reg, main_active_v_start_end_reg.regValue);

}

static void panel_dlg_sfg_setting(void)
{
    sfg_sfg_ctrl_24_RBUS sfg_sfg_ctrl_24_reg;
    sfg_sfg_ctrl_25_RBUS sfg_sfg_ctrl_25_reg;
    od_od_ctrl_RBUS od_od_ctrl_reg;
    sfg_sfg_ctrl_26_RBUS sfg_sfg_ctrl_26_reg;
    sfg_sfg_ctrl_c_RBUS sfg_sfg_ctrl_c_reg;
    sfg_sfg_ctrl_6_RBUS sfg_ctrl_6_reg;
    sfg_sfg_ctrl_0_RBUS sfg_ctrl_0_reg;

    DLG_PRINT("step 8 for sfg setting\n");

    //od
    od_od_ctrl_reg.regValue = IoReg_Read32(OD_OD_CTRL_reg);
    if(get_panel_pixel_mode() > PANEL_1_PIXEL)
    {
        od_od_ctrl_reg.od_4k120_mode = 1;
    }
    else
    {
        od_od_ctrl_reg.od_4k120_mode = 0;
    }
    IoReg_Write32(OD_OD_CTRL_reg, od_od_ctrl_reg.regValue);

    sfg_sfg_ctrl_24_reg.regValue = IoReg_Read32(SFG_SFG_CTRL_24_reg);
    sfg_sfg_ctrl_24_reg.dv_den_sta = Get_DISP_DEN_STA_VPOS();
    sfg_sfg_ctrl_24_reg.dv_den_end = Get_DISP_DEN_END_VPOS();
    IoReg_Write32(SFG_SFG_CTRL_24_reg, sfg_sfg_ctrl_24_reg.regValue);

    sfg_sfg_ctrl_25_reg.regValue = IoReg_Read32(SFG_SFG_CTRL_25_reg);
    if(get_panel_pixel_mode() > PANEL_1_PIXEL)
    {
        sfg_sfg_ctrl_25_reg.dh_den_sta = Get_DISP_DEN_STA_HPOS() / 2;
        sfg_sfg_ctrl_25_reg.dh_den_end = Get_DISP_DEN_END_HPOS() / 2;
    }
    else
    {
        sfg_sfg_ctrl_25_reg.dh_den_sta = Get_DISP_DEN_STA_HPOS();
        sfg_sfg_ctrl_25_reg.dh_den_end = Get_DISP_DEN_END_HPOS();
    }
    IoReg_Write32(SFG_SFG_CTRL_25_reg, sfg_sfg_ctrl_25_reg.regValue);

    sfg_sfg_ctrl_26_reg.regValue = IoReg_Read32(SFG_SFG_CTRL_26_reg);

    if(get_panel_pixel_mode() > PANEL_1_PIXEL)
    {
        sfg_sfg_ctrl_26_reg.dv_width = Get_DISP_VSYNC_LENGTH();
        sfg_sfg_ctrl_26_reg.dh_width = (Get_DISP_HSYNC_WIDTH() + 1) / 2 - 1;
        sfg_sfg_ctrl_26_reg.dh_total = Get_DISP_HORIZONTAL_TOTAL() / 2 - 1;
    }
    else
    {
        sfg_sfg_ctrl_26_reg.dv_width = Get_DISP_VSYNC_LENGTH();
        sfg_sfg_ctrl_26_reg.dh_width = Get_DISP_HSYNC_WIDTH();
        sfg_sfg_ctrl_26_reg.dh_total = Get_DISP_HORIZONTAL_TOTAL() - 1;
    }
    IoReg_Write32(SFG_SFG_CTRL_26_reg, sfg_sfg_ctrl_26_reg.regValue);

    sfg_sfg_ctrl_c_reg.regValue = IoReg_Read32(SFG_SFG_CTRL_C_reg);
    if(get_panel_pixel_mode() > PANEL_1_PIXEL)
    {
        sfg_sfg_ctrl_c_reg.hs_io_dist = Get_DISP_DEN_END_HPOS() / 2;
        sfg_sfg_ctrl_c_reg.hs_den_io_dist = Get_DISP_DEN_STA_HPOS() / 2;
    }
    else
    {
        sfg_sfg_ctrl_c_reg.hs_io_dist = Get_DISP_DEN_END_HPOS();
        sfg_sfg_ctrl_c_reg.hs_den_io_dist = Get_DISP_DEN_STA_HPOS();
    }
    IoReg_Write32(SFG_SFG_CTRL_C_reg, sfg_sfg_ctrl_c_reg.regValue);

    sfg_ctrl_6_reg.regValue = IoReg_Read32(SFG_SFG_CTRL_6_reg);
    sfg_ctrl_6_reg.downsample_v = panel_dlg_customer_get_down_sample();
    IoReg_Write32(SFG_SFG_CTRL_6_reg, sfg_ctrl_6_reg.regValue);

    sfg_ctrl_0_reg.regValue = IoReg_Read32(SFG_SFG_CTRL_0_reg);
    if(get_panel_pixel_mode() > PANEL_1_PIXEL)
    {
        sfg_ctrl_0_reg.pif_pixel_mode = 1;
    }
    else
    {
        sfg_ctrl_0_reg.pif_pixel_mode = 0;
    }

    IoReg_Write32(SFG_SFG_CTRL_0_reg, sfg_ctrl_0_reg.regValue);
}

static void panel_dlg_enable_double_buffer(void)
{
    ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
    ppoverlay_double_buffer_ctrl2_RBUS double_buffer_ctrl2_reg;
    ppoverlay_double_buffer_ctrl3_RBUS ppoverlay_double_buffer_ctrl3_reg;

    DLG_PRINT("step 2 for enable buffer setting\n");

    double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
    double_buffer_ctrl_reg.dreg_dbuf_en = 1;
    double_buffer_ctrl_reg.dmainreg_dbuf_en = 1;
    double_buffer_ctrl_reg.drm_multireg_dbuf_en = 1;
    IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg,double_buffer_ctrl_reg.regValue);

    double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
    double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 1;
    double_buffer_ctrl2_reg.memcdtgreg_dbuf_en = 1;
    IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl2_reg.regValue);

    ppoverlay_double_buffer_ctrl3_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
    ppoverlay_double_buffer_ctrl3_reg.d1bg_dtgreg_dbuf_en = 1;
    ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_en = 1;
    ppoverlay_double_buffer_ctrl3_reg.dmainreg_other_dbuf_en = 1;
    IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
}

static void panel_dlg_apply_double_buffer(void)
{
    ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
    ppoverlay_double_buffer_ctrl2_RBUS double_buffer_ctrl2_reg;
    ppoverlay_double_buffer_ctrl3_RBUS ppoverlay_double_buffer_ctrl3_reg;
    unsigned int count = 0;

    DLG_PRINT("step 7 for apply buffer setting\n");

    double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
    double_buffer_ctrl_reg.dreg_dbuf_set = 1;
    double_buffer_ctrl_reg.dmainreg_dbuf_set = 1;
    double_buffer_ctrl_reg.drm_multireg_dbuf_set = 1;
    IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg,double_buffer_ctrl_reg.regValue);

    double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
    double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 1;
    double_buffer_ctrl2_reg.memc_dtgreg_dbuf_set = 1;
    IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl2_reg.regValue);

    ppoverlay_double_buffer_ctrl3_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
    ppoverlay_double_buffer_ctrl3_reg.d1bg_dtgreg_dbuf_set = 1;
    ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_set = 1;
    ppoverlay_double_buffer_ctrl3_reg.dmainreg_other_dbuf_set = 1;
    IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);

    count = 0xfffff;
    while((double_buffer_ctrl_reg.dreg_dbuf_set || double_buffer_ctrl_reg.dmainreg_dbuf_set || double_buffer_ctrl_reg.drm_multireg_dbuf_set
        || double_buffer_ctrl2_reg.uzudtgreg_dbuf_set || double_buffer_ctrl2_reg.memc_dtgreg_dbuf_set || ppoverlay_double_buffer_ctrl3_reg.d1bg_dtgreg_dbuf_set
        || ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_set || ppoverlay_double_buffer_ctrl3_reg.dmainreg_other_dbuf_set) && --count)
    {
        double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
        double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
        ppoverlay_double_buffer_ctrl3_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
        usleep_range(10000, 10000);
    }

    if(count == 0)
    {
        DLG_PRINT("dtg apply timeout\n");
    }
}

static void panel_dlg_set_dpll(void)
{
    if(Get_DISPLAY_HSR_MODE() == PANEL_HSR_4k1k288)
    {
        if(Get_DEFAULT_DISPLAY_REFRESH_RATE() == 144)
        {
            return;
        }

        if(((Get_DISPLAY_REFRESH_RATE() == 144) || (Get_DISPLAY_REFRESH_RATE() == 288))
                && (get_panel_pixel_mode() > PANEL_1_PIXEL))
        {
            panel_dlg_customer_set_dpll_clk(DPLL_4k2k144);
        }
        else
        {
            panel_dlg_customer_set_dpll_clk(DPLL_4k2k120);
        }
    }
}

void panel_dlg_customer_pmic_setting_by_config(PANEL_DLG_MODE panelDlgMode)
{
    const HSR_PMIC_SETTING *hsr_pmic = NULL;
    PMIC_DATA_INFO data_info = {0};
    int i = 0;
    HSR_MODE_TYPE hsrIndex = HSR_NOMAL;
    HSR_PMIC_DATA pmicData;

    hsrIndex = panel_dlg_customer_get_hsr_index_by_dlg_mode(panelDlgMode);

    hsr_pmic = hsr_get_pmic_setting_by_type(hsrIndex);

    memset(&pmicData, 0, sizeof(HSR_PMIC_DATA));

    if((hsr_pmic != NULL) && (hsr_pmic->pmic_data[hsrIndex].len > 0))
    {
        memcpy(&pmicData, &hsr_pmic->pmic_data[hsrIndex], sizeof(HSR_PMIC_DATA));

        //update auto p gamma to pmic
        if(panel_dlg_customer_pmic_update_auto_p_gamma_data(&pmicData) != 0)
        {
            DLG_PRINT("[Error] pmic data update failed\n");
        }

        data_info.u8_reg = hsr_pmic->reg_addr;

        DLG_PRINT("data_info.u8_reg:0x%02x\n", data_info.u8_reg);

        memcpy(data_info.data, pmicData.pmic_bin, PMIC_BIN_MAX_SIZE);

        for(i = 0; i < hsr_pmic->pmic_data[hsrIndex].len; i++)
        {
            DLG_PRINT("pmic=0x%02x\n", data_info.data[i]);
        }

#ifndef UT_flag
        if (i2c_master_send_ex(hsr_pmic->i2c_group_id, hsr_pmic->i2c_addr, (unsigned char *)&data_info, hsr_pmic->pmic_data[hsrIndex].len + 1) < 0)
        {
            DLG_PRINT("send i2c cmd failed\n");
        }
#endif
    }
}


void panel_dlg_customer_timing_setting(PANEL_DLG_MODE panelDlgMode)
{
     //tconless panel need disable tcon fisrt
    if((Get_DISPLAY_PANEL_TYPE() == P_CEDS) || (Get_DISPLAY_PANEL_TYPE() == P_ISP) || (Get_DISPLAY_PANEL_TYPE() == P_CSPI) || (Get_DISPLAY_PANEL_TYPE() == P_CMPI))
    {
        tcon_tcon_ip_en_RBUS tcon_tcon_ip_en_reg;

        tcon_tcon_ip_en_reg.regValue = IoReg_Read32(TCON_TCON_IP_EN_reg);
        tcon_tcon_ip_en_reg.tcon_en = 0;
        tcon_tcon_ip_en_reg.tcon_across_frame_en = 0;
        tcon_tcon_ip_en_reg.v_vs_start = 0;

        IoReg_Write32(TCON_TCON_IP_EN_reg, tcon_tcon_ip_en_reg.regValue); 

        DLG_PRINT("tconless hsr need disable tcon\n");
        
        msleep(60);
    }

    panel_dlg_sfg_setting();

    panel_dlg_dclk_setting();

    panel_dlg_enable_double_buffer();

    panel_dlg_masterdtg_setting();

    panel_dlg_uzudtg_setting();

    panel_dlg_memcdtg_setting();

    panel_dlg_mtg_setting();

    panel_dlg_apply_double_buffer();

    panel_dlg_set_dpll();

    if((Get_DISPLAY_PANEL_TYPE() == P_CEDS) || (Get_DISPLAY_PANEL_TYPE() == P_ISP) || (Get_DISPLAY_PANEL_TYPE() == P_CSPI) || (Get_DISPLAY_PANEL_TYPE() == P_CMPI))
    {
        panel_dlg_customer_tconless_setting(panelDlgMode);
    }

    //wait for timing stable
    msleep(60);
}

static void panel_dlg_customer_tconless_setting_by_config(PANEL_DLG_MODE panelDlgMode)
{
    const HSR_TCON_SETTING *p_tcon_setting = NULL;
    int i = 0;
    HSR_MODE_TYPE hsrIndex = HSR_NOMAL;

    hsrIndex = panel_dlg_customer_get_hsr_index_by_dlg_mode(panelDlgMode);

    p_tcon_setting = hsr_get_tcon_setting_by_type(hsrIndex);

    if(p_tcon_setting == NULL)
    {
        DLG_PRINT("Error tcon config is NULL\n");
        return;
    }

    if(p_tcon_setting->tcon_data[hsrIndex].len > 0)
    {
        for(i = 0; i < p_tcon_setting->tcon_data[hsrIndex].len / sizeof(TCON_SETTING_DATA); i++)
        {
            switch(p_tcon_setting->tcon_data[hsrIndex].tcon[i].type)
            {
                case 0:
                    //DLG_PRINT("IoReg_Write32(0x%08x,0x%08x)\n", p_tcon_setting->tcon[i].addr,p_tcon_setting->tcon[i].value1);
                    IoReg_Write32(p_tcon_setting->tcon_data[hsrIndex].tcon[i].addr, p_tcon_setting->tcon_data[hsrIndex].tcon[i].value1);
                    break;

                case 1:
                    //DLG_PRINT("IoReg_Mask32(0x%08x,0x%08x,0x%08x)\n",p_tcon_setting->tcon[i].addr,p_tcon_setting->tcon[i].value1,p_tcon_setting->tcon[i].value2);
                    IoReg_Mask32(p_tcon_setting->tcon_data[hsrIndex].tcon[i].addr, p_tcon_setting->tcon_data[hsrIndex].tcon[i].value1, p_tcon_setting->tcon_data[hsrIndex].tcon[i].value2);
                    break;

                case 2:
                    break;

                case 3:
                    msleep(p_tcon_setting->tcon_data[hsrIndex].tcon[i].addr);
                    break;

                default:
                    break;

            }
        }
    }
}


void panel_dlg_customer_tconless_setting(PANEL_DLG_MODE panelDlgMode)
{
    panel_dlg_customer_tconless_setting_by_config(panelDlgMode);
}

#endif

