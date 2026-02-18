#include "i3LuaPCH.h"
#include "i3Lua.h"
#include "luabind\luabind.hpp"
#include "lua_typed_enums.h"

#include "i3Base/itl/algorithm/transform.h"
#include "i3Base/string/ext/remove_ext.h"

#include "i3Base/string/ext/is_matched_wildcard.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/ext/remove_end_slash.h"

#include "i3Base/string/ext/safe_string_copy.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/utf16_to_utf8.h"
#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "i3Base/string/algorithm/replace_all.h"

#include "i3Base/string/stack_string.h"

//#include <unordered_map>
//#include <unordered_set>

#if defined( I3_WINDOWS)
#include "i3LuaDebugWin.h"
extern i3LuaDebugWin * g_pLuaDebugWin;
#endif

static char s_szLuaErrorString[256];
static char s_strTemp[1024];
static i3LuaUserErrorHandler g_i3ErrorHandlerFunc = nullptr;
static i3LuaLogCallback g_i3LuaLogFunc = nullptr;
static bool	s_bMuteError = false;

//***************************************************************************************************//
//	Main functions
//***************************************************************************************************//
LuaState * i3Lua::Open( void)
{	
	return (LuaState *) lua_open();						
}

void i3Lua::Close( LuaState * pState)	
{	
	lua_close( pState);						
}

LuaState * i3Lua::NewThread( LuaState * pState, void * pKey)
{
	return (LuaState *) lua_newthread( pState);
}

INT32 i3Lua::ThreadResume( LuaState * pState, INT32 arg)
{
	return lua_resume( pState, arg);
}

INT32 i3Lua::ThreadYield( LuaState * pState, INT32 result)
{
	return lua_yield( pState, result);
}

INT32 i3Lua::PCall( LuaState * pState, INT32 arg, INT32 result, INT32 errfunc)
{
	INT32 re = (INT32) lua_pcall( pState, arg, result, errfunc);
	if( re != 0)	   ErrorHandler( pState, re);

	return re;
}

void i3Lua::Call( LuaState * pState, INT32 arg, INT32 result)
{
	lua_call( pState, arg, result);
}

