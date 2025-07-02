#ifndef _RTK_PLATFORM_H
#define _RTK_PLATFORM_H

#ifndef BUILD_QUICK_SHOW
#include <linux/pageremap.h>
#else
#include <no_os/kernel.h>
#include <no_os/pageremap.h>
#endif
#include <rtk_kdriver/rmm/rmm.h>
#include <rtk_kdriver/rmm/rtk_mem_layout.h>

extern _Bool mem_type_2mc;
extern _Bool is_ddr_unbalance;

enum PLAFTORM_TYPE
{
	PLATFORM_RTD299OP	= 0, //K2LP
	PLATFORM_RTD299O	= 1, //K2L
	PLATFORM_RTD289XP	= 2, //K3LP
	PLATFORM_RTD289X	= 3, //K3L
	PLATFORM_RTD289XPP	= 4, //K3LPP
	PLATFORM_RTD284X	= 5, //S4AP
	PLATFORM_RTD287X	= 6, //K4L
	PLATFORM_RTD285X	= 7, //Mac6
    PLATFORM_RTD287O    = 8, //K5L
    PLATFORM_RTD285O    = 9, //Mac6P
    PLATFORM_RTD288O    = 10, //K6L
    PLATFORM_RTD2851A   = 11,
    PLATFORM_RTD2875    = 12,
	PLATFORM_OTHER		= 255
};

#define PLATFORM_KXLP	PLATFORM_RTD2875
#define PLATFORM_KXL	PLATFORM_OTHER
#define PLATFORM_KXLPP	PLATFORM_OTHER

enum PLAFTORM_TYPE get_platform (void);


/*[MAC8PBU-332] need API for distinguish IC type*/
enum MACH_TYPE
{
	MACH_ARCH_OTHER		= 0,
	MACH_ARCH_RTK6748,
	MACH_ARCH_RTK6702,
	MACH_ARCH_RTK2851A,
#if defined(CONFIG_ARCH_RTK2851C)
	MACH_ARCH_RTK2851C,
#endif
#if defined(CONFIG_ARCH_RTK2851F)
	MACH_ARCH_RTK2851F,
#endif
	MACH_ARCH_RTK2885P,
	MACH_ARCH_RTK2875Q,
	MACH_ARCH_RTK2819A,
	MACH_ARCH_RTK2885PP,
	MACH_ARCH_RTK2885P2
};
enum MACH_TYPE get_mach_type(void);

//==============================================
enum PLATFORM_MODEL
{
	PLATFORM_MODEL_2K = 2,
	PLATFORM_MODEL_4K = 4,
	PLATFORM_MODEL_8K = 8,
	PLATFORM_MODEL_5K = 16,
	PLATFORM_MODEL_OTHER = 255
};


/* Ref https://jira.realtek.com/browse/MM2NUM-478 */
enum PRODUCT_TYPE
{
	PRODUCT_TYPE_DIAS = 1,
	PRODUCT_TYPE_DEMETER = 2,	//TV
	PRODUCT_TYPE_OTHER = 255
};

#define PRODUCT_TYPE_DEFAULT PRODUCT_TYPE_DEMETER

enum PRODUCT_TYPE get_product_type(void);

#define PLATFORM_MODEL_DEFAULT PLATFORM_MODEL_8K

enum PLATFORM_MODEL get_platform_model(void);

enum DISPLAY_RESOLUTION
{
	DISPLAY_RESOLUTION_2K = 2,
	DISPLAY_RESOLUTION_4K = 4,
	DISPLAY_RESOLUTION_8K = 8,
	DISPLAY_RESOLUTION_OTHER = 255
};

#define DISPLAY_RESOLUTION_DEFAULT DISPLAY_RESOLUTION_4K

enum DISPLAY_RESOLUTION get_display_resolution(void);

enum UI_RESOLUTION
{
	UI_RESOLUTION_720P = 1,
	UI_RESOLUTION_2K = 2,
	UI_RESOLUTION_4K = 4,
	UI_RESOLUTION_OTHER = 255
};

enum MEMC_INIT
{
	MEMC_INIT_FALSE = 0,
	MEMC_INIT_TRUE = 1,
	MEMC_INIT_UNKNOWN = 255
};

#define MEMC_INIT_DEFAULT MEMC_INIT_TRUE

#define UI_RESOLUTION_DEFAULT UI_RESOLUTION_2K

enum UI_RESOLUTION get_ui_resolution(void);
enum MEMC_INIT is_memc_init(void);

enum BOARD_VERSION{
	BOARD_A = 0,
	BOARD_B,
	BOARD_C,

	// mac8p TV version 10-19
	BOARD_2851C_demeter_ver1 = 10,     // RTK Mac8p Demeter PCB ver1,
	BOARD_2851C_demeter_ver2,          // RTK Mac8p Demeter PCB ver2,
	BOARD_2851C_demeter_ver3,          // RTK Mac8p Demeter PCB ver3,

	BOARD_2851F_demeter_ver1 = 10,     // RTK Mac8p Demeter PCB ver1,
	BOARD_2851F_demeter_ver2,          // RTK Mac8p Demeter PCB ver2,
	BOARD_2851F_demeter_ver3,          // RTK Mac8p Demeter PCB ver3,
	
	// mac8p dias version 20-39
	BOARD_tv001_2817A_dias_ver1 = 20,  // RTK Mac8p Dias PCB ver1, 
	BOARD_tv006_2817A_dias_ver1,       // tv006 Mac8p Dias PCB ver1
	BOARD_tv030_2817A_dias_ver1,       // tv030 Mac8p Dias PCB ver1
	BOARD_tv001_2817A_dias_ver2,       // RTK Mac8p Dias PCB ver2 
	BOARD_tv006_2817A_dias_ver2,       // tv006 Mac8p Dias PCB ver2
	BOARD_tv030_2817A_dias_ver2,       // tv030 Mac8p Dias PCB ver2
	
	BOARD_UNDEFINED = 255
};

#define BOARD_VERSION_DEFAULT BOARD_A

enum BOARD_VERSION get_board_version (void);

//==============================================

unsigned long get_uart_clock(void);
unsigned long get_uart_clock_from_reg_setting(unsigned int uart_no);

unsigned int get_irq_num(unsigned int hwirq);

extern int early_parse_platform_model(char *para);
extern int early_parse_product_type(char *para);

#endif //_RTK_PLATFORM_H

