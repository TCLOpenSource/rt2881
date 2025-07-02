/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2010 by Chien-An Lin <colin@realtek.com.tw>
 *
 * PROM initialisation code.
 */
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>

//#include <asm/bootinfo.h>
#include <asm/io.h>

#include <asm/barrier.h>
//#include <asm/compiler.h>
#include <asm/cmpxchg.h>
#include <asm/switch_to.h>
//#include <asm/system_info.h>
#include <asm/system_misc.h>


//#include <generated/compile.h>
#ifndef CONFIG_ARM64
#include <asm/prom.h>
#endif
//#include <venus.h>
#include <mach/platform.h>
#include <mach/common.h>
#include <rtk_kdriver/io.h>
#include <linux/module.h>
#include <linux/version.h>
#include <rbus/stb_reg.h>

#include "rtd_types.h"

//#if IS_ENABLED(CONFIG_REALTEK_PCBMGR)
//#include <rtk_kdriver/pcbMgr.h>
//#endif

#include <rtk_kdriver/rtk-kdrv-common.h>
#include <rtd_log/rtd_module_log.h>
#ifdef CONFIG_REALTEK_DISABLE_PRINTK_WHEN_BOOT
// Stan add to ignore the print in the kernel beginning. BEGIN
extern int KERNEL_PRINT_ENABLE;
// Stan add to ignore the print in the kernel beginning. END
#endif

// Add to ignore all printk. BEGIN
#ifdef CONFIG_REALTEK_IGNORE_ALL_PRINTK
extern int KERNEL_PRINTALL_ENABLE;
#endif
// Add to ignore all printk. END

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
static ulong g_envp_addr;
#endif

unsigned int debugMode = 0;
EXPORT_SYMBOL(debugMode);

//int prom_argc;
//int *_prom_argv;
int *_prom_envp = 0;
// +++ cyhuang (2011/03/28)
//                          platform_info don't need resident in sleep.text section.
platform_info_t platform_info;
// +++ cyhuang (2011/03/28)
EXPORT_SYMBOL(platform_info);

// +++ cyhuang (2011/03/28)
#ifdef CONFIG_REALTEK_SATURN_SMP
extern struct plat_smp_ops rtk_smp_ops;
#endif
// +++ cyhuang (2011/03/28)

// power on video/logo/audio related info
phys_addr_t  reserved_dvr_start = 0;
phys_addr_t  reserved_dvr_size = 0;
phys_addr_t  reserved_last_image_start = 0;
phys_addr_t  reserved_last_image_size = 0;
phys_addr_t  reserved_qs_logo_start = 0;
phys_addr_t  reserved_qs_logo_size = 0;
#ifdef CONFIG_RTK_KDRV_KILLER
char reserved_no_kill_str[64];
int reserved_no_kill_size = 0;
bool reserved_disable_killer = false;
#endif /* CONFIG_RTK_KDRV_KILLER */

EXPORT_SYMBOL(reserved_dvr_start);
EXPORT_SYMBOL(reserved_qs_logo_start);
#ifdef CONFIG_RTK_KDRV_KILLER
EXPORT_SYMBOL(reserved_no_kill_str);
EXPORT_SYMBOL(reserved_no_kill_size);
EXPORT_SYMBOL(reserved_disable_killer);
#endif /* CONFIG_RTK_KDRV_KILLER */

/*
 * YAMON (32-bit PROM) pass arguments and environment as 32-bit pointer.
 * This macro take care of sign extension, if running in 64-bit mode.
 */

