/* sndisr.h -- declarations for sndisr.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _SNDISR_H
#define _SNDISR_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

#include "hw/sb/sndisr_.h"

void *PUBLIC_CODE GetSoundHWISR(void);
void  PUBLIC_CODE SetSoundHWISRCallback(SoundHWISRCallback_t *p);

#endif  /* _SNDISR_H */
