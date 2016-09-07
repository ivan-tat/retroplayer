#ifndef _SNDISR_H
#define _SNDISR_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "sndisr_.h"

void *__far __pascal GetSoundHWISR( void );
void  __far __pascal SetSoundHWISRCallback( SoundHWISRCallback_t *p );

#endif /* _SNDISR_H */
