(* sbctl.pas -- Pascal declarations for sbctl.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit sbctl;

interface

(*$I defines.pas*)

(* The different SoundBlaster versions and its hardware :

   #  Name                   Mono    Stereo   Bits   Mixer   DSP
   0. None                       -        -      -       -      -
   1. Soundblaster 1.0
      Soundblaster 1.5       23kHz        -      8      no   1.xx

   2. Soundblaster Pro       45kHz    23kHz      8     yes   3.xx

   3. Soundblaster 2.0                                 no?
      Audioblaster 2.5       45kHz        -      8     yes   2.xx

   4. Soundblaster Pro 3
      Audioblaster Pro 4.0   45kHz    23kHz      8     yes   3.xx

   5. Soundblaster Pro (Mircochannel)
        Special version for PS/2 - technical data = 4

   6. Soundblaster 16
      Soundblaster 16 ASP    45kHz    45kHz    8,16    yes   4.xx

   DSP versions :

   SoundBlaster 1.0/1.5        1.xx
   SoundBlaster 2.0/2.5        2.xx
   SoundBlaster Pro/PRO3/PRO4  3.xx
   SoundBlaster 16/ASP         4.xx
*)

var
    sdev_name: PChar;
    sdev_hw_base: word;
    sdev_hw_irq: byte;
    sdev_hw_dma8: byte;
    sdev_hw_dma16: byte;

    sbno: byte;                 (* SoundBlaster typ (look some rows above) *)

    sdev_caps_mixer: boolean;
    sdev_caps_stereo: boolean;
    sdev_caps_16bit: boolean;
    sdev_caps_mono_maxrate: word;
    sdev_caps_stereo_maxrate: word;

    sdev_mode_rate: word;
    sdev_mode_16bit: boolean;
    sdev_mode_signed: boolean;
    sdev_mode_stereo: boolean;

procedure setvolume( vol: byte );
function sbReadDSPVersion: word;
procedure sbAdjustMode( var rate: word; var stereo: boolean; var _16bit: boolean );
procedure sbSetupMode( freq: word; stereo: boolean );
procedure sbSetupDMATransfer( p: pointer; count: word; autoinit: boolean );
function sbGetDMACounter: word;
procedure sbSetupDSPTransfer( len: word; auto: boolean );
procedure pause_play;
procedure continue_play;
procedure stop_play;

implementation

uses
    pascal,
    watcom,
    i86,
    dma,
    sbio;

(*$l sbctl.obj*)

procedure setvolume( vol: byte ); external;
function sbReadDSPVersion: word; external;
procedure sbAdjustMode( var rate: word; var stereo: boolean; var _16bit: boolean ); external;
procedure sbSetupMode( freq: word; stereo: boolean ); external;
procedure sbSetupDMATransfer( p: pointer; count: word; autoinit: boolean ); external;
function sbGetDMACounter: word; external;
procedure sbSetupDSPTransfer( len: word; auto: boolean ); external;
procedure pause_play; external;
procedure continue_play; external;
procedure stop_play; external;

end.