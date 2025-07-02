/*********************************************************
*
*   Copyright(c) 2016 Realtek Semiconductor Corp. All rights reserved.
*
*   @author realtek.com
*
**********************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/delay.h>
#include <linux/dma-direction.h>
#include <linux/dma-mapping.h>
#include <md/rtk_md.h>
#include <asm/cacheflush.h>
#include <rtd_log/rtd_module_log.h>

#include <rbus/sb2_reg.h>
#include <rbus/md_reg.h>

#include <rtk_kdriver/rtk_semaphore.h>
#include "spi-rtk.h"

/*********************************************************
*
*	macro definations
*
*********************************************************/
#define DRV_NAME		"rtk_spi_master"

#define BUSH_CLOCK		600	/*define in sb2 spec*/

/*********************************************************
*
*	variable definations
*
*********************************************************/
static struct rtk_spi_master *g_spi_master = NULL;

#if IS_ENABLED(CONFIG_RTK_KDRV_SPI)
extern unsigned long SPI_BASE_ADDR;
#else
unsigned long SPI_BASE_ADDR = 0;
#endif

/*********************************************************
*
*	function
*
*********************************************************/
static int rtk_spi_master_get_device(struct rtk_spi_master *master, int new_state)
{
#if 0
	DECLARE_WAITQUEUE(wait, current);

	/*Grab the lock and see if the device is available*/
	while (1) {
		spin_lock(&master->spi_lock);
		if (master->state == SM_READY) {
			master->state = new_state;
			spin_unlock(&master->spi_lock);
			break;
		}

		set_current_state(TASK_UNINTERRUPTIBLE);
		add_wait_queue(&master->wq, &wait);
		spin_unlock(&master->spi_lock);
		schedule();
		remove_wait_queue(&master->wq, &wait);
	}
	return 0;
#else
	if(master == NULL || new_state > 0xFF)
		return -1;
	
	_rtd_hwsem_lock(SEMA_HW_SEM_6, SEMA_HW_SEM_6_SCPU_0);
	return 0;
#endif
}

static void rtk_spi_master_release_device(struct rtk_spi_master *master)
{
#if 0
	/* Release the chip */
	spin_lock(&master->spi_lock);
	master->state = SM_READY;
	wake_up(&master->wq);
	spin_unlock(&master->spi_lock);
#else
	if(master == NULL)
		return;

	_rtd_hwsem_unlock(SEMA_HW_SEM_6, SEMA_HW_SEM_6_SCPU_0);
#endif
}

static void rtk_spi_master_set_delay_sel(unsigned int delay_sel)
{
	rtd_maskl(SB2_SFC_POS_LATCH_reg, ~SB2_SFC_POS_LATCH_delay_sel_mask, SB2_SFC_POS_LATCH_delay_sel(delay_sel));
}

static void rtk_spi_master_set_pos_latch(unsigned int pos_latch)
{
	rtd_maskl(SB2_SFC_POS_LATCH_reg, ~SB2_SFC_POS_LATCH_pos_latch_mask, SB2_SFC_POS_LATCH_pos_latch(pos_latch));
}

static int rtk_spi_master_set_clk(unsigned int clk)
{
	unsigned int clk_div  = 0;

	clk_div = BUSH_CLOCK / clk - 1;	
	rtd_maskl(SB2_SFC_SCK_reg, ~SB2_SFC_SCK_fdiv_mask, SB2_SFC_SCK_fdiv(clk_div));

	return 0;
}

static void rtk_spi_master_controler_init(void)
{
	/*set spi mode0*/
	rtd_maskl(SB2_SFC_SCK_reg, ~SB2_SFC_SCK_mode_mask, SB2_SFC_SCK_mode(0));

	/*HW SD provide the value :   Phcnt=0x60 / Plcnt=0xF(default) / tdt=0x39 */
	rtd_maskl(SB2_SFC_CE_reg, ~SB2_SFC_CE_phcnt_mask, SB2_SFC_CE_phcnt(0x60)); 
	rtd_maskl(SB2_SFC_CE_reg, ~SB2_SFC_CE_plcnt_mask, SB2_SFC_CE_plcnt(0xF));
	rtd_maskl(SB2_SFC_CE_reg, ~SB2_SFC_CE_tdt_mask, SB2_SFC_CE_tdt(0x39));
	
	/*set delay_sel init value*/
	rtk_spi_master_set_delay_sel(0x4);

	/*set pos latch falling*/
	rtk_spi_master_set_pos_latch(0x0);
}


