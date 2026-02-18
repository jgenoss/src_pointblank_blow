// ColorKeyCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "ColorKeyCtrl.h"
#include ".\colorkeyctrl.h"


// CColorKeyCtrl

IMPLEMENT_DYNAMIC(CColorKeyCtrl, CWnd)
CColorKeyCtrl::CColorKeyCtrl()
{
	m_pRTable = NULL;
	m_pGTable = NULL;
	m_pBTable = NULL;
	m_pATable = NULL;

	m_MemDC = NULL;
	m_hBitmap = NULL;

	m_bDragging = false;
}

CColorKeyCtrl::~CColorKeyCtrl()
{
	I3_SAFE_RELEASE( m_pRTable);
	I3_SAFE_RELEASE( m_pGTable);
	I3_SAFE_RELEASE( m_pBTable);
	I3_SAFE_RELEASE( m_pATable);

	if( m_hBitmap != NULL)
	{
		SelectObject( m_MemDC, m_hOldBitmap);
		DeleteObject( m_hBitmap);
		m_hBitmap = NULL;
	}

	if( m_MemDC != NULL)
	{
		DeleteDC( m_MemDC);
		m_MemDC = NULL;
	}
}

BOOL CColorKeyCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR hCursor;
	HBRUSH hBack;

	hCursor = LoadCursor( NULL, IDC_ARROW);

	hBack = (HBRUSH) GetSysColorBrush( COLOR_BTNFACE);

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, hCursor, hBack, NULL);

	return CWnd::Create( pszClass, "", dwStyle, rect, pParentWnd, nID);
}

void CColorKeyCtrl::SetColorKeyframes( i3KeyframeTable * pRTable, i3KeyframeTable * pGTable, i3KeyframeTable * pBTable, i3KeyframeTable * pATable)
{
	if( pRTable != NULL)
		pRTable->AddRef();

	if( pGTable != NULL)
		pGTable->AddRef();

	if( pBTable != NULL)
		pBTable->AddRef();

	if( pATable != NULL)
		pATable->AddRef();

	I3_SAFE_RELEASE( m_pRTable);
	I3_SAFE_RELEASE( m_pGTable);
	I3_SAFE_RELEASE( m_pBTable);
	I3_SAFE_RELEASE( m_pATable);

	m_pRTable = pRTable;
	m_pGTable = pGTable;
	m_pBTable = pBTable;
	m_pATable = pATable;

	Update();
	Invalidate();
}

#define		HH		6

inline void _drawGrip( HDC dc, INT32 x, INT32 y, bool bUp)
{
	POINT pts[4];

	pts[0].x = x - 4;		pts[0].y = y;
	pts[1].x = x;
	pts[2].x = x + 4;		pts[2].y = y;
	pts[3].x = x - 4;		pts[3].y = y;

	if( bUp)
		pts[1].y = y - 4;
	else
		pts[1].y = y + 4;
	
	Polygon( dc, pts, 4);
}

