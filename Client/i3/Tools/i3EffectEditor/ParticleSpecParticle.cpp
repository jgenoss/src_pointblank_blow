// ParticleSpecParticle.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "ParticleSpecParticle.h"
#include "SpecTextureSequence.h"
#include "GlobalVariable.h"
#include "Util.h"
#include "i3EffectEditorDlg.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/ftoa.h"

// CParticleSpecParticle dialog

IMPLEMENT_DYNAMIC(CParticleSpecParticle, i3TDKDialogBase)
CParticleSpecParticle::CParticleSpecParticle(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CParticleSpecParticle::IDD, pParent)
{
	m_pInfo = NULL;
	m_pNode = NULL;
}

CParticleSpecParticle::~CParticleSpecParticle()
{
}

void CParticleSpecParticle::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_UPDATE_INTERVAL, m_IntervalCtrl);
	DDX_Control(pDX, IDC_SPIN_UPDATE_INTERVAL, m_IntervalSpinCtrl);
	DDX_Control(pDX, IDC_ED_LIFETIME, m_LifetimeCtrl);
	DDX_Control(pDX, IDC_SPIN_LIFETIME, m_LifetimeSpinCtrl);
	DDX_Control(pDX, IDC_CB_PARTICLE_TYPE, m_TypeCtrl);
	DDX_Control(pDX, IDC_BTN_TEXTURE, m_TextureBtn);
	DDX_Control(pDX, IDC_RES_TEXT, m_ResTextCtrl);
	DDX_Control(pDX, IDC_ST_SEQ, m_SeqTextCtrl);
	DDX_Control(pDX, IDC_BTN_SEQUENCE, m_BtnSeqCtrl);
	DDX_Control(pDX, IDC_CB_BLEND_MODE, m_BlendModeCtrl);
	DDX_Control(pDX, IDC_CB_BLEND_OP, m_BlendOpCtrl);
	DDX_Control(pDX, IDC_IMAGEVIEW_PLACEHOLDER, m_ImageViewPlaceHolder);
	DDX_Control(pDX, IDC_BTN_REMOVE_SEQUENCE, m_RemoveSeqCtrl);
	DDX_Control(pDX, IDC_CHK_VOLATILE, m_VolatileCtrl);
	DDX_Control(pDX, IDC_ED_TEX_NAME, m_TextureNameCtrl);
	DDX_Control(pDX, IDC_CB_TEXTYPE, m_TexTypeCtrl);
	DDX_Control(pDX, IDC_ED_ZBIAS_SLOPE, m_DepthBiasSlopeCtrl);
	DDX_Control(pDX, IDC_ED_ZBIAS_BIAS, m_DepthBiasBiasCtrl);
	DDX_Control(pDX, IDC_CHK_FACETOCAMERA, m_BillboardingCtrl);
	DDX_Control(pDX, IDC_CHK_LIGHTING, m_LightingCtrl);
	DDX_Control(pDX, IDC_CHK_DECAL, m_CHK_Decal);
	DDX_Control(pDX, IDC_CHK_DEPTHWRITE, m_CHK_ZWrite);
	DDX_Control(pDX, IDC_CHK_DEPTHTEST, m_CHK_ZTest);
}

