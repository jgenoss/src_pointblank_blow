// i3GICDiagramView.cpp : implementation file
//

#include "stdafx.h"
#include "i3GICDiagramView.h"

#if defined( I3_DEBUG)

#include "i3GICDiagram.h"
#include "i3GICControlPoint.h"
#include "i3GICDiagramContext.h"
#include "resource.h"
#include "i3GICApp.h"

//#define DIRECT_DRAW

// i3GICDiagramView

IMPLEMENT_DYNAMIC(i3GICDiagramView, CWnd)
i3GICDiagramView::i3GICDiagramView()
{
	m_pDiagramCtx = i3GICDiagramContext::new_object();
	m_pDiagramCtx->Create( this, nullptr);

	// A4 용지 크기로 설정
	/*m_PageWidth = 2100.0f;
	m_PageHeight = 2970.0f;*/
	
	m_pKeyboard = i3InputKeyboard::new_object();
	m_pKeyboard->Create( this);
}

i3GICDiagramView::~i3GICDiagramView()
{
	I3_SAFE_RELEASE( m_pDiagramCtx);
	if( m_hGridPen)
		::DeleteObject( m_hGridPen);

	if( m_hCenterPen)
		::DeleteObject( m_hCenterPen);

	if( m_hbrBack != nullptr)
	{
		::DeleteObject( m_hbrBack);
		m_hbrBack = nullptr;
	}

	I3_SAFE_RELEASE( m_pKeyboard);
}

i3GICDiagram * i3GICDiagramView::getDiagram( void)
{
	return m_pDiagramCtx->getDiagram();
}

BOOL i3GICDiagramView::Create( LPCTSTR pszCaption, DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT32 nID)
{
	LPCTSTR pszClass;
	HCURSOR hCursor;
	HBRUSH	hbrBack;

	hCursor = ::LoadCursor( nullptr, IDC_ARROW);
	hbrBack = (HBRUSH) ::GetStockObject( WHITE_BRUSH);

	pszClass = ::AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS, hCursor, hbrBack, nullptr);

	return CWnd::Create( pszClass, pszCaption, dwStyle, rect, pParentWnd, nID);
}

void i3GICDiagramView::_Init(void)
{
	// 한번만 호출되어야 한다.

	// Mouse Cursor
	{
		memset( m_hCursors, 0, sizeof(m_hCursors));

		m_hCursors[ GIC_CURSOR_SELECT]		= ::LoadCursor( nullptr, IDC_ARROW);
		m_hCursors[ GIC_CURSOR_DISABLE]		= ::LoadCursor( nullptr, IDC_NO);
		m_hCursors[ GIC_CURSOR_CREATE]		= ::LoadCursor( g_hInstGIC, MAKEINTRESOURCE( IDC_POINTER_COPY));
		m_hCursors[ GIC_CURSOR_RESIZE_NS]	= ::LoadCursor( nullptr, IDC_SIZENS);
		m_hCursors[ GIC_CURSOR_RESIZE_WE]	= ::LoadCursor( nullptr, IDC_SIZEWE);
		m_hCursors[ GIC_CURSOR_RESIZE_NESW]	= ::LoadCursor( nullptr, IDC_SIZENESW);
		m_hCursors[ GIC_CURSOR_RESIZE_NWSE]	= ::LoadCursor( nullptr, IDC_SIZENWSE);
		m_hCursors[ GIC_CURSOR_MOVE_ALL]	= ::LoadCursor( nullptr, IDC_SIZEALL);
		m_hCursors[ GIC_CURSOR_LINK]		= ::LoadCursor( g_hInstGIC, MAKEINTRESOURCE( IDC_CURSOR_LINK));
		m_hCursors[ GIC_CURSOR_SCROLL]		= ::LoadCursor( g_hInstGIC, MAKEINTRESOURCE( IDC_CURSOR_SCROLL));
		m_hCursors[ GIC_CURSOR_CONTROL_VALUE] = ::LoadCursor( nullptr, IDC_SIZEWE);
	}

	// DC의 생성.
	#if !defined( DIRECT_DRAW)
	{
		HDC dc = ::GetDC( m_hWnd);

		m_hDrawDC = ::CreateCompatibleDC( dc);
		I3ASSERT( m_hDrawDC != nullptr);

		::ReleaseDC( m_hWnd, dc);
	}
	#else
		m_hDrawDC = ::GetDC( m_hWnd);
	#endif


	m_Zoom = 1.0f;

	m_rViewPosX = 0.0f;
	m_rViewPosY = 0.0f;

	{
		REAL32 cmx, cmy, resX, resY;

		// Size, in millimeters, of the physical screen.
		cmx = ((REAL32) GetDeviceCaps( m_hDrawDC, HORZSIZE) * 10.0f);
		cmy = ((REAL32) GetDeviceCaps( m_hDrawDC, VERTSIZE) * 10.0f);

		// Pixel Count, in pixels, of the screen
		resX = (REAL32) GetDeviceCaps( m_hDrawDC, HORZRES);
		resY = (REAL32) GetDeviceCaps( m_hDrawDC, VERTRES);

		m_factX =  cmx / resX;
		m_factY =  cmy / resY;
	}

	_SetCursor( GIC_CURSOR_SELECT);
	
	m_hGridPen = CreatePen( PS_SOLID, 1, RGB( 200, 200, 255));

	m_hCenterPen = CreatePen( PS_SOLID, 2, RGB( 100, 100, 255));

	m_hbrBack = g_pTDKGlobalRes->getBrush( I3TDK_COLOR_FORE_CAPTION_TEXT);///I3TDK_COLOR_FORE_APP_BKGND);

	i3TDK::RegisterUpdate( m_hWnd, i3GICShape::static_meta(), I3_TDK_UPDATE_ALL);
}

