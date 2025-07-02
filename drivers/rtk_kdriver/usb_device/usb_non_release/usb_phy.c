#include "arch.h"
#include "mach.h"
#include <rbus/usb3_top_reg.h>
#include <rbus/efuse_reg.h>
//#include <rbus/typec_cc_logic_tc0_reg.h>
//#include <rbus/combo_u3_dp_aphy_reg.h>
#include <rbus/sys_reg_reg.h>
#include "usb_mac.h"
#include "usb_reg.h"
#include <linux/timer.h>
#include "rtk_io.h"
#include "usb_phy.h"
//#include <mach/rtk_platform.h>
#include "usb_buf_mgr.h"
#include "software_feature.h"
#include "msg_queue_ap_def.h"
#include <rbus/mis_gpio_reg.h>
#include <rtk_kdriver/io.h>
#include "rtk_usb_device_dbg.h"

#include <linux/delay.h>

//#pragma nogcse
//PRAGMA_CODESEG(BANK43);
//PRAGMA_CONSTSEG(BANK43);

//#define HWSD_USE

UINT32 usb_phy_temp;
UINT8 *usb_addr_vir;
UINT32 usb_plug;
extern UINT32 usb_addr_phy;
extern struct dwc3 g_rtk_dwc3;
extern UINT8 rtk_dwc3_usb2_0_only;

#define USB3_10G

#define USB_PHY_DELAY()    delayus(50)

//#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

/**
 * _get_usb2_phy - get USB 2.0 phy via UTMI
 * @port1:   port number (start from 1)
 * @addr :   phy register address
 *
 * Returnn phy val if successed, otherwise errno
 */
 #if 0
static UINT8 _dwc3_get_usb2_phy(UINT8 port1, UINT8 addr)
{
        UINT32 phy_value = 0;
        do
        {
                rtd_outl(DWC3_GUSB2PHYACC(port1 - 1),
                            0x02000000 | (addr & 0x0f) << 8);
                USB_PHY_DELAY();

                rtd_outl(DWC3_GUSB2PHYACC(port1 - 1),
                            0x02000000 | (addr & 0xd0) << 4);
                USB_PHY_DELAY();

                phy_value = rtd_inl(DWC3_GUSB2PHYACC(port1 - 1));

        }
        while (phy_value & DWC3_GUSB2PHYACC_BUSY);
        /* VstsBusy bit = 0 promise PHY register access has completed */

        return DWC3_GUSB2PHYACC_DATA(phy_value);
}
#endif

/**
 * dwc3_get_usb2_phy - get USB 2.0 phy from addr in specific page
 * @dwc  :
 * @port1:   port number (start from 1)
 * @addr :   phy register address
 * @page :   what page that phy addr stay in.
 *
 * Returnn val of phy if successed, otherwise errno
 */
 /*
int dwc3_get_usb2_phy(unsigned int port1,
                      unsigned char page, unsigned int addr)
{
        int ret = 0;

        if (page > 3)
        {
                RTK_USB_DEVICE_DBG_SIMPLE( "%s: invalid page - %u\n", __func__, page);
                return -1;
        }

        ret = _dwc3_set_usb2_phy(port1, 0xf4, (page << 5) | 0x9b);  // bit [6:5] : page select = page
        if (ret)
                return ret;

        return _dwc3_get_usb2_phy(port1, addr);
}
*/

/**
 * dwc3_set_usb2_phy - set USB 2.0 phy for addr in specific page
 * @dwc  :
 * @port1:   port number (start from 1)
 * @addr :   phy register address
 * @page :   what page that phy addr stay in.
 * @val  :   val want to set
 *
 * Returnn 0 if successed, otherwise errno
 */
 #if 0
int dwc3_set_usb2_phy(UINT8 port1,
                     UINT8 page, UINT8 addr, UINT8 val)
{
        int ret = 0;
        ret = _dwc3_set_usb2_phy(port1, 0xf4, (page << 5) | 0x9b);  // bit [6:5] : page select = page
        if (ret)
                return ret;

        return _dwc3_set_usb2_phy(port1, addr, val);
}
#endif

