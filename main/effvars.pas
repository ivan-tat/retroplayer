(* effvars.pas -- Pascal declarations for effvars.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit effvars;

interface

(* tables for mixing *)
var
    sinuswave: array [0..63] of shortint;
    rampwave: array [0..63] of shortint;
    squarewave: array [0..63] of byte;

const
    wavetab: array [0..2] of word = (
        ofs(sinuswave),
        ofs(rampwave),
        ofs(squarewave) (* looks not like a square but anyway *)
        (* 'random wave' is not a table, but a call for a random number ! *)
    );

implementation

end.
