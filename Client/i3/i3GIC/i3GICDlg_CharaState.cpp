// i3GICDlg_CharaState.cpp : implementation file
//

#include "stdafx.h"
#include "i3GIC.h"
#include "i3GICDlg_CharaState.h"
#include "i3GICApp.h"

// i3GICDlg_CharaState dialog

IMPLEMENT_DYNAMIC(i3GICDlg_CharaState, CDialog)


void i3GICDlg_CharaState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_FLYING, m_CHK_Flying);
	DDX_Control(pDX, IDC_CHECK2, m_CHK_Falling);
	DDX_Control(pDX, IDC_CHECK3, m_CHK_TouchWall);
}

bool i3GICDlg_CharaState::Execute( UINT32 state)
{
	m_State = state;

	HINSTANCE hOld = AfxGetResourceHandle();

	AfxSetResourceHandle( g_hInstGIC);

	INT32 rv = DoModal();

	AfxSetResourceHandle( hOld);

	if( rv == IDOK)
		return true;

	return false;
}

BEGIN_MESSAGE_MAP(i3GICDlg_CharaState, CDialog)
END_MESSAGE_MAP()


// i3GICDlg_CharaState message handlers

BOOL i3GICDlg_CharaState::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		if( m_State & I3_CHARA_STATE_FLYING)
			m_CHK_Flying.SetCheck( BST_CHECKED);
		else
			m_CHK_Flying.SetCheck( BST_UNCHECKED);

		if( m_State & I3_CHARA_STATE_FALL)
			m_CHK_Falling.SetCheck( BST_CHECKED);
		else
			m_CHK_Falling.SetCheck( BST_UNCHECKED);

		if( m_State & I3_CHARA_STATE_TOUCHWALL)
			m_CHK_TouchWall.SetCheck( BST_CHECKED);
		else
			m_CHK_TouchWall.SetCheck( BST_UNCHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3GICDlg_CharaState::OnOK()
{
	m_State = 0;

	if( m_CHK_Flying.GetCheck() == BST_CHECKED)
		m_State |= I3_CHARA_STATE_FLYING;

	if( m_CHK_Falling.GetCheck() == BST_CHECKED)
		m_State |= I3_CHARA_STATE_FALL;

	if( m_CHK_TouchWall.GetCheck() == BST_CHECKED)
		m_State |= I3_CHARA_STATE_TOUCHWALL;

	CDialog::OnOK();
}
