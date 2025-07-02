#ifndef __SPI_RTK_H__
#define __SPI_RTK_H__

#include <linux/spi/spi.h>
#include <rbus/sb2_reg.h>
#include <rbus/md_reg.h>
#include <rtk_kdriver/io.h>


/************************************************************************
 *  Macro Define 
 ************************************************************************/
#define SPI_REG_NUM                 0x8
#define SPI_REG_BASE                SB2_SFC_OPCODE_reg

#define POS_LATCH_FALL              0
#define POS_LATCH_RISE              1

/*MD define*/
#define FLASH_2_DDR                 0
#define DDR_2_FLASH                 1

#define SET_DMA_LEN_512             2

/************************************************************************
 *  Define misc
 ************************************************************************/
#define SN_SYNC                     __asm__ __volatile__ ("DSB sy;")


/***********************************************************************/

/***********************************************************************/
struct spi_master_set_t
{
	unsigned int spi_reg;
	unsigned int spi_port;
	unsigned int spi_type;		/*0: spi flash, 1: spi general device*/

	unsigned int clock_div;		/*current clock div*/
	unsigned int delay_sel;		/*current clock delay sel*/

	unsigned int current_clock;    /*current clock*/
	unsigned int current_mode;   /*current mode*/
	unsigned char current_bits_per_word; /*current bits_per_word*/
	
	unsigned int max_clock;        /*support max clock*/
};

typedef enum {
    SM_READY,
    SM_WORKING,
    SM_SUSPEND,
}spi_master_device_state_t;

/* runtime info for spi master */
struct rtk_spi_master {
	struct spi_master *master;     /* SPI framework hookup */
	
	unsigned long base_addr;       /*SPI base address*/

	/*spi setting info*/
	struct spi_master_set_t spi_set[2];
	
	/* device get and release control */
	spinlock_t spi_lock;
	wait_queue_head_t wq;	
	spi_master_device_state_t	state;

	spinlock_t spi_lock_priv;
	unsigned long spi_lock_flags;

	/*register value store buffer for suspend and resume */
	unsigned int *reg_array;
	
	/*transfer datas*/
	int (*transfer)(struct rtk_spi_master *master, const unsigned char *tx_data, unsigned char *rx_data, unsigned int len);
	/*receive data*/
	int (*receive)(struct rtk_spi_master *master, const unsigned char *tx_data, unsigned char *rx_data, unsigned int len);
};

#endif