INT32 i3Lua::CPCall( LuaState* pState, LuaFunction func, void * ud)
{
	INT32 re = (INT32) lua_cpcall( pState, func, ud);
	if( re != 0)	   ErrorHandler( pState, re);

	return re;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic stack functions
///////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 i3Lua::StackGetTop( LuaState * pState)
{
	return (INT32) lua_gettop( pState);
}

void i3Lua::StackSetTop( LuaState * pState, INT32 Idx)
{
	lua_settop( pState, Idx);
}

void i3Lua::StackPop( LuaState * pState, INT32 count)
{
	lua_pop( pState, count);
}

void i3Lua::StackPushValue( LuaState * pState, INT32 Idx)
{
	lua_pushvalue( pState, Idx);
}

void i3Lua::StackInsert( LuaState * pState, INT32 Idx)
{
	lua_insert( pState, Idx);
}

void i3Lua::StackRemove( LuaState * pState, INT32 Idx)
{
	lua_remove( pState, Idx);
}

void i3Lua::StackReplace( LuaState * pState, INT32 Idx)
{
	lua_replace( pState, Idx);
}

INT32 i3Lua::StackCheck( LuaState * pState, INT32 nSize)
{
	return (INT32) lua_checkstack( pState, nSize);
}

INT32 i3Lua::StackNext( LuaState * pState, INT32 Idx)
{
	return (INT32) lua_next( pState, Idx);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// reference (Lua Registry)
///////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 i3Lua::Ref( LuaState * pState, bool bVal)
{
	return lua_ref( pState, bVal);
}

void i3Lua::UnRef( LuaState * pState, INT32 ref)
{
	lua_unref( pState, ref);
}

void i3Lua::GetRef( LuaState * pState, INT32 ref)
{
	lua_getref( pState, ref);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Push functions (C++ -> stack)
///////////////////////////////////////////////////////////////////////////////////////////////////////
void i3Lua::PushNumber( LuaState * pState, LuaNumber nVal)
{
	lua_pushnumber( pState, nVal);
}

void i3Lua::PushInteger( LuaState * pState, LuaInteger nVal)
{
	lua_pushinteger( pState, nVal);
}

void i3Lua::PushString( LuaState * pState, const WCHAR16 * pszString)
{
	std::size_t wlen = wcslen(pszString);

	if (wlen == 0)
	{
		lua_pushlstring( pState, "", 0);
		return;
	}

	i3::stack_string str_buff( wlen * 2, 0);		// 2배 정도면 될듯 하다.

	int numResBuff = i3String::WideCharToMultiByte( i3Language::GetCurrentCodePage(), 0, pszString, wlen, &str_buff[0], str_buff.size()+1, 0, 0);

	if (numResBuff == 0)		// 버퍼 부족..
	{
		numResBuff = i3String::WideCharToMultiByte( i3Language::GetCurrentCodePage(), 0, pszString, wlen, nullptr, 0, 0, 0);
		str_buff.resize( numResBuff );
		numResBuff = i3String::WideCharToMultiByte( i3Language::GetCurrentCodePage(), 0, pszString, wlen, &str_buff[0], str_buff.size()+1, 0, 0);
		assert( numResBuff == int(str_buff.size()) );
	}
	else
	{
		str_buff.resize( numResBuff );
	}

	lua_pushlstring( pState, str_buff.c_str(), str_buff.size());
}

void	i3Lua::PushStringUTF16To8( LuaState * pState, const WCHAR16 * pszString)
{
	std::size_t wlen = wcslen(pszString);

	if (wlen == 0)
	{
		lua_pushlstring( pState, "", 0);
		return;
	}

	i3::stack_string str_buff( wlen * 3, 0);		// 한글같은 것 감안해서 감안해서 3배로

	int numResBuff = i3String::WideCharToMultiByte( CP_UTF8, 0, pszString, wlen, &str_buff[0], str_buff.size()+1, 0, 0);
			
	if (numResBuff == 0)		// 버퍼 부족..
	{
		numResBuff = i3String::WideCharToMultiByte( CP_UTF8, 0, pszString, wlen, nullptr, 0, 0, 0);
		str_buff.resize( numResBuff );
		numResBuff = i3String::WideCharToMultiByte( CP_UTF8, 0, pszString, wlen, &str_buff[0], str_buff.size()+1, 0, 0);
		assert( numResBuff == int(str_buff.size()) );
	}
	else
	{
		str_buff.resize( numResBuff );
	}

	lua_pushlstring( pState, str_buff.c_str(), str_buff.size());
}

void	i3Lua::PushString( LuaState * pState, const WCHAR16 * pszString, size_t len)
{
	if (len == 0)
	{
		lua_pushlstring( pState, "", 0);
		return;
	}

	i3::stack_string str_buff( len * 2, 0);

	int numResBuff = i3String::WideCharToMultiByte( i3Language::GetCurrentCodePage(), 0, pszString, len, &str_buff[0], str_buff.size()+1, 0, 0);

	if (numResBuff == 0)		// 버퍼 부족..
	{
		numResBuff = i3String::WideCharToMultiByte( i3Language::GetCurrentCodePage(), 0, pszString, len, nullptr, 0, 0, 0);
		str_buff.resize( numResBuff );
		numResBuff = i3String::WideCharToMultiByte( i3Language::GetCurrentCodePage(), 0, pszString, len, &str_buff[0], str_buff.size()+1, 0, 0);
		assert( numResBuff == int(str_buff.size()) );
	}
	else
	{
		str_buff.resize( numResBuff );
	}

	lua_pushlstring( pState, str_buff.c_str(), str_buff.size());
}

void	i3Lua::PushStringUTF16To8( LuaState* pState, const i3::wliteral_range& rng)
{
	i3Lua::PushStringUTF16To8(pState, rng.c_str(), rng.size());
}

void	i3Lua::PushStringUTF16To8( LuaState * pState, const WCHAR16 * pszString, size_t len)
{
	if (len == 0)
	{
		lua_pushlstring( pState, "", 0);
		return;
	}

	i3::stack_string str_buff( len * 3, 0);

	int numResBuff = i3String::WideCharToMultiByte( CP_UTF8, 0, pszString, len, &str_buff[0], str_buff.size()+1, 0, 0);

	if (numResBuff == 0)		// 버퍼 부족..
	{
		numResBuff = i3String::WideCharToMultiByte( CP_UTF8, 0, pszString, len, nullptr, 0, 0, 0);
		str_buff.resize( numResBuff);
		numResBuff = i3String::WideCharToMultiByte( CP_UTF8, 0, pszString, len, &str_buff[0], str_buff.size()+1, 0, 0);
		assert( numResBuff == int(str_buff.size()) );
	}
	else
	{
		str_buff.resize( numResBuff);
	}

	lua_pushlstring( pState, str_buff.c_str(), str_buff.size());
}

void	i3Lua::PushString( LuaState * pState, const char* pszString, size_t len)
{
	lua_pushlstring( pState, pszString, len);
}


void i3Lua::PushString( LuaState * pState, const char * pszString)
{
	lua_pushstring( pState, pszString);
}

void i3Lua::PushfString( LuaState * pState, const char * pszString, ...)
{
	va_list arg;
	va_start( arg, pszString);

	#if defined( I3_WINDOWS)
		vsprintf_s( s_szLuaErrorString, sizeof(char) * 256, pszString, arg);
	#else
		vsprintf( s_szLuaErrorString, pszString, arg);
	#endif

	va_end( arg);

	lua_pushstring( pState, s_szLuaErrorString);
}

//	루아 스택에 문자열 값을 넣는다 (표준 ansi코드의 문자열을 utf-8로 변환하여 스택에 넣는다)
void i3Lua::PushA2UTF8String( LuaState * pState, const char * pszString)
{
	I3ASSERT( pszString != nullptr);

	WCHAR16 wStr[ 2048] = {0,};
	char szStr[ 2048] = {0,};
	INT32 cLength = i3::generic_string_size( pszString);
	I3ASSERT( cLength < 2048);
	INT32 wLength = i3String::MultiByteToWideChar( i3Language::GetCurrentCodePage(), 0, pszString, cLength, nullptr, 0);

	i3String::MultiByteToWideChar( i3Language::GetCurrentCodePage(), 0, pszString, cLength, wStr, wLength);

	cLength = i3String::WideCharToMultiByte( CP_UTF8, 0, wStr, wLength, 0, 0, 0, 0 );

	i3String::WideCharToMultiByte( CP_UTF8, 0, wStr, wLength, szStr, cLength, 0, 0 ); // UCS-2 to UTF-8

	lua_pushstring( pState, szStr);
}

void i3Lua::PushNil( LuaState * pState)
{
	lua_pushnil( pState);
}

void i3Lua::PushBoolean( LuaState * pState, bool bVal)
{
	lua_pushboolean( pState, bVal ? 1 : 0);
}

void i3Lua::PushUserData( LuaState * pState, void * p)
{
	lua_pushlightuserdata( pState, p);
}

void i3Lua::PushColor( LuaState * pState, const I3COLOR * pColor)
{
	if( pColor != nullptr)
	{
		lua_pushinteger( pState, i3Color::GetR( (COLOR*)pColor));
		lua_pushinteger( pState, i3Color::GetG( (COLOR*)pColor));
		lua_pushinteger( pState, i3Color::GetB( (COLOR*)pColor));
		lua_pushinteger( pState, i3Color::GetA( (COLOR*)pColor));
	}
	else
	{
		lua_pushinteger( pState, 255);
		lua_pushinteger( pState, 255);
		lua_pushinteger( pState, 255);
		lua_pushinteger( pState, 255);
	}
}

void i3Lua::PushCFunction( LuaState * pState, LuaCFunction func)
{
	lua_pushcfunction( pState, func);
}

//***************************************************************************************************//
//	Get functions (stack -> C++)
//***************************************************************************************************//

LuaNumber i3Lua::GetNumberArg( LuaState * pState, INT32 argNum, LuaNumber nDefault)
{
	return luaL_optnumber( pState, argNum, nDefault);
}

INT32 i3Lua::GetIntegerArg( LuaState * pState, INT32 argNum, LuaInteger nDefault)
{
	return (INT32) luaL_optint( pState, argNum, nDefault);
}

UINT32 i3Lua::GetLongArg( LuaState * pState, INT32 argNum, LuaInteger nDefault)
{
	return (UINT32) luaL_optlong( pState, argNum, nDefault);
}

//If argNum is absent of nil, copies szDefault to pBuf and returns nullptr
const char* i3Lua::GetStringArg( LuaState * pState, INT32 argNum, char * pBuf, INT32 nBufSize, const char * szDefault)
{	
	const char* pszString = luaL_optstring( pState, argNum, szDefault);

	if( pBuf != nullptr)
	{
		pBuf[0] = 0;

		if( pszString != nullptr)
		{
			i3::string_ncopy_nullpad( pBuf, pszString, nBufSize);
		}
		else if( szDefault != nullptr)
		{
			i3::string_ncopy_nullpad( pBuf, szDefault, nBufSize);
		}
	}

	return pszString;
}


I3_EXPORT_LUA bool i3Lua::GetBooleanArg( LuaState * pState, INT32 argNum, bool bDefault /*= false*/ )
{
	return luaL_optbool( pState, argNum, bDefault ? 1 : 0) != 0;
}


void * i3Lua::GetUserDataArg( LuaState * pState, INT32 argNum)
{
	return lua_touserdata( pState, argNum);
}

LuaNumber i3Lua::CheckNumberArg( LuaState * pState, INT32 argNum)
{
	return luaL_checknumber( pState, argNum);
}

INT32 i3Lua::CheckIntegerArg( LuaState * pState, INT32 argNum)
{
	return luaL_checkint( pState, argNum);
}

UINT32 i3Lua::CheckLongArg( LuaState * pState, INT32 argNum)
{
	return (UINT32) luaL_checklong( pState, argNum);
}

const char * i3Lua::CheckStringArg( LuaState * pState, INT32 argNum)
{
	return luaL_checkstring( pState, argNum);
}

I3LUA_TYPE i3Lua::StackIsType( LuaState * pState, INT32 Idx)
{
	return (I3LUA_TYPE) lua_type( pState, Idx);
}

bool i3Lua::StackIsNumber( LuaState * pState, INT32 Idx)
{	
	return lua_isnumber(pState, Idx) ? true : false;
}

bool i3Lua::StackIsString( LuaState * pState, INT32 Idx)
{	
	return lua_isstring( pState, Idx) ? true : false;		
}

bool i3Lua::StackIsFunction( LuaState * pState, INT32 Idx)
{
	return lua_isfunction( pState, Idx) ? true : false;
}

bool i3Lua::StackIsCFunction( LuaState * pState, INT32 Idx)
{	
	return lua_iscfunction( pState, Idx) ? true : false;	
}

bool i3Lua::StackIsUserData( LuaState * pState, INT32 Idx)
{	
	return lua_isuserdata(pState, Idx) ? true : false;
}

bool i3Lua::StackIsEqual( LuaState * pState, INT32 Idx1, INT32 Idx2)
{	
	return lua_equal( pState, Idx1, Idx2) ? true : false;	
}

bool i3Lua::StackIsLessthan( LuaState * pState, INT32 Idx1, INT32 Idx2)
{
	return lua_lessthan( pState, Idx1, Idx2) ? true : false;
}

const char * i3Lua::StackTypeName( LuaState * pState, I3LUA_TYPE type)
{
	return lua_typename( pState, (INT32) type);
}

LuaNumber i3Lua::StackToNumber( LuaState * pState, INT32 Idx)
{
	return lua_tonumber( pState, Idx);
}

INT32 i3Lua::StackToInteger( LuaState * pState, INT32 Idx)
{
	return lua_tointeger( pState, Idx);			
}

bool i3Lua::StackToBoolean( LuaState * pState, INT32 Idx)
{	
	return lua_toboolean(pState, Idx) ? true : false;
}

const char* i3Lua::StackToString( LuaState * pState, INT32 Idx)	
{	
	return lua_tostring( pState, Idx);			
}

INT32 i3Lua::StackToStrlen( LuaState * pState, INT32 Idx)	
{	
	return (INT32) lua_strlen( pState, Idx);	
}

LuaCFunction i3Lua::StackToCFunction( LuaState * pState, INT32 Idx)	
{	
	return lua_tocfunction( pState, Idx);			
}

void * i3Lua::StackToUserData( LuaState * pState, INT32 Idx)	
{	
	return lua_touserdata( pState, Idx);			
}

LuaState * i3Lua::StackToThread( LuaState * pState, INT32 Idx)	
{	
	return lua_tothread( pState, Idx);			
}

const void * i3Lua::StackToPointer( LuaState * pState, INT32 Idx)	
{	
	return lua_topointer( pState, Idx);			
}

//***************************************************************************************************//
//	Get functions (lua -> stack)
//***************************************************************************************************//
void i3Lua::GetGlobal( LuaState * pState, const char * s)
{
	lua_getglobal( pState, s);
}

void i3Lua::GetField( LuaState* pState, INT32 idx, const wchar_t * k)
{
//	char conv[MAX_PATH * 4];
	i3::string conv;

//	i3String::UCS2toUTF8( conv, k, sizeof( conv));
	i3::utf16_to_utf8(k, conv);

	lua_getfield( pState, idx, conv.c_str());
}

void i3Lua::Rawget( LuaState * pState, INT32 Idx)
{
	lua_rawget( pState, Idx);
}

void i3Lua::Rawgeti( LuaState * pState, INT32 idx, INT32 n)
{
	lua_rawgeti( pState, idx, n);
}

void * i3Lua::NewUserData( LuaState * pState, size_t nSize)
{
	return lua_newuserdata( pState, nSize);
}

//***************************************************************************************************//
//	Auxiliary functions
//***************************************************************************************************//
INT32 i3Lua::LoadFile( LuaState * pState, const char * pszFileName)
{
	INT32 re = (INT32) luaL_loadfile( pState, pszFileName);
	if( re != 0)	   ErrorHandler( pState, re);

	return re;
}

INT32 i3Lua::LoadBuffer( LuaState * pState, const char * pBuf, size_t nSize, const char * pName)
{
#if defined( I3_DEBUG)
	INT32 re = (INT32) luaL_loadbuffer( pState, pBuf, nSize, pName);
#else
	INT32 re = (INT32) luaL_loadbuffer( pState, pBuf, nSize, nullptr);
#endif

	if( re != 0)	   ErrorHandler( pState, re);

	return re;
}

INT32 i3Lua::LoadString( LuaState * pState, const char * pszString)
{
#if defined( I3_DEBUG)
	INT32 re = (INT32) luaL_loadbuffer( pState, pszString, strlen( pszString), pszString);
#else
	INT32 re = (INT32) luaL_loadbuffer( pState, pszString, strlen( pszString), nullptr);
#endif
	if( re != 0)	   ErrorHandler( pState, re);

	return re;
}

INT32 i3Lua::DoFile( LuaState * pState, const char * pszFileName)
{
	INT32 re = (INT32) luaL_dofile( pState, pszFileName);
	if( re != 0)	   ErrorHandler( pState, re);

	return re;
}

INT32 i3Lua::DoString( LuaState * pState, const char * pszString)
{
	INT32 re = (INT32) luaL_dostring( pState, pszString);
	if( re != 0)	   ErrorHandler( pState, re);

	return re;
}


//***************************************************************************************************//
//	Library functions
//***************************************************************************************************//
INT32 i3Lua::LibOpenBase( LuaState * pState)
{
	return luaopen_base( pState);
}

INT32 i3Lua::LibOpenIo( LuaState * pState)
{
	// somehow not working 
	// Look: LibOpenPackage
	return luaopen_io( pState);	
}

INT32 i3Lua::LibOpenString( LuaState * pState)
{
	return luaopen_string( pState);
}

INT32 i3Lua::LibOpenMath( LuaState * pState)
{
	return luaopen_math( pState);
}

INT32 i3Lua::LibOpenDebug( LuaState * pState)
{
	#if defined( I3_WINDOWS)
		return luaopen_debug( pState);
	#else
		return 0;
	#endif
}

INT32 i3Lua::LibOpenTable( LuaState * pState)
{
	return luaopen_table( pState);
}

INT32 i3Lua::LibOpenPackage( LuaState * pState)
{
	// not working 
	// http://www.gamedev.net/community/forums/topic.asp?topic_id=387007
	return luaopen_package( pState); 
}

INT32 i3Lua::LibOpenOS( LuaState * pState)
{
	return  luaopen_os( pState);
}

INT32 i3Lua::LibOpenStd( LuaState * pState)
{
	INT32 re = 0;

	luaL_openlibs(pState);

	luabind::open(pState);
//
//	re = LibOpenBase( pState);
//	re = LibOpenPackage( pState);
//	re = LibOpenString( pState);
//	re = LibOpenMath( pState);
//	re = LibOpenTable( pState);
//	re = LibOpenOS( pState);
//
////#if defined( I3_DEBUG)
//	re = LibOpenDebug( pState);
////W#endif
//
	return re;
}	

void i3Lua::LibOpenLibs(LuaState* pState)
{
	luaL_openlibs(pState);
}

//***************************************************************************************************//
//	useful macros
//***************************************************************************************************//

void i3Lua::RegisterFunc( LuaState * pState, const char * pFuncName, LuaFunction pFunction)
{
	lua_register( pState, pFuncName, pFunction);
}

void	i3Lua::RegisterFuncs( LuaState * pState, const char * pLibName, LuaFuncReg * pLibs)
{
	luaL_openlib( pState, pLibName, pLibs, 0);
}

const char * i3Lua::GetErrorString( LuaState * pState)
{
	return luaL_checkstring( pState, -1);
}

void i3Lua::Error( LuaState * pState)
{
	lua_error( pState);
}

void i3Lua::SetMuteErrorMsg( bool bMute)
{
	s_bMuteError = bMute;
}

INT32 i3Lua::SetError(LuaState * pState, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);

	#if defined( I3_WINDOWS)
		vsprintf_s(s_szLuaErrorString, sizeof(char)*256, fmt, argp);
	#else
		vsprintf(s_szLuaErrorString, fmt, argp);
	#endif

	va_end(argp);

	if( s_bMuteError)
		return 0;

	//I3TRACE((const char*) s_szLuaErrorString);
	//I3PRINTLOG(I3LOG_FATAL,  s_szLuaErrorString);

	return luaL_error( pState, s_szLuaErrorString);
}

INT32 i3Lua::SetError(LuaState * pState, const wchar_t * fmt, ...)
{
	wchar_t szwBuf[MAX_PATH];

	va_list argp;
	va_start(argp, fmt);

	#if defined( I3_WINDOWS)
		vswprintf_s( szwBuf, _countof( szwBuf), fmt, argp);
	#else
		vswprintf(s_szLuaErrorString, fmt, argp);
	#endif

	va_end(argp);

	if( s_bMuteError)
		return 0;

//	char szBuf[MAX_PATH*4];
//	i3String::UCS2toUTF8( szBuf, szwBuf, sizeof( szBuf));
//	I3TRACE( szBuf);
	i3::string strBuf;
	i3::utf16_to_utf8(szwBuf, strBuf);
	return luaL_error( pState, strBuf.c_str());
}

const char * i3Lua::GetLuaVersion( void)
{
	return LUA_VERSION;
}

///////////////////////////////////////////////////////////////////////////////////
// State에 enum값을 넣는다.
bool i3Lua::AddEnumToLua(LuaState* L, const char* tname, ...)			
{
	va_list args;
    std::stringstream code;
    char* ename;
    int evalue;
    
    code << tname << " = setmetatable({}, {";
    code << "__index = {";

    // Iterate over the variadic arguments adding the enum values.
    va_start(args, tname);
    while ((ename = va_arg(args, char*)) != 0)
    {
        evalue = va_arg(args, int);
        code << ename << "={value=" << evalue << ",type=\"" << tname << "\"},";
    } 
    va_end(args);

    code << "},";
    code << "__newindex = function(table, key, value) error(\"Attempt to modify read-only table\") end,";
    code << "__metatable = false});";

    // Execute lua code
    if ( luaL_loadbuffer(L, code.str().c_str(), code.str().length(), 0) || lua_pcall(L, 0, 0, 0) )
    {
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }
    return true;
}

// 해당 enum값이 동일 type의 enum값인지 확인한다
bool i3Lua::CheckEnumType(LuaState* L, const char* tname, int index)
{
	return check_enum_type(L, tname, index);
}
// enum값을 정수형으로 얻어온다.
INT32 i3Lua::GetEnumValue(LuaState* L, int index)
{
	return get_enum_value(L, index);
}

void i3Lua::SetLogCallBack( i3LuaLogCallback cb)
{
	g_i3LuaLogFunc = cb;
}

INT32 func_panic(lua_State * pState)
{
	const char * pszStr = lua_tostring(pState, 1);

	i3Lua::Log( pszStr);

	lua_getglobal(pState, "ERROR");

	throw std::exception( pszStr);
	
//	return 0;			// 쓰로우 행동 때문에 닿지 않으며, 리턴없이도 컴파일 가능 (2012.04.09.수빈)
}

//	log
void i3Lua::Log(const char * fmt, ...)
{
	if( g_i3LuaLogFunc != nullptr)	
	{
		va_list argp;
		va_start(argp, fmt);
		vsprintf( s_strTemp, fmt, argp);
		va_end(argp);

		g_i3LuaLogFunc( s_strTemp);	

		I3TRACE( s_strTemp);
	}
}

//***************************************************************************************************//
//	garbage-collection function
//***************************************************************************************************//
INT32 i3Lua::GabageCollector( LuaState * pState, I3LUA_GC_PARAM param, INT32 data)
{
	return lua_gc( pState, (INT32) param, data);
}




//***************************************************************************************************//
//	dump function
//***************************************************************************************************//
INT32 i3Lua::Dump( lua_State* pState, const Proto* f, lua_Writer w, void* data, INT32 strip)
{
	INT32	re	= 0;

	lua_lock(L);
	re	= luaU_dump( pState, f, w, data, strip );
	lua_unlock(L);

	return re;
}

void i3Lua::UPrint( const Proto* f, int full )
{
	luaU_print( f, full );
}

Proto* i3Lua::NewProto( lua_State* pState )
{
	return luaF_newproto( pState );
}

TString* i3Lua::NewLiteral( lua_State* pState, const char* str )
{
	return luaS_newlstr( pState, str, i3::generic_string_size( str) - 1 );
}

void** i3Lua::NewVector( lua_State * pState, INT32 n, size_t t)
{
	return (void**)luaM_reallocv( pState, nullptr, 0, n, t );
}

void i3Lua::IncreaseTop( lua_State * pState )
{
	incr_top( pState );
}


//***************************************************************************************************//
//	i3Lua defined expanded function
//***************************************************************************************************//

LuaState * i3Lua::Create( UINT32 nInitType)
{
	//	create lua instance
	LuaState * L = Open();

	LibOpenStd( L);

	lua_atpanic( L, func_panic);

#if defined( I3_DEBUG)
	RegisterDefaultFunction( L);
#endif

/*
	//	lua library load
	if( nInitType & I3LUA_LIB_BASE || nInitType & I3LUA_LIB_ALL)			LibOpenBase( L);
	if( nInitType & I3LUA_LIB_IO || nInitType & I3LUA_LIB_ALL)			LibOpenIo( L);
	if( nInitType & I3LUA_LIB_STRING || nInitType & I3LUA_LIB_ALL)		LibOpenString( L);
	if( nInitType & I3LUA_LIB_MATH || nInitType & I3LUA_LIB_ALL)		LibOpenMath( L);
	if( nInitType & I3LUA_LIB_DEBUG || nInitType & I3LUA_LIB_ALL)		LibOpenDebug( L);
	if( nInitType & I3LUA_LIB_TABLE || nInitType & I3LUA_LIB_ALL)		LibOpenTable( L);
	if( nInitType & I3LUA_LIB_PACKAGE || nInitType & I3LUA_LIB_ALL)		LibOpenPackage( L);
	if( nInitType & I3LUA_LIB_OS || nInitType & I3LUA_LIB_ALL)			LibOpenOS( L);
*/
	return L;
}

void i3Lua::OpenLibrary( LuaState * pState, UINT32 nInitType)
{
	if( nInitType & I3LUA_LIB_BASE )		LibOpenBase( pState);
	if( nInitType & I3LUA_LIB_IO )			LibOpenIo( pState);
	if( nInitType & I3LUA_LIB_STRING )		LibOpenString( pState);
	if( nInitType & I3LUA_LIB_MATH )		LibOpenMath( pState);
	if( nInitType & I3LUA_LIB_DEBUG )		LibOpenDebug( pState);
	if( nInitType & I3LUA_LIB_TABLE )		LibOpenTable( pState);
	if( nInitType & I3LUA_LIB_PACKAGE )		LibOpenPackage( pState);
	if( nInitType & I3LUA_LIB_OS )			LibOpenOS( pState);
}

void i3Lua::Destroy( LuaState * pState)
{
	i3Lua::Close( pState);
}

void i3Lua::SetErrorCallBack( i3LuaUserErrorHandler pCB)
{
	g_i3ErrorHandlerFunc = pCB;
}

void i3Lua::ErrorHandler( LuaState * pState, INT32 nErrorCode)
{
	if( nErrorCode != 0)	//	0 is OK
	{
		char szError[1024];
		const char * pszErrorString = GetErrorString( pState);

		switch( nErrorCode)
		{
		case LUA_YIELD:	
			i3::snprintf( szError, 1024, "[lua yield error] %s\n", pszErrorString);	
			break;
		case LUA_ERRRUN:	
			i3::snprintf( szError, 1024, "[lua runtime error] %s\n", pszErrorString);	
			break;
		case LUA_ERRSYNTAX:	
			i3::snprintf( szError, 1024, "[lua syntax error] %s\n", pszErrorString);	
			break;		
		case LUA_ERRMEM:	
			i3::snprintf( szError, 1024, "[lua memory error] %s\n", pszErrorString);	
			break;
		case LUA_ERRERR:	
			i3::snprintf( szError, 1024, "[lua error handler] %s\n", pszErrorString);	
			break;
		default:
			szError[0] = 0;
			break;
		}		
/*
		if( g_pLuaDebugWin)	//	디버그 창이 있다면 디버그창에 메시지 출력
		{			
			DebugWinPrint( szError);
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  szError);
		}
*/
		Log( szError);

		I3LUA_PRINT( (const char*) szError);

		TraceStack( pState);
		//I3PRINTLOG(I3LOG_FATAL,  szError);

		//	등록된 에러 처리 콜백 함수 호출
		if( g_i3ErrorHandlerFunc != nullptr)	
		{
			i3LuaErrorInfo ErrorInfo;	//	에러 정보를 담는다.
			ErrorInfo._pLuaState = pState;
			ErrorInfo._nErrorCode = nErrorCode;
			ErrorInfo._pszErrorString = szError;

			//	등록된 콜백함수에 에러 정보를 담아 호출한다.
			g_i3ErrorHandlerFunc( &ErrorInfo);	
		}
	}
}

bool i3Lua::DoCommand( LuaState * pState, const char * fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	
	#if defined( I3_WINDOWS)
		vsprintf_s( s_szLuaErrorString, sizeof(char)*256, fmt, argp);
	#else
		vsprintf( s_szLuaErrorString, fmt, argp);
	#endif

	va_end(argp);

	return ( DoString( pState, s_szLuaErrorString) ? true: false);
}

LuaNumber i3Lua::GetVarNumberFromLua( LuaState * pState, const char * pszNumber)
{
	PushString( pState, pszNumber);
	Rawget( pState, LUA_GLOBALSINDEX);

	return StackToNumber( pState, -1);
}

INT32 i3Lua::GetVarIntegerFromLua( LuaState * pState, const char * pszInteger)
{
	PushString( pState, pszInteger);
	Rawget( pState, LUA_GLOBALSINDEX);

	return StackToInteger( pState, -1);
}

LuaCFunction i3Lua::GetFunctionFromLua( LuaState * pState, const char * pszFunction)
{
	PushString( pState, pszFunction);
	Rawget( pState, LUA_GLOBALSINDEX);

	return StackToCFunction( pState, -1);
}

const char * i3Lua::GetVarStringFromLua( LuaState * pState, const char * pszString)
{
	PushString( pState, pszString);
	Rawget( pState, LUA_GLOBALSINDEX);

	return StackToString( pState, -1);
}

void i3Lua::TraceStack( LuaState * pState)
{
	INT32 i = 0;
	INT32 count = StackGetTop( pState);

	I3TRACE( "------------------------------------\n");
	I3TRACE( "Lua Stack top : %d\n", count);
	for ( i = 1; i <= count; i++)
	{
		I3LUA_TYPE type = StackIsType( pState, i);
		switch( type)
		{
		case I3LUA_NONE:		
		case I3LUA_NIL:		
		case I3LUA_TABLE:
		case I3LUA_LIGHTUSERDATA:
		case I3LUA_FUNCTION:
		case I3LUA_USERDATA:
		case I3LUA_THREAD:			I3TRACE( "%d: %s\n", i, StackTypeName( pState, type));		break;
		case I3LUA_BOOLEAN:			I3TRACE( "%d: %s = %d\n", i, StackTypeName( pState, type), (INT32) StackToBoolean( pState, i));	break;
		case I3LUA_NUMBER:			I3TRACE( "%d: %s = %d\n", i, StackTypeName( pState, type), (INT32) StackToNumber( pState, i));		break;
		case I3LUA_STRING:			I3TRACE( "%d: %s = %s\n", i, StackTypeName( pState, type), StackToString( pState, i));		break;
		}
	}
}

INT32 i3Lua::GetFuncKey( LuaState * pState, const char * pFuncName)
{
	if( pFuncName)
		GetGlobal( pState, pFuncName);

	return Ref( pState, 1);		//	참조값을 반환한다.
}

void i3Lua::CallFuncKey1( LuaState * pState, INT32 nFuncKey, LuaNumber val, bool bNoProtected)
{
	GetRef( pState, nFuncKey);		//	참조값을 스택에 넣는다.
	PushNumber( pState, val);		//	매개변수를 넣는다.

	if( bNoProtected)
		Call( pState, 1, 0);		//	Normal Call - 1 Parameter, No return		
	else
		PCall( pState, 1, 0, 0);	//	Protected Call - 1 Parameter, No return		
}

void i3Lua::CallFuncKey2( LuaState * pState, INT32 nFuncKey, LuaNumber val1, LuaNumber val2, bool bNoProtected)
{
	GetRef( pState, nFuncKey);		//	참조값을 스택에 넣는다.
	PushNumber( pState, val1);		//	첫번째 매개변수를 넣는다.
	PushNumber( pState, val2);		//	두번째 매개변수를 넣는다.

	if( bNoProtected)
		Call( pState, 2, 0);		//	Normal Call - 2 Parameter, No return		
	else
		PCall( pState, 2, 0, 0);	//	Protected Call - 2 Parameter, No return		
}


INT32 i3Lua::NewMetaTable( LuaState * pState, const char * pT)
{
	return luaL_newmetatable( pState, pT);
}

void i3Lua::NewTable( LuaState * pState)
{
	lua_newtable( pState);
}

void i3Lua::SetTable( LuaState * pState, INT32 idx)
{
	lua_settable( pState, idx);
}

void i3Lua::GetTable( LuaState * pState, INT32 idx)
{
	lua_gettable( pState, idx);
}

INT32 i3Lua::GetTableSize( LuaState * pState, INT32 idx)
{
	return static_cast<INT32>(lua_objlen( pState, idx));
}


// stack의 type 체크  
// pszStr : false이면 출력되는 문자열
// pState : LuaState
// idx : stack index
// type : 체크할 type
// type2 : 체크할 type2
// return : !(!type & !type2);	// 둘다 아니라면 false,
bool i3Lua::CheckStackType( const char * pszStr, LuaState * pState, INT32 idx, I3LUA_TYPE type, I3LUA_TYPE type2)
{
	I3LUA_TYPE stackType = i3Lua::StackIsType( pState, idx);
	if( (stackType != type) && (stackType != type2) )
	{	
		I3LUA_PRINT( "warning %s invalid arg(%d) type. = %s", pszStr, idx, StackTypeName( pState, stackType) );
		return false;
	}
	return true;
}

// stack의 count 체크
// pszStr : false이면 출력되는 문자열
// pState : LuaState
// count : 확인할 카운트
// return (StackTop() == count)
bool i3Lua::CheckStackCount( const char * pszStr, LuaState * pState, INT32 count)
{
	if( StackGetTop( pState) != count)
	{
		I3LUA_PRINT( "warning %s invalid arg count. %d", pszStr, StackGetTop( pState));
		return false;
	}
	return true;
}

bool i3Lua::CheckNULLValue( const char * pszStr, void * pValue, INT32 idx)
{
	if( pValue == nullptr)
	{
		I3LUA_PRINT( "warning %s arg(%d) is nullptr.", pszStr, idx);
		return false;
	}
	return true;
}

bool i3Lua::CheckBool( const char * pszStr, bool b, INT32 idx)
{
	if( b == false)
	{
		I3LUA_PRINT( "warning %s arg(%d) bool = false.", pszStr, idx);
		return false;
	}
	return true;
}

bool i3Lua::CheckClassMetaType( const char * pszStr, i3ElementBase * pBase, i3ClassMeta * pMeta, INT32 idx)
{
	I3ASSERT( pBase != nullptr);
	I3ASSERT( pMeta != nullptr);

	if( pBase->kind_of( pMeta) == false)
	{
		I3LUA_PRINT( "warning %s arg(%d) invalid ClassMeta..", pszStr, idx);
		return false;
	}

	return true;
}

bool i3Lua::CheckClassMetaExactType( const char * pszStr, i3ElementBase * pBase, i3ClassMeta * pMeta, INT32 idx)
{
	I3ASSERT( pBase != nullptr);
	I3ASSERT( pMeta != nullptr);

	if( pBase->same_of( pMeta) == false)
	{
		I3LUA_PRINT( "warning %s arg(%d) invalid ExactClassMeta..", pszStr, idx);
		return false;
	}

	return true;
}


//***************************************************************************************************//
//	루아 디버그 창 (Win32 에서만 가능. 기타 플래폼에서는 사용할 수 없습니다.)
//***************************************************************************************************//
#if defined( I3_DEBUG)

static INT32 Debug_Print(LuaState *L) 
{
#if defined( I3_WINDOWS)
  INT32 n = i3Lua::StackGetTop(L);  /* number of arguments */
  INT32 i;

  i3Lua::GetGlobal(L, "tostring");
  for (i=1; i<=n; i++) 
  {
    const char *s;

	i3Lua::StackPushValue(L, -1);  /* function to be called */
    i3Lua::StackPushValue(L, i);   /* value to print */
	i3Lua::Call(L, 1, 1);

	s = i3Lua::StackToString(L, -1);  /* get result */
    if (s == nullptr)
		return i3Lua::SetError(L, "`tostring' must return a string to `print'");

	if (i>1) i3LuaDebugWin::Write("\t");

    i3LuaDebugWin::Write(s);
	I3TRACE( s);
	i3Lua::StackPop(L, 1);  /* pop result */
  }
  i3LuaDebugWin::Write("\n");
#endif	// I3_WINDOWS

  return 0;
}

LuaFuncReg DebugGlue[] = 
{
  {"Print",					Debug_Print},
  {"print",					Debug_Print},

  {nullptr, nullptr}
};

void i3Lua::RegisterDefaultFunction(LuaState * pState)
{
	for(int i=0; DebugGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, DebugGlue[i].name, DebugGlue[i].func);
	}
}

#if defined( I3_WINDOWS)
HWND i3Lua::StartDebugWin( HINSTANCE hInstance, LuaState * pState, DebugWinInitInfo * pInfo)
{
	if( !g_pLuaDebugWin)
		g_pLuaDebugWin = i3LuaDebugWin::new_object();

	if(!g_pLuaDebugWin->GetHwnd())
		g_pLuaDebugWin->Init( hInstance, pInfo);	//	디버그 창 초기화

	// Display to debug window
	i3Lua::DoString( pState, "print( string.format( \"Debug starting from %s!\", _VERSION))");
	i3Lua::DoString( pState, "print( \"-------------------------------------------------------- \")");

	return ( g_pLuaDebugWin->GetHwnd());
}

void i3Lua::BindDebugLuaFunction( LuaState * pState)
{
	if( g_pLuaDebugWin != nullptr)
	{
		g_pLuaDebugWin->SetLuaState( pState);

		// init the glue functions required for the debug window
		for(int i=0; DebugGlue[i].name; i++)
		{
			i3Lua::RegisterFunc( g_pLuaDebugWin->GetLuaState(), DebugGlue[i].name, DebugGlue[i].func);
		}
	}
}

void i3Lua::StopDebugWin()
{
#if defined( I3_WINDOWS)
	I3_SAFE_RELEASE( g_pLuaDebugWin);
#endif
}

#endif

void i3Lua::DebugWinPrint( const char * pString, ...)
{	
	#if defined( I3_WINDOWS)
	if( g_pLuaDebugWin == nullptr)
		return;
	#endif

	va_list arg;
	va_start( arg, pString);
	vsprintf( s_strTemp, pString, arg);
	va_end( arg);

	#if defined( I3_WINDOWS)
		vsprintf_s( s_szLuaErrorString, sizeof(char)*256, pString, arg);
	#else
		vsprintf( s_szLuaErrorString, pString, arg);
	#endif

	va_end( arg);

	i3::generic_string_cat( s_szLuaErrorString, "\n");

	#if defined( I3_WINDOWS)
	i3LuaDebugWin::Write( s_szLuaErrorString);
	#endif

	I3TRACE( s_szLuaErrorString);
}

#endif	// I3_DEBUG

void i3Lua::ErrorForFunctionCall( LuaState * pState)
{
	char szErrorCall[512];
	
	sprintf_s( szErrorCall, 512, "function function_event (func, ...)\n");
	sprintf_s( szErrorCall, 512, "%s if type(func) == \"function\" then\n", szErrorCall);
	sprintf_s( szErrorCall, 512, "%s return func(unpack(arg)) -- 일반적인 호출\n", szErrorCall);
	sprintf_s( szErrorCall, 512, "%s else\n", szErrorCall);
	sprintf_s( szErrorCall, 512, "%s local h = metatable(func).__call\n", szErrorCall);
	sprintf_s( szErrorCall, 512, "%s if h then\n", szErrorCall);
	sprintf_s( szErrorCall, 512, "%s tinsert(arg, 1, func)\n", szErrorCall);
	sprintf_s( szErrorCall, 512, "%s return h(unpack(arg))\n", szErrorCall);
	sprintf_s( szErrorCall, 512, "%s else\n", szErrorCall);
	sprintf_s( szErrorCall, 512, "%s error(\"함수가 아닌 값에 호출을 실행했습니다.\")\n", szErrorCall);
	sprintf_s( szErrorCall, 512, "%s end\n", szErrorCall);
	sprintf_s( szErrorCall, 512, "%s end\n", szErrorCall);
	sprintf_s( szErrorCall, 512, "%s end\n", szErrorCall);

	DoString( pState, szErrorCall);
}

INT32 i3Lua::TraceCallStack(lua_State* L)
{
  lua_getfield(L, LUA_GLOBALSINDEX, "debug");
  if (!lua_istable(L, -1)) {
       lua_pop(L, 1);
       return 1;
  }

  lua_getfield(L, -1, "traceback");// _TRACEBACK은 전역 함수로서 콜스택을 출력하는 함수다.
  if (!lua_isfunction(L, -1)) {
       lua_pop(L, 2);
       return 1;
  }

  lua_pushvalue(L, 1); // pass error message
  lua_pushinteger(L, 2); // skip this function and traceback
  lua_call(L, 2, 1);     // call debug.traceback
  return 1;
}

namespace
{

	int lua_dump_custom(lua_State *L, lua_Writer writer, void *data) 
	{
		int status = 1;
		lua_lock(L);
		TValue * o = L->top - 1;
		if (isLfunction(o))
			status = luaU_dump(L, clvalue(o)->l.p, writer, data, 1);			// 0 -> 1... 스트립 옵션..
		lua_unlock(L);
		return status;
	}


	int ldump_writer(lua_State *, const void *b, size_t size, void *B)
	{
		luaL_addlstring((luaL_Buffer *) B, (const char *) b, size);
		return 0;
	}

	bool __GetCompileBuffer_IfLoaded( LuaState* L, const char * pszFileName, i3::vector<BYTE>& out, int top )
	{
		bool result = false;

		luaL_Buffer b;
		luaL_buffinit( L, &b);
		lua_dump_custom( L, ldump_writer, &b);
		luaL_pushresult( &b);

		size_t buff_len; 
		const char* szBuff = lua_tolstring(L, -1, &buff_len );

		if (buff_len > 0)
		{
			out.resize(buff_len);
			i3::copy(szBuff, szBuff + buff_len, out.begin());
			I3TRACE(" PRECOMPILE: '%s' -> %d bytes\n", pszFileName, buff_len);
			result = true;
		}
		else
		{
			out.clear();
			I3TRACE("luaScriptCompile - 0 FileSize :: %d - %s\n", __LINE__, pszFileName);
		}

		lua_settop( L, top);

		return result;
	}

}


bool	 i3Lua::GetCompiledBuffer( LuaState* pState, const char * pszFileName, i3::vector<BYTE>& out)
{
	int top = lua_gettop( pState);
	
	if (luaL_loadfile( pState, pszFileName) != 0)
	{
		out.clear();
		I3TRACE("luaScriptCompile :: %d - %s: %s\n", __LINE__, pszFileName, lua_tostring(pState, -1) );
		lua_pop(pState, 1);
		return false;
	}
	
	return __GetCompileBuffer_IfLoaded(pState, pszFileName, out, top);
}

bool	 i3Lua::GetCompiledBuffer( LuaState* pState, const char* pBuf, size_t nSize, const char * pName, i3::vector<BYTE>& out)
{
	int top = lua_gettop( pState);
	
	if (luaL_loadbuffer(pState, pBuf, nSize, pName) != 0 )
	{
		out.clear();
		I3TRACE("luaScriptCompile :: %d - %s: %s\n", __LINE__, pName, lua_tostring(pState, -1) );
		lua_pop(pState, 1);
		return false;
	}
	return __GetCompileBuffer_IfLoaded(pState, pName, out, top);
}

namespace
{
	//
	// 복사를 조금 줄이려면 vector.swap이나 swap()을 쓰는게 좋을 것이다...
	//
	// .lua와 .luab는 같은 키를 유지하는게 좋기 때문에 키파일에서 확장자를 뗀다...
	//
	//

	typedef i3::unordered_map< i3::string, i3::vector<BYTE> >	preload_map_type;
	preload_map_type	g_lua_preload_map;		// 안지운다...당분간은 프로그램 종료시까지 안지울것이다..
	//i3::vector< i3::string > g_vec_require;	//	필수 리콰이어의 파일명을 모음...
	typedef i3::unordered_set< i3::string> require_set_type;
	require_set_type	g_lua_require_set;


	// 위 함수와 비슷하지만, 귀찮게 콜백타지 않고, 해당경로의 파일들만 잘 모셔둠... (귀찮지 않게 i3사용)
	// 리턴값은 파일의 갯수임.. 서브디렉토리검색은 옵션인데, 디렉토리명자체는 파일로 간주하지 않는다..
	// 2012.02.14.수빈..

	INT32 _Rec_GatherFiles_System( const char* pszPath, const char* pszFilter, i3::vector<i3::string>& outFiles, bool bSubDirSearch)
	{
		INT32 nCount = 0;
		HANDLE hFind;
		char _TempPath[MAX_PATH];
		WIN32_FIND_DATA wfd;

		_snprintf( _TempPath, MAX_PATH, "%s/%s", pszPath, pszFilter);
		hFind = ::FindFirstFile( _TempPath, &wfd);
		if( hFind != INVALID_HANDLE_VALUE)
		{
			do 
			{
				if ( !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )		// 디렉토리 자체에는 관심 없음..
				{
// 의외의 복병인데, 와일드카드의 경우.. *.lua를 필터조건을 걸어도 *.luab가 통과되기 때문에, 이걸 재설정..
				//	if ( i3String::IsMatchedWildcard( wfd.cFileName, const_cast<char*>(pszFilter) ) )	
					if (i3::is_matched_wildcard(wfd.cFileName, pszFilter) )
					{
						outFiles.push_back(pszPath);
						const size_t last_idx = outFiles.size()-1;
						i3::string& str = outFiles[last_idx];
						str += '/';	str += wfd.cFileName;
						++nCount;
					}
				}
			} while( ::FindNextFile( hFind, &wfd) == TRUE);

			::FindClose( hFind);
		}

		if( bSubDirSearch ) 
		{
			sprintf( _TempPath, "%s/*.*", pszPath);

			hFind = FindFirstFile( _TempPath, &wfd);

			if( hFind != INVALID_HANDLE_VALUE)
			{
				do {
					if( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
						!(wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) 
					{
						if((strcmp( wfd.cFileName, ".") != 0) && (strcmp( wfd.cFileName, "..") != 0)) 
						{
							_snprintf( _TempPath, MAX_PATH, "%s/%s", pszPath, wfd.cFileName);
								
							// 재귀호출
							nCount += _Rec_GatherFiles_System( _TempPath, pszFilter, outFiles, bSubDirSearch );
						}
					}

				} while( ::FindNextFile( hFind, &wfd) == TRUE);

				::FindClose( hFind);
			}
		}
		
		return nCount;
	}

	INT32 GatherFiles_System(const char* pszPath, const char* pszFilter, i3::vector<i3::string>& outFiles, bool bSubDirSearch )
	{
		char szPath[MAX_PATH];
		i3::safe_string_copy(szPath, pszPath, MAX_PATH);
		i3::make_unix_path(szPath);		//  '/'로 치환..
		i3::remove_end_slash(szPath);	// 마지막 슬래시 제거..

		char szCurPath[MAX_PATH];
		::GetCurrentDirectory( sizeof(szCurPath) - 1, szCurPath);
		INT32 cnt = _Rec_GatherFiles_System( szPath, pszFilter, outFiles, bSubDirSearch);
		::SetCurrentDirectory( szCurPath);

		return cnt;
	}

	// i3String::IsMatchedWildcard란 함수를 찾음..
	INT32 _Rec_GatherFiles_Pack( i3PackNode* pPackNode, const i3::string& pszPath, const char* szFilter, i3::vector<i3::string>& outFiles, bool bSubDirSearch)
	{	
		INT32 nResCount = 0;
		const INT32 nFile = pPackNode->getFileCount();
		I3_PACK_FILE_REF* pFile;
		for (INT32 i = 0 ; i < nFile ; ++i)
		{
			pFile = pPackNode->getFile(i);

		//	if ( i3String::IsMatchedWildcard( pFile->m_szName, const_cast<char*>(szFilter) ) )
			if ( i3::is_matched_wildcard( pFile->m_szName, szFilter ) )
			{
				outFiles.push_back(pszPath);
				const size_t last_idx = outFiles.size() - 1;
				i3::string& str = outFiles[last_idx];
				str += '/';	str += pFile->m_szName;
				++nResCount;
			}
		}

		// pPackNode를 재귀적으로 굴려서 경로붙은 파일이름을 모두 획득할것..
		if ( bSubDirSearch )
		{
			i3::string strNextPath;
			const INT32 nChild = pPackNode->getChildCount();
			i3PackNode* pChild;
			for ( INT32 i = 0 ; i < nChild ; ++i)
			{
				pChild = static_cast<i3PackNode*>(pPackNode->getChild(i));
				strNextPath = pszPath;
				strNextPath += '/';
				strNextPath += pChild->GetName();
				nResCount += _Rec_GatherFiles_Pack(pChild, strNextPath, szFilter, outFiles, bSubDirSearch);
			}
		}

		return nResCount;
	}
	
	INT32	GatherFiles_Pack( const char* pszPath, const char* szFilter, i3::vector<i3::string>& outFiles, bool bSubDirSearch)
	{
		char szPath[MAX_PATH];
		i3::safe_string_copy(szPath, pszPath, MAX_PATH);
		i3::remove_end_slash(szPath);	// 마지막 슬래시 제거..
		i3::make_unix_path(szPath);		// '/' 치환..

		i3PackNode* pPackNode = i3Pack::FindDirectoryPackNode(szPath);
	
		if( pPackNode == nullptr)
			return 0;
		
		INT32 nRes = _Rec_GatherFiles_Pack(pPackNode, i3::string(szPath), szFilter, outFiles, bSubDirSearch);

		return nRes;
	}


}



bool		i3Lua::LoadFileFromPreload( LuaState* L, const char* pszFileName)
{
	i3::stack_string strPath;
	strPath = pszFileName;
	i3::remove_ext(strPath);
	i3::make_unix_path(strPath);

	i3::replace_all(strPath, "//", "/");
	i3::to_upper(strPath);

	preload_map_type::iterator it_find = g_lua_preload_map.find(strPath.c_str());
	if (it_find == g_lua_preload_map.end())
	{
		return false;
	}

	i3::vector<BYTE>& buff = it_find->second;

	return ( i3Lua::LoadBuffer(L, reinterpret_cast<const char*>(&buff[0]), buff.size(), pszFileName) == 0 );
}

namespace
{

void		_AddPreloadDirectory(const char* szLuaDir, const char* szFilter, bool bPack, bool bSubDirSearch)
{
	
	i3::vector<i3::string>	vec_files;
	// 팩킹을 먼저 찾고, 맵에 데이터 넣은 뒤에....(패킹데이터 우선 원칙?)
	// 나중에 넌팩킹을 찾는다...
	if (bPack)
		GatherFiles_Pack(szLuaDir, szFilter, vec_files, bSubDirSearch);
	else
		GatherFiles_System(szLuaDir, szFilter, vec_files, bSubDirSearch);
	
	// 모두 대문자로 변경..-_-
	{
		const size_t num = vec_files.size();
		for (size_t i = 0; i < num ; ++i)
		{
			i3::string& str = vec_files[i];
			i3::transform(str.begin(), str.end(), str.begin(), &::toupper);
		}
	}
	//

	const INT32 num = INT32(vec_files.size());

	i3FileStream stream;
	LuaState* L = i3Lua::Open();

	for (INT32 i = 0 ; i < num ; ++i)
	{
		const i3::string& strFile = vec_files[i];
		
		size_t ext_pos = strFile.rfind('.');
		i3::string str_key( strFile, 0, ext_pos);		// 이구조에서는 확장자를 못찾으면 전체를 잡게 됨..
		
		size_t find_count = g_lua_preload_map.count(str_key);
		if ( find_count > 0 )		// 같아버리면 이미 존재하는 것으로 스킵 대상..
			continue;				// 오류가 아닌것으로 간주
		
		if (!stream.Open(strFile.c_str(), STREAM_READ | STREAM_SHAREREAD) )
		{
			I3PRINTLOG(I3LOG_WARN,  "Can't preopen %s file.", strFile );
			stream.Close();
			continue;
		}

		INT32 filesize = stream.GetSize();
		if (filesize == 0)
		{
			I3TRACE( "Zero FileSize %s file.\n", strFile );
			stream.Close();
			continue;
		}

		char* buff = (char*) i3MemAlloc( sizeof(char) * filesize);
		stream.Read(buff, sizeof(char) * filesize);
		stream.Close();
		
		i3::vector<BYTE>  result_buff;
		if (i3Lua::GetCompiledBuffer(L, buff, filesize, strFile.c_str(), result_buff))
		{
			// 성공한 경우, 확장자를 떼고 맵에 담자..
			
			preload_map_type::iterator res = 
				g_lua_preload_map.insert( preload_map_type::value_type(str_key, i3::vector<BYTE>())).first;		
			// Insert를 이렇게 취하는데는 스왑이 복사를 하지 않는다는 이유가 있음..
			result_buff.swap( res->second );
		}

		I3MEM_SAFE_FREE(buff);
	}

	i3Lua::Close(L);

}
	
void	_SaveCompiledFileDirectory(const char* szSrcDir, const char* szDestDir, const char* szSrcFilter, const char* szDestExt)
{
	i3::vector<i3::string>	vec_files;
	i3::string					strDestDir = szDestDir;

	if ( strDestDir.size() == 0 )
		return;
	if (strDestDir[strDestDir.size() - 1 ] != '/')
		strDestDir += '/';

	GatherFiles_System(szSrcDir, szSrcFilter, vec_files, false);

	const INT32 num = INT32(vec_files.size());
	I3TRACE("===================================\n");
	I3TRACE(" Compile And Save : %s --> %s   \n", szSrcFilter, szDestExt);
	I3TRACE(" Src Directory : %s\n", szSrcDir);
	I3TRACE(" Dest Directory : %s\n", szDestDir);
	I3TRACE("===================================\n");

	i3FileStream stream;
	LuaState* L = i3Lua::Open();

	for (INT32 i = 0 ; i < num ; ++i)
	{
		const i3::string& strFile = vec_files[i];

		if (!stream.Open(strFile.c_str(), STREAM_READ | STREAM_SHAREREAD) )
		{
			I3PRINTLOG(I3LOG_WARN,  "Can't preopen %s file.", strFile );
			stream.Close();
			continue;
		}

		INT32 filesize = stream.GetSize();
		if (filesize == 0)
		{
			I3TRACE( "Zero FileSize %s file.\n", strFile );
			stream.Close();
			continue;
		}

		char* buff = (char*) i3MemAlloc( sizeof(char) * filesize);
		stream.Read(buff, sizeof(char) * filesize);
		stream.Close();

		i3::vector<BYTE>  result_buff;
		if (i3Lua::GetCompiledBuffer(L, buff, filesize, strFile.c_str(), result_buff))
		{
			INT32 sizeBuff = INT32(result_buff.size());

			if (sizeBuff > 0)
			{
				size_t name_pos = strFile.rfind('/');

				if (name_pos == i3::string::npos)
					name_pos = 0;
				else
					++name_pos;

				i3::string strFilenameOnly(strFile, name_pos);

				i3::string strSaveFile(strDestDir);
				strSaveFile += strFilenameOnly;

				size_t ext_pos = strSaveFile.rfind('.');
				if (ext_pos != i3::string::npos)
					strSaveFile.resize(ext_pos);

				if (szDestExt[0] != '.')
					strSaveFile += '.';
				strSaveFile += szDestExt;

				if ( stream.Create(strSaveFile.c_str(), STREAM_WRITE) )
				{
					stream.Write(&result_buff[0], sizeBuff);
					stream.Close();
				}
			}
		}

		I3MEM_SAFE_FREE(buff);
	}

	i3Lua::Close(L);

	I3TRACE("===================================\n");
	I3TRACE(" Compile And Save Completed        \n");
	I3TRACE(" Src Directory : %s\n", szSrcDir);
	I3TRACE(" Dest Directory : %s\n", szDestDir);
	I3TRACE("===================================\n");

}

}

extern "C" int PreRequire_Glue(lua_State *L)
{
	const char* module_name = lua_tostring(L, 1);		// 모듈의 이름..  최대한 로딩을 지연시킴..
	
	i3::string str_filename(module_name);
	// 다른건 모르겠고, 올바른 파일 이름으로 이걸 스택에 푸시해둬야한다..
	{
		const size_t num_char = str_filename.size();
		for (size_t j = 0; j < num_char ; ++j)
		{
			char& s = str_filename[j];
			if (s == '.' ) s = '/';			// 파일 경로로 변환..
		}
	}

	i3::transform(str_filename.begin(), str_filename.end(), str_filename.begin(), ::toupper);		// 파일명은 대문자로 통일.. (확장자는 없으니. 덧붙이지 않음)

	preload_map_type::iterator it_find = g_lua_preload_map.find(str_filename);

	if ( it_find == g_lua_preload_map.end() ) 
	{
		I3TRACE("Lua Preload Require Process Failed : %s\n", module_name);	// 일반적인 루틴으로 변경하기 어려움..
		return 0;		// 
	}

	i3::vector<BYTE>& buff = it_find->second;		// 참조형타입이어야한다..

	if ( luaL_loadbuffer(L, reinterpret_cast<const char*>(&buff[0]), buff.size(), str_filename.c_str() ) != 0 )
	{
		I3TRACE("luaScriptCompile :: %d - %s: %s\n", __LINE__, module_name, lua_tostring(L, -1) );
		lua_pop(L, 1);	// 오류문자열1개..
		return 0;
	}
	
	lua_pushstring(L, module_name);  /* pass name as argument to module */
	lua_call(L, 1, 1);  /* run loaded module */
	
	return 1;			// lua_call함수의 1임..
}

// 루아파일들을 통째로 읽어들인다... 읽어들인 것들은 LuaState가 없고, 일정한 메모리를 추가로 차지하게 됨..
// 엔진 구조상 먼저 팩을 조사하기 때문에, 이것부터 파일명을 찾고, 그다음에 다시 디렉토리를 다시 찾도록 한다..
// 한번 읽어들인 루아는 다시 읽지 않도록 조절한다..


void		i3Lua::AddPreloadDirectory( const char* szLuaDir, bool bSubDirSearch)
{
#if !defined(I3_DEBUG)
	_AddPreloadDirectory(szLuaDir, "*.luab", true, bSubDirSearch);
	_AddPreloadDirectory(szLuaDir, "*.luab", false, bSubDirSearch);
#endif
	_AddPreloadDirectory(szLuaDir, "*.lua", true, bSubDirSearch);
	_AddPreloadDirectory(szLuaDir, "*.lua", false, bSubDirSearch);
}

void	i3Lua::SaveCompiledFileDirectory( const char* szSrcDir, const char* szDestDir)
{
	_SaveCompiledFileDirectory(szSrcDir, szDestDir, "*.lua", "lua");
}

void	i3Lua::SaveCompiledFileDirectory( const char* szLuaDir)
{
	_SaveCompiledFileDirectory(szLuaDir, szLuaDir, "*.lua", "luab");
}

namespace
{

void	_AddPreloadRequireDirectory(const char* szLuaDir, const char* szFilter, bool bPack, bool bSubDirSearch)
{
	i3::vector<i3::string>	vec_files;
	if (bPack)
		GatherFiles_Pack(szLuaDir, szFilter, vec_files, bSubDirSearch);
	else
		GatherFiles_System(szLuaDir, szFilter, vec_files, bSubDirSearch);

	// 모두 대문자로 변경..-_-
	const size_t num = vec_files.size();
	for (size_t i = 0; i < num ; ++i)
	{
		i3::string& str = vec_files[i];
		size_t ext_pos = str.rfind('.');
		i3::string str_key( str, 0, ext_pos);		// 이구조에서는 확장자를 못찾으면 전체를 잡게 됨..
		g_lua_require_set.insert(str_key);			// 같은 것은 들어가지 않을 것이다..
	}
	//
}

}


void	i3Lua::AddPreloadRequireDirectory( const char* szLuaDir, bool bSubDirSearch)
{
#if !defined(I3_DEBUG)
	_AddPreloadRequireDirectory(szLuaDir, "*.luab", true, bSubDirSearch);
	_AddPreloadRequireDirectory(szLuaDir, "*.luab", false, bSubDirSearch);
#endif
	_AddPreloadRequireDirectory(szLuaDir, "*.lua", true, bSubDirSearch);
	_AddPreloadRequireDirectory(szLuaDir, "*.lua", false, bSubDirSearch);
}

// 공통 리콰이어 대상에 대해 따로 모아둔다..  스테이트를 만들때, 루아 표준Lib를 받아들이고 나서, 적재만하고 call은 하지 않게 조정한다..
// 이렇게 처리하는 이유는 루아쪽의 require가 제대로 리콰이어루아를 2번읽지 않으면서도, lua_call은 알맞게 잘 돌도록 조정하기 위한 것임..
bool		i3Lua::AddPreloadRequire( const char* pszPath)
{
	char szPath[MAX_PATH];
	i3::safe_string_copy(szPath, pszPath, MAX_PATH);
	i3::remove_ext(szPath);			// 확장자 뗌..
	i3::make_unix_path(szPath);			// '/'치환
	i3::pair< require_set_type::iterator, bool> res_pair = g_lua_require_set.insert(szPath);		// 실패하면 들어가지 않을 것이다..
	return res_pair.second;
}
//

void		i3Lua::ProcessRequireFromPreload(LuaState* L)
{
//  package.preload를 건드려야함..(따라서, 패키지 모듈이 선행되어야할 것이 기대됨...)	
//  프리로드 되지 않았다면, 사실상 의미가 없기 때문에 모두 스킵한다...
	lua_getfield(L, LUA_GLOBALSINDEX, "package");
	lua_getfield(L, -1, "preload");
	if (!lua_istable(L, -1))
		luaL_error(L, LUA_QL("package.preload") " must be a table");

	require_set_type::const_iterator it = g_lua_require_set.begin();
	require_set_type::const_iterator it_end = g_lua_require_set.end();

	for ( ; it != it_end ; ++it)
	{
		const i3::string& str_file = *it;		// 이걸쓰지 말고 확장자를 떼야함..

		i3::string str_modulename(str_file);
		// 다른건 모르겠고, 올바른 모듈 이름으로 이걸 스택에 푸시해둬야한다..
		{
			const size_t num_char = str_modulename.size();
			for (size_t j = 0; j < num_char ; ++j)
			{
				char& s = str_modulename[j];
				if (s == '/' ) s = '.';			// 유닉스형 경로만 처리했음..
			}
		}
		
		// 루아의 package.preload에 알맞게 삽입해야함...
		lua_getfield(L, -1, str_modulename.c_str());
		if (!lua_isnil(L, -1))  //  찾아진 케이스...넣을 필요가 사라짐..
		{
			lua_pop(L, 1);	continue;
		}
		
		lua_pop(L, 1);
		
		i3::string upper_str(str_file);
		i3::transform(upper_str.begin(), upper_str.end(), upper_str.begin(), &::toupper);

		preload_map_type::iterator it_find = g_lua_preload_map.find(upper_str);

		if ( it_find == g_lua_preload_map.end() ) // 못찾았기 때문에 쓰지 않고 버린다..
			continue;

		// 적재과정이 필요함...
		lua_pushcfunction(L, &PreRequire_Glue);
		lua_setfield(L, -2, str_modulename.c_str());		// 로드된 버퍼자체를 해당모듈네임으로 넣음...// 호출되면 모듈이름과 버퍼가 모두 팝된다..
		
	}

	lua_pop(L, 2);		// preload와 package를 팝..
}



