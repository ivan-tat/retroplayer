(* Patterns *)

procedure display_row(ordr,row:byte);
var
    pat: PMUSPAT;
    pA:Parray;
    i,b:byte;
  begin
    pat := muspatl_get(mod_Patterns, ORDER[ordr]);
    if (muspat_is_EM_data(pat)) then
        pA := muspat_map_EM_data(pat)
    else
        pA := muspat_get_data(pat);
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
        pat := muspatl_get(mod_Patterns, playState_pattern);
        if (muspat_is_EM_data(pat)) then
            muspat_map_EM_data(pat);
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
    chn: PMIXCHN;
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
        chn := @channel[i];
        n := ord(mixchn_is_playing(chn)) * mixchn_get_instrument_num(chn) * ord(mixchn_get_type(chn) > 0);
        if (chnlastinst[i] <> n) and (chnlastinst[i] > 0) then
        begin
            textcolor(white);
            textbackground(blue);
            write_no(chnlastinst[i]-1);
        end;
        chnlastinst[i]:=n;
        if n<>0 then
        begin
            textcolor(blue);
            textbackground(white);
            write_no(n-1);
        end;
      end;
    scrLeave;
  end;

(* Samples *)

procedure refr_sample;
var i,n:byte;
    chn: PMIXCHN;
  begin
    for i:=0 to usedchannels-1 do
      begin
        chn := @channel[i];
        n := ord(mixchn_is_playing(chn)) * mixchn_get_instrument_num(chn) * ord(mixchn_get_type(chn) > 0);
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
