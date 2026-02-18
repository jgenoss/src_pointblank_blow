// ParticleSpecScale.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "ParticleSpecScale.h"
#include ".\particlespecscale.h"
#include "GlobalVariable.h"
#include "i3EffectEditorDlg.h"
#include "Util.h"
#include "i3Base/string/ext/ftoa.h"

// CParticleSpecScale dialog

IMPLEMENT_DYNAMIC(CParticleSpecScale, i3TDKDialogBase)
CParticleSpecScale::CParticleSpecScale(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CParticleSpecScale::IDD, pParent)
{
	m_pInfo = NULL;
}

CParticleSpecScale::~CParticleSpecScale()
{
}

void CParticleSpecScale::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_SCALE_ARG0_CONST2, m_Arg0ConstCtrl);
	DDX_Control(pDX, IDC_SPIN_SCALE_ARG0_CONST2, m_Arg0ConstSpinCtrl);
	DDX_Control(pDX, IDC_ED_POS_ARG0_FACTOR2, m_Arg0FactorCtrl);
	DDX_Control(pDX, IDC_SPIN_SCALE_ARG0_FACTOR2, m_Arg0FactorSpinCtrl);
	DDX_Control(pDX, IDC_ED_SCALE_ARG1_CONST2, m_Arg1ConstCtrl);
	DDX_Control(pDX, IDC_SPIN_POS_ARG1_CONST2, m_Arg1ConstSpinCtrl);
	DDX_Control(pDX, IDC_ED_POS_ARG1_FACTOR2, m_Arg1FactorCtrl);
	DDX_Control(pDX, IDC_SPIN_SCALE_ARG1_FACTOR2, m_Arg1FactorSpinCtrl);
	DDX_Control(pDX, IDC_ED_POS_CLAMP_MIN, m_MinCtrl);
	DDX_Control(pDX, IDC_ED_POS_CLAMP_MAX, m_MaxCtrl);
	DDX_Control(pDX, IDC_SCALE_INTERPOLATION, m_InterpolateCtrl);
	DDX_Control(pDX, IDC_POS_CLAMP, m_ClampCtrl);
	DDX_Control(pDX, IDC_CB_SCALE_ARG0_TYPE2, m_Arg0FuncCtrl);
	DDX_Control(pDX, IDC_CB_SCALE_ARG1_TYPE2, m_Arg1FuncCtrl);
	DDX_Control(pDX, IDC_BTN_SCALE_ARG0_TABLE2, m_Arg0GraphCtrl);
	DDX_Control(pDX, IDC_BTN_SCALE_ARG1_TABLE2, m_Arg1GraphCtrl);
	DDX_Control(pDX, IDC_FACTOR_X, m_FactorXCtrl);
	DDX_Control(pDX, IDC_FACTOR_X2, m_FactorYCtrl);
	DDX_Control(pDX, IDC_FACTOR_X3, m_FactorZCtrl);
	DDX_Control(pDX, IDC_UPDATE_INTERVAL, m_UpdateIntervalCtrl);
	DDX_Control(pDX, IDC_UPDATE_FRAME, m_UpdateFrameCtrl);
	DDX_Control(pDX, IDC_INITIAL_VARIATION, m_InitialRandomFactorCtrl);
	DDX_Control(pDX, IDC_ED_BILLBOARD_FACTOR, m_BillboardScaleFactorCtrl);
}

