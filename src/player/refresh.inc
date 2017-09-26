(* Channels *)

procedure write_Note(b:byte);
const
    nname: array[0..11] of string[2] =
        ('C-','C#','D-','D#','E-','F-', 'F#','G-','G#','A-','A#','B-');
begin
    if b<254 then write(nname[b and $0f],b shr 4)
        else if b=254 then write('^^.') else write('...');
end;

procedure display_row(ordr,row:byte);
var
    pat: PMUSPAT;
    pA:Parray;
    i,b:byte;
  begin
    pat := patList_get(ORDER[ordr]);
    pA := patGetData(pat);
    write(row:2,'   ');
    for i:=startchn-1 to startchn+3 do
      begin
        if (i+1-startchn) mod 2 = 0 then begin textbackground(black);textcolor(lightgray) end
        else begin textbackground(white);textcolor(black) end;
        if i<=usedchannels-1 then
          begin
            { write Note : }
            b:=pA^[row*usedchannels*5+i*5];
            write_Note(b);
            { write Instrument : }
            b:=pA^[row*usedchannels*5+i*5+1];
            write(' ',b div 10,b mod 10);
            { write volume : }
            b:=pA^[row*usedchannels*5+i*5+2];
            if b<255 then write(' ',b div 10,b mod 10) else write(' ..');
            { write special command+info: }
            b:=pA^[row*usedchannels*5+i*5+3];
            if b<255 then write(' ',chr(ord('A')+b-1)) else write(' .');
            b:=pA^[row*usedchannels*5+i*5+4];
            write(hexb(b), ' ');
          end
        else
          write('              ');
      end;
  end;

procedure display_currow;
  begin
    display_row(playState_order,playState_row);writeln;
  end;

{$I EFX_NAME.INC}

procedure refr_channels;
const types:array[0..4] of string[6] = ('OFF','Left','Right','Adlib1','Adlib2');
var i:byte;
    j:byte;
  begin
    gotoxy(1,8);
    textbackground(blue);textcolor(white);
    i:=0;j:=0;
    while i<usedchannels do
      begin
        if (channel[i].bChannelType<3) and
           ((usedchannels<17) or (channel[i].bChannelType>0)) then
            { display only sample channels }
          begin
            inc(j);
            write(' Chn ',(i+1):2,' (',types[channel[i].bChannelType]:5,') ');
            if channel[i].bEnabled then write('*') else write(' ');
            if channel[i].bSmpFlags and SMPFLAG_LOOP <> 0 then write('!') else write(' ');
            if channel[i].bEffFlags and EFFFLAG_CONTINUE <> 0 then write('c') else write(' ');
            write('(',channel[i].bIns:2,') ');
            write('[');
            if channel[i].bEnabled then
              begin
                write_note(channel[i].bNote);
                { Arpeggio test : }
                {write(',');write_note(channel[i].bArpNotes[0]);write(',');write_note(channel[i].bArpNotes[1]);}
              end
            else write('...');
            write('] ');
            write('<',channel[i].wSmpPeriod:5,'>');
            write(' ',channel[i].dSmpStep.DD/64/1024:7:4);
            { Arpeggio test : }
            {write(' ',channel[i].dArpSmpSteps[1].DD/64/1024:7:4);
            write(' ',channel[i].dArpSmpSteps[2].DD/64/1024:7:4);}
            write(' ',channel[i].bSmpVol:2);
            if channel[i].bCommand <= MAXEFF then
            write(' ',get_effectname(i,channel[i].bCommand,channel[i].bCommand2,channel[i].bParameter));clreol;
            write(#13#10);
          end;
        inc(i);
      end;
    if j=0 then writeln(' No digital channels to mix ...     :( ');
  end;

(* Patterns *)

procedure refr_patterns;
var
    i,j:byte;
    pat: PMUSPAT;
  begin
    window(1,9,80,25);textbackground(white);textcolor(black);
    if playState_row+1<lastrow then
      begin
        j:=prevorder(playState_order);
        for i:=lastrow to 63 do
          begin
            gotoxy(1,17);writeln;gotoxy(1,17);display_row(j,i);
          end;
        lastrow:=0;
        { new pattern }
        (* switch EMSpage zero to curpattern *)
        pat := patList_get(playState_pattern);
        patMapData(pat);
      end;
    for i:=lastrow to playState_row do
      begin
        gotoxy(1,17);writeln;gotoxy(1,17);display_row(playState_order,i);
      end;
    lastrow:=playState_row+1;
    scrLeave;
  end;

(* Instruments *)

procedure refr_inst;
var i:byte;
    n:byte;
  procedure write_no(j:word);
    begin
      gotoxy((j div 15)*11+1,j mod 15+1);
      write(j+1:2);
    end;
  begin
    textbackground(blue);textcolor(white);
    if useEMS then window(1,11,80,25) else window(1,10,80,25);
    for i:=0 to usedchannels-1 do
      begin
        n:=ord(channel[i].bEnabled)* channel[i].bIns*ord(channel[i].bChannelType>0);
        if (chnlastinst[i]<>n) and (chnlastinst[i]>0) then
          begin textcolor(white);textbackground(blue);write_no(chnlastinst[i]-1) end;
        chnlastinst[i]:=n;if n<>0 then begin textcolor(blue);textbackground(white);write_no(n-1) end;
      end;
    scrLeave;
  end;

(* Samples *)

procedure refr_sample;
var i,n:byte;
  begin
    for i:=0 to usedchannels-1 do
      begin
        n:=ord(channel[i].bEnabled)* channel[i].bIns*ord(channel[i].bChannelType>0);
        if (chnlastinst[i]<>n) and (chnlastinst[i]>0) and
           (samplepage*18<chnlastinst[i]) and (samplepage*18+19>chnlastinst[i]) then
          begin
            textcolor(white);textbackground(blue);
            gotoxy(1,7+chnlastinst[i]-samplepage*18);
            write(chnlastinst[i]:3,'.');
          end;
        chnlastinst[i]:=n;
        if (n<>0) and (samplepage*18<n) and (samplepage*18+19>n) then
          begin
            textcolor(blue);textbackground(white);
            gotoxy(1,7+chnlastinst[i]-samplepage*18);
            write(n:3,'.');
          end;
      end;
  end;

(* Debug *)

procedure refr_debug;
begin
    if (isWindowCreated(DebugWindow)) then
    begin
        focusWindow(DebugWindow);
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
        leaveWindow(DebugWindow);
    end;
end;

(* Router *)

procedure refresh_scr;
  begin
    case screen_no of
      0: { Helpscreen (do not refrsh) };
      1: { channel view }     refr_channels;
      2: { pattern view }     refr_patterns;
      3: { Instrument infos } refr_inst;
      4: { sample infos }     refr_sample;
      5: (* DEBUG *)          refr_debug;
    end;
  end;
