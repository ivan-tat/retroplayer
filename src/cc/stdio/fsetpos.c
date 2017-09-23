/* fsetpos.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/string.h"

#include "cc/stdio.h"
#include "cc/stdio/_io.h"

int cc_fsetpos(FILE *stream, fpos_t pos)
{
    if (stream)
    {
        return pascal_seek(stream, pos) ? 0 : -1;
    }
    else
        return -1;
}
