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

/*** Music pattern ***/

#define _muspat_set_EM_data(o, v)        o->flags = (o->flags & ~MUSPATFL_EM) | (v ? MUSPATFL_EM : 0)
#define _muspat_is_EM_data(o)            ((o->flags & MUSPATFL_EM) != 0)
#define _muspat_set_own_EM_handle(o, v)  o->flags = (o->flags & ~MUSPATFL_OWNHDL) | (v ? MUSPATFL_OWNHDL : 0)
#define _muspat_is_own_EM_handle(o)      ((o->flags & MUSPATFL_OWNHDL) != 0)
#define _muspat_set_channels(o, v)       o->channels = v
#define _muspat_get_channels(o)          o->channels
#define _muspat_set_rows(o, v)           o->rows = v
#define _muspat_get_rows(o)              o->rows
#define _muspat_set_size(o, v)           o->size = v
#define _muspat_get_size(o)              o->size
#define _muspat_set_data(o, v)           { o->data_off = FP_OFF(v); o->data_seg = FP_SEG(v); }
#define _muspat_get_data(o)              MK_FP(o->data_seg, o->data_off)
#define _muspat_set_EM_data_handle(o, v) o->handle = v
#define _muspat_get_EM_data_handle(o)    o->handle
#define _muspat_set_EM_data_page(o, v)   o->data_seg = v
#define _muspat_get_EM_data_page(o)      o->data_seg
#define _muspat_set_EM_data_offset(o, v) o->data_off = v
#define _muspat_get_EM_data_offset(o)    o->data_off
#define _muspat_get_EM_data(o)           MK_FP(emsFrameSeg, o->data_off)

void PUBLIC_CODE muspat_clear(MUSPAT *self)
{
    if (self)
    {
        memset(self, 0, sizeof(MUSPAT));
        _muspat_set_EM_data_handle(self, EMSBADHDL);
    }
}

void PUBLIC_CODE muspat_set_EM_data(MUSPAT *self, bool value)
{
    if (self)
        _muspat_set_EM_data(self, value);
}

bool PUBLIC_CODE muspat_is_EM_data(MUSPAT *self)
{
    if (self)
        return _muspat_is_EM_data(self);
    else
        return false;
}

void PUBLIC_CODE muspat_set_own_EM_handle(MUSPAT *self, bool value)
{
    if (self)
        _muspat_set_own_EM_handle(self, value);
}

bool PUBLIC_CODE muspat_is_own_EM_handle(MUSPAT *self)
{
    if (self)
        return _muspat_is_own_EM_handle(self);
    else
        return false;
}

void PUBLIC_CODE muspat_set_channels(MUSPAT *self, uint8_t value)
{
    if (self)
        _muspat_set_channels(self, value);
}

uint8_t PUBLIC_CODE muspat_get_channels(MUSPAT *self)
{
    if (self)
        return _muspat_get_channels(self);
    else
        return 0;
}

void PUBLIC_CODE muspat_set_rows(MUSPAT *self, uint8_t value)
{
    if (self)
        _muspat_set_rows(self, value);
}

uint8_t PUBLIC_CODE muspat_get_rows(MUSPAT *self)
{
    if (self)
        return _muspat_get_rows(self);
    else
        return 0;
}

void PUBLIC_CODE muspat_set_size(MUSPAT *self, uint16_t value)
{
    if (self)
        _muspat_set_size(self, value);
}

uint16_t PUBLIC_CODE muspat_get_size(MUSPAT *self)
{
    if (self)
        return _muspat_get_size(self);
    else
        return 0;
}

void PUBLIC_CODE muspat_set_data(MUSPAT *self, void *value)
{
    if (self)
        _muspat_set_data(self, value);
}

void PUBLIC_CODE muspat_set_EM_data_handle(MUSPAT *self, EMSHDL value)
{
    if (self)
        _muspat_set_EM_data_handle(self, value);
}

EMSHDL PUBLIC_CODE muspat_get_EM_data_handle(MUSPAT *self)
{
    if (self)
        return _muspat_get_EM_data_handle(self);
    else
        return EMSBADHDL;
}

void PUBLIC_CODE muspat_set_EM_data_page(MUSPAT *self, uint16_t value)
{
    if (self)
        _muspat_set_EM_data_page(self, value);
}

uint16_t PUBLIC_CODE muspat_get_EM_data_page(MUSPAT *self)
{
    if (self)
        return _muspat_get_EM_data_page(self);
    else
        return 0;
}

void PUBLIC_CODE muspat_set_EM_data_offset(MUSPAT *self, uint16_t value)
{
    if (self)
        _muspat_set_EM_data_offset(self, value);
}

uint16_t PUBLIC_CODE muspat_get_EM_data_offset(MUSPAT *self)
{
    if (self)
        return _muspat_get_EM_data_offset(self);
    else
        return 0;
}

void *PUBLIC_CODE muspat_get_data(MUSPAT *self)
{
    if (self)
    {
        if (_muspat_is_EM_data(self))
            return _muspat_get_EM_data(self);
        else
            return _muspat_get_data(self);
    }
    else
        return NULL;
}

void *PUBLIC_CODE muspat_map_EM_data(MUSPAT *self)
{
    if (self)
    {
        if (_muspat_is_EM_data(self))
            if (emsMap(_muspat_get_EM_data_handle(self), _muspat_get_EM_data_page(self), 0))
                return _muspat_get_EM_data(self);
    }

    return NULL;
}

void PUBLIC_CODE muspat_free(MUSPAT *self)
{
    void *p;

    if (self)
    {
        if (_muspat_is_EM_data(self))
        {
            if (_muspat_is_own_EM_handle(self))
                emsFree(_muspat_get_EM_data_handle(self));
        }
        else
        {
            p = _muspat_get_data(self);
            if (p)
                _dos_freemem(FP_SEG(p));
        }

        muspat_clear(self);
    }
}

/*** Patterns list ***/

static EMSNAME EMS_PATLIST_HANDLE_NAME = "patlist";

void __far _muspatl_clear_item(void *self, void *item)
{
    muspat_clear((MUSPAT *)item);
}

void __far _muspatl_free_item(void *self, void *item)
{
    muspat_free((MUSPAT *)item);
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

#ifdef DEFINE_LOCAL_DATA

extern MUSPATLIST *PUBLIC_DATA mod_Patterns;

#endif  /* DEFINE_LOCAL_DATA */
