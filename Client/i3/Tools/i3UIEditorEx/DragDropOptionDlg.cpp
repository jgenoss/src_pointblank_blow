// DragDropOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DragDropOptionDlg.h"


// CDragDropOptionDlg dialog

IMPLEMENT_DYNAMIC(CDragDropOptionDlg, CDialog)

CDragDropOptionDlg::CDragDropOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDragDropOptionDlg::IDD, pParent)
	, m_opOverWrite(0)
	, m_opAddSub(0)
	, m_opCancel(0)
{
	m_nSelectedItem = 2;
}

CDragDropOptionDlg::~CDragDropOptionDlg()
{
}

void CDragDropOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDragDropOptionDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO1, &CDragDropOptionDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CDragDropOptionDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CDragDropOptionDlg::OnBnClickedRadio3)
END_MESSAGE_MAP()


// CDragDropOptionDlg message handlers

void CDragDropOptionDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);
	m_nSelectedItem = 0;
}

void CDragDropOptionDlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO2);
	m_nSelectedItem = 1;
}

void CDragDropOptionDlg::OnBnClickedRadio3()
{
	// TODO: Add your control notification handler code here
	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO3);
	m_nSelectedItem = 2;
}

void CDragDropOptionDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}

void CDragDropOptionDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}

BOOL CDragDropOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_nSelectedItem = 2;

	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO3);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
