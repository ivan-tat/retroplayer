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

/*** Initialization ***/

DECLARE_REGISTRATION(s3mplay)

#endif  /* S3MPLAY_H */
