/* fillvars.h -- declarations for fillvars.pas.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef FILLVARS_H
#define FILLVARS_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove reserved words "extern" and PUBLIC_DATA macros when done.

#include "..\pascal\pascal.h"

/* DMA buffer */
extern void    *PUBLIC_DATA AllocBuffer;
    /* position where we allocate DMA buffer - remember that we may use second half ... */
extern void    *PUBLIC_DATA DMAbuffer;
extern void    *PUBLIC_DATA PlayBuffer;
    /* pointer to DMAbuffer - for public use, but don't write into it !!!
       - it's never used for any action while mixing !
       - while playing you can read the DMA base counter
       and find out in that way what sample value the
       SB currently plays ... refer to DMA Controller */
extern uint8_t  PUBLIC_DATA NumBuffers;
    /* number of parts in DMAbuffer */
extern uint8_t  PUBLIC_DATA LastReady;
    /* last part of DMAbuffer we calculated last call */
extern uint8_t  PUBLIC_DATA DMAHalf;
    /* last part of DMAbuffer we have to fill */
extern uint16_t PUBLIC_DATA DMARealBufSize[64];

/* mixer */

extern void   *PUBLIC_DATA TickBuffer;
    /* the well known buffer for one tick - size depends on _currennt_tempo_ */
extern bool    PUBLIC_DATA LQMode;
    /* flag if lowquality mode */
extern bool    PUBLIC_DATA TooSlow;
extern bool    PUBLIC_DATA JustInFill;
extern bool    PUBLIC_DATA RasterTime;
extern uint8_t PUBLIC_DATA FPS;
    /* frames per second ... default is about 70Hz */

/* EMM */

extern uint16_t PUBLIC_DATA SavHandle;

#endif /* FILLVARS_H */
