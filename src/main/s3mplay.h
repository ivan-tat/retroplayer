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

extern char *PLAYER_VERSION;

void     PUBLIC_CODE player_clear_error(void);
bool     PUBLIC_CODE player_is_error(void);
const char *PUBLIC_CODE player_get_error(void);
bool     PUBLIC_CODE player_init(void);
bool     PUBLIC_CODE player_init_device(uint8_t type);
void     PUBLIC_CODE player_device_dump_conf(void);
char    *PUBLIC_CODE player_device_get_name(void);
bool     PUBLIC_CODE player_set_mode(bool f_16bits, bool f_stereo, uint16_t rate, bool LQ);
uint16_t PUBLIC_CODE player_get_output_rate(void);
uint8_t  PUBLIC_CODE player_get_output_channels(void);
uint8_t  PUBLIC_CODE player_get_output_bits(void);
bool     PUBLIC_CODE player_get_output_lq(void);
void     PUBLIC_CODE player_set_master_volume(uint8_t value);
uint8_t  PUBLIC_CODE player_get_master_volume(void);
void     PUBLIC_CODE player_set_order(bool extended);
bool     PUBLIC_CODE player_load_s3m(char *name);
void     player_set_pos(uint8_t start_order, uint8_t start_row, bool keep);
bool     PUBLIC_CODE player_play_start(void);
void     PUBLIC_CODE player_play_pause(void);
void     PUBLIC_CODE player_play_continue(void);
void     PUBLIC_CODE player_play_stop(void);
uint16_t PUBLIC_CODE player_get_buffer_pos(void);
uint8_t  PUBLIC_CODE player_get_speed(void);
uint8_t  PUBLIC_CODE player_get_tempo(void);
uint8_t  PUBLIC_CODE player_get_pattern_delay(void);
void     PUBLIC_CODE player_free_module(void);
void     PUBLIC_CODE player_free(void);

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
#pragma aux player_set_order "*";
#pragma aux player_load_s3m "*";
#pragma aux player_set_pos "*";
#pragma aux player_play_start "*";
#pragma aux player_play_pause "*";
#pragma aux player_play_continue "*";
#pragma aux player_play_stop "*";
#pragma aux player_get_buffer_pos "*";
#pragma aux player_get_speed "*";
#pragma aux player_get_tempo "*";
#pragma aux player_get_pattern_delay "*";
#pragma aux player_free_module "*";
#pragma aux player_free "*";

#pragma aux register_s3mplay "*";
#pragma aux unregister_s3mplay "*";

#endif  /* __WATCOMC__ */

#endif  /* S3MPLAY_H */
