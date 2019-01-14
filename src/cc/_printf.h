/* _printf.h -- declarations for _printf.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC__PRINTF_H
#define CC__PRINTF_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdarg.h>
#include "cc/string.h"
#include "dstream.h"

bool __far _printf (DATASTREAM *stream, const char *format, va_list ap);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _printf "*";

#endif  /* __WATCOMC__ */

#endif  /* CC__PRINTF_H */
