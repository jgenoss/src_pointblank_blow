#include "stdafx.h"
#include "i3TDKDockFrameWnd.h"
#include "i3TDKDockSplitWnd.h"
#include "i3TDKDockWnd.h"
#include "afxadv.h"
#include "i3TDKDock.h"
#include ".\i3tdkdockframewnd.h"
#include "i3Base/string/ext/atoi.h"

i3TDKDockFrameWnd *		g_pDockFrameWnd;

static TCHAR s_szSide[][32] = 
	{
		_T("L"),
		_T("U"),
		_T("R"),
		_T("D")
	};

IMPLEMENT_DYNCREATE( i3TDKDockFrameWnd, CFrameWnd)

BEGIN_MESSAGE_MAP(i3TDKDockFrameWnd, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
//	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

i3TDKDockFrameWnd::i3TDKDockFrameWnd(void)
{
	g_pDockFrameWnd = this;
}

void i3TDKDockFrameWnd::Reset(void)
{
	
	CWnd * pPane;

	// Dock/Float Window들을 제거하기 전에
	// Pane들을 우선 대피시켜야 한다.
	for(size_t i = 0; i < m_PaneList.size(); i++)
	{
		pPane = m_PaneList[i];

		pPane->ShowWindow( SW_HIDE);
		pPane->SetParent( this);
		pPane->SetOwner( this);
	}

	m_pDock->RemoveAllPane();

	{
		CRect rc;

		GetClientRect( &rc);

		m_pDock->SetWindowPos( nullptr, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}

	for(size_t i = 0; i < m_FloatWndList.size(); i++)
	{
		i3TDKDockWnd * pDock = m_FloatWndList[i];

		delete pDock;
	}

	m_FloatWndList.clear();
}

void i3TDKDockFrameWnd::DockWindow( CWnd * pWnd, const TCHAR* pszLoc, INT32 w, INT32 h)
{
	i3TDKDockWnd * pDock;

	pWnd->ShowWindow( SW_SHOW);

	pDock = m_pDock->PreparePath( pszLoc, w, h);

	pDock->AddPane( pWnd);

//	m_PaneList.Add( pWnd);
	i3::vu::push_back_once(m_PaneList, pWnd);
}

void i3TDKDockFrameWnd::DrawDragRect( RECT * pRect)
{
	CRect rc = *pRect;

	SetROP2( m_DragDC, R2_XORPEN);

	::DrawFocusRect( m_DragDC, &rc);

	InflateRect( &rc, -1, -1);
	::DrawFocusRect( m_DragDC, &rc);

	InflateRect( &rc, -1, -1);
	::DrawFocusRect( m_DragDC, &rc);
}

void i3TDKDockFrameWnd::BeginDock( CWnd * pWnd, I3TDK_DOCK_DROP_TYPE type, INT32 idx, POINT pt, INT32 defw, INT32 defh)
{
	SetCapture();

	// Screen-Coordinates
	m_DragStartPt = pt;

	pWnd->GetWindowRect( &m_DragWndRect);

	m_DragWndRect.right = m_DragWndRect.left + defw;
	m_DragWndRect.bottom = m_DragWndRect.top + defh;

	m_pDragWnd = pWnd;

	m_OldDragRect = m_DragWndRect;

	m_bDockDrag = true;

	m_FindDockInfo.m_pDragWnd = m_pDragWnd;
	m_FindDockInfo.m_Point = pt;
	m_FindDockInfo.m_Hit = I3TDK_DOCK_PART_NONE;
	m_FindDockInfo.m_DropType = type;
	m_FindDockInfo.m_PaneIndex = idx;

	m_DragDC = ::GetDC( nullptr);

	DrawDragRect( &m_DragWndRect);
}

BOOL i3TDKDockFrameWnd::_FindPaneByPos(void)
{
	BOOL Rv;
	i3TDKDockWnd * pDockCtrl;
	I3TDK_DOCK_PART hit;

	// 우선 Dock Window에서 먼저 검색한다.
	Rv = m_pDock->FindPaneByPos( nullptr, &m_FindDockInfo);
	if( Rv == TRUE)
		return Rv;

	// 만약 Dock Window 중에 찾아지지 않았다면
	// Floating Window 들 중에서 찾는다.
	for(size_t i = 0; i < m_FloatWndList.size(); i++)
	{
		pDockCtrl = m_FloatWndList[i];

		hit = pDockCtrl->HitTestRect( m_FindDockInfo.m_Point);
		if( hit != I3TDK_DOCK_PART_NONE)
		{
			m_FindDockInfo.m_Hit = hit;
			m_FindDockInfo.m_pWnd = pDockCtrl;
			return TRUE;
		}
	}

	return FALSE;
}

INT32 i3TDKDockFrameWnd::_FindPaneInDockList( i3::vector<i3TDKDockWnd*>& List, CWnd * pWnd)
{
	i3TDKDockWnd * pDock;

	for(size_t i = 0; i < List.size(); i++)
	{
		pDock = List[i];

		if( pDock->FindPane( pWnd) != -1)
			return i;
	}

	return -1;
}

void i3TDKDockFrameWnd::DragDock( POINT pt, bool bFloat)
{
	INT32 dx, dy, qx, qy;
	bool bDock = false;

	// 이전 Tracking Rect를 지운다.
	DrawDragRect( &m_OldDragRect);

	m_FindDockInfo.m_Point = pt;
	m_FindDockInfo.m_Hit = I3TDK_DOCK_PART_NONE;

	if( bFloat)
	{
		// 강제로 Floating 시키는 상태..
		m_FindDockInfo.m_pWnd = m_FindDockInfo.m_pDragWnd;
	}
	else
	{
		if( PtInRect( &m_DragWndRect, pt) == FALSE)
		{
			// Dragging하고 있는 Window의 영역 안이라면
			// 아예 대상 Docking Window를 검색하지 않도록 한다.
			if( _FindPaneByPos())
			{
				if( m_FindDockInfo.m_pWnd != m_pDragWnd)
				{
					bDock = true;
				}
			}
		}
		else
		{
			m_FindDockInfo.m_pWnd = m_FindDockInfo.m_pDragWnd;
		}
	}

	if( bDock)
	{
		// 어딘가 Docking될 수 있다.
		m_FindDockInfo.m_pWnd->GetWindowRect( &m_OldDragRect);

		qx = m_OldDragRect.Width() >> 1;
		qy = m_OldDragRect.Height() >> 1;

		switch( m_FindDockInfo.m_Hit)
		{
			case I3TDK_DOCK_PART_LEFT :		
				m_OldDragRect.right = m_OldDragRect.left + qx;
				break;

			case I3TDK_DOCK_PART_RIGHT :
				m_OldDragRect.left = m_OldDragRect.right - qx;
				break;

			case I3TDK_DOCK_PART_TOP :
				m_OldDragRect.bottom = m_OldDragRect.top + qy;
				break;

			case I3TDK_DOCK_PART_BOTTOM :
				m_OldDragRect.top = m_OldDragRect.bottom - qy;
				break;
		}
	}
	else
	{
		// Docking되지 않고 Floating되어야 한다.

		dx = pt.x - m_DragStartPt.x;
		dy = pt.y - m_DragStartPt.y;

		m_OldDragRect.left = m_DragWndRect.left + dx;
		m_OldDragRect.top = m_DragWndRect.top + dy;
		m_OldDragRect.right = m_DragWndRect.right + dx;
		m_OldDragRect.bottom = m_DragWndRect.bottom + dy;
	}

	DrawDragRect( &m_OldDragRect);
}

void i3TDKDockFrameWnd::_ExtractPanes( I3TDK_FIND_DOCK_INFO * pInfo)
{
	CWnd * pSrcWnd;
	INT32 i;

	pSrcWnd = pInfo->m_pDragWnd;

	m_MovePaneList.clear();

	if( pInfo->m_DropType == I3TDK_DOCK_DROP_WHOLE)
	{
		// 전체가 이동되는...
		I3ASSERT( pSrcWnd->IsKindOf( RUNTIME_CLASS( i3TDKDockWnd)));

		i3TDKDockWnd * pDock = (i3TDKDockWnd *) pSrcWnd;

		for( i = 0; i < pDock->GetPaneCount(); i++)
		{
			m_MovePaneList.push_back( pDock->GetPane(i));
		}

		// 이동 대상인 Pane들을 모두 제거한다.
		// Pane들 자체는 제거되지 않는다.
		pDock->RemoveAllPane();

		// 원본이 되는 Dock Window는 후의 GarbageCollect 처리해서 제거된다.
	}
	else
	{
		// Tab 중의 하나만 이동되는...
		I3ASSERT( pSrcWnd->IsKindOf( RUNTIME_CLASS( i3TDKTabCtrl)));

		i3TDKTabCtrl * pTab = (i3TDKTabCtrl *) pSrcWnd;
		CWnd * pWnd;

		pWnd = pTab->GetPane( pInfo->m_PaneIndex);
		m_MovePaneList.push_back( pWnd);

		pTab->RemovePane( pWnd);		// Tab에서 제거.
	}
}

i3TDKDockWnd * i3TDKDockFrameWnd::NewFloatWnd( INT32 defw, INT32 defh, RECT * pRect)
{
	i3TDKDockWnd * pDock;

	pDock = new i3TDKDockWnd;

	pDock->Create( WS_VISIBLE, defw, defh, *pRect, nullptr, (UINT) pDock);
	pDock->SetOwner( this);

	m_FloatWndList.push_back( pDock);

	return pDock;
}

i3TDKDockWnd * i3TDKDockFrameWnd::_PrepareDock( I3TDK_FIND_DOCK_INFO * pInfo)
{
	CWnd * pDestWnd;
	i3TDKDockWnd * pDock = nullptr;
	CRect rc;

	if( pInfo->m_Hit == I3TDK_DOCK_PART_NONE)
	{
		INT32 w, h;

		pInfo->m_pDragWnd->GetWindowRect( &rc);

		w = rc.Width();
		h = rc.Height();

		// Floating되어야 하는 상황
		pDock = NewFloatWnd( w, h, &m_OldDragRect);
	}
	else
	{
		pDestWnd = pInfo->m_pWnd;

		// 대상이 되는 Window의 종류 파악
		// 2가지 경우.
		// 1. i3TDKDockSplitWnd
		// 2. i3TDKDockWnd

		if( pInfo->m_Hit == I3TDK_DOCK_PART_IN)
		{
			I3ASSERT( pDestWnd->IsKindOf( RUNTIME_CLASS( i3TDKDockWnd)));

			pDock = (i3TDKDockWnd *) pDestWnd;
		}
		else
		{
			i3TDKDockSplitWnd * pSplit;
			TCHAR szLoc[32];

			if( pDestWnd->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)))
			{
				pSplit = (i3TDKDockSplitWnd *) pDestWnd;
				i3::generic_string_copy( szLoc, s_szSide[ pInfo->m_Hit]);
			}
			else 
			{
				pSplit = (i3TDKDockSplitWnd *) pInfo->m_pParent;

				pSplit->GetPanePath( pInfo->m_pWnd, szLoc);

				i3::generic_string_cat( szLoc, s_szSide[ pInfo->m_Hit]);
			}

			I3ASSERT( pSplit->IsKindOf( RUNTIME_CLASS( i3TDKDockSplitWnd)));

			pInfo->m_pDragWnd->GetWindowRect( &rc);

			pDock = pSplit->PreparePath( szLoc, rc.Width(), rc.Height());
		}
	}

	return pDock;
}