UINT8 dwc3_usb2_load_phy(void)
{
#if 0
        //////////////////////////XHCI USB2.0 port0 ///////////////////////////////////
        /////// 0xF4_change Page1 //////
        rtd_outl(0x18069014,0x000000bb);     
        rtd_outl(0x18058280,0x00000400);
        rtd_outl(0x18058280,0x02000400); 
        rtd_outl(0x18058280,0x00000f00);
        rtd_outl(0x18058280,0x02000f00);

        /////// 0xE5/Page1 ///////
        rtd_outl(0x18069014,0x0000000f); 
        rtd_outl(0x18058280,0x00000500);
        rtd_outl(0x18058280,0x02000500);
        rtd_outl(0x18058280,0x00000e00);
        rtd_outl(0x18058280,0x02000e00);

        /////// 0xE6/Page1 ///////
        rtd_outl(0x18069014,0x00000058); 
        rtd_outl(0x18058280,0x00000600);
        rtd_outl(0x18058280,0x02000600);
        rtd_outl(0x18058280,0x00000e00);
        rtd_outl(0x18058280,0x02000e00);

        /////// 0xE7/Page1 ///////
        rtd_outl(0x18069014,0x000000e3); 
        rtd_outl(0x18058280,0x00000700);
        rtd_outl(0x18058280,0x02000700);
        rtd_outl(0x18058280,0x00000e00);
        rtd_outl(0x18058280,0x02000e00);

        /////// 0xF4_change Page0 //////
        rtd_outl(0x18069014,0x0000009b);    //Select Page0	
        rtd_outl(0x18058280,0x00000400);
        rtd_outl(0x18058280,0x02000400);
        rtd_outl(0x18058280,0x00000f00);
        rtd_outl(0x18058280,0x02000f00);

        /////// 0xE0 ///////
        rtd_outl(0x18069014,0x00000017);    //Z0=5(0x17)
        //rtd_outl(0x18069014,0x00003e3e3);    //Z0 autoK-->e3
        rtd_outl(0x18058280,0x00000000);
        rtd_outl(0x18058280,0x02000000);
        rtd_outl(0x18058280,0x00000e00);
        rtd_outl(0x18058280,0x02000e00);

        /////// 0xE1 ///////
        //rtd_outl(0x18069014,0x00000018);  			
        //rtd_outl(0x18058280,0x00000100);
        //rtd_outl(0x18058280,0x02000100);
        //rtd_outl(0x18058280,0x00000e00);
        //rtd_outl(0x18058280,0x02000e00);

        /////// 0xE2 /////// 
        //rtd_outl(0x18069014,0x00000078);
        //rtd_outl(0x18058280,0x00000200);
        //rtd_outl(0x18058280,0x02000200);
        //rtd_outl(0x18058280,0x00000e00);
        //rtd_outl(0x18058280,0x02000e00);

        /////// 0xE3 ///////
        //rtd_outl(0x18069014,0x000000c5);  			
        //rtd_outl(0x18058280,0x00000300);
        //rtd_outl(0x18058280,0x02000300);
        //rtd_outl(0x18058280,0x00000e00);
        //rtd_outl(0x18058280,0x02000e00);

        /////// 0xE4 ///////
        rtd_outl(0x18069014,0x00000095);    //swing bit[4:0]=0x15
        rtd_outl(0x18058280,0x00000400);
        rtd_outl(0x18058280,0x02000400);
        rtd_outl(0x18058280,0x00000e00);
        rtd_outl(0x18058280,0x02000e00);

        //////// 0xE5 ////////
        rtd_outl(0x18069014,0x00000039);    //double sensitivity 1.5x
        rtd_outl(0x18058280,0x00000500);
        rtd_outl(0x18058280,0x02000500);
        rtd_outl(0x18058280,0x00000e00);
        rtd_outl(0x18058280,0x02000e00);

        /////// 0xF4_change Page1 ////
        rtd_outl(0x18069014,0x000000bb);    //Select Page1
        rtd_outl(0x18058280,0x00000400);
        rtd_outl(0x18058280,0x02000400);
        rtd_outl(0x18058280,0x00000f00);
        rtd_outl(0x18058280,0x02000f00);

        /////// 0xE1/Page1 ////////////////
        rtd_outl(0x18069014,0x0000001f);    // SRCbit[4:2]=7, NSQD_Cal bit[1]=1,NSQH_Cal bit[0]=1  
        rtd_outl(0x18058280,0x00000100);
        rtd_outl(0x18058280,0x02000100);    
        rtd_outl(0x18058280,0x00000e00);
        rtd_outl(0x18058280,0x02000e00);

        /////// 0xF4_change Page2 ////
        rtd_outl(0x18069014,0x000000db);    //Select Page2
        rtd_outl(0x18058280,0x02000400);
        rtd_outl(0x18058280,0x02000f00);
        rtd_outl(0x18058284,0x02000400);
        rtd_outl(0x18058284,0x02000f00);

        /////// 0xE7 page2 //////           
        rtd_outl(0x18069014,0x00000044);    //bit[7:4]=4,SENH OBJ, bit[3:0]=4,SEND OBJ	
        rtd_outl(0x18058280,0x00000700);
        rtd_outl(0x18058280,0x02000700);
        rtd_outl(0x18058280,0x00000e00);
        rtd_outl(0x18058280,0x02000e00);

        /////// 0xF4_change Page0 ////
        rtd_outl(0x18069014,0x0000009b);     //Select Page0
        rtd_outl(0x18058280,0x00000400);
        rtd_outl(0x18058280,0x02000400);    
        rtd_outl(0x18058280,0x00000f00);
        rtd_outl(0x18058280,0x02000f00);

        /////// 0xE7_ç¬¬ä?æ¬¡è¨­å®?///////
        rtd_outl(0x18069014,0x00000081);  	//bit[7:4] device disconnt level
        rtd_outl(0x18058280,0x00000700);
        rtd_outl(0x18058280,0x02000700);
        rtd_outl(0x18058280,0x00000e00);
        rtd_outl(0x18058280,0x02000e00);

        /////// 0xF4 //////
        rtd_outl(0x18069014,0x000000bb);  	//Select Page1	
        rtd_outl(0x18058280,0x00000400);
        rtd_outl(0x18058280,0x02000400);
        rtd_outl(0x18058280,0x00000f00);
        rtd_outl(0x18058280,0x02000f00);

        /////// 0xE0/page1 //////
        rtd_outl(0x18069014,0x00000021);       //bit[2]=0,Calibration sensitivity
        rtd_outl(0x18058280,0x00000000);
        rtd_outl(0x18058280,0x02000000);
        rtd_outl(0x18058280,0x00000e00);
        rtd_outl(0x18058280,0x02000e00);

        rtd_outl(0x18069014,0x00000025);       //bit[2]=1,Calibration sensitivity
        rtd_outl(0x18058280,0x00000000);
        rtd_outl(0x18058280,0x02000000);
        rtd_outl(0x18058280,0x00000e00);
        rtd_outl(0x18058280,0x02000e00);

        /////// 0xF4 //////
        rtd_outl(0x18069014,0x0000009b);    //Select Page0	
        rtd_outl(0x18058280,0x00000400);
        rtd_outl(0x18058280,0x02000400);
        rtd_outl(0x18058280,0x00000f00);
        rtd_outl(0x18058280,0x02000f00);

        /////// 0xE7_ç¬¬ä?æ¬¡è¨­å®?///////
        rtd_outl(0x18069014,0x00000081);  //bit[7:4]=8 device disconnt level
        rtd_outl(0x18058280,0x00000700);
        rtd_outl(0x18058280,0x02000700);
        rtd_outl(0x18058280,0x00000e00);
        rtd_outl(0x18058280,0x02000e00);

        /////// 0xE6 ///////
        //rtd_outl(0x18069014,0x00000001);    //bit[1:0]=1, RX BOOST
        //rtd_outl(0x18058280,0x00000600);
        //rtd_outl(0x18058280,0x02000600);
        //rtd_outl(0x18058280,0x00000e00);
        //rtd_outl(0x18058280,0x02000e00);

        /////// 0xF0 ///////
        //rtd_outl(0x18069014,0x000000fc); 		
        //rtd_outl(0x18058280,0x00000000);
        //rtd_outl(0x18058280,0x02000000);
        //rtd_outl(0x18058280,0x00000f00);
        //rtd_outl(0x18058280,0x02000f00);

        /////// 0xF1 ///////
        //rtd_outl(0x18069014,0x0000008c);  			
        //rtd_outl(0x18058280,0x00000100);
        //rtd_outl(0x18058280,0x02000100);
        //rtd_outl(0x18058280,0x00000f00);
        //rtd_outl(0x18058280,0x02000f00);

        /////// 0xF2 ///////
        //rtd_outl(0x18069014,0x00000000);  			
        //rtd_outl(0x18058280,0x00000200);
        //rtd_outl(0x18058280,0x02000200);
        //rtd_outl(0x18058280,0x00000f00);
        //rtd_outl(0x18058280,0x02000f00);

        /////// 0xF3 ///////
        rtd_outl(0x18069014,0x00000011); 
        //rtd_outl(0x18058280,0x00000300);
        //rtd_outl(0x18058280,0x02000300);
        //rtd_outl(0x18058280,0x00000f00);
        //rtd_outl(0x18058280,0x02000f00);

        /////// 0xF5 ///////
        rtd_outl(0x18069014,0x00000015);  		
        //rtd_outl(0x18058280,0x00000500);
        //rtd_outl(0x18058280,0x02000500);
        //rtd_outl(0x18058280,0x00000f00);
        //rtd_outl(0x18058280,0x02000f00);

        /////// 0xF6 ///////
        //rtd_outl(0x18069014,0x00000000);    //fix full speed cross eye  			
        //rtd_outl(0x18058280,0x00000600);
        //rtd_outl(0x18058280,0x02000600);
        //rtd_outl(0x18058280,0x00000f00);
        //rtd_outl(0x18058280,0x02000f00);

        /////// 0xF7 ///////
        rtd_outl(0x18069014,0x000000ba);    //bit 8=1 usb FIFO new mode under 16, over 12 bit
        rtd_outl(0x18058280,0x00000700);
        rtd_outl(0x18058280,0x02000700);
        rtd_outl(0x18058280,0x00000f00);
        rtd_outl(0x18058280,0x02000f00);

        /////// page 0 //////
        rtd_outl(0x18069014,0x00009b9b);  	///9b=page 0
        rtd_outl(0x18058280,0x00000400);
        rtd_outl(0x18058280,0x02000400);
        rtd_outl(0x18058280,0x00000f00);
        rtd_outl(0x18058280,0x02000f00);
#endif
        return 0;
}

