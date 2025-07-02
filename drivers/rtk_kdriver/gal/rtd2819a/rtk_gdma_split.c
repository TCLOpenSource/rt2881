#include <generated/autoconf.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/pageremap.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/syscalls.h>
#ifdef CONFIG_ARM64
#include <rtk_kdriver/io.h>
#endif
#include <rtk_kdriver/rtk-kdrv-common.h>
#include <linux/uaccess.h>
#include <asm/barrier.h>
#include <asm/cacheflush.h>
#include <mach/platform.h>
#include <mach/system.h>
#include <rbus/sb2_reg.h>
#include <rbus/pif_reg.h>
#include <gal/rtk_gdma_split.h>
#include <gal/rtk_gdma_export_user.h>

#include <gal/rtk_gdma_driver.h>

#include <rtk_kdriver/i2c_h5_customized.h>
#include <rtk_kdriver/i2c-rtk-api.h>
//#include "i2c-rtk-api.h"


extern int gDebugGDMA_loglevel;


extern int i2c_master_send_ex_flag(unsigned char bus_id, unsigned char addr, unsigned char *write_buff, unsigned int write_len, __u16 flags);
extern int gDebugGDMA_loglevel;
int gOSD_SPLIT_MODE=0;

extern int change_OSD_mode;
int GDMA_split_mode(int mode)
{	
	//The following setting should be set on bootcode panel side
	//rtd_outl(PIF_PIF_PN_CTRL_reg,rtd_inl(PIF_PIF_PN_CTRL_reg)|0xfff);
	return 1;
}

#if !defined(CONFIG_RTK_KDRV_GDMA_MODULE)
static int __init early_parse_osd_split_mode(char *cmdline)
{
    static char *mode;
    mode = cmdline;
    if(cmdline != NULL){
        change_OSD_mode = (int)simple_strtoul(mode, &mode, 10);
		pr_debug("[GDMA] osd_split_mode=%d\n", change_OSD_mode);
    }
    return 0;
}
early_param("osd_split_mode", early_parse_osd_split_mode);
#else
void rtk_parse_osd_split_mode(void)
{
    char str[4]={0};
    char *mode;
    if(rtk_parse_commandline_equal("osd_split_mode", str, sizeof(str)) == 0)
    {
        pr_err("[GDMA] WARNING: can't get osd_split_mode from bootargs, use default value change_OSD_mode:%d\n",change_OSD_mode);
        return;
    }

    mode = str;
    change_OSD_mode = (int)simple_strtoul(mode, &mode, 10);
    pr_debug("[GDMA] osd_split_mode=%d\n", change_OSD_mode);
    return;
}
#endif



int gdma_split_notify( int enable, int param2)
{
//#ifdef I2C_RTK_API_SUPPORT
#if !defined(CONFIG_REALTEK_RESCUE_LINUX)
    unsigned char buf[7];
    unsigned char port_id = 1;
    unsigned char i2c_addr = H5X_I2C_ADDR;

    if(enable) {
        gOSD_SPLIT_MODE = 1;
    } else {
        gOSD_SPLIT_MODE = 0;
    }

    memset(buf, 0, sizeof(buf));
    buf[0] = I2C_ONLY_OSD;
    buf[1] = 0; //H5X's OSD_RLK_I2C_ONLY_VSC_SOURCE is 0
    buf[2] = gOSD_SPLIT_MODE; // Split ON/OFF
    buf[3] = 1;   // Param2
    buf[4] = 2;   // Param3 (reserved)
    buf[5] = 3;   // Param4 (reserved)
    buf[6] = 0xFE; //checksum
    i2c_master_send_ex_flag(port_id, i2c_addr, buf, 7, I2C_M_FAST_SPEED);

    GDMA_PRINT(GDMA_LOG_DUMP, "%s split:%d, I2C send to H5X [%d,%d,%d,%d,%d,%d,%d] \n", __FUNCTION__, gOSD_SPLIT_MODE, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
#endif
    return 0;
}
EXPORT_SYMBOL(gdma_split_notify);
