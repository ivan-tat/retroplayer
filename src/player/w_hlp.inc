(* Help window *)

procedure display_keys; far; external;

procedure display_helpscreen;
  begin
    textcolor(white);textbackground(blue);
    window(1,8,80,25);clrscr;
    writeln;
    display_keys;
    window(1,1,80,25);
  end;
