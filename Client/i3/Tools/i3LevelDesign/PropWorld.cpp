// PropWorld.cpp : implementation file
//

#include "pch.h"
/*
#include "i3LevelDesign.h"
#include "PropWorld.h"
#include "ColorDlg.h"
#include ".\propworld.h"
*/
// CPropWorld dialog

//IMPLEMENT_DYNAMIC(CPropWorld, i3TDKDialogBase)
//CPropWorld::CPropWorld(CWnd* pParent /*=NULL*/)
/*	: i3TDKDialogBase(CPropWorld::IDD, pParent)
	, m_WfDensity(0)
	, m_SfDensity(0)
	, m_WfNear(0)
	, m_SfNear(0)
	, m_WfFar(0)
	, m_SfFar(0)
{
}

CPropWorld::~CPropWorld()
{
}

void CPropWorld::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_SHOWRENDERABLEMESH, m_bShowRenderableMesh);
	DDX_Control(pDX, IDC_ED_WORLDCOL, m_edWorldCol);
	DDX_Control(pDX, IDC_ED_HEROCOL, m_edHeroCol);
	DDX_Control(pDX, IDC_ED_HITCOL, m_edHitCol);
	DDX_Control(pDX, IDC_BTN_WORLDCOL_SEL, m_btnWorldColSel);
	DDX_Control(pDX, IDC_BTN_HEROCOL_SEL, m_btnHeroColSel);
	DDX_Control(pDX, IDC_BTN_HITCOL_SEL, m_btnHitColSel);
	DDX_Control(pDX, IDC_COL_RESET, m_btnReset);
	DDX_Control(pDX, IDC_CHK_ENFOGWORLD, m_bEnableFog_World);
	DDX_Control(pDX, IDC_CHK_ENFOG_SKY, m_bEnableFog_Sky);
	DDX_Control(pDX, IDC_BTN_SEL_AMBIENT, m_btnSelAmbient);
	DDX_Control(pDX, IDC_BTN_SEL_AMBIENT2, m_btnSelAmbient_Sky);
	DDX_Control(pDX, IDC_STATIC_AMBIENT, m_stAmbient);
	DDX_Control(pDX, IDC_STATIC_AMBIENT2, m_stAmbient_Sky);
	DDX_Control(pDX, IDC_EDIT_WFDENSKTY, m_edWfDensity);
	DDX_Control(pDX, IDC_EDIT_SFDENSITY, m_edSfDensity);
	DDX_Control(pDX, IDC_COMBO_WFMODE, m_cmbWFMode);
	DDX_Control(pDX, IDC_COMBO_SFMODE, m_cmbSFMode);
	DDX_Control(pDX, IDC_EDIT_WFNEAR, m_edWFNear);
	DDX_Control(pDX, IDC_EDIT_SFNEAR, m_edSFNear);
	DDX_Control(pDX, IDC_EDIT_WFFAR, m_edWfFar);
	DDX_Control(pDX, IDC_EDIT_SFFAR, m_edSfFar);
	DDX_Control(pDX, IDC_ED_SKYPATH, m_edSkyPath);
	DDX_Control(pDX, IDC_BTN_SELSKYPATH, m_btnSelSkyPath);
	DDX_Text(pDX, IDC_EDIT_WFDENSKTY, m_WfDensity);
	DDX_Text(pDX, IDC_EDIT_SFDENSITY, m_SfDensity);
	DDX_Text(pDX, IDC_EDIT_WFNEAR, m_WfNear);
	DDX_Text(pDX, IDC_EDIT_SFNEAR, m_SfNear);
	DDX_Text(pDX, IDC_EDIT_WFFAR, m_WfFar);
	DDX_Text(pDX, IDC_EDIT_SFFAR, m_SfFar);
	DDX_Control(pDX, IDC_ED_LIGHTVOLUME, m_edLightVolume);
	DDX_Control(pDX, IDC_BTN_LIGHTVOLUME, m_btnLightVolume);
}

BEGIN_MESSAGE_MAP(CPropWorld, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_BTN_WORLDCOL_SEL, OnBnClickedBtnWorldcolSel)
	ON_BN_CLICKED(IDC_BTN_HEROCOL_SEL, OnBnClickedBtnHerocolSel)
	ON_BN_CLICKED(IDC_BTN_HITCOL_SEL, OnBnClickedBtnHitcolSel)
	ON_BN_CLICKED(IDC_CHK_SHOWRENDERABLEMESH, OnBnClickedChkShowrenderablemesh)
	ON_BN_CLICKED(IDC_COL_RESET, OnBnClickedColReset)
	ON_BN_CLICKED(IDC_BTN_SEL_AMBIENT, OnBnClickedBtnSelAmbient)
	ON_BN_CLICKED(IDC_BTN_SEL_AMBIENT2, OnBnClickedBtnSelAmbient2)
	ON_BN_CLICKED(IDC_BTN_SELSKYPATH, OnBnClickedBtnSelskypath)
	ON_BN_CLICKED(IDC_CHK_ENFOG_SKY, OnBnClickedChkEnfogSky)
	ON_BN_CLICKED(IDC_CHK_ENFOGWORLD, OnBnClickedChkEnfogworld)
	ON_CBN_SELCHANGE(IDC_COMBO_SFMODE, OnCbnSelchangeComboSfmode)
	ON_CBN_SELCHANGE(IDC_COMBO_WFMODE, OnCbnSelchangeComboWfmode)
	ON_EN_CHANGE(IDC_EDIT_SFDENSITY, OnEnChangeEditSfdensity)
	ON_EN_CHANGE(IDC_EDIT_SFFAR, OnEnChangeEditSffar)
	ON_EN_CHANGE(IDC_EDIT_SFNEAR, OnEnChangeEditSfnear)
	ON_EN_CHANGE(IDC_EDIT_WFDENSKTY, OnEnChangeEditWfdenskty)
	ON_EN_CHANGE(IDC_EDIT_WFFAR, OnEnChangeEditWffar)
	ON_EN_CHANGE(IDC_EDIT_WFNEAR, OnEnChangeEditWfnear)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_LIGHTVOLUME, OnBnClickedBtnLightvolume)
END_MESSAGE_MAP()

// CPropWorld message handlers
void CPropWorld::SetObject( i3ElementBase * pObj)
{
	if( m_pWorld == pObj)
		return ;

	m_pWorld = (i3LevelWorld*)pObj;

	if( !i3::same_of<i3LevelWorld*>(pObj)) //->IsExactTypeOf( i3LevelWorld::static_meta()) == FALSE)
	{
		m_bShowRenderableMesh.EnableWindow( FALSE);
		m_edWorldCol.EnableWindow( FALSE);
		m_edHeroCol.EnableWindow( FALSE);
		m_edHitCol.EnableWindow( FALSE);
		m_btnWorldColSel.EnableWindow( FALSE);
		m_btnHeroColSel.EnableWindow( FALSE);
		m_btnHitColSel.EnableWindow( FALSE);
		return ;
	}

	UpdateWorldData();
}

void CPropWorld::UpdateWorldData()
{
	char szTemp[MAX_PATH];

	//
	if( m_pWorld->GetFogEnable( I3FRM_FOG_WORLD))
		m_bEnableFog_World.SetCheck( BST_CHECKED);
	else
		m_bEnableFog_World.SetCheck( BST_UNCHECKED);

	//
	if( m_pWorld->GetFogEnable( I3FRM_FOG_SKY))
		m_bEnableFog_Sky.SetCheck( BST_CHECKED);
	else
		m_bEnableFog_Sky.SetCheck( BST_UNCHECKED);

	{
		m_edWfDensity.EnableWindow( m_pWorld->GetFogEnable( I3FRM_FOG_WORLD));
		m_cmbWFMode.EnableWindow( m_pWorld->GetFogEnable( I3FRM_FOG_WORLD));
		m_edWFNear.EnableWindow( m_pWorld->GetFogEnable( I3FRM_FOG_WORLD));
		m_edWfFar.EnableWindow( m_pWorld->GetFogEnable( I3FRM_FOG_WORLD));
		m_stAmbient.EnableWindow( m_pWorld->GetFogEnable( I3FRM_FOG_WORLD));

		m_edSfDensity.EnableWindow( m_pWorld->GetFogEnable( I3FRM_FOG_SKY));
		m_cmbSFMode.EnableWindow( m_pWorld->GetFogEnable( I3FRM_FOG_SKY));
		m_edSFNear.EnableWindow( m_pWorld->GetFogEnable( I3FRM_FOG_SKY));
		m_edSfFar.EnableWindow( m_pWorld->GetFogEnable( I3FRM_FOG_SKY));
		m_stAmbient_Sky.EnableWindow( m_pWorld->GetFogEnable( I3FRM_FOG_SKY));
	}

	if( m_pWorld->GetFogEnable( I3FRM_FOG_WORLD))
	{
		sprintf( szTemp, "%f", m_pWorld->GetFogDensity( I3FRM_FOG_WORLD));
		m_edWfDensity.SetWindowText( szTemp);

		m_cmbWFMode.SetCurSel( (INT32)(m_pWorld->GetFogMode( I3FRM_FOG_WORLD)));

		sprintf( szTemp, "%f", m_pWorld->GetFogNear( I3FRM_FOG_WORLD));
		m_edWFNear.SetWindowText( szTemp);

		sprintf( szTemp, "%f", m_pWorld->GetFogFar( I3FRM_FOG_WORLD));
		m_edWfFar.SetWindowText( szTemp);
	}

	if( m_pWorld->GetFogEnable( I3FRM_FOG_SKY))
	{
		sprintf( szTemp, "%f", m_pWorld->GetFogDensity( I3FRM_FOG_SKY));
		m_edSfDensity.SetWindowText( szTemp);

		m_cmbSFMode.SetCurSel( (INT32)(m_pWorld->GetFogMode( I3FRM_FOG_SKY)));

		sprintf( szTemp, "%f", m_pWorld->GetFogNear( I3FRM_FOG_SKY));
		m_edSFNear.SetWindowText( szTemp);

		sprintf( szTemp, "%f", m_pWorld->GetFogFar( I3FRM_FOG_SKY));
		m_edSfFar.SetWindowText( szTemp);
	}

	i3SceneGraphInfo * pInfo = m_pWorld->getSkyScene();
	if( pInfo != NULL)
	{
		if( pInfo->hasName())
		{
			m_edSkyPath.SetWindowText( pInfo->GetName());
		}
		else
		{
			m_edSkyPath.SetWindowText( "Invalid path");
		}
	}
	else
	{
		m_edSkyPath.SetWindowText("Not Assigned");
	}

	//Collision
	if( m_pWorld->IsShowMesh() )
	{
		m_bShowRenderableMesh.SetCheck( BST_CHECKED);
	}
	else
	{
		m_bShowRenderableMesh.SetCheck( BST_UNCHECKED);
	}

	i3SceneGraphInfo * pWorldCol = m_pWorld->getWorldCol();
	if( pWorldCol != NULL)
	{
		m_edWorldCol.SetWindowText((LPCTSTR)pWorldCol->GetName());
	}
	else
	{
		m_edWorldCol.SetWindowText((LPCTSTR)"Not Assigned");
	}

	i3SceneGraphInfo * pHeroCol = m_pWorld->getHeroCol();
	if( pHeroCol != NULL)
	{
		m_edHeroCol.SetWindowText((LPCTSTR)pHeroCol->GetName());
	}
	else
	{
		m_edHeroCol.SetWindowText((LPCTSTR)"Not Assigned");
	}

	i3SceneGraphInfo * pHitCol = m_pWorld->getHitCol();
	if( pHitCol != NULL)
	{
		m_edHitCol.SetWindowText((LPCTSTR)pHitCol->GetName());
	}
	else
	{
		m_edHitCol.SetWindowText((LPCTSTR)"Not Assigned");
	}

	i3SceneGraphInfo * pLightVolumeCol = m_pWorld->getLightVolumeCol();
	if( pLightVolumeCol != NULL)
	{
		m_edLightVolume.SetWindowText((LPCTSTR)pLightVolumeCol->GetName());
	}
	else
	{
		m_edLightVolume.SetWindowText( (LPCTSTR)"Not Assigned");
	}
}

void CPropWorld::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
}

void CPropWorld::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
}

void CPropWorld::OnBnClickedBtnWorldcolSel()
{
	// TODO: Add your control notification handler code here
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Scene Collision File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)
		return;

	i3SceneFile File;
	if( File.Load( LPCTSTR( Dlg.GetPathName())) == STREAM_ERR)
	{
		AfxMessageBox( "맞지 않는 형식의 파일입니다.", MB_OK, 0);
		return;
	}

	char szTemp[MAX_PATH];
	i3Node * pNode = (i3Node*)File.GetSceneGraph();
	if( pNode != NULL)
	{
		i3SceneGraphInfo * pInfo = i3SceneGraphInfo::new_object_ref();
		pInfo->setInstanceSg( pNode);

		i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), LPCTSTR( Dlg.GetPathName()), szTemp);
		pNode->SetName( szTemp);
		pInfo->SetName( szTemp);
		pInfo->SetExternRes();

		m_pWorld->setWorldCol( pInfo);
		m_edWorldCol.SetWindowText(szTemp);
	}
}

void CPropWorld::OnBnClickedBtnHerocolSel()
{
	// TODO: Add your control notification handler code here
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Scene Collision File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)
		return;

	i3SceneFile File;
	if( File.Load( LPCTSTR( Dlg.GetPathName())) == STREAM_ERR)
	{
		AfxMessageBox( "맞지 않는 형식의 파일입니다.", MB_OK, 0);
		return ;
	}

	char szTemp[MAX_PATH];
	i3Node * pNode = (i3Node*)File.GetSceneGraph();
	if( pNode != NULL)
	{
		i3SceneGraphInfo * pInfo = i3SceneGraphInfo::new_object_ref();
		pInfo->setInstanceSg( pNode);

		i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), LPCTSTR( Dlg.GetPathName()), szTemp);
		pNode->SetName( szTemp);
		pInfo->SetName( szTemp);
		pInfo->SetExternRes();

		m_pWorld->setHeroCol( pInfo);
		m_edHeroCol.SetWindowText(szTemp);
	}
}

void CPropWorld::OnBnClickedBtnHitcolSel()
{
	// TODO: Add your control notification handler code here
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Scene Collision File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)return;

	i3SceneFile File;
	if( File.Load( LPCTSTR( Dlg.GetPathName())) == STREAM_ERR)
	{
		AfxMessageBox( "맞지 않는 형식의 파일입니다.", MB_OK, 0);
		return ;
	}

	char szTemp[MAX_PATH];
	i3Node * pNode = (i3Node*)File.GetSceneGraph();
	if( pNode != NULL)
	{
		i3SceneGraphInfo * pInfo = i3SceneGraphInfo::new_object_ref();
		pInfo->setInstanceSg( pNode);

		i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), LPCTSTR( Dlg.GetPathName()), szTemp);
		pNode->SetName( szTemp);
		pInfo->SetName( szTemp);
		pInfo->SetExternRes();

		m_pWorld->setHitCol( pInfo);
		m_edHitCol.SetWindowText(szTemp);
	}
}

void CPropWorld::OnBnClickedBtnLightvolume()
{
	// TODO: Add your control notification handler code here
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Scene Collision File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)return;

	i3SceneFile File;
	if( File.Load( LPCTSTR( Dlg.GetPathName())) == STREAM_ERR)
	{
		AfxMessageBox( "맞지 않는 형식의 파일입니다.", MB_OK, 0);
		return ;
	}

	char szTemp[MAX_PATH];
	i3Node * pNode = (i3Node*)File.GetSceneGraph();
	if( pNode != NULL)
	{
		i3SceneGraphInfo * pInfo = i3SceneGraphInfo::new_object_ref();
		pInfo->setInstanceSg( pNode);

		i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), LPCTSTR( Dlg.GetPathName()), szTemp);
		pNode->SetName( szTemp);
		pInfo->SetName( szTemp);
		pInfo->SetExternRes();

		m_pWorld->setLightVolumeCol( pInfo);
		m_edLightVolume.SetWindowText(szTemp);
	}
}


void CPropWorld::OnBnClickedChkShowrenderablemesh()
{
	// TODO: Add your control notification handler code here
	if( m_pWorld != NULL)
	{
		if(m_bShowRenderableMesh.GetCheck() == BST_CHECKED)
            m_pWorld->ShowRenderableMesh( TRUE);
		else
			m_pWorld->ShowRenderableMesh( FALSE);
	}
}

void CPropWorld::OnBnClickedColReset()
{
	// TODO: Add your control notification handler code here
	if( m_pWorld != NULL)
	{
		m_pWorld->setWorldCol( NULL);
		m_pWorld->setHeroCol( NULL);
		m_pWorld->setHitCol( NULL);

		UpdateWorldData();
	}
}

BOOL CPropWorld::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	m_cmbWFMode.AddString("NONE");
	m_cmbWFMode.AddString("LINEAR");
	m_cmbWFMode.AddString("EXP");
	m_cmbWFMode.AddString("EXP2");

	m_cmbSFMode.AddString("NONE");
	m_cmbSFMode.AddString("LINEAR");
	m_cmbSFMode.AddString("EXP");
	m_cmbSFMode.AddString("EXP2");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPropWorld::OnBnClickedBtnSelAmbient()
{
	// TODO: Add your control notification handler code here
	CColorDlg ColorDlg; 
	ColorDlg.SetColor( m_pWorld->GetFogColor( I3FRM_FOG_WORLD));

	if(ColorDlg.DoModal() == IDOK)
	{
		//Save Color Proeprty
		I3COLOR Col;
		ColorDlg.GetColor( &Col);
		m_pWorld->SetFogColor( &Col, I3FRM_FOG_WORLD);
		Invalidate();
	}
}

void CPropWorld::OnBnClickedBtnSelAmbient2()
{
	// TODO: Add your control notification handler code here
	CColorDlg ColorDlg; 
	ColorDlg.SetColor( m_pWorld->GetFogColor( I3FRM_FOG_SKY));
	if(ColorDlg.DoModal() == IDOK)
	{
		//Save Color Proeprty
		I3COLOR Col;
		ColorDlg.GetColor( &Col);
		m_pWorld->SetFogColor( &Col, I3FRM_FOG_SKY);
		Invalidate();
	}
}

void CPropWorld::OnBnClickedBtnSelskypath()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Scene Sky File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)return;

	i3SceneFile File;
	if( File.Load( LPCTSTR( Dlg.GetPathName())) == STREAM_ERR)
	{
		AfxMessageBox( "맞지 않는 형식의 파일입니다.", MB_OK, 0);
		return ;
	}

	char szTemp[MAX_PATH];
	i3Node * pNode = (i3Node*)File.GetSceneGraph();
	if( pNode != NULL)
	{
		i3SceneGraphInfo * pInfo = i3SceneGraphInfo::new_object_ref();
		pInfo->setInstanceSg( pNode);

		i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), LPCTSTR( Dlg.GetPathName()), szTemp);
		pNode->SetName( szTemp);
		pInfo->SetName( szTemp);
		pInfo->SetExternRes();

		m_pWorld->setSkyScene( pInfo);
		m_edSkyPath.SetWindowText(szTemp);
	}
}

void CPropWorld::OnBnClickedChkEnfogSky()
{
	// TODO: Add your control notification handler code here
	if( m_pWorld != NULL)
	{
		if(m_bEnableFog_Sky.GetCheck() == BST_CHECKED)
			m_pWorld->SetFogEnable( TRUE, I3FRM_FOG_SKY);
		else
			m_pWorld->SetFogEnable( FALSE, I3FRM_FOG_SKY);
	}

	UpdateWorldData();
}

void CPropWorld::OnBnClickedChkEnfogworld()
{
	// TODO: Add your control notification handler code here
	if( m_pWorld != NULL)
	{
		if(m_bEnableFog_World.GetCheck() == BST_CHECKED)
			m_pWorld->SetFogEnable( TRUE, I3FRM_FOG_WORLD);
		else
			m_pWorld->SetFogEnable( FALSE, I3FRM_FOG_WORLD);
	}

	UpdateWorldData();
}

void CPropWorld::OnCbnSelchangeComboSfmode()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	m_pWorld->SetFogMode( (I3G_FOG_MODE)m_cmbSFMode.GetCurSel(), I3FRM_FOG_SKY);
}

void CPropWorld::OnCbnSelchangeComboWfmode()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	m_pWorld->SetFogMode( (I3G_FOG_MODE)m_cmbWFMode.GetCurSel(), I3FRM_FOG_WORLD);
}

void CPropWorld::OnEnChangeEditSfdensity()
{
	UpdateData();

	m_pWorld->SetFogDensity( m_SfDensity, I3FRM_FOG_SKY);
}

void CPropWorld::OnEnChangeEditWfdenskty()
{
	UpdateData();

	m_pWorld->SetFogDensity( m_WfDensity, I3FRM_FOG_WORLD);
}

void CPropWorld::OnEnChangeEditSffar()
{
	UpdateData();

	m_pWorld->SetFogFar( m_SfFar, I3FRM_FOG_SKY);
}

void CPropWorld::OnEnChangeEditSfnear()
{
	UpdateData();

	m_pWorld->SetFogNear( m_SfNear, I3FRM_FOG_SKY);
}

void CPropWorld::OnEnChangeEditWffar()
{
	UpdateData();

	m_pWorld->SetFogFar( m_WfFar, I3FRM_FOG_WORLD);
}

void CPropWorld::OnEnChangeEditWfnear()
{
	UpdateData();

	m_pWorld->SetFogNear( m_WfNear, I3FRM_FOG_WORLD);
}

void CPropWorld::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call i3TDKDialogBase::OnPaint() for painting messages
	CRect Rect;
	COLORREF Color;	
	HBRUSH OldBrush, NewBrush;

	//Ambient
	{
		I3COLOR * pCol = m_pWorld->GetFogColor( I3FRM_FOG_WORLD);
		Color = RGB( pCol->r, pCol->g, pCol->b);
		NewBrush = CreateSolidBrush(Color);
		
		ASSERT(NewBrush);

		OldBrush = (HBRUSH) dc.SelectObject(NewBrush);

		m_stAmbient.GetWindowRect( &Rect);
		ScreenToClient( &Rect);
		dc.Rectangle(&Rect);
		dc.SelectObject(OldBrush);
		DeleteObject(NewBrush);
	}

		//Ambient
	{
		I3COLOR * pCol = m_pWorld->GetFogColor( I3FRM_FOG_SKY);
		Color = RGB( pCol->r, pCol->g, pCol->b);
		NewBrush = CreateSolidBrush(Color);
		
		ASSERT(NewBrush);

		OldBrush = (HBRUSH) dc.SelectObject(NewBrush);

		m_stAmbient_Sky.GetWindowRect( &Rect);
		ScreenToClient( &Rect);
		dc.Rectangle(&Rect);
		dc.SelectObject(OldBrush);
		DeleteObject(NewBrush);
	}
}
*/