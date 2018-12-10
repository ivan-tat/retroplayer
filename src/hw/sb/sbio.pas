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

procedure sbioMixerReset;
procedure sbioMixerRead;
procedure sbioMixerWrite;

procedure sbioDSPReset;
procedure sbioDSPRead;
procedure sbioDSPReadQueue;
procedure sbioDSPWrite;
procedure sbioDSPWriteQueue;

procedure sbioDSPAcknowledgeIRQ;

implementation

(*$l sbio.obj*)

procedure sbioMixerReset; external;
procedure sbioMixerRead; external;
procedure sbioMixerWrite; external;

procedure sbioDSPReset; external;
procedure sbioDSPRead; external;
procedure sbioDSPReadQueue; external;
procedure sbioDSPWrite; external;
procedure sbioDSPWriteQueue; external;

procedure sbioDSPAcknowledgeIRQ; external;

end.
