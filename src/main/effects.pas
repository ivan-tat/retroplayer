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

const
    EFFIDX_NONE              = 0;
    EFFIDX_A_SET_SPEED       = 1;
    EFFIDX_B_JUMP            = 2;
    EFFIDX_C_PATTERN_BREAK   = 3;
    EFFIDX_D_VOLUME_SLIDE    = 4;
    EFFIDX_E_PITCH_DOWN      = 5;
    EFFIDX_F_PITCH_UP        = 6;
    EFFIDX_G_PORTAMENTO      = 7;
    EFFIDX_H_VIBRATO         = 8;
    EFFIDX_I_TREMOR          = 9;
    EFFIDX_J_ARPEGGIO        = 10;
    EFFIDX_K_VIB_VOLSLIDE    = 11;
    EFFIDX_L_PORTA_VOLSLIDE  = 12;
    EFFIDX_M                 = 13;
    EFFIDX_N                 = 14;
    EFFIDX_O                 = 15;
    EFFIDX_P                 = 16;
    EFFIDX_Q_RETRIG_VOLSLIDE = 17;
    EFFIDX_R_TREMOLO         = 18;
    EFFIDX_S_SPECIAL         = 19;
    EFFIDX_T_SET_TEMPO       = 20;
    EFFIDX_U_FINE_VIBRATO    = 21;
    EFFIDX_V_SET_GVOLUME     = 22;
    EFFIDX_W                 = 23;
    EFFIDX_X                 = 24;
    EFFIDX_Y                 = 25;
    EFFIDX_Z                 = 26;

const
    MAXEFF = 22;

(*
const
    EFFGROUP_GLOBAL  = 0;
    EFFGROUP_PATTERN = 1;
    EFFGROUP_ROW     = 2;
    EFFGROUP_NOTE    = 3;
    EFFGROUP_PITCH   = 4;
    EFFGROUP_VOLUME  = 5;
    EFFGROUP_PANNING = 6;
    EFFGROUP_SAMPLE  = 7;
*)

var
    playState_jumpToOrder_bFlag: boolean;
    playState_jumpToOrder_bPos: byte;
    playState_patBreak_bFlag: boolean;
    playState_patBreak_bPos: byte;
    playState_patLoop_bNow: boolean;
    playState_gVolume_bFlag: boolean;
    playState_gVolume_bValue: byte;
    playState_patDelay_bNow: boolean;
    chnState_patDelay_bCommandSaved: byte;
    chnState_patDelay_bParameterSaved: byte;
    chnState_cur_bNote: byte;
    chnState_cur_bIns: byte;
    chnState_cur_bVol: byte;
    chnState_porta_flag: boolean;
    chnState_porta_wSmpPeriodOld: word;
    chnState_porta_dSmpStepOld: longint;
    chnState_arp_bFlag: boolean;

procedure set_speed(value: byte);
procedure set_tempo(value: byte);
function  chn_effInit(var chn: TChannel; param: byte): boolean;
procedure chn_effHandle(var chn: TChannel);
procedure chn_effTick(var chn: TChannel);
function  chn_effCanContinue(var chn: TChannel): boolean;
procedure chn_effStop(var chn: TChannel);

implementation

uses
    watcom,
    ems,
    musins,
    s3mvars,
    effvars,
    mixvars,
    mixer;

(*$l effects.obj*)

procedure set_speed(value: byte); external;
procedure set_tempo(value: byte); external;
function  chn_effInit(var chn: TChannel; param: byte): boolean; external;
procedure chn_effHandle(var chn: TChannel); external;
procedure chn_effTick(var chn: TChannel); external;
function  chn_effCanContinue(var chn: TChannel): boolean; external;
procedure chn_effStop(var chn: TChannel); external;

end.
