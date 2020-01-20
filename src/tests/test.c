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
#include "test.h"

#define CHECK_IO 1

#if CHECK_IO == 1
# define IOCHK _cc_CheckInOutRes ();
#else   /* CHECK_IO != 1 */
# define IOCHK
#endif  /* CHECK_IO != 1 */

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
    **/
    system_init ();
    _cc_Exit (main (_cc_argc, _cc_argv));
}

int main (int argc, const char **argv)
{
    char s[256];
    int i;
    int32_t l;
    FILE f;
    char f_buf[128];

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
    (*$I+*) (*I/O error checking*)
    var
        s: String;
        l: LongInt;
        f: Text;
        i: Integer;
    begin
        WriteLn ('This is a simple console application.');
        WriteLn (Output, 'Yes, it`s true.');
        Write (Output, 'Enter text: ');
        ReadLn (Input, s);
        WriteLn (Output, 'Text="', s, '"');
        Write (Output, 'Enter long integer number (-2^32...2^32-1): ');
        ReadLn (Input, l);
        WriteLn (Output, 'Value="', l, '"');
        Assign (f, 'test.txt');
        Rewrite (f);
        WriteLn (f, 'Test for an ordinary text file.');
        for i := 1 to 100 do
            WriteLn (f, 'Line ', i:3, '.');
        Close (f);
    end.
    */
    cc_TextWriteString (&cc_Output, "This is a simple console application.", 0); IOCHK
    cc_TextWriteLn (&cc_Output); IOCHK
    cc_TextWriteString (&cc_Output, "Yes, it`s true.", 0); IOCHK
    cc_TextWriteLn (&cc_Output); IOCHK
    cc_TextWriteString (&cc_Output, "Enter text: ", 0); IOCHK
    cc_TextSync (&cc_Output); IOCHK
    cc_TextReadString (&cc_Input, &s, 255); IOCHK
    cc_TextEOL (&cc_Input); IOCHK
    cc_TextWriteString (&cc_Output, "Text=\"", 0); IOCHK
    cc_TextWriteString (&cc_Output, &s, 0); IOCHK
    cc_TextWriteChar (&cc_Output, '"', 0); IOCHK
    cc_TextWriteLn (&cc_Output); IOCHK
    cc_TextWriteString (&cc_Output, "Enter long integer number (-2^32...2^32-1): ", 0); IOCHK
    cc_TextSync (&cc_Output); IOCHK
    l = cc_TextReadInteger (&cc_Input); IOCHK
    cc_TextEOL (&cc_Input); IOCHK
    cc_TextWriteString (&cc_Output, "Value=\"", 0); IOCHK
    cc_TextWriteInteger (&cc_Output, l, 0); IOCHK
    cc_TextWriteChar (&cc_Output, '"', 0); IOCHK
    cc_TextWriteLn (&cc_Output); IOCHK
    cc_TextSync (&cc_Output); IOCHK

    SYSDEBUG_INFO_ ("value=%ld", (int32_t) l);
    SYSDEBUG_END ();

    cc_TextAssign (&f, f_buf, 128, "test.txt"); IOCHK
    cc_TextRewrite (&f); IOCHK
    cc_TextWriteString (&f, "Test for an ordinary text file.", 0); IOCHK
    cc_TextWriteLn (&f); IOCHK
    for (i = 1; i <= 100; i++)
    {
        cc_TextWriteString (&f, "Line ", 0); IOCHK
        cc_TextWriteInteger (&f, i, 3); IOCHK
        cc_TextWriteChar (&f, '.', 0); IOCHK
        cc_TextWriteLn (&f); IOCHK
    }
    cc_TextClose (&f);

    return 0;
}
