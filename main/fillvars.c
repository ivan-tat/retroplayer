/* fillvars.c -- variables for functions to fill DMA buffer.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

/* DMA buffer */

void    *AllocBuffer;
    /* position where we allocate DMA buffer - remember that we may use second half ... */
void    *DMAbuffer;
void    *PlayBuffer;
    /* pointer to DMAbuffer - for public use, but don't write into it !!!
       - it's never used for any action while mixing !
       - while playing you can read the DMA base counter
       and find out in that way what sample value the
       SB currently plays ... refer to DMA Controller */
uint8_t  NumBuffers;
    /* number of parts in DMAbuffer */
uint8_t  LastReady;
    /* last part of DMAbuffer we calculated last call */
uint8_t  DMAHalf;
    /* last part of DMAbuffer we have to fill */
uint16_t DMARealBufSize[64];

/* mixer */

void   *TickBuffer;
    /* the well known buffer for one tick - size depends on _currennt_tempo_ */
bool    LQMode;
    /* flag if lowquality mode */
bool    TooSlow;
bool    JustInFill;
bool    RasterTime;
uint8_t FPS;
    /* frames per second ... default is about 70Hz */

/* EMM */

uint16_t SavHandle;
