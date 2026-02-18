// ParticleSpecColor.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "ParticleSpecColor.h"
#include ".\particlespeccolor.h"
#include "GlobalVariable.h"
#include "i3EffectEditorDlg.h"
#include "Util.h"
#include "ColorKeyEditDialog.h"

// CParticleSpecColor dialog

IMPLEMENT_DYNAMIC(CParticleSpecColor, i3TDKDialogBase)
CParticleSpecColor::CParticleSpecColor(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CParticleSpecColor::IDD, pParent)
{
	m_pInfo = NULL;
}

CParticleSpecColor::~CParticleSpecColor()
{
}

void CParticleSpecColor::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_COLOR_ALPHA_TYPE2, m_AlphaFuncCtrl);
	DDX_Control(pDX, IDC_COLOR_INTERPOLATION, m_InterpCtrl);
	DDX_Control(pDX, IDC_CHK_USE_COLOR_RANDOM, m_UseRandomCtrl);
	DDX_Control(pDX, IDC_CB_COLOR_RGB_TYPE, m_RGBFuncCtrl);
	DDX_Control(pDX, IDC_BTN_EDITKEY, m_ColorKeyEditCtrl);
	DDX_Control(pDX, IDC_ED_RGB_CONST_R, m_InitRCtrl);
	DDX_Control(pDX, IDC_ED_RGB_CONST_G, m_InitGCtrl);
	DDX_Control(pDX, IDC_ED_RGB_CONST_B, m_InitBCtrl);
	DDX_Control(pDX, IDC_ED_RGB_CONST_ALPHA, m_InitACtrl);
	DDX_Control(pDX, IDC_BTN_EDITALPHAKEY, m_AlphaKeyEditCtrl);
}

void CParticleSpecColor::SetParticle( i3ParticleInfo * pInfo)
{
	char conv[256];

	m_pInfo = pInfo;

	if( m_pInfo == NULL)
	{
		m_AlphaFuncCtrl.SetCurSel( -1);
	}
	else
	{
		// Interpolate
		if((pInfo->GetColorStyle() & I3_PARTICLE_PARAM_STYLE_INTERPOLATE) != 0)
			m_InterpCtrl.SetCheck( BST_CHECKED);
		else
			m_InterpCtrl.SetCheck( BST_UNCHECKED);

		// Use Random
        if( (pInfo->GetColorStyle() & I3_PARTICLE_PARAM_STYLE_RANDOM) != 0)
			m_UseRandomCtrl.SetCheck( BST_CHECKED);
		else
			m_UseRandomCtrl.SetCheck( BST_UNCHECKED);

		// RGB Type
		switch( pInfo->GetRGBFunc())
		{
			case i3ParticleInfo::FUNC_NONE :		m_RGBFuncCtrl.SetCurSel( 0);	break;
			case i3ParticleInfo::FUNC_CONSTANT :	m_RGBFuncCtrl.SetCurSel( 1);	break;
			case i3ParticleInfo::FUNC_SIN :			m_RGBFuncCtrl.SetCurSel( 2);	break;
			case i3ParticleInfo::FUNC_COS :			m_RGBFuncCtrl.SetCurSel( 3);	break;
			case i3ParticleInfo::FUNC_RND :			m_RGBFuncCtrl.SetCurSel( 4);	break;
			case i3ParticleInfo::FUNC_TABLE :		m_RGBFuncCtrl.SetCurSel( 5);	break;
		}

		// Alpha Type
		switch( pInfo->GetAlphaFunc())
		{
			case i3ParticleInfo::FUNC_NONE :		m_AlphaFuncCtrl.SetCurSel( 0);	break;
			case i3ParticleInfo::FUNC_CONSTANT :	m_AlphaFuncCtrl.SetCurSel( 1);	break;
			case i3ParticleInfo::FUNC_SIN :			m_AlphaFuncCtrl.SetCurSel( 2);	break;
			case i3ParticleInfo::FUNC_COS :			m_AlphaFuncCtrl.SetCurSel( 3);	break;
			case i3ParticleInfo::FUNC_RND :			m_AlphaFuncCtrl.SetCurSel( 4);	break;
			case i3ParticleInfo::FUNC_TABLE :		m_AlphaFuncCtrl.SetCurSel( 5);	break;
		}

		// Initial Values
		{
			sprintf( conv, "%d", pInfo->GetRGBInit() & 0xFF);
			m_InitRCtrl.SetWindowText( conv);

			sprintf( conv, "%d", (pInfo->GetRGBInit() >> 8) & 0xFF);
			m_InitGCtrl.SetWindowText( conv);

			sprintf( conv, "%d", (pInfo->GetRGBInit() >> 16) & 0xFF);
			m_InitBCtrl.SetWindowText( conv);
			
			sprintf( conv, "%d", pInfo->GetAlphaInit());
			m_InitACtrl.SetWindowText( conv);
		}
	}

	m_InterpCtrl.EnableWindow( m_pInfo != NULL);
	m_UseRandomCtrl.EnableWindow( m_pInfo != NULL);
	m_RGBFuncCtrl.EnableWindow( m_pInfo != NULL);
	m_ColorKeyEditCtrl.EnableWindow( (m_pInfo != NULL) && (m_pInfo->GetRGBFunc() == i3ParticleInfo::FUNC_TABLE));
	m_AlphaKeyEditCtrl.EnableWindow( (m_pInfo != NULL) && (m_pInfo->GetAlphaFunc() == i3ParticleInfo::FUNC_TABLE));
	m_AlphaFuncCtrl.EnableWindow( m_pInfo != NULL);
	m_InitRCtrl.EnableWindow( m_pInfo != NULL);
	m_InitGCtrl.EnableWindow( m_pInfo != NULL);
	m_InitBCtrl.EnableWindow( m_pInfo != NULL);
	m_InitACtrl.EnableWindow( m_pInfo != NULL);

	UpdateData( FALSE);
}