void CParticleSpecParticle::SetParticle( i3Particle * pNode)
{
	char conv[256];

	m_pNode = pNode;
	
	if( pNode == NULL)
	{
		m_pInfo = NULL;

		m_TypeCtrl.SetCurSel( -1);
		m_IntervalCtrl.SetWindowText( "");
		m_LifetimeCtrl.SetWindowText( "");
		m_VolatileCtrl.SetCheck( BST_UNCHECKED);

		m_BlendModeCtrl.SetCurSel( -1);
		m_BlendOpCtrl.SetCurSel( -1);

		m_TextureNameCtrl.SetWindowText( "");

		m_ImageViewCtrl.SetImage( NULL);

		m_DepthBiasSlopeCtrl.SetWindowText( "");
		m_DepthBiasBiasCtrl.SetWindowText( "");
		m_BillboardingCtrl.SetCheck( BST_UNCHECKED);
		m_LightingCtrl.SetCheck( BST_UNCHECKED);
		m_CHK_Decal.SetCheck( BST_UNCHECKED);
		m_CHK_ZWrite.SetCheck( BST_UNCHECKED);
		m_CHK_ZTest.SetCheck( BST_UNCHECKED);
	}
	else
	{
		m_pInfo = pNode->GetParticleInfo();

		switch( m_pInfo->GetType())
		{
			case i3ParticleInfo::PARTICLE_POINTSPRITE :			m_TypeCtrl.SetCurSel( 0);	break;
			case i3ParticleInfo::PARTICLE_SPRITE :				m_TypeCtrl.SetCurSel( 1);	break;
			case i3ParticleInfo::PARTICLE_BILLBOARD :			m_TypeCtrl.SetCurSel( 2);	break;
			case i3ParticleInfo::PARTICLE_SCENE :				m_TypeCtrl.SetCurSel( 3);	break;
			case i3ParticleInfo::PARTICLE_ANCHOR_BILLBOARD :	m_TypeCtrl.SetCurSel( 4);	break;
		}

		if( pNode->getBlendEnable() == false)
		{
			m_BlendModeCtrl.SetCurSel( 0);
		}
		else
		{
			if( pNode->getDestBlend() == I3G_BLEND_INVSRCALPHA)
				m_BlendModeCtrl.SetCurSel( 1);
			else if( pNode->getDestBlend() == I3G_BLEND_ONE)
				m_BlendModeCtrl.SetCurSel( 2);
		}

		switch( pNode->getBlendOperation())
		{
			case I3G_BLEND_OP_ADD :			m_BlendOpCtrl.SetCurSel( 0);	break;
			case I3G_BLEND_OP_SUBTRACT :	m_BlendOpCtrl.SetCurSel( 1);	break;
			case I3G_BLEND_OP_REVSUBTRACT :	m_BlendOpCtrl.SetCurSel( 2);	break;
			case I3G_BLEND_OP_MIN :			m_BlendOpCtrl.SetCurSel( 3);	break;
			case I3G_BLEND_OP_MAX :			m_BlendOpCtrl.SetCurSel( 4);	break;
			default :						m_BlendOpCtrl.SetCurSel( -1);	break;
		}


		REAL2STR( m_pInfo->GetLifeTime(), conv);
		m_LifetimeCtrl.SetWindowText( conv);

		if( m_pInfo->IsVolatile())
			m_VolatileCtrl.SetCheck( BST_CHECKED);
		else
			m_VolatileCtrl.SetCheck( BST_UNCHECKED);

		m_TextureBtn.EnableWindow( TRUE);

		{
			i3Texture * pTex = m_pNode->getTexture();

			if((pTex != NULL) && (pTex->hasName()))
				m_TextureNameCtrl.SetWindowText( pTex->GetName());
			else
				m_TextureNameCtrl.SetWindowText( "");

			m_ImageViewCtrl.SetImage( pTex);
		}

		i3::ftoa( pNode->getZBiasSlope(), conv);
		m_DepthBiasSlopeCtrl.SetWindowText( conv);

		i3::ftoa( pNode->getZBias(), conv);
		m_DepthBiasBiasCtrl.SetWindowText( conv);

		if( pNode->GetFaceToCameraEnable())
			m_BillboardingCtrl.SetCheck( BST_CHECKED);
		else
			m_BillboardingCtrl.SetCheck( BST_UNCHECKED);

		if( pNode->getLightingEnable())
			m_LightingCtrl.SetCheck( BST_CHECKED);
		else
			m_LightingCtrl.SetCheck( BST_UNCHECKED);

		if( pNode->getDecalEnable())
			m_CHK_Decal.SetCheck( BST_CHECKED);
		else
			m_CHK_Decal.SetCheck( BST_UNCHECKED);

		if( pNode->getZWriteEnable())
			m_CHK_ZWrite.SetCheck( BST_CHECKED);
		else
			m_CHK_ZWrite.SetCheck( BST_UNCHECKED);

		if( pNode->getZTestEnable())
			m_CHK_ZTest.SetCheck( BST_CHECKED);
		else
			m_CHK_ZTest.SetCheck( BST_UNCHECKED);

	}

	m_TexTypeCtrl.SetCurSel( 0);

	m_TypeCtrl.EnableWindow( m_pInfo != NULL);
	m_BlendModeCtrl.EnableWindow( m_pInfo != NULL);
	m_BlendOpCtrl.EnableWindow( m_pInfo != NULL);

	m_IntervalCtrl.EnableWindow( m_pInfo != NULL);
	m_LifetimeCtrl.EnableWindow( m_pInfo != NULL);

	m_VolatileCtrl.EnableWindow( m_pInfo != NULL);

	m_TextureBtn.EnableWindow( m_pNode != NULL);
	m_TextureNameCtrl.EnableWindow( m_pNode != NULL);

	m_TexTypeCtrl.EnableWindow( m_pNode != NULL);
	m_DepthBiasSlopeCtrl.EnableWindow( m_pNode != NULL);
	m_DepthBiasBiasCtrl.EnableWindow( m_pNode != NULL);
	m_BillboardingCtrl.EnableWindow( m_pNode != NULL);
	m_LightingCtrl.EnableWindow( m_pNode != NULL);
	m_CHK_Decal.EnableWindow( m_pNode != NULL);
	m_CHK_ZWrite.EnableWindow( m_pNode != NULL);
	m_CHK_ZTest.EnableWindow( m_pNode != NULL);

	if( m_TypeCtrl.GetCurSel() == 3)
	{
		m_SeqTextCtrl.EnableWindow( FALSE);
		m_BtnSeqCtrl.EnableWindow( FALSE);
		m_RemoveSeqCtrl.EnableWindow( FALSE);
	}
	else
	{
		m_SeqTextCtrl.EnableWindow( m_pInfo != NULL);
		m_BtnSeqCtrl.EnableWindow( m_pInfo != NULL);
		m_RemoveSeqCtrl.EnableWindow( (m_pNode != NULL) && (m_pNode->GetTextureSequence() != NULL));
	}

	UpdateData( FALSE);
}


