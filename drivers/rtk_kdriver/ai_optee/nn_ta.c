#include <linux/delay.h>
#include <rtd_log/rtd_module_log.h>

#define CFG_RTK_CMA_MAP 0

#include <linux/tee.h>
#include <linux/uuid.h>
#include <linux/tee_drv.h>
#include <rbus/sys_reg_reg.h>
#include <io.h>

static const uuid_t pta_uuid =
    UUID_INIT(0x7d41609a, 0x60f5, 0x455a,
        0xad, 0x88, 0x71, 0xcf, 0x83, 0x8f, 0x76, 0xc0);

static const uuid_t uta_uuid =
    UUID_INIT(0xd0a6eb56, 0x5648, 0x479d,
        0x9b, 0x17, 0x21, 0x22, 0xfd, 0xca, 0x6e, 0xe7);

struct nn_ta {
	struct tee_context *ctx;
	__u32 session;
};
static DEFINE_MUTEX(ta_mutex);
static bool aipq_start;

DECLARE_WAIT_QUEUE_HEAD(aipq_wq);


enum pta_nn_cmd_id {
	PTA_NN_CMD_UPDATE_HW_LOCK_STATUS = 0,
	PTA_NN_CMD_ESITIMATE_NPU_USAGE_START,
	PTA_NN_CMD_GET_NPU_USAGE,
	PTA_NN_CMD_MANAGE_POWER,
	PTA_NN_CMD_OPEN_DRV,
	PTA_NN_CMD_CLOSE_DRV,
	PTA_NN_CMD_GET_HW_INFO,
	PTA_NN_CMD_RESET_HW,
	PTA_NN_CMD_INIT_HW,
	PTA_NN_CMD_SUBMIT,
};


enum user_ta_nn_cmd_id {
	USER_TA_NN_CMD_INIT_DRIVER = 0,
	USER_TA_NN_CMD_CREATE_MODEL,
	USER_TA_NN_CMD_DESTROY_MODEL,
	USER_TA_NN_CMD_RUN_MODEL,
	USER_TA_NN_CMD_POST_PROCESS,
	USER_TA_NN_CMD_MANAGE_POWER,
};

static int nn_ta_match(struct tee_ioctl_version_data *data, const void *vers)
{
	return 1;
}

static void nn_ta_deinit(struct nn_ta *ta)
{
	if (ta->session) {
		tee_client_close_session(ta->ctx, ta->session);
		ta->session = 0;
	}

	if (ta->ctx) {
		tee_client_close_context(ta->ctx);
		ta->ctx = NULL;
	}
}

static int nn_ta_init(struct nn_ta *ta, bool call_pta)
{

	int rc = 0;
	struct tee_ioctl_open_session_arg arg = {};
	struct tee_ioctl_version_data vers = {
		.impl_id = TEE_IMPL_ID_OPTEE,
		.impl_caps = TEE_OPTEE_CAP_TZ,
		.gen_caps = TEE_GEN_CAP_GP,
	};

	if (ta->session != 0) {
		return 0;
	}

	ta->ctx = tee_client_open_context(NULL, nn_ta_match, NULL, &vers);
	if (ta->ctx == NULL) {
		rtd_pr_vpq_ai_emerg("open tee/context failed\n");
		goto err;
	}

	memcpy(arg.uuid, call_pta ? pta_uuid.b : uta_uuid.b, TEE_IOCTL_UUID_LEN);
	arg.clnt_login = TEE_IOCTL_LOGIN_PUBLIC;
	rc = tee_client_open_session(ta->ctx, &arg, NULL);
	if (rc || arg.ret) {
		rtd_pr_vpq_ai_emerg("open tee/session failed. rc=0x%x, arg=0x%x\n", rc, arg.ret);
		goto err;
	}

	ta->session = arg.session;
	return 0;

err:
	nn_ta_deinit(ta);
	rtd_pr_vpq_ai_emerg("nn ta: open failed\n");
	return -EINVAL;
}



int nn_ta_npu_usage_est_start(void)
{
	int ret = -EPERM;
	int rc = 0;
	struct tee_ioctl_invoke_arg arg = {};
	struct tee_param param[4] = {};
	struct nn_ta ta = {};

	mutex_lock(&ta_mutex);
	if (nn_ta_init(&ta, 1) != 0)
		goto finish;

	arg.func = PTA_NN_CMD_ESITIMATE_NPU_USAGE_START;
	arg.session = ta.session;
	arg.num_params = 4;

	rc = tee_client_invoke_func(ta.ctx, &arg, param);
	nn_ta_deinit(&ta);

	if (rc || arg.ret)
		goto finish;

	ret = 0;

finish:
	mutex_unlock(&ta_mutex);
	return ret;
}

