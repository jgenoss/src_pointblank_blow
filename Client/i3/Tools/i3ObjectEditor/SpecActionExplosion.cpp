// SpecActionExplosion.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "SpecActionExplosion.h"
#include ".\specactionexplosion.h"
#include "MainFrm.h"
#include "GlobalVar.h"
#include "i3Base/string/ext/ftoa.h"

// CSpecActionExplosion dialog

IMPLEMENT_DYNAMIC(CSpecActionExplosion, i3TDKDialogBase)
CSpecActionExplosion::CSpecActionExplosion(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecActionExplosion::IDD, pParent)
{
	m_pAction = NULL;
}

CSpecActionExplosion::~CSpecActionExplosion()
{
}

void CSpecActionExplosion::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_RANGE, m_RageCtrl);
	DDX_Control(pDX, IDC_ED_DAMAGE, m_DamageCtrl);
	DDX_Control(pDX, IDC_ED_FORCE, m_ForceCtrl);
	DDX_Control(pDX, IDC_ED_SLOPE, m_SlopeCtrl);
	DDX_Control(pDX, IDC_CHK_ATTACH, m_AttachEnableCtrl);
	DDX_Control(pDX, IDC_CB_BONELIST, m_BoneCBCtrl);
}

void	CSpecActionExplosion::SetObject( i3ElementBase * pObj)
{
	char conv[256];

	m_pAction = (i3ActionExplosion *) pObj;

	// Range
	i3::ftoa( m_pAction->getRange(), conv);
	m_RageCtrl.SetWindowText( conv);

	// Damage
	i3::ftoa( m_pAction->getDamage(), conv);
	m_DamageCtrl.SetWindowText( conv);

	// Force
	i3::ftoa( m_pAction->getForce(), conv);
	m_ForceCtrl.SetWindowText( conv);

	// Slope
	i3::ftoa( m_pAction->getSlope(), conv);
	m_SlopeCtrl.SetWindowText( conv);

	m_RageCtrl.SetModify( FALSE);
	m_DamageCtrl.SetModify( FALSE);
	m_ForceCtrl.SetModify( FALSE);
	m_SlopeCtrl.SetModify( FALSE);

	// Bone 이름 추가
	{
		CMainFrame * pFrameWnd = (CMainFrame *) AfxGetMainWnd();

		m_BoneCBCtrl.ResetContent();
		m_BoneCBCtrl.SetCurSel( -1);

		INT32 idxState = pFrameWnd->getSelectedState();
		//if( idxState == -1) idxState = 0;

		if( idxState != -1)
		{
			CObjectState * pState = g_pObject->getState( idxState);

			i3::vector<i3Node*> list;

			i3SceneGraphInfo * pSgInfo = pState->getSgResource();

			while((pSgInfo == NULL) && (idxState > 0))
			{
				idxState --;
				pState = g_pObject->getState( idxState);

				pSgInfo = pState->getSgResource();
			}

			if( pSgInfo != NULL)
			{
				i3Scene::FindNodeByType( pSgInfo->getInstanceSg(), i3Transform::static_meta(), list);

				for( size_t i = 0; i < list.size(); i++)
				{
					i3Transform * pTrans = (i3Transform *) list[i];

					if( (pTrans->hasName()) && (pTrans->IsFlag( I3_NODEFLAG_VOLATILE) == FALSE) && (pTrans->IsFlag( I3_NODEFLAG_CONTROL) == FALSE))
					{
						m_BoneCBCtrl.AddString( pTrans->GetName());
					}
				}
			}

			// Attached Bone Name
			if( m_pAction->getAttachBoneName() != NULL)
			{
				INT32 idx;

				idx = m_BoneCBCtrl.FindString( 0, m_pAction->getAttachBoneName());

				m_BoneCBCtrl.SetCurSel( idx);
			}
			else
			{
				m_BoneCBCtrl.SetCurSel( -1);
			}
		}
	}
}

BEGIN_MESSAGE_MAP(CSpecActionExplosion, i3TDKDialogBase)
	ON_EN_KILLFOCUS(IDC_ED_RANGE, OnEnKillfocusEdRange)
	ON_EN_KILLFOCUS(IDC_ED_DAMAGE, OnEnKillfocusEdDamage)
	ON_EN_KILLFOCUS(IDC_ED_FORCE, OnEnKillfocusEdForce)
	ON_EN_KILLFOCUS(IDC_ED_SLOPE, OnEnKillfocusEdSlope)
	ON_CBN_SELCHANGE(IDC_CB_BONELIST, OnCbnSelchangeCbBonelist)
	ON_BN_CLICKED(IDC_CHK_ATTACH, OnBnClickedChkAttach)
END_MESSAGE_MAP()


// CSpecActionExplosion message handlers

void CSpecActionExplosion::OnEnKillfocusEdRange()
{
	char conv[256];

	if( m_pAction == NULL)	return;

	m_RageCtrl.GetWindowText( conv, sizeof(conv) - 1);

	m_pAction->setRange( (REAL32) atof( conv));

	m_RageCtrl.SetModify( FALSE);
}

void CSpecActionExplosion::OnEnKillfocusEdDamage()
{
	char conv[256];

	if( m_pAction == NULL)	return;

	m_DamageCtrl.GetWindowText( conv, sizeof(conv) - 1);

	m_pAction->setDamage( (REAL32) atof( conv));

	m_DamageCtrl.SetModify( FALSE);
}

void CSpecActionExplosion::OnEnKillfocusEdForce()
{
	char conv[256];

	if( m_pAction == NULL)	return;

	m_ForceCtrl.GetWindowText( conv, sizeof(conv) - 1);

	m_pAction->setForce( (REAL32) atof( conv));

	m_ForceCtrl.SetModify( FALSE);
}

void CSpecActionExplosion::OnEnKillfocusEdSlope()
{
	char conv[256];

	if( m_pAction == NULL)	return;

	m_SlopeCtrl.GetWindowText( conv, sizeof(conv) - 1);

	m_pAction->setSlope( (REAL32) atof( conv));

	m_SlopeCtrl.SetModify( FALSE);
}

void CSpecActionExplosion::OnOK()
{
	OnEnKillfocusEdRange();
	OnEnKillfocusEdDamage();
	OnEnKillfocusEdForce();
	OnEnKillfocusEdSlope();
}

void CSpecActionExplosion::OnCancel()
{

}

void CSpecActionExplosion::OnCbnSelchangeCbBonelist()
{
	INT32 idx = m_BoneCBCtrl.GetCurSel();

	if( idx != -1)
	{
		char conv[256];

		m_BoneCBCtrl.GetLBText( idx, conv);

		m_pAction->SetAttachBoneName( conv);
	}
}

void CSpecActionExplosion::OnBnClickedChkAttach()
{
	if( m_AttachEnableCtrl.GetCheck() == BST_CHECKED)
		m_pAction->setAttachEnable( true);
	else
		m_pAction->setAttachEnable( false);
}