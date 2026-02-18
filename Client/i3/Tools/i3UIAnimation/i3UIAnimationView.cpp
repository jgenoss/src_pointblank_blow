
// i3UIAnimationView.cpp : i3UIAnimationView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "i3UIAnimation.h"
#endif

#include "i3UIAnimationDoc.h"
#include "i3UIAnimationView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// i3UIAnimationView

IMPLEMENT_DYNCREATE(i3UIAnimationView, CView)

BEGIN_MESSAGE_MAP(i3UIAnimationView, CView)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// i3UIAnimationView 생성/소멸

i3UIAnimationView::i3UIAnimationView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

i3UIAnimationView::~i3UIAnimationView()
{
}

BOOL i3UIAnimationView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// i3UIAnimationView 그리기

void i3UIAnimationView::OnDraw(CDC* /*pDC*/)
{
	i3UIAnimationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// i3UIAnimationView 진단

#ifdef _DEBUG
void i3UIAnimationView::AssertValid() const
{
	CView::AssertValid();
}

void i3UIAnimationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

i3UIAnimationDoc* i3UIAnimationView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(i3UIAnimationDoc)));
	return (i3UIAnimationDoc*)m_pDocument;
}
#endif //_DEBUG


// i3UIAnimationView 메시지 처리기


void i3UIAnimationView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	if (pMainWnd)
	{
		// TODO: 여기에 메시지 처리기 코드를 추가합니다.
		if (IsWindow(pMainWnd->GetTextureView()->m_hWnd))
		{
			GetWindowRect(&pMainWnd->m_ViewWindowRect);
			pMainWnd->GetTextureView()->SetWindowPos(NULL, pMainWnd->m_ViewWindowRect.left - pMainWnd->m_MainWindowRect.left - 4,
				pMainWnd->m_ViewWindowRect.top - pMainWnd->m_MainWindowRect.top - 26,
				pMainWnd->m_ViewWindowRect.Width() - 8,
				pMainWnd->m_ViewWindowRect.Height() - 8, SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}
}


void i3UIAnimationView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CView::OnPaint()을(를) 호출하지 마십시오.

	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	if (pMainWnd)
	{
		GetWindowRect(&pMainWnd->m_ViewWindowRect);
		pMainWnd->GetTextureView()->SetWindowPos(NULL, pMainWnd->m_ViewWindowRect.left - pMainWnd->m_MainWindowRect.left - 4,
			pMainWnd->m_ViewWindowRect.top - pMainWnd->m_MainWindowRect.top - 26,
			pMainWnd->m_ViewWindowRect.Width() - 8,
			pMainWnd->m_ViewWindowRect.Height() - 8, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}