BEGIN_MESSAGE_MAP(CParticleSpecColor, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_COLOR_INTERPOLATION, OnBnClickedColorInterpolation)
	ON_BN_CLICKED( IDC_CHK_USE_COLOR_RANDOM, OnBnClickedChkUseColorRandom)
	ON_CBN_SELCHANGE(IDC_CB_COLOR_RGB_TYPE, OnCbnSelchangeCbColorRgbType)
	ON_CBN_SELCHANGE(IDC_CB_COLOR_ALPHA_TYPE2, OnCbnSelchangeCbColorAlphaType2)
	ON_BN_CLICKED(IDC_BTN_RGB_TABLE_ALPHA, OnBnClickedBtnRgbTableAlpha)
	ON_BN_CLICKED(IDC_BTN_EDITKEY, OnBnClickedBtnEditkey)
	ON_BN_CLICKED(IDC_BTN_EDITALPHAKEY, OnBnClickedBtnEditalphakey)

	ON_EN_KILLFOCUS(IDC_ED_RGB_CONST_R, &CParticleSpecColor::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_RGB_CONST_G, &CParticleSpecColor::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_RGB_CONST_B, &CParticleSpecColor::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_RGB_CONST_ALPHA, &CParticleSpecColor::OnEnKillFocus)
END_MESSAGE_MAP()

// CParticleSpecColor message handlers

