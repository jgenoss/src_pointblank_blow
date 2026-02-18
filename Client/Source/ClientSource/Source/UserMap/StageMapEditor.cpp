#include "pch.h"
#include "StageMapEditor.h"
#include "../GuiNotifyReceiver.h"
#include "ViewportControl/ViewportControl.h"
#include "ViewportControl/ViewportControlObjectMove.h"
#include "ViewportControl/ViewportControlObjectRotate.h"
#include "ViewportControl/ViewportControlObjectScale.h"
#include "ViewportControl/ViewportControlObjectBrush.h"
#include "ViewportControl/ViewportControlObjectSelect.h"
#include "./StageGui/Popup/PopupColorSet.h"
#include "./StageGui/Popup/PopupSelectTexture.h"

I3_CLASS_INSTANCE( CStageMapEditor, i3Stage);

CStageMapEditor::CStageMapEditor(void)
{
	m_pMap		= NULL;
	m_pCamera	= NULL;
	m_pGuiRoot	= NULL;

	m_pDashboard		= NULL;
	m_pToolbar			= NULL;
	m_pPalette			= NULL;
	m_pPopupColorSet	= NULL;
	m_pPopupSelectTexture = NULL;

	//Control
	i3mem::FillZero( &m_CtrlCtx, sizeof( m_CtrlCtx));
	for( INT32 i = 0; i < VIEWCTRL_COUNT; ++i)
	{
		m_pViewportCtrl[ i] = NULL;
	}
	m_CtrlMode		= VIEWCTRL_MOVE;
	m_pCtrl			= NULL;
}

CStageMapEditor::~CStageMapEditor(void)
{
	I3_SAFE_RELEASE( m_pPalette);
	I3_SAFE_RELEASE( m_pToolbar);
	I3_SAFE_RELEASE( m_pDashboard);
	I3_SAFE_RELEASE( m_pMap);
	I3_SAFE_RELEASE( m_pCamera);
	I3_SAFE_RELEASE( m_pPopupColorSet);
	I3_SAFE_RELEASE( m_pPopupSelectTexture);

	for( INT32 i = 0; i < VIEWCTRL_COUNT; ++i)
	{
		I3_SAFE_RELEASE( m_pViewportCtrl[ i]);
	}
	m_pCtrl	= NULL;

	if (m_pGuiRoot)
		m_pGuiRoot->getSceneGraphNode()->RemoveFromParent();

	m_pGuiRoot = NULL;
}

void CStageMapEditor::_InitViewportCtrl()
{
	m_pViewportCtrl[ VIEWCTRL_MOVE]		= CViewportControlObjectMove::NewObject();
	m_pViewportCtrl[ VIEWCTRL_MOVE]->Create( this);

	m_pViewportCtrl[ VIEWCTRL_ROTATE]	= CViewportControlObjectRotate::NewObject();
	m_pViewportCtrl[ VIEWCTRL_ROTATE]->Create( this);

	m_pViewportCtrl[ VIEWCTRL_SCALE]	= CViewportControlObjectScale::NewObject();
	m_pViewportCtrl[ VIEWCTRL_SCALE]->Create( this);

	m_pViewportCtrl[ VIEWCTRL_PLACEOBJECT]	= CViewportControlObjectBrush::NewObject();
	m_pViewportCtrl[ VIEWCTRL_PLACEOBJECT]->Create( this);

	m_pViewportCtrl[ VIEWCTRL_SELECT] = CViewportControlObjectSelect::NewObject();
	m_pViewportCtrl[ VIEWCTRL_SELECT]->Create( this);

	SetViewportMode( VIEWCTRL_SELECT);
}

void CStageMapEditor::SetViewportMode( VIEWCTRL_MODE mode)
{
	I3ASSERT( mode >= VIEWCTRL_MOVE && mode < VIEWCTRL_COUNT);

	if( m_CtrlMode == mode)	return;

	m_CtrlMode = mode;

	if( m_pCtrl != NULL)
	{
		m_pCtrl->DetachGizmo();
	}

	m_pCtrl = m_pViewportCtrl[ mode];

	if( m_pCtrl != NULL)
	{
		m_pCtrl->AttachGizmo();
	}
}

void CStageMapEditor::StartPlaceObject( USMAP::BLOCK::ID id)
{
	SetViewportMode( VIEWCTRL_PLACEOBJECT);

	CViewportControlObjectBrush * pCtrl = (CViewportControlObjectBrush *) m_pCtrl;

	pCtrl->Start( id);
}

