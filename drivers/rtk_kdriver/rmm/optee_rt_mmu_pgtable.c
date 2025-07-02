// SPDX-License-Identifier: GPL-2.0-only
/*
 * page table command for RTK architected implementations
 *
 * Copyright (c) 2023 RealTek Inc.
 * Author: davidwang <davidwang@realtek.com>
 *
 */

#define pr_fmt(fmt)	"rt_mmu_pgtable: " fmt

#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/tee.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/tee_drv.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <linux/workqueue.h>
#include <mach/rtk_platform.h>
#include <linux/gfp.h>
#include <asm/cacheflush.h>

#include <rtd_log/rtd_module_log.h>
#include <rmm/rtk-iommu.h>
#include <rmm/pageremap.h>
#include <rmm/optee_rt_mmu_pgtable.h>
#include <md/rtk_md.h>

#include <rtk_kdriver/io.h>

#include <rbus/rt_mmu_reg.h>
#ifdef CONFIG_REALTEK_IOMMU_VERIFY
#include "optee_rt_mmu_pgtable_test_pattern.c"
#endif

static const uuid_t rt_mmu_uuid =
	UUID_INIT(0xfa704667, 0xb9bb, 0x4697,
			  0x9a, 0x1c, 0x60, 0x77, 0x15, 0xd2, 0x86, 0xbd);

//#define RT_MMU_PGTABLE_CMD_GET_ENTRY	  100
//#define RT_MMU_PGTABLE_CMD_SET_ENTRY		200
#define RT_MMU_PGTABLE_CMD_GET_ENTRY_RBUS		100
#define RT_MMU_PGTABLE_CMD_SET_ENTRY_RBUS		200
#define RT_MMU_PGTABLE_CMD_SET_ENTRY_DMA		201
#define RT_MMU_PGTABLE_CMD_UNSET_ENTRY		300
#define RT_MMU_PGTABLE_CMD_TESTCASE  400
#define RT_MMU_PGTABLE_CMD_GET_MODULE_INFO  10
#define RT_MMU_PGTABLE_CMD_DEBUG  1000

#define RT_MMU_TEE_NUM_PARAM 4

#ifdef CONFIG_REALTEK_IOMMU_VERIFY
static VO_TIMING VODMA_StdTimingTable[13] =
{
	// 525i60, 13.5Mhz pixel clock
	{
		858,			// h total
		525, //262,	// v total
		720,			// h active,704
		240,			// v active
		69, //70,		// h start
		16,			// v start
		0,			// interlace
		2703,		// sample rate in 10000Hz
		60000		//frame rate
	},

	// 525p60, 27Mhz pixel clock
	{
		858,			// h total
		525,			// v total
		720,			// h active,704
		480,			// v active
		70,			// h start
		16,			// v start
		1,			// progressive
		2703,		// sample rate in 10000Hz
		60000		//frame rate
	},

	// 625i50, 13.5Mhz pixel clock
	{
		864,			// h total
		625,			// v total
		720,			// h active,704
		288,			// v active
		72,			// h start
		16,			// v start
		0,			// interlace
		2700,		// sample rate in 10000Hz
		50000		//frame rate
	},

	// 625p50, 27Mhz pixel clock
	{
		864,			// h total
		625,			// v total
		720,			// h active,704
		576,			// v active
		72,			// h start
		16,			// v start
		1,			// progressive
		2700,		// sample rate in 10000Hz
		50000		//frame rate
	},

	// 720p50, 74.25Mhz pixel clock
	{
		1980,		// h total
		750,			// v total
		1280,		// h active
		720,			// v active
		350,			// h start
		16,			// v start
		1,			// progressive
		7425,		// sample rate in 10000Hz
		50000		//frame rate
	},

	// 720p60, 74.25Mhz pixel clock
	{
		1650,		// h total
		750,			// v total
		1280,		// h active
		720,			// v active
		186,			// h start
		16,			// v start
		1,			// progressive
		7425,		// sample rate in 10000Hz
		60000		//frame rate
	},

	// 1080i50, 74.25Mhz pixel clock
	{
		2200,		// h total, temporally test by jeff, original is 2640
		1125,		// v total
		1920,		// h active
		540,			// v active
		140,			// h start
		16,			// v start
		0,			// interlace
		14850,		// sample rate in 10000Hz
		50000		//frame rate
	},

	// 1080i60, 74.25Mhz pixel clock
	{
		2200,		// h total
		1125,			// v total
		1920,		// h active
		540,			// v active
		140,			// h start
		16,			// v start
		0,			// interlace
		14850,		// sample rate in 10000Hz
		60000		//frame rate
	},

	// 1080p50, 148.5Mhz pixel clock
	{
		2200,		// h total, temporally test by jeff, original is 2640
		1125,		// v total
		1920,		// h active
		1080,		// v active
		140,			// h start
		16,			// v start
		1,			// progressive
		14850,		// sample rate in 10000Hz
		50000		//frame rate
	},

	// 1080p60, 148.5Mhz pixel clock
	{
		2200,		// h total
		1125,		// v total
		1920,		// h active
		1080,		// v active
		140,			// h start
		16,			// v start
		1,			// progressive
		14850,		// sample rate in 10000Hz
		60000		//frame rate
	},

	//sxga
	{
		1688,		// h total
		1066,		// v total
		1280,		// h active,704
		1024,		// v active
		140,//404,			// h start
		16,//35,			// v start
		1,			// progressive
		10800,		// sample rate in 10000Hz
		59900			//frame rate
	},

    // 2160p30, 270Mhz pixel clock
    {
        4120,       // h total
        2192,       // v total
        3840,       // h active
        2160,       // v active
        140,            // h start
        16,         // v start
        1,          // progressive
        27100,      // sample rate in 10000Hz
        30000       //frame rate
    },

    // 2160p60, 540Mhz pixel clock
    {
        4120,       // h total
        2192,       // v total
        3840,       // h active
        2160,       // v active
        140,            // h start
        16,         // v start
        1,          // progressive
        54200,      // sample rate in 10000Hz
        60000       //frame rate
    },
} ;
#endif

/* for sysfs global */
unsigned int get_entry_compare;
unsigned long g_iova, g_size;
void *g_cpu_addr = NULL;
unsigned long set_entry_length = 0;
unsigned int set_entry_rbus = 0;

#ifdef CONFIG_REALTEK_IOMMU_VERIFY
unsigned int md_size_iova_free = 0;
VODMA_TIMING_MODE g_timing = TIMING_MODE_1080I50;
unsigned int g_frame_keep_count = 0;
static struct workqueue_struct *frame_keep_md_access_workq;
static struct delayed_work delayed_frame_keep_work;
int frame_keep_md_access_workq_delay = 500; // workqueue thread polling interval
#endif
static unsigned int test_pgtable[NUM_PT_ENTRIES];

struct optee_ta {
    struct tee_context *ctx;
    __u32 session;
};

struct rt_mmu_pgtable_shm {
	struct tee_shm *shm_buffer;
	int shm_len;
	phys_addr_t shm_pa;
	unsigned long *shm_va;
}; 

struct rt_mmu_cmd_param {
	int command;
	unsigned int in_out_attr[RT_MMU_TEE_NUM_PARAM];
	unsigned long in_out_val_a[RT_MMU_TEE_NUM_PARAM];
	unsigned long in_out_val_b[RT_MMU_TEE_NUM_PARAM];

#define MAX_TEE_NUM_SHM_PARAM 1
	int mem_size;
	struct rt_mmu_pgtable_shm shm[MAX_TEE_NUM_SHM_PARAM];
};

