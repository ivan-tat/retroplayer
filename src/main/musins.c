/* musins.c -- musical instrument handling library.

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

#include "main/musins.h"

/*** Musical instrument ***/

void PUBLIC_CODE musins_init(MUSINS *self)
{
    if (self)
        memset(self, 0, sizeof(MUSINS));
}

void PUBLIC_CODE musins_set_type(MUSINS *self, MUSINSTYPE value)
{
    if (self)
        self->bType = value;
}

MUSINSTYPE PUBLIC_CODE musins_get_type(MUSINS *self)
{
    if (self)
        return self->bType;
    else
        return MUSINST_EMPTY;
}

void PUBLIC_CODE musins_set_looped(MUSINS *self, bool value)
{
    if (self)
    {
        if (value)
            self->flags |= SMPFLAG_LOOP;
        else
            self->flags &= ~SMPFLAG_LOOP;
    }
}

bool PUBLIC_CODE musins_is_looped(MUSINS *self)
{
    if (self)
        return (self->flags & SMPFLAG_LOOP) != 0;
    else
        return false;
}

void PUBLIC_CODE musins_set_EM_data(MUSINS *self, bool value)
{
    if (self)
    {
        if (value)
            self->memseg |= 0xf000;
        else
            self->memseg &= ~0xf000;
    }
}

bool PUBLIC_CODE musins_is_EM_data(MUSINS *self)
{
    if (self)
        return self->memseg >= 0xf000;
    else
        return false;
}

void PUBLIC_CODE musins_set_EM_data_page(MUSINS *self, uint16_t value)
{
    if (self)
        self->memseg = (self->memseg & 0xf000) + (value & 0x0fff);
}

uint16_t PUBLIC_CODE musins_get_EM_data_page(MUSINS *self)
{
    if (self)
        return self->memseg & 0x0fff;
    else
        return 0;
}

void PUBLIC_CODE musins_set_data(MUSINS *self, void *value)
{
    if (self)
        self->memseg = FP_SEG(value);
}

void *PUBLIC_CODE musins_get_data(MUSINS *self)
{
    if (self)
    {
        if (musins_is_EM_data(self))
            return emsFramePtr;
        else
            return MK_FP(self->memseg, 0);
    }
    else
        return NULL;
}

void *PUBLIC_CODE musins_map_EM_data(MUSINS *self)
{
    uint16_t page;
    uint8_t physPage, count;

    if (self)
    {
        if (musins_is_EM_data(self))
        {
            page = musins_get_EM_data_page(self);
            physPage = 0;
            count = (uint16_t)(((uint32_t)musins_get_length(self) + 0x3fff) >> 1) >> 13;
            while (count--)
                if (!emsMap(SmpEMSHandle, page++, physPage++))
                    return NULL;
        }
        return musins_get_data(self);
    }
    else
        return NULL;
}

void PUBLIC_CODE musins_set_length(MUSINS *self, uint32_t value)
{
    if (self)
        self->slength = value;
}

uint32_t PUBLIC_CODE musins_get_length(MUSINS *self)
{
    if (self)
        return self->slength;
    else
        return 0;
}

void PUBLIC_CODE musins_set_loop_start(MUSINS *self, uint32_t value)
{
    if (self)
        self->loopbeg = value;
}

uint32_t PUBLIC_CODE musins_get_loop_start(MUSINS *self)
{
    if (self)
        return self->loopbeg;
    else
        return 0;
}

void PUBLIC_CODE musins_set_loop_end(MUSINS *self, uint32_t value)
{
    if (self)
        self->loopend = value;
}

uint32_t PUBLIC_CODE musins_get_loop_end(MUSINS *self)
{
    if (self)
        return self->loopend;
    else
        return 0;
}

void PUBLIC_CODE musins_set_volume(MUSINS *self, uint8_t value)
{
    if (self)
        self->vol = value;
}

uint8_t PUBLIC_CODE musins_get_volume(MUSINS *self)
{
    if (self)
        return self->vol;
    else
        return 0;
}

