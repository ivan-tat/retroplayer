/* effects.c -- effects handling.

   This is free and unencumbered software released into the public domain. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "general.h"

#define effTickProc( name ) void __near name( struct channel_t *chn )

typedef void __near effTickProc_t( struct channel_t *chn );

effTickProc( effTick_none );
effTickProc( effTick_D_VolumeSlide );
effTickProc( effTick_D_VolumeSlide_Down );
effTickProc( effTick_D_VolumeSlide_Up );
effTickProc( effTick_E_PitchDown );
effTickProc( effTick_E_PitchDown_Down );
effTickProc( effTick_F_PitchUp );
effTickProc( effTick_F_PitchUp_Up );
effTickProc( effTick_G_Portamento );
effTickProc( effTick_H_Vibrato );
effTickProc( effTick_I_Tremor );
effTickProc( effTick_J_Arpeggio );
effTickProc( effTick_K_VibratoVolSlide );
effTickProc( effTick_L_PortamentoVolSlide );
effTickProc( effTick_Q_Retrigger );
effTickProc( effTick_Q_Retrigger_SlideDown );
effTickProc( effTick_Q_Retrigger_Use2div3 );
effTickProc( effTick_Q_Retrigger_Use1div2 );
effTickProc( effTick_Q_Retrigger_SlideUp );
effTickProc( effTick_Q_Retrigger_Use3div2 );
effTickProc( effTick_Q_Retrigger_Use2div1 );
effTickProc( effTick_R_Tremolo );
effTickProc( effTick_S_Special );
effTickProc( effTick_S_Special_NoteCut );
effTickProc( effTick_S_Special_NoteDelay );
effTickProc( effTick_U_FineVibrato );

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

const static effTickProc_t *effTick_D_VolumeSlide_tab[] = {
    effTick_D_VolumeSlide_Down,
    effTick_D_VolumeSlide_Up,
    effTick_none,
    effTick_none
};

const static effTickProc_t *effTick_E_PitchDown_tab[] = {
    effTick_E_PitchDown_Down,
    effTick_none,
    effTick_none
};

const static effTickProc_t *effTick_F_PitchUp_tab[] = {
    effTick_F_PitchUp_Up,
    effTick_none,
    effTick_none
};

const static effTickProc_t *effTick_Q_Retrigger_tab[] = {
    effTick_none,
    effTick_Q_Retrigger_SlideDown,
    effTick_Q_Retrigger_Use2div3,
    effTick_Q_Retrigger_Use1div2,
    effTick_Q_Retrigger_SlideUp,
    effTick_Q_Retrigger_Use3div2,
    effTick_Q_Retrigger_Use2div1
};

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
    effTick_S_Special_NoteCut,
    effTick_S_Special_NoteDelay,
    effTick_none,   // Pattern delay
    effTick_none    // funkrepeat
};

void __near voltest( struct channel_t *chn, uint8_t vol )
{
    chn->bSampleVol = ( vol <= 63 ? vol : 63 );
}

effTickProc( effTick_none )
{
    return;
}

effTickProc( effTick_D_VolumeSlide )
{
    effTick_D_VolumeSlide_tab[ chn->cmd2nd ]( chn );
}

effTickProc( effTick_D_VolumeSlide_Down )
{
    // TODO
    return;
}

effTickProc( effTick_D_VolumeSlide_Up )
{
    // TODO
    return;
}

effTickProc( effTick_E_PitchDown )
{
    effTick_E_PitchDown_tab[ chn->cmd2nd ]( chn );
}

effTickProc( effTick_E_PitchDown_Down )
{
    // TODO
    return;
}

effTickProc( effTick_F_PitchUp )
{
    effTick_F_PitchUp_tab[ chn->cmd2nd ]( chn );
}

effTickProc( effTick_F_PitchUp_Up )
{
    // TODO
    return;
}

effTickProc( effTick_G_Portamento )
{
    // TODO
    return;
}

effTickProc( effTick_H_Vibrato )
{
    // TODO
    return;
}

effTickProc( effTick_I_Tremor )
{
    // TODO
    return;
}

effTickProc( effTick_J_Arpeggio )
{
    // TODO
    return;
}

effTickProc( effTick_K_VibratoVolSlide )
{
    effTick_D_VolumeSlide( chn );
    effTick_H_Vibrato( chn );
}

effTickProc( effTick_L_PortamentoVolSlide )
{
    effTick_D_VolumeSlide( chn );
    effTick_G_Portamento( chn );
}

effTickProc( effTick_Q_Retrigger )
{
    // TODO
    return;
}


effTickProc( effTick_Q_Retrigger_SlideDown )
{
    // TODO
    return;
}

effTickProc( effTick_Q_Retrigger_Use2div3 )
{
    // TODO
    return;
}

effTickProc( effTick_Q_Retrigger_Use1div2 )
{
    // TODO
    return;
}

effTickProc( effTick_Q_Retrigger_SlideUp )
{
    // TODO
    return;
}

effTickProc( effTick_Q_Retrigger_Use3div2 )
{
    // TODO
    return;
}

effTickProc( effTick_Q_Retrigger_Use2div1 )
{
    // TODO
    return;
}

effTickProc( effTick_R_Tremolo )
{
    // TODO
    return;
}

effTickProc( effTick_S_Special )
{
    effTick_S_Special_tab[ chn->cmd2nd ]( chn );
}

effTickProc( effTick_S_Special_NoteCut )
{
    // TODO
    return;
}

effTickProc( effTick_S_Special_NoteDelay )
{
    // TODO
    return;
}

effTickProc( effTick_U_FineVibrato )
{
    // TODO
    return;
}