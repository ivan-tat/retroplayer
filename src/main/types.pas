(* types.pas -- generic types.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit types;

interface

(*$I defines.pas*)

type
    dword = packed record
    case integer of
        0: ( DD: longint );
        1: ( LX, HX: word );
        2: ( Rea, Int: word );
    end;
    tarray = array[0..65532] of byte;
    parray = ^tarray;

implementation

end.
