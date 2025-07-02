#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/pageremap.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <rtk_kdriver/io.h>
#include <base_types.h>
#define CFG_RTK_CMA_MAP 0
#include <linux/kernel.h>
#include <linux/tee.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/tee_drv.h>
#include <linux/uuid.h>
#include <rbus/timer_reg.h>
#include <rtk_se_ta.h>
#include <rtd_log/rtd_module_log.h>

#define TEE_NUM_PARAM 4
#define TA_NAME		"se.ta"
static const uuid_t SE_UUID =UUID_INIT(0xf3d9dae3, 0x4ec4, 0x4bcc,0xbb, 0xe2, 0x2c, 0x04, 0x36, 0x7c, 0x90, 0xb0);
	
#define SE_WRITE_CMD	0
#define SE_GO_CMD		1
#define SE_CHECK_CMD		2
#define SE_LOCK_MEM_CMD		3
#define SE_UNLOCK_MEM_CMD		4

#define SE_ONE_CMD_SIZE  1024
#define SE_CHECK_TEE_TIMEOUT_COUNT 100
struct optee_ta se_ta;
//struct tee_shm *se_shm_buffer = NULL;
int se_shm_len = 0;
//phys_addr_t se_shm_pa = 0;
//char *se_shm_va = 0;
static int optee_se_match(struct tee_ioctl_version_data *data, const void *vers)
{
        return 1;
}
/* 0: success, -N: failure (N: value) */
int optee_se_init(void)
{
        int ret = 0, rc = 0;
        struct tee_ioctl_open_session_arg arg;
        struct tee_ioctl_version_data vers = {
                .impl_id = TEE_IMPL_ID_OPTEE,
                .impl_caps = TEE_OPTEE_CAP_TZ,
                .gen_caps = TEE_GEN_CAP_GP,
        };
        if(se_ta.session != 0) {
                return 0;
        }
        memset(&se_ta, 0, sizeof(se_ta));
        se_ta.ctx = tee_client_open_context(NULL, optee_se_match, NULL, &vers);
        if(se_ta.ctx == NULL) {
                rtd_pr_se_err("optee_se: no ta context\n");
                ret = -EINVAL;
                goto err;
        }
        memset(&arg, 0, sizeof(arg));
        memcpy(&arg.uuid, &SE_UUID, sizeof(uuid_t));
        arg.clnt_login = TEE_IOCTL_LOGIN_PUBLIC;
        rc = tee_client_open_session(se_ta.ctx, &arg, NULL);
        if(rc) {
                rtd_pr_se_err("optee_se: open_session failed ret %x arg %x", rc, arg.ret);
                ret = -EINVAL;
                goto err;
        }
        if (arg.ret) {
                ret = -EINVAL;
                goto err;
        }
        se_ta.session = arg.session;
        return 0;
err:
        if (se_ta.session) {
                tee_client_close_session(se_ta.ctx, se_ta.session);
                rtd_pr_se_err("optee_se: open failed close session \n");
                se_ta.session = 0;
        }
        if (se_ta.ctx) {
                tee_client_close_context(se_ta.ctx);
                rtd_pr_se_err("optee_se: open failed close context\n");
                se_ta.ctx = NULL;
        }
        rtd_pr_se_err("open_session fail\n");
        rtd_pr_se_err("%s %d\n", __func__, __LINE__);
        return ret;
}
void optee_se_deinit(void)
{
    if (se_ta.session) {
            tee_client_close_session(se_ta.ctx, se_ta.session);
            se_ta.session = 0;
    }
    if (se_ta.ctx) {
            tee_client_close_context(se_ta.ctx);
            se_ta.ctx = NULL;
    }
}

/* 0: success, -N: failure (N: value) */
int optee_se_lockmem_cmd(unsigned int src_addr         , unsigned int src_BufferSize,unsigned int dst_addr, unsigned 
int dst_BufferSize)
{	
		int ret = 0, rc = 0;
		struct tee_ioctl_invoke_arg arg;
		struct tee_param *param = NULL;
		if(se_ta.ctx == NULL) {
				rtd_pr_se_err("optee_meminfo: no ta context\n");
				ret = -EINVAL;
				goto out;
		}
		param = kcalloc(TEE_NUM_PARAM, sizeof(struct tee_param), GFP_KERNEL);
		if(param == NULL) {
				rtd_pr_se_err("optee_se_set_cw malloc param fail\n");
				ret = -EINVAL;
				goto out;
		}
		memset(&arg, 0, sizeof(arg));
		arg.func = SE_LOCK_MEM_CMD;
		arg.session = se_ta.session;
		arg.num_params = TEE_NUM_PARAM;
		memset(param, 0, sizeof(struct tee_param) * TEE_NUM_PARAM);
		param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
		param[0].u.value.a = src_addr; // NONE
		param[0].u.value.b = src_BufferSize; // NONE
		param[1].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
		param[1].u.value.a = dst_addr; // NONE
		param[1].u.value.b = dst_BufferSize; // NONE
		param[2].attr = TEE_IOCTL_PARAM_ATTR_TYPE_NONE;	
		param[3].attr = TEE_IOCTL_PARAM_ATTR_TYPE_NONE;
		rc = tee_client_invoke_func(se_ta.ctx, &arg, param);
		if (rc || arg.ret) {
				rtd_pr_se_err("optee_meminfo: invoke failed ret %x arg.ret %x\n", rc, arg.ret);
				ret = -EINVAL;
				goto out;
		}
		//rtd_pr_se_err("check se cmd: invoke param[0].u.value.a %lld \n", param[0].u.value.a);		
out:
		if (param)
				kfree(param);
		if (ret)
				return ret;
		else
				return 0;
}

