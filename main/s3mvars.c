/* s3mvars.c -- variables for s3m play.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "s3mtypes.h"

/* general module information */

bool             mod_isLoaded;
modTitle_t       mod_Title;         /* name given by the musician */
modTrackerName_t mod_TrackerName;   /* tracker version file was created with */

/* module options */

bool modOption_ST2Vibrato;      /* not supported */
bool modOption_ST2Tempo;        /* not supported */
bool modOption_AmigaSlides;     /* not supported */
bool modOption_SBfilter;        /* not supported */
bool modOption_CostumeFlag;     /* not supported */
bool modOption_VolZeroOptim;    /* PSIs volume-zero optimization */
bool modOption_AmigaLimits;     /* check for amiga limits */
bool modOption_SignedData;      /* signed/unsigned data */
                                /* (only volumetable differs in those modes) */

/* play options */

bool playOption_ST3Order;
    /* if true then handle order like ST3 - if a "--"=255 is found -
        stop or loop to the song start (look playOption_LoopSong) */
    /* if false - play the whole order and simply skip the "--"
        if (CurOrder==OrdNum) then stop or loop to the beginning */
bool playOption_LoopSong;
    /* flag if restart if we reach the end of the S3M module */

/* instruments */

instrumentsList_t *Instruments; /* pointer to data for all instruments */
uint16_t InsNum;

/* patterns */

patternsList_t Pattern; /* segment for every pattern */
                        /* 0xFxyy -> at EMS page YY on Offset X*5120 */
uint16_t PatNum;
uint16_t PatLength;     /* length of one pattern */

/* song arrangement */

ordersList_t Order;
uint16_t OrdNum;
uint8_t  LastOrder; /* last order to play */

#ifdef BETATEST
uint16_t StartOrder;
#endif

channelsList_t Channel; /* all public/private data for every channel */
uint8_t UsedChannels;   /* possible values : 1..32 (kill all Adlib) */

/* initial state */

uint8_t InitTempo;  /* initial tempo */
uint8_t InitSpeed;  /* initial speed */

/* play state */

bool    EndOfSong;
uint8_t CurTempo;   /* current tempo - count of ticks per note */
uint8_t CurSpeed;   /* current speed - length of one tick */
uint8_t GVolume;    /* global volume -> usedvol = instrvol*gvolume/255 */
uint8_t MVolume;    /* master volume -> calc posttables */

/* position in song - you can change it while playing to jump arround */

uint8_t CurOrder;   /* position in song arrangement */
uint8_t CurPattern; /* current pattern - is specified also by Order[curorder] */
                    /* so it's only for the user ... */
uint8_t CurLine;    /* current line in pattern */
uint8_t CurTick;    /* current tick - we only calc one tick per call */
                    /* (look at MIXING.ASM) */

/* pattern loop */

bool    PLoop_On;    /* in a Pattern loop? */
uint8_t PLoop_No;    /* number of loops left */
uint8_t PLoop_To;    /* position to loop to */

/* pattern delay */

uint8_t PatternDelay;

/* EMM */

bool     UseEMS;
uint16_t PatEMSHandle;  /* handle to access EMS for patterns */
uint16_t SmpEMSHandle;  /* hanlde to access EMS for samples */
                        /* I seperated them, but that does not matter, well ? */
bool     EMSPat;        /* patterns in EMS ? */
bool     EMSSmp;        /* samples in EMS ? */
uint8_t  PatPerPage;    /* count of patterns per page (<64!!!) */
