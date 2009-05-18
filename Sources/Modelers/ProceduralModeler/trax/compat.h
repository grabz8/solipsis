#ifndef _compat_h
#define _compat_h 1

/*
 * Copyright (c) 1993 Martin Birgmeier
 * All rights reserved.
 *
 * You may redistribute unmodified or modified versions of this source
 * code provided that the above copyright notice and this and the
 * following conditions are retained.
 *
 * This software is provided ``as is'', and comes with no warranties
 * of any kind. I shall in no event be liable for anything that happens
 * to anyone/anything when using this software.
 */

#ifdef cplus_plus
extern "C" {
#endif

/* drand48, erand48 and jrand48 functions from FreeBSD libc 
 * 30/07/2001 + 25/01/2007 S. Grignard */

#include <math.h>

extern double	 drand48 (void);
extern double	 erand48 (unsigned short[3]);
extern void	 srand48 (long);
extern double    asinh(double);
extern long  jrand48 (unsigned short[3]);


void		_dorand48 (unsigned short[3]);

#define	RAND48_SEED_0	(0x330e)
#define	RAND48_SEED_1	(0xabcd)
#define	RAND48_SEED_2	(0x1234)
#define	RAND48_MULT_0	(0xe66d)
#define	RAND48_MULT_1	(0xdeec)
#define	RAND48_MULT_2	(0x0005)
#define	RAND48_ADD	(0x000b)


#ifdef c_plus_plus
}
#endif

#endif
