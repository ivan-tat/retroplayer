(* stdio.pas -- Pascal declarations for custom "stdio" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit stdio;

interface

(*$I defines.pas*)

procedure cc_fclose;
procedure cc_fopen;
procedure cc_fread;
procedure cc_fsetpos;
procedure cc_fwrite;

procedure cc_fprintf;
procedure cc_printf;
procedure cc_snprintf;
procedure cc_sprintf;
procedure cc_vfprintf;
procedure cc_vprintf;
procedure cc_vsnprintf;
procedure cc_vsprintf;

implementation

uses
    watcom,
    pascal,
    startup,
    debugfn,
    errno_,
    ctype,
    dos_,
    string_,
    io;

procedure pascal_write(s: pchar); far;
begin
    Write(s);
end;

(*$L stdio/fclose.obj*)
procedure cc_fclose; external;
(*$L stdio/fopen.obj*)
procedure cc_fopen; external;
(*$L stdio/fread.obj*)
procedure cc_fread; external;
(*$L stdio/fsetpos.obj*)
procedure cc_fsetpos; external;
(*$L stdio/fwrite.obj*)
procedure cc_fwrite; external;

(*$L stdio/_printf.obj*)
procedure dataStreamFlush; external;
procedure dataStreamWrite; external;
procedure dataStreamInitMemory; external;
procedure dataStreamInitStdOut; external;
procedure dataStreamInitFile; external;
procedure _dsprintf; external;

(*$L stdio/printf.obj*)
procedure cc_printf; external;
(*$L stdio/fprintf.obj*)
procedure cc_fprintf; external;
(*$L stdio/sprintf.obj*)
procedure cc_sprintf; external;
(*$L stdio/snprintf.obj*)
procedure cc_snprintf; external;
(*$L stdio/vprintf.obj*)
procedure cc_vprintf; external;
(*$L stdio/vfprintf.obj*)
procedure cc_vfprintf; external;
(*$L stdio/vsprintf.obj*)
procedure cc_vsprintf; external;
(*$L stdio/vsnprntf.obj*)
procedure cc_vsnprintf; external;

end.