/**********************************************************************
**
**  For multi spi device support, switch to right port
**
**  port0: 2'b00 : non-ROM boot
**  port1: 2'b10 : FCIC
**  port2: 2'b01, 2'b11: Panel/LED
***********************************************************************/
static void rtk_spi_master_port0_setting(struct spi_master_set_t *pset)
{
	/*FLASH: pinmux setting*/
	rtd_part_outl(0xB8000898,7,4,0x0);//SPI_WP,:R647, Ps_Reg:gpio_61_ps
	rtd_part_outl(0xB800089c,31,28,0x0);//SPI_DI,:R649, Ps_Reg:gpio_62_ps
	rtd_part_outl(0xB800089c,15,12,0x0);//SPI_HOLD,:R651, Ps_Reg:gpio_64_ps
	rtd_part_outl(0xB800089c,23,20,0x0);//SPI_CS_N,:R655, Ps_Reg:gpio_63_ps
	rtd_part_outl(0xB80008a0,31,28,0x0);//SPI_DO,:R656, Ps_Reg:gpio_66_ps
	rtd_part_outl(0xB800089c,7,4,0x0);//SPI_SCLK,:R659, Ps_Reg:gpio_65_ps

	/*port setting*/
	rtd_outl(SB2_SFC_WP_reg, SB2_SFC_WP_ce_n(0x0) | SB2_SFC_WP_write_en2(1));
}

static void rtk_spi_master_port1_setting(struct spi_master_set_t *pset)
{
	/*FCIC: pinmux setting*/
	rtd_part_outl(0xB800089c,23,20,0x1);//SPI_FCIC_CS_N,:R655, Ps_Reg:gpio_63_ps
	rtd_part_outl(0xB800089c,7,4,0x1);//SPI_FCIC_SCLK,:R659, Ps_Reg:gpio_65_ps
	rtd_part_outl(0xB80008c8,10,8,0x0);//SPI_FCIC_DI_src0,:input mux
	rtd_part_outl(0xB800089c,31,28,0x1);//SPI_FCIC_DI,:R649, Ps_Reg:gpio_62_ps
	rtd_part_outl(0xB80008c8,6,4,0x0);//SPI_FCIC_DO_src0,:input mux
	rtd_part_outl(0xB80008a0,31,28,0x1);//SPI_FCIC_DO,:R656, Ps_Reg:gpio_66_ps

	/*port setting*/
	rtd_outl(SB2_SFC_WP_reg, SB2_SFC_WP_ce_n(0x2) | SB2_SFC_WP_write_en2(1));
}

static void rtk_spi_master_port2_setting(struct spi_master_set_t *pset)
{
	/*PLED: pinmux setting*/
	rtd_part_outl(0xB800089c,31,28,0x4);//SPI_PLED_DI,:R649, Ps_Reg:gpio_62_ps
	rtd_part_outl(0xB800089c,23,20,0x4);//SPI_PLED_CS_N,:R655, Ps_Reg:gpio_63_ps
	rtd_part_outl(0xB80008a0,31,28,0x4);//SPI_PLED_DO,:R656, Ps_Reg:gpio_66_ps
	rtd_part_outl(0xB800089c,7,4,0x4);//SPI_PLED_SCLK,:R659, Ps_Reg:gpio_65_ps

	/*port setting*/
	rtd_outl(SB2_SFC_WP_reg, SB2_SFC_WP_ce_n(0x3) | SB2_SFC_WP_write_en2(1));
}

static int rtk_spi_master_port_switch(struct spi_master_set_t *pset)
{
	/*disable auto cmd*/
	rtd_outl(SB2_SFC_EN_WR_reg, SB2_SFC_EN_WR_wt_prog_en(0) | SB2_SFC_EN_WR_wr_en_op(0x00));
	rtd_outl(SB2_SFC_WAIT_WR_reg, SB2_SFC_WAIT_WR_wt_prog_done(0) | SB2_SFC_WAIT_WR_rdsr_op(0x00));

	/*set pinmux & driver strength & port*/
	if(pset->spi_port== 0){
		rtk_spi_master_port0_setting(pset);
	}else if(pset->spi_port == 1){
		rtk_spi_master_port1_setting(pset);
	}else {
		rtk_spi_master_port2_setting(pset);
	}

	/*set clock & delay_sel*/
	rtk_spi_master_set_delay_sel(pset->delay_sel);
	rtk_spi_master_set_clk(pset->current_clock/1000000);

	/*set spi mode*/
	switch (pset->current_mode & (SPI_CPHA | SPI_CPOL)) {
		case SPI_MODE_0:
			rtd_maskl(SB2_SFC_SCK_reg, ~SB2_SFC_SCK_mode_mask, SB2_SFC_SCK_mode(0));
			break;
		case SPI_MODE_3:
			rtd_maskl(SB2_SFC_SCK_reg, ~SB2_SFC_SCK_mode_mask, SB2_SFC_SCK_mode(1));
			break;
	}

	return 0;
}

