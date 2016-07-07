(* Declarations for SBCTL.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit sbctl;

interface

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
    sbno:byte;                  (* SoundBlaster typ (look some rows above) *)
    dsp_addr:word;              (* Baseaddress detected SB uses *)
    sdev_hw_irq: byte;
    sdev_hw_dma8: byte;
    sdev_hw_dma16: byte;

    stereo_possible:boolean;    (* flag if stereo is possible on detected SB *)
    _16bit_possible:boolean;    (* flag if 16bit play is possible on detected SB *)
    maxstereorate:word;         (* max stereo samplerate on detected SB *)
    maxmonorate:word;           (* max mono samplerate on detected SB *)

    _16bit:boolean;             (* flag if 16bit-play on/off *)
    signeddata:boolean;         (* play signed data ? (only on SB16 possible) *)
    stereo:boolean;             (* flag if stereo-play on/off *)

function sbMixerRead( reg: byte ): byte;
procedure sbMixerWrite( reg, data: byte );
procedure speaker_on;
procedure speaker_off;

procedure setupDMATransfer( p: pointer; count: word; autoinit: boolean );
  (* config DMAcontroller for different transfer modes *)

procedure sbSetupDSPTransfer( len: word; b16, auto: boolean );

implementation

uses crt, dma, sbio;

(*$l sbctl.obj*)

function sbMixerRead( reg: byte ): byte; external;
procedure sbMixerWrite( reg, data: byte ); external;
procedure speaker_on; external;
procedure speaker_off; external;
procedure setupDMATransfer( p: pointer; count: word; autoinit: boolean ); external;
procedure sbSetupDSPTransfer( len: word; b16, auto: boolean ); external;
end.
