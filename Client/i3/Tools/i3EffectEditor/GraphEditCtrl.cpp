// GraphEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "GraphEditCtrl.h"
#include ".\grapheditctrl.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/string/ext/ftoa.h"

#define		GR_SX		30
#define		GR_SY		10

#define		IDC_STEPCOUNT		100
#define		IDC_VALUE			101

// CGraphEditCtrl

#define _getX( tm)		(m_GraphRect.left + (INT32)(tm * m_XFactor))
#define _getY( val)		(m_GraphRect.bottom - (INT32)(((val) - m_Min) * m_YFactor))

IMPLEMENT_DYNAMIC(CGraphEditCtrl, CWnd)
CGraphEditCtrl::CGraphEditCtrl()
{
	m_pTable = NULL;

	m_hAxisPen = NULL;
	m_hLinePen = NULL;
	m_hGraphBackBrush = NULL;
	m_hCPBackBrush = NULL;
	m_hFont = NULL;

	m_bDragging = false;
	m_bLimitEnable = false;
	m_LimitMin = 0.0f;
	m_LimitMax = 0.0f;
}

CGraphEditCtrl::~CGraphEditCtrl()
{
	DeleteObject( m_hAxisPen);
	DeleteObject( m_hLinePen);
	DeleteObject( m_hGraphBackBrush);
	DeleteObject( m_hFont);
	DeleteObject( m_hCPBackBrush);

	RemoveAllCP();
}

BOOL CGraphEditCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR lpszClassName;
	HBRUSH hbrBack;

	m_hAxisPen = ::CreatePen( PS_SOLID, 1, GetSysColor( COLOR_3DDKSHADOW));

	m_hLinePen = ::CreatePen( PS_SOLID, 1, RGB( 255, 128, 128));

	m_hGraphBackBrush = ::CreateSolidBrush( GetSysColor( COLOR_WINDOW));

	m_hCPBackBrush = ::CreateSolidBrush( GetSysColor( COLOR_3DHILIGHT));

	{
		LOGFONT	log;

		memset( &log, 0, sizeof(log));
		log.lfHeight = 12;
		log.lfWidth = 0;
		log.lfWeight = 0;
		log.lfCharSet = DEFAULT_CHARSET;
		log.lfOutPrecision = OUT_DEFAULT_PRECIS;
		log.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		log.lfQuality = DRAFT_QUALITY;
		log.lfPitchAndFamily = DEFAULT_PITCH;
		strcpy( log.lfFaceName, "Tahoma");

		m_hFont = CreateFontIndirect( &log);
	}

	m_hArrowCursor = ::LoadCursor( NULL, IDC_ARROW);

	m_hCrossCursor = ::LoadCursor( NULL, IDC_CROSS);

	m_hMoveCursor = ::LoadCursor( NULL, IDC_SIZEALL);

	hbrBack = ::GetSysColorBrush( COLOR_3DFACE);

	lpszClassName = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, m_hArrowCursor, hbrBack, NULL);

	return CWnd::Create( lpszClassName, "", dwStyle, rect, pParentWnd, nID);
}

void CGraphEditCtrl::SetTable( i3KeyframeTable * pTable, REAL32 TimeLength, REAL32 lMin, REAL32 lMax, bool bLimit)
{
	char conv[256];

	m_pTable = pTable;

	m_LimitMin = lMin;
	m_LimitMax = lMax;
	m_bLimitEnable = bLimit;

	if( m_pTable == NULL)
	{
		m_StepCtrl.SetWindowText( "");
	}
	else
	{
		sprintf( conv, "%d", m_pTable->GetCount());
		m_StepCtrl.SetWindowText( conv);	
	}

	m_StepCtrl.EnableWindow( m_pTable != NULL);

	BuildCP();

	Invalidate();
}

