/* musmod.c -- musical module handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "common.h"
#include "cc/string.h"
#include "main/pcmsmp.h"
#include "main/musins.h"
#include "main/muspat.h"

#include "main/musmod.h"

MUSMODFLAGS __far __musmod_set_flags (MUSMODFLAGS _flags, MUSMODFLAGS _mask, MUSMODFLAGS _set, bool raise)
{
    if (raise)
        return (_flags & _mask) | _set;
    else
        return _flags & _mask;
}

#define _musmod_set_title(o, v)     strncpy (_musmod_get_title (o), v, MUSMOD_TITLE_LEN)
#define _musmod_clear_title(o)      memset (_musmod_get_title (o), 0, MUSMOD_TITLE_LEN)
#define _musmod_set_format(o, v)    strncpy (_musmod_get_format (o), v, MUSMOD_FORMAT_LEN)
#define _musmod_clear_format(o)     memset (_musmod_get_format (o), 0, MUSMOD_FORMAT_LEN)

void __far musmod_init (MUSMOD *self)
{
    if (self)
    {
        memset (self, 0, sizeof (MUSMOD));
        memset (_musmod_get_channels (self), MUSMODCHNPAN_CENTER, sizeof (MUSMODCHN) * MUSMOD_CHANNELS_MAX);
        pcmsmpl_init (_musmod_get_samples (self));      // clear
        musinsl_init (_musmod_get_instruments (self));  // clear
        muspatl_init (_musmod_get_patterns (self));     // clear
    }
}

void __far musmod_set_title (MUSMOD *self, char *value)
{
    if (self)
        if (value)
            _musmod_set_title (self, value);
        else
            _musmod_clear_title (self);
}

void __far musmod_set_format (MUSMOD *self, char *value)
{
    if (self)
        if (value)
            _musmod_set_format (self, value);
        else
            _musmod_clear_format (self);
}

void __far musmod_free (MUSMOD *self)
{
    if (self)
    {
        musinsl_free (_musmod_get_instruments (self));
        pcmsmpl_free (_musmod_get_samples (self));
        muspatl_free (_musmod_get_patterns (self));

        musmod_init (self); // clear
    }
}

/*** Variables ***/

#ifdef DEFINE_LOCAL_DATA

MUSMOD *mod_Track;

#endif  /* DEFINE_LOCAL_DATA */

