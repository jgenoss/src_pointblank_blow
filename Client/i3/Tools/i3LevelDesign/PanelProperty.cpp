// PanelProperty.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "PanelProperty.h"
#include ".\panelproperty.h"


// CPanelProperty dialog

IMPLEMENT_DYNAMIC(CPanelProperty, i3TDKDialogBase)
CPanelProperty::CPanelProperty(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelProperty::IDD, pParent)
{
}

CPanelProperty::~CPanelProperty()
{

}

void CPanelProperty::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelProperty, i3TDKDialogBase)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPanelProperty message handlers

void CPanelProperty::OnCancel()
{
}

void CPanelProperty::OnOK()
{
}

BOOL CPanelProperty::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rect;

		GetClientRect( &rect);

		rect.left = 3;
		rect.top = 3;
		rect.right -= 3;
		rect.bottom -= 3;
		m_FoldCtrl.Create( "Properties", WS_VISIBLE | WS_CHILD, rect, this, 100);

		m_CommonPropDlg.Create( m_CommonPropDlg.IDD, &m_FoldCtrl);
		m_FoldCtrl.AddPane( &m_CommonPropDlg, "Common Properties");

		m_SpecPropDlg.Create( m_SpecPropDlg.IDD, &m_FoldCtrl);
		m_FoldCtrl.AddPane( &m_SpecPropDlg, "Specific...");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelProperty::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_FoldCtrl.m_hWnd))
	{
		m_FoldCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOZORDER | SWP_NOACTIVATE);
	}
}
