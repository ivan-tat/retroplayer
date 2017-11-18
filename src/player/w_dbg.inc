(* Debug window *)

var
    DebugWindowInstance: TWINDOWRECT;

const
    DebugWindow: PWINDOWRECT = nil;

procedure prep_debug;
begin
    if (not window_is_created(DebugWindow)) then
    begin
        window_init(@DebugWindowInstance, lightgray, black, 1, 8, scrWidth, scrHeight);
        DebugWindow := @DebugWindowInstance;
    end;
    window_show(DebugWindow);
    window_leave(DebugWindow);
end;

procedure refr_debug;
begin
    if (window_is_created(DebugWindow)) then
    begin
        window_focus(DebugWindow);
        gotoxy(1, 1);
        write('Mix. buf. channels:                      ', mixChannels:3);
        clreol;
        gotoxy(1, 2);
        write('Mix. buf. rate:                          ', mixSampleRate:5);
        clreol;
        gotoxy(1, 3);
        write('Mix. buf. samples per channel:           ', mixBufSamplesPerChannel:5);
        clreol;
        gotoxy(1, 4);
        write('Mix. buf. samples:                       ', mixBufSamples:5);
        clreol;
        gotoxy(1, 5);
        write('Mix. buf. tick samples per channel:      ', mixTickSamplesPerChannel:5);
        clreol;
        gotoxy(1, 6);
        write('Mix. buf. tick samples per channel left: ', mixTickSamplesPerChannelLeft:5);
        clreol;
        gotoxy(1, 7);
        write('DMA buf. allocated:         ', hexw(seg(sndDMABuf.buf^.Unaligned^)), ':', hexw(ofs(sndDMABuf.buf^.Unaligned^)));
        clreol;
        gotoxy(1, 8);
        write('DMA buf. aligned:           ', hexw(seg(sndDMABuf.buf^.Data^)), ':', hexw(ofs(sndDMABuf.buf^.Data^)));
        clreol;
        gotoxy(1, 9);
        write('DMA buf. frame size:        ', sndDMABuf.frameSize:5);
        clreol;
        gotoxy(1, 10);
        write('DMA buf. frames count:      ', sndDMABuf.framesCount:3);
        clreol;
        gotoxy(1, 11);
        write('DMA buf. last frame:        ', sndDMABuf.frameLast:3);
        clreol;
        gotoxy(1, 12);
        write('DMA buf. current frame:     ', sndDMABuf.frameActive:3);
        clreol;
        gotoxy(1, 13);
        write('DMA buf. frames per second: ', playOption_FPS:3);
        clreol;
        window_leave(DebugWindow);
    end;
end;

