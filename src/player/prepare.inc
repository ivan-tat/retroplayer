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

(* Instruments *)

var chnlastinst:array[0..max_channels-1] of byte;

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

(* Samples *)

var samplepage:byte;
    wassmp_scr:boolean;

procedure prep_smp;
const what:array[false..true] of string[3] = ('OFF','ON ');
var i,j,k,l:byte;
    ins: PMUSINS;
    title: String[29];
  begin
    textcolor(white);textbackground(blue);
    window(1,7,80,25);clrscr;
    textbackground(brown);textcolor(white);
    write('   #  Samplename                 Loop Beg   End   Len   C2Speed');clreol;
    gotoxy(1,2);
    textbackground(blue);
    if wassmp_scr then
      begin { only next page ... }
        inc(samplepage);
        i:=samplepage*18+1;
        if i>insnum then begin samplepage:=0;i:=1 end;
      end
    else
      begin
        i:=1; { start at instrument 1 }
        samplepage:=0;
      end;
    for j:=1 to 18 do
      if i<insnum+1 then
        begin
            ins := musinsl_get(mod_Instruments, i - 1);
            if (j > 1) then
                write(#10#13);
            if (musins_get_type(ins) = MUSINST_PCM) then
                textcolor(white)
            else
                textcolor(lightgray);
            strncpy(@(title[1]), musins_get_title(ins), 28);
            title[0] := Chr(28);
            title[29] := #0;
            l := strlen(@(title[1]));
            for k := l to 27 do
                title[l + 1] := ' ';
            write(
                ' ', i:2, '. ',
                title, ' ',
                what[musins_is_looped(ins)], ' ',
                hexw(musins_get_loop_start(ins)), 'h ',
                hexw(musins_get_loop_end(ins)), 'h ',
                hexw(musins_get_length(ins)), 'h ',
                musins_get_rate(ins):5, 'Hz'
            );
            inc(i);
        end;
    scrLeave;
  end;