void CParticleSpecScale::SetParticle( i3ParticleInfo * pInfo)
{
	char conv[256];

	m_pInfo = pInfo;

	if( m_pInfo == NULL)
	{
		m_InterpolateCtrl.SetCheck( BST_UNCHECKED);

		m_UpdateIntervalCtrl.SetWindowText( "");
		m_UpdateFrameCtrl.SetWindowText( "");

		m_InitialRandomFactorCtrl.SetWindowText( "");

		m_FactorXCtrl.SetWindowText( "");
		m_FactorYCtrl.SetWindowText( "");
		m_FactorZCtrl.SetWindowText( "");

		m_Arg0FuncCtrl.SetCurSel( -1);
		m_Arg0ConstCtrl.SetWindowText( "");
		m_Arg0FactorCtrl.SetWindowText( "");
		m_Arg1FuncCtrl.SetCurSel( -1);
		m_Arg1ConstCtrl.SetWindowText( "");
		m_Arg1FactorCtrl.SetWindowText( "");

		m_BillboardScaleFactorCtrl.SetWindowText( "");
	}
	else
	{
		//m_bUseRandom = (pInfo->GetScaleStyle() & I3_PARTICLE_PARAM_STYLE_RANDOM) != 0;
		if( pInfo->GetScaleStyle() & I3_PARTICLE_PARAM_STYLE_INTERPOLATE)
			m_InterpolateCtrl.SetCheck( BST_CHECKED);
		else
			m_InterpolateCtrl.SetCheck( BST_UNCHECKED);

		i3::ftoa( m_pInfo->GetScaleUpdateInterval() * 1000.0f, conv);
		m_UpdateIntervalCtrl.SetWindowText( conv);

		// Billboard Scale Factor
		i3::ftoa( m_pInfo->GetScaleBillboardFactor(), conv);
		m_BillboardScaleFactorCtrl.SetWindowText( conv);

		// Initial Variation
		i3::ftoa( m_pInfo->GetScaleInitialRandomFactor(), conv);
		m_InitialRandomFactorCtrl.SetWindowText( conv);

		i3::ftoa( i3Vector::GetX( pInfo->GetScaleMask()), conv);
		m_FactorXCtrl.SetWindowText( conv);

		i3::ftoa( i3Vector::GetY( pInfo->GetScaleMask()), conv);
		m_FactorYCtrl.SetWindowText( conv);

		i3::ftoa( i3Vector::GetZ( pInfo->GetScaleMask()), conv);
		m_FactorZCtrl.SetWindowText( conv);

		// Arg0
		{
			switch( pInfo->GetScaleFunc( 0))
			{
				case i3ParticleInfo::FUNC_NONE :		m_Arg0FuncCtrl.SetCurSel( 0);	break;
				case i3ParticleInfo::FUNC_CONSTANT :	m_Arg0FuncCtrl.SetCurSel( 1);	break;
				case i3ParticleInfo::FUNC_SIN :			m_Arg0FuncCtrl.SetCurSel( 2);	break;
				case i3ParticleInfo::FUNC_COS :			m_Arg0FuncCtrl.SetCurSel( 3);	break;
				case i3ParticleInfo::FUNC_RND :			m_Arg0FuncCtrl.SetCurSel( 4);	break;
				case i3ParticleInfo::FUNC_TABLE :		m_Arg0FuncCtrl.SetCurSel( 5);	break;
			}

			REAL2STR( pInfo->GetScaleInit( 0), conv);
			m_Arg0ConstCtrl.SetWindowText( conv);

			REAL2STR( pInfo->GetScaleFactor( 0), conv);
			m_Arg0FactorCtrl.SetWindowText( conv);
		}

		// Arg1
		{
			switch( pInfo->GetScaleFunc( 1))
			{
				case i3ParticleInfo::FUNC_NONE :		m_Arg1FuncCtrl.SetCurSel( 0);	break;
				case i3ParticleInfo::FUNC_CONSTANT :	m_Arg1FuncCtrl.SetCurSel( 1);	break;
				case i3ParticleInfo::FUNC_SIN :			m_Arg1FuncCtrl.SetCurSel( 2);	break;
				case i3ParticleInfo::FUNC_COS :			m_Arg1FuncCtrl.SetCurSel( 3);	break;
				case i3ParticleInfo::FUNC_RND :			m_Arg1FuncCtrl.SetCurSel( 4);	break;
				case i3ParticleInfo::FUNC_TABLE :		m_Arg1FuncCtrl.SetCurSel( 5);	break;
			}

			REAL2STR( pInfo->GetScaleInit( 1), conv);
			m_Arg1ConstCtrl.SetWindowText( conv);

			REAL2STR( pInfo->GetScaleFactor( 1), conv);
			m_Arg1FactorCtrl.SetWindowText( conv);
		}

		m_GraphViewCtrl.Set( m_pInfo->GetObjectLifeTime(), m_pInfo->GetScaleFunc(0), m_pInfo->GetScaleInit(0), m_pInfo->GetScaleFactor(0),
			m_pInfo->GetScaleFunc( 1), m_pInfo->GetScaleInit(1), m_pInfo->GetScaleFactor(1),
			(m_pInfo->GetScaleStyle() & I3_PARTICLE_PARAM_STYLE_CLAMP) != 0, m_pInfo->GetScaleMin(), m_pInfo->GetScaleMax());
	}

	UpdateData( FALSE);

	m_InterpolateCtrl.EnableWindow( m_pInfo != NULL);

	m_UpdateIntervalCtrl.EnableWindow( m_pInfo != NULL);
	m_UpdateFrameCtrl.EnableWindow( m_pInfo != NULL);

	m_InitialRandomFactorCtrl.EnableWindow( m_pInfo != NULL);
	m_BillboardScaleFactorCtrl.EnableWindow( (m_pInfo != NULL) && (m_pInfo->GetType() == i3ParticleInfo::PARTICLE_BILLBOARD));

	m_ClampCtrl.EnableWindow( m_pInfo != NULL);
	m_MinCtrl.EnableWindow( m_pInfo != NULL);
	m_MaxCtrl.EnableWindow( m_pInfo != NULL);

	m_FactorXCtrl.EnableWindow( m_pInfo != NULL);
	m_FactorYCtrl.EnableWindow( m_pInfo != NULL);
	m_FactorZCtrl.EnableWindow( m_pInfo != NULL);

	m_Arg0FuncCtrl.EnableWindow( m_pInfo != NULL);
	m_Arg0ConstCtrl.EnableWindow( m_pInfo != NULL);
	m_Arg0FactorCtrl.EnableWindow( m_pInfo != NULL);
	m_Arg0GraphCtrl.EnableWindow( m_pInfo != NULL);

	m_Arg1FuncCtrl.EnableWindow( m_pInfo != NULL);
	m_Arg1ConstCtrl.EnableWindow( m_pInfo != NULL);
	m_Arg1FactorCtrl.EnableWindow( m_pInfo != NULL);
	m_Arg1GraphCtrl.EnableWindow( m_pInfo != NULL);
}

