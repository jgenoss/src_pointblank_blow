// Prop_LvElm_World.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvElm_World.h"
#include "DlgSelectSg.h"
#include "DlgWorldSectionEdit.h"
#include "DlgMapSectorEdit.h"
#include "i3Base/string/ext/ftoa.h"

// CProp_LvElm_World dialog

IMPLEMENT_DYNAMIC(CProp_LvElm_World, i3TDKDialogBase)
CProp_LvElm_World::CProp_LvElm_World(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvElm_World::IDD, pParent)
{
}

CProp_LvElm_World::~CProp_LvElm_World()
{
	CDlgMapSectorEdit::Destroy();
}

void CProp_LvElm_World::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_WORLD, m_WorldPathCtrl);
	DDX_Control(pDX, IDC_CHK_ENFOGWORLD, m_WorldFogEnableCtrl);
	DDX_Control(pDX, IDC_STATIC_AMBIENT, m_WorldFogColorCtrl);
	DDX_Control(pDX, IDC_EDIT_WFDENSKTY, m_WorldFogDensityCtrl);
	DDX_Control(pDX, IDC_COMBO_WFMODE, m_WorldFogModeCtrl);
	DDX_Control(pDX, IDC_EDIT_WFNEAR, m_WorldFogNearCtrl);
	DDX_Control(pDX, IDC_EDIT_WFFAR, m_WorldFogFarCtrl);
	DDX_Control(pDX, IDC_ED_SKYPATH, m_SkyPathCtrl);
	DDX_Control(pDX, IDC_CHK_ENFOG_SKY, m_SkyFogEnableCtrl);
	DDX_Control(pDX, IDC_STATIC_AMBIENT2, m_SkyFogColorCtrl);
	DDX_Control(pDX, IDC_EDIT_SFDENSITY, m_SkyFogDensityCtrl);
	DDX_Control(pDX, IDC_COMBO_SFMODE, m_SkyFogModeCtrl);
	DDX_Control(pDX, IDC_EDIT_SFNEAR, m_SkyFogNearCtrl);
	DDX_Control(pDX, IDC_EDIT_SFFAR, m_SkyFogFarCtrl);
	DDX_Control(pDX, IDC_ED_WORLDCOL, m_WorldCollCtrl);
	DDX_Control(pDX, IDC_ED_HITCOL, m_HitCollCtrl);
	DDX_Control(pDX, IDC_ED_LIGHTVOLUME, m_LightCollCtrl);
}

void CProp_LvElm_World::_SetWorldFogColor( COLOR * pColor)
{
	m_COL_WorldFog.setColor( pColor);
	m_COL_WorldFog.Invalidate();
}

void CProp_LvElm_World::_SetSkyFogColor( COLOR * pColor)
{
	m_COL_SkyFog.setColor( pColor);
	m_COL_SkyFog.Invalidate();
}