unsigned long nn_ta_npu_usage_get(void)
{
	unsigned long idle = 0, total = 0, usage = 0;

	int rc = 0;
	struct tee_ioctl_invoke_arg arg = {};
	struct tee_param param[4] = {};
	struct nn_ta ta = {};

	mutex_lock(&ta_mutex);
	if (nn_ta_init(&ta, 1) != 0)
		goto finish;

	arg.func = PTA_NN_CMD_GET_NPU_USAGE;
	arg.session = ta.session;
	arg.num_params = 4;
	param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_OUTPUT;

	rc = tee_client_invoke_func(ta.ctx, &arg, param);
	idle = param[0].u.value.a;
	total = param[0].u.value.b;

	nn_ta_deinit(&ta);

	if (rc || arg.ret)
		goto finish;


	if ((total>0)&&(total>idle))
		usage=(total-idle)*100/total;

finish:
	mutex_unlock(&ta_mutex);
	return usage;
}
EXPORT_SYMBOL(nn_ta_npu_usage_est_start);
EXPORT_SYMBOL(nn_ta_npu_usage_get);

static int nn_ta_power_management(bool power_on)
{
	int ret = -EPERM;

	int rc = 0;
	struct tee_ioctl_invoke_arg arg = {};
	struct tee_param param[4] = {};
	struct nn_ta ta = {};

	mutex_lock(&ta_mutex);
	if (nn_ta_init(&ta, 1) != 0)
		goto finish;

	arg.func = PTA_NN_CMD_MANAGE_POWER;
	arg.session = ta.session;
	arg.num_params = 4;
	param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
	param[0].u.value.a = power_on;

	rc = tee_client_invoke_func(ta.ctx, &arg, param);
	nn_ta_deinit(&ta);

	if (rc || arg.ret)
		goto finish;

	ret = 0;

finish:
	mutex_unlock(&ta_mutex);
	return ret;
}

static int nn_user_ta_power_management(bool power_on)
{
	int rc = 0;
	struct tee_ioctl_invoke_arg arg = {};
	struct tee_param param[4] = {};
	struct nn_ta ta = {};

	if(aipq_start == 0)
		return -EPERM;

	if (nn_ta_init(&ta, 0) != 0)
		return -EPERM;

	arg.func = USER_TA_NN_CMD_MANAGE_POWER;
	arg.session = ta.session;
	arg.num_params = 4;
	param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
	param[0].u.value.a = power_on;


	rc = tee_client_invoke_func(ta.ctx, &arg, param);
	nn_ta_deinit(&ta);

	if (rc || arg.ret)
		return -EPERM;

	return 0;
}
int nn_ta_set_aipq_status(bool start)
{
	aipq_start = start;
	wake_up(&aipq_wq);

	return 0;
}
EXPORT_SYMBOL(nn_ta_set_aipq_status);

bool nn_ta_get_aipq_status(void)
{
	return aipq_start;
}
EXPORT_SYMBOL(nn_ta_get_aipq_status);

int nn_ta_wait_aipq_disabled(void)
{
	if (aipq_start == false)
		return 0;

	wait_event_timeout(aipq_wq, (aipq_start == false),
                                      msecs_to_jiffies(5000));

	return (aipq_start == false) ? 0 : -EPERM;

}
int nn_ta_suspend(void)
{
	nn_user_ta_power_management(0);
	return nn_ta_power_management(0);
}
int nn_ta_resume(void)
{
	nn_user_ta_power_management(1);
	return nn_ta_power_management(1);
}
EXPORT_SYMBOL(nn_ta_suspend);
EXPORT_SYMBOL(nn_ta_resume);

int nn_ta_open_drv(void)
{
	struct tee_ioctl_invoke_arg arg = {};
	struct tee_param param[4] = {};
	struct nn_ta ta = {};
	int ret = -EPERM;
	int rc = 0;

	mutex_lock(&ta_mutex);
	if (nn_ta_init(&ta, 1) != 0)
		goto finish;

	arg.func = PTA_NN_CMD_OPEN_DRV;
	arg.session = ta.session;
	arg.num_params = 4;

	rc = tee_client_invoke_func(ta.ctx, &arg, param);
	nn_ta_deinit(&ta);

	if (rc || arg.ret)
		goto finish;

	ret = 0;

finish:
	mutex_unlock(&ta_mutex);
	return ret;
}
EXPORT_SYMBOL(nn_ta_open_drv);

int nn_ta_close_drv(void)
{
	struct tee_ioctl_invoke_arg arg = {};
	struct tee_param param[4] = {};
	struct nn_ta ta = {};
	int ret = -EPERM;
	int rc = 0;

	mutex_lock(&ta_mutex);
	if (nn_ta_init(&ta, 1) != 0)
		goto finish;

	arg.func = PTA_NN_CMD_CLOSE_DRV;
	arg.session = ta.session;
	arg.num_params = 4;

	rc = tee_client_invoke_func(ta.ctx, &arg, param);
	nn_ta_deinit(&ta);

	if (rc || arg.ret)
		goto finish;

	ret = 0;

finish:
	mutex_unlock(&ta_mutex);
	return ret;
}
EXPORT_SYMBOL(nn_ta_close_drv);

