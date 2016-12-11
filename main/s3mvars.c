/* s3mvars.c -- variables for s3m play.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove EXTERN_LINK, PUBLIC_DATA and PUBLIC_CODE macros when done.

#include "..\pascal\pascal.h"

#include "s3mtypes.h"

/* general module information */

EXTERN_LINK bool             PUBLIC_DATA mod_isLoaded;
EXTERN_LINK modTitle_t       PUBLIC_DATA mod_Title;
EXTERN_LINK modTrackerName_t PUBLIC_DATA mod_TrackerName;

/* module options */

EXTERN_LINK bool PUBLIC_DATA modOption_ST2Vibrato;
EXTERN_LINK bool PUBLIC_DATA modOption_ST2Tempo;
EXTERN_LINK bool PUBLIC_DATA modOption_AmigaSlides;
EXTERN_LINK bool PUBLIC_DATA modOption_SBfilter;
EXTERN_LINK bool PUBLIC_DATA modOption_CostumeFlag;
EXTERN_LINK bool PUBLIC_DATA modOption_VolZeroOptim;
EXTERN_LINK bool PUBLIC_DATA modOption_AmigaLimits;
EXTERN_LINK bool PUBLIC_DATA modOption_SignedData;
EXTERN_LINK bool PUBLIC_DATA modOption_Stereo;

/* play options */

EXTERN_LINK bool PUBLIC_DATA playOption_ST3Order;
EXTERN_LINK bool PUBLIC_DATA playOption_LoopSong;

/* instruments */

EXTERN_LINK instrumentsList_t *PUBLIC_DATA Instruments;
EXTERN_LINK uint16_t PUBLIC_DATA InsNum;

/* patterns */

EXTERN_LINK patternsList_t PUBLIC_DATA Pattern;
EXTERN_LINK uint16_t PUBLIC_DATA PatNum;
EXTERN_LINK uint16_t PUBLIC_DATA PatLength;

/* song arrangement */

EXTERN_LINK ordersList_t PUBLIC_DATA Order;
EXTERN_LINK uint16_t PUBLIC_DATA OrdNum;
EXTERN_LINK uint8_t  PUBLIC_DATA LastOrder;

#ifdef DEBUG
EXTERN_LINK uint16_t PUBLIC_DATA StartOrder;
#endif

EXTERN_LINK channelsList_t PUBLIC_DATA Channel;
EXTERN_LINK uint8_t PUBLIC_DATA UsedChannels;

/* initial state */

EXTERN_LINK uint8_t PUBLIC_DATA InitTempo;
EXTERN_LINK uint8_t PUBLIC_DATA InitSpeed;

/* play state */

EXTERN_LINK bool    PUBLIC_DATA EndOfSong;
EXTERN_LINK uint8_t PUBLIC_DATA CurTempo;
EXTERN_LINK uint8_t PUBLIC_DATA CurSpeed;
EXTERN_LINK uint8_t PUBLIC_DATA GVolume;
EXTERN_LINK uint8_t PUBLIC_DATA MVolume;

/* position in song - you can change it while playing to jump arround */

EXTERN_LINK uint8_t PUBLIC_DATA CurOrder;
EXTERN_LINK uint8_t PUBLIC_DATA CurPattern;
EXTERN_LINK uint8_t PUBLIC_DATA CurLine;
EXTERN_LINK uint8_t PUBLIC_DATA CurTick;

/* pattern loop */

EXTERN_LINK bool    PUBLIC_DATA PLoop_On;
EXTERN_LINK uint8_t PUBLIC_DATA PLoop_No;
EXTERN_LINK uint8_t PUBLIC_DATA PLoop_To;

/* pattern delay */

EXTERN_LINK uint8_t PUBLIC_DATA PatternDelay;

/* EMM */

EXTERN_LINK bool     PUBLIC_DATA UseEMS;
EXTERN_LINK uint16_t PUBLIC_DATA PatEMSHandle;
EXTERN_LINK uint16_t PUBLIC_DATA SmpEMSHandle;
EXTERN_LINK bool     PUBLIC_DATA EMSPat;
EXTERN_LINK bool     PUBLIC_DATA EMSSmp;
EXTERN_LINK uint8_t  PUBLIC_DATA PatPerPage;