void CProp_LvElm_World::_UpdateInfo( void)
{
	char conv[256];

	{
		BOOL bEnable = (BOOL)( m_pWorld != NULL);
		m_WorldPathCtrl.EnableWindow( bEnable);
		m_WorldFogEnableCtrl.EnableWindow( bEnable);
		m_WorldFogModeCtrl.EnableWindow( bEnable);
		m_WorldFogColorCtrl.EnableWindow( bEnable);
		m_COL_WorldFog.EnableWindow( bEnable);
		m_WorldFogDensityCtrl.EnableWindow( bEnable);
		m_WorldFogNearCtrl.EnableWindow( bEnable);
		m_WorldFogFarCtrl.EnableWindow( bEnable);

		m_SkyPathCtrl.EnableWindow( bEnable);
        m_SkyFogEnableCtrl.EnableWindow( bEnable);
		m_SkyFogModeCtrl.EnableWindow( bEnable);
		m_SkyFogColorCtrl.EnableWindow( bEnable);
		m_COL_SkyFog.EnableWindow( bEnable);
		m_SkyFogDensityCtrl.EnableWindow( bEnable);
		m_SkyFogNearCtrl.EnableWindow( bEnable);
		m_SkyFogFarCtrl.EnableWindow( bEnable);

		m_WorldCollCtrl.EnableWindow( bEnable);
		m_HitCollCtrl.EnableWindow( bEnable);
		m_LightCollCtrl.EnableWindow( bEnable);
	}

	if( m_pWorld  != NULL)
	{
		// World
		if( m_pWorld->getWorld() != NULL)
			m_WorldPathCtrl.SetWindowText( m_pWorld->getWorld()->GetName());
		else
			m_WorldPathCtrl.SetWindowText( "(None)");

		// Fog
		if( m_pWorld->GetFogEnable( I3FRM_FOG_WORLD))
			m_WorldFogEnableCtrl.SetCheck( BST_CHECKED);
		else
			m_WorldFogEnableCtrl.SetCheck( BST_UNCHECKED);

		switch( m_pWorld->GetFogMode( I3FRM_FOG_WORLD))
		{
			case I3G_FOG_LINEAR :	m_WorldFogModeCtrl.SetCurSel( 0);	break;
			case I3G_FOG_EXP :		m_WorldFogModeCtrl.SetCurSel( 1);	break;
			case I3G_FOG_EXP2 :		m_WorldFogModeCtrl.SetCurSel( 2);	break;
			default :				m_WorldFogModeCtrl.SetCurSel( -1);	break;
		}

		// Fog Color
		_SetWorldFogColor( m_pWorld->GetFogColor( I3FRM_FOG_WORLD));

		// Fog Density
		i3::ftoa( m_pWorld->GetFogDensity( I3FRM_FOG_WORLD), conv);
		m_WorldFogDensityCtrl.SetWindowText( conv);

		// Fog Near
		i3::ftoa( m_pWorld->GetFogNear( I3FRM_FOG_WORLD), conv);
		m_WorldFogNearCtrl.SetWindowText( conv);

		// Fog Far
		i3::ftoa( m_pWorld->GetFogFar( I3FRM_FOG_WORLD), conv);
		m_WorldFogFarCtrl.SetWindowText( conv);

		if( g_pScene != NULL)
		{
			g_pScene->SetFogEnable( m_pWorld->GetFogEnable( I3FRM_FOG_WORLD));
			g_pScene->SetFogMode( m_pWorld->GetFogMode( I3FRM_FOG_WORLD));
			g_pScene->SetFogDensity( m_pWorld->GetFogDensity( I3FRM_FOG_WORLD));
			g_pScene->SetFogColor( m_pWorld->GetFogColor( I3FRM_FOG_WORLD));
			g_pScene->SetFogNear( m_pWorld->GetFogNear( I3FRM_FOG_WORLD));
			g_pScene->SetFogFar( m_pWorld->GetFogFar( I3FRM_FOG_WORLD));
		}

		/////////////////////////
		// Sky
		if( m_pWorld->getSky() != NULL)
			m_SkyPathCtrl.SetWindowText( m_pWorld->getSky()->GetName());
		else
			m_SkyPathCtrl.SetWindowText( "(None)");

		// Fog
		if( m_pWorld->GetFogEnable( I3FRM_FOG_SKY))
			m_SkyFogEnableCtrl.SetCheck( BST_CHECKED);
		else
			m_SkyFogEnableCtrl.SetCheck( BST_UNCHECKED);

		switch( m_pWorld->GetFogMode( I3FRM_FOG_SKY))
		{
			case I3G_FOG_LINEAR :	m_SkyFogModeCtrl.SetCurSel( 0);	break;
			case I3G_FOG_EXP :		m_SkyFogModeCtrl.SetCurSel( 1);	break;
			case I3G_FOG_EXP2 :		m_SkyFogModeCtrl.SetCurSel( 2);	break;
			default :				m_SkyFogModeCtrl.SetCurSel( -1);	break;
		}

		// Fog Color
		_SetSkyFogColor( m_pWorld->GetFogColor( I3FRM_FOG_SKY));

		// Fog Density
		i3::ftoa( m_pWorld->GetFogDensity( I3FRM_FOG_SKY), conv);
		m_SkyFogDensityCtrl.SetWindowText( conv);

		// Fog Near
		i3::ftoa( m_pWorld->GetFogNear( I3FRM_FOG_SKY), conv);
		m_SkyFogNearCtrl.SetWindowText( conv);

		// Fog Far
		i3::ftoa( m_pWorld->GetFogFar( I3FRM_FOG_SKY), conv);
		m_SkyFogFarCtrl.SetWindowText( conv);


		////////////////////////
		// World Collision
		if( m_pWorld->getWorldCollision() != NULL)
			m_WorldCollCtrl.SetWindowText( m_pWorld->getWorldCollision()->GetName());
		else
			m_WorldCollCtrl.SetWindowText( "(None)");

		if( m_pWorld->getHitCollision() != NULL)
			m_HitCollCtrl.SetWindowText( m_pWorld->getHitCollision()->GetName());
		else
			m_HitCollCtrl.SetWindowText( "(None)");

		if( m_pWorld->getLightVolumeCollision() != NULL)
			m_LightCollCtrl.SetWindowText( m_pWorld->getLightVolumeCollision()->GetName());
		else
			m_LightCollCtrl.SetWindowText( "(None)");
	}
}