void i3GICDiagramView::_Close(void)
{
	if( m_hbmpDraw != nullptr)
	{
		::SelectObject( m_hDrawDC, m_hbmpOld);
		::DeleteObject( m_hbmpDraw);
		m_hbmpDraw = nullptr;
	}

	if( m_hDrawDC != nullptr)
	{
		#if !defined( DIRECT_DRAW)
			::DeleteDC( m_hDrawDC);
		#else
			::ReleaseDC( m_hWnd, m_hDrawDC);
		#endif

		m_hDrawDC = nullptr;
	}
}

void i3GICDiagramView::_SetCursor( GIC_MOUSE_CURSOR cursor)
{
	::SetCursor( m_hCursors[ cursor]);

	::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) m_hCursors[ cursor]);

	m_CurCursor = cursor;
}

void i3GICDiagramView::_AdjustView(void)
{
	//
	// 0.1 mm 단위의 좌표계를 사용하며,
	// Left->Right, Top->Bottom 축방향의 Space를 사용한다.
	//

	// Mapping Mode의 설정
	::SetMapMode( m_hDrawDC, MM_ANISOTROPIC);

	
	m_ViewWidth		= (m_factX * m_ClientWidth * m_Zoom);
	m_ViewHeight	= (m_factY * m_ClientHeight * m_Zoom);

	m_rOffsetX = m_ViewWidth * 0.5f;
	m_rOffsetY = m_ViewHeight * 0.5f;

	// 전체 Page에 대한 설정
	{	
		::SetWindowExtEx( m_hDrawDC, (INT32) m_ViewWidth, (INT32) m_ViewHeight, nullptr);
	}

	// Viewport의 설정
	{
		::SetViewportExtEx( m_hDrawDC, m_ClientWidth, m_ClientHeight, nullptr);
	}
}

void i3GICDiagramView::_SetViewSize( UINT32 cx, UINT32 cy)
{
	if( m_hDrawDC == nullptr)
		return;

	// 이미 생성된 Bitmap이 있다면 제거한다.
	if( m_hbmpDraw != nullptr)
	{
		::SelectObject( m_hDrawDC, m_hbmpOld);
		::DeleteObject( m_hbmpDraw);
	}

	m_hbmpDraw = CreateBitmap( cx, cy, 
					GetDeviceCaps( m_hDrawDC, PLANES), 
					GetDeviceCaps( m_hDrawDC, BITSPIXEL),
					nullptr);

	if( m_hbmpDraw != nullptr)
	{
		m_hbmpOld = (HBITMAP) ::SelectObject( m_hDrawDC, m_hbmpDraw);
	}

	m_ClientWidth = cx;
	m_ClientHeight = cy;

	_AdjustView();
}

void i3GICDiagramView::_DrawPage( void)
{
	RECT rt;

	I3ASSERT( m_pDiagramCtx != nullptr);

	rt.left = 0;
	rt.top = 0;
	rt.right = m_ClientWidth;
	rt.bottom = m_ClientHeight;

	_LP_SIZE( &rt);

	// 바닥을 지운다.
	::FillRect( m_hDrawDC, &rt, m_hbrBack);
}

