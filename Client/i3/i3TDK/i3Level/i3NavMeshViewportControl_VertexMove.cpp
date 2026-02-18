#include "stdafx.h"
#include "i3NavMeshViewportControl_VertexMove.h"
#include "i3LevelViewport.h"
#include "i3LevelFramework.h"
#include "i3LevelGlobalVariable.h"
#include "../i3TDKGlobalVariable.h"

#include "i3NavMeshDataMgr.h"
#include "i3NavMesh.h"

I3_CLASS_INSTANCE( i3NavMeshViewportControl_VertexMove); //, i3LevelViewportControl);

void i3NavMeshViewportControl_VertexMove::Create(void)
{
	m_pAxis = i3LevelAxisMove::new_object();
	m_pAxis->Create();

	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_MOVE));
}

bool	i3NavMeshViewportControl_VertexMove::OnStart( I3_LEVEL_EDIT_CONTEXT * pCtx)
{
	i3LevelFramework* pFramework = pCtx->m_pFramework;
	pFramework->ResizeAxis();

	return true;
}

bool i3NavMeshViewportControl_VertexMove::OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnLButtonDown( pCtx, nFlags, point );

	i3LevelFramework * pFramework = pCtx->m_pFramework;
	pFramework->SaveMatrix();

	return true;
}

bool i3NavMeshViewportControl_VertexMove::OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnLButtonUp( pCtx, nFlags, point );
	
	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->SetVertexApplyPoint();

	pCtx->m_pFramework->ResizeAxis();

	return false;
}

bool i3NavMeshViewportControl_VertexMove::OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	if( pCtx->m_bDragging)
	{
		bool bSnap = (GetAsyncKeyState( VK_LMENU) & 0x8000) == 0;

		i3Level::GetScene()->NavMoving( pCtx->m_DragStartPt, point, &pCtx->m_AxisMask, bSnap);
	}

	return false;
}

