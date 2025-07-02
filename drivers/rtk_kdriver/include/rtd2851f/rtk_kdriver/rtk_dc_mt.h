#ifndef __DC_MT_H__
#define __DC_MT_H__
#ifndef BUILD_QUICK_SHOW

#include <hw_monitor/rtk_dc_mt_common.h>

#else

// Below definition are fake API in quick show for CMA flow. Don't care about functionality.
#define DCMT_MAX_REGION_CNT 10
typedef struct dcmt_cma_desc_s
{
    unsigned int pa_start[DCMT_MAX_REGION_CNT];
    unsigned int pa_size[DCMT_MAX_REGION_CNT];
    unsigned int region_cnt;
    unsigned int sorted;
}dcmt_cma_desc_t;

int dcmt_cma_mdomain_set(dcmt_cma_desc_t *desc);
int dcmt_cma_mdomain_unset(dcmt_cma_desc_t *desc);
int dcmt_cma_mdomain_unset_without_deferfree(void);
int dcmt_cma_rtnr_set(dcmt_cma_desc_t *desc);
int dcmt_cma_rtnr_unset(dcmt_cma_desc_t *desc);
// Above definition are fake API in quick show for CMA flow. Don't care about functionality.

#endif /* BUILD_QUICK_SHOW */
#endif
