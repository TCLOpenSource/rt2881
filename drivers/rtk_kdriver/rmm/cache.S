/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Code copied from arch/arm64/mm/cache.S.
 */

#include <linux/linkage.h>
//#include <asm/assembler.h>

#ifdef CONFIG_ARM64	
#include <asm/alternative.h>

/*
 * read_ctr - read CTR_EL0. If the system has mismatched register fields,
 * provide the system wide safe value from arm64_ftr_reg_ctrel0.sys_val
 */
	.macro	__rtk_read_ctr, reg
	mrs	\reg, ctr_el0			// read CTR
	nop
	.endm

/*
 * dcache_line_size - get the safe D-cache line size across all CPUs
 */
	.macro	__rtk_dcache_line_size, reg, tmp
	__rtk_read_ctr	\tmp
	ubfm		\tmp, \tmp, #16, #19	// cache line size encoding
	mov		\reg, #4		// bytes per word
	lsl		\reg, \reg, \tmp	// actual cache line size
	.endm

/*
 * Macro to perform a data cache maintenance for the interval
 * [kaddr, kaddr + size)
 *
 * 	op:		operation passed to dc instruction
 * 	domain:		domain used in dsb instruciton
 * 	kaddr:		starting virtual address of the region
 * 	size:		size of the region
 * 	Corrupts:	kaddr, size, tmp1, tmp2
 */
	.macro __rtk_dcache_op_workaround_clean_cache, op, kaddr
alternative_if_not ARM64_WORKAROUND_CLEAN_CACHE
	dc	\op, \kaddr
alternative_else
	dc	civac, \kaddr
alternative_endif
	.endm

	.macro __rtk_dcache_by_line_op op, domain, kaddr, size, tmp1, tmp2
	__rtk_dcache_line_size \tmp1, \tmp2
	add	\size, \kaddr, \size
	sub	\tmp2, \tmp1, #1
	bic	\kaddr, \kaddr, \tmp2
9998:
	.ifc	\op, cvau
	__rtk_dcache_op_workaround_clean_cache \op, \kaddr
	.else
	.ifc	\op, cvac
	__rtk_dcache_op_workaround_clean_cache \op, \kaddr
	.else
	.ifc	\op, cvap
	sys	3, c7, c12, 1, \kaddr	// dc cvap
	.else
	.ifc	\op, cvadp
	sys	3, c7, c13, 1, \kaddr	// dc cvadp
	.else
	dc	\op, \kaddr
	.endif
	.endif
	.endif
	.endif
	add	\kaddr, \kaddr, \tmp1
	cmp	\kaddr, \size
	b.lo	9998b
	dsb	\domain
	.endm

/*
 *	__rtk_dma_flush_area(start, size)
 *
 *	clean & invalidate D / U line
 *
 *	- start   - virtual start address of region
 *	- size    - size in question
 */
SYM_FUNC_START_PI(__rtk_dma_flush_area)
	__rtk_dcache_by_line_op civac, sy, x0, x1, x2, x3
	ret
SYM_FUNC_END_PI(__rtk_dma_flush_area)

/*
 *	__rtk_dma_clean_area(start, size)
 *	- start   - virtual start address of region
 *	- size    - size in question
 */
SYM_FUNC_START_PI(__rtk_dma_clean_area)
	__rtk_dcache_by_line_op cvac, sy, x0, x1, x2, x3
	ret
SYM_FUNC_END_PI(__rtk_dma_clean_area)

/*
 *	__rtk_dma_inv_area(start, size)
 *	- start   - virtual start address of region
 *	- size    - size in question
 */
SYM_FUNC_START_PI(__rtk_dma_inv_area)
	add	x1, x1, x0
	__rtk_dcache_line_size x2, x3
	sub	x3, x2, #1
	tst	x1, x3				// end cache line aligned?
	bic	x1, x1, x3
	b.eq	1f
	dc	civac, x1			// clean & invalidate D / U line
1:	tst	x0, x3				// start cache line aligned?
	bic	x0, x0, x3
	b.eq	2f
	dc	civac, x0			// clean & invalidate D / U line
	b	3f
2:	dc	ivac, x0			// invalidate D / U line
3:	add	x0, x0, x2
	cmp	x0, x1
	b.lo	2b
	dsb	sy
	ret
SYM_FUNC_END_PI(__rtk_dma_inv_area)

#else

	.irp	c,,eq,ne,cs,cc,mi,pl,vs,vc,hi,ls,ge,lt,gt,le,hs,lo
	.macro	ret\c, reg
#if __LINUX_ARM_ARCH__ < 6
	mov\c	pc, \reg
#else
	.ifeqs	"\reg", "lr"
	bx\c	\reg
	.else
	mov\c	pc, \reg
	.endif
#endif
	.endm
	.endr
	
/*
 * dcache_line_size - get the minimum D-cache line size from the CTR register
 * on ARMv7.
 */
	.macro	__rtk_dcache_line_size, reg, tmp
#ifdef CONFIG_CPU_V7M
	movw	\tmp, #:lower16:BASEADDR_V7M_SCB + V7M_SCB_CTR
	movt	\tmp, #:upper16:BASEADDR_V7M_SCB + V7M_SCB_CTR
	ldr     \tmp, [\tmp]
#else
	mrc	p15, 0, \tmp, c0, c0, 1		@ read ctr
#endif
	lsr	\tmp, \tmp, #16
	and	\tmp, \tmp, #0xf		@ cache line size encoding
	mov	\reg, #4			@ bytes per word
	mov	\reg, \reg, lsl \tmp		@ actual cache line size
	.endm

/*
 *	v7_dma_flush_range(start,end)
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
ENTRY(__rtk_v7_dma_flush_range)
	__rtk_dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r0, r0, r3
#ifdef CONFIG_ARM_ERRATA_764369
	ALT_SMP(W(dsb))
	ALT_UP(W(nop))
#endif
1:
	mcr	p15, 0, r0, c7, c14, 1		@ clean & invalidate D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb	st
	ret	lr
ENDPROC(__rtk_v7_dma_flush_range)

/*
 *	v7_dma_clean_range(start,end)
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
ENTRY(__rtk_v7_dma_clean_range)
	__rtk_dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r0, r0, r3
#ifdef CONFIG_ARM_ERRATA_764369
	ALT_SMP(W(dsb))
	ALT_UP(W(nop))
#endif
1:
	mcr	p15, 0, r0, c7, c10, 1		@ clean D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb	st
	ret	lr
ENDPROC(__rtk_v7_dma_clean_range)

/*
 *	v7_dma_inv_range(start,end)
 *
 *	Invalidate the data cache within the specified region; we will
 *	be performing a DMA operation in this region and we want to
 *	purge old data in the cache.
 *
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
ENTRY(__rtk_v7_dma_inv_range)
	__rtk_dcache_line_size r2, r3
	sub	r3, r2, #1
	tst	r0, r3
	bic	r0, r0, r3
#ifdef CONFIG_ARM_ERRATA_764369
	ALT_SMP(W(dsb))
	ALT_UP(W(nop))
#endif
	mcrne	p15, 0, r0, c7, c14, 1		@ clean & invalidate D / U line
	addne	r0, r0, r2

	tst	r1, r3
	bic	r1, r1, r3
	mcrne	p15, 0, r1, c7, c14, 1		@ clean & invalidate D / U line
	cmp	r0, r1
1:
	mcrlo	p15, 0, r0, c7, c6, 1		@ invalidate D / U line
	addlo	r0, r0, r2
	cmplo	r0, r1
	blo	1b
	dsb	st
	ret	lr
ENDPROC(__rtk_v7_dma_inv_range)

#endif