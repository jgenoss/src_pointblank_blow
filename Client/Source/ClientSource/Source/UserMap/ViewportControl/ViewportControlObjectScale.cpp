#include "pch.h"
#include "ViewportControlObjectScale.h"
#include "../Gizmo/GizmoScale.h"

I3_CLASS_INSTANCE( CViewportControlObjectScale, CViewportControl);

CViewportControlObjectScale::CViewportControlObjectScale(void)
{
}

CViewportControlObjectScale::~CViewportControlObjectScale(void)
{
}

void CViewportControlObjectScale::Create(void)
{
	m_pGizmo = CGizmoScale::NewObject();
	m_pGizmo->Create();

//	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_SCALE));
}

void CViewportControlObjectScale::OnLButtonDown( UINT nFlags, POINT point)
{
	/*
	i3LevelFramework * pFramework = pCtx->m_pFramework;

	CViewportControl::OnLButtonDown( pCtx, nFlags, point );

	pFramework->SaveMatrix();
	*/
}

void CViewportControlObjectScale::OnLButtonUp( UINT nFlags, POINT point)
{
	CViewportControl::OnLButtonUp( nFlags, point );
/*
	i3LevelViewport * pViewport = pCtx->m_pViewport;
	i3Level::Update( pViewport->m_hWnd, I3_TDK_UPDATE_MOVE, NULL, i3LevelElement3D::GetClassMeta());

	i3LevelFramework * pFramework = pCtx->m_pFramework;

	i3LevelUndo::ADD_Transform( pFramework->getScene());
*/
}

void CViewportControlObjectScale::OnMouseMove( UINT nFlags, POINT point)
{
//	if( pCtx->m_bDragging)
	{
	//	i3Level::GetScene()->Scaling( pCtx->m_DragStartPt, point, &pCtx->m_GizmoMask);
	}
}
