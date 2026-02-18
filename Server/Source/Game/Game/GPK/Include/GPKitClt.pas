unit GPKitClt;

interface
uses
  Windows, SysUtils;

type
  BOOL = Byte;
  PPointer = ^Pointer;

  TSetDynCode = function(selfptr: Pointer; lpDynCode: PChar; nLen: LongWord): BOOL; stdcall;
  TEncode = function (selfptr: Pointer; lpData: Pointer; nLen: LongWord): BOOL; stdcall;
  TDecode = function (selfptr: Pointer; lpData: Pointer; nLen: LongWord): BOOL; stdcall;
  TRelease = procedure (selfptr: Pointer); stdcall;


  PICltDynMethod = ^ICltDynMethod;
  ICltDynMethod = packed record
    SetDynCode: TSetDynCode;
    Encode: TEncode;
    Decode: TDecode;
    Release: TRelease;
  end;

  PIGPKCltDynCode = ^IGPKCltDynCode;
  IGPKCltDynCode = packed record
    Method: PICltDynMethod;
  end;



function GPKStart(UpdateServerURL: PChar; szGameCode: PChar):PIGPKCltDynCode; stdcall; external 'GPKitClt.dll' index 1;

implementation

{
µ÷ÓÃÊ¾Àý£º
  DynCode: PISDCltDynCode;

  DynCode := GPKStart('http://127.0.0.1/woool/gpk','demo');

  DynCode.Method.SetDynCode(DynCode, PChar(lpDynCode), nLen);
}

end.
