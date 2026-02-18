// DlgSelectResolution.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "DlgSelectResolution.h"
#include ".\dlgselectresolution.h"


// CDlgSelectResolution dialog

IMPLEMENT_DYNAMIC(CDlgSelectResolution, CDialog)
CDlgSelectResolution::CDlgSelectResolution(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectResolution::IDD, pParent)
{
	m_nResolutionWidth = 1024;
	m_nResolutionHeight = 768;
}

CDlgSelectResolution::~CDlgSelectResolution()
{
}

void CDlgSelectResolution::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESOLUTION_WIDTH, m_edResolutionWidth);
	DDX_Control(pDX, IDC_RESOLUTION_HEIGHT, m_edResolutionHeight);
}


BEGIN_MESSAGE_MAP(CDlgSelectResolution, CDialog)
	ON_EN_KILLFOCUS(IDC_RESOLUTION_HEIGHT, OnEnKillfocusResolutionHeight)
	ON_EN_KILLFOCUS(IDC_RESOLUTION_WIDTH, OnEnKillfocusResolutionWidth)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSelectResolution message handlers

BOOL CDlgSelectResolution::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	char conv[MAX_PATH];

	sprintf( conv, "%d", m_nResolutionWidth);
	m_edResolutionWidth.SetWindowText( conv);

	sprintf( conv, "%d", m_nResolutionHeight);
	m_edResolutionHeight.SetWindowText( conv);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectResolution::OnEnKillfocusResolutionHeight()
{
	// TODO: Add your control notification handler code here
	
	if( m_edResolutionHeight.GetModify())
	{
		char conv[MAX_PATH];
		m_edResolutionHeight.GetWindowText( conv, MAX_PATH -1);

		m_nResolutionHeight = atoi( conv);
	}
}

void CDlgSelectResolution::OnEnKillfocusResolutionWidth()
{
	// TODO: Add your control notification handler code here
	if( m_edResolutionWidth.GetModify())
	{
		char conv[MAX_PATH];
		m_edResolutionWidth.GetWindowText( conv, MAX_PATH -1);

		m_nResolutionWidth = atoi( conv);
	}
}

void CDlgSelectResolution::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
