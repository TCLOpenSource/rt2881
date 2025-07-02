/*
 *  linux/arch/arm/mach-rtd299s/rtk_crt.c
 *
 *  Copyright (C) 2015 Realtek
 *  All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <rtd_log/rtd_module_log.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>

#include <rbus/sb2_reg.h>
#include <rbus/sys_reg_reg.h>
#include <mach/system.h>
#include <rtk_kdriver/io.h>

#include <linux/pageremap.h>
#include <mach/rtk_platform.h>
#include <rbus/md_reg.h>
#include <rbus/wdog_reg.h>
#include <rbus/dc_mc_reg.h>
#include <rbus/dc_mc3_reg.h>
#include <rbus/dc_sys_reg.h>
#include <rbus/mc_ptg_reg.h>
#include <rbus/scpu_core_reg.h>

#include <linux/kthread.h> //kthread_create(), kthread_run()

/* rtk_dft char device variable */
#define DFT_MAJOR 0   /* dynamic major by default */
#define DFT_NR_DEVS 1

#define RTKDFT_IOC_MAGIC  'D'
#define RTKDFT_IOC_PHASE_SCAN    _IO(RTKDFT_IOC_MAGIC, 1)
#define RTKDFT_IOC_GET_PHASE_SCAN_INFO    _IO(RTKDFT_IOC_MAGIC, 2)
#define RTKDFT_IOC_AUTO_PHASE_SCAN    _IO(RTKDFT_IOC_MAGIC, 3)

#define TX_VREF_VAL    8
#define TX_DQSPI       2
#define RX_VREF_TAP    5
#define RX_TE_TAP      3
#define BYTE_MAX       4
#define TX_VREF        2
#define RX_VREF        2
#define DC_NUM         2
#define DQ_NUM         8
#define DQM_NUM        (DQ_NUM+1)

#define MD_LENGTH      0x480000

#define HW_SETTING_POK_KEY_ADDR        0x00016c00
#define HW_SETTING_POK_TX_ADDR         (HW_SETTING_POK_KEY_ADDR+0x10)  //0x00016c10
#define HW_SETTING_POK_RX_ADDR         (HW_SETTING_POK_KEY_ADDR+0x20)  //0x00016c20
#define HW_SETTING_POK_CHECKSUM_ADDR   (HW_SETTING_POK_KEY_ADDR+0x200) //0x00016e00
#define HW_SETTING_POK_TX_TAG          0xA1A25566
#define HW_SETTING_POK_RX_TAG          0xA1A43344

int dft_major 		= DFT_MAJOR;
int dft_minor 		= 0;
int dft_nr_devs 	= DFT_NR_DEVS;
dev_t dft_devnum;
static struct class *dft_class;
static struct kobject *dft_kobj;
struct cdev dft_cdev;
spinlock_t dft_lock;

static struct device *dft_device;
static struct platform_device *dft_platform_devs;
static unsigned char OutData[128];

static signed char tx_pre_dqspi_set = 0;
static char byte_num[2] = {BYTE_MAX, BYTE_MAX};
struct task_struct *dft_kthread;
#ifdef CONFIG_COMPAT
static int g_dft_kthreadRunning = false;
static int g_dftphasescan_ret=-1;
#endif
#if IS_ENABLED(CONFIG_RTK_KDRV_MD)
extern bool smd_waitComplete(uint64_t handle, unsigned long timeout, uint64_t *hwCnt) ;
#endif

// return  0: OK,  -1: fail
static char MD_rw(const unsigned int addr, const unsigned int length)
{
	unsigned int get_wr_crc;//, get_rd_crc;
	unsigned int timeoutcnt;
	unsigned int start_addr, dst_addr;
	unsigned char retry = 5;
#if IS_ENABLED(CONFIG_RTK_KDRV_MD)
	static const unsigned long timeout = 100;

	uint64_t unused;
	while (smd_waitComplete(0, timeout, &unused) == false) {
		rtd_pr_dft_info("MD_rw: smd_waitComplete more than %lu ms\n", timeout);
	}
#endif

	do {
		start_addr = addr;
		dst_addr = start_addr + length;
		rtd_outl(MD_MD_W_CRC_CTRL_reg, 0x00000003);
		rtd_outl(MD_MD_REG_MODE_reg, 0x00000000);
		rtd_outl(MD_MD_W_CRC_MASK1_reg, 0xffffffff);
		rtd_outl(MD_MD_W_CRC_MASK2_reg, 0xffffffff);
		rtd_outl(MD_MD_REG_CTRL_reg, 0x00000002);
		rtd_outl(MD_MD_REG_CMD1_reg, 0x00000045);
		rtd_outl(MD_MD_CONST_ASCENDING_VALUE_reg, 0x00000004);
		rtd_outl(MD_MD_REG_CMD2_reg, start_addr);
		rtd_outl(MD_MD_REG_CMD4_reg, length);
		rtd_outl(MD_MD_REG_CMD5_reg, 0xA5A55A5A);
		rtd_outl(MD_MD_REG_MODE_reg, 0x00000001);

		//check done bit
		timeoutcnt = 0xfffff00;
		do {
			if(rtd_inl(MD_MD_REG_MODE_reg) == 0)
				break;
			udelay(10);
		} while(timeoutcnt--);

		//write CRC result
		get_wr_crc = rtd_inl(MD_MD_W_CRC_RESULT_reg);

		// MD read
		rtd_outl(MD_MD_DES_CRC_reg, get_wr_crc);
		rtd_outl(MD_MD_R_CRC_CTRL_reg, 0x00000007);
		rtd_outl(MD_MD_R_CRC_MASK1_reg, 0xffffffff);
		rtd_outl(MD_MD_R_CRC_MASK2_reg, 0xffffffff);
		rtd_outl(MD_MD_REG_MODE_reg, 0x00000000);
		rtd_outl(MD_MD_REG_CTRL_reg, 0x00000002);
		rtd_outl(MD_MD_REG_CMD1_reg, 0x00000005);
		rtd_outl(MD_MD_REG_CMD2_reg, dst_addr);
		rtd_outl(MD_MD_REG_CMD3_reg, start_addr);
		rtd_outl(MD_MD_REG_CMD4_reg, length);
		rtd_outl(MD_MD_REG_MODE_reg, 0x00000001);

		//check done bit
		timeoutcnt = 0xfffff00;
		do {
			if(rtd_inl(MD_MD_REG_MODE_reg) == 0)
				break;
			udelay(10);
		} while(timeoutcnt--);

		//Read CRC result
		//get_rd_crc = rtd_inl(MD_MD_R_CRC_RESULT_reg);

		if(!rtd_inl(MD_MD_CRC_STATUS_reg)) // value must = 0
			break;

	}while(retry--);

	if(retry)
		return 0;
	else
		return -1;
}

