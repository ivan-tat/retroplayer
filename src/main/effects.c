/* effects.c -- effects handling.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "dos/ems.h"
#include "main/s3mtypes.h"
#include "main/s3mvars.h"
#include "main/effvars.h"
#include "main/mixvars.h"
#include "main/mixer.h"
#include "main/effects.h"

// TODO: remove PUBLIC_CODE macros when done.

#define get_i8_value(off, pos) *(int8_t *)MK_FP(FP_SEG(&wavetab), off + pos)

/*** Effects ***/

// Effect's method
typedef bool __near emInit_t  (MIXCHN *chn, uint8_t param);
typedef void __near emHandle_t(MIXCHN *chn);
typedef void __near emTick_t  (MIXCHN *chn);
typedef bool __near emCont_t  (MIXCHN *chn);
typedef void __near emStop_t  (MIXCHN *chn);

// Effect's methods table
typedef struct effMethodsTable_t
{
    emInit_t   __near *init;
    emHandle_t __near *handle;
    emTick_t   __near *tick;
    emCont_t   __near *cont;
    emStop_t   __near *stop;
};
typedef struct effMethodsTable_t EFFMT;

// Effect's method name
#define NAME_INIT(name)   eff_##name##_init
#define NAME_HANDLE(name) eff_##name##_handle
#define NAME_TICK(name)   eff_##name##_tick
#define NAME_CONT(name)   eff_##name##_cont
#define NAME_STOP(name)   eff_##name##_stop

// Effect's method definition
#define METHOD_INIT(name)   bool __near NAME_INIT(name)  (MIXCHN *chn, uint8_t param)
#define METHOD_HANDLE(name) void __near NAME_HANDLE(name)(MIXCHN *chn)
#define METHOD_TICK(name)   void __near NAME_TICK(name)  (MIXCHN *chn)
#define METHOD_CONT(name)   bool __near NAME_CONT(name)  (MIXCHN *chn)
#define METHOD_STOP(name)   void __near NAME_STOP(name)  (MIXCHN *chn)

// Effect description
#define EFFECT(name) eff_##name##_desc
#define DEFINE_EFFECT(name, init, handle, tick, cont, stop) \
static const EFFMT __near EFFECT(name) = \
{ \
    NAME_INIT  (init), \
    NAME_HANDLE(handle), \
    NAME_TICK  (tick), \
    NAME_CONT  (cont), \
    NAME_STOP  (stop) \
}

// Effect descriptions list
#define EFFECTS_LIST(name) eff_##name##_desc_list
#define DEFINE_EFFECTS_LIST(name) static const EFFMT __near *EFFECTS_LIST(name)[]

// Sub-effect description
#define SUB_EFFECT(name) eff_##name##_sub_desc
#define DEFINE_SUB_EFFECT(name, init, handle, tick, cont, stop) \
static const EFFMT __near SUB_EFFECT(name) = \
{ \
    NAME_INIT  (init), \
    NAME_HANDLE(handle), \
    NAME_TICK  (tick), \
    NAME_CONT  (cont), \
    NAME_STOP  (stop) \
}

// Sub-effect descriptions list
#define SUB_EFFECTS_LIST(name) eff_##name##_sub_desc_list
#define DEFINE_SUB_EFFECTS_LIST(name) static const EFFMT __near *SUB_EFFECTS_LIST(name)[]

/*** No effect ***/

// effect continue checks

METHOD_CONT(allow);
METHOD_CONT(deny);

// empty effect

METHOD_INIT  (none);
METHOD_HANDLE(none);
METHOD_TICK  (none);
METHOD_STOP  (none);
DEFINE_EFFECT(none, none, none, none, deny, none);

// empty sub-effect

DEFINE_SUB_EFFECT(none, none, none, none, deny, none);

/****** Global effects ******/

/*** Set tempo ***/
/* Scream Tracker 3 command: T */

METHOD_INIT(setTempo);
DEFINE_EFFECT(setTempo, setTempo, none, none, deny, none);

/*** Set speed ***/
/* Scream Tracker 3 command: A */

METHOD_INIT(setSpeed);
DEFINE_EFFECT(setSpeed, setSpeed, none, none, deny, none);

/*** Jump to order ***/
/* Scream Tracker 3 command: B */

METHOD_INIT(jumpToOrder);
DEFINE_EFFECT(jumpToOrder, jumpToOrder, none, none, deny, none);

/*** Pattern break ***/
/* Scream Tracker 3 command: C */

METHOD_INIT(patBreak);
DEFINE_EFFECT(patBreak, patBreak, none, none, deny, none);

/*** Set global volume ***/
/* Scream Tracker 3 command: V */

METHOD_INIT(setGVol);
DEFINE_EFFECT(setGVol, setGVol, none, none, deny, none);

// TODO: move here pattern delay

/****** Channel effects ******/

/*** Volume slide ***/
/* Scream Tracker 3 command: D */

// sub-effects

METHOD_TICK(volSlide_down);
DEFINE_SUB_EFFECT(volSlide_down, none, none, volSlide_down, allow, none);

METHOD_TICK(volSlide_up);
DEFINE_SUB_EFFECT(volSlide_up, none, none, volSlide_up, allow, none);

METHOD_HANDLE(volSlide_fineDown);
DEFINE_SUB_EFFECT(volSlide_fineDown, none, volSlide_fineDown, none, allow, none);

METHOD_HANDLE(volSlide_fineUp);
DEFINE_SUB_EFFECT(volSlide_fineUp, none, volSlide_fineUp, none, allow, none);

// sub-effects list

DEFINE_SUB_EFFECTS_LIST(volSlide) =
{
    &SUB_EFFECT(volSlide_down),
    &SUB_EFFECT(volSlide_up),
    &SUB_EFFECT(volSlide_fineDown),
    &SUB_EFFECT(volSlide_fineUp)
};

