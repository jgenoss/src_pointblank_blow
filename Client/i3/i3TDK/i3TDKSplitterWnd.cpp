// i3TDKSplitterWnd.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKSplitterWnd.h"
#include "i3Base/string/ext/extract_directoryname_filename.h"
#include "i3Base/itl/range/algorithm/find.h"

// i3TDKSplitterWnd

IMPLEMENT_DYNAMIC(i3TDKSplitterWnd, CWnd)

i3TDKSplitterWnd::~i3TDKSplitterWnd()
{
	if( m_hCursorArrow != nullptr)
	{
		::DeleteObject( m_hCursorArrow);
		::DeleteObject( m_hCursorSize);

		m_hCursorArrow = nullptr;
		m_hCursorSize = nullptr;
	}
}

BOOL i3TDKSplitterWnd::Create( UINT32 dwExStyle, UINT32 dwStyle, const RECT & rect, 
							UINT32 MinSz, 
							CWnd * pParentWnd, UINT32 nID, CCreateContext * pContext)
{
	LPCTSTR lpszClass;
	HBRUSH hBack;
	HICON hIcon;
	BOOL Rv;

	m_ExStyle = dwExStyle;
	m_MinSize = MinSz;

	m_hCursorArrow = ::LoadCursor( nullptr, IDC_ARROW);
	ASSERT( m_hCursorArrow != nullptr);

	hBack = ::GetSysColorBrush( COLOR_BTNFACE);
	ASSERT( hBack != nullptr);

	hIcon = nullptr;

	lpszClass = AfxRegisterWndClass( CS_VREDRAW | CS_HREDRAW, m_hCursorArrow, hBack, hIcon);
	ASSERT( lpszClass != nullptr);

	dwStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	Rv = CWnd::Create( lpszClass, _T("SplitterWnd"), dwStyle, rect, pParentWnd, nID, pContext);
	ASSERT( Rv != 0);

	if( m_ExStyle & I3_SPLS_EX_VERT)
	{
		m_hCursorSize = ::LoadCursor( nullptr, IDC_SIZENS);
	}
	else
	{
		m_hCursorSize = ::LoadCursor( nullptr, IDC_SIZEWE);
	}

	return Rv;
}

INT32 i3TDKSplitterWnd::GetPaneCount(void)
{
	INT32 i, count;

	for( i = 0, count = 0; i < 2; i++)
	{
		if( m_pPane[i] != nullptr)
			count++;
	}

	return count;
}

void i3TDKSplitterWnd::SetPane( INT32 idx, CWnd * pPane)
{
	m_pPane[idx] = pPane;

	ReplacePanes( -1, -1);
}

BOOL i3TDKSplitterWnd::AddPane( CWnd * pPane)
{
	ASSERT((m_pPane[0] == nullptr) || (m_pPane[1] == nullptr));

	if( m_pPane[0] == nullptr)
	{
		m_pPane[0] = pPane;
	}
	else if( m_pPane[1] == nullptr)
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

void i3TDKSplitterWnd::RemovePane( CWnd * pPane)
{
	ASSERT( (m_pPane[0] == pPane) || (m_pPane[1] == pPane));

	if( m_pPane[0] == pPane)
	{
		m_pPane[0] = m_pPane[1];
		m_pPane[1] = nullptr;
	}
	else
	{
		m_pPane[1] = nullptr;
	}
}

void i3TDKSplitterWnd::RemovePane( INT32 idx)
{
	ASSERT( (idx == 0) || (idx == 1));

	if( idx == 0)
	{
		m_pPane[0] = m_pPane[1];
		m_pPane[1] = nullptr;
	}
	else
	{
		m_pPane[1] = nullptr;
	}
}

void i3TDKSplitterWnd::CalcLayout( int cx, int cy, CRect * pRect1, CRect * pRect2)
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

	if( m_pPane[1] != nullptr)
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
	else if( m_pPane[0] == nullptr)
	{
		// Pane이 전혀 없다.
		pRect1->right = 0;
		pRect1->bottom = 0;

		pRect2->right = 0;
		pRect2->bottom = 0;
	}
}

