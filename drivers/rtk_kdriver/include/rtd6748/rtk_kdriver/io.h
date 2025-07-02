/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2012 by Chuck Chen <ycchen@realtek.com>
 *
 * IO mappings for rtd299o
 */

#ifndef _ASM_MACH_IO_H_
#define _ASM_MACH_IO_H_

#include <mach/iomap.h>
#include <asm/io.h>

#define __io(a)     __typesafe_io((a) & IO_SPACE_LIMIT)
#ifdef CONFIG_ARM64
#define __esb()     asm volatile("esb" : : : "memory")
#else
#define __esb()     ;
#endif
__attribute__((__flatten__)) static inline unsigned char rtd_inb(unsigned long addr)
{
    unsigned char val;
    if (addr < (RBUS_BASE_VIRT_OLD+RBUS_BASE_SIZE) && addr >= RBUS_BASE_VIRT_OLD)
        val = __raw_readb((void __iomem *)(addr-RBUS_BASE_VIRT_OLD+RBUS_BASE_VIRT));
    else
        val = __raw_readb((void __iomem *)addr);
    __esb();
    return val;
}
__attribute__((__flatten__)) static inline unsigned short rtd_inw(unsigned long addr)
{
    unsigned short val;
    if (addr < (RBUS_BASE_VIRT_OLD+RBUS_BASE_SIZE) && addr >= RBUS_BASE_VIRT_OLD)
        return __raw_readw((void __iomem *)(addr-RBUS_BASE_VIRT_OLD+RBUS_BASE_VIRT));
    else
        return __raw_readw((void __iomem *)addr);
    __esb();
    return val;
}
__attribute__((__flatten__))  static inline unsigned int rtd_inl(unsigned long addr)
{
    unsigned int val;
    if (addr < (RBUS_BASE_VIRT_OLD+RBUS_BASE_SIZE) && addr >= RBUS_BASE_VIRT_OLD)
        val = __raw_readl((void __iomem *)(addr-RBUS_BASE_VIRT_OLD+RBUS_BASE_VIRT));
    else
        val = __raw_readl((void __iomem *)addr);
    __esb();
    return val;
}
__attribute__((__flatten__)) static inline void rtd_outb(unsigned long addr, unsigned char val)
{
    if (addr < (RBUS_BASE_VIRT_OLD+RBUS_BASE_SIZE) && addr >= RBUS_BASE_VIRT_OLD)
        __raw_writeb(val,(void __iomem *)(addr-RBUS_BASE_VIRT_OLD+RBUS_BASE_VIRT));
    else
        __raw_writeb(val,(void __iomem *)addr);
    __esb();
}
__attribute__((__flatten__)) static inline void rtd_outw(unsigned long addr, unsigned short val)
{
    if (addr < (RBUS_BASE_VIRT_OLD+RBUS_BASE_SIZE) && addr >= RBUS_BASE_VIRT_OLD)
        __raw_writew(val,(void __iomem *)(addr-RBUS_BASE_VIRT_OLD+RBUS_BASE_VIRT));
    else
        __raw_writew(val,(void __iomem *)addr);
    __esb();
}
__attribute__((__flatten__)) static inline void rtd_outl(unsigned long addr, unsigned int val)
{
    if (addr < (RBUS_BASE_VIRT_OLD+RBUS_BASE_SIZE) && addr >= RBUS_BASE_VIRT_OLD)
        __raw_writel(val,(void __iomem *)(addr-RBUS_BASE_VIRT_OLD+RBUS_BASE_VIRT));
    else
        __raw_writel(val,(void __iomem *)addr);
    __esb();
}

static inline void rtd_part_outl(unsigned int reg_addr, unsigned int endBit, unsigned int startBit, unsigned int value)
{
    if((endBit - startBit + 1) < 32)
        rtd_outl(reg_addr,(rtd_inl(reg_addr) & (~(((1<<(endBit-startBit+1))-1)<<startBit))) | (value<<startBit));
    else
        rtd_outl(reg_addr, value);
}

static inline unsigned int rtd_part_inl(unsigned int reg_addr, unsigned int endBit, unsigned int startBit)
{
	return (rtd_inl(reg_addr)>>startBit)&((1<<(endBit-startBit+1))-1);
}


#define rtd_maskl(offset, andMask, orMask) rtd_outl(offset, ((rtd_inl(offset) & (andMask)) | (orMask)))
#define rtd_setbits(offset, Mask) rtd_outl(offset, (rtd_inl(offset) | Mask))
#define rtd_clearbits(offset, Mask) rtd_outl(offset, ((rtd_inl(offset) & ~(Mask))))

#include <mach/system.h>
#include <rbus/misc_reg.h>
#include <rbus/misc_uart_reg.h>
#include <rbus/wdog_reg.h>
#include <rbus/iso_misc_off_reg.h>
#include <mach/serial.h>

#endif	//_ASM_MACH_IO_H_
