.387

REGPACK struct
_ax     dw ?
_bx     dw ?
_cx     dw ?
_dx     dw ?
_bp     dw ?
_si     dw ?
_di     dw ?
_ds     dw ?
_es     dw ?
_flags  dw ?
REGPACK ends

DGROUP group _DATA

DOINTR_TEXT segment word public use16 'CODE'
assume cs:DOINTR_TEXT, ds:DGROUP, ss:DGROUP

public _cc_DoINTR
_cc_DoINTR proc far
; int num (bx)
; union REGPACK *regs (dx:ax)
    push    bp
    push    ds
    push    dx
    push    ax
    xor     bh,bh
    mov     cx,bx
    shl     bx,1
    add     bx,cx
    add     bx,offset inttable
    mov     cx,cs
    mov     es,cx
    call    near ptr doit

    push    bx
    push    ds

    push    bp
    mov     bp,sp
    lds     bx,dword ptr [bp+6]
    pushf
    pop     word ptr [bx][REGPACK._flags]
    mov     word ptr [bx][REGPACK._ax],ax
    mov     word ptr [bx][REGPACK._cx],cx
    mov     word ptr [bx][REGPACK._dx],dx
    mov     word ptr [bx][REGPACK._si],si
    mov     word ptr [bx][REGPACK._di],di
    pop     word ptr [bx][REGPACK._bp]
    pop     word ptr [bx][REGPACK._ds]
    pop     word ptr [bx][REGPACK._bx]
    mov     word ptr [bx][REGPACK._es],es
    add     sp,4
    pop     ds
    pop     bp
    ret
_cc_DoINTR endp

doit:
; union REGPACK *regs (dx:ax)
    push    es
    push    bx
    mov     ds,dx
    mov     bx,ax
    push    word ptr [bx][REGPACK._ds]
    mov     ax,word ptr [bx][REGPACK._ax]
    mov     cx,word ptr [bx][REGPACK._cx]
    mov     dx,word ptr [bx][REGPACK._dx]
    mov     si,word ptr [bx][REGPACK._si]
    mov     di,word ptr [bx][REGPACK._di]
    mov     bp,word ptr [bx][REGPACK._bp]
    mov     es,word ptr [bx][REGPACK._es]
    mov     bx,word ptr [bx][REGPACK._bx]
    pop     ds
    retf

int_stub_list macro _start, _end
local _num
_num = _start
  rept _end + 1 - _start
    int     _num
    if _num eq 3
    nop     ; alignment
    endif
    ret
    _num = _num + 1
  endm
endm

inttable:
    int_stub_list 0, 24H

    jmp     near ptr int25_stub
    jmp     near ptr int26_stub

    int_stub_list 27H, 0ffh

int25_stub:
    int     25H
    jae     int25_success
    popf
    stc
    ret
int25_success:
    popf
    clc
    ret

int26_stub:
    int     26H
    jae     int26_success
    popf
    stc
    ret
int26_success:
    popf
    clc
    ret

DOINTR_TEXT ends

_DATA segment word public use16 'DATA'
_DATA ends

end
