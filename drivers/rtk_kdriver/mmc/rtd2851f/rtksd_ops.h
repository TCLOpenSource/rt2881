#ifndef __RTKSD_OPS_H__
#define __RTKSD_OPS_H__

void rtkcr_set_mis_gpio(u32 gpio_num, u8 ctl);
void rtkcr_set_iso_gpio(u32 gpio_num, u8 ctl);
u8 get_emmc_type_select(const char *drv_name);


char *rtkcr_parse_token(const char *parsed_string, const char *token);
void rtkcr_chk_param(u32 * pparam, u32 len, u8 * ptr);
int rtkcr_chk_VerA(void);

int mmc_fast_write(unsigned int blk_addr,
                    unsigned int data_size,
                    unsigned char *buffer);

int mmc_fast_read(unsigned int blk_addr,
                    unsigned int data_size,
                    unsigned char *buffer);

#endif
