#ifndef BUILD_QUICK_SHOW
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/pm_runtime.h>
#if IS_ENABLED(CONFIG_REALTEK_PCBMGR)
#include <mach/pcbMgr.h>
#endif
#include <linux/version.h>
#include <linux/slab.h>
#else
#include <sysdefs.h>
#include <malloc.h>
#include <timer.h>
#include <string.h>
#include <vsprintf.h>
#include <rtk_kdriver/pcbMgr.h>
#endif
#include <mach/platform.h>
#include <mach/rtk_platform.h>
#include <rtk_kdriver/rtk_gpio.h>
#include "rtk_amp_interface.h"
#include <rtk_kdriver/quick_show/quick_show.h>


#define MULTI_AMP_CNT 4

#define AMP_MAJOR  0
#define DRV_NAME   "RTKamp"

static struct amp_controller rtk_amp[MULTI_AMP_CNT];
static const char  drv_name[] = DRV_NAME;
static unsigned long amp_table_phy_address = 0;
static unsigned int str_resume =0;
static unsigned int str_init_amp =0;
static unsigned int power_off_mode = 0;

static unsigned int amp_boot_sel[MULTI_AMP_CNT] = {0};
static unsigned int amp_boot_addr[MULTI_AMP_CNT] = {0};
AMP_DEVICE_ID_E amp_sel[MULTI_AMP_CNT] = {
    TI_TAS5707,
    TI_TAS5707,
    TI_TAS5707,
    TI_TAS5707,
};
static char *p_amp_dev_addr[MULTI_AMP_CNT] = {
    "AMP_DEVICE_ADDR",
    "AMP_DEVICE_ADDR",
    "AMP_DEVICE_ADDR",
    "AMP_DEVICE_ADDR"
};

#ifndef BUILD_QUICK_SHOW
static struct workqueue_struct *ampWq = NULL;
static struct delayed_work amp_delayWork;

extern platform_info_t platform_info;

MODULE_LICENSE("Dual BSD/GPL");

#ifdef CONFIG_PM
static int amp_suspend(struct device *dev);
static int amp_resume(struct device *dev);
static int rtk_amp_probe(struct platform_device *pdev);
static void rtk_amp_shutdown(struct platform_device *pdev);
int amp_pm_runtime_suspend(struct device *dev);
int amp_pm_runtime_resume(struct device *dev);

static struct platform_device *amp_devs;

static const struct dev_pm_ops amp_pm_ops = {
        .suspend    = amp_suspend,
        .resume     = amp_resume,
        .runtime_suspend    = amp_pm_runtime_suspend,
        .runtime_resume    = amp_pm_runtime_resume,
};
#endif

static struct platform_driver amp_driver = {
        .driver = {
                .name         = (char *)drv_name,
                .bus          = &platform_bus_type,
#ifdef CONFIG_PM
                .pm           = &amp_pm_ops,
#endif
        },
        .probe = rtk_amp_probe,
        .shutdown	 = rtk_amp_shutdown,
};

#else

#define __init
#define early_param(x, x1)
extern int sscanf(const char *buf, const char *fmt, ...);
extern char *getenv(const char *name);
extern char * strsep(char **,const char *);

#endif

void rtk_amp_ops_set(int amp_num, int sel)
{
    memset(rtk_amp[amp_num].name, 0, sizeof(rtk_amp[amp_num].name));
    switch(sel){
        case RTK_ALC1310:
            rtk_amp[amp_num].ops = alc1310_func;
            rtk_amp[amp_num].addr_size = ALC1310_ADDR_SIZE;
            rtk_amp[amp_num].data_size = ALC1310_DATA_SIZE;
            rtk_amp[amp_num].param_get = alc1310_param_get;
            rtk_amp[amp_num].param_set = alc1310_param_set;
            rtk_amp[amp_num].mute_set = alc1310_mute_set;
            rtk_amp[amp_num].dump_all = alc1310_dump_all;

            snprintf(rtk_amp[amp_num].name , 32 , "ALC1310");
            break;

        case ESMT_AD82010:
            rtk_amp[amp_num].ops = ad82010_func;
            rtk_amp[amp_num].addr_size = AD82010_ADDR_SIZE;
            rtk_amp[amp_num].data_size = AD82010_DATA_SIZE;
            rtk_amp[amp_num].param_get = ad82010_param_get;
            rtk_amp[amp_num].param_set = ad82010_param_set;
            rtk_amp[amp_num].mute_set = ad82010_mute_set;
            rtk_amp[amp_num].dump_all = ad82010_dump_all;

            snprintf(rtk_amp[amp_num].name , 32 , "AD82010");
            break;

        case ESMT_AD82120:
        case ESMT_AD82120_M:
            if(amp_boot_sel[amp_num]==ESMT_AD82120_M){
                AMP_INFO("ad82120_mono \n");
                rtk_amp[amp_num].ops = ad82120m_func;
            }
            else{
                rtk_amp[amp_num].ops = ad82120_func;
            }
            rtk_amp[amp_num].addr_size = AD82120_ADDR_SIZE;
            rtk_amp[amp_num].data_size = AD82120_DATA_SIZE;
            rtk_amp[amp_num].param_get = ad82120_param_get;
            rtk_amp[amp_num].param_set = ad82120_param_set;
            rtk_amp[amp_num].mute_set = ad82120_mute_set;
            rtk_amp[amp_num].dump_all = ad82120_dump_all;

            snprintf(rtk_amp[amp_num].name , 32 , "AD82120");
            break;

        case ESMT_AD82120B:
        case ESMT_AD82120B_M:
            if(amp_boot_sel[amp_num]==ESMT_AD82120_M){
                AMP_INFO("ad82120b_mono \n");
                rtk_amp[amp_num].ops = ad82120bm_func;
            }
            else{
                rtk_amp[amp_num].ops = ad82120b_func;
            }
            rtk_amp[amp_num].addr_size = AD82120B_ADDR_SIZE;
            rtk_amp[amp_num].data_size = AD82120B_DATA_SIZE;
            rtk_amp[amp_num].param_get = ad82120b_param_get;
            rtk_amp[amp_num].param_set = ad82120b_param_set;
            rtk_amp[amp_num].mute_set = ad82120b_mute_set;
            rtk_amp[amp_num].dump_all = ad82120b_dump_all;
            snprintf(rtk_amp[amp_num].name , 32 , "AD82120B");
            break;

        case ESMT_AD82088:
            rtk_amp[amp_num].ops = ad82088_func;
            rtk_amp[amp_num].addr_size = AD82088_ADDR_SIZE;
            rtk_amp[amp_num].data_size = AD82088_DATA_SIZE;
            rtk_amp[amp_num].param_get = ad82088_param_get;
            rtk_amp[amp_num].param_set = ad82088_param_set;
            rtk_amp[amp_num].mute_set = ad82088_mute_set;
            rtk_amp[amp_num].dump_all = ad82088_dump_all;
            rtk_amp[amp_num].amp_reset = ad82088_amp_reset;
            rtk_amp[amp_num].amp_device_suspend = ad82088_amp_mute_set;
            snprintf(rtk_amp[amp_num].name , 32 , "AD82088");
            break;

        case ESMT_AD82088_D:
            rtk_amp[amp_num].ops = ad82088d_func;
            rtk_amp[amp_num].addr_size = AD82088D_ADDR_SIZE;
            rtk_amp[amp_num].data_size = AD82088D_DATA_SIZE;
            rtk_amp[amp_num].param_get = ad82088d_param_get;
            rtk_amp[amp_num].param_set = ad82088d_param_set;
            rtk_amp[amp_num].mute_set = ad82088d_mute_set;
            rtk_amp[amp_num].amp_reset = ad82088d_amp_reset;
            rtk_amp[amp_num].amp_device_suspend = ad82088d_amp_mute_set;
            snprintf(rtk_amp[amp_num].name , 32 , "AD82088D");
            break;

        case ESMT_AD82050:
            rtk_amp[amp_num].ops = ad82050_func;
            rtk_amp[amp_num].addr_size = AD82050_ADDR_SIZE;
            rtk_amp[amp_num].data_size = AD82050_DATA_SIZE;
            rtk_amp[amp_num].param_get = ad82050_param_get;
            rtk_amp[amp_num].param_set = ad82050_param_set;
            rtk_amp[amp_num].mute_set = ad82050_mute_set;
            rtk_amp[amp_num].amp_reset = ad82050_amp_reset;
            snprintf(rtk_amp[amp_num].name , 32 , "AD82050");
            break;

        case TI_TAS5805:
            rtk_amp[amp_num].ops = tas5805m_func;
            rtk_amp[amp_num].addr_size = TAS5805M_ADDR_SIZE;
            rtk_amp[amp_num].data_size = TAS5805M_DATA_SIZE;
            rtk_amp[amp_num].param_get = tas5805m_param_get;
            rtk_amp[amp_num].param_set = tas5805m_param_set;
            rtk_amp[amp_num].mute_set = tas5805m_mute_set;
            rtk_amp[amp_num].amp_reset = tas5805m_amp_reset;
            rtk_amp[amp_num].dump_all = tas5805m_dump_all;

            snprintf(rtk_amp[amp_num].name , 32 , "TAS5805");
            break;

        case TI_TAS5751:
            rtk_amp[amp_num].ops = tas5751_func;
            rtk_amp[amp_num].addr_size = TAS5751_ADDR_SIZE;
            rtk_amp[amp_num].data_size = TAS5751_DATA_SIZE;
            rtk_amp[amp_num].param_get = tas5751_param_get;
            rtk_amp[amp_num].param_set = tas5751_param_set;
            rtk_amp[amp_num].mute_set = tas5751_mute_set;
            rtk_amp[amp_num].dump_all = tas5751_dump_all;

            snprintf(rtk_amp[amp_num].name , 32 , "TAS5751");
            break;

        case TI_TAS5707:
            rtk_amp[amp_num].ops = tas5707_func;
            rtk_amp[amp_num].addr_size = TAS5707_ADDR_SIZE;
            rtk_amp[amp_num].data_size = TAS5707_DATA_SIZE;
            rtk_amp[amp_num].param_get = tas5707_param_get;
            rtk_amp[amp_num].param_set = tas5707_param_set;
            rtk_amp[amp_num].mute_set = tas5707_mute_set;
            rtk_amp[amp_num].dump_all = tas5707_dump_all;
            rtk_amp[amp_num].amp_reset = tas5707_amp_reset;

            snprintf(rtk_amp[amp_num].name , 32 , "TAS5707");
            break;

        case TI_TAS5711:
            rtk_amp[amp_num].ops = tas5711_func;
            snprintf(rtk_amp[amp_num].name , 32 , "TAS5711");
            /*rtk_amp.param_get = tas5707_param_get;
            rtk_amp.param_set = tas5707_param_set;
            rtk_amp.mute_set = tas5707_mute_set;
            rtk_amp.dump_all = tas5707_dump_all;*/
            break;

       case RTK_ALC1312:
            rtk_amp[amp_num].ops = alc1312_func;
            rtk_amp[amp_num].addr_size = ALC1312_ADDR_SIZE;
            rtk_amp[amp_num].data_size = ALC1312_DATA_SIZE;
            rtk_amp[amp_num].param_get = alc1312_param_get;
            rtk_amp[amp_num].param_set = alc1312_param_set;
            rtk_amp[amp_num].mute_set = alc1312_mute_set;
            rtk_amp[amp_num].dump_all = alc1312_dump_all;

            break;

       case WA_6819:
            rtk_amp[amp_num].ops = wa6819_func;
            snprintf(rtk_amp[amp_num].name , 32 , "WA6819");
            break;

       case WA_156819:
       case WA156819_LP:
            if(amp_boot_sel[amp_num]==WA156819_LP){
                AMP_INFO("WA6819_LP \n");
                rtk_amp[amp_num].ops = WA156819LP_func;
            }else{
                rtk_amp[amp_num].ops = WA156819_func;
            }
            rtk_amp[amp_num].addr_size = WA156819_ADDR_SIZE;
            rtk_amp[amp_num].data_size = WA156819_DATA_SIZE;
            rtk_amp[amp_num].param_get = WA156819_param_get;
            rtk_amp[amp_num].param_set = WA156819_param_set;
            rtk_amp[amp_num].mute_set = WA156819_mute_set;
            rtk_amp[amp_num].amp_reset = WA156819_amp_reset;
            snprintf(rtk_amp[amp_num].name , 32 , "WA156819");
            break;

       case FS_2105:
       case FS_2105_P:
            if(amp_boot_sel[amp_num]==FS_2105_P){
                AMP_INFO("FS2105_PBTL \n");
                rtk_amp[amp_num].ops = FS2105P_func;
            }
            else{
                rtk_amp[amp_num].ops = FS2105_func;
            }
            rtk_amp[amp_num].addr_size = FS2105_ADDR_SIZE;
            rtk_amp[amp_num].data_size = FS2105_ADDR_SIZE;
            rtk_amp[amp_num].param_get = FS2105_param_get;
            rtk_amp[amp_num].param_set = FS2105_param_set;
            rtk_amp[amp_num].mute_set = FS2105_mute_set;
            rtk_amp[amp_num].amp_reset = FS2105_amp_reset;
            snprintf(rtk_amp[amp_num].name , 32 , "FS2105");
            break;

        case NF_NTP8918:
            AMP_ERR("[%s , %d] set to NF_NTP8918 \n",__func__, __LINE__);
            rtk_amp[amp_num].ops = ntp8918_func;
            rtk_amp[amp_num].addr_size = NTP8918_ADDR_SIZE;
            rtk_amp[amp_num].data_size = NTP8918_DATA_SIZE;
            rtk_amp[amp_num].param_get = ntp8918_param_get;
            rtk_amp[amp_num].param_set = ntp8918_param_set;
            rtk_amp[amp_num].mute_set = ntp8918_mute_set;
            rtk_amp[amp_num].dump_all = ntp8918_dump_all;

            snprintf(rtk_amp[amp_num].name , 32 , "NTP8918");
            break;

        default:
            AMP_ERR("%s fail , set to ALC1310 , but maybe get no sound due to AMP power on setting incorrect in bootcode \n",__func__ );
            rtk_amp[amp_num].ops = alc1310_func;
            rtk_amp[amp_num].param_get = alc1310_param_get;
            rtk_amp[amp_num].param_set = alc1310_param_set;
            rtk_amp[amp_num].mute_set = alc1310_mute_set;
            rtk_amp[amp_num].dump_all = alc1310_dump_all;

            snprintf(rtk_amp[amp_num].name , 32 , "ALC1310");
    }

    AMP_INFO("%s amp%d name = %s \n",__func__, amp_num+1, rtk_amp[amp_num].name );

}

