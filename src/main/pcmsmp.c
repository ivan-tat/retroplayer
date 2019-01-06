/* pcmsmp.c -- PCM sample handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "debug.h"
#include "cc/dos.h"
#include "cc/stdio.h"
#include "dos/ems.h"
#include "main/s3mtypes.h"

#include "main/pcmsmp.h"

/*** PCM sample ***/

PCMSMPFLAGS __far __pcmsmp_set_flags (PCMSMPFLAGS _flags, PCMSMPFLAGS _mask, PCMSMPFLAGS _set, bool raise)
{
    PCMSMPFLAGS result;

    result = _flags & _mask;
    if (raise)
        result |= _set;

    return result;
}

#define _pcmsmp_set_title(o, v) strncpy (_pcmsmp_get_title (o), v, PCMSMP_TITLE_LEN)
#define _pcmsmp_clear_title(o)  memset (_pcmsmp_get_title (o), 0, PCMSMP_TITLE_LEN)

void __far pcmsmp_init (PCMSMP *self)
{
    if (self)
    {
        memset (self, 0, sizeof (PCMSMP));
        _pcmsmp_set_EM_data_handle (self, EMSBADHDL);
    }
}

void *__far pcmsmp_map_EM_data (PCMSMP *self)
{
    EMSHDL handle;
    uint16_t page;
    int count, i;

    if (_pcmsmp_is_EM_data (self))
    {
        handle = _pcmsmp_get_EM_data_handle (self);
        page = _pcmsmp_get_EM_data_page (self);
        count = pcmsmp_get_EM_data_pages_count (self);
        for (i = 0; i < count; i++)
        {
            if (!emsMap (handle, page, i))
                return NULL;
            page++;
        }
        return _pcmsmp_get_EM_data (self);
    }
    else
        return NULL;
}

void __far pcmsmp_set_title (PCMSMP *self, char *value)
{
    if (self)
        if (value)
            _pcmsmp_set_title (self, value);
        else
            _pcmsmp_clear_title (self);
}

char *__far pcmsmp_get_title (PCMSMP *self)
{
    if (self)
        return _pcmsmp_get_title (self);
    else
        return NULL;
}

void __far pcmsmp_free (PCMSMP *self)
{
    void *data;

    if (_pcmsmp_is_EM_data (self))
    {
        if (_pcmsmp_is_own_EM_handle (self))
            emsFree (_pcmsmp_get_EM_data_handle (self));
    }
    else
    {
        data = _pcmsmp_get_data (self);
        if (data)
            _dos_freemem (FP_SEG (data));
    }
}

/*** PCM samples list ***/

PCMSMPLFLAGS __far __pcmsmpl_set_flags (PCMSMPLFLAGS _flags, PCMSMPLFLAGS _mask, PCMSMPLFLAGS _set, bool raise)
{
    PCMSMPLFLAGS result;

    result = _flags & _mask;
    if (raise)
        result |= _set;

    return result;
}

void __far _pcmsmpl_init_item (void *self, void *item)
{
    pcmsmp_init ((PCMSMP *) item);
}

void __far _pcmsmpl_free_item (void *self, void *item)
{
    pcmsmp_free ((PCMSMP *) item);
}

void __far pcmsmpl_init (PCMSMPLIST *self)
{
    if (self)
    {
        memset (self, 0, sizeof (PCMSMPLIST));
        dynarr_init (_pcmsmpl_get_list (self), self, sizeof (PCMSMP), _pcmsmpl_init_item, _pcmsmpl_free_item);
        _pcmsmpl_set_EM_handle (self, EMSBADHDL);
    }
}

void __far pcmsmpl_free (PCMSMPLIST *self)
{
    if (self)
    {
        dynarr_free (_pcmsmpl_get_list (self));

        if (_pcmsmpl_is_own_EM_handle (self))
            emsFree (_pcmsmpl_get_EM_handle (self));

        pcmsmpl_init (self);    // clear
    }
}

/*** Debug ***/

#if DEBUG == 1

void __far DEBUG_dump_sample_info (PCMSMP *smp, uint8_t index)
{
    #define _BUF_SIZE 80
    char place[_BUF_SIZE];
    void *data;

    if (DEBUG)
    {
        if (_pcmsmp_is_EM_data (smp))
        {
            snprintf (place, _BUF_SIZE,
                    "EM, handle=0x%04X(%s), page=0x%04X(%d):0x%04X",
                _pcmsmp_get_EM_data_handle (smp),
                _pcmsmp_is_own_EM_handle (smp) ? "priv" : "glob",
                _pcmsmp_get_EM_data_page (smp),
                _pcmsmp_get_EM_data_pages_count (smp),
                _pcmsmp_get_EM_data_offset (smp)
            );
        }
        else
        {
            data = _pcmsmp_get_data (smp);
            snprintf (place, _BUF_SIZE, "DOS, data=0x%04X:0x%04X",
                FP_SEG (data),
                FP_OFF (data)
            );
        }
        /* hint: s=sample a=available v=volume b=bits r=rate s=size l=loop p=place t=title */
        DEBUG_INFO_ (
            "DEBUG_dump_sample_info",
            "s=%2d, a=%hhu, v=%2hu, b=%hhu, r=%5u, s=%5u, l=%hhu:%5u-%5u, p=%s, t='%s'",
            (uint8_t) index,
            (uint8_t) _pcmsmp_is_available (smp),
            (uint8_t) _pcmsmp_get_volume (smp),
            (uint8_t) _pcmsmp_get_bits (smp),
            (uint16_t) _pcmsmp_get_rate (smp),
            (uint16_t) _pcmsmp_get_size (smp),
            (uint8_t) _pcmsmp_get_loop (smp),
            (uint16_t) _pcmsmp_get_loop_start (smp),
            (uint16_t) _pcmsmp_get_loop_end (smp),
            (char *) place,
            (char *) _pcmsmp_get_title (smp)
        );
    }
    #undef _BUF_SIZE
}

#endif  // DEBUG
