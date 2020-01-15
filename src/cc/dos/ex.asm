; ex.asm -- part of custom "dos" library.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large,c
.286

_DATA segment word public use16 'DATA'
_DATA ends

_BSS segment word public use16 'DATA'
saved_stack dd ?
_BSS ends

DGROUP group _DATA, _BSS

EX_TEXT segment word public use16 'CODE'
assume cs:EX_TEXT, ds:DGROUP, ss:DGROUP

public _cc_dos_exec_asm
_cc_dos_exec_asm proc far pPath: dword, pParam: dword
    push    bx
    push    cx
    push    dx
    push    si
    push    di
;    push    ds
    mov     word ptr [saved_stack+0],sp
    mov     word ptr [saved_stack+2],ss
    mov     ax,4b00h
    lds     dx,[pPath]
    les     bx,[pParam]
    int     21h
    jb      short @error
    xor     ax,ax
@error:
    mov     bx,DGROUP
    mov     ds,bx
    cli
    mov     sp,word ptr [saved_stack+0]
    mov     ss,word ptr [saved_stack+2]
    sti
;    pop     ds
    pop     di
    pop     si
    pop     dx
    pop     cx
    pop     bx
    ret
_cc_dos_exec_asm endp

EX_TEXT ends

end