static void get_ddr_byte_num(void)
{
	char ddr_num = 0;

	//DC1: MC1 BYTE0-3; DC2: MC3 BYTE0-3
	if((rtd_inl(0xb80c9008) >> 31) & 0x1) // MC1 enable or not?
	{
		//MC1 => 0x18007000[1]: 0(2byte), 1(4byte)
		ddr_num = ((rtd_inl(DC_SYS_DC_DUMMY0_reg) >> 1) & 0x1) + 1;
		byte_num[0] = ddr_num*2;
	}

	if((rtd_inl(0xb80ca008) >> 31) & 0x1) // MC3 enable or not?
	{
		//MC3 => 0x18007000[2]: 0(2byte), 1(4byte)
		ddr_num = ((rtd_inl(DC_SYS_DC_DUMMY0_reg) >> 2) & 0x1) + 1;
		byte_num[1] = ddr_num*2;
	}

	//rtd_pr_dft_info("byte_num => %02x %02x\n", byte_num[0], byte_num[1]);
}

static void read_tx_vref(unsigned char dc, signed char *v_range, signed char *v_val, unsigned int *mr6)
{
	unsigned int mr6_val;
	unsigned int reg[DC_NUM] = {DC_MC_MC_DMY_FW9_reg, DC_MC_MC_DMY_FW10_reg}; //dummy for read vref
	unsigned int dummy_mr6 = SCPU_CORE_SC_DUMMY14_reg;
	unsigned char i;

	if(rtd_inl(reg[dc]) == 0)
	{
		rtd_maskl(DC_MC_MC_DBG_MOD_0_reg +dc*0x2000, 0x40ffffff, 0x86000000);
		mr6_val = (rtd_inl(DC_MC_MC_DBG_MOD_0_reg +dc*0x2000) >> 4) & 0x3ffff;

		*v_range = (mr6_val>>6) & 0x1;
		*v_val = (mr6_val & 0x3f);
		*mr6 = (mr6_val >> 7); //record mr6 except for vref range & value
		rtd_pr_dft_info("mr6: %x, %x, %x\n", *mr6, *v_range, *v_val);
	}
	else
	{
		for(i = 0; i < TX_VREF; i++)
		{
			if(rtd_inl(dummy_mr6) == 0)
				mr6_val = rtd_inl(reg[dc]) >> (i*16);
			else
				mr6_val = rtd_inl(dummy_mr6) >> (i*16);

			*(v_range+i) = (mr6_val>>6) & 0x1;
			*(v_val+i) = (mr6_val & 0x3f);
			*(mr6+i) = (mr6_val >> 7) & 0x1ff;
			rtd_pr_dft_info("mr6-%d: %x, %x, %x\n", i, *(mr6+i), *(v_range+i), *(v_val+i));
		}
	}
}

static void read_tx_dqspi(const unsigned char dc, signed char *dqspi)
{
	*(dqspi) = (rtd_inl(0xb80c9010+dc*0x1000) >> 16) & 0x1f;
	*(dqspi+1) = (rtd_inl(0xb80c9010+dc*0x1000) >> 24) & 0x1f;
	*(dqspi+2) = rtd_inl(0xb80c9014+dc*0x1000) & 0x1f;
	*(dqspi+3) = (rtd_inl(0xb80c9014+dc*0x1000) >> 8) & 0x1f;
}

static void check_vref_change_range(signed char *range, signed char *s)
{
	if((*range == 0) && (*s < 0)) { //change range 1 to 2. range2 base is 59.95%.
		*range = 1;
		*s += 0x17;
	}
	if((*range == 1) && (*s > 0x32)) { //change range 2 to 1. range1 base is 77.55%.
		*range = 0;
		*s -= 0x17; //0x32-0x1b = 0x17
	}
}

static void write_tx_vref_setting(unsigned char dc, unsigned int mr6, signed char tx_v_range, signed char tx_v_value)
{
	unsigned int offset = dc*0x2000;

	//MRS(Mode Register Set):bit[31:28]=0x6; [21:19]=0x6; [7]=0x1:enable, 0x0:disable
	rtd_outl(DC_MC_MC_CMD_0_reg +offset, (0x60300000 | (mr6 << 7) | (tx_v_range << 6) | tx_v_value));
	rtd_outl(DC_MC_MC_CMD_8_reg +offset, 0x001e0003); //cmd_ctl_t1[31:16],cmd_ctl_t0[15:0]
	rtd_outl(DC_MC_MC_CMD_ctl_0_reg +offset, 0x80000000); //cmd_start[31],cmd_execute_posi[9:8],cmd_num[2:0]
	rtd_outl(DC_MC_MC_CMD_ctl_0_reg +offset, 0x80000000);
}

static void set_tx_vref(unsigned char dc, signed char *range, signed char *s, unsigned int *mr6, const signed char diff)
{
	signed char tx_v_range, tx_v_value;
	unsigned int reg[DC_NUM] = {DC_MC_MC_DMY_FW9_reg, DC_MC_MC_DMY_FW10_reg}; //dummy for read vref
	unsigned int dummy_mr6 = SCPU_CORE_SC_DUMMY14_reg, tmp_val = 0;
	unsigned char i;

	if(rtd_inl(reg[dc]) == 0)
	{
		//range1: 60%~92.5%, range2: 45%~77.5%
		tx_v_range = *range;
		tx_v_value = (*s + diff);
		if(diff) {
			check_vref_change_range(&tx_v_range, &tx_v_value);
			rtd_pr_dft_info("{%02x %02x} (%d)\n", tx_v_range, tx_v_value, diff);
		}
		write_tx_vref_setting(dc, *mr6, tx_v_range, tx_v_value);
	}
	else
	{
		for(i = 0; i < TX_VREF; i++)
		{
			//range1: 60%~92.5%, range2: 45%~77.5%
			tx_v_range = *(range+i);
			tx_v_value = (*(s+i) + diff);
			if(diff) {
				check_vref_change_range(&tx_v_range, &tx_v_value);
				rtd_pr_dft_info("%d:{%02x %02x} (%d)\n", i, tx_v_range, tx_v_value, diff);
			}

			rtd_maskl(DC_MC_MC_SYS_IO_1_reg +dc*0x2000, 0xfffffcff, ((2-i) << 8)); //[9:8] => 2:set byte0/1; 1:set byte2/3; 0:normal
			write_tx_vref_setting(dc, *(mr6+i), tx_v_range, tx_v_value);
			tmp_val |= (((*(mr6+i) << 7) | (tx_v_range << 6) | tx_v_value) << (i*16));
		}
		rtd_maskl(DC_MC_MC_SYS_IO_1_reg +dc*0x2000, 0xfffffcff, 0x00000000); //[9:8]=0,cs_mrs_out_mode=0, normal mode
		rtd_outl(dummy_mr6, tmp_val); //record mr6 dummy
	}
}

