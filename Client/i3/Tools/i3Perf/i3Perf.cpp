// i3Perf.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "i3Perf.h"
#include "i3PerfConfig.h"
#include "GlobalVar.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
BOOL	g_bFocused = FALSE;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	BOOL bContinue = TRUE;

	{
		i3MemoryInit();

		i3Error::Init();
		i3Error::SetExceptionHandlerOption( I3MINDUMP_WITHDATASEGS );
        i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_DEBUG );
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_CALLBACK);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_DEBUG | I3ERR_CHN_CALLBACK);

		i3FrameworkRegisterMetas();

		i3PerfConfig::RegisterMeta();
		i3PerfFramework::RegisterMeta();

		g_pConfig = i3PerfConfig::Load( "i3Perf.config");
	}

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_I3PERF, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_I3PERF);

	msg.message = WM_USER;
	msg.wParam = 0;

	// Main message loop:
	while((bContinue ) && (WM_QUIT != msg.message  ))
    {
        // Use PeekMessage() if the app is active, so we can use idle time to
        // render the scene. Else, use GetMessage() to avoid eating CPU time.
		INT32 rv;

		if( g_bFocused)
			rv = PeekMessage( &msg, NULL, 0, 0, PM_REMOVE);
		else
			rv = GetMessage( &msg, NULL, 0, 0);

		if( rv)
		{
            // Translate and dispatch the message
            if( 0 == TranslateAccelerator( g_hwndMain, hAccelTable, &msg ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
        		
		if( msg.message == WM_DESTROY)
			bContinue = FALSE;

		//for( i = 0; i < 60; i++)
		if( g_bFocused)
		{
			g_pViewer->Render();
		}
    }

	g_pConfig->Save();

	I3_SAFE_RELEASE( g_pFramework);
	I3_SAFE_RELEASE( g_pViewer);
	I3_SAFE_RELEASE( g_pConfig);
	
	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_I3PERF);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_I3PERF;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	INT32 cx, cy;
	i3VideoInfo video;
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;

	video.m_Width				= 640;
	video.m_Height				= 480;
	video.m_bFullScreen			= FALSE;

	hInst = hInstance; // Store instance handle in our global variable

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

	hWnd = CreateWindow(szWindowClass, szTitle, dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, cx, cy, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	g_hwndMain = hWnd;

	{
		g_pViewer = i3Viewer::NewObject();

		g_pViewer->AddState( I3_VIEWER_STATE_INGAME);
		//g_pViewer->SetPendingUpdateEnable( TRUE);

		if( g_pViewer->Create( (void *) hWnd, &video) == FALSE)
		{
			MessageBox( hWnd, "프로그램의 초기화에 실패했습니다(3).\r\n종료합니다.", "Error", MB_OK);
			return FALSE;
		}

		g_pFramework = i3PerfFramework::NewObject();
		g_pFramework->Create( g_pViewer);


		g_pFramework->SetPanelVisible( FALSE);
		g_pViewer->Play();
	}

	{
		if( g_pConfig->getLastI3S()[0] != 0)
		{
			g_pFramework->LoadI3S( g_pConfig->getLastI3S());
		}
	}

	DragAcceptFiles( hWnd, TRUE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
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
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;

	case WM_SHOWWINDOW:
		SetFocus( hWnd);
		break;

	case WM_SETFOCUS :
		g_bFocused = TRUE;
		break;

	case WM_KILLFOCUS :
		g_bFocused = FALSE;
		break;

	case WM_DROPFILES :
		{
			if( g_pFramework != NULL)
			{
				HDROP hDrop = (HDROP) wParam;
				char szPath[MAX_PATH + 1];

				DragQueryFile( hDrop, 0, szPath, sizeof(szPath) - 1);

				((i3ViewerFramework *) g_pFramework)->LoadI3S( szPath);

				DragFinish( hDrop);
			}
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
