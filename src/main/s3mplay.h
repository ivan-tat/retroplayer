/* s3mplay.h -- declarations for s3mplay.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef S3MPLAY_H
#define S3MPLAY_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "debug.h"
#include "main/mixchn.h"
#include "main/mixer.h"
#include "main/fillvars.h"

/*** Sound ***/

typedef uint8_t sound_device_type_t;
typedef sound_device_type_t SNDDEVTYPE;

#define SNDDEVTYPE_NONE 0
#define SNDDEVTYPE_SB   1

typedef uint8_t sound_device_setup_method_t;
typedef sound_device_setup_method_t SNDDEVSETMET;

#define SNDDEVSETMET_MANUAL 0
#define SNDDEVSETMET_DETECT 1
#define SNDDEVSETMET_ENV    2
#define SNDDEVSETMET_INPUT  3

/*** Player ***/

extern char *PLAYER_VERSION;

typedef void MUSPLAYER;

// General initialization

MUSPLAYER  *__far player_new (void);
bool        __far player_init (MUSPLAYER *self);
bool     __far player_is_error (MUSPLAYER *self);
const char *__far player_get_error (MUSPLAYER *self);
void     __far player_set_EM_usage (MUSPLAYER *self, bool value);
bool     __far player_is_EM_in_use (MUSPLAYER *self);

// Output device

bool     __far player_init_device (MUSPLAYER *self, SNDDEVTYPE type, SNDDEVSETMET method);
char    *__far player_device_get_name (MUSPLAYER *self);
void     __far player_device_dump_conf (MUSPLAYER *self);
SNDDMABUF *__far player_get_sound_buffer (MUSPLAYER *self);
void     __far player_set_sound_buffer_fps (MUSPLAYER *self, uint8_t value);
bool     __far player_set_mode (MUSPLAYER *self, bool f_16bits, bool f_stereo, uint16_t rate, bool LQ);
uint16_t __far player_get_output_rate (MUSPLAYER *self);
uint8_t  __far player_get_output_channels (MUSPLAYER *self);
uint8_t  __far player_get_output_bits (MUSPLAYER *self);
bool     __far player_get_output_lq (MUSPLAYER *self);
bool     __far player_play_start (MUSPLAYER *self);
void     __far player_play_pause (MUSPLAYER *self);
void     __far player_play_continue (MUSPLAYER *self);
void     __far player_play_stop (MUSPLAYER *self);
uint16_t __far player_get_buffer_pos (MUSPLAYER *self);
void     __far player_free_device (MUSPLAYER *self);

// Mixer

bool     __far player_init_mixer (MUSPLAYER *self);
MIXER   *__far player_get_mixer (MUSPLAYER *self);
void     __far player_free_mixer (MUSPLAYER *self);

// Song

bool     __far player_load_s3m (MUSPLAYER *self, char *name, MUSMOD **_track);
bool     __far player_set_active_track (MUSPLAYER *self, MUSMOD *track);
PLAYSTATE *__far player_get_play_state (MUSPLAYER *self);
uint8_t  __far player_get_master_volume (MUSPLAYER *self);
void     __far player_set_master_volume (MUSPLAYER *self, uint8_t value);
void     __far player_free_module (MUSPLAYER *self, MUSMOD *track);
void     __far player_free_modules (MUSPLAYER *self);

// General finalization

void     __far player_free (MUSPLAYER *self);
void     __far player_delete (MUSPLAYER **self);

/*** Initialization ***/

DECLARE_REGISTRATION (s3mplay)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux PLAYER_VERSION "*";

#pragma aux player_new "*";
#pragma aux player_init "*";
#pragma aux player_is_error "*";
#pragma aux player_get_error "*";
#pragma aux player_set_EM_usage "*";
#pragma aux player_is_EM_in_use "*";

#pragma aux player_init_device "*";
#pragma aux player_device_get_name "*";
#pragma aux player_device_dump_conf "*";
#pragma aux player_get_sound_buffer "*";
#pragma aux player_set_sound_buffer_fps "*";
#pragma aux player_set_mode "*";
#pragma aux player_get_output_rate "*";
#pragma aux player_get_output_channels "*";
#pragma aux player_get_output_bits "*";
#pragma aux player_get_output_lq "*";
#pragma aux player_play_start "*";
#pragma aux player_play_pause "*";
#pragma aux player_play_continue "*";
#pragma aux player_play_stop "*";
#pragma aux player_get_buffer_pos "*";
#pragma aux player_free_device "*";

#pragma aux player_init_mixer "*";
#pragma aux player_get_mixer "*";
#pragma aux player_free_mixer "*";

#pragma aux player_load_s3m "*";
#pragma aux player_set_active_track "*";
#pragma aux player_get_play_state "*";
#pragma aux player_set_master_volume "*";
#pragma aux player_get_master_volume "*";
#pragma aux player_free_module "*";
#pragma aux player_free_modules "*";

#pragma aux player_free "*";
#pragma aux player_delete "*";

#pragma aux register_s3mplay "*";
#pragma aux unregister_s3mplay "*";

#endif  /* __WATCOMC__ */

#endif  /* S3MPLAY_H */
