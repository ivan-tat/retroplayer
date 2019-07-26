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
#include "main/musmod.h"

typedef void LOADER_S3M;

LOADER_S3M *load_s3m_new (void);
void        load_s3m_init (LOADER_S3M *self);
MUSMOD     *load_s3m_load (LOADER_S3M *self, const char *name, bool use_EM);
const char *load_s3m_get_error (LOADER_S3M *self);
void        load_s3m_free (LOADER_S3M *self);
void        load_s3m_delete (LOADER_S3M **self);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux load_s3m_new "*";
#pragma aux load_s3m_init "*";
#pragma aux load_s3m_load "*";
#pragma aux load_s3m_get_error "*";
#pragma aux load_s3m_free "*";
#pragma aux load_s3m_delete "*";

#endif  /* __WATCOMC__ */

#endif  /* LOADS3M_H */