void CColorKeyCtrl::Update(void)
{
	HBRUSH hBrush, hNullBrush, hOldBrush;
	HPEN hPen, hOldPen;
	INT32 i, j;

	hBrush = (HBRUSH) GetSysColorBrush( COLOR_3DHILIGHT);
	hNullBrush = (HBRUSH) GetStockObject( NULL_BRUSH);
	hPen = (HPEN) GetStockObject( BLACK_PEN);

	hOldBrush = (HBRUSH) SelectObject( m_MemDC, hNullBrush);
	hOldPen = (HPEN) SelectObject( m_MemDC, hPen);

	// 배경을 지움
	::FillRect( m_MemDC, &m_ClientRect, hBrush);

	// 테두리를 그림
	::Rectangle( m_MemDC, 0, 0, m_ClientRect.Width(), m_ClientRect.Height());

	// 상한선을 그림
	::MoveToEx( m_MemDC, 0, HH, NULL);
	::LineTo( m_MemDC, m_ClientRect.Width(), HH);

	// Gradation Image
	{
		UINT32 * pDest, Pitch;
		REAL32 t, rt, r, g, b, a, back, ta;

		Pitch = i3Gfx::CalcRasterBufferSize( m_ClientRect.Width(), 1, 4, I3G_IMAGE_FORMAT_RGBA_8888);

		rt = 1.0f / (m_ClientRect.Width() - 2);

		// 한 줄을 만든다.
		for( j = 0; j < m_ClientRect.Height() - 12; j++)
		{
			pDest = (UINT32 *)((UINT8 *)m_pPixel + (Pitch * (j + HH)) + (1 * 4));

			for( i = 0; i < m_ClientRect.Width() - 2; i++)
			{
				t = i * rt;

				back = ((((i >> 3) ^ (j >> 3))) & 0x01) ? 92.0f : 140.0f;

				// R
				if( m_pRTable != NULL)
					r = m_pRTable->GetKeyframe( t, i3KeyframeTable::LERP);
				else
					r = 255.0f;

				// G
				if( m_pGTable != NULL)
					g = m_pGTable->GetKeyframe( t, i3KeyframeTable::LERP);
				else
					g = 255.0f;

				// B
				if( m_pBTable != NULL)
					b = m_pBTable->GetKeyframe( t, i3KeyframeTable::LERP);
				else
					b = 255.0f;

				// A
				if( m_pATable != NULL)
					a = m_pATable->GetKeyframe( t, i3KeyframeTable::LERP);
				else
					a = 255.0f;

				ta = a * 0.003921568627450980392156862745098f;			// / 255.0f

				// G:0 R:8 A:16 B:24
				*pDest =	(((UINT32)((r * ta) + (back * (1.0f - ta))) & 0xFF) << 16) |
							(((UINT32)((g * ta) + (back * (1.0f - ta))) & 0xFF) << 8) |
							(((UINT32)((b * ta) + (back * (1.0f - ta))) & 0xFF) << 0);

				pDest++;
			}
		}
	}

	// Keyframe을 그림.
	// RGB Table은 어느 하나만 NULL일 수 없다.
	// 그러나 A는 개별적으로 설정될 수 있다.
	if( m_pRTable != NULL)
	{
		// RGB Keyframs
		// Red Keyframe table을 기준으로 그린다.
		// green과 Blue는 항상 Red와 같은 개수의 Keyframe을 가진다고 가정한다.
		INT32 x;

		hBrush = GetSysColorBrush( COLOR_3DDKSHADOW);
		::SelectObject( m_MemDC, hBrush);

		for( i = 0; i < m_pRTable->GetCount(); i++)
		{
			x = (INT32)(m_pRTable->GetTime( i) * m_ClientRect.Width());

			_drawGrip( m_MemDC, x, 1, false);

			if( m_bDragging)
			{
				if((m_bAlphaDrag == FALSE) && (m_DragIndex == i))
				{
					CRect rc;

					rc.SetRect( x - 1, HH, x + 1, m_ClientRect.Height() - HH);

					DrawFocusRect( m_MemDC, &rc);
				}
			}
		}
	}

	if( m_pATable != NULL)
	{
		// Alpha Keyframs
		INT32 x;

		hBrush = GetSysColorBrush( COLOR_3DDKSHADOW);
		::SelectObject( m_MemDC, hBrush);

		for( i = 0; i < m_pATable->GetCount(); i++)
		{
			x = (INT32)(m_pATable->GetTime( i) * m_ClientRect.Width());

			_drawGrip( m_MemDC, x, m_ClientRect.Height() - 1, true);

			if( m_bDragging)
			{
				if((m_bAlphaDrag == TRUE) && (m_DragIndex == i))
				{
					CRect rc;

					rc.SetRect( x - 1, HH, x + 1, m_ClientRect.Height() - HH);

					DrawFocusRect( m_MemDC, &rc);
				}
			}
		}
	}
	
	::SelectObject( m_MemDC, hOldBrush);
	::SelectObject( m_MemDC, hOldPen);
}