/* 0: success, -N: failure (N: value) */
int optee_se_unlockmem_cmd(unsigned int src_addr         , unsigned int src_BufferSize,unsigned int dst_addr, unsigned int dst_BufferSize)
{	
		int ret = 0, rc = 0;
		struct tee_ioctl_invoke_arg arg;
		struct tee_param *param = NULL;
		if(se_ta.ctx == NULL) {
				rtd_pr_se_err("optee_meminfo: no ta context\n");
				ret = -EINVAL;
				goto out;
		}
		param = kcalloc(TEE_NUM_PARAM, sizeof(struct tee_param), GFP_KERNEL);
		if(param == NULL) {
				rtd_pr_se_err("optee_se_set_cw malloc param fail\n");
				ret = -EINVAL;
				goto out;
		}
		memset(&arg, 0, sizeof(arg));
		arg.func = SE_UNLOCK_MEM_CMD;
		arg.session = se_ta.session;
		arg.num_params = TEE_NUM_PARAM;
		memset(param, 0, sizeof(struct tee_param) * TEE_NUM_PARAM);
		param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
		param[0].u.value.a = src_addr; // NONE
		param[0].u.value.b = src_BufferSize; // NONE
		param[1].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
		param[1].u.value.a = dst_addr; // NONE
		param[1].u.value.b = dst_BufferSize; // NONE
		param[2].attr = TEE_IOCTL_PARAM_ATTR_TYPE_NONE;	
		param[3].attr = TEE_IOCTL_PARAM_ATTR_TYPE_NONE;
		rc = tee_client_invoke_func(se_ta.ctx, &arg, param);
		if (rc || arg.ret) {
				rtd_pr_se_err("optee_meminfo: invoke failed ret %x arg.ret %x\n", rc, arg.ret);
				ret = -EINVAL;
				goto out;
		}
		//rtd_pr_se_err("check se cmd: invoke param[0].u.value.a %lld \n", param[0].u.value.a);		
out:
		if (param)
				kfree(param);
		if (ret)
				return ret;
		else
				return 0;
}