BEGIN_MESSAGE_MAP(CParticleSpecParticle, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_CB_PARTICLE_TYPE, OnCbnSelchangeCbParticleType)
//	ON_EN_CHANGE(IDC_ED_UPDATE_INTERVAL, OnEnChangeEdUpdateInterval)
//	ON_EN_CHANGE(IDC_ED_LIFETIME, OnEnChangeEdLifetime)
	ON_BN_CLICKED(IDC_CHK_VOLATILE, OnBnClickedChkVolatile)
	ON_BN_CLICKED(IDC_BTN_TEXTURE, OnBnClickedBtnTexture)
	ON_CBN_SELENDOK(IDC_CB_PARTICLE_TYPE, OnCbnSelendokCbParticleType)
	ON_BN_CLICKED(IDC_BTN_SEQUENCE, OnBnClickedBtnSequence)
	ON_CBN_SELCHANGE(IDC_CB_BLEND_MODE, OnCbnSelchangeCbBlendMode)
	ON_CBN_SELCHANGE(IDC_CB_BLEND_OP, OnCbnSelchangeCbBlendOp)
	ON_BN_CLICKED(IDC_BTN_REMOVE_SEQUENCE, OnBnClickedBtnRemoveSequence)
	ON_CBN_SELCHANGE(IDC_CB_TEXTYPE, OnCbnSelchangeCbTextype)
	ON_BN_CLICKED(IDC_CHK_FACETOCAMERA, OnBnClickedChkFacetocamera)
	ON_BN_CLICKED(IDC_CHK_LIGHTING, OnBnClickedChkLighting)
	ON_BN_CLICKED(IDC_CHK_DECAL, OnBnClickedChkDecal)
	ON_BN_CLICKED(IDC_CHK_DEPTHWRITE, OnBnClickedChkDepthwrite)
	ON_BN_CLICKED(IDC_CHK_DEPTHTEST, OnBnClickedChkDepthTest)

	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_UPDATE_INTERVAL,  &CParticleSpecParticle::OnDeltaPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LIFETIME,  &CParticleSpecParticle::OnDeltaPosSpin)

	ON_EN_KILLFOCUS(IDC_ED_UPDATE_INTERVAL, &CParticleSpecParticle::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_LIFETIME, &CParticleSpecParticle::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_ZBIAS_SLOPE, &CParticleSpecParticle::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_ZBIAS_BIAS, &CParticleSpecParticle::OnEnKillFocus)