static void set_tx_dqspi(const unsigned char dc, const signed char diff)
{
	signed char step;
	signed char pi_cur[BYTE_MAX];
	unsigned char i;

	read_tx_dqspi(dc, (signed char *)&pi_cur);

	step = diff - tx_pre_dqspi_set;
	while(step)
	{
		if(step > 0)
		{
			for(i = 0; i < byte_num[dc]; i++)
			{
				pi_cur[i] += 1;
				if(pi_cur[i] > 31) //up boundary
					pi_cur[i] -= 32;
			}
			step--;
		}
		else
		{
			for(i = 0; i < byte_num[dc]; i++)
			{
				pi_cur[i] -= 1;
				if(pi_cur[i] < 0) //low boundary
					pi_cur[i] += 32;
			}
			step++;
		}

		rtd_maskl(0xb80c9010+dc*0x1000, 0xe0e0ffff, ((pi_cur[1] << 24) | (pi_cur[0] << 16)));
		rtd_maskl(0xb80c9014+dc*0x1000, 0xffffe0e0, ((pi_cur[3] << 8) | pi_cur[2]));
		//rtd_pr_dft_info("%02x %02x %02x %02x\n", pi_cur[0], pi_cur[1], pi_cur[2], pi_cur[3]);
	}

	tx_pre_dqspi_set = diff;

}

int tx_calibration(unsigned int md_len, signed char vref_tap, signed char dqppi)
{
	signed char tx_vref_dqspi_set[4][2] = {{vref_tap, -dqppi}, {vref_tap, dqppi}, {-vref_tap, dqppi}, {-vref_tap, -dqppi}};
	signed char vref_range[TX_VREF], vref_value[TX_VREF];
	signed char dqs[BYTE_MAX] = {[0 ... (BYTE_MAX-1)] = 0};
	unsigned char round = 0, dc;
	char res = -1, limit = 4;
	unsigned long md_addr;
	unsigned int mr6_val[TX_VREF];
	void * md_vir;

	md_vir = dvr_malloc(md_len*2);
	md_addr = dvr_to_phys(md_vir);
	//rtd_pr_dft_info("md_addr = %lx\n", md_addr);
	if(md_addr == INVALID_VAL)
	{
		rtd_pr_dft_info("md malloc fail\n");
		return -1;
	}

	if(MD_rw((unsigned int)md_addr, md_len))
	{
		dvr_free(md_vir);
		return -2;
	}

	get_ddr_byte_num();
	for(dc = 0; dc < DC_NUM; dc++)
	{
		if(byte_num[dc] == 0)
			continue;

		//save original settings
		//Vref
		if(vref_tap == 0)
			limit = 2;

		read_tx_vref(dc, (signed char *)&vref_range, (signed char *)&vref_value, (unsigned int *)&mr6_val);
		read_tx_dqspi(dc, (signed char *)&dqs);

		for(round = 0; round < limit; round++)
		{
			//rtd_pr_dft_info("[%d, %d]\n", tx_vref_dqspi_set[round][0], tx_vref_dqspi_set[round][1]);
			set_tx_vref(dc, (signed char *)&vref_range, (signed char *)&vref_value, (unsigned int *)&mr6_val, tx_vref_dqspi_set[round][0]);
			set_tx_dqspi(dc, tx_vref_dqspi_set[round][1]);
			res = MD_rw((unsigned int)md_addr, md_len);
			if(res) //fail
				break;
		}

		//back to original point
		set_tx_vref(dc, (signed char *)&vref_range, (signed char *)&vref_value, (unsigned int *)&mr6_val, 0);
		set_tx_dqspi(dc, 0);

	}

	dvr_free(md_vir);

	if(res) //fail
		return (round+1);
	else
		return 0;

}

static void read_rx_vref(const unsigned char dc, signed char *v_range, signed char *v_val)
{
	unsigned int offset = dc*0x1000;

	*(v_range) = (rtd_inl(0xb80c91fc+offset)>>7) & 0x1;  // byte0/1
	*(v_range+1) = (rtd_inl(0xb80c91fc+offset)>>23) & 0x1; // byte2/3

	*(v_val) = (rtd_inl(0xb80c91fc+offset)>>1) & 0x3f;  // byte0/1 (S[5] bit no use)
	*(v_val+1) = (rtd_inl(0xb80c91fc+offset)>>17) & 0x3f; // byte2/3
}

static void read_rx_te(const unsigned char dc, unsigned char *Te_p, unsigned char *Te_n)
{
	short i, j;
	const unsigned int te_p_reg[BYTE_MAX] = {0xb80c9440, 0xb80c9444, 0xb80c9640, 0xb80c9644};
	unsigned int p_reg, n_reg;
	unsigned char pos = 0, shift;

	pos = 0;
	for(i = 0; i < byte_num[dc]; i++)
	{
		p_reg = te_p_reg[i]+dc*0x1000;
		n_reg = te_p_reg[i]+dc*0x1000+0x18; //te_n_reg[i];
		//rtd_pr_dft_info("rtd_inl(%08x) = %08x\n", p_reg, rtd_inl(p_reg));
		//rtd_pr_dft_info("rtd_inl(%08x) = %08x\n", n_reg, rtd_inl(n_reg));
		for(j = 0; j < DQM_NUM; j++)
		{
			if(((j%4) == 0) && (j != 0))
			{
				p_reg += 8;
				n_reg += 8;
				//rtd_pr_dft_info("rtd_inl(%08x) = %08x\n", p_reg, rtd_inl(p_reg));
				//rtd_pr_dft_info("rtd_inl(%08x) = %08x\n", n_reg, rtd_inl(n_reg));
			}

			shift = (j % 4) * 8;
			*(Te_p+pos) = (rtd_inl(p_reg) >> shift) & 0xff;
			*(Te_n+pos) = (rtd_inl(n_reg) >> shift) & 0xff;

			pos++;
		}
	}
}

