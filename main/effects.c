/* effects.c -- effects handling.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <i86.h>
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "..\pascal\pascal.h"
#include "..\dos\emstool.h"
#include "s3mtypes.h"
#include "s3mvars.h"
#include "effvars.h"
#include "mixvars.h"
#include "mixer.h"
#include "effects.h"

#define CHNNOTE_EMPTY 0xff
#define CHNNOTE_OFF 0xfe

#define CHNINSVOL_EMPTY 0xff
#define CHNINSVOL_MAX 63

#define MID_C_RATE 8363

/*** Effects ***/

typedef bool __near effProcInit_t  (MIXCHN *chn, uint8_t param);
typedef void __near effProcHandle_t(MIXCHN *chn);
typedef void __near effProcTick_t  (MIXCHN *chn);

#define enInit(name)   eff_##name##_init
#define enHandle(name) eff_##name##_handle
#define enTick(name)   eff_##name##_tick

#define epInit(name)   bool __near enInit(name)  (MIXCHN *chn, uint8_t param)
#define epHandle(name) void __near enHandle(name)(MIXCHN *chn)
#define epTick(name)   void __near enTick(name)  (MIXCHN *chn)

typedef struct effRoutines_t
{
    effProcInit_t   __near *init;
    effProcHandle_t __near *handle;
    effProcTick_t   __near *tick;
};

// Effect description
#define enDesc(name) eff_##name##_desc
#define descEff(name) static const struct effRoutines_t enDesc(name)

// List of effect descriptions
#define enDescList(name) eff_##name##_list
#define descEffList(name) static const struct effRoutines_t __near *enDescList(name)[]

typedef struct effSubRoutines_t
{
    effProcHandle_t __near *handle;
    effProcTick_t   __near *tick;
};

// Sub-effect description
#define enSubDesc(name) effsub_##name##_desc
#define descSubEff(name) static const struct effSubRoutines_t enSubDesc(name)

// List of sub-effect descriptions
#define enSubDescList(name) effsub_##name##_list
#define descSubEffList(name) static const struct effSubRoutines_t __near *enSubDescList(name)[]

/*** No effect ***/

epInit  (none);
epHandle(none);
epTick  (none);
descEff(none) =
{
    enInit  (none),
    enHandle(none),
    enTick  (none)
};

// sub-effects

descSubEff(none) =
{
    enHandle(none),
    enTick  (none)
};

/*** A: Set speed ***/

epInit(setSpeed);
descEff(setSpeed) =
{
    enInit  (setSpeed),
    enHandle(none),
    enTick  (none)
};

/*** B: Jump to order ***/

epInit(jumpToOrder);
descEff(jumpToOrder) =
{
    enInit  (jumpToOrder),
    enHandle(none),
    enTick  (none)
};

/*** C: Pattern break ***/

epInit(patBreak);
descEff(patBreak) =
{
    enInit  (patBreak),
    enHandle(none),
    enTick  (none)
};

/*** D: Volume slide ***/

epInit(volSlide);
epTick(volSlide);
descEff(volSlide) =
{
    enInit  (volSlide),
    enHandle(none),
    enTick  (volSlide)
};

// sub-effects

epTick(volSlide_down);
descSubEff(volSlide_down) =
{
    enHandle(none),
    enTick  (volSlide_down)
};

epTick(volSlide_up);
descSubEff(volSlide_up) =
{
    enHandle(none),
    enTick  (volSlide_up)
};

// sub-effects list

#define EFFIDX_VOLSLIDE_DOWN      0
#define EFFIDX_VOLSLIDE_UP        1
#define EFFIDX_VOLSLIDE_FINE_DOWN 2
#define EFFIDX_VOLSLIDE_FINE_UP   3

descSubEffList(volSlide) =
{
    &enSubDesc(volSlide_down),
    &enSubDesc(volSlide_up),
    &enSubDesc(none),
    &enSubDesc(none)
};

/*** E: Pitch slide down ***/

