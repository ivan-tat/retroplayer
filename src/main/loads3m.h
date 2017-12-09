/* loads3m.h -- declarations for loads3m.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef LOADS3M_H
#define LOADS3M_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "main/s3mtypes.h"

typedef void S3MLOADER;

S3MLOADER *s3mloader_new(void);
void     s3mloader_init(S3MLOADER *self);
bool     s3mloader_load(S3MLOADER *self, const char *name);
const char *s3mloader_get_error(S3MLOADER *self);
void     s3mloader_free(S3MLOADER *self);
void     s3mloader_delete(S3MLOADER **self);

#ifdef __WATCOMC__
#pragma aux s3mloader_new "*";
#pragma aux s3mloader_init "*";
#pragma aux s3mloader_load "*";
#pragma aux s3mloader_get_error "*";
#pragma aux s3mloader_free "*";
#pragma aux s3mloader_delete "*";
#endif

#endif  /* LOADS3M_H */
