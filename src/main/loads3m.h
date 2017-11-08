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

S3MLOADER *PUBLIC_CODE s3mloader_new(void);
void     PUBLIC_CODE s3mloader_init(S3MLOADER *self);
bool     PUBLIC_CODE s3mloader_load(S3MLOADER *self, const char *name);
const char *PUBLIC_CODE s3mloader_get_error(S3MLOADER *self);
void     PUBLIC_CODE s3mloader_free(S3MLOADER *self);
void     PUBLIC_CODE s3mloader_delete(S3MLOADER **self);
    
#endif  /* LOADS3M_H */