BEGIN_MESSAGE_MAP(CParticleSpecScale, i3TDKDialogBase)

	ON_WM_KILLFOCUS()
	ON_EN_UPDATE(IDC_UPDATE_INTERVAL, OnEnUpdateUpdateInterval)
	ON_BN_CLICKED(IDC_POS_CLAMP, OnBnClickedPosClamp)
	ON_CBN_SELCHANGE(IDC_CB_SCALE_ARG0_TYPE2, OnCbnSelchangeCbScaleArg0Type2)
	ON_CBN_SELCHANGE(IDC_CB_SCALE_ARG1_TYPE2, OnCbnSelchangeCbScaleArg1Type2)
	ON_BN_CLICKED(IDC_BTN_SCALE_ARG0_TABLE2, OnBnClickedBtnScaleArg0Table2)
	ON_BN_CLICKED(IDC_BTN_SCALE_ARG1_TABLE2, OnBnClickedBtnScaleArg1Table2)

	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALE_ARG0_CONST2,  &CParticleSpecScale::OnDeltaPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALE_ARG0_FACTOR2,  &CParticleSpecScale::OnDeltaPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POS_ARG1_CONST2,  &CParticleSpecScale::OnDeltaPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALE_ARG1_FACTOR2,  &CParticleSpecScale::OnDeltaPosSpin)

	ON_EN_KILLFOCUS(IDC_UPDATE_INTERVAL, &CParticleSpecScale::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_INITIAL_VARIATION, &CParticleSpecScale::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_BILLBOARD_FACTOR, &CParticleSpecScale::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_CLAMP_MIN, &CParticleSpecScale::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_CLAMP_MAX, &CParticleSpecScale::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_FACTOR_X, &CParticleSpecScale::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_FACTOR_X2, &CParticleSpecScale::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_FACTOR_X3, &CParticleSpecScale::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_SCALE_ARG0_CONST2, &CParticleSpecScale::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_ARG0_FACTOR2, &CParticleSpecScale::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_SCALE_ARG1_CONST2, &CParticleSpecScale::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_ARG1_FACTOR2, &CParticleSpecScale::OnEnKillFocus)
