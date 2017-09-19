/* readnote.c -- read and play pattern.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#include <i86.h>
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "pascal/pascal.h"
#include "dos/emstool.h"
#include "main/s3mtypes.h"
#include "main/s3mvars.h"
#include "main/effvars.h"
#include "main/mixvars.h"
#include "main/mixer.h"
#include "main/effects.h"
#include "main/readnote.h"

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
    void *data;
    if (pat)
    {
        data = (void *)0;
        if (pat->data_seg)
            data = patMapData(pat);
        if (!data)
            return false;
        desc->pattern = pat;
        desc->data = data;
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
    chnState_patDelay_bCommandSaved = chn_getCommand(chn);

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
        if (chn_getCommand(chn) == cmd)
        {
            if (chn_effCanContinue(chn))
                chn->bEffFlags |= EFFFLAG_CONTINUE;
        };
    }
    else
    {
        if (chn_getCommand(chn) != EFFIDX_NONE)
            chn_effStop(chn);
    };

    chn_setCommand(chn, cmd);
    chn_setSubCommand(chn, 0);

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
                chn_setState(chn, false);
        /* read volume */
        if (_isVolume(chnState_cur_bVol))
        {
            chnState_cur_bVol = chnState_cur_bVol > CHNINSVOL_MAX ?
                CHNINSVOL_MAX : chnState_cur_bVol;
            chn_setSampleVolume(chn, (chnState_cur_bVol * GVolume) >> 6);
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

    pattern_seekRow(&patDesc, CurLine);

    for (i = 0; i < UsedChannels; i++)
    {
        chn = &Channel[i];
        if (chn->bChannelType <= 2)
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
        GVolume = playState_gVolume_bValue;

    CurTick = CurSpeed;

    // Pattern break ?
    if (playState_patBreak_bFlag)
    {
        CurLine = playState_patBreak_bPos;
        pattern_close(&patDesc);
        return FLOWSTATE_PATTERNEND;
    };

    // Pattern loop ?
    if (playState_patLoop_bNow)
    {
        PLoop_No--;
        if (PLoop_No)
        {
            CurLine = PLoop_To;
            pattern_close(&patDesc);
            return FLOWSTATE_WAIT;
        }
        else
        {
            PLoop_To = CurLine + 1;
            PLoop_On = false;
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
                CurLine++;
                if (CurLine < 64)
                    status = FLOWSTATE_WAIT;
                else
                {
                    CurLine = 0;
                    status = FLOWSTATE_PATTERNEND;
                };
                break;

            case FLOWSTATE_PATTERNEND:
                CurOrder++;
                status = FLOWSTATE_PATTERNJUMP;
                break;

            case FLOWSTATE_PATTERNJUMP:
                PLoop_To = 0;
                if (CurOrder > LastOrder)
                    status = FLOWSTATE_SONGSTOP;
                else
                    status = FLOWSTATE_SONGLOOP;
                break;

            case FLOWSTATE_SONGLOOP:
                patIndex = Order[CurOrder];
                if (patIndex >= 254)
                    status = FLOWSTATE_PATTERNEND;
                else
                {
                    CurPattern = patIndex;
                    if (firstPlay)
                    {
                        firstPlay = false;
                        playState_jumpToOrder_bFlag = false;
                        playState_patBreak_bFlag = false;
                        playState_gVolume_bFlag = false;
                        playState_patLoop_bNow = false;
                        playState_patDelay_bNow = PatternDelay != 0;
                        pat = patList_get(patIndex);
                        status = pat_playRow(pat);
                    }
                    else
                        status = FLOWSTATE_WAIT;
                };
                break;

            case FLOWSTATE_SONGSTOP:
                if (playOption_LoopSong)
                {
                    CurOrder = 0;
                    status = FLOWSTATE_SONGLOOP;
                }
                else
                {
                    EndOfSong = true;
                    status = FLOWSTATE_WAIT;
                };
                break;

            default:
                status = FLOWSTATE_WAIT;
                break;
        };
    };
}
