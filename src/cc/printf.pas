(* _printf.pas -- Pascal declarations for _printf.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit printf;

interface

(*$I defines.pas*)

procedure _printf;

implementation

uses
    watcom,
    string_,
    startup,
    dstream;

(*$L _printf.obj*)
procedure _printf; external;

end.
