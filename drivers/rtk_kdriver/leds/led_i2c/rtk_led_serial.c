#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/ioport.h>
#include <linux/kfifo.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/signal.h>
#include <linux/stat.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/version.h>
#include <rtd_log/rtd_module_log.h>
#include <rbus/iso_misc_off_uart_reg.h>
#include <rbus/rbus_types.h>
#include <rbus/misc_uart_reg.h>
#include <rtk_kdriver/io.h>
#include <linux/version.h>
#include <rtk_kdriver/rtk_crt.h>

#include <rtk_kdriver/rtk_gpio.h>
#include "rtk_led_serial.h"
#include <rtk_kdriver/rtk-kdrv-common.h>

int ledserial_dev_major;
int ledserial_dev_minor;

static int m_disable_ledserial_ioctl=0;
static int m_enable_ledserial_print=0;
static int m_mode=0;
static int m_enable_farfiled_led =true;

#define DMIC_DET_PIN_AE27_GPIO41  0xb8061134
#ifdef CONFIG_ARCH_RTK2851A
LED_UART_REG_BAKUP _gUartRegBakup;
#elif defined(CONFIG_ARCH_RTK2851C) || defined(CONFIG_ARCH_RTK2851F) || defined(CONFIG_ARCH_RTK2885P) ||defined(CONFIG_ARCH_RTK2875Q) || defined(CONFIG_ARCH_RTK2819A)

#else
volatile static unsigned int m_irtx_index = 0;
#endif

struct cdev *dev_cdevp_ledserial = NULL;
static struct device *ledserial_device_st;
static struct class *_gstledserial_class;
static struct platform_device *pst_rtk_ledserial_platform_dev;

static DEFINE_MUTEX(rtk_led_serial_mutex);
static DEFINE_SPINLOCK(rtk_ledserial_irq_lock);
void set_farfield_set_ledcolor_mute(bool muteflag);
RTK_LEDSERIAL_ATTR _gLedSerialtInfo;

#ifdef CONFIG_ARCH_RTK2851A
static void ledserial_part_outl(unsigned int reg_addr, unsigned int endBit, unsigned int startBit, unsigned int value)
{
	unsigned int X,A,result;

	X=(1<<(endBit-startBit+1))-1;
	A=rtd_inl(reg_addr);

	result = (A & (~(X<<startBit))) | (value<<startBit);
	rtd_outl(reg_addr,result);
}


//uart led setting
static void _uart_led_setting(void)
{
	unsigned int clkmux_regvalue;
	iso_misc_off_uart_u0lcr_RBUS         	uart_lcr;
	iso_misc_off_uart_u0ier_dlh_RBUS      	uart_ier_dlh;
	iso_misc_off_uart_u0rbr_thr_dll_RBUS  	uart_rbr_thr_dll;
	iso_misc_off_uart_u0iir_fcr_RBUS    	uart_iir_fcr;


	uart_lcr.regValue = rtd_inl(UART_LCR) ;
	uart_lcr.dlab = 0x01;
	rtd_outl( UART_LCR , uart_lcr.regValue );

	uart_ier_dlh.regValue = rtd_inl(UART_IER_DLH) ;
	uart_rbr_thr_dll.regValue = rtd_inl(UART_RBR_THR_DLL);
	uart_iir_fcr.regValue = rtd_inl(UART_IIR_FCR);
	clkmux_regvalue = rtd_inl(UART_CLKMUX);

	//bakup the uart register when first call
	if(!_gUartRegBakup.inited)
	{
		_gUartRegBakup.uart_lcr_value = uart_lcr.regValue;
		_gUartRegBakup.uart_ier_dlh_value = uart_ier_dlh.regValue;
		_gUartRegBakup.uart_rbr_thr_dll_value = uart_rbr_thr_dll.regValue;
		_gUartRegBakup.uart_clkmux_value = clkmux_regvalue;
		_gUartRegBakup.uart_iir_fcr_value = uart_iir_fcr.regValue;
		_gUartRegBakup.inited = 1;
	}
	//clk setting
	rtd_outl(UART_CLKMUX, clkmux_regvalue & (~BIT0));

	uart_lcr.dlab = 0x01;
	uart_lcr.wls = 0x01;
	rtd_outl( UART_LCR , uart_lcr.regValue );

	uart_rbr_thr_dll.dll = 0x02;
	uart_ier_dlh.dlh = 0x00;

	rtd_outl( UART_RBR_THR_DLL , uart_rbr_thr_dll.regValue );
	rtd_outl( UART_IER_DLH , uart_ier_dlh.regValue );

	uart_iir_fcr.iid = uart_iir_fcr.iid |  UART2_ENABLE_FIFO()  | UART2_TX_FIFO_RESET();
	rtd_outl( UART_IIR_FCR , uart_iir_fcr.regValue );

	uart_lcr.regValue = rtd_inl(UART_LCR) ;
	uart_lcr.dlab = 0x00;
	rtd_outl( UART_LCR , uart_lcr.regValue );

	mdelay(10);
	//LEDSERIAL_WARN("rtd_inl(UART_IER_DLH):0x%x \n",rtd_inl(UART_IER_DLH));
	//LEDSERIAL_WARN("rtd_inl(UART_RBR_THR_DLL):0x%x\n",rtd_inl(UART_RBR_THR_DLL));
	//LEDSERIAL_WARN("rtd_inl(UART_IIR_FCR):0x%x \n",rtd_inl(UART_IIR_FCR));
	//LEDSERIAL_WARN("rtd_inl(UART_CLKMUX):0x%x\n",rtd_inl(UART_CLKMUX));
	LEDSERIAL_WARN("rtd_inl(UART_LCR):0x%x \n",rtd_inl(UART_LCR));
}
#endif

int GetLedMode(void )
{
	//rtd_pr_led_emerg(KERN_EMERG,"ledserial","rtd_inl(0xb8060220 = 0x%x ,m_mode = %d\n",rtd_inl(0xb8060220),m_mode);
	return m_mode;
}

#ifdef CONFIG_ARCH_RTK2851A
unsigned char GetUartRegBakup_inited(void )
{
	return _gUartRegBakup.inited;
}

