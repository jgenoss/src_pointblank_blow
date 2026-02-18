#include "stdafx.h"

#ifdef USE_I3EXEC
HINSTANCE		g_hInstance = NULL;
#endif

#if defined( I3_DEBUG)
#include "DebugPanel.h"

CDebugPanel *	g_pDebugPanel = NULL;

void ToggleDebugPanel(void)
{
	if( g_pDebugPanel == NULL)
	{
		//AfxSetResourceHandle( hInst);

		g_pDebugPanel = new CDebugPanel;

		g_pDebugPanel->Create( IDD_DEBUG_PANEL, NULL);
	}

	if( g_pDebugPanel->IsWindowVisible())
	{
		g_pDebugPanel->ShowWindow( SW_HIDE);
	}
	else
	{
		g_pDebugPanel->ShowWindow( SW_SHOW);
	}
}

void CloseDebugPanel(void)
{
	I3_SAFE_DELETE( g_pDebugPanel);
}

#endif