static struct optee_ta rt_mmu_pgtable_ta;

static unsigned int __maybe_unused swap_endian(unsigned int input)
{
	unsigned int output;

	output = (input & 0xff000000) >> 24 |
			(input & 0x00ff0000) >> 8 |
			(input & 0x0000ff00) << 8 |
			(input & 0x000000ff) << 24;

	return output;
}

static int rt_mmu_pgtable_match(struct tee_ioctl_version_data *data, const void *vers)
{
	return 1;
}

int __maybe_unused rt_mmu_pgtable_init(void)
{
	int ret = 0, rc = 0;
	const uuid_t *uuid_in = &rt_mmu_uuid;
	struct tee_param *param = NULL;
	struct tee_ioctl_open_session_arg arg;
	struct tee_ioctl_version_data vers = {
		.impl_id = TEE_IMPL_ID_OPTEE,
		.impl_caps = TEE_OPTEE_CAP_TZ,
		.gen_caps = TEE_GEN_CAP_GP,
	};

	memset(&rt_mmu_pgtable_ta, 0, sizeof(rt_mmu_pgtable_ta));
	rt_mmu_pgtable_ta.ctx = tee_client_open_context(NULL, rt_mmu_pgtable_match, NULL, &vers);
	if(rt_mmu_pgtable_ta.ctx == NULL) {
		rtd_pr_rmm_err("%s: no ta context\n", __func__);
		ret = -EINVAL;
		goto err;
	}

	memset(&arg, 0, sizeof(arg));
	memcpy(&arg.uuid, uuid_in->b, TEE_IOCTL_UUID_LEN);
	arg.clnt_login = TEE_IOCTL_LOGIN_PUBLIC;
	arg.num_params = RT_MMU_TEE_NUM_PARAM;
	rtd_pr_rmm_info("arg uuid %pUl \n", arg.uuid);

	param = kcalloc(RT_MMU_TEE_NUM_PARAM, sizeof(struct tee_param), GFP_KERNEL);
	if(!param)
	{
		rtd_pr_rmm_err("%s: no mem param\n", __func__);
		ret = -ENOMEM;
		goto err;
	}

	memset(param, 0, sizeof(struct tee_param) * RT_MMU_TEE_NUM_PARAM);
	param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
	param[0].u.value.a = 0;
	param[0].u.value.b = 0;
	param[1].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
	param[1].u.value.a = 0;
	param[1].u.value.b = 0;
	param[2].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_OUTPUT;
	param[2].u.value.a = 0;
	param[2].u.value.b = 0;
	param[3].attr = TEE_IOCTL_PARAM_ATTR_TYPE_NONE;
	rc = tee_client_open_session(rt_mmu_pgtable_ta.ctx, &arg, param);
	if(rc){
		rtd_pr_rmm_err("open_session failed ret %x arg %x", rc, arg.ret);
		ret = -EINVAL;
		goto err;
	}
	if (arg.ret) {
		ret = -EINVAL;
		goto err;
	}

	rt_mmu_pgtable_ta.session = arg.session;

	if(param)
		kfree(param);

	return 0;
err:
	if (rt_mmu_pgtable_ta.session) {
		tee_client_close_session(rt_mmu_pgtable_ta.ctx, rt_mmu_pgtable_ta.session);
		rtd_pr_rmm_err("open failed close session \n");
		rt_mmu_pgtable_ta.session = 0;
	}
	if (rt_mmu_pgtable_ta.ctx) {
		tee_client_close_context(rt_mmu_pgtable_ta.ctx);
		rtd_pr_rmm_err("open failed close context\n");
		rt_mmu_pgtable_ta.ctx = NULL;
	}
	if(param)
		kfree(param);

	rtd_pr_rmm_err("open_session fail\n");

	return ret;

}
void __maybe_unused rt_mmu_pgtable_deinit(void)
{
	if (rt_mmu_pgtable_ta.session) {
		tee_client_close_session(rt_mmu_pgtable_ta.ctx, rt_mmu_pgtable_ta.session);
		rt_mmu_pgtable_ta.session = 0;
	}

	if (rt_mmu_pgtable_ta.ctx) {
		tee_client_close_context(rt_mmu_pgtable_ta.ctx);
		rt_mmu_pgtable_ta.ctx = NULL;
	}
}

