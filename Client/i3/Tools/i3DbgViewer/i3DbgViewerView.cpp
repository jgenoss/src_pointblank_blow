// i3DbgViewerView.cpp : Ci3DbgViewerView 클래스의 구현
//

#include "stdafx.h"
#include "i3DbgViewer.h"

#include "i3DbgViewerDoc.h"
#include "i3DbgViewerView.h"
#include ".\i3dbgviewerview.h"

#include "GfxCmdCore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3DbgViewerView

IMPLEMENT_DYNCREATE(Ci3DbgViewerView, CView)

BEGIN_MESSAGE_MAP(Ci3DbgViewerView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// Ci3DbgViewerView 생성/소멸

Ci3DbgViewerView::Ci3DbgViewerView()
{
	m_pViewer = NULL;
	m_pFramework = NULL;
}

Ci3DbgViewerView::~Ci3DbgViewerView()
{
}

BOOL Ci3DbgViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// Ci3DbgViewerView 그리기

void Ci3DbgViewerView::OnDraw(CDC* /*pDC*/)
{
	Ci3DbgViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}


// Ci3DbgViewerView 진단

#ifdef _DEBUG
void Ci3DbgViewerView::AssertValid() const
{
	CView::AssertValid();
}

void Ci3DbgViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Ci3DbgViewerDoc* Ci3DbgViewerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ci3DbgViewerDoc)));
	return (Ci3DbgViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// Ci3DbgViewerView 메시지 처리기

BOOL Ci3DbgViewerView::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}

int Ci3DbgViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pViewer = i3DebugViewer::NewObject();
	m_pViewer->Create( m_hWnd, 100, 100, 0, 0, 0, 0, 1, 0);

	m_pViewer->Pause();
	//sm_pViewer->SetPendingUpdateEnable( true);

	m_pFramework = i3ViewerFramework::NewObject();
	m_pFramework->Create( m_pViewer);
	m_pFramework->SetPanelVisible( false);

	return 0;
}

void Ci3DbgViewerView::OnDestroy()
{
	I3_SAFE_RELEASE( m_pFramework);
	I3_SAFE_RELEASE( m_pViewer);

	CView::OnDestroy();
}

void Ci3DbgViewerView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void Ci3DbgViewerView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	Ci3DbgViewerDoc * pDoc = GetDocument();

	m_pViewer->setCmdBuffer( (char *) pDoc->m_Buff.GetHandle());
	m_pFramework->SetBound( &pDoc->m_BoundMin, &pDoc->m_BoundMax);
}
