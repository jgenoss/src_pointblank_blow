// DummyWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ViewerWnd.h"
#include ".\viewerwnd.h"
// CViewerWnd

IMPLEMENT_DYNAMIC(CViewerWnd, CWnd)
CViewerWnd::CViewerWnd()
{
	m_pViewer = NULL;
}

CViewerWnd::~CViewerWnd()
{
}


BEGIN_MESSAGE_MAP(CViewerWnd, CWnd)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
END_MESSAGE_MAP()

BOOL CViewerWnd::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR lpszClass;
	HCURSOR hCursor;
	HBRUSH hbrBack;

	hCursor = ::LoadCursor( NULL, IDC_ARROW);

	hbrBack = (HBRUSH) GetSysColorBrush( COLOR_3DFACE);

	lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS, hCursor, hbrBack, NULL);
	ASSERT( lpszClass);

	return CWnd::Create( lpszClass, NULL, dwStyle, rect, pParentWnd, nID, NULL);
}

void CViewerWnd::SetViewer( i3Viewer * pViewer)
{
	CRect rc;

	m_pViewer = pViewer;

	GetClientRect( &rc);

	m_pViewer->Resize( rc.Width(), rc.Height());
}

// CViewerWnd message handlers


void CViewerWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void CViewerWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//CWnd::OnChar(nChar, nRepCnt, nFlags);
}
