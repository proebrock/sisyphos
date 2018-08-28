#ifndef __GLOBALS_H__
#define __GLOBALS_H__



#include <stdlib.h>
#include <stdio.h>



#ifndef NDEBUG

extern void assert_failed(const char *test, const char *file,
	int line, const char *message);

#define ASSERT(TST)			\
			((TST) ? ((void) 0) : (assert_failed(#TST, __FILE__, __LINE__, (const char *)0)))
#define ASSERTMSG(TST, MSG)	\
			((TST) ? ((void) 0) : (assert_failed(#TST, __FILE__, __LINE__, (MSG))))
#define DEBUGMSG(MSG)		\
			(fprintf(stderr, (MSG)))

extern void check_bitoperations();

#else	/* NDEBUG */

#define ASSERT(TST)			((void)0)
#define ASSERTMSG(TST,MSG)	((void)0)
#define DEBUGMSG(MSG)		((void)0)

#endif	/* NDEBUG */



#ifdef linux

	typedef char				int8;
	typedef short				int16;
	typedef int					int32;
	typedef long long			int64;

	typedef unsigned char		uint8;
	typedef unsigned short		uint16;
	typedef unsigned int		uint32;
	typedef unsigned long long	uint64;

	#define UINT64CONST(C)	(uint64)(C ## ull)

#endif	/* linux */

#ifdef WIN32

	typedef __int8				int8;
	typedef __int16				int16;
	typedef __int32				int32;
	typedef __int64				int64;

	typedef unsigned __int8		uint8;
	typedef unsigned __int16	uint16;
	typedef unsigned __int32	uint32;
	typedef unsigned __int64	uint64;

	#define UINT64CONST(C)	(uint64)(C)

#endif	/* WIN32 */



#define MIN(A, B)	(((A) < (B)) ? (A) : (B))
#define MAX(A, B)	(((A) > (B)) ? (A) : (B))



/* basic bit operations, bit 0 is least significant bit */

/* get mask with bit N set */
#define BITMASK(N)				(UINT64CONST(1) << (N))
/* check if bit N is set in X (result is bool) */
#define TESTBIT(X, N)			(((X) & BITMASK(N)) != 0)
/* set bit N in X */
#define SETBIT(X, N)			(X) |= BITMASK(N)
/* unset bit N in X */
#define CLEARBIT(X, N)			(X) &= (~BITMASK(N))
/* copy bit SN in S into bit SD in D */
#define COPYBIT(D, ND, S, NS)	\
	(D) = ((D) & ~(UINT64CONST(1) << (ND))) | ((((S) >> (NS)) & UINT64CONST(1)) << (ND))



/*
** bit operations are the real bottleneck when working with bitboards,
** but they are replaceable by efficient assembler routines for special
** target architectures
*/

/* common implementation, should work on all architectures */
#if !defined(__i386__) && !defined(__amd64__) && !defined(__arm__)

#warning "No specialized version of GETNEXTBIT, performance suboptimal."
#define GETNEXTBIT(MASK, POS)						\
	for (POS = 0; !TESTBIT((MASK), (POS)); (POS)++);

#endif	/* common case */



#ifdef __i386__

/* using commands bsf/bsr (bit scan forward/reverse
   machine coded commands are jump commands: unable to use labels in macro */
#define GETNEXTBIT(MASK, POS)						 \
		__asm__ __volatile__ ("or %%eax,%%eax		;\
			.byte 0x74,0x05							;\
			bsf %%eax,%%eax							;\
			.byte 0xeb,0x05							;\
			bsf %%edx,%%eax							;\
			addb $0x20,%%al							" : "=a"(POS) : "A"(MASK));

#endif	/* ARCH_I386 */



/* implementation for AMD 64-Bit machines */
#ifdef __amd64__

#define GETNEXTBIT(MASK, POS)						\
		__asm__ __volatile__ ("bsfq %%rax,%%rax" : "=a" (POS) : "a" (MASK));

#endif	/* ARCH_AMD64 */



#ifdef __arm__

#define GETNEXTBIT(MASK, POS)						\
		__asm__ __volatile ("clz %0, %1" : "=&r" (POS) : "r" (MASK));	\
		(POS) = 64 - 1 - (POS);

#endif /* ARCH_ARM */




#define CLEARNEXTBIT(MASK, POS)						\
	GETNEXTBIT((MASK), (POS));						\
	CLEARBIT((MASK), (POS));



/* toggle an integer value between 0 and 1: TOGGLED(0) = 1, TOGGLED(1) = 0 */
#define TOGGLED(X)		((X) ^ 1)



extern void si_print(FILE *f, uint64 n);



#endif	/* __GLOBALS_H__ */
