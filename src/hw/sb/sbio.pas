(* sbio.pas -- Pascal declarations for sbio.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit sbio;

interface

(*$I defines.pas*)

type
    TSBIOError = word;

const
    E_SBIO_SUCCESS          = 0;
    E_SBIO_DSP_RESET_FAILED = 1;
    E_SBIO_DSP_READ_FAILED  = 2;
    E_SBIO_DSP_WRITE_FAILED = 3;

var
    sbioError: TSBIOError;

const
    SBIO_MIXER_DAC_LEVEL     = $04;
    SBIO_MIXER_MASTER_VOLUME = $22;
    SBIO_MIXER_MASTER_LEFT   = $30;
    SBIO_MIXER_MASTER_RIGHT  = $31;
    SBIO_MIXER_VOICE_LEFT    = $32;
    SBIO_MIXER_VOICE_RIGHT   = $33;

procedure sbioMixerReset( base: word );
function  sbioMixerRead( base: word; reg: byte ): byte;
procedure sbioMixerWrite( base: word; reg, data: byte );
function  sbioDSPReset( base: word ): boolean;
function  sbioDSPRead( base: word ): byte;
function  sbioDSPWrite( base: word; data: byte ): boolean;
procedure sbioDSPAcknowledgeIRQ( base: word; mode16bit: boolean );

implementation

(*$l sbio.obj*)

procedure sbioMixerReset( base: word ); external;
function  sbioMixerRead( base: word; reg: byte ): byte; external;
procedure sbioMixerWrite( base: word; reg, data: byte ); external;
function  sbioDSPReset( base: word ): boolean; external;
function  sbioDSPRead( base: word ): byte; external;
function  sbioDSPWrite( base: word; data: byte ): boolean; external;
procedure sbioDSPAcknowledgeIRQ( base: word; mode16bit: boolean ); external;

end.