#define EFFIDX_VOLSLIDE_DOWN      0
#define EFFIDX_VOLSLIDE_UP        1
#define EFFIDX_VOLSLIDE_FINE_DOWN 2
#define EFFIDX_VOLSLIDE_FINE_UP   3
#define EFFIDX_VOLSLIDE_MAX       3

// router

METHOD_INIT  (volSlide);
METHOD_HANDLE(volSlide);
METHOD_TICK  (volSlide);
DEFINE_EFFECT(volSlide, volSlide, volSlide, volSlide, allow, none);

/*** Pitch slide down ***/
/* Scream Tracker 3 command: E */

// sub-effects

METHOD_TICK(pitchDown_normal);
DEFINE_SUB_EFFECT(pitchDown_normal, none, none, pitchDown_normal, allow, none);

METHOD_HANDLE(pitchDown_fine);
DEFINE_SUB_EFFECT(pitchDown_fine, none, pitchDown_fine, none, allow, none);

METHOD_HANDLE(pitchDown_extra);
DEFINE_SUB_EFFECT(pitchDown_extra, none, pitchDown_extra, none, allow, none);

// sub-effects list

DEFINE_SUB_EFFECTS_LIST(pitchDown) =
{
    &SUB_EFFECT(pitchDown_normal),
    &SUB_EFFECT(pitchDown_fine),
    &SUB_EFFECT(pitchDown_extra)
};

#define EFFIDX_PITCHDOWN_NORMAL 0
#define EFFIDX_PITCHDOWN_FINE   1
#define EFFIDX_PITCHDOWN_EXTRA  2
#define EFFIDX_PITCHDOWN_MAX    2

// router

METHOD_INIT  (pitchDown);
METHOD_HANDLE(pitchDown);
METHOD_TICK  (pitchDown);
DEFINE_EFFECT(pitchDown, pitchDown, pitchDown, pitchDown, allow, none);

/*** Pitch slide up ***/
/* Scream Tracker 3 command: F */

// sub-effects

METHOD_TICK(pitchUp_normal);
DEFINE_SUB_EFFECT(pitchUp_normal, none, none, pitchUp_normal, allow, none);

METHOD_HANDLE(pitchUp_fine);
DEFINE_SUB_EFFECT(pitchUp_fine, none, pitchUp_fine, none, allow, none);

METHOD_HANDLE(pitchUp_extra);
DEFINE_SUB_EFFECT(pitchUp_extra, none, pitchUp_extra, none, allow, none);

// sub-effects list

DEFINE_SUB_EFFECTS_LIST(pitchUp) =
{
    &SUB_EFFECT(pitchUp_normal),
    &SUB_EFFECT(pitchUp_fine),
    &SUB_EFFECT(pitchUp_extra)
};

#define EFFIDX_PITCHUP_NORMAL 0
#define EFFIDX_PITCHUP_FINE   1
#define EFFIDX_PITCHUP_EXTRA  2
#define EFFIDX_PITCHUP_MAX    2

// router

METHOD_INIT  (pitchUp);
METHOD_HANDLE(pitchUp);
METHOD_TICK  (pitchUp);
DEFINE_EFFECT(pitchUp, pitchUp, pitchUp, pitchUp, allow, none);

/*** Portamento to note ***/
/* Scream Tracker 3 command: G */

METHOD_INIT  (porta);
METHOD_HANDLE(porta);
METHOD_TICK  (porta);
DEFINE_EFFECT(porta, porta, porta, porta, allow, none);

/*** Portamento to note + Volume slide ***/
/* Scream Tracker 3 command: L (G + D) */

METHOD_INIT  (porta_vol);
METHOD_HANDLE(porta_vol);
METHOD_TICK  (porta_vol);
DEFINE_EFFECT(porta_vol, porta_vol, porta_vol, porta_vol, allow, none);

/*** Vibrato (normal) ***/
/* Scream Tracker 3 command: H */

METHOD_INIT  (vibNorm);
METHOD_HANDLE(vibNorm);
METHOD_TICK  (vibNorm);
METHOD_CONT  (vibNorm);
METHOD_STOP  (vibNorm);
DEFINE_EFFECT(vibNorm, vibNorm, vibNorm, vibNorm, vibNorm, vibNorm);

/*** Vibrato (fine) ***/
/* Scream Tracker 3 command: U */

METHOD_TICK(vibFine);
DEFINE_EFFECT(vibFine, vibNorm, none, vibFine, vibNorm, vibNorm);

/*** Vibrato (normal) + Volume slide ***/
/* Scream Tracker 3 command: K (H + D) */

METHOD_INIT  (vibNorm_vol);
METHOD_HANDLE(vibNorm_vol);
METHOD_TICK  (vibNorm_vol);
DEFINE_EFFECT(vibNorm_vol, vibNorm_vol, vibNorm_vol, vibNorm_vol, vibNorm, vibNorm);

/*** Tremor ***/
/* Scream Tracker 3 command: I */

METHOD_INIT(tremor);
METHOD_TICK(tremor);
DEFINE_EFFECT(tremor, tremor, none, tremor, allow, none);

/*** Arpeggio ***/
/* Scream Tracker 3 command: J */

METHOD_INIT  (arpeggio);
METHOD_HANDLE(arpeggio);
METHOD_TICK  (arpeggio);
METHOD_STOP  (arpeggio);
DEFINE_EFFECT(arpeggio, arpeggio, arpeggio, arpeggio, allow, arpeggio);

/*** Set sample offset ***/
/* Scream Tracker 3 command: O */

METHOD_HANDLE(sampleOffset);
DEFINE_EFFECT(sampleOffset, none, sampleOffset, none, deny, none);

/*** Note retrigger + Volume slide ***/
/* Scream Tracker 3 command: Q */

METHOD_INIT(retrig);
METHOD_TICK(retrig);
DEFINE_EFFECT(retrig, retrig, none, retrig, allow, none);

// sub-effects