static int rt_mmu_pgtable_tee_shm_prepare(struct tee_context *ctx, struct rt_mmu_pgtable_shm *data, int size)
{
	int ret = 0, rc = 0;

	data->shm_len = size;

	// alloc share memory
	data->shm_buffer = tee_shm_alloc(ctx, data->shm_len, TEE_SHM_MAPPED | TEE_SHM_DMA_BUF);
	if (data->shm_buffer == NULL) {
        rtd_pr_rmm_err("%s: no shm_buffer\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	// get share memory virtual addr for data accessing
	data->shm_va = tee_shm_get_va(data->shm_buffer, 0);
	if (data->shm_va == NULL) {
		ret = -ENOMEM;
		goto out;
	}

	// get share memory physial addr for tee param
	rc = tee_shm_get_pa(data->shm_buffer, 0, &data->shm_pa);
	if (rc) {
		ret = -ENOMEM;
		goto out;
	}

	rtd_pr_rmm_info("shm_buffer=%lx, shm_len=%d, va=%lx, pa=%x\n", (unsigned long)data->shm_buffer, 
			data->shm_len, (unsigned long)data->shm_va, (unsigned int)data->shm_pa);

	return 0;

out:
	if (data->shm_buffer)
		tee_shm_free(data->shm_buffer);

	return ret;
}

static inline void rt_mmu_pgtable_tee_shm_release(struct tee_context *ctx, struct rt_mmu_pgtable_shm *data)
{
	if (data->shm_buffer)
		tee_shm_free(data->shm_buffer);

	data->shm_buffer = NULL;
	data->shm_len = 0;
	data->shm_pa = 0;
	data->shm_va = NULL;
}

int rt_mmu_pgtable_cmd_op(struct rt_mmu_cmd_param *cmd_param)
{
	int ret = 0, rc = 0;
	__u32 ta_session = rt_mmu_pgtable_ta.session;
	struct tee_ioctl_invoke_arg arg;
	struct tee_param *param = NULL;
	struct rt_mmu_pgtable_shm *shm = cmd_param->shm;
	int i = 0;
	int has_val_output = 0;

	param = kcalloc(RT_MMU_TEE_NUM_PARAM, sizeof(struct tee_param), GFP_KERNEL);
	if(!param)
	{
		rtd_pr_rmm_err("%s: no mem param\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	memset(&arg, 0, sizeof(arg));
	arg.func = cmd_param->command;
	arg.session = ta_session;
	arg.num_params = RT_MMU_TEE_NUM_PARAM;

	memset(param, 0, sizeof(struct tee_param) * RT_MMU_TEE_NUM_PARAM);
	for (i = 0; i < RT_MMU_TEE_NUM_PARAM; i++) {
		param[i].attr = cmd_param->in_out_attr[i];
		if (param[i].attr == TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_INPUT || param[i].attr == TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_OUTPUT) {
			param[i].u.memref.shm = shm->shm_buffer;
			param[i].u.memref.size = shm->shm_len;
			rtd_pr_rmm_info("param[%d], memref, shm=%lx, size=%lx\n", i, 
					(unsigned long)param[i].u.memref.shm, (unsigned long)param[i].u.memref.size);
		} else if (param[i].attr == TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT) {
			param[i].u.value.a = cmd_param->in_out_val_a[i];
			param[i].u.value.b = cmd_param->in_out_val_b[i];
			rtd_pr_rmm_info("param[%d], in_val, a=%lx, b=%lx\n", i, (unsigned long)param[i].u.value.a,
					(unsigned long)param[i].u.value.b);
		} else if (param[i].attr == TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_OUTPUT) {
			has_val_output = 1;			
		}
	}
	rc = tee_client_invoke_func(rt_mmu_pgtable_ta.ctx, &arg, param);
	if (rc || arg.ret) {
		rtd_pr_rmm_err("%s: invoke failed ret %x arg.ret %x\n", __func__, rc, arg.ret);
		ret = -EINVAL;
		goto out;
	}

	if (has_val_output) {
		for (i = 0; i < RT_MMU_TEE_NUM_PARAM; i++) {
			if (param[i].attr == TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_OUTPUT) {
				cmd_param->in_out_val_a[i] = param[i].u.value.a;
				cmd_param->in_out_val_b[i] = param[i].u.value.b;
				rtd_pr_rmm_info("param[%d], out_val, a=%lx, b=%lx\n", i, (unsigned long)param[i].u.value.a,
						(unsigned long)param[i].u.value.b);
			}
		}
	}

out:
	if (param)
		kfree(param);

	if (ret)
		return ret;
	else
		return 0;
}

/* get table from lowlevel */
static int __maybe_unused rt_mmu_pgtable_init_modules (void)
{
	int ret = 0;
	struct tee_context *ta_ctx = rt_mmu_pgtable_ta.ctx;

	struct rt_mmu_cmd_param _cmd_param = {
		.command = RT_MMU_PGTABLE_CMD_GET_MODULE_INFO,
		.in_out_attr = {TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_OUTPUT,
						TEE_IOCTL_PARAM_ATTR_TYPE_NONE,
						TEE_IOCTL_PARAM_ATTR_TYPE_NONE,
						TEE_IOCTL_PARAM_ATTR_TYPE_NONE},
		.in_out_val_a = {0, 0, 0, 0},
		.in_out_val_b = {0, 0, 0, 0},
		.mem_size = (sizeof(unsigned int) * NUM_PT_ENTRIES), /* entry */
		.shm = {{0}}
	};
	struct rt_mmu_cmd_param *cmd_param = &_cmd_param;
	struct rt_mmu_pgtable_shm *shm = cmd_param->shm;

	if(ta_ctx == NULL) {
		rtd_pr_rmm_err("%s: no ta context\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	if (cmd_param->mem_size) {
		ret = rt_mmu_pgtable_tee_shm_prepare(ta_ctx, shm, cmd_param->mem_size);
		if (ret) {
			rtd_pr_rmm_err("%s: shm_prepare fail\n", __func__);
			goto out;
		}
	}

	ret = rt_mmu_pgtable_cmd_op(cmd_param);
	if (ret) {
		rtd_pr_rmm_err("%s: cmd %d err, ret=%x\n", __func__, cmd_param->command, ret);
		goto out;
	}

	if(likely(test_pgtable != NULL) && cmd_param->mem_size) {
		rtd_pr_rmm_info("%s: dst=%lx, src=%lx, size=%x\n", __func__, (unsigned long)test_pgtable, (unsigned long)shm->shm_va, (unsigned int)shm->shm_len);
		memcpy((char *)test_pgtable, (char *)shm->shm_va, shm->shm_len);
	} else 
		rtd_pr_rmm_err(" no mem pa_array\n");

out:
	if (cmd_param->mem_size)
		rt_mmu_pgtable_tee_shm_release(ta_ctx, shm);

	return ret;
}

int __maybe_unused rt_mmu_pgtable_get_entries (unsigned long iova, unsigned long size, unsigned int *pgtable)
{
	int ret = 0;
	struct tee_context *ta_ctx = rt_mmu_pgtable_ta.ctx;

	struct rt_mmu_cmd_param _cmd_param = {
		.command = RT_MMU_PGTABLE_CMD_GET_ENTRY_RBUS,
		.in_out_attr = {TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT,
						TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_OUTPUT,
						TEE_IOCTL_PARAM_ATTR_TYPE_NONE,
						TEE_IOCTL_PARAM_ATTR_TYPE_NONE},
		.in_out_val_a = {0, 0, 0, 0},
		.in_out_val_b = {0, 0, 0, 0},
		.mem_size = 0,
		.shm = {{0}}
	};
	struct rt_mmu_cmd_param *cmd_param = &_cmd_param;
	struct rt_mmu_pgtable_shm *shm = cmd_param->shm;

	cmd_param->in_out_val_a[0] = iova;
	cmd_param->in_out_val_b[0] = size;
	cmd_param->mem_size = sizeof(unsigned int) * (size >> SPAGE_ORDER);

	rtd_pr_rmm_info("%s: iova_base=%lx, size=%lx\n", __func__, iova, size);

	if(ta_ctx == NULL) {
		rtd_pr_rmm_err("%s: no ta context\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	if (cmd_param->mem_size) {
		ret = rt_mmu_pgtable_tee_shm_prepare(ta_ctx, shm, cmd_param->mem_size);
		if (ret) {
			rtd_pr_rmm_err("%s: shm_prepare fail\n", __func__);
			goto out;
		}
	}

	ret = rt_mmu_pgtable_cmd_op(cmd_param);
	if (ret) {
		rtd_pr_rmm_err("%s: cmd %d err, ret=%x\n", __func__, cmd_param->command, ret);
		goto out;
	}

	if (pgtable != NULL && cmd_param->mem_size) {
		rtd_pr_rmm_info("%s: dst=%lx, src=%lx, size=%x\n", __func__, (unsigned long)pgtable, (unsigned long)shm->shm_va, (unsigned int)shm->shm_len);
		memcpy((char *)pgtable, (char *)shm->shm_va, shm->shm_len);
	} else 
		rtd_pr_rmm_err(" no mem pgtable\n");

out:
	if (cmd_param->mem_size)
		rt_mmu_pgtable_tee_shm_release(ta_ctx, shm);

	return ret;
}

int __maybe_unused rt_mmu_pgtable_get_entries_and_compare(unsigned long iova, unsigned long size, unsigned int *pgtable, int *cmp_result)
{
	int ret = 0;
	struct tee_context *ta_ctx = rt_mmu_pgtable_ta.ctx;

	struct rt_mmu_cmd_param _cmd_param = {
		.command = RT_MMU_PGTABLE_CMD_DEBUG,
		.in_out_attr = {TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT,
						TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_OUTPUT,
						TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_OUTPUT,
						TEE_IOCTL_PARAM_ATTR_TYPE_NONE},
		.in_out_val_a = {0, 0, 0, 0},
		.in_out_val_b = {0, 0, 0, 0},
		.mem_size = 0,
		.shm = {{0}}
	};
	struct rt_mmu_cmd_param *cmd_param = &_cmd_param;
	struct rt_mmu_pgtable_shm *shm = cmd_param->shm;

	if(ta_ctx == NULL) {
		rtd_pr_rmm_err("%s: no ta context\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	cmd_param->in_out_val_a[0] = iova;
	cmd_param->in_out_val_b[0] = size;
	cmd_param->mem_size = sizeof(unsigned int) * NUM_PT_ENTRIES;

	rtd_pr_rmm_info("%s: iova_base=%lx, size=%lx\n", __func__, iova, size);

	if (cmd_param->mem_size) {
		ret = rt_mmu_pgtable_tee_shm_prepare(ta_ctx, shm, cmd_param->mem_size);
		if (ret) {
			rtd_pr_rmm_err("%s: shm_prepare fail\n", __func__);
			goto out;
		}
	}

	ret = rt_mmu_pgtable_cmd_op(cmd_param);
	if (ret) {
		rtd_pr_rmm_err("%s: cmd %d err, ret=%x\n", __func__, cmd_param->command, ret);
		goto out;
	}

	if (pgtable != NULL && cmd_param->mem_size) {
		rtd_pr_rmm_info("%s: dst=%lx, src=%lx, size=%x\n", __func__, (unsigned long)pgtable, (unsigned long)shm->shm_va, (unsigned int)shm->shm_len);
		memcpy((char *)pgtable, (char *)shm->shm_va, shm->shm_len);
	} else 
		rtd_pr_rmm_err(" no mem pgtable\n");

	rtd_pr_rmm_info("%s: compare result=%ld\n", __func__, cmd_param->in_out_val_a[2]);
	if (cmp_result)
		*cmp_result = cmd_param->in_out_val_a[2];

out:
	if (cmd_param->mem_size)
		rt_mmu_pgtable_tee_shm_release(ta_ctx, shm);

	return ret;
}

int __maybe_unused rt_mmu_pgtable_set_entries (unsigned long iova, unsigned long size,
											   unsigned int dma, unsigned int *pgtable)
{
	int ret = 0;
	struct tee_context *ta_ctx = rt_mmu_pgtable_ta.ctx;

	struct rt_mmu_cmd_param _cmd_param = {
		.command = RT_MMU_PGTABLE_CMD_SET_ENTRY_RBUS,
		.in_out_attr = {TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT,
						TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_INPUT,
						TEE_IOCTL_PARAM_ATTR_TYPE_NONE,
						TEE_IOCTL_PARAM_ATTR_TYPE_NONE},
		.in_out_val_a = {0, 0, 0, 0},
		.in_out_val_b = {0, 0, 0, 0},
		.mem_size = 0,
		.shm = {{0}}
	};
	struct rt_mmu_cmd_param *cmd_param = &_cmd_param;
	struct rt_mmu_pgtable_shm *shm = cmd_param->shm;

	cmd_param->in_out_val_a[0] = iova;
	cmd_param->in_out_val_b[0] = size;
	cmd_param->mem_size = sizeof(unsigned int) * (size >> SPAGE_ORDER);
	if (dma)
		cmd_param->command = RT_MMU_PGTABLE_CMD_SET_ENTRY_DMA;

	rtd_pr_rmm_info("%s: iova_base=%lx, size=%lx, pgtable=%lx\n", __func__, iova, size, (unsigned long)pgtable);

	if(ta_ctx == NULL) {
		rtd_pr_rmm_err("%s: no ta context\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	if (cmd_param->mem_size) {
		ret = rt_mmu_pgtable_tee_shm_prepare(ta_ctx, shm, cmd_param->mem_size);
		if (ret) {
			rtd_pr_rmm_err("%s: shm_prepare fail\n", __func__);
			goto out;
		}
	}

	if (pgtable != NULL && cmd_param->mem_size) {
		rtd_pr_rmm_info("%s: dst=%lx, src=%lx, size=%x\n", __func__, (unsigned long)shm->shm_va, (unsigned long)pgtable, (unsigned int)shm->shm_len);
		memcpy((char *)shm->shm_va, (char *)pgtable, shm->shm_len);
	} else {
		rtd_pr_rmm_err(" no mem pgtable\n");
		ret = -EINVAL;
		goto out;
	}

	ret = rt_mmu_pgtable_cmd_op(cmd_param);
	if (ret) {
		rtd_pr_rmm_err("%s: cmd %d err, ret=%x\n", __func__, cmd_param->command, ret);
		goto out;
	}

out:
	if (cmd_param->mem_size)
		rt_mmu_pgtable_tee_shm_release(ta_ctx, shm);

	return ret;
}

int __maybe_unused rt_mmu_pgtable_unset_entries (int module_id, unsigned int *pa_array)
{
	int ret = 0;
	struct tee_context *ta_ctx = rt_mmu_pgtable_ta.ctx;

	struct rt_mmu_cmd_param _cmd_param = {
		.command = RT_MMU_PGTABLE_CMD_UNSET_ENTRY,
		.in_out_attr = {TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT,
						TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_OUTPUT,
						TEE_IOCTL_PARAM_ATTR_TYPE_NONE,
						TEE_IOCTL_PARAM_ATTR_TYPE_NONE},
		.in_out_val_a = {1, 0, 0, 0},
		.in_out_val_b = {0, 0, 0, 0},
		.mem_size = (sizeof(unsigned int) * NUM_PT_ENTRIES), /* entry */
		.shm = {{0}}
	};
	struct rt_mmu_cmd_param *cmd_param = &_cmd_param;
	struct rt_mmu_pgtable_shm *shm = cmd_param->shm;

	if(ta_ctx == NULL) {
		rtd_pr_rmm_err("%s: no ta context\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	if (cmd_param->mem_size) {
		ret = rt_mmu_pgtable_tee_shm_prepare(ta_ctx, shm, cmd_param->mem_size);
		if (ret) {
			rtd_pr_rmm_err("%s: shm_prepare fail\n", __func__);
			goto out;
		}
	}

	ret = rt_mmu_pgtable_cmd_op(cmd_param);
	if (ret) {
		rtd_pr_rmm_err("%s: cmd %d err, ret=%x\n", __func__, cmd_param->command, ret);
		goto out;
	}

	if(pa_array != NULL && cmd_param->mem_size)
		memcpy((char *)pa_array, (char *)shm->shm_va, shm->shm_len);
	else 
		rtd_pr_rmm_err(" no mem pa_array\n");

out:
	if (cmd_param->mem_size)
		rt_mmu_pgtable_tee_shm_release(ta_ctx, shm);

	return ret;
}

static ssize_t rt_mmu_open_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	unsigned long size = 0;
	int ret = 0;
	ret = rt_mmu_pgtable_init();
    return sprintf(buf, "%llu\n", (u64)size);
}
static ssize_t rt_mmu_close_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	unsigned long size = 0;
	rt_mmu_pgtable_deinit();
    return sprintf(buf, "%llu\n", (u64)size);
}

static ssize_t rt_mmu_get_entry_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	const char *ptr = buf;
	char *endptr;

	get_entry_compare = (unsigned int)simple_strtol(ptr, &endptr, 10);
	ptr = endptr+1;

	return count;
}

static ssize_t rt_mmu_get_entry_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int result = 0;
	unsigned long size = 0;
	int ret = 0;
//	unsigned int *pgtable;
//	pgtable = rtk_iommu_iova_table_offset(g_iova, test_pgtable);

	unsigned long free_kbytes_1, free_kbytes_2;

	if (get_entry_compare)
		ret = rt_mmu_pgtable_get_entries_and_compare(g_iova, g_size, test_pgtable, &result);
	else 
		ret = rt_mmu_pgtable_get_entries(g_iova, g_size, test_pgtable);

	free_kbytes_1 = global_zone_page_state(NR_FREE_PAGES) << (PAGE_SHIFT - 10);
	rtd_pr_rmm_info("%s: %d: free =%ldKB\n", __func__, __LINE__, free_kbytes_1);

	rtk_iommu_dma_free(NULL, g_size, g_cpu_addr, g_iova, 0);

	free_kbytes_2 = global_zone_page_state(NR_FREE_PAGES) << (PAGE_SHIFT - 10);
	rtd_pr_rmm_info("%s: %d: free =%ldKB\n", __func__, __LINE__, free_kbytes_2);

	rtd_pr_rmm_info("%s: meminfo diff %ldKB, iova size = %ldKB\n", __func__, free_kbytes_2 - free_kbytes_1, g_size / 1024);

	g_iova = 0;
	g_size = 0;
	g_cpu_addr = NULL;

	if (free_kbytes_1 > free_kbytes_2) { // begin should be smaller
		size = 1; // fail
		goto out;
	}
	if (result != 0 && get_entry_compare)
		size = 1; // fail
	else
		size = 0;

	if (ret != 0)
		size = 1; // fail

out:
    return sprintf(buf, "%llu\n", (u64)size);
}

static ssize_t rt_mmu_set_entry_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	const char *ptr = buf;
	char *endptr;

	set_entry_length = simple_strtol(ptr, &endptr, 10);
	ptr = endptr+1;
	set_entry_rbus = (unsigned int)simple_strtol(ptr, &endptr, 10);
	ptr = endptr+1;

    return count;
}

static ssize_t rt_mmu_set_entry_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	unsigned long size = 0;
//	int ret = 0;

	unsigned long free_kbytes_1, free_kbytes_2;

	free_kbytes_1 = global_zone_page_state(NR_FREE_PAGES) <<(PAGE_SHIFT - 10);
	rtd_pr_rmm_info("%s: %d: free =%ldKB\n", __func__, __LINE__, free_kbytes_1);

	if (set_entry_length == 0) {
		rtd_pr_rmm_err("%s: no given length\n", __func__);
		size = 1; // fail
	}

	rtk_iommu_vbm_iova_exit(NULL);

	g_size = set_entry_length;
	g_cpu_addr = rtk_iommu_dma_alloc(NULL, g_size, (dma_addr_t *)&g_iova, GFP_KERNEL, 0);
	if (g_cpu_addr == NULL) {
		rtd_pr_rmm_err("%s: alloc fail\n", __func__);
		size = 1; // fail
	}

	free_kbytes_2 = global_zone_page_state(NR_FREE_PAGES) << (PAGE_SHIFT - 10);
	rtd_pr_rmm_info("%s: %d: free =%ldKB\n", __func__, __LINE__, free_kbytes_2);

	rtd_pr_rmm_info("%s: meminfo diff %ldKB, iova size = %ldKB\n", __func__, free_kbytes_1 - free_kbytes_2, g_size / 1024);

	if (free_kbytes_1 < free_kbytes_2) { // begin should be larger
		size = 1; // fail
		goto out;
	}
	if ((free_kbytes_1 - free_kbytes_2) >= (g_size / 1024))
		size = 0;
	else
		size = 1; // fail

//	pgtable = rtk_iommu_iova_table_offset(g_iova);
//	rtd_pr_rmm_info("%s: iova=%lx, size=%lx, cpu_addr=%lx, dma_mode=%d, pgtable=%lx\n", __func__,
//			g_iova, length, (unsigned long)g_cpu_addr, dma, (unsigned long)pgtable);

//	ret = rt_mmu_pgtable_set_entries(g_iova, g_size, dma, pgtable);

out:
	return sprintf(buf, "%llu\n", (u64)size);
}

static ssize_t rt_mmu_unset_entry_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	unsigned long size = 0;
//	int ret = 0;
//	ret = rt_mmu_pgtable_get_entries(1, test_pgtable);
    return sprintf(buf, "%llu\n", (u64)size);
}

static ssize_t __maybe_unused rt_mmu_get_module_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	unsigned long size = 0;
	int ret = 0;
	ret = rt_mmu_pgtable_init_modules();
    return sprintf(buf, "%llu\n", (u64)size);
}

#ifdef CONFIG_REALTEK_IOMMU_VERIFY
unsigned int vodma_pattern_gen(VO_TIMING *tmode, VODMA_CHROMA_FORMAT chroma_format, VODMA_DMA_MODE blk_mode)
{
	unsigned int width;
	unsigned int height;
	unsigned int color_y, color_u, color_v;

	color_y = 128;
	color_u = 140; //cr
	color_v = 100; //cb

	width = tmode->HWidth;
	height = (tmode->isProg)?tmode->VHeight:(tmode->VHeight*2);

	if (blk_mode == DMA_BLOCK_MODE) //block mode
	{
		rtd_pr_rmm_err("%s: no support block mode\n", __func__);
		return 0;
	}
	else //sequential mode
	{
		unsigned char *addr_yuv;
		unsigned int length_total;
		unsigned int i;
		int pixel_byte = 1;
		unsigned long attrs = 0;
		
		if (chroma_format==CHROMA_YUV422 || chroma_format==CHROMA_RGB565 || chroma_format==CHROMA_GRAY)
			pixel_byte = 2;
		else if (chroma_format==CHROMA_RGB888 || chroma_format==CHROMA_YUV444)
			pixel_byte = 3;
		else if (chroma_format==CHROMA_ARGB8888)
			pixel_byte = 4;

		// seq 422 field based
		length_total = ((width*height*pixel_byte+63)/64)*64;

		rtk_iommu_vbm_iova_exit(NULL);

#ifdef CONFIG_REALTEK_IOMMU_CONTIG_MEMORY_HACK
		attrs = DMA_ATTR_FORCE_CONTIGUOUS;
#endif
		g_cpu_addr = rtk_iommu_dma_alloc(NULL, length_total, (dma_addr_t *)&g_iova, GFP_KERNEL, attrs);
		if (g_cpu_addr == NULL) {
			rtd_pr_rmm_err("%s: alloc fail\n", __func__);
			return 0;
		}

		addr_yuv = (unsigned char *)g_cpu_addr;
		g_size = length_total;
		rtd_pr_rmm_info("%s: vaddr=%lx/%x, size=%d \n", __func__, (unsigned long)addr_yuv, (unsigned int)g_iova, length_total);

		for (i=0;i<length_total;i+=pixel_byte)
		{
			color_y = (((i/pixel_byte)%width)/64) * (255/(width/64)); //vertical color bar
			//color_y = ((i/pixel_byte)/(width*64)) * (255/(height/64)); //horizontal color bar

			if (chroma_format==CHROMA_YUV422 || chroma_format==CHROMA_GRAY)
			{
				*(addr_yuv+i) = color_y; //y
				*(addr_yuv+i+1) = (i%4==0)?color_v:color_u; //uv
			}

			else if (chroma_format==CHROMA_RGB565)
			{
				unsigned int rgb565 = (((color_y & 0xF8)>>3)<<11) | (((color_u & 0xFC)>>2)<<5) | (((color_v & 0xF8)>>3));
				*(addr_yuv+i) = (rgb565 & 0xFF00)>>8;
				*(addr_yuv+i+1) = (rgb565 & 0x00FF);
			}

			else if (chroma_format==CHROMA_RGB888 || chroma_format==CHROMA_YUV444)
			{
				*(addr_yuv+i+0) = color_u;
				*(addr_yuv+i+1) = color_v;
				*(addr_yuv+i+2) = color_y;
				*(addr_yuv+i+3) = color_u;
				*(addr_yuv+i+4) = color_v;
				*(addr_yuv+i+5) = color_y; //R
				*(addr_yuv+i+6) = color_u; //G
				*(addr_yuv+i+7) = color_v; //B

				*(addr_yuv+i+8) = color_v;
				*(addr_yuv+i+9) = color_y;
				*(addr_yuv+i+10) = color_u;
				*(addr_yuv+i+11) = color_v;
				*(addr_yuv+i+12) = color_y;
				*(addr_yuv+i+13) = color_u;
				*(addr_yuv+i+14) = color_v;
				*(addr_yuv+i+15) = color_y;

				*(addr_yuv+i+16) = color_y;
				*(addr_yuv+i+17) = color_u;
				*(addr_yuv+i+18) = color_v;
				*(addr_yuv+i+19) = color_y;
				*(addr_yuv+i+20) = color_u;
				*(addr_yuv+i+21) = color_v;
				*(addr_yuv+i+22) = color_y;
				*(addr_yuv+i+23) = color_u;

				i+=pixel_byte*(8-1);
			}

			else if (chroma_format==CHROMA_ARGB8888)
			{
				*(addr_yuv+i+0) = 0; 			//A
				*(addr_yuv+i+1) = color_y; 		//R
				*(addr_yuv+i+2) = color_u;		//G
				*(addr_yuv+i+3) = color_v;		//B

				*(addr_yuv+i+4) = 0; 			//A
				*(addr_yuv+i+5) = color_y;		//R
				*(addr_yuv+i+6) = color_u;		//G
				*(addr_yuv+i+7) = color_v;		//B

				i+=pixel_byte*(2-1);
			}
		}
	}

	return g_iova;
}

static ssize_t rt_mmu_testcase_vo_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	const char *ptr = buf;
	char *endptr;

	g_timing = (VODMA_TIMING_MODE)simple_strtol(ptr, &endptr, 10);
	ptr = endptr+1;

	if (g_timing >= VODMA_TIMING_MODE_NUM)
		g_timing = TIMING_MODE_1080I50; // default
	
    return count;	
}

static ssize_t rt_mmu_testcase_vo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	unsigned long size;
	
	VO_TIMING * tmode;	//arg for internal used
	VO_TIMING tmode0;
	unsigned int iova;
//	int i = 0;
	
	memcpy(&tmode0, &VODMA_StdTimingTable[g_timing], sizeof(VO_TIMING));

	tmode = &tmode0;

	tmode->HStartPos	= 140;
	tmode->VStartPos	= 26;
	tmode->HTotal		= tmode->HWidth + (tmode->HStartPos * 2);
	tmode->VTotal		= (tmode->VHeight*(tmode->isProg?1:2)) + (tmode->VStartPos * 2) + (tmode->isProg?0:8);
	tmode->SampleRate	= (tmode->HTotal * tmode->VTotal * (tmode->FrameRate/1000) + 9999)/ 10000;

	iova = (unsigned long)vodma_pattern_gen(tmode, CHROMA_YUV422, DMA_SEQUENTIAL_MODE);

	if (g_cpu_addr)
	{
#ifdef CONFIG_REALTEK_IOMMU_CONTIG_MEMORY_HACK
		size = (unsigned long)virt_to_phys(g_cpu_addr);
#else
		size = (unsigned long)iova;
#endif
#if 0
		/* endian swap */
		for (i = 0; i < g_size/sizeof(unsigned int); i+=4)
		{
			unsigned int *__tmp = (unsigned int *)g_cpu_addr;
			unsigned int val = *__tmp;
			*__tmp = swap_endian(val);

//			rtd_pr_rmm_info("__tmp[%06d]=%lx: %08x, %08x, %08x, %08x\n", i, (unsigned long)&__tmp[i],
//					__tmp[i+0], __tmp[i+1], __tmp[i+2], __tmp[i+3]);
		}
#endif

		dmac_flush_range(g_cpu_addr, g_cpu_addr + g_size);
	} else {
		size = 0;
	}

	return sprintf(buf, "%llu\n", (u64)size);
}

