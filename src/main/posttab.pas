(* posttab.pas -- Pascal declarations for posttab.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit posttab;

interface

(*$I defines.pas*)

procedure amptab_set_volume;
procedure amplify_s32;
procedure clip_s32_u8;
procedure clip_s32_s8;
procedure clip_s32_u16;
procedure clip_s32_s16;
procedure clip_s32_u8_lq;
procedure clip_s32_s8_lq;
procedure clip_s32_u16_lq;
procedure clip_s32_s16_lq;
procedure clip_s32_u8_lq_stereo;
procedure clip_s32_s8_lq_stereo;
procedure clip_s32_u16_lq_stereo;
procedure clip_s32_s16_lq_stereo;

implementation

uses
(*$ifdef DEBUG*)
    watcom,
    stdio;
(*$else*)
    watcom;
(*$endif*)

(*$l posttab.obj*)

procedure amptab_set_volume; external;
procedure amplify_s32; external;
procedure clip_s32_u8; external;
procedure clip_s32_s8; external;
procedure clip_s32_u16; external;
procedure clip_s32_s16; external;
procedure clip_s32_u8_lq; external;
procedure clip_s32_s8_lq; external;
procedure clip_s32_u16_lq; external;
procedure clip_s32_s16_lq; external;
procedure clip_s32_u8_lq_stereo; external;
procedure clip_s32_s8_lq_stereo; external;
procedure clip_s32_u16_lq_stereo; external;
procedure clip_s32_s16_lq_stereo; external;

end.
