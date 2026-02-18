// Dlg_SubsetSpec.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg_SubsetSpec.h"


// CDlg_SubsetSpec dialog

IMPLEMENT_DYNAMIC(CDlg_SubsetSpec, CDialog)

CDlg_SubsetSpec::CDlg_SubsetSpec(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_SubsetSpec::IDD, pParent)
{
	m_pRule = NULL;
}

CDlg_SubsetSpec::~CDlg_SubsetSpec()
{
}

void CDlg_SubsetSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_PATH, m_ED_Path);
	DDX_Control(pDX, IDC_ED_FILE, m_ED_Name);
	DDX_Control(pDX, IDC_ED_SIZE, m_ED_Size);
}

bool CDlg_SubsetSpec::Execute( i3VTSubsetRule * pRule)
{
	m_pRule = pRule;

	if( DoModal() == IDCANCEL)
		return false;

	return true;
}

BEGIN_MESSAGE_MAP(CDlg_SubsetSpec, CDialog)
END_MESSAGE_MAP()


// CDlg_SubsetSpec message handlers

BOOL CDlg_SubsetSpec::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( m_pRule != NULL)
	{
		char conv[256];

		m_ED_Path.SetWindowText( m_pRule->m_strPath.c_str());
		m_ED_Name.SetWindowText( m_pRule->m_strName.c_str());

		INT32 mb = m_pRule->m_LimitSize / (1024 * 1024);
		i3::snprintf( conv, _countof( conv), "%d", mb);
		m_ED_Size.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_SubsetSpec::OnOK()
{
	if( m_pRule != NULL)
	{
		char conv[256];

		m_ED_Name.GetWindowText( conv, sizeof(conv));
		m_pRule->m_strName = conv;

		m_ED_Size.GetWindowText( conv, sizeof(conv));
		m_pRule->m_LimitSize = atoi( conv) * 1024 * 1024;
	}

	CDialog::OnOK();
}