void CProp_LvElm_World::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);
	
	if( pObj != NULL)
	{
		m_pWorldElm = (i3LevelWorld *) pObj;
		m_pWorldRes = (i3LevelResWorld *) m_pWorldElm->getRes();
		m_pWorld = m_pWorldRes->getWorld();

		I3ASSERT( m_pWorld == m_pWorldElm->getWorld());

		_UpdateInfo();	
	}
	else
	{
		m_pWorldElm = NULL;
		m_pWorldRes = NULL;
		m_pWorld = NULL;
	}
}


BEGIN_MESSAGE_MAP(CProp_LvElm_World, i3TDKDialogBase)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_WORLD_SEL, OnBnClickedBtnWorldSel)
	ON_BN_CLICKED(IDC_CHK_ENFOGWORLD, OnBnClickedChkEnfogworld)
	ON_BN_CLICKED(IDC_BTN_SEL_AMBIENT, OnBnClickedBtnSelAmbient)
	ON_EN_KILLFOCUS(IDC_EDIT_WFDENSKTY, OnEnKillfocusEditWfdenskty)
	ON_CBN_SELCHANGE(IDC_COMBO_SFMODE, OnCbnSelchangeComboSfmode)
	ON_EN_KILLFOCUS(IDC_EDIT_WFNEAR, OnEnKillfocusEditWfnear)
	ON_EN_KILLFOCUS(IDC_EDIT_WFFAR, OnEnKillfocusEditWffar)
	ON_BN_CLICKED(IDC_BTN_SELSKYPATH, OnBnClickedBtnSelskypath)
	ON_BN_CLICKED(IDC_CHK_ENFOG_SKY, OnBnClickedChkEnfogSky)
	ON_BN_CLICKED(IDC_BTN_SEL_AMBIENT2, OnBnClickedBtnSelAmbient2)
	ON_EN_KILLFOCUS(IDC_EDIT_SFDENSITY, OnEnKillfocusEditSfdensity)
	ON_CBN_SELCHANGE(IDC_COMBO_WFMODE, OnCbnSelchangeComboWfmode)
	ON_EN_KILLFOCUS(IDC_EDIT_SFNEAR, OnEnKillfocusEditSfnear)
	ON_EN_KILLFOCUS(IDC_EDIT_SFFAR, OnEnKillfocusEditSffar)
	ON_BN_CLICKED(IDC_BTN_WORLDCOL_SEL, OnBnClickedBtnWorldcolSel)
	ON_BN_CLICKED(IDC_BTN_HITCOL_SEL, OnBnClickedBtnHitcolSel)
	ON_BN_CLICKED(IDC_BTN_LIGHTVOLUME, OnBnClickedBtnLightvolume)
	ON_BN_CLICKED(IDC_BTN_SECTIONEDIT, OnBnClickedBtnSectionedit)
	ON_BN_CLICKED(IDC_BTN_SECTOREDIT,OnBnClickedBtnSectoredit)
END_MESSAGE_MAP()


// CProp_LvElm_World message handlers

