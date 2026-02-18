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

// i3LevelEditorView.cpp : implementation of the Ci3LevelDesignView class
//

#include "pch.h"
#include "i3LevelDesign.h"

#include "i3LevelDesignDoc.h"
#include "i3LevelDesignView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "i3LevelDesign.h"

//////////////////////////////////////////////////////////////////////////


// Ci3LevelDesignView

IMPLEMENT_DYNCREATE(Ci3LevelDesignView, CView)

BEGIN_MESSAGE_MAP(Ci3LevelDesignView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Ci3LevelDesignView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// Ci3LevelDesignView construction/destruction

Ci3LevelDesignView::Ci3LevelDesignView()
{
	// TODO: add construction code here

}

Ci3LevelDesignView::~Ci3LevelDesignView()
{
}

BOOL Ci3LevelDesignView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Ci3LevelDesignView drawing

void Ci3LevelDesignView::OnDraw(CDC* /*pDC*/)
{
	Ci3LevelDesignDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}


// Ci3LevelDesignView printing


void Ci3LevelDesignView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL Ci3LevelDesignView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Ci3LevelDesignView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Ci3LevelDesignView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void Ci3LevelDesignView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Ci3LevelDesignView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// Ci3LevelDesignView diagnostics

#ifdef _DEBUG
void Ci3LevelDesignView::AssertValid() const
{
	CView::AssertValid();
}

void Ci3LevelDesignView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Ci3LevelDesignDoc* Ci3LevelDesignView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ci3LevelDesignDoc)));
	return (Ci3LevelDesignDoc*)m_pDocument;
}
#endif //_DEBUG



void Ci3LevelDesignView::DropObject( i3LevelRes * pRes, POINT pt)
{
	i3LevelElement3D * pInstance;

	{
		// 새로운 Instance를 생성
		// 생성된 Instance는 이미 Scene에 추가되어 있다.
		pInstance = pRes->CreateInstance( g_pScene);

		//{
		//	i3LevelUndoInfoCreate * pInfo = i3LevelUndoInfoCreate::new_object();

		//	pInfo->SetCreatedObject( pInstance);
		//	pInfo->Execute();
		//}

		if( pInstance != NULL)
		{
			VEC3D pos;

			i3LevelFramework * pFramework;
			pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();
			
			pFramework->GetPickedPos( pt, &pos );

			pInstance->setPos( &pos);
			g_pScene->SelectElement( pInstance);
			
		}
	}
}

// Ci3LevelDesignView message handlers

int Ci3LevelDesignView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rt;

		GetClientRect( &rt);

		m_Viewport.Create( WS_CHILD | WS_VISIBLE, rt, this, IDC_VIEWERDUMMYWND, g_pScene);

		i3Level::RegisterUpdateNotify( m_hWnd, i3LevelScene::static_meta(), I3_TDK_UPDATE_ALL);

		ReplaceControl( -1, -1);
	}

	return 0;
}


void Ci3LevelDesignView::ReplaceControl( int cx, int cy)
{
	if( cx == -1)
	{
		CRect rt;

		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	if( ::IsWindow( m_Viewport.m_hWnd))
	{
		m_Viewport.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}
void Ci3LevelDesignView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	ReplaceControl( cx, cy);
}

void Ci3LevelDesignView::OnUpdateSys( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		m_Viewport.Invalidate();
	}
}
LRESULT Ci3LevelDesignView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case	WM_TDK_UPDATE:
			OnUpdateSys( (I3_TDK_UPDATE_INFO *) wParam);
			break;
	}

	return CView::WindowProc(message, wParam, lParam);
}
