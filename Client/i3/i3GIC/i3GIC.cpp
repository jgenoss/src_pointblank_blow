// i3GIC.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "i3GICApp.h"
#include "i3GIC.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HINSTANCE g_hInstGIC = nullptr;

static bool s_bRegisterGICLib = false;

I3_EXPORT_GIC
void i3GICRegisterMetas(void)
{
#if defined( I3_DEBUG)
	if( s_bRegisterGICLib == false)
	{
		/*
		i3GICObject::RegisterMeta();
		i3GICShape::RegisterMeta();
		i3GICShapeFigure::RegisterMeta();
		i3GICShapeRect::RegisterMeta();
		i3GICShapeRectBar::RegisterMeta();
		i3GICShapeTextRect::RegisterMeta();
		i3GICShapeRenderTexture::RegisterMeta();
		i3GICShapeLine::RegisterMeta();
		i3GICShapeBezierLine::RegisterMeta();
		i3GICShapeCircle::RegisterMeta();
		i3GICShapeCircleBar::RegisterMeta();
		i3GICShapeTriangle::RegisterMeta();
		i3GICControlPoint::RegisterMeta();
		i3GICLinkPoint::RegisterMeta();
		i3GICDiagram::RegisterMeta();
		i3GICDiagramContext::RegisterMeta();

		i3GICShapeTemplate::RegisterMeta();
		*/
	}
#endif
}

BOOL WINAPI DllMain( HANDLE hInst, DWORD dwReason, LPVOID)
{
	switch( dwReason)
	{
		case DLL_PROCESS_ATTACH :	//i3GfxRegisterMetas();
			i3System::HoldDllModule((HMODULE)hInst);
			g_hInstGIC = (HINSTANCE) hInst;
			break;
	}

	return TRUE;
}