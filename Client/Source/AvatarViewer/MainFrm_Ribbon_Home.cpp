#include "pch.h"
#include "MainFrm.h"
#include "GlobalVar.h"
#include "resource.h"

#include "DlgSelectBody.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"

static void FindWeaponList( const char * pszSubFolder, i3::vector_map<i3::string, UINT128> &list)
{
	::BOOL bDir = SetCurrentDirectory(i3ResourceFile::GetWorkingFolder());
	if (FALSE == bDir)
	{
		i3ResourceFile::SetWorkingFolder("");
		return;
	}

	char szPath[ MAX_PATH];
	char szExt[ 16];

	if( pszSubFolder != NULL)
		i3::snprintf( szPath, sizeof(szPath), "%s/Weapon/%s/*.*", i3ResourceFile::GetWorkingFolder(), pszSubFolder);
	else
		i3::snprintf( szPath, sizeof(szPath), "%s/Weapon/*.*", i3ResourceFile::GetWorkingFolder());

	HANDLE				hFind		= INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		ffd;

	hFind = ::FindFirstFile( szPath, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	BOOL bFind = TRUE;
	while( bFind)
	{
		bFind = ::FindNextFile( hFind, &ffd);

		if( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if( i3::contain_string( ffd.cFileName, ".svn") >= 0 ||
				i3::contain_string( ffd.cFileName, ".") >= 0 ||
				i3::contain_string( ffd.cFileName, "..") >= 0 ||
				i3::contain_string( ffd.cFileName, "Resource") >= 0 ||
				i3::contain_string( ffd.cFileName, "Res") >= 0 )
			{
				continue;
			}

			char szSub[ MAX_PATH];
			if( pszSubFolder != NULL)
				i3::snprintf( szSub, sizeof(szSub), "%s/%s", pszSubFolder, ffd.cFileName);
			else
				i3::snprintf( szSub, sizeof(szSub), "%s", ffd.cFileName);
			FindWeaponList( szSub, list);
		}
		else
		{
			i3::extract_fileext( ffd.cFileName, szExt);
			i3::to_upper( szExt);

			if( i3::generic_is_iequal( szExt, "I3S") )
			{
				// make Full Path
				i3::string str;
				if( pszSubFolder != NULL)
					i3::sprintf( str, "Weapon/%s/%s", pszSubFolder, ffd.cFileName);
				else
					i3::sprintf( str, "Weapon/%s", ffd.cFileName);
				
				i3::make_unix_path( str);
				i3::to_upper( str);
				UINT128 code;

				i3MD5::Gen( (char*)str.c_str(), str.size(), (char *) &code);
				
				list.insert( i3::vector_map<i3::string, UINT128>::value_type(str, code));
			}
		}
	}

	::FindClose(hFind);
}

void CMainFrame::InitRibbon_Home(void)
{
	CMFCRibbonPanel* pPanel;

	// Add "Home" category with "Clipboard" panel:
	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory( "Home", IDB_ICONS_32, IDB_ICONS_32, CSize( 16, 16), CSize( 16, 16));

	// Camera
	{
		pPanel = pCategory->AddPanel( "Camera");

		m_pBtn_Pane_CamSet = new CMFCRibbonButton( ID_CAMERA_SET,			"Set");
		pPanel->Add( m_pBtn_Pane_CamSet);
		pPanel->Add( new CMFCRibbonButton( ID_CAMERA_SAVE,			"Save"));
		pPanel->Add( new CMFCRibbonButton( ID_CAMERA_LOAD,			"Load"));
		pPanel->Add( new CMFCRibbonButton( ID_CAMERA_1PV,			"1PV"));
	}

	// View Panel
	{
		pPanel = pCategory->AddPanel( "View");

		// Grid
		{
			pPanel->Add( new CMFCRibbonButton( ID_VIEW_GRID,			"Grid",		6, 6));
		}

		// LOD
		{
			pPanel->Add( new CMFCRibbonSeparator( FALSE));
			pPanel->Add( new CMFCRibbonLabel( "LOD"));

			CMFCRibbonSlider * pSlider = new CMFCRibbonSlider( ID_OPTIONS_LOD, 80);
			pSlider->SetText( "LOD");
			pSlider->SetZoomButtons();
			pSlider->SetZoomIncrement( 1);
			pSlider->SetRange( 0, 3);

			pSlider->SetPos( 1);

			pPanel->Add( pSlider);
		}
	}

	// Environment
	{
		pPanel = pCategory->AddPanel( "Environment");

		// Clear Color
		{
			m_pCOL_ClearColor = new i3TDKRibbonColor( ID_OPTIONS_CLEARCOLOR, "Clear", TRUE, 2, 2);
			m_pCOL_ClearColor->setColor( & g_pOption->m_BackColor);
			pPanel->Add( m_pCOL_ClearColor);
		}

		// Ambient Color
		{
			m_pCOL_AmbientColor = new i3TDKRibbonColor( ID_OPTIONS_AMBIENTCOLOR, "Ambient", TRUE, 2, 2);
			m_pCOL_AmbientColor->setColor( & g_pOption->m_Ambient);
			pPanel->Add( m_pCOL_AmbientColor);
		}

		// Light Color
		{
			m_pCOL_LightColor = new i3TDKRibbonColor( ID_OPTIONS_LIGHTCOLOR, "Light", TRUE, 2, 2);
			m_pCOL_LightColor->setColor( &g_pOption->m_LightDiffuse);
			pPanel->Add( m_pCOL_LightColor);
		}

		// Light Intensity
		{
			pPanel->Add( new CMFCRibbonSeparator( FALSE));
			pPanel->Add( new CMFCRibbonLabel( "Light Intensity"));

			CMFCRibbonSlider * pSlider = new CMFCRibbonSlider( ID_OPTIONS_LIGHTINTENSITY, 80);
			pSlider->SetText( "Light Intensity");
			pSlider->SetZoomButtons();
			pSlider->SetRange( 0, 300);

			pSlider->SetPos( (INT32) (g_pOption->m_LightIntensity * 100.0f));

			pPanel->Add( pSlider);
		}

		//lisght 조정
		{
			pPanel->Add(new CMFCRibbonSeparator(FALSE));
			pPanel->Add(new CMFCRibbonLabel("Light Vertical"));

			CMFCRibbonSlider * pSlider = new CMFCRibbonSlider(ID_OPTIONS_LIGHTVERTICAL, 80);
			pSlider->SetText("Light Vertical");
			pSlider->SetZoomButtons();
			pSlider->SetRange(-180, 180);

			pSlider->SetPos((INT32)(g_pOption->m_LightRho));

			pPanel->Add(pSlider);

			pPanel->Add(new CMFCRibbonSeparator(FALSE));
			pPanel->Add(new CMFCRibbonLabel("Light Horizon"));

			pSlider = new CMFCRibbonSlider(ID_OPTIONS_LIGHTHORIZON, 80);
			pSlider->SetText("Light Horizon");
			pSlider->SetZoomButtons();
			pSlider->SetRange(-180, 180);

			pSlider->SetPos((INT32)(g_pOption->m_LightTheta));

			pPanel->Add(pSlider);
		}

		// Shadow Map Size
		{
			// Shadow
			{
				pPanel->Add( new CMFCRibbonSeparator( FALSE));
				pPanel->Add( new CMFCRibbonButton( ID_VIEW_SHADOW,			"Shadow",		5, 5));
				pPanel->Add( new CMFCRibbonSeparator( FALSE));
			}

			// Shadow Map Size
			{
				m_pCB_ShadowSize = new CMFCRibbonComboBox( ID_OPTIONS_SHADOWSIZE, FALSE, 30, "Shadow Size");

				m_pCB_ShadowSize->AddItem( "256");
				m_pCB_ShadowSize->AddItem( "512");
				m_pCB_ShadowSize->AddItem( "1024");
				m_pCB_ShadowSize->AddItem( "2048");
				m_pCB_ShadowSize->AddItem( "4096");

				pPanel->Add( m_pCB_ShadowSize);
			}
		}

		// Cube Map
		{
			pPanel->Add( new CMFCRibbonSeparator( FALSE));
			pPanel->Add( new CMFCRibbonLabel( "Reflect Map"));

			m_pBTN_ReflectMap = new CMFCRibbonButton( ID_OPTIONS_REFLECTMAP, "(None_Dummy_Length_Call)");
			pPanel->Add( m_pBTN_ReflectMap);
		}
	}

	// Tools Panel
	{
		pPanel = pCategory->AddPanel( "Tools");

		// Geometry Tool
		pPanel->Add( new CMFCRibbonButton( ID_TOOLS_GEOMETRY,			"Mesh Tool",		 7));

		// Texture Tool
		pPanel->Add( new CMFCRibbonButton( ID_TOOLS_TEXTURES,			"Texture Tool",		 8));

		// Project Config
		pPanel->Add( new CMFCRibbonButton( ID_TOOLS_PROJECTCONFIG,		"Project Config.",	-1));
	}

	// Panels
	{
		pPanel = pCategory->AddPanel( "Panes");

		m_pCHK_Pane_Avatar = new CMFCRibbonCheckBox( ID_VIEW_AVATAR, _T("Avatar Pane"));
		pPanel->Add( m_pCHK_Pane_Avatar);

		m_pCHK_Pane_Anim = new CMFCRibbonCheckBox( ID_VIEW_ANIM, _T("Animations Pane"));
		pPanel->Add( m_pCHK_Pane_Anim);

		m_pCHK_Pane_Part = new CMFCRibbonCheckBox( ID_VIEW_PART, _T("Part Pane"));
		pPanel->Add( m_pCHK_Pane_Part);

	}

	// Select Team
	{
		pPanel = pCategory->AddPanel( "Team Color");

		pPanel->Add( new CMFCRibbonButton( ID_COLOR_RED_TEAM,			"Red"		));
		pPanel->Add( new CMFCRibbonButton( ID_COLOR_BLUE_TEAM,			"Blue"		));
		pPanel->Add( new CMFCRibbonButton( ID_COLOR_OFF,				"Off"		));
	}

	// attachment option
	{
		pPanel = pCategory->AddPanel( "Attachment Option");
		pPanel->Add( new CMFCRibbonCheckBox( ID_ATTACHMENT_ATTACHRIGHTHAND,			"Attach Right Hand"		));
		pPanel->Add( new CMFCRibbonCheckBox( ID_ATTACHMENT_ATTACHBACKPACK,			"Attach Backpack"		));

		m_pCB_WeaponList = new CMFCRibbonComboBox( ID_ATTACHMENT_WEAPONLIST, FALSE, 60, "Weapon List");

		FindWeaponList( NULL, m_weaponList);

		i3::vector_map<i3::string, UINT128>::iterator it = m_weaponList.begin();

		while( it != m_weaponList.end())
		{
			i3::string name;
			i3::extract_filetitle( it->first, name);
			m_pCB_WeaponList->AddItem( name.c_str(), (DWORD_PTR)it->first.c_str());
			it++;
		}

		pPanel->Add( m_pCB_WeaponList);
	}

	// Make tool
	{
		pPanel = pCategory->AddPanel( "Make");
		pPanel->Add( new CMFCRibbonButton( ID_MAKE_SAVEMERGEDTEXTURE, "Save Macro Texture"));
		pPanel->Add( new CMFCRibbonButton( ID_MAKE_CONVERTSGBYMACROTEXTURE, "Convert UV by macro texture"));
		pPanel->Add( new CMFCRibbonButton( ID_MAKE_CONVERTBLENDINDEX, "Convert Blend Index"));
	}
}


void CMainFrame::OnCameraSetBtn( void)
{
	m_Panel_CamSet.SetCamera();
}

void CMainFrame::OnUpdateCameraSetBtn( CCmdUI * pCmdUI)
{
}

void CMainFrame::OnCameraSaveBtn()
{
	i3Vector::Copy( &g_pOption->m_vTranslation, &m_Panel_CamSet.m_vTranslation);
	i3Vector::Copy( &g_pOption->m_vRotation, &m_Panel_CamSet.m_vRotation);
}

void CMainFrame::OnUpdateCameraSaveBtn(CCmdUI * pCmdUI)
{
}

void CMainFrame::OnCameraLoadBtn()
{
	i3Vector::Copy( &m_Panel_CamSet.m_vTranslation, &g_pOption->m_vTranslation);
	i3Vector::Copy( &m_Panel_CamSet.m_vRotation, &g_pOption->m_vRotation);

	// update camera
	m_Panel_CamSet.ApplyCamera();
}

void CMainFrame::OnUpdateCameraLoadBtn(CCmdUI * pCmdUI)
{
}

void CMainFrame::OnCamera1PVBtn()
{
	if( m_bCam1PVMode == false)
	{
		INT32 boneIdx = g_pAvatar->GetBody()->FindBoneByName( "Neck");
		MATRIX * pCamMtx = g_pFramework->GetCurrentCamera()->GetMatrix();

		i3Matrix::Copy( &m_mtxSaved3PVCam, pCamMtx);

		i3Matrix::Identity( pCamMtx);
		
		VEC3D vPos, offset;
		i3Vector::Set( &offset, 0.00565f, 0.1392f, -0.029f);

		MATRIX * pBoneMtx = g_pAvatar->GetBody()->getCacheMatrix( boneIdx);

		i3Matrix::PostRotateY( pCamMtx, I3_DEG2RAD( 180.0f));

		i3Vector::Add(&vPos, &offset, i3Matrix::GetPos( pBoneMtx));

		i3Matrix::SetPos( pCamMtx, &vPos);

		g_pFramework->GetCurrentCamera()->SetFOV( I3_DEG2RAD(70.f));

		m_AvatarPreLOD = g_pAvatar->GetLOD();
		g_pAvatar->SetLOD( 0);

		m_bCam1PVMode = true;
	}
	else
	{
		g_pAvatar->SetLOD( m_AvatarPreLOD);
		i3Matrix::Copy( g_pFramework->GetCurrentCamera()->GetMatrix(), &m_mtxSaved3PVCam);

		m_bCam1PVMode = false;
	}
}

void CMainFrame::OnUpdateCamera1PVBtn(CCmdUI * pCmdUI)
{
	if( m_bCam1PVMode)
		pCmdUI->SetText( "3PV");
	else
		pCmdUI->SetText( "1PV");
}

///////////////////////////////////
// Shadow
void CMainFrame::OnViewShadow()
{
	g_pOption->m_bShadowEnable = !g_pOption->m_bShadowEnable;

	g_pFramework->setShadowEnable( g_pOption->m_bShadowEnable == TRUE);

	if( g_pRenderPanel != NULL)
		g_pRenderPanel->Invalidate();

	RefreshRibbon();
}

void CMainFrame::OnUpdateViewShadow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( g_pOption->m_bShadowEnable);
}

