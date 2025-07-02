#ifndef _RTK_UART_IO_H_
#define _RTK_UART_IO_H_

// for serial port used
#include <mach/system.h>
#include <rbus/misc_reg.h>
#include <rbus/misc_uart_reg.h>
#include <rbus/wdog_reg.h>
#include <rbus/iso_misc_off_reg.h>
#include <mach/serial.h>

#ifdef CONFIG_ARCH_RTK2885P
//#include <rbus/M8P_misc_uart2_reg.h>
#include <../common_header/rbus/rtd7032/M8P_misc_uart2_reg.h>
#endif
#include <mach/rtk_platform.h>


#ifdef CONFIG_RTK_KDRV_SERIAL_8250_UART_DMA
#if (SYSTEM_CONSOLE == 0)
	#define SERIAL_PORT_DFNS    \
		{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (char *)(ISO_MISC_OFF_UART_U0RBR_THR_DLL_reg), 2, IRQF_SHARED, \
			0, 0, 0, 0},   /* ttyS0 */
#elif (SYSTEM_CONSOLE == 1)
	#define SERIAL_PORT_DFNS    \
		{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED, \
			(char *)(MISC_UART_UR1_CTRL_reg), PAGE_SIZE, 0, 8},   /* ttyS0 */
#elif (SYSTEM_CONSOLE == 2)
	#define SERIAL_PORT_DFNS    \
		{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (char *)(MISC_UART_U2RBR_THR_DLL_reg), 2, IRQF_SHARED, \
			0, 0, 0, 0},   /* ttyS0 */
#endif

#if (CONFIG_RTK_KDRV_SERIAL_8250_RUNTIME_UARTS == 2)
	#undef SERIAL_PORT_DFNS
	#if (SYSTEM_CONSOLE == 0)
		#define SERIAL_PORT_DFNS    \
      	{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(ISO_MISC_OFF_UART_U0RBR_THR_DLL_reg), 2, IRQF_SHARED, \
      		0, 0, 0, 0}, /* ttyS0 */ \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED, \
				(char *)(MISC_UART_UR1_CTRL_reg), PAGE_SIZE, 0, 8},   /* ttyS1 */
	#elif (SYSTEM_CONSOLE == 1)
		#define SERIAL_PORT_DFNS    \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED, \
				(char *)(MISC_UART_UR1_CTRL_reg), PAGE_SIZE, 0, 8},   /* ttyS0 */ \
         { 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(ISO_MISC_OFF_UART_U0RBR_THR_DLL_reg), 2, IRQF_SHARED, \
         	0, 0, 0, 0}, /* ttyS1 */
	#endif
#endif

