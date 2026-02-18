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

// i3VTexView.cpp : implementation of the Ci3VTexView class
//

#include "stdafx.h"
#include "i3VTex.h"

#include "i3VTexDoc.h"
#include "i3VTexView.h"
#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3VTexView

IMPLEMENT_DYNCREATE(Ci3VTexView, CView)

BEGIN_MESSAGE_MAP(Ci3VTexView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Ci3VTexView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

// Ci3VTexView construction/destruction

Ci3VTexView::Ci3VTexView()
{
	m_X = 0;
	m_Y = 0;
	m_W = m_H = 0;

	m_pFramework= NULL;
	m_pUIRenderer = NULL;

	m_hSprite = -1;
}

Ci3VTexView::~Ci3VTexView()
{
	if( (m_hSprite != -1) && (m_pUIRenderer != NULL))
	{
		m_pUIRenderer->FreeSprite( m_hSprite);
		m_hSprite = -1;
	}

	I3_SAFE_RELEASE( m_pFramework);
}

BOOL Ci3VTexView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |= WS_VSCROLL | WS_HSCROLL;

	return CView::PreCreateWindow(cs);
}

// Ci3VTexView drawing

void Ci3VTexView::OnDraw(CDC* /*pDC*/)
{
	Ci3VTexDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

}


// Ci3VTexView printing


void Ci3VTexView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL Ci3VTexView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Ci3VTexView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Ci3VTexView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void Ci3VTexView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Ci3VTexView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// Ci3VTexView diagnostics

#ifdef _DEBUG
void Ci3VTexView::AssertValid() const
{
	CView::AssertValid();
}

void Ci3VTexView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Ci3VTexDoc* Ci3VTexView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ci3VTexDoc)));
	return (Ci3VTexDoc*)m_pDocument;
}
#endif //_DEBUG


// Ci3VTexView message handlers

int Ci3VTexView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rt;

		GetClientRect( &rt);

		m_ViewerCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 101);

		m_pFramework = i3VTexFramework::NewObject();
		m_pFramework->Create( m_ViewerCtrl.getViewer());

		m_pUIRenderer = m_pFramework->getUIRenderer();
		g_pFramework = m_pFramework;
	}

	return 0;
}

void Ci3VTexView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_W = cx;
	m_H = cy;

	if( ::IsWindow( m_ViewerCtrl.m_hWnd))
	{
		m_ViewerCtrl.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);

		_UpdateSprite();

		InitScroll();
	}
}

void Ci3VTexView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	Ci3VTexDoc * pDoc = GetDocument();

	if( m_pUIRenderer == NULL)
		return;

	m_pUIRenderer->SetTexture( pDoc->m_pTex);

	if( pDoc->m_pTex == NULL)
	{
		// VTex가 없다.
		if( m_hSprite != -1)
		{
			m_pUIRenderer->FreeSprite( m_hSprite);
			m_hSprite = -1;
		}
	}
	else
	{
		if( m_hSprite == -1)
		{
			m_hSprite = m_pUIRenderer->AllocSprite();

			_UpdateSprite();
		}
	}

	InitScroll();
}

void Ci3VTexView::_UpdateSprite(void)
{
	Ci3VTexDoc * pDoc = GetDocument();

	// UV 좌표 갱신
	if( (pDoc->m_pTex != NULL) && (m_pUIRenderer != NULL))
	{
		REAL32 u1, v1, u2, v2, r;

		r = 1.0f / pDoc->m_pTex->getWidth();
		u1 = m_X * r;
		v1 = m_Y * r;
		u2 = (m_X + m_W) * r;
		v2 = (m_Y + m_H) * r;

		m_pUIRenderer->setPos( m_hSprite, 0.0f, 0.0f, 0.0f);
		m_pUIRenderer->setSizeAndRotate( m_hSprite, (REAL32) m_W, (REAL32) m_H, 0.0f);
		m_pUIRenderer->setTexCoord( m_hSprite, u1, v1, u2, v2);
		m_pUIRenderer->setTexRange( m_hSprite, u1, v1, u2, v2);
		m_pUIRenderer->setColor( m_hSprite, 255, 255, 255, 255);
	}
}

void Ci3VTexView::InitScroll(void)
{
	Ci3VTexDoc * pDoc = GetDocument();
	
	if( pDoc == NULL)
		return;

	i3VirtualTexture * pTex = pDoc->m_pTex;

	if( pTex == NULL)
		return;

	SCROLLINFO info;

	info.cbSize		= sizeof(info);
	info.fMask		= SIF_PAGE | SIF_RANGE;
	info.nMin		= 0;
	info.nMax		= pTex->getWidth();
	info.nPage		= m_W;

	::SetScrollInfo( m_hWnd, SB_HORZ, &info, TRUE);

	info.nMax		= pTex->getHeight();
	info.nPage		= m_H;

	::SetScrollInfo( m_hWnd, SB_VERT, &info, TRUE);
}

void Ci3VTexView::UpdateHScroll(void)
{
	SCROLLINFO info;

	info.cbSize		= sizeof(info);
	info.fMask		= SIF_POS | SIF_TRACKPOS;
	info.nPos		= m_X;
	info.nTrackPos	= m_X;

	::SetScrollInfo( m_hWnd, SB_HORZ, &info, TRUE);
}

void Ci3VTexView::UpdateVScroll(void)
{
	SCROLLINFO info;

	info.cbSize		= sizeof(info);
	info.fMask		= SIF_POS | SIF_TRACKPOS;
	info.nPos		= m_Y;
	info.nTrackPos	= m_Y;

	::SetScrollInfo( m_hWnd, SB_VERT, &info, TRUE);
}

void Ci3VTexView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Ci3VTexDoc * pDoc = GetDocument();
	
	if( pDoc == NULL)
		return;

	i3VirtualTexture * pTex = pDoc->m_pTex;

	if( pTex == NULL)
		return;

	switch( nSBCode)
	{
		case SB_LINEDOWN :		m_X += 1;		break;
		case SB_LINEUP :		m_X -= 1;		break;
		case SB_PAGEDOWN :		m_X += m_W;		break;
		case SB_PAGEUP :		m_X -= m_W;		break;
		case SB_THUMBPOSITION :	m_X = nPos;		break;
		case SB_THUMBTRACK :	m_X = nPos;		break;
	}

	if( m_X < 0)		m_X = 0;
	if( (m_X + m_W) > pTex->getWidth())
		m_X = pTex->getWidth() - m_W;

	UpdateHScroll();
	_UpdateSprite();
	m_ViewerCtrl.Invalidate();

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void Ci3VTexView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Ci3VTexDoc * pDoc = GetDocument();
	
	if( pDoc == NULL)
		return;

	i3VirtualTexture * pTex = pDoc->m_pTex;

	if( pTex == NULL)
		return;

	switch( nSBCode)
	{
		case SB_LINEDOWN :		m_Y += 1;		break;
		case SB_LINEUP :		m_Y -= 1;		break;
		case SB_PAGEDOWN :		m_Y += m_H;		break;
		case SB_PAGEUP :		m_Y -= m_H;		break;
		case SB_THUMBPOSITION :	m_Y = nPos;		break;
		case SB_THUMBTRACK :	m_Y = nPos;		break;
	}

	if( m_Y < 0)		m_Y = 0;
	if( (m_Y + m_H) > pTex->getHeight())
		m_Y = pTex->getHeight() - m_H;

	UpdateVScroll();
	_UpdateSprite();
	m_ViewerCtrl.Invalidate();

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}