void i3GICDiagramView::__DrawGrid( INT32 x, INT32 y)
{	
	m_pDiagramCtx->selectObject( m_hGridPen);
	m_pDiagramCtx->moveTo( (INT32)(-m_ViewWidth*0.5f), (INT32)(-m_ViewHeight*0.5f));

	INT32 i, j;

	I3ASSERT( x > 0);
	I3ASSERT( y > 0);
	
	INT32 countX = (INT32) i3Math::div( m_ViewWidth, (REAL32) x);
	INT32 countY = (INT32) i3Math::div( m_ViewHeight, (REAL32) y);

	RECT rect;

	REAL32 rReX = m_rViewPosX - i3Math::fmod( m_rViewPosX, (REAL32) x);
	REAL32 rReY = m_rViewPosY - i3Math::fmod( m_rViewPosY, (REAL32) y);

	for( i = 0; i < countX; i++)
	{
		rect.left = (INT32)(-m_ViewWidth*0.5f - rReX);
		rect.top = (INT32)(-m_ViewHeight*0.5f - rReY);
		rect.right = (INT32) (m_ViewWidth*0.5f - rReX);

		for( j = 0; j < countY; j++)
		{
			if( j < countY/2)
				rect.bottom = (INT32)(j * y - rReY);
			else
				rect.bottom = (INT32)((j-countY) * y - rReY);

			m_pDiagramCtx->moveTo( rect.left, rect.bottom );
			m_pDiagramCtx->lineTo( rect.right, rect.bottom );
		}

		if( i < countX/2)
			rect.right = (INT32)(i * x - rReX);
		else
			rect.right = (INT32)((i-countX)*x - rReX);

		rect.bottom = (INT32) (m_ViewHeight*0.5f - rReY);

		m_pDiagramCtx->moveTo( rect.right, rect.top);
		m_pDiagramCtx->lineTo( rect.right, rect.bottom );
	}

	// Center line
	m_pDiagramCtx->selectObject( m_hCenterPen);
	m_pDiagramCtx->moveTo( (INT32)(-m_ViewWidth*0.5f-rReX), 0);
	m_pDiagramCtx->lineTo( (INT32)(m_ViewWidth*0.5f-rReX), 0);

	m_pDiagramCtx->moveTo( 0, (INT32)(-m_ViewHeight*0.5f-rReY));
	m_pDiagramCtx->lineTo( 0, (INT32)(m_ViewHeight*0.5f-rReY));
}

void i3GICDiagramView::_DrawShapes( RECT * pRect)
{
	I3ASSERT( m_pDiagramCtx != nullptr);

	m_pDiagramCtx->resetOffstack( getViewPosX(), getViewPosY());

	// Page를 그린다.
	_DrawPage();

	// Grid를 그린다.
	if( m_pDiagramCtx->isGridEnable())
	{
		__DrawGrid( m_pDiagramCtx->getGridX(), m_pDiagramCtx->getGridY());
	}

	if( pRect == nullptr)
	{
		// 전체 화면에 대한 Update
		//m_pDiagramCtx->setUpdateRect( 0, 0, m_ClientWidth, m_ClientHeight);
		//m_pDiagramCtx->setUpdateRect( -getViewPosX(), -getViewPosY(), m_ClientWidth - getViewPosX(), m_ClientHeight - getViewPosY());
		m_pDiagramCtx->setUpdateRect( (INT32) -getViewPosX(), (INT32)-getViewPosY(), (INT32) (m_ViewWidth - getViewPosX()), (INT32)(m_ViewHeight - getViewPosY()));
	}
	else
	{
		m_pDiagramCtx->setUpdateRect( pRect);
	}

	// Shape들을 그린다.
	m_pDiagramCtx->DrawShapes( pRect);


}

void i3GICDiagramView::_DrawSelBox( RECT * pRect)
{
	INT32 temp;

	HDC dc = ::GetDC( m_hWnd);
	int OldROP2 = ::GetROP2( dc);

	if( pRect->left > pRect->right)
	{
		temp = pRect->left;
		pRect->left = pRect->right;
		pRect->right = temp;
	}

	if( pRect->top > pRect->bottom)
	{
		temp = pRect->top;
		pRect->top = pRect->bottom;
		pRect->bottom = temp;
	}

	::SetROP2( dc, R2_XORPEN);
	::DrawFocusRect( dc, pRect);

	::SetROP2( dc, OldROP2);
	::ReleaseDC( m_hWnd, dc);
}

