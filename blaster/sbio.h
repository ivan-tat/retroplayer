#ifndef _SBIO_H
#define _SBIO_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

/* Sound Blaster hardware i/o errors */

typedef uint16_t SBIOError_t;

extern __pascal sbioError;

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

void    __far __pascal sbioMixerReset( uint16_t base );
uint8_t __far __pascal sbioMixerRead( uint16_t base, uint8_t reg );
void    __far __pascal sbioMixerWrite( uint16_t base, uint8_t reg, uint8_t data );

/* DSP */

bool    __far __pascal sbioDSPReset( uint16_t base );
uint8_t __far __pascal sbioDSPRead( uint16_t base );
bool    __far __pascal sbioDSPWrite( uint16_t base, uint8_t data );

/* IRQ */

void __far __pascal sbioDSPAcknowledgeIRQ( uint16_t base, bool mode16bit );

#endif /* _SBIO_H */