METHOD_INIT(retrig_none);
DEFINE_SUB_EFFECT(retrig_none, retrig_none, none, none, allow, none);

METHOD_INIT(retrig_slideDown);
METHOD_TICK(retrig_slideDown);
DEFINE_SUB_EFFECT(retrig_slideDown, retrig_slideDown, none, retrig_slideDown, allow, none);

METHOD_INIT(retrig_use2div3);
METHOD_TICK(retrig_use2div3);
DEFINE_SUB_EFFECT(retrig_use2div3, retrig_use2div3, none, retrig_use2div3, allow, none);

METHOD_INIT(retrig_use1div2);
METHOD_TICK(retrig_use1div2);
DEFINE_SUB_EFFECT(retrig_use1div2, retrig_use1div2, none, retrig_use1div2, allow, none);

METHOD_INIT(retrig_slideUp);
METHOD_TICK(retrig_slideUp);
DEFINE_SUB_EFFECT(retrig_slideUp, retrig_slideUp, none, retrig_slideUp, allow, none);

METHOD_INIT(retrig_use3div2);
METHOD_TICK(retrig_use3div2);
DEFINE_SUB_EFFECT(retrig_use3div2, retrig_use3div2, none, retrig_use3div2, allow, none);

METHOD_INIT(retrig_use2div1);
METHOD_TICK(retrig_use2div1);
DEFINE_SUB_EFFECT(retrig_use2div1, retrig_use2div1, none, retrig_use2div1, allow, none);

// sub-effects list

DEFINE_SUB_EFFECTS_LIST(retrig) =
{
    &SUB_EFFECT(retrig_none),
    &SUB_EFFECT(retrig_slideDown),
    &SUB_EFFECT(retrig_use2div3),
    &SUB_EFFECT(retrig_use1div2),
    &SUB_EFFECT(retrig_slideUp),
    &SUB_EFFECT(retrig_use3div2),
    &SUB_EFFECT(retrig_use2div1)
};

#define EFFIDX_RETRIG_VOLSLIDE_NONE     0
#define EFFIDX_RETRIG_VOLSLIDE_DOWN     1
#define EFFIDX_RETRIG_VOLSLIDE_USE2DIV3 2
#define EFFIDX_RETRIG_VOLSLIDE_USE1DIV2 3
#define EFFIDX_RETRIG_VOLSLIDE_UP       4
#define EFFIDX_RETRIG_VOLSLIDE_USE3DIV2 5
#define EFFIDX_RETRIG_VOLSLIDE_USE2DIV1 6

static const uint8_t eff_retrig_route[16] =
{
    EFFIDX_RETRIG_VOLSLIDE_NONE,
    EFFIDX_RETRIG_VOLSLIDE_DOWN,
    EFFIDX_RETRIG_VOLSLIDE_DOWN,
    EFFIDX_RETRIG_VOLSLIDE_DOWN,
    EFFIDX_RETRIG_VOLSLIDE_DOWN,
    EFFIDX_RETRIG_VOLSLIDE_DOWN,
    EFFIDX_RETRIG_VOLSLIDE_USE2DIV3,
    EFFIDX_RETRIG_VOLSLIDE_USE1DIV2,
    EFFIDX_RETRIG_VOLSLIDE_NONE,
    EFFIDX_RETRIG_VOLSLIDE_UP,
    EFFIDX_RETRIG_VOLSLIDE_UP,
    EFFIDX_RETRIG_VOLSLIDE_UP,
    EFFIDX_RETRIG_VOLSLIDE_UP,
    EFFIDX_RETRIG_VOLSLIDE_UP,
    EFFIDX_RETRIG_VOLSLIDE_USE3DIV2,
    EFFIDX_RETRIG_VOLSLIDE_USE2DIV1
};

/*** Tremolo ***/
/* Scream Tracker 3 command: R */

METHOD_INIT(tremolo);
METHOD_HANDLE(tremolo);
METHOD_TICK(tremolo);
DEFINE_EFFECT(tremolo, tremolo, tremolo, tremolo, allow, none);

/****** Special effects ******/
/* Scream Tracker 3 command: S */

// sub-effects

/*** Fine tune ***/
/* Scream Tracker 3 command: S3 */

METHOD_HANDLE(special_fineTune);
DEFINE_SUB_EFFECT(special_fineTune, none, special_fineTune, none, deny, none);

/*** Set vibrato waveform ***/

METHOD_INIT(special_setVibWave);
DEFINE_SUB_EFFECT(special_setVibWave, special_setVibWave, none, none, deny, none);

/*** Set tremolo waveform ***/

METHOD_INIT(special_setTremWave);
DEFINE_SUB_EFFECT(special_setTremWave, special_setTremWave, none, none, deny, none);

/*** Pattern loop ***/

METHOD_INIT(special_patLoop);
DEFINE_SUB_EFFECT(special_patLoop, special_patLoop, none, none, deny, none);

/*** Note cut ***/
/* Scream Tracker 3 command: SC */

METHOD_TICK(special_noteCut);
DEFINE_SUB_EFFECT(special_noteCut, none, none, special_noteCut, deny, none);

/*** Note delay ***/
/* Scream Tracker 3 command: SD */

METHOD_INIT(special_noteDelay);
METHOD_TICK(special_noteDelay);
DEFINE_SUB_EFFECT(special_noteDelay, special_noteDelay, none, special_noteDelay, deny, none);

/*** Pattern delay ***/
/* Scream Tracker 3 command: SE */

METHOD_INIT  (special_patDelay);
METHOD_HANDLE(special_patDelay);
DEFINE_SUB_EFFECT(special_patDelay, special_patDelay, special_patDelay, none, deny, none);

// sub-effects list

