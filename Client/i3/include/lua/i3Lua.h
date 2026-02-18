#ifndef __I3LUA_H
#define __I3LUA_H

//	=================================================================================
//	lua (ver. 5.1) and i3Lua
//
//	Programed : 백성민
//	Build Date : 2006.5.9
//	Last Update : 2006.5.26
//  License : Copyright (C) 1994-2006 Lua.org, PUC-Rio. 
//	Homepage : http://www.lua.org
//	=================================================================================

//	note1. 현재 Lua 내부는 i3Base에는 없는 <time.h> <locale.h>의 c 라이브러리를 사용합니다.
//		그리고 i3Math에는 등록되지 않고 <math.h>에만 있는 몇몇 함수들도 사용하므로 PC이외의 기종일 경우 
//		현재 라이브러리와 함수를 지원하는지 확인해 주십시요
//	note2. i3Lua에는 없는 오리지날 루아 함수를 사용하려면 i3Lua에 함수를 추가하여 사용하십시요.
//	note3. i3Lua에서는 오리지날 루아의 루아 스크립트에서는 사용 불가능한 '//' 를 주석으로 사용할수 있게 변경되었습니다.

#include "i3LuaDefine.h"
#include "i3Base.h"
#include "i3Math.h"

#if defined( I3_DEBUG)
#define	I3LUA_PRINT			i3Lua::DebugWinPrint
#else
#define I3LUA_PRINT			__noop
#endif

namespace i3Lua
{
	//***************************************************************************************************//
	//	Main functions
	//***************************************************************************************************//

	//	Lua 를 초기화한다.
	I3_EXPORT_LUA	LuaState *	Open( void);					

	//	Lua 를 종료한다.
	I3_EXPORT_LUA	void		Close( LuaState * pState);

	//	새로운 state가 반환한다. 이 state는 모든 전역 환경(테이블등등)은 공유하지만 독립적인 실시간 스택을 가지고 있다. 
	//	각 쓰레드는 전역변수를 위한 독립적인 테이블을 가지고 있다. 쓰레드를 생성하면, 이 테이블은 인자로 주어진 state와 동일하다. 
	//	원래의 state와 함께 모든 전역 개체를 공유한다. 	그러나 독립적인 실시간 스택을 소유한다. 
	I3_EXPORT_LUA LuaState *	NewThread( LuaState * pState, void * pKey);

	I3_EXPORT_LUA INT32		ThreadResume( LuaState * pState, INT32 arg);

	I3_EXPORT_LUA INT32		ThreadYield( LuaState * pState, INT32 result);


	//***************************************************************************************************//
	//	load & excute functions
	//***************************************************************************************************//

	//	Lua 파일을 읽고 바로 실행한다.	 (내부적으로 loadfile후에 pcall을 호출한다.)
	//	Opens the named file and executes its contents as a Lua chunk
	I3_EXPORT_LUA	INT32		DoFile( LuaState * pState, const char * pszFileName);

	//	Lua 문자열을 읽고 바로 실행한다. (내부적으로 loadstring후에 pcall을 호출한다.)
	I3_EXPORT_LUA	INT32		DoString( LuaState * pState, const char * pszString);

	//	Lua 파일을 읽고 컴파일한다.
	I3_EXPORT_LUA	INT32		LoadFile( LuaState * pState, const char * pszFileName);

	//	Buffer를 읽고 컴파일한다.
	I3_EXPORT_LUA	INT32		LoadBuffer( LuaState * pState, const char * pBuf, size_t nSize, const char * pName);

	//	Lua 문자열을 읽고 컴파일한다.
	I3_EXPORT_LUA	INT32		LoadString( LuaState * pState, const char * pszString);

