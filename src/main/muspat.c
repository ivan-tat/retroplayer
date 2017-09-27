/* muspat.c -- musical pattern handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dos.h"
#include "dos/ems.h"

#include "main/muspat.h"

#ifdef DEFINE_LOCAL_DATA

extern MUSPATLIST *PUBLIC_DATA mod_Patterns;

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
        return _patGetDataInEM(pat, patListGetPatLength(mod_Patterns));
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
        if (emsMap(patListGetHandle(mod_Patterns), logPage, physPage))
            return _patGetDataInEM(pat, patListGetPatLength(mod_Patterns));
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

static EMSNAME EMS_PATLIST_HANDLE_NAME = "patlist";

MUSPATLIST *PUBLIC_CODE patList_new(void)
{
    uint16_t seg;
            
    if (!_dos_allocmem(_dos_para(sizeof(MUSPATLIST)), &seg))
        return MK_FP(seg, 0);
    else
        return NULL;
}

void PUBLIC_CODE patList_clear(MUSPATLIST *self)
{
    if (self)
        if (self->list && self->count)
            memset(self->list, 0, self->count * sizeof(MUSPAT));
}

void PUBLIC_CODE patList_delete(MUSPATLIST **self)
{           
    if (self)
        if (*self)
        {
            _dos_freemem(FP_SEG(*self));
            *self = NULL;
        }
}


void PUBLIC_CODE patList_set(MUSPATLIST *self, int16_t index, MUSPAT *item)
{
    if (self)
        if (self->list && index < self->count)
        {
            if (item)
                self->list[index].data_seg = item->data_seg;
            else
                pat_clear(&(self->list[index]));
        }
}

MUSPAT *PUBLIC_CODE patList_get(MUSPATLIST *self, int16_t index)
{
    if (self)
        if (self->list && index >= 0 && index < self->count)
            return &(self->list[index]);

    return NULL;
}

bool PUBLIC_CODE patList_set_count(MUSPATLIST *self, uint16_t count)
{
    uint16_t seg, max;
    unsigned int size;
    bool result;
    unsigned int i;

    if (self)
    {
        if (self->count != count)
        {
            size = _dos_para(count * sizeof(MUSPAT));
            if (count > self->count)
            {
                // Grow
                if (self->count)
                    result = !_dos_setblock(size, FP_SEG(self->list), &max);
                else
                {

                    result = !_dos_allocmem(size, &seg);
                    if (result)
                        self->list = MK_FP(seg, 0);
                    else
                        self->list = NULL;
                }

                if (result)
                {
                    memset(&(self->list[self->count]), 0, (count - self->count) * sizeof(MUSPAT));
                    self->count = count;
                }
            }
            else
            {
                // Shrink
                for (i = count; i < self->count; i++)
                    patFree(&(self->list[i]));
                if (count)
                    _dos_setblock(size, FP_SEG(self->list), &max);
                else
                {
                    _dos_freemem(FP_SEG(self->list));
                    self->list = NULL;
                }
                self->count = count;
                result = true;
            }
        }
        else
            result = true;
    }
    else
        result = false;

    return result;
}

uint16_t PUBLIC_CODE patList_get_count(MUSPATLIST *self)
{
    if (self)
        return self->count;
    else
        return 0;
}

void PUBLIC_CODE patListSetPatLength(MUSPATLIST *self, uint16_t value)
{
    if (self)
        self->patLength = value;
}

uint16_t PUBLIC_CODE patListGetPatLength(MUSPATLIST *self)
{
    if (self)
        return self->patLength;
    else
        return 0;
}

void PUBLIC_CODE patListSetUseEM(MUSPATLIST *self, bool value)
{
    if (self)
        self->useEM = value;
}

bool PUBLIC_CODE patListIsInEM(MUSPATLIST *self)
{
    if (self)
        return self->useEM;
    else
        return false;
}

void PUBLIC_CODE patListSetHandle(MUSPATLIST *self, EMSHDL value)
{
    if (self)
        self->handle = value;
}

EMSHDL PUBLIC_CODE patListGetHandle(MUSPATLIST *self)
{
    if (self)
        return self->handle;
    else
        return 0;
}

void PUBLIC_CODE patListSetHandleName(MUSPATLIST *self)
{
    if (self)
        if (self->useEM)
            emsSetHandleName(self->handle, EMS_PATLIST_HANDLE_NAME);
}

void PUBLIC_CODE patListSetPatPerPage(MUSPATLIST *self, uint8_t value)
{
    if (self)
        self->patPerPage = value;
}

uint8_t PUBLIC_CODE patListGetPatPerPage(MUSPATLIST *self)
{
    if (self)
        return self->patPerPage;
    else
        return 0;
}

uint32_t PUBLIC_CODE patListGetUsedEM(MUSPATLIST *self)
{
    if (self)
        if (self->useEM)
            return 16 * emsGetHandleSize(self->handle);

    return 0;
}

void PUBLIC_CODE patListFree(MUSPATLIST *self)
{
    uint16_t count;
    int i;
    MUSPAT *pat;

    if (self)
    {
        if (self->list)
        {
            count = self->count;
            for (i = 0; i < count; i++)
            {
                pat = patList_get(self, i);
                patFree(pat);
            }
            _dos_freemem(FP_SEG(self->list));
        }

        if (self->useEM)
        {
            emsFree(self->handle);
            self->useEM = false;
        }
    }
}
