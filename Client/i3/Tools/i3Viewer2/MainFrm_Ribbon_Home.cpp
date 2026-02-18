#include "stdafx.h"
#include "MainFrm.h"
#include "GlobalVar.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/atof.h"
#include "i3Base/string/ext/ftoa.h"
#include "dlgBatchRenAttrOpti.h"
#include "dlgColorOptionApply.h"
#include "dlgBlendOptionApply.h"
#include <stdio.h>
#include <math.h>

void CMainFrame::InitRibbon_Home(void)
{
	CMFCRibbonPanel* pPanel;

	// Add "Home" category with "Clipboard" panel:
	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory( "Home", IDB_ICONS_32, IDB_ICONS_32, CSize( 16, 16), CSize( 16, 16));
	CMFCRibbonCategory* pCategory_Light = m_wndRibbonBar.AddCategory( "Light", IDB_ICONS_32, IDB_ICONS_32, CSize( 16, 16), CSize( 16, 16));
	CMFCRibbonCategory* pCategory2 = m_wndRibbonBar.AddCategory( "Screen", IDB_ICONS_32, IDB_ICONS_32, CSize( 16, 16), CSize( 16, 16));
	

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
		pPanel = pCategory_Light->AddPanel( "Light");

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
			m_pEdit_LightIntensity = new CMFCRibbonEdit(ID_EDIT_LIGHTINTENSITY, 30, _T("Intensity : "));

			TCHAR conv[128];
			i3::ftoa(g_pOption->m_LightIntensity, conv);
			m_pEdit_LightIntensity->SetEditText(conv);
			pPanel->Add(m_pEdit_LightIntensity);

			m_pSlider_LightIntensity = new CMFCRibbonSlider( ID_OPTIONS_LIGHTINTENSITY, 80);
			m_pSlider_LightIntensity->SetText( "Light Intensity");
			m_pSlider_LightIntensity->SetZoomButtons();
			m_pSlider_LightIntensity->SetRange( 0, 300);
			m_pSlider_LightIntensity->SetPos( (INT32) (g_pOption->m_LightIntensity * 100.0f));

			pPanel->Add( m_pSlider_LightIntensity);
		}

		//Light Position
		{
			{
				pPanel->Add(new CMFCRibbonSeparator(FALSE));
				m_pEdit_LightPosRho = new CMFCRibbonEdit(ID_EDIT_LIGHTPOSRHO, 40, _T("Vertical : "));
				pPanel->Add(m_pEdit_LightPosRho);

				m_pSlider_LightPosRho = new CMFCRibbonSlider( ID_SLIDER_LIGHTPOSRHO, 80);
				m_pSlider_LightPosRho->SetZoomButtons();
				m_pSlider_LightPosRho->SetRange( -180, 180);
				m_pSlider_LightPosRho->SetPos( (INT32) (g_pOption->m_LightRho));
				m_pSlider_LightPosRho->SetZoomIncrement(1);
				pPanel->Add(m_pSlider_LightPosRho);

				TCHAR conv[128];
				i3::ftoa(g_pOption->m_LightRho, conv);
				m_pEdit_LightPosRho->SetEditText(conv);

				m_pCHK_LightPosVisible = new CMFCRibbonCheckBox(ID_CHECK_LIGHTPOS, _T("Light Position Visible"));
				pPanel->Add(m_pCHK_LightPosVisible);
			}

			pPanel->Add(new CMFCRibbonSeparator(FALSE));

			{
				m_pEdit_LightPosTheta = new CMFCRibbonEdit(ID_EDIT_LIGHTPOSTHETA, 40, _T("Horizontal : "));
				pPanel->Add(m_pEdit_LightPosTheta);

				m_pSlider_LightPosTheta = new CMFCRibbonSlider( ID_SLIDER_LIGHTPOSTHETA, 80);
				m_pSlider_LightPosTheta->SetZoomButtons();
				m_pSlider_LightPosTheta->SetRange( 0, 360);
				m_pSlider_LightPosTheta->SetPos( (INT32) (g_pOption->m_LightTheta));
				m_pSlider_LightPosTheta->SetZoomIncrement(1);
				pPanel->Add(m_pSlider_LightPosTheta);

				TCHAR conv[128];
				i3::ftoa(g_pOption->m_LightTheta, conv);
				m_pEdit_LightPosTheta->SetEditText(conv);
			}

		}

		//Environment
		pPanel = pCategory_Light->AddPanel( "Environment");

		// Shadow Map Size
		{
			// Shadow
			{
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

	// Post Process Panel
	// pBloom 널체크 진행해야 한다.
	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewer->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

	{
		pPanel = pCategory2->AddPanel( "Post Process");

		m_pEdit_BloomIntensity = new CMFCRibbonEdit(ID_EDIT_BLOOMINTENSITY, 40, _T("Bloom Intensity : "));
		pPanel->Add(m_pEdit_BloomIntensity);

		m_pSlider_BloomIntensity = new CMFCRibbonSlider( ID_SLIDER_BLOOMINTENSITY, 80);
		m_pSlider_BloomIntensity->SetZoomButtons();
		m_pSlider_BloomIntensity->SetRange(0, 300);
		m_pSlider_BloomIntensity->SetZoomIncrement(1);
		pPanel->Add(m_pSlider_BloomIntensity);
	}

	{
		m_pCHK_BloomEnable = new CMFCRibbonCheckBox(ID_CHECK_BLOOM, _T("Bloom Enable"));
		pPanel->Add(m_pCHK_BloomEnable);
	}


	pPanel->Add(new CMFCRibbonSeparator(FALSE));
                     
	{
		m_pEdit_BloomThreshold = new CMFCRibbonEdit(ID_EDIT_BLOOMTHRESHOLD, 40, _T("Bloom Threshold : "));
		pPanel->Add(m_pEdit_BloomThreshold);

		m_pSlider_BloomThreshold = new CMFCRibbonSlider( ID_SLIDER_BLOOMTHRESHOLD, 80);
		m_pSlider_BloomThreshold->SetZoomButtons();
		m_pSlider_BloomThreshold->SetRange( 0, 100);
		m_pSlider_BloomThreshold->SetZoomIncrement(1);
		pPanel->Add(m_pSlider_BloomThreshold);
	}

	{
		m_pEdit_BlurIteration = new CMFCRibbonEdit(ID_EDIT_BLURITERATION, 40, _T("Blur Iteration Count : "));
		pPanel->Add(m_pEdit_BlurIteration);
	}

	pPanel->Add(new CMFCRibbonSeparator(FALSE));	

	{
		m_pEdit_BlurTheta = new CMFCRibbonEdit(ID_EDIT_GAUSSIANTHETA, 40, _T("Blur Theta : "));
		pPanel->Add(m_pEdit_BlurTheta);

		m_pSlider_BlurTheta = new CMFCRibbonSlider( ID_SLIDER_GAUSSIANTHETA, 80);
		m_pSlider_BlurTheta->SetZoomButtons();
		m_pSlider_BlurTheta->SetRange( 0, 100);
		m_pSlider_BlurTheta->SetZoomIncrement(1);
		pPanel->Add(m_pSlider_BlurTheta);
	}

	if(pBloom != NULL)
	{
		TCHAR conv[128];
		i3::ftoa(pBloom->GetBloomIntensity(), conv);

		m_pEdit_BloomIntensity->SetEditText(conv);
		m_pSlider_BloomIntensity->SetPos((INT32)(pBloom->GetBloomIntensity() * 100));

		i3::ftoa(pBloom->GetBloomThreshold(), conv);

		m_pEdit_BloomThreshold->SetEditText(conv);
		m_pSlider_BloomThreshold->SetPos((INT32)(pBloom->GetBloomThreshold() * 100));

		i3::itoa(pBloom->GetBlurIteration(), conv, _countof(conv));
		m_pEdit_BlurIteration->SetEditText(conv);

		i3::ftoa(pBloom->GetBlurTheta(), conv);

		m_pEdit_BlurTheta->SetEditText(conv);
		m_pSlider_BlurTheta->SetPos((INT32)(pBloom->GetBlurTheta() * 10));
	}

	// Camera
	{
		pPanel = pCategory->AddPanel("Camera");
		m_pEdit_FOV = new CMFCRibbonEdit(ID_CAMERA_FOV, 45, _T("FOV(Degree)"));

		TCHAR conv[128];
		i3::ftoa(g_pOption->m_fovDegree, conv);
		m_pEdit_FOV->SetEditText(conv);

		pPanel->Add(m_pEdit_FOV);
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

		// ChangeTex Test
		pPanel->Add( new CMFCRibbonButton( ID_TOOLS_CHANGETEX,			"ChangeTex PEF",	 8));

		// Project Config
		pPanel->Add( new CMFCRibbonButton( ID_TOOLS_PROJECTCONFIG,		"Project Config.",	8));

		//
		pPanel->Add( new CMFCRibbonButton( ID_TOOLS_BATCH_RENATTR_OPTI,	"Batch RenAttrOpti", 8));

		pPanel->Add( new CMFCRibbonButton( ID_TOOLS_TOTALBLEND,	"Blend Option Apply", 8));

		pPanel->Add( new CMFCRibbonButton( ID_TOOLS_TOTALCOLOR,	"Color Option Apply", 8));
	}

	

	// Optimizer
	{
		pPanel = pCategory->AddPanel("Optimizer");
		m_pBtnOpt1 = new CMFCRibbonButton(ID_BTN_OPT1, "RemoveAnim", 8);
		pPanel->Add(m_pBtnOpt1);

		m_pBtnOpt2 = new CMFCRibbonButton(ID_BTN_OPT2, "ConvertDXTTexture", 8);
		pPanel->Add(m_pBtnOpt2);

		m_pBtnOpt3 = new CMFCRibbonButton(ID_BTN_OPT3, "MipmapGen", 8);
		pPanel->Add(m_pBtnOpt3);

		m_pBtnOpt4 = new CMFCRibbonButton(ID_BTN_OPT4, "Remove Vertex Color", 8);
		pPanel->Add(m_pBtnOpt4);
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
	CString intensityStr = m_pEdit_LightIntensity->GetEditText();
	REAL32 intensity = (REAL32)atof(intensityStr);

	char conv[256];
	i3::snprintf(conv, sizeof(conv), "%.1f", intensity);
	intensity = (REAL32)atof(conv);

	g_pOption->m_LightIntensity = intensity;

	//Slider bar 적용
	m_pSlider_LightIntensity->SetPos( (INT32) (g_pOption->m_LightIntensity * 100.0f));
	m_pEdit_LightIntensity->SetEditText(conv);

	//실제 컬러 적용
	g_pFramework->ApplyOptionColors();
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
				i3::snprintf( conv, _countof( conv), "%.1f", g_pOption->m_LightIntensity);
				m_pEdit_LightIntensity->SetEditText(conv);
			}
		}
		
		pCmdUI->Enable( TRUE);
	}
	else
	{
		pCmdUI->Enable( FALSE);
	}
}

