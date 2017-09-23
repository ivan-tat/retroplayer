/* fclose.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "pascal/pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/string.h"

#include "cc/stdio.h"
#include "cc/stdio/_io.h"

void cc_fclose(FILE *stream)
{
    if (stream)
    {
        pascal_close(stream);
        _dos_freemem(FP_SEG(stream));
    };
}
