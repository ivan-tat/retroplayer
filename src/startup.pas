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

const
    STDINBUF_SIZE = 128;
    STDOUTBUF_SIZE = 128;

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
    cc_InputBuf: array [1..STDINBUF_SIZE] of Byte;
    cc_Output: File;        (* absolute System.Output; *)
    cc_OutputBuf: array [1..STDOUTBUF_SIZE] of Byte;

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
procedure cc_system_init;
procedure _cc_ExitWithError;
procedure _cc_Exit;

procedure cc_IOResult;
procedure _cc_CheckInOutRes;

procedure cc_TextAssign;
procedure cc_TextSetTextBuf;
procedure cc_TextReset;
procedure cc_TextRewrite;
procedure cc_TextAppend;
procedure cc_TextFlush;
procedure cc_TextClose;
procedure cc_TextEOL;
procedure cc_TextReadString;
procedure cc_TextReadChar;
procedure cc_TextReadInteger;
procedure cc_TextWriteString;
procedure cc_TextWriteChar;
procedure cc_TextWriteInteger;
procedure cc_TextWriteLn;
procedure cc_TextSync;

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

(*$l startup\chkio.obj*)

procedure _cc_CheckInOutRes; external;

(*$l startup.obj*)

procedure _cc_local_int0; external;
procedure _cc_local_int23; external;
(*procedure _cc_on_exit; external;*)
procedure cc_system_init; external;
procedure _cc_ExitWithError; external;
procedure _cc_Exit; external;

procedure cc_IOResult; external;

procedure cc_TextAssign; external;
procedure cc_TextSetTextBuf; external;
procedure cc_TextReset; external;
procedure cc_TextRewrite; external;
procedure cc_TextAppend; external;
procedure cc_TextFlush; external;
procedure cc_TextClose; external;
procedure cc_TextEOL; external;
procedure cc_TextReadString; external;
procedure cc_TextReadChar; external;
procedure cc_TextReadInteger; external;
procedure cc_TextWriteString; external;
procedure cc_TextWriteChar; external;
procedure cc_TextWriteInteger; external;
procedure cc_TextWriteLn; external;
procedure cc_TextSync; external;

end.
