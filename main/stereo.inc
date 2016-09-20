_stereo_tick0:    cmp   [patterndelay],0
                 je    _nodelay     ; or pattern delay done ...
                 dec   [patterndelay]
                 jz    _nodelay
                 dec   [curline]
_nodelay:        setborder 4
                 call near ptr [READNEWNOTES]
                 setborder 1
                 jmp  _continuecalc

_anewtick:     mov           ax,[BPT]
               mov           [TickBytesLeft],ax
               mov           [calleffects],1
               cmp           [curtick],1
               jbe           _stereo_tick0
               dec           [curtick]
               jmp           _continuecalc

calc_stereo_tick PROC NEAR
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
               jz            _anewtick
_continuecalc:
               mov           ax,word ptr [offset tickbuffer +2]
               mov           es,ax 
               cmp           [EndOfSong],1
               je            _afterall
               mov           al,[usedchannels]
               mov           [curchannel],al
               ; number of ticks we calc for every tick :
               mov           ax,[TickBytesLeft]
               shl           ax,1
               mov           cx,[DMArealBufsize+2]
               sub           cx,[nextPosition]
               cmp           cx,ax
               jbe           _cantfinishtick
               mov           cx,ax     ; finish that Tick and loop to fill the whole tickbuffer
_cantfinishtick:
               mov           [sample2calc],cx
               cmp           cx,0
               je            _afterall

               xor           bp,bp
_chnLoop:      cmp           ds:[channel.channeltyp+bp],0
               je            _nextchannel
               cmp           ds:[channel.channeltyp+bp],2
               ja            _nextchannel
               cmp           [calleffects],0
               je            _noeff
               ; do effects for this channel :

               mov           al,[curtick]
               cmp           al,[curspeed]
               je            _noeff_forfirst
_doeff:        mov           bx,ds:[channel.command+bp]
               cmp           bx,2*255
               je            _noeff
               call          [effects + bx]
_noeff:
_noeff_forfirst:
               ; check if mixing :
               cmp           ds:[channel.enabled+bp],0
               je            _nextchannel

               mov           ax,ds:[channel.SampleSEG+bp]

               ; well now check if in EMS :
               cmp           ax,0f000h
               jb            _noEMSsample

               mov           edi,ds:[channel.sCurpos+bp]   ; load it for EMS optim.
               rol           edi,16

               mov           si,ds:[channel.sLoopend+bp]

               and           ax,0fffh
               mov           bx,ax
               mov           ax,04400h
               mov           dx,[smpEMShandle]  ; dx = handle
               ; EMS access optimization (switch on only pages we really need !)
EMSoptim2:     cmp           di,16*1024
               jb            EMSoptim1
               setborder 9
               sub           di,16*1024
               sub           si,16*1024
               inc           bx
               inc           al                         ; don't need this page !
               jmp           EMSoptim2
EMSoptim1:     mov           cx,1
               cmp           di,16*1024-2*1024
               jb            _onemorepage
               cmp           si,16*1026
               jbe           _onemorepage
               inc           cx                         ; need two pages ...
_onemorepage:  push          bp bx ax dx cx
               ; Set page number:
               int           67h
               cmp           ah,0
               je            _noEMSprob
               mov           dl,0
               div           dl         ; <- cause a "div by 0", if EMSdriver does not work correct
_noEMSprob:    pop           cx dx ax bx bp
               inc           al
               inc           bx
               dec           cx
               jnz           _onemorepage
               mov           ax,[frameseg]
               setborder 1
_noEMSsample:  mov           gs,ax

               lfs           si,[volumetableptr]

               mov           edi,ds:[channel.sCurpos+bp]   ; need to load this before EMS handling ...
               rol           edi,16

               mov           edx,ds:[channel.sStep+bp]
               rol           edx,16

               xor           ebx,ebx
               mov           bh,ds:[channel.SampleVol+bp]

               mov           si,[nextPosition]
               shl           si,1
               mov           cx,[sample2calc]
               shr           cx,1               ; stereo :)

               ; first check for correct position inside sample
               ; oh well - now stereo position
               cmp           ds:[channel.channeltyp+bp],1
               je            _leftside
               add           si,2
_leftside:

        call    MixSampleStereo8

_aftercalc:    cmp           di,ds:[channel.sLoopend+bp]
               jae           _sampleends
_back2main:    rol           edi,16
               mov           dword ptr ds:[channel.sCurpos+bp],edi

_nextchannel:  add           bp,size channel
               dec           [curchannel]
               jnz           _chnLoop

               mov           ax,[sample2calc]
               add           [nextPosition],ax
               shr           ax,1
               sub           [TickBytesLeft],ax
               mov           ax,[DMArealBufsize+2]
               cmp           [nextPosition],ax
               jb            _anewtick

_afterall:     pop           bp
               ret

_sampleends:   cmp           ds:[channel.sloopflag+bp],0
               je            _no_loopflag
_tryagain:     sub           di,ds:[channel.sloopEnd+bp]
               add           di,ds:[channel.sloopstart+bp]
               cmp           di,ds:[channel.sloopEnd+bp]
               jae           _tryagain
               jmp           _back2main
_no_loopflag:  mov           ds:[channel.enabled+bp],0
               jmp           _back2main

calc_stereo_tick ENDP
