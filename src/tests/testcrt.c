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
#include "tests/testcrt.h"

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
        TextColor (_cyan);
        WriteLn ('This is a simple console application.');
        TextBackGround (_blue);
        WriteLn (Output, 'Yes, it`s true.');
        TextBackGround (_black);
        TextColor (_white);
        Write (Output, 'Enter text: ');
        TextColor (_red);
        ReadLn (Input, s);
        TextColor (_lightgray);
        WriteLn (Output, 'Text="', s, '"');
        TextColor (_lightgreen);
        Write (Output, 'Enter long integer number (-2^32...2^32-1): ');
        ReadLn (Input, l);
        TextColor (_yellow);
        WriteLn (Output, 'Value="', l, '"');
    end.
    */
    cc_textcolor (_cyan);
    cc_TextWriteString (&cc_Output, "This is a simple console application.", 0);
    cc_TextWriteLn (&cc_Output);
    cc_textbackground (_blue);
    cc_TextWriteString (&cc_Output, "Yes, it`s true.", 0);
    cc_TextWriteLn (&cc_Output);
    cc_textbackground (_black);
    cc_textcolor (_white);
    cc_TextWriteString (&cc_Output, "Enter text: ", 0);
    cc_TextSync (&cc_Output);
    cc_textcolor (_red);
    cc_TextReadString (&cc_Input, &s, 255);
    cc_textcolor (_lightgray);
    cc_TextEOL (&cc_Input);
    cc_TextWriteString (&cc_Output, "Text=\"", 0);
    cc_TextWriteString (&cc_Output, &s, 0);
    cc_TextWriteChar (&cc_Output, '"', 0);
    cc_TextWriteLn (&cc_Output);
    cc_textcolor (_lightgreen);
    cc_TextWriteString (&cc_Output, "Enter long integer number (-2^32...2^32-1): ", 0);
    cc_TextSync (&cc_Output);
    l = cc_TextReadInteger (&cc_Input);
    cc_TextEOL (&cc_Input);
    cc_textcolor (_yellow);
    cc_TextWriteString (&cc_Output, "Value=\"", 0);
    cc_TextWriteInteger (&cc_Output, l, 0);
    cc_TextWriteChar (&cc_Output, '"', 0);
    cc_TextWriteLn (&cc_Output);
    /**/
    SYSDEBUG_INFO_ ("value=%ld", (int32_t) l);
    SYSDEBUG_END ();
    return 0;
}