/**************************************************************
**
**	rtk spi master priv interface
**
***************************************************************/
static void rtk_spi_master_read_md(unsigned long from, unsigned int len, const unsigned char *buf)
{
	smd_checkComplete();

	spin_lock_irqsave(&g_spi_master->spi_lock_priv, g_spi_master->spi_lock_flags);
	
	// set DDR and flash addr
	rtd_outl(MD_MD_FDMA_DDR_SADDR_reg, (unsigned int)virt_to_phys(buf));
	rtd_outl(MD_MD_FDMA_FL_SADDR_reg, MD_MD_FDMA_FL_SADDR_fdma_fl_saddr((unsigned int)from));
	rtd_outl(MD_MD_FDMA_CTRL2_reg, MD_MD_FDMA_CTRL2_fdma_length(len) | MD_MD_FDMA_CTRL2_fdma_dir(FLASH_2_DDR) | \
		MD_MD_FDMA_CTRL2_fdma_max_xfer(SET_DMA_LEN_512) | MD_MD_FDMA_CTRL2_fdma_swap(0) | \
		MD_MD_FDMA_CTRL2_fl_mapping_mode(1)); // forward map, no_swap, DMA LEN, FLASH to DDR

	dmac_flush_range(buf, buf + len);
	outer_flush_range(virt_to_phys(buf), virt_to_phys(buf + len));

	SN_SYNC;
	rtd_outl(MD_MD_FDMA_CTRL1_reg, MD_MD_FDMA_CTRL1_write_en1(1) | MD_MD_FDMA_CTRL1_fdma_go(1));

	while(rtd_inl(MD_MD_FDMA_CTRL1_reg) & MD_MD_FDMA_CTRL1_fdma_go_mask); // wait for MD done its operation

	rtd_outl(MD_MD_SMQ_INT_STATUS_reg, MD_MD_SMQ_INT_STATUS_fdma_done(1));	// clear flash done bit

	SN_SYNC;
	spin_unlock_irqrestore(&g_spi_master->spi_lock_priv, g_spi_master->spi_lock_flags);

	/*invalid cache*/
	dmac_inv_range(buf, buf + len);
	outer_inv_range(virt_to_phys(buf), virt_to_phys(buf + len));

	return;
}

static void rtk_spi_master_write_md(unsigned long to, unsigned int len, const unsigned char *buf)
{
	smd_checkComplete();

	spin_lock_irqsave(&g_spi_master->spi_lock_priv, g_spi_master->spi_lock_flags);
	
	/* set DDR and flash addr */
	rtd_outl(MD_MD_FDMA_DDR_SADDR_reg, (unsigned int)virt_to_phys(buf));
	rtd_outl(MD_MD_FDMA_FL_SADDR_reg, MD_MD_FDMA_FL_SADDR_fdma_fl_saddr((unsigned int)to));
#if 0
	rtd_outl(MD_MD_FDMA_CTRL2_reg, MD_MD_FDMA_CTRL2_fdma_length(len) | MD_MD_FDMA_CTRL2_fdma_dir(DDR_2_FLASH) | \
		MD_MD_FDMA_CTRL2_fdma_max_xfer(SET_DMA_LEN_512) | MD_MD_FDMA_CTRL2_fdma_swap(0) | \
		MD_MD_FDMA_CTRL2_fl_mapping_mode(1)); // forward map, no_swap, DMA LEN, FLASH to DDR
#else
	rtd_setbits(MD_MD_DBG_reg, MD_MD_DBG_md_dbg_fdma_write_general_length_en(1));
	rtd_outl(MD_MD_FDMA_CTRL2_reg, MD_MD_FDMA_CTRL2_fdma_max_xfer(3) | MD_MD_FDMA_CTRL2_fdma_length(len) | MD_MD_FDMA_CTRL2_fdma_dir(DDR_2_FLASH) | \
		MD_MD_FDMA_CTRL2_fdma_swap(0) | MD_MD_FDMA_CTRL2_fl_mapping_mode(1));
#endif

	dmac_flush_range(buf, buf + len);
	outer_flush_range(virt_to_phys(buf), virt_to_phys(buf + len));

	SN_SYNC;
	rtd_outl(MD_MD_FDMA_CTRL1_reg, MD_MD_FDMA_CTRL1_write_en1(1) | MD_MD_FDMA_CTRL1_fdma_go(1));

	while(rtd_inl(MD_MD_FDMA_CTRL1_reg) & MD_MD_FDMA_CTRL1_fdma_go_mask); // wait for MD done its operation
	
	rtd_outl(MD_MD_SMQ_INT_STATUS_reg, MD_MD_SMQ_INT_STATUS_fdma_done(1));  // clear flash done bit

	SN_SYNC;
	spin_unlock_irqrestore(&g_spi_master->spi_lock_priv, g_spi_master->spi_lock_flags);

	return;
}

