#include "StdAfx.h"
#include "UVEditCtrl.h"
#include "GlobalVariable.h"
#include ".\uveditctrl.h"

IMPLEMENT_DYNCREATE( CUVEditCtrl, i3TDKImageViewCtrl );

enum UVEDIT_MOUSEOVER_EDGE
{
	EDGE_INDEX_LEFT = 0,
	EDGE_INDEX_TOP,
	EDGE_INDEX_RIGHT,
	EDGE_INDEX_BOTTOM,
	EDGE_INDEX_MAX,
};

CUVEditCtrl::CUVEditCtrl(void)
{
	m_bEditUVEnable = FALSE;
	m_pEditUVImage	= NULL;
	
	m_posOldLBDrag	= CPoint(0,0);
	m_bLBDragging	= FALSE;

	m_LastMouseOverEdge = EDGE_INDEX_MAX;

	m_nPenColor		= WHITE_PEN;
	_DrawText[0] = 0;
}

CUVEditCtrl::~CUVEditCtrl(void)
{
	I3_SAFE_RELEASE( m_pEditUVImage );
}

BEGIN_MESSAGE_MAP( CUVEditCtrl, i3TDKImageViewCtrl )
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CUVEditCtrl::SetImageForEditUV( i3GuiImage * pImage )
{
	if( pImage != NULL )
		pImage->AddRef();

	I3_SAFE_RELEASE( m_pEditUVImage );

	m_pEditUVImage = pImage;

	m_FocusRect.SetRect( m_pEditUVImage->m_u, m_pEditUVImage->m_v,
						 m_pEditUVImage->m_u + m_pEditUVImage->m_w,
						 m_pEditUVImage->m_v + m_pEditUVImage->m_h);

	i3TDKImageViewCtrl::SetImage( m_pEditUVImage->m_pTexture );

	SetDrawText( pImage);
}

BOOL CUVEditCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if( m_bEditUVEnable == FALSE )	return FALSE;

	return i3TDKImageViewCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CUVEditCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_bLBDragging)
	{
		CRect rt = m_FocusRect;

		CPoint delta;
		CPoint totalDelta = point - m_posDragStart;

		if( m_LastMouseOverEdge < EDGE_INDEX_MAX )
		{
			delta.x = (LONG)(( totalDelta.x / m_Zoom) + m_rtDragStart.right) - m_FocusRect.right;
			delta.y = (LONG)(( totalDelta.y / m_Zoom) + m_rtDragStart.bottom) - m_FocusRect.bottom;

			_ChangeSize( m_LastMouseOverEdge, delta);
		}
		else
		{
			delta.x = (LONG)(( totalDelta.x / m_Zoom) + m_rtDragStart.left) - m_FocusRect.left;
			delta.y = (LONG)(( totalDelta.y / m_Zoom) + m_rtDragStart.top) - m_FocusRect.top;

            _MoveUV( delta );
		}

		m_posOldLBDrag = point;

		rt = _FindMinRange(&rt, &m_FocusRect);
		rt.top -= 30;
		rt.right += 200;
		rt.bottom += 60;

		InvalidateRect( &rt);
	}
	else
	{
		if(( m_pEditUVImage != NULL) && ( m_bEditUVEnable != FALSE))
		{
			_IsMouseEdgeOver( point );
		}
	}

	i3TDKImageViewCtrl::OnMouseMove(nFlags, point);
}

void CUVEditCtrl::OnDraw(HDC dc)
{
	i3TDKImageViewCtrl::OnDraw( dc );

	if(( m_pEditUVImage != NULL) && ( m_bEditUVEnable != FALSE))
	{
		::SelectObject( dc, GetStockObject( NULL_BRUSH));
		//::SelectObject( dc, GetStockObject( m_nPenColor));		

		RECT rt = _FindMinRange( &m_FocusRect, &m_FocusRect);

		HPEN hPen, hOldPen;
		hPen = CreatePen( PS_SOLID, 1, RGB( 255,0,255));
		hOldPen = (HPEN)::SelectObject( dc, hPen);

		::Rectangle( dc, rt.left, rt.top, rt.right, rt.bottom);
		::SelectObject( dc, hOldPen);

		//SetBkMode( dc, TRANSPARENT);

		rt.top -= 18;	
		if( rt.top <= 0)
		{
			rt.top = rt.bottom;
			rt.bottom += 32;
		}

		rt.right += 300;
		DrawText( dc, _DrawText, strlen(_DrawText), &rt, 0);
	}
}

void CUVEditCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();

	if(( m_pEditUVImage != NULL) && ( m_bEditUVEnable != FALSE))
	{
		_IsMouseEdgeOver(point);

		RECT rt = _FindMinRange(&m_FocusRect, &m_FocusRect);

		if( (m_LastMouseOverEdge < EDGE_INDEX_MAX ) || ::PtInRect( &rt, point ))
		{
			m_posOldLBDrag = point;

			m_bLBDragging = TRUE;

			m_posDragStart = point;

			SetCapture();

			m_rtDragStart.SetRect( m_FocusRect.left, m_FocusRect.top, m_FocusRect.right, m_FocusRect.bottom);

			if( m_LastMouseOverEdge >= EDGE_INDEX_MAX)
			{
				SetCursor( g_pTDKGlobalRes->m_hCursorHand );
			}
		}
	}

	i3TDKImageViewCtrl::OnLButtonDown(nFlags, point);
}

void CUVEditCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	{
		if( m_bLBDragging )
		{
			//마우스 좌클릭 드래깅으로는 포지션 값만 변합니다.
			m_pEditUVImage->m_u = m_FocusRect.left;
			m_pEditUVImage->m_v = m_FocusRect.top;
			m_bLBDragging = FALSE;
			ReleaseCapture();
		}
	}

	i3TDKImageViewCtrl::OnLButtonUp(nFlags, point);
}

void CUVEditCtrl::SetDrawText( i3GuiImage * pImage)
{
	sprintf( _DrawText, "pos: %d, %d size: %d, %d", pImage->m_u, pImage->m_v, pImage->m_w, pImage->m_h);
}

void CUVEditCtrl::_SetValidRect(i3GuiImage * pImage)
{
	if( pImage != NULL )
	{
		i3Texture * pTex = pImage->m_pTexture;

		if( pImage->m_u <= 0 )
		{
			pImage->m_u = 0;
		}
		else if( pImage->m_u >= (INT32)pTex->GetWidth())
		{
			pImage->m_u = pTex->GetWidth() - 1;
		}

		if( pImage->m_v <= 0 )
		{
			pImage->m_v = 0;
		}
		else if( pImage->m_v >= (INT32)pTex->GetHeight())
		{
			pImage->m_v = pTex->GetHeight() - 1;
		}

		if( pImage->m_w + pImage->m_u > (INT32)pTex->GetWidth())
		{
			pImage->m_w = pTex->GetWidth() - pImage->m_u;
		}

		if( pImage->m_h + pImage->m_v > (INT32)pTex->GetHeight())
		{
			pImage->m_h = pTex->GetHeight() - pImage->m_v;
		}

		m_FocusRect.left	= pImage->m_u;
		m_FocusRect.top		= pImage->m_v;
		m_FocusRect.right	= pImage->m_u + pImage->m_w;
		m_FocusRect.bottom	= pImage->m_v + pImage->m_h;

		SetDrawText( pImage);
	}
}

void CUVEditCtrl::_MoveUV(CPoint delta)
{
	if(( m_pEditUVImage == NULL) || ( m_bEditUVEnable == FALSE))	return ;

	m_FocusRect.left += delta.x;
	m_FocusRect.top += delta.y;

	m_FocusRect.right += delta.x;
	m_FocusRect.bottom += delta.y;

	m_pEditUVImage->m_u = m_FocusRect.left;
	m_pEditUVImage->m_v = m_FocusRect.top;

	_SetValidRect(m_pEditUVImage);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_UVEDIT, m_pEditUVImage);
}

void CUVEditCtrl::_ChangeSize(INT32	nEdge, CPoint	nDelta)
{
	if(( m_pEditUVImage == NULL) || ( m_bEditUVEnable == FALSE))	return ;

	if( nEdge == EDGE_INDEX_LEFT )
	{
		m_FocusRect.left += nDelta.x;
		m_pEditUVImage->m_u = m_FocusRect.left;
		m_pEditUVImage->m_w = abs(m_FocusRect.right - m_FocusRect.left);
	}
	else if( nEdge == EDGE_INDEX_RIGHT )
	{
		m_FocusRect.right += nDelta.x;
		m_pEditUVImage->m_w = abs(m_FocusRect.right - m_FocusRect.left);
	}
    else if( nEdge == EDGE_INDEX_TOP )
	{
		m_FocusRect.top += nDelta.y;
		m_pEditUVImage->m_v = m_FocusRect.top;
		m_pEditUVImage->m_h = abs(m_FocusRect.bottom - m_FocusRect.top);
	}
	else if( nEdge == EDGE_INDEX_BOTTOM )
	{
		m_FocusRect.bottom += nDelta.y;
		m_pEditUVImage->m_h = abs(m_FocusRect.bottom - m_FocusRect.top);
	}
	else
	{
	 	return ;
	}

	_SetValidRect(m_pEditUVImage);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT | I3_TDK_USRWND_UVEDIT, m_pEditUVImage);
}