epInit(pitchDown);
epTick(pitchDown);
descEff(pitchDown) =
{
    enInit  (pitchDown),
    enHandle(none),
    enTick  (pitchDown)
};

// sub-effects

epTick(pitchDown_down);
descSubEff(pitchDown_down) =
{
    enHandle(none),
    enTick  (pitchDown_down)
};

// sub-effects list

#define EFFIDX_PITCHDOWN_NORMAL 0
#define EFFIDX_PITCHDOWN_FINE   1
#define EFFIDX_PITCHDOWN_EXTRA  2

descSubEffList(pitchDown) =
{
    &enSubDesc(pitchDown_down),
    &enSubDesc(none),
    &enSubDesc(none)
};

/*** F: Pitch slide up ***/

epInit(pitchUp);
epTick(pitchUp);
descEff(pitchUp) =
{
    enInit  (pitchUp),
    enHandle(none),
    enTick  (pitchUp)
};

// sub-effects

epTick(pitchUp_up);
descSubEff(pitchUp_up) =
{
    enHandle(none),
    enTick  (pitchUp_up)
};

// sub-effects list

#define EFFIDX_PITCHUP_NORMAL 0
#define EFFIDX_PITCHUP_FINE   1
#define EFFIDX_PITCHUP_EXTRA  2

descSubEffList(pitchUp) =
{
    &enSubDesc(pitchUp_up),
    &enSubDesc(none),
    &enSubDesc(none)
};

/*** G: Portamento to note ***/

epInit(porta);
epTick(porta);
descEff(porta) =
{
    enInit  (porta),
    enHandle(none),
    enTick  (porta)
};

/*** H: Vibrato ***/

epInit(vibrato);
epTick(vibrato);
descEff(vibrato) =
{
    enInit  (vibrato),
    enHandle(none),
    enTick  (vibrato)
};

/*** I: Tremor ***/

epInit(tremor);
epTick(tremor);
descEff(tremor) =
{
    enInit  (tremor),
    enHandle(none),
    enTick  (tremor)
};

/*** J: Arpeggio ***/

epInit(arpeggio);
epTick(arpeggio);
descEff(arpeggio) =
{
    enInit  (arpeggio),
    enHandle(none),
    enTick  (arpeggio)
};

/*** K: Vibrato (H) + Volume slide (D) ***/

epInit(vib_volSlide);
epTick(vib_volSlide);
descEff(vib_volSlide) =
{
    enInit  (vib_volSlide),
    enHandle(none),
    enTick  (vib_volSlide)
};

/*** L: Portamento to note (G) + Volume slide (D) ***/

epInit(porta_volSlide);
epTick(porta_volSlide);
descEff(porta_volSlide) =
{
    enInit  (porta_volSlide),
    enHandle(none),
    enTick  (porta_volSlide)
};

/*** Q: Note retrigger + Volume slide ***/

epInit(retrig);
epTick(retrig);
descEff(retrig) =
{
    enInit  (retrig),
    enHandle(none),
    enTick  (retrig)
};

// sub-effects

epTick(retrig_slideDown);
descSubEff(retrig_slideDown) =
{
    enHandle(none),
    enTick  (retrig_slideDown)
};

epTick(retrig_use2div3);
descSubEff(retrig_use2div3) =
{
    enHandle(none),
    enTick  (retrig_use2div3)
};

epTick(retrig_use1div2);
descSubEff(retrig_use1div2) =
{
    enHandle(none),
    enTick  (retrig_use1div2)
};

epTick(retrig_slideUp);
descSubEff(retrig_slideUp) =
{
    enHandle(none),
    enTick  (retrig_slideUp)
};

epTick(retrig_use3div2);
descSubEff(retrig_use3div2) =
{
    enHandle(none),
    enTick  (retrig_use3div2)
};

epTick(retrig_use2div1);
descSubEff(retrig_use2div1) =
{
    enHandle(none),
    enTick  (retrig_use2div1)
};

// sub-effects list

