// ColorKeyEditDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "ColorKeyEditDialog.h"
#include ".\colorkeyeditdialog.h"
#include "GlobalVariable.h"
#include "OptionInfo.h"

// CColorKeyEditDialog dialog

IMPLEMENT_DYNAMIC(CColorKeyEditDialog, CDialog)
CColorKeyEditDialog::CColorKeyEditDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CColorKeyEditDialog::IDD, pParent)
{
	m_pRTable = NULL;
	m_pGTable = NULL;
	m_pBTable = NULL;
	m_pATable = NULL;
}

CColorKeyEditDialog::~CColorKeyEditDialog()
{
}

void CColorKeyEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CColorKeyEditDialog::Execute( POINT pt, i3KeyframeTable * pRTable, i3KeyframeTable * pGTable, i3KeyframeTable * pBTable, i3KeyframeTable * pATable)
{
	m_Point = pt;

	m_pRTable = pRTable;
	m_pGTable = pGTable;
	m_pBTable = pBTable;
	m_pATable = pATable;

	m_ColorKeyCtrl.SetColorKeyframes( m_pRTable, m_pGTable, m_pBTable, m_pATable);

	if( ! IsWindowVisible())
		::SetWindowPos( m_hWnd, NULL, m_Point.x, m_Point.y, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);

	ShowWindow( SW_SHOW);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CColorKeyEditDialog, CDialog)
	
END_MESSAGE_MAP()


// CColorKeyEditDialog message handlers

BOOL CColorKeyEditDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		CRect rect;

		GetClientRect( &rect);

		rect.left += 3;
		rect.right -= 3;
		rect.top += 3;
		rect.bottom -= 25;
		if( rect.bottom < rect.top)
			rect.bottom = rect.top + 3;

		m_ColorKeyCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 100);

		m_ColorKeyCtrl.SetColorKeyframes( m_pRTable, m_pGTable, m_pBTable, m_pATable);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