	//  LuaState가 유효한 상태에서, Load시키고 난 결과물을 버퍼에 보관한다.. 버퍼는 읽기전에 크기를 알수 없다고 가정.. 일단 버퍼를 얻었으면 LuaState는 제거해도 된다.
	I3_EXPORT_LUA	bool		GetCompiledBuffer( LuaState* pState, const char * pszFileName, i3::vector<BYTE>& out);
	I3_EXPORT_LUA	bool		GetCompiledBuffer( LuaState* pState, const char* pBuf, size_t nSize, const char * pName, i3::vector<BYTE>& out);
	//

	// 루아파일들을 통째로 읽어들인다... 읽어들인 것들은 LuaState가 없고, 일정한 메모리를 추가로 차지하게 됨..
	I3_EXPORT_LUA	void		SaveCompiledFileDirectory( const char* szSrcDir, const char* szDestDir);
	I3_EXPORT_LUA	void		SaveCompiledFileDirectory( const char* szLuaDir);
	I3_EXPORT_LUA	void		AddPreloadDirectory( const char* szLuaDir, bool bSubDirSearch);
	I3_EXPORT_LUA	bool		LoadFileFromPreload( LuaState* L, const char* pszFileName);

	// 공통 리콰이어 대상에 대해 따로 모아둔다..  스테이트를 만들때, 루아 표준Lib를 받아들이고 나서, 적재만하고 call은 하지 않게 조정한다..
	// 이렇게 처리하는 이유는 루아쪽의 require가 제대로 리콰이어루아를 2번읽지 않으면서도, lua_call은 알맞게 잘 돌도록 조정하기 위한 것임..
	I3_EXPORT_LUA	bool		AddPreloadRequire( const char* szPath);
	I3_EXPORT_LUA	void		AddPreloadRequireDirectory( const char* szLuaDir, bool bSubDirSearch);
	I3_EXPORT_LUA	void		ProcessRequireFromPreload(LuaState* pState);
	//

	//	읽어들인 루아를 실행한다. (Protected Call. 에러가 발생해도 에러 메시지만 출력하고 그대로 진행한다.)
	//	성공시에는 0을 반환하고, 그렇지 않으면 다음 에러 코드들 중 하나를 반환한다
	//	: 파라메타
	//	arg - 인자 갯수
	//	result - 반환값 갯수
	//	: 에러 코드
	//	LUA_ERRRUN --- 실행시간 에러 
	//	LUA_ERRMEM --- 메모리 할당 에러. 그러한 에러들에 대해 루아는 에러 핸들러 함수를 호출하지 않는다. 
	//	LUA_ERRERR --- 에러 핸들러 함수가 실행되는 동안의 에러.
	//	Calls a function in protected mode
	I3_EXPORT_LUA	INT32		PCall( LuaState * pState, INT32 arg, INT32 result, INT32 errfunc);

	//	읽어들인 루아를 실행한다. (도중 에러가 발생하면 바로 실행을 중단하고 종료한다.)
	I3_EXPORT_LUA	void		Call( LuaState * pState, INT32 arg, INT32 result);

	//	Calls the C function in protected mode 
	I3_EXPORT_LUA	INT32		CPCall( LuaState* pState, LuaFunction func, void * ud);


	//***************************************************************************************************//
	//	basic stack manipulation
	//***************************************************************************************************//

	//	top 요소의 인덱스를 얻을 수 있다. 
	I3_EXPORT_LUA	INT32		StackGetTop( LuaState * pState);

	//	스택의 top으로 설정한다
	I3_EXPORT_LUA	void		StackSetTop( LuaState * pState, INT32 Idx);

	//	count개만큼 스택으로부터 pop처리를 수행한다. 
	I3_EXPORT_LUA	void		StackPop( LuaState * pState, INT32 count);

	//	주어진 인덱스상의 값의 복사본을 스택에 푸쉬 처리한다
	I3_EXPORT_LUA	void		StackPushValue( LuaState * pState, INT32 Idx);

	//	Moves the top element into the given valid index
	I3_EXPORT_LUA	void		StackInsert( LuaState * pState, INT32 Idx);

