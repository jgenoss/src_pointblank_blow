#include "i3FrameworkPCH.h"
#include "i3Framework.h"

extern "C" int i3FRM_toggleDebugTextEnable(lua_State *L)
{
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if( pFramework == nullptr)
		return 0;

	i3Viewer * pViewer = pFramework->GetViewer();
	if( pViewer == nullptr)
		return 0;

	return 0;
}

extern "C" int i3Gfx_SetRenderMode(lua_State *L)
{
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if( pFramework == nullptr)		return 0;

	i3Viewer * pViewer = pFramework->GetViewer();
	if( pViewer == nullptr)	return 0;

	i3RenderContext * pCtx = pViewer->GetRenderContext();
	if( pCtx == nullptr)		return 0;

	I3G_RENDER_MODE mode = (I3G_RENDER_MODE)(i3Lua::GetIntegerArg(L, 1, 0) + 1);
	if((mode == I3G_RENDER_POINT) || (mode == I3G_RENDER_LINE) || (mode == I3G_RENDER_POLYGON))
	{
		pCtx->SetRenderMode(mode);
	}

	return 0;
}

extern "C" int i3Gfx_SetShadeMode(lua_State *L)
{
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if( pFramework == nullptr)		return 0;

	i3Viewer * pViewer = pFramework->GetViewer();
	if( pViewer == nullptr)	return 0;

	i3RenderContext * pCtx = pViewer->GetRenderContext();
	if( pCtx == nullptr)		return 0;

	I3G_SHADE_MODE mode = (I3G_SHADE_MODE)(i3Lua::GetIntegerArg(L, 1, 0)+1);
	if((mode == I3G_SHADE_FLAT) || (mode == I3G_SHADE_GOURAUD) || (mode == I3G_SHADE_PHONG) ||
			(mode == I3G_SHADE_CUSTOM) || (mode == I3G_SHADE_LUXMAP) || (mode == I3G_SHADE_HEMISPHERE) ||
			(mode == I3G_SHADE_GI1))
	{
		pCtx->SetShadeMode(mode);
	}

	return 0;
}

extern "C" int i3Gfx_SetFogMode(lua_State *L)
{
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if( pFramework == nullptr)		return 0;

	i3Viewer * pViewer = pFramework->GetViewer();
	if( pViewer == nullptr)	return 0;

	i3RenderContext * pCtx = pViewer->GetRenderContext();
	if( pCtx == nullptr)		return 0;

	I3G_FOG_MODE mode = (I3G_FOG_MODE)i3Lua::GetIntegerArg(L, 1, 0);
	if((mode == I3G_FOG_NONE) || (mode == I3G_FOG_LINEAR) || (mode == I3G_FOG_EXP) || (mode == I3G_FOG_EXP2))
	{
		COLOR col;
		i3Color::Set(&col, (UINT8) 255,255,125,255);

		if( mode == I3G_FOG_NONE)
		{
			pCtx->SetFogEnable(false);
		}
		else
		{
			pCtx->SetFogEnable(true);
			pCtx->SetFogMode(mode);
			pCtx->SetFogColor(&col);
		}
	}

	return 0;
}

extern "C" int i3Gfx_SetFogNear(lua_State *L)
{
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if( pFramework == nullptr)		return 0;

	i3Viewer * pViewer = pFramework->GetViewer();
	if( pViewer == nullptr)	return 0;

	i3RenderContext * pCtx = pViewer->GetRenderContext();
	if( pCtx == nullptr)		return 0;

	REAL32 f = i3Lua::GetNumberArg( L, 1, 0.0f);

	pCtx->SetFogNearFar( f, pCtx->GetFogFar());

	return 0;
}

extern "C" int i3Gfx_SetFogFar(lua_State *L)
{
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if( pFramework == nullptr)		return 0;

	i3Viewer * pViewer = pFramework->GetViewer();
	if( pViewer == nullptr)	return 0;

	i3RenderContext * pCtx = pViewer->GetRenderContext();
	if( pCtx == nullptr)		return 0;

	REAL32 f = i3Lua::GetNumberArg( L, 1, 0.0f);

	pCtx->SetFogNearFar( pCtx->GetFogNear(), f);

	return 0;
}

extern "C" int i3Gfx_SetFogDensity(lua_State *L)
{
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if( pFramework == nullptr)		return 0;

	i3Viewer * pViewer = pFramework->GetViewer();
	if( pViewer == nullptr)	return 0;

	i3RenderContext * pCtx = pViewer->GetRenderContext();
	if( pCtx == nullptr)		return 0;

	REAL32 density = i3Lua::GetNumberArg(L, 1, 0.0f);

	pCtx->SetFogDensity( density);

	return 0;
}

LuaFuncReg i3FrameWorkGlue[] = 
{
	{"frm_toggleDebugTextEnable",	i3FRM_toggleDebugTextEnable},
	{"Gfx_SetRenderMode",			i3Gfx_SetRenderMode	},
	{"Gfx_SetShadeMode",			i3Gfx_SetShadeMode	},
	{"Gfx_SetFogMode",				i3Gfx_SetFogMode	},
	{"Gfx_SetFogNear",				i3Gfx_SetFogNear	},
	{"Gfx_SetFogFar",				i3Gfx_SetFogFar		},
	{"Gfx_SetFogDensity",			i3Gfx_SetFogDensity	},

	{nullptr, nullptr}
};

void i3Framework::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3FrameWorkGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3FrameWorkGlue[i].name, i3FrameWorkGlue[i].func);
	}
}
