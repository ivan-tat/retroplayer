/* sbio.h -- declarations for sbio.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef SBIO_H
#define SBIO_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

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

void    PUBLIC_CODE sbioMixerReset(uint16_t base);
uint8_t PUBLIC_CODE sbioMixerRead(uint16_t base, uint8_t reg);
void    PUBLIC_CODE sbioMixerWrite(uint16_t base, uint8_t reg, uint8_t data);

/* DSP */

bool    PUBLIC_CODE sbioDSPReset(uint16_t base);
uint8_t PUBLIC_CODE sbioDSPRead(uint16_t base);
bool    PUBLIC_CODE sbioDSPReadQueue(uint16_t base, uint8_t *data, uint16_t length);
bool    PUBLIC_CODE sbioDSPWrite(uint16_t base, uint8_t data);
bool    PUBLIC_CODE sbioDSPWriteQueue(uint16_t base, uint8_t *data, uint16_t length);

/* IRQ */

void PUBLIC_CODE sbioDSPAcknowledgeIRQ(uint16_t base, bool mode16bit);

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

#endif  /* SBIO_H */
