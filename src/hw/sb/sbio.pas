(* sbio.pas -- Pascal declarations for sbio.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit sbio;

interface

(*$I defines.pas*)

(*$ifdef DEFINE_LOCAL_DATA*)

var
    sbioError: Word;

(*$endif*)  (* DEFINE_LOCAL_DATA *)

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
