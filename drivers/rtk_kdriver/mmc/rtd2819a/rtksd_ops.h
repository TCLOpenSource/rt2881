/*
 *  Copyright (C) 2010 Realtek Semiconductors, All Rights Reserved.
 *
 */




#ifndef __RTKSD_OPS_H
#define __RTKSD_OPS_H

int mmc_fast_write(unsigned int blk_addr,
				unsigned int data_size,
				unsigned char *buffer);

int mmc_fast_read(unsigned int blk_addr,
				unsigned int data_size,
				unsigned char *buffer);

#endif

/* end of file */


