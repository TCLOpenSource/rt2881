#include <linux/types.h>
#include <rtd_log/rtd_module_log.h>
#include <ioctrl/vpq/vpq_cmd_id.h>
#include <vgip_isr/scalerAI.h>
#include <ai_optee/nn_ta.h>
#include <ai_optee/nn_svp.h>
#include <ai_optee/ai_optee.h>


#define ROUND_UP(x, y) (((x) + (y-1))&(~(y-1)))

DEFINE_SPINLOCK(vip_nn_buf_lock);


/*
 * == 0 : buffer not ready
 * == 1 : memory allocation start
 * == 2 : memory allocation complete
 * refcnt++ : buffer r/w start
 * refcnt-- : buffer r/w complete
 * == 1 : memory release start
 * == 0 : buffer release complete
 * */
u32 vip_nn_buf_refcnt;

AIPQ_INIT_FUNC aipq_init_cb;

typedef struct {
	u32 phy_addr;
	u32 size;
	u8* vir_addr;
	u8* vir_uncached_addr;
	u8 protected;
} CMA_BUF_INFO_T;

enum {
	VIPLITE_HEAP_BIT = (1 << 0),
	DATA_BIT         = (1 << 1),
	SYS_HEAP_BIT     = (1 << 2),
};

static DEFINE_MUTEX(svp_flag_mutex);
static u8 video_svp_flag = (VIPLITE_HEAP_BIT | SYS_HEAP_BIT);
static CMA_BUF_INFO_T nn_optee;
static CMA_BUF_INFO_T sys_heap;
static CMA_BUF_INFO_T vip_nn_buffer;
static CMA_BUF_INFO_T nb_buffer;

static VIP_NN_BUFFER_LEN *nn_buf_len_tbl;
static VIP_NN_CTRL *nn_ctrl;
static int ai_optee_set_protection(u8 secure);
static int malloc_vip_nn_buf(void);
static void release_vip_nn_buf(void);
static bool get_nn_tee_buf(CMA_BUF_INFO_T *cma, NN_BUF_INFO_T *info, bool needUncachedMem, bool needCachedMem)
{
	unsigned long phy_addr = INVALID_VAL;

	if (cma->size > 0) {
		if (cma->phy_addr == 0)
			return 0;

		info->pa = cma->phy_addr;
		info->size = cma->size;
		return 1;

	}

	info->size = ROUND_UP(info->size, 0x200000);

	if (needUncachedMem)
		cma->vir_addr = (u8*)dvr_malloc_uncached_specific(info->size, GFP_DCU2_FIRST, (void**)&cma->vir_uncached_addr);
	else if (needCachedMem)
		cma->vir_addr = (u8*)dvr_malloc_specific(info->size, GFP_DCU2_FIRST);
	else
		phy_addr = pli_malloc(info->size, GFP_DCU2_FIRST);

	if (cma->vir_addr == 0 && phy_addr == INVALID_VAL) {
		rtd_pr_vpq_ai_emerg("[%s %d] out of memory: size=%d\n", __func__, __LINE__, info->size);
		return 0;
	}

	if (cma->vir_addr)
		phy_addr = dvr_to_phys(cma->vir_addr);

	cma->size = info->size;
	cma->phy_addr = phy_addr;
	info->pa = cma->phy_addr;
	return 1;

}

static u32 get_nn_buf_total_size(u32* flag_len, u32* info_len, u32* data_len)
{
	u32 i;
	u32 flag_size = 0, info_size = 0, data_size = 0;

	for (i=0; i < VIP_NN_BUFFER_NUM; i++) {
		data_size += ROUND_UP(nn_buf_len_tbl[i].data_len, 256);
		flag_size += ROUND_UP(nn_buf_len_tbl[i].flag_len, 64);
		info_size += ROUND_UP(nn_buf_len_tbl[i].info_len, 64);
	}
	data_size = ROUND_UP(data_size, 4096);

	if (flag_len)   *flag_len = flag_size;
	if (info_len)   *info_len = info_size;
	if (data_len)   *data_len = data_size;


	return (data_size+flag_size+info_size);
}

static bool release_nn_tee_buf(CMA_BUF_INFO_T *cma)
{
	if (cma->vir_addr)
		dvr_free(cma->vir_addr);
	else if (cma->phy_addr)
		pli_free(cma->phy_addr);

	memset(cma, 0, sizeof(CMA_BUF_INFO_T));
	return 1;
}

