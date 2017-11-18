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
