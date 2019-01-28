/* readnote.c -- read and play pattern.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "dos/ems.h"
#include "main/s3mtypes.h"
#include "main/mixchn.h"
#include "main/muspat.h"
#include "main/musmod.h"
#include "main/s3mvars.h"
#include "main/effvars.h"
#include "main/mixer.h"
#include "main/effects.h"

#include "main/readnote.h"

/**********************************************************************/

/* Pattern flow state & Song flow state */
// TODO: Split Pattern & Song flow state
typedef unsigned char patternFlowState_t;
typedef patternFlowState_t PATFLOWSTATE;

// Pattern
#define FLOWSTATE_WAIT        0
#define FLOWSTATE_ROWEND      1
#define FLOWSTATE_PATTERNEND  2
#define FLOWSTATE_PATTERNJUMP 3
// Song
#define FLOWSTATE_SONGLOOP    4
#define FLOWSTATE_SONGSTOP    5

/**********************************************************************/

void __near _play_channel (MUSMOD *track, ROWSTATE *rs, MIXCHN *chn, MUSPATCHNEVENT *event)
{
    MUSINSLIST *instruments;
    MUSINS *ins;
    unsigned char cmd, param;
    CHNSTATE _cs, *cs;

    cs = &_cs;
    cs->flags &= ~CHNSTATEFL_PORTAMENTO;
    cs->patdelay_saved_command = mixchn_get_command (chn);

    if (rs->flags & ROWSTATEFL_PATTERN_DELAY)
    {
        cs->cur_instrument  = CHN_INS_NONE;
        cs->cur_note        = CHN_NOTE_NONE;
        cs->cur_note_volume = CHN_NOTEVOL_NONE;
    }
    else
    {
        cs->cur_instrument  = event->data.instrument;
        cs->cur_note        = event->data.note;
        cs->cur_note_volume = event->data.note_volume;
    }

    /* read effects - it may change the read instr/note ! */

    cmd = event->data.command;
    if (cmd > MAXEFF)
        cmd = EFFIDX_NONE;
    param = event->data.parameter;

    chn->bEffFlags = 0; /* important! */

    if (cmd != EFFIDX_NONE)
    {
        if (mixchn_get_command(chn) == cmd)
        {
            if (chn_effCanContinue (chn, cs))
                chn->bEffFlags |= EFFFLAG_CONTINUE;
        }
    }
    else
    {
        if (mixchn_get_command(chn) != EFFIDX_NONE)
            chn_effStop(chn);
    }

    mixchn_set_command(chn, cmd);
    mixchn_set_sub_command(chn, 0);

    if (chn_effInit (rs, chn, cs, param))
    {
        instruments = musmod_get_instruments (track);

        /* read instrument */
        /* reinit instrument data and keep sample position */
        if (((cs->cur_instrument) != CHN_INS_NONE)
        &&  ((cs->cur_instrument) <= CHN_INS_MAX))
        {
            ins = musinsl_get (instruments, _get_instrument (cs->cur_instrument));
            if (musins_get_type(ins) == MUSINST_PCM)
                chn_setupInstrument (chn, cs->cur_instrument);
            else
                cs->cur_instrument = CHN_INS_NONE;
        }
        /* read note */
        if (((cs->cur_note) != CHN_NOTE_OFF)
        &&  ((cs->cur_note) != CHN_NOTE_NONE))
            chn_setupNote (chn, cs->cur_note, (cs->flags & CHNSTATEFL_PORTAMENTO) != 0);
        else
            if (cs->cur_note == CHN_NOTE_OFF)
                mixchn_set_playing(chn, false);
        /* read volume */
        if (cs->cur_note_volume != CHN_NOTEVOL_NONE)
        {
            if (cs->cur_note_volume > CHN_NOTEVOL_MAX)
                cs->cur_note_volume = CHN_NOTEVOL_MAX;
            mixchn_set_note_volume (chn, cs->cur_note_volume);
        }
        chn_effHandle (chn, cs);
    }
}

void __near _play_event (MUSMOD *track, ROWSTATE *rs, MUSPATROWEVENT *e)
{
    MIXCHNLIST *channels;
    MIXCHN *chn;

    channels = mod_Channels;
    chn = mixchnl_get (channels, e->channel);

    if (mixchn_get_type (chn) == MIXCHNTYPE_PCM)
        _play_channel (track, rs, chn, & (e->event));
}

