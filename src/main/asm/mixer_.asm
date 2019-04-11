; mixer_.asm -- mixer functions.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large,pascal
.386

DGROUP group _DATA

include mixer_.def

_DATA segment word public use16 'DATA'

dwofs macro name, no
    dw offset &name&no
endm

; Mix played sample to output buffer

LOOP_1_SHIFT equ 3
LOOP_1_LENGTH equ (1 shl LOOP_1_SHIFT)

LOOP_2_SHIFT equ 3
LOOP_2_LENGTH equ (1 shl LOOP_2_SHIFT)

counter = 0
table_1 label word
rept LOOP_1_LENGTH
    dwofs loop_1_, %counter
    counter = counter + 1
endm

counter = 0
table_2 label word
rept LOOP_2_LENGTH
    dwofs loop_2_, %counter
    counter = counter + 1
endm

; Combined 'Play & Mix' methods

; mono loop jump table:

LOOP_MIX_8_MONO_SHIFT equ 3
LOOP_MIX_8_MONO_LENGTH equ (1 shl LOOP_MIX_8_MONO_SHIFT)

LOOP_MIX_16_MONO_SHIFT equ 3
LOOP_MIX_16_MONO_LENGTH equ (1 shl LOOP_MIX_16_MONO_SHIFT)

LOOP_MIX_8_STEREO_SHIFT equ 3
LOOP_MIX_8_STEREO_LENGTH equ (1 shl LOOP_MIX_8_STEREO_SHIFT)

LOOP_MIX_16_STEREO_SHIFT equ 3
LOOP_MIX_16_STEREO_LENGTH equ (1 shl LOOP_MIX_16_STEREO_SHIFT)

counter = 0
table_mix_8_mono label word
rept LOOP_MIX_8_MONO_LENGTH
    dwofs loop_mix_8_mono_, %counter
    counter = counter + 1
endm

counter = 0
table_mix_16_mono label word
rept LOOP_MIX_16_MONO_LENGTH
    dwofs loop_mix_16_mono_, %counter
    counter = counter + 1
endm

counter = 0
table_mix_8_stereo label word
rept LOOP_MIX_8_STEREO_LENGTH
    dwofs loop_mix_8_stereo_, %counter
    counter = counter + 1
endm

counter = 0
table_mix_16_stereo label word
rept LOOP_MIX_16_STEREO_LENGTH
    dwofs loop_mix_16_stereo_, %counter
    counter = counter + 1
endm

_DATA ends

MIXER__TEXT segment word public use16 'CODE'
assume cs:MIXER__TEXT,ds:DGROUP,ss:DGROUP

public _mix_sample
_mix_sample proc far _dOutBuf: dword, _dSmpBuf: dword, _wVolTabSeg: word, _bVol: byte, _wCount: word
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
    and     bx,LOOP_1_LENGTH-1
    shr     cx,LOOP_1_SHIFT
    inc     cx
    neg     bx
    add     bx,LOOP_1_LENGTH
    cmp     bx,LOOP_1_LENGTH
    jne     _mix_sample@_no0
    dec     cx
    xor     bx,bx
_mix_sample@_no0:
    shl     bx,1    ; HINT: 'bx' = n * 2
    mov     si,word ptr [_dSmpBuf]
    les     di,dword ptr [_dOutBuf]
    mov     ax,word ptr ds:[table_1][bx]
    push    ax
    sub     si,bx   ; NOTE: smpbuf is 1 channel @ 16 bits
    shl     bx,1    ; HINT: 'bx' = n * 4
    sub     di,bx   ; NOTE: mixbuf is 1 channel @ 32 bits
    xor     ebx,ebx
    mov     ds,word ptr [_dSmpBuf+2]
    mov     fs,word ptr [_wVolTabSeg]
    mov     bh,byte ptr [_bVol]
    mov     edx,8000h
; word at ss:[sp] = jump target
; cx = loops count
; ebx = ((volume & 0x3f) << 8) + (x & 0xff)
; edx = 0x8000
; ds:si = sample buffer
; es:di = output buffer
; fs:0 = volume table
    retn    ; jump

