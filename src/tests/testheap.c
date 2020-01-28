#include "defines.h"

#include <stdint.h>
#include "cc/string.h"
#include "cc/i86.h"
#include "cc/io.h"
#include "cc/stdio.h"
#include "cc/stdlib.h"
#include "cc/unistd.h"
#include "sysdbg.h"
#include "debug.h"
#include "startup.h"
#include "tests/testheap.h"

#define CHECK_IO 1

#if CHECK_IO == 1
# define IOCHK _cc_CheckInOutRes ();
#else   /* CHECK_IO != 1 */
# define IOCHK
#endif  /* CHECK_IO != 1 */

void __noreturn _start_c (void)
{
    system_init ();
    if (!heap_init (0, 0xffff))
    {
        cc_TextWriteString (&cc_Output, "Failed to setup heap.", 0); IOCHK
        cc_TextWriteLn (&cc_Output); IOCHK
        _cc_Exit (1);
    }
    _cc_Exit (main ());
}

int main (void)
{
    cc_TextWriteString (&cc_Output, "MemAvail = ", 0); IOCHK
    cc_TextWriteInteger (&cc_Output, memavail (), 0); IOCHK
    cc_TextWriteLn (&cc_Output); IOCHK
    cc_TextWriteString (&cc_Output, "MaxAvail = ", 0); IOCHK
    cc_TextWriteInteger (&cc_Output, maxavail (), 0); IOCHK
    cc_TextWriteLn (&cc_Output); IOCHK
    cc_TextSync (&cc_Output); IOCHK
    return 0;
}