static void rtk_spi_master_md_process(unsigned long flash_addr, unsigned int len, const unsigned char *buf, unsigned char is_read)
{
	if(is_read)
		rtk_spi_master_read_md(flash_addr, len, buf);
	else
		rtk_spi_master_write_md(flash_addr, len, buf);

	return;
}

static void rtk_spi_master_switch_read_mode(unsigned char read_cmd)
{
	rtd_outl(SB2_SFC_OPCODE_reg, SB2_SFC_OPCODE_opc(read_cmd) | SB2_SFC_OPCODE_dual_quad_mode(0)); //switch flash to read mode
	rtd_outl(SB2_SFC_CTL_reg, SB2_SFC_CTL_dataen(1) | SB2_SFC_CTL_adren(0) | SB2_SFC_CTL_dmycnt(0)); //data_en=1 addr_en=1 dmy_en=0

	SN_SYNC;
}

static void rtk_spi_master_switch_write_mode(unsigned char write_cmd, unsigned int data_len)
{
	rtd_outl(SB2_SFC_OPCODE_reg, SB2_SFC_OPCODE_opc(write_cmd) | SB2_SFC_OPCODE_dual_quad_mode(0));
	if(data_len == 1)
		rtd_outl(SB2_SFC_CTL_reg, SB2_SFC_CTL_dataen(0) | SB2_SFC_CTL_adren(0) | SB2_SFC_CTL_dmycnt(0)); //data_en=1 addr_en=1 dmy_en=0
	else
		rtd_outl(SB2_SFC_CTL_reg, SB2_SFC_CTL_dataen(1) | SB2_SFC_CTL_adren(0) | SB2_SFC_CTL_dmycnt(0)); //data_en=1 addr_en=1 dmy_en=0

	SN_SYNC;
}

static int	rtk_spi_master_transfer_priv(struct rtk_spi_master *master, 
										const unsigned char *tx_data, 
										unsigned char *rx_data, 
										unsigned int len)
{
	volatile unsigned char tmp = 0;
	unsigned long base_addr = 0;
	
	/*parameter sanity check*/
	if( master == NULL || tx_data == NULL || len < 1 ){
		rtd_pr_spi_dev_err("transfer func parameter check fail, data len = %d \r\n", len);
		return -1;
	}

	base_addr = master->base_addr;

	/* set cmd */
	rtk_spi_master_switch_write_mode(tx_data[0], len);

	rtd_pr_spi_dev_debug("[transfer] reg 0x%x(0x%x), reg 0x%x(0x%x) \r\n", \
		SB2_SFC_OPCODE_reg, rtd_inl(SB2_SFC_OPCODE_reg), SB2_SFC_CTL_reg, rtd_inl(SB2_SFC_CTL_reg));
	
	/*trigger data transfer*/
	if(len < 2){
		tmp = rtd_inb(base_addr);
	}else if(len == 2){
		/*cpu mode*/
		rtd_outb(base_addr, tx_data[1]);       // write one bytes data
	}else{
		/*md mode*/
		rtk_spi_master_md_process(0x0, len-1, &tx_data[1], 0);
	}
	
	return 0;
}