//restore before uart setting
static void  _uart_restore_before_setting( void )
{

	if(m_mode == 0)
	{
		iso_misc_off_uart_u0lcr_RBUS        uart_lcr;
		uart_lcr.regValue = _gUartRegBakup.uart_lcr_value;
		uart_lcr.dlab = 0x01;
		//LEDSERIAL_WARN("%s _gUartRegBakup.uart_lcr_value=0x%x, \n" , __func__,_gUartRegBakup.uart_lcr_value);
		rtd_outl( UART_CLKMUX ,_gUartRegBakup.uart_clkmux_value);
		LEDSERIAL_WARN("%s _gUartRegBakup.uart_clkmux_value=0x%x, \n" , __func__,_gUartRegBakup.uart_clkmux_value);
		rtd_outl( UART_LCR , uart_lcr.regValue);

		//LEDSERIAL_WARN("%s _gUartRegBakup.uart_ier_dlh_value=0x%x, \n" , __func__,_gUartRegBakup.uart_ier_dlh_value);
		//LEDSERIAL_WARN("%s _gUartRegBakup.uart_rbr_thr_dll_value=0x%x, \n" , __func__,_gUartRegBakup.uart_rbr_thr_dll_value);
		rtd_outl( UART_IER_DLH , _gUartRegBakup.uart_ier_dlh_value );
		rtd_outl( UART_RBR_THR_DLL , _gUartRegBakup.uart_rbr_thr_dll_value);

		uart_lcr.dlab = 0x01;
		rtd_outl( UART_LCR , uart_lcr.regValue);
		LEDSERIAL_WARN("%s _gUartRegBakup.uart_iir_fcr_value=0x%x, \n" , __func__,_gUartRegBakup.uart_iir_fcr_value);
		rtd_outl( UART_IIR_FCR , _gUartRegBakup.uart_iir_fcr_value);
		_gUartRegBakup.inited = 0;
		uart_lcr.dlab = 0x00;
		rtd_outl( UART_LCR , uart_lcr.regValue);
	}else
	{
	}
}

//uart serial send 2bit
static int _uart_led_send_2bits(int dat)
{
	iso_misc_off_uart_u0rbr_thr_dll_RBUS uart_rbr_thr;

	uart_rbr_thr.regValue = rtd_inl(UART_RBR_THR_DLL);

	switch(dat&3)
	{
		case 0:
			uart_rbr_thr.dll = 0x37;//0x3B;//11 0111
			break;
		case 1:
			uart_rbr_thr.dll = 0x07;//000111;//10 0111
			break;
		case 2:
			uart_rbr_thr.dll = 0x34;// 110100 0x36;//11 0110
			break;
		case 3:
			uart_rbr_thr.dll = 0x04;// 00 0100 //0x26;//10 0110
			break;
		default:
			break;
	}
	rtd_outl( UART_RBR_THR_DLL , uart_rbr_thr.regValue );
	return 0 ;
}

//uart send 24bit
static int _uart_led_send_24bits( unsigned int data )
{
	int i = 0 ;

	for( i = 22 ; i >= 0 ; i=i-2 )
	{
		_uart_led_send_2bits(((data>>i) & 3));
	}
	return 0 ;
}
#elif defined(CONFIG_ARCH_RTK2851C) || defined(CONFIG_ARCH_RTK2851F) || defined(CONFIG_ARCH_RTK2885P) ||defined(CONFIG_ARCH_RTK2875Q) || defined(CONFIG_ARCH_RTK2819A)
static void _irtx_led_setting(void)
{
	rtd_outl(MISC_IRTX_IRTX_EN_RST_reg, 0x00000111);    //reset ip && enable ip func;
	rtd_outl(MISC_IRTX_IRTX_CLKDIV_reg, 0x00000004);    //one unit is 20ns
	rtd_outl(MISC_IRTX_IRTX_INT_TH_reg, 0x00000070);    //set buf len is 96
	rtd_outl(MISC_IRTX_PWM_CTRL_reg,   0x00000000);     //disable pwm carrier;
}
static int _irtx_led_send_bits(unsigned int dat)
{
	//while()
	switch(dat&1)
	{
	case 0:
		rtd_outl(MISC_IRTX_TI_DATA_reg,(15<<1) |1);
		rtd_outl(MISC_IRTX_TI_CTRL_reg,0x1);

		rtd_outl(MISC_IRTX_TI_DATA_reg,(45<<1) | 0);
		rtd_outl(MISC_IRTX_TI_CTRL_reg,0x1);
		break;
	case 1:
		rtd_outl(MISC_IRTX_TI_DATA_reg,(45<<1) | 1);
		rtd_outl(MISC_IRTX_TI_CTRL_reg,0x1);

		rtd_outl(MISC_IRTX_TI_DATA_reg,(15<<1) | 0);
		rtd_outl(MISC_IRTX_TI_CTRL_reg,0x1);

		break;
	default:
		break;
	}
	return 0;
}

//uart send 24bit
static int _irtx_led_send_24bits( unsigned int data )
{
	int i = 0;

	for(i = 23; i >= 0; i--)
	{
		_irtx_led_send_bits(((data>>i) & 0x1));
	}
	return 0 ;
}

#else

static void _irtx_led_setting(void)
{
	rtd_outl(MISC_IRTX_IRTX_EN_RST_reg, 0x00000111);    //reset ip && enable ip func;
	rtd_outl(MISC_IRTX_IRTX_CLKDIV_reg, 0x00000004);    //one unit is 20ns
	rtd_outl(MISC_IRTX_PWM_CTRL_reg,   0x00000000);     //disable pwm carrier;
}

static int _irtx_led_send_bits(unsigned int dat)
{
	switch(dat&1)
	{
	case 0:
		rtd_outl(MISC_IRTX_TI_CTRL_reg, LED_IRTX_TI_BUF_IDX(m_irtx_index++) | LED_IRTX_TI_BUF_WDATA_LEVEL(1) | LED_IRTX_TI_BUF_WDATA_LENGTH(15) | LED_IRTX_TI_BUF_WR(1));
		while(rtd_inl(MISC_IRTX_TI_CTRL_reg) & LED_IRTX_TI_BUF_WR_MASK);
		rtd_outl(MISC_IRTX_TI_CTRL_reg, LED_IRTX_TI_BUF_IDX(m_irtx_index++) | LED_IRTX_TI_BUF_WDATA_LEVEL(0) | LED_IRTX_TI_BUF_WDATA_LENGTH(45) | LED_IRTX_TI_BUF_WR(1));
		while(rtd_inl(MISC_IRTX_TI_CTRL_reg) & LED_IRTX_TI_BUF_WR_MASK);

		break;
	case 1:
		rtd_outl(MISC_IRTX_TI_CTRL_reg, LED_IRTX_TI_BUF_IDX(m_irtx_index++) | LED_IRTX_TI_BUF_WDATA_LEVEL(1) | LED_IRTX_TI_BUF_WDATA_LENGTH(45) | LED_IRTX_TI_BUF_WR(1));
		while(rtd_inl(MISC_IRTX_TI_CTRL_reg) & LED_IRTX_TI_BUF_WR_MASK);
		rtd_outl(MISC_IRTX_TI_CTRL_reg, LED_IRTX_TI_BUF_IDX(m_irtx_index++) | LED_IRTX_TI_BUF_WDATA_LEVEL(0) | LED_IRTX_TI_BUF_WDATA_LENGTH(15) | LED_IRTX_TI_BUF_WR(1));
		while(rtd_inl(MISC_IRTX_TI_CTRL_reg) & LED_IRTX_TI_BUF_WR_MASK);

		break;
	default:
		break;
	}
	return 0;
}