static void rt_mmu_testcase_md_make_pattern(void *cpu_addr, unsigned long size)
{
//	int i = 0;
	if (size == sizeof(md_cpy_pattern)) {
		memcpy((char *)cpu_addr, (char *)md_cpy_pattern, size);
	} else {
		memset((char *)cpu_addr, 0x0f, size);
	}

//	for (i = 0; i < 400/sizeof(unsigned int); i+=4)
//	{
//		unsigned int *__g_cpu_addr = (unsigned int *)g_cpu_addr;
//		rtd_pr_rmm_info("cpu_addr[%06d]=%lx: %08x, %08x, %08x, %08x\n", i, (unsigned long)&__g_cpu_addr[i], 
//			 __g_cpu_addr[i+0], __g_cpu_addr[i+1], __g_cpu_addr[i+2], __g_cpu_addr[i+3]);
//	}

//	for (i = 0; i < 400/sizeof(unsigned int); i+=4)
//	{
//		unsigned int *__md_pat = (unsigned int *)md_cpy_pattern;
//		rtd_pr_rmm_info("md_pat[%06d]=%lx: %08x, %08x, %08x, %08x\n", i, (unsigned long)&__md_pat[i], 
//			 __md_pat[i+0], __md_pat[i+1], __md_pat[i+2], __md_pat[i+3]);
//	}

	dmac_flush_range(cpu_addr, cpu_addr+size);
}

