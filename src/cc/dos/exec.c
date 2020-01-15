/* exec.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$exec$*"
#endif

#include <stdbool.h>
#include <stdint.h>
#include "cc/errno.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dos.h"
#include "cc/dos/error086.h"
#include "cc/dos/ex.h"

unsigned _cc_dos_exec (uint16_t env_seg, const char *fname, const char *cmd)
{
    unsigned len;
    char _fname[79+1], _cmd[126+2], *p;
    struct cc_dosfcb_t fcb[2];
    struct cc_dos_execparam_t param;

    len = strlen (fname);
    if (len > 79) len = 79;
    if (len) memcpy (_fname, fname, len);
    _fname [len] = 0;   /* NULL terminated */

    len = strlen (cmd);
    if (len > 126) len = 126;
    _cmd [0] = len;
    if (len) memcpy (& (_cmd [1]), cmd, len);
    _cmd [len + 1] = 0x0d;  /* command line must end with CR */

    param.env_seg = env_seg;
    param.cmd_tail = _cmd;
    p = _cc_dos_parsfnm (& (_cmd [1]), & (fcb [0]), CC_FCB_RMPATH);
    _cc_dos_parsfnm (p, & (fcb [1]), CC_FCB_RMPATH);
    param.fcb[0] = & (fcb [0]);
    param.fcb[1] = & (fcb [1]);
    _cc_doserrno = _cc_dos_exec_asm (_fname, &param);
    if (_cc_doserrno)
        return __cc_doserror2 ();
    else
        return 0;
}
