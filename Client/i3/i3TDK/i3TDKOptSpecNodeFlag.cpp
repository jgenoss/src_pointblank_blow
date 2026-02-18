// i3TDKOptSpecNodeFlag.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKOptSpecNodeFlag.h"
#include ".\i3tdkoptspecnodeflag.h"


// i3TDKOptSpecNodeFlag dialog

IMPLEMENT_DYNAMIC(i3TDKOptSpecNodeFlag, i3TDKOptSpecBase)

void i3TDKOptSpecNodeFlag::DoDataExchange(CDataExchange* pDX)
{
	i3TDKOptSpecBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_DISABLE, m_CHK_Disable);
	DDX_Control(pDX, IDC_CHK_DISABLE2, m_CHK_Disable2);
	DDX_Control(pDX, IDC_TDK_CHK_INVISIBLE, m_CHK_Invisible);
	DDX_Control(pDX, IDC_CHK_INVISIBLE2, m_CHK_Invisible2);
	DDX_Control(pDX, IDC_CHK_MUSTBEPROCESS, m_CHK_MustBeProcess);
	DDX_Control(pDX, IDC_CHK_MUSTBEPROCESS2, m_CHK_MustBeProcess2);
	DDX_Control(pDX, IDC_CHK_TRANSFORM, m_CHK_Transform);
	DDX_Control(pDX, IDC_CHK_TRANSFORM2, m_CHK_Transform2);
	DDX_Control(pDX, IDC_CHK_TRANSPARENT, m_CHK_Transparent);
	DDX_Control(pDX, IDC_CHK_TRANSPARENT2, m_CHK_Transparent2);
	DDX_Control(pDX, IDC_CHK_STATIC, m_CHK_Static);
	DDX_Control(pDX, IDC_CHK_STATIC2, m_CHK_Static2);
	DDX_Control(pDX, IDC_TDK_CHK_VOLATILE, m_CHK_Volatile);
	DDX_Control(pDX, IDC_CHK_VOLATILE2, m_CHK_Volatile2);
	DDX_Control(pDX, IDC_TDK_CHK_CONTROL, m_CHK_Control);
	DDX_Control(pDX, IDC_CHK_CONTROL2, m_CHK_Control2);
	DDX_Control(pDX, IDC_TDK_CHK_MANUAL_UPDATE, m_CHK_ManualUpdate);
	DDX_Control(pDX, IDC_CHK_MANUAL_UPDATE2, m_CHK_ManualUpdate2);
	DDX_Control(pDX, IDC_CHK_DISABLE_SHADOW_CAST, m_CHK_DisableShadowCast);
	DDX_Control(pDX, IDC_CHK_DISABLE_SHADOW_CAST2, m_CHK_DisableShadowCast2);
	DDX_Control(pDX, IDC_CHK_DISABLE_SHADOW_RECEIVE, m_CHK_DisableShadowReceive);
	DDX_Control(pDX, IDC_CHK_DISABLE_SHADOW_RECEIVE2, m_CHK_DisableShadowReceive2);
	DDX_Control(pDX, IDC_CHK_NOWORK, m_CHK_NoWork);
	DDX_Control(pDX, IDC_CHK_NOWORK2, m_CHK_NoWork2);
}


BEGIN_MESSAGE_MAP(i3TDKOptSpecNodeFlag, i3TDKOptSpecBase)
END_MESSAGE_MAP()


static void SetChkBtnByFlag(UINT32 givenFlag, UINT32 fl, CButton& btn)
{
	if(givenFlag & fl)
		btn.SetCheck(BST_CHECKED);
	else
		btn.SetCheck(BST_UNCHECKED);
}


// i3TDKOptSpecNodeFlag message handlers

