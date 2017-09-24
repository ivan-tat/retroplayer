(* effvars.pas -- Pascal declarations for effvars.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit effvars;

interface

(*$I defines.pas*)

(* tables for mixing *)

const
    sinuswave: array [0..63] of shortint =
(
(*$I _wsinus.inc*)
);

const
    rampwave: array [0..63] of shortint =
(
(*$I _wramp.inc*)
);

const
    squarewave: array [0..63] of shortint =
(
(*$I _wsquare.inc*)
);

const
    wavetab: array [0..2] of word =
    (
        ofs(sinuswave),
        ofs(rampwave),
        ofs(squarewave)
        (* 'random wave' is not a table, but a call for a random number ! *)
    );

implementation

end.
