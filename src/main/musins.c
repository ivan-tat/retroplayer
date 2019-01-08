/* musins.c -- musical instrument handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/stdio.h"
#include "debug.h"
#include "cc/dos.h"
#include "dynarray.h"
#include "dos/ems.h"
#include "main/pcmsmp.h"

#include "main/musins.h"

/*** Musical instrument ***/

#define _musins_set_title(o, v) strncpy (_musins_get_title (o), v, MUSINS_TITLE_LEN)
#define _musins_clear_title(o)  memset (_musins_get_title (o), 0, MUSINS_TITLE_LEN)

void __far musins_init (MUSINS *self)
{
    if (self)
    {
        memset (self, 0, sizeof (MUSINS));
        //_musins_set_type (self, MUSINST_EMPTY);
    }
}

void __far musins_set_title (MUSINS *self, char *value)
{
    if (self)
        if (value)
            _musins_set_title (self, value);
        else
            _musins_clear_title (self);
}

void __far musins_free (MUSINS *self)
{
    PCMSMP *smp;

    if (self)
    {
        if (musins_get_type (self) == MUSINST_PCM)
        {
            smp = musins_get_sample (self);
            pcmsmp_free (smp);
            pcmsmp_init (smp);  // clear
        }
        musins_init (self); // clear
    }
}

/*** Musical instruments list ***/

void __far _musinsl_init_item (void *self, void *item)
{
    musins_init ((MUSINS *) item);
}

void __far _musinsl_free_item (void *self, void *item)
{
    musins_free ((MUSINS *) item);
}

void __far musinsl_init (MUSINSLIST *self)
{
    if (self)
        dynarr_init (_musinsl_get_list (self), self, sizeof (MUSINS), _musinsl_init_item, _musinsl_free_item);
}

void __far musinsl_free (MUSINSLIST *self)
{
    if (self)
        dynarr_free (_musinsl_get_list (self));
}

/*** Debug ***/

#if DEBUG == 1

void __far DEBUG_dump_instrument_info (MUSINS *self, uint8_t index, PCMSMPLIST *samples)
{
    #define _BUF_SIZE 8
    char *type;
    char s[_BUF_SIZE];
    int link;

    if (DEBUG)
    {
        switch (_musins_get_type (self))
        {
        case MUSINST_EMPTY:
            type = "none";
            link = -1;
            break;
        case MUSINST_PCM:
            type = "sample";
            link = pcmsmpl_indexof (samples, _musins_get_sample (self));
            break;
        default:
            type = "unknown";
            link = -1;
            break;
        };
        snprintf (s, _BUF_SIZE, "%hd", link);
        DEBUG_INFO_ (
            "DEBUG_dump_instrument_info",
            "index=%hu, type=%s (index=%s), volume=%hu, title='%s'",
            (uint8_t) index,
            (char *) type,
            (char *) s,
            (uint8_t) _musins_get_note_volume (self),
            (char *) _musins_get_title (self)
        );
    }
    #undef _BUF_SIZE
}

#endif  // DEBUG
