// PanelSpec.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "PanelSpec.h"
#include "ObjectState.h"


// CPanelSpec dialog

IMPLEMENT_DYNAMIC(CPanelSpec, i3TDKDialogContainer)
CPanelSpec::CPanelSpec(CWnd* pParent /*=NULL*/)
	: i3TDKDialogContainer(CPanelSpec::IDD, pParent)
{
}

CPanelSpec::~CPanelSpec()
{
}

void CPanelSpec::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogContainer::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelSpec, i3TDKDialogContainer)
END_MESSAGE_MAP()


// CPanelSpec message handlers

BOOL CPanelSpec::OnInitDialog()
{
	i3TDKDialogContainer::OnInitDialog();

	{
		m_SpecState.Create( CSpecState::IDD, this);
		AddSpecDialog( CObjectState::static_meta(), &m_SpecState);

		m_SpecObject.Create( CSpecObject::IDD, this);
		AddSpecDialog( CObjectTemplate::static_meta(), &m_SpecObject);

		m_SpecActionEmitEffect.Create( CSpecActionEmitEffect::IDD, this);
		AddSpecDialog( i3ActionEmitEffect::static_meta(), &m_SpecActionEmitEffect);

		m_SpecActionVisible.Create( CSpecActionVisible::IDD, this);
		AddSpecDialog( i3ActionVisible::static_meta(), &m_SpecActionVisible);

		m_SpecActionDynamicState.Create( CSpecActionDynamicState::IDD, this);
		AddSpecDialog( i3ActionDynamicState::static_meta(), &m_SpecActionDynamicState);

		m_SpecActionPath.Create( SpecActionPath::IDD, this);
		AddSpecDialog( i3ActionPlayAnim::static_meta(), &m_SpecActionPath);

		m_SpecActionExplosion.Create( CSpecActionExplosion::IDD, this);
		AddSpecDialog( i3ActionExplosion::static_meta(), &m_SpecActionExplosion);

		m_SpecActionSound.Create( CSpecActionSound::IDD, this);
		AddSpecDialog( i3ActionPlaySound::static_meta(), &m_SpecActionSound);

		m_SpecActionDisable.Create( SpecActionDisable::IDD, this);
		AddSpecDialog( i3ActionDisable::static_meta(), &m_SpecActionDisable);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelSpec::OnOK()
{
}

void CPanelSpec::OnCancel()
{
}
