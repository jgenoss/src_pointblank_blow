// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// i3UIEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include <AFXPRIV.H>	// 응용 프로그램을 마지막으로 수정할 때 전환 함수에 사용된 Windows 메시지(WM_INITIALUPDATE)를 정의하는 새 헤더 파일이 포함되어야 합니다.
#include "i3UIEditor.h"
#include "MainFrm.h"

#include "i3UIEditorDoc.h"
#include "i3UIEditorView.h"
#include "i3UIEMeta.h"
#include "GlobalVariables.h"
#include "UIGlobalRes.h"
//#include "BugTrap/BugTrap.h"

#include "UndoSystem2/UIUndoSystem.h"

#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/extract_fileext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3UIEditorApp

BEGIN_MESSAGE_MAP(Ci3UIEditorApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &Ci3UIEditorApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// Ci3UIEditorApp construction
Ci3UIEditorApp::Ci3UIEditorApp()
{

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only Ci3UIEditorApp object

Ci3UIEditorApp theApp;

static i3GameInterface *	TryLoadGame( const char * pszPath)
{
	char LogBuf[MAX_PATH];
	char szExt[127];

//	i3String::SplitFileExt( pszPath, szExt, sizeof( szExt) - 1);
	i3::extract_fileext(pszPath, szExt);

	if( i3::generic_is_iequal( szExt, "DLL") == false &&
		i3::generic_is_iequal( szExt, "BIN") == false &&
		i3::generic_is_iequal( szExt, "i3Exec") == false )
	{
		i3::snprintf( LogBuf, MAX_PATH, "* Invalid Exec. extension : %s\r\n", szExt);
		I3TRACE(LogBuf);
		return NULL;
	}

	i3::snprintf( LogBuf, MAX_PATH, "* load : %s\r\n", pszPath);
	I3TRACE(LogBuf);

	char CurDir[MAX_PATH] = {};

	CString str;

	str = theApp.GetProfileString( LPCTSTR("Settings"), LPCTSTR("Working Path"));

//	const char * pszWorkingFolder = i3ResourceFile::GetWorkingFolder();
	if( str.GetString()[ 0] == 0)
	//if( pszWorkingFolder[0] == 0)
	{
		GetCurrentDirectory(MAX_PATH, CurDir);
	}
	else
	{
		//i3::string_ncopy_nullpad( CurDir, pszWorkingFolder, MAX_PATH-1);
		i3::string_ncopy_nullpad( CurDir, str.GetString(), MAX_PATH-1);
	}

	i3::snprintf( LogBuf, MAX_PATH, "* Current Dir : %s\r\n", CurDir);
	I3TRACE(LogBuf);

	char Path[MAX_PATH] = {};
	char ExeFullPath[MAX_PATH] = {};
	char LibPath[MAX_PATH] = {};


	// exe와 같은경로에 있는 Exec를 로드합니다.
	GetModuleFileName(NULL, ExeFullPath, MAX_PATH);
//	i3String::SplitPath(ExeFullPath, Path);
	i3::extract_directoryname(ExeFullPath, Path);
	i3::snprintf(LibPath, MAX_PATH, "%s\\%s", CurDir, pszPath);


	g_hModule = ::LoadLibrary( LibPath );
	if ( g_hModule == NULL )
	{
		char szTemp[256];

		DWORD error = ::GetLastError();
		i3::snprintf( szTemp, 256, "* LibPath : %s - error:%d\r\n", LibPath, error);
		I3TRACE(szTemp);

		
		I3TRACE("load lib fail\r\n");
		
		
		return NULL;
	}

	I3_CREATE_GAME_INTERFACE_PROC pProc;

	pProc = (I3_CREATE_GAME_INTERFACE_PROC) GetProcAddress( g_hModule, "i3CreateGameInterface");
	if( pProc == NULL)
	{
		I3TRACE("can not found proc\r\n");
		return NULL;
	}

	return pProc( Path);
}

// Ci3UIEditorApp initialization

BOOL Ci3UIEditorApp::InitInstance()
{
	#if defined (I3_DEBUG)//메모리 세팅 
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
//	INT32 i;
//	for( i = 1576900; i < 1576901; ++i)
//	{
//		_CrtSetBreakAlloc(i);
//	}

	//_CrtSetBreakAlloc(1667425);
	#endif

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Zepetto"));
	LoadStdProfileSettings(9);  // Load standard INI file options

	CString ExecModuleName = theApp.GetProfileString(LPCTSTR("Settings"), LPCTSTR("ExecModuleName"));
	if(ExecModuleName.GetLength() != 0)
	{
		g_pInterface = TryLoadGame((const char*)LPCTSTR( ExecModuleName));
		if( g_pInterface == NULL)
		{
			MessageBox( NULL, "i3Exec 파일을 로드 할 수 없습니다. 다시 설치해 주세요.\n", "에러", MB_OK);

			i3MemoryInit();
		}
		else
		{
			INT32 runExecModule = theApp.GetProfileInt( LPCTSTR("Settings"), LPCTSTR("runExecModule"), 1);
			setRunModule( (runExecModule > 0) ? true : false);

			if( getRunModuleEnable())
			{
				i3MemoryInit( g_pInterface->GetInitialMemSize());

				g_pInterface->OnRegisterMeta();
			}
			else
			{
				i3MemoryInit();
			}
		}
	}
	else
	{
		i3MemoryInit();
	}

	{
		i3Error::Init();
		
		i3Error::SetExceptionHandlerOption( I3MINDUMP_WITHDATASEGS );
		i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_CALLBACK | I3ERR_CHN_DEBUG);
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_CALLBACK | I3ERR_CHN_DEBUG);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_DEBUG);
		i3Error::SetChannelName( I3LOG_FATAL, "i3UIEditor.log");
		i3Error::SetChannelName( I3LOG_WARN, "i3UIEditor.log");

		i3Error::SetCallback( I3LOG_NOTICE, CallbackLog, NULL);
		i3Error::SetCallback( I3LOG_WARN, CallbackLog, NULL);
		i3Error::SetCallback( I3LOG_FATAL, CallbackLog, NULL);

		i3TDKRegisterMetas();
		RegisterUIEMetas();

		UIGlobalRes::Init();

		i3UI::setToolDevMode(true);
	}

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(Ci3UIEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(Ci3UIEditorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	

	// 다중뷰 설정
	{
		CView* pActiveView = ((CMainFrame*)m_pMainWnd)->GetActiveView();

		m_pScreenView = pActiveView;
		m_pTemplateView = (CView*)new Ci3UIEditorView_Template;

		CDocument* pCurrentDoc = ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();

		// Initialize a CCreateContext to point to the active document.
		// With this context, the new view is added to the document
		// when the view is created in CView::OnCreate().
		CCreateContext newContext;
		newContext.m_pNewViewClass = NULL;
		newContext.m_pNewDocTemplate = NULL;
		newContext.m_pLastView = NULL;
		newContext.m_pCurrentFrame = NULL;
		newContext.m_pCurrentDoc = pCurrentDoc;

		// The ID of the initial active view is AFX_IDW_PANE_FIRST.
		// Incrementing this value by one for additional views works
		// in the standard document/view case but the technique cannot
		// be extended for the CSplitterWnd case.
		UINT viewID = AFX_IDW_PANE_FIRST + 1;
		CRect rect(0, 0, 0, 0); // Gets resized later.

		// Create the new view. In this example, the view persists for
		// the life of the application. The application automatically
		// deletes the view when the application is closed.
		m_pTemplateView->Create(NULL, "TemplateView", WS_CHILD, rect, m_pMainWnd, viewID, &newContext);

		// When a document template creates a view, the WM_INITIALUPDATE
		// message is sent automatically. However, this code must
		// explicitly send the message, as follows.
		m_pTemplateView->SendMessage(WM_INITIALUPDATE, 0, 0);
	}

	UIGlobalRes::InitAfter();

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	
	((CFrameWnd*)m_pMainWnd)->SetActiveView( m_pScreenView);
	SwitchView();

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}


I3ERROR_HANDLE Ci3UIEditorApp::CallbackLog( I3LOG_TYPE type, char * pszWhere, char * pszMsg, void * pUserData)
{
	CMainFrame * pMain = (CMainFrame *) theApp.m_pMainWnd;

	if( pMain)
	{
		CUIPaneOutput* pOutput = pMain->getOutputPane();

		if( pOutput)
		{
			char strMessage[256];

			switch( type)
			{
			case I3LOG_FATAL:
				sprintf( strMessage, "\n\n[Fatal]\n\n %s : %s", pszWhere, pszMsg);
				break;
			case I3LOG_WARN:
				sprintf( strMessage, "\n\n[Warning]\n\n %s : %s", pszWhere, pszMsg);
				break;
			default:
				sprintf( strMessage, "\n\n[Notice]\n\n %s", pszMsg);
				break;
			}

			//	Log 창에 메시지 출력
			pOutput->OutputNotice( strMessage, true);
		}
	}

	if( type == I3LOG_FATAL)
		return I3ERROR_HANDLE_BREAK;
	else
		return I3ERROR_HANDLE_CONTINUE;	
}


// switches to the new view and returns the previous view
CView * Ci3UIEditorApp::SwitchView( EVIEW eView)
{
	if( m_pMainWnd == NULL)
		return NULL;

	CView* pActiveView = ((CMainFrame*)m_pMainWnd)->GetActiveView();
	CView* pNewView= NULL;

	/*{
		BOOL bScrPane = (eView == EVIEW_SCREEN) ? TRUE : FALSE;
		
		CMainFrame* pMain = (CMainFrame*)m_pMainWnd;
		pMain->getScreenPane()->ShowPane( TRUE, FALSE, bScrPane);
		pMain->getTemplatePane()->ShowPane( TRUE, FALSE, !bScrPane);
	}*/
	
	switch( eView)
	{
	case EVIEW_SCREEN:		pNewView = m_pScreenView;	break;
	case EVIEW_TEMPLATE:	pNewView = m_pTemplateView;	break;
	default:				pNewView = m_pScreenView;	break;
	}

	// ok?
	if( pActiveView == pNewView)	return pActiveView;

	if( pActiveView != NULL && pNewView != NULL)
	{
		// Exchange view window IDs so RecalcLayout() works.
		UINT temp = ::GetWindowLong(pActiveView->m_hWnd, GWL_ID);
		::SetWindowLong(pActiveView->m_hWnd, GWL_ID, ::GetWindowLong(pNewView->m_hWnd, GWL_ID));
		::SetWindowLong(pNewView->m_hWnd, GWL_ID, temp);
	}
	
	if( pActiveView != NULL)
	{
		pActiveView->ShowWindow(SW_HIDE);
	}

	if( pNewView != NULL)
	{
		pNewView->ShowWindow(SW_SHOW);
		
		((CFrameWnd*) m_pMainWnd)->SetActiveView(pNewView);
		((CFrameWnd*) m_pMainWnd)->RecalcLayout();

		pNewView->Invalidate();

		bool bScreen = (eView == EVIEW_SCREEN) ? true : false;
		((CMainFrame*)m_pMainWnd)->UpdateStatusBar( bScreen);
	}

	return pActiveView;

	//previous version
	//if( pActiveView == NULL || pNewView == NULL)
	//	return NULL;

	//// Exchange view window IDs so RecalcLayout() works.
	//UINT temp = ::GetWindowLong(pActiveView->m_hWnd, GWL_ID);
	//::SetWindowLong(pActiveView->m_hWnd, GWL_ID, ::GetWindowLong(pNewView->m_hWnd, GWL_ID));
	//::SetWindowLong(pNewView->m_hWnd, GWL_ID, temp);

	//pActiveView->ShowWindow(SW_HIDE);
	//pNewView->ShowWindow(SW_SHOW);

	//((CFrameWnd*) m_pMainWnd)->SetActiveView(pNewView);
	//((CFrameWnd*) m_pMainWnd)->RecalcLayout();

	//pNewView->Invalidate();

	////Update StatusBar Text
	//bool bScreen = (eView == EVIEW_SCREEN) ? true : false;
	//((CMainFrame*)m_pMainWnd)->UpdateStatusBar( bScreen);
	//	
	//return pActiveView;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void Ci3UIEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// Ci3UIEditorApp customization load/save methods

void Ci3UIEditorApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void Ci3UIEditorApp::LoadCustomState()
{
}

void Ci3UIEditorApp::SaveCustomState()
{
}

// Ci3UIEditorApp message handlers




int Ci3UIEditorApp::ExitInstance()
{
	UIGlobalRes::Close();

	I3_SAFE_RELEASE( g_pFramework);
	I3_SAFE_RELEASE( g_pViewer);

	if( g_pInterface != NULL && getRunModuleEnable())
	{
		g_pInterface->OnExitInstance();
	}

	i3TDKGlobalRes::Close();

	i3System::TerminateSys();

	//i3ElementBase::DumpObjects();

	if( g_hModule != NULL)
	{
		FreeLibrary( g_hModule);
	}
//	new int[625/4];				// 일부러 낸 릭..crtdbg 씹혔는지 체크용도로 주석건채로 그냥 놔두시기 바랍니다..(12.09.19.수빈)
	return CWinAppEx::ExitInstance();
}


BOOL Ci3UIEditorApp::PreTranslateMessage(MSG* pMsg)
{
	/*
	switch( pMsg->message)
	{
	case WM_KEYDOWN:
		{
			bool bRet = FALSE;
			if( GetKeyState( VK_CONTROL) & 0x8000)
			{
				switch( pMsg->wParam)
				{
				case 'Y':	
					{	
						CMainFrame* pMain = (CMainFrame*)m_pMainWnd;
						if( pMain != NULL)
						{
							pMain->OnRedo();
							bRet = TRUE;
						}
					}
					break;
				case 'Z':
					{
						CMainFrame* pMain = (CMainFrame*)m_pMainWnd;
						if( pMain != NULL)
						{
							pMain->OnUndo();
							bRet = TRUE;
						}
					}
					break;
				case 'P':
					{
//						UIUndo::TraceCurrentStack();
						break;
					}
				default:	break;
				}
			}
			if( bRet)	return TRUE;
		}
	}
	*/

	return CWinAppEx::PreTranslateMessage(pMsg);
}

int Ci3UIEditorApp::Run()
{
	ASSERT_VALID(this);
	_AFX_THREAD_STATE* pState = AfxGetThreadState();

	// for tracking the idle time state
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;

	for (;;)
	{
		while (bIdle && !PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE))
		{
			if (!OnIdle(lIdleCount++))
				bIdle = FALSE; // assume "no idle" state
		}

		while (PeekMessage(&pState->m_msgCur, NULL, NULL, NULL, PM_NOREMOVE))
		{
			if (!PumpMessage())
				return ExitInstance();

			if (IsIdleMessage(&pState->m_msgCur))
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}
		}

		// 		if(!g_Flag.m_bIsActive) 
		// 			GetMessage( &pState->m_msgCur, NULL, 0U, 0U );

		Ci3UIEditorView * pView = (Ci3UIEditorView*)theApp.getScreenView();
		pView->OnDraw(NULL);
	}

	return CWinApp::Run();
}