// ParticleSpecPosition.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "ParticleSpecPosition.h"
#include ".\particlespecposition.h"
#include "Util.h"
#include "GlobalVariable.h"
#include "i3EffectEditorDlg.h"
#include "i3Base/string/ext/ftoa.h"

// CParticleSpecPosition dialog

IMPLEMENT_DYNAMIC(CParticleSpecPosition, i3TDKDialogBase)
CParticleSpecPosition::CParticleSpecPosition(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CParticleSpecPosition::IDD, pParent)
	, m_bInterpolate(FALSE)
	, m_bGravity(FALSE)
{
	m_pInfo = NULL;
}

CParticleSpecPosition::~CParticleSpecPosition()
{
}

void CParticleSpecPosition::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_POS_INTERPOLATION, m_bInterpolate);
	DDX_Control(pDX, IDC_ED_POS_ARG0_CONST, m_Arg0ConstCtrl);
	DDX_Control(pDX, IDC_SPIN_POS_ARG0_CONST, m_Arg0ConstSpinCtrl);
	DDX_Control(pDX, IDC_ED_POS_ARG0_FACTOR, m_Arg0FactorCtrl);
	DDX_Control(pDX, IDC_SPIN_POS_ARG0_FACTOR, m_Arg0FactorSpinCtrl);
	DDX_Control(pDX, IDC_ED_POS_ARG1_CONST, m_Arg1ConstCtrl);
	DDX_Control(pDX, IDC_SPIN_POS_ARG1_CONST, m_Arg1ConstSpinCtrl);
	DDX_Control(pDX, IDC_ED_POS_ARG1_FACTOR, m_Arg1FactorCtrl);
	DDX_Control(pDX, IDC_SPIN_POS_ARG1_FACTOR, m_Arg1FactorSpinCtrl);
	DDX_Control(pDX, IDC_ED_POS_CLAMP_MIN, m_MinCtrl);
	DDX_Control(pDX, IDC_ED_POS_CLAMP_MAX, m_MaxCtrl);
	DDX_Check(pDX, IDC_POS_GRAVITY, m_bGravity);
	DDX_Control(pDX, IDC_ED_POS_RANDOM_FACTOR_X, m_RandomFactorX);
	DDX_Control(pDX, IDC_ED_POS_RANDOM_FACTOR_Y, m_RandomFactorY);
	DDX_Control(pDX, IDC_ED_POS_RANDOM_FACTOR_Z, m_RandomFactorZ);
	DDX_Control(pDX, IDC_ED_POS_RANDOM_INTERVAL, m_RandomInterval);
	DDX_Control(pDX, IDC_CHK_USE_RANDOM, m_UseRandomCtrl);
	DDX_Control(pDX, IDC_UPDATE_INTERVAL, m_UpdateIntervalCtrl);
	DDX_Control(pDX, IDC_UPDATE_FRAME, m_UpdateFrameCtrl);
	DDX_Control(pDX, IDC_POS_CLAMP, m_ClampCtrl);
	DDX_Control(pDX, IDC_CB_POS_ARG0_TYPE, m_Arg0FuncCtrl);
	DDX_Control(pDX, IDC_CB_POS_ARG1_TYPE, m_Arg1FuncCtrl);
	DDX_Control(pDX, IDC_BTN_POS_ARG0_TABLE, m_Arg0GraphCtrl);
	DDX_Control(pDX, IDC_BTN_POS_ARG1_TABLE, m_Arg1GraphCtrl);
	DDX_Control(pDX, IDC_INITIAL_RANDOM_FACTOR, m_InitialRandomFactor);
}