void CMainFrame::OnUpdate_LightPosRho(CCmdUI * pCmdUI)
{
	CMFCRibbonCmdUI * pUI = (CMFCRibbonCmdUI *) pCmdUI;
	CMFCRibbonSlider * pCtrl = (CMFCRibbonSlider *) pUI->m_pUpdated;

	if( pCmdUI->m_pOther == NULL)
		return;
	
	if( g_pOption != NULL)
	{
		REAL32 rho = (REAL32) pCtrl->GetPos();

		if( g_pFramework->GetLightRho() != rho)
		{
			g_pOption->m_LightRho = rho;
			g_pFramework->SetLightRho(rho);

			{
				char conv[256];
				i3::snprintf( conv, _countof( conv), "%d", rho);
				m_pEdit_LightPosRho->SetEditText(conv);
			}
		}

		pCmdUI->Enable( TRUE);
	}
	else
	{
		pCmdUI->Enable( FALSE);
	}
}

void CMainFrame::OnUpdate_LightPosTheta(CCmdUI * pCmdUI)
{
	CMFCRibbonCmdUI * pUI = (CMFCRibbonCmdUI *) pCmdUI;
	CMFCRibbonSlider * pCtrl = (CMFCRibbonSlider *) pUI->m_pUpdated;

	if( pCmdUI->m_pOther == NULL)
		return;

	if( g_pOption != NULL)
	{
		REAL32 theta = (REAL32) pCtrl->GetPos();

		if( g_pFramework->GetLightTheta() != theta)
		{
			g_pOption->m_LightTheta = theta;
			g_pFramework->SetLightTheta(theta);

			{
				char conv[256];
				i3::snprintf( conv, _countof( conv), "%d", theta);
				m_pEdit_LightPosTheta->SetEditText(conv);
			}
		}

		pCmdUI->Enable( TRUE);
	}
	else
	{
		pCmdUI->Enable( FALSE);
	}

}