align 2

loop_1 macro no
loop_1_&no:
    mov     bl,byte ptr ds:[si][counter*2+1]
    movsx   eax,word ptr fs:[ebx+ebx]
    mov     bl,byte ptr ds:[si][counter*2]
    add     ax,word ptr fs:[edx+ebx*2]
    add     dword ptr es:[di][counter*4],eax
endm

counter = 0
rept LOOP_1_LENGTH
    loop_1 %counter
    counter = counter + 1
endm
    add     si,LOOP_1_LENGTH*2
    add     di,LOOP_1_LENGTH*4
    dec     cx
    jnz     loop_1_0

_mix_sample_exit:
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
_mix_sample endp

public _mix_sample2
_mix_sample2 proc far _dOutBuf: dword, _dSmpBuf: dword, _wVolTabSeg: word, _bVol: byte, _wCount: word
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
    and     bx,LOOP_2_LENGTH-1
    shr     cx,LOOP_2_SHIFT
    inc     cx
    neg     bx
    add     bx,LOOP_2_LENGTH
    cmp     bx,LOOP_2_LENGTH
    jne     _mix_sample2@_no0
    dec     cx
    xor     bx,bx
_mix_sample2@_no0:
    shl     bx,1    ; HINT: 'bx' = n * 2
    mov     si,word ptr [_dSmpBuf]
    les     di,dword ptr [_dOutBuf]
    mov     ax,word ptr ds:[table_2][bx]
    push    ax
    sub     si,bx   ; NOTE: smpbuf is 1 channel @ 16 bits
    shl     bx,2    ; HINT: 'bx' = n * 8
    sub     di,bx   ; NOTE: mixbuf is 2 channels @ 32 bits
    xor     ebx,ebx
    mov     ds,word ptr [_dSmpBuf+2]
    mov     fs,word ptr [_wVolTabSeg]
    mov     bh,byte ptr [_bVol]
    mov     edx,8000h
; word at ss:[sp] = jump target
; cx = loops count
; ebx = ((volume & 0x3f) << 8) + (x & 0xff)
; edx = 0x8000
; ds:si = sample buffer
; es:di = output buffer
; fs:0 = volume table
    retn    ; jump

align 2

loop_2 macro no
loop_2_&no:
    mov     bl,byte ptr ds:[si][counter*2+1]
    movsx   eax,word ptr fs:[ebx+ebx]
    mov     bl,byte ptr ds:[si][counter*2]
    add     ax,word ptr fs:[edx+ebx*2]
    add     dword ptr es:[di][counter*8],eax
endm

counter = 0
rept LOOP_2_LENGTH
    loop_2 %counter
    counter = counter + 1
endm

    add     si,LOOP_2_LENGTH*2
    add     di,LOOP_2_LENGTH*8
    dec     cx
    jnz     loop_2_0

    jmp     _mix_sample_exit
_mix_sample2 endp

public _MixSampleMono8
_MixSampleMono8 proc far _dOutBuf: dword, _dSmpInfo: dword, _wVolTabSeg: word, _bVol: byte, _wCount: word
local _count: word
local _step_frac: word
    push    ax
    push    bx
    push    cx
    push    dx
    push    si
    push    di
    push    ds
    push    es
    push    fs
    push    gs
    mov     bx,[_wCount]
    mov     cx,bx
    and     bx,LOOP_MIX_8_MONO_LENGTH-1
    shr     cx,LOOP_MIX_8_MONO_SHIFT
    inc     cx
    neg     bx
    add     bx,LOOP_MIX_8_MONO_LENGTH
    cmp     bx,LOOP_MIX_8_MONO_LENGTH
    jne     _MixSampleMono8@_no0
    dec     cx
    xor     bx,bx