void ScalerTimer_DelayXms(UINT32 time)
{
//		delayms(time);
    RTK_USB_DEVICE_DBG_SIMPLE( "rtk_usb_device ### ScalerTimer_DelayXms need to implement ----------------\n");
}

#if 0
void rtd_part_outl_usb(UINT32 reg_addr, UINT32 endBit, UINT32 startBit, UINT32 value)
{
    rtd_outl(reg_addr,(rtd_inl(reg_addr) & (~((((UINT32)1<<(endBit-startBit+1))-1)<<startBit))) | (value<<startBit));
}
#endif

void step_0_USB3_init_V01_CRT_XHCI(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "rtk_usb_device ### step_0_USB3_init_V01_CRT_XHCI need to implement ----------------\n");
}

UINT8 usb3_phy_dump_dfe(void)
{
   return 0;
}


//Note mac settings in PHY
//rtd_outl(0x18058700,0x00480804);//[2:0] 0xc700 Device speed

void usb_dv_setting(void)
{
    RTK_USB_DEVICE_DBG_SIMPLE( "MAC tPWM range & deemp setup 0x3 0x7\n");
    rtd_outl(0xb80ad03c,0xee91752d);
    rtd_outl(0xb80ad040,0x000284f0);
    //post=0x7 main=0x3f pre=0x3
    rtd_outl(0xb80ad060,0x7fc3);//for normal link
    rtd_outl(0xb80ad064,0xfc3);//for CP13 post=0x0
    rtd_outl(0xb80ad068,0x7fc0);//for CP14 pre=0x0
    rtd_outl(0xb80ad06c,0xfc0);//for CP16 pre and post=0x0
}

UINT8 is_usb_flip(void)
{
#if 0
    if(GetPD_Mode() != _PD_FUNCTION_DISABLE) {
        // PD mode
        return TYPEC_CC_LOGIC_TC0_CC_HW_FSM_get_hw_cc_port_sel(rtd_inl(TYPEC_CC_LOGIC_TC0_CC_HW_FSM_reg)) ? 1 : 0;
    }
    else {
        // non PD mode
        return ((rtd_inl(MIS_GPIO_GP0DATI_reg) & 0x08000000) == 0x08000000) ? 0 : 1;         // GP_26
    }
#else
    RTK_USB_DEVICE_DBG_SIMPLE( "rtk_usb_device ### is_usb_flip need to implement ----------------\n");
    return 0;
#endif
}

