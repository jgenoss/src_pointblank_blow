// SelectSpringDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3ClothSim.h"
#include "SelectSpringDialog.h"
#include ".\selectspringdialog.h"


// CSelectSpringDialog dialog

IMPLEMENT_DYNAMIC(CSelectSpringDialog, CDialog)
CSelectSpringDialog::CSelectSpringDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectSpringDialog::IDD, pParent)
{
}

CSelectSpringDialog::~CSelectSpringDialog()
{
}

void CSelectSpringDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DRAGCTRL_PLACEHOLDER, m_PlaceHolderCtrl);
}


BEGIN_MESSAGE_MAP(CSelectSpringDialog, CDialog)
	ON_BN_CLICKED(IDC_SELECT, OnBnClickedSelect)
	ON_BN_CLICKED(IDC_CANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CSelectSpringDialog message handlers
BOOL CSelectSpringDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		CRect rect;

		m_PlaceHolderCtrl.GetWindowRect( &rect);

		ScreenToClient( &rect);

		m_DirCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 10101);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectSpringDialog::OnBnClickedSelect()
{
	OnOK();
}

void CSelectSpringDialog::OnBnClickedCancel()
{
	OnCancel();
}

