/* limits.h -- declarations for custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_DOS_LIMITS_H
#define CC_DOS_LIMITS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#define cc_DirStr_size  68
#define cc_NameStr_size 8
#define cc_ExtStr_size  4
#define cc_ComStr_size  128
#define cc_PathStr_size 80

#endif  /* CC_DOS_LIMITS_H */