//uart send 24bit
static int _irtx_led_send_24bits( unsigned int data )
{
	int i = 0;

	for(i = 23; i >= 0; i--)
	{
		_irtx_led_send_bits(((data>>i) & 0x1));
	}
	return 0 ;
}
#endif

// Tag: LIGHTBAR_OPTION
// setting:
// 0: keep lightbar_setting;
// NonHotKey Mode >> 1: ONLY ENABLE; 2: ONLY BLINK; 3: ALL ENABLE;
// HotKey Mode >> 17: ONLY ENABLE; 18: ONLY BLINK; 19: ALL ENABLE;
// control:
// 0: LED blue; 1: LED orange; 2: LED black;
// 3: enter Factory CMD; 4: exit Factory CMD; 5: Disable 51 core;
// 6: enter HotKey Mode; 7: exit HotKey Mode;
// 8: PowerOn LED blue.
// Factory CMD mode: Do not accept other control except exit(4).
// HotKey mode: Always keep the LED on and white.
#define LIGHTBAR_BLUE_COLOR (0x002a003f)
#define LIGHTBAR_ORANGE_COLOR (0x00030600)
#define LIGHTBAR_BLACK_COLOR (0x00)
#define LIGHTBAR_WHITE_COLOR (0x00808080)

static int lightbar_setting = 0;
static bool lightbar_facMode = false;
static bool lightbar_51Status = true;
static bool lightbar_as_uart0_tx = false;
static bool lightbar_hotKey = false;
static bool lightbar_wait_reset = false; //need to wait for AP to reset setting after exiting hotKey.

void rtk_lightbar_show(int data)
{
	int loop_cnt = 4;
	int bit_cnt = 24;
	unsigned long lightbar_show_flag;
	spin_lock_irqsave(&rtk_ledserial_irq_lock, lightbar_show_flag);
	for (;loop_cnt > 0;) {
		loop_cnt --;
		for (bit_cnt = 24; bit_cnt > 0;) {
			bit_cnt --;
			if ((data >> bit_cnt) & 1) {
				rtd_outl(0xB8062300, 0x30);
			} else {
				rtd_outl(0xB8062300, 0x3e);
			}
			udelay(1);
		}
	}
	spin_unlock_irqrestore(&rtk_ledserial_irq_lock, lightbar_show_flag);
	mdelay(1);
}

static void rtk_lightbar_set_as_uart0_tx(void)
{
	//uart0 tx setting
	rtd_outl(0xB806230C, ((rtd_inl(0xB806230C) & 0xFFFFFF7F) | 0x80));
	rtd_outl(0xB8060058, (rtd_inl(0xB8060058) & 0xFFFFFFFE));
	rtd_outl(0xB806230C, ((rtd_inl(0xB806230C) & 0xFFFFFF7C) | 0x81));
	rtd_outl(0xB8062300, ((rtd_inl(0xB8062300) & 0xFFFFFF00) | 0x01));
	rtd_outl(0xB8062304, (rtd_inl(0xB8062304) & 0xFFFFFF00));
	rtd_outl(0xB8062308, ((rtd_inl(0xB8062308) & 0xFFFFFFF0) | 0x05));
	rtd_outl(0xB806230C, (rtd_inl(0xB806230C) & 0xFFFFFF7F));
	udelay(1);
	//change AG15 to uart0 tx
	rtd_outl(0xB8060230, (rtd_inl(0xB8060230) & 0xFFFF0FFF));
	mdelay(2);
	lightbar_as_uart0_tx = true;
}

static void rtk_lightbar_set_as_gpo(void)
{
	mdelay(2);
	//change AG15 to GPO
	rtd_outl(0xB8061104, 0x00800001);
	rtd_outl(0xB8061124, 0x00800001);
	udelay(1);
	rtd_outl(0xB8060230, ((rtd_inl(0xB8060230) & 0xFFFF0FFF) | 0x0000F000));
	lightbar_as_uart0_tx = false;
}

static void rtk_lightbar_51core_disable(void)
{
	//Disable 51 core once to turn off the breathing effect.
	rtd_outl(0xB8060034, 0x00000200);
	rtd_outl(0xB8060044, 0x00000200);
}

static void rtk_lightbar_control(int control)
{
	switch(control) {
		case 0:
		case 4:
		case 5:
			if(lightbar_setting > 1) {
				rtk_lightbar_show(LIGHTBAR_BLUE_COLOR);
			}
			break;
		case 8:
			if(lightbar_setting > 1) {
				rtk_lightbar_set_as_uart0_tx();
				rtk_lightbar_show(LIGHTBAR_BLUE_COLOR);
			}
			break;
		case 1:
			if (lightbar_setting == 3) {
				rtk_lightbar_show(LIGHTBAR_ORANGE_COLOR);
			} else {
				rtk_lightbar_show(LIGHTBAR_BLACK_COLOR);
			}
			rtk_lightbar_set_as_gpo();
			break;
		case 3:
			rtk_lightbar_show(LIGHTBAR_BLACK_COLOR);
			rtk_lightbar_set_as_gpo();
			break;
		case 2:
		default:
			rtk_lightbar_show(LIGHTBAR_BLACK_COLOR);
			break;
	}
}

