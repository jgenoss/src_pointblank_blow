// SpecActionDynamicState.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "SpecActionDynamicState.h"
#include ".\specactiondynamicstate.h"


// CSpecActionDynamicState dialog

IMPLEMENT_DYNAMIC(CSpecActionDynamicState, i3TDKDialogBase)
CSpecActionDynamicState::CSpecActionDynamicState(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecActionDynamicState::IDD, pParent)
{
	m_pAction = NULL;
}

CSpecActionDynamicState::~CSpecActionDynamicState()
{
}

void CSpecActionDynamicState::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_DYNAMIC_STATE, m_DynamicStateCtrl);
}

void CSpecActionDynamicState::SetObject( i3ElementBase * pObj)
{
	m_pAction = (i3ActionDynamicState *) pObj;

	// Dynamic State
	m_DynamicStateCtrl.SetCurSel( (INT32) m_pAction->getState());
}

BEGIN_MESSAGE_MAP(CSpecActionDynamicState, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_CB_DYNAMIC_STATE, OnCbnSelchangeCbDynamicState)
END_MESSAGE_MAP()


// CSpecActionDynamicState message handlers

void CSpecActionDynamicState::OnCbnSelchangeCbDynamicState()
{
	if( m_pAction != NULL)
	{
		m_pAction->setState( (I3GAME_DYNAMIC_STATE) m_DynamicStateCtrl.GetCurSel());
	}
}

void CSpecActionDynamicState::OnOK(void)
{
}

void CSpecActionDynamicState::OnCancel(void)
{
}