void PUBLIC_CODE musins_set_rate(MUSINS *self, uint32_t value)
{
    if (self)
        self->c2speed = value;
}

uint32_t PUBLIC_CODE musins_get_rate(MUSINS *self)
{
    if (self)
        return self->c2speed;
    else
        return 0;
}

void PUBLIC_CODE musins_set_title(MUSINS *self, char *value)
{
    if (self)
        strncpy(self->IName, value, MUSINS_TITLE_LENGTH_MAX);
}

char *PUBLIC_CODE musins_get_title(MUSINS *self)
{
    if (self)
        return self->IName;
    else
        return NULL;
}

void PUBLIC_CODE musins_free(MUSINS *self)
{
    void *data;

    if (self)
    {
        if (musins_get_type(self) == MUSINST_PCM)
        {
            if (!musins_is_EM_data(self))
            {
                data = musins_get_data(self);
                if (data)
                {
                    _dos_freemem(FP_SEG(data));
                    musins_set_data(self, NULL);
                }
            }
        }
        musins_set_type(self, MUSINST_EMPTY);
    }
}

/*** Musical instruments list ***/

static EMSNAME EMS_INSLIST_HANDLE_NAME = "inslist";

MUSINSLIST *PUBLIC_CODE musinsl_new(void)
{
    uint16_t seg;

    if (!_dos_allocmem(_dos_para(MAX_INSTRUMENTS * sizeof(MUSINS)), &seg))
        return MK_FP(seg, 0);
    else
        return NULL;
}

void PUBLIC_CODE musinsl_init(MUSINSLIST *self)
{
    uint16_t i;

    if (self)
        for (i = 0; i < MAX_INSTRUMENTS; i++)
            musins_set_type(musinsl_get(self, i), MUSINST_EMPTY);
}

void PUBLIC_CODE musinsl_set_EM_data(MUSINSLIST *self, bool value)
{
    if (self)
        EMSSmp = value;
}

bool PUBLIC_CODE musinsl_is_EM_data(MUSINSLIST *self)
{
    if (self)
        return EMSSmp;
    else
        return false;
}

MUSINS *PUBLIC_CODE musinsl_get(MUSINSLIST *self, uint16_t index)
{
    if (self)
        return (MUSINS *)MK_FP(FP_SEG(self), FP_OFF(self) + index * sizeof(MUSINS));
    else
        return NULL;
}

void PUBLIC_DATA musinsl_set_EM_data_handle(MUSINSLIST *self, EMSHDL value)
{
    if (self)
        SmpEMSHandle = value;
}

void PUBLIC_CODE musinsl_set_EM_handle_name(MUSINSLIST *self)
{
    if (self)
        emsSetHandleName(SmpEMSHandle, &EMS_INSLIST_HANDLE_NAME);
}

uint32_t PUBLIC_CODE musinsl_get_used_EM(MUSINSLIST *self)
{
    if (EMSSmp)
        return (uint32_t) emsGetHandleSize(SmpEMSHandle) << 4;
    else
        return 0;
}

void PUBLIC_CODE musinsl_free(MUSINSLIST *self)
{
    uint16_t i;

    for (i = 0; i < MAX_INSTRUMENTS; i++)
        musins_free(musinsl_get(mod_Instruments, i));

    if (EMSSmp)
    {
        emsFree(SmpEMSHandle);
        SmpEMSHandle = EMSBADHDL;
        EMSSmp = false;
    }
}

void PUBLIC_CODE musinsl_delete(MUSINSLIST **self)
{
    if (self)
        if (*self)
        {
            _dos_freemem(FP_SEG(*self));
            *self = NULL;
        }
}

#ifdef DEFINE_LOCAL_DATA

/* instruments */

instrumentsList_t *PUBLIC_DATA mod_Instruments;
uint16_t PUBLIC_DATA InsNum;
bool     PUBLIC_DATA EMSSmp;
uint16_t PUBLIC_DATA SmpEMSHandle;

#endif  /* DEFINE_LOCAL_DATA */