#define EFFIDX_RETRIG_VOLSLIDE_NONE     0
#define EFFIDX_RETRIG_VOLSLIDE_DOWN     1
#define EFFIDX_RETRIG_VOLSLIDE_USE2DIV3 2
#define EFFIDX_RETRIG_VOLSLIDE_USE1DIV2 3
#define EFFIDX_RETRIG_VOLSLIDE_UP       4
#define EFFIDX_RETRIG_VOLSLIDE_USE3DIV2 5
#define EFFIDX_RETRIG_VOLSLIDE_USE2DIV1 6

descSubEffList(retrig) =
{
    &enSubDesc(none),
    &enSubDesc(retrig_slideDown),
    &enSubDesc(retrig_use2div3),
    &enSubDesc(retrig_use1div2),
    &enSubDesc(retrig_slideUp),
    &enSubDesc(retrig_use3div2),
    &enSubDesc(retrig_use2div1)
};

/*** R: Tremolo ***/

epInit(tremolo);
epTick(tremolo);
descEff(tremolo) =
{
    enInit  (tremolo),
    enHandle(none),
    enTick  (tremolo)
};

/*** S: Special effects ***/

epInit(special);
epTick(special);
descEff(special) =
{
    enInit  (special),
    enHandle(none),
    enTick  (special)
};

// sub-effects

epTick(special_noteCut);
descSubEff(special_noteCut) =
{
    enHandle(none),
    enTick  (special_noteCut)
};

epTick(special_noteDelay);
descSubEff(special_noteDelay) =
{
    enHandle(none),
    enTick  (special_noteDelay)
};

// sub-effects list

descSubEffList(special) =
{
    &enSubDesc(none),               /* S0 - n/a */
    &enSubDesc(none),               /* S1 - Set filter */
    &enSubDesc(none),               /* S2 - Set glissando */
    &enSubDesc(none),               /* S3 - Set finetune */
    &enSubDesc(none),               /* S4 - Set vibrato waveform */
    &enSubDesc(none),               /* S5 - Set tremolo waveform */
    &enSubDesc(none),               /* S6 - n/a */
    &enSubDesc(none),               /* S7 - n/a */
    &enSubDesc(none),               /* S8 - Panning */
    &enSubDesc(none),               /* S9 - n/a */
    &enSubDesc(none),               /* SA - Stereo control */
    &enSubDesc(none),               /* SB - Pattern loop */
    &enSubDesc(special_noteCut),    /* SCx - Note cut */
    &enSubDesc(special_noteDelay),  /* SDx - Note delay */
    &enSubDesc(none),               /* SE - Pattern delay */
    &enSubDesc(none)                /* SF - Function repeat */
};

/*** T: Set tempo ***/

epInit(setTempo);
descEff(setTempo) =
{
    enInit  (setTempo),
    enHandle(none),
    enTick  (none)
};

/*** U: Fine vibrato ***/

epTick(fineVibrato);
descEff(fineVibrato) =
{
    enInit  (vibrato),
    enHandle(none),
    enTick  (fineVibrato)
};

/*** V: Set global volume ***/

epInit(setGVol);
descEff(setGVol) =
{
    enInit  (setGVol),
    enHandle(none),
    enTick  (none)
};

/*** Main effects table ***/

#define MAXEFF 22

descEffList(main) =
{
    &enDesc(none),
    &enDesc(setSpeed),      // A
    &enDesc(jumpToOrder),   // B
    &enDesc(patBreak),      // C
    &enDesc(volSlide),      // D
    &enDesc(pitchDown),     // E
    &enDesc(pitchUp),       // F
    &enDesc(porta),         // G
    &enDesc(vibrato),       // H
    &enDesc(tremor),        // I
    &enDesc(arpeggio),      // J
    &enDesc(vib_volSlide),  // K: (H) + (D)
    &enDesc(porta_volSlide),// L: (G) + (D)
    &enDesc(none),          // M
    &enDesc(none),          // N
    &enDesc(none),          // O
    &enDesc(none),          // P
    &enDesc(retrig),        // Q
    &enDesc(tremolo),       // R
    &enDesc(special),       // S
    &enDesc(setTempo),      // T
    &enDesc(fineVibrato),   // U
    &enDesc(setGVol),       // V
    // W
    // X
    // Y
    // Z
};