int rtk_amp_i2c_id(void)
{
    return rtk_amp[0].amp_i2c_id;
}

int amp_get_i2c_id(void)
{
    unsigned long long param;

    if (pcb_mgr_get_enum_info_byname("AMP_I2C", &param) != 0)
    {
        AMP_ERR("%s line:%d bootcode pcb info lose to define  \"AMP_I2C\"  \n",__func__, __LINE__ );
        return -1;
    }

    AMP_WARN("%s line:%d get amp I2c id = %lld  \n",__func__, __LINE__  , param );

    return param;
}

int amp_get_addr(char *addr_name)
{
    unsigned long long param;

    if (pcb_mgr_get_enum_info_byname(addr_name, &param) != 0)
    {
        AMP_ERR("%s line:%d bootcode pcb info lose to define  \"%s\"  \n",__func__, __LINE__, addr_name);
        return -1;
    }

    AMP_WARN("%s line:%d get amp slave address(8bit) = %llx  \n",__func__, __LINE__  , param );

    return (param >> 1);
}



int parser_amp_cmd(const char *buf, int len,unsigned char *data)
{
    char* const delim = " ";
    char *token;
    int i = 0;
    while (NULL != (token = strsep((char **)&buf, delim))) {
        if(i >= 1)
        {
             data[i-1] = (unsigned char)simple_strtol( token,NULL,16);
        }
        i++;
    }
    if(i == len)
        return 0;
    else
        return (-1);
}

int rtk_amp_pin_get(unsigned char *pcbname)
{
    RTK_GPIO_PCBINFO_T pinInfo;
    int ret = 0;
    unsigned char val = 0;

    ret = rtk_gpio_get_pcb_info(pcbname,&pinInfo);
    if(ret == 0)
    {
        val = rtk_gpio_output_get(pinInfo.gid);
        if(pinInfo.invert) {
            val = (val) ? 0 : 1;
        }
        return val;
    }

    AMP_WARN("AMP %s not define in pcb_name\n",pcbname);
    return -1;
}

int rtk_amp_pin_set(unsigned char *pcbname,unsigned char val)
{
    RTK_GPIO_PCBINFO_T pinInfo;
    int ret = 0;

    ret = rtk_gpio_get_pcb_info(pcbname,&pinInfo);
    if(ret == 0)
    {
        if(pinInfo.invert) {
            val = (val) ? 0 : 1;
        }
        return rtk_gpio_output(pinInfo.gid,val);
    }

    AMP_WARN("AMP %s not define in pcb_name\n",pcbname);
    return -1;
}

void rtk_amp_power_on_sequence(void)
{
    RTK_GPIO_PCBINFO_T pinInfo;
    memset(&pinInfo,0,sizeof(pinInfo));

    //use rtk_gpio_get_pcb_info to check is valid pin or not
    if(rtk_gpio_get_pcb_info("PIN_AMP_RESET",&pinInfo) == 0){
        if(rtk_amp_pin_get("PIN_AMP_RESET") == 0){
            rtk_amp_pin_set("PIN_AMP_RESET", 1);
            mdelay(20);
        }
    }else{
        //AMP_ERR("%s not find PIN_AMP_RESET=%x\n", __func__, pinInfo.gid);
    }
    if(rtk_gpio_get_pcb_info("PIN_AMP_MUTE",&pinInfo) == 0){
        if(rtk_amp_pin_get("PIN_AMP_MUTE") == 0){
            rtk_amp_pin_set("PIN_AMP_MUTE", 1);
            //AMP_ERR("%s set PIN_AMP_MUTE=1\n", __func__);
            mdelay(30);//need delay 30ms before I2C cmd
        }
    }else{
        AMP_ERR("%s not find PIN_AMP_MUTE=%x\n", __func__, pinInfo.gid);
    }

}