BEGIN_MESSAGE_MAP(CColorKeyCtrl, CWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CColorKeyCtrl message handlers

void CColorKeyCtrl::OnPaint()
{
	CPaintDC dc(this);
	RECT * pRc;
	INT32 x, y, w, h;

	pRc = &dc.m_ps.rcPaint;

	x = pRc->left;
	y = pRc->top;
	w = pRc->right - pRc->left;
	h = pRc->bottom - pRc->top;

	BitBlt( dc.m_hDC, x, y, w, h, m_MemDC, x, y, SRCCOPY);
}


void CColorKeyCtrl::OnSize(UINT nType, int cx, int cy)
{
	bool bFirst = false;
	BITMAPINFO bmi;
	BITMAPINFOHEADER * pHdr;

	CWnd::OnSize(nType, cx, cy);

	m_ClientRect.SetRect( 0, 0, cx, cy);

	if((cx <= 0) && (cy <= 0))
		return;

	if( m_MemDC == NULL)
	{
		HDC dc = ::GetDC( m_hWnd);

		m_MemDC = CreateCompatibleDC( dc);
		I3ASSERT( m_MemDC);

		bFirst = true;

		::ReleaseDC( m_hWnd, dc);
	}

	if( m_hBitmap != NULL)
	{
		SelectObject( m_MemDC, m_hOldBitmap);
		DeleteObject( m_hBitmap);
	}

	{
		memset( &bmi, 0, sizeof(bmi));
		pHdr = & bmi.bmiHeader;

		pHdr->biSize = sizeof(BITMAPINFOHEADER);
		pHdr->biWidth = cx;
		pHdr->biHeight = cy;
		pHdr->biPlanes = 1;
		pHdr->biBitCount = 32;
		pHdr->biCompression = BI_RGB;
		pHdr->biSizeImage = 0;
	}

	m_hBitmap = CreateDIBSection( m_MemDC, &bmi, DIB_RGB_COLORS, (void **) &m_pPixel, NULL, 0);
	I3ASSERT( m_hBitmap != NULL);

	if( bFirst)
	{
		m_hOldBitmap = (HBITMAP) SelectObject( m_MemDC, m_hBitmap);
	}
	else
	{
		SelectObject( m_MemDC, m_hBitmap);
	}

	Update();
}

INT32 CColorKeyCtrl::FindKeyframeByPos( POINT pt, BOOL * pUpSide)
{
	INT32 x, i;
	i3KeyframeTable * pTbl = NULL;

	if((pt.y >= 0) && (pt.y <= HH))
	{ 
		// RGB Table 중의 Keyframe 을 선택하려 한다.
		pTbl = m_pRTable;
		*pUpSide = TRUE;
	}
	else if( (pt.y <= m_ClientRect.Height()) && (pt.y >= (m_ClientRect.Height() - HH)))
	{
		// Alpha Keyframe을 선택하려 한다.
		pTbl = m_pATable;
		*pUpSide = FALSE;
	}

	if( pTbl == NULL)
		return -1;

	for( i = 0; i < pTbl->GetCount(); i++)
	{
		x = (INT32) ((m_ClientRect.Width() - 2) * pTbl->GetTime( i));

		if( abs( x - pt.x) <= 4)
		{
			return i;
		}
	}

	return -1;
}

void CColorKeyCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	INT32 idx;
	BOOL bUpside;
	REAL32 val;

	idx = FindKeyframeByPos( point, &bUpside);

	if( idx == -1)
	{
		REAL32 t = point.x / (REAL32)(m_ClientRect.Width() - 2);

		if( t < 0.0f)	return;
		if( t > 1.0f)	return;

		if( bUpside)
		{
			if( m_pRTable != NULL)
			{
				val = m_pRTable->GetKeyframe( t, i3KeyframeTable::LERP);
				m_pRTable->InsertKeyframe( t, val);

				val = m_pGTable->GetKeyframe( t, i3KeyframeTable::LERP);
				m_pGTable->InsertKeyframe( t, val);

				val = m_pBTable->GetKeyframe( t, i3KeyframeTable::LERP);
				m_pBTable->InsertKeyframe( t, val);
			}
		}
		else
		{
			if( m_pATable != NULL)
			{
				val = m_pATable->GetKeyframe( t, i3KeyframeTable::LERP);
				m_pATable->InsertKeyframe( t, val);
			}
		}

		Update();
		Invalidate();
	}
	else
	{
		// 기존 Keyframe의 Time을 조절하려 한다.
		if( bUpside)
		{
			// 가장 선두와 가장 마지막 Keyframe은 시간 조절할 수 없다.
			if( (idx <= 0) || (idx >= m_pRTable->GetCount() - 1))
				return;
		}
		else
		{
			if( (idx <= 0) || (idx >= m_pATable->GetCount() - 1))
				return;
		}

		::SetCapture( m_hWnd);

		m_bDragging = TRUE;
		m_bAlphaDrag = !bUpside;
		m_DragStartPt = point;
		m_DragIndex = idx;

		Update();

		InvalidateRect( CRect( point.x - 4, 0, point.x + 4, m_ClientRect.Height()));
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CColorKeyCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging == TRUE)
	{
		::ReleaseCapture();
		m_bDragging = FALSE;

		if((point.x < 0) || (point.x > m_ClientRect.right))
		{
			// 바깥까지 Draggin을 한 경우, 해당 Keyframe을 제거하는 것이라 생각한다.
			
			if( m_bAlphaDrag == FALSE)
			{
				m_pRTable->RemoveKeyframe( m_DragIndex);
				m_pGTable->RemoveKeyframe( m_DragIndex);
				m_pBTable->RemoveKeyframe( m_DragIndex);
			}
			else
			{
				m_pATable->RemoveKeyframe( m_DragIndex);
			}
		}

		Update();
		Invalidate();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CColorKeyCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging == FALSE)
	{
		CWnd::OnMouseMove(nFlags, point);
		return;
	}

	REAL32 t;

	t = point.x / (REAL32)(m_ClientRect.Width() - 2);

	if( m_bAlphaDrag == FALSE)
	{
		// t의 범위는 일정한 t-1과 t+1 사이의 값이어야 한다.
		{
			REAL32 lmin, lmax;

			if( m_DragIndex > 0)
				lmin = m_pRTable->GetTime( m_DragIndex-1);
			else
				lmin = 0.0f;

			if( m_DragIndex < m_pRTable->GetCount() - 1)
				lmax = m_pRTable->GetTime( m_DragIndex+1);
			else
				lmax = 1.0f;

			t = MAX( lmin, t);
			t = MIN( lmax, t);
		}


		m_pRTable->SetTime( m_DragIndex, t);
		m_pGTable->SetTime( m_DragIndex, t);
		m_pBTable->SetTime( m_DragIndex, t);
	}
	else
	{
		// t의 범위는 일정한 t-1과 t+1 사이의 값이어야 한다.
		{
			REAL32 lmin, lmax;

			if( m_DragIndex > 0)
				lmin = m_pATable->GetTime( m_DragIndex-1);
			else
				lmin = 0.0f;

			if( m_DragIndex < m_pATable->GetCount() - 1)
				lmax = m_pATable->GetTime( m_DragIndex+1);
			else
				lmax = 1.0f;

			t = MAX( lmin, t);
			t = MIN( lmax, t);
		}

		m_pATable->SetTime( m_DragIndex, t);
	}

	Update();

	Invalidate();
}

