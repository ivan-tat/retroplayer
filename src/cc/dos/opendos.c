/* opendos.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/dos/error086.h"
#include "cc/share.h"

/*
unsigned _cc_dos_open(const char *fname, unsigned mode, int *fd)
*/
/*
 * Input:
 *      "mode" is a arithmetic OR of:
 *      - file mode flags as defined in "cc/dos.h" file (CC_O_*_DOS constants) and
 *      - access/open mode flags as defined in "cc/share.h" (CC_SH_* constants)
 */
uint16_t PUBLIC_CODE _cc_dos_open(const char *fname, uint16_t mode, int16_t *fd)
{
    union CC_REGPACK regs;
    uint16_t error;

    regs.h.ah = 0x3d;
    regs.h.al = mode;
    regs.w.dx = FP_OFF(fname);
    regs.w.ds = FP_SEG(fname);
    cc_intr(0x21, &regs);
    error = __cc_doserror(&regs);
    if (!error)
        *fd = regs.w.ax;
    return error;
}
