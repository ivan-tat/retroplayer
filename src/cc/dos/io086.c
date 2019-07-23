/* io086.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$io086$*"
#endif

#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/dos/error086.h"

/*
 * Requirements:
 *      DOS 2.0+
 * Description:
 *      If AX = CX and CF = NC the read was successful.
 *      If AX = 0 then EOF was reached.
 *      If 0 < AX < CX then:
 *          if reading from device, the input line is AX bytes long.
 *          if reading from file, the call has read up to the EOF.
 */
/*
unsigned _cc_dos_read(int fd, void __far *buf, unsigned count, unsigned *numbytes)
*/
uint16_t __far _cc_dos_read (int16_t fd, void __far *buf, uint16_t count, uint16_t *numbytes)
{
    union CC_REGPACK regs;
    uint16_t error;

    regs.h.ah = 0x3f;
    regs.w.bx = fd;
    regs.w.cx = count;
    regs.w.dx = FP_OFF(buf);
    regs.w.ds = FP_SEG(buf);
    cc_intr(0x21, &regs);
    error = __cc_doserror(&regs);
    if (!error)
        *numbytes = regs.w.ax;
    return error;
}

/*
 * Requirements:
 *      DOS 2.0+
 * Description:
 *      If AX = CX the read was successful.
 *      If AX < CX then an error occured (most likely a full disk).
 *          Caution: CF is not set in this case.
 *      DOS 3.0+:
 *          If CX = 0 on entry, the file is truncated at the current position,
 *          or the file is padded to the current position.
 */
/*
unsigned _cc_dos_write(int fd, void __far *buf, unsigned count, unsigned *numbytes)
*/
uint16_t __far _cc_dos_write (int16_t fd, void __far *buf, uint16_t count, uint16_t *numbytes)
{
    union CC_REGPACK regs;
    uint16_t error;

    regs.h.ah = 0x40;
    regs.w.bx = fd;
    regs.w.cx = count;
    regs.w.dx = FP_OFF(buf);
    regs.w.ds = FP_SEG(buf);
    cc_intr(0x21, &regs);
    error = __cc_doserror(&regs);
    if (!error)
        *numbytes = regs.w.ax;
    return error;
}
