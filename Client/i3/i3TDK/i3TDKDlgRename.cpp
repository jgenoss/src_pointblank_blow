// SelectNodeTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKDlgRename.h"
#include "resource.h"
#include "i3TDKGlobalRes.h"

// i3TDKDlgRename dialog

IMPLEMENT_DYNAMIC(i3TDKDlgRename, i3TDKOptSpecBase)

BOOL i3TDKDlgRename::Execute( const TCHAR * pszHelp, const TCHAR* pszInitName)
{
	if( pszInitName != nullptr)
		m_strName = pszInitName;
	else
		m_strName.clear();

	m_pszHelpText = pszHelp;

	i3TDK::SetResInstance();

	if( DoModal() == IDCANCEL)
	{
		i3TDK::RestoreResInstance();
		return FALSE;
	}
	i3TDK::RestoreResInstance();

	return TRUE;
}

void i3TDKDlgRename::DoDataExchange(CDataExchange* pDX)
{
	i3TDKOptSpecBase::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_TDK_NAME,	m_NameCtrl);
	DDX_Control(pDX, IDC_NARRATION, m_HelpTextCtrl);
}

BEGIN_MESSAGE_MAP(i3TDKDlgRename, i3TDKOptSpecBase)
END_MESSAGE_MAP()


// i3TDKDlgRename message handlers

BOOL i3TDKDlgRename::OnInitDialog()
{
	i3TDKOptSpecBase::OnInitDialog();

	{
		m_NameCtrl.SetWindowText( m_strName.c_str());

		if( m_pszHelpText != nullptr)
		{
			m_HelpTextCtrl.SetWindowText( m_pszHelpText);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKDlgRename::OnOK()
{
	TCHAR strTmp[MAX_PATH];

	m_NameCtrl.GetWindowText( strTmp, _countof(strTmp) );	
	
	m_strName = strTmp;

	i3TDKOptSpecBase::OnOK();
}
