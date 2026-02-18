// i3TDKDockSplitWnd.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKDockSplitWnd.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKDockWnd.h"
#include "i3TDKDock.h"

// i3TDKDockSplitWnd

IMPLEMENT_DYNAMIC(i3TDKDockSplitWnd, CWnd)

#define LEFT			0
#define	RIGHT			1
#define	UP				0
#define DOWN			1

i3TDKDockSplitWnd::~i3TDKDockSplitWnd()
{
	RemoveAllPane();
}

BOOL i3TDKDockSplitWnd::Create( UINT32 dwExStyle, UINT32 dwStyle, const RECT & rect, 
							UINT32 MinSz, 
							CWnd * pParentWnd, UINT32 nID, CCreateContext * pContext)
{
	LPCTSTR lpszClass;
	HBRUSH hBack;
	HICON hIcon;
	BOOL Rv;

	m_ExStyle = dwExStyle;
	m_MinSize = MinSz;

	hBack = g_pTDKGlobalRes->getBrush( I3TDK_COLOR_FORE_APP_BKGND);
	ASSERT( hBack != nullptr);

	hIcon = nullptr;

	lpszClass = AfxRegisterWndClass( CS_VREDRAW | CS_HREDRAW, g_pTDKGlobalRes->m_hCursorArrow, hBack, hIcon);
	ASSERT( lpszClass != nullptr);

	dwStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	Rv = CWnd::Create( lpszClass, nullptr, dwStyle, rect, pParentWnd, nID, pContext);
	ASSERT( Rv != 0);

	{
		CRect rc;

		GetClientRect( &rc);

		m_Width = rc.Width();
		m_Height = rc.Height();
	}

	return Rv;
}

void i3TDKDockSplitWnd::SetChildAttr( CWnd * pWnd)
{
	UINT32 style;

	// Style
	style = GetWindowLong( pWnd->m_hWnd, GWL_STYLE);

	style |= WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	SetWindowLong( pWnd->m_hWnd, GWL_STYLE, style);

	pWnd->SetParent( this);
	pWnd->SetOwner( this);
}

INT32 i3TDKDockSplitWnd::GetPaneCount(void)
{
	INT32 i, count;

	for( i = 0, count = 0; i < 2; i++)
	{
		if( m_pPane[i] != nullptr)
			count++;
	}

	return count;
}

void i3TDKDockSplitWnd::SetPane( INT32 idx, CWnd * pPane, INT32 w, INT32 h)
{
	INT32 self_sz, limit;

	m_pPane[idx] = pPane;

	SetChildAttr( pPane);

	// 양쪽이 모두 있기 때문에 크기를 조절해 줄 필요가 있다.
	if( m_ExStyle & I3TDK_DOCK_STYLE_VERT)
	{
		// 세로 방향
		self_sz = h;
		limit = m_Height;
	}
	else
	{
		// 가로 방향
		self_sz = w;
		limit = m_Width;
	}

		// 크기 반영
	if((m_pPane[0] != nullptr) && (m_pPane[1] != nullptr))
	{
		self_sz += 2;		// Split Pos의 여백 공간만큼 증가.

		if( m_SplitPos != -1)
		{
			if( idx == 0)
				m_SplitPos = MAX( self_sz, m_SplitPos);
			else
				m_SplitPos = MIN( limit - self_sz, m_SplitPos);
		}
		else
		{
			if( idx == 0)
				m_SplitPos = self_sz;
			else
				m_SplitPos = limit - self_sz;
		}
	}
	else
	{
		// 어느 한쪽만 들어온 경우...
		if( idx == 0)
			m_SplitPos = self_sz;
		else
			m_SplitPos = limit - self_sz;
	}

	if( m_SplitPos < m_MinSize)
		m_SplitPos = m_MinSize;
	
	if( m_SplitPos > (limit - m_MinSize))
		m_SplitPos = limit - m_MinSize;

	ReplacePanes( -1, -1);
}