	//	지정한 인덱스 위치의 값을 제거하고, 지워진 곳을 매꾸기위해 나머지 위치의 요소들을 모두 끌어올린다.
	I3_EXPORT_LUA	void		StackRemove( LuaState * pState, INT32 Idx);

	//	각 요소들의 위치이동없이 주어진 인덱스 위치로 top 요소의 값을 이동시킨다. (이것은 top의 값을 주어진 위치로 덮어쓰고, pop처리한 것과 같다.) 
	I3_EXPORT_LUA void		StackReplace( LuaState * pState, INT32 Idx);

	//	스택크기가 top + nSize값만큼 증가하게 된다
	I3_EXPORT_LUA INT32		StackCheck( LuaState * pState, INT32 nSize);

	//	Pops a key from the stack, and pushes a key-value pair from the table at the given index 
	I3_EXPORT_LUA INT32		StackNext( LuaState * pState, INT32 Idx);


	//***************************************************************************************************//
	//	reference (Lua Registry)
	//***************************************************************************************************//
	I3_EXPORT_LUA	INT32		Ref( LuaState * pState, bool bVal);

	I3_EXPORT_LUA	void		UnRef( LuaState * pState, INT32 ref);

	I3_EXPORT_LUA	void		GetRef( LuaState * pState, INT32 ref);


	//***************************************************************************************************//
	//	Push functions (C++ -> stack)
	//***************************************************************************************************//

	//	루아 스택에 수치값을 넣는다. ( 스크립트 함수로 값을 반환할때 사용)
	I3_EXPORT_LUA	void		PushNumber( LuaState * pState, LuaNumber nVal);

	//	루아 스택에 Integer값을 넣는다. ( 스크립트 함수로 값을 반환할때 사용)
	I3_EXPORT_LUA	void		PushInteger( LuaState * pState, LuaInteger nVal);

	//	루아 스택에 문자열값을 넣는다. ( 스크립트 함수로 값을 반환할때 사용)
	I3_EXPORT_LUA	void		PushString( LuaState * pState, const WCHAR16 * pszString);
	I3_EXPORT_LUA	void		PushStringUTF16To8( LuaState * pState, const WCHAR16 * pszString);
	I3_EXPORT_LUA	void		PushString( LuaState * pState, const char* pszString);
	
	I3_EXPORT_LUA	void		PushString( LuaState * pState, const WCHAR16 * pszString, size_t len);
	I3_EXPORT_LUA	void		PushStringUTF16To8( LuaState * pState, const WCHAR16 * pszString, size_t len);

	I3_EXPORT_LUA	void		PushStringUTF16To8( LuaState* pState, const i3::wliteral_range& rng);


	I3_EXPORT_LUA	void		PushString( LuaState * pState, const char* pszString, size_t len);


	//	루아 스택에 문자열값을 넣는다 (가변 변수 지원)
	I3_EXPORT_LUA	void		PushfString( LuaState * pState, const char * pszString, ...);

	//	루아 스택에 문자열 값을 넣는다 (표준 ansi코드의 문자열을 utf-8로 변환하여 스택에 넣는다)
	I3_EXPORT_LUA	void		PushA2UTF8String( LuaState * pState, const char * pszString);

	//	루아 스택에 nil 값을 넣는다. ( 스크립트 함수로 값을 반환할때 사용)
	I3_EXPORT_LUA	void		PushNil( LuaState * pState);

	//	루아 스택에 Boolean 값을 넣는다.
	I3_EXPORT_LUA	void		PushBoolean( LuaState * pState, bool bVal);

	//	루아스택에 포인터를 넣는다.
	I3_EXPORT_LUA	void		PushUserData( LuaState * pState, void * p);

	// Color값 R,G,B,A를 넣는다.
	// Stack은 4개를 넣게된다.
	I3_EXPORT_LUA	void		PushColor( LuaState * pState, const I3COLOR * pColor);

