#include "pch.h"
#include "ViewportControlObjectMove.h"
#include "../Gizmo/GizmoMove.h"

I3_CLASS_INSTANCE( CViewportControlObjectMove, CViewportControl);

CViewportControlObjectMove::CViewportControlObjectMove(void)
{
}

CViewportControlObjectMove::~CViewportControlObjectMove(void)
{
}

void CViewportControlObjectMove::Create(void)
{
	m_pGizmo = CGizmoMove::NewObject();
	m_pGizmo->Create();

//	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_MOVE));
}

void CViewportControlObjectMove::OnLButtonDown( UINT nFlags, POINT point)
{
	CViewportControl::OnLButtonDown( nFlags, point );

	//i3LevelFramework * pFramework = pCtx->m_pFramework;

	//pFramework->SaveMatrix();
}

void CViewportControlObjectMove::OnLButtonUp( UINT nFlags, POINT point)
{
	CViewportControl::OnLButtonUp( nFlags, point );
/*
	i3LevelViewport * pViewport = pCtx->m_pViewport;
	i3Level::Update( pViewport->m_hWnd, I3_TDK_UPDATE_MOVE, NULL, i3LevelElement3D::GetClassMeta());

	i3LevelFramework * pFramework = pCtx->m_pFramework;

	i3LevelUndo::ADD_Transform( pFramework->getScene());
*/
}

void CViewportControlObjectMove::OnMouseMove( UINT nFlags, POINT point)
{
	/*
	if( pCtx->m_bDragging)
	{
		i3Level::GetScene()->Moving( pCtx->m_DragStartPt, point, &pCtx->m_GizmoMask);
	}*/
}

