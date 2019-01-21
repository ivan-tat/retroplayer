/* effects.c -- effects handling.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/stdio.h"
#include "dos/ems.h"
#include "main/s3mtypes.h"
#include "main/s3mvars.h"
#include "main/effvars.h"
#include "main/mixer.h"
#include "main/muspat.h"
#include "main/effects.h"

#define get_i8_value(off, pos) *(int8_t *)MK_FP(FP_SEG(&wavetab), off + pos)

/*** Effects ***/

// Effect's method
typedef bool __near emInit_t  (MIXCHN *chn, uint8_t param);
typedef void __near emHandle_t(MIXCHN *chn);
typedef void __near emTick_t  (MIXCHN *chn);
typedef bool __near emCont_t  (MIXCHN *chn);
typedef void __near emStop_t  (MIXCHN *chn);
typedef void __near emGetName_t (MIXCHN *chn, char *__s, size_t __maxlen);

// Effect's methods table
typedef struct effMethodsTable_t
{
    emInit_t   __near *init;
    emHandle_t __near *handle;
    emTick_t   __near *tick;
    emCont_t   __near *cont;
    emStop_t   __near *stop;
    emGetName_t __near *get_name;
};
typedef struct effMethodsTable_t EFFMT;

// Effect's method name (effm)
#define NAME_INIT(name)     effm_##name##_init
#define NAME_HANDLE(name)   effm_##name##_handle
#define NAME_TICK(name)     effm_##name##_tick
#define NAME_CONT(name)     effm_##name##_cont
#define NAME_STOP(name)     effm_##name##_stop
#define NAME_GET_NAME(name) effm_##name##_get_name

// Effect's method definition
#define METHOD_INIT(name)   bool __near NAME_INIT(name)  (MIXCHN *chn, uint8_t param)
#define METHOD_HANDLE(name) void __near NAME_HANDLE(name)(MIXCHN *chn)
#define METHOD_TICK(name)   void __near NAME_TICK(name)  (MIXCHN *chn)
#define METHOD_CONT(name)   bool __near NAME_CONT(name)  (MIXCHN *chn)
#define METHOD_STOP(name)   void __near NAME_STOP(name)  (MIXCHN *chn)
#define METHOD_GET_NAME(name) void __near NAME_GET_NAME (name) (MIXCHN *chn, char *__s, size_t __maxlen)

#define DEFINE_METHOD_INIT(name)     METHOD_INIT (name)
#define DEFINE_METHOD_HANDLE(name)   METHOD_HANDLE (name)
#define DEFINE_METHOD_TICK(name)     METHOD_TICK (name)
#define DEFINE_METHOD_CONT(name)     METHOD_CONT (name)
#define DEFINE_METHOD_STOP(name)     METHOD_STOP (name)
#define DEFINE_METHOD_GET_NAME(name) METHOD_GET_NAME (name)

// Effect's description (effd)
#define EFFECT(name) effd_##name
#define DEFINE_EFFECT(name, init, handle, tick, cont, stop, get_name) \
static const EFFMT __near EFFECT(name) = \
{ \
    NAME_INIT  (init), \
    NAME_HANDLE(handle), \
    NAME_TICK  (tick), \
    NAME_CONT  (cont), \
    NAME_STOP  (stop), \
    NAME_GET_NAME(get_name) \
}

// Effects descriptions list (effdl)
#define EFFECTS_LIST(name) effdl_##name
#define DEFINE_EFFECTS_LIST(name) static const EFFMT __near *EFFECTS_LIST(name)[]

// Sub-effects description (effsd)
#define SUB_EFFECT(name) effsd_##name
#define DEFINE_SUB_EFFECT(name, init, handle, tick, cont, stop) \
static const EFFMT __near SUB_EFFECT(name) = \
{ \
    NAME_INIT  (init), \
    NAME_HANDLE(handle), \
    NAME_TICK  (tick), \
    NAME_CONT  (cont), \
    NAME_STOP  (stop) \
}

// Sub-effects descriptions list (effsdl)
#define SUB_EFFECTS_LIST(name) effsdl_##name
#define DEFINE_SUB_EFFECTS_LIST(name) static const EFFMT __near *SUB_EFFECTS_LIST(name)[]

// Sub-effects names list (effsnl)
#define SUB_EFFECTS_NAME_LIST(name) effsnl_##name
#define DEFINE_SUB_EFFECTS_NAME_LIST(name) static const char __near *SUB_EFFECTS_NAME_LIST (name)[]

/*** No effect ***/

// effect continue checks

DEFINE_METHOD_CONT(allow);
DEFINE_METHOD_CONT(deny);

// empty effect

DEFINE_METHOD_INIT     (none);
DEFINE_METHOD_HANDLE   (none);
DEFINE_METHOD_TICK     (none);
DEFINE_METHOD_STOP     (none);
DEFINE_METHOD_GET_NAME (none);
DEFINE_EFFECT (none, none, none, none, deny, none, none);

// empty effect with name 'n/a'

DEFINE_METHOD_INIT     (none);
DEFINE_METHOD_HANDLE   (none);
DEFINE_METHOD_TICK     (none);
DEFINE_METHOD_STOP     (none);
DEFINE_METHOD_GET_NAME (none_na);
DEFINE_EFFECT (none_na, none, none, none, deny, none, none_na);

// empty sub-effect

DEFINE_SUB_EFFECT(none, none, none, none, deny, none);

/****** Global effects ******/

/*** Set tempo ***/
/* Scream Tracker 3 command: T */

DEFINE_METHOD_INIT     (setTempo);
DEFINE_METHOD_GET_NAME (setTempo);
DEFINE_EFFECT (setTempo, setTempo, none, none, deny, none, setTempo);

/*** Set speed ***/
/* Scream Tracker 3 command: A */

DEFINE_METHOD_INIT     (setSpeed);
DEFINE_METHOD_GET_NAME (setSpeed);
DEFINE_EFFECT (setSpeed, setSpeed, none, none, deny, none, setSpeed);

/*** Jump to order ***/
/* Scream Tracker 3 command: B */