extern bool    PUBLIC_DATA playState_jumpToOrder_bFlag;
extern uint8_t PUBLIC_DATA playState_jumpToOrder_bPos;
extern bool    PUBLIC_DATA playState_patBreak_bFlag;
extern uint8_t PUBLIC_DATA playState_patBreak_bPos;
extern bool    PUBLIC_DATA playState_patLoop_bNow;
extern bool    PUBLIC_DATA playState_gVolume_bFlag;
extern uint8_t PUBLIC_DATA playState_gVolume_bValue;
extern bool    PUBLIC_DATA playState_patDelay_bNow;

/* Channel state */

/* call 'readnotes' inside a pattern delay, */
/* if then ignore all notes/inst/vol ! */

/* now some variables I added after I found out those amazing things */
/* about pattern delay */

/* save effect,parameter for pattern delay */
extern uint16_t PUBLIC_DATA chnState_patDelay_wCommandSaved;
extern uint8_t  PUBLIC_DATA chnState_patDelay_bParameterSaved;

/* normaly it will be a copie of es:[di], but in */
/* pattern delay = 0 -> ignore note */
extern uint8_t PUBLIC_DATA chnState_cur_bNote;
extern uint8_t PUBLIC_DATA chnState_cur_bIns;   /* the same thing for instrument */
extern uint8_t PUBLIC_DATA chnState_cur_bVol;   /* and for volume */

/* to save portamento values : */
extern bool     PUBLIC_DATA chnState_porta_flag;
extern uint16_t PUBLIC_DATA chnState_porta_wSmpPeriodOld;
extern uint32_t PUBLIC_DATA chnState_porta_dSmpStepOld;

extern bool     PUBLIC_DATA chnState_arp_bFlag;
    /* a little one for arpeggio */

void *PUBLIC_CODE mapPatternData(void *pat)
{
    unsigned int logPage;
    unsigned char physPage;
    if (isPatternDataInEM(FP_SEG(pat))) {
        logPage = getPatternDataLogPageInEM(FP_SEG(pat));
        physPage = 0;
        if (! EmsMap(patListEMHandle, logPage, physPage))
            return MK_FP(0, 0);
        return MK_FP(FrameSEG[0], getPatternDataOffsetInEM(FP_SEG(pat)));
    } else {
        return pat;
    };
}

void PUBLIC_CODE chn_setSamplePeriod(MIXCHN *chn, int32_t period)
{
    if (period) {
        if (period < chn->wSmpPeriodLow)  period = chn->wSmpPeriodLow; else
        if (period > chn->wSmpPeriodHigh) period = chn->wSmpPeriodHigh;
        chn->wSmpPeriod = period;
        chn->dSmpStep = mixCalcSampleStep(period);
    } else {
        chn->wSmpPeriod = 0;
        chn->dSmpStep = 0;
    };
}

void PUBLIC_CODE chn_setSampleVolume(MIXCHN *chn, int16_t vol)
{
    if (vol < 0) chn->bSmpVol = 0; else chn->bSmpVol = vol > CHNINSVOL_MAX ? CHNINSVOL_MAX : vol;
}

void __near chn_setPeriodLimits(MIXCHN *chn, uint16_t rate, bool amiga)
{
    if (amiga) {
         // B-5, C-3:
         chn->wSmpPeriodLow  = (unsigned long)(MID_C_RATE * (unsigned long)getNotePeriod((5 << 4) + 11)) / rate;
         chn->wSmpPeriodHigh = (unsigned long)(MID_C_RATE * (unsigned long)getNotePeriod((3 << 4) +  0)) / rate;
    } else {
         // B-7, C-0:
         chn->wSmpPeriodLow  = (unsigned long)(MID_C_RATE * (unsigned long)getNotePeriod((7 << 4) + 11)) / rate;
         chn->wSmpPeriodHigh = (unsigned long)(MID_C_RATE * (unsigned long)getNotePeriod((0 << 4) +  0)) / rate;
    };
}

