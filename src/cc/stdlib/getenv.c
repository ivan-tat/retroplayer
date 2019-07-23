/* getenv.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$getenv$*"
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "common.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dos.h"
#include "cc/errno.h"
#include "cc/stdlib.h"
#include "cc/stdlib/_env.h"

char *cc_getenv(const char *name)
{
    uint16_t nlen;
    struct dosenvlist_t *dosenv;
    char *value;

    if (_env_name_check(name, &nlen))
        return NULL;

    dosenv = &_dos_env;
    value = _dosenv_find(dosenv, name, nlen);
    if (value)
        value = &(value[nlen + 1]);
    return value;
}
