(* debug.pas -- Pascal declarations for "debug" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    debug;

interface

(*$I defines.pas*)

procedure _DEBUG_LOG_;
procedure _DEBUG_BEGIN_;
procedure _DEBUG_END_;
procedure _DEBUG_FAIL_;
procedure _DEBUG_SUCCESS_;
procedure _DEBUG_REG_;
procedure _DEBUG_UNREG_;
procedure Debug_Msg(filename: PChar; method: PChar; msg: PChar);
procedure Debug_Info(filename: PChar; method: PChar; msg: PChar);
procedure Debug_Warn(filename: PChar; method: PChar; msg: PChar);
procedure Debug_Err(filename: PChar; method: PChar; msg: PChar);
procedure Debug_Begin(filename: PChar; method: PChar);
procedure Debug_End(filename: PChar; method: PChar);
procedure Debug_Fail(filename: PChar; method: PChar; msg: PChar);
procedure Debug_Success(filename: PChar; method: PChar);

implementation

uses
    pascal,
    stdio,
    stdlib,
    string_,
    conio;

var
    debuglogfile: file;

(*$L debug.obj*)
procedure _DEBUG_LOG_; external;
procedure _DEBUG_BEGIN_; external;
procedure _DEBUG_END_; external;
procedure _DEBUG_FAIL_; external;
procedure _DEBUG_SUCCESS_; external;
procedure _DEBUG_REG_; external;
procedure _DEBUG_UNREG_; external;
procedure Debug_Msg(filename: PChar; method: PChar; msg: PChar); external;
procedure Debug_Info(filename: PChar; method: PChar; msg: PChar); external;
procedure Debug_Warn(filename: PChar; method: PChar; msg: PChar); external;
procedure Debug_Err(filename: PChar; method: PChar; msg: PChar); external;
procedure Debug_Begin(filename: PChar; method: PChar); external;
procedure Debug_End(filename: PChar; method: PChar); external;
procedure Debug_Fail(filename: PChar; method: PChar; msg: PChar); external;
procedure Debug_Success(filename: PChar; method: PChar); external;

procedure register_debug; far; external;

begin
    register_debug;
end.
