(* effects.pas -- Pascal declarations for effects.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit effects;

interface

uses
    s3mtypes;

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

var
    playState_jumpToOrder_bFlag: boolean;
    playState_jumpToOrder_bPos: byte;
    playState_patBreak_bFlag: boolean;
    playState_patBreak_bPos: byte;
    playState_patLoop_bNow: boolean;
    playState_gVolume_bFlag: boolean;
    playState_gVolume_bValue: byte;
    playState_patDelay_bNow: boolean;
    chnState_patDelay_wCommandSaved: word;
    chnState_patDelay_bParameterSaved: byte;
    chnState_cur_bNote: byte;
    chnState_cur_bIns: byte;
    chnState_cur_bVol: byte;
    chnState_porta_flag: boolean;
    chnState_porta_wSmpPeriodOld: word;
    chnState_porta_dSmpStepOld: longint;
    chnState_arp_bFlag: boolean;

procedure set_tempo(tempo: byte);
function  mapPatternData(p: pointer): pointer;
procedure chn_setSamplePeriod(var chn: TChannel; period: longint);
procedure chn_setSampleVolume(var chn: TChannel; vol: integer);
procedure chn_setupInstrument(var chn: TChannel; insNum: byte);
function  chn_calcNotePeriod(var chn: TChannel; ins: pointer; note: byte): word;
function  chn_calcNoteStep(var chn: TChannel; ins: pointer; note: byte): longint;
procedure chn_setupNote(var chn: TChannel; note: byte; keep: boolean);
function  chn_effInit(var chn: TChannel; param: byte): boolean;
procedure chn_effHandle(var chn: TChannel);
procedure chn_effTick(var chn: TChannel);

implementation

uses
    i4d,
    i4m,
    emstool,
    s3mvars,
    effvars,
    mixvars,
    mixer;

(*$l effects.obj*)

procedure set_tempo(tempo: byte); external;
function  mapPatternData(p: pointer): pointer; external;
procedure chn_setSamplePeriod(var chn: TChannel; period: longint); external;
procedure chn_setSampleVolume(var chn: TChannel; vol: integer); external;
procedure chn_setupInstrument(var chn: TChannel; insNum: byte); external;
function  chn_calcNotePeriod(var chn: TChannel; ins: pointer; note: byte): word; external;
function  chn_calcNoteStep(var chn: TChannel; ins: pointer; note: byte): longint; external;
procedure chn_setupNote(var chn: TChannel; note: byte; keep: boolean); external;
function  chn_effInit(var chn: TChannel; param: byte): boolean; external;
procedure chn_effHandle(var chn: TChannel); external;
procedure chn_effTick(var chn: TChannel); external;

end.