void usb_insert_direct_setting(void)
{
#if 0
    if(Get_Ext_DP_MUX_Select()==_Ext_DP_MUX_Select_EJ899I) {
        // Reverse
         RTK_USB_DEVICE_DBG_SIMPLE( "Fix Cable Reverse insertion\n");
        
         usb_phy_temp = rtd_inl(SYS_REG_SYS_CLKSEL_reg);
         usb_phy_temp = usb_phy_temp | (SYS_REG_SYS_CLKSEL_usb3_lane_sel(1));
         rtd_outl(SYS_REG_SYS_CLKSEL_reg, usb_phy_temp);
        
         usb_phy_temp = rtd_inl(USB3_TOP_USB3_PHY_CONFIG_reg);
         usb_phy_temp = usb_phy_temp | (USB3_TOP_USB3_PHY_CONFIG_reg_usb3_pipe_lane_config(1));
         rtd_outl(USB3_TOP_USB3_PHY_CONFIG_reg, usb_phy_temp);
         usb_plug = 0x3;
    } else if( (Get_DP_USB_Lane_Fix_Enable() == _FUNCTION_ENABLE) || (Get_Ext_DP_MUX_Select()==_Ext_DP_MUX_Select_EJ899M) || (Get_Ext_DP_MUX_Select()==_Ext_DP_MUX_Select_CYPD3176)) {
        // Positive
        RTK_USB_DEVICE_DBG_SIMPLE( "Fix Cable Positive insertion\n");

        usb_phy_temp = rtd_inl(SYS_REG_SYS_CLKSEL_reg);
        usb_phy_temp = usb_phy_temp & ~(SYS_REG_SYS_CLKSEL_usb3_lane_sel(1));
        rtd_outl(SYS_REG_SYS_CLKSEL_reg, usb_phy_temp);

        usb_phy_temp = rtd_inl(USB3_TOP_USB3_PHY_CONFIG_reg);
        usb_phy_temp = usb_phy_temp & ~(USB3_TOP_USB3_PHY_CONFIG_reg_usb3_pipe_lane_config(1));
        rtd_outl(USB3_TOP_USB3_PHY_CONFIG_reg, usb_phy_temp);
        usb_plug = 0x1;
    } else {
        if(is_usb_flip()) {
            // Positive
            RTK_USB_DEVICE_DBG_SIMPLE( "USB Cable Positive insertion\n");
        
            usb_phy_temp = rtd_inl(SYS_REG_SYS_CLKSEL_reg);
            usb_phy_temp = usb_phy_temp & ~(SYS_REG_SYS_CLKSEL_usb3_lane_sel(1));
            rtd_outl(SYS_REG_SYS_CLKSEL_reg, usb_phy_temp);
        
            usb_phy_temp = rtd_inl(USB3_TOP_USB3_PHY_CONFIG_reg);
            usb_phy_temp = usb_phy_temp & ~(USB3_TOP_USB3_PHY_CONFIG_reg_usb3_pipe_lane_config(1));
            rtd_outl(USB3_TOP_USB3_PHY_CONFIG_reg, usb_phy_temp);
            usb_plug = 0x1;
        } else {
           // Reverse
            RTK_USB_DEVICE_DBG_SIMPLE( "USB Cable Reverse insertion\n");
        
            usb_phy_temp = rtd_inl(SYS_REG_SYS_CLKSEL_reg);
            usb_phy_temp = usb_phy_temp | (SYS_REG_SYS_CLKSEL_usb3_lane_sel(1));
            rtd_outl(SYS_REG_SYS_CLKSEL_reg, usb_phy_temp);
        
            usb_phy_temp = rtd_inl(USB3_TOP_USB3_PHY_CONFIG_reg);
            usb_phy_temp = usb_phy_temp | (USB3_TOP_USB3_PHY_CONFIG_reg_usb3_pipe_lane_config(1));
            rtd_outl(USB3_TOP_USB3_PHY_CONFIG_reg, usb_phy_temp);
            usb_plug = 0x3;
        }
    }
#else
    RTK_USB_DEVICE_DBG_SIMPLE( "rtk_usb_device ### usb_insert_direct_setting need to implement ----------------\n");
#endif
}

