/* exv.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/errno.h"
#include "cc/string.h"

#include "cc/unistd.h"

extern int16_t pascal_doserror;
#ifdef __WATCOMC__
#pragma aux pascal_doserror "*";
#endif  /* __WATCOMC__ */

int cc_execv(const char *filename, char *const argv[])
{
    char pathstr[cc_PathStr_size];
    char cmdline[pascal_String_size];
    int n;
    size_t len, part;
    char *arg;
    bool space;

    strctopas(pathstr, filename, cc_PathStr_size);
    len = 0;
    n = 0;
    while ((arg = argv[n]) && (len < pascal_String_size - 1))
    {
        part = strlen(arg);
        if (part)
        {
            if (len)
            {
                space = true;
                part++;
            }
            else
                space = false;

            if (part > pascal_String_size - 1 - len)
                part = pascal_String_size - 1 - len;

            if (space)
            {
                cmdline[1 + len] = ' ';
                len++;
                part--;
            }

            if (part)
            {
                memcpy(cmdline + 1 + len, arg, part);
                len += part;
            }
        }

        n++;
    }
    cmdline[0] = len;

    pascal_swapvectors();
    pascal_exec(pathstr, cmdline);
    pascal_swapvectors();

    switch (pascal_doserror)
    {
    case 0:
        cc_errno = CC_EZERO;
        break;
    case 2:
        cc_errno = CC_ENOENT;
        break;
    case 3:
        cc_errno = CC_ENOENT;
        break;
    case 5:
        cc_errno = CC_EACCES;
        break;
    case 6:
        cc_errno = CC_EFAULT;
        break;
    case 8:
        cc_errno = CC_ENOMEM;
        break;
    case 10:
        cc_errno = CC_EINVAL;
        break;
    case 11:
        cc_errno = CC_EINVAL;
        break;
    case 18:
        cc_errno = CC_EMFILE;
        break;
    default:
        cc_errno = CC_EZERO;
        break;
    }
    return 0;
}