void rtk_amp_power_off_sequence(void)
{
    RTK_GPIO_PCBINFO_T pinInfo;
    int last_mute_state = 1;

    memset(&pinInfo,0,sizeof(pinInfo));

    //use rtk_gpio_get_pcb_info to check is valid pin or not
    if(rtk_gpio_get_pcb_info("PIN_AMP_MUTE",&pinInfo) == 0){
        last_mute_state = rtk_amp_pin_get("PIN_AMP_MUTE");
        rtk_amp_pin_set("PIN_AMP_MUTE", 0);
    }

    if(rtk_gpio_get_pcb_info("PIN_AMP_RESET",&pinInfo) == 0){
        if(last_mute_state && (rtk_amp_pin_get("PIN_AMP_RESET") == 1)){
            //reset pin need delay at least 35ms after mute pin pull down 
            mdelay(35);
        }
        rtk_amp_pin_set("PIN_AMP_RESET", 0);
    }
}

#if defined(CONFIG_RTK_KDRV_AMP) || defined(BUILD_QUICK_SHOW)
static int __init amp_ops_init(char *options)
{

    if(options == NULL){
        AMP_INFO("boot amp select is NULL\n");
        amp_boot_sel[0] = AMP_SEL_NULL;
        amp_boot_addr[0] = AMP_ADDR_NULL;
    }
    else{
        AMP_INFO("boot select amp:: %s\n", options);
        if (sscanf(options, "%d-%x", &amp_boot_sel[0], &amp_boot_addr[0]) < 1){
            return 1;
        }

    }

    AMP_INFO("boot select amp sel_index = %d, addr = %x\n",amp_boot_sel[0], amp_boot_addr[0]);
    return 1;
}

early_param("amp", amp_ops_init);

static int __init amp2_ops_init(char *options)
{

    if(options == NULL){
        AMP_INFO("\nboot amp2 select is NULL\n");
        amp_boot_sel[1] = AMP_SEL_NULL;
        amp_boot_addr[1] = AMP_ADDR_NULL;
    }
    else{
        AMP_INFO("\nboot select amp2: %s\n", options);
        if (sscanf(options, "%d-%x", &amp_boot_sel[1], &amp_boot_addr[1]) < 1){
            return 1;
        }
    }

    AMP_INFO("boot select amp2 sel_index = %d, addr = %x\n",amp_boot_sel[1], amp_boot_addr[1]);
    return 1;
}
early_param("amp2", amp2_ops_init);

static int __init amp3_ops_init(char *options)
{
    if(options == NULL){
        AMP_ERR("\nboot amp3 select is NULL\n");
        amp_boot_sel[2] = AMP_SEL_NULL;
        amp_boot_addr[2] = AMP_ADDR_NULL;
    }
    else{
        AMP_ERR("\nboot select amp3: %s\n", options);
        if (sscanf(options, "%d-%x", &amp_boot_sel[2], &amp_boot_addr[2]) < 1){
            return 1;
        }
    }
    AMP_ERR("boot select amp3 sel_index = %d, addr = %x\n",amp_boot_sel[2], amp_boot_addr[2]);
    return 1;
}
early_param("amp3", amp3_ops_init);

static int __init amp4_ops_init(char *options)
{
    if(options == NULL){
        AMP_ERR("\nboot amp4 select is NULL\n");
        amp_boot_sel[3] = AMP_SEL_NULL;
        amp_boot_addr[3] = AMP_ADDR_NULL;
    }
    else{
        AMP_ERR("\nboot select amp4: %s\n", options);
        if (sscanf(options, "%d-%x", &amp_boot_sel[3], &amp_boot_addr[3]) < 1){
            return 1;
        }
    }
    AMP_ERR("boot select amp4 sel_index = %d, addr = %x\n",amp_boot_sel[3], amp_boot_addr[3]);
    return 1;
}
early_param("amp4", amp4_ops_init);

static int __init amp_table_phy_address_init(char *options)
{

    if(options != NULL){
        if (sscanf(options, "%x", &amp_table_phy_address) < 1){
            return 1;
        }
    }
    AMP_INFO("amp_table_phy_address= %x\n",amp_table_phy_address);
    return 1;
}
early_param("amp_table_phy_address", amp_table_phy_address_init);

static int __init power_off_mode_init(char *value)
{
    AMP_ERR("power off mode.\n");
    power_off_mode = 1;
    return 0;
}
early_param("POWERDOWN", power_off_mode_init);

#else
extern bool rtk_parse_commandline_equal(const char *string, char *output_string, int string_size);
static int  amp_ops_init(void)
{
    char str[20] = {0};

    if (0 == rtk_parse_commandline_equal("amp", str, sizeof(str)))
    {
        AMP_ERR("[%s] line: %d, can't get amp from bootargs\n", __func__, __LINE__);
        amp_boot_sel[0] = AMP_SEL_NULL;
        amp_boot_addr[0] = AMP_ADDR_NULL;
        return -1;
    }

    AMP_ERR("boot select amp:: %s\n", str);
    if (sscanf(str, "%d-%x", &amp_boot_sel[0], &amp_boot_addr[0]) < 1){
        return 1;
    }

    AMP_ERR("boot select amp sel_index = %d, addr = %x\n",amp_boot_sel[0], amp_boot_addr[0]);
    return 1;
}

static int  amp2_ops_init(void)
{
    char str[20] = {0};

    if (0 == rtk_parse_commandline_equal("amp2", str, sizeof(str)))
    {
        AMP_ERR("[%s] line: %d, can't get amp from bootargs\n", __func__, __LINE__);
        amp_boot_sel[1] = AMP_SEL_NULL;
        amp_boot_addr[1] = AMP_ADDR_NULL;
        return -1;
    }

    AMP_ERR("boot select amp2:: %s\n", str);
    if (sscanf(str, "%d-%x", &amp_boot_sel[1], &amp_boot_addr[1]) < 1){
        return 1;
    }

    AMP_ERR("boot select amp2 sel_index = %d, addr = %x\n",amp_boot_sel[1], amp_boot_addr[1]);
    return 1;
}

static int  amp3_ops_init(void)
{
    char str[20] = {0};

    if (0 == rtk_parse_commandline_equal("amp3", str, sizeof(str)))
    {
        AMP_ERR("[%s] line: %d, can't get amp from bootargs\n", __func__, __LINE__);
        amp_boot_sel[2] = AMP_SEL_NULL;
        amp_boot_addr[2] = AMP_ADDR_NULL;
        return -1;
    }

    AMP_ERR("boot select amp3:: %s\n", str);
    if (sscanf(str, "%d-%x", &amp_boot_sel[2], &amp_boot_addr[2]) < 1){
        return 1;
    }

    AMP_ERR("boot select amp3 sel_index = %d, addr = %x\n",amp_boot_sel[2], amp_boot_addr[2]);
    return 1;
}

static int  amp4_ops_init(void)
{
    char str[20] = {0};

    if (0 == rtk_parse_commandline_equal("amp4", str, sizeof(str)))
    {
        AMP_ERR("[%s] line: %d, can't get amp from bootargs\n", __func__, __LINE__);
        amp_boot_sel[3] = AMP_SEL_NULL;
        amp_boot_addr[3] = AMP_ADDR_NULL;
        return -1;
    }

    AMP_ERR("boot select amp4:: %s\n", str);
    if (sscanf(str, "%d-%x", &amp_boot_sel[3], &amp_boot_addr[3]) < 1){
        return 1;
    }

    AMP_ERR("boot select amp4 sel_index = %d, addr = %x\n",amp_boot_sel[3], amp_boot_addr[3]);
    return 1;
}

#endif

#ifndef BUILD_QUICK_SHOW
void do_ampOps(struct work_struct *work)
{
    int amp_num=0;
#ifdef ENABLE_NEW_AMP_INI_FLOW
    if(is_new_amp_ini_flow()){
	    if(is_kernel_init_amp() || str_resume)
	        do_ampIniOps();
    }else
#endif
    {
	    for (amp_num=0; amp_num<MULTI_AMP_CNT; amp_num++)
	    {
	        if (rtk_amp[amp_num].slave_addr != AMP_ADDR_NULL && rtk_amp[amp_num].ops != NULL)
	        {
	            AMP_ERR("[%s] id = %d , addr = 0x%X \n",
	                __func__ , rtk_amp[amp_num].amp_i2c_id,rtk_amp[amp_num].slave_addr );
	            rtk_amp[amp_num].ops(rtk_amp[amp_num].amp_i2c_id, rtk_amp[amp_num].slave_addr);
	        }
	    }
    }
    str_init_amp = 1;
}

void _init_amp_delay_wq(work_func_t func)
{
    AMP_INFO("[%s] \n", __func__ );

    if (NULL == ampWq)
    {
        ampWq = create_workqueue("Amp workqueue");
        if (NULL == ampWq)
        {
            AMP_ERR("[%s] ERROR: Create Amp workqueue failed!\n", __func__ );
        }
        else
        {
            AMP_ERR("[%s] Create Amp workqueue successful!\n", __func__ );
            INIT_DELAYED_WORK(&amp_delayWork, func);
        }
    }

}

int _start_amp_delay_wq(unsigned int delayms)
{
    int ret = -1;

    AMP_ERR("[%s] \n", __func__ );

    if (ampWq)
    {
        ret = queue_delayed_work(ampWq, &amp_delayWork, msecs_to_jiffies(delayms));
    }

    return ret;
}

void _cancel_amp_delay_wq(void)
{
    AMP_ERR("[%s] \n", __func__ );
    if (ampWq)
    {
        cancel_delayed_work(&amp_delayWork);
        flush_workqueue(ampWq);
    }
}
static void rtk_amp_shutdown(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    amp_suspend(dev);
}

