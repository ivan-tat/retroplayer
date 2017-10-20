/* readnote.c -- read and play pattern.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "dos/ems.h"
#include "main/s3mtypes.h"
#include "main/muspat.h"
#include "main/s3mvars.h"
#include "main/effvars.h"
#include "main/mixvars.h"
#include "main/mixer.h"
#include "main/effects.h"

#include "main/readnote.h"

// TODO: remove PUBLIC_CODE macros when done.

/**********************************************************************/

/* Pattern's descriptor for sequentional reading */
typedef struct patternDesc_t
{
    MUSPAT *pattern;
    unsigned char *data;
    unsigned int row;
    unsigned int channel;
    unsigned int offset;
};
typedef struct patternDesc_t PATDESC;

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

bool __near pattern_open(PATDESC *desc, MUSPAT *pat)
{
    if (pat)
    {
        desc->pattern = pat;
        if (muspat_is_EM_data(pat))
            desc->data = muspat_map_EM_data(pat);
        else
            desc->data = muspat_get_data(pat);
        if (!desc->data)
            return false;
        desc->row = 0;
        desc->channel = 0;
        desc->offset = 0;
        return true;
    }
    else
        return false;
}

void __near pattern_seekRow(PATDESC *desc, unsigned int row)
{
    desc->row = row;
    desc->channel = 0;
    desc->offset = 5 * UsedChannels * row;
}

void __near pattern_close(PATDESC *desc)
{
    desc->pattern = (void *)0;
    desc->data = (void *)0;
    desc->row = 0;
    desc->channel = 0;
    desc->offset = 0;
}

/**********************************************************************/

bool __near pat_playNextChannel(PATDESC *desc, MIXCHN *chn)
{
    unsigned char *patData;
    unsigned int patOffset;
    struct instrument_t *ins;
    unsigned int cmd;
    unsigned char param;

    if (desc->channel >= UsedChannels)
        return false;

    patData = desc->data;
    patOffset = desc->offset;

    chnState_porta_flag = false;
    chnState_patDelay_bCommandSaved = mixchn_get_command(chn);

    if (playState_patDelay_bNow)
    {
        chnState_cur_bNote = CHNNOTE_EMPTY;
        chnState_cur_bIns  = 0;
        chnState_cur_bVol  = CHNINSVOL_EMPTY;
    }
    else
    {
        chnState_cur_bNote = patData[patOffset];
        chnState_cur_bIns  = patData[patOffset + 1];
        chnState_cur_bVol  = patData[patOffset + 2];
    };

    /* read effects - it may change the read instr/note ! */

    cmd = patData[patOffset + 3];
    if (cmd > MAXEFF)
        cmd = EFFIDX_NONE;
    param = patData[patOffset + 4];

    desc->channel++;
    desc->offset += 5;

    chn->bEffFlags = 0; /* important! */

    if (cmd != EFFIDX_NONE)
    {
        if (mixchn_get_command(chn) == cmd)
        {
            if (chn_effCanContinue(chn))
                chn->bEffFlags |= EFFFLAG_CONTINUE;
        };
    }
    else
    {
        if (mixchn_get_command(chn) != EFFIDX_NONE)
            chn_effStop(chn);
    };

    mixchn_set_command(chn, cmd);
    mixchn_set_sub_command(chn, 0);

    if (chn_effInit(chn, param))
    {
        /* read instrument */
        /* reinit instrument data and keep sample position */
        if (chnState_cur_bIns)
        {
            ins = insList_get(chnState_cur_bIns);
            if (ins_isSample(ins))
                chn_setupInstrument(chn, chnState_cur_bIns);
            else
                chnState_cur_bIns = 0;
        };
        /* read note */
        if (_isNote(chnState_cur_bNote))
            chn_setupNote(chn, chnState_cur_bNote, chnState_porta_flag);
        else
            if (chnState_cur_bNote == CHNNOTE_OFF)
                mixchn_set_playing(chn, false);
        /* read volume */
        if (_isVolume(chnState_cur_bVol))
        {
            chnState_cur_bVol = chnState_cur_bVol > CHNINSVOL_MAX ?
                CHNINSVOL_MAX : chnState_cur_bVol;
            mixchn_set_sample_volume(chn, (chnState_cur_bVol * playState_gVolume) >> 6);
        };
        chn_effHandle(chn);
    };

    return desc->channel < UsedChannels;
};