static void set_rx_vref(unsigned char dc, signed char *range, signed char *s, signed char diff)
{
	signed char byte[RX_VREF]; //01, 23
	unsigned char i;
	signed char rx_v_range[RX_VREF], rx_v_value[RX_VREF];

	for(i = 0; i < RX_VREF; i++)
	{
		rx_v_range[i] = *(range+i);
		rx_v_value[i] = (*(s+i) + diff);
		if(diff)
		{
			//range=0, 29%~92%, range=1, 1.5%~64.5%
			if((rx_v_range[i] == 0) && (rx_v_value[i] < 0)) //change range 0 -> 1.
			{
				rx_v_range[i] = 1;
				rx_v_value[i] += 28; //29-1 = 28
			}
			if((rx_v_range[i] == 1) && (rx_v_value[i] > 31)) //change range 1 -> 0.
			{
				rx_v_range[i] = 0;
				rx_v_value[i] -= 28;
			}
		}
	}
	rtd_pr_dft_info("DC%d range: %02x %02x, s: %02x %02x\n", dc, rx_v_range[0], rx_v_range[1], rx_v_value[0], rx_v_value[1]);
	
	for(i = 0; i < RX_VREF; i++)
		byte[i] = ((rx_v_range[i] << 6) | rx_v_value[i]);

	rtd_outl(0xb80c91fc+dc*0x1000, ((byte[1] << 25) | (byte[1] << 17) | (byte[0] << 9) | (byte[0] << 1)));

	rtd_outl(0xb80c923c+dc*0x1000, 0x00000003); //update write delay tap
	rtd_outl(0xb80c923c+dc*0x1000, 0x0000000c); //update read delay tap
}

static void set_rx_te(unsigned char dc, signed char *Te_p, signed char *Te_n, signed char diff)
{
	const unsigned int te_p_reg[BYTE_MAX] = {0xb80c9440, 0xb80c9444, 0xb80c9640, 0xb80c9644};
	unsigned char i, j;
	unsigned int p_reg, n_reg;
	unsigned int p_val, n_val;
	unsigned char shift;
	unsigned char *te_p = Te_p;
	unsigned char *te_n = Te_n;

	for(i = 0; i < byte_num[dc]; i++)
	{
		p_reg = te_p_reg[i]+dc*0x1000;
		n_reg = te_p_reg[i]+dc*0x1000+0x18; //te_n_reg[i];
		p_val = 0;
		n_val = 0;
		for(j = 0; j < DQM_NUM; j++)
		{
			shift = (j % 4) * 8;
			p_val |= ((*te_p +diff) << shift);
			n_val |= ((*te_n +diff) << shift);
			te_p++;
			te_n++;

			if((j % 4) == 3)
			{
				rtd_outl(p_reg, p_val);
				rtd_outl(n_reg, n_val);
				//rtd_pr_dft_info("rtd_inl(%08x) = %08x\n", p_reg, p_val);
				//rtd_pr_dft_info("rtd_inl(%08x) = %08x\n", n_reg, n_val);
				p_reg += 8;
				n_reg += 8;
				p_val = 0;
				n_val = 0;
			}
		}
		rtd_outl(p_reg, p_val);
		rtd_outl(n_reg, n_val);
		//rtd_pr_dft_info("rtd_inl(%08x) = %08x\n", p_reg, p_val);
		//rtd_pr_dft_info("rtd_inl(%08x) = %08x\n", n_reg, n_val);
	}

	rtd_outl(0xb80c923c+dc*0x1000, 0x0000000c);
}

int rx_calibration(unsigned int md_len, signed char vref_tap, signed char te_tap)
{
	signed char rx_vref_te_set[4][2] = {{vref_tap, -te_tap}, {vref_tap, te_tap}, {-vref_tap, te_tap}, {-vref_tap, -te_tap}};
	signed char vref_range[RX_VREF], vref_s[RX_VREF];
	signed char te_p[BYTE_MAX][DQM_NUM], te_n[BYTE_MAX][DQM_NUM];
	unsigned char round, limit = 4, dc;
	char res = -1, autoK = 1;
	unsigned long md_addr;
	void * md_vir;

	md_vir = dvr_malloc(md_len*2);
	md_addr = dvr_to_phys(md_vir);
	//rtd_pr_dft_info("md_addr = %lx\n", md_addr);
	if(md_addr == INVALID_VAL)
	{
		rtd_pr_dft_info("md malloc fail\n");
		return -1;
	}

	if(MD_rw((unsigned int)md_addr, md_len))
	{
		dvr_free(md_vir);
		return -2;
	}

	get_ddr_byte_num();

	//save original settings
	//Vref
	if(vref_tap == 0)
		limit = 2;

	for(dc = 0; dc < DC_NUM; dc++)
	{
		read_rx_vref(dc, (signed char *)&vref_range, (signed char *)&vref_s);
		read_rx_te(dc, (signed char *)&te_p, (signed char *)&te_n);

		//disable 3point
		rtd_maskl(0xb80c9238+dc*0x1000, 0xFFFFFFCF, 0x00000030); // [5:4]=0x3

		if((rtd_inl(0xb80c9238+dc*0x1000) & 0x60000) == 0x60000) {
			autoK = 0;
		}
		else
		{
			rtd_maskl(0xb80c9238+dc*0x1000, 0xfff7ffff, 0x0); //dqs_en not update [19]=0
			rtd_maskl(0xb80c92d0+dc*0x1000, 0xffffffdf, (1<<5)); //delta k off [5]=1
		}

		for(round = 0; round < limit; round++)
		{
			//rtd_pr_dft_info("[%d %d]\n", rx_vref_te_set[round][0], rx_vref_te_set[round][1]);
			set_rx_vref(dc, (signed char *)&vref_range, (signed char *)&vref_s, rx_vref_te_set[round][0]);
			set_rx_te(dc, (signed char *)&te_p, (signed char *)&te_n, rx_vref_te_set[round][1]);
	
			res = MD_rw((unsigned int)md_addr, md_len);
			if(res) //fail
				break;
		}

		//back to original point
		set_rx_vref(dc, (signed char *)&vref_range, (signed char *)&vref_s, 0);
		set_rx_te(dc, (signed char *)&te_p, (signed char *)&te_n, 0);

		//enable 3point
		rtd_maskl(0xb80c9238+dc*0x1000, 0xFFFFFFCF, 0x00000000);// [5:4]=0x0
		if(autoK) {
			rtd_maskl(0xb80c9238+dc*0x1000, 0xfff7ffff, (1<<19));
			rtd_maskl(0xb80c92d0+dc*0x1000, 0xffffffdf, 0x0);
		}
	}

	dvr_free(md_vir);

	if(res) //fail
		return (round+1);
	else
		return 0;

}

