; sampler.asm -- sampler functions.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large,pascal
.386

DGROUP group _DATA

include "sampler.def"

_DATA segment word public use16 'DATA'

dwofs macro name, no
    dw offset &name&no
endm

; Play sample nearest methods

; 8-bits loop jump table:

LOOP_NEAREST_8_SHIFT equ 3
LOOP_NEAREST_8_LENGTH equ (1 shl LOOP_NEAREST_8_SHIFT)

counter = 0
table_nearest_8 label word
rept LOOP_NEAREST_8_LENGTH
    dwofs loop_nearest_8_, %counter
    counter = counter + 1
endm

; 16-bits loop jump table:

LOOP_NEAREST_16_SHIFT equ 3
LOOP_NEAREST_16_LENGTH equ (1 shl LOOP_NEAREST_16_SHIFT)

counter = 0
table_nearest_16 label word
rept LOOP_NEAREST_16_LENGTH
    dwofs loop_nearest_16_, %counter
    counter = counter + 1
endm

_DATA ends

SAMPLER_TEXT segment word public use16 'CODE'
assume cs:SAMPLER_TEXT, ds:DGROUP, ss:DGROUP

public _play_sample_nearest_8
_play_sample_nearest_8 proc far _dOutBuf: dword, _dSmpInfo: dword, _wCount: word
local _count: word
    push    ax
    push    bx
    push    cx
    push    dx
    push    si
    push    di
    push    ds
    push    es
    push    fs
    mov     bx,[_wCount]
    mov     cx,bx
    and     bx,LOOP_NEAREST_8_LENGTH-1
    shr     cx,LOOP_NEAREST_8_SHIFT
    inc     cx
    neg     bx
    add     bx,LOOP_NEAREST_8_LENGTH
    cmp     bx,LOOP_NEAREST_8_LENGTH
    jne     _play_sample_nearest_8@_no0
    dec     cx
    xor     bx,bx
_play_sample_nearest_8@_no0:
    mov     [_count],cx
    shl     bx,1
    les     di,[_dOutBuf]
    sub     di,bx
    mov     bx,ds:[table_nearest_8][bx]
    push    bx
    lfs     bx,[_dSmpInfo]
    xor     esi,esi
    lds     si,dword ptr fs:[bx][play_sample_info_t.dData]
    mov     cx,word ptr fs:[bx][play_sample_info_t.dPos]
    add     si,word ptr fs:[bx][play_sample_info_t.dPos+2]
    mov     dx,word ptr fs:[bx][play_sample_info_t.dStep+2]
    mov     bx,word ptr fs:[bx][play_sample_info_t.dStep]
    xor     al,al
; word at ss:[sp] = jump target
; al = always 0
; ah = new sample value
; bx = step.frac
; cx = pos.frac
; dx = step.int
; si = pos.int
; ds = sample data DOS segment
; es:di = output buffer
    retn    ; jump

align 2

loop_nearest_8 macro no
loop_nearest_8_&no:
    mov     ah,ds:[si]
    add     cx,bx
    adc     si,dx
    mov     es:[di+counter*2],ax
endm

counter = 0
rept LOOP_NEAREST_8_LENGTH
    loop_nearest_8 %counter
    counter = counter + 1
endm
    add     di,LOOP_NEAREST_8_LENGTH*2
    dec     [_count]
    jnz     loop_nearest_8_0

    mov     bx,word ptr [_dSmpInfo]
    sub     si,word ptr fs:[bx][play_sample_info_t.dData]

_play_sample_nearest_8_exit:
    mov     word ptr fs:[bx][play_sample_info_t.dPos],cx
    mov     word ptr fs:[bx][play_sample_info_t.dPos+2],si

    pop     fs
    pop     es
    pop     ds
    pop     di
    pop     si
    pop     dx
    pop     cx
    pop     bx
    pop     ax
    ret
_play_sample_nearest_8 endp

public _play_sample_nearest_16
_play_sample_nearest_16 proc far _dOutBuf: dword, _dSmpInfo: dword, _wCount: word
local _count: word
    push    ax
    push    bx
    push    cx
    push    dx
    push    si
    push    di
    push    ds
    push    es
    push    fs
    mov     bx,[_wCount]
    mov     cx,bx
    and     bx,LOOP_NEAREST_16_LENGTH-1
    shr     cx,LOOP_NEAREST_16_SHIFT
    inc     cx
    neg     bx
    add     bx,LOOP_NEAREST_16_LENGTH
    cmp     bx,LOOP_NEAREST_16_LENGTH
    jne     _play_sample_nearest_16@_no0
    dec     cx
    xor     bx,bx
_play_sample_nearest_16@_no0:
    mov     [_count],cx
    shl     bx,1
    les     di,[_dOutBuf]
    sub     di,bx
    mov     bx,ds:[table_nearest_16][bx]
    push    bx
    lfs     bx,[_dSmpInfo]
    xor     esi,esi
    lds     si,dword ptr fs:[bx][play_sample_info_t.dData]
    shr     si,1    ; 'esi' is in 16 bits units
    mov     cx,word ptr fs:[bx][play_sample_info_t.dPos]
    add     si,word ptr fs:[bx][play_sample_info_t.dPos+2]
    mov     dx,word ptr fs:[bx][play_sample_info_t.dStep+2]
    mov     bx,word ptr fs:[bx][play_sample_info_t.dStep]
; word at ss:[sp] = jump target
; ax = new sample value
; bx = step.frac
; cx = pos.frac
; dx = step.int
; si = pos.int
; ds = sample data DOS segment
; es:di = output buffer
    retn    ; jump

align 2

loop_nearest_16 macro no
loop_nearest_16_&no:
    mov     ax,ds:[esi+esi]
    add     cx,bx
    adc     si,dx
    mov     es:[di+counter*2],ax
endm

counter = 0
rept LOOP_NEAREST_16_LENGTH
    loop_nearest_16 %counter
    counter = counter + 1
endm
    add     di,LOOP_NEAREST_16_LENGTH*2
    dec     [_count]
    jnz     loop_nearest_16_0

    mov     bx,word ptr [_dSmpInfo]
    shl     si,1
    sub     si,word ptr fs:[bx][play_sample_info_t.dData]
    shr     si,1
    jmp     _play_sample_nearest_8_exit
_play_sample_nearest_16 endp

SAMPLER_TEXT ends

end