/*virtual*/ void i3GICDiagramView::OnDraw( void)
{
	_DrawShapes( nullptr);
}

void i3GICDiagramView::SetMouseMode( GIC_MOUSEMODE mode)
{
	GIC_MOUSE_CURSOR newCursor = m_pDiagramCtx->SetMouseMode( mode);

	if( newCursor != GIC_CURSOR_NA)
		_SetCursor( newCursor);
}

/*virtual*/ bool i3GICDiagramView::OnMouseMoveForLink( i3GICShape * pShape)
{
	if( pShape == nullptr)
		return false;

	return true;
}


BEGIN_MESSAGE_MAP(i3GICDiagramView, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()



// i3GICDiagramView message handlers


int i3GICDiagramView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	_Init();

	return 0;
}

void i3GICDiagramView::OnDestroy()
{
	_Close();

	CWnd::OnDestroy();
}


void i3GICDiagramView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// Bitmap을 크기에 맞게 재조정...
	if((cx <= 0) || (cy <= 0))
		return;

	_SetViewSize( cx, cy);
}


void i3GICDiagramView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	OnDraw();

	#if !defined( DIRECT_DRAW)
	{
		POINT sz;

		sz.x = m_ClientWidth;
		sz.y = m_ClientHeight;

		_LP_SIZE( &sz);

		::StretchBlt( dc.m_hDC, 0, 0, m_ClientWidth, m_ClientHeight, m_hDrawDC, 0, 0, sz.x, sz.y, SRCCOPY);
	}
	#endif
}

BOOL i3GICDiagramView::OnEraseBkgnd(CDC* pDC)
{
	return 0;
}

void i3GICDiagramView::OnLButtonDown(UINT nFlags, CPoint point)
{
	::SetFocus( m_hWnd);

	if( m_pDiagramCtx != nullptr)
		m_pDiagramCtx->ProcessLButtonDown( nFlags, point);

	::SetCapture( m_hWnd);

	CWnd::OnLButtonDown(nFlags, point);
}

void i3GICDiagramView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_pDiagramCtx != nullptr)
		m_pDiagramCtx->ProcessLButtonUp( nFlags, point);

	::ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void i3GICDiagramView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnLButtonDblClk(nFlags, point);
}


void i3GICDiagramView::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnMButtonDblClk(nFlags, point);
}

void i3GICDiagramView::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	SetCapture();

	m_PrevMouseMode = m_pDiagramCtx->getMouseMode();

	m_pDiagramCtx->SetMouseMode( GIC_MOUSEMODE_SCROLL);

	_SetCursor( GIC_CURSOR_SCROLL);

	m_ScrollStart = point;

	CWnd::OnMButtonDown(nFlags, point);
}

void i3GICDiagramView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	m_pDiagramCtx->SetMouseMode( m_PrevMouseMode);

	_SetCursor( GIC_CURSOR_SELECT);

	ReleaseCapture();

	CWnd::OnMButtonUp(nFlags, point);
}

void i3GICDiagramView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnRButtonDblClk(nFlags, point);
}

void i3GICDiagramView::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	CWnd::OnRButtonDown(nFlags, point);
}

bool i3GICDiagramView::_CheckClipboardEmpty( const char * pszFormat)
{
	HANDLE hMem;
	UINT idFormat;

	bool bEmpty = false;

	if( ::OpenClipboard( nullptr) == FALSE)
		bEmpty = true;

	idFormat = ::RegisterClipboardFormat( pszFormat);
	if( idFormat == 0)
		bEmpty = true;

	hMem = ::GetClipboardData( idFormat);
	if( hMem == nullptr)
		bEmpty = true;

	return bEmpty;
}