	//	루아 스택에 루아글루 함수를 넣는다.
	I3_EXPORT_LUA	void		PushCFunction( LuaState * pState, LuaCFunction func);



	//***************************************************************************************************//
	//	Get functions (stack -> C++)
	//***************************************************************************************************//

	//	루아 함수에서 보낸 매개변수 값을 받아온다. (기본적인 처리는 Check...Arg와 같지만 default 값을 설정할수 있다)
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// return double
	I3_EXPORT_LUA LuaNumber	GetNumberArg( LuaState * pState, INT32 argNum, LuaNumber nDefault = 0);

	// return int
	I3_EXPORT_LUA INT32		GetIntegerArg( LuaState * pState, INT32 argNum, LuaInteger nDefault = 0);

	// return long
	I3_EXPORT_LUA UINT32	GetLongArg( LuaState * pState, INT32 argNum, LuaInteger nDefault = 0);

	I3_EXPORT_LUA const char*	GetStringArg( LuaState * pState, INT32 argNum, char * pBuf, INT32 nBufSize, const char * szDefault = nullptr);
	I3_EXPORT_LUA bool		GetBooleanArg( LuaState * pState, INT32 argNum, bool bDefault = false);

	//	포인터인 인자를 루아 스택에서 받아온다.
	I3_EXPORT_LUA void *		GetUserDataArg( LuaState * pState, INT32 argNum);

	//	루아 함수에서 보낸 매개변수 값이 지정된 타입인지 검사후 반환 ( 정상일경우 매개변수 값을 반환)
	///////////////////////////////////////////////////////////////////////////////////////////////////
	I3_EXPORT_LUA	LuaNumber	CheckNumberArg( LuaState * pState, INT32 argNum);

	I3_EXPORT_LUA	INT32		CheckIntegerArg( LuaState * pState, INT32 argNum);

	I3_EXPORT_LUA	UINT32		CheckLongArg( LuaState * pState, INT32 argNum);

	I3_EXPORT_LUA	const char *CheckStringArg( LuaState * pState, INT32 argNum);

	//	Lua Stack에 저장된 값의 타입 체크
	I3_EXPORT_LUA	I3LUA_TYPE	StackIsType( LuaState * pState, INT32 Idx);
	I3_EXPORT_LUA	bool		StackIsNumber( LuaState * pState, INT32 Idx);
	I3_EXPORT_LUA	bool		StackIsString( LuaState * pState, INT32 Idx);

	//	루아 함수와 C 함수 모두 판단한다. (주의)
	I3_EXPORT_LUA	bool		StackIsFunction( LuaState * pState, INT32 Idx);

	//	C 함수만 판단한다.
	I3_EXPORT_LUA	bool		StackIsCFunction( LuaState * pState, INT32 Idx);

	I3_EXPORT_LUA	bool		StackIsUserData( LuaState * pState, INT32 Idx);

	I3_EXPORT_LUA	bool		StackIsEqual( LuaState * pState, INT32 Idx1, INT32 Idx2);

	I3_EXPORT_LUA	bool		StackIsLessthan( LuaState * pState, INT32 Idx1, INT32 Idx2);

	I3_EXPORT_LUA	const char *StackTypeName( LuaState * pState, I3LUA_TYPE type);

	//	Lua Stack의 값을 직접 가져온다. 	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	I3_EXPORT_LUA	LuaNumber		StackToNumber( LuaState * pState, INT32 Idx);

	I3_EXPORT_LUA	INT32			StackToInteger( LuaState * pState, INT32 Idx);

	I3_EXPORT_LUA	bool			StackToBoolean( LuaState * pState, INT32 Idx);

	I3_EXPORT_LUA	const char*		StackToString( LuaState * pState, INT32 Idx);

	I3_EXPORT_LUA	INT32			StackToStrlen( LuaState * pState, INT32 Idx);

	I3_EXPORT_LUA	LuaCFunction 	StackToCFunction( LuaState * pState, INT32 Idx);