DEFINE_METHOD_INIT     (jumpToOrder);
DEFINE_METHOD_GET_NAME (jumpToOrder);
DEFINE_EFFECT (jumpToOrder, jumpToOrder, none, none, deny, none, jumpToOrder);

/*** Pattern break ***/
/* Scream Tracker 3 command: C */

DEFINE_METHOD_INIT     (patBreak);
DEFINE_METHOD_GET_NAME (patBreak);
DEFINE_EFFECT (patBreak, patBreak, none, none, deny, none, patBreak);

/*** Set global volume ***/
/* Scream Tracker 3 command: V */

DEFINE_METHOD_INIT     (setGVol);
DEFINE_METHOD_GET_NAME (setGVol);
DEFINE_EFFECT (setGVol, setGVol, none, none, deny, none, setGVol);

// TODO: move here pattern delay

/****** Channel effects ******/

/*** Volume slide ***/
/* Scream Tracker 3 command: D */

// sub-effects

DEFINE_METHOD_TICK(volSlide_down);
DEFINE_SUB_EFFECT(volSlide_down, none, none, volSlide_down, allow, none);

DEFINE_METHOD_TICK(volSlide_up);
DEFINE_SUB_EFFECT(volSlide_up, none, none, volSlide_up, allow, none);

DEFINE_METHOD_HANDLE(volSlide_fineDown);
DEFINE_SUB_EFFECT(volSlide_fineDown, none, volSlide_fineDown, none, allow, none);

DEFINE_METHOD_HANDLE(volSlide_fineUp);
DEFINE_SUB_EFFECT(volSlide_fineUp, none, volSlide_fineUp, none, allow, none);

// sub-effects list

DEFINE_SUB_EFFECTS_LIST(volSlide) =
{
    &SUB_EFFECT(volSlide_down),
    &SUB_EFFECT(volSlide_up),
    &SUB_EFFECT(volSlide_fineDown),
    &SUB_EFFECT(volSlide_fineUp)
};

DEFINE_SUB_EFFECTS_NAME_LIST (volSlide) =
{
    "vol.dn (norm)",
    "vol.up (norm)",
    "vol.dn (fine)",
    "vol.up (fine)"
};

#define EFFIDX_VOLSLIDE_DOWN      0
#define EFFIDX_VOLSLIDE_UP        1
#define EFFIDX_VOLSLIDE_FINE_DOWN 2
#define EFFIDX_VOLSLIDE_FINE_UP   3
#define EFFIDX_VOLSLIDE_MAX       3

// router

DEFINE_METHOD_INIT     (volSlide);
DEFINE_METHOD_HANDLE   (volSlide);
DEFINE_METHOD_TICK     (volSlide);
DEFINE_METHOD_GET_NAME (volSlide);
DEFINE_EFFECT (volSlide, volSlide, volSlide, volSlide, allow, none, volSlide);

/*** Pitch slide down ***/
/* Scream Tracker 3 command: E */

// sub-effects

DEFINE_METHOD_TICK(pitchDown_normal);
DEFINE_SUB_EFFECT(pitchDown_normal, none, none, pitchDown_normal, allow, none);

DEFINE_METHOD_HANDLE(pitchDown_fine);
DEFINE_SUB_EFFECT(pitchDown_fine, none, pitchDown_fine, none, allow, none);

DEFINE_METHOD_HANDLE(pitchDown_extra);
DEFINE_SUB_EFFECT(pitchDown_extra, none, pitchDown_extra, none, allow, none);

// sub-effects list

DEFINE_SUB_EFFECTS_LIST(pitchDown) =
{
    &SUB_EFFECT(pitchDown_normal),
    &SUB_EFFECT(pitchDown_fine),
    &SUB_EFFECT(pitchDown_extra)
};

DEFINE_SUB_EFFECTS_NAME_LIST (pitchDown) =
{
    "pit.dn (norm)",
    "pit.dn (fine)",
    "pit.dn (extr)"
};

#define EFFIDX_PITCHDOWN_NORMAL 0
#define EFFIDX_PITCHDOWN_FINE   1
#define EFFIDX_PITCHDOWN_EXTRA  2
#define EFFIDX_PITCHDOWN_MAX    2

// router

DEFINE_METHOD_INIT     (pitchDown);
DEFINE_METHOD_HANDLE   (pitchDown);
DEFINE_METHOD_TICK     (pitchDown);
DEFINE_METHOD_GET_NAME (pitchDown);
DEFINE_EFFECT (pitchDown, pitchDown, pitchDown, pitchDown, allow, none, pitchDown);

/*** Pitch slide up ***/
/* Scream Tracker 3 command: F */

// sub-effects

DEFINE_METHOD_TICK(pitchUp_normal);
DEFINE_SUB_EFFECT(pitchUp_normal, none, none, pitchUp_normal, allow, none);

DEFINE_METHOD_HANDLE(pitchUp_fine);
DEFINE_SUB_EFFECT(pitchUp_fine, none, pitchUp_fine, none, allow, none);

DEFINE_METHOD_HANDLE(pitchUp_extra);
DEFINE_SUB_EFFECT(pitchUp_extra, none, pitchUp_extra, none, allow, none);

// sub-effects list

DEFINE_SUB_EFFECTS_LIST(pitchUp) =
{
    &SUB_EFFECT(pitchUp_normal),
    &SUB_EFFECT(pitchUp_fine),
    &SUB_EFFECT(pitchUp_extra)
};

DEFINE_SUB_EFFECTS_NAME_LIST (pitchUp) =
{
    "pit.up (norm)",
    "pit.up (fine)",
    "pit.up (extr)"
};

#define EFFIDX_PITCHUP_NORMAL 0
#define EFFIDX_PITCHUP_FINE   1
#define EFFIDX_PITCHUP_EXTRA  2
#define EFFIDX_PITCHUP_MAX    2

// router

DEFINE_METHOD_INIT     (pitchUp);
DEFINE_METHOD_HANDLE   (pitchUp);
DEFINE_METHOD_TICK     (pitchUp);
DEFINE_METHOD_GET_NAME (pitchUp);
DEFINE_EFFECT (pitchUp, pitchUp, pitchUp, pitchUp, allow, none, pitchUp);