void CStageMapEditor::CancelPlaceObject(void)
{
	CViewportControlObjectBrush * pCtrl = (CViewportControlObjectBrush *) m_pCtrl;

	pCtrl->Cancel();

	SetViewportMode( VIEWCTRL_MOVE);
}

void CStageMapEditor::_InitGUI(void)
{
	{
		i3UI::RegisterDefaultMessageBox( I3UI_MSGBOX_OK, "MapEditor/MsgBox.i3UI");
	}

	/////////////////////////////
	// Dashboard
	{
		m_pDashboard = CPanel_Dashboard::NewObject();

		m_pDashboard->Create( this);
	}

	//////////////////////////////
	// Toolbar
	{
		m_pToolbar = CPanel_Toolbar::NewObject();
		m_pToolbar->Create( this);
	}

	////////////////////////////////
	// Palette
	{
		m_pPalette = CPanel_Palette::NewObject();
		m_pPalette->Create( this);
	}

	/*
	m_pPopupColorSet = CPopupColorSet::NewObject();
	m_pPopupColorSet->CreateColorSet( this, m_pGuiRoot);

	m_pPopupSelectTexture = CPopupSelectTexture::NewObject();
	m_pPopupSelectTexture->CreateSelectTexture( this, m_pGuiRoot);
	*/
}


void CStageMapEditor::PopupPalette( REAL32 x, REAL32 y)
{
	m_pPalette->Popup( x, y);
}

void CStageMapEditor::OnCreate(void)
{
	{
		m_pMap = CUserMap::NewObject();

		i3Node * pAttachNode = m_pFramework->GetLayerRoot( "WorldRoot");

		m_pMap->Create( pAttachNode, TRUE);
	}

	{
		m_pCamera = CMapEditorCamera::NewObject();

		i3Camera * pCamera = m_pFramework->GetDefaultCamera();

		m_pCamera->InitCamera( pCamera);
	}

	{
		// gui 파일 읽기
		if( i3UI::LoadUI("MapEditor/MapEditor.i3UI") == false)
		{
			I3FATAL( "Could not load GUI file.");
			return;
		}

		// GuiRoot를 찾아 초기화한다
		//g_pGUIRoot->ADDControlByGUI( "GUI/Project/ColorSet.gui");
	}

	_InitGUI();
	_InitViewportCtrl();

	SetViewportMode( VIEWCTRL_MOVE);
}

BOOL CStageMapEditor::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	if( event == I3_EVT_GUI_NOTIFY)
	{
		I3GUI_CONTROL_NOTIFY * pInfo = (I3GUI_CONTROL_NOTIFY *) param2;

		if( pInfo->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
	
		}
	}

	return i3Stage::OnEvent( event, pObj, param2, code);
}

void CStageMapEditor::OnInput(void)
{
#if defined( I3_DEBUG)
	i3InputDeviceManager * pManager = g_pViewer->GetInputDeviceManager();
	i3InputKeyboard * pKey = pManager->GetKeyboard();
	if( pKey->GetStrokeState( I3I_KEY_B))
	{
		m_pMap->addObject( USMAP::BLOCK::ID_CUBE);
	}
#endif

	if( m_pCamera != NULL)
	{
		m_pCamera->OnInput();
	}

	i3Stage::OnInput();
}

void CStageMapEditor::OnSelectObject( CUserMap_Object * pObj)
{
	SetViewportMode( VIEWCTRL_MOVE);
}

#if defined( I3_WINDOWS)
void CStageMapEditor::OnLButtonDown( UINT32 nFlag, POINT point)
{
	CUserMap_Object * pPicked = PickObject( point);
	if( pPicked != NULL)
	{
		OnSelectObject( pPicked);
	}

	if( m_pCtrl != NULL)	m_pCtrl->OnLButtonDown( nFlag, point);
	if( m_pCamera != NULL)	m_pCamera->OnLButtonDown( nFlag, point);
}

void CStageMapEditor::OnLButtonUp( UINT32 nFlag, POINT point)
{
	if( m_pCtrl != NULL)	m_pCtrl->OnLButtonUp( nFlag, point);
	if( m_pCamera != NULL)	m_pCamera->OnLButtonUp( nFlag, point);
}

void CStageMapEditor::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
}

void CStageMapEditor::OnMButtonDown( UINT32 nFlag, POINT point)
{
	if( m_pCtrl != NULL)	m_pCtrl->OnMButtonDown( nFlag, point);
	if( m_pCamera != NULL)	m_pCamera->OnMButtonDown( nFlag, point);
}

