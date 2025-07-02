/*
 * ad82088d.h  --  ad82088d ALSA SoC Audio driver
 *
 * Copyright 1998 Elite Semiconductor Memory Technology
 *
 * Author: ESMT Audio/Power Product BU Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public  version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __AD82088D_H__
#define __AD82088D_H__

#define AD82088D_ADDR_SIZE                1
#define AD82088D_DATA_SIZE                1
#define AD82088D_END_FLAG   (0xff)
//#define AD82088D_DEBUG      (0)


void ad82088d_amp_reset(void);
void ad82088d_func(int amp_i2c_id, unsigned short addr);
int ad82088d_mute_set(int on_off, int amp_i2c_id, unsigned short slave_addr);
void ad82088d_amp_mute_set(int value);
int ad82088d_param_get(unsigned char *reg, int amp_i2c_id, unsigned short slave_addr);
int ad82088d_param_set(unsigned char *data_wr, int amp_i2c_id, unsigned short slave_addr);


	

#endif /* __AD82088D_H__ */