void CParticleSpecPosition::SetParticle( i3ParticleInfo * pInfo)
{
	char conv[256];

	m_pInfo = pInfo;

	if( m_pInfo == NULL)
	{
		m_bInterpolate = FALSE;
		m_bGravity = FALSE;

		m_UpdateIntervalCtrl.SetWindowText( "");
		m_UpdateFrameCtrl.SetWindowText( "0/60");

		m_InitialRandomFactor.SetWindowText( "");

		m_MinCtrl.SetWindowText( "");
		m_MaxCtrl.SetWindowText( "");

		m_UseRandomCtrl.SetCheck( BST_UNCHECKED);
		m_RandomFactorX.SetWindowText( "");
		m_RandomFactorY.SetWindowText( "");
		m_RandomFactorZ.SetWindowText( "");
		m_RandomInterval.SetWindowText( "");

		m_Arg0FuncCtrl.SetCurSel( -1);
		m_Arg0ConstCtrl.SetWindowText( "");
		m_Arg0FactorCtrl.SetWindowText( "");
		m_Arg1FuncCtrl.SetCurSel( -1);
		m_Arg1ConstCtrl.SetWindowText( "");
		m_Arg1FactorCtrl.SetWindowText( "");
	}
	else
	{
		m_bInterpolate = (pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_INTERPOLATE) != 0;

		if((pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_RANDOM) != 0)
			m_UseRandomCtrl.SetCheck( BST_CHECKED);
		else
			m_UseRandomCtrl.SetCheck( BST_UNCHECKED);

		m_bGravity = (pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_GRAVITY) != 0;

		i3::ftoa( m_pInfo->GetPositionUpdateInterval() * 1000.0f, conv);
		m_UpdateIntervalCtrl.SetWindowText( conv);

		sprintf( conv, "%d/60", int(m_pInfo->GetPositionUpdateInterval() / 60.0f) );
		m_UpdateFrameCtrl.SetWindowText( conv);

		// Initial Random Factor
		i3::ftoa( m_pInfo->GetPositionInitialRandomFactor(), conv);
		m_InitialRandomFactor.SetWindowText( conv);

		if((pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_CLAMP) != 0)
			m_ClampCtrl.SetCheck( BST_CHECKED);
		else
			m_ClampCtrl.SetCheck( BST_UNCHECKED);

		REAL2STR( pInfo->GetPositionMin(), conv);
		m_MinCtrl.SetWindowText( conv);

		REAL2STR( pInfo->GetPositionMax(), conv);
		m_MaxCtrl.SetWindowText( conv);

		REAL2STR( i3Vector::GetX( pInfo->GetPositionRandomFactor()), conv);
		m_RandomFactorX.SetWindowText( conv);

		REAL2STR( i3Vector::GetY( pInfo->GetPositionRandomFactor()), conv);
		m_RandomFactorY.SetWindowText( conv);

		REAL2STR( i3Vector::GetZ( pInfo->GetPositionRandomFactor()), conv);
		m_RandomFactorZ.SetWindowText( conv);

		REAL2STR( pInfo->GetPositionRandomInterval() * 1000.0f, conv);
		m_RandomInterval.SetWindowText( conv);

		// Arg0
		{
			switch( pInfo->GetPositionFunc( 0))
			{
				case i3ParticleInfo::FUNC_NONE :		m_Arg0FuncCtrl.SetCurSel( 0); break;
				case i3ParticleInfo::FUNC_CONSTANT :	m_Arg0FuncCtrl.SetCurSel( 1);	break;
				case i3ParticleInfo::FUNC_SIN :			m_Arg0FuncCtrl.SetCurSel( 2); break;
				case i3ParticleInfo::FUNC_COS :			m_Arg0FuncCtrl.SetCurSel( 3);	break;
				case i3ParticleInfo::FUNC_RND :			m_Arg0FuncCtrl.SetCurSel( 4);	break;
				case i3ParticleInfo::FUNC_TABLE :		m_Arg0FuncCtrl.SetCurSel( 5); break;
			}

			REAL2STR( pInfo->GetPositionInit( 0), conv);
			m_Arg0ConstCtrl.SetWindowText( conv);

			REAL2STR( pInfo->GetPositionFactor( 0), conv);
			m_Arg0FactorCtrl.SetWindowText( conv);
		}

		// Arg1
		{
			switch( pInfo->GetPositionFunc( 1))
			{
				case i3ParticleInfo::FUNC_NONE :		m_Arg1FuncCtrl.SetCurSel( 0); break;
				case i3ParticleInfo::FUNC_CONSTANT :	m_Arg1FuncCtrl.SetCurSel( 1);	break;
				case i3ParticleInfo::FUNC_SIN :			m_Arg1FuncCtrl.SetCurSel( 2); break;
				case i3ParticleInfo::FUNC_COS :			m_Arg1FuncCtrl.SetCurSel( 3);	break;
				case i3ParticleInfo::FUNC_RND :			m_Arg1FuncCtrl.SetCurSel( 4);	break;
				case i3ParticleInfo::FUNC_TABLE :		m_Arg1FuncCtrl.SetCurSel( 5); break;
			}

			REAL2STR( pInfo->GetPositionInit( 1), conv);
			m_Arg1ConstCtrl.SetWindowText( conv);

			REAL2STR( pInfo->GetPositionFactor( 1), conv);
			m_Arg1FactorCtrl.SetWindowText( conv);
		}

		m_GraphViewCtrl.Set( m_pInfo->GetObjectLifeTime(), m_pInfo->GetPositionFunc(0), m_pInfo->GetPositionInit(0), m_pInfo->GetPositionFactor(0),
			m_pInfo->GetPositionFunc( 1), m_pInfo->GetPositionInit(1), m_pInfo->GetPositionFactor(1),
			(m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_CLAMP) != 0, m_pInfo->GetPositionMin(), m_pInfo->GetPositionMax());
	}

	m_UpdateIntervalCtrl.EnableWindow( m_pInfo != NULL);
	m_UpdateFrameCtrl.EnableWindow( m_pInfo != NULL);

	m_InitialRandomFactor.EnableWindow( m_pInfo != NULL);

	m_ClampCtrl.EnableWindow( m_pInfo != NULL);
	m_MinCtrl.EnableWindow( (m_pInfo != NULL) && (m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_CLAMP));
	m_MaxCtrl.EnableWindow( (m_pInfo != NULL) && (m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_CLAMP));

	m_UseRandomCtrl.EnableWindow( (m_pInfo != NULL));
	m_RandomFactorX.EnableWindow( (m_pInfo != NULL) && (m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_RANDOM));
	m_RandomFactorY.EnableWindow( (m_pInfo != NULL) && (m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_RANDOM));
	m_RandomFactorZ.EnableWindow( (m_pInfo != NULL) && (m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_RANDOM));
	m_RandomInterval.EnableWindow( (m_pInfo != NULL) && (m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_RANDOM));

	m_Arg0FuncCtrl.EnableWindow( (m_pInfo != NULL));
	m_Arg0ConstCtrl.EnableWindow( (m_pInfo != NULL));
	m_Arg0FactorCtrl.EnableWindow( (m_pInfo != NULL));
	m_Arg0GraphCtrl.EnableWindow( (m_pInfo != NULL) && (m_pInfo->GetPositionTable(0) != NULL));

	m_Arg1FuncCtrl.EnableWindow( m_pInfo != NULL);
	m_Arg1ConstCtrl.EnableWindow( (m_pInfo != NULL));
	m_Arg1FactorCtrl.EnableWindow( (m_pInfo != NULL));
	m_Arg1GraphCtrl.EnableWindow( (m_pInfo != NULL) && (m_pInfo->GetPositionTable(1) != NULL));

	UpdateData( FALSE);
}