static int rtk_spi_master_receive_priv(struct rtk_spi_master *master, 
										const unsigned char *tx_data, 
										unsigned char *rx_data, 
										unsigned int len)
{
	unsigned long base_addr = 0;

	/*parameter sanity check*/
	if(master == NULL || tx_data == NULL || rx_data == NULL || len < 1){
		rtd_pr_spi_dev_err("receive func parameter check fail, data len = %d \r\n", len);
		return -1;
	}

	base_addr =  master->base_addr;

	/*set cmd*/
	rtk_spi_master_switch_read_mode(tx_data[0]);

	rtd_pr_spi_dev_debug("[receive] reg 0x%x(0x%x), reg 0x%x(0x%x) \r\n", \
			SB2_SFC_OPCODE_reg, rtd_inl(SB2_SFC_OPCODE_reg), SB2_SFC_CTL_reg, rtd_inl(SB2_SFC_CTL_reg));

	if(len == 1){
		/*cpu mode*/
		rx_data[0] = rtd_inb(base_addr);       // read one bytes data
	}else{
		/*md mode*/
		rtk_spi_master_md_process(0x0, len, rx_data, 1);
	}

	return 0;
}

/*spi master interface*/
static int rtk_spi_master_setup(struct spi_device *spi)
{
	struct rtk_spi_master *drv_data = spi_master_get_devdata(spi->master);
	struct spi_master_set_t *pset = &(drv_data->spi_set[spi->chip_select]);
	int ret = -1;

	rtd_pr_spi_dev_debug("[%s] spi port: %d , spi alias: %s \r\n", __func__, spi->chip_select, spi->modalias);

	rtk_spi_master_get_device(drv_data, SM_WORKING);

	/*set spi bits_per_word*/
	if(spi->bits_per_word != 8){
		rtd_pr_spi_dev_err("Only support bits_per_word is 8 , %d!\r\n", spi->bits_per_word);
		goto final;
	}
	pset->current_bits_per_word = spi->bits_per_word;
	
	/* set spi work mode*/
	switch (spi->mode & (SPI_CPHA | SPI_CPOL)) {
		case SPI_MODE_0:
			rtd_maskl(SB2_SFC_SCK_reg, ~SB2_SFC_SCK_mode_mask, SB2_SFC_SCK_mode(0));
			rtd_pr_spi_dev_info("Set SPI mode0, reg: 0x%x value: 0x%x!\r\n", SB2_SFC_SCK_reg, rtd_inl(SB2_SFC_SCK_reg));
			break;
		case SPI_MODE_3:
			rtd_maskl(SB2_SFC_SCK_reg, ~SB2_SFC_SCK_mode_mask, SB2_SFC_SCK_mode(1));
			rtd_pr_spi_dev_info("Set SPI mode3, reg: 0x%x value: 0x%x!\r\n", SB2_SFC_SCK_reg, rtd_inl(SB2_SFC_SCK_reg));
			break;
		default:
			rtd_pr_spi_dev_err("Not support this spi mode: 0x%x \r\n", spi->mode);
			goto final;
	}
	pset->current_mode = spi->mode;

	/*set clock*/
	if(spi->max_speed_hz > pset->max_clock){
		rtd_pr_spi_dev_err("Set clock %d , current spi max support clock %d !\r\n", spi->max_speed_hz, pset->max_clock);
		goto final;
	}

	rtk_spi_master_set_clk(spi->max_speed_hz / 1000000);
	pset->current_clock = spi->max_speed_hz;

	ret = 0;
final:
	rtk_spi_master_release_device(drv_data);
	
	return ret;
}

static void rtk_spi_master_cleanup(struct spi_device *spi)
{
	rtd_pr_spi_dev_info("%s enter...\r\n", __func__);
}

static int rtk_spi_master_transfer_one_message(struct spi_master *master,
						struct spi_message *msg)
{
	struct rtk_spi_master *drv_data = spi_master_get_devdata(master);
	struct spi_transfer *t;
	struct spi_device *spi = msg->spi;
	int status = 0;

	rtd_pr_spi_dev_debug("spi port: %d , spi alias: %s \r\n", spi->chip_select, spi->modalias);

	msg->state = NULL;
	msg->actual_length = 0;
	
	list_for_each_entry(t, &msg->transfers, transfer_list) {
		msg->state = t;

		rtk_spi_master_get_device(drv_data, SM_WORKING);
		rtk_spi_master_port_switch(&(drv_data->spi_set[spi->chip_select]));
       
		if(t->rx_buf == NULL){
			/* transfer data to spi device */
			status = drv_data->transfer(drv_data, t->tx_buf, NULL, t->len); 
			if(status < 0){
				rtk_spi_master_release_device(drv_data);
				rtd_pr_spi_dev_err("transfer data fail!\r\n");
				break;
			}
		}else{
			/* receive data from spi device*/
			status = drv_data->receive(drv_data, t->tx_buf, t->rx_buf, t->len);
			if(status < 0){
				rtk_spi_master_release_device(drv_data);
				rtd_pr_spi_dev_err("receive data fail!\r\n");
				break;
			}
		}

		rtk_spi_master_release_device(drv_data);
		
		msg->actual_length += t->len;
		if (t->delay.value)
			udelay(t->delay.value);
	}

