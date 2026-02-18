// i3PackFileEditorView.cpp : Ci3PackFileEditorView 클래스의 구현
//

#include "stdafx.h"
#include "i3PackFileEditor.h"

#include "i3PackFileEditorDoc.h"
#include "i3PackFileEditorView.h"
#include ".\i3packfileeditorview.h"
#include "GlobalVariable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3PackFileEditorView

IMPLEMENT_DYNCREATE(Ci3PackFileEditorView, CView)

BEGIN_MESSAGE_MAP(Ci3PackFileEditorView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// Ci3PackFileEditorView 생성/소멸

Ci3PackFileEditorView::Ci3PackFileEditorView()
{
	g_pView = this;
}

Ci3PackFileEditorView::~Ci3PackFileEditorView()
{
}

BOOL Ci3PackFileEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

// Ci3PackFileEditorView 그리기

void Ci3PackFileEditorView::OnDraw(CDC* /*pDC*/)
{
	Ci3PackFileEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

void Ci3PackFileEditorView::Log( const char * pszFile, const char * pszMsg)
{
	m_PaneLog.Log( pszFile, pszMsg);
}

void Ci3PackFileEditorView::ClearLog(void)
{
	m_PaneLog.ClearLog();
}

// Ci3PackFileEditorView 진단

#ifdef _DEBUG
void Ci3PackFileEditorView::AssertValid() const
{
	CView::AssertValid();
}

void Ci3PackFileEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Ci3PackFileEditorDoc* Ci3PackFileEditorView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ci3PackFileEditorDoc)));
	return (Ci3PackFileEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// Ci3PackFileEditorView 메시지 처리기

int Ci3PackFileEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;

	{
		GetClientRect( &rect);

		m_Splitter0.Create( 0, WS_CHILD | WS_VISIBLE, rect, 48, this, IDC_SPLITTER0);
		m_Splitter1.Create( I3_SPLS_EX_VERT, WS_CHILD | WS_VISIBLE, rect, 48, &m_Splitter0, IDC_SPLITTER1);
		m_Splitter2.Create( I3_SPLS_EX_VERT, WS_CHILD | WS_VISIBLE, rect, 48, &m_Splitter1, IDC_SPLITTER2);

		{
			m_PaneTree.Create( CPaneTree::IDD, &m_Splitter0);
			m_PaneTree.SetView( this);

			m_Splitter0.AddPane( &m_PaneTree);
			m_Splitter0.AddPane( &m_Splitter1);
		}

		{
			m_PaneCommon.Create( CPaneCommon::IDD, &m_Splitter1);

			m_Splitter1.AddPane( &m_PaneCommon);
			m_Splitter1.AddPane( &m_Splitter2);
		}

		{
			m_PaneProperty.Create( CPaneProperty::IDD, &m_Splitter2);
			m_PaneLog.Create( CPaneLog::IDD, &m_Splitter2);

			m_Splitter2.AddPane( &m_PaneProperty);
			m_Splitter2.AddPane( &m_PaneLog);
		}

		char szTemp[MAX_PATH];
		sprintf( szTemp, "%s.CFG", g_szExeDir );
		m_Splitter0.LoadWindowInfo( szTemp );
	}

	return 0;
}

void Ci3PackFileEditorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if( ::IsWindow( m_Splitter0.m_hWnd))
	{
		m_Splitter0.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Ci3PackFileEditorView::OnClose()
{
	CView::OnClose();
}

void Ci3PackFileEditorView::OnDestroy()
{
	if( ! g_MyCmdInfo.m_bCompileAndExit )
	{
		char szTemp[MAX_PATH];
		sprintf( szTemp, "%s.CFG", g_szExeDir );
//		m_Splitter0.SaveWindowInfo( szTemp );
	}

	CView::OnDestroy();
}