END_MESSAGE_MAP()


// CParticleSpecParticle message handlers

BOOL CParticleSpecParticle::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rect;

		m_ImageViewPlaceHolder.GetWindowRect( &rect);

		ScreenToClient( &rect);

		m_ImageViewCtrl.Create( WS_CHILD | WS_VISIBLE, this, 1827);
		m_ImageViewCtrl.SetWindowPos( NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}

	InitSpinCtrl();

	SetParticle( NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CParticleSpecParticle::InitSpinCtrl()
{
	m_IntervalSpinCtrl.SetBuddy( &m_IntervalCtrl);
	m_IntervalSpinCtrl.SetRange(0, SPINCTRL_MAX_COUNT);

	m_LifetimeSpinCtrl.SetBuddy( &m_LifetimeCtrl);
	m_LifetimeSpinCtrl.SetRange(0, 100);
}


void CParticleSpecParticle::OnCbnSelchangeCbParticleType()
{
	if( m_pInfo == NULL)
		return;

	UpdateData( TRUE);

	g_bNeedToCreate = TRUE;
	g_pMainWnd->StopRender();

	EnterRenderCS();

    switch( m_TypeCtrl.GetCurSel())
	{
		case 0 :	m_pInfo->SetType( i3ParticleInfo::PARTICLE_POINTSPRITE);	break;
		case 1 :	m_pInfo->SetType( i3ParticleInfo::PARTICLE_SPRITE);	break;
		case 2 :	m_pInfo->SetType( i3ParticleInfo::PARTICLE_BILLBOARD);	break;
		case 3 :	m_pInfo->SetType( i3ParticleInfo::PARTICLE_SCENE);		break;
		case 4 :	m_pInfo->SetType( i3ParticleInfo::PARTICLE_ANCHOR_BILLBOARD);	break;
	}
	
	LeaveRenderCS();

	if( m_TypeCtrl.GetCurSel() == 3)
	{
		m_ResTextCtrl.SetWindowText( "Scene");

		m_SeqTextCtrl.EnableWindow( FALSE);
		m_BtnSeqCtrl.EnableWindow( FALSE);
	}
	else
	{
		m_ResTextCtrl.SetWindowText( "Texture");

		m_SeqTextCtrl.EnableWindow( TRUE);
		m_BtnSeqCtrl.EnableWindow( TRUE);
	}

	//UpdateEditor( UPDATE_EDIT_EFFECT, m_pNode);
}


void CParticleSpecParticle::OnBnClickedChkVolatile()
{
	if( m_pInfo == NULL)
		return;

	UpdateData( TRUE);

	if( m_VolatileCtrl.GetCheck() == BST_CHECKED)
		m_pInfo->SetVolatile( TRUE);	
	else
		m_pInfo->SetVolatile( FALSE);
}


void CParticleSpecParticle::OnOK()
{
	char conv[256];

	if( m_pInfo == NULL)
		return;

	UpdateData( TRUE);

	m_LifetimeCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetLifeTime( (REAL32) atof( conv));

	// Depth Bias
	{
		m_DepthBiasSlopeCtrl.GetWindowText( conv, sizeof(conv) - 1);
		m_pNode->setZBiasSlope( (REAL32) atof( conv));

		m_DepthBiasBiasCtrl.GetWindowText( conv, sizeof(conv) - 1);
		m_pNode->setZBias( (REAL32) atof( conv));
	}

	g_bNeedToCreate = TRUE;
	g_pMainWnd->StopRender();
}

void CParticleSpecParticle::OnCancel()
{
}

void CParticleSpecParticle::OnBnClickedBtnTexture()
{
	if( m_pInfo == NULL)
		return;

	if( m_pInfo->GetType() == i3ParticleInfo::PARTICLE_SCENE)
	{
		CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
			"i3Engine Scene File(*.i3s)|*.i3s||", this);

		if( Dlg.DoModal() == IDCANCEL)
			return;

		{
			i3SceneFile file;

			if( file.Load( LPCTSTR( Dlg.GetPathName())) == STREAM_ERR)
			{
				DebugBox( m_hWnd, "Could not open %s i3Engine scene file.", LPCTSTR( Dlg.GetPathName()));
				return;
			}

			{
				i3SceneGraphInfo * pInfo = file.getSceneGraphInfo();

				{
					char szRelPath[ MAX_PATH];

					i3::make_relative_path( g_pOption->m_szWorkPath, LPCTSTR( Dlg.GetPathName()), szRelPath  );

					pInfo->SetName( szRelPath);
				}

				pInfo->SetExternRes( TRUE);

				m_pNode->SetMesh( pInfo);

				i3Scene::ModifyDynamicState( m_pNode, I3_PHYSIX_STATE_KINEMATIC | I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_RESPONSE | I3_PHYSIX_STATE_NO_COLLISION, 0);
			}

			g_bNeedToCreate = TRUE;
			g_pMainWnd->StopRender();
		}
	}
	else
	{
		CFileDialog	Dlg( TRUE, "", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
			"Image File(*.TGA;*.I3I)|*.TGA;*.I3I||", this);

		if( Dlg.DoModal() == IDCANCEL)
			return;

		{
			i3ImageFile file;
			i3Texture * pTex;

			pTex = file.Load( LPCTSTR( Dlg.GetPathName()));
			if( pTex == NULL)
			{
				DebugBox( m_hWnd, "Could not open %s image file.", LPCTSTR( Dlg.GetPathName()));
				return;
			}

			pTex->SetExternRes( TRUE);

			{
				char szName[MAX_PATH];

				i3::make_relative_path( g_pOption->m_szWorkPath, LPCTSTR( Dlg.GetPathName()), szName  );

				pTex->SetName( szName);

				m_TextureNameCtrl.SetWindowText( szName);
			}

			if( m_TexTypeCtrl.GetCurSel() == 0)
			{
				// Diffuse Map
				m_pNode->setTexture( pTex);
			}
			else
			{
				// Normal Map
				m_pNode->setNormalMap( pTex);
			}

			m_pNode->RefreshChanges();

			m_ImageViewCtrl.SetImage( pTex);

			pTex->Release();
		}
	}
}

