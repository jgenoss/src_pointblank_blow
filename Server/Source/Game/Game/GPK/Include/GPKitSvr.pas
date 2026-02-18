unit GPKitSvr;

interface
uses
  Windows, SysUtils;

type
  PPointer = ^Pointer;

  TDestroy = procedure(selfptr: Pointer); stdcall;
  TLoadBinary = function (selfptr: Pointer; pszSvrBinDir: PChar; pszsCltBinDir: PChar): Integer; stdcall;
  TGetRandIdx = function (selfptr: Pointer): Integer; stdcall;
  TGetCltDynCode = function (selfptr: Pointer; nCodeIdx: Integer; ppCodeRet: PPointer): Integer; stdcall;
  TEncode = function (selfptr: Pointer; lpData: Pointer; nLen: LongWord; nCodeIdx: Integer): Byte; stdcall;
  TDecode = function (selfptr: Pointer; lpData: Pointer; nLen: LongWord; nCodeIdx: Integer): Byte; stdcall;
  TRelease = procedure (selfptr: Pointer); stdcall;

  PISvrDynMethod = ^ISvrDynMethod;
  ISvrDynMethod = packed record
    Destroy: TDestroy;
    LoadBinary: TLoadBinary;
    GetRandIdx: TGetRandIdx;
    GetCltDynCode: TGetCltDynCode;
    Encode: TEncode;
    Decode: TDecode;
    Release: TRelease;
  end;

  PIGPKSvrDynCode = ^IGPKSvrDynCode;
  IGPKSvrDynCode = packed record
    Method: PISvrDynMethod;
  end;

function GPKCreateSvrDynCode: PIGPKSvrDynCode; stdcall; external 'GPKitSvr.dll';

implementation

{
µ÷ÓÃÊ¾Àý£º
  DynCode: PIGPKSvrDynCode;

  DynCode := GPKCreateSvrDynCode;

  DynCode.Method.LoadBinary(DynCode, PChar(ServerDir), PChar(ClientDir));
}

end.
