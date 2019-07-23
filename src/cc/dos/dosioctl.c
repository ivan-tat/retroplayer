/* dosioctl.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$dosioctl$*"
#endif

#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/dos/error086.h"

/*
 * Requirements:
 *      DOS 2.0+
 *      File must not be read-only.
 * Description:
 *      Checks to see if a handle represents a device of a disk file and
 *      obtains information about that device or file.
 *      Returns EZERO on success or, otherwise sets "errno" variable.
 */
/*
unsigned _cc_dos_ioctl_query_flags (int16_t fd, cc_ioctl_info_t *info)
*/
uint16_t __far _cc_dos_ioctl_query_flags (int16_t fd, cc_ioctl_info_t __far *info)
{
    union CC_REGPACK regs;
    uint16_t error;

    regs.w.ax = 0x4400;
    regs.w.bx = fd;
    cc_intr (0x21, &regs);
    error = __cc_doserror (&regs);
    if (!error)
        *info = regs.w.dx;
    return error;
}