i3TDKDockWnd * i3TDKDockSplitWnd::_PrepareDockWnd( INT32 side, INT32 w, INT32 h)
{
	i3TDKDockWnd * pDockCtrl = nullptr;
	CRect rc;
	INT32 other = side ^ 1;

	// Split Position을 조정한다.
	getRect( side, w, h, &rc);

	// 만약 다른 Window가 이미 있다면...
	// 강제로 해당 방향에 밀어넣기 위한 조치를 한다.
	// 
	// 다른쪽이 비어있는 경우, 원래 있던 Pane은 다른쪽으로 이동시킨다.
	// 다른쪽이 비어있지 않다면, 해당 side의 것을 InsertChild 한다.
	if( m_pPane[side] != nullptr)
	{
		if( m_pPane[other] == nullptr)
		{
			// 빈, 반대쪽으로 기존의 Pane을 이동시킨다.
			m_pPane[other] = m_pPane[side];
		}
		else
		{
			// side편의 Child에 새로운 Split Window를 밀어넣고,
			// 기존의 Pane Child를 새로운 Split Window에 넣어준다.
			i3TDKDockSplitWnd * pDockWnd;

			pDockWnd = InsertChildDock( side, w, h, IsHorz());

			pDockCtrl = pDockWnd->_PrepareDockWnd( side, w, h);
			return pDockCtrl;
		}
	}

	{
		// 새로운 FolderCtrl을 만들어서 pPane을 Child로 추가..
		pDockCtrl = new i3TDKDockWnd;

		pDockCtrl->Create( WS_CHILD | WS_VISIBLE, w, h, rc, this, side);

		SetPane( side, pDockCtrl, w, h);
		pDockCtrl->AddState( I3TDK_DOCK_STATE_DOCKED);
	}

	return pDockCtrl;
}

void i3TDKDockSplitWnd::RemovePane( CWnd * pPane)
{
	ASSERT( (m_pPane[0] == pPane) || (m_pPane[1] == pPane));

	if( m_pPane[0] == pPane)
	{
		m_pPane[0] = m_pPane[1];
	}

	SetPane( 1, nullptr, 0, 0);
}

void i3TDKDockSplitWnd::RemovePane( INT32 idx)
{
	ASSERT( (idx == 0) || (idx == 1));

	if( idx == 0)
	{
		m_pPane[0] = m_pPane[1];
	}

	SetPane( 1, nullptr, 0, 0);
}

void i3TDKDockSplitWnd::RemoveAllPane(void)
{
	INT32 i;

	for( i = 0; i < 2; i++)
	{
		if( m_pPane[i] != nullptr)
		{
			if( m_pPane[i]->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)) ||
				m_pPane[i]->IsKindOf( RUNTIME_CLASS( i3TDKDockWnd))	)
			{
				m_pPane[i]->DestroyWindow();
				delete m_pPane[i];				// 무슨 이유인지 몰라도 주석되어있었는데..현재로서는 메모리릭의 원인으로 보여서..
				m_pPane[i] = nullptr;				// delete라인의 주석을 푼다...(2012.09.07.수빈)
			}
			else
			{
				I3ASSERT_0;
			}
		}
	}
}

void i3TDKDockSplitWnd::GetPanePath( CWnd * pPane, TCHAR* pszStr)
{
	INT32 idx;

	if( GetPane(0) == pPane)
		idx = 0;
	else if( GetPane(1) == pPane)
		idx = 1;
	else
	{
		I3ASSERT_0;
		return;
	}

	if( m_ExStyle & I3TDK_DOCK_STYLE_VERT)
	{
		// 세로 방향
		if( idx == 0)		i3::generic_string_copy( pszStr, _T("U") );
		else				i3::generic_string_copy( pszStr, _T("D") );
	}
	else
	{
		// 가로 방향
		if( idx == 0)		i3::generic_string_copy( pszStr, _T("L") );
		else				i3::generic_string_copy( pszStr, _T("R") );
	}
}