DEFINE_SUB_EFFECTS_LIST(special) =
{
    &SUB_EFFECT(none),
    &SUB_EFFECT(special_fineTune),
    &SUB_EFFECT(special_setVibWave),
    &SUB_EFFECT(special_setTremWave),
    &SUB_EFFECT(special_patLoop),
    &SUB_EFFECT(special_noteCut),
    &SUB_EFFECT(special_noteDelay),
    &SUB_EFFECT(special_patDelay)
};

#define EFFIDX_SPECIAL_NONE      0
#define EFFIDX_SPECIAL_FINETUNE  1
#define EFFIDX_SPECIAL_VIBWAVE   2
#define EFFIDX_SPECIAL_TREMWAVE  3
#define EFFIDX_SPECIAL_PATLOOP   4
#define EFFIDX_SPECIAL_NOTECUT   5
#define EFFIDX_SPECIAL_NOTEDELAY 6
#define EFFIDX_SPECIAL_PATDELAY  7
#define EFFIDX_SPECIAL_MAX       7

static const uint8_t eff_special_route[16] =
{
    EFFIDX_SPECIAL_NONE,    /* S0 - n/a */
    EFFIDX_SPECIAL_NONE,    /* S1 - Set filter */
    EFFIDX_SPECIAL_NONE,    /* S2 - Set glissando */
    EFFIDX_SPECIAL_FINETUNE,
    EFFIDX_SPECIAL_VIBWAVE,
    EFFIDX_SPECIAL_TREMWAVE,
    EFFIDX_SPECIAL_NONE,    /* S6 - n/a */
    EFFIDX_SPECIAL_NONE,    /* S7 - n/a */
    EFFIDX_SPECIAL_NONE,    /* S8 - Panning */
    EFFIDX_SPECIAL_NONE,    /* S9 - n/a */
    EFFIDX_SPECIAL_NONE,    /* SA - Stereo control */
    EFFIDX_SPECIAL_PATLOOP,
    EFFIDX_SPECIAL_NOTECUT,
    EFFIDX_SPECIAL_NOTEDELAY,
    EFFIDX_SPECIAL_PATDELAY,
    EFFIDX_SPECIAL_NONE     /* SF - Function repeat */
};

// router

METHOD_INIT(special);
METHOD_TICK(special);
DEFINE_EFFECT(special, special, none, special, deny, none);

/*** Main effects table ***/

DEFINE_EFFECTS_LIST(main) =
{
    &EFFECT(none),
    &EFFECT(setSpeed),      // A
    &EFFECT(jumpToOrder),   // B
    &EFFECT(patBreak),      // C
    &EFFECT(volSlide),      // D
    &EFFECT(pitchDown),     // E
    &EFFECT(pitchUp),       // F
    &EFFECT(porta),         // G
    &EFFECT(vibNorm),       // H
    &EFFECT(tremor),        // I
    &EFFECT(arpeggio),      // J
    &EFFECT(vibNorm_vol),   // K: (H) + (D)
    &EFFECT(porta_vol),     // L: (G) + (D)
    &EFFECT(none),          // M
    &EFFECT(none),          // N
    &EFFECT(none),          // O
    &EFFECT(none),          // P
    &EFFECT(retrig),        // Q
    &EFFECT(tremolo),       // R
    &EFFECT(special),       // S
    &EFFECT(setTempo),      // T
    &EFFECT(vibFine),       // U
    &EFFECT(setGVol),       // V
    // W
    // X
    // Y
    // Z
};

void PUBLIC_CODE set_speed(uint8_t value)
{
    if (value > 0)
        playState_speed = value;
}

void PUBLIC_CODE set_tempo(uint8_t value)
{
    if (value >= 32)
        playState_tempo = value;
    else
        value = playState_tempo;
    if (value)
        mixTickSamplesPerChannel = (long)mixSampleRate * 5 / (int)(value * 2);
}

void PUBLIC_CODE chn_setupInstrument(MIXCHN *chn, uint8_t insNum)
{
    struct instrument_t *ins;
    unsigned int rate;
    unsigned int flags;
    ins = insList_get(insNum);
    if (ins_isSample(ins))
    {
        rate = ins_getSampleRate(ins);
        if (rate)
        {
            chn->bIns = insNum;
            chn_setInstrument(chn, ins);
            chn_setSampleVolume(chn, (ins->vol * playState_gVolume) >> 6);
            chn_setSampleData(chn, ins_getSampleData(ins));
            flags = 0;
            if (ins->flags & 0x01)
                flags |= SMPFLAG_LOOP;
            chn->bSmpFlags = flags;
            chn->wSmpLoopStart = ins_getSampleLoopStart(ins);
            chn->wSmpLoopEnd = ins_getSampleLoopEnd(ins);
            chn->wSmpStart = 0; // reset start position
            chn_setPeriodLimits(chn, rate, modOption_AmigaLimits);
        }
        else
            // don't play it - it's wrong !
            chn->bIns = 0;
    }
    else
        // don't play it - it's wrong !
        chn->bIns = 0;
}

uint16_t PUBLIC_CODE chn_calcNotePeriod(MIXCHN *chn, struct instrument_t *ins, uint8_t note)
{
    unsigned int period;
    // calc period with st3 finetune
    period = (unsigned long)(MID_C_RATE * (unsigned long)getNotePeriod(note)) / ins_getSampleRate(ins);
    // Now check borders
    return chn_checkSamplePeriod(chn, period);
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
    if (chn->bIns)
    {
        ins = chn_getInstrument(chn);
        if (ins_isSample(ins))
        {
            chn_setSamplePeriod(chn, chn_calcNotePeriod(chn, ins, note));
            if (! keep)
            {
                // restart instrument
                chn->dSmpPos = (unsigned long)chn->wSmpStart << 16;
                chn_setState(chn, true);
            };
        };
    };
}

/*** Effects ***/

uint8_t __near checkPara0(MIXCHN *chn, uint8_t param)
{
    if (param)
        chn_setEffectParam(chn, param);
    else
        param = chn_getEffectParam(chn);
    return param;
}

