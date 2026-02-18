#include "stdafx.h"
#include "MainFrm.h"
#include "GlobalVar.h"

void CMainFrame::InitRibbon_Home(void)
{
	CMFCRibbonPanel* pPanel;

	// Add "Home" category with "Clipboard" panel:
	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory( "Home", IDB_ICONS_32, IDB_ICONS_32, CSize( 16, 16), CSize( 16, 16));

	// View Panel
	{
		pPanel = pCategory->AddPanel( "View");

		// View Modes
		{
			CMFCRibbonButtonsGroup* pGroup = new CMFCRibbonButtonsGroup;

			pPanel->Add( pGroup);

			pGroup->AddButton( new CMFCRibbonButton( ID_VIEW_NORMAL,	"Normal View",		0));
			pGroup->AddButton( new CMFCRibbonButton( ID_VIEW_MESH,		"Mesh View",		1));
			pGroup->AddButton( new CMFCRibbonButton( ID_VIEW_COLLIDER,	"Collider View",	2));
			pGroup->AddButton( new CMFCRibbonButton( ID_VIEW_WIRE,		"Wire View",		3));
			pGroup->AddButton( new CMFCRibbonButton( ID_VIEW_SELONLY,	"Select-Only View", 4));
		}

		// Grid
		{
			pPanel->Add( new CMFCRibbonButton( ID_VIEW_GRID,			"Grid",		6, 6));
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

		// Texture Tool
		pPanel->Add( new CMFCRibbonButton( ID_TOOLS_PHYSIXTOOL,			"Physics Tool",		 8));

		// Project Config
		pPanel->Add( new CMFCRibbonButton( ID_TOOLS_PROJECTCONFIG,		"Project Config.",	-1));
	}

	// Panels
	{
		pPanel = pCategory->AddPanel( "Panes");

		m_pCHK_PaneSg = new CMFCRibbonCheckBox( ID_PANE_SG, _T("Scene-Graph Pane"));
		pPanel->Add( m_pCHK_PaneSg);

		m_pCHK_PaneSpec = new CMFCRibbonCheckBox( ID_PANE_SPEC, _T("Specification Pane"));
		pPanel->Add( m_pCHK_PaneSpec);
	}
}

/////////////////////////////////////
// Normal View
void CMainFrame::OnViewNormal()
{
	if( g_pFramework != NULL)
	{
		g_pFramework->setViewMode( VIEWER_MODE_NORMAL);

		if( g_pRenderPanel != NULL)
			g_pRenderPanel->Invalidate();
	}
}

void CMainFrame::OnUpdateViewNormal(CCmdUI *pCmdUI)
{
	if( g_pOption->m_ViewerMode == VIEWER_MODE_NORMAL)
		pCmdUI->SetCheck( TRUE);
	else
		pCmdUI->SetCheck( FALSE);
}

/////////////////////////////////////
// Mesh View
void CMainFrame::OnViewMesh()
{
	if( g_pFramework != NULL)
	{
		g_pFramework->setViewMode( VIEWER_MODE_MESH);

		if( g_pRenderPanel != NULL)
			g_pRenderPanel->Invalidate();
	}
}

void CMainFrame::OnUpdateViewMesh(CCmdUI *pCmdUI)
{
	if( g_pOption->m_ViewerMode == VIEWER_MODE_MESH)
		pCmdUI->SetCheck( TRUE);
	else
		pCmdUI->SetCheck( FALSE);
}

/////////////////////////////////////
// Collider View
void CMainFrame::OnViewCollider()
{
	if( g_pFramework != NULL)
	{
		g_pFramework->setViewMode( VIEWER_MODE_COLL);

		if( g_pRenderPanel != NULL)
			g_pRenderPanel->Invalidate();
	}
}

void CMainFrame::OnUpdateViewCollider(CCmdUI *pCmdUI)
{
	if( g_pOption->m_ViewerMode == VIEWER_MODE_MESH)
		pCmdUI->SetCheck( TRUE);
	else
		pCmdUI->SetCheck( FALSE);
}

/////////////////////////////////////
// Wire View
void CMainFrame::OnViewWire()
{
	if( g_pFramework != NULL)
	{
		g_pFramework->setViewMode( VIEWER_MODE_WIRE);

		if( g_pRenderPanel != NULL)
			g_pRenderPanel->Invalidate();
	}
}

void CMainFrame::OnUpdateViewWire(CCmdUI *pCmdUI)
{
	if( g_pOption->m_ViewerMode == VIEWER_MODE_WIRE)
		pCmdUI->SetCheck( TRUE);
	else
		pCmdUI->SetCheck( FALSE);
}

/////////////////////////////////////
// Select-Only View
void CMainFrame::OnViewSelonly()
{
	if( g_pFramework != NULL)
	{
		g_pFramework->setViewMode( VIEWER_MODE_SELONLY);

		if( g_pRenderPanel != NULL)
			g_pRenderPanel->Invalidate();
	}
}

void CMainFrame::OnUpdateViewSelonly(CCmdUI *pCmdUI)
{
	if( g_pOption->m_ViewerMode == VIEWER_MODE_SELONLY)
		pCmdUI->SetCheck( TRUE);
	else
		pCmdUI->SetCheck( FALSE);
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
				i3String::Format( conv, _countof( conv), "(%.1f)", g_pOption->m_LightIntensity);

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

		i3String::Format( conv, _countof(conv), "%d", g_pOption->m_ShadowSize);

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

	i3String::Copy( g_pOption->m_szReflectMap, LPCTSTR( Dlg.GetPathName()));

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

			i3String::SplitFileName( g_pOption->m_szReflectMap, szName, TRUE);

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

	if( g_pFramework->getSgInfo() == NULL)
		return;

	i3TDKDlgGeometryTool dlg;

	dlg.Execute( g_pFramework->getSgInfo()->getInstanceSg());
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

	if( g_pFramework->getSgInfo() == NULL)
		return;

	i3TDKDlgTextureTool dlg;

	i3SceneGraphInfo * pInfo = g_pFramework->getSgInfo();

	if( pInfo->getCommonSg() != NULL)
		dlg.Execute( pInfo->getCommonSg());
	else
		dlg.Execute( pInfo->getInstanceSg());
}

void CMainFrame::OnUpdateToolsTextures(CCmdUI *pCmdUI)
{
}

///////////////////////////////////
// Project Config
void CMainFrame::OnToolsProjectconfig()
{
	i3TDKDlgEnvSet dlg;

	dlg.Execute();
}

void CMainFrame::OnUpdateToolsProjectconfig(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


///////////////////////////////////
// Physics Tool
void CMainFrame::OnToolsPhysics()
{
	i3TDKDlg_Physix_Tool dlg;

	i3SceneGraphInfo * pInfo = g_pFramework->getSgInfo();

	if( pInfo->getCommonSg() != NULL)
		dlg.Execute( pInfo->getCommonSg());
	else
		dlg.Execute( pInfo->getInstanceSg());
}

//////////////////////////////////////////////////////////////
// Panes
void CMainFrame::OnClicked_PaneSg()
{
	BOOL bShow = !m_Panel_Sg.IsVisible();

	ShowPane( &m_Panel_Sg, bShow, FALSE, bShow);
}

void CMainFrame::OnUpdate_PaneSg( CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck( m_Panel_Sg.IsWindowVisible());
}


void CMainFrame::OnClicked_PaneSpec()
{
	BOOL bShow = !m_Dock_Spec.IsVisible();

	ShowPane( &m_Dock_Spec, bShow, FALSE, bShow);
}

void CMainFrame::OnUpdate_PaneSpec( CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck( m_Dock_Spec.IsWindowVisible());
}
