; dosexe16.asm -- part of custom "startup" library.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.387
.model large

include "startup.def"

extern _start_c: far proc

CONST segment word public use16 'DATA'
CONST ends

CONST2 segment word public use16 'DATA'
CONST2 ends

_DATA segment word public use16 'DATA'
_DATA ends

_BSS segment word public use16 'BSS'
_BSS ends

STACK segment word public use16 'STACK'
STACK ends

; Segment "STACK_TOP" is used to determine a size of the program in memory.
; Segment must be aligned to a paragraph (16 bytes). It will be placed after
; "STACK" segment when linking (segments are arranged in ascending order).
STACK_TOP segment para public use16 'STACK'
STACK_TOP ends

DGROUP group CONST, CONST2, _DATA, _BSS, STACK, STACK_TOP

dosexe16_TEXT segment word public use16 'CODE'
assume cs:dosexe16_TEXT, ds:dgroup, ss:dgroup

public _start_asm
_start_asm:
    ; ES = PSP segment
    mov     ax, DGROUP
    mov     ds, ax
    mov     ax, es
    mov     [_cc_psp], ax
    ; DOS may allocate as much memory as possible for the program on startup.
    ; It depends on the linker (16-bit value at offset 0x000C of executable
    ; file is the number of paragraphs to allocate, 0xffff = maximum).
    ; To avoid this we must manually adjust allocated memory block: shrink
    ; allocated memory block for the program to it's minimum size.
    ; New size in paragraphs: from PSP to STACK_TOP.
    mov     bx, STACK_TOP
    sub     bx, ax
    mov     ah, 4ah ; DOS Function 4Ah - Shrink or expand a memory block.
    ; ES = segment of an allocated memory block (PSP for us).
    ; BX = a new size in paragraphs.
    int     21h
    ; We shrink a larger memory block (or at least it has the same size)
    ; so we may ignore returned status.
    jmp     _start_c

dosexe16_TEXT ends

end
