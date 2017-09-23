/* s3mplay.c -- main library for playing music modules.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/stdio.h"
#include "main/effects.h"

#include "main/s3mplay.h"

/*** Initialization ***/

extern void __near __pascal s3mplayInit(void);
extern void __near __pascal s3mplayDone(void);

DEFINE_REGISTRATION(s3mplay, s3mplayInit, s3mplayDone)
