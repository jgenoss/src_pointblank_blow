// PackScriptView.cpp : implementation file
//

#include "stdafx.h"
#include "RscTool.h"
#include "PackScriptView.h"

#include "PackScriptLeftView.h"
#include "PackScriptRightView.h"
#include "PathView.h"
#include "PackDiffView.h"
#include "LocaleStringDiffView.h"

// PackScriptView

IMPLEMENT_DYNCREATE(PackScriptView, CView)




PackScriptView::PackScriptView()
{

}

PackScriptView::~PackScriptView()
{

}

BEGIN_MESSAGE_MAP(PackScriptView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()




// PackScriptView drawing

void PackScriptView::OnDraw(CDC* pDC)
{
//	CDocument* pDoc = GetDocument();
	// TODO: add draw code here

//	pDC->TextOut(0,0, L"테스트");

}


// PackScriptView diagnostics

#ifdef _DEBUG
void PackScriptView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void PackScriptView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// PackScriptView message handlers

int PackScriptView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	::SetWindowTheme(m_splitter.GetSafeHwnd(), L"explorer", 0);


	CCreateContext cc;
	memset(&cc, 0, sizeof(CCreateContext));
	  
	if (!m_splitter.CreateStatic(this, 1, 4, WS_CHILD | WS_VISIBLE))
		return FALSE;

	cc.m_pNewViewClass = RUNTIME_CLASS(PathView);
	if (!m_splitter.CreateView(0, 0, RUNTIME_CLASS(PathView), CSize(0,0), &cc))
		return FALSE;

	m_splitter_col3.CreateStatic(&m_splitter, 2, 1, WS_CHILD|WS_VISIBLE, m_splitter.IdFromRowCol(0, 3));
	
	cc.m_pNewViewClass = RUNTIME_CLASS(PackScriptLeftView);
	if (!m_splitter.CreateView(0, 1, RUNTIME_CLASS(PackScriptLeftView), CSize( 0, 0), &cc))
		return FALSE;
	  
	cc.m_pNewViewClass = RUNTIME_CLASS(PackDiffView);
	if (!m_splitter_col3.CreateView(0, 0, RUNTIME_CLASS(PackDiffView), CSize(0,0), &cc))
		return FALSE;

	cc.m_pNewViewClass = RUNTIME_CLASS(LocaleStringDiffView);
	if (!m_splitter_col3.CreateView(1, 0, RUNTIME_CLASS(LocaleStringDiffView), CSize(0,0), &cc))
		return FALSE;

	cc.m_pNewViewClass = RUNTIME_CLASS(PackScriptRightView);
	if (!m_splitter.CreateView(0, 2, RUNTIME_CLASS(PackScriptRightView), CSize(0, 0), &cc))
		return FALSE;
	
	PathView* pPathView = (PathView*)m_splitter.GetPane(0,0);
	PackScriptLeftView* pLeftView = (PackScriptLeftView*)m_splitter.GetPane(0,1);
	PackDiffView*       pPackDiffView = (PackDiffView*) m_splitter_col3.GetPane(0, 0);
	LocaleStringDiffView* pLocalStringDiffView = (LocaleStringDiffView*) m_splitter_col3.GetPane(1, 0);
	PackScriptRightView* pRightView = (PackScriptRightView*) m_splitter.GetPane(0,2);
	pPathView->SetLeftView(pLeftView);
	pLeftView->SetRightView(pRightView);
	pLeftView->SetPackDiffView(pPackDiffView);
	pLeftView->SetLocaleStringDiffView(pLocalStringDiffView);	
	m_splitter.SetRedraw(FALSE);	// SetRedraw는 그리지 않는 블록구간을 설정하는 함수임..
									// (창최대화된 상태로 생성시..OnInitialUpdate()이전에 창이 렌더링되는 MFC버그에 기인함 )

	return 0;
}

void PackScriptView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	
	m_splitter.MoveWindow(0, 0, cx, cy);

	// TODO: Add your message handler code here
}



void PackScriptView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	  
	m_splitter.SetRedraw(TRUE);					// SetRedraw는 그리지 않는 블록구간을 설정하는 함수임..
												// (창최대화된 상태로 생성시..OnInitialUpdate()이전에 창이 렌더링되는 MFC버그에 기인함 )
				
	CRect cr;
	GetClientRect(&cr);
	
	m_splitter.SetRowInfo(0, cr.Height(), 0);
	m_splitter.SetColumnInfo(0, cr.Width() / 8, 0);
	m_splitter.SetColumnInfo(1, cr.Width() / 4, 0);
	m_splitter.SetColumnInfo(2, cr.Width() * 2 / 5, 0);
	m_splitter.RecalcLayout(); //(0, 0, cr.Width(), cr.Height());
	
	m_splitter_col3.SetRowInfo(0, cr.Height() / 2, 0);
	m_splitter_col3.RecalcLayout();

	// without this, it seems that the splitter created has negative size
	// TODO: Add your specialized code here and/or call the base class
}


BOOL PackScriptView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
//	return CView::OnEraseBkgnd(pDC);
}