static int rtk_amp_probe(struct platform_device *pdev)
{
      amp_devs= pdev;
      pm_runtime_forbid(&amp_devs->dev);
      pm_runtime_set_active(&amp_devs->dev);
      pm_runtime_enable(&amp_devs->dev);
      return 0;
}

static int amp_suspend(struct device *dev)
{
    pm_runtime_disable(&amp_devs->dev);
    str_resume = 0;
    str_init_amp = 0;
    if(rtk_amp[0].amp_device_suspend != NULL)
    {
	    rtk_amp[0].amp_device_suspend(0);
    }

    rtk_amp_power_off_sequence();

    AMP_WARN("[AMP] amp_notify_suspend\n");
    return 0;
}

static int amp_resume(struct device *dev)
{
// Please notice that this is only for REALTEK_AMP_TAS5711 & REALTEK_AMP_ACL1310
// If use different IC, please change this.
    str_resume = 1;
#ifdef CONFIG_RTK_KDRV_AMP_INIT_SKIP
#else

    AMP_WARN("[AMP] amp_notify_resume\n");

    rtk_amp_power_on_sequence();

    _start_amp_delay_wq(10);

    pm_runtime_enable(&amp_devs->dev);
#endif
    return 0;
}

int amp_pm_runtime_suspend(struct device *dev)
{
    AMP_WARN("@(%s:%d)\n", __func__, __LINE__);
    rtk_amp_power_off_sequence();

    rtk_amp_pin_set("PIN_AMP_PWR_EN", 0);
	
    return 0;
}

int amp_pm_runtime_resume(struct device *dev)
{
    int amp_num = 0;
    AMP_WARN("@(%s:%d)\n", __func__, __LINE__);

    rtk_amp_pin_set("PIN_AMP_PWR_EN", 1);
    mdelay(10);
    if(rtk_amp[amp_num].amp_reset != NULL)
    {
        rtk_amp[amp_num].amp_reset();
    }
    rtk_amp_power_on_sequence();
	
#ifdef ENABLE_NEW_AMP_INI_FLOW
    if(is_new_amp_ini_flow()){
        do_ampIniOps();
    }else
#endif
    {
        for (amp_num=0; amp_num<MULTI_AMP_CNT; amp_num++){
            if(amp_boot_sel[amp_num] != AMP_SEL_NULL)
                rtk_amp[amp_num].ops(rtk_amp[amp_num].amp_i2c_id,rtk_amp[amp_num].slave_addr);
        }
    }
    return 0;
}

static char *amp_devnode(struct device *dev, umode_t *mode)
{
    return NULL;
}


struct file_operations amp_fops = {
        .owner                  = THIS_MODULE,
};

ssize_t rtk_amp_show_param(struct device *dev,
                            struct device_attribute *attr, char *buf)
{
    ssize_t ret = 0;
    ssize_t count_delay = 0;
    char *buf_info = NULL;

    buf_info = kmalloc(PAGE_SIZE, GFP_KERNEL);
    if(buf_info == NULL)
        return ret;
    buf_info[0] = 0;

    if (strcmp(attr->attr.name, "amp_dump") == 0) {
        rtk_amp[0].dump_all(buf_info,rtk_amp[0].amp_i2c_id,rtk_amp[0].slave_addr);
        ret = sprintf(buf, "%s\n", buf_info);
    }
    else if ((rtk_amp[1].slave_addr != -1) && (strcmp(attr->attr.name, "amp_dump2") == 0)) {
        rtk_amp[1].dump_all(buf_info,rtk_amp[1].amp_i2c_id,rtk_amp[1].slave_addr);
        ret = sprintf(buf, "%s\n", buf_info);
    }else if ((rtk_amp[2].slave_addr != -1) && (strcmp(attr->attr.name, "amp_dump3") == 0)) {
        rtk_amp[2].dump_all(buf_info,rtk_amp[2].amp_i2c_id,rtk_amp[2].slave_addr);
        ret = sprintf(buf, "%s\n", buf_info);
    }else if ((rtk_amp[3].slave_addr != -1) && (strcmp(attr->attr.name, "amp_dump4") == 0)) {
        rtk_amp[3].dump_all(buf_info,rtk_amp[3].amp_i2c_id,rtk_amp[3].slave_addr);
        ret = sprintf(buf, "%s\n", buf_info);
    }else if (strcmp(attr->attr.name, "amp_slave_addr") == 0) {

        while(str_init_amp == 0){
            count_delay++;
            mdelay(10);
            //AMP_WARN("count_delay:%d\n",count_delay);
            if(count_delay>100)
                break;
        }
        if(count_delay==100){
            AMP_WARN("AP wait amp init timeout(1s) count_delay:%d\n",count_delay);
        }
        AMP_WARN("AMP Slave Address (8bit):\n");

        ret = sprintf(buf, "%x\n", (rtk_amp[0].slave_addr << 1));
    }else if (strcmp(attr->attr.name, "amp_slave_addr2") == 0) {
        AMP_WARN("AMP Slave Address2 (8bit):\n");
        ret = sprintf(buf, "%x\n", (rtk_amp[1].slave_addr << 1));
    }else if (strcmp(attr->attr.name, "amp_slave_addr3") == 0) {
        AMP_WARN("AMP Slave Address3 (8bit):\n");
        ret = sprintf(buf, "%x\n", (rtk_amp[2].slave_addr << 1));
    }else if (strcmp(attr->attr.name, "amp_slave_addr4") == 0) {
        AMP_WARN("AMP Slave Address4 (8bit):\n");
        ret = sprintf(buf, "%x\n", (rtk_amp[3].slave_addr << 1));
    }else if (strcmp(attr->attr.name, "amp_pin") == 0) {
        ret = sprintf(buf, "PIN_AMP_RESET Value:%d \nPIN_AMP_MUTE Value:%d \n", rtk_amp_pin_get("PIN_AMP_RESET"),rtk_amp_pin_get("PIN_AMP_MUTE"));
    }else if (strcmp(attr->attr.name, "amp_eq_mode") == 0) {
#ifdef ENABLE_NEW_AMP_INI_FLOW
        amp_basic_info_t basic_info;
        char ed_mode[4][16]  = {"none","desk","wall","bypass"};
        memset(&basic_info,0,sizeof(amp_basic_info_t));
        ret = get_amp_basic_info(&basic_info);
        ret = sprintf(buf, "%s\n", ed_mode[basic_info.eq_mode]);
#endif
    }else if (strcmp(attr->attr.name, "amp_info") == 0) {
#ifdef ENABLE_NEW_AMP_INI_FLOW
        amp_basic_info_t basic_info;

        char ed_mode_str[4][16]  = {"none","desk","wall","bypass"};
        char init_stage_str[4][16]  = {"none","boot","kernel","ap"};
        memset(&basic_info,0,sizeof(amp_basic_info_t));
        ret = get_amp_basic_info(&basic_info);
        if(basic_info.magic_num != 0x475a5a47 || basic_info.support_amp_num == 0){
            AMP_WARN("magicNum error! magicNum=0x%x(should be 0x475a5a47),not new amp.ini flow\n",basic_info.magic_num);
            //ret = sprintf(buf, "magicNum error! magicNum=0x%x(should be 0x475a5a47),not new amp.ini flow\n");
        }else{
            unsigned char i = 0;
            char *pBuf =  buf_info;
            memset(buf_info,0,PAGE_SIZE);
            pBuf += sprintf(pBuf, "new amp.ini flow basic info:\n");
            pBuf += sprintf(pBuf, "    support_amp_num:%d\n", basic_info.support_amp_num);
            pBuf += sprintf(pBuf, "    init_stage:%s\n", init_stage_str[basic_info.init_stage]);
            pBuf += sprintf(pBuf, "    eq_mode:%s\n", ed_mode_str[basic_info.eq_mode]);
            pBuf += sprintf(pBuf, "    use_pcb_amp:%d (1: bootcode select 0 or FlowV2_default_support1 defined in model.ini)\n\n", basic_info.default_pcb_amp);
            pBuf += sprintf(pBuf, "    amp_ini_table_addr:0x%x\n", basic_info.amp_ini_table_addr);
            pBuf += sprintf(pBuf, "    amp_ini_table_size:%d\n", basic_info.amp_ini_table_size);
            pBuf += sprintf(pBuf, "    amp_ini_real_size:%d\n", basic_info.amp_ini_real_size);

            pBuf += sprintf(pBuf, "amp.ini info:\n");
            for(i=0;i<basic_info.support_amp_num;i++){
                amp_param_layout_t amp_layout_info;
                unsigned char eq_cmd_len = 0; 
                memset(&amp_layout_info,0,sizeof(amp_param_layout_t));
                ret = get_amp_model_info(i,&amp_layout_info);
                if(basic_info.eq_mode==1)
                    eq_cmd_len = amp_layout_info.eq_desk_cmd_len;
                else if(basic_info.eq_mode==2)
                    eq_cmd_len = amp_layout_info.eq_wall_cmd_len;
                else if(basic_info.eq_mode==3)
                    eq_cmd_len = amp_layout_info.eq_bypass_cmd_len;
                pBuf += sprintf(pBuf, "    AMP%d: %s_0x%02x, qs_cmd_len:%d,init_cmd_len:%d,eq_cmd_len=%d\n", i,amp_layout_info.ampModelName,amp_layout_info.amp_addr,amp_layout_info.init_qs_cmd_len,amp_layout_info.init_cmd_len,eq_cmd_len);
            }
            //AMP_WARN(" print buf_info=%s\n",buf_info);
            ret = sprintf(buf, "%s\n", buf_info);
        }
        //ret = sprintf(buf, "%s\n", ed_mode[basic_info.eq_mode]);
#endif
    }
    kfree(buf_info);
    return ret;
}