static int rtk_lightbar_option_apply(LIGHTBAR_OPTION option)
{
	if (lightbar_facMode == false && lightbar_as_uart0_tx == false && lightbar_hotKey == false) {
		rtk_lightbar_set_as_uart0_tx();
	}
	if (option.control == 3) {
		// enter factory cmd mode
		if (lightbar_facMode == false) {
			lightbar_facMode = true;
			if (lightbar_51Status == true) {
				lightbar_51Status = false;
				rtk_lightbar_51core_disable();
			}
			if (lightbar_hotKey != true) {
				rtk_lightbar_control(option.control);
                        } else {
                                rtk_lightbar_show(LIGHTBAR_WHITE_COLOR);
                                rtk_lightbar_set_as_gpo();
                        }
		}
		return 1;
	} else if (option.control == 4) {
		// exit factory cmd mode
		lightbar_facMode = false;
		if (lightbar_hotKey != true) {
			rtk_lightbar_set_as_uart0_tx();
		}
	} else if (option.control == 6) {
		// exit hotkey mode
		if (lightbar_hotKey == false) {
			lightbar_hotKey = true;
			if (lightbar_facMode == true) {
				rtk_lightbar_set_as_uart0_tx();
			}
			rtk_lightbar_show(LIGHTBAR_WHITE_COLOR);
			rtk_lightbar_set_as_gpo();
		}
	} else if (option.control == 7) {
		// exit hotkey mode
		lightbar_hotKey = false;
		lightbar_wait_reset = true;
	} else if(option.control == 5 || option.control == 1 || option.control == 8) {
		// Start-up is completed or enter standby in a short time.
		if (lightbar_51Status == true) {
			lightbar_51Status = false;
			rtk_lightbar_51core_disable();
		}
		if (lightbar_hotKey == true && lightbar_facMode == false) {
			if (lightbar_as_uart0_tx == false) {
				rtk_lightbar_set_as_uart0_tx();
			}
			if (option.control == 1) {
				if (lightbar_setting == 19) {
					rtk_lightbar_show(LIGHTBAR_ORANGE_COLOR);
				} else {
					rtk_lightbar_show(LIGHTBAR_BLACK_COLOR);
				}
			} else {
				rtk_lightbar_show(LIGHTBAR_WHITE_COLOR);
			}
			rtk_lightbar_set_as_gpo();
			return 1;
		}
	}

	if (option.setting != 0) {
		// change by UI setting.
		if (lightbar_hotKey == false) {
			lightbar_setting = option.setting;
		}
		if (lightbar_wait_reset == true && lightbar_setting < 4) {
			lightbar_wait_reset = false;
			rtk_lightbar_set_as_uart0_tx();
			if (lightbar_setting == 1) {
				rtk_lightbar_show(LIGHTBAR_BLACK_COLOR);
			} else {
				rtk_lightbar_show(LIGHTBAR_BLUE_COLOR);
			}
			if (lightbar_facMode == true) {
				rtk_lightbar_set_as_gpo();
			}
		}
		return 1;
	} else if (lightbar_setting == 1 || lightbar_setting > 16) {
		// The function is enabled, but it is disabled in UI settings.
		if(option.control == 1) {
			rtk_lightbar_set_as_gpo();
		}
		return 0;
	}

	if(lightbar_facMode == true || lightbar_51Status == true || lightbar_hotKey == true || lightbar_wait_reset == true) {
		//In these cases, control is not continued.
		if(option.control == 1) {
			rtk_lightbar_set_as_gpo();
		}
		return 0;
	}

	rtk_lightbar_control(option.control);
	return 1;
}

//serial output 24bit for one led
static int rtk_led_serial_send_24bits( unsigned int data )
{
#ifdef CONFIG_ARCH_RTK2851A
	_uart_led_send_24bits(data); //uart serial
#else
	_irtx_led_send_24bits(data);
#endif
	return 0 ;
}

//serial output data
static int rtk_ledserial_send_bytes( int count , unsigned int *data )
{
	int i = 0;
	unsigned long ledserial_flag;
	if(m_mode)		//UART mode send data
	{
		if(data == NULL || count == 0 || count > LED_COUNT_MAX){
			LEDSERIAL_ERROR("send IO LED data NULL\n");
			return -1;
		}
		if(m_enable_farfiled_led == false)
		{
			LEDSERIAL_ERROR("apk set cannot control assistant led!\n");
			return -1;
		}
		//for (j = 0; j < 2; j++)
		{
#ifdef CONFIG_ARCH_RTK2851A
#elif defined(CONFIG_ARCH_RTK2851C) || defined(CONFIG_ARCH_RTK2851F) || defined(CONFIG_ARCH_RTK2885P) ||defined(CONFIG_ARCH_RTK2875Q) || defined(CONFIG_ARCH_RTK2819A)
#else
			rtd_outl(MISC_IRTX_IRTX_INT_reg,0x00000001);
			m_irtx_index = 0;
#endif
			spin_lock_irqsave(&rtk_ledserial_irq_lock, ledserial_flag);
			for( i = 0 ; i < count ; ++i ){
				rtk_led_serial_send_24bits(data[i]);
#if defined(CONFIG_ARCH_RTK2851C) || defined(CONFIG_ARCH_RTK2851F) || defined(CONFIG_ARCH_RTK2885P) ||defined(CONFIG_ARCH_RTK2875Q) || defined(CONFIG_ARCH_RTK2819A)
				rtd_outl(MISC_IRTX_IRTX_START_reg,0x00000001);  //start send bit
#endif
			}
			spin_unlock_irqrestore(&rtk_ledserial_irq_lock, ledserial_flag);
#ifdef CONFIG_ARCH_RTK2851A
#elif defined(CONFIG_ARCH_RTK2851C) || defined(CONFIG_ARCH_RTK2851F) || defined(CONFIG_ARCH_RTK2885P) ||defined(CONFIG_ARCH_RTK2875Q) || defined(CONFIG_ARCH_RTK2819A)
			rtd_outl(MISC_IRTX_IRTX_START_reg,0x00000001);  //start send bit
#else
			rtd_outl(MISC_IRTX_IRTX_START_reg, LED_IRTX_LEN(m_irtx_index-1) |  LED_IRTX_START(1));
			while(rtd_inl(MISC_IRTX_IRTX_INT_reg) == 0x0);		//wait irtx sending end;
#endif
			mdelay(2);
		}
		if(m_enable_ledserial_print){
			LEDSERIAL_WARN("%s data[0]=0x%x, data[1] = 0x%x , data[2] = 0x%x , data[3] = 0x%x \n" , __func__,data[0], data[1],data[2],data[3]);
		}
	}else			//GPIO not send data
	{
		LEDSERIAL_ERROR("GPIO mode can't send IO LED data\n");
		return -1;
	}
	return 0 ;
}

