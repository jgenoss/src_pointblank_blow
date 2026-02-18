#include "stdafx.h"
#include "i3LevelViewportControlRespawnCreate.h"
#include "i3LevelViewport.h"
#include "i3LevelFramework.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelStaticObject.h"

I3_CLASS_INSTANCE( i3LevelViewportControlRespawnCreate);

void	i3LevelViewportControlRespawnCreate::GetPlacePos( I3_LEVEL_EDIT_CONTEXT * pCtx, CPoint point, VEC3D * pDest)
{
	VEC3D pos1, pos2, norm, origin;
	REAL32 t;
	i3LevelViewport * pViewport = pCtx->m_pViewport;
	i3LevelFramework * pFramework = pCtx->m_pFramework;
	i3Viewer *	pViewer = pViewport->getViewer();

	{
		MATRIX tmx;
		REAL32 dx, dy;

		pFramework->GetViewProjectionMatrix( &tmx);
		i3Matrix::Inverse( &tmx, nullptr, &tmx);

		dx = ((REAL32)point.x / pViewer->GetViewWidth() * 2.0f) - 1.0f;
		dy = ((REAL32)point.y / pViewer->GetViewHeight() * 2.0f) - 1.0f;

		i3Vector::Set( &pos1, dx, -dy, 0.0f);
		i3Vector::Set( &pos2, dx, -dy, 1.0f);

		i3Vector::TransformCoord( &pos1, &pos1, &tmx);
		i3Vector::TransformCoord( &pos2, &pos2, &tmx);

		i3Vector::Set( &norm, 0.0f, 1.0f, 0.0f);
		i3Vector::Set( &origin, 0.0f, 0.0f, 0.0f);

		IntersectTriLine( &pos1, &pos2, &norm, &origin, pDest, &t);
	}
}

bool i3LevelViewportControlRespawnCreate::OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnLButtonDown( pCtx, nFlags, point );

	VEC3D pos;
//	char szName[ MAX_PATH];

	GetPlacePos( pCtx, point, &pos);

	/*
	if( !m_bCreated )
	{
		// 새로운 Path를 생성한다.
		m_pRespawnList = i3LevelRespawn::new_object_ref();
		
		g_pScene->GetUniqueElementName( "Respawn", szName);
		m_pRespawnList->SetName( szName);

		g_pScene->AddElement( m_pRespawnList);

		m_pRespawnList->AddPoint( &pos, I3_LEVEL_RESPAWN_TYPE_TEAM1 );		// 첫 점은 무조건 생성한다.
		m_bCreated	= TRUE;
	}

	m_pRespawnList->AddPoint( &pos, I3_LEVEL_RESPAWN_TYPE_TEAM1 );
	*/

	return false;
}

bool i3LevelViewportControlRespawnCreate::OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnLButtonUp( pCtx, nFlags, point );

	i3LevelViewport * pViewport = pCtx->m_pViewport;

	i3Level::Update( pViewport->m_hWnd, I3_TDK_UPDATE_MOVE, nullptr, i3LevelElement3D::static_meta());

	return false;
}

bool i3LevelViewportControlRespawnCreate::OnRButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnRButtonDown( pCtx, nFlags, point );

	i3LevelViewport * pViewport = pCtx->m_pViewport;

	pViewport->Invalidate();

	return true;
}

bool i3LevelViewportControlRespawnCreate::OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnMouseMove( pCtx, nFlags, point );

	i3LevelViewport * pViewport = pCtx->m_pViewport;

	/*
	if( m_pRespawnList != nullptr)
	{
		// 가장 마지막 Point를 Move 시킨다.
		VEC3D pos;

		GetPlacePos( pCtx, point, &pos);
	
		m_pRespawnList->SetPointPos( m_pRespawnList->getPointCount() - 1, &pos);
	}*/

	pViewport->Invalidate();

	return false;
}
