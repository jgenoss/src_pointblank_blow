// DlgWorldSectionInput.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgWorldSectionInput.h"
#include ".\dlgworldsectioninput.h"


// CDlgWorldSectionInput dialog

IMPLEMENT_DYNAMIC(CDlgWorldSectionInput, CDialog)
CDlgWorldSectionInput::CDlgWorldSectionInput(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWorldSectionInput::IDD, pParent)
{
	m_bCanEditID = true;
}

CDlgWorldSectionInput::~CDlgWorldSectionInput()
{
}

void CDlgWorldSectionInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_ID, m_IDCtrl);
	DDX_Control(pDX, IDC_ED_SECTION_NAME, m_NameCtrl);
}

bool CDlgWorldSectionInput::Execute( INT32 id, i3::pack::WORLD_SECTION_INFO * pInfo)
{
	if( pInfo == NULL)
	{
		m_ID = 0;
		m_szName[0] = 0;

		m_bCanEditID = true;
	}
	else
	{
		m_ID = id;
		strcpy( m_szName, pInfo->m_szName);

		m_bCanEditID = false;
	}

	if( DoModal() == IDOK)
		return true;

	return false;
}

BEGIN_MESSAGE_MAP(CDlgWorldSectionInput, CDialog)
END_MESSAGE_MAP()


// CDlgWorldSectionInput message handlers

BOOL CDlgWorldSectionInput::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		char conv[256];

		sprintf( conv, "%d", m_ID);

		m_IDCtrl.SetWindowText( conv);

		m_NameCtrl.SetWindowText( m_szName);

		if( m_bCanEditID)
		{
			m_IDCtrl.EnableWindow( TRUE);
		}
		else
		{
			m_IDCtrl.EnableWindow( FALSE);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWorldSectionInput::OnOK()
{
	char conv[256];

	m_IDCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_ID = atoi( conv);

	m_NameCtrl.GetWindowText( m_szName, sizeof(m_szName) - 1);

	CDialog::OnOK();
}

void CDlgWorldSectionInput::OnCancel()
{
	CDialog::OnCancel();
}