_MixSampleMono8@_no0:
    mov     [_count],cx
    shl     bx,1    ; HINT: 'bx' = n * 2
    mov     ax,ds:[table_mix_8_mono][bx]
    push    ax
    les     di,[_dOutBuf]
    shl     bx,1    ; HINT: 'bx' = n * 4
    sub     di,bx   ; NOTE: mixbuf is 1 channel @ 32 bits
    lgs     bx,[_dSmpInfo]
    lds     si,dword ptr gs:[bx][TPlaySampleInfo.dData]
    mov     cx,word ptr gs:[bx][TPlaySampleInfo.dPos]
    add     si,word ptr gs:[bx][TPlaySampleInfo.dPos+2]
    mov     ax,word ptr gs:[bx][TPlaySampleInfo.dStep]
    mov     dx,word ptr gs:[bx][TPlaySampleInfo.dStep+2]
    mov     [_step_frac],ax
    xor     ebx,ebx
    mov     bh,[_bVol]
    mov     fs,[_wVolTabSeg]
; word at ss:[sp] = jump target
; word at ss:[_step_frac] = step.frac
; ebx = (volume & 0x3f) << 8
; cx = pos.frac
; dx = step.int
; si = pos.int
; ds = sample data DOS segment
; es:di = output buffer
; fs:0 = volume table
    retn    ; jump

align 2

loop_mix_8_mono macro no
loop_mix_8_mono_&no:
    mov     bl,ds:[si]
    add     cx,[_step_frac]
    adc     si,dx
    movsx   eax,word ptr fs:[ebx+ebx]
    add     dword ptr es:[di+counter*4],eax
endm

counter = 0
rept LOOP_MIX_8_MONO_LENGTH
    loop_mix_8_mono %counter
    counter = counter + 1
endm

    add     di,LOOP_MIX_8_MONO_LENGTH*4   ; NOTE: mixbuf is 1 channel @ 32 bits
    dec     [_count]
    jnz     loop_mix_8_mono_0

    mov     bx,word ptr [_dSmpInfo]
    sub     si,word ptr gs:[bx][TPlaySampleInfo.dData]

_MixSample_exit:
    mov     word ptr gs:[bx][TPlaySampleInfo.dPos],cx
    mov     word ptr gs:[bx][TPlaySampleInfo.dPos+2],si

    pop     gs
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
_MixSampleMono8 endp

public _MixSampleMono16
_MixSampleMono16 proc far _dOutBuf: dword, _dSmpInfo: dword, _wVolTabSeg: word, _bVol: byte, _wCount: word
local _count: word
local _step_frac: word
local _step_int: word
    push    ax
    push    bx
    push    cx
    push    dx
    push    si
    push    di
    push    ds
    push    es
    push    fs
    push    gs
    mov     bx,[_wCount]
    mov     cx,bx
    and     bx,LOOP_MIX_8_MONO_LENGTH-1
    shr     cx,LOOP_MIX_8_MONO_SHIFT
    inc     cx
    neg     bx
    add     bx,LOOP_MIX_8_MONO_LENGTH
    cmp     bx,LOOP_MIX_8_MONO_LENGTH
    jne     _MixSampleMono16@_no0
    dec     cx
    xor     bx,bx
_MixSampleMono16@_no0:
    mov     [_count],cx
    shl     bx,1    ; HINT: 'bx' = n * 2
    mov     ax,ds:[table_mix_16_mono][bx]
    push    ax
    les     di,[_dOutBuf]
    shl     bx,1    ; HINT: 'bx' = n * 4
    sub     di,bx   ; NOTE: mixbuf is 1 channel @ 32 bits
    lgs     bx,[_dSmpInfo]
    xor     esi,esi
    lds     si,dword ptr gs:[bx][TPlaySampleInfo.dData]
    mov     cx,word ptr gs:[bx][TPlaySampleInfo.dPos]
    shr     si,1
    add     si,word ptr gs:[bx][TPlaySampleInfo.dPos+2]
    mov     ax,word ptr gs:[bx][TPlaySampleInfo.dStep]
    mov     dx,word ptr gs:[bx][TPlaySampleInfo.dStep+2]
    mov     [_step_frac],ax
    mov     [_step_int],dx
    mov     edx,8000h
    xor     ebx,ebx
    mov     bh,[_bVol]
    mov     fs,[_wVolTabSeg]
