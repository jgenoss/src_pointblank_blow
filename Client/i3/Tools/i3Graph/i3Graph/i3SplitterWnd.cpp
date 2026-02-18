// i3SplitterWnd.cpp : implementation file
//

#include "stdafx.h"
#include "i3SplitterWnd.h"
#include ".\i3splitterwnd.h"

// i3SplitterWnd

IMPLEMENT_DYNAMIC(i3SplitterWnd, CWnd)

static BOOL s_bRegMeta = FALSE;

i3SplitterWnd::i3SplitterWnd()
{
	INT32 i;

	if( s_bRegMeta == FALSE)
	{
		s_bRegMeta = TRUE;

		i3WindowInfo::RegisterMeta();
	}

	m_MinSize = 10;
	m_Size = 0;
	m_ExStyle = 0;

	for( i = 0; i < 2; i++)
	{
		m_pPane[i] = NULL;

		m_OldRect[i].left = 0;
		m_OldRect[i].top = 0;
		m_OldRect[i].right = 0;
		m_OldRect[i].bottom = 0;
	}
	
	m_bDragging = FALSE;
}

i3SplitterWnd::~i3SplitterWnd()
{

}

BOOL i3SplitterWnd::Create( UINT32 dwExStyle, UINT32 dwStyle, const RECT & rect, 
							UINT32 MinSz, 
							CWnd * pParentWnd, UINT32 nID, CCreateContext * pContext)
{
	LPCTSTR lpszClass;
	HBRUSH hBack;
	HICON hIcon;
	BOOL Rv;

	m_ExStyle = dwExStyle;
	m_MinSize = MinSz;

	m_hCursorArrow = ::LoadCursor( NULL, IDC_ARROW);
	ASSERT( m_hCursorArrow != NULL);

	hBack = ::GetSysColorBrush( COLOR_BTNFACE);
	ASSERT( hBack != NULL);

	hIcon = NULL;

	lpszClass = AfxRegisterWndClass( CS_VREDRAW | CS_HREDRAW, m_hCursorArrow, hBack, hIcon);
	ASSERT( lpszClass != NULL);

	dwStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	Rv = CWnd::Create( lpszClass, NULL, dwStyle, rect, pParentWnd, nID, pContext);
	ASSERT( Rv != NULL);

	if( m_ExStyle & I3_SPLS_EX_VERT)
	{
		m_hCursorSize = ::LoadCursor( NULL, IDC_SIZENS);
	}
	else
	{
		m_hCursorSize = ::LoadCursor( NULL, IDC_SIZEWE);
	}

	return Rv;
}

INT32 i3SplitterWnd::GetPaneCount(void)
{
	INT32 i, count;

	for( i = 0, count = 0; i < 2; i++)
	{
		if( m_pPane[i] != NULL)
			count++;
	}

	return count;
}

void i3SplitterWnd::SetPane( INT32 idx, CWnd * pPane)
{
	m_pPane[idx] = pPane;

	ReplacePanes( -1, -1);
}

BOOL i3SplitterWnd::AddPane( CWnd * pPane)
{
	ASSERT((m_pPane[0] == NULL) || (m_pPane[1] == NULL));

	if( m_pPane[0] == NULL)
	{
		m_pPane[0] = pPane;
	}
	else if( m_pPane[1] == NULL)
	{
		m_pPane[1] = pPane;

		if( m_Size == 0)
		{
			CRect rt;

			GetClientRect( &rt);

			if( m_ExStyle & I3_SPLS_EX_VERT)
				m_Size = rt.Height() >> 1;
			else
				m_Size = rt.Width() >> 1;
		}
	}

	ReplacePanes( -1, -1);

	return TRUE;
}

void i3SplitterWnd::RemovePane( CWnd * pPane)
{
	ASSERT( (m_pPane[0] == pPane) || (m_pPane[1] == pPane));

	if( m_pPane[0] == pPane)
	{
		m_pPane[0] = m_pPane[1];
		m_pPane[1] = NULL;
	}
	else
	{
		m_pPane[1] = NULL;
	}
}

void i3SplitterWnd::RemovePane( INT32 idx)
{
	ASSERT( (idx == 0) || (idx == 1));

	if( idx == 0)
	{
		m_pPane[0] = m_pPane[1];
		m_pPane[1] = NULL;
	}
	else
	{
		m_pPane[1] = NULL;
	}
}