///////////////////////////////////
// Grid
void CMainFrame::OnViewGrid()
{
	g_pOption->m_bGridShow = !g_pOption->m_bGridShow;

	g_pFramework->SetGridShow( g_pOption->m_bGridShow == TRUE);

	if( g_pRenderPanel != NULL)
		g_pRenderPanel->Invalidate();
}

void CMainFrame::OnUpdateViewGrid(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( g_pOption->m_bGridShow);
}

////////////////////////////////////
// LOD
void CMainFrame::OnChanged_LOD()
{
}

void CMainFrame::OnUpdate_LOD( CCmdUI * pCmdUI)
{
	CMFCRibbonCmdUI * pUI = (CMFCRibbonCmdUI *) pCmdUI;
	CMFCRibbonSlider * pCtrl = (CMFCRibbonSlider *) pUI->m_pUpdated;

	// RibbonBar를 최소화시킨 상태에서 RibbonBar를 사용하면,
	// 실제 Create 단계에서 생성된 각 Ribbon Control이 사용되는 것이 아니라,
	// Clone이 MFC 내부에서 생성되어 사용된다.
	//
	// 이 이유로, Update되는 시점에서 각 Control의 Pointer를 직접 얻어와서
	// 사용하지 않으면, Update 된 값이 제대로 반영되어 있지 않은 버그가 발생한다.

	if( pCmdUI->m_pOther == NULL)
		return;

	if( g_pOption != NULL)
	{
		INT32 newLOD = pCtrl->GetPos();
		if (g_pAvatar)
		{
			INT32 oldLOD = g_pAvatar->GetLOD();

			if( oldLOD != newLOD)
			{
				//g_pAvatar->SetLOD( newLOD);

				{
					char conv[64];
					i3::snprintf( conv, _countof( conv), "%d", newLOD);

					pCtrl->SetToolTipText( conv);
				}
			}

			pCmdUI->Enable( TRUE);
		}
		
	}
	else
	{
		pCmdUI->Enable( FALSE);
	}
}