void i3TDKDockFrameWnd::_GarbageCollect(void)
{
	i3TDKDockWnd * pFloat;

	m_pDock->GarbageCollect();

	// Floating Window에 대해서도 처리한다.
	for( size_t i = 0; i < m_FloatWndList.size(); i++)
	{
		pFloat = m_FloatWndList[i];

		if( pFloat->GetPaneCount() <= 0)
		{
			m_FloatWndList.erase( m_FloatWndList.begin() + i);
			delete pFloat;
			i--;
		}
	}
}

void i3TDKDockFrameWnd::EndDock( POINT pt, bool bFloat)
{
	INT32 dist;
	CRect rect;
	i3TDKDockWnd * pDestDock;
	CPoint scrPt;
	

	// 이전 Tracking Rect를 지운다.
	CancelDock();

	scrPt = m_FindDockInfo.m_Point;

	// Dragging을 시작했던 점과의 거리를 구한다.
	{
		dist = (scrPt.x - m_DragStartPt.x);
		dist *= dist;

		INT32 i = (scrPt.y - m_DragStartPt.y);
		dist += (i * i);

		dist = (INT32) (sqrt( (double)dist));
	}
	
	if(m_FindDockInfo.m_Hit == I3TDK_DOCK_PART_NONE)
	{
		// Docking되지 않고 Floating되어야 한다.
		if( m_FindDockInfo.m_pDragWnd->IsKindOf( RUNTIME_CLASS( i3TDKDockWnd)))
		{
			i3TDKDockWnd * pSrcWnd = (i3TDKDockWnd *) m_FindDockInfo.m_pDragWnd;

			if( pSrcWnd->IsDocked() == false)
			{
				// 이미 Floating 상태의 Dock window라면..
				// 단순히 위치 이동으로 끝..
				pSrcWnd->SetWindowPos( nullptr, 
									m_OldDragRect.left, m_OldDragRect.top, 0, 0, 
									SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
				return;
			}
		}
	}

	if( dist < 32)
	{
		// 충분한만큼 Dragging을 하지 않았기 때문에 Drag을 하지 않는 것으로 간주.
		return;
	}

	// 이하는 단순한 이동만이 아닌, Docking Window 간에 Pane을 교환해야 하는 상황.

	// 우선 원본 Pane에서 이동 대상이 되는 Pane들을 떼어낸다.
	_ExtractPanes( &m_FindDockInfo);

	// 붙일 대상이 되는 DockWindow를 준비한다.
	pDestDock = _PrepareDock( &m_FindDockInfo);
	
	// 대상 Dock에 추가한다.
	for( size_t i = 0; i < m_MovePaneList.size(); i++)
	{
		pDestDock->AddPane( m_MovePaneList[i]);
	}

	// 이동의 결과로 불필요해진 여러 Dock들의 정리
	_GarbageCollect();

	// 위치 조정
	m_pDock->Rearrange();
}

void i3TDKDockFrameWnd::CancelDock(void)
{
	DrawDragRect( &m_OldDragRect);
	::ReleaseCapture();
	::ReleaseDC( nullptr, m_DragDC);
	m_DragDC = nullptr;
	m_pDragWnd = nullptr;
	m_bDockDrag = false;
}

int i3TDKDockFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetClientRect( &m_ClientRect);

	{
		m_pDock = new i3TDKDockSplitWnd;

		m_pDock->Create( 0, WS_CHILD | WS_VISIBLE, m_ClientRect, 32, this, 1);
	}

	return 0;
}

