// ChildView.cpp : CChildView 클래스의 구현
//

#include "stdafx.h"
#include "SimpleTest.h"
#include "ChildView.h"
#include ".\childview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CChildView 메시지 처리기

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER | WS_CLIPCHILDREN;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트
	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
	// 그리기 메시지에 대해서는 CWnd::OnPaint()를 호출하지 마십시오.
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rt;

		GetClientRect( &rt);

		m_DiagramView.Create( "View", WS_CHILD | WS_VISIBLE, rt, this, 100);

		m_Diagram.AddView( &m_DiagramView);
	}

	return 0;
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	return 0;
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if( ::IsWindow( m_DiagramView.m_hWnd))
	{
		m_DiagramView.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch( nChar)
	{
		case 'A' :
			m_DiagramView.setMetaToCreate( i3GICShapeRect::GetClassMeta());
			m_DiagramView.SetMouseMode( GIC_MOUSEMODE_CREATE);
			break;

		case 'S' :
			m_DiagramView.SetMouseMode( GIC_MOUSEMODE_NONE);
			break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
