#include "stdafx.h"
#include "i3LevelViewportControlObjectScale.h"
#include "i3LevelViewport.h"
#include "i3LevelFramework.h"
#include "i3LevelGlobalVariable.h"
#include "../i3TDKGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelViewportControlObjectScale);

void i3LevelViewportControlObjectScale::Create(void)
{
	m_pAxis = i3LevelAxisScale::new_object();
	m_pAxis->Create();

	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_SCALE));
}

bool i3LevelViewportControlObjectScale::OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelFramework * pFramework = pCtx->m_pFramework;

	i3LevelViewportControl::OnLButtonDown( pCtx, nFlags, point );

	pFramework->SaveMatrix();

	return true;
}

bool i3LevelViewportControlObjectScale::OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnLButtonUp( pCtx, nFlags, point );

	i3LevelViewport * pViewport = pCtx->m_pViewport;
	i3Level::Update( pViewport->m_hWnd, I3_TDK_UPDATE_MOVE, nullptr, i3LevelElement3D::static_meta());

	i3LevelFramework * pFramework = pCtx->m_pFramework;

	i3LevelUndo::ADD_Transform( pFramework->getScene());

	return false;
}

bool i3LevelViewportControlObjectScale::OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	if( pCtx->m_bDragging)
	{
		bool bSnap = (GetAsyncKeyState( VK_LMENU) & 0x8000) == 0;

		i3LevelFramework * pFramework = pCtx->m_pFramework;

		switch( pCtx->m_idxGrip)
		{
			case -1 :
				i3Level::GetScene()->Scaling( pFramework->getSpaceMode(), pCtx->m_DragStartPt, point, &pCtx->m_AxisMask, bSnap);
				break;

			default :
				i3Level::GetScene()->GripDrag( pCtx->m_idxGrip, pCtx->m_DragStartPt, point, bSnap);
				break;
		}
	}

	return false;
}