//serial led setting
static void  rtk_ledserial_set_logicalmode( void )
{
#ifdef CONFIG_ARCH_RTK2851A
	_uart_led_setting(); //uart serial
	ledserial_part_outl(LEDSERIAL_PINMUX, 7, 4, 0x4);
#elif defined(CONFIG_ARCH_RTK2851C) || defined(CONFIG_ARCH_RTK2851F)
	_irtx_led_setting(); //irtx serial
	//ledserial_part_outl(LEDSERIAL_PINMUX, 31, 28, 0x2);
#elif defined(CONFIG_ARCH_RTK2885P) 
	_irtx_led_setting(); //irtx serial
	//ledserial_part_outl(LEDSERIAL_PINMUX, 31, 28, 0x8);
#elif defined(CONFIG_ARCH_RTK2875Q)
	_irtx_led_setting(); //irtx serial
	//ledserial_part_outl(LEDSERIAL_PINMUX, 15, 12, 0x8);
#elif defined(CONFIG_ARCH_RTK6748)
	_irtx_led_setting(); //irtx serial
	//ledserial_part_outl(LEDSERIAL_PINMUX, 31, 28, 0x8);
#else
	_irtx_led_setting(); //irtx serial
#endif
	LEDSERIAL_WARN("%s  \n" , __func__);
	m_mode = 1;
}

//serial restore before setting
void  rtk_ledserial_restore_setting( void )
{
	LEDSERIAL_WARN("%s  \n" , __func__);
	mdelay(1);
	m_mode = 0;
#ifdef CONFIG_ARCH_RTK2851A
	if(_gUartRegBakup.inited == 1){
		ledserial_part_outl(LEDSERIAL_PINMUX, 7, 4, 0xF);
		_uart_restore_before_setting(); //uart serial
	}
#elif defined(CONFIG_ARCH_RTK2851C) || defined(CONFIG_ARCH_RTK2851F)
	_irtx_led_setting(); //irtx serial
	//ledserial_part_outl(LEDSERIAL_PINMUX, 31, 28, 0x2);
#elif defined(CONFIG_ARCH_RTK2885P)
	_irtx_led_setting(); //irtx serial
	//ledserial_part_outl(LEDSERIAL_PINMUX, 31, 28, 0x8);
#elif defined(CONFIG_ARCH_RTK2875Q)
	_irtx_led_setting(); //irtx serial
	//ledserial_part_outl(LEDSERIAL_PINMUX, 15, 12, 0x8);	
#elif defined(CONFIG_ARCH_RTK6748)
	//ledserial_part_outl(LEDSERIAL_PINMUX, 31, 28, 0x8);
	_irtx_led_setting();
#else
	_irtx_led_setting();
#endif
}

#ifdef CONFIG_PM
static int rtk_ledserial_suspend(struct device *dev)
{
	LEDSERIAL_WARN("rtk_ledserial_suspend\n");
	rtk_ledserial_restore_setting();
	return 0;
}

static int rtk_ledserial_resume(struct device *dev)
{
#ifndef CONFIG_ARCH_RTK2851A
	CRT_CLK_OnOff(IRTX,CLK_ON,NULL);
#endif
	LEDSERIAL_WARN("rtk_ledserial_resume\n");

	rtk_ledserial_set_logicalmode();

    if((rtd_inl(DMIC_DET_PIN_AE27_GPIO41) & 0x800) !=0)
    {
        set_farfield_set_ledcolor_mute(0);
    }
    else
    {
        set_farfield_set_ledcolor_mute(1);
    }
	return 0;
}

#ifdef CONFIG_HIBERNATION
static int rtk_ledserial_suspend_std(struct device *dev)
{
	LEDSERIAL_WARN("%s \n" , __func__);

	return 0;
}
static int rtk_ledserial_resume_std(struct device *dev)
{
#ifndef CONFIG_ARCH_RTK2851A
	CRT_CLK_OnOff(IRTX,CLK_ON,NULL);
#endif
	LEDSERIAL_WARN("%s \n" , __func__);

	return 0;
}
#endif

static const struct dev_pm_ops rtk_ledserial_pm_ops = {

	.suspend 	= rtk_ledserial_suspend,
	.resume 	= rtk_ledserial_resume,
#ifdef CONFIG_HIBERNATION
	.freeze 	= rtk_ledserial_suspend_std,
	.thaw  		= rtk_ledserial_resume_std,
#endif
};
#endif

static int rtk_ledserial_probe(struct platform_device *pdev)
{
	LEDSERIAL_WARN("%s():\n", __FUNCTION__);
	return 0;
}

static int rtk_ledserial_remove(struct platform_device *pdev)
{
	LEDSERIAL_WARN("%s():\n", __FUNCTION__);

	return 0;
}

static ssize_t rtk_ledserial_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	LEDSERIAL_WARN("%s():\n", __FUNCTION__);
	return -EFAULT;
}

static ssize_t rtk_ledserial_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	LEDSERIAL_WARN("%s():\n", __FUNCTION__);
	return -EFAULT;
}

static int rtk_ledserial_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int rtk_ledserial_release(struct inode *inode, struct file *filp)
{
	return 0;
}

void set_farfield_set_ledcolor_mute(bool muteflag)
{
	LED_COLOR_INPUT color_input;
	//int i = 0,j=0;
	//unsigned long ledserial_flag;
	if(m_enable_ledserial_print)
	{LEDSERIAL_WARN("m_enable_farfiled_led == %d,muteflag =%d\n",m_enable_farfiled_led,muteflag);}
   // if(first_flag&&muteflag) { return; }
	if(m_enable_farfiled_led == true)
	{
		color_input.count = 4;
		if(muteflag)
		{
		/*set led orange*/			
		color_input.colors[0]=0xCC7C1D;
		color_input.colors[1]=0xCC7C1D;
		color_input.colors[2]=0xCC7C1D;
		color_input.colors[3]=0xCC7C1D;
		}
		else
		{
		/*set led off*/			
		color_input.colors[0]=0x000000;
		color_input.colors[1]=0x000000;
		color_input.colors[2]=0x000000;
		color_input.colors[3]=0x000000;	
		} 
		LEDSERIAL_WARN("0x%x,0x%x,0x%x,0x%x\n",color_input.colors[0],color_input.colors[1],color_input.colors[2],color_input.colors[3]);
		//spin_lock_irqsave(&rtk_ledserial_irq_lock, ledserial_flag);	
		rtk_ledserial_send_bytes(color_input.count,color_input.colors);
		#if 0
		for(j = 0; j < 2; j ++)
		{
			for (i=0;i < color_input.count; i ++)
			{
				rtk_led_serial_send_24bits(color_input.colors[i]);
			}
		//	if(muteflag)
		//		mdelay(1);
			
		}		
		#endif
		 udelay(2000);  
		//spin_unlock_irqrestore(&rtk_ledserial_irq_lock, ledserial_flag);
	}
}

