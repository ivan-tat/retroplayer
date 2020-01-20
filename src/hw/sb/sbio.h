/* sbio.h -- declarations for sbio.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _SBIO_H_INCLUDED
#define _SBIO_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>

/* Sound Blaster hardware i/o errors */

typedef uint16_t sbio_error_t;
typedef sbio_error_t SBIOERR;

extern SBIOERR sbioError;

#define E_SBIO_SUCCESS 0
#define E_SBIO_DSP_RESET_FAILED 1
#define E_SBIO_DSP_READ_FAILED 2
#define E_SBIO_DSP_WRITE_FAILED 3

/* mixer */

#define SBIO_MIXER_DAC_LEVEL     0x04
#define SBIO_MIXER_MASTER_VOLUME 0x22
#define SBIO_MIXER_MASTER_LEFT   0x30
#define SBIO_MIXER_MASTER_RIGHT  0x31
#define SBIO_MIXER_VOICE_LEFT    0x32
#define SBIO_MIXER_VOICE_RIGHT   0x33

void    __far sbioMixerReset (uint16_t base);
uint8_t __far sbioMixerRead (uint16_t base, uint8_t reg);
void    __far sbioMixerWrite (uint16_t base, uint8_t reg, uint8_t data);

/* DSP */

bool    __far sbioDSPReset (uint16_t base);
uint8_t __far sbioDSPRead (uint16_t base);
bool    __far sbioDSPReadQueue (uint16_t base, uint8_t *data, uint16_t length);
bool    __far sbioDSPWrite (uint16_t base, uint8_t data);
bool    __far sbioDSPWriteQueue (uint16_t base, uint8_t *data, uint16_t length);

/* IRQ */

void __far sbioDSPAcknowledgeIRQ (uint16_t base, bool mode16bit);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux sbioError "*";

#pragma aux sbioMixerReset "*";
#pragma aux sbioMixerRead "*";
#pragma aux sbioMixerWrite "*";

#pragma aux sbioDSPReset "*";
#pragma aux sbioDSPRead "*";
#pragma aux sbioDSPReadQueue "*";
#pragma aux sbioDSPWrite "*";
#pragma aux sbioDSPWriteQueue "*";

#pragma aux sbioDSPAcknowledgeIRQ "*";

#endif  /* __WATCOMC__ */

#endif  /* !_SBIO_H_INCLUDED */
