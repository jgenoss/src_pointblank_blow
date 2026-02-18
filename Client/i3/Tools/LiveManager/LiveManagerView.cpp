
// LiveManagerView.cpp : CLiveManagerView 클래스의 구현
//

#include "stdafx.h"
#include "LiveManager.h"

#include "LiveManagerDoc.h"
#include "LiveManagerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLiveManagerView

IMPLEMENT_DYNCREATE(CLiveManagerView, CView)

BEGIN_MESSAGE_MAP(CLiveManagerView, CView)

	ON_WM_ASKCBFORMATNAME()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CLiveManagerView 생성/소멸

CLiveManagerView::CLiveManagerView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CLiveManagerView::~CLiveManagerView()
{
}

BOOL CLiveManagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CLiveManagerView 그리기

void CLiveManagerView::OnDraw(CDC* /*pDC*/)
{
	CLiveManagerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

void CLiveManagerView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CLiveManagerView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CLiveManagerView 진단

#ifdef _DEBUG
void CLiveManagerView::AssertValid() const
{
	CView::AssertValid();
}

void CLiveManagerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLiveManagerDoc* CLiveManagerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLiveManagerDoc)));
	return (CLiveManagerDoc*)m_pDocument;
}
#endif //_DEBUG



void CLiveManagerView::OnAskCbFormatName(UINT nMaxCount, LPTSTR lpszString)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnAskCbFormatName(nMaxCount, lpszString);
}

void CLiveManagerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnMouseMove(nFlags, point);
}
