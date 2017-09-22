/* dos.h -- declarations for custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_DOS_H
#define CC_DOS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>
#include "pascal/pascal.h"

struct cc_dosdate_t {
    unsigned char  day;         /* 1-31 */
    unsigned char  month;       /* 1-12 */
    unsigned short year;        /* 1980-2099 */
    unsigned char  dayofweek;   /* 0-6 (0=Sunday) */
};

struct cc_dostime_t {
    unsigned char hour;     /* 0-23 */
    unsigned char minute;   /* 0-59 */
    unsigned char second;   /* 0-59 */
    unsigned char hsecond;  /* 1/100 second; 0-99 */
};

void _cc_dos_getdate(struct cc_dosdate_t *d);
void _cc_dos_gettime(struct cc_dostime_t *t);

void __far *_cc_dos_getvect(unsigned num);
void _cc_dos_setvect(unsigned num, void __far *p);

#define __cc_dos_para(size) (((size) + 15) >> 4)

#ifdef USE_INTRINSICS

#define _cc_dos_para(size) __cc_dos_para(size)

#else   /* !USE_INTRINSICS */

/*
unsigned _cc_dos_para(unsigned size);
*/
uint16_t PUBLIC_CODE _cc_dos_para(uint16_t size);

#endif  /* !USE_INTRINSICS */

/*
unsigned _cc_dos_allocmem(unsigned size, unsigned *seg);
unsigned _cc_dos_freemem(unsigned seg);
unsigned _cc_dos_setblock(unsigned size, unsigned seg, unsigned *max);
*/

// Pascal
uint16_t PUBLIC_CODE _cc_dos_allocmem(uint16_t size, uint16_t *seg);
uint16_t PUBLIC_CODE _cc_dos_freemem(uint16_t seg);
uint16_t PUBLIC_CODE _cc_dos_setblock(uint16_t size, uint16_t seg, uint16_t *max);

/* Aliases */

#define dosdate_t cc_dosdate_t
#define dostime_t cc_dostime_t

#define _dos_getdate  _cc_dos_getdate
#define _dos_gettime  _cc_dos_gettime

#define _dos_getvect _cc_dos_getvect
#define _dos_setvect _cc_dos_setvect

#define __dos_para __cc_dos_para
#define _dos_para _cc_dos_para

#define _dos_allocmem _cc_dos_allocmem
#define _dos_freemem  _cc_dos_freemem
#define _dos_setblock _cc_dos_setblock

/* Linkning */

#ifdef __WATCOMC__

#pragma aux _cc_dos_getdate "^";
#pragma aux _cc_dos_gettime "^";
#pragma aux _cc_dos_getvect "^";
#pragma aux _cc_dos_setvect "^";
#ifndef USE_INTRINSICS
/*
#pragma aux _cc_dos_para "^";
*/
#pragma aux _cc_dos_para "_DOS_PARA";
#endif
/*
#pragma aux _cc_dos_allocmem "^";
#pragma aux _cc_dos_freemem "^";
#pragma aux _cc_dos_setblock "^";
*/
#pragma aux _cc_dos_allocmem "_DOS_ALLOCMEM";
#pragma aux _cc_dos_freemem  "_DOS_FREEMEM";
#pragma aux _cc_dos_setblock "_DOS_SETBLOCK";

#endif  /* __WATCOMC__ */

#endif  /* CC_DOS_H */
