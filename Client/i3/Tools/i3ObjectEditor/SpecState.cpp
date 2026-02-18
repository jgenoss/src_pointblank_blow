// SpecState.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "SpecState.h"
#include "ObjectState.h"
#include "GlobalVar.h"
#include ".\specstate.h"
#include "i3Base/string/ext/ftoa.h"

// CSpecState dialog

IMPLEMENT_DYNAMIC(CSpecState, i3TDKDialogBase)
CSpecState::CSpecState(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecState::IDD, pParent)
{
}

CSpecState::~CSpecState()
{
}

void CSpecState::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_HP, m_HPCtrl);
	DDX_Control(pDX, IDC_ED_SG, m_SgCtrl);
	DDX_Control(pDX, IDC_CHK_SHADOW_CASTER, m_CHK_ShadowCaster);
}

void CSpecState::SetObject( i3ElementBase * pObj)
{
	I3ASSERT( i3::kind_of<CObjectState*>(pObj)); //->IsTypeOf( CObjectState::static_meta()));

	m_pObject = pObj;
	m_pState = (CObjectState *) pObj;

	if( pObj == NULL)
	{
		m_HPCtrl.SetWindowText( "");
		m_SgCtrl.SetWindowText( "");

		g_pFramework->setEmitEffectIconEnable( false);
	}
	else
	{
		CObjectState * pState = (CObjectState *) pObj;
		char conv[128];

		i3::ftoa( pState->getHPRatio(), conv);
		m_HPCtrl.SetWindowText( conv);

		i3SceneGraphInfo * pRes = pState->getSgResource();

		if( pRes == NULL)
		{
			m_SgCtrl.SetWindowText( "(None)");
			m_CHK_ShadowCaster.SetCheck( BST_UNCHECKED);
		}
		else
		{
			m_SgCtrl.SetWindowText( pRes->GetName());

			if( pState->getShadowCastEnable())
				m_CHK_ShadowCaster.SetCheck( BST_CHECKED);
			else
				m_CHK_ShadowCaster.SetCheck( BST_UNCHECKED);
		}

		// Event 중에 Emit Effect가 있는지 확인하고 있다면 Icon 표시한다.
		INT32 i;
		bool bEmitEffectIcon = false;
		
		for( i = 0; i < m_pState->getEventCount(); i++)
		{
			i3TimeEventInfo * pInfo = m_pState->getEvent( i);

			if( i3::same_of<i3ActionEmitEffect*>(pInfo->getAction())) //->IsExactTypeOf( i3ActionEmitEffect::static_meta()))
			{
				bEmitEffectIcon = true;
			}
		}

		g_pFramework->setEmitEffectIconEnable( bEmitEffectIcon);
	}
}

BEGIN_MESSAGE_MAP(CSpecState, i3TDKDialogBase)
	ON_EN_KILLFOCUS(IDC_ED_HP, OnEnKillfocusEdHp)
	ON_BN_CLICKED(IDC_BTN_SG, OnBnClickedBtnSg)
	ON_BN_CLICKED(IDC_CHK_SHADOW_CASTER, OnBnClickedChkShadowCaster)
END_MESSAGE_MAP()


// CSpecState message handlers

void CSpecState::OnCancel()
{
}

void CSpecState::OnOK()
{
	if( m_HPCtrl.GetModify())
	{
		OnEnKillfocusEdHp();
	}
}

void CSpecState::OnEnKillfocusEdHp()
{
	CObjectState * pState = (CObjectState *) getObject();

	if( pState == NULL)
		return;

	char conv[128];

	m_HPCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pState->setHPRatio( (REAL32) atof( conv));

	m_HPCtrl.SetModify( FALSE);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pState);
}

void CSpecState::OnBnClickedBtnSg()
{
	CObjectState * pState = (CObjectState *) getObject();
	if( pState == NULL)
		return;

	CFileDialog	Dlg( TRUE, "i3Engine Scene-Graph File", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Scene-Graph File(*.I3S)|*.I3S||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	g_pObject->ChangeStateSg( pState, LPCTSTR( Dlg.GetPathName()));
}

void CSpecState::OnBnClickedChkShadowCaster()
{
	CObjectState * pState = (CObjectState *) getObject();
	if( pState == NULL)
		return;

	if( m_CHK_ShadowCaster.GetCheck() == BST_CHECKED)
		pState->setShadowCastEnable( true);
	else
		pState->setShadowCastEnable( false);
}
