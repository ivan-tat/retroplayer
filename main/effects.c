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

#include "s3mtypes.h"
#include "s3mvars.h"
#include "effvars.h"
#include "mixvars.h"
#include "mixer.h"

#define CHNNOTE_EMPTY 0xff
#define CHNNOTE_OFF 0xfe

#define CHNINSVOL_EMPTY 0xff
#define CHNINSVOL_MAX 63

#define MID_C_RATE 8363

#define getNotePeriod(note) ((ST3Periods[(note) & 0x0f] << 4) >> ((note) >> 4))

#define effTickProc(name) void __near name(MIXCHN *chn)

typedef void __near effTickProc_t(MIXCHN *chn);

effTickProc(effTick_none);
/* A - */
/* B - */
/* C - */
effTickProc(effTick_D_VolumeSlide);
effTickProc(effTick_E_PitchDown);
effTickProc(effTick_F_PitchUp);
effTickProc(effTick_G_Portamento);
effTickProc(effTick_H_Vibrato);
effTickProc(effTick_I_Tremor);
effTickProc(effTick_J_Arpeggio);
effTickProc(effTick_K_VibratoVolSlide);
effTickProc(effTick_L_PortamentoVolSlide);
/* M - */
/* N - */
/* O - */
/* P - */
effTickProc(effTick_Q_Retrigger);
effTickProc(effTick_R_Tremolo);
effTickProc(effTick_S_Special);
/* T - */
effTickProc(effTick_U_FineVibrato);
/* V - */
/* W - */
/* X - */
/* Y - */
/* Z - */

#define MAXEFF 22

const static effTickProc_t *effTick_tab[] = {
    effTick_none,
    effTick_none,
    effTick_none,
    effTick_none,
    effTick_D_VolumeSlide,
    effTick_E_PitchDown,
    effTick_F_PitchUp,
    effTick_G_Portamento,
    effTick_H_Vibrato,
    effTick_I_Tremor,
    effTick_J_Arpeggio,
    effTick_K_VibratoVolSlide,
    effTick_L_PortamentoVolSlide,
    effTick_none,
    effTick_none,
    effTick_none,
    effTick_none,
    effTick_Q_Retrigger,
    effTick_R_Tremolo,
    effTick_S_Special,
    effTick_none,
    effTick_U_FineVibrato,
    effTick_none
};

/* D */

effTickProc(effTick_D_VolumeSlide_Down);
effTickProc(effTick_D_VolumeSlide_Up);

const static effTickProc_t *effTick_D_VolumeSlide_tab[] = {
    effTick_D_VolumeSlide_Down,
    effTick_D_VolumeSlide_Up,
    effTick_none,
    effTick_none
};

/* E */

effTickProc(effTick_E_PitchDown_Down);

const static effTickProc_t *effTick_E_PitchDown_tab[] = {
    effTick_E_PitchDown_Down,
    effTick_none,
    effTick_none
};

/* F */

effTickProc(effTick_F_PitchUp_Up);

const static effTickProc_t *effTick_F_PitchUp_tab[] = {
    effTick_F_PitchUp_Up,
    effTick_none,
    effTick_none
};

/* Q */

effTickProc(effTick_Q_Retrigger_SlideDown);
effTickProc(effTick_Q_Retrigger_Use2div3);
effTickProc(effTick_Q_Retrigger_Use1div2);
effTickProc(effTick_Q_Retrigger_SlideUp);
effTickProc(effTick_Q_Retrigger_Use3div2);
effTickProc(effTick_Q_Retrigger_Use2div1);

const static effTickProc_t *effTick_Q_Retrigger_tab[] = {
    effTick_none,
    effTick_Q_Retrigger_SlideDown,
    effTick_Q_Retrigger_Use2div3,
    effTick_Q_Retrigger_Use1div2,
    effTick_Q_Retrigger_SlideUp,
    effTick_Q_Retrigger_Use3div2,
    effTick_Q_Retrigger_Use2div1
};

/* S */

effTickProc(effTick_S_Special_C_NoteCut);
effTickProc(effTick_S_Special_D_NoteDelay);

const static effTickProc_t *effTick_S_Special_tab[] = {
    effTick_none,   // N/A
    effTick_none,   // set filter
    effTick_none,   // set glissando
    effTick_none,   // set finetune
    effTick_none,   // set vibrato waveform
    effTick_none,   // set tremolo waveform
    effTick_none,   // N/A
    effTick_none,   // N/A
    effTick_none,   // panning
    effTick_none,   // N/A
    effTick_none,   // stereo control
    effTick_none,   // Pattern loop things
    effTick_S_Special_C_NoteCut,
    effTick_S_Special_D_NoteDelay,
    effTick_none,   // Pattern delay
    effTick_none    // funkrepeat
};

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

//#ifdef __WATCOMC__
//#pragma aux chn_setupInstrument modify [ es ];
//#endif

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

//#ifdef __WATCOMC__
//#pragma aux chn_setupNote modify [ es ];
//#endif

void PUBLIC_CODE chn_effTick(MIXCHN *chn)
{
    if (chn->wCommand >> 1 <= MAXEFF) effTick_tab[chn->wCommand >> 1](chn);
}

