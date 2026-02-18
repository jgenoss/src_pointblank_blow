// i3PropertySheetCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "i3PropertySheetCtrl.h"
#include ".\i3propertysheetctrl.h"


// i3PropertySheetCtrl

IMPLEMENT_DYNAMIC(i3PropertySheetCtrl, CWnd)
i3PropertySheetCtrl::i3PropertySheetCtrl()
{
	m_pCurActive = NULL;
}

i3PropertySheetCtrl::~i3PropertySheetCtrl()
{
}

BOOL i3PropertySheetCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR lpszClassName;
	HCURSOR hCursor;
	HBRUSH hbrBack;

	hCursor = ::LoadCursor( NULL, IDC_ARROW);

	hbrBack = ::GetSysColorBrush( COLOR_3DFACE);

	lpszClassName = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hbrBack, NULL);

	return CWnd::Create( lpszClassName, "", dwStyle, rect, pParentWnd, nID);
}

INT32 i3PropertySheetCtrl::AddPanel( CWnd * pWnd)
{
	INT32 idx = (INT32)m_WndList.size();

	m_WndList.push_back( pWnd);

	pWnd->SetParent( this);

	if( m_WndList.size() > 1)
	{
		pWnd->ShowWindow( SW_HIDE);
	}
	else
	{
		SetActivePanel( idx);
	}

	return idx;
}

void i3PropertySheetCtrl::SetActivePanel( INT32 idx)
{
	CWnd * pWnd;
	CRect rc;

	GetClientRect( &rc);

	pWnd = m_WndList[idx];

	if( m_pCurActive != NULL)
	{
		m_pCurActive->ShowWindow( SW_HIDE);
	}

	m_pCurActive = pWnd;

	pWnd->SetWindowPos( NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOACTIVATE | SWP_NOZORDER );
	pWnd->ShowWindow( SW_SHOW);
}

void i3PropertySheetCtrl::SetActivePanel( CWnd * pWnd)
{
	INT32 idx;

	idx = FindPanel( pWnd);
	if( idx == -1)
		return;

	SetActivePanel( idx);
}

INT32 i3PropertySheetCtrl::FindPanel( CWnd * pWnd)
{
	
	CWnd * pTemp;

	for(size_t i = 0; i < m_WndList.size(); i++)
	{
		pTemp = m_WndList[i];

		if( pTemp == pWnd)
			return i;
	}

	return -1;
}

BEGIN_MESSAGE_MAP(i3PropertySheetCtrl, CWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()



// i3PropertySheetCtrl message handlers


void i3PropertySheetCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if( m_pCurActive != NULL)
	{
		SetActivePanel( m_pCurActive);
	}
}
