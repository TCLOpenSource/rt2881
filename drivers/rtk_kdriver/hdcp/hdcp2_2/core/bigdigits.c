/* $Id: bigdigits.c $ */

/******************** SHORT COPYRIGHT NOTICE**************************
This source code is part of the BigDigits multiple-precision
arithmetic library Version 2.2 originally written by David Ireland,
copyright (c) 2001-8 D.I. Management Services Pty Limited, all rights
reserved. It is provided "as is" with no warranties. You may use
this software under the terms of the full copyright notice
"bigdigitsCopyright.txt" that should have been included with this
library or can be obtained from <www.di-mgt.com.au/bigdigits.html>.
This notice must always be retained in any copy.
******************* END OF COPYRIGHT NOTICE***************************/
/*
	Last updated:
	$Date: 2008-07-31 12:54:00 $
	$Revision: 2.2.0 $
	$Author: dai $
*/

/* Core code for BigDigits library "mp" functions */

#include <linux/time.h>
#include <hdcp/hdcp2_2/bigdigits.h>
#include <hdcp/hdcp2_2/hdcp2_hal.h>

/* For debugging - these are NOOPs */
#define DPRINTF0(s)
#define DPRINTF1(s, a1)

#ifdef DEBUG_BIGDIGITS
#define LOCALDBG ENABLE
#else
#define LOCALDBG DISABLE
#endif

/***************************************/
/* VERSION NUMBERS - USED IN MPVERSION */
/***************************************/
static const int kMajor = 2, kMinor = 2, kRelease;

/* Flags for preprocessor definitions used (=last digit of mpVersion) */
#ifdef USE_SPASM
static const int kUseSpasm = 1;

#else
static const int kUseSpasm;

#endif

#ifdef USE_64WITH32
static const int kUse64with32 = 2;

#else
static const int kUse64with32;

#endif

#ifdef NO_ALLOCS
static const int kUseNoAllocs = 5;

#else
static const int kUseNoAllocs;

#endif

/* Useful definitions */
#ifndef max
#define max(a, b)            (((a) > (b)) ? (a) : (b))
#endif

/****************************/
/* ERROR HANDLING FUNCTIONS */
/****************************/
/* Change these to suit your tastes and operating system. */
#if defined(_WIN32) || defined(WIN32)
/* Win32 GUI alternative */
#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
void mpFail(char *msg)
{
	MessageBox(NULL, msg, "BigDigits Error", MB_ICONERROR);
	exit(EXIT_FAILURE);
}
#else	/* Ordinary console program */

void mpFail(char *msg)
{
	H2DBGLOG((LOCALDBG, "%s\n", msg));
}

void assert(int x);

void assert(int x)
{
	if (!x) {
		H2DBGLOG((LOCALDBG, "ASSERT!!!!!"));
	}
}

#endif

/*******************************/
/* MEMORY ALLOCATION FUNCTIONS */
/*******************************/
#ifndef NO_ALLOCS
DIGIT_T *mpAlloc(size_t ndigits)
{
	DIGIT_T *ptr;
	ptr = (DIGIT_T *) kcalloc(ndigits, sizeof(DIGIT_T), GFP_KERNEL);
	if (!ptr)
		mpFail((char *)"mpAlloc: Unable to allocate memory.");
	return ptr;
}

void mpFree(DIGIT_T **p)
{
	if (*p) {
		kfree(*p);
		*p = NULL;
	}
}

#endif	/* NO_ALLOCS */

/* Force linker to include copyright notice in executable object image */
volatile char *copyright_notice(void)
{
	return (volatile char *)
		"Contains multiple-precision arithmetic code originally written by David Ireland,"
		 " copyright (c) 2001-8 by D.I. Management Services Pty Limited <www.di-mgt.com.au>,"
		 " and is used with permission.";
}

/* To use, include this statement somewhere in the final code:

	copyright_notice();

It has no real effect at run time.
Thanks to Phil Zimmerman for this idea.
*/

/****************/
/* VERSION INFO */
/****************/
int mpVersion(void)
{
	return kMajor * 1000 + kMinor * 100 + kRelease * 10 + kUseSpasm + kUse64with32 + kUseNoAllocs;
}


/**************************************/
/* CORE SINGLE PRECISION CALCULATIONS */
/* (double where necessary)           */
/**************************************/

/* [v2.2] Moved these functions into main file
	and added third option using 64-bit arithmetic if available.
OPTIONS:
1. define USE_64WITH32 to use 64-bit types on a 32-bit machine; or
2. define USE_SPASM to use Intel ASM (32-bit Intel compilers with __asm support); or
3. use default "long" calculations (any platform)
*/

#ifdef USE_64WITH32
/* 1. We are on a 32-bit machine with a 64-bit type available. */
#pragma message("USE_64WITH32 is set")

/* Make sure we have a uint64_t available */
#if defined (_WIN32) || defined(WIN32)
typedef unsigned __int64 uint64_t;

#elif !defined(HAVE_C99INCLUDES) && !defined(HAVE_SYS_TYPES)
typedef unsigned long long int uint64_t;

#endif
int spMultiply(uint32_t p[2], uint32_t x, uint32_t y)
{
	/* Use a 64-bit temp for product */
	uint64_t t = (uint64_t) x * (uint64_t) y;

	/* then split into two parts */
	p[1] = (uint32_t) (t >> 32);
	p[0] = (uint32_t) (t & 0xFFFFFFFF);
	return 0;
}

uint32_t spDivide(uint32_t *pq, uint32_t *pr, const uint32_t u[2], uint32_t v)
{
	uint64_t uu, q;
	uu = (uint64_t) u[1] << 32 | (uint64_t) u[0];
	q = uu / (uint64_t) v;

	*pr = (uint32_t) (uu - q * v);
	*pq = (uint32_t) (q & 0xFFFFFFFF);
	return (uint32_t) (q >> 32);
}

#elif defined(USE_SPASM)
/* Use Intel MASM to compute sp products and divisions */
#pragma message("Using MASM")
int spMultiply(uint32_t p[2], uint32_t x, uint32_t y)
/* ASM version explicitly for 32-bit integers */
{
/* Computes p = (p1p0) = x * y. No restrictions on input. */
	__asm {
		mov eax, x
		xor edx, edx
		mul y
		;
		Product in edx : eax
		mov ebx, p
		mov dword ptr[ebx], eax
		mov dword ptr[ebx + 4], edx
	}
	return 0;
}

uint32_t spDivide(uint32_t *pq, uint32_t *pr, const uint32_t u[2], uint32_t v)
/* ASM version explicitly for 32-bit integers */
{
/* Computes quotient q = u / v, remainder r = u mod v.
   Returns overflow (1) if q > word size (b) otherwise returns 0.
   CAUTION: Requires v >= [b/2] i.e. v to have its high bit set.
   (q1q0) = (u1u0)/v0
   (r0)   = (u1u0) mod v0
   Sets *pr = r0, *pq = q0 and returns "overflow" q1 (either 0 or 1).
*/
	uint32_t overflow = 0;
	__asm {
		;
		Dividend u in EDX : EAX, divisor in v
		mov ebx, u
		mov eax, dword ptr[ebx]
		mov edx, dword ptr[ebx + 4]
		;
		Catch overflow(edx >= divisor)
		cmp edx, v
		jb no_overflow
		;
		If so, set edx = edx - divisor and flag it
		sub edx, v
		mov overflow, 1
		no_overflow :
		div v
		;
		Quotient in EAX, Remainder in EDX
		mov ebx, pq
		mov dword ptr[ebx], eax
		mov ebx, pr
		mov dword ptr[ebx], edx
	}
	return overflow;
}

#else
/* Default routines the "long" way */
int spMultiply(DIGIT_T p[2], DIGIT_T x, DIGIT_T y)
{
	/*      Computes p = x * y */

	/*      Ref: Arbitrary Precision Computation
	       http://numbers.computation.free.fr/Constants/constants.html

	       high    p1                p0     low
	       +--------+--------+--------+--------+
	       |      x1*y1      |      x0*y0      |
	       +--------+--------+--------+--------+
	       +-+--------+--------+
	       |1| (x0*y1 + x1*y1) |
	       +-+--------+--------+
	       ^carry from adding (x0*y1+x1*y1) together
	       +-+
	       |1|< carry from adding LOHALF t
	       +-+  to high half of p0
	 */
	DIGIT_T x0, y0, x1, y1;
	DIGIT_T t, u, carry;

	/*      Split each x,y into two halves
	       x = x0 + B*x1
	       y = y0 + B*y1
	       where B = 2^16, half the digit size
	       Product is
	       xy = x0y0 + B(x0y1 + x1y0) + B^2(x1y1)
	 */
	x0 = LOHALF(x);
	x1 = HIHALF(x);
	y0 = LOHALF(y);
	y1 = HIHALF(y);

	/* Calc low part - no carry */
	p[0] = x0 * y0;

	/* Calc middle part */
	t = x0 * y1;
	u = x1 * y0;
	t += u;
	if (t < u)
		carry = 1;
	else
		carry = 0;

	/*      This carry will go to high half of p[1]
	       + high half of t into low half of p[1] */
	carry = TOHIGH(carry) + HIHALF(t);

	/* Add low half of t to high half of p[0] */
	t = TOHIGH(t);
	p[0] += t;
	if (p[0] < t)
		carry++;
	p[1] = x1 * y1;
	p[1] += carry;
	return 0;
}


