#include "pch.h"
#include "ViewportControlRespawnCreate.h"

I3_CLASS_INSTANCE( CViewportControlRespawnCreate, CViewportControl);

CViewportControlRespawnCreate::CViewportControlRespawnCreate(void)
{
	//m_pRespawnList = NULL;
	m_bCreated	= FALSE;
}

CViewportControlRespawnCreate::~CViewportControlRespawnCreate(void)
{
}

void	CViewportControlRespawnCreate::GetPlacePos( POINT point, VEC3D * pDest)
{
	/*
	VEC3D pos1, pos2, norm, origin;
	REAL32 t;
	i3LevelViewport * pViewport = pCtx->m_pViewport;
	i3LevelFramework * pFramework = pCtx->m_pFramework;
	i3Viewer *	pViewer = pViewport->getViewer();

	{
		MATRIX tmx;
		REAL32 dx, dy;

		pFramework->GetViewProjectionMatrix( &tmx);
		i3Matrix::Inverse( &tmx, NULL, &tmx);

		dx = ((REAL32)point.x / pViewer->GetViewWidth() * 2.0f) - 1.0f;
		dy = ((REAL32)point.y / pViewer->GetViewHeight() * 2.0f) - 1.0f;

		i3Vector::Set( &pos1, dx, -dy, 0.0f);
		i3Vector::Set( &pos2, dx, -dy, 1.0f);

		i3Vector::TransformCoord( &pos1, &pos1, &tmx);
		i3Vector::TransformCoord( &pos2, &pos2, &tmx);

		i3Vector::Set( &norm, 0.0f, 1.0f, 0.0f);
		i3Vector::Set( &origin, 0.0f, 0.0f, 0.0f);

		IntersectTriLine( &pos1, &pos2, &norm, &origin, pDest, &t);
	}*/
}

void CViewportControlRespawnCreate::OnLButtonDown( UINT nFlags, POINT point)
{
	CViewportControl::OnLButtonDown( nFlags, point );

	VEC3D pos;
//	char szName[ MAX_PATH];

	GetPlacePos( point, &pos);

	/*
	if( !m_bCreated )
	{
		// 새로운 Path를 생성한다.
		m_pRespawnList = i3LevelRespawn::NewObjectRef();
		
		g_pScene->GetUniqueElementName( "Respawn", szName);
		m_pRespawnList->SetName( szName);

		g_pScene->AddElement( m_pRespawnList);

		m_pRespawnList->AddPoint( &pos, I3_LEVEL_RESPAWN_TYPE_TEAM1 );		// 첫 점은 무조건 생성한다.
		m_bCreated	= TRUE;
	}

	m_pRespawnList->AddPoint( &pos, I3_LEVEL_RESPAWN_TYPE_TEAM1 );
	*/
}

void CViewportControlRespawnCreate::OnLButtonUp( UINT nFlags, POINT point)
{
	CViewportControl::OnLButtonUp( nFlags, point );
/*
	i3LevelViewport * pViewport = pCtx->m_pViewport;

	i3Level::Update( pViewport->m_hWnd, I3_TDK_UPDATE_MOVE, NULL, i3LevelElement3D::GetClassMeta());
	*/
}

void CViewportControlRespawnCreate::OnRButtonDown( UINT nFlags, POINT point)
{
	CViewportControl::OnRButtonDown( nFlags, point );
/*
	i3LevelViewport * pViewport = pCtx->m_pViewport;

	pViewport->Invalidate();*/
}

void CViewportControlRespawnCreate::OnMouseMove( UINT nFlags, POINT point)
{
	CViewportControl::OnMouseMove( nFlags, point );

//	i3LevelViewport * pViewport = pCtx->m_pViewport;

	/*
	if( m_pRespawnList != NULL)
	{
		// 가장 마지막 Point를 Move 시킨다.
		VEC3D pos;

		GetPlacePos( pCtx, point, &pos);
	
		m_pRespawnList->SetPointPos( m_pRespawnList->getPointCount() - 1, &pos);
	}*/

//	pViewport->Invalidate();
}
