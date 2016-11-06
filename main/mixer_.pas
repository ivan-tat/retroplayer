(* Pascal declarations for mixer_.asm. *)

unit mixer_;

interface

procedure _mixCalcSampleStep;
procedure _MixSampleMono8;
procedure _MixSampleStereo8;

implementation

uses
    mixvars;

(*$l mixer_.obj*)
procedure _mixCalcSampleStep; external;
procedure _MixSampleMono8; external;
procedure _MixSampleStereo8; external;

end.