void CMainFrame::OnClicked_LightPosVisible()
{
	bool bVisible = !g_pFramework->IsLightAxisVisible();

	g_pFramework->SetLightAxisVisible(bVisible);
}

void CMainFrame::OnUpdate_LightPosVisible(CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck(g_pFramework->IsLightAxisVisible());
}

void CMainFrame::OnChanged_BloomIntensity()
{
	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewer->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

	if(pBloom != NULL)
	{
		REAL32 theta = (REAL32) m_pSlider_BloomIntensity->GetPos();
		theta = theta / 100.f;

		if( pBloom->GetBloomIntensity() != theta)
		{
			pBloom->SetBloomIntensity(theta);

			{
				char conv[256];
				i3::snprintf( conv, _countof( conv), "%.2f", theta);
				m_pEdit_BloomIntensity->SetEditText(conv);
			}
		}
	}
}

void CMainFrame::OnUpdate_BloomIntensity(CCmdUI * pCmdUI)
{
	if( pCmdUI->m_pOther == NULL)
		return;
	
	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewer->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

	if(pBloom != NULL)
	{
		m_pSlider_BloomIntensity->SetPos((INT32)(pBloom->GetBloomIntensity() * 100));

		TCHAR conv[128];

		i3::ftoa(pBloom->GetBloomIntensity(), conv);
		m_pEdit_BloomIntensity->SetEditText(conv);

		pCmdUI->Enable( TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnChanged_BloomThreshold()
{
	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewer->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

	if(pBloom != NULL)
	{
		REAL32 theta = (REAL32) m_pSlider_BloomThreshold->GetPos();
		theta = theta / 100.f;

		if( pBloom->GetBloomThreshold() != theta)
		{
			pBloom->SetBloomThreshold(theta);
			{
				char conv[256];
				i3::snprintf( conv, _countof( conv), "%.2f", theta);
				m_pEdit_BloomThreshold->SetEditText(conv);
			}
		}
	}
}

void CMainFrame::OnUpdate_BloomThreshold(CCmdUI * pCmdUI)
{
	if( pCmdUI->m_pOther == NULL)
		return;

	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewer->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

	if(pBloom != NULL)
	{
		m_pSlider_BloomThreshold->SetPos((INT32)(pBloom->GetBloomThreshold() * 100));

		TCHAR conv[128];

		i3::ftoa(pBloom->GetBloomThreshold(), conv);
		m_pEdit_BloomThreshold->SetEditText(conv);

		pCmdUI->Enable( TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnClicked_BloomEnable()
{
	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewer->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());
	if(pBloom != NULL)
	{
		bool bVisible = !pBloom->IsEnable();
		pBloom->SetEnable(bVisible);
	}
}

void CMainFrame::OnUpdate_BloomEnable(CCmdUI * pCmdUI)
{
	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewer->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());
	if(pBloom != NULL)
	{
		pCmdUI->SetCheck(pBloom->IsEnable());
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

void CMainFrame::OnChanged_BlurIteration()
{
	CString iterationStr = m_pEdit_BlurIteration->GetEditText();
	INT32 iteration = (INT32)atoi(iterationStr);

	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewer->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());
	if(pBloom != NULL)
	{
		pBloom->SetBlurIteration(iteration);
	}
}

void CMainFrame::OnUpdate_BlurIteration(CCmdUI * pCmdUI)
{
	if((pCmdUI->m_pOther == NULL))
		return;

	if( m_pEdit_BlurIteration->HasFocus())
		return;

	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewer->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

	if(pBloom != NULL)
	{
		TCHAR conv[128];

		i3::itoa(pBloom->GetBlurIteration(), conv, _countof(conv));
		m_pEdit_BlurIteration->SetEditText(conv);

		pCmdUI->Enable( TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnChanged_BlurTheta()
{
	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewer->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

	if(pBloom != NULL)
	{
		REAL32 theta = (REAL32) m_pSlider_BlurTheta->GetPos();
		theta = theta / 10.f;

		if( pBloom->GetBlurTheta() != theta)
		{
			pBloom->SetBlurTheta(theta);
			{
				char conv[256];
				i3::snprintf( conv, _countof( conv), "%.1f", theta);
				m_pEdit_BlurTheta->SetEditText(conv);
			}
		}
	}
}

void CMainFrame::OnUpdate_BlurTheta(CCmdUI * pCmdUI)
{
	if( pCmdUI->m_pOther == NULL)
		return;

	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewer->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

	if(pBloom != NULL)
	{
		m_pSlider_BlurTheta->SetPos((INT32)(pBloom->GetBlurTheta() * 10));

		TCHAR conv[128];

		i3::ftoa(pBloom->GetBlurTheta(), conv);
		m_pEdit_BlurTheta->SetEditText(conv);

		pCmdUI->Enable( TRUE);
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

		//	i3String::SplitFileName( g_pOption->m_szReflectMap, szName, TRUE);
			i3::extract_filename(g_pOption->m_szReflectMap, szName);

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

void CMainFrame::OnToolsChangeTex()
{
	if ( m_dlgChangeTex == NULL )		
		return;


	m_dlgChangeTex->ShowWindow(SW_SHOW);
}

void CMainFrame::OnUpdateToolsChangeTex(CCmdUI* pCmdUI)
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
// Batch RenAttr Opti
void CMainFrame::OnToolsBatchRenAttrOpti()
{
	HINSTANCE old = AfxGetResourceHandle();	
	AfxSetResourceHandle(GetModuleHandle(NULL));

	dlgBatchRenAttrOpti dlg;
	dlg.DoModal();

	AfxSetResourceHandle(old);
}

void CMainFrame::OnUpdateToolsBatchRenAttrOpti(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


///////////////////////////////////
// Physics Tool
void CMainFrame::OnToolsPhysics()
{
	i3TDKDlg_Physix_Tool dlg;

	i3SceneGraphInfo * pInfo = g_pFramework->getSgInfo();
	
	if (pInfo == NULL) return;			// 메시가 없으면 아무것없이 리턴하게 처리.

	if( pInfo->getCommonSg() != NULL)
		dlg.Execute( pInfo->getCommonSg());
	else
		dlg.Execute( pInfo->getInstanceSg());
}

void CMainFrame::OnToolsTotalBlend()
{
	if( g_pFramework == NULL)
		return;

	if( g_pFramework->getSgInfo() == NULL)
		return;

	HINSTANCE old = AfxGetResourceHandle();	
	AfxSetResourceHandle(GetModuleHandle(NULL));

	i3SceneGraphInfo * pInfo = g_pFramework->getSgInfo();

	dlgBlendOptionApply dlg;
	dlg.Execute(pInfo->getInstanceSg());

	AfxSetResourceHandle(old);
}

void CMainFrame::OnUpdateTotalBlend(CCmdUI * pCmdUI)
{

}

void CMainFrame::OnToolsTotalColor()
{
	if( g_pFramework == NULL)
		return;

	if( g_pFramework->getSgInfo() == NULL)
		return;

	HINSTANCE old = AfxGetResourceHandle();	
	AfxSetResourceHandle(GetModuleHandle(NULL));

	i3SceneGraphInfo * pInfo = g_pFramework->getSgInfo();

	dlgColorOptionApply dlg;
	dlg.Execute(pInfo->getInstanceSg());

	AfxSetResourceHandle(old);
}

void CMainFrame::OnUpdateTotalColor(CCmdUI * pCmdUI)
{

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

void CMainFrame::OnChange_CameraFOV(void)
{
	CString fovStr = m_pEdit_FOV->GetEditText();
	INT32 fov = atoi(fovStr);

	g_pOption->m_fovDegree = fov;
	g_pFramework->ApplyOptionCamera();
}

void CMainFrame::OnClicked_Opt1()
{
	OnClicked_Optimizer(i3OptRemoveAnim::static_meta());
}

void CMainFrame::OnClicked_Opt2()
{
	OnClicked_Optimizer(i3OptConvertDXTTexture::static_meta());
}

void CMainFrame::OnClicked_Opt3()
{
	OnClicked_Optimizer(i3OptMipmapGen::static_meta());
}

void CMainFrame::OnClicked_Opt4()
{
	i3OptRemoveVertexComponent * pOpt = i3OptRemoveVertexComponent::new_object();

	pOpt->SetRemoveColor( TRUE);

	g_pFramework->Optimize( pOpt, m_Panel_Sg.getSceneGraphTreeCtrl());


	I3_MUST_RELEASE( pOpt);
}

void CMainFrame::OnClicked_Optimizer(i3ClassMeta * pMeta)
{
	i3SceneOptimizer * pOpt = (i3SceneOptimizer *) pMeta->create_instance();
	I3_MUST_ADDREF(pOpt);
	
	g_pFramework->Optimize(pOpt, m_Panel_Sg.getSceneGraphTreeCtrl());

	I3_MUST_RELEASE(pOpt);
}