BEGIN_MESSAGE_MAP(CParticleSpecPosition, i3TDKDialogBase)\
	ON_BN_CLICKED(IDC_CHK_USE_RANDOM, OnBnClickedChkUseRandom)
	//ON_EN_CHANGE(IDC_UPDATE_INTERVAL, OnEnChangeUpdateInterval)
	ON_EN_UPDATE(IDC_UPDATE_INTERVAL, OnEnUpdateUpdateInterval)
	ON_BN_CLICKED(IDC_POS_CLAMP, OnBnClickedPosClamp)
	ON_BN_CLICKED(IDC_BTN_POS_ARG0_TABLE, OnBnClickedBtnPosArg0Table)
	ON_CBN_SELCHANGE(IDC_CB_POS_ARG0_TYPE, OnCbnSelchangeCbPosArg0Type)
	ON_CBN_SELCHANGE(IDC_CB_POS_ARG1_TYPE, OnCbnSelchangeCbPosArg1Type)
	ON_BN_CLICKED(IDC_BTN_POS_ARG1_TABLE, OnBnClickedBtnPosArg1Table)


	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POS_ARG0_CONST,  &CParticleSpecScale::OnDeltaPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POS_ARG0_FACTOR,  &CParticleSpecScale::OnDeltaPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POS_ARG1_CONST,  &CParticleSpecScale::OnDeltaPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POS_ARG1_FACTOR,  &CParticleSpecScale::OnDeltaPosSpin)

	ON_EN_KILLFOCUS(IDC_UPDATE_INTERVAL, &CParticleSpecPosition::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_INITIAL_RANDOM_FACTOR, &CParticleSpecPosition::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_CLAMP_MIN, &CParticleSpecPosition::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_CLAMP_MAX, &CParticleSpecPosition::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_RANDOM_FACTOR_X, &CParticleSpecPosition::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_RANDOM_FACTOR_Y, &CParticleSpecPosition::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_RANDOM_FACTOR_Z, &CParticleSpecPosition::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_RANDOM_INTERVAL, &CParticleSpecPosition::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_ARG0_CONST, &CParticleSpecPosition::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_ARG0_FACTOR, &CParticleSpecPosition::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_ARG1_CONST, &CParticleSpecPosition::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_POS_ARG1_FACTOR, &CParticleSpecPosition::OnEnKillFocus)