; word at ss:[sp] = jump target
; word at ss:[_step_frac] = step.frac
; word at ss:[_step_int] = step.int
; ebx = (volume & 0x3f) << 8
; cx = pos.frac
; edx = 0x8000
; esi = pos.int & 0x7fff
; ds = sample data DOS segment
; es:di = output buffer
; fs:0 = volume table
    retn    ; jump

align 2

loop_mix_16_mono macro no
loop_mix_16_mono_&no:
    mov     bl,ds:[esi+esi+1]
    movsx   eax,word ptr fs:[ebx+ebx]
    mov     bl,ds:[esi+esi]
    add     ax,word ptr fs:[edx+ebx*2]
    add     cx,[_step_frac]
    adc     si,[_step_int]
    add     dword ptr es:[di+counter*4],eax
endm

counter = 0
rept LOOP_MIX_16_MONO_LENGTH
    loop_mix_16_mono %counter
    counter = counter + 1
endm

    add     di,LOOP_MIX_16_MONO_LENGTH*4   ; NOTE: mixbuf is 1 channel @ 32 bits
    dec     [_count]
    jnz     loop_mix_16_mono_0

    mov     bx,word ptr [_dSmpInfo]
    shl     si,1
    sub     si,word ptr gs:[bx][TPlaySampleInfo.dData]
    shr     si,1
    jmp     _MixSample_exit
_MixSampleMono16 endp

public _MixSampleStereo8
_MixSampleStereo8 proc far _dOutBuf: dword, _dSmpInfo: dword, _wVolTabSeg: word, _bVol: byte, _wCount: word
local _count: word
local _step_frac: word
    push    ax
    push    bx
    push    cx
    push    dx
    push    si
    push    di
    push    ds
    push    es
    push    fs
    push    gs
    mov     bx,[_wCount]
    mov     cx,bx
    and     bx,LOOP_MIX_8_STEREO_LENGTH-1
    shr     cx,LOOP_MIX_8_STEREO_SHIFT
    inc     cx
    neg     bx
    add     bx,LOOP_MIX_8_STEREO_LENGTH
    cmp     bx,LOOP_MIX_8_STEREO_LENGTH
    jne     _MixSampleStereo8@_no0
    dec     cx
    xor     bx,bx
_MixSampleStereo8@_no0:
    mov     [_count],cx
    shl     bx,1    ; HINT: 'bx' = n * 2
    mov     ax,ds:[table_mix_8_stereo][bx]
    push    ax
    les     di,[_dOutBuf]
    shl     bx,2    ; HINT: 'bx' = n * 8
    sub     di,bx   ; NOTE: mixbuf is 2 channels @ 32 bits
    lgs     bx,[_dSmpInfo]
    lds     si,dword ptr gs:[bx][TPlaySampleInfo.dData]
    mov     cx,word ptr gs:[bx][TPlaySampleInfo.dPos]
    add     si,word ptr gs:[bx][TPlaySampleInfo.dPos+2]
    mov     ax,word ptr gs:[bx][TPlaySampleInfo.dStep]
    mov     dx,word ptr gs:[bx][TPlaySampleInfo.dStep+2]
    mov     [_step_frac],ax
    xor     ebx,ebx
    mov     bh,[_bVol]
    mov     fs,[_wVolTabSeg]
; word at ss:[sp] = jump target
; word at ss:[_step_frac] = step.frac
; ebx = (volume & 0x3f) << 8
; cx = pos.frac
; dx = step.int
; si = pos.int
; ds = sample data DOS segment
; es:di = output buffer
; fs:0 = volume table
    retn    ; jump

align 2

loop_mix_8_stereo macro no
loop_mix_8_stereo_&no:
    mov     bl,ds:[si]
    add     cx,[_step_frac]
    adc     si,dx
    movsx   eax,word ptr fs:[ebx+ebx]
    add     es:[di][counter*8],eax