////////////////////////////////////
// Clear Color
void CMainFrame::OnClicked_ClearColor()
{
	i3Color::Set( & g_pOption->m_BackColor, m_pCOL_ClearColor->getColor());

	g_pFramework->ApplyOptionColors();
}

////////////////////////////////////
// Ambient Color
void CMainFrame::OnClicked_AmbientColor()
{
	i3Color::Set( &g_pOption->m_Ambient, m_pCOL_AmbientColor->getColor());

	g_pFramework->ApplyOptionColors();
}

////////////////////////////////////
// Light Color
void CMainFrame::OnClicked_LightColor()
{
	i3Color::Set( &g_pOption->m_LightDiffuse, m_pCOL_LightColor->getColor());

	g_pFramework->ApplyOptionColors();
}

////////////////////////////////////
// Light Intensity
void CMainFrame::OnChanged_LightIntensity()
{
}

void CMainFrame::OnUpdate_LightIntensity( CCmdUI * pCmdUI)
{
	CMFCRibbonCmdUI * pUI = (CMFCRibbonCmdUI *) pCmdUI;
	CMFCRibbonSlider * pCtrl = (CMFCRibbonSlider *) pUI->m_pUpdated;

	// RibbonBar를 최소화시킨 상태에서 RibbonBar를 사용하면,
	// 실제 Create 단계에서 생성된 각 Ribbon Control이 사용되는 것이 아니라,
	// Clone이 MFC 내부에서 생성되어 사용된다.
	//
	// 이 이유로, Update되는 시점에서 각 Control의 Pointer를 직접 얻어와서
	// 사용하지 않으면, Update 된 값이 제대로 반영되어 있지 않은 버그가 발생한다.

	if( pCmdUI->m_pOther == NULL)
		return;

	if( g_pOption != NULL)
	{
		REAL32 intensity = (REAL32) pCtrl->GetPos() * 0.01f;
		
		if( g_pOption->m_LightIntensity != intensity)
		{
			g_pOption->m_LightIntensity = intensity;
			g_pFramework->ApplyOptionColors();

			{
				char conv[256];
				i3::snprintf( conv, _countof( conv), "(%.1f)", g_pOption->m_LightIntensity);

				pCtrl->SetToolTipText( conv);
			}
		}
		
		pCmdUI->Enable( TRUE);
	}
	else
	{
		pCmdUI->Enable( FALSE);
	}
}

