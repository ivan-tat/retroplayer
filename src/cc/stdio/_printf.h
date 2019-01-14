/* _printf.h -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_STDIO__PRINTF_H
#define CC_STDIO__PRINTF_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dstream.h"

#include "cc/stdio.h"

bool __far _datastream_flush_console (DATASTREAM *self);

/*** Externals ***/

extern void __far __pascal pascal_write (const char *str);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _datastream_flush_console "*";

#pragma aux pascal_write "*" modify [ ax bx cx dx si di es ];

#endif  /* __WATCOMC__ */

#endif  /* CC_STDIO__PRINTF_H */
