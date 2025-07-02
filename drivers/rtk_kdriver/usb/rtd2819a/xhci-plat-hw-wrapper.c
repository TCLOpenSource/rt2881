#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/usb/phy.h>
#include <linux/slab.h>
#include <linux/acpi.h>
#include <linux/version.h>
#include <asm/delay.h>
#include <linux/mutex.h>
#include <rtk_kdriver/io.h>
#include <rtk_kdriver/rtk_crt.h>
#include <rbus/usb3_top_reg.h>
#include <rbus/usb3_gen2_phy_ctrl_reg.h>
#include <rbus/usb3_phy_ctrl_reg.h>
#include "xhci.h"
#include "xhci-plat.h"

#define USB_PHY_DELAY 50

static DEFINE_MUTEX(crt_mutex);
int xhci_usb3_phy_init_post(struct platform_device *pdev);
static unsigned int get_usb_application_scene(void)
{
	static unsigned int g_application_scene_index = 0xFFFFFFFF;
	if(g_application_scene_index == 0xFFFFFFFF) {
#ifdef CONFIG_REALTEK_RESCUE_LINUX
		g_application_scene_index =  0;
#else
		g_application_scene_index = 5;
		//g_application_scene_index =  0;
#endif
	}
	xhci_plat_info("scene index=%u\n", g_application_scene_index);
	return g_application_scene_index;
}


bool is_usb3_on(void)
{
        bool is_mac_clk_on = false, is_mac_release = false;
        unsigned clk = 0, mac_rst = 0;

        // clock: USB MAC
        clk = rtd_inl(SYS_REG_SYS_CLKEN6_reg);
        if (clk & (SYS_REG_SYS_CLKEN6_clken_usb3_hst_dwc_mask 
			| SYS_REG_SYS_CLKEN6_clken_usb3_hst_wrapper_mask))
                is_mac_clk_on = true;


        // reset: USB MAC
        mac_rst = rtd_inl(SYS_REG_SYS_SRST6_reg);
        if (mac_rst & (SYS_REG_SYS_SRST6_rstn_usb3_hst_dwc_mask |
                    SYS_REG_SYS_SRST6_rstn_usb3_hst_wrapper_mask))
                is_mac_release = true;

        return (is_mac_clk_on && is_mac_release);
}
EXPORT_SYMBOL(is_usb3_on);


void usb3_crt_on(void)
{
		unsigned int index = get_usb_application_scene();
        mutex_lock(&crt_mutex);
		if(index == 0) {
				//xhci u3 5G U2 P0
				CRT_CLK_OnOff(USB, CLK_ON, (void *)30);
		}else if(index == 1 || index == 5) {
				//xhci u3 10G U2 P0
				CRT_CLK_OnOff(USB, CLK_ON, (void *)31);
		} else {
				//xhci 5g u2 p1
				CRT_CLK_OnOff(USB, CLK_ON, (void *)32);
		}
        
        mutex_unlock(&crt_mutex);
}
EXPORT_SYMBOL(usb3_crt_on);


void usb3_crt_off(void)
{
		unsigned int index = get_usb_application_scene();
        mutex_lock(&crt_mutex);
        if(index == 0) {
				//xhci u3 5G U2 P0
				CRT_CLK_OnOff(USB, CLK_OFF, (void *)30);
		}else if(index == 1 || index == 5) {
				//xhci u3 10G U2 P0
				CRT_CLK_OnOff(USB, CLK_OFF, (void *)31);
		} else {
				//xhci 5g u2 p1
				CRT_CLK_OnOff(USB, CLK_OFF, (void *)32);
		}
        mutex_unlock(&crt_mutex);
}
EXPORT_SYMBOL(usb3_crt_off);


static void xhci_usb3_mdio_5G_phy_write(
        struct xhci_hcd *xhci,
        unsigned char           port,
        unsigned char           phy_addr,
        unsigned char           reg_addr,
        unsigned short          val
)
{
	//TODO
}

