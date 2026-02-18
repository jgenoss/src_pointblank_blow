#if !defined( __GLOBAL_VARIABLE_H)
#define __GLOBAL_VARIABLE_H

#include "OptionInfo.h"

class Ci3EffectEditorDlg;

extern BOOL	g_bNeedToCreate;
extern CRITICAL_SECTION		g_RenderCS;
extern Ci3EffectEditorDlg *	g_pMainWnd;
extern HANDLE				g_hTermEvent;
extern UINT32				g_FPS;
extern COptionInfo *		g_pOption;

inline	void InitRenderCS(void)
{
	InitializeCriticalSection( &g_RenderCS);
}

inline	void EnterRenderCS(void)
{
	EnterCriticalSection( &g_RenderCS);
}

inline void LeaveRenderCS(void)
{
	LeaveCriticalSection( &g_RenderCS);
}

#endif