BOOL i3TDKOptSpecNodeFlag::OnInitDialog()
{
	i3TDKOptSpecBase::OnInitDialog();

	if(m_pOpt != nullptr && i3::same_of<i3OptNodeFlag*>(m_pOpt))
	{
		i3OptNodeFlag* nodeFlag = (i3OptNodeFlag*)m_pOpt;

		// Add Masks
		{
			UINT32 af = nodeFlag->getAddFlag();

			SetChkBtnByFlag(af, I3_NODEFLAG_DISABLE,					m_CHK_Disable);
			SetChkBtnByFlag(af, I3_NODEFLAG_INVISIBLE,					m_CHK_Invisible);
			SetChkBtnByFlag(af, I3_NODEFLAG_MUSTBEPROCESS,				m_CHK_MustBeProcess);
			SetChkBtnByFlag(af, I3_NODEFLAG_TRANSFORM,					m_CHK_Transform);
			SetChkBtnByFlag(af, I3_NODEFLAG_TRANSPARENT,				m_CHK_Transparent);
			SetChkBtnByFlag(af, I3_NODEFLAG_STATIC,						m_CHK_Static);
			SetChkBtnByFlag(af, I3_NODEFLAG_VOLATILE,					m_CHK_Volatile);
			SetChkBtnByFlag(af, I3_NODEFLAG_CONTROL,					m_CHK_Control);
			SetChkBtnByFlag(af, I3_NODEFLAG_MANUAL_UPDATE,				m_CHK_ManualUpdate);
			SetChkBtnByFlag(af, I3_NODEFLAG_DISABLE_SHADOW_CASTER,		m_CHK_DisableShadowCast);
			SetChkBtnByFlag(af, I3_NODEFLAG_DISABLE_SHADOW_RECEIVER,	m_CHK_DisableShadowReceive);
			SetChkBtnByFlag(af, I3_NODEFLAG_NO_WORK,					m_CHK_NoWork);
		}

		// Remove Masks
		{
			UINT32 rf = nodeFlag->getRemoveFlag();

			SetChkBtnByFlag(rf, I3_NODEFLAG_DISABLE,					m_CHK_Disable2);
			SetChkBtnByFlag(rf, I3_NODEFLAG_INVISIBLE,					m_CHK_Invisible2);
			SetChkBtnByFlag(rf, I3_NODEFLAG_MUSTBEPROCESS,				m_CHK_MustBeProcess2);
			SetChkBtnByFlag(rf, I3_NODEFLAG_TRANSFORM,					m_CHK_Transform2);
			SetChkBtnByFlag(rf, I3_NODEFLAG_TRANSPARENT,				m_CHK_Transparent2);
			SetChkBtnByFlag(rf, I3_NODEFLAG_STATIC,						m_CHK_Static2);
			SetChkBtnByFlag(rf, I3_NODEFLAG_VOLATILE,					m_CHK_Volatile2);
			SetChkBtnByFlag(rf, I3_NODEFLAG_CONTROL,					m_CHK_Control2);
			SetChkBtnByFlag(rf, I3_NODEFLAG_MANUAL_UPDATE,				m_CHK_ManualUpdate2);
			SetChkBtnByFlag(rf, I3_NODEFLAG_DISABLE_SHADOW_CASTER,		m_CHK_DisableShadowCast2);
			SetChkBtnByFlag(rf, I3_NODEFLAG_DISABLE_SHADOW_RECEIVER,	m_CHK_DisableShadowReceive2);
			SetChkBtnByFlag(rf, I3_NODEFLAG_NO_WORK,					m_CHK_NoWork2);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKOptSpecNodeFlag::OnOK()
{
	i3OptNodeFlag * pOpt = (i3OptNodeFlag *) m_pOpt;

	// Add Masks
	{
		UINT32 mask = 0;

		// Disable
		if( m_CHK_Disable.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_DISABLE;

		// Invisiable
		if( m_CHK_Invisible.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_INVISIBLE;

		// Must be processed
		if( m_CHK_MustBeProcess.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_MUSTBEPROCESS;

		// Transform
		if( m_CHK_Transform.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_TRANSFORM;

		// Transparent
		if( m_CHK_Transparent.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_TRANSPARENT;

		// Static
		if( m_CHK_Static.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_STATIC;

		// Volatile
		if( m_CHK_Volatile.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_VOLATILE;

		// Control
		if( m_CHK_Control.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_CONTROL;

		// Manual Update
		if( m_CHK_ManualUpdate.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_MANUAL_UPDATE;

		// Disable Shadow Casting
		if( m_CHK_DisableShadowCast.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_DISABLE_SHADOW_CASTER;

		// Disable Shadow Receiving
		if( m_CHK_DisableShadowReceive.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_DISABLE_SHADOW_RECEIVER;

		// No Work
		if( m_CHK_NoWork.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_NO_WORK;

		pOpt->setAddFlag( mask);
	}

	// Remove Masks
	{
		UINT32 mask = 0;

		// Disable
		if( m_CHK_Disable2.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_DISABLE;

		// Invisiable
		if( m_CHK_Invisible2.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_INVISIBLE;

		// Must be processed
		if( m_CHK_MustBeProcess2.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_MUSTBEPROCESS;

		// Transform
		if( m_CHK_Transform2.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_TRANSFORM;

		// Transparent
		if( m_CHK_Transparent2.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_TRANSPARENT;

		// Static
		if( m_CHK_Static2.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_STATIC;

		// Volatile
		if( m_CHK_Volatile2.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_VOLATILE;

		// Control
		if( m_CHK_Control2.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_CONTROL;

		// Manual Update
		if( m_CHK_ManualUpdate2.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_MANUAL_UPDATE;

		// Disable Shadow Casting
		if( m_CHK_DisableShadowCast2.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_DISABLE_SHADOW_CASTER;

		// Disable Shadow Receiving
		if( m_CHK_DisableShadowReceive2.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_DISABLE_SHADOW_RECEIVER;

		// No Work
		if( m_CHK_NoWork2.GetCheck() == BST_CHECKED)
			mask |= I3_NODEFLAG_NO_WORK;

		pOpt->setRemoveFlag( mask);
	}

	i3TDKOptSpecBase::OnOK();
}

