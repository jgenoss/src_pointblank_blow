// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// i3Viewer2View.cpp : Ci3Viewer2View 클래스의 구현
//

#include "stdafx.h"
#include "i3Viewer2.h"
#include "i3Viewer2Doc.h"
#include "i3Viewer2View.h"

#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3Viewer2View

IMPLEMENT_DYNCREATE(Ci3Viewer2View, CView)

BEGIN_MESSAGE_MAP(Ci3Viewer2View, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Ci3Viewer2View::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// Ci3Viewer2View 생성/소멸

Ci3Viewer2View::Ci3Viewer2View()
{
	m_pFramework = NULL;
}

Ci3Viewer2View::~Ci3Viewer2View()
{
	I3_SAFE_RELEASE( m_pFramework);
}

BOOL Ci3Viewer2View::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

int Ci3Viewer2View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rect;

		rect.SetRect( 0, 0, 100, 100);

		m_ViewerCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 100);

		{
			m_pFramework = i3Viewer2Framework::new_object();
			m_pFramework->Create( m_ViewerCtrl.getViewer());
		}

		g_pViewer = m_ViewerCtrl.getViewer();
		g_pFramework = m_pFramework;

		g_pRenderPanel = &m_ViewerCtrl;
	}

	{
		i3TDK::RegisterUpdate( m_hWnd, i3Node::static_meta(), I3_TDK_UPDATE_ALL);
	}

	return 0;
}


// Ci3Viewer2View 그리기

void Ci3Viewer2View::OnDraw(CDC* /*pDC*/)
{
	Ci3Viewer2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// Ci3Viewer2View 인쇄


void Ci3Viewer2View::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Ci3Viewer2View::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// Ci3Viewer2View 진단

#ifdef _DEBUG
void Ci3Viewer2View::AssertValid() const
{
	CView::AssertValid();
}

void Ci3Viewer2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Ci3Viewer2Doc* Ci3Viewer2View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ci3Viewer2Doc)));
	return (Ci3Viewer2Doc*)m_pDocument;
}
#endif //_DEBUG


// Ci3Viewer2View 메시지 처리기


void Ci3Viewer2View::OnDestroy()
{
	m_pFramework->SetSceneGraph( NULL);
	I3_SAFE_RELEASE( m_pFramework);

	CView::OnDestroy();
}

void Ci3Viewer2View::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( m_pFramework != NULL)
		{
			m_pFramework->SelectNode( (i3Node *) pInfo->m_pObject);

			m_ViewerCtrl.Invalidate();
		}
	}
}

LRESULT Ci3Viewer2View::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CView::WindowProc(message, wParam, lParam);
}

void Ci3Viewer2View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if( ::IsWindow( m_ViewerCtrl.m_hWnd))
	{
		m_ViewerCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Ci3Viewer2View::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	m_ViewerCtrl.SetFocus();
}