void i3TDKDockFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	if( nType != SIZE_MINIMIZED)
	{
		m_ClientRect.right = cx;
		m_ClientRect.bottom = cy;

		if( m_pDock != nullptr)
		{
			m_pDock->SetWindowPos( nullptr, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
		}
	}
}

void i3TDKDockFrameWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDockDrag )
	{
		ClientToScreen( &point);
		EndDock( point, (nFlags & MK_CONTROL) != 0);
	}

	CFrameWnd::OnLButtonUp(nFlags, point);
}

void i3TDKDockFrameWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDockDrag)
	{
		ClientToScreen( &point);
		DragDock( point, (nFlags & MK_CONTROL) != 0);
	}

	CFrameWnd::OnMouseMove(nFlags, point);
}

BOOL i3TDKDockFrameWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	INT32 mxx, mxy, mnx, mny;

	cs.x = AfxGetApp()->GetProfileInt( _T("DockFrameWnd"), _T("X"), CW_USEDEFAULT);
	cs.y = AfxGetApp()->GetProfileInt( _T("DockFrameWnd"), _T("Y"), CW_USEDEFAULT);
	cs.cx = AfxGetApp()->GetProfileInt( _T("DockFrameWnd"), _T("CX"), CW_USEDEFAULT);
	cs.cy = AfxGetApp()->GetProfileInt( _T("DockFrameWnd"), _T("CY"), CW_USEDEFAULT);

	mnx = GetSystemMetrics( SM_XVIRTUALSCREEN);
	mny = GetSystemMetrics( SM_YVIRTUALSCREEN);
	mxx = mnx + GetSystemMetrics( SM_CXVIRTUALSCREEN);
	mxy = mny + GetSystemMetrics( SM_CYVIRTUALSCREEN);

	cs.style |= WS_CLIPCHILDREN;

	if( cs.x < mnx)				cs.x = mnx;
	if( cs.y < mny)				cs.y = mny;

	if( cs.x > mxx)
	{
		if( (cs.x + cs.cx) > mxx)
		{
			cs.x = mnx;
			cs.cx = mxx - cs.x;
		}
		else
		{
			cs.x = mxx - cs.cx;
		}
	}

	if( cs.y > mxy)
	{
		if( (cs.y + cs.cy) > mxy)
		{
			cs.y = mny;
			cs.cy = mxy - cs.y;
		}
		else
		{
			cs.y = mxy - cs.cy;
		}
	}

	if( (cs.x + cs.cx) > mxx)	cs.cx = mxx - cs.x;
	if( (cs.y + cs.cy) > mxy)	cs.cy = mxy - cs.y;

	// 프레임 사이즈가 완전 이상하게 나와서 추가 함
	if( cs.cx <= 0 )	cs.cx = 1024;
	if( cs.cy <= 0 )	cs.cy = 768;

	return CFrameWnd::PreCreateWindow(cs);
}