static void xhci_usb3_mdio_10G_phy_write(
        struct xhci_hcd *xhci,
        unsigned char           port,
        unsigned char           phy_addr,
        unsigned char           reg_addr,
        unsigned short          val
)
{
	//TODO
}


void xhci_usb3_mdio_write(
        struct xhci_hcd *xhci,
        unsigned char           port,
        unsigned char           phy_addr,
        unsigned char           reg_addr,
        unsigned short          val
){
	unsigned int index = get_usb_application_scene();
	if(index == 0 || index == 1 || index == 5)
		xhci_usb3_mdio_10G_phy_write(xhci, port, phy_addr, reg_addr, val);
	else
		xhci_usb3_mdio_5G_phy_write(xhci, port, phy_addr, reg_addr, val);
}

unsigned short xhci_usb3_mdio_10G_phy_read(
        struct xhci_hcd *xhci,
        unsigned char       port,
        unsigned char       phy_addr,
        unsigned char       reg_addr
){
	return 0;
}

unsigned short xhci_usb3_mdio_5G_phy_read(
        struct xhci_hcd *xhci,
        unsigned char       port,
        unsigned char       phy_addr,
        unsigned char       reg_addr
){
	return 0;
}


unsigned short xhci_usb3_mdio_read(
        struct xhci_hcd *xhci,
        unsigned char       port,
        unsigned char       phy_addr,
        unsigned char       reg_addr
){
	unsigned int index = get_usb_application_scene();
	if(index == 0 || index == 1 || index == 5)
		return xhci_usb3_mdio_10G_phy_read(xhci, port, phy_addr, reg_addr);
	else
		return xhci_usb3_mdio_5G_phy_read(xhci, port, phy_addr, reg_addr);	
}


static void do_u3_5G_phy_setting(void)
{
	xhci_plat_info("--------- %s start ------------ \n", __func__);
	//TODO:init phy settings + OTP
	
}

