// DlgSetMapSectorIndex.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgSetMapSectorIndex.h"
#include ".\dlgsetmapsectorindex.h"


// CDlgSetMapSectorIndex dialog

IMPLEMENT_DYNAMIC(CDlgSetMapSectorIndex, CDialog)
CDlgSetMapSectorIndex::CDlgSetMapSectorIndex(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetMapSectorIndex::IDD, pParent)
{
}

CDlgSetMapSectorIndex::~CDlgSetMapSectorIndex()
{
}

void CDlgSetMapSectorIndex::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MAPSECTORINDEX, m_edSectorIndex);
}


BEGIN_MESSAGE_MAP(CDlgSetMapSectorIndex, CDialog)
END_MESSAGE_MAP()


// CDlgSetMapSectorIndex message handlers
bool CDlgSetMapSectorIndex::Execute(  i3ActionSetMapSector * pAction)
{
	m_pAction = pAction;

	if( DoModal() == IDOK)
		return true;

	return false;
}

BOOL CDlgSetMapSectorIndex::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( m_pAction != NULL)
	{
		char conv[128];

		sprintf( conv, "%d", m_pAction->getMapSector());

		m_edSectorIndex.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSetMapSectorIndex::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	char conv[256];

	if( m_pAction != NULL)
	{
		m_edSectorIndex.GetWindowText( conv, sizeof(conv) - 1);

		m_pAction->setMapSector( atoi( conv));
	}

	CDialog::OnOK();
}
