/* _fprintf.h -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_STDIO__FPRINTF_H
#define CC_STDIO__FPRINTF_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include "cc/dstream.h"

bool __far _datastream_flush_file (DATASTREAM *self);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _datastream_flush_file "*";

#endif  /* __WATCOMC__ */

#endif  /* CC_STDIO__FPRINTF_H */
