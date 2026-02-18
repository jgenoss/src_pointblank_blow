// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3RSCGen.h"

#include "MainFrm.h"

#include "GlobalVar.h"

#include "OptionDlg.h"
#include ".\mainfrm.h"

#include "DlgRSCRefResCpy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_OPTION, OnViewOption)
	ON_COMMAND(ID_DEBUG_DUMPGDI, &CMainFrame::OnDebugDumpgdi)
	ON_COMMAND(ID_RSC_REFRESCPY, &CMainFrame::OnRSCRefResCpy)
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
CMainFrame *	AfxGetMainFrame()
{
	return (CMainFrame *)AfxGetMainWnd();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndRSGToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndRSGToolBar.LoadToolBar(IDR_RSGTOOLBAR) || 
		!m_wndRSGToolBar.CreateAdditional(ID_RSCSAVE, 100))
	{
		TRACE0("검색 툴바 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	m_wndRSGToolBar.EnableDocking(CBRS_ALIGN_RIGHT | CBRS_ALIGN_TOP);
	CRect rect;
	DWORD dw;
	UINT n;

	RecalcLayout();
	m_wndToolBar.GetWindowRect(&rect);
	rect.OffsetRect(1, 10);
	dw = m_wndToolBar.GetBarStyle();

	n = 0;
	n = (dw & CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw & CBRS_ALIGN_BOTTOM && n == 0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw & CBRS_ALIGN_LEFT && n == 0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw & CBRS_ALIGN_RIGHT && n == 0) ? AFX_IDW_DOCKBAR_RIGHT : n;

	DockControlBar(&m_wndRSGToolBar, n, &rect);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.
	cs.x  = g_pProfile->m_MainWndX;
	cs.y  = g_pProfile->m_MainWndY;
	cs.cx = g_pProfile->m_MainWndCX;
	cs.cy = g_pProfile->m_MainWndCY;

	return TRUE;
}


// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 메시지 처리기


void CMainFrame::OnDestroy()
{
	i3Registry::Destroy();

	CRect rect;

	if( ::IsWindow( m_hWnd))
	{
		GetWindowRect( &rect);

		g_pProfile->m_MainWndX	= rect.left;
		g_pProfile->m_MainWndY	= rect.top;
		g_pProfile->m_MainWndCX	= rect.Width();
		g_pProfile->m_MainWndCY	= rect.Height();

		g_pProfile->Save();
	}

	CFrameWnd::OnDestroy();
}

void CMainFrame::OnViewOption()
{
	// TODO: Add your command handler code here
	OptionDlg Dlg;

	Dlg.DoModal();
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnDebugDumpgdi()
{
	i3TDK::DumpGDIObjects();
}

void CMainFrame::OnRSCRefResCpy()
{
	CDlgRSCRefResCpy dlg;
	dlg.DoModal();
}
