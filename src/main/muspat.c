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
#include "dynarray.h"
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

void __far _muspatl_clear_item(void *self, void *item)
{
    pat_clear((MUSPAT *)item);
}

void __far _muspatl_free_item(void *self, void *item)
{
    patFree((MUSPAT *)item);
}

MUSPATLIST *PUBLIC_CODE patList_new(void)
{
    uint16_t seg;
    MUSPATLIST *self;
            
    if (!_dos_allocmem(_dos_para(sizeof(MUSPATLIST)), &seg))
    {
        self = MK_FP(seg, 0);
        _dynarr_init(&(self->list), self, sizeof(MUSPAT), _muspatl_clear_item, _muspatl_free_item);
        return self;
    }
    else
        return NULL;
}

void PUBLIC_CODE patList_clear(MUSPATLIST *self)
{
    if (self)
        _dynarr_clear_list(&(self->list));
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


void PUBLIC_CODE patList_set(MUSPATLIST *self, uint16_t index, MUSPAT *item)
{
    if (self)
        _dynarr_set_item(&(self->list), index, item);
}

MUSPAT *PUBLIC_CODE patList_get(MUSPATLIST *self, uint16_t index)
{
    if (self)
        return _dynarr_get_item(&(self->list), index);
    else
        return NULL;
}

bool PUBLIC_CODE patList_set_count(MUSPATLIST *self, uint16_t count)
{
    if (self)
        return _dynarr_set_size(&(self->list), count); 
    else
        return false;
}

uint16_t PUBLIC_CODE patList_get_count(MUSPATLIST *self)
{
    if (self)
        return _dynarr_get_size(&(self->list));
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
        _dynarr_free(&(self->list));

        if (self->useEM)
        {
            emsFree(self->handle);
            self->useEM = false;
        }
    }
}
