#pragma once

// lua
extern "C"{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
};

#define LUAREG_PAIR(name) {#name , name}
#define LUAREG_END() { NULL, NULL}

#define MODE_SCRIPT

#ifdef MODE_SCRIPT
#define MODE_SCRIPT_TRACE(format, ...) I3TRACE(format, __VA_ARGS__)
#define MODE_SCRIPT_RETURN return
#define USE_OLD_VERSION
#define LUA_RETURN
#else
#define MODE_SCRIPT_TRACE(msg)
#define MODE_SCRIPT_RETURN
#define USE_OLD_VERSION return;
#define LUA_RETURN return
#endif