void i3TDKDockFrameWnd::OnDestroy()
{
	if( IsIconic() == FALSE)
	{
		INT32 x, y, w, h;
		CRect rc;

		GetWindowRect( &rc);

		x = rc.left;
		y = rc.top;
		w = rc.Width();
		h = rc.Height();

		AfxGetApp()->WriteProfileInt( _T("DockFrameWnd"), _T("X"), x);
		AfxGetApp()->WriteProfileInt( _T("DockFrameWnd"), _T("Y"), y);
		AfxGetApp()->WriteProfileInt( _T("DockFrameWnd"), _T("CX"), w);
		AfxGetApp()->WriteProfileInt( _T("DockFrameWnd"), _T("CY"), h);
	}

	SaveDockContext();

	// 메모리릭으로 인해 추가된 루틴. (1.5에서 이미 반영된 상태인데..주석남기지 않았었음) 12.10.29.수빈.

	m_pDock->DestroyWindow();
	delete m_pDock;		// 여기가 안좋으면 소멸자로..
	m_pDock = nullptr;

	for(size_t i = 0; i < m_FloatWndList.size(); i++)
	{
		i3TDKDockWnd * pDock = m_FloatWndList[i];
		pDock->DestroyWindow();
		delete pDock;
	}

	m_FloatWndList.clear();

	/*
	{
		INT32 i;

		if( m_pDock != nullptr)
		{
			delete m_pDock;
			m_pDock = nullptr;
		}

		for( i = 0; i < m_FloatWndList.GetCount(); i++)
		{
			i3TDKDockWnd * pDock = (i3TDKDockWnd *) m_FloatWndList.Items[i];

			delete pDock;
		}

		m_FloatWndList.Clear();
	}
	*/

	CFrameWnd::OnDestroy();
}

