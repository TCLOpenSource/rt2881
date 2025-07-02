#ifndef __NN_SVP_H__
#define __NN_SVP_H__

#include <rtk_vdec_svp.h>
void nnip_svp_func_init(svp_func_type_t enable_func, svp_func_type_t disable_func);
int nnip_svp_enable_protection(unsigned int addr, unsigned int size);
int nnip_svp_disable_protection(unsigned int addr, unsigned int size);

#endif //__NN_SVP_H__