static void set_farfield_led_on_off(bool b_on_off) /* true: dmic mute ,color orange, false: dmic on, led off */
{
	if(b_on_off) /* dmic mute. first color orange,then change to usb*/
	{
		//dmic_interrupt_dmic();
		set_farfield_set_ledcolor_mute(b_on_off);
		//dmic_interrupt_usb();
		
	}
	else /* dmic on. first change to uart, then led color off*/
	{
		//dmic_interrupt_dmic();
		set_farfield_set_ledcolor_mute(b_on_off);
	}
	return;
}

static void set_farfield_led_enable_control(bool b_enable_control)
{
	LED_COLOR_INPUT color_input;
	//int i = 0,j=0;
	//unsigned long ledserial_flag;
	m_enable_farfiled_led = b_enable_control;
	if(m_enable_ledserial_print)
	{
		LEDSERIAL_WARN("b_enable_control == %d \n",b_enable_control );
	}
	if(m_enable_farfiled_led ==false)
	{
		/*turn off led*/
		color_input.count = 4;
		color_input.colors[0]=0x000000;
		color_input.colors[1]=0x000000;
		color_input.colors[2]=0x000000;
		color_input.colors[3]=0x000000;
		
		//spin_lock_irqsave(&rtk_ledserial_irq_lock, ledserial_flag);	
		rtk_ledserial_send_bytes(color_input.count,color_input.colors);
		#if 0
		for(j = 0; j < 2; j ++)
		{
			for (i=0;i < color_input.count; i ++)
			{
				rtk_led_serial_send_24bits(color_input.colors[i]);
			}
			
		}
		#endif
		udelay(2000);
		//spin_unlock_irqrestore(&rtk_ledserial_irq_lock, ledserial_flag);
	}
}

static long rtk_ledserial_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int retval = 0;
	LED_COLOR_INPUT color_input;
	LIGHTBAR_OPTION lightbar_option;
	bool led_enable_led_control = true;
	bool dmic_led_mute_flag = false;

	if(m_disable_ledserial_ioctl)
		return retval;
	mutex_lock(&rtk_led_serial_mutex);
	switch (cmd) {

		case RTK_LED_IOC_SET_COLOR:
			if (copy_from_user((void *)&color_input, (const void __user *)arg, sizeof(LED_COLOR_INPUT)))
			{
				retval = -EFAULT;
				break;
			}
			retval = rtk_ledserial_send_bytes(color_input.count, color_input.colors);
			break;
		case RTK_LED_IOC_INIT:
			rtk_ledserial_set_logicalmode();
			break;

		case RTK_LED_IOC_DEINIT:
			rtk_ledserial_restore_setting();
			break;

		case RTK_LED_IOC_ENABLE_LED:
			if (copy_from_user((void *)&led_enable_led_control, (const void __user *)arg, sizeof(bool)))
			{
				retval = -EFAULT;
				break;
			}
			set_farfield_led_enable_control(led_enable_led_control);
			break;

		case RTK_LED_IOC_SET_DMIC_ON_OFF:
			if (copy_from_user((void *)&dmic_led_mute_flag, (const void __user *)arg, sizeof(bool))) 
			{
				retval = -EFAULT;
				break;
			}
			set_farfield_led_on_off(dmic_led_mute_flag);
			break;
		case RTK_LIGHTBAR_IOC_SET_OPTION:
			if (lightbar_setting == 0)
				break;
			if (copy_from_user((void *)&lightbar_option, (const void __user *)arg, sizeof(LIGHTBAR_OPTION)))
			{
				retval = -EFAULT;
				break;
			}
			retval = rtk_lightbar_option_apply(lightbar_option);
			break;

		default:
			LEDSERIAL_WARN("Unknow commnad \n");
			retval = -ENOTTY;
			break;
	}

	mutex_unlock(&rtk_led_serial_mutex);
	return retval;
}

#ifdef CONFIG_COMPAT
long rtk_ledserial_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	return rtk_ledserial_ioctl(filp, cmd, (unsigned long)arg);
}
#endif

static struct file_operations rtk_ledserial_fops = {
	.owner 			= THIS_MODULE,
	.read 			= rtk_ledserial_read,
	.write 			= rtk_ledserial_write,
	.unlocked_ioctl 	= rtk_ledserial_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl		= rtk_ledserial_compat_ioctl,
#endif
	.release 		= rtk_ledserial_release,
	.open 			= rtk_ledserial_open,

};


static const struct of_device_id rtk_ledserial_devices[] = {
	{.compatible = "rtk-ledserial",},
	{},
};

static struct platform_driver rtk_ledserial_driver = {
	.driver = {
		.name = "rtk-ledserial",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(rtk_ledserial_devices),
#ifdef CONFIG_PM
		.pm	= &rtk_ledserial_pm_ops,
#endif
	},
	.probe = rtk_ledserial_probe,
	.remove = rtk_ledserial_remove,
};


ssize_t rtk_ledserial_show_param(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret = 0;
	RTK_LEDSERIAL_ATTR *ledserial = NULL;

	if (dev->platform_data != NULL) {
		ledserial = (RTK_LEDSERIAL_ATTR *) dev->platform_data;
		if (ledserial->mapped == false) {
			LEDSERIAL_WARN("This ledserial isn't initialize.\n");
			return 0;
		}
	}
	if( ledserial == NULL){
		LEDSERIAL_WARN("ledserial is null.\n");
		return 0;
	}
	if (strncmp(attr->attr.name, "ledserial", strlen("ledserial")  ) == 0) {

		LEDSERIAL_WARN("not support read ledserial .\n");
	} else if (strncmp(attr->attr.name, "disable_ioctl", strlen("disable_ioctl")) == 0) {

		ret = sprintf(buf, "%d\n", m_disable_ledserial_ioctl);
	}else if (strncmp(attr->attr.name, "mode", strlen("mode")) == 0) {

		ret = sprintf(buf, "%s\n", ((m_mode)?"UART":"GPIO"));
	}else if (strncmp(attr->attr.name, "enable_print", strlen("enable_print")) == 0) {
		ret = sprintf(buf, "%d\n", m_enable_ledserial_print);
	}
	return ret;
}