/*** Portamento to note ***/
/* Scream Tracker 3 command: G */

DEFINE_METHOD_INIT     (porta);
DEFINE_METHOD_HANDLE   (porta);
DEFINE_METHOD_TICK     (porta);
DEFINE_METHOD_GET_NAME (porta);
DEFINE_EFFECT (porta, porta, porta, porta, allow, none, porta);

/*** Portamento to note + Volume slide ***/
/* Scream Tracker 3 command: L (G + D) */

DEFINE_METHOD_INIT     (porta_vol);
DEFINE_METHOD_HANDLE   (porta_vol);
DEFINE_METHOD_TICK     (porta_vol);
DEFINE_METHOD_GET_NAME (porta_vol);
DEFINE_EFFECT (porta_vol, porta_vol, porta_vol, porta_vol, allow, none, porta_vol);

DEFINE_SUB_EFFECTS_NAME_LIST (porta_vol) =
{
    "port.+vol.dn (norm)",
    "port.+vol.up (norm)",
    "port.+vol.dn (fine)",
    "port.+vol.up (fine)"
};

/*** Vibrato (normal) ***/
/* Scream Tracker 3 command: H */

DEFINE_METHOD_INIT     (vibNorm);
DEFINE_METHOD_HANDLE   (vibNorm);
DEFINE_METHOD_TICK     (vibNorm);
DEFINE_METHOD_CONT     (vibNorm);
DEFINE_METHOD_STOP     (vibNorm);
DEFINE_METHOD_GET_NAME (vibNorm);
DEFINE_EFFECT (vibNorm, vibNorm, vibNorm, vibNorm, vibNorm, vibNorm, vibNorm);

/*** Vibrato (fine) ***/
/* Scream Tracker 3 command: U */

DEFINE_METHOD_TICK     (vibFine);
DEFINE_METHOD_GET_NAME (vibFine);
DEFINE_EFFECT (vibFine, vibNorm, none, vibFine, vibNorm, vibNorm, vibFine);

/*** Vibrato (normal) + Volume slide ***/
/* Scream Tracker 3 command: K (H + D) */

DEFINE_METHOD_INIT     (vibNorm_vol);
DEFINE_METHOD_HANDLE   (vibNorm_vol);
DEFINE_METHOD_TICK     (vibNorm_vol);
DEFINE_METHOD_GET_NAME (vibNorm_vol);
DEFINE_EFFECT (vibNorm_vol, vibNorm_vol, vibNorm_vol, vibNorm_vol, vibNorm, vibNorm, vibNorm_vol);

DEFINE_SUB_EFFECTS_NAME_LIST (vibNorm_vol) =
{
    "vib.+vol.dn (norm)",
    "vib.+vol.up (norm)",
    "vib.+vol.dn (fine)",
    "vib.+vol.up (fine)"
};

/*** Tremor ***/
/* Scream Tracker 3 command: I */

DEFINE_METHOD_INIT     (tremor);
DEFINE_METHOD_TICK     (tremor);
DEFINE_METHOD_GET_NAME (tremor);
DEFINE_EFFECT (tremor, tremor, none, tremor, allow, none, tremor);

/*** Arpeggio ***/
/* Scream Tracker 3 command: J */

DEFINE_METHOD_INIT     (arpeggio);
DEFINE_METHOD_HANDLE   (arpeggio);
DEFINE_METHOD_TICK     (arpeggio);
DEFINE_METHOD_STOP     (arpeggio);
DEFINE_METHOD_GET_NAME (arpeggio);
DEFINE_EFFECT (arpeggio, arpeggio, arpeggio, arpeggio, allow, arpeggio, arpeggio);

/*** Set sample offset ***/
/* Scream Tracker 3 command: O */

DEFINE_METHOD_HANDLE   (sampleOffset);
DEFINE_METHOD_GET_NAME (sampleOffset);
DEFINE_EFFECT (sampleOffset, none, sampleOffset, none, deny, none, sampleOffset);

/*** Note retrigger + Volume slide ***/
/* Scream Tracker 3 command: Q */

DEFINE_METHOD_INIT     (retrig);
DEFINE_METHOD_TICK     (retrig);
DEFINE_METHOD_GET_NAME (retrig);
DEFINE_EFFECT (retrig, retrig, none, retrig, allow, none, retrig);

// sub-effects

DEFINE_METHOD_INIT(retrig_none);
DEFINE_SUB_EFFECT(retrig_none, retrig_none, none, none, allow, none);

DEFINE_METHOD_INIT(retrig_slideDown);
DEFINE_METHOD_TICK(retrig_slideDown);
DEFINE_SUB_EFFECT(retrig_slideDown, retrig_slideDown, none, retrig_slideDown, allow, none);

DEFINE_METHOD_INIT(retrig_use2div3);
DEFINE_METHOD_TICK(retrig_use2div3);
DEFINE_SUB_EFFECT(retrig_use2div3, retrig_use2div3, none, retrig_use2div3, allow, none);

DEFINE_METHOD_INIT(retrig_use1div2);
DEFINE_METHOD_TICK(retrig_use1div2);
DEFINE_SUB_EFFECT(retrig_use1div2, retrig_use1div2, none, retrig_use1div2, allow, none);

DEFINE_METHOD_INIT(retrig_slideUp);
DEFINE_METHOD_TICK(retrig_slideUp);
DEFINE_SUB_EFFECT(retrig_slideUp, retrig_slideUp, none, retrig_slideUp, allow, none);

DEFINE_METHOD_INIT(retrig_use3div2);
DEFINE_METHOD_TICK(retrig_use3div2);
DEFINE_SUB_EFFECT(retrig_use3div2, retrig_use3div2, none, retrig_use3div2, allow, none);

DEFINE_METHOD_INIT(retrig_use2div1);
DEFINE_METHOD_TICK(retrig_use2div1);
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

