#include "pch.h"
#include "ViewportControlObjectBrush.h"
#include "../UserMap.h"
#include "../StageMapEditor.h"

I3_CLASS_INSTANCE( CViewportControlObjectBrush, CViewportControl);

CViewportControlObjectBrush::CViewportControlObjectBrush(void)
{
	m_pPlaceObject = NULL;
}

CViewportControlObjectBrush::~CViewportControlObjectBrush(void)
{
	I3_SAFE_RELEASE( m_pPlaceObject);
}

void CViewportControlObjectBrush::Start( USMAP::BLOCK::ID id)
{
	CUserMap_Object * pNewObj;

	if( id == USMAP::BLOCK::ID_NONE)
	{
		Cancel();
	}
	else
	{
		pNewObj = m_pMap->addObject( id);

		I3_REF_CHANGE( m_pPlaceObject, pNewObj);

		// 배치 중인 Object는 Raycast가 안되어야 한다.
		pNewObj->SetDynamicState( I3GAME_DYNAMIC_STATE_NONE);
	}
}

void CViewportControlObjectBrush::Cancel(void)
{
	if( m_pPlaceObject != NULL)
	{
		m_pMap->removeObject( m_pPlaceObject);

		m_pPlaceObject->Release();
		m_pPlaceObject = NULL;

		m_pStage->SetViewportMode( VIEWCTRL_MOVE);
	}
}

void CViewportControlObjectBrush::End(void)
{
	if( m_pPlaceObject != NULL)
	{
		m_pPlaceObject->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC);
		m_pPlaceObject->Release();
		m_pPlaceObject = NULL;

		m_pStage->SetViewportMode( VIEWCTRL_MOVE);
	}
}

void CViewportControlObjectBrush::OnLButtonDown( UINT nFlags, POINT point)
{
	VIEWPORT_CONTROL_CONTEXT * pCtx = m_pStage->getViewportCtrlCtx();

	pCtx->m_DragStartPt = point;
}

void CViewportControlObjectBrush::OnLButtonUp( UINT nFlags, POINT point)
{
	VIEWPORT_CONTROL_CONTEXT * pCtx = m_pStage->getViewportCtrlCtx();

	INT32 dx, dy;

	dx = point.x - pCtx->m_DragStartPt.x;
	dy = point.y - pCtx->m_DragStartPt.y;

	if( (abs( dx) > 2) || (abs( dy) > 2))
		return ;

	End();
}


void CViewportControlObjectBrush::OnMouseMove( UINT nFlags, POINT point)
{
	VEC3D vStart, vEnd, pos;
	i3CollideeLine line;

	m_pStage->ScreenToWorld( point, &vStart, &vEnd);

	line.SetStart( &vStart);
	line.SetEnd( &vEnd);

	I3_PHYSIX_HIT_RESULT * pHit = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK, NULL);

	if( pHit == NULL)
	{
		VEC4D plane;
		REAL32 t;

		// 원점 XZ 평면과 직선의 교점을 계산.
		i3Plane::Set( &plane, &I3_ZEROVECTOR, &I3_YAXIS);
		i3Plane::Intersection( &plane, &vStart, &vEnd, &t, &pos);
	}
	else
	{
		i3Vector::Copy( &pos, &pHit->m_Intersect);
	}

	m_pPlaceObject->SetSnappedPos( &pos);
}
