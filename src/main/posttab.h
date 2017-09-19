/* posttab.h -- declarations for posttab.pas.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _POSTTAB_H
#define _POSTTAB_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

extern uint8_t  __pascal post8bit[4096];
extern uint16_t __pascal post16bit[4096];

void __far __pascal calcPostTable( uint8_t vol, bool use16bit );

#endif /* _POSTTAB_H */
