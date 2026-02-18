// ParticleSpecRotate.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "ParticleSpecRotate.h"
#include ".\particlespecrotate.h"
#include "GlobalVariable.h"
#include "i3EffectEditorDlg.h"
#include "Util.h"
#include "i3Base/string/ext/ftoa.h"

// CParticleSpecRotate dialog

IMPLEMENT_DYNAMIC(CParticleSpecRotate, i3TDKDialogBase)
CParticleSpecRotate::CParticleSpecRotate(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CParticleSpecRotate::IDD, pParent)
{
	m_pInfo = NULL;
}

CParticleSpecRotate::~CParticleSpecRotate()
{
}

void CParticleSpecRotate::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_ROT_ARG0_TYPE, m_FuncCtrl);
	DDX_Control(pDX, IDC_ED_ROT_ARG0_CONST, m_InitCtrl);
	DDX_Control(pDX, IDC_ED_ROT_ARG0_FACTOR, m_FactorCtrl);
	DDX_Control(pDX, IDC_BTN_ROT_ARG0_TABLE, m_GraphCtrl);
	DDX_Control(pDX, IDC_RANDOM_VARIATION, m_RandomFactorCtrl);
	DDX_Control(pDX, IDC_SPIN_POS_ARG0_CONST, m_Arg0ConstSpinCtrl);
	DDX_Control(pDX, IDC_SPIN_POS_ARG0_FACTOR, m_Arg0FactorSpinCtrl);
}

void CParticleSpecRotate::SetParticle( i3ParticleInfo * pInfo)
{
	char conv[256];

	m_pInfo = pInfo;

	if( m_pInfo != NULL)
	{
		// Function
		switch( pInfo->GetRotationFunc())
		{
			case i3ParticleInfo::FUNC_NONE :		m_FuncCtrl.SetCurSel( 0); break;
			case i3ParticleInfo::FUNC_CONSTANT :	m_FuncCtrl.SetCurSel( 1);	break;
			case i3ParticleInfo::FUNC_SIN :			m_FuncCtrl.SetCurSel( 2); break;
			case i3ParticleInfo::FUNC_COS :			m_FuncCtrl.SetCurSel( 3);	break;
			case i3ParticleInfo::FUNC_RND :			m_FuncCtrl.SetCurSel( 4);	break;
			case i3ParticleInfo::FUNC_TABLE :		m_FuncCtrl.SetCurSel( 5); break;
		}

		// Init
		REAL2STR( pInfo->GetRotationInit(), conv);
		m_InitCtrl.SetWindowText( conv);

		// Factor
		REAL2STR( pInfo->GetRotationFactor(), conv);
		m_FactorCtrl.SetWindowText( conv);

		// Random Factor
		i3::ftoa( pInfo->GetRotateRandomFactor(), conv);
		m_RandomFactorCtrl.SetWindowText( conv);
	}
	else
	{
		m_FuncCtrl.SetCurSel( -1);
		m_InitCtrl.SetWindowText( "");
		m_FactorCtrl.SetWindowText( "");
	}

	m_FuncCtrl.EnableWindow( (m_pInfo != NULL));
	m_InitCtrl.EnableWindow( (m_pInfo != NULL));
	m_FactorCtrl.EnableWindow( (m_pInfo != NULL));

	m_GraphCtrl.EnableWindow( (m_pInfo != NULL) && (m_pInfo->GetRotationTable() != NULL));
}

BEGIN_MESSAGE_MAP(CParticleSpecRotate, i3TDKDialogBase)
	ON_WM_KILLFOCUS()
	ON_CBN_SELCHANGE(IDC_CB_ROT_ARG0_TYPE, OnCbnSelchangeCbRotArg0Type)
	ON_BN_CLICKED(IDC_BTN_ROT_ARG0_TABLE, OnBnClickedBtnRotArg0Table)

	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POS_ARG0_CONST,  &CParticleSpecRotate::OnDeltaPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POS_ARG0_FACTOR,  &CParticleSpecRotate::OnDeltaPosSpin)

	ON_EN_KILLFOCUS(IDC_RANDOM_VARIATION, &CParticleSpecRotate::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_ROT_ARG0_CONST, &CParticleSpecRotate::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_ROT_ARG0_FACTOR, &CParticleSpecRotate::OnEnKillFocus)
END_MESSAGE_MAP()

// CParticleSpecRotate message handlers

BOOL CParticleSpecRotate::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	SetParticle( NULL);
	InitSpinCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CParticleSpecRotate::InitSpinCtrl()
{
	m_Arg0ConstSpinCtrl.SetBuddy(&m_InitCtrl);
	m_Arg0ConstSpinCtrl.SetRange(SPINCTRL_NEGATIVE_MAX_COUNT, SPINCTRL_MAX_COUNT);

	m_Arg0FactorSpinCtrl.SetBuddy(&m_FactorCtrl);
	m_Arg0FactorSpinCtrl.SetRange(SPINCTRL_NEGATIVE_MAX_COUNT, SPINCTRL_MAX_COUNT);
}

void CParticleSpecRotate::OnOK()
{
	char conv[256];

	if(m_pInfo == NULL)
		return;

	// Init Random Variation
	m_RandomFactorCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetRotateRandomFactor( (REAL32) atof( conv));

	// Arg0 Init
	m_InitCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetRotationInit( (REAL32) atof( conv));

	// Arg0 Factor
	m_FactorCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetRotationFactor( (REAL32) atof( conv));

	g_bNeedToCreate = TRUE;
	g_pMainWnd->StopRender();
}

void CParticleSpecRotate::OnCancel()
{
}

void CParticleSpecRotate::OnEnKillFocus()
{
	OnOK();
}

void CParticleSpecRotate::OnDeltaPosSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnOK();
}

void CParticleSpecRotate::OnCbnSelchangeCbRotArg0Type()
{
	// Arg0 Function
	switch( m_FuncCtrl.GetCurSel())
	{
		case 0 :	m_pInfo->SetRotationFunc( i3ParticleInfo::FUNC_NONE);	break;
		case 1 :	m_pInfo->SetRotationFunc( i3ParticleInfo::FUNC_CONSTANT);	break;
		case 2 :	m_pInfo->SetRotationFunc( i3ParticleInfo::FUNC_SIN);	break;
		case 3 :	m_pInfo->SetRotationFunc( i3ParticleInfo::FUNC_COS);	break;
		case 4 :	m_pInfo->SetRotationFunc( i3ParticleInfo::FUNC_RND);	break;
		case 5 :	
			m_pInfo->SetRotationFunc( i3ParticleInfo::FUNC_TABLE);	

			if( m_pInfo->GetRotationTable() == NULL)
			{
				i3KeyframeTable * pNew = CreateKeyframeTable( 10);

				m_pInfo->SetRotationTable( pNew);

				m_GraphCtrl.EnableWindow( TRUE);
			}
			break;
	}
}

void CParticleSpecRotate::OnBnClickedBtnRotArg0Table()
{
	g_pMainWnd->Edit( m_pInfo->GetRotationTable(), 0);
}