void CMainFrame::OnUpdate_LightVertical(CCmdUI * pCmdUI)
{
	CMFCRibbonCmdUI * pUI = (CMFCRibbonCmdUI *)pCmdUI;
	CMFCRibbonSlider * pCtrl = (CMFCRibbonSlider *)pUI->m_pUpdated;

	// RibbonBar를 최소화시킨 상태에서 RibbonBar를 사용하면,
	// 실제 Create 단계에서 생성된 각 Ribbon Control이 사용되는 것이 아니라,
	// Clone이 MFC 내부에서 생성되어 사용된다.
	//
	// 이 이유로, Update되는 시점에서 각 Control의 Pointer를 직접 얻어와서
	// 사용하지 않으면, Update 된 값이 제대로 반영되어 있지 않은 버그가 발생한다.

	if (pCmdUI->m_pOther == NULL)
		return;

	if (g_pOption != NULL)
	{
		REAL32 fValue = (REAL32)pCtrl->GetPos();

		if (g_pOption->m_LightRho != fValue)
		{
			g_pOption->m_LightRho = fValue;
			g_pFramework->ApplyOptionColors();
			{
				char conv[256];
				i3::snprintf(conv, _countof(conv), "(%.2f)", g_pOption->m_LightRho);

				pCtrl->SetToolTipText(conv);
			}
		}

		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}


void CMainFrame::OnUpdate_LightHorizon(CCmdUI * pCmdUI)
{
	CMFCRibbonCmdUI * pUI = (CMFCRibbonCmdUI *)pCmdUI;
	CMFCRibbonSlider * pCtrl = (CMFCRibbonSlider *)pUI->m_pUpdated;

	// RibbonBar를 최소화시킨 상태에서 RibbonBar를 사용하면,
	// 실제 Create 단계에서 생성된 각 Ribbon Control이 사용되는 것이 아니라,
	// Clone이 MFC 내부에서 생성되어 사용된다.
	//
	// 이 이유로, Update되는 시점에서 각 Control의 Pointer를 직접 얻어와서
	// 사용하지 않으면, Update 된 값이 제대로 반영되어 있지 않은 버그가 발생한다.

	if (pCmdUI->m_pOther == NULL)
		return;

	if (g_pOption != NULL)
	{
		REAL32 fValue = (REAL32)pCtrl->GetPos();

		if (g_pOption->m_LightTheta != fValue)
		{
			g_pOption->m_LightTheta = fValue;
			g_pFramework->ApplyOptionColors();
			{
				char conv[256];
				i3::snprintf(conv, _countof(conv), "(%.2f)", g_pOption->m_LightTheta);

				pCtrl->SetToolTipText(conv);
			}
		}

		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}


///////////////////////////////////
// Shadow Size
void CMainFrame::OnCBChange_ShadowSize()
{
	INT32 idx = m_pCB_ShadowSize->GetCurSel();

	if( idx >= 0)
	{
		INT32 mapSize = atoi( m_pCB_ShadowSize->GetItem( idx));

		g_pOption->m_ShadowSize = mapSize;

		g_pFramework->ApplyOptionShadow();
	}
	
	RefreshRibbon();
}

void CMainFrame::OnUpdate_ShadowSize( CCmdUI * pCmdUI)
{
	if( pCmdUI->m_pOther == NULL)
		return;

	if( g_pOption != NULL)
	{
		char conv[128];

		i3::snprintf( conv, _countof(conv), "%d", g_pOption->m_ShadowSize);

		m_pCB_ShadowSize->SelectItem( conv);
		
		pCmdUI->Enable( TRUE);
	}
	else
	{
		m_pCB_ShadowSize->SelectItem( -1);
		pCmdUI->Enable( FALSE);
	}
}

///////////////////////////////////
// Geometry Tool
void CMainFrame::OnClicked_ReflectMap()
{
	CFileDialog	Dlg( TRUE, "i3i|DDS", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"Reflect Cube Map Files(*.dds)|*.dds|", this);

	if( Dlg.DoModal() == IDCANCEL)
		return;

	i3::safe_string_copy( g_pOption->m_szReflectMap, LPCTSTR( Dlg.GetPathName()), MAX_PATH );

	g_pFramework->ApplyOptionReflectMap();

	RefreshRibbon();
}

void CMainFrame::OnUpdate_ReflectMap( CCmdUI * pCmdUI)
{
	if( pCmdUI->m_pOther == NULL)
		return;

	if( g_pOption != NULL)
	{
		if( g_pOption->m_szReflectMap[0] == 0)
		{
			m_pBTN_ReflectMap->SetText( "(None)");
		}
		else
		{
			char szName[MAX_PATH];

			i3::extract_filename( g_pOption->m_szReflectMap, szName);

			m_pBTN_ReflectMap->SetText( szName);
		}
	}
}

///////////////////////////////////
// Geometry Tool
void CMainFrame::OnToolsGeometry()
{
	if( g_pFramework == NULL)
		return;

	i3TDKDlgGeometryTool dlg;

	dlg.Execute( g_pFramework->getAvatarSg());
}

void CMainFrame::OnUpdateToolsGeometry(CCmdUI *pCmdUI)
{
}

///////////////////////////////////
// Texture Tool
void CMainFrame::OnToolsTextures()
{
	if( g_pFramework == NULL)
		return;

	i3TDKDlgTextureTool dlg;

	dlg.Execute( g_pFramework->getAvatarSg());
}

void CMainFrame::OnUpdateToolsTextures(CCmdUI *pCmdUI)
{
}

///////////////////////////////////
// Project Config
void CMainFrame::OnToolsProjectconfig()
{
	i3TDKDlgEnvSet dlg;

	if( dlg.Execute() == true)
	{
		m_Panel_Part.LoadEquipData();
		m_Panel_Anim.LoadAnimation();
		m_Panel_Avatar.CreatePartsList();
	}
}

void CMainFrame::OnUpdateToolsProjectconfig(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

//////////////////////////////////////////////////////////////
// Panes

void CMainFrame::OnClicked_ViewAvatar()
{
	BOOL bShow = !m_Panel_Avatar.IsVisible();

	ShowPane( &m_Panel_Avatar, bShow, FALSE, bShow);
}

void CMainFrame::OnUpdate_ViewAvatar( CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck( m_Panel_Avatar.IsWindowVisible());
}


void CMainFrame::OnClicked_ViewAnim()
{
	BOOL bShow = !m_Panel_Anim.IsVisible();

	ShowPane( &m_Panel_Anim, bShow, FALSE, bShow);
}

void CMainFrame::OnUpdate_ViewAnim( CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck( m_Panel_Anim.IsWindowVisible());
}

void CMainFrame::OnClicked_ViewPart()
{
	BOOL bShow = !m_Panel_Part.IsVisible();

	ShowPane( &m_Panel_Part, bShow, FALSE, bShow);
}

void CMainFrame::OnUpdate_ViewPart( CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck( m_Panel_Part.IsWindowVisible());
}


void CMainFrame::OnSelectBlueTeam()
{
	m_eTeamType = TEAM_BLUE;
	g_pFramework->SetTeam( m_eTeamType);
	m_Panel_Avatar.UpdateAll();
}

void CMainFrame::OnSelectRedTeam()
{
	m_eTeamType = TEAM_RED;
	g_pFramework->SetTeam( m_eTeamType);
	m_Panel_Avatar.UpdateAll();
}

void CMainFrame::OnUpdateSelectTeamBlue( CCmdUI * pCmdUI)
{
	if (m_eTeamType == TEAM_BLUE)
	{
		if (g_pAvatar && g_pOption->m_eTeamColor != TEAM_BLUE)
		{
			g_pAvatar->SetChangeCampTexture(TEAM_BLUE);
			g_pOption->m_eTeamColor = TEAM_BLUE;
			I3TRACE("=================== Blue Team On\n");
		}
		pCmdUI->SetCheck(1);
	}
	else
		pCmdUI->SetCheck(0);
}

void CMainFrame::OnUpdateSelectTeamRed( CCmdUI * pCmdUI)
{
	if (m_eTeamType == TEAM_RED)
	{
		if (g_pAvatar && g_pOption->m_eTeamColor != TEAM_RED)
		{
			g_pAvatar->SetChangeCampTexture(TEAM_RED);
			g_pOption->m_eTeamColor = TEAM_RED;
			I3TRACE(" Red Team On======================\n");
		}
		pCmdUI->SetCheck(1);
	}
	else
		pCmdUI->SetCheck(0);
}

void CMainFrame::OnSelectTeamColorOff()
{
	m_eTeamType = TEAM_NONE;
}

void CMainFrame::OnUpdateSelectTeamColorOff(CCmdUI * pCmdUI)
{
	if (m_eTeamType == TEAM_NONE)
	{
		if (g_pAvatar && g_pOption->m_eTeamColor != TEAM_NONE)
		{
			g_pAvatar->SetChangeCampTexture(TEAM_NONE);
			g_pOption->m_eTeamColor = TEAM_NONE;
			I3TRACE(" TeamColor Off======================\n");
		}
		pCmdUI->SetCheck(1);
	}
	else
		pCmdUI->SetCheck(0);
}

void CMainFrame::OnClicked_AttachWeaponRightHand()
{
	if( g_pAvatar != NULL && g_pAvatar->getRightHandWeapon() != NULL)
	{
		if( g_pAvatar->getRightHandWeapon()->IsFlag(I3_NODEFLAG_DISABLE))
			g_pAvatar->getRightHandWeapon()->RemoveFlag(I3_NODEFLAG_DISABLE);
		else
			g_pAvatar->getRightHandWeapon()->AddFlag(I3_NODEFLAG_DISABLE);
	}
}

void CMainFrame::OnUpdate_AttachWeaponRightHand( CCmdUI * pCmdUI)
{
	if( g_pAvatar != NULL && g_pAvatar->getRightHandWeapon() != NULL)
	{
		pCmdUI->SetCheck( g_pAvatar->getRightHandWeapon()->IsFlag(I3_NODEFLAG_DISABLE)?0:1);
	}
	else
	{
		pCmdUI->SetCheck( 0);
	}
}

void CMainFrame::OnClicked_AttachWeaponBackpack()
{
	if( g_pAvatar != NULL && g_pAvatar->getBackpackWeapon() != NULL)
	{
		if( g_pAvatar->getBackpackWeapon()->IsFlag(I3_NODEFLAG_DISABLE))
			g_pAvatar->getBackpackWeapon()->RemoveFlag(I3_NODEFLAG_DISABLE);
		else
			g_pAvatar->getBackpackWeapon()->AddFlag(I3_NODEFLAG_DISABLE);
	}
}

void CMainFrame::OnUpdate_AttachWeaponBackpack( CCmdUI * pCmdUI)
{
	if( g_pAvatar != NULL && g_pAvatar->getBackpackWeapon() != NULL)
	{
		pCmdUI->SetCheck( g_pAvatar->getBackpackWeapon()->IsFlag(I3_NODEFLAG_DISABLE)?0:1);
	}
	else
	{
		pCmdUI->SetCheck( 0);
	}
}

void CMainFrame::OnClicked_ChangeAttachWeapon()
{
}

void CMainFrame::OnUpdate_ChangeAttachWeapon( CCmdUI * pCmdUI)
{
}


void CMainFrame::OnSaveMergedTexture()
{
	if( g_pAvatar != NULL)
	{
		CFileDialog	Dlg( FALSE, "i3i", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
			"i3Engine Image File(*.i3i)|*.i3i||", NULL);

		if( Dlg.DoModal() != IDOK )
		{
			return; 
		}

		g_pAvatar->SaveMacroTex( LPCTSTR( Dlg.GetPathName()));
	}
}

void CMainFrame::OnUpdateSaveMergedTexture( CCmdUI * pCmdUI)
{
}


void CMainFrame::OnConvertSGbyMacroTexture()
{
	if( g_pAvatar != NULL)
	{
		//g_pAvatar->SaveMacroTex( LPCTSTR( Dlg.GetPathName()));
	}
}

void CMainFrame::OnUpdateConvertSGbyMacroTexture( CCmdUI * pCmdUI)
{
}

void CMainFrame::OnConvertBlendIndex()
{
	CDlg_SelectBody dlg;
	if( dlg.DoModal() == IDOK)
	{
	}
}

void CMainFrame::OnUpdateConvertBlendIndex( CCmdUI * pCmdUI)
{
}

void CMainFrame::OnChanged_WeaponSelect()
{
	if( m_pCB_WeaponList->GetCurSel() != -1)
	{
		if( g_pAvatar != NULL)
		{
			g_pAvatar->LoadWeapon( (const char*) m_pCB_WeaponList->GetItemData( m_pCB_WeaponList->GetCurSel()) );
		}
	}
}

void CMainFrame::OnUpdate_WeaponSelect( CCmdUI * pCmdUI)
{
}
