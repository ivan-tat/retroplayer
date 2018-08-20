(* vga.pas -- declarations for vga.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit vga;

interface

(*$I defines.pas*)

var
    drawseg: Word;

procedure vga_wait_vsync;
procedure vga_clear_page_320x200x8(c: Byte);
procedure vga_line(x1, y1, x2, y2: Word; f: Byte);
procedure vga_bar(o, b, l: Word);

implementation

uses
    pascal,
    i86,
    stdlib,
    string_,
    conio,
    debug;

(*$l vga.obj*)

procedure vga_wait_vsync; external;
procedure vga_clear_page_320x200x8(c: Byte); external;

(*$L line.obj*)
procedure vga_line(x1, y1, x2, y2: Word; f: Byte); external;

procedure vga_bar(o, b, l: Word); assembler;
asm
      mov      ax,[drawseg]
      mov      es,ax
      mov      di,[o]
      mov      bx,320
      mov      dx,[b]
      add      bx,dx
      push     bp
      mov      bp,[l]
      cmp      bp,0
      je       @@n
      shl      bp,1
      mov      ax,7
    @@b:
      mov      cx,dx
      rep stosb
      sub      di,bx
      dec      bp
      jnz      @@b
    @@n:
      pop      bp
      push     bp
      mov      bp,[l]
      neg      bp
      add      bp,64
      shl      bp,1
      mov      ax,1
      cmp      bp,0
      jz       @@n2
    @@b2:
      mov      cx,dx
      rep stosb
      sub      di,bx
      dec      bp
      jnz      @@b2
    @@n2:
      pop      bp
    @@e:
end;

procedure register_vga; far; external;

begin
    register_vga;
end.
