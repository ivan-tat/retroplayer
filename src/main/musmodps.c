/* musmodps.c -- music module play state handling methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$main$musmodps$*"
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/string.h"
#include "common.h"
#include "dynarray.h"
#include "debug.h"
#include "main/s3mtypes.h"
#include "main/musmod.h"
#include "main/mixchn.h"
#include "main/musmodps.h"

PLAYSTATEFLAGS __far __playstate_set_flags (PLAYSTATEFLAGS _flags, PLAYSTATEFLAGS _mask, PLAYSTATEFLAGS _set, bool _raise)
{
    if (_raise)
        return (_flags & _mask) | _set;
    else
        return _flags & _mask;
}

void __far playstate_init (PLAYSTATE *self)
{
    if (self)
        memset (self, 0, sizeof (PLAYSTATE));
    else
        DEBUG_ERR ("self is NULL!");
}

bool __far playstate_alloc_channels (PLAYSTATE *self)
{
    MUSMOD *track;
    MIXCHNLIST *channels;
    uint8_t num_channels;

    if (self)
    {
        track = self->track;
        if (track && musmod_is_loaded (track))
        {
            channels = self->channels;
            if (!channels)
            {
                channels = _new (MIXCHNLIST);
                if (!channels)
                {
                    DEBUG_ERR_ ("Failed to allocate memory for %s.", "mixing channels object");
                    return false;
                }
                mixchnl_init (channels);
                self->channels = channels;
            }

            num_channels = musmod_get_channels_count (track);

            if (mixchnl_get_count (channels) != num_channels)
                if (!mixchnl_set_count (channels, musmod_get_channels_count (track)))
                {
                    DEBUG_ERR_ ("Failed to allocate memory for %s.", "mixing channels");
                    return false;
                }

            return true;
        }
        else
        {
            DEBUG_ERR ("Track is not set or loaded.");
            return false;
        }
    }
    else
    {
        DEBUG_ERR ("self is NULL!");
        return false;
    }
}

void __far playstate_reset_channels (PLAYSTATE *self)
{
    MUSMOD *track;
    MIXCHNLIST *channels;
    uint8_t num_channels, num_channels2, i;
    MIXCHN *chn;
    MIXCHNTYPE type;
    MIXCHNPAN pan;
    MIXCHNFLAGS flags;

    if (self)
    {
        track = self->track;
        if (track && musmod_is_loaded (track))
        {
            channels = self->channels;
            if (channels)
            {
                num_channels = musmod_get_channels_count (track);
                num_channels2 = mixchnl_get_count (channels);
                if (num_channels > num_channels2)
                    num_channels = num_channels2;

                for (i = 0; i < num_channels; i++)
                {
                    chn = mixchnl_get (channels, i);
                    pan = musmod_get_channels (track)[i].pan & MUSMODCHNPANFL_PAN_MASK;

                    if (musmod_get_channels (track)[i].pan & MUSMODCHNPANFL_ENABLED)
                    {
                        type = MIXCHNTYPE_PCM;
                        flags = MIXCHNFL_ENABLED | MIXCHNFL_MIXING;
                    }
                    else
                    {
                        type = MIXCHNTYPE_NONE;
                        flags = 0;
                    }

                    mixchn_set_type (chn, type);
                    mixchn_set_pan (chn, pan);
                    mixchn_set_flags (chn, flags);
                    if (mixchn_get_type (chn) != MIXCHNTYPE_NONE)
                        mixchn_reset_wave_tables (chn);
                }
            }
            else
                DEBUG_ERR ("No mixing channels.");
        }
        else
            DEBUG_ERR ("Track is not set or loaded.");
    }
    else
        DEBUG_ERR ("self is NULL!");
}

void __far playstate_free_channels (PLAYSTATE *self)
{
    if (self)
    {
        if (self->channels)
        {
            mixchnl_free (self->channels);
            _delete (self->channels);
        }
    }
    else
        DEBUG_ERR ("self is NULL!");
}

void __far playstate_set_speed (PLAYSTATE *self, uint8_t value)
{
    if (self)
    {
        if (value > 0)
            self->speed = value;
    }
    else
        DEBUG_ERR ("self is NULL!");
}

void __far playstate_set_tempo (PLAYSTATE *self, uint8_t value)
{
    if (self)
    {
        if (value >= 32)
            self->tempo = value;
        else
            value = self->tempo;

        if (value)
            self->tick_samples_per_channel = (long)self->rate * 5 / (int)(value * 2);
    }
    else
        DEBUG_ERR ("self is NULL!");
}

void __far playstate_setup_patterns_order (PLAYSTATE *self)
{
    MUSMOD *track;
    int i;

    if (self)
    {
        track = self->track;
        if (track && musmod_is_loaded (track))
            i = muspatorder_find_last (musmod_get_order (track), self->flags & PLAYSTATEFL_SKIPENDMARK);
        else
            i = 0;

        self->order_last = i;
    }
    else
        DEBUG_ERR ("self is NULL!");
}

int __far playstate_find_next_pattern (PLAYSTATE *self, int index, int step)
{
    MUSMOD *track;
    MUSPATORDER *order;
    int start, last, pos;
    bool skipend;

    if (self)
    {
        track = self->track;
        if (track && musmod_is_loaded (track))
        {
            order = musmod_get_order (track);
            start = self->order_start;
            last = self->order_last;
            pos = index;
            skipend =  self->flags & PLAYSTATEFL_SKIPENDMARK;

            // Check bounds

            if ((step < 0) && (pos <= start))
                // Rewind
                return muspatorder_find_next_pattern (order, start, last, start, 1, skipend);

            if ((step > 0) && (pos >= self->order_last))
            {
                if (self->flags & PLAYSTATEFL_SONGLOOP)
                    // Rewind
                    return muspatorder_find_next_pattern (order, start, last, start, 1, skipend);
                else
                    // Stop
                    return -1;
            }

            pos = muspatorder_find_next_pattern (order, start, last, pos + step, step, skipend);

            if (pos < 0)
            {
                if ((step < 0) || (self->flags & PLAYSTATEFL_SONGLOOP))
                    // Rewind
                    return muspatorder_find_next_pattern (order, start, last, start, 1, skipend);
            }

            return pos;
        }
        else
        {
            DEBUG_ERR ("Track is not set or loaded.");
            return -1;
        }
    }
    else
    {
        DEBUG_ERR ("self is NULL!");
        return -1;
    }
}

void __far playstate_set_pos (PLAYSTATE *self, uint8_t start_order, uint8_t start_row, bool keep)
{
    MUSMOD *track;
    MUSPATORDER *order;
    MUSPATORDENT *order_entry;

    if (self)
    {
        track = self->track;
        if (track && musmod_is_loaded (track))
        {
            order = musmod_get_order (track);

            self->order = start_order;                  // next order to read from
            order_entry = muspatorder_get (order, start_order);
            self->pattern = *order_entry;               // next pattern to read from
            self->row = start_row;                      // next row to read from
            self->tick = 1;                             // last tick (go to next row)
            self->tick_samples_per_channel_left = 0;    // immediately next tick

            if (!keep)
            {
                // reset pattern effects:
                self->patdelay_count = 0;
                self->flags &= ~PLAYSTATEFL_PATLOOP;
                self->patloop_count = 0;
                self->patloop_start_row = 0;
            }
        }
        else
            DEBUG_ERR ("Track is not set or loaded.");
    }
    else
        DEBUG_ERR ("self is NULL!");
}

void __far playstate_set_initial_state (PLAYSTATE *self)
{
    MUSMOD *track;

    if (self)
    {
        track = self->track;
        if (track && musmod_is_loaded (track))
        {
            playstate_set_tempo (self, musmod_get_tempo (track));   // first priority (is output mixer-dependant)
            playstate_set_speed (self, musmod_get_speed (track));   // second priority (is song's internal value)
            self->global_volume = musmod_get_global_volume (track); // is song's internal value
            self->master_volume = musmod_get_master_volume (track); // is song's output
        }
        else
            DEBUG_ERR ("Track is not set or loaded.");
    }
    else
        DEBUG_ERR ("self is NULL!");
}

void __far playstate_free (PLAYSTATE *self)
{
    if (self)
        playstate_free_channels (self);
    else
        DEBUG_ERR ("self is NULL!");
}