void read_ck_pi(signed char *ck_pi)
{
	*ck_pi = rtd_inl(0xb80c9010) & 0x1f; //[4:0]: CK0 PI
	*(ck_pi+1) = rtd_inl(0xb80ca010) & 0x1f; //[4:0]: CK1 PI
}

void set_ck_pi(unsigned char dc, signed char ck_pi)
{
	signed char pi = ck_pi;

	if(pi > 31) //up boundary
		pi -= 32;

	if(pi < 0) //low boundary
		pi += 32;

	rtd_maskl(0xb80c9010+dc*0x1000, 0xffffffe0, pi);
}

void ddr_phase_scan_set(signed char ck, signed char tx_vref, signed char tx_dqspi, signed char rx_vref, signed char rx_te_tap)
{
	signed char ck_pi[DC_NUM];
	signed char tx_vref_range[TX_VREF], tx_vref_value[TX_VREF];
	signed char dqs[BYTE_MAX] = {[0 ... (BYTE_MAX-1)] = 0};
	signed char rx_vref_range[RX_VREF], rx_vref_s[RX_VREF];
	signed char te_p[BYTE_MAX][DQM_NUM], te_n[BYTE_MAX][DQM_NUM];
	unsigned char dc;
	unsigned int mr6_val[TX_VREF];
	char autoK = 1;

	get_ddr_byte_num();
	read_ck_pi((signed char *)&ck_pi);

	for(dc = 0; dc < DC_NUM; dc++)
	{
		if(byte_num[dc] == 0)
			continue;

		set_ck_pi(dc, (ck_pi[dc] +ck));

		read_tx_vref(dc, (signed char *)&tx_vref_range, (signed char *)&tx_vref_value, (unsigned int *)&mr6_val);
		read_tx_dqspi(dc, (signed char *)&dqs);

		tx_pre_dqspi_set = 0;
		set_tx_vref(dc, (signed char *)&tx_vref_range, (signed char *)&tx_vref_value, (unsigned int *)&mr6_val, tx_vref);
		set_tx_dqspi(dc, tx_dqspi);

		read_rx_vref(dc, (signed char *)&rx_vref_range, (signed char *)&rx_vref_s);
		read_rx_te(dc, (signed char *)&te_p, (signed char *)&te_n);

		//disable 3point
		rtd_maskl(0xb80c9238+dc*0x1000, 0xFFFFFFCF, 0x00000030); // [5:4]=0x3
		if((rtd_inl(0xb80c9238+dc*0x1000) & 0x60000) == 0x60000) {
			autoK = 0;
		}
		else
		{
			rtd_maskl(0xb80c9238+dc*0x1000, 0xfff7ffff, 0x0); //dqs_en not update [19]=0
			rtd_maskl(0xb80c92d0+dc*0x1000, 0xffffffdf, (1<<5)); //delta k off [5]=1
		}

		set_rx_vref(dc, (signed char *)&rx_vref_range, (signed char *)&rx_vref_s, rx_vref);
		set_rx_te(dc, (signed char *)&te_p, (signed char *)&te_n, rx_te_tap);

		//enable 3point
		rtd_maskl(0xb80c9238+dc*0x1000, 0xFFFFFFCF, 0x00000000);// [5:4]=0x0
		if(autoK) {
			rtd_maskl(0xb80c9238+dc*0x1000, 0xfff7ffff, (1<<19));
			rtd_maskl(0xb80c92d0+dc*0x1000, 0xffffffdf, 0x0);
		}
	}
}

static void get_tx_result(unsigned int *vref, signed char *pi)
{
	//signed char *data_ptr = phys_to_virt(HW_SETTING_POK_TX_ADDR);
	signed char vref_range[TX_VREF] = {0}, vref_value[TX_VREF] = {0};
	unsigned char i, dc;
	unsigned int mr6_val[TX_VREF];

	for(dc = 0; dc < DC_NUM; dc++)
	{
		if(byte_num[dc] == 0)
			continue;

		read_tx_vref(dc, (signed char *)&vref_range, (signed char *)&vref_value, (unsigned int *)&mr6_val);
		for(i = 0; i < TX_VREF; i++) {
			if(vref_value[i]) {
				if(vref_range[i] == 0)
					*(vref+i+dc*TX_VREF) = 6000 + 65*vref_value[i];
				else if(vref_range[i] == 1)
					*(vref+i+dc*TX_VREF) = 4500 + 65*vref_value[i];
			}
		}

		read_tx_dqspi(dc, (pi+dc*BYTE_MAX));
	}

}