	/*send message to core that transfer done*/
	msg->status = status;
	spi_finalize_current_message(master);

	return status;
}

#ifdef CONFIG_OF
static int rtk_spi_master_of_probe(struct platform_device *pdev)
{
	struct spi_master *master = platform_get_drvdata(pdev);
	struct rtk_spi_master *drv_data = spi_master_get_devdata(master);

	struct device_node *nc;
	unsigned int port = 0;
	unsigned int reg = 0;
	unsigned int max_clock = 0;
	char	modalias[32] = {0};
	int rc;

	if (!pdev->dev.of_node)
		return -ENXIO;

	for_each_available_child_of_node(pdev->dev.of_node, nc) {
		rc = of_property_read_u32(nc, "reg", &reg);
		if (rc) {
			rtd_pr_spi_dev_err("DTS: cannot find spi reg value!\r\n");
			return -ENXIO;
		}
		
		rc = of_property_read_u32(nc, "realtek,spi_port", &port);
		if(rc != 0 || port > 3){
			rtd_pr_spi_dev_err("DTS: cannot find spi port!\r\n");
			return -ENXIO;
		}

		rc = of_property_read_u32(nc, "spi-max-frequency", &max_clock);
		if(rc != 0 || port > 3){
			rtd_pr_spi_dev_err("DTS: cannot find spi-max-frequency!\r\n");
			return -ENXIO;
		}

		of_modalias_node(nc, modalias, sizeof(modalias));

		drv_data->spi_set[reg].spi_reg = reg;
		drv_data->spi_set[reg].spi_port = port;
		drv_data->spi_set[reg].max_clock= max_clock;
		drv_data->spi_set[reg].spi_type = 1; 	/*spi general device*/

		rtd_pr_spi_dev_notice("modalias: %s port: %d reg: %d max_clock: %d \r\n", modalias, port, reg, max_clock);
	}

#if IS_ENABLED(CONFIG_RTK_KDRV_SPI)
	drv_data->base_addr  = (unsigned long)SPI_BASE_ADDR;

	if(!drv_data->base_addr)
		return -EIO;

	return 0;
#else
	void __iomem *virt_base;
	
	virt_base =  of_iomap(pdev->dev.of_node, 0);

	WARN(!virt_base, "unable to map rtk-spi area\n");
	
	if(IS_ERR(virt_base))
		return -EIO;
	else
		drv_data->base_addr  = (unsigned long)virt_base;
	
	return 0;
#endif
}
#else /* !CONFIG_OF */
static int rtk_spi_master_of_probe(struct platform_device *pdev)
{
	return 0;
}
#endif /* CONFIG_OF */

/****************************************************************
*
*	debug node
*
*****************************************************************/
static ssize_t
spi_dev_status_dev_show(struct device *dev, struct device_attribute *attr,
        char *buf)
{
	unsigned int i = 0;
	unsigned int reg_clk = 0;
	unsigned int reg_delay_sel = 0;
	unsigned int delay_sel = 0;
	unsigned int clock = 0;
	unsigned int rising = 0;

	for(i = 0; i < 2; i++){
		struct spi_master_set_t *pset = &(g_spi_master->spi_set[i]);
		rtd_pr_spi_dev_emerg("%20s: %d \r\n", "SPI Reg", pset->spi_reg);
		rtd_pr_spi_dev_emerg("%20s: %d \r\n", "SPI Port", pset->spi_port);
		rtd_pr_spi_dev_emerg("%20s: %d \r\n", "SPI Type", pset->spi_type);

		rtd_pr_spi_dev_emerg("%20s: %d \r\n", "clock div", pset->clock_div);
		rtd_pr_spi_dev_emerg("%20s: %d \r\n", "delay sel", pset->delay_sel);

		rtd_pr_spi_dev_emerg("%20s: %d \r\n", "SPI Clock", pset->current_clock);
		rtd_pr_spi_dev_emerg("%20s: %d \r\n", "SPI Mode", pset->current_mode);
		rtd_pr_spi_dev_emerg("%20s: %d \r\n", "Max Clock", pset->max_clock);
	}
	
	reg_clk = rtd_inl(SB2_SFC_SCK_reg);
	reg_delay_sel = rtd_inl(SB2_SFC_POS_LATCH_reg);

	clock = BUSH_CLOCK/(1 + SB2_SFC_SCK_get_fdiv(reg_clk));
	delay_sel = SB2_SFC_POS_LATCH_get_delay_sel(reg_delay_sel);
	rising = SB2_SFC_POS_LATCH_get_pos_latch(reg_delay_sel);
	
	return snprintf(buf,128, "reg(0x%x)=0x%x reg(0x%x)=0x%x clock = %dMHZ delay_sel=%d pos_latch: %s\r\n", \
			SB2_SFC_SCK_reg, reg_clk, SB2_SFC_POS_LATCH_reg, reg_delay_sel,\
			clock, delay_sel, rising == 1 ? "rising edge": "falling edge");
}

