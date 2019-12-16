#include "defines.h"

#include <stdint.h>
#include "cc/string.h"
#include "cc/io.h"
#include "cc/i86.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "sysdbg.h"
#include "debug.h"
#include "startup.h"
#include "test.h"

void __noreturn _start_c (void)
{
    /**
    System_Initialize ();
    Crt_Initialize ();
    **/
    system_init ();
    console_init ();
    delay_init ();
    _cc_Exit (main (_argc, _argv));
}

int main (int argc, const char **argv)
{
    char s[256];
    int i;
    int32_t l;

    SYSDEBUG_BEGIN ();
    SYSDEBUG_INFO_ ("Arguments count = %i", argc);
    for (i = 0; i < argc; i++)
    {
        SYSDEBUG_INFO_ ("Argument [%i] = %s", i, argv[i]);
    }

    /*
    cc_TextReadString (&cc_Input, &s, 255);
    SYSDEBUG_INFO_ ("strlen=%i", strlen (&s));
    cc_TextEOL (&cc_Input);
    cc_TextWriteString (&cc_Output, &s, 0);
    cc_TextWriteLn (&cc_Output);
    cc_TextSync (&cc_Output);
    /**/

    /*
    var
        s: String;
        l: LongInt;

    begin
        WriteLn ('This is a simple console application.');
        WriteLn (Output, 'Yes, it`s true.');
        Write (Output, 'Enter text: ');
        ReadLn (Input, s);
        WriteLn (Output, 'Text="', s, '"');
        Write (Output, 'Enter long integer number (-2^32...2^32-1): ');
        ReadLn (Input, l);
        WriteLn (Output, 'Value="', l, '"');
    end.
    */
    cc_TextWriteString (&cc_Output, "This is a simple console application.", 0);
    cc_TextWriteLn (&cc_Output);
    cc_TextWriteString (&cc_Output, "Yes, it`s true.", 0);
    cc_TextWriteLn (&cc_Output);
    cc_TextWriteString (&cc_Output, "Enter text: ", 0);
    cc_TextSync (&cc_Output);
    cc_TextReadString (&cc_Input, &s, 255);
    cc_TextEOL (&cc_Input);
    cc_TextWriteString (&cc_Output, "Text=\"", 0);
    cc_TextWriteString (&cc_Output, &s, 0);
    cc_TextWriteChar (&cc_Output, '"', 0);
    cc_TextWriteLn (&cc_Output);
    cc_TextWriteString (&cc_Output, "Enter long integer number (-2^32...2^32-1): ", 0);
    cc_TextSync (&cc_Output);
    l = cc_TextReadInteger (&cc_Input);
    cc_TextEOL (&cc_Input);
    cc_TextWriteString (&cc_Output, "Value=\"", 0);
    cc_TextWriteInteger (&cc_Output, l, 0);
    cc_TextWriteChar (&cc_Output, '"', 0);
    cc_TextWriteLn (&cc_Output);
    /**/
    SYSDEBUG_INFO_ ("value=%l", (int32_t) l);
    SYSDEBUG_END ();
    return 0;
}
