// PaneProperty.cpp : implementation file
//

#include "stdafx.h"
#include "i3PackFileEditor.h"
#include "PaneProperty.h"
#include ".\paneproperty.h"


// CPaneProperty dialog

IMPLEMENT_DYNAMIC(CPaneProperty, CDialog)
CPaneProperty::CPaneProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneProperty::IDD, pParent)
{
	m_pCurSpec = NULL;
}

CPaneProperty::~CPaneProperty()
{
}

void CPaneProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CPaneProperty::PopupSpec( i3ElementBase * pObj)
{
	if( m_pCurSpec != NULL)
	{
		m_pCurSpec->ShowWindow( SW_HIDE);
		m_pCurSpec = NULL;
	}

	if( pObj != NULL)
	{
	}

	if( m_pCurSpec != NULL)
	{
		CRect rect;

		GetClientRect( &rect);

		m_pCurSpec->SetParent( this);
		m_pCurSpec->SetWindowPos( NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
		m_pCurSpec->ShowWindow( SW_SHOW);
	}
}

BEGIN_MESSAGE_MAP(CPaneProperty, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPaneProperty message handlers

void CPaneProperty::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( m_pCurSpec != NULL)
	{
		m_pCurSpec->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}
