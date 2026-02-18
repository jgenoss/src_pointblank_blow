// GraphViewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "GraphViewCtrl.h"
#include ".\graphviewctrl.h"


// CGraphViewCtrl

IMPLEMENT_DYNAMIC(CGraphViewCtrl, CWnd)
CGraphViewCtrl::CGraphViewCtrl()
{
	m_hBitmap = NULL;
	m_DC = NULL;
	m_hFont = NULL;

	m_Duration = 0.0f;
}

CGraphViewCtrl::~CGraphViewCtrl()
{
	if( m_hFont != NULL)
	{
		SelectObject( m_DC, m_hOldFont);
		DeleteObject( m_hFont);
		m_hFont = NULL;
	}

	if( m_hBitmap != NULL)
	{
		SelectObject( m_DC, m_hOldBitmap);
		DeleteObject( m_hBitmap);
		m_hBitmap = NULL;
	}

	if( m_DC != NULL)
	{
		DeleteDC( m_DC);
		m_DC = NULL;
	}
}

BOOL CGraphViewCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR lpszClassName;
	HCURSOR hCursor;
	HBRUSH hbrBack;

	{
		HDC dc;

		dc = ::GetDC( NULL);

		m_DC = CreateCompatibleDC( dc);

		SetBkMode( m_DC, TRANSPARENT);
		SetTextColor( m_DC, RGB( 185, 181, 255));

		::ReleaseDC( NULL, dc);
	}

	{
		LOGFONT log;

		memset( &log, 0, sizeof(log));

		log.lfHeight = 12;
		log.lfCharSet = DEFAULT_CHARSET;
		log.lfOutPrecision = OUT_DEFAULT_PRECIS;
		log.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		log.lfQuality = DRAFT_QUALITY;
		log.lfPitchAndFamily = DEFAULT_PITCH;
		strcpy( log.lfFaceName, "Tahoma");

		m_hFont = CreateFontIndirect( &log);
		m_hOldFont = (HFONT) ::SelectObject( m_DC, m_hFont);
	}

	hbrBack = (HBRUSH) ::GetStockObject( NULL_BRUSH);

	hCursor = ::LoadCursor( NULL, IDC_ARROW);

	lpszClassName = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hbrBack, NULL);

	return CWnd::Create( lpszClassName, "", dwStyle, rect, pParentWnd, nID);
}

void CGraphViewCtrl::Simulate( INT32 count, REAL32 * pResult)
{
	INT32 i, last;
	REAL32 timeSlice, tSlice, time, t;
	REAL32 regs[2];
	
	tSlice = 1.0f / count;
	timeSlice = m_Duration * tSlice;

	if( m_Func[1] != i3ParticleInfo::FUNC_NONE)
		last = 1;
	else
		last = 0;

	regs[0] = m_Init[0];
	regs[1] = m_Init[1];

	t = 0.0f;
	time = 0.0f;

	for( i = 0; i < count; i++)
	{
		switch( m_Func[last])
		{
			case i3ParticleInfo::FUNC_CONSTANT :	
				if( last == 1)
					regs[last] = m_Init[last];
				break;
			case i3ParticleInfo::FUNC_SIN :			regs[last] = i3Math::vsin( t) * m_Factor[ last];	break;
			case i3ParticleInfo::FUNC_COS :			regs[last] = i3Math::vcos( t) * m_Factor[ last];	break;
			//case i3ParticleInfo::FUNC_TABLE :		regs[last] = m_pTable[last]->GetKeyframe( time, PosIntp);	
		}

		if( last == 1)
		{
			regs[0] += regs[1] * timeSlice;
		}

		if( m_bClamp)
		{
			regs[0] = i3Math::Clamp( regs[0], m_MinValue, m_MaxValue);
		}

		pResult[i] = regs[0];
		t += tSlice;
		time += timeSlice;

	}
}

#define CNT		32