END_MESSAGE_MAP()


// CParticleSpecScale message handlers

BOOL CParticleSpecScale::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	InitSpinCtrl();

	{
		CRect rc;

		rc.SetRect( 81, 208, 81 + 70, 208 + 70);

		m_GraphViewCtrl.Create( WS_CHILD | WS_VISIBLE, rc, this, IDC_GRAPH_VIEW);
	}

	SetParticle( NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CParticleSpecScale::InitSpinCtrl()
{
	m_Arg0ConstSpinCtrl.SetBuddy( &m_Arg0ConstCtrl);
	m_Arg0ConstSpinCtrl.SetRange(SPINCTRL_NEGATIVE_MAX_COUNT, SPINCTRL_MAX_COUNT);

	m_Arg0FactorSpinCtrl.SetBuddy( &m_Arg0FactorCtrl);
	m_Arg0FactorSpinCtrl.SetRange(SPINCTRL_NEGATIVE_MAX_COUNT, SPINCTRL_MAX_COUNT);

	m_Arg1ConstSpinCtrl.SetBuddy( &m_Arg1ConstCtrl);
	m_Arg1ConstSpinCtrl.SetRange(SPINCTRL_NEGATIVE_MAX_COUNT, SPINCTRL_MAX_COUNT);

	m_Arg1FactorSpinCtrl.SetBuddy( &m_Arg1FactorCtrl);
	m_Arg1FactorSpinCtrl.SetRange(SPINCTRL_NEGATIVE_MAX_COUNT, SPINCTRL_MAX_COUNT);
}

void CParticleSpecScale::OnCancel()
{
}

void CParticleSpecScale::OnOK()
{
	char conv[256];

	if( m_pInfo == NULL)
		return;

	UpdateData( TRUE);

	if( m_InterpolateCtrl.GetCheck() == BST_CHECKED)
		m_pInfo->SetScaleStyle( m_pInfo->GetScaleStyle() | I3_PARTICLE_PARAM_STYLE_INTERPOLATE);
	else
		m_pInfo->SetScaleStyle( m_pInfo->GetScaleStyle() & ~I3_PARTICLE_PARAM_STYLE_INTERPOLATE);

	if( m_UpdateIntervalCtrl.GetModify())
	{
		m_UpdateIntervalCtrl.GetWindowText( conv, sizeof(conv) - 1);
		m_pInfo->SetScaleUpdateInterval( (REAL32) (atof( conv) * 0.001));

		g_bNeedToCreate = true;
		g_pMainWnd->StopRender();
	}

	// Initial Random Factor
	{
		m_InitialRandomFactorCtrl.GetWindowText( conv, sizeof(conv) - 1);
		m_pInfo->SetScaleInitialRandomFactor( (REAL32) atof( conv));
	}

	// Billboard Scale Factor
	{
		m_BillboardScaleFactorCtrl.GetWindowText( conv, sizeof(conv) - 1);
		m_pInfo->SetScaleBillboardFactor( (REAL32) atof( conv));
	}

	{
		REAL32 x, y, z;

		m_FactorXCtrl.GetWindowText( conv, sizeof(conv) - 1);
		x = (REAL32) atof( conv);

		m_FactorYCtrl.GetWindowText( conv, sizeof(conv) - 1);
		y = (REAL32) atof( conv);

		m_FactorZCtrl.GetWindowText( conv, sizeof(conv) - 1);
		z = (REAL32) atof( conv);

		m_pInfo->SetScaleMask( x, y, z);
	}

	m_Arg0ConstCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetScaleInit( 0, (REAL32) atof( conv));

	m_Arg0FactorCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetScaleFactor( 0, (REAL32) atof( conv));

	m_Arg1ConstCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetScaleInit( 1, (REAL32) atof( conv));

	m_Arg1FactorCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetScaleFactor( 1, (REAL32) atof( conv));

	m_GraphViewCtrl.Set( m_pInfo->GetObjectLifeTime(), m_pInfo->GetScaleFunc(0), m_pInfo->GetScaleInit(0), m_pInfo->GetScaleFactor(0),
		m_pInfo->GetScaleFunc( 1), m_pInfo->GetScaleInit(1), m_pInfo->GetScaleFactor(1),
		(m_pInfo->GetScaleStyle() & I3_PARTICLE_PARAM_STYLE_CLAMP) != 0, m_pInfo->GetScaleMin(), m_pInfo->GetScaleMax());

	g_bNeedToCreate = TRUE;
	g_pMainWnd->StopRender();
}

void CParticleSpecScale::OnEnKillFocus()
{
	OnOK();
}

void CParticleSpecScale::OnDeltaPosSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnOK();
}

