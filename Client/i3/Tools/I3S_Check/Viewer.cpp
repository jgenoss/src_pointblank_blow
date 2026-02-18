#include "stdafx.h"
#include "Viewer.h"
#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static HWND g_hwndMain;

bool		InitViewer(void)
{
	LPCTSTR pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW);

	g_hwndMain = ::CreateWindow( pszClass, "Viewer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, NULL, NULL, 
			::GetModuleHandle(NULL), NULL);
	I3ASSERT( g_hwndMain != NULL);

	//ShowWindow( g_hwndMain, SW_SHOW);

	i3VideoInfo video;

	video.m_bFullScreen = FALSE;
	video.m_Width = 100;
	video.m_Height = 100;

	g_pViewer = i3Viewer::new_object();
	g_pViewer->Create( g_hwndMain, &video);

	g_pFramework = i3ViewerFramework::new_object();
	g_pFramework->Create( g_pViewer);

	return true;
}

bool		CloseViewer(void)
{
	I3_SAFE_RELEASE( g_pFramework);
	I3_SAFE_RELEASE( g_pViewer);

	::DestroyWindow( g_hwndMain);

	return true;
}