DEFINE_SUB_EFFECTS_NAME_LIST (retrig) =
{
    "retrig. (no sld)",
    "retrig. (vol.dn)",
    "retrig. (vol2/3)",
    "retrig. (vol./2)",
    "retrig. (vol.up)",
    "retrig. (vol3/2)",
    "retrig. (vol.*2)"
};

#define EFFIDX_RETRIG_VOLSLIDE_NONE     0
#define EFFIDX_RETRIG_VOLSLIDE_DOWN     1
#define EFFIDX_RETRIG_VOLSLIDE_USE2DIV3 2
#define EFFIDX_RETRIG_VOLSLIDE_USE1DIV2 3
#define EFFIDX_RETRIG_VOLSLIDE_UP       4
#define EFFIDX_RETRIG_VOLSLIDE_USE3DIV2 5
#define EFFIDX_RETRIG_VOLSLIDE_USE2DIV1 6
#define EFFIDX_RETRIG_VOLSLIDE_MAX      6

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

DEFINE_METHOD_INIT     (tremolo);
DEFINE_METHOD_HANDLE   (tremolo);
DEFINE_METHOD_TICK     (tremolo);
DEFINE_METHOD_GET_NAME (tremolo);
DEFINE_EFFECT (tremolo, tremolo, tremolo, tremolo, allow, none, tremolo);

/****** Special effects ******/
/* Scream Tracker 3 command: S */

// sub-effects

/*** Fine tune ***/
/* Scream Tracker 3 command: S3 */

DEFINE_METHOD_HANDLE(special_fineTune);
DEFINE_SUB_EFFECT(special_fineTune, none, special_fineTune, none, deny, none);

/*** Set vibrato waveform ***/

DEFINE_METHOD_INIT(special_setVibWave);
DEFINE_SUB_EFFECT(special_setVibWave, special_setVibWave, none, none, deny, none);

/*** Set tremolo waveform ***/

DEFINE_METHOD_INIT(special_setTremWave);
DEFINE_SUB_EFFECT(special_setTremWave, special_setTremWave, none, none, deny, none);

/*** Pattern loop ***/

DEFINE_METHOD_INIT(special_patLoop);
DEFINE_SUB_EFFECT(special_patLoop, special_patLoop, none, none, deny, none);

/*** Note cut ***/
/* Scream Tracker 3 command: SC */

DEFINE_METHOD_TICK(special_noteCut);
DEFINE_SUB_EFFECT(special_noteCut, none, none, special_noteCut, deny, none);

/*** Note delay ***/
/* Scream Tracker 3 command: SD */

DEFINE_METHOD_INIT(special_noteDelay);
DEFINE_METHOD_TICK(special_noteDelay);
DEFINE_SUB_EFFECT(special_noteDelay, special_noteDelay, none, special_noteDelay, deny, none);

/*** Pattern delay ***/
/* Scream Tracker 3 command: SE */

DEFINE_METHOD_INIT  (special_patDelay);
DEFINE_METHOD_HANDLE(special_patDelay);
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

DEFINE_SUB_EFFECTS_NAME_LIST (special) =
{
    NULL,
    "set finetune",
    "set vib. wave",
    "set trm. wave",
    "pat. loop",
    "note cut",
    "note delay",
    "pat. delay"
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
    EFFIDX_SPECIAL_NONE,    /* 0 - n/a */
    EFFIDX_SPECIAL_NONE,    /* 1 - Set filter */
    EFFIDX_SPECIAL_NONE,    /* 2 - Set glissando */
    EFFIDX_SPECIAL_FINETUNE,
    EFFIDX_SPECIAL_VIBWAVE,
    EFFIDX_SPECIAL_TREMWAVE,
    EFFIDX_SPECIAL_NONE,    /* 6 - n/a */
    EFFIDX_SPECIAL_NONE,    /* 7 - n/a */
    EFFIDX_SPECIAL_NONE,    /* 8 - Panning */
    EFFIDX_SPECIAL_NONE,    /* 9 - n/a */
    EFFIDX_SPECIAL_NONE,    /* A - Stereo control */
    EFFIDX_SPECIAL_PATLOOP,
    EFFIDX_SPECIAL_NOTECUT,
    EFFIDX_SPECIAL_NOTEDELAY,
    EFFIDX_SPECIAL_PATDELAY,
    EFFIDX_SPECIAL_NONE     /* SF - Function repeat */
};

// router

DEFINE_METHOD_INIT     (special);
DEFINE_METHOD_TICK     (special);
DEFINE_METHOD_GET_NAME (special);
DEFINE_EFFECT (special, special, none, special, deny, none, special);

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
    &EFFECT(none_na),       // M
    &EFFECT(none_na),       // N
    &EFFECT(sampleOffset),  // O
    &EFFECT(none_na),       // P
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

/*** Effects ***/

uint8_t __near checkPara0(MIXCHN *chn, uint8_t param)
{
    if (param)
        mixchn_set_command_parameter(chn, param);
    else
        param = mixchn_get_command_parameter(chn);
    return param;
}