static void do_u3_10G_phy_setting(void)
{
	xhci_plat_info("--------- %s start ------------ \n", __func__);
	//PHY access start
	rtd_outl(0xb80db004,0x80000000);//[31]enable mdio=1
	rtd_outl(0xb80db010,0x00000111);//[0]usb3_gen2_phy_enable=1, [4]

	rtd_outl(0xb80db000,0x00000005);
	rtd_outl(0xb80db004,0x80000001);//phy_read
	rtd_outl(0xb80db000,0x00000005);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x30840005);
	rtd_outl(0xb80db004,0x80000002);//phy_write

	//APHY reg=0x155
	rtd_outl(0xb80db000,0x3c850155);//[13]REG0_RX_INVOOBS_EN_5G=1
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x00000155);
	rtd_outl(0xb80db004,0x80000001);//phy_read
	rtd_outl(0xb80db000,0x00000155);
	rtd_outl(0xb80db004,0x80000001);
	rtd_inl(0xb80db008);//read value

	//APHY reg=0x1a5
	rtd_outl(0xb80db000,0x3c8501a5);//[13]REG0_RX_INVOOBS_EN_10G=1
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x000001a5);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db004,0x80000001);
	rtd_inl(0xb80db008);//read value

	//APHY reg=0x159
	rtd_outl(0xb80db000,0x00000159);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db004,0x80000001);
	rtd_inl(0xb80db008);//read value
	rtd_outl(0xb80db000,0x05850159);//
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x00000159);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db004,0x80000001);
	rtd_inl(0xb80db008);//read value

	//APHY reg=0x1a9
	rtd_outl(0xb80db000,0x000001a9);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db004,0x80000001);
	rtd_inl(0xb80db008);//read value
	rtd_outl(0xb80db000,0x058501a9);//
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x000001a9);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db004,0x80000001);
	rtd_inl(0xb80db008);//read value

	//APHY reg=0x1b2
	rtd_outl(0xb80db000,0x000001b2);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db004,0x80000001);
	rtd_inl(0xb80db008);//read value
	rtd_outl(0xb80db000,0x012001b2);//
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x000001b2);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db004,0x80000001);
	rtd_inl(0xb80db008);//read value

	//APHY reg=0x162
	rtd_outl(0xb80db000,0x00000162);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db004,0x80000001);
	rtd_inl(0xb80db008);//read value
	rtd_outl(0xb80db000,0x01200162);//
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x00000162);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db004,0x80000001);
	rtd_inl(0xb80db008);//read value

	rtd_outl(0xb80db000,0x00000005);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x00000005);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x30840005);
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x00001c02);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x00001c02);
	rtd_outl(0xb80db004,0x80000001);
	//CDR_DFE reg=1c02 [1]ckrdy_sel=1, [2]v2a_delay_ana_en=1, [3]oobs_gated=1, 
	//[6]px_lfps_sel=1,[7]p1_lfps_sel=1, [15:8]delay time for clk ready after cmu ready=0x8000
	rtd_outl(0xb80db000,0x085e1c02);
	rtd_outl(0xb80db004,0x80000002);//phy_writ
	rtd_outl(0xb80db000,0x00001c02);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db004,0x80000001);
	rtd_inl(0xb80db008);//read value
	rtd_outl(0xb80db000,0x00001c00);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x00001c00);
	rtd_outl(0xb80db004,0x80000001);
	//CDR_DFE reg=1c00 [0]phy_reset_n=1, [1]phy_off_reset_n=1, [2]phy_clk_en=1, [3]low_latency=1
	//[4]rx_negedge_latch=1, [6]noclk_pipeline_en=1, [12]rx_en_sel=1
	rtd_outl(0xb80db000,0x105f1c00);
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x00031c08);//reg=1c08 [15:0]p2p0_delay_5g=0x3
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x00031c0a);//reg=1c0a [15:0]p2p0_delay_10g=0x3
	rtd_outl(0xb80db004,0x80000002);//phy_write

	//test add
	rtd_outl(0xb80db000,0x93001194);//REG0_ST_MODE_5G[15]=1
	rtd_outl(0xb80db004,0x80000002);//[1] mdio write
	rtd_outl(0xb80db000,0x930011c4);//REG0_ST_MODE_10G[15]=1
	rtd_outl(0xb80db004,0x80000002);//[1] mdio write
	//test add end
	rtd_outl(0xb80db000,0x0000100c);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x0000100c);
	rtd_outl(0xb80db004,0x80000001);
	//CDR_DFE reg=100c, [9:0]offset_timeout=0x3F, [10]offset_oobs2_pc=0,[11]oofset_oobs_ok=1
	//[12]offset_oobs2_gray_en=0,[13]offset_oobs1_pc=0, [14]offset_oobs1_ok=1,[15]offset_oobs1_gray_en=0
	//rtd_outl(0xb80db000,0x4BFF1100c);
	//rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x0000100a);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x0000100a);
	rtd_outl(0xb80db004,0x80000001);
	//CDR_DFE reg=100a, [0]offset_oobs_pc=0,[1]offset_oobs_ok=1, [3]offset_oobs_gray_en=0
	//[9:4]ofset_divisor=0x8
	rtd_outl(0xb80db000,0x0082100a);
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x0000100a);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x0000100a);
	rtd_outl(0xb80db004,0x80000001);
	//CDR_DFE reg=100a,[9:4]ofset_divisor=0x1
	rtd_outl(0xb80db000,0x0012100a);
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x000018c0);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x000018c0);
	rtd_outl(0xb80db004,0x80000001);
	//FLD_SSC reg=18c0, [0]ssc_en_5g=0, [1]ssc_set_valid_5g=0
	rtd_outl(0xb80db000,0x000018c0);
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x000018d0);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x000018d0);
	rtd_outl(0xb80db004,0x80000001);
	//FLD_SSC reg=18d0, [0]ssc_en_10g=0, [1]ssc_set_valid_10g=0
	rtd_outl(0xb80db000,0x000018d0);
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x00001936);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x00001936);
	rtd_outl(0xb80db004,0x80000001);
	//FLD_SSC reg=1936, [8:0]ssc_up_n_code_5g=0xc6
	rtd_outl(0xb80db000,0x00c61936);
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x00001946);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x00001946);
	rtd_outl(0xb80db004,0x80000001);
	//FLD_SSC reg=1946, [8:0]ssc_up_n_code_10g=0xc6
	rtd_outl(0xb80db000,0x00c61946);
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x00001c40);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x00001c40);
	rtd_outl(0xb80db004,0x80000001);
	//usb32phy reg=1c40, [0]spd_page_load=1
	rtd_outl(0xb80db000,0x00011c40);
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x00001c40);//[0]spd_page_load=0
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x000018c0);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x000018c0);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x000218c0);//[1]ssc_set_valid_10g=1
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x000018c0);//[1]ssc_set_valid_10g=0
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x000018d0);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x000018d0);
	rtd_outl(0xb80db004,0x80000001);
	rtd_outl(0xb80db000,0x000218d0);//[1]ssc_set_valid_10g=1
	rtd_outl(0xb80db004,0x80000002);//phy_write
	rtd_outl(0xb80db000,0x000018d0);//[1]ssc_set_valid_10g=0
	rtd_outl(0xb80db004,0x80000002);//phy_write

	///debug pin

	//rtd_outl(0xb80009f8,0x20201000);//Pimnux all
	//rtd_outl(0xb80009f8,0x25104000);//Pimnux all
	//rtd_outl(0xb800088C,0xE0F0F0F0);//measure [31:28]pad_gpio_46_io=0xE(test_en_bit0)
	//rtd_outl(0xb8000830,0xe0000000);//test_en_bit1
	//rtd_outl(0xb8000844,0xe0000000);//test_en_bit2
	//rtd_outl(0xb8016cac,0xe0e0e0e0);//etn test_en_bit2 to 5
	//rtd_outl(0xb8016cb4,0xe0e0e0e0);//etn test_en_bit6 to 9
	//rtd_outl(0xb80d90cc,0x00001100);//MAC   11-->22

	rtd_outl(0xb80db000,0x00021c46);//DPHY default=0000
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x00041c44);//DPHY default=0000 rotate
	rtd_outl(0xb80db004,0x80000002);
	///debug pin end///

	rtd_outl(0xb80db000,0x00011C40); //1c40 default=0000
	rtd_outl(0xb80db004,0x80000002);

	rtd_outl(0xb80db000,0xE0FA0153);//<3>rx_test_en  DPHY default=E0F2
	rtd_outl(0xb80db004,0x80000002);

	rtd_outl(0xb80db000,0xE0FA01A3);//<3>rx_test_en  DPHY default=E0F2
	rtd_outl(0xb80db004,0x80000002);



	//////offset manual///////////////////////////////////////

	//rtd_outl(0xb80db000,0x707011B6);//oobs1&2 manual 
	//rtd_outl(0xb80db004,0x80000002);
	//
	//rtd_outl(0xb80db000,0x578011BE);//fore manual=128
	//rtd_outl(0xb80db004,0x80000002);
	//
	//rtd_outl(0xb80db000,0x01C111B8);//sa data manual [1000001]
	//rtd_outl(0xb80db004,0x80000002);
	//
	//rtd_outl(0xb80db000,0x018112B8);//sa edge manual [0000001]
	//rtd_outl(0xb80db004,0x80000002);
	//
	//rtd_outl(0xb80db000,0xe06011BA);//sa dh&dl manual 
	//rtd_outl(0xb80db004,0x80000002);
	//
	//rtd_outl(0xb80db000,0x80e011BC);//sum manual 
	//rtd_outl(0xb80db004,0x80000002);

	//rtd_outl(0xb80db000,0x000711AC);//servo manual 
	//rtd_outl(0xb80db004,0x80000002);
	//rtd_outl(0xb80db000,0x240011AE);//servo ini
	//rtd_outl(0xb80db004,0x80000002);

	//rtd_outl(0xb80db000,0x018011B5);//sa_mon_ manual 
	//rtd_outl(0xb80db004,0x80000002);

	///10G odfset manual

	//rtd_outl(0xb80db000,0x707011E6);//oobs1&2 manual 
	//rtd_outl(0xb80db004,0x80000002);
	//
	//rtd_outl(0xb80db000,0x578011EE);//fore manual=128
	//rtd_outl(0xb80db004,0x80000002);
	//
	//rtd_outl(0xb80db000,0x01C111E8);//sa data manual [1000001]
	//rtd_outl(0xb80db004,0x80000002);
	//
	//rtd_outl(0xb80db000,0x018112E8);//sa edge manual [0000001]
	//rtd_outl(0xb80db004,0x80000002);
	//
	//rtd_outl(0xb80db000,0xe06011EA);//sa dh&dl manual 
	//rtd_outl(0xb80db004,0x80000002);
	//
	//rtd_outl(0xb80db000,0x80e011EC);//sum manual 
	//rtd_outl(0xb80db004,0x80000002);

	//rtd_outl(0xb80db000,0x000711DC);//servo manual 
	//rtd_outl(0xb80db004,0x80000002);
	//rtd_outl(0xb80db000,0x240011DE);//servo ini
	//rtd_outl(0xb80db004,0x80000002);

	//rtd_outl(0xb80db000,0x018011E5);//sa_mon_ manual 
	//rtd_outl(0xb80db004,0x80000002);
	////
	/////////////////////////////////////////////////
	//rtd_outl(0xb80db000,0x00011C40);// spd_page_load
	//rtd_outl(0xb80db004,0x80000002);
	//rtd_outl(0xb80db000,0x000F1F04);//
	//rtd_outl(0xb80db004,0x80000002);
	//rtd_outl(0xb80db000,0x00011C2A);//5G
	//rtd_outl(0xb80db004,0x80000002);




	//////////EQ//////////////////

	rtd_outl(0xb80db000,0x100011A4);//EQ manual=0x9 5G,  adjust at [14:10]
	rtd_outl(0xb80db004,0x80000002);

	rtd_outl(0xb80db000,0x100011D4);//EQ manual=0x9 10G adjust at [14:10]
	rtd_outl(0xb80db004,0x80000002);

	// force det term
	//rtd_outl(0xb80db000,0x04100165);// aphy=0x165, [10]reg0_tx_force_rcvdet_5g
	//rtd_outl(0xb80db004,0x80000002);
	//rtd_outl(0xb80db000,0x041001b5);// aphy=0x1b5, [10]reg0_tx_force_rcvdet_10g
	//rtd_outl(0xb80db004,0x80000002);
	///////////////////////////////////////////

	////////New APHY DPHY setting////
	rtd_outl(0xb80db000,0xC31D0152);//dfe_reserved=00011101
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x3C850155); //1C85--->3C85 rx_invoobs_en=[1]
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x380C0156);//5G [1:0]ivga_adj=[11]   [13:11]=leq_ac_range[111]
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x6A3C0157);//5G leq_iadj=[01]
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x61400158);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x05850159);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x0232015A);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x0F16015E);//5G rx_reserved[5:4]spd_sel=01,[3]iboost_vga_leq=0 ,[0] no use
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x01200162);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x69E30195);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x0031019C);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0xC31D01A2);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x3C9501A5); //1C85--->3C85// 3c95 ibrx_adj=[10]
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x380C01A6); //10G [1:0]ivga_adj=[11]   [13:11]=leq_ac_range[111]
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0xEA7A01A7);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x614001A8);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x058501A9);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x023201AA);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x0F3601AE);//rx_reserved[5:4]spd_sel=11,[3]iboost_vga_leq=0, [0] no use
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x012001B2);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x69E301E5);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x003101EC);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x299C0202);//5G CMU_PI_MAIN_IBSEL=[0100]
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x299C0252);//10G CMU_PI_MAIN_IBSEL=[0100]
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x91801192);//gen1 BBW_ST,KP,KI
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x91801194);//gen1 ST,KP,KI
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x600C119C);//HPERIOD=0xC  5G
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x4002119E);//LPERIOD=0x2  5G
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0xE87F11B2);//[14:12]rotation mode=[110]
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x020911BD);//ZF_MODE &ZF2_mode & ZF3_mode=edg mode
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0xE87F11E2);//[14:12]rotation mode=[110]
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x020911ED);//ZF_MODE &ZF2_mode & ZF3_mode=edg mode
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x601811CC);//HPERIOD=0xb8  10G
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x400411CE);//LPERIOD=0x4  10G
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x90C011C2);//gen2 BBW_ST,KP,KI
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x90C011C4);//gen2 ST,KP,KI
	rtd_outl(0xb80db004,0x80000002);
	//rtd_outl(0xb80db000,0x08CE1C02);
	//rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x00271842);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x02E41844);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x02E41846);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x00271852);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x02E41854);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x02E41856);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x00B718C2);//N code
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x00B718D2);//N code
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0xF68418C4);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x000218C6);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x02F418C8);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x000018CA);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0xF68418C4);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x000218C6);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x02F418C8);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x000018CA);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x000218D6);//10G fcode
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0xF68418D4);//10G fcode
	rtd_outl(0xb80db004,0x80000002);

	////////////////////tap0 manual//////

	rtd_outl(0xb80db000,0x000011AA);//tap2~3 init=0x0 tap1=0
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x000011DA);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x000311AC);//tap4 ini=0x0
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x000311DC);
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0xC1FF11E0);//tap1~4 load-in=1
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0xC1FF11B0);//tap1~4 load-in=1 
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x01FF11E4);//tap1~4 adapt_en=1
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x01FF11B4);//tap1~4 adapt_en=1
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x00F01022);//tap1 th min=F, tap1 max/min=0, tap2 max=0
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x0F001024);//tap2 th min=F ,tap2 min=0 ,tap3 max/min=0
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0xF00F1026);//tap3/4 th min=F, tap4 max/min=0
	rtd_outl(0xb80db004,0x80000002);
	rtd_outl(0xb80db000,0x41FF102B);//reg0_th_min_en<8:0>=111111111
	rtd_outl(0xb80db004,0x80000002);
	////////////////////////////////////////////////

	rtd_outl(0xb80db000,0x000218C0);//toggle setvalid
	rtd_outl(0xb80db004,0x80000002);

	rtd_outl(0xb80db000,0x000018C0);//toggle setvalid
	rtd_outl(0xb80db004,0x80000002);

	rtd_outl(0xb80db000,0x000218D0);//toggle setvalid 100
	rtd_outl(0xb80db004,0x80000002);

	rtd_outl(0xb80db000,0x000018D0);//toggle setvalid
	rtd_outl(0xb80db004,0x80000002);
}