BOOL CColorKeyCtrl::OnEraseBkgnd(CDC* pDC)
{
	return 0;
}

void CColorKeyCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	INT32 idx;
	BOOL bUpside;
	COLORREF col;

	idx = FindKeyframeByPos( point, &bUpside);
	if( idx == -1)
		return;

	if( bUpside)
	{
		UINT8 r, g, b;

		r = (UINT8) m_pRTable->GetKeyframe( idx);
		g = (UINT8) m_pGTable->GetKeyframe( idx);
		b = (UINT8) m_pBTable->GetKeyframe( idx);

		col = RGB( r, g, b);
	}
	else
	{
		UINT8 a;

		a = (UINT8) m_pATable->GetKeyframe( idx);

		col = RGB( a, a, a);
	}

	{
		CColorDialog dlg( col, CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT, this);

		if( dlg.DoModal() == IDCANCEL)
			return;

		col = dlg.GetColor();
	}

	if( bUpside)
	{
		REAL32 val;

		val = (REAL32) GetRValue( col);
		m_pRTable->SetKeyframeByIndex( idx, val);

		val = (REAL32) GetGValue( col);
		m_pGTable->SetKeyframeByIndex( idx, val);

		val = (REAL32) GetBValue( col);
		m_pBTable->SetKeyframeByIndex( idx, val);
	}
	else
	{
		REAL32 val;

		val = (REAL32) (GetRValue( col) + GetGValue(col) + GetBValue(col));
		val *= 0.3333333333f;

		m_pATable->SetKeyframeByIndex( idx, val);
	}

	Update();
	Invalidate();


	CWnd::OnLButtonDblClk(nFlags, point);
}
