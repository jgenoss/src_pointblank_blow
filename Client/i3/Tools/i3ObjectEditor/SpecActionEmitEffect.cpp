// SpecActionEmitEffect.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "SpecActionEmitEffect.h"
#include "GlobalVar.h"
#include "MainFrm.h"
#include ".\specactionemiteffect.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/ftoa.h"

// CSpecActionEmitEffect dialog

IMPLEMENT_DYNAMIC(CSpecActionEmitEffect, i3TDKDialogBase)
CSpecActionEmitEffect::CSpecActionEmitEffect(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecActionEmitEffect::IDD, pParent)
{
}

CSpecActionEmitEffect::~CSpecActionEmitEffect()
{
}

void CSpecActionEmitEffect::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_EFFECT, m_EffectNameCtrl);
	DDX_Control(pDX, IDC_CHK_ATTACH, m_AttachEnableCtrl);
	DDX_Control(pDX, IDC_CB_BONELIST, m_BoneCBCtrl);
	DDX_Control(pDX, IDC_ED_OFFSET_X, m_ED_OffsetX);
	DDX_Control(pDX, IDC_ED_OFFSET_Y, m_ED_OffsetY);
	DDX_Control(pDX, IDC_ED_OFFSET_Z, m_ED_OffsetZ);
	DDX_Control(pDX, IDC_EDIT2, m_ED_Theta);
	DDX_Control(pDX, IDC_EDIT3, m_ED_Rho);
	DDX_Control(pDX, IDC_CHK_EMITTOWORLD, m_CHK_EmitToWorld);
	DDX_Control(pDX, IDC_CHK_REPEAT2, m_CHK_Repeat);
	DDX_Control(pDX, IDC_ED_REPEAT_LIFETIME, m_ED_RepeatLifeTime);
	DDX_Control(pDX, IDC_ED_REPEAT_INTERVAL, m_ED_RepeatInterval);
	DDX_Control(pDX, IDC_ED_SCALE, m_ED_Scale);
}

void CSpecActionEmitEffect::SetObject( i3ElementBase * pObj)
{
	char conv[256];

	m_pAction = (i3ActionEmitEffect *) pObj;

	// Effect Name
	if( m_pAction->getEffectName() != NULL)
	{
		m_EffectNameCtrl.SetWindowText( m_pAction->getEffectName());
	}
	else
	{
		m_EffectNameCtrl.SetWindowText( "(None)");
	}

	// Attach to bone
	if( m_pAction->getAttachEnable())
	{
		m_AttachEnableCtrl.SetCheck( BST_CHECKED);
	}
	else
	{
		m_AttachEnableCtrl.SetCheck( BST_UNCHECKED);
	}

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

				for(size_t i = 0; i < list.size(); ++i)
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

	// Emit To World
	if( m_pAction->getEmitToWorldState())
		m_CHK_EmitToWorld.SetCheck( BST_CHECKED);
	else
		m_CHK_EmitToWorld.SetCheck( BST_UNCHECKED);

	// Repeat
	if( m_pAction->isRepeat())
		m_CHK_Repeat.SetCheck( BST_CHECKED);
	else
		m_CHK_Repeat.SetCheck( BST_UNCHECKED);

	// Repeat Lifetime
	{
		i3::ftoa( m_pAction->getRepeatDuration(), conv);

		m_ED_RepeatLifeTime.SetWindowText( conv);
		m_ED_RepeatLifeTime.EnableWindow( m_pAction->isRepeat());
		m_ED_RepeatLifeTime.SetModify( FALSE);
	}

	// Repeat Interval
	{
		i3::ftoa( m_pAction->getRepeatInterval(), conv);
		
		m_ED_RepeatInterval.SetWindowText( conv);
		m_ED_RepeatInterval.EnableWindow( m_pAction->isRepeat());
		m_ED_RepeatInterval.SetModify( FALSE);
	}

	// Offset
	VEC3D * pOff = m_pAction->getOffset();

	i3::ftoa( pOff->x, conv);
	m_ED_OffsetX.SetWindowText( conv);
	m_ED_OffsetX.SetModify( FALSE);
	
	i3::ftoa( pOff->y, conv);
	m_ED_OffsetY.SetWindowText( conv);
	m_ED_OffsetY.SetModify( FALSE);
	
	i3::ftoa( pOff->z, conv);
	m_ED_OffsetZ.SetWindowText( conv);
	m_ED_OffsetZ.SetModify( FALSE);

	// Theta
	i3::ftoa( I3_RAD2DEG( m_pAction->getEmitTheta()), conv);
	m_ED_Theta.SetWindowText( conv);
	m_ED_Theta.SetModify( FALSE);

	// Rho
	i3::ftoa( I3_RAD2DEG( m_pAction->getEmitRho()), conv);
	m_ED_Rho.SetWindowText( conv);
	m_ED_Rho.SetModify( FALSE);

	// Scale
	i3::ftoa( m_pAction->getScale(), conv);
	m_ED_Scale.SetWindowText( conv);
	m_ED_Scale.SetModify( FALSE);

	g_pFramework->setEmitEffectIconPos( m_pAction->getLocalOffsetMatrix());
}