ssize_t rtk_ledserial_set_param(struct device *dev,
			     struct device_attribute *attr,
			     const char *buf, size_t count)
{
	int i=0;
	RTK_LEDSERIAL_ATTR *ledserial = NULL;
	unsigned int led_data[LED_COLOR_COUNT] = {0};

	if (dev->platform_data != NULL) {
		ledserial = (RTK_LEDSERIAL_ATTR *) dev->platform_data;
		if (ledserial->mapped == false) {
			LEDSERIAL_WARN("This LEDSERIAL isn't initialize.\n");
			return 0;
		}
	}

	if( ledserial == NULL){
		LEDSERIAL_WARN("ledserial is null.\n");
		return 0;
	}

	if (strncmp(attr->attr.name, "ledserial", strlen("ledserial")) == 0) {

		sscanf(buf,"0x%x 0x%x 0x%x 0x%x ",&led_data[0],&led_data[1],
						&led_data[2],&led_data[3]);
		rtk_ledserial_send_bytes(LED_COLOR_COUNT, led_data);
		if(m_enable_ledserial_print){
			LEDSERIAL_WARN("ledserial led_data[%d]:%d\n",i,led_data[i]);
		}
	}else if (strncmp(attr->attr.name, "disable_ioctl", strlen("disable_ioctl")) == 0) {
		unsigned int  disable_ioctl;
		sscanf(buf,"%d",&disable_ioctl);
		if (disable_ioctl== 0 ){
			m_disable_ledserial_ioctl = disable_ioctl;
			if(m_enable_ledserial_print)
				LEDSERIAL_WARN("all ledserial ioctl disactive \n");
		}else if( disable_ioctl== 1){
			m_disable_ledserial_ioctl = disable_ioctl;
			if(m_enable_ledserial_print)
				LEDSERIAL_WARN("all ledserial ioctl active \n");
		}else{
			if(m_enable_ledserial_print)
				LEDSERIAL_WARN("access disable_ioctl Fail!\n");
		}
	}else if (strncmp(attr->attr.name, "mode", strlen("mode")) == 0) {
		unsigned int  mode;
		sscanf(buf,"%d",&mode);
		if (mode == 0 ){
			m_mode = mode;
			rtk_ledserial_restore_setting();
			if(m_enable_ledserial_print)
				LEDSERIAL_WARN("mode is gpio \n");
		}else if(mode == 1){
			m_mode = mode;
			rtk_ledserial_set_logicalmode();
			if(m_enable_ledserial_print)
				LEDSERIAL_WARN("Mode is Uart \n");
		}else{
			if(m_enable_ledserial_print)
				LEDSERIAL_WARN("access mode Fail!\n");
		}
	}else if (strncmp(attr->attr.name, "enable_print", strlen("enable_print")) == 0) {
		unsigned int  enable_print;
		sscanf(buf,"%d",&enable_print);
		if (enable_print == 0 ){
			m_enable_ledserial_print = enable_print;

		}else if( enable_print == 1){
			m_enable_ledserial_print = enable_print;

		}else{
			if(m_enable_ledserial_print)
				LEDSERIAL_WARN("access disable_print Fail!\n");
		}
	}
	return count;
}


DEVICE_ATTR( ledserial     , S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,rtk_ledserial_show_param, rtk_ledserial_set_param);
DEVICE_ATTR( disable_ioctl, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,rtk_ledserial_show_param, rtk_ledserial_set_param);
DEVICE_ATTR( mode, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,rtk_ledserial_show_param, rtk_ledserial_set_param);
DEVICE_ATTR( enable_print, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,rtk_ledserial_show_param, rtk_ledserial_set_param);

static int led_serial_device_attr_init(void)
{
	int ret = 0;

	snprintf( _gLedSerialtInfo.nodename, 16, "ledserial"  );

	_gLedSerialtInfo.mapped = true;

	_gLedSerialtInfo.ledserial_device_st = device_create(_gstledserial_class, NULL, MKDEV(0, 0), NULL, _gLedSerialtInfo.nodename);

	ret = device_create_file(_gLedSerialtInfo.ledserial_device_st, &dev_attr_ledserial);
	ret = device_create_file(_gLedSerialtInfo.ledserial_device_st, &dev_attr_disable_ioctl);
	ret = device_create_file(_gLedSerialtInfo.ledserial_device_st, &dev_attr_mode);
	ret = device_create_file(_gLedSerialtInfo.ledserial_device_st, &dev_attr_enable_print);

	_gLedSerialtInfo.ledserial_device_st->platform_data =(void *)&_gLedSerialtInfo;

	LEDSERIAL_WARN("[%s]  ,Create Node  %s  device id = %d\n",__func__ ,  _gLedSerialtInfo.nodename,_gLedSerialtInfo.ledserial_device_st->id  );

	return 0;
}


static int register_ledserial_cdev(void) {
	dev_t dev;
	int ret;

	ret = alloc_chrdev_region(&dev, 0, 1, "ledserial");
	if (ret) {
		LEDSERIAL_ERROR("can't alloc chrdev\n");
		return ret;
	}

	ledserial_dev_major = MAJOR(dev);
	ledserial_dev_minor = MINOR(dev);

	dev_cdevp_ledserial = kzalloc(sizeof(struct cdev), GFP_KERNEL);
	if (dev_cdevp_ledserial == NULL) {
		LEDSERIAL_ERROR("kzalloc failed\n");
		goto KZALLOC_FAILED;
	}

	cdev_init(dev_cdevp_ledserial, &rtk_ledserial_fops);
	dev_cdevp_ledserial->owner = THIS_MODULE;
	dev_cdevp_ledserial->ops = &rtk_ledserial_fops;
	ret = cdev_add(dev_cdevp_ledserial, MKDEV(ledserial_dev_major, ledserial_dev_minor), 1);
	if (ret < 0) {
		LEDSERIAL_WARN(" add chr dev failed\n");
		goto CDEV_ADD_FAILED;
	}
	ledserial_device_st = device_create(_gstledserial_class, NULL, MKDEV(ledserial_dev_major, ledserial_dev_minor), NULL, "rtk_ledserial");
	LEDSERIAL_WARN("register chrdev(%d,%d) success.\n", ledserial_dev_major, ledserial_dev_minor);

	return 0;
CDEV_ADD_FAILED:
	cdev_del(dev_cdevp_ledserial);
KZALLOC_FAILED:
	if(dev_cdevp_ledserial) {
		kfree(dev_cdevp_ledserial);
		dev_cdevp_ledserial = NULL;
	}
	unregister_chrdev_region(MKDEV(ledserial_dev_major, ledserial_dev_minor),1);
	ledserial_dev_major = 0;
	ledserial_dev_minor = 0;
	return -1;

}

