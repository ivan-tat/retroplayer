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

#ifdef DEFINE_LOCAL_DATA

/* EMS */

bool UseEMS;
EMSHDL SavHandle;

/* general module information */

bool             mod_isLoaded;
modTitle_t       mod_Title;
modTrackerName_t mod_TrackerName;

/* module options */

bool modOption_ST2Vibrato;
bool modOption_ST2Tempo;
bool modOption_AmigaSlides;
bool modOption_SBfilter;
bool modOption_CostumeFlag;
bool modOption_VolZeroOptim;
bool modOption_AmigaLimits;
bool modOption_Stereo;

/* play options */

bool playOption_ST3Order;
bool playOption_LoopSong;

/* song arrangement */

ordersList_t Order;
uint16_t OrdNum;
uint8_t  LastOrder;

channelsList_t mod_Channels;
uint8_t mod_ChannelsCount;

/* initial state */

uint8_t  initState_tempo;
uint8_t  initState_speed;
uint16_t initState_startOrder;

/* play state */

bool     playState_songEnded;
uint16_t playState_rate;
uint8_t  playState_tempo;
uint8_t  playState_speed;
uint8_t  playState_gVolume;
uint8_t  playState_mVolume;
uint16_t playState_tick_samples_per_channel;    /* depends on rate and tempo */

/* position in song - you can change it while playing to jump arround */

uint8_t  playState_order;
uint8_t  playState_pattern;
uint8_t  playState_row;
uint8_t  playState_tick;
uint16_t playState_tick_samples_per_channel_left;   /* samples per channel left to next tick */

/* pattern loop */

bool    playState_patLoopActive;
uint8_t playState_patLoopCount;
uint8_t playState_patLoopStartRow;

/* pattern delay */

uint8_t playState_patDelayCount;

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
