// ColorPickCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "ColorPickCtrl.h"


// CColorPickCtrl

IMPLEMENT_DYNAMIC(CColorPickCtrl, CWnd)
CColorPickCtrl::CColorPickCtrl()
{
}

CColorPickCtrl::~CColorPickCtrl()
{
}

BOOL CColorPickCtrl::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR hCursor;
	HBRUSH hBack;

	hCursor = LoadCursor( NULL, IDC_CROSS);
	hBack = (HBRUSH) GetSysColorBrush( COLOR_BTNFACE);

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hBack, NULL);

	return CWnd::Create( pszClass, "", dwStyle, rect, pParentWnd, nID);
}

BEGIN_MESSAGE_MAP(CColorPickCtrl, CWnd)
END_MESSAGE_MAP()

// CColorPickCtrl message handlers

