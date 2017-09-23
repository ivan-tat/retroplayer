(* stdio.pas -- Pascal declarations for custom "stdio" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit stdio;

interface

(*$I defines.pas*)

procedure cc_fclose_;
procedure cc_fopen_;
procedure cc_fread_;
procedure cc_fsetpos_;
procedure cc_fwrite_;

procedure cc_fprintf_;
procedure cc_printf_;
procedure cc_snprintf_;
procedure cc_sprintf_;
procedure cc_vfprintf_;
procedure cc_vprintf_;
procedure cc_vsnprintf_;
procedure cc_vsprintf_;

implementation

uses
    watcom,
    pascal,
    errno_,
    ctype,
    dos_,
    string_;

procedure pascal_assign(var f: file; path: pchar); far;
begin
    System.Assign(f, path);
end;

function pascal_reset(var f: file): boolean; far;
begin
    (*$I-*)
    System.Reset(f, 1);
    (*$I+*)
    pascal_reset := IOResult = 0;
end;

function pascal_rewrite(var f: file): boolean; far;
begin
    (*$I-*)
    System.Rewrite(f, 1);
    (*$I+*)
    pascal_rewrite := IOResult = 0;
end;

procedure pascal_close(var f: file); far;
begin
    System.Close(f);
end;

function pascal_seek(var f: file; pos: longint): boolean; far;
begin
    (*$I-*)
    System.Seek(f, pos);
    (*$I+*)
    pascal_seek := IOResult = 0;
end;

function pascal_blockread(var f: file; var buf; size: word; var actual: word): boolean; far;
begin
    (*$I-*)
    System.BlockRead(f, buf, size, actual);
    (*$I+*)
    pascal_blockread := IOResult = 0;
end;

function pascal_blockwrite(var f: file; var buf; size: word; var actual: word): boolean; far;
begin
    (*$I-*)
    System.BlockWrite(f, buf, size, actual);
    (*$I+*)
    pascal_blockwrite := IOResult = 0;
end;

procedure pascal_write(s: pchar); far;
begin
    Write(s);
end;

(*$L stdio/fclose.obj*)
procedure cc_fclose_; external;
(*$L stdio/fopen.obj*)
procedure cc_fopen_; external;
(*$L stdio/fread.obj*)
procedure cc_fread_; external;
(*$L stdio/fsetpos.obj*)
procedure cc_fsetpos_; external;
(*$L stdio/fwrite.obj*)
procedure cc_fwrite_; external;

(*$L stdio/_printf.obj*)
procedure dataStreamFlush_; external;
procedure dataStreamWrite_; external;
procedure dataStreamInitMemory_; external;
procedure dataStreamInitStdOut_; external;
procedure dataStreamInitFile_; external;
procedure _dsprintf_; external;

(*$L stdio/printf.obj*)
procedure cc_printf_; external;
(*$L stdio/fprintf.obj*)
procedure cc_fprintf_; external;
(*$L stdio/sprintf.obj*)
procedure cc_sprintf_; external;
(*$L stdio/snprintf.obj*)
procedure cc_snprintf_; external;
(*$L stdio/vprintf.obj*)
procedure cc_vprintf_; external;
(*$L stdio/vfprintf.obj*)
procedure cc_vfprintf_; external;
(*$L stdio/vsprintf.obj*)
procedure cc_vsprintf_; external;
(*$L stdio/vsnprntf.obj*)
procedure cc_vsnprintf_; external;

end.
