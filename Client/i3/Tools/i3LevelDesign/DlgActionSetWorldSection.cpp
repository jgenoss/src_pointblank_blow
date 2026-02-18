// DlgActionSetWorldSection.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgActionSetWorldSection.h"
#include ".\dlgactionsetworldsection.h"


// CDlgActionSetWorldSection dialog

IMPLEMENT_DYNAMIC(CDlgActionSetWorldSection, CDialog)
CDlgActionSetWorldSection::CDlgActionSetWorldSection(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgActionSetWorldSection::IDD, pParent)
{
}

CDlgActionSetWorldSection::~CDlgActionSetWorldSection()
{
}

void CDlgActionSetWorldSection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_WORLD_SECTION, m_ED_Section);
}

bool CDlgActionSetWorldSection::Execute(  i3ActionSetWorldSection * pAction)
{
	m_pAction = pAction;

	if( DoModal() == IDOK)
		return true;

	return false;
}

BEGIN_MESSAGE_MAP(CDlgActionSetWorldSection, CDialog)
END_MESSAGE_MAP()


// CDlgActionSetWorldSection message handlers

void CDlgActionSetWorldSection::OnOK()
{
	char conv[256];

	if( m_pAction != NULL)
	{
		m_ED_Section.GetWindowText( conv, sizeof(conv) - 1);

		m_pAction->setWorldSection( atoi( conv));
	}

	CDialog::OnOK();
}

BOOL CDlgActionSetWorldSection::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( m_pAction != NULL)
	{
		char conv[128];

		sprintf( conv, "%d", m_pAction->getWorldSection());

		m_ED_Section.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