void PUBLIC_CODE chn_setupInstrument(MIXCHN *chn, uint8_t insNum)
{
    struct instrument_t *ins;
    unsigned int rate;
    unsigned int flags;
    ins = insList_get(insNum);
    if (ins_isSample(ins)) {
        rate = ins_getSampleRate(ins);
        if (rate) {
            chn->bIns = insNum;
            chn_setInstrument(chn, ins);
            chn_setSampleVolume(chn, (ins->vol * GVolume) >> 6);
            chn_setSampleData(chn, ins_getSampleData(ins));
            flags = 0;
            if (ins->flags & 0x01) flags |= SMPFLAG_LOOP;
            chn->bSmpFlags = flags;
            chn->wSmpLoopStart = ins_getSampleLoopStart(ins);
            chn->wSmpLoopEnd = ins_getSampleLoopEnd(ins);
            chn->wSmpStart = 0; // reset start position
            chn_setPeriodLimits(chn, rate, modOption_AmigaLimits);
        } else {
            // don't play it - it's wrong !
            chn->bIns = 0;
        };
    } else {
        // don't play it - it's wrong !
        chn->bIns = 0;
    };
}

uint16_t PUBLIC_CODE chn_calcNotePeriod(MIXCHN *chn, struct instrument_t *ins, uint8_t note)
{
    unsigned int period;
    // calc period with st3 finetune
    period = (unsigned long)(MID_C_RATE * (unsigned long)getNotePeriod(note)) / ins_getSampleRate(ins);
    // Now check borders
    if (period < chn->wSmpPeriodLow)  period = chn->wSmpPeriodLow;
    if (period > chn->wSmpPeriodHigh) period = chn->wSmpPeriodHigh;
    return period;
}

uint32_t PUBLIC_CODE chn_calcNoteStep(MIXCHN *chn, struct instrument_t *ins, uint8_t note)
{
    unsigned int period;
    period = chn_calcNotePeriod(chn, ins, note);
    if (period)
        return mixCalcSampleStep(period);
    else
        return 0;
}

void PUBLIC_CODE chn_setupNote(MIXCHN *chn, uint8_t note, bool keep)
{
    struct instrument_t *ins;
    chn->bNote = note;
    chn->wSmpPeriod = 0;    // clear it first - just to make sure we really set it
    if (chn->bIns) {
        ins = chn_getInstrument(chn);
        if (ins_isSample(ins)) {
            chn_setSamplePeriod(chn, chn_calcNotePeriod(chn, ins, note));
            if (! keep) {
                // restart instrument
                chn->dSmpPos = (unsigned long)chn->wSmpStart << 16;
                chn->bEnabled = 1;
            };
        };
    };
}

/*** Effects ***/

uint8_t __near checkPara0(MIXCHN *chn, uint8_t param)
{
    if (param)
        chn->bParameter = param;
    else
        param = chn->bParameter;
    return param;
}

uint8_t __near checkPara0not(MIXCHN *chn, uint8_t param)
{
    if (param)
        chn->bParameter = param;
    return param;
}

/*** No effect ***/

epInit(none)
{
    param = checkPara0(chn, param);
    return true;
}

epHandle(none)
{
    return;
}

epTick(none)
{
    return;
}

/*** A: Set speed ***/

epInit(setSpeed)
{
    param = checkPara0not(chn, param);
    if (param)
        CurSpeed = param;
    return true;
}

/*** B: Jump to order ***/

epInit(jumpToOrder)
{
    param = checkPara0not(chn, param);
    playState_jumpToOrder_bFlag = true;
    playState_jumpToOrder_bPos = param;
    return true;
}

/*** C: Pattern break ***/