void rtk_usb_phy3_init_common(void)
{
RTK_USB_DEVICE_DBG_SIMPLE( "USB3 Gen2 PHY v0918\n");
rtd_outl(0xb8000138,0x06000000);
rtd_outl(0xb8000128,0x18000000);
rtd_outl(0xb8000128,0x18000001);
rtd_outl(0xb8000138,0x06000001);//usb3 dev dwc clk
rtd_outl(0xb8000104,0x13000011);
rtd_outl(0xb8000114,0x13000001);
rtd_outl(0xb80ac100,0x0000001e);//dwc mac global bus
rtd_outl(0xb800010c,0x00000011);
rtd_outl(0xb8000128,0x06000000);
rtd_outl(0xb8000128,0x06000001);//usb3 gen2 phy rst toggle
//PHY access start
rtd_outl(0xb80db004,0x80000000);//[31]enable mdio=1
rtd_outl(0xb80db010,0x00000011);//[0]usb3_gen2_phy_enable=1, [4]
rtd_outl(0xb80db000,0x30840005);
rtd_outl(0xb80db004,0x80000002);//phy_write

//APHY reg=0x155
rtd_outl(0xb80db000,0x3c850155);//[13]REG0_RX_INVOOBS_EN_5G=1
rtd_outl(0xb80db004,0x80000002);//phy_write

//APHY reg=0x1a5
rtd_outl(0xb80db000,0x3c8501a5);//[13]REG0_RX_INVOOBS_EN_10G=1
rtd_outl(0xb80db004,0x80000002);//phy_write

//APHY reg=0x159
rtd_outl(0xb80db000,0x05850159);
rtd_outl(0xb80db004,0x80000002);//phy_write

//APHY reg=0x1a9
rtd_outl(0xb80db000,0x058501a9);
rtd_outl(0xb80db004,0x80000002);//phy_write

//APHY reg=0x1b2

rtd_outl(0xb80db000,0x012001b2);
rtd_outl(0xb80db004,0x80000002);//phy_write

//APHY reg=0x162
rtd_outl(0xb80db000,0x01200162);
rtd_outl(0xb80db004,0x80000002);//phy_write


rtd_outl(0xb80db000,0x30840005);
rtd_outl(0xb80db004,0x80000002);//phy_write

//[1]ckrdy_sel=1, [2]v2a_delay_ana_en=1,[3]oobs_gated=1, 
//[6]px_lfps_sel=1,[7]p1_lfps_sel=1,[15:8]delay T clk after cmu ready=0x8000
rtd_outl(0xb80db000,0x08ce1c02);
rtd_outl(0xb80db004,0x80000002);//phy_write

//[0]phy_reset_n=1, [1]phy_off_reset_n=1, [2]phy_clk_en=1, [3]low_latency=1
//[4]rx_negedge_latch=1, [6]noclk_pipeline_en=1, [12]rx_en_sel=1
rtd_outl(0xb80db000,0x125f1c00);////[9]tx_deemp_en=1
rtd_outl(0xb80db004,0x80000002);//phy_write
rtd_outl(0xb80db000,0x00031c08);//reg=1c08 [15:0]p2p0_delay_5g=0x3
rtd_outl(0xb80db004,0x80000002);//phy_write
rtd_outl(0xb80db000,0x00031c0a);//reg=1c0a [15:0]p2p0_delay_10g=0x3
rtd_outl(0xb80db004,0x80000002);//phy_write

rtd_outl(0xb80db000,0x93001194);//REG0_ST_MODE_5G[15]=1
rtd_outl(0xb80db004,0x80000002);//[1] mdio write
rtd_outl(0xb80db000,0x930011c4);//REG0_ST_MODE_10G[15]=1
rtd_outl(0xb80db004,0x80000002);//[1] mdio write


//[9:0]offset_timeout=0x3F, [11]oofset_oobs_ok=1, [12]offset_oobs2_gray_en=0
//[13]offset_oobs1_pc=0, [14]offset_oobs1_ok=1,[15]offset_oobs1_gray_en=0
//rtd_outl(0xb80db000,0x4BFF100c);
//rtd_outl(0xb80db004,0x80000002);//phy_write

//CDR_DFE reg=100a, [0]offset_oobs_pc=0,[1]offset_oobs_ok=1 
//[3]offset_oobs_gray_en=0, [9:4]ofset_divisor=0x8
rtd_outl(0xb80db000,0x0082100a);
rtd_outl(0xb80db004,0x80000002);//phy_write

//CDR_DFE reg=100a,[9:4]ofset_divisor=0x1
rtd_outl(0xb80db000,0x0012100a);
rtd_outl(0xb80db004,0x80000002);//phy_write

//FLD_SSC reg=18c0, [0]ssc_en_5g=0, [1]ssc_set_valid_5g=0
rtd_outl(0xb80db000,0x000018c0);
rtd_outl(0xb80db004,0x80000002);//phy_write

//FLD_SSC reg=18d0, [0]ssc_en_10g=0, [1]ssc_set_valid_10g=0
rtd_outl(0xb80db000,0x000018d0);
rtd_outl(0xb80db004,0x80000002);//phy_write

//FLD_SSC reg=1936, [8:0]ssc_up_n_code_5g=0xc6
rtd_outl(0xb80db000,0x00c61936);
rtd_outl(0xb80db004,0x80000002);//phy_write

//FLD_SSC reg=1946, [8:0]ssc_up_n_code_10g=0xc6
rtd_outl(0xb80db000,0x00c61946);
rtd_outl(0xb80db004,0x80000002);//phy_write

rtd_outl(0xb80db000,0x00011c40);//[0]spd_page_load=1
rtd_outl(0xb80db004,0x80000002);//phy_write
rtd_outl(0xb80db000,0x00001c40);//[0]spd_page_load=0
rtd_outl(0xb80db004,0x80000002);//phy_write

rtd_outl(0xb80db000,0x000218c0);//[1]ssc_set_valid_10g=1
rtd_outl(0xb80db004,0x80000002);//phy_write
rtd_outl(0xb80db000,0x000018c0);//[1]ssc_set_valid_10g=0
rtd_outl(0xb80db004,0x80000002);//phy_write

rtd_outl(0xb80db000,0x000218d0);//[1]ssc_set_valid_10g=1
rtd_outl(0xb80db004,0x80000002);//phy_write
rtd_outl(0xb80db000,0x000018d0);//[1]ssc_set_valid_10g=0
rtd_outl(0xb80db004,0x80000002);//phy_write
rtd_outl(0xb800021c,0x00000000);//mux set to 10G PHY and device MAC


rtd_outl(0xb80db000,0x00011C40); //1c40 default=0000
rtd_outl(0xb80db004,0x80000002);

rtd_outl(0xb80db000,0xE0FA0153);//<3>rx_test_en  DPHY default=E0F2
rtd_outl(0xb80db004,0x80000002);

rtd_outl(0xb80db000,0xE0FA01A3);//<3>rx_test_en  DPHY default=E0F2
rtd_outl(0xb80db004,0x80000002);
msleep(5);

//////////EQ//////////////////

rtd_outl(0xb80db000,0x100011A4);//if EQ manual 5G, [15]=1 and adjust at [14:10]
rtd_outl(0xb80db004,0x80000002);

rtd_outl(0xb80db000,0x100011D4);//if EQ manual 10G, [15]=1 and adjust at [14:10]
rtd_outl(0xb80db004,0x80000002);

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
rtd_outl(0xb80db000,0xA5850159);//[15:12]RX_OOBS_VCM=0xA
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x0232015A);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x0F16015E);//5G rx_reserved[5:4]spd_sel=01,[3]iboost_vga_leq=0 ,[0] no use
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x01200162);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x23020163);//RX_Z0=0x8
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x001C0167);//post1_amp=0x7
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x00C00168);//pre_amp=0x3
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x00E0016B);//5G TX_RCALI_ADJR=0x7
rtd_outl(0xb80db004,0x80000002);
//rtd_outl(0xb80db000,0x03E8016C);//5G RXD_SEL=[00],TX_VCM_SEL=[10]
rtd_outl(0xb80db000,0x03E1016C);//5G RXD_SEL=[01],TX_VCM_SEL=[00]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x0010016D);//5G TX_SR_CTRL=0x4
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x1111016E);//5G TX_Z0SEL=0x2, TX_VCM_DET_MODE=[1]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x69E30195);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x001F019B);//5G LPF_RP=[11],LPF_3RD_SEL=[1]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x0031019C);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0xC31D01A2);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x3C9501A5);//1C85--->3C85// 3c95 ibrx_adj=[10]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x380C01A6);//10G [1:0]ivga_adj=[11]   [13:11]=leq_ac_range[111]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0xEA7A01A7);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x614001A8);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0xA58501A9);//[15:12]RX_OOBS_VCM=0xA
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x023201AA);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x0F3601AE);//rx_reserved[5:4]spd_sel=11,[3]iboost_vga_leq=0, [0] no use
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x012001B2);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x230301B3);//RX_Z0=0x8
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x001C01B7);//post1_amp=0x7
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x00C001B8);//pre_amp=0x3
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x00E001BB);//10G TX_RCALI_ADJR=0x7
rtd_outl(0xb80db004,0x80000002);
//rtd_outl(0xb80db000,0x03E801BC);//10G RXD_SEL=[00],TX_VCM_SEL=[10]
rtd_outl(0xb80db000,0x03E101BC);//10G RXD_SEL=[01],TX_VCM_SEL=[00]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x001001BD);//10G TX_SR_CTRL=0x4
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x111101BE);//10G TX_VCM_DET_MODE=[1],10G TX_Z0SEL=0x2
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x69E301E5);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x001F01EB);//10G LPF_RP=[11],LPF_3RD_SEL=[1]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x003101EC);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x0C0E0200);//5G<13>RCV_VREF_TYPE=[0]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x0C0E0250);//10G<13>RCV_VREF_TYPE=[0]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x299C0202);//5G CMU_PI_MAIN_IBSEL=[0100]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x299C0252);//10G CMU_PI_MAIN_IBSEL=[0100]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x4BFF100c);//offset_cal_timeout
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x91001192);//gen1 BBW_ST,KP=0x4,KI
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x91001194);//gen1 ST,KP=0x4,KI
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x6D10119C);//HPERIOD=0x110  5G_NEW mode
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x4020119E);//LPERIOD=0x20  5G_NEW mode
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0xE87F11B2);//[14:12]rotation mode=[110]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x0EA511BB);//reg_leq_mf_mode=[101],mf_rl=0,mf_gray_en=1,mf_inv=0,mf_en=1,mf_gain=111
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x020911BD);//ZF_MODE &ZF2_mode & ZF3_mode=edg mode
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x01CF11BF);//mf_max=1111, mf_init=111
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0xE87F11E2);//[14:12]rotation mode=[110]
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x020911ED);//ZF_MODE &ZF2_mode & ZF3_mode=edg mode
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x6DF911CC);//HPERIOD=0x1F9  10G_new mode
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x404011CE);//LPERIOD=0x40  10G_new mode
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x90C011C2);//gen2 BBW_ST,KP,KI
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x90C011C4);//gen2 ST,KP,KI
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x0eA511EB);//reg_leq_mf_mode=[101],mf_rl=0,mf_gray_en=1,mf_inv=0,mf_en=1,mf_gain=111
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x020911ED);//ZF_MODE &ZF2_mode & ZF3_mode=edg mode
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x01CF11EF);//mf_max=1111, mf_init=111
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x08CE1C02);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x002C1842);//5G divide_num=0x2C
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x032E1844);//5G up_limit=0x32E
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x032E1846);//5G dn_limit=0x32E
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x002C1852);//10G divide_num=0x2C
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x032E1854);//10G up_limit=0x32E
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x032E1856);//10G dn_limit=0x32E
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x00B718C2);//N code
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x00B718D2);//N code
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0xF68418C4);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x000218C6);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x07F718C8);//5G step_in<15:0>=0x7f7
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x000018CA);//5G step_in<19:16>
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x035918CC);//5G Tbase=0x359
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0xF68418C4);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x000218C6);
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x07F718D8);//10G step_in<15:0>=0x7f7
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x000018DA);//10G step_in<19:16>
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x035918DC);//10G Tbase=0x359
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x000218D6);//10G fcode
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0xF68418D4);//10G fcode
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x00181C5C);//[3][4],1:deemp control by post1_amp, 0:deemp control by post1_6dB_amp
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x0812100A);//[11:10]reg_offset_stable_cnt=[10] 
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0xFF831C48);//[7]TXCMON always on
rtd_outl(0xb80db004,0x80000002);