#ifdef CONFIG_ARM64
//This number is the same as bootcode
#define SYS_USER_ENVIRONMENT_MAX_INDEX 99
#define ENV_LIST_SIZE  (SYS_USER_ENVIRONMENT_MAX_INDEX*2)
static char *_env_list[ENV_LIST_SIZE];
#define prom_envp(index) _env_list[index]
#else
#define prom_envp(index) ((char *)((ulong)_prom_envp[(index)]))
#endif
/* donot use this function after "mem_init" because 0~1M are recycled in mem_init and prom_envp is located in that region. */
char *prom_getenv(char *envname)
{
	/*
	 * Return a pointer to the given environment variable.
	 * In 64-bit mode: we're using 64-bit pointers, but all pointers
	 * in the PROM structures are only 32-bit, so we need some
	 * workarounds, if we are running in 64-bit mode.
	 */
	int i, index=0;

// If debugging with jtag (that means Linux runs without bootloader), there will be no bootloader parameter passed to kernel.
	//return NULL;

	/* Prevent access NULL pointer */
	if (_prom_envp == 0)
		return NULL;

	i = strlen(envname);
	while (prom_envp(index)) {

		if(strncmp(envname, prom_envp(index), i) == 0) {
			//rtd_pr_mach_rtk_info("prom_getenv(%s):index = %d, prom_envp = %p\n", envname,index, prom_envp(index));
			return (prom_envp(index+1));
		}
		index += 2;
	}
//	rtd_pr_mach_rtk_info("prom_getenv(%s):Not found!\n",envname);
	return NULL;
}

/* Exp: parse_variable("ethaddr", platform_info.ethaddr, 20, "00.01.02.03.04.05")
		parse_variable will read the value of the variable in bootloader.
	variable_name: The name of variable
	store_ptr: The location to store the result
	max_len: The maximum length of the result
	default_ptr: If the variable cannot be found or be read and default_ptr is not NULL, use the default_str instead.
 */
int parse_variable(const char *variable_name, char *store_ptr, int max_len, const char *default_str)
{
	char *ptr;

	ptr = prom_getenv((char *)variable_name);
	if(ptr) {
		if(strlen(ptr) < max_len) {
			strncpy(store_ptr, ptr, max_len);
                        store_ptr[max_len-1] = 0;
			return 0;
		}
	}
	if(default_str) {
		strncpy(store_ptr, default_str, max_len);
                store_ptr[max_len-1] = 0;
        }
	return -1;
}

/*
	Exp: parse_series_variable("system_parameters_", platform_info.AES_CCMP, 571, 4, 1, 0)
		system_parameters_1, system_parameters_2, and ... will be read, joined, and put in platform_info.AES_CCMP
		If level = 1, only system_parameters_ will be read.
	variable_name: The prefix of variable name
	store_ptr: The location to store the result
	max_len: The maximum length of the result
	level: How many variables will be read
	contiguous: If "contiguous = 1", system_parameters_2 won't be parsed if system_parameters_1 doesn't exist. If "contiguous = 0", " " will be appended between variables.
	txt2bin: If "txt2bin = 1", text will be converted into binary like this: "43" ==> 'C'. If "txt2bin = 1", "contiguous" must be 1.
 */