// Descendents들을 모두 검색해 Path를 구한다.
BOOL i3TDKDockSplitWnd::FindPanePath( CWnd * pPane, TCHAR* pszStr)
{
	INT32 i;
	CWnd * pChild;
	
	for( i = 0; i < 2; i++)
	{
		pChild = m_pPane[i];

		if( m_ExStyle & I3TDK_DOCK_STYLE_VERT)
		{
			// 세로 방향
			if( i == 0)		*pszStr = 'U';
			else			*pszStr = 'D';
		}
		else
		{
			// 가로 방향
			if( i == 0)		*pszStr = 'L';
			else			*pszStr = 'R';
		}

		if( pChild == pPane)
		{
			*(pszStr + 1) = 0;
			return TRUE;
		}


		if( pChild != nullptr)
		{
			if( pChild->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)))
			{
				i3TDKDockSplitWnd * pSplit = (i3TDKDockSplitWnd *) pChild;

				if( pSplit->FindPanePath( pPane, pszStr + 1) == TRUE)
					return TRUE;
			}
			else if( pChild->IsKindOf( RUNTIME_CLASS( i3TDKDockWnd)))
			{
				i3TDKDockWnd * pDock = (i3TDKDockWnd *) pChild;

				if( pDock->FindPane( pPane) != -1)
				{
					*(pszStr + 1) = 0;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

void i3TDKDockSplitWnd::getRect( INT32 side, INT32 w, INT32 h, CRect * pRect)
{
	INT32 other = side ^ 1;

	pRect->left = 0;
	pRect->top = 0;
	pRect->right = m_Width;
	pRect->bottom = m_Height;

	if( m_pPane[other] != nullptr)
	{
		// 양쪽 모두 Pane이 있는 경우에 대한 계산

		if( m_ExStyle & I3TDK_DOCK_STYLE_VERT)
		{
			// 세로 방향
			if( side == 0)
			{
				// 위쪽
				pRect->bottom = h - 2;
			}
			else
			{
				// 아래쪽
				pRect->top = m_Height - h + 2;
			}
		}
		else
		{
			// 가로 방향
			if( side == 0)
			{
				// 왼쪽
				pRect->right = w - 2;
			}
			else
			{
				// 오른쪽
				pRect->left = m_Width - w + 2;
			}
		}
	}
}

void i3TDKDockSplitWnd::CalcLayout( int cx, int cy, CRect * pRect1, CRect * pRect2)
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

	if((m_pPane[0] != nullptr) && ( m_pPane[1] != nullptr))
	{
		// Pane이 2개 있다.
		if( m_ExStyle & I3TDK_DOCK_STYLE_VERT)
		{
			// 수직 분할
			pRect1->bottom = m_SplitPos - 2;

			pRect2->top = m_SplitPos + 2;
		}
		else
		{
			// 수평 분할
			pRect1->right = m_SplitPos - 2;

			pRect2->left = m_SplitPos + 2;
		}
	}
	else if( m_pPane[0] != nullptr)
	{
		pRect2->right = 0;
		pRect2->bottom = 0;
	}
	else if( m_pPane[1] != nullptr)
	{
		pRect1->right = 0;
		pRect1->bottom = 0;
	}
	else
	{
		// Pane이 전혀 없다.
		pRect1->right = 0;
		pRect1->bottom = 0;

		pRect2->right = 0;
		pRect2->bottom = 0;
	}
}

void i3TDKDockSplitWnd::CalcSplitter( int cx, int cy, int newSz, CRect * pRect)
{
	if((cx == -1) || (cy == -1))
	{
		CRect rt;

		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	if( m_ExStyle & I3TDK_DOCK_STYLE_VERT)
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

void i3TDKDockSplitWnd::ReplacePanes( int cx, int cy, bool bRecurse)
{
	CRect Rc[2];
	INT32 i, w, h;
	bool bReplace;

	if( i3TDKDock::IsPendingUpdate())
		return;

	CalcLayout( cx, cy, &Rc[0], &Rc[1]);

	for( i = 0; i < 2; i++)
	{
		bReplace = false;

		if( m_pPane[i] != nullptr)
		{
			w = Rc[i].Width();
			h = Rc[i].Height();

			//if( Rc[i] != m_OldRect[i])
			{
				m_pPane[i]->SetWindowPos( nullptr, Rc[i].left, Rc[i].top, w, h, SWP_NOACTIVATE | SWP_NOZORDER);

				m_OldRect[i] = Rc[i];
			}

			if( m_pPane[i]->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)))
			{
				i3TDKDockSplitWnd * pChildSplit = (i3TDKDockSplitWnd *) m_pPane[i];

				pChildSplit->ReplacePanes( -1, -1);
			}
		}
	}
}

void i3TDKDockSplitWnd::DragStart( CPoint pt)
{
	CRect rt;

	SetCapture();

	m_ScreenDC = ::GetDC( nullptr);

	GetClientRect( &rt);
	m_Width = rt.Width();
	m_Height = rt.Height();

	CalcSplitter( m_Width, m_Height, m_SplitPos, &rt);

	ClientToScreen( &rt);

	m_DragRect = rt;

	::DrawFocusRect( m_ScreenDC, &rt);
	rt.InflateRect( 1, 1, -1, -1);
	::DrawFocusRect( m_ScreenDC, &rt);
}

void i3TDKDockSplitWnd::Drag( CPoint pt)
{
	CRect rt;

	// Erase Old Pos.
	{
		rt = m_DragRect;

		::DrawFocusRect( m_ScreenDC, &rt);
		rt.InflateRect( 1, 1, -1, -1);
		::DrawFocusRect( m_ScreenDC, &rt);
	}

	if( m_ExStyle & I3TDK_DOCK_STYLE_VERT)
	{
		m_SplitPos = pt.y;

		if( m_SplitPos < m_MinSize)
			m_SplitPos = m_MinSize;

		if( m_SplitPos > (m_Height - m_MinSize))
			m_SplitPos = m_Height - m_MinSize;
	}
	else
	{
		m_SplitPos = pt.x;

		if( m_SplitPos < m_MinSize)
			m_SplitPos = m_MinSize;

		if( m_SplitPos > (m_Width - m_MinSize))
			m_SplitPos = m_Width - m_MinSize;
	}

	CalcSplitter( m_Width, m_Height, m_SplitPos, &rt);

	ClientToScreen( &rt);

	m_DragRect = rt;

	::DrawFocusRect( m_ScreenDC, &rt);
	rt.InflateRect( 1, 1, -1, -1);
	::DrawFocusRect( m_ScreenDC, &rt);
}

void i3TDKDockSplitWnd::DragEnd( CPoint pt)
{
	CRect rt;

	CalcSplitter( m_Width, m_Height, m_SplitPos, &rt);

	rt = m_DragRect;

	::DrawFocusRect( m_ScreenDC, &rt);
	rt.InflateRect( 1, 1, -1, -1);
	::DrawFocusRect( m_ScreenDC, &rt);

	::ReleaseCapture();
	::ReleaseDC( nullptr, m_ScreenDC);

	ReplacePanes( m_Width, m_Height);
}

void i3TDKDockSplitWnd::Rearrange(void)
{
	ReplacePanes( -1, -1);
}

i3TDKDockSplitWnd *	i3TDKDockSplitWnd::InsertChildDock( INT32 side, INT32 w, INT32 h, bool bHorz)
{
	i3TDKDockSplitWnd * pTemp;
	CRect rc;
	UINT32 exStyle, Style;
	CWnd * pWnd;

	pWnd = m_pPane[side];

	// 새로운 DockWindow를 생성한다.
	pTemp = new i3TDKDockSplitWnd;

	if( bHorz)
		exStyle = 0;
	else
		exStyle = I3TDK_DOCK_STYLE_VERT;

	Style = ::GetWindowLong( m_hWnd, GWL_STYLE);

	I3ASSERT( pWnd != nullptr);
	pWnd->GetWindowRect( &rc);

	//getRect( side, rc.Width(), rc.Height(), &rc);
	ScreenToClient( &rc);

	pTemp->Create( exStyle, Style, rc, m_MinSize, this, side);

	SetPane( side, pTemp, rc.Width(), rc.Height());

	// 항상 첫번째에 추가한다. (첫번째에 들어간 Child라면 이후에 방향이 변경되어도 무난)
	pTemp->SetPane( LEFT, pWnd, rc.Width(), rc.Height());

	return pTemp;
}

i3TDKDockSplitWnd * i3TDKDockSplitWnd::_SplitDown( INT32 side, INT32 w, INT32 h, bool bHorz)
{
	i3TDKDockSplitWnd * pNewDock;
	UINT32 style, exStyle;
	INT32 other;
	CRect rc, wndrc;

	other = side ^ 1;
	exStyle = m_ExStyle;
	style = GetWindowLong( m_hWnd, GWL_STYLE);

	if( bHorz)
		m_ExStyle &= ~I3TDK_DOCK_STYLE_VERT;
	else
		m_ExStyle |= I3TDK_DOCK_STYLE_VERT;

	{
		// 자신을 Split하기 위해 기존의 내용을 대피시킨다.
		{
			pNewDock = new i3TDKDockSplitWnd;

			GetWindowRect( &wndrc);

			if( bHorz)
				getRect( other, wndrc.Width() - w, wndrc.Height(), &rc);
			else
				getRect( other, wndrc.Width(), wndrc.Height() - h, &rc);

			// 동일한 방향의 새로운 Child Dock...
			pNewDock->Create( exStyle, style, rc, m_MinSize, this, other);

			// 기존의 Child를 새로운 Dock으로 이전시킨다.
			if( m_pPane[0] != nullptr)
				pNewDock->SetPane( LEFT, m_pPane[0], m_SplitPos, m_SplitPos);

			if( m_pPane[1] != nullptr)
				pNewDock->SetPane( RIGHT, m_pPane[1], m_SplitPos, m_SplitPos);
		}
		
		// 초기화
		{
			m_pPane[0] = nullptr;
			m_pPane[1] = nullptr;
		}

		SetPane( other, pNewDock, rc.Width(), rc.Height());
	}

	// 새로운 Dock을 만들어 자신의 Left Child로 등록
	{
		pNewDock = new i3TDKDockSplitWnd;

		getRect( side, w, h, &rc);

		pNewDock->Create( m_ExStyle, style, rc, m_MinSize, this, side);

		SetPane( side, pNewDock, rc.Width(), rc.Height());
	}

	return pNewDock;
}

i3TDKDockSplitWnd *	 i3TDKDockSplitWnd::NewDockWnd( bool bHorz, INT32 w, INT32 h)
{
	CRect rc;
	i3TDKDockSplitWnd * pTemp;
	UINT32 exStyle = 0;

	pTemp = new i3TDKDockSplitWnd;

	rc.SetRect( 0, 0, w, h);

	if( bHorz == false)
		exStyle |= I3TDK_DOCK_STYLE_VERT;

	pTemp->Create( exStyle, WS_VISIBLE | WS_CHILD, rc, m_MinSize, this, GetTickCount());

	return pTemp;
}

// 지정된 문자열의 경로에 맞도록, Hierarchy를 구성한다.
// Recursive Call Function
void i3TDKDockSplitWnd::_PreparePath( const TCHAR* pszLoc, INT32 w, INT32 h)
{
	i3TDKDockSplitWnd * pTemp = this;
	INT32 idx = 0;
	bool bHorz = false;

	switch( *pszLoc)
	{
		case 'L' :
		case 'l' :		idx = 0;		bHorz = true;		break;
		case 'R' :
		case 'r' :		idx = 1;		bHorz = true;		break;
		case 'U' :
		case 'u' :		idx = 0;		bHorz = false;		break;
		case 'D' :
		case 'd' :		idx = 1;		bHorz = false;		break;
	}

	if( IsEmpty())
	{
		// 만약 빈 DockInfo라면 가로/세로 Style을 자유롭게 변경할 수 있는
		// 상태이기 때문에, 자신의 방향을 조절해서 강제로 맞춘다.

		if( bHorz)
			m_ExStyle &= ~I3TDK_DOCK_STYLE_VERT;
		else
			m_ExStyle |= I3TDK_DOCK_STYLE_VERT;
	}

	// 마지막 Path Code는 Prepare 대상이 아니기 때문에...
	if( *(pszLoc + 1) == 0)
	{
		// 마지막이라 하더라도, 양쪽 Pane이 모두 들어가 있다면
		// 추가할 수 없기 때문에 Split을 해야한다.
		if( (m_pPane[0] == nullptr) || (m_pPane[1] == nullptr))
			return;
	}

	if( ((m_ExStyle & I3TDK_DOCK_STYLE_VERT) == 0) == bHorz)
	{
		// 원하는 방향과 현재 자신의 Style이 같다면..
		// 단순 추가.
		if( m_pPane[ idx] == nullptr)
		{
			CRect rc;

			getRect( idx, w, h, &rc);

			pTemp = NewDockWnd( bHorz, rc.Width(), rc.Height());

			SetPane( idx, pTemp, rc.Width(), rc.Height());
		}
		else
		{
			if( m_pPane[idx]->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)))
			{
				// Recursive Call할 DockWindow를 설정.
				pTemp = (i3TDKDockSplitWnd *) m_pPane[ idx];
			}
			else
			{
				// Recursive Call을 해야하는데, DockWindow가 아니기 때문에
				// 새로운 DockWindow를 생성하여 기존의 Child를 붙인다.
				pTemp = InsertChildDock( idx, w, h, bHorz);
			}
		}
	}
	else
	{
		// 요구되는 방향과 자신의 Style이 같지 않다면...
		// 새로운 Dock Info를 생성해, Tree를 Split Down한다.
		pTemp = _SplitDown( idx, w, h, bHorz);
	}

	if((pTemp != nullptr) && ( *(pszLoc + 1) != 0))
	{
		// Recursive call
		pTemp->_PreparePath( pszLoc + 1, w, h);
	}
}

