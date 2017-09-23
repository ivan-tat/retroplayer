/* sndisr_.h -- declarations for sndisr_.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _SNDISR__H
#define _SNDISR__H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include "pascal.h"

typedef void PUBLIC_CODE SoundHWISRCallback_t(void);

extern SoundHWISRCallback_t *PUBLIC_DATA SoundHWISRCallback;

extern void PUBLIC_CODE SoundHWISR(void);

#endif  /* _SNDISR__H */