BOOL CParticleSpecColor::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		m_ColorDlg.Create( CColorKeyEditDialog::IDD, this);
	}

	SetParticle( NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CParticleSpecColor::OnBnClickedColorInterpolation()
{
	UpdateData( TRUE);

	if( m_pInfo != NULL)
	{
		if( m_InterpCtrl.GetCheck() == BST_CHECKED)
			m_pInfo->SetColorStyle( m_pInfo->GetColorStyle() | I3_PARTICLE_PARAM_STYLE_INTERPOLATE);
		else
			m_pInfo->SetColorStyle( m_pInfo->GetColorStyle() & ~I3_PARTICLE_PARAM_STYLE_INTERPOLATE);
	}
}


void CParticleSpecColor::OnBnClickedChkUseColorRandom()
{
	UpdateData( TRUE);

	if( m_pInfo != NULL)
	{
		if( m_UseRandomCtrl.GetCheck() == BST_CHECKED)
			m_pInfo->SetColorStyle( m_pInfo->GetColorStyle() | I3_PARTICLE_PARAM_STYLE_RANDOM);
		else
			m_pInfo->SetColorStyle( m_pInfo->GetColorStyle() & ~I3_PARTICLE_PARAM_STYLE_RANDOM);
	}
}


void CParticleSpecColor::OnCbnSelchangeCbColorRgbType()
{
	UpdateData( TRUE);

	if( m_pInfo == NULL)
		return;

	switch( m_RGBFuncCtrl.GetCurSel())
	{
		case 0 :	m_pInfo->SetRGBFunc( i3ParticleInfo::FUNC_NONE);	break;
		case 1 :	m_pInfo->SetRGBFunc( i3ParticleInfo::FUNC_CONSTANT);	break;
		case 2 :	m_pInfo->SetRGBFunc( i3ParticleInfo::FUNC_SIN);	break;
		case 3 :	m_pInfo->SetRGBFunc( i3ParticleInfo::FUNC_COS);	break;
		case 4 :	m_pInfo->SetRGBFunc( i3ParticleInfo::FUNC_RND);	break;
		case 5 :	
			m_pInfo->SetRGBFunc( i3ParticleInfo::FUNC_TABLE);	

			if( m_pInfo->GetRTable() == NULL)
			{
				i3KeyframeTable * pNew = CreateColorKeyframeTable( 3);

				m_pInfo->SetRTable( pNew);
			}

			if( m_pInfo->GetGTable() == NULL)
			{
				i3KeyframeTable * pNew = CreateColorKeyframeTable( 3);

				m_pInfo->SetGTable( pNew);
			}

			if( m_pInfo->GetBTable() == NULL)
			{
				i3KeyframeTable * pNew = CreateColorKeyframeTable( 3);

				m_pInfo->SetBTable( pNew);
			}
			break;
	}

	m_ColorKeyEditCtrl.EnableWindow( m_pInfo->GetRGBFunc() == i3ParticleInfo::FUNC_TABLE);
}

void CParticleSpecColor::OnCbnSelchangeCbColorAlphaType2()
{
	UpdateData( TRUE);

	if( m_pInfo == NULL)
		return;

	switch( m_AlphaFuncCtrl.GetCurSel())
	{
		case 0 :	m_pInfo->SetAlphaFunc( i3ParticleInfo::FUNC_NONE);	break;
		case 1 :	m_pInfo->SetAlphaFunc( i3ParticleInfo::FUNC_CONSTANT);	break;
		case 2 :	m_pInfo->SetAlphaFunc( i3ParticleInfo::FUNC_SIN);	break;
		case 3 :	m_pInfo->SetAlphaFunc( i3ParticleInfo::FUNC_COS);	break;
		case 4 :	m_pInfo->SetAlphaFunc( i3ParticleInfo::FUNC_RND);	break;
		case 5 :	
			m_pInfo->SetAlphaFunc( i3ParticleInfo::FUNC_TABLE);	

			if( m_pInfo->GetAlphaTable() == NULL)
			{
				i3KeyframeTable * pNew = CreateColorKeyframeTable( 3);

				m_pInfo->SetAlphaTable( pNew);
			}
			break;
	}

	m_AlphaKeyEditCtrl.EnableWindow( m_pInfo->GetAlphaFunc() == i3ParticleInfo::FUNC_TABLE);
}

void CParticleSpecColor::OnCancel()
{
}

void CParticleSpecColor::OnOK()
{

	UpdateData(TRUE);

	g_bNeedToCreate = TRUE;
	g_pMainWnd->StopRender();
}

void CParticleSpecColor::OnBnClickedBtnRgbTableAlpha()
{
	g_pMainWnd->Edit( m_pInfo->GetAlphaTable(), 1);
}

void CParticleSpecColor::OnEnKillFocus()
{
	OnOK();
}

void CParticleSpecColor::OnBnClickedBtnEditkey()
{
	CRect rect;

	m_ColorKeyEditCtrl.GetWindowRect( &rect);

	m_ColorDlg.Execute( rect.TopLeft(), m_pInfo->GetRTable(), m_pInfo->GetGTable(), m_pInfo->GetBTable(), NULL);
}

void CParticleSpecColor::OnBnClickedBtnEditalphakey()
{
	CRect rect;

	m_AlphaKeyEditCtrl.GetWindowRect( &rect);

	m_ColorDlg.Execute( rect.TopLeft(), NULL, NULL, NULL, m_pInfo->GetAlphaTable());
}
