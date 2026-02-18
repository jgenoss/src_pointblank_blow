#include "pch.h"
#include "ViewportControlObjectRotate.h"
#include "../Gizmo/GizmoRotate.h"

I3_CLASS_INSTANCE( CViewportControlObjectRotate, CViewportControl);

CViewportControlObjectRotate::CViewportControlObjectRotate(void)
{
}

CViewportControlObjectRotate::~CViewportControlObjectRotate(void)
{
}

void CViewportControlObjectRotate::Create(void)
{
	m_pGizmo = CGizmoRotate::NewObject();
	m_pGizmo->Create();

//	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_ROTATE));
}

void CViewportControlObjectRotate::OnLButtonDown( UINT nFlags, POINT point)
{
	CViewportControl::OnLButtonDown( nFlags, point );

	//i3LevelFramework * pFramework = pCtx->m_pFramework;
	//pFramework->SaveMatrix();
}

void CViewportControlObjectRotate::OnLButtonUp( UINT nFlags, POINT point)
{
	CViewportControl::OnLButtonUp( nFlags, point );
/*
	i3LevelViewport * pViewport = pCtx->m_pViewport;
	i3Level::Update( pViewport->m_hWnd, I3_TDK_UPDATE_MOVE, NULL, i3LevelElement3D::GetClassMeta());

	i3LevelFramework * pFramework = pCtx->m_pFramework;

	i3LevelUndo::ADD_Transform( pFramework->getScene());
*/
}

void CViewportControlObjectRotate::OnMouseMove( UINT nFlags, POINT point)
{
	/*
	i3LevelFramework * pFramework = pCtx->m_pFramework;

	if( pCtx->m_bDragging)
	{
		// World-Space의 Point로 변환
		MATRIX mtx, inv;
		i3LevelViewport * pView;

		pView = i3LevelViewport::GetCurrentFocusViewport();

		pFramework->GetViewProjectionMatrix( &mtx);
		i3Matrix::Inverse( &inv, NULL, &mtx);

		VEC3D s, e;
		REAL32 rw, rh, hw, hh;

		hw = (REAL32)(pView->GetViewWidth() >> 1);
		hh = (REAL32)(pView->GetViewHeight() >> 1);

		rw = 1.0f / hw;
		rh = -1.0f / hh;

		// Clipping Space에서의 시작점과 끝점을 설정한다.
		i3Vector::Set( &s, (pCtx->m_DragStartPt.x - hw) * rw, (pCtx->m_DragStartPt.y - hh) * rh, 0.0f);
		i3Vector::Set( &e, (point.x - hw) * rw, (point.y - hh) * rh, 0.0f);

		// World-Space에서의 시작점 끝점.
		i3Vector::TransformCoord( &s, &s, &inv);
		i3Vector::TransformCoord( &e, &e, &inv);

		//i3Level::GetScene()->Rotating( &s, &e, &pCtx->m_GizmoMask);

		CGizmoRotate * pGizmo = (CGizmoRotate *) getGizmo();

		VEC3D diff, Gizmo;
		i3Vector::Sub( &diff, pGizmo->getPos(), pGizmo->getPickedPos());

		i3Vector::Normalize( &diff, &diff);

		// 현재 보는 View에서 회전축에 맞추어 Gizmo가 회전하는 Moment Vector를 구한다.
		i3Vector::Cross( &Gizmo, &diff, pFramework->GetGizmoMask());
		i3Vector::Normalize( &Gizmo, &Gizmo);

		// Dragging Vector를 Gizmo Vector 방향에서의 길이로 변환한다.
		i3Vector::Sub( &diff, &e, &s);

		REAL32 rot = i3Vector::Dot( &Gizmo, &diff) * 10.0f;

		VEC3D rotVec;
		i3Vector::Scale( &rotVec, pFramework->GetGizmoMask(), rot);

		g_pScene->Rotating( rotVec.x, rotVec.y, rotVec.z);

		pFramework->OnSelect( NULL );
	}*/
}
