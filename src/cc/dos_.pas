(* dos_.pas -- Pascal declarations for custom "dos" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    dos_;

interface

uses
    dos;

(*$I defines.pas*)

(* DOS Program Segment Prefix *)

type
    dospsp_p = ^dospsp_t;
    dospsp_t = packed record
        int_20_opcode: Word;
        mem_size: Word;
        reserved1: Byte;
        dos_func_dispatcher: array [0..4] of Byte;
        int_22_ptr: Pointer;
        int_23_ptr: Pointer;
        int_24_ptr: Pointer;
        parent_seg: Word;
        file_handles_array: array [0..19] of Byte;
        env_seg: Word;
        last_stack_ptr: Pointer;
        handles_array_size: Word;
        handles_array_ptr: Pointer;
        prev_psp_ptr: Pointer;
        reserved2: array [0..19] of Byte;
        int_21_retf_opcodes: array [0..2] of Byte;
        reserved3: array[0..8] of Byte;
        FCB1: array [0..15] of Byte;
        FCB2: array [0..19] of Byte;
        param_str: String [127];
    end;

(* DOS Memory Control Block *)

type
  dosmcb_p = ^dosmcb_t;
  dosmcb_t = packed record
    ident: Char;
    owner_psp_seg: Word;
    size: Word;
    reserved: array [0..10] of Byte;
    program_name: array [0..7] of Char;
    data: Byte;
  end;

procedure pascal_swapvectors;
procedure pascal_exec(Name: PathStr; CmdLine: String);

procedure __cc_set_errno_dos;

procedure __cc_doserror;

procedure _cc_dos_getdate;
procedure _cc_dos_gettime;

procedure _cc_dos_getvect;
procedure _cc_dos_setvect;

procedure _cc_dos_para;
procedure _cc_dos_allocmem;
procedure _cc_dos_freemem;
procedure _cc_dos_setblock;

procedure _cc_dos_getpsp;
procedure _cc_dos_getmasterpsp;

procedure _cc_dos_creat;
procedure _cc_dos_creatnew;
procedure _cc_dos_open;

procedure _cc_dos_read;
procedure _cc_dos_write;

procedure _cc_dos_close;
procedure _cc_dos_commit;

implementation

uses
    pascal,
    errno_,
    i86;

procedure pascal_swapvectors;
begin
    Dos.SwapVectors;
end;

procedure pascal_exec(Name: PathStr; CmdLine: String);
begin
    Dos.Exec(Name, CmdLine);
end;

(*$L dos\dosret.obj*)
procedure __cc_set_errno_dos; external;

(*$L dos\error086.obj*)
procedure __cc_doserror; external;

(*$L dos\gtime086.obj*)
procedure _cc_dos_getdate; external;
procedure _cc_dos_gettime; external;

(*$L dos\d_getvec.obj*)
procedure _cc_dos_getvect; external;

(*$L dos\d_setvec.obj*)
procedure _cc_dos_setvect; external;

(*$L dos\mem086.obj*)
procedure _cc_dos_para; external;
procedure _cc_dos_allocmem; external;
procedure _cc_dos_freemem; external;
procedure _cc_dos_setblock; external;

(*$L dos\psp.obj*)
procedure _cc_dos_getpsp; external;
procedure _cc_dos_getmasterpsp; external;

(*$L dos\creatdos.obj*)
procedure _cc_dos_creat; external;
procedure _cc_dos_creatnew; external;

(*$L dos\opendos.obj*)
procedure _cc_dos_open; external;

(*$L dos\io086.obj*)
procedure _cc_dos_read; external;
procedure _cc_dos_write; external;

(*$L dos\filedos.obj*)
procedure _cc_dos_close; external;
procedure _cc_dos_commit; external;

end.
