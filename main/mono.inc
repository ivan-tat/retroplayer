mono_tick0:    cmp   [patterndelay],0
               je    nodelay     ; or pattern delay done ...
               dec   [patterndelay]
               jz    nodelay
               dec   [curline]
nodelay:       setborder 4
               call near ptr [READNEWNOTES]
               setborder 1
               jmp  continuecalc

aNewtick:      mov           ax,[BPT]
               mov           [TickBytesLeft],ax
               mov           [calleffects],1
               cmp           [curtick],1
               jbe           mono_tick0
               dec           [curtick]
               jmp           continuecalc

calc_mono_tick PROC NEAR
               push          bp
               ; first fill tickbuffer with ZERO = 2048+offset post8bit
               ; for 16bit play then ofcourse a bit different value ...
               ; just only for 8bit play mode
               mov           ax,word ptr [offset tickbuffer +2]
               mov           es,ax
               mov           ax,2048
               xor           di,di
               mov           cx,[DMArealBufsize+2]
               setborder     3
               rep stosw
               setborder     1
               mov           [nextPosition],0
               mov           [calleffects],0
               cmp           [TickBytesLeft],0
               jz            aNewTick
continuecalc:
               mov           ax,word ptr [offset tickbuffer +2]
               mov           es,ax 
               cmp           [EndOfSong],1
               je            afterall
               mov           al,[usedchannels]
               mov           [curchannel],al
               
               ; number of ticks we calc for every tick :
               mov           ax,[TickBytesLeft]
               mov           cx,[DMArealBufsize+2]
               sub           cx,[nextPosition]
               cmp           cx,ax
               jbe           cantfinishTick
               mov           cx,ax     ; finish that Tick and loop to fill the whole tickbuffer
cantfinishTick:
               mov           [sample2calc],cx
               cmp           cx,0
               je            afterall

               xor           bp,bp
chnLoop:       cmp           ds:[channel.channeltyp+bp],0
               je            nextchannel
               cmp           ds:[channel.channeltyp+bp],2
               ja            nextchannel
               cmp           [calleffects],0
               je            noeff
               ; do effects for this channel :
               mov           al,[curtick]
               cmp           al,[curspeed]
               je            noeff_forfirst
doeff:         mov           bx,ds:[channel.command+bp]
               cmp           bx,255*2
               je            noeff
               call          [effects + bx]
noeff:
noeff_forfirst:
               ; check if mixing :
               cmp           ds:[channel.enabled+bp],0
               je            nextchannel

        ; well now check if in EMS :
        mov     ax,ds:[channel.SampleSEG+bp]
        cmp     ax,0f000h
        jb      noEMSsample

        and     ax,0fffh
        xor     bx,bx
        mov     cx,ds:[channel.sLoopend+bp]
onemorepage:
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
        jnz     noemsprob
#
;comment #
        xchg    bx,ax
        mov     ah,044h
        mov     dx,[smpEMShandle]
        int     67h
        test    ah,ah
        jz      noemsprob
;#
        ; cause an exception "division by zero" because EMS driver does not work correct
        xor     dl,dl
        div     dl
noemsprob:
        pop     bp
        pop     cx
        pop     bx
        pop     ax
        inc     ax
        inc     bl
        sub     cx,16*1024
        jnc     onemorepage
        mov     ax,[frameseg]

noEMSsample:
        mov     gs,ax

        lfs     si,[volumetableptr]

        xor     ebx,ebx
        mov     bh,ds:[channel.SampleVol+bp]

        mov     si,[nextPosition]
        shl     si,1
        mov     cx,[sample2calc]
        shr     cx,1

        mov     edi,ds:[channel.sCurpos+bp]
        rol     edi,16

        mov     edx,ds:[channel.sStep+bp]
        rol     edx,16

        cmp     di,ds:[channel.sLoopend+bp]
        jae     sampleends1
back2calc1:
        call    MixSampleMono8

aftercalc:     rol           edi,16
               mov           ds:[channel.sCurpos+bp],edi

nextchannel:   add           bp,size channel
               dec           [curchannel]
               jnz           chnLoop

               mov           ax,[sample2calc]
               sub           [TickBytesLeft],ax
               add           [nextPosition],ax
               mov           ax,[DMArealBufsize+2]
               cmp           [nextPosition],ax
               jb            aNewTick

afterall:      pop           bp
               ret

sampleends1:   cmp           ds:[channel.sloopflag+bp],0
               je            no_loopflag1
tryagain1:     sub           di,ds:[channel.sloopEnd+bp]
               add           di,ds:[channel.sloopstart+bp]
               cmp           di,ds:[channel.sloopEnd+bp]
               jae           tryagain1
               jmp           back2calc1
no_loopflag1:  mov           ds:[channel.enabled+bp],0
               jmp           aftercalc

calc_mono_tick ENDP