endm

counter = 0
rept LOOP_MIX_8_STEREO_LENGTH
    loop_mix_8_stereo %counter
    counter = counter + 1
endm

    add     di,LOOP_MIX_8_STEREO_LENGTH*8 ; NOTE: mixbuf is 2 channels @ 32 bits
    dec     [_count]
    jnz     loop_mix_8_stereo_0

    mov     bx,word ptr [_dSmpInfo]
    sub     si,word ptr gs:[bx][TPlaySampleInfo.dData]
    jmp     _MixSample_exit
_MixSampleStereo8 endp

public _MixSampleStereo16
_MixSampleStereo16 proc far _dOutBuf: dword, _dSmpInfo: dword, _wVolTabSeg: word, _bVol: byte, _wCount: word
local _count: word
local _step_frac: word
local _step_int: word
    push    ax
    push    bx
    push    cx
    push    dx
    push    si
    push    di
    push    ds
    push    es
    push    fs
    push    gs
    mov     bx,[_wCount]
    mov     cx,bx
    and     bx,LOOP_MIX_16_STEREO_LENGTH-1
    shr     cx,LOOP_MIX_16_STEREO_SHIFT
    inc     cx
    neg     bx
    add     bx,LOOP_MIX_16_STEREO_LENGTH
    cmp     bx,LOOP_MIX_16_STEREO_LENGTH
    jne     _MixSampleStereo16@_no0
    dec     cx
    xor     bx,bx
_MixSampleStereo16@_no0:
    mov     [_count],cx
    shl     bx,1    ; HINT: 'bx' = n * 2
    mov     ax,ds:[table_mix_16_stereo][bx]
    push    ax
    les     di,[_dOutBuf]
    shl     bx,2    ; HINT: 'bx' = n * 8
    sub     di,bx   ; NOTE: mixbuf is 2 channels @ 32 bits
    lgs     bx,[_dSmpInfo]
    lds     si,dword ptr gs:[bx][TPlaySampleInfo.dData]
    mov     cx,word ptr gs:[bx][TPlaySampleInfo.dPos]
    shr     si,1
    add     si,word ptr gs:[bx][TPlaySampleInfo.dPos+2]
    mov     ax,word ptr gs:[bx][TPlaySampleInfo.dStep]
    mov     dx,word ptr gs:[bx][TPlaySampleInfo.dStep+2]
    mov     [_step_frac],ax
    mov     [_step_int],dx
    mov     edx,8000h
    xor     ebx,ebx
    mov     bh,[_bVol]
    mov     fs,[_wVolTabSeg]
; word at ss:[sp] = jump target
; word at ss:[_step_frac] = step.frac
; word at ss:[_step_int] = step.int
; ebx = (volume & 0x3f) << 8
; cx = pos.frac
; edx = 0x8000
; esi = pos.int & 0x7fff
; ds = sample data DOS segment
; es:di = output buffer
; fs:0 = volume table
    retn    ; jump

align 2

loop_mix_16_stereo macro no
loop_mix_16_stereo_&no:
    mov     bl,ds:[esi+esi+1]
    movsx   eax,word ptr fs:[ebx+ebx]
    mov     bl,ds:[esi+esi]
    add     ax,word ptr fs:[edx+ebx*2]
    add     cx,[_step_frac]
    adc     si,[_step_int]
    add     es:[di][counter*8],eax
endm

counter = 0
rept LOOP_MIX_16_STEREO_LENGTH
    loop_mix_16_stereo %counter
    counter = counter + 1
endm

    add     di,LOOP_MIX_16_STEREO_LENGTH*8 ; NOTE: mixbuf is 2 channels @ 32 bits
    dec     [_count]
    jnz     loop_mix_16_stereo_0

    mov     bx,word ptr [_dSmpInfo]
    shl     si,1
    sub     si,word ptr gs:[bx][TPlaySampleInfo.dData]
    shr     si,1
    jmp     _MixSample_exit
_MixSampleStereo16 endp


MIXER__TEXT ends

end
