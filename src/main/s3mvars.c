/* s3mvars.c -- variables for s3m play.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/dos.h"
#include "dos/ems.h"
#include "main/s3mtypes.h"

#include "main/s3mvars.h"

// TODO: remove EXTERN_LINK, PUBLIC_DATA and PUBLIC_CODE macros when done.

#ifdef DEFINE_LOCAL_DATA

/* EMS */

bool PUBLIC_DATA UseEMS;

/* general module information */

bool             PUBLIC_DATA mod_isLoaded;
modTitle_t       PUBLIC_DATA mod_Title;
modTrackerName_t PUBLIC_DATA mod_TrackerName;

/* module options */

bool PUBLIC_DATA modOption_ST2Vibrato;
bool PUBLIC_DATA modOption_ST2Tempo;
bool PUBLIC_DATA modOption_AmigaSlides;
bool PUBLIC_DATA modOption_SBfilter;
bool PUBLIC_DATA modOption_CostumeFlag;
bool PUBLIC_DATA modOption_VolZeroOptim;
bool PUBLIC_DATA modOption_AmigaLimits;
bool PUBLIC_DATA modOption_SignedData;
bool PUBLIC_DATA modOption_Stereo;

/* play options */

bool PUBLIC_DATA playOption_ST3Order;
bool PUBLIC_DATA playOption_LoopSong;

/* instruments */

instrumentsList_t *PUBLIC_DATA Instruments;
uint16_t PUBLIC_DATA InsNum;
bool     PUBLIC_DATA EMSSmp;
uint16_t PUBLIC_DATA SmpEMSHandle;

/* patterns */

patternsList_t PUBLIC_DATA Pattern;
uint16_t PUBLIC_DATA patListCount;
uint16_t PUBLIC_DATA patListPatLength;
bool     PUBLIC_DATA patListUseEM;
uint16_t PUBLIC_DATA patListEMHandle;
uint8_t  PUBLIC_DATA patListPatPerEMPage;

/* song arrangement */

ordersList_t PUBLIC_DATA Order;
uint16_t PUBLIC_DATA OrdNum;
uint8_t  PUBLIC_DATA LastOrder;

#ifdef DEBUG
uint16_t PUBLIC_DATA StartOrder;
#endif

channelsList_t PUBLIC_DATA Channel;
uint8_t PUBLIC_DATA UsedChannels;

/* initial state */

uint8_t PUBLIC_DATA InitTempo;
uint8_t PUBLIC_DATA InitSpeed;

/* play state */

bool    PUBLIC_DATA EndOfSong;
uint8_t PUBLIC_DATA CurTempo;
uint8_t PUBLIC_DATA CurSpeed;
uint8_t PUBLIC_DATA GVolume;
uint8_t PUBLIC_DATA MVolume;

/* position in song - you can change it while playing to jump arround */

uint8_t PUBLIC_DATA CurOrder;
uint8_t PUBLIC_DATA CurPattern;
uint8_t PUBLIC_DATA CurLine;
uint8_t PUBLIC_DATA CurTick;

/* pattern loop */

bool    PUBLIC_DATA PLoop_On;
uint8_t PUBLIC_DATA PLoop_No;
uint8_t PUBLIC_DATA PLoop_To;

/* pattern delay */

uint8_t PUBLIC_DATA PatternDelay;

#endif  /* DEFINE_LOCAL_DATA */

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
        if (emsMap(patListEMHandle, logPage, physPage))
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
        return 16*emsGetHandleSize(patListEMHandle);
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
        emsFree(patListEMHandle);
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
