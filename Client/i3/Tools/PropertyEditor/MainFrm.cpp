// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "PropertyEditor.h"
#include "GlobalVariable.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#include "PropertyEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_INDICATOR_STRING1,
	ID_INDICATOR_STRING2,
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
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndSearchToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndSearchToolBar.LoadToolBar(IDR_TB_SEARCH) || 
		!m_wndSearchToolBar.CreateAdditional(ID_TB_SEARCH_CB_SEARCH_LIST, 500))
	{
		TRACE0("검색 툴바 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	InitStatusBar();

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndSearchToolBar.EnableDocking(CBRS_ALIGN_RIGHT | CBRS_ALIGN_TOP);

	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndToolBar);
	DockControlBarLeftOf(&m_wndSearchToolBar, &m_wndToolBar);
	
	return 0;
}

void CMainFrame::DockControlBarLeftOf(CControlBar* pBar, CControlBar* pLeftOf)
{
    CRect rect;
    DWORD dw;
    UINT n;

    RecalcLayout();
    pLeftOf->GetWindowRect(&rect);
    rect.OffsetRect(1,10);
    dw=pLeftOf->GetBarStyle();

    n = 0;
    n = (dw & CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
    n = (dw & CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
    n = (dw & CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
    n = (dw & CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;
    
    DockControlBar(pBar,n,&rect);
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// 문서가 열릴때 자동으로 FrameWnd의 타이틀 텍스트를 수정하는
	// 플래그를 제거합니다.
	cs.style &= ~(LONG)FWS_ADDTOTITLE;

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.
	cs.x  = g_pProfile->m_MainWnd.left;
	cs.y  = g_pProfile->m_MainWnd.top;
	cs.cx = g_pProfile->m_MainWnd.right - cs.x;
	cs.cy = g_pProfile->m_MainWnd.bottom - cs.y;

	return TRUE;
}

void	CMainFrame::SetWindowTextEx(LPCTSTR pText)
{
	if ( NULL == pText || (NULL != pText && '\0' == *pText) )
		pText = _T("Noname");

	CString	title;
	title.Format(_T("Item & Weapon Editor - %s"), pText);

	SetWindowText(title);
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
	if( ::IsWindow( m_hWnd))
	{
		GetWindowRect( &g_pProfile->m_MainWnd );
	}

	CFrameWnd::OnDestroy();
}


CMainFrame *	AfxGetMainFrame()
{
	return (CMainFrame *)AfxGetMainWnd();
}


void CMainFrame::InitStatusBar()
{
	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_STRING1, SBPS_NORMAL | SBPS_NOBORDERS, 0 );
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_STRING2, SBPS_NORMAL | SBPS_NOBORDERS, 0 );
	m_wndStatusBar.SetPaneInfo(2, ID_SEPARATOR, SBPS_NORMAL | SBPS_STRETCH | SBPS_NOBORDERS, 0);
}
void CMainFrame::ShowStatusBar()
{
	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_STRING1, SBPS_NORMAL, 100 );
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_STRING2, SBPS_NORMAL, 250 );
}
void CMainFrame::HideStatusBar()
{
	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_STRING1, SBPS_NORMAL | SBPS_NOBORDERS, 0 );
	m_wndStatusBar.SetPaneText(0, "" );
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_STRING2, SBPS_NORMAL | SBPS_NOBORDERS, 0 );
	m_wndStatusBar.SetPaneText(1, "" );
}

void CMainFrame::ResizeStatusBar(int cxWidth)
{

}

void CMainFrame::ResetStatusText1()
{
	m_wndStatusBar.SetPaneText( 0, "" );
}
void CMainFrame::ResetStatusText2()
{
	m_wndStatusBar.SetPaneText( 1, "" );
}
void CMainFrame::ResetStatusText3()
{
	m_wndStatusBar.SetPaneText( 2, "" );
}

void CMainFrame::SetStatusText1(const CString & Text)
{
	m_wndStatusBar.SetPaneText( 0, Text );
}
void CMainFrame::SetStatusText2(const CString & Text)
{
	m_wndStatusBar.SetPaneText( 1, Text );
}
void CMainFrame::SetStatusText3(const CString & Text)
{
	m_wndStatusBar.SetPaneText( 2, Text );
}