uint8_t __near checkPara0not(MIXCHN *chn, uint8_t param)
{
    if (param)
        mixchn_set_command_parameter(chn, param);
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

METHOD_GET_NAME (none)
{
    __s[0] = 0;
}

/*** No effect: n/a ***/

METHOD_GET_NAME (none_na)
{
    snprintf (__s, __maxlen, "n/a");
}

/*** Set speed ***/

METHOD_INIT(setSpeed)
{
    param = checkPara0not(chn, param);
    playState_set_speed (&playState, param);
    return true;
}

METHOD_GET_NAME (setSpeed)
{
    snprintf (__s, __maxlen,
        "set speed %hhu",
        mixchn_get_command_parameter (chn)
    );
}

/*** Set tempo ***/

METHOD_INIT(setTempo)
{
    param = checkPara0not(chn, param);
    playState_set_tempo (&playState, param);
    return true;
}

METHOD_GET_NAME (setTempo)
{
    snprintf (__s, __maxlen,
        "set tempo %hhu",
        mixchn_get_command_parameter (chn)
    );
}

/*** Jump to order ***/

METHOD_INIT(jumpToOrder)
{
    param = checkPara0not(chn, param);
    rowState.flags |= ROWSTATEFL_JUMP_TO_ORDER;
    rowState.jump_pos = param;
    return true;
}

METHOD_GET_NAME (jumpToOrder)
{
    snprintf (__s, __maxlen,
        "jump to %03hhu",
        mixchn_get_command_parameter (chn)
    );
}

/*** Pattern break ***/

METHOD_INIT(patBreak)
{
    param = checkPara0not(chn, param);
    rowState.flags |= ROWSTATEFL_PATTERN_BREAK;
    rowState.break_pos = (((param >> 4) * 10) + (param & 0x0f)) & 0x3f;
    return true;
}

METHOD_GET_NAME (patBreak)
{
    snprintf (__s, __maxlen,
        "break to %03hhu",
        mixchn_get_command_parameter (chn)
    );
}

/*** Set global volume ***/

METHOD_INIT(setGVol)
{
    param = checkPara0not(chn, param);
    rowState.flags |= ROWSTATEFL_GLOBAL_VOLUME;
    rowState.global_volume = param > 64 ? 64 : param;
    return true;
}

METHOD_GET_NAME (setGVol)
{
    snprintf (__s, __maxlen,
        "set gvol %02hhX",
        mixchn_get_command_parameter (chn)
    );
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
        }
    }
    mixchn_set_sub_command(chn, cmd);
    return true;
}

METHOD_HANDLE(volSlide)
{
    uint8_t cmd;
    cmd = mixchn_get_sub_command(chn);
    if (cmd <= EFFIDX_VOLSLIDE_MAX)
        SUB_EFFECTS_LIST(volSlide)[cmd]->handle(chn);
}

METHOD_TICK(volSlide)
{
    uint8_t cmd;
    cmd = mixchn_get_sub_command(chn);
    if (cmd <= EFFIDX_VOLSLIDE_MAX)
        SUB_EFFECTS_LIST(volSlide)[cmd]->tick(chn);
}

METHOD_TICK(volSlide_down)
{
    mixchn_set_note_volume (chn, mixchn_get_note_volume (chn) - (mixchn_get_command_parameter (chn) & 0x0f));
}

METHOD_TICK(volSlide_up)
{
    mixchn_set_note_volume (chn, mixchn_get_note_volume (chn) + (mixchn_get_command_parameter (chn) >> 4));
}

METHOD_HANDLE(volSlide_fineDown)
{
    mixchn_set_note_volume (chn, mixchn_get_note_volume (chn) - (mixchn_get_command_parameter (chn) & 0x0f));
}

METHOD_HANDLE(volSlide_fineUp)
{
    mixchn_set_note_volume (chn, mixchn_get_note_volume (chn) + (mixchn_get_command_parameter (chn) >> 4));
}

METHOD_GET_NAME (volSlide)
{
    uint8_t cmd, param;

    cmd = mixchn_get_sub_command (chn);
    param = mixchn_get_command_parameter (chn);

    if (cmd <= EFFIDX_VOLSLIDE_MAX)
        snprintf (__s, __maxlen,
            "%s %02hhX",
            SUB_EFFECTS_NAME_LIST (volSlide)[cmd],
            param
        );
    else
        snprintf (__s, __maxlen,
            "n/a"
        );
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
    mixchn_set_sub_command(chn, cmd);
    return true;
}

METHOD_HANDLE(pitchDown)
{
    uint8_t cmd;
    cmd = mixchn_get_sub_command(chn);
    if (cmd <= EFFIDX_PITCHDOWN_MAX)
        SUB_EFFECTS_LIST(pitchDown)[cmd]->handle(chn);
}

METHOD_TICK(pitchDown)
{
    uint8_t cmd;
    cmd = mixchn_get_sub_command(chn);
    if (cmd <= EFFIDX_PITCHDOWN_MAX)
        SUB_EFFECTS_LIST(pitchDown)[cmd]->tick(chn);
}

METHOD_TICK(pitchDown_normal)
{
    mixchn_setup_sample_period(chn, mixchn_get_sample_period(chn) + (mixchn_get_command_parameter(chn) << 2));
}

METHOD_HANDLE(pitchDown_fine)
{
    mixchn_setup_sample_period(chn, mixchn_get_sample_period(chn) + ((mixchn_get_command_parameter(chn) & 0x0f) << 2));
}

METHOD_HANDLE(pitchDown_extra)
{
    mixchn_setup_sample_period(chn, mixchn_get_sample_period(chn) + (mixchn_get_command_parameter(chn) & 0x0f));
}

METHOD_GET_NAME (pitchDown)
{
    uint8_t cmd, param;

    cmd = mixchn_get_sub_command (chn);
    param = mixchn_get_command_parameter (chn);

    if (cmd <= EFFIDX_PITCHDOWN_MAX)
        snprintf (__s, __maxlen,
            "%s %02hhX",
            SUB_EFFECTS_NAME_LIST (pitchDown)[cmd],
            param
        );
    else
        snprintf (__s, __maxlen,
            "n/a"
        );
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
    mixchn_set_sub_command(chn, cmd);
    return true;
}

METHOD_HANDLE(pitchUp)
{
    uint8_t cmd;
    cmd = mixchn_get_sub_command(chn);
    if (cmd <= EFFIDX_PITCHUP_MAX)
        SUB_EFFECTS_LIST(pitchUp)[cmd]->handle(chn);
}

METHOD_TICK(pitchUp)
{
    uint8_t cmd;
    cmd = mixchn_get_sub_command(chn);
    if (cmd <= EFFIDX_PITCHUP_MAX)
        SUB_EFFECTS_LIST(pitchUp)[cmd]->tick(chn);
}

METHOD_TICK(pitchUp_normal)
{
    mixchn_setup_sample_period(chn, mixchn_get_sample_period(chn) - (mixchn_get_command_parameter(chn) << 2));
}

METHOD_HANDLE(pitchUp_fine)
{
    mixchn_setup_sample_period(chn, mixchn_get_sample_period(chn) - ((mixchn_get_command_parameter(chn) & 0x0f) << 2));
}