	I3_EXPORT_LUA	void *			StackToUserData( LuaState * pState, INT32 Idx);

	I3_EXPORT_LUA	LuaState *		StackToThread( LuaState * pState, INT32 Idx);

	//	Get table or function pointer, or userdata
	I3_EXPORT_LUA	const void *	StackToPointer( LuaState * pState, INT32 Idx);

	//***************************************************************************************************//
	//	Get functions (lua -> stack)
	//***************************************************************************************************//
	//	Pushes onto the stack the value of the global name
	I3_EXPORT_LUA	void		GetGlobal( LuaState * pState, const char * s);

	inline void		GetField( LuaState* pState, INT32 idx, const char* k)
	{
		lua_getfield( pState, idx, k);
	}

	I3_EXPORT_LUA	void		GetField( LuaState* pState, INT32 idx, const wchar_t * k);

	I3_EXPORT_LUA	void		Rawget( LuaState * pState, INT32 Idx);

	I3_EXPORT_LUA	void		Rawgeti( LuaState * pState, INT32 idx, INT32 n);


	//	This function allocates a new block of memory with the given size. pushes on the stack a new full userdata 
	//	with the block address, and returns this address. 
	//	full 유저데이타를 생성한다. ( 루아 환경내에 주어진 크기만큼 새로운 메모리블럭을 할당)
	I3_EXPORT_LUA	void *		NewUserData( LuaState * pState, size_t nSize);


	//***************************************************************************************************//
	//	Library functions
	//***************************************************************************************************//

	//	Lua Base Library를 초기화한다.
	I3_EXPORT_LUA	INT32		LibOpenBase( LuaState * pState);

	//	Lua I/O Library를 초기화한다.
	I3_EXPORT_LUA	INT32		LibOpenIo( LuaState * pState);

	//	Lua String Library를 초기화한다.
	I3_EXPORT_LUA	INT32		LibOpenString( LuaState * pState);

	//	Lua Math Library를 초기화한다.
	I3_EXPORT_LUA	INT32		LibOpenMath( LuaState * pState);

	//	Lua Debug Library를 초기화한다.
	I3_EXPORT_LUA	INT32		LibOpenDebug( LuaState * pState);

	//	Lua Table Library를 초기화한다.
	I3_EXPORT_LUA	INT32		LibOpenTable( LuaState * pState);

	//	Lua의 다른 확장 Library를 초기화한다. 	
	I3_EXPORT_LUA	INT32		LibOpenPackage( LuaState * pState);

	//	Lua System Library를 초기화한다.
	I3_EXPORT_LUA	INT32		LibOpenOS( LuaState * pState);

	I3_EXPORT_LUA	INT32		LibOpenStd( LuaState * pState);

	I3_EXPORT_LUA	void		LibOpenLibs(LuaState* pState);

	//***************************************************************************************************//
	//	useful macros
	//***************************************************************************************************//
	//	스크립트에서 호출할수 있는 루아글루 함수(Callback)를 등록한다.
	I3_EXPORT_LUA	void		RegisterFunc( LuaState * pState, const char * pFuncName, LuaFunction pFunction); 

	I3_EXPORT_LUA	void		RegisterFuncs( LuaState * pState, const char * pLibName, LuaFuncReg * pFunctions);

	//	마지막 에러의 문자열을 반환한다.
	I3_EXPORT_LUA	const char *GetErrorString( LuaState * pState);

	//	Generates a Lua error. 에러를 낸다. 	
	I3_EXPORT_LUA	void		Error( LuaState * pState);

	//	메시지를 담아 에러를 낸다.
	I3_EXPORT_LUA	INT32				SetError(LuaState * pState, const char *fmt, ...);
	I3_EXPORT_LUA	INT32				SetError(LuaState * pState, const wchar_t * fmt, ...);

	// set log call back
	I3_EXPORT_LUA	void				SetLogCallBack( i3LuaLogCallback cb);