#else
#if (SYSTEM_CONSOLE == 0)
	#define SERIAL_PORT_DFNS    \
		{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (char *)(ISO_MISC_OFF_UART_U0RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS0 */
#elif (SYSTEM_CONSOLE == 1)
	#define SERIAL_PORT_DFNS    \
		{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS0 */
#elif (SYSTEM_CONSOLE == 2)
	#define SERIAL_PORT_DFNS    \
		{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (char *)(MISC_UART_U2RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS0 */
#endif

#if (CONFIG_RTK_KDRV_SERIAL_8250_RUNTIME_UARTS == 2)
	#undef SERIAL_PORT_DFNS
#ifndef CONFIG_REALTEK_FPGA
	#if (SYSTEM_CONSOLE == 0)
		#define SERIAL_PORT_DFNS    \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(ISO_MISC_OFF_UART_U0RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS0 */ \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS1 */
	#elif (SYSTEM_CONSOLE == 1)
		#define SERIAL_PORT_DFNS    \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED}, 	/* ttyS1 */ \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(ISO_MISC_OFF_UART_U0RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS0 */
	#endif
#else
	#define SERIAL_PORT_DFNS    \
	{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED},       /* ttyS0 */ \
	{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED},       /* ttyS1 */
#endif
#endif
#endif


#ifdef  CONFIG_ARCH_RTK2885P

#ifdef CONFIG_RTK_KDRV_SERIAL_8250_UART_DMA
#if (SYSTEM_CONSOLE == 0)
	#define RTD2885P_SERIAL_PORT_DFNS    \
		{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (char *)(ISO_MISC_OFF_UART_U0RBR_THR_DLL_reg), 2, IRQF_SHARED, 0, 0, 0, 0},   /* ttyS0 */
#elif (SYSTEM_CONSOLE == 1)
	#define RTD2885P_SERIAL_PORT_DFNS    \
		{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED, (char *)(MISC_UART_UR1_CTRL_reg), PAGE_SIZE, 0, 8},   /* ttyS0 */
#elif (SYSTEM_CONSOLE == 2)
	#define RTD2885P_SERIAL_PORT_DFNS    \
		{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (char *)(MISC_UART_U2RBR_THR_DLL_reg), 2, IRQF_SHARED, 0, 0, 0, 0},   /* ttyS0 */
#endif

#if (CONFIG_RTK_KDRV_SERIAL_8250_RUNTIME_UARTS == 2)
	#undef RTD2885P_SERIAL_PORT_DFNS
	#if (SYSTEM_CONSOLE == 0)
		#define RTD2885P_SERIAL_PORT_DFNS    \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(ISO_MISC_OFF_UART_U0RBR_THR_DLL_reg), 2, IRQF_SHARED, 0, 0, 0, 0}, /* ttyS0 */ \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED, (char *)(MISC_UART_UR1_CTRL_reg), PAGE_SIZE, 0, 8}, /* ttyS1 */ \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(M8P_MISC_UART2_U2RBR_THR_DLL_reg), 2, IRQF_SHARED,    0, 0, 0, 0},   /* ttyS2 */
	#elif (SYSTEM_CONSOLE == 1)
		#define RTD2885P_SERIAL_PORT_DFNS    \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED, (char *)(MISC_UART_UR1_CTRL_reg), PAGE_SIZE, 0, 8},   /* ttyS0 */ \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(ISO_MISC_OFF_UART_U0RBR_THR_DLL_reg), 2, IRQF_SHARED, 0, 0, 0, 0}, /* ttyS1 */ \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(M8P_MISC_UART2_U2RBR_THR_DLL_reg),    2, IRQF_SHARED, 0, 0, 0, 0}, /* ttyS2 */
	#endif
#endif

#else
#if (SYSTEM_CONSOLE == 0)
	#define RTD2885P_SERIAL_PORT_DFNS    \
		{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (char *)(ISO_MISC_OFF_UART_U0RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS0 */
#elif (SYSTEM_CONSOLE == 1)
	#define RTD2885P_SERIAL_PORT_DFNS    \
		{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS0 */
#elif (SYSTEM_CONSOLE == 2)
	#define RTD2885P_SERIAL_PORT_DFNS    \
		{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (char *)(MISC_UART_U2RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS0 */
#endif

#if (CONFIG_RTK_KDRV_SERIAL_8250_RUNTIME_UARTS == 2)
	#undef RTD2885P_SERIAL_PORT_DFNS
#ifndef CONFIG_REALTEK_FPGA
	#if (SYSTEM_CONSOLE == 0)
		#define RTD2885P_SERIAL_PORT_DFNS    \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(ISO_MISC_OFF_UART_U0RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS0 */ \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS1 */ \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(M8P_MISC_UART2_U2RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS2 */
	#elif (SYSTEM_CONSOLE == 1)
		#define RTD2885P_SERIAL_PORT_DFNS    \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED}, 	/* ttyS1 */ \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(ISO_MISC_OFF_UART_U0RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS0 */ \
			{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(M8P_MISC_UART2_U2RBR_THR_DLL_reg), 2, IRQF_SHARED},   /* ttyS2 */
	#endif
#else
	#define RTD2885P_SERIAL_PORT_DFNS    \
	{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED},       /* ttyS0 */ \
	{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(MISC_UART_U1RBR_THR_DLL_reg), 2, IRQF_SHARED},       /* ttyS1 */ \
	{ 0, (UART_CLOCK>>4), 0, -1, UPF_SHARE_IRQ|UPF_BOOT_AUTOCONF, 0, UPIO_MEM32, (unsigned char *)(M8P_MISC_UART2_U2RBR_THR_DLL_reg), 2, IRQF_SHARED},       /* ttyS2 */
#endif
#endif
#endif

#else
#define RTD2885P_SERIAL_PORT_DFNS SERIAL_PORT_DFNS
#endif

#endif