END_MESSAGE_MAP()


// CParticleSpecPosition message handlers

BOOL CParticleSpecPosition::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();
	
	InitSpinCtrl();

	{
		CRect rc;

		rc.SetRect( 81, 225, 81 + 70, 225 + 70);

		m_GraphViewCtrl.Create( WS_CHILD | WS_VISIBLE, rc, this, IDC_GRAPH_VIEW);
	}

	SetParticle( NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CParticleSpecPosition::InitSpinCtrl()
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

void CParticleSpecPosition::OnOK()
{
	char conv[256];

	if( m_pInfo == NULL)
		return;

	UpdateData( TRUE);

	// Interpolation
	if( m_bInterpolate)
		m_pInfo->SetPositionStyle( m_pInfo->GetPositionStyle() | I3_PARTICLE_PARAM_STYLE_INTERPOLATE);
	else
		m_pInfo->SetPositionStyle( m_pInfo->GetPositionStyle() & ~I3_PARTICLE_PARAM_STYLE_INTERPOLATE);

	// Gravity
	if( m_bGravity)
		m_pInfo->SetPositionStyle( m_pInfo->GetPositionStyle() | I3_PARTICLE_PARAM_STYLE_GRAVITY);
	else
		m_pInfo->SetPositionStyle( m_pInfo->GetPositionStyle() & ~I3_PARTICLE_PARAM_STYLE_GRAVITY);

	// Update Interval
	if( m_UpdateIntervalCtrl.GetModify())
	{
		m_UpdateIntervalCtrl.GetWindowText( conv, sizeof(conv) -1);
		m_pInfo->SetPositionUpdateInterval( (REAL32) (atof( conv) * 0.001));

		g_bNeedToCreate = true;
		g_pMainWnd->StopRender();
	}

	// Initial Random Variation
	{
		m_InitialRandomFactor.GetWindowText( conv, sizeof(conv) - 1);
		m_pInfo->SetPositionInitialRandomFactor( (REAL32) atof(conv));
	}

	// Clamp
	if( m_ClampCtrl.GetCheck() == BST_CHECKED)
		m_pInfo->SetPositionStyle( m_pInfo->GetPositionStyle() | I3_PARTICLE_PARAM_STYLE_CLAMP);
	else
		m_pInfo->SetPositionStyle( m_pInfo->GetPositionStyle() & ~I3_PARTICLE_PARAM_STYLE_CLAMP);

	// Clamp Min & Max
	{
		m_MinCtrl.GetWindowText( conv, sizeof(conv) - 1);
		m_pInfo->SetPositionMin( (REAL32) atof( conv));

		m_MaxCtrl.GetWindowText( conv, sizeof(conv) - 1);
		m_pInfo->SetPositionMax( (REAL32) atof( conv));
	}

	// Random
	{
		REAL32 x, y, z;

		m_RandomFactorX.GetWindowText( conv, sizeof(conv) - 1);
		x = (REAL32) atof( conv);

		m_RandomFactorY.GetWindowText( conv, sizeof(conv) - 1);
		y = (REAL32) atof( conv);

		m_RandomFactorZ.GetWindowText( conv, sizeof(conv) - 1);
		z = (REAL32) atof( conv);

		m_pInfo->SetPositionRandomFactor( x, y, z);

		m_RandomInterval.GetWindowText( conv, sizeof(conv) - 1);
		m_pInfo->SetPositionRandomInterval( (REAL32) (atof( conv) * 0.001));
	}

	// Arg0 Init
	m_Arg0ConstCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetPositionInit( 0, (REAL32) atof( conv));

	// Arg0 Factor
	m_Arg0FactorCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetPositionFactor( 0, (REAL32) atof( conv));

	m_Arg1ConstCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetPositionInit( 1, (REAL32) atof( conv));

	m_Arg1FactorCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetPositionFactor( 1, (REAL32) atof( conv));

	m_GraphViewCtrl.Set( m_pInfo->GetObjectLifeTime(), m_pInfo->GetPositionFunc(0), m_pInfo->GetPositionInit(0), m_pInfo->GetPositionFactor(0),
		m_pInfo->GetPositionFunc( 1), m_pInfo->GetPositionInit(1), m_pInfo->GetPositionFactor(1),
		(m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_CLAMP) != 0, m_pInfo->GetPositionMin(), m_pInfo->GetPositionMax());

	g_bNeedToCreate = TRUE;
	g_pMainWnd->StopRender();
}

void CParticleSpecPosition::OnCancel()
{
}

void CParticleSpecPosition::OnEnKillFocus()
{
	OnOK();
}

void CParticleSpecPosition::OnDeltaPosSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnOK();
}


