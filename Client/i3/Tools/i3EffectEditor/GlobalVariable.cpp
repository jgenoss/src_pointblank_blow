#include "stdafx.h"
#include "i3EffectEditorDlg.h"

BOOL	g_bNeedToCreate = TRUE;

CRITICAL_SECTION		g_RenderCS;
Ci3EffectEditorDlg *	g_pMainWnd = NULL;
HANDLE					g_hTermEvent = NULL;
UINT32					g_FPS = 60;
COptionInfo *			g_pOption = NULL;
