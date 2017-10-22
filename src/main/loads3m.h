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
#include "main/muspat.h"

#define S3M_MAX_PATTERNS 100

typedef struct S3M_loader_t
{
    int16_t err;
    FILE f;
    char *buffer;
    uint16_t inspara[MAX_INSTRUMENTS];
    uint16_t patpara[S3M_MAX_PATTERNS];
    uint32_t smppara[MAX_INSTRUMENTS];
    uint16_t pat_EM_pages;
    uint16_t pat_EM_page;
    uint16_t pat_EM_page_offset;
    uint16_t smp_EM_pages;
    uint16_t smp_EM_page;
};
typedef struct S3M_loader_t S3MLOADER;

S3MLOADER *PUBLIC_CODE s3mloader_new(void);
void       PUBLIC_CODE s3mloader_clear(S3MLOADER *self);
void       PUBLIC_CODE s3mloader_delete(S3MLOADER **self);
bool       PUBLIC_CODE s3mloader_allocbuf(S3MLOADER *self);
bool       PUBLIC_CODE s3mloader_load_pattern(S3MLOADER *self, uint8_t index, uint32_t pos);
void       PUBLIC_CODE unpackPattern(uint8_t *src, uint8_t *dst, uint8_t maxrow, uint8_t maxchn);
void       PUBLIC_CODE s3mloader_free(S3MLOADER *self);
    
#endif  /* LOADS3M_H */