int parse_series_variable(const char *variable_name, char *store_ptr, int max_len, int level, int contiguous, int txt2bin)
{
	int i, len=0, sublen, wholestr_ptr=0;
	char *ptr;
	char ext_variable_name[32];

	memset(store_ptr, 0, max_len);
	if(strlen(variable_name)>32-2) {
		rtd_pr_mach_rtk_info("%s,%d: Variable name too long!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	if(level<1 || level > 99) {
		rtd_pr_mach_rtk_info("%s,%d: Parameter \"level\" error!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	if(txt2bin && !contiguous) {
		rtd_pr_mach_rtk_info("%s,%d: Parameter \"txt2bin\" error!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	for(i=1;i<=level;i++) {
		if(level == 1)
			snprintf(ext_variable_name,32, "%s", variable_name);
		else			
			snprintf(ext_variable_name,32, "%s%d", variable_name, i);
		ptr = prom_getenv(ext_variable_name);
//		rtd_pr_mach_rtk_info("%s=%s\n", ext_variable_name, ptr);
		if(ptr) {
			sublen = strlen(ptr);
			if(txt2bin) {
				if(sublen%2) {
					rtd_pr_mach_rtk_info("Error! If \"txt2bin = 1\", there must be even number of chars in a sub-string.\n");
					return -1;
				}
				len+=sublen/2;
			} else {
				len+=sublen;
				if(i>1 && !contiguous)
					len+=1;
			}
			if(len <= max_len && (txt2bin || len < max_len)) {
				if(txt2bin) {
					char halfword;
					unsigned char wholeword=0;
					int substr_ptr;

					for(substr_ptr=0;substr_ptr<sublen;substr_ptr++) {
						halfword = *(ptr+substr_ptr);
						if(halfword>='0' && halfword<='9')
							halfword -= '0';
						else if(halfword>='a' && halfword<='f')
							halfword -= ('a'-10);
						else if(halfword>='A' && halfword<='F')
							halfword -= ('A'-10);
						else {
							rtd_pr_mach_rtk_info("%s,%d: Error! Not legal number char!\n", __FUNCTION__, __LINE__);
							return -1;
						}
						if(substr_ptr%2) {
							wholeword += halfword;
							*(store_ptr+wholestr_ptr) = (char) wholeword;
							wholestr_ptr++;
						} else
							wholeword = halfword<<4;
					}
				} else {
					if(!contiguous && i>1)
						strncat(store_ptr, " ", max_len - strlen(store_ptr)-1);
					strncat(store_ptr, ptr, max_len - strlen(store_ptr)-1);
				}
			} else {
				rtd_pr_mach_rtk_info("%s,%d: Overflow. Skip the other data!\n", __FUNCTION__, __LINE__);
				break;
			}
		} else if(contiguous)
			break;
	}

	if(txt2bin)
		return wholestr_ptr;
	else {
		store_ptr[strlen(store_ptr)]=0;
		return strlen(store_ptr);
	}
}

void prom_init(void)
{
	int i = 0, len;
	char *ptr;
	char parse_tmpbuf[128];

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
	_prom_envp = (int *)phys_to_virt(g_envp_addr);
#endif

#if 1
	if (_prom_envp != 0)
	{
#ifdef CONFIG_ARM64	

        while(_prom_envp[i]) {

            if ( i >= ENV_LIST_SIZE ) {
                rtd_pr_mach_rtk_info("WARN!!, prom_init overflow!\n");
                break;
            }
            //rtd_pr_mach_rtk_info("_prom_envp[%d] = 0x%08x ...", i, _prom_envp[i]);
            _env_list[i] = (char*)phys_to_virt((phys_addr_t)_prom_envp[i]); //name
            _env_list[i+1] = (char*)phys_to_virt((phys_addr_t)_prom_envp[i+1]); //value
            //rtd_pr_mach_rtk_info("virt(%p)\n",_env_list[i]);

            i += 2;

        }
        _env_list[i] = NULL;

#else
		while(_prom_envp[i]) {

//			rtd_pr_mach_rtk_err("_prom_envp[%d] = 0x%08x ...", i, _prom_envp[i]);

			_prom_envp[i] = (ulong)phys_to_virt((ulong)_prom_envp[i]);

//			rtd_pr_mach_rtk_err("0x%08x\n", _prom_envp[i]);

			i++;
		}
#endif
	}
	else
	{
		rtd_pr_mach_rtk_err("warning: _prom_envp is %p in prom_init\n", _prom_envp);
	}
#endif

// Add to ignore all printk. BEGIN
#ifdef CONFIG_REALTEK_IGNORE_ALL_PRINTK
	KERNEL_PRINTALL_ENABLE = 0;
#endif
// Add to ignore all printk. END

// Stan add to ignore the print in the kernel beginning. BEGIN
#ifdef CONFIG_REALTEK_DISABLE_PRINTK_WHEN_BOOT
	KERNEL_PRINT_ENABLE = 0;
#else
	// We will have printk in the kernel init.
	// KERNEL_PRINT_ENABLE = 1;
#endif

#ifdef CONFIG_REALTEK_VOLTAGE_CTRL
        platform_info.enable_dvs = 1;
#endif

#if defined(CONFIG_RTK_KDRV_SERIAL_8250)
#ifdef CONFIG_REALTEK_SYSTEM_OWNS_SECOND_UART
	rtd_pr_mach_rtk_info(" System CPU has 2 UARTs.\n");
#else
	rtd_pr_mach_rtk_info(" System CPU has 1 UART.\n");
#endif
#endif

#ifdef CONFIG_REALTEK_USE_EXTERNAL_TIMER_INTERRUPT
	rtd_pr_mach_rtk_info(" System CPU uses external timer interrupt.\n");
#else
	rtd_pr_mach_rtk_info(" System CPU uses internal timer interrupt.\n");
#endif

	platform_info.update_mode = 0;
/////	sprintf(platform_info.kernel_source_code_info, "%s\n%s", LINUX_SOURCE_CODE_SVN, UTS_VERSION);

	ptr = prom_getenv("bootrev"); // support format 0001.0203.0405
	if(ptr) {
		int state=0;
		unsigned int chip_id = 0, chip_info = 0;
		char *pos1, *pos2;
		char id1[6], id2[6];

		strncpy(platform_info.bootloader_version, ptr, min((int)(strlen(ptr)),31));
		platform_info.bootloader_version[31]=0;
		pos1 = strchr(ptr, '.');
		pos2 = strrchr(ptr, '.');
		if(pos1 == (ptr+4) && pos2 == (ptr+9))
		{
			rtd_pr_mach_rtk_info(" Bootloader version: %s.\n", ptr);

			strncpy(id1, ptr, 4);
			id1[4] = 0;

			strncpy(id2, ptr+5, 4);
			id2[4]=0;
		}
		else {
			rtd_pr_mach_rtk_info(" The format of Bootloader version is unidentified. Reset it to default.\n");

			strncpy(id1, "0000", 5);
			strncpy(id2, "0000", 5);
		}

		platform_info.company_id = simple_strtol(id1, (char **)NULL, 16);
		platform_info.board_id = simple_strtol(id2, (char **)NULL, 16);
		platform_info.cpu_id = (platform_info.board_id & 0xff00) >> 8;
		platform_info.board_id += (platform_info.company_id << 16);

#ifndef CONFIG_REALTEK_FPGA
		chip_id = (rtd_inl(STB_SC_VerID_reg)&0xffff0000) >> 16;
		chip_info = (rtd_inl(STB_SC_VerID_reg)&0xffff);
		switch(chip_id) {
			case 0x6330:
				platform_info.cpu_id = realtek_298x_cpu;
				break;

			case 0x6363:
				platform_info.cpu_id = realtek_294x_cpu;
				break;

			default:
				state = 3;
				break;
		}

		if(state == 1)
			rtd_pr_mach_rtk_info(" Error! Bootloader has a wrong CPU ID? \n");
		else if(state == 2)
			rtd_pr_mach_rtk_info(" Error! Unknown SB2_CHIP_INFO. Linux is too old? \n");
		else if(state == 3)
			rtd_pr_mach_rtk_info(" Error! Unknown SB2_CHIP_ID. Linux is too old? \n");
		else
			rtd_pr_mach_rtk_info(" Company:0x%x  CPU:0x%x  Board:0x%x\n", platform_info.company_id, platform_info.cpu_id, platform_info.board_id);
#endif

	} else {
		rtd_pr_mach_rtk_info(" Bootloader version is unidentified. Reset it to default.\n");
		platform_info.board_id = realtek_294x_demo_board;
		platform_info.cpu_id = realtek_294x_cpu;
		platform_info.company_id = company_realtek;
		strncpy(platform_info.bootloader_version, "NULL", 31);
	}

	parse_variable("ethaddr", platform_info.ethaddr, 20, "00.01.02.03.04.05");
	rtd_pr_mach_rtk_info(" Ethernet Mac address: %s\n", platform_info.ethaddr);
	if(prom_getenv("Gbit"))
		platform_info.ethtype = 1;
	else
		platform_info.ethtype = 0;
	if(!parse_variable("usb_param", platform_info.usb_param, 20, ""))
		rtd_pr_mach_rtk_info(" USB parameters: %s\n", platform_info.usb_param);
	if(!parse_variable("usb1_param", platform_info.usb1_param, 20, ""))
		rtd_pr_mach_rtk_info(" USB1 parameters: %s\n", platform_info.usb1_param);
	if(!parse_variable("usb2_param", platform_info.usb2_param, 20, ""))
		rtd_pr_mach_rtk_info(" USB2 parameters: %s\n", platform_info.usb2_param);

	platform_info.AES_CCMP_len = parse_series_variable("AES_CCMP_", platform_info.AES_CCMP, AES_CCMP_LEN, 6, 1, 1);
#if IS_ENABLED(CONFIG_REALTEK_PCBMGR)
#ifdef CONFIG_CUSTOMER_TV010
    platform_info.panel_parameter_len = parse_series_variable("panel_parameter_", platform_info.panel_parameter, PANEL_PARAMETER_LEN, 4, 1, 1);
#else
	platform_info.panel_parameter_len = parse_series_variable("panel_parameter_", platform_info.panel_parameter, PANEL_PARAMETER_LEN, 4, 1, 1);
#endif
	platform_info.pcb_enum_parameter_len = parse_series_variable("pcb_parameter_", platform_info.pcb_enum_parameter, PCB_ENUM_PARAMETER_LEN, PCB_ENUM_PARAMETER_LEVEL, 1, 0);
	platform_info.pcb_st_enum_parameter_len = parse_series_variable("pcb_st_parameter_", platform_info.pcb_st_enum_parameter, PCB_ENUM_PARAMETER_LEN, PCB_ENUM_PARAMETER_LEVEL, 1, 0);
//	rtd_pr_mach_rtk_info("pcb_enum_parameter_len=%d\n", platform_info.pcb_enum_parameter_len);
#endif

	parse_series_variable("system_parameters_", platform_info.system_parameters, SYSTEM_PARAMETERS_LEN, 6, 0, 0);

	if(!parse_series_variable("mmc_param_", platform_info.mmc_param, MMC_PARAM_LEN, 12, 0, 0))
		rtd_pr_mach_rtk_info(" emmc parameters: %s\n", platform_info.mmc_param);

	parse_series_variable("signature_", platform_info.signature, SIGNATURE_LEN, 2, 1, 0);
	platform_info.modelconfig_len=parse_series_variable("modelconfig_", platform_info.modelconfig,MODEL_CONFIG_LEN,1,1,1);
//	rtd_pr_mach_rtk_info(" Model Config length=%d\n",platform_info.modelconfig_len);

	platform_info.secure_boot=0;
	parse_series_variable("AES_IMG_KEY", platform_info.AES_IMG_KEY, AES_IMG_KEY_LEN, 1, 1, 1);
	if(parse_series_variable("RSA_KEY_MODULUS_", platform_info.RSA_KEY_MODULUS, RSA_KEY_MODULUS_LEN, 3, 1, 1) == 128)
		platform_info.secure_boot=1;
	parse_variable("rtc_base_date_year", parse_tmpbuf, 128, "2013");
	platform_info.rtc_base_date_year = simple_strtol(parse_tmpbuf, NULL, 10);
	if(platform_info.rtc_base_date_year < 1970) {
		rtd_pr_mach_rtk_info(" Base year of RTC error. Reset to year 2013.\n");
		platform_info.rtc_base_date_year = 2013;
	} else
		rtd_pr_mach_rtk_info(" Base year of RTC is %d.\n", platform_info.rtc_base_date_year);

	parse_series_variable("_custom_param_", platform_info._custom_param_, _CUSTOM_PARAM_LEN, 50, 1, 0);
	
	parse_variable("androidboot", platform_info.androidboot, 20, "");

	/* Prevent access NULL pointer */
	if (_prom_envp != 0)
	{
		for(i=0, len=0; prom_envp(i); i+=2) {
			char *varname = prom_envp(i);
			int varlen = strlen(varname);

			if(strncmp("_priv_", varname, strlen("_priv_")) == 0 &&
				varlen > (strlen("_priv_")+1) && varname[varlen-1] == '1')
			{
				char subname[128];

				if(len) {
					if(len >= _PRIV_LEN-1) {
						rtd_pr_mach_rtk_info("%s,%d: Overflow. Skip the other data!\n", __FUNCTION__, __LINE__);
						break;
					}
					strcat(platform_info._priv_, " ");
					len += 1;
				}
				strncpy(subname, varname, varlen-1);
				subname[varlen-1] = 0;
				if((len+strlen(subname)-strlen("_priv_")+1+1) >= _PRIV_LEN) {
					rtd_pr_mach_rtk_info("%s,%d: Overflow. Skip the other data!\n", __FUNCTION__, __LINE__);
					break;
				}
				else
					snprintf(platform_info._priv_+len, _PRIV_LEN-len-2, "%s=", subname+strlen("_priv_"));
				len += strlen(subname)-strlen("_priv_")+1;
				// KWarning: checked ok by eaton.chiou@realtek.com
				parse_series_variable(subname, platform_info._priv_+len, _PRIV_LEN-len, 9, 1, 0);
				len += strlen(platform_info._priv_+len);
			}
		}
	}

/*
// These lines are for debugging when you encounter "irq 3: nobody cared!"
	rtd_pr_mach_rtk_info("Registers: VENUS_MIS_UMSK_ISR: 0x%08X\tVENUS_MIS_ISR: 0x%08X\n", inl(VENUS_MIS_UMSK_ISR), inl(VENUS_MIS_ISR));
	outl(inl(VENUS_MIS_U0LCR)&~0x80, VENUS_MIS_U0LCR);
	rtd_pr_mach_rtk_info("Registers: VENUS_MIS_U0IIR_FCR: 0x%08X\tVENUS_MIS_U0IER_DLH: 0x%08X\n", inl(VENUS_MIS_U0IIR_FCR), inl(VENUS_MIS_U0IER_DLH));
	outl(inl(VENUS_MIS_U1LCR)&~0x80, VENUS_MIS_U1LCR);
	rtd_pr_mach_rtk_info("Registers: VENUS_MIS_U1IIR_FCR: 0x%08X\tVENUS_MIS_U1IER_DLH: 0x%08X\n", inl(VENUS_MIS_U1IIR_FCR), inl(VENUS_MIS_U1IER_DLH));
*/

/////	prom_init_cmdline();
/////	prom_meminit();

// +++ cyhuang (2011/03/28)
#ifdef CONFIG_REALTEK_SATURN_SMP
   register_smp_ops(&rtk_smp_ops);
#endif
// +++ cyhuang (2011/03/28)

}

/*
 * Pick out the environment pointer.  We look for envp=ptr,
 * where environment pointer is ptr.
 */
static int early_envp(char *envp_str)
{
	extern int *_prom_envp;
	ulong envp_addr;
        char *endp = NULL;

	//  rtd_pr_mach_rtk_err("Setup envp... %s\n", envp_str);

        envp_addr = simple_strtoul(envp_str, &endp, 16);

	_prom_envp = (int *)phys_to_virt(envp_addr);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
	g_envp_addr = envp_addr;
#endif
	//  rtd_pr_mach_rtk_err("Setup envp_addr=%lx (_prom_envp=%x)\n", envp_addr, (unsigned int)_prom_envp);

	return 0;
}

/*
 * Pick out the reserved memory that will be reclaimed later.  We look for reclaim=size@start,
 * where start and size are "size[KkMm]"
 */
static int early_reclaim(char *p)
{
	char *endp;

	reserved_dvr_start = PHYS_OFFSET;
	reserved_dvr_size  = memparse(p, &endp);
	if (*endp == '@')
		reserved_dvr_start = memparse(endp + 1, NULL);

	rtd_pr_mach_rtk_info("[POV] reclaim=0x%08llx@0x%08llx\n", reserved_dvr_size, reserved_dvr_start);

	return 0;
}

/*
 * Pick out the last_image memory that will be reclaimed later.  We look for last_image=size@start,
 * where start and size are "size[KkMm]"
 */
static int early_last_image(char *p)
{
	char *endp;

	reserved_last_image_start = PHYS_OFFSET;
	reserved_last_image_size  = memparse(p, &endp);
	if (*endp == '@')
		reserved_last_image_start = memparse(endp + 1, NULL);

	//	rtd_pr_mach_rtk_info("last_image=0x%08x@0x%08x\n", reserved_last_image_size, reserved_last_image_start);

	return 0;
}

/*
 * Pick out the last_image memory that will be reclaimed later.  We look for last_image=size@start,
 * where start and size are "size[KkMm]"
 */
static int early_qs_logo(char *p)
{
	char *endp;

	reserved_qs_logo_start = 0;
	reserved_qs_logo_size = memparse(p, &endp);
	if (*endp == '@')
		reserved_qs_logo_start = memparse(endp + 1, NULL);

	rtd_pr_mach_rtk_notice("[QS] %s: qs_logo=0x%08x@0x%08x\n", __func__, reserved_qs_logo_size, reserved_qs_logo_start);

	return 0;
}

#ifdef CONFIG_RTK_KDRV_KILLER
static int early_disable_killer(char *p)
{
    if (*p == '1') {
        reserved_disable_killer = true;
	    rtd_pr_mach_rtk_notice("[KILLER] Disable Killer from cmdline\n");
    }
    return 0;
}

/*
 * Pick out the non-kill list which would be killed by RTK Killer
 * where "size@A,B,C,D,..."
 */
static int early_no_kill_list(char *p)
{
	char *endp;
	int i = 0;

	reserved_no_kill_size = memparse(p, &endp);
	rtd_pr_mach_rtk_notice("[KILLER] non-kill tasks size is %d\n", reserved_no_kill_size);

	if (*endp++ == '@' && reserved_no_kill_size != 0) {
		rtd_pr_mach_rtk_notice("[KILLER] %s are NOT killed by RTK Killer\n", endp);
		
		while (strlen(endp) < 64)
			reserved_no_kill_str[i++] = *endp++;
		
		while (i < 64)
			reserved_no_kill_str[i++] = '\0';
	}
	return 0;
}
#endif /* CONFIG_RTK_KDRV_KILLER */

static int early_debugMode(char *str)
{
	ulong val;
	int err;

	if (!str) {
		debugMode = RELEASE_LEVEL;
		return 0;
	}

	err = kstrtoul(str, 16, &val);
	if(err)
		return err;

	debugMode = (unsigned int)val;

	return 0;
}

rtk_rtdlog_info_t console_rtdlog={0}; //FIXME, need to reove by hao_liu 20220324 talk

#ifdef CONFIG_RTK_KDRV_COMMON
early_param("envp", early_envp);
early_param("reclaim", early_reclaim);
early_param("last_image", early_last_image);
early_param("qs_logo", early_qs_logo);
#ifdef CONFIG_RTK_KDRV_KILLER
early_param("no_kill_list", early_no_kill_list);
early_param("disable_killer", early_disable_killer);
#endif /* CONFIG_RTK_KDRV_KILLER */
//early_param("debugMode", early_debugMode);
#else//MODULE
void parse_envp(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("envp", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get envp from bootargs\n");
		return;
	}
	early_envp(str);
}
void parse_reclaim(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("reclaim", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get reclaim from bootargs\n");
		return;
	}
	early_reclaim(str);
}
void parse_last_image(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("last_image", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get last_image from bootargs\n");
		return;
	}
	early_last_image(str);
}


void parse_qs_logo(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("qs_logo", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get qs_logo from bootargs\n");
		return;
	}
	early_qs_logo(str);
}

#ifdef CONFIG_RTK_KDRV_KILLER
void parse_disable_killer(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("disable_killer", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get disable_killer from bootargs\n");
		return;
	}
	early_disable_killer(str);
}

void parse_no_kill_list(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("no_kill_list", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get no_kill_list from bootargs\n");
		return;
	}
	early_no_kill_list(str);
}
#endif /* CONFIG_RTK_KDRV_KILLER */

#endif
void disable_panic_timeout_reboot(void)
{
	if(debugMode != RELEASE_LEVEL)
		panic_timeout = 0;
}
void parse_debugMode(void)
{
	char str[16];

	if(rtk_parse_commandline_equal("debugMode", str, sizeof(str)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get debugMode from bootargs\n");
	}else{
		early_debugMode(str);
	}
	disable_panic_timeout_reboot();
}