////////////////////tap manual all level 0//////
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
rtd_outl(0xb80db000,0x000211E4);//tap1=1,tap2~4 adapt_en=0
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x000211B4);//tap1=1,tap2~4 adapt_en=0
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0xFF001022);//tap1 th min=0, tap1 max/min=F, tap2 max=0
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x0F001024);//tap2 th min=F ,tap2 min=0 ,tap3 max/min=0
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0xF00F1026);//tap3/4 th min=F, tap4 max/min=0
rtd_outl(0xb80db004,0x80000002);
rtd_outl(0xb80db000,0x4000102B);//reg0_th_min_en<8:0>=000000000
rtd_outl(0xb80db004,0x80000002);

/////////////////////SSC_ON & toggle valid///////////////////////
rtd_outl(0xb80db000,0x000318C0);//toggle setvalid
rtd_outl(0xb80db004,0x80000002);

rtd_outl(0xb80db000,0x000118C0);//toggle setvalid
rtd_outl(0xb80db004,0x80000002);

rtd_outl(0xb80db000,0x000318D0);//toggle setvalid 100
rtd_outl(0xb80db004,0x80000002);

rtd_outl(0xb80db000,0x000118D0);//toggle setvalid
rtd_outl(0xb80db004,0x80000002);

msleep(5);
}