uint8_t __near checkPara0not(MIXCHN *chn, uint8_t param)
{
    if (param)
        chn_setEffectParam(chn, param);
    return param;
}

/*** Continue checks ***/

METHOD_CONT(allow)
{
    return true;
}

METHOD_CONT(deny)
{
    return false;
}

/*** No effect ***/

METHOD_INIT(none)
{
    param = checkPara0(chn, param);
    return true;
}

METHOD_HANDLE(none)
{
    return;
}

METHOD_TICK(none)
{
    return;
}

METHOD_STOP(none)
{
    return;
}

/*** Set speed ***/

METHOD_INIT(setSpeed)
{
    param = checkPara0not(chn, param);
    set_speed(param);
    return true;
}

/*** Set tempo ***/

METHOD_INIT(setTempo)
{
    param = checkPara0not(chn, param);
    set_tempo(param);
    return true;
}

/*** Jump to order ***/

METHOD_INIT(jumpToOrder)
{
    param = checkPara0not(chn, param);
    playState_jumpToOrder_bFlag = true;
    playState_jumpToOrder_bPos = param;
    return true;
}

/*** Pattern break ***/

METHOD_INIT(patBreak)
{
    param = checkPara0not(chn, param);
    playState_patBreak_bFlag = true;
    playState_patBreak_bPos = (((param >> 4) * 10) + (param & 0x0f)) & 0x3f;
    return true;
}

/*** Set global volume ***/

METHOD_INIT(setGVol)
{
    param = checkPara0not(chn, param);
    playState_gVolume_bFlag = true;
    playState_gVolume_bValue = param > 64 ? 64 : param;
    return true;
}

/*** Volume slide ***/

METHOD_INIT(volSlide)
{
    uint8_t cmd;
    param = checkPara0(chn, param);
    if ((param & 0xf0) == 0xf0)
    {
        if (param == 0xf0)
            // x0
            cmd = EFFIDX_VOLSLIDE_UP;
        else
            // Fx
            cmd = EFFIDX_VOLSLIDE_FINE_DOWN;
    }
    else
    {
        if ((param & 0x0f) == 0x0f)
        {
            if (param == 0x0f)
                // 0x
                cmd = EFFIDX_VOLSLIDE_DOWN;
            else
                // xF
                cmd = EFFIDX_VOLSLIDE_FINE_UP;
        }
        else
        {
            if (param & 0x0f)
                // 0x
                cmd = EFFIDX_VOLSLIDE_DOWN;
            else
                // x0
                cmd = EFFIDX_VOLSLIDE_UP;
        };
    };
    chn_setSubCommand(chn, cmd);
    return true;
}

METHOD_HANDLE(volSlide)
{
    uint8_t cmd;
    cmd = chn_getSubCommand(chn);
    if (cmd <= EFFIDX_VOLSLIDE_MAX)
        SUB_EFFECTS_LIST(volSlide)[cmd]->handle(chn);
}

METHOD_TICK(volSlide)
{
    uint8_t cmd;
    cmd = chn_getSubCommand(chn);
    if (cmd <= EFFIDX_VOLSLIDE_MAX)
        SUB_EFFECTS_LIST(volSlide)[cmd]->tick(chn);
}

METHOD_TICK(volSlide_down)
{
    chn_setSampleVolume(chn, chn->bSmpVol - (chn_getEffectParam(chn) & 0x0f));
}

METHOD_TICK(volSlide_up)
{
    chn_setSampleVolume(chn, ((chn->bSmpVol + (chn_getEffectParam(chn) >> 4)) * playState_gVolume) >> 6);
}

METHOD_HANDLE(volSlide_fineDown)
{
    chn_setSampleVolume(chn, chn->bSmpVol - (chn_getEffectParam(chn) & 0x0f));
}

METHOD_HANDLE(volSlide_fineUp)
{
    chn_setSampleVolume(chn, chn->bSmpVol + (chn_getEffectParam(chn) >> 4));
}

/*** Pitch slide down ***/

METHOD_INIT(pitchDown)
{
    uint8_t cmd;
    param = checkPara0(chn, param);
    if (param < 0xe0)
        // xx
        cmd = EFFIDX_PITCHDOWN_NORMAL;
    else
        if (param < 0xf0)
            // Ex
            cmd = EFFIDX_PITCHDOWN_EXTRA;
        else
            // Fx
            cmd = EFFIDX_PITCHDOWN_FINE;
    chn_setSubCommand(chn, cmd);
    return true;
}

METHOD_HANDLE(pitchDown)
{
    uint8_t cmd;
    cmd = chn_getSubCommand(chn);
    if (cmd <= EFFIDX_PITCHDOWN_MAX)
        SUB_EFFECTS_LIST(pitchDown)[cmd]->handle(chn);
}

METHOD_TICK(pitchDown)
{
    uint8_t cmd;
    cmd = chn_getSubCommand(chn);
    if (cmd <= EFFIDX_PITCHDOWN_MAX)
        SUB_EFFECTS_LIST(pitchDown)[cmd]->tick(chn);
}

METHOD_TICK(pitchDown_normal)
{
    chn_setSamplePeriod(chn, chn_getSamplePeriod(chn) + (chn_getEffectParam(chn) << 2));
}

METHOD_HANDLE(pitchDown_fine)
{
    chn_setSamplePeriod(chn, chn_getSamplePeriod(chn) + ((chn_getEffectParam(chn) & 0x0f) << 2));
}

METHOD_HANDLE(pitchDown_extra)
{
    chn_setSamplePeriod(chn, chn_getSamplePeriod(chn) + (chn_getEffectParam(chn) & 0x0f));
}

/*** Pitch slide up ***/

METHOD_INIT(pitchUp)
{
    uint8_t cmd;
    param = checkPara0(chn, param);
    if (param < 0xe0)
        // xx
        cmd = EFFIDX_PITCHUP_NORMAL;
    else
        if (param < 0xf0)
            // Ex
            cmd = EFFIDX_PITCHUP_EXTRA;
        else
            // Fx
            cmd = EFFIDX_PITCHUP_FINE;
    chn_setSubCommand(chn, cmd);
    return true;
}