/* spDivide */
#define B (MAX_HALF_DIGIT + 1)
static void spMultSub(DIGIT_T uu[2], DIGIT_T qhat, DIGIT_T v1, DIGIT_T v0)
{
	/*      Compute uu = uu - q(v1v0)
	       where uu = u3u2u1u0, u3 = 0
	       and u_n, v_n are all half-digits
	       even though v1, v2 are passed as full digits.
	*/
	DIGIT_T p0, p1, t;
	p0 = qhat * v0;
	p1 = qhat * v1;
	t = p0 + TOHIGH(LOHALF(p1));
	uu[0] -= t;
	if (uu[0] > MAX_DIGIT - t)
		uu[1]--;	/* Borrow */
	uu[1] -= HIHALF(p1);
}

DIGIT_T spDivide(DIGIT_T *q, DIGIT_T *r, const DIGIT_T u[2], DIGIT_T v)
{
	/*      Computes quotient q = u / v, remainder r = u mod v
				   where u is a double digit
				   and q, v, r are single precision digits.
				   Returns high digit of quotient (max value is 1)
				   CAUTION: Assumes normalised such that v1 >= b/2
				   where b is size of HALF_DIGIT
				   i.e. the most significant bit of v should be one

				   In terms of half-digits in Knuth notation:
				   (q2q1q0) = (u4u3u2u1u0) / (v1v0)
				   (r1r0) = (u4u3u2u1u0) mod (v1v0)
				   for m = 2, n = 2 where u4 = 0
				   q2 is either 0 or 1.
				   We set q = (q1q0) and return q2 as "overflow"
	*/
	DIGIT_T qhat, rhat, t, v0, v1, u0, u1, u2, u3;
	DIGIT_T uu[2], q2;

	/* Check for normalisation */
	if (!(v & HIBITMASK)) {		/* Stop if assert is working, else return error */
		assert(v & HIBITMASK);
		*q = *r = 0;
		return MAX_DIGIT;
	}

	/* Split up into half-digits */
	v0 = LOHALF(v);
	v1 = HIHALF(v);
	u0 = LOHALF(u[0]);
	u1 = HIHALF(u[0]);
	u2 = LOHALF(u[1]);
	u3 = HIHALF(u[1]);

	/* Do three rounds of Knuth Algorithm D Vol 2 p272 */

	/*      ROUND 1. Set j = 2 and calculate q2 */
	/*      Estimate qhat = (u4u3)/v1  = 0 or 1
	       then set (u4u3u2) -= qhat(v1v0)
	       where u4 = 0.
	*/
	/* [Replaced in Version 2] -->
	qhat = u3 / v1;
	if (qhat > 0)
	{
		rhat = u3 - qhat * v1;
		t = TOHIGH(rhat) | u2;
		if (qhat * v0 > t)
			qhat--;
	}
	<-- */
	qhat = (u3 < v1 ? 0 : 1);
	if (qhat > 0) {		/* qhat is one, so no need to mult */
		rhat = u3 - v1;

		/* t = r.b + u2 */
		t = TOHIGH(rhat) | u2;
		if (v0 > t)
			qhat--;
	}
	uu[1] = 0;		/* (u4) */
	uu[0] = u[1];		/* (u3u2) */
	if (qhat > 0) {
		/* (u4u3u2) -= qhat(v1v0) where u4 = 0 */
		spMultSub(uu, qhat, v1, v0);
		if (HIHALF(uu[1]) != 0) {	/* Add back */
			qhat--;
			uu[0] += v;
			uu[1] = 0;
		}
	}
	q2 = qhat;

	/*      ROUND 2. Set j = 1 and calculate q1 */
	/*      Estimate qhat = (u3u2) / v1
	       then set (u3u2u1) -= qhat(v1v0)
	*/
	t = uu[0];
	qhat = t / v1;
	rhat = t - qhat * v1;

	/* Test on v0 */
	t = TOHIGH(rhat) | u1;
	if ((qhat == B) || (qhat * v0 > t)) {
		qhat--;
		rhat += v1;
		t = TOHIGH(rhat) | u1;
		if ((rhat < B) && (qhat * v0 > t))
			qhat--;
		}

	/*      Multiply and subtract
	       (u3u2u1)' = (u3u2u1) - qhat(v1v0)
	*/
	uu[1] = HIHALF(uu[0]);	/* (0u3) */
	uu[0] = TOHIGH(LOHALF(uu[0])) | u1;	/* (u2u1) */
	spMultSub(uu, qhat, v1, v0);
	if (HIHALF(uu[1]) != 0) {		/* Add back */
		qhat--;
		uu[0] += v;
		uu[1] = 0;
	}
	/* q1 = qhat */
	*q = TOHIGH(qhat);

	/* ROUND 3. Set j = 0 and calculate q0 */
	/*      Estimate qhat = (u2u1) / v1
	       then set (u2u1u0) -= qhat(v1v0)
	*/
	t = uu[0];
	qhat = t / v1;
	rhat = t - qhat * v1;

	/* Test on v0 */
	t = TOHIGH(rhat) | u0;
	if ((qhat == B) || (qhat * v0 > t)) {
		qhat--;
		rhat += v1;
		t = TOHIGH(rhat) | u0;
		if ((rhat < B) && (qhat * v0 > t))
			qhat--;
	}

	/*      Multiply and subtract
	       (u2u1u0)" = (u2u1u0)' - qhat(v1v0)
	*/
	uu[1] = HIHALF(uu[0]);	/* (0u2) */
	uu[0] = TOHIGH(LOHALF(uu[0])) | u0;	/* (u1u0) */
	spMultSub(uu, qhat, v1, v0);
	if (HIHALF(uu[1]) != 0) {		/* Add back */
		qhat--;
		uu[0] += v;
		uu[1] = 0;
	}

	/* q0 = qhat */
	*q |= LOHALF(qhat);

	/* Remainder is in (u1u0) i.e. uu[0] */
	*r = uu[0];
	return q2;
}


#endif	/* Conditional single-digit mult & div routines */

/************************/
/* ARITHMETIC FUNCTIONS */
/************************/
DIGIT_T mpAdd(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], size_t ndigits)
{
	/*      Calculates w = u + v
	       where w, u, v are multiprecision integers of ndigits each
	       Returns carry if overflow. Carry = 0 or 1.

	       Ref: Knuth Vol 2 Ch 4.3.1 p 266 Algorithm A.
	*/
	DIGIT_T k;
	size_t j;
	assert(w != v);

	/* Step A1. Initialise */
	k = 0;
	for (j = 0; j < ndigits; j++) {
		/*      Step A2. Add digits w_j = (u_j + v_j + k)
		       Set k = 1 if carry (overflow) occurs
		*/
		w[j] = u[j] + k;
		if (w[j] < k)
			k = 1;
		else
			k = 0;
		w[j] += v[j];
		if (w[j] < v[j])
			k++;
	}		/* Step A3. Loop on j */
	return k;		/* w_n = k */
}

DIGIT_T mpSubtract(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], size_t ndigits)
{
	/*      Calculates w = u - v where u >= v
	       w, u, v are multiprecision integers of ndigits each
	       Returns 0 if OK, or 1 if v > u.

	       Ref: Knuth Vol 2 Ch 4.3.1 p 267 Algorithm S.
	*/
	DIGIT_T k;
	size_t j;
	assert(w != v);

	/* Step S1. Initialise */
	k = 0;
	for (j = 0; j < ndigits; j++) {
		/*      Step S2. Subtract digits w_j = (u_j - v_j - k)
		       Set k = 1 if borrow occurs.
		*/
		w[j] = u[j] - k;
		if (w[j] > MAX_DIGIT - k)
			k = 1;
		else
			k = 0;
		w[j] -= v[j];
		if (w[j] > MAX_DIGIT - v[j])
			k++;
	}		/* Step S3. Loop on j */
	return k;		/* Should be zero if u >= v */
}

int mpMultiply(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], size_t ndigits)
{
	/*      Computes product w = u * v
	       where u, v are multiprecision integers of ndigits each
	       and w is a multiprecision integer of 2*ndigits

	       Ref: Knuth Vol 2 Ch 4.3.1 p 268 Algorithm M.
	*/
	DIGIT_T k, t[2];
	size_t i, j, m, n;
	assert(w != u && w != v);
	m = n = ndigits;

	/* Step M1. Initialise */
	for (i = 0; i < 2 * m; i++)
		w[i] = 0;
	for (j = 0; j < n; j++) {
		/* Step M2. Zero multiplier? */
		if (v[j] == 0) {
			w[j + m] = 0;
		} else {
			/* Step M3. Initialise i */
			k = 0;
			for (i = 0; i < m; i++) {
				/* Step M4. Multiply and add */
				/* t = u_i * v_j + w_(i+j) + k */
				spMultiply(t, u[i], v[j]);
				t[0] += k;
				if (t[0] < k)
					t[1]++;
				t[0] += w[i + j];
				if (t[0] < w[i + j])
					t[1]++;
				w[i + j] = t[0];
				k = t[1];
			}

			/* Step M5. Loop on i, set w_(j+m) = k */
			w[j + m] = k;
		}
	}		/* Step M6. Loop on j */
	return 0;
}