void CGraphEditCtrl::SetCP( CONTROLPOINT * pCP, INT32 x, INT32 y)
{
	pCP->rect.left = (x - 2);
	pCP->rect.top = (y - 2);
	pCP->rect.right = (x + 3);
	pCP->rect.bottom = (y + 3);
}

void CGraphEditCtrl::AddCP( INT32 x, INT32 y)
{
	CONTROLPOINT * pNew;

	pNew = new CONTROLPOINT;

	SetCP( pNew, x, y);

	m_CPList.push_back( pNew);
}

void CGraphEditCtrl::RemoveCP( INT32 idx)
{
	CONTROLPOINT * pCP;

	pCP = m_CPList[idx];

	delete pCP;

	i3::vu::erase_index(m_CPList, idx);
}

void CGraphEditCtrl::RemoveAllCP(void)
{
	
	i3::cu::for_each_delete_clear(m_CPList);

}

INT32 CGraphEditCtrl::FindCP( POINT pt)
{
	
	CONTROLPOINT * pCP;

	for(size_t i = 0; i < m_CPList.size(); i++)
	{
		pCP = m_CPList[i];

		if( PtInRect( & pCP->rect, pt))
			return i;
	}

	return -1;
}

void CGraphEditCtrl::UpdateParams(void)
{
	INT32 i;
	REAL32 * pTable;

	if( m_pTable != NULL)
	{
		pTable = m_pTable->GetTable();

		for( i = 0; i < m_pTable->GetCount(); i++)
		{
			if( i == 0)
			{
				m_Min = m_Max = pTable[i];
			}
			else
			{
				m_Min = MIN( m_Min, pTable[i]);
				m_Max = MAX( m_Max, pTable[i]);
			}
		}

		//  1.0 단위로 끊는다.
		if( i3Math::abs( i3Math::fmod( m_Min, 1.0f)) > 0.0f)
			m_Min = m_Min - i3Math::fmod( m_Min, 1.0f) - 1.0f;
		else
			m_Min = m_Min - i3Math::fmod( m_Min, 1.0f);

		if( i3Math::abs( i3Math::fmod( m_Max, 1.0f)) > 0.0f)
			m_Max = m_Max - i3Math::fmod( m_Max, 1.0f) + 1.0f;
		else
			m_Max = m_Max - i3Math::fmod( m_Max, 1.0f);
		
		m_XFactor = (REAL32)(m_GraphRect.Width());
		m_YFactor = (REAL32)((1.0f / (m_Max - m_Min)) * m_GraphRect.Height());
	}
	else
	{
		m_Min = -1.0f;
		m_Max = 1.0f;

		m_XFactor = (REAL32)(m_GraphRect.Width());
		m_YFactor = (REAL32)((1.0f / (m_Max - m_Min)) * m_GraphRect.Height());
	}

	m_bDrawZeroLine = TRUE;
	if((m_Min > 0.0f) || (m_Max < 0.0f))
		m_bDrawZeroLine = FALSE;

	m_YOrg = _getY(0.0f);
}

void CGraphEditCtrl::ResetCP(void)
{
	INT32 x, y;
	CONTROLPOINT * pCP;
	REAL32 * pTable;
	REAL32 tm, rstep;

	if( m_pTable == NULL)
		return;

	pTable = m_pTable->GetTable();

	tm = 0.0f;
	rstep = 1.0f / ((INT32)m_CPList.size() - 1);

	for(size_t i = 0; i < m_CPList.size(); i++, tm += rstep)
	{
		pCP = m_CPList[i];

		x = (INT32)_getX( tm);
		y = (INT32)_getY( pTable[i]);

		SetCP( pCP, x, y);
	}
}

void CGraphEditCtrl::BuildCP(void)
{
	INT32 i;

	RemoveAllCP();

	if( m_pTable != NULL)
	{
		for( i = 0; i < m_pTable->GetCount(); i++)
		{
			AddCP( 0, 0);
		}
	}

	UpdateParams();

	ResetCP();
}