i3TDKDockSplitWnd * i3TDKDockSplitWnd::_FindPath( const TCHAR* pszLoc)
{
	i3TDKDockSplitWnd * pTemp;
	INT32 idx = 0;

	switch( *pszLoc)
	{
		case 'U' :
		case 'u' :
		case 'L' :
		case 'l' :		idx = 0;		break;
		case 'R' :
		case 'r' :
		case 'D' :
		case 'd' :		idx = 1;		break;

		default :
			I3ASSERT_0;
	}

	if( *(pszLoc + 1) == 0)
	{
		return this;
	}

	// 아직 찾는 중이므로 Recursive하게 Call해 들어간다.
	I3ASSERT( m_pPane[idx]->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)));

	pTemp = (i3TDKDockSplitWnd *) m_pPane[idx];

	return pTemp->_FindPath( pszLoc + 1);
}

i3TDKDockWnd * i3TDKDockSplitWnd::PreparePath( const TCHAR* pszLoc, INT32 w, INT32 h)
{
	i3TDKDockSplitWnd * pParent;
	INT32 side;
	i3TDKDockWnd * pDockWnd;

	// Path를 따라가며, 없다면 DockWnd Tree를 추가-구성한다.
	// 마지막 Path Code에 대해서는 처리하지 않아야 한다.
	_PreparePath( pszLoc, w, h);

	// 마지막 Path의 Parent를 Return한다.
	pParent = _FindPath( pszLoc);

	switch( pszLoc[ i3::generic_string_size(pszLoc) - 1])
	{
		case 'U' :
		case 'u' :
		case 'L' :
		case 'l' :		side = 0;		break;
		case 'R' :
		case 'r' :
		case 'D' :
		case 'd' :		side = 1;		break;

		default :
			I3ASSERT_0;
			return nullptr;
	}

	// AddPane() 함수 안에서는 해당 side의 Pane이 있건 없건,
	// 다른 종류의 Window이건 간에 처리를 해 주어야 한다.
	// 또한 방향이 같지 않은 경우에 대해서도 처리한다.
	pDockWnd = pParent->_PrepareDockWnd( side, w, h);

	ReplacePanes( -1, -1);

	return pDockWnd;
}

