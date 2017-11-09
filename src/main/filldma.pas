(* filldma.pas -- Pascal declarations for filldma.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit filldma;

interface

uses
    fillvars;

(*$I defines.pas*)

procedure fill_DMAbuffer(mixbuf: pointer; outbuf: PSNDDMABUF);

implementation

uses
    string_,
    ems,
    sbctl,
    posttab,
    s3mvars,
    mixer,
    mixing;

(*$l filldma.obj*)
procedure fill_DMAbuffer(mixbuf: pointer; outbuf: PSNDDMABUF); external;

end.