METHOD_HANDLE(pitchUp)
{
    uint8_t cmd;
    cmd = chn_getSubCommand(chn);
    if (cmd <= EFFIDX_PITCHUP_MAX)
        SUB_EFFECTS_LIST(pitchUp)[cmd]->handle(chn);
}

METHOD_TICK(pitchUp)
{
    uint8_t cmd;
    cmd = chn_getSubCommand(chn);
    if (cmd <= EFFIDX_PITCHUP_MAX)
        SUB_EFFECTS_LIST(pitchUp)[cmd]->tick(chn);
}

METHOD_TICK(pitchUp_normal)
{
    chn_setSamplePeriod(chn, chn_getSamplePeriod(chn) - (chn_getEffectParam(chn) << 2));
}

METHOD_HANDLE(pitchUp_fine)
{
    chn_setSamplePeriod(chn, chn_getSamplePeriod(chn) - ((chn_getEffectParam(chn) & 0x0f) << 2));
}

METHOD_HANDLE(pitchUp_extra)
{
    chn_setSamplePeriod(chn, chn_getSamplePeriod(chn) - (chn_getEffectParam(chn) & 0x0f));
}

/*** Portamento to note ***/

void __near eff_porta_start(MIXCHN *chn)
{
    chnState_porta_flag = true;
    if (_isNote(chnState_cur_bNote))
    {
        /* now save some values (we want to slide from) */
        chnState_porta_dSmpStepOld   = chn_getSampleStep(chn);
        chnState_porta_wSmpPeriodOld = chn_getSamplePeriod(chn);
    };
}

void __near eff_porta_stop(MIXCHN *chn)
{
    chnState_porta_flag = false;
    chn_setCommand(chn, EFFIDX_NONE);
}

METHOD_INIT(porta)
{
    param = checkPara0not(chn, param);

    if (param)
        chn->bPortParam = param;

    if (chn_getState(chn))
        eff_porta_start(chn);
    else
        eff_porta_stop(chn);

    return true;
}

METHOD_HANDLE(porta)
{
    if (_isNote(chnState_cur_bNote))
    {
        chn->wSmpPeriodDest = chn_getSamplePeriod(chn);
        _chn_setSamplePeriod(chn, chnState_porta_wSmpPeriodOld);
        chn_setSampleStep(chn, chnState_porta_dSmpStepOld);
    };
}

METHOD_TICK(porta)
{
    long period = chn_getSamplePeriod(chn);
    unsigned int slide = chn->bPortParam << 2;   // <- use amiga slide = para*4
    if (period > chn->wSmpPeriodDest)
    {
        period -= slide;
        if (period < chn->wSmpPeriodDest)
            period = chn->wSmpPeriodDest;
    }
    else
    {
        period += slide;
        if (period > chn->wSmpPeriodDest)
            period = chn->wSmpPeriodDest;
    };
    chn_setSamplePeriod(chn, period);
}

/*** Portamento to note + Volume slide ***/

METHOD_INIT(porta_vol)
{
    bool state;
    state = EFFECT(porta).init(chn, 0);
    if (chnState_porta_flag)
        state |= EFFECT(volSlide).init(chn, param);
    return state;
}

METHOD_HANDLE(porta_vol)
{
    EFFECT(volSlide).handle(chn);
    EFFECT(porta).handle(chn);
}

METHOD_TICK(porta_vol)
{
    EFFECT(volSlide).tick(chn);
    EFFECT(porta).tick(chn);
}

/*** Vibrato (normal) ***/

METHOD_INIT(vibNorm)
{
    param = checkPara0not(chn, param);

    if (chn->bEffFlags & EFFFLAG_CONTINUE)
        chn->bTabPos = 0;

    if (!param)
        param = chn->bVibParam;
    else
        if (!(param & 0xf0))
            param |= chn->bVibParam & 0xf0;

    chn->bVibParam = param;
    return true;
}

METHOD_HANDLE(vibNorm)
{
    if (!(chn->bEffFlags & EFFFLAG_CONTINUE))
        chn->wSmpPeriodOld = chn_getSamplePeriod(chn);
}

METHOD_TICK(vibNorm)
{
    unsigned int pos;
    if (chn_getState(chn))
    {
        /* next position in table: */
        pos = (chn->bTabPos + (chn->bVibParam >> 4)) & 0x3f;
        chn->bTabPos = pos;
        chn_setSamplePeriod(chn, chn->wSmpPeriodOld +
            ((get_i8_value(chn->wVibTab, pos) * (chn->bVibParam & 0x0f)) >> 4));
    };
}

METHOD_CONT(vibNorm)
{
    return !_isNote(chnState_cur_bNote);
}

METHOD_STOP(vibNorm)
{
    unsigned int period;
    period = chn->wSmpPeriodOld;
    chn->wSmpPeriod = period;
    if (period)
        chn->dSmpStep = mixCalcSampleStep(period);
}

/*** Vibrato (fine) ***/

METHOD_TICK(vibFine)
{
    unsigned int pos;
    if (chn_getState(chn))
    {
        /* next position in table: */
        pos = (chn->bTabPos + (chn->bVibParam >> 4)) & 0x3f;
        chn->bTabPos = pos;
        chn_setSamplePeriod(chn, chn->wSmpPeriodOld +
            ((get_i8_value(chn->wVibTab, pos) * (chn->bVibParam & 0x0f)) >> 8));
    };
}

/*** Vibrato (normal) + Volume slide ***/

METHOD_INIT(vibNorm_vol)
{
    if (!(chn->bEffFlags & EFFFLAG_CONTINUE))
        chn->bTabPos = 0;
    EFFECT(volSlide).init(chn, param);
    return true;
}

