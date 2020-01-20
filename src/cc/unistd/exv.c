/* exv.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$unistd$exv$*"
#endif  /* __WATCOMC__ */

#include "pascal.h"
#include "startup.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/errno.h"
#include "cc/string.h"
#include "sysdbg.h"
#include "cc/unistd.h"

/*
 * Description:
 *      This is not really an "execv()" but more like a "system()" method.
 *      It stops execution of current program (but background tasks are still
 *      working if any) and runs a specified one with arguments. After it
 *      finished the main program is continued.
 */
int cc_execv (const char *filename, char *const argv[])
{
    char cmdline[126+1];
    int n;
    size_t len, part;
    char *arg;
    bool space;
    int status;

    len = 0;
    if (argv)
    {
        n = 0;
        while ((arg = argv[n]) && (len < 126))
        {
            part = strlen (arg);
            if (part)
            {
                if (len)
                {
                    space = true;
                    part++;
                }
                else
                    space = false;

                if (part > 126 - len)
                    part = 126 - len;

                if (space)
                {
                    cmdline [len] = ' ';
                    len++;
                    part--;
                }

                if (part)
                {
                    memcpy (cmdline + len, arg, part);
                    len += part;
                }
            }
            n++;
        }
    }
    cmdline [len] = 0;

    /* Pascal SwapVectors() must be the last call before executing. */
    cc_dos_swapvectors ();
#if LINKER_TPC == 1
    pascal_swapvectors ();
#endif  /* LINKER_TPC == 1 */
    status = _cc_dos_exec (
        ((struct cc_dospsp_t *) MK_FP (_cc_psp, 0))->env_seg, filename, cmdline);
#if LINKER_TPC == 1
    pascal_swapvectors ();
#endif  /* LINKER_TPC == 1 */
    cc_dos_swapvectors ();
    return status != 0 ? -1 : 0;
}