/*virtual*/ void i3GICDiagramView::OnRButtonUpProcess( UINT nFlags, CPoint point)
{	
	CMenu menu;
	CPoint pt;
	UINT32 cmd;

	pt = point;

	_LP( &pt);

	REAL32 x = (REAL32)(pt.x );// * m_factX;
	REAL32 y = (REAL32)(pt.y );// * m_factY;
	m_pDiagramCtx->setMousePos( (REAL32)x, (REAL32)y);

	i3::vector<i3PersistantElement*> list;
	m_pDiagramCtx->getDiagram()->GetSelectedShapes( list);

	if( menu.CreatePopupMenu() )
	{
		menu.AppendMenu( MF_STRING, ID_DIAGRAM_SELECTALL, _T("&Select all..."));
		menu.AppendMenu( MF_STRING, ID_DIAGRAM_UNSELECTALL, _T("&Unselect all..."));
		menu.AppendMenu( MF_SEPARATOR, 0, _T(""));
		menu.AppendMenu( MF_STRING, ID_DIAGRAM_COPY, _T("&Copy"));
		menu.AppendMenu( MF_STRING, ID_DIAGRAM_PASTE, _T("&Paste"));
		menu.AppendMenu( MF_STRING, ID_DIAGRAM_DELETE, _T("&Delete"));

		if( _CheckClipboardEmpty( I3GIC_CLIPBOARDFORMAT) )
			menu.EnableMenuItem( ID_DIAGRAM_PASTE, MF_BYCOMMAND | MF_GRAYED);

		if( list.size() == 0)
		{
			menu.EnableMenuItem( ID_DIAGRAM_COPY, MF_BYCOMMAND | MF_GRAYED);
			menu.EnableMenuItem( ID_DIAGRAM_DELETE, MF_BYCOMMAND | MF_GRAYED);
		}

		if( isPopupFlag( I3GIC_POPUP_FLAG_NO_DEPTHALIGN) == false)
		{
			CMenu menuDepthAlign;

			menuDepthAlign.CreatePopupMenu();

			menuDepthAlign.AppendMenu( MF_STRING, ID_DEPTHPOSITION_TOBACK, _T("뒤로 보내기"));
			menuDepthAlign.AppendMenu( MF_STRING, ID_DEPTHPOSITION_TOTHETAIL, _T("맨 뒤로 보내기"));
			menuDepthAlign.AppendMenu( MF_STRING, ID_DEPTHPOSITION_TOFRONT, _T("&앞으로 보내기"));
			menuDepthAlign.AppendMenu( MF_STRING, ID_DEPTHPOSITION_TOTHEFOREMOST, _T("&맨 앞으로 보내기"));

			menu.AppendMenu( MF_POPUP, (UINT) menuDepthAlign.m_hMenu, _T("Depth Align"));

			if( list.size() != 1)
			{
				menu.EnableMenuItem( ID_DEPTHPOSITION_TOBACK, MF_BYCOMMAND | MF_GRAYED);
				menu.EnableMenuItem( ID_DEPTHPOSITION_TOTHETAIL, MF_BYCOMMAND | MF_GRAYED);
				menu.EnableMenuItem( ID_DEPTHPOSITION_TOFRONT, MF_BYCOMMAND | MF_GRAYED);
				menu.EnableMenuItem( ID_DEPTHPOSITION_TOTHEFOREMOST, MF_BYCOMMAND | MF_GRAYED);
			}
		}
		
		if( isPopupFlag( I3GIC_POPUP_FLAG_NO_ALIGN) == false)
		{
			CMenu menuAlign;

			menuAlign.CreatePopupMenu();
			menuAlign.AppendMenu( MF_STRING, ID_ALIGN_TOLEFT, _T("왼쪽"));
			menuAlign.AppendMenu( MF_STRING, ID_ALIGN_TOCENTER, _T("가운데"));
			menuAlign.AppendMenu( MF_STRING, ID_ALIGN_TORIGHT, _T("오른쪽"));
			menuAlign.AppendMenu( MF_STRING, ID_ALIGN_TOP, _T("위쪽"));
			menuAlign.AppendMenu( MF_STRING, ID_ALIGN_MIDDLE, _T("중앙"));
			menuAlign.AppendMenu( MF_STRING, ID_ALIGN_BOTTOM, _T("아래쪽"));

			if( list.size() == 0)
			{
				menuAlign.EnableMenuItem( ID_ALIGN_TOLEFT, MF_BYCOMMAND | MF_GRAYED);
				menuAlign.EnableMenuItem( ID_ALIGN_TOCENTER, MF_BYCOMMAND | MF_GRAYED);
				menuAlign.EnableMenuItem( ID_ALIGN_TORIGHT, MF_BYCOMMAND | MF_GRAYED);
				menuAlign.EnableMenuItem( ID_ALIGN_TOP, MF_BYCOMMAND | MF_GRAYED);
				menuAlign.EnableMenuItem( ID_ALIGN_MIDDLE, MF_BYCOMMAND | MF_GRAYED);
				menuAlign.EnableMenuItem( ID_ALIGN_BOTTOM, MF_BYCOMMAND | MF_GRAYED);
			}

			menu.AppendMenu( MF_POPUP, (UINT) menuAlign.m_hMenu, _T("Align"));
		}

		menu.AppendMenu( MF_SEPARATOR, 0, _T(""));

		OnQueryPopupMenu( &menu);

		GetCursorPos( &pt);

		cmd = menu.TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
					pt.x, pt.y, (CWnd *) this, nullptr);

		if( cmd != 0)
		{	
			m_pDiagramCtx->ProcessPopupMenu( cmd);

			OnSelectPopupMenu( cmd);
		}
	}
}