epInit(patBreak)
{
    param = checkPara0not(chn, param);
    playState_patBreak_bFlag = true;
    playState_patBreak_bPos = (((param >> 4) * 10) + (param & 0x0f)) & 0x3f;
    return true;
}

/*** D: Volume slide ***/

epInit(volSlide)
{
    uint8_t subCmd;
    param = checkPara0(chn, param);
    if ((param & 0xf0) == 0xf0)
    {
        if (param == 0xf0)
            // x0
            subCmd = EFFIDX_VOLSLIDE_UP;
        else
            // Fx
            subCmd = EFFIDX_VOLSLIDE_FINE_DOWN;
    }
    else
    {
        if ((param & 0x0f) == 0x0f)
        {
            if (param == 0x0f)
                // 0x
                subCmd = EFFIDX_VOLSLIDE_DOWN;
            else
                // xF
                subCmd = EFFIDX_VOLSLIDE_FINE_UP;
        }
        else
        {
            if (param & 0x0f)
                // 0x
                subCmd = EFFIDX_VOLSLIDE_DOWN;
            else
                // x0
                subCmd = EFFIDX_VOLSLIDE_UP;
        };
    };
    chn_setSubCommand(chn, subCmd);
    return true;
}

epTick(volSlide)
{
    uint8_t cmd;
    cmd = chn_getSubCommand(chn);
    if (cmd <= 3)
        enSubDescList(volSlide)[cmd]->tick(chn);
}

epTick(volSlide_down)
{
    chn_setSampleVolume(chn, chn->bSmpVol - (chn->bParameter & 0x0f));
}

epTick(volSlide_up)
{
    chn_setSampleVolume(chn, ((chn->bSmpVol + (chn->bParameter >> 4)) * GVolume) >> 6);
}

/*** E: Pitch slide down ***/

epInit(pitchDown)
{
    uint8_t subCmd;
    param = checkPara0(chn, param);
    if (param < 0xe0)
        // xx
        subCmd = EFFIDX_PITCHDOWN_NORMAL;
    else
        if (param < 0xf0)
            // Ex
            subCmd = EFFIDX_PITCHDOWN_EXTRA;
        else
            // Fx
            subCmd = EFFIDX_PITCHDOWN_FINE;
    chn_setSubCommand(chn, subCmd);
    return true;
}

epTick(pitchDown)
{
    uint8_t cmd;
    cmd = chn_getSubCommand(chn);
    if (cmd <= 2)
        enSubDescList(pitchDown)[cmd]->tick(chn);
}

epTick(pitchDown_down)
{
    chn_setSamplePeriod(chn, chn->wSmpPeriod + (chn->bParameter << 2));
}

/*** F: Pitch slide up ***/

epInit(pitchUp)
{
    uint8_t subCmd;
    param = checkPara0(chn, param);
    if (param < 0xe0)
        subCmd = EFFIDX_PITCHUP_NORMAL;
    else
        if (param < 0xf0)
            subCmd = EFFIDX_PITCHUP_EXTRA;
        else
            subCmd = EFFIDX_PITCHUP_FINE;
    chn_setSubCommand(chn, subCmd);
    return true;
}

epTick(pitchUp)
{
    uint8_t cmd;
    cmd = chn_getSubCommand(chn);
    if (cmd <= 2)
        enSubDescList(pitchUp)[cmd]->tick(chn);
}

epTick(pitchUp_up)
{
    chn_setSamplePeriod(chn, chn->wSmpPeriod - (chn->bParameter << 2));
}

/*** G: Portamento to note ***/

void __near eff_porta_stop(MIXCHN *chn)
{
    chnState_porta_flag = false;
    chn_setCommand(chn, EFFIDX_NONE);
}

