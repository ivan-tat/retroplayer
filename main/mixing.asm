; mixing.asm -- mixer functions.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large,pascal
.386

DGROUP group _DATA

include ..\dos\emstool.def
include ..\blaster\sbctl.def
include s3mtypes.def
include s3mvars.def
include effvars.def
include mixvars.def
include fillvars.def
include voltab.def
include effects.def
include mixer.def
include mixer_.def
include readnote.def

_DATA segment word public use16 'DATA'
_DATA ends

MIXING_TEXT segment word public use16 'CODE'
assume cs:MIXING_TEXT,ds:DGROUP

include border.inc

mapData proc far _wSeg: word, _wLen: word
local _wLogPage: word
local _wPhysPage: word
        push    si
        push    es  ; ES is lost after INT 67h
        ; well now check if in EMS :
        mov     ax,[_wSeg]
        cmp     ax,0f000h
        jb      mapData@_exit
        and     ax,00fffh
        mov     [_wLogPage],ax
        mov     [_wPhysPage],0
mapData@_nextPage:
        push    [smpEMShandle]  ; handle
        push    [_wLogPage]     ; logical page
        push    [_wPhysPage]    ; physical page
        call    EmsMap
        test    al,al
        jnz     mapData@_ok
        ; cause an exception "division by zero" because EMS driver does not work correct
        xor     dl,dl
        div     dl
mapData@_ok:
        inc     [_wLogPage]
        inc     [_wPhysPage]
        sub     [_wLen],16*1024
        jnc     mapData@_nextPage
        mov     ax,[frameseg]
mapData@_exit:
        pop     es
        pop     si
        ret
mapData endp

public calc_tick
calc_tick proc far
local nextPosition: word
local sample2calc:  word    ; in mono number of bytes/ in stereo number of words
local curchannel:   byte
local calleffects:  byte

        ; first fill tickbuffer with ZERO = 2048
        ; for 16bit play then ofcourse a bit different value ...
        ; just only for 8bit play mode
        mov     ax,word ptr [offset tickbuffer+2]
        mov     es,ax
        mov     ax,2048
        xor     di,di
        mov     cx,[DMArealBufsize+2]
        setborder     3
        rep stosw
        setborder     1
        mov     [nextPosition],0
        mov     [calleffects],0
        cmp     [TickBytesLeft],0
        jnz     _continuecalc

calc_tick_anewtick:
        mov     ax,[BPT]
        mov     [TickBytesLeft],ax
        mov     [calleffects],1
        cmp     [curtick],1
        ja      calc_tick_decrtick

        cmp     [patterndelay],0
        je      calc_tick_nodelay     ; or pattern delay done ...

        dec     [patterndelay]
        jz      calc_tick_nodelay

        dec     [curline]

calc_tick_nodelay:
        setborder 4
        call    readnewnotes
        setborder 1
        jmp     _continuecalc

calc_tick_decrtick:
        dec     [curtick]

_continuecalc:
        mov     ax,word ptr [offset tickbuffer+2]
        mov     es,ax
        cmp     [EndOfSong],1
        je      _afterall

        mov     al,[usedchannels]
        mov     [curchannel],al
        ; number of bytes (?) we calc for every tick:
        mov     ax,[TickBytesLeft]
        cmp     [stereo],0
        je      _skip_0

        shl     ax,1    ; only in stereo

_skip_0:
        mov     cx,[DMArealBufsize+2]
        sub     cx,[nextPosition]
        cmp     cx,ax
        jbe     _cantfinishtick

        mov     cx,ax     ; finish that Tick and loop to fill the whole tickbuffer

_cantfinishtick:
        mov     [sample2calc],cx
        cmp     cx,0
        je      _afterall

        lea     si,[Channel]

_chnLoop:
        cmp     byte ptr [SI][TChannel.bChannelType],0
        je      _nextchannel

        cmp     byte ptr [SI][TChannel.bChannelType],2
        ja      _nextchannel

        cmp     [calleffects],0
        je      _noeff

        ; do effects for this channel :
        mov     al,[curtick]
        cmp     al,[curspeed]
        je      _noeff_forfirst

_doeff:
        push    es  ; save
        push    ds  ; FP_SEG(channel_t *)
        push    si  ; FP_OFF(channel_t *)
        call    chn_effTick
        pop     es  ; restore
_noeff:
_noeff_forfirst:
        ; check if mixing :
        cmp     byte ptr [SI][TChannel.bEnabled],0
        je      _nextchannel

        push    [SI][TChannel.wSmpSeg]
        push    [SI][TChannel.wSmpLoopEnd]
        call    mapData
        mov     gs,ax

        lfs     ax,[volumetableptr]

        xor     ebx,ebx
        mov     bh,[SI][TChannel.bSmpVol]

        mov     ax,[nextPosition]
        shl     ax,1
        cmp     [stereo],0
        je      _skip_2

        ; oh well - now stereo position
        cmp     byte ptr [SI][TChannel.bChannelType],1
        je      _leftside

        add     ax,2

_leftside:
_skip_2:
        mov     cx,[sample2calc]
        cmp     [stereo],0
        je      _skip_5

        shr     cx,1    ; only in stereo

_skip_5:
        mov     edi,[SI][TChannel.dSmpPos]
        rol     edi,16

        mov     edx,[SI][TChannel.dSmpStep]
        rol     edx,16

        ; first check for correct position inside sample
        cmp     di,[SI][TChannel.wSmpLoopEnd]
        jae     _sampleends

        cmp     [stereo],0
        je      _skip_3

        push    si
        mov     si,ax
        call    _MixSampleStereo8
        pop     si
        jmp     _skip_4

_skip_3:
        push    si
        mov     si,ax
        call    _MixSampleMono8
        pop     si

_skip_4:
_aftercalc:
        cmp     di,[SI][TChannel.wSmpLoopEnd]
        jae     _sampleends

_back2main:
        rol     edi,16
        mov     [SI][TChannel.dSmpPos],edi

_nextchannel:
        add     si,size TChannel
        dec     [curchannel]
        jnz     _chnLoop

        mov     ax,[sample2calc]
        add     [nextPosition],ax
        cmp     [stereo],0
        je      _skip_1

        shr     ax,1    ; only in stereo

_skip_1:
        sub     [TickBytesLeft],ax
        mov     ax,[DMArealBufsize+2]
        cmp     [nextPosition],ax
        jb      calc_tick_anewtick

_afterall:
        ret

_sampleends:
        test    byte ptr [SI][TChannel.bSmpFlags],SMPFLAG_LOOP
        jz      _no_loopflag

_tryagain:
        sub     di,[SI][TChannel.wSmpLoopEnd]
        add     di,[SI][TChannel.wSmpLoopStart]
        cmp     di,[SI][TChannel.wSmpLoopEnd]
        jae     _tryagain

        jmp     _back2main

_no_loopflag:
        mov     byte ptr [SI][TChannel.bEnabled],0
        jmp     _back2main

calc_tick endp

MIXING_TEXT ends

end