static int rt_mmu_testcase_md_result_compare(void *dst, void *src, unsigned long size)
{
//	int i = 0;
//	for (i = 0; i < size/sizeof(unsigned int); i+=4)
//	{
//		unsigned int *__dst_uncache = (unsigned int *)dst;
//		rtd_pr_rmm_info("dst_uncache[%06d]=%lx: %08x, %08x, %08x, %08x\n", i, (unsigned long)&__dst_uncache[i], 
//			 __dst_uncache[i+0], __dst_uncache[i+1], __dst_uncache[i+2], __dst_uncache[i+3]);
//	}
	
	if (size == sizeof(md_cpy_pattern)) {
		if (memcmp((char *)src, (char *)dst, size) != 0) {
			rtd_pr_rmm_err("%s: %d: compare NG\n", __func__, __LINE__);
			return -1;
		}
		if (memcmp((char *)dst, (char *)md_cpy_pattern, size) != 0) {
			rtd_pr_rmm_err("%s: %d: compare NG\n", __func__, __LINE__);
			return -2;
		}
	} else {
		if (memchr_inv((char *)dst, 0x0f, size) != NULL) {
			return -1;
		}
	}

	return 0;
}

static ssize_t rt_mmu_testcase_md_size_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	const char *ptr = buf;
	char *endptr;

	g_size = simple_strtol(ptr, &endptr, 10);
	ptr = endptr+1;
	set_entry_rbus = (unsigned int)simple_strtol(ptr, &endptr, 10);
	ptr = endptr+1;
	md_size_iova_free = (unsigned int)simple_strtol(ptr, &endptr, 10);
	ptr = endptr+1;

    return count;
}

