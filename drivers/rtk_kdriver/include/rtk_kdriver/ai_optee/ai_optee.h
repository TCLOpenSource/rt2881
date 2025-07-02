#ifndef SCALER_AI_OPTEE_H
#define SCALER_AI_OPTEE_H

#include <ioctrl/vpq/vpq_cmd_id.h>

typedef void (*AIPQ_INIT_FUNC)(void);

bool ai_optee_get_nn_tee_buf(NN_BUF_INFO_T *info);
bool ai_optee_release_nn_tee_buf(NN_BUF_INFO_T *info);
void ai_optee_release_all_tee_buf(void);

bool ai_optee_init(VIP_NN_CTRL *ctrl, VIP_NN_BUFFER_LEN *nn_buf_len_tbl_tbl, AIPQ_INIT_FUNC aipq_init_cb_ptr);
void ai_optee_deinit(void);

int ai_optee_update_secure_status(bool secure);

int ai_optee_buf_rw_trylock(void);
void ai_optee_buf_rw_unlock(void);

#endif