void CParticleSpecPosition::OnBnClickedChkUseRandom()
{
	BOOL bEnable;

	// User Random
	if( m_UseRandomCtrl.GetCheck() == BST_CHECKED)
	{
		m_pInfo->SetPositionStyle( m_pInfo->GetPositionStyle() | I3_PARTICLE_PARAM_STYLE_RANDOM);
		bEnable = TRUE;
	}
	else
	{
		m_pInfo->SetPositionStyle( m_pInfo->GetPositionStyle() & ~I3_PARTICLE_PARAM_STYLE_RANDOM);
		bEnable = FALSE;
	}

	m_RandomFactorX.EnableWindow( bEnable);
	m_RandomFactorY.EnableWindow( bEnable);
	m_RandomFactorZ.EnableWindow( bEnable);
	m_RandomInterval.EnableWindow( bEnable);
}


void CParticleSpecPosition::OnEnUpdateUpdateInterval()
{
	char conv[256];
	REAL32 val;

	m_UpdateIntervalCtrl.GetWindowText( conv, sizeof(conv) - 1);

	val = (REAL32)( atof(conv) * 0.001);

	sprintf( conv, "%d/60", (INT32)(val * 60.0f));
	m_UpdateFrameCtrl.SetWindowText( conv);
}

void CParticleSpecPosition::OnBnClickedPosClamp()
{
	BOOL bEnable;

	if( m_ClampCtrl.GetCheck() == BST_CHECKED)
		bEnable	= TRUE;
	else
		bEnable = FALSE;

	m_MinCtrl.EnableWindow( bEnable);
	m_MaxCtrl.EnableWindow( bEnable);
}

void CParticleSpecPosition::OnCbnSelchangeCbPosArg0Type()
{
	// Arg0 Function
	switch( m_Arg0FuncCtrl.GetCurSel())
	{
		case 0 :	m_pInfo->SetPositionFunc( 0, i3ParticleInfo::FUNC_NONE);	break;
		case 1 :	m_pInfo->SetPositionFunc( 0, i3ParticleInfo::FUNC_CONSTANT);	break;
		case 2 :	m_pInfo->SetPositionFunc( 0, i3ParticleInfo::FUNC_SIN);	break;
		case 3 :	m_pInfo->SetPositionFunc( 0, i3ParticleInfo::FUNC_COS);	break;
		case 4 :	m_pInfo->SetPositionFunc( 0, i3ParticleInfo::FUNC_RND);	break;
		case 5 :	
			m_pInfo->SetPositionFunc( 0, i3ParticleInfo::FUNC_TABLE);	

			if( m_pInfo->GetPositionTable( 0) == NULL)
			{
				i3KeyframeTable * pNew = CreateKeyframeTable( 10);

				m_pInfo->SetPositionTable( 0, pNew);

				m_Arg0GraphCtrl.EnableWindow( TRUE);
			}
			break;
	}
}

void CParticleSpecPosition::OnCbnSelchangeCbPosArg1Type()
{
	switch( m_Arg1FuncCtrl.GetCurSel())
	{
		case 0 :	m_pInfo->SetPositionFunc( 1, i3ParticleInfo::FUNC_NONE);	break;
		case 1 :	m_pInfo->SetPositionFunc( 1, i3ParticleInfo::FUNC_CONSTANT);	break;
		case 2 :	m_pInfo->SetPositionFunc( 1, i3ParticleInfo::FUNC_SIN);	break;
		case 3 :	m_pInfo->SetPositionFunc( 1, i3ParticleInfo::FUNC_COS);	break;
		case 4 :	m_pInfo->SetPositionFunc( 1, i3ParticleInfo::FUNC_RND);	break;
		case 5 :	
			m_pInfo->SetPositionFunc( 1, i3ParticleInfo::FUNC_TABLE);	

			if( m_pInfo->GetPositionTable( 1) == NULL)
			{
				i3KeyframeTable * pNew = CreateKeyframeTable( 10);

				m_pInfo->SetPositionTable( 1, pNew);

				m_Arg1GraphCtrl.EnableWindow( TRUE);
			}
			break;
	}
}

void CParticleSpecPosition::OnBnClickedBtnPosArg0Table()
{
	g_pMainWnd->Edit( m_pInfo->GetPositionTable( 0), 0);
}

void CParticleSpecPosition::OnBnClickedBtnPosArg1Table()
{
	g_pMainWnd->Edit( m_pInfo->GetPositionTable( 1), 0);
}
