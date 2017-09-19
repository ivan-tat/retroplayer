(* Text window *)

const
    scrWidth = 80;
    scrHeight = 25;

type
    TWindowRect = record
        visible: boolean;
        focused: boolean;
        fgColor, bgColor: byte;
        x0, y0, x1, y1: byte;
        width, height: byte;
    end;
    PWindowRect = ^TWindowRect;

var
    DebugWindowInstance: TWindowRect;

const
    DebugWindow: PWindowRect = nil;

procedure createWindow(Self: PWindowRect; fgColor, bgColor: byte; x0, y0, x1, y1: byte);
begin
    Self^.visible := false;
    Self^.focused := false;
    Self^.fgColor := fgColor;
    Self^.bgColor := bgColor;
    Self^.x0 := x0;
    Self^.y0 := y0;
    Self^.x1 := x1;
    Self^.y1 := y1;
    Self^.width := x1-x0+1;
    Self^.height := y1-y0+1;
end;

function isWindowCreated(Self: PWindowRect): boolean;
begin
    isWindowCreated := Self <> nil;
end;

procedure showWindow(Self: PWindowRect);
begin
    Window(Self^.x0, Self^.y0, Self^.x1, Self^.y1);
    TextColor(Self^.fgColor);
    TextBackground(Self^.bgColor);
    ClrScr;
    Self^.visible := true;
    Self^.focused := true;
end;

procedure focusWindow(Self: PWindowRect);
begin
    if (Self^.visible) then
    begin
        Window(Self^.x0, Self^.y0, Self^.x1, Self^.y1);
        TextColor(Self^.fgColor);
        TextBackground(Self^.bgColor);
        Self^.focused := true;
    end;
end;

procedure leaveWindow(Self: PWindowRect);
begin
    Self^.focused := false;
    Window(1, 1, scrWidth, scrHeight);
end;

procedure closeWindow(Self: PWindowRect);
begin
    if (Self^.focused) then leaveWindow(Self);
    Self^.visible := false;
end;

const
    scrHeaderTop = 1;
    scrHeaderBottom = 7;
    scrHeaderLeft = 1;
    scrHeaderRight = scrWidth;

    scrInfoTop = scrHeaderBottom + 1;
    scrInfoBottom = scrHeight;
    scrInfoLeft = 1;
    scrInfoRight = scrWidth;

procedure scrOpenInfoBackground(textCol, bgCol: byte);
begin
    textcolor(textCol);
    textbackground(bgCol);
    window(scrInfoLeft, scrInfoTop, scrInfoRight, scrInfoBottom);
    clrscr;
end;

procedure scrLeave;
begin
    window(1, 1, scrWidth, scrHeight);
end;

(* Channels *)

procedure prep_channels;
begin
    scrOpenInfoBackground(white, blue);
    scrLeave;
end;

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
    lastrow:=curline;
    { switch EMSpage 0 to curpattern }
    pat := patList_get(curpattern);
    if (patIsDataInEM(pat)) then
        EmsMap(patListEMHandle, patGetDataLogPageInEM(pat), 0);
  end;

(* Instruments *)

var chnlastinst:array[0..max_channels-1] of byte;

procedure prep_inst;
var i,l:word;
function posstring(b:byte):string;
var psmp:pSmpheader;
  begin
    psmp:=addr(Instruments^[b]);
    if psmp^.typ=1 then
      begin
        posstring:=hexw(psmp^.mempos)+'h';
      end
    else
      begin
        posstring:='     ';
      end;
  end;
  begin
    scrOpenInfoBackground(white, blue);
    writeln(' Sample positions in memory : ');
    l:=15;
    textcolor(yellow);
    if useEMS then begin write(#10' EMS used by samples :',getusedEMSsmp,' KB') end;
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
var i,j:byte;
    Psmp:PSmpHeader;
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
          psmp:=addr(Instruments^[i]);
          if j>1 then write(#10#13);
          if pSmp^.typ=1 then textcolor(white) else textcolor(lightgray);
          write(' ',i:2,'. ',leftformat(psmp^.name,28),' ',what[(psmp^.flags and 1) = 1],' ',
                hexw(psmp^.loopbeg),'h ',hexw(psmp^.loopend),'h ',hexw(psmp^.length),'h ',pSmp^.c2speed:5,'Hz');
          inc(i);
        end;
    scrLeave;
  end;

(* Debug *)

procedure prep_debug;
begin
    if (not isWindowCreated(DebugWindow)) then
    begin
        createWindow(@DebugWindowInstance, lightgray, black, 1, 8, scrWidth, scrHeight);
        DebugWindow := @DebugWindowInstance;
    end;
    showWindow(DebugWindow);
    leaveWindow(DebugWindow);
end;

(* Router *)

procedure prepare_scr;
  begin
    case screen_no of
      0: { help screen }      display_helpscreen;
      1: { channel view }     prep_channels;
      2: { pattern view }     prep_patterns;
      3: { Instrument infos } prep_inst;
      4: { sample infos }     prep_smp;
      5: (* DEBUG *)          prep_debug;
    end;
    wassmp_scr:=screen_no=4;
  end;