#ifdef CONFIG_RTK_KDRV_SERIAL_LED
static int __init lightbar_setting_init(char *options)
{
	if (sscanf(options, "%d", &lightbar_setting) == 1) {
		if (lightbar_setting > 1) {
			if (lightbar_setting > 16) {
				lightbar_hotKey = true;
			} else {
				lightbar_hotKey = false;
			}
			if(lightbar_setting != 17) {
				lightbar_51Status = true;
				lightbar_as_uart0_tx = true;
			}
		}
		LEDSERIAL_ERROR("lightbar_setting = %d\n", lightbar_setting);
		return 1;
	}
	lightbar_setting = 0;
	LEDSERIAL_ERROR("lightbar_setting is default\n");
	return 1;
}
early_param("lightbar_setting", lightbar_setting_init);
#endif

#ifdef CONFIG_RTK_KDRV_SERIAL_LED_MODULE
static int __init lightbar_setting_module_init(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("lightbar_setting", str, sizeof(strings)) == 0)
	{
		LEDSERIAL_ERROR("Error : can't get lightbar_setting from bootargs\n");
		return 1;
	}
	if (sscanf(str, "%d", &lightbar_setting) == 1) {
		if (lightbar_setting > 1) {
			if (lightbar_setting > 16) {
				lightbar_hotKey = true;
			} else {
				lightbar_hotKey = false;
			}
			if(lightbar_setting != 17) {
				lightbar_51Status = true;
				lightbar_as_uart0_tx = true;
			}
		}
		LEDSERIAL_ERROR("lightbar_setting = %d\n", lightbar_setting);
		return 1;
	}
	lightbar_setting = 0;
	LEDSERIAL_ERROR("lightbar_setting is default\n");
	return 1;
}
#endif

int __init rtk_led_serial_init(void)
{
	int ret = 0;
	unsigned long long param = 0x0;

#ifdef CONFIG_RTK_KDRV_SERIAL_LED_MODULE
	lightbar_setting_module_init();
#endif
	if (lightbar_setting > 0) {
		LEDSERIAL_ERROR("This is lightbar mode!\n");
	} else {
		if (pcb_mgr_get_enum_info_byname("GOOGLE_LED_CFG", &param) == 0)
		{
			if((param & 0x3) != 0x01){
				LEDSERIAL_ERROR("not enable uart ledserial func\n");
				return ret;
			}
		}else
		{
			LEDSERIAL_ERROR("not define uart ledserial func\n");
			return ret;
		}
	}

#ifndef CONFIG_ARCH_RTK2851A
	CRT_CLK_OnOff(IRTX,CLK_ON,NULL);
#else
	_gUartRegBakup.inited = 0;
#endif

	ret = platform_driver_register(&rtk_ledserial_driver);
	if (ret != 0)
		goto FAILED_REGISTER_PLATFORM_DRIVER;

	_gstledserial_class = class_create(THIS_MODULE, "rtk-ledserial");

	if (_gstledserial_class == NULL)
		goto FAILED_CREATE_GSTLEDSERIAL_CLASS;

	led_serial_device_attr_init();
	rtk_ledserial_set_logicalmode();
	rtk_gpio_output(rtk_gpio_id(LEDSERIAL_GPIO_TYPE, LEDSERIAL_GPIO_INDEX),1);
	rtk_gpio_set_dir(rtk_gpio_id(LEDSERIAL_GPIO_TYPE, LEDSERIAL_GPIO_INDEX),1);

	pst_rtk_ledserial_platform_dev =platform_device_register_simple("rtk-ledserial", -1, NULL, 0);
	if (pst_rtk_ledserial_platform_dev == NULL) {
		goto FAILED_REGISTER_PLATFROM_DEVICE;
	}

	ret = register_ledserial_cdev();
	if (ret != 0)
		goto FAILED_REGISTER_LEDSERIAL_CDEV;

	return ret;

FAILED_REGISTER_LEDSERIAL_CDEV:
	platform_device_unregister(pst_rtk_ledserial_platform_dev);

FAILED_REGISTER_PLATFROM_DEVICE:
	device_remove_file(_gLedSerialtInfo.ledserial_device_st, &dev_attr_ledserial);
	device_remove_file(_gLedSerialtInfo.ledserial_device_st, &dev_attr_disable_ioctl);
	device_remove_file(_gLedSerialtInfo.ledserial_device_st, &dev_attr_mode);
	device_remove_file(_gLedSerialtInfo.ledserial_device_st, &dev_attr_enable_print);

	device_destroy(_gstledserial_class, MKDEV(0, 0));
	class_destroy(_gstledserial_class);

FAILED_CREATE_GSTLEDSERIAL_CLASS:
	platform_driver_unregister(&rtk_ledserial_driver);
FAILED_REGISTER_PLATFORM_DRIVER:
	LEDSERIAL_ERROR( "[%s] ======================================\n",__func__);
	LEDSERIAL_ERROR( "[%s] LEDSERIAL Register Error!\n", __func__);
	LEDSERIAL_ERROR( "[%s] ======================================\n",__func__);
	return -1;


}

static void __exit rtk_led_serial_uninit(void)
{
	cdev_del(dev_cdevp_ledserial);
	unregister_chrdev_region(MKDEV(ledserial_dev_major, ledserial_dev_minor),1);

	device_remove_file(_gLedSerialtInfo.ledserial_device_st, &dev_attr_ledserial);
	device_remove_file(_gLedSerialtInfo.ledserial_device_st, &dev_attr_disable_ioctl);
	device_remove_file(_gLedSerialtInfo.ledserial_device_st, &dev_attr_mode);
	device_remove_file(_gLedSerialtInfo.ledserial_device_st, &dev_attr_enable_print);

	device_destroy(_gstledserial_class, MKDEV(0, 0));

	class_destroy(_gstledserial_class);

	platform_driver_unregister(&rtk_ledserial_driver);

	platform_device_unregister(pst_rtk_ledserial_platform_dev);

}

late_initcall(rtk_led_serial_init);
module_exit(rtk_led_serial_uninit);
MODULE_AUTHOR("junrui.peng, Realtek Semiconductor");
MODULE_LICENSE("GPL");

