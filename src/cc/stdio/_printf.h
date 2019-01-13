/* _printf.h -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_STDIO_PRINTF_H
#define CC_STDIO_PRINTF_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "dstream.h"

#include "cc/stdio.h"

bool __far _system_flush_file (DATASTREAM *self);
bool __far _system_flush_stdout (DATASTREAM *self);

void __far _printf (DATASTREAM *stream, const char *format, va_list ap);

/*** Externals ***/

extern void __far __pascal pascal_write (const char *str);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _system_flush_file "*";
#pragma aux _system_flush_stdout "*";

#pragma aux _printf "*";

#pragma aux pascal_write "*" modify [ ax bx cx dx si di es ];

#endif  /* __WATCOMC__ */

#endif  /* CC_STDIO_PRINTF_H */
