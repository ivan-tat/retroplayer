/* stdio.h -- declarations for custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_STDIO_H
#define CC_STDIO_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "pascal/pascal.h"

/* Types */

typedef PASCALFILE FILE;

typedef int32_t fpos_t;

/* Methods */

FILE  *cc_fopen(const char *path, const char *mode);
void   cc_fclose(FILE *stream);
int    cc_fsetpos(FILE *stream, fpos_t pos);
size_t cc_fread(void *ptr, size_t size, size_t n, FILE *stream);
size_t cc_fwrite(void *ptr, size_t size, size_t n, FILE *stream);

int cc_printf(const char *format, ...);
int cc_fprintf(FILE *stream, const char *format, ...);
int cc_sprintf(char *str, const char *format, ...);
int cc_snprintf(char *str, size_t size, const char *format, ...);
int cc_vprintf(const char *format, va_list ap);
int cc_vfprintf(FILE *stream, const char *format, va_list ap);
int cc_vsprintf(char *str, const char *format, va_list ap);
int cc_vsnprintf(char *str, size_t size, const char *format, va_list ap);

/* Aliases */

#define fopen   cc_fopen
#define fclose  cc_fclose
#define fsetpos cc_fsetpos
#define fread   cc_fread
#define fwrite  cc_fwrite

#define printf    cc_printf
#define fprintf   cc_fprintf
#define sprintf   cc_sprintf
#define snprintf  cc_snprintf
#define vprintf   cc_vprintf
#define vfprintf  cc_vfprintf
#define vsprintf  cc_vsprintf
#define vsnprintf cc_vsnprintf

#endif  /* CC_STDIO_H */