void i3TDKSplitterWnd::CalcSplitter( int cx, int cy, int newSz, CRect * pRect)
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

void i3TDKSplitterWnd::ReplacePanes( int cx, int cy)
{
	CRect Rc[2];
	INT32 i, w, h;

	CalcLayout( cx, cy, &Rc[0], &Rc[1]);

	for( i = 0; i < 2; i++)
	{
		if( m_pPane[i] != nullptr)
		{
			if( Rc[i] != m_OldRect[i])
			{
				w = Rc[i].Width();
				h = Rc[i].Height();

				//m_pPane[i]->MoveWindow( Rc[i].left, Rc[i].top, w, h, TRUE);
				//m_pPane[i]->PostMessage( WM_SIZE, 0, MAKELONG( w, h));
				m_pPane[i]->SetWindowPos( nullptr, Rc[i].left, Rc[i].top, Rc[i].Width(), Rc[i].Height(), SWP_NOZORDER);

				m_OldRect[i] = Rc[i];
			}
		}
	}
}

void i3TDKSplitterWnd::DragStart( CPoint pt)
{
	CRect rt;

	SetCapture();

	m_ScreenDC = ::GetDC( nullptr);

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

void i3TDKSplitterWnd::Drag( CPoint pt)
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

void i3TDKSplitterWnd::DragEnd( CPoint pt)
{
	CRect rt;

	CalcSplitter( m_Width, m_Height, m_Size, &rt);

	rt = m_DragRect;

	::DrawFocusRect( m_ScreenDC, &rt);
	rt.InflateRect( 1, 1, -1, -1);
	::DrawFocusRect( m_ScreenDC, &rt);

	::ReleaseCapture();
	::ReleaseDC( nullptr, m_ScreenDC);

	ReplacePanes( m_Width, m_Height);
}

void i3TDKSplitterWnd::Rearrange(void)
{
	ReplacePanes( -1, -1);
}

BEGIN_MESSAGE_MAP(i3TDKSplitterWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

// i3TDKSplitterWnd message handlers

BOOL i3TDKSplitterWnd::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}

void i3TDKSplitterWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void i3TDKSplitterWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	ReplacePanes( cx, cy);
}

void i3TDKSplitterWnd::OnMouseMove(UINT nFlags, CPoint point)
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

void i3TDKSplitterWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_bDragging == FALSE)
	{
		DragStart( point);

		m_bDragging = TRUE;

	}

	CWnd::OnLButtonDown(nFlags, point);
}

