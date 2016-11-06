(* fillvars.pas -- Pascal declarations for fillvars.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit fillvars;

interface

(* DMA buffer *)
var
    AllocBuffer: pointer;
        (* position where we allocate DMA buffer - remember that we may use second half ... *)
    DMAbuffer: pointer;
    PlayBuffer: pointer;
        (* pointer to DMAbuffer - for public use, but don't write into it !!!
           - it's never used for any action while mixing !
           - while playing you can read the DMA base counter
           and find out in that way what sample value the
           SB currently plays ... refer to DMA Controller *)
    NumBuffers: byte;
        (* number of parts in DMAbuffer *)
    LastReady: byte;
        (* last ready calculated DMAbuffer part *)
    DMAHalf: byte;
        (* last DMAbuffer part to calculate *)
    DMARealBufSize: array [0..63] of word;
        (* e.g. 0,128,256,384 <- positions of dmabuffer parts (changes with samplerate) *)

(* mixer *)
var
    TickBuffer: pointer;
        (* the well known buffer for one tick - size depends on _currennt_tempo_ *)
    LQMode: boolean;
        (* flag if lowquality mode *)
    TooSlow: boolean;
    JustInFill: boolean;
    RasterTime: boolean;
    FPS: byte;
        (* frames per second ... default is about 70Hz *)

(* EMM *)
var
    SavHandle: word;
        (* EMS handle for saving mapping while playing *)

implementation

end.