void CParticleSpecParticle::OnCbnSelendokCbParticleType()
{
	// TODO: Add your control notification handler code here
}

void CParticleSpecParticle::OnBnClickedBtnSequence()
{
	CSpecTextureSequence Dlg;

	if( Dlg.Execute( m_pNode) == FALSE)
		return;

	m_RemoveSeqCtrl.EnableWindow( m_pNode->GetTextureSequence() != NULL);
}

void CParticleSpecParticle::OnCbnSelchangeCbBlendMode()
{
	switch( m_BlendModeCtrl.GetCurSel())
	{
		case 0 :
			m_pNode->setBlendEnable( false);
			break;

		case 1 :		// Modulate
			m_pNode->setBlendEnable( true);
			m_pNode->setSrcBlend( I3G_BLEND_SRCALPHA);	
			m_pNode->setDestBlend( I3G_BLEND_INVSRCALPHA);
			break;

		case 2 :		// Add
			m_pNode->setBlendEnable( true);
			m_pNode->setSrcBlend( I3G_BLEND_SRCALPHA);
			m_pNode->setDestBlend( I3G_BLEND_ONE);
			break;
	}

	m_pNode->RefreshChanges();
}


void CParticleSpecParticle::OnCbnSelchangeCbBlendOp()
{
	switch( m_BlendOpCtrl.GetCurSel())
	{
		case 0 :		// Add
			m_pNode->setBlendOperation( I3G_BLEND_OP_ADD);
			break;

		case 1 :		// Subtract
			m_pNode->setBlendOperation( I3G_BLEND_OP_SUBTRACT);
			break;

		case 2 :		// Rev-Subtract
			m_pNode->setBlendOperation( I3G_BLEND_OP_REVSUBTRACT);
			break;

		case 3 :		// Min
			m_pNode->setBlendOperation( I3G_BLEND_OP_MIN);
			break;

		case 4 :		// Max
			m_pNode->setBlendOperation( I3G_BLEND_OP_MAX);
			break;
	}

	m_pNode->RefreshChanges();
}