/* mpDivide */
static DIGIT_T mpMultSub(DIGIT_T wn, DIGIT_T w[], const DIGIT_T v[], DIGIT_T q, size_t n)
{
	/*      Compute w = w - qv
	where w = (WnW[n-1]...W[0])
	return modified Wn.
	*/
	DIGIT_T k, t[2];
	size_t i;
	if (q == 0)		/* No change */
		return wn;
	k = 0;
	for (i = 0; i < n; i++) {
		spMultiply(t, q, v[i]);
		w[i] -= k;
		if (w[i] > MAX_DIGIT - k)
			k = 1;
		else
			k = 0;
		w[i] -= t[0];
		if (w[i] > MAX_DIGIT - t[0])
			k++;
		k += t[1];
	}
	/* Cope with Wn not stored in array w[0..n-1] */
	wn -= k;
	return wn;
}

static int QhatTooBig(DIGIT_T qhat, DIGIT_T rhat, DIGIT_T vn2, DIGIT_T ujn2)
{
	/*      Returns true if Qhat is too big
	 i.e. if (Qhat * Vn-2) > (b.Rhat + Uj+n-2)
	*/
	DIGIT_T t[2];
	spMultiply(t, qhat, vn2);
	if (t[1] < rhat)
		return 0;
	else if (t[1] > rhat)
		return 1;
	else if (t[0] > ujn2)
		return 1;
	return 0;
}

int mpDivide(DIGIT_T q[], DIGIT_T r[], const DIGIT_T u[], size_t udigits, DIGIT_T v[], size_t vdigits)
{
	/*      Computes quotient q = u / v and remainder r = u mod v
				   where q, r, u are multiple precision digits
				   all of udigits and the divisor v is vdigits.

				   Ref: Knuth Vol 2 Ch 4.3.1 p 272 Algorithm D.

				   Do without extra storage space, i.e. use r[] for
				   normalised u[], unnormalise v[] at end, and cope with
				   extra digit Uj+n added to u after normalisation.

				   WARNING: this trashes q and r first, so cannot do
				   u = u / v or v = u mod v.
				   It also changes v temporarily so cannot make it const.
	*/
	size_t shift;
	int n, m, j;
	DIGIT_T bitmask, overflow;
	DIGIT_T qhat, rhat, t[2];
	DIGIT_T *uu, *ww;
	int qhatOK, cmp;

	/* Clear q and r */
	mpSetZero(q, udigits);
	mpSetZero(r, udigits);

	/* Work out exact sizes of u and v */
	n = (int)mpSizeof(v, vdigits);
	m = (int)mpSizeof(u, udigits);
	m -= n;

	/* Catch special cases */
	if (n == 0)
		return -1;	/* Error: divide by zero */
	if (n == 1) {		/* Use short division instead */
		r[0] = mpShortDiv(q, u, v[0], udigits);
		return 0;
	}
	if (m < 0) {		/* v > u, so just set q = 0 and r = u */
		mpSetEqual(r, u, udigits);
		return 0;
	}
	if (m == 0) {		/* u and v are the same length */
		cmp = mpCompare(u, v, (size_t) n);
		if (cmp < 0) {	/* v > u, as above */
			mpSetEqual(r, u, udigits);
			return 0;
		} else if (cmp == 0) {	/* v == u, so set q = 1 and r = 0 */
			mpSetDigit(q, 1, udigits);
			return 0;
		}
	}

	/*      In Knuth notation, we have:
	       Given
	       u = (Um+n-1 ... U1U0)
	       v = (Vn-1 ... V1V0)
	       Compute
	       q = u/v = (QmQm-1 ... Q0)
	       r = u mod v = (Rn-1 ... R1R0)
	*/

	/*      Step D1. Normalise */
	/*      Requires high bit of Vn-1
	       to be set, so find most signif. bit then shift left,
	       i.e. d = 2^shift, u' = u * d, v' = v * d.
	 */
	bitmask = HIBITMASK;
	for (shift = 0; shift < BITS_PER_DIGIT; shift++) {
		if (v[n - 1] & bitmask)
			break;
		bitmask >>= 1;
	}

	/* Normalise v in situ - NB only shift non-zero digits */
	overflow = mpShiftLeft(v, v, shift, n);

	/* Copy normalised dividend u*d into r */
	overflow = mpShiftLeft(r, u, shift, n + m);
	uu = r;		/* Use ptr to keep notation constant */
	t[0] = overflow;	/* Extra digit Um+n */

	/* Step D2. Initialise j. Set j = m */
	for (j = m; j >= 0; j--) {
		/* Step D3. Set Qhat = [(b.Uj+n + Uj+n-1)/Vn-1]
		       and Rhat = remainder */
		qhatOK = 0;
		t[1] = t[0];	/* This is Uj+n */
		t[0] = uu[j + n - 1];
		overflow = spDivide(&qhat, &rhat, t, v[n - 1]);

		/* Test Qhat */
		if (overflow) {	/* Qhat == b so set Qhat = b - 1 */
			qhat = MAX_DIGIT;
			rhat = uu[j + n - 1];
			rhat += v[n - 1];
			if (rhat < v[n - 1])	/* Rhat >= b, so no re-test */
				qhatOK = 1;
		}

		/* [VERSION 2: Added extra test "qhat && "] */
		if (qhat && !qhatOK && QhatTooBig(qhat, rhat, v[n - 2], uu[j + n - 2])) {
			/* If Qhat.Vn-2 > b.Rhat + Uj+n-2
				   decrease Qhat by one, increase Rhat by Vn-1
			*/
			qhat--;
			rhat += v[n - 1];

			/* Repeat this test if Rhat < b */
			if (!(rhat < v[n - 1]))
			if (QhatTooBig(qhat, rhat, v[n - 2], uu[j + n - 2]))
				qhat--;
		}

		/* Step D4. Multiply and subtract */
		ww = &uu[j];
		overflow = mpMultSub(t[1], ww, v, qhat, (size_t) n);

		/* Step D5. Test remainder. Set Qj = Qhat */
		q[j] = qhat;
		if (overflow) {	/* Step D6. Add back if D4 was negative */
			q[j]--;
			//overflow = mpAdd(ww, ww, v, (size_t) n);
		}
		t[0] = uu[j + n - 1];	/* Uj+n on next round */
	}		/* Step D7. Loop on j */

	/* Clear high digits in uu */
	for (j = n; j < m + n; j++)
		uu[j] = 0;

	/* Step D8. Unnormalise. */
	mpShiftRight(r, r, shift, n);
	mpShiftRight(v, v, shift, n);
	return 0;
}

int mpSquare(DIGIT_T w[], const DIGIT_T x[], size_t ndigits)
/* New in Version 2.0 */
{
	/*      Computes square w = x * x
	       where x is a multiprecision integer of ndigits
	       and w is a multiprecision integer of 2*ndigits

	       Ref: Menezes p596 Algorithm 14.16 with errata.
	*/
	DIGIT_T k, p[2], u[2], cbit, carry;
	size_t i, j, t, i2, cpos;
	assert(w != x);
	t = ndigits;

	/* 1. For i from 0 to (2t-1) do: w_i = 0 */
	i2 = t << 1;
	for (i = 0; i < i2; i++)
		w[i] = 0;
	carry = 0;
	cpos = i2 - 1;

	/* 2. For i from 0 to (t-1) do: */
	for (i = 0; i < t; i++) {

		/* 2.1 (uv) = w_2i + x_i * x_i, w_2i = v, c = u
		       Careful, w_2i may be double-prec
		*/
		i2 = i << 1;	/* 2*i */
		spMultiply(p, x[i], x[i]);
		p[0] += w[i2];
		if (p[0] < w[i2])
			p[1]++;
		k = 0;		/* p[1] < b, so no overflow here */
		if (i2 == cpos && carry) {
			p[1] += carry;
			if (p[1] < carry)
				k++;
			carry = 0;
		}
		w[i2] = p[0];
		u[0] = p[1];
		u[1] = k;

		/* 2.2 for j from (i+1) to (t-1) do:
		       (uv) = w_{i+j} + 2x_j * x_i + c,
		       w_{i+j} = v, c = u,
		       u is double-prec
		       w_{i+j} is dbl if [i+j] == cpos
		*/
		k = 0;
		for (j = i + 1; j < t; j++) {
			/* p = x_j * x_i */
			spMultiply(p, x[j], x[i]);

			/* p = 2p <=> p <<= 1 */
			cbit = (p[0] & HIBITMASK) != 0;
			k = (p[1] & HIBITMASK) != 0;
			p[0] <<= 1;
			p[1] <<= 1;
			p[1] |= cbit;

			/* p = p + c */
			p[0] += u[0];
			if (p[0] < u[0]) {
				p[1]++;
				if (p[1] == 0)
					k++;
			}
			p[1] += u[1];
			if (p[1] < u[1])
				k++;

			/* p = p + w_{i+j} */
			p[0] += w[i + j];
			if (p[0] < w[i + j]) {
				p[1]++;
				if (p[1] == 0)
					k++;
			}
			if ((i + j) == cpos && carry) {	/* catch overflow from last round */
				p[1] += carry;
				if (p[1] < carry)
					k++;
				carry = 0;
			}

			/* w_{i+j} = v, c = u */
			w[i + j] = p[0];
			u[0] = p[1];
			u[1] = k;
		}

		/* 2.3 w_{i+t} = u */
		w[i + t] = u[0];

		/* remember overflow in w_{i+t} */
		carry = u[1];
		cpos = i + t;
	}

	/* (NB original step 3 deleted in Menezes errata) */

	/* Return w */
	return 0;
}