void i3TDKSplitterWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging == TRUE)
	{
		DragEnd( point);

		m_bDragging = FALSE;
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void i3TDKSplitterWnd::SetWindowInfo( CWnd * pWnd, i3TDKWindowInfo * pInfo)
{
	CRect rect;

	pWnd->GetWindowRect( &rect);

	if( pInfo->GetParent() != nullptr)
	{
		pWnd->GetParent()->ScreenToClient( &rect);
	}

	pInfo->SetX( rect.left);
	pInfo->SetY( rect.top);
	pInfo->SetWidth( rect.Width());
	pInfo->SetHeight( rect.Height());
	pInfo->SetSize( m_Size);
}

BOOL i3TDKSplitterWnd::OnSaveWindowInfo( i3TDKWindowInfo * pInfo)
{
	i3TDKWindowInfo * pChild;
	INT32 i;

	SetWindowInfo( this, pInfo);

	for( i = 0; i < 2; i++)
	{
        if( m_pPane[i] != nullptr)
		{
			pChild = i3TDKWindowInfo::new_object();

			pInfo->SetPane( i, pChild);
			pChild->SetParent( pInfo);

			if( m_pPane[i]->IsKindOf( RUNTIME_CLASS( i3TDKSplitterWnd)))
			{
				i3TDKSplitterWnd * pSplitter = (i3TDKSplitterWnd *) m_pPane[i];

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

BOOL i3TDKSplitterWnd::OnLoadWindowInfo( i3TDKWindowInfo * pInfo)
{
	INT32 i;

	m_Size = pInfo->GetSize();

	/*
	SetWindowPos( nullptr, pInfo->GetX(), pInfo->GetY(), pInfo->GetWidth(), pInfo->GetHeight(), 
		SWP_NOACTIVATE | SWP_NOZORDER);
		*/

	for( i = 0; i < 2; i++)
	{
		if( m_pPane[i] == nullptr)
			continue;

		if( m_pPane[i]->IsKindOf( RUNTIME_CLASS( i3TDKSplitterWnd)))
		{
			i3TDKSplitterWnd * pSplitter = (i3TDKSplitterWnd *) m_pPane[i];

			pSplitter->OnLoadWindowInfo( pInfo->GetPane( i));
		}
	}

	return TRUE;
}

BOOL i3TDKSplitterWnd::SaveWindowInfo( const char * pszPath)
{
	i3TDKWindowInfo * pRoot;
	UINT32 Rc;

	pRoot = i3TDKWindowInfo::new_object();
	I3ASSERT( pRoot);

	OnSaveWindowInfo( pRoot);

	{
		i3ResourceFile file;
		char szPath[MAX_PATH], szName[MAX_PATH]; //, * pszName;


//		List ObjList;
//		ObjList.Add( pRoot);
		i3::vector<i3PersistantElement*> ObjList(1, pRoot);
		file.SetObjects( ObjList);

		i3::safe_string_copy( szPath, GetCommandLineA(), MAX_PATH);

		if( szPath[0] == '"')
		{
			i3::safe_string_copy( szPath, szPath + 1, MAX_PATH);
			char* pRight = i3::range::find( szPath, '"' );
			*pRight = 0;
		}

	//	pszName = i3String::SplitFileName( szPath, szName, FALSE);
	//	*pszName = 0;
		i3::extract_directoryname_filename(szPath, szPath, szName);

		i3::snprintf( szName, MAX_PATH, "%s%s", szPath, pszPath);

		Rc = file.Save( szName);

		pRoot->Release();

		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKSplitterWnd::SaveWindowInfo()", "Could not create splitter window info.");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL i3TDKSplitterWnd::LoadWindowInfo( const char * pszPath)
{
	i3ResourceFile file;
	UINT32 Rc;
	INT32 i;
	i3TDKWindowInfo * pRoot = nullptr;
	i3PersistantElement * pElm;
	char szPath[MAX_PATH], szName[MAX_PATH]; //, * pszName;

	i3::safe_string_copy( szPath, GetCommandLineA(), MAX_PATH);
	if( szPath[0] == '"')
	{
		i3::safe_string_copy( szPath, szPath + 1, MAX_PATH);
		char *pRight = strchr( szPath, '"' );
		*pRight = 0;
	}

//	pszName = i3String::SplitFileName( szPath, szName, nullptr);
//	*pszName = 0;
	i3::extract_directoryname_filename(szPath, szPath, szName);
	//

	sprintf( szName, "%s%s", szPath, pszPath);

	Rc = file.Load( szName);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3SplittreWnd::LoadWindowInfo()", "Could not load %s splitter window info.", pszPath);
		return FALSE;
	}

	for( i = 0, pRoot = nullptr; i < file.GetObjectCount(); i++)
	{
		pElm = file.GetObject( i);

		if( i3::same_of<i3TDKWindowInfo*>(pElm))
		{
			i3TDKWindowInfo * pTemp = (i3TDKWindowInfo *) pElm;

			if( pTemp->GetParent() == nullptr)
			{
				pRoot = pTemp;
				break;
			}
		}
	}

	I3ASSERT_RETURN( pRoot != nullptr, FALSE);

	if( (pRoot->GetWidth() > 0) && (pRoot->GetHeight() > 0))
	{
		if( GetParent() != nullptr)
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
			if( ::GetMenu( hwnd) != nullptr)
				bMenu = TRUE;
			else
				bMenu = FALSE;

			AdjustWindowRectEx( &rect, dwStyle, bMenu, dwExStyle);

			GetParent()->SetWindowPos( nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
		}


		OnLoadWindowInfo( pRoot);
	}

	I3_MUST_ADDREF(pRoot);
	I3_MUST_RELEASE(pRoot);


	return TRUE;
}

void i3TDKSplitterWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanged(lpwndpos);

	ReplacePanes( lpwndpos->cx, lpwndpos->cy);
}
