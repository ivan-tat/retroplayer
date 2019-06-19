(* startup.pas -- Pascal declarations for "startup" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    startup;

interface

(*$I defines.pas*)

(*$ifdef DEFINE_LOCAL_DATA*)

var
    cc_PrefixSeg: Word;     (* absolute System.PrefixSeg; *)
    cc_ErrorAddr: Pointer;  (* absolute System.ErrorAddr; *)
    cc_ExitProc: Pointer       absolute System.ExitProc;
    cc_ExitCode: Integer;   (* absolute System.ExitCode; *)
    cc_InOutRes: Integer       absolute System.InOutRes;
    cc_Test8086: Byte;      (* absolute System.Test8086; *)
    cc_Input: File;         (* absolute System.Input; *)
    cc_Output: File;        (* absolute System.Output; *)

const
    _cc_ExitList: array[1..32] of Pointer = (
        nil, nil, nil, nil, nil, nil, nil, nil,
        nil, nil, nil, nil, nil, nil, nil, nil,
        nil, nil, nil, nil, nil, nil, nil, nil,
        nil, nil, nil, nil, nil, nil, nil, nil
    );
    _cc_ExitCount: Byte = 0;

(*$endif*)

function  pascal_paramcount: Word;
procedure pascal_paramstr(var dest: String; i: Byte);
procedure pascal_Halt(status: Integer);

procedure custom_argc;
procedure custom_argv;
(*procedure _cc_on_exit;*)
procedure _cc_startup;
procedure _cc_ExitWithError;
procedure _cc_Exit;

implementation

uses
    watcom,
    pascal,
    cpu,
    string_,
    dos_;

function pascal_paramcount: Word;
begin
    pascal_paramcount := System.ParamCount;
end;

procedure pascal_paramstr(var dest: String; i: Byte);
begin
    dest := System.ParamStr(i);
end;

procedure pascal_Halt(status: Integer);
begin
    System.Halt(status);
end;

(*$l startup\ints.obj*)

procedure _cc_local_int0_asm; external;
procedure _cc_local_int23_asm; external;

(*$l startup.obj*)

procedure _cc_local_int0; external;
procedure _cc_local_int23; external;
procedure custom_argc; external;
procedure custom_argv; external;
(*procedure _cc_on_exit; external;*)
procedure _cc_startup; external;
procedure _cc_ExitWithError; external;
procedure _cc_Exit; external;

end.