epInit(porta)
{
    param = checkPara0not(chn, param);

    if (param)
        chn->bPortParam = param;

    chnState_porta_flag = true;

    /* check first if portamento really possible: */
    if (!chn->bEnabled)
        eff_porta_stop(chn);
    else
        if ((chnState_cur_bNote != CHNNOTE_OFF)
        &&  (chnState_cur_bNote != CHNNOTE_EMPTY))
        {
            /* now save some values (we want to slide from) */
            chnState_porta_dSmpStepOld = chn->dSmpStep;
            chnState_porta_wSmpPeriodOld = chn->wSmpPeriod;
        };
    return true;
}

epTick(porta)
{
    long period = chn->wSmpPeriod;
    unsigned int slide = chn->bPortParam << 2;   // <- use amiga slide = para*4
    if (period > chn->wSmpPeriodDest) {
        period -= slide;
        if (period < chn->wSmpPeriodDest)
            period = chn->wSmpPeriodDest;
    } else {
        period += slide;
        if (period > chn->wSmpPeriodDest)
            period = chn->wSmpPeriodDest;
    };
    chn_setSamplePeriod(chn, period);
}

/*** H: Vibrato ***/

epInit(vibrato)
{
    param = checkPara0not(chn, param);

    if (chn->bEffFlags & EFFFLAG_CONTINUE)
        chn->bTabPos = 0;

    if (!param)
        param = chn->bVibParam;

    if (!(param & 0xf0))
        param |= chn->bVibParam & 0xf0;

    chn->bVibParam = param;
    return true;
}

epTick(vibrato)
{
    unsigned int pos;
    if (chn->bEnabled) {
        /* next position in table: */
        pos = (chn->bTabPos + (chn->bVibParam >> 4)) & 0x3f;
        chn->bTabPos = pos;
        chn_setSamplePeriod(chn, chn->wSmpPeriodOld +
            ((*(int8_t *)MK_FP(FP_SEG(&wavetab), chn->wVibTab + pos) *
                (chn->bVibParam & 0x0f)) >> 4));
    };
}

/*** I: Tremor ***/

epInit(tremor)
{
    param = checkPara0(chn, param);
    // TODO
    return true;
}

epTick(tremor)
{
    // TODO
    return;
}

/*** J: Arpeggio ***/

epInit(arpeggio)
{
    param = checkPara0(chn, param);
    // TODO
    return true;
}

epTick(arpeggio)
{
    unsigned int pos = chn->bArpPos + 1;
    if (pos >= 3)
        pos = 0;
    chn->bArpPos = pos;
    chn->dSmpStep = chn->dArpSmpSteps[pos];
}

/*** K: Vibrato (H) + Volume slide (D) ***/

epInit(vib_volSlide)
{
    param = checkPara0(chn, param);
    // TODO
    return true;
}

epTick(vib_volSlide)
{
    enDesc(volSlide).tick(chn);
    enDesc(vibrato).tick(chn);
}

/*** L: Portamento to note (G) + Volume slide (D) ***/

epInit(porta_volSlide)
{
    param = checkPara0(chn, param);
    // TODO
    return true;
}

epTick(porta_volSlide)
{
    enDesc(volSlide).tick(chn);
    enDesc(porta).tick(chn);
}

/*** Q: Note retrigger + Volume slide ***/

epInit(retrig)
{
    param = checkPara0(chn, param);
    // TODO
    return true;
}

epTick(retrig)
{
    unsigned int ticks;
    uint8_t cmd;
    if (chn->bRetrigTicks) {
        if (! --chn->bRetrigTicks)
            return;
    };
    chn->dSmpPos = 0;
    ticks = chn->bParameter & 0x0f;
    if (ticks) {
        chn->bRetrigTicks = ticks;
        cmd = chn_getSubCommand(chn);
        if (cmd <= 6)
            enSubDescList(retrig)[cmd]->tick(chn);
    };
}

epTick(retrig_slideDown)
{
    chn_setSampleVolume(chn, chn->bSmpVol - (int)(1 << (chn->bParameter >> 4)));
}

epTick(retrig_use2div3)
{
    chn_setSampleVolume(chn, (int)(chn->bSmpVol * 2) / 3);
}