	//	log
	I3_EXPORT_LUA	void				Log(const char * fmt, ...);

	//	루아 버전 
	I3_EXPORT_LUA	const char *GetLuaVersion( void);

	I3_EXPORT_LUA	bool				AddEnumToLua(LuaState* L, const char* tname, ...);			// State에 enum값을 넣는다.
	I3_EXPORT_LUA	bool				CheckEnumType(LuaState* L, const char* tname, int index);	// 해당 enum값이 동일 type의 enum값인지 확인한다.
	I3_EXPORT_LUA	INT32				GetEnumValue(LuaState* L, int index);						// enum값을 정수형으로 얻어온다.

	//***************************************************************************************************//
	//	garbage-collection function
	//***************************************************************************************************//

	I3_EXPORT_LUA	INT32				GabageCollector( LuaState * pState, I3LUA_GC_PARAM param, INT32 data);



	//***************************************************************************************************//
	//	dump function
	//***************************************************************************************************//
	I3_EXPORT_LUA	INT32		Dump( LuaState* pState, const Proto* f, lua_Writer w, void* data, INT32 strip);	// 덤프

	I3_EXPORT_LUA	void		UPrint( const Proto* f, int full );
	I3_EXPORT_LUA	Proto*		NewProto( lua_State *L );

	I3_EXPORT_LUA	TString*	NewLiteral( lua_State* pState, const char* str );
	I3_EXPORT_LUA	void**		NewVector( lua_State * pState, INT32 n, size_t t);

	I3_EXPORT_LUA	void		IncreaseTop( lua_State * pState );



	//***************************************************************************************************//
	//	i3Lua defined expanded function (기존 lua를 엔진에 맞게 확장한 함수들)
	//***************************************************************************************************//
	// nInitType : i3LuaDefine.h 참조 
	I3_EXPORT_LUA	LuaState *	Create( UINT32 nInitType);
	I3_EXPORT_LUA	void		OpenLibrary( LuaState * pState, UINT32 nInitType);
	I3_EXPORT_LUA	void		Destroy( LuaState * pState );

	//	루아 스크립트에 명령어를 실행한다. (DoString과 동일)
	I3_EXPORT_LUA	bool		DoCommand( LuaState * pState, const char * fmt, ...);

	//	루아 스크립트에 선언된 전역 변수값을 가져온다.
	I3_EXPORT_LUA	LuaNumber	GetVarNumberFromLua( LuaState * pState, const char * pszNumber);
	I3_EXPORT_LUA	INT32		GetVarIntegerFromLua( LuaState * pState, const char * pszInteger);
	I3_EXPORT_LUA	const char *GetVarStringFromLua( LuaState * pState, const char * pszString);
	I3_EXPORT_LUA	LuaCFunction GetFunctionFromLua( LuaState * pState, const char * pszFunction);

	//	루아 스택에 들어 있는 값들을 Debug Output으로 출력한다.
	I3_EXPORT_LUA	void		TraceStack( LuaState * pState);

	//	에러 처리 콜백함수 등록
	I3_EXPORT_LUA	void		SetErrorCallBack( i3LuaUserErrorHandler pCB);

	//	에러를 판단해 에러를 처리한다. (이때 에러 콜백 함수가 등록되 있다면 콜백 함수를 호출한다.)
	I3_EXPORT_LUA	void		ErrorHandler( LuaState * pState, INT32 nErrorCode);

	// 콜스택을 출력하는 함수
	I3_EXPORT_LUA	INT32		TraceCallStack( LuaState * pState);

	//	Lua Function의 참조키를 얻는다.
	I3_EXPORT_LUA	INT32		GetFuncKey( LuaState * pState, const char * pFuncName);

	//	참조키를 이용하여 Lua Function을 실행한다. ( bNoProtected가 true면 Call을, false면 PCall을 호출한다.)
	I3_EXPORT_LUA	void		CallFuncKey1( LuaState * pState, INT32 nFuncKey, LuaNumber val, bool bNoProtected = false);