int mpEqual(const DIGIT_T a[], const DIGIT_T b[], size_t ndigits)
{
	/*      Returns true if a == b, else false*/
	if (ndigits == 0)
		return -1;
	while (ndigits--) {
		if (a[ndigits] != b[ndigits])
		return 0;	/* False */
	}
	return !0;		/* True */
}

int mpCompare(const DIGIT_T a[], const DIGIT_T b[], size_t ndigits)
{
	/*      Returns sign of (a - b)*/
	if (ndigits == 0)
		return 0;
	while (ndigits--) {
		if (a[ndigits] > b[ndigits])
			return 1;	/* GT */
		if (a[ndigits] < b[ndigits])
			return -1;	/* LT */
	}
	return 0;		/* EQ */
}

int mpIsZero(const DIGIT_T a[], size_t ndigits)
{
	/*      Returns true if a == 0, else false*/
	size_t i;
	if (ndigits == 0)
		return -1;
	for (i = 0; i < ndigits; i++) {	/* Start at lsb */
		if (a[i] != 0)
		return 0;	/* False */
	}
	return !0;		/* True */
}

size_t mpSizeof(const DIGIT_T a[], size_t ndigits)
{
	/* Returns size of significant digits in a */
	while (ndigits--) {
		if (a[ndigits] != 0)
			return ++ndigits;
	}
	return 0;
}

size_t mpBitLength(const DIGIT_T d[], size_t ndigits)
	/* Returns no of significant bits in d */
{
	size_t n, i, bits;
	DIGIT_T mask;
	if (!d || ndigits == 0)
		return 0;
	n = mpSizeof(d, ndigits);
	if (0 == n)
		return 0;
	for (i = 0, mask = HIBITMASK; mask > 0; mask >>= 1, i++) {
		if (d[n - 1] & mask)
			break;
	}
	bits = n * BITS_PER_DIGIT - i;
	return bits;
}

void mpSetEqual(DIGIT_T a[], const DIGIT_T b[], size_t ndigits)
{
	/* Sets a = b */
	size_t i;
	for (i = 0; i < ndigits; i++) {
		a[i] = b[i];
	}
}

volatile DIGIT_T mpSetZero(volatile DIGIT_T a[], size_t ndigits)
{
	/* Sets a = 0 */
	/* Prevent optimiser ignoring this */
	volatile DIGIT_T optdummy;
	volatile DIGIT_T *p = a;
	while (ndigits--)
		a[ndigits] = 0;
	optdummy = *p;
	return optdummy;
}

void mpSetDigit(DIGIT_T a[], DIGIT_T d, size_t ndigits)
{
	/* Sets a = d where d is a single digit */
	size_t i;
	for (i = 1; i < ndigits; i++) {
		a[i] = 0;
	}
	a[0] = d;
}

/**********************/
/* BIT-WISE FUNCTIONS */
/**********************/
DIGIT_T mpShiftLeft(DIGIT_T a[], const DIGIT_T *b, size_t shift, size_t ndigits)
{
	/* Computes a = b << shift */

	/* [v2.1] Modified to cope with shift > BITS_PERDIGIT */
	size_t i, y, nw, bits;
	DIGIT_T mask, carry, nextcarry;

	/* Do we shift whole digits? */
	if (shift >= BITS_PER_DIGIT) {
		nw = shift / BITS_PER_DIGIT;
		i = ndigits;
		while (i--) {
			if (i >= nw)
				a[i] = b[i - nw];
			else
				a[i] = 0;
		}

		/* Call again to shift bits inside digits */
		bits = shift % BITS_PER_DIGIT;
		carry = b[ndigits - nw] << bits;
		if (bits)
			carry |= mpShiftLeft(a, a, bits, ndigits);
		return carry;
	} else {
		bits = shift;
	}

	/* Construct mask = high bits set */
	mask = ~(~(DIGIT_T) 0 >> bits);
	y = BITS_PER_DIGIT - bits;
	carry = 0;
	for (i = 0; i < ndigits; i++) {
		nextcarry = (b[i] & mask) >> y;
		a[i] = b[i] << bits | carry;
		carry = nextcarry;
	}
	return carry;
}

DIGIT_T mpShiftRight(DIGIT_T a[], const DIGIT_T b[], size_t shift, size_t ndigits)
{
	/* Computes a = b >> shift */

	/* [v2.1] Modified to cope with shift > BITS_PERDIGIT */
	size_t i, y, nw, bits;
	DIGIT_T mask, carry, nextcarry;

	/* Do we shift whole digits? */
	if (shift >= BITS_PER_DIGIT) {
		nw = shift / BITS_PER_DIGIT;
		for (i = 0; i < ndigits; i++) {
			if ((i + nw) < ndigits)
				a[i] = b[i + nw];
			else
				a[i] = 0;
		}

		/* Call again to shift bits inside digits */
		bits = shift % BITS_PER_DIGIT;
		carry = b[nw - 1] >> bits;
		if (bits)
			carry |= mpShiftRight(a, a, bits, ndigits);
		return carry;
		} else {
			bits = shift;
		}

	/* Construct mask to set low bits */
	/* (thanks to Jesse Chisholm for suggesting this improved technique) */
	mask = ~(~(DIGIT_T) 0 << bits);
	y = BITS_PER_DIGIT - bits;
	carry = 0;
	i = ndigits;
	while (i--) {
		if (y < BITS_PER_DIGIT)
			nextcarry = (b[i] & mask) << y;
		else
			nextcarry = 0;
		a[i] = b[i] >> bits | carry;
		carry = nextcarry;
	}
	return carry;
}

int mpSetBit(DIGIT_T a[], size_t ndigits, size_t ibit, int value)
	/* Set bit n (0..nbits-1) with value 1 or 0 */
{
	size_t idigit, bit_to_set;
	DIGIT_T mask;

	/* Which digit? (0-based) */
	idigit = ibit / BITS_PER_DIGIT;
	if (idigit >= ndigits)
		return -1;

	/* Set mask */
	bit_to_set = ibit % BITS_PER_DIGIT;
	mask = 0x01 << bit_to_set;
	if (value)
		a[idigit] |= mask;
	else
		a[idigit] &= (~mask);
	return 0;
}

int mpGetBit(DIGIT_T a[], size_t ndigits, size_t ibit)
	/* Returns value 1 or 0 of bit n (0..nbits-1); or -1 if out of range */
{
	size_t idigit, bit_to_get;
	DIGIT_T mask;

	/* Which digit? (0-based) */
	idigit = ibit / BITS_PER_DIGIT;
	if (idigit >= ndigits)
		return -1;

	/* Set mask */
	bit_to_get = ibit % BITS_PER_DIGIT;
	mask = 0x01 << bit_to_get;
	return (a[idigit] & mask) ? 1 : 0;
}

void mpModPowerOf2(DIGIT_T a[], size_t ndigits, size_t L)
	/* Computes a = a mod 2^L */
	/* i.e. clears all bits >= L */
{
	size_t i, nw, bits;
	DIGIT_T mask;

	/* High digits to clear */
	nw = L / BITS_PER_DIGIT;
	for (i = nw + 1; i < ndigits; i++)
		a[i] = 0;

	/* Low bits to keep */
	bits = L % BITS_PER_DIGIT;
	mask = ~(~0 << bits);
	if (ndigits > nw)
		a[nw] &= mask;
}

void mpXorBits(DIGIT_T a[], const DIGIT_T b[], const DIGIT_T c[], size_t ndigits)
	/* Computes bitwise a = b XOR c */
{
	size_t i;
	for (i = 0; i < ndigits; i++)
		a[i] = b[i] ^ c[i];
}