epTick(retrig_use1div2)
{
    chn_setSampleVolume(chn, chn->bSmpVol >> 1);
}

epTick(retrig_slideUp)
{
    chn_setSampleVolume(chn, chn->bSmpVol + (int)(1 << (chn->bParameter >> 4)));
}

epTick(retrig_use3div2)
{
    chn_setSampleVolume(chn, (int)(chn->bSmpVol * 3) >> 1);
}

epTick(retrig_use2div1)
{
    chn_setSampleVolume(chn, (int)chn->bSmpVol << 1);
}

/*** R: Tremolo ***/

epInit(tremolo)
{
    param = checkPara0(chn, param);
    // TODO
    return true;
}

epTick(tremolo)
{
    /* next position in table: */
    chn->bTabPos = (chn->bTabPos + (chn->bParameter >> 4)) & 0x3f;
    chn_setSampleVolume(chn, chn->bSmpVolOld +
        ((*(int8_t *)MK_FP(FP_SEG(&wavetab), chn->wTrmTab + chn->bTabPos) *
        (chn->bParameter & 0x0f)) >> 6));
}

/*** S: Special effects ***/

epInit(special)
{
    param = checkPara0(chn, param);
    // TODO
    return true;
}

epTick(special)
{
    uint8_t cmd;
    cmd = chn_getSubCommand(chn);
    if (cmd <= 15)
        enSubDescList(special)[cmd]->tick(chn);
}

epTick(special_noteCut)
{
    if (! --chn->bDelayTicks)
        chn->bEnabled = 0;  // disable it
}

epTick(special_noteDelay)
{
    unsigned int insNum, note;
    if (! --chn->bDelayTicks) {
        insNum = chn->bSavIns;
        if (insNum) {
            chn_setupInstrument(chn, insNum);
        };
        note = chn->bSavNote;
        if (note != CHNNOTE_EMPTY) {
            if (note == CHNNOTE_OFF) {
                chn->bEnabled = 0;  // stop mixing
            } else {
                chn_setupNote(chn, note, 0);
                chn->bEnabled = 1;  // do mixing
            };
        };
        if (chn->bSavVol != CHNINSVOL_EMPTY)
            chn_setSampleVolume(chn, (chn->bSavVol * GVolume) >> 6);
        chn_setCommand(chn, 0);  // no more NoteDelay
    };
}

/*** T: Set tempo ***/

epInit(setTempo)
{
    param = checkPara0(chn, param);
    // TODO
    return true;
}

/*** U: Fine vibrato ***/

epTick(fineVibrato)
{
    unsigned int pos;
    if (chn->bEnabled) {
        /* next position in table: */
        pos = (chn->bTabPos + (chn->bVibParam >> 4)) & 0x3f;
        chn->bTabPos = pos;
        chn_setSamplePeriod(chn, chn->wSmpPeriodOld +
            ((*(int8_t *)MK_FP(FP_SEG(&wavetab), chn->wVibTab + pos)) *
            (chn->bVibParam & 0x0f)) >> 8);
    };
}

/*** V: Set global volume ***/

epInit(setGVol)
{
    param = checkPara0(chn, param);
    // TODO
    return true;
}

/*** General effects handling ***/

bool PUBLIC_CODE chn_effInit(MIXCHN *chn, uint8_t param)
{
    uint8_t cmd;
    cmd = chn_getCommand(chn);
    if (cmd <= MAXEFF)
        return enDescList(main)[cmd]->init(chn, param);
    else
        return false;
}

void PUBLIC_CODE chn_effHandle(MIXCHN *chn)
{
    uint8_t cmd;
    cmd = chn_getCommand(chn);
    if (cmd <= MAXEFF)
        enDescList(main)[cmd]->handle(chn);
}

void PUBLIC_CODE chn_effTick(MIXCHN *chn)
{
    uint8_t cmd;
    cmd = chn_getCommand(chn);
    if (cmd <= MAXEFF)
        enDescList(main)[cmd]->tick(chn);
}