bool ai_optee_get_nn_tee_buf(NN_BUF_INFO_T *info)
{
	VIP_NN_MEMORY_CTRL *mem;

	switch(info->buf_id)
	{
	case VIPLITE_HEAP:
		mutex_lock(&svp_flag_mutex);
		if(!get_nn_tee_buf(&nn_optee, info, 0, 0)) {
			mutex_unlock(&svp_flag_mutex);
			return 0;
		}

		ai_optee_set_protection(video_svp_flag);
		mutex_unlock(&svp_flag_mutex);

		nn_ta_set_aipq_status(1);
		return 1;

	case VIPLITE_SYS_HEAP:
		mutex_lock(&svp_flag_mutex);
		if(!get_nn_tee_buf(&sys_heap, info, 0, 0)) {
			mutex_unlock(&svp_flag_mutex);
			return 0;
		}

		ai_optee_set_protection(video_svp_flag);
		mutex_unlock(&svp_flag_mutex);

		return 1;

	case NET_DATA_ALL:
		if (vip_nn_buffer.phy_addr == 0)
			malloc_vip_nn_buf();

		mutex_lock(&svp_flag_mutex);
		ai_optee_set_protection(video_svp_flag);
		mutex_unlock(&svp_flag_mutex);

		if (vip_nn_buffer.phy_addr) {
			info->pa = vip_nn_buffer.phy_addr;
			info->size = vip_nn_buffer.size;
			return 1;
		}
		return 0;
	case NB_FILE:
		return get_nn_tee_buf(&nb_buffer, info, 0, 0);
	case MODEL_HEAP:
	#ifdef CONFIG_ARCH_RTK2885P
		return 0;
	#else
		info->size = carvedout_buf_query(CARVEDOUT_SCALER_NN, (void **)&info->pa);
		return (info->pa != 0);
	#endif
	default:
		break;
	}

	if (nn_ctrl == 0 || info->net_id >= VIP_NN_BUFFER_MAX)
		return 0;

	switch(info->buf_id)
	{
	case NET_DATA:
		mem = &nn_ctrl->NN_data_Addr[info->net_id];
		break;
	case NET_INFO:
		mem = &nn_ctrl->NN_info_Addr[info->net_id];
		break;
	case NET_FLAG:
		mem = &nn_ctrl->NN_flag_Addr[info->net_id];
		break;
	default:
		return 0;
	}

	info->pa = mem->phy_addr_align;
	info->size = mem->size;
	return 1;
}

bool ai_optee_release_nn_tee_buf(NN_BUF_INFO_T *info)
{
	switch(info->buf_id)
	{
	case VIPLITE_HEAP:
		mutex_lock(&svp_flag_mutex);
		ai_optee_set_protection(0);
		mutex_unlock(&svp_flag_mutex);

		release_nn_tee_buf(&nn_optee);
		release_nn_tee_buf(&sys_heap);
		nn_ta_set_aipq_status(0);
		return 1;

	case NB_FILE:
		return release_nn_tee_buf(&nb_buffer);

	case NET_DATA_ALL:
		release_vip_nn_buf();
		return 1;
	default:
		return 1;
	}
}
void ai_optee_release_all_tee_buf(void)
{

	mutex_lock(&svp_flag_mutex);
	ai_optee_set_protection(0);
	mutex_unlock(&svp_flag_mutex);

	release_nn_tee_buf(&nn_optee);
	release_nn_tee_buf(&sys_heap);
	release_nn_tee_buf(&nb_buffer);

	release_vip_nn_buf();
}

