/* _printf.h -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CC_STDIO__PRINTF_H_INCLUDED
#define _CC_STDIO__PRINTF_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dstream.h"
#include "cc/stdio.h"

bool __far _datastream_flush_console (DATASTREAM *self);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _datastream_flush_console "*";

#endif  /* __WATCOMC__ */

#endif  /* !_CC_STDIO__PRINTF_H_INCLUDED */