	//	참조키를 이용하여 Lua Function을 실행한다. ( 인자가 둘이다.)
	I3_EXPORT_LUA	void		CallFuncKey2( LuaState * pState, INT32 nFuncKey, LuaNumber val1, LuaNumber val2, bool bNoProtected = false);


	//***************************************************************************************************//
	//	루아 디버그 창 (Win32 에서만 가능. 기타 플래폼에서는 사용할 수 없습니다.)
	//***************************************************************************************************//

#if defined( I3_DEBUG) 

#if defined( I3_WINDOWS)
	//	디버그 창을 생성 및 초기화
	I3_EXPORT_LUA HWND	StartDebugWin( HINSTANCE hInstance, LuaState * pState, DebugWinInitInfo * pInfo = nullptr);

	I3_EXPORT_LUA void	BindDebugLuaFunction( LuaState * pState);

	//	디버그 창을 제거
	I3_EXPORT_LUA void	StopDebugWin();

#endif

	//	디버그 창에 메시지 출력용 LuaGlue 함수	
	I3_EXPORT_LUA void	DebugWinPrint( const char * pString, ...);		

	I3_EXPORT_LUA	void		RegisterDefaultFunction(LuaState * pState);

#endif	// I3_DEBUG

	// metatable 기능 추가
	// i3Engine에서 사용하는 structure들을 매핑하기 위해서 사용합니다.
	I3_EXPORT_LUA INT32	NewMetaTable( LuaState * pState, const char * pT);

	I3_EXPORT_LUA void	NewTable( LuaState * pState);

	I3_EXPORT_LUA void	SetTable( LuaState * pState, INT32 idx);
	I3_EXPORT_LUA void	GetTable( LuaState * pState, INT32 idx);
	I3_EXPORT_LUA INT32	GetTableSize( LuaState * pState, INT32 idx);

	// stack의 type 체크  
	// pszStr : false이면 출력되는 문자열
	// pState : LuaState
	// idx : stack index
	// type : 체크할 type
	// type2 : 체크할 type2
	// return : !(!type & !type2);	// 둘다 아니라면 false,
	I3_EXPORT_LUA bool	CheckStackType( const char * pszStr, LuaState * pState, INT32 idx, I3LUA_TYPE type, I3LUA_TYPE type2 = I3LUA_NONE);

	// stack의 count 체크
	// pszStr : false이면 출력되는 문자열
	// pState : LuaState
	// count : 확인할 카운트
	// return (StackTop() == count)
	I3_EXPORT_LUA bool	CheckStackCount( const char * pszStr, LuaState * pState, INT32 count);

	// return (pValue != nullptr);
	// false이면 pszStr 출력
	I3_EXPORT_LUA bool	CheckNULLValue( const char * pszStr, void * pValue, INT32 idx);

	// return b;
	// false이면 pszStr 출력
	I3_EXPORT_LUA bool	CheckBool( const char * pszStr, bool b, INT32 idx);

	// meta 체크
	I3_EXPORT_LUA bool	CheckClassMetaType( const char * pszStr, i3ElementBase * pBase, i3ClassMeta * pMeta, INT32 idx);
	I3_EXPORT_LUA bool	CheckClassMetaExactType( const char * pszStr, i3ElementBase * pBase, i3ClassMeta * pMeta, INT32 idx);

	////////////////////////////////////////////////////////////////////////////////////////////////
	//					Lua Function declaration
	I3_EXPORT_LUA	void	ErrorForFunctionCall( LuaState * pState);

	I3_EXPORT_LUA	void	SetMuteErrorMsg( bool bMute);

};

#if defined( I3_DEBUG)
#define I3LUAPRINT(s)	i3Lua::DebugWinPrint(s)
#else
#define I3LUAPRINT(s)	__noop
#endif

#endif //__I3LUA_H


