#include "StdAfx.h"
#include "GlobalVariables.h"
#include "i3UIEFramework.h"

void LuaStressTest( void);

I3_CLASS_INSTANCE( i3UIEFramework); //, i3Framework);

i3UIEFramework::i3UIEFramework(void)
{
	g_pFramework		= this;
}

i3UIEFramework::~i3UIEFramework(void)
{
	g_pFramework		= NULL;
}

void i3UIEFramework::OnCreate()
{
//	i3ViewerFramework::OnCreate();
	i3Framework::OnCreate();

	I3ASSERT( i3UI::getManager());
	i3UI::getManager()->SetEditMode( true);
}

//void i3UIEFramework::OnUpdate( REAL32 fDeltaTime)
//{
//	//LuaStressTest();
//}

//////////////////////////////////////////////////
// LuaStressTest 
extern "C" int luaGluee(lua_State *L)
{
	//i3Lua::PushNumber( L, (lua_Number)nVal1);

	//res[0] = (INT32)i3Lua::GetNumberArg(L, 1, 0);
	
	return 0;	// "number of return values"
}

//#define PROFILING_COUNT 10000
//#define SP() { clock_t s = clock();
//#define EP() clock_t e = clock(); I3TRACE("\n%lf sec\n", (double)(e-s)/CLOCKS_PER_SEC);	}

LuaFuncReg LuaGlue[] = 
{
	{"luaGluee",				luaGluee	},
	{NULL, NULL}
};

void registerLuaFunction(LuaState * pState)
{
	if( pState == NULL)
		return;

	for(int i=0; LuaGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, LuaGlue[i].name, LuaGlue[i].func);
	}
}

void LuaStressTest( void)
{
	LuaState *	pLuaState = NULL;
	
	// Ready Lua
	{
		pLuaState = i3Lua::Create(0);
		I3ASSERT( pLuaState != NULL);
		
		i3Lua::DoFile( pLuaState, "D:/i3_2/include/i3Framework/i3UI/StressTest.lua");
		registerLuaFunction( pLuaState);
	}

	//INT32 nProfileCount = 60;
	//// DoString
	//__SP( nProfileCount);
	//{
	//	for( INT32 i=0; i<PROFILING_COUNT; i++)
	//	{
	//		// Todo
	//	}
	//}
	//__EP( nProfileCount);
	//nProfileCount++;

}