/* i86.h -- declarations for custom "i86" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_I86_H
#define CC_I86_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

/* intr structs */

struct CC_REGPACKB
{
    unsigned char al, ah;
    unsigned char bl, bh;
    unsigned char cl, ch;
    unsigned char dl, dh;
};
#define REGPACKB CC_REGPACKB

struct CC_REGPACKW
{
    unsigned short ax;
    unsigned short bx;
    unsigned short cx;
    unsigned short dx;
    unsigned short bp;
    unsigned short si;
    unsigned short di;
    unsigned short ds;
    unsigned short es;
    unsigned int flags;
};
#define REGPACKW CC_REGPACKW

union CC_REGPACK
{
    struct REGPACKB h;
    struct REGPACKW w;
};
#define REGPACK CC_REGPACK

/* bits defined for flags field defined in REGPACKW */

enum
{
    CC_INTR_CF = 0x0001, /* carry */
    CC_INTR_PF = 0x0004, /* parity */
    CC_INTR_AF = 0x0010, /* auxiliary carry */
    CC_INTR_ZF = 0x0040, /* zero */
    CC_INTR_SF = 0x0080, /* sign */
    CC_INTR_TF = 0x0100, /* trace */
    CC_INTR_IF = 0x0200, /* interrupt */
    CC_INTR_DF = 0x0400, /* direction */
    CC_INTR_OF = 0x0800  /* overflow */
};

#define INTR_CF CC_INTR_CF
#define INTR_PF CC_INTR_PF
#define INTR_AF CC_INTR_AF
#define INTR_ZF CC_INTR_ZF
#define INTR_SF CC_INTR_SF
#define INTR_TF CC_INTR_TF
#define INTR_IF CC_INTR_IF
#define INTR_DF CC_INTR_DF
#define INTR_OF CC_INTR_OF

extern void cc_delay(unsigned int __milliseconds);

#ifdef __WATCOMC__

extern void __far __watcall cc_intr(int, union CC_REGPACK *);

void _cc_disable(void);
#pragma aux _cc_disable = \
    "cli" \
    parm nomemory \
    modify nomemory exact [];

void _cc_enable(void);
#pragma aux _cc_enable = \
    "sti" \
    parm nomemory \
    modify nomemory exact [];

#pragma intrinsic(_cc_disable, _cc_enable);

#else

extern void cc_intr(int, union CC_REGPACK *);

extern void _cc_disable(void);
extern void _cc_enable(void);

#endif  /* __WATCOMC__ */

/*** Aliases ***/

#define delay cc_delay
#define intr cc_intr

#define _disable _cc_disable
#define _enable _cc_enable

/* macros to break 'far' pointers into segment and offset components */

#ifndef FP_OFF
 #define FP_OFF(__p) ((unsigned)(__p))
#endif

#ifndef FP_SEG
 #define FP_SEG(__p) ((unsigned)((unsigned long)(void __far*)(__p) >> 16))
#endif

/* make a far pointer from segment and offset */

#ifndef MK_FP
 #define MK_FP(__s,__o) (((unsigned short)(__s)):>((void __near *)(__o)))
#endif

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux cc_delay "*";
#pragma aux cc_intr "*" parm [ ax ] [ bx cx ] modify [ ax bx cx dx es ];

#endif  /* __WATCOMC__ */

#endif  /* CC_I86_H */