BOOL CProp_LvElm_World::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rt;

		m_WorldFogColorCtrl.GetWindowRect( &rt);
		ScreenToClient( &rt);
		m_COL_WorldFog.Create( WS_CHILD | WS_VISIBLE, rt, this, ID_COLOR_FOG);

		m_SkyFogColorCtrl.GetWindowRect( &rt);
		ScreenToClient( &rt);
		m_COL_SkyFog.Create( WS_CHILD | WS_VISIBLE, rt, this, ID_COLOR_SKY_FOG);
	}

	i3Level::RegisterUpdateNotify( m_hWnd, i3LevelElement3D::static_meta(), I3_TDK_UPDATE_EDIT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProp_LvElm_World::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void CProp_LvElm_World::OnOK()
{
	if( m_WorldFogDensityCtrl.GetModify() || 
		m_WorldFogNearCtrl.GetModify() ||
		m_WorldFogFarCtrl.GetModify() ||
		m_SkyFogDensityCtrl.GetModify() ||
		m_SkyFogNearCtrl.GetModify() ||
		m_SkyFogFarCtrl.GetModify())
	{
		i3LevelUndo::ADD_Edit( m_pWorldElm);
	}

	if( m_WorldFogDensityCtrl.GetModify())
	{
		OnEnKillfocusEditWfdenskty();
	}

	if( m_WorldFogNearCtrl.GetModify())
	{
		OnEnKillfocusEditWfnear();
	}

	if( m_WorldFogFarCtrl.GetModify())
	{
		OnEnKillfocusEditWffar();
	}

	if( m_SkyFogDensityCtrl.GetModify())
		OnEnKillfocusEditSfdensity();

	if( m_SkyFogNearCtrl.GetModify())
		OnEnKillfocusEditSfnear();

	if( m_SkyFogFarCtrl.GetModify())
		OnEnKillfocusEditSffar();
}

void CProp_LvElm_World::OnCancel()
{
}

bool CProp_LvElm_World::GetSgFile( CString & strFile)
{
	CFileDialog	dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"Scene-Graph File(*.i3s)|*.i3s||", this);

	if( dlg.DoModal() != IDOK)
		return false;

	strFile = dlg.GetPathName();

	return true;
}

void CProp_LvElm_World::OnBnClickedBtnWorldSel()
{
	CString strFile;

	if( GetSgFile( strFile) == false)
		return;

	i3LevelResSceneGraph * pRes = (i3LevelResSceneGraph *) g_pResDB->GetRes( LPCTSTR( strFile));
	if( pRes == NULL)
	{
		AfxMessageBox( "해당 파일을 읽어 들일 수 없습니다.");
		return;
	}

	i3LevelUndo::ADD_Edit( m_pWorldElm);

	pRes->OnValidate( g_pScene, false);

	m_pWorldRes->SetWorld( pRes);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldRes);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorld);
}

void CProp_LvElm_World::OnBnClickedChkEnfogworld()
{
	if( m_pWorld->GetFogEnable( I3FRM_FOG_WORLD) != ( m_WorldFogEnableCtrl.GetCheck() == BST_CHECKED))
	{
		i3LevelUndo::ADD_Edit( m_pWorldElm);

		m_pWorld->SetFogEnable( m_WorldFogEnableCtrl.GetCheck() == BST_CHECKED, I3FRM_FOG_WORLD);

		g_pScene->SetFogEnable( m_WorldFogEnableCtrl.GetCheck() == BST_CHECKED);

		m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldRes);
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorld);
	}
}


void CProp_LvElm_World::OnBnClickedBtnSelAmbient()
{
	i3TDKColorPicker dlg;
	COLORREAL col;

	i3Color::Set( &col, m_pWorld->GetFogColor( I3FRM_FOG_WORLD));

	if( dlg.Execute( &col))
	{
		if( i3Color::IsSame( &col, dlg.getSelectedColor()) == FALSE)
		{
			COLOR col2;

			i3Color::Set( &col2, dlg.getSelectedColor());
	
			i3LevelUndo::ADD_Edit( m_pWorldElm);

			m_pWorld->SetFogColor( &col2);
			g_pScene->SetFogColor( &col2);

			m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

			_SetWorldFogColor( &col2);
		}
	}
}

void CProp_LvElm_World::OnEnKillfocusEditWfdenskty()
{
	char conv[128];

	if( m_WorldFogDensityCtrl.GetModify())
	{
		i3LevelUndo::ADD_Edit( m_pWorldElm);

		m_WorldFogDensityCtrl.GetWindowText( conv, sizeof(conv) - 1);
		
		m_pWorld->SetFogDensity( (REAL32) atof( conv));
		g_pScene->SetFogDensity((REAL32) atof( conv));

		m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

		m_WorldFogDensityCtrl.SetModify( FALSE);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
	}
}

void CProp_LvElm_World::OnCbnSelchangeComboWfmode()
{
	i3LevelUndo::ADD_Edit( m_pWorldElm);

	switch( m_WorldFogModeCtrl.GetCurSel())
	{
		case 0 :
			{
				m_pWorld->SetFogMode( I3G_FOG_LINEAR);	
				g_pScene->SetFogMode( I3G_FOG_LINEAR);
			}break;
		case 1 :
			{
				m_pWorld->SetFogMode( I3G_FOG_EXP);	
				g_pScene->SetFogMode( I3G_FOG_EXP);
			}break;
		case 2 :
			{
				m_pWorld->SetFogMode( I3G_FOG_EXP2);
				g_pScene->SetFogMode( I3G_FOG_EXP2);
			}break;
	}

	m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
}

