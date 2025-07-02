#ifndef __NTP8918_H__
#define __NTP8918_H__

#define NTP8918_DEBUG                   0
#define NTP8918_ADDR_SIZE               (1)
#define NTP8918_DATA_SIZE               (1)

#define NTP8918_SOFT_MUTE_REG      (0x33)

void ntp8918_amp_reset(void);
void ntp8918_func(int bus_id, unsigned short addr);
int ntp8918_dump_all(const char *buf, int amp_i2c_id,unsigned short slave_addr);
int ntp8918_mute_set(int on_off, int amp_i2c_id, unsigned short slave_addr);
int ntp8918_param_get(unsigned char *reg, int amp_i2c_id, unsigned short slave_addr);
int ntp8918_param_set(unsigned char *data_wr, int amp_i2c_id, unsigned short slave_addr);
#endif