METHOD_HANDLE(vibNorm_vol)
{
    EFFECT(volSlide).handle(chn);
    EFFECT(vibNorm).handle(chn);
}

METHOD_TICK(vibNorm_vol)
{
    EFFECT(volSlide).tick(chn);
    EFFECT(vibNorm).tick(chn);
}

/*** Tremor ***/

METHOD_INIT(tremor)
{
    param = checkPara0(chn, param);
    // TODO
    return true;
}

METHOD_TICK(tremor)
{
    // TODO
    return;
}

/*** Arpeggio ***/

METHOD_INIT(arpeggio)
{
    if (param)
    {
        chnState_arp_bFlag = true;
        chn_setEffectParam(chn, param);
    }
    else
        chnState_arp_bFlag = false;
    return true;
}

METHOD_HANDLE(arpeggio)
{
    uint8_t param, note;
    struct instrument_t *ins;

    if (!chnState_arp_bFlag)
    {
        if (chn->bEffFlags & EFFFLAG_CONTINUE)
            return;
        /* start arpeggio: */
        chn->bArpPos = 0;
    };

    param = chn_getEffectParam(chn);

    note = _unpackNote(chn->bNote) + (param >> 4);
    chn->bArpNotes[0] = _packNote(note > NOTE_MAX ? NOTE_MAX : note);

    note = _unpackNote(chn->bNote) + (param & 0x0f);
    chn->bArpNotes[1] = _packNote(note > NOTE_MAX ? NOTE_MAX : note);

    ins = chn_getInstrument(chn);
    chn->dArpSmpSteps[0] = chn_calcNoteStep(chn, ins, chn->bNote);
    chn->dArpSmpSteps[1] = chn_calcNoteStep(chn, ins, chn->bArpNotes[0]);
    chn->dArpSmpSteps[2] = chn_calcNoteStep(chn, ins, chn->bArpNotes[0]);
}

METHOD_TICK(arpeggio)
{
    unsigned int pos = chn->bArpPos + 1;
    if (pos >= 3)
        pos = 0;
    chn->bArpPos = pos;
    chn->dSmpStep = chn->dArpSmpSteps[pos];
}

METHOD_STOP(arpeggio)
{
    chn->dSmpStep = chn->dArpSmpSteps[0];
}

/*** Set sample offset ***/

METHOD_HANDLE(sampleOffset)
{
    uint8_t param;
    param = chn_getEffectParam(chn);
    chn->wSmpStart = param << 8;
    if (_isNote(chnState_cur_bNote))
        chn->dSmpPos = (unsigned long)chn->wSmpStart << 16;
}

/*** Note retrigger + Volume slide ***/

METHOD_INIT(retrig)
{
    uint8_t ticks;
    if (!param)
        param = chn_getEffectParam(chn);
    else
    {
        chn_setEffectParam(chn, param);
        ticks = param & 0x0f;
        if (ticks)
            chn->bRetrigTicks = ticks - 1;
        else
        {
            chn_setCommand(chn, EFFIDX_NONE);
            return true;
        };
    };
    return SUB_EFFECTS_LIST(retrig)[eff_retrig_route[param >> 4]]->init(chn, param);
}

METHOD_TICK(retrig)
{
    unsigned int ticks;
    uint8_t cmd;
    if (chn->bRetrigTicks)
    {
        if (! --chn->bRetrigTicks)
            return;
    };
    chn->dSmpPos = 0;
    ticks = chn_getEffectParam(chn) & 0x0f;
    if (ticks)
    {
        chn->bRetrigTicks = ticks;
        cmd = chn_getSubCommand(chn);
        if (cmd <= 6)
            SUB_EFFECTS_LIST(retrig)[cmd]->tick(chn);
    };
}

METHOD_INIT(retrig_none)
{
    chn_setSubCommand(chn, EFFIDX_RETRIG_VOLSLIDE_NONE);
    return true;
}

METHOD_INIT(retrig_slideDown)
{
    chn_setSubCommand(chn, EFFIDX_RETRIG_VOLSLIDE_DOWN);
    return true;
}

METHOD_INIT(retrig_use2div3)
{
    chn_setSubCommand(chn, EFFIDX_RETRIG_VOLSLIDE_USE2DIV3);
    return true;
}

METHOD_INIT(retrig_use1div2)
{
    chn_setSubCommand(chn, EFFIDX_RETRIG_VOLSLIDE_USE1DIV2);
    return true;
}

METHOD_INIT(retrig_slideUp)
{
    chn_setSubCommand(chn, EFFIDX_RETRIG_VOLSLIDE_UP);
    return true;
}

METHOD_INIT(retrig_use3div2)
{
    chn_setSubCommand(chn, EFFIDX_RETRIG_VOLSLIDE_USE3DIV2);
    return true;
}

METHOD_INIT(retrig_use2div1)
{
    chn_setSubCommand(chn, EFFIDX_RETRIG_VOLSLIDE_USE2DIV1);
    return true;
}

METHOD_TICK(retrig_slideDown)
{
    chn_setSampleVolume(chn, chn->bSmpVol - (int)(1 << (chn_getEffectParam(chn) >> 4)));
}

METHOD_TICK(retrig_use2div3)
{
    chn_setSampleVolume(chn, (int)(chn->bSmpVol * 2) / 3);
}

METHOD_TICK(retrig_use1div2)
{
    chn_setSampleVolume(chn, chn->bSmpVol >> 1);
}

METHOD_TICK(retrig_slideUp)
{
    chn_setSampleVolume(chn, chn->bSmpVol + (int)(1 << (chn_getEffectParam(chn) >> 4)));
}

METHOD_TICK(retrig_use3div2)
{
    chn_setSampleVolume(chn, (int)(chn->bSmpVol * 3) >> 1);
}

METHOD_TICK(retrig_use2div1)
{
    chn_setSampleVolume(chn, (int)chn->bSmpVol << 1);
}

