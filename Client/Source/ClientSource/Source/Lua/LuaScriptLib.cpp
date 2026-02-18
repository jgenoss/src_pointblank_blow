/*----------------------------------------------------------------------------
    LUA SCRIPT LIB.C
    Small library of Lua glue routines to support script management.

    contact: Matthew Harmon matt@matthewharmon.com
*/

/*--------------------------------------------------------------------------*/
/*                              Include Files                               */
/*--------------------------------------------------------------------------*/
#include "pch.h"
#ifdef I3_WINDOWS
	#include    <windows.h>     // just for Beep()
#endif
#include    <stdio.h>
#include    <string.h>

#include    "LuaScript.h"
#include    "LuaScriptLib.h"
#include    "LuaManager.h"

#pragma warning(push)
//임시적으로 컴파일 경고 막음
#pragma warning (disable : 4189; disable : 4244)
/*--------------------------------------------------------------------------*/
/*                           Compilation Control                            */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                            Defined Constants                             */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                              Data Structures                             */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                            Public Prototypes                             */    
/*--------------------------------------------------------------------------*/

static LUASCRIPT*   GetScriptObject				(lua_State* l);
static int          LuaCreateChara				(lua_State* l);
static int          LuaSetCharaVelocity			(lua_State* l);
static int          LuaSetCharaAnimation		(lua_State* l);
static int          LuaCommand					(lua_State* l);
static int          LuaGetPosFromCToLua			(lua_State* l);
static int          LuaGetPosFromLuaToC			(lua_State* l);



/*--------------------------------------------------------------------------*/
/*                             Global Variables                             */
/*--------------------------------------------------------------------------*/

// table of function names and entry points that will be registered with Lua
static const luaL_reg CharaLib[] =
{
    {"CreateChara",					LuaCreateChara},
	{"SetCharaVelocity",			LuaSetCharaVelocity},
	{"SetCharaAnimation",			LuaSetCharaAnimation},
	{"GetPosFromCToLua",			LuaGetPosFromCToLua},
	{"GetPosFromLuaToC",			LuaGetPosFromLuaToC},
    {nullptr, nullptr}
};


/*----------------------------------------------------------------------------
    LUA OPEN SCRIPT LIB
    Registers all the basic script support functions with Lua.
*/

void LuaOpenScriptLib(
    lua_State*  l)
    {
    // register our library of routines under the table name Script
    luaL_openlib(l, "Chara", CharaLib, 0);
    }


/*----------------------------------------------------------------------------
    LUA BEEP
    Expose the windows Beep() function.  Grab frequency and duration from
    the stack.
*/

static int LuaGetPosFromCToLua(lua_State* l)
{
	INT32 iID = luaL_checknumber(l, 1);
	VEC3D v3Pos;
	v3Pos.x = v3Pos.y = v3Pos.z = 0.0f;

	lua_newtable (l);
	lua_pushnumber(l, 1);
	lua_pushnumber(l, v3Pos.x);
	lua_settable (l, -3);

	lua_pushnumber(l, 2);
	lua_pushnumber(l, v3Pos.y);
	lua_settable (l, -3);

	lua_pushnumber(l, 3);
	lua_pushnumber(l, v3Pos.z);
	lua_settable (l, -3);

	return 1;
}

static int LuaGetPosFromLuaToC(lua_State* l)
{
	INT32 iID = (INT32)luaL_checknumber(l, 1);
	REAL32 rX = (REAL32)luaL_checknumber(l, 2);
	REAL32 rY = (REAL32)luaL_checknumber(l, 3);
	REAL32 rZ = (REAL32)luaL_checknumber(l, 4);

	return 0;
}

static int LuaCreateChara(lua_State* l)
{
	INT32 iID = luaL_checknumber(l, 1);

	return 0;
}

static int LuaSetCharaVelocity(lua_State* l)
{
	INT32 iID = (INT32)luaL_checknumber(l, 1);
	REAL32 rVelocity = (REAL32)luaL_checknumber(l, 2);

	return(0);
}

static int LuaSetCharaAnimation(lua_State* l)
{
	INT32 iID = (INT32)luaL_checknumber(l, 1);
	INT32 iAniIdx = (INT32)luaL_checknumber(l, 2);

	return(0);
}



/*----------------------------------------------------------------------------
    GET SCRIPT OBJECT
    Retrieves the address of the C++ object managing the given lua_State.
*/

static LUASCRIPT* GetScriptObject(lua_State* l)
{
	lua_pushlightuserdata(l, l);
	lua_gettable(l, LUA_GLOBALSINDEX );
	return((LUASCRIPT*)lua_touserdata(l, -1));
}

#pragma warning(pop)