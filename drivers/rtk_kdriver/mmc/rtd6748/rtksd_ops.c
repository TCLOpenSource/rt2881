/*
 * MMC/SD/SDIO driver
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/mbus.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/mmc/host.h>
#include <asm/unaligned.h>

#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/workqueue.h>
#include <linux/completion.h>
#include "mmc_reg.h"
//#include "rtksdio.h"
#include "rtksd_ops.h"

#define MIS_GP0DATO_reg         0xB801B11C
#define MIS_GP0DIR_reg          0xB801B100

extern unsigned int test_start_flag;

#ifdef GPIO_DEBUG
void trans_db_gpio(void)
{
	u32 reginfo;
	/*
	 * 1. 0xB800_0804[31:28] = F    --> pin share as gpio
	 * 2. 0x1801_BC00[4] = 1  --> output mode
	 * 3. 0x1801_BC18[4]   is output data
	 */
	cr_writel(cr_readl(MIS_GP0DIR_reg) | 0x10, MIS_GP0DIR_reg);

	reginfo = cr_readl(MIS_GP0DATO_reg);
	if (reginfo & 0x10) {
		cr_writel(reginfo & ~0x10, MIS_GP0DATO_reg);
	} else {
		cr_writel(reginfo | 0x10, MIS_GP0DATO_reg);
	}

}

void trans_rst_gpio(void)
{
	u32 reginfo;
	cr_writel(cr_readl(MIS_GP0DIR_reg) | 0x00100000, MIS_GP0DIR_reg);
	reginfo = cr_readl(MIS_GP0DATO_reg);

	if (reginfo & 0x00100000) {
		cr_writel(reginfo & ~0x00100000, MIS_GP0DATO_reg);
	} else {
		cr_writel(reginfo | 0x00100000, MIS_GP0DATO_reg);
	}

}
#else
#define trans_db_gpio()
#define trans_rst_gpio()
#endif

char *rtkcr_parse_token(const char *parsed_string, const char *token)
{
	const char *ptr = parsed_string;
	const char *start, *end, *value_start, *value_end;
	char *ret_str;

	while (1) {
		value_start = value_end = 0;
		for (; *ptr == ' ' || *ptr == '\t'; ptr++) ;
		if (*ptr == '\0')
			break;
		start = ptr;
		for (;
		     *ptr != ' ' && *ptr != '\t' && *ptr != '=' && *ptr != '\0';
		     ptr++) ;
		end = ptr;
		if (*ptr == '=') {
			ptr++;
			if (*ptr == '"') {
				ptr++;
				value_start = ptr;
				for (; *ptr != '"' && *ptr != '\0'; ptr++) ;
				if (*ptr != '"'
				    || (*(ptr + 1) != '\0' && *(ptr + 1) != ' '
					&& *(ptr + 1) != '\t')) {
					break;
				}
			} else {
				value_start = ptr;
				for (;
				     *ptr != ' ' && *ptr != '\t' && *ptr != '\0'
				     && *ptr != '"'; ptr++) ;
				if (*ptr == '"') {
					break;
				}
			}
			value_end = ptr;
		}

		if (!strncmp(token, start, end - start)) {
			if (value_start) {
				ret_str =
				    kmalloc(value_end - value_start + 1,
					    GFP_KERNEL);
				/* KWarning: checked ok by alexkh@realtek.com */
				if (ret_str) {
					strncpy(ret_str, value_start,
						value_end - value_start);
					ret_str[value_end - value_start] = '\0';
				}
				return ret_str;
			} else {
				ret_str = kmalloc(1, GFP_KERNEL);
				/* KWarning: checked ok by alexkh@realtek.com */
				if (ret_str)
					strcpy(ret_str, "");
				return ret_str;
			}
		}
	}

	return (char *)NULL;
}

EXPORT_SYMBOL_GPL(rtkcr_parse_token);

void rtkcr_chk_param(u32 *pparam, u32 len, u8 *ptr)
{
	u32 value, i;

	*pparam = 0;
	for (i = 0; i < len; i++) {
		value = ptr[i] - '0';
		/* KWarning: checked ok by alexkh@realtek.com */
		if ((value >= 0) && (value <= 9)) {
			*pparam += value << (4 * (len - 1 - i));
			continue;
		}

		value = ptr[i] - 'a';
		/* KWarning: checked ok by alexkh@realtek.com */
		if ((value >= 0) && (value <= 5)) {
			value += 10;
			*pparam += value << (4 * (len - 1 - i));
			continue;
		}

		value = ptr[i] - 'A';
		/* KWarning: checked ok by alexkh@realtek.com */
		if ((value >= 0) && (value <= 5)) {
			value += 10;
			*pparam += value << (4 * (len - 1 - i));
			continue;
		}
	}
}

EXPORT_SYMBOL_GPL(rtkcr_chk_param);