void CProp_LvElm_World::OnEnKillfocusEditWfnear()
{
	if( m_WorldFogNearCtrl.GetModify())
	{
		i3LevelUndo::ADD_Edit( m_pWorldElm);

		char conv[128];

		m_WorldFogNearCtrl.GetWindowText( conv, sizeof(conv) - 1);

		m_pWorld->SetFogNear( (REAL32) atof( conv));
		g_pScene->SetFogNear( (REAL32) atof( conv));

		m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

		m_WorldFogNearCtrl.SetModify( FALSE);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
	}
}

void CProp_LvElm_World::OnEnKillfocusEditWffar()
{
	if( m_WorldFogFarCtrl.GetModify())
	{
		i3LevelUndo::ADD_Edit( m_pWorldElm);

		char conv[128];

		m_WorldFogFarCtrl.GetWindowText( conv, sizeof(conv) - 1);

		m_pWorld->SetFogFar( (REAL32) atof( conv));
		g_pScene->SetFogFar( (REAL32) atof( conv));

		m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

		m_WorldFogFarCtrl.SetModify( FALSE);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
	}
}

void CProp_LvElm_World::OnBnClickedBtnSelskypath()
{
	CString strFile;

	if( GetSgFile( strFile) == false)
		return;

	i3LevelUndo::ADD_Edit( m_pWorldElm);

	i3LevelResSceneGraph * pRes = (i3LevelResSceneGraph *) g_pResDB->GetRes( LPCTSTR( strFile));
	if( pRes == NULL)
	{
		AfxMessageBox( "해당 파일을 읽어 들일 수 없습니다.");
		return;
	}

	pRes->OnValidate( g_pScene, false);

	m_pWorldRes->SetSky( pRes);

	m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

	m_SkyPathCtrl.SetWindowText( pRes->getPath());

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldRes);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorld);
}

void CProp_LvElm_World::OnBnClickedChkEnfogSky()
{
	i3LevelUndo::ADD_Edit( m_pWorldElm);

	if( m_SkyFogEnableCtrl.GetCheck() == BST_CHECKED)
		m_pWorld->SetFogEnable( TRUE, I3FRM_FOG_SKY);
	else
		m_pWorld->SetFogEnable( FALSE, I3FRM_FOG_SKY);

	m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldRes);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorld);
}


void CProp_LvElm_World::OnBnClickedBtnSelAmbient2()
{
	i3TDKColorPicker dlg;
	COLORREAL col;

	i3Color::Set( &col, m_pWorld->GetFogColor( I3FRM_FOG_SKY));

	if( dlg.Execute( &col))
	{
		if( i3Color::IsSame( &col, dlg.getSelectedColor()) == FALSE)
		{
			i3LevelUndo::ADD_Edit( m_pWorldElm);

			COLOR col2;

			i3Color::Set( &col2, dlg.getSelectedColor());

			m_pWorld->SetFogColor( &col2, I3FRM_FOG_SKY);
			m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

			_SetSkyFogColor( &col2);
		}
	}
}

void CProp_LvElm_World::OnEnKillfocusEditSfdensity()
{
	if( m_SkyFogDensityCtrl.GetModify())
	{
		i3LevelUndo::ADD_Edit( m_pWorldElm);

		char conv[128];

		m_SkyFogDensityCtrl.GetWindowText( conv, sizeof(conv) - 1);
		
		m_pWorld->SetFogDensity( (REAL32) atof( conv), I3FRM_FOG_SKY);
		m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

		m_SkyFogDensityCtrl.SetModify( FALSE);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
	}
}

void CProp_LvElm_World::OnCbnSelchangeComboSfmode()
{
	i3LevelUndo::ADD_Edit( m_pWorldElm);

	switch( m_WorldFogModeCtrl.GetCurSel())
	{
		case 0 :	m_pWorld->SetFogMode( I3G_FOG_LINEAR,	I3FRM_FOG_SKY);	break;
		case 1 :	m_pWorld->SetFogMode( I3G_FOG_EXP,		I3FRM_FOG_SKY);	break;
		case 2 :	m_pWorld->SetFogMode( I3G_FOG_EXP2,		I3FRM_FOG_SKY);	break;
	}
	m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
}

