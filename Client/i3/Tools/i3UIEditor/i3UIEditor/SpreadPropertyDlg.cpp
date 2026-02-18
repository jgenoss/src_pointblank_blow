// SpreadPropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "SpreadPropertyDlg.h"

// CSpreadPropertyDlg dialog

IMPLEMENT_DYNAMIC(CSpreadPropertyDlg, CDialog)
CSpreadPropertyDlg::CSpreadPropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpreadPropertyDlg::IDD, pParent)
{
	m_nColCount		= 5;
	m_nSpreadWidth	= 1.0f;
	m_nSpreadHeight = 1.0f;
}

CSpreadPropertyDlg::~CSpreadPropertyDlg()
{
}

void CSpreadPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SPREAD_COLCOUNT, m_SpreadColCount);
	DDX_Control(pDX, IDC_SPREAD_WIDTH, m_SpreadWidth);
	DDX_Control(pDX, IDC_EDIT_SPREAD_HEIGHT, m_SpreadHeight);
}


BEGIN_MESSAGE_MAP(CSpreadPropertyDlg, CDialog)
END_MESSAGE_MAP()

// CSpreadPropertyDlg message handlers
BOOL CSpreadPropertyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_SpreadColCount.SetWindowText( "5");
	m_SpreadWidth.SetWindowText( "1.0");
	m_SpreadHeight.SetWindowText( "1.0");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpreadPropertyDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	char conv[MAX_PATH];

	m_SpreadColCount.GetWindowText( conv, MAX_PATH -1);
	m_nColCount = atoi( conv);

	m_SpreadWidth.GetWindowText( conv, MAX_PATH -1);
	m_nSpreadWidth = (REAL32)atof( conv);

	m_SpreadHeight.GetWindowText( conv, MAX_PATH -1);
	m_nSpreadHeight = (REAL32)atof( conv);

	CDialog::OnOK();
}

void CSpreadPropertyDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}