void rtk_usb_phy3_init_EQ(void)
{
    unsigned int usb32_offset_k_tmp = 0;
    unsigned int usb32_offset_k_value = 0;
    unsigned char usb_phy_temp = 0;
    //FW offset cal flow
    //1 rate clock gating
    rtd_outl(0xb80db000,0x00071c3e);
    rtd_outl(0xb80db004,0x80000002);
    //2 load all setting of APHY, CDR, DFE
    rtd_outl(0xb80db000,0x00011c40);
    rtd_outl(0xb80db004,0x80000002);
    
    //3 Set N_CODE, DIVIDE_NUM, UP/DN LIMIT
    rtd_outl(0xb80db000,0x00b71f52); // USB3G1
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00b71f62); // USB3G2
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0xf6841f54); // USB3G1
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0xf6841f64); // USB3G2
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00021f56); // USB3G1
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00021f66); // USB3G2
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00b71f58); // USB3G1
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00b71f68); // USB3G2
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00271f5a); // USB3G1
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00271f6a); // USB3G2
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x02e41f5c); // USB3G1
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x02e41f6c); // USB3G2
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x02e41f5e); // USB3G1
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x02e41f6e); // USB3G2
    rtd_outl(0xb80db004,0x80000002);
    
    //For Gen1
    //1 set test mode and reset
    rtd_outl(0xb80db000,0x000d1f00);
    rtd_outl(0xb80db004,0x80000002);
    //2 clear CMU EN/RX_EN/RSTB_BITERR/OOBS_RSTB/CLKWR_EN/DATA_EN
    rtd_outl(0xb80db000,0x00001f02);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00001f04);
    rtd_outl(0xb80db004,0x80000002);
    //3 set CMU EN/RX_EN/RSTB_BITERR/OOBS_RSTB/CLKWR_EN/DATA_EN
    rtd_outl(0xb80db000,0xfc011f02);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x003c1f04);
    rtd_outl(0xb80db004,0x80000002);
    //4 Release reset
    rtd_outl(0xb80db000,0x000f1f00);
    rtd_outl(0xb80db004,0x80000002);
    //5 Set Offset debug read, Wait Offset_OK = 1
    rtd_outl(0xb80db000,0x00021c44);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00021c46);
    rtd_outl(0xb80db004,0x80000002);
    
    while(1)
    {
        rtd_outl(0xb80db000,0x00001c98);
        rtd_outl(0xb80db004,0x80000001);
        rtd_outl(0xb80db004,0x80000001);
        if( (rtd_inl(0xb80db008) & 0x00800000) == 0x00800000){
            RTK_USB_DEVICE_DBG_SIMPLE("bit 7 done 0x00001c98 = 0x%x\n", rtd_inl(0xb80db008));
            break;
        }
        RTK_USB_DEVICE_DBG_SIMPLE("Gen1 0x00001c98 = 0x%x\n", rtd_inl(0xb80db008));
        if(usb_phy_temp>1000)
            break;
        usb_phy_temp++;
    }
    //6 Set Offset debug read, read & write back to register (offset_oobs1_bin)
    rtd_outl(0xb80db000,0x0002102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x1f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, (usb32_offset_k_tmp | 0x200011B6));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011B6);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11B6 = 0x%x\n", rtd_inl(0xb80db008));
    
    //7 offset_oobs2_bin
    rtd_outl(0xb80db000,0x0003102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x1f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000,0x000011B6);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    usb32_offset_k_value = rtd_inl(0xb80db008);
    usb32_offset_k_tmp = (usb32_offset_k_tmp >> 8) | usb32_offset_k_value;
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, (usb32_offset_k_tmp | 0x202011B6));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011B6);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11B6 = 0x%x\n", rtd_inl(0xb80db008));
    
    //8 offset_sa_bin_edge
    rtd_outl(0xb80db000,0x0004102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x7f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x008012B8));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000012B8);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("12B8 = 0x%x\n", rtd_inl(0xb80db008));
    
    //9 offset_sa_bin_data
    rtd_outl(0xb80db000,0x0005102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x7f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x008011B8));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011B8);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11B8 = 0x%x\n", rtd_inl(0xb80db008));
    
    //10 offset_sa_bin_dh
    rtd_outl(0xb80db000,0x0006102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x1f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, (usb32_offset_k_tmp | 0x200011BA));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011BA);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11BA = 0x%x\n", rtd_inl(0xb80db008));
    
    //11 offset_sa_bin_dl
    rtd_outl(0xb80db000,0x0007102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x1f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000,0x000011BA);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    usb32_offset_k_value = rtd_inl(0xb80db008);
    usb32_offset_k_tmp = (usb32_offset_k_tmp >> 8) | usb32_offset_k_value;
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, (usb32_offset_k_tmp | 0x202011BA));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011BA);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11BA = 0x%x\n", rtd_inl(0xb80db008));
    
    //12 offset_sum_bin
    rtd_outl(0xb80db000,0x0001102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x1f000000);
    //rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x002011BC));
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x000011BC));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011BC);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11BC = 0x%x\n", rtd_inl(0xb80db008));
    
    //13 offset_sum_bin
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0xff000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x040011BE));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011BE);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11BE = 0x%x\n", rtd_inl(0xb80db008));
    
    //14 offset_sa_mon_bin_0
    rtd_outl(0xb80db000,0x0014102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x7f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x008011B5));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011B5);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11B5 = 0x%x\n", rtd_inl(0xb80db008));
    
    //15 offset_sa_mon_bin_1
    rtd_outl(0xb80db000,0x0015102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x7f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x008011B7));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011B7);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11B7 = 0x%x\n", rtd_inl(0xb80db008));
    
    //16 offset_sa_mon_bin_2
    rtd_outl(0xb80db000,0x0016102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x7f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x008011B9));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011B9);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11B9 = 0x%x\n", rtd_inl(0xb80db008));
    
    //for Gen2
    //1 set test mode and reset
    rtd_outl(0xb80db000,0x001d1f00);
    rtd_outl(0xb80db004,0x80000002);
    //2 clear CMU EN/RX_EN/RSTB_BITERR/OOBS_RSTB/CLKWR_EN/DATA_EN
    rtd_outl(0xb80db000,0x00001f02);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00001f04);
    rtd_outl(0xb80db004,0x80000002);
    //3 set CMU EN/RX_EN/RSTB_BITERR/OOBS_RSTB/CLKWR_EN/DATA_E
    rtd_outl(0xb80db000,0xfc011f02);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x003c1f04);
    rtd_outl(0xb80db004,0x80000002);
    //4 release reset
    rtd_outl(0xb80db000,0x001f1f00);
    rtd_outl(0xb80db004,0x80000002);
    //5 Set Offset debug read
    rtd_outl(0xb80db000,0x00021c44);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00021c46);
    rtd_outl(0xb80db004,0x80000002);
    usb_phy_temp = 0;
    while(1)
    {
        rtd_outl(0xb80db000,0x00001c98);
        rtd_outl(0xb80db004,0x80000001);
        rtd_outl(0xb80db004,0x80000001);
        if( (rtd_inl(0xb80db008) & 0x00800000) == 0x00800000){
            RTK_USB_DEVICE_DBG_SIMPLE("bit 7 done 0x00001c98 = 0x%x\n", rtd_inl(0xb80db008));
            break;
        }
        RTK_USB_DEVICE_DBG_SIMPLE("Gen2 0x00001c98 = 0x%x\n", rtd_inl(0xb80db008));
        if(usb_phy_temp>100)
        break;
        usb_phy_temp++;
    }
    //6 offset_oobs1_bin
    rtd_outl(0xb80db000,0x0002102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x1f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, (usb32_offset_k_tmp | 0x200011E6));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011E6);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11E6 = 0x%x\n", rtd_inl(0xb80db008));
    
    //7 offset_oobs2_bin
    rtd_outl(0xb80db000,0x0003102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x1f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000,0x000011E6);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    usb32_offset_k_value = rtd_inl(0xb80db008);
    usb32_offset_k_tmp = (usb32_offset_k_tmp >> 8) | usb32_offset_k_value;
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, (usb32_offset_k_tmp | 0x202011E6));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011E6);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11E6 = 0x%x\n", rtd_inl(0xb80db008));
    
    //8 offset_sa_bin_edge
    rtd_outl(0xb80db000,0x0004102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x7f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x008012E8));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000012E8);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("12E8 = 0x%x\n", rtd_inl(0xb80db008));
    
    //9 offset_sa_bin_data
    rtd_outl(0xb80db000,0x0005102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x7f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x008011E8));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011E8);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11E8 = 0x%x\n", rtd_inl(0xb80db008));
    
    //10 offset_sa_bin_dh
    rtd_outl(0xb80db000,0x0006102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x1f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, (usb32_offset_k_tmp | 0x200011BA));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011EA);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11EA = 0x%x\n", rtd_inl(0xb80db008));
    
    //11 offset_sa_bin_dl
    rtd_outl(0xb80db000,0x0007102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x1f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000,0x000011EA);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    usb32_offset_k_value = rtd_inl(0xb80db008);
    usb32_offset_k_tmp = (usb32_offset_k_tmp >> 8) | usb32_offset_k_value;
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, (usb32_offset_k_tmp | 0x202011EA));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011EA);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11EA = 0x%x\n", rtd_inl(0xb80db008));
    
    //12 offset_sum_bin
    rtd_outl(0xb80db000,0x0001102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x1f000000);
    //rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x002011EC));
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x000011EC));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011EC);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11EC = 0x%x\n", rtd_inl(0xb80db008));
    
    //13 offset_fore_bin
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0xff000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x040011EE));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011EE);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11EE = 0x%x\n", rtd_inl(0xb80db008));
    
    //14 offset_sa_mon_bin_0
    rtd_outl(0xb80db000,0x0014102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x7f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x008011E5));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011E5);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11E5 = 0x%x\n", rtd_inl(0xb80db008));
    
    //15 offset_sa_mon_bin_1
    rtd_outl(0xb80db000,0x0015102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x7f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x008011E7));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011E7);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11E7 = 0x%x\n", rtd_inl(0xb80db008));
    
    //16 offset_sa_mon_bin_2
    rtd_outl(0xb80db000,0x0016102E);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x0000102E);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("102E = 0x%x\n", rtd_inl(0xb80db008));
    rtd_outl(0xb80db000,0x00001030);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    usb32_offset_k_tmp = (rtd_inl(0xb80db008) & 0x7f000000);
    RTK_USB_DEVICE_DBG_SIMPLE("usb32_offset_k_tmp = 0x%x\n", usb32_offset_k_tmp);
    rtd_outl(0xb80db000, ((usb32_offset_k_tmp >> 8) | 0x008011E9));
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x000011E9);
    rtd_outl(0xb80db004,0x80000001);
    rtd_outl(0xb80db004,0x80000001);
    RTK_USB_DEVICE_DBG_SIMPLE("11E9 = 0x%x\n", rtd_inl(0xb80db008));
    //End
    //1 clear all test mode
    rtd_outl(0xb80db000,0x00011c40);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00001f02);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00001f04);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00011f00);
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0x00001f00);
    rtd_outl(0xb80db004,0x80000002);
    //2 clear rate clock gating
    rtd_outl(0xb80db000,0x80071c3e);
    rtd_outl(0xb80db004,0x80000002);
    
    //////EQ servo adapt/////////////////////////////////////
    //RX_EN_KOFFSET_BACKGND=1
    rtd_outl(0xb80db000,0xE0FB0153);//DPHY default=E0F2
    rtd_outl(0xb80db004,0x80000002);
    rtd_outl(0xb80db000,0xE0FB01A3);//DPHY default=E0F2
    rtd_outl(0xb80db004,0x80000002);
    //read servo range after TSEQ
    //rtd_outl(0xb80db000,0x000c102E);
    //rtd_outl(0xb80db004,0x80000002);
    //rtd_outl(0xb80db000,0x00001030);
    //rtd_outl(0xb80db004,0x80000001);
    //rtd_outl(0xb80db004,0x80000001);
    //RTK_USB_DEVICE_DBG_SIMPLE("servo range[1:0]0x00001030 = 0x%x\n", rtd_inl(0xb80db008));
    