static ssize_t
spi_dev_status_dev_store(struct device *dev,
              struct device_attribute *attr,
             const char *buf, size_t count)
{
	rtd_pr_spi_dev_err("%s %d, Not support!\n", __func__, __LINE__);
	rtd_pr_spi_dev_err("%s\n", buf);

	return count;
}

DEVICE_ATTR(spi_dev_status, S_IRUGO|S_IWUSR|S_IWGRP,
            spi_dev_status_dev_show,spi_dev_status_dev_store);

static int rtk_spi_master_probe(struct platform_device *pdev)
{
	struct spi_master *master = NULL;
	struct rtk_spi_master *drv_data = NULL;
	int err = -ENODEV;
	int att_err;

	rtd_pr_spi_dev_notice("spi master init ... version 1.0\r\n");

	/* allocate master with space for drv_data */
	master = spi_alloc_master(&pdev->dev, sizeof(struct rtk_spi_master));
	if(!master) {
		rtd_pr_spi_dev_err("can not alloc spi_master\n");
		return -ENOMEM;
	}

	att_err = device_create_file(&pdev->dev, &dev_attr_spi_dev_status);
	
	/* setup the master state. */
	master->dev.of_node = pdev->dev.of_node;
	master->bus_num = pdev->id;
	master->num_chipselect = 2;
	master->mode_bits = SPI_CPOL | SPI_CPHA;
	master->max_speed_hz = 16000000;	/*max freq, 16MHZ*/
	master->min_speed_hz = 1000000;
	master->setup = rtk_spi_master_setup;
	master->cleanup = rtk_spi_master_cleanup;
	master->transfer_one_message = rtk_spi_master_transfer_one_message;

	platform_set_drvdata(pdev, master);

	drv_data = spi_master_get_devdata(master);
	drv_data->master = master;

	/*set default spi setting info*/
	drv_data->spi_set[0].current_clock= master->min_speed_hz;
	drv_data->spi_set[0].current_mode = SPI_MODE_0;
	drv_data->spi_set[0].current_bits_per_word = 8;
	drv_data->spi_set[0].clock_div = 0x25; 
	drv_data->spi_set[0].delay_sel = 4;
	memcpy(&drv_data->spi_set[1], &drv_data->spi_set[0], sizeof(struct spi_master_set_t));

	/*init spi master controler*/
	rtk_spi_master_controler_init();

	/*init spi clock*/
	rtk_spi_master_set_clk(drv_data->spi_set[0].current_clock/1000000);

	/* probe spi master base address */
	err = rtk_spi_master_of_probe(pdev);
	if (err){
		rtd_pr_spi_dev_err("get spi master base address fail!\r\n");
		goto exit;
	}

	/*set spi master init status */
	spin_lock_init(&drv_data->spi_lock_priv);
	spin_lock_init(&drv_data->spi_lock);
	drv_data->state = SM_READY;
	init_waitqueue_head(&drv_data->wq);

	/* register transfer and receive callback */
	drv_data->transfer = rtk_spi_master_transfer_priv;
	drv_data->receive = rtk_spi_master_receive_priv;
	
	/* register spi master  and add spi device*/
	err = spi_register_master(spi_master_get(master));
	if (err){
		rtd_pr_spi_dev_err("register spi master fail!\r\n");
		goto exit;
	}

	g_spi_master = drv_data;
	
	rtd_pr_spi_dev_info("spi master init ... finish!\r\n");
	return 0;
	
exit:
	spi_master_put(master);
	rtd_pr_spi_dev_info("spi master init ... fail!\r\n");
	return err;	
}

static int rtk_spi_master_remove(struct platform_device *pdev)
{
	struct spi_master *master = platform_get_drvdata(pdev);
	
	spi_master_put(master);
	
	rtd_pr_spi_dev_info("spi master remove ... finish!\r\n");
	return 0;
}