void mpOrBits(DIGIT_T a[], const DIGIT_T b[], const DIGIT_T c[], size_t ndigits)
	/* Computes bitwise a = b OR c */
{
	size_t i;
	for (i = 0; i < ndigits; i++)
		a[i] = b[i] | c[i];
}

void mpAndBits(DIGIT_T a[], const DIGIT_T b[], const DIGIT_T c[], size_t ndigits)
	/* Computes bitwise a = b AND c */
{
	size_t i;
	for (i = 0; i < ndigits; i++)
		a[i] = b[i] & c[i];
}

void mpNotBits(DIGIT_T a[], const DIGIT_T b[], size_t ndigits)
	/* Computes bitwise a = NOT b */
{
	size_t i;
	for (i = 0; i < ndigits; i++)
		a[i] = ~b[i];
}


/*****************************************/
/* FUNCTIONS WITH A SINGLE (SHORT) DIGIT */
/*****************************************/
DIGIT_T mpShortAdd(DIGIT_T w[], const DIGIT_T u[], DIGIT_T v, size_t ndigits)
{
	/*      Calculates w = u + v
	       where w, u are multiprecision integers of ndigits each
	       and v is a single precision digit.
	       Returns carry if overflow.

	       Ref: Derived from Knuth Algorithm A.
	*/
	DIGIT_T k;
	size_t j;
	k = 0;

	/* Add v to first digit of u */
	w[0] = u[0] + v;
	if (w[0] < v)
		k = 1;
	else
		k = 0;
	/* Add carry to subsequent digits */
	for (j = 1; j < ndigits; j++) {
		w[j] = u[j] + k;
		if (w[j] < k)
			k = 1;
		else
			k = 0;
	}
	return k;
}

DIGIT_T mpShortSub(DIGIT_T w[], const DIGIT_T u[], DIGIT_T v, size_t ndigits)
{
	/*      Calculates w = u - v
	       where w, u are multiprecision integers of ndigits each
	       and v is a single precision digit.
	       Returns borrow: 0 if u >= v, or 1 if v > u.

	       Ref: Derived from Knuth Algorithm S.
	*/
	DIGIT_T k;
	size_t j;
	k = 0;

	/* Subtract v from first digit of u */
	w[0] = u[0] - v;
	if (w[0] > MAX_DIGIT - v)
		k = 1;
	else
		k = 0;

	/* Subtract borrow from subsequent digits */
	for (j = 1; j < ndigits; j++) {
		w[j] = u[j] - k;
		if (w[j] > MAX_DIGIT - k)
			k = 1;
		else
			k = 0;
	}
	return k;
}

DIGIT_T mpShortMult(DIGIT_T w[], const DIGIT_T u[], DIGIT_T v, size_t ndigits)
{
	/*      Computes product w = u * v
	       Returns overflow k
	       where w, u are multiprecision integers of ndigits each
	       and v, k are single precision digits

	       Ref: Knuth Algorithm M.
	*/
	DIGIT_T k, t[2];
	size_t j;
	if (v == 0) {		/* [2005-08-29] Set w = 0 */
		for (j = 0; j < ndigits; j++)
			w[j] = 0;
		return 0;
	}
	k = 0;
	for (j = 0; j < ndigits; j++) {
		    /* t = x_i * v */
		spMultiply(t, u[j], v);

		    /* w_i = LOHALF(t) + carry */
		w[j] = t[0] + k;

		    /* Overflow? */
		 if (w[j] < k)
			t[1]++;

		    /* Carry forward HIHALF(t) */
		 k = t[1];
	}
	return k;
}

DIGIT_T mpShortDiv(DIGIT_T q[], const DIGIT_T u[], DIGIT_T v, size_t ndigits)
{
	    /*      Calculates quotient q = u div v
	       Returns remainder r = u mod v
	       where q, u are multiprecision integers of ndigits each
	       and r, v are single precision digits.

	       Makes no assumptions about normalisation.

	       Ref: Knuth Vol 2 Ch 4.3.1 Exercise 16 p625
	     */
	size_t j;
	DIGIT_T t[2], r;
	size_t shift;
	DIGIT_T bitmask, overflow, *uu;
	if (ndigits == 0)
		return 0;
	if (v == 0)
		return 0;	/* Divide by zero error */

	    /*      Normalise first */
	    /*      Requires high bit of V
	       to be set, so find most signif. bit then shift left,
	       i.e. d = 2^shift, u' = u * d, v' = v * d.
	     */
	bitmask = HIBITMASK;
	for (shift = 0; shift < BITS_PER_DIGIT; shift++) {
		if (v & bitmask)
			break;
		bitmask >>= 1;
	}
	if(shift < 32)
		v <<= shift;
	else
		v = 0;
	overflow = mpShiftLeft(q, u, shift, ndigits);
	uu = q;

	    /* Step S1 - modified for extra digit. */
	r = overflow;	/* New digit Un */
	j = ndigits;
	while (j--) {
		    /* Step S2. */
		t[1] = r;
		t[0] = uu[j];
		overflow = spDivide(&q[j], &r, t, v);
	}

	    /* Unnormalise */
	if(shift < 32)
		r >>= shift;
	else
		r = 0;
	return r;
}

DIGIT_T mpShortMod(const DIGIT_T a[], DIGIT_T d, size_t ndigits)
{
	    /*      Calculates r = a mod d
	       where a is a multiprecision integer of ndigits
	       and r, d are single precision digits.
	       Use remainder from divide function.
	     */
	DIGIT_T r = 0;

/* Allocate temp storage */
#ifdef NO_ALLOCS
	DIGIT_T q[MAX_FIXED_DIGITS * 2];
	assert(ndigits <= MAX_FIXED_DIGITS);

#else
	DIGIT_T *q;
	q = mpAlloc(ndigits * 2);

#endif
	r = mpShortDiv(q, a, d, ndigits);
	mpDESTROY(q, ndigits);
	return r;
}

int mpShortCmp(const DIGIT_T a[], DIGIT_T b, size_t ndigits)
{

	    /* Returns sign of (a - b) where b is a single digit */
	size_t i;

	    /* Changed in Ver 2.0 to cope with zero-length a */
	if (ndigits == 0)
		return b ? -1 : 0;
	for (i = 1; i < ndigits; i++) {
		if (a[i] != 0)
			return 1;	/* GT */
	}
	if (a[0] < b)
		return -1;	/* LT */

	else if (a[0] > b)
		return 1;	/* GT */
	return 0;		/* EQ */
}

/*********************************/
/* FUNCTIONS FOR SIGNED INTEGERS */
/*********************************/
/* Added [v2.2] */
int mpIsNegative(const DIGIT_T x[], size_t ndigits)
	/* Returns TRUE (1) if x < 0, else FALSE (0) */
{
	return ((x[ndigits - 1] & HIBITMASK) != 0);
}

int mpChs(DIGIT_T x[], const DIGIT_T y[], size_t ndigits)
	/* Sets x = -y */
{
	int isneg = mpIsNegative(y, ndigits);
	if (isneg) {		/* negative to positive: x = ~(y-1) */
		mpShortSub(x, y, 1, ndigits);
		mpNotBits(x, x, ndigits);
	} else {		/* positive to negative: x = ~(y)+1 */
		mpNotBits(x, x, ndigits);
		mpShortAdd(x, y, 1, ndigits);
	}
	return isneg;
}

int mpAbs(DIGIT_T x[], const DIGIT_T y[], size_t ndigits)
	/* Sets x = |y| */
{
	int isneg = mpIsNegative(y, ndigits);
	if (isneg)
		mpChs(x, y, ndigits);
	else
		mpSetEqual(x, y, ndigits);
	return isneg;
}


/*******************/
/* PRINT FUNCTIONS */
/*******************/
/* [v2.1] changed to use C99 format types */
void mpPrint(const DIGIT_T *p, size_t len)
{
	while (len--) {
		H2DBGLOG((LOCALDBG, "%08x ", p[len]));
	}
}

void mpPrintNL(const DIGIT_T *p, size_t len)
{
	size_t i = 0;
	while (len--) {
		if ((i % 8) == 0 && i)
			H2DBGLOG((LOCALDBG, "\n"));
		H2DBGLOG((LOCALDBG, "%08x ", p[len]));
		i++;
	}
	H2DBGLOG((LOCALDBG, "\n"));
}

void mpPrintTrim(const DIGIT_T *p, size_t len)
{
	/* Trim leading zeroes */
	while (len--) {
		if (p[len] != 0)
			break;
	}
	len++;

	/* Catch empty len to show 0 */
	if (0 == len)
		len = 1;
	mpPrint(p, len);
}

void mpPrintTrimNL(const DIGIT_T *p, size_t len)
{
	 /* Trim leading zeroes */
	while (len--) {
		if (p[len] != 0)
			break;
	}
	len++;

	 /* Catch empty len to show 0 */
	if (0 == len)
		len = 1;
	mpPrintNL(p, len);
}


