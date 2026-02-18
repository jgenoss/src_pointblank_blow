// SpecActionVisible.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "SpecActionVisible.h"
#include ".\specactionvisible.h"
#include "i3Base/string/ext/ftoa.h"

// CSpecActionVisible dialog

IMPLEMENT_DYNAMIC(CSpecActionVisible, i3TDKDialogBase)
CSpecActionVisible::CSpecActionVisible(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecActionVisible::IDD, pParent)
{
	m_pAction = NULL;
}

CSpecActionVisible::~CSpecActionVisible()
{
}

void CSpecActionVisible::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_ENABLE, m_EnableCtrl);
	DDX_Control(pDX, IDC_EDIT_ALPHATIME, m_EditAlphaTimeCtrl );
}

void CSpecActionVisible::SetObject( i3ElementBase * pObj)
{
	char conv[256];
	m_pAction = (i3ActionVisible *) pObj;

	if( m_pAction != NULL)
	{
		if( m_pAction->getEnable())
			m_EnableCtrl.SetCheck( BST_CHECKED);
		else
			m_EnableCtrl.SetCheck( BST_UNCHECKED);

		i3::ftoa( m_pAction->getAlphaTime(), conv);
		m_EditAlphaTimeCtrl.SetWindowText( conv );
	}
}

BEGIN_MESSAGE_MAP(CSpecActionVisible, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_CHK_ENABLE, OnBnClickedChkEnable)
	ON_EN_CHANGE(IDC_EDIT_ALPHATIME, OnEnChangeEditAlphatime)
END_MESSAGE_MAP()


// CSpecActionVisible message handlers

void CSpecActionVisible::OnBnClickedChkEnable()
{
	if( m_pAction != NULL)
	{
		if( m_EnableCtrl.GetCheck() == BST_CHECKED)
			m_pAction->setEnable( true);
		else
			m_pAction->setEnable( false);
	}
}

void CSpecActionVisible::OnEnChangeEditAlphatime()
{
	char conv[256];

	if( m_pAction != NULL )
	{
		m_EditAlphaTimeCtrl.GetWindowText( conv, sizeof(conv) - 1 );
		m_pAction->setAlphaTime( (REAL32) atof( conv) );
	}
}

void CSpecActionVisible::OnOK(void)
{
}

void CSpecActionVisible::OnCancel(void)
{
}

