unit voltab;

interface

type
    voltab_t = array[0..64,0..255] of integer;
    voltab_p = ^voltab_t;

var
    VolumeTablePtr: voltab_p;

procedure initVolumeTable;
function allocVolumeTable: boolean;
procedure calcVolumeTable( sign: boolean );
procedure freeVolumeTable;

implementation

uses
    dosproc;

(*$l voltab.obj*)

procedure initVolumeTable; external;
function allocVolumeTable: boolean; external;
procedure calcVolumeTable( sign: boolean ); external;
procedure freeVolumeTable; external;

end.