// 해당 Window를 Recursive하게 검색하여 제거한다.
void i3TDKDockSplitWnd::UndockWindow( CWnd * pWnd)
{
	INT32 i;

	for( i = 0; i < 2; i++)
	{
		if( GetPane(i) == nullptr)
			continue;

		if( GetPane(i) == pWnd)
		{
			// 해당 Window.
			// 제거한다. 그러나 단순한 Undock이기 때문에 아무런 Memory Freeing 처리는
			// 하지 않는다.
			m_pPane[i] = nullptr;
			Rearrange();
			return;
		}

		if( GetPane(i)->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)))
		{
			// Recursive Call
			i3TDKDockSplitWnd * pTemp = (i3TDKDockSplitWnd *) GetPane(i);

			pTemp->UndockWindow( pWnd);
		}
		else
		{
			I3ASSERT( GetPane(i)->IsKindOf( RUNTIME_CLASS( i3TDKDockWnd)));

			i3TDKDockWnd * pTemp = (i3TDKDockWnd *) GetPane(i);

			if( pTemp->FindPane( pWnd) != -1)
			{
				pTemp->RemovePane( pWnd);

				return;
			}
		}
	}
}

void i3TDKDockSplitWnd::_collapseChildSplit( INT32 idx)
{
	i3TDKDockSplitWnd * pSplit = (i3TDKDockSplitWnd *) GetPane( idx);

	// 여기에 호출된 상태에서 pSplit은 어느 한쪽의 Child만 가지고
	// 있거나, 또는 Child를 전혀 가지고 있지 않은 경우이다.

	if( (pSplit->GetPane(0) == nullptr) && (pSplit->GetPane(1) == nullptr))
	{
		// 양쪽 모두 없는 경우, 아예 제거한다.
		m_pPane[idx] = nullptr;
	}
	else if( pSplit->GetPane(0) != nullptr)
	{
		m_pPane[idx] = pSplit->GetPane(0);
		pSplit->m_pPane[0] = nullptr;

		SetChildAttr( m_pPane[idx]);
	}
	else if( pSplit->GetPane(1) != nullptr)
	{
		m_pPane[idx] = pSplit->GetPane(1);
		pSplit->m_pPane[1] = nullptr;

		SetChildAttr( m_pPane[idx]);
	}

	delete pSplit;
}

