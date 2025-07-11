/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2012 by Chuck Chen <ycchen@realtek.com>
 *
 * Header for code common to machines.
 */

#ifndef __ARCH_ARM_MACH_COMMON_H
#define __ARCH_ARM_MACH_COMMON_H

#include<linux/reboot.h>

extern unsigned long DRAM_size;

extern void rtk_map_common_io(void);

extern void rtk_init_early(void);
extern void rtk_map_io(void);
extern void __init rtk_reserve_cma(phys_addr_t dma_limit, phys_addr_t lowmem_limit);
extern void __init rtk_reserve(void);
extern void __init gic_init_irq(void);
extern void rtk_l2_cache_init(void);

extern void rtk_secondary_startup(void);
extern void platform_cpu_die(unsigned int cpu);
extern int platform_cpu_disable(unsigned int cpu);
extern int platform_cpu_kill(unsigned int cpu);
extern struct smp_operations rtk_smp_ops;
extern volatile int pen_release;

extern void rtk_post_processing(void);
extern void interrupt_vendor_hook_init(void);
void parse_debugMode(void);
#endif	//__ARCH_ARM_MACH_COMMON_H