#ifdef CONFIG_PM
static int rtk_spi_master_suspend(struct device *dev)
{
	struct spi_master *master = dev_get_drvdata(dev);
	struct rtk_spi_master *drv_data = spi_master_get_devdata(master);
	int ret = 0;
	int i = 0;
	
	ret = rtk_spi_master_get_device(drv_data, SM_SUSPEND);
	if(ret < 0){
		rtd_pr_spi_dev_err("Set spi master status to suspend mode fail!\r\n");
		return ret;	
	}

	if(drv_data->reg_array == NULL){
		drv_data->reg_array = (unsigned int *)kmalloc(SPI_REG_NUM * sizeof(unsigned int), GFP_KERNEL);
		if(drv_data->reg_array == NULL){
			rtd_pr_spi_dev_err("malloc buffer to store register status fail!\r\n");
			ret = -ENOMEM;
			goto final;
		}
	}

	/*dump all spi master register */
	for(i = 0; i < SPI_REG_NUM; i++)
		drv_data->reg_array[i] = rtd_inl(SPI_REG_BASE + i * sizeof(unsigned int));

	ret = 0;
	
final:
	rtk_spi_master_release_device(drv_data);
	return ret;	
}

static int rtk_spi_master_resume(struct device *dev)
{
	struct spi_master *master = dev_get_drvdata(dev);
	struct rtk_spi_master *drv_data = spi_master_get_devdata(master);
	int ret = 0;
	int i = 0;

	ret = rtk_spi_master_get_device(drv_data, SM_SUSPEND);
	if(ret < 0){
		rtd_pr_spi_dev_err("Set spi master status to suspend/resume mode fail!\r\n");

		if(drv_data->reg_array) {
			kfree(drv_data->reg_array);
			drv_data->reg_array = NULL;
		}
		return ret;	
	}

	if(drv_data->reg_array != NULL) {
		/*restore all spi master register */
		for(i = 0; i < SPI_REG_NUM; i++)
			rtd_outl(SPI_REG_BASE + i * sizeof(unsigned int), drv_data->reg_array[i]);

		/*special register, need set write enable bit to write value*/
		if(drv_data->reg_array[4] & SB2_SFC_WP_ce_n_mask)
			rtd_outl(SB2_SFC_WP_reg,  drv_data->reg_array[4] | SB2_SFC_WP_write_en2(1));

		kfree(drv_data->reg_array);
		drv_data->reg_array = NULL;
		ret = 0;
	} else {
		rtd_pr_spi_dev_err("restore spi master register fail!\n");
		ret = -1;
	}

	rtk_spi_master_release_device(drv_data);
	return ret;
}

static const struct dev_pm_ops rtk_spi_master_pm_ops = {
		.suspend = rtk_spi_master_suspend,
		.resume = rtk_spi_master_resume,
		
#ifdef CONFIG_HIBERNATION
		.restore = rtk_spi_master_resume,
		.freeze = rtk_spi_master_suspend,
		.thaw = rtk_spi_master_resume,
#endif		
};
#define RTK_SPI_MASTER_PM_OPS	(&rtk_spi_master_pm_ops)
#else
#define RTK_SPI_MASTER_PM_OPS	NULL
#endif


#ifdef CONFIG_OF
static const struct of_device_id rtk_spi_master_match[] = {
	{ .compatible = "realtek,rtk_spi_master", },
	{},
};
MODULE_DEVICE_TABLE(of, rtk_spi_master_match);
#endif /* CONFIG_OF */

static struct platform_driver rtk_spi_master_driver = {
	.probe = rtk_spi_master_probe,
	.remove = rtk_spi_master_remove,
	.driver = {
		.name = DRV_NAME,
		.pm = RTK_SPI_MASTER_PM_OPS,
		.of_match_table = of_match_ptr(rtk_spi_master_match),
	},
};
module_platform_driver(rtk_spi_master_driver);


#if !IS_ENABLED(CONFIG_RTK_KDRV_SPI)
int spi_read_internal(unsigned int type, unsigned long offset, unsigned int count, void *buf)
{
	rtd_pr_spi_dev_err("Not support %s \r\n", __func__);
	return -1;
}
EXPORT_SYMBOL(spi_read_internal);
#endif


MODULE_DESCRIPTION("Realtek SPI master controler driver");
MODULE_AUTHOR("Realtek <xxx_xxxx@apowertec.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRV_NAME);
MODULE_SOFTDEP("pre: rtk-spinor");	/*make sure rtk-spinor.ko insert first*/
