(* Patterns *)

var lastrow:byte;

procedure prep_patterns;
var
    w:word;
    pat: PMUSPAT;
  begin
    scrOpenInfoBackground(white, black);
    textbackground(white);textcolor(black);
    for w:=0 to 2 do begin gotoxy(w*28+6,1);write('    Chn ',2*w+startchn:2,'    '); end;
    textbackground(black);textcolor(white);
    for w:=0 to 1 do begin gotoxy(w*28+20,1);write('    Chn ',2*w+1+startchn:2,'    '); end;
    scrLeave;
    lastrow:=playState_row;
    pat := muspatl_get(mod_Patterns, playState_pattern);
    if (muspat_is_EM_data(pat)) then
        muspat_map_EM_data(pat);
  end;

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