BEGIN_MESSAGE_MAP(CSpecActionEmitEffect, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_BTN_EFFECT, OnBnClickedBtnEffect)
	ON_EN_KILLFOCUS(IDC_ED_OFFSET_X, OnEnKillfocusEdOffsetX)
	ON_EN_KILLFOCUS(IDC_ED_OFFSET_Y, OnEnKillfocusEdOffsetY)
	ON_EN_KILLFOCUS(IDC_ED_OFFSET_Z, OnEnKillfocusEdOffsetZ)
	ON_EN_KILLFOCUS(IDC_EDIT2, OnEnKillfocusEdit2)
	ON_EN_KILLFOCUS(IDC_EDIT3, OnEnKillfocusEdit3)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_CB_BONELIST, OnCbnSelchangeCbBonelist)
	ON_BN_CLICKED(IDC_CHK_ATTACH, OnBnClickedChkAttach)
	ON_BN_CLICKED(IDC_CHK_EMITTOWORLD, OnBnClickedChkEmittoworld)
	ON_BN_CLICKED(IDC_CHK_REPEAT2, OnBnClickedChkRepeat2)
	ON_EN_KILLFOCUS(IDC_ED_SCALE, OnEnKillfocusEdScale)
END_MESSAGE_MAP()


// CSpecActionEmitEffect message handlers

BOOL CSpecActionEmitEffect::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// Bone List의 갱신
	{
		//g_pObject->
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpecActionEmitEffect::OnOK()
{
	char conv[256];
	bool bEdit = false;

	if( m_pAction == NULL)
		return;

	if( m_ED_OffsetX.GetModify() || m_ED_OffsetY.GetModify() || m_ED_OffsetZ.GetModify())
	{
		VEC3D off;

		// Offset X
		m_ED_OffsetX.GetWindowText( conv, sizeof(conv));
		m_ED_OffsetX.SetModify( FALSE);
		off.x = (REAL32) atof( conv);

		// Offset Y
		m_ED_OffsetY.GetWindowText( conv, sizeof(conv));
		m_ED_OffsetY.SetModify( FALSE);
		off.y = (REAL32) atof( conv);

		// Offset Z
		m_ED_OffsetZ.GetWindowText( conv, sizeof(conv));
		m_ED_OffsetZ.SetModify( FALSE);
		off.z = (REAL32) atof( conv);

		m_pAction->setOffset( &off);
		bEdit = true;
	}

	if( m_ED_Theta.GetModify())
	{
		m_ED_Theta.GetWindowText( conv, sizeof(conv));
		m_ED_Theta.SetModify( FALSE);

		m_pAction->setEmitTheta( I3_DEG2RAD( ((REAL32) atof( conv))));
		bEdit = true;
	}

	if( m_ED_Rho.GetModify())
	{
		m_ED_Rho.GetWindowText( conv, sizeof(conv));
		m_ED_Rho.SetModify( FALSE);

		m_pAction->setEmitRho( I3_DEG2RAD( ((REAL32) atof( conv))));
		bEdit = true;
	}

	if( m_ED_RepeatLifeTime.GetModify())
	{
		m_ED_RepeatLifeTime.GetWindowText( conv, sizeof(conv));
		m_ED_RepeatLifeTime.SetModify( FALSE);

		m_pAction->setRepeatDuration( (REAL32) atof( conv));
	}

	if( m_ED_RepeatInterval.GetModify())
	{
		m_ED_RepeatInterval.GetWindowText( conv, sizeof(conv));
		m_ED_RepeatInterval.SetModify( FALSE);

		m_pAction->setRepeatInterval( (REAL32) atof( conv));
	}

	OnEnKillfocusEdScale();

	if( bEdit)
	{
		g_pFramework->setEmitEffectIconPos( m_pAction->getLocalOffsetMatrix());

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pAction);
	}
}