/************************/
/* CONVERSION FUNCTIONS */
/************************/
size_t mpConvFromOctets(DIGIT_T a[], size_t ndigits, const unsigned char *c, size_t nbytes)
/* Converts nbytes octets into big digit a of max size ndigits
   Returns actual number of digits set (may be larger than mpSizeof)
*/
{
	size_t i;
	int j, k;
	DIGIT_T t;
	mpSetZero(a, ndigits);

	    /* Read in octets, least significant first */
	    /* i counts into big_d, j along c, and k is # bits to shift */
	for (i = 0, j = (int)nbytes - 1; i < ndigits && j >= 0; i++) {
		t = 0;
		for (k = 0; j >= 0 && k < BITS_PER_DIGIT; j--, k += 8)
			t |= ((DIGIT_T) c[j]) << k;
		a[i] = t;
	}
	return i;
}

size_t mpConvToOctets(const DIGIT_T a[], size_t ndigits, unsigned char *c, size_t nbytes)
/* Convert big digit a into string of octets, in big-endian order,
   padding on the left to nbytes or truncating if necessary.
   Return number of octets required excluding leading zero bytes.
*/
{
	int j, k, len;
	DIGIT_T t;
	size_t i, noctets, nbits;
	nbits = mpBitLength(a, ndigits);
	noctets = (nbits + 7) / 8;
	len = (int)nbytes;
	for (i = 0, j = len - 1; i < ndigits && j >= 0; i++) {
		t = a[i];
		for (k = 0; j >= 0 && k < BITS_PER_DIGIT; j--, k += 8)
			c[j] = (unsigned char)(t >> k);
	}
	for (; j >= 0; j--)
		c[j] = 0;
	return (size_t) noctets;
}

/***************************/
/* NUMBER THEORY FUNCTIONS */
/***************************/
int mpModulo(DIGIT_T r[], const DIGIT_T u[], size_t udigits, DIGIT_T v[], size_t vdigits)
{

	    /*      Computes r = u mod v
	       where r, v are multiprecision integers of length vdigits
	       and u is a multiprecision integer of length udigits.
	       r may overlap v.

	       Note that r here is only vdigits long,
	       whereas in mpDivide it is udigits long.

	       Use remainder from mpDivide function.
	     */
	size_t nn = max(udigits, vdigits);

/* Allocate temp storage */
#ifdef NO_ALLOCS
	DIGIT_T qq[MAX_FIXED_DIGITS];
	DIGIT_T rr[MAX_FIXED_DIGITS];
	assert(nn <= MAX_FIXED_DIGITS);
	udigits = MAX_FIXED_DIGITS;

#else
	DIGIT_T *qq, *rr;
	qq = mpAlloc(udigits);
	rr = mpAlloc(nn);

#endif
	/* rr[nn] = u mod v */
	mpDivide(qq, rr, u, udigits, v, vdigits);

	/* Final r is only vdigits long */
	mpSetEqual(r, rr, vdigits);
	mpDESTROY(rr, udigits);
	mpDESTROY(qq, udigits);
	return 0;
}

int mpModMult(DIGIT_T a[], const DIGIT_T x[], const DIGIT_T y[], DIGIT_T m[], size_t ndigits)
{				/*      Computes a = (x * y) mod m */
/* Double-length temp variable p */
#ifdef NO_ALLOCS
	DIGIT_T p[MAX_FIXED_DIGITS * 2];
	assert(ndigits <= MAX_FIXED_DIGITS);

#else
	DIGIT_T *p;
	p = mpAlloc(ndigits * 2);

#endif
	    /* Calc p[2n] = x * y */
	mpMultiply(p, x, y, ndigits);

	    /* Then modulo (NOTE: a is OK at only ndigits long) */
	mpModulo(a, p, ndigits * 2, m, ndigits);
	mpDESTROY(p, ndigits * 2);
	return 0;
}

int mpModInv(DIGIT_T inv[], const DIGIT_T u[], const DIGIT_T v[], size_t ndigits)
{
	/*      Computes inv = u^(-1) mod v */

	    /*      Ref: Knuth Algorithm X Vol 2 p 342
	       ignoring u2, v2, t2
	       and avoiding negative numbers.
	       Returns non-zero if inverse undefined.
	     */
	int bIterations;
	int result;

/* Allocate temp storage */
#ifdef NO_ALLOCS
	DIGIT_T u1[MAX_FIXED_DIGITS];
	DIGIT_T u3[MAX_FIXED_DIGITS];
	DIGIT_T v1[MAX_FIXED_DIGITS];
	DIGIT_T v3[MAX_FIXED_DIGITS];
	DIGIT_T t1[MAX_FIXED_DIGITS];
	DIGIT_T t3[MAX_FIXED_DIGITS];
	DIGIT_T q[MAX_FIXED_DIGITS];
	DIGIT_T w[2 * MAX_FIXED_DIGITS];
	assert(ndigits <= MAX_FIXED_DIGITS);
#else
	DIGIT_T *u1, *u3, *v1, *v3, *t1, *t3, *q, *w;
	u1 = mpAlloc(ndigits);
	u3 = mpAlloc(ndigits);
	v1 = mpAlloc(ndigits);
	v3 = mpAlloc(ndigits);
	t1 = mpAlloc(ndigits);
	t3 = mpAlloc(ndigits);
	q = mpAlloc(ndigits);
	w = mpAlloc(2 * ndigits);

#endif
	    /* Step X1. Initialise */
	mpSetDigit(u1, 1, ndigits);	/* u1 = 1 */
	mpSetEqual(u3, u, ndigits);	/* u3 = u */
	mpSetZero(v1, ndigits);	/* v1 = 0 */
	mpSetEqual(v3, v, ndigits);	/* v3 = v */
	bIterations = 1;	/* Remember odd/even iterations */
	while (!mpIsZero(v3, ndigits)) {		/* Step X3. Divide and "Subtract" */
		mpDivide(q, t3, u3, ndigits, v3, ndigits);

		    /* q = u3 / v3, t3 = u3 % v3 */
		mpMultiply(w, q, v1, ndigits);	/* w = q * v1 */
		mpAdd(t1, u1, w, ndigits);	/* t1 = u1 + w */

		    /* Swap u1 = v1; v1 = t1; u3 = v3; v3 = t3 */
		mpSetEqual(u1, v1, ndigits);
		mpSetEqual(v1, t1, ndigits);
		mpSetEqual(u3, v3, ndigits);
		mpSetEqual(v3, t3, ndigits);
		bIterations = -bIterations;
	}
	if (bIterations < 0)
		mpSubtract(inv, v, u1, ndigits);	/* inv = v - u1 */
	else
		mpSetEqual(inv, u1, ndigits);	/* inv = u1 */

	    /* Make sure u3 = gcd(u,v) == 1 */
	if (mpShortCmp(u3, 1, ndigits) != 0) {
		result = 1;
		mpSetZero(inv, ndigits);
	} else
		result = 0;

	    /* Clear up */
	mpDESTROY(u1, ndigits);
	mpDESTROY(v1, ndigits);
	mpDESTROY(t1, ndigits);
	mpDESTROY(u3, ndigits);
	mpDESTROY(v3, ndigits);
	mpDESTROY(t3, ndigits);
	mpDESTROY(q, ndigits);
	mpDESTROY(w, 2 * ndigits);
	return result;
}

int mpGcd(DIGIT_T g[], const DIGIT_T x[], const DIGIT_T y[], size_t ndigits)
{

	    /* Computes g = gcd(x, y) */
	    /* Ref: Schneier  */

/*	Allocate temp storage */
#ifdef NO_ALLOCS
	DIGIT_T yy[MAX_FIXED_DIGITS];
	DIGIT_T xx[MAX_FIXED_DIGITS];
	assert(ndigits <= MAX_FIXED_DIGITS);

#else
	DIGIT_T *yy, *xx;
	yy = mpAlloc(ndigits);
	xx = mpAlloc(ndigits);

#endif
	mpSetZero(yy, ndigits);
	mpSetZero(xx, ndigits);
	mpSetEqual(xx, x, ndigits);
	mpSetEqual(yy, y, ndigits);
	mpSetEqual(g, yy, ndigits);	/* g = y */
	while (!mpIsZero(xx, ndigits)) {/* while (x > 0) */
		mpSetEqual(g, xx, ndigits);	/* g = x */
		mpModulo(xx, yy, ndigits, xx, ndigits);	/* x = y mod x */
		mpSetEqual(yy, g, ndigits);	/* y = g; */
	}
	mpDESTROY(xx, ndigits);
	mpDESTROY(yy, ndigits);
	return 0;		/* gcd is in g */
}