msleep(5);
}

UINT8 rtk_usb_phy3_init(void)
{
    rtk_usb_phy3_init_common();
    rtk_usb_phy3_init_EQ();
    return 0;
}

UINT8 dwc3_usb3_load_phy(void)
{
    rtk_usb_phy3_init();
    return 0;
}

UINT8 dwc3_usb3_load_mac(void)
{
    usb_dv_setting();
    return 0;
}

void dwc3_sram_proc(void)
{
    UINT32 size;
    //UINT32 temp;

    usbbuf_init();
    usb_request_usbbuf(USB_SRAM_BUF, &usb_addr_phy, &usb_addr_vir, &size);

    // QFN case: Need to add more UAC sram size, so usb sram size need to offset 0x200.
    if(Get_Package_Type()) {
        usb_addr_phy = usb_addr_phy + 0x200;
        usb_addr_vir = usb_addr_vir + 0x200;
        size = size - 0x200;
    }

    RTK_USB_DEVICE_DBG_SIMPLE( "usb SRAM phy=%x vir=%x size=%x\n", usb_addr_phy, usb_addr_vir, size);

    //dwc3_memset((void *)&g_rtk_dwc3, 0, sizeof(g_rtk_dwc3));

    // Clear SRAM
    //for(temp= (UINT32)usb_addr_vir; temp<(UINT32)(usb_addr_vir+0x1000); temp=temp+4) {
    //    rtd_mem_outl((UINT32)temp, 0x00000000);
    //}
}

void rtk_dwc3_usb_load_phy(void)
{
    dwc3_sram_proc();
    dwc3_usb3_load_phy();
    dwc3_usb2_load_phy();
}

void rtk_usb3_phy_reset(void) __banked
{
    RTK_USB_DEVICE_DBG_SIMPLE( "rtk_usb_device ### rtk_usb3_phy_reset need to implement ----------------\n");
//    rtd_outl(0x180309c0,0x0000005d); //reg_spphy_off_reset_n
//    rtd_outl(0x180309c0,0x0000005f); //reg_spphy_off_reset_n
//    delayms(30);
}

