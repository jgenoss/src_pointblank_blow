#include "stdafx.h"
#include "i3LevelViewportControlObjectMove.h"
#include "i3LevelViewport.h"
#include "i3LevelFramework.h"
#include "i3LevelGlobalVariable.h"
#include "../i3TDKGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelViewportControlObjectMove);

void i3LevelViewportControlObjectMove::Create(void)
{
	m_pAxis = i3LevelAxisMove::new_object();
	m_pAxis->Create();

	m_hCursor = ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_LV_MOVE));
}

bool i3LevelViewportControlObjectMove::OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnLButtonDown( pCtx, nFlags, point );

	i3LevelFramework * pFramework = pCtx->m_pFramework;

	if( nFlags & MK_SHIFT)
	{
		// Copy & Paste 동작으로 간주.
		// 현재 선택된 Object들의 Clone을 만들고 Dragging을 다시 시작해야 한다.
		pFramework->Copy();
		pFramework->Paste( false);
	}

	pFramework->SaveMatrix();

	return true;
}

bool i3LevelViewportControlObjectMove::OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	i3LevelViewportControl::OnLButtonUp( pCtx, nFlags, point );

	i3LevelViewport * pViewport = pCtx->m_pViewport;
	i3Level::Update( pViewport->m_hWnd, I3_TDK_UPDATE_MOVE, nullptr, i3LevelElement3D::static_meta());

	i3LevelFramework * pFramework = pCtx->m_pFramework;

	i3LevelUndo::ADD_Transform( pFramework->getScene());

	return false;
}

bool i3LevelViewportControlObjectMove::OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	if( pCtx->m_bDragging)
	{
		i3LevelFramework * pFramework = pCtx->m_pFramework;

		bool bSnap = (GetAsyncKeyState( VK_LMENU) & 0x8000) == 0;

		i3Level::GetScene()->Moving( pFramework->getSpaceMode(), pCtx->m_DragStartPt, point, &pCtx->m_AxisMask, bSnap);
	}

	return false;
}