void	i3TDKDockFrameWnd::SaveDockContext(void)
{
	INT32 idx;
	TCHAR szPath[128], szLabel[256], szCtx[MAX_PATH];
	CWnd * pPane;
	CRect rc;
	i3::vector<i3TDKDockWnd*> DockList;

	// 저장할 모든 i3TDKDockWnd의 List를 구한다.
	// 이들은 Docking된 것과 Floating된 것들을 모두 포함하여
	// 저장한다.
	{
		// Docked DockWnd
		m_pDock->GetAllDockWnd( DockList);

		// Floating DockWnd
		for(size_t i = 0; i < m_FloatWndList.size(); i++)
		{
			DockList.push_back( m_FloatWndList[i]);
		}
	}

	// 모든 Dock에 대한 정보를 Registry에 저장한다.
	//
	// Format
	//	-	Docking		: DOCK1		[1] [PATH] [Width] [Height]
	//  -	Floating	: DOCK2		[0] [Left] [Top] [Width] [Height]
	{
		AfxGetApp()->WriteProfileInt( _T("DOCK"), _T("DockCount"), (INT32)DockList.size());

		for( size_t i = 0; i < DockList.size(); i++)
		{
			i3TDKDockWnd * pDock = DockList[i];

			i3::snprintf( szLabel, _countof(szLabel), _T("DOCK%d"), i);

			pDock->GetWindowRect( &rc);

			if( pDock->IsDocked())
			{
				m_pDock->FindPanePath( pDock, szPath);

				i3::snprintf( szCtx, _countof(szCtx), _T("1 %s %d %d"), szPath, rc.Width(), rc.Height());
			}
			else
			{
				i3::snprintf( szCtx, _countof(szCtx), _T("0 %d %d %d %d"), rc.left, rc.top, rc.Width(), rc.Height());
			}

			AfxGetApp()->WriteProfileString( _T("DOCK"), szLabel, szCtx);
		}
	}

	// 개별 Pane에 대한 Index 및 크기를 저장해 둔다.
	// 필요 정보
	//	1. DockWnd Index

	AfxGetApp()->WriteProfileInt( _T("DOCK"), _T("PaneCount"), (INT32)m_PaneList.size());

	for(size_t i = 0; i < m_PaneList.size(); i++)
	{
		pPane = m_PaneList[i];

		idx = -1;

		// DockWnd List 중에서 찾는다.
		idx  = _FindPaneInDockList( DockList, pPane);
		I3ASSERT( idx != -1);

		i3::snprintf( szLabel, _countof(szLabel), _T("PANE%d"), i);

		AfxGetApp()->WriteProfileInt( _T("DOCK"), szLabel, idx);
	}
}

