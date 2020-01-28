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
#include "tests/testexec.h"

#define CHECK_IO 1

#if CHECK_IO == 1
# define IOCHK _cc_CheckInOutRes ();
#else   /* CHECK_IO != 1 */
# define IOCHK
#endif  /* CHECK_IO != 1 */

void __noreturn _start_c (void)
{
    system_init ();

    if (!environ_init ())
    {
        cc_TextWriteString (&cc_Output, "Failed to setup DOS environment variables.", 0); IOCHK
        cc_TextWriteLn (&cc_Output); IOCHK
        _cc_Exit (1);
    }
    _cc_Exit (main (_cc_argc, _cc_argv));
}

int main (int argc, const char **argv)
{
    char **s, *comspec;
    int i;

    SYSDEBUG_BEGIN ();
    SYSDEBUG_INFO_ ("Arguments count = %i", argc);
    for (i = 0; i < argc; i++)
    {
        SYSDEBUG_INFO_ ("Argument [%i] = %s", i, argv[i]);
    }
    #if SYSDEBUG == 1
    s = cc_environ;
    i = 0;
    while (*s)
    {
        SYSDEBUG_INFO_ ("EnvStr[%i] = %s", i, *s);
        s++;
        i++;
    }
    #endif  /* SYSDEBUG == 1 */

    cc_TextWriteString (&cc_Output, "Starting DOS shell... (to return use 'exit' command)", 0); IOCHK
    cc_TextWriteLn (&cc_Output); IOCHK
    cc_TextSync (&cc_Output); IOCHK

    comspec = cc_getenv ("COMSPEC");
    if (comspec)
    {
        if (cc_execv (comspec, NULL))
        {
            cc_TextWriteString (&cc_Output, "Error ", 0); IOCHK
            cc_TextWriteInteger (&cc_Output, cc_errno, 0); IOCHK
            cc_TextWriteString (&cc_Output, " (DOS error ", 0); IOCHK
            cc_TextWriteInteger (&cc_Output, _cc_doserrno, 0); IOCHK
            cc_TextWriteChar (&cc_Output, ')', 0); IOCHK
            cc_TextWriteLn (&cc_Output); IOCHK
            cc_TextSync (&cc_Output); IOCHK
        }
        else
        {
            cc_TextWriteString (&cc_Output, "Success.", 0); IOCHK
            cc_TextWriteLn (&cc_Output); IOCHK
            cc_TextSync (&cc_Output); IOCHK
        }
    }
    else
    {
        cc_TextWriteString (&cc_Output, "Environment variable COMSPEC is not found." CRLF, 0); IOCHK
        cc_TextSync (&cc_Output); IOCHK
    }
    SYSDEBUG_END ();
    return 0;
}