void i3TDKDockSplitWnd::_collapseChildDock( INT32 idx)
{
	i3TDKDockWnd * pDock = (i3TDKDockWnd *) GetPane( idx);

	delete pDock; 

	m_pPane[idx] = nullptr;
}

void i3TDKDockSplitWnd::GarbageCollect(void)
{
	INT32 i;

	// 우선 Child Pane들에게 정리할 기회를 먼저 준다.
	{
		for( i = 0;	i < 2; i++)
		{
			if( GetPane(i) == nullptr)
				continue;

			if( GetPane(i)->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)))
			{
				// Recursive Call
				i3TDKDockSplitWnd * pSplit = (i3TDKDockSplitWnd *) GetPane(i);

				pSplit->GarbageCollect();

				// 어느 한쪽의 Child Pane이 없는 경우는 정리 대상이 된다.
				if((pSplit->GetPane(0) == nullptr) || (pSplit->GetPane(1) == nullptr))
				{
					_collapseChildSplit(i);
				}
			}
			else if( GetPane(i)->IsKindOf( RUNTIME_CLASS( i3TDKDockWnd)))
			{
				i3TDKDockWnd * pDock = (i3TDKDockWnd *) GetPane(i);

				// 아무런 Pane도 가지고 있지 않은 Dock Window라면 제거
				if( pDock->GetPaneCount() <= 0)
				{
					_collapseChildDock( i);
				}
			}
		}
	}	
}