void i3SplitterWnd::CalcLayout( int cx, int cy, CRect * pRect1, CRect * pRect2)
{
	if((cx == -1) || (cy == -1))
	{
		CRect rt;

		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	pRect1->left = 0;
	pRect1->top = 0;
	pRect1->right = cx;
	pRect1->bottom = cy;

	*pRect2 = *pRect1;

	if( m_pPane[1] != NULL)
	{
		// Pane이 2개 있다.
		if( m_ExStyle & I3_SPLS_EX_VERT)
		{
			// 수직 분할
			pRect1->bottom = m_Size - 2;

			pRect2->top = m_Size + 2;
		}
		else
		{
			// 수평 분할
			pRect1->right = m_Size - 2;

			pRect2->left = m_Size + 2;
		}
	}
	else if( m_pPane[0] == NULL)
	{
		// Pane이 전혀 없다.
		pRect1->right = 0;
		pRect1->bottom = 0;

		pRect2->right = 0;
		pRect2->bottom = 0;
	}
}

void i3SplitterWnd::CalcSplitter( int cx, int cy, int newSz, CRect * pRect)
{
	if((cx == -1) || (cy == -1))
	{
		CRect rt;

		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	if( m_ExStyle & I3_SPLS_EX_VERT)
	{
		// 수직 분할

		pRect->left = 0;
		pRect->top = newSz - 2;
		pRect->right = cx;
		pRect->bottom = newSz + 2;
	}
	else
	{
		// 수평 분할
		pRect->left = newSz - 2;
		pRect->top = 0;
		pRect->right = newSz + 2;
		pRect->bottom = cy;
	}
}

void i3SplitterWnd::ReplacePanes( int cx, int cy)
{
	CRect Rc[2];
	INT32 i, w, h;

	CalcLayout( cx, cy, &Rc[0], &Rc[1]);

	for( i = 0; i < 2; i++)
	{
		if( m_pPane[i] != NULL)
		{
			if( Rc[i] != m_OldRect[i])
			{
				w = Rc[i].Width();
				h = Rc[i].Height();

				//m_pPane[i]->MoveWindow( Rc[i].left, Rc[i].top, w, h, TRUE);
				//m_pPane[i]->PostMessage( WM_SIZE, 0, MAKELONG( w, h));
				m_pPane[i]->SetWindowPos( NULL, Rc[i].left, Rc[i].top, Rc[i].Width(), Rc[i].Height(), SWP_NOZORDER);

				m_OldRect[i] = Rc[i];
			}
		}
	}
}

void i3SplitterWnd::DragStart( CPoint pt)
{
	CRect rt;

	SetCapture();

	m_ScreenDC = ::GetDC( NULL);

	GetClientRect( &rt);
	m_Width = rt.Width();
	m_Height = rt.Height();

	CalcSplitter( m_Width, m_Height, m_Size, &rt);

	ClientToScreen( &rt);

	m_DragRect = rt;

	::DrawFocusRect( m_ScreenDC, &rt);
	rt.InflateRect( 1, 1, -1, -1);
	::DrawFocusRect( m_ScreenDC, &rt);
}

void i3SplitterWnd::Drag( CPoint pt)
{
	CRect rt;

	// Erase Old Pos.
	{
		rt = m_DragRect;

		::DrawFocusRect( m_ScreenDC, &rt);
		rt.InflateRect( 1, 1, -1, -1);
		::DrawFocusRect( m_ScreenDC, &rt);
	}

	if( m_ExStyle & I3_SPLS_EX_VERT)
	{
		m_Size = pt.y;

		if( m_Size < m_MinSize)
			m_Size = m_MinSize;

		if( m_Size > (m_Height - m_MinSize))
			m_Size = m_Height - m_MinSize;
	}
	else
	{
		m_Size = pt.x;

		if( m_Size < m_MinSize)
			m_Size = m_MinSize;

		if( m_Size > (m_Width - m_MinSize))
			m_Size = m_Width - m_MinSize;
	}

	CalcSplitter( m_Width, m_Height, m_Size, &rt);

	ClientToScreen( &rt);

	m_DragRect = rt;

	::DrawFocusRect( m_ScreenDC, &rt);
	rt.InflateRect( 1, 1, -1, -1);
	::DrawFocusRect( m_ScreenDC, &rt);
}

void i3SplitterWnd::DragEnd( CPoint pt)
{
	CRect rt;

	CalcSplitter( m_Width, m_Height, m_Size, &rt);

	rt = m_DragRect;

	::DrawFocusRect( m_ScreenDC, &rt);
	rt.InflateRect( 1, 1, -1, -1);
	::DrawFocusRect( m_ScreenDC, &rt);

	::ReleaseCapture();
	::ReleaseDC( NULL, m_ScreenDC);

	ReplacePanes( m_Width, m_Height);
}

void i3SplitterWnd::Rearrange(void)
{
	ReplacePanes( -1, -1);
}

BEGIN_MESSAGE_MAP(i3SplitterWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

// i3SplitterWnd message handlers

BOOL i3SplitterWnd::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}

void i3SplitterWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void i3SplitterWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	ReplacePanes( cx, cy);
}

void i3SplitterWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging == TRUE)
	{
		Drag( point);

		SetCursor( m_hCursorSize);
	}
	else
	{
		CRect rt;

		CalcSplitter( -1, -1, m_Size, &rt);

		if( PtInRect( &rt, point))
		{
			SetCursor( m_hCursorSize);
		}
		else
		{
			SetCursor( m_hCursorArrow);
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void i3SplitterWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_bDragging == FALSE)
	{
		DragStart( point);

		m_bDragging = TRUE;

	}

	CWnd::OnLButtonDown(nFlags, point);
}

