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

extern char *PUBLIC_DATA PLAYER_VERSION;

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
bool     PUBLIC_CODE player_play_start(void);
void     PUBLIC_CODE player_play_pause(void);
void     PUBLIC_CODE player_play_continue(void);
void     PUBLIC_CODE player_play_stop(void);
uint16_t PUBLIC_CODE player_get_DMA_counter(void);
uint8_t  PUBLIC_CODE player_get_speed(void);
uint8_t  PUBLIC_CODE player_get_tempo(void);
uint8_t  PUBLIC_CODE player_get_pattern_delay(void);
void     PUBLIC_CODE player_free_module(void);
void     PUBLIC_CODE player_free(void);

/*** Initialization ***/

DECLARE_REGISTRATION(s3mplay)

#endif  /* S3MPLAY_H */