int mpSqrt(DIGIT_T s[], const DIGIT_T x[], size_t ndigits)
	/* Computes integer square root s = floor(sqrt(x)) */
	/* [Added v2.1] Based on lsqrt() function */
{

/*	Allocate temp storage */
#ifdef NO_ALLOCS
	DIGIT_T v0[MAX_FIXED_DIGITS];
	DIGIT_T q0[MAX_FIXED_DIGITS];
	DIGIT_T x0[MAX_FIXED_DIGITS];
	DIGIT_T x1[MAX_FIXED_DIGITS];
	DIGIT_T t[MAX_FIXED_DIGITS];
	assert(ndigits <= MAX_FIXED_DIGITS);
#else
	DIGIT_T *v0, *q0, *x0, *x1, *t;
	v0 = mpAlloc(ndigits);
	q0 = mpAlloc(ndigits);
	x0 = mpAlloc(ndigits);
	x1 = mpAlloc(ndigits);
	t = mpAlloc(ndigits);
#endif
	    /* if (x <= 1) return x */
	if (mpShortCmp(x, 1, ndigits) <= 0) {
		mpSetEqual(s, x, ndigits);
		goto done;
	}

	    /* v0 = x - 1 */
	mpShortSub(v0, x, 1, ndigits);

	    /* x0 = x/2 */
	mpShortDiv(x0, x, 2, ndigits);
	while (1) {
		    /* q0 = v0/x0 */
		mpDivide(q0, t, v0, ndigits, x0, ndigits);

		    /* x1 = (x0 + q0)/2 */
		mpAdd(t, x0, q0, ndigits);
		mpShortDiv(x1, t, 2, ndigits);

		    /* if (q0 >= x0) break */
		if (mpCompare(q0, x0, ndigits) >= 0)
			break;

		    /* x0 = x1 */
		mpSetEqual(x0, x1, ndigits);
	}

	    /* return x1 */
	mpSetEqual(s, x1, ndigits);
done:
	mpDESTROY(v0, ndigits);
	mpDESTROY(q0, ndigits);
	mpDESTROY(x0, ndigits);
	mpDESTROY(x1, ndigits);
	mpDESTROY(t, ndigits);
	return 0;
}

int mpJacobi(const DIGIT_T a[], const DIGIT_T n[], size_t ndigits)
	/* Returns Jacobi(a, n) = {0, +1, -1} */
	/* [Added v2.2] */
{

/* Ref: Menezes.
Algorithm 2.149 Jacobi symbol (and Legendre symbol) computation
JACOBI(a,n)
INPUT: an odd integer n >= 3, and an integer a, 0 <= a < n.
OUTPUT: the Jacobi symbol (a/n) (and hence the Legendre symbol when n is prime).
1. If a = 0 then return(0).
2. If a = 1 then return(1).
3. Write a = 2^e.a_1, where a_1 is odd.
4. If e is even then set s <-- 1. Otherwise set s <-- 1 if n \equiv 1 or 7 (mod 8),
or set s <-- -1 if n \equiv 3 or 5 (mod 8).
5. If n \equiv 3 (mod 4) and a1 \equiv 3 (mod 4) then set s <-- -s.
6. Set n1 <-- n mod a1.
7. If a1 = 1 then return(s); otherwise return(s * JACOBI(n1,a1)).
*/
	int s;
	DIGIT_T nmod8;
	unsigned e;

/*	Allocate temp storage */
#ifdef NO_ALLOCS
	DIGIT_T a1[MAX_FIXED_DIGITS];
	DIGIT_T n1[MAX_FIXED_DIGITS];
	assert(ndigits <= MAX_FIXED_DIGITS);
#else
	DIGIT_T *a1, *n1;
	a1 = mpAlloc(ndigits);
	n1 = mpAlloc(ndigits);
#endif
	    /* 1. If a = 0 then return(0). */
	if (mpIsZero(a, ndigits)) {
		s = 0;
		goto done;
	}

	    /* 2. If a = 1 then return(1). */
	if (mpShortCmp(a, 1, ndigits) == 0) {
		s = 1;
		goto done;
	}

	    /* 3. Write a = 2^e.a_1, where a_1 is odd. */
	mpSetEqual(a1, a, ndigits);
	for (e = 0; mpISEVEN(a1, ndigits); e++) {
		mpShiftRight(a1, a1, 1, ndigits);
	}

	    /* 4.
	       If e is even then set s <-- 1. Otherwise set s <-- 1 if n \equiv 1 or 7 (mod 8),
	       or set s <-- -1 if n \equiv 3 or 5 (mod 8). */
	if (ISEVEN(e))
		s = 1;
	else {
		nmod8 = mpShortMod(n, 8, ndigits);
		if (nmod8 == 1 || nmod8 == 7)
			s = 1;
		else
			s = -1;
	}

	    /* 5. If n \equiv 3 (mod 4) and a1 \equiv 3 (mod 4) then set s <-- -s. */
	if (mpShortMod(n, 4, ndigits) == 3 && mpShortMod(a1, 4, ndigits) == 3)
		s = -s;

	    /*
	       6. Set n1 <-- n mod a1.
	       7. If a1 = 1 then return(s); otherwise return(s * JACOBI(n1,a1)).
	     */
	if (mpShortCmp(a1, 1, ndigits) != 0) {
		mpModulo(n1, n, ndigits, a1, ndigits);
		s = s * mpJacobi(n1, a1, ndigits);
	}
done:
	mpDESTROY(a1, ndigits);
	mpDESTROY(n1, ndigits);
	return s;
}

/**************************/
/* MODULAR EXPONENTIATION */
/**************************/
/*	[v2.2] Modified to use sliding-window exponentiation.
	mpModExp_1 is the earlier version [<2.2] now using macros for modular squaring & mult
*/

#ifdef NO_ALLOCS
static int mpModExp_1(DIGIT_T y[], const DIGIT_T x[], const DIGIT_T n[], DIGIT_T d[], size_t ndigits);

#else
static int mpModExp_windowed(DIGIT_T y[], const DIGIT_T x[], const DIGIT_T n[], DIGIT_T d[], size_t ndigits);

#endif
int mpModExp(DIGIT_T y[], const DIGIT_T x[], const DIGIT_T n[], DIGIT_T d[], size_t ndigits)
	/* Computes y = x^n mod d */
{
#ifdef NO_ALLOCS
	return mpModExp_1(y, x, n, d, ndigits);
#else
	return mpModExp_windowed(y, x, n, d, ndigits);
#endif
}

/* MACROS TO DO MODULAR SQUARING AND MULTIPLICATION USING PRE-ALLOCATED TEMPS */
/* Required lengths |y|=|t1|=|t2|=2*n, |m|=n; but final |y|=n */
/* Square: y = (y * y) mod m */
#define mpMODSQUARETEMP(y, m, n, t1, t2) do {mpSquare(t1, y, n); mpDivide(t2, y, t1, n * 2, m, n); } while (0)
/* Mult:   y = (y * x) mod m */
#define mpMODMULTTEMP(y, x, m, n, t1, t2) do {mpMultiply(t1, x, y, n); mpDivide(t2, y, t1, n * 2, m, n); } while (0)
static int mpModExp_1(DIGIT_T yout[], const DIGIT_T x[], const DIGIT_T e[], DIGIT_T m[], size_t ndigits)
{				/*      Computes y = x^e mod m */

	    /*      Binary left-to-right method */
	    /*  [v2.2] removed const restriction on m[] to avoid using an extra alloc'd var
	       (m is changed in-situ during the divide operation then restored) */
	DIGIT_T mask;
	size_t n;
	size_t nn = ndigits * 2;

	    /* Create some double-length temps */
#ifdef NO_ALLOCS
	DIGIT_T t1[MAX_FIXED_DIGITS * 2];
	DIGIT_T t2[MAX_FIXED_DIGITS * 2];
	DIGIT_T y[MAX_FIXED_DIGITS * 2];
	assert(ndigits <= MAX_FIXED_DIGITS);
#else
	DIGIT_T *t1, *t2, *y;
	t1 = mpAlloc(nn);
	t2 = mpAlloc(nn);
	y = mpAlloc(nn);
#endif
	assert(ndigits != 0);
	n = mpSizeof(e, ndigits);

	 /* Catch e==0 => x^0=1 */
	if (0 == n) {
		mpSetDigit(yout, 1, ndigits);
		goto done;
	}

	    /* Find second-most significant bit in e */
	for (mask = HIBITMASK; mask > 0; mask >>= 1) {
		if (e[n - 1] & mask)
			break;
	}
	mpNEXTBITMASK(mask, n);

	    /* Set y = x */
	mpSetEqual(y, x, ndigits);

	    /* For bit j = k-2 downto 0 */
	while (n) {
		    /* Square y = y * y mod n */
		mpMODSQUARETEMP(y, m, ndigits, t1, t2);
		if (e[n - 1] & mask) {	/*      if e(j) == 1 then multiply  y = y * x mod n */
			mpMODMULTTEMP(y, x, m, ndigits, t1, t2);
		}

		    /* Move to next bit */
		mpNEXTBITMASK(mask, n);
	}

	    /* Return y */
	mpSetEqual(yout, y, ndigits);
done:
	mpDESTROY(t1, nn);
	mpDESTROY(t2, nn);
	mpDESTROY(y, ndigits);
	return 0;
}


/* Use sliding window alternative only if NO_ALLOCS not defined */
#ifndef NO_ALLOCS

