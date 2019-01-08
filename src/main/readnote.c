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

void __near _play_channel (MIXCHN *chn, MUSPATCHNEVENT *event)
{
    MUSMOD *track;
    MUSINSLIST *instruments;
    MUSINS *ins;
    unsigned char cmd, param;

    chnState_porta_flag = false;
    chnState_patDelay_bCommandSaved = mixchn_get_command(chn);

    if (playState_patDelay_bNow)
    {
        chnState_cur_bIns  = CHNINS_EMPTY;
        chnState_cur_bNote = CHNNOTE_EMPTY;
        chnState_cur_bVol  = CHNVOL_EMPTY;
    }
    else
    {
        chnState_cur_bIns  = event->data.instrument;
        chnState_cur_bNote = event->data.note;
        chnState_cur_bVol  = event->data.volume;
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
            if (chn_effCanContinue(chn))
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

    if (chn_effInit(chn, param))
    {
        track = mod_Track;
        instruments = musmod_get_instruments (track);

        /* read instrument */
        /* reinit instrument data and keep sample position */
        if (_isInstrument (chnState_cur_bIns))
        {
            ins = musinsl_get (instruments, _unpackInstrument (chnState_cur_bIns));
            if (musins_get_type(ins) == MUSINST_PCM)
                chn_setupInstrument(chn, chnState_cur_bIns);
            else
                chnState_cur_bIns = CHNINS_EMPTY;
        }
        /* read note */
        if (_isNote(chnState_cur_bNote))
            chn_setupNote(chn, chnState_cur_bNote, chnState_porta_flag);
        else
            if (chnState_cur_bNote == CHNNOTE_OFF)
                mixchn_set_playing(chn, false);
        /* read volume */
        if (_isVolume(chnState_cur_bVol))
        {
            chnState_cur_bVol = chnState_cur_bVol > CHNVOL_MAX ? CHNVOL_MAX : chnState_cur_bVol;
            mixchn_set_note_volume (chn, chnState_cur_bVol);
        }
        chn_effHandle(chn);
    }
}

void __near _play_event (MUSPATROWEVENT *e)
{
    MIXCHNLIST *channels;
    MIXCHN *chn;

    channels = mod_Channels;
    chn = mixchnl_get (channels, e->channel);

    if (mixchn_get_type (chn) == MIXCHNTYPE_PCM)
        _play_channel (chn, & (e->event));
}

bool __near _play_row (MUSPAT *pat, uint16_t row)
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
            _play_event (&empty);
            c++;
        }

        if (row_ev_ok)
        {
            _play_event (&e);
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
    playState_row++;

    if (playState_row < muspat_get_rows (state->pat))
        state->status = FLOWSTATE_WAIT;
    else
    {
        playState_row = 0;
        state->status = FLOWSTATE_PATTERNEND;
    }
}

void __near on_pattern_end (TRACKSTATE *state)
{
    playState_order++;

    state->status = FLOWSTATE_PATTERNJUMP;
}

void __near on_pattern_jump (TRACKSTATE *state)
{
    playState_patLoopStartRow = 0;

    if (playState_order > LastOrder)
        state->status = FLOWSTATE_SONGSTOP;
    else
        state->status = FLOWSTATE_SONGLOOP;
}

void __near on_track_loop (TRACKSTATE *state)
{
    MUSMOD *track;
    MUSPATLIST *patterns;
    unsigned int i;

    track = mod_Track;

    i = Order[playState_order];

    if (i >= 254)
    {
        state->status = FLOWSTATE_PATTERNEND;
        return;
    }
    else
    {
        playState_pattern = i;

        if (state->firstPlay)
        {
            state->firstPlay = false;
            playState_jumpToOrder_bFlag = false;
            playState_patBreak_bFlag = false;
            playState_gVolume_bFlag = false;
            playState_patLoop_bNow = false;
            playState_patDelay_bNow = playState_patDelayCount != 0;
            patterns = musmod_get_patterns (track);
            state->pat = muspatl_get (patterns, i);

            if (!_play_row (state->pat, playState_row))
            {
                state->status = FLOWSTATE_ROWEND;
                return;
            }

            if (playState_gVolume_bFlag)
                playState_gVolume = playState_gVolume_bValue;

            playState_tick = playState_speed;

            // Pattern break ?
            if (playState_patBreak_bFlag)
            {
                playState_row = playState_patBreak_bPos;
                state->status = FLOWSTATE_PATTERNEND;
                return;
            }

            // Pattern loop ?
            if (playState_patLoop_bNow)
            {
                playState_patLoopCount--;
                if (playState_patLoopCount)
                {
                    playState_row = playState_patLoopStartRow;
                    state->status = FLOWSTATE_WAIT;
                    return;
                }
                else
                {
                    playState_patLoopStartRow = playState_row + 1;
                    playState_patLoopActive = false;
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
        playState_order = 0;
        state->status = FLOWSTATE_SONGLOOP;
    }
    else
    {
        playState_songEnded = true;
        state->status = FLOWSTATE_WAIT;
    }
}

void __far readnewnotes (void)
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
                on_track_loop (& ts);
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
