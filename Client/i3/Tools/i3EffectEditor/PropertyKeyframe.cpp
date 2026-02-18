// PropertyKeyframe.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "PropertyKeyframe.h"
#include ".\propertykeyframe.h"


// CPropertyKeyframe dialog

IMPLEMENT_DYNAMIC(CPropertyKeyframe, CPropertyPage)
CPropertyKeyframe::CPropertyKeyframe()
	: CPropertyPage(CPropertyKeyframe::IDD)
{
}

CPropertyKeyframe::~CPropertyKeyframe()
{
}

void CPropertyKeyframe::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

void CPropertyKeyframe::Edit( i3KeyframeTable * pTable, INT32 type)
{
	switch( type)
	{
		case 0 :	m_GraphEditor.SetTable( pTable);	break;
		case 1 :	m_GraphEditor.SetTable( pTable, 0.0f, 0.0f, 255.0f, true);
	}
}


BEGIN_MESSAGE_MAP(CPropertyKeyframe, CPropertyPage)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPropertyKeyframe message handlers

BOOL CPropertyKeyframe::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	{
		CRect rc;

		rc.SetRect( 0, 0, 10, 10);

		m_GraphEditor.Create( WS_CHILD | WS_VISIBLE, rc, this, 100);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CPropertyKeyframe::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	if( ::IsWindow( m_GraphEditor.m_hWnd))
	{
		m_GraphEditor.SetWindowPos( NULL, 0, 0, cx, MIN( cy, 220), SWP_NOACTIVATE | SWP_NOZORDER);
	}
}


void CPropertyKeyframe::OnOK()
{
}

void CPropertyKeyframe::OnCancel()
{
}