/*** Tremolo ***/

METHOD_INIT(tremolo)
{
    if (!(chn->bEffFlags & EFFFLAG_CONTINUE))
        chn->bTabPos = 0;
    if (param)
    {
        if (!(param & 0xf0))
            param |= chn_getEffectParam(chn) & 0xf0;
        chn_setEffectParam(chn, param);
    };
    return true;
}

METHOD_HANDLE(tremolo)
{
    if ((chnState_cur_bIns != 0)
    ||  (chnState_cur_bVol != CHNINSVOL_EMPTY)
    ||  (!(chn->bEffFlags & EFFFLAG_CONTINUE)))
        chn->bSmpVolOld = chn->bSmpVol;
}

METHOD_TICK(tremolo)
{
    unsigned int pos;
    uint8_t param = chn_getEffectParam(chn);
    /* next position in table: */
    pos = (chn->bTabPos + (param >> 4)) & 0x3f;
    chn->bTabPos = pos;
    chn_setSampleVolume(chn, chn->bSmpVolOld +
        ((get_i8_value(chn->wTrmTab, pos) * (param & 0x0f)) >> 6));
}

/*** Special effects ***/

METHOD_HANDLE(special_fineTune)
{
    // TODO
    return;
}

METHOD_INIT(special_setVibWave)
{
    // FIXME: random wave is not a table but a call to a random()
    param &= 3;
    if (param == 3)
        chn->wVibTab = wavetab[0];
    else
        chn->wVibTab = wavetab[param];
    return true;
}

METHOD_INIT(special_setTremWave)
{
    // FIXME: random wave is not a table but a call to a random()
    param &= 3;
    if (param == 3)
        chn->wTrmTab = wavetab[0];
    else
        chn->wTrmTab = wavetab[param];
    return true;
}

METHOD_INIT(special_patLoop)
{
    if (!param)
        playState_patLoopStartRow = playState_row;
    else
    {
        if (!playState_patLoopActive)
        {
            playState_patLoopActive = true;
            param++;
            playState_patLoopCount = param;
        };
        playState_patLoop_bNow = true;
    };

    return true;
}

METHOD_TICK(special_noteCut)
{
    if (! --chn->bDelayTicks)
        chn_setState(chn, false);
}

METHOD_INIT(special_noteDelay)
{
    chn->bDelayTicks = param;
    if (!playState_patDelay_bNow)
    {
        /* new note, instrument, volume for later use */
        chn->bSavNote = chnState_cur_bNote;
        chn->bSavIns = chnState_cur_bIns;
        chn->bSavVol = chnState_cur_bVol;
    }
    return false;   /* setup note, instrument, volume later */
}

METHOD_TICK(special_noteDelay)
{
    unsigned int insNum, note;
    if (! --chn->bDelayTicks)
    {
        insNum = chn->bSavIns;
        if (insNum)
            chn_setupInstrument(chn, insNum);
        note = chn->bSavNote;
        if (note != CHNNOTE_EMPTY)
        {
            if (note == CHNNOTE_OFF)
                chn_setState(chn, false);
            else
            {
                chn_setupNote(chn, note, 0);
                chn_setState(chn, true);
            };
        };
        if (chn->bSavVol != CHNINSVOL_EMPTY)
            chn_setSampleVolume(chn, (chn->bSavVol * playState_gVolume) >> 6);
        chn_setCommand(chn, EFFIDX_NONE);
    };
}

METHOD_INIT(special_patDelay)
{
    if (!playState_patDelay_bNow)
    {
        playState_patDelayCount = param + 1;
        chnState_patDelay_bParameterSaved = chn_getEffectParam(chn);
    };
    return true;
}

METHOD_HANDLE(special_patDelay)
{
    chn_setCommand(chn, chnState_patDelay_bCommandSaved);
    chn_setSubCommand(chn, 0);
    chn_setEffectParam(chn, chnState_patDelay_bParameterSaved);
}

METHOD_INIT(special)
{
    uint8_t cmd;
    param = checkPara0(chn, param);
    cmd = eff_special_route[param >> 4];
    chn_setSubCommand(chn, cmd);
    return SUB_EFFECTS_LIST(special)[cmd]->init(chn, param & 0x0f);
}

METHOD_TICK(special)
{
    uint8_t cmd;
    cmd = chn_getSubCommand(chn);
    if (cmd <= EFFIDX_SPECIAL_MAX)
        SUB_EFFECTS_LIST(special)[cmd]->tick(chn);
}

/*** General effects handling ***/

bool PUBLIC_CODE chn_effInit(MIXCHN *chn, uint8_t param)
{
    uint8_t cmd;
    cmd = chn_getCommand(chn);
    if (cmd <= MAXEFF)
        return EFFECTS_LIST(main)[cmd]->init(chn, param);
    else
        return false;
}

void PUBLIC_CODE chn_effHandle(MIXCHN *chn)
{
    uint8_t cmd;
    cmd = chn_getCommand(chn);
    if (cmd <= MAXEFF)
        EFFECTS_LIST(main)[cmd]->handle(chn);
}

void PUBLIC_CODE chn_effTick(MIXCHN *chn)
{
    uint8_t cmd;
    cmd = chn_getCommand(chn);
    if (cmd <= MAXEFF)
        EFFECTS_LIST(main)[cmd]->tick(chn);
}

bool PUBLIC_CODE chn_effCanContinue(MIXCHN *chn)
{
    uint8_t cmd;
    cmd = chn_getCommand(chn);
    if (cmd <= MAXEFF)
        return EFFECTS_LIST(main)[cmd]->cont(chn);
    else
        return false;
}

void PUBLIC_CODE chn_effStop(MIXCHN *chn)
{
    uint8_t cmd;
    cmd = chn_getCommand(chn);
    if (cmd <= MAXEFF)
        EFFECTS_LIST(main)[cmd]->stop(chn);
}