int nn_ta_reset_hw(u32 vip_version)
{
	struct tee_ioctl_invoke_arg arg = {};
	struct tee_param param[4] = {};
	struct nn_ta ta = {};
	int ret = -EPERM;
	int rc = 0;

	mutex_lock(&ta_mutex);
	if (nn_ta_init(&ta, 1) != 0)
		goto finish;

	arg.func = PTA_NN_CMD_RESET_HW;
	arg.session = ta.session;
	arg.num_params = 4;
	param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
	param[0].u.value.a = vip_version;

	rc = tee_client_invoke_func(ta.ctx, &arg, param);
	nn_ta_deinit(&ta);

	if (rc || arg.ret)
		goto finish;

	ret = 0;

finish:
	mutex_unlock(&ta_mutex);
	return ret;
}
EXPORT_SYMBOL(nn_ta_reset_hw);

int nn_ta_init_hw(u32 vip_version, u32 clkgating_en, u32 axi_sram_size)
{
	struct tee_ioctl_invoke_arg arg = {};
	struct tee_param param[4] = {};
	struct nn_ta ta = {};
	int ret = -EPERM;
	int rc = 0;

	mutex_lock(&ta_mutex);
	if (nn_ta_init(&ta, 1) != 0) {
		rtd_pr_vpq_ai_emerg("[%s %d]PTA_NN_CMD_INIT_HW fails\n", __func__, __LINE__);
		goto finish;
	}

	arg.func = PTA_NN_CMD_INIT_HW;
	arg.session = ta.session;
	arg.num_params = 4;
	param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
	param[0].u.value.a = vip_version;
	param[0].u.value.b = clkgating_en;
	param[1].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
	param[1].u.value.a = axi_sram_size;

	rc = tee_client_invoke_func(ta.ctx, &arg, param);
	nn_ta_deinit(&ta);

	if (rc || arg.ret) {
		rtd_pr_vpq_ai_emerg("[%s %d]PTA_NN_CMD_INIT_HW fails\n", __func__, __LINE__);
		goto finish;
	}

	ret = 0;

finish:
	mutex_unlock(&ta_mutex);
	return ret;
}
EXPORT_SYMBOL(nn_ta_init_hw);

int nn_ta_submit_cmd(u32 vip_version, u32 cmd_physical, u32 cmd_size)
{
	struct tee_ioctl_invoke_arg arg = {};
	struct tee_param param[4] = {};
	struct nn_ta ta = {};
	int ret = -EPERM;
	int rc = 0;

	mutex_lock(&ta_mutex);
	if (nn_ta_init(&ta, 1) != 0)
		goto finish;

	arg.func = PTA_NN_CMD_SUBMIT;
	arg.session = ta.session;
	arg.num_params = 4;
	param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
	param[0].u.value.a = vip_version;
	param[0].u.value.b = cmd_physical;
	param[1].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
	param[1].u.value.a = cmd_size;

	rc = tee_client_invoke_func(ta.ctx, &arg, param);
	nn_ta_deinit(&ta);

	if (rc || arg.ret)
		goto finish;

	ret = 0;

finish:
	mutex_unlock(&ta_mutex);
	return ret;
}
EXPORT_SYMBOL(nn_ta_submit_cmd);

int nn_ta_read_hw_info(u32* chip_ver1, u32* chip_ver2, u32* chip_cid, u32* chip_date)
{
	struct tee_ioctl_invoke_arg arg = {};
	struct tee_param param[4] = {};
	struct nn_ta ta = {};
	int ret = -EPERM;
	int rc = 0;

	mutex_lock(&ta_mutex);
	if (nn_ta_init(&ta, 1) != 0)
		goto finish;

	arg.func = PTA_NN_CMD_GET_HW_INFO;
	arg.session = ta.session;
	arg.num_params = 4;
	param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_OUTPUT;
	param[1].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_OUTPUT;

	rc = tee_client_invoke_func(ta.ctx, &arg, param);
	nn_ta_deinit(&ta);

	if (rc || arg.ret)
		goto finish;

	*chip_ver1 = param[0].u.value.a;
	*chip_ver2 = param[0].u.value.b;
	*chip_cid  = param[1].u.value.a;
	*chip_date = param[1].u.value.b;

	ret = 0;
finish:
	mutex_unlock(&ta_mutex);
	return ret;
}
EXPORT_SYMBOL(nn_ta_read_hw_info);