void CParticleSpecParticle::OnBnClickedBtnRemoveSequence()
{
	m_pNode->SetTextureSequence( NULL);

	m_RemoveSeqCtrl.EnableWindow( FALSE);

	m_pNode->RefreshChanges();
}

void CParticleSpecParticle::OnEnKillFocus()
{
	OnOK();
}

void CParticleSpecParticle::OnDeltaPosSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnOK();
}

void CParticleSpecParticle::OnCbnSelchangeCbTextype()
{
	i3Texture * pTex;

	switch( m_TexTypeCtrl.GetCurSel())
	{
		case 0 :		pTex = m_pNode->getTexture();	break;
		default :		pTex = m_pNode->getNormalMap(); break;
	}

	if((pTex != NULL) && (pTex->hasName()))
		m_TextureNameCtrl.SetWindowText( pTex->GetName());
	else
		m_TextureNameCtrl.SetWindowText( "");

	m_ImageViewCtrl.SetImage( pTex);
}

void CParticleSpecParticle::OnBnClickedChkFacetocamera()
{
	if( m_pNode != NULL)
	{
		if( m_BillboardingCtrl.GetCheck() == BST_CHECKED)
			m_pNode->SetFaceToCameraEnable( TRUE);
		else
			m_pNode->SetFaceToCameraEnable( FALSE);
	}

	m_pNode->RefreshChanges();
}


void CParticleSpecParticle::OnBnClickedChkLighting()
{
	if( m_pNode != NULL)
	{
		if( m_LightingCtrl.GetCheck() == BST_CHECKED)
			m_pNode->setLightingEnable( TRUE);
		else
			m_pNode->setLightingEnable( FALSE);
	}

	m_pNode->RefreshChanges();
}

void CParticleSpecParticle::OnBnClickedChkDecal()
{
	if( m_pNode != NULL)
	{
		if( m_CHK_Decal.GetCheck() == BST_CHECKED)
			m_pNode->setDecalEnable( true);
		else
			m_pNode->setDecalEnable( false);
	}
}

void CParticleSpecParticle::OnBnClickedChkDepthwrite()
{
	if( m_pNode != NULL)
	{
		if( m_CHK_ZWrite.GetCheck() == BST_CHECKED)
			m_pNode->setZWriteEnable( true);
		else
			m_pNode->setZWriteEnable( false);
	}

	m_pNode->RefreshChanges();
}

void CParticleSpecParticle::OnBnClickedChkDepthTest()
{
	if( m_pNode != NULL)
	{
		if( m_CHK_ZTest.GetCheck() == BST_CHECKED)
			m_pNode->setZTestEnable( true);
		else
			m_pNode->setZTestEnable( false);
	}

	m_pNode->RefreshChanges();
}