METHOD_HANDLE(pitchUp_extra)
{
    mixchn_setup_sample_period(chn, mixchn_get_sample_period(chn) - (mixchn_get_command_parameter(chn) & 0x0f));
}

METHOD_GET_NAME (pitchUp)
{
    uint8_t cmd, param;

    cmd = mixchn_get_sub_command (chn);
    param = mixchn_get_command_parameter (chn);

    if (cmd <= EFFIDX_PITCHUP_MAX)
        snprintf (__s, __maxlen,
            "%s %02hhX",
            SUB_EFFECTS_NAME_LIST (pitchUp)[cmd],
            param
        );
    else
        snprintf (__s, __maxlen,
            "n/a"
        );
}

/*** Portamento to note ***/

void __near eff_porta_start(MIXCHN *chn)
{
    chnState_porta_flag = true;
    if (((chnState_cur_bNote) != CHN_NOTE_OFF)
    &&  ((chnState_cur_bNote) != CHN_NOTE_NONE))
    {
        /* now save some values (we want to slide from) */
        chnState_porta_dSmpStepOld   = mixchn_get_sample_step(chn);
        chnState_porta_wSmpPeriodOld = mixchn_get_sample_period(chn);
    }
}

void __near eff_porta_stop(MIXCHN *chn)
{
    chnState_porta_flag = false;
    mixchn_set_command(chn, EFFIDX_NONE);
}

METHOD_INIT(porta)
{
    param = checkPara0not(chn, param);

    if (param)
        chn->bPortParam = param;

    if (mixchn_is_playing(chn))
        eff_porta_start(chn);
    else
        eff_porta_stop(chn);

    return true;
}

METHOD_HANDLE(porta)
{
    if (((chnState_cur_bNote) != CHN_NOTE_OFF)
    &&  ((chnState_cur_bNote) != CHN_NOTE_NONE))
    {
        chn->wSmpPeriodDest = mixchn_get_sample_period(chn);
        mixchn_set_sample_period(chn, chnState_porta_wSmpPeriodOld);
        mixchn_set_sample_step(chn, chnState_porta_dSmpStepOld);
    }
}

METHOD_TICK(porta)
{
    long period = mixchn_get_sample_period(chn);
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
    }
    mixchn_setup_sample_period(chn, period);
}