static ssize_t rt_mmu_testcase_md_size_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	unsigned long size = 0;
	unsigned long attrs = 0;
	void *dst_cache = NULL, *dst_uncache = NULL;
	int ret = 0;
//	int i = 0;

	if (g_size == 0) {
		rtd_pr_rmm_err("%s: no given g_size\n", __func__);
		size = 1;
		goto out;
	}

	rtk_iommu_vbm_iova_exit(NULL);

#ifdef CONFIG_REALTEK_IOMMU_CONTIG_MEMORY_HACK
	attrs = DMA_ATTR_FORCE_CONTIGUOUS;
#endif
	g_cpu_addr = rtk_iommu_dma_alloc(NULL, g_size, (dma_addr_t *)&g_iova, GFP_KERNEL, attrs);
	if (g_cpu_addr == NULL) {
		rtd_pr_rmm_err("%s: alloc fail\n", __func__);
		size = 1;
		goto out;
	}

	// make pattern
	rt_mmu_testcase_md_make_pattern((void *)g_cpu_addr, g_size);

	dst_cache = dvr_malloc_uncached(g_size, &dst_uncache);
	if (dst_cache == NULL) {
		rtd_pr_rmm_err("%s: no dst_cache\n", __func__);
		size = 1; // fail
		goto out;
	}

