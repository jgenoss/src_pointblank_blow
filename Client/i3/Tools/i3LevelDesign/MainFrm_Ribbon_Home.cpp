#include "pch.h"
#include "MainFrm.h"
#include "DlgElementList.h"
#include "DlgExport.h"
#include "MainFrm.h"
#include "OptionDlg.h"

#include "i3Level/i3NavMeshUtil.h"

#include "i3LevelDesignDoc.h"
#include "i3LevelDesign.h"

#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/ftoa.h"
#include "DlgHitMapTimeSet.h"

struct CTRLMODE_BTN_DATA
{
	UINT	id;
	INT		img;	
	const char* szName;

	CTRLMODE_BTN_DATA( UINT _id, INT _img, const char* _name )
	{
		id = _id;
		img = _img;
		szName = _name;
	}
};

CTRLMODE_BTN_DATA g_CTRLMODE_BTN_DATA[ MAX_LEVEL_CONTROL_MODE ] =
{
	CTRLMODE_BTN_DATA( ID_RIBBON_SELECTION,		0, "SELECT" ),
	CTRLMODE_BTN_DATA( ID_RIBBON_TRANSLATION,	1, "MOVE" ),
	CTRLMODE_BTN_DATA( ID_RIBBON_ROTATION,		2, "ROTATE" ),
	CTRLMODE_BTN_DATA( ID_RIBBON_SCALE,			3, "SCALE" ),
	CTRLMODE_BTN_DATA( ID_RIBBON_OBJBRUSH,		4, "OBJECT BRUSH" ),
	CTRLMODE_BTN_DATA( ID_RIBBON_PATHEDIT,		5, "EDIT PATH" ),
};