void	i3TDKDockFrameWnd::LoadDockContext(void)
{
	INT32 DockCount, PaneCount, i, w, h;
	i3TDKDockWnd * pDock;
	CWnd * pPane;
	TCHAR szLabel[128];
	TCHAR szToken[128], sep, szPath[128];

	CRect rc;
	CString str;
	i3TLineLexer<TCHAR> lex;
	i3::vector<i3TDKDockWnd*> DockList;

	// DockWnd들을 미리 생성하여 둔다.
	DockCount = AfxGetApp()->GetProfileInt( _T("DOCK"), _T("DockCount"), 0);
	PaneCount = AfxGetApp()->GetProfileInt( _T("DOCK"), _T("PaneCount"), 0);

	if((DockCount <= 0) && (PaneCount <= 0))
	{
		// Registry에 아무런 값이 없었기 때문에
		// Loading 작업을 진행하지 않는다.
		return;
	}

	Reset();
	
	//////////////////////
	rc.SetRect( 0, 0, 32, 32);

	i3TDKDock::SetPendingUpdate( true);

	for( i = 0; i < DockCount; i++)
	{
		i3::snprintf( szLabel, _countof(szLabel), _T("DOCK%d"), i);

		str = AfxGetApp()->GetProfileString( _T("DOCK"), szLabel, _T("") );

		lex.SetMasterString( LPCTSTR( str));

		// Docking/Floating
		lex.GetNextToken( szToken, &sep, _T(" "));

		if( i3::atoi( szToken))
		{
			// Docked DockWnd

			// Path
			lex.GetNextToken( szPath, &sep, _T(" "));

			// Width
			lex.GetNextToken( szToken, &sep, _T(" "));
			w = i3::atoi( szToken);

			// Height
			lex.GetNextToken( szToken, &sep, _T(" "));
			h = i3::atoi( szToken);

			pDock = m_pDock->PreparePath( szPath, w, h);
		}
		else
		{
			// Floated DockWnd

			// Left
			lex.GetNextToken( szToken, &sep, _T(" "));
			rc.left = i3::atoi( szToken);

			// Top
			lex.GetNextToken( szToken, &sep, _T(" "));
			rc.top = i3::atoi( szToken);

			// Width
			lex.GetNextToken( szToken, &sep, _T(" "));
			w = i3::atoi( szToken);
			rc.right = rc.left + w;

			// Height
			lex.GetNextToken( szToken, &sep, _T(" "));
			h = i3::atoi( szToken);
			rc.bottom = rc.top + h;

			pDock = NewFloatWnd( w, h, &rc);
		}

		DockList.push_back( pDock);
	}

	i3TDKDock::SetPendingUpdate( false);

	// 크기 재조정
	for( i = 0; i < DockCount; i++)
	{
		pDock = DockList[i];

		i3::snprintf( szLabel, _countof(szLabel), _T("DOCK%d"), i);

		str = AfxGetApp()->GetProfileString( _T("DOCK"), szLabel, _T("") );

		lex.SetMasterString( LPCTSTR( str) );

		// Docking/Floating
		lex.GetNextToken( szToken, &sep, _T(" "));

		if( i3::atoi( szToken))
		{
			// Docked DockWnd

			// Path
			lex.GetNextToken( szPath, &sep, _T(" "));		// 사용하지 않지만...

			// Width
			lex.GetNextToken( szToken, &sep, _T(" "));
			w = i3::atoi( szToken);

			// Height
			lex.GetNextToken( szToken, &sep, _T(" "));
			h = i3::atoi( szToken);

			I3TRACE( "------------%s [%d, %d]\n", szLabel, w, h);

			m_pDock->AdjustPaneSize( 0, pDock, w, h);		// 크기를 강제로 맞춘다>
		}
	}

	// 개별 Pane에 대한 정보를 읽어온다.
	// 필요 정보
	//	1. DockWnd Index

	for( i = 0; i < PaneCount; i++)
	{
		i3::snprintf( szLabel, _countof(szLabel), _T("PANE%d"), i);

		pPane = m_PaneList[i];

		INT32 idx = AfxGetApp()->GetProfileInt( _T("DOCK"), szLabel, -1);

		if( idx != -1)
		{
			pDock = DockList[idx];

			pDock->AddPane( pPane);
		}
	}
}

void i3TDKDockFrameWnd::PostNcDestroy()
{
	CFrameWnd::PostNcDestroy();
}