static bool assign_vip_nn_buffer(void)
{
	u32 i, data_size;
	u32 pa = vip_nn_buffer.phy_addr;
	u32 remain_size = vip_nn_buffer.size;
	u8* cached_addr = vip_nn_buffer.vir_addr;
	u8* noncached_addr = vip_nn_buffer.vir_uncached_addr;

	rtd_pr_vpq_ai_info("nn buf start =0x%x\n", vip_nn_buffer.phy_addr);
	rtd_pr_vpq_ai_info("nn buf size =%d\n", vip_nn_buffer.size);
	rtd_pr_vpq_ai_info("nn buf cached start =%px\n", vip_nn_buffer.vir_addr);
	rtd_pr_vpq_ai_info("nn buf noncached start =%px\n", vip_nn_buffer.vir_uncached_addr);

	if (cached_addr == 0 || noncached_addr == 0)
		return 0;

	if (nn_ctrl->NN_flag_Addr[0].pVir_addr_align)
		return 1;

	//data
	get_nn_buf_total_size(0, 0, &data_size);
	for (i=0; i < VIP_NN_BUFFER_NUM; i++) {
		VIP_NN_MEMORY_CTRL * mem = &nn_ctrl->NN_data_Addr[i];

		if (nn_buf_len_tbl[i].data_len == 0)
			continue;

		mem->phy_addr_align = pa;
		mem->size = ROUND_UP(nn_buf_len_tbl[i].data_len, 256);
		mem->pVir_addr_align = 0;

		pa += mem->size;
		rtd_pr_vpq_ai_info("NN_data_Addr[%d] start =0x%x\n", i, mem->phy_addr_align);
	}

	pa = vip_nn_buffer.phy_addr+data_size;
	remain_size = vip_nn_buffer.size-data_size;
	cached_addr = vip_nn_buffer.vir_addr+data_size;
	noncached_addr = vip_nn_buffer.vir_uncached_addr+data_size;

	//flag
	for (i=0; i < VIP_NN_BUFFER_NUM; i++) {
		VIP_NN_MEMORY_CTRL * mem = &nn_ctrl->NN_flag_Addr[i];

		if (nn_buf_len_tbl[i].flag_len == 0)
			continue;

		mem->phy_addr_align = pa;
		mem->size = ROUND_UP(nn_buf_len_tbl[i].flag_len, 64);
		mem->pVir_addr_align = (unsigned int *)noncached_addr;
		memset(noncached_addr, 0, mem->size);

		pa += mem->size;
		remain_size -= mem->size;
		noncached_addr += mem->size;
		cached_addr += mem->size;
	}


	//info
	for (i=0; i < VIP_NN_BUFFER_NUM; i++) {
		VIP_NN_MEMORY_CTRL * mem = &nn_ctrl->NN_info_Addr[i];

		if (nn_buf_len_tbl[i].info_len == 0)
			continue;

		mem->phy_addr_align = pa;
		mem->size = ROUND_UP(nn_buf_len_tbl[i].info_len, 64);
		mem->pVir_addr_align = (unsigned int *)cached_addr;
		memset(noncached_addr, 0, mem->size);

		pa += mem->size;
		remain_size -= mem->size;
		cached_addr += mem->size;
		noncached_addr += mem->size;
	}


	return 1;
}

bool ai_optee_init(VIP_NN_CTRL *ctrl, VIP_NN_BUFFER_LEN *nn_buf_len_tbl_tbl, AIPQ_INIT_FUNC aipq_init_cb_ptr)
{

	//NN_BUF_INFO_T info;
	unsigned long nn_pa = 0, nn_size = 0;
	nn_size = carvedout_buf_query(CARVEDOUT_SCALER_NN, (void **)&nn_pa);

	nn_ctrl = ctrl;
	nn_buf_len_tbl = nn_buf_len_tbl_tbl;
	aipq_init_cb = aipq_init_cb_ptr;

	return 1;
#if 0
	info.buf_id = VIPLITE_HEAP;
	info.size = VIPLITE_HEAP_SIZE + NN_TA_HEAP_SIZE;// + MODEL_HEAP_SIZE;
	if(nn_size == 0)
		info.size += 2*1024*1024;
	if(get_nn_tee_buf(&nn_optee, &info, 0, 0) == 0)
		return 0;


	return (malloc_vip_nn_buf() == 0);
#endif
}
void ai_optee_deinit(void)
{
	ai_optee_release_all_tee_buf();
}

int ai_optee_update_secure_status(bool secure)
{
	int ret;

	mutex_lock(&svp_flag_mutex);
	if(secure)
		video_svp_flag = (VIPLITE_HEAP_BIT | DATA_BIT | SYS_HEAP_BIT);
	else
		video_svp_flag = (VIPLITE_HEAP_BIT | SYS_HEAP_BIT);

	ret = ai_optee_set_protection(video_svp_flag);
	mutex_unlock(&svp_flag_mutex);

	return ret;
}
static int ai_optee_apply_protection(bool enable, u32 phy_addr, u32 size)
{
	if (enable)
		nnip_svp_enable_protection(phy_addr, size);
	else
		nnip_svp_disable_protection(phy_addr, size);

	rtd_pr_vpq_ai_emerg("%s nn buf protection 0x%x~0x%x\n", enable ? "enable" : "disable", phy_addr, phy_addr+size);
	return 0;
}
int ai_optee_set_protection(u8 flag)
{
	u32 size = 0, do_protection;

	do_protection = ((flag & VIPLITE_HEAP_BIT) != 0);
	do {
		if (nn_optee.phy_addr == 0)
			break;

		if (nn_optee.protected == do_protection)
			break;

		size = VIPLITE_HEAP_SIZE-4096;
		size = size & (~0xfff);
		ai_optee_apply_protection(do_protection, nn_optee.phy_addr+4096, size);
		nn_optee.protected = do_protection;

	} while(0);

	do_protection = ((flag & SYS_HEAP_BIT) != 0);
	do {
		if (sys_heap.phy_addr == 0)
			break;

		if (sys_heap.protected == do_protection)
			break;

		size = NN_TA_HEAP_SIZE & (~0xfff);
		ai_optee_apply_protection(do_protection, sys_heap.phy_addr, size);
		sys_heap.protected = do_protection;

	} while(0);

	do_protection = ((flag & DATA_BIT) != 0);
	do {
		u32 pa;

		if (nn_ctrl->NN_data_Addr[0].phy_addr_align == 0)
			break;

		if (vip_nn_buffer.protected == do_protection)
			break;

		size = 0;
		get_nn_buf_total_size(0, 0, &size);
		size = size & (~0xfff);
		pa = ROUND_UP(nn_ctrl->NN_data_Addr[0].phy_addr_align, 4096);
		ai_optee_apply_protection(do_protection, pa, size);
		vip_nn_buffer.protected = do_protection;

	} while(0);

	return 0;
}

