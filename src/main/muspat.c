/* muspat.c -- musical pattern handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "dos/ems.h"

#include "main/muspat.h"

#ifdef DEFINE_LOCAL_DATA

/* patterns */

patternsList_t PUBLIC_DATA Pattern;
uint16_t PUBLIC_DATA patListCount;
uint16_t PUBLIC_DATA patListPatLength;
bool     PUBLIC_DATA patListUseEM;
uint16_t PUBLIC_DATA patListEMHandle;
uint8_t  PUBLIC_DATA patListPatPerEMPage;

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