void i3GICDiagramView::OnRButtonUp(UINT nFlags, CPoint point)
{
	OnRButtonUpProcess( nFlags, point);

	CWnd::OnRButtonUp(nFlags, point);
}


void i3GICDiagramView::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_pDiagramCtx == nullptr)
	{
		CWnd::OnMouseMove(nFlags, point);
		return;
	}

	if( m_pDiagramCtx->getMouseMode() == GIC_MOUSEMODE_SCROLL)
	{
		POINT pt;

		pt.x = point.x - m_ScrollStart.x;
		pt.y = point.y - m_ScrollStart.y;

		_LP_SIZE( &pt);

		m_rViewPosX += (REAL32) pt.x;
		m_rViewPosY += (REAL32) pt.y;

		m_ScrollStart = point;

		Invalidate();
	}
	else
	{
		GIC_MOUSE_CURSOR newCursor = m_pDiagramCtx->ProcessMouseMove( nFlags, point);

		if( newCursor != GIC_CURSOR_NA)
			_SetCursor( newCursor);
	}

	CWnd::OnMouseMove(nFlags, point);
}

BOOL i3GICDiagramView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//if( nFlags & MK_CONTROL)
	{
		// Zoom In / Out
		if( zDelta > 0)
		{
			m_Zoom += m_Zoom * 0.25f;
		}
		else
		{
			m_Zoom -= m_Zoom * 0.25f;
		}

		_AdjustView();
		Invalidate();
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

#define SCROLL_STEP			5.0f


void i3GICDiagramView::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
	{
		if( i3::kind_of<i3GICShape*>(pInfo->m_pMeta))
		{
			Invalidate();
		}
	}

	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( i3::kind_of<i3GICShape* >(pInfo->m_pMeta))
		{
			Invalidate();
		}
	}
	
	if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
	{
		if( i3::kind_of<i3GICShape*>(pInfo->m_pMeta))
		{
			Invalidate();
		}
	}

	if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		if( i3::kind_of<i3GICShape*>(pInfo->m_pMeta))
		{
			Invalidate();
		}
	}

	if( pInfo->m_Event & I3_TDK_UPDATE_REQUEST)
	{
		
	}
}


LRESULT i3GICDiagramView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( m_pDiagramCtx != nullptr)
		m_pDiagramCtx->ObserveShapeState();

	switch( message)
	{
	case WM_TDK_UPDATE :
		{
			OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
		}
		break;
	case WM_KEYDOWN :
		if( m_pKeyboard != nullptr)
		{
			m_pKeyboard->OnKeyDown( wParam);

			if( m_pDiagramCtx != nullptr)
			{
				if( m_pKeyboard->GetPressState( I3I_KEY_LCTRL))
				{
					if( wParam == 'C')
						m_pDiagramCtx->CopyToClipboard();
					else if(wParam == 'V')
						m_pDiagramCtx->PasteFromClipboard();
				}

				if(wParam == VK_DELETE )
					m_pDiagramCtx->DeleteShape();
			}
		}

		if( m_pDiagramCtx != nullptr)
			m_pDiagramCtx->ProcessKeyDown( wParam, 0, 0);

		break;
	case WM_KEYUP :
	case WM_SYSKEYUP:
		if( m_pKeyboard != nullptr)
		{
			m_pKeyboard->OnKeyUp( wParam);
		}
		break;
	}

	return CWnd::WindowProc(message, wParam, lParam);
}


void i3GICDiagramView::_ResetKeyinput( void)
{
	if( m_pKeyboard != nullptr)
	{
		m_pKeyboard->Reset();
	}
}

void i3GICDiagramView::SetMetaToCreate( i3ClassMeta * pClass)
{
	m_pDiagramCtx->setMetaToCreate(pClass);
	m_pMetaToCreate = pClass;
}


#endif	// I3_DEBUG
