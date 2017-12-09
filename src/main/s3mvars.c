/* s3mvars.c -- variables for s3m play.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/dos.h"
#include "dos/ems.h"
#include "main/s3mtypes.h"

#include "main/s3mvars.h"

// TODO: remove EXTERN_LINK, PUBLIC_DATA and PUBLIC_CODE macros when done.

#ifdef DEFINE_LOCAL_DATA

/* EMS */

bool PUBLIC_DATA UseEMS;
EMSHDL PUBLIC_DATA SavHandle;

/* general module information */

bool             PUBLIC_DATA mod_isLoaded;
modTitle_t       PUBLIC_DATA mod_Title;
modTrackerName_t PUBLIC_DATA mod_TrackerName;

/* module options */

bool PUBLIC_DATA modOption_ST2Vibrato;
bool PUBLIC_DATA modOption_ST2Tempo;
bool PUBLIC_DATA modOption_AmigaSlides;
bool PUBLIC_DATA modOption_SBfilter;
bool PUBLIC_DATA modOption_CostumeFlag;
bool PUBLIC_DATA modOption_VolZeroOptim;
bool PUBLIC_DATA modOption_AmigaLimits;
bool PUBLIC_DATA modOption_Stereo;

/* play options */

bool PUBLIC_DATA playOption_ST3Order;
bool PUBLIC_DATA playOption_LoopSong;

/* song arrangement */

ordersList_t PUBLIC_DATA Order;
uint16_t PUBLIC_DATA OrdNum;
uint8_t  PUBLIC_DATA LastOrder;

channelsList_t PUBLIC_DATA Channel;
uint8_t PUBLIC_DATA UsedChannels;

/* initial state */

uint8_t  PUBLIC_DATA initState_tempo;
uint8_t  PUBLIC_DATA initState_speed;
uint16_t PUBLIC_DATA initState_startOrder;

/* play state */

bool     PUBLIC_DATA playState_songEnded;
uint16_t PUBLIC_DATA playState_rate;
uint8_t  PUBLIC_DATA playState_tempo;
uint8_t  PUBLIC_DATA playState_speed;
uint8_t  PUBLIC_DATA playState_gVolume;
uint8_t  PUBLIC_DATA playState_mVolume;
uint16_t PUBLIC_DATA playState_tick_samples_per_channel;    /* depends on rate and tempo */

/* position in song - you can change it while playing to jump arround */

uint8_t  PUBLIC_DATA playState_order;
uint8_t  PUBLIC_DATA playState_pattern;
uint8_t  PUBLIC_DATA playState_row;
uint8_t  PUBLIC_DATA playState_tick;
uint16_t PUBLIC_DATA playState_tick_samples_per_channel_left;   /* samples per channel left to next tick */

/* pattern loop */

bool    PUBLIC_DATA playState_patLoopActive;
uint8_t PUBLIC_DATA playState_patLoopCount;
uint8_t PUBLIC_DATA playState_patLoopStartRow;

/* pattern delay */

uint8_t PUBLIC_DATA playState_patDelayCount;

#endif  /* DEFINE_LOCAL_DATA */

void playState_set_speed(uint8_t value)
{
    if (value > 0)
        playState_speed = value;
}

void playState_set_tempo(uint8_t value)
{
    if (value >= 32)
        playState_tempo = value;
    else
        value = playState_tempo;

    if (value)
        playState_tick_samples_per_channel = (long)playState_rate * 5 / (int)(value * 2);
}
