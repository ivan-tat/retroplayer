(* hexdigts.pas -- Pascal declarations for "hexdigts" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    hexdigts;

interface

(*$I defines.pas*)

(*$ifdef DEFINE_LOCAL_DATA*)

const
    HEXDIGITS: array [0..15] of char = '0123456789ABCDEF';

(*$endif*)

implementation

end.
