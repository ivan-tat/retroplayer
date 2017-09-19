(* mixtypes.pas -- Pascal declarations for mixtypes.h.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixtypes;

interface

(*$I defines.pas*)

const
    MIXSMPFLAG_16BITS = $01;
    MIXSMPFLAG_LOOP   = $02;

type
    TPlaySampleInfo = record
        dData: pointer;
        dPos: longint;
        dStep: longint;
        wLen: word;
        wLoopStart: word;
        wLoopEnd: word;
        wFlags: word;
    end;

implementation

end.
