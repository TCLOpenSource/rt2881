#ifndef NN_TA_H
#define NN_TA_H


int nn_ta_npu_usage_est_start(void);
unsigned long nn_ta_npu_usage_get(void);

int nn_ta_suspend(void);
int nn_ta_resume(void);
int nn_ta_set_aipq_status(bool start);
bool nn_ta_get_aipq_status(void);
int nn_ta_wait_aipq_disabled(void);

int nn_ta_open_drv(void);
int nn_ta_close_drv(void);
int nn_ta_reset_hw(u32 vip_version);
int nn_ta_init_hw(u32 vip_version, u32 clkgating_en, u32 axi_sram_size);
int nn_ta_submit_cmd(u32 vip_version, u32 cmd_physical, u32 cmd_size);
int nn_ta_read_hw_info(u32* chip_ver1, u32* chip_ver2, u32* chip_cid, u32* chip_date);

#endif
