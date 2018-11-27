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

extern int16_t PUBLIC_DATA pascal_doserror;

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
        errno = EZERO;
        break;
    case 2:
        errno = ENOENT;
        break;
    case 3:
        errno = ENOENT;
        break;
    case 5:
        errno = EACCES;
        break;
    case 6:
        errno = EFAULT;
        break;
    case 8:
        errno = ENOMEM;
        break;
    case 10:
        errno = EINVAL;
        break;
    case 11:
        errno = EINVAL;
        break;
    case 18:
        errno = EMFILE;
        break;
    default:
        errno = EZERO;
        break;
    }
    return 0;
}
