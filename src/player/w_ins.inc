(* Instruments *)

function posstring(i: Byte): String;
var
    ins: PMUSINS;
begin
    ins := musinsl_get(mod_Instruments, i - 1);
    if (musins_get_type(ins) = MUSINST_PCM) then
    begin
        if (musins_is_EM_data(ins)) then
            posstring := 'E' + hexw(musins_get_EM_data_page(ins))
        else
            posstring := ' ' + hexw(seg(musins_get_data(ins)^))
    end
    else
        posstring := '     ';
end;

procedure prep_inst;
var i,l:word;
  begin
    scrOpenInfoBackground(white, blue);
    writeln(' Sample positions in memory : ');
    l:=15;
    textcolor(yellow);
    if (useEMS) then
        write(#10' EMS used by samples: ', musinsl_get_used_EM(mod_Instruments), ' KiB');
    textcolor(white);
    for i:=1 to l do
      begin
        write(#13#10,i:2,': ',posstring(i),'  ');
        write(i+l:2,': ',posstring(i+l),'  ');
        write(i+2*l,': ',posstring(i+2*l),'  ');
        write(i+3*l,': ',posstring(i+3*l),'  ');
        write(i+4*l,': ',posstring(i+4*l),'  ');
        write(i+5*l,': ',posstring(i+5*l),'  ');
        if i+6*l<100 then write(i+6*l,': ',posstring(i+6*l),'  ');
      end;
    textcolor(white);textbackground(blue);
    scrLeave;
  end;

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
