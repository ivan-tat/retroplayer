; mixer_.asm -- mixer functions.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large,pascal
.386

DGROUP group _DATA

include mixer_t.def

_DATA segment word public use16 'DATA'

dwofs macro name, no
        dw      offset &name&no
endm

LOOP_MONO_LENGTH equ 16
LOOP_MONO_SHIFT equ 4

; mono innerloop table:
counter = 0
innerloop_mono_tab label word
rept LOOP_MONO_LENGTH
        dwofs   innerloop_mono, %counter
        counter = counter + 1
endm

LOOP_STEREO_LENGTH equ 16
LOOP_STEREO_SHIFT equ 4

; stereo innerloop table:
counter = 0
innerloop_stereo_tab label word
rept LOOP_STEREO_LENGTH
        dwofs   innerloop_stereo, %counter
        counter = counter + 1
endm

_DATA ends

MIXER__TEXT segment word public use16 'CODE'
assume cs:MIXER__TEXT,ds:DGROUP,ss:DGROUP

public _MixSampleMono8
_MixSampleMono8 proc far _dOutBuf: dword, _dSmpInfo: dword, _wVolTabSeg: word, _bVol: byte, _wCount: word
local _count: word
local _step: dword
        push    si
        push    di
        push    ds
        push    es
        push    fs
        push    gs
        mov     ax,[_wCount]
        mov     cx,ax
        and     ax,LOOP_MONO_LENGTH-1
        shr     cx,LOOP_MONO_SHIFT
        inc     cx
        neg     ax
        add     ax,LOOP_MONO_LENGTH
        cmp     ax,LOOP_MONO_LENGTH
        jne     _MixSampleMono8@_no0
        dec     cx
        xor     ax,ax
_MixSampleMono8@_no0:
        mov     [_count],cx
        shl     ax,1
        les     di,[_dOutBuf]
        sub     di,ax
        ;sub     si,ax
        mov     bx,ax
        mov     ax,ds:[innerloop_mono_tab+bx]
        lgs     bx,[_dSmpInfo]
        mov     ds,word ptr gs:[bx][TPlaySampleInfo.dData+2]
        mov     esi,gs:[bx][TPlaySampleInfo.dPos]
        rol     esi,16
        add     si,word ptr gs:[bx][TPlaySampleInfo.dData]
        mov     edx,gs:[bx][TPlaySampleInfo.dStep]
        rol     edx,16
        xor     ebx,ebx
        mov     bh,[_bVol]
        mov     fs,[_wVolTabSeg]
        jmp     ax

align 2

innerloop_mono macro no
innerloop_mono&no:
        mov     bl,ds:[si]      ; get PCM value
        add     esi,[_step]     ; next sample pos
        adc     si,0            ; next sample pos (carry flag into account)
        mov     ax,fs:[ebx+ebx] ; convert PCM value with volumetable
        add     es:[di+pos],ax  ; mix result to out buffer
endm

z = 0
pos = 0
rept LOOP_MONO_LENGTH
    innerloop_mono %z
    z = z + 1
    pos = pos + 2
endm
        add     di,LOOP_MONO_LENGTH*2
        dec     [_count]
        jnz     innerloop_mono0

        mov     bx,word ptr [_dSmpInfo]
        sub     si,word ptr gs:[bx][TPlaySampleInfo.dData]
        rol     esi,16
        mov     gs:[bx][TPlaySampleInfo.dPos],esi

        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     di
        pop     si
        ret
_MixSampleMono8 endp

public _MixSampleStereo8
_MixSampleStereo8 proc far _dOutBuf: dword, _dSmpInfo: dword, _wVolTabSeg: word, _bVol: byte, _wCount: word
local _count: word
local _step: dword
        push    si
        push    di
        push    ds
        push    es
        push    fs
        push    gs
        mov     ax,[_wCount]
        mov     cx,ax
        and     ax,LOOP_STEREO_LENGTH-1
        shr     cx,LOOP_STEREO_SHIFT
        inc     cx
        neg     ax
        add     ax,LOOP_STEREO_LENGTH
        cmp     ax,LOOP_STEREO_LENGTH
        jne     _MixSampleStereo8@_no0
        dec     cx
        xor     ax,ax
_MixSampleStereo8@_no0:
        mov     [_count],cx
        shl     ax,1
        les     di,[_dOutBuf]
        sub     di,ax
        sub     di,ax
        mov     bx,ax
        mov     ax,ds:[innerloop_stereo_tab+bx]
        lgs     bx,[_dSmpInfo]
        mov     ds,word ptr gs:[bx][TPlaySampleInfo.dData+2]
        mov     esi,gs:[bx][TPlaySampleInfo.dPos]
        rol     esi,16
        add     si,word ptr gs:[bx][TPlaySampleInfo.dData]
        mov     edx,gs:[bx][TPlaySampleInfo.dStep]
        rol     edx,16
        mov     [_step],edx
        xor     ebx,ebx
        mov     bh,[_bVol]
        mov     fs,[_wVolTabSeg]
        jmp     ax

align 2

innerloop_stereo macro no
innerloop_stereo&no:
        mov     bl,ds:[si]      ; get PCM value
        add     esi,[_step]     ; next sample pos
        adc     si,0            ; next sample pos (carry flag into account)
        mov     ax,fs:[ebx+ebx] ; convert PCM value with volumetable
        add     es:[di+pos],ax  ; mix result to out buffer
endm

z = 0
pos = 0
rept LOOP_STEREO_LENGTH
    innerloop_stereo %z
    z = z + 1
    pos = pos + 4
endm
        add     di,LOOP_STEREO_LENGTH*4
        dec     [_count]
        jnz     innerloop_stereo0

        mov     bx,word ptr [_dSmpInfo]
        sub     si,word ptr gs:[bx][TPlaySampleInfo.dData]
        rol     esi,16
        mov     gs:[bx][TPlaySampleInfo.dPos],esi

        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     di
        pop     si
        ret
_MixSampleStereo8 endp

MIXER__TEXT ends

end