void CStageMapEditor::OnMButtonUp( UINT32 nFlag, POINT point)
{
	if( m_pCtrl != NULL)	m_pCtrl->OnMButtonUp( nFlag, point);
	if( m_pCamera != NULL)	m_pCamera->OnMButtonUp( nFlag, point);
}

void CStageMapEditor::OnRButtonDown( UINT32 nFlag, POINT point)
{
	if( m_pCtrl != NULL)	m_pCtrl->OnRButtonDown( nFlag, point);
	if( m_pCamera != NULL)	m_pCamera->OnRButtonDown( nFlag, point);
}

void CStageMapEditor::OnRButtonUp( UINT32 nFlag, POINT point)
{
	if( m_pCtrl != NULL)	m_pCtrl->OnRButtonUp( nFlag, point);
	if( m_pCamera != NULL)	m_pCamera->OnRButtonUp( nFlag, point);
}

void CStageMapEditor::OnMouseMove( UINT32 nFlags, POINT point)
{
	if( m_pCtrl != NULL)	m_pCtrl->OnMouseMove( nFlags, point);
	if( m_pCamera != NULL)	m_pCamera->OnMouseMove( nFlags, point);
}

void CStageMapEditor::OnMouseWheel( UINT nFlag, short zDelta, POINT point)
{
	if( m_pCtrl != NULL)	m_pCtrl->OnMouseWheel( nFlag, zDelta, point);
	if( m_pCamera != NULL)	m_pCamera->OnMouseWheel( nFlag, zDelta, point);
}

void CStageMapEditor::OnKeyDown( UINT32 nKey)
{
	if( m_pCtrl != NULL)	m_pCtrl->OnKeyDown( nKey);
	if( m_pCamera != NULL)	m_pCamera->OnKeyDown( nKey);
}

void CStageMapEditor::OnKeyUp( UINT32 nKey)
{
	if( m_pCtrl != NULL)	m_pCtrl->OnKeyUp( nKey);
	if( m_pCamera != NULL)	m_pCamera->OnKeyUp( nKey);

	switch( nKey)
	{
		case I3I_KEY_Q:	SetViewportMode( VIEWCTRL_SELECT);	break;
		case I3I_KEY_W:	SetViewportMode( VIEWCTRL_MOVE);	break;
		case I3I_KEY_E:	SetViewportMode( VIEWCTRL_ROTATE);	break;
		case I3I_KEY_R:	SetViewportMode( VIEWCTRL_SCALE);	break;
	}
}
#endif

void CStageMapEditor::OnChangeVideo( i3VideoInfo * pInfo)
{
	if( m_pCamera != NULL)
	{
		m_pCamera->OnChangeVideo( pInfo);
	}
}

void CStageMapEditor::GetViewProjectionMatrix(MATRIX * pViewProj)
{
	i3Camera * pCam = m_pCamera->getCamera();

	i3Matrix::Mul( pViewProj, pCam->GetViewMatrixAttr()->GetMatrix(), pCam->GetProjectionMatrix());
}

void CStageMapEditor::ScreenToWorld( POINT pos, VEC3D * pStart, VEC3D * pEnd)
{
	MATRIX mat, matInvViewProj;
	VEC3D vec1, vec2;
	REAL32 dx, dy;
	INT32 w, h;

	w = g_pViewer->GetViewWidth();
	h = g_pViewer->GetViewHeight();

	GetViewProjectionMatrix( &mat);
	i3Matrix::Inverse( &matInvViewProj, NULL, &mat);

	// Screen 좌표를 0.0~1.0 범위의 좌표로 Normalize한다.
	dx = ((REAL32)pos.x / w * 2.0f) - 1.0f;
	dy = ((REAL32)pos.y / h * 2.0f) - 1.0f;

	i3Vector::Set( &vec1, dx, -dy, 0.0f);
	i3Vector::Set( &vec2, dx, -dy, 1.0f);

	i3Vector::TransformCoord( pStart,	&vec1, &matInvViewProj);
	i3Vector::TransformCoord( pEnd,		&vec2, &matInvViewProj);
}

CUserMap_Object * CStageMapEditor::PickObject( POINT point)
{
	if( m_pMap == NULL)
		return NULL;

	VEC3D	vecDir, vecStart;
	ScreenToWorld( point, &vecStart, &vecDir);

	CUserMap_Object * pPicked = m_pMap->GetSelectedObject( &vecStart, &vecDir);

	return pPicked;
}
