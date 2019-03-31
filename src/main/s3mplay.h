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

void     __far player_clear_error (void);
bool     __far player_is_error (void);
const char *__far player_get_error (void);
bool     __far player_init (void);
bool     __far player_init_device (SNDDEVTYPE type, SNDDEVSETMET method);
void     __far player_device_dump_conf (void);
char    *__far player_device_get_name (void);
bool     __far player_set_mode (bool f_16bits, bool f_stereo, uint16_t rate, bool LQ);
uint16_t __far player_get_output_rate (void);
uint8_t  __far player_get_output_channels (void);
uint8_t  __far player_get_output_bits (void);
bool     __far player_get_output_lq (void);
void     __far player_set_master_volume (uint8_t value);
uint8_t  __far player_get_master_volume (void);
MIXER   *__far player_get_mixer (void);
void     __far player_set_order (bool skipend);
void     __far player_set_order_start (uint8_t value);
int      __far player_find_next_pattern (MUSMOD *track, PLAYSTATE *ps, int index, int step);
void     __far player_set_song_loop (bool value);
bool     __far player_load_s3m (char *name, MUSMOD **_track);
MIXCHNLIST *__far player_get_mixing_channels (void);
void     __far player_set_pos (MUSMOD *track, PLAYSTATE *ps, uint8_t start_order, uint8_t start_row, bool keep);
void     __far player_song_stop (MUSMOD *track, PLAYSTATE *ps);
bool     __far player_play_start (void);
void     __far player_play_pause (void);
void     __far player_play_continue (void);
void     __far player_play_stop (void);
uint16_t __far player_get_buffer_pos (void);
uint8_t  __far player_get_speed (void);
uint8_t  __far player_get_tempo (void);
uint8_t  __far player_get_pattern_delay (void);
void     __far player_free_module (MUSMOD *track);
void     __far player_free_modules (void);
void     __far player_free (void);

/*** Initialization ***/

DECLARE_REGISTRATION (s3mplay)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux PLAYER_VERSION "*";

#pragma aux player_clear_error "*";
#pragma aux player_is_error "*";
#pragma aux player_get_error "*";
#pragma aux player_init "*";
#pragma aux player_init_device "*";
#pragma aux player_device_dump_conf "*";
#pragma aux player_device_get_name "*";
#pragma aux player_set_mode "*";
#pragma aux player_get_output_rate "*";
#pragma aux player_get_output_channels "*";
#pragma aux player_get_output_bits "*";
#pragma aux player_get_output_lq "*";
#pragma aux player_set_master_volume "*";
#pragma aux player_get_master_volume "*";
#pragma aux player_get_mixer "*";
#pragma aux player_set_order "*";
#pragma aux player_set_order_start "*";
#pragma aux player_find_next_pattern "*";
#pragma aux player_set_song_loop "*";
#pragma aux player_load_s3m "*";
#pragma aux player_get_mixing_channels "*";
#pragma aux player_set_pos "*";
#pragma aux player_song_stop "*";
#pragma aux player_play_start "*";
#pragma aux player_play_pause "*";
#pragma aux player_play_continue "*";
#pragma aux player_play_stop "*";
#pragma aux player_get_buffer_pos "*";
#pragma aux player_get_speed "*";
#pragma aux player_get_tempo "*";
#pragma aux player_get_pattern_delay "*";
#pragma aux player_free_module "*";
#pragma aux player_free_modules "*";
#pragma aux player_free "*";

#pragma aux register_s3mplay "*";
#pragma aux unregister_s3mplay "*";

#endif  /* __WATCOMC__ */

#endif  /* S3MPLAY_H */
