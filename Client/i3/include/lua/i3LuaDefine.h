#ifndef __I3_LUA_DEFINE_H__
#define __I3_LUA_DEFINE_H__


#define luac_c
//#define LUA_CORE


#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

// 루아 덤프용 헤더
#include "ldo.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstring.h"
#include "lundump.h"


#include "i3Base.h"
#include "i3Math.h"

#if defined( I3_WINDOWS) && defined( I3_DLL)
	#if defined( I3_LUA_BUILD)
		#define	I3_EXPORT_LUA		__declspec( dllexport)
	#else
		#define	I3_EXPORT_LUA		__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_LUA)
		#define	I3_EXPORT_LUA
	#endif
#endif

//	Opaque structure that keeps the whole state of a Lua interpreter
#define LuaState			lua_State
#define LuaNumber			lua_Number
#define LuaInteger			lua_Integer

//	LuaGlue Function Register
#define LuaFuncReg			luaL_reg

//	LuaGlue Function
#define LuaCFunction		lua_CFunction

//	루아 스크립트에서 호출하는 LuaGlue 함수 원형
typedef INT32 (* LuaFunction) (LuaState * pLuaState);


//	루아 초기화 
#define I3LUA_LIB_ALL		0x00000001
#define I3LUA_LIB_BASE		0x00000002
#define I3LUA_LIB_IO		0x00000004
#define I3LUA_LIB_STRING	0x00000008
#define I3LUA_LIB_MATH		0x00000010
#define I3LUA_LIB_DEBUG		0x00000020
#define I3LUA_LIB_TABLE		0x00000040
#define I3LUA_LIB_PACKAGE	0x00000080
#define I3LUA_LIB_OS		0x00000100

//	루아 변수 타입
enum I3LUA_TYPE
{
	I3LUA_NONE			= LUA_TNONE,
	I3LUA_NIL			= LUA_TNIL,
	I3LUA_BOOLEAN		= LUA_TBOOLEAN,
	I3LUA_LIGHTUSERDATA	= LUA_TLIGHTUSERDATA,	//	pointer	( 메모리 주소값만)
	I3LUA_NUMBER		= LUA_TNUMBER,
	I3LUA_STRING		= LUA_TSTRING,
	I3LUA_TABLE			= LUA_TTABLE,
	I3LUA_FUNCTION		= LUA_TFUNCTION,
	I3LUA_USERDATA		= LUA_TUSERDATA,		//	full user data (루아내에 메모리가 할당 객체. 테이블등)
	I3LUA_THREAD		= LUA_TTHREAD,
};

//	루아 GabageCollector 파라메터
enum I3LUA_GC_PARAM
{
	I3LUA_GCSTOP		= LUA_GCSTOP,			//	stops the garbage collector. 
	I3LUA_GCRESTART		= LUA_GCRESTART,		//	restarts the garbage collector. 
	I3LUA_GCCOLLECT		= LUA_GCCOLLECT,		//	performs a full garbage-collection cycle. 
	I3LUA_GCCOUNT		= LUA_GCCOUNT,			//	returns the current amount of memory (in Kbytes) in use by Lua. 
	I3LUA_GCCOUNTB		= LUA_GCCOUNTB,			//	returns the remainder of dividing the current amount of bytes of memory in use by Lua by 1024. 
	I3LUA_GCSTEP		= LUA_GCSTEP,			//	performs an incremental step of garbage collection. The step "size" is controlled by data 
												//	The function returns 1 if the step finished a garbage-collection cycle. 
	I3LUA_GCSETPAUSE	= LUA_GCSETPAUSE,		//	sets data/100 as the new value for the pause of the collector 
	I3LUA_GCSETSTEPMUL	= LUA_GCSETSTEPMUL,		//	sets arg/100 as the new value for the step multiplier of the collector 
};

//	lua error info
struct i3LuaErrorInfo
{
	LuaState *	_pLuaState = nullptr;
	INT32		_nErrorCode = 0;
	/*	lua defined error code. 0 is OK
		LUA_YIELD		1
		LUA_ERRRUN		2
		LUA_ERRSYNTAX	3
		LUA_ERRMEM		4
		LUA_ERRERR		5
	*/
	char *		_pszErrorString = nullptr;

} ;

struct DebugWinInitInfo
{
	INT32	_x = 0;
	INT32	_y = 0;
	INT32	_width = 0;
	INT32	_height = 0;

} ;

//	User Call Back Function
typedef void (* i3LuaUserErrorHandler) ( i3LuaErrorInfo * pInfo);
typedef void (* i3LuaLogCallback)(const char * pszLog);

#endif