INT32 CUVEditCtrl::_IsMouseEdgeOver(CPoint point)
{
	if(( m_pEditUVImage == NULL) || ( m_bEditUVEnable == FALSE))	return EDGE_INDEX_MAX;

	INT32	nEdge = EDGE_INDEX_MAX;
	RECT rt = _FindMinRange(&m_FocusRect, &m_FocusRect);

	if( (rt.left <= point.x) && (rt.right >= point.x))
	{
		if( (point.y >= rt.top - 2) && (point.y <= rt.top + 2))		nEdge = EDGE_INDEX_TOP;
		else if(( point.y >= rt.bottom - 2) && (point.y <= rt.bottom +2))	nEdge = EDGE_INDEX_BOTTOM;
	}

	if( (rt.top <= point.y) && (rt.bottom >= point.y))
	{
		if( (point.x >= rt.left - 2) && (point.x <= rt.left + 2) )		nEdge = EDGE_INDEX_LEFT;
		else if( (point.x >= rt.right - 2) && (point.x <= rt.right +2))	nEdge = EDGE_INDEX_RIGHT;
	}

	if(( nEdge == EDGE_INDEX_LEFT) || ( nEdge == EDGE_INDEX_RIGHT))
	{
		SetCursor( g_pTDKGlobalRes->m_hCursorSizeWE  );
	}
	else if((nEdge == EDGE_INDEX_TOP) || (nEdge == EDGE_INDEX_BOTTOM))
	{
		SetCursor( g_pTDKGlobalRes->m_hCursorSizeNS);
	}
	else
		SetCursor( g_pTDKGlobalRes->m_hCursorArrow );

	m_LastMouseOverEdge = nEdge;
	return nEdge;
}

BOOL CUVEditCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return 1;
	//return i3TDKImageViewCtrl::OnEraseBkgnd(pDC);
}

void CUVEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if( m_bEditUVEnable == FALSE ) return ;

	CPoint pt( 0,0);
	RECT	rt = m_FocusRect;

	INT32	nEdge = EDGE_INDEX_LEFT;

	switch( nChar )
	{
		case VK_LEFT:
			{
				pt.x = -1;
				nEdge = EDGE_INDEX_RIGHT;
			}
			break;
			
		case VK_RIGHT:
			{
				pt.x = 1;
				nEdge = EDGE_INDEX_RIGHT;
			}
			break;


		case VK_DOWN:
			{
				pt.y = 1;
				nEdge = EDGE_INDEX_BOTTOM;
			}
			break;
			
		case VK_UP:		
			{
				pt.y = -1;
				nEdge = EDGE_INDEX_BOTTOM;
			}
			break;


		case VK_ESCAPE:
			{
				if( m_bLBDragging )
				{
					m_bLBDragging = FALSE;
					ReleaseCapture();

					m_FocusRect.SetRect( m_pEditUVImage->m_u, m_pEditUVImage->m_v,
					m_pEditUVImage->m_u + m_pEditUVImage->m_w,
					m_pEditUVImage->m_v + m_pEditUVImage->m_h);

					Invalidate();
					return;
				}
			}
			break;
		default:
			return;
	}

	if( GetKeyState( VK_LSHIFT ) & 0x80)
	{
		_ChangeSize( nEdge, pt); 
	}
	else
	{
		_MoveUV( pt);
	}

	{
		rt = _FindMinRange( &rt, &m_FocusRect);
		rt.top -= 30;
		rt.right += 200;
		rt.bottom += 60;
		InvalidateRect( &rt);
	}

	i3TDKImageViewCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

RECT CUVEditCtrl::_FindMinRange( RECT * pRect1, RECT * pRect2)
{
	RECT rt;
	//Find InvalidateRect
	if( pRect1 != pRect2)
	{
		rt.right	= (LONG)max( pRect1->right * m_Zoom,	pRect2->right * m_Zoom);
		rt.bottom	= (LONG)max( pRect1->bottom * m_Zoom,	pRect2->bottom * m_Zoom);
		rt.left		= (LONG)min( pRect1->left * m_Zoom,		pRect2->left * m_Zoom);
		rt.top		= (LONG)min( pRect1->top * m_Zoom,		pRect2->top * m_Zoom);
	}
	else
	{
		rt.left		= (LONG)(pRect1->left * m_Zoom);
		rt.top		= (LONG)(pRect1->top * m_Zoom);
		rt.right	= (LONG)(pRect1->right * m_Zoom);
		rt.bottom	= (LONG)(pRect1->bottom * m_Zoom);
	}

	rt.left -= m_posScroll.x;
	rt.top -= m_posScroll.y;

	rt.right -= m_posScroll.x;
	rt.bottom -= m_posScroll.y;

	return rt;
}