static void get_rx_result(signed int *vref, signed char *Te_p, signed char *Te_n)
{
	//unsigned char *data_ptr = phys_to_virt(HW_SETTING_POK_RX_ADDR);
	signed char vref_range[RX_VREF], vref_value[RX_VREF];
	signed char te_p[BYTE_MAX][DQM_NUM], te_n[BYTE_MAX][DQM_NUM];
	unsigned char i, j, pos = 0, dc;

	//rtd_pr_dft_info("magic = %x\n", *(unsigned int *)data_ptr);
	for(dc = 0; dc < DC_NUM; dc++)
	{
		if(byte_num[dc] == 0)
			continue;
		
		read_rx_vref(dc, (signed char *)&vref_range, (signed char *)&vref_value);
		rtd_pr_dft_info("DC%d vref: %x %x %x %x\n", dc, vref_range[0], vref_value[0], vref_range[1], vref_value[1]);
		for(i = 0; i < RX_VREF; i++) {
			if(vref_range[i] == 0) {
				*(vref+i+dc*RX_VREF) = 29 + vref_value[i];
			}
			else if(vref_range[i] == 1) {
				*(vref+i+dc*RX_VREF) = 1 + vref_value[i];
			}
		}
		//rtd_pr_dft_info("%d vref: %x %x %x %x\n", __LINE__, *vref, *(vref+1), *(vref+2), *(vref+3));

		read_rx_te(dc, (signed char *)&te_p, (signed char *)&te_n);
		pos = 0;
		for(i = 0; i < BYTE_MAX; i++)
		{
			for(j = 0; j < DQM_NUM; j++)
			{
				*(Te_p+pos +dc*BYTE_MAX*DQM_NUM) = te_p[i][j];
				*(Te_n+pos +dc*BYTE_MAX*DQM_NUM) = te_n[i][j];
				pos++;
			}
		}
	}
}

static int ddr_phase_scan(void)
{
	int res1 = 0, res2 = 0, loop = 0;

	rtd_pr_dft_notice("\n=== Check Tx/Rx Calibration===\n");
	do {
		res1 = tx_calibration(MD_LENGTH, TX_VREF_VAL, TX_DQSPI);
		res2 = rx_calibration(MD_LENGTH, RX_VREF_TAP, RX_TE_TAP);

		loop++;
		if((res1 == 0) && (res2 == 0))
			rtd_pr_dft_notice("=========TX/RX PASS (%d)=========\n", loop);
		else {
			if(res1)
				rtd_pr_dft_emerg("=========TX FAIL (%d.%d)=========\n", loop, res1);
			if(res2)
				rtd_pr_dft_emerg("=========RX FAIL (%d.%d)=========\n", loop, res2);

			return loop;
		}
	}while(loop < 10);
	rtd_pr_dft_notice("\n================================\n");

	return 0;
}
#ifdef CONFIG_COMPAT
static int ddr_phase_scan_thread(void *arg)
{
	int ret = 0;

	g_dft_kthreadRunning = true;

	g_dftphasescan_ret = ddr_phase_scan();

	g_dft_kthreadRunning = false;
	kthread_stop(dft_kthread);

	return ret;
}
#endif
static ssize_t dft_phase_scan(struct kobject *kobj, struct kobj_attribute *attr,
						char *buf)
{
	int ret = 0;

	ret = ddr_phase_scan();
	if(ret)
	{
		return sprintf(buf, "TX/RX:FAIL(%d)\n", ret);
	}
	else
	{
		return sprintf(buf, "TX/RX:PASS\n");
	}
}

static ssize_t dft_phase_scan_tx_show(struct kobject *kobj, struct kobj_attribute *attr,
						char *buf)
{
	return sprintf(buf, "%s\n", OutData);
}

static ssize_t dft_phase_scan_tx_store(struct kobject *kobj, struct kobj_attribute *attr,
						const char *buf, size_t count)
{
	const char *ptr = buf;
	char *endptr;
	signed char tx_vref = TX_VREF_VAL, tx_dqspi = TX_DQSPI;
	unsigned long md_length = MD_LENGTH;
	int res;

	md_length = simple_strtol(ptr, &endptr, 16);
	ptr = endptr+1;
	tx_vref = simple_strtol(ptr, &endptr, 16);
	ptr = endptr+1;
	tx_dqspi = simple_strtol(ptr, &endptr, 16);

	rtd_pr_dft_notice("\n===Test Tx Calibration===\n");
	rtd_pr_dft_notice("md_length = %lx, tx_vref = %x, rx_te_tap = %x\n", md_length, tx_vref, tx_dqspi);
	res = tx_calibration(md_length, tx_vref, tx_dqspi);
	if(res)
	{
		rtd_pr_dft_notice("=========TX FAIL:%d=========\n", res);
		sprintf(OutData, "TX FAIL:%d\n", res);
	}
	else
	{
		sprintf(OutData, "TX PASS\n");
	}
	rtd_pr_dft_notice("\n================================\n");

	return count;
}

static ssize_t dft_phase_scan_rx_show(struct kobject *kobj, struct kobj_attribute *attr,
						char *buf)
{
	return sprintf(buf, "%s\n", OutData);
}

static ssize_t dft_phase_scan_rx_store(struct kobject *kobj, struct kobj_attribute *attr,
						const char *buf, size_t count)
{
	const char *ptr = buf;
	char *endptr;
	signed char rx_vref_tap = RX_VREF_TAP, rx_te_tap = RX_TE_TAP;
	unsigned long md_length = MD_LENGTH;
	int res;

	md_length = simple_strtol(ptr, &endptr, 16);

	ptr = endptr+1;
	rx_vref_tap = simple_strtol(ptr, &endptr, 16);

	ptr = endptr+1;
	rx_te_tap = simple_strtol(ptr, &endptr, 16);
	rtd_pr_dft_notice("\n===Test Rx Calibration===\n");
	rtd_pr_dft_notice("md_length = %lx, rx_vref_tap = %x, rx_te_tap = %x\n", md_length, rx_vref_tap, rx_te_tap);
	res = rx_calibration(md_length, rx_vref_tap, rx_te_tap);
	if(res)
	{
		rtd_pr_dft_notice("=========RX FAIL:%d=========\n", res);
		sprintf(OutData, "RX FAIL:%d\n", res);
	}
	else
	{
		sprintf(OutData, "RX PASS\n");
	}
	rtd_pr_dft_notice("\n================================\n");

	return count;
}