ssize_t rtk_amp_store_param(struct device *dev,
                           struct device_attribute *attr,
                           const char *buf, size_t count)
{
    int ret = 0;
    int on_off = 0;
    char cmd1[16] = {0};
    unsigned char data[16] = {0};

    if (strcmp(attr->attr.name, "amp_mute") == 0) {
        sscanf(buf, "%d\n",&on_off);
        ret = rtk_amp[0].mute_set(on_off,rtk_amp[0].amp_i2c_id,rtk_amp[0].slave_addr);
    }
    else if (strcmp(attr->attr.name, "amp_param") == 0) {
        sscanf(buf, "%4s\n", &cmd1[0]);
        if (strcmp(cmd1, "get") == 0) {
            ret = parser_amp_cmd(buf,rtk_amp[0].addr_size,data);
            ret = rtk_amp[0].param_get(data,rtk_amp[0].amp_i2c_id,rtk_amp[0].slave_addr);
        }
        else if(strcmp(cmd1, "set") == 0) {
           ret = parser_amp_cmd(buf ,rtk_amp[0].addr_size+rtk_amp[0].data_size, data);
           ret = rtk_amp[0].param_set(data,rtk_amp[0].amp_i2c_id,rtk_amp[0].slave_addr);
        }
        else if(strcmp(cmd1, "init") == 0) {
           rtk_amp[0].ops(rtk_amp[0].amp_i2c_id,rtk_amp[0].slave_addr);
        }
        else {
           AMP_ERR("No this amp command\n");
        }

    }else if ((rtk_amp[1].slave_addr != -1) && (strcmp(attr->attr.name, "amp_param2") == 0)) {
        sscanf(buf, "%4s\n", &cmd1[0]);
        if (strcmp(cmd1, "get") == 0) {
            ret = parser_amp_cmd(buf,rtk_amp[1].addr_size,data);
            ret = rtk_amp[1].param_get(data,rtk_amp[1].amp_i2c_id,rtk_amp[1].slave_addr);
        }
        else if(strcmp(cmd1, "set") == 0) {
           ret = parser_amp_cmd(buf ,rtk_amp[1].addr_size+rtk_amp[1].data_size, data);
           ret = rtk_amp[1].param_set(data,rtk_amp[1].amp_i2c_id,rtk_amp[1].slave_addr);
        }
        else if(strcmp(cmd1, "init") == 0) {
           rtk_amp[1].ops(rtk_amp[1].amp_i2c_id,rtk_amp[1].slave_addr);
        }
        else {
           AMP_ERR("No this amp command\n");
        }

    }else if ((rtk_amp[2].slave_addr != -1) && (strcmp(attr->attr.name, "amp_param3") == 0)) {
        sscanf(buf, "%4s\n", &cmd1[0]);
        if (strcmp(cmd1, "get") == 0) {
            ret = parser_amp_cmd(buf,rtk_amp[2].addr_size,data);
            ret = rtk_amp[2].param_get(data,rtk_amp[2].amp_i2c_id,rtk_amp[2].slave_addr);
        }
        else if(strcmp(cmd1, "set") == 0) {
           ret = parser_amp_cmd(buf ,rtk_amp[2].addr_size+rtk_amp[2].data_size, data);
           ret = rtk_amp[2].param_set(data,rtk_amp[2].amp_i2c_id,rtk_amp[2].slave_addr);
        }
        else if(strcmp(cmd1, "init") == 0) {
           rtk_amp[2].ops(rtk_amp[2].amp_i2c_id,rtk_amp[2].slave_addr);
        }
        else {
           AMP_ERR("No this amp command\n");
        }

    }else if ((rtk_amp[3].slave_addr != -1) && (strcmp(attr->attr.name, "amp_param4") == 0)) {
        sscanf(buf, "%4s\n", &cmd1[0]);
        if (strcmp(cmd1, "get") == 0) {
            ret = parser_amp_cmd(buf,rtk_amp[3].addr_size,data);
            ret = rtk_amp[3].param_get(data,rtk_amp[3].amp_i2c_id,rtk_amp[3].slave_addr);
        }
        else if(strcmp(cmd1, "set") == 0) {
            ret = parser_amp_cmd(buf ,rtk_amp[3].addr_size+rtk_amp[3].data_size, data);
            ret = rtk_amp[3].param_set(data,rtk_amp[3].amp_i2c_id,rtk_amp[3].slave_addr);
        }
        else if(strcmp(cmd1, "init") == 0) {
            rtk_amp[3].ops(rtk_amp[3].amp_i2c_id,rtk_amp[3].slave_addr);
        }
        else {
        AMP_ERR("No this amp command\n");
        }

    }else if (strcmp(attr->attr.name, "amp_pin") == 0) {
        sscanf(buf, "%4s %d\n", &cmd1[0],&on_off);
        if (strcmp(cmd1, "mute") == 0) {
            ret = rtk_amp_pin_set("PIN_AMP_MUTE",on_off);
        }else if (strcmp(cmd1, "reset") == 0) {
            ret = rtk_amp_pin_set("PIN_AMP_RESET",on_off);
        }
    }else if (strcmp(attr->attr.name, "amp_init") == 0) {
        sscanf(buf, "%4s\n", &cmd1[0]);
        AMP_ERR("amp_init cmd:%s\n",cmd1);
#ifdef ENABLE_NEW_AMP_INI_FLOW
        if(is_new_amp_ini_flow()){
            do_ampIniOps();
        }
#endif
    }else if (strcmp(attr->attr.name, "amp_info") == 0) {

    }else if (strcmp(attr->attr.name, "amp_eq_mode") == 0) {
        sscanf(buf, "%4s\n", &cmd1[0]);
        AMP_ERR("amp_eq_mode cmd:%s\n",cmd1);
#ifdef ENABLE_NEW_AMP_INI_FLOW
        set_amp_eq_mode(cmd1);
#endif
    }
    return count;
}

DEVICE_ATTR(amp_slave_addr, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_mute, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_dump, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_param, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_init, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_pin, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);

DEVICE_ATTR(amp_slave_addr2, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_dump2, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_param2, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);

DEVICE_ATTR(amp_slave_addr3, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_dump3, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_param3, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);

DEVICE_ATTR(amp_slave_addr4, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_dump4, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_param4, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_eq_mode, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);
DEVICE_ATTR(amp_info, S_IWUSR | S_IWGRP | S_IRUGO, rtk_amp_show_param,
                    rtk_amp_store_param);

static struct class *amp_class = NULL;
static struct cdev amp_cdev;
static struct device *amp_device_this;