#ifdef CONFIG_REALTEK_IOMMU_CONTIG_MEMORY_HACK
	smd_memcpy(dvr_to_phys(dst_cache), virt_to_phys(g_cpu_addr), g_size);
#else
	smd_memcpy(dvr_to_phys(dst_cache), g_iova, g_size);
#endif
   smd_checkComplete();

	// compare result
	ret = rt_mmu_testcase_md_result_compare(dst_uncache, g_cpu_addr, g_size);
	if (ret == -1) {
		rtd_pr_rmm_err("%s: %d: compare NG, iova=%lx, size=%ld\n", __func__, __LINE__, g_iova, g_size);
		size = 2; // fail
		goto out;
	} else if (ret == -2) {
		rtd_pr_rmm_err("%s: %d: compare NG, iova=%lx, size=%ld\n", __func__, __LINE__, g_iova, g_size);
		size = 3; // fail
		goto out;
	}

	rtd_pr_rmm_info("%s: compare OK\n", __func__);
	size = 0;

out:
	if (g_cpu_addr != NULL && md_size_iova_free)
		rtk_iommu_dma_free(NULL, g_size, g_cpu_addr, g_iova, 0);
	if (dst_cache)
		dvr_free(dst_cache);
	
	g_iova = 0;
	g_size = 0;
	g_cpu_addr = NULL;
	
	return sprintf(buf, "%llu\n", (u64)size);
}

static ssize_t rt_mmu_testcase_reg_access_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int i;
	int err = 0;
	int mismatch_check = 0;
	int count = sizeof(spec) / sizeof(struct spec_register);

	rtd_pr_rmm_info("%s: count=%d, sizeof(size)=%ld/%ld", __func__, count, sizeof(spec), sizeof(struct spec_register));

	for (i = 0; i < count; i++) {
		if (spec[i].check_def_value) {
			if (spec[i].def_value != rtd_inl(spec[i].rbus_addr)) {
				rtd_pr_rmm_info("%s: %08x->%08x %s default value mismatch", __func__, spec[i].def_value, rtd_inl(spec[i].rbus_addr), spec[i].name);
				mismatch_check++;
			}
		}
	}

	if (mismatch_check)
		err = 1;
	else
		err = 0;

	return err;
}

static ssize_t rt_mmu_testcase_dup_table_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	unsigned long size = 0;
	unsigned long length = 0;
	unsigned long iova_1, iova_2;
	void *cpu_addr1 = NULL, *cpu_addr2 = NULL;
	unsigned long attrs = 0;
	void *dst_cache = NULL, *dst_uncache = NULL;
	int copy_size = 0;
	unsigned int *pgtable1, *pgtable2;
	int ret = 0;

	length = 400*1024;
	copy_size = sizeof(unsigned int) * (length >> SPAGE_ORDER);

	rtk_iommu_vbm_iova_exit(NULL);

#ifdef CONFIG_REALTEK_IOMMU_CONTIG_MEMORY_HACK
	attrs = DMA_ATTR_FORCE_CONTIGUOUS;
#endif
	cpu_addr1 = rtk_iommu_dma_alloc(NULL, length, (dma_addr_t *)&iova_1, GFP_KERNEL, attrs);
	if (cpu_addr1 == NULL) {
		rtd_pr_rmm_err("%s: alloc fail iova_1\n", __func__);
		size = 1;
		goto out;
	}
	cpu_addr2 = rtk_iommu_dma_alloc(NULL, length, (dma_addr_t *)&iova_2, GFP_KERNEL, attrs);
	if (cpu_addr2 == NULL) {
		rtd_pr_rmm_err("%s: alloc fail iova_2\n", __func__);
		size = 1;
		goto out;
	}

	// duplicate table , copy iova_1 to iova_2
	pgtable1 = rtk_iommu_iova_table_offset(iova_1);
	pgtable2 = rtk_iommu_iova_table_offset(iova_2);
	memcpy((char *)pgtable2, (char *)pgtable1, copy_size);
	ret = rt_mmu_pgtable_set_entries(iova_2, length, 1, pgtable2); // dma mode

	memcpy((char *)cpu_addr1, (char *)md_cpy_pattern, length);
	dmac_flush_range(cpu_addr1, cpu_addr1+length);

	dst_cache = dvr_malloc_uncached(length, &dst_uncache);
	if (dst_cache == NULL) {
		rtd_pr_rmm_err("%s: no dst_cache\n", __func__);
		size = 1; // fail
		goto out;
	}
#ifdef CONFIG_REALTEK_IOMMU_CONTIG_MEMORY_HACK
	smd_memcpy(dvr_to_phys(dst_cache), virt_to_phys(cpu_addr1), length);
#else
	smd_memcpy(dvr_to_phys(dst_cache), iova_1, length);
#endif
   smd_checkComplete();

	if (memcmp((char *)cpu_addr1, (char *)dst_uncache, length) != 0) {
		rtd_pr_rmm_err("%s: %d: compare NG\n", __func__, __LINE__);
		size = 2; // fail
		goto out;
	}
	if (memcmp((char *)dst_uncache, (char *)md_cpy_pattern, length) != 0) {
		rtd_pr_rmm_err("%s: %d: compare NG\n", __func__, __LINE__);
		size = 3; // fail
		goto out;
	}

	// make data corruption in iova_2
	memset((char *)dst_uncache, 0x0f, length);
#ifdef CONFIG_REALTEK_IOMMU_CONTIG_MEMORY_HACK
	smd_memcpy(virt_to_phys(cpu_addr1), dvr_to_phys(dst_cache), length);
#else
	smd_memcpy(iova_2, dvr_to_phys(dst_cache), length);
#endif
   smd_checkComplete();
   
	if (memcmp((char *)cpu_addr1, (char *)md_cpy_pattern, length) != 0) {
		rtd_pr_rmm_err("%s: %d: compare NG\n", __func__, __LINE__);
		size = 4; // fail
		goto out;
	}

	rtd_pr_rmm_info("%s: compare OK\n", __func__);
	size = 0;

out:
	if (size == 4) { // table conflict
		ret = rt_mmu_pgtable_get_entries(iova_2, length, (unsigned int *)dst_cache);
		if (memcmp((char *)pgtable1, (char *)dst_cache, copy_size) == 0) {
			rtd_pr_rmm_info("%s: %d: compare OK as expect\n", __func__, __LINE__);
			size = 0;
		}
	}
	if (cpu_addr1 != NULL)
		rtk_iommu_dma_free(NULL, length, cpu_addr1, iova_1, 0);
	if (cpu_addr2 != NULL)
		rtk_iommu_dma_free(NULL, length, cpu_addr2, iova_2, 0);
	if (dst_cache)
		dvr_free(dst_cache);
	
	return sprintf(buf, "%llu\n", (u64)size);
}