bool __near pat_skipNextChannel(PATDESC *desc)
{
    if (desc->channel >= UsedChannels)
        return false;

    desc->channel++;
    desc->offset += 5;

    return desc->channel < UsedChannels;
};

/**********************************************************************/

PATFLOWSTATE __near pat_playRow(MUSPAT *pat)
{
    PATDESC patDesc;
    unsigned char i;
    MIXCHN *chn;

    if (!pat)
        return FLOWSTATE_ROWEND;

    if (!pattern_open(&patDesc, pat))
        return FLOWSTATE_ROWEND;

    pattern_seekRow(&patDesc, playState_row);

    for (i = 0; i < UsedChannels; i++)
    {
        chn = &Channel[i];
        if (mixchn_get_type(chn) <= 2)
        {
            if (!pat_playNextChannel(&patDesc, chn))
                break;
        }
        else
        {
            if (!pat_skipNextChannel(&patDesc))
                break;
        };
    };

    if (playState_gVolume_bFlag)
        playState_gVolume = playState_gVolume_bValue;

    playState_tick = playState_speed;

    // Pattern break ?
    if (playState_patBreak_bFlag)
    {
        playState_row = playState_patBreak_bPos;
        pattern_close(&patDesc);
        return FLOWSTATE_PATTERNEND;
    };

    // Pattern loop ?
    if (playState_patLoop_bNow)
    {
        playState_patLoopCount--;
        if (playState_patLoopCount)
        {
            playState_row = playState_patLoopStartRow;
            pattern_close(&patDesc);
            return FLOWSTATE_WAIT;
        }
        else
        {
            playState_patLoopStartRow = playState_row + 1;
            playState_patLoopActive = false;
        };
    };

    pattern_close(&patDesc);
    return FLOWSTATE_ROWEND;
}

/**********************************************************************/

void PUBLIC_CODE readnewnotes(void)
{
    char firstPlay;
    PATFLOWSTATE status;
    unsigned int patIndex;
    MUSPAT *pat;

    firstPlay = true;
    status = FLOWSTATE_SONGLOOP;

    while (status != FLOWSTATE_WAIT)
    {
        switch (status)
        {
            case FLOWSTATE_ROWEND:
                playState_row++;
                if (playState_row < 64)
                    status = FLOWSTATE_WAIT;
                else
                {
                    playState_row = 0;
                    status = FLOWSTATE_PATTERNEND;
                };
                break;

            case FLOWSTATE_PATTERNEND:
                playState_order++;
                status = FLOWSTATE_PATTERNJUMP;
                break;

            case FLOWSTATE_PATTERNJUMP:
                playState_patLoopStartRow = 0;
                if (playState_order > LastOrder)
                    status = FLOWSTATE_SONGSTOP;
                else
                    status = FLOWSTATE_SONGLOOP;
                break;

            case FLOWSTATE_SONGLOOP:
                patIndex = Order[playState_order];
                if (patIndex >= 254)
                    status = FLOWSTATE_PATTERNEND;
                else
                {
                    playState_pattern = patIndex;
                    if (firstPlay)
                    {
                        firstPlay = false;
                        playState_jumpToOrder_bFlag = false;
                        playState_patBreak_bFlag = false;
                        playState_gVolume_bFlag = false;
                        playState_patLoop_bNow = false;
                        playState_patDelay_bNow = playState_patDelayCount != 0;
                        pat = patList_get(mod_Patterns, patIndex);
                        status = pat_playRow(pat);
                    }
                    else
                        status = FLOWSTATE_WAIT;
                };
                break;

            case FLOWSTATE_SONGSTOP:
                if (playOption_LoopSong)
                {
                    playState_order = 0;
                    status = FLOWSTATE_SONGLOOP;
                }
                else
                {
                    playState_songEnded = true;
                    status = FLOWSTATE_WAIT;
                };
                break;

            default:
                status = FLOWSTATE_WAIT;
                break;
        };
    };
}