static ssize_t dft_phase_result(struct kobject *kobj, struct kobj_attribute *attr,
						char *buf)
{
	unsigned int tx_vref[DC_NUM][TX_VREF] = {0}, rx_vref[DC_NUM][RX_VREF] = {0};
	signed char ck[DC_NUM], dqspi[DC_NUM][BYTE_MAX];
	signed char te_p[DC_NUM][BYTE_MAX][DQM_NUM], te_n[DC_NUM][BYTE_MAX][DQM_NUM];
	int len;
	unsigned char i, j, dc;
	char str[1024] = {0};

	rtd_pr_dft_notice("\n===Show DDR Phase===\n");
	get_ddr_byte_num();
	
	read_ck_pi((signed char *)&ck);
	len = snprintf(str + strlen(str), sizeof(str) - strlen(str), "CK PI: %02x %02x\n", ck[0], ck[1]);
	if(len < 0)
		goto end_proc_str;

	get_tx_result((unsigned int *)&tx_vref, (signed char *)&dqspi);
	for(dc = 0; dc < DC_NUM; dc++)
	{
		len = snprintf(str + strlen(str), sizeof(str) - strlen(str), "DC%d Tx Vref: %d.%02d%c %d.%02d%c\nTx DQS_PI: ",
			dc, (tx_vref[dc][0]/100), (tx_vref[dc][0]%100), 0x25, (tx_vref[dc][1]/100), (tx_vref[dc][1]%100), 0x25);
		if(len < 0)
			goto end_proc_str;

		for(i = 0; i < byte_num[dc]; i++) {
			len = snprintf(str + strlen(str), sizeof(str) - strlen(str), "0x%02x ", dqspi[dc][i]);
			if(len < 0)
				goto end_proc_str;
		}

		len = snprintf(str + strlen(str), sizeof(str) - strlen(str), "\n");
		if(len < 0)
			goto end_proc_str;
	}

	get_rx_result((unsigned int *)&rx_vref, (signed char *)&te_p, (signed char *)&te_n);
	for(dc = 0; dc < DC_NUM; dc++)
	{
		len = snprintf(str + strlen(str), sizeof(str) - strlen(str), "DC%d Rx Vref: %d%c %d%c \nRx Te_p:\n",
			dc, rx_vref[dc][0], 0x25, rx_vref[dc][1], 0x25);
		if(len < 0)
			goto end_proc_str;

		for(i = 0; i < byte_num[dc]; i++)
		{
			for(j = 0; j < DQM_NUM; j++) {
				len = snprintf(str + strlen(str), sizeof(str) - strlen(str), "0x%02x ", te_p[dc][i][j]);
				if(len < 0)
					goto end_proc_str;
			}

			len = snprintf(str + strlen(str), sizeof(str) - strlen(str), "\n");
			if(len < 0)
				goto end_proc_str;
		}

		len = snprintf(str + strlen(str), sizeof(str) - strlen(str), "Rx Te_n:\n");
		if(len < 0)
			goto end_proc_str;

		for(i = 0; i < byte_num[dc]; i++)
		{
			for(j = 0; j < DQM_NUM; j++) {
				len = snprintf(str + strlen(str), sizeof(str) - strlen(str), "0x%02x ", te_n[dc][i][j]);
				if(len < 0)
					goto end_proc_str;
			}

			len = snprintf(str + strlen(str), sizeof(str) - strlen(str), "\n");
			if(len < 0)
				goto end_proc_str;
		}
	}

	rtd_pr_dft_notice("\n=====================\n");

end_proc_str:

	memcpy(buf, str, strlen(str) + 1); // +1 for null character '\0'

	return strlen(buf);
}

static ssize_t dft_phase_set(struct kobject *kobj, struct kobj_attribute *attr,
						const char *buf, size_t count)
{
	const char *ptr = buf;
	char *endptr;
	signed char ck = 0, tx_vref = 0, tx_dqspi = 0, rx_vref = 0, rx_te_tap = 0;

	ck = simple_strtol(ptr, &endptr, 16);
	ptr = endptr+1;
	tx_vref = simple_strtol(ptr, &endptr, 16);
	ptr = endptr+1;
	tx_dqspi = simple_strtol(ptr, &endptr, 16);
	ptr = endptr+1;
	rx_vref = simple_strtol(ptr, &endptr, 16);
	ptr = endptr+1;
	rx_te_tap = simple_strtol(ptr, &endptr, 16);

	rtd_pr_dft_notice("\n===Set DDR Phase===\n");
	rtd_pr_dft_notice("ck = %d, tx_vref = %d, tx_dqspi = %d, rx_verf = %d, rx_te_tap = %d\n", ck, tx_vref, tx_dqspi, rx_vref, rx_te_tap);
	ddr_phase_scan_set(ck, tx_vref, tx_dqspi, rx_vref, rx_te_tap);
	rtd_pr_dft_notice("\n================================\n");

	return count;
}

static ssize_t dft_phase_scan_test_store(struct kobject *kobj, struct kobj_attribute *attr,
						const char *buf, size_t count)
{
	const char *ptr = buf;
	char *endptr;
	unsigned long md_length = MD_LENGTH, md_addr;
	void * md_vir;

	md_length = simple_strtol(ptr, &endptr, 16);

	rtd_pr_dft_notice("\n===Phase Scan Test===\n");
	md_vir = dvr_malloc(md_length*2);
	md_addr = dvr_to_phys(md_vir);
	rtd_pr_dft_notice("md_addr = %lx, md_length = %lx\n", md_length, md_addr);
	if(md_addr == INVALID_VAL)
	{
		rtd_pr_dft_info("md malloc fail\n");
	}
	else
	{
		if(MD_rw((unsigned int)md_addr, md_length)) {
			sprintf(OutData, "MD FAIL: addr:%lx, len %lx\n", md_addr, md_length);
		}
		else {
			sprintf(OutData, "MD PASS\n");
		}
	}

	dvr_free(md_vir);
	rtd_pr_dft_notice("\n================================\n");

	return count;
}

static ssize_t dft_phase_scan_test_show(struct kobject *kobj, struct kobj_attribute *attr,
						char *buf)
{
	return sprintf(buf, "%s\n", OutData);
}


static struct kobj_attribute dft_phase_scan_attribute =
	__ATTR(phase_scan, 0644, dft_phase_scan, NULL);
static struct kobj_attribute dft_phase_scan_tx_attribute =
	__ATTR(phase_scan_tx, 0644, dft_phase_scan_tx_show, dft_phase_scan_tx_store);
static struct kobj_attribute dft_phase_scan_rx_attribute =
	__ATTR(phase_scan_rx, 0644, dft_phase_scan_rx_show, dft_phase_scan_rx_store);
static struct kobj_attribute dft_phase_result_attribute =
	__ATTR(phase_result, 0644, dft_phase_result, NULL);
