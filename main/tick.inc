public calc_tick
calc_tick proc near
        push    bp
        ; first fill tickbuffer with ZERO = 2048+offset post8bit
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

        xor     bp,bp

_chnLoop:
        cmp     ds:[channel.channeltyp+bp],0
        je      _nextchannel

        cmp     ds:[channel.channeltyp+bp],2
        ja      _nextchannel

        cmp     [calleffects],0
        je      _noeff

        ; do effects for this channel :
        mov     al,[curtick]
        cmp     al,[curspeed]
        je      _noeff_forfirst

_doeff:
        mov     bx,ds:[channel.command+bp]
        cmp     bx,255*2
        je      _noeff

        call    [effects + bx]

_noeff:
_noeff_forfirst:
        ; check if mixing :
        cmp     ds:[channel.enabled+bp],0
        je      _nextchannel

        ; well now check if in EMS :
        mov     ax,ds:[channel.SampleSEG+bp]
        cmp     ax,0f000h
        jb      _noEMSsample

        and     ax,0fffh
        xor     bx,bx
        mov     cx,ds:[channel.sLoopend+bp]

_onemorepage:
        push    ax
        push    bx
        push    cx
        push    bp
comment #
; somehow this does not work properly...
        push    word ptr [smpEMShandle] ; handle
        push    ax  ; logical page
        push    bx  ; physical page
        call    EmsMap
        test    al,al
        jnz     _noemsprob
#
;comment #
        xchg    bx,ax
        mov     ah,044h
        mov     dx,[smpEMShandle]
        int     67h
        test    ah,ah
        jz      _noemsprob
;#
        ; cause an exception "division by zero" because EMS driver does not work correct
        xor     dl,dl
        div     dl

_noemsprob:
        pop     bp
        pop     cx
        pop     bx
        pop     ax
        inc     ax
        inc     bl
        sub     cx,16*1024
        jnc     _onemorepage

        mov     ax,[frameseg]

_noEMSsample:
        mov     gs,ax

        lfs     si,[volumetableptr]

        xor     ebx,ebx
        mov     bh,ds:[channel.SampleVol+bp]

        mov     si,[nextPosition]
        shl     si,1
        cmp     [stereo],0
        je      _skip_2

        ; oh well - now stereo position
        cmp     ds:[channel.channeltyp+bp],1
        je      _leftside

        add     si,2

_leftside:
_skip_2:
        mov     cx,[sample2calc]
        cmp     [stereo],0
        je      _skip_5

        shr     cx,1    ; only in stereo

_skip_5:
        mov     edi,ds:[channel.sCurpos+bp]
        rol     edi,16

        mov     edx,ds:[channel.sStep+bp]
        rol     edx,16

        ; first check for correct position inside sample
        cmp     di,ds:[channel.sLoopend+bp]
        jae     _sampleends

        cmp     [stereo],0
        je      _skip_3

        call    MixSampleStereo8
        jmp     _skip_4

_skip_3:
        call    MixSampleMono8

_skip_4:
_aftercalc:
        cmp     di,ds:[channel.sLoopend+bp]
        jae     _sampleends

_back2main:
        rol     edi,16
        mov     ds:[channel.sCurpos+bp],edi

_nextchannel:
        add     bp,size TChannel
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
        pop     bp
        ret

_sampleends:
        cmp     ds:[channel.sloopflag+bp],0
        je      _no_loopflag

_tryagain:
        sub     di,ds:[channel.sloopEnd+bp]
        add     di,ds:[channel.sloopstart+bp]
        cmp     di,ds:[channel.sloopEnd+bp]
        jae     _tryagain

        jmp     _back2main

_no_loopflag:
        mov     ds:[channel.enabled+bp],0
        jmp     _back2main

calc_tick endp
