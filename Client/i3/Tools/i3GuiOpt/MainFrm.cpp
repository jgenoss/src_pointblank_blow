// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "i3GuiOpt.h"
#include "MainFrm.h"
#include "GlobalVar.h"
#include "i3Base/string/ext/extract_fileext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, i3TDKDockFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, i3TDKDockFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_COMMAND(ID_TOOLS_PROJECTCONFIGURATION, &CMainFrame::OnToolsProjectconfiguration)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
	I3_SAFE_RELEASE(g_pEnvSet);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDockFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		HICON hIcon;
		HINSTANCE hInst = AfxGetApp()->m_hInstance;

		hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( IDR_MAINFRAME), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		::SetClassLong( m_hWnd, GCL_HICONSM, (LONG) hIcon);

		hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( IDR_MAINFRAME), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
		::SetClassLong( m_hWnd, GCL_HICON, (LONG) hIcon);
	}

	{
		g_pOption = COptionInfo::new_object();
	}

	{
		g_pOptConfig = COptConfig::new_object();
	}

	

	{
		g_pEnvSet = i3TDKEnvSet::new_object();
	}

	{
		m_Panel_Palette.Create( CPanel_Palette::IDD, this);
		DockWindow( &m_Panel_Palette, "L", 120, 720);

		m_Panel_Files.Create( CPanel_Files::IDD, this);
		DockWindow( &m_Panel_Files, "R", 120, 720);

		m_Panel_Log.Create( CPanel_Log::IDD, this);
		DockWindow( &m_Panel_Log, "LD", 120, 80);

		m_Panel_Opt.Create( CPanel_Opt::IDD, this);
		DockWindow( &m_Panel_Opt, "LUR", 120, 720);

		m_Panel_Viewer.Create( CPanel_Viewer::IDD, this);
		DockWindow( &m_Panel_Viewer, "LURR", 120, 120);
	}

	LoadDockContext();

	DragAcceptFiles( TRUE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !i3TDKDockFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	i3TDKDockFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	i3TDKDockFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// otherwise, do default handling
	return i3TDKDockFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void CMainFrame::OnDestroy()
{
	if( g_pOption != NULL)
	{
		i3::destroy_instance(g_pOption); //delete g_pOption;
		g_pOption = NULL;
	}

	i3TDKDockFrameWnd::OnDestroy();

	I3_SAFE_RELEASE( g_pOptConfig);
	g_FileList.clear();
}

void CMainFrame::OnToolsProjectconfiguration()
{
	i3TDKDlgEnvSet dlg;

	dlg.Execute();
}

void CMainFrame::StartProcessing(void)
{
	m_Panel_Files.StartProcessing();
	m_Panel_Opt.StartProcessing();
	m_Panel_Palette.StartProcessing();
	m_Panel_Viewer.StartProcessing();
	m_Panel_Log.StartProcessing();
}

void CMainFrame::EndProcessing(void)
{
	m_Panel_Files.EndProcessing();
	m_Panel_Opt.EndProcessing();
	m_Panel_Palette.EndProcessing();
	m_Panel_Viewer.EndProcessing();
	m_Panel_Log.EndProcessing();
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case WM_OPT_FILE :		m_Panel_Log.OnOptFile( (INT32) wParam);	break;
		case WM_OPT_OPT :		m_Panel_Log.OnOptOpt( (INT32) wParam);	break;
		case WM_OPT_END :		
			m_Panel_Log.OnOptEnd();

			EndProcessing();
			break;

		case WM_OPT_CANCEL :	
			m_Panel_Log.OnOptCancel();	

			EndProcessing();
			break;
	}

	return i3TDKDockFrameWnd::WindowProc(message, wParam, lParam);
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	int count, i; 
	char FileName[MAX_PATH], szExt[128];

	count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < count; i++)
	{
		DragQueryFile( hDropInfo, i, FileName, sizeof(FileName) - 1);

	//	i3String::SplitFileExt( FileName, szExt, sizeof(szExt));
		i3::extract_fileext(FileName, szExt);

		if( i3::generic_is_iequal( szExt, "i3OptConfig") )
		{
			// Optimizer Configuration File
			m_Panel_Opt.LoadConfig( FileName);
		}
		else if( i3::generic_is_iequal( szExt, "i3OptList") )
		{
			m_Panel_Files.LoadFileList( FileName);
		}
		else
		{
			m_Panel_Files.AddFile( FileName);
		}
	}
}