static int rtk_amp_init(void) {
        int result = 0;
        int amp_num = 0;
        dev_t dev = 0;    //MKDEV(AMP_MAJOR, 0);
		
        AMP_ERR("amp: AMP init for Realtek AMP(2021/02/01)\n");
		
        if(power_off_mode){
            AMP_ERR("power_off_mode no need init amp\n");
            return -1;
        }
#ifdef CONFIG_RTK_KDRV_AMP_MODULE
        amp_ops_init();
        amp2_ops_init();
        amp3_ops_init();
        amp4_ops_init();
#endif
		
        rtk_amp_power_on_sequence();

        //result = register_chrdev_region(dev, 1, "RTKamp");
        result = alloc_chrdev_region(&dev, 0, 1, "RTKamp");
        if (result < 0) {
            AMP_ERR("amp: can not get chrdev region...\n");
            return result;
        }

        amp_class = class_create(THIS_MODULE, "RTKamp");
        if (IS_ERR(amp_class)) {
            AMP_ERR("amp: can not create class...\n");
            result = PTR_ERR(amp_class);
            goto fail_class_create;
        }

        amp_class->devnode = amp_devnode;

        amp_devs = platform_device_register_simple("RTKamp", -1, NULL, 0);
        if (platform_driver_register(&amp_driver) != 0) {
            AMP_ERR("amp: can not register platform driver...\n");
            result = -EINVAL;
            goto fail_platform_driver_register;
        }

        cdev_init(&amp_cdev, &amp_fops);
        amp_cdev.owner = THIS_MODULE;
        amp_cdev.ops = &amp_fops;
        result = cdev_add(&amp_cdev, dev, 1);
        if (result < 0) {
            AMP_ERR("amp: can not add character device...\n");
            goto fail_cdev_init;
        }
        amp_device_this = device_create(amp_class, NULL, dev, NULL, "RTKamp");

        device_create_file(amp_device_this,&dev_attr_amp_mute);
        device_create_file(amp_device_this,&dev_attr_amp_pin);
        device_create_file(amp_device_this,&dev_attr_amp_init);
        device_create_file(amp_device_this,&dev_attr_amp_eq_mode);
        device_create_file(amp_device_this,&dev_attr_amp_info);

        if(amp_boot_sel[0] != AMP_SEL_NULL && amp_boot_addr[0] != AMP_ADDR_NULL){
            device_create_file(amp_device_this,&dev_attr_amp_slave_addr);
            device_create_file(amp_device_this,&dev_attr_amp_dump);
            device_create_file(amp_device_this,&dev_attr_amp_param);
        }

        if(amp_boot_sel[1] != AMP_SEL_NULL && amp_boot_addr[1] != AMP_ADDR_NULL){
            device_create_file(amp_device_this,&dev_attr_amp_slave_addr2);
            device_create_file(amp_device_this,&dev_attr_amp_dump2);
            device_create_file(amp_device_this,&dev_attr_amp_param2);
        }

        if(amp_boot_sel[2] != AMP_SEL_NULL && amp_boot_addr[2] != AMP_ADDR_NULL){
            device_create_file(amp_device_this,&dev_attr_amp_slave_addr3);
            device_create_file(amp_device_this,&dev_attr_amp_dump3);
            device_create_file(amp_device_this,&dev_attr_amp_param3);
        }

        if(amp_boot_sel[3] != AMP_SEL_NULL && amp_boot_addr[3] != AMP_ADDR_NULL){
            device_create_file(amp_device_this,&dev_attr_amp_slave_addr4);
            device_create_file(amp_device_this,&dev_attr_amp_dump4);
            device_create_file(amp_device_this,&dev_attr_amp_param4);
        }

        for (amp_num=0; amp_num<MULTI_AMP_CNT; amp_num++)
        {
            rtk_amp[amp_num].amp_i2c_id = amp_get_i2c_id();
            if(rtk_amp[amp_num].amp_i2c_id == (-1))
            {
                AMP_ERR("%s line:%d amp i2c id failed \n",__func__, __LINE__);
                return -1;
            }

            if(amp_boot_addr[amp_num] == AMP_ADDR_NULL)
            {
                rtk_amp[amp_num].sel_index = 0;
                rtk_amp[amp_num].slave_addr = 0;
                if(amp_num == 0){
                    rtk_amp[amp_num].sel_index = amp_sel[amp_num];
                    rtk_amp[amp_num].slave_addr = amp_get_addr(p_amp_dev_addr[amp_num]);
                    if(rtk_amp[amp_num].slave_addr == (-1))
                    {
                        AMP_ERR("%s line:%d amp%d slave address failed \n",__func__, __LINE__, amp_num+1);
                        //return -1;
                    }
                }
            }
            else{
                rtk_amp[amp_num].sel_index = amp_boot_sel[amp_num];
                rtk_amp[amp_num].slave_addr = amp_boot_addr[amp_num];
            }

        }

#ifdef CONFIG_RTK_KDRV_AMP_INIT_SKIP
#else
        for (amp_num=0; amp_num<MULTI_AMP_CNT; amp_num++)
        {
            if(amp_boot_sel[amp_num] != AMP_SEL_NULL)
                rtk_amp_ops_set(amp_num, rtk_amp[amp_num].sel_index);
        }

        if(amp_boot_sel[0]==ESMT_AD82120&&amp_boot_sel[1]==ESMT_AD82120&&amp_boot_sel[2]==ESMT_AD82120_M){
            ad82120_amp_phase_reset(rtk_amp[0].amp_i2c_id);
        }
        _init_amp_delay_wq(do_ampOps);

        if (is_QS_amp_enable() == 0) {
            _start_amp_delay_wq(0);
        }
#endif
        //AMP_ERR("\n AMP I2C Port_Id2  =  %d Slave_addr(7bit):%x\n", rtk_amp.amp_i2c_id,  rtk_amp.slave_addr);

#ifdef CONFIG_RTK_AMP_USER_RESUME
        #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
        // Nothing
        #else
        amp_devs->dev.power.is_userresume = true;
        #endif
        device_enable_async_suspend(&(amp_devs->dev));
#endif
        return 0;

fail_cdev_init:
        platform_driver_unregister(&amp_driver);
fail_platform_driver_register:
        platform_device_unregister(amp_devs);
        amp_devs = NULL;
        class_destroy(amp_class);
fail_class_create:
        amp_class = NULL;
        unregister_chrdev_region(dev, 1);
        return result;

}


static void rtk_amp_exit(void) {
    dev_t dev = MKDEV(AMP_MAJOR, 0);

    if ((amp_devs == NULL) || (amp_class == NULL))
        BUG();

    device_destroy(amp_class, dev);
    cdev_del(&amp_cdev);

    platform_driver_unregister(&amp_driver);
    platform_device_unregister(amp_devs);
    amp_devs = NULL;

    class_destroy(amp_class);
    amp_class = NULL;


    device_remove_file(amp_device_this, &dev_attr_amp_mute);
    device_remove_file(amp_device_this, &dev_attr_amp_pin);
    device_remove_file(amp_device_this,&dev_attr_amp_init);
    device_remove_file(amp_device_this,&dev_attr_amp_eq_mode);
    device_remove_file(amp_device_this,&dev_attr_amp_info);

    if(amp_boot_sel[0] != AMP_SEL_NULL && amp_boot_addr[0] != AMP_ADDR_NULL){
        device_remove_file(amp_device_this, &dev_attr_amp_slave_addr);
        device_remove_file(amp_device_this, &dev_attr_amp_dump);
        device_remove_file(amp_device_this, &dev_attr_amp_param);
    }

    if(amp_boot_sel[1] != AMP_SEL_NULL && amp_boot_addr[1] != AMP_ADDR_NULL){
        device_remove_file(amp_device_this, &dev_attr_amp_slave_addr2);
        device_remove_file(amp_device_this, &dev_attr_amp_dump2);
        device_remove_file(amp_device_this, &dev_attr_amp_param2);
    }
	
    if(amp_boot_sel[2] != AMP_SEL_NULL && amp_boot_addr[2] != AMP_ADDR_NULL){
        device_remove_file(amp_device_this, &dev_attr_amp_slave_addr3);
        device_remove_file(amp_device_this, &dev_attr_amp_dump3);
        device_remove_file(amp_device_this, &dev_attr_amp_param3);
    }
	
    if(amp_boot_sel[3] != AMP_SEL_NULL && amp_boot_addr[3] != AMP_ADDR_NULL){
        device_remove_file(amp_device_this, &dev_attr_amp_slave_addr4);
        device_remove_file(amp_device_this, &dev_attr_amp_dump4);
        device_remove_file(amp_device_this, &dev_attr_amp_param4);
    }
    unregister_chrdev_region(dev, 1);
}

fs_initcall(rtk_amp_init);
module_exit(rtk_amp_exit);

#else
void do_ampOps(void)
{
    int amp_num=0;
#ifdef ENABLE_NEW_AMP_INI_FLOW
    if(is_new_amp_ini_flow()){
	    do_ampIniOps();
    }else
#endif
    {
	    for (amp_num=0; amp_num<MULTI_AMP_CNT; amp_num++)
	    {
	        if (rtk_amp[amp_num].slave_addr != AMP_ADDR_NULL && rtk_amp[amp_num].ops != NULL)
	        {
	            AMP_INFO("[%s] id = %d , addr = 0x%X \n",
	                __func__ , rtk_amp[amp_num].amp_i2c_id,rtk_amp[amp_num].slave_addr );
	            rtk_amp[amp_num].ops(rtk_amp[amp_num].amp_i2c_id, rtk_amp[amp_num].slave_addr);
	        }
	    }
    }
}


int rtk_amp_init(void) {
    int result = 0;
    int amp_num = 0;
    char str[20] = {0};
    int ret = 0;

#if 0
    ret = rtk_gpio_get_pcb_info("PIN_AMP_RESET",&pinInfo);
    if (ret)
        AMP_ERR("%s PIN_AMP_RESET=%x,ret=%d\n", __func__, pinInfo.gid, ret);

    if(ret == 0){
        rtk_gpio_set_dir(pinInfo.gid, 1);
        rtk_gpio_output(pinInfo.gid, 1);
        mdelay(20);
    }
#endif
    rtk_amp_power_on_sequence();

    AMP_INFO("amp: AMP init for Realtek AMP(2020/12/24)\n");

    if(getenv("amp_select") != NULL && getenv("amp_select_value") != NULL) {
        snprintf(str, 20, "%s-%s ", getenv("amp_select"),getenv("amp_select_value"));
        amp_ops_init(str);
    }

    if(getenv("amp_select2") != NULL && getenv("amp_select_value2") != NULL) {
        snprintf(str, 20, "%s-%s ", getenv("amp_select2"),getenv("amp_select_value2"));
        amp2_ops_init(str);
    }

    if(getenv("amp_select3") != NULL && getenv("amp_select_value3") != NULL) {
        snprintf(str, 20, "%s-%s ", getenv("amp_select3"),getenv("amp_select_value3"));
        amp3_ops_init(str);
    }

    if(getenv("amp_select4") != NULL && getenv("amp_select_value4") != NULL) {
        snprintf(str, 20, "%s-%s ", getenv("amp_select4"),getenv("amp_select_value4"));
        amp4_ops_init(str);
    }

	if(getenv("amp_table_phy_address") != NULL) {
        snprintf(str, 20, "%s ", getenv("amp_table_phy_address"));
        //AMP_ERR("amp: str=%s\n",str);
        if(sscanf(str, "%x", &amp_table_phy_address)  < 1){
            AMP_ERR("%s:line=%d,sscanf amp_table_phy_address fail\n", __func__,__LINE__);
        }
    }


    for (amp_num=0; amp_num<MULTI_AMP_CNT; amp_num++)
    {
        rtk_amp[amp_num].amp_i2c_id = amp_get_i2c_id();
        if(rtk_amp[amp_num].amp_i2c_id == (-1))
        {
            //AMP_ERR("%s line:%d amp i2c id failed \n",__func__, __LINE__);
            return -1;
        }

        if(amp_boot_addr[amp_num] == AMP_ADDR_NULL)
        {
            rtk_amp[amp_num].sel_index = 0;
            rtk_amp[amp_num].slave_addr = 0;
            if(amp_num == 0){
                rtk_amp[amp_num].sel_index = amp_sel[amp_num];
                rtk_amp[amp_num].slave_addr = amp_get_addr(p_amp_dev_addr[amp_num]);
                if(rtk_amp[amp_num].slave_addr == (-1))
                {
                    AMP_ERR("%s line:%d amp%d slave address failed \n",__func__, __LINE__, amp_num+1);
                    return -1;
                }
            }
        }
        else{
            rtk_amp[amp_num].sel_index = amp_boot_sel[amp_num];
            rtk_amp[amp_num].slave_addr = amp_boot_addr[amp_num];
        }

    }

#ifdef CONFIG_RTK_KDRV_AMP_INIT_SKIP
#else
    for (amp_num=0; amp_num<MULTI_AMP_CNT; amp_num++)
    {
        if(amp_boot_sel[amp_num] != AMP_SEL_NULL)
            rtk_amp_ops_set(amp_num, rtk_amp[amp_num].sel_index);
    }

    do_ampOps();
#endif

    return 0;
}


