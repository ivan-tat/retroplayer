/* sndisr.c -- Hardware IRQ callback for sound card.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

#include "hw/sb/sndisr.h"

void *PUBLIC_CODE GetSoundHWISR(void)
{
    return &SoundHWISR;
}

void PUBLIC_CODE SetSoundHWISRCallback(SoundHWISRCallback_t *p)
{
    SoundHWISRCallback = p;
}
