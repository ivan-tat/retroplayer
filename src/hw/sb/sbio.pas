(* sbio.pas -- Pascal declarations for sbio.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit sbio;

interface

(*$I defines.pas*)

type
    TSBIOERR = Word;

const
    E_SBIO_SUCCESS          = 0;
    E_SBIO_DSP_RESET_FAILED = 1;
    E_SBIO_DSP_READ_FAILED  = 2;
    E_SBIO_DSP_WRITE_FAILED = 3;

var
    sbioError: TSBIOERR;

const
    SBIO_MIXER_DAC_LEVEL     = $04;
    SBIO_MIXER_MASTER_VOLUME = $22;
    SBIO_MIXER_MASTER_LEFT   = $30;
    SBIO_MIXER_MASTER_RIGHT  = $31;
    SBIO_MIXER_VOICE_LEFT    = $32;
    SBIO_MIXER_VOICE_RIGHT   = $33;

procedure sbioMixerReset(base: Word);
function  sbioMixerRead(base: Word; reg: Byte): Byte;
procedure sbioMixerWrite(base: Word; reg, data: Byte);
function  sbioDSPReset(base: Word): Boolean;
function  sbioDSPRead(base: Word): Byte;
function  sbioDSPReadQueue(base: Word; data: Pointer; length: Word): Boolean;
function  sbioDSPWrite(base: Word; data: Byte): Boolean;
function  sbioDSPWriteQueue(base: Word; data: Pointer; length: Word): Boolean;
procedure sbioDSPAcknowledgeIRQ(base: Word; mode16bit: Boolean);

implementation

(*$l sbio.obj*)

procedure sbioMixerReset(base: Word); external;
function  sbioMixerRead(base: Word; reg: Byte): Byte; external;
procedure sbioMixerWrite(base: Word; reg, data: Byte); external;
function  sbioDSPReset(base: Word): Boolean; external;
function  sbioDSPRead(base: Word): Byte; external;
function  sbioDSPReadQueue(base: Word; data: Pointer; length: Word): Boolean; external;
function  sbioDSPWrite(base: Word; data: Byte): Boolean; external;
function  sbioDSPWriteQueue(base: Word; data: Pointer; length: Word): Boolean; external;
procedure sbioDSPAcknowledgeIRQ(base: Word; mode16bit: Boolean); external;

end.