#endif


#ifdef ENABLE_NEW_AMP_INI_FLOW
unsigned char *amp_ini_table_buf = NULL;

#ifdef BUILD_QUICK_SHOW
#define amp_malloc(x) malloc(x)
#define amp_free(x) free(x)
#else
#define amp_malloc(x) kmalloc(x, GFP_KERNEL)
#define amp_free(x) kfree(x)
#endif


unsigned char *get_amp_table_buffer(void){
	if(amp_ini_table_buf == NULL){
		if(amp_table_phy_address == 0){
			AMP_INFO("%s:line=%d,amp_table_phy_address=0,use default address=0x%x\n", __func__,__LINE__,AMP_INI_SHARE_BUFF_START_ADDR);
			amp_table_phy_address = AMP_INI_SHARE_BUFF_START_ADDR;
		}
#ifdef BUILD_QUICK_SHOW
		amp_ini_table_buf = (unsigned char *)amp_table_phy_address;
#else
		amp_ini_table_buf = (unsigned char *)phys_to_virt (amp_table_phy_address);
#endif
	}
	return 	amp_ini_table_buf;
}

int is_new_amp_ini_flow(void){
	amp_basic_info_t basic_info;
	unsigned char *pBuf = get_amp_table_buffer();

	if(pBuf == NULL){
		AMP_ERR("%s:line=%d,pBuf is NULL\n", __func__,__LINE__);
		return 0;
	}
	
	memset(&basic_info,0,sizeof(basic_info));
	memcpy(&basic_info,pBuf,sizeof(basic_info));

	if(basic_info.magic_num != 0x475a5a47 || basic_info.support_amp_num == 0){
		//AMP_ERR("%s:line=%d,magicNum error! magicNum=0x%x(should be 0x475a5a47),support_amp_num=%d\n", __func__,__LINE__,basic_info.magic_num,basic_info.support_amp_num);
		return 0;
	}
	AMP_INFO("%s:line=%d,new amp flow,support_amp_num=%d,init_stage=%d[1:boot,2:kernel,3:ap]\n", __func__,__LINE__,basic_info.support_amp_num,basic_info.init_stage);
	
	return 1;
}


int is_kernel_init_amp(void){
	amp_basic_info_t basic_info;
	unsigned char *pBuf = get_amp_table_buffer();
	if(pBuf == NULL){
		AMP_ERR("%s:line=%d,pBuf is NULL\n", __func__,__LINE__);
		return 0;
	}

	memset(&basic_info,0,sizeof(basic_info));
	memcpy(&basic_info,pBuf,sizeof(basic_info));

	if(basic_info.magic_num != 0x475a5a47 || basic_info.support_amp_num == 0){
		//AMP_ERR("%s:line=%d,magicNum error! magicNum=0x%x(should be 0x475a5a47),support_amp_num=%d\n", __func__,__LINE__,basic_info.magic_num,basic_info.support_amp_num);
		return 0;
	}
	//1:boot,2:kernel,3:ap
	if(basic_info.init_stage==2){
		AMP_ERR("[%s] amp init in kernel side, init_stage=%d(1:boot,2:kernel,3:ap)!\n", __func__,basic_info.init_stage);
		return 1;
	}
	return 0;
}

int get_amp_basic_info(amp_basic_info_t *basic_info){
	unsigned char *pBuf = get_amp_table_buffer();

	if(basic_info == NULL || pBuf == NULL){
		AMP_ERR("%s:line=%d,basic_info == NULL\n", __func__,__LINE__);
		return -1;
	}
	memcpy(basic_info,pBuf,sizeof(amp_basic_info_t));
	return 0;
}


int set_amp_eq_mode(char *eq_mode){
	unsigned char *pBuf = get_amp_table_buffer();
	amp_basic_info_t basic_info;
	amp_param_layout_t amp_layout_info; 
	unsigned char i = 0, amp_i2c_id= amp_get_i2c_id();
	AMP_I2C_CMD eqType = AMP_EQ_DESK_CMD;
	int ret = 0;

	memset(&basic_info,0,sizeof(basic_info));
	memcpy(&basic_info,pBuf,sizeof(amp_basic_info_t));

	if(eq_mode == NULL || pBuf ==NULL){
		AMP_ERR("%s:line=%d,Parameter error! eq=%p\n", __func__,__LINE__,eq_mode);
		return -1;
	}
	if(basic_info.magic_num != 0x475a5a47){
		AMP_ERR("%s:line=%d,magicNum error! magic_num=0x%x(should be 0x475a5a47)\n", __func__,__LINE__,basic_info.magic_num);
		return -2;
	}
	//AMP_ERR("%s:line=%d,eq=%s\n", __func__,__LINE__,eq_mode);

	if(strncmp(eq_mode, "desk", 4) == 0 || strncmp(eq_mode, "DESK", 4) == 0){
		basic_info.eq_mode = 1;
		eqType = AMP_EQ_DESK_CMD;
	}else if(strncmp(eq_mode, "wall", 4) == 0 || strncmp(eq_mode, "WALL", 4) == 0){
		basic_info.eq_mode = 2;
		eqType = AMP_EQ_WALL_CMD;
	}else if(strncmp(eq_mode, "bypass", 6) == 0 || strncmp(eq_mode, "BYPASS", 6) == 0){
		basic_info.eq_mode = 3;
		eqType = AMP_EQ_BYPASS_CMD;
	}else{
		basic_info.eq_mode = 0;
		eqType = AMP_EQ_NONE_CMD;
	}
	memcpy(pBuf,&basic_info,sizeof(amp_basic_info_t));

	if(basic_info.eq_mode >0){
		for(i=0;i<basic_info.support_amp_num;i++){
			memset(&amp_layout_info,0,sizeof(amp_param_layout_t));
			ret = get_amp_model_info(i,&amp_layout_info);
			if(ret<0){
				AMP_ERR("%s get_amp_model_info fail:line=%d,i=%d,ret=%d\n", __func__,__LINE__,i,ret);
				continue;
			}
			ret = do_amp_i2c_cmd(amp_i2c_id,eqType,amp_layout_info);
			if(ret<0){
				AMP_ERR("%s line=%d,excute eq i2c cmd fail,eq_mode=%d,ret=%d\n", __func__,__LINE__,basic_info.eq_mode,ret);
			}
		}
	}
	return 0;
}

int get_amp_model_info(unsigned char id,amp_param_layout_t *amp_layout_info){
	unsigned char *pBuf = get_amp_table_buffer();
	amp_basic_info_t basic_info;

	memcpy(&basic_info,pBuf,sizeof(amp_basic_info_t));
	pBuf+=sizeof(amp_basic_info_t);

	if(id >= basic_info.support_amp_num || amp_layout_info == NULL || pBuf == NULL){
		//AMP_ERR("%s:line=%d,Parameter error! id=%d,support_amp_num=%d,amp_layout_info=%p\n", __func__,__LINE__,id,basic_info.support_amp_num,amp_layout_info);
		return -1;
	}
	if(basic_info.magic_num != 0x475a5a47){
		//AMP_ERR("%s:line=%d,magicNum error! magic_num=0x%x(should be 0x475a5a47)\n", __func__,__LINE__,basic_info.magic_num);
		return -2;
	}

	memcpy(amp_layout_info,pBuf+sizeof(amp_param_layout_t) * (id),sizeof(amp_param_layout_t));
	return 0;
}