static void do_u2_p1_phy_setting(void)
{
	xhci_plat_info("--------- %s start ------------ \n", __func__);
	udelay(USB_PHY_DELAY);
	//init phy settings + OTP
}

static void do_u2_p0_phy_setting(void)
{
	xhci_plat_info("--------- %s start ------------ \n", __func__);
	//TODO:init phy settings + OTP
}

/************************************************************/
int xhci_usb_mac_init(struct platform_device *pdev)
{
        struct usb_hcd *hcd = platform_get_drvdata(pdev);
        struct xhci_global_regs __iomem *global_regs = hcd_to_xhci_priv(hcd)->global_regs;
        struct xhci_plat_priv *priv = hcd_to_xhci_priv(hcd);
        unsigned port_mask = 0;
        unsigned int i = 0;
		unsigned int index = get_usb_application_scene();

        xhci_plat_info("--------- %s start ------------ \n", __func__);
		
        port_mask = (1 << priv->usb2_port_cnt) - 1;
        // UTMI reset
        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB2_PHY_UTMI_REG, port_mask);

        // UTMI release
        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB2_PHY_UTMI_REG, 0x0);

        // PHY ignores suspend signal and always keep UTMI clock active
        rtd_setbits(priv->wrapper_regs.XHCI_USB3_TOP_USB_TMP_1_REG, USB3_TOP_USB_TMP_1_test_reg_1(1)
                    | USB3_TOP_USB_TMP_1_usb_ref_sel(0xd));
        // Wdone make sure that RW-inorder
        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_WRAP_CTR_REG, rtd_inl(priv->wrapper_regs.XHCI_USB3_TOP_WRAP_CTR_REG) | USB3_TOP_WRAP_CTR_dbus_wait_wdone(1));

        rtd_outl(0xb8058104, rtd_inl(0xb8058104)|(0xf<<8) );

        // usb 3.0 p3 mode -> p2 mode
        for(i = 0; i < priv->usb3_port_cnt; i++)
                writel(0x010C0002, &global_regs->gusb3pipectl[i]); //P3 mode->p2 mode

        //axi init
        rtd_outl(0xb8058100, 0x0000000e);

        //suspend timer setting  intital setting
        rtd_outl(0xb8058110, 0x34b81004);

        // bit3=1, port0-UTMI clcok=30MHz
        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_GUSB2PHYCFG0_REG,
                 rtd_inl(priv->wrapper_regs.XHCI_USB3_TOP_GUSB2PHYCFG0_REG) | (1 << 3));

        xhci_plat_info("--------- %s finish ------------ \n", __func__);

        if(index == 0) {
				//xhci u3 5G U2 P0
				rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB3_PHY_PIPE_REG, 0x1);
				udelay(400);
				do_u3_5G_phy_setting();
				rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB3_PHY_PIPE_REG, 0x0);
				udelay(400);
				xhci_usb3_phy_init_post(pdev);
				
				do_u2_p0_phy_setting();
				// P0 u2phy: // P1 u2phy: 
				// let U2PHY UTMI interface are triggered at NEG edge	
				rtd_outl(0xb805A014,0x0000000c);
				rtd_outl(0xb8058280,0x02000100);
				rtd_outl(0xb8058280,0x02000f00);
				
				
		}else if(index == 1 || index == 5) {
				
				//xhci u3 10G U2 P0
				do_u3_10G_phy_setting();
				rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB3_PHY_PIPE_REG, 0x1);
				udelay(400);
				rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB3_PHY_PIPE_REG, 0x0);
				udelay(400);
				
				xhci_usb3_phy_init_post(pdev);
				
				do_u2_p0_phy_setting();
				// P0 u2phy: // P1 u2phy: 
				// let U2PHY UTMI interface are triggered at NEG edge	
				rtd_outl(0xb805A014,0x0000000c);
				rtd_outl(0xb8058280,0x02000100);
				rtd_outl(0xb8058280,0x02000f00);
				
				
		} else {
				//xhci 5g u2 p1
				rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB3_PHY_PIPE_REG, 0x1);
				udelay(400);
				do_u3_5G_phy_setting();
				rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB3_PHY_PIPE_REG, 0x0);
				udelay(400);
				xhci_usb3_phy_init_post(pdev);
				
				do_u2_p1_phy_setting();
				// P0 u2phy: // P1 u2phy: 
				// let U2PHY UTMI interface are triggered at NEG edge	
				rtd_outl(0xb805A014,0x0000000c);
				rtd_outl(0xb8058280,0x02000100);
				rtd_outl(0xb8058280,0x02000f00);
				
				
		}

		
        return 0;
}

