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

patternsList_t PUBLIC_DATA mod_Patterns;
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

void PUBLIC_CODE patSetDataInEM(MUSPAT *pat, uint8_t page, uint8_t part)
{
    _patSetDataInEM(pat, page, part);
}

bool PUBLIC_CODE patIsDataInEM(MUSPAT *pat)
{
    return _patIsDataInEM(pat);
}

void *PUBLIC_CODE patGetData(MUSPAT *pat)
{
    if (_patIsDataInEM(pat))
        return _patGetDataInEM(pat, patListPatLength);
    else
        return MK_FP(pat->data_seg, 0);
}

void *PUBLIC_CODE patMapData(MUSPAT *pat)
{
    unsigned int logPage;
    unsigned char physPage;

    if (_patIsDataInEM(pat))
    {
        logPage = _patGetDataEMPage(pat);
        physPage = 0;
        if (emsMap(patListEMHandle, logPage, physPage))
            return _patGetDataInEM(pat, patListPatLength);
        else
            return MK_FP(0, 0);
    }
    else
        return MK_FP(pat->data_seg, 0);;
}

uint8_t PUBLIC_CODE patGetDataEMPage(MUSPAT *pat)
{
    return _patGetDataEMPage(pat);
}

uint8_t PUBLIC_CODE patGetDataEMPart(MUSPAT *pat)
{
    return _patGetDataEMPart(pat);
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
        mod_Patterns[index].data_seg = pat->data_seg;
    else
        pat_clear(&(mod_Patterns[index]));
}

MUSPAT *PUBLIC_CODE patList_get(int16_t index)
{
    if ((index >= 0) && (index < MAX_PATTERNS))
        return &(mod_Patterns[index]);
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
        pat_clear(&(mod_Patterns[i]));
}

void PUBLIC_CODE patListDone(void)
{
    patListFree();
}
