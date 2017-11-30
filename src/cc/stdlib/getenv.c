/* getenv.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdbool.h>

#include "pascal.h"
#include "common.h"
#include "cc/string.h"

#include "cc/stdlib.h"

extern void __near __pascal pascal_getenv(char *dest, const char *name);

#ifdef __WATCOMC__
#pragma aux pascal_getenv modify [ax bx cx dx si di es];
#endif

char *custom_getenv(char *dest, const char *name, size_t maxlen)
{
    char _n[pascal_String_size];
    char _s[pascal_String_size];
    size_t len;

    strncpy(&_n[1], name, pascal_String_size - 1);
    len = strlen(name);
    if (len > pascal_String_size - 1)
        len = pascal_String_size - 1;
    _n[0] = len;
    pascal_getenv(_s, _n);
    strpastoc(dest, _s, maxlen);
    len = _s[0];
    if (len > maxlen - 1)
        len = maxlen - 1;
    if (len > 0)
        memcpy(dest, &_s[1], len);
    dest[len] = 0;

    return dest;
}
