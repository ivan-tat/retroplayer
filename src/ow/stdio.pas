(* stdio.pas -- Pascal declarations for stdio.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit stdio;

interface

(*$I defines.pas*)

procedure printf_;
procedure fopen_;
procedure fclose_;

implementation

uses
    pascal,
    watcom,
    dos_;

procedure printstring(s: pchar); near;
begin
    write(s);
end;

(*$l printf.obj*)
procedure printf_; external;

(*$l stdio.obj*)
procedure fopen_; external;
procedure fclose_; external;

end.
