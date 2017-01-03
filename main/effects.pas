(* effects.pas -- Pascal declarations for effects.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit effects;

interface

uses
    s3mtypes;

procedure chn_setSamplePeriod(var chn: TChannel; period: longint);
procedure chn_setSampleVolume(var chn: TChannel; vol: integer);
procedure chn_setupInstrument(var chn: TChannel; insNum: byte);
function  chn_calcNotePeriod(var chn: TChannel; ins: pointer; note: byte): word;
function  chn_calcNoteStep(var chn: TChannel; ins: pointer; note: byte): longint;
procedure chn_setupNote(var chn: TChannel; note: byte; keep: boolean);
procedure chn_effTick(var chn: TChannel);

implementation

uses
    i4d,
    i4m,
    s3mvars,
    effvars,
    mixvars,
    mixer;

(*$l effects.obj*)

procedure chn_setSamplePeriod(var chn: TChannel; period: longint); external;
procedure chn_setSampleVolume(var chn: TChannel; vol: integer); external;
procedure chn_setupInstrument(var chn: TChannel; insNum: byte); external;
function  chn_calcNotePeriod(var chn: TChannel; ins: pointer; note: byte): word; external;
function  chn_calcNoteStep(var chn: TChannel; ins: pointer; note: byte): longint; external;
procedure chn_setupNote(var chn: TChannel; note: byte; keep: boolean); external;
procedure chn_effTick(var chn: TChannel); external;

end.
