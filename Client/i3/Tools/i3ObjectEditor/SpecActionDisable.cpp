// SpecActionDisable.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "SpecActionDisable.h"
#include ".\specactiondisable.h"
#include "i3Base/string/ext/ftoa.h"

// SpecActionDisable dialog

IMPLEMENT_DYNAMIC(SpecActionDisable, i3TDKDialogBase)
SpecActionDisable::SpecActionDisable(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(SpecActionDisable::IDD, pParent)
{
	m_pAction = NULL;
}

SpecActionDisable::~SpecActionDisable()
{
}

void SpecActionDisable::SetObject( i3ElementBase * pObj)
{
	char conv[256];
	m_pAction = (i3ActionDisable *) pObj;

	if( m_pAction != NULL)
	{
		if( m_pAction->getEnableState())
			m_CHK_Enable.SetCheck( BST_CHECKED);
		else
			m_CHK_Enable.SetCheck( BST_UNCHECKED);

		i3::ftoa( m_pAction->getFadeTime(), conv);
		m_ED_FadeTime.SetWindowText( conv );
	}
}

void SpecActionDisable::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_ENABLE, m_CHK_Enable);
	DDX_Control(pDX, IDC_ED_FADETIME, m_ED_FadeTime);
}


BEGIN_MESSAGE_MAP(SpecActionDisable, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_CHK_ENABLE, OnBnClickedChkEnable)
	ON_EN_KILLFOCUS(IDC_ED_FADETIME, OnEnKillfocusEdFadetime)
END_MESSAGE_MAP()


// SpecActionDisable message handlers

void SpecActionDisable::OnBnClickedChkEnable()
{
	if( m_pAction != NULL)
	{
		if( m_CHK_Enable.GetCheck() == BST_CHECKED)
			m_pAction->setEnableState( true);
		else
			m_pAction->setEnableState( false);
	}
}

void SpecActionDisable::OnEnKillfocusEdFadetime()
{
	char conv[256];

	if( m_pAction != NULL )
	{
		m_ED_FadeTime.GetWindowText( conv, sizeof(conv) - 1 );
		m_pAction->setFadeTime( (REAL32) atof( conv) );
	}
}

void SpecActionDisable::OnOK(void)
{
}

void SpecActionDisable::OnCancel(void)
{
}
