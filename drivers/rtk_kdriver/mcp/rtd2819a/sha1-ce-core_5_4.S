#include <linux/linkage.h>
#include <asm/assembler.h>

	.text
	.arch		armv8-a+crypto

	k0		.req	v0
	k1		.req	v1
	k2		.req	v2
	k3		.req	v3

	t0		.req	v4
	t1		.req	v5

	dga		.req	q6
	dgav		.req	v6
	dgb		.req	s7
	dgbv		.req	v7

	dg0q		.req	q12
	dg0s		.req	s12
	dg0v		.req	v12
	dg1s		.req	s13
	dg1v		.req	v13
	dg2s		.req	s14

	.macro		add_only, op, ev, rc, s0, dg1
	.ifc		\ev, ev
	add		t1.4s, v\s0\().4s, \rc\().4s
	sha1h		dg2s, dg0s
	.ifnb		\dg1
	sha1\op		dg0q, \dg1, t0.4s
	.else
	sha1\op		dg0q, dg1s, t0.4s
	.endif
	.else
	.ifnb		\s0
	add		t0.4s, v\s0\().4s, \rc\().4s
	.endif
	sha1h		dg1s, dg0s
	sha1\op		dg0q, dg2s, t1.4s
	.endif
	.endm

	.macro		add_update, op, ev, rc, s0, s1, s2, s3, dg1
	sha1su0		v\s0\().4s, v\s1\().4s, v\s2\().4s
	add_only	\op, \ev, \rc, \s1, \dg1
	sha1su1		v\s0\().4s, v\s3\().4s
	.endm

	.macro		loadrc, k, val, tmp
	movz		\tmp, :abs_g0_nc:\val
	movk		\tmp, :abs_g1:\val
	dup		\k, \tmp
	.endm

	/*
	 * void rtk_sha1_ce_transform(struct sha1_ce_state *sst, u8 const *src,
	 *			  int blocks)
	 */
ENTRY(rtk_sha1_ce_transform)
	frame_push	3

	mov		x19, x0
	mov		x20, x1
	mov		x21, x2

	/* load round constants */
0:	loadrc		k0.4s, 0x5a827999, w6
	loadrc		k1.4s, 0x6ed9eba1, w6
	loadrc		k2.4s, 0x8f1bbcdc, w6
	loadrc		k3.4s, 0xca62c1d6, w6

	/* load state */
	ld1		{dgav.4s}, [x19]
	ldr		dgb, [x19, #16]

	/* load sha1_ce_state::finalize */
	ldr_l		w4, rtk_sha1_ce_offsetof_finalize, x4
	ldr		w4, [x19, x4]

	/* load input */
1:	ld1		{v8.4s-v11.4s}, [x20], #64
	sub		w21, w21, #1

CPU_LE(	rev32		v8.16b, v8.16b		)
CPU_LE(	rev32		v9.16b, v9.16b		)
CPU_LE(	rev32		v10.16b, v10.16b	)
CPU_LE(	rev32		v11.16b, v11.16b	)

2:	add		t0.4s, v8.4s, k0.4s
	mov		dg0v.16b, dgav.16b

	add_update	c, ev, k0,  8,  9, 10, 11, dgb
	add_update	c, od, k0,  9, 10, 11,  8
	add_update	c, ev, k0, 10, 11,  8,  9
	add_update	c, od, k0, 11,  8,  9, 10
	add_update	c, ev, k1,  8,  9, 10, 11

	add_update	p, od, k1,  9, 10, 11,  8
	add_update	p, ev, k1, 10, 11,  8,  9
	add_update	p, od, k1, 11,  8,  9, 10
	add_update	p, ev, k1,  8,  9, 10, 11
	add_update	p, od, k2,  9, 10, 11,  8

	add_update	m, ev, k2, 10, 11,  8,  9
	add_update	m, od, k2, 11,  8,  9, 10
	add_update	m, ev, k2,  8,  9, 10, 11
	add_update	m, od, k2,  9, 10, 11,  8
	add_update	m, ev, k3, 10, 11,  8,  9

	add_update	p, od, k3, 11,  8,  9, 10
	add_only	p, ev, k3,  9
	add_only	p, od, k3, 10
	add_only	p, ev, k3, 11
	add_only	p, od

	/* update state */
	add		dgbv.2s, dgbv.2s, dg1v.2s
	add		dgav.4s, dgav.4s, dg0v.4s

	cbz		w21, 3f

	if_will_cond_yield_neon
	st1		{dgav.4s}, [x19]
	str		dgb, [x19, #16]
	do_cond_yield_neon
	b		0b
	endif_yield_neon

	b		1b

	/*
	 * Final block: add padding and total bit count.
	 * Skip if the input size was not a round multiple of the block size,
	 * the padding is handled by the C code in that case.
	 */
3:	cbz		x4, 4f
	ldr_l		w4, rtk_sha1_ce_offsetof_count, x4
	ldr		x4, [x19, x4]
	movi		v9.2d, #0
	mov		x8, #0x80000000
	movi		v10.2d, #0
	ror		x7, x4, #29		// ror(lsl(x4, 3), 32)
	fmov		d8, x8
	mov		x4, #0
	mov		v11.d[0], xzr
	mov		v11.d[1], x7
	b		2b

	/* store new state */
4:	st1		{dgav.4s}, [x19]
	str		dgb, [x19, #16]
	frame_pop
	ret
ENDPROC(rtk_sha1_ce_transform)