I3TDK_DOCK_PART i3TDKDockSplitWnd::HitTestRect( POINT pt)
{
	CRect temp, scrRect;

	GetWindowRect( &scrRect);

	if( PtInRect( &scrRect, pt) == FALSE)
		return I3TDK_DOCK_PART_NONE;

	temp = scrRect;

	// Left side
	{
		temp.right = MIN( scrRect.left + DOCK_SPACE, scrRect.right);

		if( PtInRect( &temp, pt))
			return I3TDK_DOCK_PART_LEFT;
	}

	// Right side
	{
		temp.left = MAX( scrRect.right - DOCK_SPACE, scrRect.left);
		temp.right = scrRect.right;

		if( PtInRect( &temp, pt))
			return I3TDK_DOCK_PART_RIGHT;
	}

	// top side
	{
		temp.left = scrRect.left;
		temp.bottom = MIN( scrRect.top + DOCK_SPACE, scrRect.bottom);

		if( PtInRect( &temp, pt))
			return I3TDK_DOCK_PART_TOP;
	}

	// bottom side
	{
		temp.top = MAX( scrRect.bottom - DOCK_SPACE, scrRect.top);
		temp.bottom = scrRect.bottom;

		if( PtInRect( &temp, pt))
			return I3TDK_DOCK_PART_BOTTOM;
	}

	return I3TDK_DOCK_PART_NONE;
}

BOOL i3TDKDockSplitWnd::FindPaneByPos( i3TDKDockSplitWnd * pParent, I3TDK_FIND_DOCK_INFO * pInfo)
{
	i3TDKDockSplitWnd * pChildDock;
	I3TDK_DOCK_PART hit;
	INT32 i;

	for( i = 0; i < 2; i++)
	{
		if((GetPane(i) != nullptr) && (GetPane(i) != pInfo->m_pDragWnd))
		{
			if( GetPane(i)->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)))
			{
				pChildDock = (i3TDKDockSplitWnd *) GetPane(i);

				if( pChildDock->FindPaneByPos( this, pInfo) == TRUE)
					return TRUE;
			}
			else
			{
				i3TDKDockWnd * pDockCtrl = (i3TDKDockWnd *) GetPane(i);

				hit = pDockCtrl->HitTestRect( pInfo->m_Point);

				if( hit != I3TDK_DOCK_PART_NONE)
				{
					// 추가해야 한다.
					pInfo->m_Hit = hit;
					pInfo->m_pParent = this;
					pInfo->m_pWnd = GetPane(i);
					return TRUE;
				}
			}
		}
	}

	// 자기의 Rect로 테스트
	hit = HitTestRect( pInfo->m_Point);

	if( hit != I3TDK_DOCK_PART_NONE)
	{
		pInfo->m_Hit = hit;
		pInfo->m_pParent = pParent;
		pInfo->m_pWnd = this;
		return TRUE;
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(i3TDKDockSplitWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// i3TDKDockSplitWnd message handlers

BOOL i3TDKDockSplitWnd::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}

void i3TDKDockSplitWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void i3TDKDockSplitWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	{
		// 이전 크기에 대한 비율을 감안해, Split Pos를 바꾼다.
		if( IsHorz())
		{
			if( m_Width != 0)
			{
				m_SplitPos = m_SplitPos * cx / m_Width;
			}
		}
		else
		{
			if( m_Height != 0)
			{
				m_SplitPos = m_SplitPos * cy / m_Height;
			}
		}
	}

	m_Width = cx;
	m_Height = cy;

	ReplacePanes( cx, cy);
}

void i3TDKDockSplitWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanged(lpwndpos);

	m_Width = lpwndpos->cx;
	m_Height = lpwndpos->cy;

	ReplacePanes( lpwndpos->cx, lpwndpos->cy);
}


void i3TDKDockSplitWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging == TRUE)
	{
		Drag( point);

		if( m_ExStyle & I3TDK_DOCK_STYLE_VERT)
			SetCursor( g_pTDKGlobalRes->m_hCursorSizeNS);
		else
			SetCursor( g_pTDKGlobalRes->m_hCursorSizeWE);
	}
	else
	{
		CRect rt;

		CalcSplitter( -1, -1, m_SplitPos, &rt);

		if( PtInRect( &rt, point))
		{
			if( m_ExStyle & I3TDK_DOCK_STYLE_VERT)
				SetCursor( g_pTDKGlobalRes->m_hCursorSizeNS);
			else
				SetCursor( g_pTDKGlobalRes->m_hCursorSizeWE);
		}
		else
		{
			SetCursor( g_pTDKGlobalRes->m_hCursorArrow);
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void i3TDKDockSplitWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_bDragging == FALSE)
	{
		DragStart( point);

		m_bDragging = TRUE;

	}

	CWnd::OnLButtonDown(nFlags, point);
}

