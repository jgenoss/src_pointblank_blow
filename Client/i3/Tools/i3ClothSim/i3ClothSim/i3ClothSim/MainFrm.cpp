// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3ClothSim.h"

#include "MainFrm.h"
#include ".\mainfrm.h"
#include "ClothFramework.h"
#include "GlobalVariable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, i3TDKDockFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, i3TDKDockFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_DEBUG_DUMP, OnDebugDump)
	ON_WM_DROPFILES()
	ON_UPDATE_COMMAND_UI( ID_SELMASK_PARTICLE, OnUpdateSelMaskParticle)
	ON_UPDATE_COMMAND_UI( ID_SELMASK_SPRING, OnUpdateSelMaskSpring)
	ON_COMMAND( ID_SELMASK_PARTICLE, OnSelMaskParticle)
	ON_COMMAND( ID_SELMASK_SPRING, OnSelMaskSpring)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


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

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_SELECT_TOOL))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	{
		DockWindow( &m_wndToolBar, "U", 300, 20);

		m_PaneViewer.Create( CPaneViewer::IDD, this);
		DockWindow( &m_PaneViewer, "DL", 480, 320);

		m_PaneEnv.Create( CPaneEnvironment::IDD, this);
		DockWindow( &m_PaneEnv, "DLD", 180, 250);

		m_PaneObject.Create( PaneObject::IDD, this);
		DockWindow( &m_PaneObject, "DR", 200, 140);

		m_PaneParam.Create( CPaneParam::IDD, this);
		DockWindow( &m_PaneParam, "DRD", 200, 220);

		m_PaneSpring.Create( CPaneSpring::IDD, this);
		DockWindow( &m_PaneSpring, "DRDD", 200, 200);
	}

	LoadDockContext();

	DragAcceptFiles( TRUE);

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

void CMainFrame::OnDebugDump()
{
	m_pDock->DumpPath( 0);
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1];
	char szExt[32];

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

		i3String::SplitFileExt( TempName, szExt, sizeof(szExt) - 1);

		if( i3String::Compare( szExt, "I3S") == 0)
		{
			g_pFramework->LoadI3S( TempName);
		}
	}

	DragFinish( hDropInfo);
}

void CMainFrame::OnUpdateSelMaskParticle(CCmdUI *pCmdUI)
{
	if( g_pFramework->getSelectMask() & SELMASK_PARTICLE)
		pCmdUI->SetCheck( 1);
	else
		pCmdUI->SetCheck( 0);

	pCmdUI->Enable();
}

void CMainFrame::OnUpdateSelMaskSpring( CCmdUI * pCmdUI)
{
	if( g_pFramework->getSelectMask() & SELMASK_SPRING)
		pCmdUI->SetCheck( 1);
	else
		pCmdUI->SetCheck( 0);

	pCmdUI->Enable();
}

void CMainFrame::OnSelMaskParticle(void)
{
	if( g_pFramework->getSelectMask() & SELMASK_PARTICLE)
		g_pFramework->removeSelectMask( SELMASK_PARTICLE);
	else
		g_pFramework->addSelectMask( SELMASK_PARTICLE);
}

void CMainFrame::OnSelMaskSpring(void)
{
	if( g_pFramework->getSelectMask() & SELMASK_SPRING)
		g_pFramework->removeSelectMask( SELMASK_SPRING);
	else
		g_pFramework->addSelectMask( SELMASK_SPRING);
}
