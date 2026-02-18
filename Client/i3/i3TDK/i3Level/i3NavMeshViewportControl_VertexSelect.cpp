#include "stdafx.h"
#include "i3NavMeshViewportControl_VertexSelect.h"

#include "../i3TDKGlobalVariable.h"

#include "i3LevelGlobalVariable.h"
#include "i3LevelViewport.h"

#include "i3NavMeshDataMgr.h"
#include "i3NavMesh.h"

I3_CLASS_INSTANCE( i3NavMeshViewportControl_VertexSelect); //, i3LevelViewportControl );

void	i3NavMeshViewportControl_VertexSelect::DrawRect( HWND hWnd, const CPoint& pt1, const CPoint& pt2 )
{
	HDC dc;
	CRect rect;

	dc = ::GetDC( hWnd);

	rect.left		= MIN( pt1.x, pt2.x );
	rect.top		= MIN( pt1.y, pt2.y );
	rect.right		= MAX( pt1.x, pt2.x );
	rect.bottom		= MAX( pt1.y, pt2.y );

	::DrawFocusRect( dc, &rect );
	::ReleaseDC( hWnd, dc );
}

void	i3NavMeshViewportControl_VertexSelect::Create()
{
	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_BANDSELECT ) );
}

bool	i3NavMeshViewportControl_VertexSelect::OnStart( I3_LEVEL_EDIT_CONTEXT * pCtx)
{
	return true;
}

bool	i3NavMeshViewportControl_VertexSelect::OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point )
{	
	pCtx->m_OldDragPt = point;
	pCtx->m_pFramework->PendingUpdate();
	pCtx->m_pFramework->SaveMatrix();

	return true;
}

bool	i3NavMeshViewportControl_VertexSelect::OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point )
{
	pCtx->m_pFramework->RestoreUpdate();

	bool isSelBand = false;

	{
		INT32 ddx, ddy;

		ddx = pCtx->m_DragStartPt.x - point.x;	ddx *= ddx;
		ddy = pCtx->m_DragStartPt.y - point.y;	ddy *= ddy;

		isSelBand = (9 < ddx) && (9 < ddy);
	}

	i3LevelScene * pScene = i3Level::GetScene();

	if( nullptr != pScene )
	{
		SELECT_NAVMESH_TYPE mode = SEL_NAVMESH_EXCLUSIVE;

		if( GetKeyState( VK_LCONTROL) & 0x8000)
		{
			mode = SEL_NAVMESH_INCLUSIVE;
		}
		else if( GetKeyState( VK_SHIFT) & 0x8000)
		{
			mode = SEL_NAVMESH_ALTERNATIVE;
		}
		else if( GetKeyState( VK_LMENU) & 0x8000)
		{
			mode = SEL_NAVMESH_UNSELECT;
		}

		i3NavMesh* pNav = pScene->GetNavMeshRoot();

		if( nullptr != pNav )
		{
			if( false == isSelBand )
			{				
				pNav->SelectVertex( point, mode );
			}
			else
			{
				pNav->SelectBandVertex( pCtx->m_DragStartPt, point, mode );
			}
		}
	}	

	pCtx->m_pViewport->Invalidate();	
	pCtx->m_pFramework->OnNavSelect();
	return true;
}

bool	i3NavMeshViewportControl_VertexSelect::OnRButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point )
{
	return true;
}

bool	i3NavMeshViewportControl_VertexSelect::OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point )
{
	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->UpdateMouseMove( point, true, false );

	if( true == pCtx->m_bDragging )
	{
		DrawRect( pCtx->m_pViewport->m_hWnd, pCtx->m_DragStartPt, pCtx->m_OldDragPt );

		pCtx->m_OldDragPt = point;

		DrawRect( pCtx->m_pViewport->m_hWnd, pCtx->m_DragStartPt, point );
	}
	
	return true;
}