static struct kobj_attribute dft_phase_set_attribute =
	__ATTR(phase_set, 0644, NULL, dft_phase_set);
static struct kobj_attribute dft_phase_scan_test_attribute =
	__ATTR(phase_scan_test, 0644, dft_phase_scan_test_show, dft_phase_scan_test_store);

static struct attribute *attrs[] = {
	&dft_phase_scan_attribute.attr,
	&dft_phase_scan_tx_attribute.attr,
	&dft_phase_scan_rx_attribute.attr,
	&dft_phase_result_attribute.attr,
	&dft_phase_set_attribute.attr,
	&dft_phase_scan_test_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

/* DFT char device file operation session */
long dft_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long retval = 0;
	/* no user space ioctl  */

	switch (cmd) {
		case RTKDFT_IOC_PHASE_SCAN: {
			if(ddr_phase_scan())
				retval = -1;

			break;
		}
		default:
			rtd_pr_dft_err("DFT : wrong ioctl cmd\n");
			retval = -ENOTTY;
	}

	return retval;
}

#ifdef CONFIG_COMPAT
long dft_ioctl_compat(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long retval = 0;
	int scan_ret = 0;
	/* no user space ioctl  */

	switch (cmd) {
		case RTKDFT_IOC_PHASE_SCAN: {
			if (g_dft_kthreadRunning == false){
				dft_kthread = kthread_run(ddr_phase_scan_thread, NULL, "rtkdtf_thread");
				if (!IS_ERR(dft_kthread)) {
					retval= 0; //create ddr_phase_scan success
				}
				else
					retval = -1;
			}
			break;
		}
		case RTKDFT_IOC_GET_PHASE_SCAN_INFO: {
			if((g_dft_kthreadRunning == true) && dft_kthread){
				scan_ret = 2; //busy
			}
			else if(g_dft_kthreadRunning == false){
				if(g_dftphasescan_ret == 0)
					scan_ret = 0; //success;
				else
					scan_ret = 1; //fail
			}

			if(copy_to_user((void __user *)arg, (void *)&scan_ret, sizeof(int)))
			{
				retval = -EFAULT;
				break;
			}
			retval= 0;
			break;
		}
		case RTKDFT_IOC_AUTO_PHASE_SCAN: {
			if(ddr_phase_scan())
				retval = -1;

			break;
		}
		default:
			rtd_pr_dft_err("DFT : wrong ioctl cmd\n");
			retval = -ENOTTY;
	}

	return retval;
}
#endif

int dft_open(struct inode *inode, struct file *filp)
{
	return 0;
}

int dft_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/* DFT file operation */
struct file_operations dft_fops = {
read:
	NULL,
unlocked_ioctl:
	dft_ioctl,
#ifdef CONFIG_COMPAT
compat_ioctl:
	dft_ioctl_compat,
#endif
open:
	dft_open,
release:
	dft_release,
};

/* set /dev/dft mode to 0666  */
static char *dft_devnode(struct device *dev, umode_t *mode)
{
	if (mode)
		*mode = 0666;
	return NULL;
}

#ifdef CONFIG_PM
static int dft_suspend(struct platform_device *dev, pm_message_t state)
{
	rtd_pr_dft_debug("dft_suspend\n");
	return 0;
}
static int dft_resume(struct platform_device *dev)
{
	rtd_pr_dft_debug("dft_resume\n");
	return 0;
}
#endif

static struct platform_driver dft_platform_driver = {
#ifdef CONFIG_PM
	.suspend    = dft_suspend,
	.resume     = dft_resume,
#endif
	. driver = {
		.name       = "dft",
		.bus        = &platform_bus_type,
	} ,
} ;


static int __init dft_module_init(void)
{
	int retval;

	dft_kobj = kobject_create_and_add("dft", NULL);
	if (!dft_kobj)
		return -ENOMEM;

	retval = sysfs_create_group(dft_kobj, &attr_group);
	if (retval)
		kobject_put(dft_kobj);

	/* register device to get major and minor number */
	if (dft_major) {
		dft_devnum = MKDEV(dft_major, dft_minor);
		retval = register_chrdev_region(dft_devnum, dft_nr_devs, "dft");
	} else {
		retval = alloc_chrdev_region(&dft_devnum, dft_minor, dft_nr_devs, "dft");
		dft_major = MAJOR(dft_devnum);
	}
	if (retval < 0) {
		rtd_pr_dft_warn("DFT : can't get major %d\n", dft_major);
		return retval;
	}

	/* create device node by udev API */
	dft_class = class_create(THIS_MODULE, "dft");
	if (IS_ERR(dft_class)) {
		return PTR_ERR(dft_class);
	}
	dft_class->devnode = dft_devnode;
	dft_device = device_create(dft_class, NULL, dft_devnum, NULL, "dft");
	rtd_pr_dft_info("DFT module init, major number = %d, device name = %s \n", dft_major, dev_name(dft_device));

	/* cdev API to register file operation */
	cdev_init(&dft_cdev, &dft_fops);
	dft_cdev.owner = THIS_MODULE;
	dft_cdev.ops = &dft_fops;
	retval = cdev_add(&dft_cdev, dft_devnum , 1);
	if (retval) {
		rtd_pr_dft_err("Error %d adding char_reg_setup_cdev", retval);
	}

	dft_platform_devs = platform_device_register_simple("dft", -1, NULL, 0);
	/* rtd_pr_dft_info("DFT platform device name %s\n", dev_name(&dft_platform_devs->dev)); */

	if (platform_driver_register(&dft_platform_driver) != 0) {
		platform_device_unregister(dft_platform_devs);
		dft_platform_devs = NULL;
		rtd_pr_dft_warn("dft platform driver register fail\n");
		return retval;
	}

	spin_lock_init(&dft_lock);

	return retval;
}

static void __exit dft_module_exit(void)
{
	if (dft_platform_devs)
		platform_device_unregister(dft_platform_devs);

	platform_driver_unregister(&dft_platform_driver);

	kobject_put(dft_kobj);
	device_destroy(dft_class, dft_devnum);
	class_destroy(dft_class);
	unregister_chrdev_region(dft_devnum, dft_nr_devs);

}

module_init(dft_module_init);
module_exit(dft_module_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("realtek.com");
