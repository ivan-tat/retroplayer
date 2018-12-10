/* setenv.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

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

int16_t cc_setenv(const char *name, const char *value, int16_t overwrite)
{
    uint16_t nlen, vlen;
    struct dosenvlist_t *oenv, nenv;
    uint16_t seg;
    uint16_t envsize, count, endptr, ssize, left;

    if (_env_name_check(name, &nlen))
        return -1;

    oenv = &_dos_env;

    if (overwrite)
    {
        if (cc_unsetenv(name))
            return -1;
    }
    else
    {
        if (_dosenv_find(oenv, name, nlen))
            return 0;
    }

    envsize = oenv->size;
    if (_dos_allocmem(_dos_para(envsize), &seg))
    {
        cc_errno = CC_ENOMEM;
        return -1;
    }
    nenv.arr = MK_FP(seg, 0);
    nenv.size = envsize;

    memcpy(nenv.arr, oenv->arr, envsize);
    endptr = _dosenv_find_end(&nenv, &count);
    /* "%s=%s\0" */
    vlen = strlen(value);
    ssize = nlen + 1 + vlen + 1;
    /* the last byte is a terminating zero for the whole strings list */
    left = envsize - endptr - 1;
    if (ssize > left)
    {
        ssize = left;
        /* "%s=\0" */
        if (ssize < nlen + 1 + 1)
        {
            _dos_freemem(FP_SEG(nenv.arr));
            cc_errno = CC_ENOMEM;
            return -1;
        }
    }

    /* name */
    memcpy(&(nenv.arr[endptr]), name, nlen);
    endptr += nlen;
    ssize -= nlen;
    /* = */
    nenv.arr[endptr] = '=';
    endptr++;
    ssize--;
    /* value or it's part */
    if (ssize > 1)
    {
        memcpy(&(nenv.arr[endptr]), value, ssize - 1);
        endptr += ssize - 1;
        ssize -= ssize - 1;
    }
    /* 0 */
    nenv.arr[endptr] = 0;
    /*
    endptr++;
    ssize--;
    */
    memcpy(oenv->arr, nenv.arr, envsize);
    _dos_freemem(FP_SEG(nenv.arr));
    if (cc_environ_rebuild())
        return 0;
    else
    {
        cc_errno = CC_ENOMEM;
        return -1;
    }
}
