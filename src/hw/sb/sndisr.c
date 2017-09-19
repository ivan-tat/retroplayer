/* sndisr.c -- Hardware IRQ callback for sound card.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "hw/sb/sndisr.h"

void *__far __pascal GetSoundHWISR( void )
{
    return &SoundHWISR;
}

void __far __pascal SetSoundHWISRCallback( SoundHWISRCallback_t *p )
{
    SoundHWISRCallback = p;
}