#if defined(CONFIG_RTK_KDRV_PCIE)
static DEFINE_MUTEX(pcie_share_mutex);
struct usb_hcd *hcd_u3 = NULL;
int usb_chk_pcie_enable(void){
        u32 reg_info;

        reg_info = rtd_inl(SYS_REG_SYS_CLKSEL_reg);
        xhci_plat_info("SYS_REG_SYS_CLKSEL_reg = 0x%08x \n",reg_info);
        reg_info &= SYS_REG_SYS_CLKSEL_pcie_usb3_sel_mask;
        xhci_plat_info("SYS_REG_SYS_CLKSEL_reg[7] = 0x%x \n",reg_info);
        if(reg_info == 0){   // PCIe enabled
                xhci_plat_info("PCIe enabled\n");
                return 1;
        }else{
                xhci_plat_info("PCIe no be enabled\n");
                return 0;
        }
}

int usb_chk_pcie_share(struct usb_hcd *hcd)
{
        int ret = 0;
        u32 reginfo;
        struct xhci_plat_priv *priv = NULL;

        if(hcd == hcd_u3){
                xhci_plat_info("U3 hcd match (hcd: 0x%pR)\n",hcd);
                mutex_lock(&pcie_share_mutex);
                if(usb_chk_pcie_enable()){
                        priv = hcd_to_xhci_priv(hcd);
                        xhci_plat_info("disable U3 func for share PCIe\n");
                        reginfo = rtd_inl(priv->wrapper_regs.XHCI_USB3_TOP_HMAC_CTR0_REG);
                        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_HMAC_CTR0_REG,reginfo | BIT(9)|BIT(8));
                        xhci_plat_info("0x%08x=0x%08x\n",
                                priv->wrapper_regs.XHCI_USB3_TOP_HMAC_CTR0_REG,
                                rtd_inl(priv->wrapper_regs.XHCI_USB3_TOP_HMAC_CTR0_REG));
                        reginfo = rtd_inl(0xb805811c);
                        rtd_outl(0xb805811c,reginfo|BIT(26));
                        xhci_plat_info("0x%08x=0x%08x\n",0xb805811c,rtd_inl(0xb805811c));
                        ret = 1;
                }
                mutex_unlock(&pcie_share_mutex);
        }
        return ret;
}
#endif

