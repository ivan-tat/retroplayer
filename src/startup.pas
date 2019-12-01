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
    _cc_psp: Word;          (* absolute System.PrefixSeg; *)
    _cc_argc: Word;
    _cc_argv: Pointer;
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

(*procedure _cc_on_exit;*)
procedure _cc_startup;
procedure _cc_ExitWithError;
procedure _cc_Exit;

procedure cc_FileAssign;
procedure cc_FileSetTextBuf;
procedure cc_FileReset;
procedure cc_FileRewrite;
procedure cc_FileAppend;
procedure cc_FileFlush;
procedure cc_FileClose;
procedure cc_FileEOL;
procedure cc_FileReadString;
procedure cc_FileReadChar;
procedure cc_FileReadInteger;
procedure cc_FileWriteString;
procedure cc_FileWriteChar;
procedure cc_FileWriteInteger;
procedure cc_FileWriteLn;
procedure cc_FileFlushBuffer;

implementation

uses
    watcom,
    pascal,
    cpu,
    string_,
    stdio,
    sysdbg,
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
(*procedure _cc_on_exit; external;*)
procedure _cc_startup; external;
procedure _cc_ExitWithError; external;
procedure _cc_Exit; external;

procedure cc_FileAssign; external;
procedure cc_FileSetTextBuf; external;
procedure cc_FileReset; external;
procedure cc_FileRewrite; external;
procedure cc_FileAppend; external;
procedure cc_FileFlush; external;
procedure cc_FileClose; external;
procedure cc_FileEOL; external;
procedure cc_FileReadString; external;
procedure cc_FileReadChar; external;
procedure cc_FileReadInteger; external;
procedure cc_FileWriteString; external;
procedure cc_FileWriteChar; external;
procedure cc_FileWriteInteger; external;
procedure cc_FileWriteLn; external;
procedure cc_FileFlushBuffer; external;

end.
