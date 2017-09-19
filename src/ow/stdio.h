/* stdio.h -- declarations for stdio.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CUSTOM_STDIO_H
#define CUSTOM_STDIO_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

#include "pascal/pascal.h"

typedef PASCALFILE FILE;

void printf(char *format, ...);

FILE *fopen(char *path, char *mode);
void  fclose(FILE *file);

#endif  /* CUSTOM_STDIO_H */