void	CMainFrame::InitRibbon_Home(void)
{
	CMFCRibbonPanel* pPanel;

	// Home
	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory( "Home", IDB_BUTTONS, IDB_BUTTONS );

	// Edit Panel
	{
		pPanel = pCategory->AddPanel( "Edit", m_PanelImages.ExtractIcon(27));

		CMFCRibbonComboBox* pCombo = new CMFCRibbonComboBox( ID_RIBBON_MODE, FALSE, 100 );

		pCombo->AddItem( "Common" );
		pCombo->AddItem( "Navigtaion Mesh" );
		pCombo->SelectItem( 0 );
		pPanel->Add( pCombo );

		pPanel->Add( new CMFCRibbonSeparator );

		// View Modes
		{	
			CMFCToolBarImages* pImg;
			CMFCToolBarImages* pDisImg;
			CMFCRibbonButtonsGroup* pGroup;
			CMFCRibbonButton * pBtn;

			pImg	= &m_RibbonHome_EnableImg;   //new CMFCToolBarImages;		// 자동삭제가 안되서, 멤버객체로 대체함.
			pDisImg =  &m_RibbonHome_DisableImg; //new CMFCToolBarImages;		// (2012.09.12.수빈)
			pGroup = new CMFCRibbonButtonsGroup;

			pPanel->Add( pGroup);

			pImg->Load( IDB_SELECT_TYPE );
			pImg->SetImageSize( CSize( 32,32 ), TRUE );

			pDisImg->Load( IDB_SELECT_TYPE_DISABLE );
			pDisImg->SetImageSize( CSize( 32,32 ), TRUE );

			pGroup->SetImages( pImg, pImg, pDisImg );			

			for( int i = 0; i < MAX_LEVEL_CONTROL_MODE; ++i )
			{
				pBtn = new CMFCRibbonButton( g_CTRLMODE_BTN_DATA[i].id, g_CTRLMODE_BTN_DATA[i].szName, g_CTRLMODE_BTN_DATA[i].img, g_CTRLMODE_BTN_DATA[i].img );
				pBtn->SetToolTipText( g_CTRLMODE_BTN_DATA[i].szName );
				pGroup->AddButton( pBtn );
			}
		}

		// Gizmo Space Mode
		{
			m_pCB_SpaceMode = new CMFCRibbonComboBox( ID_SPACE_MODE, FALSE, 90, "Space");

			for( INT32 i = 0; i < I3LV_SPACE::SPACE_COUNT; i++)
			{
				I3LV_SPACE::TYPE space = (I3LV_SPACE::TYPE) i;

				m_pCB_SpaceMode->AddItem( I3LV_SPACE::getSpaceString( space), NULL);
			}

			pPanel->Add( m_pCB_SpaceMode);
		}

		pPanel->Add( new CMFCRibbonSeparator);

		pPanel->Add( new CMFCRibbonButton( IDC_BTN_PARENT_LINK,			"Parent Link",			8, 8));
		pPanel->Add( new CMFCRibbonButton( IDC_BTN_PARENT_UNLINK,		"Parent Unlink",		9, 9));

		pPanel->Add( new CMFCRibbonButton( IDC_BTN_COLLISION_EDIT,		"Edit Collision Mesh",	10, 10));

		pPanel->Add( new CMFCRibbonSeparator);

		pPanel->Add( new CMFCRibbonButton( IDC_BTN_ELM_UNDO,			"Undo"));
		pPanel->Add( new CMFCRibbonButton( IDC_BTN_ELM_REDO,			"Redo"));
		pPanel->Add( new CMFCRibbonButton( IDC_BTN_ELM_REDO_CLEAR,		"Clear Undo List"));
	}

	// Tool Panels
	{
		pPanel = pCategory->AddPanel( "Tool", m_PanelImages.ExtractIcon(27));

		pPanel->Add( new CMFCRibbonButton( IDC_BTN_ELM_LIST,			"Element Lists",		4, 4));
		
		pPanel->Add( new CMFCRibbonButton( IDC_BTN_LAYER,				"Layers",		5, 5));

		pPanel->Add( new CMFCRibbonButton( IDC_BTN_ELM_EXPORT,			"Export",		7, 7));

		pPanel->Add( new CMFCRibbonButton( IDC_BTN_OPTION,				"Options",		12, 12));

		pPanel->Add( new CMFCRibbonSeparator);

		pPanel->Add( new CMFCRibbonButton( ID_EDIT_REASSIGN_RES_NAME,	"Reassign Generated Resource Name", -1, -1));
	}


	CMFCRibbonCategory* pCategory2 = m_wndRibbonBar.AddCategory( "Post Process", IDB_BUTTONS, IDB_BUTTONS );

	{
		pPanel = pCategory2->AddPanel("Bloom", m_PanelImages.ExtractIcon(27));

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

	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewerCtrl->getViewer()->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

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


	{
		CMFCRibbonCategory* pCategory3 = m_wndRibbonBar.AddCategory("Hitmap", IDB_BUTTONS, IDB_BUTTONS);
		pPanel = pCategory3->AddPanel("Hit", m_PanelImages.ExtractIcon(27));

		CMFCRibbonButton* pbutton = new CMFCRibbonButton(IDC_BTN_HITMAP_TIMESET, "Make Query", 13, 13);
		pPanel->Add(pbutton);
		
	}
	m_pHitMapMng = new CHitMapManager;
	m_pHitMapMng->Init();
}


void CMainFrame::OnBtnElmSelection()
{
	m_pFramework->SetLevelCtrlMode( LCM_SELECT );
}

void CMainFrame::OnBtnElmTranslation()
{
	m_pFramework->SetLevelCtrlMode( LCM_MOVE );
}

void CMainFrame::OnBtnElmRotation()
{
	m_pFramework->SetLevelCtrlMode( LCM_ROTATE );
}

void CMainFrame::OnBtnElmScale()
{
	m_pFramework->SetLevelCtrlMode( LCM_SCALE );
}

void CMainFrame::OnBtnElmPathedit()
{
	m_pFramework->SetLevelCtrlMode( LCM_PATHEDIT );
}

void CMainFrame::OnBtnElmObjbrush()
{
	m_pFramework->SetLevelCtrlMode( LCM_OBJBRUSH );
}

void CMainFrame::OnBtnElmList()
{
	CDlgElementList dlg;

	dlg.DoModal();
}

void CMainFrame::OnBtnElmExport()
{
	CDlgExport dlg;
	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();
	Ci3LevelDesignDoc * pDoc = (Ci3LevelDesignDoc *) pMainWnd->GetActiveDocument();

	if( pDoc->IsSavedLevel() == false)
	{
		int id = AfxMessageBox( "Export를 하기 위해서는 Level의 이름을 결정하기 위해 저장해야 합니다.\r\n저장하시겠습니까?", MB_YESNO);

		if( id != IDYES)
			return;

		//pDoc->ForceSave();
	}

	dlg.DoModal();
}

void CMainFrame::OnBtnCollisionEdit()
{
	i3LevelFramework * pFramework;
	pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	pFramework->ToggleCollisionEditMode();
}

void CMainFrame::OnBtnLayer()
{
	if( m_DlgLayer.GetStyle() & WS_VISIBLE)
		m_DlgLayer.ShowWindow( SW_HIDE);
	else
		m_DlgLayer.ShowWindow( SW_SHOW);
}

void CMainFrame::OnBtnOption()
{
	COptionDlg dlg;

	dlg.DoModal();
}

void CMainFrame::OnBtnElmUndo()
{
	i3LevelUndo::Undo();
}

void CMainFrame::OnBtnElmRedo()
{
	i3LevelUndo::Redo();
}

void CMainFrame::OnBtnEditEscape()
{
	/*i3LevelFramework * pFramework;
	pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	if( NULL == pFramework )
	{
		return;
	}

	switch( pFramework->GetLevelEditMode() )
	{
	case	LEM_NORMAL:
			i3Level::GetScene()->UnselectAll();
			break;

	case	LEM_NAVMESH_POINT:
	case	LEM_NAVMESH_POLYGON:
			{
				if( i3Level::GetNavMesh() )
					i3Level::GetNavMesh()->ClearSelectList();
			}
			break;

	case	LEM_NAVMESH_POINT_ADD:
			{
				theApp.SetLevelEditMode( LEM_NAVMESH_POINT );
			}
			break;
	}*/
}

void CMainFrame::OnBtnElmUndoListClear()
{
	i3LevelUndo::ResetSystem();
}

void CMainFrame::OnEditReassignResName()
{
	i3LevelFramework * pFramework;
	pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	pFramework->ReassignGeneratedResName();
}

void CMainFrame::OnBtnChangeMode()
{
	CMFCRibbonComboBox* pcombo = (CMFCRibbonComboBox*)m_wndRibbonBar.FindByID( ID_RIBBON_MODE );

	if( pcombo )
	{
		LEVEL_EDIT_MODE mode = (LEVEL_EDIT_MODE) pcombo->GetCurSel();

		i3LevelScene* pScene = i3Level::GetScene();

		switch( mode )
		{
		case	LEM_NORMAL:
				pScene->SetGridShow( true );
				break;

		case	LEM_NAVMESH_POINT:
		case	LEM_NAVMESH_POLYGON:
				pScene->SetGridShow( false );
				break;
		}		

		theApp.SetLevelEditMode( mode );
	}
}

void	CMainFrame::OnUpdateRibbonCtrlMode( CCmdUI* pCmdUI )
{
	LEVEL_EDIT_MODE		editMode = m_pFramework->GetLevelEditMode();
	LEVEL_CONTROL_MODE	ctrlMode = m_pFramework->GetLevelCtrlMode();


	BOOL bEnable	= FALSE;
	BOOL bCheck		= g_CTRLMODE_BTN_DATA[ ctrlMode ].id == pCmdUI->m_nID;

	switch( editMode )
	{
	case	LEM_NORMAL:
			bEnable = TRUE;
			break;

	case	LEM_NAVMESH_POINT:
	case	LEM_NAVMESH_POLYGON:	
			bEnable =	(g_CTRLMODE_BTN_DATA[ LCM_SELECT ].id == pCmdUI->m_nID) ||
						(g_CTRLMODE_BTN_DATA[ LCM_MOVE ].id == pCmdUI->m_nID) ;
			break;
	}

	pCmdUI->SetCheck( bCheck );
	pCmdUI->Enable( bEnable );
}

void	CMainFrame::OnUpdateRibbonUndo( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( 0 < i3LevelUndo::GetUndoCount() );
}

void	CMainFrame::OnUpdateRibbonRedo( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( 0 < i3LevelUndo::GetRedoCount() );
}

void	CMainFrame::OnUpdateRibbonUndoListClear( CCmdUI* pCmdUI )
{
	BOOL bEnable = 0 < i3LevelUndo::GetRedoCount() || 0 < i3LevelUndo::GetUndoCount();
	pCmdUI->Enable( bEnable );
}

void CMainFrame::OnSpaceMode()
{
	i3LevelFramework * pFramework;
	pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	switch( m_pCB_SpaceMode->GetCurSel())
	{
		case 0 :	pFramework->setSpaceMode( I3LV_SPACE::WORLD);	break;
		case 1 :	pFramework->setSpaceMode( I3LV_SPACE::LOCAL);	break;
	}
}

void CMainFrame::OnUpdate_SpaceMode( CCmdUI * pCmd)
{
	i3LevelFramework * pFramework;
	pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	if( pCmd->m_pOther == NULL)
		return;

	pCmd->Enable();

	switch( pFramework->getSpaceMode())
	{
		case I3LV_SPACE::WORLD :		m_pCB_SpaceMode->SelectItem( 0);	break;
		case I3LV_SPACE::LOCAL :		m_pCB_SpaceMode->SelectItem( 1);	break;
		default :						m_pCB_SpaceMode->SelectItem( -1);	break;
	}
}

void CMainFrame::OnChanged_BloomIntensity()
{
	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewerCtrl->getViewer()->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

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

	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewerCtrl->getViewer()->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

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
	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewerCtrl->getViewer()->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

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

	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewerCtrl->getViewer()->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

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
	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewerCtrl->getViewer()->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());
	if(pBloom != NULL)
	{
		bool bVisible = !pBloom->IsEnable();
		pBloom->SetEnable(bVisible);
	}
}

void CMainFrame::OnUpdate_BloomEnable(CCmdUI * pCmdUI)
{
	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewerCtrl->getViewer()->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());
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

	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewerCtrl->getViewer()->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());
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

	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewerCtrl->getViewer()->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

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
	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewerCtrl->getViewer()->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

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

	i3PostProcessBloom * pBloom = (i3PostProcessBloom *)g_pViewerCtrl->getViewer()->GetPostProcessManager()->FindPostProcess(i3PostProcessBloom::static_meta());

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

void CMainFrame::OnBtn_LoadHitMap()
{
	CDlgHitMapTimeSet dlg;
	dlg.DoModal();
	if (dlg.GetState())
	{
		m_pHitMapMng->Build(dlg.m_tmStart, dlg.m_tmEnd);
		m_PanelHitMap.ResetRoomList();
	}
}