void CGraphViewCtrl::Draw(void)
{
	CRect Rect;
	REAL32 plot[CNT], Min, Max, tm, xFactor, yFactor;
	INT32 i;
	HPEN hAxisPen, hPlotPen, hOldPen;
	INT32 cx, cy, _x, _y, _tx, _ty;
	char conv[64];
	bool bTextOut;
	SIZE sz;

	if( m_hBitmap != NULL)
	{
		SelectObject( m_DC, m_hOldBitmap);
		DeleteObject( m_hBitmap);
		m_hBitmap = NULL;
	}

	GetClientRect( &Rect);

	cx = Rect.Width();
	cy = Rect.Height();

	m_hBitmap = CreateBitmap( cx, cy, 1, ::GetDeviceCaps( m_DC, BITSPIXEL), NULL);
	m_hOldBitmap = (HBITMAP) ::SelectObject( m_DC, m_hBitmap);

	// 배경 지움.
	{
		HBRUSH hBrush = CreateSolidBrush( RGB( 45, 39, 36));

		FillRect( m_DC, &Rect, hBrush);

		::DeleteObject( hBrush);
	}

	{
		hAxisPen = CreatePen( PS_SOLID, 1, RGB( 167, 167, 167));

		hPlotPen = CreatePen( PS_SOLID, 1, RGB( 255, 155, 155));

		hOldPen = (HPEN) ::SelectObject( m_DC, hAxisPen);
	}

	// Y 축선
	{
		::MoveToEx( m_DC, 2, 2, NULL);
		::LineTo( m_DC, 2, cy - 2);
	}

	// X 축선
	{
		::MoveToEx( m_DC, 2, cy >> 1, NULL);
		::LineTo( m_DC, cx - 2, cy >> 1);
	}

	if( m_Duration > 0.0f)
	{
		Simulate( CNT, plot);

		Min = Max = plot[0];

		for( i = 1; i < CNT; i++)
		{
			Min = MIN( Min, plot[i]);
			Max = MAX( Max, plot[i]);
		}

		// 범위 재 산정.
		{
			if( i3Math::abs(Min) > i3Math::abs(Max))
			{
				Max = -Min;
			}
			else
			{
				Min = -Max;
			}
		}

		xFactor = 1.0f / (m_Duration) * (cx - 4);
		yFactor = 1.0f / (Max - Min) * (cy - 4);

		tm = 0.0f;

		::SelectObject( m_DC, hPlotPen);

		for( i = 0; i < CNT; i++)
		{
			bTextOut = false;

			_x = (INT32)(2 + (tm * xFactor));
			_y = (INT32)((Max - plot[i]) * yFactor) + 2;

			if( i == 0)
			{
				MoveToEx( m_DC, _x, _y, NULL);
				bTextOut = true;
			}
			else
			{
				LineTo( m_DC, _x, _y);

				if( i == CNT - 1)
					bTextOut = true;
			}

			if( bTextOut)
			{
				INT32 len;

				if( plot[i] > 10.0f)
					sprintf( conv, "%.1f", plot[i]);
				else if( plot[i] > 1.0f)
					sprintf( conv, "%.2f", plot[i]);
				else
					sprintf( conv, "%.3f", plot[i]);

				len = strlen( conv);

				if( _x < (cx >> 1))
					_tx = 2;
				else
				{
					GetTextExtentPoint32( m_DC, conv, len, &sz);
					_tx = -sz.cx - 2;
				}

				if( _y < (cx >> 1))
					_ty = 2;
				else
					_ty = -14;
				
				::TextOut( m_DC, _x + _tx, _y + _ty, conv, len);
			}

			tm += (m_Duration / CNT);
		}
	}
}

void	CGraphViewCtrl::Set( REAL32 duration, i3ParticleInfo::FUNC_TYPE func0, REAL32 init0, REAL32 factor0,
		i3ParticleInfo::FUNC_TYPE func1, REAL32 init1, REAL32 factor1,
		BOOL bClamp, REAL32 minval, REAL32 maxval)
{
	m_Duration = duration;
	m_Func[0] = func0;
	m_Func[1] = func1;
	m_Init[0] = init0;
	m_Init[1] = init1;
	m_Factor[0] = factor0;
	m_Factor[1] = factor1;

	m_bClamp = bClamp;
	m_MinValue = minval;
	m_MaxValue = maxval;

	Draw();

	Invalidate();
}

BEGIN_MESSAGE_MAP(CGraphViewCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CGraphViewCtrl message handlers


int CGraphViewCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CGraphViewCtrl::OnDestroy()
{
	CWnd::OnDestroy();

}

void CGraphViewCtrl::OnPaint()
{
	CRect rect;
	CPaintDC dc(this); // device context for painting

	GetClientRect( &rect);

	BitBlt( dc.m_hDC, rect.left, rect.top, rect.right, rect.bottom, m_DC, 0, 0, SRCCOPY);
}

void CGraphViewCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	Draw();
}
