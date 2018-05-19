{$M 16000,0,1000}
program example_for_s3mplay;

uses
    pascal,
    crt,
    dos,
    string_,
    stdio,
    stdlib,
    sbctl,
    vga,
    s3mvars,
    fillvars,
    mixchn,
    s3mplay;

(*$I defines.pas*)

const
    def_rate = 45454;
    def_stereo = true;
    def_16bits = false;
    def_lq = false;

type Parray = ^TArray;
     TArray = array[0..10000] of byte;

var opt_rate: Word;
    opt_stereo: Boolean;
    opt_16bits: Boolean;
    opt_lq: Boolean;
    opt_filename: String;
    drawseg: Word;

  {$L LINES.OBJ}
  procedure linie(x1,y1,x2,y2:word;f:byte); external;

(*$L playosci.obj*)
procedure playosci_init; far; external;

procedure vga_clear_page_320x200x8(c: Byte);
begin
    asm
    mov        ax,[drawseg]
    mov        es,ax
    xor        di,di
    mov        cx,32000
    mov        al,[c]
    mov        ah,al
    rep stosw
    end;
end;

procedure bar(o,b,l:word); assembler;
    asm
      mov      ax,[drawseg]
      mov      es,ax
      mov      di,[o]
      mov      bx,320
      mov      dx,[b]
      add      bx,dx
      push     bp
      mov      bp,[l]
      cmp      bp,0
      je       @@n
      shl      bp,1
      mov      ax,7
    @@b:
      mov      cx,dx
      rep stosb
      sub      di,bx
      dec      bp
      jnz      @@b
    @@n:
      pop      bp
      push     bp
      mov      bp,[l]
      neg      bp
      add      bp,64
      shl      bp,1
      mov      ax,1
      cmp      bp,0
      jz       @@n2
    @@b2:
      mov      cx,dx
      rep stosb
      sub      di,bx
      dec      bp
      jnz      @@b2
    @@n2:
      pop      bp
    @@e:
    end;

var pos:word;
    i:word;
    h:Parray;
    scr:array[0..319] of byte;
    scr2:array[0..319] of byte;
    b:byte;
    yl,yr:integer;
    chn: PMIXCHN;

begin
    opt_rate := def_rate;
    opt_stereo := def_stereo;
    opt_16bits := def_16bits;
    opt_lq := def_lq;
    opt_filename := paramstr(1);
    drawseg := $a000;

    textbackground(black);
    textcolor(lightgray);
    clrscr;
    writeln(' OSCI-S3M-PLAYER for SoundBlasters written by Cyder of Green Apple');
    writeln(' ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~');
    writeln(' Version : ',PLAYER_VERSION);

    if (opt_filename='') then halt;
    writeln;
    playosci_init;

    if (not player_set_mode (opt_16bits, opt_stereo, opt_rate, opt_lq)) then
        halt;

    if (not player_play_start) then
        halt;

    writeln('DMA buffer frame size: ', sndDMABuf.frameSize);
    writeln('Stop playing and exit with <ESC>');
    writeln('Press any key to switch to oscillator...');
    readkey;

    vbios_set_mode($13);
    vga_clear_page_320x200x8(1);
    { DIsplay Oscilator : }
    if (not opt_stereo) then
    begin
        h := sndDMABuf.buf^.Data;
        while not keypressed do
        begin
          vga_wait_vsync;
          for pos:=0 to usedchannels-1 do
          begin
            chn := @channel[pos];
            bar(320*170+pos*15+10,10,mixchn_get_sample_volume(chn)*ord(mixchn_is_playing(chn)));
          end;
          yl:=h^[player_get_buffer_pos] shr 1;
          for pos:=1 to 319 do
            begin
              i:=player_get_buffer_pos; { current position in DMA buffer }
              linie(pos-1,scr[pos-1],pos,scr[pos],1);
              scr[pos-1]:=yl;yl:=h^[i] shr 1;
              linie(pos-1,scr[pos-1],pos,yl,14);
            end;
          scr[319]:=yl;
        end;
    end
  else { in stereo mode : }
    begin
      h:=sndDMABuf.buf^.Data;
      while not keypressed do
        begin
          for pos:=0 to usedchannels-1 do
          begin
            chn := @channel[pos];
            bar(320*170+pos*15+10,10,mixchn_get_sample_volume(chn)*ord(mixchn_is_playing(chn)));
          end;
          i:=player_get_buffer_pos and $fffe;
          yl:=h^[i] shr 2;yr:=h^[i+1] shr 2;
          for pos:=1 to 319 do
            begin
              i:=player_get_buffer_pos and $fffe; { current position in DMA buffer }
              if (i > sndDMABuf.frameSize) then b:=7 else b:=4;
              { left channel : }
              linie(pos-1,36+scr[pos-1],pos,36+scr[pos],1);
              scr[pos-1]:=yl;yl:=h^[i] shr 2;
              linie(pos-1,36+scr[pos-1],pos,36+yl,14);
              { right channel : }
              linie(pos-1,110+scr2[pos-1],pos,110+scr2[pos],1);
              scr2[pos-1]:=yr;yr:=h^[i+1] shr 2;
              linie(pos-1,110+scr2[pos-1],pos,110+yr,14);
            end;
          scr[319]:=yl;
          scr2[319]:=yr;
        end;
    end;

    vbios_set_mode(3);

    player_free;
end.
