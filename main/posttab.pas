unit posttab;

interface

var
    post8bit :array[0..4095] of byte;
    post16bit:array[0..4095] of word;

procedure calcPostTable( vol: byte; use16bit: boolean );

implementation

uses i4m;

(*$l posttab.obj*)

procedure calcPostTable( vol: byte; use16bit: boolean ); external;

end.
