// ItemDetailView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "ItemDetailFrame.h"

#include "MainFrm.h"

#include "ItemDetailView.h"


// ItemDetailView

IMPLEMENT_DYNCREATE(ItemDetailFrame, CFrameWndEx)

ItemDetailFrame::ItemDetailFrame()
{

}

ItemDetailFrame::~ItemDetailFrame()
{
}


BEGIN_MESSAGE_MAP(ItemDetailFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// ItemDetailView 메시지 처리기입니다.

void ItemDetailFrame::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	GetMainFrame()->DestroyPopupWndFromList(this);

	CFrameWndEx::PostNcDestroy();
}

int ItemDetailFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CCreateContext newContext;
	newContext.m_pNewViewClass = RUNTIME_CLASS(ItemDetailView);
	newContext.m_pCurrentFrame = this;
	newContext.m_pCurrentDoc   = this->GetActiveDocument();
	
	m_pView = (ItemDetailView*)this->CreateView(&newContext);

	m_pView->SendMessage(WM_INITIALUPDATE);
	
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	DWORD dwNoCloseBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE & ~(AFX_CBRS_CLOSE | AFX_CBRS_FLOAT);

	BOOL bRes = m_script_pane.Create(L"PackScript File List", this, 
		CRect(0,0,200,400),
		TRUE, 1212, 
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_BOTTOM
		,AFX_CBRS_REGULAR_TABS, dwNoCloseBarStyle);
	
	m_script_pane.EnableDocking(CBRS_ALIGN_ANY);
	
	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	DockPane(&m_script_pane);

	return 0;
}



void ItemDetailFrame::SetAssocFileSet(const AssocFileSetPtr& ptr)
{
	m_pView->SetAssocFileSet(ptr);
	m_script_pane.SetAssocFileSet(ptr);
}

BOOL ItemDetailFrame::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	return TRUE;
//	return CFrameWndEx::OnEraseBkgnd(pDC);
}

BOOL ItemDetailFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (!CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

	// Size the window to 1/3 screen size and center it

	int fullscreen_w = ::GetSystemMetrics(SM_CXSCREEN);
	int fullscreen_h = ::GetSystemMetrics(SM_CYSCREEN);

	cs.cx = fullscreen_w * 9 / 10;
	cs.cy = fullscreen_h * 5 / 6;

	cs.y = (fullscreen_h - cs.cy) / 2;
	cs.x = (fullscreen_w - cs.cx) / 2;

	return TRUE;
}