EXPORT_SYMBOL(ai_optee_init);
EXPORT_SYMBOL(ai_optee_deinit);
EXPORT_SYMBOL(ai_optee_get_nn_tee_buf);
EXPORT_SYMBOL(ai_optee_release_nn_tee_buf);
EXPORT_SYMBOL(ai_optee_update_secure_status);

int malloc_vip_nn_buf(void)
{
	int malloc_done, do_malloc;
	NN_BUF_INFO_T info;

	do {
		spin_lock_bh(&vip_nn_buf_lock);
		malloc_done = (vip_nn_buf_refcnt >= 2);
		do_malloc = (vip_nn_buf_refcnt == 0);
		if (do_malloc) {
			vip_nn_buf_refcnt++;
		}
		spin_unlock_bh(&vip_nn_buf_lock);

		if (malloc_done)
			return 0;

		if (!do_malloc)
			msleep(10);
	} while (!do_malloc);

	info.buf_id = NET_DATA_ALL;
	info.size = get_nn_buf_total_size(0, 0, 0);
	if(get_nn_tee_buf(&vip_nn_buffer, &info, 1, 1))
		assign_vip_nn_buffer();

	if (aipq_init_cb)
		aipq_init_cb();

	spin_lock_bh(&vip_nn_buf_lock);
	vip_nn_buf_refcnt++;
	spin_unlock_bh(&vip_nn_buf_lock);

	return 0;
}

void release_vip_nn_buf(void)
{
	int do_release;

	spin_lock_bh(&vip_nn_buf_lock);
	if (vip_nn_buf_refcnt == 0) {
		spin_unlock_bh(&vip_nn_buf_lock);
		return;
	}
	vip_nn_buf_refcnt--;

	do_release = (vip_nn_buf_refcnt == 1);
	spin_unlock_bh(&vip_nn_buf_lock);

	while (!do_release) {
		msleep(10);

		spin_lock_bh(&vip_nn_buf_lock);
		do_release = (vip_nn_buf_refcnt == 1);
		spin_unlock_bh(&vip_nn_buf_lock);
	}

	release_nn_tee_buf(&vip_nn_buffer);

	memset(nn_ctrl->NN_flag_Addr, 0, sizeof(VIP_NN_MEMORY_CTRL)*VIP_NN_BUFFER_NUM);
	memset(nn_ctrl->NN_info_Addr, 0, sizeof(VIP_NN_MEMORY_CTRL)*VIP_NN_BUFFER_NUM);
	memset(nn_ctrl->NN_data_Addr, 0, sizeof(VIP_NN_MEMORY_CTRL)*VIP_NN_BUFFER_NUM);

	spin_lock_bh(&vip_nn_buf_lock);
	vip_nn_buf_refcnt--;
	spin_unlock_bh(&vip_nn_buf_lock);
}
int ai_optee_buf_rw_trylock(void)
{
	spin_lock_bh(&vip_nn_buf_lock);

	if (vip_nn_buf_refcnt < 2) {

		spin_unlock_bh(&vip_nn_buf_lock);
		return -EFAULT;
	}

	vip_nn_buf_refcnt++;
	spin_unlock_bh(&vip_nn_buf_lock);
	return 0;
}
void ai_optee_buf_rw_unlock(void)
{
	spin_lock_bh(&vip_nn_buf_lock);
	vip_nn_buf_refcnt--;
	spin_unlock_bh(&vip_nn_buf_lock);
}
EXPORT_SYMBOL(ai_optee_buf_rw_trylock);
EXPORT_SYMBOL(ai_optee_buf_rw_unlock);