void i3TDKDockSplitWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging == TRUE)
	{
		DragEnd( point);

		m_bDragging = FALSE;
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void i3TDKDockSplitWnd::SetWindowInfo( CWnd * pWnd, i3TDKWindowInfo * pInfo)
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
	pInfo->SetSize( m_SplitPos);
}

void i3TDKDockSplitWnd::GetAllDockWnd( i3::vector<i3TDKDockWnd*>& List)
{
	INT32 i;

	for( i = 0; i < 2; i++)
	{
		if( m_pPane[i] != nullptr)
		{
			if( m_pPane[i]->IsKindOf( RUNTIME_CLASS( i3TDKDockWnd)))
			{
				List.push_back( static_cast<i3TDKDockWnd*>(m_pPane[i]));
			}
			else if( m_pPane[i]->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)))
			{
				i3TDKDockSplitWnd * pSplit = (i3TDKDockSplitWnd *) m_pPane[i];

				pSplit->GetAllDockWnd( List);
			}
		}
	}
}

void i3TDKDockSplitWnd::DumpPath( INT32 level)
{
	INT32 i;
	char tab[1024];

	tab[level] = 0;

	for( i = 0; i < 2; i++)
	{
		if( m_pPane[i] != nullptr)
		{
			if( m_pPane[i]->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)))
			{
				i3TDKDockSplitWnd * pChild = (i3TDKDockSplitWnd *) m_pPane[i];

				pChild->DumpPath( level + 1);
			}
			else
			{
				//CRuntimeClass * pRTTI = m_pPane[i]->GetRuntimeClass();
			}
		}
	}
}

BOOL i3TDKDockSplitWnd::AdjustPaneSize( INT32 level, CWnd * pChild, INT32 cx, INT32 cy)
{
	INT32 i, side = -1;
	i3TDKDockSplitWnd * pSplit;
	CRect rc, PaneRc[2];

	if( i3TDKDock::IsPendingUpdate())
		return TRUE;

	for( i = 0; i < 2; i++)
	{
		if( m_pPane[i] != nullptr)
		{
			if( m_pPane[i]->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)))
			{
				pSplit = (i3TDKDockSplitWnd *) m_pPane[i];

				if( pSplit->AdjustPaneSize( level + 1, pChild, cx, cy))
				{
					// 해당 Pane의 하위에 크기를 강제하려는 pChild가 있다.
					side = i;
				}
			}
			else if( m_pPane[i] == pChild)
			{
				side = i;
				m_pPane[i]->SetWindowPos( nullptr, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
			}

			m_pPane[i]->GetWindowRect( &PaneRc[i]);
		}
		else
		{
			PaneRc[i].SetRect( 0, 0, 0, 0);
		}
	}

	if( side != -1)
	{
		// 크기 조절이 필요함.
		rc.left = 0;
		rc.top = 0;

		if( IsHorz())
		{
			if((m_pPane[0] != nullptr) && (m_pPane[1] != nullptr))
				rc.right = PaneRc[0].Width() + PaneRc[1].Width() + 4;
			else
				rc.right = PaneRc[side].Width();

			rc.bottom = PaneRc[side].Height();			
		}
		else
		{
			rc.right = PaneRc[side].Width();

			if((m_pPane[0] != nullptr) && (m_pPane[1] != nullptr))
				rc.bottom = PaneRc[0].Height() + PaneRc[1].Height() + 4;
			else
				rc.bottom = PaneRc[side].Height();
		}

		if( level > 0)
		{
			// 자기 자신의 크기를 늘여서라도 크기를 맞춘다.
			SetWindowPos( nullptr, 0, 0, rc.Width(), rc.Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
		}

		// 최상위 SplitWnd는 크기는 바꾸지 않고, SplitPos만 조절하여
		// 원하는 크기를 맞추어 준다.
		{
			GetClientRect( &rc);

			if( IsHorz())
			{
				if( side == 0)
					m_SplitPos = PaneRc[0].Width() + 2;
				else
					m_SplitPos = rc.Width() - PaneRc[1].Width() - 2;
			}
			else
			{
				if( side == 0)
					m_SplitPos = PaneRc[0].Height() + 2;
				else
					m_SplitPos = rc.Height() - PaneRc[1].Height() - 2;
			}
		}

		if( level == 0)
		{
			ReplacePanes( -1, -1, true);
		}

		return TRUE;
	}

	return FALSE;
}

void i3TDKDockSplitWnd::OnDestroy()
{
	RemoveAllPane();

	CWnd::OnDestroy();
}
