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

/* play options */

bool playOption_ST3Order;
bool playOption_LoopSong;

/* song arrangement */

uint8_t  LastOrder;

/* initial state */

uint16_t initState_startOrder;

/* play state */

PLAYSTATE playState;

#endif  /* DEFINE_LOCAL_DATA */

void __far playState_set_speed (PLAYSTATE *self, uint8_t value)
{
    if (value > 0)
        self->speed = value;
}

void __far playState_set_tempo (PLAYSTATE *self, uint8_t value)
{
    if (value >= 32)
        self->tempo = value;
    else
        value = self->tempo;

    if (value)
        self->tick_samples_per_channel = (long)self->rate * 5 / (int)(value * 2);
}
