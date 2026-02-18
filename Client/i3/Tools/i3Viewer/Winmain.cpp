// i3Viewer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>
#include "resource.h"
#include "i3GameInterfaceViewer.h"

#ifdef I3_WINDOWS

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND g_hwndMain;
HMODULE	g_hModule = NULL;

i3Viewer *			g_pViewer = NULL;
i3Framework *		g_pFramework = NULL;
i3GameInterface *	g_pInterface = NULL;
HANDLE				s_hSyncEvent;
char				g_szStartStage[256] = {0,};

// Forward declarations of functions included in this code module:
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

static i3GameInterface *	TryLoadGame( const char * pszPath)
{
	char szExt[127];
	i3GameInterface * pInterface = NULL;

	i3String::SplitFileExt( pszPath, szExt, sizeof( szExt) - 1);

	if( (i3String::Compare( szExt, "DLL") != 0) &&
		(i3String::Compare( szExt, "BIN") != 0) &&
		(i3String::Compare( szExt, "i3Exec") != 0))
	{
		return NULL;
	}

	char szFull[ MAX_PATH], szBack[ MAX_PATH];

	i3String::SplitPath( pszPath, szFull);

	::GetCurrentDirectory( sizeof( szBack) - 1, szBack);

	if( szFull[0] != 0)
	{
		::SetCurrentDirectory( szFull);
	}

	I3TRACE( "Try to load %s...\n", pszPath);

	g_hModule = ::LoadLibrary( pszPath);

	::SetCurrentDirectory( szBack);

	if( g_hModule != NULL)
	{
		I3_CREATE_GAME_INTERFACE_PROC pProc;

		pProc = (I3_CREATE_GAME_INTERFACE_PROC) GetProcAddress( g_hModule, "i3CreateGameInterface");
		if( pProc != NULL)
		{
			pInterface = pProc( szFull);
		}
	}

	return pInterface;
}

static void _ProcessArg( INT32 argc, const char * pszBuf)
{
	switch( argc)
	{
		case 0 :	g_pInterface = TryLoadGame( pszBuf);	break;
		default :	strcpy( g_szStartStage, pszBuf);	break;
	}
}

static i3GameInterface *	ParseParameters( const char * pszCmdLine)
{
	char * pszTemp = (char *) pszCmdLine;
	char szBuf[ MAX_PATH];
	INT32 idx = 0, argc = 0;
	bool bString = false;

	while( *pszTemp)
	{
		switch( *pszTemp)
		{
			case '"' :
				if( bString == false)
				{
					bString = true;
				}
				else
				{
					szBuf[ idx] = 0;

					_ProcessArg( argc, szBuf);
					argc++;

					bString = false;
				}
				idx = 0;
				break;

			case '\t' :
			case ' ' :
				if( bString )
				{
					// 문자열 중...
					szBuf[idx] = *pszTemp;
					idx++;
				}
				else
				{
					if( idx > 0)
					{
						szBuf[idx] = 0;

						_ProcessArg( argc, szBuf);
						argc++;

						idx = 0;
					}
				}
				break;

			default :
				szBuf[ idx] = *pszTemp;
				idx++;
				break;
		}

		pszTemp++;
	}

	if( idx > 0)
	{
		szBuf[ idx] = 0;
		_ProcessArg( argc, szBuf);
		argc++;
	}

	if( g_pInterface == NULL)
	{
		//g_pInterface = new i3GameInterfaceViewer;
	}

	return g_pInterface;
}

