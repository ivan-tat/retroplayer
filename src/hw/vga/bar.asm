; bar.asm - bar drawing routine.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large, pascal
.286

DGROUP group _DATA

_DATA segment word public use16 'DATA'
_DATA ends

BAR_TEXT segment word public use16 'CODE'
assume cs:BAR_TEXT,ds:DGROUP

public vga_bar
vga_bar proc far buf: dword, o: word, b: word, l: word
    les     ax,[buf]
    mov     di,[o]
    mov     bx,320
    mov     dx,[b]
    add     bx,dx
    push    bp
    mov     bp,[l]
    cmp     bp,0
    je      @@n
    shl     bp,1
    mov     ax,7
@@b:
    mov     cx,dx
    rep stosb
    sub     di,bx
    dec     bp
    jnz     @@b
@@n:
    pop     bp
    push    bp
    mov     bp,[l]
    neg     bp
    add     bp,64
    shl     bp,1
    mov     ax,1
    cmp     bp,0
    jz      @@n2
@@b2:
    mov     cx,dx
    rep stosb
    sub     di,bx
    dec     bp
    jnz     @@b2
@@n2:
    pop     bp
@@e:
    ret
vga_bar endp

BAR_TEXT ends

end