extern void *vbm_heap_dmabuf_array_frame_keep_set(unsigned int frame, unsigned long *iova, unsigned long *size);
extern int vbm_heap_dmabuf_array_frame_keep_get(int (* func_ptr)(char, void *, unsigned long, unsigned long));
extern void vbm_heap_dmabuf_array_frame_keep_release(void);

int frame_keep_md_access_compare(char frame, void *cpu_addr, unsigned long iova, unsigned long size)
{
	void *dst_cache = NULL, *dst_uncache = NULL;
	unsigned long len = 400*1024;
	int ret = 0;

	dst_cache = dvr_malloc_uncached(len, &dst_uncache);
	if (dst_cache == NULL) {
		ret = -3;
		goto out;
	}

	if (cpu_addr == NULL) {
		ret = -4;
		goto out;
	}
	
#ifdef CONFIG_REALTEK_IOMMU_CONTIG_MEMORY_HACK
	smd_memcpy(dvr_to_phys(dst_cache), virt_to_phys(cpu_addr), len);
#else
	smd_memcpy(dvr_to_phys(dst_cache), iova, len);
#endif
   smd_checkComplete();
   
	// compare result
	ret = rt_mmu_testcase_md_result_compare(dst_uncache, cpu_addr, len);
	if (ret == -1) {
		rtd_pr_rmm_err("%s: %d: frame[%d] compare NG, iova=%lx, size=%ld\n", __func__, __LINE__, frame, iova, len);
		goto out;
	} else if (ret == -2) {
		rtd_pr_rmm_err("%s: %d: frame[%d] compare NG, iova=%lx, size=%ld\n", __func__, __LINE__, frame, iova, len);
		goto out;
	}

	rtd_pr_rmm_info("%s: frame[%d] compare OK, iova=%lx, size=%ld\n", __func__, frame, iova, len);

out:
	if (dst_cache)
		dvr_free(dst_cache);
	
	return ret;
}

static void frame_keep_md_access_func(struct work_struct *unused)
{
	int ret = 0;

	ret = vbm_heap_dmabuf_array_frame_keep_get(frame_keep_md_access_compare);
	
	if (frame_keep_md_access_workq)
		queue_delayed_work(frame_keep_md_access_workq, &delayed_frame_keep_work, msecs_to_jiffies(frame_keep_md_access_workq_delay));
}

static ssize_t rt_mmu_testcase_frame_keep_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	const char *ptr = buf;
	char *endptr = NULL;
	int i = 0;

	g_frame_keep_count = simple_strtol(ptr, &endptr, 10);
	ptr = endptr+1;

	rtd_pr_rmm_info("%s: total keep frame %d\n", __func__, g_frame_keep_count);

	for (i = 0; i < g_frame_keep_count; i++) {
		unsigned int frame;
		void *cpu_addr;
		unsigned long iova, size;
		
		frame = (unsigned int)simple_strtol(ptr, &endptr, 10);
		ptr = endptr+1;

		cpu_addr = vbm_heap_dmabuf_array_frame_keep_set(frame, &iova, &size);
		if (cpu_addr)
			rt_mmu_testcase_md_make_pattern((void *)cpu_addr, 400*1024); // using md_pattern
	}
	
	frame_keep_md_access_func(&delayed_frame_keep_work.work);

    return count;
}

static ssize_t rt_mmu_testcase_frame_keep_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	unsigned long size = 0;

	cancel_delayed_work_sync(&delayed_frame_keep_work);
	vbm_heap_dmabuf_array_frame_keep_release();

	g_frame_keep_count = 0;

	return sprintf(buf, "%llu\n", (u64)size);
}
#endif

static struct kobj_attribute rt_mmu_open_attr =
    __ATTR(open, 0644, rt_mmu_open_show, NULL);
static struct kobj_attribute rt_mmu_get_entry_attr =
    __ATTR(get_entry, 0644, rt_mmu_get_entry_show, rt_mmu_get_entry_store);
static struct kobj_attribute rt_mmu_set_entry_attr =
    __ATTR(set_entry, 0644, rt_mmu_set_entry_show, rt_mmu_set_entry_store);
static struct kobj_attribute rt_mmu_unset_entry_attr =
    __ATTR(unset_entry, 0644, rt_mmu_unset_entry_show, NULL);
static struct kobj_attribute rt_mmu_get_module_attr =
    __ATTR(get_module, 0644, rt_mmu_get_module_show, NULL);
static struct kobj_attribute rt_mmu_debug_attr =
    __ATTR(debug, 0644, NULL, NULL);
#ifdef CONFIG_REALTEK_IOMMU_VERIFY
static struct kobj_attribute rt_mmu_testcase_md_size_attr =
    __ATTR(testcase_md_size, 0644, rt_mmu_testcase_md_size_show, rt_mmu_testcase_md_size_store);
static struct kobj_attribute rt_mmu_testcase_vo_attr =
    __ATTR(testcase_vo, 0644, rt_mmu_testcase_vo_show, rt_mmu_testcase_vo_store);
static struct kobj_attribute rt_mmu_testcase_dup_table_attr =
    __ATTR(testcase_dup_table, 0644, rt_mmu_testcase_dup_table_show, NULL);
static struct kobj_attribute rt_mmu_testcase_frame_keep_attr =
    __ATTR(testcase_frame_keep, 0644, rt_mmu_testcase_frame_keep_show, rt_mmu_testcase_frame_keep_store);
static struct kobj_attribute rt_mmu_testcase_reg_access_attr =
    __ATTR(testcase_reg_access, 0644, rt_mmu_testcase_reg_access_show, NULL);
#endif
static struct kobj_attribute rt_mmu_close_attr =
    __ATTR(close, 0644, rt_mmu_close_show, NULL);

static struct attribute *rt_mmu_attrs[] = {
    &rt_mmu_open_attr.attr,
    &rt_mmu_get_entry_attr.attr,
    &rt_mmu_set_entry_attr.attr,
    &rt_mmu_unset_entry_attr.attr,
    &rt_mmu_get_module_attr.attr,
    &rt_mmu_debug_attr.attr,
#ifdef CONFIG_REALTEK_IOMMU_VERIFY
    &rt_mmu_testcase_md_size_attr.attr,
    &rt_mmu_testcase_vo_attr.attr,
    &rt_mmu_testcase_dup_table_attr.attr,
	&rt_mmu_testcase_frame_keep_attr.attr,
    &rt_mmu_testcase_reg_access_attr.attr,
#endif
    &rt_mmu_close_attr.attr,
    NULL,
};

static struct attribute_group rtk_mmu_attr_group = {
    .name = "rt_mmu",
    .attrs = rt_mmu_attrs,
};

static int realtek_sysfs_rt_mmu_init(struct kobject *parent_kobj)
{
    int err = 0;

    err = sysfs_create_group(parent_kobj, &rtk_mmu_attr_group);
    if(err) {
        rtd_pr_rmm_err("Realtek: sysfs init rt_mmu failed, err=%d\n", err);
    }
    return err;
}

int __init mm_rt_mmu_sysfs(void)
{
    int err = 0;

	err = realtek_sysfs_rt_mmu_init(mm_kobj);
    if (err) {
        rtd_pr_rmm_err("failed to register rt_mmu group, err=%d\n", err);
    } else {
#ifdef CONFIG_REALTEK_IOMMU_VERIFY
		frame_keep_md_access_workq = create_singlethread_workqueue("frame_keep_md_access");
		INIT_DELAYED_WORK(&delayed_frame_keep_work, frame_keep_md_access_func);
#endif
    }
    return err;
}
//device_initcall(mm_rt_mmu_sysfs);
