(* effects.pas -- Pascal declarations for effects.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit effects;

interface

(*$I defines.pas*)

uses
    s3mtypes,
    mixchn;

procedure chn_effInit;
procedure chn_effHandle;
procedure chn_effTick;
procedure chn_effCanContinue;
procedure chn_effStop;
procedure chn_effGetName;

implementation

uses
    watcom,
    string_,
    stdio,
    ems,
    musins,
    musmodps,
    effvars,
    mixer;

(*$l effects.obj*)

procedure chn_effInit; external;
procedure chn_effHandle; external;
procedure chn_effTick; external;
procedure chn_effCanContinue; external;
procedure chn_effStop; external;
procedure chn_effGetName; external;

end.