/*
SLIDING-WINDOW EXPONENTIATION
Ref: Menezes, chap 14, p616.
k is called the window size.

14.85 Algorithm Sliding-window exponentiation
INPUT: g, e = (e_t.e_{t-1}...e1.e0)_2 with e_t = 1, and an integer k >= 1.
OUTPUT: g^e.
1. Precomputation.
	1.1 g_1 <-- g, g_2 <-- g^2.
	1.2 For i from 1 to (2^{k-1} - 1) do: g_{2i+1} <-- g_{2i-1} * g_2.
2. A <-- 1, i <-- t.
3. While i >= 0 do the following:
	3.1 If e_i = 0 then do: A <-- A^2, i <-- i - 1.
	3.2 Otherwise (e_i != 0), find the longest bitstring e_i.e_{i-1}...e_l such that i-l+1 <= k
and e_l = 1, and do the following:
	A <-- A^{2i-l+1} * g_{(e_i.e_{i-1}...e_l)_2}
	i <-- l - 1.
4. Return(A).
*/

/*
Optimal values of k for various exponent sizes.
	The references on this differ in their recommendations.
	These values reflect experiments we've done on our systems.
	You can adjust this to suit your own situation.
*/
static size_t WindowLenTable[] = {
/* k=1   2   3   4    5     6     7     8 */
	5, 16, 64, 240, 768, 1024, 2048, 4096
};


#define WINLENTBLMAX (sizeof(WindowLenTable)/sizeof(WindowLenTable[0]))

/*	The process used here to read bits into the lookahead buffer could be improved slightly as
	some bits are read in more than once. But we think this function is tricky enough without
	adding more complexity for marginal benefit.
*/
static int mpModExp_windowed(DIGIT_T yout[], const DIGIT_T g[], const DIGIT_T e[], DIGIT_T m[], size_t ndigits)
/* Computes y = g^e mod m using sliding-window exponentiation */
{
	size_t nbits;		/* Number of significant bits in e */
	size_t winlen;		/* Window size */
	DIGIT_T mask;		/* Bit mask used for main loop */
	size_t nwd;		/* Digit counter for main loop */
	DIGIT_T lkamask;	/* Bit mask for lookahead */
	size_t lkanwd;		/* Digit counter for lookahead */
	DIGIT_T lkabuf;	/* One-digit lookahead buffer */
	size_t lkalen;		/* Actual size of window for current lookahead buffer */
	int in_window;		/* Flag for in-window state */
	DIGIT_T *gtable[(1 << (WINLENTBLMAX - 1))];	/* Table of ptrs to g1, g3, g5,... */
	size_t ngt;		/* No of elements in gtable */
	size_t idxmult;	/* Index (in gtable) of next multiplier to use: 0=g1, 1=g3, 2=g5,... */
	DIGIT_T *g2;		/* g2 = g^2 */
	DIGIT_T *temp1, *temp2;	/* Temp big digits, needed for MULT and SQUARE macros */
	DIGIT_T *a;		/* A */
	int aisone;		/* Flag that A == 1 */
	size_t nn;		/* 2 * ndigits */
	size_t i;		/* Temp counter */

	    /* Get actual size of e */
	nbits = mpBitLength(e, ndigits);
	DPRINTF1("nbits=%d\n", nbits);

	    /* Catch easy ones */
	if (nbits == 0) {		/* g^0 = 1 */
		mpSetDigit(yout, 1, ndigits);
		return 1;
	}
	if (nbits == 1) {		/* g^1 = g */
		mpSetEqual(yout, g, ndigits);
		return 1;
	}

	    /* Lookup optimised window length for this size of e */
	for (winlen = 0; winlen < WINLENTBLMAX && winlen < BITS_PER_DIGIT; winlen++) {
		if (WindowLenTable[winlen] > nbits)
			break;
	}
	DPRINTF1("winlen=%d\n", winlen);

	    /* Default to simple L-R method for 1-bit window */
	if (winlen <= 1)
		return mpModExp_1(yout, g, e, m, ndigits);

	    /* Allocate temp vars - NOTE: all are 2n long */
	nn = 2 * ndigits;
	temp1 = mpAlloc(nn);
	temp2 = mpAlloc(nn);
	g2 = mpAlloc(nn);
	a = mpAlloc(nn);

	    /* 1. PRECOMPUTATION */
	    /* 1.1 g1 <-- g, (we already have g in the input, so just point to it) */
	gtable[0] = (DIGIT_T *) g;

	    /* g2 <-- g^2 */
	mpModMult(g2, gtable[0], gtable[0], m, ndigits);

	    /* 1.2 For i from 1 to (2^{k-1} - 1) do: g_{2i+1} <-- g_{2i-1} * g_2. */
	    /* i.e. we store (g1, g3, g5, g7,...) */
	ngt = ((size_t) 1 << (winlen - 1));
	for (i = 1; i < ngt; i++) {
		    /* NOTE: we need these elements to be 2n digits long for the mpMODMULTTEMP fn,
		       but the final result is only n digits long */
		gtable[i] = mpAlloc(nn);

		    /*mpModMult(gtable[i], gtable[i-1], g2, m, ndigits);*/
		mpSetEqual(gtable[i], gtable[i - 1], ndigits);
		mpMODMULTTEMP(gtable[i], g2, m, ndigits, temp1, temp2);
	}

	    /* 2. A <-- 1 (use flag) */
	    /*mpSetDigit(a, 1, ndigits);*/
	aisone = 1;

	    /* Find most significant bit in e */
	nwd = mpSizeof(e, ndigits);
	for (mask = HIBITMASK; mask > 0; mask >>= 1) {
		if (e[nwd - 1] & mask)
			break;
	}

	    /* i <-- t; 3. While i >= 0 do the following: */
	    /* i.e. look at high bit and every subsequent bit L->R in turn */
	lkalen = 0;
	in_window = 0;
	idxmult = 0;
	while (nwd) {
		    /* We always square each time around */
		    /* A <-- A^2 */
		if (!aisone) {/* 1^2 = 1! */
			mpMODSQUARETEMP(a, m, ndigits, temp1, temp2);
		}
		if (!in_window) {	/* Do we start another window? */
			if ((e[nwd - 1] & mask)) {	/* Yes, bit is '1', so setup this window */
				in_window = 1;

				    /* Read in look-ahead buffer (a single digit) */
				lkamask = mask;
				lkanwd = nwd;

				    /* Read in this and the next winlen-1 bits into lkabuf */
				lkabuf = 0x1;
				for (i = 0; i < winlen - 1; i++) {
					mpNEXTBITMASK(lkamask, lkanwd);
					lkabuf <<= 1;

					    /* if lkanwd==0 we have passed the end, so just append a zero bit */
					if (lkanwd && (e[lkanwd - 1] & lkamask)) {
						lkabuf |= 0x1;
					}
				}
				DPRINTF1("(%x)", lkabuf);

				    /* Compute this window's length */
				    /* i.e. keep shifting right until we have a '1' bit at the end */
				for (lkalen = winlen - 1; lkalen > 0; lkalen--, lkabuf >>= 1) {
					if (ISODD(lkabuf))
						break;
				}

				    /* Set next multipler to use */
				    /* idx = (value(buf) - 1) / 2 */
				idxmult = lkabuf >> 1;
				DPRINTF0("1");
			} else {	/* No, bit is '0', so just loop */
				DPRINTF0("0 ");
			}
		} else {	/* We are in a window... */
			DPRINTF1("%s", ((e[nwd - 1] & mask) ? "1" : "0"));

			    /* Has it finished yet? */
			if (lkalen > 0) {
				lkalen--;
			}
		}
		    /* Are we at end of this window? */
		if (in_window && lkalen < 1) {	/* Yes, so compute A <-- A * g_l */
			if (aisone) {
				mpSetEqual(a, gtable[idxmult], ndigits);
				aisone = 0;
			} else {
				mpMODMULTTEMP(a, gtable[idxmult], m, ndigits, temp1, temp2);
			}
			DPRINTF1("[%x]", idxmult);
			DPRINTF0("/ ");
			in_window = 0;
			lkalen = 0;
		}
		mpNEXTBITMASK(mask, nwd);
	}

	    /* Finally, cope with anything left in the final window */
	if (in_window) {
		if (aisone) {
			mpSetEqual(a, gtable[idxmult], ndigits);
			aisone = 0;
		} else {
			mpMODMULTTEMP(a, gtable[idxmult], m, ndigits, temp1, temp2);
		}
		DPRINTF1("[%x]", idxmult);
		DPRINTF0("//");
	}
	DPRINTF0("\n");

	    /* 4. Return (A) */
	mpSetEqual(yout, a, ndigits);

	    /* Clean up */
	mpDESTROY(a, nn);
	mpDESTROY(g2, nn);
	mpDESTROY(temp1, nn);
	mpDESTROY(temp2, nn);

	    /* CAUTION: don't clean gtable[0] */
	for (i = 1; i < ngt; i++)
		mpDESTROY(gtable[i], nn);
	return 0;
}

#endif	/* !NO_ALLOCS */