METHOD_GET_NAME (porta)
{
    snprintf (__s, __maxlen,
        "porta %02hhX",
        chn->bPortParam
    );
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

METHOD_GET_NAME (porta_vol)
{
    uint8_t cmd, param;

    cmd = mixchn_get_sub_command (chn);
    param = mixchn_get_command_parameter (chn);

    if (cmd <= EFFIDX_VOLSLIDE_MAX)
        snprintf (__s, __maxlen,
            "%s %02hhX",
            SUB_EFFECTS_NAME_LIST (porta_vol)[cmd],
            param
        );
    else
        snprintf (__s, __maxlen,
            "n/a"
        );
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
        chn->wSmpPeriodOld = mixchn_get_sample_period(chn);
}

METHOD_TICK(vibNorm)
{
    unsigned int pos;
    if (mixchn_is_playing(chn))
    {
        /* next position in table: */
        pos = (chn->bTabPos + (chn->bVibParam >> 4)) & 0x3f;
        chn->bTabPos = pos;
        mixchn_setup_sample_period(chn, chn->wSmpPeriodOld +
            ((get_i8_value(chn->wVibTab, pos) * (chn->bVibParam & 0x0f)) >> 4));
    }
}

METHOD_CONT(vibNorm)
{
    return (((chnState_cur_bNote) == CHN_NOTE_OFF)
    ||      ((chnState_cur_bNote) == CHN_NOTE_NONE));
}

METHOD_STOP(vibNorm)
{
    unsigned int period;
    period = chn->wSmpPeriodOld;
    mixchn_set_sample_period(chn, period);
    if (period)
        mixchn_set_sample_step (chn, _calc_sample_step (period, playState.rate));
}

METHOD_GET_NAME (vibNorm)
{
    snprintf (__s, __maxlen,
        "vib. (norm) %02hhX",
        chn->bVibParam
    );
}

/*** Vibrato (fine) ***/

METHOD_TICK(vibFine)
{
    unsigned int pos;
    if (mixchn_is_playing(chn))
    {
        /* next position in table: */
        pos = (chn->bTabPos + (chn->bVibParam >> 4)) & 0x3f;
        chn->bTabPos = pos;
        mixchn_setup_sample_period(chn, chn->wSmpPeriodOld +
            ((get_i8_value(chn->wVibTab, pos) * (chn->bVibParam & 0x0f)) >> 8));
    }
}

METHOD_GET_NAME (vibFine)
{
    snprintf (__s, __maxlen,
        "vib. (fine) %02hhX",
        chn->bVibParam
    );
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

METHOD_GET_NAME (vibNorm_vol)
{
    uint8_t cmd, param;

    cmd = mixchn_get_sub_command (chn);
    param = mixchn_get_command_parameter (chn);

    if (cmd <= EFFIDX_VOLSLIDE_MAX)
        snprintf (__s, __maxlen,
            "%s %02hhX",
            SUB_EFFECTS_NAME_LIST (vibNorm_vol)[cmd],
            param
        );
    else
        snprintf (__s, __maxlen,
            "n/a"
        );
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

METHOD_GET_NAME (tremor)
{
    snprintf (__s, __maxlen,
        "tremor (n/a)"
    );
}

/*** Arpeggio ***/

METHOD_INIT(arpeggio)
{
    if (param)
    {
        chnState_arp_bFlag = true;
        mixchn_set_command_parameter(chn, param);
    }
    else
        chnState_arp_bFlag = false;
    return true;
}

METHOD_HANDLE(arpeggio)
{
    uint8_t param, note;
    PCMSMP *smp;
    uint32_t rate;

    if (!chnState_arp_bFlag)
    {
        if (chn->bEffFlags & EFFFLAG_CONTINUE)
            return;
        /* start arpeggio: */
        chn->bArpPos = 0;
    }

    param = mixchn_get_command_parameter(chn);

    note = _unpack_note (mixchn_get_note (chn)) + (param >> 4);
    chn->bArpNotes[0] = _pack_note (note > NOTE_MAX ? NOTE_MAX : note);

    note = _unpack_note (mixchn_get_note (chn)) + (param & 0x0f);
    chn->bArpNotes[1] = _pack_note (note > NOTE_MAX ? NOTE_MAX : note);

    smp = mixchn_get_sample (chn);
    if (smp && pcmsmp_is_available (smp))
    {
        rate = pcmsmp_get_rate (smp);
        if (rate)
        {
            chn->dArpSmpSteps[0] = chn_calcNoteStep(chn, rate, mixchn_get_note (chn));
            chn->dArpSmpSteps[1] = chn_calcNoteStep(chn, rate, chn->bArpNotes[0]);
            chn->dArpSmpSteps[2] = chn_calcNoteStep(chn, rate, chn->bArpNotes[1]);
        }
    }
}

METHOD_TICK(arpeggio)
{
    unsigned int pos = chn->bArpPos + 1;
    if (pos >= 3)
        pos = 0;
    chn->bArpPos = pos;
    mixchn_set_sample_step(chn, chn->dArpSmpSteps[pos]);
}

METHOD_STOP(arpeggio)
{
    mixchn_set_sample_step(chn, chn->dArpSmpSteps[0]);
}

METHOD_GET_NAME (arpeggio)
{
    snprintf (__s, __maxlen,
        "arpeggio %02hhX",
        mixchn_get_command_parameter (chn)
    );
}

/*** Set sample offset ***/

METHOD_HANDLE(sampleOffset)
{
    uint8_t param;
    param = mixchn_get_command_parameter(chn);
    chn->wSmpStart = param << 8;
    if (((chnState_cur_bNote) != CHN_NOTE_OFF)
    &&  ((chnState_cur_bNote) != CHN_NOTE_NONE))
        chn->dSmpPos = (unsigned long)chn->wSmpStart << 16;
}

METHOD_GET_NAME (sampleOffset)
{
    snprintf (__s, __maxlen,
        "sample ofs. %02hhX00",
        mixchn_get_command_parameter (chn)
    );
}

/*** Note retrigger + Volume slide ***/

METHOD_INIT(retrig)
{
    uint8_t ticks;
    if (!param)
        param = mixchn_get_command_parameter(chn);
    else
    {
        mixchn_set_command_parameter(chn, param);
        ticks = param & 0x0f;
        if (ticks)
            chn->bRetrigTicks = ticks - 1;
        else
        {
            mixchn_set_command(chn, EFFIDX_NONE);
            return true;
        }
    }
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
    }
    chn->dSmpPos = 0;
    ticks = mixchn_get_command_parameter(chn) & 0x0f;
    if (ticks)
    {
        chn->bRetrigTicks = ticks;
        cmd = mixchn_get_sub_command(chn);
        if (cmd <= 6)
            SUB_EFFECTS_LIST(retrig)[cmd]->tick(chn);
    }
}

METHOD_INIT(retrig_none)
{
    mixchn_set_sub_command(chn, EFFIDX_RETRIG_VOLSLIDE_NONE);
    return true;
}

METHOD_INIT(retrig_slideDown)
{
    mixchn_set_sub_command(chn, EFFIDX_RETRIG_VOLSLIDE_DOWN);
    return true;
}

METHOD_INIT(retrig_use2div3)
{
    mixchn_set_sub_command(chn, EFFIDX_RETRIG_VOLSLIDE_USE2DIV3);
    return true;
}

METHOD_INIT(retrig_use1div2)
{
    mixchn_set_sub_command(chn, EFFIDX_RETRIG_VOLSLIDE_USE1DIV2);
    return true;
}

METHOD_INIT(retrig_slideUp)
{
    mixchn_set_sub_command(chn, EFFIDX_RETRIG_VOLSLIDE_UP);
    return true;
}

METHOD_INIT(retrig_use3div2)
{
    mixchn_set_sub_command(chn, EFFIDX_RETRIG_VOLSLIDE_USE3DIV2);
    return true;
}

METHOD_INIT(retrig_use2div1)
{
    mixchn_set_sub_command(chn, EFFIDX_RETRIG_VOLSLIDE_USE2DIV1);
    return true;
}

METHOD_TICK(retrig_slideDown)
{
    mixchn_set_note_volume (chn, mixchn_get_note_volume (chn) - (int)(1 << (mixchn_get_command_parameter (chn) >> 4)));
}

METHOD_TICK(retrig_use2div3)
{
    mixchn_set_note_volume (chn, (int) (mixchn_get_note_volume (chn) * 2) / 3);
}

METHOD_TICK(retrig_use1div2)
{
    mixchn_set_note_volume (chn, mixchn_get_note_volume (chn) >> 1);
}

METHOD_TICK(retrig_slideUp)
{
    mixchn_set_note_volume (chn, mixchn_get_note_volume (chn) + (int) (1 << (mixchn_get_command_parameter (chn) >> 4)));
}

METHOD_TICK(retrig_use3div2)
{
    mixchn_set_note_volume (chn, (int) (mixchn_get_note_volume (chn) * 3) >> 1);
}

METHOD_TICK(retrig_use2div1)
{
    mixchn_set_note_volume (chn, (int) mixchn_get_note_volume (chn) << 1);
}

METHOD_GET_NAME (retrig)
{
    uint8_t cmd, param;

    cmd = mixchn_get_sub_command (chn);
    param = mixchn_get_command_parameter (chn);

    if (cmd <= EFFIDX_RETRIG_VOLSLIDE_MAX)
        snprintf (__s, __maxlen,
            "%s %02hhX",
            SUB_EFFECTS_NAME_LIST (retrig)[cmd],
            param
        );
    else
        snprintf (__s, __maxlen,
            "n/a"
        );
}

/*** Tremolo ***/

METHOD_INIT(tremolo)
{
    if (!(chn->bEffFlags & EFFFLAG_CONTINUE))
        chn->bTabPos = 0;
    if (param)
    {
        if (!(param & 0xf0))
            param |= mixchn_get_command_parameter(chn) & 0xf0;
        mixchn_set_command_parameter(chn, param);
    }
    return true;
}

METHOD_HANDLE(tremolo)
{
    if ((chnState_cur_bIns != CHN_INS_NONE)
    ||  (chnState_cur_bVol != CHN_NOTEVOL_NONE)
    ||  (!(chn->bEffFlags & EFFFLAG_CONTINUE)))
        chn->bSmpVolOld = mixchn_get_note_volume (chn);
}

METHOD_TICK(tremolo)
{
    unsigned int pos;
    uint8_t param = mixchn_get_command_parameter(chn);
    /* next position in table: */
    pos = (chn->bTabPos + (param >> 4)) & 0x3f;
    chn->bTabPos = pos;
    mixchn_set_note_volume (chn, chn->bSmpVolOld +
        ((get_i8_value(chn->wTrmTab, pos) * (param & 0x0f)) >> 6));
}

METHOD_GET_NAME (tremolo)
{
    snprintf (__s, __maxlen, "n/a");
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
        playState.patloop_start_row = playState.row;
    else
    {
        if (!(playState.flags & PLAYSTATEFL_PATLOOP))
        {
            playState.flags |= PLAYSTATEFL_PATLOOP;
            param++;
            playState.patloop_count = param;
        }
        rowState.flags |= ROWSTATEFL_PATTERN_LOOP;
    }

    return true;
}

METHOD_TICK(special_noteCut)
{
    if (! --chn->bDelayTicks)
        mixchn_set_playing(chn, false);
}

METHOD_INIT(special_noteDelay)
{
    chn->bDelayTicks = param;
    if (!(rowState.flags & ROWSTATEFL_PATTERN_DELAY))
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
        if (note != CHN_NOTE_NONE)
        {
            if (note == CHN_NOTE_OFF)
                mixchn_set_playing(chn, false);
            else
            {
                chn_setupNote(chn, note, 0);
                mixchn_set_playing(chn, true);
            }
        }
        if (chn->bSavVol != CHN_NOTEVOL_NONE)
            mixchn_set_note_volume (chn, chn->bSavVol);
        mixchn_set_command(chn, EFFIDX_NONE);
    }
}

METHOD_INIT(special_patDelay)
{
    if (!(rowState.flags & ROWSTATEFL_PATTERN_DELAY))
    {
        playState.patdelay_count = param + 1;
        chnState_patDelay_bParameterSaved = mixchn_get_command_parameter(chn);
    }
    return true;
}

METHOD_HANDLE(special_patDelay)
{
    mixchn_set_command(chn, chnState_patDelay_bCommandSaved);
    mixchn_set_sub_command(chn, 0);
    mixchn_set_command_parameter(chn, chnState_patDelay_bParameterSaved);
}

METHOD_INIT(special)
{
    uint8_t cmd;
    param = checkPara0(chn, param);
    cmd = eff_special_route[param >> 4];
    mixchn_set_sub_command(chn, cmd);
    return SUB_EFFECTS_LIST(special)[cmd]->init(chn, param & 0x0f);
}

METHOD_TICK(special)
{
    uint8_t cmd;
    cmd = mixchn_get_sub_command(chn);
    if (cmd <= EFFIDX_SPECIAL_MAX)
        SUB_EFFECTS_LIST(special)[cmd]->tick(chn);
}

METHOD_GET_NAME (special)
{
    uint8_t cmd, param;
    const char *name;

    cmd = mixchn_get_sub_command (chn);
    param = mixchn_get_command_parameter (chn);

    if ((cmd != EFFIDX_SPECIAL_NONE) && (cmd <= EFFIDX_SPECIAL_MAX))
    {
        name = SUB_EFFECTS_NAME_LIST (special)[cmd];
        if (name)
        {
            snprintf (__s, __maxlen,
                "%s %hhX",
                name,
                param & 15
            );
            return;
        }
    }
    snprintf (__s, __maxlen,
        "n/a"
    );
}

/*** General effects handling ***/

bool chn_effInit(MIXCHN *chn, uint8_t param)
{
    uint8_t cmd;
    cmd = mixchn_get_command(chn);
    if (cmd <= MAXEFF)
        return EFFECTS_LIST(main)[cmd]->init(chn, param);
    else
        return false;
}

void chn_effHandle(MIXCHN *chn)
{
    uint8_t cmd;
    cmd = mixchn_get_command(chn);
    if (cmd <= MAXEFF)
        EFFECTS_LIST(main)[cmd]->handle(chn);
}

void chn_effTick(MIXCHN *chn)
{
    uint8_t cmd;
    cmd = mixchn_get_command(chn);
    if (cmd <= MAXEFF)
        EFFECTS_LIST(main)[cmd]->tick(chn);
}

bool chn_effCanContinue(MIXCHN *chn)
{
    uint8_t cmd;
    cmd = mixchn_get_command(chn);
    if (cmd <= MAXEFF)
        return EFFECTS_LIST(main)[cmd]->cont(chn);
    else
        return false;
}

void chn_effStop(MIXCHN *chn)
{
    uint8_t cmd;
    cmd = mixchn_get_command(chn);
    if (cmd <= MAXEFF)
        EFFECTS_LIST(main)[cmd]->stop(chn);
}

void chn_effGetName (MIXCHN *chn, char *__s, size_t __maxlen)
{
    #define _BUF_SIZE 40
    char s[_BUF_SIZE];
    uint8_t cmd, param;

    s[0] = 0;

    cmd = mixchn_get_command (chn);
    param = mixchn_get_command_parameter (chn);

    if ((cmd != CHN_CMD_NONE) && (cmd <= MAXEFF))
    {
        snprintf (s, 5, "%c%02hhX: ",  'A' + cmd - 1, param);
        EFFECTS_LIST (main)[cmd]->get_name (chn, s + 5, _BUF_SIZE - 5);
    }

    strncpy (__s, s, __maxlen);
    #undef _BUF_SIZE
}