void i3SplitterWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging == TRUE)
	{
		DragEnd( point);

		m_bDragging = FALSE;
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void i3SplitterWnd::SetWindowInfo( CWnd * pWnd, i3WindowInfo * pInfo)
{
	CRect rect;

	pWnd->GetWindowRect( &rect);

	if( pInfo->GetParent() != NULL)
	{
		pWnd->GetParent()->ScreenToClient( &rect);
	}

	pInfo->SetX( rect.left);
	pInfo->SetY( rect.top);
	pInfo->SetWidth( rect.Width());
	pInfo->SetHeight( rect.Height());
	pInfo->SetSize( m_Size);
}

BOOL i3SplitterWnd::OnSaveWindowInfo( i3WindowInfo * pInfo)
{
	i3WindowInfo * pChild;
	INT32 i;

	SetWindowInfo( this, pInfo);

	for( i = 0; i < 2; i++)
	{
        if( m_pPane[i] != NULL)
		{
			pChild = i3WindowInfo::NewObject();

			pInfo->SetPane( i, pChild);
			pChild->SetParent( pInfo);

			if( m_pPane[i]->IsKindOf( RUNTIME_CLASS( i3SplitterWnd)))
			{
				i3SplitterWnd * pSplitter = (i3SplitterWnd *) m_pPane[i];

				// Recursive Call
				pSplitter->OnSaveWindowInfo( pChild);
			}
			else
			{
				SetWindowInfo( m_pPane[i], pChild);
			}
		}
	}

	return TRUE;
}

BOOL i3SplitterWnd::OnLoadWindowInfo( i3WindowInfo * pInfo)
{
	INT32 i;

	m_Size = pInfo->GetSize();

	/*
	SetWindowPos( NULL, pInfo->GetX(), pInfo->GetY(), pInfo->GetWidth(), pInfo->GetHeight(), 
		SWP_NOACTIVATE | SWP_NOZORDER);
		*/

	for( i = 0; i < 2; i++)
	{
		if( m_pPane[i] == NULL)
			continue;

		if( m_pPane[i]->IsKindOf( RUNTIME_CLASS( i3SplitterWnd)))
		{
			i3SplitterWnd * pSplitter = (i3SplitterWnd *) m_pPane[i];

			pSplitter->OnLoadWindowInfo( pInfo->GetPane( i));
		}
	}

	return TRUE;
}

BOOL i3SplitterWnd::SaveWindowInfo( const char * pszPath)
{
	i3WindowInfo * pRoot;
	UINT32 Rc;

	pRoot = i3WindowInfo::NewObject();
	I3ASSERT( pRoot);

	OnSaveWindowInfo( pRoot);

	{
		i3ResourceFile file;
		i3List ObjList;

		ObjList.Add( pRoot);

		file.SetObjects( &ObjList);

		Rc = file.Save( pszPath);

		pRoot->Release();

		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SplitterWnd::SaveWindowInfo()", "Could not create splitter window info.");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL i3SplitterWnd::LoadWindowInfo( const char * pszPath)
{
	i3ResourceFile file;
	UINT32 Rc;
	INT32 i;
	i3WindowInfo * pRoot = NULL;
	i3PersistantElement * pElm;

	Rc = file.Load( pszPath);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3SplittreWnd::LoadWindowInfo()", "Could not load %s splitter window info.", pszPath);
		return FALSE;
	}

	for( i = 0, pRoot = NULL; i < file.GetObjectCount(); i++)
	{
		pElm = file.GetObject( i);

		if( pElm->IsExactTypeOf( i3WindowInfo::GetClassMeta()))
		{
			i3WindowInfo * pTemp = (i3WindowInfo *) pElm;

			if( pTemp->GetParent() == NULL)
			{
				pRoot = pTemp;
				break;
			}
		}
	}

	I3ASSERT( pRoot != NULL);

	if( GetParent() != NULL)
	{
		CRect rect;
		BOOL bMenu;
		DWORD dwStyle, dwExStyle;
		HWND hwnd;

		rect.left = pRoot->GetX();
		rect.top = pRoot->GetY();
		rect.right = pRoot->GetX() + pRoot->GetWidth();
		rect.bottom = pRoot->GetY() + pRoot->GetHeight();

		hwnd = GetParent()->m_hWnd;

		dwStyle = ::GetWindowLong( hwnd, GWL_STYLE);
		dwExStyle = ::GetWindowLong( hwnd, GWL_EXSTYLE);
		if( ::GetMenu( hwnd) != NULL)
			bMenu = TRUE;
		else
			bMenu = FALSE;

		AdjustWindowRectEx( &rect, dwStyle, bMenu, dwExStyle);

		GetParent()->SetWindowPos( NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}


	OnLoadWindowInfo( pRoot);

	pRoot->AddRef();
	pRoot->Release();


	return TRUE;
}

void i3SplitterWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanged(lpwndpos);

	ReplacePanes( lpwndpos->cx, lpwndpos->cy);
}