bool __near _play_row (MUSMOD *track, ROWSTATE *rs, MUSPAT *pat, uint16_t row)
{
    MUSPATIO f;
    MUSPATROWEVENT e, empty;
    MIXCHNLIST *channels;
    unsigned int num_channels;
    unsigned char c, next_c;
    bool row_read, row_ev_ok;

    if ((!pat) || (!muspatio_open (&f, pat, MUSPATIOMD_READ)))
        return false;

    muspatio_seek (&f, row, 0);

    channels = mod_Channels;
    num_channels = mixchnl_get_count (channels);

    /* Linear reading of pattern's events while increasing channel number */
    muspatrowevent_clear (&empty);
    c = 0;
    row_read = !muspatio_is_end_of_row (&f);
    while (c < num_channels)
    {
        // walk through from current channel (c) to the end
        row_ev_ok = false;
        next_c = num_channels;

        if (row_read)
        {
            muspatio_read (&f, &e);
            if (e.channel < num_channels)
            {
                // walk through from current channel (c) to current event's channel
                row_ev_ok = true;
                next_c = e.channel;
                row_read = !muspatio_is_end_of_row (&f);
            }
        }

        /* walk through from channel (c) to (next_c)  */
        while (c < next_c)
        {
            empty.channel = c;
            _play_event (track, rs, &empty);
            c++;
        }

        if (row_ev_ok)
        {
            _play_event (track, rs, &e);
            c++;
        }
    }

    muspatio_close (&f);

    return true;
}

/**********************************************************************/

typedef struct track_state_t
{
    PATFLOWSTATE status;
    bool firstPlay;
    MUSPAT *pat;
};
typedef struct track_state_t TRACKSTATE;

void __near on_row_end (TRACKSTATE *state)
{
    playState.row++;

    if (playState.row < muspat_get_rows (state->pat))
        state->status = FLOWSTATE_WAIT;
    else
    {
        playState.row = 0;
        state->status = FLOWSTATE_PATTERNEND;
    }
}

void __near on_pattern_end (TRACKSTATE *state)
{
    playState.order++;

    state->status = FLOWSTATE_PATTERNJUMP;
}

void __near on_pattern_jump (TRACKSTATE *state)
{
    playState.patloop_start_row = 0;

    if (playState.order > LastOrder)
        state->status = FLOWSTATE_SONGSTOP;
    else
        state->status = FLOWSTATE_SONGLOOP;
}

void __near on_track_loop (MUSMOD *track, TRACKSTATE *state)
{
    MUSPATLIST *patterns;
    MUSPATORDER *order;
    MUSPATORDENT *order_entry;
    ROWSTATE _rs, *rs;
    unsigned int i;

    order = musmod_get_order (track);
    order_entry = muspatorder_get (order, playState.order);
    rs = &_rs;

    i = *order_entry;

    if ((i == MUSPATORDENT_SKIP)
    ||  (i == MUSPATORDENT_END))
    {
        state->status = FLOWSTATE_PATTERNEND;
        return;
    }
    else
    {
        playState.pattern = i;

        if (state->firstPlay)
        {
            state->firstPlay = false;
            rs->flags = 0;
            if (playState.patdelay_count)
                rs->flags |= ROWSTATEFL_PATTERN_DELAY;
            patterns = musmod_get_patterns (track);
            state->pat = muspatl_get (patterns, i);

            if (!_play_row (track, rs, state->pat, playState.row))
            {
                state->status = FLOWSTATE_ROWEND;
                return;
            }

            if (rs->flags & ROWSTATEFL_GLOBAL_VOLUME)
                playState.global_volume = rs->global_volume;

            playState.tick = playState.speed;

            // Pattern break ?
            if (rs->flags & ROWSTATEFL_PATTERN_BREAK)
            {
                playState.row = rs->break_pos;
                state->status = FLOWSTATE_PATTERNEND;
                return;
            }

            // Pattern loop ?
            if (rs->flags & ROWSTATEFL_PATTERN_LOOP)
            {
                playState.patloop_count--;
                if (playState.patloop_count)
                {
                    playState.row = playState.patloop_start_row;
                    state->status = FLOWSTATE_WAIT;
                    return;
                }
                else
                {
                    playState.patloop_start_row = playState.row + 1;
                    playState.flags &= ~PLAYSTATEFL_PATLOOP;
                }
            }

            state->status = FLOWSTATE_ROWEND;
        }
        else
            state->status = FLOWSTATE_WAIT;
    }
}

void __near on_track_stop (TRACKSTATE *state)
{
    if (playOption_LoopSong)
    {
        playState.order = 0;
        state->status = FLOWSTATE_SONGLOOP;
    }
    else
    {
        playState.flags |= PLAYSTATEFL_END;
        state->status = FLOWSTATE_WAIT;
    }
}

void __far readnewnotes (MUSMOD *track)
{
    TRACKSTATE ts;

    ts.status = FLOWSTATE_SONGLOOP;
    ts.firstPlay = true;

    while (ts.status != FLOWSTATE_WAIT)
    {
        switch (ts.status)
        {
            case FLOWSTATE_ROWEND:
                on_row_end (& ts);
                break;

            case FLOWSTATE_PATTERNEND:
                on_pattern_end (& ts);
                break;

            case FLOWSTATE_PATTERNJUMP:
                on_pattern_jump (& ts);
                break;

            case FLOWSTATE_SONGLOOP:
                on_track_loop (track, & ts);
                break;

            case FLOWSTATE_SONGSTOP:
                on_track_stop (& ts);
                break;

            default:
                ts.status = FLOWSTATE_WAIT;
                break;
        }
    }
}
