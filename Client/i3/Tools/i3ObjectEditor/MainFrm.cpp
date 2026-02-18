// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "MainFrm.h"
#include "GlobalVar.h"
#include "DlgOption.h"
#include "DlgRefObject.h"
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
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_TOOLS_OPTION, OnToolsOption)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_TOOLS_REFERENCEOBJECTS, OnToolsReferenceobjects)
END_MESSAGE_MAP()


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
	I3_SAFE_RELEASE( g_pObject);
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
		g_pOption = new COptionInfo;
	}

	{
		m_PanelState.Create( CPanelState::IDD, this);
		m_PanelState.SetWindowText( "State");
		DockWindow( &m_PanelState, "L", 100, 100);

		m_PanelViewer.Create( CPanelViewer::IDD, this);
		m_PanelViewer.SetWindowText( "Preview");
		DockWindow( &m_PanelViewer, "R", 100, 100);

		m_PanelSpec.Create( CPanelSpec::IDD, this);
		m_PanelSpec.SetWindowText( "Specification");
		DockWindow( &m_PanelSpec, "RR", 100, 100);

		m_PanelTimeEvent.Create( CPanelTimeEvent::IDD, this);
		m_PanelTimeEvent.SetWindowText( "Tracks");
		DockWindow( &m_PanelTimeEvent, "RLD", 100, 100);

	}

	LoadDockContext();

	DragAcceptFiles( TRUE);

	OnFileNew();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !i3TDKDockFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame 진단

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


// CMainFrame 메시지 처리기

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 그렇지 않으면 기본 처리합니다.
	return i3TDKDockFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnDestroy()
{
	if( g_pOption != NULL)
	{
		delete g_pOption;
		g_pOption = NULL;
	}

	I3_SAFE_RELEASE( g_pObject);

	i3Game::getEffectManager()->RemoveAllEffect();

	i3TDKDockFrameWnd::OnDestroy();
}

void CMainFrame::OnNewObject(void)
{
	I3_SAFE_RELEASE( g_pObject);

	g_pObject = CObjectTemplate::new_object();

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SCENE, g_pObject);

	m_szFileName[0] = 0;

	i3Game::getEffectManager()->RemoveAllEffect();
}

void CMainFrame::OnLoadObject( const char * pszPath)
{
	i3ResourceFile file;

	i3Game::getEffectManager()->RemoveAllEffect();

	if( file.Load( pszPath) == STREAM_ERR)
	{
		NotifyBox( m_hWnd, "%s 파일을 열 수가 없다. 흠...이건 심각하다...", pszPath);
		return;
	}

	CObjectTemplate * pObj = (CObjectTemplate *) file.getKeyObject();
	
	if( !i3::kind_of<CObjectTemplate*>(pObj)) //->IsTypeOf( CObjectTemplate::static_meta()) == FALSE)
	{
		NotifyBox( m_hWnd, "날 속이려 하는거냐. 확장자만 i3Obj지 전혀 i3ObjectEditor 파일이 아니자나!");
		return;
	}

	I3_REF_CHANGE( g_pObject, pObj);

	strcpy( m_szFileName, pszPath);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SCENE, g_pObject);
}

void CMainFrame::OnSaveObject( const char * pszPath)
{
	i3ResourceFile file;

	if( g_pObject == NULL)
	{
		NotifyBox( m_hWnd, "저장할 내용이 없구먼...");
		return;
	}

	g_pObject->Flush();

	file.setKeyObject( g_pObject);

	if( file.Save( pszPath) == STREAM_ERR)
	{
		NotifyBox( m_hWnd, "저장이 안된다. 용량문제? 일 가능성은 별로 없고...내가 만든 버그일 가능성이 더...--;;");
		return;
	}

	strcpy( m_szFileName, pszPath);
}

void CMainFrame::OnFileNew()
{
	OnNewObject();
}

void CMainFrame::OnFileOpen()
{
	CFileDialog	Dlg( TRUE, "i3ObjectEditor", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3ObjectEditor File(*.i3ObjectEditor)|*.i3ObjectEditor||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	OnLoadObject( LPCTSTR( Dlg.GetPathName()));
}

void CMainFrame::OnFileSave()
{
	if( m_szFileName[0] == 0)
	{
		OnFileSaveas();
		return;
	}

	OnSaveObject( m_szFileName);
}

void CMainFrame::OnFileSaveas()
{
	CFileDialog	Dlg( FALSE, "i3ObjectEditor", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"i3ObjectEditor File(*.i3ObjectEditor)|*.i3ObjectEditor||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	OnSaveObject( LPCTSTR( Dlg.GetPathName()));
}

void CMainFrame::OnToolsOption()
{
	CDlgOption dlg;

	dlg.DoModal();
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1], Ext[32];
//	char szTempPath[MAX_PATH], * pPath;
//	char szName[MAX_PATH];
	bool bProcessed = false;

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

//		i3String::SplitFileExt( TempName, Ext, sizeof(Ext) - 1);
		i3::extract_fileext(TempName, Ext);
/*
		i3::safe_string_copy( szTempPath, TempName, MAX_PATH );
		pPath = i3String::SplitFileName( szTempPath, szName, FALSE);
		*pPath = 0;
*/
		if( i3::generic_is_iequal( Ext, "I3S") )
		{
			g_pObject->LoadAndAddState( TempName);
			bProcessed = true;
		}
		else if (i3::generic_is_iequal(Ext, "i3ObjectEditor"))
		{
			OnLoadObject(TempName);
		}
	}

	DragFinish( hDropInfo);
}

void CMainFrame::OnStartPlay(void)
{
	m_PanelTimeEvent.OnStartPlay();
}

void CMainFrame::OnStartPause(void)
{
	m_PanelTimeEvent.OnStartPause();
}

void CMainFrame::OnChangeTime( REAL32 tm)
{
}

void CMainFrame::OnFileExport()
{
	CFileDialog	Dlg( FALSE, "i3Obj", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"i3Object File(*.i3Obj)|*.i3Obj||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	g_pObject->Export( LPCTSTR( Dlg.GetPathName()));
}

void CMainFrame::OnToolsReferenceobjects()
{
	CDlgRefObject dlg;

	dlg.Execute( g_pObject);
}