int do_amp_i2c_cmd(unsigned char amp_i2c_id,AMP_I2C_CMD cmdType,amp_param_layout_t amp_layout_info){
	unsigned char *pBuf = get_amp_table_buffer();
	unsigned char *cmd_buf = NULL;
	unsigned int cmd_buf_len = 0,retry_count=0;

	if(pBuf == NULL){
		AMP_ERR("%s:line=%d,Parameter error! cmdType=%d\n", __func__,__LINE__,cmdType);
		return -1;
	}

	//AMP_INFO("%s:line=%d,  cmdType=%d\n", __func__,__LINE__,cmdType);

	if(cmdType == AMP_RESET_CMD){
		if(amp_layout_info.soft_reset_cmd_len >0){
			cmd_buf_len = amp_layout_info.soft_reset_cmd_len;
			cmd_buf  = (unsigned char *)amp_malloc(cmd_buf_len);
			memset(cmd_buf,0,cmd_buf_len);
			memcpy(cmd_buf,pBuf+amp_layout_info.soft_reset_cmd_offset,cmd_buf_len);
		}else{
			AMP_ERR("%s:line=%d, no reset cmd\n", __func__,__LINE__);
			return -2;
		}
	}else if(cmdType == AMP_QS_INIT_CMD){
		if(amp_layout_info.init_qs_cmd_len >0){
			cmd_buf_len = amp_layout_info.init_qs_cmd_len;
			cmd_buf  = (unsigned char *)amp_malloc(cmd_buf_len);
			memset(cmd_buf,0,cmd_buf_len);
			memcpy(cmd_buf,pBuf+amp_layout_info.init_qs_cmd_offset,cmd_buf_len);
		}else{
			AMP_ERR("%s:line=%d, no qs cmd\n", __func__,__LINE__);
			return -2;
		}
	}else if(cmdType == AMP_INIT_CMD){
		if(amp_layout_info.init_cmd_len >0){
			cmd_buf_len = amp_layout_info.init_cmd_len;
			cmd_buf  = (unsigned char *)amp_malloc(cmd_buf_len);
			memset(cmd_buf,0,cmd_buf_len);
			memcpy(cmd_buf,pBuf+amp_layout_info.init_cmd_offset,cmd_buf_len);
		}else{
			AMP_ERR("%s:line=%d, no init cmd\n", __func__,__LINE__);
			return -2;
		}
	}else if(cmdType == AMP_EQ_DESK_CMD){
		if(amp_layout_info.eq_desk_cmd_len >0){
			cmd_buf_len = amp_layout_info.eq_desk_cmd_len;
			cmd_buf  = (unsigned char *)amp_malloc(cmd_buf_len);
			memset(cmd_buf,0,cmd_buf_len);
			memcpy(cmd_buf,pBuf+amp_layout_info.eq_desk_cmd_offset,cmd_buf_len);
		}else{
			AMP_ERR("%s:line=%d, no eq desk cmd\n", __func__,__LINE__);
			return -2;
		}
	}else if(cmdType == AMP_EQ_WALL_CMD){
		if(amp_layout_info.eq_wall_cmd_len >0){
			cmd_buf_len = amp_layout_info.eq_wall_cmd_len;
			cmd_buf  = (unsigned char *)amp_malloc(cmd_buf_len);
			memset(cmd_buf,0,cmd_buf_len);
			memcpy(cmd_buf,pBuf+amp_layout_info.eq_wall_cmd_offset,cmd_buf_len);
		}else{
			AMP_ERR("%s:line=%d, no eq wall cmd\n", __func__,__LINE__);
			return -2;
		}
	}else if(cmdType == AMP_EQ_BYPASS_CMD){
		if(amp_layout_info.eq_bypass_cmd_len >0){
			cmd_buf_len = amp_layout_info.eq_bypass_cmd_len;
			cmd_buf  = (unsigned char *)amp_malloc(cmd_buf_len);
			memset(cmd_buf,0,cmd_buf_len);
			memcpy(cmd_buf,pBuf+amp_layout_info.eq_bypass_cmd_offset,cmd_buf_len);
		}else{
			AMP_ERR("%s:line=%d, no eq bypass cmd\n", __func__,__LINE__);
			return -2;
		}
	}else{
		AMP_ERR("%s:line=%d,unsupport cmdType=%d\n", __func__,__LINE__,cmdType);
		return -2;
	}
	
	//AMP_ERR("%s:line=%d,cmd_buf_len=%d,start dump cmd_buf\n", __func__,__LINE__,cmd_buf_len);
	//dump_mem(cmd_buf,cmd_buf_len);
	//AMP_ERR("%s:line=%d, dump cmd_buf end\n", __func__,__LINE__);
	
	if(cmd_buf_len >0){
		unsigned char *pCur = cmd_buf;
		unsigned char *pEnd = cmd_buf+cmd_buf_len;
		unsigned char i2c_cmd_buf[256];
		unsigned int i2c_cmd_len = 0;
		unsigned int delayTime=1;
		while(pCur < pEnd){
			if(*pCur++ == 0x47){
				i2c_cmd_len = *pCur++;
				memset(i2c_cmd_buf,0,sizeof(i2c_cmd_buf));
				memcpy(i2c_cmd_buf,pCur,i2c_cmd_len);
				//dump_mem(i2c_cmd_buf,i2c_cmd_len);
				//AMP_ERR("%s:line=%d, init i2c_cmd_len=%d\n", __func__,__LINE__,i2c_cmd_len);
				//if (I2C_Write_EX(AMP_I2C, amp_layout_info.amp_addr,i2c_cmd_len, i2c_cmd_buf) < 0){
				if(i2c_cmd_len==7 && i2c_cmd_buf[0]==0xff && i2c_cmd_buf[1]==0xff && i2c_cmd_buf[2]==0xaa && i2c_cmd_buf[3]==0x55){
					delayTime = i2c_cmd_buf[6];
					//printf("%s:line=%d, init delayTime=%d\n", __func__,__LINE__,delayTime);
					mdelay(delayTime);
					pCur+=i2c_cmd_len;
					continue;
				}
				if (i2c_master_send_ex_flag(amp_i2c_id, amp_layout_info.amp_addr, i2c_cmd_buf, i2c_cmd_len, I2C_M_FAST_SPEED) < 0) {
					AMP_ERR("program %s failed\n",amp_layout_info.ampModelName);
					if(retry_count++ < 6){
						AMP_ERR("program %s always failed, init amp interrupt\n",amp_layout_info.ampModelName);
						break;
					}
					
				}
				pCur+=i2c_cmd_len;
			}
		}
	}
	if(cmd_buf)
		amp_free(cmd_buf);
	return 0;
}

void do_ampIniOps(void){
	amp_basic_info_t basic_info;
	amp_param_layout_t amp_layout_info; 
	unsigned char i=0;
	int ret=0;
	unsigned char amp_i2c_id= amp_get_i2c_id();

	memset(&basic_info,0,sizeof(amp_basic_info_t));

	get_amp_basic_info(&basic_info);
	if(basic_info.magic_num != 0x475a5a47 || basic_info.support_amp_num==0){
		//AMP_ERR("%s:line=%d,magicNum error! magicNum=0x%x(should be 0x475a5a47)\n", __func__,__LINE__,basic_info.magic_num);
		return;
	}
	if(basic_info.support_amp_num==0){
		AMP_ERR("%s:line=%d,support_amp_num==0, skip amp init\n", __func__,__LINE__);
		return;
	}
	AMP_ERR("%s:line=%d,amp_num=%d,eq_mode=%d[0:no need,1:desk,2:wall,3:bypass]\n", __func__,__LINE__,basic_info.support_amp_num,basic_info.eq_mode);

	for(i=0;i<basic_info.support_amp_num;i++){
		memset(&amp_layout_info,0,sizeof(amp_param_layout_t));
		ret = get_amp_model_info(i,&amp_layout_info);
		if(ret<0){
			AMP_ERR("%s fail:line=%d,i=%d,ret=%d\n", __func__,__LINE__,(i+1),ret);
			continue;
		}
		AMP_ERR("%s:line=%d,AMP[%d]:%s,addr=0x%02x\n", __func__,__LINE__,(i+1),amp_layout_info.ampModelName,amp_layout_info.amp_addr);
#ifdef BUILD_QUICK_SHOW
		ret = do_amp_i2c_cmd(amp_i2c_id,AMP_QS_INIT_CMD,amp_layout_info);
		if(ret<0 && ret!=-3){
			AMP_ERR("%s :line=%d,QS excute i2c cmd fail,ret=%d\n", __func__,__LINE__,ret);
			ret = do_amp_i2c_cmd(amp_i2c_id,AMP_INIT_CMD,amp_layout_info);
		}
		if(ret==0)
			AMP_ERR("%s :line=%d,QS excute i2c cmd done,ret=%d\n", __func__,__LINE__,ret);
#else
		ret = do_amp_i2c_cmd(amp_i2c_id,AMP_INIT_CMD,amp_layout_info);
		if(ret==0)
			AMP_ERR("%s :line=%d,kernel excute i2c cmd done,ret=%d\n", __func__,__LINE__,ret);
#endif
		if(ret == -3){
			AMP_ERR("program AMP[%d]:%s,addr=0x%x always failed, please check HW\n",(i+1),amp_layout_info.ampModelName,amp_layout_info.amp_addr);
			continue;
		}

		if(basic_info.eq_mode >0){
			AMP_I2C_CMD ed_mode[4] = {AMP_EQ_NONE_CMD,AMP_EQ_DESK_CMD,AMP_EQ_WALL_CMD,AMP_EQ_BYPASS_CMD};
			ret = do_amp_i2c_cmd(amp_i2c_id,ed_mode[basic_info.eq_mode],amp_layout_info);
			if(ret<0){
				//AMP_ERR("%s line=%d,excute eq i2c cmd fail,eq_mode=%d,ret=%d\n", __func__,__LINE__,basic_info.eq_mode,ret);
			}
		}
		
	}
	
}
#endif
