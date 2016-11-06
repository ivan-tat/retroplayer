(* mixvars -- Pascal declarations for mixvars.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixvars;

interface

const
    ST3Periods: array [0..11] of word = (
        1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907
    );

var
    UseRate: word;
    BPT: word;
        (* bytes per tick - depends on samplerate + tempo *)
    TickBytesLeft: word;
        (* Bytes left to next Tick *)

implementation

end.