/* 0: success, -N: failure (N: value) */
int optee_se_excute_cmd(unsigned char* pbyCommandBuffer, int lCommandLength)
{
        int ret = 0, rc = 0;
        struct tee_ioctl_invoke_arg arg;
        struct tee_param *param = NULL;
        struct tee_shm *shm_buffer = NULL;
        int shm_len = 0;
        char *shm_va;
        if(se_ta.ctx == NULL) {
                rtd_pr_se_err("optee_meminfo: no ta context\n");
                ret = -EINVAL;
                goto out;
        }
        param = kcalloc(TEE_NUM_PARAM, sizeof(struct tee_param), GFP_KERNEL);
        if(param == NULL) {
                rtd_pr_se_err("optee_se_set_cw malloc param fail\n");
                ret = -EINVAL;
                goto out;
        }
        memset(&arg, 0, sizeof(arg));
		arg.func = SE_WRITE_CMD;
        arg.session = se_ta.session;
        arg.num_params = TEE_NUM_PARAM;
        shm_len = SE_ONE_CMD_SIZE;
        shm_buffer = tee_shm_alloc(se_ta.ctx, shm_len, TEE_SHM_MAPPED);
        if (shm_buffer == NULL) {
                rtd_pr_se_err("optee_se: no shm_buffer\n");
                ret = -ENOMEM;
                goto out;
        }
        // get share memory virtual addr for data accessing
        shm_va = (char*)tee_shm_get_va(shm_buffer, 0);
        if (shm_va == NULL) {
                ret = -ENOMEM;
                goto out;
        }
        memset(param, 0, sizeof(struct tee_param) * TEE_NUM_PARAM);
		memcpy(shm_va,pbyCommandBuffer,lCommandLength);
        param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
        param[0].u.value.a = 0; // NONE
        param[0].u.value.b = 0; // NONE
        param[1].attr = TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_INPUT;
        param[1].u.memref.shm = shm_buffer; // TPK_DESCRAMBLE_ALGO_AES_128_CBC_OFB
        param[1].u.memref.size = lCommandLength;
        param[2].attr = TEE_IOCTL_PARAM_ATTR_TYPE_NONE;
        param[3].attr = TEE_IOCTL_PARAM_ATTR_TYPE_NONE;
        rc = tee_client_invoke_func(se_ta.ctx, &arg, param);
        if (rc || arg.ret) {
                rtd_pr_se_err("optee_meminfo: invoke failed ret %x arg.ret %x\n", rc, arg.ret);
                ret = -EINVAL;
                goto out;
        }
out:
        if (shm_buffer)
                tee_shm_free(shm_buffer);
        if (param)
                kfree(param);
        if (ret)
                return ret;
        else
                return 0;
}
/* 0: success, -N: failure (N: value) */
int optee_se_check_cmd(int* se_ready)
{
        int ret = 0, rc = 0;
        struct tee_ioctl_invoke_arg arg;
        struct tee_param *param = NULL;
        if(se_ta.ctx == NULL) {
                rtd_pr_se_err("optee_meminfo: no ta context\n");
                ret = -EINVAL;
                goto out;
        }
        param = kcalloc(TEE_NUM_PARAM, sizeof(struct tee_param), GFP_KERNEL);
        if(param == NULL) {
                rtd_pr_se_err("optee_se_set_cw malloc param fail\n");
                ret = -EINVAL;
                goto out;
        }
        memset(&arg, 0, sizeof(arg));
		arg.func = SE_CHECK_CMD;
        arg.session = se_ta.session;
        arg.num_params = TEE_NUM_PARAM;
        memset(param, 0, sizeof(struct tee_param) * TEE_NUM_PARAM);
        param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_OUTPUT;
        param[0].u.value.a = 7; // NONE
        param[0].u.value.b = 8; // NONE
        param[1].attr = TEE_IOCTL_PARAM_ATTR_TYPE_NONE;
        param[2].attr = TEE_IOCTL_PARAM_ATTR_TYPE_NONE;
        param[3].attr = TEE_IOCTL_PARAM_ATTR_TYPE_NONE;
        rc = tee_client_invoke_func(se_ta.ctx, &arg, param);
        if (rc || arg.ret) {
                rtd_pr_se_err("optee_meminfo: invoke failed ret %x arg.ret %x\n", rc, arg.ret);
                ret = -EINVAL;
                goto out;
        }
		//rtd_pr_se_err("check se cmd: invoke param[0].u.value.a %lld \n", param[0].u.value.a);
		if(param[0].u.value.a)
			*se_ready = 1;
		else
            *se_ready = 0;
out:
        if (param)
                kfree(param);
        if (ret)
                return ret;
        else
                return 0;
}
int call_se_ta_excute_cmd(unsigned char* pbyCommandBuffer, int lCommandLength)
{
        uint32_t time_90k_01;
        uint32_t time_90k_02;
        uint32_t time_90k_03;
        uint32_t time_90k_04;
		unsigned int delaycounter = 0;
		int se_ret=0;
        time_90k_01 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
        if (optee_se_init())
                return 0;
        time_90k_02 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
		optee_se_excute_cmd(pbyCommandBuffer, lCommandLength);
		while (1) {
			//msleep(1);
			optee_se_check_cmd(&se_ret);
			//rtd_pr_se_err("func=%s line=%d se_ret=%d\n",__FUNCTION__,__LINE__,se_ret);
            if(se_ret)
				break;
			//msleep(1);
			if(delaycounter++ > SE_CHECK_TEE_TIMEOUT_COUNT){
				rtd_pr_se_err("func=%s line=%d optee_se_check_cmd timeout se_ret=%d\n",__FUNCTION__,__LINE__,se_ret);
				break;
			}
		}
		//rtd_pr_se_err("func=%s line=%d optee_se_check_cmd check timeout se_ret=%d count=%d\n",__FUNCTION__,__LINE__,se_ret,delaycounter);
        time_90k_03 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
		optee_se_deinit();
        time_90k_04 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
        //rtd_pr_se_err("time_90k_01 = %d time_90k_02 = %d time_90k_03 = %d time_90k_04 = %d \n", time_90k_01, time_90k_02, time_90k_03, time_90k_04);
        return 0;
}

int call_se_ta_lock_cmd(unsigned int src_addr,unsigned int src_BufferSize,unsigned int dst_addr,unsigned int dst_BufferSize,bool block)
{
	int se_ret=0;
	if (optee_se_init())
			return 0;

	if(block)
	{
		se_ret = optee_se_lockmem_cmd(src_addr,src_BufferSize, dst_addr,dst_BufferSize);
	}
	else
	{
		se_ret = optee_se_unlockmem_cmd(src_addr,src_BufferSize, dst_addr,dst_BufferSize);
	}

	optee_se_deinit();
	return se_ret;
}

EXPORT_SYMBOL(call_se_ta_excute_cmd);
EXPORT_SYMBOL(call_se_ta_lock_cmd);

