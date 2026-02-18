#if !defined( _GLOBAL_VARIABLES_H__)
#define _GLOBAL_VARIABLES_H__

#include "i3UIEFramework.h"

extern HMODULE				g_hModule;
extern i3GameInterface  *	g_pInterface;
extern i3Viewer			*	g_pViewer;
extern i3Framework		*	g_pFramework;
extern HWND					g_hActiveView;
extern RECT					g_rtTarget;
extern HBRUSH				g_hbrBackground;

extern REAL32				g_fZoom;

#endif