void CSpecActionEmitEffect::OnCancel()
{
}

void CSpecActionEmitEffect::OnBnClickedBtnEffect()
{
	CFileDialog	Dlg( TRUE, "I3S", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Sg. File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	//
	INT32 idx = g_pObject->LoadEffect( LPCTSTR( Dlg.GetPathName()));

	i3SceneGraphInfo * pInfo = g_pObject->getEffect( idx);
	char szName[ MAX_PATH];

//	i3String::SplitFileName( pInfo->GetName(), szName, FALSE);
	i3::extract_filetitle(pInfo->GetName(), szName);

	m_pAction->SetEffectName( szName);

	m_EffectNameCtrl.SetWindowText( szName);
}

void CSpecActionEmitEffect::OnEnKillfocusEdOffsetX()
{
	OnOK();
}

void CSpecActionEmitEffect::OnEnKillfocusEdOffsetY()
{
	OnOK();
}

void CSpecActionEmitEffect::OnEnKillfocusEdOffsetZ()
{
	OnOK();
}

void CSpecActionEmitEffect::OnEnKillfocusEdit2()
{
	OnOK();
}

void CSpecActionEmitEffect::OnEnKillfocusEdit3()
{
	OnOK();
}

void CSpecActionEmitEffect::OnShowWindow(BOOL bShow, UINT nStatus)
{
	i3TDKDialogBase::OnShowWindow(bShow, nStatus);

}

void CSpecActionEmitEffect::OnCbnSelchangeCbBonelist()
{
	INT32 idx = m_BoneCBCtrl.GetCurSel();

	if( idx != -1)
	{
		char conv[256];

		m_BoneCBCtrl.GetLBText( idx, conv);

		m_pAction->SetAttachBoneName( conv);
	}
}

void CSpecActionEmitEffect::OnBnClickedChkAttach()
{
	if( m_AttachEnableCtrl.GetCheck() == BST_CHECKED)
		m_pAction->setAttachEnable( true);
	else
		m_pAction->setAttachEnable( false);
}

void CSpecActionEmitEffect::OnBnClickedChkEmittoworld()
{
	m_pAction->setEmitToWorldState( m_CHK_EmitToWorld.GetCheck() == BST_CHECKED);
}

void CSpecActionEmitEffect::OnBnClickedChkRepeat2()
{
	m_pAction->setRepeat( m_CHK_Repeat.GetCheck() == BST_CHECKED);

	m_ED_RepeatLifeTime.EnableWindow( m_pAction->isRepeat() == true);
	m_ED_RepeatInterval.EnableWindow( m_pAction->isRepeat() == true);
}

void CSpecActionEmitEffect::OnEnKillfocusEdScale()
{
	char conv[256];

	if( m_ED_Scale.GetModify())
	{
		m_ED_Scale.GetWindowText( conv, sizeof(conv));
		m_ED_Scale.SetModify( FALSE);

		m_pAction->setScale( (REAL32) atof( conv));
	}
}
