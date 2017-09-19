/* sndisr.h -- declarations for sndisr.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _SNDISR_H
#define _SNDISR_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "hw/sb/sndisr_.h"

void *__far __pascal GetSoundHWISR( void );
void  __far __pascal SetSoundHWISRCallback( SoundHWISRCallback_t *p );

#endif /* _SNDISR_H */
