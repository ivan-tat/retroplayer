(* Declarations for SBCTL.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit sbctl;

interface

(* The different SoundBlaster versions and its hardware :

   0. - no soundblaster present
   1. Soundblaster 1.0/1.5
        23kHz(mono)                   8bit        no mixer

   2. Soundblaster Pro
        45kHz(mono)    23kHz(stereo)  8bit        mixer

   3. Soundblaster 2.0/ Audioblaster 2.5
        45kHz(mono)                   8bit        audiob. with mixer

   4. Soundblaster Pro3/Audioblaster Pro 4.0
        45kHz(mono)    23kHz(stereo)  8bit        mixer

   5. Soundblaster Pro (Mircochannel)
        Special version for PS/2 - technical data = 4

   6. Soundblaster 16/16ASP
        45kHz(mono)    45kHz(stereo)  8/16bit        mixer

   DSP versions :

   SoundBlaster 1.0/1.5        1.xx
   SoundBlaster 2.0/2.5        2.xx
   SoundBlaster Pro/PRO3/PRO4  3.xx
   SoundBlaster 16/ASP         4.xx
*)

var
    sbno:byte;                  (* SoundBlaster typ (look some rows above) *)
    dsp_addr:word;              (* Baseaddress detected SB uses *)
    irq_no:byte;                (* IRQ detected SB uses *)
    dma_channel:byte;           (* DMA channel for 8 Bit play *)
    dma_16bitchannel:byte;      (* DMA channel for 16 Bit play *)

    stereo_possible:boolean;    (* flag if stereo is possible on detected SB *)
    _16bit_possible:boolean;    (* flag if 16bit play is possible on detected SB *)
    maxstereorate:word;         (* max stereo samplerate on detected SB *)
    maxmonorate:word;           (* max mono samplerate on detected SB *)

    _16bit:boolean;             (* flag if 16bit-play on/off *)
    signeddata:boolean;         (* play signed data ? (only on SB16 possible) *)
    stereo:boolean;             (* flag if stereo-play on/off *)

function sbMixerRead( reg: byte ): byte;
procedure sbMixerWrite( reg, data: byte );
procedure sbSetupDSPTransfer( len: word; b16, auto: boolean );

implementation

uses sbio;

(*$l sbctl.obj*)

function sbMixerRead( reg: byte ): byte; external;
procedure sbMixerWrite( reg, data: byte ); external;
procedure sbSetupDSPTransfer( len: word; b16, auto: boolean ); external;

end.
