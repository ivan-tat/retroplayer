/* s3mvars.c -- variables for s3m play.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove EXTERN_LINK, PUBLIC_DATA and PUBLIC_CODE macros when done.

#include "pascal/pascal.h"
#include "ow/dos_.h"
#include "dos/emstool.h"
#include "main/s3mtypes.h"
#include "main/s3mvars.h"

/* EMM */

EXTERN_LINK bool PUBLIC_DATA UseEMS;

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
EXTERN_LINK bool     PUBLIC_DATA EMSSmp;
EXTERN_LINK uint16_t PUBLIC_DATA SmpEMSHandle;

/* patterns */

EXTERN_LINK patternsList_t PUBLIC_DATA Pattern;
EXTERN_LINK uint16_t PUBLIC_DATA patListCount;
EXTERN_LINK uint16_t PUBLIC_DATA patListPatLength;
EXTERN_LINK bool     PUBLIC_DATA patListUseEM;
EXTERN_LINK uint16_t PUBLIC_DATA patListEMHandle;
EXTERN_LINK uint8_t  PUBLIC_DATA patListPatPerEMPage;

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

/*** Patterns ***/

void PUBLIC_CODE pat_clear(MUSPAT *pat)
{
    pat->data_seg = 0;
}

void PUBLIC_CODE patSetData(MUSPAT *pat, void *p)
{
    pat->data_seg = FP_SEG(p);
}

void PUBLIC_CODE patSetDataInEM(MUSPAT *pat, uint8_t logpage, uint8_t part)
{
    pat->data_seg = setPatternDataInEM(logpage, part);
}

bool PUBLIC_CODE patIsDataInEM(MUSPAT *pat)
{
    return isPatternDataInEM(pat->data_seg);
}

void *PUBLIC_CODE patGetData(MUSPAT *pat)
{
    uint16_t data_seg = pat->data_seg;
    if (isPatternDataInEM(data_seg))
        return getPatternDataInEM(data_seg, patListPatLength);
    else
        return MK_FP(data_seg, 0);
}

void *PUBLIC_CODE patMapData(MUSPAT *pat)
{
    uint16_t data_seg = pat->data_seg;
    unsigned int logPage;
    unsigned char physPage;
    if (isPatternDataInEM(data_seg))
    {
        logPage = getPatternDataLogPageInEM(data_seg);
        physPage = 0;
        if (EmsMap(patListEMHandle, logPage, physPage))
            return getPatternDataInEM(data_seg, patListPatLength);
        else
            return MK_FP(0, 0);
    }
    else
        return MK_FP(data_seg, 0);;
}

uint8_t PUBLIC_CODE patGetDataLogPageInEM(MUSPAT *pat)
{
    return getPatternDataLogPageInEM(pat->data_seg);
}

uint8_t PUBLIC_CODE patGetDataPartInEM(MUSPAT *pat)
{
    return getPatternDataPartInEM(pat->data_seg);
}

void PUBLIC_CODE patFree(MUSPAT *pat)
{
    void *p;

    if (!patIsDataInEM(pat))
    {
        p = patGetData(pat);
        if (p)
            _dos_freemem(FP_SEG(p));
        pat_clear(pat);
    };
}

/*** Patterns list ***/

void PUBLIC_CODE patList_set(int16_t index, MUSPAT *pat)
{
    if (pat)
        Pattern[index].data_seg = pat->data_seg;
    else
        pat_clear(&(Pattern[index]));
}

MUSPAT *PUBLIC_CODE patList_get(int16_t index)
{
    if ((index >= 0) && (index < MAX_PATTERNS))
        return &(Pattern[index]);
    else
        return (void *)0;
}

uint32_t PUBLIC_CODE patListGetUsedEM(void)
{
    if (patListUseEM)
        return 16*EmsGetHandleSize(patListEMHandle);
    else
        return 0;
}

void PUBLIC_CODE patListFree(void)
{
    int i;
    MUSPAT *pat;

    for (i = 0; i < MAX_PATTERNS; i++)
    {
        pat = patList_get(i);
        patFree(pat);
    };
    if (patListUseEM)
    {
        EmsFree(patListEMHandle);
        patListUseEM = false;
    };
}

void PUBLIC_CODE patListInit(void)
{
    int i;
    for (i = 0; i < MAX_PATTERNS; i++)
        pat_clear(&(Pattern[i]));
}

void PUBLIC_CODE patListDone(void)
{
    patListFree();
}
