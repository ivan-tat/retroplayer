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
