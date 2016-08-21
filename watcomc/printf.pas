(* Declarations for printf.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit printf;

interface

procedure printf_;

implementation

uses
    i4d;

(*$l printf.obj*)

procedure printstring( s: pchar ); near;
begin
    write( s );
end;

procedure printf_; external;

end.
