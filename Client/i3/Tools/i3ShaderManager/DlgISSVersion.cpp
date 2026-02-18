// DlgISSVersion.cpp : implementation file
//

#include "stdafx.h"
#include "i3ShaderManager.h"
#include "DlgISSVersion.h"
#include ".\dlgissversion.h"


// CDlgISSVersion dialog

IMPLEMENT_DYNAMIC(CDlgISSVersion, CDialog)
CDlgISSVersion::CDlgISSVersion(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgISSVersion::IDD, pParent)
{
	m_pCache = NULL;
}

CDlgISSVersion::~CDlgISSVersion()
{
}

void CDlgISSVersion::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_VERSION, m_VersionCtrl);
}

bool CDlgISSVersion::Execute( i3ShaderCache * pCache)
{
	m_pCache = pCache;

	if( DoModal() == IDOK)
		return true;

	return false;
}

BEGIN_MESSAGE_MAP(CDlgISSVersion, CDialog)
END_MESSAGE_MAP()


// CDlgISSVersion message handlers

BOOL CDlgISSVersion::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		char conv[256];

		sprintf( conv, "%d", m_pCache->getISSVersion());

		m_VersionCtrl.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgISSVersion::OnOK()
{
	{
		char conv[256];

		m_VersionCtrl.GetWindowText( conv, sizeof(conv) - 1);

		m_pCache->setISSVersion( (UINT32) atoi( conv));
	}

	CDialog::OnOK();
}
