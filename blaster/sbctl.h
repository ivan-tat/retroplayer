#ifndef _SBCTL_H
#define _SBCTL_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

/* hardware config */

extern uint8_t  __pascal sbno;              /* type */
extern uint16_t __pascal dsp_addr;          /* base address */
extern uint8_t  __pascal irq_no;            /* IRQ */
extern uint8_t  __pascal dma_channel;       /* DMA channel for 8bit play */
extern uint8_t  __pascal dma_16bitchannel;  /* DMA channel for 16bit play */

/* capabilities */

extern bool     __pascal stereo_possible;   /* flag if stereo is possible */
extern bool     __pascal _16Bit_possible;   /* flag if 16bit play is possible */
extern uint16_t __pascal maxstereorate;     /* max stereo samplerate */
extern uint16_t __pascal maxmonorate;       /* max mono samplerate */

/* transfer mode */

extern bool __pascal _16bit;        /* (only on SB16 possible) */
extern bool __pascal signeddata;    /* (only on SB16 possible) */
extern bool __pascal stereo;

void __far __pascal sbMixerWrite( uint8_t reg, uint8_t data );
uint8_t __far __pascal sbMixerRead( uint8_t reg );
void __far __pascal speaker_on( void ); /* Does not work on SB16 */
void __far __pascal speaker_off( void );
void __far __pascal setupDMATransfer( void *p, uint16_t count, bool autoinit );
void __far __pascal sbSetupDSPTransfer( uint16_t len, bool b16, bool autoinit );

#endif /* _SBCTL_H */