int xhci_usb3_phy_init_post(struct platform_device *pdev)
{
        struct usb_hcd *hcd = platform_get_drvdata(pdev);
        struct xhci_plat_priv *priv = hcd_to_xhci_priv(hcd);
		unsigned int index = get_usb_application_scene();
		
#if defined(CONFIG_RTK_KDRV_PCIE)
        if(hcd->rsrc_start == 0x18050000){
            //xhci_plat_info("U3 hcd is 0x%pR rsrc_start=0x%08x\n",hcd,hcd->rsrc_start);
            hcd_u3 = hcd;
        }
#endif	
		if((index != 1) && (index != 5)) {
        	rtd_outl(USB3_PHY_CTRL_USB3_ANA_PHY0_reg, 0xe);
        	udelay(450);
		}
		
        if(index == 0) //rescue just use u2 phy
                rtd_outl(USB3_TOP_HMAC_CTR0_reg, 0xc1000311);

#if defined(CONFIG_RTK_KDRV_PCIE) //disable U3 function
        usb_chk_pcie_share(hcd);
#endif

        //u3Rxterm_force_val=0,//Rxterm_force_en=0
        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB_TMP_2_REG,
                 USB3_TOP_USB_TMP_2_usb3_eco_option1(1));
        return 0;
}

int xhci_usb_mac_postinit(struct platform_device *pdev)
{
        struct usb_hcd *hcd = platform_get_drvdata(pdev);
        struct xhci_global_regs __iomem *global_regs = hcd_to_xhci_priv(hcd)->global_regs;
        struct xhci_plat_priv *priv = hcd_to_xhci_priv(hcd);

        ////////////let usb 3.0 R Term turn on////////////
        //Rxterm_force_en=1
        unsigned int i = 0;
        unsigned int set_value = 0;
		unsigned int index = get_usb_application_scene();

        for(i  = 0; i < priv->usb3_port_cnt; i++)
                set_value |= (1 << (USB3_TOP_USB_TMP_2_usb3_rxterm_force_en_p0_shift + (i * 2)));
        set_value |=  USB3_TOP_USB_TMP_2_usb3_eco_option1(1);

        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB_TMP_2_REG, set_value);

        for(i  = 0; i < priv->usb3_port_cnt; i++)
        {
        		if(index == 1 || index == 5)
					writel(0x010e0003, &global_regs->gusb3pipectl[i]); //P3 mode->p2 mode
				else
                	writel(0x010e0002, &global_regs->gusb3pipectl[i]); //P3 mode->p2 mode
                xhci_plat_info("current mode: %s \n",
                               readl(&global_regs->gusb3pipectl[i]) & BIT(17) ? "p3" : "p2");
        }
		
        return 0;
}
