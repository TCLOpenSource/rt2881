#include <linux/kernel.h>
#include <linux/err.h>
#include <ai_optee/nn_svp.h>


static svp_func_type_t  nnip_open_protect;
static svp_func_type_t  nnip_close_protect;

void nnip_svp_func_init(svp_func_type_t enable_func, svp_func_type_t disable_func)
{
	nnip_open_protect = enable_func;
	nnip_close_protect = disable_func;
}
EXPORT_SYMBOL(nnip_svp_func_init);

int nnip_svp_enable_protection(unsigned int addr, unsigned int size)
{
	if (!nnip_open_protect)
		return -EINVAL;

	return nnip_open_protect(addr, size, TYPE_SVP_PROTECT_NNIP);
}

int nnip_svp_disable_protection(unsigned int addr, unsigned int size)
{
	if (!nnip_close_protect)
		return -EINVAL;

	return nnip_close_protect(addr, size, TYPE_SVP_PROTECT_NNIP);
}