//
BOOL CreateViewerWindow(HINSTANCE hInstance)
{
	HWND hWnd;
	int nCmdShow, cx, cy, cxIcon, cyIcon, cxIconSm, cyIconSm;
	DWORD dwStyle;
	WNDCLASSEX wcex;
	UINT32 idIcon;
	char szTitle[512];
	i3VideoInfo video;

	hInst = hInstance; // Store instance handle in our global variable

	g_pInterface->GetVideoConfig( &video);
	
	if( g_pInterface->GetInitialWindowVisible() == false)
		nCmdShow = SW_HIDE;
	else
		nCmdShow = SW_SHOW;

	dwStyle = g_pInterface->GetViewportStyle();
	idIcon = g_pInterface->GetIcon();

	cxIcon = GetSystemMetrics( SM_CXICON);
	cyIcon = GetSystemMetrics( SM_CYICON);
	cxIconSm = GetSystemMetrics( SM_CXSMICON);
	cyIconSm = GetSystemMetrics( SM_CYSMICON);

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;

	if( idIcon == 0)
	{
		wcex.hIcon			= (HICON) LoadImage( hInstance, (LPCTSTR) IDI_SMALL, IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR);
		wcex.hIconSm		= (HICON) LoadImage( hInstance, (LPCTSTR) IDI_SMALL, IMAGE_ICON, cxIconSm, cyIconSm, LR_DEFAULTCOLOR);
	}
	else
	{
		wcex.hIcon			= (HICON) LoadImage( g_hModule, (LPCTSTR) idIcon, IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR);
		wcex.hIconSm		= (HICON) LoadImage( g_hModule, (LPCTSTR) idIcon, IMAGE_ICON, cxIconSm, cyIconSm, LR_DEFAULTCOLOR);
	}

	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_I3VIEWER;
	wcex.lpszClassName	= szWindowClass;

	RegisterClassEx( &wcex);

	{
		RECT Rect;

		Rect.left = 0;
		Rect.top = 0;
		Rect.right = video.m_Width;
		Rect.bottom = video.m_Height;

		AdjustWindowRect( &Rect, dwStyle, TRUE);

		cx = Rect.right - Rect.left;
		cy = Rect.bottom - Rect.top;
	}

	//sprintf( szTitle, "%s version %d.%d - Copyright Zepetto Inc.", g_pInterface->GetLongTitle(), g_pInterface->GetVersion() >> 16, g_pInterface->GetVersion() & 0xFFFF);
	sprintf( szTitle, "Point Blank");

	hWnd = CreateWindow( szWindowClass, szTitle, dwStyle,
		CW_USEDEFAULT, 0, cx, cy, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	g_hwndMain = hWnd;

	DragAcceptFiles( hWnd, TRUE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE /*hPrevInstance*/,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	MSG msg;
	HACCEL hAccel;
	HANDLE	hMutex = NULL;
	i3VideoInfo video;

	// 인자를 검사한다.
	{
		g_pInterface = ParseParameters( lpCmdLine);

		if( g_pInterface == NULL)
		{
			NotifyBox( NULL, "Could not found appropriate i3GameInterface class object.\n");
			return -1;
		}
	}

	g_pInterface->GetVideoConfig( &video);

	// 이중 실행을 방지한다.
	if( g_pInterface->GetMultipleInstanceEnable() == false)
	{
		hMutex = OpenMutex( MUTEX_ALL_ACCESS, FALSE, g_pInterface->GetTitle());
		if( hMutex == NULL )
		{
			hMutex = ::CreateMutex( NULL, TRUE, g_pInterface->GetTitle());
		}
		else
		{
			char conv[256];

			sprintf( conv, "%s 프로그램이 이미 실행 중입니다.", g_pInterface->GetTitle());
			MessageBox( NULL, conv, NULL, 0 );
			return FALSE;
		}
	}

	// Initialize global strings
	LoadString(hInstance, IDC_I3VIEWER, szWindowClass, MAX_LOADSTRING);

	i3MemoryInit( g_pInterface->GetInitialMemSize());

	g_pInterface->OnRegisterMeta();

	if( g_pInterface->OnInitInstance( hInstance, g_hwndMain) == false)
	{
		MessageBox( NULL, "프로그램의 초기화에 실패했습니다(2).\r\n종료합니다.", "Error", MB_OK);
		return FALSE;
	}

	// Perform application initialization:
	if (! CreateViewerWindow( hInstance)) 
	{
		MessageBox( NULL, "프로그램의 초기화에 실패했습니다(1).\r\n종료합니다.", "Error", MB_OK);
		return FALSE;
	}

	/////////////////////////////////////////////
	{
		g_pViewer = i3Viewer::NewObject();
		I3ASSERT( g_pViewer != NULL );

		g_pViewer->AddState( I3_VIEWER_STATE_INGAME);

		if( g_pViewer->Create( (void *) g_hwndMain, &video, g_pInterface->GetInputDeviceMask()) == FALSE)
		{
			MessageBox( g_hwndMain, "프로그램의 초기화에 실패했습니다(3).\r\n종료합니다.", "Error", MB_OK);
			return FALSE;
		}

		g_pFramework = g_pInterface->CreateFramework( g_pViewer);
		if( g_pFramework == NULL)
		{
			MessageBox( g_hwndMain, "프로그램의 초기화에 실패했습니다(4).\r\n종료합니다.", "Error", MB_OK);
			return FALSE;
		}

		// 만약 지정된 Start Stage가 있다면...
		if( g_szStartStage[0] != 0)
		{
			char szStageName[128];
			char szStageType[128], * pszDest, * pszSrc;
			INT32 i;

			// StageName(StageType) 형식의 입력을 Parsing한다.

			pszSrc = g_szStartStage;
			pszDest = szStageName;

			for( i = 0; *pszSrc != 0; i++)
			{
				switch( *pszSrc)
				{
					case '(' :
					case '[' :
					case ')' :
					case ']' :
						*pszDest = 0;

						pszDest = szStageType;
						break;

					default :
						*pszDest = *pszSrc;
						pszDest++;
						break;
				}

				pszSrc ++;
			}

			if( (pszDest != szStageName) && (pszDest != szStageType))
				*pszDest = 0;

			INT32 idx = g_pFramework->FindStageInfoByName( szStageName);

			if( idx != -1)
			{
				i3StageInfo * pStartStage = g_pFramework->getStageInfo( idx);

				g_pFramework->SetStartStageInfo( pStartStage);
			}
			else
			{
				i3StageInfo * pStartStage = i3StageInfo::NewObjectRef();

				pStartStage->SetName( szStageType);
				pStartStage->setStageClassName( szStageType);

				g_pFramework->AddStageInfo( pStartStage);
				g_pFramework->SetStartStageInfo( pStartStage);
			}
		}

		if( g_pInterface->OnInitFramework( g_pFramework, lpCmdLine) == false)
		{
			MessageBox( g_hwndMain, "프로그램의 초기화에 실패했습니다(5).\r\n종료합니다.", "Error", MB_OK);
			return FALSE;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	hAccel = LoadAccelerators(hInstance, (LPCTSTR)IDC_I3VIEWER);

    // Now we're ready to recieve and process Windows messages.
    BOOL bGotMsg, bContinue = TRUE;
	UINT32	IntervalForFrame = 1000 / g_pInterface->GetTargetFPS();

	s_hSyncEvent = CreateEvent( NULL, TRUE, TRUE, NULL);
	timeSetEvent( IntervalForFrame, 3, (LPTIMECALLBACK) s_hSyncEvent, 0, TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
	I3ASSERT( s_hSyncEvent != NULL);

    msg.message = WM_NULL;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

    while((bContinue ) && (WM_QUIT != msg.message  ))
    {
        // Use PeekMessage() if the app is active, so we can use idle time to
        // render the scene. Else, use GetMessage() to avoid eating CPU time.
		if( g_pViewer->IsReady())
            bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
        else
            bGotMsg = GetMessage( &msg, NULL, 0U, 0U );

        if( bGotMsg )
        {
            // Translate and dispatch the message
            if( 0 == TranslateAccelerator( g_hwndMain, hAccel, &msg ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
        		
		if( msg.message == WM_DESTROY)
			bContinue = FALSE;
		else
			bContinue = g_pInterface->OnDrive( g_pViewer);

		WaitForSingleObject( s_hSyncEvent, INFINITE);
		ResetEvent( s_hSyncEvent);		
    }

	CloseHandle( s_hSyncEvent );

	if( hMutex != NULL)
	{
		CloseHandle( hMutex );
	}

	I3_SAFE_RELEASE( g_pFramework);
	I3_SAFE_RELEASE( g_pViewer);

	g_pInterface->OnExitInstance();

	if( g_hModule != NULL)
	{
		FreeLibrary( g_hModule);
	}

	return (int) msg.wParam;
}

static HHOOK g_hKeyHook = NULL;

LRESULT CALLBACK KeyHookProc( int nCode, WPARAM wParam, LPARAM lParam)
{
	if( nCode >= 0)
	{	//	창모드에서 Alt 키가 SysMenu를 호출하지 못하도록 막기위해... by KOMET
		if( wParam == VK_MENU)
		{
			wParam &= ~VK_MENU;		//	키맵에서 제거한다.			
		}
	}

	return CallNextHookEx( g_hKeyHook, nCode, wParam, lParam);
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_CREATE :
			//	키보드 후킹
			g_hKeyHook = SetWindowsHookEx( WH_KEYBOARD, KeyHookProc, NULL, GetCurrentThreadId());
			break;

		case WM_DESTROY:
			{
				UnhookWindowsHookEx( g_hKeyHook);	//	키보드 후킹 종료
				PostQuitMessage(0);
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DROPFILES :
			{
				if( (g_pFramework != NULL) && (g_pFramework->IsExactTypeOf( i3ViewerFramework::GetClassMeta())))
				{
					HDROP hDrop = (HDROP) wParam;
					char szPath[MAX_PATH + 1];

					DragQueryFile( hDrop, 0, szPath, sizeof(szPath) - 1);

					((i3ViewerFramework *) g_pFramework)->LoadI3S( szPath);

					DragFinish( hDrop);
				}
			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


#endif I3_WINDOWS
