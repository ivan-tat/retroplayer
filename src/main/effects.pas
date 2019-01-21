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

var
    rowState: packed record
        flags: Byte;
        jump_pos: Byte;
        break_pos: Byte;
        global_volume: Byte;
    end;
    chnState_patDelay_bCommandSaved: byte;
    chnState_patDelay_bParameterSaved: byte;
    chnState_cur_bNote: byte;
    chnState_cur_bIns: byte;
    chnState_cur_bVol: byte;
    chnState_porta_flag: boolean;
    chnState_porta_wSmpPeriodOld: word;
    chnState_porta_dSmpStepOld: longint;
    chnState_arp_bFlag: boolean;

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
    s3mvars,
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
