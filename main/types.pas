unit types;

interface

type
    dword = packed record
    case integer of
        0: ( DD: longint );
        1: ( LX, HX: word );
        2: ( Rea, Int: word );
    end;
    tarray = array[0..65532] of byte;
    parray = ^tarray;

implementation

end.
