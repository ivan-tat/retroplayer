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
#include "main/effects.h"

#define noerror 0
#define notenoughmem -1
#define allreadyallocbuffers -6
#define nota386orhigher -7
#define nosounddevice -8
#define noS3Minmemory -9
#define internal_failure -11
#define E_failed_to_load_file -100

extern int16_t PUBLIC_DATA player_error;
extern const char *PUBLIC_DATA player_error_msg;

bool     PUBLIC_CODE player_init(void);
bool     PUBLIC_CODE player_init_device(uint8_t type);
bool     PUBLIC_CODE player_set_mode(bool f_16bits, bool f_stereo, uint16_t rate, bool LQ);
uint16_t PUBLIC_CODE playGetSampleRate(void);
bool     PUBLIC_CODE player_is_lq_mode(void);
void     PUBLIC_CODE playSetMasterVolume(uint8_t value);
uint8_t  PUBLIC_CODE playGetMasterVolume(void);
void     PUBLIC_CODE playSetOrder(bool extended);
bool     PUBLIC_CODE player_load_s3m(char *name);
bool     PUBLIC_CODE playStart(void);
uint8_t  PUBLIC_CODE playGetSpeed(void);
uint8_t  PUBLIC_CODE playGetTempo(void);
uint8_t  PUBLIC_CODE playGetPatternDelay(void);
void     PUBLIC_CODE player_free_module(void);
void     PUBLIC_CODE player_free(void);

/*** Initialization ***/

DECLARE_REGISTRATION(s3mplay)

#endif  /* S3MPLAY_H */