void CGraphEditCtrl::SetClientRect( INT32 cx, INT32 cy)
{
	m_ClientRect.left = 0;
	m_ClientRect.top = 0;
	m_ClientRect.right = cx;
	m_ClientRect.bottom = cy;

	m_GraphRect.left = GR_SX;
	m_GraphRect.top = GR_SY;
	m_GraphRect.right = m_ClientRect.right - 4;
	m_GraphRect.bottom = m_ClientRect.bottom - 32;

	m_StepCtrl.SetWindowPos( NULL, m_GraphRect.right - 20, m_ClientRect.bottom - 19, 20, 16, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CGraphEditCtrl::DrawAxis( HDC dc)
{
	char conv[256];
	SIZE sz;
	INT32 len;

	::SelectObject( dc, m_hAxisPen);

	// X 축
	::MoveToEx( dc, m_GraphRect.left - 4, m_GraphRect.top - 4, NULL);
	::LineTo( dc, m_GraphRect.left - 4, m_GraphRect.bottom + 4);

	// Y 축
	::LineTo( dc, m_GraphRect.right + 4, m_GraphRect.bottom + 4);

	// Min
	{
		i3::ftoa( m_Min, conv);

		len = strlen( conv);

		GetTextExtentPoint32( dc, conv, len, &sz);
		::TextOut( dc, m_GraphRect.left - sz.cx - 5, m_GraphRect.bottom - sz.cy + 2, conv, len);
	}

	// Max
	{
		i3::ftoa( m_Max, conv);

		len = strlen( conv);

		GetTextExtentPoint32( dc, conv, len, &sz);
		::TextOut( dc, m_GraphRect.left - sz.cx - 5, m_GraphRect.top - 2 , conv, len);
	}

	if( m_bDrawZeroLine)
	{
		::MoveToEx( dc, m_GraphRect.left, m_YOrg, NULL);
		::LineTo( dc, m_GraphRect.right, m_YOrg);

		::TextOut( dc, m_GraphRect.left - 10, m_YOrg - 6, "0", 1);
	}
}

void CGraphEditCtrl::DrawPlotLine( HDC dc)
{
	INT32 i;
	REAL32 * pTable, tm, rstep;
	INT32 x, y;

	if( m_pTable == NULL)
		return;

	tm = 0.0f;
	rstep = 1.0f / (m_pTable->GetCount() - 1);

	pTable = m_pTable->GetTable();

	SelectObject( dc, m_hLinePen);

	for( i = 0; i < m_pTable->GetCount(); i++)
	{
		x = _getX( tm);
		y = _getY( pTable[i]);

		if( i == 0)
		{
			::MoveToEx( dc, x, y, NULL);
		}
		else
		{
			::LineTo( dc, x, y);
		}

		tm += rstep;
	}
}

void CGraphEditCtrl::DrawCP( HDC dc)
{
	INT32 x1, y1, x2, y2;
	CONTROLPOINT * pCP;

	::SelectObject( dc, m_hCPBackBrush);
	::SelectObject( dc, m_hAxisPen);
	
	for(size_t i = 0; i < m_CPList.size(); i++)
	{
		pCP = m_CPList[i];

		x1 = pCP->rect.left;
		y1 = pCP->rect.top;
		x2 = pCP->rect.right;
		y2 = pCP->rect.bottom;

		::Rectangle( dc, x1, y1, x2, y2);
	}
}

void CGraphEditCtrl::DrawGraph( HDC dc)
{
	CRect grc;
	HPEN hOldPen;
	HBRUSH hOldBrush;
	HFONT hOldFont;

	hOldPen = (HPEN) ::SelectObject( dc, m_hAxisPen);
	hOldBrush = (HBRUSH) ::SelectObject( dc, m_hGraphBackBrush);
	hOldFont = (HFONT) ::SelectObject( dc, m_hFont);

	::SetBkMode( dc, TRANSPARENT);
	::SetTextColor( dc, GetSysColor( COLOR_BTNSHADOW));

	{
		::TextOut( dc, m_ClientRect.right - 48, m_ClientRect.bottom - 18, "Step", 4);
	}

	// Draw Background
	{
		CRect rc;

		rc = m_GraphRect;

		rc.left -= 4;
		rc.top -= 4;
		rc.right += 4;
		rc.bottom += 4;

		FillRect( dc, &rc, m_hGraphBackBrush);
	}

	DrawAxis( dc);

	DrawPlotLine( dc);

	DrawCP( dc);

	::SelectObject( dc, hOldPen);
	::SelectObject( dc, hOldBrush);
	::SelectObject( dc, hOldFont);
}

void CGraphEditCtrl::SetTableCount( INT32 count)
{
	REAL32 * pNewTable, * pTable;
	REAL32 tm, step;
	INT32 i;

	if( count <= 0)
		return;

	if( m_pTable == NULL)
		return;

	pNewTable = new REAL32[ count];
	
	tm = 0.0f;
	step = 1.0f / count;

	for( i = 0; i < count; i++)
	{
		pNewTable[i] = m_pTable->GetKeyframe( tm, i3KeyframeTable::LERP);

		tm += step;
	}

	m_pTable->Create( count, TRUE);

	pTable = m_pTable->GetTable();

	for( i = 0; i < count; i++)
	{
		pTable[i] = pNewTable[i];
	}

	delete pNewTable;

	UpdateParams();
	BuildCP();
	Invalidate();
}

BEGIN_MESSAGE_MAP(CGraphEditCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CGraphEditCtrl message handlers
int CGraphEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rc;

		m_StepCtrl.Create( WS_CHILD | WS_VISIBLE, rc, this, IDC_STEPCOUNT);
		m_StepCtrl.ModifyStyleEx( 0, WS_EX_STATICEDGE);
		m_StepCtrl.SetFont( m_hFont);
	}

	SetTable( NULL);

	return 0;
}


void CGraphEditCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	DrawGraph( dc.m_hDC);
}

void CGraphEditCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	SetClientRect( cx, cy);

	UpdateParams();
	ResetCP();
}


void CGraphEditCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging)
	{
		CONTROLPOINT * pCP = m_CPList[ m_DragCP];
		REAL32 * pTable = m_pTable->GetTable();
		REAL32 diff;

		diff = (((pCP->rect.top + pCP->rect.bottom) >> 1) - point.y) / m_YFactor;

		pTable[ m_DragCP] += diff;
		if( m_bLimitEnable)
		{
			if( pTable[m_DragCP] < m_LimitMin)
				pTable[m_DragCP] = m_LimitMin;
			if( pTable[m_DragCP] > m_LimitMax)
				pTable[m_DragCP] = m_LimitMax;
		}

		UpdateParams();
		ResetCP();
		Invalidate();
	}
	else
	{
		INT32 idx;

		idx = FindCP( point);
		if( idx != -1)
		{
			SetMouseCursor( m_hMoveCursor);
		}
		else if( PtInRect( &m_GraphRect, point))
		{
			SetMouseCursor( m_hCrossCursor);
		}
		else
		{
			SetMouseCursor( m_hArrowCursor);
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CGraphEditCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	INT32 idx;

	idx = FindCP( point);
	if( idx != -1)
	{
		::SetCapture( m_hWnd);

		m_DragCP = idx;
		m_bDragging = true;
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CGraphEditCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging)
	{
		m_bDragging = false;

		::ReleaseCapture();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

BOOL CGraphEditCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR * pHdr = (NMHDR *) lParam;

	if( pHdr->code == NM_RETURN)
	{
		switch( pHdr->idFrom)
		{
			case IDC_STEPCOUNT :
				{
					char conv[256];

					m_StepCtrl.GetWindowText( conv, sizeof(conv) - 1);
					
					SetTableCount( atoi( conv));
				}
				break;
		}
	}

	return CWnd::OnNotify(wParam, lParam, pResult);
}
