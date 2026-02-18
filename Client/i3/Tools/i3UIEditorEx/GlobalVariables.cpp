#include "stdafx.h"
#include "GlobalVariables.h"

HMODULE					g_hModule		= NULL;
i3GameInterface		*	g_pInterface	= NULL;
i3Viewer			*	g_pViewer		= NULL;
i3Framework			*	g_pFramework	= NULL;
HWND					g_hActiveView	= NULL;
RECT					g_rtTarget;
HBRUSH					g_hbrBackground = NULL;
REAL32					g_fZoom = 1.f;
