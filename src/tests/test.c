#include "defines.h"

#include <stdint.h>
#include "cc/string.h"
#include "cc/io.h"
#include "cc/stdio.h"
#include "sysdbg.h"
#include "debug.h"
#include "startup.h"
#include "test.h"

/*
void __near dosprint (const char *s);
#pragma aux dosprint "*" parm [ ax dx ];
void __near dosprint (const char *s)
{
    _asm {
    "push ds"
    "mov ds,dx"
    "mov dx,ax"
    "mov ah,9"
    "int 0x21"
    "pop ds"
    }
}
*/

void __noreturn _start_c (void)
{
    /**
    System_Initialize ();
    Crt_Initialize ();
    **/
    _cc_startup ();
    _cc_Exit (main (_cc_argc, _cc_argv));
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
    FileReadString (&cc_Input, &s, 255);
    SYSDEBUG_INFO_ ("strlen=%i", strlen (&s));
    FileSkipToNextLine (&cc_Input);
    FileWriteString (&cc_Output, &s, 0);
    FileWriteNewLine (&cc_Output);
    FileFlushBuffer (&cc_Output);
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
    FileWriteString (&cc_Output, "This is a simple console application.", 0);
    FileWriteNewLine (&cc_Output);
    FileWriteString (&cc_Output, "Yes, it`s true.", 0);
    FileWriteNewLine (&cc_Output);
    FileWriteString (&cc_Output, "Enter text: ", 0);
    FileFlushBuffer (&cc_Output);
    FileReadString (&cc_Input, &s, 255);
    FileSkipToNextLine (&cc_Input);
    FileWriteString (&cc_Output, "Text=\"", 0);
    FileWriteString (&cc_Output, &s, 0);
    FileWriteChar (&cc_Output, '"', 0);
    FileWriteNewLine (&cc_Output);
    FileWriteString (&cc_Output, "Enter long integer number (-2^32...2^32-1): ", 0);
    FileFlushBuffer (&cc_Output);
    l = FileReadNumber (&cc_Input);
    FileSkipToNextLine (&cc_Input);
    FileWriteString (&cc_Output, "Value=\"", 0);
    FileWriteNumber (&cc_Output, l, 0);
    FileWriteChar (&cc_Output, '"', 0);
    FileWriteNewLine (&cc_Output);
    /**/
    SYSDEBUG_INFO_ ("value=%l", (int32_t) l);
    SYSDEBUG_END ();
    return 0;
}