//#ifdef __WATCOMC__
//#pragma aux chn_effTick modify [ es ];
//#endif

effTickProc(effTick_none)
{
    return;
}

effTickProc(effTick_D_VolumeSlide)
{
    if (chn->wCommand2 >> 1 <= 3) effTick_D_VolumeSlide_tab[chn->wCommand2 >> 1](chn);
}

effTickProc(effTick_D_VolumeSlide_Down)
{
    chn_setSampleVolume(chn, chn->bSmpVol - (chn->bParameter & 0x0f));
}

effTickProc(effTick_D_VolumeSlide_Up)
{
    chn_setSampleVolume(chn, ((chn->bSmpVol + (chn->bParameter >> 4)) * GVolume) >> 6);
}

effTickProc(effTick_E_PitchDown)
{
    if (chn->wCommand2 >> 1 <= 2) effTick_E_PitchDown_tab[chn->wCommand2 >> 1](chn);
}

effTickProc(effTick_E_PitchDown_Down)
{
    chn_setSamplePeriod(chn, chn->wSmpPeriod + (chn->bParameter << 2));
}

effTickProc(effTick_F_PitchUp)
{
    if (chn->wCommand2 >> 1 <= 2) effTick_F_PitchUp_tab[chn->wCommand2 >> 1](chn);
}

effTickProc(effTick_F_PitchUp_Up)
{
    chn_setSamplePeriod(chn, chn->wSmpPeriod - (chn->bParameter << 2));
}

effTickProc(effTick_G_Portamento)
{
    long period = chn->wSmpPeriod;
    unsigned int slide = chn->bPortParam << 2;   // <- use amiga slide = para*4
    if (period > chn->wSmpPeriodDest) {
        period -= slide;
        if (period < chn->wSmpPeriodDest) period = chn->wSmpPeriodDest;
    } else {
        period += slide;
        if (period > chn->wSmpPeriodDest) period = chn->wSmpPeriodDest;
    };
    chn_setSamplePeriod(chn, period);
}

effTickProc(effTick_H_Vibrato)
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

effTickProc(effTick_I_Tremor)
{
    // TODO
    return;
}

effTickProc(effTick_J_Arpeggio)
{
    unsigned int pos = chn->bArpPos + 1;
    if (pos >= 3) pos = 0;
    chn->bArpPos = pos;
    chn->dSmpStep = chn->dArpSmpSteps[pos];
}

effTickProc(effTick_K_VibratoVolSlide)
{
    effTick_D_VolumeSlide(chn);
    effTick_H_Vibrato(chn);
}

effTickProc(effTick_L_PortamentoVolSlide)
{
    effTick_D_VolumeSlide(chn);
    effTick_G_Portamento(chn);
}

effTickProc(effTick_Q_Retrigger)
{
    unsigned int ticks;
    if (chn->bRetrigTicks) {
        if (! --chn->bRetrigTicks) return;
    };
    chn->dSmpPos = 0;
    ticks = chn->bParameter & 0x0f;
    if (ticks) {
        chn->bRetrigTicks = ticks;
        if (chn->wCommand2 >> 1 <= 6) effTick_Q_Retrigger_tab[chn->wCommand2 >> 1](chn);
    };
}

effTickProc(effTick_Q_Retrigger_SlideDown)
{
    chn_setSampleVolume(chn, chn->bSmpVol - (int)(1 << (chn->bParameter >> 4)));
}

effTickProc(effTick_Q_Retrigger_Use2div3)
{
    chn_setSampleVolume(chn, (int)(chn->bSmpVol * 2) / 3);
}

effTickProc(effTick_Q_Retrigger_Use1div2)
{
    chn_setSampleVolume(chn, chn->bSmpVol >> 1);
}

effTickProc(effTick_Q_Retrigger_SlideUp)
{
    chn_setSampleVolume(chn, chn->bSmpVol + (int)(1 << (chn->bParameter >> 4)));
}

effTickProc(effTick_Q_Retrigger_Use3div2)
{
    chn_setSampleVolume(chn, (int)(chn->bSmpVol * 3) >> 1);
}

effTickProc(effTick_Q_Retrigger_Use2div1)
{
    chn_setSampleVolume(chn, (int)chn->bSmpVol << 1);
}

effTickProc(effTick_R_Tremolo)
{
    /* next position in table: */
    chn->bTabPos = (chn->bTabPos + (chn->bParameter >> 4)) & 0x3f;
    chn_setSampleVolume(chn, chn->bSmpVolOld +
        ((*(int8_t *)MK_FP(FP_SEG(&wavetab), chn->wTrmTab + chn->bTabPos) *
        (chn->bParameter & 0x0f)) >> 6));
}

effTickProc(effTick_S_Special)
{
    if (chn->wCommand2 >> 1 <= 15) effTick_S_Special_tab[chn->wCommand2 >> 1](chn);
}

effTickProc(effTick_S_Special_C_NoteCut)
{
    if (! --chn->bDelayTicks) chn->bEnabled = 0;  // disable it
}

effTickProc(effTick_S_Special_D_NoteDelay)
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
        chn->wCommand = 0;  // no more NoteDelay
    };
}

effTickProc(effTick_U_FineVibrato)
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
