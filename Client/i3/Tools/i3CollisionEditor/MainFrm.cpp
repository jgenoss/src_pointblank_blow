// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3CollisionEditor.h"

#include "MainFrm.h"
#include "GlobalVar.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, i3TDKDockFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, i3TDKDockFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_TOOLS_TERRAINAUTO, OnToolsTerrainauto)
	ON_COMMAND(ID_TOOLS_VOLUMELIGHTASSIGN, OnToolsVolumelightassign)
END_MESSAGE_MAP()


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDockFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		m_PanelToolbar.Create( CPanelToolbar::IDD, this);
		m_PanelToolbar.SetWindowText( "Tools");
		DockWindow( &m_PanelToolbar, "U", 100, 100);

		m_PanelSceneGraph.Create( CPanelSceneGraph::IDD, this);
		m_PanelSceneGraph.SetWindowText( "Scene-Graph");
		DockWindow( &m_PanelSceneGraph, "D", 100, 100);

		m_PanelViewer.Create( CPanelViewer::IDD, this);
		m_PanelViewer.SetWindowText( "Viewer");
		DockWindow( &m_PanelViewer, "DR", 100, 100);		

		m_PanelGroup.Create( CPanelGroup::IDD, this);
		m_PanelGroup.SetWindowText( "Collider Group");
		DockWindow( &m_PanelGroup, "DD", 100, 100);

		m_PanelSpec.Create( CPanelSpec::IDD, this);
		m_PanelSpec.SetWindowText( "Specification");
		DockWindow( &m_PanelSpec, "DRD", 100, 100);
	}

	LoadDockContext();

	DragAcceptFiles( TRUE);

	{
		g_pProject = Project::NewObject();
	}

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

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1], Ext[32];
	char szTempPath[MAX_PATH], * pPath;
	char szName[MAX_PATH];

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

		i3String::SplitFileExt( TempName, Ext, sizeof(Ext) - 1);
		i3String::Copy( szTempPath, TempName);

		pPath = i3String::SplitFileName( szTempPath, szName, FALSE);
		*pPath = 0;

		if( i3String::Compare( Ext, "I3S") == 0)
		{
			OpenProject( TempName);
		}
	}

	DragFinish( hDropInfo);
}

void	CMainFrame::NewProject(void)
{
	g_pProject->Reset();
}

void	CMainFrame::OpenProject( const char * pszPath)
{
	g_pProject->LoadI3S( pszPath);
}

void	CMainFrame::SaveProject( const char * pszPath)
{
}

void CMainFrame::OnDestroy()
{
	I3_SAFE_RELEASE( g_pProject);

	i3TDKDockFrameWnd::OnDestroy();
}

void CMainFrame::OnFileOpen()
{
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Scene-Graph File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK) return;

	g_pProject->LoadI3S( LPCTSTR( Dlg.GetPathName()));
}

void CMainFrame::OnFileSave()
{
	if( g_pProject->getFileName()[0] == 0)
	{
		OnFileSaveas();
		return;
	}

	g_pProject->SaveI3S();
}

void CMainFrame::OnFileSaveas()
{
	CFileDialog	Dlg( FALSE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Scene-Graph File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)	return;

	g_pProject->setFileName( LPCTSTR( Dlg.GetPathName()));
	g_pProject->SaveI3S();
}

void CMainFrame::OnToolsTerrainauto()
{
	g_pProject->AutoTerrainAssign();
}

void CMainFrame::OnToolsVolumelightassign()
{
	g_pProject->VolumeLightAssign();
}
