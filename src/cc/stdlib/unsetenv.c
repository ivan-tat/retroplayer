/* unsetenv.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$unsetenv$*"
#endif  /* __WATCOMC__ */

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

#define _ptrl(p) ((uint32_t)(FP_SEG(p) * 16) + FP_OFF(p))

uint32_t _ptrdiff(void *p1, void *p2)
{
    return _ptrl(p2) - _ptrl(p1);
}

int16_t cc_unsetenv(const char *name)
{
    uint16_t nlen;
    struct dosenvlist_t *oenv, nenv;
    uint16_t envsize;
    uint16_t seg;
    char *s, *endptr;
    uint16_t ssize, diff;

    if (_env_name_check(name, &nlen))
        return -1;

    oenv = &_dos_env;
    envsize = oenv->size;

    if (_dos_allocmem(_dos_para(envsize), &seg))
    {
        cc_errno = CC_ENOMEM;
        return -1;
    }
    nenv.arr = MK_FP(seg, 0);
    nenv.size = envsize;

    memcpy(nenv.arr, oenv->arr, envsize);
    s = _dosenv_find(&nenv, name, nlen);
    if (s)
    {
        ssize = strlen(s) + 1;
        diff = _ptrdiff(nenv.arr, s);
        endptr = &(s[ssize]);
        memcpy(s, endptr, envsize - (diff + ssize));
        memcpy(oenv->arr, nenv.arr, envsize);
    }

    _dos_freemem(FP_SEG(nenv.arr));

    if (cc_environ_rebuild())
        return 0;
    else
    {
        cc_errno = CC_ENOMEM;
        return -1;
    }
}