void CProp_LvElm_World::OnEnKillfocusEditSfnear()
{
	if( m_SkyFogNearCtrl.GetModify())
	{
		i3LevelUndo::ADD_Edit( m_pWorldElm);

		char conv[128];

		m_SkyFogNearCtrl.GetWindowText( conv, sizeof(conv) - 1);

		m_pWorld->SetFogNear( (REAL32) atof( conv), I3FRM_FOG_SKY);
		m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

		m_SkyFogNearCtrl.SetModify( FALSE);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
	}
}

void CProp_LvElm_World::OnEnKillfocusEditSffar()
{
	if( m_SkyFogFarCtrl.GetModify())
	{
		i3LevelUndo::ADD_Edit( m_pWorldElm);
		char conv[128];

		m_SkyFogFarCtrl.GetWindowText( conv, sizeof(conv) - 1);

		m_pWorld->SetFogFar( (REAL32) atof( conv), I3FRM_FOG_SKY);
		m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

		m_SkyFogFarCtrl.SetModify( FALSE);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
	}
}

void CProp_LvElm_World::OnBnClickedBtnWorldcolSel()
{
	CString strFile;

	if( GetSgFile( strFile) == false)
		return;

	i3LevelResSceneGraph * pRes = (i3LevelResSceneGraph *) g_pResDB->GetRes( LPCTSTR( strFile));
	if( pRes == NULL)
	{
		AfxMessageBox( "해당 파일을 읽어 들일 수 없습니다.");
		return;
	}

	i3LevelUndo::ADD_Edit( m_pWorldElm);

	pRes->OnValidate( g_pScene, false);

	m_pWorldRes->SetWorldCollision( pRes);

	m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

	m_WorldCollCtrl.SetWindowText( pRes->getPath());

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldRes);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorld);
}

void CProp_LvElm_World::OnBnClickedBtnHitcolSel()
{
	CString strFile;

	if( GetSgFile( strFile) == false)
		return;

	i3LevelResSceneGraph * pRes = (i3LevelResSceneGraph *) g_pResDB->GetRes( LPCTSTR( strFile));
	if( pRes == NULL)
	{
		AfxMessageBox( "해당 파일을 읽어 들일 수 없습니다.");
		return;
	}

	i3LevelUndo::ADD_Edit( m_pWorldElm);

	pRes->OnValidate( g_pScene, false);

	m_pWorldRes->SetHitCollision( pRes);

	m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

	m_HitCollCtrl.SetWindowText( pRes->getPath());

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldRes);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorld);
}

void CProp_LvElm_World::OnBnClickedBtnLightvolume()
{
	CString strFile;

	if( GetSgFile( strFile) == false)
		return;

	i3LevelResSceneGraph * pRes = (i3LevelResSceneGraph *) g_pResDB->GetRes( LPCTSTR( strFile));
	if( pRes == NULL)
	{
		AfxMessageBox( "해당 파일을 읽어 들일 수 없습니다.");
		return;
	}

	i3LevelUndo::ADD_Edit( m_pWorldElm);

	pRes->OnValidate( g_pScene, false);

	m_pWorldRes->SetLightVolumeCollision( pRes);

	m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);

	m_LightCollCtrl.SetWindowText( pRes->getPath());

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldElm);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorldRes);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pWorld);
}

void CProp_LvElm_World::OnBnClickedBtnSectionedit()
{
	CDlgWorldSectionEdit dlg;
	i3WorldSectionTable * pTable;

	if( m_pWorld == NULL)
		return;

	pTable = m_pWorld->getWorldSectionTable();

	if( pTable == NULL)
	{
		pTable = i3WorldSectionTable::new_object();
	}
	else
	{
		pTable->AddRef();
		I3ASSERT( pTable == m_pWorldRes->getWorld()->getWorldSectionTable());
	}

	// For UndoInfo
	dlg.SetWorld( m_pWorldElm);

	if( dlg.Execute( pTable))
	{
		m_pWorld->setWorldSectionTable( pTable);

		m_pWorldRes->addResState( I3LV_RES_STATE_MODIFIED);
	}

	pTable->Release();
}

void CProp_LvElm_World::OnBnClickedBtnSectoredit()
{
	 i3MapSectorTable * pTable;

	if( m_pWorld == NULL)
		return;

	pTable = m_pWorld->getMapSectorTable();

	if( pTable == NULL)
	{
		pTable = i3MapSectorTable::new_object();

		m_pWorld->setMapSectorTable( pTable);
	}

	// For UndoInfo
	CDlgMapSectorEdit::Execute( m_pWorldElm, m_pWorldRes, pTable);
}