void CParticleSpecScale::OnEnUpdateUpdateInterval()
{
	char conv[256];
	REAL32 val;

	m_UpdateIntervalCtrl.GetWindowText( conv, sizeof(conv) - 1);
	val = (REAL32) (atof( conv) * 0.001);

	sprintf( conv, "%d/60", (INT32)(val * 60.0f));
	m_UpdateFrameCtrl.SetWindowText( conv);

}

void CParticleSpecScale::OnBnClickedPosClamp()
{
	if( m_ClampCtrl.GetCheck() == BST_CHECKED)
	{
		m_MinCtrl.EnableWindow( TRUE);
		m_MaxCtrl.EnableWindow( TRUE);
	}
	else
	{
		m_MinCtrl.EnableWindow( FALSE);
		m_MaxCtrl.EnableWindow( FALSE);
	}
}

void CParticleSpecScale::OnCbnSelchangeCbScaleArg0Type2()
{
	switch( m_Arg0FuncCtrl.GetCurSel())
	{
		case 0 :	m_pInfo->SetScaleFunc( 0, i3ParticleInfo::FUNC_NONE);	break;
		case 1 :	m_pInfo->SetScaleFunc( 0, i3ParticleInfo::FUNC_CONSTANT);	break;
		case 2 :	m_pInfo->SetScaleFunc( 0, i3ParticleInfo::FUNC_SIN);	break;
		case 3 :	m_pInfo->SetScaleFunc( 0, i3ParticleInfo::FUNC_COS);	break;
		case 4 :	m_pInfo->SetScaleFunc( 0, i3ParticleInfo::FUNC_RND);	break;
		case 5 :	
			m_pInfo->SetScaleFunc( 0, i3ParticleInfo::FUNC_TABLE);	

			if( m_pInfo->GetScaleTable( 0) == NULL)
			{
				i3KeyframeTable * pNew = CreateKeyframeTable( 10);

				m_pInfo->SetScaleTable( 0, pNew);
			}
			break;
	}
}

void CParticleSpecScale::OnCbnSelchangeCbScaleArg1Type2()
{
	switch( m_Arg1FuncCtrl.GetCurSel())
	{
		case 0 :	m_pInfo->SetScaleFunc( 1, i3ParticleInfo::FUNC_NONE); break;
		case 1 :	m_pInfo->SetScaleFunc( 1, i3ParticleInfo::FUNC_CONSTANT);	break;
		case 2 :	m_pInfo->SetScaleFunc( 1, i3ParticleInfo::FUNC_SIN);	break;
		case 3 :	m_pInfo->SetScaleFunc( 1, i3ParticleInfo::FUNC_COS);	break;
		case 4 :	m_pInfo->SetScaleFunc( 1, i3ParticleInfo::FUNC_RND);	break;
		case 5 :	
			m_pInfo->SetScaleFunc( 1, i3ParticleInfo::FUNC_TABLE);	

			if( m_pInfo->GetScaleTable( 1) == NULL)
			{
				i3KeyframeTable * pNew = CreateKeyframeTable( 10);

				m_pInfo->SetScaleTable( 1, pNew);
			}
			break;
	}
}

void CParticleSpecScale::OnBnClickedBtnScaleArg0Table2()
{
	g_pMainWnd->Edit( m_pInfo->GetScaleTable( 0), 0);
}

void CParticleSpecScale::OnBnClickedBtnScaleArg1Table2()
{
	g_pMainWnd->Edit( m_pInfo->GetScaleTable( 1), 0);
}
