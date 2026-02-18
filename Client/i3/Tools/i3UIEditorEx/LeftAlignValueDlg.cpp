// LeftAlignValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LeftAlignValueDlg.h"


// CLeftAlignValueDlg dialog

IMPLEMENT_DYNAMIC(CLeftAlignValueDlg, CDialog)

CLeftAlignValueDlg::CLeftAlignValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLeftAlignValueDlg::IDD, pParent)
{
	m_X = m_Y = 0.0f;
}

CLeftAlignValueDlg::~CLeftAlignValueDlg()
{
}

void CLeftAlignValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_POS_LEFT, m_edPosX);
	DDX_Control(pDX, IDC_EDIT_POS_TOP, m_edPosY);
}


BEGIN_MESSAGE_MAP(CLeftAlignValueDlg, CDialog)
	ON_EN_KILLFOCUS(IDC_EDIT_POS_LEFT, &CLeftAlignValueDlg::OnEnKillfocusEditPosLeft)
	ON_EN_KILLFOCUS(IDC_EDIT_POS_TOP, &CLeftAlignValueDlg::OnEnKillfocusEditPosTop)
END_MESSAGE_MAP()


// CLeftAlignValueDlg message handlers

void CLeftAlignValueDlg::OnEnKillfocusEditPosLeft()
{
	// TODO: Add your control notification handler code here
	if( m_edPosX.GetModify())
	{
		char conv[MAX_PATH];

		m_edPosX.GetWindowText(conv, MAX_PATH -1);
		
		m_X = (REAL32)atof( conv);
	}
}

void CLeftAlignValueDlg::OnEnKillfocusEditPosTop()
{
	// TODO: Add your control notification handler code here
	if( m_edPosY.GetModify())
	{
		char conv[MAX_PATH];

		m_edPosY.GetWindowText(conv, MAX_PATH -1);
		
		m_Y = (REAL32)atof( conv);
	}
}

void CLeftAlignValueDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	OnEnKillfocusEditPosLeft();
	OnEnKillfocusEditPosTop();

	CDialog::OnOK();
}

void CLeftAlignValueDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}

void CLeftAlignValueDlg::_UpdateInfo(void)
{
	char conv[MAX_PATH];

	i3::snprintf( conv, MAX_PATH, "%f", m_X);
	m_edPosX.SetWindowText((LPCTSTR)conv);

	i3::snprintf( conv, MAX_PATH, "%f", m_Y);
	m_edPosY.SetWindowText((LPCTSTR)conv);
}