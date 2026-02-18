// Dlg_SpreadValue.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg_SplitValue.h"


// CDlg_SpreadValue dialog

IMPLEMENT_DYNAMIC(CDlg_SplitValue, CDialog)

CDlg_SplitValue::CDlg_SplitValue(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_SplitValue::IDD, pParent)
{
	m_Col = 1;
	m_Margin = 0;
}

CDlg_SplitValue::~CDlg_SplitValue()
{
}

void CDlg_SplitValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_COLUMN_COUNT, m_edColumnCount);
	DDX_Control(pDX, IDC_EDIT_MARGIN_VALUE, m_edMargin);
}


BEGIN_MESSAGE_MAP(CDlg_SplitValue, CDialog)
	ON_EN_KILLFOCUS(IDC_EDIT_COLUMN_COUNT, &CDlg_SplitValue::OnEnKillfocusEditColumnCount)
	ON_EN_KILLFOCUS(IDC_EDIT_MARGIN_VALUE, &CDlg_SplitValue::OnEnKillfocusEditMarginValue)
END_MESSAGE_MAP()


// CDlg_SpreadValue message handlers

void CDlg_SplitValue::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	OnEnKillfocusEditColumnCount();
	OnEnKillfocusEditMarginValue();

	CDialog::OnOK();
}

void CDlg_SplitValue::_UpdateInfo(void)
{
	char conv[MAX_PATH];

	i3::snprintf( conv, MAX_PATH, "%d", m_Col);
	m_edColumnCount.SetWindowText((LPCTSTR)conv);

	i3::snprintf( conv, MAX_PATH, "%d", m_Margin);
	m_edMargin.SetWindowText((LPCTSTR)conv);
}

void CDlg_SplitValue::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}

void CDlg_SplitValue::OnEnKillfocusEditColumnCount()
{
	// TODO: Add your control notification handler code here
	if( m_edColumnCount.GetModify())
	{
		char conv[MAX_PATH];

		m_edColumnCount.GetWindowText(conv, MAX_PATH -1);
		
		m_Col = atoi( conv);
	}
}

void CDlg_SplitValue::OnEnKillfocusEditMarginValue()
{
	// TODO: Add your control notification handler code here
	if( m_edMargin.GetModify())
	{
		char conv[MAX_PATH];

		m_edMargin.GetWindowText(conv, MAX_PATH -1);
		
		m_Margin = atoi( conv);
	}
}

BOOL CDlg_SplitValue::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	_UpdateInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
